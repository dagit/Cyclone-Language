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
# 69 "core.h"
void exit(int);
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 300
int isspace(int);
# 313
char*getenv(const char*);
# 318
int system(const char*);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 152 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 187
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 193
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 198
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 252 "core.h"
void Cyc_Core_rethrow(void*);
# 256
const char*Cyc_Core_get_exn_name(void*);
# 258
const char*Cyc_Core_get_exn_filename();
# 261
int Cyc_Core_get_exn_lineno();
# 270
struct _dyneither_ptr Cstring_to_string(char*);
# 280
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 55 "arg.h"
typedef void*Cyc_Arg_spec_t;
# 57
typedef struct Cyc_List_List*Cyc_Arg_speclist_t;
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 102 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 107
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 118
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 32 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);
# 37
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 61
extern int Cyc_Position_use_gcc_style_location;
# 63
extern int Cyc_Position_max_errors;
# 65
int Cyc_Position_error_p();
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1176 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1178
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 32 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
# 36
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 78 "parse_tab.h"
typedef struct Cyc_Yyltype Cyc_yyltype;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 31 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 118 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 227
extern int Cyc_Tcenv_warn_override;
# 44 "tcutil.h"
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 34 "tc.h"
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 35 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Interface_I;
# 37 "interface.h"
typedef struct Cyc_Interface_I*Cyc_Interface_i_t;
# 40
struct Cyc_Interface_I*Cyc_Interface_empty();
# 49 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 53
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Dict_Dict ae);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 61
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 76 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 82
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 85
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 88
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 91
typedef struct Cyc_Interface_I*Cyc_interface_t;
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_ParseErrors_print_state_and_token;
# 48
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 53
int Cyc_Toc_warn_bounds_checks;
int Cyc_Toc_warn_all_null_deref;
# 63
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
# 94
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D0;struct Cyc_List_List*_tmp3CF;Cyc_ccargs=((_tmp3CF=_cycalloc(sizeof(*_tmp3CF)),((_tmp3CF->hd=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0[0]=s,_tmp3D0)))),((_tmp3CF->tl=Cyc_ccargs,_tmp3CF))))));}
# 102
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3D1;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D1="vc",_tag_dyneither(_tmp3D1,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3D2;Cyc_add_ccarg(((_tmp3D2="-DVC_C",_tag_dyneither(_tmp3D2,sizeof(char),7))));};}else{
# 107
const char*_tmp3D3;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D3="gcc",_tag_dyneither(_tmp3D3,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3D4;Cyc_add_ccarg(((_tmp3D4="-DGCC_C",_tag_dyneither(_tmp3D4,sizeof(char),8))));};}}}
# 115
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 123
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D5;Cyc_output_file=((_tmp3D5=_cycalloc(sizeof(*_tmp3D5)),((_tmp3D5[0]=s,_tmp3D5))));}
# 128
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 137
extern char*Cdef_inc_path;
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Ccomp;
extern char*Cversion;static char _tmp7[1]="";
# 144
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 149
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D8;struct Cyc_List_List*_tmp3D7;Cyc_cppargs=((_tmp3D7=_cycalloc(sizeof(*_tmp3D7)),((_tmp3D7->hd=((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=s,_tmp3D8)))),((_tmp3D7->tl=Cyc_cppargs,_tmp3D7))))));}
# 154
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3DC;void*_tmp3DB[1];struct Cyc_String_pa_PrintArg_struct _tmp3DA;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DA.tag=0,((_tmp3DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DB[0]=& _tmp3DA,Cyc_aprintf(((_tmp3DC="-iprefix %s",_tag_dyneither(_tmp3DC,sizeof(char),12))),_tag_dyneither(_tmp3DB,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3E0;void*_tmp3DF[1];struct Cyc_String_pa_PrintArg_struct _tmp3DE;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DE.tag=0,((_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DF[0]=& _tmp3DE,Cyc_aprintf(((_tmp3E0="-iwithprefix %s",_tag_dyneither(_tmp3E0,sizeof(char),16))),_tag_dyneither(_tmp3DF,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3E4;void*_tmp3E3[1];struct Cyc_String_pa_PrintArg_struct _tmp3E2;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E2.tag=0,((_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E3[0]=& _tmp3E2,Cyc_aprintf(((_tmp3E4="-iwithprefixbefore %s",_tag_dyneither(_tmp3E4,sizeof(char),22))),_tag_dyneither(_tmp3E3,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3E8;void*_tmp3E7[1];struct Cyc_String_pa_PrintArg_struct _tmp3E6;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E6.tag=0,((_tmp3E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E7[0]=& _tmp3E6,Cyc_aprintf(((_tmp3E8="-isystem %s",_tag_dyneither(_tmp3E8,sizeof(char),12))),_tag_dyneither(_tmp3E7,sizeof(void*),1)))))))));}
# 166
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3EC;void*_tmp3EB[1];struct Cyc_String_pa_PrintArg_struct _tmp3EA;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3EA.tag=0,((_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3EB[0]=& _tmp3EA,Cyc_aprintf(((_tmp3EC="-idirafter %s",_tag_dyneither(_tmp3EC,sizeof(char),14))),_tag_dyneither(_tmp3EB,sizeof(void*),1)))))))));}
# 170
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 173
Cyc_target_arch=s;
{const char*_tmp3ED;Cyc_add_ccarg(((_tmp3ED="-b",_tag_dyneither(_tmp3ED,sizeof(char),3))));}
Cyc_add_ccarg(s);}
# 178
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 183
static void Cyc_print_version(){
{const char*_tmp3F1;void*_tmp3F0[1];struct Cyc_String_pa_PrintArg_struct _tmp3EF;(_tmp3EF.tag=0,((_tmp3EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3F0[0]=& _tmp3EF,Cyc_printf(((_tmp3F1="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3F1,sizeof(char),34))),_tag_dyneither(_tmp3F0,sizeof(void*),1)))))));}
{const char*_tmp3F6;void*_tmp3F5[2];struct Cyc_String_pa_PrintArg_struct _tmp3F4;struct Cyc_String_pa_PrintArg_struct _tmp3F3;(_tmp3F3.tag=0,((_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3F5[0]=& _tmp3F4,((_tmp3F5[1]=& _tmp3F3,Cyc_printf(((_tmp3F6="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3F6,sizeof(char),39))),_tag_dyneither(_tmp3F5,sizeof(void*),2)))))))))))));}
{const char*_tmp3FA;void*_tmp3F9[1];struct Cyc_String_pa_PrintArg_struct _tmp3F8;(_tmp3F8.tag=0,((_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3F9[0]=& _tmp3F8,Cyc_printf(((_tmp3FA="Standard library directory: %s\n",_tag_dyneither(_tmp3FA,sizeof(char),32))),_tag_dyneither(_tmp3F9,sizeof(void*),1)))))));}
{const char*_tmp3FE;void*_tmp3FD[1];struct Cyc_String_pa_PrintArg_struct _tmp3FC;(_tmp3FC.tag=0,((_tmp3FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3FD[0]=& _tmp3FC,Cyc_printf(((_tmp3FE="Standard include directory: %s\n",_tag_dyneither(_tmp3FE,sizeof(char),32))),_tag_dyneither(_tmp3FD,sizeof(void*),1)))))));}
exit(0);}
# 192
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp27=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp27 <= 4)return 0;else{
const char*_tmp3FF;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp27 - 4)),((_tmp3FF=".cyc",_tag_dyneither(_tmp3FF,sizeof(char),5))))== 0;}}
# 198
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 2)return;{
struct _dyneither_ptr _tmp2A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp29 - 2);
struct _dyneither_ptr*_tmp402;struct Cyc_List_List*_tmp401;Cyc_cyclone_exec_path=((_tmp401=_cycalloc(sizeof(*_tmp401)),((_tmp401->hd=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=_tmp2A,_tmp402)))),((_tmp401->tl=Cyc_cyclone_exec_path,_tmp401))))));};}
# 211
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp403;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp403="-lxml",_tag_dyneither(_tmp403,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 217
const char*_tmp404;Cyc_add_ccarg(((_tmp404="-lxml_a",_tag_dyneither(_tmp404,sizeof(char),8))));}}else{
# 219
struct _dyneither_ptr*_tmp407;struct Cyc_List_List*_tmp406;Cyc_libargs=((_tmp406=_cycalloc(sizeof(*_tmp406)),((_tmp406->hd=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407[0]=s,_tmp407)))),((_tmp406->tl=Cyc_libargs,_tmp406))))));}}
# 222
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 226
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp408;Cyc_add_ccarg(((_tmp408="-save-temps",_tag_dyneither(_tmp408,sizeof(char),12))));};}
# 231
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp409;Cyc_add_cpparg(((_tmp409="-M",_tag_dyneither(_tmp409,sizeof(char),3))));};}
# 238
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp40A;Cyc_dependencies_target=((_tmp40A=_cycalloc(sizeof(*_tmp40A)),((_tmp40A[0]=s,_tmp40A))));}
# 243
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp40B;Cyc_add_ccarg(((_tmp40B="-c",_tag_dyneither(_tmp40B,sizeof(char),3))));};}
# 248
static void Cyc_set_nocppprecomp(){
{const char*_tmp40C;Cyc_add_cpparg(((_tmp40C="-no-cpp-precomp",_tag_dyneither(_tmp40C,sizeof(char),16))));}{
const char*_tmp40D;Cyc_add_ccarg(((_tmp40D="-no-cpp-precomp",_tag_dyneither(_tmp40D,sizeof(char),16))));};}
# 253
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 257
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 260
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 264
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
{const char*_tmp40E;Cyc_add_ccarg(((_tmp40E="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp40E,sizeof(char),18))));}
Cyc_set_elim_se();}
# 269
static void Cyc_set_noboundschecks(){
const char*_tmp40F;Cyc_add_ccarg(((_tmp40F="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp40F,sizeof(char),23))));}
# 272
static void Cyc_set_nonullchecks(){
const char*_tmp410;Cyc_add_ccarg(((_tmp410="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp410,sizeof(char),21))));}
# 275
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 281
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp411;Cyc_add_ccarg(((_tmp411="-DNO_CYC_PREFIX",_tag_dyneither(_tmp411,sizeof(char),16))));};}
# 286
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp412;Cyc_add_ccarg(((_tmp412="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp412,sizeof(char),21))));}{
const char*_tmp413;Cyc_add_cpparg(((_tmp413="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp413,sizeof(char),21))));};}
# 292
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp414;Cyc_add_ccarg(((_tmp414="-pg",_tag_dyneither(_tmp414,sizeof(char),4))));};}
# 297
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp415;Cyc_add_ccarg(((_tmp415="-S",_tag_dyneither(_tmp415,sizeof(char),3))));};}
# 302
static void Cyc_set_all_warnings(){
Cyc_Toc_warn_bounds_checks=1;
Cyc_Toc_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;}
# 311
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0,Cyc_CYCLONEFILE  = 1};
# 315
typedef enum Cyc_inputtype Cyc_inputtype_t;
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
const char*_tmp416;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp416="cyc",_tag_dyneither(_tmp416,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp417;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp417="none",_tag_dyneither(_tmp417,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 323
const char*_tmp41B;void*_tmp41A[1];struct Cyc_String_pa_PrintArg_struct _tmp419;(_tmp419.tag=0,((_tmp419.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp41A[0]=& _tmp419,Cyc_fprintf(Cyc_stderr,((_tmp41B="Input type '%s' not supported\n",_tag_dyneither(_tmp41B,sizeof(char),31))),_tag_dyneither(_tmp41A,sizeof(void*),1)))))));}}}
# 327
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp44=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp45=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 333
struct _dyneither_ptr _tmp46=Cyc_strlen((struct _dyneither_ptr)_tmp44)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp44,(struct _dyneither_ptr)_tmp45): _tmp45;
return(struct _dyneither_ptr)_tmp46;}
# 339
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 341
static void Cyc_add_other(struct _dyneither_ptr s){
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 344
{struct _dyneither_ptr*_tmp41E;struct Cyc_List_List*_tmp41D;Cyc_cyclone_files=((_tmp41D=_cycalloc(sizeof(*_tmp41D)),((_tmp41D->hd=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=s,_tmp41E)))),((_tmp41D->tl=Cyc_cyclone_files,_tmp41D))))));}{
# 347
struct _dyneither_ptr _tmp49=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp41F;struct _dyneither_ptr _tmp4A=Cyc_strconcat((struct _dyneither_ptr)_tmp49,((_tmp41F=".c",_tag_dyneither(_tmp41F,sizeof(char),3))));
Cyc_add_ccarg((struct _dyneither_ptr)_tmp4A);};}else{
# 351
Cyc_add_ccarg(s);}}
# 354
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_untag_dyneither_ptr(s,sizeof(char),1));}}struct _tuple21{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};
# 365
struct Cyc_List_List*Cyc_read_specs(struct _dyneither_ptr file){
struct Cyc_List_List*_tmp4C=0;
int c;
int i;
char strname[256];
char strvalue[4096];
struct Cyc___cycFILE*spec_file=Cyc_fopen((const char*)_untag_dyneither_ptr(file,sizeof(char),1),"r");
# 373
if(spec_file == 0){
{const char*_tmp423;void*_tmp422[1];struct Cyc_String_pa_PrintArg_struct _tmp421;(_tmp421.tag=0,((_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp422[0]=& _tmp421,Cyc_fprintf(Cyc_stderr,((_tmp423="Error opening spec file %s\n",_tag_dyneither(_tmp423,sizeof(char),28))),_tag_dyneither(_tmp422,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 379
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp428;void*_tmp427[2];struct Cyc_String_pa_PrintArg_struct _tmp426;struct Cyc_Int_pa_PrintArg_struct _tmp425;(_tmp425.tag=1,((_tmp425.f1=(unsigned long)c,((_tmp426.tag=0,((_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp427[0]=& _tmp426,((_tmp427[1]=& _tmp425,Cyc_fprintf(Cyc_stderr,((_tmp428="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp428,sizeof(char),55))),_tag_dyneither(_tmp427,sizeof(void*),2)))))))))))));}
# 388
Cyc_fflush(Cyc_stderr);}
# 390
goto CLEANUP_AND_RETURN;}
# 392
JUST_AFTER_STAR:
 i=0;
while(1){
c=Cyc_fgetc(spec_file);
if(c == - 1){
{const char*_tmp42C;void*_tmp42B[1];struct Cyc_String_pa_PrintArg_struct _tmp42A;(_tmp42A.tag=0,((_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42B[0]=& _tmp42A,Cyc_fprintf(Cyc_stderr,((_tmp42C="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp42C,sizeof(char),44))),_tag_dyneither(_tmp42B,sizeof(void*),1)))))));}
# 400
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}
# 403
if(c == ':'){
strname[_check_known_subscript_notnull(256,i)]='\000';
break;}
# 407
strname[_check_known_subscript_notnull(256,i)]=(char)c;
++ i;
if(i >= 256){
{const char*_tmp430;void*_tmp42F[1];struct Cyc_String_pa_PrintArg_struct _tmp42E;(_tmp42E.tag=0,((_tmp42E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42F[0]=& _tmp42E,Cyc_fprintf(Cyc_stderr,((_tmp430="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp430,sizeof(char),50))),_tag_dyneither(_tmp42F,sizeof(void*),1)))))));}
# 413
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 417
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
break;}
# 422
if(c == '*'){
# 424
{struct _tuple21*_tmp43D;struct _dyneither_ptr*_tmp43C;const char*_tmp43B;struct _dyneither_ptr*_tmp43A;struct Cyc_List_List*_tmp439;_tmp4C=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439->hd=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->f1=((_tmp43C=_cycalloc(sizeof(*_tmp43C)),((_tmp43C[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp43C)))),((_tmp43D->f2=(
(_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp43B="",_tag_dyneither(_tmp43B,sizeof(char),1)))),_tmp43A)))),_tmp43D)))))),((_tmp439->tl=_tmp4C,_tmp439))))));}
goto JUST_AFTER_STAR;}
# 429
strvalue[0]=(char)c;
i=1;
while(1){
c=Cyc_fgetc(spec_file);
if((c == - 1  || c == '\n') || c == '\r'){strvalue[_check_known_subscript_notnull(4096,i)]='\000';break;}
strvalue[_check_known_subscript_notnull(4096,i)]=(char)c;
++ i;
if(i >= 4096){
{const char*_tmp442;void*_tmp441[2];struct Cyc_String_pa_PrintArg_struct _tmp440;struct Cyc_String_pa_PrintArg_struct _tmp43F;(_tmp43F.tag=0,((_tmp43F.f1=(struct _dyneither_ptr)
# 439
_tag_dyneither(strname,sizeof(char),256),((_tmp440.tag=0,((_tmp440.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp441[0]=& _tmp440,((_tmp441[1]=& _tmp43F,Cyc_fprintf(Cyc_stderr,((_tmp442="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp442,sizeof(char),50))),_tag_dyneither(_tmp441,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 445
{struct _tuple21*_tmp44B;struct _dyneither_ptr*_tmp44A;struct _dyneither_ptr*_tmp449;struct Cyc_List_List*_tmp448;_tmp4C=((_tmp448=_cycalloc(sizeof(*_tmp448)),((_tmp448->hd=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B->f1=((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp44A)))),((_tmp44B->f2=(
(_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp449)))),_tmp44B)))))),((_tmp448->tl=_tmp4C,_tmp448))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 450
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4C;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp455(struct _dyneither_ptr*_tmp6D,unsigned int*_tmp454,unsigned int*_tmp453,struct _dyneither_ptr**_tmp451){for(*_tmp454=0;*_tmp454 < *_tmp453;(*_tmp454)++){(*_tmp451)[*_tmp454]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp6D).curr)[(int)*_tmp454];}}
# 456
struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline){
if((char*)cmdline.curr == (char*)(_tag_dyneither(0,0,0)).curr)return _tag_dyneither(0,0,0);{
unsigned long _tmp67=Cyc_strlen((struct _dyneither_ptr)cmdline);
struct Cyc_List_List*_tmp68=0;
char buf[4096];
int i=0;
int j=0;
if(_tmp67 > 4096)goto DONE;
while(1){
# 466
while(1){
if(i >= _tmp67)goto DONE;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)goto DONE;
if(!isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
++ i;}
# 472
j=0;
# 477
while(1){
if(i >= _tmp67)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
if(isspace((int)*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))))break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== '\\'){
# 483
++ i;
if(i >= _tmp67)break;
if(*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i))== 0)break;
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}else{
# 490
buf[_check_known_subscript_notnull(4096,j)]=*((const char*)_check_dyneither_subscript(cmdline,sizeof(char),i));
++ j;}
# 493
++ i;}
# 495
if(j < 4096)
buf[_check_known_subscript_notnull(4096,j)]='\000';{
# 498
struct _dyneither_ptr*_tmp44E;struct Cyc_List_List*_tmp44D;_tmp68=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->hd=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp44E)))),((_tmp44D->tl=_tmp68,_tmp44D))))));};}
# 500
DONE:
 _tmp68=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp68);
{struct Cyc_List_List*_tmp44F;_tmp68=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp44F->tl=_tmp68,_tmp44F))))));}{
struct _dyneither_ptr _tmp6D=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp68);
unsigned int _tmp454;unsigned int _tmp453;struct _dyneither_ptr _tmp452;struct _dyneither_ptr*_tmp451;unsigned int _tmp450;struct _dyneither_ptr _tmp6E=(_tmp450=_get_dyneither_size(_tmp6D,sizeof(struct _dyneither_ptr*)),((_tmp451=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp450)),((_tmp452=_tag_dyneither(_tmp451,sizeof(struct _dyneither_ptr),_tmp450),((((_tmp453=_tmp450,_tmp455(& _tmp6D,& _tmp454,& _tmp453,& _tmp451))),_tmp452)))))));
return _tmp6E;};};}
# 510
int Cyc_compile_failure=0;
# 512
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp73;_push_handler(& _tmp73);{int _tmp75=0;if(setjmp(_tmp73.handler))_tmp75=1;if(!_tmp75){{struct Cyc___cycFILE*_tmp76=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp76;};_pop_handler();}else{void*_tmp74=(void*)_exn_thrown;void*_tmp78=_tmp74;void*_tmp79;_LL1:;_LL2:
# 516
 Cyc_compile_failure=1;
{const char*_tmp45A;void*_tmp459[2];struct Cyc_String_pa_PrintArg_struct _tmp458;struct Cyc_String_pa_PrintArg_struct _tmp457;(_tmp457.tag=0,((_tmp457.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp458.tag=0,((_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp459[0]=& _tmp458,((_tmp459[1]=& _tmp457,Cyc_fprintf(Cyc_stderr,((_tmp45A="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp45A,sizeof(char),29))),_tag_dyneither(_tmp459,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp79=_tmp78;_LL4:(void)_throw(_tmp79);_LL0:;}};}
# 523
void CYCALLOCPROFILE_mark(const char*s);
# 525
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 529
const char*_tmp45B;struct _dyneither_ptr explain_string=(_tmp45B="",_tag_dyneither(_tmp45B,sizeof(char),1));
struct Cyc_List_List*_tmp7E=0;
# 532
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 534
{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){_tmp7E=f(env,tds);;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*_tmp83=_tmp80;void*_tmp84;void*_tmp85;_LL6: _tmp84=_tmp83;_LL7:
# 537
{const char*_tmp45F;void*_tmp45E[1];struct Cyc_String_pa_PrintArg_struct _tmp45D;(_tmp45D.tag=0,((_tmp45D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp45E[0]=& _tmp45D,Cyc_fprintf(Cyc_stderr,((_tmp45F="COMPILER STAGE %s\n",_tag_dyneither(_tmp45F,sizeof(char),19))),_tag_dyneither(_tmp45E,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp84);_LL8: _tmp85=_tmp83;_LL9:(void)_throw(_tmp85);_LL5:;}};}
# 540
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp462;void*_tmp461;(_tmp461=0,Cyc_fprintf(Cyc_stderr,((_tmp462="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp462,sizeof(char),22))),_tag_dyneither(_tmp461,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp7E;}else{
# 547
if(Cyc_v_r){
{const char*_tmp466;void*_tmp465[1];struct Cyc_String_pa_PrintArg_struct _tmp464;(_tmp464.tag=0,((_tmp464.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp465[0]=& _tmp464,Cyc_fprintf(Cyc_stderr,((_tmp466="%s completed.\n",_tag_dyneither(_tmp466,sizeof(char),15))),_tag_dyneither(_tmp465,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return _tmp7E;}}
# 552
return _tmp7E;}
# 555
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp8F;_push_handler(& _tmp8F);{int _tmp91=0;if(setjmp(_tmp8F.handler))_tmp91=1;if(!_tmp91){
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp90=(void*)_exn_thrown;void*_tmp93=_tmp90;void*_tmp95;_LLB: {struct Cyc_Position_Exit_exn_struct*_tmp94=(struct Cyc_Position_Exit_exn_struct*)_tmp93;if(_tmp94->tag != Cyc_Position_Exit)goto _LLD;}_LLC:
# 562
 Cyc_compile_failure=1;goto _LLA;_LLD: _tmp95=_tmp93;_LLE:(void)_throw(_tmp95);_LLA:;}};}
# 564
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 568
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 570
struct _tuple22 _tmp96=*tcenv;struct _RegionHandle*_tmp98;struct Cyc_Tcenv_Tenv*_tmp99;struct _tuple22 _tmp97=_tmp96;_tmp98=_tmp97.f1;_tmp99=_tmp97.f2;
Cyc_Tc_tc(_tmp98,_tmp99,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp99,tds);
return tds;}
# 577
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 584
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmp9B;struct Cyc___cycFILE*_tmp9C;struct Cyc___cycFILE*_tmp9D;struct _tuple23*_tmp9A=params;_tmp9B=_tmp9A->f1;_tmp9C=_tmp9A->f2;_tmp9D=_tmp9A->f3;{
struct Cyc_Interface_I*_tmp9E=Cyc_Interface_extract(_tmp9B->ae);
if(_tmp9C == 0)
Cyc_Interface_save(_tmp9E,_tmp9D);else{
# 590
struct Cyc_Interface_I*_tmp9F=Cyc_Interface_parse(_tmp9C);
const char*_tmp46B;const char*_tmp46A;struct _tuple20*_tmp469;if(!Cyc_Interface_is_subinterface(_tmp9F,_tmp9E,((_tmp469=_cycalloc(sizeof(*_tmp469)),((_tmp469->f1=((_tmp46A="written interface",_tag_dyneither(_tmp46A,sizeof(char),18))),((_tmp469->f2=((_tmp46B="maximal interface",_tag_dyneither(_tmp46B,sizeof(char),18))),_tmp469))))))))
Cyc_compile_failure=1;else{
# 594
Cyc_Interface_save(_tmp9F,_tmp9D);}}
# 596
return tds;};}
# 599
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 602
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 607
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 609
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 611
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 616
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 619
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp46E;void*_tmp46D;(_tmp46D=0,Cyc_fprintf(Cyc_stderr,((_tmp46E="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp46E,sizeof(char),53))),_tag_dyneither(_tmp46D,sizeof(void*),0)));}
return 1;}{
# 623
const char*_tmp470;const char*_tmp46F;struct Cyc___cycFILE*_tmpA5=Cyc_try_file_open(file,((_tmp46F="r",_tag_dyneither(_tmp46F,sizeof(char),2))),((_tmp470="internal compiler file",_tag_dyneither(_tmp470,sizeof(char),23))));
if(_tmpA5 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpAC=1024;unsigned int i;for(i=0;i < _tmpAC;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpA5);
if(n_read != 1024  && !Cyc_feof(_tmpA5)){
Cyc_compile_failure=1;
{const char*_tmp474;void*_tmp473[1];struct Cyc_String_pa_PrintArg_struct _tmp472;(_tmp472.tag=0,((_tmp472.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp473[0]=& _tmp472,Cyc_fprintf(Cyc_stderr,((_tmp474="\nError: problem copying %s\n",_tag_dyneither(_tmp474,sizeof(char),28))),_tag_dyneither(_tmp473,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 636
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp478;void*_tmp477[1];struct Cyc_String_pa_PrintArg_struct _tmp476;(_tmp476.tag=0,((_tmp476.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp477[0]=& _tmp476,Cyc_fprintf(Cyc_stderr,((_tmp478="\nError: problem copying %s\n",_tag_dyneither(_tmp478,sizeof(char),28))),_tag_dyneither(_tmp477,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 644
Cyc_fclose(_tmpA5);
return 0;};};}
# 649
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpAF=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpAF.expand_typedefs=!Cyc_noexpand_r;
_tmpAF.to_VC=Cyc_Cyclone_tovc_r;
_tmpAF.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpAF.generate_line_directives=Cyc_generate_line_directives_r;
_tmpAF.print_full_evars=Cyc_print_full_evars_r;
_tmpAF.print_all_tvars=Cyc_print_all_tvars_r;
_tmpAF.print_all_kinds=Cyc_print_all_kinds_r;
_tmpAF.print_all_effects=Cyc_print_all_effects_r;
# 660
if(Cyc_inline_functions_r){
const char*_tmp47B;void*_tmp47A;(_tmp47A=0,Cyc_fprintf(out_file,((_tmp47B="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp47B,sizeof(char),27))),_tag_dyneither(_tmp47A,sizeof(void*),0)));}
# 664
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp47E;void*_tmp47D;(_tmp47D=0,Cyc_fprintf(out_file,((_tmp47E="#include <setjmp.h>\n",_tag_dyneither(_tmp47E,sizeof(char),21))),_tag_dyneither(_tmp47D,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 670
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 672
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpAF);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 676
Cyc_Absyndump_set_params(& _tmpAF);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 679
Cyc_fflush(out_file);
return tds;}
# 685
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 693
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpB4=0;
unsigned long _tmpB5=Cyc_strlen((struct _dyneither_ptr)s);
# 698
while(_tmpB5 > 0){
struct _dyneither_ptr _tmpB6=Cyc_strchr(s,c);
if((char*)_tmpB6.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp481;struct Cyc_List_List*_tmp480;_tmpB4=((_tmp480=_cycalloc(sizeof(*_tmp480)),((_tmp480->hd=((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481[0]=s,_tmp481)))),((_tmp480->tl=_tmpB4,_tmp480))))));}
break;}else{
# 705
{struct _dyneither_ptr*_tmp484;struct Cyc_List_List*_tmp483;_tmpB4=((_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483->hd=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpB6).curr - s.curr)/ sizeof(char))),_tmp484)))),((_tmp483->tl=_tmpB4,_tmp483))))));}
_tmpB5 -=(((struct _dyneither_ptr)_tmpB6).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpB6,sizeof(char),1);}}
# 710
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB4);};}
# 714
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpBB;_push_handler(& _tmpBB);{int _tmpBD=0;if(setjmp(_tmpBB.handler))_tmpBD=1;if(!_tmpBD){{const char*_tmp485;f=Cyc_file_open(file,((_tmp485="r",_tag_dyneither(_tmp485,sizeof(char),2))));};_pop_handler();}else{void*_tmpBC=(void*)_exn_thrown;void*_tmpC0=_tmpBC;void*_tmpC1;_LL10:;_LL11: goto _LLF;_LL12: _tmpC1=_tmpC0;_LL13:(void)_throw(_tmpC1);_LLF:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 722
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp486;struct _dyneither_ptr tmp=(_tmp486="",_tag_dyneither(_tmp486,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC2=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC2.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC2)== 0)continue;
{const char*_tmp487;_tmpC2=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC2,((_tmp487=":",_tag_dyneither(_tmp487,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC2,(struct _dyneither_ptr)tmp);}
# 730
return tmp;}
# 735
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC5=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC5.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC5)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpC5,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp488;return(_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488[0]=s,_tmp488)));}};}
# 743
return 0;}
# 746
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpC7=Cyc_find(dirs,file);
if(_tmpC7 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp48D;void*_tmp48C[2];struct Cyc_String_pa_PrintArg_struct _tmp48B;struct Cyc_String_pa_PrintArg_struct _tmp48A;(_tmp48A.tag=0,((_tmp48A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp48C[0]=& _tmp48B,((_tmp48C[1]=& _tmp48A,Cyc_fprintf(Cyc_stderr,((_tmp48D="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp48D,sizeof(char),56))),_tag_dyneither(_tmp48C,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp49D;const char*_tmp49C;void*_tmp49B[2];struct Cyc_String_pa_PrintArg_struct _tmp49A;struct Cyc_String_pa_PrintArg_struct _tmp499;struct Cyc_Core_Failure_exn_struct*_tmp498;(int)_throw((void*)((_tmp498=_cycalloc(sizeof(*_tmp498)),((_tmp498[0]=((_tmp49D.tag=Cyc_Core_Failure,((_tmp49D.f1=(struct _dyneither_ptr)((_tmp499.tag=0,((_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp49B[0]=& _tmp49A,((_tmp49B[1]=& _tmp499,Cyc_aprintf(((_tmp49C="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp49C,sizeof(char),56))),_tag_dyneither(_tmp49B,sizeof(void*),2)))))))))))))),_tmp49D)))),_tmp498)))));};}
# 758
return*_tmpC7;}
# 763
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 765
struct Cyc_List_List*_tmpD2=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp4A0;struct Cyc_List_List*_tmp49F;_tmpD2=((_tmp49F=_cycalloc(sizeof(*_tmp49F)),((_tmp49F->hd=((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A0)))),((_tmp49F->tl=_tmpD2,_tmp49F))))));}
# 769
_tmpD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD2);
return _tmpD2;}
# 775
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 777
struct Cyc_List_List*_tmpD5=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp4A1;_tmpD5=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4A1->tl=_tmpD5,_tmp4A1))))));}{
struct _dyneither_ptr*_tmp4A4;struct Cyc_List_List*_tmp4A3;_tmpD5=((_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->hd=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A4)))),((_tmp4A3->tl=_tmpD5,_tmp4A3))))));};}
# 782
_tmpD5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD5);
return _tmpD5;}
# 787
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL14:
 goto _LL15;case '"': _LL15:
 goto _LL16;case ';': _LL16:
 goto _LL17;case '&': _LL17:
 goto _LL18;case '(': _LL18:
 goto _LL19;case ')': _LL19:
 goto _LL1A;case '|': _LL1A:
 goto _LL1B;case '^': _LL1B:
 goto _LL1C;case '<': _LL1C:
 goto _LL1D;case '>': _LL1D:
 goto _LL1E;case '\n': _LL1E:
# 802
 goto _LL1F;case '\t': _LL1F:
 return 1;default: _LL20:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4AD(unsigned int*_tmp4AC,unsigned int*_tmp4AB,char**_tmp4A9){for(*_tmp4AC=0;*_tmp4AC < *_tmp4AB;(*_tmp4AC)++){(*_tmp4A9)[*_tmp4AC]=(char)'\000';}}
# 809
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpD9=Cyc_strlen((struct _dyneither_ptr)s);
# 813
int _tmpDA=0;
int _tmpDB=0;
{int i=0;for(0;i < _tmpD9;++ i){
char _tmpDC=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpDC == '\'')++ _tmpDA;else{
if(Cyc_is_other_special(_tmpDC))++ _tmpDB;}}}
# 822
if(_tmpDA == 0  && _tmpDB == 0)
return s;
# 826
if(_tmpDA == 0){
struct _dyneither_ptr*_tmp4A7;struct _dyneither_ptr*_tmp4A6[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4A6[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4A6[1]=((_tmp4A7=_cycalloc(sizeof(*_tmp4A7)),((_tmp4A7[0]=(struct _dyneither_ptr)s,_tmp4A7)))),((_tmp4A6[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A6,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 830
unsigned long _tmpE1=(_tmpD9 + _tmpDA)+ _tmpDB;
unsigned int _tmp4AC;unsigned int _tmp4AB;struct _dyneither_ptr _tmp4AA;char*_tmp4A9;unsigned int _tmp4A8;struct _dyneither_ptr s2=(_tmp4A8=_tmpE1 + 1,((_tmp4A9=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4A8 + 1)),((_tmp4AA=_tag_dyneither(_tmp4A9,sizeof(char),_tmp4A8 + 1),((((_tmp4AB=_tmp4A8,((_tmp4AD(& _tmp4AC,& _tmp4AB,& _tmp4A9),_tmp4A9[_tmp4AB]=(char)0)))),_tmp4AA)))))));
int _tmpE2=0;
int _tmpE3=0;
for(0;_tmpE2 < _tmpD9;++ _tmpE2){
char _tmpE4=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE2));
if(_tmpE4 == '\''  || Cyc_is_other_special(_tmpE4)){
char _tmp4B0;char _tmp4AF;struct _dyneither_ptr _tmp4AE;(_tmp4AE=_dyneither_ptr_plus(s2,sizeof(char),_tmpE3 ++),((_tmp4AF=*((char*)_check_dyneither_subscript(_tmp4AE,sizeof(char),0)),((_tmp4B0='\\',((_get_dyneither_size(_tmp4AE,sizeof(char))== 1  && (_tmp4AF == '\000'  && _tmp4B0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4AE.curr)=_tmp4B0)))))));}{
char _tmp4B3;char _tmp4B2;struct _dyneither_ptr _tmp4B1;(_tmp4B1=_dyneither_ptr_plus(s2,sizeof(char),_tmpE3 ++),((_tmp4B2=*((char*)_check_dyneither_subscript(_tmp4B1,sizeof(char),0)),((_tmp4B3=_tmpE4,((_get_dyneither_size(_tmp4B1,sizeof(char))== 1  && (_tmp4B2 == '\000'  && _tmp4B3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4B1.curr)=_tmp4B3)))))));};}
# 840
return(struct _dyneither_ptr)s2;};}
# 842
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4B4;return(_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4[0]=Cyc_sh_escape_string(*sp),_tmp4B4)));}
# 846
static void Cyc_process_file(struct _dyneither_ptr filename){
# 848
struct _dyneither_ptr _tmpF0=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4B5;struct _dyneither_ptr _tmpF1=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4B5=".cyp",_tag_dyneither(_tmp4B5,sizeof(char),5))));
const char*_tmp4B6;struct _dyneither_ptr _tmpF2=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4B6=".cyci",_tag_dyneither(_tmp4B6,sizeof(char),6))));
const char*_tmp4B7;struct _dyneither_ptr _tmpF3=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4B7=".cycio",_tag_dyneither(_tmp4B7,sizeof(char),7))));
const char*_tmp4B8;struct _dyneither_ptr _tmpF4=Cyc_strconcat((struct _dyneither_ptr)_tmpF0,((_tmp4B8=".c",_tag_dyneither(_tmp4B8,sizeof(char),3))));
# 855
if(Cyc_v_r){const char*_tmp4BC;void*_tmp4BB[1];struct Cyc_String_pa_PrintArg_struct _tmp4BA;(_tmp4BA.tag=0,((_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4BB[0]=& _tmp4BA,Cyc_fprintf(Cyc_stderr,((_tmp4BC="Compiling %s\n",_tag_dyneither(_tmp4BC,sizeof(char),14))),_tag_dyneither(_tmp4BB,sizeof(void*),1)))))));}{
# 858
const char*_tmp4BE;const char*_tmp4BD;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4BD="r",_tag_dyneither(_tmp4BD,sizeof(char),2))),((_tmp4BE="input file",_tag_dyneither(_tmp4BE,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 865
const char*_tmp4C5;struct _dyneither_ptr*_tmp4C4;const char*_tmp4C3;struct Cyc_List_List*_tmp4C2;struct _dyneither_ptr _tmpF8=
Cyc_str_sepstr(((_tmp4C2=_cycalloc(sizeof(*_tmp4C2)),((_tmp4C2->hd=((_tmp4C4=_cycalloc(sizeof(*_tmp4C4)),((_tmp4C4[0]=(struct _dyneither_ptr)((_tmp4C3="",_tag_dyneither(_tmp4C3,sizeof(char),1))),_tmp4C4)))),((_tmp4C2->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4C2)))))),(
(_tmp4C5=" ",_tag_dyneither(_tmp4C5,sizeof(char),2))));
# 875
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpF9=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4C6;_tmpF9=Cyc_add_subdir(_tmpF9,((_tmp4C6="include",_tag_dyneither(_tmp4C6,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4C9;struct Cyc_List_List*_tmp4C8;_tmpF9=((_tmp4C8=_cycalloc(sizeof(*_tmp4C8)),((_tmp4C8->hd=((_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9[0]=def_inc_path,_tmp4C9)))),((_tmp4C8->tl=_tmpF9,_tmp4C8))))));}{
# 881
char*_tmpFD=getenv("CYCLONE_INCLUDE_PATH");
if(_tmpFD != 0){
char*_tmp4CA;_tmpF9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4CA=_tmpFD,_tag_dyneither(_tmp4CA,sizeof(char),_get_zero_arr_size_char((void*)_tmp4CA,1)))),':'),_tmpF9);}{
# 886
const char*_tmp4D1;struct _dyneither_ptr*_tmp4D0;const char*_tmp4CF;struct Cyc_List_List*_tmp4CE;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4CE=_cycalloc(sizeof(*_tmp4CE)),((_tmp4CE->hd=((_tmp4D0=_cycalloc(sizeof(*_tmp4D0)),((_tmp4D0[0]=(struct _dyneither_ptr)((_tmp4CF="",_tag_dyneither(_tmp4CF,sizeof(char),1))),_tmp4D0)))),((_tmp4CE->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpF9),_tmp4CE)))))),(
(_tmp4D1=" -I",_tag_dyneither(_tmp4D1,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4D5;void*_tmp4D4[1];struct Cyc_String_pa_PrintArg_struct _tmp4D3;ofile_string=(struct _dyneither_ptr)((_tmp4D3.tag=0,((_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4D4[0]=& _tmp4D3,Cyc_aprintf(((_tmp4D5=" > %s",_tag_dyneither(_tmp4D5,sizeof(char),6))),_tag_dyneither(_tmp4D4,sizeof(void*),1))))))));}else{
# 895
const char*_tmp4D6;ofile_string=((_tmp4D6="",_tag_dyneither(_tmp4D6,sizeof(char),1)));}}else{
# 897
const char*_tmp4DA;void*_tmp4D9[1];struct Cyc_String_pa_PrintArg_struct _tmp4D8;ofile_string=(struct _dyneither_ptr)((_tmp4D8.tag=0,((_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF1)),((_tmp4D9[0]=& _tmp4D8,Cyc_aprintf(((_tmp4DA=" > %s",_tag_dyneither(_tmp4DA,sizeof(char),6))),_tag_dyneither(_tmp4D9,sizeof(void*),1))))))));}{
# 899
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 903
if(Cyc_dependencies_target == 0){
# 907
const char*_tmp4DB;fixup_string=((_tmp4DB=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4DB,sizeof(char),35)));}else{
# 912
const char*_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_PrintArg_struct _tmp4DD;fixup_string=(struct _dyneither_ptr)(
(_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4DE[0]=& _tmp4DD,Cyc_aprintf(((_tmp4DF=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4DF,sizeof(char),29))),_tag_dyneither(_tmp4DE,sizeof(void*),1))))))));}}else{
# 916
const char*_tmp4E0;fixup_string=((_tmp4E0="",_tag_dyneither(_tmp4E0,sizeof(char),1)));}{
# 918
const char*_tmp4E9;void*_tmp4E8[6];struct Cyc_String_pa_PrintArg_struct _tmp4E7;struct Cyc_String_pa_PrintArg_struct _tmp4E6;struct Cyc_String_pa_PrintArg_struct _tmp4E5;struct Cyc_String_pa_PrintArg_struct _tmp4E4;struct Cyc_String_pa_PrintArg_struct _tmp4E3;struct Cyc_String_pa_PrintArg_struct _tmp4E2;struct _dyneither_ptr _tmp10B=(_tmp4E2.tag=0,((_tmp4E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4E3.tag=0,((_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4E4.tag=0,((_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 921
Cyc_sh_escape_string(filename)),((_tmp4E5.tag=0,((_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF8),((_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4E8[0]=& _tmp4E7,((_tmp4E8[1]=& _tmp4E6,((_tmp4E8[2]=& _tmp4E5,((_tmp4E8[3]=& _tmp4E4,((_tmp4E8[4]=& _tmp4E3,((_tmp4E8[5]=& _tmp4E2,Cyc_aprintf(((_tmp4E9="%s %s%s %s%s%s",_tag_dyneither(_tmp4E9,sizeof(char),15))),_tag_dyneither(_tmp4E8,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 924
if(Cyc_v_r){const char*_tmp4ED;void*_tmp4EC[1];struct Cyc_String_pa_PrintArg_struct _tmp4EB;(_tmp4EB.tag=0,((_tmp4EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10B),((_tmp4EC[0]=& _tmp4EB,Cyc_fprintf(Cyc_stderr,((_tmp4ED="%s\n",_tag_dyneither(_tmp4ED,sizeof(char),4))),_tag_dyneither(_tmp4EC,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp10B,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4F0;void*_tmp4EF;(_tmp4EF=0,Cyc_fprintf(Cyc_stderr,((_tmp4F0="\nError: preprocessing\n",_tag_dyneither(_tmp4F0,sizeof(char),23))),_tag_dyneither(_tmp4EF,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
return;}
# 931
if(Cyc_stop_after_cpp_r)return;
# 934
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF1);{
const char*_tmp4F2;const char*_tmp4F1;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF1,((_tmp4F1="r",_tag_dyneither(_tmp4F1,sizeof(char),2))),((_tmp4F2="file",_tag_dyneither(_tmp4F2,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 938
struct Cyc_List_List*tds=0;
# 941
{struct _handler_cons _tmp111;_push_handler(& _tmp111);{int _tmp113=0;if(setjmp(_tmp111.handler))_tmp113=1;if(!_tmp113){
{const char*_tmp4F3;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F3="parsing",_tag_dyneither(_tmp4F3,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp112=(void*)_exn_thrown;void*_tmp116=_tmp112;void*_tmp117;void*_tmp118;_LL23: _tmp117=_tmp116;_LL24:
# 945
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
Cyc_Core_rethrow(_tmp117);_LL25: _tmp118=_tmp116;_LL26:(void)_throw(_tmp118);_LL22:;}};}
# 949
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
return;}
# 955
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 960
struct _RegionHandle _tmp119=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp119;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp11A=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 965
struct _tuple22 _tmp4F4;struct _tuple22 _tmp11B=(_tmp4F4.f1=tc_rgn,((_tmp4F4.f2=_tmp11A,_tmp4F4)));
{const char*_tmp4F5;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F5="type checking",_tag_dyneither(_tmp4F5,sizeof(char),14))),Cyc_do_typecheck,& _tmp11B,tds);}
# 968
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 972
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4F6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F6="control-flow checking",_tag_dyneither(_tmp4F6,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 980
if(Cyc_generate_interface_r){
const char*_tmp4F9;const char*_tmp4F8;const char*_tmp4F7;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4F7=(const char*)_untag_dyneither_ptr(_tmpF2,sizeof(char),1),_tag_dyneither(_tmp4F7,sizeof(char),_get_zero_arr_size_char((void*)_tmp4F7,1)))),((_tmp4F8="w",_tag_dyneither(_tmp4F8,sizeof(char),2))),((_tmp4F9="interface file",_tag_dyneither(_tmp4F9,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 985
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 992
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpF2,sizeof(char),1),"r");
const char*_tmp4FB;const char*_tmp4FA;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpF3,((_tmp4FA="w",_tag_dyneither(_tmp4FA,sizeof(char),2))),((_tmp4FB="interface object file",_tag_dyneither(_tmp4FB,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 999
Cyc_Position_reset_position(_tmpF2);{
struct _tuple23 _tmp4FC;struct _tuple23 _tmp121=(_tmp4FC.f1=_tmp11A,((_tmp4FC.f2=inter_file,((_tmp4FC.f3=inter_objfile,_tmp4FC)))));
{const char*_tmp4FD;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FD="interface checking",_tag_dyneither(_tmp4FD,sizeof(char),19))),Cyc_do_interface,& _tmp121,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1007
if(Cyc_cf_r)goto PRINTC;
# 1010
{const char*_tmp4FE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FE="translation to C",_tag_dyneither(_tmp4FE,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
_npop_handler(0);return;}
# 1016
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpF1);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1024
const char*_tmp4FF;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FF="post-pass to VC",_tag_dyneither(_tmp4FF,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1031
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp501;const char*_tmp500;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp500="w",_tag_dyneither(_tmp500,sizeof(char),2))),((_tmp501="output file",_tag_dyneither(_tmp501,sizeof(char),12))));}else{
# 1037
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp503;const char*_tmp502;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp502="w",_tag_dyneither(_tmp502,sizeof(char),2))),((_tmp503="output file",_tag_dyneither(_tmp503,sizeof(char),12))));}else{
# 1041
const char*_tmp505;const char*_tmp504;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF4,((_tmp504="w",_tag_dyneither(_tmp504,sizeof(char),2))),((_tmp505="output file",_tag_dyneither(_tmp505,sizeof(char),12))));}}
# 1043
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1045
{struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
if(!Cyc_noprint_r){
const char*_tmp506;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp506="printing",_tag_dyneither(_tmp506,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1046
;_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp134=_tmp130;void*_tmp135;void*_tmp136;_LL28: _tmp135=_tmp134;_LL29:
# 1050
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp509;struct Cyc_List_List*_tmp508;Cyc_cfiles=((_tmp508=_cycalloc(sizeof(*_tmp508)),((_tmp508->hd=((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509[0]=(struct _dyneither_ptr)_tmpF4,_tmp509)))),((_tmp508->tl=Cyc_cfiles,_tmp508))))));}
Cyc_Core_rethrow(_tmp135);_LL2A: _tmp136=_tmp134;_LL2B:(void)_throw(_tmp136);_LL27:;}};}
# 1055
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp50C;struct Cyc_List_List*_tmp50B;Cyc_cfiles=((_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B->hd=((_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C[0]=(struct _dyneither_ptr)_tmpF4,_tmp50C)))),((_tmp50B->tl=Cyc_cfiles,_tmp50B))))));};}
# 960
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp153[8]="<final>";
# 1060
static struct _dyneither_ptr Cyc_final_str={_tmp153,_tmp153,_tmp153 + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1063
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1067
struct _dyneither_ptr basename;
{struct _handler_cons _tmp154;_push_handler(& _tmp154);{int _tmp156=0;if(setjmp(_tmp154.handler))_tmp156=1;if(!_tmp156){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp155=(void*)_exn_thrown;void*_tmp158=_tmp155;void*_tmp15A;_LL2D: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp159=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp158;if(_tmp159->tag != Cyc_Core_Invalid_argument)goto _LL2F;}_LL2E:
# 1071
 basename=*n;goto _LL2C;_LL2F: _tmp15A=_tmp158;_LL30:(void)_throw(_tmp15A);_LL2C:;}};}{
# 1073
const char*_tmp50D;struct _dyneither_ptr _tmp15B=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp50D=".cycio",_tag_dyneither(_tmp50D,sizeof(char),7))));
const char*_tmp50F;const char*_tmp50E;struct Cyc___cycFILE*_tmp15C=Cyc_try_file_open((struct _dyneither_ptr)_tmp15B,((_tmp50E="rb",_tag_dyneither(_tmp50E,sizeof(char),3))),((_tmp50F="interface object file",_tag_dyneither(_tmp50F,sizeof(char),22))));
if(_tmp15C == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1080
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15B);{
# 1082
struct Cyc_Interface_I*_tmp15D=Cyc_Interface_load(_tmp15C);
Cyc_file_close(_tmp15C);
return _tmp15D;};};};}
# 1087
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1091
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp515(unsigned int*_tmp514,unsigned int*_tmp513,int**_tmp511){for(*_tmp514=0;*_tmp514 < *_tmp513;(*_tmp514)++){(*_tmp511)[*_tmp514]=(int)0;}}static void _tmp51D(unsigned int*_tmp51C,unsigned int*_tmp51B,struct _dyneither_ptr**_tmp519){for(*_tmp51C=0;*_tmp51C < *_tmp51B;(*_tmp51C)++){(*_tmp519)[*_tmp51C]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1128
_tag_dyneither(0,0,0));}}static void _tmp523(unsigned int*_tmp522,unsigned int*_tmp521,struct _dyneither_ptr**_tmp51F){for(*_tmp522=0;*_tmp522 < *_tmp521;(*_tmp522)++){(*_tmp51F)[*_tmp522]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1094
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1099
GC_blacklist_warn_clear();{
# 1101
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1106
struct _RegionHandle _tmp161=_new_region("r");struct _RegionHandle*r=& _tmp161;_push_region(r);{
# 1108
unsigned int _tmp514;unsigned int _tmp513;struct _dyneither_ptr _tmp512;int*_tmp511;unsigned int _tmp510;struct _dyneither_ptr _tmp162=(_tmp510=(unsigned int)argc,((_tmp511=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp510)),((_tmp512=_tag_dyneither(_tmp511,sizeof(int),_tmp510),((((_tmp513=_tmp510,_tmp515(& _tmp514,& _tmp513,& _tmp511))),_tmp512)))))));
int _tmp163=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp516;if(Cyc_strncmp(((_tmp516="-B",_tag_dyneither(_tmp516,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;}else{
# 1116
const char*_tmp517;if(Cyc_strcmp(((_tmp517="-b",_tag_dyneither(_tmp517,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i))=1;
++ _tmp163;}}}{
# 1128
unsigned int _tmp51C;unsigned int _tmp51B;struct _dyneither_ptr _tmp51A;struct _dyneither_ptr*_tmp519;unsigned int _tmp518;struct _dyneither_ptr _tmp166=(_tmp518=(unsigned int)(_tmp163 + 1),((_tmp519=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp518)),((_tmp51A=_tag_dyneither(_tmp519,sizeof(struct _dyneither_ptr),_tmp518),((((_tmp51B=_tmp518,_tmp51D(& _tmp51C,& _tmp51B,& _tmp519))),_tmp51A)))))));
unsigned int _tmp522;unsigned int _tmp521;struct _dyneither_ptr _tmp520;struct _dyneither_ptr*_tmp51F;unsigned int _tmp51E;struct _dyneither_ptr _tmp167=(_tmp51E=(unsigned int)(argc - _tmp163),((_tmp51F=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp51E)),((_tmp520=_tag_dyneither(_tmp51F,sizeof(struct _dyneither_ptr),_tmp51E),((((_tmp521=_tmp51E,_tmp523(& _tmp522,& _tmp521,& _tmp51F))),_tmp520)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp162,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp166,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1137
struct _tuple24*_tmpAB2;const char*_tmpAB1;const char*_tmpAB0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpAAF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpAAE;const char*_tmpAAD;struct _tuple24*_tmpAAC;const char*_tmpAAB;const char*_tmpAAA;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpAA9;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpAA8;const char*_tmpAA7;struct _tuple24*_tmpAA6;const char*_tmpAA5;const char*_tmpAA4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpAA3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpAA2;const char*_tmpAA1;struct _tuple24*_tmpAA0;const char*_tmpA9F;const char*_tmpA9E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA9D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA9C;const char*_tmpA9B;struct _tuple24*_tmpA9A;const char*_tmpA99;const char*_tmpA98;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA97;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA96;const char*_tmpA95;struct _tuple24*_tmpA94;const char*_tmpA93;const char*_tmpA92;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA91;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA90;const char*_tmpA8F;struct _tuple24*_tmpA8E;const char*_tmpA8D;const char*_tmpA8C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA8B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA8A;const char*_tmpA89;struct _tuple24*_tmpA88;const char*_tmpA87;const char*_tmpA86;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA85;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA84;const char*_tmpA83;struct _tuple24*_tmpA82;const char*_tmpA81;const char*_tmpA80;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA7F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA7E;const char*_tmpA7D;struct _tuple24*_tmpA7C;const char*_tmpA7B;const char*_tmpA7A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA79;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA78;const char*_tmpA77;struct _tuple24*_tmpA76;const char*_tmpA75;const char*_tmpA74;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA73;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA72;const char*_tmpA71;struct _tuple24*_tmpA70;const char*_tmpA6F;const char*_tmpA6E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6C;const char*_tmpA6B;struct _tuple24*_tmpA6A;const char*_tmpA69;const char*_tmpA68;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA67;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA66;const char*_tmpA65;struct _tuple24*_tmpA64;const char*_tmpA63;const char*_tmpA62;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA61;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA60;const char*_tmpA5F;struct _tuple24*_tmpA5E;const char*_tmpA5D;const char*_tmpA5C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5A;const char*_tmpA59;struct _tuple24*_tmpA58;const char*_tmpA57;const char*_tmpA56;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA55;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA54;const char*_tmpA53;struct _tuple24*_tmpA52;const char*_tmpA51;const char*_tmpA50;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA4F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA4E;const char*_tmpA4D;struct _tuple24*_tmpA4C;const char*_tmpA4B;const char*_tmpA4A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA49;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA48;const char*_tmpA47;struct _tuple24*_tmpA46;const char*_tmpA45;const char*_tmpA44;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA43;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA42;const char*_tmpA41;struct _tuple24*_tmpA40;const char*_tmpA3F;const char*_tmpA3E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA3D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA3C;const char*_tmpA3B;struct _tuple24*_tmpA3A;const char*_tmpA39;const char*_tmpA38;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA37;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA36;const char*_tmpA35;struct _tuple24*_tmpA34;const char*_tmpA33;const char*_tmpA32;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA31;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA30;const char*_tmpA2F;struct _tuple24*_tmpA2E;const char*_tmpA2D;const char*_tmpA2C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA2B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA2A;const char*_tmpA29;struct _tuple24*_tmpA28;const char*_tmpA27;const char*_tmpA26;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA25;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA24;const char*_tmpA23;struct _tuple24*_tmpA22;const char*_tmpA21;const char*_tmpA20;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA1F;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA1E;const char*_tmpA1D;struct _tuple24*_tmpA1C;const char*_tmpA1B;const char*_tmpA1A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA19;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA18;const char*_tmpA17;struct _tuple24*_tmpA16;const char*_tmpA15;const char*_tmpA14;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA13;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA12;const char*_tmpA11;struct _tuple24*_tmpA10;const char*_tmpA0F;const char*_tmpA0E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA0D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA0C;const char*_tmpA0B;struct _tuple24*_tmpA0A;const char*_tmpA09;const char*_tmpA08;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA07;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA06;const char*_tmpA05;struct _tuple24*_tmpA04;const char*_tmpA03;const char*_tmpA02;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA01;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA00;const char*_tmp9FF;struct _tuple24*_tmp9FE;const char*_tmp9FD;const char*_tmp9FC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9FB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9FA;const char*_tmp9F9;struct _tuple24*_tmp9F8;const char*_tmp9F7;const char*_tmp9F6;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F5;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F4;const char*_tmp9F3;struct _tuple24*_tmp9F2;const char*_tmp9F1;const char*_tmp9F0;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9EF;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9EE;const char*_tmp9ED;struct _tuple24*_tmp9EC;const char*_tmp9EB;const char*_tmp9EA;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E9;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E8;const char*_tmp9E7;struct _tuple24*_tmp9E6;const char*_tmp9E5;const char*_tmp9E4;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E3;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E2;const char*_tmp9E1;struct _tuple24*_tmp9E0;const char*_tmp9DF;const char*_tmp9DE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DC;const char*_tmp9DB;struct _tuple24*_tmp9DA;const char*_tmp9D9;const char*_tmp9D8;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D7;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D6;const char*_tmp9D5;struct _tuple24*_tmp9D4;const char*_tmp9D3;const char*_tmp9D2;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9D1;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9D0;const char*_tmp9CF;struct _tuple24*_tmp9CE;const char*_tmp9CD;const char*_tmp9CC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CA;const char*_tmp9C9;struct _tuple24*_tmp9C8;const char*_tmp9C7;const char*_tmp9C6;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9C5;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9C4;const char*_tmp9C3;struct _tuple24*_tmp9C2;const char*_tmp9C1;const char*_tmp9C0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9BF;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9BE;const char*_tmp9BD;struct _tuple24*_tmp9BC;const char*_tmp9BB;const char*_tmp9BA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9B9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9B8;const char*_tmp9B7;struct _tuple24*_tmp9B6;const char*_tmp9B5;const char*_tmp9B4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B2;const char*_tmp9B1;struct _tuple24*_tmp9B0;const char*_tmp9AF;const char*_tmp9AE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9AD;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9AC;const char*_tmp9AB;struct _tuple24*_tmp9AA;const char*_tmp9A9;const char*_tmp9A8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A7;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A6;const char*_tmp9A5;struct _tuple24*_tmp9A4;const char*_tmp9A3;const char*_tmp9A2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A0;const char*_tmp99F;struct _tuple24*_tmp99E;const char*_tmp99D;const char*_tmp99C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp99B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp99A;const char*_tmp999;struct _tuple24*_tmp998;const char*_tmp997;const char*_tmp996;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp995;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp994;const char*_tmp993;struct _tuple24*_tmp992;const char*_tmp991;const char*_tmp990;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp98F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98E;const char*_tmp98D;struct _tuple24*_tmp98C;const char*_tmp98B;const char*_tmp98A;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp989;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp988;const char*_tmp987;struct _tuple24*_tmp986;const char*_tmp985;const char*_tmp984;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp983;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp982;const char*_tmp981;struct _tuple24*_tmp980;const char*_tmp97F;const char*_tmp97E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97C;const char*_tmp97B;struct _tuple24*_tmp97A;const char*_tmp979;const char*_tmp978;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp977;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp976;const char*_tmp975;struct _tuple24*_tmp974;const char*_tmp973;const char*_tmp972;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp971;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp970;const char*_tmp96F;struct _tuple24*_tmp96E;const char*_tmp96D;const char*_tmp96C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96A;const char*_tmp969;struct _tuple24*_tmp968;const char*_tmp967;const char*_tmp966;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp965;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp964;const char*_tmp963;struct _tuple24*_tmp962;const char*_tmp961;const char*_tmp960;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95E;const char*_tmp95D;struct _tuple24*_tmp95C;const char*_tmp95B;const char*_tmp95A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp959;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp958;const char*_tmp957;struct _tuple24*_tmp956;const char*_tmp955;const char*_tmp954;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp953;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp952;const char*_tmp951;struct _tuple24*_tmp950;const char*_tmp94F;const char*_tmp94E;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94D;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94C;const char*_tmp94B;struct _tuple24*_tmp94A;const char*_tmp949;const char*_tmp948;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp947;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp946;const char*_tmp945;struct _tuple24*_tmp944;const char*_tmp943;const char*_tmp942;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp941;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp940;const char*_tmp93F;struct _tuple24*_tmp93E;const char*_tmp93D;const char*_tmp93C;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93B;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93A;const char*_tmp939;struct _tuple24*_tmp938;const char*_tmp937;const char*_tmp936;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp935;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp934;const char*_tmp933;struct _tuple24*_tmp932;const char*_tmp931;const char*_tmp930;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92F;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92E;const char*_tmp92D;struct _tuple24*_tmp92C;const char*_tmp92B;const char*_tmp92A;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp929;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp928;const char*_tmp927;struct _tuple24*_tmp926;const char*_tmp925;const char*_tmp924;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp923;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp922;const char*_tmp921;struct _tuple24*_tmp920;const char*_tmp91F;const char*_tmp91E;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp91D;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp91C;const char*_tmp91B;struct _tuple24*_tmp91A;const char*_tmp919;const char*_tmp918;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp917;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp916;const char*_tmp915;struct _tuple24*_tmp914;const char*_tmp913;const char*_tmp912;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp911;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp910;const char*_tmp90F;struct _tuple24*_tmp90E;const char*_tmp90D;const char*_tmp90C;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp90B;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp90A;const char*_tmp909;struct _tuple24*_tmp908;const char*_tmp907;const char*_tmp906;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp905;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp904;const char*_tmp903;struct _tuple24*_tmp902;const char*_tmp901;const char*_tmp900;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8FF;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8FE;const char*_tmp8FD;struct _tuple24*_tmp8FC;const char*_tmp8FB;const char*_tmp8FA;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F9;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F8;const char*_tmp8F7;struct _tuple24*_tmp8F6;const char*_tmp8F5;const char*_tmp8F4;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F3;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F2;const char*_tmp8F1;struct _tuple24*_tmp8F0;const char*_tmp8EF;const char*_tmp8EE;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8ED;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8EC;const char*_tmp8EB;struct _tuple24*_tmp8EA;const char*_tmp8E9;const char*_tmp8E8;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E7;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E6;const char*_tmp8E5;struct _tuple24*_tmp8E4;const char*_tmp8E3;const char*_tmp8E2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8E1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8E0;const char*_tmp8DF;struct _tuple24*_tmp8DE;const char*_tmp8DD;const char*_tmp8DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8DB;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8DA;const char*_tmp8D9;struct _tuple24*_tmp8D8[79];struct Cyc_List_List*options=
(_tmp8D8[78]=(
# 1375
(_tmp8DE=_region_malloc(r,sizeof(*_tmp8DE)),((_tmp8DE->f1=((_tmp8DD="--parsestate",_tag_dyneither(_tmp8DD,sizeof(char),13))),((_tmp8DE->f2=0,((_tmp8DE->f3=((_tmp8DC="",_tag_dyneither(_tmp8DC,sizeof(char),1))),((_tmp8DE->f4=(void*)(
(_tmp8DA=_region_malloc(r,sizeof(*_tmp8DA)),((_tmp8DA[0]=((_tmp8DB.tag=3,((_tmp8DB.f1=& Cyc_ParseErrors_print_state_and_token,_tmp8DB)))),_tmp8DA)))),((_tmp8DE->f5=(
(_tmp8D9="print the parse state and token on a syntax error",_tag_dyneither(_tmp8D9,sizeof(char),50))),_tmp8DE)))))))))))),((_tmp8D8[77]=(
# 1372
(_tmp8E4=_region_malloc(r,sizeof(*_tmp8E4)),((_tmp8E4->f1=((_tmp8E3="--manyerrors",_tag_dyneither(_tmp8E3,sizeof(char),13))),((_tmp8E4->f2=0,((_tmp8E4->f3=((_tmp8E2="",_tag_dyneither(_tmp8E2,sizeof(char),1))),((_tmp8E4->f4=(void*)(
(_tmp8E0=_region_malloc(r,sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=0,((_tmp8E1.f1=Cyc_set_many_errors,_tmp8E1)))),_tmp8E0)))),((_tmp8E4->f5=(
(_tmp8DF="don't stop after only a few errors.",_tag_dyneither(_tmp8DF,sizeof(char),36))),_tmp8E4)))))))))))),((_tmp8D8[76]=(
# 1369
(_tmp8EA=_region_malloc(r,sizeof(*_tmp8EA)),((_tmp8EA->f1=((_tmp8E9="--interface",_tag_dyneither(_tmp8E9,sizeof(char),12))),((_tmp8EA->f2=0,((_tmp8EA->f3=((_tmp8E8=" <file>",_tag_dyneither(_tmp8E8,sizeof(char),8))),((_tmp8EA->f4=(void*)(
(_tmp8E6=_region_malloc(r,sizeof(*_tmp8E6)),((_tmp8E6[0]=((_tmp8E7.tag=5,((_tmp8E7.f1=Cyc_set_specified_interface,_tmp8E7)))),_tmp8E6)))),((_tmp8EA->f5=(
(_tmp8E5="Set the interface file to be <file>.",_tag_dyneither(_tmp8E5,sizeof(char),37))),_tmp8EA)))))))))))),((_tmp8D8[75]=(
# 1366
(_tmp8F0=_region_malloc(r,sizeof(*_tmp8F0)),((_tmp8F0->f1=((_tmp8EF="--geninterface",_tag_dyneither(_tmp8EF,sizeof(char),15))),((_tmp8F0->f2=0,((_tmp8F0->f3=((_tmp8EE="",_tag_dyneither(_tmp8EE,sizeof(char),1))),((_tmp8F0->f4=(void*)(
(_tmp8EC=_region_malloc(r,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=3,((_tmp8ED.f1=& Cyc_generate_interface_r,_tmp8ED)))),_tmp8EC)))),((_tmp8F0->f5=(
(_tmp8EB="Generate interface files",_tag_dyneither(_tmp8EB,sizeof(char),25))),_tmp8F0)))))))))))),((_tmp8D8[74]=(
# 1363
(_tmp8F6=_region_malloc(r,sizeof(*_tmp8F6)),((_tmp8F6->f1=((_tmp8F5="-idirafter",_tag_dyneither(_tmp8F5,sizeof(char),11))),((_tmp8F6->f2=0,((_tmp8F6->f3=((_tmp8F4="<dir>",_tag_dyneither(_tmp8F4,sizeof(char),6))),((_tmp8F6->f4=(void*)(
(_tmp8F2=_region_malloc(r,sizeof(*_tmp8F2)),((_tmp8F2[0]=((_tmp8F3.tag=5,((_tmp8F3.f1=Cyc_add_idirafter,_tmp8F3)))),_tmp8F2)))),((_tmp8F6->f5=(
(_tmp8F1="Add the directory to the second include path.",_tag_dyneither(_tmp8F1,sizeof(char),46))),_tmp8F6)))))))))))),((_tmp8D8[73]=(
# 1360
(_tmp8FC=_region_malloc(r,sizeof(*_tmp8FC)),((_tmp8FC->f1=((_tmp8FB="-isystem",_tag_dyneither(_tmp8FB,sizeof(char),9))),((_tmp8FC->f2=0,((_tmp8FC->f3=((_tmp8FA="<dir>",_tag_dyneither(_tmp8FA,sizeof(char),6))),((_tmp8FC->f4=(void*)(
(_tmp8F8=_region_malloc(r,sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8F9.tag=5,((_tmp8F9.f1=Cyc_add_isystem,_tmp8F9)))),_tmp8F8)))),((_tmp8FC->f5=(
(_tmp8F7="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8F7,sizeof(char),90))),_tmp8FC)))))))))))),((_tmp8D8[72]=(
# 1357
(_tmp902=_region_malloc(r,sizeof(*_tmp902)),((_tmp902->f1=((_tmp901="-iwithprefixbefore",_tag_dyneither(_tmp901,sizeof(char),19))),((_tmp902->f2=0,((_tmp902->f3=((_tmp900="<dir>",_tag_dyneither(_tmp900,sizeof(char),6))),((_tmp902->f4=(void*)(
(_tmp8FE=_region_malloc(r,sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp8FF.tag=5,((_tmp8FF.f1=Cyc_add_iwithprefixbefore,_tmp8FF)))),_tmp8FE)))),((_tmp902->f5=(
(_tmp8FD="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8FD,sizeof(char),45))),_tmp902)))))))))))),((_tmp8D8[71]=(
# 1354
(_tmp908=_region_malloc(r,sizeof(*_tmp908)),((_tmp908->f1=((_tmp907="-iwithprefix",_tag_dyneither(_tmp907,sizeof(char),13))),((_tmp908->f2=0,((_tmp908->f3=((_tmp906="<dir>",_tag_dyneither(_tmp906,sizeof(char),6))),((_tmp908->f4=(void*)(
(_tmp904=_region_malloc(r,sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=5,((_tmp905.f1=Cyc_add_iwithprefix,_tmp905)))),_tmp904)))),((_tmp908->f5=(
(_tmp903="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp903,sizeof(char),47))),_tmp908)))))))))))),((_tmp8D8[70]=(
# 1351
(_tmp90E=_region_malloc(r,sizeof(*_tmp90E)),((_tmp90E->f1=((_tmp90D="-iprefix",_tag_dyneither(_tmp90D,sizeof(char),9))),((_tmp90E->f2=0,((_tmp90E->f3=((_tmp90C="<prefix>",_tag_dyneither(_tmp90C,sizeof(char),9))),((_tmp90E->f4=(void*)(
(_tmp90A=_region_malloc(r,sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=5,((_tmp90B.f1=Cyc_add_iprefix,_tmp90B)))),_tmp90A)))),((_tmp90E->f5=(
(_tmp909="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp909,sizeof(char),67))),_tmp90E)))))))))))),((_tmp8D8[69]=(
# 1348
(_tmp914=_region_malloc(r,sizeof(*_tmp914)),((_tmp914->f1=((_tmp913="-noregions",_tag_dyneither(_tmp913,sizeof(char),11))),((_tmp914->f2=0,((_tmp914->f3=((_tmp912="",_tag_dyneither(_tmp912,sizeof(char),1))),((_tmp914->f4=(void*)(
(_tmp910=_region_malloc(r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=3,((_tmp911.f1=& Cyc_Absyn_no_regions,_tmp911)))),_tmp910)))),((_tmp914->f5=(
(_tmp90F="Generate code that doesn't use regions",_tag_dyneither(_tmp90F,sizeof(char),39))),_tmp914)))))))))))),((_tmp8D8[68]=(
# 1345
(_tmp91A=_region_malloc(r,sizeof(*_tmp91A)),((_tmp91A->f1=((_tmp919="-port",_tag_dyneither(_tmp919,sizeof(char),6))),((_tmp91A->f2=0,((_tmp91A->f3=((_tmp918="",_tag_dyneither(_tmp918,sizeof(char),1))),((_tmp91A->f4=(void*)(
(_tmp916=_region_malloc(r,sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=0,((_tmp917.f1=Cyc_set_port_c_code,_tmp917)))),_tmp916)))),((_tmp91A->f5=(
(_tmp915="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp915,sizeof(char),38))),_tmp91A)))))))))))),((_tmp8D8[67]=(
# 1342
(_tmp920=_region_malloc(r,sizeof(*_tmp920)),((_tmp920->f1=((_tmp91F="-detailedlocation",_tag_dyneither(_tmp91F,sizeof(char),18))),((_tmp920->f2=0,((_tmp920->f3=((_tmp91E="",_tag_dyneither(_tmp91E,sizeof(char),1))),((_tmp920->f4=(void*)(
(_tmp91C=_region_malloc(r,sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91D.tag=4,((_tmp91D.f1=& Cyc_Position_use_gcc_style_location,_tmp91D)))),_tmp91C)))),((_tmp920->f5=(
(_tmp91B="Try to give more detailed location information for errors",_tag_dyneither(_tmp91B,sizeof(char),58))),_tmp920)))))))))))),((_tmp8D8[66]=(
# 1339
(_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926->f1=((_tmp925="-noregister",_tag_dyneither(_tmp925,sizeof(char),12))),((_tmp926->f2=0,((_tmp926->f3=((_tmp924="",_tag_dyneither(_tmp924,sizeof(char),1))),((_tmp926->f4=(void*)(
(_tmp922=_region_malloc(r,sizeof(*_tmp922)),((_tmp922[0]=((_tmp923.tag=3,((_tmp923.f1=& Cyc_Parse_no_register,_tmp923)))),_tmp922)))),((_tmp926->f5=(
(_tmp921="Treat register storage class as public",_tag_dyneither(_tmp921,sizeof(char),39))),_tmp926)))))))))))),((_tmp8D8[65]=(
# 1336
(_tmp92C=_region_malloc(r,sizeof(*_tmp92C)),((_tmp92C->f1=((_tmp92B="-warnregioncoerce",_tag_dyneither(_tmp92B,sizeof(char),18))),((_tmp92C->f2=0,((_tmp92C->f3=((_tmp92A="",_tag_dyneither(_tmp92A,sizeof(char),1))),((_tmp92C->f4=(void*)(
(_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928[0]=((_tmp929.tag=3,((_tmp929.f1=& Cyc_Tcutil_warn_region_coerce,_tmp929)))),_tmp928)))),((_tmp92C->f5=(
(_tmp927="Warn when any region coercion is inserted",_tag_dyneither(_tmp927,sizeof(char),42))),_tmp92C)))))))))))),((_tmp8D8[64]=(
# 1333
(_tmp932=_region_malloc(r,sizeof(*_tmp932)),((_tmp932->f1=((_tmp931="-warnaliascoercion",_tag_dyneither(_tmp931,sizeof(char),19))),((_tmp932->f2=0,((_tmp932->f3=((_tmp930="",_tag_dyneither(_tmp930,sizeof(char),1))),((_tmp932->f4=(void*)(
(_tmp92E=_region_malloc(r,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=3,((_tmp92F.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp92F)))),_tmp92E)))),((_tmp932->f5=(
(_tmp92D="Warn when any alias coercion is inserted",_tag_dyneither(_tmp92D,sizeof(char),41))),_tmp932)))))))))))),((_tmp8D8[63]=(
# 1330
(_tmp938=_region_malloc(r,sizeof(*_tmp938)),((_tmp938->f1=((_tmp937="-warnnullchecks",_tag_dyneither(_tmp937,sizeof(char),16))),((_tmp938->f2=0,((_tmp938->f3=((_tmp936="",_tag_dyneither(_tmp936,sizeof(char),1))),((_tmp938->f4=(void*)(
(_tmp934=_region_malloc(r,sizeof(*_tmp934)),((_tmp934[0]=((_tmp935.tag=3,((_tmp935.f1=& Cyc_Toc_warn_all_null_deref,_tmp935)))),_tmp934)))),((_tmp938->f5=(
(_tmp933="Warn when any null check can't be eliminated",_tag_dyneither(_tmp933,sizeof(char),45))),_tmp938)))))))))))),((_tmp8D8[62]=(
# 1327
(_tmp93E=_region_malloc(r,sizeof(*_tmp93E)),((_tmp93E->f1=((_tmp93D="-warnboundschecks",_tag_dyneither(_tmp93D,sizeof(char),18))),((_tmp93E->f2=0,((_tmp93E->f3=((_tmp93C="",_tag_dyneither(_tmp93C,sizeof(char),1))),((_tmp93E->f4=(void*)(
(_tmp93A=_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A[0]=((_tmp93B.tag=3,((_tmp93B.f1=& Cyc_Toc_warn_bounds_checks,_tmp93B)))),_tmp93A)))),((_tmp93E->f5=(
(_tmp939="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp939,sizeof(char),44))),_tmp93E)))))))))))),((_tmp8D8[61]=(
# 1324
(_tmp944=_region_malloc(r,sizeof(*_tmp944)),((_tmp944->f1=((_tmp943="-CI",_tag_dyneither(_tmp943,sizeof(char),4))),((_tmp944->f2=0,((_tmp944->f3=((_tmp942=" <file>",_tag_dyneither(_tmp942,sizeof(char),8))),((_tmp944->f4=(void*)(
(_tmp940=_region_malloc(r,sizeof(*_tmp940)),((_tmp940[0]=((_tmp941.tag=5,((_tmp941.f1=Cyc_set_cyc_include,_tmp941)))),_tmp940)))),((_tmp944->f5=(
(_tmp93F="Set cyc_include.h to be <file>",_tag_dyneither(_tmp93F,sizeof(char),31))),_tmp944)))))))))))),((_tmp8D8[60]=(
# 1321
(_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A->f1=((_tmp949="-compile-for-boot",_tag_dyneither(_tmp949,sizeof(char),18))),((_tmp94A->f2=0,((_tmp94A->f3=((_tmp948="",_tag_dyneither(_tmp948,sizeof(char),1))),((_tmp94A->f4=(void*)(
(_tmp946=_region_malloc(r,sizeof(*_tmp946)),((_tmp946[0]=((_tmp947.tag=3,((_tmp947.f1=& Cyc_Lex_compile_for_boot_r,_tmp947)))),_tmp946)))),((_tmp94A->f5=(
(_tmp945="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp945,sizeof(char),71))),_tmp94A)))))))))))),((_tmp8D8[59]=(
# 1318
(_tmp950=_region_malloc(r,sizeof(*_tmp950)),((_tmp950->f1=((_tmp94F="-nocyc_setjmp",_tag_dyneither(_tmp94F,sizeof(char),14))),((_tmp950->f2=0,((_tmp950->f3=((_tmp94E="",_tag_dyneither(_tmp94E,sizeof(char),1))),((_tmp950->f4=(void*)(
(_tmp94C=_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C[0]=((_tmp94D.tag=3,((_tmp94D.f1=& Cyc_nocyc_setjmp_r,_tmp94D)))),_tmp94C)))),((_tmp950->f5=(
(_tmp94B="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp94B,sizeof(char),46))),_tmp950)))))))))))),((_tmp8D8[58]=(
# 1315
(_tmp956=_region_malloc(r,sizeof(*_tmp956)),((_tmp956->f1=((_tmp955="-printalleffects",_tag_dyneither(_tmp955,sizeof(char),17))),((_tmp956->f2=0,((_tmp956->f3=((_tmp954="",_tag_dyneither(_tmp954,sizeof(char),1))),((_tmp956->f4=(void*)(
(_tmp952=_region_malloc(r,sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=3,((_tmp953.f1=& Cyc_print_all_effects_r,_tmp953)))),_tmp952)))),((_tmp956->f5=(
(_tmp951="Print effects for functions (type debugging)",_tag_dyneither(_tmp951,sizeof(char),45))),_tmp956)))))))))))),((_tmp8D8[57]=(
# 1312
(_tmp95C=_region_malloc(r,sizeof(*_tmp95C)),((_tmp95C->f1=((_tmp95B="-printfullevars",_tag_dyneither(_tmp95B,sizeof(char),16))),((_tmp95C->f2=0,((_tmp95C->f3=((_tmp95A="",_tag_dyneither(_tmp95A,sizeof(char),1))),((_tmp95C->f4=(void*)(
(_tmp958=_region_malloc(r,sizeof(*_tmp958)),((_tmp958[0]=((_tmp959.tag=3,((_tmp959.f1=& Cyc_print_full_evars_r,_tmp959)))),_tmp958)))),((_tmp95C->f5=(
(_tmp957="Print full information for evars (type debugging)",_tag_dyneither(_tmp957,sizeof(char),50))),_tmp95C)))))))))))),((_tmp8D8[56]=(
# 1309
(_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962->f1=((_tmp961="-printallkinds",_tag_dyneither(_tmp961,sizeof(char),15))),((_tmp962->f2=0,((_tmp962->f3=((_tmp960="",_tag_dyneither(_tmp960,sizeof(char),1))),((_tmp962->f4=(void*)(
(_tmp95E=_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E[0]=((_tmp95F.tag=3,((_tmp95F.f1=& Cyc_print_all_kinds_r,_tmp95F)))),_tmp95E)))),((_tmp962->f5=(
(_tmp95D="Always print kinds of type variables",_tag_dyneither(_tmp95D,sizeof(char),37))),_tmp962)))))))))))),((_tmp8D8[55]=(
# 1306
(_tmp968=_region_malloc(r,sizeof(*_tmp968)),((_tmp968->f1=((_tmp967="-printalltvars",_tag_dyneither(_tmp967,sizeof(char),15))),((_tmp968->f2=0,((_tmp968->f3=((_tmp966="",_tag_dyneither(_tmp966,sizeof(char),1))),((_tmp968->f4=(void*)(
(_tmp964=_region_malloc(r,sizeof(*_tmp964)),((_tmp964[0]=((_tmp965.tag=3,((_tmp965.f1=& Cyc_print_all_tvars_r,_tmp965)))),_tmp964)))),((_tmp968->f5=(
(_tmp963="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp963,sizeof(char),57))),_tmp968)))))))))))),((_tmp8D8[54]=(
# 1303
(_tmp96E=_region_malloc(r,sizeof(*_tmp96E)),((_tmp96E->f1=((_tmp96D="-noexpandtypedefs",_tag_dyneither(_tmp96D,sizeof(char),18))),((_tmp96E->f2=0,((_tmp96E->f3=((_tmp96C="",_tag_dyneither(_tmp96C,sizeof(char),1))),((_tmp96E->f4=(void*)(
(_tmp96A=_region_malloc(r,sizeof(*_tmp96A)),((_tmp96A[0]=((_tmp96B.tag=3,((_tmp96B.f1=& Cyc_noexpand_r,_tmp96B)))),_tmp96A)))),((_tmp96E->f5=(
(_tmp969="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp969,sizeof(char),41))),_tmp96E)))))))))))),((_tmp8D8[53]=(
# 1300
(_tmp974=_region_malloc(r,sizeof(*_tmp974)),((_tmp974->f1=((_tmp973="-noremoveunused",_tag_dyneither(_tmp973,sizeof(char),16))),((_tmp974->f2=0,((_tmp974->f3=((_tmp972="",_tag_dyneither(_tmp972,sizeof(char),1))),((_tmp974->f4=(void*)(
(_tmp970=_region_malloc(r,sizeof(*_tmp970)),((_tmp970[0]=((_tmp971.tag=3,((_tmp971.f1=& Cyc_noshake_r,_tmp971)))),_tmp970)))),((_tmp974->f5=(
(_tmp96F="Don't remove externed variables that aren't used",_tag_dyneither(_tmp96F,sizeof(char),49))),_tmp974)))))))))))),((_tmp8D8[52]=(
# 1297
(_tmp97A=_region_malloc(r,sizeof(*_tmp97A)),((_tmp97A->f1=((_tmp979="-nogc",_tag_dyneither(_tmp979,sizeof(char),6))),((_tmp97A->f2=0,((_tmp97A->f3=((_tmp978="",_tag_dyneither(_tmp978,sizeof(char),1))),((_tmp97A->f4=(void*)(
(_tmp976=_region_malloc(r,sizeof(*_tmp976)),((_tmp976[0]=((_tmp977.tag=3,((_tmp977.f1=& Cyc_nogc_r,_tmp977)))),_tmp976)))),((_tmp97A->f5=(
(_tmp975="Don't link in the garbage collector",_tag_dyneither(_tmp975,sizeof(char),36))),_tmp97A)))))))))))),((_tmp8D8[51]=(
# 1294
(_tmp980=_region_malloc(r,sizeof(*_tmp980)),((_tmp980->f1=((_tmp97F="-nocyc",_tag_dyneither(_tmp97F,sizeof(char),7))),((_tmp980->f2=0,((_tmp980->f3=((_tmp97E="",_tag_dyneither(_tmp97E,sizeof(char),1))),((_tmp980->f4=(void*)(
(_tmp97C=_region_malloc(r,sizeof(*_tmp97C)),((_tmp97C[0]=((_tmp97D.tag=0,((_tmp97D.f1=Cyc_set_nocyc,_tmp97D)))),_tmp97C)))),((_tmp980->f5=(
(_tmp97B="Don't add implicit namespace Cyc",_tag_dyneither(_tmp97B,sizeof(char),33))),_tmp980)))))))))))),((_tmp8D8[50]=(
# 1291
(_tmp986=_region_malloc(r,sizeof(*_tmp986)),((_tmp986->f1=((_tmp985="-no-cpp-precomp",_tag_dyneither(_tmp985,sizeof(char),16))),((_tmp986->f2=0,((_tmp986->f3=((_tmp984="",_tag_dyneither(_tmp984,sizeof(char),1))),((_tmp986->f4=(void*)(
(_tmp982=_region_malloc(r,sizeof(*_tmp982)),((_tmp982[0]=((_tmp983.tag=0,((_tmp983.f1=Cyc_set_nocppprecomp,_tmp983)))),_tmp982)))),((_tmp986->f5=(
(_tmp981="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp981,sizeof(char),40))),_tmp986)))))))))))),((_tmp8D8[49]=(
# 1288
(_tmp98C=_region_malloc(r,sizeof(*_tmp98C)),((_tmp98C->f1=((_tmp98B="-use-cpp",_tag_dyneither(_tmp98B,sizeof(char),9))),((_tmp98C->f2=0,((_tmp98C->f3=((_tmp98A="<path>",_tag_dyneither(_tmp98A,sizeof(char),7))),((_tmp98C->f4=(void*)(
(_tmp988=_region_malloc(r,sizeof(*_tmp988)),((_tmp988[0]=((_tmp989.tag=5,((_tmp989.f1=Cyc_set_cpp,_tmp989)))),_tmp988)))),((_tmp98C->f5=(
(_tmp987="Indicate which preprocessor to use",_tag_dyneither(_tmp987,sizeof(char),35))),_tmp98C)))))))))))),((_tmp8D8[48]=(
# 1285
(_tmp992=_region_malloc(r,sizeof(*_tmp992)),((_tmp992->f1=((_tmp991="--inline-checks",_tag_dyneither(_tmp991,sizeof(char),16))),((_tmp992->f2=0,((_tmp992->f3=((_tmp990="",_tag_dyneither(_tmp990,sizeof(char),1))),((_tmp992->f4=(void*)(
(_tmp98E=_region_malloc(r,sizeof(*_tmp98E)),((_tmp98E[0]=((_tmp98F.tag=0,((_tmp98F.f1=Cyc_set_inline_functions,_tmp98F)))),_tmp98E)))),((_tmp992->f5=(
(_tmp98D="Inline bounds checks instead of #define",_tag_dyneither(_tmp98D,sizeof(char),40))),_tmp992)))))))))))),((_tmp8D8[47]=(
# 1282
(_tmp998=_region_malloc(r,sizeof(*_tmp998)),((_tmp998->f1=((_tmp997="--noboundschecks",_tag_dyneither(_tmp997,sizeof(char),17))),((_tmp998->f2=0,((_tmp998->f3=((_tmp996="",_tag_dyneither(_tmp996,sizeof(char),1))),((_tmp998->f4=(void*)(
(_tmp994=_region_malloc(r,sizeof(*_tmp994)),((_tmp994[0]=((_tmp995.tag=0,((_tmp995.f1=Cyc_set_noboundschecks,_tmp995)))),_tmp994)))),((_tmp998->f5=(
(_tmp993="Disable bounds checks",_tag_dyneither(_tmp993,sizeof(char),22))),_tmp998)))))))))))),((_tmp8D8[46]=(
# 1279
(_tmp99E=_region_malloc(r,sizeof(*_tmp99E)),((_tmp99E->f1=((_tmp99D="--nonullchecks",_tag_dyneither(_tmp99D,sizeof(char),15))),((_tmp99E->f2=0,((_tmp99E->f3=((_tmp99C="",_tag_dyneither(_tmp99C,sizeof(char),1))),((_tmp99E->f4=(void*)(
(_tmp99A=_region_malloc(r,sizeof(*_tmp99A)),((_tmp99A[0]=((_tmp99B.tag=0,((_tmp99B.f1=Cyc_set_nonullchecks,_tmp99B)))),_tmp99A)))),((_tmp99E->f5=(
(_tmp999="Disable null checks",_tag_dyneither(_tmp999,sizeof(char),20))),_tmp99E)))))))))))),((_tmp8D8[45]=(
# 1276
(_tmp9A4=_region_malloc(r,sizeof(*_tmp9A4)),((_tmp9A4->f1=((_tmp9A3="--nochecks",_tag_dyneither(_tmp9A3,sizeof(char),11))),((_tmp9A4->f2=0,((_tmp9A4->f3=((_tmp9A2="",_tag_dyneither(_tmp9A2,sizeof(char),1))),((_tmp9A4->f4=(void*)(
(_tmp9A0=_region_malloc(r,sizeof(*_tmp9A0)),((_tmp9A0[0]=((_tmp9A1.tag=0,((_tmp9A1.f1=Cyc_set_nochecks,_tmp9A1)))),_tmp9A0)))),((_tmp9A4->f5=(
(_tmp99F="Disable bounds/null checks",_tag_dyneither(_tmp99F,sizeof(char),27))),_tmp9A4)))))))))))),((_tmp8D8[44]=(
# 1273
(_tmp9AA=_region_malloc(r,sizeof(*_tmp9AA)),((_tmp9AA->f1=((_tmp9A9="--nolineno",_tag_dyneither(_tmp9A9,sizeof(char),11))),((_tmp9AA->f2=0,((_tmp9AA->f3=((_tmp9A8="",_tag_dyneither(_tmp9A8,sizeof(char),1))),((_tmp9AA->f4=(void*)(
(_tmp9A6=_region_malloc(r,sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A7.tag=0,((_tmp9A7.f1=Cyc_clear_lineno,_tmp9A7)))),_tmp9A6)))),((_tmp9AA->f5=(
(_tmp9A5="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp9A5,sizeof(char),63))),_tmp9AA)))))))))))),((_tmp8D8[43]=(
# 1270
(_tmp9B0=_region_malloc(r,sizeof(*_tmp9B0)),((_tmp9B0->f1=((_tmp9AF="-save-c",_tag_dyneither(_tmp9AF,sizeof(char),8))),((_tmp9B0->f2=0,((_tmp9B0->f3=((_tmp9AE="",_tag_dyneither(_tmp9AE,sizeof(char),1))),((_tmp9B0->f4=(void*)(
(_tmp9AC=_region_malloc(r,sizeof(*_tmp9AC)),((_tmp9AC[0]=((_tmp9AD.tag=3,((_tmp9AD.f1=& Cyc_save_c_r,_tmp9AD)))),_tmp9AC)))),((_tmp9B0->f5=(
(_tmp9AB="Don't delete temporary C files",_tag_dyneither(_tmp9AB,sizeof(char),31))),_tmp9B0)))))))))))),((_tmp8D8[42]=(
# 1267
(_tmp9B6=_region_malloc(r,sizeof(*_tmp9B6)),((_tmp9B6->f1=((_tmp9B5="-save-temps",_tag_dyneither(_tmp9B5,sizeof(char),12))),((_tmp9B6->f2=0,((_tmp9B6->f3=((_tmp9B4="",_tag_dyneither(_tmp9B4,sizeof(char),1))),((_tmp9B6->f4=(void*)(
(_tmp9B2=_region_malloc(r,sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B3.tag=0,((_tmp9B3.f1=Cyc_set_save_temps,_tmp9B3)))),_tmp9B2)))),((_tmp9B6->f5=(
(_tmp9B1="Don't delete temporary files",_tag_dyneither(_tmp9B1,sizeof(char),29))),_tmp9B6)))))))))))),((_tmp8D8[41]=(
# 1264
(_tmp9BC=_region_malloc(r,sizeof(*_tmp9BC)),((_tmp9BC->f1=((_tmp9BB="-c-comp",_tag_dyneither(_tmp9BB,sizeof(char),8))),((_tmp9BC->f2=0,((_tmp9BC->f3=((_tmp9BA=" <compiler>",_tag_dyneither(_tmp9BA,sizeof(char),12))),((_tmp9BC->f4=(void*)(
(_tmp9B8=_region_malloc(r,sizeof(*_tmp9B8)),((_tmp9B8[0]=((_tmp9B9.tag=5,((_tmp9B9.f1=Cyc_set_c_compiler,_tmp9B9)))),_tmp9B8)))),((_tmp9BC->f5=(
(_tmp9B7="Produce C output for the given compiler",_tag_dyneither(_tmp9B7,sizeof(char),40))),_tmp9BC)))))))))))),((_tmp8D8[40]=(
# 1261
(_tmp9C2=_region_malloc(r,sizeof(*_tmp9C2)),((_tmp9C2->f1=((_tmp9C1="-un-gcc",_tag_dyneither(_tmp9C1,sizeof(char),8))),((_tmp9C2->f2=0,((_tmp9C2->f3=((_tmp9C0="",_tag_dyneither(_tmp9C0,sizeof(char),1))),((_tmp9C2->f4=(void*)(
(_tmp9BE=_region_malloc(r,sizeof(*_tmp9BE)),((_tmp9BE[0]=((_tmp9BF.tag=0,((_tmp9BF.f1=Cyc_set_tovc,_tmp9BF)))),_tmp9BE)))),((_tmp9C2->f5=(
(_tmp9BD="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp9BD,sizeof(char),57))),_tmp9C2)))))))))))),((_tmp8D8[39]=(
# 1258
(_tmp9C8=_region_malloc(r,sizeof(*_tmp9C8)),((_tmp9C8->f1=((_tmp9C7="-no-elim-statement-expressions",_tag_dyneither(_tmp9C7,sizeof(char),31))),((_tmp9C8->f2=0,((_tmp9C8->f3=((_tmp9C6="",_tag_dyneither(_tmp9C6,sizeof(char),1))),((_tmp9C8->f4=(void*)(
(_tmp9C4=_region_malloc(r,sizeof(*_tmp9C4)),((_tmp9C4[0]=((_tmp9C5.tag=4,((_tmp9C5.f1=& Cyc_elim_se_r,_tmp9C5)))),_tmp9C4)))),((_tmp9C8->f5=(
(_tmp9C3="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9C3,sizeof(char),47))),_tmp9C8)))))))))))),((_tmp8D8[38]=(
# 1254
(_tmp9CE=_region_malloc(r,sizeof(*_tmp9CE)),((_tmp9CE->f1=((_tmp9CD="-elim-statement-expressions",_tag_dyneither(_tmp9CD,sizeof(char),28))),((_tmp9CE->f2=0,((_tmp9CE->f3=((_tmp9CC="",_tag_dyneither(_tmp9CC,sizeof(char),1))),((_tmp9CE->f4=(void*)(
(_tmp9CA=_region_malloc(r,sizeof(*_tmp9CA)),((_tmp9CA[0]=((_tmp9CB.tag=3,((_tmp9CB.f1=& Cyc_elim_se_r,_tmp9CB)))),_tmp9CA)))),((_tmp9CE->f5=(
# 1257
(_tmp9C9="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9C9,sizeof(char),66))),_tmp9CE)))))))))))),((_tmp8D8[37]=(
# 1251
(_tmp9D4=_region_malloc(r,sizeof(*_tmp9D4)),((_tmp9D4->f1=((_tmp9D3="-up",_tag_dyneither(_tmp9D3,sizeof(char),4))),((_tmp9D4->f2=0,((_tmp9D4->f3=((_tmp9D2="",_tag_dyneither(_tmp9D2,sizeof(char),1))),((_tmp9D4->f4=(void*)(
(_tmp9D0=_region_malloc(r,sizeof(*_tmp9D0)),((_tmp9D0[0]=((_tmp9D1.tag=4,((_tmp9D1.f1=& Cyc_pp_r,_tmp9D1)))),_tmp9D0)))),((_tmp9D4->f5=(
(_tmp9CF="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9CF,sizeof(char),55))),_tmp9D4)))))))))))),((_tmp8D8[36]=(
# 1248
(_tmp9DA=_region_malloc(r,sizeof(*_tmp9DA)),((_tmp9DA->f1=((_tmp9D9="-pp",_tag_dyneither(_tmp9D9,sizeof(char),4))),((_tmp9DA->f2=0,((_tmp9DA->f3=((_tmp9D8="",_tag_dyneither(_tmp9D8,sizeof(char),1))),((_tmp9DA->f4=(void*)(
(_tmp9D6=_region_malloc(r,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=3,((_tmp9D7.f1=& Cyc_pp_r,_tmp9D7)))),_tmp9D6)))),((_tmp9DA->f5=(
(_tmp9D5="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9D5,sizeof(char),47))),_tmp9DA)))))))))))),((_tmp8D8[35]=(
# 1245
(_tmp9E0=_region_malloc(r,sizeof(*_tmp9E0)),((_tmp9E0->f1=((_tmp9DF="-ic",_tag_dyneither(_tmp9DF,sizeof(char),4))),((_tmp9E0->f2=0,((_tmp9E0->f3=((_tmp9DE="",_tag_dyneither(_tmp9DE,sizeof(char),1))),((_tmp9E0->f4=(void*)(
(_tmp9DC=_region_malloc(r,sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=3,((_tmp9DD.f1=& Cyc_ic_r,_tmp9DD)))),_tmp9DC)))),((_tmp9E0->f5=(
(_tmp9DB="Activate the link-checker",_tag_dyneither(_tmp9DB,sizeof(char),26))),_tmp9E0)))))))))))),((_tmp8D8[34]=(
# 1242
(_tmp9E6=_region_malloc(r,sizeof(*_tmp9E6)),((_tmp9E6->f1=((_tmp9E5="-stopafter-toc",_tag_dyneither(_tmp9E5,sizeof(char),15))),((_tmp9E6->f2=0,((_tmp9E6->f3=((_tmp9E4="",_tag_dyneither(_tmp9E4,sizeof(char),1))),((_tmp9E6->f4=(void*)(
(_tmp9E2=_region_malloc(r,sizeof(*_tmp9E2)),((_tmp9E2[0]=((_tmp9E3.tag=3,((_tmp9E3.f1=& Cyc_toc_r,_tmp9E3)))),_tmp9E2)))),((_tmp9E6->f5=(
(_tmp9E1="Stop after translation to C",_tag_dyneither(_tmp9E1,sizeof(char),28))),_tmp9E6)))))))))))),((_tmp8D8[33]=(
# 1239
(_tmp9EC=_region_malloc(r,sizeof(*_tmp9EC)),((_tmp9EC->f1=((_tmp9EB="-stopafter-cf",_tag_dyneither(_tmp9EB,sizeof(char),14))),((_tmp9EC->f2=0,((_tmp9EC->f3=((_tmp9EA="",_tag_dyneither(_tmp9EA,sizeof(char),1))),((_tmp9EC->f4=(void*)(
(_tmp9E8=_region_malloc(r,sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=3,((_tmp9E9.f1=& Cyc_cf_r,_tmp9E9)))),_tmp9E8)))),((_tmp9EC->f5=(
(_tmp9E7="Stop after control-flow checking",_tag_dyneither(_tmp9E7,sizeof(char),33))),_tmp9EC)))))))))))),((_tmp8D8[32]=(
# 1236
(_tmp9F2=_region_malloc(r,sizeof(*_tmp9F2)),((_tmp9F2->f1=((_tmp9F1="-noprint",_tag_dyneither(_tmp9F1,sizeof(char),9))),((_tmp9F2->f2=0,((_tmp9F2->f3=((_tmp9F0="",_tag_dyneither(_tmp9F0,sizeof(char),1))),((_tmp9F2->f4=(void*)(
(_tmp9EE=_region_malloc(r,sizeof(*_tmp9EE)),((_tmp9EE[0]=((_tmp9EF.tag=3,((_tmp9EF.f1=& Cyc_noprint_r,_tmp9EF)))),_tmp9EE)))),((_tmp9F2->f5=(
(_tmp9ED="Do not print output (when stopping early)",_tag_dyneither(_tmp9ED,sizeof(char),42))),_tmp9F2)))))))))))),((_tmp8D8[31]=(
# 1233
(_tmp9F8=_region_malloc(r,sizeof(*_tmp9F8)),((_tmp9F8->f1=((_tmp9F7="-stopafter-tc",_tag_dyneither(_tmp9F7,sizeof(char),14))),((_tmp9F8->f2=0,((_tmp9F8->f3=((_tmp9F6="",_tag_dyneither(_tmp9F6,sizeof(char),1))),((_tmp9F8->f4=(void*)(
(_tmp9F4=_region_malloc(r,sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F5.tag=3,((_tmp9F5.f1=& Cyc_tc_r,_tmp9F5)))),_tmp9F4)))),((_tmp9F8->f5=(
(_tmp9F3="Stop after type checking",_tag_dyneither(_tmp9F3,sizeof(char),25))),_tmp9F8)))))))))))),((_tmp8D8[30]=(
# 1230
(_tmp9FE=_region_malloc(r,sizeof(*_tmp9FE)),((_tmp9FE->f1=((_tmp9FD="-stopafter-parse",_tag_dyneither(_tmp9FD,sizeof(char),17))),((_tmp9FE->f2=0,((_tmp9FE->f3=((_tmp9FC="",_tag_dyneither(_tmp9FC,sizeof(char),1))),((_tmp9FE->f4=(void*)(
(_tmp9FA=_region_malloc(r,sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FB.tag=3,((_tmp9FB.f1=& Cyc_parseonly_r,_tmp9FB)))),_tmp9FA)))),((_tmp9FE->f5=(
(_tmp9F9="Stop after parsing",_tag_dyneither(_tmp9F9,sizeof(char),19))),_tmp9FE)))))))))))),((_tmp8D8[29]=(
# 1227
(_tmpA04=_region_malloc(r,sizeof(*_tmpA04)),((_tmpA04->f1=((_tmpA03="-E",_tag_dyneither(_tmpA03,sizeof(char),3))),((_tmpA04->f2=0,((_tmpA04->f3=((_tmpA02="",_tag_dyneither(_tmpA02,sizeof(char),1))),((_tmpA04->f4=(void*)(
(_tmpA00=_region_malloc(r,sizeof(*_tmpA00)),((_tmpA00[0]=((_tmpA01.tag=3,((_tmpA01.f1=& Cyc_stop_after_cpp_r,_tmpA01)))),_tmpA00)))),((_tmpA04->f5=(
(_tmp9FF="Stop after preprocessing",_tag_dyneither(_tmp9FF,sizeof(char),25))),_tmpA04)))))))))))),((_tmp8D8[28]=(
# 1224
(_tmpA0A=_region_malloc(r,sizeof(*_tmpA0A)),((_tmpA0A->f1=((_tmpA09="-Wall",_tag_dyneither(_tmpA09,sizeof(char),6))),((_tmpA0A->f2=0,((_tmpA0A->f3=((_tmpA08="",_tag_dyneither(_tmpA08,sizeof(char),1))),((_tmpA0A->f4=(void*)(
(_tmpA06=_region_malloc(r,sizeof(*_tmpA06)),((_tmpA06[0]=((_tmpA07.tag=0,((_tmpA07.f1=Cyc_set_all_warnings,_tmpA07)))),_tmpA06)))),((_tmpA0A->f5=(
(_tmpA05="Turn on all warnings",_tag_dyneither(_tmpA05,sizeof(char),21))),_tmpA0A)))))))))))),((_tmp8D8[27]=(
# 1221
(_tmpA10=_region_malloc(r,sizeof(*_tmpA10)),((_tmpA10->f1=((_tmpA0F="-Woverride",_tag_dyneither(_tmpA0F,sizeof(char),11))),((_tmpA10->f2=0,((_tmpA10->f3=((_tmpA0E="",_tag_dyneither(_tmpA0E,sizeof(char),1))),((_tmpA10->f4=(void*)(
(_tmpA0C=_region_malloc(r,sizeof(*_tmpA0C)),((_tmpA0C[0]=((_tmpA0D.tag=3,((_tmpA0D.f1=& Cyc_Tcenv_warn_override,_tmpA0D)))),_tmpA0C)))),((_tmpA10->f5=(
(_tmpA0B="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmpA0B,sizeof(char),58))),_tmpA10)))))))))))),((_tmp8D8[26]=(
# 1218
(_tmpA16=_region_malloc(r,sizeof(*_tmpA16)),((_tmpA16->f1=((_tmpA15="-Wlose-unique",_tag_dyneither(_tmpA15,sizeof(char),14))),((_tmpA16->f2=0,((_tmpA16->f3=((_tmpA14="",_tag_dyneither(_tmpA14,sizeof(char),1))),((_tmpA16->f4=(void*)(
(_tmpA12=_region_malloc(r,sizeof(*_tmpA12)),((_tmpA12[0]=((_tmpA13.tag=3,((_tmpA13.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmpA13)))),_tmpA12)))),((_tmpA16->f5=(
(_tmpA11="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmpA11,sizeof(char),49))),_tmpA16)))))))))))),((_tmp8D8[25]=(
# 1214
(_tmpA1C=_region_malloc(r,sizeof(*_tmpA1C)),((_tmpA1C->f1=((_tmpA1B="-b",_tag_dyneither(_tmpA1B,sizeof(char),3))),((_tmpA1C->f2=0,((_tmpA1C->f3=((_tmpA1A="<machine>",_tag_dyneither(_tmpA1A,sizeof(char),10))),((_tmpA1C->f4=(void*)(
(_tmpA18=_region_malloc(r,sizeof(*_tmpA18)),((_tmpA18[0]=((_tmpA19.tag=5,((_tmpA19.f1=Cyc_set_target_arch,_tmpA19)))),_tmpA18)))),((_tmpA1C->f5=(
# 1217
(_tmpA17="Set target machine",_tag_dyneither(_tmpA17,sizeof(char),19))),_tmpA1C)))))))))))),((_tmp8D8[24]=(
# 1211
(_tmpA22=_region_malloc(r,sizeof(*_tmpA22)),((_tmpA22->f1=((_tmpA21="-MT",_tag_dyneither(_tmpA21,sizeof(char),4))),((_tmpA22->f2=0,((_tmpA22->f3=((_tmpA20=" <target>",_tag_dyneither(_tmpA20,sizeof(char),10))),((_tmpA22->f4=(void*)(
(_tmpA1E=_region_malloc(r,sizeof(*_tmpA1E)),((_tmpA1E[0]=((_tmpA1F.tag=5,((_tmpA1F.f1=Cyc_set_dependencies_target,_tmpA1F)))),_tmpA1E)))),((_tmpA22->f5=(
(_tmpA1D="Give target for dependencies",_tag_dyneither(_tmpA1D,sizeof(char),29))),_tmpA22)))))))))))),((_tmp8D8[23]=(
# 1207
(_tmpA28=_region_malloc(r,sizeof(*_tmpA28)),((_tmpA28->f1=((_tmpA27="-MG",_tag_dyneither(_tmpA27,sizeof(char),4))),((_tmpA28->f2=0,((_tmpA28->f3=((_tmpA26="",_tag_dyneither(_tmpA26,sizeof(char),1))),((_tmpA28->f4=(void*)(
(_tmpA24=_region_malloc(r,sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA25.tag=1,((_tmpA25.f1=Cyc_add_cpparg,_tmpA25)))),_tmpA24)))),((_tmpA28->f5=(
# 1210
(_tmpA23="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmpA23,sizeof(char),68))),_tmpA28)))))))))))),((_tmp8D8[22]=(
# 1204
(_tmpA2E=_region_malloc(r,sizeof(*_tmpA2E)),((_tmpA2E->f1=((_tmpA2D="-M",_tag_dyneither(_tmpA2D,sizeof(char),3))),((_tmpA2E->f2=0,((_tmpA2E->f3=((_tmpA2C="",_tag_dyneither(_tmpA2C,sizeof(char),1))),((_tmpA2E->f4=(void*)(
(_tmpA2A=_region_malloc(r,sizeof(*_tmpA2A)),((_tmpA2A[0]=((_tmpA2B.tag=0,((_tmpA2B.f1=Cyc_set_produce_dependencies,_tmpA2B)))),_tmpA2A)))),((_tmpA2E->f5=(
(_tmpA29="Produce dependencies",_tag_dyneither(_tmpA29,sizeof(char),21))),_tmpA2E)))))))))))),((_tmp8D8[21]=(
# 1201
(_tmpA34=_region_malloc(r,sizeof(*_tmpA34)),((_tmpA34->f1=((_tmpA33="-S",_tag_dyneither(_tmpA33,sizeof(char),3))),((_tmpA34->f2=0,((_tmpA34->f3=((_tmpA32="",_tag_dyneither(_tmpA32,sizeof(char),1))),((_tmpA34->f4=(void*)(
(_tmpA30=_region_malloc(r,sizeof(*_tmpA30)),((_tmpA30[0]=((_tmpA31.tag=0,((_tmpA31.f1=Cyc_set_stop_after_asmfile,_tmpA31)))),_tmpA30)))),((_tmpA34->f5=(
(_tmpA2F="Stop after producing assembly code",_tag_dyneither(_tmpA2F,sizeof(char),35))),_tmpA34)))))))))))),((_tmp8D8[20]=(
# 1198
(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A->f1=((_tmpA39="-pa",_tag_dyneither(_tmpA39,sizeof(char),4))),((_tmpA3A->f2=0,((_tmpA3A->f3=((_tmpA38="",_tag_dyneither(_tmpA38,sizeof(char),1))),((_tmpA3A->f4=(void*)(
(_tmpA36=_region_malloc(r,sizeof(*_tmpA36)),((_tmpA36[0]=((_tmpA37.tag=0,((_tmpA37.f1=Cyc_set_pa,_tmpA37)))),_tmpA36)))),((_tmpA3A->f5=(
(_tmpA35="Compile for profiling with aprof",_tag_dyneither(_tmpA35,sizeof(char),33))),_tmpA3A)))))))))))),((_tmp8D8[19]=(
# 1195
(_tmpA40=_region_malloc(r,sizeof(*_tmpA40)),((_tmpA40->f1=((_tmpA3F="-pg",_tag_dyneither(_tmpA3F,sizeof(char),4))),((_tmpA40->f2=0,((_tmpA40->f3=((_tmpA3E="",_tag_dyneither(_tmpA3E,sizeof(char),1))),((_tmpA40->f4=(void*)(
(_tmpA3C=_region_malloc(r,sizeof(*_tmpA3C)),((_tmpA3C[0]=((_tmpA3D.tag=0,((_tmpA3D.f1=Cyc_set_pg,_tmpA3D)))),_tmpA3C)))),((_tmpA40->f5=(
(_tmpA3B="Compile for profiling with gprof",_tag_dyneither(_tmpA3B,sizeof(char),33))),_tmpA40)))))))))))),((_tmp8D8[18]=(
# 1192
(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->f1=((_tmpA45="-p",_tag_dyneither(_tmpA45,sizeof(char),3))),((_tmpA46->f2=0,((_tmpA46->f3=((_tmpA44="",_tag_dyneither(_tmpA44,sizeof(char),1))),((_tmpA46->f4=(void*)(
(_tmpA42=_region_malloc(r,sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=1,((_tmpA43.f1=Cyc_add_ccarg,_tmpA43)))),_tmpA42)))),((_tmpA46->f5=(
(_tmpA41="Compile for profiling with prof",_tag_dyneither(_tmpA41,sizeof(char),32))),_tmpA46)))))))))))),((_tmp8D8[17]=(
# 1189
(_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C->f1=((_tmpA4B="-g",_tag_dyneither(_tmpA4B,sizeof(char),3))),((_tmpA4C->f2=0,((_tmpA4C->f3=((_tmpA4A="",_tag_dyneither(_tmpA4A,sizeof(char),1))),((_tmpA4C->f4=(void*)(
(_tmpA48=_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=1,((_tmpA49.f1=Cyc_add_ccarg,_tmpA49)))),_tmpA48)))),((_tmpA4C->f5=(
(_tmpA47="Compile for debugging",_tag_dyneither(_tmpA47,sizeof(char),22))),_tmpA4C)))))))))))),((_tmp8D8[16]=(
# 1186
(_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52->f1=((_tmpA51="-fomit-frame-pointer",_tag_dyneither(_tmpA51,sizeof(char),21))),((_tmpA52->f2=0,((_tmpA52->f3=((_tmpA50="",_tag_dyneither(_tmpA50,sizeof(char),1))),((_tmpA52->f4=(void*)(
(_tmpA4E=_region_malloc(r,sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=1,((_tmpA4F.f1=Cyc_add_ccarg,_tmpA4F)))),_tmpA4E)))),((_tmpA52->f5=(
(_tmpA4D="Omit frame pointer",_tag_dyneither(_tmpA4D,sizeof(char),19))),_tmpA52)))))))))))),((_tmp8D8[15]=(
# 1183
(_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58->f1=((_tmpA57="-O3",_tag_dyneither(_tmpA57,sizeof(char),4))),((_tmpA58->f2=0,((_tmpA58->f3=((_tmpA56="",_tag_dyneither(_tmpA56,sizeof(char),1))),((_tmpA58->f4=(void*)(
(_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA55.tag=1,((_tmpA55.f1=Cyc_add_ccarg,_tmpA55)))),_tmpA54)))),((_tmpA58->f5=(
(_tmpA53="Even more optimization",_tag_dyneither(_tmpA53,sizeof(char),23))),_tmpA58)))))))))))),((_tmp8D8[14]=(
# 1180
(_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E->f1=((_tmpA5D="-O2",_tag_dyneither(_tmpA5D,sizeof(char),4))),((_tmpA5E->f2=0,((_tmpA5E->f3=((_tmpA5C="",_tag_dyneither(_tmpA5C,sizeof(char),1))),((_tmpA5E->f4=(void*)(
(_tmpA5A=_region_malloc(r,sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA5B.tag=1,((_tmpA5B.f1=Cyc_add_ccarg,_tmpA5B)))),_tmpA5A)))),((_tmpA5E->f5=(
(_tmpA59="A higher level of optimization",_tag_dyneither(_tmpA59,sizeof(char),31))),_tmpA5E)))))))))))),((_tmp8D8[13]=(
# 1177
(_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64->f1=((_tmpA63="-O",_tag_dyneither(_tmpA63,sizeof(char),3))),((_tmpA64->f2=0,((_tmpA64->f3=((_tmpA62="",_tag_dyneither(_tmpA62,sizeof(char),1))),((_tmpA64->f4=(void*)(
(_tmpA60=_region_malloc(r,sizeof(*_tmpA60)),((_tmpA60[0]=((_tmpA61.tag=1,((_tmpA61.f1=Cyc_add_ccarg,_tmpA61)))),_tmpA60)))),((_tmpA64->f5=(
(_tmpA5F="Optimize",_tag_dyneither(_tmpA5F,sizeof(char),9))),_tmpA64)))))))))))),((_tmp8D8[12]=(
# 1174
(_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A->f1=((_tmpA69="-O0",_tag_dyneither(_tmpA69,sizeof(char),4))),((_tmpA6A->f2=0,((_tmpA6A->f3=((_tmpA68="",_tag_dyneither(_tmpA68,sizeof(char),1))),((_tmpA6A->f4=(void*)(
(_tmpA66=_region_malloc(r,sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA67.tag=1,((_tmpA67.f1=Cyc_add_ccarg,_tmpA67)))),_tmpA66)))),((_tmpA6A->f5=(
(_tmpA65="Don't optimize",_tag_dyneither(_tmpA65,sizeof(char),15))),_tmpA6A)))))))))))),((_tmp8D8[11]=(
# 1171
(_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70->f1=((_tmpA6F="-s",_tag_dyneither(_tmpA6F,sizeof(char),3))),((_tmpA70->f2=0,((_tmpA70->f3=((_tmpA6E="",_tag_dyneither(_tmpA6E,sizeof(char),1))),((_tmpA70->f4=(void*)(
(_tmpA6C=_region_malloc(r,sizeof(*_tmpA6C)),((_tmpA6C[0]=((_tmpA6D.tag=1,((_tmpA6D.f1=Cyc_add_ccarg,_tmpA6D)))),_tmpA6C)))),((_tmpA70->f5=(
(_tmpA6B="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA6B,sizeof(char),60))),_tmpA70)))))))))))),((_tmp8D8[10]=(
# 1168
(_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76->f1=((_tmpA75="-x",_tag_dyneither(_tmpA75,sizeof(char),3))),((_tmpA76->f2=0,((_tmpA76->f3=((_tmpA74=" <language>",_tag_dyneither(_tmpA74,sizeof(char),12))),((_tmpA76->f4=(void*)(
(_tmpA72=_region_malloc(r,sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA73.tag=5,((_tmpA73.f1=Cyc_set_inputtype,_tmpA73)))),_tmpA72)))),((_tmpA76->f5=(
(_tmpA71="Specify <language> for the following input files",_tag_dyneither(_tmpA71,sizeof(char),49))),_tmpA76)))))))))))),((_tmp8D8[9]=(
# 1165
(_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C->f1=((_tmpA7B="-c",_tag_dyneither(_tmpA7B,sizeof(char),3))),((_tmpA7C->f2=0,((_tmpA7C->f3=((_tmpA7A="",_tag_dyneither(_tmpA7A,sizeof(char),1))),((_tmpA7C->f4=(void*)(
(_tmpA78=_region_malloc(r,sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA79.tag=0,((_tmpA79.f1=Cyc_set_stop_after_objectfile,_tmpA79)))),_tmpA78)))),((_tmpA7C->f5=(
(_tmpA77="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA77,sizeof(char),61))),_tmpA7C)))))))))))),((_tmp8D8[8]=(
# 1162
(_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82->f1=((_tmpA81="-m",_tag_dyneither(_tmpA81,sizeof(char),3))),((_tmpA82->f2=1,((_tmpA82->f3=((_tmpA80="<option>",_tag_dyneither(_tmpA80,sizeof(char),9))),((_tmpA82->f4=(void*)(
(_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7F.tag=1,((_tmpA7F.f1=Cyc_add_marg,_tmpA7F)))),_tmpA7E)))),((_tmpA82->f5=(
(_tmpA7D="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA7D,sizeof(char),52))),_tmpA82)))))))))))),((_tmp8D8[7]=(
# 1159
(_tmpA88=_region_malloc(r,sizeof(*_tmpA88)),((_tmpA88->f1=((_tmpA87="-l",_tag_dyneither(_tmpA87,sizeof(char),3))),((_tmpA88->f2=1,((_tmpA88->f3=((_tmpA86="<libname>",_tag_dyneither(_tmpA86,sizeof(char),10))),((_tmpA88->f4=(void*)(
(_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=1,((_tmpA85.f1=Cyc_add_libarg,_tmpA85)))),_tmpA84)))),((_tmpA88->f5=(
(_tmpA83="Library file",_tag_dyneither(_tmpA83,sizeof(char),13))),_tmpA88)))))))))))),((_tmp8D8[6]=(
# 1156
(_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E->f1=((_tmpA8D="-L",_tag_dyneither(_tmpA8D,sizeof(char),3))),((_tmpA8E->f2=1,((_tmpA8E->f3=((_tmpA8C="<dir>",_tag_dyneither(_tmpA8C,sizeof(char),6))),((_tmpA8E->f4=(void*)(
(_tmpA8A=_region_malloc(r,sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=1,((_tmpA8B.f1=Cyc_add_ccarg,_tmpA8B)))),_tmpA8A)))),((_tmpA8E->f5=(
(_tmpA89="Add to the list of directories for -l",_tag_dyneither(_tmpA89,sizeof(char),38))),_tmpA8E)))))))))))),((_tmp8D8[5]=(
# 1153
(_tmpA94=_region_malloc(r,sizeof(*_tmpA94)),((_tmpA94->f1=((_tmpA93="-I",_tag_dyneither(_tmpA93,sizeof(char),3))),((_tmpA94->f2=1,((_tmpA94->f3=((_tmpA92="<dir>",_tag_dyneither(_tmpA92,sizeof(char),6))),((_tmpA94->f4=(void*)(
(_tmpA90=_region_malloc(r,sizeof(*_tmpA90)),((_tmpA90[0]=((_tmpA91.tag=1,((_tmpA91.f1=Cyc_add_cpparg,_tmpA91)))),_tmpA90)))),((_tmpA94->f5=(
(_tmpA8F="Add to the list of directories to search for include files",_tag_dyneither(_tmpA8F,sizeof(char),59))),_tmpA94)))))))))))),((_tmp8D8[4]=(
# 1150
(_tmpA9A=_region_malloc(r,sizeof(*_tmpA9A)),((_tmpA9A->f1=((_tmpA99="-B",_tag_dyneither(_tmpA99,sizeof(char),3))),((_tmpA9A->f2=1,((_tmpA9A->f3=((_tmpA98="<file>",_tag_dyneither(_tmpA98,sizeof(char),7))),((_tmpA9A->f4=(void*)(
(_tmpA96=_region_malloc(r,sizeof(*_tmpA96)),((_tmpA96[0]=((_tmpA97.tag=1,((_tmpA97.f1=Cyc_add_cyclone_exec_path,_tmpA97)))),_tmpA96)))),((_tmpA9A->f5=(
(_tmpA95="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA95,sizeof(char),60))),_tmpA9A)))))))))))),((_tmp8D8[3]=(
# 1147
(_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->f1=((_tmpA9F="-D",_tag_dyneither(_tmpA9F,sizeof(char),3))),((_tmpAA0->f2=1,((_tmpAA0->f3=((_tmpA9E="<name>[=<value>]",_tag_dyneither(_tmpA9E,sizeof(char),17))),((_tmpAA0->f4=(void*)(
(_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9D.tag=1,((_tmpA9D.f1=Cyc_add_cpparg,_tmpA9D)))),_tmpA9C)))),((_tmpAA0->f5=(
(_tmpA9B="Pass definition to preprocessor",_tag_dyneither(_tmpA9B,sizeof(char),32))),_tmpAA0)))))))))))),((_tmp8D8[2]=(
# 1144
(_tmpAA6=_region_malloc(r,sizeof(*_tmpAA6)),((_tmpAA6->f1=((_tmpAA5="-o",_tag_dyneither(_tmpAA5,sizeof(char),3))),((_tmpAA6->f2=0,((_tmpAA6->f3=((_tmpAA4=" <file>",_tag_dyneither(_tmpAA4,sizeof(char),8))),((_tmpAA6->f4=(void*)(
(_tmpAA2=_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA3.tag=5,((_tmpAA3.f1=Cyc_set_output_file,_tmpAA3)))),_tmpAA2)))),((_tmpAA6->f5=(
(_tmpAA1="Set the output file name to <file>",_tag_dyneither(_tmpAA1,sizeof(char),35))),_tmpAA6)))))))))))),((_tmp8D8[1]=(
# 1141
(_tmpAAC=_region_malloc(r,sizeof(*_tmpAAC)),((_tmpAAC->f1=((_tmpAAB="--version",_tag_dyneither(_tmpAAB,sizeof(char),10))),((_tmpAAC->f2=0,((_tmpAAC->f3=((_tmpAAA="",_tag_dyneither(_tmpAAA,sizeof(char),1))),((_tmpAAC->f4=(void*)(
(_tmpAA8=_region_malloc(r,sizeof(*_tmpAA8)),((_tmpAA8[0]=((_tmpAA9.tag=0,((_tmpAA9.f1=Cyc_print_version,_tmpAA9)))),_tmpAA8)))),((_tmpAAC->f5=(
(_tmpAA7="Print version information and exit",_tag_dyneither(_tmpAA7,sizeof(char),35))),_tmpAAC)))))))))))),((_tmp8D8[0]=(
# 1138
(_tmpAB2=_region_malloc(r,sizeof(*_tmpAB2)),((_tmpAB2->f1=((_tmpAB1="-v",_tag_dyneither(_tmpAB1,sizeof(char),3))),((_tmpAB2->f2=0,((_tmpAB2->f3=((_tmpAB0="",_tag_dyneither(_tmpAB0,sizeof(char),1))),((_tmpAB2->f4=(void*)(
(_tmpAAE=_region_malloc(r,sizeof(*_tmpAAE)),((_tmpAAE[0]=((_tmpAAF.tag=3,((_tmpAAF.f1=& Cyc_v_r,_tmpAAF)))),_tmpAAE)))),((_tmpAB2->f5=(
(_tmpAAD="Print compilation stages verbosely",_tag_dyneither(_tmpAAD,sizeof(char),35))),_tmpAB2)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8D8,sizeof(struct _tuple24*),79)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1380
const char*_tmpAB3;struct _dyneither_ptr optstring=(_tmpAB3="Options:",_tag_dyneither(_tmpAB3,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp166);
# 1385
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1393
char*_tmpAB4;struct _dyneither_ptr cyclone_exec_prefix=(_tmpAB4=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpAB4,sizeof(char),_get_zero_arr_size_char((void*)_tmpAB4,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpAB7;struct Cyc_List_List*_tmpAB6;Cyc_cyclone_exec_path=((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->hd=((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7[0]=cyclone_exec_prefix,_tmpAB7)))),((_tmpAB6->tl=Cyc_cyclone_exec_path,_tmpAB6))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpABD;const char*_tmpABC;struct Cyc_List_List*_tmpABB;Cyc_cyclone_exec_path=(
(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->hd=((_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpABC="cyc-lib",_tag_dyneither(_tmpABC,sizeof(char),8)))),_tmpABD)))),((_tmpABB->tl=Cyc_cyclone_exec_path,_tmpABB))))));}
# 1401
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1409
const char*_tmpABE;struct _dyneither_ptr _tmp16D=Cyc_do_find(cyclone_arch_path,((_tmpABE="cycspecs",_tag_dyneither(_tmpABE,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpAC2;void*_tmpAC1[1];struct Cyc_String_pa_PrintArg_struct _tmpAC0;(_tmpAC0.tag=0,((_tmpAC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpAC1[0]=& _tmpAC0,Cyc_fprintf(Cyc_stderr,((_tmpAC2="Reading from specs file %s\n",_tag_dyneither(_tmpAC2,sizeof(char),28))),_tag_dyneither(_tmpAC1,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp171=Cyc_read_specs(_tmp16D);
struct _dyneither_ptr _tmp172=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
{struct _dyneither_ptr _tmp176=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp171,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp172=Cyc_split_specs(_tmp176);}
# 1414
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp179=_tmp174;void*_tmp17B;_LL32: {struct Cyc_Core_Not_found_exn_struct*_tmp17A=(struct Cyc_Core_Not_found_exn_struct*)_tmp179;if(_tmp17A->tag != Cyc_Core_Not_found)goto _LL34;}_LL33:
# 1417
 goto _LL31;_LL34: _tmp17B=_tmp179;_LL35:(void)_throw(_tmp17B);_LL31:;}};}
# 1419
if((struct _dyneither_ptr*)_tmp172.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp172);}
# 1424
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp167);{
const char*_tmpAC3;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpAC3="",_tag_dyneither(_tmpAC3,sizeof(char),1))))== 0){
# 1442 "cyclone.cyc"
const char*_tmp17D=Cyc_produce_dependencies?"":" -E";
const char*_tmpACB;void*_tmpACA[3];struct Cyc_String_pa_PrintArg_struct _tmpAC9;const char*_tmpAC8;struct Cyc_String_pa_PrintArg_struct _tmpAC7;struct Cyc_String_pa_PrintArg_struct _tmpAC6;Cyc_set_cpp((struct _dyneither_ptr)((_tmpAC6.tag=0,((_tmpAC6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16D),((_tmpAC7.tag=0,((_tmpAC7.f1=(struct _dyneither_ptr)((_tmpAC8=_tmp17D,_tag_dyneither(_tmpAC8,sizeof(char),_get_zero_arr_size_char((void*)_tmpAC8,1)))),((_tmpAC9.tag=0,((_tmpAC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpACA[0]=& _tmpAC9,((_tmpACA[1]=& _tmpAC7,((_tmpACA[2]=& _tmpAC6,Cyc_aprintf(((_tmpACB="%s -w -x c%s -specs %s",_tag_dyneither(_tmpACB,sizeof(char),23))),_tag_dyneither(_tmpACA,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1446
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpACE;void*_tmpACD;(_tmpACD=0,Cyc_fprintf(Cyc_stderr,((_tmpACE="missing file\n",_tag_dyneither(_tmpACE,sizeof(char),14))),_tag_dyneither(_tmpACD,sizeof(void*),0)));}
exit(1);}
# 1455
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpACF;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpACF="cyc_setjmp.h",_tag_dyneither(_tmpACF,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAD0;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAD0="cyc_include.h",_tag_dyneither(_tmpAD0,sizeof(char),14))));}
# 1461
{struct Cyc_List_List*_tmp372=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp372 != 0;_tmp372=_tmp372->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp372->hd));
if(Cyc_compile_failure){int _tmp373=1;_npop_handler(0);return _tmp373;}}}
# 1466
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp374=0;_npop_handler(0);return _tmp374;}
# 1472
if(Cyc_ccargs == 0){int _tmp375=0;_npop_handler(0);return _tmp375;}
# 1475
{const char*_tmpAD1;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAD1="-L",_tag_dyneither(_tmpAD1,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1477
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAD2;struct _dyneither_ptr _tmp377=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAD2=" ",_tag_dyneither(_tmpAD2,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpAD4;struct Cyc_List_List*_tmpAD3;struct _dyneither_ptr _tmp378=Cyc_str_sepstr(((_tmpAD3=_cycalloc(sizeof(*_tmpAD3)),((_tmpAD3->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAD3->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAD3)))))),((_tmpAD4=" ",_tag_dyneither(_tmpAD4,sizeof(char),2))));
# 1483
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpAD6;const char*_tmpAD5;int _tmp379=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1488
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAD6=".a",_tag_dyneither(_tmpAD6,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAD5=".lib",_tag_dyneither(_tmpAD5,sizeof(char),5))));
if(_tmp379){
stdlib=0;{
const char*_tmpAD7;stdlib_string=(struct _dyneither_ptr)((_tmpAD7="",_tag_dyneither(_tmpAD7,sizeof(char),1)));};}else{
# 1496
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpAD8;libcyc_flag=((_tmpAD8="-lcyc_a",_tag_dyneither(_tmpAD8,sizeof(char),8)));}
{const char*_tmpAD9;nogc_filename=((_tmpAD9="nogc_a.a",_tag_dyneither(_tmpAD9,sizeof(char),9)));}{
const char*_tmpADA;runtime_filename=((_tmpADA="runtime_cyc_a.o",_tag_dyneither(_tmpADA,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpADB;libcyc_flag=((_tmpADB="-lcyc_nocheck",_tag_dyneither(_tmpADB,sizeof(char),14)));}
{const char*_tmpADC;nogc_filename=((_tmpADC="nogc.a",_tag_dyneither(_tmpADC,sizeof(char),7)));}{
const char*_tmpADD;runtime_filename=((_tmpADD="runtime_cyc.o",_tag_dyneither(_tmpADD,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpADE;libcyc_flag=((_tmpADE="-lcyc_pg",_tag_dyneither(_tmpADE,sizeof(char),9)));}
{const char*_tmpADF;runtime_filename=((_tmpADF="runtime_cyc_pg.o",_tag_dyneither(_tmpADF,sizeof(char),17)));}{
const char*_tmpAE0;nogc_filename=((_tmpAE0="nogc_pg.a",_tag_dyneither(_tmpAE0,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1514
{const char*_tmpAE1;libcyc_flag=((_tmpAE1="-lcycboot",_tag_dyneither(_tmpAE1,sizeof(char),10)));}
{const char*_tmpAE2;nogc_filename=((_tmpAE2="nogc.a",_tag_dyneither(_tmpAE2,sizeof(char),7)));}{
const char*_tmpAE3;runtime_filename=((_tmpAE3="runtime_cyc.o",_tag_dyneither(_tmpAE3,sizeof(char),14)));};}else{
# 1518
{const char*_tmpAE4;libcyc_flag=((_tmpAE4="-lcyc",_tag_dyneither(_tmpAE4,sizeof(char),6)));}
{const char*_tmpAE5;nogc_filename=((_tmpAE5="nogc.a",_tag_dyneither(_tmpAE5,sizeof(char),7)));}{
const char*_tmpAE6;runtime_filename=((_tmpAE6="runtime_cyc.o",_tag_dyneither(_tmpAE6,sizeof(char),14)));};}}}}
# 1522
{const char*_tmpAE7;gc_filename=((_tmpAE7="gc.a",_tag_dyneither(_tmpAE7,sizeof(char),5)));}{
# 1524
struct _dyneither_ptr _tmp38B=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp38C=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1531
stdlib=0;{
const char*_tmpAED;void*_tmpAEC[3];struct Cyc_String_pa_PrintArg_struct _tmpAEB;struct Cyc_String_pa_PrintArg_struct _tmpAEA;struct Cyc_String_pa_PrintArg_struct _tmpAE9;stdlib_string=(struct _dyneither_ptr)((_tmpAE9.tag=0,((_tmpAE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38B),((_tmpAEA.tag=0,((_tmpAEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpAEB.tag=0,((_tmpAEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38C),((_tmpAEC[0]=& _tmpAEB,((_tmpAEC[1]=& _tmpAEA,((_tmpAEC[2]=& _tmpAE9,Cyc_aprintf(((_tmpAED=" %s %s %s",_tag_dyneither(_tmpAED,sizeof(char),10))),_tag_dyneither(_tmpAEC,sizeof(void*),3))))))))))))))))))));};};}
# 1535
if(Cyc_ic_r){struct _handler_cons _tmp392;_push_handler(& _tmp392);{int _tmp394=0;if(setjmp(_tmp392.handler))_tmp394=1;if(!_tmp394){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp395=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp379){
struct Cyc_List_List*_tmpAEE;_tmp395=((_tmpAEE=_cycalloc(sizeof(*_tmpAEE)),((_tmpAEE->hd=Cyc_final_strptr,((_tmpAEE->tl=_tmp395,_tmpAEE))))));}{
# 1543
struct Cyc_Interface_I*_tmp397=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp395,_tmp395);
if(_tmp397 == 0){
{const char*_tmpAF1;void*_tmpAF0;(_tmpAF0=0,Cyc_fprintf(Cyc_stderr,((_tmpAF1="Error: interfaces incompatible\n",_tag_dyneither(_tmpAF1,sizeof(char),32))),_tag_dyneither(_tmpAF0,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp39A=1;_npop_handler(1);return _tmp39A;};}
# 1550
if(_tmp379){
if(Cyc_output_file != 0){
const char*_tmpAF5;void*_tmpAF4[1];struct Cyc_String_pa_PrintArg_struct _tmpAF3;struct _dyneither_ptr _tmp39B=(_tmpAF3.tag=0,((_tmpAF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAF4[0]=& _tmpAF3,Cyc_aprintf(((_tmpAF5="%s.cycio",_tag_dyneither(_tmpAF5,sizeof(char),9))),_tag_dyneither(_tmpAF4,sizeof(void*),1)))))));
const char*_tmpAF7;const char*_tmpAF6;struct Cyc___cycFILE*_tmp39C=Cyc_try_file_open((struct _dyneither_ptr)_tmp39B,((_tmpAF6="wb",_tag_dyneither(_tmpAF6,sizeof(char),3))),((_tmpAF7="interface object file",_tag_dyneither(_tmpAF7,sizeof(char),22))));
if(_tmp39C == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp39D=1;_npop_handler(1);return _tmp39D;};}
# 1559
Cyc_Interface_save(_tmp397,_tmp39C);
Cyc_file_close(_tmp39C);}}else{
# 1563
const char*_tmpAFC;const char*_tmpAFB;struct _tuple20*_tmpAFA;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp397,(
(_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA->f1=((_tmpAFB="empty interface",_tag_dyneither(_tmpAFB,sizeof(char),16))),((_tmpAFA->f2=((_tmpAFC="global interface",_tag_dyneither(_tmpAFC,sizeof(char),17))),_tmpAFA)))))))){
{const char*_tmpAFF;void*_tmpAFE;(_tmpAFE=0,Cyc_fprintf(Cyc_stderr,((_tmpAFF="Error: some objects are still undefined\n",_tag_dyneither(_tmpAFF,sizeof(char),41))),_tag_dyneither(_tmpAFE,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A8=1;_npop_handler(1);return _tmp3A8;};}}};};
# 1536
;_pop_handler();}else{void*_tmp393=(void*)_exn_thrown;void*_tmp3AA=_tmp393;void*_tmp3AB;void*_tmp3AC;_LL37: _tmp3AB=_tmp3AA;_LL38:
# 1573
{const char*_tmpB09;void*_tmpB08[3];const char*_tmpB07;struct Cyc_String_pa_PrintArg_struct _tmpB06;const char*_tmpB05;struct Cyc_String_pa_PrintArg_struct _tmpB04;struct Cyc_Int_pa_PrintArg_struct _tmpB03;(_tmpB03.tag=1,((_tmpB03.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpB04.tag=0,((_tmpB04.f1=(struct _dyneither_ptr)((_tmpB05=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpB05,sizeof(char),_get_zero_arr_size_char((void*)_tmpB05,1)))),((_tmpB06.tag=0,((_tmpB06.f1=(struct _dyneither_ptr)((_tmpB07=Cyc_Core_get_exn_name(_tmp3AB),_tag_dyneither(_tmpB07,sizeof(char),_get_zero_arr_size_char((void*)_tmpB07,1)))),((_tmpB08[0]=& _tmpB06,((_tmpB08[1]=& _tmpB04,((_tmpB08[2]=& _tmpB03,Cyc_fprintf(Cyc_stderr,((_tmpB09="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpB09,sizeof(char),131))),_tag_dyneither(_tmpB08,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3B4=1;_npop_handler(0);return _tmp3B4;};_LL39: _tmp3AC=_tmp3AA;_LL3A:(void)_throw(_tmp3AC);_LL36:;}};}{
# 1579
const char*_tmpB0A;struct _dyneither_ptr outfile_str=(_tmpB0A="",_tag_dyneither(_tmpB0A,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpB0E;void*_tmpB0D[1];struct Cyc_String_pa_PrintArg_struct _tmpB0C;outfile_str=(struct _dyneither_ptr)((_tmpB0C.tag=0,((_tmpB0C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpB0D[0]=& _tmpB0C,Cyc_aprintf(((_tmpB0E=" -o %s",_tag_dyneither(_tmpB0E,sizeof(char),7))),_tag_dyneither(_tmpB0D,sizeof(void*),1))))))));}{
# 1583
const char*_tmpB16;void*_tmpB15[5];struct Cyc_String_pa_PrintArg_struct _tmpB14;struct Cyc_String_pa_PrintArg_struct _tmpB13;struct Cyc_String_pa_PrintArg_struct _tmpB12;struct Cyc_String_pa_PrintArg_struct _tmpB11;struct Cyc_String_pa_PrintArg_struct _tmpB10;struct _dyneither_ptr _tmp3B8=
(_tmpB10.tag=0,((_tmpB10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp378),((_tmpB11.tag=0,((_tmpB11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpB12.tag=0,((_tmpB12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpB13.tag=0,((_tmpB13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp377),((_tmpB14.tag=0,((_tmpB14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpB15[0]=& _tmpB14,((_tmpB15[1]=& _tmpB13,((_tmpB15[2]=& _tmpB12,((_tmpB15[3]=& _tmpB11,((_tmpB15[4]=& _tmpB10,Cyc_aprintf(((_tmpB16="%s %s%s %s%s",_tag_dyneither(_tmpB16,sizeof(char),13))),_tag_dyneither(_tmpB15,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1586
if(Cyc_v_r){{const char*_tmpB1A;void*_tmpB19[1];struct Cyc_String_pa_PrintArg_struct _tmpB18;(_tmpB18.tag=0,((_tmpB18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B8),((_tmpB19[0]=& _tmpB18,Cyc_fprintf(Cyc_stderr,((_tmpB1A="%s\n",_tag_dyneither(_tmpB1A,sizeof(char),4))),_tag_dyneither(_tmpB19,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3B8,sizeof(char),1))!= 0){
{const char*_tmpB1D;void*_tmpB1C;(_tmpB1C=0,Cyc_fprintf(Cyc_stderr,((_tmpB1D="Error: C compiler failed\n",_tag_dyneither(_tmpB1D,sizeof(char),26))),_tag_dyneither(_tmpB1C,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3BE=1;_npop_handler(0);return _tmp3BE;};}
# 1593
Cyc_remove_cfiles();{
# 1595
int _tmp3BF=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3BF;};};};};};
# 1106 "cyclone.cyc"
;_pop_region(r);};}
