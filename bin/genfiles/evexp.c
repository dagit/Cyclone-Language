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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
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
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1003 "absyn.h"
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 134 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 71
void*Cyc_Tcutil_compress(void*t);
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 163
int Cyc_Tcutil_typecmp(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);struct _tuple11{union Cyc_Absyn_Cnst f1;int f2;};
# 41 "evexp.cyc"
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 43
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){
union Cyc_Absyn_Cnst _tmp0=cn;enum Cyc_Absyn_Sign _tmp1;char _tmp2;enum Cyc_Absyn_Sign _tmp3;short _tmp4;_LL1: if((_tmp0.Char_c).tag != 2)goto _LL3;_tmp1=((struct _tuple3)(_tmp0.Char_c).val).f1;_tmp2=((struct _tuple3)(_tmp0.Char_c).val).f2;_LL2:
 return Cyc_Absyn_Int_c(_tmp1,(int)_tmp2);_LL3: if((_tmp0.Short_c).tag != 4)goto _LL5;_tmp3=((struct _tuple4)(_tmp0.Short_c).val).f1;_tmp4=((struct _tuple4)(_tmp0.Short_c).val).f2;_LL4:
# 47
 return Cyc_Absyn_Int_c(_tmp3,(int)_tmp4);_LL5:;_LL6:
 return cn;_LL0:;}
# 52
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp5=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp7;int _tmp8;struct _tuple11 _tmp6=_tmp5;_tmp7=_tmp6.f1;_tmp8=_tmp6.f2;
if(!_tmp8){
struct _tuple10 _tmp193;return(_tmp193.f1=0,((_tmp193.f2=0,_tmp193)));}{
union Cyc_Absyn_Cnst _tmpA=Cyc_Evexp_promote_const(_tmp7);union Cyc_Absyn_Cnst _tmpB=_tmpA;int _tmpC;_LL8: if((_tmpB.Int_c).tag != 5)goto _LLA;_tmpC=((struct _tuple5)(_tmpB.Int_c).val).f2;_LL9: {
struct _tuple10 _tmp194;return(_tmp194.f1=(unsigned int)_tmpC,((_tmp194.f2=1,_tmp194)));}_LLA:;_LLB:
{const char*_tmp197;void*_tmp196;(_tmp196=0,Cyc_Tcutil_terr(e->loc,((_tmp197="expecting unsigned int",_tag_dyneither(_tmp197,sizeof(char),23))),_tag_dyneither(_tmp196,sizeof(void*),0)));}{struct _tuple10 _tmp198;return(_tmp198.f1=0,((_tmp198.f2=1,_tmp198)));};_LL7:;};}struct _tuple12{int f1;int f2;};
# 62
static struct _tuple12 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp11=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp13;int _tmp14;struct _tuple11 _tmp12=_tmp11;_tmp13=_tmp12.f1;_tmp14=_tmp12.f2;
if(!_tmp14){
struct _tuple12 _tmp199;return(_tmp199.f1=0,((_tmp199.f2=0,_tmp199)));}{
union Cyc_Absyn_Cnst _tmp16=Cyc_Evexp_promote_const(_tmp13);union Cyc_Absyn_Cnst _tmp17=_tmp16;int _tmp18;long long _tmp19;_LLD: if((_tmp17.Int_c).tag != 5)goto _LLF;_tmp18=((struct _tuple5)(_tmp17.Int_c).val).f2;_LLE: {
struct _tuple12 _tmp19A;return(_tmp19A.f1=_tmp18 != 0,((_tmp19A.f2=1,_tmp19A)));}_LLF: if((_tmp17.LongLong_c).tag != 6)goto _LL11;_tmp19=((struct _tuple6)(_tmp17.LongLong_c).val).f2;_LL10: {
struct _tuple12 _tmp19B;return(_tmp19B.f1=_tmp19 != 0,((_tmp19B.f2=1,_tmp19B)));}_LL11: if((_tmp17.Null_c).tag != 1)goto _LL13;_LL12: {
struct _tuple12 _tmp19C;return(_tmp19C.f1=0,((_tmp19C.f2=0,_tmp19C)));}_LL13:;_LL14:
{const char*_tmp19F;void*_tmp19E;(_tmp19E=0,Cyc_Tcutil_terr(e->loc,((_tmp19F="expecting bool",_tag_dyneither(_tmp19F,sizeof(char),15))),_tag_dyneither(_tmp19E,sizeof(void*),0)));}{struct _tuple12 _tmp1A0;return(_tmp1A0.f1=0,((_tmp1A0.f2=0,_tmp1A0)));};_LLC:;};}struct _tuple13{enum Cyc_Absyn_Primop f1;union Cyc_Absyn_Cnst f2;};
# 74
static struct _tuple11 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
struct _tuple11 _tmp20=Cyc_Evexp_eval_const_exp(e);union Cyc_Absyn_Cnst _tmp22;int _tmp23;struct _tuple11 _tmp21=_tmp20;_tmp22=_tmp21.f1;_tmp23=_tmp21.f2;
if(!_tmp23){
struct _tuple11 _tmp1A1;return(_tmp1A1.f1=_tmp22,((_tmp1A1.f2=0,_tmp1A1)));}
{struct _tuple13 _tmp1A2;struct _tuple13 _tmp25=(_tmp1A2.f1=p,((_tmp1A2.f2=_tmp22,_tmp1A2)));struct _tuple13 _tmp26=_tmp25;enum Cyc_Absyn_Sign _tmp27;int _tmp28;enum Cyc_Absyn_Sign _tmp29;int _tmp2A;int _tmp2B;_LL16: if(_tmp26.f1 != Cyc_Absyn_Plus)goto _LL18;_LL17:
 goto _LL15;_LL18: if(_tmp26.f1 != Cyc_Absyn_Minus)goto _LL1A;if(((_tmp26.f2).Int_c).tag != 5)goto _LL1A;_tmp27=((struct _tuple5)((_tmp26.f2).Int_c).val).f1;_tmp28=((struct _tuple5)((_tmp26.f2).Int_c).val).f2;_LL19:
 _tmp22=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,- _tmp28);goto _LL15;_LL1A: if(_tmp26.f1 != Cyc_Absyn_Bitnot)goto _LL1C;if(((_tmp26.f2).Int_c).tag != 5)goto _LL1C;_tmp29=((struct _tuple5)((_tmp26.f2).Int_c).val).f1;_tmp2A=((struct _tuple5)((_tmp26.f2).Int_c).val).f2;_LL1B:
 _tmp22=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,~ _tmp2A);goto _LL15;_LL1C: if(_tmp26.f1 != Cyc_Absyn_Not)goto _LL1E;if(((_tmp26.f2).Int_c).tag != 5)goto _LL1E;_tmp2B=((struct _tuple5)((_tmp26.f2).Int_c).val).f2;_LL1D:
 _tmp22=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,_tmp2B == 0?1: 0);goto _LL15;_LL1E: if(_tmp26.f1 != Cyc_Absyn_Not)goto _LL20;if(((_tmp26.f2).Null_c).tag != 1)goto _LL20;_LL1F:
 _tmp22=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1);goto _LL15;_LL20:;_LL21:
{const char*_tmp1A5;void*_tmp1A4;(_tmp1A4=0,Cyc_Tcutil_terr(e->loc,((_tmp1A5="bad constant expression",_tag_dyneither(_tmp1A5,sizeof(char),24))),_tag_dyneither(_tmp1A4,sizeof(void*),0)));}goto _LL15;_LL15:;}{
# 86
struct _tuple11 _tmp1A6;return(_tmp1A6.f1=_tmp22,((_tmp1A6.f2=1,_tmp1A6)));};}struct _tuple14{enum Cyc_Absyn_Primop f1;int f2;};
# 90
static struct _tuple11 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple11 _tmp30=Cyc_Evexp_eval_const_exp(e1);union Cyc_Absyn_Cnst _tmp32;int _tmp33;struct _tuple11 _tmp31=_tmp30;_tmp32=_tmp31.f1;_tmp33=_tmp31.f2;{
struct _tuple11 _tmp34=Cyc_Evexp_eval_const_exp(e2);union Cyc_Absyn_Cnst _tmp36;int _tmp37;struct _tuple11 _tmp35=_tmp34;_tmp36=_tmp35.f1;_tmp37=_tmp35.f2;
if(!_tmp33  || !_tmp37){
struct _tuple11 _tmp1A7;return(_tmp1A7.f1=_tmp32,((_tmp1A7.f2=0,_tmp1A7)));}
_tmp32=Cyc_Evexp_promote_const(_tmp32);
_tmp36=Cyc_Evexp_promote_const(_tmp36);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;
{union Cyc_Absyn_Cnst _tmp39=_tmp32;enum Cyc_Absyn_Sign _tmp3A;int _tmp3B;_LL23: if((_tmp39.Int_c).tag != 5)goto _LL25;_tmp3A=((struct _tuple5)(_tmp39.Int_c).val).f1;_tmp3B=((struct _tuple5)(_tmp39.Int_c).val).f2;_LL24:
 s1=_tmp3A;i1=_tmp3B;goto _LL22;_LL25:;_LL26:
{const char*_tmp1AA;void*_tmp1A9;(_tmp1A9=0,Cyc_Tcutil_terr(e1->loc,((_tmp1AA="bad constant expression",_tag_dyneither(_tmp1AA,sizeof(char),24))),_tag_dyneither(_tmp1A9,sizeof(void*),0)));}{struct _tuple11 _tmp1AB;return(_tmp1AB.f1=_tmp32,((_tmp1AB.f2=1,_tmp1AB)));};_LL22:;}
# 103
{union Cyc_Absyn_Cnst _tmp3F=_tmp36;enum Cyc_Absyn_Sign _tmp40;int _tmp41;_LL28: if((_tmp3F.Int_c).tag != 5)goto _LL2A;_tmp40=((struct _tuple5)(_tmp3F.Int_c).val).f1;_tmp41=((struct _tuple5)(_tmp3F.Int_c).val).f2;_LL29:
 s2=_tmp40;i2=_tmp41;goto _LL27;_LL2A:;_LL2B:
{const char*_tmp1AE;void*_tmp1AD;(_tmp1AD=0,Cyc_Tcutil_terr(e2->loc,((_tmp1AE="bad constant expression",_tag_dyneither(_tmp1AE,sizeof(char),24))),_tag_dyneither(_tmp1AD,sizeof(void*),0)));}{struct _tuple11 _tmp1AF;return(_tmp1AF.f1=_tmp32,((_tmp1AF.f2=1,_tmp1AF)));};_LL27:;}
# 107
switch(p){case Cyc_Absyn_Div: _LL2C:
 goto _LL2D;case Cyc_Absyn_Mod: _LL2D:
# 110
 if(i2 == 0){
{const char*_tmp1B2;void*_tmp1B1;(_tmp1B1=0,Cyc_Tcutil_terr(e2->loc,((_tmp1B2="division by zero in constant expression",_tag_dyneither(_tmp1B2,sizeof(char),40))),_tag_dyneither(_tmp1B1,sizeof(void*),0)));}{
struct _tuple11 _tmp1B3;return(_tmp1B3.f1=_tmp32,((_tmp1B3.f2=1,_tmp1B3)));};}
# 114
break;default: _LL2E:
 break;}{
# 117
int has_u_arg=s1 == Cyc_Absyn_Unsigned  || s2 == Cyc_Absyn_Unsigned;
unsigned int u1=(unsigned int)i1;
unsigned int u2=(unsigned int)i2;
int i3=0;
unsigned int u3=0;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;
{struct _tuple14 _tmp1B4;struct _tuple14 _tmp48=(_tmp1B4.f1=p,((_tmp1B4.f2=has_u_arg,_tmp1B4)));struct _tuple14 _tmp49=_tmp48;_LL31: if(_tmp49.f1 != Cyc_Absyn_Plus)goto _LL33;if(_tmp49.f2 != 0)goto _LL33;_LL32:
 i3=i1 + i2;use_i3=1;goto _LL30;_LL33: if(_tmp49.f1 != Cyc_Absyn_Times)goto _LL35;if(_tmp49.f2 != 0)goto _LL35;_LL34:
 i3=i1 * i2;use_i3=1;goto _LL30;_LL35: if(_tmp49.f1 != Cyc_Absyn_Minus)goto _LL37;if(_tmp49.f2 != 0)goto _LL37;_LL36:
 i3=i1 - i2;use_i3=1;goto _LL30;_LL37: if(_tmp49.f1 != Cyc_Absyn_Div)goto _LL39;if(_tmp49.f2 != 0)goto _LL39;_LL38:
 i3=i1 / i2;use_i3=1;goto _LL30;_LL39: if(_tmp49.f1 != Cyc_Absyn_Mod)goto _LL3B;if(_tmp49.f2 != 0)goto _LL3B;_LL3A:
 i3=i1 % i2;use_i3=1;goto _LL30;_LL3B: if(_tmp49.f1 != Cyc_Absyn_Plus)goto _LL3D;if(_tmp49.f2 != 1)goto _LL3D;_LL3C:
 u3=u1 + u2;use_u3=1;goto _LL30;_LL3D: if(_tmp49.f1 != Cyc_Absyn_Times)goto _LL3F;if(_tmp49.f2 != 1)goto _LL3F;_LL3E:
 u3=u1 * u2;use_u3=1;goto _LL30;_LL3F: if(_tmp49.f1 != Cyc_Absyn_Minus)goto _LL41;if(_tmp49.f2 != 1)goto _LL41;_LL40:
 u3=u1 - u2;use_u3=1;goto _LL30;_LL41: if(_tmp49.f1 != Cyc_Absyn_Div)goto _LL43;if(_tmp49.f2 != 1)goto _LL43;_LL42:
 u3=u1 / u2;use_u3=1;goto _LL30;_LL43: if(_tmp49.f1 != Cyc_Absyn_Mod)goto _LL45;if(_tmp49.f2 != 1)goto _LL45;_LL44:
 u3=u1 % u2;use_u3=1;goto _LL30;_LL45: if(_tmp49.f1 != Cyc_Absyn_Eq)goto _LL47;_LL46:
 b3=i1 == i2;use_b3=1;goto _LL30;_LL47: if(_tmp49.f1 != Cyc_Absyn_Neq)goto _LL49;_LL48:
 b3=i1 != i2;use_b3=1;goto _LL30;_LL49: if(_tmp49.f1 != Cyc_Absyn_Gt)goto _LL4B;if(_tmp49.f2 != 0)goto _LL4B;_LL4A:
 b3=i1 > i2;use_b3=1;goto _LL30;_LL4B: if(_tmp49.f1 != Cyc_Absyn_Lt)goto _LL4D;if(_tmp49.f2 != 0)goto _LL4D;_LL4C:
 b3=i1 < i2;use_b3=1;goto _LL30;_LL4D: if(_tmp49.f1 != Cyc_Absyn_Gte)goto _LL4F;if(_tmp49.f2 != 0)goto _LL4F;_LL4E:
 b3=i1 >= i2;use_b3=1;goto _LL30;_LL4F: if(_tmp49.f1 != Cyc_Absyn_Lte)goto _LL51;if(_tmp49.f2 != 0)goto _LL51;_LL50:
 b3=i1 <= i2;use_b3=1;goto _LL30;_LL51: if(_tmp49.f1 != Cyc_Absyn_Gt)goto _LL53;if(_tmp49.f2 != 1)goto _LL53;_LL52:
 b3=u1 > u2;use_b3=1;goto _LL30;_LL53: if(_tmp49.f1 != Cyc_Absyn_Lt)goto _LL55;if(_tmp49.f2 != 1)goto _LL55;_LL54:
 b3=u1 < u2;use_b3=1;goto _LL30;_LL55: if(_tmp49.f1 != Cyc_Absyn_Gte)goto _LL57;if(_tmp49.f2 != 1)goto _LL57;_LL56:
 b3=u1 >= u2;use_b3=1;goto _LL30;_LL57: if(_tmp49.f1 != Cyc_Absyn_Lte)goto _LL59;if(_tmp49.f2 != 1)goto _LL59;_LL58:
 b3=u1 <= u2;use_b3=1;goto _LL30;_LL59: if(_tmp49.f1 != Cyc_Absyn_Bitand)goto _LL5B;_LL5A:
 u3=u1 & u2;use_u3=1;goto _LL30;_LL5B: if(_tmp49.f1 != Cyc_Absyn_Bitor)goto _LL5D;_LL5C:
 u3=u1 | u2;use_u3=1;goto _LL30;_LL5D: if(_tmp49.f1 != Cyc_Absyn_Bitxor)goto _LL5F;_LL5E:
 u3=u1 ^ u2;use_u3=1;goto _LL30;_LL5F: if(_tmp49.f1 != Cyc_Absyn_Bitlshift)goto _LL61;_LL60:
 u3=u1 << u2;use_u3=1;goto _LL30;_LL61: if(_tmp49.f1 != Cyc_Absyn_Bitlrshift)goto _LL63;_LL62:
 u3=u1 >> u2;use_u3=1;goto _LL30;_LL63: if(_tmp49.f1 != Cyc_Absyn_Bitarshift)goto _LL65;_LL64:
{const char*_tmp1B7;void*_tmp1B6;(_tmp1B6=0,Cyc_Tcutil_terr(e1->loc,((_tmp1B7=">>> NOT IMPLEMENTED",_tag_dyneither(_tmp1B7,sizeof(char),20))),_tag_dyneither(_tmp1B6,sizeof(void*),0)));}goto _LL66;_LL65:;_LL66:
{const char*_tmp1BA;void*_tmp1B9;(_tmp1B9=0,Cyc_Tcutil_terr(e1->loc,((_tmp1BA="bad constant expression",_tag_dyneither(_tmp1BA,sizeof(char),24))),_tag_dyneither(_tmp1B9,sizeof(void*),0)));}{struct _tuple11 _tmp1BB;return(_tmp1BB.f1=_tmp32,((_tmp1BB.f2=1,_tmp1BB)));};_LL30:;}
# 155
if(use_i3){struct _tuple11 _tmp1BC;return(_tmp1BC.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,i3),((_tmp1BC.f2=1,_tmp1BC)));}
if(use_u3){struct _tuple11 _tmp1BD;return(_tmp1BD.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,(int)u3),((_tmp1BD.f2=1,_tmp1BD)));}
if(use_b3){struct _tuple11 _tmp1BE;return(_tmp1BE.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,b3?1: 0),((_tmp1BE.f2=1,_tmp1BE)));}{
const char*_tmp1C1;void*_tmp1C0;(_tmp1C0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1C1="Evexp::eval_const_binop",_tag_dyneither(_tmp1C1,sizeof(char),24))),_tag_dyneither(_tmp1C0,sizeof(void*),0)));};};};};}struct _tuple15{void*f1;union Cyc_Absyn_Cnst f2;};
# 164
static struct _tuple11 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){
struct _tuple11 ans;
{void*_tmp55=e->r;void*_tmp56=_tmp55;union Cyc_Absyn_Cnst _tmp59;struct Cyc_Absyn_Exp*_tmp5B;struct Cyc_Absyn_Exp*_tmp5C;struct Cyc_Absyn_Exp*_tmp5D;struct Cyc_Absyn_Exp*_tmp5F;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Exp*_tmp63;enum Cyc_Absyn_Primop _tmp65;struct Cyc_List_List*_tmp66;void*_tmp68;void*_tmp6D;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Enumfield*_tmp70;struct Cyc_Absyn_Enumfield*_tmp72;_LL68: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp57=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp57->tag != 1)goto _LL6A;}_LL69: {
# 168
struct _tuple11 _tmp1C2;return(_tmp1C2.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C2.f2=0,_tmp1C2)));}_LL6A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp58=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp58->tag != 0)goto _LL6C;else{_tmp59=_tmp58->f1;}}_LL6B: {
struct _tuple11 _tmp1C3;return(_tmp1C3.f1=_tmp59,((_tmp1C3.f2=1,_tmp1C3)));}_LL6C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp5A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp5A->tag != 5)goto _LL6E;else{_tmp5B=_tmp5A->f1;_tmp5C=_tmp5A->f2;_tmp5D=_tmp5A->f3;}}_LL6D: {
# 171
struct _tuple12 _tmp75=Cyc_Evexp_eval_const_bool_exp(_tmp5B);int _tmp77;int _tmp78;struct _tuple12 _tmp76=_tmp75;_tmp77=_tmp76.f1;_tmp78=_tmp76.f2;
if(!_tmp78){
Cyc_Evexp_eval_const_exp(_tmp5C);
Cyc_Evexp_eval_const_exp(_tmp5D);{
struct _tuple11 _tmp1C4;return(_tmp1C4.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C4.f2=0,_tmp1C4)));};}
# 177
ans=_tmp77?Cyc_Evexp_eval_const_exp(_tmp5C): Cyc_Evexp_eval_const_exp(_tmp5D);
goto _LL67;}_LL6E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp5E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp5E->tag != 6)goto _LL70;else{_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;}}_LL6F: {
# 180
struct _tuple12 _tmp7A=Cyc_Evexp_eval_const_bool_exp(_tmp5F);int _tmp7C;int _tmp7D;struct _tuple12 _tmp7B=_tmp7A;_tmp7C=_tmp7B.f1;_tmp7D=_tmp7B.f2;
if(!_tmp7D){
Cyc_Evexp_eval_const_exp(_tmp60);{
struct _tuple11 _tmp1C5;return(_tmp1C5.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C5.f2=0,_tmp1C5)));};}
# 185
if(_tmp7C)ans=Cyc_Evexp_eval_const_exp(_tmp60);else{
struct _tuple11 _tmp1C6;ans=((_tmp1C6.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C6.f2=1,_tmp1C6))));}
goto _LL67;}_LL70: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp61=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp61->tag != 7)goto _LL72;else{_tmp62=_tmp61->f1;_tmp63=_tmp61->f2;}}_LL71: {
# 189
struct _tuple12 _tmp80=Cyc_Evexp_eval_const_bool_exp(_tmp62);int _tmp82;int _tmp83;struct _tuple12 _tmp81=_tmp80;_tmp82=_tmp81.f1;_tmp83=_tmp81.f2;
if(!_tmp83){
Cyc_Evexp_eval_const_exp(_tmp63);{
struct _tuple11 _tmp1C7;return(_tmp1C7.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1C7.f2=0,_tmp1C7)));};}
# 194
if(!_tmp82)ans=Cyc_Evexp_eval_const_exp(_tmp63);else{
struct _tuple11 _tmp1C8;ans=((_tmp1C8.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,1),((_tmp1C8.f2=1,_tmp1C8))));}
goto _LL67;}_LL72: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp64=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp64->tag != 2)goto _LL74;else{_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;}}_LL73:
# 198
 if(_tmp66 == 0){
{const char*_tmp1CB;void*_tmp1CA;(_tmp1CA=0,Cyc_Tcutil_terr(e->loc,((_tmp1CB="bad static expression (no args to primop)",_tag_dyneither(_tmp1CB,sizeof(char),42))),_tag_dyneither(_tmp1CA,sizeof(void*),0)));}{
struct _tuple11 _tmp1CC;return(_tmp1CC.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1CC.f2=1,_tmp1CC)));};}
# 202
if(_tmp66->tl == 0){
ans=Cyc_Evexp_eval_const_unprimop(_tmp65,(struct Cyc_Absyn_Exp*)_tmp66->hd);
goto _LL67;}
# 206
if(((struct Cyc_List_List*)_check_null(_tmp66->tl))->tl == 0){
ans=Cyc_Evexp_eval_const_binprimop(_tmp65,(struct Cyc_Absyn_Exp*)_tmp66->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp66->tl))->hd);
goto _LL67;}
# 210
{const char*_tmp1CF;void*_tmp1CE;(_tmp1CE=0,Cyc_Tcutil_terr(e->loc,((_tmp1CF="bad static expression (too many args to primop)",_tag_dyneither(_tmp1CF,sizeof(char),48))),_tag_dyneither(_tmp1CE,sizeof(void*),0)));}{
struct _tuple11 _tmp1D0;return(_tmp1D0.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1D0.f2=1,_tmp1D0)));};_LL74: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp67=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp67->tag != 38)goto _LL76;else{_tmp68=(void*)_tmp67->f1;}}_LL75:
# 214
{void*_tmp8C=Cyc_Tcutil_compress(_tmp68);void*_tmp8D=_tmp8C;struct Cyc_Absyn_Exp*_tmp8F;_LL85: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8D;if(_tmp8E->tag != 18)goto _LL87;else{_tmp8F=_tmp8E->f1;}}_LL86:
 return Cyc_Evexp_eval_const_exp(_tmp8F);_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 218
goto _LL77;_LL76: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp69=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp69->tag != 16)goto _LL78;}_LL77:
 goto _LL79;_LL78: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp6A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp6A->tag != 17)goto _LL7A;}_LL79:
 goto _LL7B;_LL7A: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp6B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp6B->tag != 18)goto _LL7C;}_LL7B:
{struct _tuple11 _tmp1D1;ans=((_tmp1D1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1D1.f2=0,_tmp1D1))));}goto _LL67;_LL7C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp6C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp6C->tag != 13)goto _LL7E;else{_tmp6D=(void*)_tmp6C->f1;_tmp6E=_tmp6C->f2;}}_LL7D:
# 224
 ans=Cyc_Evexp_eval_const_exp(_tmp6E);
if(ans.f2){
struct _tuple15 _tmp1D2;struct _tuple15 _tmp91=(_tmp1D2.f1=Cyc_Tcutil_compress(_tmp6D),((_tmp1D2.f2=ans.f1,_tmp1D2)));struct _tuple15 _tmp92=_tmp91;enum Cyc_Absyn_Sign _tmp94;enum Cyc_Absyn_Size_of _tmp95;enum Cyc_Absyn_Sign _tmp96;char _tmp97;enum Cyc_Absyn_Sign _tmp99;enum Cyc_Absyn_Size_of _tmp9A;enum Cyc_Absyn_Sign _tmp9B;short _tmp9C;enum Cyc_Absyn_Sign _tmp9E;enum Cyc_Absyn_Size_of _tmp9F;enum Cyc_Absyn_Sign _tmpA0;int _tmpA1;void*_tmpA3;enum Cyc_Absyn_Sign _tmpA4;char _tmpA5;void*_tmpA7;enum Cyc_Absyn_Sign _tmpA8;short _tmpA9;void*_tmpAB;enum Cyc_Absyn_Sign _tmpAC;int _tmpAD;_LL8A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92.f1;if(_tmp93->tag != 6)goto _LL8C;else{_tmp94=_tmp93->f1;_tmp95=_tmp93->f2;}}if(((_tmp92.f2).Char_c).tag != 2)goto _LL8C;_tmp96=((struct _tuple3)((_tmp92.f2).Char_c).val).f1;_tmp97=((struct _tuple3)((_tmp92.f2).Char_c).val).f2;_LL8B:
# 228
 _tmp99=_tmp94;_tmp9A=_tmp95;_tmp9B=_tmp96;_tmp9C=(short)_tmp97;goto _LL8D;_LL8C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92.f1;if(_tmp98->tag != 6)goto _LL8E;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}if(((_tmp92.f2).Short_c).tag != 4)goto _LL8E;_tmp9B=((struct _tuple4)((_tmp92.f2).Short_c).val).f1;_tmp9C=((struct _tuple4)((_tmp92.f2).Short_c).val).f2;_LL8D:
 _tmp9E=_tmp99;_tmp9F=_tmp9A;_tmpA0=_tmp9B;_tmpA1=(int)_tmp9C;goto _LL8F;_LL8E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp92.f1;if(_tmp9D->tag != 6)goto _LL90;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}if(((_tmp92.f2).Int_c).tag != 5)goto _LL90;_tmpA0=((struct _tuple5)((_tmp92.f2).Int_c).val).f1;_tmpA1=((struct _tuple5)((_tmp92.f2).Int_c).val).f2;_LL8F:
# 231
 if(_tmp9E != _tmpA0)
ans.f1=Cyc_Absyn_Int_c(_tmp9E,_tmpA1);
goto _LL89;_LL90:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp92.f1;if(_tmpA2->tag != 19)goto _LL92;else{_tmpA3=(void*)_tmpA2->f1;}}if(((_tmp92.f2).Char_c).tag != 2)goto _LL92;_tmpA4=((struct _tuple3)((_tmp92.f2).Char_c).val).f1;_tmpA5=((struct _tuple3)((_tmp92.f2).Char_c).val).f2;_LL91:
# 235
 _tmpA7=_tmpA3;_tmpA8=_tmpA4;_tmpA9=(short)_tmpA5;goto _LL93;_LL92:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp92.f1;if(_tmpA6->tag != 19)goto _LL94;else{_tmpA7=(void*)_tmpA6->f1;}}if(((_tmp92.f2).Short_c).tag != 4)goto _LL94;_tmpA8=((struct _tuple4)((_tmp92.f2).Short_c).val).f1;_tmpA9=((struct _tuple4)((_tmp92.f2).Short_c).val).f2;_LL93:
 _tmpAB=_tmpA7;_tmpAC=_tmpA8;_tmpAD=(int)_tmpA9;goto _LL95;_LL94:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp92.f1;if(_tmpAA->tag != 19)goto _LL96;else{_tmpAB=(void*)_tmpAA->f1;}}if(((_tmp92.f2).Int_c).tag != 5)goto _LL96;_tmpAC=((struct _tuple5)((_tmp92.f2).Int_c).val).f1;_tmpAD=((struct _tuple5)((_tmp92.f2).Int_c).val).f2;_LL95:
# 238
 if(_tmpAD < 0){
const char*_tmp1D5;void*_tmp1D4;(_tmp1D4=0,Cyc_Tcutil_terr(e->loc,((_tmp1D5="cannot cast negative number to a tag type",_tag_dyneither(_tmp1D5,sizeof(char),42))),_tag_dyneither(_tmp1D4,sizeof(void*),0)));}
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1D8;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1D7;Cyc_Tcutil_unify(_tmpAB,(void*)((_tmp1D7=_cycalloc(sizeof(*_tmp1D7)),((_tmp1D7[0]=((_tmp1D8.tag=18,((_tmp1D8.f1=Cyc_Absyn_const_exp(ans.f1,0),_tmp1D8)))),_tmp1D7)))));}
ans.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmpAD);
goto _LL89;_LL96:;_LL97:
{const char*_tmp1DC;void*_tmp1DB[1];struct Cyc_String_pa_PrintArg_struct _tmp1DA;(_tmp1DA.tag=0,((_tmp1DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp6D)),((_tmp1DB[0]=& _tmp1DA,Cyc_Tcutil_terr(e->loc,((_tmp1DC="cannot cast to %s",_tag_dyneither(_tmp1DC,sizeof(char),18))),_tag_dyneither(_tmp1DB,sizeof(void*),1)))))));}goto _LL89;_LL89:;}
# 245
goto _LL67;_LL7E: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp6F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp6F->tag != 32)goto _LL80;else{_tmp70=_tmp6F->f3;}}_LL7F:
 _tmp72=_tmp70;goto _LL81;_LL80: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp71=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp56;if(_tmp71->tag != 31)goto _LL82;else{_tmp72=_tmp71->f3;}}_LL81:
 ans=Cyc_Evexp_eval_const_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_check_null(_tmp72))->tag));goto _LL67;_LL82:;_LL83:
# 249
{const char*_tmp1E0;void*_tmp1DF[1];struct Cyc_String_pa_PrintArg_struct _tmp1DE;(_tmp1DE.tag=0,((_tmp1DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1DF[0]=& _tmp1DE,Cyc_Tcutil_terr(e->loc,((_tmp1E0="bad static expression %s",_tag_dyneither(_tmp1E0,sizeof(char),25))),_tag_dyneither(_tmp1DF,sizeof(void*),1)))))));}{
struct _tuple11 _tmp1E1;return(_tmp1E1.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Signed,0),((_tmp1E1.f2=1,_tmp1E1)));};_LL67:;}
# 252
if(ans.f2){
void*c;
union Cyc_Absyn_Cnst _tmpBA=ans.f1;
{union Cyc_Absyn_Cnst _tmpBB=_tmpBA;_LL99: if((_tmpBB.Int_c).tag != 5)goto _LL9B;if(((struct _tuple5)(_tmpBB.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL9B;if(((struct _tuple5)(_tmpBB.Int_c).val).f2 != 0)goto _LL9B;_LL9A: {
# 257
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
# 259
c=(void*)& uzero;
goto _LL98;}_LL9B: if((_tmpBB.Int_c).tag != 5)goto _LL9D;if(((struct _tuple5)(_tmpBB.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL9D;if(((struct _tuple5)(_tmpBB.Int_c).val).f2 != 1)goto _LL9D;_LL9C: {
# 262
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 264
c=(void*)& uone;
goto _LL98;}_LL9D: if((_tmpBB.Int_c).tag != 5)goto _LL9F;if(((struct _tuple5)(_tmpBB.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL9F;if(((struct _tuple5)(_tmpBB.Int_c).val).f2 != 0)goto _LL9F;_LL9E: {
# 267
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 269
c=(void*)& szero;
goto _LL98;}_LL9F: if((_tmpBB.Int_c).tag != 5)goto _LLA1;if(((struct _tuple5)(_tmpBB.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLA1;if(((struct _tuple5)(_tmpBB.Int_c).val).f2 != 1)goto _LLA1;_LLA0: {
# 272
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
# 274
c=(void*)& sone;
goto _LL98;}_LLA1:;_LLA2:
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1E4;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1E3;c=(void*)((_tmp1E3=_cycalloc(sizeof(*_tmp1E3)),((_tmp1E3[0]=((_tmp1E4.tag=0,((_tmp1E4.f1=_tmpBA,_tmp1E4)))),_tmp1E3))));}goto _LL98;_LL98:;}
# 278
e->r=c;}
# 280
return ans;}
# 283
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){
void*_tmpC2=e->r;void*_tmpC3=_tmpC2;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD3;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_List_List*_tmpD6;struct Cyc_Absyn_Exp*_tmpD9;_LLA4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC4=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC4->tag != 32)goto _LLA6;}_LLA5:
 goto _LLA7;_LLA6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC5=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC5->tag != 31)goto _LLA8;}_LLA7:
 goto _LLA9;_LLA8: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC6=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC6->tag != 16)goto _LLAA;}_LLA9:
 goto _LLAB;_LLAA: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC7=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC7->tag != 17)goto _LLAC;}_LLAB:
 goto _LLAD;_LLAC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC8=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC8->tag != 18)goto _LLAE;}_LLAD:
 goto _LLAF;_LLAE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpC9->tag != 0)goto _LLB0;}_LLAF:
 return 1;_LLB0: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCA=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpCA->tag != 5)goto _LLB2;else{_tmpCB=_tmpCA->f1;_tmpCC=_tmpCA->f2;_tmpCD=_tmpCA->f3;}}_LLB1:
# 292
 return(Cyc_Evexp_c_can_eval(_tmpCB) && Cyc_Evexp_c_can_eval(_tmpCC)) && Cyc_Evexp_c_can_eval(_tmpCD);_LLB2: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpCE=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpCE->tag != 6)goto _LLB4;else{_tmpCF=_tmpCE->f1;_tmpD0=_tmpCE->f2;}}_LLB3:
 _tmpD2=_tmpCF;_tmpD3=_tmpD0;goto _LLB5;_LLB4: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpD1=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpD1->tag != 7)goto _LLB6;else{_tmpD2=_tmpD1->f1;_tmpD3=_tmpD1->f2;}}_LLB5:
# 295
 return Cyc_Evexp_c_can_eval(_tmpD2) && Cyc_Evexp_c_can_eval(_tmpD3);_LLB6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpD4=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpD4->tag != 2)goto _LLB8;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;}}_LLB7:
# 297
 for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_tmpD6->hd))return 0;}
return 1;_LLB8: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpD7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpD7->tag != 38)goto _LLBA;}_LLB9:
 return 0;_LLBA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC3;if(_tmpD8->tag != 13)goto _LLBC;else{_tmpD9=_tmpD8->f2;}}_LLBB:
 return Cyc_Evexp_c_can_eval(_tmpD9);_LLBC:;_LLBD:
 return 0;_LLA3:;}
# 306
static int Cyc_Evexp_const_exp_case_number(struct Cyc_Absyn_Exp*e){
void*_tmpDA=e->r;void*_tmpDB=_tmpDA;_LLBF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDC=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDC->tag != 0)goto _LLC1;}_LLC0:
 return 1;_LLC1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDD=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDD->tag != 5)goto _LLC3;}_LLC2:
 return 2;_LLC3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDE->tag != 2)goto _LLC5;}_LLC4:
 return 3;_LLC5: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDF=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpDF->tag != 16)goto _LLC7;}_LLC6:
 goto _LLC8;_LLC7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpE0=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE0->tag != 17)goto _LLC9;}_LLC8:
 return 4;_LLC9: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE1->tag != 18)goto _LLCB;}_LLCA:
 return 5;_LLCB: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE2->tag != 13)goto _LLCD;}_LLCC:
 return 6;_LLCD: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE3->tag != 6)goto _LLCF;}_LLCE:
 return 7;_LLCF: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE4->tag != 7)goto _LLD1;}_LLD0:
 return 8;_LLD1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpE5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB;if(_tmpE5->tag != 38)goto _LLD3;}_LLD2:
 return 9;_LLD3:;_LLD4:
# 319
{const char*_tmp1E8;void*_tmp1E7[1];struct Cyc_String_pa_PrintArg_struct _tmp1E6;(_tmp1E6.tag=0,((_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1E7[0]=& _tmp1E6,Cyc_Tcutil_terr(e->loc,((_tmp1E8="bad static expression %s",_tag_dyneither(_tmp1E8,sizeof(char),25))),_tag_dyneither(_tmp1E7,sizeof(void*),1)))))));}return 0;_LLBE:;}struct _tuple16{void*f1;void*f2;};
# 323
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmpE9=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmpEB;int _tmpEC;struct _tuple10 _tmpEA=_tmpE9;_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;{
struct _tuple10 _tmpED=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmpEF;int _tmpF0;struct _tuple10 _tmpEE=_tmpED;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;
if(_tmpEC  && _tmpF0)
return(int)(_tmpEB - _tmpEF);{
int _tmpF1=Cyc_Evexp_const_exp_case_number(e1);
int _tmpF2=Cyc_Evexp_const_exp_case_number(e2);
if(_tmpF1 != _tmpF2)
return _tmpF1 - _tmpF2;{
struct _tuple16 _tmp1E9;struct _tuple16 _tmpF3=(_tmp1E9.f1=e1->r,((_tmp1E9.f2=e2->r,_tmp1E9)));struct _tuple16 _tmpF4=_tmpF3;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp108;enum Cyc_Absyn_Primop _tmp10A;struct Cyc_List_List*_tmp10B;enum Cyc_Absyn_Primop _tmp10D;struct Cyc_List_List*_tmp10E;void*_tmp110;void*_tmp112;void*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp118;void*_tmp11A;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;void*_tmp120;struct Cyc_List_List*_tmp121;void*_tmp123;struct Cyc_List_List*_tmp124;void*_tmp126;struct Cyc_Absyn_Exp*_tmp127;void*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;void*_tmp12C;void*_tmp12E;_LLD6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmpF5->tag != 5)goto _LLD8;else{_tmpF6=_tmpF5->f1;_tmpF7=_tmpF5->f2;_tmpF8=_tmpF5->f3;}}{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmpF9->tag != 5)goto _LLD8;else{_tmpFA=_tmpF9->f1;_tmpFB=_tmpF9->f2;_tmpFC=_tmpF9->f3;}};_LLD7: {
# 337
int _tmp12F=Cyc_Evexp_const_exp_cmp(_tmpF8,_tmpFC);
if(_tmp12F != 0)return _tmp12F;
_tmpFE=_tmpF6;_tmpFF=_tmpF7;_tmp101=_tmpFA;_tmp102=_tmpFB;goto _LLD9;}_LLD8:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmpFD->tag != 6)goto _LLDA;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;}}{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp100->tag != 6)goto _LLDA;else{_tmp101=_tmp100->f1;_tmp102=_tmp100->f2;}};_LLD9:
 _tmp104=_tmpFE;_tmp105=_tmpFF;_tmp107=_tmp101;_tmp108=_tmp102;goto _LLDB;_LLDA:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp103=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp103->tag != 7)goto _LLDC;else{_tmp104=_tmp103->f1;_tmp105=_tmp103->f2;}}{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp106->tag != 7)goto _LLDC;else{_tmp107=_tmp106->f1;_tmp108=_tmp106->f2;}};_LLDB: {
# 342
int _tmp130=Cyc_Evexp_const_exp_cmp(_tmp104,_tmp107);
if(_tmp130 != 0)return _tmp130;
return Cyc_Evexp_const_exp_cmp(_tmp105,_tmp108);}_LLDC:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp109=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp109->tag != 2)goto _LLDE;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp10C->tag != 2)goto _LLDE;else{_tmp10D=_tmp10C->f1;_tmp10E=_tmp10C->f2;}};_LLDD: {
# 346
int _tmp131=(int)_tmp10A - (int)_tmp10D;
if(_tmp131 != 0)return _tmp131;
for(0;_tmp10B != 0  && _tmp10E != 0;(_tmp10B=_tmp10B->tl,_tmp10E=_tmp10E->tl)){
int _tmp132=Cyc_Evexp_const_exp_cmp((struct Cyc_Absyn_Exp*)_tmp10B->hd,(struct Cyc_Absyn_Exp*)_tmp10E->hd);
if(_tmp132 != 0)
return _tmp132;}
# 353
return 0;}_LLDE:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp10F->tag != 16)goto _LLE0;else{_tmp110=(void*)_tmp10F->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp111->tag != 16)goto _LLE0;else{_tmp112=(void*)_tmp111->f1;}};_LLDF:
 return Cyc_Tcutil_typecmp(_tmp110,_tmp112);_LLE0:{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp113=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp113->tag != 16)goto _LLE2;else{_tmp114=(void*)_tmp113->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp115->tag != 17)goto _LLE2;else{_tmp116=_tmp115->f1;}};_LLE1: {
# 356
void*_tmp133=_tmp116->topt;
if(_tmp133 == 0){
const char*_tmp1EC;void*_tmp1EB;(_tmp1EB=0,Cyc_Tcutil_terr(e2->loc,((_tmp1EC="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1EC,sizeof(char),51))),_tag_dyneither(_tmp1EB,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp(_tmp114,(void*)_check_null(_tmp133));}_LLE2:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp117->tag != 17)goto _LLE4;else{_tmp118=_tmp117->f1;}}{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp119->tag != 16)goto _LLE4;else{_tmp11A=(void*)_tmp119->f1;}};_LLE3: {
# 361
void*_tmp136=_tmp118->topt;
if(_tmp136 == 0){
const char*_tmp1EF;void*_tmp1EE;(_tmp1EE=0,Cyc_Tcutil_terr(e1->loc,((_tmp1EF="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1EF,sizeof(char),51))),_tag_dyneither(_tmp1EE,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp136),_tmp11A);}_LLE4:{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp11B->tag != 17)goto _LLE6;else{_tmp11C=_tmp11B->f1;}}{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp11D->tag != 17)goto _LLE6;else{_tmp11E=_tmp11D->f1;}};_LLE5: {
# 366
void*_tmp139=_tmp11C->topt;
void*_tmp13A=_tmp11E->topt;
if(_tmp139 == 0){
const char*_tmp1F2;void*_tmp1F1;(_tmp1F1=0,Cyc_Tcutil_terr(e1->loc,((_tmp1F2="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1F2,sizeof(char),51))),_tag_dyneither(_tmp1F1,sizeof(void*),0)));}
if(_tmp13A == 0){
const char*_tmp1F5;void*_tmp1F4;(_tmp1F4=0,Cyc_Tcutil_terr(e2->loc,((_tmp1F5="cannot handle sizeof(exp) here -- use sizeof(type)",_tag_dyneither(_tmp1F5,sizeof(char),51))),_tag_dyneither(_tmp1F4,sizeof(void*),0)));}
return Cyc_Tcutil_typecmp((void*)_check_null(_tmp139),(void*)_check_null(_tmp13A));}_LLE6:{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp11F->tag != 18)goto _LLE8;else{_tmp120=(void*)_tmp11F->f1;_tmp121=_tmp11F->f2;}}{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp122->tag != 18)goto _LLE8;else{_tmp123=(void*)_tmp122->f1;_tmp124=_tmp122->f2;}};_LLE7: {
# 374
int _tmp13F=Cyc_Tcutil_typecmp(_tmp120,_tmp123);
if(_tmp13F != 0)return _tmp13F;{
int _tmp140=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp121);
int _tmp141=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp124);
if(_tmp140 < _tmp141)return - 1;
if(_tmp141 < _tmp140)return 1;
for(0;_tmp121 != 0  && _tmp124 != 0;(_tmp121=_tmp121->tl,_tmp124=_tmp124->tl)){
struct _tuple16 _tmp1F6;struct _tuple16 _tmp142=(_tmp1F6.f1=(void*)_tmp121->hd,((_tmp1F6.f2=(void*)_tmp124->hd,_tmp1F6)));struct _tuple16 _tmp143=_tmp142;struct _dyneither_ptr*_tmp149;struct _dyneither_ptr*_tmp14B;unsigned int _tmp14D;unsigned int _tmp14F;_LLEF:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp144=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp143.f1;if(_tmp144->tag != 0)goto _LLF1;}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp145=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp143.f2;if(_tmp145->tag != 1)goto _LLF1;};_LLF0:
 return - 1;_LLF1:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp146=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp143.f1;if(_tmp146->tag != 1)goto _LLF3;}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp147=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp143.f2;if(_tmp147->tag != 0)goto _LLF3;};_LLF2:
 return 1;_LLF3:{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp148=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp143.f1;if(_tmp148->tag != 0)goto _LLF5;else{_tmp149=_tmp148->f1;}}{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp14A=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp143.f2;if(_tmp14A->tag != 0)goto _LLF5;else{_tmp14B=_tmp14A->f1;}};_LLF4: {
# 385
int c=Cyc_strptrcmp(_tmp149,_tmp14B);
if(c != 0)return c;
goto _LLEE;}_LLF5:{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp14C=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp143.f1;if(_tmp14C->tag != 1)goto _LLEE;else{_tmp14D=_tmp14C->f1;}}{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp14E=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp143.f2;if(_tmp14E->tag != 1)goto _LLEE;else{_tmp14F=_tmp14E->f1;}};_LLF6: {
# 389
int c=(int)(_tmp14D - _tmp14F);
if(c != 0)return c;
goto _LLEE;}_LLEE:;}
# 393
return 0;};}_LLE8:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp125->tag != 13)goto _LLEA;else{_tmp126=(void*)_tmp125->f1;_tmp127=_tmp125->f2;}}{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp128->tag != 13)goto _LLEA;else{_tmp129=(void*)_tmp128->f1;_tmp12A=_tmp128->f2;}};_LLE9: {
# 395
int _tmp151=Cyc_Tcutil_typecmp(_tmp126,_tmp129);
if(_tmp151 != 0)return _tmp151;
return Cyc_Evexp_const_exp_cmp(_tmp127,_tmp12A);}_LLEA:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF4.f1;if(_tmp12B->tag != 38)goto _LLEC;else{_tmp12C=(void*)_tmp12B->f1;}}{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp12D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpF4.f2;if(_tmp12D->tag != 38)goto _LLEC;else{_tmp12E=(void*)_tmp12D->f1;}};_LLEB:
# 399
 if(Cyc_Tcutil_unify(_tmp12C,_tmp12E))return 0;
return Cyc_Tcutil_typecmp(_tmp12C,_tmp12E);_LLEC:;_LLED: {
const char*_tmp1F9;void*_tmp1F8;(_tmp1F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1F9="Evexp::const_exp_cmp, unexpected case",_tag_dyneither(_tmp1F9,sizeof(char),38))),_tag_dyneither(_tmp1F8,sizeof(void*),0)));}_LLD5:;};};};}
# 404
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return Cyc_Evexp_const_exp_cmp(e1,e2)== 0;}
# 407
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple10 _tmp155=Cyc_Evexp_eval_const_uint_exp(e1);unsigned int _tmp157;int _tmp158;struct _tuple10 _tmp156=_tmp155;_tmp157=_tmp156.f1;_tmp158=_tmp156.f2;{
struct _tuple10 _tmp159=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp15B;int _tmp15C;struct _tuple10 _tmp15A=_tmp159;_tmp15B=_tmp15A.f1;_tmp15C=_tmp15A.f2;
if(_tmp158  && _tmp15C)
return _tmp157 <= _tmp15B;
return Cyc_Evexp_same_const_exp(e1,e2);};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 417
int Cyc_Evexp_okay_szofarg(void*t){
void*_tmp15D=Cyc_Tcutil_compress(t);void*_tmp15E=_tmp15D;struct Cyc_Absyn_Tvar*_tmp161;struct Cyc_Absyn_Datatypefield*_tmp166;struct Cyc_List_List*_tmp168;void*_tmp16F;struct Cyc_Absyn_Exp*_tmp170;union Cyc_Absyn_AggrInfoU _tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Typedefdecl*_tmp184;_LLF8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp15E;if(_tmp15F->tag != 0)goto _LLFA;}_LLF9:
 return 0;_LLFA: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp15E;if(_tmp160->tag != 2)goto _LLFC;else{_tmp161=_tmp160->f1;}}_LLFB: {
# 421
enum Cyc_Absyn_KindQual _tmp185=(Cyc_Tcutil_tvar_kind(_tmp161,& Cyc_Tcutil_bk))->kind;switch(_tmp185){case Cyc_Absyn_BoxKind: _LL132:
 return 1;default: _LL133:
 return 0;}}_LLFC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15E;if(_tmp162->tag != 1)goto _LLFE;else{if(_tmp162->f1 == 0)goto _LLFE;if(((struct Cyc_Absyn_Kind*)(_tmp162->f1)->v)->kind != Cyc_Absyn_BoxKind)goto _LLFE;}}_LLFD:
# 425
 return 1;_LLFE: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15E;if(_tmp163->tag != 1)goto _LL100;}_LLFF:
 return 0;_LL100: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp164=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp15E;if(_tmp164->tag != 3)goto _LL102;}_LL101:
 return 1;_LL102: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15E;if(_tmp165->tag != 4)goto _LL104;else{if((((_tmp165->f1).field_info).KnownDatatypefield).tag != 2)goto _LL104;_tmp166=((struct _tuple1)(((_tmp165->f1).field_info).KnownDatatypefield).val).f2;}}_LL103:
# 429
 _tmp168=_tmp166->typs;goto _LL105;_LL104: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp15E;if(_tmp167->tag != 10)goto _LL106;else{_tmp168=_tmp167->f1;}}_LL105:
# 431
 for(0;_tmp168 != 0;_tmp168=_tmp168->tl){
if(!Cyc_Evexp_okay_szofarg((*((struct _tuple17*)_tmp168->hd)).f2))
return 0;}
return 1;_LL106: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp15E;if(_tmp169->tag != 4)goto _LL108;}_LL107:
# 436
 return 0;_LL108: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15E;if(_tmp16A->tag != 5)goto _LL10A;}_LL109:
# 439
 return 1;_LL10A: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp15E;if(_tmp16B->tag != 19)goto _LL10C;}_LL10B:
# 441
 goto _LL10D;_LL10C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp15E;if(_tmp16C->tag != 6)goto _LL10E;}_LL10D:
 goto _LL10F;_LL10E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp15E;if(_tmp16D->tag != 7)goto _LL110;}_LL10F:
 return 1;_LL110: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15E;if(_tmp16E->tag != 8)goto _LL112;else{_tmp16F=(_tmp16E->f1).elt_type;_tmp170=(_tmp16E->f1).num_elts;}}_LL111:
# 445
 return _tmp170 != 0;_LL112: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15E;if(_tmp171->tag != 9)goto _LL114;}_LL113:
 return 0;_LL114: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15E;if(_tmp172->tag != 15)goto _LL116;}_LL115:
 return 1;_LL116: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp15E;if(_tmp173->tag != 16)goto _LL118;}_LL117:
 return 1;_LL118: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp15E;if(_tmp174->tag != 11)goto _LL11A;else{_tmp175=(_tmp174->f1).aggr_info;_tmp176=(_tmp174->f1).targs;}}_LL119: {
# 450
struct Cyc_Absyn_Aggrdecl*ad;
{union Cyc_Absyn_AggrInfoU _tmp186=_tmp175;struct Cyc_Absyn_Aggrdecl*_tmp187;_LL136: if((_tmp186.UnknownAggr).tag != 1)goto _LL138;_LL137: {
const char*_tmp1FC;void*_tmp1FB;(_tmp1FB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1FC="szof on unchecked StructType",_tag_dyneither(_tmp1FC,sizeof(char),29))),_tag_dyneither(_tmp1FB,sizeof(void*),0)));}_LL138: if((_tmp186.KnownAggr).tag != 2)goto _LL135;_tmp187=*((struct Cyc_Absyn_Aggrdecl**)(_tmp186.KnownAggr).val);_LL139:
 ad=_tmp187;_LL135:;}
# 455
if(ad->impl == 0)
return 0;
{
struct _RegionHandle _tmp18A=_new_region("temp");struct _RegionHandle*temp=& _tmp18A;_push_region(temp);
{struct Cyc_List_List*_tmp18B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,_tmp176);
{struct Cyc_List_List*_tmp18C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp18C != 0;_tmp18C=_tmp18C->tl){
if(!Cyc_Evexp_okay_szofarg(Cyc_Tcutil_rsubstitute(temp,_tmp18B,((struct Cyc_Absyn_Aggrfield*)_tmp18C->hd)->type))){int _tmp18D=0;_npop_handler(0);return _tmp18D;}}}{
int _tmp18E=1;_npop_handler(0);return _tmp18E;};}
# 459
;_pop_region(temp);}
# 464
_tmp178=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(((struct Cyc_Absyn_Aggrdecl*)_check_null(ad))->impl))->fields;goto _LL11B;}_LL11A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp15E;if(_tmp177->tag != 12)goto _LL11C;else{_tmp178=_tmp177->f2;}}_LL11B:
# 466
 for(0;_tmp178 != 0;_tmp178=_tmp178->tl){
if(!Cyc_Evexp_okay_szofarg(((struct Cyc_Absyn_Aggrfield*)_tmp178->hd)->type))
return 0;}
return 1;_LL11C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp15E;if(_tmp179->tag != 13)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp15E;if(_tmp17A->tag != 14)goto _LL120;}_LL11F:
 return 1;_LL120: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15E;if(_tmp17B->tag != 26)goto _LL122;}_LL121:
 goto _LL123;_LL122: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp15E;if(_tmp17C->tag != 20)goto _LL124;}_LL123:
 goto _LL125;_LL124: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp15E;if(_tmp17D->tag != 22)goto _LL126;}_LL125:
 goto _LL127;_LL126: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp15E;if(_tmp17E->tag != 21)goto _LL128;}_LL127:
 goto _LL129;_LL128: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp15E;if(_tmp17F->tag != 24)goto _LL12A;}_LL129:
 goto _LL12B;_LL12A: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp15E;if(_tmp180->tag != 23)goto _LL12C;}_LL12B:
 goto _LL12D;_LL12C: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp15E;if(_tmp181->tag != 18)goto _LL12E;}_LL12D:
 goto _LL12F;_LL12E: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp15E;if(_tmp182->tag != 25)goto _LL130;}_LL12F:
 return 0;_LL130: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15E;if(_tmp183->tag != 17)goto _LLF7;else{_tmp184=_tmp183->f3;}}_LL131:
# 482
 if(_tmp184 == 0  || _tmp184->kind == 0){
struct Cyc_String_pa_PrintArg_struct _tmp204;void*_tmp203[1];const char*_tmp202;void*_tmp201;(_tmp201=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmp204.tag=0,((_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp203[0]=& _tmp204,Cyc_aprintf(((_tmp202="szof typedeftype %s",_tag_dyneither(_tmp202,sizeof(char),20))),_tag_dyneither(_tmp203,sizeof(void*),1)))))))),_tag_dyneither(_tmp201,sizeof(void*),0)));}
return((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp184->kind))->v)->kind == Cyc_Absyn_BoxKind;_LLF7:;}
