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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 121 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
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
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 215 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 225
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 232
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 288
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 64
void Cyc_Position_post_error(struct Cyc_Position_Error*);
# 80 "absyn.h"
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 92
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 96
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 104
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
# 155
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 176
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 181
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 188
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 208
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 210
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 219
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 433 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 909
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 1162
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1166
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 134 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 71
void*Cyc_Tcutil_compress(void*t);
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 284 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 325
int Cyc_Tcutil_bits_only(void*t);
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
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 100
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 102
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 104
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;char Cyc_CfFlowInfo_IsZero[7]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 81
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 87
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();
# 92
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 100
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 121
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 123
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc);
# 125
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2);
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r);
# 166
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 169
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 172
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple12{void*f1;struct Cyc_List_List*f2;};
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 180
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 185
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 206 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 210
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple13{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2);
# 221
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 226
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233
extern struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val;
extern struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val;
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 237
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t);
# 239
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 36 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 40
Cyc_CfFlowInfo_anal_error=1;
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 53 "cf_flowinfo.cyc"
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_UniqueRgn_k_val={0};
struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct Cyc_CfFlowInfo_RefCntRgn_k_val={1};
# 57
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
union Cyc_CfFlowInfo_AbsLVal _tmp3B5;return((_tmp3B5.PlaceL).val=x,(((_tmp3B5.PlaceL).tag=1,_tmp3B5)));}
# 60
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
union Cyc_CfFlowInfo_AbsLVal _tmp3B6;return((_tmp3B6.UnknownL).val=0,(((_tmp3B6.UnknownL).tag=2,_tmp3B6)));}
# 63
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){union Cyc_CfFlowInfo_FlowInfo _tmp3B7;return((_tmp3B7.BottomFL).val=0,(((_tmp3B7.BottomFL).tag=1,_tmp3B7)));}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
struct _tuple11 _tmp3BA;union Cyc_CfFlowInfo_FlowInfo _tmp3B9;return((_tmp3B9.ReachableFL).val=((_tmp3BA.f1=fd,((_tmp3BA.f2=r,_tmp3BA)))),(((_tmp3B9.ReachableFL).tag=2,_tmp3B9)));}
# 69
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2);
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2);
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(struct _RegionHandle*r){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_CfFlowInfo_Place*_tmp3F1;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp3F0;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3EF;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3EE;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3ED;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3EC;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3EB;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp3EA;struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct _tmp3E9;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3E8;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3E7;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3E6;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3E5;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3E4;struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct _tmp3E3;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3E2;struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct _tmp3E1;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3E0;struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct _tmp3DF;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3DE;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct _tmp3DD;struct Cyc_CfFlowInfo_FlowEnv*_tmp3DC;return(_tmp3DC=_region_malloc(r,sizeof(*_tmp3DC)),((_tmp3DC->r=r,((_tmp3DC->zero=(void*)(
# 76
(_tmp3DE=_region_malloc(r,sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3DD.tag=0,_tmp3DD)),_tmp3DE)))),((_tmp3DC->notzeroall=(void*)(
(_tmp3E0=_region_malloc(r,sizeof(*_tmp3E0)),((_tmp3E0[0]=((_tmp3DF.tag=1,_tmp3DF)),_tmp3E0)))),((_tmp3DC->notzerothis=(void*)(
(_tmp3E2=_region_malloc(r,sizeof(*_tmp3E2)),((_tmp3E2[0]=((_tmp3E1.tag=2,_tmp3E1)),_tmp3E2)))),((_tmp3DC->unknown_none=(void*)(
(_tmp3E4=_region_malloc(r,sizeof(*_tmp3E4)),((_tmp3E4[0]=((_tmp3E3.tag=3,((_tmp3E3.f1=Cyc_CfFlowInfo_NoneIL,_tmp3E3)))),_tmp3E4)))),((_tmp3DC->unknown_this=(void*)(
(_tmp3E6=_region_malloc(r,sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E5.tag=3,((_tmp3E5.f1=Cyc_CfFlowInfo_ThisIL,_tmp3E5)))),_tmp3E6)))),((_tmp3DC->unknown_all=(void*)(
(_tmp3E8=_region_malloc(r,sizeof(*_tmp3E8)),((_tmp3E8[0]=((_tmp3E7.tag=3,((_tmp3E7.f1=Cyc_CfFlowInfo_AllIL,_tmp3E7)))),_tmp3E8)))),((_tmp3DC->esc_none=(void*)(
(_tmp3EA=_region_malloc(r,sizeof(*_tmp3EA)),((_tmp3EA[0]=((_tmp3E9.tag=4,((_tmp3E9.f1=Cyc_CfFlowInfo_NoneIL,_tmp3E9)))),_tmp3EA)))),((_tmp3DC->esc_this=(void*)(
(_tmp3EC=_region_malloc(r,sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3EB.tag=4,((_tmp3EB.f1=Cyc_CfFlowInfo_ThisIL,_tmp3EB)))),_tmp3EC)))),((_tmp3DC->esc_all=(void*)(
(_tmp3EE=_region_malloc(r,sizeof(*_tmp3EE)),((_tmp3EE[0]=((_tmp3ED.tag=4,((_tmp3ED.f1=Cyc_CfFlowInfo_AllIL,_tmp3ED)))),_tmp3EE)))),((_tmp3DC->mt_flowdict=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(void*,void*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_root_cmp),((_tmp3DC->mt_place_set=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*)))Cyc_Dict_rempty)(r,Cyc_CfFlowInfo_place_cmp),((_tmp3DC->dummy_place=(
(_tmp3F1=_region_malloc(r,sizeof(*_tmp3F1)),((_tmp3F1->root=(void*)((_tmp3EF=_region_malloc(r,sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3F0.tag=1,((_tmp3F0.f1=& dummy_exp,((_tmp3F0.f2=(void*)& Cyc_Absyn_VoidType_val,_tmp3F0)))))),_tmp3EF)))),((_tmp3F1->fields=0,_tmp3F1)))))),_tmp3DC)))))))))))))))))))))))))));}
# 93
int Cyc_CfFlowInfo_update_place_set(struct Cyc_Dict_Dict*set,void*place,unsigned int loc){
if(set != 0){
# 96
if(((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(*set,place))
return 1;else{
# 99
*set=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,unsigned int v))Cyc_Dict_insert)(*set,place,loc);}}
# 101
return 0;}
# 105
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp1F=*place;void*_tmp21;struct Cyc_List_List*_tmp22;struct Cyc_CfFlowInfo_Place _tmp20=_tmp1F;_tmp21=_tmp20.root;_tmp22=_tmp20.fields;{
void*_tmp23=_tmp21;struct Cyc_Absyn_Vardecl*_tmp25;_LL1: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp24=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp23;if(_tmp24->tag != 0)goto _LL3;else{_tmp25=_tmp24->f1;}}_LL2:
# 109
 if(_tmp22 == 0)
return Cyc_Absynpp_qvar2string(_tmp25->name);else{
# 112
const char*_tmp3F5;void*_tmp3F4[1];struct Cyc_String_pa_PrintArg_struct _tmp3F3;return(struct _dyneither_ptr)((_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp25->name)),((_tmp3F4[0]=& _tmp3F3,Cyc_aprintf(((_tmp3F5="reachable from %s",_tag_dyneither(_tmp3F5,sizeof(char),18))),_tag_dyneither(_tmp3F4,sizeof(void*),1))))))));}_LL3:;_LL4: {
const char*_tmp3F8;void*_tmp3F7;(_tmp3F7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3F8="error locations not for VarRoots",_tag_dyneither(_tmp3F8,sizeof(char),33))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}_LL0:;};}
# 118
static unsigned int Cyc_CfFlowInfo_combine_consume_f(void*env,void*place,unsigned int loc1,unsigned int loc2){
# 122
return loc1;}
# 127
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint){
# 134
int _tmp2B=0;
struct Cyc_Dict_Dict _tmp2C=((struct Cyc_Dict_Dict(*)(unsigned int(*f)(int*,struct Cyc_CfFlowInfo_Place*,unsigned int,unsigned int),int*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)((unsigned int(*)(int*env,struct Cyc_CfFlowInfo_Place*place,unsigned int loc1,unsigned int loc2))Cyc_CfFlowInfo_combine_consume_f,& _tmp2B,s1,s2);
# 139
return _tmp2C;}struct _tuple14{void*f1;unsigned int f2;};
# 142
int Cyc_CfFlowInfo_place_set_subset(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
if((int)s1.t == (int)s2.t)return 1;{
struct _handler_cons _tmp2D;_push_handler(& _tmp2D);{int _tmp2F=0;if(setjmp(_tmp2D.handler))_tmp2F=1;if(!_tmp2F){
{struct _RegionHandle _tmp30=_new_region("temp");struct _RegionHandle*temp=& _tmp30;_push_region(temp);
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(temp,s1);
struct _tuple14 _tmp31=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(temp,s1);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp31)){
void*_tmp32=_tmp31.f1;
if(!((int(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_member)(s2,_tmp32)){
# 154
int _tmp33=0;_npop_handler(1);return _tmp33;}}{
# 157
int _tmp34=1;_npop_handler(1);return _tmp34;};}
# 146
;_pop_region(temp);}
# 145
;_pop_handler();}else{void*_tmp2E=(void*)_exn_thrown;void*_tmp36=_tmp2E;void*_tmp37;void*_tmp38;_LL6: _tmp37=_tmp36;_LL7:
# 161
 return 1;_LL8: _tmp38=_tmp36;_LL9:(void)_throw(_tmp38);_LL5:;}};};}
# 165
int Cyc_CfFlowInfo_place_set_equals(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2){
return Cyc_CfFlowInfo_place_set_subset(s1,s2) && Cyc_CfFlowInfo_place_set_subset(s2,s1);}
# 170
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp39=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp39,f)== 0)return n;
n=n + 1;}{
# 177
struct Cyc_Core_Impossible_exn_struct _tmp405;const char*_tmp404;void*_tmp403[1];struct Cyc_String_pa_PrintArg_struct _tmp402;struct Cyc_Core_Impossible_exn_struct*_tmp401;(int)_throw((void*)((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401[0]=((_tmp405.tag=Cyc_Core_Impossible,((_tmp405.f1=(struct _dyneither_ptr)((_tmp402.tag=0,((_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp403[0]=& _tmp402,Cyc_aprintf(((_tmp404="get_field_index_fs failed: %s",_tag_dyneither(_tmp404,sizeof(char),30))),_tag_dyneither(_tmp403,sizeof(void*),1)))))))),_tmp405)))),_tmp401)))));};}
# 180
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp3F=Cyc_Tcutil_compress(t);void*_tmp40=_tmp3F;union Cyc_Absyn_AggrInfoU _tmp42;struct Cyc_List_List*_tmp44;_LLB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 11)goto _LLD;else{_tmp42=(_tmp41->f1).aggr_info;}}_LLC: {
# 183
struct Cyc_List_List*_tmp45=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp42))->impl))->fields;
_tmp44=_tmp45;goto _LLE;}_LLD: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp43=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp40;if(_tmp43->tag != 12)goto _LLF;else{_tmp44=_tmp43->f2;}}_LLE:
# 186
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp44,f);_LLF:;_LL10: {
# 188
struct Cyc_Core_Impossible_exn_struct _tmp412;const char*_tmp411;void*_tmp410[1];struct Cyc_String_pa_PrintArg_struct _tmp40F;struct Cyc_Core_Impossible_exn_struct*_tmp40E;(int)_throw((void*)((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=((_tmp412.tag=Cyc_Core_Impossible,((_tmp412.f1=(struct _dyneither_ptr)((_tmp40F.tag=0,((_tmp40F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp410[0]=& _tmp40F,Cyc_aprintf(((_tmp411="get_field_index failed: %s",_tag_dyneither(_tmp411,sizeof(char),27))),_tag_dyneither(_tmp410,sizeof(void*),1)))))))),_tmp412)))),_tmp40E)))));}_LLA:;}
# 193
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple0 _tmp413;struct _tuple0 _tmp4B=(_tmp413.f1=r1,((_tmp413.f2=r2,_tmp413)));struct _tuple0 _tmp4C=_tmp4B;struct Cyc_Absyn_Vardecl*_tmp4E;struct Cyc_Absyn_Vardecl*_tmp50;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp56;int _tmp5A;int _tmp5C;_LL12:{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4D=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C.f1;if(_tmp4D->tag != 0)goto _LL14;else{_tmp4E=_tmp4D->f1;}}{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp4F=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C.f2;if(_tmp4F->tag != 0)goto _LL14;else{_tmp50=_tmp4F->f1;}};_LL13:
 return(int)_tmp4E - (int)_tmp50;_LL14: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp51=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C.f1;if(_tmp51->tag != 0)goto _LL16;}_LL15:
 return - 1;_LL16: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp52=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp4C.f2;if(_tmp52->tag != 0)goto _LL18;}_LL17:
 return 1;_LL18:{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp53=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4C.f1;if(_tmp53->tag != 1)goto _LL1A;else{_tmp54=_tmp53->f1;}}{struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp55=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4C.f2;if(_tmp55->tag != 1)goto _LL1A;else{_tmp56=_tmp55->f1;}};_LL19:
 return(int)_tmp54 - (int)_tmp56;_LL1A: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp57=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4C.f1;if(_tmp57->tag != 1)goto _LL1C;}_LL1B:
 return - 1;_LL1C: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp58=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp4C.f2;if(_tmp58->tag != 1)goto _LL1E;}_LL1D:
 return 1;_LL1E:{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp59=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4C.f1;if(_tmp59->tag != 2)goto _LL11;else{_tmp5A=_tmp59->f1;}}{struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp5B=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp4C.f2;if(_tmp5B->tag != 2)goto _LL11;else{_tmp5C=_tmp5B->f1;}};_LL1F:
 return _tmp5A - _tmp5C;_LL11:;};}
# 207
static int Cyc_CfFlowInfo_intlist_cmp(struct Cyc_List_List*list1,struct Cyc_List_List*list2){
# 209
return((int(*)(int(*cmp)(int,int),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Core_intcmp,list1,list2);}
# 212
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp5E=*p1;void*_tmp60;struct Cyc_List_List*_tmp61;struct Cyc_CfFlowInfo_Place _tmp5F=_tmp5E;_tmp60=_tmp5F.root;_tmp61=_tmp5F.fields;{
struct Cyc_CfFlowInfo_Place _tmp62=*p2;void*_tmp64;struct Cyc_List_List*_tmp65;struct Cyc_CfFlowInfo_Place _tmp63=_tmp62;_tmp64=_tmp63.root;_tmp65=_tmp63.fields;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp60,_tmp64);
if(i != 0)
return i;
return Cyc_CfFlowInfo_intlist_cmp(_tmp61,_tmp65);};};};}
# 224
static struct _dyneither_ptr*Cyc_CfFlowInfo_place2string(struct Cyc_CfFlowInfo_Place*p){
struct Cyc_List_List*sl=0;
struct Cyc_CfFlowInfo_Place _tmp66=*p;void*_tmp68;struct Cyc_List_List*_tmp69;struct Cyc_CfFlowInfo_Place _tmp67=_tmp66;_tmp68=_tmp67.root;_tmp69=_tmp67.fields;
{void*_tmp6A=_tmp68;struct Cyc_Absyn_Vardecl*_tmp6C;struct Cyc_Absyn_Exp*_tmp6E;int _tmp70;_LL21: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6B=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp6A;if(_tmp6B->tag != 0)goto _LL23;else{_tmp6C=_tmp6B->f1;}}_LL22:
{struct _dyneither_ptr*_tmp420;const char*_tmp41F;void*_tmp41E[1];struct Cyc_String_pa_PrintArg_struct _tmp41D;struct Cyc_List_List*_tmp41C;sl=((_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->hd=((_tmp420=_cycalloc(sizeof(*_tmp420)),((_tmp420[0]=((_tmp41D.tag=0,((_tmp41D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C->name).f2),((_tmp41E[0]=& _tmp41D,Cyc_aprintf(((_tmp41F="%s",_tag_dyneither(_tmp41F,sizeof(char),3))),_tag_dyneither(_tmp41E,sizeof(void*),1)))))))),_tmp420)))),((_tmp41C->tl=sl,_tmp41C))))));}goto _LL20;_LL23: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6D=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp6A;if(_tmp6D->tag != 1)goto _LL25;else{_tmp6E=_tmp6D->f1;}}_LL24:
{struct _dyneither_ptr*_tmp42D;const char*_tmp42C;void*_tmp42B[1];struct Cyc_Int_pa_PrintArg_struct _tmp42A;struct Cyc_List_List*_tmp429;sl=((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429->hd=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D[0]=((_tmp42A.tag=1,((_tmp42A.f1=(unsigned long)((int)_tmp6E),((_tmp42B[0]=& _tmp42A,Cyc_aprintf(((_tmp42C="mpt%d",_tag_dyneither(_tmp42C,sizeof(char),6))),_tag_dyneither(_tmp42B,sizeof(void*),1)))))))),_tmp42D)))),((_tmp429->tl=sl,_tmp429))))));}goto _LL20;_LL25: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6F=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp6A;if(_tmp6F->tag != 2)goto _LL20;else{_tmp70=_tmp6F->f1;}}_LL26:
{struct _dyneither_ptr*_tmp43A;const char*_tmp439;void*_tmp438[1];struct Cyc_Int_pa_PrintArg_struct _tmp437;struct Cyc_List_List*_tmp436;sl=((_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436->hd=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=((_tmp437.tag=1,((_tmp437.f1=(unsigned long)_tmp70,((_tmp438[0]=& _tmp437,Cyc_aprintf(((_tmp439="param%d",_tag_dyneither(_tmp439,sizeof(char),8))),_tag_dyneither(_tmp438,sizeof(void*),1)))))))),_tmp43A)))),((_tmp436->tl=sl,_tmp436))))));}goto _LL20;_LL20:;}
# 232
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
struct _dyneither_ptr*_tmp447;const char*_tmp446;void*_tmp445[1];struct Cyc_Int_pa_PrintArg_struct _tmp444;struct Cyc_List_List*_tmp443;sl=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->hd=((_tmp447=_cycalloc(sizeof(*_tmp447)),((_tmp447[0]=((_tmp444.tag=1,((_tmp444.f1=(unsigned long)((int)_tmp69->hd),((_tmp445[0]=& _tmp444,Cyc_aprintf(((_tmp446="%d",_tag_dyneither(_tmp446,sizeof(char),3))),_tag_dyneither(_tmp445,sizeof(void*),1)))))))),_tmp447)))),((_tmp443->tl=sl,_tmp443))))));}{
struct Cyc_String_pa_PrintArg_struct _tmp452;const char*_tmp451;void*_tmp450[1];const char*_tmp44F;struct _dyneither_ptr*_tmp44E;struct _dyneither_ptr*_tmp85=(_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E[0]=((_tmp452.tag=0,((_tmp452.f1=(struct _dyneither_ptr)((_tmp451="",_tag_dyneither(_tmp451,sizeof(char),1))),((_tmp450[0]=& _tmp452,Cyc_aprintf(((_tmp44F="%s",_tag_dyneither(_tmp44F,sizeof(char),3))),_tag_dyneither(_tmp450,sizeof(void*),1)))))))),_tmp44E)));
for(0;sl != 0;sl=sl->tl){
const char*_tmp457;void*_tmp456[2];struct Cyc_String_pa_PrintArg_struct _tmp455;struct Cyc_String_pa_PrintArg_struct _tmp454;*_tmp85=((_tmp454.tag=0,((_tmp454.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp85),((_tmp455.tag=0,((_tmp455.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)sl->hd)),((_tmp456[0]=& _tmp455,((_tmp456[1]=& _tmp454,Cyc_aprintf(((_tmp457="%s.%s",_tag_dyneither(_tmp457,sizeof(char),6))),_tag_dyneither(_tmp456,sizeof(void*),2))))))))))))));}
return _tmp85;};}
# 250 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval);static void _tmp45D(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp45C,unsigned int*_tmp45B,void***_tmp459){for(*_tmp45C=0;*_tmp45C < *_tmp45B;(*_tmp45C)++){(*_tmp459)[*_tmp45C]=(void*)(*fenv)->unknown_all;}}
# 255
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 261
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
unsigned int _tmp45C;unsigned int _tmp45B;struct _dyneither_ptr _tmp45A;void**_tmp459;unsigned int _tmp458;struct _dyneither_ptr d=(_tmp458=sz,((_tmp459=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp458)),((_tmp45A=_tag_dyneither(_tmp459,sizeof(void*),_tmp458),((((_tmp45B=_tmp458,_tmp45D(& fenv,& _tmp45C,& _tmp45B,& _tmp459))),_tmp45A)))))));
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp8F=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct _dyneither_ptr*_tmp91;void*_tmp92;struct Cyc_Absyn_Aggrfield*_tmp90=_tmp8F;_tmp91=_tmp90->name;_tmp92=_tmp90->type;
if(_get_dyneither_size(*_tmp91,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp92,leafval);}}
# 269
return d;}struct _tuple15{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 273
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple15*env,struct Cyc_Absyn_Aggrfield*f){
# 277
struct _RegionHandle*_tmp98;struct Cyc_List_List*_tmp99;struct _tuple15*_tmp97=env;_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;{
void*_tmp9A=Cyc_Tcutil_rsubstitute(_tmp98,_tmp99,f->type);
struct Cyc_Absyn_Exp*_tmp9B=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp9C=_tmp9B == 0?0: Cyc_Tcutil_rsubsexp(_tmp98,_tmp99,_tmp9B);
struct Cyc_Absyn_Aggrfield*_tmp45E;return(_tmp45E=_region_malloc(_tmp98,sizeof(*_tmp45E)),((_tmp45E->name=f->name,((_tmp45E->tq=f->tq,((_tmp45E->type=_tmp9A,((_tmp45E->width=f->width,((_tmp45E->attributes=f->attributes,((_tmp45E->requires_clause=_tmp9C,_tmp45E)))))))))))));};}struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 288
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 294
struct _RegionHandle _tmp9E=_new_region("temp");struct _RegionHandle*temp=& _tmp9E;_push_region(temp);
# 298
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp9F=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpA0=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmpA1=Cyc_Tcutil_tvar_kind(_tmp9F,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA2=_tmpA1;_LL28: if(_tmpA2->kind != Cyc_Absyn_RgnKind)goto _LL2A;_LL29:
 goto _LL2B;_LL2A: if(_tmpA2->kind != Cyc_Absyn_EffKind)goto _LL2C;_LL2B:
 continue;_LL2C:;_LL2D:
 goto _LL27;_LL27:;}{
# 307
struct _tuple16*_tmp461;struct Cyc_List_List*_tmp460;inst=((_tmp460=_region_malloc(temp,sizeof(*_tmp460)),((_tmp460->hd=((_tmp461=_region_malloc(temp,sizeof(*_tmp461)),((_tmp461->f1=_tmp9F,((_tmp461->f2=_tmpA0,_tmp461)))))),((_tmp460->tl=inst,_tmp460))))));};}
# 309
if(inst != 0){
struct _tuple15 _tmp462;struct _tuple15 _tmpA5=(_tmp462.f1=temp,((_tmp462.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst),_tmp462)));
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple15*,struct Cyc_Absyn_Aggrfield*),struct _tuple15*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmpA5,fs);
struct _dyneither_ptr _tmpA6=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA6;}else{
# 314
struct _dyneither_ptr _tmpA8=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0);return _tmpA8;}}
# 298
;_pop_region(temp);}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);static void _tmp468(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp467,unsigned int*_tmp466,void***_tmp464){for(*_tmp467=0;*_tmp467 < *_tmp466;(*_tmp467)++){(*_tmp464)[*_tmp467]=(void*)(*fenv)->unknown_all;}}
# 318
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 324
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 326
{void*_tmpA9=Cyc_Tcutil_compress(t);void*_tmpAA=_tmpA9;struct Cyc_Absyn_Datatypefield*_tmpAC;struct Cyc_List_List*_tmpAE;union Cyc_Absyn_AggrInfoU _tmpB0;struct Cyc_List_List*_tmpB1;enum Cyc_Absyn_AggrKind _tmpB3;struct Cyc_List_List*_tmpB4;void*_tmpB6;union Cyc_Absyn_Constraint*_tmpB7;void*_tmpB9;union Cyc_Absyn_Constraint*_tmpBB;_LL2F: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpAA;if(_tmpAB->tag != 4)goto _LL31;else{if((((_tmpAB->f1).field_info).KnownDatatypefield).tag != 2)goto _LL31;_tmpAC=((struct _tuple2)(((_tmpAB->f1).field_info).KnownDatatypefield).val).f2;}}_LL30:
# 328
 if(_tmpAC->typs == 0)
return leafval;
_tmpAE=_tmpAC->typs;goto _LL32;_LL31: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAA;if(_tmpAD->tag != 10)goto _LL33;else{_tmpAE=_tmpAD->f1;}}_LL32: {
# 332
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpAE);
# 335
unsigned int _tmp467;unsigned int _tmp466;struct _dyneither_ptr _tmp465;void**_tmp464;unsigned int _tmp463;struct _dyneither_ptr d=(_tmp463=sz,((_tmp464=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp463)),((_tmp465=_tag_dyneither(_tmp464,sizeof(void*),_tmp463),((((_tmp466=_tmp463,_tmp468(& fenv,& _tmp467,& _tmp466,& _tmp464))),_tmp465)))))));
{int i=0;for(0;i < sz;++ i){
*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpAE))->hd)).f2,leafval);
_tmpAE=_tmpAE->tl;}}{
# 340
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp46E;struct Cyc_CfFlowInfo_UnionRInfo _tmp46D;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp46C;return(void*)((_tmp46C=_region_malloc(fenv->r,sizeof(*_tmp46C)),((_tmp46C[0]=((_tmp46E.tag=6,((_tmp46E.f1=((_tmp46D.is_union=0,((_tmp46D.fieldnum=- 1,_tmp46D)))),((_tmp46E.f2=d,_tmp46E)))))),_tmp46C))));};}_LL33: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA;if(_tmpAF->tag != 11)goto _LL35;else{_tmpB0=(_tmpAF->f1).aggr_info;_tmpB1=(_tmpAF->f1).targs;}}_LL34: {
# 342
struct Cyc_Absyn_Aggrdecl*_tmpC3=Cyc_Absyn_get_known_aggrdecl(_tmpB0);
if(_tmpC3->impl == 0)
goto _LL2E;{
struct Cyc_List_List*_tmpC4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC3->impl))->fields;
if(_tmpB1 == 0){_tmpB3=_tmpC3->kind;_tmpB4=_tmpC4;goto _LL36;}{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp474;struct Cyc_CfFlowInfo_UnionRInfo _tmp473;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp472;return(void*)((_tmp472=_region_malloc(fenv->r,sizeof(*_tmp472)),((_tmp472[0]=(
(_tmp474.tag=6,((_tmp474.f1=((_tmp473.is_union=_tmpC3->kind == Cyc_Absyn_UnionA,((_tmp473.fieldnum=- 1,_tmp473)))),((_tmp474.f2=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmpC3->tvs,_tmpB1,_tmpC4,_tmpC3->kind == Cyc_Absyn_UnionA,leafval),_tmp474)))))),_tmp472))));};};}_LL35: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA;if(_tmpB2->tag != 12)goto _LL37;else{_tmpB3=_tmpB2->f1;_tmpB4=_tmpB2->f2;}}_LL36: {
# 353
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp47A;struct Cyc_CfFlowInfo_UnionRInfo _tmp479;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp478;return(void*)((_tmp478=_region_malloc(fenv->r,sizeof(*_tmp478)),((_tmp478[0]=((_tmp47A.tag=6,((_tmp47A.f1=((_tmp479.is_union=_tmpB3 == Cyc_Absyn_UnionA,((_tmp479.fieldnum=- 1,_tmp479)))),((_tmp47A.f2=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmpB4,_tmpB3 == Cyc_Absyn_UnionA,leafval),_tmp47A)))))),_tmp478))));}_LL37:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAA;if(_tmpB5->tag != 8)goto _LL39;else{_tmpB6=(_tmpB5->f1).elt_type;_tmpB7=(_tmpB5->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB7))goto _LL39;_LL38:
# 357
 return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_bits_only(_tmpB6)?fenv->unknown_all: leafval;_LL39: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAA;if(_tmpB8->tag != 19)goto _LL3B;else{_tmpB9=(void*)_tmpB8->f1;}}_LL3A:
 return leafval;_LL3B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA;if(_tmpBA->tag != 5)goto _LL3D;else{_tmpBB=((_tmpBA->f1).ptr_atts).nullable;}}if(!(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpBB)))goto _LL3D;_LL3C:
{void*_tmpCB=leafval;_LL40: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCC=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpCB;if(_tmpCC->tag != 3)goto _LL42;else{if(_tmpCC->f1 != Cyc_CfFlowInfo_ThisIL)goto _LL42;}}_LL41:
 return fenv->notzerothis;_LL42: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpCD=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpCB;if(_tmpCD->tag != 3)goto _LL44;else{if(_tmpCD->f1 != Cyc_CfFlowInfo_AllIL)goto _LL44;}}_LL43:
 return fenv->notzeroall;_LL44:;_LL45:
 goto _LL3F;_LL3F:;}
# 365
goto _LL2E;_LL3D:;_LL3E:
 goto _LL2E;_LL2E:;}
# 369
return !no_init_bits_only  && Cyc_Tcutil_bits_only(t)?fenv->unknown_all: leafval;}
# 372
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 378
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmpCE=r;struct Cyc_Absyn_Exp*_tmpD0;int _tmpD1;void*_tmpD2;int _tmpD4;int _tmpD5;struct _dyneither_ptr _tmpD6;void*_tmpD8;_LL47: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpCF=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpCE;if(_tmpCF->tag != 7)goto _LL49;else{_tmpD0=_tmpCF->f1;_tmpD1=_tmpCF->f2;_tmpD2=(void*)_tmpCF->f3;}}_LL48:
# 381
 if(_tmpD0 == e  && _tmpD1 == env_iteration){
*needs_unconsume=1;
return 0;}
# 385
return 1;_LL49: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpD3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpCE;if(_tmpD3->tag != 6)goto _LL4B;else{_tmpD4=(_tmpD3->f1).is_union;_tmpD5=(_tmpD3->f1).fieldnum;_tmpD6=_tmpD3->f2;}}_LL4A:
# 387
 if(!_tmpD4  || _tmpD5 == - 1){
unsigned int _tmpD9=_get_dyneither_size(_tmpD6,sizeof(void*));
{int i=0;for(0;i < _tmpD9;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,((void**)_tmpD6.curr)[i],needs_unconsume))
return 1;}}
# 393
return 0;}else{
# 396
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmpD6,sizeof(void*),_tmpD5)),needs_unconsume);}_LL4B: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpD7=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpCE;if(_tmpD7->tag != 8)goto _LL4D;else{_tmpD8=(void*)_tmpD7->f2;}}_LL4C:
# 398
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmpD8,needs_unconsume);_LL4D:;_LL4E:
 return 0;_LL46:;}void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);static void _tmp480(struct _dyneither_ptr*_tmpE1,unsigned int*_tmp47F,unsigned int*_tmp47E,void***_tmp47C){for(*_tmp47F=0;*_tmp47F < *_tmp47E;(*_tmp47F)++){(*_tmp47C)[*_tmp47F]=(void*)((void**)(*_tmpE1).curr)[(int)*_tmp47F];}}
# 405
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmpDA=r;struct Cyc_Absyn_Exp*_tmpDC;int _tmpDD;void*_tmpDE;struct Cyc_CfFlowInfo_UnionRInfo _tmpE0;struct _dyneither_ptr _tmpE1;struct Cyc_Absyn_Vardecl*_tmpE3;void*_tmpE4;_LL50: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpDB=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpDA;if(_tmpDB->tag != 7)goto _LL52;else{_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;_tmpDE=(void*)_tmpDB->f3;}}_LL51:
# 408
 return _tmpDE;_LL52: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpDF=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDA;if(_tmpDF->tag != 6)goto _LL54;else{_tmpE0=_tmpDF->f1;_tmpE1=_tmpDF->f2;}}_LL53: {
# 410
unsigned int _tmpE5=_get_dyneither_size(_tmpE1,sizeof(void*));
int change=0;
unsigned int _tmp47F;unsigned int _tmp47E;struct _dyneither_ptr _tmp47D;void**_tmp47C;unsigned int _tmp47B;struct _dyneither_ptr d2=(_tmp47B=_tmpE5,((_tmp47C=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp47B)),((_tmp47D=_tag_dyneither(_tmp47C,sizeof(void*),_tmp47B),((((_tmp47E=_tmp47B,_tmp480(& _tmpE1,& _tmp47F,& _tmp47E,& _tmp47C))),_tmp47D)))))));
{int i=0;for(0;i < _tmpE5;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,((void**)_tmpE1.curr)[i]);
if(*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))!= ((void**)_tmpE1.curr)[i])
change=1;}}
# 418
if(change){
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp483;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp482;return(void*)((_tmp482=_region_malloc(fenv->r,sizeof(*_tmp482)),((_tmp482[0]=((_tmp483.tag=6,((_tmp483.f1=_tmpE0,((_tmp483.f2=d2,_tmp483)))))),_tmp482))));}else{
return r;}}_LL54: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpDA;if(_tmpE2->tag != 8)goto _LL56;else{_tmpE3=_tmpE2->f1;_tmpE4=(void*)_tmpE2->f2;}}_LL55: {
# 422
void*_tmpEC=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmpE4);
if(_tmpEC != _tmpE4){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp486;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp485;return(void*)((_tmp485=_region_malloc(fenv->r,sizeof(*_tmp485)),((_tmp485[0]=((_tmp486.tag=8,((_tmp486.f1=_tmpE3,((_tmp486.f2=_tmpEC,_tmp486)))))),_tmp485))));}else{
return r;}}_LL56:;_LL57:
 return r;_LL4F:;}void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r);static void _tmp490(struct Cyc_CfFlowInfo_FlowEnv**fenv,unsigned int*_tmp48F,unsigned int*_tmp48E,void***_tmp48C){for(*_tmp48F=0;*_tmp48F < *_tmp48E;(*_tmp48F)++){(*_tmp48C)[*_tmp48F]=(void*)(*fenv)->unknown_all;}}static void _tmp499(unsigned int*_tmp498,unsigned int*_tmp497,void***_tmp495,struct _dyneither_ptr*_tmp103){for(*_tmp498=0;*_tmp498 < *_tmp497;(*_tmp498)++){(*_tmp495)[*_tmp498]=(void*)((void**)(*_tmp103).curr)[(int)*_tmp498];}}
# 432
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple0 _tmp487;struct _tuple0 _tmpEF=(_tmp487.f1=Cyc_Tcutil_compress(t),((_tmp487.f2=r,_tmp487)));struct _tuple0 _tmpF0=_tmpEF;struct Cyc_Absyn_Vardecl*_tmpF2;void*_tmpF3;struct Cyc_List_List*_tmpF5;struct Cyc_CfFlowInfo_UnionRInfo _tmpF7;struct _dyneither_ptr _tmpF8;union Cyc_Absyn_AggrInfoU _tmpFA;struct Cyc_CfFlowInfo_UnionRInfo _tmpFC;struct _dyneither_ptr _tmpFD;enum Cyc_Absyn_AggrKind _tmpFF;struct Cyc_List_List*_tmp100;struct Cyc_CfFlowInfo_UnionRInfo _tmp102;struct _dyneither_ptr _tmp103;_LL59: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpF1=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF0.f2;if(_tmpF1->tag != 8)goto _LL5B;else{_tmpF2=_tmpF1->f1;_tmpF3=(void*)_tmpF1->f2;}}_LL5A: {
# 435
void*_tmp104=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmpF3);
if(_tmp104 != _tmpF3){struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp48A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp489;return(void*)((_tmp489=_region_malloc(fenv->r,sizeof(*_tmp489)),((_tmp489[0]=((_tmp48A.tag=8,((_tmp48A.f1=_tmpF2,((_tmp48A.f2=_tmp104,_tmp48A)))))),_tmp489))));}else{
return r;}}_LL5B:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF0.f1;if(_tmpF4->tag != 10)goto _LL5D;else{_tmpF5=_tmpF4->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpF6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF0.f2;if(_tmpF6->tag != 6)goto _LL5D;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}};_LL5C: {
# 439
unsigned int _tmp107=_get_dyneither_size(_tmpF8,sizeof(void*));
unsigned int _tmp48F;unsigned int _tmp48E;struct _dyneither_ptr _tmp48D;void**_tmp48C;unsigned int _tmp48B;struct _dyneither_ptr d2=(_tmp48B=_tmp107,((_tmp48C=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp48B)),((_tmp48D=_tag_dyneither(_tmp48C,sizeof(void*),_tmp48B),((((_tmp48E=_tmp48B,_tmp490(& fenv,& _tmp48F,& _tmp48E,& _tmp48C))),_tmp48D)))))));
{int i=0;for(0;i < _tmp107;++ i){
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmpF5))->hd)).f2,consumer,iteration,((void**)_tmpF8.curr)[i]);
_tmpF5=_tmpF5->tl;}}{
# 445
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp493;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp492;return(void*)((_tmp492=_region_malloc(fenv->r,sizeof(*_tmp492)),((_tmp492[0]=((_tmp493.tag=6,((_tmp493.f1=_tmpF7,((_tmp493.f2=d2,_tmp493)))))),_tmp492))));};}_LL5D:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF0.f1;if(_tmpF9->tag != 11)goto _LL5F;else{_tmpFA=(_tmpF9->f1).aggr_info;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpFB=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF0.f2;if(_tmpFB->tag != 6)goto _LL5F;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f2;}};_LL5E: {
# 447
struct Cyc_Absyn_Aggrdecl*_tmp10E=Cyc_Absyn_get_known_aggrdecl(_tmpFA);
if(_tmp10E->impl == 0)return r;
_tmpFF=_tmp10E->kind;_tmp100=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10E->impl))->fields;_tmp102=_tmpFC;_tmp103=_tmpFD;goto _LL60;}_LL5F:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpFE=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF0.f1;if(_tmpFE->tag != 12)goto _LL61;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp101=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF0.f2;if(_tmp101->tag != 6)goto _LL61;else{_tmp102=_tmp101->f1;_tmp103=_tmp101->f2;}};_LL60: {
# 451
unsigned int _tmp498;unsigned int _tmp497;struct _dyneither_ptr _tmp496;void**_tmp495;unsigned int _tmp494;struct _dyneither_ptr d2=(_tmp494=_get_dyneither_size(_tmp103,sizeof(void*)),((_tmp495=(void**)_region_malloc(fenv->r,_check_times(sizeof(void*),_tmp494)),((_tmp496=_tag_dyneither(_tmp495,sizeof(void*),_tmp494),((((_tmp497=_tmp494,_tmp499(& _tmp498,& _tmp497,& _tmp495,& _tmp103))),_tmp496)))))));
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp100);
{int i=0;for(0;i < sz;(i ++,_tmp100=_tmp100->tl)){
struct Cyc_Absyn_Aggrfield*_tmp10F=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp100))->hd;struct _dyneither_ptr*_tmp111;void*_tmp112;struct Cyc_Absyn_Aggrfield*_tmp110=_tmp10F;_tmp111=_tmp110->name;_tmp112=_tmp110->type;
if(_get_dyneither_size(*_tmp111,sizeof(char))!= 1)
*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp112,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp103,sizeof(void*),i)));}}{
# 458
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp49C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp49B;return(void*)((_tmp49B=_region_malloc(fenv->r,sizeof(*_tmp49B)),((_tmp49B[0]=((_tmp49C.tag=6,((_tmp49C.f1=_tmp102,((_tmp49C.f2=d2,_tmp49C)))))),_tmp49B))));};}_LL61:;_LL62:
# 460
 if(Cyc_Tcutil_is_noalias_pointer(t,0)){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp49F;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp49E;return(void*)((_tmp49E=_region_malloc(fenv->r,sizeof(*_tmp49E)),((_tmp49E[0]=((_tmp49F.tag=7,((_tmp49F.f1=consumer,((_tmp49F.f2=iteration,((_tmp49F.f3=r,_tmp49F)))))))),_tmp49E))));}else{
return r;}_LL58:;}struct _tuple18{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 466
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 469
struct _RegionHandle _tmp11C=_new_region("r");struct _RegionHandle*r=& _tmp11C;_push_region(r);
{struct _tuple18 _tmp4A0;struct _tuple18 elem=(_tmp4A0.f1=place,((_tmp4A0.f2=0,_tmp4A0)));
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple18*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp11D=elem.f1;
struct Cyc_CfFlowInfo_Place _tmp11E=*place;void*_tmp120;struct Cyc_List_List*_tmp121;struct Cyc_CfFlowInfo_Place _tmp11F=_tmp11E;_tmp120=_tmp11F.root;_tmp121=_tmp11F.fields;{
struct Cyc_CfFlowInfo_Place _tmp122=*_tmp11D;void*_tmp124;struct Cyc_List_List*_tmp125;struct Cyc_CfFlowInfo_Place _tmp123=_tmp122;_tmp124=_tmp123.root;_tmp125=_tmp123.fields;
if(Cyc_CfFlowInfo_root_cmp(_tmp120,_tmp124)!= 0)
continue;
for(0;_tmp121 != 0  && _tmp125 != 0;(_tmp121=_tmp121->tl,_tmp125=_tmp125->tl)){
if((int)_tmp121->hd != (int)_tmp125->hd)break;}
if(_tmp121 == 0){
int _tmp126=1;_npop_handler(0);return _tmp126;}};}{
# 483
int _tmp127=0;_npop_handler(0);return _tmp127;};}
# 470
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct _RegionHandle*rgn;struct Cyc_List_List*places;};
# 496
typedef struct Cyc_CfFlowInfo_EscPile*Cyc_CfFlowInfo_escpile_t;
# 498
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 501
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place)){
struct Cyc_List_List*_tmp4A1;pile->places=((_tmp4A1=_region_malloc(pile->rgn,sizeof(*_tmp4A1)),((_tmp4A1->hd=place,((_tmp4A1->tl=pile->places,_tmp4A1))))));}}
# 504
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmp12A=r;void*_tmp12C;void*_tmp12E;struct Cyc_CfFlowInfo_Place*_tmp130;struct Cyc_CfFlowInfo_UnionRInfo _tmp132;struct _dyneither_ptr _tmp133;_LL64: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp12B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp12A;if(_tmp12B->tag != 7)goto _LL66;else{_tmp12C=(void*)_tmp12B->f3;}}_LL65:
 Cyc_CfFlowInfo_add_places(pile,_tmp12C);return;_LL66: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp12D=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp12A;if(_tmp12D->tag != 8)goto _LL68;else{_tmp12E=(void*)_tmp12D->f2;}}_LL67:
 Cyc_CfFlowInfo_add_places(pile,_tmp12E);return;_LL68: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp12F=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp12A;if(_tmp12F->tag != 5)goto _LL6A;else{_tmp130=_tmp12F->f1;}}_LL69:
 Cyc_CfFlowInfo_add_place(pile,_tmp130);return;_LL6A: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp131=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp12A;if(_tmp131->tag != 6)goto _LL6C;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;}}_LL6B:
# 510
{int i=0;for(0;i < _get_dyneither_size(_tmp133,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,((void**)_tmp133.curr)[i]);}}
return;_LL6C:;_LL6D:
 return;_LL63:;}static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val);static void _tmp4A7(struct _RegionHandle**r,void**new_val,unsigned int*_tmp4A6,unsigned int*_tmp4A5,void***_tmp4A3,struct _dyneither_ptr*_tmp137){for(*_tmp4A6=0;*_tmp4A6 < *_tmp4A5;(*_tmp4A6)++){(*_tmp4A3)[*_tmp4A6]=(void*)
# 525
Cyc_CfFlowInfo_insert_place_inner(*r,*new_val,((void**)(*_tmp137).curr)[(int)*_tmp4A6]);}}
# 520
static void*Cyc_CfFlowInfo_insert_place_inner(struct _RegionHandle*r,void*new_val,void*old_val){
# 522
void*_tmp134=old_val;int _tmp136;struct _dyneither_ptr _tmp137;struct Cyc_Absyn_Exp*_tmp139;int _tmp13A;void*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13D;void*_tmp13E;_LL6F: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp135=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp134;if(_tmp135->tag != 6)goto _LL71;else{_tmp136=(_tmp135->f1).is_union;_tmp137=_tmp135->f2;}}_LL70: {
# 524
unsigned int _tmp4A6;unsigned int _tmp4A5;struct _dyneither_ptr _tmp4A4;void**_tmp4A3;unsigned int _tmp4A2;struct _dyneither_ptr d2=(_tmp4A2=_get_dyneither_size(_tmp137,sizeof(void*)),((_tmp4A3=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp4A2)),((_tmp4A4=_tag_dyneither(_tmp4A3,sizeof(void*),_tmp4A2),((((_tmp4A5=_tmp4A2,_tmp4A7(& r,& new_val,& _tmp4A6,& _tmp4A5,& _tmp4A3,& _tmp137))),_tmp4A4)))))));
# 528
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4AD;struct Cyc_CfFlowInfo_UnionRInfo _tmp4AC;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4AB;return(void*)((_tmp4AB=_region_malloc(r,sizeof(*_tmp4AB)),((_tmp4AB[0]=((_tmp4AD.tag=6,((_tmp4AD.f1=((_tmp4AC.is_union=_tmp136,((_tmp4AC.fieldnum=- 1,_tmp4AC)))),((_tmp4AD.f2=d2,_tmp4AD)))))),_tmp4AB))));}_LL71: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp138=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp134;if(_tmp138->tag != 7)goto _LL73;else{_tmp139=_tmp138->f1;_tmp13A=_tmp138->f2;_tmp13B=(void*)_tmp138->f3;}}_LL72: {
# 530
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp4B0;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp4AF;return(void*)((_tmp4AF=_region_malloc(r,sizeof(*_tmp4AF)),((_tmp4AF[0]=((_tmp4B0.tag=7,((_tmp4B0.f1=_tmp139,((_tmp4B0.f2=_tmp13A,((_tmp4B0.f3=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp13B),_tmp4B0)))))))),_tmp4AF))));}_LL73: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp13C=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp134;if(_tmp13C->tag != 8)goto _LL75;else{_tmp13D=_tmp13C->f1;_tmp13E=(void*)_tmp13C->f2;}}_LL74: {
# 532
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4B3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4B2;return(void*)((_tmp4B2=_region_malloc(r,sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B3.tag=8,((_tmp4B3.f1=_tmp13D,((_tmp4B3.f2=Cyc_CfFlowInfo_insert_place_inner(r,new_val,_tmp13E),_tmp4B3)))))),_tmp4B2))));}_LL75:;_LL76:
 return new_val;_LL6E:;}static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval);static void _tmp4B9(struct _dyneither_ptr*d,unsigned int*_tmp4B8,unsigned int*_tmp4B7,void***_tmp4B5){for(*_tmp4B8=0;*_tmp4B8 < *_tmp4B7;(*_tmp4B8)++){(*_tmp4B5)[*_tmp4B8]=(void*)((void**)(*d).curr)[(int)*_tmp4B8];}}
# 539
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _RegionHandle*r,struct _dyneither_ptr d,int n,void*rval){
# 541
void*_tmp14A=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmp14A == rval)return d;{
unsigned int _tmp4B8;unsigned int _tmp4B7;struct _dyneither_ptr _tmp4B6;void**_tmp4B5;unsigned int _tmp4B4;struct _dyneither_ptr res=(_tmp4B4=_get_dyneither_size(d,sizeof(void*)),((_tmp4B5=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp4B4)),((_tmp4B6=_tag_dyneither(_tmp4B5,sizeof(void*),_tmp4B4),((((_tmp4B7=_tmp4B4,_tmp4B9(& d,& _tmp4B8,& _tmp4B7,& _tmp4B5))),_tmp4B6)))))));
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple19{struct Cyc_List_List*f1;void*f2;};
# 553
static void*Cyc_CfFlowInfo_insert_place_outer(struct _RegionHandle*r,struct Cyc_List_List*fs,void*old_val,void*new_val){
# 557
if(fs == 0)
return Cyc_CfFlowInfo_insert_place_inner(r,new_val,old_val);{
struct _tuple19 _tmp4BA;struct _tuple19 _tmp14F=(_tmp4BA.f1=fs,((_tmp4BA.f2=old_val,_tmp4BA)));struct _tuple19 _tmp150=_tmp14F;int _tmp151;struct Cyc_List_List*_tmp152;int _tmp154;struct _dyneither_ptr _tmp155;struct Cyc_Absyn_Vardecl*_tmp157;void*_tmp158;_LL78: if(_tmp150.f1 == 0)goto _LL7A;_tmp151=(int)(_tmp150.f1)->hd;_tmp152=(_tmp150.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp153=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp150.f2;if(_tmp153->tag != 6)goto _LL7A;else{_tmp154=(_tmp153->f1).is_union;_tmp155=_tmp153->f2;}};_LL79: {
# 561
void*_tmp159=Cyc_CfFlowInfo_insert_place_outer(r,_tmp152,*((void**)_check_dyneither_subscript(_tmp155,sizeof(void*),_tmp151)),new_val);
struct _dyneither_ptr _tmp15A=Cyc_CfFlowInfo_aggr_dict_insert(r,_tmp155,_tmp151,_tmp159);
if((void**)_tmp15A.curr == (void**)_tmp155.curr)return old_val;else{
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4C0;struct Cyc_CfFlowInfo_UnionRInfo _tmp4BF;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4BE;return(void*)((_tmp4BE=_region_malloc(r,sizeof(*_tmp4BE)),((_tmp4BE[0]=((_tmp4C0.tag=6,((_tmp4C0.f1=((_tmp4BF.is_union=_tmp154,((_tmp4BF.fieldnum=- 1,_tmp4BF)))),((_tmp4C0.f2=_tmp15A,_tmp4C0)))))),_tmp4BE))));}}_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp156=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp150.f2;if(_tmp156->tag != 8)goto _LL7C;else{_tmp157=_tmp156->f1;_tmp158=(void*)_tmp156->f2;}}_LL7B: {
# 566
void*_tmp15E=Cyc_CfFlowInfo_insert_place_outer(r,fs,_tmp158,new_val);
if(_tmp15E == _tmp158)return old_val;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4C3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4C2;return(void*)((_tmp4C2=_region_malloc(r,sizeof(*_tmp4C2)),((_tmp4C2[0]=((_tmp4C3.tag=8,((_tmp4C3.f1=_tmp157,((_tmp4C3.f2=_tmp15E,_tmp4C3)))))),_tmp4C2))));}}_LL7C:;_LL7D: {
struct Cyc_Core_Impossible_exn_struct _tmp4C9;const char*_tmp4C8;struct Cyc_Core_Impossible_exn_struct*_tmp4C7;(int)_throw((void*)((_tmp4C7=_cycalloc(sizeof(*_tmp4C7)),((_tmp4C7[0]=((_tmp4C9.tag=Cyc_Core_Impossible,((_tmp4C9.f1=((_tmp4C8="bad insert place",_tag_dyneither(_tmp4C8,sizeof(char),17))),_tmp4C9)))),_tmp4C7)))));}_LL77:;};}
# 576
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict*all_changed,struct Cyc_Dict_Dict d){
# 580
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmp165=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,_tmp165,0);{
void*oldval;void*newval;
{struct _handler_cons _tmp166;_push_handler(& _tmp166);{int _tmp168=0;if(setjmp(_tmp166.handler))_tmp168=1;if(!_tmp168){oldval=Cyc_CfFlowInfo_lookup_place(d,_tmp165);;_pop_handler();}else{void*_tmp167=(void*)_exn_thrown;void*_tmp16A=_tmp167;void*_tmp16C;_LL7F: {struct Cyc_Dict_Absent_exn_struct*_tmp16B=(struct Cyc_Dict_Absent_exn_struct*)_tmp16A;if(_tmp16B->tag != Cyc_Dict_Absent)goto _LL81;}_LL80:
 continue;_LL81: _tmp16C=_tmp16A;_LL82:(void)_throw(_tmp16C);_LL7E:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmp16D=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);switch(_tmp16D){case Cyc_CfFlowInfo_AllIL: _LL83:
 newval=fenv->esc_all;break;case Cyc_CfFlowInfo_ThisIL: _LL84:
 newval=fenv->esc_this;break;case Cyc_CfFlowInfo_NoneIL: _LL85:
 newval=fenv->esc_none;break;}}
# 592
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmp16E=*_tmp165;void*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_CfFlowInfo_Place _tmp16F=_tmp16E;_tmp170=_tmp16F.root;_tmp171=_tmp16F.fields;
d=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp170,
Cyc_CfFlowInfo_insert_place_outer(fenv->r,_tmp171,
# 597
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp170),newval));};};}
# 600
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct _RegionHandle*e;struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 612
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmp172=r;enum Cyc_CfFlowInfo_InitLevel _tmp174;enum Cyc_CfFlowInfo_InitLevel _tmp176;void*_tmp17B;_LL88: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp173=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp173->tag != 3)goto _LL8A;else{_tmp174=_tmp173->f1;}}_LL89:
 return _tmp174;_LL8A: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp175=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp175->tag != 4)goto _LL8C;else{_tmp176=_tmp175->f1;}}_LL8B:
 return _tmp176;_LL8C: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp177=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp177->tag != 0)goto _LL8E;}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp178=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp178->tag != 1)goto _LL90;}_LL8F:
 return Cyc_CfFlowInfo_AllIL;_LL90: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp179=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp179->tag != 2)goto _LL92;}_LL91:
 return Cyc_CfFlowInfo_ThisIL;_LL92: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp17A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp172;if(_tmp17A->tag != 7)goto _LL94;else{_tmp17B=(void*)_tmp17A->f3;}}_LL93:
 return Cyc_CfFlowInfo_NoneIL;_LL94:;_LL95: {
struct Cyc_Core_Impossible_exn_struct _tmp4CF;const char*_tmp4CE;struct Cyc_Core_Impossible_exn_struct*_tmp4CD;(int)_throw((void*)((_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD[0]=((_tmp4CF.tag=Cyc_Core_Impossible,((_tmp4CF.f1=((_tmp4CE="initlevel_approx",_tag_dyneither(_tmp4CE,sizeof(char),17))),_tmp4CF)))),_tmp4CD)))));}_LL87:;}
# 623
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 625
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmp17F=r;void*_tmp181;int _tmp183;int _tmp184;struct _dyneither_ptr _tmp185;struct _dyneither_ptr _tmp187;struct Cyc_CfFlowInfo_Place*_tmp189;_LL97: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp180=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp17F;if(_tmp180->tag != 8)goto _LL99;else{_tmp181=(void*)_tmp180->f2;}}_LL98:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmp181,acc);_LL99:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp182=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F;if(_tmp182->tag != 6)goto _LL9B;else{_tmp183=(_tmp182->f1).is_union;_tmp184=(_tmp182->f1).fieldnum;_tmp185=_tmp182->f2;}}if(!_tmp183)goto _LL9B;_LL9A: {
# 633
unsigned int _tmp18A=_get_dyneither_size(_tmp185,sizeof(void*));
this_ans=Cyc_CfFlowInfo_NoneIL;
if(_tmp184 == - 1){
int i=0;for(0;i < _tmp18A;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp185.curr)[i],Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=Cyc_CfFlowInfo_AllIL;
break;}}}else{
# 643
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmp185,sizeof(void*),_tmp184)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=Cyc_CfFlowInfo_AllIL;}
goto _LL96;}_LL9B: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp186=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp17F;if(_tmp186->tag != 6)goto _LL9D;else{_tmp187=_tmp186->f2;}}_LL9C: {
# 647
unsigned int _tmp18B=_get_dyneither_size(_tmp187,sizeof(void*));
this_ans=Cyc_CfFlowInfo_AllIL;
{int i=0;for(0;i < _tmp18B;++ i){
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,((void**)_tmp187.curr)[i],this_ans);}}
goto _LL96;}_LL9D: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp188=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp17F;if(_tmp188->tag != 5)goto _LL9F;else{_tmp189=_tmp188->f1;}}_LL9E:
# 653
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmp189))
this_ans=Cyc_CfFlowInfo_AllIL;else{
# 656
{struct Cyc_List_List*_tmp4D0;env->seen=((_tmp4D0=_region_malloc(env->e,sizeof(*_tmp4D0)),((_tmp4D0->hd=_tmp189,((_tmp4D0->tl=env->seen,_tmp4D0))))));}
this_ans=Cyc_CfFlowInfo_initlevel_rec(env,Cyc_CfFlowInfo_lookup_place(env->d,_tmp189),Cyc_CfFlowInfo_AllIL);
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;
if(this_ans == Cyc_CfFlowInfo_NoneIL)
this_ans=Cyc_CfFlowInfo_ThisIL;}
# 662
goto _LL96;_LL9F:;_LLA0:
 this_ans=Cyc_CfFlowInfo_initlevel_approx(r);_LL96:;}
# 665
if(this_ans == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
if(this_ans == Cyc_CfFlowInfo_ThisIL  || acc == Cyc_CfFlowInfo_ThisIL)return Cyc_CfFlowInfo_ThisIL;
return Cyc_CfFlowInfo_AllIL;}
# 669
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle*_tmp18D=env->r;
struct Cyc_CfFlowInfo_InitlevelEnv _tmp4D1;struct Cyc_CfFlowInfo_InitlevelEnv _tmp18E=(_tmp4D1.e=_tmp18D,((_tmp4D1.d=d,((_tmp4D1.seen=0,_tmp4D1)))));
return Cyc_CfFlowInfo_initlevel_rec(& _tmp18E,r,Cyc_CfFlowInfo_AllIL);}struct _tuple20{void*f1;int f2;};
# 676
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp190=*place;void*_tmp192;struct Cyc_List_List*_tmp193;struct Cyc_CfFlowInfo_Place _tmp191=_tmp190;_tmp192=_tmp191.root;_tmp193=_tmp191.fields;{
void*_tmp194=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp192);
for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
retry: {struct _tuple20 _tmp4D2;struct _tuple20 _tmp195=(_tmp4D2.f1=_tmp194,((_tmp4D2.f2=(int)_tmp193->hd,_tmp4D2)));struct _tuple20 _tmp196=_tmp195;void*_tmp198;int _tmp199;struct Cyc_CfFlowInfo_UnionRInfo _tmp19B;struct _dyneither_ptr _tmp19C;int _tmp19D;_LLA2:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp197=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1;if(_tmp197->tag != 8)goto _LLA4;else{_tmp198=(void*)_tmp197->f2;}}_tmp199=_tmp196.f2;_LLA3:
# 682
 _tmp194=_tmp198;goto retry;_LLA4:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp19A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1;if(_tmp19A->tag != 6)goto _LLA6;else{_tmp19B=_tmp19A->f1;_tmp19C=_tmp19A->f2;}}_tmp19D=_tmp196.f2;_LLA5:
# 684
 _tmp194=*((void**)_check_dyneither_subscript(_tmp19C,sizeof(void*),_tmp19D));
goto _LLA1;_LLA6:;_LLA7: {
# 690
struct Cyc_Core_Impossible_exn_struct _tmp4D8;const char*_tmp4D7;struct Cyc_Core_Impossible_exn_struct*_tmp4D6;(int)_throw((void*)((_tmp4D6=_cycalloc(sizeof(*_tmp4D6)),((_tmp4D6[0]=((_tmp4D8.tag=Cyc_Core_Impossible,((_tmp4D8.f1=((_tmp4D7="bad lookup_place",_tag_dyneither(_tmp4D7,sizeof(char),17))),_tmp4D8)))),_tmp4D6)))));}_LLA1:;}}
# 692
return _tmp194;};}
# 695
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmp1A2=rval;void*_tmp1A5;void*_tmp1A7;int _tmp1A9;int _tmp1AA;struct _dyneither_ptr _tmp1AB;_LLA9: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1A3=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1A2;if(_tmp1A3->tag != 4)goto _LLAB;}_LLAA:
 return 0;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp1A4=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1A2;if(_tmp1A4->tag != 7)goto _LLAD;else{_tmp1A5=(void*)_tmp1A4->f3;}}_LLAC:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp1A5);_LLAD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1A6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1A2;if(_tmp1A6->tag != 8)goto _LLAF;else{_tmp1A7=(void*)_tmp1A6->f2;}}_LLAE:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmp1A7);_LLAF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1A8=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1A2;if(_tmp1A8->tag != 6)goto _LLB1;else{_tmp1A9=(_tmp1A8->f1).is_union;_tmp1AA=(_tmp1A8->f1).fieldnum;_tmp1AB=_tmp1A8->f2;}}_LLB0:
# 701
 if(_tmp1A9  && _tmp1AA != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmp1AB,sizeof(void*),_tmp1AA)));else{
# 704
unsigned int sz=_get_dyneither_size(_tmp1AB,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(((void**)_tmp1AB.curr)[i]))return 0;}}
return 1;}_LLB1:;_LLB2:
# 709
 return 1;_LLA8:;}
# 712
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 717
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r){
struct _RegionHandle*_tmp1AC=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp4D9;struct Cyc_CfFlowInfo_EscPile*pile=(_tmp4D9=_region_malloc(_tmp1AC,sizeof(*_tmp4D9)),((_tmp4D9->rgn=_tmp1AC,((_tmp4D9->places=0,_tmp4D9)))));
Cyc_CfFlowInfo_add_places(pile,r);
return Cyc_CfFlowInfo_escape_these(fenv,pile,all_changed,d);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;struct Cyc_Dict_Dict*all_changed;};static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval);static void _tmp4E6(struct Cyc_CfFlowInfo_AssignEnv**env,unsigned int*_tmp4E5,unsigned int*_tmp4E4,void***_tmp4E2,struct _dyneither_ptr*_tmp1BE,struct _dyneither_ptr*_tmp1BB){for(*_tmp4E5=0;*_tmp4E5 < *_tmp4E4;(*_tmp4E5)++){(*_tmp4E2)[*_tmp4E5]=(void*)
# 755
Cyc_CfFlowInfo_assign_place_inner(*env,((void**)(*_tmp1BB).curr)[(int)*_tmp4E5],*((void**)_check_dyneither_subscript(*_tmp1BE,sizeof(void*),(int)*_tmp4E5)));}}
# 735
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 740
struct _tuple0 _tmp4DA;struct _tuple0 _tmp1AE=(_tmp4DA.f1=oldval,((_tmp4DA.f2=newval,_tmp4DA)));struct _tuple0 _tmp1AF=_tmp1AE;void*_tmp1B1;struct Cyc_Absyn_Vardecl*_tmp1B3;void*_tmp1B4;struct Cyc_CfFlowInfo_Place*_tmp1B7;struct Cyc_CfFlowInfo_UnionRInfo _tmp1BA;struct _dyneither_ptr _tmp1BB;struct Cyc_CfFlowInfo_UnionRInfo _tmp1BD;struct _dyneither_ptr _tmp1BE;enum Cyc_CfFlowInfo_InitLevel _tmp1C0;_LLB4: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1B0=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f1;if(_tmp1B0->tag != 8)goto _LLB6;else{_tmp1B1=(void*)_tmp1B0->f2;}}_LLB5:
# 742
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp1B1,newval);_LLB6: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1B2=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f2;if(_tmp1B2->tag != 8)goto _LLB8;else{_tmp1B3=_tmp1B2->f1;_tmp1B4=(void*)_tmp1B2->f2;}}_LLB7: {
# 744
void*_tmp1C1=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp1B4);
if(_tmp1C1 == _tmp1B4)return newval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4DD;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4DC;return(void*)((_tmp4DC=_region_malloc((env->fenv)->r,sizeof(*_tmp4DC)),((_tmp4DC[0]=((_tmp4DD.tag=8,((_tmp4DD.f1=_tmp1B3,((_tmp4DD.f2=_tmp1C1,_tmp4DD)))))),_tmp4DC))));}}_LLB8:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1B5=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f1;if(_tmp1B5->tag != 4)goto _LLBA;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp1B6=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f2;if(_tmp1B6->tag != 5)goto _LLBA;else{_tmp1B7=_tmp1B6->f1;}};_LLB9:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp1B7);goto _LLBB;_LLBA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1B8=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f1;if(_tmp1B8->tag != 4)goto _LLBC;}_LLBB:
# 749
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp4E0;void*_tmp4DF;(_tmp4DF=0,Cyc_CfFlowInfo_aerr(env->loc,((_tmp4E0="assignment puts possibly-uninitialized data in an escaped location",_tag_dyneither(_tmp4E0,sizeof(char),67))),_tag_dyneither(_tmp4DF,sizeof(void*),0)));}
# 752
return(env->fenv)->esc_all;_LLBC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1B9=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f1;if(_tmp1B9->tag != 6)goto _LLBE;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1BC=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f2;if(_tmp1BC->tag != 6)goto _LLBE;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}};_LLBD: {
# 754
unsigned int _tmp4E5;unsigned int _tmp4E4;struct _dyneither_ptr _tmp4E3;void**_tmp4E2;unsigned int _tmp4E1;struct _dyneither_ptr new_d=
(_tmp4E1=_get_dyneither_size(_tmp1BB,sizeof(void*)),((_tmp4E2=(void**)_region_malloc((env->fenv)->r,_check_times(sizeof(void*),_tmp4E1)),((_tmp4E3=_tag_dyneither(_tmp4E2,sizeof(void*),_tmp4E1),((((_tmp4E4=_tmp4E1,_tmp4E6(& env,& _tmp4E5,& _tmp4E4,& _tmp4E2,& _tmp1BE,& _tmp1BB))),_tmp4E3)))))));
# 758
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1BB,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp1BB.curr)[i]){
change=1;break;}}}
# 763
if(!change){
if(!_tmp1BA.is_union)return oldval;
new_d=_tmp1BB;}else{
# 768
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp1BB,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp1BE,sizeof(void*),i))){
change=1;break;}}}
# 773
if(!change){
if(!_tmp1BA.is_union)return newval;
new_d=_tmp1BE;}}{
# 778
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4E9;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4E8;return(void*)((_tmp4E8=_region_malloc((env->fenv)->r,sizeof(*_tmp4E8)),((_tmp4E8[0]=((_tmp4E9.tag=6,((_tmp4E9.f1=_tmp1BD,((_tmp4E9.f2=new_d,_tmp4E9)))))),_tmp4E8))));};}_LLBE: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp1BF=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1AF.f2;if(_tmp1BF->tag != 4)goto _LLC0;else{_tmp1C0=_tmp1BF->f1;}}_LLBF:
# 780
 switch(_tmp1C0){case Cyc_CfFlowInfo_NoneIL: _LLC2:
 return(env->fenv)->unknown_none;case Cyc_CfFlowInfo_ThisIL: _LLC3:
 return(env->fenv)->unknown_this;case Cyc_CfFlowInfo_AllIL: _LLC4:
 return(env->fenv)->unknown_all;}_LLC0:;_LLC1:
# 785
 return newval;_LLB3:;}
# 791
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 795
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*fs,int fs_prefix,void*oldval,void*newval){
# 800
if(fs == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple19 _tmp4EA;struct _tuple19 _tmp1CD=(_tmp4EA.f1=fs,((_tmp4EA.f2=oldval,_tmp4EA)));struct _tuple19 _tmp1CE=_tmp1CD;struct Cyc_Absyn_Vardecl*_tmp1D0;void*_tmp1D1;int _tmp1D2;struct Cyc_List_List*_tmp1D3;int _tmp1D5;int _tmp1D6;struct _dyneither_ptr _tmp1D7;_LLC7: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1CF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1CE.f2;if(_tmp1CF->tag != 8)goto _LLC9;else{_tmp1D0=_tmp1CF->f1;_tmp1D1=(void*)_tmp1CF->f2;}}_LLC8: {
# 803
void*_tmp1D8=Cyc_CfFlowInfo_assign_place_outer(env,fs,fs_prefix,_tmp1D1,newval);
if(_tmp1D8 == _tmp1D1)return oldval;else{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp4ED;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp4EC;return(void*)((_tmp4EC=_region_malloc((env->fenv)->r,sizeof(*_tmp4EC)),((_tmp4EC[0]=((_tmp4ED.tag=8,((_tmp4ED.f1=_tmp1D0,((_tmp4ED.f2=_tmp1D8,_tmp4ED)))))),_tmp4EC))));}}_LLC9: if(_tmp1CE.f1 == 0)goto _LLCB;_tmp1D2=(int)(_tmp1CE.f1)->hd;_tmp1D3=(_tmp1CE.f1)->tl;{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp1D4=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1CE.f2;if(_tmp1D4->tag != 6)goto _LLCB;else{_tmp1D5=(_tmp1D4->f1).is_union;_tmp1D6=(_tmp1D4->f1).fieldnum;_tmp1D7=_tmp1D4->f2;}};_LLCA: {
# 807
void*_tmp1DB=Cyc_CfFlowInfo_assign_place_outer(env,_tmp1D3,fs_prefix + 1,*((void**)_check_dyneither_subscript(_tmp1D7,sizeof(void*),_tmp1D2)),newval);
# 809
struct _dyneither_ptr _tmp1DC=Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1D7,_tmp1D2,_tmp1DB);
if((void**)_tmp1DC.curr == (void**)_tmp1D7.curr  && (!_tmp1D5  || _tmp1D6 == _tmp1D2))return oldval;
_tmp1D7=_tmp1DC;
# 814
if(_tmp1D5){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp1D7,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp1D2){
struct _dyneither_ptr _tmp1DD=
Cyc_CfFlowInfo_aggr_dict_insert((env->fenv)->r,_tmp1D7,i,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->r,(env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp1D7,sizeof(void*),i))));
# 824
if((void**)_tmp1DD.curr != (void**)_tmp1D7.curr){
_tmp1D7=_tmp1DD;
changed=1;}}}}
# 832
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp1DE=env->root_place;void*_tmp1E0;struct Cyc_List_List*_tmp1E1;struct Cyc_CfFlowInfo_Place*_tmp1DF=_tmp1DE;_tmp1E0=_tmp1DF->root;_tmp1E1=_tmp1DF->fields;{
int*_tmp4EE;struct Cyc_List_List*_tmp1E2=((struct Cyc_List_List*(*)(struct _RegionHandle*r,int(*f)(int*,int),int*env,struct Cyc_List_List*x))Cyc_List_rfilter_c)((env->pile)->rgn,(int(*)(int*n,int unused))Cyc_CfFlowInfo_nprefix,((_tmp4EE=_cycalloc_atomic(sizeof(*_tmp4EE)),((_tmp4EE[0]=fs_prefix,_tmp4EE)))),_tmp1E1);
struct Cyc_CfFlowInfo_Place*_tmp4EF;struct Cyc_CfFlowInfo_Place*_tmp1E3=(_tmp4EF=_region_malloc((env->pile)->rgn,sizeof(*_tmp4EF)),((_tmp4EF->root=_tmp1E0,((_tmp4EF->fields=_tmp1E2,_tmp4EF)))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,_tmp1E3,0);};}}{
# 839
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp4F5;struct Cyc_CfFlowInfo_UnionRInfo _tmp4F4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4F3;return(void*)((_tmp4F3=_region_malloc((env->fenv)->r,sizeof(*_tmp4F3)),((_tmp4F3[0]=((_tmp4F5.tag=6,((_tmp4F5.f1=((_tmp4F4.is_union=_tmp1D5,((_tmp4F4.fieldnum=_tmp1D2,_tmp4F4)))),((_tmp4F5.f2=_tmp1D7,_tmp4F5)))))),_tmp4F3))));};}_LLCB:;_LLCC: {
struct Cyc_Core_Impossible_exn_struct _tmp4FB;const char*_tmp4FA;struct Cyc_Core_Impossible_exn_struct*_tmp4F9;(int)_throw((void*)((_tmp4F9=_cycalloc(sizeof(*_tmp4F9)),((_tmp4F9[0]=((_tmp4FB.tag=Cyc_Core_Impossible,((_tmp4FB.f1=((_tmp4FA="bad assign place",_tag_dyneither(_tmp4FA,sizeof(char),17))),_tmp4FB)))),_tmp4F9)))));}_LLC6:;};}
# 843
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r){
# 854
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(all_changed,place,0);{
struct _RegionHandle*_tmp1ED=fenv->r;
void*_tmp1EF;struct Cyc_List_List*_tmp1F0;struct Cyc_CfFlowInfo_Place*_tmp1EE=place;_tmp1EF=_tmp1EE->root;_tmp1F0=_tmp1EE->fields;{
struct Cyc_CfFlowInfo_EscPile*_tmp4FE;struct Cyc_CfFlowInfo_AssignEnv _tmp4FD;struct Cyc_CfFlowInfo_AssignEnv env=(_tmp4FD.fenv=fenv,((_tmp4FD.pile=((_tmp4FE=_region_malloc(_tmp1ED,sizeof(*_tmp4FE)),((_tmp4FE->rgn=_tmp1ED,((_tmp4FE->places=0,_tmp4FE)))))),((_tmp4FD.d=d,((_tmp4FD.loc=loc,((_tmp4FD.root_place=place,((_tmp4FD.all_changed=all_changed,_tmp4FD)))))))))));
void*newval=Cyc_CfFlowInfo_assign_place_outer(& env,_tmp1F0,0,((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp1EF),r);
return Cyc_CfFlowInfo_escape_these(fenv,env.pile,all_changed,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp1EF,newval));};};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 870
typedef struct Cyc_CfFlowInfo_JoinEnv*Cyc_CfFlowInfo_joinenv_t;
enum Cyc_CfFlowInfo_WhoIsChanged{Cyc_CfFlowInfo_Neither  = 0,Cyc_CfFlowInfo_One  = 1,Cyc_CfFlowInfo_Two  = 2};struct Cyc_CfFlowInfo_AfterEnv{struct Cyc_CfFlowInfo_JoinEnv joinenv;struct Cyc_Dict_Dict chg1;struct Cyc_Dict_Dict chg2;struct Cyc_CfFlowInfo_Place*curr_place;struct Cyc_List_List**last_field_cell;enum Cyc_CfFlowInfo_WhoIsChanged changed;};
# 880
typedef struct Cyc_CfFlowInfo_AfterEnv*Cyc_CfFlowInfo_afterenv_t;
# 882
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple21{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2);static void _tmp511(struct Cyc_CfFlowInfo_JoinEnv**env,unsigned int*_tmp510,unsigned int*_tmp50F,void***_tmp50D,struct _dyneither_ptr*_tmp231,struct _dyneither_ptr*_tmp22D){for(*_tmp510=0;*_tmp510 < *_tmp50F;(*_tmp510)++){(*_tmp50D)[*_tmp510]=(void*)
# 957 "cf_flowinfo.cyc"
((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(*env,0,((void**)(*_tmp22D).curr)[(int)*_tmp510],*((void**)_check_dyneither_subscript(*_tmp231,sizeof(void*),(int)*_tmp510)));}}
# 892
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;{
struct _RegionHandle*_tmp1F3=(env->fenv)->r;
# 896
{struct _tuple0 _tmp4FF;struct _tuple0 _tmp1F4=(_tmp4FF.f1=r1,((_tmp4FF.f2=r2,_tmp4FF)));struct _tuple0 _tmp1F5=_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F7;void*_tmp1F8;struct Cyc_Absyn_Vardecl*_tmp1FA;void*_tmp1FB;struct Cyc_Absyn_Vardecl*_tmp1FD;void*_tmp1FE;struct Cyc_Absyn_Vardecl*_tmp200;void*_tmp201;struct Cyc_Absyn_Exp*_tmp203;int _tmp204;void*_tmp205;struct Cyc_Absyn_Exp*_tmp207;int _tmp208;void*_tmp209;struct Cyc_Absyn_Exp*_tmp20B;int _tmp20C;void*_tmp20D;struct Cyc_Absyn_Exp*_tmp20F;int _tmp210;void*_tmp211;struct Cyc_CfFlowInfo_Place*_tmp213;struct Cyc_CfFlowInfo_Place*_tmp215;struct Cyc_CfFlowInfo_Place*_tmp217;struct Cyc_CfFlowInfo_Place*_tmp21B;struct Cyc_CfFlowInfo_Place*_tmp21D;struct Cyc_CfFlowInfo_Place*_tmp221;struct Cyc_CfFlowInfo_Place*_tmp223;struct Cyc_CfFlowInfo_Place*_tmp225;int _tmp22B;int _tmp22C;struct _dyneither_ptr _tmp22D;int _tmp22F;int _tmp230;struct _dyneither_ptr _tmp231;_LLCE:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp1F6->tag != 8)goto _LLD0;else{_tmp1F7=_tmp1F6->f1;_tmp1F8=(void*)_tmp1F6->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1F9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp1F9->tag != 8)goto _LLD0;else{_tmp1FA=_tmp1F9->f1;_tmp1FB=(void*)_tmp1F9->f2;}};_LLCF:
# 899
 if(_tmp1F7 == _tmp1FA){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp502;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp501;return(void*)((_tmp501=_region_malloc(_tmp1F3,sizeof(*_tmp501)),((_tmp501[0]=((_tmp502.tag=8,((_tmp502.f1=_tmp1F7,((_tmp502.f2=Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F8,_tmp1FB),_tmp502)))))),_tmp501))));}else{
# 902
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1F8,_tmp1FB);}_LLD0: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1FC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp1FC->tag != 8)goto _LLD2;else{_tmp1FD=_tmp1FC->f1;_tmp1FE=(void*)_tmp1FC->f2;}}_LLD1:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp1FE,r2);_LLD2: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp1FF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp1FF->tag != 8)goto _LLD4;else{_tmp200=_tmp1FF->f1;_tmp201=(void*)_tmp1FF->f2;}}_LLD3:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp201);_LLD4:{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp202=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp202->tag != 7)goto _LLD6;else{_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;_tmp205=(void*)_tmp202->f3;}}{struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp206=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp206->tag != 7)goto _LLD6;else{_tmp207=_tmp206->f1;_tmp208=_tmp206->f2;_tmp209=(void*)_tmp206->f3;}};_LLD5:
# 907
 if(_tmp203 == _tmp207){
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp505;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp504;return(void*)((_tmp504=_region_malloc(_tmp1F3,sizeof(*_tmp504)),((_tmp504[0]=((_tmp505.tag=7,((_tmp505.f1=_tmp203,((_tmp505.f2=_tmp204 > _tmp208?_tmp204: _tmp208,((_tmp505.f3=
Cyc_CfFlowInfo_join_absRval(env,a,_tmp205,_tmp209),_tmp505)))))))),_tmp504))));}
{void*_tmp236=_tmp205;struct Cyc_CfFlowInfo_Place*_tmp238;_LLF1: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp237=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp236;if(_tmp237->tag != 5)goto _LLF3;else{_tmp238=_tmp237->f1;}}_LLF2:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp238);goto _LLF0;_LLF3:;_LLF4:
 goto _LLF0;_LLF0:;}
# 914
{void*_tmp239=_tmp209;struct Cyc_CfFlowInfo_Place*_tmp23B;_LLF6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp23A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp239;if(_tmp23A->tag != 5)goto _LLF8;else{_tmp23B=_tmp23A->f1;}}_LLF7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp23B);goto _LLF5;_LLF8:;_LLF9:
 goto _LLF5;_LLF5:;}
# 918
goto _LLCD;_LLD6: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp20A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp20A->tag != 7)goto _LLD8;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_tmp20D=(void*)_tmp20A->f3;}}_LLD7: {
# 920
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp508;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp507;return(void*)((_tmp507=_region_malloc(_tmp1F3,sizeof(*_tmp507)),((_tmp507[0]=((_tmp508.tag=7,((_tmp508.f1=_tmp20B,((_tmp508.f2=_tmp20C,((_tmp508.f3=Cyc_CfFlowInfo_join_absRval(env,a,_tmp20D,r2),_tmp508)))))))),_tmp507))));}_LLD8: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp20E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp20E->tag != 7)goto _LLDA;else{_tmp20F=_tmp20E->f1;_tmp210=_tmp20E->f2;_tmp211=(void*)_tmp20E->f3;}}_LLD9: {
# 922
struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct _tmp50B;struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp50A;return(void*)((_tmp50A=_region_malloc(_tmp1F3,sizeof(*_tmp50A)),((_tmp50A[0]=((_tmp50B.tag=7,((_tmp50B.f1=_tmp20F,((_tmp50B.f2=_tmp210,((_tmp50B.f3=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp211),_tmp50B)))))))),_tmp50A))));}_LLDA:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp212=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp212->tag != 5)goto _LLDC;else{_tmp213=_tmp212->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp214=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp214->tag != 5)goto _LLDC;else{_tmp215=_tmp214->f1;}};_LLDB:
# 925
 if(Cyc_CfFlowInfo_place_cmp(_tmp213,_tmp215)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp213);
Cyc_CfFlowInfo_add_place(env->pile,_tmp215);
goto _LLCD;_LLDC:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp216=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp216->tag != 5)goto _LLDE;else{_tmp217=_tmp216->f1;}}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp218=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp218->tag != 2)goto _LLDE;};_LLDD:
# 930
 _tmp21B=_tmp217;goto _LLDF;_LLDE:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp219=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp219->tag != 2)goto _LLE0;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp21A=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp21A->tag != 5)goto _LLE0;else{_tmp21B=_tmp21A->f1;}};_LLDF:
# 932
 Cyc_CfFlowInfo_add_place(env->pile,_tmp21B);
return(env->fenv)->notzerothis;_LLE0:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp21C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp21C->tag != 5)goto _LLE2;else{_tmp21D=_tmp21C->f1;}}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp21E=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp21E->tag != 1)goto _LLE2;};_LLE1:
# 936
 Cyc_CfFlowInfo_add_place(env->pile,_tmp21D);{
enum Cyc_CfFlowInfo_InitLevel _tmp240=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);switch(_tmp240){case Cyc_CfFlowInfo_AllIL: _LLFA:
 return(env->fenv)->notzeroall;default: _LLFB:
 return(env->fenv)->notzerothis;}};_LLE2:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp21F=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp21F->tag != 1)goto _LLE4;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp220=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp220->tag != 5)goto _LLE4;else{_tmp221=_tmp220->f1;}};_LLE3:
# 942
 Cyc_CfFlowInfo_add_place(env->pile,_tmp221);{
enum Cyc_CfFlowInfo_InitLevel _tmp241=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);switch(_tmp241){case Cyc_CfFlowInfo_AllIL: _LLFD:
 return(env->fenv)->notzeroall;default: _LLFE:
 return(env->fenv)->notzerothis;}};_LLE4: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp222=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp222->tag != 5)goto _LLE6;else{_tmp223=_tmp222->f1;}}_LLE5:
# 948
 Cyc_CfFlowInfo_add_place(env->pile,_tmp223);goto _LLCD;_LLE6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp224=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp224->tag != 5)goto _LLE8;else{_tmp225=_tmp224->f1;}}_LLE7:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp225);goto _LLCD;_LLE8:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp226=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp226->tag != 1)goto _LLEA;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp227=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp227->tag != 2)goto _LLEA;};_LLE9:
# 952
 goto _LLEB;_LLEA:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp228=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp228->tag != 2)goto _LLEC;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp229=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp229->tag != 1)goto _LLEC;};_LLEB:
 return(env->fenv)->notzerothis;_LLEC:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp22A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f1;if(_tmp22A->tag != 6)goto _LLEE;else{_tmp22B=(_tmp22A->f1).is_union;_tmp22C=(_tmp22A->f1).fieldnum;_tmp22D=_tmp22A->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp22E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1F5.f2;if(_tmp22E->tag != 6)goto _LLEE;else{_tmp22F=(_tmp22E->f1).is_union;_tmp230=(_tmp22E->f1).fieldnum;_tmp231=_tmp22E->f2;}};_LLED: {
# 956
unsigned int _tmp510;unsigned int _tmp50F;struct _dyneither_ptr _tmp50E;void**_tmp50D;unsigned int _tmp50C;struct _dyneither_ptr new_d=
(_tmp50C=_get_dyneither_size(_tmp22D,sizeof(void*)),((_tmp50D=(void**)_region_malloc(_tmp1F3,_check_times(sizeof(void*),_tmp50C)),((_tmp50E=_tag_dyneither(_tmp50D,sizeof(void*),_tmp50C),((((_tmp50F=_tmp50C,_tmp511(& env,& _tmp510,& _tmp50F,& _tmp50D,& _tmp231,& _tmp22D))),_tmp50E)))))));
# 961
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp22D,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp22D.curr)[i]){
change=1;break;}}}
# 966
if(!change){
if(!_tmp22B)return r1;
new_d=_tmp22D;}else{
# 971
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp22D,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp231,sizeof(void*),i))){
change=1;break;}}}
# 976
if(!change){
if(!_tmp22B)return r2;
new_d=_tmp231;}}{
# 981
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp517;struct Cyc_CfFlowInfo_UnionRInfo _tmp516;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp515;return(void*)((_tmp515=_region_malloc(_tmp1F3,sizeof(*_tmp515)),((_tmp515[0]=((_tmp517.tag=6,((_tmp517.f1=((_tmp516.is_union=_tmp22B,((_tmp516.fieldnum=_tmp22C == _tmp230?_tmp22C: - 1,_tmp516)))),((_tmp517.f2=new_d,_tmp517)))))),_tmp515))));};}_LLEE:;_LLEF:
# 983
 goto _LLCD;_LLCD:;}{
# 985
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple0 _tmp518;struct _tuple0 _tmp24A=(_tmp518.f1=r1,((_tmp518.f2=r2,_tmp518)));struct _tuple0 _tmp24B=_tmp24A;_LL101: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp24C=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp24B.f1;if(_tmp24C->tag != 4)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp24D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp24B.f2;if(_tmp24D->tag != 4)goto _LL105;}_LL104: {
# 990
struct _tuple21 _tmp519;struct _tuple21 _tmp24E=(_tmp519.f1=il1,((_tmp519.f2=il2,_tmp519)));struct _tuple21 _tmp24F=_tmp24E;_LL108: if(_tmp24F.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL10A;_LL109:
 goto _LL10B;_LL10A: if(_tmp24F.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL10C;_LL10B: return(env->fenv)->esc_none;_LL10C: if(_tmp24F.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL10E;_LL10D:
 goto _LL10F;_LL10E: if(_tmp24F.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL110;_LL10F: return(env->fenv)->esc_this;_LL110:;_LL111:
 return(env->fenv)->esc_all;_LL107:;}_LL105:;_LL106: {
# 996
struct _tuple21 _tmp51A;struct _tuple21 _tmp251=(_tmp51A.f1=il1,((_tmp51A.f2=il2,_tmp51A)));struct _tuple21 _tmp252=_tmp251;_LL113: if(_tmp252.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL115;_LL114:
 goto _LL116;_LL115: if(_tmp252.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL117;_LL116: return(env->fenv)->unknown_none;_LL117: if(_tmp252.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL119;_LL118:
 goto _LL11A;_LL119: if(_tmp252.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL11B;_LL11A: return(env->fenv)->unknown_this;_LL11B:;_LL11C:
 return(env->fenv)->unknown_all;_LL112:;}_LL100:;};};}struct _tuple22{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 1004
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1007
struct _tuple22 _tmp51B;struct _tuple22 _tmp255=(_tmp51B.f1=f1,((_tmp51B.f2=f2,_tmp51B)));struct _tuple22 _tmp256=_tmp255;struct Cyc_Dict_Dict _tmp257;struct Cyc_List_List*_tmp258;struct Cyc_Dict_Dict _tmp259;struct Cyc_List_List*_tmp25A;_LL11E: if(((_tmp256.f1).BottomFL).tag != 1)goto _LL120;_LL11F:
 return f2;_LL120: if(((_tmp256.f2).BottomFL).tag != 1)goto _LL122;_LL121:
 return f1;_LL122: if(((_tmp256.f1).ReachableFL).tag != 2)goto _LL11D;_tmp257=((struct _tuple11)((_tmp256.f1).ReachableFL).val).f1;_tmp258=((struct _tuple11)((_tmp256.f1).ReachableFL).val).f2;if(((_tmp256.f2).ReachableFL).tag != 2)goto _LL11D;_tmp259=((struct _tuple11)((_tmp256.f2).ReachableFL).val).f1;_tmp25A=((struct _tuple11)((_tmp256.f2).ReachableFL).val).f2;_LL123:
# 1013
 if(_tmp257.t == _tmp259.t  && _tmp258 == _tmp25A)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct _RegionHandle*_tmp25B=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp51E;struct Cyc_CfFlowInfo_JoinEnv _tmp51D;struct Cyc_CfFlowInfo_JoinEnv _tmp25C=(_tmp51D.fenv=fenv,((_tmp51D.pile=((_tmp51E=_region_malloc(_tmp25B,sizeof(*_tmp51E)),((_tmp51E->rgn=_tmp25B,((_tmp51E->places=0,_tmp51E)))))),((_tmp51D.d1=_tmp257,((_tmp51D.d2=_tmp259,_tmp51D)))))));
struct Cyc_Dict_Dict _tmp25D=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp25C,_tmp257,_tmp259);
struct Cyc_List_List*_tmp25E=Cyc_Relations_join_relns(fenv->r,_tmp258,_tmp25A);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp25C.pile,all_changed,_tmp25D),_tmp25E);};_LL11D:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 1025
struct _tuple13 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,struct _tuple13 pr1,struct _tuple13 pr2){
# 1032
union Cyc_CfFlowInfo_FlowInfo _tmp263;void*_tmp264;struct _tuple13 _tmp262=pr1;_tmp263=_tmp262.f1;_tmp264=_tmp262.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp266;void*_tmp267;struct _tuple13 _tmp265=pr2;_tmp266=_tmp265.f1;_tmp267=_tmp265.f2;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,all_changed,_tmp263,_tmp266);
struct _tuple23 _tmp51F;struct _tuple23 _tmp268=(_tmp51F.f1=_tmp263,((_tmp51F.f2=_tmp266,((_tmp51F.f3=outflow,_tmp51F)))));struct _tuple23 _tmp269=_tmp268;struct Cyc_Dict_Dict _tmp26A;struct Cyc_Dict_Dict _tmp26B;struct Cyc_Dict_Dict _tmp26C;struct Cyc_List_List*_tmp26D;_LL125: if(((_tmp269.f1).BottomFL).tag != 1)goto _LL127;_LL126: {
struct _tuple13 _tmp520;return(_tmp520.f1=outflow,((_tmp520.f2=_tmp267,_tmp520)));}_LL127: if(((_tmp269.f2).BottomFL).tag != 1)goto _LL129;_LL128: {
struct _tuple13 _tmp521;return(_tmp521.f1=outflow,((_tmp521.f2=_tmp264,_tmp521)));}_LL129: if(((_tmp269.f1).ReachableFL).tag != 2)goto _LL12B;_tmp26A=((struct _tuple11)((_tmp269.f1).ReachableFL).val).f1;if(((_tmp269.f2).ReachableFL).tag != 2)goto _LL12B;_tmp26B=((struct _tuple11)((_tmp269.f2).ReachableFL).val).f1;if(((_tmp269.f3).ReachableFL).tag != 2)goto _LL12B;_tmp26C=((struct _tuple11)((_tmp269.f3).ReachableFL).val).f1;_tmp26D=((struct _tuple11)((_tmp269.f3).ReachableFL).val).f2;_LL12A:
# 1040
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp264,_tmp267)){struct _tuple13 _tmp522;return(_tmp522.f1=outflow,((_tmp522.f2=_tmp267,_tmp522)));}
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp267,_tmp264)){struct _tuple13 _tmp523;return(_tmp523.f1=outflow,((_tmp523.f2=_tmp264,_tmp523)));}{
struct _RegionHandle*_tmp272=fenv->r;
struct Cyc_CfFlowInfo_EscPile*_tmp526;struct Cyc_CfFlowInfo_JoinEnv _tmp525;struct Cyc_CfFlowInfo_JoinEnv _tmp273=(_tmp525.fenv=fenv,((_tmp525.pile=((_tmp526=_region_malloc(_tmp272,sizeof(*_tmp526)),((_tmp526->rgn=_tmp272,((_tmp526->places=0,_tmp526)))))),((_tmp525.d1=_tmp26A,((_tmp525.d2=_tmp26B,_tmp525)))))));
void*_tmp274=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp273,0,_tmp264,_tmp267);
struct _tuple13 _tmp527;return(_tmp527.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,_tmp273.pile,all_changed,_tmp26C),_tmp26D),((_tmp527.f2=_tmp274,_tmp527)));};_LL12B:;_LL12C: {
# 1048
struct Cyc_Core_Impossible_exn_struct _tmp52D;const char*_tmp52C;struct Cyc_Core_Impossible_exn_struct*_tmp52B;(int)_throw((void*)((_tmp52B=_cycalloc(sizeof(*_tmp52B)),((_tmp52B[0]=((_tmp52D.tag=Cyc_Core_Impossible,((_tmp52D.f1=((_tmp52C="join_flow_and_rval: BottomFL outflow",_tag_dyneither(_tmp52C,sizeof(char),37))),_tmp52D)))),_tmp52B)))));}_LL124:;};};}
# 1052
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*,int,void*,void*);static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2);static void _tmp537(struct Cyc_CfFlowInfo_AfterEnv**env,unsigned int*_tmp536,unsigned int*_tmp535,void***_tmp533,struct _dyneither_ptr*_tmp28F,struct _dyneither_ptr*_tmp28B){for(*_tmp536=0;*_tmp536 < *_tmp535;(*_tmp536)++){(*_tmp533)[*_tmp536]=(void*)
# 1093
Cyc_CfFlowInfo_after_absRval_d(*env,(int)*_tmp536,((void**)(*_tmp28B).curr)[(int)*_tmp536],*((void**)_check_dyneither_subscript(*_tmp28F,sizeof(void*),(int)*_tmp536)));}}
# 1053
static void*Cyc_CfFlowInfo_after_absRval(struct Cyc_CfFlowInfo_AfterEnv*env,void*r1,void*r2){
int changed1=env->changed == Cyc_CfFlowInfo_One  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg1,env->curr_place);
int changed2=env->changed == Cyc_CfFlowInfo_Two  || ((int(*)(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*k))Cyc_Dict_member)(env->chg2,env->curr_place);
if(changed1  && changed2)
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);
# 1061
if(changed1){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg2))
return r1;
env->changed=Cyc_CfFlowInfo_One;}
# 1066
if(changed2){
if(!Cyc_CfFlowInfo_prefix_of_member(env->curr_place,env->chg1))
return r2;
env->changed=Cyc_CfFlowInfo_Two;}{
# 1074
struct _tuple0 _tmp52E;struct _tuple0 _tmp27C=(_tmp52E.f1=r1,((_tmp52E.f2=r2,_tmp52E)));struct _tuple0 _tmp27D=_tmp27C;struct Cyc_Absyn_Vardecl*_tmp281;void*_tmp282;struct Cyc_Absyn_Vardecl*_tmp284;void*_tmp285;int _tmp289;int _tmp28A;struct _dyneither_ptr _tmp28B;int _tmp28D;int _tmp28E;struct _dyneither_ptr _tmp28F;_LL12E: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp27E=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp27D.f1;if(_tmp27E->tag != 7)goto _LL130;}_LL12F:
# 1076
 goto _LL131;_LL130: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp27F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp27D.f2;if(_tmp27F->tag != 7)goto _LL132;}_LL131:
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL132:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp280=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp27D.f1;if(_tmp280->tag != 8)goto _LL134;else{_tmp281=_tmp280->f1;_tmp282=(void*)_tmp280->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp283=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp27D.f2;if(_tmp283->tag != 8)goto _LL134;else{_tmp284=_tmp283->f1;_tmp285=(void*)_tmp283->f2;}};_LL133:
# 1079
 if(_tmp281 == _tmp284){
void*_tmp290=Cyc_CfFlowInfo_after_absRval(env,_tmp282,_tmp285);
if(_tmp290 == _tmp282)return r1;
if(_tmp290 == _tmp285)return r2;{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp531;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp530;return(void*)((_tmp530=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp530)),((_tmp530[0]=((_tmp531.tag=8,((_tmp531.f1=_tmp281,((_tmp531.f2=_tmp290,_tmp531)))))),_tmp530))));};}else{
# 1085
return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);}_LL134: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp286=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp27D.f2;if(_tmp286->tag != 8)goto _LL136;}_LL135:
 goto _LL137;_LL136: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp287=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp27D.f1;if(_tmp287->tag != 8)goto _LL138;}_LL137:
# 1088
 return((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& env->joinenv,0,r1,r2);_LL138:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp288=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp27D.f1;if(_tmp288->tag != 6)goto _LL13A;else{_tmp289=(_tmp288->f1).is_union;_tmp28A=(_tmp288->f1).fieldnum;_tmp28B=_tmp288->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp28C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp27D.f2;if(_tmp28C->tag != 6)goto _LL13A;else{_tmp28D=(_tmp28C->f1).is_union;_tmp28E=(_tmp28C->f1).fieldnum;_tmp28F=_tmp28C->f2;}};_LL139: {
# 1091
unsigned int _tmp536;unsigned int _tmp535;struct _dyneither_ptr _tmp534;void**_tmp533;unsigned int _tmp532;struct _dyneither_ptr new_d=
(_tmp532=
_get_dyneither_size(_tmp28B,sizeof(void*)),((_tmp533=(void**)_region_malloc(((env->joinenv).fenv)->r,_check_times(sizeof(void*),_tmp532)),((_tmp534=_tag_dyneither(_tmp533,sizeof(void*),_tmp532),((((_tmp535=_tmp532,_tmp537(& env,& _tmp536,& _tmp535,& _tmp533,& _tmp28F,& _tmp28B))),_tmp534)))))));
# 1098
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp28B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= ((void**)_tmp28B.curr)[i]){
change=1;break;}}}
# 1103
if(!change)
return r1;
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp28B,sizeof(void*));++ i){
if(*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i))!= *((void**)_check_dyneither_subscript(_tmp28F,sizeof(void*),i))){
change=1;break;}}}
# 1110
if(!change)
return r2;{
# 1114
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp53D;struct Cyc_CfFlowInfo_UnionRInfo _tmp53C;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp53B;return(void*)((_tmp53B=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53D.tag=6,((_tmp53D.f1=((_tmp53C.is_union=_tmp289,((_tmp53C.fieldnum=_tmp28A == _tmp28E?_tmp28A: - 1,_tmp53C)))),((_tmp53D.f2=new_d,_tmp53D)))))),_tmp53B))));};}_LL13A:;_LL13B: {
# 1122
struct Cyc_Core_Impossible_exn_struct _tmp543;const char*_tmp542;struct Cyc_Core_Impossible_exn_struct*_tmp541;(int)_throw((void*)((_tmp541=_cycalloc(sizeof(*_tmp541)),((_tmp541[0]=((_tmp543.tag=Cyc_Core_Impossible,((_tmp543.f1=((_tmp542="after_absRval -- non-aggregates!",_tag_dyneither(_tmp542,sizeof(char),33))),_tmp543)))),_tmp541)))));}_LL12D:;};}
# 1125
static void*Cyc_CfFlowInfo_after_absRval_d(struct Cyc_CfFlowInfo_AfterEnv*env,int key,void*r1,void*r2){
# 1128
if(r1 == r2)return r1;{
struct Cyc_List_List**_tmp29E=env->last_field_cell;
enum Cyc_CfFlowInfo_WhoIsChanged _tmp29F=env->changed;
{struct Cyc_List_List*_tmp544;*env->last_field_cell=((_tmp544=_region_malloc(((env->joinenv).fenv)->r,sizeof(*_tmp544)),((_tmp544->hd=(void*)key,((_tmp544->tl=0,_tmp544))))));}
# 1134
env->last_field_cell=&((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl;{
void*_tmp2A1=Cyc_CfFlowInfo_after_absRval(env,r1,r2);
env->last_field_cell=_tmp29E;
((struct Cyc_List_List*)_check_null(*env->last_field_cell))->tl=0;
env->changed=_tmp29F;
return _tmp2A1;};};}
# 1141
static void*Cyc_CfFlowInfo_after_root(struct Cyc_CfFlowInfo_AfterEnv*env,void*root,void*r1,void*r2){
# 1143
if(r1 == r2)return r1;
{struct Cyc_CfFlowInfo_Place _tmp545;*env->curr_place=((_tmp545.root=root,((_tmp545.fields=0,_tmp545))));}
env->last_field_cell=&(env->curr_place)->fields;
env->changed=Cyc_CfFlowInfo_Neither;
return Cyc_CfFlowInfo_after_absRval(env,r1,r2);}
# 1150
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict*all_changed,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2,struct Cyc_Dict_Dict chg1,struct Cyc_Dict_Dict chg2){
# 1156
struct _tuple22 _tmp546;struct _tuple22 _tmp2A3=(_tmp546.f1=f1,((_tmp546.f2=f2,_tmp546)));struct _tuple22 _tmp2A4=_tmp2A3;struct Cyc_Dict_Dict _tmp2A5;struct Cyc_List_List*_tmp2A6;struct Cyc_Dict_Dict _tmp2A7;struct Cyc_List_List*_tmp2A8;_LL13D: if(((_tmp2A4.f1).BottomFL).tag != 1)goto _LL13F;_LL13E:
 goto _LL140;_LL13F: if(((_tmp2A4.f2).BottomFL).tag != 1)goto _LL141;_LL140:
 return Cyc_CfFlowInfo_BottomFL();_LL141: if(((_tmp2A4.f1).ReachableFL).tag != 2)goto _LL13C;_tmp2A5=((struct _tuple11)((_tmp2A4.f1).ReachableFL).val).f1;_tmp2A6=((struct _tuple11)((_tmp2A4.f1).ReachableFL).val).f2;if(((_tmp2A4.f2).ReachableFL).tag != 2)goto _LL13C;_tmp2A7=((struct _tuple11)((_tmp2A4.f2).ReachableFL).val).f1;_tmp2A8=((struct _tuple11)((_tmp2A4.f2).ReachableFL).val).f2;_LL142:
# 1160
 if(_tmp2A5.t == _tmp2A7.t  && _tmp2A6 == _tmp2A8)return f1;{
# 1163
struct _RegionHandle*_tmp2A9=fenv->r;
struct Cyc_CfFlowInfo_Place*_tmp2AA=fenv->dummy_place;
struct Cyc_CfFlowInfo_EscPile*_tmp54A;struct Cyc_CfFlowInfo_AfterEnv _tmp549;struct Cyc_CfFlowInfo_AfterEnv _tmp2AB=(_tmp549.joinenv=(((_tmp549.joinenv).fenv=fenv,(((_tmp549.joinenv).pile=((_tmp54A=_region_malloc(_tmp2A9,sizeof(*_tmp54A)),((_tmp54A->rgn=_tmp2A9,((_tmp54A->places=0,_tmp54A)))))),(((_tmp549.joinenv).d1=_tmp2A5,(((_tmp549.joinenv).d2=_tmp2A7,_tmp549.joinenv)))))))),((_tmp549.chg1=chg1,((_tmp549.chg2=chg2,((_tmp549.curr_place=_tmp2AA,((_tmp549.last_field_cell=& _tmp2AA->fields,((_tmp549.changed=Cyc_CfFlowInfo_Neither,_tmp549)))))))))));
# 1169
struct Cyc_Dict_Dict _tmp2AC=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_AfterEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_AfterEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_union_two_c)(Cyc_CfFlowInfo_after_root,& _tmp2AB,_tmp2A5,_tmp2A7);
struct Cyc_List_List*_tmp2AD=Cyc_Relations_join_relns(fenv->r,_tmp2A6,_tmp2A8);
return Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_escape_these(fenv,(_tmp2AB.joinenv).pile,all_changed,_tmp2AC),_tmp2AD);};_LL13C:;}
# 1178
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 1182
{struct _tuple0 _tmp54B;struct _tuple0 _tmp2B1=(_tmp54B.f1=r1,((_tmp54B.f2=r2,_tmp54B)));struct _tuple0 _tmp2B2=_tmp2B1;struct Cyc_Absyn_Vardecl*_tmp2B4;void*_tmp2B5;struct Cyc_Absyn_Vardecl*_tmp2B7;void*_tmp2B8;struct Cyc_CfFlowInfo_Place*_tmp2BC;struct Cyc_CfFlowInfo_Place*_tmp2BE;int _tmp2C2;int _tmp2C3;struct _dyneither_ptr _tmp2C4;int _tmp2C6;int _tmp2C7;struct _dyneither_ptr _tmp2C8;_LL144:{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B3=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2B3->tag != 8)goto _LL146;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=(void*)_tmp2B3->f2;}}{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B6=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2B6->tag != 8)goto _LL146;else{_tmp2B7=_tmp2B6->f1;_tmp2B8=(void*)_tmp2B6->f2;}};_LL145:
# 1184
 return _tmp2B4 == _tmp2B7  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp2B5,_tmp2B8);_LL146: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2B9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2B9->tag != 8)goto _LL148;}_LL147:
 goto _LL149;_LL148: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2BA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2BA->tag != 8)goto _LL14A;}_LL149:
 return 0;_LL14A:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2BB=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2BB->tag != 5)goto _LL14C;else{_tmp2BC=_tmp2BB->f1;}}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2BD=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2BD->tag != 5)goto _LL14C;else{_tmp2BE=_tmp2BD->f1;}};_LL14B:
 return Cyc_CfFlowInfo_place_cmp(_tmp2BC,_tmp2BE)== 0;_LL14C: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2BF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2BF->tag != 5)goto _LL14E;}_LL14D:
 goto _LL14F;_LL14E: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp2C0=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2C0->tag != 5)goto _LL150;}_LL14F:
 return 0;_LL150:{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C1=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2C1->tag != 6)goto _LL152;else{_tmp2C2=(_tmp2C1->f1).is_union;_tmp2C3=(_tmp2C1->f1).fieldnum;_tmp2C4=_tmp2C1->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2C5=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2C5->tag != 6)goto _LL152;else{_tmp2C6=(_tmp2C5->f1).is_union;_tmp2C7=(_tmp2C5->f1).fieldnum;_tmp2C8=_tmp2C5->f2;}};_LL151:
# 1192
 if(_tmp2C2  && _tmp2C3 != _tmp2C7)return 0;
if((void**)_tmp2C4.curr == (void**)_tmp2C8.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp2C4,sizeof(void*));++ i){
if(!((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,((void**)_tmp2C4.curr)[i],*((void**)_check_dyneither_subscript(_tmp2C8,sizeof(void*),i))))return 0;}}
return 1;_LL152:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2C9=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2C9->tag != 1)goto _LL154;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2CA=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2CA->tag != 2)goto _LL154;};_LL153:
 return 1;_LL154: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp2CB=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2CB->tag != 2)goto _LL156;}_LL155:
 return 0;_LL156: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp2CC=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2CC->tag != 0)goto _LL158;}_LL157:
 goto _LL159;_LL158: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp2CD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2CD->tag != 1)goto _LL15A;}_LL159:
 return 0;_LL15A:{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2CE=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2CE->tag != 4)goto _LL15C;}{struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2CF=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f2;if(_tmp2CF->tag != 4)goto _LL15C;};_LL15B:
 goto _LL143;_LL15C: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp2D0=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp2B2.f1;if(_tmp2D0->tag != 4)goto _LL15E;}_LL15D:
 return 0;_LL15E:;_LL15F:
 goto _LL143;_LL143:;}{
# 1205
struct _tuple21 _tmp54C;struct _tuple21 _tmp2D2=(_tmp54C.f1=Cyc_CfFlowInfo_initlevel_approx(r1),((_tmp54C.f2=Cyc_CfFlowInfo_initlevel_approx(r2),_tmp54C)));struct _tuple21 _tmp2D3=_tmp2D2;_LL161: if(_tmp2D3.f1 != Cyc_CfFlowInfo_AllIL)goto _LL163;if(_tmp2D3.f2 != Cyc_CfFlowInfo_AllIL)goto _LL163;_LL162:
 return 1;_LL163: if(_tmp2D3.f2 != Cyc_CfFlowInfo_NoneIL)goto _LL165;_LL164:
 return 1;_LL165: if(_tmp2D3.f1 != Cyc_CfFlowInfo_NoneIL)goto _LL167;_LL166:
 return 0;_LL167: if(_tmp2D3.f2 != Cyc_CfFlowInfo_ThisIL)goto _LL169;_LL168:
 return 1;_LL169: if(_tmp2D3.f1 != Cyc_CfFlowInfo_ThisIL)goto _LL160;_LL16A:
 return 0;_LL160:;};}
# 1220
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 1222
struct _tuple22 _tmp54D;struct _tuple22 _tmp2D5=(_tmp54D.f1=f1,((_tmp54D.f2=f2,_tmp54D)));struct _tuple22 _tmp2D6=_tmp2D5;struct Cyc_Dict_Dict _tmp2D7;struct Cyc_List_List*_tmp2D8;struct Cyc_Dict_Dict _tmp2D9;struct Cyc_List_List*_tmp2DA;_LL16C: if(((_tmp2D6.f1).BottomFL).tag != 1)goto _LL16E;_LL16D:
 return 1;_LL16E: if(((_tmp2D6.f2).BottomFL).tag != 1)goto _LL170;_LL16F:
 return 0;_LL170: if(((_tmp2D6.f1).ReachableFL).tag != 2)goto _LL16B;_tmp2D7=((struct _tuple11)((_tmp2D6.f1).ReachableFL).val).f1;_tmp2D8=((struct _tuple11)((_tmp2D6.f1).ReachableFL).val).f2;if(((_tmp2D6.f2).ReachableFL).tag != 2)goto _LL16B;_tmp2D9=((struct _tuple11)((_tmp2D6.f2).ReachableFL).val).f1;_tmp2DA=((struct _tuple11)((_tmp2D6.f2).ReachableFL).val).f2;_LL171:
# 1226
 if(_tmp2D7.t == _tmp2D9.t  && _tmp2D8 == _tmp2DA)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp2D7,_tmp2D9) && 
Cyc_Relations_relns_approx(_tmp2D8,_tmp2DA);_LL16B:;}
# 1232
struct _tuple12 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv){
# 1234
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp2DC=rv;struct Cyc_Absyn_Vardecl*_tmp2DE;void*_tmp2DF;_LL173: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp2DD=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp2DC;if(_tmp2DD->tag != 8)goto _LL175;else{_tmp2DE=_tmp2DD->f1;_tmp2DF=(void*)_tmp2DD->f2;}}_LL174:
# 1239
{struct Cyc_List_List*_tmp54E;names=((_tmp54E=_region_malloc(rgn,sizeof(*_tmp54E)),((_tmp54E->hd=_tmp2DE,((_tmp54E->tl=names,_tmp54E))))));}rv=_tmp2DF;goto _LL172;_LL175:;_LL176:
# 1241
 done=1;goto _LL172;_LL172:;}{
# 1244
struct _tuple12 _tmp54F;return(_tmp54F.f1=rv,((_tmp54F.f2=names,_tmp54F)));};}
# 1247
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
switch(il){case Cyc_CfFlowInfo_NoneIL: _LL177:
{const char*_tmp552;void*_tmp551;(_tmp551=0,Cyc_fprintf(Cyc_stderr,((_tmp552="uninitialized",_tag_dyneither(_tmp552,sizeof(char),14))),_tag_dyneither(_tmp551,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_ThisIL: _LL178:
{const char*_tmp555;void*_tmp554;(_tmp554=0,Cyc_fprintf(Cyc_stderr,((_tmp555="this-initialized",_tag_dyneither(_tmp555,sizeof(char),17))),_tag_dyneither(_tmp554,sizeof(void*),0)));}break;case Cyc_CfFlowInfo_AllIL: _LL179:
{const char*_tmp558;void*_tmp557;(_tmp557=0,Cyc_fprintf(Cyc_stderr,((_tmp558="all-initialized",_tag_dyneither(_tmp558,sizeof(char),16))),_tag_dyneither(_tmp557,sizeof(void*),0)));}break;}}
# 1255
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp2E8=root;struct Cyc_Absyn_Vardecl*_tmp2EA;struct Cyc_Absyn_Exp*_tmp2EC;void*_tmp2ED;_LL17C: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2E9=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp2E8;if(_tmp2E9->tag != 0)goto _LL17E;else{_tmp2EA=_tmp2E9->f1;}}_LL17D:
# 1258
{const char*_tmp55C;void*_tmp55B[1];struct Cyc_String_pa_PrintArg_struct _tmp55A;(_tmp55A.tag=0,((_tmp55A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2EA->name)),((_tmp55B[0]=& _tmp55A,Cyc_fprintf(Cyc_stderr,((_tmp55C="Root(%s)",_tag_dyneither(_tmp55C,sizeof(char),9))),_tag_dyneither(_tmp55B,sizeof(void*),1)))))));}goto _LL17B;_LL17E: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp2EB=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp2E8;if(_tmp2EB->tag != 1)goto _LL180;else{_tmp2EC=_tmp2EB->f1;_tmp2ED=(void*)_tmp2EB->f2;}}_LL17F:
# 1260
{const char*_tmp561;void*_tmp560[2];struct Cyc_String_pa_PrintArg_struct _tmp55F;struct Cyc_String_pa_PrintArg_struct _tmp55E;(_tmp55E.tag=0,((_tmp55E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp2ED)),((_tmp55F.tag=0,((_tmp55F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp2EC)),((_tmp560[0]=& _tmp55F,((_tmp560[1]=& _tmp55E,Cyc_fprintf(Cyc_stderr,((_tmp561="MallocPt(%s,%s)",_tag_dyneither(_tmp561,sizeof(char),16))),_tag_dyneither(_tmp560,sizeof(void*),2)))))))))))));}goto _LL17B;_LL180: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp2EE=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp2E8;if(_tmp2EE->tag != 2)goto _LL17B;}_LL181:
# 1263
{const char*_tmp564;void*_tmp563;(_tmp563=0,Cyc_fprintf(Cyc_stderr,((_tmp564="InitParam(_,_)",_tag_dyneither(_tmp564,sizeof(char),15))),_tag_dyneither(_tmp563,sizeof(void*),0)));}goto _LL17B;_LL17B:;}
# 1267
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);{
struct Cyc_List_List*_tmp2F8=p->fields;
while(_tmp2F8 != 0){
{const char*_tmp568;void*_tmp567[1];struct Cyc_Int_pa_PrintArg_struct _tmp566;(_tmp566.tag=1,((_tmp566.f1=(unsigned long)((int)_tmp2F8->hd),((_tmp567[0]=& _tmp566,Cyc_fprintf(Cyc_stderr,((_tmp568=".%d",_tag_dyneither(_tmp568,sizeof(char),4))),_tag_dyneither(_tmp567,sizeof(void*),1)))))));}
_tmp2F8=_tmp2F8->tl;}{
# 1274
const char*_tmp56B;void*_tmp56A;(_tmp56A=0,Cyc_fprintf(Cyc_stderr,((_tmp56B=" ",_tag_dyneither(_tmp56B,sizeof(char),2))),_tag_dyneither(_tmp56A,sizeof(void*),0)));};};}
# 1277
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp2FE=1;
while(x != 0){
if(!_tmp2FE){{const char*_tmp56E;void*_tmp56D;(_tmp56D=0,Cyc_fprintf(Cyc_stderr,((_tmp56E=", ",_tag_dyneither(_tmp56E,sizeof(char),3))),_tag_dyneither(_tmp56D,sizeof(void*),0)));}_tmp2FE=0;}
pr(x->hd);
x=x->tl;}{
# 1284
const char*_tmp571;void*_tmp570;(_tmp570=0,Cyc_fprintf(Cyc_stderr,((_tmp571="\n",_tag_dyneither(_tmp571,sizeof(char),2))),_tag_dyneither(_tmp570,sizeof(void*),0)));};}
# 1287
void Cyc_CfFlowInfo_print_dict_set(struct Cyc_Dict_Dict p,void(*pr)(void*)){
struct _RegionHandle _tmp303=_new_region("r");struct _RegionHandle*r=& _tmp303;_push_region(r);
{struct _handler_cons _tmp304;_push_handler(& _tmp304);{int _tmp306=0;if(setjmp(_tmp304.handler))_tmp306=1;if(!_tmp306){
{struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,p);
struct _tuple14 _tmp307=*((struct _tuple14*(*)(struct _RegionHandle*,struct Cyc_Dict_Dict d))Cyc_Dict_rchoose)(r,p);
int first=1;
{const char*_tmp574;void*_tmp573;(_tmp573=0,Cyc_fprintf(Cyc_stderr,((_tmp574="{ ",_tag_dyneither(_tmp574,sizeof(char),3))),_tag_dyneither(_tmp573,sizeof(void*),0)));}
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple14*))Cyc_Iter_next)(iter,& _tmp307)){
void*_tmp30A=_tmp307.f1;
if(!first){
{const char*_tmp577;void*_tmp576;(_tmp576=0,Cyc_fprintf(Cyc_stderr,((_tmp577=", ",_tag_dyneither(_tmp577,sizeof(char),3))),_tag_dyneither(_tmp576,sizeof(void*),0)));}
first=0;}
# 1300
pr(_tmp30A);}{
# 1302
const char*_tmp57A;void*_tmp579;(_tmp579=0,Cyc_fprintf(Cyc_stderr,((_tmp57A="}\n",_tag_dyneither(_tmp57A,sizeof(char),3))),_tag_dyneither(_tmp579,sizeof(void*),0)));};}
# 1290
;_pop_handler();}else{void*_tmp305=(void*)_exn_thrown;void*_tmp310=_tmp305;void*_tmp311;void*_tmp312;_LL183: _tmp311=_tmp310;_LL184:
# 1305
{const char*_tmp57D;void*_tmp57C;(_tmp57C=0,Cyc_fprintf(Cyc_stderr,((_tmp57D="{ }\n",_tag_dyneither(_tmp57D,sizeof(char),5))),_tag_dyneither(_tmp57C,sizeof(void*),0)));}
_npop_handler(0);return;_LL185: _tmp312=_tmp310;_LL186:(void)_throw(_tmp312);_LL182:;}};}
# 1289
;_pop_region(r);}
# 1311
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp315=rval;enum Cyc_CfFlowInfo_InitLevel _tmp31A;enum Cyc_CfFlowInfo_InitLevel _tmp31C;struct Cyc_CfFlowInfo_Place*_tmp31E;int _tmp320;int _tmp321;struct _dyneither_ptr _tmp322;struct Cyc_Absyn_Exp*_tmp324;int _tmp325;void*_tmp326;struct Cyc_Absyn_Vardecl*_tmp328;void*_tmp329;_LL188: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp316=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp316->tag != 0)goto _LL18A;}_LL189:
{const char*_tmp580;void*_tmp57F;(_tmp57F=0,Cyc_fprintf(Cyc_stderr,((_tmp580="Zero",_tag_dyneither(_tmp580,sizeof(char),5))),_tag_dyneither(_tmp57F,sizeof(void*),0)));}goto _LL187;_LL18A: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp317=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp317->tag != 1)goto _LL18C;}_LL18B:
{const char*_tmp583;void*_tmp582;(_tmp582=0,Cyc_fprintf(Cyc_stderr,((_tmp583="NotZeroAll",_tag_dyneither(_tmp583,sizeof(char),11))),_tag_dyneither(_tmp582,sizeof(void*),0)));}goto _LL187;_LL18C: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp318=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp318->tag != 2)goto _LL18E;}_LL18D:
{const char*_tmp586;void*_tmp585;(_tmp585=0,Cyc_fprintf(Cyc_stderr,((_tmp586="NotZeroThis",_tag_dyneither(_tmp586,sizeof(char),12))),_tag_dyneither(_tmp585,sizeof(void*),0)));}goto _LL187;_LL18E: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp319=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp319->tag != 3)goto _LL190;else{_tmp31A=_tmp319->f1;}}_LL18F:
{const char*_tmp589;void*_tmp588;(_tmp588=0,Cyc_fprintf(Cyc_stderr,((_tmp589="Unknown(",_tag_dyneither(_tmp589,sizeof(char),9))),_tag_dyneither(_tmp588,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp31A);
{const char*_tmp58C;void*_tmp58B;(_tmp58B=0,Cyc_fprintf(Cyc_stderr,((_tmp58C=")",_tag_dyneither(_tmp58C,sizeof(char),2))),_tag_dyneither(_tmp58B,sizeof(void*),0)));}goto _LL187;_LL190: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp31B=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp31B->tag != 4)goto _LL192;else{_tmp31C=_tmp31B->f1;}}_LL191:
{const char*_tmp58F;void*_tmp58E;(_tmp58E=0,Cyc_fprintf(Cyc_stderr,((_tmp58F="Esc(",_tag_dyneither(_tmp58F,sizeof(char),5))),_tag_dyneither(_tmp58E,sizeof(void*),0)));}Cyc_CfFlowInfo_print_initlevel(_tmp31C);
{const char*_tmp592;void*_tmp591;(_tmp591=0,Cyc_fprintf(Cyc_stderr,((_tmp592=")",_tag_dyneither(_tmp592,sizeof(char),2))),_tag_dyneither(_tmp591,sizeof(void*),0)));}goto _LL187;_LL192: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp31D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp31D->tag != 5)goto _LL194;else{_tmp31E=_tmp31D->f1;}}_LL193:
{const char*_tmp595;void*_tmp594;(_tmp594=0,Cyc_fprintf(Cyc_stderr,((_tmp595="AddrOf(",_tag_dyneither(_tmp595,sizeof(char),8))),_tag_dyneither(_tmp594,sizeof(void*),0)));}Cyc_CfFlowInfo_print_place(_tmp31E);
{const char*_tmp598;void*_tmp597;(_tmp597=0,Cyc_fprintf(Cyc_stderr,((_tmp598=")",_tag_dyneither(_tmp598,sizeof(char),2))),_tag_dyneither(_tmp597,sizeof(void*),0)));}goto _LL187;_LL194: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp31F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp31F->tag != 6)goto _LL196;else{_tmp320=(_tmp31F->f1).is_union;_tmp321=(_tmp31F->f1).fieldnum;_tmp322=_tmp31F->f2;}}_LL195:
# 1323
 if(_tmp320){
{const char*_tmp59B;void*_tmp59A;(_tmp59A=0,Cyc_fprintf(Cyc_stderr,((_tmp59B="AggrUnion{",_tag_dyneither(_tmp59B,sizeof(char),11))),_tag_dyneither(_tmp59A,sizeof(void*),0)));}
if(_tmp321 != - 1){
const char*_tmp59F;void*_tmp59E[1];struct Cyc_Int_pa_PrintArg_struct _tmp59D;(_tmp59D.tag=1,((_tmp59D.f1=(unsigned long)_tmp321,((_tmp59E[0]=& _tmp59D,Cyc_fprintf(Cyc_stderr,((_tmp59F="tag == %d;",_tag_dyneither(_tmp59F,sizeof(char),11))),_tag_dyneither(_tmp59E,sizeof(void*),1)))))));}}else{
# 1328
const char*_tmp5A2;void*_tmp5A1;(_tmp5A1=0,Cyc_fprintf(Cyc_stderr,((_tmp5A2="AggrStruct{",_tag_dyneither(_tmp5A2,sizeof(char),12))),_tag_dyneither(_tmp5A1,sizeof(void*),0)));}
{int i=0;for(0;i < _get_dyneither_size(_tmp322,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(((void**)_tmp322.curr)[i]);
if(i + 1 < _get_dyneither_size(_tmp322,sizeof(void*))){const char*_tmp5A5;void*_tmp5A4;(_tmp5A4=0,Cyc_fprintf(Cyc_stderr,((_tmp5A5=",",_tag_dyneither(_tmp5A5,sizeof(char),2))),_tag_dyneither(_tmp5A4,sizeof(void*),0)));}}}
# 1333
{const char*_tmp5A8;void*_tmp5A7;(_tmp5A7=0,Cyc_fprintf(Cyc_stderr,((_tmp5A8="}",_tag_dyneither(_tmp5A8,sizeof(char),2))),_tag_dyneither(_tmp5A7,sizeof(void*),0)));}
goto _LL187;_LL196: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp323=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp323->tag != 7)goto _LL198;else{_tmp324=_tmp323->f1;_tmp325=_tmp323->f2;_tmp326=(void*)_tmp323->f3;}}_LL197:
# 1336
{const char*_tmp5AD;void*_tmp5AC[2];struct Cyc_String_pa_PrintArg_struct _tmp5AB;struct Cyc_Int_pa_PrintArg_struct _tmp5AA;(_tmp5AA.tag=1,((_tmp5AA.f1=(unsigned long)_tmp325,((_tmp5AB.tag=0,((_tmp5AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp324)),((_tmp5AC[0]=& _tmp5AB,((_tmp5AC[1]=& _tmp5AA,Cyc_fprintf(Cyc_stderr,((_tmp5AD="[Consumed(%s,%d,",_tag_dyneither(_tmp5AD,sizeof(char),17))),_tag_dyneither(_tmp5AC,sizeof(void*),2)))))))))))));}
Cyc_CfFlowInfo_print_absrval(_tmp326);{const char*_tmp5B0;void*_tmp5AF;(_tmp5AF=0,Cyc_fprintf(Cyc_stderr,((_tmp5B0=")]",_tag_dyneither(_tmp5B0,sizeof(char),3))),_tag_dyneither(_tmp5AF,sizeof(void*),0)));}
goto _LL187;_LL198: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp327=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp315;if(_tmp327->tag != 8)goto _LL187;else{_tmp328=_tmp327->f1;_tmp329=(void*)_tmp327->f2;}}_LL199:
# 1340
{const char*_tmp5B4;void*_tmp5B3[1];struct Cyc_String_pa_PrintArg_struct _tmp5B2;(_tmp5B2.tag=0,((_tmp5B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp328->name)),((_tmp5B3[0]=& _tmp5B2,Cyc_fprintf(Cyc_stderr,((_tmp5B4="[NamedLocation(%s,",_tag_dyneither(_tmp5B4,sizeof(char),19))),_tag_dyneither(_tmp5B3,sizeof(void*),1)))))));}
Cyc_CfFlowInfo_print_absrval(_tmp329);{const char*_tmp5B7;void*_tmp5B6;(_tmp5B6=0,Cyc_fprintf(Cyc_stderr,((_tmp5B7=")]",_tag_dyneither(_tmp5B7,sizeof(char),3))),_tag_dyneither(_tmp5B6,sizeof(void*),0)));}
goto _LL187;_LL187:;}
# 1346
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
{const char*_tmp5BA;void*_tmp5B9;(_tmp5B9=0,Cyc_fprintf(Cyc_stderr,((_tmp5BA="    ",_tag_dyneither(_tmp5BA,sizeof(char),5))),_tag_dyneither(_tmp5B9,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_root(root);
{const char*_tmp5BD;void*_tmp5BC;(_tmp5BC=0,Cyc_fprintf(Cyc_stderr,((_tmp5BD=" --> ",_tag_dyneither(_tmp5BD,sizeof(char),6))),_tag_dyneither(_tmp5BC,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(rval);{
const char*_tmp5C0;void*_tmp5BF;(_tmp5BF=0,Cyc_fprintf(Cyc_stderr,((_tmp5C0="\n",_tag_dyneither(_tmp5C0,sizeof(char),2))),_tag_dyneither(_tmp5BF,sizeof(void*),0)));};}
# 1354
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1358
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp358=f;struct Cyc_Dict_Dict _tmp359;struct Cyc_List_List*_tmp35A;_LL19B: if((_tmp358.BottomFL).tag != 1)goto _LL19D;_LL19C:
{const char*_tmp5C3;void*_tmp5C2;(_tmp5C2=0,Cyc_fprintf(Cyc_stderr,((_tmp5C3="  BottomFL\n",_tag_dyneither(_tmp5C3,sizeof(char),12))),_tag_dyneither(_tmp5C2,sizeof(void*),0)));}goto _LL19A;_LL19D: if((_tmp358.ReachableFL).tag != 2)goto _LL19A;_tmp359=((struct _tuple11)(_tmp358.ReachableFL).val).f1;_tmp35A=((struct _tuple11)(_tmp358.ReachableFL).val).f2;_LL19E:
# 1362
{const char*_tmp5C6;void*_tmp5C5;(_tmp5C5=0,Cyc_fprintf(Cyc_stderr,((_tmp5C6="  ReachableFL:\n",_tag_dyneither(_tmp5C6,sizeof(char),16))),_tag_dyneither(_tmp5C5,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_flowdict(_tmp359);
{const char*_tmp5C9;void*_tmp5C8;(_tmp5C8=0,Cyc_fprintf(Cyc_stderr,((_tmp5C9="\n  Relations: ",_tag_dyneither(_tmp5C9,sizeof(char),15))),_tag_dyneither(_tmp5C8,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp35A);
{const char*_tmp5CC;void*_tmp5CB;(_tmp5CB=0,Cyc_fprintf(Cyc_stderr,((_tmp5CC="\n",_tag_dyneither(_tmp5CC,sizeof(char),2))),_tag_dyneither(_tmp5CB,sizeof(void*),0)));}
goto _LL19A;_LL19A:;}
# 1379 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_contains_region(void*rgn,void*t){
void*_tmp363=Cyc_Tcutil_compress(t);void*_tmp364=_tmp363;struct Cyc_Absyn_Tvar*_tmp371;struct Cyc_List_List*_tmp373;struct Cyc_List_List*_tmp375;struct Cyc_List_List*_tmp377;struct Cyc_List_List*_tmp379;struct Cyc_List_List*_tmp37B;void*_tmp37D;void*_tmp37E;struct Cyc_List_List*_tmp381;struct Cyc_List_List*_tmp383;void*_tmp385;void*_tmp387;void*_tmp389;void*_tmp38B;void*_tmp38C;void*_tmp38E;_LL1A0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp364;if(_tmp365->tag != 26)goto _LL1A2;}_LL1A1:
 goto _LL1A3;_LL1A2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp364;if(_tmp366->tag != 0)goto _LL1A4;}_LL1A3:
 goto _LL1A5;_LL1A4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp364;if(_tmp367->tag != 6)goto _LL1A6;}_LL1A5:
 goto _LL1A7;_LL1A6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp364;if(_tmp368->tag != 7)goto _LL1A8;}_LL1A7:
 goto _LL1A9;_LL1A8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp364;if(_tmp369->tag != 13)goto _LL1AA;}_LL1A9:
 goto _LL1AB;_LL1AA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp364;if(_tmp36A->tag != 14)goto _LL1AC;}_LL1AB:
 goto _LL1AD;_LL1AC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp364;if(_tmp36B->tag != 19)goto _LL1AE;}_LL1AD:
 goto _LL1AF;_LL1AE: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp364;if(_tmp36C->tag != 20)goto _LL1B0;}_LL1AF:
 goto _LL1B1;_LL1B0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp364;if(_tmp36D->tag != 1)goto _LL1B2;}_LL1B1:
 return 0;_LL1B2: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp36E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp364;if(_tmp36E->tag != 21)goto _LL1B4;}_LL1B3: {
# 1391
void*_tmp390=rgn;_LL1D7: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp391=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp390;if(_tmp391->tag != 0)goto _LL1D9;}_LL1D8:
 return 1;_LL1D9:;_LL1DA:
 return 0;_LL1D6:;}_LL1B4: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp364;if(_tmp36F->tag != 22)goto _LL1B6;}_LL1B5: {
# 1396
void*_tmp392=rgn;_LL1DC: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp393=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp392;if(_tmp393->tag != 1)goto _LL1DE;}_LL1DD:
 return 1;_LL1DE:;_LL1DF:
 return 0;_LL1DB:;}_LL1B6: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp370=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp364;if(_tmp370->tag != 2)goto _LL1B8;else{_tmp371=_tmp370->f1;}}_LL1B7: {
# 1401
void*_tmp394=rgn;struct Cyc_Absyn_Tvar*_tmp398;_LL1E1: {struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*_tmp395=(struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct*)_tmp394;if(_tmp395->tag != 1)goto _LL1E3;}_LL1E2:
 goto _LL1E4;_LL1E3: {struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*_tmp396=(struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct*)_tmp394;if(_tmp396->tag != 0)goto _LL1E5;}_LL1E4:
 return 0;_LL1E5: {struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*_tmp397=(struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct*)_tmp394;if(_tmp397->tag != 2)goto _LL1E0;else{_tmp398=_tmp397->f1;}}_LL1E6:
 return Cyc_Absyn_tvar_cmp(_tmp371,_tmp398)== 0;_LL1E0:;}_LL1B8: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp364;if(_tmp372->tag != 3)goto _LL1BA;else{_tmp373=(_tmp372->f1).targs;}}_LL1B9:
# 1414 "cf_flowinfo.cyc"
 _tmp375=_tmp373;goto _LL1BB;_LL1BA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp364;if(_tmp374->tag != 17)goto _LL1BC;else{_tmp375=_tmp374->f2;}}_LL1BB:
 _tmp377=_tmp375;goto _LL1BD;_LL1BC: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp364;if(_tmp376->tag != 11)goto _LL1BE;else{_tmp377=(_tmp376->f1).targs;}}_LL1BD:
 _tmp379=_tmp377;goto _LL1BF;_LL1BE: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp378=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp364;if(_tmp378->tag != 4)goto _LL1C0;else{_tmp379=(_tmp378->f1).targs;}}_LL1BF:
 _tmp37B=_tmp379;goto _LL1C1;_LL1C0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp37A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp364;if(_tmp37A->tag != 24)goto _LL1C2;else{_tmp37B=_tmp37A->f1;}}_LL1C1:
# 1419
 return((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_CfFlowInfo_contains_region,rgn,_tmp37B);_LL1C2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp364;if(_tmp37C->tag != 5)goto _LL1C4;else{_tmp37D=(_tmp37C->f1).elt_typ;_tmp37E=((_tmp37C->f1).ptr_atts).rgn;}}_LL1C3:
# 1421
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp37E) || Cyc_CfFlowInfo_contains_region(rgn,_tmp37D);_LL1C4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp364;if(_tmp37F->tag != 9)goto _LL1C6;}_LL1C5:
 return 0;_LL1C6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp364;if(_tmp380->tag != 10)goto _LL1C8;else{_tmp381=_tmp380->f1;}}_LL1C7:
# 1424
 for(0;_tmp381 != 0;_tmp381=_tmp381->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,(*((struct _tuple17*)_tmp381->hd)).f2))return 1;}
return 0;_LL1C8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp364;if(_tmp382->tag != 12)goto _LL1CA;else{_tmp383=_tmp382->f2;}}_LL1C9:
# 1428
 for(0;_tmp383 != 0;_tmp383=_tmp383->tl){
if(Cyc_CfFlowInfo_contains_region(rgn,((struct Cyc_Absyn_Aggrfield*)_tmp383->hd)->type))return 1;}
return 0;_LL1CA: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp384=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp364;if(_tmp384->tag != 8)goto _LL1CC;else{_tmp385=(_tmp384->f1).elt_type;}}_LL1CB:
 _tmp387=_tmp385;goto _LL1CD;_LL1CC: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp386=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp364;if(_tmp386->tag != 23)goto _LL1CE;else{_tmp387=(void*)_tmp386->f1;}}_LL1CD:
 _tmp389=_tmp387;goto _LL1CF;_LL1CE: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp388=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp364;if(_tmp388->tag != 25)goto _LL1D0;else{_tmp389=(void*)_tmp388->f1;}}_LL1CF:
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp389);_LL1D0: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp364;if(_tmp38A->tag != 16)goto _LL1D2;else{_tmp38B=(void*)_tmp38A->f1;_tmp38C=(void*)_tmp38A->f2;}}_LL1D1:
# 1435
 return Cyc_CfFlowInfo_contains_region(rgn,_tmp38B) || Cyc_CfFlowInfo_contains_region(rgn,_tmp38C);_LL1D2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp38D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp364;if(_tmp38D->tag != 15)goto _LL1D4;else{_tmp38E=(void*)_tmp38D->f1;}}_LL1D3:
# 1438
 return 0;_LL1D4: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp364;if(_tmp38F->tag != 18)goto _LL19F;}_LL1D5:
 return 0;_LL19F:;}struct _tuple24{struct Cyc_CfFlowInfo_FlowEnv*f1;struct Cyc_Dict_Dict f2;void*f3;};
# 1444
static void Cyc_CfFlowInfo_kill_root(struct _tuple24*env,void*root,void*rval){
struct Cyc_CfFlowInfo_FlowEnv*_tmp39A;struct Cyc_Dict_Dict*_tmp39B;void*_tmp39C;struct _tuple24*_tmp399=env;_tmp39A=_tmp399->f1;_tmp39B=(struct Cyc_Dict_Dict*)& _tmp399->f2;_tmp39C=_tmp399->f3;{
void*_tmp39D=root;struct Cyc_Absyn_Vardecl*_tmp39F;void*_tmp3A1;_LL1E8: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp39E=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp39D;if(_tmp39E->tag != 0)goto _LL1EA;else{_tmp39F=_tmp39E->f1;}}_LL1E9:
# 1449
 if(Cyc_CfFlowInfo_contains_region(_tmp39C,_tmp39F->type))
# 1451
rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp39A,_tmp39F->type,0,_tmp39A->unknown_none);
*_tmp39B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp39B,root,rval);
goto _LL1E7;_LL1EA: {struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp3A0=(struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp39D;if(_tmp3A0->tag != 1)goto _LL1EC;else{_tmp3A1=(void*)_tmp3A0->f2;}}_LL1EB:
# 1456
 if(!Cyc_CfFlowInfo_contains_region(_tmp39C,_tmp3A1))
# 1458
*_tmp39B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(*_tmp39B,root,rval);
goto _LL1E7;_LL1EC: {struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp3A2=(struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp39D;if(_tmp3A2->tag != 2)goto _LL1E7;}_LL1ED:
# 1462
 goto _LL1E7;_LL1E7:;};}
# 1467
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_kill_flowdict_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict fd,void*rgn){
# 1469
void*_tmp3A3=Cyc_Tcutil_compress(rgn);void*_tmp3A4=_tmp3A3;struct Cyc_Absyn_Tvar*_tmp3A6;_LL1EF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3A5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3A4;if(_tmp3A5->tag != 2)goto _LL1F1;else{_tmp3A6=_tmp3A5->f1;}}_LL1F0: {
# 1471
struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct _tmp5CD;struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct v=(_tmp5CD.tag=2,((_tmp5CD.f1=_tmp3A6,_tmp5CD)));
void*v2=(void*)& v;
struct _tuple24 _tmp5CE;struct _tuple24 env=(_tmp5CE.f1=fenv,((_tmp5CE.f2=fenv->mt_flowdict,((_tmp5CE.f3=v2,_tmp5CE)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1F1: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp3A7=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp3A4;if(_tmp3A7->tag != 21)goto _LL1F3;}_LL1F2: {
# 1478
struct _tuple24 _tmp5CF;struct _tuple24 env=(_tmp5CF.f1=fenv,((_tmp5CF.f2=fenv->mt_flowdict,((_tmp5CF.f3=(void*)& Cyc_CfFlowInfo_UniqueRgn_k_val,_tmp5CF)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1F3: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp3A8=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp3A4;if(_tmp3A8->tag != 22)goto _LL1F5;}_LL1F4: {
# 1483
struct _tuple24 _tmp5D0;struct _tuple24 env=(_tmp5D0.f1=fenv,((_tmp5D0.f2=fenv->mt_flowdict,((_tmp5D0.f3=(void*)& Cyc_CfFlowInfo_RefCntRgn_k_val,_tmp5D0)))));
((void(*)(void(*f)(struct _tuple24*,void*,void*),struct _tuple24*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_CfFlowInfo_kill_root,& env,fd);
return env.f2;}_LL1F5:;_LL1F6: {
# 1487
struct Cyc_Core_Impossible_exn_struct _tmp5D6;const char*_tmp5D5;struct Cyc_Core_Impossible_exn_struct*_tmp5D4;(int)_throw((void*)((_tmp5D4=_cycalloc(sizeof(*_tmp5D4)),((_tmp5D4[0]=((_tmp5D6.tag=Cyc_Core_Impossible,((_tmp5D6.f1=((_tmp5D5="kill_flowdict_region",_tag_dyneither(_tmp5D5,sizeof(char),21))),_tmp5D6)))),_tmp5D4)))));}_LL1EE:;}
# 1492
static struct Cyc_List_List*Cyc_CfFlowInfo_kill_relns_region(struct Cyc_List_List*relns,void*rgn){
# 1495
return 0;}
# 1500
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn){
union Cyc_CfFlowInfo_FlowInfo _tmp3B0=f;struct Cyc_Dict_Dict _tmp3B1;struct Cyc_List_List*_tmp3B2;_LL1F8: if((_tmp3B0.BottomFL).tag != 1)goto _LL1FA;_LL1F9:
 return f;_LL1FA: if((_tmp3B0.ReachableFL).tag != 2)goto _LL1F7;_tmp3B1=((struct _tuple11)(_tmp3B0.ReachableFL).val).f1;_tmp3B2=((struct _tuple11)(_tmp3B0.ReachableFL).val).f2;_LL1FB: {
# 1504
struct Cyc_Dict_Dict _tmp3B3=Cyc_CfFlowInfo_kill_flowdict_region(fenv,_tmp3B1,rgn);
struct Cyc_List_List*_tmp3B4=Cyc_CfFlowInfo_kill_relns_region(_tmp3B2,rgn);
# 1507
return Cyc_CfFlowInfo_ReachableFL(_tmp3B3,_tmp3B4);}_LL1F7:;}
