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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 148 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 64
void*Cyc_List_hd(struct Cyc_List_List*x);
# 67
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 100
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 104
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 112
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 117
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x);
# 120
void Cyc_List_app_c(void*(*f)(void*,void*),void*,struct Cyc_List_List*x);
# 124
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 129
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 139
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 141
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 145
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x);
# 149
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*,void*accum,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 157
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*,struct Cyc_List_List*x,void*accum);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 167
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 201
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 214
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 220
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 227
struct Cyc_List_List*Cyc_List_merge(int(*cmp)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 230
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);
# 234
struct Cyc_List_List*Cyc_List_imp_merge(int(*cmp)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 254
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 281
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);
# 288
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 298
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x);
# 303
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 310
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 328
void*Cyc_List_assoc(struct Cyc_List_List*l,void*k);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 342
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x);
# 345
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 349
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x);
# 354
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 359
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x);
# 364
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x);
# 367
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r,struct Cyc_List_List*x);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr);
# 374
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr);
# 378
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int));
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 397
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*x);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 32 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){
int i=0;
# 35
while(x != 0){
++ i;
x=x->tl;}
# 39
return i;}
# 43
void*Cyc_List_hd(struct Cyc_List_List*x){
if(x == 0){struct Cyc_Core_Failure_exn_struct _tmp47;const char*_tmp46;struct Cyc_Core_Failure_exn_struct*_tmp45;(int)_throw((void*)((_tmp45=_cycalloc(sizeof(*_tmp45)),((_tmp45[0]=((_tmp47.tag=Cyc_Core_Failure,((_tmp47.f1=((_tmp46="hd",_tag_dyneither(_tmp46,sizeof(char),3))),_tmp47)))),_tmp45)))));}
return x->hd;}
# 49
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x){
if(x == 0){struct Cyc_Core_Failure_exn_struct _tmp4D;const char*_tmp4C;struct Cyc_Core_Failure_exn_struct*_tmp4B;(int)_throw((void*)((_tmp4B=_cycalloc(sizeof(*_tmp4B)),((_tmp4B[0]=((_tmp4D.tag=Cyc_Core_Failure,((_tmp4D.f1=((_tmp4C="tl",_tag_dyneither(_tmp4C,sizeof(char),3))),_tmp4D)))),_tmp4B)))));}
return x->tl;}
# 56
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*r,struct _dyneither_ptr argv){
struct Cyc_List_List*result=0;
{int i=(int)(_get_dyneither_size(argv,sizeof(void*))- 1);for(0;i >= 0;-- i){
struct Cyc_List_List*_tmp4E;result=((_tmp4E=_region_malloc(r,sizeof(*_tmp4E)),((_tmp4E->hd=*((void**)_check_dyneither_subscript(argv,sizeof(void*),i)),((_tmp4E->tl=result,_tmp4E))))));}}
return result;}
# 65
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr argv){
struct Cyc_List_List*result=0;
{int i=(int)(_get_dyneither_size(argv,sizeof(void*))- 1);for(0;i >= 0;-- i){
struct Cyc_List_List*_tmp4F;result=((_tmp4F=_cycalloc(sizeof(*_tmp4F)),((_tmp4F->hd=*((void**)_check_dyneither_subscript(argv,sizeof(void*),i)),((_tmp4F->tl=result,_tmp4F))))));}}
return result;}
# 73
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r2,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 76
if(x == 0)return 0;
{struct Cyc_List_List*_tmp50;result=((_tmp50=_region_malloc(r2,sizeof(*_tmp50)),((_tmp50->hd=x->hd,((_tmp50->tl=0,_tmp50))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*_tmp51;struct Cyc_List_List*temp=(_tmp51=_region_malloc(r2,sizeof(*_tmp51)),((_tmp51->hd=x->hd,((_tmp51->tl=0,_tmp51)))));
prev->tl=temp;
prev=temp;}
# 84
return result;}
# 87
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){
return Cyc_List_rcopy(Cyc_Core_heap_region,x);}
# 92
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*r2,void*(*f)(void*),struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 95
if(x == 0)return 0;
{struct Cyc_List_List*_tmp52;result=((_tmp52=_region_malloc(r2,sizeof(*_tmp52)),((_tmp52->hd=f(x->hd),((_tmp52->tl=0,_tmp52))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*_tmp53;struct Cyc_List_List*temp=(_tmp53=_region_malloc(r2,sizeof(*_tmp53)),((_tmp53->hd=f(x->hd),((_tmp53->tl=0,_tmp53)))));
prev->tl=temp;
prev=temp;}
# 103
return result;}
# 106
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){
return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}
# 111
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 114
if(x == 0)return 0;
{struct Cyc_List_List*_tmp54;result=((_tmp54=_region_malloc(r2,sizeof(*_tmp54)),((_tmp54->hd=f(env,x->hd),((_tmp54->tl=0,_tmp54))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*_tmp55;struct Cyc_List_List*e=(_tmp55=_region_malloc(r2,sizeof(*_tmp55)),((_tmp55->hd=f(env,x->hd),((_tmp55->tl=0,_tmp55)))));
prev->tl=e;
prev=e;}
# 122
return result;}
# 125
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char Cyc_List_List_mismatch[14]="List_mismatch";
# 131
struct Cyc_List_List_mismatch_exn_struct Cyc_List_List_mismatch_val={Cyc_List_List_mismatch};
# 136
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*r3,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
# 138
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 140
if(x == 0  && y == 0)return 0;
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 143
{struct Cyc_List_List*_tmp56;result=((_tmp56=_region_malloc(r3,sizeof(*_tmp56)),((_tmp56->hd=f(x->hd,y->hd),((_tmp56->tl=0,_tmp56))))));}
prev=result;
# 146
x=x->tl;
y=y->tl;
# 149
while(x != 0  && y != 0){
struct Cyc_List_List*_tmp57;struct Cyc_List_List*temp=(_tmp57=_region_malloc(r3,sizeof(*_tmp57)),((_tmp57->hd=f(x->hd,y->hd),((_tmp57->tl=0,_tmp57)))));
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 156
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 160
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}
# 165
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*r3,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 167
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 169
if((x == 0  && y == 0) && z == 0)return 0;
if((x == 0  || y == 0) || z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 172
{struct Cyc_List_List*_tmp58;result=((_tmp58=_region_malloc(r3,sizeof(*_tmp58)),((_tmp58->hd=f(x->hd,y->hd,z->hd),((_tmp58->tl=0,_tmp58))))));}
prev=result;
# 175
x=x->tl;
y=y->tl;
z=z->tl;
# 179
while((x != 0  && y != 0) && z != 0){
struct Cyc_List_List*_tmp59;struct Cyc_List_List*temp=(_tmp59=_region_malloc(r3,sizeof(*_tmp59)),((_tmp59->hd=f(x->hd,y->hd,z->hd),((_tmp59->tl=0,_tmp59)))));
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 187
if((x != 0  || y != 0) || z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 191
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rmap3(Cyc_Core_heap_region,f,x,y,z);}
# 198
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){
while(x != 0){
f(x->hd);
x=x->tl;}}
# 205
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0){
f(env,x->hd);
x=x->tl;}}
# 215
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){
while(x != 0){
f(x->hd);
x=x->tl;}}
# 222
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0){
f(env,x->hd);
x=x->tl;}}
# 231
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 237
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 240
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(env,x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 246
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 251
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 257
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 259
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
while(x != 0  && y != 0){
f(env,x->hd,y->hd);
x=x->tl;
y=y->tl;}
# 265
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 271
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){
while(x != 0){
accum=f(accum,x->hd);
x=x->tl;}
# 276
return accum;}
# 279
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){
while(x != 0){
accum=f(env,accum,x->hd);
x=x->tl;}
# 284
return accum;}
# 290
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;else{
return f(x->hd,Cyc_List_fold_right(f,x->tl,accum));}}
# 294
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;else{
return f(env,x->hd,Cyc_List_fold_right_c(f,env,x->tl,accum));}}
# 301
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 303
while(x != 0){
{struct Cyc_List_List*_tmp5A;y=((_tmp5A=_region_malloc(r2,sizeof(*_tmp5A)),((_tmp5A->hd=x->hd,((_tmp5A->tl=y,_tmp5A))))));}
x=x->tl;}
# 307
return y;}
# 310
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
# 315
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
# 317
if(x == 0)
return 0;
return Cyc_List_rrevappend(r2,x,0);}
# 322
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){
return Cyc_List_rrev(Cyc_Core_heap_region,x);}
# 327
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){
if(x == 0)return x;else{
# 330
struct Cyc_List_List*first=x;
struct Cyc_List_List*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_List_List*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 339
return first;}}
# 344
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 347
if(x == 0)return y;
if(y == 0)return Cyc_List_rcopy(r2,x);
{struct Cyc_List_List*_tmp5B;result=((_tmp5B=_region_malloc(r2,sizeof(*_tmp5B)),((_tmp5B->hd=x->hd,((_tmp5B->tl=0,_tmp5B))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*_tmp5C;struct Cyc_List_List*temp=(_tmp5C=_region_malloc(r2,sizeof(*_tmp5C)),((_tmp5C->hd=x->hd,((_tmp5C->tl=0,_tmp5C)))));
prev->tl=temp;
prev=temp;}
# 356
prev->tl=y;
return result;}
# 360
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rappend(Cyc_Core_heap_region,x,y);}
# 367
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*z;
# 370
if(x == 0)return y;
if(y == 0)return x;
for(z=x;z->tl != 0;z=z->tl){
# 374
;}
z->tl=y;
return x;}
# 380
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r3,struct Cyc_List_List*x){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_List_List*x,struct Cyc_List_List*accum))Cyc_List_fold_right_c)(Cyc_List_rappend,r3,x,0);}
# 384
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){
return Cyc_List_rflatten(Cyc_Core_heap_region,x);}
# 389
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 392
if(a == 0)return b;
if(b == 0)return a;
# 399
if(less_eq(a->hd,b->hd)<= 0){
c=a;
a=a->tl;}else{
# 403
c=b;
b=b->tl;}
# 406
d=c;
# 408
while(a != 0  && b != 0){
# 410
if(less_eq(a->hd,b->hd)<= 0){
c->tl=a;
c=a;
a=a->tl;}else{
# 415
c->tl=b;
c=b;
b=b->tl;}}
# 421
if(a == 0)
c->tl=b;else{
# 424
c->tl=a;}
return d;}
# 429
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
if(x == 0)return x;{
struct Cyc_List_List*temp=x->tl;
if(temp == 0)return x;{
# 435
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;
struct Cyc_List_List*b=temp;struct Cyc_List_List*bptr=b;
x=b->tl;
while(x != 0){
aptr->tl=x;
aptr=x;
x=x->tl;
if(x != 0){
bptr->tl=x;
bptr=x;
x=x->tl;}}
# 448
aptr->tl=0;
bptr->tl=0;
return Cyc_List_imp_merge(less_eq,
Cyc_List_rimp_merge_sort(less_eq,a),Cyc_List_rimp_merge_sort(less_eq,b));};};}
# 458
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r2,int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return Cyc_List_rimp_merge_sort(less_eq,Cyc_List_rcopy(r2,x));}
# 463
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r3,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 467
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 470
if(a == 0)return Cyc_List_rcopy(r3,b);
if(b == 0)return Cyc_List_rcopy(r3,a);
# 477
if(less_eq(a->hd,b->hd)<= 0){
{struct Cyc_List_List*_tmp5D;c=((_tmp5D=_region_malloc(r3,sizeof(*_tmp5D)),((_tmp5D->hd=a->hd,((_tmp5D->tl=0,_tmp5D))))));}
a=a->tl;}else{
# 481
{struct Cyc_List_List*_tmp5E;c=((_tmp5E=_region_malloc(r3,sizeof(*_tmp5E)),((_tmp5E->hd=b->hd,((_tmp5E->tl=0,_tmp5E))))));}
b=b->tl;}
# 484
d=c;
# 486
while(a != 0  && b != 0){
# 488
struct Cyc_List_List*temp;
if(less_eq(a->hd,b->hd)<= 0){
{struct Cyc_List_List*_tmp5F;temp=((_tmp5F=_region_malloc(r3,sizeof(*_tmp5F)),((_tmp5F->hd=a->hd,((_tmp5F->tl=0,_tmp5F))))));}
c->tl=temp;
c=temp;
a=a->tl;}else{
# 495
{struct Cyc_List_List*_tmp60;temp=((_tmp60=_region_malloc(r3,sizeof(*_tmp60)),((_tmp60->hd=b->hd,((_tmp60->tl=0,_tmp60))))));}
c->tl=temp;
c=temp;
b=b->tl;}}
# 502
if(a == 0)
c->tl=Cyc_List_rcopy(r3,b);else{
# 505
c->tl=Cyc_List_rcopy(r3,a);}
return d;}
# 509
struct Cyc_List_List*Cyc_List_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return Cyc_List_rmerge_sort(Cyc_Core_heap_region,less_eq,x);}
# 513
struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
return Cyc_List_rmerge(Cyc_Core_heap_region,less_eq,a,b);}char Cyc_List_Nth[4]="Nth";
# 519
struct Cyc_List_Nth_exn_struct Cyc_List_Nth_val={Cyc_List_Nth};
# 524
void*Cyc_List_nth(struct Cyc_List_List*x,int i){
# 526
while(i > 0  && x != 0){
-- i;
x=x->tl;}
# 530
if(i < 0  || x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
return x->hd;}
# 536
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){
if(i < 0)(int)_throw((void*)& Cyc_List_Nth_val);
while(i != 0){
if(x == 0)(int)_throw((void*)& Cyc_List_Nth_val);
x=x->tl;
-- i;}
# 543
return x;}
# 548
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){
while(x != 0  && pred(x->hd)){x=x->tl;}
return x == 0;}
# 552
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0  && pred(env,x->hd)){x=x->tl;}
return x == 0;}
# 559
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){
while(x != 0  && !pred(x->hd)){x=x->tl;}
return x != 0;}
# 563
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
while(x != 0  && !pred(env,x->hd)){x=x->tl;}
return x != 0;}
# 568
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
void*v=0;
for(0;x != 0;x=x->tl){
v=pred(env,x->hd);
if((unsigned int)v)break;}
# 574
return v;}struct _tuple2{void*f1;void*f2;};
# 579
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 581
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 583
if(x == 0  && y == 0)return 0;
if(x == 0  || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 586
{struct _tuple2*_tmp63;struct Cyc_List_List*_tmp62;result=((_tmp62=_region_malloc(r3,sizeof(*_tmp62)),((_tmp62->hd=((_tmp63=_region_malloc(r4,sizeof(*_tmp63)),((_tmp63->f1=x->hd,((_tmp63->f2=y->hd,_tmp63)))))),((_tmp62->tl=0,_tmp62))))));}
prev=result;
# 589
x=x->tl;
y=y->tl;
# 592
while(x != 0  && y != 0){
{struct _tuple2*_tmp66;struct Cyc_List_List*_tmp65;temp=((_tmp65=_region_malloc(r3,sizeof(*_tmp65)),((_tmp65->hd=((_tmp66=_region_malloc(r4,sizeof(*_tmp66)),((_tmp66->f1=x->hd,((_tmp66->f2=y->hd,_tmp66)))))),((_tmp65->tl=0,_tmp65))))));}
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 599
if(x != 0  || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 603
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct _tuple3{void*f1;void*f2;void*f3;};
# 607
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 609
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 611
if((x == 0  && y == 0) && z == 0)return 0;
if((x == 0  || y == 0) || z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 614
{struct _tuple3*_tmp69;struct Cyc_List_List*_tmp68;result=((_tmp68=_region_malloc(r3,sizeof(*_tmp68)),((_tmp68->hd=((_tmp69=_region_malloc(r4,sizeof(*_tmp69)),((_tmp69->f1=x->hd,((_tmp69->f2=y->hd,((_tmp69->f3=z->hd,_tmp69)))))))),((_tmp68->tl=0,_tmp68))))));}
prev=result;
# 617
x=x->tl;
y=y->tl;
z=z->tl;
# 621
while((x != 0  && y != 0) && z != 0){
{struct _tuple3*_tmp6C;struct Cyc_List_List*_tmp6B;temp=((_tmp6B=_region_malloc(r3,sizeof(*_tmp6B)),((_tmp6B->hd=((_tmp6C=_region_malloc(r4,sizeof(*_tmp6C)),((_tmp6C->f1=x->hd,((_tmp6C->f2=y->hd,((_tmp6C->f3=z->hd,_tmp6C)))))))),((_tmp6B->tl=0,_tmp6B))))));}
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 629
if((x != 0  || y != 0) || z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 633
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);}
# 639
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x){
# 641
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 644
if(x == 0){struct _tuple0 _tmp6D;return(_tmp6D.f1=0,((_tmp6D.f2=0,_tmp6D)));}
# 646
{struct Cyc_List_List*_tmp6E;prev1=(result1=((_tmp6E=_region_malloc(r3,sizeof(*_tmp6E)),((_tmp6E->hd=(((struct _tuple2*)x->hd)[0]).f1,((_tmp6E->tl=0,_tmp6E)))))));}
{struct Cyc_List_List*_tmp6F;prev2=(result2=((_tmp6F=_region_malloc(r4,sizeof(*_tmp6F)),((_tmp6F->hd=(((struct _tuple2*)x->hd)[0]).f2,((_tmp6F->tl=0,_tmp6F)))))));}
# 649
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmp70;temp1=((_tmp70=_region_malloc(r3,sizeof(*_tmp70)),((_tmp70->hd=(((struct _tuple2*)x->hd)[0]).f1,((_tmp70->tl=0,_tmp70))))));}
{struct Cyc_List_List*_tmp71;temp2=((_tmp71=_region_malloc(r4,sizeof(*_tmp71)),((_tmp71->hd=(((struct _tuple2*)x->hd)[0]).f2,((_tmp71->tl=0,_tmp71))))));}
prev1->tl=temp1;
prev2->tl=temp2;
prev1=temp1;
prev2=temp2;}{
# 657
struct _tuple0 _tmp72;return(_tmp72.f1=result1,((_tmp72.f2=result2,_tmp72)));};}
# 660
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){
return Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 666
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x){
# 669
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
struct Cyc_List_List*result3;struct Cyc_List_List*prev3;struct Cyc_List_List*temp3;
# 673
if(x == 0){struct _tuple1 _tmp73;return(_tmp73.f1=0,((_tmp73.f2=0,((_tmp73.f3=0,_tmp73)))));}
# 675
{struct Cyc_List_List*_tmp74;prev1=(result1=((_tmp74=_region_malloc(r3,sizeof(*_tmp74)),((_tmp74->hd=(((struct _tuple3*)x->hd)[0]).f1,((_tmp74->tl=0,_tmp74)))))));}
{struct Cyc_List_List*_tmp75;prev2=(result2=((_tmp75=_region_malloc(r4,sizeof(*_tmp75)),((_tmp75->hd=(((struct _tuple3*)x->hd)[0]).f2,((_tmp75->tl=0,_tmp75)))))));}
{struct Cyc_List_List*_tmp76;prev3=(result3=((_tmp76=_region_malloc(r5,sizeof(*_tmp76)),((_tmp76->hd=(((struct _tuple3*)x->hd)[0]).f3,((_tmp76->tl=0,_tmp76)))))));}
# 679
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmp77;temp1=((_tmp77=_region_malloc(r3,sizeof(*_tmp77)),((_tmp77->hd=(((struct _tuple3*)x->hd)[0]).f1,((_tmp77->tl=0,_tmp77))))));}
{struct Cyc_List_List*_tmp78;temp2=((_tmp78=_region_malloc(r4,sizeof(*_tmp78)),((_tmp78->hd=(((struct _tuple3*)x->hd)[0]).f2,((_tmp78->tl=0,_tmp78))))));}
{struct Cyc_List_List*_tmp79;temp3=((_tmp79=_region_malloc(r5,sizeof(*_tmp79)),((_tmp79->hd=(((struct _tuple3*)x->hd)[0]).f3,((_tmp79->tl=0,_tmp79))))));}
prev1->tl=temp1;
prev2->tl=temp2;
prev3->tl=temp3;
prev1=temp1;
prev2=temp2;
prev3=temp3;}{
# 690
struct _tuple1 _tmp7A;return(_tmp7A.f1=result1,((_tmp7A.f2=result2,((_tmp7A.f3=result3,_tmp7A)))));};}
# 693
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){
return Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 700
int Cyc_List_memq(struct Cyc_List_List*l,void*x){
while(l != 0){
if(l->hd == x)return 1;
l=l->tl;}
# 705
return 0;}
# 708
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp(l->hd,x)== 0)return 1;
l=l->tl;}
# 713
return 0;}
# 722
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){
while(l != 0){
if((((struct _tuple2*)l->hd)[0]).f1 == x)return(((struct _tuple2*)l->hd)[0]).f2;
l=l->tl;}
# 727
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 730
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp((*((struct _tuple2*)l->hd)).f1,x)== 0)return(*((struct _tuple2*)l->hd)).f2;
l=l->tl;}
# 735
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 742
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
struct Cyc_List_List*prev=0;
struct Cyc_List_List*_tmp31=l;
while(_tmp31 != 0){
if(cmp(_tmp31->hd,x)== 0){
if(prev == 0)
return((struct Cyc_List_List*)_check_null(l))->tl;else{
# 750
prev->tl=_tmp31->tl;
return l;}}
# 754
prev=_tmp31;
_tmp31=_tmp31->tl;}
# 757
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 760
static int Cyc_List_ptrequal(void*x,void*y){
return !(x == y);}
# 767
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){
return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
# 773
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){
while(l != 0){
if((((struct _tuple2*)l->hd)[0]).f1 == x)return 1;
l=l->tl;}
# 778
return 0;}
# 783
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
while(l != 0){
if(cmp((((struct _tuple2*)l->hd)[0]).f1,x)== 0)return 1;
l=l->tl;}
# 788
return 0;}
# 794
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){
while(x != 0){
struct Cyc_List_List*_tmp32=x->tl;
if(_tmp32 != 0){
if(cmp(x->hd,_tmp32->hd)== 0){struct Cyc_Core_Opt*_tmp7B;return(_tmp7B=_cycalloc(sizeof(*_tmp7B)),((_tmp7B->v=x->hd,_tmp7B)));}}
x=_tmp32;}
# 801
return 0;}struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x);static void _tmp82(struct Cyc_List_List**x,unsigned int*_tmp81,unsigned int*_tmp80,void***_tmp7E){for(*_tmp81=0;*_tmp81 < *_tmp80;(*_tmp81)++){
# 810
void*_tmp7C;(*_tmp7E)[*_tmp81]=(void*)((_tmp7C=((struct Cyc_List_List*)_check_null(*x))->hd,((*x=(*x)->tl,_tmp7C))));}}
# 805
struct _dyneither_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){
int s;
struct _dyneither_ptr arr;
# 809
s=Cyc_List_length(x);
{unsigned int _tmp81;unsigned int _tmp80;struct _dyneither_ptr _tmp7F;void**_tmp7E;unsigned int _tmp7D;arr=((_tmp7D=(unsigned int)s,((_tmp7E=(void**)_region_malloc(r2,_check_times(sizeof(void*),_tmp7D)),((_tmp7F=_tag_dyneither(_tmp7E,sizeof(void*),_tmp7D),((((_tmp80=_tmp7D,_tmp82(& x,& _tmp81,& _tmp80,& _tmp7E))),_tmp7F))))))));}
# 813
return arr;}
# 816
struct _dyneither_ptr Cyc_List_to_array(struct Cyc_List_List*x){
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}
# 821
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _dyneither_ptr arr){
struct Cyc_List_List*ans=0;
{int i=(int)(_get_dyneither_size(arr,sizeof(void*))- 1);for(0;i >= 0;-- i){
struct Cyc_List_List*_tmp83;ans=((_tmp83=_region_malloc(r2,sizeof(*_tmp83)),((_tmp83->hd=*((void**)_check_dyneither_subscript(arr,sizeof(void*),i)),((_tmp83->tl=ans,_tmp83))))));}}
return ans;}
# 828
struct Cyc_List_List*Cyc_List_from_array(struct _dyneither_ptr arr){
return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
# 832
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
struct Cyc_List_List*_tmp84;res=((_tmp84=_region_malloc(r,sizeof(*_tmp84)),((_tmp84->hd=f(i),((_tmp84->tl=res,_tmp84))))));}}
# 837
return Cyc_List_imp_rev(res);}
# 840
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){
return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}
# 844
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
struct Cyc_List_List*_tmp85;res=((_tmp85=_region_malloc(r,sizeof(*_tmp85)),((_tmp85->hd=f(env,i),((_tmp85->tl=res,_tmp85))))));}}
# 849
return Cyc_List_imp_rev(res);}
# 852
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){
return Cyc_List_rtabulate_c(Cyc_Core_heap_region,n,f,env);}
# 856
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 859
if((unsigned int)l1 == (unsigned int)l2)
return 0;{
int _tmp3C=cmp(l1->hd,l2->hd);
if(_tmp3C != 0)
return _tmp3C;};}
# 865
if(l1 != 0)
return 1;
if(l2 != 0)
return - 1;
return 0;}
# 873
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(0;l1 != 0  && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 877
if((unsigned int)l1 == (unsigned int)l2)
return 1;{
int _tmp3D=cmp(l1->hd,l2->hd);
if(_tmp3D != 0)
return 0;};}
# 883
return l1 == 0;}
# 886
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 890
struct Cyc_List_List*_tmp86;struct Cyc_List_List*result=(_tmp86=_region_malloc(r,sizeof(*_tmp86)),((_tmp86->hd=l->hd,((_tmp86->tl=0,_tmp86)))));
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(0;l != 0;l=l->tl){
if(f(env,l->hd)){
{struct Cyc_List_List*_tmp87;temp=((_tmp87=_region_malloc(r,sizeof(*_tmp87)),((_tmp87->hd=l->hd,((_tmp87->tl=0,_tmp87))))));}
end->tl=temp;
end=temp;}}
# 900
return result->tl;};}
# 903
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}
# 907
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 911
struct Cyc_List_List*_tmp88;struct Cyc_List_List*result=(_tmp88=_region_malloc(r,sizeof(*_tmp88)),((_tmp88->hd=l->hd,((_tmp88->tl=0,_tmp88)))));
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(0;l != 0;l=l->tl){
if(f(l->hd)){
{struct Cyc_List_List*_tmp89;temp=((_tmp89=_region_malloc(r,sizeof(*_tmp89)),((_tmp89->hd=l->hd,((_tmp89->tl=0,_tmp89))))));}
end->tl=temp;
end=temp;}}
# 921
return result->tl;};}
# 924
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){
return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
