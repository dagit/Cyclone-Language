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
typedef unsigned short Cyc_mode_t;
# 41
double modf(double,double*);struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 161
int Cyc_putc(int,struct Cyc___cycFILE*);
# 174
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 228
int Cyc_vprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 239
int Cyc_vsnprintf(struct _dyneither_ptr,unsigned long,struct _dyneither_ptr,struct _dyneither_ptr);
# 243
int Cyc_vsprintf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 87 "printf.cyc"
static struct _dyneither_ptr Cyc_parg2string(void*x){
void*_tmp0=x;_LL1: {struct Cyc_String_pa_PrintArg_struct*_tmp1=(struct Cyc_String_pa_PrintArg_struct*)_tmp0;if(_tmp1->tag != 0)goto _LL3;}_LL2: {
const char*_tmpD2;return(_tmpD2="string",_tag_dyneither(_tmpD2,sizeof(char),7));}_LL3: {struct Cyc_Int_pa_PrintArg_struct*_tmp2=(struct Cyc_Int_pa_PrintArg_struct*)_tmp0;if(_tmp2->tag != 1)goto _LL5;}_LL4: {
const char*_tmpD3;return(_tmpD3="int",_tag_dyneither(_tmpD3,sizeof(char),4));}_LL5: {struct Cyc_Double_pa_PrintArg_struct*_tmp3=(struct Cyc_Double_pa_PrintArg_struct*)_tmp0;if(_tmp3->tag != 2)goto _LL7;}_LL6: {
# 92
const char*_tmpD4;return(_tmpD4="double",_tag_dyneither(_tmpD4,sizeof(char),7));}_LL7: {struct Cyc_LongDouble_pa_PrintArg_struct*_tmp4=(struct Cyc_LongDouble_pa_PrintArg_struct*)_tmp0;if(_tmp4->tag != 3)goto _LL9;}_LL8: {
const char*_tmpD5;return(_tmpD5="long double",_tag_dyneither(_tmpD5,sizeof(char),12));}_LL9: {struct Cyc_ShortPtr_pa_PrintArg_struct*_tmp5=(struct Cyc_ShortPtr_pa_PrintArg_struct*)_tmp0;if(_tmp5->tag != 4)goto _LLB;}_LLA: {
const char*_tmpD6;return(_tmpD6="short *",_tag_dyneither(_tmpD6,sizeof(char),8));}_LLB: {struct Cyc_IntPtr_pa_PrintArg_struct*_tmp6=(struct Cyc_IntPtr_pa_PrintArg_struct*)_tmp0;if(_tmp6->tag != 5)goto _LL0;}_LLC: {
const char*_tmpD7;return(_tmpD7="unsigned long *",_tag_dyneither(_tmpD7,sizeof(char),16));}_LL0:;}
# 99
static void*Cyc_badarg(struct _dyneither_ptr s){
struct Cyc_Core_Invalid_argument_exn_struct _tmpDA;struct Cyc_Core_Invalid_argument_exn_struct*_tmpD9;return(void*)_throw((void*)((_tmpD9=_cycalloc(sizeof(*_tmpD9)),((_tmpD9[0]=((_tmpDA.tag=Cyc_Core_Invalid_argument,((_tmpDA.f1=s,_tmpDA)))),_tmpD9)))));}
# 104
static int Cyc_va_arg_int(struct _dyneither_ptr ap){
void*_tmpF=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp10=_tmpF;unsigned long _tmp12;_LLE: {struct Cyc_Int_pa_PrintArg_struct*_tmp11=(struct Cyc_Int_pa_PrintArg_struct*)_tmp10;if(_tmp11->tag != 1)goto _LL10;else{_tmp12=_tmp11->f1;}}_LLF:
 return(int)_tmp12;_LL10:;_LL11: {
const char*_tmpDB;return((int(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpDB="printf expected int",_tag_dyneither(_tmpDB,sizeof(char),20))));}_LLD:;}
# 111
static long Cyc_va_arg_long(struct _dyneither_ptr ap){
void*_tmp14=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp15=_tmp14;unsigned long _tmp17;_LL13: {struct Cyc_Int_pa_PrintArg_struct*_tmp16=(struct Cyc_Int_pa_PrintArg_struct*)_tmp15;if(_tmp16->tag != 1)goto _LL15;else{_tmp17=_tmp16->f1;}}_LL14:
 return(long)_tmp17;_LL15:;_LL16: {
const char*_tmpDC;return((long(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpDC="printf expected int",_tag_dyneither(_tmpDC,sizeof(char),20))));}_LL12:;}
# 118
static unsigned long Cyc_va_arg_ulong(struct _dyneither_ptr ap){
void*_tmp19=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp1A=_tmp19;unsigned long _tmp1C;_LL18: {struct Cyc_Int_pa_PrintArg_struct*_tmp1B=(struct Cyc_Int_pa_PrintArg_struct*)_tmp1A;if(_tmp1B->tag != 1)goto _LL1A;else{_tmp1C=_tmp1B->f1;}}_LL19:
 return _tmp1C;_LL1A:;_LL1B: {
const char*_tmpDD;return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpDD="printf expected int",_tag_dyneither(_tmpDD,sizeof(char),20))));}_LL17:;}
# 125
static unsigned long Cyc_va_arg_uint(struct _dyneither_ptr ap){
void*_tmp1E=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp1F=_tmp1E;unsigned long _tmp21;_LL1D: {struct Cyc_Int_pa_PrintArg_struct*_tmp20=(struct Cyc_Int_pa_PrintArg_struct*)_tmp1F;if(_tmp20->tag != 1)goto _LL1F;else{_tmp21=_tmp20->f1;}}_LL1E:
 return _tmp21;_LL1F:;_LL20: {
const char*_tmpDE;return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpDE="printf expected int",_tag_dyneither(_tmpDE,sizeof(char),20))));}_LL1C:;}
# 133
static double Cyc_va_arg_double(struct _dyneither_ptr ap){
void*_tmp23=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp24=_tmp23;double _tmp26;long double _tmp28;_LL22: {struct Cyc_Double_pa_PrintArg_struct*_tmp25=(struct Cyc_Double_pa_PrintArg_struct*)_tmp24;if(_tmp25->tag != 2)goto _LL24;else{_tmp26=_tmp25->f1;}}_LL23:
 return _tmp26;_LL24: {struct Cyc_LongDouble_pa_PrintArg_struct*_tmp27=(struct Cyc_LongDouble_pa_PrintArg_struct*)_tmp24;if(_tmp27->tag != 3)goto _LL26;else{_tmp28=_tmp27->f1;}}_LL25:
 return(double)_tmp28;_LL26:;_LL27: {
# 138
const char*_tmpE2;void*_tmpE1[1];struct Cyc_String_pa_PrintArg_struct _tmpE0;(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)((struct _dyneither_ptr)((_tmpE0.tag=0,((_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_parg2string(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0)))),((_tmpE1[0]=& _tmpE0,Cyc_aprintf(((_tmpE2="printf expected double but found %s",_tag_dyneither(_tmpE2,sizeof(char),36))),_tag_dyneither(_tmpE1,sizeof(void*),1))))))))));}_LL21:;}
# 144
static short*Cyc_va_arg_short_ptr(struct _dyneither_ptr ap){
void*_tmp2C=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp2D=_tmp2C;short*_tmp2F;_LL29: {struct Cyc_ShortPtr_pa_PrintArg_struct*_tmp2E=(struct Cyc_ShortPtr_pa_PrintArg_struct*)_tmp2D;if(_tmp2E->tag != 4)goto _LL2B;else{_tmp2F=_tmp2E->f1;}}_LL2A:
 return _tmp2F;_LL2B:;_LL2C: {
const char*_tmpE3;(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpE3="printf expected short pointer",_tag_dyneither(_tmpE3,sizeof(char),30)))));}_LL28:;}
# 152
static unsigned long*Cyc_va_arg_int_ptr(struct _dyneither_ptr ap){
void*_tmp31=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp32=_tmp31;unsigned long*_tmp34;_LL2E: {struct Cyc_IntPtr_pa_PrintArg_struct*_tmp33=(struct Cyc_IntPtr_pa_PrintArg_struct*)_tmp32;if(_tmp33->tag != 5)goto _LL30;else{_tmp34=_tmp33->f1;}}_LL2F:
 return _tmp34;_LL30:;_LL31: {
const char*_tmpE4;(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpE4="printf expected int pointer",_tag_dyneither(_tmpE4,sizeof(char),28)))));}_LL2D:;}
# 160
static const struct _dyneither_ptr Cyc_va_arg_string(struct _dyneither_ptr ap){
void*_tmp36=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0));void*_tmp37=_tmp36;struct _dyneither_ptr _tmp39;_LL33: {struct Cyc_String_pa_PrintArg_struct*_tmp38=(struct Cyc_String_pa_PrintArg_struct*)_tmp37;if(_tmp38->tag != 0)goto _LL35;else{_tmp39=_tmp38->f1;}}_LL34:
 return _tmp39;_LL35:;_LL36: {
const char*_tmpE5;(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(((_tmpE5="printf expected string",_tag_dyneither(_tmpE5,sizeof(char),23)))));}_LL32:;}
# 177 "printf.cyc"
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr endp);
# 206 "printf.cyc"
enum Cyc_BASE{Cyc_OCT  = 0,Cyc_DEC  = 1,Cyc_HEX  = 2};
typedef enum Cyc_BASE Cyc_base_t;
# 212
inline static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int howmany){
# 214
int i=0;
while(i < howmany){
if(ioputc((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),0)),ioputc_env)== - 1)return i;
_dyneither_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 220
return i;}
# 223
static int Cyc__IO_nzsputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int howmany){
# 225
int i=0;
while(i < howmany){
char c;
if((c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0)))== 0  || ioputc((int)c,ioputc_env)== - 1)return i;
_dyneither_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 232
return i;}
# 238
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){
# 240
int i=0;
while(i < howmany){
if(ioputc((int)c,ioputc_env)== - 1)return i;
++ i;}
# 245
return i;}
# 249
static struct _dyneither_ptr Cyc_my_memchr(struct _dyneither_ptr s,char c,int n){
int sz=(int)_get_dyneither_size(s,sizeof(char));
n=n < sz?n: sz;
for(0;n != 0;(n --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)return s;}
# 255
return _tag_dyneither(0,0,0);}
# 258
static struct _dyneither_ptr Cyc_my_nzmemchr(struct _dyneither_ptr s,char c,int n){
int sz=(int)_get_dyneither_size(s,sizeof(char));
n=n < sz?n: sz;
for(0;n != 0;(n --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)return s;}
# 264
return _tag_dyneither(0,0,0);}
# 267
inline static const unsigned long Cyc_my_strlen(struct _dyneither_ptr s){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
unsigned int i=0;
while(i < sz  && ((const char*)s.curr)[(int)i]!= 0){++ i;}
return i;}
# 278
int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap){
# 282
struct _dyneither_ptr fmt;
register int ch;
register int n;
# 286
struct _dyneither_ptr cp=_tag_dyneither(0,0,0);
# 289
struct _dyneither_ptr cp2=_tag_dyneither(0,0,0);
# 292
struct _dyneither_ptr cp3=_tag_dyneither(0,0,0);
# 295
int which_cp;
struct _dyneither_ptr fmark;
register int flags;
int ret;
int width;
int prec;
char sign;
# 303
char sign_string[2]={'\000','\000'};
int softsign=0;
double _double;
int fpprec;
unsigned long _ulong;
int dprec;
int dpad;
int fieldsz;
# 314
int size=0;
# 316
char buf[349];
{unsigned int _tmp60=348;unsigned int i;for(i=0;i < _tmp60;i ++){buf[i]=(char)'\000';}buf[_tmp60]=(char)0;}{char ox[2]={'\000','\000'};
enum Cyc_BASE base;
# 342 "printf.cyc"
fmt=fmt0;
ret=0;
# 348
for(0;1;0){
# 351
fmark=fmt;{
unsigned int fmt_sz=_get_dyneither_size(fmt,sizeof(char));
for(n=0;(n < fmt_sz  && (ch=(int)((const char*)fmt.curr)[n])!= '\000') && ch != '%';++ n){
;}
fmt=_dyneither_ptr_plus(fmt,sizeof(char),n);
# 357
if((n=(fmt.curr - fmark.curr)/ sizeof(char))!= 0){
do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)fmark,n)!= n)goto error;}while(0);
ret +=n;}
# 361
if(ch == '\000')
goto done;
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);
# 365
flags=0;
dprec=0;
fpprec=0;
width=0;
prec=-1;
sign='\000';
# 372
rflag: ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));
reswitch: which_cp=0;
switch(ch){case ' ': _LL37:
# 381
 if(!((int)sign))
sign=' ';
goto rflag;case '#': _LL38:
# 385
 flags |=8;
goto rflag;case '*': _LL39:
# 394
 width=Cyc_va_arg_int(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
if(width >= 0)
goto rflag;
width=- width;
goto _LL3A;case '-': _LL3A:
# 400
 flags |=16;
flags &=~ 32;
goto rflag;case '+': _LL3B:
# 404
 sign='+';
goto rflag;case '.': _LL3C:
# 407
 if((ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0)))== '*'){
n=Cyc_va_arg_int(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
prec=n < 0?- 1: n;
goto rflag;}
# 412
n=0;
while((unsigned int)(ch - '0')<= 9){
n=10 * n + (ch - '0');
ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}
# 417
prec=n < 0?- 1: n;
goto reswitch;case '0': _LL3D:
# 425
 if(!(flags & 16))
flags |=32;
goto rflag;case '1': _LL3E:
 goto _LL3F;case '2': _LL3F: goto _LL40;case '3': _LL40: goto _LL41;case '4': _LL41: goto _LL42;case '5': _LL42:
 goto _LL43;case '6': _LL43: goto _LL44;case '7': _LL44: goto _LL45;case '8': _LL45: goto _LL46;case '9': _LL46:
 n=0;
do{
n=10 * n + (ch - '0');
ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0));}while((unsigned int)(ch - '0')<= 9);
# 435
width=n;
goto reswitch;case 'L': _LL47:
# 438
 flags |=2;
goto rflag;case 'h': _LL48:
# 441
 flags |=4;
goto rflag;case 'l': _LL49:
# 444
 flags |=1;
goto rflag;case 'c': _LL4A:
# 447
{char*_tmpE6;cp=((_tmpE6=buf,_tag_dyneither(_tmpE6,sizeof(char),_get_zero_arr_size_char((void*)_tmpE6,349))));}
{char _tmpE9;char _tmpE8;struct _dyneither_ptr _tmpE7;(_tmpE7=cp,((_tmpE8=*((char*)_check_dyneither_subscript(_tmpE7,sizeof(char),0)),((_tmpE9=(char)Cyc_va_arg_int(ap),((_get_dyneither_size(_tmpE7,sizeof(char))== 1  && (_tmpE8 == '\000'  && _tmpE9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpE7.curr)=_tmpE9)))))));}_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
size=1;
sign='\000';
break;case 'D': _LL4B:
# 453
 flags |=1;
goto _LL4C;case 'd': _LL4C:
 goto _LL4D;case 'i': _LL4D:
 _ulong=(unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(ap)): Cyc_va_arg_int(ap)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){
_ulong=- _ulong;
sign='-';}
# 461
base=Cyc_DEC;
goto number;case 'e': _LL4E:
 goto _LL4F;case 'E': _LL4F: goto _LL50;case 'f': _LL50: goto _LL51;case 'F': _LL51: goto _LL52;case 'g': _LL52:
 goto _LL53;case 'G': _LL53:
 _double=Cyc_va_arg_double(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
# 470
if(prec > 39){
if(ch != 'g'  && ch != 'G'  || flags & 8)
fpprec=prec - 39;
prec=39;}else{
if(prec == - 1)
prec=6;}
# 482
{char*_tmpEA;cp=((_tmpEA=buf,_tag_dyneither(_tmpEA,sizeof(char),_get_zero_arr_size_char((void*)_tmpEA,349))));}
{char _tmpED;char _tmpEC;struct _dyneither_ptr _tmpEB;(_tmpEB=cp,((_tmpEC=*((char*)_check_dyneither_subscript(_tmpEB,sizeof(char),0)),((_tmpED='\000',((_get_dyneither_size(_tmpEB,sizeof(char))== 1  && (_tmpEC == '\000'  && _tmpED != '\000')?_throw_arraybounds(): 1,*((char*)_tmpEB.curr)=_tmpED)))))));}
{char*_tmpEE;size=Cyc___cvt_double(_double,prec,flags,& softsign,ch,cp,
# 486
_dyneither_ptr_plus(((_tmpEE=buf,_tag_dyneither(_tmpEE,sizeof(char),_get_zero_arr_size_char((void*)_tmpEE,349)))),sizeof(char),(int)(sizeof(buf)- 1)));}
if(softsign)
sign='-';
if(*((char*)_check_dyneither_subscript(cp,sizeof(char),0))== '\000')
_dyneither_ptr_inplace_plus(& cp,sizeof(char),1);
break;case 'n': _LL54:
# 493
 if(flags & 1)
*Cyc_va_arg_int_ptr(ap)=(unsigned long)ret;else{
if(flags & 4)
*Cyc_va_arg_short_ptr(ap)=(short)ret;else{
# 498
*Cyc_va_arg_int_ptr(ap)=(unsigned long)ret;}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 'O': _LL55:
# 502
 flags |=1;
goto _LL56;case 'o': _LL56:
# 505
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=Cyc_OCT;
goto nosign;case 'p': _LL57:
# 517 "printf.cyc"
 _ulong=(unsigned long)Cyc_va_arg_long(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=Cyc_HEX;
flags |=64;
ch=(int)'x';
goto nosign;case 's': _LL58: {
# 523
struct _dyneither_ptr b=Cyc_va_arg_string(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
which_cp=3;cp3=b;
if(prec >= 0){
struct _dyneither_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);
if((char*)p.curr != (char*)(_tag_dyneither(0,0,0)).curr){
size=(p.curr - cp3.curr)/ sizeof(char);
if(size > prec)
size=prec;}else{
# 532
size=prec;}}else{
# 534
size=(int)Cyc_my_strlen(cp3);}
sign='\000';
break;}case 'U': _LL59:
# 538
 flags |=1;
goto _LL5A;case 'u': _LL5A:
# 541
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=Cyc_DEC;
goto nosign;case 'X': _LL5B:
 goto _LL5C;case 'x': _LL5C:
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=Cyc_HEX;
# 548
if(flags & 8  && _ulong != 0)
flags |=64;
# 552
nosign: sign='\000';
# 558
number: if((dprec=prec)>= 0)
flags &=~ 32;
# 566
{char*_tmpEF;cp=_dyneither_ptr_plus(((_tmpEF=buf,_tag_dyneither(_tmpEF,sizeof(char),_get_zero_arr_size_char((void*)_tmpEF,349)))),sizeof(char),(308 + 39)+ 1);}
if(_ulong != 0  || prec != 0){
struct _dyneither_ptr xdigs;
# 574
switch(base){case Cyc_OCT: _LL5E:
# 576
 do{
{char _tmpF2;char _tmpF1;struct _dyneither_ptr _tmpF0;(_tmpF0=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpF1=*((char*)_check_dyneither_subscript(_tmpF0,sizeof(char),0)),((_tmpF2=(char)((_ulong & 7)+ '0'),((_get_dyneither_size(_tmpF0,sizeof(char))== 1  && (_tmpF1 == '\000'  && _tmpF2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpF0.curr)=_tmpF2)))))));}
_ulong >>=3;}while((int)_ulong);
# 581
if(flags & 8  && *((char*)_check_dyneither_subscript(cp,sizeof(char),0))!= '0'){
char _tmpF5;char _tmpF4;struct _dyneither_ptr _tmpF3;(_tmpF3=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpF4=*((char*)_check_dyneither_subscript(_tmpF3,sizeof(char),0)),((_tmpF5='0',((_get_dyneither_size(_tmpF3,sizeof(char))== 1  && (_tmpF4 == '\000'  && _tmpF5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpF3.curr)=_tmpF5)))))));}
break;case Cyc_DEC: _LL5F:
# 587
 while(_ulong >= 10){
{char _tmpF8;char _tmpF7;struct _dyneither_ptr _tmpF6;(_tmpF6=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpF7=*((char*)_check_dyneither_subscript(_tmpF6,sizeof(char),0)),((_tmpF8=(char)(_ulong % 10 + '0'),((_get_dyneither_size(_tmpF6,sizeof(char))== 1  && (_tmpF7 == '\000'  && _tmpF8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpF6.curr)=_tmpF8)))))));}
_ulong /=10;}
# 591
{char _tmpFB;char _tmpFA;struct _dyneither_ptr _tmpF9;(_tmpF9=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpFA=*((char*)_check_dyneither_subscript(_tmpF9,sizeof(char),0)),((_tmpFB=(char)(_ulong + '0'),((_get_dyneither_size(_tmpF9,sizeof(char))== 1  && (_tmpFA == '\000'  && _tmpFB != '\000')?_throw_arraybounds(): 1,*((char*)_tmpF9.curr)=_tmpFB)))))));}
break;case Cyc_HEX: _LL60:
# 595
 if(ch == 'X'){
const char*_tmpFC;xdigs=((_tmpFC="0123456789ABCDEF",_tag_dyneither(_tmpFC,sizeof(char),17)));}else{
# 598
const char*_tmpFD;xdigs=((_tmpFD="0123456789abcdef",_tag_dyneither(_tmpFD,sizeof(char),17)));}
do{
{char _tmp100;char _tmpFF;struct _dyneither_ptr _tmpFE;(_tmpFE=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1),((_tmpFF=*((char*)_check_dyneither_subscript(_tmpFE,sizeof(char),0)),((_tmp100=*((const char*)_check_dyneither_subscript(xdigs,sizeof(char),(int)(_ulong & 15))),((_get_dyneither_size(_tmpFE,sizeof(char))== 1  && (_tmpFF == '\000'  && _tmp100 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpFE.curr)=_tmp100)))))));}
_ulong >>=4;}while((int)_ulong);
# 603
break;}}
# 611
{char*_tmp101;size=((_dyneither_ptr_plus(((_tmp101=buf,_tag_dyneither(_tmp101,sizeof(char),_get_zero_arr_size_char((void*)_tmp101,349)))),sizeof(char),(308 + 39)+ 1)).curr - cp.curr)/ sizeof(char);}
skipsize:
 break;default: _LL5D:
# 615
 if(ch == '\000')
goto done;
# 618
{char*_tmp102;cp=((_tmp102=buf,_tag_dyneither(_tmp102,sizeof(char),_get_zero_arr_size_char((void*)_tmp102,349))));}
{char _tmp105;char _tmp104;struct _dyneither_ptr _tmp103;(_tmp103=cp,((_tmp104=*((char*)_check_dyneither_subscript(_tmp103,sizeof(char),0)),((_tmp105=(char)ch,((_get_dyneither_size(_tmp103,sizeof(char))== 1  && (_tmp104 == '\000'  && _tmp105 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp103.curr)=_tmp105)))))));}
size=1;
sign='\000';
break;}
# 647 "printf.cyc"
fieldsz=size + fpprec;
dpad=dprec - size;
if(dpad < 0)
dpad=0;
# 652
if((int)sign)
++ fieldsz;else{
if(flags & 64)
fieldsz +=2;}
fieldsz +=dpad;
# 659
if((flags & (16 | 32))== 0){
if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}
# 663
if((int)sign){
{char _tmp10A;char _tmp109;struct _dyneither_ptr _tmp108;char*_tmp107;(_tmp108=_dyneither_ptr_plus(((_tmp107=sign_string,_tag_dyneither(_tmp107,sizeof(char),_get_zero_arr_size_char((void*)_tmp107,2)))),sizeof(char),0),((_tmp109=*((char*)_check_dyneither_subscript(_tmp108,sizeof(char),0)),((_tmp10A=sign,((_get_dyneither_size(_tmp108,sizeof(char))== 1  && (_tmp109 == '\000'  && _tmp10A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp108.curr)=_tmp10A)))))));}
do{char*_tmp10B;if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)((_tmp10B=sign_string,_tag_dyneither(_tmp10B,sizeof(char),_get_zero_arr_size_char((void*)_tmp10B,2)))),1)!= 1)goto error;}while(0);}else{
if(flags & 64){
ox[0]='0';
ox[1]=(char)ch;
do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,_tag_dyneither(ox,sizeof(char),2),2)!= 2)goto error;}while(0);}}
# 673
if((flags & (16 | 32))== 32){
if(Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz)< width - fieldsz)goto error;}
# 677
if(Cyc__IO_padn(ioputc,ioputc_env,'0',dpad)< dpad)goto error;
# 680
if(which_cp == 0)
do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp,size)!= size)goto error;}while(0);else{
if(which_cp == 2)
do{if(Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp2,size)!= size)goto error;}while(0);else{
if(which_cp == 3)
do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp3,size)!= size)goto error;}while(0);}}
# 688
if(Cyc__IO_padn(ioputc,ioputc_env,'0',fpprec)< fpprec)goto error;
# 691
if(flags & 16){
if(Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz)< width - fieldsz)goto error;}
# 695
ret +=width > fieldsz?width: fieldsz;};}
# 698
done:
 return ret;
error:
 return - 1;};}
# 705
static struct _dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch){
# 707
struct _dyneither_ptr t;
char expbuffer[309];
{unsigned int _tmp7A=308;unsigned int i;for(i=0;i < _tmp7A;i ++){expbuffer[i]=(char)'0';}expbuffer[_tmp7A]=(char)0;}{char*_tmp10C;struct _dyneither_ptr expbuf=(_tmp10C=(char*)expbuffer,_tag_dyneither(_tmp10C,sizeof(char),_get_zero_arr_size_char((void*)_tmp10C,309)));
{char _tmp10F;char _tmp10E;struct _dyneither_ptr _tmp10D;(_tmp10D=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp10E=*((char*)_check_dyneither_subscript(_tmp10D,sizeof(char),0)),((_tmp10F=(char)fmtch,((_get_dyneither_size(_tmp10D,sizeof(char))== 1  && (_tmp10E == '\000'  && _tmp10F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp10D.curr)=_tmp10F)))))));}
if(exp < 0){
exp=- exp;{
char _tmp112;char _tmp111;struct _dyneither_ptr _tmp110;(_tmp110=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp111=*((char*)_check_dyneither_subscript(_tmp110,sizeof(char),0)),((_tmp112='-',((_get_dyneither_size(_tmp110,sizeof(char))== 1  && (_tmp111 == '\000'  && _tmp112 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp110.curr)=_tmp112)))))));};}else{
# 716
char _tmp115;char _tmp114;struct _dyneither_ptr _tmp113;(_tmp113=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp114=*((char*)_check_dyneither_subscript(_tmp113,sizeof(char),0)),((_tmp115='+',((_get_dyneither_size(_tmp113,sizeof(char))== 1  && (_tmp114 == '\000'  && _tmp115 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp113.curr)=_tmp115)))))));}
t=_dyneither_ptr_plus(expbuf,sizeof(char),308);
if(exp > 9){
do{
char _tmp118;char _tmp117;struct _dyneither_ptr _tmp116;(_tmp116=_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),((_tmp117=*((char*)_check_dyneither_subscript(_tmp116,sizeof(char),0)),((_tmp118=(char)(exp % 10 + '0'),((_get_dyneither_size(_tmp116,sizeof(char))== 1  && (_tmp117 == '\000'  && _tmp118 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp116.curr)=_tmp118)))))));}while((exp /=10)> 9);
# 722
{char _tmp11B;char _tmp11A;struct _dyneither_ptr _tmp119;(_tmp119=_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),((_tmp11A=*((char*)_check_dyneither_subscript(_tmp119,sizeof(char),0)),((_tmp11B=(char)(exp + '0'),((_get_dyneither_size(_tmp119,sizeof(char))== 1  && (_tmp11A == '\000'  && _tmp11B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp119.curr)=_tmp11B)))))));}{
char _tmp11E;char _tmp11D;struct _dyneither_ptr _tmp11C;for(0;(char*)t.curr < (char*)(_dyneither_ptr_plus(expbuf,sizeof(char),308)).curr;(_tmp11C=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp11D=*((char*)_check_dyneither_subscript(_tmp11C,sizeof(char),0)),((_tmp11E=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),sizeof(char),0)),((_get_dyneither_size(_tmp11C,sizeof(char))== 1  && (_tmp11D == '\000'  && _tmp11E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp11C.curr)=_tmp11E)))))))){;}};}else{
# 726
{char _tmp121;char _tmp120;struct _dyneither_ptr _tmp11F;(_tmp11F=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp120=*((char*)_check_dyneither_subscript(_tmp11F,sizeof(char),0)),((_tmp121='0',((_get_dyneither_size(_tmp11F,sizeof(char))== 1  && (_tmp120 == '\000'  && _tmp121 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp11F.curr)=_tmp121)))))));}{
char _tmp124;char _tmp123;struct _dyneither_ptr _tmp122;(_tmp122=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1),((_tmp123=*((char*)_check_dyneither_subscript(_tmp122,sizeof(char),0)),((_tmp124=(char)(exp + '0'),((_get_dyneither_size(_tmp122,sizeof(char))== 1  && (_tmp123 == '\000'  && _tmp124 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp122.curr)=_tmp124)))))));};}
# 729
return p;};}
# 732
static struct _dyneither_ptr Cyc_sround(double fract,int*exp,struct _dyneither_ptr start,struct _dyneither_ptr end,char ch,int*signp){
# 736
double tmp=0.0;
# 738
if(fract != 0.0)
modf(fract * 10,& tmp);else{
# 741
tmp=(double)(ch - '0');}
if(tmp > 4)
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);
{char _tmp127;char _tmp126;struct _dyneither_ptr _tmp125;if(((_tmp125=end,((_tmp126=*((char*)_check_dyneither_subscript(_tmp125,sizeof(char),0)),((_tmp127=_tmp126 + '\001',((_get_dyneither_size(_tmp125,sizeof(char))== 1  && (_tmp126 == '\000'  && _tmp127 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp125.curr)=_tmp127))))))))<= '9')
break;}
{char _tmp12A;char _tmp129;struct _dyneither_ptr _tmp128;(_tmp128=end,((_tmp129=*((char*)_check_dyneither_subscript(_tmp128,sizeof(char),0)),((_tmp12A='0',((_get_dyneither_size(_tmp128,sizeof(char))== 1  && (_tmp129 == '\000'  && _tmp12A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp128.curr)=_tmp12A)))))));}
if((char*)end.curr == (char*)start.curr){
if((unsigned int)exp){
{char _tmp12D;char _tmp12C;struct _dyneither_ptr _tmp12B;(_tmp12B=end,((_tmp12C=*((char*)_check_dyneither_subscript(_tmp12B,sizeof(char),0)),((_tmp12D='1',((_get_dyneither_size(_tmp12B,sizeof(char))== 1  && (_tmp12C == '\000'  && _tmp12D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp12B.curr)=_tmp12D)))))));}
++(*exp);}else{
# 755
{char _tmp130;char _tmp12F;struct _dyneither_ptr _tmp12E;(_tmp12E=_dyneither_ptr_inplace_plus(& end,sizeof(char),-1),((_tmp12F=*((char*)_check_dyneither_subscript(_tmp12E,sizeof(char),0)),((_tmp130='1',((_get_dyneither_size(_tmp12E,sizeof(char))== 1  && (_tmp12F == '\000'  && _tmp130 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp12E.curr)=_tmp130)))))));}
_dyneither_ptr_inplace_plus(& start,sizeof(char),-1);}
# 758
break;}}else{
# 762
if(*signp == '-')
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0))!= '0')
break;
if((char*)end.curr == (char*)start.curr)
*signp=0;}}
# 771
return start;}
# 774
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr endp){
# 777
struct _dyneither_ptr p;struct _dyneither_ptr t;
register double fract;
int dotrim=0;int expcnt;int gformat=0;
double integer=0.0;double tmp=0.0;
# 782
expcnt=0;
if(number < 0){
number=- number;
*signp=(int)'-';}else{
# 787
*signp=0;}
# 789
fract=modf(number,& integer);
# 792
t=_dyneither_ptr_inplace_plus(& startp,sizeof(char),1);
# 798
for(p=_dyneither_ptr_plus(endp,sizeof(char),- 1);(char*)p.curr >= (char*)startp.curr  && integer != 0.0;++ expcnt){
tmp=modf(integer / 10,& integer);{
char _tmp133;char _tmp132;struct _dyneither_ptr _tmp131;(_tmp131=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),-1),((_tmp132=*((char*)_check_dyneither_subscript(_tmp131,sizeof(char),0)),((_tmp133=(char)((int)((tmp + .01)* 10)+ '0'),((_get_dyneither_size(_tmp131,sizeof(char))== 1  && (_tmp132 == '\000'  && _tmp133 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp131.curr)=_tmp133)))))));};}
# 802
switch(fmtch){case 'f': _LL63:
 goto _LL64;case 'F': _LL64:
# 805
 if(expcnt){
char _tmp136;char _tmp135;struct _dyneither_ptr _tmp134;for(0;(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr;(_tmp134=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp135=*((char*)_check_dyneither_subscript(_tmp134,sizeof(char),0)),((_tmp136=*((char*)_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp134,sizeof(char))== 1  && (_tmp135 == '\000'  && _tmp136 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp134.curr)=_tmp136)))))))){;}}else{
# 808
char _tmp139;char _tmp138;struct _dyneither_ptr _tmp137;(_tmp137=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp138=*((char*)_check_dyneither_subscript(_tmp137,sizeof(char),0)),((_tmp139='0',((_get_dyneither_size(_tmp137,sizeof(char))== 1  && (_tmp138 == '\000'  && _tmp139 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp137.curr)=_tmp139)))))));}
# 813
if(prec  || flags & 8){
char _tmp13C;char _tmp13B;struct _dyneither_ptr _tmp13A;(_tmp13A=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp13B=*((char*)_check_dyneither_subscript(_tmp13A,sizeof(char),0)),((_tmp13C='.',((_get_dyneither_size(_tmp13A,sizeof(char))== 1  && (_tmp13B == '\000'  && _tmp13C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp13A.curr)=_tmp13C)))))));}
# 816
if(fract != 0.0){
if(prec)
do{
fract=modf(fract * 10,& tmp);{
char _tmp13F;char _tmp13E;struct _dyneither_ptr _tmp13D;(_tmp13D=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp13E=*((char*)_check_dyneither_subscript(_tmp13D,sizeof(char),0)),((_tmp13F=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp13D,sizeof(char))== 1  && (_tmp13E == '\000'  && _tmp13F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp13D.curr)=_tmp13F)))))));};}while(
-- prec  && fract != 0.0);
if(fract != 0.0)
startp=Cyc_sround(fract,0,startp,
_dyneither_ptr_plus(t,sizeof(char),- 1),'\000',signp);}
# 826
{char _tmp142;char _tmp141;struct _dyneither_ptr _tmp140;for(0;prec --;(_tmp140=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp141=*((char*)_check_dyneither_subscript(_tmp140,sizeof(char),0)),((_tmp142='0',((_get_dyneither_size(_tmp140,sizeof(char))== 1  && (_tmp141 == '\000'  && _tmp142 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp140.curr)=_tmp142)))))))){;}}
break;case 'e': _LL65:
 goto _LL66;case 'E': _LL66:
 eformat: if(expcnt){
{char _tmp145;char _tmp144;struct _dyneither_ptr _tmp143;(_tmp143=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp144=*((char*)_check_dyneither_subscript(_tmp143,sizeof(char),0)),((_tmp145=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& p,sizeof(char),1),sizeof(char),0)),((_get_dyneither_size(_tmp143,sizeof(char))== 1  && (_tmp144 == '\000'  && _tmp145 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp143.curr)=_tmp145)))))));}
if(prec  || flags & 8){
char _tmp148;char _tmp147;struct _dyneither_ptr _tmp146;(_tmp146=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp147=*((char*)_check_dyneither_subscript(_tmp146,sizeof(char),0)),((_tmp148='.',((_get_dyneither_size(_tmp146,sizeof(char))== 1  && (_tmp147 == '\000'  && _tmp148 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp146.curr)=_tmp148)))))));}
# 834
for(0;prec  && (char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr;-- prec){
char _tmp14B;char _tmp14A;struct _dyneither_ptr _tmp149;(_tmp149=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp14A=*((char*)_check_dyneither_subscript(_tmp149,sizeof(char),0)),((_tmp14B=*((char*)_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp149,sizeof(char))== 1  && (_tmp14A == '\000'  && _tmp14B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp149.curr)=_tmp14B)))))));}
# 841
if(!prec  && (char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr){
fract=(double)0;
startp=Cyc_sround((double)0,& expcnt,startp,
_dyneither_ptr_plus(t,sizeof(char),- 1),*((char*)_check_dyneither_subscript(p,sizeof(char),0)),signp);}
# 847
-- expcnt;}else{
# 850
if(fract != 0.0){
# 852
for(expcnt=- 1;1;-- expcnt){
fract=modf(fract * 10,& tmp);
if(tmp != 0.0)
break;}
# 857
{char _tmp14E;char _tmp14D;struct _dyneither_ptr _tmp14C;(_tmp14C=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp14D=*((char*)_check_dyneither_subscript(_tmp14C,sizeof(char),0)),((_tmp14E=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp14C,sizeof(char))== 1  && (_tmp14D == '\000'  && _tmp14E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp14C.curr)=_tmp14E)))))));}
if(prec  || flags & 8){
char _tmp151;char _tmp150;struct _dyneither_ptr _tmp14F;(_tmp14F=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp150=*((char*)_check_dyneither_subscript(_tmp14F,sizeof(char),0)),((_tmp151='.',((_get_dyneither_size(_tmp14F,sizeof(char))== 1  && (_tmp150 == '\000'  && _tmp151 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp14F.curr)=_tmp151)))))));}}else{
# 862
{char _tmp154;char _tmp153;struct _dyneither_ptr _tmp152;(_tmp152=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp153=*((char*)_check_dyneither_subscript(_tmp152,sizeof(char),0)),((_tmp154='0',((_get_dyneither_size(_tmp152,sizeof(char))== 1  && (_tmp153 == '\000'  && _tmp154 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp152.curr)=_tmp154)))))));}
if(prec  || flags & 8){
char _tmp157;char _tmp156;struct _dyneither_ptr _tmp155;(_tmp155=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp156=*((char*)_check_dyneither_subscript(_tmp155,sizeof(char),0)),((_tmp157='.',((_get_dyneither_size(_tmp155,sizeof(char))== 1  && (_tmp156 == '\000'  && _tmp157 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp155.curr)=_tmp157)))))));}}}
# 867
if(fract != 0.0){
if(prec)
do{
fract=modf(fract * 10,& tmp);{
char _tmp15A;char _tmp159;struct _dyneither_ptr _tmp158;(_tmp158=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp159=*((char*)_check_dyneither_subscript(_tmp158,sizeof(char),0)),((_tmp15A=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp158,sizeof(char))== 1  && (_tmp159 == '\000'  && _tmp15A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp158.curr)=_tmp15A)))))));};}while(
-- prec  && fract != 0.0);
if(fract != 0.0)
startp=Cyc_sround(fract,& expcnt,startp,
_dyneither_ptr_plus(t,sizeof(char),- 1),'\000',signp);}
# 878
{char _tmp15D;char _tmp15C;struct _dyneither_ptr _tmp15B;for(0;prec --;(_tmp15B=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp15C=*((char*)_check_dyneither_subscript(_tmp15B,sizeof(char),0)),((_tmp15D='0',((_get_dyneither_size(_tmp15B,sizeof(char))== 1  && (_tmp15C == '\000'  && _tmp15D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp15B.curr)=_tmp15D)))))))){;}}
# 881
if(gformat  && !(flags & 8)){
while((char*)t.curr > (char*)startp.curr  && *((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0'){;}
if(*((char*)_check_dyneither_subscript(t,sizeof(char),0))== '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
# 887
t=Cyc_exponent(t,expcnt,fmtch);
break;case 'g': _LL67:
 goto _LL68;case 'G': _LL68:
# 891
 if(!prec)
++ prec;
# 899
if(expcnt > prec  || (!expcnt  && fract != 0.0) && fract < .0001){
# 907
-- prec;
fmtch -=2;
gformat=1;
goto eformat;}
# 916
if(expcnt){
char _tmp160;char _tmp15F;struct _dyneither_ptr _tmp15E;for(0;(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr < (char*)endp.curr;(((_tmp15E=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp15F=*((char*)_check_dyneither_subscript(_tmp15E,sizeof(char),0)),((_tmp160=*((char*)_check_dyneither_subscript(p,sizeof(char),0)),((_get_dyneither_size(_tmp15E,sizeof(char))== 1  && (_tmp15F == '\000'  && _tmp160 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp15E.curr)=_tmp160)))))))),-- prec)){;}}else{
# 919
char _tmp163;char _tmp162;struct _dyneither_ptr _tmp161;(_tmp161=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp162=*((char*)_check_dyneither_subscript(_tmp161,sizeof(char),0)),((_tmp163='0',((_get_dyneither_size(_tmp161,sizeof(char))== 1  && (_tmp162 == '\000'  && _tmp163 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp161.curr)=_tmp163)))))));}
# 924
if(prec  || flags & 8){
dotrim=1;{
char _tmp166;char _tmp165;struct _dyneither_ptr _tmp164;(_tmp164=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp165=*((char*)_check_dyneither_subscript(_tmp164,sizeof(char),0)),((_tmp166='.',((_get_dyneither_size(_tmp164,sizeof(char))== 1  && (_tmp165 == '\000'  && _tmp166 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp164.curr)=_tmp166)))))));};}else{
# 929
dotrim=0;}
# 931
if(fract != 0.0){
if(prec){
# 935
do{
fract=modf(fract * 10,& tmp);{
char _tmp169;char _tmp168;struct _dyneither_ptr _tmp167;(_tmp167=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp168=*((char*)_check_dyneither_subscript(_tmp167,sizeof(char),0)),((_tmp169=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp167,sizeof(char))== 1  && (_tmp168 == '\000'  && _tmp169 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp167.curr)=_tmp169)))))));};}while(
tmp == 0.0  && !expcnt);
while(-- prec  && fract != 0.0){
fract=modf(fract * 10,& tmp);{
char _tmp16C;char _tmp16B;struct _dyneither_ptr _tmp16A;(_tmp16A=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp16B=*((char*)_check_dyneither_subscript(_tmp16A,sizeof(char),0)),((_tmp16C=(char)((int)tmp + '0'),((_get_dyneither_size(_tmp16A,sizeof(char))== 1  && (_tmp16B == '\000'  && _tmp16C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp16A.curr)=_tmp16C)))))));};}}
# 944
if(fract != 0.0)
startp=Cyc_sround(fract,0,startp,
_dyneither_ptr_plus(t,sizeof(char),- 1),'\000',signp);}
# 949
if(flags & 8){
char _tmp16F;char _tmp16E;struct _dyneither_ptr _tmp16D;for(0;prec --;(_tmp16D=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),((_tmp16E=*((char*)_check_dyneither_subscript(_tmp16D,sizeof(char),0)),((_tmp16F='0',((_get_dyneither_size(_tmp16D,sizeof(char))== 1  && (_tmp16E == '\000'  && _tmp16F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp16D.curr)=_tmp16F)))))))){;}}else{
if(dotrim){
while((char*)t.curr > (char*)startp.curr  && *((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0))== '0'){;}
if(*((char*)_check_dyneither_subscript(t,sizeof(char),0))!= '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}}
# 956
break;default: _LL69: {
struct Cyc_Core_Impossible_exn_struct _tmp175;const char*_tmp174;struct Cyc_Core_Impossible_exn_struct*_tmp173;(int)_throw((void*)((_tmp173=_cycalloc(sizeof(*_tmp173)),((_tmp173[0]=((_tmp175.tag=Cyc_Core_Impossible,((_tmp175.f1=((_tmp174="__cvt_double",_tag_dyneither(_tmp174,sizeof(char),13))),_tmp175)))),_tmp173)))));}}
# 959
return(t.curr - startp.curr)/ sizeof(char);}
# 963
int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 966
int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,f,fmt,ap);
return ans;}
# 971
int Cyc_fprintf(struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 975
return Cyc_vfprintf(f,fmt,ap);}
# 978
int Cyc_vprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 981
int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);
return ans;}
# 986
int Cyc_printf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 989
int ans;
ans=Cyc_vprintf(fmt,ap);
return ans;}struct _tuple0{struct _dyneither_ptr*f1;unsigned long*f2;};
# 994
static int Cyc_putc_string(int c,struct _tuple0*sptr_n){
struct _dyneither_ptr*_tmpCA;unsigned long*_tmpCB;struct _tuple0*_tmpC9=sptr_n;_tmpCA=_tmpC9->f1;_tmpCB=_tmpC9->f2;{
struct _dyneither_ptr s=*_tmpCA;
unsigned long n=*_tmpCB;
if(n == 0)return - 1;
*((char*)_check_dyneither_subscript(s,sizeof(char),0))=(char)c;
_dyneither_ptr_inplace_plus(_tmpCA,sizeof(char),1);
*_tmpCB=n - 1;
return 1;};}
# 1005
int Cyc_vsnprintf(struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1008
int ans;
struct _dyneither_ptr _tmpCC=s;
unsigned long _tmpCD=n;
struct _tuple0 _tmp176;struct _tuple0 _tmpCE=(_tmp176.f1=& _tmpCC,((_tmp176.f2=& _tmpCD,_tmp176)));
ans=((int(*)(int(*ioputc)(int,struct _tuple0*),struct _tuple0*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpCE,fmt,ap);
if(0 <= ans)
*((char*)_check_dyneither_subscript(s,sizeof(char),ans))='\000';
return ans;}
# 1018
int Cyc_snprintf(struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1021
return Cyc_vsnprintf(s,n,fmt,ap);}
# 1024
int Cyc_vsprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1027
return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),fmt,ap);}
# 1030
int Cyc_sprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1033
return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),fmt,ap);}
# 1036
static int Cyc_putc_void(int c,int dummy){
return 1;}
# 1040
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1044
int size=((int(*)(int(*ioputc)(int,int),int ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 1;
char*_tmp178;unsigned int _tmp177;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp177=size + 1,((_tmp178=_cyccalloc_atomic(sizeof(char),_tmp177),_tag_dyneither(_tmp178,sizeof(char),_tmp177)))));
Cyc_vsprintf(_dyneither_ptr_decrease_size(s,sizeof(char),1),fmt,ap);
return s;}
# 1050
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*r1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1053
return Cyc_vrprintf(r1,fmt,ap);}
# 1056
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1059
return Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
