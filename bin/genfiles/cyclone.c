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
int Cyc_Position_error_p();struct Cyc_Relations_Reln;
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1198 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1200
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
typedef struct Cyc_Abstractdeclarator Cyc_abstractdeclarator_t;struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
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
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 230
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
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
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
struct _dyneither_ptr*_tmp3D1;struct Cyc_List_List*_tmp3D0;Cyc_ccargs=((_tmp3D0=_cycalloc(sizeof(*_tmp3D0)),((_tmp3D0->hd=((_tmp3D1=_cycalloc(sizeof(*_tmp3D1)),((_tmp3D1[0]=s,_tmp3D1)))),((_tmp3D0->tl=Cyc_ccargs,_tmp3D0))))));}
# 102
void Cyc_set_c_compiler(struct _dyneither_ptr s){
const char*_tmp3D2;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D2="vc",_tag_dyneither(_tmp3D2,sizeof(char),3))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;{
const char*_tmp3D3;Cyc_add_ccarg(((_tmp3D3="-DVC_C",_tag_dyneither(_tmp3D3,sizeof(char),7))));};}else{
# 107
const char*_tmp3D4;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp3D4="gcc",_tag_dyneither(_tmp3D4,sizeof(char),4))))== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;{
const char*_tmp3D5;Cyc_add_ccarg(((_tmp3D5="-DGCC_C",_tag_dyneither(_tmp3D5,sizeof(char),8))));};}}}
# 115
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 123
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp3D6;Cyc_output_file=((_tmp3D6=_cycalloc(sizeof(*_tmp3D6)),((_tmp3D6[0]=s,_tmp3D6))));}
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
struct _dyneither_ptr*_tmp3D9;struct Cyc_List_List*_tmp3D8;Cyc_cppargs=((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8->hd=((_tmp3D9=_cycalloc(sizeof(*_tmp3D9)),((_tmp3D9[0]=s,_tmp3D9)))),((_tmp3D8->tl=Cyc_cppargs,_tmp3D8))))));}
# 154
static void Cyc_add_iprefix(struct _dyneither_ptr s){
const char*_tmp3DD;void*_tmp3DC[1];struct Cyc_String_pa_PrintArg_struct _tmp3DB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DB.tag=0,((_tmp3DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3DC[0]=& _tmp3DB,Cyc_aprintf(((_tmp3DD="-iprefix %s",_tag_dyneither(_tmp3DD,sizeof(char),12))),_tag_dyneither(_tmp3DC,sizeof(void*),1)))))))));}
# 157
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
const char*_tmp3E1;void*_tmp3E0[1];struct Cyc_String_pa_PrintArg_struct _tmp3DF;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3DF.tag=0,((_tmp3DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E0[0]=& _tmp3DF,Cyc_aprintf(((_tmp3E1="-iwithprefix %s",_tag_dyneither(_tmp3E1,sizeof(char),16))),_tag_dyneither(_tmp3E0,sizeof(void*),1)))))))));}
# 160
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_String_pa_PrintArg_struct _tmp3E3;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E4[0]=& _tmp3E3,Cyc_aprintf(((_tmp3E5="-iwithprefixbefore %s",_tag_dyneither(_tmp3E5,sizeof(char),22))),_tag_dyneither(_tmp3E4,sizeof(void*),1)))))))));}
# 163
static void Cyc_add_isystem(struct _dyneither_ptr s){
const char*_tmp3E9;void*_tmp3E8[1];struct Cyc_String_pa_PrintArg_struct _tmp3E7;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3E7.tag=0,((_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3E8[0]=& _tmp3E7,Cyc_aprintf(((_tmp3E9="-isystem %s",_tag_dyneither(_tmp3E9,sizeof(char),12))),_tag_dyneither(_tmp3E8,sizeof(void*),1)))))))));}
# 166
static void Cyc_add_idirafter(struct _dyneither_ptr s){
const char*_tmp3ED;void*_tmp3EC[1];struct Cyc_String_pa_PrintArg_struct _tmp3EB;Cyc_add_cpparg((struct _dyneither_ptr)((_tmp3EB.tag=0,((_tmp3EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp3EC[0]=& _tmp3EB,Cyc_aprintf(((_tmp3ED="-idirafter %s",_tag_dyneither(_tmp3ED,sizeof(char),14))),_tag_dyneither(_tmp3EC,sizeof(void*),1)))))))));}
# 170
static struct _dyneither_ptr Cyc_target_arch={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_target_arch(struct _dyneither_ptr s){
# 173
Cyc_target_arch=s;
{const char*_tmp3EE;Cyc_add_ccarg(((_tmp3EE="-b",_tag_dyneither(_tmp3EE,sizeof(char),3))));}
Cyc_add_ccarg(s);}
# 178
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}
# 183
static void Cyc_print_version(){
{const char*_tmp3F2;void*_tmp3F1[1];struct Cyc_String_pa_PrintArg_struct _tmp3F0;(_tmp3F0.tag=0,((_tmp3F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cversion)),((_tmp3F1[0]=& _tmp3F0,Cyc_printf(((_tmp3F2="The Cyclone compiler, version %s\n",_tag_dyneither(_tmp3F2,sizeof(char),34))),_tag_dyneither(_tmp3F1,sizeof(void*),1)))))));}
{const char*_tmp3F7;void*_tmp3F6[2];struct Cyc_String_pa_PrintArg_struct _tmp3F5;struct Cyc_String_pa_PrintArg_struct _tmp3F4;(_tmp3F4.tag=0,((_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cstring_to_string(Ccomp)),((_tmp3F5.tag=0,((_tmp3F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Carch)),((_tmp3F6[0]=& _tmp3F5,((_tmp3F6[1]=& _tmp3F4,Cyc_printf(((_tmp3F7="Compiled for architecture: %s CC=\"%s\"\n",_tag_dyneither(_tmp3F7,sizeof(char),39))),_tag_dyneither(_tmp3F6,sizeof(void*),2)))))))))))));}
{const char*_tmp3FB;void*_tmp3FA[1];struct Cyc_String_pa_PrintArg_struct _tmp3F9;(_tmp3F9.tag=0,((_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path)),((_tmp3FA[0]=& _tmp3F9,Cyc_printf(((_tmp3FB="Standard library directory: %s\n",_tag_dyneither(_tmp3FB,sizeof(char),32))),_tag_dyneither(_tmp3FA,sizeof(void*),1)))))));}
{const char*_tmp3FF;void*_tmp3FE[1];struct Cyc_String_pa_PrintArg_struct _tmp3FD;(_tmp3FD.tag=0,((_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path)),((_tmp3FE[0]=& _tmp3FD,Cyc_printf(((_tmp3FF="Standard include directory: %s\n",_tag_dyneither(_tmp3FF,sizeof(char),32))),_tag_dyneither(_tmp3FE,sizeof(void*),1)))))));}
exit(0);}
# 192
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp27=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp27 <= 4)return 0;else{
const char*_tmp400;return Cyc_strcmp((struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp27 - 4)),((_tmp400=".cyc",_tag_dyneither(_tmp400,sizeof(char),5))))== 0;}}
# 198
static struct Cyc_List_List*Cyc_cyclone_exec_path=0;
static void Cyc_add_cyclone_exec_path(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 2)return;{
struct _dyneither_ptr _tmp2A=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,2,_tmp29 - 2);
struct _dyneither_ptr*_tmp403;struct Cyc_List_List*_tmp402;Cyc_cyclone_exec_path=((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402->hd=((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=_tmp2A,_tmp403)))),((_tmp402->tl=Cyc_cyclone_exec_path,_tmp402))))));};}
# 211
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
const char*_tmp404;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp404="-lxml",_tag_dyneither(_tmp404,sizeof(char),6))))== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 217
const char*_tmp405;Cyc_add_ccarg(((_tmp405="-lxml_a",_tag_dyneither(_tmp405,sizeof(char),8))));}}else{
# 219
struct _dyneither_ptr*_tmp408;struct Cyc_List_List*_tmp407;Cyc_libargs=((_tmp407=_cycalloc(sizeof(*_tmp407)),((_tmp407->hd=((_tmp408=_cycalloc(sizeof(*_tmp408)),((_tmp408[0]=s,_tmp408)))),((_tmp407->tl=Cyc_libargs,_tmp407))))));}}
# 222
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 226
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;{
const char*_tmp409;Cyc_add_ccarg(((_tmp409="-save-temps",_tag_dyneither(_tmp409,sizeof(char),12))));};}
# 231
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;{
const char*_tmp40A;Cyc_add_cpparg(((_tmp40A="-M",_tag_dyneither(_tmp40A,sizeof(char),3))));};}
# 238
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
struct _dyneither_ptr*_tmp40B;Cyc_dependencies_target=((_tmp40B=_cycalloc(sizeof(*_tmp40B)),((_tmp40B[0]=s,_tmp40B))));}
# 243
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;{
const char*_tmp40C;Cyc_add_ccarg(((_tmp40C="-c",_tag_dyneither(_tmp40C,sizeof(char),3))));};}
# 248
static void Cyc_set_nocppprecomp(){
{const char*_tmp40D;Cyc_add_cpparg(((_tmp40D="-no-cpp-precomp",_tag_dyneither(_tmp40D,sizeof(char),16))));}{
const char*_tmp40E;Cyc_add_ccarg(((_tmp40E="-no-cpp-precomp",_tag_dyneither(_tmp40E,sizeof(char),16))));};}
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
{const char*_tmp40F;Cyc_add_ccarg(((_tmp40F="-DCYC_ANSI_OUTPUT",_tag_dyneither(_tmp40F,sizeof(char),18))));}
Cyc_set_elim_se();}
# 269
static void Cyc_set_noboundschecks(){
const char*_tmp410;Cyc_add_ccarg(((_tmp410="-DNO_CYC_BOUNDS_CHECKS",_tag_dyneither(_tmp410,sizeof(char),23))));}
# 272
static void Cyc_set_nonullchecks(){
const char*_tmp411;Cyc_add_ccarg(((_tmp411="-DNO_CYC_NULL_CHECKS",_tag_dyneither(_tmp411,sizeof(char),21))));}
# 275
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 281
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;{
const char*_tmp412;Cyc_add_ccarg(((_tmp412="-DNO_CYC_PREFIX",_tag_dyneither(_tmp412,sizeof(char),16))));};}
# 286
static void Cyc_set_pa(){
Cyc_pa_r=1;
{const char*_tmp413;Cyc_add_ccarg(((_tmp413="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp413,sizeof(char),21))));}{
const char*_tmp414;Cyc_add_cpparg(((_tmp414="-DCYC_REGION_PROFILE",_tag_dyneither(_tmp414,sizeof(char),21))));};}
# 292
static void Cyc_set_pg(){
Cyc_pg_r=1;{
const char*_tmp415;Cyc_add_ccarg(((_tmp415="-pg",_tag_dyneither(_tmp415,sizeof(char),4))));};}
# 297
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;{
const char*_tmp416;Cyc_add_ccarg(((_tmp416="-S",_tag_dyneither(_tmp416,sizeof(char),3))));};}
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
const char*_tmp417;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp417="cyc",_tag_dyneither(_tmp417,sizeof(char),4))))== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
const char*_tmp418;if(Cyc_strcmp((struct _dyneither_ptr)s,((_tmp418="none",_tag_dyneither(_tmp418,sizeof(char),5))))== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 323
const char*_tmp41C;void*_tmp41B[1];struct Cyc_String_pa_PrintArg_struct _tmp41A;(_tmp41A.tag=0,((_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp41B[0]=& _tmp41A,Cyc_fprintf(Cyc_stderr,((_tmp41C="Input type '%s' not supported\n",_tag_dyneither(_tmp41C,sizeof(char),31))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}}}
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
{struct _dyneither_ptr*_tmp41F;struct Cyc_List_List*_tmp41E;Cyc_cyclone_files=((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E->hd=((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F[0]=s,_tmp41F)))),((_tmp41E->tl=Cyc_cyclone_files,_tmp41E))))));}{
# 347
struct _dyneither_ptr _tmp49=Cyc_make_base_filename(s,Cyc_output_file);
const char*_tmp420;struct _dyneither_ptr _tmp4A=Cyc_strconcat((struct _dyneither_ptr)_tmp49,((_tmp420=".c",_tag_dyneither(_tmp420,sizeof(char),3))));
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
{const char*_tmp424;void*_tmp423[1];struct Cyc_String_pa_PrintArg_struct _tmp422;(_tmp422.tag=0,((_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp423[0]=& _tmp422,Cyc_fprintf(Cyc_stderr,((_tmp424="Error opening spec file %s\n",_tag_dyneither(_tmp424,sizeof(char),28))),_tag_dyneither(_tmp423,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 0;}
# 379
while(1){
while(1){
c=Cyc_fgetc(spec_file);
if(isspace(c))continue;
if(c == '*')break;
if(c != - 1){
{const char*_tmp429;void*_tmp428[2];struct Cyc_String_pa_PrintArg_struct _tmp427;struct Cyc_Int_pa_PrintArg_struct _tmp426;(_tmp426.tag=1,((_tmp426.f1=(unsigned long)c,((_tmp427.tag=0,((_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp428[0]=& _tmp427,((_tmp428[1]=& _tmp426,Cyc_fprintf(Cyc_stderr,((_tmp429="Error reading spec file %s: unexpected character '%c'\n",_tag_dyneither(_tmp429,sizeof(char),55))),_tag_dyneither(_tmp428,sizeof(void*),2)))))))))))));}
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
{const char*_tmp42D;void*_tmp42C[1];struct Cyc_String_pa_PrintArg_struct _tmp42B;(_tmp42B.tag=0,((_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp42C[0]=& _tmp42B,Cyc_fprintf(Cyc_stderr,((_tmp42D="Error reading spec file %s: unexpected EOF\n",_tag_dyneither(_tmp42D,sizeof(char),44))),_tag_dyneither(_tmp42C,sizeof(void*),1)))))));}
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
{const char*_tmp431;void*_tmp430[1];struct Cyc_String_pa_PrintArg_struct _tmp42F;(_tmp42F.tag=0,((_tmp42F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp430[0]=& _tmp42F,Cyc_fprintf(Cyc_stderr,((_tmp431="Error reading spec file %s: string name too long\n",_tag_dyneither(_tmp431,sizeof(char),50))),_tag_dyneither(_tmp430,sizeof(void*),1)))))));}
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
{struct _tuple21*_tmp43E;struct _dyneither_ptr*_tmp43D;const char*_tmp43C;struct _dyneither_ptr*_tmp43B;struct Cyc_List_List*_tmp43A;_tmp4C=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->hd=((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E->f1=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp43D)))),((_tmp43E->f2=(
(_tmp43B=_cycalloc(sizeof(*_tmp43B)),((_tmp43B[0]=(struct _dyneither_ptr)Cyc_strdup(((_tmp43C="",_tag_dyneither(_tmp43C,sizeof(char),1)))),_tmp43B)))),_tmp43E)))))),((_tmp43A->tl=_tmp4C,_tmp43A))))));}
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
{const char*_tmp443;void*_tmp442[2];struct Cyc_String_pa_PrintArg_struct _tmp441;struct Cyc_String_pa_PrintArg_struct _tmp440;(_tmp440.tag=0,((_tmp440.f1=(struct _dyneither_ptr)
# 439
_tag_dyneither(strname,sizeof(char),256),((_tmp441.tag=0,((_tmp441.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp442[0]=& _tmp441,((_tmp442[1]=& _tmp440,Cyc_fprintf(Cyc_stderr,((_tmp443="Error reading spec file %s: value of %s too long\n",_tag_dyneither(_tmp443,sizeof(char),50))),_tag_dyneither(_tmp442,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
goto CLEANUP_AND_RETURN;}}
# 445
{struct _tuple21*_tmp44C;struct _dyneither_ptr*_tmp44B;struct _dyneither_ptr*_tmp44A;struct Cyc_List_List*_tmp449;_tmp4C=((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449->hd=((_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C->f1=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strname,sizeof(char),256)),_tmp44B)))),((_tmp44C->f2=(
(_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(strvalue,sizeof(char),4096)),_tmp44A)))),_tmp44C)))))),((_tmp449->tl=_tmp4C,_tmp449))))));}
if(c == - 1)goto CLEANUP_AND_RETURN;}
# 450
CLEANUP_AND_RETURN:
 Cyc_fclose(spec_file);
return _tmp4C;}struct _dyneither_ptr Cyc_split_specs(struct _dyneither_ptr cmdline);static void _tmp456(struct _dyneither_ptr*_tmp6D,unsigned int*_tmp455,unsigned int*_tmp454,struct _dyneither_ptr**_tmp452){for(*_tmp455=0;*_tmp455 < *_tmp454;(*_tmp455)++){(*_tmp452)[*_tmp455]=(struct _dyneither_ptr)*((struct _dyneither_ptr**)(*_tmp6D).curr)[(int)*_tmp455];}}
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
struct _dyneither_ptr*_tmp44F;struct Cyc_List_List*_tmp44E;_tmp68=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E->hd=((_tmp44F=_cycalloc(sizeof(*_tmp44F)),((_tmp44F[0]=(struct _dyneither_ptr)Cyc_strdup(_tag_dyneither(buf,sizeof(char),4096)),_tmp44F)))),((_tmp44E->tl=_tmp68,_tmp44E))))));};}
# 500
DONE:
 _tmp68=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp68);
{struct Cyc_List_List*_tmp450;_tmp68=((_tmp450=_cycalloc(sizeof(*_tmp450)),((_tmp450->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmp450->tl=_tmp68,_tmp450))))));}{
struct _dyneither_ptr _tmp6D=((struct _dyneither_ptr(*)(struct Cyc_List_List*x))Cyc_List_to_array)(_tmp68);
unsigned int _tmp455;unsigned int _tmp454;struct _dyneither_ptr _tmp453;struct _dyneither_ptr*_tmp452;unsigned int _tmp451;struct _dyneither_ptr _tmp6E=(_tmp451=_get_dyneither_size(_tmp6D,sizeof(struct _dyneither_ptr*)),((_tmp452=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp451)),((_tmp453=_tag_dyneither(_tmp452,sizeof(struct _dyneither_ptr),_tmp451),((((_tmp454=_tmp451,_tmp456(& _tmp6D,& _tmp455,& _tmp454,& _tmp452))),_tmp453)))))));
return _tmp6E;};};}
# 510
int Cyc_compile_failure=0;
# 512
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp73;_push_handler(& _tmp73);{int _tmp75=0;if(setjmp(_tmp73.handler))_tmp75=1;if(!_tmp75){{struct Cyc___cycFILE*_tmp76=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp76;};_pop_handler();}else{void*_tmp74=(void*)_exn_thrown;void*_tmp78=_tmp74;void*_tmp79;_LL1:;_LL2:
# 516
 Cyc_compile_failure=1;
{const char*_tmp45B;void*_tmp45A[2];struct Cyc_String_pa_PrintArg_struct _tmp459;struct Cyc_String_pa_PrintArg_struct _tmp458;(_tmp458.tag=0,((_tmp458.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp459.tag=0,((_tmp459.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp45A[0]=& _tmp459,((_tmp45A[1]=& _tmp458,Cyc_fprintf(Cyc_stderr,((_tmp45B="\nError: couldn't open %s %s\n",_tag_dyneither(_tmp45B,sizeof(char),29))),_tag_dyneither(_tmp45A,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);
return 0;_LL3: _tmp79=_tmp78;_LL4:(void)_throw(_tmp79);_LL0:;}};}
# 523
void CYCALLOCPROFILE_mark(const char*s);
# 525
struct Cyc_List_List*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(void*,struct Cyc_List_List*),void*env,struct Cyc_List_List*tds){
# 529
const char*_tmp45C;struct _dyneither_ptr explain_string=(_tmp45C="",_tag_dyneither(_tmp45C,sizeof(char),1));
struct Cyc_List_List*_tmp7E=0;
# 532
CYCALLOCPROFILE_mark((const char*)_check_null(_untag_dyneither_ptr(stage_name,sizeof(char),1)));
# 534
{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){_tmp7E=f(env,tds);;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*_tmp83=_tmp80;void*_tmp84;void*_tmp85;_LL6: _tmp84=_tmp83;_LL7:
# 537
{const char*_tmp460;void*_tmp45F[1];struct Cyc_String_pa_PrintArg_struct _tmp45E;(_tmp45E.tag=0,((_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp45F[0]=& _tmp45E,Cyc_fprintf(Cyc_stderr,((_tmp460="COMPILER STAGE %s\n",_tag_dyneither(_tmp460,sizeof(char),19))),_tag_dyneither(_tmp45F,sizeof(void*),1)))))));}
Cyc_Core_rethrow(_tmp84);_LL8: _tmp85=_tmp83;_LL9:(void)_throw(_tmp85);_LL5:;}};}
# 540
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
{const char*_tmp463;void*_tmp462;(_tmp462=0,Cyc_fprintf(Cyc_stderr,((_tmp463="\nCOMPILATION FAILED!\n",_tag_dyneither(_tmp463,sizeof(char),22))),_tag_dyneither(_tmp462,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);
return _tmp7E;}else{
# 547
if(Cyc_v_r){
{const char*_tmp467;void*_tmp466[1];struct Cyc_String_pa_PrintArg_struct _tmp465;(_tmp465.tag=0,((_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name),((_tmp466[0]=& _tmp465,Cyc_fprintf(Cyc_stderr,((_tmp467="%s completed.\n",_tag_dyneither(_tmp467,sizeof(char),15))),_tag_dyneither(_tmp466,sizeof(void*),1)))))));}
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
ans=Cyc_Parse_parse_file(f);;_pop_handler();}else{void*_tmp90=(void*)_exn_thrown;void*_tmp93=_tmp90;void*_tmp95;void*_tmp96;_LLB: {struct Cyc_Position_Exit_exn_struct*_tmp94=(struct Cyc_Position_Exit_exn_struct*)_tmp93;if(_tmp94->tag != Cyc_Position_Exit)goto _LLD;}_LLC:
# 562
 Cyc_compile_failure=1;goto _LLA;_LLD: _tmp95=_tmp93;_LLE:
 Cyc_Core_rethrow(_tmp95);_LLF: _tmp96=_tmp93;_LL10:(void)_throw(_tmp96);_LLA:;}};}
# 565
Cyc_Lex_lex_init(1);
return ans;};}struct _tuple22{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;};
# 569
struct Cyc_List_List*Cyc_do_typecheck(struct _tuple22*tcenv,struct Cyc_List_List*tds){
# 571
struct _tuple22 _tmp97=*tcenv;struct _RegionHandle*_tmp99;struct Cyc_Tcenv_Tenv*_tmp9A;struct _tuple22 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
Cyc_Tc_tc(_tmp99,_tmp9A,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(_tmp9A,tds);
return tds;}
# 578
struct Cyc_List_List*Cyc_do_cfcheck(int ignore,struct Cyc_List_List*tds){
Cyc_NewControlFlow_cf_check(tds);
return tds;}struct _tuple23{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 585
struct Cyc_List_List*Cyc_do_interface(struct _tuple23*params,struct Cyc_List_List*tds){
struct Cyc_Tcenv_Tenv*_tmp9C;struct Cyc___cycFILE*_tmp9D;struct Cyc___cycFILE*_tmp9E;struct _tuple23*_tmp9B=params;_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;_tmp9E=_tmp9B->f3;{
struct Cyc_Interface_I*_tmp9F=Cyc_Interface_extract(_tmp9C->ae);
if(_tmp9D == 0)
Cyc_Interface_save(_tmp9F,_tmp9E);else{
# 591
struct Cyc_Interface_I*_tmpA0=Cyc_Interface_parse(_tmp9D);
const char*_tmp46C;const char*_tmp46B;struct _tuple20*_tmp46A;if(!Cyc_Interface_is_subinterface(_tmpA0,_tmp9F,((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A->f1=((_tmp46B="written interface",_tag_dyneither(_tmp46B,sizeof(char),18))),((_tmp46A->f2=((_tmp46C="maximal interface",_tag_dyneither(_tmp46C,sizeof(char),18))),_tmp46A))))))))
Cyc_compile_failure=1;else{
# 595
Cyc_Interface_save(_tmpA0,_tmp9E);}}
# 597
return tds;};}
# 600
struct Cyc_List_List*Cyc_do_translate(int ignore,struct Cyc_List_List*tds){
return Cyc_Toc_toc(tds);}
# 603
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 608
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 610
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 612
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 617
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 620
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{const char*_tmp46F;void*_tmp46E;(_tmp46E=0,Cyc_fprintf(Cyc_stderr,((_tmp46F="Internal error: copy_internal_file called with NULL\n",_tag_dyneither(_tmp46F,sizeof(char),53))),_tag_dyneither(_tmp46E,sizeof(void*),0)));}
return 1;}{
# 624
const char*_tmp471;const char*_tmp470;struct Cyc___cycFILE*_tmpA6=Cyc_try_file_open(file,((_tmp470="r",_tag_dyneither(_tmp470,sizeof(char),2))),((_tmp471="internal compiler file",_tag_dyneither(_tmp471,sizeof(char),23))));
if(_tmpA6 == 0)return 1;{
unsigned long n_read=1024;
unsigned long n_written;
char buf[1024];
{unsigned int _tmpAD=1024;unsigned int i;for(i=0;i < _tmpAD;i ++){buf[i]=(char)'\000';}}while(n_read == 1024){
n_read=Cyc_fread(_tag_dyneither(buf,sizeof(char),1024),1,1024,_tmpA6);
if(n_read != 1024  && !Cyc_feof(_tmpA6)){
Cyc_compile_failure=1;
{const char*_tmp475;void*_tmp474[1];struct Cyc_String_pa_PrintArg_struct _tmp473;(_tmp473.tag=0,((_tmp473.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp474[0]=& _tmp473,Cyc_fprintf(Cyc_stderr,((_tmp475="\nError: problem copying %s\n",_tag_dyneither(_tmp475,sizeof(char),28))),_tag_dyneither(_tmp474,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}
# 637
n_written=Cyc_fwrite(_tag_dyneither(buf,sizeof(char),1024),1,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
{const char*_tmp479;void*_tmp478[1];struct Cyc_String_pa_PrintArg_struct _tmp477;(_tmp477.tag=0,((_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp478[0]=& _tmp477,Cyc_fprintf(Cyc_stderr,((_tmp479="\nError: problem copying %s\n",_tag_dyneither(_tmp479,sizeof(char),28))),_tag_dyneither(_tmp478,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);
return 1;}}
# 645
Cyc_fclose(_tmpA6);
return 0;};};}
# 650
struct Cyc_List_List*Cyc_do_print(struct Cyc___cycFILE*out_file,struct Cyc_List_List*tds){
struct Cyc_Absynpp_Params _tmpB0=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmpB0.expand_typedefs=!Cyc_noexpand_r;
_tmpB0.to_VC=Cyc_Cyclone_tovc_r;
_tmpB0.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmpB0.generate_line_directives=Cyc_generate_line_directives_r;
_tmpB0.print_full_evars=Cyc_print_full_evars_r;
_tmpB0.print_all_tvars=Cyc_print_all_tvars_r;
_tmpB0.print_all_kinds=Cyc_print_all_kinds_r;
_tmpB0.print_all_effects=Cyc_print_all_effects_r;
# 661
if(Cyc_inline_functions_r){
const char*_tmp47C;void*_tmp47B;(_tmp47B=0,Cyc_fprintf(out_file,((_tmp47C="#define _INLINE_FUNCTIONS\n",_tag_dyneither(_tmp47C,sizeof(char),27))),_tag_dyneither(_tmp47B,sizeof(void*),0)));}
# 665
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r){
const char*_tmp47F;void*_tmp47E;(_tmp47E=0,Cyc_fprintf(out_file,((_tmp47F="#include <setjmp.h>\n",_tag_dyneither(_tmp47F,sizeof(char),21))),_tag_dyneither(_tmp47E,sizeof(void*),0)));}else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 671
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 673
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmpB0);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 677
Cyc_Absyndump_set_params(& _tmpB0);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 680
Cyc_fflush(out_file);
return tds;}
# 686
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file(*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 694
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmpB5=0;
unsigned long _tmpB6=Cyc_strlen((struct _dyneither_ptr)s);
# 699
while(_tmpB6 > 0){
struct _dyneither_ptr _tmpB7=Cyc_strchr(s,c);
if((char*)_tmpB7.curr == (char*)(_tag_dyneither(0,0,0)).curr){
{struct _dyneither_ptr*_tmp482;struct Cyc_List_List*_tmp481;_tmpB5=((_tmp481=_cycalloc(sizeof(*_tmp481)),((_tmp481->hd=((_tmp482=_cycalloc(sizeof(*_tmp482)),((_tmp482[0]=s,_tmp482)))),((_tmp481->tl=_tmpB5,_tmp481))))));}
break;}else{
# 706
{struct _dyneither_ptr*_tmp485;struct Cyc_List_List*_tmp484;_tmpB5=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->hd=((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485[0]=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmpB7).curr - s.curr)/ sizeof(char))),_tmp485)))),((_tmp484->tl=_tmpB5,_tmp484))))));}
_tmpB6 -=(((struct _dyneither_ptr)_tmpB7).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmpB7,sizeof(char),1);}}
# 711
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB5);};}
# 715
static int Cyc_file_exists(struct _dyneither_ptr file){
struct Cyc___cycFILE*f=0;
{struct _handler_cons _tmpBC;_push_handler(& _tmpBC);{int _tmpBE=0;if(setjmp(_tmpBC.handler))_tmpBE=1;if(!_tmpBE){{const char*_tmp486;f=Cyc_file_open(file,((_tmp486="r",_tag_dyneither(_tmp486,sizeof(char),2))));};_pop_handler();}else{void*_tmpBD=(void*)_exn_thrown;void*_tmpC1=_tmpBD;void*_tmpC2;_LL12:;_LL13: goto _LL11;_LL14: _tmpC2=_tmpC1;_LL15:(void)_throw(_tmpC2);_LL11:;}};}
if(f == 0)return 0;else{
Cyc_fclose(f);return 1;}}
# 723
static struct _dyneither_ptr Cyc_sprint_list(struct Cyc_List_List*dirs){
const char*_tmp487;struct _dyneither_ptr tmp=(_tmp487="",_tag_dyneither(_tmp487,sizeof(char),1));
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC3=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC3.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC3)== 0)continue;
{const char*_tmp488;_tmpC3=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC3,((_tmp488=":",_tag_dyneither(_tmp488,sizeof(char),2))));}
tmp=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)_tmpC3,(struct _dyneither_ptr)tmp);}
# 731
return tmp;}
# 736
static struct _dyneither_ptr*Cyc_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
if((char*)file.curr == (char*)(_tag_dyneither(0,0,0)).curr)return 0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr _tmpC6=*((struct _dyneither_ptr*)dirs->hd);
if((char*)_tmpC6.curr == (char*)(_tag_dyneither(0,0,0)).curr  || Cyc_strlen((struct _dyneither_ptr)_tmpC6)== 0)continue;{
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_Filename_concat(_tmpC6,file);
if(Cyc_file_exists(s)){struct _dyneither_ptr*_tmp489;return(_tmp489=_cycalloc(sizeof(*_tmp489)),((_tmp489[0]=s,_tmp489)));}};}
# 744
return 0;}
# 747
static struct _dyneither_ptr Cyc_do_find(struct Cyc_List_List*dirs,struct _dyneither_ptr file){
struct _dyneither_ptr*_tmpC8=Cyc_find(dirs,file);
if(_tmpC8 == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
{const char*_tmp48E;void*_tmp48D[2];struct Cyc_String_pa_PrintArg_struct _tmp48C;struct Cyc_String_pa_PrintArg_struct _tmp48B;(_tmp48B.tag=0,((_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_sprint_list(dirs)),((_tmp48C.tag=0,((_tmp48C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp48D[0]=& _tmp48C,((_tmp48D[1]=& _tmp48B,Cyc_fprintf(Cyc_stderr,((_tmp48E="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp48E,sizeof(char),56))),_tag_dyneither(_tmp48D,sizeof(void*),2)))))))))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Failure_exn_struct _tmp49E;const char*_tmp49D;void*_tmp49C[2];struct Cyc_String_pa_PrintArg_struct _tmp49B;struct Cyc_String_pa_PrintArg_struct _tmp49A;struct Cyc_Core_Failure_exn_struct*_tmp499;(int)_throw((void*)((_tmp499=_cycalloc(sizeof(*_tmp499)),((_tmp499[0]=((_tmp49E.tag=Cyc_Core_Failure,((_tmp49E.f1=(struct _dyneither_ptr)((_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sprint_list(dirs)),((_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file),((_tmp49C[0]=& _tmp49B,((_tmp49C[1]=& _tmp49A,Cyc_aprintf(((_tmp49D="Error: can't find internal compiler file %s in path %s\n",_tag_dyneither(_tmp49D,sizeof(char),56))),_tag_dyneither(_tmp49C,sizeof(void*),2)))))))))))))),_tmp49E)))),_tmp499)))));};}
# 759
return*_tmpC8;}
# 764
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 766
struct Cyc_List_List*_tmpD3=0;
for(0;dirs != 0;dirs=dirs->tl){
struct _dyneither_ptr*_tmp4A1;struct Cyc_List_List*_tmp4A0;_tmpD3=((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0->hd=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A1)))),((_tmp4A0->tl=_tmpD3,_tmp4A0))))));}
# 770
_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD3);
return _tmpD3;}
# 776
static struct Cyc_List_List*Cyc_also_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 778
struct Cyc_List_List*_tmpD6=0;
for(0;dirs != 0;dirs=dirs->tl){
{struct Cyc_List_List*_tmp4A2;_tmpD6=((_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->hd=(struct _dyneither_ptr*)dirs->hd,((_tmp4A2->tl=_tmpD6,_tmp4A2))))));}{
struct _dyneither_ptr*_tmp4A5;struct Cyc_List_List*_tmp4A4;_tmpD6=((_tmp4A4=_cycalloc(sizeof(*_tmp4A4)),((_tmp4A4->hd=((_tmp4A5=_cycalloc(sizeof(*_tmp4A5)),((_tmp4A5[0]=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir),_tmp4A5)))),((_tmp4A4->tl=_tmpD6,_tmp4A4))))));};}
# 783
_tmpD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD6);
return _tmpD6;}
# 788
static int Cyc_is_other_special(char c){
switch(c){case '\\': _LL16:
 goto _LL17;case '"': _LL17:
 goto _LL18;case ';': _LL18:
 goto _LL19;case '&': _LL19:
 goto _LL1A;case '(': _LL1A:
 goto _LL1B;case ')': _LL1B:
 goto _LL1C;case '|': _LL1C:
 goto _LL1D;case '^': _LL1D:
 goto _LL1E;case '<': _LL1E:
 goto _LL1F;case '>': _LL1F:
 goto _LL20;case '\n': _LL20:
# 803
 goto _LL21;case '\t': _LL21:
 return 1;default: _LL22:
 return 0;}}static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s);static void _tmp4AE(unsigned int*_tmp4AD,unsigned int*_tmp4AC,char**_tmp4AA){for(*_tmp4AD=0;*_tmp4AD < *_tmp4AC;(*_tmp4AD)++){(*_tmp4AA)[*_tmp4AD]=(char)'\000';}}
# 810
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpDA=Cyc_strlen((struct _dyneither_ptr)s);
# 814
int _tmpDB=0;
int _tmpDC=0;
{int i=0;for(0;i < _tmpDA;++ i){
char _tmpDD=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpDD == '\'')++ _tmpDB;else{
if(Cyc_is_other_special(_tmpDD))++ _tmpDC;}}}
# 823
if(_tmpDB == 0  && _tmpDC == 0)
return s;
# 827
if(_tmpDB == 0){
struct _dyneither_ptr*_tmp4A8;struct _dyneither_ptr*_tmp4A7[3];return(struct _dyneither_ptr)Cyc_strconcat_l(((_tmp4A7[2]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((_tmp4A7[1]=((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8[0]=(struct _dyneither_ptr)s,_tmp4A8)))),((_tmp4A7[0]=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"'",sizeof(char),2),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4A7,sizeof(struct _dyneither_ptr*),3)))))))));}{
# 831
unsigned long _tmpE2=(_tmpDA + _tmpDB)+ _tmpDC;
unsigned int _tmp4AD;unsigned int _tmp4AC;struct _dyneither_ptr _tmp4AB;char*_tmp4AA;unsigned int _tmp4A9;struct _dyneither_ptr s2=(_tmp4A9=_tmpE2 + 1,((_tmp4AA=(char*)_cycalloc_atomic(_check_times(sizeof(char),_tmp4A9 + 1)),((_tmp4AB=_tag_dyneither(_tmp4AA,sizeof(char),_tmp4A9 + 1),((((_tmp4AC=_tmp4A9,((_tmp4AE(& _tmp4AD,& _tmp4AC,& _tmp4AA),_tmp4AA[_tmp4AC]=(char)0)))),_tmp4AB)))))));
int _tmpE3=0;
int _tmpE4=0;
for(0;_tmpE3 < _tmpDA;++ _tmpE3){
char _tmpE5=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpE3));
if(_tmpE5 == '\''  || Cyc_is_other_special(_tmpE5)){
char _tmp4B1;char _tmp4B0;struct _dyneither_ptr _tmp4AF;(_tmp4AF=_dyneither_ptr_plus(s2,sizeof(char),_tmpE4 ++),((_tmp4B0=*((char*)_check_dyneither_subscript(_tmp4AF,sizeof(char),0)),((_tmp4B1='\\',((_get_dyneither_size(_tmp4AF,sizeof(char))== 1  && (_tmp4B0 == '\000'  && _tmp4B1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4AF.curr)=_tmp4B1)))))));}{
char _tmp4B4;char _tmp4B3;struct _dyneither_ptr _tmp4B2;(_tmp4B2=_dyneither_ptr_plus(s2,sizeof(char),_tmpE4 ++),((_tmp4B3=*((char*)_check_dyneither_subscript(_tmp4B2,sizeof(char),0)),((_tmp4B4=_tmpE5,((_get_dyneither_size(_tmp4B2,sizeof(char))== 1  && (_tmp4B3 == '\000'  && _tmp4B4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp4B2.curr)=_tmp4B4)))))));};}
# 841
return(struct _dyneither_ptr)s2;};}
# 843
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
struct _dyneither_ptr*_tmp4B5;return(_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5[0]=Cyc_sh_escape_string(*sp),_tmp4B5)));}
# 847
static void Cyc_process_file(struct _dyneither_ptr filename){
# 849
struct _dyneither_ptr _tmpF1=Cyc_make_base_filename(filename,Cyc_output_file);
const char*_tmp4B6;struct _dyneither_ptr _tmpF2=Cyc_strconcat((struct _dyneither_ptr)_tmpF1,((_tmp4B6=".cyp",_tag_dyneither(_tmp4B6,sizeof(char),5))));
const char*_tmp4B7;struct _dyneither_ptr _tmpF3=(char*)Cyc_specified_interface.curr != (char*)(_tag_dyneither(0,0,0)).curr?Cyc_specified_interface:(struct _dyneither_ptr)
Cyc_strconcat((struct _dyneither_ptr)_tmpF1,((_tmp4B7=".cyci",_tag_dyneither(_tmp4B7,sizeof(char),6))));
const char*_tmp4B8;struct _dyneither_ptr _tmpF4=Cyc_strconcat((struct _dyneither_ptr)_tmpF1,((_tmp4B8=".cycio",_tag_dyneither(_tmp4B8,sizeof(char),7))));
const char*_tmp4B9;struct _dyneither_ptr _tmpF5=Cyc_strconcat((struct _dyneither_ptr)_tmpF1,((_tmp4B9=".c",_tag_dyneither(_tmp4B9,sizeof(char),3))));
# 856
if(Cyc_v_r){const char*_tmp4BD;void*_tmp4BC[1];struct Cyc_String_pa_PrintArg_struct _tmp4BB;(_tmp4BB.tag=0,((_tmp4BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename),((_tmp4BC[0]=& _tmp4BB,Cyc_fprintf(Cyc_stderr,((_tmp4BD="Compiling %s\n",_tag_dyneither(_tmp4BD,sizeof(char),14))),_tag_dyneither(_tmp4BC,sizeof(void*),1)))))));}{
# 859
const char*_tmp4BF;const char*_tmp4BE;struct Cyc___cycFILE*f0=Cyc_try_file_open(filename,((_tmp4BE="r",_tag_dyneither(_tmp4BE,sizeof(char),2))),((_tmp4BF="input file",_tag_dyneither(_tmp4BF,sizeof(char),11))));
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 866
const char*_tmp4C6;struct _dyneither_ptr*_tmp4C5;const char*_tmp4C4;struct Cyc_List_List*_tmp4C3;struct _dyneither_ptr _tmpF9=
Cyc_str_sepstr(((_tmp4C3=_cycalloc(sizeof(*_tmp4C3)),((_tmp4C3->hd=((_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5[0]=(struct _dyneither_ptr)((_tmp4C4="",_tag_dyneither(_tmp4C4,sizeof(char),1))),_tmp4C5)))),((_tmp4C3->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs)),_tmp4C3)))))),(
(_tmp4C6=" ",_tag_dyneither(_tmp4C6,sizeof(char),2))));
# 876
struct _dyneither_ptr def_inc_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_inc_path);
struct Cyc_List_List*_tmpFA=Cyc_add_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);
{const char*_tmp4C7;_tmpFA=Cyc_add_subdir(_tmpFA,((_tmp4C7="include",_tag_dyneither(_tmp4C7,sizeof(char),8))));}
if(Cyc_strlen((struct _dyneither_ptr)def_inc_path)> 0){
struct _dyneither_ptr*_tmp4CA;struct Cyc_List_List*_tmp4C9;_tmpFA=((_tmp4C9=_cycalloc(sizeof(*_tmp4C9)),((_tmp4C9->hd=((_tmp4CA=_cycalloc(sizeof(*_tmp4CA)),((_tmp4CA[0]=def_inc_path,_tmp4CA)))),((_tmp4C9->tl=_tmpFA,_tmp4C9))))));}{
# 882
char*_tmpFE=getenv("CYCLONE_INCLUDE_PATH");
if(_tmpFE != 0){
char*_tmp4CB;_tmpFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_split_by_char(((_tmp4CB=_tmpFE,_tag_dyneither(_tmp4CB,sizeof(char),_get_zero_arr_size_char((void*)_tmp4CB,1)))),':'),_tmpFA);}{
# 887
const char*_tmp4D2;struct _dyneither_ptr*_tmp4D1;const char*_tmp4D0;struct Cyc_List_List*_tmp4CF;struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)
Cyc_str_sepstr(((_tmp4CF=_cycalloc(sizeof(*_tmp4CF)),((_tmp4CF->hd=((_tmp4D1=_cycalloc(sizeof(*_tmp4D1)),((_tmp4D1[0]=(struct _dyneither_ptr)((_tmp4D0="",_tag_dyneither(_tmp4D0,sizeof(char),1))),_tmp4D1)))),((_tmp4CF->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpFA),_tmp4CF)))))),(
(_tmp4D2=" -I",_tag_dyneither(_tmp4D2,sizeof(char),4))));
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0){
const char*_tmp4D6;void*_tmp4D5[1];struct Cyc_String_pa_PrintArg_struct _tmp4D4;ofile_string=(struct _dyneither_ptr)((_tmp4D4.tag=0,((_tmp4D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file))),((_tmp4D5[0]=& _tmp4D4,Cyc_aprintf(((_tmp4D6=" > %s",_tag_dyneither(_tmp4D6,sizeof(char),6))),_tag_dyneither(_tmp4D5,sizeof(void*),1))))))));}else{
# 896
const char*_tmp4D7;ofile_string=((_tmp4D7="",_tag_dyneither(_tmp4D7,sizeof(char),1)));}}else{
# 898
const char*_tmp4DB;void*_tmp4DA[1];struct Cyc_String_pa_PrintArg_struct _tmp4D9;ofile_string=(struct _dyneither_ptr)((_tmp4D9.tag=0,((_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpF2)),((_tmp4DA[0]=& _tmp4D9,Cyc_aprintf(((_tmp4DB=" > %s",_tag_dyneither(_tmp4DB,sizeof(char),6))),_tag_dyneither(_tmp4DA,sizeof(void*),1))))))));}{
# 900
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 904
if(Cyc_dependencies_target == 0){
# 908
const char*_tmp4DC;fixup_string=((_tmp4DC=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'",_tag_dyneither(_tmp4DC,sizeof(char),35)));}else{
# 913
const char*_tmp4E0;void*_tmp4DF[1];struct Cyc_String_pa_PrintArg_struct _tmp4DE;fixup_string=(struct _dyneither_ptr)(
(_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target))),((_tmp4DF[0]=& _tmp4DE,Cyc_aprintf(((_tmp4E0=" | sed 's/^.*\\.cyc\\.o:/%s:/'",_tag_dyneither(_tmp4E0,sizeof(char),29))),_tag_dyneither(_tmp4DF,sizeof(void*),1))))))));}}else{
# 917
const char*_tmp4E1;fixup_string=((_tmp4E1="",_tag_dyneither(_tmp4E1,sizeof(char),1)));}{
# 919
const char*_tmp4EA;void*_tmp4E9[6];struct Cyc_String_pa_PrintArg_struct _tmp4E8;struct Cyc_String_pa_PrintArg_struct _tmp4E7;struct Cyc_String_pa_PrintArg_struct _tmp4E6;struct Cyc_String_pa_PrintArg_struct _tmp4E5;struct Cyc_String_pa_PrintArg_struct _tmp4E4;struct Cyc_String_pa_PrintArg_struct _tmp4E3;struct _dyneither_ptr _tmp10C=(_tmp4E3.tag=0,((_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string),((_tmp4E4.tag=0,((_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string),((_tmp4E5.tag=0,((_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 922
Cyc_sh_escape_string(filename)),((_tmp4E6.tag=0,((_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string),((_tmp4E7.tag=0,((_tmp4E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF9),((_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp),((_tmp4E9[0]=& _tmp4E8,((_tmp4E9[1]=& _tmp4E7,((_tmp4E9[2]=& _tmp4E6,((_tmp4E9[3]=& _tmp4E5,((_tmp4E9[4]=& _tmp4E4,((_tmp4E9[5]=& _tmp4E3,Cyc_aprintf(((_tmp4EA="%s %s%s %s%s%s",_tag_dyneither(_tmp4EA,sizeof(char),15))),_tag_dyneither(_tmp4E9,sizeof(void*),6)))))))))))))))))))))))))))))))))))));
# 925
if(Cyc_v_r){const char*_tmp4EE;void*_tmp4ED[1];struct Cyc_String_pa_PrintArg_struct _tmp4EC;(_tmp4EC.tag=0,((_tmp4EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp10C),((_tmp4ED[0]=& _tmp4EC,Cyc_fprintf(Cyc_stderr,((_tmp4EE="%s\n",_tag_dyneither(_tmp4EE,sizeof(char),4))),_tag_dyneither(_tmp4ED,sizeof(void*),1)))))));}
if(system((const char*)_untag_dyneither_ptr(_tmp10C,sizeof(char),1))!= 0){
Cyc_compile_failure=1;
{const char*_tmp4F1;void*_tmp4F0;(_tmp4F0=0,Cyc_fprintf(Cyc_stderr,((_tmp4F1="\nError: preprocessing\n",_tag_dyneither(_tmp4F1,sizeof(char),23))),_tag_dyneither(_tmp4F0,sizeof(void*),0)));}
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
return;}
# 932
if(Cyc_stop_after_cpp_r)return;
# 935
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpF2);{
const char*_tmp4F3;const char*_tmp4F2;struct Cyc___cycFILE*in_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF2,((_tmp4F2="r",_tag_dyneither(_tmp4F2,sizeof(char),2))),((_tmp4F3="file",_tag_dyneither(_tmp4F3,sizeof(char),5))));
if(Cyc_compile_failure)return;{
# 939
struct Cyc_List_List*tds=0;
# 942
{struct _handler_cons _tmp112;_push_handler(& _tmp112);{int _tmp114=0;if(setjmp(_tmp112.handler))_tmp114=1;if(!_tmp114){
{const char*_tmp4F4;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F4="parsing",_tag_dyneither(_tmp4F4,sizeof(char),8))),Cyc_do_parse,(struct Cyc___cycFILE*)_check_null(in_file),tds);};_pop_handler();}else{void*_tmp113=(void*)_exn_thrown;void*_tmp117=_tmp113;void*_tmp118;void*_tmp119;_LL25: _tmp118=_tmp117;_LL26:
# 946
 Cyc_file_close((struct Cyc___cycFILE*)_check_null(in_file));
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
Cyc_Core_rethrow(_tmp118);_LL27: _tmp119=_tmp117;_LL28:(void)_throw(_tmp119);_LL24:;}};}
# 950
Cyc_file_close(in_file);
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
return;}
# 956
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}{
# 961
struct _RegionHandle _tmp11A=_new_region("tc_rgn");struct _RegionHandle*tc_rgn=& _tmp11A;_push_region(tc_rgn);{
struct Cyc_Tcenv_Tenv*_tmp11B=Cyc_Tcenv_tc_init(tc_rgn);
if(Cyc_parseonly_r)goto PRINTC;{
# 966
struct _tuple22 _tmp4F5;struct _tuple22 _tmp11C=(_tmp4F5.f1=tc_rgn,((_tmp4F5.f2=_tmp11B,_tmp4F5)));
{const char*_tmp4F6;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F6="type checking",_tag_dyneither(_tmp4F6,sizeof(char),14))),Cyc_do_typecheck,& _tmp11C,tds);}
# 969
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
_npop_handler(0);return;}
# 973
if(Cyc_tc_r)goto PRINTC;
{const char*_tmp4F7;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4F7="control-flow checking",_tag_dyneither(_tmp4F7,sizeof(char),22))),Cyc_do_cfcheck,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
_npop_handler(0);return;}
# 981
if(Cyc_generate_interface_r){
const char*_tmp4FA;const char*_tmp4F9;const char*_tmp4F8;struct Cyc___cycFILE*inter_file=Cyc_try_file_open(((_tmp4F8=(const char*)_untag_dyneither_ptr(_tmpF3,sizeof(char),1),_tag_dyneither(_tmp4F8,sizeof(char),_get_zero_arr_size_char((void*)_tmp4F8,1)))),((_tmp4F9="w",_tag_dyneither(_tmp4F9,sizeof(char),2))),((_tmp4FA="interface file",_tag_dyneither(_tmp4FA,sizeof(char),15))));
if(inter_file == 0){
Cyc_compile_failure=1;_npop_handler(0);return;}
# 986
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 993
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_dyneither_ptr(_tmpF3,sizeof(char),1),"r");
const char*_tmp4FC;const char*_tmp4FB;struct Cyc___cycFILE*inter_objfile=Cyc_try_file_open((struct _dyneither_ptr)_tmpF4,((_tmp4FB="w",_tag_dyneither(_tmp4FB,sizeof(char),2))),((_tmp4FC="interface object file",_tag_dyneither(_tmp4FC,sizeof(char),22))));
if(inter_objfile == 0){
Cyc_compile_failure=1;
_npop_handler(0);return;}
# 1000
Cyc_Position_reset_position(_tmpF3);{
struct _tuple23 _tmp4FD;struct _tuple23 _tmp122=(_tmp4FD.f1=_tmp11B,((_tmp4FD.f2=inter_file,((_tmp4FD.f3=inter_objfile,_tmp4FD)))));
{const char*_tmp4FE;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FE="interface checking",_tag_dyneither(_tmp4FE,sizeof(char),19))),Cyc_do_interface,& _tmp122,tds);}
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);};}};}
# 1008
if(Cyc_cf_r)goto PRINTC;
# 1011
{const char*_tmp4FF;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp4FF="translation to C",_tag_dyneither(_tmp4FF,sizeof(char),17))),Cyc_do_translate,1,tds);}
if(Cyc_compile_failure){
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
_npop_handler(0);return;}
# 1017
if(!Cyc_compile_failure)
Cyc_Tcutil_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpF2);
if(Cyc_compile_failure){_npop_handler(0);return;}
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r){
# 1025
const char*_tmp500;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp500="post-pass to VC",_tag_dyneither(_tmp500,sizeof(char),16))),Cyc_do_tovc,1,tds);}
if(Cyc_compile_failure){_npop_handler(0);return;}
# 1032
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0){
const char*_tmp502;const char*_tmp501;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp501="w",_tag_dyneither(_tmp501,sizeof(char),2))),((_tmp502="output file",_tag_dyneither(_tmp502,sizeof(char),12))));}else{
# 1038
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0){
const char*_tmp504;const char*_tmp503;out_file=Cyc_try_file_open(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmp503="w",_tag_dyneither(_tmp503,sizeof(char),2))),((_tmp504="output file",_tag_dyneither(_tmp504,sizeof(char),12))));}else{
# 1042
const char*_tmp506;const char*_tmp505;out_file=Cyc_try_file_open((struct _dyneither_ptr)_tmpF5,((_tmp505="w",_tag_dyneither(_tmp505,sizeof(char),2))),((_tmp506="output file",_tag_dyneither(_tmp506,sizeof(char),12))));}}
# 1044
if(Cyc_compile_failure  || !((unsigned int)out_file)){_npop_handler(0);return;}
# 1046
{struct _handler_cons _tmp130;_push_handler(& _tmp130);{int _tmp132=0;if(setjmp(_tmp130.handler))_tmp132=1;if(!_tmp132){
if(!Cyc_noprint_r){
const char*_tmp507;tds=((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,struct Cyc_List_List*tds))Cyc_do_stage)(((_tmp507="printing",_tag_dyneither(_tmp507,sizeof(char),9))),Cyc_do_print,out_file,tds);}
# 1047
;_pop_handler();}else{void*_tmp131=(void*)_exn_thrown;void*_tmp135=_tmp131;void*_tmp136;void*_tmp137;_LL2A: _tmp136=_tmp135;_LL2B:
# 1051
 Cyc_compile_failure=1;
Cyc_file_close(out_file);
{struct _dyneither_ptr*_tmp50A;struct Cyc_List_List*_tmp509;Cyc_cfiles=((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509->hd=((_tmp50A=_cycalloc(sizeof(*_tmp50A)),((_tmp50A[0]=(struct _dyneither_ptr)_tmpF5,_tmp50A)))),((_tmp509->tl=Cyc_cfiles,_tmp509))))));}
Cyc_Core_rethrow(_tmp136);_LL2C: _tmp137=_tmp135;_LL2D:(void)_throw(_tmp137);_LL29:;}};}
# 1056
Cyc_file_close(out_file);{
struct _dyneither_ptr*_tmp50D;struct Cyc_List_List*_tmp50C;Cyc_cfiles=((_tmp50C=_cycalloc(sizeof(*_tmp50C)),((_tmp50C->hd=((_tmp50D=_cycalloc(sizeof(*_tmp50D)),((_tmp50D[0]=(struct _dyneither_ptr)_tmpF5,_tmp50D)))),((_tmp50C->tl=Cyc_cfiles,_tmp50C))))));};}
# 961
;_pop_region(tc_rgn);};};};};};};};};};}static char _tmp154[8]="<final>";
# 1061
static struct _dyneither_ptr Cyc_final_str={_tmp154,_tmp154,_tmp154 + 8};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1064
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1068
struct _dyneither_ptr basename;
{struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){
basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp159=_tmp156;void*_tmp15B;_LL2F: {struct Cyc_Core_Invalid_argument_exn_struct*_tmp15A=(struct Cyc_Core_Invalid_argument_exn_struct*)_tmp159;if(_tmp15A->tag != Cyc_Core_Invalid_argument)goto _LL31;}_LL30:
# 1072
 basename=*n;goto _LL2E;_LL31: _tmp15B=_tmp159;_LL32:(void)_throw(_tmp15B);_LL2E:;}};}{
# 1074
const char*_tmp50E;struct _dyneither_ptr _tmp15C=Cyc_strconcat((struct _dyneither_ptr)basename,((_tmp50E=".cycio",_tag_dyneither(_tmp50E,sizeof(char),7))));
const char*_tmp510;const char*_tmp50F;struct Cyc___cycFILE*_tmp15D=Cyc_try_file_open((struct _dyneither_ptr)_tmp15C,((_tmp50F="rb",_tag_dyneither(_tmp50F,sizeof(char),3))),((_tmp510="interface object file",_tag_dyneither(_tmp510,sizeof(char),22))));
if(_tmp15D == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1081
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp15C);{
# 1083
struct Cyc_Interface_I*_tmp15E=Cyc_Interface_load(_tmp15D);
Cyc_file_close(_tmp15D);
return _tmp15E;};};};}
# 1088
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 1092
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};int Cyc_main(int argc,struct _dyneither_ptr argv);static void _tmp516(unsigned int*_tmp515,unsigned int*_tmp514,int**_tmp512){for(*_tmp515=0;*_tmp515 < *_tmp514;(*_tmp515)++){(*_tmp512)[*_tmp515]=(int)0;}}static void _tmp51E(unsigned int*_tmp51D,unsigned int*_tmp51C,struct _dyneither_ptr**_tmp51A){for(*_tmp51D=0;*_tmp51D < *_tmp51C;(*_tmp51D)++){(*_tmp51A)[*_tmp51D]=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1129
_tag_dyneither(0,0,0));}}static void _tmp524(unsigned int*_tmp523,unsigned int*_tmp522,struct _dyneither_ptr**_tmp520){for(*_tmp523=0;*_tmp523 < *_tmp522;(*_tmp523)++){(*_tmp520)[*_tmp523]=(struct _dyneither_ptr)((struct _dyneither_ptr)
_tag_dyneither(0,0,0));}}
# 1095
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1100
GC_blacklist_warn_clear();{
# 1102
struct Cyc_List_List*cyclone_arch_path;
struct _dyneither_ptr def_lib_path;
struct _dyneither_ptr comp=(struct _dyneither_ptr)Cstring_to_string(Ccomp);
# 1107
struct _RegionHandle _tmp162=_new_region("r");struct _RegionHandle*r=& _tmp162;_push_region(r);{
# 1109
unsigned int _tmp515;unsigned int _tmp514;struct _dyneither_ptr _tmp513;int*_tmp512;unsigned int _tmp511;struct _dyneither_ptr _tmp163=(_tmp511=(unsigned int)argc,((_tmp512=(int*)_cycalloc_atomic(_check_times(sizeof(int),_tmp511)),((_tmp513=_tag_dyneither(_tmp512,sizeof(int),_tmp511),((((_tmp514=_tmp511,_tmp516(& _tmp515,& _tmp514,& _tmp512))),_tmp513)))))));
int _tmp164=0;
int i;int j;int k;
for(i=1;i < argc;++ i){
const char*_tmp517;if(Cyc_strncmp(((_tmp517="-B",_tag_dyneither(_tmp517,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)),2)== 0){
*((int*)_check_dyneither_subscript(_tmp163,sizeof(int),i))=1;
++ _tmp164;}else{
# 1117
const char*_tmp518;if(Cyc_strcmp(((_tmp518="-b",_tag_dyneither(_tmp518,sizeof(char),3))),(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i)))== 0){
*((int*)_check_dyneither_subscript(_tmp163,sizeof(int),i))=1;
++ _tmp164;
++ i;if(i >= argc)break;
*((int*)_check_dyneither_subscript(_tmp163,sizeof(int),i))=1;
++ _tmp164;}}}{
# 1129
unsigned int _tmp51D;unsigned int _tmp51C;struct _dyneither_ptr _tmp51B;struct _dyneither_ptr*_tmp51A;unsigned int _tmp519;struct _dyneither_ptr _tmp167=(_tmp519=(unsigned int)(_tmp164 + 1),((_tmp51A=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp519)),((_tmp51B=_tag_dyneither(_tmp51A,sizeof(struct _dyneither_ptr),_tmp519),((((_tmp51C=_tmp519,_tmp51E(& _tmp51D,& _tmp51C,& _tmp51A))),_tmp51B)))))));
unsigned int _tmp523;unsigned int _tmp522;struct _dyneither_ptr _tmp521;struct _dyneither_ptr*_tmp520;unsigned int _tmp51F;struct _dyneither_ptr _tmp168=(_tmp51F=(unsigned int)(argc - _tmp164),((_tmp520=(struct _dyneither_ptr*)_cycalloc(_check_times(sizeof(struct _dyneither_ptr),_tmp51F)),((_tmp521=_tag_dyneither(_tmp520,sizeof(struct _dyneither_ptr),_tmp51F),((((_tmp522=_tmp51F,_tmp524(& _tmp523,& _tmp522,& _tmp520))),_tmp521)))))));
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),0))=(*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp168,sizeof(struct _dyneither_ptr),0))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),0)));
for(i=(j=(k=1));i < argc;++ i){
if(*((int*)_check_dyneither_subscript(_tmp163,sizeof(int),i)))*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp167,sizeof(struct _dyneither_ptr),j ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));else{
*((struct _dyneither_ptr*)_check_dyneither_subscript(_tmp168,sizeof(struct _dyneither_ptr),k ++))=*((struct _dyneither_ptr*)_check_dyneither_subscript(argv,sizeof(struct _dyneither_ptr),i));}}{
# 1138
struct _tuple24*_tmpAB3;const char*_tmpAB2;const char*_tmpAB1;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpAB0;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpAAF;const char*_tmpAAE;struct _tuple24*_tmpAAD;const char*_tmpAAC;const char*_tmpAAB;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpAAA;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpAA9;const char*_tmpAA8;struct _tuple24*_tmpAA7;const char*_tmpAA6;const char*_tmpAA5;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpAA4;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpAA3;const char*_tmpAA2;struct _tuple24*_tmpAA1;const char*_tmpAA0;const char*_tmpA9F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA9E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA9D;const char*_tmpA9C;struct _tuple24*_tmpA9B;const char*_tmpA9A;const char*_tmpA99;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA98;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA97;const char*_tmpA96;struct _tuple24*_tmpA95;const char*_tmpA94;const char*_tmpA93;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA92;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA91;const char*_tmpA90;struct _tuple24*_tmpA8F;const char*_tmpA8E;const char*_tmpA8D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA8C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA8B;const char*_tmpA8A;struct _tuple24*_tmpA89;const char*_tmpA88;const char*_tmpA87;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA86;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA85;const char*_tmpA84;struct _tuple24*_tmpA83;const char*_tmpA82;const char*_tmpA81;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA80;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA7F;const char*_tmpA7E;struct _tuple24*_tmpA7D;const char*_tmpA7C;const char*_tmpA7B;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA7A;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA79;const char*_tmpA78;struct _tuple24*_tmpA77;const char*_tmpA76;const char*_tmpA75;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA74;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA73;const char*_tmpA72;struct _tuple24*_tmpA71;const char*_tmpA70;const char*_tmpA6F;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA6E;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA6D;const char*_tmpA6C;struct _tuple24*_tmpA6B;const char*_tmpA6A;const char*_tmpA69;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA68;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA67;const char*_tmpA66;struct _tuple24*_tmpA65;const char*_tmpA64;const char*_tmpA63;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA62;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA61;const char*_tmpA60;struct _tuple24*_tmpA5F;const char*_tmpA5E;const char*_tmpA5D;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA5C;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA5B;const char*_tmpA5A;struct _tuple24*_tmpA59;const char*_tmpA58;const char*_tmpA57;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA56;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA55;const char*_tmpA54;struct _tuple24*_tmpA53;const char*_tmpA52;const char*_tmpA51;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA50;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA4F;const char*_tmpA4E;struct _tuple24*_tmpA4D;const char*_tmpA4C;const char*_tmpA4B;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA4A;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA49;const char*_tmpA48;struct _tuple24*_tmpA47;const char*_tmpA46;const char*_tmpA45;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA44;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA43;const char*_tmpA42;struct _tuple24*_tmpA41;const char*_tmpA40;const char*_tmpA3F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA3E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA3D;const char*_tmpA3C;struct _tuple24*_tmpA3B;const char*_tmpA3A;const char*_tmpA39;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA38;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA37;const char*_tmpA36;struct _tuple24*_tmpA35;const char*_tmpA34;const char*_tmpA33;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA32;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA31;const char*_tmpA30;struct _tuple24*_tmpA2F;const char*_tmpA2E;const char*_tmpA2D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA2C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA2B;const char*_tmpA2A;struct _tuple24*_tmpA29;const char*_tmpA28;const char*_tmpA27;struct Cyc_Arg_Flag_spec_Arg_Spec_struct _tmpA26;struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmpA25;const char*_tmpA24;struct _tuple24*_tmpA23;const char*_tmpA22;const char*_tmpA21;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA20;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA1F;const char*_tmpA1E;struct _tuple24*_tmpA1D;const char*_tmpA1C;const char*_tmpA1B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmpA1A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmpA19;const char*_tmpA18;struct _tuple24*_tmpA17;const char*_tmpA16;const char*_tmpA15;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA14;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA13;const char*_tmpA12;struct _tuple24*_tmpA11;const char*_tmpA10;const char*_tmpA0F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA0E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA0D;const char*_tmpA0C;struct _tuple24*_tmpA0B;const char*_tmpA0A;const char*_tmpA09;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmpA08;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmpA07;const char*_tmpA06;struct _tuple24*_tmpA05;const char*_tmpA04;const char*_tmpA03;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmpA02;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmpA01;const char*_tmpA00;struct _tuple24*_tmp9FF;const char*_tmp9FE;const char*_tmp9FD;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9FC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9FB;const char*_tmp9FA;struct _tuple24*_tmp9F9;const char*_tmp9F8;const char*_tmp9F7;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F6;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9F5;const char*_tmp9F4;struct _tuple24*_tmp9F3;const char*_tmp9F2;const char*_tmp9F1;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9F0;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9EF;const char*_tmp9EE;struct _tuple24*_tmp9ED;const char*_tmp9EC;const char*_tmp9EB;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9EA;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E9;const char*_tmp9E8;struct _tuple24*_tmp9E7;const char*_tmp9E6;const char*_tmp9E5;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9E4;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9E3;const char*_tmp9E2;struct _tuple24*_tmp9E1;const char*_tmp9E0;const char*_tmp9DF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9DE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9DD;const char*_tmp9DC;struct _tuple24*_tmp9DB;const char*_tmp9DA;const char*_tmp9D9;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9D8;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9D7;const char*_tmp9D6;struct _tuple24*_tmp9D5;const char*_tmp9D4;const char*_tmp9D3;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9D2;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9D1;const char*_tmp9D0;struct _tuple24*_tmp9CF;const char*_tmp9CE;const char*_tmp9CD;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9CC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9CB;const char*_tmp9CA;struct _tuple24*_tmp9C9;const char*_tmp9C8;const char*_tmp9C7;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp9C6;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp9C5;const char*_tmp9C4;struct _tuple24*_tmp9C3;const char*_tmp9C2;const char*_tmp9C1;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9C0;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9BF;const char*_tmp9BE;struct _tuple24*_tmp9BD;const char*_tmp9BC;const char*_tmp9BB;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp9BA;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp9B9;const char*_tmp9B8;struct _tuple24*_tmp9B7;const char*_tmp9B6;const char*_tmp9B5;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9B4;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9B3;const char*_tmp9B2;struct _tuple24*_tmp9B1;const char*_tmp9B0;const char*_tmp9AF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp9AE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp9AD;const char*_tmp9AC;struct _tuple24*_tmp9AB;const char*_tmp9AA;const char*_tmp9A9;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A8;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A7;const char*_tmp9A6;struct _tuple24*_tmp9A5;const char*_tmp9A4;const char*_tmp9A3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp9A2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp9A1;const char*_tmp9A0;struct _tuple24*_tmp99F;const char*_tmp99E;const char*_tmp99D;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp99C;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp99B;const char*_tmp99A;struct _tuple24*_tmp999;const char*_tmp998;const char*_tmp997;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp996;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp995;const char*_tmp994;struct _tuple24*_tmp993;const char*_tmp992;const char*_tmp991;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp990;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp98F;const char*_tmp98E;struct _tuple24*_tmp98D;const char*_tmp98C;const char*_tmp98B;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp98A;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp989;const char*_tmp988;struct _tuple24*_tmp987;const char*_tmp986;const char*_tmp985;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp984;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp983;const char*_tmp982;struct _tuple24*_tmp981;const char*_tmp980;const char*_tmp97F;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp97E;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp97D;const char*_tmp97C;struct _tuple24*_tmp97B;const char*_tmp97A;const char*_tmp979;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp978;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp977;const char*_tmp976;struct _tuple24*_tmp975;const char*_tmp974;const char*_tmp973;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp972;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp971;const char*_tmp970;struct _tuple24*_tmp96F;const char*_tmp96E;const char*_tmp96D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp96C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp96B;const char*_tmp96A;struct _tuple24*_tmp969;const char*_tmp968;const char*_tmp967;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp966;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp965;const char*_tmp964;struct _tuple24*_tmp963;const char*_tmp962;const char*_tmp961;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp960;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp95F;const char*_tmp95E;struct _tuple24*_tmp95D;const char*_tmp95C;const char*_tmp95B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp95A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp959;const char*_tmp958;struct _tuple24*_tmp957;const char*_tmp956;const char*_tmp955;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp954;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp953;const char*_tmp952;struct _tuple24*_tmp951;const char*_tmp950;const char*_tmp94F;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp94E;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp94D;const char*_tmp94C;struct _tuple24*_tmp94B;const char*_tmp94A;const char*_tmp949;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp948;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp947;const char*_tmp946;struct _tuple24*_tmp945;const char*_tmp944;const char*_tmp943;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp942;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp941;const char*_tmp940;struct _tuple24*_tmp93F;const char*_tmp93E;const char*_tmp93D;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp93C;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp93B;const char*_tmp93A;struct _tuple24*_tmp939;const char*_tmp938;const char*_tmp937;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp936;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp935;const char*_tmp934;struct _tuple24*_tmp933;const char*_tmp932;const char*_tmp931;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp930;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp92F;const char*_tmp92E;struct _tuple24*_tmp92D;const char*_tmp92C;const char*_tmp92B;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp92A;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp929;const char*_tmp928;struct _tuple24*_tmp927;const char*_tmp926;const char*_tmp925;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp924;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp923;const char*_tmp922;struct _tuple24*_tmp921;const char*_tmp920;const char*_tmp91F;struct Cyc_Arg_Clear_spec_Arg_Spec_struct _tmp91E;struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_tmp91D;const char*_tmp91C;struct _tuple24*_tmp91B;const char*_tmp91A;const char*_tmp919;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp918;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp917;const char*_tmp916;struct _tuple24*_tmp915;const char*_tmp914;const char*_tmp913;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp912;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp911;const char*_tmp910;struct _tuple24*_tmp90F;const char*_tmp90E;const char*_tmp90D;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp90C;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp90B;const char*_tmp90A;struct _tuple24*_tmp909;const char*_tmp908;const char*_tmp907;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp906;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp905;const char*_tmp904;struct _tuple24*_tmp903;const char*_tmp902;const char*_tmp901;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp900;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8FF;const char*_tmp8FE;struct _tuple24*_tmp8FD;const char*_tmp8FC;const char*_tmp8FB;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8FA;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F9;const char*_tmp8F8;struct _tuple24*_tmp8F7;const char*_tmp8F6;const char*_tmp8F5;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8F4;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8F3;const char*_tmp8F2;struct _tuple24*_tmp8F1;const char*_tmp8F0;const char*_tmp8EF;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8EE;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8ED;const char*_tmp8EC;struct _tuple24*_tmp8EB;const char*_tmp8EA;const char*_tmp8E9;struct Cyc_Arg_String_spec_Arg_Spec_struct _tmp8E8;struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp8E7;const char*_tmp8E6;struct _tuple24*_tmp8E5;const char*_tmp8E4;const char*_tmp8E3;struct Cyc_Arg_Unit_spec_Arg_Spec_struct _tmp8E2;struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp8E1;const char*_tmp8E0;struct _tuple24*_tmp8DF;const char*_tmp8DE;const char*_tmp8DD;struct Cyc_Arg_Set_spec_Arg_Spec_struct _tmp8DC;struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp8DB;const char*_tmp8DA;struct _tuple24*_tmp8D9[79];struct Cyc_List_List*options=
(_tmp8D9[78]=(
# 1376
(_tmp8DF=_region_malloc(r,sizeof(*_tmp8DF)),((_tmp8DF->f1=((_tmp8DE="--parsestate",_tag_dyneither(_tmp8DE,sizeof(char),13))),((_tmp8DF->f2=0,((_tmp8DF->f3=((_tmp8DD="",_tag_dyneither(_tmp8DD,sizeof(char),1))),((_tmp8DF->f4=(void*)(
(_tmp8DB=_region_malloc(r,sizeof(*_tmp8DB)),((_tmp8DB[0]=((_tmp8DC.tag=3,((_tmp8DC.f1=& Cyc_ParseErrors_print_state_and_token,_tmp8DC)))),_tmp8DB)))),((_tmp8DF->f5=(
(_tmp8DA="print the parse state and token on a syntax error",_tag_dyneither(_tmp8DA,sizeof(char),50))),_tmp8DF)))))))))))),((_tmp8D9[77]=(
# 1373
(_tmp8E5=_region_malloc(r,sizeof(*_tmp8E5)),((_tmp8E5->f1=((_tmp8E4="--manyerrors",_tag_dyneither(_tmp8E4,sizeof(char),13))),((_tmp8E5->f2=0,((_tmp8E5->f3=((_tmp8E3="",_tag_dyneither(_tmp8E3,sizeof(char),1))),((_tmp8E5->f4=(void*)(
(_tmp8E1=_region_malloc(r,sizeof(*_tmp8E1)),((_tmp8E1[0]=((_tmp8E2.tag=0,((_tmp8E2.f1=Cyc_set_many_errors,_tmp8E2)))),_tmp8E1)))),((_tmp8E5->f5=(
(_tmp8E0="don't stop after only a few errors.",_tag_dyneither(_tmp8E0,sizeof(char),36))),_tmp8E5)))))))))))),((_tmp8D9[76]=(
# 1370
(_tmp8EB=_region_malloc(r,sizeof(*_tmp8EB)),((_tmp8EB->f1=((_tmp8EA="--interface",_tag_dyneither(_tmp8EA,sizeof(char),12))),((_tmp8EB->f2=0,((_tmp8EB->f3=((_tmp8E9=" <file>",_tag_dyneither(_tmp8E9,sizeof(char),8))),((_tmp8EB->f4=(void*)(
(_tmp8E7=_region_malloc(r,sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=5,((_tmp8E8.f1=Cyc_set_specified_interface,_tmp8E8)))),_tmp8E7)))),((_tmp8EB->f5=(
(_tmp8E6="Set the interface file to be <file>.",_tag_dyneither(_tmp8E6,sizeof(char),37))),_tmp8EB)))))))))))),((_tmp8D9[75]=(
# 1367
(_tmp8F1=_region_malloc(r,sizeof(*_tmp8F1)),((_tmp8F1->f1=((_tmp8F0="--geninterface",_tag_dyneither(_tmp8F0,sizeof(char),15))),((_tmp8F1->f2=0,((_tmp8F1->f3=((_tmp8EF="",_tag_dyneither(_tmp8EF,sizeof(char),1))),((_tmp8F1->f4=(void*)(
(_tmp8ED=_region_malloc(r,sizeof(*_tmp8ED)),((_tmp8ED[0]=((_tmp8EE.tag=3,((_tmp8EE.f1=& Cyc_generate_interface_r,_tmp8EE)))),_tmp8ED)))),((_tmp8F1->f5=(
(_tmp8EC="Generate interface files",_tag_dyneither(_tmp8EC,sizeof(char),25))),_tmp8F1)))))))))))),((_tmp8D9[74]=(
# 1364
(_tmp8F7=_region_malloc(r,sizeof(*_tmp8F7)),((_tmp8F7->f1=((_tmp8F6="-idirafter",_tag_dyneither(_tmp8F6,sizeof(char),11))),((_tmp8F7->f2=0,((_tmp8F7->f3=((_tmp8F5="<dir>",_tag_dyneither(_tmp8F5,sizeof(char),6))),((_tmp8F7->f4=(void*)(
(_tmp8F3=_region_malloc(r,sizeof(*_tmp8F3)),((_tmp8F3[0]=((_tmp8F4.tag=5,((_tmp8F4.f1=Cyc_add_idirafter,_tmp8F4)))),_tmp8F3)))),((_tmp8F7->f5=(
(_tmp8F2="Add the directory to the second include path.",_tag_dyneither(_tmp8F2,sizeof(char),46))),_tmp8F7)))))))))))),((_tmp8D9[73]=(
# 1361
(_tmp8FD=_region_malloc(r,sizeof(*_tmp8FD)),((_tmp8FD->f1=((_tmp8FC="-isystem",_tag_dyneither(_tmp8FC,sizeof(char),9))),((_tmp8FD->f2=0,((_tmp8FD->f3=((_tmp8FB="<dir>",_tag_dyneither(_tmp8FB,sizeof(char),6))),((_tmp8FD->f4=(void*)(
(_tmp8F9=_region_malloc(r,sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FA.tag=5,((_tmp8FA.f1=Cyc_add_isystem,_tmp8FA)))),_tmp8F9)))),((_tmp8FD->f5=(
(_tmp8F8="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.",_tag_dyneither(_tmp8F8,sizeof(char),90))),_tmp8FD)))))))))))),((_tmp8D9[72]=(
# 1358
(_tmp903=_region_malloc(r,sizeof(*_tmp903)),((_tmp903->f1=((_tmp902="-iwithprefixbefore",_tag_dyneither(_tmp902,sizeof(char),19))),((_tmp903->f2=0,((_tmp903->f3=((_tmp901="<dir>",_tag_dyneither(_tmp901,sizeof(char),6))),((_tmp903->f4=(void*)(
(_tmp8FF=_region_malloc(r,sizeof(*_tmp8FF)),((_tmp8FF[0]=((_tmp900.tag=5,((_tmp900.f1=Cyc_add_iwithprefixbefore,_tmp900)))),_tmp8FF)))),((_tmp903->f5=(
(_tmp8FE="Add <prefix>/<dir> to the main include path.",_tag_dyneither(_tmp8FE,sizeof(char),45))),_tmp903)))))))))))),((_tmp8D9[71]=(
# 1355
(_tmp909=_region_malloc(r,sizeof(*_tmp909)),((_tmp909->f1=((_tmp908="-iwithprefix",_tag_dyneither(_tmp908,sizeof(char),13))),((_tmp909->f2=0,((_tmp909->f3=((_tmp907="<dir>",_tag_dyneither(_tmp907,sizeof(char),6))),((_tmp909->f4=(void*)(
(_tmp905=_region_malloc(r,sizeof(*_tmp905)),((_tmp905[0]=((_tmp906.tag=5,((_tmp906.f1=Cyc_add_iwithprefix,_tmp906)))),_tmp905)))),((_tmp909->f5=(
(_tmp904="Add <prefix>/<dir> to the second include path.",_tag_dyneither(_tmp904,sizeof(char),47))),_tmp909)))))))))))),((_tmp8D9[70]=(
# 1352
(_tmp90F=_region_malloc(r,sizeof(*_tmp90F)),((_tmp90F->f1=((_tmp90E="-iprefix",_tag_dyneither(_tmp90E,sizeof(char),9))),((_tmp90F->f2=0,((_tmp90F->f3=((_tmp90D="<prefix>",_tag_dyneither(_tmp90D,sizeof(char),9))),((_tmp90F->f4=(void*)(
(_tmp90B=_region_malloc(r,sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90C.tag=5,((_tmp90C.f1=Cyc_add_iprefix,_tmp90C)))),_tmp90B)))),((_tmp90F->f5=(
(_tmp90A="Specify <prefix> as the prefix for subsequent -iwithprefix options",_tag_dyneither(_tmp90A,sizeof(char),67))),_tmp90F)))))))))))),((_tmp8D9[69]=(
# 1349
(_tmp915=_region_malloc(r,sizeof(*_tmp915)),((_tmp915->f1=((_tmp914="-noregions",_tag_dyneither(_tmp914,sizeof(char),11))),((_tmp915->f2=0,((_tmp915->f3=((_tmp913="",_tag_dyneither(_tmp913,sizeof(char),1))),((_tmp915->f4=(void*)(
(_tmp911=_region_malloc(r,sizeof(*_tmp911)),((_tmp911[0]=((_tmp912.tag=3,((_tmp912.f1=& Cyc_Absyn_no_regions,_tmp912)))),_tmp911)))),((_tmp915->f5=(
(_tmp910="Generate code that doesn't use regions",_tag_dyneither(_tmp910,sizeof(char),39))),_tmp915)))))))))))),((_tmp8D9[68]=(
# 1346
(_tmp91B=_region_malloc(r,sizeof(*_tmp91B)),((_tmp91B->f1=((_tmp91A="-port",_tag_dyneither(_tmp91A,sizeof(char),6))),((_tmp91B->f2=0,((_tmp91B->f3=((_tmp919="",_tag_dyneither(_tmp919,sizeof(char),1))),((_tmp91B->f4=(void*)(
(_tmp917=_region_malloc(r,sizeof(*_tmp917)),((_tmp917[0]=((_tmp918.tag=0,((_tmp918.f1=Cyc_set_port_c_code,_tmp918)))),_tmp917)))),((_tmp91B->f5=(
(_tmp916="Suggest how to port C code to Cyclone",_tag_dyneither(_tmp916,sizeof(char),38))),_tmp91B)))))))))))),((_tmp8D9[67]=(
# 1343
(_tmp921=_region_malloc(r,sizeof(*_tmp921)),((_tmp921->f1=((_tmp920="-detailedlocation",_tag_dyneither(_tmp920,sizeof(char),18))),((_tmp921->f2=0,((_tmp921->f3=((_tmp91F="",_tag_dyneither(_tmp91F,sizeof(char),1))),((_tmp921->f4=(void*)(
(_tmp91D=_region_malloc(r,sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91E.tag=4,((_tmp91E.f1=& Cyc_Position_use_gcc_style_location,_tmp91E)))),_tmp91D)))),((_tmp921->f5=(
(_tmp91C="Try to give more detailed location information for errors",_tag_dyneither(_tmp91C,sizeof(char),58))),_tmp921)))))))))))),((_tmp8D9[66]=(
# 1340
(_tmp927=_region_malloc(r,sizeof(*_tmp927)),((_tmp927->f1=((_tmp926="-noregister",_tag_dyneither(_tmp926,sizeof(char),12))),((_tmp927->f2=0,((_tmp927->f3=((_tmp925="",_tag_dyneither(_tmp925,sizeof(char),1))),((_tmp927->f4=(void*)(
(_tmp923=_region_malloc(r,sizeof(*_tmp923)),((_tmp923[0]=((_tmp924.tag=3,((_tmp924.f1=& Cyc_Parse_no_register,_tmp924)))),_tmp923)))),((_tmp927->f5=(
(_tmp922="Treat register storage class as public",_tag_dyneither(_tmp922,sizeof(char),39))),_tmp927)))))))))))),((_tmp8D9[65]=(
# 1337
(_tmp92D=_region_malloc(r,sizeof(*_tmp92D)),((_tmp92D->f1=((_tmp92C="-warnregioncoerce",_tag_dyneither(_tmp92C,sizeof(char),18))),((_tmp92D->f2=0,((_tmp92D->f3=((_tmp92B="",_tag_dyneither(_tmp92B,sizeof(char),1))),((_tmp92D->f4=(void*)(
(_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929[0]=((_tmp92A.tag=3,((_tmp92A.f1=& Cyc_Tcutil_warn_region_coerce,_tmp92A)))),_tmp929)))),((_tmp92D->f5=(
(_tmp928="Warn when any region coercion is inserted",_tag_dyneither(_tmp928,sizeof(char),42))),_tmp92D)))))))))))),((_tmp8D9[64]=(
# 1334
(_tmp933=_region_malloc(r,sizeof(*_tmp933)),((_tmp933->f1=((_tmp932="-warnaliascoercion",_tag_dyneither(_tmp932,sizeof(char),19))),((_tmp933->f2=0,((_tmp933->f3=((_tmp931="",_tag_dyneither(_tmp931,sizeof(char),1))),((_tmp933->f4=(void*)(
(_tmp92F=_region_malloc(r,sizeof(*_tmp92F)),((_tmp92F[0]=((_tmp930.tag=3,((_tmp930.f1=& Cyc_Tcutil_warn_alias_coerce,_tmp930)))),_tmp92F)))),((_tmp933->f5=(
(_tmp92E="Warn when any alias coercion is inserted",_tag_dyneither(_tmp92E,sizeof(char),41))),_tmp933)))))))))))),((_tmp8D9[63]=(
# 1331
(_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939->f1=((_tmp938="-warnnullchecks",_tag_dyneither(_tmp938,sizeof(char),16))),((_tmp939->f2=0,((_tmp939->f3=((_tmp937="",_tag_dyneither(_tmp937,sizeof(char),1))),((_tmp939->f4=(void*)(
(_tmp935=_region_malloc(r,sizeof(*_tmp935)),((_tmp935[0]=((_tmp936.tag=3,((_tmp936.f1=& Cyc_Toc_warn_all_null_deref,_tmp936)))),_tmp935)))),((_tmp939->f5=(
(_tmp934="Warn when any null check can't be eliminated",_tag_dyneither(_tmp934,sizeof(char),45))),_tmp939)))))))))))),((_tmp8D9[62]=(
# 1328
(_tmp93F=_region_malloc(r,sizeof(*_tmp93F)),((_tmp93F->f1=((_tmp93E="-warnboundschecks",_tag_dyneither(_tmp93E,sizeof(char),18))),((_tmp93F->f2=0,((_tmp93F->f3=((_tmp93D="",_tag_dyneither(_tmp93D,sizeof(char),1))),((_tmp93F->f4=(void*)(
(_tmp93B=_region_malloc(r,sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93C.tag=3,((_tmp93C.f1=& Cyc_Toc_warn_bounds_checks,_tmp93C)))),_tmp93B)))),((_tmp93F->f5=(
(_tmp93A="Warn when bounds checks can't be eliminated",_tag_dyneither(_tmp93A,sizeof(char),44))),_tmp93F)))))))))))),((_tmp8D9[61]=(
# 1325
(_tmp945=_region_malloc(r,sizeof(*_tmp945)),((_tmp945->f1=((_tmp944="-CI",_tag_dyneither(_tmp944,sizeof(char),4))),((_tmp945->f2=0,((_tmp945->f3=((_tmp943=" <file>",_tag_dyneither(_tmp943,sizeof(char),8))),((_tmp945->f4=(void*)(
(_tmp941=_region_malloc(r,sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=5,((_tmp942.f1=Cyc_set_cyc_include,_tmp942)))),_tmp941)))),((_tmp945->f5=(
(_tmp940="Set cyc_include.h to be <file>",_tag_dyneither(_tmp940,sizeof(char),31))),_tmp945)))))))))))),((_tmp8D9[60]=(
# 1322
(_tmp94B=_region_malloc(r,sizeof(*_tmp94B)),((_tmp94B->f1=((_tmp94A="-compile-for-boot",_tag_dyneither(_tmp94A,sizeof(char),18))),((_tmp94B->f2=0,((_tmp94B->f3=((_tmp949="",_tag_dyneither(_tmp949,sizeof(char),1))),((_tmp94B->f4=(void*)(
(_tmp947=_region_malloc(r,sizeof(*_tmp947)),((_tmp947[0]=((_tmp948.tag=3,((_tmp948.f1=& Cyc_Lex_compile_for_boot_r,_tmp948)))),_tmp947)))),((_tmp94B->f5=(
(_tmp946="Compile using the special boot library instead of the standard library",_tag_dyneither(_tmp946,sizeof(char),71))),_tmp94B)))))))))))),((_tmp8D9[59]=(
# 1319
(_tmp951=_region_malloc(r,sizeof(*_tmp951)),((_tmp951->f1=((_tmp950="-nocyc_setjmp",_tag_dyneither(_tmp950,sizeof(char),14))),((_tmp951->f2=0,((_tmp951->f3=((_tmp94F="",_tag_dyneither(_tmp94F,sizeof(char),1))),((_tmp951->f4=(void*)(
(_tmp94D=_region_malloc(r,sizeof(*_tmp94D)),((_tmp94D[0]=((_tmp94E.tag=3,((_tmp94E.f1=& Cyc_nocyc_setjmp_r,_tmp94E)))),_tmp94D)))),((_tmp951->f5=(
(_tmp94C="Do not use compiler special file cyc_setjmp.h",_tag_dyneither(_tmp94C,sizeof(char),46))),_tmp951)))))))))))),((_tmp8D9[58]=(
# 1316
(_tmp957=_region_malloc(r,sizeof(*_tmp957)),((_tmp957->f1=((_tmp956="-printalleffects",_tag_dyneither(_tmp956,sizeof(char),17))),((_tmp957->f2=0,((_tmp957->f3=((_tmp955="",_tag_dyneither(_tmp955,sizeof(char),1))),((_tmp957->f4=(void*)(
(_tmp953=_region_malloc(r,sizeof(*_tmp953)),((_tmp953[0]=((_tmp954.tag=3,((_tmp954.f1=& Cyc_print_all_effects_r,_tmp954)))),_tmp953)))),((_tmp957->f5=(
(_tmp952="Print effects for functions (type debugging)",_tag_dyneither(_tmp952,sizeof(char),45))),_tmp957)))))))))))),((_tmp8D9[57]=(
# 1313
(_tmp95D=_region_malloc(r,sizeof(*_tmp95D)),((_tmp95D->f1=((_tmp95C="-printfullevars",_tag_dyneither(_tmp95C,sizeof(char),16))),((_tmp95D->f2=0,((_tmp95D->f3=((_tmp95B="",_tag_dyneither(_tmp95B,sizeof(char),1))),((_tmp95D->f4=(void*)(
(_tmp959=_region_malloc(r,sizeof(*_tmp959)),((_tmp959[0]=((_tmp95A.tag=3,((_tmp95A.f1=& Cyc_print_full_evars_r,_tmp95A)))),_tmp959)))),((_tmp95D->f5=(
(_tmp958="Print full information for evars (type debugging)",_tag_dyneither(_tmp958,sizeof(char),50))),_tmp95D)))))))))))),((_tmp8D9[56]=(
# 1310
(_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963->f1=((_tmp962="-printallkinds",_tag_dyneither(_tmp962,sizeof(char),15))),((_tmp963->f2=0,((_tmp963->f3=((_tmp961="",_tag_dyneither(_tmp961,sizeof(char),1))),((_tmp963->f4=(void*)(
(_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F[0]=((_tmp960.tag=3,((_tmp960.f1=& Cyc_print_all_kinds_r,_tmp960)))),_tmp95F)))),((_tmp963->f5=(
(_tmp95E="Always print kinds of type variables",_tag_dyneither(_tmp95E,sizeof(char),37))),_tmp963)))))))))))),((_tmp8D9[55]=(
# 1307
(_tmp969=_region_malloc(r,sizeof(*_tmp969)),((_tmp969->f1=((_tmp968="-printalltvars",_tag_dyneither(_tmp968,sizeof(char),15))),((_tmp969->f2=0,((_tmp969->f3=((_tmp967="",_tag_dyneither(_tmp967,sizeof(char),1))),((_tmp969->f4=(void*)(
(_tmp965=_region_malloc(r,sizeof(*_tmp965)),((_tmp965[0]=((_tmp966.tag=3,((_tmp966.f1=& Cyc_print_all_tvars_r,_tmp966)))),_tmp965)))),((_tmp969->f5=(
(_tmp964="Print all type variables (even implicit default effects)",_tag_dyneither(_tmp964,sizeof(char),57))),_tmp969)))))))))))),((_tmp8D9[54]=(
# 1304
(_tmp96F=_region_malloc(r,sizeof(*_tmp96F)),((_tmp96F->f1=((_tmp96E="-noexpandtypedefs",_tag_dyneither(_tmp96E,sizeof(char),18))),((_tmp96F->f2=0,((_tmp96F->f3=((_tmp96D="",_tag_dyneither(_tmp96D,sizeof(char),1))),((_tmp96F->f4=(void*)(
(_tmp96B=_region_malloc(r,sizeof(*_tmp96B)),((_tmp96B[0]=((_tmp96C.tag=3,((_tmp96C.f1=& Cyc_noexpand_r,_tmp96C)))),_tmp96B)))),((_tmp96F->f5=(
(_tmp96A="Don't expand typedefs in pretty printing",_tag_dyneither(_tmp96A,sizeof(char),41))),_tmp96F)))))))))))),((_tmp8D9[53]=(
# 1301
(_tmp975=_region_malloc(r,sizeof(*_tmp975)),((_tmp975->f1=((_tmp974="-noremoveunused",_tag_dyneither(_tmp974,sizeof(char),16))),((_tmp975->f2=0,((_tmp975->f3=((_tmp973="",_tag_dyneither(_tmp973,sizeof(char),1))),((_tmp975->f4=(void*)(
(_tmp971=_region_malloc(r,sizeof(*_tmp971)),((_tmp971[0]=((_tmp972.tag=3,((_tmp972.f1=& Cyc_noshake_r,_tmp972)))),_tmp971)))),((_tmp975->f5=(
(_tmp970="Don't remove externed variables that aren't used",_tag_dyneither(_tmp970,sizeof(char),49))),_tmp975)))))))))))),((_tmp8D9[52]=(
# 1298
(_tmp97B=_region_malloc(r,sizeof(*_tmp97B)),((_tmp97B->f1=((_tmp97A="-nogc",_tag_dyneither(_tmp97A,sizeof(char),6))),((_tmp97B->f2=0,((_tmp97B->f3=((_tmp979="",_tag_dyneither(_tmp979,sizeof(char),1))),((_tmp97B->f4=(void*)(
(_tmp977=_region_malloc(r,sizeof(*_tmp977)),((_tmp977[0]=((_tmp978.tag=3,((_tmp978.f1=& Cyc_nogc_r,_tmp978)))),_tmp977)))),((_tmp97B->f5=(
(_tmp976="Don't link in the garbage collector",_tag_dyneither(_tmp976,sizeof(char),36))),_tmp97B)))))))))))),((_tmp8D9[51]=(
# 1295
(_tmp981=_region_malloc(r,sizeof(*_tmp981)),((_tmp981->f1=((_tmp980="-nocyc",_tag_dyneither(_tmp980,sizeof(char),7))),((_tmp981->f2=0,((_tmp981->f3=((_tmp97F="",_tag_dyneither(_tmp97F,sizeof(char),1))),((_tmp981->f4=(void*)(
(_tmp97D=_region_malloc(r,sizeof(*_tmp97D)),((_tmp97D[0]=((_tmp97E.tag=0,((_tmp97E.f1=Cyc_set_nocyc,_tmp97E)))),_tmp97D)))),((_tmp981->f5=(
(_tmp97C="Don't add implicit namespace Cyc",_tag_dyneither(_tmp97C,sizeof(char),33))),_tmp981)))))))))))),((_tmp8D9[50]=(
# 1292
(_tmp987=_region_malloc(r,sizeof(*_tmp987)),((_tmp987->f1=((_tmp986="-no-cpp-precomp",_tag_dyneither(_tmp986,sizeof(char),16))),((_tmp987->f2=0,((_tmp987->f3=((_tmp985="",_tag_dyneither(_tmp985,sizeof(char),1))),((_tmp987->f4=(void*)(
(_tmp983=_region_malloc(r,sizeof(*_tmp983)),((_tmp983[0]=((_tmp984.tag=0,((_tmp984.f1=Cyc_set_nocppprecomp,_tmp984)))),_tmp983)))),((_tmp987->f5=(
(_tmp982="Don't do smart preprocessing (mac only)",_tag_dyneither(_tmp982,sizeof(char),40))),_tmp987)))))))))))),((_tmp8D9[49]=(
# 1289
(_tmp98D=_region_malloc(r,sizeof(*_tmp98D)),((_tmp98D->f1=((_tmp98C="-use-cpp",_tag_dyneither(_tmp98C,sizeof(char),9))),((_tmp98D->f2=0,((_tmp98D->f3=((_tmp98B="<path>",_tag_dyneither(_tmp98B,sizeof(char),7))),((_tmp98D->f4=(void*)(
(_tmp989=_region_malloc(r,sizeof(*_tmp989)),((_tmp989[0]=((_tmp98A.tag=5,((_tmp98A.f1=Cyc_set_cpp,_tmp98A)))),_tmp989)))),((_tmp98D->f5=(
(_tmp988="Indicate which preprocessor to use",_tag_dyneither(_tmp988,sizeof(char),35))),_tmp98D)))))))))))),((_tmp8D9[48]=(
# 1286
(_tmp993=_region_malloc(r,sizeof(*_tmp993)),((_tmp993->f1=((_tmp992="--inline-checks",_tag_dyneither(_tmp992,sizeof(char),16))),((_tmp993->f2=0,((_tmp993->f3=((_tmp991="",_tag_dyneither(_tmp991,sizeof(char),1))),((_tmp993->f4=(void*)(
(_tmp98F=_region_malloc(r,sizeof(*_tmp98F)),((_tmp98F[0]=((_tmp990.tag=0,((_tmp990.f1=Cyc_set_inline_functions,_tmp990)))),_tmp98F)))),((_tmp993->f5=(
(_tmp98E="Inline bounds checks instead of #define",_tag_dyneither(_tmp98E,sizeof(char),40))),_tmp993)))))))))))),((_tmp8D9[47]=(
# 1283
(_tmp999=_region_malloc(r,sizeof(*_tmp999)),((_tmp999->f1=((_tmp998="--noboundschecks",_tag_dyneither(_tmp998,sizeof(char),17))),((_tmp999->f2=0,((_tmp999->f3=((_tmp997="",_tag_dyneither(_tmp997,sizeof(char),1))),((_tmp999->f4=(void*)(
(_tmp995=_region_malloc(r,sizeof(*_tmp995)),((_tmp995[0]=((_tmp996.tag=0,((_tmp996.f1=Cyc_set_noboundschecks,_tmp996)))),_tmp995)))),((_tmp999->f5=(
(_tmp994="Disable bounds checks",_tag_dyneither(_tmp994,sizeof(char),22))),_tmp999)))))))))))),((_tmp8D9[46]=(
# 1280
(_tmp99F=_region_malloc(r,sizeof(*_tmp99F)),((_tmp99F->f1=((_tmp99E="--nonullchecks",_tag_dyneither(_tmp99E,sizeof(char),15))),((_tmp99F->f2=0,((_tmp99F->f3=((_tmp99D="",_tag_dyneither(_tmp99D,sizeof(char),1))),((_tmp99F->f4=(void*)(
(_tmp99B=_region_malloc(r,sizeof(*_tmp99B)),((_tmp99B[0]=((_tmp99C.tag=0,((_tmp99C.f1=Cyc_set_nonullchecks,_tmp99C)))),_tmp99B)))),((_tmp99F->f5=(
(_tmp99A="Disable null checks",_tag_dyneither(_tmp99A,sizeof(char),20))),_tmp99F)))))))))))),((_tmp8D9[45]=(
# 1277
(_tmp9A5=_region_malloc(r,sizeof(*_tmp9A5)),((_tmp9A5->f1=((_tmp9A4="--nochecks",_tag_dyneither(_tmp9A4,sizeof(char),11))),((_tmp9A5->f2=0,((_tmp9A5->f3=((_tmp9A3="",_tag_dyneither(_tmp9A3,sizeof(char),1))),((_tmp9A5->f4=(void*)(
(_tmp9A1=_region_malloc(r,sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A2.tag=0,((_tmp9A2.f1=Cyc_set_nochecks,_tmp9A2)))),_tmp9A1)))),((_tmp9A5->f5=(
(_tmp9A0="Disable bounds/null checks",_tag_dyneither(_tmp9A0,sizeof(char),27))),_tmp9A5)))))))))))),((_tmp8D9[44]=(
# 1274
(_tmp9AB=_region_malloc(r,sizeof(*_tmp9AB)),((_tmp9AB->f1=((_tmp9AA="--nolineno",_tag_dyneither(_tmp9AA,sizeof(char),11))),((_tmp9AB->f2=0,((_tmp9AB->f3=((_tmp9A9="",_tag_dyneither(_tmp9A9,sizeof(char),1))),((_tmp9AB->f4=(void*)(
(_tmp9A7=_region_malloc(r,sizeof(*_tmp9A7)),((_tmp9A7[0]=((_tmp9A8.tag=0,((_tmp9A8.f1=Cyc_clear_lineno,_tmp9A8)))),_tmp9A7)))),((_tmp9AB->f5=(
(_tmp9A6="Don't generate line numbers for debugging (automatic with -pp)",_tag_dyneither(_tmp9A6,sizeof(char),63))),_tmp9AB)))))))))))),((_tmp8D9[43]=(
# 1271
(_tmp9B1=_region_malloc(r,sizeof(*_tmp9B1)),((_tmp9B1->f1=((_tmp9B0="-save-c",_tag_dyneither(_tmp9B0,sizeof(char),8))),((_tmp9B1->f2=0,((_tmp9B1->f3=((_tmp9AF="",_tag_dyneither(_tmp9AF,sizeof(char),1))),((_tmp9B1->f4=(void*)(
(_tmp9AD=_region_malloc(r,sizeof(*_tmp9AD)),((_tmp9AD[0]=((_tmp9AE.tag=3,((_tmp9AE.f1=& Cyc_save_c_r,_tmp9AE)))),_tmp9AD)))),((_tmp9B1->f5=(
(_tmp9AC="Don't delete temporary C files",_tag_dyneither(_tmp9AC,sizeof(char),31))),_tmp9B1)))))))))))),((_tmp8D9[42]=(
# 1268
(_tmp9B7=_region_malloc(r,sizeof(*_tmp9B7)),((_tmp9B7->f1=((_tmp9B6="-save-temps",_tag_dyneither(_tmp9B6,sizeof(char),12))),((_tmp9B7->f2=0,((_tmp9B7->f3=((_tmp9B5="",_tag_dyneither(_tmp9B5,sizeof(char),1))),((_tmp9B7->f4=(void*)(
(_tmp9B3=_region_malloc(r,sizeof(*_tmp9B3)),((_tmp9B3[0]=((_tmp9B4.tag=0,((_tmp9B4.f1=Cyc_set_save_temps,_tmp9B4)))),_tmp9B3)))),((_tmp9B7->f5=(
(_tmp9B2="Don't delete temporary files",_tag_dyneither(_tmp9B2,sizeof(char),29))),_tmp9B7)))))))))))),((_tmp8D9[41]=(
# 1265
(_tmp9BD=_region_malloc(r,sizeof(*_tmp9BD)),((_tmp9BD->f1=((_tmp9BC="-c-comp",_tag_dyneither(_tmp9BC,sizeof(char),8))),((_tmp9BD->f2=0,((_tmp9BD->f3=((_tmp9BB=" <compiler>",_tag_dyneither(_tmp9BB,sizeof(char),12))),((_tmp9BD->f4=(void*)(
(_tmp9B9=_region_malloc(r,sizeof(*_tmp9B9)),((_tmp9B9[0]=((_tmp9BA.tag=5,((_tmp9BA.f1=Cyc_set_c_compiler,_tmp9BA)))),_tmp9B9)))),((_tmp9BD->f5=(
(_tmp9B8="Produce C output for the given compiler",_tag_dyneither(_tmp9B8,sizeof(char),40))),_tmp9BD)))))))))))),((_tmp8D9[40]=(
# 1262
(_tmp9C3=_region_malloc(r,sizeof(*_tmp9C3)),((_tmp9C3->f1=((_tmp9C2="-un-gcc",_tag_dyneither(_tmp9C2,sizeof(char),8))),((_tmp9C3->f2=0,((_tmp9C3->f3=((_tmp9C1="",_tag_dyneither(_tmp9C1,sizeof(char),1))),((_tmp9C3->f4=(void*)(
(_tmp9BF=_region_malloc(r,sizeof(*_tmp9BF)),((_tmp9BF[0]=((_tmp9C0.tag=0,((_tmp9C0.f1=Cyc_set_tovc,_tmp9C0)))),_tmp9BF)))),((_tmp9C3->f5=(
(_tmp9BE="Avoid gcc extensions in C output (except for attributes)",_tag_dyneither(_tmp9BE,sizeof(char),57))),_tmp9C3)))))))))))),((_tmp8D9[39]=(
# 1259
(_tmp9C9=_region_malloc(r,sizeof(*_tmp9C9)),((_tmp9C9->f1=((_tmp9C8="-no-elim-statement-expressions",_tag_dyneither(_tmp9C8,sizeof(char),31))),((_tmp9C9->f2=0,((_tmp9C9->f3=((_tmp9C7="",_tag_dyneither(_tmp9C7,sizeof(char),1))),((_tmp9C9->f4=(void*)(
(_tmp9C5=_region_malloc(r,sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=4,((_tmp9C6.f1=& Cyc_elim_se_r,_tmp9C6)))),_tmp9C5)))),((_tmp9C9->f5=(
(_tmp9C4="Do not avoid statement expressions in C output",_tag_dyneither(_tmp9C4,sizeof(char),47))),_tmp9C9)))))))))))),((_tmp8D9[38]=(
# 1255
(_tmp9CF=_region_malloc(r,sizeof(*_tmp9CF)),((_tmp9CF->f1=((_tmp9CE="-elim-statement-expressions",_tag_dyneither(_tmp9CE,sizeof(char),28))),((_tmp9CF->f2=0,((_tmp9CF->f3=((_tmp9CD="",_tag_dyneither(_tmp9CD,sizeof(char),1))),((_tmp9CF->f4=(void*)(
(_tmp9CB=_region_malloc(r,sizeof(*_tmp9CB)),((_tmp9CB[0]=((_tmp9CC.tag=3,((_tmp9CC.f1=& Cyc_elim_se_r,_tmp9CC)))),_tmp9CB)))),((_tmp9CF->f5=(
# 1258
(_tmp9CA="Avoid statement expressions in C output (implies --inline-checks)",_tag_dyneither(_tmp9CA,sizeof(char),66))),_tmp9CF)))))))))))),((_tmp8D9[37]=(
# 1252
(_tmp9D5=_region_malloc(r,sizeof(*_tmp9D5)),((_tmp9D5->f1=((_tmp9D4="-up",_tag_dyneither(_tmp9D4,sizeof(char),4))),((_tmp9D5->f2=0,((_tmp9D5->f3=((_tmp9D3="",_tag_dyneither(_tmp9D3,sizeof(char),1))),((_tmp9D5->f4=(void*)(
(_tmp9D1=_region_malloc(r,sizeof(*_tmp9D1)),((_tmp9D1[0]=((_tmp9D2.tag=4,((_tmp9D2.f1=& Cyc_pp_r,_tmp9D2)))),_tmp9D1)))),((_tmp9D5->f5=(
(_tmp9D0="Ugly print the C code that Cyclone generates (default)",_tag_dyneither(_tmp9D0,sizeof(char),55))),_tmp9D5)))))))))))),((_tmp8D9[36]=(
# 1249
(_tmp9DB=_region_malloc(r,sizeof(*_tmp9DB)),((_tmp9DB->f1=((_tmp9DA="-pp",_tag_dyneither(_tmp9DA,sizeof(char),4))),((_tmp9DB->f2=0,((_tmp9DB->f3=((_tmp9D9="",_tag_dyneither(_tmp9D9,sizeof(char),1))),((_tmp9DB->f4=(void*)(
(_tmp9D7=_region_malloc(r,sizeof(*_tmp9D7)),((_tmp9D7[0]=((_tmp9D8.tag=3,((_tmp9D8.f1=& Cyc_pp_r,_tmp9D8)))),_tmp9D7)))),((_tmp9DB->f5=(
(_tmp9D6="Pretty print the C code that Cyclone generates",_tag_dyneither(_tmp9D6,sizeof(char),47))),_tmp9DB)))))))))))),((_tmp8D9[35]=(
# 1246
(_tmp9E1=_region_malloc(r,sizeof(*_tmp9E1)),((_tmp9E1->f1=((_tmp9E0="-ic",_tag_dyneither(_tmp9E0,sizeof(char),4))),((_tmp9E1->f2=0,((_tmp9E1->f3=((_tmp9DF="",_tag_dyneither(_tmp9DF,sizeof(char),1))),((_tmp9E1->f4=(void*)(
(_tmp9DD=_region_malloc(r,sizeof(*_tmp9DD)),((_tmp9DD[0]=((_tmp9DE.tag=3,((_tmp9DE.f1=& Cyc_ic_r,_tmp9DE)))),_tmp9DD)))),((_tmp9E1->f5=(
(_tmp9DC="Activate the link-checker",_tag_dyneither(_tmp9DC,sizeof(char),26))),_tmp9E1)))))))))))),((_tmp8D9[34]=(
# 1243
(_tmp9E7=_region_malloc(r,sizeof(*_tmp9E7)),((_tmp9E7->f1=((_tmp9E6="-stopafter-toc",_tag_dyneither(_tmp9E6,sizeof(char),15))),((_tmp9E7->f2=0,((_tmp9E7->f3=((_tmp9E5="",_tag_dyneither(_tmp9E5,sizeof(char),1))),((_tmp9E7->f4=(void*)(
(_tmp9E3=_region_malloc(r,sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=3,((_tmp9E4.f1=& Cyc_toc_r,_tmp9E4)))),_tmp9E3)))),((_tmp9E7->f5=(
(_tmp9E2="Stop after translation to C",_tag_dyneither(_tmp9E2,sizeof(char),28))),_tmp9E7)))))))))))),((_tmp8D9[33]=(
# 1240
(_tmp9ED=_region_malloc(r,sizeof(*_tmp9ED)),((_tmp9ED->f1=((_tmp9EC="-stopafter-cf",_tag_dyneither(_tmp9EC,sizeof(char),14))),((_tmp9ED->f2=0,((_tmp9ED->f3=((_tmp9EB="",_tag_dyneither(_tmp9EB,sizeof(char),1))),((_tmp9ED->f4=(void*)(
(_tmp9E9=_region_malloc(r,sizeof(*_tmp9E9)),((_tmp9E9[0]=((_tmp9EA.tag=3,((_tmp9EA.f1=& Cyc_cf_r,_tmp9EA)))),_tmp9E9)))),((_tmp9ED->f5=(
(_tmp9E8="Stop after control-flow checking",_tag_dyneither(_tmp9E8,sizeof(char),33))),_tmp9ED)))))))))))),((_tmp8D9[32]=(
# 1237
(_tmp9F3=_region_malloc(r,sizeof(*_tmp9F3)),((_tmp9F3->f1=((_tmp9F2="-noprint",_tag_dyneither(_tmp9F2,sizeof(char),9))),((_tmp9F3->f2=0,((_tmp9F3->f3=((_tmp9F1="",_tag_dyneither(_tmp9F1,sizeof(char),1))),((_tmp9F3->f4=(void*)(
(_tmp9EF=_region_malloc(r,sizeof(*_tmp9EF)),((_tmp9EF[0]=((_tmp9F0.tag=3,((_tmp9F0.f1=& Cyc_noprint_r,_tmp9F0)))),_tmp9EF)))),((_tmp9F3->f5=(
(_tmp9EE="Do not print output (when stopping early)",_tag_dyneither(_tmp9EE,sizeof(char),42))),_tmp9F3)))))))))))),((_tmp8D9[31]=(
# 1234
(_tmp9F9=_region_malloc(r,sizeof(*_tmp9F9)),((_tmp9F9->f1=((_tmp9F8="-stopafter-tc",_tag_dyneither(_tmp9F8,sizeof(char),14))),((_tmp9F9->f2=0,((_tmp9F9->f3=((_tmp9F7="",_tag_dyneither(_tmp9F7,sizeof(char),1))),((_tmp9F9->f4=(void*)(
(_tmp9F5=_region_malloc(r,sizeof(*_tmp9F5)),((_tmp9F5[0]=((_tmp9F6.tag=3,((_tmp9F6.f1=& Cyc_tc_r,_tmp9F6)))),_tmp9F5)))),((_tmp9F9->f5=(
(_tmp9F4="Stop after type checking",_tag_dyneither(_tmp9F4,sizeof(char),25))),_tmp9F9)))))))))))),((_tmp8D9[30]=(
# 1231
(_tmp9FF=_region_malloc(r,sizeof(*_tmp9FF)),((_tmp9FF->f1=((_tmp9FE="-stopafter-parse",_tag_dyneither(_tmp9FE,sizeof(char),17))),((_tmp9FF->f2=0,((_tmp9FF->f3=((_tmp9FD="",_tag_dyneither(_tmp9FD,sizeof(char),1))),((_tmp9FF->f4=(void*)(
(_tmp9FB=_region_malloc(r,sizeof(*_tmp9FB)),((_tmp9FB[0]=((_tmp9FC.tag=3,((_tmp9FC.f1=& Cyc_parseonly_r,_tmp9FC)))),_tmp9FB)))),((_tmp9FF->f5=(
(_tmp9FA="Stop after parsing",_tag_dyneither(_tmp9FA,sizeof(char),19))),_tmp9FF)))))))))))),((_tmp8D9[29]=(
# 1228
(_tmpA05=_region_malloc(r,sizeof(*_tmpA05)),((_tmpA05->f1=((_tmpA04="-E",_tag_dyneither(_tmpA04,sizeof(char),3))),((_tmpA05->f2=0,((_tmpA05->f3=((_tmpA03="",_tag_dyneither(_tmpA03,sizeof(char),1))),((_tmpA05->f4=(void*)(
(_tmpA01=_region_malloc(r,sizeof(*_tmpA01)),((_tmpA01[0]=((_tmpA02.tag=3,((_tmpA02.f1=& Cyc_stop_after_cpp_r,_tmpA02)))),_tmpA01)))),((_tmpA05->f5=(
(_tmpA00="Stop after preprocessing",_tag_dyneither(_tmpA00,sizeof(char),25))),_tmpA05)))))))))))),((_tmp8D9[28]=(
# 1225
(_tmpA0B=_region_malloc(r,sizeof(*_tmpA0B)),((_tmpA0B->f1=((_tmpA0A="-Wall",_tag_dyneither(_tmpA0A,sizeof(char),6))),((_tmpA0B->f2=0,((_tmpA0B->f3=((_tmpA09="",_tag_dyneither(_tmpA09,sizeof(char),1))),((_tmpA0B->f4=(void*)(
(_tmpA07=_region_malloc(r,sizeof(*_tmpA07)),((_tmpA07[0]=((_tmpA08.tag=0,((_tmpA08.f1=Cyc_set_all_warnings,_tmpA08)))),_tmpA07)))),((_tmpA0B->f5=(
(_tmpA06="Turn on all warnings",_tag_dyneither(_tmpA06,sizeof(char),21))),_tmpA0B)))))))))))),((_tmp8D9[27]=(
# 1222
(_tmpA11=_region_malloc(r,sizeof(*_tmpA11)),((_tmpA11->f1=((_tmpA10="-Woverride",_tag_dyneither(_tmpA10,sizeof(char),11))),((_tmpA11->f2=0,((_tmpA11->f3=((_tmpA0F="",_tag_dyneither(_tmpA0F,sizeof(char),1))),((_tmpA11->f4=(void*)(
(_tmpA0D=_region_malloc(r,sizeof(*_tmpA0D)),((_tmpA0D[0]=((_tmpA0E.tag=3,((_tmpA0E.f1=& Cyc_Tcenv_warn_override,_tmpA0E)))),_tmpA0D)))),((_tmpA11->f5=(
(_tmpA0C="Warn when a local variable overrides an existing variable",_tag_dyneither(_tmpA0C,sizeof(char),58))),_tmpA11)))))))))))),((_tmp8D9[26]=(
# 1219
(_tmpA17=_region_malloc(r,sizeof(*_tmpA17)),((_tmpA17->f1=((_tmpA16="-Wlose-unique",_tag_dyneither(_tmpA16,sizeof(char),14))),((_tmpA17->f2=0,((_tmpA17->f3=((_tmpA15="",_tag_dyneither(_tmpA15,sizeof(char),1))),((_tmpA17->f4=(void*)(
(_tmpA13=_region_malloc(r,sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA14.tag=3,((_tmpA14.f1=& Cyc_NewControlFlow_warn_lose_unique,_tmpA14)))),_tmpA13)))),((_tmpA17->f5=(
(_tmpA12="Try to warn when a unique pointer might get lost",_tag_dyneither(_tmpA12,sizeof(char),49))),_tmpA17)))))))))))),((_tmp8D9[25]=(
# 1215
(_tmpA1D=_region_malloc(r,sizeof(*_tmpA1D)),((_tmpA1D->f1=((_tmpA1C="-b",_tag_dyneither(_tmpA1C,sizeof(char),3))),((_tmpA1D->f2=0,((_tmpA1D->f3=((_tmpA1B="<machine>",_tag_dyneither(_tmpA1B,sizeof(char),10))),((_tmpA1D->f4=(void*)(
(_tmpA19=_region_malloc(r,sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1A.tag=5,((_tmpA1A.f1=Cyc_set_target_arch,_tmpA1A)))),_tmpA19)))),((_tmpA1D->f5=(
# 1218
(_tmpA18="Set target machine",_tag_dyneither(_tmpA18,sizeof(char),19))),_tmpA1D)))))))))))),((_tmp8D9[24]=(
# 1212
(_tmpA23=_region_malloc(r,sizeof(*_tmpA23)),((_tmpA23->f1=((_tmpA22="-MT",_tag_dyneither(_tmpA22,sizeof(char),4))),((_tmpA23->f2=0,((_tmpA23->f3=((_tmpA21=" <target>",_tag_dyneither(_tmpA21,sizeof(char),10))),((_tmpA23->f4=(void*)(
(_tmpA1F=_region_malloc(r,sizeof(*_tmpA1F)),((_tmpA1F[0]=((_tmpA20.tag=5,((_tmpA20.f1=Cyc_set_dependencies_target,_tmpA20)))),_tmpA1F)))),((_tmpA23->f5=(
(_tmpA1E="Give target for dependencies",_tag_dyneither(_tmpA1E,sizeof(char),29))),_tmpA23)))))))))))),((_tmp8D9[23]=(
# 1208
(_tmpA29=_region_malloc(r,sizeof(*_tmpA29)),((_tmpA29->f1=((_tmpA28="-MG",_tag_dyneither(_tmpA28,sizeof(char),4))),((_tmpA29->f2=0,((_tmpA29->f3=((_tmpA27="",_tag_dyneither(_tmpA27,sizeof(char),1))),((_tmpA29->f4=(void*)(
(_tmpA25=_region_malloc(r,sizeof(*_tmpA25)),((_tmpA25[0]=((_tmpA26.tag=1,((_tmpA26.f1=Cyc_add_cpparg,_tmpA26)))),_tmpA25)))),((_tmpA29->f5=(
# 1211
(_tmpA24="When producing dependencies assume that missing files are generated",_tag_dyneither(_tmpA24,sizeof(char),68))),_tmpA29)))))))))))),((_tmp8D9[22]=(
# 1205
(_tmpA2F=_region_malloc(r,sizeof(*_tmpA2F)),((_tmpA2F->f1=((_tmpA2E="-M",_tag_dyneither(_tmpA2E,sizeof(char),3))),((_tmpA2F->f2=0,((_tmpA2F->f3=((_tmpA2D="",_tag_dyneither(_tmpA2D,sizeof(char),1))),((_tmpA2F->f4=(void*)(
(_tmpA2B=_region_malloc(r,sizeof(*_tmpA2B)),((_tmpA2B[0]=((_tmpA2C.tag=0,((_tmpA2C.f1=Cyc_set_produce_dependencies,_tmpA2C)))),_tmpA2B)))),((_tmpA2F->f5=(
(_tmpA2A="Produce dependencies",_tag_dyneither(_tmpA2A,sizeof(char),21))),_tmpA2F)))))))))))),((_tmp8D9[21]=(
# 1202
(_tmpA35=_region_malloc(r,sizeof(*_tmpA35)),((_tmpA35->f1=((_tmpA34="-S",_tag_dyneither(_tmpA34,sizeof(char),3))),((_tmpA35->f2=0,((_tmpA35->f3=((_tmpA33="",_tag_dyneither(_tmpA33,sizeof(char),1))),((_tmpA35->f4=(void*)(
(_tmpA31=_region_malloc(r,sizeof(*_tmpA31)),((_tmpA31[0]=((_tmpA32.tag=0,((_tmpA32.f1=Cyc_set_stop_after_asmfile,_tmpA32)))),_tmpA31)))),((_tmpA35->f5=(
(_tmpA30="Stop after producing assembly code",_tag_dyneither(_tmpA30,sizeof(char),35))),_tmpA35)))))))))))),((_tmp8D9[20]=(
# 1199
(_tmpA3B=_region_malloc(r,sizeof(*_tmpA3B)),((_tmpA3B->f1=((_tmpA3A="-pa",_tag_dyneither(_tmpA3A,sizeof(char),4))),((_tmpA3B->f2=0,((_tmpA3B->f3=((_tmpA39="",_tag_dyneither(_tmpA39,sizeof(char),1))),((_tmpA3B->f4=(void*)(
(_tmpA37=_region_malloc(r,sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA38.tag=0,((_tmpA38.f1=Cyc_set_pa,_tmpA38)))),_tmpA37)))),((_tmpA3B->f5=(
(_tmpA36="Compile for profiling with aprof",_tag_dyneither(_tmpA36,sizeof(char),33))),_tmpA3B)))))))))))),((_tmp8D9[19]=(
# 1196
(_tmpA41=_region_malloc(r,sizeof(*_tmpA41)),((_tmpA41->f1=((_tmpA40="-pg",_tag_dyneither(_tmpA40,sizeof(char),4))),((_tmpA41->f2=0,((_tmpA41->f3=((_tmpA3F="",_tag_dyneither(_tmpA3F,sizeof(char),1))),((_tmpA41->f4=(void*)(
(_tmpA3D=_region_malloc(r,sizeof(*_tmpA3D)),((_tmpA3D[0]=((_tmpA3E.tag=0,((_tmpA3E.f1=Cyc_set_pg,_tmpA3E)))),_tmpA3D)))),((_tmpA41->f5=(
(_tmpA3C="Compile for profiling with gprof",_tag_dyneither(_tmpA3C,sizeof(char),33))),_tmpA41)))))))))))),((_tmp8D9[18]=(
# 1193
(_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47->f1=((_tmpA46="-p",_tag_dyneither(_tmpA46,sizeof(char),3))),((_tmpA47->f2=0,((_tmpA47->f3=((_tmpA45="",_tag_dyneither(_tmpA45,sizeof(char),1))),((_tmpA47->f4=(void*)(
(_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=1,((_tmpA44.f1=Cyc_add_ccarg,_tmpA44)))),_tmpA43)))),((_tmpA47->f5=(
(_tmpA42="Compile for profiling with prof",_tag_dyneither(_tmpA42,sizeof(char),32))),_tmpA47)))))))))))),((_tmp8D9[17]=(
# 1190
(_tmpA4D=_region_malloc(r,sizeof(*_tmpA4D)),((_tmpA4D->f1=((_tmpA4C="-g",_tag_dyneither(_tmpA4C,sizeof(char),3))),((_tmpA4D->f2=0,((_tmpA4D->f3=((_tmpA4B="",_tag_dyneither(_tmpA4B,sizeof(char),1))),((_tmpA4D->f4=(void*)(
(_tmpA49=_region_malloc(r,sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=1,((_tmpA4A.f1=Cyc_add_ccarg,_tmpA4A)))),_tmpA49)))),((_tmpA4D->f5=(
(_tmpA48="Compile for debugging",_tag_dyneither(_tmpA48,sizeof(char),22))),_tmpA4D)))))))))))),((_tmp8D9[16]=(
# 1187
(_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53->f1=((_tmpA52="-fomit-frame-pointer",_tag_dyneither(_tmpA52,sizeof(char),21))),((_tmpA53->f2=0,((_tmpA53->f3=((_tmpA51="",_tag_dyneither(_tmpA51,sizeof(char),1))),((_tmpA53->f4=(void*)(
(_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=1,((_tmpA50.f1=Cyc_add_ccarg,_tmpA50)))),_tmpA4F)))),((_tmpA53->f5=(
(_tmpA4E="Omit frame pointer",_tag_dyneither(_tmpA4E,sizeof(char),19))),_tmpA53)))))))))))),((_tmp8D9[15]=(
# 1184
(_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59->f1=((_tmpA58="-O3",_tag_dyneither(_tmpA58,sizeof(char),4))),((_tmpA59->f2=0,((_tmpA59->f3=((_tmpA57="",_tag_dyneither(_tmpA57,sizeof(char),1))),((_tmpA59->f4=(void*)(
(_tmpA55=_region_malloc(r,sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=1,((_tmpA56.f1=Cyc_add_ccarg,_tmpA56)))),_tmpA55)))),((_tmpA59->f5=(
(_tmpA54="Even more optimization",_tag_dyneither(_tmpA54,sizeof(char),23))),_tmpA59)))))))))))),((_tmp8D9[14]=(
# 1181
(_tmpA5F=_region_malloc(r,sizeof(*_tmpA5F)),((_tmpA5F->f1=((_tmpA5E="-O2",_tag_dyneither(_tmpA5E,sizeof(char),4))),((_tmpA5F->f2=0,((_tmpA5F->f3=((_tmpA5D="",_tag_dyneither(_tmpA5D,sizeof(char),1))),((_tmpA5F->f4=(void*)(
(_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=1,((_tmpA5C.f1=Cyc_add_ccarg,_tmpA5C)))),_tmpA5B)))),((_tmpA5F->f5=(
(_tmpA5A="A higher level of optimization",_tag_dyneither(_tmpA5A,sizeof(char),31))),_tmpA5F)))))))))))),((_tmp8D9[13]=(
# 1178
(_tmpA65=_region_malloc(r,sizeof(*_tmpA65)),((_tmpA65->f1=((_tmpA64="-O",_tag_dyneither(_tmpA64,sizeof(char),3))),((_tmpA65->f2=0,((_tmpA65->f3=((_tmpA63="",_tag_dyneither(_tmpA63,sizeof(char),1))),((_tmpA65->f4=(void*)(
(_tmpA61=_region_malloc(r,sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA62.tag=1,((_tmpA62.f1=Cyc_add_ccarg,_tmpA62)))),_tmpA61)))),((_tmpA65->f5=(
(_tmpA60="Optimize",_tag_dyneither(_tmpA60,sizeof(char),9))),_tmpA65)))))))))))),((_tmp8D9[12]=(
# 1175
(_tmpA6B=_region_malloc(r,sizeof(*_tmpA6B)),((_tmpA6B->f1=((_tmpA6A="-O0",_tag_dyneither(_tmpA6A,sizeof(char),4))),((_tmpA6B->f2=0,((_tmpA6B->f3=((_tmpA69="",_tag_dyneither(_tmpA69,sizeof(char),1))),((_tmpA6B->f4=(void*)(
(_tmpA67=_region_malloc(r,sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=1,((_tmpA68.f1=Cyc_add_ccarg,_tmpA68)))),_tmpA67)))),((_tmpA6B->f5=(
(_tmpA66="Don't optimize",_tag_dyneither(_tmpA66,sizeof(char),15))),_tmpA6B)))))))))))),((_tmp8D9[11]=(
# 1172
(_tmpA71=_region_malloc(r,sizeof(*_tmpA71)),((_tmpA71->f1=((_tmpA70="-s",_tag_dyneither(_tmpA70,sizeof(char),3))),((_tmpA71->f2=0,((_tmpA71->f3=((_tmpA6F="",_tag_dyneither(_tmpA6F,sizeof(char),1))),((_tmpA71->f4=(void*)(
(_tmpA6D=_region_malloc(r,sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=1,((_tmpA6E.f1=Cyc_add_ccarg,_tmpA6E)))),_tmpA6D)))),((_tmpA71->f5=(
(_tmpA6C="Remove all symbol table and relocation info from executable",_tag_dyneither(_tmpA6C,sizeof(char),60))),_tmpA71)))))))))))),((_tmp8D9[10]=(
# 1169
(_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77->f1=((_tmpA76="-x",_tag_dyneither(_tmpA76,sizeof(char),3))),((_tmpA77->f2=0,((_tmpA77->f3=((_tmpA75=" <language>",_tag_dyneither(_tmpA75,sizeof(char),12))),((_tmpA77->f4=(void*)(
(_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=5,((_tmpA74.f1=Cyc_set_inputtype,_tmpA74)))),_tmpA73)))),((_tmpA77->f5=(
(_tmpA72="Specify <language> for the following input files",_tag_dyneither(_tmpA72,sizeof(char),49))),_tmpA77)))))))))))),((_tmp8D9[9]=(
# 1166
(_tmpA7D=_region_malloc(r,sizeof(*_tmpA7D)),((_tmpA7D->f1=((_tmpA7C="-c",_tag_dyneither(_tmpA7C,sizeof(char),3))),((_tmpA7D->f2=0,((_tmpA7D->f3=((_tmpA7B="",_tag_dyneither(_tmpA7B,sizeof(char),1))),((_tmpA7D->f4=(void*)(
(_tmpA79=_region_malloc(r,sizeof(*_tmpA79)),((_tmpA79[0]=((_tmpA7A.tag=0,((_tmpA7A.f1=Cyc_set_stop_after_objectfile,_tmpA7A)))),_tmpA79)))),((_tmpA7D->f5=(
(_tmpA78="Produce an object file instead of an executable; do not link",_tag_dyneither(_tmpA78,sizeof(char),61))),_tmpA7D)))))))))))),((_tmp8D9[8]=(
# 1163
(_tmpA83=_region_malloc(r,sizeof(*_tmpA83)),((_tmpA83->f1=((_tmpA82="-m",_tag_dyneither(_tmpA82,sizeof(char),3))),((_tmpA83->f2=1,((_tmpA83->f3=((_tmpA81="<option>",_tag_dyneither(_tmpA81,sizeof(char),9))),((_tmpA83->f4=(void*)(
(_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F[0]=((_tmpA80.tag=1,((_tmpA80.f1=Cyc_add_marg,_tmpA80)))),_tmpA7F)))),((_tmpA83->f5=(
(_tmpA7E="GCC specific: pass machine-dependent flag on to gcc",_tag_dyneither(_tmpA7E,sizeof(char),52))),_tmpA83)))))))))))),((_tmp8D9[7]=(
# 1160
(_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->f1=((_tmpA88="-l",_tag_dyneither(_tmpA88,sizeof(char),3))),((_tmpA89->f2=1,((_tmpA89->f3=((_tmpA87="<libname>",_tag_dyneither(_tmpA87,sizeof(char),10))),((_tmpA89->f4=(void*)(
(_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA86.tag=1,((_tmpA86.f1=Cyc_add_libarg,_tmpA86)))),_tmpA85)))),((_tmpA89->f5=(
(_tmpA84="Library file",_tag_dyneither(_tmpA84,sizeof(char),13))),_tmpA89)))))))))))),((_tmp8D9[6]=(
# 1157
(_tmpA8F=_region_malloc(r,sizeof(*_tmpA8F)),((_tmpA8F->f1=((_tmpA8E="-L",_tag_dyneither(_tmpA8E,sizeof(char),3))),((_tmpA8F->f2=1,((_tmpA8F->f3=((_tmpA8D="<dir>",_tag_dyneither(_tmpA8D,sizeof(char),6))),((_tmpA8F->f4=(void*)(
(_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8C.tag=1,((_tmpA8C.f1=Cyc_add_ccarg,_tmpA8C)))),_tmpA8B)))),((_tmpA8F->f5=(
(_tmpA8A="Add to the list of directories for -l",_tag_dyneither(_tmpA8A,sizeof(char),38))),_tmpA8F)))))))))))),((_tmp8D9[5]=(
# 1154
(_tmpA95=_region_malloc(r,sizeof(*_tmpA95)),((_tmpA95->f1=((_tmpA94="-I",_tag_dyneither(_tmpA94,sizeof(char),3))),((_tmpA95->f2=1,((_tmpA95->f3=((_tmpA93="<dir>",_tag_dyneither(_tmpA93,sizeof(char),6))),((_tmpA95->f4=(void*)(
(_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA92.tag=1,((_tmpA92.f1=Cyc_add_cpparg,_tmpA92)))),_tmpA91)))),((_tmpA95->f5=(
(_tmpA90="Add to the list of directories to search for include files",_tag_dyneither(_tmpA90,sizeof(char),59))),_tmpA95)))))))))))),((_tmp8D9[4]=(
# 1151
(_tmpA9B=_region_malloc(r,sizeof(*_tmpA9B)),((_tmpA9B->f1=((_tmpA9A="-B",_tag_dyneither(_tmpA9A,sizeof(char),3))),((_tmpA9B->f2=1,((_tmpA9B->f3=((_tmpA99="<file>",_tag_dyneither(_tmpA99,sizeof(char),7))),((_tmpA9B->f4=(void*)(
(_tmpA97=_region_malloc(r,sizeof(*_tmpA97)),((_tmpA97[0]=((_tmpA98.tag=1,((_tmpA98.f1=Cyc_add_cyclone_exec_path,_tmpA98)))),_tmpA97)))),((_tmpA9B->f5=(
(_tmpA96="Add to the list of directories to search for compiler files",_tag_dyneither(_tmpA96,sizeof(char),60))),_tmpA9B)))))))))))),((_tmp8D9[3]=(
# 1148
(_tmpAA1=_region_malloc(r,sizeof(*_tmpAA1)),((_tmpAA1->f1=((_tmpAA0="-D",_tag_dyneither(_tmpAA0,sizeof(char),3))),((_tmpAA1->f2=1,((_tmpAA1->f3=((_tmpA9F="<name>[=<value>]",_tag_dyneither(_tmpA9F,sizeof(char),17))),((_tmpAA1->f4=(void*)(
(_tmpA9D=_region_malloc(r,sizeof(*_tmpA9D)),((_tmpA9D[0]=((_tmpA9E.tag=1,((_tmpA9E.f1=Cyc_add_cpparg,_tmpA9E)))),_tmpA9D)))),((_tmpAA1->f5=(
(_tmpA9C="Pass definition to preprocessor",_tag_dyneither(_tmpA9C,sizeof(char),32))),_tmpAA1)))))))))))),((_tmp8D9[2]=(
# 1145
(_tmpAA7=_region_malloc(r,sizeof(*_tmpAA7)),((_tmpAA7->f1=((_tmpAA6="-o",_tag_dyneither(_tmpAA6,sizeof(char),3))),((_tmpAA7->f2=0,((_tmpAA7->f3=((_tmpAA5=" <file>",_tag_dyneither(_tmpAA5,sizeof(char),8))),((_tmpAA7->f4=(void*)(
(_tmpAA3=_region_malloc(r,sizeof(*_tmpAA3)),((_tmpAA3[0]=((_tmpAA4.tag=5,((_tmpAA4.f1=Cyc_set_output_file,_tmpAA4)))),_tmpAA3)))),((_tmpAA7->f5=(
(_tmpAA2="Set the output file name to <file>",_tag_dyneither(_tmpAA2,sizeof(char),35))),_tmpAA7)))))))))))),((_tmp8D9[1]=(
# 1142
(_tmpAAD=_region_malloc(r,sizeof(*_tmpAAD)),((_tmpAAD->f1=((_tmpAAC="--version",_tag_dyneither(_tmpAAC,sizeof(char),10))),((_tmpAAD->f2=0,((_tmpAAD->f3=((_tmpAAB="",_tag_dyneither(_tmpAAB,sizeof(char),1))),((_tmpAAD->f4=(void*)(
(_tmpAA9=_region_malloc(r,sizeof(*_tmpAA9)),((_tmpAA9[0]=((_tmpAAA.tag=0,((_tmpAAA.f1=Cyc_print_version,_tmpAAA)))),_tmpAA9)))),((_tmpAAD->f5=(
(_tmpAA8="Print version information and exit",_tag_dyneither(_tmpAA8,sizeof(char),35))),_tmpAAD)))))))))))),((_tmp8D9[0]=(
# 1139
(_tmpAB3=_region_malloc(r,sizeof(*_tmpAB3)),((_tmpAB3->f1=((_tmpAB2="-v",_tag_dyneither(_tmpAB2,sizeof(char),3))),((_tmpAB3->f2=0,((_tmpAB3->f3=((_tmpAB1="",_tag_dyneither(_tmpAB1,sizeof(char),1))),((_tmpAB3->f4=(void*)(
(_tmpAAF=_region_malloc(r,sizeof(*_tmpAAF)),((_tmpAAF[0]=((_tmpAB0.tag=3,((_tmpAB0.f1=& Cyc_v_r,_tmpAB0)))),_tmpAAF)))),((_tmpAB3->f5=(
(_tmpAAE="Print compilation stages verbosely",_tag_dyneither(_tmpAAE,sizeof(char),35))),_tmpAB3)))))))))))),((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp8D9,sizeof(struct _tuple24*),79)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))));
# 1381
const char*_tmpAB4;struct _dyneither_ptr optstring=(_tmpAB4="Options:",_tag_dyneither(_tmpAB4,sizeof(char),9));
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp167);
# 1386
if((char*)Cyc_target_arch.curr == (char*)(_tag_dyneither(0,0,0)).curr)Cyc_target_arch=(struct _dyneither_ptr)Cstring_to_string(Carch);{
# 1394
char*_tmpAB5;struct _dyneither_ptr cyclone_exec_prefix=(_tmpAB5=getenv("CYCLONE_EXEC_PREFIX"),_tag_dyneither(_tmpAB5,sizeof(char),_get_zero_arr_size_char((void*)_tmpAB5,1)));
if((char*)cyclone_exec_prefix.curr != (char*)(_tag_dyneither(0,0,0)).curr){
struct _dyneither_ptr*_tmpAB8;struct Cyc_List_List*_tmpAB7;Cyc_cyclone_exec_path=((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=cyclone_exec_prefix,_tmpAB8)))),((_tmpAB7->tl=Cyc_cyclone_exec_path,_tmpAB7))))));}
def_lib_path=(struct _dyneither_ptr)Cstring_to_string(Cdef_lib_path);
if(Cyc_strlen((struct _dyneither_ptr)def_lib_path)> 0){
struct _dyneither_ptr*_tmpABE;const char*_tmpABD;struct Cyc_List_List*_tmpABC;Cyc_cyclone_exec_path=(
(_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->hd=((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=(struct _dyneither_ptr)Cyc_Filename_concat(def_lib_path,((_tmpABD="cyc-lib",_tag_dyneither(_tmpABD,sizeof(char),8)))),_tmpABE)))),((_tmpABC->tl=Cyc_cyclone_exec_path,_tmpABC))))));}
# 1402
Cyc_cyclone_exec_path=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_cyclone_exec_path);
cyclone_arch_path=Cyc_also_subdir(Cyc_cyclone_exec_path,Cyc_target_arch);{
# 1410
const char*_tmpABF;struct _dyneither_ptr _tmp16E=Cyc_do_find(cyclone_arch_path,((_tmpABF="cycspecs",_tag_dyneither(_tmpABF,sizeof(char),9))));
if(Cyc_v_r){const char*_tmpAC3;void*_tmpAC2[1];struct Cyc_String_pa_PrintArg_struct _tmpAC1;(_tmpAC1.tag=0,((_tmpAC1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16E),((_tmpAC2[0]=& _tmpAC1,Cyc_fprintf(Cyc_stderr,((_tmpAC3="Reading from specs file %s\n",_tag_dyneither(_tmpAC3,sizeof(char),28))),_tag_dyneither(_tmpAC2,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp172=Cyc_read_specs(_tmp16E);
struct _dyneither_ptr _tmp173=_tag_dyneither(0,0,0);
{struct _handler_cons _tmp174;_push_handler(& _tmp174);{int _tmp176=0;if(setjmp(_tmp174.handler))_tmp176=1;if(!_tmp176){
{struct _dyneither_ptr _tmp177=*((struct _dyneither_ptr*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_assoc_cmp)(Cyc_strptrcmp,_tmp172,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"cyclone",sizeof(char),8));
_tmp173=Cyc_split_specs(_tmp177);}
# 1415
;_pop_handler();}else{void*_tmp175=(void*)_exn_thrown;void*_tmp17A=_tmp175;void*_tmp17C;_LL34: {struct Cyc_Core_Not_found_exn_struct*_tmp17B=(struct Cyc_Core_Not_found_exn_struct*)_tmp17A;if(_tmp17B->tag != Cyc_Core_Not_found)goto _LL36;}_LL35:
# 1418
 goto _LL33;_LL36: _tmp17C=_tmp17A;_LL37:(void)_throw(_tmp17C);_LL33:;}};}
# 1420
if((struct _dyneither_ptr*)_tmp173.curr != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp173);}
# 1425
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,optstring,_tmp168);{
const char*_tmpAC4;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_cpp,((_tmpAC4="",_tag_dyneither(_tmpAC4,sizeof(char),1))))== 0){
# 1443 "cyclone.cyc"
const char*_tmp17E=Cyc_produce_dependencies?"":" -E";
const char*_tmpACC;void*_tmpACB[3];struct Cyc_String_pa_PrintArg_struct _tmpACA;const char*_tmpAC9;struct Cyc_String_pa_PrintArg_struct _tmpAC8;struct Cyc_String_pa_PrintArg_struct _tmpAC7;Cyc_set_cpp((struct _dyneither_ptr)((_tmpAC7.tag=0,((_tmpAC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16E),((_tmpAC8.tag=0,((_tmpAC8.f1=(struct _dyneither_ptr)((_tmpAC9=_tmp17E,_tag_dyneither(_tmpAC9,sizeof(char),_get_zero_arr_size_char((void*)_tmpAC9,1)))),((_tmpACA.tag=0,((_tmpACA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpACB[0]=& _tmpACA,((_tmpACB[1]=& _tmpAC8,((_tmpACB[2]=& _tmpAC7,Cyc_aprintf(((_tmpACC="%s -w -x c%s -specs %s",_tag_dyneither(_tmpACC,sizeof(char),23))),_tag_dyneither(_tmpACB,sizeof(void*),3)))))))))))))))))))));}};};};};};};}
# 1447
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
{const char*_tmpACF;void*_tmpACE;(_tmpACE=0,Cyc_fprintf(Cyc_stderr,((_tmpACF="missing file\n",_tag_dyneither(_tmpACF,sizeof(char),14))),_tag_dyneither(_tmpACE,sizeof(void*),0)));}
exit(1);}
# 1456
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r){
const char*_tmpAD0;Cyc_cyc_setjmp=Cyc_do_find(cyclone_arch_path,((_tmpAD0="cyc_setjmp.h",_tag_dyneither(_tmpAD0,sizeof(char),13))));}
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0){
const char*_tmpAD1;Cyc_cyc_include=Cyc_do_find(Cyc_cyclone_exec_path,((_tmpAD1="cyc_include.h",_tag_dyneither(_tmpAD1,sizeof(char),14))));}
# 1462
{struct Cyc_List_List*_tmp373=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp373 != 0;_tmp373=_tmp373->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp373->hd));
if(Cyc_compile_failure){int _tmp374=1;_npop_handler(0);return _tmp374;}}}
# 1467
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r){int _tmp375=0;_npop_handler(0);return _tmp375;}
# 1473
if(Cyc_ccargs == 0){int _tmp376=0;_npop_handler(0);return _tmp376;}
# 1476
{const char*_tmpAD2;Cyc_add_ccarg((struct _dyneither_ptr)Cyc_strconcat(((_tmpAD2="-L",_tag_dyneither(_tmpAD2,sizeof(char),3))),(struct _dyneither_ptr)def_lib_path));}
# 1478
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
const char*_tmpAD3;struct _dyneither_ptr _tmp378=Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs),((_tmpAD3=" ",_tag_dyneither(_tmpAD3,sizeof(char),2))));
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
const char*_tmpAD5;struct Cyc_List_List*_tmpAD4;struct _dyneither_ptr _tmp379=Cyc_str_sepstr(((_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->hd=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),((_tmpAD4->tl=
((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs),_tmpAD4)))))),((_tmpAD5=" ",_tag_dyneither(_tmpAD5,sizeof(char),2))));
# 1484
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
const char*_tmpAD7;const char*_tmpAD6;int _tmp37A=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1489
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAD7=".a",_tag_dyneither(_tmpAD7,sizeof(char),3))))) || 
Cyc_output_file != 0  && Cyc_Filename_check_suffix(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)),((_tmpAD6=".lib",_tag_dyneither(_tmpAD6,sizeof(char),5))));
if(_tmp37A){
stdlib=0;{
const char*_tmpAD8;stdlib_string=(struct _dyneither_ptr)((_tmpAD8="",_tag_dyneither(_tmpAD8,sizeof(char),1)));};}else{
# 1497
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
{const char*_tmpAD9;libcyc_flag=((_tmpAD9="-lcyc_a",_tag_dyneither(_tmpAD9,sizeof(char),8)));}
{const char*_tmpADA;nogc_filename=((_tmpADA="nogc_a.a",_tag_dyneither(_tmpADA,sizeof(char),9)));}{
const char*_tmpADB;runtime_filename=((_tmpADB="runtime_cyc_a.o",_tag_dyneither(_tmpADB,sizeof(char),16)));};}else{
if(Cyc_nocheck_r){
{const char*_tmpADC;libcyc_flag=((_tmpADC="-lcyc_nocheck",_tag_dyneither(_tmpADC,sizeof(char),14)));}
{const char*_tmpADD;nogc_filename=((_tmpADD="nogc.a",_tag_dyneither(_tmpADD,sizeof(char),7)));}{
const char*_tmpADE;runtime_filename=((_tmpADE="runtime_cyc.o",_tag_dyneither(_tmpADE,sizeof(char),14)));};}else{
if(Cyc_pg_r){
{const char*_tmpADF;libcyc_flag=((_tmpADF="-lcyc_pg",_tag_dyneither(_tmpADF,sizeof(char),9)));}
{const char*_tmpAE0;runtime_filename=((_tmpAE0="runtime_cyc_pg.o",_tag_dyneither(_tmpAE0,sizeof(char),17)));}{
const char*_tmpAE1;nogc_filename=((_tmpAE1="nogc_pg.a",_tag_dyneither(_tmpAE1,sizeof(char),10)));};}else{
if(Cyc_Lex_compile_for_boot_r){
# 1515
{const char*_tmpAE2;libcyc_flag=((_tmpAE2="-lcycboot",_tag_dyneither(_tmpAE2,sizeof(char),10)));}
{const char*_tmpAE3;nogc_filename=((_tmpAE3="nogc.a",_tag_dyneither(_tmpAE3,sizeof(char),7)));}{
const char*_tmpAE4;runtime_filename=((_tmpAE4="runtime_cyc.o",_tag_dyneither(_tmpAE4,sizeof(char),14)));};}else{
# 1519
{const char*_tmpAE5;libcyc_flag=((_tmpAE5="-lcyc",_tag_dyneither(_tmpAE5,sizeof(char),6)));}
{const char*_tmpAE6;nogc_filename=((_tmpAE6="nogc.a",_tag_dyneither(_tmpAE6,sizeof(char),7)));}{
const char*_tmpAE7;runtime_filename=((_tmpAE7="runtime_cyc.o",_tag_dyneither(_tmpAE7,sizeof(char),14)));};}}}}
# 1523
{const char*_tmpAE8;gc_filename=((_tmpAE8="gc.a",_tag_dyneither(_tmpAE8,sizeof(char),5)));}{
# 1525
struct _dyneither_ptr _tmp38C=Cyc_nogc_r?
Cyc_do_find(cyclone_arch_path,nogc_filename):
 Cyc_do_find(cyclone_arch_path,gc_filename);
struct _dyneither_ptr _tmp38D=Cyc_do_find(cyclone_arch_path,runtime_filename);
# 1532
stdlib=0;{
const char*_tmpAEE;void*_tmpAED[3];struct Cyc_String_pa_PrintArg_struct _tmpAEC;struct Cyc_String_pa_PrintArg_struct _tmpAEB;struct Cyc_String_pa_PrintArg_struct _tmpAEA;stdlib_string=(struct _dyneither_ptr)((_tmpAEA.tag=0,((_tmpAEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38C),((_tmpAEB.tag=0,((_tmpAEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag),((_tmpAEC.tag=0,((_tmpAEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38D),((_tmpAED[0]=& _tmpAEC,((_tmpAED[1]=& _tmpAEB,((_tmpAED[2]=& _tmpAEA,Cyc_aprintf(((_tmpAEE=" %s %s %s",_tag_dyneither(_tmpAEE,sizeof(char),10))),_tag_dyneither(_tmpAED,sizeof(void*),3))))))))))))))))))));};};}
# 1536
if(Cyc_ic_r){struct _handler_cons _tmp393;_push_handler(& _tmp393);{int _tmp395=0;if(setjmp(_tmp393.handler))_tmp395=1;if(!_tmp395){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp396=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(stdlib,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));
if(!_tmp37A){
struct Cyc_List_List*_tmpAEF;_tmp396=((_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF->hd=Cyc_final_strptr,((_tmpAEF->tl=_tmp396,_tmpAEF))))));}{
# 1544
struct Cyc_Interface_I*_tmp398=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp396,_tmp396);
if(_tmp398 == 0){
{const char*_tmpAF2;void*_tmpAF1;(_tmpAF1=0,Cyc_fprintf(Cyc_stderr,((_tmpAF2="Error: interfaces incompatible\n",_tag_dyneither(_tmpAF2,sizeof(char),32))),_tag_dyneither(_tmpAF1,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp39B=1;_npop_handler(1);return _tmp39B;};}
# 1551
if(_tmp37A){
if(Cyc_output_file != 0){
const char*_tmpAF6;void*_tmpAF5[1];struct Cyc_String_pa_PrintArg_struct _tmpAF4;struct _dyneither_ptr _tmp39C=(_tmpAF4.tag=0,((_tmpAF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpAF5[0]=& _tmpAF4,Cyc_aprintf(((_tmpAF6="%s.cycio",_tag_dyneither(_tmpAF6,sizeof(char),9))),_tag_dyneither(_tmpAF5,sizeof(void*),1)))))));
const char*_tmpAF8;const char*_tmpAF7;struct Cyc___cycFILE*_tmp39D=Cyc_try_file_open((struct _dyneither_ptr)_tmp39C,((_tmpAF7="wb",_tag_dyneither(_tmpAF7,sizeof(char),3))),((_tmpAF8="interface object file",_tag_dyneither(_tmpAF8,sizeof(char),22))));
if(_tmp39D == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp39E=1;_npop_handler(1);return _tmp39E;};}
# 1560
Cyc_Interface_save(_tmp398,_tmp39D);
Cyc_file_close(_tmp39D);}}else{
# 1564
const char*_tmpAFD;const char*_tmpAFC;struct _tuple20*_tmpAFB;if(!Cyc_Interface_is_subinterface(Cyc_Interface_empty(),_tmp398,(
(_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB->f1=((_tmpAFC="empty interface",_tag_dyneither(_tmpAFC,sizeof(char),16))),((_tmpAFB->f2=((_tmpAFD="global interface",_tag_dyneither(_tmpAFD,sizeof(char),17))),_tmpAFB)))))))){
{const char*_tmpB00;void*_tmpAFF;(_tmpAFF=0,Cyc_fprintf(Cyc_stderr,((_tmpB00="Error: some objects are still undefined\n",_tag_dyneither(_tmpB00,sizeof(char),41))),_tag_dyneither(_tmpAFF,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3A9=1;_npop_handler(1);return _tmp3A9;};}}};};
# 1537
;_pop_handler();}else{void*_tmp394=(void*)_exn_thrown;void*_tmp3AB=_tmp394;void*_tmp3AC;void*_tmp3AD;_LL39: _tmp3AC=_tmp3AB;_LL3A:
# 1574
{const char*_tmpB0A;void*_tmpB09[3];const char*_tmpB08;struct Cyc_String_pa_PrintArg_struct _tmpB07;const char*_tmpB06;struct Cyc_String_pa_PrintArg_struct _tmpB05;struct Cyc_Int_pa_PrintArg_struct _tmpB04;(_tmpB04.tag=1,((_tmpB04.f1=(unsigned long)Cyc_Core_get_exn_lineno(),((_tmpB05.tag=0,((_tmpB05.f1=(struct _dyneither_ptr)((_tmpB06=Cyc_Core_get_exn_filename(),_tag_dyneither(_tmpB06,sizeof(char),_get_zero_arr_size_char((void*)_tmpB06,1)))),((_tmpB07.tag=0,((_tmpB07.f1=(struct _dyneither_ptr)((_tmpB08=Cyc_Core_get_exn_name(_tmp3AC),_tag_dyneither(_tmpB08,sizeof(char),_get_zero_arr_size_char((void*)_tmpB08,1)))),((_tmpB09[0]=& _tmpB07,((_tmpB09[1]=& _tmpB05,((_tmpB09[2]=& _tmpB04,Cyc_fprintf(Cyc_stderr,((_tmpB0A="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu",_tag_dyneither(_tmpB0A,sizeof(char),131))),_tag_dyneither(_tmpB09,sizeof(void*),3)))))))))))))))))));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3B5=1;_npop_handler(0);return _tmp3B5;};_LL3B: _tmp3AD=_tmp3AB;_LL3C:(void)_throw(_tmp3AD);_LL38:;}};}{
# 1580
const char*_tmpB0B;struct _dyneither_ptr outfile_str=(_tmpB0B="",_tag_dyneither(_tmpB0B,sizeof(char),1));
if(Cyc_output_file != 0){
const char*_tmpB0F;void*_tmpB0E[1];struct Cyc_String_pa_PrintArg_struct _tmpB0D;outfile_str=(struct _dyneither_ptr)((_tmpB0D.tag=0,((_tmpB0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file)))),((_tmpB0E[0]=& _tmpB0D,Cyc_aprintf(((_tmpB0F=" -o %s",_tag_dyneither(_tmpB0F,sizeof(char),7))),_tag_dyneither(_tmpB0E,sizeof(void*),1))))))));}{
# 1584
const char*_tmpB17;void*_tmpB16[5];struct Cyc_String_pa_PrintArg_struct _tmpB15;struct Cyc_String_pa_PrintArg_struct _tmpB14;struct Cyc_String_pa_PrintArg_struct _tmpB13;struct Cyc_String_pa_PrintArg_struct _tmpB12;struct Cyc_String_pa_PrintArg_struct _tmpB11;struct _dyneither_ptr _tmp3B9=
(_tmpB11.tag=0,((_tmpB11.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp379),((_tmpB12.tag=0,((_tmpB12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string),((_tmpB13.tag=0,((_tmpB13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str),((_tmpB14.tag=0,((_tmpB14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp378),((_tmpB15.tag=0,((_tmpB15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)comp),((_tmpB16[0]=& _tmpB15,((_tmpB16[1]=& _tmpB14,((_tmpB16[2]=& _tmpB13,((_tmpB16[3]=& _tmpB12,((_tmpB16[4]=& _tmpB11,Cyc_aprintf(((_tmpB17="%s %s%s %s%s",_tag_dyneither(_tmpB17,sizeof(char),13))),_tag_dyneither(_tmpB16,sizeof(void*),5)))))))))))))))))))))))))))))));
# 1587
if(Cyc_v_r){{const char*_tmpB1B;void*_tmpB1A[1];struct Cyc_String_pa_PrintArg_struct _tmpB19;(_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B9),((_tmpB1A[0]=& _tmpB19,Cyc_fprintf(Cyc_stderr,((_tmpB1B="%s\n",_tag_dyneither(_tmpB1B,sizeof(char),4))),_tag_dyneither(_tmpB1A,sizeof(void*),1)))))));}Cyc_fflush(Cyc_stderr);}
if(system((const char*)_untag_dyneither_ptr(_tmp3B9,sizeof(char),1))!= 0){
{const char*_tmpB1E;void*_tmpB1D;(_tmpB1D=0,Cyc_fprintf(Cyc_stderr,((_tmpB1E="Error: C compiler failed\n",_tag_dyneither(_tmpB1E,sizeof(char),26))),_tag_dyneither(_tmpB1D,sizeof(void*),0)));}
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp3BF=1;_npop_handler(0);return _tmp3BF;};}
# 1594
Cyc_remove_cfiles();{
# 1596
int _tmp3C0=Cyc_compile_failure?1: 0;_npop_handler(0);return _tmp3C0;};};};};};
# 1107 "cyclone.cyc"
;_pop_region(r);};}
