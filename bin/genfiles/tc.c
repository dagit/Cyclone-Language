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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 252 "core.h"
void Cyc_Core_rethrow(void*);
# 281
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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Relations_Reln;
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 765 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 927
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 935
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 941
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 944
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 984
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 1006
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1017
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1089
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1177
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 65
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 72
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);struct _tuple10{void*f1;void*f2;};
# 232 "dict.h"
struct _tuple10*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 255
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);struct Cyc_RgnOrder_RgnPO;
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
# 119 "tcenv.h"
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 126
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 131
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 199
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 227
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 74
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_is_function_type(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 114
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 242
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 314
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
int Cyc_Tcutil_supports_default(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 41 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 66 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 69
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 71
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 79
void*Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 88
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 85
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 47 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 50
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 56
(*_tmp1).f3=1;
return 1;}}
# 60
return 0;}
# 63
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
{const char*_tmp2AD;void*_tmp2AC[1];struct Cyc_String_pa_PrintArg_struct _tmp2AB;(_tmp2AB.tag=0,((_tmp2AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att)),((_tmp2AC[0]=& _tmp2AB,Cyc_Tcutil_terr(loc,((_tmp2AD="%s has an out-of-range index",_tag_dyneither(_tmp2AD,sizeof(char),29))),_tag_dyneither(_tmp2AC,sizeof(void*),1)))))));}
return 0;}
# 68
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 70
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 73
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i)){
const char*_tmp2B3;void*_tmp2B2[3];struct Cyc_String_pa_PrintArg_struct _tmp2B1;struct Cyc_String_pa_PrintArg_struct _tmp2B0;struct Cyc_Int_pa_PrintArg_struct _tmp2AF;(_tmp2AF.tag=1,((_tmp2AF.f1=(unsigned long)i,((_tmp2B0.tag=0,((_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2),((_tmp2B1.tag=0,((_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2),((_tmp2B2[0]=& _tmp2B1,((_tmp2B2[1]=& _tmp2B0,((_tmp2B2[2]=& _tmp2AF,Cyc_Tcutil_terr(loc,((_tmp2B3="incompatible %s() and %s() attributes on parameter %d",_tag_dyneither(_tmp2B3,sizeof(char),54))),_tag_dyneither(_tmp2B2,sizeof(void*),3)))))))))))))))))));}}
# 77
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{const char*_tmp2B6;struct _tuple13 _tmp2B5;struct _tuple13 init_params=(_tmp2B5.f1=0,((_tmp2B5.f2=((_tmp2B6="initializes",_tag_dyneither(_tmp2B6,sizeof(char),12))),_tmp2B5)));
const char*_tmp2B9;struct _tuple13 _tmp2B8;struct _tuple13 nolive_unique_params=(_tmp2B8.f1=0,((_tmp2B8.f2=((_tmp2B9="noliveunique",_tag_dyneither(_tmp2B9,sizeof(char),13))),_tmp2B8)));
const char*_tmp2BC;struct _tuple13 _tmp2BB;struct _tuple13 noconsume_params=(_tmp2BB.f1=0,((_tmp2BB.f2=((_tmp2BC="noconsume",_tag_dyneither(_tmp2BC,sizeof(char),10))),_tmp2BB)));
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmpE;struct Cyc_List_List*_tmpF;_LL1: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC;if(_tmpD->tag != 9)goto _LL3;else{_tmpE=(_tmpD->f1).attributes;_tmpF=(_tmpD->f1).args;}}_LL2: {
# 84
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpF);
for(0;_tmpE != 0;_tmpE=_tmpE->tl){
void*_tmp10=(void*)_tmpE->hd;void*_tmp11=_tmp10;int _tmp13;int _tmp15;int _tmp17;_LL6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp12=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp11;if(_tmp12->tag != 20)goto _LL8;else{_tmp13=_tmp12->f1;}}_LL7:
# 88
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp13,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp13,init_params,noconsume_params);{
struct _tuple8*_tmp18=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp13 - 1);void*_tmp1A;struct _tuple8*_tmp19=_tmp18;_tmp1A=_tmp19->f3;{
const char*_tmp2BD;struct _dyneither_ptr s=(_tmp2BD="initializes attribute allowed only on",_tag_dyneither(_tmp2BD,sizeof(char),38));
{void*_tmp1B=Cyc_Tcutil_compress(_tmp1A);void*_tmp1C=_tmp1B;void*_tmp1E;union Cyc_Absyn_Constraint*_tmp1F;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp21;_LLF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C;if(_tmp1D->tag != 5)goto _LL11;else{_tmp1E=(_tmp1D->f1).elt_typ;_tmp1F=((_tmp1D->f1).ptr_atts).nullable;_tmp20=((_tmp1D->f1).ptr_atts).bounds;_tmp21=((_tmp1D->f1).ptr_atts).zero_term;}}_LL10:
# 95
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp1F)){
const char*_tmp2C1;void*_tmp2C0[1];struct Cyc_String_pa_PrintArg_struct _tmp2BF;(_tmp2BF.tag=0,((_tmp2BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2C0[0]=& _tmp2BF,Cyc_Tcutil_terr(loc,((_tmp2C1="%s non-null pointers",_tag_dyneither(_tmp2C1,sizeof(char),21))),_tag_dyneither(_tmp2C0,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_bound_one(_tmp20)){
const char*_tmp2C5;void*_tmp2C4[1];struct Cyc_String_pa_PrintArg_struct _tmp2C3;(_tmp2C3.tag=0,((_tmp2C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2C4[0]=& _tmp2C3,Cyc_Tcutil_terr(loc,((_tmp2C5="%s pointers of size 1",_tag_dyneither(_tmp2C5,sizeof(char),22))),_tag_dyneither(_tmp2C4,sizeof(void*),1)))))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp21)){
const char*_tmp2C9;void*_tmp2C8[1];struct Cyc_String_pa_PrintArg_struct _tmp2C7;(_tmp2C7.tag=0,((_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2C8[0]=& _tmp2C7,Cyc_Tcutil_terr(loc,((_tmp2C9="%s pointers to non-zero-terminated arrays",_tag_dyneither(_tmp2C9,sizeof(char),42))),_tag_dyneither(_tmp2C8,sizeof(void*),1)))))));}
goto _LLE;_LL11:;_LL12: {
# 103
const char*_tmp2CD;void*_tmp2CC[1];struct Cyc_String_pa_PrintArg_struct _tmp2CB;(_tmp2CB.tag=0,((_tmp2CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp2CC[0]=& _tmp2CB,Cyc_Tcutil_terr(loc,((_tmp2CD="%s pointers",_tag_dyneither(_tmp2CD,sizeof(char),12))),_tag_dyneither(_tmp2CC,sizeof(void*),1)))))));}_LLE:;}
# 105
{struct Cyc_List_List*_tmp2CE;init_params.f1=((_tmp2CE=_region_malloc(temp,sizeof(*_tmp2CE)),((_tmp2CE->hd=(void*)_tmp13,((_tmp2CE->tl=init_params.f1,_tmp2CE))))));}
goto _LL5;};};_LL8: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp14=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp11;if(_tmp14->tag != 21)goto _LLA;else{_tmp15=_tmp14->f1;}}_LL9:
# 108
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp15,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp15,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp30=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp15 - 1);void*_tmp32;struct _tuple8*_tmp31=_tmp30;_tmp32=_tmp31->f3;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp32,0)){
const char*_tmp2D1;void*_tmp2D0;(_tmp2D0=0,Cyc_Tcutil_terr(loc,((_tmp2D1="noliveunique attribute allowed only on unique pointers",_tag_dyneither(_tmp2D1,sizeof(char),55))),_tag_dyneither(_tmp2D0,sizeof(void*),0)));}
{struct Cyc_List_List*_tmp2D2;nolive_unique_params.f1=((_tmp2D2=_region_malloc(temp,sizeof(*_tmp2D2)),((_tmp2D2->hd=(void*)_tmp15,((_tmp2D2->tl=nolive_unique_params.f1,_tmp2D2))))));}
goto _LL5;};_LLA: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp16=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp11;if(_tmp16->tag != 22)goto _LLC;else{_tmp17=_tmp16->f1;}}_LLB:
# 117
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp17,nargs,(void*)_tmpE->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp17,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp17,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp36=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmpF,_tmp17 - 1);void*_tmp38;struct _tuple8*_tmp37=_tmp36;_tmp38=_tmp37->f3;
# 122
{struct Cyc_List_List*_tmp2D3;noconsume_params.f1=((_tmp2D3=_region_malloc(temp,sizeof(*_tmp2D3)),((_tmp2D3->hd=(void*)_tmp17,((_tmp2D3->tl=noconsume_params.f1,_tmp2D3))))));}
goto _LL5;};_LLC:;_LLD:
 goto _LL5;_LL5:;}
# 127
goto _LL0;}_LL3:;_LL4: {
const char*_tmp2D6;void*_tmp2D5;(_tmp2D5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp2D6="fnTypeAttsOK: not a function type",_tag_dyneither(_tmp2D6,sizeof(char),34))),_tag_dyneither(_tmp2D5,sizeof(void*),0)));}_LL0:;}
# 79
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 133
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 137
enum Cyc_Absyn_Scope _tmp43;union Cyc_Absyn_Nmspace _tmp44;struct _dyneither_ptr*_tmp45;void*_tmp46;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_List_List*_tmp48;struct Cyc_Absyn_Vardecl*_tmp42=vd;_tmp43=_tmp42->sc;_tmp44=(_tmp42->name)->f1;_tmp45=(_tmp42->name)->f2;_tmp46=_tmp42->type;_tmp47=_tmp42->initializer;_tmp48=_tmp42->attributes;
# 143
{union Cyc_Absyn_Nmspace _tmp49=_tmp44;_LL14: if((_tmp49.Rel_n).tag != 1)goto _LL16;_LL15:
# 146
(*vd->name).f1=Cyc_Absyn_Abs_n(te->ns,vd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL13;_LL16:;_LL17:
 goto _LL13;_LL13:;}
# 152
{void*_tmp4A=Cyc_Tcutil_compress(_tmp46);void*_tmp4B=_tmp4A;void*_tmp4D;struct Cyc_Absyn_Tqual _tmp4E;union Cyc_Absyn_Constraint*_tmp4F;unsigned int _tmp50;_LL19:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B;if(_tmp4C->tag != 8)goto _LL1B;else{_tmp4D=(_tmp4C->f1).elt_type;_tmp4E=(_tmp4C->f1).tq;if((_tmp4C->f1).num_elts != 0)goto _LL1B;_tmp4F=(_tmp4C->f1).zero_term;_tmp50=(_tmp4C->f1).zt_loc;}}if(!(_tmp47 != 0))goto _LL1B;_LL1A:
# 154
{void*_tmp51=_tmp47->r;void*_tmp52=_tmp51;struct _dyneither_ptr _tmp54;struct _dyneither_ptr _tmp56;struct Cyc_Absyn_Exp*_tmp58;struct Cyc_Absyn_Exp*_tmp5A;struct Cyc_List_List*_tmp5C;struct Cyc_List_List*_tmp5E;_LL1E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp53=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp53->tag != 0)goto _LL20;else{if(((_tmp53->f1).String_c).tag != 8)goto _LL20;_tmp54=(struct _dyneither_ptr)((_tmp53->f1).String_c).val;}}_LL1F:
# 156
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp54,sizeof(char)),0),_tmp4F,_tmp50));
goto _LL1D;_LL20: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp55=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp55->tag != 0)goto _LL22;else{if(((_tmp55->f1).Wstring_c).tag != 9)goto _LL22;_tmp56=(struct _dyneither_ptr)((_tmp55->f1).Wstring_c).val;}}_LL21:
# 159
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp(1,0),_tmp4F,_tmp50));
goto _LL1D;_LL22: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp57=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp57->tag != 26)goto _LL24;else{_tmp58=_tmp57->f2;}}_LL23:
 _tmp5A=_tmp58;goto _LL25;_LL24: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp59=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp59->tag != 27)goto _LL26;else{_tmp5A=_tmp59->f1;}}_LL25:
# 164
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,_tmp5A,_tmp4F,_tmp50));
goto _LL1D;_LL26: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp5B->tag != 35)goto _LL28;else{_tmp5C=_tmp5B->f2;}}_LL27:
 _tmp5E=_tmp5C;goto _LL29;_LL28: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp52;if(_tmp5D->tag != 25)goto _LL2A;else{_tmp5E=_tmp5D->f1;}}_LL29:
# 168
 _tmp46=(vd->type=Cyc_Absyn_array_typ(_tmp4D,_tmp4E,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp5E),0),_tmp4F,_tmp50));
goto _LL1D;_LL2A:;_LL2B:
 goto _LL1D;_LL1D:;}
# 172
goto _LL18;_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 176
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp46);
# 178
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp46);
# 181
{void*_tmp5F=Cyc_Tcutil_compress(_tmp46);void*_tmp60=_tmp5F;_LL2D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60;if(_tmp61->tag != 8)goto _LL2F;}_LL2E:
 vd->escapes=0;goto _LL2C;_LL2F:;_LL30:
 vd->escapes=1;goto _LL2C;_LL2C:;}
# 186
if(Cyc_Tcutil_is_function_type(_tmp46)){
_tmp48=Cyc_Tcutil_transfer_fn_type_atts(_tmp46,_tmp48);
Cyc_Tc_fnTypeAttsOK(te,_tmp46,loc);}
# 191
if(_tmp43 == Cyc_Absyn_Extern  || _tmp43 == Cyc_Absyn_ExternC){
if(_tmp47 != 0  && !in_cinclude){
const char*_tmp2D9;void*_tmp2D8;(_tmp2D8=0,Cyc_Tcutil_terr(loc,((_tmp2D9="extern declaration should not have initializer",_tag_dyneither(_tmp2D9,sizeof(char),47))),_tag_dyneither(_tmp2D8,sizeof(void*),0)));}}else{
if(!Cyc_Tcutil_is_function_type(_tmp46)){
# 198
for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
void*_tmp64=(void*)_tmp48->hd;void*_tmp65=_tmp64;_LL32: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp66=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp65;if(_tmp66->tag != 6)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp67=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp65;if(_tmp67->tag != 8)goto _LL36;}_LL35:
# 203
 goto _LL37;_LL36: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp68=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp65;if(_tmp68->tag != 9)goto _LL38;}_LL37:
 goto _LL39;_LL38: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp69=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp65;if(_tmp69->tag != 10)goto _LL3A;}_LL39:
 goto _LL3B;_LL3A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp6A=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6A->tag != 11)goto _LL3C;}_LL3B:
 goto _LL3D;_LL3C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp6B=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6B->tag != 12)goto _LL3E;}_LL3D:
 goto _LL3F;_LL3E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp6C=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6C->tag != 13)goto _LL40;}_LL3F:
 goto _LL41;_LL40: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp6D=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp65;if(_tmp6D->tag != 14)goto _LL42;}_LL41:
 continue;_LL42:;_LL43:
# 211
{const char*_tmp2DE;void*_tmp2DD[2];struct Cyc_String_pa_PrintArg_struct _tmp2DC;struct Cyc_String_pa_PrintArg_struct _tmp2DB;(_tmp2DB.tag=0,((_tmp2DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name)),((_tmp2DC.tag=0,((_tmp2DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp48->hd)),((_tmp2DD[0]=& _tmp2DC,((_tmp2DD[1]=& _tmp2DB,Cyc_Tcutil_terr(loc,((_tmp2DE="bad attribute %s for variable %s",_tag_dyneither(_tmp2DE,sizeof(char),33))),_tag_dyneither(_tmp2DD,sizeof(void*),2)))))))))))));}
goto _LL31;_LL31:;}
# 216
if(_tmp47 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp46)){
const char*_tmp2E3;void*_tmp2E2[2];struct Cyc_String_pa_PrintArg_struct _tmp2E1;struct Cyc_String_pa_PrintArg_struct _tmp2E0;(_tmp2E0.tag=0,((_tmp2E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp46)),((_tmp2E1.tag=0,((_tmp2E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp2E2[0]=& _tmp2E1,((_tmp2E2[1]=& _tmp2E0,Cyc_Tcutil_terr(loc,((_tmp2E3="initializer required for variable %s of type %s",_tag_dyneither(_tmp2E3,sizeof(char),48))),_tag_dyneither(_tmp2E2,sizeof(void*),2)))))))))))));}}else{
# 221
struct Cyc_Absyn_Exp*_tmp76=_tmp47;
# 225
struct _handler_cons _tmp77;_push_handler(& _tmp77);{int _tmp79=0;if(setjmp(_tmp77.handler))_tmp79=1;if(!_tmp79){
{void*_tmp7A=Cyc_Tcexp_tcExpInitializer(te,& _tmp46,_tmp76);
if(!Cyc_Tcutil_coerce_assign(te,_tmp76,_tmp46)){
struct _dyneither_ptr _tmp7B=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp7C=" declared with type ";
struct _dyneither_ptr _tmp7D=Cyc_Absynpp_typ2string(_tmp46);
const char*_tmp7E=" but initialized with type ";
struct _dyneither_ptr _tmp7F=Cyc_Absynpp_typ2string(_tmp7A);
{const char*_tmp2E5;const char*_tmp2E4;if((((Cyc_strlen((struct _dyneither_ptr)_tmp7B)+ Cyc_strlen(((_tmp2E4=_tmp7C,_tag_dyneither(_tmp2E4,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E4,21))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp7D))+ Cyc_strlen(((_tmp2E5=_tmp7E,_tag_dyneither(_tmp2E5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E5,28))))))+ Cyc_strlen((struct _dyneither_ptr)_tmp7F)> 70){
const char*_tmp2F1;void*_tmp2F0[5];struct Cyc_String_pa_PrintArg_struct _tmp2EF;const char*_tmp2EE;struct Cyc_String_pa_PrintArg_struct _tmp2ED;struct Cyc_String_pa_PrintArg_struct _tmp2EC;const char*_tmp2EB;struct Cyc_String_pa_PrintArg_struct _tmp2EA;struct Cyc_String_pa_PrintArg_struct _tmp2E9;(_tmp2E9.tag=0,((_tmp2E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F),((_tmp2EA.tag=0,((_tmp2EA.f1=(struct _dyneither_ptr)((_tmp2EB=_tmp7E,_tag_dyneither(_tmp2EB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EB,28)))),((_tmp2EC.tag=0,((_tmp2EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7D),((_tmp2ED.tag=0,((_tmp2ED.f1=(struct _dyneither_ptr)((_tmp2EE=_tmp7C,_tag_dyneither(_tmp2EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EE,21)))),((_tmp2EF.tag=0,((_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7B),((_tmp2F0[0]=& _tmp2EF,((_tmp2F0[1]=& _tmp2ED,((_tmp2F0[2]=& _tmp2EC,((_tmp2F0[3]=& _tmp2EA,((_tmp2F0[4]=& _tmp2E9,Cyc_Tcutil_terr(loc,((_tmp2F1="%s%s\n\t%s\n%s\n\t%s",_tag_dyneither(_tmp2F1,sizeof(char),16))),_tag_dyneither(_tmp2F0,sizeof(void*),5)))))))))))))))))))))))))))))));}else{
# 236
const char*_tmp2FD;void*_tmp2FC[5];struct Cyc_String_pa_PrintArg_struct _tmp2FB;const char*_tmp2FA;struct Cyc_String_pa_PrintArg_struct _tmp2F9;struct Cyc_String_pa_PrintArg_struct _tmp2F8;const char*_tmp2F7;struct Cyc_String_pa_PrintArg_struct _tmp2F6;struct Cyc_String_pa_PrintArg_struct _tmp2F5;(_tmp2F5.tag=0,((_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7F),((_tmp2F6.tag=0,((_tmp2F6.f1=(struct _dyneither_ptr)((_tmp2F7=_tmp7E,_tag_dyneither(_tmp2F7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F7,28)))),((_tmp2F8.tag=0,((_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7D),((_tmp2F9.tag=0,((_tmp2F9.f1=(struct _dyneither_ptr)((_tmp2FA=_tmp7C,_tag_dyneither(_tmp2FA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FA,21)))),((_tmp2FB.tag=0,((_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7B),((_tmp2FC[0]=& _tmp2FB,((_tmp2FC[1]=& _tmp2F9,((_tmp2FC[2]=& _tmp2F8,((_tmp2FC[3]=& _tmp2F6,((_tmp2FC[4]=& _tmp2F5,Cyc_Tcutil_terr(loc,((_tmp2FD="%s%s%s%s%s",_tag_dyneither(_tmp2FD,sizeof(char),11))),_tag_dyneither(_tmp2FC,sizeof(void*),5)))))))))))))))))))))))))))))));}}
Cyc_Tcutil_explain_failure();
# 239
if(!Cyc_Tcutil_is_const_exp(_tmp76)){
const char*_tmp300;void*_tmp2FF;(_tmp2FF=0,Cyc_Tcutil_terr(loc,((_tmp300="initializer is not a constant expression",_tag_dyneither(_tmp300,sizeof(char),41))),_tag_dyneither(_tmp2FF,sizeof(void*),0)));}}}
# 226
;_pop_handler();}else{void*_tmp78=(void*)_exn_thrown;void*_tmp97=_tmp78;void*_tmp99;_LL45: {struct Cyc_Tcenv_Env_error_exn_struct*_tmp98=(struct Cyc_Tcenv_Env_error_exn_struct*)_tmp97;if(_tmp98->tag != Cyc_Tcenv_Env_error)goto _LL47;}_LL46:
# 244
{const char*_tmp303;void*_tmp302;(_tmp302=0,Cyc_Tcutil_terr(loc,((_tmp303="initializer is not a constant expression",_tag_dyneither(_tmp303,sizeof(char),41))),_tag_dyneither(_tmp302,sizeof(void*),0)));}
goto _LL44;_LL47: _tmp99=_tmp97;_LL48:(void)_rethrow(_tmp99);_LL44:;}};}}else{
# 250
for(0;_tmp48 != 0;_tmp48=_tmp48->tl){
void*_tmp9C=(void*)_tmp48->hd;void*_tmp9D=_tmp9C;_LL4A: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp9E=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL4C;}_LL4B:
# 253
 goto _LL4D;_LL4C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp9F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL4E;}_LL4D:
 goto _LL4F;_LL4E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA0->tag != 2)goto _LL50;}_LL4F:
 goto _LL51;_LL50: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA1->tag != 3)goto _LL52;}_LL51:
 goto _LL53;_LL52: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpA2=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA2->tag != 4)goto _LL54;}_LL53:
 goto _LL55;_LL54: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA3=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA3->tag != 19)goto _LL56;}_LL55:
 goto _LL57;_LL56: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpA4=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA4->tag != 20)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmpA5=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA5->tag != 23)goto _LL5A;}_LL59:
 goto _LL5B;_LL5A: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmpA6=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA6->tag != 5)goto _LL5C;}_LL5B: {
const char*_tmp306;void*_tmp305;(_tmp305=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp306="tcVardecl: fn type atts in function var decl",_tag_dyneither(_tmp306,sizeof(char),45))),_tag_dyneither(_tmp305,sizeof(void*),0)));}_LL5C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpA7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA7->tag != 6)goto _LL5E;}_LL5D:
# 263
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpA8=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9D;if(_tmpA8->tag != 7)goto _LL60;}_LL5F:
# 265
{const char*_tmp30A;void*_tmp309[1];struct Cyc_String_pa_PrintArg_struct _tmp308;(_tmp308.tag=0,((_tmp308.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp48->hd)),((_tmp309[0]=& _tmp308,Cyc_Tcutil_terr(loc,((_tmp30A="bad attribute %s in function declaration",_tag_dyneither(_tmp30A,sizeof(char),41))),_tag_dyneither(_tmp309,sizeof(void*),1)))))));}
goto _LL49;_LL60:;_LL61:
 continue;_LL49:;}}}
# 273
{struct _handler_cons _tmpAE;_push_handler(& _tmpAE);{int _tmpB0=0;if(setjmp(_tmpAE.handler))_tmpB0=1;if(!_tmpB0){
{struct _tuple14*_tmpB1=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,_tmp45);
void*_tmpB2=(*_tmpB1).f1;void*_tmpB3=_tmpB2;void*_tmpB5;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB4=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpB3;if(_tmpB4->tag != 0)goto _LL65;else{_tmpB5=(void*)_tmpB4->f1;}}_LL64: {
# 277
struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp30D;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp30C;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpBA=(_tmp30C=_cycalloc(sizeof(*_tmp30C)),((_tmp30C[0]=((_tmp30D.tag=1,((_tmp30D.f1=vd,_tmp30D)))),_tmp30C)));
void*_tmpBB=Cyc_Tcdecl_merge_binding(_tmpB5,(void*)_tmpBA,loc,Cyc_Tc_tc_msg);
if(_tmpBB == (void*)& Cyc_Absyn_Unresolved_b_val){_npop_handler(0);return;}
# 281
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmpBB != _tmpB5  || (*_tmpB1).f2){
# 284
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp313;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp312;struct _tuple14*_tmp311;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp45,(
(_tmp311=_cycalloc(sizeof(*_tmp311)),((_tmp311->f1=(void*)((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=((_tmp312.tag=0,((_tmp312.f1=_tmpBB,_tmp312)))),_tmp313)))),((_tmp311->f2=1,_tmp311)))))));}}
_npop_handler(0);return;}_LL65: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpB3;if(_tmpB6->tag != 1)goto _LL67;}_LL66:
# 288
{const char*_tmp316;void*_tmp315;(_tmp315=0,Cyc_Tcutil_warn(loc,((_tmp316="variable declaration shadows previous struct declaration",_tag_dyneither(_tmp316,sizeof(char),57))),_tag_dyneither(_tmp315,sizeof(void*),0)));}
goto _LL62;_LL67: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB7=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpB3;if(_tmpB7->tag != 2)goto _LL69;}_LL68:
# 291
{const char*_tmp319;void*_tmp318;(_tmp318=0,Cyc_Tcutil_warn(loc,((_tmp319="variable declaration shadows previous datatype constructor",_tag_dyneither(_tmp319,sizeof(char),59))),_tag_dyneither(_tmp318,sizeof(void*),0)));}
goto _LL62;_LL69: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB8=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpB3;if(_tmpB8->tag != 4)goto _LL6B;}_LL6A:
 goto _LL6C;_LL6B: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpB3;if(_tmpB9->tag != 3)goto _LL62;}_LL6C:
# 295
{const char*_tmp31C;void*_tmp31B;(_tmp31B=0,Cyc_Tcutil_warn(loc,((_tmp31C="variable declaration shadows previous enum tag",_tag_dyneither(_tmp31C,sizeof(char),47))),_tag_dyneither(_tmp31B,sizeof(void*),0)));}
goto _LL62;_LL62:;}
# 274
;_pop_handler();}else{void*_tmpAF=(void*)_exn_thrown;void*_tmpC8=_tmpAF;void*_tmpCA;_LL6E: {struct Cyc_Dict_Absent_exn_struct*_tmpC9=(struct Cyc_Dict_Absent_exn_struct*)_tmpC8;if(_tmpC9->tag != Cyc_Dict_Absent)goto _LL70;}_LL6F:
# 298
 goto _LL6D;_LL70: _tmpCA=_tmpC8;_LL71:(void)_rethrow(_tmpCA);_LL6D:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp32B;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp32A;struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp329;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp328;struct _tuple14*_tmp327;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp45,(
(_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327->f1=(void*)((_tmp32B=_cycalloc(sizeof(*_tmp32B)),((_tmp32B[0]=((_tmp328.tag=0,((_tmp328.f1=(void*)((_tmp32A=_cycalloc(sizeof(*_tmp32A)),((_tmp32A[0]=((_tmp329.tag=1,((_tmp329.f1=vd,_tmp329)))),_tmp32A)))),_tmp328)))),_tmp32B)))),((_tmp327->f2=0,_tmp327)))))));}}
# 304
static int Cyc_Tc_is_main(struct _tuple0*n){
union Cyc_Absyn_Nmspace _tmpD1;struct _dyneither_ptr*_tmpD2;struct _tuple0*_tmpD0=n;_tmpD1=_tmpD0->f1;_tmpD2=_tmpD0->f2;{
union Cyc_Absyn_Nmspace _tmpD3=_tmpD1;_LL73: if((_tmpD3.Abs_n).tag != 2)goto _LL75;if((struct Cyc_List_List*)(_tmpD3.Abs_n).val != 0)goto _LL75;_LL74: {
# 308
const char*_tmp32C;return Cyc_strcmp((struct _dyneither_ptr)*_tmpD2,((_tmp32C="main",_tag_dyneither(_tmp32C,sizeof(char),5))))== 0;}_LL75:;_LL76:
 return 0;_LL72:;};}
# 313
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,int in_cinclude,struct Cyc_List_List**exports){
# 316
struct _dyneither_ptr*v=(*fd->name).f2;
# 320
if(fd->sc == Cyc_Absyn_ExternC  && !in_cinclude){
const char*_tmp32F;void*_tmp32E;(_tmp32E=0,Cyc_Tcutil_terr(loc,((_tmp32F="extern \"C\" functions cannot be implemented in Cyclone",_tag_dyneither(_tmp32F,sizeof(char),54))),_tag_dyneither(_tmp32E,sizeof(void*),0)));}
# 323
{union Cyc_Absyn_Nmspace _tmpD7=(*fd->name).f1;union Cyc_Absyn_Nmspace _tmpD8=_tmpD7;_LL78: if((_tmpD8.Rel_n).tag != 1)goto _LL7A;if((struct Cyc_List_List*)(_tmpD8.Rel_n).val != 0)goto _LL7A;_LL79:
# 326
(*fd->name).f1=Cyc_Absyn_Abs_n(te->ns,fd->sc == Cyc_Absyn_ExternC  || in_cinclude);
goto _LL77;_LL7A:;_LL7B:
# 329
 goto _LL77;_LL77:;}
# 332
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 334
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 336
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 339
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpD9=(void*)atts->hd;void*_tmpDA=_tmpD9;_LL7D: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmpDB=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmpDA;if(_tmpDB->tag != 7)goto _LL7F;}_LL7E:
 goto _LL80;_LL7F: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDC=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmpDA;if(_tmpDC->tag != 6)goto _LL81;}_LL80:
# 343
{const char*_tmp333;void*_tmp332[1];struct Cyc_String_pa_PrintArg_struct _tmp331;(_tmp331.tag=0,((_tmp331.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp332[0]=& _tmp331,Cyc_Tcutil_terr(loc,((_tmp333="bad attribute %s for function",_tag_dyneither(_tmp333,sizeof(char),30))),_tag_dyneither(_tmp332,sizeof(void*),1)))))));}
goto _LL7C;_LL81:;_LL82:
 goto _LL7C;_LL7C:;}}
# 349
{struct _handler_cons _tmpE0;_push_handler(& _tmpE0);{int _tmpE2=0;if(setjmp(_tmpE0.handler))_tmpE2=1;if(!_tmpE2){
{struct _tuple14*_tmpE3=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->ordinaries,v);
void*_tmpE4=(*_tmpE3).f1;void*_tmpE5=_tmpE4;void*_tmpE7;_LL84: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE6=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE5;if(_tmpE6->tag != 0)goto _LL86;else{_tmpE7=(void*)_tmpE6->f1;}}_LL85: {
# 353
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp336;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp335;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpEC=(_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp336.tag=2,((_tmp336.f1=fd,_tmp336)))),_tmp335)));
void*_tmpED=Cyc_Tcdecl_merge_binding(_tmpE7,(void*)_tmpEC,loc,Cyc_Tc_tc_msg);
if(_tmpED == (void*)& Cyc_Absyn_Unresolved_b_val)goto _LL83;
# 357
if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
if(!(_tmpED == _tmpE7  && (*_tmpE3).f2)){
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp33C;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp33B;struct _tuple14*_tmp33A;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A->f1=(void*)((_tmp33C=_cycalloc(sizeof(*_tmp33C)),((_tmp33C[0]=((_tmp33B.tag=0,((_tmp33B.f1=_tmpED,_tmp33B)))),_tmp33C)))),((_tmp33A->f2=1,_tmp33A)))))));}}
goto _LL83;}_LL86: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpE8=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpE5;if(_tmpE8->tag != 1)goto _LL88;}_LL87:
# 363
{const char*_tmp33F;void*_tmp33E;(_tmp33E=0,Cyc_Tcutil_warn(loc,((_tmp33F="function declaration shadows previous type declaration",_tag_dyneither(_tmp33F,sizeof(char),55))),_tag_dyneither(_tmp33E,sizeof(void*),0)));}
goto _LL83;_LL88: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpE9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpE5;if(_tmpE9->tag != 2)goto _LL8A;}_LL89:
# 366
{const char*_tmp342;void*_tmp341;(_tmp341=0,Cyc_Tcutil_warn(loc,((_tmp342="function declaration shadows previous datatype constructor",_tag_dyneither(_tmp342,sizeof(char),59))),_tag_dyneither(_tmp341,sizeof(void*),0)));}
goto _LL83;_LL8A: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpEA=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpE5;if(_tmpEA->tag != 4)goto _LL8C;}_LL8B:
 goto _LL8D;_LL8C: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpEB=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpE5;if(_tmpEB->tag != 3)goto _LL83;}_LL8D:
# 370
{const char*_tmp345;void*_tmp344;(_tmp344=0,Cyc_Tcutil_warn(loc,((_tmp345="function declaration shadows previous enum tag",_tag_dyneither(_tmp345,sizeof(char),47))),_tag_dyneither(_tmp344,sizeof(void*),0)));}
goto _LL83;_LL83:;}
# 350
;_pop_handler();}else{void*_tmpE1=(void*)_exn_thrown;void*_tmpFA=_tmpE1;void*_tmpFC;_LL8F: {struct Cyc_Dict_Absent_exn_struct*_tmpFB=(struct Cyc_Dict_Absent_exn_struct*)_tmpFA;if(_tmpFB->tag != Cyc_Dict_Absent)goto _LL91;}_LL90:
# 374
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp348;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp347;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpFD=(_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347[0]=((_tmp348.tag=2,((_tmp348.f1=fd,_tmp348)))),_tmp347)));
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp357;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp356;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp355;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp354;struct _tuple14*_tmp353;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,v,(
(_tmp353=_cycalloc(sizeof(*_tmp353)),((_tmp353->f1=(void*)((_tmp357=_cycalloc(sizeof(*_tmp357)),((_tmp357[0]=((_tmp354.tag=0,((_tmp354.f1=(void*)((_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356[0]=((_tmp355.tag=2,((_tmp355.f1=fd,_tmp355)))),_tmp356)))),_tmp354)))),_tmp357)))),((_tmp353->f2=0,_tmp353)))))));}
# 379
goto _LL8E;_LL91: _tmpFC=_tmpFA;_LL92:(void)_rethrow(_tmpFC);_LL8E:;}};}
# 383
if(in_cinclude)return;{
# 388
struct _RegionHandle _tmp105=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmp105;_push_region(fnrgn);{
struct Cyc_Tcenv_Fenv*_tmp106=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmp358;struct Cyc_Tcenv_Tenv*_tmp107=(_tmp358=_region_malloc(fnrgn,sizeof(*_tmp358)),((_tmp358->ns=te->ns,((_tmp358->ae=te->ae,((_tmp358->le=_tmp106,((_tmp358->allow_valueof=0,_tmp358)))))))));
# 392
struct _handler_cons _tmp108;_push_handler(& _tmp108);{int _tmp10A=0;if(setjmp(_tmp108.handler))_tmp10A=1;if(!_tmp10A){
Cyc_Tcstmt_tcStmt(_tmp107,fd->body,0);
# 396
Cyc_Tcenv_check_delayed_effects(_tmp107);
Cyc_Tcenv_check_delayed_constraints(_tmp107);
# 400
if(!Cyc_Tcenv_all_labels_resolved(_tmp107)){
const char*_tmp35B;void*_tmp35A;(_tmp35A=0,Cyc_Tcutil_terr(loc,((_tmp35B="function has goto statements to undefined labels",_tag_dyneither(_tmp35B,sizeof(char),49))),_tag_dyneither(_tmp35A,sizeof(void*),0)));}
# 393
;_pop_handler();}else{void*_tmp109=(void*)_exn_thrown;void*_tmp10E=_tmp109;void*_tmp110;void*_tmp111;_LL94: {struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp10F=(struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*)_tmp10E;if(_tmp10F->tag != Cyc_Tcutil_AbortTypeCheckingFunction)goto _LL96;}_LL95:
# 403
 goto _LL93;_LL96: _tmp110=_tmp10E;_LL97:
 Cyc_Core_rethrow(_tmp110);_LL98: _tmp111=_tmp10E;_LL99:(void)_rethrow(_tmp111);_LL93:;}};}
# 408
if(Cyc_Tc_is_main(fd->name)){
# 410
{void*_tmp113=Cyc_Tcutil_compress(fd->ret_type);void*_tmp114=_tmp113;enum Cyc_Absyn_Size_of _tmp117;_LL9B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp115=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp114;if(_tmp115->tag != 0)goto _LL9D;}_LL9C:
{const char*_tmp35E;void*_tmp35D;(_tmp35D=0,Cyc_Tcutil_warn(loc,((_tmp35E="main declared with return type void",_tag_dyneither(_tmp35E,sizeof(char),36))),_tag_dyneither(_tmp35D,sizeof(void*),0)));}goto _LL9A;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp116=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp114;if(_tmp116->tag != 6)goto _LL9F;else{_tmp117=_tmp116->f2;}}_LL9E:
 goto _LL9A;_LL9F:;_LLA0:
# 414
{const char*_tmp362;void*_tmp361[1];struct Cyc_String_pa_PrintArg_struct _tmp360;(_tmp360.tag=0,((_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type)),((_tmp361[0]=& _tmp360,Cyc_Tcutil_terr(loc,((_tmp362="main declared with return type %s instead of int or void",_tag_dyneither(_tmp362,sizeof(char),57))),_tag_dyneither(_tmp361,sizeof(void*),1)))))));}
goto _LL9A;_LL9A:;}
# 418
if(fd->c_varargs  || fd->cyc_varargs != 0){
const char*_tmp365;void*_tmp364;(_tmp364=0,Cyc_Tcutil_terr(loc,((_tmp365="main declared with varargs",_tag_dyneither(_tmp365,sizeof(char),27))),_tag_dyneither(_tmp364,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp11F=fd->args;
if(_tmp11F != 0){
struct _tuple8*_tmp120=(struct _tuple8*)_tmp11F->hd;void*_tmp122;struct _tuple8*_tmp121=_tmp120;_tmp122=_tmp121->f3;
{void*_tmp123=Cyc_Tcutil_compress(_tmp122);void*_tmp124=_tmp123;enum Cyc_Absyn_Size_of _tmp126;_LLA2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp125=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp124;if(_tmp125->tag != 6)goto _LLA4;else{_tmp126=_tmp125->f2;}}_LLA3:
 goto _LLA1;_LLA4:;_LLA5:
# 426
{const char*_tmp369;void*_tmp368[1];struct Cyc_String_pa_PrintArg_struct _tmp367;(_tmp367.tag=0,((_tmp367.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp122)),((_tmp368[0]=& _tmp367,Cyc_Tcutil_terr(loc,((_tmp369="main declared with first argument of type %s instead of int",_tag_dyneither(_tmp369,sizeof(char),60))),_tag_dyneither(_tmp368,sizeof(void*),1)))))));}
goto _LLA1;_LLA1:;}
# 430
_tmp11F=_tmp11F->tl;
if(_tmp11F != 0){
struct _tuple8*_tmp12A=(struct _tuple8*)_tmp11F->hd;void*_tmp12C;struct _tuple8*_tmp12B=_tmp12A;_tmp12C=_tmp12B->f3;
_tmp11F=_tmp11F->tl;
if(_tmp11F != 0){
const char*_tmp36C;void*_tmp36B;(_tmp36B=0,Cyc_Tcutil_terr(loc,((_tmp36C="main declared with too many arguments",_tag_dyneither(_tmp36C,sizeof(char),38))),_tag_dyneither(_tmp36B,sizeof(void*),0)));}{
struct Cyc_Core_Opt*_tmp36D;struct Cyc_Core_Opt*tvs=(_tmp36D=_cycalloc(sizeof(*_tmp36D)),((_tmp36D->v=fd->tvs,_tmp36D)));
struct Cyc_Core_Opt*_tmp375;struct Cyc_Core_Opt*_tmp374;struct Cyc_Core_Opt*_tmp373;struct Cyc_Core_Opt*_tmp372;struct Cyc_Core_Opt*_tmp371;struct Cyc_Core_Opt*_tmp370;struct Cyc_Core_Opt*_tmp36F;struct Cyc_Core_Opt*_tmp36E;if(((!Cyc_Tcutil_unify(_tmp12C,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp374=_cycalloc(sizeof(*_tmp374)),((_tmp374->v=& Cyc_Tcutil_rk,_tmp374)))),tvs)),
Cyc_Absyn_new_evar(((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375->v=& Cyc_Tcutil_rk,_tmp375)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmp12C,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp372=_cycalloc(sizeof(*_tmp372)),((_tmp372->v=& Cyc_Tcutil_rk,_tmp372)))),tvs)),
# 442
Cyc_Absyn_new_evar(((_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->v=& Cyc_Tcutil_rk,_tmp373)))),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp12C,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(((_tmp370=_cycalloc(sizeof(*_tmp370)),((_tmp370->v=& Cyc_Tcutil_rk,_tmp370)))),tvs)),
Cyc_Absyn_new_evar(((_tmp371=_cycalloc(sizeof(*_tmp371)),((_tmp371->v=& Cyc_Tcutil_rk,_tmp371)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmp12C,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(((_tmp36E=_cycalloc(sizeof(*_tmp36E)),((_tmp36E->v=& Cyc_Tcutil_rk,_tmp36E)))),tvs)),
# 449
Cyc_Absyn_new_evar(((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->v=& Cyc_Tcutil_rk,_tmp36F)))),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))){
const char*_tmp379;void*_tmp378[1];struct Cyc_String_pa_PrintArg_struct _tmp377;(_tmp377.tag=0,((_tmp377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp12C)),((_tmp378[0]=& _tmp377,Cyc_Tcutil_terr(loc,((_tmp379="second argument of main has type %s instead of char??",_tag_dyneither(_tmp379,sizeof(char),54))),_tag_dyneither(_tmp378,sizeof(void*),1)))))));}};}}};}
# 388
;_pop_region(fnrgn);};};}
# 459
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _dyneither_ptr*v=(*td->name).f2;
# 465
{union Cyc_Absyn_Nmspace _tmp13B=(*td->name).f1;union Cyc_Absyn_Nmspace _tmp13C=_tmp13B;_LLA7: if((_tmp13C.Rel_n).tag != 1)goto _LLA9;_LLA8:
# 468
(*td->name).f1=Cyc_Absyn_Abs_n(te->ns,0);
goto _LLA6;_LLA9:;_LLAA:
 goto _LLA6;_LLA6:;}
# 473
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(ge->typedefs,v)){
{const char*_tmp37D;void*_tmp37C[1];struct Cyc_String_pa_PrintArg_struct _tmp37B;(_tmp37B.tag=0,((_tmp37B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp37C[0]=& _tmp37B,Cyc_Tcutil_terr(loc,((_tmp37D="redeclaration of typedef %s",_tag_dyneither(_tmp37D,sizeof(char),28))),_tag_dyneither(_tmp37C,sizeof(void*),1)))))));}
return;}
# 478
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 487
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmp140=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmp141=_tmp140;struct Cyc_Core_Opt**_tmp143;struct Cyc_Core_Opt**_tmp145;struct Cyc_Absyn_Kind*_tmp146;_LLAC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp142=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp141;if(_tmp142->tag != 1)goto _LLAE;else{_tmp143=(struct Cyc_Core_Opt**)& _tmp142->f1;}}_LLAD:
# 490
 if(td->defn != 0){
const char*_tmp381;void*_tmp380[1];struct Cyc_String_pa_PrintArg_struct _tmp37F;(_tmp37F.tag=0,((_tmp37F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name),((_tmp380[0]=& _tmp37F,Cyc_Tcutil_warn(loc,((_tmp381="type variable %s is not used in typedef definition",_tag_dyneither(_tmp381,sizeof(char),51))),_tag_dyneither(_tmp380,sizeof(void*),1)))))));}
# 493
{struct Cyc_Core_Opt*_tmp382;*_tmp143=((_tmp382=_cycalloc(sizeof(*_tmp382)),((_tmp382->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk),_tmp382))));}goto _LLAB;_LLAE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp144=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141;if(_tmp144->tag != 2)goto _LLB0;else{_tmp145=(struct Cyc_Core_Opt**)& _tmp144->f1;_tmp146=_tmp144->f2;}}_LLAF:
# 500
{struct Cyc_Core_Opt*_tmp383;*_tmp145=((_tmp383=_cycalloc(sizeof(*_tmp383)),((_tmp383->v=Cyc_Tcutil_kind_to_bound(_tmp146),_tmp383))));}
goto _LLAB;_LLB0:;_LLB1:
 continue;_LLAB:;}}
# 506
ge->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(ge->typedefs,v,td);}
# 509
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 516
struct _RegionHandle _tmp14C=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp14C;_push_region(uprev_rgn);
# 518
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmp14D=(struct _tuple10*)rpo->hd;void*_tmp14F;void*_tmp150;struct _tuple10*_tmp14E=_tmp14D;_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,_tmp14F);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,_tmp150);}{
# 524
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 527
struct Cyc_List_List*_tmp151=fields;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct Cyc_Absyn_Aggrfield*_tmp152=(struct Cyc_Absyn_Aggrfield*)_tmp151->hd;struct _dyneither_ptr*_tmp154;struct Cyc_Absyn_Tqual _tmp155;void*_tmp156;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_List_List*_tmp158;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Aggrfield*_tmp153=_tmp152;_tmp154=_tmp153->name;_tmp155=_tmp153->tq;_tmp156=_tmp153->type;_tmp157=_tmp153->width;_tmp158=_tmp153->attributes;_tmp159=_tmp153->requires_clause;
# 530
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp154)){
const char*_tmp387;void*_tmp386[1];struct Cyc_String_pa_PrintArg_struct _tmp385;(_tmp385.tag=0,((_tmp385.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp154),((_tmp386[0]=& _tmp385,Cyc_Tcutil_terr(loc,((_tmp387="duplicate member %s",_tag_dyneither(_tmp387,sizeof(char),20))),_tag_dyneither(_tmp386,sizeof(void*),1)))))));}
# 534
{const char*_tmp388;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp154,((_tmp388="",_tag_dyneither(_tmp388,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp389;prev_fields=((_tmp389=_region_malloc(uprev_rgn,sizeof(*_tmp389)),((_tmp389->hd=_tmp154,((_tmp389->tl=prev_fields,_tmp389))))));}}{
# 537
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 541
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmp151->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,_tmp156);
# 546
(((struct Cyc_Absyn_Aggrfield*)_tmp151->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmp151->hd)->tq).print_const,_tmp156);
# 549
Cyc_Tcutil_check_bitfield(loc,te,_tmp156,_tmp157,_tmp154);
# 551
if((unsigned int)_tmp159){
if(str_or_union != Cyc_Absyn_UnionA){
const char*_tmp38C;void*_tmp38B;(_tmp38B=0,Cyc_Tcutil_terr(loc,((_tmp38C="@requires clauses are only allowed on union members",_tag_dyneither(_tmp38C,sizeof(char),52))),_tag_dyneither(_tmp38B,sizeof(void*),0)));}
{struct _RegionHandle _tmp161=_new_region("temp");struct _RegionHandle*temp=& _tmp161;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp162=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp162,0,_tmp159);}
# 555
;_pop_region(temp);}
# 558
if(!Cyc_Tcutil_is_integral(_tmp159)){
const char*_tmp390;void*_tmp38F[1];struct Cyc_String_pa_PrintArg_struct _tmp38E;(_tmp38E.tag=0,((_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 561
Cyc_Absynpp_typ2string((void*)_check_null(_tmp159->topt))),((_tmp38F[0]=& _tmp38E,Cyc_Tcutil_terr(_tmp159->loc,((_tmp390="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp390,sizeof(char),54))),_tag_dyneither(_tmp38F,sizeof(void*),1)))))));}else{
# 563
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp393;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp392;Cyc_Tcutil_check_type(_tmp159->loc,te,tvs,& Cyc_Tcutil_ik,0,(void*)((_tmp392=_cycalloc(sizeof(*_tmp392)),((_tmp392[0]=((_tmp393.tag=18,((_tmp393.f1=_tmp159,_tmp393)))),_tmp392)))));}{
# 565
struct Cyc_List_List*_tmp168=Cyc_Relations_exp2relns(uprev_rgn,_tmp159);
# 572
if(!Cyc_Relations_consistent_relations(_tmp168)){
const char*_tmp396;void*_tmp395;(_tmp395=0,Cyc_Tcutil_terr(_tmp159->loc,((_tmp396="@requires clause may be unsatisfiable",_tag_dyneither(_tmp396,sizeof(char),38))),_tag_dyneither(_tmp395,sizeof(void*),0)));}
# 578
{struct Cyc_List_List*_tmp16B=prev_relations;for(0;_tmp16B != 0;_tmp16B=_tmp16B->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp168,(struct Cyc_List_List*)_tmp16B->hd))){
const char*_tmp399;void*_tmp398;(_tmp398=0,Cyc_Tcutil_terr(_tmp159->loc,((_tmp399="@requires clause may overlap with previous clauses",_tag_dyneither(_tmp399,sizeof(char),51))),_tag_dyneither(_tmp398,sizeof(void*),0)));}}}{
# 583
struct Cyc_List_List*_tmp39A;prev_relations=((_tmp39A=_region_malloc(uprev_rgn,sizeof(*_tmp39A)),((_tmp39A->hd=_tmp168,((_tmp39A->tl=prev_relations,_tmp39A))))));};};}}else{
# 586
if(prev_relations != 0){
const char*_tmp39D;void*_tmp39C;(_tmp39C=0,Cyc_Tcutil_terr(loc,((_tmp39D="if one field has a @requires clause, they all must",_tag_dyneither(_tmp39D,sizeof(char),51))),_tag_dyneither(_tmp39C,sizeof(void*),0)));}}};}};
# 518
;_pop_region(uprev_rgn);}
# 593
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 595
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp171=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp172=_tmp171;struct Cyc_Core_Opt**_tmp174;struct Cyc_Core_Opt**_tmp176;enum Cyc_Absyn_AliasQual _tmp177;struct Cyc_Core_Opt**_tmp179;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Absyn_Kind*_tmp17C;enum Cyc_Absyn_AliasQual _tmp17E;_LLB3: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp173=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp172;if(_tmp173->tag != 1)goto _LLB5;else{_tmp174=(struct Cyc_Core_Opt**)& _tmp173->f1;}}_LLB4:
# 598
{struct Cyc_Core_Opt*_tmp39E;*_tmp174=((_tmp39E=_cycalloc(sizeof(*_tmp39E)),((_tmp39E->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp39E))));}continue;_LLB5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp175=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp172;if(_tmp175->tag != 2)goto _LLB7;else{_tmp176=(struct Cyc_Core_Opt**)& _tmp175->f1;if((_tmp175->f2)->kind != Cyc_Absyn_MemKind)goto _LLB7;_tmp177=(_tmp175->f2)->aliasqual;}}_LLB6:
# 600
 if(constrain_top_kind  && _tmp177 == Cyc_Absyn_Top){
struct Cyc_Absyn_Kind*_tmp3A1;struct Cyc_Core_Opt*_tmp3A0;*_tmp176=((_tmp3A0=_cycalloc(sizeof(*_tmp3A0)),((_tmp3A0->v=Cyc_Tcutil_kind_to_bound(((_tmp3A1=_cycalloc_atomic(sizeof(*_tmp3A1)),((_tmp3A1->kind=Cyc_Absyn_BoxKind,((_tmp3A1->aliasqual=Cyc_Absyn_Aliasable,_tmp3A1))))))),_tmp3A0))));}else{
# 603
struct Cyc_Absyn_Kind*_tmp3A4;struct Cyc_Core_Opt*_tmp3A3;*_tmp176=((_tmp3A3=_cycalloc(sizeof(*_tmp3A3)),((_tmp3A3->v=Cyc_Tcutil_kind_to_bound(((_tmp3A4=_cycalloc_atomic(sizeof(*_tmp3A4)),((_tmp3A4->kind=Cyc_Absyn_BoxKind,((_tmp3A4->aliasqual=_tmp177,_tmp3A4))))))),_tmp3A3))));}
continue;_LLB7:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp178=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp172;if(_tmp178->tag != 2)goto _LLB9;else{_tmp179=(struct Cyc_Core_Opt**)& _tmp178->f1;if((_tmp178->f2)->kind != Cyc_Absyn_BoxKind)goto _LLB9;if((_tmp178->f2)->aliasqual != Cyc_Absyn_Top)goto _LLB9;}}if(!constrain_top_kind)goto _LLB9;_LLB8:
# 606
{struct Cyc_Absyn_Kind*_tmp3A7;struct Cyc_Core_Opt*_tmp3A6;*_tmp179=((_tmp3A6=_cycalloc(sizeof(*_tmp3A6)),((_tmp3A6->v=Cyc_Tcutil_kind_to_bound(((_tmp3A7=_cycalloc_atomic(sizeof(*_tmp3A7)),((_tmp3A7->kind=Cyc_Absyn_BoxKind,((_tmp3A7->aliasqual=Cyc_Absyn_Aliasable,_tmp3A7))))))),_tmp3A6))));}
continue;_LLB9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp172;if(_tmp17A->tag != 2)goto _LLBB;else{_tmp17B=(struct Cyc_Core_Opt**)& _tmp17A->f1;_tmp17C=_tmp17A->f2;}}_LLBA:
# 609
{struct Cyc_Core_Opt*_tmp3A8;*_tmp17B=((_tmp3A8=_cycalloc(sizeof(*_tmp3A8)),((_tmp3A8->v=Cyc_Tcutil_kind_to_bound(_tmp17C),_tmp3A8))));}continue;_LLBB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp17D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp172;if(_tmp17D->tag != 0)goto _LLBD;else{if((_tmp17D->f1)->kind != Cyc_Absyn_MemKind)goto _LLBD;_tmp17E=(_tmp17D->f1)->aliasqual;}}_LLBC:
# 611
{const char*_tmp3B0;void*_tmp3AF[3];struct Cyc_String_pa_PrintArg_struct _tmp3AE;struct Cyc_String_pa_PrintArg_struct _tmp3AD;struct Cyc_Absyn_Kind*_tmp3AC;struct Cyc_String_pa_PrintArg_struct _tmp3AB;(_tmp3AB.tag=0,((_tmp3AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp3AC=_cycalloc_atomic(sizeof(*_tmp3AC)),((_tmp3AC->kind=Cyc_Absyn_MemKind,((_tmp3AC->aliasqual=_tmp17E,_tmp3AC)))))))),((_tmp3AD.tag=0,((_tmp3AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3AE.tag=0,((_tmp3AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3AF[0]=& _tmp3AE,((_tmp3AF[1]=& _tmp3AD,((_tmp3AF[2]=& _tmp3AB,Cyc_Tcutil_terr(loc,((_tmp3B0="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3B0,sizeof(char),57))),_tag_dyneither(_tmp3AF,sizeof(void*),3)))))))))))))))))));}
continue;_LLBD:;_LLBE:
 continue;_LLB2:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 619
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _dyneither_ptr*_tmp18D=(*ad->name).f2;
# 626
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp18E=(void*)atts->hd;void*_tmp18F=_tmp18E;_LLC0: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp190=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp18F;if(_tmp190->tag != 7)goto _LLC2;}_LLC1:
 goto _LLC3;_LLC2: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp191=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp18F;if(_tmp191->tag != 6)goto _LLC4;}_LLC3:
 continue;_LLC4:;_LLC5:
# 631
{const char*_tmp3B5;void*_tmp3B4[2];struct Cyc_String_pa_PrintArg_struct _tmp3B3;struct Cyc_String_pa_PrintArg_struct _tmp3B2;(_tmp3B2.tag=0,((_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp18D),((_tmp3B3.tag=0,((_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp3B4[0]=& _tmp3B3,((_tmp3B4[1]=& _tmp3B2,Cyc_Tcutil_terr(loc,((_tmp3B5="bad attribute %s in %s definition",_tag_dyneither(_tmp3B5,sizeof(char),34))),_tag_dyneither(_tmp3B4,sizeof(void*),2)))))))))))));}
goto _LLBF;_LLBF:;}}{
# 636
struct Cyc_List_List*_tmp196=ad->tvs;
# 638
{union Cyc_Absyn_Nmspace _tmp197=(*ad->name).f1;union Cyc_Absyn_Nmspace _tmp198=_tmp197;_LLC7: if((_tmp198.Rel_n).tag != 1)goto _LLC9;if((struct Cyc_List_List*)(_tmp198.Rel_n).val != 0)goto _LLC9;_LLC8:
# 641
(*ad->name).f1=Cyc_Absyn_Abs_n(te->ns,ad->sc == Cyc_Absyn_ExternC);
goto _LLC6;_LLC9:;_LLCA:
# 644
 goto _LLC6;_LLC6:;}
# 648
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
# 650
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 654
{struct _tuple15 _tmp3B6;struct _tuple15 _tmp199=(_tmp3B6.f1=ad->impl,((_tmp3B6.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->aggrdecls,_tmp18D),_tmp3B6)));struct _tuple15 _tmp19A=_tmp199;struct Cyc_List_List*_tmp19B;struct Cyc_List_List*_tmp19C;struct Cyc_List_List*_tmp19D;int _tmp19E;struct Cyc_List_List*_tmp19F;struct Cyc_List_List*_tmp1A0;struct Cyc_List_List*_tmp1A1;int _tmp1A2;struct Cyc_Absyn_Aggrdecl**_tmp1A3;struct Cyc_Absyn_Aggrdecl**_tmp1A4;_LLCC: if(_tmp19A.f1 != 0)goto _LLCE;if(_tmp19A.f2 != 0)goto _LLCE;_LLCD:
# 657
 Cyc_Tc_rule_out_memkind(loc,_tmp18D,_tmp196,0);
# 659
{struct Cyc_Absyn_Aggrdecl**_tmp3B7;ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp18D,((_tmp3B7=_cycalloc(sizeof(*_tmp3B7)),((_tmp3B7[0]=ad,_tmp3B7)))));}
goto _LLCB;_LLCE: if(_tmp19A.f1 == 0)goto _LLD0;_tmp19B=(_tmp19A.f1)->exist_vars;_tmp19C=(_tmp19A.f1)->rgn_po;_tmp19D=(_tmp19A.f1)->fields;_tmp19E=(_tmp19A.f1)->tagged;if(_tmp19A.f2 != 0)goto _LLD0;_LLCF: {
# 664
struct Cyc_Absyn_Aggrdecl*_tmp3BA;struct Cyc_Absyn_Aggrdecl**_tmp3B9;struct Cyc_Absyn_Aggrdecl**_tmp1A6=(_tmp3B9=_cycalloc(sizeof(*_tmp3B9)),((_tmp3B9[0]=((_tmp3BA=_cycalloc(sizeof(*_tmp3BA)),((_tmp3BA->kind=ad->kind,((_tmp3BA->sc=Cyc_Absyn_Extern,((_tmp3BA->name=ad->name,((_tmp3BA->tvs=_tmp196,((_tmp3BA->impl=0,((_tmp3BA->attributes=ad->attributes,_tmp3BA)))))))))))))),_tmp3B9)));
# 666
ge->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(ge->aggrdecls,_tmp18D,_tmp1A6);
# 671
Cyc_Tcutil_check_unique_tvars(loc,_tmp19B);
# 673
Cyc_Tcutil_add_tvar_identities(_tmp19B);
# 676
if(_tmp19E  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3BD;void*_tmp3BC;(_tmp3BC=0,Cyc_Tcutil_terr(loc,((_tmp3BD="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3BD,sizeof(char),56))),_tag_dyneither(_tmp3BC,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp196,_tmp19B),_tmp19C,_tmp19D);
# 680
Cyc_Tc_rule_out_memkind(loc,_tmp18D,_tmp196,0);
# 683
Cyc_Tc_rule_out_memkind(loc,_tmp18D,_tmp19B,1);
# 685
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp19E){
# 688
struct Cyc_List_List*f=_tmp19D;for(0;f != 0;f=f->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0){
const char*_tmp3C3;void*_tmp3C2[3];struct Cyc_String_pa_PrintArg_struct _tmp3C1;struct Cyc_String_pa_PrintArg_struct _tmp3C0;struct Cyc_String_pa_PrintArg_struct _tmp3BF;(_tmp3BF.tag=0,((_tmp3BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)),((_tmp3C0.tag=0,((_tmp3C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp18D),((_tmp3C1.tag=0,((_tmp3C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name),((_tmp3C2[0]=& _tmp3C1,((_tmp3C2[1]=& _tmp3C0,((_tmp3C2[2]=& _tmp3BF,Cyc_Tcutil_warn(loc,((_tmp3C3="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp3C3,sizeof(char),98))),_tag_dyneither(_tmp3C2,sizeof(void*),3)))))))))))))))))));}}}
# 693
*_tmp1A6=ad;
goto _LLCB;}_LLD0: if(_tmp19A.f1 == 0)goto _LLD2;_tmp19F=(_tmp19A.f1)->exist_vars;_tmp1A0=(_tmp19A.f1)->rgn_po;_tmp1A1=(_tmp19A.f1)->fields;_tmp1A2=(_tmp19A.f1)->tagged;if(_tmp19A.f2 == 0)goto _LLD2;_tmp1A3=*_tmp19A.f2;_LLD1:
# 697
 if(ad->kind != (*_tmp1A3)->kind){
const char*_tmp3C6;void*_tmp3C5;(_tmp3C5=0,Cyc_Tcutil_terr(loc,((_tmp3C6="cannot reuse struct names for unions and vice-versa",_tag_dyneither(_tmp3C6,sizeof(char),52))),_tag_dyneither(_tmp3C5,sizeof(void*),0)));}{
# 700
struct Cyc_Absyn_Aggrdecl*_tmp1B2=*_tmp1A3;
# 702
{struct Cyc_Absyn_Aggrdecl*_tmp3C7;*_tmp1A3=((_tmp3C7=_cycalloc(sizeof(*_tmp3C7)),((_tmp3C7->kind=ad->kind,((_tmp3C7->sc=Cyc_Absyn_Extern,((_tmp3C7->name=ad->name,((_tmp3C7->tvs=_tmp196,((_tmp3C7->impl=0,((_tmp3C7->attributes=ad->attributes,_tmp3C7))))))))))))));}
# 708
Cyc_Tcutil_check_unique_tvars(loc,_tmp19F);
# 710
Cyc_Tcutil_add_tvar_identities(_tmp19F);
# 712
if(_tmp1A2  && ad->kind == Cyc_Absyn_StructA){
const char*_tmp3CA;void*_tmp3C9;(_tmp3C9=0,Cyc_Tcutil_terr(loc,((_tmp3CA="@tagged qualifier is only allowed on union declarations",_tag_dyneither(_tmp3CA,sizeof(char),56))),_tag_dyneither(_tmp3C9,sizeof(void*),0)));}
Cyc_Tc_tcAggrImpl(te,ge,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp196,_tmp19F),_tmp1A0,_tmp1A1);
# 716
*_tmp1A3=_tmp1B2;
_tmp1A4=_tmp1A3;goto _LLD3;};_LLD2: if(_tmp19A.f1 != 0)goto _LLCB;if(_tmp19A.f2 == 0)goto _LLCB;_tmp1A4=*_tmp19A.f2;_LLD3: {
# 720
struct Cyc_Absyn_Aggrdecl*_tmp1B6=Cyc_Tcdecl_merge_aggrdecl(*_tmp1A4,ad,loc,Cyc_Tc_tc_msg);
if(_tmp1B6 == 0)
return;else{
# 724
Cyc_Tc_rule_out_memkind(loc,_tmp18D,_tmp196,0);
# 727
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp18D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 730
*_tmp1A4=_tmp1B6;
ad=_tmp1B6;
goto _LLCB;}}_LLCB:;}{
# 736
struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3D0;struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp3CF;struct _tuple14*_tmp3CE;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,_tmp18D,(
(_tmp3CE=_cycalloc(sizeof(*_tmp3CE)),((_tmp3CE->f1=(void*)((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=((_tmp3CF.tag=1,((_tmp3CF.f1=ad,_tmp3CF)))),_tmp3D0)))),((_tmp3CE->f2=1,_tmp3CE)))))));};};}
# 740
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _dyneither_ptr*v,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp1BB=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp1BC=_tmp1BB;struct Cyc_Core_Opt**_tmp1BE;struct Cyc_Core_Opt**_tmp1C0;struct Cyc_Core_Opt**_tmp1C2;struct Cyc_Core_Opt**_tmp1C4;struct Cyc_Core_Opt**_tmp1C6;struct Cyc_Core_Opt**_tmp1C8;struct Cyc_Core_Opt**_tmp1CB;enum Cyc_Absyn_KindQual _tmp1CC;enum Cyc_Absyn_KindQual _tmp1CE;enum Cyc_Absyn_AliasQual _tmp1D0;_LLD5: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1BD=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1BD->tag != 1)goto _LLD7;else{_tmp1BE=(struct Cyc_Core_Opt**)& _tmp1BD->f1;}}_LLD6:
 _tmp1C0=_tmp1BE;goto _LLD8;_LLD7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1BF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1BF->tag != 2)goto _LLD9;else{_tmp1C0=(struct Cyc_Core_Opt**)& _tmp1BF->f1;if((_tmp1BF->f2)->kind != Cyc_Absyn_MemKind)goto _LLD9;if((_tmp1BF->f2)->aliasqual != Cyc_Absyn_Top)goto _LLD9;}}_LLD8:
# 745
 _tmp1C2=_tmp1C0;goto _LLDA;_LLD9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1C1->tag != 2)goto _LLDB;else{_tmp1C2=(struct Cyc_Core_Opt**)& _tmp1C1->f1;if((_tmp1C1->f2)->kind != Cyc_Absyn_MemKind)goto _LLDB;if((_tmp1C1->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDB;}}_LLDA:
# 747
{struct Cyc_Core_Opt*_tmp3D1;*_tmp1C2=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk),_tmp3D1))));}goto _LLD4;_LLDB: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1C3->tag != 2)goto _LLDD;else{_tmp1C4=(struct Cyc_Core_Opt**)& _tmp1C3->f1;if((_tmp1C3->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDD;if((_tmp1C3->f2)->aliasqual != Cyc_Absyn_Top)goto _LLDD;}}_LLDC:
 _tmp1C6=_tmp1C4;goto _LLDE;_LLDD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1C5->tag != 2)goto _LLDF;else{_tmp1C6=(struct Cyc_Core_Opt**)& _tmp1C5->f1;if((_tmp1C5->f2)->kind != Cyc_Absyn_AnyKind)goto _LLDF;if((_tmp1C5->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LLDF;}}_LLDE:
# 750
{struct Cyc_Core_Opt*_tmp3D2;*_tmp1C6=((_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak),_tmp3D2))));}goto _LLD4;_LLDF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1C7=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1C7->tag != 2)goto _LLE1;else{_tmp1C8=(struct Cyc_Core_Opt**)& _tmp1C7->f1;if((_tmp1C7->f2)->kind != Cyc_Absyn_RgnKind)goto _LLE1;if((_tmp1C7->f2)->aliasqual != Cyc_Absyn_Top)goto _LLE1;}}_LLE0:
# 752
{struct Cyc_Core_Opt*_tmp3D3;*_tmp1C8=((_tmp3D3=_cycalloc(sizeof(*_tmp3D3)),((_tmp3D3->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp3D3))));}goto _LLD4;_LLE1: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1C9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1C9->tag != 0)goto _LLE3;else{if((_tmp1C9->f1)->kind != Cyc_Absyn_RgnKind)goto _LLE3;if((_tmp1C9->f1)->aliasqual != Cyc_Absyn_Top)goto _LLE3;}}_LLE2:
# 754
{const char*_tmp3D8;void*_tmp3D7[2];struct Cyc_String_pa_PrintArg_struct _tmp3D6;struct Cyc_String_pa_PrintArg_struct _tmp3D5;(_tmp3D5.tag=0,((_tmp3D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3D6.tag=0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D7[0]=& _tmp3D6,((_tmp3D7[1]=& _tmp3D5,Cyc_Tcutil_terr(loc,((_tmp3D8="type %s attempts to abstract type variable %s of kind TR",_tag_dyneither(_tmp3D8,sizeof(char),57))),_tag_dyneither(_tmp3D7,sizeof(void*),2)))))))))))));}
goto _LLD4;_LLE3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1CA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1CA->tag != 2)goto _LLE5;else{_tmp1CB=(struct Cyc_Core_Opt**)& _tmp1CA->f1;_tmp1CC=(_tmp1CA->f2)->kind;if((_tmp1CA->f2)->aliasqual != Cyc_Absyn_Unique)goto _LLE5;}}_LLE4:
 _tmp1CE=_tmp1CC;goto _LLE6;_LLE5: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1CD->tag != 0)goto _LLE7;else{_tmp1CE=(_tmp1CD->f1)->kind;if((_tmp1CD->f1)->aliasqual != Cyc_Absyn_Unique)goto _LLE7;}}_LLE6:
# 758
{const char*_tmp3E0;void*_tmp3DF[3];struct Cyc_String_pa_PrintArg_struct _tmp3DE;struct Cyc_String_pa_PrintArg_struct _tmp3DD;struct Cyc_Absyn_Kind*_tmp3DC;struct Cyc_String_pa_PrintArg_struct _tmp3DB;(_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 760
Cyc_Absynpp_kind2string(((_tmp3DC=_cycalloc_atomic(sizeof(*_tmp3DC)),((_tmp3DC->kind=_tmp1CE,((_tmp3DC->aliasqual=Cyc_Absyn_Unique,_tmp3DC)))))))),((_tmp3DD.tag=0,((_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3DF[0]=& _tmp3DE,((_tmp3DF[1]=& _tmp3DD,((_tmp3DF[2]=& _tmp3DB,Cyc_Tcutil_terr(loc,((_tmp3E0="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3E0,sizeof(char),57))),_tag_dyneither(_tmp3DF,sizeof(void*),3)))))))))))))))))));}goto _LLD4;_LLE7: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1CF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1BC;if(_tmp1CF->tag != 0)goto _LLE9;else{if((_tmp1CF->f1)->kind != Cyc_Absyn_MemKind)goto _LLE9;_tmp1D0=(_tmp1CF->f1)->aliasqual;}}_LLE8:
# 762
{const char*_tmp3E8;void*_tmp3E7[3];struct Cyc_String_pa_PrintArg_struct _tmp3E6;struct Cyc_String_pa_PrintArg_struct _tmp3E5;struct Cyc_Absyn_Kind*_tmp3E4;struct Cyc_String_pa_PrintArg_struct _tmp3E3;(_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 764
Cyc_Absynpp_kind2string(((_tmp3E4=_cycalloc_atomic(sizeof(*_tmp3E4)),((_tmp3E4->kind=Cyc_Absyn_MemKind,((_tmp3E4->aliasqual=_tmp1D0,_tmp3E4)))))))),((_tmp3E5.tag=0,((_tmp3E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name),((_tmp3E6.tag=0,((_tmp3E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3E7[0]=& _tmp3E6,((_tmp3E7[1]=& _tmp3E5,((_tmp3E7[2]=& _tmp3E3,Cyc_Tcutil_terr(loc,((_tmp3E8="type %s attempts to abstract type variable %s of kind %s",_tag_dyneither(_tmp3E8,sizeof(char),57))),_tag_dyneither(_tmp3E7,sizeof(void*),3)))))))))))))))))));}goto _LLD4;_LLE9:;_LLEA:
 goto _LLD4;_LLD4:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 771
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 780
{struct Cyc_List_List*_tmp1E4=fields;for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
struct Cyc_Absyn_Datatypefield*_tmp1E5=(struct Cyc_Absyn_Datatypefield*)_tmp1E4->hd;
# 783
{struct Cyc_List_List*typs=_tmp1E5->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp1E5->loc,te,tvs,& Cyc_Tcutil_tmk,0,(*((struct _tuple16*)typs->hd)).f2);{
# 786
struct _RegionHandle _tmp1E6=_new_region("temp");struct _RegionHandle*temp=& _tmp1E6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2)){
const char*_tmp3EC;void*_tmp3EB[1];struct Cyc_String_pa_PrintArg_struct _tmp3EA;(_tmp3EA.tag=0,((_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 790
Cyc_Absynpp_qvar2string(_tmp1E5->name)),((_tmp3EB[0]=& _tmp3EA,Cyc_Tcutil_terr(_tmp1E5->loc,((_tmp3EC="noalias pointers in datatypes are not allowed (%s)",_tag_dyneither(_tmp3EC,sizeof(char),51))),_tag_dyneither(_tmp3EB,sizeof(void*),1)))))));}
# 792
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp1E5->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 786
;_pop_region(temp);};}}{
# 797
union Cyc_Absyn_Nmspace _tmp1EA=(*_tmp1E5->name).f1;union Cyc_Absyn_Nmspace _tmp1EB=_tmp1EA;_LLEC: if((_tmp1EB.Rel_n).tag != 1)goto _LLEE;if((struct Cyc_List_List*)(_tmp1EB.Rel_n).val != 0)goto _LLEE;_LLED:
# 799
 if(is_extensible)
(*_tmp1E5->name).f1=Cyc_Absyn_Abs_n(te->ns,0);else{
# 802
(*_tmp1E5->name).f1=(*name).f1;}
goto _LLEB;_LLEE: if((_tmp1EB.Rel_n).tag != 1)goto _LLF0;_LLEF:
# 805
{const char*_tmp3F0;void*_tmp3EF[1];struct Cyc_String_pa_PrintArg_struct _tmp3EE;(_tmp3EE.tag=0,((_tmp3EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp1E5->name)),((_tmp3EF[0]=& _tmp3EE,Cyc_Tcutil_terr(_tmp1E5->loc,((_tmp3F0="qualified datatypefield declarations are not allowed (%s)",_tag_dyneither(_tmp3F0,sizeof(char),58))),_tag_dyneither(_tmp3EF,sizeof(void*),1)))))));}
goto _LLEB;_LLF0: if((_tmp1EB.C_n).tag != 3)goto _LLF2;_LLF1:
 goto _LLEB;_LLF2: if((_tmp1EB.Abs_n).tag != 2)goto _LLF4;_LLF3:
 goto _LLEB;_LLF4: if((_tmp1EB.Loc_n).tag != 4)goto _LLEB;_LLF5: {
const char*_tmp3F3;void*_tmp3F2;(_tmp3F2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3F3="tcDatatypeFields: Loc_n",_tag_dyneither(_tmp3F3,sizeof(char),24))),_tag_dyneither(_tmp3F2,sizeof(void*),0)));}_LLEB:;};}}{
# 814
struct Cyc_List_List*fields2;
if(is_extensible){
# 817
int _tmp1F1=1;
struct Cyc_List_List*_tmp1F2=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp1F1,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp1F1)
fields2=_tmp1F2;else{
# 822
fields2=0;}}else{
# 824
struct _RegionHandle _tmp1F3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1F3;_push_region(uprev_rgn);
# 826
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp1F4=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1F4->name).f2)){
const char*_tmp3F8;void*_tmp3F7[2];struct Cyc_String_pa_PrintArg_struct _tmp3F6;struct Cyc_String_pa_PrintArg_struct _tmp3F5;(_tmp3F5.tag=0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj),((_tmp3F6.tag=0,((_tmp3F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F4->name).f2),((_tmp3F7[0]=& _tmp3F6,((_tmp3F7[1]=& _tmp3F5,Cyc_Tcutil_terr(_tmp1F4->loc,((_tmp3F8="duplicate field name %s in %s",_tag_dyneither(_tmp3F8,sizeof(char),30))),_tag_dyneither(_tmp3F7,sizeof(void*),2)))))))))))));}else{
# 832
struct Cyc_List_List*_tmp3F9;prev_fields=((_tmp3F9=_region_malloc(uprev_rgn,sizeof(*_tmp3F9)),((_tmp3F9->hd=(*_tmp1F4->name).f2,((_tmp3F9->tl=prev_fields,_tmp3F9))))));}
# 834
if(_tmp1F4->sc != Cyc_Absyn_Public){
{const char*_tmp3FD;void*_tmp3FC[1];struct Cyc_String_pa_PrintArg_struct _tmp3FB;(_tmp3FB.tag=0,((_tmp3FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1F4->name).f2),((_tmp3FC[0]=& _tmp3FB,Cyc_Tcutil_warn(loc,((_tmp3FD="ignoring scope of field %s",_tag_dyneither(_tmp3FD,sizeof(char),27))),_tag_dyneither(_tmp3FC,sizeof(void*),1)))))));}
_tmp1F4->sc=Cyc_Absyn_Public;}}}
# 839
fields2=fields;}
# 826
;_pop_region(uprev_rgn);}
# 845
{struct Cyc_List_List*_tmp1FD=fields;for(0;_tmp1FD != 0;_tmp1FD=_tmp1FD->tl){
struct Cyc_Absyn_Datatypefield*_tmp1FE=(struct Cyc_Absyn_Datatypefield*)_tmp1FD->hd;
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp403;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp402;struct _tuple14*_tmp401;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp1FE->name).f2,(
(_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->f1=(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp402.tag=2,((_tmp402.f1=tudres,((_tmp402.f2=_tmp1FE,_tmp402)))))),_tmp403)))),((_tmp401->f2=1,_tmp401)))))));}}
# 851
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 854
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _dyneither_ptr*v=(*tud->name).f2;
const char*_tmp405;const char*_tmp404;struct _dyneither_ptr obj=tud->is_extensible?(_tmp405="@extensible datatype",_tag_dyneither(_tmp405,sizeof(char),21)):((_tmp404="datatype",_tag_dyneither(_tmp404,sizeof(char),9)));
# 861
struct Cyc_List_List*tvs=tud->tvs;
# 864
Cyc_Tcutil_check_unique_tvars(loc,tvs);
# 866
Cyc_Tcutil_add_tvar_identities(tvs);{
# 871
struct _RegionHandle _tmp202=_new_region("temp");struct _RegionHandle*temp=& _tmp202;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 881 "tc.cyc"
{struct _handler_cons _tmp203;_push_handler(& _tmp203);{int _tmp205=0;if(setjmp(_tmp203.handler))_tmp205=1;if(!_tmp205){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible){struct Cyc_Dict_Absent_exn_struct _tmp408;struct Cyc_Dict_Absent_exn_struct*_tmp407;(int)_throw((void*)((_tmp407=_cycalloc_atomic(sizeof(*_tmp407)),((_tmp407[0]=((_tmp408.tag=Cyc_Dict_Absent,_tmp408)),_tmp407)))));}
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 887
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 882
;_pop_handler();}else{void*_tmp204=(void*)_exn_thrown;void*_tmp209=_tmp204;void*_tmp20B;_LLF7: {struct Cyc_Dict_Absent_exn_struct*_tmp20A=(struct Cyc_Dict_Absent_exn_struct*)_tmp209;if(_tmp20A->tag != Cyc_Dict_Absent)goto _LLF9;}_LLF8:
# 891
{union Cyc_Absyn_Nmspace _tmp20C=(*tud->name).f1;union Cyc_Absyn_Nmspace _tmp20D=_tmp20C;_LLFC: if((_tmp20D.Rel_n).tag != 1)goto _LLFE;if((struct Cyc_List_List*)(_tmp20D.Rel_n).val != 0)goto _LLFE;_LLFD:
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LLFB;_LLFE:;_LLFF:
 goto _LLFB;_LLFB:;}{
# 895
struct Cyc_Absyn_Datatypedecl***_tmp20E=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->datatypedecls,v);
{struct Cyc_Absyn_Datatypedecl***_tmp409;tud_opt=(unsigned int)_tmp20E?(_tmp409=_region_malloc(temp,sizeof(*_tmp409)),((_tmp409[0]=*_tmp20E,_tmp409))): 0;}
goto _LLF6;};_LLF9: _tmp20B=_tmp209;_LLFA:(void)_rethrow(_tmp20B);_LLF6:;}};}{
# 902
struct _tuple17 _tmp40A;struct _tuple17 _tmp210=(_tmp40A.f1=tud->fields,((_tmp40A.f2=tud_opt,_tmp40A)));struct _tuple17 _tmp211=_tmp210;struct Cyc_List_List**_tmp212;struct Cyc_List_List**_tmp213;struct Cyc_Absyn_Datatypedecl**_tmp214;struct Cyc_Absyn_Datatypedecl**_tmp215;_LL101: if(_tmp211.f1 != 0)goto _LL103;if(_tmp211.f2 != 0)goto _LL103;_LL102:
# 905
 Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
{struct Cyc_Absyn_Datatypedecl**_tmp40B;ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=tud,_tmp40B)))));}
goto _LL100;_LL103: if(_tmp211.f1 == 0)goto _LL105;_tmp212=(struct Cyc_List_List**)&(_tmp211.f1)->v;if(_tmp211.f2 != 0)goto _LL105;_LL104: {
# 910
struct Cyc_Absyn_Datatypedecl*_tmp40E;struct Cyc_Absyn_Datatypedecl**_tmp40D;struct Cyc_Absyn_Datatypedecl**_tmp217=(_tmp40D=_cycalloc(sizeof(*_tmp40D)),((_tmp40D[0]=((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E->sc=Cyc_Absyn_Extern,((_tmp40E->name=tud->name,((_tmp40E->tvs=tvs,((_tmp40E->fields=0,((_tmp40E->is_extensible=tud->is_extensible,_tmp40E)))))))))))),_tmp40D)));
# 912
ge->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(ge->datatypedecls,v,_tmp217);
# 915
*_tmp212=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp212,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
*_tmp217=tud;
goto _LL100;}_LL105: if(_tmp211.f1 == 0)goto _LL107;_tmp213=(struct Cyc_List_List**)&(_tmp211.f1)->v;if(_tmp211.f2 == 0)goto _LL107;_tmp214=*_tmp211.f2;_LL106: {
# 920
struct Cyc_Absyn_Datatypedecl*_tmp21A=*_tmp214;
# 923
if((!tud->is_extensible  && (unsigned int)_tmp21A) && _tmp21A->is_extensible)
tud->is_extensible=1;
# 926
{struct Cyc_Absyn_Datatypedecl*_tmp40F;*_tmp214=((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F->sc=Cyc_Absyn_Extern,((_tmp40F->name=tud->name,((_tmp40F->tvs=tvs,((_tmp40F->fields=0,((_tmp40F->is_extensible=tud->is_extensible,_tmp40F))))))))))));}
# 930
*_tmp213=Cyc_Tc_tcDatatypeFields(te,ge,loc,obj,tud->is_extensible,tud->name,*_tmp213,tvs,tud);
# 934
*_tmp214=_tmp21A;
_tmp215=_tmp214;goto _LL108;}_LL107: if(_tmp211.f1 != 0)goto _LL100;if(_tmp211.f2 == 0)goto _LL100;_tmp215=*_tmp211.f2;_LL108: {
# 938
struct Cyc_Absyn_Datatypedecl*_tmp21C=Cyc_Tcdecl_merge_datatypedecl(*_tmp215,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,v,tvs);
if(_tmp21C == 0){
_npop_handler(0);return;}else{
# 943
*_tmp215=_tmp21C;
goto _LL100;}}_LL100:;};}
# 872 "tc.cyc"
;_pop_region(temp);};}
# 950 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Genv*ge,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _dyneither_ptr*v=(*ed->name).f2;
# 957
{union Cyc_Absyn_Nmspace _tmp220=(*ed->name).f1;union Cyc_Absyn_Nmspace _tmp221=_tmp220;_LL10A: if((_tmp221.Rel_n).tag != 1)goto _LL10C;if((struct Cyc_List_List*)(_tmp221.Rel_n).val != 0)goto _LL10C;_LL10B:
# 960
(*ed->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);
goto _LL109;_LL10C:;_LL10D:
 goto _LL109;_LL109:;}
# 966
if(ed->fields != 0){struct _RegionHandle _tmp222=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp222;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp223=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 972
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp223->name).f2)){
const char*_tmp413;void*_tmp412[1];struct Cyc_String_pa_PrintArg_struct _tmp411;(_tmp411.tag=0,((_tmp411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp223->name).f2),((_tmp412[0]=& _tmp411,Cyc_Tcutil_terr(_tmp223->loc,((_tmp413="duplicate enum constructor %s",_tag_dyneither(_tmp413,sizeof(char),30))),_tag_dyneither(_tmp412,sizeof(void*),1)))))));}else{
# 975
struct Cyc_List_List*_tmp414;prev_fields=((_tmp414=_region_malloc(uprev_rgn,sizeof(*_tmp414)),((_tmp414->hd=(*_tmp223->name).f2,((_tmp414->tl=prev_fields,_tmp414))))));}
# 977
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(ge->ordinaries,(*_tmp223->name).f2)!= 0){
const char*_tmp418;void*_tmp417[1];struct Cyc_String_pa_PrintArg_struct _tmp416;(_tmp416.tag=0,((_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp223->name).f2),((_tmp417[0]=& _tmp416,Cyc_Tcutil_terr(_tmp223->loc,((_tmp418="enum field name %s shadows global name",_tag_dyneither(_tmp418,sizeof(char),39))),_tag_dyneither(_tmp417,sizeof(void*),1)))))));}
# 980
if(_tmp223->tag == 0){
_tmp223->tag=Cyc_Absyn_uint_exp(tag_count,_tmp223->loc);
++ tag_count;}else{
# 985
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp223->tag))){
struct _tuple11 _tmp22B=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp223->tag));unsigned int _tmp22D;int _tmp22E;struct _tuple11 _tmp22C=_tmp22B;_tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;
if(_tmp22E)tag_count=_tmp22D + 1;}}
# 989
(*_tmp223->name).f1=Cyc_Absyn_Abs_n(te->ns,ed->sc == Cyc_Absyn_ExternC);}}
# 967
;_pop_region(uprev_rgn);}
# 994
{struct _handler_cons _tmp22F;_push_handler(& _tmp22F);{int _tmp231=0;if(setjmp(_tmp22F.handler))_tmp231=1;if(!_tmp231){
{struct Cyc_Absyn_Enumdecl**_tmp232=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->enumdecls,v);
struct Cyc_Absyn_Enumdecl*_tmp233=Cyc_Tcdecl_merge_enumdecl(*_tmp232,ed,loc,Cyc_Tc_tc_msg);
if(_tmp233 == 0){_npop_handler(0);return;}
*_tmp232=_tmp233;}
# 995
;_pop_handler();}else{void*_tmp230=(void*)_exn_thrown;void*_tmp235=_tmp230;void*_tmp237;_LL10F: {struct Cyc_Dict_Absent_exn_struct*_tmp236=(struct Cyc_Dict_Absent_exn_struct*)_tmp235;if(_tmp236->tag != Cyc_Dict_Absent)goto _LL111;}_LL110: {
# 1000
struct Cyc_Absyn_Enumdecl**_tmp419;struct Cyc_Absyn_Enumdecl**_tmp238=(_tmp419=_cycalloc(sizeof(*_tmp419)),((_tmp419[0]=ed,_tmp419)));
ge->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)(ge->enumdecls,v,_tmp238);
goto _LL10E;}_LL111: _tmp237=_tmp235;_LL112:(void)_rethrow(_tmp237);_LL10E:;}};}
# 1004
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp23A=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp41F;struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp41E;struct _tuple14*_tmp41D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple14*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp23A->name).f2,(
(_tmp41D=_cycalloc(sizeof(*_tmp41D)),((_tmp41D->f1=(void*)((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=((_tmp41E.tag=3,((_tmp41E.f1=ed,((_tmp41E.f2=_tmp23A,_tmp41E)))))),_tmp41F)))),((_tmp41D->f2=1,_tmp41D)))))));}}
# 1013
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp23E=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp23E->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp23E->tag))){
const char*_tmp424;void*_tmp423[2];struct Cyc_String_pa_PrintArg_struct _tmp422;struct Cyc_String_pa_PrintArg_struct _tmp421;(_tmp421.tag=0,((_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp23E->name).f2),((_tmp422.tag=0,((_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp423[0]=& _tmp422,((_tmp423[1]=& _tmp421,Cyc_Tcutil_terr(loc,((_tmp424="enum %s, field %s: expression is not constant",_tag_dyneither(_tmp424,sizeof(char),46))),_tag_dyneither(_tmp423,sizeof(void*),2)))))))))))));}}}}
# 1023
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
switch(sc){case Cyc_Absyn_Static: _LL113:
# 1026
 if(!in_include){
const char*_tmp427;void*_tmp426;(_tmp426=0,Cyc_Tcutil_warn(loc,((_tmp427="static declaration nested within extern \"C\"",_tag_dyneither(_tmp427,sizeof(char),44))),_tag_dyneither(_tmp426,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Abstract: _LL114:
# 1030
{const char*_tmp42A;void*_tmp429;(_tmp429=0,Cyc_Tcutil_warn(loc,((_tmp42A="abstract declaration nested within extern \"C\"",_tag_dyneither(_tmp42A,sizeof(char),46))),_tag_dyneither(_tmp429,sizeof(void*),0)));}
return 0;case Cyc_Absyn_Public: _LL115:
 goto _LL116;case Cyc_Absyn_Register: _LL116:
 goto _LL117;case Cyc_Absyn_Extern: _LL117:
 return 1;case Cyc_Absyn_ExternC: _LL118:
# 1036
{const char*_tmp42D;void*_tmp42C;(_tmp42C=0,Cyc_Tcutil_warn(loc,((_tmp42D="nested extern \"C\" declaration",_tag_dyneither(_tmp42D,sizeof(char),30))),_tag_dyneither(_tmp42C,sizeof(void*),0)));}
return 1;}}
# 1041
static void Cyc_Tc_resolve_export_namespace(struct Cyc_Tcenv_Tenv*te,struct _tuple12*exp){
struct _tuple12 _tmp249=*exp;unsigned int _tmp24B;struct _tuple0*_tmp24C;struct _tuple12 _tmp24A=_tmp249;_tmp24B=_tmp24A.f1;_tmp24C=_tmp24A.f2;{
union Cyc_Absyn_Nmspace _tmp24E;struct _dyneither_ptr*_tmp24F;struct _tuple0*_tmp24D=_tmp24C;_tmp24E=_tmp24D->f1;_tmp24F=_tmp24D->f2;{
union Cyc_Absyn_Nmspace _tmp250=_tmp24E;_LL11B: if((_tmp250.Rel_n).tag != 1)goto _LL11D;if((struct Cyc_List_List*)(_tmp250.Rel_n).val != 0)goto _LL11D;_LL11C:
# 1047
(*_tmp24C).f1=Cyc_Absyn_Abs_n(te->ns,1);
goto _LL11A;_LL11D:;_LL11E:
 goto _LL11A;_LL11A:;};};}
# 1058
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int in_externCinclude,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 1065
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_List_List*last=0;
# 1068
struct Cyc_List_List*_tmp251=ds0;for(0;_tmp251 != 0;(last=_tmp251,_tmp251=_tmp251->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp251->hd;
unsigned int loc=d->loc;
# 1072
void*_tmp252=d->r;void*_tmp253=_tmp252;struct Cyc_Absyn_Vardecl*_tmp258;struct Cyc_Absyn_Fndecl*_tmp25A;struct Cyc_Absyn_Typedefdecl*_tmp25C;struct Cyc_Absyn_Aggrdecl*_tmp25E;struct Cyc_Absyn_Datatypedecl*_tmp260;struct Cyc_Absyn_Enumdecl*_tmp262;struct _dyneither_ptr*_tmp266;struct Cyc_List_List*_tmp267;union Cyc_Absyn_Nmspace _tmp269;struct _dyneither_ptr*_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26F;struct Cyc_List_List*_tmp270;_LL120: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp254=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp254->tag != 2)goto _LL122;}_LL121:
 goto _LL123;_LL122: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp255=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp255->tag != 3)goto _LL124;}_LL123:
# 1075
{const char*_tmp430;void*_tmp42F;(_tmp42F=0,Cyc_Tcutil_terr(loc,((_tmp430="top level let-declarations are not implemented",_tag_dyneither(_tmp430,sizeof(char),47))),_tag_dyneither(_tmp42F,sizeof(void*),0)));}
goto _LL11F;_LL124: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp256=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp256->tag != 4)goto _LL126;}_LL125:
# 1078
{const char*_tmp433;void*_tmp432;(_tmp432=0,Cyc_Tcutil_terr(loc,((_tmp433="top level region declarations are not implemented",_tag_dyneither(_tmp433,sizeof(char),50))),_tag_dyneither(_tmp432,sizeof(void*),0)));}
goto _LL11F;_LL126: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp257=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp257->tag != 0)goto _LL128;else{_tmp258=_tmp257->f1;}}_LL127:
# 1081
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp258->sc,in_externCinclude))
_tmp258->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,ge,loc,_tmp258,check_var_init,in_externCinclude,exports);
goto _LL11F;_LL128: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp259=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp259->tag != 1)goto _LL12A;else{_tmp25A=_tmp259->f1;}}_LL129:
# 1086
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp25A->sc,in_externCinclude))
_tmp25A->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,ge,loc,_tmp25A,in_externCinclude,exports);
goto _LL11F;_LL12A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp25B=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp25B->tag != 8)goto _LL12C;else{_tmp25C=_tmp25B->f1;}}_LL12B:
# 1091
 Cyc_Tc_tcTypedefdecl(te,ge,loc,_tmp25C);
goto _LL11F;_LL12C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp25D=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp25D->tag != 5)goto _LL12E;else{_tmp25E=_tmp25D->f1;}}_LL12D:
# 1094
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp25E->sc,in_externCinclude))
_tmp25E->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,ge,loc,_tmp25E);
goto _LL11F;_LL12E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp25F=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp25F->tag != 6)goto _LL130;else{_tmp260=_tmp25F->f1;}}_LL12F:
# 1099
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp260->sc,in_externCinclude))
_tmp260->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,ge,loc,_tmp260);
goto _LL11F;_LL130: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp261=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp261->tag != 7)goto _LL132;else{_tmp262=_tmp261->f1;}}_LL131:
# 1104
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp262->sc,in_externCinclude))
_tmp262->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,ge,loc,_tmp262);
goto _LL11F;_LL132: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp263=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp263->tag != 13)goto _LL134;}_LL133:
# 1109
{const char*_tmp436;void*_tmp435;(_tmp435=0,Cyc_Tcutil_warn(d->loc,((_tmp436="spurious __cyclone_port_on__",_tag_dyneither(_tmp436,sizeof(char),29))),_tag_dyneither(_tmp435,sizeof(void*),0)));}
goto _LL11F;_LL134: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp264=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp264->tag != 14)goto _LL136;}_LL135:
# 1112
 goto _LL11F;_LL136: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp265=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp265->tag != 9)goto _LL138;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}_LL137: {
# 1115
struct Cyc_List_List*_tmp277=te->ns;
struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp278=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp277,((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437->hd=_tmp266,((_tmp437->tl=0,_tmp437)))))));
# 1119
if(!((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(ge->namespaces,_tmp266)){
ge->namespaces=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(grgn,ge->namespaces,_tmp266);
te->ae=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(te->ae,_tmp278,Cyc_Tcenv_empty_genv(grgn));}
# 1124
te->ns=_tmp278;
Cyc_Tc_tc_decls(te,_tmp267,in_externC,in_externCinclude,check_var_init,grgn,exports);
te->ns=_tmp277;
goto _LL11F;}_LL138: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp268=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp268->tag != 10)goto _LL13A;else{_tmp269=(_tmp268->f1)->f1;_tmp26A=(_tmp268->f1)->f2;_tmp26B=_tmp268->f2;}}_LL139: {
# 1131
struct _dyneither_ptr*first;
struct Cyc_List_List*rest;
{union Cyc_Absyn_Nmspace _tmp27A=_tmp269;struct _dyneither_ptr*_tmp27B;struct Cyc_List_List*_tmp27C;struct _dyneither_ptr*_tmp27D;struct Cyc_List_List*_tmp27E;struct _dyneither_ptr*_tmp27F;struct Cyc_List_List*_tmp280;_LL13F: if((_tmp27A.Loc_n).tag != 4)goto _LL141;_LL140:
 goto _LL142;_LL141: if((_tmp27A.Rel_n).tag != 1)goto _LL143;if((struct Cyc_List_List*)(_tmp27A.Rel_n).val != 0)goto _LL143;_LL142:
 goto _LL144;_LL143: if((_tmp27A.C_n).tag != 3)goto _LL145;if((struct Cyc_List_List*)(_tmp27A.C_n).val != 0)goto _LL145;_LL144:
 goto _LL146;_LL145: if((_tmp27A.Abs_n).tag != 2)goto _LL147;if((struct Cyc_List_List*)(_tmp27A.Abs_n).val != 0)goto _LL147;_LL146:
# 1138
 first=_tmp26A;rest=0;goto _LL13E;_LL147: if((_tmp27A.Rel_n).tag != 1)goto _LL149;if((struct Cyc_List_List*)(_tmp27A.Rel_n).val == 0)goto _LL149;_tmp27B=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27A.Rel_n).val)->hd;_tmp27C=((struct Cyc_List_List*)(_tmp27A.Rel_n).val)->tl;_LL148:
 _tmp27D=_tmp27B;_tmp27E=_tmp27C;goto _LL14A;_LL149: if((_tmp27A.C_n).tag != 3)goto _LL14B;if((struct Cyc_List_List*)(_tmp27A.C_n).val == 0)goto _LL14B;_tmp27D=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27A.C_n).val)->hd;_tmp27E=((struct Cyc_List_List*)(_tmp27A.C_n).val)->tl;_LL14A:
 _tmp27F=_tmp27D;_tmp280=_tmp27E;goto _LL14C;_LL14B: if((_tmp27A.Abs_n).tag != 2)goto _LL13E;if((struct Cyc_List_List*)(_tmp27A.Abs_n).val == 0)goto _LL13E;_tmp27F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp27A.Abs_n).val)->hd;_tmp280=((struct Cyc_List_List*)(_tmp27A.Abs_n).val)->tl;_LL14C:
# 1142
 first=_tmp27F;{struct Cyc_List_List*_tmp438;rest=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp280,((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->hd=_tmp26A,((_tmp438->tl=0,_tmp438)))))));}goto _LL13E;_LL13E:;}{
# 1144
struct Cyc_List_List*_tmp282=Cyc_Tcenv_resolve_namespace(te,loc,first,rest);
{struct Cyc_List_List*_tmp439;ge->availables=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439->hd=_tmp282,((_tmp439->tl=ge->availables,_tmp439))))));}
Cyc_Tc_tc_decls(te,_tmp26B,in_externC,in_externCinclude,check_var_init,grgn,exports);
ge->availables=((struct Cyc_List_List*)_check_null(ge->availables))->tl;
goto _LL11F;};}_LL13A: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp26C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp26C->tag != 11)goto _LL13C;else{_tmp26D=_tmp26C->f1;}}_LL13B:
# 1151
 Cyc_Tc_tc_decls(te,_tmp26D,1,in_externCinclude,check_var_init,grgn,exports);
goto _LL11F;_LL13C: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp26E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp253;if(_tmp26E->tag != 12)goto _LL11F;else{_tmp26F=_tmp26E->f1;_tmp270=_tmp26E->f2;}}_LL13D:
# 1155
((void(*)(void(*f)(struct Cyc_Tcenv_Tenv*,struct _tuple12*),struct Cyc_Tcenv_Tenv*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tc_resolve_export_namespace,te,_tmp270);{
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp270,(unsigned int)exports?*exports: 0);
Cyc_Tc_tc_decls(te,_tmp26F,1,1,check_var_init,grgn,& newexs);
# 1160
for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
struct _tuple12*_tmp284=(struct _tuple12*)_tmp270->hd;
if(!(*_tmp284).f3){
const char*_tmp43D;void*_tmp43C[1];struct Cyc_String_pa_PrintArg_struct _tmp43B;(_tmp43B.tag=0,((_tmp43B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp284).f2)),((_tmp43C[0]=& _tmp43B,Cyc_Tcutil_warn((*_tmp284).f1,((_tmp43D="%s is exported but not defined",_tag_dyneither(_tmp43D,sizeof(char),31))),_tag_dyneither(_tmp43C,sizeof(void*),1)))))));}}
# 1166
goto _LL11F;};_LL11F:;}}
# 1171
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1173
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{struct _RegionHandle*rgn;int in_cinclude;struct Cyc_Dict_Dict nsdict;};
# 1190 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1192
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1194
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}struct _tuple18{struct Cyc_Tcenv_Genv*f1;struct Cyc_Set_Set*f2;};
# 1198
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp288=d->r;void*_tmp289=_tmp288;struct Cyc_Absyn_Vardecl*_tmp28B;struct Cyc_List_List**_tmp28D;struct Cyc_List_List**_tmp28F;struct Cyc_List_List**_tmp291;struct Cyc_List_List**_tmp293;_LL14E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp28A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp28A->tag != 0)goto _LL150;else{_tmp28B=_tmp28A->f1;}}_LL14F:
# 1203
 if((env->in_cinclude  || _tmp28B->sc != Cyc_Absyn_Extern  && _tmp28B->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp28B->name,Cyc_Absyn_uniquergn_qvar))
# 1206
return 1;{
# 1208
struct _tuple0*_tmp294=_tmp28B->name;union Cyc_Absyn_Nmspace _tmp296;struct _dyneither_ptr*_tmp297;struct _tuple0*_tmp295=_tmp294;_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;{
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp298=_tmp296;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp29B;_LL15B: if((_tmp298.Loc_n).tag != 4)goto _LL15D;_LL15C:
 ns=0;goto _LL15A;_LL15D: if((_tmp298.Rel_n).tag != 1)goto _LL15F;_tmp299=(struct Cyc_List_List*)(_tmp298.Rel_n).val;_LL15E:
 ns=_tmp299;goto _LL15A;_LL15F: if((_tmp298.C_n).tag != 3)goto _LL161;_tmp29A=(struct Cyc_List_List*)(_tmp298.C_n).val;_LL160:
 ns=_tmp29A;goto _LL15A;_LL161: if((_tmp298.Abs_n).tag != 2)goto _LL15A;_tmp29B=(struct Cyc_List_List*)(_tmp298.Abs_n).val;_LL162:
 ns=_tmp29B;goto _LL15A;_LL15A:;}{
# 1216
struct _tuple18*_tmp29C=((struct _tuple18*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(env->nsdict,ns);
struct Cyc_Tcenv_Genv*_tmp29D=(*_tmp29C).f1;
int _tmp29E=(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp29D->ordinaries,_tmp297)).f2;
if(!_tmp29E)
(*_tmp29C).f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_rinsert)(env->rgn,(*_tmp29C).f2,_tmp297);
# 1224
return _tmp29E;};};};_LL150: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp28C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp28C->tag != 11)goto _LL152;else{_tmp28D=(struct Cyc_List_List**)& _tmp28C->f1;}}_LL151:
 _tmp28F=_tmp28D;goto _LL153;_LL152: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp28E=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp28E->tag != 10)goto _LL154;else{_tmp28F=(struct Cyc_List_List**)& _tmp28E->f2;}}_LL153:
 _tmp291=_tmp28F;goto _LL155;_LL154: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp290=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp290->tag != 9)goto _LL156;else{_tmp291=(struct Cyc_List_List**)& _tmp290->f2;}}_LL155:
# 1228
*_tmp291=Cyc_Tc_treeshake_f(env,*_tmp291);
return 1;_LL156: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp292=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp289;if(_tmp292->tag != 12)goto _LL158;else{_tmp293=(struct Cyc_List_List**)& _tmp292->f1;}}_LL157: {
# 1231
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp293=Cyc_Tc_treeshake_f(env,*_tmp293);
env->in_cinclude=in_cinclude;
return 1;}_LL158:;_LL159:
 return 1;_LL14D:;}
# 1240
static int Cyc_Tc_treeshake_remove_f(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,void*y){
return !((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(set,x);}
# 1244
static struct _tuple18*Cyc_Tc_treeshake_make_env_f(struct _RegionHandle*rgn,struct Cyc_Tcenv_Genv*ge){
# 1246
struct _tuple18*_tmp43E;return(_tmp43E=_region_malloc(rgn,sizeof(*_tmp43E)),((_tmp43E->f1=ge,((_tmp43E->f2=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(rgn,Cyc_strptrcmp),_tmp43E)))));}struct _tuple19{struct Cyc_List_List*f1;struct _tuple18*f2;};
# 1249
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct _RegionHandle _tmp2A0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2A0;_push_region(rgn);
{struct Cyc_Tc_TreeshakeEnv _tmp43F;struct Cyc_Tc_TreeshakeEnv _tmp2A1=(_tmp43F.rgn=rgn,((_tmp43F.in_cinclude=0,((_tmp43F.nsdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct _tuple18*(*f)(struct _RegionHandle*,struct Cyc_Tcenv_Genv*),struct _RegionHandle*env,struct Cyc_Dict_Dict d))Cyc_Dict_rmap_c)(rgn,Cyc_Tc_treeshake_make_env_f,rgn,te->ae),_tmp43F)))));
struct Cyc_List_List*_tmp2A2=Cyc_Tc_treeshake_f(& _tmp2A1,ds);
if(((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp2A1.nsdict)){
struct Cyc_List_List*_tmp2A3=_tmp2A2;_npop_handler(0);return _tmp2A3;}{
# 1257
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(Cyc_Core_heap_region,_tmp2A1.nsdict);
struct _tuple19 _tmp2A4=*((struct _tuple19*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(rgn,_tmp2A1.nsdict);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple19*))Cyc_Iter_next)(iter,& _tmp2A4)){
struct Cyc_Tcenv_Genv*_tmp2A6;struct Cyc_Set_Set*_tmp2A7;struct _tuple19 _tmp2A5=_tmp2A4;_tmp2A6=(_tmp2A5.f2)->f1;_tmp2A7=(_tmp2A5.f2)->f2;
_tmp2A6->ordinaries=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*f)(struct Cyc_Set_Set*,struct _dyneither_ptr*,struct _tuple14*),struct Cyc_Set_Set*env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(_tmp2A6->grgn,(int(*)(struct Cyc_Set_Set*set,struct _dyneither_ptr*x,struct _tuple14*y))Cyc_Tc_treeshake_remove_f,_tmp2A7,_tmp2A6->ordinaries);}{
# 1263
struct Cyc_List_List*_tmp2A8=_tmp2A2;_npop_handler(0);return _tmp2A8;};};}
# 1251
;_pop_region(rgn);}
