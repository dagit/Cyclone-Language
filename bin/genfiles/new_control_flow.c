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
typedef int Cyc_bool;struct Cyc_Core_NewRegion{struct _DynRegionHandle*dynregion;};
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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 123 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 138
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 160
extern struct _RegionHandle*Cyc_Core_heap_region;
# 162
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 197
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 203
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 208
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 327 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 332
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 339
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 126 "dict.h"
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 47
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 56
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 971 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1150
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1152
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1164
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1168
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1170
int Cyc_Absyn_is_require_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 252 "tcutil.h"
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 39
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;struct _tuple13{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 58
struct _tuple13 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 62
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 64
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 70
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 78
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 81
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 86
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 89
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 94
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 96
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 98
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 104
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 110
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
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
# 128
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;
struct Cyc_Dict_Dict Cyc_CfFlowInfo_union_place_set(struct Cyc_Dict_Dict s1,struct Cyc_Dict_Dict s2,int disjoint);struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
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
# 172
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple15{void*f1;struct Cyc_List_List*f2;};
struct _tuple15 Cyc_CfFlowInfo_unname_rval(struct _RegionHandle*rgn,void*rv);
# 180
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
# 183
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 185
void Cyc_CfFlowInfo_print_absrval(void*rval);
# 206 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,void*r);
# 210
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*all_changed,struct Cyc_CfFlowInfo_Place*place,void*r);
# 215
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple16{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple16 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*all_changed,struct _tuple16 pr1,struct _tuple16 pr2);
# 221
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_after_flow(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 226
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_kill_flow_region(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f,void*rgn);struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 35
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds);
# 39
extern int Cyc_NewControlFlow_warn_lose_unique;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 62 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 66
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 74
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 79
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp72C;struct Cyc_NewControlFlow_CFStmtAnnot _tmp72B;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp72A;enclosee->annot=(void*)((_tmp72A=_cycalloc(sizeof(*_tmp72A)),((_tmp72A[0]=((_tmp72C.tag=Cyc_NewControlFlow_CFAnnot,((_tmp72C.f1=((_tmp72B.encloser=encloser,((_tmp72B.visited=0,_tmp72B)))),_tmp72C)))),_tmp72A))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 110 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 118
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 120
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 127
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 130
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp5=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4;if(_tmp5->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp5->f1;}}_LL2:
 return _tmp6;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp732;const char*_tmp731;struct Cyc_Core_Impossible_exn_struct*_tmp730;(int)_throw((void*)((_tmp730=_cycalloc(sizeof(*_tmp730)),((_tmp730[0]=((_tmp732.tag=Cyc_Core_Impossible,((_tmp732.f1=((_tmp731="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp731,sizeof(char),37))),_tmp732)))),_tmp730)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp733;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp733=_region_malloc(env->r,sizeof(*_tmp733)),((_tmp733[0]=Cyc_CfFlowInfo_BottomFL(),_tmp733)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 144
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 147
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 155
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 161
_tmpB->visited=env->iteration_num;{
struct _tuple20 _tmp734;return(_tmp734.f1=_tmpB,((_tmp734.f2=_tmpC,_tmp734)));};}
# 171
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 179
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 186
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 188
struct Cyc_NewControlFlow_AnalEnv*_tmpF;unsigned int _tmp10;struct Cyc_Dict_Dict _tmp11;struct _tuple21*_tmpE=ckenv;_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;{
void*_tmp12=root;struct Cyc_Absyn_Vardecl*_tmp14;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp13=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp12;if(_tmp13->tag != 0)goto _LL8;else{_tmp14=_tmp13->f1;}}_LL7:
# 191
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpF->fenv)->r,_tmp14->type)){
retry: {void*_tmp15=rval;void*_tmp17;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp16->tag != 8)goto _LLD;else{_tmp17=(void*)_tmp16->f2;}}_LLC:
 rval=_tmp17;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp18->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp19->tag != 3)goto _LL11;else{if(_tmp19->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp1A->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 200
{const char*_tmp738;void*_tmp737[1];struct Cyc_String_pa_PrintArg_struct _tmp736;(_tmp736.tag=0,((_tmp736.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp737[0]=& _tmp736,Cyc_Tcutil_warn(_tmp10,((_tmp738="unique pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp738,sizeof(char),60))),_tag_dyneither(_tmp737,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 204
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 212
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1F);
# 218
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp739;struct _tuple19 _tmp21=(_tmp739.f1=flow,((_tmp739.f2=_tmp20,_tmp739)));struct _tuple19 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple21 _tmp73A;struct _tuple21 _tmp25=(_tmp73A.f1=env,((_tmp73A.f2=s->loc,((_tmp73A.f3=_tmp24,_tmp73A)))));
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp25,_tmp23);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 227
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){
# 234
*_tmp1F=_tmp20;
# 238
if(_tmp1E->visited == env->iteration_num)
# 240
env->iterate_again=1;}}
# 245
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 249
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_Dict_Dict _tmp29;struct Cyc_List_List*_tmp2A;_LL1B: if((_tmp28.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp28.ReachableFL).tag != 2)goto _LL1A;_tmp29=((struct _tuple14)(_tmp28.ReachableFL).val).f1;_tmp2A=((struct _tuple14)(_tmp28.ReachableFL).val).f2;_LL1E:
# 252
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp73D;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp73C;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp73C=_region_malloc(fenv->r,sizeof(*_tmp73C)),((_tmp73C[0]=((_tmp73D.tag=0,((_tmp73D.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp73D)))),_tmp73C)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp740;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp73F;_tmp2C=(void*)((_tmp73F=_region_malloc(fenv->r,sizeof(*_tmp73F)),((_tmp73F[0]=((_tmp740.tag=8,((_tmp740.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp740.f2=_tmp2C,_tmp740)))))),_tmp73F))));}
# 259
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp741;if(Cyc_Relations_consistent_relations(((_tmp741=_region_malloc(r,sizeof(*_tmp741)),((_tmp741->hd=_tmp31,((_tmp741->tl=assume,_tmp741))))))))
return 0;}
# 277
return 1;}
# 280
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp33=e->r;void*_tmp34=_tmp33;struct Cyc_Absyn_Exp*_tmp36;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34;if(_tmp35->tag != 13)goto _LL22;else{_tmp36=_tmp35->f2;}}_LL21:
 return _tmp36;_LL22:;_LL23:
 return e;_LL1F:;}
# 287
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 289
union Cyc_CfFlowInfo_FlowInfo _tmp37=inflow;struct Cyc_List_List*_tmp38;_LL25: if((_tmp37.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp37.ReachableFL).tag != 2)goto _LL24;_tmp38=((struct _tuple14)(_tmp37.ReachableFL).val).f2;_LL28:
# 292
{void*_tmp39=Cyc_Tcutil_compress(t);void*_tmp3A=_tmp39;union Cyc_Absyn_AggrInfoU _tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3F;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3B->tag != 11)goto _LL2C;else{_tmp3C=(_tmp3B->f1).aggr_info;_tmp3D=(_tmp3B->f1).targs;}}_LL2B: {
# 294
struct Cyc_Absyn_Aggrdecl*_tmp40=Cyc_Absyn_get_known_aggrdecl(_tmp3C);
struct Cyc_Absyn_Aggrfield*_tmp41=Cyc_Absyn_lookup_decl_field(_tmp40,f);
struct Cyc_Absyn_Exp*_tmp42=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp41))->requires_clause;
if(_tmp42 != 0){
struct _RegionHandle _tmp43=_new_region("temp");struct _RegionHandle*temp=& _tmp43;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp44=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp40->tvs,_tmp3D),_tmp42);
# 301
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp44))){
{const char*_tmp746;void*_tmp745[2];struct Cyc_String_pa_PrintArg_struct _tmp744;struct Cyc_String_pa_PrintArg_struct _tmp743;(_tmp743.tag=0,((_tmp743.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp744.tag=0,((_tmp744.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp745[0]=& _tmp744,((_tmp745[1]=& _tmp743,Cyc_CfFlowInfo_aerr(loc,((_tmp746="unable to prove %s, required to access %s",_tag_dyneither(_tmp746,sizeof(char),42))),_tag_dyneither(_tmp745,sizeof(void*),2)))))))))))));}
{const char*_tmp749;void*_tmp748;(_tmp748=0,Cyc_fprintf(Cyc_stderr,((_tmp749="  [recorded facts on path: ",_tag_dyneither(_tmp749,sizeof(char),28))),_tag_dyneither(_tmp748,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp74C;void*_tmp74B;(_tmp74B=0,Cyc_fprintf(Cyc_stderr,((_tmp74C="]\n",_tag_dyneither(_tmp74C,sizeof(char),3))),_tag_dyneither(_tmp74B,sizeof(void*),0)));};}}
# 299
;_pop_region(temp);}
# 308
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3E->tag != 12)goto _LL2E;else{_tmp3F=_tmp3E->f2;}}_LL2D: {
# 310
struct Cyc_Absyn_Aggrfield*_tmp4D=Cyc_Absyn_lookup_field(_tmp3F,f);
struct Cyc_Absyn_Exp*_tmp4E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp4D))->requires_clause;
if(_tmp4E != 0){
struct _RegionHandle _tmp4F=_new_region("temp");struct _RegionHandle*temp=& _tmp4F;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp4E))){
{const char*_tmp751;void*_tmp750[2];struct Cyc_String_pa_PrintArg_struct _tmp74F;struct Cyc_String_pa_PrintArg_struct _tmp74E;(_tmp74E.tag=0,((_tmp74E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp74F.tag=0,((_tmp74F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp750[0]=& _tmp74F,((_tmp750[1]=& _tmp74E,Cyc_CfFlowInfo_aerr(loc,((_tmp751="unable to prove %s, required to access %s",_tag_dyneither(_tmp751,sizeof(char),42))),_tag_dyneither(_tmp750,sizeof(void*),2)))))))))))));}
{const char*_tmp754;void*_tmp753;(_tmp753=0,Cyc_fprintf(Cyc_stderr,((_tmp754="  [recorded facts on path: ",_tag_dyneither(_tmp754,sizeof(char),28))),_tag_dyneither(_tmp753,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp757;void*_tmp756;(_tmp756=0,Cyc_fprintf(Cyc_stderr,((_tmp757="]\n",_tag_dyneither(_tmp757,sizeof(char),3))),_tag_dyneither(_tmp756,sizeof(void*),0)));};}
# 314
;_pop_region(temp);}
# 321
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 324
goto _LL24;_LL24:;}
# 328
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp58=inflow;struct Cyc_Dict_Dict _tmp59;struct Cyc_List_List*_tmp5A;_LL31: if((_tmp58.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp58.ReachableFL).tag != 2)goto _LL30;_tmp59=((struct _tuple14)(_tmp58.ReachableFL).val).f1;_tmp5A=((struct _tuple14)(_tmp58.ReachableFL).val).f2;_LL34:
# 332
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp59,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp75A;void*_tmp759;(_tmp759=0,Cyc_CfFlowInfo_aerr(loc,((_tmp75A="expression may not be fully initialized",_tag_dyneither(_tmp75A,sizeof(char),40))),_tag_dyneither(_tmp759,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp75B;struct _tuple0 _tmp5F=(_tmp75B.f1=Cyc_Tcutil_compress(t),((_tmp75B.f2=r,_tmp75B)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp62=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp62->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp63->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp64->tag != 8)goto _LL3E;else{_tmp65=(void*)_tmp64->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp65);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60.f1;if(_tmp66->tag != 4)goto _LL40;else{if((((_tmp66->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp67=((struct _tuple3)(((_tmp66->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp68=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp68->tag != 6)goto _LL40;else{_tmp69=_tmp68->f2;}};_LL3F:
# 349
 if(_tmp67->typs == 0)
return;
_tmp6B=_tmp67->typs;_tmp6D=_tmp69;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6A->tag != 10)goto _LL42;else{_tmp6B=_tmp6A->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp6C->tag != 6)goto _LL42;else{_tmp6D=_tmp6C->f2;}};_LL41: {
# 353
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B);
{int i=0;for(0;i < sz;(i ++,_tmp6B=_tmp6B->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp6D,sizeof(void*),i)));}}
# 357
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6E->tag != 11)goto _LL44;else{_tmp6F=(_tmp6E->f1).aggr_info;_tmp70=(_tmp6E->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp71->tag != 6)goto _LL44;else{_tmp72=_tmp71->f2;}};_LL43: {
# 360
struct Cyc_Absyn_Aggrdecl*_tmp79=Cyc_Absyn_get_known_aggrdecl(_tmp6F);
if(_tmp79->impl == 0)return;{
struct Cyc_List_List*_tmp7A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->fields;
struct _RegionHandle _tmp7B=_new_region("temp");struct _RegionHandle*temp=& _tmp7B;_push_region(temp);
{struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp79->tvs,_tmp70);
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp7A=_tmp7A->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp7A))->hd)->type;
if(_tmp7C != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp7C,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp72.curr)[i]);}}
# 370
_npop_handler(0);return;}
# 364
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp73->tag != 12)goto _LL46;else{_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp76=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp76->tag != 6)goto _LL46;else{_tmp77=_tmp76->f2;}};_LL45:
# 373
{int i=0;for(0;i < _get_dyneither_size(_tmp77,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type,((void**)_tmp77.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60.f1;if(_tmp78->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_Tcutil_warn(loc,((_tmp75E="argument may still contain unique pointers",_tag_dyneither(_tmp75E,sizeof(char),43))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_CfFlowInfo_aerr(loc,((_tmp761="noliveunique attribute requires unique pointer",_tag_dyneither(_tmp761,sizeof(char),47))),_tag_dyneither(_tmp760,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 391
void*_tmp85=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp86=r;void*_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8A;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp87=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp87->tag != 8)goto _LL52;else{_tmp88=(void*)_tmp87->f2;}}_LL51:
 r=_tmp88;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp89=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp89->tag != 5)goto _LL54;else{_tmp8A=_tmp89->f1;}}_LL53:
# 395
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp85,Cyc_CfFlowInfo_lookup_place(_tmp81,_tmp8A));
goto _LL4F;_LL54:;_LL55:
# 398
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp85)){
const char*_tmp764;void*_tmp763;(_tmp763=0,Cyc_Tcutil_warn(loc,((_tmp764="argument may contain live unique pointers",_tag_dyneither(_tmp764,sizeof(char),42))),_tag_dyneither(_tmp763,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 402
struct Cyc_Dict_Dict _tmp8D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp81,env->all_changed,r);
if(_tmp81.t == _tmp8D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp8E=Cyc_CfFlowInfo_ReachableFL(_tmp8D,_tmp82);
Cyc_NewControlFlow_update_tryflow(env,_tmp8E);
return _tmp8E;};};};_LL4A:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 411
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 418
if(es == 0){
struct _tuple23 _tmp765;return(_tmp765.f1=inflow,((_tmp765.f2=0,_tmp765)));}
if(es->tl == 0){
struct _tuple16 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple16 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp768;struct _tuple23 _tmp767;return(_tmp767.f1=_tmp92,((_tmp767.f2=((_tmp768=_region_malloc(rgn,sizeof(*_tmp768)),((_tmp768->hd=_tmp93,((_tmp768->tl=0,_tmp768)))))),_tmp767)));};}{
# 424
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 431
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 435
{struct Cyc_Dict_Dict*_tmp769;env->all_changed=((_tmp769=_region_malloc(env->r,sizeof(*_tmp769)),((_tmp769[0]=(env->fenv)->mt_place_set,_tmp769))));}{
struct _tuple16 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple16 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp76A;rvals=((_tmp76A=_region_malloc(rgn,sizeof(*_tmp76A)),((_tmp76A->hd=_tmp9A,((_tmp76A->tl=0,_tmp76A))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 441
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp76B;env->all_changed=((_tmp76B=_region_malloc(env->r,sizeof(*_tmp76B)),((_tmp76B[0]=(env->fenv)->mt_place_set,_tmp76B))));}{
struct _tuple16 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple16 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp76C;rvals=((_tmp76C=_region_malloc(rgn,sizeof(*_tmp76C)),((_tmp76C->hd=_tmpA0,((_tmp76C->tl=rvals,_tmp76C))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp9F,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 447
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 450
old_inflow=inflow;
# 453
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 458
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 462
struct Cyc_Dict_Dict*_tmp76D;env->all_changed=((_tmp76D=_region_malloc(env->r,sizeof(*_tmp76D)),((_tmp76D[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp76D))));}
# 464
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 466
struct _tuple23 _tmp76E;return(_tmp76E.f1=outflow,((_tmp76E.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp76E)));};};}
# 471
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 474
struct _RegionHandle _tmpA4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA4;_push_region(rgn);
{struct _tuple23 _tmpA5=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 475
union Cyc_CfFlowInfo_FlowInfo _tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple23 _tmpA6=_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
# 477
{union Cyc_CfFlowInfo_FlowInfo _tmpA9=_tmpA7;struct Cyc_Dict_Dict _tmpAA;_LL57: if((_tmpA9.ReachableFL).tag != 2)goto _LL59;_tmpAA=((struct _tuple14)(_tmpA9.ReachableFL).val).f1;_LL58:
# 479
 for(0;_tmpA8 != 0;(_tmpA8=_tmpA8->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAA,(void*)_tmpA8->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp771;void*_tmp770;(_tmp770=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp771="expression may not be initialized",_tag_dyneither(_tmp771,sizeof(char),34))),_tag_dyneither(_tmp770,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 485
struct _tuple16 _tmp772;struct _tuple16 _tmpAE=(_tmp772.f1=_tmpA7,((_tmp772.f2=(env->fenv)->unknown_all,_tmp772)));_npop_handler(0);return _tmpAE;};}
# 475
;_pop_region(rgn);}
# 493
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 500
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp778;const char*_tmp777;struct Cyc_Core_Impossible_exn_struct*_tmp776;(int)_throw((void*)((_tmp776=_cycalloc(sizeof(*_tmp776)),((_tmp776[0]=((_tmp778.tag=Cyc_Core_Impossible,((_tmp778.f1=((_tmp777="consume_zero_ral",_tag_dyneither(_tmp777,sizeof(char),17))),_tmp778)))),_tmp776)))));}else{
# 505
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 509
return new_rval;}}}
# 522 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 528
union Cyc_CfFlowInfo_FlowInfo _tmpB3=outflow;struct Cyc_Dict_Dict _tmpB4;struct Cyc_List_List*_tmpB5;_LL5C: if((_tmpB3.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpB3.ReachableFL).tag != 2)goto _LL5B;_tmpB4=((struct _tuple14)(_tmpB3.ReachableFL).val).f1;_tmpB5=((struct _tuple14)(_tmpB3.ReachableFL).val).f2;_LL5F: {
# 531
union Cyc_CfFlowInfo_AbsLVal _tmpB6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB7=_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL61: if((_tmpB7.UnknownL).tag != 2)goto _LL63;_LL62:
# 535
 return outflow;_LL63: if((_tmpB7.PlaceL).tag != 1)goto _LL60;_tmpB8=(struct Cyc_CfFlowInfo_Place*)(_tmpB7.PlaceL).val;_LL64: {
# 539
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp77B;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp77A;nzval=(void*)((_tmp77A=_region_malloc((env->fenv)->r,sizeof(*_tmp77A)),((_tmp77A[0]=((_tmp77B.tag=8,((_tmp77B.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp77B.f2=nzval,_tmp77B)))))),_tmp77A))));}
# 543
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 548
return _tmpBB;};}_LL60:;}_LL5B:;}
# 557
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 560
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple19 _tmp77C;return(_tmp77C.f1=outflow,((_tmp77C.f2=outflow,_tmp77C)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 563
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple19 _tmp77D;return(_tmp77D.f1=outflow,((_tmp77D.f2=outflow,_tmp77D)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 566
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp780;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp77F;nzval=(void*)((_tmp77F=_region_malloc((env->fenv)->r,sizeof(*_tmp77F)),((_tmp77F[0]=((_tmp780.tag=8,((_tmp780.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp780.f2=nzval,_tmp780)))))),_tmp77F))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp783;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp782;zval=(void*)((_tmp782=_region_malloc((env->fenv)->r,sizeof(*_tmp782)),((_tmp782[0]=((_tmp783.tag=8,((_tmp783.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp783.f2=zval,_tmp783)))))),_tmp782))));};}
# 572
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple19 _tmp784;return
(_tmp784.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp784.f2=
# 577
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp784)));};}_LL6A:;}_LL65:;}
# 583
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 591
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 595
void*_tmpCB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCC=_tmpCB;void*_tmpCE;union Cyc_Absyn_Constraint*_tmpCF;union Cyc_Absyn_Constraint*_tmpD0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 5)goto _LL72;else{_tmpCE=(_tmpCD->f1).elt_typ;_tmpCF=((_tmpCD->f1).ptr_atts).bounds;_tmpD0=((_tmpCD->f1).ptr_atts).zero_term;}}_LL71: {
# 597
union Cyc_CfFlowInfo_FlowInfo _tmpD1=f;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD3;_LL75: if((_tmpD1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 599
struct _tuple16 _tmp785;return(_tmp785.f1=f,((_tmp785.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp785)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 602
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct _tuple15 _tmpD5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple15 _tmpD6=_tmpD5;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
void*_tmpD9=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpE0;enum Cyc_CfFlowInfo_InitLevel _tmpE3;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDA->tag != 8)goto _LL7C;else{_tmpDB=_tmpDA->f1;_tmpDC=(void*)_tmpDA->f2;}}_LL7B: {
# 606
struct Cyc_Core_Impossible_exn_struct _tmp78B;const char*_tmp78A;struct Cyc_Core_Impossible_exn_struct*_tmp789;(int)_throw((void*)((_tmp789=_cycalloc(sizeof(*_tmp789)),((_tmp789[0]=((_tmp78B.tag=Cyc_Core_Impossible,((_tmp78B.f1=((_tmp78A="named location in anal_derefR",_tag_dyneither(_tmp78A,sizeof(char),30))),_tmp78B)))),_tmp789)))));}_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDD->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDE->tag != 2)goto _LL80;}_LL7F:
# 609
{void*_tmpE7=e->annot;void*_tmpE8=_tmpE7;struct Cyc_List_List*_tmpEA;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE8;if(_tmpE9->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpEA=_tmpE9->f1;}}_LL8A:
# 611
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEA))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 615
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC;if(_tmpED->tag != 22)goto _LL90;}_LL8F:
# 617
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp78E;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp78D;e->annot=(void*)((_tmp78D=_cycalloc(sizeof(*_tmp78D)),((_tmp78D[0]=((_tmp78E.tag=Cyc_CfFlowInfo_NotZero,((_tmp78E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp78E)))),_tmp78D))));}
goto _LL8D;_LL90:;_LL91:
# 620
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 623
goto _LL88;_LL88:;}
# 625
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpDF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDF->tag != 5)goto _LL82;else{_tmpE0=_tmpDF->f1;}}_LL81: {
# 629
int possibly_null=0;
{void*_tmpF0=e->annot;void*_tmpF1=_tmpF0;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;_LL93: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpF2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL95;else{_tmpF3=_tmpF2->f1;}}_LL94:
# 632
 possibly_null=1;
_tmpF5=_tmpF3;goto _LL96;_LL95: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpF4=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF4->tag != Cyc_CfFlowInfo_NotZero)goto _LL97;else{_tmpF5=_tmpF4->f1;}}_LL96:
# 635
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpF5))goto _LL98;
goto _LL92;_LL97:;_LL98:
# 639
{void*_tmpF6=e->r;void*_tmpF7=_tmpF6;_LL9A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF7;if(_tmpF8->tag != 22)goto _LL9C;}_LL9B:
# 641
 if(possibly_null){
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp791;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp790;e->annot=(void*)((_tmp790=_cycalloc(sizeof(*_tmp790)),((_tmp790[0]=((_tmp791.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp791.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp791)))),_tmp790))));}else{
# 644
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp794;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp793;e->annot=(void*)((_tmp793=_cycalloc(sizeof(*_tmp793)),((_tmp793[0]=((_tmp794.tag=Cyc_CfFlowInfo_NotZero,((_tmp794.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp794)))),_tmp793))));}
goto _LL99;_LL9C:;_LL9D:
# 647
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 650
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LL99;_LL99:;}
# 653
goto _LL92;_LL92:;}{
# 655
struct _tuple16 _tmp795;return(_tmp795.f1=f,((_tmp795.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpE0),_tmp795)));};}_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE1->tag != 0)goto _LL84;}_LL83:
# 657
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp796;return(_tmp796.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp796.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp796)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE2->tag != 3)goto _LL86;else{_tmpE3=_tmpE2->f1;}}_LL85:
# 660
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpE3,_tmpD8);goto _LL87;_LL86:;_LL87:
# 662
{void*_tmpFF=e->r;void*_tmp100=_tmpFF;_LL9F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp100;if(_tmp101->tag != 22)goto _LLA1;}_LLA0:
# 664
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp799;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp798;e->annot=(void*)((_tmp798=_cycalloc(sizeof(*_tmp798)),((_tmp798[0]=((_tmp799.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp799.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp799)))),_tmp798))));}
goto _LL9E;_LLA1:;_LLA2:
# 667
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9E;_LL9E:;}
# 670
goto _LL79;_LL79:;};}else{
# 673
void*_tmp104=e->annot;void*_tmp105=_tmp104;struct Cyc_List_List*_tmp107;_LLA4: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp106=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp105;if(_tmp106->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA6;else{_tmp107=_tmp106->f1;}}_LLA5:
# 675
 if(!Cyc_Relations_same_relns(_tmpD3,_tmp107))goto _LLA7;
goto _LLA3;_LLA6:;_LLA7:
# 678
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp79C;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp79B;e->annot=(void*)((_tmp79B=_cycalloc(sizeof(*_tmp79B)),((_tmp79B[0]=((_tmp79C.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp79C.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp79C)))),_tmp79B))));}
goto _LLA3;_LLA3:;}{
# 682
enum Cyc_CfFlowInfo_InitLevel _tmp10A=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmp10A){case Cyc_CfFlowInfo_NoneIL: _LLA8:
# 684
{void*_tmp10B=r;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10B;if(_tmp10C->tag != 7)goto _LLAD;}_LLAC:
# 686
{const char*_tmp79F;void*_tmp79E;(_tmp79E=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp79F="attempt to use a unique pointer that has already been copied",_tag_dyneither(_tmp79F,sizeof(char),61))),_tag_dyneither(_tmp79E,sizeof(void*),0)));}
goto _LLAA;_LLAD:;_LLAE: {
# 689
const char*_tmp7A2;void*_tmp7A1;(_tmp7A1=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7A2="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7A2,sizeof(char),46))),_tag_dyneither(_tmp7A1,sizeof(void*),0)));}_LLAA:;}
# 691
goto _LLA9;case Cyc_CfFlowInfo_AllIL: _LLA9: {
# 693
struct _tuple16 _tmp7A3;return(_tmp7A3.f1=f,((_tmp7A3.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7A3)));}case Cyc_CfFlowInfo_ThisIL: _LLAF: {
# 695
struct _tuple16 _tmp7A4;return(_tmp7A4.f1=f,((_tmp7A4.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp7A4)));}}};_LL74:;}_LL72:;_LL73: {
# 698
struct Cyc_Core_Impossible_exn_struct _tmp7AA;const char*_tmp7A9;struct Cyc_Core_Impossible_exn_struct*_tmp7A8;(int)_throw((void*)((_tmp7A8=_cycalloc(sizeof(*_tmp7A8)),((_tmp7A8[0]=((_tmp7AA.tag=Cyc_Core_Impossible,((_tmp7AA.f1=((_tmp7A9="right deref of non-pointer-type",_tag_dyneither(_tmp7A9,sizeof(char),32))),_tmp7AA)))),_tmp7A8)))));}_LL6F:;}
# 705
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 708
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 711
{void*_tmp116=e1->r;void*_tmp117=_tmp116;void*_tmp119;_LLB2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp118=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp117;if(_tmp118->tag != 1)goto _LLB4;else{_tmp119=(void*)_tmp118->f2;}}_LLB3: {
# 713
struct Cyc_Absyn_Vardecl*_tmp11A=Cyc_Tcutil_nonesc_vardecl(_tmp119);
if(_tmp11A != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp11A);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 719
goto _LLB1;}_LLB4:;_LLB5:
 goto _LLB1;_LLB1:;}{
# 723
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 729
return relns;};}
# 737
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 742
struct _tuple18 _tmp11B=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp11D;struct _tuple18 _tmp11C=_tmp11B;_tmp11D=_tmp11C.f2;
{struct _tuple18 _tmp7AB;struct _tuple18 _tmp11E=(_tmp7AB.f1=inflow,((_tmp7AB.f2=_tmp11D,_tmp7AB)));struct _tuple18 _tmp11F=_tmp11E;struct Cyc_Dict_Dict _tmp120;struct Cyc_List_List*_tmp121;struct Cyc_CfFlowInfo_Place*_tmp122;_LLB7: if(((_tmp11F.f1).ReachableFL).tag != 2)goto _LLB9;_tmp120=((struct _tuple14)((_tmp11F.f1).ReachableFL).val).f1;_tmp121=((struct _tuple14)((_tmp11F.f1).ReachableFL).val).f2;if(((_tmp11F.f2).PlaceL).tag != 1)goto _LLB9;_tmp122=(struct Cyc_CfFlowInfo_Place*)((_tmp11F.f2).PlaceL).val;_LLB8: {
# 745
void*_tmp123=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 747
struct _tuple15 _tmp124=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp126;struct Cyc_List_List*_tmp127;struct _tuple15 _tmp125=_tmp124;_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;
for(0;_tmp127 != 0;_tmp127=_tmp127->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7AE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7AD;_tmp123=(void*)((_tmp7AD=_region_malloc((env->fenv)->r,sizeof(*_tmp7AD)),((_tmp7AD[0]=((_tmp7AE.tag=8,((_tmp7AE.f1=(struct Cyc_Absyn_Vardecl*)_tmp127->hd,((_tmp7AE.f2=_tmp123,_tmp7AE)))))),_tmp7AD))));}
# 752
_tmp120=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp120,env->all_changed,_tmp122,_tmp123);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp120,_tmp121);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 758
goto _LLB6;}_LLB9: if(((_tmp11F.f1).BottomFL).tag != 1)goto _LLBB;_LLBA:
 goto _LLB6;_LLBB:;_LLBC:
# 761
{const char*_tmp7B1;void*_tmp7B0;(_tmp7B0=0,Cyc_CfFlowInfo_aerr(loc,((_tmp7B1="noconsume() parameters must be l-values",_tag_dyneither(_tmp7B1,sizeof(char),40))),_tag_dyneither(_tmp7B0,sizeof(void*),0)));}
goto _LLB6;_LLB6:;}
# 764
return inflow;}
# 769
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 777
union Cyc_CfFlowInfo_FlowInfo _tmp12D=outflow;struct Cyc_Dict_Dict _tmp12E;struct Cyc_List_List*_tmp12F;_LLBE: if((_tmp12D.BottomFL).tag != 1)goto _LLC0;_LLBF: {
# 779
struct _tuple16 _tmp7B2;return(_tmp7B2.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7B2.f2=rval,_tmp7B2)));}_LLC0: if((_tmp12D.ReachableFL).tag != 2)goto _LLBD;_tmp12E=((struct _tuple14)(_tmp12D.ReachableFL).val).f1;_tmp12F=((struct _tuple14)(_tmp12D.ReachableFL).val).f2;_LLC1: {
# 781
union Cyc_CfFlowInfo_AbsLVal _tmp131=lval;struct Cyc_CfFlowInfo_Place*_tmp132;_LLC3: if((_tmp131.PlaceL).tag != 1)goto _LLC5;_tmp132=(struct Cyc_CfFlowInfo_Place*)(_tmp131.PlaceL).val;_LLC4: {
# 783
struct Cyc_Dict_Dict _tmp133=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp12E,env->all_changed,_tmp132,rval);
_tmp12F=Cyc_Relations_reln_assign_exp(fenv->r,_tmp12F,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp133,_tmp12F);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 789
struct _tuple15 _tmp134=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp12E,_tmp132));void*_tmp136;struct _tuple15 _tmp135=_tmp134;_tmp136=_tmp135.f1;{
void*_tmp137=_tmp136;_LLC8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp138=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp137;if(_tmp138->tag != 3)goto _LLCA;else{if(_tmp138->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCA;}}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp139=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp137;if(_tmp139->tag != 7)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp13A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp137;if(_tmp13A->tag != 0)goto _LLCE;}_LLCD:
 goto _LLC7;_LLCE:;_LLCF:
# 795
{const char*_tmp7B5;void*_tmp7B4;(_tmp7B4=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7B5="assignment may overwrite unique pointer(s)",_tag_dyneither(_tmp7B5,sizeof(char),43))),_tag_dyneither(_tmp7B4,sizeof(void*),0)));}
goto _LLC7;_LLC7:;};}
# 800
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple16 _tmp7B6;return(_tmp7B6.f1=outflow,((_tmp7B6.f2=rval,_tmp7B6)));};}_LLC5: if((_tmp131.UnknownL).tag != 2)goto _LLC2;_LLC6: {
# 803
struct _tuple16 _tmp7B7;return(_tmp7B7.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp7B7.f2=rval,_tmp7B7)));}_LLC2:;}_LLBD:;}
# 810
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 816
union Cyc_CfFlowInfo_FlowInfo _tmp13F=f;struct Cyc_Dict_Dict _tmp140;struct Cyc_List_List*_tmp141;_LLD1: if((_tmp13F.BottomFL).tag != 1)goto _LLD3;_LLD2:
 return Cyc_CfFlowInfo_BottomFL();_LLD3: if((_tmp13F.ReachableFL).tag != 2)goto _LLD0;_tmp140=((struct _tuple14)(_tmp13F.ReachableFL).val).f1;_tmp141=((struct _tuple14)(_tmp13F.ReachableFL).val).f2;_LLD4:
# 821
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7BD;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7BC;struct Cyc_CfFlowInfo_Place*_tmp7BB;_tmp140=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp140,env->all_changed,(
(_tmp7BB=_region_malloc(env->r,sizeof(*_tmp7BB)),((_tmp7BB->root=(void*)((_tmp7BD=_region_malloc(env->r,sizeof(*_tmp7BD)),((_tmp7BD[0]=((_tmp7BC.tag=0,((_tmp7BC.f1=vd,_tmp7BC)))),_tmp7BD)))),((_tmp7BB->fields=0,_tmp7BB)))))),rval);}
# 824
_tmp141=Cyc_Relations_reln_assign_var(env->r,_tmp141,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp145=Cyc_CfFlowInfo_ReachableFL(_tmp140,_tmp141);
Cyc_NewControlFlow_update_tryflow(env,_tmp145);
# 829
return _tmp145;};_LLD0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 833
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 840
if(vds == 0)return inflow;{
# 843
struct Cyc_List_List*_tmp146=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp7BE;es=((_tmp7BE=_cycalloc(sizeof(*_tmp7BE)),((_tmp7BE->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp7BE->tl=es,_tmp7BE))))));}}}
# 849
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp146,fenv->unknown_all,pat_loc,name_locs);
# 851
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 855
struct _tuple16 _tmp148=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp14A;void*_tmp14B;struct _tuple16 _tmp149=_tmp148;_tmp14A=_tmp149.f1;_tmp14B=_tmp149.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp14A,_tmp14B);}}{
# 863
struct Cyc_List_List*_tmp14C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp14C != 0;_tmp14C=_tmp14C->tl){
struct _tuple24*_tmp14D=(struct _tuple24*)_tmp14C->hd;struct Cyc_Absyn_Vardecl**_tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct _tuple24*_tmp14E=_tmp14D;_tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;
if(_tmp14F != 0  && _tmp150 != 0){
if(_tmp150->topt == 0){
const char*_tmp7C2;void*_tmp7C1[1];struct Cyc_String_pa_PrintArg_struct _tmp7C0;(_tmp7C0.tag=0,((_tmp7C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp150)),((_tmp7C1[0]=& _tmp7C0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7C2="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp7C2,sizeof(char),41))),_tag_dyneither(_tmp7C1,sizeof(void*),1)))))));}{
# 877
struct Cyc_List_List _tmp7C3;struct Cyc_List_List l=(_tmp7C3.hd=_tmp150,((_tmp7C3.tl=0,_tmp7C3)));
union Cyc_CfFlowInfo_FlowInfo _tmp154=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp155=Cyc_NewControlFlow_anal_Lexp(env,_tmp154,0,0,_tmp150);union Cyc_CfFlowInfo_AbsLVal _tmp157;struct _tuple18 _tmp156=_tmp155;_tmp157=_tmp156.f2;{
struct _tuple16 _tmp158=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp154,_tmp150);union Cyc_CfFlowInfo_FlowInfo _tmp15A;void*_tmp15B;struct _tuple16 _tmp159=_tmp158;_tmp15A=_tmp159.f1;_tmp15B=_tmp159.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp15C=_tmp15A;struct Cyc_Dict_Dict _tmp15D;struct Cyc_List_List*_tmp15E;_LLD6: if((_tmp15C.ReachableFL).tag != 2)goto _LLD8;_tmp15D=((struct _tuple14)(_tmp15C.ReachableFL).val).f1;_tmp15E=((struct _tuple14)(_tmp15C.ReachableFL).val).f2;_LLD7:
# 883
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp15F=_tmp157;struct Cyc_CfFlowInfo_Place*_tmp160;_LLDB: if((_tmp15F.PlaceL).tag != 1)goto _LLDD;_tmp160=(struct Cyc_CfFlowInfo_Place*)(_tmp15F.PlaceL).val;_LLDC:
# 886
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7C6;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7C5;_tmp15B=(void*)((_tmp7C5=_region_malloc(fenv->r,sizeof(*_tmp7C5)),((_tmp7C5[0]=((_tmp7C6.tag=8,((_tmp7C6.f1=*_tmp14F,((_tmp7C6.f2=_tmp15B,_tmp7C6)))))),_tmp7C5))));}{
# 889
void*_tmp163=Cyc_CfFlowInfo_lookup_place(_tmp15D,_tmp160);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7C9;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7C8;_tmp163=(void*)((_tmp7C8=_region_malloc(fenv->r,sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7C9.tag=8,((_tmp7C9.f1=*_tmp14F,((_tmp7C9.f2=_tmp163,_tmp7C9)))))),_tmp7C8))));}
_tmp15D=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp15D,env->all_changed,_tmp160,_tmp163);
_tmp15A=Cyc_CfFlowInfo_ReachableFL(_tmp15D,_tmp15E);
goto _LLDA;};_LLDD:;_LLDE:
# 896
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp150->topt)) && !
# 898
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp14F)->type)){
# 900
const char*_tmp7CD;void*_tmp7CC[1];struct Cyc_String_pa_PrintArg_struct _tmp7CB;(_tmp7CB.tag=0,((_tmp7CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp150)),((_tmp7CC[0]=& _tmp7CB,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp7CD="aliased pattern expression |%s| not an l-value",_tag_dyneither(_tmp7CD,sizeof(char),47))),_tag_dyneither(_tmp7CC,sizeof(void*),1)))))));}_LLDA:;}
# 908
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp15A,*_tmp14F,_tmp150,_tmp15B,pat_loc);
goto _LLD5;_LLD8:;_LLD9:
# 911
 goto _LLD5;_LLD5:;};};};}}
# 916
return inflow;};};}
# 919
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 921
void*_tmp16A=e->r;void*_tmp16B=_tmp16A;struct Cyc_Absyn_Exp*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;_LLE0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp16C->tag != 1)goto _LLE2;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp16D=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp16C->f2);if(_tmp16D->tag != 4)goto _LLE2;}}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp16E->tag != 1)goto _LLE4;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp16F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp16E->f2);if(_tmp16F->tag != 3)goto _LLE4;}}_LLE3:
 goto _LLE5;_LLE4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp170=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp170->tag != 1)goto _LLE6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp171=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp170->f2);if(_tmp171->tag != 5)goto _LLE6;}}_LLE5:
 return 1;_LLE6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp172->tag != 19)goto _LLE8;else{_tmp173=_tmp172->f1;}}_LLE7:
 _tmp175=_tmp173;goto _LLE9;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp174->tag != 20)goto _LLEA;else{_tmp175=_tmp174->f1;}}_LLE9:
 _tmp177=_tmp175;goto _LLEB;_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp176=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp176->tag != 21)goto _LLEC;else{_tmp177=_tmp176->f1;}}_LLEB:
# 928
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp177,cast_ok);_LLEC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp178->tag != 22)goto _LLEE;else{_tmp179=_tmp178->f1;}}_LLED: {
# 930
void*_tmp17C=Cyc_Tcutil_compress((void*)_check_null(_tmp179->topt));void*_tmp17D=_tmp17C;_LLF3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17D;if(_tmp17E->tag != 10)goto _LLF5;}_LLF4:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp179,cast_ok);_LLF5:;_LLF6:
 return 0;_LLF2:;}_LLEE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16B;if(_tmp17A->tag != 13)goto _LLF0;else{_tmp17B=_tmp17A->f2;}}_LLEF:
# 935
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17B,cast_ok);else{
return 0;}_LLF0:;_LLF1:
 return 0;_LLDF:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp880(unsigned int*_tmp87F,unsigned int*_tmp87E,void***_tmp87C,struct Cyc_List_List**_tmp383){for(*_tmp87F=0;*_tmp87F < *_tmp87E;(*_tmp87F)++){
# 1522 "new_control_flow.cyc"
void*_tmp87A;(*_tmp87C)[*_tmp87F]=(void*)((_tmp87A=(void*)((struct Cyc_List_List*)_check_null(*_tmp383))->hd,((*_tmp383=(*_tmp383)->tl,_tmp87A))));}}
# 942 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 946
struct Cyc_CfFlowInfo_FlowEnv*_tmp17F=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 959
{union Cyc_CfFlowInfo_FlowInfo _tmp180=inflow;struct Cyc_Dict_Dict _tmp181;struct Cyc_List_List*_tmp182;_LLF8: if((_tmp180.BottomFL).tag != 1)goto _LLFA;_LLF9: {
struct _tuple16 _tmp7CE;return(_tmp7CE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CE.f2=_tmp17F->unknown_all,_tmp7CE)));}_LLFA: if((_tmp180.ReachableFL).tag != 2)goto _LLF7;_tmp181=((struct _tuple14)(_tmp180.ReachableFL).val).f1;_tmp182=((struct _tuple14)(_tmp180.ReachableFL).val).f2;_LLFB:
 d=_tmp181;relns=_tmp182;_LLF7:;}
# 974 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 995 "new_control_flow.cyc"
struct _tuple18 _tmp184=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp186;union Cyc_CfFlowInfo_AbsLVal _tmp187;struct _tuple18 _tmp185=_tmp184;_tmp186=_tmp185.f1;_tmp187=_tmp185.f2;{
struct _tuple18 _tmp7CF;struct _tuple18 _tmp188=(_tmp7CF.f1=_tmp186,((_tmp7CF.f2=_tmp187,_tmp7CF)));struct _tuple18 _tmp189=_tmp188;struct Cyc_Dict_Dict _tmp18A;struct Cyc_List_List*_tmp18B;struct Cyc_CfFlowInfo_Place*_tmp18C;_LLFD: if(((_tmp189.f1).ReachableFL).tag != 2)goto _LLFF;_tmp18A=((struct _tuple14)((_tmp189.f1).ReachableFL).val).f1;_tmp18B=((struct _tuple14)((_tmp189.f1).ReachableFL).val).f2;if(((_tmp189.f2).PlaceL).tag != 1)goto _LLFF;_tmp18C=(struct Cyc_CfFlowInfo_Place*)((_tmp189.f2).PlaceL).val;_LLFE: {
# 998
void*_tmp18D=Cyc_CfFlowInfo_lookup_place(_tmp18A,_tmp18C);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp18D,& needs_unconsume)){
{const char*_tmp7D2;void*_tmp7D1;(_tmp7D1=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7D2="expression attempts to copy unique value that may be copied elsewhere.",_tag_dyneither(_tmp7D2,sizeof(char),71))),_tag_dyneither(_tmp7D1,sizeof(void*),0)));}{
struct _tuple16 _tmp7D3;return(_tmp7D3.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D3.f2=_tmp17F->unknown_all,_tmp7D3)));};}else{
# 1004
if(needs_unconsume){
# 1006
struct _tuple16 _tmp7D4;return(_tmp7D4.f1=_tmp186,((_tmp7D4.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp17F,_tmp18D),_tmp7D4)));}else{
# 1009
void*_tmp192=Cyc_CfFlowInfo_make_unique_consumed(_tmp17F,(void*)_check_null(e->topt),e,env->iteration_num,_tmp18D);
struct Cyc_Dict_Dict _tmp193=Cyc_CfFlowInfo_assign_place(_tmp17F,e->loc,_tmp18A,env->all_changed,_tmp18C,_tmp192);
# 1021
struct _tuple16 _tmp7D5;return(_tmp7D5.f1=Cyc_CfFlowInfo_ReachableFL(_tmp193,_tmp18B),((_tmp7D5.f2=_tmp18D,_tmp7D5)));}}}_LLFF:;_LL100:
# 1023
 goto _LLFC;_LLFC:;};}{
# 1026
void*_tmp196=e->r;void*_tmp197=_tmp196;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Vardecl*_tmp1B0;struct Cyc_Absyn_Vardecl*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B6;enum Cyc_Absyn_Primop _tmp1B8;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Exp*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C8;struct Cyc_List_List*_tmp1C9;int _tmp1CB;struct Cyc_Absyn_Exp*_tmp1CC;void**_tmp1CD;struct Cyc_Absyn_Exp*_tmp1CE;int _tmp1CF;struct Cyc_Absyn_Exp*_tmp1D1;struct Cyc_Absyn_Exp*_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1DB;struct _dyneither_ptr*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct _dyneither_ptr*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct _dyneither_ptr*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_List_List*_tmp1F1;struct Cyc_Absyn_Datatypedecl*_tmp1F2;struct Cyc_List_List*_tmp1F4;struct Cyc_List_List*_tmp1F6;struct Cyc_List_List*_tmp1F8;enum Cyc_Absyn_AggrKind _tmp1F9;struct Cyc_List_List*_tmp1FA;struct Cyc_List_List*_tmp1FD;struct Cyc_Absyn_Vardecl*_tmp1FF;struct Cyc_Absyn_Exp*_tmp200;struct Cyc_Absyn_Exp*_tmp201;int _tmp202;struct Cyc_Absyn_Exp*_tmp204;void*_tmp205;int _tmp206;struct Cyc_Absyn_Stmt*_tmp208;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp198->tag != 13)goto _LL104;else{_tmp199=_tmp198->f2;if(_tmp198->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL104;}}_LL103: {
# 1030
struct _tuple16 _tmp20F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp199);union Cyc_CfFlowInfo_FlowInfo _tmp211;void*_tmp212;struct _tuple16 _tmp210=_tmp20F;_tmp211=_tmp210.f1;_tmp212=_tmp210.f2;{
struct _tuple16 _tmp213=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp211,_tmp199,_tmp212);union Cyc_CfFlowInfo_FlowInfo _tmp215;void*_tmp216;struct _tuple16 _tmp214=_tmp213;_tmp215=_tmp214.f1;_tmp216=_tmp214.f2;{
struct _tuple16 _tmp7D6;return(_tmp7D6.f1=_tmp215,((_tmp7D6.f2=_tmp212,_tmp7D6)));};};}_LL104: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp19A->tag != 13)goto _LL106;else{_tmp19B=_tmp19A->f2;}}_LL105:
# 1036
 _tmp19D=_tmp19B;goto _LL107;_LL106: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp19C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp19C->tag != 11)goto _LL108;else{_tmp19D=_tmp19C->f1;}}_LL107:
 _tmp19F=_tmp19D;goto _LL109;_LL108: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp19E->tag != 12)goto _LL10A;else{_tmp19F=_tmp19E->f1;}}_LL109:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19F);_LL10A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A0=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A0->tag != 0)goto _LL10C;else{if(((_tmp1A0->f1).Null_c).tag != 1)goto _LL10C;}}_LL10B:
# 1040
 goto _LL10D;_LL10C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A1->tag != 0)goto _LL10E;else{if(((_tmp1A1->f1).Int_c).tag != 5)goto _LL10E;if(((struct _tuple7)((_tmp1A1->f1).Int_c).val).f2 != 0)goto _LL10E;}}_LL10D: {
struct _tuple16 _tmp7D7;return(_tmp7D7.f1=inflow,((_tmp7D7.f2=_tmp17F->zero,_tmp7D7)));}_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A2=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A2->tag != 0)goto _LL110;else{if(((_tmp1A2->f1).Int_c).tag != 5)goto _LL110;}}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A3->tag != 1)goto _LL112;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1A4=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp1A3->f2);if(_tmp1A4->tag != 2)goto _LL112;}}_LL111: {
struct _tuple16 _tmp7D8;return(_tmp7D8.f1=inflow,((_tmp7D8.f2=_tmp17F->notzeroall,_tmp7D8)));}_LL112: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A5->tag != 30)goto _LL114;else{if(_tmp1A5->f1 != 0)goto _LL114;}}_LL113:
# 1045
 goto _LL115;_LL114: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A6->tag != 0)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A7->tag != 17)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A8->tag != 16)goto _LL11A;}_LL119:
 goto _LL11B;_LL11A: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1A9->tag != 18)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1AA->tag != 32)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1AB->tag != 31)goto _LL120;}_LL11F: {
struct _tuple16 _tmp7D9;return(_tmp7D9.f1=inflow,((_tmp7D9.f2=_tmp17F->unknown_all,_tmp7D9)));}_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1AC->tag != 1)goto _LL122;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1AD=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp1AC->f2);if(_tmp1AD->tag != 1)goto _LL122;}}_LL121: {
# 1054
struct _tuple16 _tmp7DA;return(_tmp7DA.f1=inflow,((_tmp7DA.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp7DA)));}_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1AE->tag != 1)goto _LL124;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1AF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp1AE->f2);if(_tmp1AF->tag != 3)goto _LL124;else{_tmp1B0=_tmp1AF->f1;}}}_LL123:
# 1057
 _tmp1B3=_tmp1B0;goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1B1->tag != 1)goto _LL126;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1B2=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp1B1->f2);if(_tmp1B2->tag != 4)goto _LL126;else{_tmp1B3=_tmp1B2->f1;}}}_LL125:
 _tmp1B6=_tmp1B3;goto _LL127;_LL126: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1B4->tag != 1)goto _LL128;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B5=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp1B4->f2);if(_tmp1B5->tag != 5)goto _LL128;else{_tmp1B6=_tmp1B5->f1;}}}_LL127: {
# 1061
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7DB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp7DB.tag=0,((_tmp7DB.f1=_tmp1B6,_tmp7DB)));
struct _tuple16 _tmp7DC;return(_tmp7DC.f1=inflow,((_tmp7DC.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp7DC)));}_LL128: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1B7->tag != 2)goto _LL12A;else{_tmp1B8=_tmp1B7->f1;_tmp1B9=_tmp1B7->f2;}}_LL129: {
# 1068
struct _tuple16 _tmp21E=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1B9);union Cyc_CfFlowInfo_FlowInfo _tmp220;void*_tmp221;struct _tuple16 _tmp21F=_tmp21E;_tmp220=_tmp21F.f1;_tmp221=_tmp21F.f2;{
struct _tuple16 _tmp7DD;return(_tmp7DD.f1=_tmp220,((_tmp7DD.f2=_tmp221,_tmp7DD)));};}_LL12A: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1BA->tag != 4)goto _LL12C;else{_tmp1BB=_tmp1BA->f1;}}_LL12B: {
# 1072
struct Cyc_List_List _tmp7DE;struct Cyc_List_List _tmp223=(_tmp7DE.hd=_tmp1BB,((_tmp7DE.tl=0,_tmp7DE)));
struct _tuple16 _tmp224=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp223);union Cyc_CfFlowInfo_FlowInfo _tmp226;struct _tuple16 _tmp225=_tmp224;_tmp226=_tmp225.f1;{
struct _tuple18 _tmp227=Cyc_NewControlFlow_anal_Lexp(env,_tmp226,0,0,_tmp1BB);union Cyc_CfFlowInfo_AbsLVal _tmp229;struct _tuple18 _tmp228=_tmp227;_tmp229=_tmp228.f2;
{struct _tuple25 _tmp7DF;struct _tuple25 _tmp22A=(_tmp7DF.f1=_tmp229,((_tmp7DF.f2=_tmp226,_tmp7DF)));struct _tuple25 _tmp22B=_tmp22A;struct Cyc_CfFlowInfo_Place*_tmp22C;struct Cyc_Dict_Dict _tmp22D;struct Cyc_List_List*_tmp22E;_LL16B: if(((_tmp22B.f1).PlaceL).tag != 1)goto _LL16D;_tmp22C=(struct Cyc_CfFlowInfo_Place*)((_tmp22B.f1).PlaceL).val;if(((_tmp22B.f2).ReachableFL).tag != 2)goto _LL16D;_tmp22D=((struct _tuple14)((_tmp22B.f2).ReachableFL).val).f1;_tmp22E=((struct _tuple14)((_tmp22B.f2).ReachableFL).val).f2;_LL16C:
# 1077
 _tmp22E=Cyc_Relations_reln_kill_exp(_tmp17F->r,_tmp22E,_tmp1BB);
_tmp226=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp17F,_tmp1BB->loc,_tmp22D,env->all_changed,_tmp22C,_tmp17F->unknown_all),_tmp22E);
# 1082
goto _LL16A;_LL16D:;_LL16E:
 goto _LL16A;_LL16A:;}{
# 1085
struct _tuple16 _tmp7E0;return(_tmp7E0.f1=_tmp226,((_tmp7E0.f2=_tmp17F->unknown_all,_tmp7E0)));};};}_LL12C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1BC=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1BC->tag != 3)goto _LL12E;else{_tmp1BD=_tmp1BC->f1;if(_tmp1BC->f2 == 0)goto _LL12E;_tmp1BE=_tmp1BC->f3;}}_LL12D:
# 1088
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7E3;void*_tmp7E2;(_tmp7E2=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7E3="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7E3,sizeof(char),58))),_tag_dyneither(_tmp7E2,sizeof(void*),0)));}{
struct _tuple16 _tmp7E4;return(_tmp7E4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E4.f2=_tmp17F->unknown_all,_tmp7E4)));};}{
# 1092
struct Cyc_List_List _tmp7E5;struct Cyc_List_List _tmp235=(_tmp7E5.hd=_tmp1BE,((_tmp7E5.tl=0,_tmp7E5)));
struct Cyc_List_List _tmp7E6;struct Cyc_List_List _tmp236=(_tmp7E6.hd=_tmp1BD,((_tmp7E6.tl=& _tmp235,_tmp7E6)));
struct _tuple16 _tmp237=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp236);union Cyc_CfFlowInfo_FlowInfo _tmp239;struct _tuple16 _tmp238=_tmp237;_tmp239=_tmp238.f1;{
struct _tuple18 _tmp23A=Cyc_NewControlFlow_anal_Lexp(env,_tmp239,0,0,_tmp1BD);union Cyc_CfFlowInfo_AbsLVal _tmp23C;struct _tuple18 _tmp23B=_tmp23A;_tmp23C=_tmp23B.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp23D=_tmp239;struct Cyc_Dict_Dict _tmp23E;struct Cyc_List_List*_tmp23F;_LL170: if((_tmp23D.ReachableFL).tag != 2)goto _LL172;_tmp23E=((struct _tuple14)(_tmp23D.ReachableFL).val).f1;_tmp23F=((struct _tuple14)(_tmp23D.ReachableFL).val).f2;_LL171:
# 1098
{union Cyc_CfFlowInfo_AbsLVal _tmp240=_tmp23C;struct Cyc_CfFlowInfo_Place*_tmp241;_LL175: if((_tmp240.PlaceL).tag != 1)goto _LL177;_tmp241=(struct Cyc_CfFlowInfo_Place*)(_tmp240.PlaceL).val;_LL176:
# 1102
 _tmp23F=Cyc_Relations_reln_kill_exp(_tmp17F->r,_tmp23F,_tmp1BD);
_tmp23E=Cyc_CfFlowInfo_assign_place(_tmp17F,_tmp1BD->loc,_tmp23E,env->all_changed,_tmp241,_tmp17F->unknown_all);
# 1105
_tmp239=Cyc_CfFlowInfo_ReachableFL(_tmp23E,_tmp23F);
# 1109
goto _LL174;_LL177: if((_tmp240.UnknownL).tag != 2)goto _LL174;_LL178:
# 1112
 goto _LL174;_LL174:;}
# 1114
goto _LL16F;_LL172:;_LL173:
 goto _LL16F;_LL16F:;}{
# 1117
struct _tuple16 _tmp7E7;return(_tmp7E7.f1=_tmp239,((_tmp7E7.f2=_tmp17F->unknown_all,_tmp7E7)));};};};_LL12E: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1BF=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1BF->tag != 3)goto _LL130;else{_tmp1C0=_tmp1BF->f1;if(_tmp1BF->f2 != 0)goto _LL130;_tmp1C1=_tmp1BF->f3;}}_LL12F:
# 1121
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7EA="cannot track unique pointers through multiple assignments",_tag_dyneither(_tmp7EA,sizeof(char),58))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}{
struct _tuple16 _tmp7EB;return(_tmp7EB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EB.f2=_tmp17F->unknown_all,_tmp7EB)));};}{
# 1125
struct Cyc_Dict_Dict*_tmp248=env->all_changed;
# 1127
{struct Cyc_Absyn_Exp*_tmp7EC[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp7EC[1]=_tmp1C1,((_tmp7EC[0]=_tmp1C0,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp7EC,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp7ED;env->all_changed=((_tmp7ED=_region_malloc(env->r,sizeof(*_tmp7ED)),((_tmp7ED[0]=_tmp17F->mt_place_set,_tmp7ED))));}{
struct _tuple18 _tmp24B=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1C0);union Cyc_CfFlowInfo_FlowInfo _tmp24D;union Cyc_CfFlowInfo_AbsLVal _tmp24E;struct _tuple18 _tmp24C=_tmp24B;_tmp24D=_tmp24C.f1;_tmp24E=_tmp24C.f2;{
struct Cyc_Dict_Dict _tmp24F=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp7EE;env->all_changed=((_tmp7EE=_region_malloc(env->r,sizeof(*_tmp7EE)),((_tmp7EE[0]=_tmp17F->mt_place_set,_tmp7EE))));}{
struct _tuple16 _tmp251=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C1);union Cyc_CfFlowInfo_FlowInfo _tmp253;void*_tmp254;struct _tuple16 _tmp252=_tmp251;_tmp253=_tmp252.f1;_tmp254=_tmp252.f2;{
struct Cyc_Dict_Dict _tmp255=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp256=Cyc_CfFlowInfo_after_flow(_tmp17F,& _tmp24F,_tmp24D,_tmp253,_tmp24F,_tmp255);
# 1138
union Cyc_CfFlowInfo_FlowInfo _tmp257=Cyc_CfFlowInfo_join_flow(_tmp17F,_tmp248,inflow,_tmp256);
# 1141
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp257,inflow)){
if(_tmp248 == 0)
env->all_changed=0;else{
# 1145
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp248,
Cyc_CfFlowInfo_union_place_set(_tmp24F,_tmp255,0),0);}
# 1148
return Cyc_NewControlFlow_do_assign(_tmp17F,env,_tmp256,_tmp1C0,_tmp24E,_tmp1C1,_tmp254,e->loc);}
# 1151
inflow=_tmp257;};};};};}};_LL130: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1C2->tag != 8)goto _LL132;else{_tmp1C3=_tmp1C2->f1;_tmp1C4=_tmp1C2->f2;}}_LL131: {
# 1156
struct _tuple16 _tmp258=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp25A;void*_tmp25B;struct _tuple16 _tmp259=_tmp258;_tmp25A=_tmp259.f1;_tmp25B=_tmp259.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25A,_tmp1C4);}_LL132: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1C5->tag != 10)goto _LL134;else{_tmp1C6=_tmp1C5->f1;}}_LL133: {
# 1160
struct _tuple16 _tmp25C=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp25E;void*_tmp25F;struct _tuple16 _tmp25D=_tmp25C;_tmp25E=_tmp25D.f1;_tmp25F=_tmp25D.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1C6->loc,_tmp25E,_tmp25F);{
struct _tuple16 _tmp7EF;return(_tmp7EF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EF.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp7EF)));};}_LL134: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1C7=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1C7->tag != 9)goto _LL136;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LL135: {
# 1165
struct _RegionHandle _tmp261=_new_region("temp");struct _RegionHandle*temp=& _tmp261;_push_region(temp);
{struct Cyc_List_List*_tmp7F0;struct _tuple23 _tmp262=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp7F0=_region_malloc(temp,sizeof(*_tmp7F0)),((_tmp7F0->hd=_tmp1C8,((_tmp7F0->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1C9),_tmp7F0)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp264;struct Cyc_List_List*_tmp265;struct _tuple23 _tmp263=_tmp262;_tmp264=_tmp263.f1;_tmp265=_tmp263.f2;{
# 1168
union Cyc_CfFlowInfo_FlowInfo _tmp266=Cyc_NewControlFlow_use_Rval(env,_tmp1C8->loc,_tmp264,(void*)((struct Cyc_List_List*)_check_null(_tmp265))->hd);
_tmp265=_tmp265->tl;{
# 1171
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp267=Cyc_Tcutil_compress((void*)_check_null(_tmp1C8->topt));void*_tmp268=_tmp267;void*_tmp26A;_LL17A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp268;if(_tmp269->tag != 5)goto _LL17C;else{_tmp26A=(_tmp269->f1).elt_typ;}}_LL17B:
# 1176
{void*_tmp26B=Cyc_Tcutil_compress(_tmp26A);void*_tmp26C=_tmp26B;struct Cyc_List_List*_tmp26E;_LL17F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26C;if(_tmp26D->tag != 9)goto _LL181;else{_tmp26E=(_tmp26D->f1).attributes;}}_LL180:
# 1178
 for(0;_tmp26E != 0;_tmp26E=_tmp26E->tl){
# 1180
void*_tmp26F=(void*)_tmp26E->hd;void*_tmp270=_tmp26F;int _tmp272;int _tmp274;int _tmp276;_LL184: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp271=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp270;if(_tmp271->tag != 20)goto _LL186;else{_tmp272=_tmp271->f1;}}_LL185:
# 1182
{struct Cyc_List_List*_tmp7F1;init_params=((_tmp7F1=_region_malloc(temp,sizeof(*_tmp7F1)),((_tmp7F1->hd=(void*)_tmp272,((_tmp7F1->tl=init_params,_tmp7F1))))));}goto _LL183;_LL186: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp273=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp270;if(_tmp273->tag != 21)goto _LL188;else{_tmp274=_tmp273->f1;}}_LL187:
# 1184
{struct Cyc_List_List*_tmp7F2;nolive_unique_params=((_tmp7F2=_region_malloc(temp,sizeof(*_tmp7F2)),((_tmp7F2->hd=(void*)_tmp274,((_tmp7F2->tl=nolive_unique_params,_tmp7F2))))));}
goto _LL183;_LL188: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp275=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp270;if(_tmp275->tag != 22)goto _LL18A;else{_tmp276=_tmp275->f1;}}_LL189:
# 1188
{struct Cyc_List_List*_tmp7F3;noconsume_params=((_tmp7F3=_region_malloc(temp,sizeof(*_tmp7F3)),((_tmp7F3->hd=(void*)_tmp276,((_tmp7F3->tl=noconsume_params,_tmp7F3))))));}
goto _LL183;_LL18A:;_LL18B:
 goto _LL183;_LL183:;}
# 1192
goto _LL17E;_LL181:;_LL182: {
const char*_tmp7F6;void*_tmp7F5;(_tmp7F5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F6="anal_Rexp: bad function type",_tag_dyneither(_tmp7F6,sizeof(char),29))),_tag_dyneither(_tmp7F5,sizeof(void*),0)));}_LL17E:;}
# 1195
goto _LL179;_LL17C:;_LL17D: {
const char*_tmp7F9;void*_tmp7F8;(_tmp7F8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F9="anal_Rexp: bad function type",_tag_dyneither(_tmp7F9,sizeof(char),29))),_tag_dyneither(_tmp7F8,sizeof(void*),0)));}_LL179:;}
# 1198
{int i=1;for(0;_tmp265 != 0;(((_tmp265=_tmp265->tl,_tmp1C9=((struct Cyc_List_List*)_check_null(_tmp1C9))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp27E=_tmp264;struct Cyc_Dict_Dict _tmp27F;_LL18D: if((_tmp27E.BottomFL).tag != 1)goto _LL18F;_LL18E:
 goto _LL18C;_LL18F: if((_tmp27E.ReachableFL).tag != 2)goto _LL18C;_tmp27F=((struct _tuple14)(_tmp27E.ReachableFL).val).f1;_LL190:
# 1203
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp27F,(void*)_tmp265->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7FC;void*_tmp7FB;(_tmp7FB=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd)->loc,((_tmp7FC="expression may not be initialized",_tag_dyneither(_tmp7FC,sizeof(char),34))),_tag_dyneither(_tmp7FB,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp282=_tmp266;struct Cyc_Dict_Dict _tmp283;struct Cyc_List_List*_tmp284;_LL192: if((_tmp282.BottomFL).tag != 1)goto _LL194;_LL193:
 goto _LL191;_LL194: if((_tmp282.ReachableFL).tag != 2)goto _LL191;_tmp283=((struct _tuple14)(_tmp282.ReachableFL).val).f1;_tmp284=((struct _tuple14)(_tmp282.ReachableFL).val).f2;_LL195: {
# 1210
struct Cyc_Dict_Dict _tmp285=Cyc_CfFlowInfo_escape_deref(_tmp17F,_tmp283,env->all_changed,(void*)_tmp265->hd);
{void*_tmp286=(void*)_tmp265->hd;void*_tmp287=_tmp286;struct Cyc_CfFlowInfo_Place*_tmp289;_LL197: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp288=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp287;if(_tmp288->tag != 5)goto _LL199;else{_tmp289=_tmp288->f1;}}_LL198:
# 1213
{void*_tmp28A=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd)->topt));void*_tmp28B=_tmp28A;void*_tmp28D;_LL19C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B;if(_tmp28C->tag != 5)goto _LL19E;else{_tmp28D=(_tmp28C->f1).elt_typ;}}_LL19D:
# 1215
 _tmp285=Cyc_CfFlowInfo_assign_place(_tmp17F,((struct Cyc_Absyn_Exp*)_tmp1C9->hd)->loc,_tmp285,env->all_changed,_tmp289,
# 1217
Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,_tmp28D,0,_tmp17F->esc_all));
# 1220
goto _LL19B;_LL19E:;_LL19F: {
const char*_tmp7FF;void*_tmp7FE;(_tmp7FE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7FF="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp7FF,sizeof(char),39))),_tag_dyneither(_tmp7FE,sizeof(void*),0)));}_LL19B:;}
# 1223
goto _LL196;_LL199:;_LL19A:
 goto _LL196;_LL196:;}
# 1226
_tmp266=Cyc_CfFlowInfo_ReachableFL(_tmp285,_tmp284);
goto _LL191;}_LL191:;}
# 1229
goto _LL18C;_LL18C:;}else{
# 1232
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1237
_tmp266=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd)->topt),_tmp266,(void*)_tmp265->hd);else{
# 1247
_tmp266=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1C9))->hd)->loc,_tmp266,(void*)_tmp265->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1C9->hd)->topt),0))
_tmp266=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp266,(struct Cyc_Absyn_Exp*)_tmp1C9->hd,((struct Cyc_Absyn_Exp*)_tmp1C9->hd)->loc,(void*)_tmp265->hd);}}}}
# 1254
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1C8->topt))){
struct _tuple16 _tmp800;struct _tuple16 _tmp291=(_tmp800.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp800.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp800)));_npop_handler(0);return _tmp291;}else{
# 1257
struct _tuple16 _tmp801;struct _tuple16 _tmp293=(_tmp801.f1=_tmp266,((_tmp801.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp801)));_npop_handler(0);return _tmp293;}};};}
# 1166
;_pop_region(temp);}_LL136: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1CA->tag != 33)goto _LL138;else{_tmp1CB=(_tmp1CA->f1).is_calloc;_tmp1CC=(_tmp1CA->f1).rgn;_tmp1CD=(_tmp1CA->f1).elt_type;_tmp1CE=(_tmp1CA->f1).num_elts;_tmp1CF=(_tmp1CA->f1).fat_result;}}_LL137: {
# 1260
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp804;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp803;void*root=(void*)((_tmp803=_region_malloc(_tmp17F->r,sizeof(*_tmp803)),((_tmp803[0]=((_tmp804.tag=1,((_tmp804.f1=_tmp1CE,((_tmp804.f2=(void*)_check_null(e->topt),_tmp804)))))),_tmp803))));
struct Cyc_CfFlowInfo_Place*_tmp805;struct Cyc_CfFlowInfo_Place*place=(_tmp805=_region_malloc(_tmp17F->r,sizeof(*_tmp805)),((_tmp805->root=root,((_tmp805->fields=0,_tmp805)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp808;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp807;void*rval=(void*)((_tmp807=_region_malloc(_tmp17F->r,sizeof(*_tmp807)),((_tmp807[0]=((_tmp808.tag=5,((_tmp808.f1=place,_tmp808)))),_tmp807))));
void*place_val;
if(_tmp1CF)place_val=_tmp17F->notzeroall;else{
if(_tmp1CB)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,*((void**)_check_null(_tmp1CD)),0,_tmp17F->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,*((void**)_check_null(_tmp1CD)),0,_tmp17F->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1CC != 0){
struct _RegionHandle _tmp295=_new_region("temp");struct _RegionHandle*temp=& _tmp295;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp809[2];struct _tuple23 _tmp296=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp809[1]=_tmp1CE,((_tmp809[0]=_tmp1CC,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp809,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1271
union Cyc_CfFlowInfo_FlowInfo _tmp298;struct Cyc_List_List*_tmp299;struct _tuple23 _tmp297=_tmp296;_tmp298=_tmp297.f1;_tmp299=_tmp297.f2;
# 1273
outflow=_tmp298;}
# 1271
;_pop_region(temp);}else{
# 1276
struct _tuple16 _tmp29B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1CE);union Cyc_CfFlowInfo_FlowInfo _tmp29D;struct _tuple16 _tmp29C=_tmp29B;_tmp29D=_tmp29C.f1;
outflow=_tmp29D;}{
# 1279
union Cyc_CfFlowInfo_FlowInfo _tmp29E=outflow;struct Cyc_Dict_Dict _tmp29F;struct Cyc_List_List*_tmp2A0;_LL1A1: if((_tmp29E.BottomFL).tag != 1)goto _LL1A3;_LL1A2: {
struct _tuple16 _tmp80A;return(_tmp80A.f1=outflow,((_tmp80A.f2=rval,_tmp80A)));}_LL1A3: if((_tmp29E.ReachableFL).tag != 2)goto _LL1A0;_tmp29F=((struct _tuple14)(_tmp29E.ReachableFL).val).f1;_tmp2A0=((struct _tuple14)(_tmp29E.ReachableFL).val).f2;_LL1A4: {
# 1282
struct _tuple16 _tmp80B;return(_tmp80B.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29F,root,place_val),_tmp2A0),((_tmp80B.f2=rval,_tmp80B)));}_LL1A0:;};};}_LL138: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1D0=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1D0->tag != 34)goto _LL13A;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}_LL139: {
# 1286
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1292
struct _RegionHandle _tmp2A8=_new_region("temp");struct _RegionHandle*temp=& _tmp2A8;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp80C[2];struct _tuple23 _tmp2A9=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp80C[1]=_tmp1D2,((_tmp80C[0]=_tmp1D1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp80C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1293
union Cyc_CfFlowInfo_FlowInfo _tmp2AB;struct Cyc_List_List*_tmp2AC;struct _tuple23 _tmp2AA=_tmp2A9;_tmp2AB=_tmp2AA.f1;_tmp2AC=_tmp2AA.f2;
# 1295
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AC))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AC->tl))->hd;
outflow=_tmp2AB;}{
# 1300
struct _tuple18 _tmp2AE=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D1);union Cyc_CfFlowInfo_AbsLVal _tmp2B0;struct _tuple18 _tmp2AF=_tmp2AE;_tmp2B0=_tmp2AF.f2;{
struct _tuple18 _tmp2B1=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D2);union Cyc_CfFlowInfo_AbsLVal _tmp2B3;struct _tuple18 _tmp2B2=_tmp2B1;_tmp2B3=_tmp2B2.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2B4=outflow;struct Cyc_Dict_Dict _tmp2B5;struct Cyc_List_List*_tmp2B6;_LL1A6: if((_tmp2B4.ReachableFL).tag != 2)goto _LL1A8;_tmp2B5=((struct _tuple14)(_tmp2B4.ReachableFL).val).f1;_tmp2B6=((struct _tuple14)(_tmp2B4.ReachableFL).val).f2;_LL1A7:
# 1304
{union Cyc_CfFlowInfo_AbsLVal _tmp2B7=_tmp2B0;struct Cyc_CfFlowInfo_Place*_tmp2B8;_LL1AB: if((_tmp2B7.PlaceL).tag != 1)goto _LL1AD;_tmp2B8=(struct Cyc_CfFlowInfo_Place*)(_tmp2B7.PlaceL).val;_LL1AC:
# 1306
 _tmp2B5=Cyc_CfFlowInfo_assign_place(_tmp17F,_tmp1D1->loc,_tmp2B5,env->all_changed,_tmp2B8,right_rval);
# 1308
goto _LL1AA;_LL1AD: if((_tmp2B7.UnknownL).tag != 2)goto _LL1AA;_LL1AE:
# 1313
 if(Cyc_CfFlowInfo_initlevel(_tmp17F,_tmp2B5,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp80F;void*_tmp80E;(_tmp80E=0,Cyc_Tcutil_terr(_tmp1D2->loc,((_tmp80F="expression may not be fully initialized",_tag_dyneither(_tmp80F,sizeof(char),40))),_tag_dyneither(_tmp80E,sizeof(void*),0)));}
goto _LL1AA;_LL1AA:;}
# 1317
{union Cyc_CfFlowInfo_AbsLVal _tmp2BB=_tmp2B3;struct Cyc_CfFlowInfo_Place*_tmp2BC;_LL1B0: if((_tmp2BB.PlaceL).tag != 1)goto _LL1B2;_tmp2BC=(struct Cyc_CfFlowInfo_Place*)(_tmp2BB.PlaceL).val;_LL1B1:
# 1319
 _tmp2B5=Cyc_CfFlowInfo_assign_place(_tmp17F,_tmp1D2->loc,_tmp2B5,env->all_changed,_tmp2BC,left_rval);
# 1321
goto _LL1AF;_LL1B2: if((_tmp2BB.UnknownL).tag != 2)goto _LL1AF;_LL1B3:
# 1323
 if(Cyc_CfFlowInfo_initlevel(_tmp17F,_tmp2B5,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp812;void*_tmp811;(_tmp811=0,Cyc_Tcutil_terr(_tmp1D1->loc,((_tmp812="expression may not be fully initialized",_tag_dyneither(_tmp812,sizeof(char),40))),_tag_dyneither(_tmp811,sizeof(void*),0)));}
goto _LL1AF;_LL1AF:;}
# 1328
_tmp2B6=Cyc_Relations_reln_kill_exp(_tmp17F->r,_tmp2B6,_tmp1D1);
_tmp2B6=Cyc_Relations_reln_kill_exp(_tmp17F->r,_tmp2B6,_tmp1D2);
# 1331
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B5,_tmp2B6);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL1A5;_LL1A8:;_LL1A9:
 goto _LL1A5;_LL1A5:;}{
# 1338
struct _tuple16 _tmp813;struct _tuple16 _tmp2C0=(_tmp813.f1=outflow,((_tmp813.f2=_tmp17F->unknown_all,_tmp813)));_npop_handler(0);return _tmp2C0;};};};
# 1292
;_pop_region(temp);}_LL13A: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1D3->tag != 15)goto _LL13C;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;}}_LL13B: {
# 1341
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp816;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp815;void*root=(void*)((_tmp815=_region_malloc(_tmp17F->r,sizeof(*_tmp815)),((_tmp815[0]=((_tmp816.tag=1,((_tmp816.f1=_tmp1D5,((_tmp816.f2=(void*)_check_null(e->topt),_tmp816)))))),_tmp815))));
struct Cyc_CfFlowInfo_Place*_tmp817;struct Cyc_CfFlowInfo_Place*place=(_tmp817=_region_malloc(_tmp17F->r,sizeof(*_tmp817)),((_tmp817->root=root,((_tmp817->fields=0,_tmp817)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp81A;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp819;void*rval=(void*)((_tmp819=_region_malloc(_tmp17F->r,sizeof(*_tmp819)),((_tmp819[0]=((_tmp81A.tag=5,((_tmp81A.f1=place,_tmp81A)))),_tmp819))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1D4 != 0){
struct _RegionHandle _tmp2C1=_new_region("temp");struct _RegionHandle*temp=& _tmp2C1;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp81B[2];struct _tuple23 _tmp2C2=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp81B[1]=_tmp1D5,((_tmp81B[0]=_tmp1D4,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp81B,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1349
union Cyc_CfFlowInfo_FlowInfo _tmp2C4;struct Cyc_List_List*_tmp2C5;struct _tuple23 _tmp2C3=_tmp2C2;_tmp2C4=_tmp2C3.f1;_tmp2C5=_tmp2C3.f2;
# 1351
outflow=_tmp2C4;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C5))->tl))->hd;}
# 1349
;_pop_region(temp);}else{
# 1355
struct _tuple16 _tmp2C7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D5);union Cyc_CfFlowInfo_FlowInfo _tmp2C9;void*_tmp2CA;struct _tuple16 _tmp2C8=_tmp2C7;_tmp2C9=_tmp2C8.f1;_tmp2CA=_tmp2C8.f2;
outflow=_tmp2C9;
place_val=_tmp2CA;}{
# 1359
union Cyc_CfFlowInfo_FlowInfo _tmp2CB=outflow;struct Cyc_Dict_Dict _tmp2CC;struct Cyc_List_List*_tmp2CD;_LL1B5: if((_tmp2CB.BottomFL).tag != 1)goto _LL1B7;_LL1B6: {
struct _tuple16 _tmp81C;return(_tmp81C.f1=outflow,((_tmp81C.f2=rval,_tmp81C)));}_LL1B7: if((_tmp2CB.ReachableFL).tag != 2)goto _LL1B4;_tmp2CC=((struct _tuple14)(_tmp2CB.ReachableFL).val).f1;_tmp2CD=((struct _tuple14)(_tmp2CB.ReachableFL).val).f2;_LL1B8: {
# 1362
struct _tuple16 _tmp81D;return(_tmp81D.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CC,root,place_val),_tmp2CD),((_tmp81D.f2=rval,_tmp81D)));}_LL1B4:;};};}_LL13C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1D6->tag != 14)goto _LL13E;else{_tmp1D7=_tmp1D6->f1;}}_LL13D: {
# 1366
struct _tuple18 _tmp2D5=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1D7);union Cyc_CfFlowInfo_FlowInfo _tmp2D7;union Cyc_CfFlowInfo_AbsLVal _tmp2D8;struct _tuple18 _tmp2D6=_tmp2D5;_tmp2D7=_tmp2D6.f1;_tmp2D8=_tmp2D6.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2D9=_tmp2D8;struct Cyc_CfFlowInfo_Place*_tmp2DA;_LL1BA: if((_tmp2D9.UnknownL).tag != 2)goto _LL1BC;_LL1BB: {
struct _tuple16 _tmp81E;return(_tmp81E.f1=_tmp2D7,((_tmp81E.f2=_tmp17F->notzeroall,_tmp81E)));}_LL1BC: if((_tmp2D9.PlaceL).tag != 1)goto _LL1B9;_tmp2DA=(struct Cyc_CfFlowInfo_Place*)(_tmp2D9.PlaceL).val;_LL1BD: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp824;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp823;struct _tuple16 _tmp822;return(_tmp822.f1=_tmp2D7,((_tmp822.f2=(void*)((_tmp824=_region_malloc(env->r,sizeof(*_tmp824)),((_tmp824[0]=((_tmp823.tag=5,((_tmp823.f1=_tmp2DA,_tmp823)))),_tmp824)))),_tmp822)));}_LL1B9:;};}_LL13E: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1D8=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1D8->tag != 19)goto _LL140;else{_tmp1D9=_tmp1D8->f1;}}_LL13F: {
# 1373
struct _tuple16 _tmp2DF=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1D9);union Cyc_CfFlowInfo_FlowInfo _tmp2E1;void*_tmp2E2;struct _tuple16 _tmp2E0=_tmp2DF;_tmp2E1=_tmp2E0.f1;_tmp2E2=_tmp2E0.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E1,_tmp1D9,_tmp2E2);}_LL140: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1DA=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1DA->tag != 20)goto _LL142;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;}}_LL141: {
# 1377
struct _tuple16 _tmp2E3=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DB);union Cyc_CfFlowInfo_FlowInfo _tmp2E5;void*_tmp2E6;struct _tuple16 _tmp2E4=_tmp2E3;_tmp2E5=_tmp2E4.f1;_tmp2E6=_tmp2E4.f2;
if(_tmp1DB->topt == 0){
{const char*_tmp828;void*_tmp827[1];struct Cyc_String_pa_PrintArg_struct _tmp826;(_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp827[0]=& _tmp826,Cyc_fprintf(Cyc_stderr,((_tmp828="aggrmember exp %s\n",_tag_dyneither(_tmp828,sizeof(char),19))),_tag_dyneither(_tmp827,sizeof(void*),1)))))));}{
const char*_tmp82C;void*_tmp82B[1];struct Cyc_String_pa_PrintArg_struct _tmp82A;(_tmp82A.tag=0,((_tmp82A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1DB)),((_tmp82B[0]=& _tmp82A,Cyc_fprintf(Cyc_stderr,((_tmp82C="oops! %s.topt = null!\n",_tag_dyneither(_tmp82C,sizeof(char),23))),_tag_dyneither(_tmp82B,sizeof(void*),1)))))));};}{
# 1382
void*_tmp2ED=(void*)_check_null(_tmp1DB->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2ED)){
# 1385
struct _tuple16 _tmp82D;return(_tmp82D.f1=_tmp2E5,((_tmp82D.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp82D)));}
# 1387
if(Cyc_Absyn_is_require_union_type(_tmp2ED))
Cyc_NewControlFlow_check_requires(_tmp1DB->loc,_tmp17F->r,_tmp2ED,_tmp1DC,_tmp2E5);{
# 1390
struct _tuple15 _tmp2EF=Cyc_CfFlowInfo_unname_rval(_tmp17F->r,_tmp2E6);void*_tmp2F1;struct _tuple15 _tmp2F0=_tmp2EF;_tmp2F1=_tmp2F0.f1;{
void*_tmp2F2=_tmp2F1;int _tmp2F4;int _tmp2F5;struct _dyneither_ptr _tmp2F6;_LL1BF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F3=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F2;if(_tmp2F3->tag != 6)goto _LL1C1;else{_tmp2F4=(_tmp2F3->f1).is_union;_tmp2F5=(_tmp2F3->f1).fieldnum;_tmp2F6=_tmp2F3->f2;}}_LL1C0: {
# 1393
int _tmp2F7=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1DB->topt),_tmp1DC);
if((_tmp2F4  && _tmp2F5 != - 1) && _tmp2F5 != _tmp2F7){
struct _tuple16 _tmp82E;return(_tmp82E.f1=_tmp2E5,((_tmp82E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),1,_tmp17F->unknown_none),_tmp82E)));}{
struct _tuple16 _tmp82F;return(_tmp82F.f1=_tmp2E5,((_tmp82F.f2=*((void**)_check_dyneither_subscript(_tmp2F6,sizeof(void*),_tmp2F7)),_tmp82F)));};}_LL1C1:;_LL1C2:
# 1398
{const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_fprintf(Cyc_stderr,((_tmp832="the bad rexp is :",_tag_dyneither(_tmp832,sizeof(char),18))),_tag_dyneither(_tmp831,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2F1);
{const char*_tmp835;void*_tmp834;(_tmp834=0,Cyc_fprintf(Cyc_stderr,((_tmp835="\n",_tag_dyneither(_tmp835,sizeof(char),2))),_tag_dyneither(_tmp834,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp842;const char*_tmp841;void*_tmp840[1];struct Cyc_String_pa_PrintArg_struct _tmp83F;struct Cyc_Core_Impossible_exn_struct*_tmp83E;(int)_throw((void*)((_tmp83E=_cycalloc(sizeof(*_tmp83E)),((_tmp83E[0]=((_tmp842.tag=Cyc_Core_Impossible,((_tmp842.f1=(struct _dyneither_ptr)((_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp840[0]=& _tmp83F,Cyc_aprintf(((_tmp841="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp841,sizeof(char),26))),_tag_dyneither(_tmp840,sizeof(void*),1)))))))),_tmp842)))),_tmp83E)))));};_LL1BE:;};};};}_LL142: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1DD->tag != 37)goto _LL144;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}_LL143: {
# 1408
struct _tuple16 _tmp303=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DE);union Cyc_CfFlowInfo_FlowInfo _tmp305;void*_tmp306;struct _tuple16 _tmp304=_tmp303;_tmp305=_tmp304.f1;_tmp306=_tmp304.f2;
# 1410
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1DE->topt))){
struct _tuple16 _tmp843;return(_tmp843.f1=_tmp305,((_tmp843.f2=_tmp17F->unknown_all,_tmp843)));}{
struct _tuple15 _tmp308=Cyc_CfFlowInfo_unname_rval(_tmp17F->r,_tmp306);void*_tmp30A;struct _tuple15 _tmp309=_tmp308;_tmp30A=_tmp309.f1;{
void*_tmp30B=_tmp30A;int _tmp30D;int _tmp30E;struct _dyneither_ptr _tmp30F;_LL1C4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30B;if(_tmp30C->tag != 6)goto _LL1C6;else{_tmp30D=(_tmp30C->f1).is_union;_tmp30E=(_tmp30C->f1).fieldnum;_tmp30F=_tmp30C->f2;}}_LL1C5: {
# 1415
int _tmp310=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1DE->topt),_tmp1DF);
if(_tmp30D  && _tmp30E != - 1){
if(_tmp30E != _tmp310){
struct _tuple16 _tmp844;return(_tmp844.f1=_tmp305,((_tmp844.f2=_tmp17F->zero,_tmp844)));}else{
# 1420
struct _tuple16 _tmp845;return(_tmp845.f1=_tmp305,((_tmp845.f2=_tmp17F->notzeroall,_tmp845)));}}else{
# 1422
struct _tuple16 _tmp846;return(_tmp846.f1=_tmp305,((_tmp846.f2=_tmp17F->unknown_all,_tmp846)));}}_LL1C6:;_LL1C7: {
# 1424
struct Cyc_Core_Impossible_exn_struct _tmp853;const char*_tmp852;void*_tmp851[1];struct Cyc_String_pa_PrintArg_struct _tmp850;struct Cyc_Core_Impossible_exn_struct*_tmp84F;(int)_throw((void*)((_tmp84F=_cycalloc(sizeof(*_tmp84F)),((_tmp84F[0]=((_tmp853.tag=Cyc_Core_Impossible,((_tmp853.f1=(struct _dyneither_ptr)((_tmp850.tag=0,((_tmp850.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp851[0]=& _tmp850,Cyc_aprintf(((_tmp852="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp852,sizeof(char),26))),_tag_dyneither(_tmp851,sizeof(void*),1)))))))),_tmp853)))),_tmp84F)))));}_LL1C3:;};};}_LL144: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1E0=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1E0->tag != 21)goto _LL146;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LL145: {
# 1429
struct _tuple16 _tmp319=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E1);union Cyc_CfFlowInfo_FlowInfo _tmp31B;void*_tmp31C;struct _tuple16 _tmp31A=_tmp319;_tmp31B=_tmp31A.f1;_tmp31C=_tmp31A.f2;{
# 1432
struct _tuple16 _tmp31D=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp31B,_tmp1E1,_tmp31C);union Cyc_CfFlowInfo_FlowInfo _tmp31F;void*_tmp320;struct _tuple16 _tmp31E=_tmp31D;_tmp31F=_tmp31E.f1;_tmp320=_tmp31E.f2;{
# 1435
void*_tmp321=Cyc_Tcutil_compress((void*)_check_null(_tmp1E1->topt));void*_tmp322=_tmp321;void*_tmp324;_LL1C9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp323=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp322;if(_tmp323->tag != 5)goto _LL1CB;else{_tmp324=(_tmp323->f1).elt_typ;}}_LL1CA:
# 1437
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp324)){
# 1439
struct _tuple16 _tmp854;return(_tmp854.f1=_tmp31F,((_tmp854.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp854)));}
# 1441
if(Cyc_Absyn_is_require_union_type(_tmp324))
Cyc_NewControlFlow_check_requires(_tmp1E1->loc,_tmp17F->r,_tmp324,_tmp1E2,_tmp31B);{
# 1444
struct _tuple15 _tmp326=Cyc_CfFlowInfo_unname_rval(_tmp17F->r,_tmp320);void*_tmp328;struct _tuple15 _tmp327=_tmp326;_tmp328=_tmp327.f1;{
void*_tmp329=_tmp328;int _tmp32B;int _tmp32C;struct _dyneither_ptr _tmp32D;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp32A=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp329;if(_tmp32A->tag != 6)goto _LL1D0;else{_tmp32B=(_tmp32A->f1).is_union;_tmp32C=(_tmp32A->f1).fieldnum;_tmp32D=_tmp32A->f2;}}_LL1CF: {
# 1447
int _tmp32E=Cyc_CfFlowInfo_get_field_index(_tmp324,_tmp1E2);
if((_tmp32B  && _tmp32C != - 1) && _tmp32C != _tmp32E){
struct _tuple16 _tmp855;return(_tmp855.f1=_tmp31F,((_tmp855.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),1,_tmp17F->unknown_none),_tmp855)));}{
struct _tuple16 _tmp856;return(_tmp856.f1=_tmp31F,((_tmp856.f2=*((void**)_check_dyneither_subscript(_tmp32D,sizeof(void*),_tmp32E)),_tmp856)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp85C;const char*_tmp85B;struct Cyc_Core_Impossible_exn_struct*_tmp85A;(int)_throw((void*)((_tmp85A=_cycalloc(sizeof(*_tmp85A)),((_tmp85A[0]=((_tmp85C.tag=Cyc_Core_Impossible,((_tmp85C.f1=((_tmp85B="anal_Rexp: AggrArrow",_tag_dyneither(_tmp85B,sizeof(char),21))),_tmp85C)))),_tmp85A)))));}_LL1CD:;};};_LL1CB:;_LL1CC: {
# 1453
struct Cyc_Core_Impossible_exn_struct _tmp862;const char*_tmp861;struct Cyc_Core_Impossible_exn_struct*_tmp860;(int)_throw((void*)((_tmp860=_cycalloc(sizeof(*_tmp860)),((_tmp860[0]=((_tmp862.tag=Cyc_Core_Impossible,((_tmp862.f1=((_tmp861="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp861,sizeof(char),25))),_tmp862)))),_tmp860)))));}_LL1C8:;};};}_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1E3->tag != 5)goto _LL148;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;_tmp1E6=_tmp1E3->f3;}}_LL147: {
# 1457
struct _tuple19 _tmp337=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E4);union Cyc_CfFlowInfo_FlowInfo _tmp339;union Cyc_CfFlowInfo_FlowInfo _tmp33A;struct _tuple19 _tmp338=_tmp337;_tmp339=_tmp338.f1;_tmp33A=_tmp338.f2;{
struct _tuple16 _tmp33B=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp339,_tmp1E5);
struct _tuple16 _tmp33C=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33A,_tmp1E6);
# 1461
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17F,env->all_changed,_tmp33B,_tmp33C);};}_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1E7=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1E7->tag != 6)goto _LL14A;else{_tmp1E8=_tmp1E7->f1;_tmp1E9=_tmp1E7->f2;}}_LL149: {
# 1464
struct _tuple19 _tmp33D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E8);union Cyc_CfFlowInfo_FlowInfo _tmp33F;union Cyc_CfFlowInfo_FlowInfo _tmp340;struct _tuple19 _tmp33E=_tmp33D;_tmp33F=_tmp33E.f1;_tmp340=_tmp33E.f2;{
struct _tuple16 _tmp341=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33F,_tmp1E9);union Cyc_CfFlowInfo_FlowInfo _tmp343;void*_tmp344;struct _tuple16 _tmp342=_tmp341;_tmp343=_tmp342.f1;_tmp344=_tmp342.f2;{
struct _tuple16 _tmp863;struct _tuple16 _tmp345=(_tmp863.f1=_tmp343,((_tmp863.f2=_tmp344,_tmp863)));
struct _tuple16 _tmp864;struct _tuple16 _tmp346=(_tmp864.f1=_tmp340,((_tmp864.f2=_tmp17F->zero,_tmp864)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17F,env->all_changed,_tmp345,_tmp346);};};}_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1EA=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1EA->tag != 7)goto _LL14C;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;}}_LL14B: {
# 1471
struct _tuple19 _tmp349=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp34B;union Cyc_CfFlowInfo_FlowInfo _tmp34C;struct _tuple19 _tmp34A=_tmp349;_tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;{
struct _tuple16 _tmp34D=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp34C,_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp34F;void*_tmp350;struct _tuple16 _tmp34E=_tmp34D;_tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;{
struct _tuple16 _tmp865;struct _tuple16 _tmp351=(_tmp865.f1=_tmp34F,((_tmp865.f2=_tmp350,_tmp865)));
struct _tuple16 _tmp866;struct _tuple16 _tmp352=(_tmp866.f1=_tmp34B,((_tmp866.f2=_tmp17F->notzeroall,_tmp866)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp17F,env->all_changed,_tmp351,_tmp352);};};}_LL14C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1ED->tag != 22)goto _LL14E;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;}}_LL14D: {
# 1478
struct _RegionHandle _tmp355=_new_region("temp");struct _RegionHandle*temp=& _tmp355;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp867[2];struct _tuple23 _tmp356=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp867[1]=_tmp1EF,((_tmp867[0]=_tmp1EE,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp867,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1479
union Cyc_CfFlowInfo_FlowInfo _tmp358;struct Cyc_List_List*_tmp359;struct _tuple23 _tmp357=_tmp356;_tmp358=_tmp357.f1;_tmp359=_tmp357.f2;{
# 1483
union Cyc_CfFlowInfo_FlowInfo _tmp35A=_tmp358;
{union Cyc_CfFlowInfo_FlowInfo _tmp35B=_tmp358;struct Cyc_Dict_Dict _tmp35C;struct Cyc_List_List*_tmp35D;_LL1D3: if((_tmp35B.ReachableFL).tag != 2)goto _LL1D5;_tmp35C=((struct _tuple14)(_tmp35B.ReachableFL).val).f1;_tmp35D=((struct _tuple14)(_tmp35B.ReachableFL).val).f2;_LL1D4:
# 1488
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp35C,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp359))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp86A;void*_tmp869;(_tmp869=0,Cyc_CfFlowInfo_aerr(_tmp1EF->loc,((_tmp86A="expression may not be initialized",_tag_dyneither(_tmp86A,sizeof(char),34))),_tag_dyneither(_tmp869,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp360=Cyc_NewControlFlow_add_subscript_reln(_tmp17F->r,_tmp35D,_tmp1EE,_tmp1EF);
if(_tmp35D != _tmp360)
_tmp35A=Cyc_CfFlowInfo_ReachableFL(_tmp35C,_tmp360);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1496
void*_tmp361=Cyc_Tcutil_compress((void*)_check_null(_tmp1EE->topt));void*_tmp362=_tmp361;struct Cyc_List_List*_tmp364;union Cyc_Absyn_Constraint*_tmp366;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp363=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp362;if(_tmp363->tag != 10)goto _LL1DA;else{_tmp364=_tmp363->f1;}}_LL1D9: {
# 1498
struct _tuple15 _tmp367=Cyc_CfFlowInfo_unname_rval(_tmp17F->r,(void*)((struct Cyc_List_List*)_check_null(_tmp359))->hd);void*_tmp369;struct _tuple15 _tmp368=_tmp367;_tmp369=_tmp368.f1;{
void*_tmp36A=_tmp369;struct _dyneither_ptr _tmp36C;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp36B=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp36A;if(_tmp36B->tag != 6)goto _LL1E1;else{_tmp36C=_tmp36B->f2;}}_LL1E0: {
# 1501
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1EF)).f1;
struct _tuple16 _tmp86B;struct _tuple16 _tmp36E=(_tmp86B.f1=_tmp35A,((_tmp86B.f2=*((void**)_check_dyneither_subscript(_tmp36C,sizeof(void*),(int)i)),_tmp86B)));_npop_handler(0);return _tmp36E;}_LL1E1:;_LL1E2: {
struct Cyc_Core_Impossible_exn_struct _tmp871;const char*_tmp870;struct Cyc_Core_Impossible_exn_struct*_tmp86F;(int)_throw((void*)((_tmp86F=_cycalloc(sizeof(*_tmp86F)),((_tmp86F[0]=((_tmp871.tag=Cyc_Core_Impossible,((_tmp871.f1=((_tmp870="anal_Rexp: Subscript",_tag_dyneither(_tmp870,sizeof(char),21))),_tmp871)))),_tmp86F)))));}_LL1DE:;};}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp365=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362;if(_tmp365->tag != 5)goto _LL1DC;else{_tmp366=((_tmp365->f1).ptr_atts).bounds;}}_LL1DB: {
# 1506
struct _tuple16 _tmp372=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp358,_tmp1EE,(void*)((struct Cyc_List_List*)_check_null(_tmp359))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp374;void*_tmp375;struct _tuple16 _tmp373=_tmp372;_tmp374=_tmp373.f1;_tmp375=_tmp373.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp376=_tmp374;_LL1E4: if((_tmp376.BottomFL).tag != 1)goto _LL1E6;_LL1E5: {
struct _tuple16 _tmp872;struct _tuple16 _tmp378=(_tmp872.f1=_tmp374,((_tmp872.f2=_tmp375,_tmp872)));_npop_handler(0);return _tmp378;}_LL1E6:;_LL1E7: {
struct _tuple16 _tmp873;struct _tuple16 _tmp37A=(_tmp873.f1=_tmp35A,((_tmp873.f2=_tmp375,_tmp873)));_npop_handler(0);return _tmp37A;}_LL1E3:;};}_LL1DC:;_LL1DD: {
# 1511
struct Cyc_Core_Impossible_exn_struct _tmp879;const char*_tmp878;struct Cyc_Core_Impossible_exn_struct*_tmp877;(int)_throw((void*)((_tmp877=_cycalloc(sizeof(*_tmp877)),((_tmp877[0]=((_tmp879.tag=Cyc_Core_Impossible,((_tmp879.f1=((_tmp878="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp878,sizeof(char),33))),_tmp879)))),_tmp877)))));}_LL1D7:;};};}
# 1479
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1F0=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1F0->tag != 30)goto _LL150;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;}}_LL14F:
# 1516
 _tmp1F4=_tmp1F1;goto _LL151;_LL150: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1F3->tag != 23)goto _LL152;else{_tmp1F4=_tmp1F3->f1;}}_LL151: {
# 1518
struct _RegionHandle _tmp37F=_new_region("temp");struct _RegionHandle*temp=& _tmp37F;_push_region(temp);
{struct _tuple23 _tmp380=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1F4,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp382;struct Cyc_List_List*_tmp383;struct _tuple23 _tmp381=_tmp380;_tmp382=_tmp381.f1;_tmp383=_tmp381.f2;{
unsigned int _tmp87F;unsigned int _tmp87E;struct _dyneither_ptr _tmp87D;void**_tmp87C;unsigned int _tmp87B;struct _dyneither_ptr aggrdict=
(_tmp87B=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1F4),((_tmp87C=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp87B)),((_tmp87D=_tag_dyneither(_tmp87C,sizeof(void*),_tmp87B),((((_tmp87E=_tmp87B,_tmp880(& _tmp87F,& _tmp87E,& _tmp87C,& _tmp383))),_tmp87D)))))));
# 1526
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp88A;struct Cyc_CfFlowInfo_UnionRInfo _tmp889;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp888;struct _tuple16 _tmp887;struct _tuple16 _tmp388=(_tmp887.f1=_tmp382,((_tmp887.f2=(void*)((_tmp88A=_region_malloc(env->r,sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp888.tag=6,((_tmp888.f1=((_tmp889.is_union=0,((_tmp889.fieldnum=- 1,_tmp889)))),((_tmp888.f2=aggrdict,_tmp888)))))),_tmp88A)))),_tmp887)));_npop_handler(0);return _tmp388;};}
# 1519
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1F5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1F5->tag != 29)goto _LL154;else{_tmp1F6=_tmp1F5->f2;}}_LL153: {
# 1529
struct Cyc_List_List*fs;
{void*_tmp38E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp38F=_tmp38E;struct Cyc_List_List*_tmp391;_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp390=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38F;if(_tmp390->tag != 12)goto _LL1EB;else{_tmp391=_tmp390->f2;}}_LL1EA:
 fs=_tmp391;goto _LL1E8;_LL1EB:;_LL1EC: {
struct Cyc_Core_Impossible_exn_struct _tmp890;const char*_tmp88F;struct Cyc_Core_Impossible_exn_struct*_tmp88E;(int)_throw((void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp890.tag=Cyc_Core_Impossible,((_tmp890.f1=((_tmp88F="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp88F,sizeof(char),35))),_tmp890)))),_tmp88E)))));}_LL1E8:;}
# 1534
_tmp1F8=_tmp1F6;_tmp1F9=Cyc_Absyn_StructA;_tmp1FA=fs;goto _LL155;}_LL154: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1F7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1F7->tag != 28)goto _LL156;else{_tmp1F8=_tmp1F7->f3;if(_tmp1F7->f4 == 0)goto _LL156;_tmp1F9=(_tmp1F7->f4)->kind;if((_tmp1F7->f4)->impl == 0)goto _LL156;_tmp1FA=((_tmp1F7->f4)->impl)->fields;}}_LL155: {
# 1536
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp395=_new_region("temp");struct _RegionHandle*temp=& _tmp395;_push_region(temp);
{struct _tuple23 _tmp396=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1F8),1,1);
# 1538
union Cyc_CfFlowInfo_FlowInfo _tmp398;struct Cyc_List_List*_tmp399;struct _tuple23 _tmp397=_tmp396;_tmp398=_tmp397.f1;_tmp399=_tmp397.f2;{
# 1540
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp17F,_tmp1FA,_tmp1F9 == Cyc_Absyn_UnionA,_tmp17F->unknown_all);
# 1543
int field_no=-1;
{int i=0;for(0;_tmp399 != 0;(((_tmp399=_tmp399->tl,_tmp1F8=_tmp1F8->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp1F8))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp39A=(void*)ds->hd;void*_tmp39B=_tmp39A;struct _dyneither_ptr*_tmp39E;_LL1EE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp39C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp39B;if(_tmp39C->tag != 0)goto _LL1F0;}_LL1EF: {
struct Cyc_Core_Impossible_exn_struct _tmp896;const char*_tmp895;struct Cyc_Core_Impossible_exn_struct*_tmp894;(int)_throw((void*)((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894[0]=((_tmp896.tag=Cyc_Core_Impossible,((_tmp896.f1=((_tmp895="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp895,sizeof(char),22))),_tmp896)))),_tmp894)))));}_LL1F0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp39D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp39B;if(_tmp39D->tag != 1)goto _LL1ED;else{_tmp39E=_tmp39D->f1;}}_LL1F1:
# 1550
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1FA,_tmp39E);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp399->hd;
# 1553
if(_tmp1F9 == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp3A2=(*((struct _tuple26*)_tmp1F8->hd)).f2;
_tmp398=Cyc_NewControlFlow_use_Rval(env,_tmp3A2->loc,_tmp398,(void*)_tmp399->hd);
# 1557
Cyc_NewControlFlow_check_requires(_tmp3A2->loc,_tmp17F->r,exp_type,_tmp39E,_tmp398);}_LL1ED:;}}}{
# 1560
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp89C;struct Cyc_CfFlowInfo_UnionRInfo _tmp89B;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp89A;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3A3=(_tmp89A=_region_malloc(env->r,sizeof(*_tmp89A)),((_tmp89A[0]=((_tmp89C.tag=6,((_tmp89C.f1=((_tmp89B.is_union=_tmp1F9 == Cyc_Absyn_UnionA,((_tmp89B.fieldnum=field_no,_tmp89B)))),((_tmp89C.f2=aggrdict,_tmp89C)))))),_tmp89A)));
struct _tuple16 _tmp89D;struct _tuple16 _tmp3A5=(_tmp89D.f1=_tmp398,((_tmp89D.f2=(void*)_tmp3A3,_tmp89D)));_npop_handler(0);return _tmp3A5;};};}
# 1538
;_pop_region(temp);}_LL156: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FB=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1FB->tag != 28)goto _LL158;}_LL157: {
# 1564
struct Cyc_Core_Impossible_exn_struct _tmp8A3;const char*_tmp8A2;struct Cyc_Core_Impossible_exn_struct*_tmp8A1;(int)_throw((void*)((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A3.tag=Cyc_Core_Impossible,((_tmp8A3.f1=((_tmp8A2="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8A2,sizeof(char),31))),_tmp8A3)))),_tmp8A1)))));}_LL158: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1FC=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1FC->tag != 25)goto _LL15A;else{_tmp1FD=_tmp1FC->f1;}}_LL159: {
# 1566
struct _RegionHandle _tmp3AC=_new_region("temp");struct _RegionHandle*temp=& _tmp3AC;_push_region(temp);
{struct Cyc_List_List*_tmp3AD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1FD);
struct _tuple23 _tmp3AE=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3AD,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp3B0;struct Cyc_List_List*_tmp3B1;struct _tuple23 _tmp3AF=_tmp3AE;_tmp3B0=_tmp3AF.f1;_tmp3B1=_tmp3AF.f2;
for(0;_tmp3B1 != 0;(_tmp3B1=_tmp3B1->tl,_tmp3AD=_tmp3AD->tl)){
_tmp3B0=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3AD))->hd)->loc,_tmp3B0,(void*)_tmp3B1->hd);}{
struct _tuple16 _tmp8A4;struct _tuple16 _tmp3B3=(_tmp8A4.f1=_tmp3B0,((_tmp8A4.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,(void*)_check_null(e->topt),0,_tmp17F->unknown_all),_tmp8A4)));_npop_handler(0);return _tmp3B3;};}
# 1567
;_pop_region(temp);}_LL15A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp1FE=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp1FE->tag != 26)goto _LL15C;else{_tmp1FF=_tmp1FE->f1;_tmp200=_tmp1FE->f2;_tmp201=_tmp1FE->f3;_tmp202=_tmp1FE->f4;}}_LL15B: {
# 1575
struct _tuple16 _tmp3B4=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp200);union Cyc_CfFlowInfo_FlowInfo _tmp3B6;void*_tmp3B7;struct _tuple16 _tmp3B5=_tmp3B4;_tmp3B6=_tmp3B5.f1;_tmp3B7=_tmp3B5.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3B8=_tmp3B6;struct Cyc_Dict_Dict _tmp3B9;struct Cyc_List_List*_tmp3BA;_LL1F3: if((_tmp3B8.BottomFL).tag != 1)goto _LL1F5;_LL1F4: {
struct _tuple16 _tmp8A5;return(_tmp8A5.f1=_tmp3B6,((_tmp8A5.f2=_tmp17F->unknown_all,_tmp8A5)));}_LL1F5: if((_tmp3B8.ReachableFL).tag != 2)goto _LL1F2;_tmp3B9=((struct _tuple14)(_tmp3B8.ReachableFL).val).f1;_tmp3BA=((struct _tuple14)(_tmp3B8.ReachableFL).val).f2;_LL1F6:
# 1579
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3B9,_tmp3B7)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8A8;void*_tmp8A7;(_tmp8A7=0,Cyc_CfFlowInfo_aerr(_tmp200->loc,((_tmp8A8="expression may not be initialized",_tag_dyneither(_tmp8A8,sizeof(char),34))),_tag_dyneither(_tmp8A7,sizeof(void*),0)));}{
# 1583
struct Cyc_List_List*new_relns=_tmp3BA;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp1FF);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp200,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3BA);
# 1589
if(_tmp3BA != new_relns)
_tmp3B6=Cyc_CfFlowInfo_ReachableFL(_tmp3B9,new_relns);{
# 1593
void*_tmp3BE=_tmp3B7;_LL1F8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3BF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3BE;if(_tmp3BF->tag != 0)goto _LL1FA;}_LL1F9: {
struct _tuple16 _tmp8A9;return(_tmp8A9.f1=_tmp3B6,((_tmp8A9.f2=_tmp17F->unknown_all,_tmp8A9)));}_LL1FA: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3C0=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3BE;if(_tmp3C0->tag != 2)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3C1=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3BE;if(_tmp3C1->tag != 1)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3C2=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3BE;if(_tmp3C2->tag != 5)goto _LL200;}_LL1FF: {
# 1598
struct Cyc_List_List _tmp8AA;struct Cyc_List_List _tmp3C4=(_tmp8AA.hd=_tmp1FF,((_tmp8AA.tl=0,_tmp8AA)));
_tmp3B6=Cyc_NewControlFlow_add_vars(_tmp17F,_tmp3B6,& _tmp3C4,_tmp17F->unknown_all,e->loc,0);{
# 1601
struct _tuple16 _tmp3C5=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B6,_tmp201);union Cyc_CfFlowInfo_FlowInfo _tmp3C7;void*_tmp3C8;struct _tuple16 _tmp3C6=_tmp3C5;_tmp3C7=_tmp3C6.f1;_tmp3C8=_tmp3C6.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3C9=_tmp3C7;struct Cyc_Dict_Dict _tmp3CA;_LL203: if((_tmp3C9.BottomFL).tag != 1)goto _LL205;_LL204: {
struct _tuple16 _tmp8AB;return(_tmp8AB.f1=_tmp3C7,((_tmp8AB.f2=_tmp17F->unknown_all,_tmp8AB)));}_LL205: if((_tmp3C9.ReachableFL).tag != 2)goto _LL202;_tmp3CA=((struct _tuple14)(_tmp3C9.ReachableFL).val).f1;_LL206:
# 1605
 if(Cyc_CfFlowInfo_initlevel(_tmp17F,_tmp3CA,_tmp3C8)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8AE;void*_tmp8AD;(_tmp8AD=0,Cyc_CfFlowInfo_aerr(_tmp200->loc,((_tmp8AE="expression may not be initialized",_tag_dyneither(_tmp8AE,sizeof(char),34))),_tag_dyneither(_tmp8AD,sizeof(void*),0)));}{
struct _tuple16 _tmp8AF;return(_tmp8AF.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8AF.f2=_tmp17F->unknown_all,_tmp8AF)));};}_LL202:;}
# 1610
_tmp3B6=_tmp3C7;
goto _LL201;};}_LL200:;_LL201:
# 1613
 while(1){
struct Cyc_List_List _tmp8B0;struct Cyc_List_List _tmp3D0=(_tmp8B0.hd=_tmp1FF,((_tmp8B0.tl=0,_tmp8B0)));
_tmp3B6=Cyc_NewControlFlow_add_vars(_tmp17F,_tmp3B6,& _tmp3D0,_tmp17F->unknown_all,e->loc,0);{
struct _tuple16 _tmp3D1=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B6,_tmp201);union Cyc_CfFlowInfo_FlowInfo _tmp3D3;void*_tmp3D4;struct _tuple16 _tmp3D2=_tmp3D1;_tmp3D3=_tmp3D2.f1;_tmp3D4=_tmp3D2.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3D5=_tmp3D3;struct Cyc_Dict_Dict _tmp3D6;_LL208: if((_tmp3D5.BottomFL).tag != 1)goto _LL20A;_LL209:
 goto _LL207;_LL20A: if((_tmp3D5.ReachableFL).tag != 2)goto _LL207;_tmp3D6=((struct _tuple14)(_tmp3D5.ReachableFL).val).f1;_LL20B:
# 1620
 if(Cyc_CfFlowInfo_initlevel(_tmp17F,_tmp3D6,_tmp3D4)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8B3;void*_tmp8B2;(_tmp8B2=0,Cyc_CfFlowInfo_aerr(_tmp200->loc,((_tmp8B3="expression may not be initialized",_tag_dyneither(_tmp8B3,sizeof(char),34))),_tag_dyneither(_tmp8B2,sizeof(void*),0)));}{
struct _tuple16 _tmp8B4;return(_tmp8B4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B4.f2=_tmp17F->unknown_all,_tmp8B4)));};}_LL207:;}{
# 1625
union Cyc_CfFlowInfo_FlowInfo _tmp3DA=Cyc_CfFlowInfo_join_flow(_tmp17F,env->all_changed,_tmp3B6,_tmp3D3);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3DA,_tmp3B6))
break;
_tmp3B6=_tmp3DA;};};}{
# 1630
struct _tuple16 _tmp8B5;return(_tmp8B5.f1=_tmp3B6,((_tmp8B5.f2=_tmp17F->unknown_all,_tmp8B5)));};_LL1F7:;};};_LL1F2:;};}_LL15C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp203=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp203->tag != 27)goto _LL15E;else{_tmp204=_tmp203->f1;_tmp205=(void*)_tmp203->f2;_tmp206=_tmp203->f3;}}_LL15D: {
# 1636
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8B8;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8B7;void*root=(void*)((_tmp8B7=_region_malloc(_tmp17F->r,sizeof(*_tmp8B7)),((_tmp8B7[0]=((_tmp8B8.tag=1,((_tmp8B8.f1=_tmp204,((_tmp8B8.f2=(void*)_check_null(e->topt),_tmp8B8)))))),_tmp8B7))));
struct Cyc_CfFlowInfo_Place*_tmp8B9;struct Cyc_CfFlowInfo_Place*place=(_tmp8B9=_region_malloc(_tmp17F->r,sizeof(*_tmp8B9)),((_tmp8B9->root=root,((_tmp8B9->fields=0,_tmp8B9)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8BC;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8BB;void*rval=(void*)((_tmp8BB=_region_malloc(_tmp17F->r,sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BC.tag=5,((_tmp8BC.f1=place,_tmp8BC)))),_tmp8BB))));
void*place_val;
# 1644
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp17F,_tmp205,0,_tmp17F->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp3DD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp204);union Cyc_CfFlowInfo_FlowInfo _tmp3DF;struct _tuple16 _tmp3DE=_tmp3DD;_tmp3DF=_tmp3DE.f1;
outflow=_tmp3DF;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E0=outflow;struct Cyc_Dict_Dict _tmp3E1;struct Cyc_List_List*_tmp3E2;_LL20D: if((_tmp3E0.BottomFL).tag != 1)goto _LL20F;_LL20E: {
struct _tuple16 _tmp8BD;return(_tmp8BD.f1=outflow,((_tmp8BD.f2=rval,_tmp8BD)));}_LL20F: if((_tmp3E0.ReachableFL).tag != 2)goto _LL20C;_tmp3E1=((struct _tuple14)(_tmp3E0.ReachableFL).val).f1;_tmp3E2=((struct _tuple14)(_tmp3E0.ReachableFL).val).f2;_LL210: {
# 1652
struct _tuple16 _tmp8BE;return(_tmp8BE.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E1,root,place_val),_tmp3E2),((_tmp8BE.f2=rval,_tmp8BE)));}_LL20C:;};};};}_LL15E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp207=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp207->tag != 36)goto _LL160;else{_tmp208=_tmp207->f1;}}_LL15F: {
# 1656
struct _tuple17 _tmp8BF;struct _tuple17 _tmp3EA=(_tmp8BF.f1=(env->fenv)->unknown_all,((_tmp8BF.f2=copy_ctxt,_tmp8BF)));
union Cyc_CfFlowInfo_FlowInfo _tmp3EB=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp208,& _tmp3EA);
struct _tuple16 _tmp8C0;return(_tmp8C0.f1=_tmp3EB,((_tmp8C0.f2=_tmp3EA.f1,_tmp8C0)));}_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp209=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp209->tag != 1)goto _LL162;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp20A=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp209->f2);if(_tmp20A->tag != 0)goto _LL162;}}_LL161:
# 1660
 goto _LL163;_LL162: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp20B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp20B->tag != 35)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp20C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp20C->tag != 24)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp20D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp20D->tag != 38)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp20E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp197;if(_tmp20E->tag != 39)goto _LL101;}_LL169: {
# 1665
struct Cyc_Core_Impossible_exn_struct _tmp8C6;const char*_tmp8C5;struct Cyc_Core_Impossible_exn_struct*_tmp8C4;(int)_throw((void*)((_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4[0]=((_tmp8C6.tag=Cyc_Core_Impossible,((_tmp8C6.f1=((_tmp8C5="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8C5,sizeof(char),31))),_tmp8C6)))),_tmp8C4)))));}_LL101:;};}
# 1675
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1684
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F1=env->fenv;
void*_tmp3F2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3F3=_tmp3F2;void*_tmp3F5;union Cyc_Absyn_Constraint*_tmp3F6;union Cyc_Absyn_Constraint*_tmp3F7;_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F3;if(_tmp3F4->tag != 5)goto _LL214;else{_tmp3F5=(_tmp3F4->f1).elt_typ;_tmp3F6=((_tmp3F4->f1).ptr_atts).bounds;_tmp3F7=((_tmp3F4->f1).ptr_atts).zero_term;}}_LL213: {
# 1687
union Cyc_CfFlowInfo_FlowInfo _tmp3F8=f;struct Cyc_Dict_Dict _tmp3F9;struct Cyc_List_List*_tmp3FA;_LL217: if((_tmp3F8.BottomFL).tag != 1)goto _LL219;_LL218: {
struct _tuple18 _tmp8C7;return(_tmp8C7.f1=f,((_tmp8C7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8C7)));}_LL219: if((_tmp3F8.ReachableFL).tag != 2)goto _LL216;_tmp3F9=((struct _tuple14)(_tmp3F8.ReachableFL).val).f1;_tmp3FA=((struct _tuple14)(_tmp3F8.ReachableFL).val).f2;_LL21A:
# 1691
 if(Cyc_Tcutil_is_bound_one(_tmp3F6)){
struct _tuple15 _tmp3FC=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp3FE;struct Cyc_List_List*_tmp3FF;struct _tuple15 _tmp3FD=_tmp3FC;_tmp3FE=_tmp3FD.f1;_tmp3FF=_tmp3FD.f2;
retry: {void*_tmp400=_tmp3FE;void*_tmp405;struct Cyc_List_List*_tmp406;enum Cyc_CfFlowInfo_InitLevel _tmp409;_LL21C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp401=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp401->tag != 8)goto _LL21E;}_LL21D: {
# 1695
struct Cyc_Core_Impossible_exn_struct _tmp8CD;const char*_tmp8CC;struct Cyc_Core_Impossible_exn_struct*_tmp8CB;(int)_throw((void*)((_tmp8CB=_cycalloc(sizeof(*_tmp8CB)),((_tmp8CB[0]=((_tmp8CD.tag=Cyc_Core_Impossible,((_tmp8CD.f1=((_tmp8CC="named location in anal_derefL",_tag_dyneither(_tmp8CC,sizeof(char),30))),_tmp8CD)))),_tmp8CB)))));}_LL21E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp402=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp402->tag != 1)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp403=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp403->tag != 2)goto _LL222;}_LL221:
# 1698
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8D0;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8CF;e->annot=(void*)((_tmp8CF=_cycalloc(sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D0.tag=Cyc_CfFlowInfo_NotZero,((_tmp8D0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8D0)))),_tmp8CF))));}goto _LL21B;_LL222: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp404=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp404->tag != 5)goto _LL224;else{_tmp405=(_tmp404->f1)->root;_tmp406=(_tmp404->f1)->fields;}}_LL223:
# 1701
 if(expanded_place){
# 1704
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8D3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8D2;e->annot=(void*)((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8D3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8D3)))),_tmp8D2))));}else{
# 1707
void*_tmp411=e->annot;void*_tmp412=_tmp411;_LL22B: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp413=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp412;if(_tmp413->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL22D;}_LL22C:
# 1711
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8D6;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8D5;e->annot=(void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D6.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8D6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8D6)))),_tmp8D5))));}
goto _LL22A;_LL22D:;_LL22E:
# 1714
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8D9;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8D8;e->annot=(void*)((_tmp8D8=_cycalloc(sizeof(*_tmp8D8)),((_tmp8D8[0]=((_tmp8D9.tag=Cyc_CfFlowInfo_NotZero,((_tmp8D9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8D9)))),_tmp8D8))));}
goto _LL22A;_LL22A:;}{
# 1718
struct Cyc_CfFlowInfo_Place*_tmp8DC;struct _tuple18 _tmp8DB;return(_tmp8DB.f1=f,((_tmp8DB.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8DC=_region_malloc(_tmp3F1->r,sizeof(*_tmp8DC)),((_tmp8DC->root=_tmp405,((_tmp8DC->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3F1->r,_tmp406,flds),_tmp8DC))))))),_tmp8DB)));};_LL224: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp407=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp407->tag != 0)goto _LL226;}_LL225:
# 1720
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp8DD;return(_tmp8DD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8DD.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8DD)));};_LL226: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp408=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp400;if(_tmp408->tag != 3)goto _LL228;else{_tmp409=_tmp408->f1;}}_LL227:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp409,_tmp3FF);goto _LL229;_LL228:;_LL229:
# 1725
 if(passthrough_consumes){
void*_tmp41B=_tmp3FE;void*_tmp41D;_LL230: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp41C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp41B;if(_tmp41C->tag != 7)goto _LL232;else{_tmp41D=(void*)_tmp41C->f3;}}_LL231:
 _tmp3FE=_tmp41D;goto retry;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 1731
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8E0;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8DF;e->annot=(void*)((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E0.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8E0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8E0)))),_tmp8DF))));};_LL21B:;}}else{
# 1735
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8E3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8E2;e->annot=(void*)((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8E3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FA),_tmp8E3)))),_tmp8E2))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3F1,_tmp3F9,r)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8E6;void*_tmp8E5;(_tmp8E5=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8E6="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp8E6,sizeof(char),46))),_tag_dyneither(_tmp8E5,sizeof(void*),0)));}{
struct _tuple18 _tmp8E7;return(_tmp8E7.f1=f,((_tmp8E7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8E7)));};_LL216:;}_LL214:;_LL215: {
# 1740
struct Cyc_Core_Impossible_exn_struct _tmp8ED;const char*_tmp8EC;struct Cyc_Core_Impossible_exn_struct*_tmp8EB;(int)_throw((void*)((_tmp8EB=_cycalloc(sizeof(*_tmp8EB)),((_tmp8EB[0]=((_tmp8ED.tag=Cyc_Core_Impossible,((_tmp8ED.f1=((_tmp8EC="left deref of non-pointer-type",_tag_dyneither(_tmp8EC,sizeof(char),31))),_tmp8ED)))),_tmp8EB)))));}_LL211:;}
# 1748
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1751
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp428=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp429=inflow;struct Cyc_Dict_Dict _tmp42A;struct Cyc_List_List*_tmp42B;_LL235: if((_tmp429.BottomFL).tag != 1)goto _LL237;_LL236: {
struct _tuple18 _tmp8EE;return(_tmp8EE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8EE.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8EE)));}_LL237: if((_tmp429.ReachableFL).tag != 2)goto _LL234;_tmp42A=((struct _tuple14)(_tmp429.ReachableFL).val).f1;_tmp42B=((struct _tuple14)(_tmp429.ReachableFL).val).f2;_LL238:
# 1756
 d=_tmp42A;_LL234:;}{
# 1760
void*_tmp42D=e->r;void*_tmp42E=_tmp42D;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Exp*_tmp434;struct Cyc_Absyn_Vardecl*_tmp437;struct Cyc_Absyn_Vardecl*_tmp43A;struct Cyc_Absyn_Vardecl*_tmp43D;struct Cyc_Absyn_Vardecl*_tmp440;struct Cyc_Absyn_Exp*_tmp442;struct _dyneither_ptr*_tmp443;struct Cyc_Absyn_Exp*_tmp445;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*_tmp44A;struct _dyneither_ptr*_tmp44B;_LL23A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp42F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp42F->tag != 13)goto _LL23C;else{_tmp430=_tmp42F->f2;}}_LL23B:
 _tmp432=_tmp430;goto _LL23D;_LL23C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp431=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp431->tag != 11)goto _LL23E;else{_tmp432=_tmp431->f1;}}_LL23D:
 _tmp434=_tmp432;goto _LL23F;_LL23E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp433=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp433->tag != 12)goto _LL240;else{_tmp434=_tmp433->f1;}}_LL23F:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp434,flds);_LL240: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp435=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp435->tag != 1)goto _LL242;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp436=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp435->f2);if(_tmp436->tag != 3)goto _LL242;else{_tmp437=_tmp436->f1;}}}_LL241:
# 1765
 _tmp43A=_tmp437;goto _LL243;_LL242: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp438=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp438->tag != 1)goto _LL244;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp439=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp438->f2);if(_tmp439->tag != 4)goto _LL244;else{_tmp43A=_tmp439->f1;}}}_LL243:
 _tmp43D=_tmp43A;goto _LL245;_LL244: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp43B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp43B->tag != 1)goto _LL246;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp43C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp43B->f2);if(_tmp43C->tag != 5)goto _LL246;else{_tmp43D=_tmp43C->f1;}}}_LL245: {
# 1768
struct Cyc_CfFlowInfo_Place*_tmp8F8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp8F7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp8F6;struct _tuple18 _tmp8F5;return(_tmp8F5.f1=inflow,((_tmp8F5.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8F8=_region_malloc(env->r,sizeof(*_tmp8F8)),((_tmp8F8->root=(void*)((_tmp8F6=_region_malloc(env->r,sizeof(*_tmp8F6)),((_tmp8F6[0]=((_tmp8F7.tag=0,((_tmp8F7.f1=_tmp43D,_tmp8F7)))),_tmp8F6)))),((_tmp8F8->fields=flds,_tmp8F8))))))),_tmp8F5)));}_LL246: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp43E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp43E->tag != 1)goto _LL248;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp43F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp43E->f2);if(_tmp43F->tag != 1)goto _LL248;else{_tmp440=_tmp43F->f1;}}}_LL247: {
# 1770
struct _tuple18 _tmp8F9;return(_tmp8F9.f1=inflow,((_tmp8F9.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F9)));}_LL248: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp441=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp441->tag != 21)goto _LL24A;else{_tmp442=_tmp441->f1;_tmp443=_tmp441->f2;}}_LL249:
# 1773
{void*_tmp451=Cyc_Tcutil_compress((void*)_check_null(_tmp442->topt));void*_tmp452=_tmp451;void*_tmp454;_LL253: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp452;if(_tmp453->tag != 5)goto _LL255;else{_tmp454=(_tmp453->f1).elt_typ;}}_LL254:
# 1775
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp454)){
Cyc_NewControlFlow_check_requires(_tmp442->loc,_tmp428->r,_tmp454,_tmp443,inflow);{
struct Cyc_List_List*_tmp8FA;flds=((_tmp8FA=_region_malloc(env->r,sizeof(*_tmp8FA)),((_tmp8FA->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp454,_tmp443),((_tmp8FA->tl=flds,_tmp8FA))))));};}
# 1779
goto _LL252;_LL255:;_LL256: {
struct Cyc_Core_Impossible_exn_struct _tmp900;const char*_tmp8FF;struct Cyc_Core_Impossible_exn_struct*_tmp8FE;(int)_throw((void*)((_tmp8FE=_cycalloc(sizeof(*_tmp8FE)),((_tmp8FE[0]=((_tmp900.tag=Cyc_Core_Impossible,((_tmp900.f1=((_tmp8FF="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp8FF,sizeof(char),25))),_tmp900)))),_tmp8FE)))));}_LL252:;}
# 1782
_tmp445=_tmp442;goto _LL24B;_LL24A: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp444=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp444->tag != 19)goto _LL24C;else{_tmp445=_tmp444->f1;}}_LL24B:
# 1785
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp445->topt),1)){
# 1787
struct _tuple18 _tmp459=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp445->topt),1),passthrough_consumes,_tmp445);
# 1787
union Cyc_CfFlowInfo_FlowInfo _tmp45B;union Cyc_CfFlowInfo_AbsLVal _tmp45C;struct _tuple18 _tmp45A=_tmp459;_tmp45B=_tmp45A.f1;_tmp45C=_tmp45A.f2;{
# 1791
struct _tuple18 _tmp901;struct _tuple18 _tmp45D=(_tmp901.f1=_tmp45B,((_tmp901.f2=_tmp45C,_tmp901)));struct _tuple18 _tmp45E=_tmp45D;struct Cyc_Dict_Dict _tmp45F;struct Cyc_List_List*_tmp460;struct Cyc_CfFlowInfo_Place*_tmp461;_LL258: if(((_tmp45E.f1).ReachableFL).tag != 2)goto _LL25A;_tmp45F=((struct _tuple14)((_tmp45E.f1).ReachableFL).val).f1;_tmp460=((struct _tuple14)((_tmp45E.f1).ReachableFL).val).f2;if(((_tmp45E.f2).PlaceL).tag != 1)goto _LL25A;_tmp461=(struct Cyc_CfFlowInfo_Place*)((_tmp45E.f2).PlaceL).val;_LL259: {
# 1793
void*_tmp462=Cyc_CfFlowInfo_lookup_place(_tmp45F,_tmp461);
struct _tuple15 _tmp463=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp462);void*_tmp465;struct Cyc_List_List*_tmp466;struct _tuple15 _tmp464=_tmp463;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;{
void*_tmp467=_tmp465;struct Cyc_Absyn_Vardecl*_tmp469;void*_tmp46A;_LL25D: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp468=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp467;if(_tmp468->tag != 8)goto _LL25F;else{_tmp469=_tmp468->f1;_tmp46A=(void*)_tmp468->f2;}}_LL25E: {
# 1797
struct Cyc_Core_Impossible_exn_struct _tmp907;const char*_tmp906;struct Cyc_Core_Impossible_exn_struct*_tmp905;(int)_throw((void*)((_tmp905=_cycalloc(sizeof(*_tmp905)),((_tmp905[0]=((_tmp907.tag=Cyc_Core_Impossible,((_tmp907.f1=((_tmp906="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp906,sizeof(char),38))),_tmp907)))),_tmp905)))));}_LL25F: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp46B=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp467;if(_tmp46B->tag != 7)goto _LL261;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp46C=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp46B->f3);if(_tmp46C->tag != 5)goto _LL261;}}_LL260:
 goto _LL262;_LL261: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp46D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp467;if(_tmp46D->tag != 5)goto _LL263;}_LL262:
# 1802
 return Cyc_NewControlFlow_anal_derefL(env,_tmp45B,_tmp445,_tmp45B,_tmp462,passthrough_consumes,0,flds);_LL263:;_LL264: {
# 1809
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp428,_tmp45F,_tmp465);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp428->unknown_all: _tmp428->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp428,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp445->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp90A;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp909;void*new_root=(void*)((_tmp909=_region_malloc(_tmp428->r,sizeof(*_tmp909)),((_tmp909[0]=((_tmp90A.tag=1,((_tmp90A.f1=e,((_tmp90A.f2=(void*)_check_null(e->topt),_tmp90A)))))),_tmp909))));
struct Cyc_CfFlowInfo_Place*_tmp90B;struct Cyc_CfFlowInfo_Place*place=(_tmp90B=_region_malloc(_tmp428->r,sizeof(*_tmp90B)),((_tmp90B->root=new_root,((_tmp90B->fields=0,_tmp90B)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp90E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp90D;void*res=(void*)((_tmp90D=_region_malloc(_tmp428->r,sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=5,((_tmp90E.f1=place,_tmp90E)))),_tmp90D))));
for(0;_tmp466 != 0;_tmp466=_tmp466->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp911;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp910;res=(void*)((_tmp910=_region_malloc(_tmp428->r,sizeof(*_tmp910)),((_tmp910[0]=((_tmp911.tag=8,((_tmp911.f1=(struct Cyc_Absyn_Vardecl*)_tmp466->hd,((_tmp911.f2=res,_tmp911)))))),_tmp910))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp45F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp45F,new_root,new_rval);
_tmp45F=Cyc_CfFlowInfo_assign_place(_tmp428,e->loc,_tmp45F,env->all_changed,_tmp461,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp473=Cyc_CfFlowInfo_ReachableFL(_tmp45F,_tmp460);
# 1827
return Cyc_NewControlFlow_anal_derefL(env,_tmp473,_tmp445,_tmp473,res,passthrough_consumes,1,flds);};}_LL25C:;};}_LL25A:;_LL25B:
# 1830
 goto _LL257;_LL257:;};}{
# 1833
struct _tuple16 _tmp47A=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp445);union Cyc_CfFlowInfo_FlowInfo _tmp47C;void*_tmp47D;struct _tuple16 _tmp47B=_tmp47A;_tmp47C=_tmp47B.f1;_tmp47D=_tmp47B.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp445,_tmp47C,_tmp47D,passthrough_consumes,0,flds);};_LL24C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp446=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp446->tag != 22)goto _LL24E;else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL24D: {
# 1837
void*_tmp47E=Cyc_Tcutil_compress((void*)_check_null(_tmp447->topt));void*_tmp47F=_tmp47E;union Cyc_Absyn_Constraint*_tmp482;_LL266: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp47F;if(_tmp480->tag != 10)goto _LL268;}_LL267: {
# 1839
unsigned int _tmp483=(Cyc_Evexp_eval_const_uint_exp(_tmp448)).f1;
struct Cyc_List_List*_tmp912;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp447,((_tmp912=_region_malloc(env->r,sizeof(*_tmp912)),((_tmp912->hd=(void*)((int)_tmp483),((_tmp912->tl=flds,_tmp912)))))));}_LL268: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp47F;if(_tmp481->tag != 5)goto _LL26A;else{_tmp482=((_tmp481->f1).ptr_atts).bounds;}}_LL269: {
# 1842
struct _RegionHandle _tmp485=_new_region("temp");struct _RegionHandle*temp=& _tmp485;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp913[2];struct _tuple23 _tmp486=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp913[1]=_tmp448,((_tmp913[0]=_tmp447,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp913,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1843
union Cyc_CfFlowInfo_FlowInfo _tmp488;struct Cyc_List_List*_tmp489;struct _tuple23 _tmp487=_tmp486;_tmp488=_tmp487.f1;_tmp489=_tmp487.f2;{
# 1845
union Cyc_CfFlowInfo_FlowInfo _tmp48A=_tmp488;
{union Cyc_CfFlowInfo_FlowInfo _tmp48B=_tmp488;struct Cyc_Dict_Dict _tmp48C;struct Cyc_List_List*_tmp48D;_LL26D: if((_tmp48B.ReachableFL).tag != 2)goto _LL26F;_tmp48C=((struct _tuple14)(_tmp48B.ReachableFL).val).f1;_tmp48D=((struct _tuple14)(_tmp48B.ReachableFL).val).f2;_LL26E:
# 1848
 if(Cyc_CfFlowInfo_initlevel(_tmp428,_tmp48C,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp489))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp916;void*_tmp915;(_tmp915=0,Cyc_CfFlowInfo_aerr(_tmp448->loc,((_tmp916="expression may not be initialized",_tag_dyneither(_tmp916,sizeof(char),34))),_tag_dyneither(_tmp915,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp490=Cyc_NewControlFlow_add_subscript_reln(_tmp428->r,_tmp48D,_tmp447,_tmp448);
if(_tmp48D != _tmp490)
_tmp48A=Cyc_CfFlowInfo_ReachableFL(_tmp48C,_tmp490);
goto _LL26C;};_LL26F:;_LL270:
 goto _LL26C;_LL26C:;}{
# 1856
struct _tuple18 _tmp491=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp447,_tmp488,(void*)((struct Cyc_List_List*)_check_null(_tmp489))->hd,passthrough_consumes,0,flds);union Cyc_CfFlowInfo_FlowInfo _tmp493;union Cyc_CfFlowInfo_AbsLVal _tmp494;struct _tuple18 _tmp492=_tmp491;_tmp493=_tmp492.f1;_tmp494=_tmp492.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp495=_tmp493;_LL272: if((_tmp495.BottomFL).tag != 1)goto _LL274;_LL273: {
struct _tuple18 _tmp917;struct _tuple18 _tmp497=(_tmp917.f1=_tmp493,((_tmp917.f2=_tmp494,_tmp917)));_npop_handler(0);return _tmp497;}_LL274:;_LL275: {
struct _tuple18 _tmp918;struct _tuple18 _tmp499=(_tmp918.f1=_tmp48A,((_tmp918.f2=_tmp494,_tmp918)));_npop_handler(0);return _tmp499;}_LL271:;};};};}
# 1843
;_pop_region(temp);}_LL26A:;_LL26B: {
# 1862
struct Cyc_Core_Impossible_exn_struct _tmp91E;const char*_tmp91D;struct Cyc_Core_Impossible_exn_struct*_tmp91C;(int)_throw((void*)((_tmp91C=_cycalloc(sizeof(*_tmp91C)),((_tmp91C[0]=((_tmp91E.tag=Cyc_Core_Impossible,((_tmp91E.f1=((_tmp91D="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp91D,sizeof(char),33))),_tmp91E)))),_tmp91C)))));}_LL265:;}_LL24E: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp449=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp42E;if(_tmp449->tag != 20)goto _LL250;else{_tmp44A=_tmp449->f1;_tmp44B=_tmp449->f2;}}_LL24F: {
# 1866
void*_tmp49E=(void*)_check_null(_tmp44A->topt);
if(Cyc_Absyn_is_require_union_type(_tmp49E))
Cyc_NewControlFlow_check_requires(_tmp44A->loc,_tmp428->r,_tmp49E,_tmp44B,inflow);
# 1870
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp49E)){
struct _tuple18 _tmp91F;return(_tmp91F.f1=inflow,((_tmp91F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp91F)));}{
# 1873
struct Cyc_List_List*_tmp920;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp44A,(
(_tmp920=_region_malloc(env->r,sizeof(*_tmp920)),((_tmp920->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp49E,_tmp44B),((_tmp920->tl=flds,_tmp920)))))));};}_LL250:;_LL251: {
# 1876
struct _tuple18 _tmp921;return(_tmp921.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp921.f2=Cyc_CfFlowInfo_UnknownL(),_tmp921)));}_LL239:;};}
# 1880
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1883
struct _tuple18 _tmp4A2=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp4A4;union Cyc_CfFlowInfo_AbsLVal _tmp4A5;struct _tuple18 _tmp4A3=_tmp4A2;_tmp4A4=_tmp4A3.f1;_tmp4A5=_tmp4A3.f2;{
struct _tuple18 _tmp922;return(_tmp922.f1=_tmp4A4,((_tmp922.f2=_tmp4A5,_tmp922)));};}
# 1890
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1893
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp4A7=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp4A9;struct _tuple18 _tmp4A8=_tmp4A7;_tmp4A9=_tmp4A8.f1;
inflow=_tmp4A9;}}}
# 1901
return inflow;}
# 1907
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1910
struct Cyc_CfFlowInfo_FlowEnv*_tmp4AA=env->fenv;
# 1913
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp4AB=_new_region("temp");struct _RegionHandle*temp=& _tmp4AB;_push_region(temp);{
struct _tuple23 _tmp4AC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp4AE;struct Cyc_List_List*_tmp4AF;struct _tuple23 _tmp4AD=_tmp4AC;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp4AF))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4AF->tl))->hd;
f=_tmp4AE;}{
# 1923
union Cyc_CfFlowInfo_FlowInfo _tmp4B0=f;struct Cyc_Dict_Dict _tmp4B1;struct Cyc_List_List*_tmp4B2;_LL277: if((_tmp4B0.BottomFL).tag != 1)goto _LL279;_LL278: {
struct _tuple19 _tmp923;struct _tuple19 _tmp4B4=(_tmp923.f1=f,((_tmp923.f2=f,_tmp923)));_npop_handler(0);return _tmp4B4;}_LL279: if((_tmp4B0.ReachableFL).tag != 2)goto _LL276;_tmp4B1=((struct _tuple14)(_tmp4B0.ReachableFL).val).f1;_tmp4B2=((struct _tuple14)(_tmp4B0.ReachableFL).val).f2;_LL27A: {
# 1926
struct Cyc_Absyn_Exp*_tmp4B5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp4B6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B1,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp926;void*_tmp925;(_tmp925=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp926="expression may not be initialized",_tag_dyneither(_tmp926,sizeof(char),34))),_tag_dyneither(_tmp925,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4B1,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp929;void*_tmp928;(_tmp928=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp929="expression may not be initialized",_tag_dyneither(_tmp929,sizeof(char),34))),_tag_dyneither(_tmp928,sizeof(void*),0)));}{
# 1933
union Cyc_CfFlowInfo_FlowInfo _tmp4BB=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4BC=f;
# 1938
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp4BD=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp4BF;struct Cyc_List_List*_tmp4C0;struct _tuple15 _tmp4BE=_tmp4BD;_tmp4BF=_tmp4BE.f1;_tmp4C0=_tmp4BE.f2;{
struct _tuple15 _tmp4C1=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp4C3;struct Cyc_List_List*_tmp4C4;struct _tuple15 _tmp4C2=_tmp4C1;_tmp4C3=_tmp4C2.f1;_tmp4C4=_tmp4C2.f2;{
struct _tuple0 _tmp92A;struct _tuple0 _tmp4C5=(_tmp92A.f1=_tmp4BF,((_tmp92A.f2=_tmp4C3,_tmp92A)));struct _tuple0 _tmp4C6=_tmp4C5;enum Cyc_CfFlowInfo_InitLevel _tmp4C8;enum Cyc_CfFlowInfo_InitLevel _tmp4CC;_LL27C:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4C7=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4C7->tag != 3)goto _LL27E;else{_tmp4C8=_tmp4C7->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4C9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4C9->tag != 0)goto _LL27E;};_LL27D: {
# 1945
struct _tuple19 _tmp4DB=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4B5,_tmp4C8,_tmp4C0);union Cyc_CfFlowInfo_FlowInfo _tmp4DD;union Cyc_CfFlowInfo_FlowInfo _tmp4DE;struct _tuple19 _tmp4DC=_tmp4DB;_tmp4DD=_tmp4DC.f1;_tmp4DE=_tmp4DC.f2;
switch(p){case Cyc_Absyn_Eq: _LL290:
 _tmp4BB=_tmp4DE;_tmp4BC=_tmp4DD;break;case Cyc_Absyn_Neq: _LL291:
 _tmp4BB=_tmp4DD;_tmp4BC=_tmp4DE;break;default: _LL292: {
struct Cyc_Core_Impossible_exn_struct _tmp930;const char*_tmp92F;struct Cyc_Core_Impossible_exn_struct*_tmp92E;(int)_throw((void*)((_tmp92E=_cycalloc(sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp930.tag=Cyc_Core_Impossible,((_tmp930.f1=((_tmp92F="anal_test, zero-split",_tag_dyneither(_tmp92F,sizeof(char),22))),_tmp930)))),_tmp92E)))));}}
# 1951
goto _LL27B;}_LL27E:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4CA->tag != 0)goto _LL280;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4CB=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4CB->tag != 3)goto _LL280;else{_tmp4CC=_tmp4CB->f1;}};_LL27F: {
# 1953
struct _tuple19 _tmp4E2=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4B6,_tmp4CC,_tmp4C4);union Cyc_CfFlowInfo_FlowInfo _tmp4E4;union Cyc_CfFlowInfo_FlowInfo _tmp4E5;struct _tuple19 _tmp4E3=_tmp4E2;_tmp4E4=_tmp4E3.f1;_tmp4E5=_tmp4E3.f2;
switch(p){case Cyc_Absyn_Eq: _LL294:
 _tmp4BB=_tmp4E5;_tmp4BC=_tmp4E4;break;case Cyc_Absyn_Neq: _LL295:
 _tmp4BB=_tmp4E4;_tmp4BC=_tmp4E5;break;default: _LL296: {
struct Cyc_Core_Impossible_exn_struct _tmp936;const char*_tmp935;struct Cyc_Core_Impossible_exn_struct*_tmp934;(int)_throw((void*)((_tmp934=_cycalloc(sizeof(*_tmp934)),((_tmp934[0]=((_tmp936.tag=Cyc_Core_Impossible,((_tmp936.f1=((_tmp935="anal_test, zero-split",_tag_dyneither(_tmp935,sizeof(char),22))),_tmp936)))),_tmp934)))));}}
# 1959
goto _LL27B;}_LL280:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4CD->tag != 0)goto _LL282;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4CE->tag != 0)goto _LL282;};_LL281:
# 1961
 if(p == Cyc_Absyn_Eq)_tmp4BC=Cyc_CfFlowInfo_BottomFL();else{
_tmp4BB=Cyc_CfFlowInfo_BottomFL();}
goto _LL27B;_LL282:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4CF=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4CF->tag != 0)goto _LL284;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4D0=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4D0->tag != 1)goto _LL284;};_LL283:
 goto _LL285;_LL284:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4D1->tag != 0)goto _LL286;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4D2=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4D2->tag != 2)goto _LL286;};_LL285:
 goto _LL287;_LL286:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4D3->tag != 0)goto _LL288;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4D4=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4D4->tag != 5)goto _LL288;};_LL287:
 goto _LL289;_LL288:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4D5->tag != 1)goto _LL28A;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D6=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4D6->tag != 0)goto _LL28A;};_LL289:
 goto _LL28B;_LL28A:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4D7->tag != 2)goto _LL28C;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4D8->tag != 0)goto _LL28C;};_LL28B:
 goto _LL28D;_LL28C:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f1;if(_tmp4D9->tag != 5)goto _LL28E;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4C6.f2;if(_tmp4DA->tag != 0)goto _LL28E;};_LL28D:
# 1970
 if(p == Cyc_Absyn_Neq)_tmp4BC=Cyc_CfFlowInfo_BottomFL();else{
_tmp4BB=Cyc_CfFlowInfo_BottomFL();}
goto _LL27B;_LL28E:;_LL28F:
 goto _LL27B;_LL27B:;};};}
# 1981
{struct _tuple0 _tmp937;struct _tuple0 _tmp4EA=(_tmp937.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp4B5->topt)),((_tmp937.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp4B6->topt)),_tmp937)));struct _tuple0 _tmp4EB=_tmp4EA;_LL299: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4EB.f1;if(_tmp4EC->tag != 6)goto _LL29B;else{if(_tmp4EC->f1 != Cyc_Absyn_Unsigned)goto _LL29B;}}_LL29A:
 goto _LL29C;_LL29B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4EB.f2;if(_tmp4ED->tag != 6)goto _LL29D;else{if(_tmp4ED->f1 != Cyc_Absyn_Unsigned)goto _LL29D;}}_LL29C:
 goto _LL29E;_LL29D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4EE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EB.f1;if(_tmp4EE->tag != 19)goto _LL29F;}_LL29E:
 goto _LL2A0;_LL29F: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4EF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4EB.f2;if(_tmp4EF->tag != 19)goto _LL2A1;}_LL2A0:
 goto _LL298;_LL2A1:;_LL2A2: {
struct _tuple19 _tmp938;struct _tuple19 _tmp4F1=(_tmp938.f1=_tmp4BB,((_tmp938.f2=_tmp4BC,_tmp938)));_npop_handler(0);return _tmp4F1;}_LL298:;}
# 1990
switch(p){case Cyc_Absyn_Eq: _LL2A3:
 goto _LL2A4;case Cyc_Absyn_Neq: _LL2A4: goto _LL2A5;case Cyc_Absyn_Gt: _LL2A5: goto _LL2A6;case Cyc_Absyn_Gte: _LL2A6: goto _LL2A7;case Cyc_Absyn_Lt: _LL2A7: goto _LL2A8;case Cyc_Absyn_Lte: _LL2A8: break;default: _LL2A9: {
struct _tuple19 _tmp939;struct _tuple19 _tmp4F4=(_tmp939.f1=_tmp4BB,((_tmp939.f2=_tmp4BC,_tmp939)));_npop_handler(0);return _tmp4F4;}}{
# 1995
struct _RegionHandle*_tmp4F5=(env->fenv)->r;
struct _tuple13 _tmp4F6=Cyc_Relations_primop2relation(_tmp4B5,p,_tmp4B6);struct Cyc_Absyn_Exp*_tmp4F8;enum Cyc_Relations_Relation _tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct _tuple13 _tmp4F7=_tmp4F6;_tmp4F8=_tmp4F7.f1;_tmp4F9=_tmp4F7.f2;_tmp4FA=_tmp4F7.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2000
if(Cyc_Relations_exp2relnop(_tmp4F8,& n1) && Cyc_Relations_exp2relnop(_tmp4FA,& n2)){
# 2002
struct Cyc_List_List*_tmp4FB=Cyc_Relations_add_relation(_tmp4F5,n1,_tmp4F9,n2,_tmp4B2);
# 2006
struct Cyc_List_List*_tmp4FC=Cyc_Relations_add_relation(_tmp4F5,n2,Cyc_Relations_flip_relation(_tmp4F9),n1,_tmp4B2);
struct _tuple19 _tmp93A;struct _tuple19 _tmp4FD=(_tmp93A.f1=_tmp4BB,((_tmp93A.f2=_tmp4BC,_tmp93A)));struct _tuple19 _tmp4FE=_tmp4FD;struct Cyc_Dict_Dict _tmp4FF;struct Cyc_Dict_Dict _tmp500;struct Cyc_Dict_Dict _tmp501;struct Cyc_Dict_Dict _tmp502;_LL2AC: if(((_tmp4FE.f1).ReachableFL).tag != 2)goto _LL2AE;_tmp4FF=((struct _tuple14)((_tmp4FE.f1).ReachableFL).val).f1;if(((_tmp4FE.f2).ReachableFL).tag != 2)goto _LL2AE;_tmp500=((struct _tuple14)((_tmp4FE.f2).ReachableFL).val).f1;_LL2AD: {
# 2009
struct _tuple19 _tmp93B;struct _tuple19 _tmp504=(_tmp93B.f1=Cyc_CfFlowInfo_ReachableFL(_tmp4FF,_tmp4FB),((_tmp93B.f2=Cyc_CfFlowInfo_ReachableFL(_tmp500,_tmp4FC),_tmp93B)));_npop_handler(0);return _tmp504;}_LL2AE: if(((_tmp4FE.f2).ReachableFL).tag != 2)goto _LL2B0;_tmp501=((struct _tuple14)((_tmp4FE.f2).ReachableFL).val).f1;_LL2AF: {
# 2011
struct _tuple19 _tmp93C;struct _tuple19 _tmp506=(_tmp93C.f1=_tmp4BB,((_tmp93C.f2=Cyc_CfFlowInfo_ReachableFL(_tmp501,_tmp4FC),_tmp93C)));_npop_handler(0);return _tmp506;}_LL2B0: if(((_tmp4FE.f1).ReachableFL).tag != 2)goto _LL2B2;_tmp502=((struct _tuple14)((_tmp4FE.f1).ReachableFL).val).f1;_LL2B1: {
# 2013
struct _tuple19 _tmp93D;struct _tuple19 _tmp508=(_tmp93D.f1=Cyc_CfFlowInfo_ReachableFL(_tmp502,_tmp4FB),((_tmp93D.f2=_tmp4BC,_tmp93D)));_npop_handler(0);return _tmp508;}_LL2B2:;_LL2B3: {
# 2015
struct _tuple19 _tmp93E;struct _tuple19 _tmp50A=(_tmp93E.f1=_tmp4BB,((_tmp93E.f2=_tmp4BC,_tmp93E)));_npop_handler(0);return _tmp50A;}_LL2AB:;}else{
# 2018
struct _tuple19 _tmp93F;struct _tuple19 _tmp50D=(_tmp93F.f1=_tmp4BB,((_tmp93F.f2=_tmp4BC,_tmp93F)));_npop_handler(0);return _tmp50D;}};};};}_LL276:;};
# 1915
;_pop_region(temp);}
# 2024
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2026
struct Cyc_CfFlowInfo_FlowEnv*_tmp50E=env->fenv;
void*_tmp50F=e->r;void*_tmp510=_tmp50F;struct Cyc_Absyn_Exp*_tmp512;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51F;enum Cyc_Absyn_Primop _tmp521;struct Cyc_List_List*_tmp522;_LL2B5: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp511=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp511->tag != 5)goto _LL2B7;else{_tmp512=_tmp511->f1;_tmp513=_tmp511->f2;_tmp514=_tmp511->f3;}}_LL2B6: {
# 2029
struct _tuple19 _tmp523=Cyc_NewControlFlow_anal_test(env,inflow,_tmp512);union Cyc_CfFlowInfo_FlowInfo _tmp525;union Cyc_CfFlowInfo_FlowInfo _tmp526;struct _tuple19 _tmp524=_tmp523;_tmp525=_tmp524.f1;_tmp526=_tmp524.f2;{
struct _tuple19 _tmp527=Cyc_NewControlFlow_anal_test(env,_tmp525,_tmp513);union Cyc_CfFlowInfo_FlowInfo _tmp529;union Cyc_CfFlowInfo_FlowInfo _tmp52A;struct _tuple19 _tmp528=_tmp527;_tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;{
struct _tuple19 _tmp52B=Cyc_NewControlFlow_anal_test(env,_tmp526,_tmp514);union Cyc_CfFlowInfo_FlowInfo _tmp52D;union Cyc_CfFlowInfo_FlowInfo _tmp52E;struct _tuple19 _tmp52C=_tmp52B;_tmp52D=_tmp52C.f1;_tmp52E=_tmp52C.f2;{
struct _tuple19 _tmp940;return(_tmp940.f1=Cyc_CfFlowInfo_join_flow(_tmp50E,env->all_changed,_tmp529,_tmp52D),((_tmp940.f2=
Cyc_CfFlowInfo_join_flow(_tmp50E,env->all_changed,_tmp52A,_tmp52E),_tmp940)));};};};}_LL2B7: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp515->tag != 6)goto _LL2B9;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f2;}}_LL2B8: {
# 2035
struct _tuple19 _tmp530=Cyc_NewControlFlow_anal_test(env,inflow,_tmp516);union Cyc_CfFlowInfo_FlowInfo _tmp532;union Cyc_CfFlowInfo_FlowInfo _tmp533;struct _tuple19 _tmp531=_tmp530;_tmp532=_tmp531.f1;_tmp533=_tmp531.f2;{
struct _tuple19 _tmp534=Cyc_NewControlFlow_anal_test(env,_tmp532,_tmp517);union Cyc_CfFlowInfo_FlowInfo _tmp536;union Cyc_CfFlowInfo_FlowInfo _tmp537;struct _tuple19 _tmp535=_tmp534;_tmp536=_tmp535.f1;_tmp537=_tmp535.f2;{
struct _tuple19 _tmp941;return(_tmp941.f1=_tmp536,((_tmp941.f2=Cyc_CfFlowInfo_join_flow(_tmp50E,env->all_changed,_tmp533,_tmp537),_tmp941)));};};}_LL2B9: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp518->tag != 7)goto _LL2BB;else{_tmp519=_tmp518->f1;_tmp51A=_tmp518->f2;}}_LL2BA: {
# 2039
struct _tuple19 _tmp539=Cyc_NewControlFlow_anal_test(env,inflow,_tmp519);union Cyc_CfFlowInfo_FlowInfo _tmp53B;union Cyc_CfFlowInfo_FlowInfo _tmp53C;struct _tuple19 _tmp53A=_tmp539;_tmp53B=_tmp53A.f1;_tmp53C=_tmp53A.f2;{
struct _tuple19 _tmp53D=Cyc_NewControlFlow_anal_test(env,_tmp53C,_tmp51A);union Cyc_CfFlowInfo_FlowInfo _tmp53F;union Cyc_CfFlowInfo_FlowInfo _tmp540;struct _tuple19 _tmp53E=_tmp53D;_tmp53F=_tmp53E.f1;_tmp540=_tmp53E.f2;{
struct _tuple19 _tmp942;return(_tmp942.f1=Cyc_CfFlowInfo_join_flow(_tmp50E,env->all_changed,_tmp53B,_tmp53F),((_tmp942.f2=_tmp540,_tmp942)));};};}_LL2BB: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp51B->tag != 8)goto _LL2BD;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;}}_LL2BC: {
# 2043
struct _tuple16 _tmp542=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp51C);union Cyc_CfFlowInfo_FlowInfo _tmp544;void*_tmp545;struct _tuple16 _tmp543=_tmp542;_tmp544=_tmp543.f1;_tmp545=_tmp543.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp544,_tmp51D);}_LL2BD: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp51E->tag != 2)goto _LL2BF;else{if(_tmp51E->f1 != Cyc_Absyn_Not)goto _LL2BF;if(_tmp51E->f2 == 0)goto _LL2BF;_tmp51F=(struct Cyc_Absyn_Exp*)(_tmp51E->f2)->hd;if((_tmp51E->f2)->tl != 0)goto _LL2BF;}}_LL2BE: {
# 2046
struct _tuple19 _tmp546=Cyc_NewControlFlow_anal_test(env,inflow,_tmp51F);union Cyc_CfFlowInfo_FlowInfo _tmp548;union Cyc_CfFlowInfo_FlowInfo _tmp549;struct _tuple19 _tmp547=_tmp546;_tmp548=_tmp547.f1;_tmp549=_tmp547.f2;{
struct _tuple19 _tmp943;return(_tmp943.f1=_tmp549,((_tmp943.f2=_tmp548,_tmp943)));};}_LL2BF: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp510;if(_tmp520->tag != 2)goto _LL2C1;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;}}_LL2C0:
# 2049
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp521,_tmp522);_LL2C1:;_LL2C2: {
# 2053
struct _tuple16 _tmp54B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp54D;void*_tmp54E;struct _tuple16 _tmp54C=_tmp54B;_tmp54D=_tmp54C.f1;_tmp54E=_tmp54C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp54F=_tmp54D;struct Cyc_Dict_Dict _tmp550;_LL2C4: if((_tmp54F.BottomFL).tag != 1)goto _LL2C6;_LL2C5: {
struct _tuple19 _tmp944;return(_tmp944.f1=_tmp54D,((_tmp944.f2=_tmp54D,_tmp944)));}_LL2C6: if((_tmp54F.ReachableFL).tag != 2)goto _LL2C3;_tmp550=((struct _tuple14)(_tmp54F.ReachableFL).val).f1;_LL2C7: {
# 2057
struct _tuple15 _tmp552=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp54E);void*_tmp554;struct Cyc_List_List*_tmp555;struct _tuple15 _tmp553=_tmp552;_tmp554=_tmp553.f1;_tmp555=_tmp553.f2;{
void*_tmp556=_tmp554;struct Cyc_Absyn_Vardecl*_tmp558;void*_tmp559;enum Cyc_CfFlowInfo_InitLevel _tmp562;_LL2C9: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp557=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp557->tag != 8)goto _LL2CB;else{_tmp558=_tmp557->f1;_tmp559=(void*)_tmp557->f2;}}_LL2CA: {
# 2060
struct Cyc_Core_Impossible_exn_struct _tmp94A;const char*_tmp949;struct Cyc_Core_Impossible_exn_struct*_tmp948;(int)_throw((void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948[0]=((_tmp94A.tag=Cyc_Core_Impossible,((_tmp94A.f1=((_tmp949="anal_test: bad namedlocation",_tag_dyneither(_tmp949,sizeof(char),29))),_tmp94A)))),_tmp948)))));}_LL2CB: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp55A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55A->tag != 0)goto _LL2CD;}_LL2CC: {
struct _tuple19 _tmp94B;return(_tmp94B.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp94B.f2=_tmp54D,_tmp94B)));}_LL2CD: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp55B=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55B->tag != 2)goto _LL2CF;}_LL2CE:
 goto _LL2D0;_LL2CF: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp55C=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55C->tag != 1)goto _LL2D1;}_LL2D0:
 goto _LL2D2;_LL2D1: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp55D=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55D->tag != 5)goto _LL2D3;}_LL2D2: {
struct _tuple19 _tmp94C;return(_tmp94C.f1=_tmp54D,((_tmp94C.f2=Cyc_CfFlowInfo_BottomFL(),_tmp94C)));}_LL2D3: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp55E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55E->tag != 3)goto _LL2D5;else{if(_tmp55E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2D5;}}_LL2D4:
 goto _LL2D6;_LL2D5: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp55F=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp55F->tag != 4)goto _LL2D7;else{if(_tmp55F->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2D7;}}_LL2D6:
 goto _LL2D8;_LL2D7: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp560=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp560->tag != 7)goto _LL2D9;}_LL2D8:
# 2068
{const char*_tmp94F;void*_tmp94E;(_tmp94E=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp94F="expression may not be initialized",_tag_dyneither(_tmp94F,sizeof(char),34))),_tag_dyneither(_tmp94E,sizeof(void*),0)));}{
struct _tuple19 _tmp950;return(_tmp950.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp950.f2=Cyc_CfFlowInfo_BottomFL(),_tmp950)));};_LL2D9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp561=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp561->tag != 3)goto _LL2DB;else{_tmp562=_tmp561->f1;}}_LL2DA:
# 2071
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp54D,e,_tmp562,_tmp555);_LL2DB: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp563=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp563->tag != 4)goto _LL2DD;}_LL2DC: {
struct _tuple19 _tmp951;return(_tmp951.f1=_tmp54D,((_tmp951.f2=_tmp54D,_tmp951)));}_LL2DD: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp564=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp556;if(_tmp564->tag != 6)goto _LL2C8;}_LL2DE: {
struct Cyc_Core_Impossible_exn_struct _tmp957;const char*_tmp956;struct Cyc_Core_Impossible_exn_struct*_tmp955;(int)_throw((void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=((_tmp957.tag=Cyc_Core_Impossible,((_tmp957.f1=((_tmp956="anal_test",_tag_dyneither(_tmp956,sizeof(char),10))),_tmp957)))),_tmp955)))));}_LL2C8:;};}_LL2C3:;};}_LL2B4:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2079
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2081
unsigned int _tmp572;struct Cyc_NewControlFlow_AnalEnv*_tmp573;struct Cyc_Dict_Dict _tmp574;struct _tuple27*_tmp571=ckenv;_tmp572=_tmp571->f1;_tmp573=_tmp571->f2;_tmp574=_tmp571->f3;{
void*_tmp575=root;struct Cyc_Absyn_Vardecl*_tmp577;_LL2E0: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp576=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp575;if(_tmp576->tag != 0)goto _LL2E2;else{_tmp577=_tmp576->f1;}}_LL2E1:
# 2084
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp573->fenv)->r,_tmp577->type)){
struct _tuple15 _tmp578=Cyc_CfFlowInfo_unname_rval((_tmp573->fenv)->r,rval);void*_tmp57A;struct _tuple15 _tmp579=_tmp578;_tmp57A=_tmp579.f1;{
void*_tmp57B=_tmp57A;_LL2E5: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp57C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp57B;if(_tmp57C->tag != 7)goto _LL2E7;}_LL2E6:
 goto _LL2E8;_LL2E7: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp57D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp57B;if(_tmp57D->tag != 0)goto _LL2E9;}_LL2E8:
 goto _LL2EA;_LL2E9: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp57E=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp57B;if(_tmp57E->tag != 3)goto _LL2EB;else{if(_tmp57E->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2EB;}}_LL2EA:
 goto _LL2E4;_LL2EB:;_LL2EC:
# 2091
{const char*_tmp95B;void*_tmp95A[1];struct Cyc_String_pa_PrintArg_struct _tmp959;(_tmp959.tag=0,((_tmp959.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp577->name)),((_tmp95A[0]=& _tmp959,Cyc_Tcutil_warn(_tmp572,((_tmp95B="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp95B,sizeof(char),57))),_tag_dyneither(_tmp95A,sizeof(void*),1)))))));}
goto _LL2E4;_LL2E4:;};}
# 2095
goto _LL2DF;_LL2E2:;_LL2E3:
 goto _LL2DF;_LL2DF:;};}
# 2100
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp582=flow;struct Cyc_Dict_Dict _tmp583;_LL2EE: if((_tmp582.BottomFL).tag != 1)goto _LL2F0;_LL2EF:
 return;_LL2F0: if((_tmp582.ReachableFL).tag != 2)goto _LL2ED;_tmp583=((struct _tuple14)(_tmp582.ReachableFL).val).f1;_LL2F1:
# 2104
{struct Cyc_List_List*_tmp584=env->param_roots;for(0;_tmp584 != 0;_tmp584=_tmp584->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp583,Cyc_CfFlowInfo_lookup_place(_tmp583,(struct Cyc_CfFlowInfo_Place*)_tmp584->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp95E;void*_tmp95D;(_tmp95D=0,Cyc_CfFlowInfo_aerr(loc,((_tmp95E="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp95E,sizeof(char),76))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}}}
# 2108
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp95F;struct _tuple27 _tmp587=(_tmp95F.f1=loc,((_tmp95F.f2=env,((_tmp95F.f3=_tmp583,_tmp95F)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp587,_tmp583);}
# 2112
return;_LL2ED:;}
# 2116
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2119
struct Cyc_List_List*_tmp589=0;
struct Cyc_List_List*_tmp58A=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp58B=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp58D;struct Cyc_Absyn_Exp*_tmp58E;struct _tuple24*_tmp58C=_tmp58B;_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;
if((_tmp58D != 0  && _tmp58E != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp58E->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp962;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp961;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp58F=(_tmp961=_region_malloc(frgn,sizeof(*_tmp961)),((_tmp961[0]=((_tmp962.tag=0,((_tmp962.f1=*_tmp58D,_tmp962)))),_tmp961)));
struct Cyc_CfFlowInfo_Place*_tmp963;struct Cyc_CfFlowInfo_Place*_tmp590=(_tmp963=_region_malloc(frgn,sizeof(*_tmp963)),((_tmp963->root=(void*)_tmp58F,((_tmp963->fields=0,_tmp963)))));
{struct Cyc_List_List*_tmp964;_tmp589=((_tmp964=_region_malloc(frgn,sizeof(*_tmp964)),((_tmp964->hd=_tmp590,((_tmp964->tl=_tmp589,_tmp964))))));}{
struct Cyc_List_List*_tmp965;_tmp58A=((_tmp965=_region_malloc(frgn,sizeof(*_tmp965)),((_tmp965->hd=_tmp58E,((_tmp965->tl=_tmp58A,_tmp965))))));};}}}{
# 2131
struct _tuple1 _tmp966;return(_tmp966.f1=_tmp589,((_tmp966.f2=_tmp58A,_tmp966)));};}
# 2140
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2145
{union Cyc_CfFlowInfo_FlowInfo _tmp597=flow;_LL2F3: if((_tmp597.BottomFL).tag != 1)goto _LL2F5;_LL2F4:
 return flow;_LL2F5:;_LL2F6:
 goto _LL2F2;_LL2F2:;}{
# 2149
int _tmp598=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp599=Cyc_Absyn_exp_unsigned_one;
int _tmp59A=0;
int _tmp59B=1;
# 2156
{struct Cyc_List_List*_tmp59C=consumed_vals;for(0;_tmp59C != 0;
(_tmp59C=_tmp59C->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp59D=flow;struct Cyc_Dict_Dict _tmp59E;_LL2F8: if((_tmp59D.ReachableFL).tag != 2)goto _LL2FA;_tmp59E=((struct _tuple14)(_tmp59D.ReachableFL).val).f1;_LL2F9: {
# 2166
void*_tmp59F=Cyc_CfFlowInfo_lookup_place(_tmp59E,(struct Cyc_CfFlowInfo_Place*)_tmp59C->hd);
void*_tmp5A0=_tmp59F;
struct Cyc_CfFlowInfo_Place*_tmp5A1=(struct Cyc_CfFlowInfo_Place*)_tmp59C->hd;void*_tmp5A3;struct Cyc_List_List*_tmp5A4;struct Cyc_CfFlowInfo_Place*_tmp5A2=_tmp5A1;_tmp5A3=_tmp5A2->root;_tmp5A4=_tmp5A2->fields;
if(_tmp5A4 != 0){
const char*_tmp969;void*_tmp968;(_tmp968=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp969="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp969,sizeof(char),53))),_tag_dyneither(_tmp968,sizeof(void*),0)));}
{void*_tmp5A7=_tmp5A3;struct Cyc_Absyn_Vardecl*_tmp5A9;_LL2FD: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5A8=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp5A7;if(_tmp5A8->tag != 0)goto _LL2FF;else{_tmp5A9=_tmp5A8->f1;}}_LL2FE: {
# 2179
int varok=0;
{void*_tmp5AA=_tmp59F;struct Cyc_Absyn_Vardecl*_tmp5AC;void*_tmp5AD;_LL302: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5AB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5AA;if(_tmp5AB->tag != 8)goto _LL304;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=(void*)_tmp5AB->f2;}}_LL303:
# 2182
 if(_tmp5A9 == _tmp5AC){
_tmp5A0=_tmp5AD;
# 2185
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5A9->type)){
# 2187
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp599,_tmp59B,_tmp5A0,& _tmp59A)){
if(!_tmp598){
const char*_tmp96D;void*_tmp96C[1];struct Cyc_String_pa_PrintArg_struct _tmp96B;(_tmp96B.tag=0,((_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2191
Cyc_Absynpp_qvar2string(_tmp5A9->name)),((_tmp96C[0]=& _tmp96B,Cyc_CfFlowInfo_aerr(loc,((_tmp96D="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp96D,sizeof(char),58))),_tag_dyneither(_tmp96C,sizeof(void*),1)))))));}}else{
# 2194
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp59E,_tmp5A0)!= Cyc_CfFlowInfo_AllIL  && !_tmp598){
const char*_tmp971;void*_tmp970[1];struct Cyc_String_pa_PrintArg_struct _tmp96F;(_tmp96F.tag=0,((_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2197
Cyc_Absynpp_qvar2string(_tmp5A9->name)),((_tmp970[0]=& _tmp96F,Cyc_CfFlowInfo_aerr(loc,((_tmp971="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp971,sizeof(char),84))),_tag_dyneither(_tmp970,sizeof(void*),1)))))));}else{
# 2199
varok=1;}}}else{
# 2202
varok=1;}}else{
# 2205
goto _LL305;}
goto _LL301;_LL304:;_LL305:
# 2209
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5A9->type))
varok=1;else{
if(!_tmp598){
const char*_tmp975;void*_tmp974[1];struct Cyc_String_pa_PrintArg_struct _tmp973;(_tmp973.tag=0,((_tmp973.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2214
Cyc_Absynpp_qvar2string(_tmp5A9->name)),((_tmp974[0]=& _tmp973,Cyc_CfFlowInfo_aerr(loc,((_tmp975="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp975,sizeof(char),85))),_tag_dyneither(_tmp974,sizeof(void*),1)))))));}}
goto _LL301;_LL301:;}
# 2219
if(_tmp598){
# 2225
struct _tuple18 _tmp5B7=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp5B9;union Cyc_CfFlowInfo_AbsLVal _tmp5BA;struct _tuple18 _tmp5B8=_tmp5B7;_tmp5B9=_tmp5B8.f1;_tmp5BA=_tmp5B8.f2;{
# 2228
union Cyc_CfFlowInfo_AbsLVal _tmp5BB=_tmp5BA;struct Cyc_CfFlowInfo_Place*_tmp5BC;_LL307: if((_tmp5BB.PlaceL).tag != 1)goto _LL309;_tmp5BC=(struct Cyc_CfFlowInfo_Place*)(_tmp5BB.PlaceL).val;_LL308: {
# 2232
void*_tmp5BD=Cyc_CfFlowInfo_lookup_place(_tmp59E,_tmp5BC);
{void*_tmp5BE=_tmp5BD;struct Cyc_Absyn_Vardecl*_tmp5C0;void*_tmp5C1;_LL30C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5BF=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5BE;if(_tmp5BF->tag != 8)goto _LL30E;else{_tmp5C0=_tmp5BF->f1;_tmp5C1=(void*)_tmp5BF->f2;}}_LL30D: {
# 2238
void*new_rval;
if(_tmp5C0 == _tmp5A9){
# 2242
if(varok){
# 2244
_tmp5BD=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp5C1);{
# 2253
struct _tuple16 _tmp977;struct _tuple16 _tmp976;struct _tuple16 _tmp5C2=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2256
(_tmp976.f1=_tmp5B9,((_tmp976.f2=_tmp5BD,_tmp976)))),(
(_tmp977.f1=_tmp5B9,((_tmp977.f2=_tmp5A0,_tmp977)))));
# 2253
union Cyc_CfFlowInfo_FlowInfo _tmp5C4;void*_tmp5C5;struct _tuple16 _tmp5C3=_tmp5C2;_tmp5C4=_tmp5C3.f1;_tmp5C5=_tmp5C3.f2;
# 2258
_tmp5B9=_tmp5C4;new_rval=_tmp5C5;};}else{
# 2260
new_rval=_tmp5C1;}
# 2262
{union Cyc_CfFlowInfo_FlowInfo _tmp5C8=_tmp5B9;struct Cyc_Dict_Dict _tmp5C9;struct Cyc_List_List*_tmp5CA;_LL311: if((_tmp5C8.ReachableFL).tag != 2)goto _LL313;_tmp5C9=((struct _tuple14)(_tmp5C8.ReachableFL).val).f1;_tmp5CA=((struct _tuple14)(_tmp5C8.ReachableFL).val).f2;_LL312:
# 2264
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp5C9,env->all_changed,_tmp5BC,new_rval),_tmp5CA);
# 2268
goto _LL310;_LL313:;_LL314: {
# 2270
const char*_tmp97A;void*_tmp979;(_tmp979=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp97A="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp97A,sizeof(char),42))),_tag_dyneither(_tmp979,sizeof(void*),0)));}_LL310:;}
# 2272
goto _LL30B;}else{
# 2274
goto _LL30F;}
goto _LL30B;}_LL30E:;_LL30F:
# 2281
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5A9->type)){
const char*_tmp97E;void*_tmp97D[1];struct Cyc_String_pa_PrintArg_struct _tmp97C;(_tmp97C.tag=0,((_tmp97C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)unconsume_exps->hd)),((_tmp97D[0]=& _tmp97C,Cyc_CfFlowInfo_aerr(loc,((_tmp97E="aliased expression %s was overwritten",_tag_dyneither(_tmp97E,sizeof(char),38))),_tag_dyneither(_tmp97D,sizeof(void*),1)))))));}
# 2297 "new_control_flow.cyc"
goto _LL30B;_LL30B:;}
# 2299
goto _LL306;}_LL309:;_LL30A:
# 2305
 goto _LL306;_LL306:;};}
# 2308
goto _LL2FC;}_LL2FF:;_LL300: {
# 2311
const char*_tmp981;void*_tmp980;(_tmp980=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp981="unconsume_params: root is not a varroot",_tag_dyneither(_tmp981,sizeof(char),40))),_tag_dyneither(_tmp980,sizeof(void*),0)));}_LL2FC:;}
# 2313
goto _LL2F7;}_LL2FA: if((_tmp59D.BottomFL).tag != 1)goto _LL2F7;_LL2FB: {
# 2315
const char*_tmp984;void*_tmp983;(_tmp983=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp984="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp984,sizeof(char),38))),_tag_dyneither(_tmp983,sizeof(void*),0)));}_LL2F7:;}}
# 2318
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple28{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2322
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2325
struct Cyc_CfFlowInfo_FlowEnv*_tmp5D4=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5D5=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Stmt*_tmp5D9;unsigned int _tmp5DA;struct Cyc_Absyn_Switch_clause*_tmp5D6=_tmp5D5;_tmp5D7=_tmp5D6->pat_vars;_tmp5D8=_tmp5D6->where_clause;_tmp5D9=_tmp5D6->body;_tmp5DA=_tmp5D6->loc;{
struct _tuple1 _tmp5DB=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D7))->v);struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DE;struct _tuple1 _tmp5DC=_tmp5DB;_tmp5DD=_tmp5DC.f1;_tmp5DE=_tmp5DC.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp5D7->v,_tmp5DD != 0,_tmp5DA,exp_loc);
# 2333
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp5DF=env->unique_pat_vars;
{struct _tuple28*_tmp987;struct Cyc_List_List*_tmp986;env->unique_pat_vars=(
(_tmp986=_region_malloc(env->r,sizeof(*_tmp986)),((_tmp986->hd=((_tmp987=_region_malloc(env->r,sizeof(*_tmp987)),((_tmp987->f1=_tmp5D9,((_tmp987->f2=_tmp5DD,((_tmp987->f3=_tmp5DE,_tmp987)))))))),((_tmp986->tl=_tmp5DF,_tmp986))))));}
# 2338
if(_tmp5D8 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5D8;
struct _tuple19 _tmp5E2=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp5E4;union Cyc_CfFlowInfo_FlowInfo _tmp5E5;struct _tuple19 _tmp5E3=_tmp5E2;_tmp5E4=_tmp5E3.f1;_tmp5E5=_tmp5E3.f2;
inflow=_tmp5E5;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5E4,_tmp5D9,0);}else{
# 2344
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5D9,0);}
# 2346
env->unique_pat_vars=_tmp5DF;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E6=clause_outflow;_LL316: if((_tmp5E6.BottomFL).tag != 1)goto _LL318;_LL317:
 goto _LL315;_LL318:;_LL319:
# 2351
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5DD,_tmp5DE,clause_outflow,_tmp5DA);
# 2353
if(scs->tl == 0)
return clause_outflow;else{
# 2358
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp98A;void*_tmp989;(_tmp989=0,Cyc_CfFlowInfo_aerr(_tmp5D9->loc,((_tmp98A="switch clause may implicitly fallthru",_tag_dyneither(_tmp98A,sizeof(char),38))),_tag_dyneither(_tmp989,sizeof(void*),0)));}else{
# 2361
const char*_tmp98D;void*_tmp98C;(_tmp98C=0,Cyc_Tcutil_warn(_tmp5D9->loc,((_tmp98D="switch clause may implicitly fallthru",_tag_dyneither(_tmp98D,sizeof(char),38))),_tag_dyneither(_tmp98C,sizeof(void*),0)));}
# 2363
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2365
goto _LL315;_LL315:;};};};}
# 2368
return Cyc_CfFlowInfo_BottomFL();}struct _tuple29{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2373
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple29*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2375
struct Cyc_NewControlFlow_AnalEnv*_tmp5EC;struct Cyc_Dict_Dict _tmp5ED;unsigned int _tmp5EE;struct _tuple29*_tmp5EB=vdenv;_tmp5EC=_tmp5EB->f1;_tmp5ED=_tmp5EB->f2;_tmp5EE=_tmp5EB->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5EC->fenv)->r,vd->type)){
# 2378
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp98E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp98E.tag=0,((_tmp98E.f1=vd,_tmp98E)));
# 2380
struct _tuple15 _tmp5EF=Cyc_CfFlowInfo_unname_rval((_tmp5EC->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5ED,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2380
void*_tmp5F1;struct _tuple15 _tmp5F0=_tmp5EF;_tmp5F1=_tmp5F0.f1;{
# 2382
void*_tmp5F2=_tmp5F1;_LL31B: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5F3=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F2;if(_tmp5F3->tag != 7)goto _LL31D;}_LL31C:
 goto _LL31E;_LL31D: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5F4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5F2;if(_tmp5F4->tag != 0)goto _LL31F;}_LL31E:
 goto _LL320;_LL31F: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5F5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5F2;if(_tmp5F5->tag != 3)goto _LL321;else{if(_tmp5F5->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL321;}}_LL320:
 goto _LL31A;_LL321:;_LL322:
# 2387
{const char*_tmp992;void*_tmp991[1];struct Cyc_String_pa_PrintArg_struct _tmp990;(_tmp990.tag=0,((_tmp990.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp991[0]=& _tmp990,Cyc_Tcutil_warn(_tmp5EE,((_tmp992="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp992,sizeof(char),68))),_tag_dyneither(_tmp991,sizeof(void*),1)))))));}
# 2389
goto _LL31A;_LL31A:;};}}
# 2394
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2396
{union Cyc_CfFlowInfo_FlowInfo _tmp5FA=inflow;struct Cyc_Dict_Dict _tmp5FB;_LL324: if((_tmp5FA.ReachableFL).tag != 2)goto _LL326;_tmp5FB=((struct _tuple14)(_tmp5FA.ReachableFL).val).f1;_LL325: {
# 2398
struct _tuple29 _tmp993;struct _tuple29 _tmp5FC=(_tmp993.f1=env,((_tmp993.f2=_tmp5FB,((_tmp993.f3=decl->loc,_tmp993)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp5FD=env->fenv;
{void*_tmp5FE=decl->r;void*_tmp5FF=_tmp5FE;struct Cyc_Absyn_Vardecl*_tmp601;struct Cyc_List_List*_tmp603;struct Cyc_List_List*_tmp605;_LL329: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp600=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp5FF;if(_tmp600->tag != 0)goto _LL32B;else{_tmp601=_tmp600->f1;}}_LL32A:
# 2402
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp5FC,_tmp601);
goto _LL328;_LL32B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp602=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp5FF;if(_tmp602->tag != 2)goto _LL32D;else{if(_tmp602->f2 == 0)goto _LL32D;_tmp603=(struct Cyc_List_List*)(_tmp602->f2)->v;}}_LL32C: {
# 2405
struct _tuple1 _tmp606=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp603);struct Cyc_List_List*_tmp608;struct _tuple1 _tmp607=_tmp606;_tmp608=_tmp607.f1;{
struct Cyc_List_List*_tmp609=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp608);
_tmp605=_tmp609;goto _LL32E;};}_LL32D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp604=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp5FF;if(_tmp604->tag != 3)goto _LL32F;else{_tmp605=_tmp604->f1;}}_LL32E:
# 2409
((void(*)(void(*f)(struct _tuple29*,struct Cyc_Absyn_Vardecl*),struct _tuple29*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp5FC,_tmp605);
goto _LL328;_LL32F:;_LL330:
 goto _LL328;_LL328:;}
# 2413
goto _LL323;}_LL326:;_LL327:
 goto _LL323;_LL323:;}
# 2416
return;}
# 2422
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2425
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple28*_tmp60B=(struct _tuple28*)x->hd;struct Cyc_Absyn_Stmt*_tmp60D;struct Cyc_List_List*_tmp60E;struct Cyc_List_List*_tmp60F;struct _tuple28*_tmp60C=_tmp60B;_tmp60D=_tmp60C->f1;_tmp60E=_tmp60C->f2;_tmp60F=_tmp60C->f3;
keep_going=0;{
# 2433
struct Cyc_Absyn_Stmt*_tmp610=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp610 != _tmp60D){
struct Cyc_Absyn_Stmt*_tmp611=(Cyc_NewControlFlow_get_stmt_annot(_tmp610))->encloser;
if(_tmp611 == _tmp610){
# 2439
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp60E,_tmp60F,inflow,dest->loc);
keep_going=1;
break;}
# 2443
_tmp610=_tmp611;}};}}
# 2446
return inflow;}
# 2452
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2455
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp612=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp614;union Cyc_CfFlowInfo_FlowInfo*_tmp615;struct _tuple20 _tmp613=_tmp612;_tmp614=_tmp613.f1;_tmp615=_tmp613.f2;
inflow=*_tmp615;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp616=env->fenv;
# 2462
void*_tmp617=s->r;void*_tmp618=_tmp617;struct Cyc_Absyn_Exp*_tmp61D;struct Cyc_Absyn_Exp*_tmp61F;struct Cyc_Absyn_Stmt*_tmp621;struct Cyc_Absyn_Stmt*_tmp622;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Stmt*_tmp625;struct Cyc_Absyn_Stmt*_tmp626;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Stmt*_tmp629;struct Cyc_Absyn_Stmt*_tmp62A;struct Cyc_Absyn_Stmt*_tmp62C;struct Cyc_Absyn_Exp*_tmp62D;struct Cyc_Absyn_Stmt*_tmp62E;struct Cyc_Absyn_Exp*_tmp630;struct Cyc_Absyn_Exp*_tmp631;struct Cyc_Absyn_Stmt*_tmp632;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_List_List*_tmp637;struct Cyc_Absyn_Switch_clause*_tmp638;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_List_List*_tmp641;struct Cyc_Absyn_Stmt*_tmp643;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp647;struct Cyc_Absyn_Exp*_tmp648;unsigned int _tmp649;struct Cyc_Absyn_Stmt*_tmp64A;struct Cyc_Absyn_Decl*_tmp64C;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_Absyn_Stmt*_tmp64F;struct Cyc_Absyn_Exp*_tmp651;_LL332: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp619=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp619->tag != 0)goto _LL334;}_LL333:
 return inflow;_LL334: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp61A=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp61A->tag != 6)goto _LL336;else{if(_tmp61A->f1 != 0)goto _LL336;}}_LL335:
# 2468
{union Cyc_CfFlowInfo_FlowInfo _tmp652=inflow;_LL35D: if((_tmp652.ReachableFL).tag != 2)goto _LL35F;_LL35E:
# 2470
{void*_tmp653=Cyc_Tcutil_compress(env->return_type);void*_tmp654=_tmp653;_LL362: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp655=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp654;if(_tmp655->tag != 0)goto _LL364;}_LL363:
 goto _LL361;_LL364: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp656=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp654;if(_tmp656->tag != 7)goto _LL366;}_LL365:
 goto _LL367;_LL366: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp654;if(_tmp657->tag != 6)goto _LL368;}_LL367:
# 2474
{const char*_tmp996;void*_tmp995;(_tmp995=0,Cyc_Tcutil_warn(s->loc,((_tmp996="break may cause function not to return a value",_tag_dyneither(_tmp996,sizeof(char),47))),_tag_dyneither(_tmp995,sizeof(void*),0)));}
goto _LL361;_LL368:;_LL369:
# 2477
{const char*_tmp999;void*_tmp998;(_tmp998=0,Cyc_Tcutil_terr(s->loc,((_tmp999="break may cause function not to return a value",_tag_dyneither(_tmp999,sizeof(char),47))),_tag_dyneither(_tmp998,sizeof(void*),0)));}
goto _LL361;_LL361:;}
# 2480
goto _LL35C;_LL35F:;_LL360:
 goto _LL35C;_LL35C:;}
# 2483
goto _LL337;_LL336: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp61B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp61B->tag != 3)goto _LL338;else{if(_tmp61B->f1 != 0)goto _LL338;}}_LL337:
# 2485
 if(env->noreturn){
const char*_tmp99C;void*_tmp99B;(_tmp99B=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp99C="`noreturn' function might return",_tag_dyneither(_tmp99C,sizeof(char),33))),_tag_dyneither(_tmp99B,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL338: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp61C=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp61C->tag != 3)goto _LL33A;else{_tmp61D=_tmp61C->f1;}}_LL339:
# 2491
 if(env->noreturn){
const char*_tmp99F;void*_tmp99E;(_tmp99E=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp99F="`noreturn' function might return",_tag_dyneither(_tmp99F,sizeof(char),33))),_tag_dyneither(_tmp99E,sizeof(void*),0)));}{
struct _tuple16 _tmp660=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp61D));union Cyc_CfFlowInfo_FlowInfo _tmp662;void*_tmp663;struct _tuple16 _tmp661=_tmp660;_tmp662=_tmp661.f1;_tmp663=_tmp661.f2;
_tmp662=Cyc_NewControlFlow_use_Rval(env,_tmp61D->loc,_tmp662,_tmp663);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp662);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp662,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL33A: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp61E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp61E->tag != 1)goto _LL33C;else{_tmp61F=_tmp61E->f1;}}_LL33B: {
# 2500
struct _tuple17*_tmp664=rval_opt;void**_tmp665;int _tmp666;_LL36B: if(_tmp664 == 0)goto _LL36D;_tmp665=(void**)& _tmp664->f1;_tmp666=_tmp664->f2;_LL36C: {
# 2502
struct _tuple16 _tmp667=Cyc_NewControlFlow_anal_Rexp(env,_tmp666,inflow,_tmp61F);union Cyc_CfFlowInfo_FlowInfo _tmp669;void*_tmp66A;struct _tuple16 _tmp668=_tmp667;_tmp669=_tmp668.f1;_tmp66A=_tmp668.f2;
*_tmp665=_tmp66A;
return _tmp669;}_LL36D: if(_tmp664 != 0)goto _LL36A;_LL36E:
# 2506
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp61F)).f1;_LL36A:;}_LL33C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp620=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp620->tag != 2)goto _LL33E;else{_tmp621=_tmp620->f1;_tmp622=_tmp620->f2;}}_LL33D:
# 2510
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp621,0),_tmp622,rval_opt);_LL33E: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp623=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp623->tag != 4)goto _LL340;else{_tmp624=_tmp623->f1;_tmp625=_tmp623->f2;_tmp626=_tmp623->f3;}}_LL33F: {
# 2513
struct _tuple19 _tmp66B=Cyc_NewControlFlow_anal_test(env,inflow,_tmp624);union Cyc_CfFlowInfo_FlowInfo _tmp66D;union Cyc_CfFlowInfo_FlowInfo _tmp66E;struct _tuple19 _tmp66C=_tmp66B;_tmp66D=_tmp66C.f1;_tmp66E=_tmp66C.f2;{
# 2520
union Cyc_CfFlowInfo_FlowInfo _tmp66F=Cyc_NewControlFlow_anal_stmt(env,_tmp66E,_tmp626,0);
union Cyc_CfFlowInfo_FlowInfo _tmp670=Cyc_NewControlFlow_anal_stmt(env,_tmp66D,_tmp625,0);
return Cyc_CfFlowInfo_join_flow(_tmp616,env->all_changed,_tmp670,_tmp66F);};}_LL340: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp627=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp627->tag != 5)goto _LL342;else{_tmp628=(_tmp627->f1).f1;_tmp629=(_tmp627->f1).f2;_tmp62A=_tmp627->f2;}}_LL341: {
# 2528
struct _tuple20 _tmp671=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp629);union Cyc_CfFlowInfo_FlowInfo*_tmp673;struct _tuple20 _tmp672=_tmp671;_tmp673=_tmp672.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp674=*_tmp673;
struct _tuple19 _tmp675=Cyc_NewControlFlow_anal_test(env,_tmp674,_tmp628);union Cyc_CfFlowInfo_FlowInfo _tmp677;union Cyc_CfFlowInfo_FlowInfo _tmp678;struct _tuple19 _tmp676=_tmp675;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp679=Cyc_NewControlFlow_anal_stmt(env,_tmp677,_tmp62A,0);
Cyc_NewControlFlow_update_flow(env,_tmp629,_tmp679);
return _tmp678;};};}_LL342: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp62B=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp62B->tag != 14)goto _LL344;else{_tmp62C=_tmp62B->f1;_tmp62D=(_tmp62B->f2).f1;_tmp62E=(_tmp62B->f2).f2;}}_LL343: {
# 2538
union Cyc_CfFlowInfo_FlowInfo _tmp67A=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp62C,0);
struct _tuple20 _tmp67B=Cyc_NewControlFlow_pre_stmt_check(env,_tmp67A,_tmp62E);union Cyc_CfFlowInfo_FlowInfo*_tmp67D;struct _tuple20 _tmp67C=_tmp67B;_tmp67D=_tmp67C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp67E=*_tmp67D;
struct _tuple19 _tmp67F=Cyc_NewControlFlow_anal_test(env,_tmp67E,_tmp62D);union Cyc_CfFlowInfo_FlowInfo _tmp681;union Cyc_CfFlowInfo_FlowInfo _tmp682;struct _tuple19 _tmp680=_tmp67F;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;
Cyc_NewControlFlow_update_flow(env,_tmp62C,_tmp681);
return _tmp682;};}_LL344: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp62F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp62F->tag != 9)goto _LL346;else{_tmp630=_tmp62F->f1;_tmp631=(_tmp62F->f2).f1;_tmp632=(_tmp62F->f2).f2;_tmp633=(_tmp62F->f3).f1;_tmp634=(_tmp62F->f3).f2;_tmp635=_tmp62F->f4;}}_LL345: {
# 2547
union Cyc_CfFlowInfo_FlowInfo _tmp683=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp630)).f1;
struct _tuple20 _tmp684=Cyc_NewControlFlow_pre_stmt_check(env,_tmp683,_tmp632);union Cyc_CfFlowInfo_FlowInfo*_tmp686;struct _tuple20 _tmp685=_tmp684;_tmp686=_tmp685.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp687=*_tmp686;
struct _tuple19 _tmp688=Cyc_NewControlFlow_anal_test(env,_tmp687,_tmp631);union Cyc_CfFlowInfo_FlowInfo _tmp68A;union Cyc_CfFlowInfo_FlowInfo _tmp68B;struct _tuple19 _tmp689=_tmp688;_tmp68A=_tmp689.f1;_tmp68B=_tmp689.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp68C=Cyc_NewControlFlow_anal_stmt(env,_tmp68A,_tmp635,0);
struct _tuple20 _tmp68D=Cyc_NewControlFlow_pre_stmt_check(env,_tmp68C,_tmp634);union Cyc_CfFlowInfo_FlowInfo*_tmp68F;struct _tuple20 _tmp68E=_tmp68D;_tmp68F=_tmp68E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp690=*_tmp68F;
union Cyc_CfFlowInfo_FlowInfo _tmp691=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp690,_tmp633)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp632,_tmp691);
return _tmp68B;};};};}_LL346: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp636=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp636->tag != 11)goto _LL348;else{_tmp637=_tmp636->f1;if(_tmp636->f2 == 0)goto _LL348;_tmp638=*_tmp636->f2;}}_LL347: {
# 2559
struct _RegionHandle _tmp692=_new_region("temp");struct _RegionHandle*temp=& _tmp692;_push_region(temp);
{struct _tuple23 _tmp693=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp637,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp695;struct Cyc_List_List*_tmp696;struct _tuple23 _tmp694=_tmp693;_tmp695=_tmp694.f1;_tmp696=_tmp694.f2;
# 2562
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp638->body);{
# 2564
struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp638->pat_vars))->v)).f1);
_tmp695=Cyc_NewControlFlow_add_vars(_tmp616,_tmp695,_tmp697,_tmp616->unknown_all,s->loc,0);
# 2567
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp638->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp698=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp69A;struct Cyc_Absyn_Exp*_tmp69B;struct _tuple24*_tmp699=_tmp698;_tmp69A=_tmp699->f1;_tmp69B=_tmp699->f2;
if(_tmp69A != 0){
_tmp695=Cyc_NewControlFlow_do_initialize_var(_tmp616,env,_tmp695,*_tmp69A,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp637))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp696))->hd,s->loc);
_tmp696=_tmp696->tl;
_tmp637=_tmp637->tl;}}}
# 2575
Cyc_NewControlFlow_update_flow(env,_tmp638->body,_tmp695);{
union Cyc_CfFlowInfo_FlowInfo _tmp69C=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp69C;};};}
# 2560
;_pop_region(temp);}_LL348: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp639=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp639->tag != 6)goto _LL34A;else{_tmp63A=_tmp639->f1;}}_LL349:
# 2578
 _tmp63C=_tmp63A;goto _LL34B;_LL34A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp63B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp63B->tag != 7)goto _LL34C;else{_tmp63C=_tmp63B->f1;}}_LL34B:
 _tmp63E=_tmp63C;goto _LL34D;_LL34C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp63D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp63D->tag != 8)goto _LL34E;else{_tmp63E=_tmp63D->f2;}}_LL34D:
# 2583
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp69D=_tmp614->encloser;
struct Cyc_Absyn_Stmt*_tmp69E=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp63E)))->encloser;
while(_tmp69E != _tmp69D){
struct Cyc_Absyn_Stmt*_tmp69F=(Cyc_NewControlFlow_get_stmt_annot(_tmp69D))->encloser;
if(_tmp69F == _tmp69D){
{const char*_tmp9A2;void*_tmp9A1;(_tmp9A1=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9A2="goto enters local scope or exception handler",_tag_dyneither(_tmp9A2,sizeof(char),45))),_tag_dyneither(_tmp9A1,sizeof(void*),0)));}
break;}
# 2592
_tmp69D=_tmp69F;}}
# 2596
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp63E));
# 2598
Cyc_NewControlFlow_update_flow(env,_tmp63E,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL34E: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp63F=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp63F->tag != 10)goto _LL350;else{_tmp640=_tmp63F->f1;_tmp641=_tmp63F->f2;}}_LL34F:
# 2604
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp641,_tmp640->loc);_LL350: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp642=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp642->tag != 15)goto _LL352;else{_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;}}_LL351: {
# 2609
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp643,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2617
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2620
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp644,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp6A2=scs_outflow;_LL370: if((_tmp6A2.BottomFL).tag != 1)goto _LL372;_LL371:
 goto _LL36F;_LL372:;_LL373: {
const char*_tmp9A5;void*_tmp9A4;(_tmp9A4=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9A5="last catch clause may implicitly fallthru",_tag_dyneither(_tmp9A5,sizeof(char),42))),_tag_dyneither(_tmp9A4,sizeof(void*),0)));}_LL36F:;}
# 2626
outflow=s1_outflow;
# 2628
return outflow;};};}_LL352: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp645=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp645->tag != 12)goto _LL354;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp646=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp645->f1)->r;if(_tmp646->tag != 2)goto _LL354;else{if(_tmp646->f2 == 0)goto _LL354;_tmp647=(struct Cyc_List_List*)(_tmp646->f2)->v;_tmp648=_tmp646->f3;}}_tmp649=(_tmp645->f1)->loc;_tmp64A=_tmp645->f2;}}_LL353: {
# 2638
struct _tuple1 _tmp6A5=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp647);struct Cyc_List_List*_tmp6A7;struct Cyc_List_List*_tmp6A8;struct _tuple1 _tmp6A6=_tmp6A5;_tmp6A7=_tmp6A6.f1;_tmp6A8=_tmp6A6.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp616,env,inflow,_tmp647,_tmp6A7 != 0,_tmp649,_tmp648->loc);{
struct Cyc_List_List*_tmp6A9=env->unique_pat_vars;
{struct _tuple28*_tmp9A8;struct Cyc_List_List*_tmp9A7;env->unique_pat_vars=(
(_tmp9A7=_region_malloc(env->r,sizeof(*_tmp9A7)),((_tmp9A7->hd=((_tmp9A8=_region_malloc(env->r,sizeof(*_tmp9A8)),((_tmp9A8->f1=_tmp64A,((_tmp9A8->f2=_tmp6A7,((_tmp9A8->f3=_tmp6A8,_tmp9A8)))))))),((_tmp9A7->tl=_tmp6A9,_tmp9A7))))));}
# 2646
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp64A,rval_opt);
env->unique_pat_vars=_tmp6A9;
# 2650
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp6A7,_tmp6A8,inflow,_tmp649);
# 2654
return inflow;};}_LL354: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp64B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp64B->tag != 12)goto _LL356;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;}}_LL355:
# 2738 "new_control_flow.cyc"
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp64C),_tmp64D,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp64C);
return outflow;_LL356: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp64E=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp64E->tag != 13)goto _LL358;else{_tmp64F=_tmp64E->f2;}}_LL357:
# 2743
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp64F,rval_opt);_LL358: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp650=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp618;if(_tmp650->tag != 16)goto _LL35A;else{_tmp651=_tmp650->f1;}}_LL359: {
# 2746
struct _tuple16 _tmp6AC=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp651);union Cyc_CfFlowInfo_FlowInfo _tmp6AE;void*_tmp6AF;struct _tuple16 _tmp6AD=_tmp6AC;_tmp6AE=_tmp6AD.f1;_tmp6AF=_tmp6AD.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6B0=Cyc_NewControlFlow_use_Rval(env,_tmp651->loc,_tmp6AE,_tmp6AF);
void*_tmp6B1=Cyc_Tcutil_compress((void*)_check_null(_tmp651->topt));void*_tmp6B2=_tmp6B1;void*_tmp6B4;_LL375: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6B3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6B2;if(_tmp6B3->tag != 15)goto _LL377;else{_tmp6B4=(void*)_tmp6B3->f1;}}_LL376:
# 2750
 return Cyc_CfFlowInfo_kill_flow_region(_tmp616,_tmp6AE,_tmp6B4);_LL377:;_LL378: {
struct Cyc_Core_Impossible_exn_struct _tmp9AE;const char*_tmp9AD;struct Cyc_Core_Impossible_exn_struct*_tmp9AC;(int)_throw((void*)((_tmp9AC=_cycalloc(sizeof(*_tmp9AC)),((_tmp9AC[0]=((_tmp9AE.tag=Cyc_Core_Impossible,((_tmp9AE.f1=((_tmp9AD="anal_stmt -- reset_region",_tag_dyneither(_tmp9AD,sizeof(char),26))),_tmp9AE)))),_tmp9AC)))));}_LL374:;};}_LL35A:;_LL35B: {
# 2754
struct Cyc_Core_Impossible_exn_struct _tmp9B4;const char*_tmp9B3;struct Cyc_Core_Impossible_exn_struct*_tmp9B2;(int)_throw((void*)((_tmp9B2=_cycalloc(sizeof(*_tmp9B2)),((_tmp9B2[0]=((_tmp9B4.tag=Cyc_Core_Impossible,((_tmp9B4.f1=((_tmp9B3="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp9B3,sizeof(char),56))),_tmp9B4)))),_tmp9B2)))));}_LL331:;};}
# 2759
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2762
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp6BB=env->fenv;
void*_tmp6BC=decl->r;void*_tmp6BD=_tmp6BC;struct Cyc_Absyn_Vardecl*_tmp6BF;struct Cyc_List_List*_tmp6C1;struct Cyc_Absyn_Fndecl*_tmp6C3;struct Cyc_Absyn_Tvar*_tmp6C5;struct Cyc_Absyn_Vardecl*_tmp6C6;int _tmp6C7;struct Cyc_Absyn_Exp*_tmp6C8;_LL37A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BE=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6BD;if(_tmp6BE->tag != 0)goto _LL37C;else{_tmp6BF=_tmp6BE->f1;}}_LL37B: {
# 2772
struct Cyc_List_List _tmp9B5;struct Cyc_List_List _tmp6C9=(_tmp9B5.hd=_tmp6BF,((_tmp9B5.tl=0,_tmp9B5)));
inflow=Cyc_NewControlFlow_add_vars(_tmp6BB,inflow,& _tmp6C9,_tmp6BB->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp6CA=_tmp6BF->initializer;
if(_tmp6CA == 0)
return inflow;{
struct _tuple16 _tmp6CB=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6CA);union Cyc_CfFlowInfo_FlowInfo _tmp6CD;void*_tmp6CE;struct _tuple16 _tmp6CC=_tmp6CB;_tmp6CD=_tmp6CC.f1;_tmp6CE=_tmp6CC.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp6BB,env,_tmp6CD,_tmp6BF,_tmp6CA,_tmp6CE,decl->loc);};};}_LL37C: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp6C0=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6BD;if(_tmp6C0->tag != 3)goto _LL37E;else{_tmp6C1=_tmp6C0->f1;}}_LL37D:
# 2781
 return Cyc_NewControlFlow_add_vars(_tmp6BB,inflow,_tmp6C1,_tmp6BB->unknown_none,decl->loc,0);_LL37E: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6C2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6BD;if(_tmp6C2->tag != 1)goto _LL380;else{_tmp6C3=_tmp6C2->f1;}}_LL37F:
# 2784
 Cyc_NewControlFlow_check_nested_fun(_tmp6BB,inflow,_tmp6C3);{
void*t=(void*)_check_null(_tmp6C3->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp6D0=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6C3->fn_vardecl);
# 2790
struct Cyc_List_List*_tmp9B6;return Cyc_NewControlFlow_add_vars(_tmp6BB,inflow,((_tmp9B6=_region_malloc(env->r,sizeof(*_tmp9B6)),((_tmp9B6->hd=_tmp6D0,((_tmp9B6->tl=0,_tmp9B6)))))),_tmp6BB->unknown_all,decl->loc,0);};_LL380: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp6C4=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6BD;if(_tmp6C4->tag != 4)goto _LL382;else{_tmp6C5=_tmp6C4->f1;_tmp6C6=_tmp6C4->f2;_tmp6C7=_tmp6C4->f3;_tmp6C8=_tmp6C4->f4;}}_LL381:
# 2793
 if(_tmp6C8 != 0){
struct Cyc_Absyn_Exp*_tmp6D2=_tmp6C8;
struct _tuple16 _tmp6D3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6D2);union Cyc_CfFlowInfo_FlowInfo _tmp6D5;void*_tmp6D6;struct _tuple16 _tmp6D4=_tmp6D3;_tmp6D5=_tmp6D4.f1;_tmp6D6=_tmp6D4.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp6D2->loc,_tmp6D5,_tmp6D6);}{
# 2798
struct Cyc_List_List _tmp9B7;struct Cyc_List_List _tmp6D7=(_tmp9B7.hd=_tmp6C6,((_tmp9B7.tl=0,_tmp9B7)));
return Cyc_NewControlFlow_add_vars(_tmp6BB,inflow,& _tmp6D7,_tmp6BB->unknown_all,decl->loc,0);};_LL382:;_LL383: {
# 2802
struct Cyc_Core_Impossible_exn_struct _tmp9BD;const char*_tmp9BC;struct Cyc_Core_Impossible_exn_struct*_tmp9BB;(int)_throw((void*)((_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB[0]=((_tmp9BD.tag=Cyc_Core_Impossible,((_tmp9BD.f1=((_tmp9BC="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9BC,sizeof(char),35))),_tmp9BD)))),_tmp9BB)))));}_LL379:;}
# 2810
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp6DC=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6DC;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2812
;_pop_region(frgn);}
# 2817
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2821
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2825
struct _RegionHandle*_tmp6DD=fenv->r;
unsigned int _tmp6DE=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6DE,1);{
# 2831
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6DF=inflow;struct Cyc_Dict_Dict _tmp6E0;struct Cyc_List_List*_tmp6E1;_LL385: if((_tmp6DF.BottomFL).tag != 1)goto _LL387;_LL386: {
const char*_tmp9C0;void*_tmp9BF;(_tmp9BF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C0="check_fun",_tag_dyneither(_tmp9C0,sizeof(char),10))),_tag_dyneither(_tmp9BF,sizeof(void*),0)));}_LL387: if((_tmp6DF.ReachableFL).tag != 2)goto _LL384;_tmp6E0=((struct _tuple14)(_tmp6DF.ReachableFL).val).f1;_tmp6E1=((struct _tuple14)(_tmp6DF.ReachableFL).val).f2;_LL388: {
# 2838
struct Cyc_List_List*atts;
{void*_tmp6E4=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp6E5=_tmp6E4;struct Cyc_List_List*_tmp6E7;_LL38A: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6E6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E6->tag != 9)goto _LL38C;else{_tmp6E7=(_tmp6E6->f1).attributes;}}_LL38B:
 atts=_tmp6E7;goto _LL389;_LL38C:;_LL38D: {
const char*_tmp9C3;void*_tmp9C2;(_tmp9C2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9C3="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp9C3,sizeof(char),50))),_tag_dyneither(_tmp9C2,sizeof(void*),0)));}_LL389:;}
# 2845
for(0;atts != 0;atts=atts->tl){
void*_tmp6EA=(void*)atts->hd;void*_tmp6EB=_tmp6EA;int _tmp6ED;int _tmp6EF;int _tmp6F1;_LL38F: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp6EC=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6EB;if(_tmp6EC->tag != 21)goto _LL391;else{_tmp6ED=_tmp6EC->f1;}}_LL390: {
# 2848
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp6F2=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6ED - 1);
void*t=Cyc_Tcutil_compress(_tmp6F2->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6F3=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9C6;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9C5;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6F4=(_tmp9C5=_region_malloc(_tmp6DD,sizeof(*_tmp9C5)),((_tmp9C5[0]=((_tmp9C6.tag=2,((_tmp9C6.f1=_tmp6ED,((_tmp9C6.f2=t,_tmp9C6)))))),_tmp9C5)));
struct Cyc_CfFlowInfo_Place*_tmp9C7;struct Cyc_CfFlowInfo_Place*_tmp6F5=(_tmp9C7=_region_malloc(_tmp6DD,sizeof(*_tmp9C7)),((_tmp9C7->root=(void*)_tmp6F4,((_tmp9C7->fields=0,_tmp9C7)))));
_tmp6E0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6E0,(void*)_tmp6F4,_tmp6F3);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9CD;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9CC;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9CA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9C9;_tmp6E0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6E0,(void*)((_tmp9C9=_region_malloc(_tmp6DD,sizeof(*_tmp9C9)),((_tmp9C9[0]=((_tmp9CA.tag=0,((_tmp9CA.f1=_tmp6F2,_tmp9CA)))),_tmp9C9)))),(void*)((_tmp9CC=_region_malloc(_tmp6DD,sizeof(*_tmp9CC)),((_tmp9CC[0]=((_tmp9CD.tag=5,((_tmp9CD.f1=_tmp6F5,_tmp9CD)))),_tmp9CC)))));}
goto _LL38E;}_LL391: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6EE=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6EB;if(_tmp6EE->tag != 20)goto _LL393;else{_tmp6EF=_tmp6EE->f1;}}_LL392: {
# 2861
struct Cyc_Absyn_Vardecl*_tmp6FD=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6EF - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp6FD->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9D0;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9CF;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6FE=(_tmp9CF=_region_malloc(_tmp6DD,sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D0.tag=2,((_tmp9D0.f1=_tmp6EF,((_tmp9D0.f2=elttype,_tmp9D0)))))),_tmp9CF)));
struct Cyc_CfFlowInfo_Place*_tmp9D1;struct Cyc_CfFlowInfo_Place*_tmp6FF=(_tmp9D1=_region_malloc(_tmp6DD,sizeof(*_tmp9D1)),((_tmp9D1->root=(void*)_tmp6FE,((_tmp9D1->fields=0,_tmp9D1)))));
_tmp6E0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6E0,(void*)_tmp6FE,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9D7;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9D6;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9D4;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9D3;_tmp6E0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6E0,(void*)((_tmp9D3=_region_malloc(_tmp6DD,sizeof(*_tmp9D3)),((_tmp9D3[0]=((_tmp9D4.tag=0,((_tmp9D4.f1=_tmp6FD,_tmp9D4)))),_tmp9D3)))),(void*)((_tmp9D6=_region_malloc(_tmp6DD,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=5,((_tmp9D7.f1=_tmp6FF,_tmp9D7)))),_tmp9D6)))));}
{struct Cyc_List_List*_tmp9D8;param_roots=((_tmp9D8=_region_malloc(_tmp6DD,sizeof(*_tmp9D8)),((_tmp9D8->hd=_tmp6FF,((_tmp9D8->tl=param_roots,_tmp9D8))))));}
goto _LL38E;}_LL393: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6F0=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6EB;if(_tmp6F0->tag != 22)goto _LL395;else{_tmp6F1=_tmp6F0->f1;}}_LL394: {
# 2870
struct Cyc_Absyn_Vardecl*_tmp708=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6F1 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp708->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9DB;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9DA;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp709=(_tmp9DA=_region_malloc(_tmp6DD,sizeof(*_tmp9DA)),((_tmp9DA[0]=((_tmp9DB.tag=0,((_tmp9DB.f1=_tmp708,_tmp9DB)))),_tmp9DA)));
struct Cyc_CfFlowInfo_Place*_tmp9DC;struct Cyc_CfFlowInfo_Place*_tmp70A=(_tmp9DC=_region_malloc(_tmp6DD,sizeof(*_tmp9DC)),((_tmp9DC->root=(void*)_tmp709,((_tmp9DC->fields=0,_tmp9DC)))));
struct Cyc_List_List*_tmp9DD;noconsume_roots=((_tmp9DD=_region_malloc(_tmp6DD,sizeof(*_tmp9DD)),((_tmp9DD->hd=_tmp70A,((_tmp9DD->tl=noconsume_roots,_tmp9DD))))));}
# 2876
goto _LL38E;}_LL395:;_LL396:
 goto _LL38E;_LL38E:;}
# 2879
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp6E0,_tmp6E1);}_LL384:;}{
# 2882
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6DD,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp9DE;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp9DE=_region_malloc(_tmp6DD,sizeof(*_tmp9DE)),((_tmp9DE->r=_tmp6DD,((_tmp9DE->fenv=fenv,((_tmp9DE->iterate_again=1,((_tmp9DE->iteration_num=0,((_tmp9DE->in_try=0,((_tmp9DE->tryflow=inflow,((_tmp9DE->all_changed=0,((_tmp9DE->noreturn=noreturn,((_tmp9DE->return_type=fd->ret_type,((_tmp9DE->unique_pat_vars=0,((_tmp9DE->param_roots=param_roots,((_tmp9DE->noconsume_params=noconsume_roots,((_tmp9DE->flow_table=flow_table,_tmp9DE)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2892
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2895
union Cyc_CfFlowInfo_FlowInfo _tmp70F=outflow;_LL398: if((_tmp70F.BottomFL).tag != 1)goto _LL39A;_LL399:
 goto _LL397;_LL39A:;_LL39B:
# 2898
 Cyc_NewControlFlow_check_init_params(_tmp6DE,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp6DE);
# 2902
if(noreturn){
const char*_tmp9E1;void*_tmp9E0;(_tmp9E0=0,Cyc_CfFlowInfo_aerr(_tmp6DE,((_tmp9E1="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp9E1,sizeof(char),46))),_tag_dyneither(_tmp9E0,sizeof(void*),0)));}else{
# 2905
void*_tmp712=Cyc_Tcutil_compress(fd->ret_type);void*_tmp713=_tmp712;_LL39D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp714=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp713;if(_tmp714->tag != 0)goto _LL39F;}_LL39E:
 goto _LL39C;_LL39F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp715=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp713;if(_tmp715->tag != 7)goto _LL3A1;}_LL3A0:
 goto _LL3A2;_LL3A1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp716=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp713;if(_tmp716->tag != 6)goto _LL3A3;}_LL3A2:
# 2909
{const char*_tmp9E4;void*_tmp9E3;(_tmp9E3=0,Cyc_Tcutil_warn(_tmp6DE,((_tmp9E4="function may not return a value",_tag_dyneither(_tmp9E4,sizeof(char),32))),_tag_dyneither(_tmp9E3,sizeof(void*),0)));}goto _LL39C;_LL3A3:;_LL3A4:
# 2911
{const char*_tmp9E7;void*_tmp9E6;(_tmp9E6=0,Cyc_CfFlowInfo_aerr(_tmp6DE,((_tmp9E7="function may not return a value",_tag_dyneither(_tmp9E7,sizeof(char),32))),_tag_dyneither(_tmp9E6,sizeof(void*),0)));}goto _LL39C;_LL39C:;}
# 2913
goto _LL397;_LL397:;};};};}
# 2917
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp71C=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp71D=_tmp71C;struct Cyc_Absyn_Fndecl*_tmp71F;struct Cyc_List_List*_tmp721;struct Cyc_List_List*_tmp723;struct Cyc_List_List*_tmp725;_LL3A6: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp71E=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp71D;if(_tmp71E->tag != 1)goto _LL3A8;else{_tmp71F=_tmp71E->f1;}}_LL3A7:
 Cyc_NewControlFlow_check_fun(_tmp71F);goto _LL3A5;_LL3A8: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp720=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp71D;if(_tmp720->tag != 11)goto _LL3AA;else{_tmp721=_tmp720->f1;}}_LL3A9:
 _tmp723=_tmp721;goto _LL3AB;_LL3AA: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp722=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp71D;if(_tmp722->tag != 10)goto _LL3AC;else{_tmp723=_tmp722->f2;}}_LL3AB:
 _tmp725=_tmp723;goto _LL3AD;_LL3AC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp724=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp71D;if(_tmp724->tag != 9)goto _LL3AE;else{_tmp725=_tmp724->f2;}}_LL3AD:
 Cyc_NewControlFlow_cf_check(_tmp725);goto _LL3A5;_LL3AE: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp726=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp71D;if(_tmp726->tag != 12)goto _LL3B0;}_LL3AF:
 goto _LL3A5;_LL3B0:;_LL3B1:
 goto _LL3A5;_LL3A5:;};}}
