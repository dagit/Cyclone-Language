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
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 150
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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 251
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 352
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 445
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 540
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 935 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 941
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 944
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 949
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 962
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 965
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 967
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 969
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 979
void*Cyc_Absyn_exn_typ();
# 990
extern void*Cyc_Absyn_bounds_one;
# 995
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 998
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1001
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1006
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1017
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1080
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1084
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1090
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1105
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1166
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1168
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1171
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1173
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1185
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 128 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 140
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 142
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 150
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 154
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 176
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 182
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 195
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 201
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 209
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 213
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 217
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 224
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 80
int Cyc_Tcutil_coerceable(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 118
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 137
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 238 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 298
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp6D0;(_tmp6D0=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6D0,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp6D1;return(_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1->f1=0,((_tmp6D1->f2=e,_tmp6D1)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;void*_tmp3=_tmp2;struct _tuple1*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3;if(_tmp4->tag != 1)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(void**)((void**)& _tmp4->f2);}}_LL2: {
# 79
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp5,1);void*_tmpC=_tmpB;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpD->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpD->f1);if(_tmpE->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp6D5;void*_tmp6D4[1];struct Cyc_String_pa_PrintArg_struct _tmp6D3;(_tmp6D3.tag=0,((_tmp6D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6D4[0]=& _tmp6D3,Cyc_Tcutil_terr(e1->loc,((_tmp6D5="undeclared identifier %s",_tag_dyneither(_tmp6D5,sizeof(char),25))),_tag_dyneither(_tmp6D4,sizeof(void*),1)))))));}{
int _tmp1B=0;_npop_handler(1);return _tmp1B;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 86
*_tmp6=_tmp10;{
int _tmp1C=1;_npop_handler(1);return _tmp1C;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 89
 if(_tmp13->typs == 0){
# 91
{const char*_tmp6D9;void*_tmp6D8[1];struct Cyc_String_pa_PrintArg_struct _tmp6D7;(_tmp6D7.tag=0,((_tmp6D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6D8[0]=& _tmp6D7,Cyc_Tcutil_terr(e->loc,((_tmp6D9="%s is a constant, not a function",_tag_dyneither(_tmp6D9,sizeof(char),33))),_tag_dyneither(_tmp6D8,sizeof(void*),1)))))));}{
int _tmp20=0;_npop_handler(1);return _tmp20;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6DC;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6DB;e->r=(void*)((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB[0]=((_tmp6DC.tag=30,((_tmp6DC.f1=es,((_tmp6DC.f2=_tmp12,((_tmp6DC.f3=_tmp13,_tmp6DC)))))))),_tmp6DB))));}{
int _tmp23=1;_npop_handler(1);return _tmp23;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp24=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6DF;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6DE;e->r=(void*)((_tmp6DE=_cycalloc(sizeof(*_tmp6DE)),((_tmp6DE[0]=((_tmp6DF.tag=28,((_tmp6DF.f1=_tmp15->name,((_tmp6DF.f2=0,((_tmp6DF.f3=_tmp24,((_tmp6DF.f4=_tmp15,_tmp6DF)))))))))),_tmp6DE))));}{
int _tmp27=1;_npop_handler(1);return _tmp27;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp16->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6E3;void*_tmp6E2[1];struct Cyc_String_pa_PrintArg_struct _tmp6E1;(_tmp6E1.tag=0,((_tmp6E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6E2[0]=& _tmp6E1,Cyc_Tcutil_terr(e->loc,((_tmp6E3="%s is an enum constructor, not a function",_tag_dyneither(_tmp6E3,sizeof(char),42))),_tag_dyneither(_tmp6E2,sizeof(void*),1)))))));}{
int _tmp2B=0;_npop_handler(1);return _tmp2B;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp2D=_tmp8;void*_tmp2F;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6E7;void*_tmp6E6[1];struct Cyc_String_pa_PrintArg_struct _tmp6E5;(_tmp6E5.tag=0,((_tmp6E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6E6[0]=& _tmp6E5,Cyc_Tcutil_terr(e1->loc,((_tmp6E7="undeclared identifier %s",_tag_dyneither(_tmp6E7,sizeof(char),25))),_tag_dyneither(_tmp6E6,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2F=_tmp2D;_LL16:(void)_rethrow(_tmp2F);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6EA;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6E9;e->r=(void*)((_tmp6E9=_cycalloc(sizeof(*_tmp6E9)),((_tmp6E9[0]=((_tmp6EA.tag=25,((_tmp6EA.f1=des,_tmp6EA)))),_tmp6E9))));}
return;}{
# 125
void*t=*topt;
void*_tmp35=Cyc_Tcutil_compress(t);void*_tmp36=_tmp35;union Cyc_Absyn_AggrInfoU _tmp38;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL1A;else{_tmp38=(_tmp37->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3D=_tmp38;struct Cyc_Absyn_Aggrdecl*_tmp3E;_LL21: if((_tmp3D.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6ED;void*_tmp6EC;(_tmp6EC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6ED="struct type not properly set",_tag_dyneither(_tmp6ED,sizeof(char),29))),_tag_dyneither(_tmp6EC,sizeof(void*),0)));}_LL23: if((_tmp3D.KnownAggr).tag != 2)goto _LL20;_tmp3E=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3D.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6F0;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6EF;e->r=(void*)((_tmp6EF=_cycalloc(sizeof(*_tmp6EF)),((_tmp6EF[0]=((_tmp6F0.tag=28,((_tmp6F0.f1=_tmp3E->name,((_tmp6F0.f2=0,((_tmp6F0.f3=des,((_tmp6F0.f4=_tmp3E,_tmp6F0)))))))))),_tmp6EF))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36;if(_tmp39->tag != 8)goto _LL1C;else{_tmp3A=(_tmp39->f1).elt_type;_tmp3B=(_tmp39->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6F3;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F2;e->r=(void*)((_tmp6F2=_cycalloc(sizeof(*_tmp6F2)),((_tmp6F2[0]=((_tmp6F3.tag=25,((_tmp6F3.f1=des,_tmp6F3)))),_tmp6F2))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3C->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6F6;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6F5;e->r=(void*)((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5[0]=((_tmp6F6.tag=29,((_tmp6F6.f1=t,((_tmp6F6.f2=des,_tmp6F6)))))),_tmp6F5))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6F9;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6F8;e->r=(void*)((_tmp6F8=_cycalloc(sizeof(*_tmp6F8)),((_tmp6F8[0]=((_tmp6F9.tag=25,((_tmp6F9.f1=des,_tmp6F9)))),_tmp6F8))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcutil_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6FE;void*_tmp6FD[2];struct Cyc_String_pa_PrintArg_struct _tmp6FC;struct Cyc_String_pa_PrintArg_struct _tmp6FB;(_tmp6FB.tag=0,((_tmp6FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6FC.tag=0,((_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6FD[0]=& _tmp6FC,((_tmp6FD[1]=& _tmp6FB,Cyc_Tcutil_terr(e->loc,((_tmp6FE="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp6FE,sizeof(char),59))),_tag_dyneither(_tmp6FD,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4D=e->r;void*_tmp4E=_tmp4D;enum Cyc_Absyn_Primop _tmp50;struct Cyc_List_List*_tmp51;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E;if(_tmp4F->tag != 2)goto _LL28;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27:
# 163
 if(_tmp50 == Cyc_Absyn_Eq  || _tmp50 == Cyc_Absyn_Neq){
struct _tuple0 _tmp6FF;struct _tuple0 _tmp52=(_tmp6FF.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->topt),((_tmp6FF.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp51))->tl))->hd)->topt),_tmp6FF)));struct _tuple0 _tmp53=_tmp52;void*_tmp55;void*_tmp57;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f1;if(_tmp54->tag != 15)goto _LL2D;else{_tmp55=(void*)_tmp54->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f2;if(_tmp56->tag != 15)goto _LL2D;else{_tmp57=(void*)_tmp56->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp702;struct Cyc_Tcexp_TestEnv _tmp701;return(_tmp701.eq=((_tmp702=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp702)),((_tmp702->f1=_tmp55,((_tmp702->f2=_tmp57,_tmp702)))))),((_tmp701.isTrue=_tmp50 == Cyc_Absyn_Eq,_tmp701)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp703;return(_tmp703.eq=0,((_tmp703.isTrue=0,_tmp703)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp5C=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp5D=_tmp5C;enum Cyc_Absyn_Sign _tmp5E;enum Cyc_Absyn_Sign _tmp5F;int _tmp60;enum Cyc_Absyn_Sign _tmp61;int _tmp62;struct _dyneither_ptr _tmp63;struct _dyneither_ptr _tmp64;_LL30: if((_tmp5D.Char_c).tag != 2)goto _LL32;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Signed)goto _LL32;_LL31:
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp5D.Char_c).tag != 2)goto _LL34;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp5D.Char_c).tag != 2)goto _LL36;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_None)goto _LL36;_LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp5D.Wchar_c).tag != 3)goto _LL38;_LL37:
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp5D.Short_c).tag != 4)goto _LL3A;_tmp5E=((struct _tuple5)(_tmp5D.Short_c).val).f1;_LL39:
# 210
 t=_tmp5E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp5D.LongLong_c).tag != 6)goto _LL3C;_tmp5F=((struct _tuple7)(_tmp5D.LongLong_c).val).f1;_LL3B:
# 212
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp5D.Float_c).tag != 7)goto _LL3E;_tmp60=((struct _tuple8)(_tmp5D.Float_c).val).f2;_LL3D:
 t=Cyc_Absyn_float_typ(_tmp60);goto _LL2F;_LL3E: if((_tmp5D.Int_c).tag != 5)goto _LL40;_tmp61=((struct _tuple6)(_tmp5D.Int_c).val).f1;_tmp62=((struct _tuple6)(_tmp5D.Int_c).val).f2;_LL3F:
# 215
 if(topt == 0)
t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp65=Cyc_Tcutil_compress(*topt);void*_tmp66=_tmp65;enum Cyc_Absyn_Sign _tmp68;enum Cyc_Absyn_Sign _tmp6A;enum Cyc_Absyn_Sign _tmp6C;enum Cyc_Absyn_Sign _tmp6E;void*_tmp70;struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;union Cyc_Absyn_Constraint*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;void*_tmp77;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp67->tag != 6)goto _LL49;else{_tmp68=_tmp67->f1;if(_tmp67->f2 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp68){case Cyc_Absyn_Unsigned: _LL55:
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp68,(char)_tmp62);
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp69->tag != 6)goto _LL4B;else{_tmp6A=_tmp69->f1;if(_tmp69->f2 != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp6A == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp6A,(short)_tmp62);
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6B->tag != 6)goto _LL4D;else{_tmp6C=_tmp6B->f1;if(_tmp6B->f2 != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp6C == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6D->tag != 6)goto _LL4F;else{_tmp6E=_tmp6D->f1;if(_tmp6D->f2 != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp6E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66;if(_tmp6F->tag != 5)goto _LL51;else{_tmp70=(_tmp6F->f1).elt_typ;_tmp71=(_tmp6F->f1).elt_tq;_tmp72=((_tmp6F->f1).ptr_atts).rgn;_tmp73=((_tmp6F->f1).ptr_atts).nullable;_tmp74=((_tmp6F->f1).ptr_atts).bounds;_tmp75=((_tmp6F->f1).ptr_atts).zero_term;}}if(!(_tmp62 == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp73))return*topt;{
struct Cyc_List_List*_tmp78=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp709;struct Cyc_Absyn_PtrInfo _tmp708;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp707;t=(void*)((_tmp707=_cycalloc(sizeof(*_tmp707)),((_tmp707[0]=((_tmp709.tag=5,((_tmp709.f1=((_tmp708.elt_typ=_tmp70,((_tmp708.elt_tq=_tmp71,((_tmp708.ptr_atts=(
((_tmp708.ptr_atts).rgn=_tmp72,(((_tmp708.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp708.ptr_atts).bounds=_tmp74,(((_tmp708.ptr_atts).zero_term=_tmp75,(((_tmp708.ptr_atts).ptrloc=0,_tmp708.ptr_atts)))))))))),_tmp708)))))),_tmp709)))),_tmp707))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66;if(_tmp76->tag != 19)goto _LL53;else{_tmp77=(void*)_tmp76->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp70C;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp70B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7D=(_tmp70B=_cycalloc(sizeof(*_tmp70B)),((_tmp70B[0]=((_tmp70C.tag=18,((_tmp70C.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0),_tmp70C)))),_tmp70B)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp70F;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp70E;t=(void*)((_tmp70E=_cycalloc(sizeof(*_tmp70E)),((_tmp70E[0]=((_tmp70F.tag=19,((_tmp70F.f1=(void*)_tmp7D,_tmp70F)))),_tmp70E))));}
goto _LL46;}_LL53:;_LL54:
# 261
 t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}
# 264
goto _LL2F;_LL40: if((_tmp5D.String_c).tag != 8)goto _LL42;_tmp63=(struct _dyneither_ptr)(_tmp5D.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp63,sizeof(char));
_tmp64=_tmp63;goto _LL43;_LL42: if((_tmp5D.Wstring_c).tag != 9)goto _LL44;_tmp64=(struct _dyneither_ptr)(_tmp5D.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp64);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp712;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp711;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82=(_tmp711=_cycalloc(sizeof(*_tmp711)),((_tmp711[0]=((_tmp712.tag=1,((_tmp712.f1=elen,_tmp712)))),_tmp711)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp83=Cyc_Tcutil_compress(*topt);void*_tmp84=_tmp83;struct Cyc_Absyn_Tqual _tmp86;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp84;if(_tmp85->tag != 8)goto _LL5C;else{_tmp86=(_tmp85->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp86,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84;if(_tmp87->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 302
goto _LL59;_LL5E:;_LL5F:
 goto _LL59;_LL59:;}
# 306
return t;};};_LL44: if((_tmp5D.Null_c).tag != 1)goto _LL2F;_LL45:
# 308
 if(topt != 0){
void*_tmp8A=Cyc_Tcutil_compress(*topt);void*_tmp8B=_tmp8A;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8F;union Cyc_Absyn_Constraint*_tmp90;union Cyc_Absyn_Constraint*_tmp91;union Cyc_Absyn_Constraint*_tmp92;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B;if(_tmp8C->tag != 5)goto _LL63;else{_tmp8D=(_tmp8C->f1).elt_typ;_tmp8E=(_tmp8C->f1).elt_tq;_tmp8F=((_tmp8C->f1).ptr_atts).rgn;_tmp90=((_tmp8C->f1).ptr_atts).nullable;_tmp91=((_tmp8C->f1).ptr_atts).bounds;_tmp92=((_tmp8C->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp90))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp718;struct Cyc_Absyn_PtrInfo _tmp717;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp716;return(void*)((_tmp716=_cycalloc(sizeof(*_tmp716)),((_tmp716[0]=((_tmp718.tag=5,((_tmp718.f1=((_tmp717.elt_typ=_tmp8D,((_tmp717.elt_tq=_tmp8E,((_tmp717.ptr_atts=(((_tmp717.ptr_atts).rgn=_tmp8F,(((_tmp717.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp717.ptr_atts).bounds=_tmp91,(((_tmp717.ptr_atts).zero_term=_tmp92,(((_tmp717.ptr_atts).ptrloc=0,_tmp717.ptr_atts)))))))))),_tmp717)))))),_tmp718)))),_tmp716))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp96=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp727;struct Cyc_Core_Opt*_tmp726;struct Cyc_Core_Opt*_tmp725;struct Cyc_Absyn_PtrInfo _tmp724;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp723;t=(void*)((_tmp723=_cycalloc(sizeof(*_tmp723)),((_tmp723[0]=((_tmp727.tag=5,((_tmp727.f1=((_tmp724.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725->v=_tmp96,_tmp725))))),((_tmp724.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp724.ptr_atts=(
((_tmp724.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp726=_cycalloc(sizeof(*_tmp726)),((_tmp726->v=_tmp96,_tmp726))))),(((_tmp724.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp724.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp724.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp724.ptr_atts).ptrloc=0,_tmp724.ptr_atts)))))))))),_tmp724)))))),_tmp727)))),_tmp723))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp9C=*((void**)_check_null(b));void*_tmp9D=_tmp9C;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Fndecl*_tmpA2;struct Cyc_Absyn_Vardecl*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA8;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmpA9;_push_handler(& _tmpA9);{int _tmpAB=0;if(setjmp(_tmpA9.handler))_tmpAB=1;if(!_tmpAB){
{struct _RegionHandle _tmpAC=_new_region("r");struct _RegionHandle*r=& _tmpAC;_push_region(r);
{void*_tmpAD=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpAE=_tmpAD;void*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Datatypedecl*_tmpBA;struct Cyc_Absyn_Datatypefield*_tmpBB;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpAF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpAF->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpB0=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpAF->f1);if(_tmpB0->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp72B;void*_tmp72A[1];struct Cyc_String_pa_PrintArg_struct _tmp729;void*_tmpC0=(_tmp729.tag=0,((_tmp729.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp72A[0]=& _tmp729,Cyc_Tcexp_expr_err(te,loc,0,((_tmp72B="undeclared identifier %s",_tag_dyneither(_tmp72B,sizeof(char),25))),_tag_dyneither(_tmp72A,sizeof(void*),1)))))));_npop_handler(1);return _tmpC0;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB1->tag != 0)goto _LL77;else{_tmpB2=(void*)_tmpB1->f1;}}_LL76:
# 348
*b=_tmpB2;{
void*_tmpC1=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpC1;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB3=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB3->tag != 3)goto _LL79;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp72E;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp72D;e->r=(void*)((_tmp72D=_cycalloc(sizeof(*_tmp72D)),((_tmp72D[0]=((_tmp72E.tag=31,((_tmp72E.f1=_tmpB5->name,((_tmp72E.f2=_tmpB4,((_tmp72E.f3=_tmpB5,_tmp72E)))))))),_tmp72D))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp731;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp730;void*_tmpC6=(void*)((_tmp730=_cycalloc(sizeof(*_tmp730)),((_tmp730[0]=((_tmp731.tag=13,((_tmp731.f1=_tmpB4->name,((_tmp731.f2=_tmpB4,_tmp731)))))),_tmp730))));_npop_handler(1);return _tmpC6;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB6->tag != 4)goto _LL7B;else{_tmpB7=(void*)_tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp734;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp733;e->r=(void*)((_tmp733=_cycalloc(sizeof(*_tmp733)),((_tmp733[0]=((_tmp734.tag=32,((_tmp734.f1=_tmpB8->name,((_tmp734.f2=_tmpB7,((_tmp734.f3=_tmpB8,_tmp734)))))))),_tmp733))));}{
void*_tmpC9=_tmpB7;_npop_handler(1);return _tmpC9;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB9->tag != 2)goto _LL7D;else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp737;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp736;e->r=(void*)((_tmp736=_cycalloc(sizeof(*_tmp736)),((_tmp736[0]=((_tmp737.tag=30,((_tmp737.f1=0,((_tmp737.f2=_tmpBA,((_tmp737.f3=_tmpBB,_tmp737)))))))),_tmp736))));}{
void*_tmpCC=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpBA,_tmpBB);_npop_handler(1);return _tmpCC;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpBC=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpBC->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp73B;void*_tmp73A[1];struct Cyc_String_pa_PrintArg_struct _tmp739;void*_tmpD0=(_tmp739.tag=0,((_tmp739.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp73A[0]=& _tmp739,Cyc_Tcexp_expr_err(te,loc,0,((_tmp73B="bad occurrence of type name %s",_tag_dyneither(_tmp73B,sizeof(char),31))),_tag_dyneither(_tmp73A,sizeof(void*),1)))))));_npop_handler(1);return _tmpD0;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpAA=(void*)_exn_thrown;void*_tmpD2=_tmpAA;void*_tmpD4;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpD3=(struct Cyc_Dict_Absent_exn_struct*)_tmpD2;if(_tmpD3->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp73F;void*_tmp73E[1];struct Cyc_String_pa_PrintArg_struct _tmp73D;return(_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp73E[0]=& _tmp73D,Cyc_Tcexp_expr_err(te,loc,0,((_tmp73F="undeclared identifier %s",_tag_dyneither(_tmp73F,sizeof(char),25))),_tag_dyneither(_tmp73E,sizeof(void*),1)))))));}_LL82: _tmpD4=_tmpD2;_LL83:(void)_rethrow(_tmpD4);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL6A;else{_tmpA0=_tmp9F->f1;}}_LL69:
# 368
*q=*_tmpA0->name;
return _tmpA0->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA1=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA1->tag != 2)goto _LL6C;else{_tmpA2=_tmpA1->f1;}}_LL6B:
*q=*_tmpA2->name;return Cyc_Tcutil_fndecl2typ(_tmpA2);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpA3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA3->tag != 5)goto _LL6E;else{_tmpA4=_tmpA3->f1;}}_LL6D:
 _tmpA6=_tmpA4;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA5->tag != 4)goto _LL70;else{_tmpA6=_tmpA5->f1;}}_LL6F:
 _tmpA8=_tmpA6;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA7->tag != 3)goto _LL65;else{_tmpA8=_tmpA7->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp740;(*q).f1=(((_tmp740.Loc_n).val=0,(((_tmp740.Loc_n).tag=4,_tmp740))));}
if(te->allow_valueof){
void*_tmpD9=Cyc_Tcutil_compress(_tmpA8->type);void*_tmpDA=_tmpD9;void*_tmpDC;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpDA;if(_tmpDB->tag != 19)goto _LL87;else{_tmpDC=(void*)_tmpDB->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp743;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp742;e->r=(void*)((_tmp742=_cycalloc(sizeof(*_tmp742)),((_tmp742[0]=((_tmp743.tag=38,((_tmp743.f1=_tmpDC,_tmp743)))),_tmp742))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmpA8->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpDF=fmt->r;void*_tmpE0=_tmpDF;struct _dyneither_ptr _tmpE2;struct _dyneither_ptr _tmpE5;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 0)goto _LL8C;else{if(((_tmpE1->f1).String_c).tag != 8)goto _LL8C;_tmpE2=(struct _dyneither_ptr)((_tmpE1->f1).String_c).val;}}_LL8B:
 _tmpE5=_tmpE2;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE3->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmpE3->f2)->r;if(_tmpE4->tag != 0)goto _LL8E;else{if(((_tmpE4->f1).String_c).tag != 8)goto _LL8E;_tmpE5=(struct _dyneither_ptr)((_tmpE4->f1).String_c).val;}}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpE5,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpE6=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpE6->hd);{
struct _RegionHandle _tmpE7=_new_region("temp");struct _RegionHandle*temp=& _tmpE7;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpE6->hd)){
const char*_tmp746;void*_tmp745;(_tmp745=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->loc,((_tmp746="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp746,sizeof(char),49))),_tag_dyneither(_tmp745,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpEA=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpEA != 0;
(((desc_types=desc_types->tl,_tmpEA=_tmpEA->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpEA->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp74B;void*_tmp74A[2];struct Cyc_String_pa_PrintArg_struct _tmp749;struct Cyc_String_pa_PrintArg_struct _tmp748;(_tmp748.tag=0,((_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp749.tag=0,((_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp74A[0]=& _tmp749,((_tmp74A[1]=& _tmp748,Cyc_Tcutil_terr(e->loc,((_tmp74B="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp74B,sizeof(char),48))),_tag_dyneither(_tmp74A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp74C;*alias_arg_exps=((_tmp74C=_cycalloc(sizeof(*_tmp74C)),((_tmp74C->hd=(void*)arg_cnt,((_tmp74C->tl=*alias_arg_exps,_tmp74C))))));}{
struct _RegionHandle _tmpF0=_new_region("temp");struct _RegionHandle*temp=& _tmpF0;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp74F;void*_tmp74E;(_tmp74E=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp74F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp74F,sizeof(char),49))),_tag_dyneither(_tmp74E,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp752;void*_tmp751;(_tmp751=0,Cyc_Tcutil_terr(fmt->loc,((_tmp752="too few arguments",_tag_dyneither(_tmp752,sizeof(char),18))),_tag_dyneither(_tmp751,sizeof(void*),0)));}
if(_tmpEA != 0){
{const char*_tmp755;void*_tmp754;(_tmp754=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp755="too many arguments",_tag_dyneither(_tmp755,sizeof(char),19))),_tag_dyneither(_tmp754,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp758;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp757;(int)_throw((void*)((_tmp757=_cycalloc_atomic(sizeof(*_tmp757)),((_tmp757[0]=((_tmp758.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp758)),_tmp757)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp75C;void*_tmp75B[1];struct Cyc_String_pa_PrintArg_struct _tmp75A;(_tmp75A.tag=0,((_tmp75A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75B[0]=& _tmp75A,Cyc_Tcutil_terr(loc,((_tmp75C="expecting numeric type but found %s",_tag_dyneither(_tmp75C,sizeof(char),36))),_tag_dyneither(_tmp75B,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp760;void*_tmp75F[1];struct Cyc_String_pa_PrintArg_struct _tmp75E;(_tmp75E.tag=0,((_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75F[0]=& _tmp75E,Cyc_Tcutil_terr(loc,((_tmp760="expecting integral or * type but found %s",_tag_dyneither(_tmp760,sizeof(char),42))),_tag_dyneither(_tmp75F,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp764;void*_tmp763[1];struct Cyc_String_pa_PrintArg_struct _tmp762;(_tmp762.tag=0,((_tmp762.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp763[0]=& _tmp762,Cyc_Tcutil_terr(loc,((_tmp764="expecting integral type but found %s",_tag_dyneither(_tmp764,sizeof(char),37))),_tag_dyneither(_tmp763,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmp102=t;union Cyc_Absyn_Constraint*_tmp105;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp104=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp104->tag != 5)goto _LL9B;else{_tmp105=((_tmp104->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_tmp106=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp105);void*_tmp107=_tmp106;struct Cyc_Absyn_Exp*_tmp10A;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp108=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp107;if(_tmp108->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp109=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp107;if(_tmp109->tag != 1)goto _LL9D;else{_tmp10A=_tmp109->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmp10A) && !((unsigned int)Cyc_Tcenv_allow_valueof)){
const char*_tmp767;void*_tmp766;(_tmp766=0,Cyc_Tcutil_terr(loc,((_tmp767="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp767,sizeof(char),55))),_tag_dyneither(_tmp766,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp76B;void*_tmp76A[1];struct Cyc_String_pa_PrintArg_struct _tmp769;(_tmp769.tag=0,((_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp76A[0]=& _tmp769,Cyc_Tcutil_terr(loc,((_tmp76B="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp76B,sizeof(char),47))),_tag_dyneither(_tmp76A,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp76E;void*_tmp76D;(_tmp76D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp76E="Non-unary primop",_tag_dyneither(_tmp76E,sizeof(char),17))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp772;void*_tmp771[1];struct Cyc_String_pa_PrintArg_struct _tmp770;(_tmp770.tag=0,((_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp771[0]=& _tmp770,Cyc_Tcutil_terr(e1->loc,((_tmp772="type %s cannot be used here",_tag_dyneither(_tmp772,sizeof(char),28))),_tag_dyneither(_tmp771,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp776;void*_tmp775[1];struct Cyc_String_pa_PrintArg_struct _tmp774;(_tmp774.tag=0,((_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp775[0]=& _tmp774,Cyc_Tcutil_terr(e2->loc,((_tmp776="type %s cannot be used here",_tag_dyneither(_tmp776,sizeof(char),28))),_tag_dyneither(_tmp775,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 497
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 502
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp118=t1;void*_tmp11A;struct Cyc_Absyn_Tqual _tmp11B;void*_tmp11C;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11E;union Cyc_Absyn_Constraint*_tmp11F;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp119=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp118;if(_tmp119->tag != 5)goto _LLA6;else{_tmp11A=(_tmp119->f1).elt_typ;_tmp11B=(_tmp119->f1).elt_tq;_tmp11C=((_tmp119->f1).ptr_atts).rgn;_tmp11D=((_tmp119->f1).ptr_atts).nullable;_tmp11E=((_tmp119->f1).ptr_atts).bounds;_tmp11F=((_tmp119->f1).ptr_atts).zero_term;}}_LLA5:
# 507
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp11A),& Cyc_Tcutil_tmk)){
const char*_tmp779;void*_tmp778;(_tmp778=0,Cyc_Tcutil_terr(e1->loc,((_tmp779="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp779,sizeof(char),50))),_tag_dyneither(_tmp778,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp77C;void*_tmp77B;(_tmp77B=0,Cyc_Tcutil_terr(e1->loc,((_tmp77C="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp77C,sizeof(char),54))),_tag_dyneither(_tmp77B,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp780;void*_tmp77F[1];struct Cyc_String_pa_PrintArg_struct _tmp77E;(_tmp77E.tag=0,((_tmp77E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp77F[0]=& _tmp77E,Cyc_Tcutil_terr(e2->loc,((_tmp780="expecting int but found %s",_tag_dyneither(_tmp780,sizeof(char),27))),_tag_dyneither(_tmp77F,sizeof(void*),1)))))));}{
void*_tmp127=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp11E);void*_tmp128=_tmp127;struct Cyc_Absyn_Exp*_tmp12B;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp129=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp128;if(_tmp129->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp128;if(_tmp12A->tag != 1)goto _LLA8;else{_tmp12B=_tmp12A->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11F)){
const char*_tmp783;void*_tmp782;(_tmp782=0,Cyc_Tcutil_warn(e1->loc,((_tmp783="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp783,sizeof(char),70))),_tag_dyneither(_tmp782,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp789;struct Cyc_Absyn_PtrInfo _tmp788;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp787;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E=(_tmp787=_cycalloc(sizeof(*_tmp787)),((_tmp787[0]=((_tmp789.tag=5,((_tmp789.f1=((_tmp788.elt_typ=_tmp11A,((_tmp788.elt_tq=_tmp11B,((_tmp788.ptr_atts=(
((_tmp788.ptr_atts).rgn=_tmp11C,(((_tmp788.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp788.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp788.ptr_atts).zero_term=_tmp11F,(((_tmp788.ptr_atts).ptrloc=0,_tmp788.ptr_atts)))))))))),_tmp788)))))),_tmp789)))),_tmp787)));
# 530
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp12E,Cyc_Absyn_Other_coercion);
return(void*)_tmp12E;};_LLA8:;};_LLA6:;_LLA7:
# 533
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 538
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp78E;void*_tmp78D[2];struct Cyc_String_pa_PrintArg_struct _tmp78C;struct Cyc_String_pa_PrintArg_struct _tmp78B;(_tmp78B.tag=0,((_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp78C.tag=0,((_tmp78C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp78D[0]=& _tmp78C,((_tmp78D[1]=& _tmp78B,Cyc_Tcutil_terr(e1->loc,((_tmp78E="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp78E,sizeof(char),59))),_tag_dyneither(_tmp78D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp793;void*_tmp792[2];struct Cyc_String_pa_PrintArg_struct _tmp791;struct Cyc_String_pa_PrintArg_struct _tmp790;(_tmp790.tag=0,((_tmp790.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp791.tag=0,((_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp792[0]=& _tmp791,((_tmp792[1]=& _tmp790,Cyc_Tcutil_terr(e1->loc,((_tmp793="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp793,sizeof(char),59))),_tag_dyneither(_tmp792,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp796;void*_tmp795;(_tmp795=0,Cyc_Tcutil_warn(e1->loc,((_tmp796="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp796,sizeof(char),60))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_terr(e1->loc,((_tmp799="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp799,sizeof(char),50))),_tag_dyneither(_tmp798,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp79C;void*_tmp79B;(_tmp79B=0,Cyc_Tcutil_terr(e1->loc,((_tmp79C="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp79C,sizeof(char),54))),_tag_dyneither(_tmp79B,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp7A1;void*_tmp7A0[2];struct Cyc_String_pa_PrintArg_struct _tmp79F;struct Cyc_String_pa_PrintArg_struct _tmp79E;(_tmp79E.tag=0,((_tmp79E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp79F.tag=0,((_tmp79F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7A0[0]=& _tmp79F,((_tmp7A0[1]=& _tmp79E,Cyc_Tcutil_terr(e2->loc,((_tmp7A1="expecting either %s or int but found %s",_tag_dyneither(_tmp7A1,sizeof(char),40))),_tag_dyneither(_tmp7A0,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp7A4;void*_tmp7A3;(_tmp7A3=0,Cyc_Tcutil_warn(e1->loc,((_tmp7A4="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp7A4,sizeof(char),60))),_tag_dyneither(_tmp7A3,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp7A7;void*_tmp7A6;(_tmp7A6=0,Cyc_Tcutil_warn(e1->loc,((_tmp7A7="thin pointer subtraction!",_tag_dyneither(_tmp7A7,sizeof(char),26))),_tag_dyneither(_tmp7A6,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp7AA;void*_tmp7A9;(_tmp7A9=0,Cyc_Tcutil_warn(e1->loc,((_tmp7AA="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp7AA,sizeof(char),56))),_tag_dyneither(_tmp7A9,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp7AD;void*_tmp7AC;(_tmp7AC=0,Cyc_Tcutil_warn(e1->loc,((_tmp7AD="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp7AD,sizeof(char),51))),_tag_dyneither(_tmp7AC,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 600
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 603
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 611
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 616
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 629
pointer_cmp: {
struct _tuple0 _tmp7AE;struct _tuple0 _tmp14C=(_tmp7AE.f1=Cyc_Tcutil_compress(t1),((_tmp7AE.f2=Cyc_Tcutil_compress(t2),_tmp7AE)));struct _tuple0 _tmp14D=_tmp14C;void*_tmp14F;void*_tmp151;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp14E->tag != 5)goto _LLB0;else{_tmp14F=(_tmp14E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp150->tag != 5)goto _LLB0;else{_tmp151=(_tmp150->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp14F,_tmp151))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp152->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp153->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp7B3;void*_tmp7B2[2];struct Cyc_String_pa_PrintArg_struct _tmp7B1;struct Cyc_String_pa_PrintArg_struct _tmp7B0;(_tmp7B0.tag=0,((_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B1.tag=0,((_tmp7B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B2[0]=& _tmp7B1,((_tmp7B2[1]=& _tmp7B0,Cyc_Tcutil_terr(loc,((_tmp7B3="comparison not allowed between %s and %s",_tag_dyneither(_tmp7B3,sizeof(char),41))),_tag_dyneither(_tmp7B2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 645
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 647
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 653
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp7B4;struct _tuple0 _tmp159=(_tmp7B4.f1=t1,((_tmp7B4.f2=t2,_tmp7B4)));struct _tuple0 _tmp15A=_tmp159;void*_tmp15C;void*_tmp15E;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f1;if(_tmp15B->tag != 15)goto _LLB7;else{_tmp15C=(void*)_tmp15B->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f2;if(_tmp15D->tag != 15)goto _LLB7;else{_tmp15E=(void*)_tmp15D->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp7B9;void*_tmp7B8[2];struct Cyc_String_pa_PrintArg_struct _tmp7B7;struct Cyc_String_pa_PrintArg_struct _tmp7B6;(_tmp7B6.tag=0,((_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7B7.tag=0,((_tmp7B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7B8[0]=& _tmp7B7,((_tmp7B8[1]=& _tmp7B6,Cyc_Tcutil_terr(loc,((_tmp7B9="comparison not allowed between %s and %s",_tag_dyneither(_tmp7B9,sizeof(char),41))),_tag_dyneither(_tmp7B8,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 669
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 671
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 675
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 678
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 688
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 691
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 696
const char*_tmp7BC;void*_tmp7BB;(_tmp7BB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7BC="bad binary primop",_tag_dyneither(_tmp7BC,sizeof(char),18))),_tag_dyneither(_tmp7BB,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp167=Cyc_Tcexp_tcExp(te,topt,_tmp166);
if(!Cyc_Tcutil_is_numeric(_tmp166)){
const char*_tmp7C0;void*_tmp7BF[1];struct Cyc_String_pa_PrintArg_struct _tmp7BE;(_tmp7BE.tag=0,((_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp167)),((_tmp7BF[0]=& _tmp7BE,Cyc_Tcutil_terr(_tmp166->loc,((_tmp7C0="expecting numeric type but found %s",_tag_dyneither(_tmp7C0,sizeof(char),36))),_tag_dyneither(_tmp7BF,sizeof(void*),1)))))));}
return _tmp167;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmp16B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_tmp16B){case 0: _LLCC: {
const char*_tmp7C3;void*_tmp7C2;return(_tmp7C2=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C3="primitive operator has 0 arguments",_tag_dyneither(_tmp7C3,sizeof(char),35))),_tag_dyneither(_tmp7C2,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp7C6;void*_tmp7C5;return(_tmp7C5=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7C6="primitive operator has > 2 arguments",_tag_dyneither(_tmp7C6,sizeof(char),37))),_tag_dyneither(_tmp7C5,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp170=Cyc_Tcutil_compress(t);
void*_tmp171=_tmp170;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypefield*_tmp177;void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_List_List*_tmp17C;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp171;if(_tmp172->tag != 11)goto _LLD4;else{if((((_tmp172->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp173=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp172->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp173->impl == 0){
{const char*_tmp7C9;void*_tmp7C8;(_tmp7C8=0,Cyc_Tcutil_terr(loc,((_tmp7C9="attempt to write an abstract aggregate",_tag_dyneither(_tmp7C9,sizeof(char),39))),_tag_dyneither(_tmp7C8,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp173->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp171;if(_tmp174->tag != 12)goto _LLD6;else{_tmp175=_tmp174->f2;}}_LLD5:
# 736
 for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
struct Cyc_Absyn_Aggrfield*_tmp17F=(struct Cyc_Absyn_Aggrfield*)_tmp175->hd;
if((_tmp17F->tq).real_const){
{const char*_tmp7CD;void*_tmp7CC[1];struct Cyc_String_pa_PrintArg_struct _tmp7CB;(_tmp7CB.tag=0,((_tmp7CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp17F->name),((_tmp7CC[0]=& _tmp7CB,Cyc_Tcutil_terr(loc,((_tmp7CD="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7CD,sizeof(char),56))),_tag_dyneither(_tmp7CC,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17F->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp171;if(_tmp176->tag != 4)goto _LLD8;else{if((((_tmp176->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp177=((struct _tuple2)(((_tmp176->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp183=_tmp177->typs;for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple18*_tmp184=(struct _tuple18*)_tmp183->hd;struct Cyc_Absyn_Tqual _tmp186;void*_tmp187;struct _tuple18*_tmp185=_tmp184;_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;
if(_tmp186.real_const){
{const char*_tmp7D1;void*_tmp7D0[1];struct Cyc_String_pa_PrintArg_struct _tmp7CF;(_tmp7CF.tag=0,((_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp177->name)),((_tmp7D0[0]=& _tmp7CF,Cyc_Tcutil_terr(loc,((_tmp7D1="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7D1,sizeof(char),64))),_tag_dyneither(_tmp7D0,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp187))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp171;if(_tmp178->tag != 8)goto _LLDA;else{_tmp179=(_tmp178->f1).elt_type;_tmp17A=(_tmp178->f1).tq;}}_LLD9:
# 756
 if(_tmp17A.real_const){
{const char*_tmp7D4;void*_tmp7D3;(_tmp7D3=0,Cyc_Tcutil_terr(loc,((_tmp7D4="attempt to over-write a const array",_tag_dyneither(_tmp7D4,sizeof(char),36))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp179);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp171;if(_tmp17B->tag != 10)goto _LLDC;else{_tmp17C=_tmp17B->f1;}}_LLDB:
# 762
 for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct _tuple18*_tmp18D=(struct _tuple18*)_tmp17C->hd;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp190;struct _tuple18*_tmp18E=_tmp18D;_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;
if(_tmp18F.real_const){
{const char*_tmp7D7;void*_tmp7D6;(_tmp7D6=0,Cyc_Tcutil_terr(loc,((_tmp7D7="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7D7,sizeof(char),56))),_tag_dyneither(_tmp7D6,sizeof(void*),0)));}
return 0;}
# 768
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp190))return 0;}
# 770
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 778
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 781
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp193=e->r;void*_tmp194=_tmp193;struct Cyc_Absyn_Vardecl*_tmp197;struct Cyc_Absyn_Vardecl*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct _dyneither_ptr*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp195->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp196=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp195->f2);if(_tmp196->tag != 3)goto _LLE1;else{_tmp197=_tmp196->f1;}}}_LLE0:
 _tmp19A=_tmp197;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp198->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp198->f2);if(_tmp199->tag != 4)goto _LLE3;else{_tmp19A=_tmp199->f1;}}}_LLE2:
 _tmp19D=_tmp19A;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19B->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp19B->f2);if(_tmp19C->tag != 5)goto _LLE5;else{_tmp19D=_tmp19C->f1;}}}_LLE4:
 _tmp1A0=_tmp19D;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19E->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp19F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp19E->f2);if(_tmp19F->tag != 1)goto _LLE7;else{_tmp1A0=_tmp19F->f1;}}}_LLE6:
 if(!(_tmp1A0->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A1->tag != 22)goto _LLE9;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LLE8:
# 788
{void*_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp1A2->topt));void*_tmp1B1=_tmp1B0;struct Cyc_Absyn_Tqual _tmp1B3;struct Cyc_Absyn_Tqual _tmp1B5;struct Cyc_List_List*_tmp1B7;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B2->tag != 5)goto _LLF8;else{_tmp1B3=(_tmp1B2->f1).elt_tq;}}_LLF7:
 _tmp1B5=_tmp1B3;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B4->tag != 8)goto _LLFA;else{_tmp1B5=(_tmp1B4->f1).tq;}}_LLF9:
 if(!_tmp1B5.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B6->tag != 10)goto _LLFC;else{_tmp1B7=_tmp1B6->f1;}}_LLFB: {
# 792
struct _tuple15 _tmp1B8=Cyc_Evexp_eval_const_uint_exp(_tmp1A3);unsigned int _tmp1BA;int _tmp1BB;struct _tuple15 _tmp1B9=_tmp1B8;_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;
if(!_tmp1BB){
{const char*_tmp7DA;void*_tmp7D9;(_tmp7D9=0,Cyc_Tcutil_terr(e->loc,((_tmp7DA="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7DA,sizeof(char),47))),_tag_dyneither(_tmp7D9,sizeof(void*),0)));}
return;}
# 797
{struct _handler_cons _tmp1BE;_push_handler(& _tmp1BE);{int _tmp1C0=0;if(setjmp(_tmp1BE.handler))_tmp1C0=1;if(!_tmp1C0){
{struct _tuple18*_tmp1C1=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1B7,(int)_tmp1BA);struct Cyc_Absyn_Tqual _tmp1C3;struct _tuple18*_tmp1C2=_tmp1C1;_tmp1C3=_tmp1C2->f1;
if(!_tmp1C3.real_const){_npop_handler(0);return;}}
# 798
;_pop_handler();}else{void*_tmp1BF=(void*)_exn_thrown;void*_tmp1C5=_tmp1BF;void*_tmp1C7;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1C6=(struct Cyc_List_Nth_exn_struct*)_tmp1C5;if(_tmp1C6->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 800
 return;_LL101: _tmp1C7=_tmp1C5;_LL102:(void)_rethrow(_tmp1C7);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 804
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A4->tag != 20)goto _LLEB;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LLEA:
# 806
{void*_tmp1C8=Cyc_Tcutil_compress((void*)_check_null(_tmp1A5->topt));void*_tmp1C9=_tmp1C8;struct Cyc_Absyn_Aggrdecl**_tmp1CB;struct Cyc_List_List*_tmp1CD;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CA->tag != 11)goto _LL106;else{if((((_tmp1CA->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1CB=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1CA->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 808
struct Cyc_Absyn_Aggrfield*sf=
_tmp1CB == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1CB,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1CC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CC->tag != 12)goto _LL108;else{_tmp1CD=_tmp1CC->f2;}}_LL107: {
# 813
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1CD,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 818
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A7->tag != 21)goto _LLED;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LLEC:
# 820
{void*_tmp1CE=Cyc_Tcutil_compress((void*)_check_null(_tmp1A8->topt));void*_tmp1CF=_tmp1CE;void*_tmp1D1;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CF;if(_tmp1D0->tag != 5)goto _LL10D;else{_tmp1D1=(_tmp1D0->f1).elt_typ;}}_LL10C:
# 822
{void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D1);void*_tmp1D3=_tmp1D2;struct Cyc_Absyn_Aggrdecl**_tmp1D5;struct Cyc_List_List*_tmp1D7;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D4->tag != 11)goto _LL112;else{if((((_tmp1D4->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1D5=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1D4->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 824
struct Cyc_Absyn_Aggrfield*sf=
_tmp1D5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1D5,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D6->tag != 12)goto _LL114;else{_tmp1D7=_tmp1D6->f2;}}_LL113: {
# 829
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1D7,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 834
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 837
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AA->tag != 19)goto _LLEF;else{_tmp1AB=_tmp1AA->f1;}}_LLEE:
# 839
{void*_tmp1D8=Cyc_Tcutil_compress((void*)_check_null(_tmp1AB->topt));void*_tmp1D9=_tmp1D8;struct Cyc_Absyn_Tqual _tmp1DB;struct Cyc_Absyn_Tqual _tmp1DD;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 5)goto _LL119;else{_tmp1DB=(_tmp1DA->f1).elt_tq;}}_LL118:
 _tmp1DD=_tmp1DB;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DC->tag != 8)goto _LL11B;else{_tmp1DD=(_tmp1DC->f1).tq;}}_LL11A:
 if(!_tmp1DD.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 844
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AC->tag != 11)goto _LLF1;else{_tmp1AD=_tmp1AC->f1;}}_LLF0:
 _tmp1AF=_tmp1AD;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AE->tag != 12)goto _LLF3;else{_tmp1AF=_tmp1AE->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp1AF);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 849
const char*_tmp7DE;void*_tmp7DD[1];struct Cyc_String_pa_PrintArg_struct _tmp7DC;(_tmp7DC.tag=0,((_tmp7DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7DD[0]=& _tmp7DC,Cyc_Tcutil_terr(e->loc,((_tmp7DE="attempt to write a const location: %s",_tag_dyneither(_tmp7DE,sizeof(char),38))),_tag_dyneither(_tmp7DD,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp7E1;void*_tmp7E0;void*_tmp1E4=(_tmp7E0=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7E1="increment/decrement of non-lvalue",_tag_dyneither(_tmp7E1,sizeof(char),34))),_tag_dyneither(_tmp7E0,sizeof(void*),0)));_npop_handler(0);return _tmp1E4;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp7E4;void*_tmp7E3;(_tmp7E3=0,Cyc_Tcutil_terr(e->loc,((_tmp7E4="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7E4,sizeof(char),50))),_tag_dyneither(_tmp7E3,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp7E7;void*_tmp7E6;(_tmp7E6=0,Cyc_Tcutil_terr(e->loc,((_tmp7E7="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7E7,sizeof(char),54))),_tag_dyneither(_tmp7E6,sizeof(void*),0)));}}else{
# 872
const char*_tmp7EB;void*_tmp7EA[1];struct Cyc_String_pa_PrintArg_struct _tmp7E9;(_tmp7E9.tag=0,((_tmp7E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7EA[0]=& _tmp7E9,Cyc_Tcutil_terr(e->loc,((_tmp7EB="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7EB,sizeof(char),44))),_tag_dyneither(_tmp7EA,sizeof(void*),1)))))));}}{
# 874
void*_tmp1EC=t;_npop_handler(0);return _tmp1EC;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1ED=_new_region("r");struct _RegionHandle*r=& _tmp1ED;_push_region(r);{
const char*_tmp7EC;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp7EC="conditional expression",_tag_dyneither(_tmp7EC,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp7ED;struct Cyc_List_List _tmp1EF=(_tmp7ED.hd=e3,((_tmp7ED.tl=0,_tmp7ED)));
struct Cyc_List_List _tmp7EE;struct Cyc_List_List _tmp1F0=(_tmp7EE.hd=e2,((_tmp7EE.tl=& _tmp1EF,_tmp7EE)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1F0)){
{const char*_tmp7F3;void*_tmp7F2[2];struct Cyc_String_pa_PrintArg_struct _tmp7F1;struct Cyc_String_pa_PrintArg_struct _tmp7F0;(_tmp7F0.tag=0,((_tmp7F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7F1.tag=0,((_tmp7F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7F2[0]=& _tmp7F1,((_tmp7F2[1]=& _tmp7F0,Cyc_Tcutil_terr(loc,((_tmp7F3="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7F3,sizeof(char),48))),_tag_dyneither(_tmp7F2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1F5=t;_npop_handler(0);return _tmp1F5;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp7F4;Cyc_Tcexp_tcTest(te,e1,((_tmp7F4="logical-and expression",_tag_dyneither(_tmp7F4,sizeof(char),23))));}
{const char*_tmp7F5;Cyc_Tcexp_tcTest(te,e2,((_tmp7F5="logical-and expression",_tag_dyneither(_tmp7F5,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp7F6;Cyc_Tcexp_tcTest(te,e1,((_tmp7F6="logical-or expression",_tag_dyneither(_tmp7F6,sizeof(char),22))));}
{const char*_tmp7F7;Cyc_Tcexp_tcTest(te,e2,((_tmp7F7="logical-or expression",_tag_dyneither(_tmp7F7,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 917
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 924
struct _RegionHandle _tmp1FC=_new_region("r");struct _RegionHandle*r=& _tmp1FC;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 927
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 931
{void*_tmp1FD=Cyc_Tcutil_compress(t1);void*_tmp1FE=_tmp1FD;_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp7FA;void*_tmp7F9;(_tmp7F9=0,Cyc_Tcutil_terr(loc,((_tmp7FA="cannot assign to an array",_tag_dyneither(_tmp7FA,sizeof(char),26))),_tag_dyneither(_tmp7F9,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7FD;void*_tmp7FC;(_tmp7FC=0,Cyc_Tcutil_terr(loc,((_tmp7FD="type is abstract (can't determine size).",_tag_dyneither(_tmp7FD,sizeof(char),41))),_tag_dyneither(_tmp7FC,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp800;void*_tmp7FF;void*_tmp206=(_tmp7FF=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp800="assignment to non-lvalue",_tag_dyneither(_tmp800,sizeof(char),25))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));_npop_handler(0);return _tmp206;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp207=_new_region("temp");struct _RegionHandle*temp=& _tmp207;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp803;void*_tmp802;(_tmp802=0,Cyc_Tcutil_terr(e2->loc,((_tmp803="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp803,sizeof(char),49))),_tag_dyneither(_tmp802,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp808;void*_tmp807[2];struct Cyc_String_pa_PrintArg_struct _tmp806;struct Cyc_String_pa_PrintArg_struct _tmp805;void*_tmp20A=(_tmp805.tag=0,((_tmp805.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp806.tag=0,((_tmp806.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp807[0]=& _tmp806,((_tmp807[1]=& _tmp805,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp808="type mismatch: %s != %s",_tag_dyneither(_tmp808,sizeof(char),24))),_tag_dyneither(_tmp807,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp20B=_tmp20A;_npop_handler(1);return _tmp20B;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp210=(enum Cyc_Absyn_Primop)po->v;
void*_tmp211=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp210,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp211,t1) || Cyc_Tcutil_coerceable(_tmp211) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp80D;void*_tmp80C[2];struct Cyc_String_pa_PrintArg_struct _tmp80B;struct Cyc_String_pa_PrintArg_struct _tmp80A;void*_tmp212=
(_tmp80A.tag=0,((_tmp80A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp80B.tag=0,((_tmp80B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp80C[0]=& _tmp80B,((_tmp80C[1]=& _tmp80A,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp80D="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp80D,sizeof(char),82))),_tag_dyneither(_tmp80C,sizeof(void*),2)))))))))))));
# 963
Cyc_Tcutil_unify(_tmp211,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp213=_tmp212;_npop_handler(0);return _tmp213;};}{
# 967
void*_tmp218=_tmp211;_npop_handler(0);return _tmp218;};}{
# 969
void*_tmp219=t1;_npop_handler(0);return _tmp219;};};};
# 924
;_pop_region(r);}
# 973
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp21A=_new_region("r");struct _RegionHandle*r=& _tmp21A;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 978
void*_tmp21B=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp21B;};
# 974
;_pop_region(r);}
# 982
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 986
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 989
{void*_tmp21C=Cyc_Tcutil_compress(t1);void*_tmp21D=_tmp21C;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 7)goto _LL125;else{if(_tmp21E->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp21F->tag != 6)goto _LL127;else{if(_tmp21F->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp220->tag != 6)goto _LL129;else{if(_tmp220->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp221=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp223;struct Cyc_List_List*_tmp224;unsigned int _tmp225;enum Cyc_Absyn_Scope _tmp226;struct Cyc_Absyn_Datatypefield _tmp222=_tmp221;_tmp223=_tmp222.name;_tmp224=_tmp222.typs;_tmp225=_tmp222.loc;_tmp226=_tmp222.sc;
# 999
if(_tmp224 == 0  || _tmp224->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp224->hd)).f2);
# 1002
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1007
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp227=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp229;struct Cyc_List_List*_tmp22A;unsigned int _tmp22B;enum Cyc_Absyn_Scope _tmp22C;struct Cyc_Absyn_Datatypefield _tmp228=_tmp227;_tmp229=_tmp228.name;_tmp22A=_tmp228.typs;_tmp22B=_tmp228.loc;_tmp22C=_tmp228.sc;
# 1010
if(_tmp22A == 0  || _tmp22A->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp22A->hd)).f2);
# 1014
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1019
{const char*_tmp812;void*_tmp811[2];struct Cyc_String_pa_PrintArg_struct _tmp810;struct Cyc_String_pa_PrintArg_struct _tmp80F;(_tmp80F.tag=0,((_tmp80F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp810.tag=0,((_tmp810.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp811[0]=& _tmp810,((_tmp811[1]=& _tmp80F,Cyc_Tcutil_terr(e->loc,((_tmp812="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp812,sizeof(char),54))),_tag_dyneither(_tmp811,sizeof(void*),2)))))))))))));}
return 0;}
# 1025
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1031
struct Cyc_List_List*_tmp231=args;
int _tmp232=0;
struct _RegionHandle _tmp233=_new_region("ter");struct _RegionHandle*ter=& _tmp233;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp234=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp235=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp234);
Cyc_Tcexp_tcExp(_tmp235,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1041
void*_tmp236=t;void*_tmp238;struct Cyc_Absyn_Tqual _tmp239;void*_tmp23A;union Cyc_Absyn_Constraint*_tmp23B;union Cyc_Absyn_Constraint*_tmp23C;union Cyc_Absyn_Constraint*_tmp23D;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236;if(_tmp237->tag != 5)goto _LL12E;else{_tmp238=(_tmp237->f1).elt_typ;_tmp239=(_tmp237->f1).elt_tq;_tmp23A=((_tmp237->f1).ptr_atts).rgn;_tmp23B=((_tmp237->f1).ptr_atts).nullable;_tmp23C=((_tmp237->f1).ptr_atts).bounds;_tmp23D=((_tmp237->f1).ptr_atts).zero_term;}}_LL12D:
# 1046
 Cyc_Tcenv_check_rgn_accessible(_tmp235,loc,_tmp23A);
# 1048
Cyc_Tcutil_check_nonzero_bound(loc,_tmp23C);{
void*_tmp23E=Cyc_Tcutil_compress(_tmp238);void*_tmp23F=_tmp23E;struct Cyc_List_List*_tmp241;void*_tmp242;struct Cyc_Absyn_Tqual _tmp243;void*_tmp244;struct Cyc_List_List*_tmp245;int _tmp246;struct Cyc_Absyn_VarargInfo*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp249;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_List_List*_tmp24B;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp24D;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23F;if(_tmp240->tag != 9)goto _LL133;else{_tmp241=(_tmp240->f1).tvars;_tmp242=(_tmp240->f1).effect;_tmp243=(_tmp240->f1).ret_tqual;_tmp244=(_tmp240->f1).ret_typ;_tmp245=(_tmp240->f1).args;_tmp246=(_tmp240->f1).c_varargs;_tmp247=(_tmp240->f1).cyc_varargs;_tmp248=(_tmp240->f1).rgn_po;_tmp249=(_tmp240->f1).attributes;_tmp24A=(_tmp240->f1).requires_clause;_tmp24B=(_tmp240->f1).requires_relns;_tmp24C=(_tmp240->f1).ensures_clause;_tmp24D=(_tmp240->f1).ensures_relns;}}_LL132:
# 1053
 if(_tmp241 != 0  || _tmp248 != 0){
const char*_tmp815;void*_tmp814;(_tmp814=0,Cyc_Tcutil_terr(e->loc,((_tmp815="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp815,sizeof(char),79))),_tag_dyneither(_tmp814,sizeof(void*),0)));}
# 1057
if(topt != 0)Cyc_Tcutil_unify(_tmp244,*topt);
# 1059
while(_tmp231 != 0  && _tmp245 != 0){
# 1061
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
void*t2=(*((struct _tuple9*)_tmp245->hd)).f3;
Cyc_Tcexp_tcExp(_tmp235,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,t2,& alias_coercion)){
const char*_tmp816;struct _dyneither_ptr s0=(_tmp816="actual argument has type ",_tag_dyneither(_tmp816,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp817;struct _dyneither_ptr s2=(_tmp817=" but formal has type ",_tag_dyneither(_tmp817,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp825;struct Cyc_String_pa_PrintArg_struct _tmp824;struct Cyc_String_pa_PrintArg_struct _tmp823;struct Cyc_String_pa_PrintArg_struct _tmp822;void*_tmp821[4];const char*_tmp820;void*_tmp81F;(_tmp81F=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp823.tag=0,((_tmp823.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp822.tag=0,((_tmp822.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp821[0]=& _tmp822,((_tmp821[1]=& _tmp823,((_tmp821[2]=& _tmp824,((_tmp821[3]=& _tmp825,Cyc_aprintf(((_tmp820="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp820,sizeof(char),15))),_tag_dyneither(_tmp821,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp81F,sizeof(void*),0)));}else{
# 1073
struct Cyc_String_pa_PrintArg_struct _tmp833;struct Cyc_String_pa_PrintArg_struct _tmp832;struct Cyc_String_pa_PrintArg_struct _tmp831;struct Cyc_String_pa_PrintArg_struct _tmp830;void*_tmp82F[4];const char*_tmp82E;void*_tmp82D;(_tmp82D=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp833.tag=0,((_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp832.tag=0,((_tmp832.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp831.tag=0,((_tmp831.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp82F[0]=& _tmp830,((_tmp82F[1]=& _tmp831,((_tmp82F[2]=& _tmp832,((_tmp82F[3]=& _tmp833,Cyc_aprintf(((_tmp82E="%s%s%s%s.",_tag_dyneither(_tmp82E,sizeof(char),10))),_tag_dyneither(_tmp82F,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion){
struct Cyc_List_List*_tmp834;*alias_arg_exps=((_tmp834=_cycalloc(sizeof(*_tmp834)),((_tmp834->hd=(void*)_tmp232,((_tmp834->tl=*alias_arg_exps,_tmp834))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp837;void*_tmp836;(_tmp836=0,Cyc_Tcutil_terr(e1->loc,((_tmp837="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp837,sizeof(char),49))),_tag_dyneither(_tmp836,sizeof(void*),0)));}
_tmp231=_tmp231->tl;
_tmp245=_tmp245->tl;
++ _tmp232;}{
# 1089
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp249;for(0;a != 0;a=a->tl){
void*_tmp263=(void*)a->hd;void*_tmp264=_tmp263;enum Cyc_Absyn_Format_Type _tmp266;int _tmp267;int _tmp268;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp265=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp264;if(_tmp265->tag != 19)goto _LL138;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;_tmp268=_tmp265->f3;}}_LL137:
# 1093
{struct _handler_cons _tmp269;_push_handler(& _tmp269);{int _tmp26B=0;if(setjmp(_tmp269.handler))_tmp26B=1;if(!_tmp26B){
# 1095
{struct Cyc_Absyn_Exp*_tmp26C=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp267 - 1);
# 1097
struct Cyc_Core_Opt*fmt_args;
if(_tmp268 == 0)
fmt_args=0;else{
# 1101
struct Cyc_Core_Opt*_tmp838;fmt_args=((_tmp838=_cycalloc(sizeof(*_tmp838)),((_tmp838->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp268 - 1),_tmp838))));}
args_already_checked=1;{
struct _RegionHandle _tmp26E=_new_region("temp");struct _RegionHandle*temp=& _tmp26E;_push_region(temp);
switch(_tmp266){case Cyc_Absyn_Printf_ft: _LL13A:
# 1106
 Cyc_Tcexp_check_format_args(_tmp235,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1109
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1111
 Cyc_Tcexp_check_format_args(_tmp235,_tmp26C,fmt_args,_tmp268 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1114
break;}
# 1104
;_pop_region(temp);};}
# 1095
;_pop_handler();}else{void*_tmp26A=(void*)_exn_thrown;void*_tmp270=_tmp26A;void*_tmp272;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp271=(struct Cyc_List_Nth_exn_struct*)_tmp270;if(_tmp271->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1118
{const char*_tmp83B;void*_tmp83A;(_tmp83A=0,Cyc_Tcutil_terr(loc,((_tmp83B="bad format arguments",_tag_dyneither(_tmp83B,sizeof(char),21))),_tag_dyneither(_tmp83A,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp272=_tmp270;_LL141:(void)_rethrow(_tmp272);_LL13D:;}};}
# 1120
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1124
if(_tmp245 != 0){const char*_tmp83E;void*_tmp83D;(_tmp83D=0,Cyc_Tcutil_terr(loc,((_tmp83E="too few arguments for function",_tag_dyneither(_tmp83E,sizeof(char),31))),_tag_dyneither(_tmp83D,sizeof(void*),0)));}else{
# 1126
if((_tmp231 != 0  || _tmp246) || _tmp247 != 0){
if(_tmp246)
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
Cyc_Tcexp_tcExp(_tmp235,0,(struct Cyc_Absyn_Exp*)_tmp231->hd);}else{
if(_tmp247 == 0){
const char*_tmp841;void*_tmp840;(_tmp840=0,Cyc_Tcutil_terr(loc,((_tmp841="too many arguments for function",_tag_dyneither(_tmp841,sizeof(char),32))),_tag_dyneither(_tmp840,sizeof(void*),0)));}else{
# 1133
struct Cyc_Absyn_VarargInfo _tmp279=*_tmp247;void*_tmp27B;int _tmp27C;struct Cyc_Absyn_VarargInfo _tmp27A=_tmp279;_tmp27B=_tmp27A.type;_tmp27C=_tmp27A.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp842;struct Cyc_Absyn_VarargCallInfo*_tmp27D=(_tmp842=_cycalloc(sizeof(*_tmp842)),((_tmp842->num_varargs=0,((_tmp842->injectors=0,((_tmp842->vai=_tmp247,_tmp842)))))));
# 1137
*vararg_call_info=_tmp27D;
# 1139
if(!_tmp27C)
# 1141
for(0;_tmp231 != 0;(_tmp231=_tmp231->tl,_tmp232 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
++ _tmp27D->num_varargs;
Cyc_Tcexp_tcExp(_tmp235,& _tmp27B,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,_tmp27B,& alias_coercion)){
{const char*_tmp847;void*_tmp846[2];struct Cyc_String_pa_PrintArg_struct _tmp845;struct Cyc_String_pa_PrintArg_struct _tmp844;(_tmp844.tag=0,((_tmp844.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp846[0]=& _tmp845,((_tmp846[1]=& _tmp844,Cyc_Tcutil_terr(loc,((_tmp847="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp847,sizeof(char),49))),_tag_dyneither(_tmp846,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1151
if(alias_coercion){
struct Cyc_List_List*_tmp848;*alias_arg_exps=((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848->hd=(void*)_tmp232,((_tmp848->tl=*alias_arg_exps,_tmp848))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp27B) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp84B;void*_tmp84A;(_tmp84A=0,Cyc_Tcutil_terr(e1->loc,((_tmp84B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp84B,sizeof(char),49))),_tag_dyneither(_tmp84A,sizeof(void*),0)));}}else{
# 1160
void*_tmp285=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp27B));void*_tmp286=_tmp285;struct Cyc_Absyn_Datatypedecl*_tmp288;struct Cyc_List_List*_tmp289;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp287=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp286;if(_tmp287->tag != 3)goto _LL145;else{if((((_tmp287->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp288=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp287->f1).datatype_info).KnownDatatype).val);_tmp289=(_tmp287->f1).targs;}}_LL144: {
# 1164
struct Cyc_Absyn_Datatypedecl*_tmp28A=*Cyc_Tcenv_lookup_datatypedecl(_tmp235,loc,_tmp288->name);
struct Cyc_List_List*fields=0;
if(_tmp28A->fields == 0){
const char*_tmp84F;void*_tmp84E[1];struct Cyc_String_pa_PrintArg_struct _tmp84D;(_tmp84D.tag=0,((_tmp84D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp27B)),((_tmp84E[0]=& _tmp84D,Cyc_Tcutil_terr(loc,((_tmp84F="can't inject into abstract datatype %s",_tag_dyneither(_tmp84F,sizeof(char),39))),_tag_dyneither(_tmp84E,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp28A->fields))->v;}
# 1176
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp27B),Cyc_Tcenv_curr_rgn(_tmp235))){
const char*_tmp854;void*_tmp853[2];struct Cyc_String_pa_PrintArg_struct _tmp852;struct Cyc_String_pa_PrintArg_struct _tmp851;(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp235))),((_tmp852.tag=0,((_tmp852.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp27B)),((_tmp853[0]=& _tmp852,((_tmp853[1]=& _tmp851,Cyc_Tcutil_terr(loc,((_tmp854="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp854,sizeof(char),49))),_tag_dyneither(_tmp853,sizeof(void*),2)))))))))))));}{
# 1180
struct _RegionHandle _tmp292=_new_region("rgn");struct _RegionHandle*rgn=& _tmp292;_push_region(rgn);{
struct Cyc_List_List*_tmp293=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp28A->tvs,_tmp289);
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
++ _tmp27D->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
# 1186
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp235,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp857;void*_tmp856;(_tmp856=0,Cyc_Tcutil_terr(e1->loc,((_tmp857="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp857,sizeof(char),49))),_tag_dyneither(_tmp856,sizeof(void*),0)));}}{
# 1192
struct Cyc_Absyn_Datatypefield*_tmp296=Cyc_Tcexp_tcInjection(_tmp235,e1,Cyc_Tcutil_pointer_elt_type(_tmp27B),rgn,_tmp293,fields);
# 1194
if(_tmp296 != 0){
struct Cyc_List_List*_tmp858;_tmp27D->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp27D->injectors,(
(_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858->hd=_tmp296,((_tmp858->tl=0,_tmp858)))))));}};};}}
# 1200
_npop_handler(0);goto _LL142;
# 1180
;_pop_region(rgn);};}_LL145:;_LL146:
# 1201
{const char*_tmp85B;void*_tmp85A;(_tmp85A=0,Cyc_Tcutil_terr(loc,((_tmp85B="bad inject vararg type",_tag_dyneither(_tmp85B,sizeof(char),23))),_tag_dyneither(_tmp85A,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1206
if(*alias_arg_exps == 0)
# 1215 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp235,loc,(void*)_check_null(_tmp242));{
# 1217
void*_tmp29B=_tmp244;_npop_handler(0);return _tmp29B;};};_LL133:;_LL134: {
const char*_tmp85F;void*_tmp85E[1];struct Cyc_String_pa_PrintArg_struct _tmp85D;void*_tmp29F=(_tmp85D.tag=0,((_tmp85D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp85E[0]=& _tmp85D,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp85F="expected pointer to function but found %s",_tag_dyneither(_tmp85F,sizeof(char),42))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))));_npop_handler(0);return _tmp29F;}_LL130:;};_LL12E:;_LL12F: {
# 1220
const char*_tmp863;void*_tmp862[1];struct Cyc_String_pa_PrintArg_struct _tmp861;void*_tmp2A3=(_tmp861.tag=0,((_tmp861.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp862[0]=& _tmp861,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp863="expected pointer to function but found %s",_tag_dyneither(_tmp863,sizeof(char),42))),_tag_dyneither(_tmp862,sizeof(void*),1)))))));_npop_handler(0);return _tmp2A3;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1226 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp2A4=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp2A4,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp2A4,& bogus)){
const char*_tmp868;void*_tmp867[2];struct Cyc_String_pa_PrintArg_struct _tmp866;struct Cyc_String_pa_PrintArg_struct _tmp865;(_tmp865.tag=0,((_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp866.tag=0,((_tmp866.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1231
Cyc_Absynpp_typ2string(_tmp2A4)),((_tmp867[0]=& _tmp866,((_tmp867[1]=& _tmp865,Cyc_Tcutil_terr(loc,((_tmp868="expected %s but found %s",_tag_dyneither(_tmp868,sizeof(char),25))),_tag_dyneither(_tmp867,sizeof(void*),2)))))))))))));}
# 1233
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1238
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1240
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1242
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp2A9=t1;void*_tmp2AB;struct Cyc_Absyn_Tqual _tmp2AC;void*_tmp2AD;union Cyc_Absyn_Constraint*_tmp2AE;union Cyc_Absyn_Constraint*_tmp2AF;union Cyc_Absyn_Constraint*_tmp2B0;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2AA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A9;if(_tmp2AA->tag != 5)goto _LL14A;else{_tmp2AB=(_tmp2AA->f1).elt_typ;_tmp2AC=(_tmp2AA->f1).elt_tq;_tmp2AD=((_tmp2AA->f1).ptr_atts).rgn;_tmp2AE=((_tmp2AA->f1).ptr_atts).nullable;_tmp2AF=((_tmp2AA->f1).ptr_atts).bounds;_tmp2B0=((_tmp2AA->f1).ptr_atts).zero_term;}}_LL149:
# 1246
{void*_tmp2B1=Cyc_Tcutil_compress(_tmp2AB);void*_tmp2B2=_tmp2B1;struct Cyc_List_List*_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Tqual _tmp2B6;void*_tmp2B7;struct Cyc_List_List*_tmp2B8;int _tmp2B9;struct Cyc_Absyn_VarargInfo*_tmp2BA;struct Cyc_List_List*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BF;struct Cyc_List_List*_tmp2C0;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2B3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2B2;if(_tmp2B3->tag != 9)goto _LL14F;else{_tmp2B4=(_tmp2B3->f1).tvars;_tmp2B5=(_tmp2B3->f1).effect;_tmp2B6=(_tmp2B3->f1).ret_tqual;_tmp2B7=(_tmp2B3->f1).ret_typ;_tmp2B8=(_tmp2B3->f1).args;_tmp2B9=(_tmp2B3->f1).c_varargs;_tmp2BA=(_tmp2B3->f1).cyc_varargs;_tmp2BB=(_tmp2B3->f1).rgn_po;_tmp2BC=(_tmp2B3->f1).attributes;_tmp2BD=(_tmp2B3->f1).requires_clause;_tmp2BE=(_tmp2B3->f1).requires_relns;_tmp2BF=(_tmp2B3->f1).ensures_clause;_tmp2C0=(_tmp2B3->f1).ensures_relns;}}_LL14E: {
# 1248
struct _RegionHandle _tmp2C1=_new_region("temp");struct _RegionHandle*temp=& _tmp2C1;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1251
for(0;ts != 0  && _tmp2B4 != 0;(ts=ts->tl,_tmp2B4=_tmp2B4->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2B4->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp86B;struct Cyc_List_List*_tmp86A;instantiation=(
(_tmp86A=_region_malloc(temp,sizeof(*_tmp86A)),((_tmp86A->hd=((_tmp86B=_region_malloc(temp,sizeof(*_tmp86B)),((_tmp86B->f1=(struct Cyc_Absyn_Tvar*)_tmp2B4->hd,((_tmp86B->f2=(void*)ts->hd,_tmp86B)))))),((_tmp86A->tl=instantiation,_tmp86A))))));};}
# 1258
if(ts != 0){
const char*_tmp86E;void*_tmp86D;void*_tmp2C6=
(_tmp86D=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp86E="too many type variables in instantiation",_tag_dyneither(_tmp86E,sizeof(char),41))),_tag_dyneither(_tmp86D,sizeof(void*),0)));_npop_handler(0);return _tmp2C6;}
# 1264
if(_tmp2B4 == 0){
_tmp2BB=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp2BB);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2BB);
_tmp2BB=0;}{
# 1269
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp874;struct Cyc_Absyn_FnInfo _tmp873;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp872;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872[0]=((_tmp874.tag=9,((_tmp874.f1=((_tmp873.tvars=_tmp2B4,((_tmp873.effect=_tmp2B5,((_tmp873.ret_tqual=_tmp2B6,((_tmp873.ret_typ=_tmp2B7,((_tmp873.args=_tmp2B8,((_tmp873.c_varargs=_tmp2B9,((_tmp873.cyc_varargs=_tmp2BA,((_tmp873.rgn_po=_tmp2BB,((_tmp873.attributes=_tmp2BC,((_tmp873.requires_clause=_tmp2BD,((_tmp873.requires_relns=_tmp2BE,((_tmp873.ensures_clause=_tmp2BF,((_tmp873.ensures_relns=_tmp2C0,_tmp873)))))))))))))))))))))))))),_tmp874)))),_tmp872)))));
# 1274
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp87A;struct Cyc_Absyn_PtrInfo _tmp879;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp878;void*_tmp2CA=(void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp87A.tag=5,((_tmp87A.f1=((_tmp879.elt_typ=new_fn_typ,((_tmp879.elt_tq=_tmp2AC,((_tmp879.ptr_atts=(((_tmp879.ptr_atts).rgn=_tmp2AD,(((_tmp879.ptr_atts).nullable=_tmp2AE,(((_tmp879.ptr_atts).bounds=_tmp2AF,(((_tmp879.ptr_atts).zero_term=_tmp2B0,(((_tmp879.ptr_atts).ptrloc=0,_tmp879.ptr_atts)))))))))),_tmp879)))))),_tmp87A)))),_tmp878))));_npop_handler(0);return _tmp2CA;};}
# 1249
;_pop_region(temp);}_LL14F:;_LL150:
# 1276
 goto _LL14C;_LL14C:;}
# 1278
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1281
const char*_tmp87E;void*_tmp87D[1];struct Cyc_String_pa_PrintArg_struct _tmp87C;return(_tmp87C.tag=0,((_tmp87C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp87D[0]=& _tmp87C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp87E="expecting polymorphic type but found %s",_tag_dyneither(_tmp87E,sizeof(char),40))),_tag_dyneither(_tmp87D,sizeof(void*),1)))))));};};}
# 1286
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1288
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1292
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1297
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1304
Cyc_Tcutil_unify(t2,t);{
const char*_tmp883;void*_tmp882[2];struct Cyc_String_pa_PrintArg_struct _tmp881;struct Cyc_String_pa_PrintArg_struct _tmp880;void*_tmp2D1=(_tmp880.tag=0,((_tmp880.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp882[0]=& _tmp881,((_tmp882[1]=& _tmp880,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp883="cannot cast %s to %s",_tag_dyneither(_tmp883,sizeof(char),21))),_tag_dyneither(_tmp882,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2D1;};}}}
# 1314
{struct _tuple0 _tmp884;struct _tuple0 _tmp2D6=(_tmp884.f1=e->r,((_tmp884.f2=Cyc_Tcutil_compress(t),_tmp884)));struct _tuple0 _tmp2D7=_tmp2D6;int _tmp2D9;union Cyc_Absyn_Constraint*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DC;union Cyc_Absyn_Constraint*_tmp2DD;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2D8=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2D7.f1;if(_tmp2D8->tag != 33)goto _LL154;else{_tmp2D9=(_tmp2D8->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7.f2;if(_tmp2DA->tag != 5)goto _LL154;else{_tmp2DB=((_tmp2DA->f1).ptr_atts).nullable;_tmp2DC=((_tmp2DA->f1).ptr_atts).bounds;_tmp2DD=((_tmp2DA->f1).ptr_atts).zero_term;}};_LL153:
# 1318
 if((_tmp2D9  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DD)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2DB)){
void*_tmp2DE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2DC);void*_tmp2DF=_tmp2DE;struct Cyc_Absyn_Exp*_tmp2E1;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2E0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2DF;if(_tmp2E0->tag != 1)goto _LL159;else{_tmp2E1=_tmp2E0->f1;}}_LL158:
# 1321
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2E1)).f1 == 1){
const char*_tmp887;void*_tmp886;(_tmp886=0,Cyc_Tcutil_warn(loc,((_tmp887="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp887,sizeof(char),60))),_tag_dyneither(_tmp886,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1327
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1330
return t;};}
# 1334
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1336
void**_tmp2E5=0;
struct Cyc_Absyn_Tqual _tmp2E6=Cyc_Absyn_empty_tqual(0);
int _tmp2E7=0;
if(topt != 0){
void*_tmp2E8=Cyc_Tcutil_compress(*topt);void*_tmp2E9=_tmp2E8;void**_tmp2EB;struct Cyc_Absyn_Tqual _tmp2EC;union Cyc_Absyn_Constraint*_tmp2ED;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EA->tag != 5)goto _LL15E;else{_tmp2EB=(void**)&(_tmp2EA->f1).elt_typ;_tmp2EC=(_tmp2EA->f1).elt_tq;_tmp2ED=((_tmp2EA->f1).ptr_atts).nullable;}}_LL15D:
# 1342
 _tmp2E5=_tmp2EB;
_tmp2E6=_tmp2EC;
_tmp2E7=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2ED);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1356
struct _RegionHandle _tmp2EE=_new_region("r");struct _RegionHandle*r=& _tmp2EE;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp2E5,e);
# 1360
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp88A;void*_tmp889;(_tmp889=0,Cyc_Tcutil_terr(e->loc,((_tmp88A="Cannot take the address of an alias-free path",_tag_dyneither(_tmp88A,sizeof(char),46))),_tag_dyneither(_tmp889,sizeof(void*),0)));}
# 1367
{void*_tmp2F1=e->r;void*_tmp2F2=_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F4;struct Cyc_Absyn_Exp*_tmp2F5;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2F3=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2F2;if(_tmp2F3->tag != 22)goto _LL163;else{_tmp2F4=_tmp2F3->f1;_tmp2F5=_tmp2F3->f2;}}_LL162:
# 1369
{void*_tmp2F6=Cyc_Tcutil_compress((void*)_check_null(_tmp2F4->topt));void*_tmp2F7=_tmp2F6;_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F7;if(_tmp2F8->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1374
 e0->r=(Cyc_Absyn_add_exp(_tmp2F4,_tmp2F5,0))->r;{
void*_tmp2F9=Cyc_Tcexp_tcPlus(te,_tmp2F4,_tmp2F5);_npop_handler(0);return _tmp2F9;};_LL165:;}
# 1377
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1382
{void*_tmp2FA=e->r;void*_tmp2FB=_tmp2FA;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2FC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FB;if(_tmp2FC->tag != 20)goto _LL16D;else{if(_tmp2FC->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2FD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FB;if(_tmp2FD->tag != 21)goto _LL16F;else{if(_tmp2FD->f3 != 1)goto _LL16F;}}_LL16E:
# 1385
{const char*_tmp88D;void*_tmp88C;(_tmp88C=0,Cyc_Tcutil_terr(e->loc,((_tmp88D="cannot take the address of a @tagged union member",_tag_dyneither(_tmp88D,sizeof(char),50))),_tag_dyneither(_tmp88C,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1391
struct _tuple14 _tmp300=Cyc_Tcutil_addressof_props(te,e);int _tmp302;void*_tmp303;struct _tuple14 _tmp301=_tmp300;_tmp302=_tmp301.f1;_tmp303=_tmp301.f2;
# 1393
if(Cyc_Tcutil_is_noalias_region(_tmp303,0)){
const char*_tmp890;void*_tmp88F;(_tmp88F=0,Cyc_Tcutil_terr(e->loc,((_tmp890="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp890,sizeof(char),60))),_tag_dyneither(_tmp88F,sizeof(void*),0)));}{
# 1396
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp302){
tq.print_const=1;
tq.real_const=1;}{
# 1402
void*t=_tmp2E7?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp303,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp303,tq,Cyc_Absyn_false_conref);
void*_tmp306=t;_npop_handler(0);return _tmp306;};};};
# 1356
;_pop_region(r);};}
# 1409
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1413
return Cyc_Absyn_uint_typ;
# 1415
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp894;void*_tmp893[1];struct Cyc_String_pa_PrintArg_struct _tmp892;(_tmp892.tag=0,((_tmp892.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp893[0]=& _tmp892,Cyc_Tcutil_terr(loc,((_tmp894="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp894,sizeof(char),55))),_tag_dyneither(_tmp893,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp30A=Cyc_Tcutil_compress(*topt);void*_tmp30B=_tmp30A;void*_tmp30C;void*_tmp30E;_LL172: _tmp30C=_tmp30B;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp30D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp30C;if(_tmp30D->tag != 19)goto _LL174;else{_tmp30E=(void*)_tmp30D->f1;}};_LL173: {
# 1424
struct Cyc_Absyn_Exp*_tmp30F=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp897;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp896;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp310=(_tmp896=_cycalloc(sizeof(*_tmp896)),((_tmp896[0]=((_tmp897.tag=18,((_tmp897.f1=_tmp30F,_tmp897)))),_tmp896)));
if(Cyc_Tcutil_unify(_tmp30E,(void*)_tmp310))return _tmp30C;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1430
return Cyc_Absyn_uint_typ;}
# 1433
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1440
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1442
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp313=fs;
void*_tmp314=t_orig;
for(0;_tmp313 != 0;_tmp313=_tmp313->tl){
void*_tmp315=(void*)_tmp313->hd;
void*_tmp316=_tmp315;struct _dyneither_ptr*_tmp318;unsigned int _tmp31A;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp317=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp316;if(_tmp317->tag != 0)goto _LL179;else{_tmp318=_tmp317->f1;}}_LL178: {
# 1450
int bad_type=1;
{void*_tmp31B=Cyc_Tcutil_compress(_tmp314);void*_tmp31C=_tmp31B;struct Cyc_Absyn_Aggrdecl**_tmp31E;struct Cyc_List_List*_tmp320;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31D->tag != 11)goto _LL17E;else{if((((_tmp31D->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp31E=(struct Cyc_Absyn_Aggrdecl**)(((_tmp31D->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1453
 if((*_tmp31E)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp318,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp31E)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp89B;void*_tmp89A[1];struct Cyc_String_pa_PrintArg_struct _tmp899;(_tmp899.tag=0,((_tmp899.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp89A[0]=& _tmp899,Cyc_Tcutil_terr(loc,((_tmp89B="no field of struct/union has name %s",_tag_dyneither(_tmp89B,sizeof(char),37))),_tag_dyneither(_tmp89A,sizeof(void*),1)))))));}else{
# 1458
_tmp314=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp31F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp31C;if(_tmp31F->tag != 12)goto _LL180;else{_tmp320=_tmp31F->f2;}}_LL17F: {
# 1462
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp318,_tmp320);
if(!((unsigned int)t2)){
const char*_tmp89F;void*_tmp89E[1];struct Cyc_String_pa_PrintArg_struct _tmp89D;(_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp318),((_tmp89E[0]=& _tmp89D,Cyc_Tcutil_terr(loc,((_tmp89F="no field of struct/union has name %s",_tag_dyneither(_tmp89F,sizeof(char),37))),_tag_dyneither(_tmp89E,sizeof(void*),1)))))));}else{
# 1466
_tmp314=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1471
if(bad_type){
if(_tmp313 == fs){
const char*_tmp8A3;void*_tmp8A2[1];struct Cyc_String_pa_PrintArg_struct _tmp8A1;(_tmp8A1.tag=0,((_tmp8A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8A2[0]=& _tmp8A1,Cyc_Tcutil_terr(loc,((_tmp8A3="%s is not a known struct/union type",_tag_dyneither(_tmp8A3,sizeof(char),36))),_tag_dyneither(_tmp8A2,sizeof(void*),1)))))));}else{
# 1475
const char*_tmp8A7;void*_tmp8A6[1];struct Cyc_String_pa_PrintArg_struct _tmp8A5;struct _dyneither_ptr _tmp32A=(_tmp8A5.tag=0,((_tmp8A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp8A6[0]=& _tmp8A5,Cyc_aprintf(((_tmp8A7="(%s)",_tag_dyneither(_tmp8A7,sizeof(char),5))),_tag_dyneither(_tmp8A6,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp313;x=x->tl){
void*_tmp32B=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp32C=_tmp32B;struct _dyneither_ptr*_tmp32E;unsigned int _tmp330;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp32D=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32D->tag != 0)goto _LL185;else{_tmp32E=_tmp32D->f1;}}_LL184:
# 1480
{const char*_tmp8AC;void*_tmp8AB[2];struct Cyc_String_pa_PrintArg_struct _tmp8AA;struct Cyc_String_pa_PrintArg_struct _tmp8A9;_tmp32A=((_tmp8A9.tag=0,((_tmp8A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp32E),((_tmp8AA.tag=0,((_tmp8AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8AB[0]=& _tmp8AA,((_tmp8AB[1]=& _tmp8A9,Cyc_aprintf(((_tmp8AC="%s.%s",_tag_dyneither(_tmp8AC,sizeof(char),6))),_tag_dyneither(_tmp8AB,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp32F=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp32C;if(_tmp32F->tag != 1)goto _LL182;else{_tmp330=_tmp32F->f1;}}_LL186:
# 1482
{const char*_tmp8B1;void*_tmp8B0[2];struct Cyc_String_pa_PrintArg_struct _tmp8AF;struct Cyc_Int_pa_PrintArg_struct _tmp8AE;_tmp32A=((_tmp8AE.tag=1,((_tmp8AE.f1=(unsigned long)((int)_tmp330),((_tmp8AF.tag=0,((_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8B0[0]=& _tmp8AF,((_tmp8B0[1]=& _tmp8AE,Cyc_aprintf(((_tmp8B1="%s.%d",_tag_dyneither(_tmp8B1,sizeof(char),6))),_tag_dyneither(_tmp8B0,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1484
const char*_tmp8B6;void*_tmp8B5[2];struct Cyc_String_pa_PrintArg_struct _tmp8B4;struct Cyc_String_pa_PrintArg_struct _tmp8B3;(_tmp8B3.tag=0,((_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8B4.tag=0,((_tmp8B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp32A),((_tmp8B5[0]=& _tmp8B4,((_tmp8B5[1]=& _tmp8B3,Cyc_Tcutil_terr(loc,((_tmp8B6="%s == %s is not a struct/union type",_tag_dyneither(_tmp8B6,sizeof(char),36))),_tag_dyneither(_tmp8B5,sizeof(void*),2)))))))))))));};}}
# 1487
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp319=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp316;if(_tmp319->tag != 1)goto _LL176;else{_tmp31A=_tmp319->f1;}}_LL17A: {
# 1489
int bad_type=1;
{void*_tmp340=Cyc_Tcutil_compress(_tmp314);void*_tmp341=_tmp340;struct Cyc_Absyn_Aggrdecl**_tmp343;struct Cyc_List_List*_tmp345;struct Cyc_List_List*_tmp347;struct Cyc_Absyn_Datatypefield*_tmp349;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp341;if(_tmp342->tag != 11)goto _LL18A;else{if((((_tmp342->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp343=(struct Cyc_Absyn_Aggrdecl**)(((_tmp342->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1492
 if((*_tmp343)->impl == 0)
goto _LL187;
_tmp345=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp343)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341;if(_tmp344->tag != 12)goto _LL18C;else{_tmp345=_tmp344->f2;}}_LL18B:
# 1496
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp345)<= _tmp31A){
const char*_tmp8BB;void*_tmp8BA[2];struct Cyc_Int_pa_PrintArg_struct _tmp8B9;struct Cyc_Int_pa_PrintArg_struct _tmp8B8;(_tmp8B8.tag=1,((_tmp8B8.f1=(unsigned long)((int)_tmp31A),((_tmp8B9.tag=1,((_tmp8B9.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp345),((_tmp8BA[0]=& _tmp8B9,((_tmp8BA[1]=& _tmp8B8,Cyc_Tcutil_terr(loc,((_tmp8BB="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp8BB,sizeof(char),46))),_tag_dyneither(_tmp8BA,sizeof(void*),2)))))))))))));}else{
# 1500
_tmp314=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp345,(int)_tmp31A))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp341;if(_tmp346->tag != 10)goto _LL18E;else{_tmp347=_tmp346->f1;}}_LL18D:
# 1504
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347)<= _tmp31A){
const char*_tmp8C0;void*_tmp8BF[2];struct Cyc_Int_pa_PrintArg_struct _tmp8BE;struct Cyc_Int_pa_PrintArg_struct _tmp8BD;(_tmp8BD.tag=1,((_tmp8BD.f1=(unsigned long)((int)_tmp31A),((_tmp8BE.tag=1,((_tmp8BE.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp347),((_tmp8BF[0]=& _tmp8BE,((_tmp8BF[1]=& _tmp8BD,Cyc_Tcutil_terr(loc,((_tmp8C0="tuple has too few components: %d <= %d",_tag_dyneither(_tmp8C0,sizeof(char),39))),_tag_dyneither(_tmp8BF,sizeof(void*),2)))))))))))));}else{
# 1508
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp347,(int)_tmp31A)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp341;if(_tmp348->tag != 4)goto _LL190;else{if((((_tmp348->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp349=((struct _tuple2)(((_tmp348->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1512
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs)< _tmp31A){
const char*_tmp8C5;void*_tmp8C4[2];struct Cyc_Int_pa_PrintArg_struct _tmp8C3;struct Cyc_Int_pa_PrintArg_struct _tmp8C2;(_tmp8C2.tag=1,((_tmp8C2.f1=(unsigned long)((int)_tmp31A),((_tmp8C3.tag=1,((_tmp8C3.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp349->typs),((_tmp8C4[0]=& _tmp8C3,((_tmp8C4[1]=& _tmp8C2,Cyc_Tcutil_terr(loc,((_tmp8C5="datatype field has too few components: %d < %d",_tag_dyneither(_tmp8C5,sizeof(char),47))),_tag_dyneither(_tmp8C4,sizeof(void*),2)))))))))))));}else{
# 1516
if(_tmp31A != 0)
_tmp314=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp349->typs,(int)(_tmp31A - 1))).f2;else{
if(_tmp313->tl != 0){
const char*_tmp8C8;void*_tmp8C7;(_tmp8C7=0,Cyc_Tcutil_terr(loc,((_tmp8C8="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp8C8,sizeof(char),68))),_tag_dyneither(_tmp8C7,sizeof(void*),0)));}}}
# 1521
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1525
if(bad_type){
const char*_tmp8CC;void*_tmp8CB[1];struct Cyc_String_pa_PrintArg_struct _tmp8CA;(_tmp8CA.tag=0,((_tmp8CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp314)),((_tmp8CB[0]=& _tmp8CA,Cyc_Tcutil_terr(loc,((_tmp8CC="%s is not a known type",_tag_dyneither(_tmp8CC,sizeof(char),23))),_tag_dyneither(_tmp8CB,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1530
return Cyc_Absyn_uint_typ;};}
# 1534
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp35B=_new_region("r");struct _RegionHandle*r=& _tmp35B;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp35C=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp35C,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp35D=t;void*_tmp360;void*_tmp361;union Cyc_Absyn_Constraint*_tmp362;union Cyc_Absyn_Constraint*_tmp363;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp35D;if(_tmp35E->tag != 1)goto _LL195;}_LL194: {
# 1541
struct Cyc_List_List*_tmp364=Cyc_Tcenv_lookup_type_vars(_tmp35C);
struct Cyc_Core_Opt*_tmp8CD;void*_tmp365=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp8CD=_cycalloc(sizeof(*_tmp8CD)),((_tmp8CD->v=_tmp364,_tmp8CD)))));
struct Cyc_Core_Opt*_tmp8CE;void*_tmp366=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE->v=_tmp364,_tmp8CE)))));
union Cyc_Absyn_Constraint*_tmp367=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp368=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp8CF;struct Cyc_Absyn_PtrAtts _tmp369=(_tmp8CF.rgn=_tmp366,((_tmp8CF.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp8CF.bounds=_tmp367,((_tmp8CF.zero_term=_tmp368,((_tmp8CF.ptrloc=0,_tmp8CF)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8D5;struct Cyc_Absyn_PtrInfo _tmp8D4;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8D3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp36A=(_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3[0]=((_tmp8D5.tag=5,((_tmp8D5.f1=((_tmp8D4.elt_typ=_tmp365,((_tmp8D4.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp8D4.ptr_atts=_tmp369,_tmp8D4)))))),_tmp8D5)))),_tmp8D3)));
Cyc_Tcutil_unify(t,(void*)_tmp36A);
_tmp360=_tmp365;_tmp361=_tmp366;_tmp362=_tmp367;_tmp363=_tmp368;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35D;if(_tmp35F->tag != 5)goto _LL197;else{_tmp360=(_tmp35F->f1).elt_typ;_tmp361=((_tmp35F->f1).ptr_atts).rgn;_tmp362=((_tmp35F->f1).ptr_atts).bounds;_tmp363=((_tmp35F->f1).ptr_atts).zero_term;}}_LL196:
# 1551
 Cyc_Tcenv_check_rgn_accessible(_tmp35C,loc,_tmp361);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp362);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp360),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp35C)){
const char*_tmp8D8;void*_tmp8D7;(_tmp8D7=0,Cyc_Tcutil_terr(loc,((_tmp8D8="can't dereference abstract pointer type",_tag_dyneither(_tmp8D8,sizeof(char),40))),_tag_dyneither(_tmp8D7,sizeof(void*),0)));}{
void*_tmp373=_tmp360;_npop_handler(0);return _tmp373;};_LL197:;_LL198: {
# 1557
const char*_tmp8DC;void*_tmp8DB[1];struct Cyc_String_pa_PrintArg_struct _tmp8DA;void*_tmp377=(_tmp8DA.tag=0,((_tmp8DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8DB[0]=& _tmp8DA,Cyc_Tcexp_expr_err(_tmp35C,loc,topt,((_tmp8DC="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8DC,sizeof(char),39))),_tag_dyneither(_tmp8DB,sizeof(void*),1)))))));_npop_handler(0);return _tmp377;}_LL192:;};}
# 1536
;_pop_region(r);}
# 1563
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1567
struct _RegionHandle _tmp378=_new_region("r");struct _RegionHandle*r=& _tmp378;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1571
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp379=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp37A=_tmp379;struct Cyc_Absyn_Aggrdecl*_tmp37C;struct Cyc_List_List*_tmp37D;enum Cyc_Absyn_AggrKind _tmp37F;struct Cyc_List_List*_tmp380;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37A;if(_tmp37B->tag != 11)goto _LL19C;else{if((((_tmp37B->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp37C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp37B->f1).aggr_info).KnownAggr).val);_tmp37D=(_tmp37B->f1).targs;}}_LL19B: {
# 1574
struct Cyc_Absyn_Aggrfield*_tmp381=Cyc_Absyn_lookup_decl_field(_tmp37C,f);
if(_tmp381 == 0){
const char*_tmp8E1;void*_tmp8E0[2];struct Cyc_String_pa_PrintArg_struct _tmp8DF;struct Cyc_String_pa_PrintArg_struct _tmp8DE;void*_tmp386=(_tmp8DE.tag=0,((_tmp8DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp37C->name)),((_tmp8E0[0]=& _tmp8DF,((_tmp8E0[1]=& _tmp8DE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E1="%s has no %s member",_tag_dyneither(_tmp8E1,sizeof(char),20))),_tag_dyneither(_tmp8E0,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp386;}
# 1579
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->tagged)*is_tagged=1;{
void*t2=_tmp381->type;
if(_tmp37D != 0){
struct _RegionHandle _tmp387=_new_region("rgn");struct _RegionHandle*rgn=& _tmp387;_push_region(rgn);
{struct Cyc_List_List*_tmp388=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp37C->tvs,_tmp37D);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp388,_tmp381->type);}
# 1583
;_pop_region(rgn);}
# 1589
if((((_tmp37C->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp381->requires_clause == 0){
const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;void*_tmp38C=(_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E4[0]=& _tmp8E3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E5="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E5,sizeof(char),56))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));_npop_handler(0);return _tmp38C;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp37C->impl))->exist_vars != 0){
# 1594
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_String_pa_PrintArg_struct _tmp8E7;void*_tmp390=(_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E8[0]=& _tmp8E7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E9="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8E9,sizeof(char),81))),_tag_dyneither(_tmp8E8,sizeof(void*),1)))))));_npop_handler(0);return _tmp390;}}{
# 1597
void*_tmp391=t2;_npop_handler(0);return _tmp391;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp37A;if(_tmp37E->tag != 12)goto _LL19E;else{_tmp37F=_tmp37E->f1;_tmp380=_tmp37E->f2;}}_LL19D: {
# 1599
struct Cyc_Absyn_Aggrfield*_tmp392=Cyc_Absyn_lookup_field(_tmp380,f);
if(_tmp392 == 0){
const char*_tmp8ED;void*_tmp8EC[1];struct Cyc_String_pa_PrintArg_struct _tmp8EB;void*_tmp396=(_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8EC[0]=& _tmp8EB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8ED="type has no %s member",_tag_dyneither(_tmp8ED,sizeof(char),22))),_tag_dyneither(_tmp8EC,sizeof(void*),1)))))));_npop_handler(0);return _tmp396;}
# 1604
if(((_tmp37F == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp392->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp392->requires_clause == 0){
# 1606
const char*_tmp8F1;void*_tmp8F0[1];struct Cyc_String_pa_PrintArg_struct _tmp8EF;void*_tmp39A=(_tmp8EF.tag=0,((_tmp8EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F0[0]=& _tmp8EF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F1="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8F1,sizeof(char),56))),_tag_dyneither(_tmp8F0,sizeof(void*),1)))))));_npop_handler(0);return _tmp39A;}{
void*_tmp39B=_tmp392->type;_npop_handler(0);return _tmp39B;};}_LL19E:;_LL19F: {
# 1609
const char*_tmp8F5;void*_tmp8F4[1];struct Cyc_String_pa_PrintArg_struct _tmp8F3;void*_tmp39F=(_tmp8F3.tag=0,((_tmp8F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8F4[0]=& _tmp8F3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F5="expecting struct or union, found %s",_tag_dyneither(_tmp8F5,sizeof(char),36))),_tag_dyneither(_tmp8F4,sizeof(void*),1)))))));_npop_handler(0);return _tmp39F;}_LL199:;};
# 1567
;_pop_region(r);}
# 1615
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1618
struct _RegionHandle _tmp3A0=_new_region("r");struct _RegionHandle*r=& _tmp3A0;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1623
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp3A1=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3A2=_tmp3A1;void*_tmp3A4;void*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A6;union Cyc_Absyn_Constraint*_tmp3A7;_LL1A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A2;if(_tmp3A3->tag != 5)goto _LL1A3;else{_tmp3A4=(_tmp3A3->f1).elt_typ;_tmp3A5=((_tmp3A3->f1).ptr_atts).rgn;_tmp3A6=((_tmp3A3->f1).ptr_atts).bounds;_tmp3A7=((_tmp3A3->f1).ptr_atts).zero_term;}}_LL1A2:
# 1626
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp3A6);
{void*_tmp3A8=Cyc_Tcutil_compress(_tmp3A4);void*_tmp3A9=_tmp3A8;struct Cyc_Absyn_Aggrdecl*_tmp3AB;struct Cyc_List_List*_tmp3AC;enum Cyc_Absyn_AggrKind _tmp3AE;struct Cyc_List_List*_tmp3AF;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3AA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AA->tag != 11)goto _LL1A8;else{if((((_tmp3AA->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A8;_tmp3AB=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp3AA->f1).aggr_info).KnownAggr).val);_tmp3AC=(_tmp3AA->f1).targs;}}_LL1A7: {
# 1629
struct Cyc_Absyn_Aggrfield*_tmp3B0=Cyc_Absyn_lookup_decl_field(_tmp3AB,f);
if(_tmp3B0 == 0){
const char*_tmp8FA;void*_tmp8F9[2];struct Cyc_String_pa_PrintArg_struct _tmp8F8;struct Cyc_String_pa_PrintArg_struct _tmp8F7;void*_tmp3B5=(_tmp8F7.tag=0,((_tmp8F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3AB->name)),((_tmp8F9[0]=& _tmp8F8,((_tmp8F9[1]=& _tmp8F7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FA="type %s has no %s member",_tag_dyneither(_tmp8FA,sizeof(char),25))),_tag_dyneither(_tmp8F9,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3B5;}
# 1634
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->tagged)*is_tagged=1;{
void*t3=_tmp3B0->type;
if(_tmp3AC != 0){
struct _RegionHandle _tmp3B6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3B6;_push_region(rgn);
{struct Cyc_List_List*_tmp3B7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp3AB->tvs,_tmp3AC);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3B7,_tmp3B0->type);}
# 1638
;_pop_region(rgn);}{
# 1642
struct Cyc_Absyn_Kind*_tmp3B8=Cyc_Tcutil_typ_kind(t3);
# 1645
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp3B8) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp3B9=Cyc_Tcutil_compress(t3);void*_tmp3BA=_tmp3B9;_LL1AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3BB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BA;if(_tmp3BB->tag != 8)goto _LL1AF;}_LL1AE:
 goto _LL1AC;_LL1AF:;_LL1B0: {
# 1649
const char*_tmp8FE;void*_tmp8FD[1];struct Cyc_String_pa_PrintArg_struct _tmp8FC;void*_tmp3BF=(_tmp8FC.tag=0,((_tmp8FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8FD[0]=& _tmp8FC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FE="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8FE,sizeof(char),48))),_tag_dyneither(_tmp8FD,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BF;}_LL1AC:;}
# 1654
if((((_tmp3AB->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp3B0->requires_clause == 0){
# 1657
const char*_tmp902;void*_tmp901[1];struct Cyc_String_pa_PrintArg_struct _tmp900;void*_tmp3C3=(_tmp900.tag=0,((_tmp900.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp901[0]=& _tmp900,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp902="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp902,sizeof(char),56))),_tag_dyneither(_tmp901,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C3;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3AB->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp906;void*_tmp905[1];struct Cyc_String_pa_PrintArg_struct _tmp904;void*_tmp3C7=(_tmp904.tag=0,((_tmp904.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp905[0]=& _tmp904,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp906="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp906,sizeof(char),72))),_tag_dyneither(_tmp905,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C7;}}{
# 1664
void*_tmp3C8=t3;_npop_handler(0);return _tmp3C8;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A9;if(_tmp3AD->tag != 12)goto _LL1AA;else{_tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;}}_LL1A9: {
# 1666
struct Cyc_Absyn_Aggrfield*_tmp3C9=Cyc_Absyn_lookup_field(_tmp3AF,f);
if(_tmp3C9 == 0){
const char*_tmp90A;void*_tmp909[1];struct Cyc_String_pa_PrintArg_struct _tmp908;void*_tmp3CD=(_tmp908.tag=0,((_tmp908.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp909[0]=& _tmp908,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90A="type has no %s field",_tag_dyneither(_tmp90A,sizeof(char),21))),_tag_dyneither(_tmp909,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CD;}
# 1671
if((_tmp3AE == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp3C9->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp90E;void*_tmp90D[1];struct Cyc_String_pa_PrintArg_struct _tmp90C;void*_tmp3D1=(_tmp90C.tag=0,((_tmp90C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp90D[0]=& _tmp90C,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp90E="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp90E,sizeof(char),56))),_tag_dyneither(_tmp90D,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D1;}{
void*_tmp3D2=_tmp3C9->type;_npop_handler(0);return _tmp3D2;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1676
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1679
const char*_tmp912;void*_tmp911[1];struct Cyc_String_pa_PrintArg_struct _tmp910;void*_tmp3D6=(_tmp910.tag=0,((_tmp910.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp911[0]=& _tmp910,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp912="expecting struct or union pointer, found %s",_tag_dyneither(_tmp912,sizeof(char),44))),_tag_dyneither(_tmp911,sizeof(void*),1)))))));_npop_handler(0);return _tmp3D6;};
# 1618
;_pop_region(r);}
# 1684
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1686
struct _tuple15 _tmp3D7=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp3D9;int _tmp3DA;struct _tuple15 _tmp3D8=_tmp3D7;_tmp3D9=_tmp3D8.f1;_tmp3DA=_tmp3D8.f2;
if(!_tmp3DA){
const char*_tmp915;void*_tmp914;return(_tmp914=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp915="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp915,sizeof(char),47))),_tag_dyneither(_tmp914,sizeof(void*),0)));}{
# 1690
struct _handler_cons _tmp3DD;_push_handler(& _tmp3DD);{int _tmp3DF=0;if(setjmp(_tmp3DD.handler))_tmp3DF=1;if(!_tmp3DF){
{void*_tmp3E0=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3D9)).f2;_npop_handler(0);return _tmp3E0;};_pop_handler();}else{void*_tmp3DE=(void*)_exn_thrown;void*_tmp3E2=_tmp3DE;void*_tmp3E4;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3E3=(struct Cyc_List_Nth_exn_struct*)_tmp3E2;if(_tmp3E3->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1693
const char*_tmp91A;void*_tmp919[2];struct Cyc_Int_pa_PrintArg_struct _tmp918;struct Cyc_Int_pa_PrintArg_struct _tmp917;return(_tmp917.tag=1,((_tmp917.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp918.tag=1,((_tmp918.f1=(unsigned long)((int)_tmp3D9),((_tmp919[0]=& _tmp918,((_tmp919[1]=& _tmp917,Cyc_Tcexp_expr_err(te,loc,0,((_tmp91A="index is %d but tuple has only %d fields",_tag_dyneither(_tmp91A,sizeof(char),41))),_tag_dyneither(_tmp919,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3E4=_tmp3E2;_LL1B5:(void)_rethrow(_tmp3E4);_LL1B1:;}};};}
# 1698
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1700
struct _RegionHandle _tmp3E9=_new_region("r");struct _RegionHandle*r=& _tmp3E9;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3EA=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3EA),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3EA),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3EA,e2)){
const char*_tmp91E;void*_tmp91D[1];struct Cyc_String_pa_PrintArg_struct _tmp91C;void*_tmp3EE=(_tmp91C.tag=0,((_tmp91C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp91D[0]=& _tmp91C,Cyc_Tcexp_expr_err(_tmp3EA,e2->loc,topt,((_tmp91E="expecting int subscript, found %s",_tag_dyneither(_tmp91E,sizeof(char),34))),_tag_dyneither(_tmp91D,sizeof(void*),1)))))));_npop_handler(0);return _tmp3EE;}{
# 1711
void*_tmp3EF=t1;void*_tmp3F1;struct Cyc_Absyn_Tqual _tmp3F2;void*_tmp3F3;union Cyc_Absyn_Constraint*_tmp3F4;union Cyc_Absyn_Constraint*_tmp3F5;struct Cyc_List_List*_tmp3F7;_LL1B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F0->tag != 5)goto _LL1B9;else{_tmp3F1=(_tmp3F0->f1).elt_typ;_tmp3F2=(_tmp3F0->f1).elt_tq;_tmp3F3=((_tmp3F0->f1).ptr_atts).rgn;_tmp3F4=((_tmp3F0->f1).ptr_atts).bounds;_tmp3F5=((_tmp3F0->f1).ptr_atts).zero_term;}}_LL1B8:
# 1715
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F5)){
int emit_warning=1;
{void*_tmp3F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3F4);void*_tmp3F9=_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FB;_LL1BE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3FA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F9;if(_tmp3FA->tag != 1)goto _LL1C0;else{_tmp3FB=_tmp3FA->f1;}}_LL1BF:
# 1719
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp3FD=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3FF;int _tmp400;struct _tuple15 _tmp3FE=_tmp3FD;_tmp3FF=_tmp3FE.f1;_tmp400=_tmp3FE.f2;
if(_tmp400){
struct _tuple15 _tmp401=Cyc_Evexp_eval_const_uint_exp(_tmp3FB);unsigned int _tmp403;int _tmp404;struct _tuple15 _tmp402=_tmp401;_tmp403=_tmp402.f1;_tmp404=_tmp402.f2;
if(_tmp404  && _tmp403 > _tmp3FF)emit_warning=0;}}
# 1726
goto _LL1BD;_LL1C0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3FC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F9;if(_tmp3FC->tag != 0)goto _LL1BD;}_LL1C1:
 emit_warning=0;goto _LL1BD;_LL1BD:;}
# 1729
if(emit_warning){
const char*_tmp921;void*_tmp920;(_tmp920=0,Cyc_Tcutil_warn(e2->loc,((_tmp921="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp921,sizeof(char),63))),_tag_dyneither(_tmp920,sizeof(void*),0)));}}else{
# 1733
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp407=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp409;int _tmp40A;struct _tuple15 _tmp408=_tmp407;_tmp409=_tmp408.f1;_tmp40A=_tmp408.f2;
if(_tmp40A)
Cyc_Tcutil_check_bound(loc,_tmp409,_tmp3F4);}else{
# 1739
if(Cyc_Tcutil_is_bound_one(_tmp3F4) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3F5)){
const char*_tmp924;void*_tmp923;(_tmp923=0,Cyc_Tcutil_warn(e1->loc,((_tmp924="subscript applied to pointer to one object",_tag_dyneither(_tmp924,sizeof(char),43))),_tag_dyneither(_tmp923,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3F4);}}
# 1744
Cyc_Tcenv_check_rgn_accessible(_tmp3EA,loc,_tmp3F3);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3F1),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3EA)){
const char*_tmp927;void*_tmp926;(_tmp926=0,Cyc_Tcutil_terr(e1->loc,((_tmp927="can't subscript an abstract pointer",_tag_dyneither(_tmp927,sizeof(char),36))),_tag_dyneither(_tmp926,sizeof(void*),0)));}{
void*_tmp40F=_tmp3F1;_npop_handler(0);return _tmp40F;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3F6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3EF;if(_tmp3F6->tag != 10)goto _LL1BB;else{_tmp3F7=_tmp3F6->f1;}}_LL1BA: {
void*_tmp410=Cyc_Tcexp_ithTupleType(_tmp3EA,loc,_tmp3F7,e2);_npop_handler(0);return _tmp410;}_LL1BB:;_LL1BC: {
const char*_tmp92B;void*_tmp92A[1];struct Cyc_String_pa_PrintArg_struct _tmp929;void*_tmp414=(_tmp929.tag=0,((_tmp929.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp92A[0]=& _tmp929,Cyc_Tcexp_expr_err(_tmp3EA,loc,topt,((_tmp92B="subscript applied to %s",_tag_dyneither(_tmp92B,sizeof(char),24))),_tag_dyneither(_tmp92A,sizeof(void*),1)))))));_npop_handler(0);return _tmp414;}_LL1B6:;};};}
# 1701
;_pop_region(r);}
# 1755
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp415=Cyc_Tcutil_compress(*topt);void*_tmp416=_tmp415;struct Cyc_List_List*_tmp418;_LL1C3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp417=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp416;if(_tmp417->tag != 10)goto _LL1C5;else{_tmp418=_tmp417->f1;}}_LL1C4:
# 1761
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp418)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1764
goto _LL1C2;
# 1766
for(0;es != 0;(es=es->tl,_tmp418=_tmp418->tl)){
struct _RegionHandle _tmp419=_new_region("r");struct _RegionHandle*r=& _tmp419;_push_region(r);
{int bogus=0;
void*_tmp41A=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp418))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp41A,(struct Cyc_Absyn_Exp*)es->hd);
# 1772
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp418->hd)).f2,& bogus);{
struct _tuple18*_tmp92E;struct Cyc_List_List*_tmp92D;fields=((_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D->hd=((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E->f1=(*((struct _tuple18*)_tmp418->hd)).f1,((_tmp92E->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp92E)))))),((_tmp92D->tl=fields,_tmp92D))))));};}
# 1768
;_pop_region(r);}
# 1776
done=1;
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}
# 1780
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp41D=_new_region("r");struct _RegionHandle*r=& _tmp41D;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp931;struct Cyc_List_List*_tmp930;fields=((_tmp930=_cycalloc(sizeof(*_tmp930)),((_tmp930->hd=((_tmp931=_cycalloc(sizeof(*_tmp931)),((_tmp931->f1=Cyc_Absyn_empty_tqual(0),((_tmp931->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp931)))))),((_tmp930->tl=fields,_tmp930))))));};
# 1783
;_pop_region(r);}{
# 1787
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp934;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp933;return(void*)((_tmp933=_cycalloc(sizeof(*_tmp933)),((_tmp933[0]=((_tmp934.tag=10,((_tmp934.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp934)))),_tmp933))));};}
# 1791
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1794
const char*_tmp937;void*_tmp936;return(_tmp936=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp937="tcCompoundLit",_tag_dyneither(_tmp937,sizeof(char),14))),_tag_dyneither(_tmp936,sizeof(void*),0)));}
# 1804 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1807
void*res_t2;
struct _RegionHandle _tmp424=_new_region("r");struct _RegionHandle*r=& _tmp424;_push_region(r);{
int _tmp425=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp93A;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp939;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp426=(_tmp939=_cycalloc(sizeof(*_tmp939)),((_tmp939[0]=((_tmp93A.tag=0,((_tmp93A.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp425),_tmp93A)))),_tmp939)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp426,loc);
# 1816
if(zero_term){
struct Cyc_Absyn_Exp*_tmp427=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp425 - 1);
if(!Cyc_Tcutil_is_zero(_tmp427)){
const char*_tmp93D;void*_tmp93C;(_tmp93C=0,Cyc_Tcutil_terr(_tmp427->loc,((_tmp93D="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp93D,sizeof(char),45))),_tag_dyneither(_tmp93C,sizeof(void*),0)));}}
# 1821
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1826
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1829
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1831
const char*_tmp941;void*_tmp940[1];struct Cyc_String_pa_PrintArg_struct _tmp93F;(_tmp93F.tag=0,((_tmp93F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp940[0]=& _tmp93F,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp941="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp941,sizeof(char),53))),_tag_dyneither(_tmp940,sizeof(void*),1)))))));}}
# 1835
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1840
void*_tmp42F=(void*)ds->hd;void*_tmp430=_tmp42F;struct Cyc_Absyn_Exp*_tmp433;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp431=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp430;if(_tmp431->tag != 1)goto _LL1CA;}_LL1C9:
# 1842
{const char*_tmp944;void*_tmp943;(_tmp943=0,Cyc_Tcutil_terr(loc,((_tmp944="only array index designators are supported",_tag_dyneither(_tmp944,sizeof(char),43))),_tag_dyneither(_tmp943,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp432=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp430;if(_tmp432->tag != 0)goto _LL1C7;else{_tmp433=_tmp432->f1;}}_LL1CB:
# 1845
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp433);{
struct _tuple15 _tmp436=Cyc_Evexp_eval_const_uint_exp(_tmp433);unsigned int _tmp438;int _tmp439;struct _tuple15 _tmp437=_tmp436;_tmp438=_tmp437.f1;_tmp439=_tmp437.f2;
if(!_tmp439){
const char*_tmp947;void*_tmp946;(_tmp946=0,Cyc_Tcutil_terr(_tmp433->loc,((_tmp947="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp947,sizeof(char),47))),_tag_dyneither(_tmp946,sizeof(void*),0)));}else{
if(_tmp438 != offset){
const char*_tmp94C;void*_tmp94B[2];struct Cyc_Int_pa_PrintArg_struct _tmp94A;struct Cyc_Int_pa_PrintArg_struct _tmp949;(_tmp949.tag=1,((_tmp949.f1=(unsigned long)((int)_tmp438),((_tmp94A.tag=1,((_tmp94A.f1=(unsigned long)offset,((_tmp94B[0]=& _tmp94A,((_tmp94B[1]=& _tmp949,Cyc_Tcutil_terr(_tmp433->loc,((_tmp94C="expecting index designator %d but found %d",_tag_dyneither(_tmp94C,sizeof(char),43))),_tag_dyneither(_tmp94B,sizeof(void*),2)))))))))))));}}
# 1852
goto _LL1C7;};_LL1C7:;}}}{
# 1856
void*_tmp440=res_t2;_npop_handler(0);return _tmp440;};
# 1808
;_pop_region(r);}
# 1860
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1863
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp441=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp442=_tmp441;void*_tmp444;_LL1CD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp442;if(_tmp443->tag != 19)goto _LL1CF;else{_tmp444=(void*)_tmp443->f1;}}_LL1CE:
# 1868
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp445=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp444,0),0,Cyc_Absyn_No_coercion,0);
_tmp445->topt=bound->topt;
# 1873
bound=_tmp445;}
# 1875
goto _LL1CC;_LL1CF:;_LL1D0:
# 1877
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp950;void*_tmp94F[1];struct Cyc_String_pa_PrintArg_struct _tmp94E;(_tmp94E.tag=0,((_tmp94E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp94F[0]=& _tmp94E,Cyc_Tcutil_terr(bound->loc,((_tmp950="expecting unsigned int, found %s",_tag_dyneither(_tmp950,sizeof(char),33))),_tag_dyneither(_tmp94F,sizeof(void*),1)))))));}_LL1CC:;}
# 1882
if(!(vd->tq).real_const){
const char*_tmp953;void*_tmp952;(_tmp952=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp953="comprehension index variable is not declared const!",_tag_dyneither(_tmp953,sizeof(char),52))),_tag_dyneither(_tmp952,sizeof(void*),0)));}{
# 1885
struct _RegionHandle _tmp44B=_new_region("r");struct _RegionHandle*r=& _tmp44B;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1891
struct Cyc_Tcenv_Tenv*_tmp44C=te2;
void**_tmp44D=0;
struct Cyc_Absyn_Tqual*_tmp44E=0;
union Cyc_Absyn_Constraint**_tmp44F=0;
# 1896
if(topt != 0){
void*_tmp450=Cyc_Tcutil_compress(*topt);void*_tmp451=_tmp450;struct Cyc_Absyn_PtrInfo _tmp453;void*_tmp455;struct Cyc_Absyn_Tqual _tmp456;struct Cyc_Absyn_Exp*_tmp457;union Cyc_Absyn_Constraint*_tmp458;_LL1D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp451;if(_tmp452->tag != 5)goto _LL1D4;else{_tmp453=_tmp452->f1;}}_LL1D3:
# 1899
{void**_tmp954;_tmp44D=((_tmp954=_region_malloc(r,sizeof(*_tmp954)),((_tmp954[0]=_tmp453.elt_typ,_tmp954))));}
{struct Cyc_Absyn_Tqual*_tmp955;_tmp44E=((_tmp955=_region_malloc(r,sizeof(*_tmp955)),((_tmp955[0]=_tmp453.elt_tq,_tmp955))));}
{union Cyc_Absyn_Constraint**_tmp956;_tmp44F=((_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956[0]=(_tmp453.ptr_atts).zero_term,_tmp956))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp451;if(_tmp454->tag != 8)goto _LL1D6;else{_tmp455=(_tmp454->f1).elt_type;_tmp456=(_tmp454->f1).tq;_tmp457=(_tmp454->f1).num_elts;_tmp458=(_tmp454->f1).zero_term;}}_LL1D5:
# 1904
{void**_tmp957;_tmp44D=((_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957[0]=_tmp455,_tmp957))));}
{struct Cyc_Absyn_Tqual*_tmp958;_tmp44E=((_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=_tmp456,_tmp958))));}
{union Cyc_Absyn_Constraint**_tmp959;_tmp44F=((_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959[0]=_tmp458,_tmp959))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1909
 goto _LL1D1;_LL1D1:;}{
# 1912
void*t=Cyc_Tcexp_tcExp(_tmp44C,_tmp44D,body);
# 1914
struct _RegionHandle _tmp45F=_new_region("temp");struct _RegionHandle*temp=& _tmp45F;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp95C;void*_tmp95B;(_tmp95B=0,Cyc_Tcutil_terr(body->loc,((_tmp95C="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp95C,sizeof(char),49))),_tag_dyneither(_tmp95B,sizeof(void*),0)));}
# 1918
if(_tmp44C->le == 0){
# 1920
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp95F;void*_tmp95E;(_tmp95E=0,Cyc_Tcutil_terr(bound->loc,((_tmp95F="bound is not constant",_tag_dyneither(_tmp95F,sizeof(char),22))),_tag_dyneither(_tmp95E,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp962;void*_tmp961;(_tmp961=0,Cyc_Tcutil_terr(bound->loc,((_tmp962="body is not constant",_tag_dyneither(_tmp962,sizeof(char),21))),_tag_dyneither(_tmp961,sizeof(void*),0)));}}
# 1925
if(_tmp44F != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp44F)){
# 1928
struct Cyc_Absyn_Exp*_tmp466=Cyc_Absyn_uint_exp(1,0);_tmp466->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp466,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1932
struct _RegionHandle _tmp467=_new_region("r");struct _RegionHandle*r=& _tmp467;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp965;void*_tmp964;(_tmp964=0,Cyc_Tcutil_terr(body->loc,((_tmp965="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp965,sizeof(char),49))),_tag_dyneither(_tmp964,sizeof(void*),0)));}{
# 1938
void*_tmp46A=Cyc_Absyn_array_typ(t,_tmp44E == 0?Cyc_Absyn_empty_tqual(0):*_tmp44E,bound,
_tmp44F == 0?Cyc_Absyn_false_conref:*_tmp44F,0);
void*_tmp46B=_tmp46A;_npop_handler(2);return _tmp46B;};
# 1932
;_pop_region(r);};
# 1914
;_pop_region(temp);};};}
# 1886
;_pop_region(r);};}
# 1945
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1948
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp46C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp46D=_tmp46C;void*_tmp46F;_LL1D9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp46D;if(_tmp46E->tag != 19)goto _LL1DB;else{_tmp46F=(void*)_tmp46E->f1;}}_LL1DA:
# 1953
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp470=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp46F,0),0,Cyc_Absyn_No_coercion,0);
_tmp470->topt=bound->topt;
# 1958
bound=_tmp470;}
# 1960
goto _LL1D8;_LL1DB:;_LL1DC:
# 1962
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp969;void*_tmp968[1];struct Cyc_String_pa_PrintArg_struct _tmp967;(_tmp967.tag=0,((_tmp967.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp968[0]=& _tmp967,Cyc_Tcutil_terr(bound->loc,((_tmp969="expecting unsigned int, found %s",_tag_dyneither(_tmp969,sizeof(char),33))),_tag_dyneither(_tmp968,sizeof(void*),1)))))));}_LL1D8:;}{
# 1967
struct _RegionHandle _tmp474=_new_region("r");struct _RegionHandle*r=& _tmp474;_push_region(r);
{void**_tmp475=0;
struct Cyc_Absyn_Tqual*_tmp476=0;
union Cyc_Absyn_Constraint**_tmp477=0;
# 1972
if(topt != 0){
void*_tmp478=Cyc_Tcutil_compress(*topt);void*_tmp479=_tmp478;struct Cyc_Absyn_PtrInfo _tmp47B;void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47F;union Cyc_Absyn_Constraint*_tmp480;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479;if(_tmp47A->tag != 5)goto _LL1E0;else{_tmp47B=_tmp47A->f1;}}_LL1DF:
# 1975
{void**_tmp96A;_tmp475=((_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A[0]=_tmp47B.elt_typ,_tmp96A))));}
{struct Cyc_Absyn_Tqual*_tmp96B;_tmp476=((_tmp96B=_region_malloc(r,sizeof(*_tmp96B)),((_tmp96B[0]=_tmp47B.elt_tq,_tmp96B))));}
{union Cyc_Absyn_Constraint**_tmp96C;_tmp477=((_tmp96C=_region_malloc(r,sizeof(*_tmp96C)),((_tmp96C[0]=(_tmp47B.ptr_atts).zero_term,_tmp96C))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp479;if(_tmp47C->tag != 8)goto _LL1E2;else{_tmp47D=(_tmp47C->f1).elt_type;_tmp47E=(_tmp47C->f1).tq;_tmp47F=(_tmp47C->f1).num_elts;_tmp480=(_tmp47C->f1).zero_term;}}_LL1E1:
# 1980
{void**_tmp96D;_tmp475=((_tmp96D=_region_malloc(r,sizeof(*_tmp96D)),((_tmp96D[0]=_tmp47D,_tmp96D))));}
{struct Cyc_Absyn_Tqual*_tmp96E;_tmp476=((_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E[0]=_tmp47E,_tmp96E))));}
{union Cyc_Absyn_Constraint**_tmp96F;_tmp477=((_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((_tmp96F[0]=_tmp480,_tmp96F))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1985
 goto _LL1DD;_LL1DD:;}
# 1988
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp475 != 0)Cyc_Tcutil_unify(*_tmp475,t);
# 1991
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp972;void*_tmp971;(_tmp971=0,Cyc_Tcutil_terr(bound->loc,((_tmp972="bound is not constant",_tag_dyneither(_tmp972,sizeof(char),22))),_tag_dyneither(_tmp971,sizeof(void*),0)));}}
# 1995
if(_tmp477 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp477)){
# 1998
struct Cyc_Absyn_Exp*_tmp489=Cyc_Absyn_uint_exp(1,0);_tmp489->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp489,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2002
const char*_tmp975;void*_tmp974;(_tmp974=0,Cyc_Tcutil_terr(loc,((_tmp975="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp975,sizeof(char),74))),_tag_dyneither(_tmp974,sizeof(void*),0)));};}{
# 2005
void*_tmp48C=Cyc_Absyn_array_typ(t,_tmp476 == 0?Cyc_Absyn_empty_tqual(0):*_tmp476,bound,
_tmp477 == 0?Cyc_Absyn_false_conref:*_tmp477,0);
void*_tmp48D=_tmp48C;_npop_handler(0);return _tmp48D;};}
# 1968
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2021 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2025
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp976;adptr=((_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976[0]=ad,_tmp976))));};}else{
# 2031
{struct _handler_cons _tmp48F;_push_handler(& _tmp48F);{int _tmp491=0;if(setjmp(_tmp48F.handler))_tmp491=1;if(!_tmp491){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2031
;_pop_handler();}else{void*_tmp490=(void*)_exn_thrown;void*_tmp493=_tmp490;void*_tmp495;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp494=(struct Cyc_Dict_Absent_exn_struct*)_tmp493;if(_tmp494->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2034
{const char*_tmp97A;void*_tmp979[1];struct Cyc_String_pa_PrintArg_struct _tmp978;(_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp979[0]=& _tmp978,Cyc_Tcutil_terr(loc,((_tmp97A="unbound struct/union name %s",_tag_dyneither(_tmp97A,sizeof(char),29))),_tag_dyneither(_tmp979,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp495=_tmp493;_LL1E8:(void)_rethrow(_tmp495);_LL1E4:;}};}
# 2037
*ad_opt=ad;
*tn=ad->name;}
# 2040
if(ad->impl == 0){
{const char*_tmp982;void*_tmp981[1];const char*_tmp980;const char*_tmp97F;struct Cyc_String_pa_PrintArg_struct _tmp97E;(_tmp97E.tag=0,((_tmp97E.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp97F="struct",_tag_dyneither(_tmp97F,sizeof(char),7)):((_tmp980="union",_tag_dyneither(_tmp980,sizeof(char),6)))),((_tmp981[0]=& _tmp97E,Cyc_Tcutil_terr(loc,((_tmp982="can't construct abstract %s",_tag_dyneither(_tmp982,sizeof(char),28))),_tag_dyneither(_tmp981,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2044
struct _RegionHandle _tmp49E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49E;_push_region(rgn);
# 2049
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp49F=Cyc_Tcenv_new_status(te);
if(_tmp49F == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2058
te2=Cyc_Tcenv_set_new_status(rgn,_tmp49F,te);}{
# 2060
struct _tuple12 _tmp983;struct _tuple12 _tmp4A0=(_tmp983.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp983.f2=rgn,_tmp983)));
struct Cyc_List_List*_tmp4A1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4A0,ad->tvs);
struct Cyc_List_List*_tmp4A2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4A0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp4A3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4A1);
struct Cyc_List_List*_tmp4A4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4A2);
struct Cyc_List_List*_tmp4A5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp4A1,_tmp4A2);
void*res_typ;
# 2071
if(topt != 0){
void*_tmp4A6=Cyc_Tcutil_compress(*topt);void*_tmp4A7=_tmp4A6;struct Cyc_Absyn_Aggrdecl**_tmp4A9;struct Cyc_List_List*_tmp4AA;_LL1EA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A7;if(_tmp4A8->tag != 11)goto _LL1EC;else{if((((_tmp4A8->f1).aggr_info).KnownAggr).tag != 2)goto _LL1EC;_tmp4A9=(struct Cyc_Absyn_Aggrdecl**)(((_tmp4A8->f1).aggr_info).KnownAggr).val;_tmp4AA=(_tmp4A8->f1).targs;}}_LL1EB:
# 2074
 if(*_tmp4A9 == *adptr){
{struct Cyc_List_List*_tmp4AB=_tmp4A3;for(0;_tmp4AB != 0  && _tmp4AA != 0;
(_tmp4AB=_tmp4AB->tl,_tmp4AA=_tmp4AA->tl)){
Cyc_Tcutil_unify((void*)_tmp4AB->hd,(void*)_tmp4AA->hd);}}
# 2079
res_typ=*topt;
goto _LL1E9;}
# 2082
goto _LL1ED;_LL1EC:;_LL1ED: {
# 2084
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp989;struct Cyc_Absyn_AggrInfo _tmp988;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp987;res_typ=(void*)((_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987[0]=((_tmp989.tag=11,((_tmp989.f1=((_tmp988.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp988.targs=_tmp4A3,_tmp988)))),_tmp989)))),_tmp987))));}_LL1E9:;}else{
# 2087
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp98F;struct Cyc_Absyn_AggrInfo _tmp98E;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp98D;res_typ=(void*)((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98F.tag=11,((_tmp98F.f1=((_tmp98E.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp98E.targs=_tmp4A3,_tmp98E)))),_tmp98F)))),_tmp98D))));}{
# 2090
struct Cyc_List_List*_tmp4B2=*ts;
struct Cyc_List_List*_tmp4B3=_tmp4A4;
while(_tmp4B2 != 0  && _tmp4B3 != 0){
# 2094
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp4B2->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp4B2->hd);
Cyc_Tcutil_unify((void*)_tmp4B2->hd,(void*)_tmp4B3->hd);
_tmp4B2=_tmp4B2->tl;
_tmp4B3=_tmp4B3->tl;}
# 2100
if(_tmp4B2 != 0){
const char*_tmp992;void*_tmp991;(_tmp991=0,Cyc_Tcutil_terr(loc,((_tmp992="too many explicit witness types",_tag_dyneither(_tmp992,sizeof(char),32))),_tag_dyneither(_tmp991,sizeof(void*),0)));}
# 2103
*ts=_tmp4A4;{
# 2106
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4B6=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B9;struct _tuple19*_tmp4B7=_tmp4B6;_tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;{
void*_tmp4BA=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,_tmp4B8->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp4BA,_tmp4B9);
# 2117
if(!Cyc_Tcutil_coerce_arg(te2,_tmp4B9,_tmp4BA,& bogus)){
{const char*_tmp99E;void*_tmp99D[5];struct Cyc_String_pa_PrintArg_struct _tmp99C;const char*_tmp99B;const char*_tmp99A;struct Cyc_String_pa_PrintArg_struct _tmp999;struct Cyc_String_pa_PrintArg_struct _tmp998;struct Cyc_String_pa_PrintArg_struct _tmp997;struct Cyc_String_pa_PrintArg_struct _tmp996;(_tmp996.tag=0,((_tmp996.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2121
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4B9->topt))),((_tmp997.tag=0,((_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_typ2string(_tmp4BA)),((_tmp998.tag=0,((_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp999.tag=0,((_tmp999.f1=(struct _dyneither_ptr)(
# 2119
ad->kind == Cyc_Absyn_StructA?(_tmp99A="struct",_tag_dyneither(_tmp99A,sizeof(char),7)):((_tmp99B="union",_tag_dyneither(_tmp99B,sizeof(char),6)))),((_tmp99C.tag=0,((_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B8->name),((_tmp99D[0]=& _tmp99C,((_tmp99D[1]=& _tmp999,((_tmp99D[2]=& _tmp998,((_tmp99D[3]=& _tmp997,((_tmp99D[4]=& _tmp996,Cyc_Tcutil_terr(_tmp4B9->loc,((_tmp99E="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp99E,sizeof(char),40))),_tag_dyneither(_tmp99D,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2122
Cyc_Tcutil_explain_failure();}};}{
# 2126
struct Cyc_List_List*_tmp4C4=0;
{struct Cyc_List_List*_tmp4C5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp4C5 != 0;_tmp4C5=_tmp4C5->tl){
struct _tuple0*_tmp9A1;struct Cyc_List_List*_tmp9A0;_tmp4C4=((_tmp9A0=_cycalloc(sizeof(*_tmp9A0)),((_tmp9A0->hd=((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f1),((_tmp9A1->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp4A5,(*((struct _tuple0*)_tmp4C5->hd)).f2),_tmp9A1)))))),((_tmp9A0->tl=_tmp4C4,_tmp9A0))))));}}
# 2131
_tmp4C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4C4);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp4C4);{
void*_tmp4C8=res_typ;_npop_handler(0);return _tmp4C8;};};};};};}
# 2049
;_pop_region(rgn);};}
# 2138
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2140
struct _RegionHandle _tmp4CA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4CA;_push_region(rgn);{
void*_tmp4CB=Cyc_Tcutil_compress(ts);void*_tmp4CC=_tmp4CB;enum Cyc_Absyn_AggrKind _tmp4CE;struct Cyc_List_List*_tmp4CF;_LL1EF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4CD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4CC;if(_tmp4CD->tag != 12)goto _LL1F1;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;}}_LL1F0:
# 2143
 if(_tmp4CE == Cyc_Absyn_UnionA){
const char*_tmp9A4;void*_tmp9A3;(_tmp9A3=0,Cyc_Tcutil_terr(loc,((_tmp9A4="expecting struct but found union",_tag_dyneither(_tmp9A4,sizeof(char),33))),_tag_dyneither(_tmp9A3,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp4CF);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4D2=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D5;struct _tuple19*_tmp4D3=_tmp4D2;_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp4D4->type,_tmp4D5);
# 2152
if(!Cyc_Tcutil_coerce_arg(te,_tmp4D5,_tmp4D4->type,& bogus)){
{const char*_tmp9AA;void*_tmp9A9[3];struct Cyc_String_pa_PrintArg_struct _tmp9A8;struct Cyc_String_pa_PrintArg_struct _tmp9A7;struct Cyc_String_pa_PrintArg_struct _tmp9A6;(_tmp9A6.tag=0,((_tmp9A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2155
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4D5->topt))),((_tmp9A7.tag=0,((_tmp9A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2154
Cyc_Absynpp_typ2string(_tmp4D4->type)),((_tmp9A8.tag=0,((_tmp9A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D4->name),((_tmp9A9[0]=& _tmp9A8,((_tmp9A9[1]=& _tmp9A7,((_tmp9A9[2]=& _tmp9A6,Cyc_Tcutil_terr(_tmp4D5->loc,((_tmp9AA="field %s of struct expects type %s != %s",_tag_dyneither(_tmp9AA,sizeof(char),41))),_tag_dyneither(_tmp9A9,sizeof(void*),3)))))))))))))))))));}
# 2156
Cyc_Tcutil_explain_failure();}}
# 2159
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp9AD;void*_tmp9AC;(_tmp9AC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9AD="tcAnonStruct: wrong type",_tag_dyneither(_tmp9AD,sizeof(char),25))),_tag_dyneither(_tmp9AC,sizeof(void*),0)));}_LL1EE:;}{
# 2163
void*_tmp4DD=ts;_npop_handler(0);return _tmp4DD;};
# 2140
;_pop_region(rgn);}
# 2167
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2170
struct _RegionHandle _tmp4DE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4DE;_push_region(rgn);
{struct _tuple12 _tmp9AE;struct _tuple12 _tmp4DF=(_tmp9AE.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9AE.f2=rgn,_tmp9AE)));
struct Cyc_List_List*_tmp4E0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4DF,tud->tvs);
struct Cyc_List_List*_tmp4E1=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4E0);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp9B4;struct Cyc_Absyn_DatatypeFieldInfo _tmp9B3;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9B2;void*res=(void*)(
(_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B4.tag=4,((_tmp9B4.f1=((_tmp9B3.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp9B3.targs=_tmp4E1,_tmp9B3)))),_tmp9B4)))),_tmp9B2))));
# 2178
if(topt != 0){
void*_tmp4E2=Cyc_Tcutil_compress(*topt);void*_tmp4E3=_tmp4E2;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4E4=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4E3;if(_tmp4E4->tag != 4)goto _LL1F6;}_LL1F5:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F3;_LL1F6:;_LL1F7:
 goto _LL1F3;_LL1F3:;}{
# 2184
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp4E0 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp4E0,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp9BC;void*_tmp9BB[3];struct Cyc_String_pa_PrintArg_struct _tmp9BA;struct Cyc_String_pa_PrintArg_struct _tmp9B9;const char*_tmp9B8;struct Cyc_String_pa_PrintArg_struct _tmp9B7;(_tmp9B7.tag=0,((_tmp9B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2195
e->topt == 0?(struct _dyneither_ptr)((_tmp9B8="?",_tag_dyneither(_tmp9B8,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp9B9.tag=0,((_tmp9B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2194
Cyc_Absynpp_typ2string(t)),((_tmp9BA.tag=0,((_tmp9BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9BB[0]=& _tmp9BA,((_tmp9BB[1]=& _tmp9B9,((_tmp9BB[2]=& _tmp9B7,Cyc_Tcutil_terr(e->loc,((_tmp9BC="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp9BC,sizeof(char),82))),_tag_dyneither(_tmp9BB,sizeof(void*),3)))))))))))))))))));}
# 2196
Cyc_Tcutil_explain_failure();}}
# 2199
if(es != 0){
const char*_tmp9C0;void*_tmp9BF[1];struct Cyc_String_pa_PrintArg_struct _tmp9BE;void*_tmp4EE=(_tmp9BE.tag=0,((_tmp9BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2202
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9BF[0]=& _tmp9BE,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp9C0="too many arguments for datatype constructor %s",_tag_dyneither(_tmp9C0,sizeof(char),47))),_tag_dyneither(_tmp9BF,sizeof(void*),1)))))));_npop_handler(0);return _tmp4EE;}
if(ts != 0){
const char*_tmp9C4;void*_tmp9C3[1];struct Cyc_String_pa_PrintArg_struct _tmp9C2;void*_tmp4F2=(_tmp9C2.tag=0,((_tmp9C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9C3[0]=& _tmp9C2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9C4="too few arguments for datatype constructor %s",_tag_dyneither(_tmp9C4,sizeof(char),46))),_tag_dyneither(_tmp9C3,sizeof(void*),1)))))));_npop_handler(0);return _tmp4F2;}{
void*_tmp4F3=res;_npop_handler(0);return _tmp4F3;};};}
# 2171
;_pop_region(rgn);}
# 2210
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp4F8=Cyc_Tcutil_compress(t);void*_tmp4F9=_tmp4F8;union Cyc_Absyn_Constraint*_tmp500;void*_tmp504;struct Cyc_List_List*_tmp507;union Cyc_Absyn_AggrInfoU _tmp509;struct Cyc_List_List*_tmp50A;struct Cyc_List_List*_tmp50E;_LL1F9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FA->tag != 0)goto _LL1FB;}_LL1FA:
 return 1;_LL1FB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4F9;if(_tmp4FB->tag != 1)goto _LL1FD;}_LL1FC:
 goto _LL1FE;_LL1FD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FC->tag != 2)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FD->tag != 3)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FE->tag != 4)goto _LL203;}_LL202:
 return 0;_LL203: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4FF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F9;if(_tmp4FF->tag != 5)goto _LL205;else{_tmp500=((_tmp4FF->f1).ptr_atts).nullable;}}_LL204:
# 2218
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp500);_LL205: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp501=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F9;if(_tmp501->tag != 6)goto _LL207;}_LL206:
 goto _LL208;_LL207: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp502=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4F9;if(_tmp502->tag != 7)goto _LL209;}_LL208:
 return 1;_LL209: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F9;if(_tmp503->tag != 8)goto _LL20B;else{_tmp504=(_tmp503->f1).elt_type;}}_LL20A:
 return Cyc_Tcexp_zeroable_type(_tmp504);_LL20B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4F9;if(_tmp505->tag != 9)goto _LL20D;}_LL20C:
 return 0;_LL20D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp506=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F9;if(_tmp506->tag != 10)goto _LL20F;else{_tmp507=_tmp506->f1;}}_LL20E:
# 2224
 for(0;(unsigned int)_tmp507;_tmp507=_tmp507->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp507->hd)).f2))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F9;if(_tmp508->tag != 11)goto _LL211;else{_tmp509=(_tmp508->f1).aggr_info;_tmp50A=(_tmp508->f1).targs;}}_LL210: {
# 2228
struct Cyc_Absyn_Aggrdecl*_tmp51B=Cyc_Absyn_get_known_aggrdecl(_tmp509);
if(_tmp51B->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp51C=_new_region("r");struct _RegionHandle*r=& _tmp51C;_push_region(r);
{struct Cyc_List_List*_tmp51D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp51B->tvs,_tmp50A);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp51B->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp51D,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp51E=0;_npop_handler(0);return _tmp51E;}}}{
int _tmp51F=1;_npop_handler(0);return _tmp51F;};}
# 2232
;_pop_region(r);};}_LL211: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4F9;if(_tmp50B->tag != 13)goto _LL213;}_LL212:
# 2237
 return 1;_LL213: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F9;if(_tmp50C->tag != 19)goto _LL215;}_LL214:
 return 1;_LL215: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F9;if(_tmp50D->tag != 12)goto _LL217;else{_tmp50E=_tmp50D->f2;}}_LL216:
# 2240
 for(0;_tmp50E != 0;_tmp50E=_tmp50E->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp50E->hd)->type))return 0;}
return 1;_LL217: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4F9;if(_tmp50F->tag != 14)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4F9;if(_tmp510->tag != 17)goto _LL21B;}_LL21A:
 return 0;_LL21B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4F9;if(_tmp511->tag != 16)goto _LL21D;}_LL21C:
 return 0;_LL21D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F9;if(_tmp512->tag != 15)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4F9;if(_tmp513->tag != 26)goto _LL221;}_LL220:
# 2248
 goto _LL222;_LL221: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F9;if(_tmp514->tag != 18)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp515->tag != 20)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp516->tag != 21)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4F9;if(_tmp517->tag != 22)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4F9;if(_tmp518->tag != 23)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4F9;if(_tmp519->tag != 24)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4F9;if(_tmp51A->tag != 25)goto _LL1F8;}_LL22E: {
const char*_tmp9C8;void*_tmp9C7[1];struct Cyc_String_pa_PrintArg_struct _tmp9C6;(_tmp9C6.tag=0,((_tmp9C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9C7[0]=& _tmp9C6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C8="bad type `%s' in zeroable type",_tag_dyneither(_tmp9C8,sizeof(char),31))),_tag_dyneither(_tmp9C7,sizeof(void*),1)))))));}_LL1F8:;}
# 2259
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2261
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2263
if(topt != 0){
void*_tmp523=Cyc_Tcutil_compress(*topt);void*_tmp524=_tmp523;void*_tmp526;_LL230: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp524;if(_tmp525->tag != 5)goto _LL232;else{_tmp526=(_tmp525->f1).elt_typ;}}_LL231:
# 2266
 Cyc_Tcutil_unify(_tmp526,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL22F;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 2272
const char*_tmp9D1;void*_tmp9D0[2];const char*_tmp9CF;const char*_tmp9CE;struct Cyc_String_pa_PrintArg_struct _tmp9CD;struct Cyc_String_pa_PrintArg_struct _tmp9CC;(_tmp9CC.tag=0,((_tmp9CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp9CD.tag=0,((_tmp9CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp9CE="calloc",_tag_dyneither(_tmp9CE,sizeof(char),7))):(struct _dyneither_ptr)((_tmp9CF="malloc",_tag_dyneither(_tmp9CF,sizeof(char),7))))),((_tmp9D0[0]=& _tmp9CD,((_tmp9D0[1]=& _tmp9CC,Cyc_Tcutil_terr(loc,((_tmp9D1="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp9D1,sizeof(char),60))),_tag_dyneither(_tmp9D0,sizeof(void*),2)))))))))))));};}
# 2276
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2278
enum Cyc_Absyn_AliasQual _tmp52D=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_tmp52D){case Cyc_Absyn_Unique: _LL234:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL235:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2284
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2289
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2292
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9D4;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9D3;void*expected_type=(void*)(
(_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=15,((_tmp9D4.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9D4)))),_tmp9D3))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp52E=Cyc_Tcutil_compress(handle_type);void*_tmp52F=_tmp52E;void*_tmp531;_LL238: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52F;if(_tmp530->tag != 15)goto _LL23A;else{_tmp531=(void*)_tmp530->f1;}}_LL239:
# 2297
 rgn=_tmp531;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL237;_LL23A:;_LL23B:
# 2301
{const char*_tmp9D8;void*_tmp9D7[1];struct Cyc_String_pa_PrintArg_struct _tmp9D6;(_tmp9D6.tag=0,((_tmp9D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9D7[0]=& _tmp9D6,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9D8="expecting region_t type but found %s",_tag_dyneither(_tmp9D8,sizeof(char),37))),_tag_dyneither(_tmp9D7,sizeof(void*),1)))))));}
goto _LL237;_LL237:;}else{
# 2308
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2315
{struct _RegionHandle _tmp537=_new_region("r");struct _RegionHandle*r=& _tmp537;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2326 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp9DB;void*_tmp9DA;(_tmp9DA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9DB="calloc with empty type",_tag_dyneither(_tmp9DB,sizeof(char),23))),_tag_dyneither(_tmp9DA,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2338
void*er=(*e)->r;
retry_sizeof: {
void*_tmp53A=er;void*_tmp53C;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp53F;_LL23D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp53A;if(_tmp53B->tag != 16)goto _LL23F;else{_tmp53C=(void*)_tmp53B->f1;}}_LL23E:
# 2342
 elt_type=_tmp53C;
{void**_tmp9DC;*t=((_tmp9DC=_cycalloc(sizeof(*_tmp9DC)),((_tmp9DC[0]=elt_type,_tmp9DC))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL23C;_LL23F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp53A;if(_tmp53D->tag != 2)goto _LL241;else{if(_tmp53D->f1 != Cyc_Absyn_Times)goto _LL241;if(_tmp53D->f2 == 0)goto _LL241;_tmp53E=(struct Cyc_Absyn_Exp*)(_tmp53D->f2)->hd;if((_tmp53D->f2)->tl == 0)goto _LL241;_tmp53F=(struct Cyc_Absyn_Exp*)((_tmp53D->f2)->tl)->hd;if(((_tmp53D->f2)->tl)->tl != 0)goto _LL241;}}_LL240:
# 2349
{struct _tuple0 _tmp9DD;struct _tuple0 _tmp541=(_tmp9DD.f1=_tmp53E->r,((_tmp9DD.f2=_tmp53F->r,_tmp9DD)));struct _tuple0 _tmp542=_tmp541;void*_tmp544;void*_tmp546;_LL244: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp543=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp542.f1;if(_tmp543->tag != 16)goto _LL246;else{_tmp544=(void*)_tmp543->f1;}}_LL245:
# 2351
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp544);
elt_type=_tmp544;
{void**_tmp9DE;*t=((_tmp9DE=_cycalloc(sizeof(*_tmp9DE)),((_tmp9DE[0]=elt_type,_tmp9DE))));}
num_elts=_tmp53F;
one_elt=0;
goto _LL243;_LL246: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp542.f2;if(_tmp545->tag != 16)goto _LL248;else{_tmp546=(void*)_tmp545->f1;}}_LL247:
# 2358
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp546);
elt_type=_tmp546;
{void**_tmp9DF;*t=((_tmp9DF=_cycalloc(sizeof(*_tmp9DF)),((_tmp9DF[0]=elt_type,_tmp9DF))));}
num_elts=_tmp53E;
one_elt=0;
goto _LL243;_LL248:;_LL249:
 goto No_sizeof;_LL243:;}
# 2366
goto _LL23C;_LL241:;_LL242:
# 2368
 No_sizeof: {
# 2371
struct Cyc_Absyn_Exp*_tmp54A=*e;
{void*_tmp54B=_tmp54A->r;void*_tmp54C=_tmp54B;struct Cyc_Absyn_Exp*_tmp54E;_LL24B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp54D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54C;if(_tmp54D->tag != 13)goto _LL24D;else{_tmp54E=_tmp54D->f2;}}_LL24C:
 _tmp54A=_tmp54E;goto _LL24A;_LL24D:;_LL24E:
 goto _LL24A;_LL24A:;}
# 2376
{void*_tmp54F=Cyc_Tcutil_compress((void*)_check_null(_tmp54A->topt));void*_tmp550=_tmp54F;void*_tmp552;_LL250: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp551=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp550;if(_tmp551->tag != 19)goto _LL252;else{_tmp552=(void*)_tmp551->f1;}}_LL251:
# 2378
{void*_tmp553=Cyc_Tcutil_compress(_tmp552);void*_tmp554=_tmp553;struct Cyc_Absyn_Exp*_tmp556;_LL255: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp554;if(_tmp555->tag != 18)goto _LL257;else{_tmp556=_tmp555->f1;}}_LL256:
# 2380
 er=_tmp556->r;goto retry_sizeof;_LL257:;_LL258:
 goto _LL254;_LL254:;}
# 2383
goto _LL24F;_LL252:;_LL253:
 goto _LL24F;_LL24F:;}
# 2386
elt_type=Cyc_Absyn_char_typ;
{void**_tmp9E0;*t=((_tmp9E0=_cycalloc(sizeof(*_tmp9E0)),((_tmp9E0[0]=elt_type,_tmp9E0))));}
num_elts=*e;
one_elt=0;}
# 2391
goto _LL23C;_LL23C:;}}
# 2395
*is_fat=!one_elt;
# 2398
{void*_tmp558=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp55A;_LL25A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp558;if(_tmp559->tag != 11)goto _LL25C;else{if((((_tmp559->f1).aggr_info).KnownAggr).tag != 2)goto _LL25C;_tmp55A=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp559->f1).aggr_info).KnownAggr).val);}}_LL25B:
# 2400
 if(_tmp55A->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55A->impl))->exist_vars != 0){
const char*_tmp9E3;void*_tmp9E2;(_tmp9E2=0,Cyc_Tcutil_terr(loc,((_tmp9E3="malloc with existential types not yet implemented",_tag_dyneither(_tmp9E3,sizeof(char),50))),_tag_dyneither(_tmp9E2,sizeof(void*),0)));}
goto _LL259;_LL25C:;_LL25D:
 goto _LL259;_LL259:;}{
# 2407
void*(*_tmp55D)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp55E=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp55F=Cyc_Tcutil_compress(*topt);void*_tmp560=_tmp55F;union Cyc_Absyn_Constraint*_tmp562;union Cyc_Absyn_Constraint*_tmp563;union Cyc_Absyn_Constraint*_tmp564;_LL25F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560;if(_tmp561->tag != 5)goto _LL261;else{_tmp562=((_tmp561->f1).ptr_atts).nullable;_tmp563=((_tmp561->f1).ptr_atts).bounds;_tmp564=((_tmp561->f1).ptr_atts).zero_term;}}_LL260:
# 2412
 _tmp55E=_tmp564;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp562))
_tmp55D=Cyc_Absyn_star_typ;
# 2417
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp564) && !(*is_calloc)){
{const char*_tmp9E6;void*_tmp9E5;(_tmp9E5=0,Cyc_Tcutil_warn(loc,((_tmp9E6="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9E6,sizeof(char),55))),_tag_dyneither(_tmp9E5,sizeof(void*),0)));}
*is_calloc=1;}
# 2423
{void*_tmp567=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp563);void*_tmp568=_tmp567;struct Cyc_Absyn_Exp*_tmp56B;_LL264: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp569=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp568;if(_tmp569->tag != 0)goto _LL266;}_LL265:
 goto _LL263;_LL266:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp56A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp568;if(_tmp56A->tag != 1)goto _LL268;else{_tmp56B=_tmp56A->f1;}}if(!(!one_elt))goto _LL268;_LL267: {
# 2426
int _tmp56C=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp56C  && Cyc_Evexp_same_const_exp(_tmp56B,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp563),_tmp55E);}
# 2432
{void*_tmp56D=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp56E=_tmp56D;void*_tmp570;_LL26B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp56E;if(_tmp56F->tag != 19)goto _LL26D;else{_tmp570=(void*)_tmp56F->f1;}}_LL26C: {
# 2434
struct Cyc_Absyn_Exp*_tmp571=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp570,0),0,Cyc_Absyn_No_coercion,0);
# 2436
if(Cyc_Evexp_same_const_exp(_tmp571,_tmp56B)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp563),_tmp55E);}
# 2441
goto _LL26A;}_LL26D:;_LL26E:
 goto _LL26A;_LL26A:;}
# 2444
goto _LL263;}_LL268:;_LL269:
 goto _LL263;_LL263:;}
# 2447
goto _LL25E;_LL261:;_LL262:
 goto _LL25E;_LL25E:;}
# 2450
if(!one_elt)_tmp55D=Cyc_Absyn_dyneither_typ;
return _tmp55D(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp55E);};};}
# 2455
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2461
struct _RegionHandle _tmp572=_new_region("r");struct _RegionHandle*r=& _tmp572;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp573=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp573,0,e1);{
void*_tmp574=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp573,& _tmp574,e2);};}{
# 2467
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2470
{void*_tmp575=Cyc_Tcutil_compress(t1);void*_tmp576=_tmp575;_LL270: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp577=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp576;if(_tmp577->tag != 8)goto _LL272;}_LL271:
{const char*_tmp9E9;void*_tmp9E8;(_tmp9E8=0,Cyc_Tcutil_terr(loc,((_tmp9E9="cannot assign to an array",_tag_dyneither(_tmp9E9,sizeof(char),26))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));}goto _LL26F;_LL272:;_LL273:
 goto _LL26F;_LL26F:;}
# 2475
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1)){
const char*_tmp9EC;void*_tmp9EB;(_tmp9EB=0,Cyc_Tcutil_terr(loc,((_tmp9EC="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9EC,sizeof(char),58))),_tag_dyneither(_tmp9EB,sizeof(void*),0)));}
# 2479
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp9EF;void*_tmp9EE;void*_tmp57E=(_tmp9EE=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9EF="swap non-lvalue",_tag_dyneither(_tmp9EF,sizeof(char),16))),_tag_dyneither(_tmp9EE,sizeof(void*),0)));_npop_handler(0);return _tmp57E;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp9F2;void*_tmp9F1;void*_tmp581=(_tmp9F1=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9F2="swap non-lvalue",_tag_dyneither(_tmp9F2,sizeof(char),16))),_tag_dyneither(_tmp9F1,sizeof(void*),0)));_npop_handler(0);return _tmp581;}{
# 2484
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9F5;void*_tmp9F4;void*_tmp584=(_tmp9F4=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9F5="swap value in zeroterm array",_tag_dyneither(_tmp9F5,sizeof(char),29))),_tag_dyneither(_tmp9F4,sizeof(void*),0)));_npop_handler(0);return _tmp584;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9F8;void*_tmp9F7;void*_tmp587=(_tmp9F7=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9F8="swap value in zeroterm array",_tag_dyneither(_tmp9F8,sizeof(char),29))),_tag_dyneither(_tmp9F7,sizeof(void*),0)));_npop_handler(0);return _tmp587;}
# 2491
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9FD;void*_tmp9FC[2];struct Cyc_String_pa_PrintArg_struct _tmp9FB;struct Cyc_String_pa_PrintArg_struct _tmp9FA;void*_tmp588=(_tmp9FA.tag=0,((_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp9FB.tag=0,((_tmp9FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9FC[0]=& _tmp9FB,((_tmp9FC[1]=& _tmp9FA,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9FD="type mismatch: %s != %s",_tag_dyneither(_tmp9FD,sizeof(char),24))),_tag_dyneither(_tmp9FC,sizeof(void*),2)))))))))))));
void*_tmp589=_tmp588;_npop_handler(0);return _tmp589;}{
# 2498
void*_tmp58E=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp58E;};};};
# 2461
;_pop_region(r);}
# 2502
int Cyc_Tcexp_in_stmt_exp=0;
# 2504
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp58F=_new_region("r");struct _RegionHandle*r=& _tmp58F;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2511
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2513
while(1){
void*_tmp590=s->r;void*_tmp591=_tmp590;struct Cyc_Absyn_Exp*_tmp593;struct Cyc_Absyn_Stmt*_tmp595;struct Cyc_Absyn_Stmt*_tmp596;struct Cyc_Absyn_Decl*_tmp598;struct Cyc_Absyn_Stmt*_tmp599;_LL275: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp592=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp591;if(_tmp592->tag != 1)goto _LL277;else{_tmp593=_tmp592->f1;}}_LL276: {
# 2517
void*_tmp59A=(void*)_check_null(_tmp593->topt);
if(!Cyc_Tcutil_unify(_tmp59A,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmpA01;void*_tmpA00[1];struct Cyc_String_pa_PrintArg_struct _tmp9FF;(_tmp9FF.tag=0,((_tmp9FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp59A)),((_tmpA00[0]=& _tmp9FF,Cyc_Tcutil_terr(loc,((_tmpA01="statement expression returns type %s",_tag_dyneither(_tmpA01,sizeof(char),37))),_tag_dyneither(_tmpA00,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2523
void*_tmp59E=_tmp59A;_npop_handler(0);return _tmp59E;};}_LL277: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp594=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp591;if(_tmp594->tag != 2)goto _LL279;else{_tmp595=_tmp594->f1;_tmp596=_tmp594->f2;}}_LL278:
 s=_tmp596;continue;_LL279: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp597=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp591;if(_tmp597->tag != 12)goto _LL27B;else{_tmp598=_tmp597->f1;_tmp599=_tmp597->f2;}}_LL27A:
 s=_tmp599;continue;_LL27B:;_LL27C: {
# 2527
const char*_tmpA04;void*_tmpA03;void*_tmp5A1=(_tmpA03=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmpA04="statement expression must end with expression",_tag_dyneither(_tmpA04,sizeof(char),46))),_tag_dyneither(_tmpA03,sizeof(void*),0)));_npop_handler(0);return _tmp5A1;}_LL274:;}
# 2505
;_pop_region(r);}
# 2532
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp5A2=_new_region("r");struct _RegionHandle*r=& _tmp5A2;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2537
{void*_tmp5A3=t;struct Cyc_Absyn_Aggrdecl*_tmp5A5;_LL27E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A3;if(_tmp5A4->tag != 11)goto _LL280;else{if((((_tmp5A4->f1).aggr_info).KnownAggr).tag != 2)goto _LL280;_tmp5A5=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp5A4->f1).aggr_info).KnownAggr).val);}}_LL27F:
# 2539
 if((_tmp5A5->kind == Cyc_Absyn_UnionA  && _tmp5A5->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5A5->impl))->tagged)goto _LL27D;
goto _LL281;_LL280:;_LL281:
# 2542
{const char*_tmpA08;void*_tmpA07[1];struct Cyc_String_pa_PrintArg_struct _tmpA06;(_tmpA06.tag=0,((_tmpA06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA07[0]=& _tmpA06,Cyc_Tcutil_terr(loc,((_tmpA08="expecting @tagged union but found %s",_tag_dyneither(_tmpA08,sizeof(char),37))),_tag_dyneither(_tmpA07,sizeof(void*),1)))))));}
goto _LL27D;_LL27D:;}{
# 2545
void*_tmp5A9=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp5A9;};
# 2534
;_pop_region(r);}
# 2549
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2553
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp5AA=_new_region("r");struct _RegionHandle*r=& _tmp5AA;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp5AB=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2558
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA0B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA0A;void*expected_type=(void*)(
(_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A[0]=((_tmpA0B.tag=15,((_tmpA0B.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp5AB)),_tmpA0B)))),_tmpA0A))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp5AB,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp5AC=Cyc_Tcutil_compress(handle_type);void*_tmp5AD=_tmp5AC;void*_tmp5AF;_LL283: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5AD;if(_tmp5AE->tag != 15)goto _LL285;else{_tmp5AF=(void*)_tmp5AE->f1;}}_LL284:
# 2563
 rgn=_tmp5AF;
Cyc_Tcenv_check_rgn_accessible(_tmp5AB,loc,rgn);
goto _LL282;_LL285:;_LL286:
# 2567
{const char*_tmpA0F;void*_tmpA0E[1];struct Cyc_String_pa_PrintArg_struct _tmpA0D;(_tmpA0D.tag=0,((_tmpA0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmpA0E[0]=& _tmpA0D,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmpA0F="expecting region_t type but found %s",_tag_dyneither(_tmpA0F,sizeof(char),37))),_tag_dyneither(_tmpA0E,sizeof(void*),1)))))));}
goto _LL282;_LL282:;}else{
# 2574
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2582
void*_tmp5B5=e1->r;void*_tmp5B6=_tmp5B5;struct Cyc_Core_Opt*_tmp5BA;struct Cyc_List_List*_tmp5BB;struct Cyc_List_List*_tmp5BD;_LL288: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5B7->tag != 26)goto _LL28A;}_LL289:
 goto _LL28B;_LL28A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5B8=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5B8->tag != 27)goto _LL28C;}_LL28B: {
# 2587
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp5AB,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA12;void*_tmpA11;(_tmpA11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA12="tcNew: comprehension returned non-array type",_tag_dyneither(_tmpA12,sizeof(char),45))),_tag_dyneither(_tmpA11,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AB,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AB,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2598
void*_tmp5C2=res_typ;_npop_handler(0);return _tmp5C2;};}_LL28C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5B9=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5B9->tag != 35)goto _LL28E;else{_tmp5BA=_tmp5B9->f1;_tmp5BB=_tmp5B9->f2;}}_LL28D:
# 2600
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpA15;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpA14;e1->r=(void*)((_tmpA14=_cycalloc(sizeof(*_tmpA14)),((_tmpA14[0]=((_tmpA15.tag=25,((_tmpA15.f1=_tmp5BB,_tmpA15)))),_tmpA14))));}
_tmp5BD=_tmp5BB;goto _LL28F;_LL28E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5BC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5BC->tag != 25)goto _LL290;else{_tmp5BD=_tmp5BC->f1;}}_LL28F: {
# 2603
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp5C5=Cyc_Tcutil_compress(*topt);void*_tmp5C6=_tmp5C5;void**_tmp5C8;struct Cyc_Absyn_Tqual _tmp5C9;union Cyc_Absyn_Constraint*_tmp5CA;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C6;if(_tmp5C7->tag != 5)goto _LL299;else{_tmp5C8=(void**)&(_tmp5C7->f1).elt_typ;_tmp5C9=(_tmp5C7->f1).elt_tq;_tmp5CA=((_tmp5C7->f1).ptr_atts).zero_term;}}_LL298:
# 2609
 elt_typ_opt=_tmp5C8;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5CA);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2615
void*res_typ=Cyc_Tcexp_tcArray(_tmp5AB,e1->loc,elt_typ_opt,0,zero_term,_tmp5BD);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA18;void*_tmpA17;(_tmpA17=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA18="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmpA18,sizeof(char),50))),_tag_dyneither(_tmpA17,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2624
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AB,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AB,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2630
void*_tmp5CD=res_typ;_npop_handler(0);return _tmp5CD;};};}_LL290: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5BE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5BE->tag != 0)goto _LL292;else{if(((_tmp5BE->f1).String_c).tag != 8)goto _LL292;}}_LL291: {
# 2635
void*_tmp5CE=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2637
void*_tmp5CF=Cyc_Tcexp_tcExp(_tmp5AB,& _tmp5CE,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA1B;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA1A;void*_tmp5D2=Cyc_Absyn_atb_typ(_tmp5CF,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA1A=_cycalloc(sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1B.tag=1,((_tmpA1B.f1=Cyc_Absyn_uint_exp(1,0),_tmpA1B)))),_tmpA1A)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5D2;}_LL292: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5BF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5B6;if(_tmp5BF->tag != 0)goto _LL294;else{if(((_tmp5BF->f1).Wstring_c).tag != 9)goto _LL294;}}_LL293: {
# 2642
void*_tmp5D3=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2644
void*_tmp5D4=Cyc_Tcexp_tcExp(_tmp5AB,& _tmp5D3,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA1E;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA1D;void*_tmp5D7=Cyc_Absyn_atb_typ(_tmp5D4,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA1D=_cycalloc(sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1E.tag=1,((_tmpA1E.f1=Cyc_Absyn_uint_exp(1,0),_tmpA1E)))),_tmpA1D)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5D7;}_LL294:;_LL295:
# 2651
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp5D8=Cyc_Tcutil_compress(*topt);void*_tmp5D9=_tmp5D8;void**_tmp5DB;struct Cyc_Absyn_Tqual _tmp5DC;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D9;if(_tmp5DA->tag != 5)goto _LL29E;else{_tmp5DB=(void**)&(_tmp5DA->f1).elt_typ;_tmp5DC=(_tmp5DA->f1).elt_tq;}}_LL29D:
# 2657
 topt2=_tmp5DB;goto _LL29B;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5D9;if(_tmp5DD->tag != 3)goto _LL2A0;}_LL29F:
# 2661
 bogus=*topt;
topt2=& bogus;
goto _LL29B;_LL2A0:;_LL2A1:
 goto _LL29B;_LL29B:;}{
# 2667
void*telt=Cyc_Tcexp_tcExp(_tmp5AB,topt2,e1);
# 2669
struct _RegionHandle _tmp5DE=_new_region("temp");struct _RegionHandle*temp=& _tmp5DE;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmpA21;void*_tmpA20;(_tmpA20=0,Cyc_Tcutil_terr(e1->loc,((_tmpA21="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA21,sizeof(char),49))),_tag_dyneither(_tmpA20,sizeof(void*),0)));}{
# 2674
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA27;struct Cyc_Absyn_PtrInfo _tmpA26;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA25;void*res_typ=(void*)(
(_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA27.tag=5,((_tmpA27.f1=((_tmpA26.elt_typ=telt,((_tmpA26.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA26.ptr_atts=(
((_tmpA26.ptr_atts).rgn=rgn,(((_tmpA26.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpA26.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmpA26.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmpA26.ptr_atts).ptrloc=0,_tmpA26.ptr_atts)))))))))),_tmpA26)))))),_tmpA27)))),_tmpA25))));
# 2679
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5AB,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5AB,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2686
void*_tmp5E1=res_typ;_npop_handler(1);return _tmp5E1;};};
# 2669
;_pop_region(temp);};}_LL287:;};}
# 2555
;_pop_region(r);}
# 2693
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2703
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2707
struct _RegionHandle _tmp5E5=_new_region("temp");struct _RegionHandle*temp=& _tmp5E5;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2713
const char*_tmpA2A;void*_tmpA29;(_tmpA29=0,Cyc_Tcutil_terr(e->loc,((_tmpA2A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA2A,sizeof(char),49))),_tag_dyneither(_tmpA29,sizeof(void*),0)));}{
# 2716
void*_tmp5E8=e->r;void*_tmp5E9=_tmp5E8;_LL2A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5EA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EA->tag != 25)goto _LL2A5;}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EB->tag != 26)goto _LL2A7;}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5EC=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EC->tag != 27)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5ED=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5ED->tag != 0)goto _LL2AB;else{if(((_tmp5ED->f1).Wstring_c).tag != 9)goto _LL2AB;}}_LL2AA:
 goto _LL2AC;_LL2AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5EE=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E9;if(_tmp5EE->tag != 0)goto _LL2AD;else{if(((_tmp5EE->f1).String_c).tag != 8)goto _LL2AD;}}_LL2AC: {
void*_tmp5EF=t;_npop_handler(0);return _tmp5EF;}_LL2AD:;_LL2AE:
# 2723
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2726
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2729
void*_tmp5F0=t;_npop_handler(0);return _tmp5F0;};_LL2A2:;};
# 2707
;_pop_region(temp);}
# 2740 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp5F1=e->r;void*_tmp5F2=_tmp5F1;struct Cyc_Absyn_Exp*_tmp5F4;_LL2B0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5F3=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5F2;if(_tmp5F3->tag != 11)goto _LL2B2;else{_tmp5F4=_tmp5F3->f1;}}_LL2B1:
# 2744
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5F4);
_tmp5F4->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5F4->topt),0);
e->topt=_tmp5F4->topt;
goto _LL2AF;_LL2B2:;_LL2B3:
# 2750
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2753
{void*_tmp5F5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5F6=_tmp5F5;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5FA;union Cyc_Absyn_Constraint*_tmp5FB;union Cyc_Absyn_Constraint*_tmp5FC;union Cyc_Absyn_Constraint*_tmp5FD;_LL2B5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F6;if(_tmp5F7->tag != 5)goto _LL2B7;else{_tmp5F8=(_tmp5F7->f1).elt_typ;_tmp5F9=(_tmp5F7->f1).elt_tq;_tmp5FA=((_tmp5F7->f1).ptr_atts).rgn;_tmp5FB=((_tmp5F7->f1).ptr_atts).nullable;_tmp5FC=((_tmp5F7->f1).ptr_atts).bounds;_tmp5FD=((_tmp5F7->f1).ptr_atts).zero_term;}}_LL2B6:
# 2755
{void*_tmp5FE=Cyc_Tcutil_compress(_tmp5F8);void*_tmp5FF=_tmp5FE;struct Cyc_List_List*_tmp601;void*_tmp602;struct Cyc_Absyn_Tqual _tmp603;void*_tmp604;struct Cyc_List_List*_tmp605;int _tmp606;struct Cyc_Absyn_VarargInfo*_tmp607;struct Cyc_List_List*_tmp608;struct Cyc_List_List*_tmp609;struct Cyc_Absyn_Exp*_tmp60A;struct Cyc_List_List*_tmp60B;struct Cyc_Absyn_Exp*_tmp60C;struct Cyc_List_List*_tmp60D;_LL2BA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5FF;if(_tmp600->tag != 9)goto _LL2BC;else{_tmp601=(_tmp600->f1).tvars;_tmp602=(_tmp600->f1).effect;_tmp603=(_tmp600->f1).ret_tqual;_tmp604=(_tmp600->f1).ret_typ;_tmp605=(_tmp600->f1).args;_tmp606=(_tmp600->f1).c_varargs;_tmp607=(_tmp600->f1).cyc_varargs;_tmp608=(_tmp600->f1).rgn_po;_tmp609=(_tmp600->f1).attributes;_tmp60A=(_tmp600->f1).requires_clause;_tmp60B=(_tmp600->f1).requires_relns;_tmp60C=(_tmp600->f1).ensures_clause;_tmp60D=(_tmp600->f1).ensures_relns;}}_LL2BB:
# 2757
 if(_tmp601 != 0){
struct _RegionHandle _tmp60E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp60E;_push_region(rgn);
{struct _tuple12 _tmpA2B;struct _tuple12 _tmp60F=(_tmpA2B.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA2B.f2=rgn,_tmpA2B)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp60F,_tmp601);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2766
_tmp608=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp608);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp608);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA31;struct Cyc_Absyn_FnInfo _tmpA30;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA2F;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpA2F=_cycalloc(sizeof(*_tmpA2F)),((_tmpA2F[0]=((_tmpA31.tag=9,((_tmpA31.f1=((_tmpA30.tvars=0,((_tmpA30.effect=_tmp602,((_tmpA30.ret_tqual=_tmp603,((_tmpA30.ret_typ=_tmp604,((_tmpA30.args=_tmp605,((_tmpA30.c_varargs=_tmp606,((_tmpA30.cyc_varargs=_tmp607,((_tmpA30.rgn_po=0,((_tmpA30.attributes=_tmp609,((_tmpA30.requires_clause=_tmp60A,((_tmpA30.requires_relns=_tmp60B,((_tmpA30.ensures_clause=_tmp60C,((_tmpA30.ensures_relns=_tmp60D,_tmpA30)))))))))))))))))))))))))),_tmpA31)))),_tmpA2F)))));
# 2773
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA37;struct Cyc_Absyn_PtrInfo _tmpA36;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA35;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp610=(_tmpA35=_cycalloc(sizeof(*_tmpA35)),((_tmpA35[0]=((_tmpA37.tag=5,((_tmpA37.f1=((_tmpA36.elt_typ=ftyp,((_tmpA36.elt_tq=_tmp5F9,((_tmpA36.ptr_atts=(((_tmpA36.ptr_atts).rgn=_tmp5FA,(((_tmpA36.ptr_atts).nullable=_tmp5FB,(((_tmpA36.ptr_atts).bounds=_tmp5FC,(((_tmpA36.ptr_atts).zero_term=_tmp5FD,(((_tmpA36.ptr_atts).ptrloc=0,_tmpA36.ptr_atts)))))))))),_tmpA36)))))),_tmpA37)))),_tmpA35)));
# 2775
struct Cyc_Absyn_Exp*_tmp611=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA3A;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA39;e->r=(void*)((_tmpA39=_cycalloc(sizeof(*_tmpA39)),((_tmpA39[0]=((_tmpA3A.tag=12,((_tmpA3A.f1=_tmp611,((_tmpA3A.f2=ts,_tmpA3A)))))),_tmpA39))));}
e->topt=(void*)_tmp610;};}
# 2759
;_pop_region(rgn);}
# 2780
goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 2783
goto _LL2B4;_LL2B7:;_LL2B8:
 goto _LL2B4;_LL2B4:;}
# 2786
goto _LL2AF;_LL2AF:;}
# 2788
return(void*)_check_null(e->topt);}
# 2796
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2798
struct _RegionHandle _tmp61B=_new_region("r");struct _RegionHandle*r=& _tmp61B;_push_region(r);
{struct Cyc_List_List*_tmp61C=0;
# 2801
{void*_tmp61D=fn_exp->r;void*_tmp61E=_tmp61D;struct Cyc_List_List*_tmp620;_LL2BF: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp61F=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp61E;if(_tmp61F->tag != 9)goto _LL2C1;else{_tmp620=_tmp61F->f2;}}_LL2C0:
# 2803
{void*_tmp621=e->r;void*_tmp622=_tmp621;struct Cyc_List_List*_tmp624;_LL2C4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp623=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp622;if(_tmp623->tag != 9)goto _LL2C6;else{_tmp624=_tmp623->f2;}}_LL2C5: {
# 2805
struct Cyc_List_List*_tmp625=alias_arg_exps;
int _tmp626=0;
while(_tmp625 != 0){
while(_tmp626 != (int)_tmp625->hd){
if(_tmp620 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA44;struct Cyc_Int_pa_PrintArg_struct _tmpA43;void*_tmpA42[2];const char*_tmpA41;void*_tmpA40;(_tmpA40=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA44.tag=1,((_tmpA44.f1=(unsigned long)((int)_tmp625->hd),((_tmpA43.tag=1,((_tmpA43.f1=(unsigned long)_tmp626,((_tmpA42[0]=& _tmpA43,((_tmpA42[1]=& _tmpA44,Cyc_aprintf(((_tmpA41="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA41,sizeof(char),36))),_tag_dyneither(_tmpA42,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA40,sizeof(void*),0)));}
# 2812
++ _tmp626;
_tmp620=_tmp620->tl;
_tmp624=((struct Cyc_List_List*)_check_null(_tmp624))->tl;}{
# 2817
struct _tuple11 _tmp62C=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp620))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp624))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp62E;struct Cyc_Absyn_Exp*_tmp62F;struct _tuple11 _tmp62D=_tmp62C;_tmp62E=_tmp62D.f1;_tmp62F=_tmp62D.f2;
_tmp620->hd=(void*)_tmp62F;
{struct Cyc_List_List*_tmpA45;_tmp61C=((_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45->hd=_tmp62E,((_tmpA45->tl=_tmp61C,_tmpA45))))));}
_tmp625=_tmp625->tl;};}
# 2822
goto _LL2C3;}_LL2C6:;_LL2C7: {
const char*_tmpA48;void*_tmpA47;(_tmpA47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA48="not a function call!",_tag_dyneither(_tmpA48,sizeof(char),21))),_tag_dyneither(_tmpA47,sizeof(void*),0)));}_LL2C3:;}
# 2825
goto _LL2BE;_LL2C1:;_LL2C2: {
const char*_tmpA4B;void*_tmpA4A;(_tmpA4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA4B="not a function call!",_tag_dyneither(_tmpA4B,sizeof(char),21))),_tag_dyneither(_tmpA4A,sizeof(void*),0)));}_LL2BE:;}
# 2830
while(_tmp61C != 0){
struct Cyc_Absyn_Decl*_tmp635=(struct Cyc_Absyn_Decl*)_tmp61C->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp635,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp61C=_tmp61C->tl;}
# 2837
e->topt=0;
e->r=fn_exp->r;}
# 2799
;_pop_region(r);}
# 2844
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2848
{void*_tmp636=e->r;void*_tmp637=_tmp636;struct Cyc_Absyn_Exp*_tmp639;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_List_List*_tmp63C;struct Cyc_Absyn_VarargCallInfo**_tmp63D;int*_tmp63E;struct Cyc_Core_Opt*_tmp640;struct Cyc_List_List*_tmp641;union Cyc_Absyn_Cnst*_tmp643;struct _tuple1*_tmp645;void**_tmp646;enum Cyc_Absyn_Primop _tmp648;struct Cyc_List_List*_tmp649;struct Cyc_Absyn_Exp*_tmp64B;enum Cyc_Absyn_Incrementor _tmp64C;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Core_Opt*_tmp64F;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Exp*_tmp654;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Absyn_Exp*_tmp65D;struct Cyc_Absyn_Exp*_tmp65F;struct Cyc_List_List*_tmp660;struct Cyc_Absyn_VarargCallInfo**_tmp661;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp665;struct Cyc_List_List*_tmp666;void*_tmp668;struct Cyc_Absyn_Exp*_tmp669;enum Cyc_Absyn_Coercion*_tmp66A;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp**_tmp66E;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp671;void*_tmp673;void*_tmp675;struct Cyc_List_List*_tmp676;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp67A;struct _dyneither_ptr*_tmp67B;int*_tmp67C;int*_tmp67D;struct Cyc_Absyn_Exp*_tmp67F;struct _dyneither_ptr*_tmp680;int*_tmp681;int*_tmp682;struct Cyc_Absyn_Exp*_tmp684;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_List_List*_tmp687;struct _tuple9*_tmp689;struct Cyc_List_List*_tmp68A;struct Cyc_List_List*_tmp68C;struct Cyc_Absyn_Stmt*_tmp68E;struct Cyc_Absyn_Vardecl*_tmp690;struct Cyc_Absyn_Exp*_tmp691;struct Cyc_Absyn_Exp*_tmp692;int*_tmp693;struct Cyc_Absyn_Exp*_tmp695;void*_tmp696;int*_tmp697;struct _tuple1**_tmp699;struct Cyc_List_List**_tmp69A;struct Cyc_List_List*_tmp69B;struct Cyc_Absyn_Aggrdecl**_tmp69C;void*_tmp69E;struct Cyc_List_List*_tmp69F;struct Cyc_List_List*_tmp6A1;struct Cyc_Absyn_Datatypedecl*_tmp6A2;struct Cyc_Absyn_Datatypefield*_tmp6A3;struct _tuple1**_tmp6A5;struct Cyc_Absyn_Enumdecl*_tmp6A6;struct Cyc_Absyn_Enumfield*_tmp6A7;struct _tuple1**_tmp6A9;void*_tmp6AA;struct Cyc_Absyn_Enumfield*_tmp6AB;int*_tmp6AD;struct Cyc_Absyn_Exp**_tmp6AE;void***_tmp6AF;struct Cyc_Absyn_Exp**_tmp6B0;int*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B4;struct Cyc_Absyn_Exp*_tmp6B6;struct _dyneither_ptr*_tmp6B7;void*_tmp6B9;_LL2C9: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp638=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp638->tag != 11)goto _LL2CB;else{_tmp639=_tmp638->f1;}}_LL2CA:
# 2853
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp639);
return;_LL2CB:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp63A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp63A->tag != 9)goto _LL2CD;else{_tmp63B=_tmp63A->f1;_tmp63C=_tmp63A->f2;_tmp63D=(struct Cyc_Absyn_VarargCallInfo**)& _tmp63A->f3;_tmp63E=(int*)& _tmp63A->f4;}}if(!(!(*_tmp63E)))goto _LL2CD;_LL2CC:
# 2859
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp63B,_tmp63C)){
*_tmp63E=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2863
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2866
return;_LL2CD: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp63F=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp63F->tag != 35)goto _LL2CF;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL2CE:
# 2870
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp641);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2CF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp642->tag != 0)goto _LL2D1;else{_tmp643=(union Cyc_Absyn_Cnst*)& _tmp642->f1;}}_LL2D0:
# 2875
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp643,e);goto _LL2C8;_LL2D1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp644=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp644->tag != 1)goto _LL2D3;else{_tmp645=_tmp644->f1;_tmp646=(void**)((void**)& _tmp644->f2);}}_LL2D2:
# 2877
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp645,_tmp646);goto _LL2C8;_LL2D3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp647=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp647->tag != 2)goto _LL2D5;else{_tmp648=_tmp647->f1;_tmp649=_tmp647->f2;}}_LL2D4:
# 2879
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp648,_tmp649);goto _LL2C8;_LL2D5: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp64A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp64A->tag != 4)goto _LL2D7;else{_tmp64B=_tmp64A->f1;_tmp64C=_tmp64A->f2;}}_LL2D6:
# 2881
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp64B,_tmp64C);goto _LL2C8;_LL2D7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp64D->tag != 3)goto _LL2D9;else{_tmp64E=_tmp64D->f1;_tmp64F=_tmp64D->f2;_tmp650=_tmp64D->f3;}}_LL2D8:
# 2883
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp64E,_tmp64F,_tmp650);goto _LL2C8;_LL2D9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp651->tag != 5)goto _LL2DB;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;_tmp654=_tmp651->f3;}}_LL2DA:
# 2885
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp652,_tmp653,_tmp654);goto _LL2C8;_LL2DB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp655=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp655->tag != 6)goto _LL2DD;else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;}}_LL2DC:
# 2887
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp656,_tmp657);goto _LL2C8;_LL2DD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp658=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp658->tag != 7)goto _LL2DF;else{_tmp659=_tmp658->f1;_tmp65A=_tmp658->f2;}}_LL2DE:
# 2889
 t=Cyc_Tcexp_tcOr(te,loc,_tmp659,_tmp65A);goto _LL2C8;_LL2DF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp65B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp65B->tag != 8)goto _LL2E1;else{_tmp65C=_tmp65B->f1;_tmp65D=_tmp65B->f2;}}_LL2E0:
# 2891
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp65C,_tmp65D);goto _LL2C8;_LL2E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp65E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp65E->tag != 9)goto _LL2E3;else{_tmp65F=_tmp65E->f1;_tmp660=_tmp65E->f2;_tmp661=(struct Cyc_Absyn_VarargCallInfo**)& _tmp65E->f3;}}_LL2E2: {
# 2897
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp6BB;_push_handler(& _tmp6BB);{int _tmp6BD=0;if(setjmp(_tmp6BB.handler))_tmp6BD=1;if(!_tmp6BD){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp6BC=(void*)_exn_thrown;void*_tmp6BF=_tmp6BC;struct _dyneither_ptr _tmp6C1;void*_tmp6C2;_LL31C: {struct Cyc_Core_Failure_exn_struct*_tmp6C0=(struct Cyc_Core_Failure_exn_struct*)_tmp6BF;if(_tmp6C0->tag != Cyc_Core_Failure)goto _LL31E;else{_tmp6C1=_tmp6C0->f1;}}_LL31D:
# 2904
 ok=0;
fn_exp=e;
goto _LL31B;_LL31E: _tmp6C2=_tmp6BF;_LL31F:(void)_rethrow(_tmp6C2);_LL31B:;}};}
# 2908
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp65F,_tmp660,_tmp661,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2915
goto _LL2C8;}_LL2E3: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp662=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp662->tag != 10)goto _LL2E5;else{_tmp663=_tmp662->f1;}}_LL2E4:
# 2917
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp663);goto _LL2C8;_LL2E5: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp664->tag != 12)goto _LL2E7;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}}_LL2E6:
# 2919
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp665,_tmp666);goto _LL2C8;_LL2E7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp667=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp667->tag != 13)goto _LL2E9;else{_tmp668=(void*)_tmp667->f1;_tmp669=_tmp667->f2;_tmp66A=(enum Cyc_Absyn_Coercion*)& _tmp667->f4;}}_LL2E8:
# 2921
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp668,_tmp669,_tmp66A);goto _LL2C8;_LL2E9: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp66B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp66B->tag != 14)goto _LL2EB;else{_tmp66C=_tmp66B->f1;}}_LL2EA:
# 2923
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp66C);goto _LL2C8;_LL2EB: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp66D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp66D->tag != 15)goto _LL2ED;else{_tmp66E=(struct Cyc_Absyn_Exp**)& _tmp66D->f1;_tmp66F=_tmp66D->f2;}}_LL2EC:
# 2925
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp66E,e,_tmp66F);goto _LL2C8;_LL2ED: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp670=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp670->tag != 17)goto _LL2EF;else{_tmp671=_tmp670->f1;}}_LL2EE: {
# 2927
void*_tmp6C3=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp671);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6C3);goto _LL2C8;}_LL2EF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp672->tag != 16)goto _LL2F1;else{_tmp673=(void*)_tmp672->f1;}}_LL2F0:
# 2930
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp673);goto _LL2C8;_LL2F1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp674=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp674->tag != 18)goto _LL2F3;else{_tmp675=(void*)_tmp674->f1;_tmp676=_tmp674->f2;}}_LL2F2:
# 2932
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp675,_tmp676);goto _LL2C8;_LL2F3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp677->tag != 19)goto _LL2F5;else{_tmp678=_tmp677->f1;}}_LL2F4:
# 2934
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp678);goto _LL2C8;_LL2F5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp679=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp679->tag != 20)goto _LL2F7;else{_tmp67A=_tmp679->f1;_tmp67B=_tmp679->f2;_tmp67C=(int*)& _tmp679->f3;_tmp67D=(int*)& _tmp679->f4;}}_LL2F6:
# 2936
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp67A,_tmp67B,_tmp67C,_tmp67D);goto _LL2C8;_LL2F7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp67E=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp67E->tag != 21)goto _LL2F9;else{_tmp67F=_tmp67E->f1;_tmp680=_tmp67E->f2;_tmp681=(int*)& _tmp67E->f3;_tmp682=(int*)& _tmp67E->f4;}}_LL2F8:
# 2938
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp67F,_tmp680,_tmp681,_tmp682);goto _LL2C8;_LL2F9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp683=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp683->tag != 22)goto _LL2FB;else{_tmp684=_tmp683->f1;_tmp685=_tmp683->f2;}}_LL2FA:
# 2940
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp684,_tmp685);goto _LL2C8;_LL2FB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp686=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp686->tag != 23)goto _LL2FD;else{_tmp687=_tmp686->f1;}}_LL2FC:
# 2942
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp687);goto _LL2C8;_LL2FD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp688=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp688->tag != 24)goto _LL2FF;else{_tmp689=_tmp688->f1;_tmp68A=_tmp688->f2;}}_LL2FE:
# 2944
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp689,_tmp68A);goto _LL2C8;_LL2FF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp68B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp68B->tag != 25)goto _LL301;else{_tmp68C=_tmp68B->f1;}}_LL300: {
# 2948
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp6C4=Cyc_Tcutil_compress(*topt);void*_tmp6C5=_tmp6C4;void**_tmp6C7;struct Cyc_Absyn_Tqual*_tmp6C8;union Cyc_Absyn_Constraint*_tmp6C9;_LL321: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6C6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C5;if(_tmp6C6->tag != 8)goto _LL323;else{_tmp6C7=(void**)&(_tmp6C6->f1).elt_type;_tmp6C8=(struct Cyc_Absyn_Tqual*)&(_tmp6C6->f1).tq;_tmp6C9=(_tmp6C6->f1).zero_term;}}_LL322:
# 2954
 elt_topt=_tmp6C7;
elt_tqopt=_tmp6C8;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6C9);
goto _LL320;_LL323:;_LL324:
 goto _LL320;_LL320:;}
# 2961
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp68C);goto _LL2C8;}_LL301: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp68D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp68D->tag != 36)goto _LL303;else{_tmp68E=_tmp68D->f1;}}_LL302:
# 2963
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp68E);goto _LL2C8;_LL303: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp68F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp68F->tag != 26)goto _LL305;else{_tmp690=_tmp68F->f1;_tmp691=_tmp68F->f2;_tmp692=_tmp68F->f3;_tmp693=(int*)& _tmp68F->f4;}}_LL304:
# 2965
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp690,_tmp691,_tmp692,_tmp693);goto _LL2C8;_LL305: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp694=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp694->tag != 27)goto _LL307;else{_tmp695=_tmp694->f1;_tmp696=(void*)_tmp694->f2;_tmp697=(int*)& _tmp694->f3;}}_LL306:
# 2967
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp695,_tmp696,_tmp697);goto _LL2C8;_LL307: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp698=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp698->tag != 28)goto _LL309;else{_tmp699=(struct _tuple1**)& _tmp698->f1;_tmp69A=(struct Cyc_List_List**)& _tmp698->f2;_tmp69B=_tmp698->f3;_tmp69C=(struct Cyc_Absyn_Aggrdecl**)& _tmp698->f4;}}_LL308:
# 2969
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp699,_tmp69A,_tmp69B,_tmp69C);goto _LL2C8;_LL309: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp69D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp69D->tag != 29)goto _LL30B;else{_tmp69E=(void*)_tmp69D->f1;_tmp69F=_tmp69D->f2;}}_LL30A:
# 2971
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp69E,_tmp69F);goto _LL2C8;_LL30B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6A0=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6A0->tag != 30)goto _LL30D;else{_tmp6A1=_tmp6A0->f1;_tmp6A2=_tmp6A0->f2;_tmp6A3=_tmp6A0->f3;}}_LL30C:
# 2973
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp6A1,_tmp6A2,_tmp6A3);goto _LL2C8;_LL30D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp6A4=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6A4->tag != 31)goto _LL30F;else{_tmp6A5=(struct _tuple1**)& _tmp6A4->f1;_tmp6A6=_tmp6A4->f2;_tmp6A7=_tmp6A4->f3;}}_LL30E:
# 2975
*_tmp6A5=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6A7))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA4E;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA4D;t=(void*)((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=((_tmpA4E.tag=13,((_tmpA4E.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp6A6))->name,((_tmpA4E.f2=_tmp6A6,_tmpA4E)))))),_tmpA4D))));}goto _LL2C8;_LL30F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6A8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6A8->tag != 32)goto _LL311;else{_tmp6A9=(struct _tuple1**)& _tmp6A8->f1;_tmp6AA=(void*)_tmp6A8->f2;_tmp6AB=_tmp6A8->f3;}}_LL310:
# 2978
*_tmp6A9=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp6AB))->name;
t=_tmp6AA;goto _LL2C8;_LL311: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp6AC=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6AC->tag != 33)goto _LL313;else{_tmp6AD=(int*)&(_tmp6AC->f1).is_calloc;_tmp6AE=(struct Cyc_Absyn_Exp**)&(_tmp6AC->f1).rgn;_tmp6AF=(void***)&(_tmp6AC->f1).elt_type;_tmp6B0=(struct Cyc_Absyn_Exp**)&(_tmp6AC->f1).num_elts;_tmp6B1=(int*)&(_tmp6AC->f1).fat_result;}}_LL312:
# 2981
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6AE,_tmp6AF,_tmp6B0,_tmp6AD,_tmp6B1);goto _LL2C8;_LL313: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6B2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6B2->tag != 34)goto _LL315;else{_tmp6B3=_tmp6B2->f1;_tmp6B4=_tmp6B2->f2;}}_LL314:
# 2983
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6B3,_tmp6B4);goto _LL2C8;_LL315: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6B5=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6B5->tag != 37)goto _LL317;else{_tmp6B6=_tmp6B5->f1;_tmp6B7=_tmp6B5->f2;}}_LL316:
# 2985
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6B6,_tmp6B7);goto _LL2C8;_LL317: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6B8=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6B8->tag != 38)goto _LL319;else{_tmp6B9=(void*)_tmp6B8->f1;}}_LL318:
# 2987
 if(!te->allow_valueof){
const char*_tmpA51;void*_tmpA50;(_tmpA50=0,Cyc_Tcutil_terr(e->loc,((_tmpA51="valueof(-) can only occur within types",_tag_dyneither(_tmpA51,sizeof(char),39))),_tag_dyneither(_tmpA50,sizeof(void*),0)));}
# 2995
t=Cyc_Absyn_sint_typ;
goto _LL2C8;_LL319: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6BA=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp637;if(_tmp6BA->tag != 39)goto _LL2C8;}_LL31A:
# 2998
{const char*_tmpA54;void*_tmpA53;(_tmpA53=0,Cyc_Tcutil_terr(e->loc,((_tmpA54="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA54,sizeof(char),50))),_tag_dyneither(_tmpA53,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2C8:;}
# 3001
e->topt=t;}
