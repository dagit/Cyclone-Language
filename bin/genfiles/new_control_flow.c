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
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp736;struct Cyc_NewControlFlow_CFStmtAnnot _tmp735;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp734;enclosee->annot=(void*)((_tmp734=_cycalloc(sizeof(*_tmp734)),((_tmp734[0]=((_tmp736.tag=Cyc_NewControlFlow_CFAnnot,((_tmp736.f1=((_tmp735.encloser=encloser,((_tmp735.visited=0,_tmp735)))),_tmp736)))),_tmp734))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
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
struct Cyc_Core_Impossible_exn_struct _tmp73C;const char*_tmp73B;struct Cyc_Core_Impossible_exn_struct*_tmp73A;(int)_throw((void*)((_tmp73A=_cycalloc(sizeof(*_tmp73A)),((_tmp73A[0]=((_tmp73C.tag=Cyc_Core_Impossible,((_tmp73C.f1=((_tmp73B="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp73B,sizeof(char),37))),_tmp73C)))),_tmp73A)))));}_LL0:;}
# 137
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp73D;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp73D=_region_malloc(env->r,sizeof(*_tmp73D)),((_tmp73D[0]=Cyc_CfFlowInfo_BottomFL(),_tmp73D)));
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
struct _tuple20 _tmp73E;return(_tmp73E.f1=_tmpB,((_tmp73E.f2=_tmpC,_tmp73E)));};}
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
{const char*_tmp742;void*_tmp741[1];struct Cyc_String_pa_PrintArg_struct _tmp740;(_tmp740.tag=0,((_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp741[0]=& _tmp740,Cyc_Tcutil_warn(_tmp10,((_tmp742="alias-free pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp742,sizeof(char),64))),_tag_dyneither(_tmp741,sizeof(void*),1)))))));}
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
struct _tuple19 _tmp743;struct _tuple19 _tmp21=(_tmp743.f1=flow,((_tmp743.f2=_tmp20,_tmp743)));struct _tuple19 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 221
struct _tuple21 _tmp744;struct _tuple21 _tmp25=(_tmp744.f1=env,((_tmp744.f2=s->loc,((_tmp744.f3=_tmp24,_tmp744)))));
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
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp747;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp746;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp746=_region_malloc(fenv->r,sizeof(*_tmp746)),((_tmp746[0]=((_tmp747.tag=0,((_tmp747.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp747)))),_tmp746)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp74A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp749;_tmp2C=(void*)((_tmp749=_region_malloc(fenv->r,sizeof(*_tmp749)),((_tmp749[0]=((_tmp74A.tag=8,((_tmp74A.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp74A.f2=_tmp2C,_tmp74A)))))),_tmp749))));}
# 259
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 261
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 265
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 272
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp74B;if(Cyc_Relations_consistent_relations(((_tmp74B=_region_malloc(r,sizeof(*_tmp74B)),((_tmp74B->hd=_tmp31,((_tmp74B->tl=assume,_tmp74B))))))))
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
{const char*_tmp750;void*_tmp74F[2];struct Cyc_String_pa_PrintArg_struct _tmp74E;struct Cyc_String_pa_PrintArg_struct _tmp74D;(_tmp74D.tag=0,((_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp74E.tag=0,((_tmp74E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp74F[0]=& _tmp74E,((_tmp74F[1]=& _tmp74D,Cyc_CfFlowInfo_aerr(loc,((_tmp750="unable to prove %s, required to access %s",_tag_dyneither(_tmp750,sizeof(char),42))),_tag_dyneither(_tmp74F,sizeof(void*),2)))))))))))));}
{const char*_tmp753;void*_tmp752;(_tmp752=0,Cyc_fprintf(Cyc_stderr,((_tmp753="  [recorded facts on path: ",_tag_dyneither(_tmp753,sizeof(char),28))),_tag_dyneither(_tmp752,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp756;void*_tmp755;(_tmp755=0,Cyc_fprintf(Cyc_stderr,((_tmp756="]\n",_tag_dyneither(_tmp756,sizeof(char),3))),_tag_dyneither(_tmp755,sizeof(void*),0)));};}}
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
{const char*_tmp75B;void*_tmp75A[2];struct Cyc_String_pa_PrintArg_struct _tmp759;struct Cyc_String_pa_PrintArg_struct _tmp758;(_tmp758.tag=0,((_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp759.tag=0,((_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp75A[0]=& _tmp759,((_tmp75A[1]=& _tmp758,Cyc_CfFlowInfo_aerr(loc,((_tmp75B="unable to prove %s, required to access %s",_tag_dyneither(_tmp75B,sizeof(char),42))),_tag_dyneither(_tmp75A,sizeof(void*),2)))))))))))));}
{const char*_tmp75E;void*_tmp75D;(_tmp75D=0,Cyc_fprintf(Cyc_stderr,((_tmp75E="  [recorded facts on path: ",_tag_dyneither(_tmp75E,sizeof(char),28))),_tag_dyneither(_tmp75D,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp761;void*_tmp760;(_tmp760=0,Cyc_fprintf(Cyc_stderr,((_tmp761="]\n",_tag_dyneither(_tmp761,sizeof(char),3))),_tag_dyneither(_tmp760,sizeof(void*),0)));};}
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
const char*_tmp764;void*_tmp763;(_tmp763=0,Cyc_CfFlowInfo_aerr(loc,((_tmp764="expression may not be fully initialized",_tag_dyneither(_tmp764,sizeof(char),40))),_tag_dyneither(_tmp763,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 342
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp765;struct _tuple0 _tmp5F=(_tmp765.f1=Cyc_Tcutil_compress(t),((_tmp765.f2=r,_tmp765)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
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
{const char*_tmp768;void*_tmp767;(_tmp767=0,Cyc_Tcutil_warn(loc,((_tmp768="argument may still contain alias-free pointers",_tag_dyneither(_tmp768,sizeof(char),47))),_tag_dyneither(_tmp767,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 383
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 387
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp76B;void*_tmp76A;(_tmp76A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp76B="noliveunique attribute requires alias-free pointer",_tag_dyneither(_tmp76B,sizeof(char),51))),_tag_dyneither(_tmp76A,sizeof(void*),0)));}
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
const char*_tmp76E;void*_tmp76D;(_tmp76D=0,Cyc_Tcutil_warn(loc,((_tmp76E="argument may contain live alias-free pointers",_tag_dyneither(_tmp76E,sizeof(char),46))),_tag_dyneither(_tmp76D,sizeof(void*),0)));}
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
struct _tuple23 _tmp76F;return(_tmp76F.f1=inflow,((_tmp76F.f2=0,_tmp76F)));}
if(es->tl == 0){
struct _tuple16 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple16 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp772;struct _tuple23 _tmp771;return(_tmp771.f1=_tmp92,((_tmp771.f2=((_tmp772=_region_malloc(rgn,sizeof(*_tmp772)),((_tmp772->hd=_tmp93,((_tmp772->tl=0,_tmp772)))))),_tmp771)));};}{
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
{struct Cyc_Dict_Dict*_tmp773;env->all_changed=((_tmp773=_region_malloc(env->r,sizeof(*_tmp773)),((_tmp773[0]=(env->fenv)->mt_place_set,_tmp773))));}{
struct _tuple16 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple16 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp774;rvals=((_tmp774=_region_malloc(rgn,sizeof(*_tmp774)),((_tmp774->hd=_tmp9A,((_tmp774->tl=0,_tmp774))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 441
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp775;env->all_changed=((_tmp775=_region_malloc(env->r,sizeof(*_tmp775)),((_tmp775[0]=(env->fenv)->mt_place_set,_tmp775))));}{
struct _tuple16 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple16 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp776;rvals=((_tmp776=_region_malloc(rgn,sizeof(*_tmp776)),((_tmp776->hd=_tmpA0,((_tmp776->tl=rvals,_tmp776))))));}
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
struct Cyc_Dict_Dict*_tmp777;env->all_changed=((_tmp777=_region_malloc(env->r,sizeof(*_tmp777)),((_tmp777[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp777))));}
# 464
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 466
struct _tuple23 _tmp778;return(_tmp778.f1=outflow,((_tmp778.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp778)));};};}
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
const char*_tmp77B;void*_tmp77A;(_tmp77A=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp77B="expression may not be initialized",_tag_dyneither(_tmp77B,sizeof(char),34))),_tag_dyneither(_tmp77A,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 485
struct _tuple16 _tmp77C;struct _tuple16 _tmpAE=(_tmp77C.f1=_tmpA7,((_tmp77C.f2=(env->fenv)->unknown_all,_tmp77C)));_npop_handler(0);return _tmpAE;};}
# 475
;_pop_region(rgn);}
# 493
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 500
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp782;const char*_tmp781;struct Cyc_Core_Impossible_exn_struct*_tmp780;(int)_throw((void*)((_tmp780=_cycalloc(sizeof(*_tmp780)),((_tmp780[0]=((_tmp782.tag=Cyc_Core_Impossible,((_tmp782.f1=((_tmp781="consume_zero_ral",_tag_dyneither(_tmp781,sizeof(char),17))),_tmp782)))),_tmp780)))));}else{
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
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp785;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp784;nzval=(void*)((_tmp784=_region_malloc((env->fenv)->r,sizeof(*_tmp784)),((_tmp784[0]=((_tmp785.tag=8,((_tmp785.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp785.f2=nzval,_tmp785)))))),_tmp784))));}
# 543
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 548
return _tmpBB;};}_LL60:;}_LL5B:;}
# 557
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 560
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple19 _tmp786;return(_tmp786.f1=outflow,((_tmp786.f2=outflow,_tmp786)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 563
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple19 _tmp787;return(_tmp787.f1=outflow,((_tmp787.f2=outflow,_tmp787)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 566
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp78A;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp789;nzval=(void*)((_tmp789=_region_malloc((env->fenv)->r,sizeof(*_tmp789)),((_tmp789[0]=((_tmp78A.tag=8,((_tmp78A.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp78A.f2=nzval,_tmp78A)))))),_tmp789))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp78D;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp78C;zval=(void*)((_tmp78C=_region_malloc((env->fenv)->r,sizeof(*_tmp78C)),((_tmp78C[0]=((_tmp78D.tag=8,((_tmp78D.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp78D.f2=zval,_tmp78D)))))),_tmp78C))));};}
# 572
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple19 _tmp78E;return
(_tmp78E.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp78E.f2=
# 577
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp78E)));};}_LL6A:;}_LL65:;}
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
struct _tuple16 _tmp78F;return(_tmp78F.f1=f,((_tmp78F.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp78F)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 602
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct _tuple15 _tmpD5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple15 _tmpD6=_tmpD5;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
void*_tmpD9=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpE0;enum Cyc_CfFlowInfo_InitLevel _tmpE3;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDA->tag != 8)goto _LL7C;else{_tmpDB=_tmpDA->f1;_tmpDC=(void*)_tmpDA->f2;}}_LL7B: {
# 606
struct Cyc_Core_Impossible_exn_struct _tmp795;const char*_tmp794;struct Cyc_Core_Impossible_exn_struct*_tmp793;(int)_throw((void*)((_tmp793=_cycalloc(sizeof(*_tmp793)),((_tmp793[0]=((_tmp795.tag=Cyc_Core_Impossible,((_tmp795.f1=((_tmp794="named location in anal_derefR",_tag_dyneither(_tmp794,sizeof(char),30))),_tmp795)))),_tmp793)))));}_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDD->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDE->tag != 2)goto _LL80;}_LL7F:
# 609
{void*_tmpE7=e->annot;void*_tmpE8=_tmpE7;struct Cyc_List_List*_tmpEA;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE8;if(_tmpE9->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpEA=_tmpE9->f1;}}_LL8A:
# 611
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEA))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 615
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC;if(_tmpED->tag != 22)goto _LL90;}_LL8F:
# 617
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp798;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp797;e->annot=(void*)((_tmp797=_cycalloc(sizeof(*_tmp797)),((_tmp797[0]=((_tmp798.tag=Cyc_CfFlowInfo_NotZero,((_tmp798.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp798)))),_tmp797))));}
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
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp79B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp79A;e->annot=(void*)((_tmp79A=_cycalloc(sizeof(*_tmp79A)),((_tmp79A[0]=((_tmp79B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp79B.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp79B)))),_tmp79A))));}else{
# 644
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp79E;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp79D;e->annot=(void*)((_tmp79D=_cycalloc(sizeof(*_tmp79D)),((_tmp79D[0]=((_tmp79E.tag=Cyc_CfFlowInfo_NotZero,((_tmp79E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp79E)))),_tmp79D))));}
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
struct _tuple16 _tmp79F;return(_tmp79F.f1=f,((_tmp79F.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpE0),_tmp79F)));};}_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE1->tag != 0)goto _LL84;}_LL83:
# 657
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp7A0;return(_tmp7A0.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7A0.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7A0)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE2->tag != 3)goto _LL86;else{_tmpE3=_tmpE2->f1;}}_LL85:
# 660
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpE3,_tmpD8);goto _LL87;_LL86:;_LL87:
# 662
{void*_tmpFF=e->r;void*_tmp100=_tmpFF;_LL9F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp100;if(_tmp101->tag != 22)goto _LLA1;}_LLA0:
# 664
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7A3;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7A2;e->annot=(void*)((_tmp7A2=_cycalloc(sizeof(*_tmp7A2)),((_tmp7A2[0]=((_tmp7A3.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7A3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7A3)))),_tmp7A2))));}
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
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7A6;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7A5;e->annot=(void*)((_tmp7A5=_cycalloc(sizeof(*_tmp7A5)),((_tmp7A5[0]=((_tmp7A6.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7A6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7A6)))),_tmp7A5))));}
goto _LLA3;_LLA3:;}{
# 682
enum Cyc_CfFlowInfo_InitLevel _tmp10A=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmp10A){case Cyc_CfFlowInfo_NoneIL: _LLA8: {
# 684
struct _tuple15 _tmp10B=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp10D;struct _tuple15 _tmp10C=_tmp10B;_tmp10D=_tmp10C.f1;
{void*_tmp10E=_tmp10D;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp10F->tag != 7)goto _LLAD;}_LLAC:
# 687
{const char*_tmp7A9;void*_tmp7A8;(_tmp7A8=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7A9="attempt to dereference a consumed alias-free pointer",_tag_dyneither(_tmp7A9,sizeof(char),53))),_tag_dyneither(_tmp7A8,sizeof(void*),0)));}
goto _LLAA;_LLAD:;_LLAE: {
# 690
const char*_tmp7AC;void*_tmp7AB;(_tmp7AB=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7AC="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7AC,sizeof(char),46))),_tag_dyneither(_tmp7AB,sizeof(void*),0)));}_LLAA:;}
# 692
goto _LLA9;}case Cyc_CfFlowInfo_AllIL: _LLA9: {
# 694
struct _tuple16 _tmp7AD;return(_tmp7AD.f1=f,((_tmp7AD.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7AD)));}case Cyc_CfFlowInfo_ThisIL: _LLAF: {
# 696
struct _tuple16 _tmp7AE;return(_tmp7AE.f1=f,((_tmp7AE.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp7AE)));}}};_LL74:;}_LL72:;_LL73: {
# 699
struct Cyc_Core_Impossible_exn_struct _tmp7B4;const char*_tmp7B3;struct Cyc_Core_Impossible_exn_struct*_tmp7B2;(int)_throw((void*)((_tmp7B2=_cycalloc(sizeof(*_tmp7B2)),((_tmp7B2[0]=((_tmp7B4.tag=Cyc_Core_Impossible,((_tmp7B4.f1=((_tmp7B3="right deref of non-pointer-type",_tag_dyneither(_tmp7B3,sizeof(char),32))),_tmp7B4)))),_tmp7B2)))));}_LL6F:;}
# 706
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 709
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 712
{void*_tmp119=e1->r;void*_tmp11A=_tmp119;void*_tmp11C;_LLB2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11A;if(_tmp11B->tag != 1)goto _LLB4;else{_tmp11C=(void*)_tmp11B->f2;}}_LLB3: {
# 714
struct Cyc_Absyn_Vardecl*_tmp11D=Cyc_Tcutil_nonesc_vardecl(_tmp11C);
if(_tmp11D != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp11D);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 720
goto _LLB1;}_LLB4:;_LLB5:
 goto _LLB1;_LLB1:;}{
# 724
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 730
return relns;};}
# 738
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 743
struct _tuple18 _tmp11E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp120;struct _tuple18 _tmp11F=_tmp11E;_tmp120=_tmp11F.f2;
{struct _tuple18 _tmp7B5;struct _tuple18 _tmp121=(_tmp7B5.f1=inflow,((_tmp7B5.f2=_tmp120,_tmp7B5)));struct _tuple18 _tmp122=_tmp121;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp124;struct Cyc_CfFlowInfo_Place*_tmp125;_LLB7: if(((_tmp122.f1).ReachableFL).tag != 2)goto _LLB9;_tmp123=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f1;_tmp124=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f2;if(((_tmp122.f2).PlaceL).tag != 1)goto _LLB9;_tmp125=(struct Cyc_CfFlowInfo_Place*)((_tmp122.f2).PlaceL).val;_LLB8: {
# 746
void*_tmp126=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 748
struct _tuple15 _tmp127=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp129;struct Cyc_List_List*_tmp12A;struct _tuple15 _tmp128=_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B8;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B7;_tmp126=(void*)((_tmp7B7=_region_malloc((env->fenv)->r,sizeof(*_tmp7B7)),((_tmp7B7[0]=((_tmp7B8.tag=8,((_tmp7B8.f1=(struct Cyc_Absyn_Vardecl*)_tmp12A->hd,((_tmp7B8.f2=_tmp126,_tmp7B8)))))),_tmp7B7))));}
# 753
_tmp123=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp123,env->all_changed,_tmp125,_tmp126);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp123,_tmp124);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 759
goto _LLB6;}_LLB9: if(((_tmp122.f1).BottomFL).tag != 1)goto _LLBB;_LLBA:
 goto _LLB6;_LLBB:;_LLBC:
# 762
{const char*_tmp7BB;void*_tmp7BA;(_tmp7BA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7BB="noconsume() parameters must be l-values",_tag_dyneither(_tmp7BB,sizeof(char),40))),_tag_dyneither(_tmp7BA,sizeof(void*),0)));}
goto _LLB6;_LLB6:;}
# 765
return inflow;}
# 770
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 778
union Cyc_CfFlowInfo_FlowInfo _tmp130=outflow;struct Cyc_Dict_Dict _tmp131;struct Cyc_List_List*_tmp132;_LLBE: if((_tmp130.BottomFL).tag != 1)goto _LLC0;_LLBF: {
# 780
struct _tuple16 _tmp7BC;return(_tmp7BC.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7BC.f2=rval,_tmp7BC)));}_LLC0: if((_tmp130.ReachableFL).tag != 2)goto _LLBD;_tmp131=((struct _tuple14)(_tmp130.ReachableFL).val).f1;_tmp132=((struct _tuple14)(_tmp130.ReachableFL).val).f2;_LLC1: {
# 782
union Cyc_CfFlowInfo_AbsLVal _tmp134=lval;struct Cyc_CfFlowInfo_Place*_tmp135;_LLC3: if((_tmp134.PlaceL).tag != 1)goto _LLC5;_tmp135=(struct Cyc_CfFlowInfo_Place*)(_tmp134.PlaceL).val;_LLC4: {
# 784
struct Cyc_Dict_Dict _tmp136=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp131,env->all_changed,_tmp135,rval);
_tmp132=Cyc_Relations_reln_assign_exp(fenv->r,_tmp132,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp132);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 790
struct _tuple15 _tmp137=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp131,_tmp135));void*_tmp139;struct _tuple15 _tmp138=_tmp137;_tmp139=_tmp138.f1;{
void*_tmp13A=_tmp139;_LLC8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp13B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13B->tag != 3)goto _LLCA;else{if(_tmp13B->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCA;}}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp13C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13C->tag != 7)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp13D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13D->tag != 0)goto _LLCE;}_LLCD:
 goto _LLC7;_LLCE:;_LLCF:
# 796
{const char*_tmp7BF;void*_tmp7BE;(_tmp7BE=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7BF="assignment may overwrite alias-free pointer(s)",_tag_dyneither(_tmp7BF,sizeof(char),47))),_tag_dyneither(_tmp7BE,sizeof(void*),0)));}
goto _LLC7;_LLC7:;};}
# 801
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple16 _tmp7C0;return(_tmp7C0.f1=outflow,((_tmp7C0.f2=rval,_tmp7C0)));};}_LLC5: if((_tmp134.UnknownL).tag != 2)goto _LLC2;_LLC6: {
# 804
struct _tuple16 _tmp7C1;return(_tmp7C1.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp7C1.f2=rval,_tmp7C1)));}_LLC2:;}_LLBD:;}
# 811
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 817
union Cyc_CfFlowInfo_FlowInfo _tmp142=f;struct Cyc_Dict_Dict _tmp143;struct Cyc_List_List*_tmp144;_LLD1: if((_tmp142.BottomFL).tag != 1)goto _LLD3;_LLD2:
 return Cyc_CfFlowInfo_BottomFL();_LLD3: if((_tmp142.ReachableFL).tag != 2)goto _LLD0;_tmp143=((struct _tuple14)(_tmp142.ReachableFL).val).f1;_tmp144=((struct _tuple14)(_tmp142.ReachableFL).val).f2;_LLD4:
# 822
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7C7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7C6;struct Cyc_CfFlowInfo_Place*_tmp7C5;_tmp143=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp143,env->all_changed,(
(_tmp7C5=_region_malloc(env->r,sizeof(*_tmp7C5)),((_tmp7C5->root=(void*)((_tmp7C7=_region_malloc(env->r,sizeof(*_tmp7C7)),((_tmp7C7[0]=((_tmp7C6.tag=0,((_tmp7C6.f1=vd,_tmp7C6)))),_tmp7C7)))),((_tmp7C5->fields=0,_tmp7C5)))))),rval);}
# 825
_tmp144=Cyc_Relations_reln_assign_var(env->r,_tmp144,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp148=Cyc_CfFlowInfo_ReachableFL(_tmp143,_tmp144);
Cyc_NewControlFlow_update_tryflow(env,_tmp148);
# 830
return _tmp148;};_LLD0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 834
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 841
if(vds == 0)return inflow;{
# 844
struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp7C8;es=((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp7C8->tl=es,_tmp7C8))))));}}}
# 850
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp149,fenv->unknown_all,pat_loc,name_locs);
# 852
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 856
struct _tuple16 _tmp14B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp14D;void*_tmp14E;struct _tuple16 _tmp14C=_tmp14B;_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp14D,_tmp14E);}}{
# 864
struct Cyc_List_List*_tmp14F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp14F != 0;_tmp14F=_tmp14F->tl){
struct _tuple24*_tmp150=(struct _tuple24*)_tmp14F->hd;struct Cyc_Absyn_Vardecl**_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct _tuple24*_tmp151=_tmp150;_tmp152=_tmp151->f1;_tmp153=_tmp151->f2;
if(_tmp152 != 0  && _tmp153 != 0){
if(_tmp153->topt == 0){
const char*_tmp7CC;void*_tmp7CB[1];struct Cyc_String_pa_PrintArg_struct _tmp7CA;(_tmp7CA.tag=0,((_tmp7CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp7CB[0]=& _tmp7CA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7CC="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp7CC,sizeof(char),41))),_tag_dyneither(_tmp7CB,sizeof(void*),1)))))));}{
# 878
struct Cyc_List_List _tmp7CD;struct Cyc_List_List l=(_tmp7CD.hd=_tmp153,((_tmp7CD.tl=0,_tmp7CD)));
union Cyc_CfFlowInfo_FlowInfo _tmp157=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp158=Cyc_NewControlFlow_anal_Lexp(env,_tmp157,0,0,_tmp153);union Cyc_CfFlowInfo_AbsLVal _tmp15A;struct _tuple18 _tmp159=_tmp158;_tmp15A=_tmp159.f2;{
struct _tuple16 _tmp15B=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp157,_tmp153);union Cyc_CfFlowInfo_FlowInfo _tmp15D;void*_tmp15E;struct _tuple16 _tmp15C=_tmp15B;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp15F=_tmp15D;struct Cyc_Dict_Dict _tmp160;struct Cyc_List_List*_tmp161;_LLD6: if((_tmp15F.ReachableFL).tag != 2)goto _LLD8;_tmp160=((struct _tuple14)(_tmp15F.ReachableFL).val).f1;_tmp161=((struct _tuple14)(_tmp15F.ReachableFL).val).f2;_LLD7:
# 884
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp162=_tmp15A;struct Cyc_CfFlowInfo_Place*_tmp163;_LLDB: if((_tmp162.PlaceL).tag != 1)goto _LLDD;_tmp163=(struct Cyc_CfFlowInfo_Place*)(_tmp162.PlaceL).val;_LLDC:
# 887
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7D0;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7CF;_tmp15E=(void*)((_tmp7CF=_region_malloc(fenv->r,sizeof(*_tmp7CF)),((_tmp7CF[0]=((_tmp7D0.tag=8,((_tmp7D0.f1=*_tmp152,((_tmp7D0.f2=_tmp15E,_tmp7D0)))))),_tmp7CF))));}{
# 890
void*_tmp166=Cyc_CfFlowInfo_lookup_place(_tmp160,_tmp163);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7D3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7D2;_tmp166=(void*)((_tmp7D2=_region_malloc(fenv->r,sizeof(*_tmp7D2)),((_tmp7D2[0]=((_tmp7D3.tag=8,((_tmp7D3.f1=*_tmp152,((_tmp7D3.f2=_tmp166,_tmp7D3)))))),_tmp7D2))));}
_tmp160=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp160,env->all_changed,_tmp163,_tmp166);
_tmp15D=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp161);
goto _LLDA;};_LLDD:;_LLDE:
# 897
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp153->topt)) && !
# 899
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp152)->type)){
# 901
const char*_tmp7D7;void*_tmp7D6[1];struct Cyc_String_pa_PrintArg_struct _tmp7D5;(_tmp7D5.tag=0,((_tmp7D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp7D6[0]=& _tmp7D5,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp7D7="aliased pattern expression not an l-value: %s",_tag_dyneither(_tmp7D7,sizeof(char),46))),_tag_dyneither(_tmp7D6,sizeof(void*),1)))))));}_LLDA:;}
# 909
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp15D,*_tmp152,_tmp153,_tmp15E,pat_loc);
goto _LLD5;_LLD8:;_LLD9:
# 912
 goto _LLD5;_LLD5:;};};};}}
# 917
return inflow;};};}
# 920
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 922
void*_tmp16D=e->r;void*_tmp16E=_tmp16D;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;_LLE0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp16F->tag != 1)goto _LLE2;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp170=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp16F->f2);if(_tmp170->tag != 4)goto _LLE2;}}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp171->tag != 1)goto _LLE4;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 3)goto _LLE4;}}_LLE3:
 goto _LLE5;_LLE4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp173->tag != 1)goto _LLE6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 5)goto _LLE6;}}_LLE5:
 return 1;_LLE6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp175->tag != 19)goto _LLE8;else{_tmp176=_tmp175->f1;}}_LLE7:
 _tmp178=_tmp176;goto _LLE9;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp177->tag != 20)goto _LLEA;else{_tmp178=_tmp177->f1;}}_LLE9:
 _tmp17A=_tmp178;goto _LLEB;_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp179->tag != 21)goto _LLEC;else{_tmp17A=_tmp179->f1;}}_LLEB:
# 929
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17A,cast_ok);_LLEC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17B->tag != 22)goto _LLEE;else{_tmp17C=_tmp17B->f1;}}_LLED: {
# 931
void*_tmp17F=Cyc_Tcutil_compress((void*)_check_null(_tmp17C->topt));void*_tmp180=_tmp17F;_LLF3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp180;if(_tmp181->tag != 10)goto _LLF5;}_LLF4:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17C,cast_ok);_LLF5:;_LLF6:
 return 0;_LLF2:;}_LLEE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17D->tag != 13)goto _LLF0;else{_tmp17E=_tmp17D->f2;}}_LLEF:
# 936
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17E,cast_ok);else{
return 0;}_LLF0:;_LLF1:
 return 0;_LLDF:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp88A(unsigned int*_tmp889,unsigned int*_tmp888,void***_tmp886,struct Cyc_List_List**_tmp386){for(*_tmp889=0;*_tmp889 < *_tmp888;(*_tmp889)++){
# 1523 "new_control_flow.cyc"
void*_tmp884;(*_tmp886)[*_tmp889]=(void*)((_tmp884=(void*)((struct Cyc_List_List*)_check_null(*_tmp386))->hd,((*_tmp386=(*_tmp386)->tl,_tmp884))));}}
# 943 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 947
struct Cyc_CfFlowInfo_FlowEnv*_tmp182=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 960
{union Cyc_CfFlowInfo_FlowInfo _tmp183=inflow;struct Cyc_Dict_Dict _tmp184;struct Cyc_List_List*_tmp185;_LLF8: if((_tmp183.BottomFL).tag != 1)goto _LLFA;_LLF9: {
struct _tuple16 _tmp7D8;return(_tmp7D8.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7D8.f2=_tmp182->unknown_all,_tmp7D8)));}_LLFA: if((_tmp183.ReachableFL).tag != 2)goto _LLF7;_tmp184=((struct _tuple14)(_tmp183.ReachableFL).val).f1;_tmp185=((struct _tuple14)(_tmp183.ReachableFL).val).f2;_LLFB:
 d=_tmp184;relns=_tmp185;_LLF7:;}
# 975 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 996 "new_control_flow.cyc"
struct _tuple18 _tmp187=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp189;union Cyc_CfFlowInfo_AbsLVal _tmp18A;struct _tuple18 _tmp188=_tmp187;_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{
struct _tuple18 _tmp7D9;struct _tuple18 _tmp18B=(_tmp7D9.f1=_tmp189,((_tmp7D9.f2=_tmp18A,_tmp7D9)));struct _tuple18 _tmp18C=_tmp18B;struct Cyc_Dict_Dict _tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_CfFlowInfo_Place*_tmp18F;_LLFD: if(((_tmp18C.f1).ReachableFL).tag != 2)goto _LLFF;_tmp18D=((struct _tuple14)((_tmp18C.f1).ReachableFL).val).f1;_tmp18E=((struct _tuple14)((_tmp18C.f1).ReachableFL).val).f2;if(((_tmp18C.f2).PlaceL).tag != 1)goto _LLFF;_tmp18F=(struct Cyc_CfFlowInfo_Place*)((_tmp18C.f2).PlaceL).val;_LLFE: {
# 999
void*_tmp190=Cyc_CfFlowInfo_lookup_place(_tmp18D,_tmp18F);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp190,& needs_unconsume)){
{const char*_tmp7DC;void*_tmp7DB;(_tmp7DB=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7DC="expression attempts to copy a consumed alias-free value",_tag_dyneither(_tmp7DC,sizeof(char),56))),_tag_dyneither(_tmp7DB,sizeof(void*),0)));}{
struct _tuple16 _tmp7DD;return(_tmp7DD.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7DD.f2=_tmp182->unknown_all,_tmp7DD)));};}else{
# 1005
if(needs_unconsume){
# 1007
struct _tuple16 _tmp7DE;return(_tmp7DE.f1=_tmp189,((_tmp7DE.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp182,_tmp190),_tmp7DE)));}else{
# 1010
void*_tmp195=Cyc_CfFlowInfo_make_unique_consumed(_tmp182,(void*)_check_null(e->topt),e,env->iteration_num,_tmp190);
struct Cyc_Dict_Dict _tmp196=Cyc_CfFlowInfo_assign_place(_tmp182,e->loc,_tmp18D,env->all_changed,_tmp18F,_tmp195);
# 1022
struct _tuple16 _tmp7DF;return(_tmp7DF.f1=Cyc_CfFlowInfo_ReachableFL(_tmp196,_tmp18E),((_tmp7DF.f2=_tmp190,_tmp7DF)));}}}_LLFF:;_LL100:
# 1024
 goto _LLFC;_LLFC:;};}{
# 1027
void*_tmp199=e->r;void*_tmp19A=_tmp199;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B6;struct Cyc_Absyn_Vardecl*_tmp1B9;enum Cyc_Absyn_Primop _tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_List_List*_tmp1CC;int _tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;void**_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;int _tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct _dyneither_ptr*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct _dyneither_ptr*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E4;struct _dyneither_ptr*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F4;struct Cyc_Absyn_Datatypedecl*_tmp1F5;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1FB;enum Cyc_Absyn_AggrKind _tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Vardecl*_tmp202;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp204;int _tmp205;struct Cyc_Absyn_Exp*_tmp207;void*_tmp208;int _tmp209;struct Cyc_Absyn_Stmt*_tmp20B;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19B->tag != 13)goto _LL104;else{_tmp19C=_tmp19B->f2;if(_tmp19B->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL104;}}_LL103: {
# 1031
struct _tuple16 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19C);union Cyc_CfFlowInfo_FlowInfo _tmp214;void*_tmp215;struct _tuple16 _tmp213=_tmp212;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;{
struct _tuple16 _tmp216=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp214,_tmp19C,_tmp215);union Cyc_CfFlowInfo_FlowInfo _tmp218;void*_tmp219;struct _tuple16 _tmp217=_tmp216;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;{
struct _tuple16 _tmp7E0;return(_tmp7E0.f1=_tmp218,((_tmp7E0.f2=_tmp215,_tmp7E0)));};};}_LL104: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19D->tag != 13)goto _LL106;else{_tmp19E=_tmp19D->f2;}}_LL105:
# 1037
 _tmp1A0=_tmp19E;goto _LL107;_LL106: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19F->tag != 11)goto _LL108;else{_tmp1A0=_tmp19F->f1;}}_LL107:
 _tmp1A2=_tmp1A0;goto _LL109;_LL108: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A1->tag != 12)goto _LL10A;else{_tmp1A2=_tmp1A1->f1;}}_LL109:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1A2);_LL10A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A3->tag != 0)goto _LL10C;else{if(((_tmp1A3->f1).Null_c).tag != 1)goto _LL10C;}}_LL10B:
# 1041
 goto _LL10D;_LL10C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A4->tag != 0)goto _LL10E;else{if(((_tmp1A4->f1).Int_c).tag != 5)goto _LL10E;if(((struct _tuple7)((_tmp1A4->f1).Int_c).val).f2 != 0)goto _LL10E;}}_LL10D: {
struct _tuple16 _tmp7E1;return(_tmp7E1.f1=inflow,((_tmp7E1.f2=_tmp182->zero,_tmp7E1)));}_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A5->tag != 0)goto _LL110;else{if(((_tmp1A5->f1).Int_c).tag != 5)goto _LL110;}}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A6->tag != 1)goto _LL112;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1A7=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp1A6->f2);if(_tmp1A7->tag != 2)goto _LL112;}}_LL111: {
struct _tuple16 _tmp7E2;return(_tmp7E2.f1=inflow,((_tmp7E2.f2=_tmp182->notzeroall,_tmp7E2)));}_LL112: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A8->tag != 30)goto _LL114;else{if(_tmp1A8->f1 != 0)goto _LL114;}}_LL113:
# 1046
 goto _LL115;_LL114: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A9->tag != 0)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AA->tag != 17)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AB->tag != 16)goto _LL11A;}_LL119:
 goto _LL11B;_LL11A: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AC->tag != 18)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AD->tag != 32)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AE->tag != 31)goto _LL120;}_LL11F: {
struct _tuple16 _tmp7E3;return(_tmp7E3.f1=inflow,((_tmp7E3.f2=_tmp182->unknown_all,_tmp7E3)));}_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AF->tag != 1)goto _LL122;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1B0=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp1AF->f2);if(_tmp1B0->tag != 1)goto _LL122;}}_LL121: {
# 1055
struct _tuple16 _tmp7E4;return(_tmp7E4.f1=inflow,((_tmp7E4.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp7E4)));}_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B1->tag != 1)goto _LL124;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1B2=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp1B1->f2);if(_tmp1B2->tag != 3)goto _LL124;else{_tmp1B3=_tmp1B2->f1;}}}_LL123:
# 1058
 _tmp1B6=_tmp1B3;goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B4->tag != 1)goto _LL126;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1B5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp1B4->f2);if(_tmp1B5->tag != 4)goto _LL126;else{_tmp1B6=_tmp1B5->f1;}}}_LL125:
 _tmp1B9=_tmp1B6;goto _LL127;_LL126: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B7->tag != 1)goto _LL128;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp1B7->f2);if(_tmp1B8->tag != 5)goto _LL128;else{_tmp1B9=_tmp1B8->f1;}}}_LL127: {
# 1062
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7E5;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp7E5.tag=0,((_tmp7E5.f1=_tmp1B9,_tmp7E5)));
struct _tuple16 _tmp7E6;return(_tmp7E6.f1=inflow,((_tmp7E6.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp7E6)));}_LL128: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BA->tag != 2)goto _LL12A;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL129: {
# 1069
struct _tuple16 _tmp221=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1BC);union Cyc_CfFlowInfo_FlowInfo _tmp223;void*_tmp224;struct _tuple16 _tmp222=_tmp221;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;{
struct _tuple16 _tmp7E7;return(_tmp7E7.f1=_tmp223,((_tmp7E7.f2=_tmp224,_tmp7E7)));};}_LL12A: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BD->tag != 4)goto _LL12C;else{_tmp1BE=_tmp1BD->f1;}}_LL12B: {
# 1073
struct Cyc_List_List _tmp7E8;struct Cyc_List_List _tmp226=(_tmp7E8.hd=_tmp1BE,((_tmp7E8.tl=0,_tmp7E8)));
struct _tuple16 _tmp227=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp226);union Cyc_CfFlowInfo_FlowInfo _tmp229;struct _tuple16 _tmp228=_tmp227;_tmp229=_tmp228.f1;{
struct _tuple18 _tmp22A=Cyc_NewControlFlow_anal_Lexp(env,_tmp229,0,0,_tmp1BE);union Cyc_CfFlowInfo_AbsLVal _tmp22C;struct _tuple18 _tmp22B=_tmp22A;_tmp22C=_tmp22B.f2;
{struct _tuple25 _tmp7E9;struct _tuple25 _tmp22D=(_tmp7E9.f1=_tmp22C,((_tmp7E9.f2=_tmp229,_tmp7E9)));struct _tuple25 _tmp22E=_tmp22D;struct Cyc_CfFlowInfo_Place*_tmp22F;struct Cyc_Dict_Dict _tmp230;struct Cyc_List_List*_tmp231;_LL16B: if(((_tmp22E.f1).PlaceL).tag != 1)goto _LL16D;_tmp22F=(struct Cyc_CfFlowInfo_Place*)((_tmp22E.f1).PlaceL).val;if(((_tmp22E.f2).ReachableFL).tag != 2)goto _LL16D;_tmp230=((struct _tuple14)((_tmp22E.f2).ReachableFL).val).f1;_tmp231=((struct _tuple14)((_tmp22E.f2).ReachableFL).val).f2;_LL16C:
# 1078
 _tmp231=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp231,_tmp1BE);
_tmp229=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1BE->loc,_tmp230,env->all_changed,_tmp22F,_tmp182->unknown_all),_tmp231);
# 1083
goto _LL16A;_LL16D:;_LL16E:
 goto _LL16A;_LL16A:;}{
# 1086
struct _tuple16 _tmp7EA;return(_tmp7EA.f1=_tmp229,((_tmp7EA.f2=_tmp182->unknown_all,_tmp7EA)));};};}_LL12C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1BF=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BF->tag != 3)goto _LL12E;else{_tmp1C0=_tmp1BF->f1;if(_tmp1BF->f2 == 0)goto _LL12E;_tmp1C1=_tmp1BF->f3;}}_LL12D:
# 1089
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7ED;void*_tmp7EC;(_tmp7EC=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7ED="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp7ED,sizeof(char),62))),_tag_dyneither(_tmp7EC,sizeof(void*),0)));}{
struct _tuple16 _tmp7EE;return(_tmp7EE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7EE.f2=_tmp182->unknown_all,_tmp7EE)));};}{
# 1093
struct Cyc_List_List _tmp7EF;struct Cyc_List_List _tmp238=(_tmp7EF.hd=_tmp1C1,((_tmp7EF.tl=0,_tmp7EF)));
struct Cyc_List_List _tmp7F0;struct Cyc_List_List _tmp239=(_tmp7F0.hd=_tmp1C0,((_tmp7F0.tl=& _tmp238,_tmp7F0)));
struct _tuple16 _tmp23A=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp239);union Cyc_CfFlowInfo_FlowInfo _tmp23C;struct _tuple16 _tmp23B=_tmp23A;_tmp23C=_tmp23B.f1;{
struct _tuple18 _tmp23D=Cyc_NewControlFlow_anal_Lexp(env,_tmp23C,0,0,_tmp1C0);union Cyc_CfFlowInfo_AbsLVal _tmp23F;struct _tuple18 _tmp23E=_tmp23D;_tmp23F=_tmp23E.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp240=_tmp23C;struct Cyc_Dict_Dict _tmp241;struct Cyc_List_List*_tmp242;_LL170: if((_tmp240.ReachableFL).tag != 2)goto _LL172;_tmp241=((struct _tuple14)(_tmp240.ReachableFL).val).f1;_tmp242=((struct _tuple14)(_tmp240.ReachableFL).val).f2;_LL171:
# 1099
{union Cyc_CfFlowInfo_AbsLVal _tmp243=_tmp23F;struct Cyc_CfFlowInfo_Place*_tmp244;_LL175: if((_tmp243.PlaceL).tag != 1)goto _LL177;_tmp244=(struct Cyc_CfFlowInfo_Place*)(_tmp243.PlaceL).val;_LL176:
# 1103
 _tmp242=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp242,_tmp1C0);
_tmp241=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1C0->loc,_tmp241,env->all_changed,_tmp244,_tmp182->unknown_all);
# 1106
_tmp23C=Cyc_CfFlowInfo_ReachableFL(_tmp241,_tmp242);
# 1110
goto _LL174;_LL177: if((_tmp243.UnknownL).tag != 2)goto _LL174;_LL178:
# 1113
 goto _LL174;_LL174:;}
# 1115
goto _LL16F;_LL172:;_LL173:
 goto _LL16F;_LL16F:;}{
# 1118
struct _tuple16 _tmp7F1;return(_tmp7F1.f1=_tmp23C,((_tmp7F1.f2=_tmp182->unknown_all,_tmp7F1)));};};};_LL12E: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C2->tag != 3)goto _LL130;else{_tmp1C3=_tmp1C2->f1;if(_tmp1C2->f2 != 0)goto _LL130;_tmp1C4=_tmp1C2->f3;}}_LL12F:
# 1122
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp7F4;void*_tmp7F3;(_tmp7F3=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7F4="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp7F4,sizeof(char),62))),_tag_dyneither(_tmp7F3,sizeof(void*),0)));}{
struct _tuple16 _tmp7F5;return(_tmp7F5.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F5.f2=_tmp182->unknown_all,_tmp7F5)));};}{
# 1126
struct Cyc_Dict_Dict*_tmp24B=env->all_changed;
# 1128
{struct Cyc_Absyn_Exp*_tmp7F6[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp7F6[1]=_tmp1C4,((_tmp7F6[0]=_tmp1C3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp7F6,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp7F7;env->all_changed=((_tmp7F7=_region_malloc(env->r,sizeof(*_tmp7F7)),((_tmp7F7[0]=_tmp182->mt_place_set,_tmp7F7))));}{
struct _tuple18 _tmp24E=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp250;union Cyc_CfFlowInfo_AbsLVal _tmp251;struct _tuple18 _tmp24F=_tmp24E;_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;{
struct Cyc_Dict_Dict _tmp252=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp7F8;env->all_changed=((_tmp7F8=_region_malloc(env->r,sizeof(*_tmp7F8)),((_tmp7F8[0]=_tmp182->mt_place_set,_tmp7F8))));}{
struct _tuple16 _tmp254=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C4);union Cyc_CfFlowInfo_FlowInfo _tmp256;void*_tmp257;struct _tuple16 _tmp255=_tmp254;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{
struct Cyc_Dict_Dict _tmp258=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp259=Cyc_CfFlowInfo_after_flow(_tmp182,& _tmp252,_tmp250,_tmp256,_tmp252,_tmp258);
# 1139
union Cyc_CfFlowInfo_FlowInfo _tmp25A=Cyc_CfFlowInfo_join_flow(_tmp182,_tmp24B,inflow,_tmp259);
# 1142
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp25A,inflow)){
if(_tmp24B == 0)
env->all_changed=0;else{
# 1146
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp24B,
Cyc_CfFlowInfo_union_place_set(_tmp252,_tmp258,0),0);}
# 1149
return Cyc_NewControlFlow_do_assign(_tmp182,env,_tmp259,_tmp1C3,_tmp251,_tmp1C4,_tmp257,e->loc);}
# 1152
inflow=_tmp25A;};};};};}};_LL130: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C5->tag != 8)goto _LL132;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}_LL131: {
# 1157
struct _tuple16 _tmp25B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp25D;void*_tmp25E;struct _tuple16 _tmp25C=_tmp25B;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25D,_tmp1C7);}_LL132: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C8->tag != 10)goto _LL134;else{_tmp1C9=_tmp1C8->f1;}}_LL133: {
# 1161
struct _tuple16 _tmp25F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C9);union Cyc_CfFlowInfo_FlowInfo _tmp261;void*_tmp262;struct _tuple16 _tmp260=_tmp25F;_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1C9->loc,_tmp261,_tmp262);{
struct _tuple16 _tmp7F9;return(_tmp7F9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7F9.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp7F9)));};}_LL134: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1CA->tag != 9)goto _LL136;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}_LL135: {
# 1166
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);
{struct Cyc_List_List*_tmp7FA;struct _tuple23 _tmp265=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp7FA=_region_malloc(temp,sizeof(*_tmp7FA)),((_tmp7FA->hd=_tmp1CB,((_tmp7FA->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1CC),_tmp7FA)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp267;struct Cyc_List_List*_tmp268;struct _tuple23 _tmp266=_tmp265;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{
# 1169
union Cyc_CfFlowInfo_FlowInfo _tmp269=Cyc_NewControlFlow_use_Rval(env,_tmp1CB->loc,_tmp267,(void*)((struct Cyc_List_List*)_check_null(_tmp268))->hd);
_tmp268=_tmp268->tl;{
# 1172
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp26A=Cyc_Tcutil_compress((void*)_check_null(_tmp1CB->topt));void*_tmp26B=_tmp26A;void*_tmp26D;_LL17A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B;if(_tmp26C->tag != 5)goto _LL17C;else{_tmp26D=(_tmp26C->f1).elt_typ;}}_LL17B:
# 1177
{void*_tmp26E=Cyc_Tcutil_compress(_tmp26D);void*_tmp26F=_tmp26E;struct Cyc_List_List*_tmp271;_LL17F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp270=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26F;if(_tmp270->tag != 9)goto _LL181;else{_tmp271=(_tmp270->f1).attributes;}}_LL180:
# 1179
 for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
# 1181
void*_tmp272=(void*)_tmp271->hd;void*_tmp273=_tmp272;int _tmp275;int _tmp277;int _tmp279;_LL184: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp274=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp273;if(_tmp274->tag != 20)goto _LL186;else{_tmp275=_tmp274->f1;}}_LL185:
# 1183
{struct Cyc_List_List*_tmp7FB;init_params=((_tmp7FB=_region_malloc(temp,sizeof(*_tmp7FB)),((_tmp7FB->hd=(void*)_tmp275,((_tmp7FB->tl=init_params,_tmp7FB))))));}goto _LL183;_LL186: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp276=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp273;if(_tmp276->tag != 21)goto _LL188;else{_tmp277=_tmp276->f1;}}_LL187:
# 1185
{struct Cyc_List_List*_tmp7FC;nolive_unique_params=((_tmp7FC=_region_malloc(temp,sizeof(*_tmp7FC)),((_tmp7FC->hd=(void*)_tmp277,((_tmp7FC->tl=nolive_unique_params,_tmp7FC))))));}
goto _LL183;_LL188: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp278=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp273;if(_tmp278->tag != 22)goto _LL18A;else{_tmp279=_tmp278->f1;}}_LL189:
# 1189
{struct Cyc_List_List*_tmp7FD;noconsume_params=((_tmp7FD=_region_malloc(temp,sizeof(*_tmp7FD)),((_tmp7FD->hd=(void*)_tmp279,((_tmp7FD->tl=noconsume_params,_tmp7FD))))));}
goto _LL183;_LL18A:;_LL18B:
 goto _LL183;_LL183:;}
# 1193
goto _LL17E;_LL181:;_LL182: {
const char*_tmp800;void*_tmp7FF;(_tmp7FF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp800="anal_Rexp: bad function type",_tag_dyneither(_tmp800,sizeof(char),29))),_tag_dyneither(_tmp7FF,sizeof(void*),0)));}_LL17E:;}
# 1196
goto _LL179;_LL17C:;_LL17D: {
const char*_tmp803;void*_tmp802;(_tmp802=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp803="anal_Rexp: bad function type",_tag_dyneither(_tmp803,sizeof(char),29))),_tag_dyneither(_tmp802,sizeof(void*),0)));}_LL179:;}
# 1199
{int i=1;for(0;_tmp268 != 0;(((_tmp268=_tmp268->tl,_tmp1CC=((struct Cyc_List_List*)_check_null(_tmp1CC))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp281=_tmp267;struct Cyc_Dict_Dict _tmp282;_LL18D: if((_tmp281.BottomFL).tag != 1)goto _LL18F;_LL18E:
 goto _LL18C;_LL18F: if((_tmp281.ReachableFL).tag != 2)goto _LL18C;_tmp282=((struct _tuple14)(_tmp281.ReachableFL).val).f1;_LL190:
# 1204
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp282,(void*)_tmp268->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp806;void*_tmp805;(_tmp805=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,((_tmp806="expression may not be initialized",_tag_dyneither(_tmp806,sizeof(char),34))),_tag_dyneither(_tmp805,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp285=_tmp269;struct Cyc_Dict_Dict _tmp286;struct Cyc_List_List*_tmp287;_LL192: if((_tmp285.BottomFL).tag != 1)goto _LL194;_LL193:
 goto _LL191;_LL194: if((_tmp285.ReachableFL).tag != 2)goto _LL191;_tmp286=((struct _tuple14)(_tmp285.ReachableFL).val).f1;_tmp287=((struct _tuple14)(_tmp285.ReachableFL).val).f2;_LL195: {
# 1211
struct Cyc_Dict_Dict _tmp288=Cyc_CfFlowInfo_escape_deref(_tmp182,_tmp286,env->all_changed,(void*)_tmp268->hd);
{void*_tmp289=(void*)_tmp268->hd;void*_tmp28A=_tmp289;struct Cyc_CfFlowInfo_Place*_tmp28C;_LL197: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp28B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp28A;if(_tmp28B->tag != 5)goto _LL199;else{_tmp28C=_tmp28B->f1;}}_LL198:
# 1214
{void*_tmp28D=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->topt));void*_tmp28E=_tmp28D;void*_tmp290;_LL19C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28E;if(_tmp28F->tag != 5)goto _LL19E;else{_tmp290=(_tmp28F->f1).elt_typ;}}_LL19D:
# 1216
 _tmp288=Cyc_CfFlowInfo_assign_place(_tmp182,((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->loc,_tmp288,env->all_changed,_tmp28C,
# 1218
Cyc_CfFlowInfo_typ_to_absrval(_tmp182,_tmp290,0,_tmp182->esc_all));
# 1221
goto _LL19B;_LL19E:;_LL19F: {
const char*_tmp809;void*_tmp808;(_tmp808=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp809="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp809,sizeof(char),39))),_tag_dyneither(_tmp808,sizeof(void*),0)));}_LL19B:;}
# 1224
goto _LL196;_LL199:;_LL19A:
 goto _LL196;_LL196:;}
# 1227
_tmp269=Cyc_CfFlowInfo_ReachableFL(_tmp288,_tmp287);
goto _LL191;}_LL191:;}
# 1230
goto _LL18C;_LL18C:;}else{
# 1233
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1238
_tmp269=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->topt),_tmp269,(void*)_tmp268->hd);else{
# 1248
_tmp269=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,_tmp269,(void*)_tmp268->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->topt),0))
_tmp269=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp269,(struct Cyc_Absyn_Exp*)_tmp1CC->hd,((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->loc,(void*)_tmp268->hd);}}}}
# 1255
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1CB->topt))){
struct _tuple16 _tmp80A;struct _tuple16 _tmp294=(_tmp80A.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp80A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp80A)));_npop_handler(0);return _tmp294;}else{
# 1258
struct _tuple16 _tmp80B;struct _tuple16 _tmp296=(_tmp80B.f1=_tmp269,((_tmp80B.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp80B)));_npop_handler(0);return _tmp296;}};};}
# 1167
;_pop_region(temp);}_LL136: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1CD->tag != 33)goto _LL138;else{_tmp1CE=(_tmp1CD->f1).is_calloc;_tmp1CF=(_tmp1CD->f1).rgn;_tmp1D0=(_tmp1CD->f1).elt_type;_tmp1D1=(_tmp1CD->f1).num_elts;_tmp1D2=(_tmp1CD->f1).fat_result;}}_LL137: {
# 1261
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp80E;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp80D;void*root=(void*)((_tmp80D=_region_malloc(_tmp182->r,sizeof(*_tmp80D)),((_tmp80D[0]=((_tmp80E.tag=1,((_tmp80E.f1=_tmp1D1,((_tmp80E.f2=(void*)_check_null(e->topt),_tmp80E)))))),_tmp80D))));
struct Cyc_CfFlowInfo_Place*_tmp80F;struct Cyc_CfFlowInfo_Place*place=(_tmp80F=_region_malloc(_tmp182->r,sizeof(*_tmp80F)),((_tmp80F->root=root,((_tmp80F->fields=0,_tmp80F)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp812;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp811;void*rval=(void*)((_tmp811=_region_malloc(_tmp182->r,sizeof(*_tmp811)),((_tmp811[0]=((_tmp812.tag=5,((_tmp812.f1=place,_tmp812)))),_tmp811))));
void*place_val;
if(_tmp1D2)place_val=_tmp182->notzeroall;else{
if(_tmp1CE)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,*((void**)_check_null(_tmp1D0)),0,_tmp182->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,*((void**)_check_null(_tmp1D0)),0,_tmp182->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1CF != 0){
struct _RegionHandle _tmp298=_new_region("temp");struct _RegionHandle*temp=& _tmp298;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp813[2];struct _tuple23 _tmp299=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp813[1]=_tmp1D1,((_tmp813[0]=_tmp1CF,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp813,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1272
union Cyc_CfFlowInfo_FlowInfo _tmp29B;struct Cyc_List_List*_tmp29C;struct _tuple23 _tmp29A=_tmp299;_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;
# 1274
outflow=_tmp29B;}
# 1272
;_pop_region(temp);}else{
# 1277
struct _tuple16 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp2A0;struct _tuple16 _tmp29F=_tmp29E;_tmp2A0=_tmp29F.f1;
outflow=_tmp2A0;}{
# 1280
union Cyc_CfFlowInfo_FlowInfo _tmp2A1=outflow;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*_tmp2A3;_LL1A1: if((_tmp2A1.BottomFL).tag != 1)goto _LL1A3;_LL1A2: {
struct _tuple16 _tmp814;return(_tmp814.f1=outflow,((_tmp814.f2=rval,_tmp814)));}_LL1A3: if((_tmp2A1.ReachableFL).tag != 2)goto _LL1A0;_tmp2A2=((struct _tuple14)(_tmp2A1.ReachableFL).val).f1;_tmp2A3=((struct _tuple14)(_tmp2A1.ReachableFL).val).f2;_LL1A4: {
# 1283
struct _tuple16 _tmp815;return(_tmp815.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A2,root,place_val),_tmp2A3),((_tmp815.f2=rval,_tmp815)));}_LL1A0:;};};}_LL138: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D3->tag != 34)goto _LL13A;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;}}_LL139: {
# 1287
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1293
struct _RegionHandle _tmp2AB=_new_region("temp");struct _RegionHandle*temp=& _tmp2AB;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp816[2];struct _tuple23 _tmp2AC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp816[1]=_tmp1D5,((_tmp816[0]=_tmp1D4,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp816,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1294
union Cyc_CfFlowInfo_FlowInfo _tmp2AE;struct Cyc_List_List*_tmp2AF;struct _tuple23 _tmp2AD=_tmp2AC;_tmp2AE=_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;
# 1296
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF->tl))->hd;
outflow=_tmp2AE;}{
# 1301
struct _tuple18 _tmp2B1=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D4);union Cyc_CfFlowInfo_AbsLVal _tmp2B3;struct _tuple18 _tmp2B2=_tmp2B1;_tmp2B3=_tmp2B2.f2;{
struct _tuple18 _tmp2B4=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D5);union Cyc_CfFlowInfo_AbsLVal _tmp2B6;struct _tuple18 _tmp2B5=_tmp2B4;_tmp2B6=_tmp2B5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2B7=outflow;struct Cyc_Dict_Dict _tmp2B8;struct Cyc_List_List*_tmp2B9;_LL1A6: if((_tmp2B7.ReachableFL).tag != 2)goto _LL1A8;_tmp2B8=((struct _tuple14)(_tmp2B7.ReachableFL).val).f1;_tmp2B9=((struct _tuple14)(_tmp2B7.ReachableFL).val).f2;_LL1A7:
# 1305
{union Cyc_CfFlowInfo_AbsLVal _tmp2BA=_tmp2B3;struct Cyc_CfFlowInfo_Place*_tmp2BB;_LL1AB: if((_tmp2BA.PlaceL).tag != 1)goto _LL1AD;_tmp2BB=(struct Cyc_CfFlowInfo_Place*)(_tmp2BA.PlaceL).val;_LL1AC:
# 1307
 _tmp2B8=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1D4->loc,_tmp2B8,env->all_changed,_tmp2BB,right_rval);
# 1309
goto _LL1AA;_LL1AD: if((_tmp2BA.UnknownL).tag != 2)goto _LL1AA;_LL1AE:
# 1314
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp2B8,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp819;void*_tmp818;(_tmp818=0,Cyc_Tcutil_terr(_tmp1D5->loc,((_tmp819="expression may not be fully initialized",_tag_dyneither(_tmp819,sizeof(char),40))),_tag_dyneither(_tmp818,sizeof(void*),0)));}
goto _LL1AA;_LL1AA:;}
# 1318
{union Cyc_CfFlowInfo_AbsLVal _tmp2BE=_tmp2B6;struct Cyc_CfFlowInfo_Place*_tmp2BF;_LL1B0: if((_tmp2BE.PlaceL).tag != 1)goto _LL1B2;_tmp2BF=(struct Cyc_CfFlowInfo_Place*)(_tmp2BE.PlaceL).val;_LL1B1:
# 1320
 _tmp2B8=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1D5->loc,_tmp2B8,env->all_changed,_tmp2BF,left_rval);
# 1322
goto _LL1AF;_LL1B2: if((_tmp2BE.UnknownL).tag != 2)goto _LL1AF;_LL1B3:
# 1324
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp2B8,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp81C;void*_tmp81B;(_tmp81B=0,Cyc_Tcutil_terr(_tmp1D4->loc,((_tmp81C="expression may not be fully initialized",_tag_dyneither(_tmp81C,sizeof(char),40))),_tag_dyneither(_tmp81B,sizeof(void*),0)));}
goto _LL1AF;_LL1AF:;}
# 1329
_tmp2B9=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp2B9,_tmp1D4);
_tmp2B9=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp2B9,_tmp1D5);
# 1332
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B8,_tmp2B9);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL1A5;_LL1A8:;_LL1A9:
 goto _LL1A5;_LL1A5:;}{
# 1339
struct _tuple16 _tmp81D;struct _tuple16 _tmp2C3=(_tmp81D.f1=outflow,((_tmp81D.f2=_tmp182->unknown_all,_tmp81D)));_npop_handler(0);return _tmp2C3;};};};
# 1293
;_pop_region(temp);}_LL13A: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D6->tag != 15)goto _LL13C;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL13B: {
# 1342
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp820;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp81F;void*root=(void*)((_tmp81F=_region_malloc(_tmp182->r,sizeof(*_tmp81F)),((_tmp81F[0]=((_tmp820.tag=1,((_tmp820.f1=_tmp1D8,((_tmp820.f2=(void*)_check_null(e->topt),_tmp820)))))),_tmp81F))));
struct Cyc_CfFlowInfo_Place*_tmp821;struct Cyc_CfFlowInfo_Place*place=(_tmp821=_region_malloc(_tmp182->r,sizeof(*_tmp821)),((_tmp821->root=root,((_tmp821->fields=0,_tmp821)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp824;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp823;void*rval=(void*)((_tmp823=_region_malloc(_tmp182->r,sizeof(*_tmp823)),((_tmp823[0]=((_tmp824.tag=5,((_tmp824.f1=place,_tmp824)))),_tmp823))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1D7 != 0){
struct _RegionHandle _tmp2C4=_new_region("temp");struct _RegionHandle*temp=& _tmp2C4;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp825[2];struct _tuple23 _tmp2C5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp825[1]=_tmp1D8,((_tmp825[0]=_tmp1D7,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp825,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1350
union Cyc_CfFlowInfo_FlowInfo _tmp2C7;struct Cyc_List_List*_tmp2C8;struct _tuple23 _tmp2C6=_tmp2C5;_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;
# 1352
outflow=_tmp2C7;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C8))->tl))->hd;}
# 1350
;_pop_region(temp);}else{
# 1356
struct _tuple16 _tmp2CA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D8);union Cyc_CfFlowInfo_FlowInfo _tmp2CC;void*_tmp2CD;struct _tuple16 _tmp2CB=_tmp2CA;_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;
outflow=_tmp2CC;
place_val=_tmp2CD;}{
# 1360
union Cyc_CfFlowInfo_FlowInfo _tmp2CE=outflow;struct Cyc_Dict_Dict _tmp2CF;struct Cyc_List_List*_tmp2D0;_LL1B5: if((_tmp2CE.BottomFL).tag != 1)goto _LL1B7;_LL1B6: {
struct _tuple16 _tmp826;return(_tmp826.f1=outflow,((_tmp826.f2=rval,_tmp826)));}_LL1B7: if((_tmp2CE.ReachableFL).tag != 2)goto _LL1B4;_tmp2CF=((struct _tuple14)(_tmp2CE.ReachableFL).val).f1;_tmp2D0=((struct _tuple14)(_tmp2CE.ReachableFL).val).f2;_LL1B8: {
# 1363
struct _tuple16 _tmp827;return(_tmp827.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CF,root,place_val),_tmp2D0),((_tmp827.f2=rval,_tmp827)));}_LL1B4:;};};}_LL13C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D9->tag != 14)goto _LL13E;else{_tmp1DA=_tmp1D9->f1;}}_LL13D: {
# 1367
struct _tuple18 _tmp2D8=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1DA);union Cyc_CfFlowInfo_FlowInfo _tmp2DA;union Cyc_CfFlowInfo_AbsLVal _tmp2DB;struct _tuple18 _tmp2D9=_tmp2D8;_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2DC=_tmp2DB;struct Cyc_CfFlowInfo_Place*_tmp2DD;_LL1BA: if((_tmp2DC.UnknownL).tag != 2)goto _LL1BC;_LL1BB: {
struct _tuple16 _tmp828;return(_tmp828.f1=_tmp2DA,((_tmp828.f2=_tmp182->notzeroall,_tmp828)));}_LL1BC: if((_tmp2DC.PlaceL).tag != 1)goto _LL1B9;_tmp2DD=(struct Cyc_CfFlowInfo_Place*)(_tmp2DC.PlaceL).val;_LL1BD: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp82E;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp82D;struct _tuple16 _tmp82C;return(_tmp82C.f1=_tmp2DA,((_tmp82C.f2=(void*)((_tmp82E=_region_malloc(env->r,sizeof(*_tmp82E)),((_tmp82E[0]=((_tmp82D.tag=5,((_tmp82D.f1=_tmp2DD,_tmp82D)))),_tmp82E)))),_tmp82C)));}_LL1B9:;};}_LL13E: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1DB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1DB->tag != 19)goto _LL140;else{_tmp1DC=_tmp1DB->f1;}}_LL13F: {
# 1374
struct _tuple16 _tmp2E2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DC);union Cyc_CfFlowInfo_FlowInfo _tmp2E4;void*_tmp2E5;struct _tuple16 _tmp2E3=_tmp2E2;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E4,_tmp1DC,_tmp2E5);}_LL140: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1DD->tag != 20)goto _LL142;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}_LL141: {
# 1378
struct _tuple16 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DE);union Cyc_CfFlowInfo_FlowInfo _tmp2E8;void*_tmp2E9;struct _tuple16 _tmp2E7=_tmp2E6;_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;
if(_tmp1DE->topt == 0){
{const char*_tmp832;void*_tmp831[1];struct Cyc_String_pa_PrintArg_struct _tmp830;(_tmp830.tag=0,((_tmp830.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp831[0]=& _tmp830,Cyc_fprintf(Cyc_stderr,((_tmp832="aggrmember exp %s\n",_tag_dyneither(_tmp832,sizeof(char),19))),_tag_dyneither(_tmp831,sizeof(void*),1)))))));}{
const char*_tmp836;void*_tmp835[1];struct Cyc_String_pa_PrintArg_struct _tmp834;(_tmp834.tag=0,((_tmp834.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1DE)),((_tmp835[0]=& _tmp834,Cyc_fprintf(Cyc_stderr,((_tmp836="oops! %s.topt = null!\n",_tag_dyneither(_tmp836,sizeof(char),23))),_tag_dyneither(_tmp835,sizeof(void*),1)))))));};}{
# 1383
void*_tmp2F0=(void*)_check_null(_tmp1DE->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2F0)){
# 1386
struct _tuple16 _tmp837;return(_tmp837.f1=_tmp2E8,((_tmp837.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp837)));}
# 1388
if(Cyc_Absyn_is_require_union_type(_tmp2F0))
Cyc_NewControlFlow_check_requires(_tmp1DE->loc,_tmp182->r,_tmp2F0,_tmp1DF,_tmp2E8);{
# 1391
struct _tuple15 _tmp2F2=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp2E9);void*_tmp2F4;struct _tuple15 _tmp2F3=_tmp2F2;_tmp2F4=_tmp2F3.f1;{
void*_tmp2F5=_tmp2F4;int _tmp2F7;int _tmp2F8;struct _dyneither_ptr _tmp2F9;_LL1BF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F5;if(_tmp2F6->tag != 6)goto _LL1C1;else{_tmp2F7=(_tmp2F6->f1).is_union;_tmp2F8=(_tmp2F6->f1).fieldnum;_tmp2F9=_tmp2F6->f2;}}_LL1C0: {
# 1394
int _tmp2FA=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1DE->topt),_tmp1DF);
if((_tmp2F7  && _tmp2F8 != - 1) && _tmp2F8 != _tmp2FA){
struct _tuple16 _tmp838;return(_tmp838.f1=_tmp2E8,((_tmp838.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),1,_tmp182->unknown_none),_tmp838)));}{
struct _tuple16 _tmp839;return(_tmp839.f1=_tmp2E8,((_tmp839.f2=*((void**)_check_dyneither_subscript(_tmp2F9,sizeof(void*),_tmp2FA)),_tmp839)));};}_LL1C1:;_LL1C2:
# 1399
{const char*_tmp83C;void*_tmp83B;(_tmp83B=0,Cyc_fprintf(Cyc_stderr,((_tmp83C="the bad rexp is :",_tag_dyneither(_tmp83C,sizeof(char),18))),_tag_dyneither(_tmp83B,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2F4);
{const char*_tmp83F;void*_tmp83E;(_tmp83E=0,Cyc_fprintf(Cyc_stderr,((_tmp83F="\n",_tag_dyneither(_tmp83F,sizeof(char),2))),_tag_dyneither(_tmp83E,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp84C;const char*_tmp84B;void*_tmp84A[1];struct Cyc_String_pa_PrintArg_struct _tmp849;struct Cyc_Core_Impossible_exn_struct*_tmp848;(int)_throw((void*)((_tmp848=_cycalloc(sizeof(*_tmp848)),((_tmp848[0]=((_tmp84C.tag=Cyc_Core_Impossible,((_tmp84C.f1=(struct _dyneither_ptr)((_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp84A[0]=& _tmp849,Cyc_aprintf(((_tmp84B="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp84B,sizeof(char),26))),_tag_dyneither(_tmp84A,sizeof(void*),1)))))))),_tmp84C)))),_tmp848)))));};_LL1BE:;};};};}_LL142: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1E0=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E0->tag != 37)goto _LL144;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LL143: {
# 1409
struct _tuple16 _tmp306=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E1);union Cyc_CfFlowInfo_FlowInfo _tmp308;void*_tmp309;struct _tuple16 _tmp307=_tmp306;_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;
# 1411
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1E1->topt))){
struct _tuple16 _tmp84D;return(_tmp84D.f1=_tmp308,((_tmp84D.f2=_tmp182->unknown_all,_tmp84D)));}{
struct _tuple15 _tmp30B=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp309);void*_tmp30D;struct _tuple15 _tmp30C=_tmp30B;_tmp30D=_tmp30C.f1;{
void*_tmp30E=_tmp30D;int _tmp310;int _tmp311;struct _dyneither_ptr _tmp312;_LL1C4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30E;if(_tmp30F->tag != 6)goto _LL1C6;else{_tmp310=(_tmp30F->f1).is_union;_tmp311=(_tmp30F->f1).fieldnum;_tmp312=_tmp30F->f2;}}_LL1C5: {
# 1416
int _tmp313=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1E1->topt),_tmp1E2);
if(_tmp310  && _tmp311 != - 1){
if(_tmp311 != _tmp313){
struct _tuple16 _tmp84E;return(_tmp84E.f1=_tmp308,((_tmp84E.f2=_tmp182->zero,_tmp84E)));}else{
# 1421
struct _tuple16 _tmp84F;return(_tmp84F.f1=_tmp308,((_tmp84F.f2=_tmp182->notzeroall,_tmp84F)));}}else{
# 1423
struct _tuple16 _tmp850;return(_tmp850.f1=_tmp308,((_tmp850.f2=_tmp182->unknown_all,_tmp850)));}}_LL1C6:;_LL1C7: {
# 1425
struct Cyc_Core_Impossible_exn_struct _tmp85D;const char*_tmp85C;void*_tmp85B[1];struct Cyc_String_pa_PrintArg_struct _tmp85A;struct Cyc_Core_Impossible_exn_struct*_tmp859;(int)_throw((void*)((_tmp859=_cycalloc(sizeof(*_tmp859)),((_tmp859[0]=((_tmp85D.tag=Cyc_Core_Impossible,((_tmp85D.f1=(struct _dyneither_ptr)((_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp85B[0]=& _tmp85A,Cyc_aprintf(((_tmp85C="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp85C,sizeof(char),26))),_tag_dyneither(_tmp85B,sizeof(void*),1)))))))),_tmp85D)))),_tmp859)))));}_LL1C3:;};};}_LL144: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E3->tag != 21)goto _LL146;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}}_LL145: {
# 1430
struct _tuple16 _tmp31C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E4);union Cyc_CfFlowInfo_FlowInfo _tmp31E;void*_tmp31F;struct _tuple16 _tmp31D=_tmp31C;_tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;{
# 1433
struct _tuple16 _tmp320=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp31E,_tmp1E4,_tmp31F);union Cyc_CfFlowInfo_FlowInfo _tmp322;void*_tmp323;struct _tuple16 _tmp321=_tmp320;_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{
# 1436
void*_tmp324=Cyc_Tcutil_compress((void*)_check_null(_tmp1E4->topt));void*_tmp325=_tmp324;void*_tmp327;_LL1C9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 5)goto _LL1CB;else{_tmp327=(_tmp326->f1).elt_typ;}}_LL1CA:
# 1438
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp327)){
# 1440
struct _tuple16 _tmp85E;return(_tmp85E.f1=_tmp322,((_tmp85E.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp85E)));}
# 1442
if(Cyc_Absyn_is_require_union_type(_tmp327))
Cyc_NewControlFlow_check_requires(_tmp1E4->loc,_tmp182->r,_tmp327,_tmp1E5,_tmp31E);{
# 1445
struct _tuple15 _tmp329=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp323);void*_tmp32B;struct _tuple15 _tmp32A=_tmp329;_tmp32B=_tmp32A.f1;{
void*_tmp32C=_tmp32B;int _tmp32E;int _tmp32F;struct _dyneither_ptr _tmp330;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp32D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32C;if(_tmp32D->tag != 6)goto _LL1D0;else{_tmp32E=(_tmp32D->f1).is_union;_tmp32F=(_tmp32D->f1).fieldnum;_tmp330=_tmp32D->f2;}}_LL1CF: {
# 1448
int _tmp331=Cyc_CfFlowInfo_get_field_index(_tmp327,_tmp1E5);
if((_tmp32E  && _tmp32F != - 1) && _tmp32F != _tmp331){
struct _tuple16 _tmp85F;return(_tmp85F.f1=_tmp322,((_tmp85F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),1,_tmp182->unknown_none),_tmp85F)));}{
struct _tuple16 _tmp860;return(_tmp860.f1=_tmp322,((_tmp860.f2=*((void**)_check_dyneither_subscript(_tmp330,sizeof(void*),_tmp331)),_tmp860)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp866;const char*_tmp865;struct Cyc_Core_Impossible_exn_struct*_tmp864;(int)_throw((void*)((_tmp864=_cycalloc(sizeof(*_tmp864)),((_tmp864[0]=((_tmp866.tag=Cyc_Core_Impossible,((_tmp866.f1=((_tmp865="anal_Rexp: AggrArrow",_tag_dyneither(_tmp865,sizeof(char),21))),_tmp866)))),_tmp864)))));}_LL1CD:;};};_LL1CB:;_LL1CC: {
# 1454
struct Cyc_Core_Impossible_exn_struct _tmp86C;const char*_tmp86B;struct Cyc_Core_Impossible_exn_struct*_tmp86A;(int)_throw((void*)((_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86C.tag=Cyc_Core_Impossible,((_tmp86C.f1=((_tmp86B="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp86B,sizeof(char),25))),_tmp86C)))),_tmp86A)))));}_LL1C8:;};};}_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E6->tag != 5)goto _LL148;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;_tmp1E9=_tmp1E6->f3;}}_LL147: {
# 1458
struct _tuple19 _tmp33A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E7);union Cyc_CfFlowInfo_FlowInfo _tmp33C;union Cyc_CfFlowInfo_FlowInfo _tmp33D;struct _tuple19 _tmp33B=_tmp33A;_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;{
struct _tuple16 _tmp33E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33C,_tmp1E8);
struct _tuple16 _tmp33F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33D,_tmp1E9);
# 1462
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp33E,_tmp33F);};}_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1EA=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1EA->tag != 6)goto _LL14A;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;}}_LL149: {
# 1465
struct _tuple19 _tmp340=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp342;union Cyc_CfFlowInfo_FlowInfo _tmp343;struct _tuple19 _tmp341=_tmp340;_tmp342=_tmp341.f1;_tmp343=_tmp341.f2;{
struct _tuple16 _tmp344=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp342,_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp346;void*_tmp347;struct _tuple16 _tmp345=_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{
struct _tuple16 _tmp86D;struct _tuple16 _tmp348=(_tmp86D.f1=_tmp346,((_tmp86D.f2=_tmp347,_tmp86D)));
struct _tuple16 _tmp86E;struct _tuple16 _tmp349=(_tmp86E.f1=_tmp343,((_tmp86E.f2=_tmp182->zero,_tmp86E)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp348,_tmp349);};};}_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1ED->tag != 7)goto _LL14C;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;}}_LL14B: {
# 1472
struct _tuple19 _tmp34C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EE);union Cyc_CfFlowInfo_FlowInfo _tmp34E;union Cyc_CfFlowInfo_FlowInfo _tmp34F;struct _tuple19 _tmp34D=_tmp34C;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{
struct _tuple16 _tmp350=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp34F,_tmp1EF);union Cyc_CfFlowInfo_FlowInfo _tmp352;void*_tmp353;struct _tuple16 _tmp351=_tmp350;_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;{
struct _tuple16 _tmp86F;struct _tuple16 _tmp354=(_tmp86F.f1=_tmp352,((_tmp86F.f2=_tmp353,_tmp86F)));
struct _tuple16 _tmp870;struct _tuple16 _tmp355=(_tmp870.f1=_tmp34E,((_tmp870.f2=_tmp182->notzeroall,_tmp870)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp354,_tmp355);};};}_LL14C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1F0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F0->tag != 22)goto _LL14E;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;}}_LL14D: {
# 1479
struct _RegionHandle _tmp358=_new_region("temp");struct _RegionHandle*temp=& _tmp358;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp871[2];struct _tuple23 _tmp359=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp871[1]=_tmp1F2,((_tmp871[0]=_tmp1F1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp871,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1480
union Cyc_CfFlowInfo_FlowInfo _tmp35B;struct Cyc_List_List*_tmp35C;struct _tuple23 _tmp35A=_tmp359;_tmp35B=_tmp35A.f1;_tmp35C=_tmp35A.f2;{
# 1484
union Cyc_CfFlowInfo_FlowInfo _tmp35D=_tmp35B;
{union Cyc_CfFlowInfo_FlowInfo _tmp35E=_tmp35B;struct Cyc_Dict_Dict _tmp35F;struct Cyc_List_List*_tmp360;_LL1D3: if((_tmp35E.ReachableFL).tag != 2)goto _LL1D5;_tmp35F=((struct _tuple14)(_tmp35E.ReachableFL).val).f1;_tmp360=((struct _tuple14)(_tmp35E.ReachableFL).val).f2;_LL1D4:
# 1489
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp35F,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp35C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp874;void*_tmp873;(_tmp873=0,Cyc_CfFlowInfo_aerr(_tmp1F2->loc,((_tmp874="expression may not be initialized",_tag_dyneither(_tmp874,sizeof(char),34))),_tag_dyneither(_tmp873,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp363=Cyc_NewControlFlow_add_subscript_reln(_tmp182->r,_tmp360,_tmp1F1,_tmp1F2);
if(_tmp360 != _tmp363)
_tmp35D=Cyc_CfFlowInfo_ReachableFL(_tmp35F,_tmp363);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1497
void*_tmp364=Cyc_Tcutil_compress((void*)_check_null(_tmp1F1->topt));void*_tmp365=_tmp364;struct Cyc_List_List*_tmp367;union Cyc_Absyn_Constraint*_tmp369;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp365;if(_tmp366->tag != 10)goto _LL1DA;else{_tmp367=_tmp366->f1;}}_LL1D9: {
# 1499
struct _tuple15 _tmp36A=Cyc_CfFlowInfo_unname_rval(_tmp182->r,(void*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd);void*_tmp36C;struct _tuple15 _tmp36B=_tmp36A;_tmp36C=_tmp36B.f1;{
void*_tmp36D=_tmp36C;struct _dyneither_ptr _tmp36F;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp36E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp36E->tag != 6)goto _LL1E1;else{_tmp36F=_tmp36E->f2;}}_LL1E0: {
# 1502
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1F2)).f1;
struct _tuple16 _tmp875;struct _tuple16 _tmp371=(_tmp875.f1=_tmp35D,((_tmp875.f2=*((void**)_check_dyneither_subscript(_tmp36F,sizeof(void*),(int)i)),_tmp875)));_npop_handler(0);return _tmp371;}_LL1E1:;_LL1E2: {
struct Cyc_Core_Impossible_exn_struct _tmp87B;const char*_tmp87A;struct Cyc_Core_Impossible_exn_struct*_tmp879;(int)_throw((void*)((_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879[0]=((_tmp87B.tag=Cyc_Core_Impossible,((_tmp87B.f1=((_tmp87A="anal_Rexp: Subscript",_tag_dyneither(_tmp87A,sizeof(char),21))),_tmp87B)))),_tmp879)))));}_LL1DE:;};}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365;if(_tmp368->tag != 5)goto _LL1DC;else{_tmp369=((_tmp368->f1).ptr_atts).bounds;}}_LL1DB: {
# 1507
struct _tuple16 _tmp375=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp35B,_tmp1F1,(void*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp377;void*_tmp378;struct _tuple16 _tmp376=_tmp375;_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp379=_tmp377;_LL1E4: if((_tmp379.BottomFL).tag != 1)goto _LL1E6;_LL1E5: {
struct _tuple16 _tmp87C;struct _tuple16 _tmp37B=(_tmp87C.f1=_tmp377,((_tmp87C.f2=_tmp378,_tmp87C)));_npop_handler(0);return _tmp37B;}_LL1E6:;_LL1E7: {
struct _tuple16 _tmp87D;struct _tuple16 _tmp37D=(_tmp87D.f1=_tmp35D,((_tmp87D.f2=_tmp378,_tmp87D)));_npop_handler(0);return _tmp37D;}_LL1E3:;};}_LL1DC:;_LL1DD: {
# 1512
struct Cyc_Core_Impossible_exn_struct _tmp883;const char*_tmp882;struct Cyc_Core_Impossible_exn_struct*_tmp881;(int)_throw((void*)((_tmp881=_cycalloc(sizeof(*_tmp881)),((_tmp881[0]=((_tmp883.tag=Cyc_Core_Impossible,((_tmp883.f1=((_tmp882="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp882,sizeof(char),33))),_tmp883)))),_tmp881)))));}_LL1D7:;};};}
# 1480
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F3->tag != 30)goto _LL150;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F3->f2;}}_LL14F:
# 1517
 _tmp1F7=_tmp1F4;goto _LL151;_LL150: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1F6=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F6->tag != 23)goto _LL152;else{_tmp1F7=_tmp1F6->f1;}}_LL151: {
# 1519
struct _RegionHandle _tmp382=_new_region("temp");struct _RegionHandle*temp=& _tmp382;_push_region(temp);
{struct _tuple23 _tmp383=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1F7,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp385;struct Cyc_List_List*_tmp386;struct _tuple23 _tmp384=_tmp383;_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;{
unsigned int _tmp889;unsigned int _tmp888;struct _dyneither_ptr _tmp887;void**_tmp886;unsigned int _tmp885;struct _dyneither_ptr aggrdict=
(_tmp885=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1F7),((_tmp886=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp885)),((_tmp887=_tag_dyneither(_tmp886,sizeof(void*),_tmp885),((((_tmp888=_tmp885,_tmp88A(& _tmp889,& _tmp888,& _tmp886,& _tmp386))),_tmp887)))))));
# 1527
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp894;struct Cyc_CfFlowInfo_UnionRInfo _tmp893;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp892;struct _tuple16 _tmp891;struct _tuple16 _tmp38B=(_tmp891.f1=_tmp385,((_tmp891.f2=(void*)((_tmp894=_region_malloc(env->r,sizeof(*_tmp894)),((_tmp894[0]=((_tmp892.tag=6,((_tmp892.f1=((_tmp893.is_union=0,((_tmp893.fieldnum=- 1,_tmp893)))),((_tmp892.f2=aggrdict,_tmp892)))))),_tmp894)))),_tmp891)));_npop_handler(0);return _tmp38B;};}
# 1520
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1F8=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F8->tag != 29)goto _LL154;else{_tmp1F9=_tmp1F8->f2;}}_LL153: {
# 1530
struct Cyc_List_List*fs;
{void*_tmp391=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp392=_tmp391;struct Cyc_List_List*_tmp394;_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 12)goto _LL1EB;else{_tmp394=_tmp393->f2;}}_LL1EA:
 fs=_tmp394;goto _LL1E8;_LL1EB:;_LL1EC: {
struct Cyc_Core_Impossible_exn_struct _tmp89A;const char*_tmp899;struct Cyc_Core_Impossible_exn_struct*_tmp898;(int)_throw((void*)((_tmp898=_cycalloc(sizeof(*_tmp898)),((_tmp898[0]=((_tmp89A.tag=Cyc_Core_Impossible,((_tmp89A.f1=((_tmp899="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp899,sizeof(char),35))),_tmp89A)))),_tmp898)))));}_LL1E8:;}
# 1535
_tmp1FB=_tmp1F9;_tmp1FC=Cyc_Absyn_StructA;_tmp1FD=fs;goto _LL155;}_LL154: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FA->tag != 28)goto _LL156;else{_tmp1FB=_tmp1FA->f3;if(_tmp1FA->f4 == 0)goto _LL156;_tmp1FC=(_tmp1FA->f4)->kind;if((_tmp1FA->f4)->impl == 0)goto _LL156;_tmp1FD=((_tmp1FA->f4)->impl)->fields;}}_LL155: {
# 1537
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp398=_new_region("temp");struct _RegionHandle*temp=& _tmp398;_push_region(temp);
{struct _tuple23 _tmp399=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1FB),1,1);
# 1539
union Cyc_CfFlowInfo_FlowInfo _tmp39B;struct Cyc_List_List*_tmp39C;struct _tuple23 _tmp39A=_tmp399;_tmp39B=_tmp39A.f1;_tmp39C=_tmp39A.f2;{
# 1541
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp182,_tmp1FD,_tmp1FC == Cyc_Absyn_UnionA,_tmp182->unknown_all);
# 1544
int field_no=-1;
{int i=0;for(0;_tmp39C != 0;(((_tmp39C=_tmp39C->tl,_tmp1FB=_tmp1FB->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp1FB))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp39D=(void*)ds->hd;void*_tmp39E=_tmp39D;struct _dyneither_ptr*_tmp3A1;_LL1EE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp39F=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp39E;if(_tmp39F->tag != 0)goto _LL1F0;}_LL1EF: {
struct Cyc_Core_Impossible_exn_struct _tmp8A0;const char*_tmp89F;struct Cyc_Core_Impossible_exn_struct*_tmp89E;(int)_throw((void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp8A0.tag=Cyc_Core_Impossible,((_tmp8A0.f1=((_tmp89F="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp89F,sizeof(char),22))),_tmp8A0)))),_tmp89E)))));}_LL1F0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp39E;if(_tmp3A0->tag != 1)goto _LL1ED;else{_tmp3A1=_tmp3A0->f1;}}_LL1F1:
# 1551
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1FD,_tmp3A1);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp39C->hd;
# 1554
if(_tmp1FC == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp3A5=(*((struct _tuple26*)_tmp1FB->hd)).f2;
_tmp39B=Cyc_NewControlFlow_use_Rval(env,_tmp3A5->loc,_tmp39B,(void*)_tmp39C->hd);
# 1558
Cyc_NewControlFlow_check_requires(_tmp3A5->loc,_tmp182->r,exp_type,_tmp3A1,_tmp39B);}_LL1ED:;}}}{
# 1561
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8A6;struct Cyc_CfFlowInfo_UnionRInfo _tmp8A5;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8A4;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3A6=(_tmp8A4=_region_malloc(env->r,sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A6.tag=6,((_tmp8A6.f1=((_tmp8A5.is_union=_tmp1FC == Cyc_Absyn_UnionA,((_tmp8A5.fieldnum=field_no,_tmp8A5)))),((_tmp8A6.f2=aggrdict,_tmp8A6)))))),_tmp8A4)));
struct _tuple16 _tmp8A7;struct _tuple16 _tmp3A8=(_tmp8A7.f1=_tmp39B,((_tmp8A7.f2=(void*)_tmp3A6,_tmp8A7)));_npop_handler(0);return _tmp3A8;};};}
# 1539
;_pop_region(temp);}_LL156: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FE=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FE->tag != 28)goto _LL158;}_LL157: {
# 1565
struct Cyc_Core_Impossible_exn_struct _tmp8AD;const char*_tmp8AC;struct Cyc_Core_Impossible_exn_struct*_tmp8AB;(int)_throw((void*)((_tmp8AB=_cycalloc(sizeof(*_tmp8AB)),((_tmp8AB[0]=((_tmp8AD.tag=Cyc_Core_Impossible,((_tmp8AD.f1=((_tmp8AC="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8AC,sizeof(char),31))),_tmp8AD)))),_tmp8AB)))));}_LL158: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1FF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FF->tag != 25)goto _LL15A;else{_tmp200=_tmp1FF->f1;}}_LL159: {
# 1567
struct _RegionHandle _tmp3AF=_new_region("temp");struct _RegionHandle*temp=& _tmp3AF;_push_region(temp);
{struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp200);
struct _tuple23 _tmp3B1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3B0,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp3B3;struct Cyc_List_List*_tmp3B4;struct _tuple23 _tmp3B2=_tmp3B1;_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;
for(0;_tmp3B4 != 0;(_tmp3B4=_tmp3B4->tl,_tmp3B0=_tmp3B0->tl)){
_tmp3B3=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3B0))->hd)->loc,_tmp3B3,(void*)_tmp3B4->hd);}{
struct _tuple16 _tmp8AE;struct _tuple16 _tmp3B6=(_tmp8AE.f1=_tmp3B3,((_tmp8AE.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp8AE)));_npop_handler(0);return _tmp3B6;};}
# 1568
;_pop_region(temp);}_LL15A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp201=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp201->tag != 26)goto _LL15C;else{_tmp202=_tmp201->f1;_tmp203=_tmp201->f2;_tmp204=_tmp201->f3;_tmp205=_tmp201->f4;}}_LL15B: {
# 1576
struct _tuple16 _tmp3B7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp203);union Cyc_CfFlowInfo_FlowInfo _tmp3B9;void*_tmp3BA;struct _tuple16 _tmp3B8=_tmp3B7;_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3BB=_tmp3B9;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_List_List*_tmp3BD;_LL1F3: if((_tmp3BB.BottomFL).tag != 1)goto _LL1F5;_LL1F4: {
struct _tuple16 _tmp8AF;return(_tmp8AF.f1=_tmp3B9,((_tmp8AF.f2=_tmp182->unknown_all,_tmp8AF)));}_LL1F5: if((_tmp3BB.ReachableFL).tag != 2)goto _LL1F2;_tmp3BC=((struct _tuple14)(_tmp3BB.ReachableFL).val).f1;_tmp3BD=((struct _tuple14)(_tmp3BB.ReachableFL).val).f2;_LL1F6:
# 1580
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BC,_tmp3BA)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8B2;void*_tmp8B1;(_tmp8B1=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8B2="expression may not be initialized",_tag_dyneither(_tmp8B2,sizeof(char),34))),_tag_dyneither(_tmp8B1,sizeof(void*),0)));}{
# 1584
struct Cyc_List_List*new_relns=_tmp3BD;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp202);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp203,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3BD);
# 1590
if(_tmp3BD != new_relns)
_tmp3B9=Cyc_CfFlowInfo_ReachableFL(_tmp3BC,new_relns);{
# 1594
void*_tmp3C1=_tmp3BA;_LL1F8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3C2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C2->tag != 0)goto _LL1FA;}_LL1F9: {
struct _tuple16 _tmp8B3;return(_tmp8B3.f1=_tmp3B9,((_tmp8B3.f2=_tmp182->unknown_all,_tmp8B3)));}_LL1FA: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3C3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C3->tag != 2)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3C4=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C4->tag != 1)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3C5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C5->tag != 5)goto _LL200;}_LL1FF: {
# 1599
struct Cyc_List_List _tmp8B4;struct Cyc_List_List _tmp3C7=(_tmp8B4.hd=_tmp202,((_tmp8B4.tl=0,_tmp8B4)));
_tmp3B9=Cyc_NewControlFlow_add_vars(_tmp182,_tmp3B9,& _tmp3C7,_tmp182->unknown_all,e->loc,0);{
# 1602
struct _tuple16 _tmp3C8=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B9,_tmp204);union Cyc_CfFlowInfo_FlowInfo _tmp3CA;void*_tmp3CB;struct _tuple16 _tmp3C9=_tmp3C8;_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3CC=_tmp3CA;struct Cyc_Dict_Dict _tmp3CD;_LL203: if((_tmp3CC.BottomFL).tag != 1)goto _LL205;_LL204: {
struct _tuple16 _tmp8B5;return(_tmp8B5.f1=_tmp3CA,((_tmp8B5.f2=_tmp182->unknown_all,_tmp8B5)));}_LL205: if((_tmp3CC.ReachableFL).tag != 2)goto _LL202;_tmp3CD=((struct _tuple14)(_tmp3CC.ReachableFL).val).f1;_LL206:
# 1606
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp3CD,_tmp3CB)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8B8;void*_tmp8B7;(_tmp8B7=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8B8="expression may not be initialized",_tag_dyneither(_tmp8B8,sizeof(char),34))),_tag_dyneither(_tmp8B7,sizeof(void*),0)));}{
struct _tuple16 _tmp8B9;return(_tmp8B9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8B9.f2=_tmp182->unknown_all,_tmp8B9)));};}_LL202:;}
# 1611
_tmp3B9=_tmp3CA;
goto _LL201;};}_LL200:;_LL201:
# 1614
 while(1){
struct Cyc_List_List _tmp8BA;struct Cyc_List_List _tmp3D3=(_tmp8BA.hd=_tmp202,((_tmp8BA.tl=0,_tmp8BA)));
_tmp3B9=Cyc_NewControlFlow_add_vars(_tmp182,_tmp3B9,& _tmp3D3,_tmp182->unknown_all,e->loc,0);{
struct _tuple16 _tmp3D4=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B9,_tmp204);union Cyc_CfFlowInfo_FlowInfo _tmp3D6;void*_tmp3D7;struct _tuple16 _tmp3D5=_tmp3D4;_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3D8=_tmp3D6;struct Cyc_Dict_Dict _tmp3D9;_LL208: if((_tmp3D8.BottomFL).tag != 1)goto _LL20A;_LL209:
 goto _LL207;_LL20A: if((_tmp3D8.ReachableFL).tag != 2)goto _LL207;_tmp3D9=((struct _tuple14)(_tmp3D8.ReachableFL).val).f1;_LL20B:
# 1621
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp3D9,_tmp3D7)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8BD;void*_tmp8BC;(_tmp8BC=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8BD="expression may not be initialized",_tag_dyneither(_tmp8BD,sizeof(char),34))),_tag_dyneither(_tmp8BC,sizeof(void*),0)));}{
struct _tuple16 _tmp8BE;return(_tmp8BE.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8BE.f2=_tmp182->unknown_all,_tmp8BE)));};}_LL207:;}{
# 1626
union Cyc_CfFlowInfo_FlowInfo _tmp3DD=Cyc_CfFlowInfo_join_flow(_tmp182,env->all_changed,_tmp3B9,_tmp3D6);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3DD,_tmp3B9))
break;
_tmp3B9=_tmp3DD;};};}{
# 1631
struct _tuple16 _tmp8BF;return(_tmp8BF.f1=_tmp3B9,((_tmp8BF.f2=_tmp182->unknown_all,_tmp8BF)));};_LL1F7:;};};_LL1F2:;};}_LL15C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp206=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp206->tag != 27)goto _LL15E;else{_tmp207=_tmp206->f1;_tmp208=(void*)_tmp206->f2;_tmp209=_tmp206->f3;}}_LL15D: {
# 1637
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8C2;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8C1;void*root=(void*)((_tmp8C1=_region_malloc(_tmp182->r,sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C2.tag=1,((_tmp8C2.f1=_tmp207,((_tmp8C2.f2=(void*)_check_null(e->topt),_tmp8C2)))))),_tmp8C1))));
struct Cyc_CfFlowInfo_Place*_tmp8C3;struct Cyc_CfFlowInfo_Place*place=(_tmp8C3=_region_malloc(_tmp182->r,sizeof(*_tmp8C3)),((_tmp8C3->root=root,((_tmp8C3->fields=0,_tmp8C3)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8C6;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8C5;void*rval=(void*)((_tmp8C5=_region_malloc(_tmp182->r,sizeof(*_tmp8C5)),((_tmp8C5[0]=((_tmp8C6.tag=5,((_tmp8C6.f1=place,_tmp8C6)))),_tmp8C5))));
void*place_val;
# 1645
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,_tmp208,0,_tmp182->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp3E0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp207);union Cyc_CfFlowInfo_FlowInfo _tmp3E2;struct _tuple16 _tmp3E1=_tmp3E0;_tmp3E2=_tmp3E1.f1;
outflow=_tmp3E2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E3=outflow;struct Cyc_Dict_Dict _tmp3E4;struct Cyc_List_List*_tmp3E5;_LL20D: if((_tmp3E3.BottomFL).tag != 1)goto _LL20F;_LL20E: {
struct _tuple16 _tmp8C7;return(_tmp8C7.f1=outflow,((_tmp8C7.f2=rval,_tmp8C7)));}_LL20F: if((_tmp3E3.ReachableFL).tag != 2)goto _LL20C;_tmp3E4=((struct _tuple14)(_tmp3E3.ReachableFL).val).f1;_tmp3E5=((struct _tuple14)(_tmp3E3.ReachableFL).val).f2;_LL210: {
# 1653
struct _tuple16 _tmp8C8;return(_tmp8C8.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E4,root,place_val),_tmp3E5),((_tmp8C8.f2=rval,_tmp8C8)));}_LL20C:;};};};}_LL15E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp20A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20A->tag != 36)goto _LL160;else{_tmp20B=_tmp20A->f1;}}_LL15F: {
# 1657
struct _tuple17 _tmp8C9;struct _tuple17 _tmp3ED=(_tmp8C9.f1=(env->fenv)->unknown_all,((_tmp8C9.f2=copy_ctxt,_tmp8C9)));
union Cyc_CfFlowInfo_FlowInfo _tmp3EE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp20B,& _tmp3ED);
struct _tuple16 _tmp8CA;return(_tmp8CA.f1=_tmp3EE,((_tmp8CA.f2=_tmp3ED.f1,_tmp8CA)));}_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp20C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20C->tag != 1)goto _LL162;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp20D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp20C->f2);if(_tmp20D->tag != 0)goto _LL162;}}_LL161:
# 1661
 goto _LL163;_LL162: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp20E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20E->tag != 35)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp20F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20F->tag != 24)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp210=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp210->tag != 38)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp211=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp211->tag != 39)goto _LL101;}_LL169: {
# 1666
struct Cyc_Core_Impossible_exn_struct _tmp8D0;const char*_tmp8CF;struct Cyc_Core_Impossible_exn_struct*_tmp8CE;(int)_throw((void*)((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8D0.tag=Cyc_Core_Impossible,((_tmp8D0.f1=((_tmp8CF="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8CF,sizeof(char),31))),_tmp8D0)))),_tmp8CE)))));}_LL101:;};}
# 1676
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1685
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F4=env->fenv;
void*_tmp3F5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3F6=_tmp3F5;void*_tmp3F8;union Cyc_Absyn_Constraint*_tmp3F9;union Cyc_Absyn_Constraint*_tmp3FA;_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6;if(_tmp3F7->tag != 5)goto _LL214;else{_tmp3F8=(_tmp3F7->f1).elt_typ;_tmp3F9=((_tmp3F7->f1).ptr_atts).bounds;_tmp3FA=((_tmp3F7->f1).ptr_atts).zero_term;}}_LL213: {
# 1688
union Cyc_CfFlowInfo_FlowInfo _tmp3FB=f;struct Cyc_Dict_Dict _tmp3FC;struct Cyc_List_List*_tmp3FD;_LL217: if((_tmp3FB.BottomFL).tag != 1)goto _LL219;_LL218: {
struct _tuple18 _tmp8D1;return(_tmp8D1.f1=f,((_tmp8D1.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8D1)));}_LL219: if((_tmp3FB.ReachableFL).tag != 2)goto _LL216;_tmp3FC=((struct _tuple14)(_tmp3FB.ReachableFL).val).f1;_tmp3FD=((struct _tuple14)(_tmp3FB.ReachableFL).val).f2;_LL21A:
# 1692
 if(Cyc_Tcutil_is_bound_one(_tmp3F9)){
struct _tuple15 _tmp3FF=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp401;struct Cyc_List_List*_tmp402;struct _tuple15 _tmp400=_tmp3FF;_tmp401=_tmp400.f1;_tmp402=_tmp400.f2;
retry: {void*_tmp403=_tmp401;void*_tmp408;struct Cyc_List_List*_tmp409;enum Cyc_CfFlowInfo_InitLevel _tmp40C;_LL21C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp404=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp404->tag != 8)goto _LL21E;}_LL21D: {
# 1696
struct Cyc_Core_Impossible_exn_struct _tmp8D7;const char*_tmp8D6;struct Cyc_Core_Impossible_exn_struct*_tmp8D5;(int)_throw((void*)((_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5[0]=((_tmp8D7.tag=Cyc_Core_Impossible,((_tmp8D7.f1=((_tmp8D6="named location in anal_derefL",_tag_dyneither(_tmp8D6,sizeof(char),30))),_tmp8D7)))),_tmp8D5)))));}_LL21E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp405=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp405->tag != 1)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp406=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp406->tag != 2)goto _LL222;}_LL221:
# 1699
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8DA;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8D9;e->annot=(void*)((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=Cyc_CfFlowInfo_NotZero,((_tmp8DA.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8DA)))),_tmp8D9))));}goto _LL21B;_LL222: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp407=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp407->tag != 5)goto _LL224;else{_tmp408=(_tmp407->f1)->root;_tmp409=(_tmp407->f1)->fields;}}_LL223:
# 1702
 if(expanded_place){
# 1705
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8DD;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8DC;e->annot=(void*)((_tmp8DC=_cycalloc(sizeof(*_tmp8DC)),((_tmp8DC[0]=((_tmp8DD.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8DD.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8DD)))),_tmp8DC))));}else{
# 1708
void*_tmp414=e->annot;void*_tmp415=_tmp414;_LL22B: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp416=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp415;if(_tmp416->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL22D;}_LL22C:
# 1712
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8E0;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8DF;e->annot=(void*)((_tmp8DF=_cycalloc(sizeof(*_tmp8DF)),((_tmp8DF[0]=((_tmp8E0.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8E0.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8E0)))),_tmp8DF))));}
goto _LL22A;_LL22D:;_LL22E:
# 1715
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp8E3;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp8E2;e->annot=(void*)((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E3.tag=Cyc_CfFlowInfo_NotZero,((_tmp8E3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8E3)))),_tmp8E2))));}
goto _LL22A;_LL22A:;}{
# 1719
struct Cyc_CfFlowInfo_Place*_tmp8E6;struct _tuple18 _tmp8E5;return(_tmp8E5.f1=f,((_tmp8E5.f2=Cyc_CfFlowInfo_PlaceL(((_tmp8E6=_region_malloc(_tmp3F4->r,sizeof(*_tmp8E6)),((_tmp8E6->root=_tmp408,((_tmp8E6->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3F4->r,_tmp409,flds),_tmp8E6))))))),_tmp8E5)));};_LL224: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp40A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp40A->tag != 0)goto _LL226;}_LL225:
# 1721
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp8E7;return(_tmp8E7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E7.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8E7)));};_LL226: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp40B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp40B->tag != 3)goto _LL228;else{_tmp40C=_tmp40B->f1;}}_LL227:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp40C,_tmp402);goto _LL229;_LL228:;_LL229:
# 1726
 if(passthrough_consumes){
void*_tmp41E=_tmp401;void*_tmp420;_LL230: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp41F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp41E;if(_tmp41F->tag != 7)goto _LL232;else{_tmp420=(void*)_tmp41F->f3;}}_LL231:
 _tmp401=_tmp420;goto retry;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 1732
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8EA;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8E9;e->annot=(void*)((_tmp8E9=_cycalloc(sizeof(*_tmp8E9)),((_tmp8E9[0]=((_tmp8EA.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8EA.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8EA)))),_tmp8E9))));};_LL21B:;}}else{
# 1736
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp8ED;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp8EC;e->annot=(void*)((_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp8ED.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp8ED)))),_tmp8EC))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3F4,_tmp3FC,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp425=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp427;struct _tuple15 _tmp426=_tmp425;_tmp427=_tmp426.f1;{
void*_tmp428=_tmp427;_LL235: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp429=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp428;if(_tmp429->tag != 7)goto _LL237;}_LL236:
# 1741
{const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8F0="attempt to dereference an alias-free that has already been copied",_tag_dyneither(_tmp8F0,sizeof(char),66))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
goto _LL234;_LL237:;_LL238:
# 1744
{const char*_tmp8F3;void*_tmp8F2;(_tmp8F2=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp8F3="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp8F3,sizeof(char),46))),_tag_dyneither(_tmp8F2,sizeof(void*),0)));}
goto _LL234;_LL234:;};}{
# 1748
struct _tuple18 _tmp8F4;return(_tmp8F4.f1=f,((_tmp8F4.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8F4)));};_LL216:;}_LL214:;_LL215: {
# 1750
struct Cyc_Core_Impossible_exn_struct _tmp8FA;const char*_tmp8F9;struct Cyc_Core_Impossible_exn_struct*_tmp8F8;(int)_throw((void*)((_tmp8F8=_cycalloc(sizeof(*_tmp8F8)),((_tmp8F8[0]=((_tmp8FA.tag=Cyc_Core_Impossible,((_tmp8FA.f1=((_tmp8F9="left deref of non-pointer-type",_tag_dyneither(_tmp8F9,sizeof(char),31))),_tmp8FA)))),_tmp8F8)))));}_LL211:;}
# 1758
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1761
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp432=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp433=inflow;struct Cyc_Dict_Dict _tmp434;struct Cyc_List_List*_tmp435;_LL23A: if((_tmp433.BottomFL).tag != 1)goto _LL23C;_LL23B: {
struct _tuple18 _tmp8FB;return(_tmp8FB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8FB.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8FB)));}_LL23C: if((_tmp433.ReachableFL).tag != 2)goto _LL239;_tmp434=((struct _tuple14)(_tmp433.ReachableFL).val).f1;_tmp435=((struct _tuple14)(_tmp433.ReachableFL).val).f2;_LL23D:
# 1766
 d=_tmp434;_LL239:;}{
# 1770
void*_tmp437=e->r;void*_tmp438=_tmp437;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Vardecl*_tmp441;struct Cyc_Absyn_Vardecl*_tmp444;struct Cyc_Absyn_Vardecl*_tmp447;struct Cyc_Absyn_Vardecl*_tmp44A;struct Cyc_Absyn_Exp*_tmp44C;struct _dyneither_ptr*_tmp44D;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Exp*_tmp454;struct _dyneither_ptr*_tmp455;_LL23F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp439->tag != 13)goto _LL241;else{_tmp43A=_tmp439->f2;}}_LL240:
 _tmp43C=_tmp43A;goto _LL242;_LL241: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp43B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43B->tag != 11)goto _LL243;else{_tmp43C=_tmp43B->f1;}}_LL242:
 _tmp43E=_tmp43C;goto _LL244;_LL243: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp43D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43D->tag != 12)goto _LL245;else{_tmp43E=_tmp43D->f1;}}_LL244:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp43E,flds);_LL245: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp43F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43F->tag != 1)goto _LL247;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp440=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp43F->f2);if(_tmp440->tag != 3)goto _LL247;else{_tmp441=_tmp440->f1;}}}_LL246:
# 1775
 _tmp444=_tmp441;goto _LL248;_LL247: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp442=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp442->tag != 1)goto _LL249;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp443=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp442->f2);if(_tmp443->tag != 4)goto _LL249;else{_tmp444=_tmp443->f1;}}}_LL248:
 _tmp447=_tmp444;goto _LL24A;_LL249: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp445=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp445->tag != 1)goto _LL24B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp446=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp445->f2);if(_tmp446->tag != 5)goto _LL24B;else{_tmp447=_tmp446->f1;}}}_LL24A: {
# 1778
struct Cyc_CfFlowInfo_Place*_tmp905;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp904;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp903;struct _tuple18 _tmp902;return(_tmp902.f1=inflow,((_tmp902.f2=Cyc_CfFlowInfo_PlaceL(((_tmp905=_region_malloc(env->r,sizeof(*_tmp905)),((_tmp905->root=(void*)((_tmp903=_region_malloc(env->r,sizeof(*_tmp903)),((_tmp903[0]=((_tmp904.tag=0,((_tmp904.f1=_tmp447,_tmp904)))),_tmp903)))),((_tmp905->fields=flds,_tmp905))))))),_tmp902)));}_LL24B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp448=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp448->tag != 1)goto _LL24D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp449=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp448->f2);if(_tmp449->tag != 1)goto _LL24D;else{_tmp44A=_tmp449->f1;}}}_LL24C: {
# 1780
struct _tuple18 _tmp906;return(_tmp906.f1=inflow,((_tmp906.f2=Cyc_CfFlowInfo_UnknownL(),_tmp906)));}_LL24D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp44B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp44B->tag != 21)goto _LL24F;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}}_LL24E:
# 1783
{void*_tmp45B=Cyc_Tcutil_compress((void*)_check_null(_tmp44C->topt));void*_tmp45C=_tmp45B;void*_tmp45E;_LL258: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45C;if(_tmp45D->tag != 5)goto _LL25A;else{_tmp45E=(_tmp45D->f1).elt_typ;}}_LL259:
# 1785
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp45E)){
Cyc_NewControlFlow_check_requires(_tmp44C->loc,_tmp432->r,_tmp45E,_tmp44D,inflow);{
struct Cyc_List_List*_tmp907;flds=((_tmp907=_region_malloc(env->r,sizeof(*_tmp907)),((_tmp907->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp45E,_tmp44D),((_tmp907->tl=flds,_tmp907))))));};}
# 1789
goto _LL257;_LL25A:;_LL25B: {
struct Cyc_Core_Impossible_exn_struct _tmp90D;const char*_tmp90C;struct Cyc_Core_Impossible_exn_struct*_tmp90B;(int)_throw((void*)((_tmp90B=_cycalloc(sizeof(*_tmp90B)),((_tmp90B[0]=((_tmp90D.tag=Cyc_Core_Impossible,((_tmp90D.f1=((_tmp90C="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp90C,sizeof(char),25))),_tmp90D)))),_tmp90B)))));}_LL257:;}
# 1792
_tmp44F=_tmp44C;goto _LL250;_LL24F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp44E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp44E->tag != 19)goto _LL251;else{_tmp44F=_tmp44E->f1;}}_LL250:
# 1795
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp44F->topt),1)){
# 1797
struct _tuple18 _tmp463=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp44F->topt),1),passthrough_consumes,_tmp44F);
# 1797
union Cyc_CfFlowInfo_FlowInfo _tmp465;union Cyc_CfFlowInfo_AbsLVal _tmp466;struct _tuple18 _tmp464=_tmp463;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;{
# 1801
struct _tuple18 _tmp90E;struct _tuple18 _tmp467=(_tmp90E.f1=_tmp465,((_tmp90E.f2=_tmp466,_tmp90E)));struct _tuple18 _tmp468=_tmp467;struct Cyc_Dict_Dict _tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_CfFlowInfo_Place*_tmp46B;_LL25D: if(((_tmp468.f1).ReachableFL).tag != 2)goto _LL25F;_tmp469=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f1;_tmp46A=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f2;if(((_tmp468.f2).PlaceL).tag != 1)goto _LL25F;_tmp46B=(struct Cyc_CfFlowInfo_Place*)((_tmp468.f2).PlaceL).val;_LL25E: {
# 1803
void*_tmp46C=Cyc_CfFlowInfo_lookup_place(_tmp469,_tmp46B);
struct _tuple15 _tmp46D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp46C);void*_tmp46F;struct Cyc_List_List*_tmp470;struct _tuple15 _tmp46E=_tmp46D;_tmp46F=_tmp46E.f1;_tmp470=_tmp46E.f2;{
void*_tmp471=_tmp46F;struct Cyc_Absyn_Vardecl*_tmp473;void*_tmp474;_LL262: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp472=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp472->tag != 8)goto _LL264;else{_tmp473=_tmp472->f1;_tmp474=(void*)_tmp472->f2;}}_LL263: {
# 1807
struct Cyc_Core_Impossible_exn_struct _tmp914;const char*_tmp913;struct Cyc_Core_Impossible_exn_struct*_tmp912;(int)_throw((void*)((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912[0]=((_tmp914.tag=Cyc_Core_Impossible,((_tmp914.f1=((_tmp913="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp913,sizeof(char),38))),_tmp914)))),_tmp912)))));}_LL264: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp475=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp475->tag != 7)goto _LL266;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp476=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp475->f3);if(_tmp476->tag != 5)goto _LL266;}}_LL265:
 goto _LL267;_LL266: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp477=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp477->tag != 5)goto _LL268;}_LL267:
# 1812
 return Cyc_NewControlFlow_anal_derefL(env,_tmp465,_tmp44F,_tmp465,_tmp46C,passthrough_consumes,0,flds);_LL268:;_LL269: {
# 1819
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp432,_tmp469,_tmp46F);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp432->unknown_all: _tmp432->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp432,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp44F->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp917;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp916;void*new_root=(void*)((_tmp916=_region_malloc(_tmp432->r,sizeof(*_tmp916)),((_tmp916[0]=((_tmp917.tag=1,((_tmp917.f1=e,((_tmp917.f2=(void*)_check_null(e->topt),_tmp917)))))),_tmp916))));
struct Cyc_CfFlowInfo_Place*_tmp918;struct Cyc_CfFlowInfo_Place*place=(_tmp918=_region_malloc(_tmp432->r,sizeof(*_tmp918)),((_tmp918->root=new_root,((_tmp918->fields=0,_tmp918)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp91B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp91A;void*res=(void*)((_tmp91A=_region_malloc(_tmp432->r,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp91B.tag=5,((_tmp91B.f1=place,_tmp91B)))),_tmp91A))));
for(0;_tmp470 != 0;_tmp470=_tmp470->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp91E;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp91D;res=(void*)((_tmp91D=_region_malloc(_tmp432->r,sizeof(*_tmp91D)),((_tmp91D[0]=((_tmp91E.tag=8,((_tmp91E.f1=(struct Cyc_Absyn_Vardecl*)_tmp470->hd,((_tmp91E.f2=res,_tmp91E)))))),_tmp91D))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp469=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp469,new_root,new_rval);
_tmp469=Cyc_CfFlowInfo_assign_place(_tmp432,e->loc,_tmp469,env->all_changed,_tmp46B,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp47D=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp46A);
# 1837
return Cyc_NewControlFlow_anal_derefL(env,_tmp47D,_tmp44F,_tmp47D,res,passthrough_consumes,1,flds);};}_LL261:;};}_LL25F:;_LL260:
# 1840
 goto _LL25C;_LL25C:;};}{
# 1843
struct _tuple16 _tmp484=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp44F);union Cyc_CfFlowInfo_FlowInfo _tmp486;void*_tmp487;struct _tuple16 _tmp485=_tmp484;_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp44F,_tmp486,_tmp487,passthrough_consumes,0,flds);};_LL251: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp450=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp450->tag != 22)goto _LL253;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}_LL252: {
# 1847
void*_tmp488=Cyc_Tcutil_compress((void*)_check_null(_tmp451->topt));void*_tmp489=_tmp488;union Cyc_Absyn_Constraint*_tmp48C;_LL26B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 10)goto _LL26D;}_LL26C: {
# 1849
unsigned int _tmp48D=(Cyc_Evexp_eval_const_uint_exp(_tmp452)).f1;
struct Cyc_List_List*_tmp91F;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp451,((_tmp91F=_region_malloc(env->r,sizeof(*_tmp91F)),((_tmp91F->hd=(void*)((int)_tmp48D),((_tmp91F->tl=flds,_tmp91F)))))));}_LL26D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489;if(_tmp48B->tag != 5)goto _LL26F;else{_tmp48C=((_tmp48B->f1).ptr_atts).bounds;}}_LL26E: {
# 1852
struct _RegionHandle _tmp48F=_new_region("temp");struct _RegionHandle*temp=& _tmp48F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp920[2];struct _tuple23 _tmp490=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp920[1]=_tmp452,((_tmp920[0]=_tmp451,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp920,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1853
union Cyc_CfFlowInfo_FlowInfo _tmp492;struct Cyc_List_List*_tmp493;struct _tuple23 _tmp491=_tmp490;_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{
# 1855
union Cyc_CfFlowInfo_FlowInfo _tmp494=_tmp492;
{union Cyc_CfFlowInfo_FlowInfo _tmp495=_tmp492;struct Cyc_Dict_Dict _tmp496;struct Cyc_List_List*_tmp497;_LL272: if((_tmp495.ReachableFL).tag != 2)goto _LL274;_tmp496=((struct _tuple14)(_tmp495.ReachableFL).val).f1;_tmp497=((struct _tuple14)(_tmp495.ReachableFL).val).f2;_LL273:
# 1858
 if(Cyc_CfFlowInfo_initlevel(_tmp432,_tmp496,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp493))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp923;void*_tmp922;(_tmp922=0,Cyc_CfFlowInfo_aerr(_tmp452->loc,((_tmp923="expression may not be initialized",_tag_dyneither(_tmp923,sizeof(char),34))),_tag_dyneither(_tmp922,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp49A=Cyc_NewControlFlow_add_subscript_reln(_tmp432->r,_tmp497,_tmp451,_tmp452);
if(_tmp497 != _tmp49A)
_tmp494=Cyc_CfFlowInfo_ReachableFL(_tmp496,_tmp49A);
goto _LL271;};_LL274:;_LL275:
 goto _LL271;_LL271:;}{
# 1866
struct _tuple18 _tmp49B=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp451,_tmp492,(void*)((struct Cyc_List_List*)_check_null(_tmp493))->hd,passthrough_consumes,0,flds);union Cyc_CfFlowInfo_FlowInfo _tmp49D;union Cyc_CfFlowInfo_AbsLVal _tmp49E;struct _tuple18 _tmp49C=_tmp49B;_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp49F=_tmp49D;_LL277: if((_tmp49F.BottomFL).tag != 1)goto _LL279;_LL278: {
struct _tuple18 _tmp924;struct _tuple18 _tmp4A1=(_tmp924.f1=_tmp49D,((_tmp924.f2=_tmp49E,_tmp924)));_npop_handler(0);return _tmp4A1;}_LL279:;_LL27A: {
struct _tuple18 _tmp925;struct _tuple18 _tmp4A3=(_tmp925.f1=_tmp494,((_tmp925.f2=_tmp49E,_tmp925)));_npop_handler(0);return _tmp4A3;}_LL276:;};};};}
# 1853
;_pop_region(temp);}_LL26F:;_LL270: {
# 1872
struct Cyc_Core_Impossible_exn_struct _tmp92B;const char*_tmp92A;struct Cyc_Core_Impossible_exn_struct*_tmp929;(int)_throw((void*)((_tmp929=_cycalloc(sizeof(*_tmp929)),((_tmp929[0]=((_tmp92B.tag=Cyc_Core_Impossible,((_tmp92B.f1=((_tmp92A="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp92A,sizeof(char),33))),_tmp92B)))),_tmp929)))));}_LL26A:;}_LL253: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp453=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp453->tag != 20)goto _LL255;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}}_LL254: {
# 1876
void*_tmp4A8=(void*)_check_null(_tmp454->topt);
if(Cyc_Absyn_is_require_union_type(_tmp4A8))
Cyc_NewControlFlow_check_requires(_tmp454->loc,_tmp432->r,_tmp4A8,_tmp455,inflow);
# 1880
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp4A8)){
struct _tuple18 _tmp92C;return(_tmp92C.f1=inflow,((_tmp92C.f2=Cyc_CfFlowInfo_UnknownL(),_tmp92C)));}{
# 1883
struct Cyc_List_List*_tmp92D;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp454,(
(_tmp92D=_region_malloc(env->r,sizeof(*_tmp92D)),((_tmp92D->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp4A8,_tmp455),((_tmp92D->tl=flds,_tmp92D)))))));};}_LL255:;_LL256: {
# 1886
struct _tuple18 _tmp92E;return(_tmp92E.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp92E.f2=Cyc_CfFlowInfo_UnknownL(),_tmp92E)));}_LL23E:;};}
# 1890
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1893
struct _tuple18 _tmp4AC=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp4AE;union Cyc_CfFlowInfo_AbsLVal _tmp4AF;struct _tuple18 _tmp4AD=_tmp4AC;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{
struct _tuple18 _tmp92F;return(_tmp92F.f1=_tmp4AE,((_tmp92F.f2=_tmp4AF,_tmp92F)));};}
# 1900
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1903
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp4B1=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp4B3;struct _tuple18 _tmp4B2=_tmp4B1;_tmp4B3=_tmp4B2.f1;
inflow=_tmp4B3;}}}
# 1911
return inflow;}
# 1917
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1920
struct Cyc_CfFlowInfo_FlowEnv*_tmp4B4=env->fenv;
# 1923
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp4B5=_new_region("temp");struct _RegionHandle*temp=& _tmp4B5;_push_region(temp);{
struct _tuple23 _tmp4B6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp4B8;struct Cyc_List_List*_tmp4B9;struct _tuple23 _tmp4B7=_tmp4B6;_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp4B9))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4B9->tl))->hd;
f=_tmp4B8;}{
# 1933
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=f;struct Cyc_Dict_Dict _tmp4BB;struct Cyc_List_List*_tmp4BC;_LL27C: if((_tmp4BA.BottomFL).tag != 1)goto _LL27E;_LL27D: {
struct _tuple19 _tmp930;struct _tuple19 _tmp4BE=(_tmp930.f1=f,((_tmp930.f2=f,_tmp930)));_npop_handler(0);return _tmp4BE;}_LL27E: if((_tmp4BA.ReachableFL).tag != 2)goto _LL27B;_tmp4BB=((struct _tuple14)(_tmp4BA.ReachableFL).val).f1;_tmp4BC=((struct _tuple14)(_tmp4BA.ReachableFL).val).f2;_LL27F: {
# 1936
struct Cyc_Absyn_Exp*_tmp4BF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp4C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4BB,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp933;void*_tmp932;(_tmp932=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp933="expression may not be initialized",_tag_dyneither(_tmp933,sizeof(char),34))),_tag_dyneither(_tmp932,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4BB,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp936;void*_tmp935;(_tmp935=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp936="expression may not be initialized",_tag_dyneither(_tmp936,sizeof(char),34))),_tag_dyneither(_tmp935,sizeof(void*),0)));}{
# 1943
union Cyc_CfFlowInfo_FlowInfo _tmp4C5=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4C6=f;
# 1948
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp4C7=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp4C9;struct Cyc_List_List*_tmp4CA;struct _tuple15 _tmp4C8=_tmp4C7;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;{
struct _tuple15 _tmp4CB=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp4CD;struct Cyc_List_List*_tmp4CE;struct _tuple15 _tmp4CC=_tmp4CB;_tmp4CD=_tmp4CC.f1;_tmp4CE=_tmp4CC.f2;{
struct _tuple0 _tmp937;struct _tuple0 _tmp4CF=(_tmp937.f1=_tmp4C9,((_tmp937.f2=_tmp4CD,_tmp937)));struct _tuple0 _tmp4D0=_tmp4CF;enum Cyc_CfFlowInfo_InitLevel _tmp4D2;enum Cyc_CfFlowInfo_InitLevel _tmp4D6;_LL281:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D1->tag != 3)goto _LL283;else{_tmp4D2=_tmp4D1->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D3->tag != 0)goto _LL283;};_LL282: {
# 1955
struct _tuple19 _tmp4E5=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4BF,_tmp4D2,_tmp4CA);union Cyc_CfFlowInfo_FlowInfo _tmp4E7;union Cyc_CfFlowInfo_FlowInfo _tmp4E8;struct _tuple19 _tmp4E6=_tmp4E5;_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;
switch(p){case Cyc_Absyn_Eq: _LL295:
 _tmp4C5=_tmp4E8;_tmp4C6=_tmp4E7;break;case Cyc_Absyn_Neq: _LL296:
 _tmp4C5=_tmp4E7;_tmp4C6=_tmp4E8;break;default: _LL297: {
struct Cyc_Core_Impossible_exn_struct _tmp93D;const char*_tmp93C;struct Cyc_Core_Impossible_exn_struct*_tmp93B;(int)_throw((void*)((_tmp93B=_cycalloc(sizeof(*_tmp93B)),((_tmp93B[0]=((_tmp93D.tag=Cyc_Core_Impossible,((_tmp93D.f1=((_tmp93C="anal_test, zero-split",_tag_dyneither(_tmp93C,sizeof(char),22))),_tmp93D)))),_tmp93B)))));}}
# 1961
goto _LL280;}_LL283:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D4->tag != 0)goto _LL285;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D5->tag != 3)goto _LL285;else{_tmp4D6=_tmp4D5->f1;}};_LL284: {
# 1963
struct _tuple19 _tmp4EC=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4C0,_tmp4D6,_tmp4CE);union Cyc_CfFlowInfo_FlowInfo _tmp4EE;union Cyc_CfFlowInfo_FlowInfo _tmp4EF;struct _tuple19 _tmp4ED=_tmp4EC;_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;
switch(p){case Cyc_Absyn_Eq: _LL299:
 _tmp4C5=_tmp4EF;_tmp4C6=_tmp4EE;break;case Cyc_Absyn_Neq: _LL29A:
 _tmp4C5=_tmp4EE;_tmp4C6=_tmp4EF;break;default: _LL29B: {
struct Cyc_Core_Impossible_exn_struct _tmp943;const char*_tmp942;struct Cyc_Core_Impossible_exn_struct*_tmp941;(int)_throw((void*)((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=((_tmp943.tag=Cyc_Core_Impossible,((_tmp943.f1=((_tmp942="anal_test, zero-split",_tag_dyneither(_tmp942,sizeof(char),22))),_tmp943)))),_tmp941)))));}}
# 1969
goto _LL280;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D7->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D8->tag != 0)goto _LL287;};_LL286:
# 1971
 if(p == Cyc_Absyn_Eq)_tmp4C6=Cyc_CfFlowInfo_BottomFL();else{
_tmp4C5=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D9->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DA->tag != 1)goto _LL289;};_LL288:
 goto _LL28A;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DB->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4DC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DC->tag != 2)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DD->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4DE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DE->tag != 5)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4DF=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DF->tag != 1)goto _LL28F;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E0->tag != 0)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4E1->tag != 2)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E2->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4E3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4E3->tag != 5)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E4->tag != 0)goto _LL293;};_LL292:
# 1980
 if(p == Cyc_Absyn_Neq)_tmp4C6=Cyc_CfFlowInfo_BottomFL();else{
_tmp4C5=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL293:;_LL294:
 goto _LL280;_LL280:;};};}
# 1991
{struct _tuple0 _tmp944;struct _tuple0 _tmp4F4=(_tmp944.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp4BF->topt)),((_tmp944.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp4C0->topt)),_tmp944)));struct _tuple0 _tmp4F5=_tmp4F4;_LL29E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F5.f1;if(_tmp4F6->tag != 6)goto _LL2A0;else{if(_tmp4F6->f1 != Cyc_Absyn_Unsigned)goto _LL2A0;}}_LL29F:
 goto _LL2A1;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F5.f2;if(_tmp4F7->tag != 6)goto _LL2A2;else{if(_tmp4F7->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F5.f1;if(_tmp4F8->tag != 19)goto _LL2A4;}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F5.f2;if(_tmp4F9->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL29D;_LL2A6:;_LL2A7: {
struct _tuple19 _tmp945;struct _tuple19 _tmp4FB=(_tmp945.f1=_tmp4C5,((_tmp945.f2=_tmp4C6,_tmp945)));_npop_handler(0);return _tmp4FB;}_LL29D:;}
# 2000
switch(p){case Cyc_Absyn_Eq: _LL2A8:
 goto _LL2A9;case Cyc_Absyn_Neq: _LL2A9: goto _LL2AA;case Cyc_Absyn_Gt: _LL2AA: goto _LL2AB;case Cyc_Absyn_Gte: _LL2AB: goto _LL2AC;case Cyc_Absyn_Lt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Lte: _LL2AD: break;default: _LL2AE: {
struct _tuple19 _tmp946;struct _tuple19 _tmp4FE=(_tmp946.f1=_tmp4C5,((_tmp946.f2=_tmp4C6,_tmp946)));_npop_handler(0);return _tmp4FE;}}{
# 2005
struct _RegionHandle*_tmp4FF=(env->fenv)->r;
struct _tuple13 _tmp500=Cyc_Relations_primop2relation(_tmp4BF,p,_tmp4C0);struct Cyc_Absyn_Exp*_tmp502;enum Cyc_Relations_Relation _tmp503;struct Cyc_Absyn_Exp*_tmp504;struct _tuple13 _tmp501=_tmp500;_tmp502=_tmp501.f1;_tmp503=_tmp501.f2;_tmp504=_tmp501.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2010
if(Cyc_Relations_exp2relnop(_tmp502,& n1) && Cyc_Relations_exp2relnop(_tmp504,& n2)){
# 2012
struct Cyc_List_List*_tmp505=Cyc_Relations_add_relation(_tmp4FF,n1,_tmp503,n2,_tmp4BC);
# 2016
struct Cyc_List_List*_tmp506=Cyc_Relations_add_relation(_tmp4FF,n2,Cyc_Relations_flip_relation(_tmp503),n1,_tmp4BC);
struct _tuple19 _tmp947;struct _tuple19 _tmp507=(_tmp947.f1=_tmp4C5,((_tmp947.f2=_tmp4C6,_tmp947)));struct _tuple19 _tmp508=_tmp507;struct Cyc_Dict_Dict _tmp509;struct Cyc_Dict_Dict _tmp50A;struct Cyc_Dict_Dict _tmp50B;struct Cyc_Dict_Dict _tmp50C;_LL2B1: if(((_tmp508.f1).ReachableFL).tag != 2)goto _LL2B3;_tmp509=((struct _tuple14)((_tmp508.f1).ReachableFL).val).f1;if(((_tmp508.f2).ReachableFL).tag != 2)goto _LL2B3;_tmp50A=((struct _tuple14)((_tmp508.f2).ReachableFL).val).f1;_LL2B2: {
# 2019
struct _tuple19 _tmp948;struct _tuple19 _tmp50E=(_tmp948.f1=Cyc_CfFlowInfo_ReachableFL(_tmp509,_tmp505),((_tmp948.f2=Cyc_CfFlowInfo_ReachableFL(_tmp50A,_tmp506),_tmp948)));_npop_handler(0);return _tmp50E;}_LL2B3: if(((_tmp508.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp50B=((struct _tuple14)((_tmp508.f2).ReachableFL).val).f1;_LL2B4: {
# 2021
struct _tuple19 _tmp949;struct _tuple19 _tmp510=(_tmp949.f1=_tmp4C5,((_tmp949.f2=Cyc_CfFlowInfo_ReachableFL(_tmp50B,_tmp506),_tmp949)));_npop_handler(0);return _tmp510;}_LL2B5: if(((_tmp508.f1).ReachableFL).tag != 2)goto _LL2B7;_tmp50C=((struct _tuple14)((_tmp508.f1).ReachableFL).val).f1;_LL2B6: {
# 2023
struct _tuple19 _tmp94A;struct _tuple19 _tmp512=(_tmp94A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp505),((_tmp94A.f2=_tmp4C6,_tmp94A)));_npop_handler(0);return _tmp512;}_LL2B7:;_LL2B8: {
# 2025
struct _tuple19 _tmp94B;struct _tuple19 _tmp514=(_tmp94B.f1=_tmp4C5,((_tmp94B.f2=_tmp4C6,_tmp94B)));_npop_handler(0);return _tmp514;}_LL2B0:;}else{
# 2028
struct _tuple19 _tmp94C;struct _tuple19 _tmp517=(_tmp94C.f1=_tmp4C5,((_tmp94C.f2=_tmp4C6,_tmp94C)));_npop_handler(0);return _tmp517;}};};};}_LL27B:;};
# 1925
;_pop_region(temp);}
# 2034
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2036
struct Cyc_CfFlowInfo_FlowEnv*_tmp518=env->fenv;
void*_tmp519=e->r;void*_tmp51A=_tmp519;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;enum Cyc_Absyn_Primop _tmp52B;struct Cyc_List_List*_tmp52C;_LL2BA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp51B->tag != 5)goto _LL2BC;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;_tmp51E=_tmp51B->f3;}}_LL2BB: {
# 2039
struct _tuple19 _tmp52D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp51C);union Cyc_CfFlowInfo_FlowInfo _tmp52F;union Cyc_CfFlowInfo_FlowInfo _tmp530;struct _tuple19 _tmp52E=_tmp52D;_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;{
struct _tuple19 _tmp531=Cyc_NewControlFlow_anal_test(env,_tmp52F,_tmp51D);union Cyc_CfFlowInfo_FlowInfo _tmp533;union Cyc_CfFlowInfo_FlowInfo _tmp534;struct _tuple19 _tmp532=_tmp531;_tmp533=_tmp532.f1;_tmp534=_tmp532.f2;{
struct _tuple19 _tmp535=Cyc_NewControlFlow_anal_test(env,_tmp530,_tmp51E);union Cyc_CfFlowInfo_FlowInfo _tmp537;union Cyc_CfFlowInfo_FlowInfo _tmp538;struct _tuple19 _tmp536=_tmp535;_tmp537=_tmp536.f1;_tmp538=_tmp536.f2;{
struct _tuple19 _tmp94D;return(_tmp94D.f1=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp533,_tmp537),((_tmp94D.f2=
Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp534,_tmp538),_tmp94D)));};};};}_LL2BC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp51F->tag != 6)goto _LL2BE;else{_tmp520=_tmp51F->f1;_tmp521=_tmp51F->f2;}}_LL2BD: {
# 2045
struct _tuple19 _tmp53A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp520);union Cyc_CfFlowInfo_FlowInfo _tmp53C;union Cyc_CfFlowInfo_FlowInfo _tmp53D;struct _tuple19 _tmp53B=_tmp53A;_tmp53C=_tmp53B.f1;_tmp53D=_tmp53B.f2;{
struct _tuple19 _tmp53E=Cyc_NewControlFlow_anal_test(env,_tmp53C,_tmp521);union Cyc_CfFlowInfo_FlowInfo _tmp540;union Cyc_CfFlowInfo_FlowInfo _tmp541;struct _tuple19 _tmp53F=_tmp53E;_tmp540=_tmp53F.f1;_tmp541=_tmp53F.f2;{
struct _tuple19 _tmp94E;return(_tmp94E.f1=_tmp540,((_tmp94E.f2=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp53D,_tmp541),_tmp94E)));};};}_LL2BE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp522->tag != 7)goto _LL2C0;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL2BF: {
# 2049
struct _tuple19 _tmp543=Cyc_NewControlFlow_anal_test(env,inflow,_tmp523);union Cyc_CfFlowInfo_FlowInfo _tmp545;union Cyc_CfFlowInfo_FlowInfo _tmp546;struct _tuple19 _tmp544=_tmp543;_tmp545=_tmp544.f1;_tmp546=_tmp544.f2;{
struct _tuple19 _tmp547=Cyc_NewControlFlow_anal_test(env,_tmp546,_tmp524);union Cyc_CfFlowInfo_FlowInfo _tmp549;union Cyc_CfFlowInfo_FlowInfo _tmp54A;struct _tuple19 _tmp548=_tmp547;_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;{
struct _tuple19 _tmp94F;return(_tmp94F.f1=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp545,_tmp549),((_tmp94F.f2=_tmp54A,_tmp94F)));};};}_LL2C0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp525->tag != 8)goto _LL2C2;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;}}_LL2C1: {
# 2053
struct _tuple16 _tmp54C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp526);union Cyc_CfFlowInfo_FlowInfo _tmp54E;void*_tmp54F;struct _tuple16 _tmp54D=_tmp54C;_tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp54E,_tmp527);}_LL2C2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp528->tag != 2)goto _LL2C4;else{if(_tmp528->f1 != Cyc_Absyn_Not)goto _LL2C4;if(_tmp528->f2 == 0)goto _LL2C4;_tmp529=(struct Cyc_Absyn_Exp*)(_tmp528->f2)->hd;if((_tmp528->f2)->tl != 0)goto _LL2C4;}}_LL2C3: {
# 2056
struct _tuple19 _tmp550=Cyc_NewControlFlow_anal_test(env,inflow,_tmp529);union Cyc_CfFlowInfo_FlowInfo _tmp552;union Cyc_CfFlowInfo_FlowInfo _tmp553;struct _tuple19 _tmp551=_tmp550;_tmp552=_tmp551.f1;_tmp553=_tmp551.f2;{
struct _tuple19 _tmp950;return(_tmp950.f1=_tmp553,((_tmp950.f2=_tmp552,_tmp950)));};}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp52A->tag != 2)goto _LL2C6;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL2C5:
# 2059
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp52B,_tmp52C);_LL2C6:;_LL2C7: {
# 2063
struct _tuple16 _tmp555=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp557;void*_tmp558;struct _tuple16 _tmp556=_tmp555;_tmp557=_tmp556.f1;_tmp558=_tmp556.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp559=_tmp557;struct Cyc_Dict_Dict _tmp55A;_LL2C9: if((_tmp559.BottomFL).tag != 1)goto _LL2CB;_LL2CA: {
struct _tuple19 _tmp951;return(_tmp951.f1=_tmp557,((_tmp951.f2=_tmp557,_tmp951)));}_LL2CB: if((_tmp559.ReachableFL).tag != 2)goto _LL2C8;_tmp55A=((struct _tuple14)(_tmp559.ReachableFL).val).f1;_LL2CC: {
# 2067
struct _tuple15 _tmp55C=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp558);void*_tmp55E;struct Cyc_List_List*_tmp55F;struct _tuple15 _tmp55D=_tmp55C;_tmp55E=_tmp55D.f1;_tmp55F=_tmp55D.f2;{
void*_tmp560=_tmp55E;struct Cyc_Absyn_Vardecl*_tmp562;void*_tmp563;enum Cyc_CfFlowInfo_InitLevel _tmp56C;_LL2CE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp561=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp561->tag != 8)goto _LL2D0;else{_tmp562=_tmp561->f1;_tmp563=(void*)_tmp561->f2;}}_LL2CF: {
# 2070
struct Cyc_Core_Impossible_exn_struct _tmp957;const char*_tmp956;struct Cyc_Core_Impossible_exn_struct*_tmp955;(int)_throw((void*)((_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955[0]=((_tmp957.tag=Cyc_Core_Impossible,((_tmp957.f1=((_tmp956="anal_test: bad namedlocation",_tag_dyneither(_tmp956,sizeof(char),29))),_tmp957)))),_tmp955)))));}_LL2D0: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp564=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp564->tag != 0)goto _LL2D2;}_LL2D1: {
struct _tuple19 _tmp958;return(_tmp958.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp958.f2=_tmp557,_tmp958)));}_LL2D2: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp565=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp565->tag != 2)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp566=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp566->tag != 1)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp567=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp567->tag != 5)goto _LL2D8;}_LL2D7: {
struct _tuple19 _tmp959;return(_tmp959.f1=_tmp557,((_tmp959.f2=Cyc_CfFlowInfo_BottomFL(),_tmp959)));}_LL2D8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp568=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp568->tag != 3)goto _LL2DA;else{if(_tmp568->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DA;}}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp569=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp569->tag != 4)goto _LL2DC;else{if(_tmp569->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp56A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56A->tag != 7)goto _LL2DE;}_LL2DD:
# 2078
{const char*_tmp95C;void*_tmp95B;(_tmp95B=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp95C="expression may not be initialized",_tag_dyneither(_tmp95C,sizeof(char),34))),_tag_dyneither(_tmp95B,sizeof(void*),0)));}{
struct _tuple19 _tmp95D;return(_tmp95D.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp95D.f2=Cyc_CfFlowInfo_BottomFL(),_tmp95D)));};_LL2DE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp56B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56B->tag != 3)goto _LL2E0;else{_tmp56C=_tmp56B->f1;}}_LL2DF:
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp557,e,_tmp56C,_tmp55F);_LL2E0: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp56D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56D->tag != 4)goto _LL2E2;}_LL2E1: {
struct _tuple19 _tmp95E;return(_tmp95E.f1=_tmp557,((_tmp95E.f2=_tmp557,_tmp95E)));}_LL2E2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp56E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56E->tag != 6)goto _LL2CD;}_LL2E3: {
struct Cyc_Core_Impossible_exn_struct _tmp964;const char*_tmp963;struct Cyc_Core_Impossible_exn_struct*_tmp962;(int)_throw((void*)((_tmp962=_cycalloc(sizeof(*_tmp962)),((_tmp962[0]=((_tmp964.tag=Cyc_Core_Impossible,((_tmp964.f1=((_tmp963="anal_test",_tag_dyneither(_tmp963,sizeof(char),10))),_tmp964)))),_tmp962)))));}_LL2CD:;};}_LL2C8:;};}_LL2B9:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2088
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2090
unsigned int _tmp57C;struct Cyc_NewControlFlow_AnalEnv*_tmp57D;struct Cyc_Dict_Dict _tmp57E;struct _tuple27*_tmp57B=ckenv;_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;{
void*_tmp57F=root;struct Cyc_Absyn_Vardecl*_tmp581;_LL2E5: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp580=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp57F;if(_tmp580->tag != 0)goto _LL2E7;else{_tmp581=_tmp580->f1;}}_LL2E6:
# 2093
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp57D->fenv)->r,_tmp581->type)){
struct _tuple15 _tmp582=Cyc_CfFlowInfo_unname_rval((_tmp57D->fenv)->r,rval);void*_tmp584;struct _tuple15 _tmp583=_tmp582;_tmp584=_tmp583.f1;{
void*_tmp585=_tmp584;_LL2EA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp586=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp586->tag != 7)goto _LL2EC;}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp587=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp587->tag != 0)goto _LL2EE;}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp588=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp588->tag != 3)goto _LL2F0;else{if(_tmp588->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F0;}}_LL2EF:
 goto _LL2E9;_LL2F0:;_LL2F1:
# 2100
{const char*_tmp968;void*_tmp967[1];struct Cyc_String_pa_PrintArg_struct _tmp966;(_tmp966.tag=0,((_tmp966.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp581->name)),((_tmp967[0]=& _tmp966,Cyc_Tcutil_warn(_tmp57C,((_tmp968="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp968,sizeof(char),57))),_tag_dyneither(_tmp967,sizeof(void*),1)))))));}
goto _LL2E9;_LL2E9:;};}
# 2104
goto _LL2E4;_LL2E7:;_LL2E8:
 goto _LL2E4;_LL2E4:;};}
# 2109
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp58C=flow;struct Cyc_Dict_Dict _tmp58D;_LL2F3: if((_tmp58C.BottomFL).tag != 1)goto _LL2F5;_LL2F4:
 return;_LL2F5: if((_tmp58C.ReachableFL).tag != 2)goto _LL2F2;_tmp58D=((struct _tuple14)(_tmp58C.ReachableFL).val).f1;_LL2F6:
# 2113
{struct Cyc_List_List*_tmp58E=env->param_roots;for(0;_tmp58E != 0;_tmp58E=_tmp58E->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp58D,Cyc_CfFlowInfo_lookup_place(_tmp58D,(struct Cyc_CfFlowInfo_Place*)_tmp58E->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp96B;void*_tmp96A;(_tmp96A=0,Cyc_CfFlowInfo_aerr(loc,((_tmp96B="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp96B,sizeof(char),76))),_tag_dyneither(_tmp96A,sizeof(void*),0)));}}}
# 2117
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp96C;struct _tuple27 _tmp591=(_tmp96C.f1=loc,((_tmp96C.f2=env,((_tmp96C.f3=_tmp58D,_tmp96C)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp591,_tmp58D);}
# 2121
return;_LL2F2:;}
# 2125
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_params(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2128
struct Cyc_List_List*_tmp593=0;
struct Cyc_List_List*_tmp594=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp595=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp597;struct Cyc_Absyn_Exp*_tmp598;struct _tuple24*_tmp596=_tmp595;_tmp597=_tmp596->f1;_tmp598=_tmp596->f2;
if((_tmp597 != 0  && _tmp598 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp598->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp96F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp96E;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp599=(_tmp96E=_region_malloc(frgn,sizeof(*_tmp96E)),((_tmp96E[0]=((_tmp96F.tag=0,((_tmp96F.f1=*_tmp597,_tmp96F)))),_tmp96E)));
struct Cyc_CfFlowInfo_Place*_tmp970;struct Cyc_CfFlowInfo_Place*_tmp59A=(_tmp970=_region_malloc(frgn,sizeof(*_tmp970)),((_tmp970->root=(void*)_tmp599,((_tmp970->fields=0,_tmp970)))));
{struct Cyc_List_List*_tmp971;_tmp593=((_tmp971=_region_malloc(frgn,sizeof(*_tmp971)),((_tmp971->hd=_tmp59A,((_tmp971->tl=_tmp593,_tmp971))))));}{
struct Cyc_List_List*_tmp972;_tmp594=((_tmp972=_region_malloc(frgn,sizeof(*_tmp972)),((_tmp972->hd=_tmp598,((_tmp972->tl=_tmp594,_tmp972))))));};}}}{
# 2140
struct _tuple1 _tmp973;return(_tmp973.f1=_tmp593,((_tmp973.f2=_tmp594,_tmp973)));};}
# 2149
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2154
{union Cyc_CfFlowInfo_FlowInfo _tmp5A1=flow;_LL2F8: if((_tmp5A1.BottomFL).tag != 1)goto _LL2FA;_LL2F9:
 return flow;_LL2FA:;_LL2FB:
 goto _LL2F7;_LL2F7:;}{
# 2158
int _tmp5A2=unconsume_exps != 0;
struct Cyc_Absyn_Exp*_tmp5A3=Cyc_Absyn_exp_unsigned_one;
int _tmp5A4=0;
int _tmp5A5=1;
# 2165
{struct Cyc_List_List*_tmp5A6=consumed_vals;for(0;_tmp5A6 != 0;
(_tmp5A6=_tmp5A6->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
union Cyc_CfFlowInfo_FlowInfo _tmp5A7=flow;struct Cyc_Dict_Dict _tmp5A8;_LL2FD: if((_tmp5A7.ReachableFL).tag != 2)goto _LL2FF;_tmp5A8=((struct _tuple14)(_tmp5A7.ReachableFL).val).f1;_LL2FE: {
# 2175
void*_tmp5A9=Cyc_CfFlowInfo_lookup_place(_tmp5A8,(struct Cyc_CfFlowInfo_Place*)_tmp5A6->hd);
void*_tmp5AA=_tmp5A9;
struct Cyc_CfFlowInfo_Place*_tmp5AB=(struct Cyc_CfFlowInfo_Place*)_tmp5A6->hd;void*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct Cyc_CfFlowInfo_Place*_tmp5AC=_tmp5AB;_tmp5AD=_tmp5AC->root;_tmp5AE=_tmp5AC->fields;
if(_tmp5AE != 0){
const char*_tmp976;void*_tmp975;(_tmp975=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp976="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp976,sizeof(char),53))),_tag_dyneither(_tmp975,sizeof(void*),0)));}
{void*_tmp5B1=_tmp5AD;struct Cyc_Absyn_Vardecl*_tmp5B3;_LL302: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5B2=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp5B1;if(_tmp5B2->tag != 0)goto _LL304;else{_tmp5B3=_tmp5B2->f1;}}_LL303: {
# 2188
int varok=0;
{void*_tmp5B4=_tmp5A9;struct Cyc_Absyn_Vardecl*_tmp5B6;void*_tmp5B7;_LL307: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5B5=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5B4;if(_tmp5B5->tag != 8)goto _LL309;else{_tmp5B6=_tmp5B5->f1;_tmp5B7=(void*)_tmp5B5->f2;}}_LL308:
# 2191
 if(_tmp5B3 == _tmp5B6){
_tmp5AA=_tmp5B7;
# 2194
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5B3->type)){
# 2196
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp5A3,_tmp5A5,_tmp5AA,& _tmp5A4)){
if(!_tmp5A2){
const char*_tmp97A;void*_tmp979[1];struct Cyc_String_pa_PrintArg_struct _tmp978;(_tmp978.tag=0,((_tmp978.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_qvar2string(_tmp5B3->name)),((_tmp979[0]=& _tmp978,Cyc_CfFlowInfo_aerr(loc,((_tmp97A="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp97A,sizeof(char),58))),_tag_dyneither(_tmp979,sizeof(void*),1)))))));}}else{
# 2203
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5A8,_tmp5AA)!= Cyc_CfFlowInfo_AllIL  && !_tmp5A2){
const char*_tmp97E;void*_tmp97D[1];struct Cyc_String_pa_PrintArg_struct _tmp97C;(_tmp97C.tag=0,((_tmp97C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2206
Cyc_Absynpp_qvar2string(_tmp5B3->name)),((_tmp97D[0]=& _tmp97C,Cyc_CfFlowInfo_aerr(loc,((_tmp97E="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp97E,sizeof(char),84))),_tag_dyneither(_tmp97D,sizeof(void*),1)))))));}else{
# 2208
varok=1;}}}else{
# 2211
varok=1;}}else{
# 2214
goto _LL30A;}
goto _LL306;_LL309:;_LL30A:
# 2218
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5B3->type))
varok=1;else{
if(!_tmp5A2){
const char*_tmp982;void*_tmp981[1];struct Cyc_String_pa_PrintArg_struct _tmp980;(_tmp980.tag=0,((_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2223
Cyc_Absynpp_qvar2string(_tmp5B3->name)),((_tmp981[0]=& _tmp980,Cyc_CfFlowInfo_aerr(loc,((_tmp982="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp982,sizeof(char),85))),_tag_dyneither(_tmp981,sizeof(void*),1)))))));}}
goto _LL306;_LL306:;}
# 2228
if(_tmp5A2){
# 2234
struct _tuple18 _tmp5C1=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp5C3;union Cyc_CfFlowInfo_AbsLVal _tmp5C4;struct _tuple18 _tmp5C2=_tmp5C1;_tmp5C3=_tmp5C2.f1;_tmp5C4=_tmp5C2.f2;{
# 2237
union Cyc_CfFlowInfo_AbsLVal _tmp5C5=_tmp5C4;struct Cyc_CfFlowInfo_Place*_tmp5C6;_LL30C: if((_tmp5C5.PlaceL).tag != 1)goto _LL30E;_tmp5C6=(struct Cyc_CfFlowInfo_Place*)(_tmp5C5.PlaceL).val;_LL30D: {
# 2241
void*_tmp5C7=Cyc_CfFlowInfo_lookup_place(_tmp5A8,_tmp5C6);
{void*_tmp5C8=_tmp5C7;struct Cyc_Absyn_Vardecl*_tmp5CA;void*_tmp5CB;_LL311: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5C9=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5C8;if(_tmp5C9->tag != 8)goto _LL313;else{_tmp5CA=_tmp5C9->f1;_tmp5CB=(void*)_tmp5C9->f2;}}_LL312: {
# 2247
void*new_rval;
if(_tmp5CA == _tmp5B3){
# 2251
if(varok){
# 2253
_tmp5C7=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp5CB);{
# 2262
struct _tuple16 _tmp984;struct _tuple16 _tmp983;struct _tuple16 _tmp5CC=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2265
(_tmp983.f1=_tmp5C3,((_tmp983.f2=_tmp5C7,_tmp983)))),(
(_tmp984.f1=_tmp5C3,((_tmp984.f2=_tmp5AA,_tmp984)))));
# 2262
union Cyc_CfFlowInfo_FlowInfo _tmp5CE;void*_tmp5CF;struct _tuple16 _tmp5CD=_tmp5CC;_tmp5CE=_tmp5CD.f1;_tmp5CF=_tmp5CD.f2;
# 2267
_tmp5C3=_tmp5CE;new_rval=_tmp5CF;};}else{
# 2269
new_rval=_tmp5CB;}
# 2271
{union Cyc_CfFlowInfo_FlowInfo _tmp5D2=_tmp5C3;struct Cyc_Dict_Dict _tmp5D3;struct Cyc_List_List*_tmp5D4;_LL316: if((_tmp5D2.ReachableFL).tag != 2)goto _LL318;_tmp5D3=((struct _tuple14)(_tmp5D2.ReachableFL).val).f1;_tmp5D4=((struct _tuple14)(_tmp5D2.ReachableFL).val).f2;_LL317:
# 2273
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp5D3,env->all_changed,_tmp5C6,new_rval),_tmp5D4);
# 2277
goto _LL315;_LL318:;_LL319: {
# 2279
const char*_tmp987;void*_tmp986;(_tmp986=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp987="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp987,sizeof(char),42))),_tag_dyneither(_tmp986,sizeof(void*),0)));}_LL315:;}
# 2281
goto _LL310;}else{
# 2283
goto _LL314;}
goto _LL310;}_LL313:;_LL314:
# 2290
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,_tmp5B3->type)){
const char*_tmp98B;void*_tmp98A[1];struct Cyc_String_pa_PrintArg_struct _tmp989;(_tmp989.tag=0,((_tmp989.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)unconsume_exps->hd)),((_tmp98A[0]=& _tmp989,Cyc_CfFlowInfo_aerr(loc,((_tmp98B="aliased expression %s was overwritten",_tag_dyneither(_tmp98B,sizeof(char),38))),_tag_dyneither(_tmp98A,sizeof(void*),1)))))));}
# 2306 "new_control_flow.cyc"
goto _LL310;_LL310:;}
# 2308
goto _LL30B;}_LL30E:;_LL30F:
# 2314
 goto _LL30B;_LL30B:;};}
# 2317
goto _LL301;}_LL304:;_LL305: {
# 2320
const char*_tmp98E;void*_tmp98D;(_tmp98D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp98E="unconsume_params: root is not a varroot",_tag_dyneither(_tmp98E,sizeof(char),40))),_tag_dyneither(_tmp98D,sizeof(void*),0)));}_LL301:;}
# 2322
goto _LL2FC;}_LL2FF: if((_tmp5A7.BottomFL).tag != 1)goto _LL2FC;_LL300: {
# 2324
const char*_tmp991;void*_tmp990;(_tmp990=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp991="unconsume_params: flow became Bottom!",_tag_dyneither(_tmp991,sizeof(char),38))),_tag_dyneither(_tmp990,sizeof(void*),0)));}_LL2FC:;}}
# 2327
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple28{struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 2331
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2334
struct Cyc_CfFlowInfo_FlowEnv*_tmp5DE=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5DF=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E2;struct Cyc_Absyn_Stmt*_tmp5E3;unsigned int _tmp5E4;struct Cyc_Absyn_Switch_clause*_tmp5E0=_tmp5DF;_tmp5E1=_tmp5E0->pat_vars;_tmp5E2=_tmp5E0->where_clause;_tmp5E3=_tmp5E0->body;_tmp5E4=_tmp5E0->loc;{
struct _tuple1 _tmp5E5=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5E1))->v);struct Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;struct _tuple1 _tmp5E6=_tmp5E5;_tmp5E7=_tmp5E6.f1;_tmp5E8=_tmp5E6.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp5E1->v,_tmp5E7 != 0,_tmp5E4,exp_loc);
# 2342
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp5E9=env->unique_pat_vars;
{struct _tuple28*_tmp994;struct Cyc_List_List*_tmp993;env->unique_pat_vars=(
(_tmp993=_region_malloc(env->r,sizeof(*_tmp993)),((_tmp993->hd=((_tmp994=_region_malloc(env->r,sizeof(*_tmp994)),((_tmp994->f1=_tmp5E3,((_tmp994->f2=_tmp5E7,((_tmp994->f3=_tmp5E8,_tmp994)))))))),((_tmp993->tl=_tmp5E9,_tmp993))))));}
# 2347
if(_tmp5E2 != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5E2;
struct _tuple19 _tmp5EC=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp5EE;union Cyc_CfFlowInfo_FlowInfo _tmp5EF;struct _tuple19 _tmp5ED=_tmp5EC;_tmp5EE=_tmp5ED.f1;_tmp5EF=_tmp5ED.f2;
inflow=_tmp5EF;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5EE,_tmp5E3,0);}else{
# 2353
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5E3,0);}
# 2355
env->unique_pat_vars=_tmp5E9;{
union Cyc_CfFlowInfo_FlowInfo _tmp5F0=clause_outflow;_LL31B: if((_tmp5F0.BottomFL).tag != 1)goto _LL31D;_LL31C:
 goto _LL31A;_LL31D:;_LL31E:
# 2360
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5E7,_tmp5E8,clause_outflow,_tmp5E4);
# 2362
if(scs->tl == 0)
return clause_outflow;else{
# 2367
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp997;void*_tmp996;(_tmp996=0,Cyc_CfFlowInfo_aerr(_tmp5E3->loc,((_tmp997="switch clause may implicitly fallthru",_tag_dyneither(_tmp997,sizeof(char),38))),_tag_dyneither(_tmp996,sizeof(void*),0)));}else{
# 2370
const char*_tmp99A;void*_tmp999;(_tmp999=0,Cyc_Tcutil_warn(_tmp5E3->loc,((_tmp99A="switch clause may implicitly fallthru",_tag_dyneither(_tmp99A,sizeof(char),38))),_tag_dyneither(_tmp999,sizeof(void*),0)));}
# 2372
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2374
goto _LL31A;_LL31A:;};};};}
# 2377
return Cyc_CfFlowInfo_BottomFL();}struct _tuple29{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2382
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple29*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2384
struct Cyc_NewControlFlow_AnalEnv*_tmp5F6;struct Cyc_Dict_Dict _tmp5F7;unsigned int _tmp5F8;struct _tuple29*_tmp5F5=vdenv;_tmp5F6=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_tmp5F8=_tmp5F5->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5F6->fenv)->r,vd->type)){
# 2387
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp99B;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp99B.tag=0,((_tmp99B.f1=vd,_tmp99B)));
# 2389
struct _tuple15 _tmp5F9=Cyc_CfFlowInfo_unname_rval((_tmp5F6->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5F7,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2389
void*_tmp5FB;struct _tuple15 _tmp5FA=_tmp5F9;_tmp5FB=_tmp5FA.f1;{
# 2391
void*_tmp5FC=_tmp5FB;_LL320: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp5FD=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5FC;if(_tmp5FD->tag != 7)goto _LL322;}_LL321:
 goto _LL323;_LL322: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp5FE=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp5FC;if(_tmp5FE->tag != 0)goto _LL324;}_LL323:
 goto _LL325;_LL324: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp5FF=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp5FC;if(_tmp5FF->tag != 3)goto _LL326;else{if(_tmp5FF->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL326;}}_LL325:
 goto _LL31F;_LL326:;_LL327:
# 2396
{const char*_tmp99F;void*_tmp99E[1];struct Cyc_String_pa_PrintArg_struct _tmp99D;(_tmp99D.tag=0,((_tmp99D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp99E[0]=& _tmp99D,Cyc_Tcutil_warn(_tmp5F8,((_tmp99F="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp99F,sizeof(char),68))),_tag_dyneither(_tmp99E,sizeof(void*),1)))))));}
# 2398
goto _LL31F;_LL31F:;};}}
# 2403
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2405
{union Cyc_CfFlowInfo_FlowInfo _tmp604=inflow;struct Cyc_Dict_Dict _tmp605;_LL329: if((_tmp604.ReachableFL).tag != 2)goto _LL32B;_tmp605=((struct _tuple14)(_tmp604.ReachableFL).val).f1;_LL32A: {
# 2407
struct _tuple29 _tmp9A0;struct _tuple29 _tmp606=(_tmp9A0.f1=env,((_tmp9A0.f2=_tmp605,((_tmp9A0.f3=decl->loc,_tmp9A0)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp607=env->fenv;
{void*_tmp608=decl->r;void*_tmp609=_tmp608;struct Cyc_Absyn_Vardecl*_tmp60B;struct Cyc_List_List*_tmp60D;struct Cyc_List_List*_tmp60F;_LL32E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp60A=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp609;if(_tmp60A->tag != 0)goto _LL330;else{_tmp60B=_tmp60A->f1;}}_LL32F:
# 2411
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp606,_tmp60B);
goto _LL32D;_LL330: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp60C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp609;if(_tmp60C->tag != 2)goto _LL332;else{if(_tmp60C->f2 == 0)goto _LL332;_tmp60D=(struct Cyc_List_List*)(_tmp60C->f2)->v;}}_LL331: {
# 2414
struct _tuple1 _tmp610=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp60D);struct Cyc_List_List*_tmp612;struct _tuple1 _tmp611=_tmp610;_tmp612=_tmp611.f1;{
struct Cyc_List_List*_tmp613=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp612);
_tmp60F=_tmp613;goto _LL333;};}_LL332: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp60E=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp609;if(_tmp60E->tag != 3)goto _LL334;else{_tmp60F=_tmp60E->f1;}}_LL333:
# 2418
((void(*)(void(*f)(struct _tuple29*,struct Cyc_Absyn_Vardecl*),struct _tuple29*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp606,_tmp60F);
goto _LL32D;_LL334:;_LL335:
 goto _LL32D;_LL32D:;}
# 2422
goto _LL328;}_LL32B:;_LL32C:
 goto _LL328;_LL328:;}
# 2425
return;}
# 2431
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_patvars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2434
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple28*_tmp615=(struct _tuple28*)x->hd;struct Cyc_Absyn_Stmt*_tmp617;struct Cyc_List_List*_tmp618;struct Cyc_List_List*_tmp619;struct _tuple28*_tmp616=_tmp615;_tmp617=_tmp616->f1;_tmp618=_tmp616->f2;_tmp619=_tmp616->f3;
keep_going=0;{
# 2442
struct Cyc_Absyn_Stmt*_tmp61A=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp61A != _tmp617){
struct Cyc_Absyn_Stmt*_tmp61B=(Cyc_NewControlFlow_get_stmt_annot(_tmp61A))->encloser;
if(_tmp61B == _tmp61A){
# 2448
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp618,_tmp619,inflow,dest->loc);
keep_going=1;
break;}
# 2452
_tmp61A=_tmp61B;}};}}
# 2455
return inflow;}
# 2461
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2464
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp61C=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp61E;union Cyc_CfFlowInfo_FlowInfo*_tmp61F;struct _tuple20 _tmp61D=_tmp61C;_tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;
inflow=*_tmp61F;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp620=env->fenv;
# 2471
void*_tmp621=s->r;void*_tmp622=_tmp621;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Exp*_tmp629;struct Cyc_Absyn_Stmt*_tmp62B;struct Cyc_Absyn_Stmt*_tmp62C;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Stmt*_tmp62F;struct Cyc_Absyn_Stmt*_tmp630;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Stmt*_tmp633;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*_tmp636;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Stmt*_tmp63E;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_List_List*_tmp641;struct Cyc_Absyn_Switch_clause*_tmp642;struct Cyc_Absyn_Stmt*_tmp644;struct Cyc_Absyn_Stmt*_tmp646;struct Cyc_Absyn_Stmt*_tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_List_List*_tmp64B;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_List_List*_tmp64E;struct Cyc_List_List*_tmp651;struct Cyc_Absyn_Exp*_tmp652;unsigned int _tmp653;struct Cyc_Absyn_Stmt*_tmp654;struct Cyc_Absyn_Decl*_tmp656;struct Cyc_Absyn_Stmt*_tmp657;struct Cyc_Absyn_Stmt*_tmp659;struct Cyc_Absyn_Exp*_tmp65B;_LL337: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp623=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp623->tag != 0)goto _LL339;}_LL338:
 return inflow;_LL339: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp624=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp624->tag != 6)goto _LL33B;else{if(_tmp624->f1 != 0)goto _LL33B;}}_LL33A:
# 2477
{union Cyc_CfFlowInfo_FlowInfo _tmp65C=inflow;_LL362: if((_tmp65C.ReachableFL).tag != 2)goto _LL364;_LL363:
# 2479
{void*_tmp65D=Cyc_Tcutil_compress(env->return_type);void*_tmp65E=_tmp65D;_LL367: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp65F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65E;if(_tmp65F->tag != 0)goto _LL369;}_LL368:
 goto _LL366;_LL369: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp660=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65E;if(_tmp660->tag != 7)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp661=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65E;if(_tmp661->tag != 6)goto _LL36D;}_LL36C:
# 2483
{const char*_tmp9A3;void*_tmp9A2;(_tmp9A2=0,Cyc_Tcutil_warn(s->loc,((_tmp9A3="break may cause function not to return a value",_tag_dyneither(_tmp9A3,sizeof(char),47))),_tag_dyneither(_tmp9A2,sizeof(void*),0)));}
goto _LL366;_LL36D:;_LL36E:
# 2486
{const char*_tmp9A6;void*_tmp9A5;(_tmp9A5=0,Cyc_Tcutil_terr(s->loc,((_tmp9A6="break may cause function not to return a value",_tag_dyneither(_tmp9A6,sizeof(char),47))),_tag_dyneither(_tmp9A5,sizeof(void*),0)));}
goto _LL366;_LL366:;}
# 2489
goto _LL361;_LL364:;_LL365:
 goto _LL361;_LL361:;}
# 2492
goto _LL33C;_LL33B: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp625=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp625->tag != 3)goto _LL33D;else{if(_tmp625->f1 != 0)goto _LL33D;}}_LL33C:
# 2494
 if(env->noreturn){
const char*_tmp9A9;void*_tmp9A8;(_tmp9A8=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9A9="`noreturn' function might return",_tag_dyneither(_tmp9A9,sizeof(char),33))),_tag_dyneither(_tmp9A8,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL33D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp626=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp626->tag != 3)goto _LL33F;else{_tmp627=_tmp626->f1;}}_LL33E:
# 2500
 if(env->noreturn){
const char*_tmp9AC;void*_tmp9AB;(_tmp9AB=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9AC="`noreturn' function might return",_tag_dyneither(_tmp9AC,sizeof(char),33))),_tag_dyneither(_tmp9AB,sizeof(void*),0)));}{
struct _tuple16 _tmp66A=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp627));union Cyc_CfFlowInfo_FlowInfo _tmp66C;void*_tmp66D;struct _tuple16 _tmp66B=_tmp66A;_tmp66C=_tmp66B.f1;_tmp66D=_tmp66B.f2;
_tmp66C=Cyc_NewControlFlow_use_Rval(env,_tmp627->loc,_tmp66C,_tmp66D);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp66C);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,_tmp66C,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL33F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp628=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp628->tag != 1)goto _LL341;else{_tmp629=_tmp628->f1;}}_LL340: {
# 2509
struct _tuple17*_tmp66E=rval_opt;void**_tmp66F;int _tmp670;_LL370: if(_tmp66E == 0)goto _LL372;_tmp66F=(void**)& _tmp66E->f1;_tmp670=_tmp66E->f2;_LL371: {
# 2511
struct _tuple16 _tmp671=Cyc_NewControlFlow_anal_Rexp(env,_tmp670,inflow,_tmp629);union Cyc_CfFlowInfo_FlowInfo _tmp673;void*_tmp674;struct _tuple16 _tmp672=_tmp671;_tmp673=_tmp672.f1;_tmp674=_tmp672.f2;
*_tmp66F=_tmp674;
return _tmp673;}_LL372: if(_tmp66E != 0)goto _LL36F;_LL373:
# 2515
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp629)).f1;_LL36F:;}_LL341: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp62A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp62A->tag != 2)goto _LL343;else{_tmp62B=_tmp62A->f1;_tmp62C=_tmp62A->f2;}}_LL342:
# 2519
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp62B,0),_tmp62C,rval_opt);_LL343: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp62D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp62D->tag != 4)goto _LL345;else{_tmp62E=_tmp62D->f1;_tmp62F=_tmp62D->f2;_tmp630=_tmp62D->f3;}}_LL344: {
# 2522
struct _tuple19 _tmp675=Cyc_NewControlFlow_anal_test(env,inflow,_tmp62E);union Cyc_CfFlowInfo_FlowInfo _tmp677;union Cyc_CfFlowInfo_FlowInfo _tmp678;struct _tuple19 _tmp676=_tmp675;_tmp677=_tmp676.f1;_tmp678=_tmp676.f2;{
# 2529
union Cyc_CfFlowInfo_FlowInfo _tmp679=Cyc_NewControlFlow_anal_stmt(env,_tmp678,_tmp630,0);
union Cyc_CfFlowInfo_FlowInfo _tmp67A=Cyc_NewControlFlow_anal_stmt(env,_tmp677,_tmp62F,0);
return Cyc_CfFlowInfo_join_flow(_tmp620,env->all_changed,_tmp67A,_tmp679);};}_LL345: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp631=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp631->tag != 5)goto _LL347;else{_tmp632=(_tmp631->f1).f1;_tmp633=(_tmp631->f1).f2;_tmp634=_tmp631->f2;}}_LL346: {
# 2537
struct _tuple20 _tmp67B=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp633);union Cyc_CfFlowInfo_FlowInfo*_tmp67D;struct _tuple20 _tmp67C=_tmp67B;_tmp67D=_tmp67C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp67E=*_tmp67D;
struct _tuple19 _tmp67F=Cyc_NewControlFlow_anal_test(env,_tmp67E,_tmp632);union Cyc_CfFlowInfo_FlowInfo _tmp681;union Cyc_CfFlowInfo_FlowInfo _tmp682;struct _tuple19 _tmp680=_tmp67F;_tmp681=_tmp680.f1;_tmp682=_tmp680.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp683=Cyc_NewControlFlow_anal_stmt(env,_tmp681,_tmp634,0);
Cyc_NewControlFlow_update_flow(env,_tmp633,_tmp683);
return _tmp682;};};}_LL347: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp635=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp635->tag != 14)goto _LL349;else{_tmp636=_tmp635->f1;_tmp637=(_tmp635->f2).f1;_tmp638=(_tmp635->f2).f2;}}_LL348: {
# 2547
union Cyc_CfFlowInfo_FlowInfo _tmp684=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp636,0);
struct _tuple20 _tmp685=Cyc_NewControlFlow_pre_stmt_check(env,_tmp684,_tmp638);union Cyc_CfFlowInfo_FlowInfo*_tmp687;struct _tuple20 _tmp686=_tmp685;_tmp687=_tmp686.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp688=*_tmp687;
struct _tuple19 _tmp689=Cyc_NewControlFlow_anal_test(env,_tmp688,_tmp637);union Cyc_CfFlowInfo_FlowInfo _tmp68B;union Cyc_CfFlowInfo_FlowInfo _tmp68C;struct _tuple19 _tmp68A=_tmp689;_tmp68B=_tmp68A.f1;_tmp68C=_tmp68A.f2;
Cyc_NewControlFlow_update_flow(env,_tmp636,_tmp68B);
return _tmp68C;};}_LL349: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp639=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp639->tag != 9)goto _LL34B;else{_tmp63A=_tmp639->f1;_tmp63B=(_tmp639->f2).f1;_tmp63C=(_tmp639->f2).f2;_tmp63D=(_tmp639->f3).f1;_tmp63E=(_tmp639->f3).f2;_tmp63F=_tmp639->f4;}}_LL34A: {
# 2556
union Cyc_CfFlowInfo_FlowInfo _tmp68D=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp63A)).f1;
struct _tuple20 _tmp68E=Cyc_NewControlFlow_pre_stmt_check(env,_tmp68D,_tmp63C);union Cyc_CfFlowInfo_FlowInfo*_tmp690;struct _tuple20 _tmp68F=_tmp68E;_tmp690=_tmp68F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp691=*_tmp690;
struct _tuple19 _tmp692=Cyc_NewControlFlow_anal_test(env,_tmp691,_tmp63B);union Cyc_CfFlowInfo_FlowInfo _tmp694;union Cyc_CfFlowInfo_FlowInfo _tmp695;struct _tuple19 _tmp693=_tmp692;_tmp694=_tmp693.f1;_tmp695=_tmp693.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp696=Cyc_NewControlFlow_anal_stmt(env,_tmp694,_tmp63F,0);
struct _tuple20 _tmp697=Cyc_NewControlFlow_pre_stmt_check(env,_tmp696,_tmp63E);union Cyc_CfFlowInfo_FlowInfo*_tmp699;struct _tuple20 _tmp698=_tmp697;_tmp699=_tmp698.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp69A=*_tmp699;
union Cyc_CfFlowInfo_FlowInfo _tmp69B=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp69A,_tmp63D)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp63C,_tmp69B);
return _tmp695;};};};}_LL34B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp640=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp640->tag != 11)goto _LL34D;else{_tmp641=_tmp640->f1;if(_tmp640->f2 == 0)goto _LL34D;_tmp642=*_tmp640->f2;}}_LL34C: {
# 2568
struct _RegionHandle _tmp69C=_new_region("temp");struct _RegionHandle*temp=& _tmp69C;_push_region(temp);
{struct _tuple23 _tmp69D=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp641,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp69F;struct Cyc_List_List*_tmp6A0;struct _tuple23 _tmp69E=_tmp69D;_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;
# 2571
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,_tmp642->body);{
# 2573
struct Cyc_List_List*_tmp6A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp642->pat_vars))->v)).f1);
_tmp69F=Cyc_NewControlFlow_add_vars(_tmp620,_tmp69F,_tmp6A1,_tmp620->unknown_all,s->loc,0);
# 2576
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp642->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp6A2=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp6A4;struct Cyc_Absyn_Exp*_tmp6A5;struct _tuple24*_tmp6A3=_tmp6A2;_tmp6A4=_tmp6A3->f1;_tmp6A5=_tmp6A3->f2;
if(_tmp6A4 != 0){
_tmp69F=Cyc_NewControlFlow_do_initialize_var(_tmp620,env,_tmp69F,*_tmp6A4,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp641))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6A0))->hd,s->loc);
_tmp6A0=_tmp6A0->tl;
_tmp641=_tmp641->tl;}}}
# 2584
Cyc_NewControlFlow_update_flow(env,_tmp642->body,_tmp69F);{
union Cyc_CfFlowInfo_FlowInfo _tmp6A6=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp6A6;};};}
# 2569
;_pop_region(temp);}_LL34D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp643=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp643->tag != 6)goto _LL34F;else{_tmp644=_tmp643->f1;}}_LL34E:
# 2587
 _tmp646=_tmp644;goto _LL350;_LL34F: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp645=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp645->tag != 7)goto _LL351;else{_tmp646=_tmp645->f1;}}_LL350:
 _tmp648=_tmp646;goto _LL352;_LL351: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp647=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp647->tag != 8)goto _LL353;else{_tmp648=_tmp647->f2;}}_LL352:
# 2592
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp6A7=_tmp61E->encloser;
struct Cyc_Absyn_Stmt*_tmp6A8=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp648)))->encloser;
while(_tmp6A8 != _tmp6A7){
struct Cyc_Absyn_Stmt*_tmp6A9=(Cyc_NewControlFlow_get_stmt_annot(_tmp6A7))->encloser;
if(_tmp6A9 == _tmp6A7){
{const char*_tmp9AF;void*_tmp9AE;(_tmp9AE=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9AF="goto enters local scope or exception handler",_tag_dyneither(_tmp9AF,sizeof(char),45))),_tag_dyneither(_tmp9AE,sizeof(void*),0)));}
break;}
# 2601
_tmp6A7=_tmp6A9;}}
# 2605
inflow=Cyc_NewControlFlow_unconsume_patvars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp648));
# 2607
Cyc_NewControlFlow_update_flow(env,_tmp648,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL353: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp649=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp649->tag != 10)goto _LL355;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;}}_LL354:
# 2613
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp64B,_tmp64A->loc);_LL355: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp64C=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp64C->tag != 15)goto _LL357;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;}}_LL356: {
# 2618
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp64D,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2626
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2629
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp64E,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp6AC=scs_outflow;_LL375: if((_tmp6AC.BottomFL).tag != 1)goto _LL377;_LL376:
 goto _LL374;_LL377:;_LL378: {
const char*_tmp9B2;void*_tmp9B1;(_tmp9B1=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9B2="last catch clause may implicitly fallthru",_tag_dyneither(_tmp9B2,sizeof(char),42))),_tag_dyneither(_tmp9B1,sizeof(void*),0)));}_LL374:;}
# 2635
outflow=s1_outflow;
# 2637
return outflow;};};}_LL357: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp64F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp64F->tag != 12)goto _LL359;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp650=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp64F->f1)->r;if(_tmp650->tag != 2)goto _LL359;else{if(_tmp650->f2 == 0)goto _LL359;_tmp651=(struct Cyc_List_List*)(_tmp650->f2)->v;_tmp652=_tmp650->f3;}}_tmp653=(_tmp64F->f1)->loc;_tmp654=_tmp64F->f2;}}_LL358: {
# 2647
struct _tuple1 _tmp6AF=Cyc_NewControlFlow_get_unconsume_params((env->fenv)->r,_tmp651);struct Cyc_List_List*_tmp6B1;struct Cyc_List_List*_tmp6B2;struct _tuple1 _tmp6B0=_tmp6AF;_tmp6B1=_tmp6B0.f1;_tmp6B2=_tmp6B0.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp620,env,inflow,_tmp651,_tmp6B1 != 0,_tmp653,_tmp652->loc);{
struct Cyc_List_List*_tmp6B3=env->unique_pat_vars;
{struct _tuple28*_tmp9B5;struct Cyc_List_List*_tmp9B4;env->unique_pat_vars=(
(_tmp9B4=_region_malloc(env->r,sizeof(*_tmp9B4)),((_tmp9B4->hd=((_tmp9B5=_region_malloc(env->r,sizeof(*_tmp9B5)),((_tmp9B5->f1=_tmp654,((_tmp9B5->f2=_tmp6B1,((_tmp9B5->f3=_tmp6B2,_tmp9B5)))))))),((_tmp9B4->tl=_tmp6B3,_tmp9B4))))));}
# 2655
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp654,rval_opt);
env->unique_pat_vars=_tmp6B3;
# 2659
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp6B1,_tmp6B2,inflow,_tmp653);
# 2663
return inflow;};}_LL359: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp655=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp655->tag != 12)goto _LL35B;else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;}}_LL35A:
# 2747 "new_control_flow.cyc"
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp656),_tmp657,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp656);
return outflow;_LL35B: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp658=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp658->tag != 13)goto _LL35D;else{_tmp659=_tmp658->f2;}}_LL35C:
# 2752
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp659,rval_opt);_LL35D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp65A=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp622;if(_tmp65A->tag != 16)goto _LL35F;else{_tmp65B=_tmp65A->f1;}}_LL35E: {
# 2755
struct _tuple16 _tmp6B6=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp65B);union Cyc_CfFlowInfo_FlowInfo _tmp6B8;void*_tmp6B9;struct _tuple16 _tmp6B7=_tmp6B6;_tmp6B8=_tmp6B7.f1;_tmp6B9=_tmp6B7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6BA=Cyc_NewControlFlow_use_Rval(env,_tmp65B->loc,_tmp6B8,_tmp6B9);
void*_tmp6BB=Cyc_Tcutil_compress((void*)_check_null(_tmp65B->topt));void*_tmp6BC=_tmp6BB;void*_tmp6BE;_LL37A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6BC;if(_tmp6BD->tag != 15)goto _LL37C;else{_tmp6BE=(void*)_tmp6BD->f1;}}_LL37B:
# 2759
 return Cyc_CfFlowInfo_kill_flow_region(_tmp620,_tmp6B8,_tmp6BE);_LL37C:;_LL37D: {
struct Cyc_Core_Impossible_exn_struct _tmp9BB;const char*_tmp9BA;struct Cyc_Core_Impossible_exn_struct*_tmp9B9;(int)_throw((void*)((_tmp9B9=_cycalloc(sizeof(*_tmp9B9)),((_tmp9B9[0]=((_tmp9BB.tag=Cyc_Core_Impossible,((_tmp9BB.f1=((_tmp9BA="anal_stmt -- reset_region",_tag_dyneither(_tmp9BA,sizeof(char),26))),_tmp9BB)))),_tmp9B9)))));}_LL379:;};}_LL35F:;_LL360: {
# 2763
struct Cyc_Core_Impossible_exn_struct _tmp9C1;const char*_tmp9C0;struct Cyc_Core_Impossible_exn_struct*_tmp9BF;(int)_throw((void*)((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF[0]=((_tmp9C1.tag=Cyc_Core_Impossible,((_tmp9C1.f1=((_tmp9C0="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp9C0,sizeof(char),56))),_tmp9C1)))),_tmp9BF)))));}_LL336:;};}
# 2768
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2771
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp6C5=env->fenv;
void*_tmp6C6=decl->r;void*_tmp6C7=_tmp6C6;struct Cyc_Absyn_Vardecl*_tmp6C9;struct Cyc_List_List*_tmp6CB;struct Cyc_Absyn_Fndecl*_tmp6CD;struct Cyc_Absyn_Tvar*_tmp6CF;struct Cyc_Absyn_Vardecl*_tmp6D0;int _tmp6D1;struct Cyc_Absyn_Exp*_tmp6D2;_LL37F: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6C8=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6C7;if(_tmp6C8->tag != 0)goto _LL381;else{_tmp6C9=_tmp6C8->f1;}}_LL380: {
# 2781
struct Cyc_List_List _tmp9C2;struct Cyc_List_List _tmp6D3=(_tmp9C2.hd=_tmp6C9,((_tmp9C2.tl=0,_tmp9C2)));
inflow=Cyc_NewControlFlow_add_vars(_tmp6C5,inflow,& _tmp6D3,_tmp6C5->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp6D4=_tmp6C9->initializer;
if(_tmp6D4 == 0)
return inflow;{
struct _tuple16 _tmp6D5=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6D4);union Cyc_CfFlowInfo_FlowInfo _tmp6D7;void*_tmp6D8;struct _tuple16 _tmp6D6=_tmp6D5;_tmp6D7=_tmp6D6.f1;_tmp6D8=_tmp6D6.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp6C5,env,_tmp6D7,_tmp6C9,_tmp6D4,_tmp6D8,decl->loc);};};}_LL381: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp6CA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6C7;if(_tmp6CA->tag != 3)goto _LL383;else{_tmp6CB=_tmp6CA->f1;}}_LL382:
# 2790
 return Cyc_NewControlFlow_add_vars(_tmp6C5,inflow,_tmp6CB,_tmp6C5->unknown_none,decl->loc,0);_LL383: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6CC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6C7;if(_tmp6CC->tag != 1)goto _LL385;else{_tmp6CD=_tmp6CC->f1;}}_LL384:
# 2793
 Cyc_NewControlFlow_check_nested_fun(_tmp6C5,inflow,_tmp6CD);{
void*t=(void*)_check_null(_tmp6CD->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp6DA=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6CD->fn_vardecl);
# 2799
struct Cyc_List_List*_tmp9C3;return Cyc_NewControlFlow_add_vars(_tmp6C5,inflow,((_tmp9C3=_region_malloc(env->r,sizeof(*_tmp9C3)),((_tmp9C3->hd=_tmp6DA,((_tmp9C3->tl=0,_tmp9C3)))))),_tmp6C5->unknown_all,decl->loc,0);};_LL385: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp6CE=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6C7;if(_tmp6CE->tag != 4)goto _LL387;else{_tmp6CF=_tmp6CE->f1;_tmp6D0=_tmp6CE->f2;_tmp6D1=_tmp6CE->f3;_tmp6D2=_tmp6CE->f4;}}_LL386:
# 2802
 if(_tmp6D2 != 0){
struct Cyc_Absyn_Exp*_tmp6DC=_tmp6D2;
struct _tuple16 _tmp6DD=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp6DC);union Cyc_CfFlowInfo_FlowInfo _tmp6DF;void*_tmp6E0;struct _tuple16 _tmp6DE=_tmp6DD;_tmp6DF=_tmp6DE.f1;_tmp6E0=_tmp6DE.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,_tmp6DC->loc,_tmp6DF,_tmp6E0);}{
# 2807
struct Cyc_List_List _tmp9C4;struct Cyc_List_List _tmp6E1=(_tmp9C4.hd=_tmp6D0,((_tmp9C4.tl=0,_tmp9C4)));
return Cyc_NewControlFlow_add_vars(_tmp6C5,inflow,& _tmp6E1,_tmp6C5->unknown_all,decl->loc,0);};_LL387:;_LL388: {
# 2811
struct Cyc_Core_Impossible_exn_struct _tmp9CA;const char*_tmp9C9;struct Cyc_Core_Impossible_exn_struct*_tmp9C8;(int)_throw((void*)((_tmp9C8=_cycalloc(sizeof(*_tmp9C8)),((_tmp9C8[0]=((_tmp9CA.tag=Cyc_Core_Impossible,((_tmp9CA.f1=((_tmp9C9="anal_decl: unexpected decl variant",_tag_dyneither(_tmp9C9,sizeof(char),35))),_tmp9CA)))),_tmp9C8)))));}_LL37E:;}
# 2819
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp6E6=_new_region("frgn");struct _RegionHandle*frgn=& _tmp6E6;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2821
;_pop_region(frgn);}
# 2826
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2830
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2834
struct _RegionHandle*_tmp6E7=fenv->r;
unsigned int _tmp6E8=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp6E8,1);{
# 2840
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6E9=inflow;struct Cyc_Dict_Dict _tmp6EA;struct Cyc_List_List*_tmp6EB;_LL38A: if((_tmp6E9.BottomFL).tag != 1)goto _LL38C;_LL38B: {
const char*_tmp9CD;void*_tmp9CC;(_tmp9CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9CD="check_fun",_tag_dyneither(_tmp9CD,sizeof(char),10))),_tag_dyneither(_tmp9CC,sizeof(void*),0)));}_LL38C: if((_tmp6E9.ReachableFL).tag != 2)goto _LL389;_tmp6EA=((struct _tuple14)(_tmp6E9.ReachableFL).val).f1;_tmp6EB=((struct _tuple14)(_tmp6E9.ReachableFL).val).f2;_LL38D: {
# 2847
struct Cyc_List_List*atts;
{void*_tmp6EE=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp6EF=_tmp6EE;struct Cyc_List_List*_tmp6F1;_LL38F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6F0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6EF;if(_tmp6F0->tag != 9)goto _LL391;else{_tmp6F1=(_tmp6F0->f1).attributes;}}_LL390:
 atts=_tmp6F1;goto _LL38E;_LL391:;_LL392: {
const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9D0="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmp9D0,sizeof(char),50))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}_LL38E:;}
# 2854
for(0;atts != 0;atts=atts->tl){
void*_tmp6F4=(void*)atts->hd;void*_tmp6F5=_tmp6F4;int _tmp6F7;int _tmp6F9;int _tmp6FB;_LL394: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp6F6=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp6F5;if(_tmp6F6->tag != 21)goto _LL396;else{_tmp6F7=_tmp6F6->f1;}}_LL395: {
# 2857
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp6FC=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6F7 - 1);
void*t=Cyc_Tcutil_compress(_tmp6FC->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp6FD=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9D3;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9D2;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp6FE=(_tmp9D2=_region_malloc(_tmp6E7,sizeof(*_tmp9D2)),((_tmp9D2[0]=((_tmp9D3.tag=2,((_tmp9D3.f1=_tmp6F7,((_tmp9D3.f2=t,_tmp9D3)))))),_tmp9D2)));
struct Cyc_CfFlowInfo_Place*_tmp9D4;struct Cyc_CfFlowInfo_Place*_tmp6FF=(_tmp9D4=_region_malloc(_tmp6E7,sizeof(*_tmp9D4)),((_tmp9D4->root=(void*)_tmp6FE,((_tmp9D4->fields=0,_tmp9D4)))));
_tmp6EA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6EA,(void*)_tmp6FE,_tmp6FD);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9DA;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9D9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9D7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9D6;_tmp6EA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6EA,(void*)((_tmp9D6=_region_malloc(_tmp6E7,sizeof(*_tmp9D6)),((_tmp9D6[0]=((_tmp9D7.tag=0,((_tmp9D7.f1=_tmp6FC,_tmp9D7)))),_tmp9D6)))),(void*)((_tmp9D9=_region_malloc(_tmp6E7,sizeof(*_tmp9D9)),((_tmp9D9[0]=((_tmp9DA.tag=5,((_tmp9DA.f1=_tmp6FF,_tmp9DA)))),_tmp9D9)))));}
goto _LL393;}_LL396: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp6F8=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6F5;if(_tmp6F8->tag != 20)goto _LL398;else{_tmp6F9=_tmp6F8->f1;}}_LL397: {
# 2870
struct Cyc_Absyn_Vardecl*_tmp707=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6F9 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp707->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmp9DD;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp9DC;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp708=(_tmp9DC=_region_malloc(_tmp6E7,sizeof(*_tmp9DC)),((_tmp9DC[0]=((_tmp9DD.tag=2,((_tmp9DD.f1=_tmp6F9,((_tmp9DD.f2=elttype,_tmp9DD)))))),_tmp9DC)));
struct Cyc_CfFlowInfo_Place*_tmp9DE;struct Cyc_CfFlowInfo_Place*_tmp709=(_tmp9DE=_region_malloc(_tmp6E7,sizeof(*_tmp9DE)),((_tmp9DE->root=(void*)_tmp708,((_tmp9DE->fields=0,_tmp9DE)))));
_tmp6EA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6EA,(void*)_tmp708,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp9E4;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp9E3;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9E1;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9E0;_tmp6EA=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp6EA,(void*)((_tmp9E0=_region_malloc(_tmp6E7,sizeof(*_tmp9E0)),((_tmp9E0[0]=((_tmp9E1.tag=0,((_tmp9E1.f1=_tmp707,_tmp9E1)))),_tmp9E0)))),(void*)((_tmp9E3=_region_malloc(_tmp6E7,sizeof(*_tmp9E3)),((_tmp9E3[0]=((_tmp9E4.tag=5,((_tmp9E4.f1=_tmp709,_tmp9E4)))),_tmp9E3)))));}
{struct Cyc_List_List*_tmp9E5;param_roots=((_tmp9E5=_region_malloc(_tmp6E7,sizeof(*_tmp9E5)),((_tmp9E5->hd=_tmp709,((_tmp9E5->tl=param_roots,_tmp9E5))))));}
goto _LL393;}_LL398: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp6FA=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp6F5;if(_tmp6FA->tag != 22)goto _LL39A;else{_tmp6FB=_tmp6FA->f1;}}_LL399: {
# 2879
struct Cyc_Absyn_Vardecl*_tmp712=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp6FB - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp712->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9E8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9E7;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp713=(_tmp9E7=_region_malloc(_tmp6E7,sizeof(*_tmp9E7)),((_tmp9E7[0]=((_tmp9E8.tag=0,((_tmp9E8.f1=_tmp712,_tmp9E8)))),_tmp9E7)));
struct Cyc_CfFlowInfo_Place*_tmp9E9;struct Cyc_CfFlowInfo_Place*_tmp714=(_tmp9E9=_region_malloc(_tmp6E7,sizeof(*_tmp9E9)),((_tmp9E9->root=(void*)_tmp713,((_tmp9E9->fields=0,_tmp9E9)))));
struct Cyc_List_List*_tmp9EA;noconsume_roots=((_tmp9EA=_region_malloc(_tmp6E7,sizeof(*_tmp9EA)),((_tmp9EA->hd=_tmp714,((_tmp9EA->tl=noconsume_roots,_tmp9EA))))));}
# 2885
goto _LL393;}_LL39A:;_LL39B:
 goto _LL393;_LL393:;}
# 2888
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp6EA,_tmp6EB);}_LL389:;}{
# 2891
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp6E7,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmp9EB;struct Cyc_NewControlFlow_AnalEnv*env=(_tmp9EB=_region_malloc(_tmp6E7,sizeof(*_tmp9EB)),((_tmp9EB->r=_tmp6E7,((_tmp9EB->fenv=fenv,((_tmp9EB->iterate_again=1,((_tmp9EB->iteration_num=0,((_tmp9EB->in_try=0,((_tmp9EB->tryflow=inflow,((_tmp9EB->all_changed=0,((_tmp9EB->noreturn=noreturn,((_tmp9EB->return_type=fd->ret_type,((_tmp9EB->unique_pat_vars=0,((_tmp9EB->param_roots=param_roots,((_tmp9EB->noconsume_params=noconsume_roots,((_tmp9EB->flow_table=flow_table,_tmp9EB)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2901
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2904
union Cyc_CfFlowInfo_FlowInfo _tmp719=outflow;_LL39D: if((_tmp719.BottomFL).tag != 1)goto _LL39F;_LL39E:
 goto _LL39C;_LL39F:;_LL3A0:
# 2907
 Cyc_NewControlFlow_check_init_params(_tmp6E8,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,outflow,_tmp6E8);
# 2911
if(noreturn){
const char*_tmp9EE;void*_tmp9ED;(_tmp9ED=0,Cyc_CfFlowInfo_aerr(_tmp6E8,((_tmp9EE="`noreturn' function might (implicitly) return",_tag_dyneither(_tmp9EE,sizeof(char),46))),_tag_dyneither(_tmp9ED,sizeof(void*),0)));}else{
# 2914
void*_tmp71C=Cyc_Tcutil_compress(fd->ret_type);void*_tmp71D=_tmp71C;_LL3A2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp71E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp71D;if(_tmp71E->tag != 0)goto _LL3A4;}_LL3A3:
 goto _LL3A1;_LL3A4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp71F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp71D;if(_tmp71F->tag != 7)goto _LL3A6;}_LL3A5:
 goto _LL3A7;_LL3A6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp71D;if(_tmp720->tag != 6)goto _LL3A8;}_LL3A7:
# 2918
{const char*_tmp9F1;void*_tmp9F0;(_tmp9F0=0,Cyc_Tcutil_warn(_tmp6E8,((_tmp9F1="function may not return a value",_tag_dyneither(_tmp9F1,sizeof(char),32))),_tag_dyneither(_tmp9F0,sizeof(void*),0)));}goto _LL3A1;_LL3A8:;_LL3A9:
# 2920
{const char*_tmp9F4;void*_tmp9F3;(_tmp9F3=0,Cyc_CfFlowInfo_aerr(_tmp6E8,((_tmp9F4="function may not return a value",_tag_dyneither(_tmp9F4,sizeof(char),32))),_tag_dyneither(_tmp9F3,sizeof(void*),0)));}goto _LL3A1;_LL3A1:;}
# 2922
goto _LL39C;_LL39C:;};};};}
# 2926
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp726=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp727=_tmp726;struct Cyc_Absyn_Fndecl*_tmp729;struct Cyc_List_List*_tmp72B;struct Cyc_List_List*_tmp72D;struct Cyc_List_List*_tmp72F;_LL3AB: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp728=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp727;if(_tmp728->tag != 1)goto _LL3AD;else{_tmp729=_tmp728->f1;}}_LL3AC:
 Cyc_NewControlFlow_check_fun(_tmp729);goto _LL3AA;_LL3AD: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp72A=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp727;if(_tmp72A->tag != 11)goto _LL3AF;else{_tmp72B=_tmp72A->f1;}}_LL3AE:
 _tmp72D=_tmp72B;goto _LL3B0;_LL3AF: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp72C=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp727;if(_tmp72C->tag != 10)goto _LL3B1;else{_tmp72D=_tmp72C->f2;}}_LL3B0:
 _tmp72F=_tmp72D;goto _LL3B2;_LL3B1: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp72E=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp727;if(_tmp72E->tag != 9)goto _LL3B3;else{_tmp72F=_tmp72E->f2;}}_LL3B2:
 Cyc_NewControlFlow_cf_check(_tmp72F);goto _LL3AA;_LL3B3: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp730=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp727;if(_tmp730->tag != 12)goto _LL3B5;}_LL3B4:
 goto _LL3AA;_LL3B5:;_LL3B6:
 goto _LL3AA;_LL3AA:;};}}
