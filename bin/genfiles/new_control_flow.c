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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);
# 128
int Cyc_Core_ptrcmp(void*,void*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 969 "absyn.h"
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1150
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1162
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1166
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1168
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
# 67 "new_control_flow.cyc"
typedef struct Cyc_Dict_Dict Cyc_NewControlFlow_dict_t;
typedef unsigned int Cyc_NewControlFlow_seg_t;
# 71
int Cyc_NewControlFlow_warn_lose_unique=0;struct Cyc_NewControlFlow_CFStmtAnnot{struct Cyc_Absyn_Stmt*encloser;int visited;};
# 79
typedef struct Cyc_NewControlFlow_CFStmtAnnot Cyc_NewControlFlow_cf_stmt_annot_t;static char Cyc_NewControlFlow_CFAnnot[8]="CFAnnot";struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_NewControlFlow_CFStmtAnnot f1;};
# 84
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser){
struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct _tmp761;struct Cyc_NewControlFlow_CFStmtAnnot _tmp760;struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp75F;enclosee->annot=(void*)((_tmp75F=_cycalloc(sizeof(*_tmp75F)),((_tmp75F[0]=((_tmp761.tag=Cyc_NewControlFlow_CFAnnot,((_tmp761.f1=((_tmp760.encloser=encloser,((_tmp760.visited=0,_tmp760)))),_tmp761)))),_tmp75F))));}struct Cyc_NewControlFlow_AnalEnv{struct _RegionHandle*r;struct Cyc_CfFlowInfo_FlowEnv*fenv;int iterate_again;int iteration_num;int in_try;union Cyc_CfFlowInfo_FlowInfo tryflow;struct Cyc_Dict_Dict*all_changed;int noreturn;void*return_type;struct Cyc_List_List*unique_pat_vars;struct Cyc_List_List*param_roots;struct Cyc_List_List*noconsume_params;struct Cyc_Hashtable_Table*flow_table;};
# 116 "new_control_flow.cyc"
typedef struct Cyc_NewControlFlow_AnalEnv*Cyc_NewControlFlow_analenv_t;struct _tuple17{void*f1;int f2;};
# 124
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Stmt*,struct _tuple17*);
# 126
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Decl*);struct _tuple18{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_AbsLVal f2;};
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*);
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);struct _tuple19{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*,union Cyc_CfFlowInfo_FlowInfo,struct Cyc_Absyn_Exp*);
static struct Cyc_List_List*Cyc_NewControlFlow_noalias_ptrs_rec(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*p,void*t);
# 133
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es);
# 136
static struct Cyc_NewControlFlow_CFStmtAnnot*Cyc_NewControlFlow_get_stmt_annot(struct Cyc_Absyn_Stmt*s){
void*_tmp3=s->annot;void*_tmp4=_tmp3;struct Cyc_NewControlFlow_CFStmtAnnot*_tmp6;_LL1: {struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*_tmp5=(struct Cyc_NewControlFlow_CFAnnot_Absyn_AbsynAnnot_struct*)_tmp4;if(_tmp5->tag != Cyc_NewControlFlow_CFAnnot)goto _LL3;else{_tmp6=(struct Cyc_NewControlFlow_CFStmtAnnot*)& _tmp5->f1;}}_LL2:
 return _tmp6;_LL3:;_LL4: {
struct Cyc_Core_Impossible_exn_struct _tmp767;const char*_tmp766;struct Cyc_Core_Impossible_exn_struct*_tmp765;(int)_throw((void*)((_tmp765=_cycalloc(sizeof(*_tmp765)),((_tmp765[0]=((_tmp767.tag=Cyc_Core_Impossible,((_tmp767.f1=((_tmp766="ControlFlow -- wrong stmt annotation",_tag_dyneither(_tmp766,sizeof(char),37))),_tmp767)))),_tmp765)))));}_LL0:;}
# 143
static union Cyc_CfFlowInfo_FlowInfo*Cyc_NewControlFlow_get_stmt_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s){
union Cyc_CfFlowInfo_FlowInfo**sflow=((union Cyc_CfFlowInfo_FlowInfo**(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup_opt)(env->flow_table,s);
if(sflow == 0){
union Cyc_CfFlowInfo_FlowInfo*_tmp768;union Cyc_CfFlowInfo_FlowInfo*res=(_tmp768=_region_malloc(env->r,sizeof(*_tmp768)),((_tmp768[0]=Cyc_CfFlowInfo_BottomFL(),_tmp768)));
((void(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key,union Cyc_CfFlowInfo_FlowInfo*val))Cyc_Hashtable_insert)(env->flow_table,s,res);
return res;}
# 150
return*sflow;}struct _tuple20{struct Cyc_NewControlFlow_CFStmtAnnot*f1;union Cyc_CfFlowInfo_FlowInfo*f2;};
# 153
static struct _tuple20 Cyc_NewControlFlow_pre_stmt_check(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmpB=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmpC=Cyc_NewControlFlow_get_stmt_flow(env,s);
# 161
*_tmpC=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,inflow,*_tmpC);
# 167
_tmpB->visited=env->iteration_num;{
struct _tuple20 _tmp769;return(_tmp769.f1=_tmpB,((_tmp769.f2=_tmpC,_tmp769)));};}
# 177
static void Cyc_NewControlFlow_update_tryflow(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo new_flow){
if(env->in_try)
# 185
env->tryflow=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,new_flow,env->tryflow);}struct _tuple21{struct Cyc_NewControlFlow_AnalEnv*f1;unsigned int f2;struct Cyc_Dict_Dict f3;};
# 192
static void Cyc_NewControlFlow_check_unique_root(struct _tuple21*ckenv,void*root,void*rval){
# 194
struct Cyc_NewControlFlow_AnalEnv*_tmpF;unsigned int _tmp10;struct Cyc_Dict_Dict _tmp11;struct _tuple21*_tmpE=ckenv;_tmpF=_tmpE->f1;_tmp10=_tmpE->f2;_tmp11=_tmpE->f3;{
void*_tmp12=root;struct Cyc_Absyn_Vardecl*_tmp14;_LL6: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp13=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp12;if(_tmp13->tag != 0)goto _LL8;else{_tmp14=_tmp13->f1;}}_LL7:
# 197
 if(!((int(*)(struct Cyc_Dict_Dict d,void*k,void**ans))Cyc_Dict_lookup_bool)(_tmp11,root,& rval) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmpF->fenv)->r,_tmp14->type)){
retry: {void*_tmp15=rval;void*_tmp17;_LLB: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp16=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp16->tag != 8)goto _LLD;else{_tmp17=(void*)_tmp16->f2;}}_LLC:
 rval=_tmp17;goto retry;_LLD: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp18=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp18->tag != 7)goto _LLF;}_LLE:
 goto _LL10;_LLF: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp19=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp19->tag != 3)goto _LL11;else{if(_tmp19->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL11;}}_LL10:
 goto _LL12;_LL11: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp1A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp15;if(_tmp1A->tag != 0)goto _LL13;}_LL12:
 goto _LLA;_LL13:;_LL14:
# 206
{const char*_tmp76D;void*_tmp76C[1];struct Cyc_String_pa_PrintArg_struct _tmp76B;(_tmp76B.tag=0,((_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp14->name)),((_tmp76C[0]=& _tmp76B,Cyc_Tcutil_warn(_tmp10,((_tmp76D="alias-free pointer(s) reachable from %s may become unreachable.",_tag_dyneither(_tmp76D,sizeof(char),64))),_tag_dyneither(_tmp76C,sizeof(void*),1)))))));}
goto _LLA;_LLA:;}}
# 210
goto _LL5;_LL8:;_LL9:
 goto _LL5;_LL5:;};}
# 218
static void Cyc_NewControlFlow_update_flow(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Stmt*s,union Cyc_CfFlowInfo_FlowInfo flow){
struct Cyc_NewControlFlow_CFStmtAnnot*_tmp1E=Cyc_NewControlFlow_get_stmt_annot(s);
union Cyc_CfFlowInfo_FlowInfo*_tmp1F=Cyc_NewControlFlow_get_stmt_flow(env,s);
union Cyc_CfFlowInfo_FlowInfo _tmp20=Cyc_CfFlowInfo_join_flow(env->fenv,env->all_changed,flow,*_tmp1F);
# 224
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple19 _tmp76E;struct _tuple19 _tmp21=(_tmp76E.f1=flow,((_tmp76E.f2=_tmp20,_tmp76E)));struct _tuple19 _tmp22=_tmp21;struct Cyc_Dict_Dict _tmp23;struct Cyc_Dict_Dict _tmp24;_LL16: if(((_tmp22.f1).ReachableFL).tag != 2)goto _LL18;_tmp23=((struct _tuple14)((_tmp22.f1).ReachableFL).val).f1;if(((_tmp22.f2).ReachableFL).tag != 2)goto _LL18;_tmp24=((struct _tuple14)((_tmp22.f2).ReachableFL).val).f1;_LL17: {
# 227
struct _tuple21 _tmp76F;struct _tuple21 _tmp25=(_tmp76F.f1=env,((_tmp76F.f2=s->loc,((_tmp76F.f3=_tmp24,_tmp76F)))));
((void(*)(void(*f)(struct _tuple21*,void*,void*),struct _tuple21*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_unique_root,& _tmp25,_tmp23);
goto _LL15;}_LL18:;_LL19:
 goto _LL15;_LL15:;}
# 233
if(!Cyc_CfFlowInfo_flow_lessthan_approx(_tmp20,*_tmp1F)){
# 240
*_tmp1F=_tmp20;
# 244
if(_tmp1E->visited == env->iteration_num)
# 246
env->iterate_again=1;}}
# 251
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_add_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,void*leafval,unsigned int loc,int nameit){
# 255
union Cyc_CfFlowInfo_FlowInfo _tmp28=inflow;struct Cyc_Dict_Dict _tmp29;struct Cyc_List_List*_tmp2A;_LL1B: if((_tmp28.BottomFL).tag != 1)goto _LL1D;_LL1C:
 return Cyc_CfFlowInfo_BottomFL();_LL1D: if((_tmp28.ReachableFL).tag != 2)goto _LL1A;_tmp29=((struct _tuple14)(_tmp28.ReachableFL).val).f1;_tmp2A=((struct _tuple14)(_tmp28.ReachableFL).val).f2;_LL1E:
# 258
 for(0;vds != 0;vds=vds->tl){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp772;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp771;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp2B=(_tmp771=_region_malloc(fenv->r,sizeof(*_tmp771)),((_tmp771[0]=((_tmp772.tag=0,((_tmp772.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,_tmp772)))),_tmp771)));
void*_tmp2C=Cyc_CfFlowInfo_typ_to_absrval(fenv,((struct Cyc_Absyn_Vardecl*)vds->hd)->type,0,leafval);
if(nameit){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp775;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp774;_tmp2C=(void*)((_tmp774=_region_malloc(fenv->r,sizeof(*_tmp774)),((_tmp774[0]=((_tmp775.tag=8,((_tmp775.f1=(struct Cyc_Absyn_Vardecl*)vds->hd,((_tmp775.f2=_tmp2C,_tmp775)))))),_tmp774))));}
# 265
_tmp29=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp29,(void*)_tmp2B,_tmp2C);}
# 267
return Cyc_CfFlowInfo_ReachableFL(_tmp29,_tmp2A);_LL1A:;}
# 271
static int Cyc_NewControlFlow_relns_ok(struct _RegionHandle*r,struct Cyc_List_List*assume,struct Cyc_List_List*req){
# 278
for(0;(unsigned int)req;req=req->tl){
struct Cyc_Relations_Reln*_tmp31=Cyc_Relations_negate(r,(struct Cyc_Relations_Reln*)req->hd);
struct Cyc_List_List*_tmp776;if(Cyc_Relations_consistent_relations(((_tmp776=_region_malloc(r,sizeof(*_tmp776)),((_tmp776->hd=_tmp31,((_tmp776->tl=assume,_tmp776))))))))
return 0;}
# 283
return 1;}
# 286
static struct Cyc_Absyn_Exp*Cyc_NewControlFlow_strip_cast(struct Cyc_Absyn_Exp*e){
void*_tmp33=e->r;void*_tmp34=_tmp33;struct Cyc_Absyn_Exp*_tmp36;_LL20: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp35=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp34;if(_tmp35->tag != 13)goto _LL22;else{_tmp36=_tmp35->f2;}}_LL21:
 return _tmp36;_LL22:;_LL23:
 return e;_LL1F:;}
# 293
static void Cyc_NewControlFlow_check_requires(unsigned int loc,struct _RegionHandle*r,void*t,struct _dyneither_ptr*f,union Cyc_CfFlowInfo_FlowInfo inflow){
# 295
union Cyc_CfFlowInfo_FlowInfo _tmp37=inflow;struct Cyc_List_List*_tmp38;_LL25: if((_tmp37.BottomFL).tag != 1)goto _LL27;_LL26:
 return;_LL27: if((_tmp37.ReachableFL).tag != 2)goto _LL24;_tmp38=((struct _tuple14)(_tmp37.ReachableFL).val).f2;_LL28:
# 298
{void*_tmp39=Cyc_Tcutil_compress(t);void*_tmp3A=_tmp39;union Cyc_Absyn_AggrInfoU _tmp3C;struct Cyc_List_List*_tmp3D;struct Cyc_List_List*_tmp3F;_LL2A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3B->tag != 11)goto _LL2C;else{_tmp3C=(_tmp3B->f1).aggr_info;_tmp3D=(_tmp3B->f1).targs;}}_LL2B: {
# 300
struct Cyc_Absyn_Aggrdecl*_tmp40=Cyc_Absyn_get_known_aggrdecl(_tmp3C);
struct Cyc_Absyn_Aggrfield*_tmp41=Cyc_Absyn_lookup_decl_field(_tmp40,f);
struct Cyc_Absyn_Exp*_tmp42=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp41))->requires_clause;
if(_tmp42 != 0){
struct _RegionHandle _tmp43=_new_region("temp");struct _RegionHandle*temp=& _tmp43;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp44=Cyc_Tcutil_rsubsexp(temp,((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp40->tvs,_tmp3D),_tmp42);
# 307
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp44))){
{const char*_tmp77B;void*_tmp77A[2];struct Cyc_String_pa_PrintArg_struct _tmp779;struct Cyc_String_pa_PrintArg_struct _tmp778;(_tmp778.tag=0,((_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp779.tag=0,((_tmp779.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp44))),((_tmp77A[0]=& _tmp779,((_tmp77A[1]=& _tmp778,Cyc_CfFlowInfo_aerr(loc,((_tmp77B="unable to prove %s, required to access %s",_tag_dyneither(_tmp77B,sizeof(char),42))),_tag_dyneither(_tmp77A,sizeof(void*),2)))))))))))));}
{const char*_tmp77E;void*_tmp77D;(_tmp77D=0,Cyc_fprintf(Cyc_stderr,((_tmp77E="  [recorded facts on path: ",_tag_dyneither(_tmp77E,sizeof(char),28))),_tag_dyneither(_tmp77D,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp781;void*_tmp780;(_tmp780=0,Cyc_fprintf(Cyc_stderr,((_tmp781="]\n",_tag_dyneither(_tmp781,sizeof(char),3))),_tag_dyneither(_tmp780,sizeof(void*),0)));};}}
# 305
;_pop_region(temp);}
# 314
goto _LL29;}_LL2C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A;if(_tmp3E->tag != 12)goto _LL2E;else{_tmp3F=_tmp3E->f2;}}_LL2D: {
# 316
struct Cyc_Absyn_Aggrfield*_tmp4D=Cyc_Absyn_lookup_field(_tmp3F,f);
struct Cyc_Absyn_Exp*_tmp4E=((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp4D))->requires_clause;
if(_tmp4E != 0){
struct _RegionHandle _tmp4F=_new_region("temp");struct _RegionHandle*temp=& _tmp4F;_push_region(temp);
if(!Cyc_NewControlFlow_relns_ok(r,_tmp38,Cyc_Relations_exp2relns(temp,_tmp4E))){
{const char*_tmp786;void*_tmp785[2];struct Cyc_String_pa_PrintArg_struct _tmp784;struct Cyc_String_pa_PrintArg_struct _tmp783;(_tmp783.tag=0,((_tmp783.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(Cyc_NewControlFlow_strip_cast(_tmp4E))),((_tmp785[0]=& _tmp784,((_tmp785[1]=& _tmp783,Cyc_CfFlowInfo_aerr(loc,((_tmp786="unable to prove %s, required to access %s",_tag_dyneither(_tmp786,sizeof(char),42))),_tag_dyneither(_tmp785,sizeof(void*),2)))))))))))));}
{const char*_tmp789;void*_tmp788;(_tmp788=0,Cyc_fprintf(Cyc_stderr,((_tmp789="  [recorded facts on path: ",_tag_dyneither(_tmp789,sizeof(char),28))),_tag_dyneither(_tmp788,sizeof(void*),0)));}
Cyc_Relations_print_relns(Cyc_stderr,_tmp38);{
const char*_tmp78C;void*_tmp78B;(_tmp78B=0,Cyc_fprintf(Cyc_stderr,((_tmp78C="]\n",_tag_dyneither(_tmp78C,sizeof(char),3))),_tag_dyneither(_tmp78B,sizeof(void*),0)));};}
# 320
;_pop_region(temp);}
# 327
goto _LL29;}_LL2E:;_LL2F:
 goto _LL29;_LL29:;}
# 330
goto _LL24;_LL24:;}
# 334
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp58=inflow;struct Cyc_Dict_Dict _tmp59;struct Cyc_List_List*_tmp5A;_LL31: if((_tmp58.BottomFL).tag != 1)goto _LL33;_LL32:
 return Cyc_CfFlowInfo_BottomFL();_LL33: if((_tmp58.ReachableFL).tag != 2)goto _LL30;_tmp59=((struct _tuple14)(_tmp58.ReachableFL).val).f1;_tmp5A=((struct _tuple14)(_tmp58.ReachableFL).val).f2;_LL34:
# 338
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp59,r)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp78F;void*_tmp78E;(_tmp78E=0,Cyc_CfFlowInfo_aerr(loc,((_tmp78F="expression may not be fully initialized",_tag_dyneither(_tmp78F,sizeof(char),40))),_tag_dyneither(_tmp78E,sizeof(void*),0)));}{
struct Cyc_Dict_Dict _tmp5D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp59,env->all_changed,r);
if(_tmp59.t == _tmp5D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp5E=Cyc_CfFlowInfo_ReachableFL(_tmp5D,_tmp5A);
Cyc_NewControlFlow_update_tryflow(env,_tmp5E);
return _tmp5E;};};_LL30:;}struct _tuple22{struct Cyc_Absyn_Tqual f1;void*f2;};
# 348
static void Cyc_NewControlFlow_check_nounique(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,void*r){
struct _tuple0 _tmp790;struct _tuple0 _tmp5F=(_tmp790.f1=Cyc_Tcutil_compress(t),((_tmp790.f2=r,_tmp790)));struct _tuple0 _tmp60=_tmp5F;void*_tmp65;struct Cyc_Absyn_Datatypefield*_tmp67;struct _dyneither_ptr _tmp69;struct Cyc_List_List*_tmp6B;struct _dyneither_ptr _tmp6D;union Cyc_Absyn_AggrInfoU _tmp6F;struct Cyc_List_List*_tmp70;struct _dyneither_ptr _tmp72;enum Cyc_Absyn_AggrKind _tmp74;struct Cyc_List_List*_tmp75;struct _dyneither_ptr _tmp77;_LL36: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp61=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp61->tag != 3)goto _LL38;else{if(_tmp61->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL38;}}_LL37:
 return;_LL38: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp62=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp62->tag != 0)goto _LL3A;}_LL39:
 return;_LL3A: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp63=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp63->tag != 7)goto _LL3C;}_LL3B:
 return;_LL3C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp64=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp64->tag != 8)goto _LL3E;else{_tmp65=(void*)_tmp64->f2;}}_LL3D:
 Cyc_NewControlFlow_check_nounique(env,loc,t,_tmp65);return;_LL3E:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp60.f1;if(_tmp66->tag != 4)goto _LL40;else{if((((_tmp66->f1).field_info).KnownDatatypefield).tag != 2)goto _LL40;_tmp67=((struct _tuple3)(((_tmp66->f1).field_info).KnownDatatypefield).val).f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp68=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp68->tag != 6)goto _LL40;else{_tmp69=_tmp68->f2;}};_LL3F:
# 355
 if(_tmp67->typs == 0)
return;
_tmp6B=_tmp67->typs;_tmp6D=_tmp69;goto _LL41;_LL40:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6A->tag != 10)goto _LL42;else{_tmp6B=_tmp6A->f1;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6C=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp6C->tag != 6)goto _LL42;else{_tmp6D=_tmp6C->f2;}};_LL41: {
# 359
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B);
{int i=0;for(0;i < sz;(i ++,_tmp6B=_tmp6B->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,(*((struct _tuple22*)((struct Cyc_List_List*)_check_null(_tmp6B))->hd)).f2,*((void**)_check_dyneither_subscript(_tmp6D,sizeof(void*),i)));}}
# 363
return;}_LL42:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp6E->tag != 11)goto _LL44;else{_tmp6F=(_tmp6E->f1).aggr_info;_tmp70=(_tmp6E->f1).targs;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp71=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp71->tag != 6)goto _LL44;else{_tmp72=_tmp71->f2;}};_LL43: {
# 366
struct Cyc_Absyn_Aggrdecl*_tmp79=Cyc_Absyn_get_known_aggrdecl(_tmp6F);
if(_tmp79->impl == 0)return;{
struct Cyc_List_List*_tmp7A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp79->impl))->fields;
struct _RegionHandle _tmp7B=_new_region("temp");struct _RegionHandle*temp=& _tmp7B;_push_region(temp);
{struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,_tmp79->tvs,_tmp70);
{int i=0;for(0;i < _get_dyneither_size(_tmp72,sizeof(void*));(i ++,_tmp7A=_tmp7A->tl)){
void*t=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp7A))->hd)->type;
if(_tmp7C != 0)t=Cyc_Tcutil_rsubstitute(temp,_tmp7C,t);
Cyc_NewControlFlow_check_nounique(env,loc,t,((void**)_tmp72.curr)[i]);}}
# 376
_npop_handler(0);return;}
# 370
;_pop_region(temp);};}_LL44:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp73=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp60.f1;if(_tmp73->tag != 12)goto _LL46;else{_tmp74=_tmp73->f1;_tmp75=_tmp73->f2;}}{struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp76=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp60.f2;if(_tmp76->tag != 6)goto _LL46;else{_tmp77=_tmp76->f2;}};_LL45:
# 379
{int i=0;for(0;i < _get_dyneither_size(_tmp77,sizeof(void*));(i ++,_tmp75=_tmp75->tl)){
Cyc_NewControlFlow_check_nounique(env,loc,((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp75))->hd)->type,((void**)_tmp77.curr)[i]);}}
return;_LL46:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60.f1;if(_tmp78->tag != 5)goto _LL48;}if(!
Cyc_Tcutil_is_noalias_pointer(t,0))goto _LL48;_LL47:
{const char*_tmp793;void*_tmp792;(_tmp792=0,Cyc_Tcutil_warn(loc,((_tmp793="argument may still contain alias-free pointers",_tag_dyneither(_tmp793,sizeof(char),47))),_tag_dyneither(_tmp792,sizeof(void*),0)));}
return;_LL48:;_LL49:
 return;_LL35:;}
# 389
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_use_nounique_Rval(struct Cyc_NewControlFlow_AnalEnv*env,unsigned int loc,void*t,union Cyc_CfFlowInfo_FlowInfo inflow,void*r){
union Cyc_CfFlowInfo_FlowInfo _tmp80=inflow;struct Cyc_Dict_Dict _tmp81;struct Cyc_List_List*_tmp82;_LL4B: if((_tmp80.BottomFL).tag != 1)goto _LL4D;_LL4C:
 return Cyc_CfFlowInfo_BottomFL();_LL4D: if((_tmp80.ReachableFL).tag != 2)goto _LL4A;_tmp81=((struct _tuple14)(_tmp80.ReachableFL).val).f1;_tmp82=((struct _tuple14)(_tmp80.ReachableFL).val).f2;_LL4E:
# 393
 if(!Cyc_Tcutil_is_noalias_pointer(t,0)){
{const char*_tmp796;void*_tmp795;(_tmp795=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp796="noliveunique attribute requires alias-free pointer",_tag_dyneither(_tmp796,sizeof(char),51))),_tag_dyneither(_tmp795,sizeof(void*),0)));}
return Cyc_CfFlowInfo_BottomFL();}{
# 397
void*_tmp85=Cyc_Tcutil_pointer_elt_type(t);
retry: {void*_tmp86=r;void*_tmp88;struct Cyc_CfFlowInfo_Place*_tmp8A;_LL50: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp87=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp87->tag != 8)goto _LL52;else{_tmp88=(void*)_tmp87->f2;}}_LL51:
 r=_tmp88;goto retry;_LL52: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp89=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp86;if(_tmp89->tag != 5)goto _LL54;else{_tmp8A=_tmp89->f1;}}_LL53:
# 401
 Cyc_NewControlFlow_check_nounique(env,loc,_tmp85,Cyc_CfFlowInfo_lookup_place(_tmp81,_tmp8A));
goto _LL4F;_LL54:;_LL55:
# 404
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,_tmp85)){
const char*_tmp799;void*_tmp798;(_tmp798=0,Cyc_Tcutil_warn(loc,((_tmp799="argument may contain live alias-free pointers",_tag_dyneither(_tmp799,sizeof(char),46))),_tag_dyneither(_tmp798,sizeof(void*),0)));}
return Cyc_NewControlFlow_use_Rval(env,loc,inflow,r);_LL4F:;}{
# 408
struct Cyc_Dict_Dict _tmp8D=Cyc_CfFlowInfo_escape_deref(env->fenv,_tmp81,env->all_changed,r);
if(_tmp81.t == _tmp8D.t)return inflow;{
union Cyc_CfFlowInfo_FlowInfo _tmp8E=Cyc_CfFlowInfo_ReachableFL(_tmp8D,_tmp82);
Cyc_NewControlFlow_update_tryflow(env,_tmp8E);
return _tmp8E;};};};_LL4A:;}struct _tuple23{union Cyc_CfFlowInfo_FlowInfo f1;struct Cyc_List_List*f2;};
# 417
static struct _tuple23 Cyc_NewControlFlow_anal_unordered_Rexps(struct _RegionHandle*rgn,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es,int first_is_copy,int others_are_copy){
# 424
if(es == 0){
struct _tuple23 _tmp79A;return(_tmp79A.f1=inflow,((_tmp79A.f2=0,_tmp79A)));}
if(es->tl == 0){
struct _tuple16 _tmp90=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp92;void*_tmp93;struct _tuple16 _tmp91=_tmp90;_tmp92=_tmp91.f1;_tmp93=_tmp91.f2;{
struct Cyc_List_List*_tmp79D;struct _tuple23 _tmp79C;return(_tmp79C.f1=_tmp92,((_tmp79C.f2=((_tmp79D=_region_malloc(rgn,sizeof(*_tmp79D)),((_tmp79D->hd=_tmp93,((_tmp79D->tl=0,_tmp79D)))))),_tmp79C)));};}{
# 430
struct Cyc_Dict_Dict*outer_all_changed=env->all_changed;
struct Cyc_Dict_Dict this_all_changed;
union Cyc_CfFlowInfo_FlowInfo old_inflow;
union Cyc_CfFlowInfo_FlowInfo outflow;
struct Cyc_List_List*rvals;
# 437
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
do{
this_all_changed=(env->fenv)->mt_place_set;
# 441
{struct Cyc_Dict_Dict*_tmp79E;env->all_changed=((_tmp79E=_region_malloc(env->r,sizeof(*_tmp79E)),((_tmp79E[0]=(env->fenv)->mt_place_set,_tmp79E))));}{
struct _tuple16 _tmp97=Cyc_NewControlFlow_anal_Rexp(env,first_is_copy,inflow,(struct Cyc_Absyn_Exp*)es->hd);union Cyc_CfFlowInfo_FlowInfo _tmp99;void*_tmp9A;struct _tuple16 _tmp98=_tmp97;_tmp99=_tmp98.f1;_tmp9A=_tmp98.f2;
outflow=_tmp99;
{struct Cyc_List_List*_tmp79F;rvals=((_tmp79F=_region_malloc(rgn,sizeof(*_tmp79F)),((_tmp79F->hd=_tmp9A,((_tmp79F->tl=0,_tmp79F))))));}
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);
# 447
{struct Cyc_List_List*es2=es->tl;for(0;es2 != 0;es2=es2->tl){
{struct Cyc_Dict_Dict*_tmp7A0;env->all_changed=((_tmp7A0=_region_malloc(env->r,sizeof(*_tmp7A0)),((_tmp7A0[0]=(env->fenv)->mt_place_set,_tmp7A0))));}{
struct _tuple16 _tmp9D=Cyc_NewControlFlow_anal_Rexp(env,others_are_copy,inflow,(struct Cyc_Absyn_Exp*)es2->hd);union Cyc_CfFlowInfo_FlowInfo _tmp9F;void*_tmpA0;struct _tuple16 _tmp9E=_tmp9D;_tmp9F=_tmp9E.f1;_tmpA0=_tmp9E.f2;
{struct Cyc_List_List*_tmp7A1;rvals=((_tmp7A1=_region_malloc(rgn,sizeof(*_tmp7A1)),((_tmp7A1->hd=_tmpA0,((_tmp7A1->tl=rvals,_tmp7A1))))));}
outflow=Cyc_CfFlowInfo_after_flow(env->fenv,& this_all_changed,outflow,_tmp9F,this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)));
# 453
this_all_changed=Cyc_CfFlowInfo_union_place_set(this_all_changed,*((struct Cyc_Dict_Dict*)_check_null(env->all_changed)),0);};}}
# 456
old_inflow=inflow;
# 459
inflow=Cyc_CfFlowInfo_join_flow(env->fenv,outer_all_changed,inflow,outflow);};}while(!
# 464
Cyc_CfFlowInfo_flow_lessthan_approx(inflow,old_inflow));
if(outer_all_changed == 0)
env->all_changed=0;else{
# 468
struct Cyc_Dict_Dict*_tmp7A2;env->all_changed=((_tmp7A2=_region_malloc(env->r,sizeof(*_tmp7A2)),((_tmp7A2[0]=Cyc_CfFlowInfo_union_place_set(*outer_all_changed,this_all_changed,0),_tmp7A2))));}
# 470
Cyc_NewControlFlow_update_tryflow(env,outflow);{
# 472
struct _tuple23 _tmp7A3;return(_tmp7A3.f1=outflow,((_tmp7A3.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rvals),_tmp7A3)));};};}
# 477
static struct _tuple16 Cyc_NewControlFlow_anal_use_ints(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 480
struct _RegionHandle _tmpA4=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA4;_push_region(rgn);
{struct _tuple23 _tmpA5=
Cyc_NewControlFlow_anal_unordered_Rexps(rgn,env,inflow,es,0,0);
# 481
union Cyc_CfFlowInfo_FlowInfo _tmpA7;struct Cyc_List_List*_tmpA8;struct _tuple23 _tmpA6=_tmpA5;_tmpA7=_tmpA6.f1;_tmpA8=_tmpA6.f2;
# 483
{union Cyc_CfFlowInfo_FlowInfo _tmpA9=_tmpA7;struct Cyc_Dict_Dict _tmpAA;_LL57: if((_tmpA9.ReachableFL).tag != 2)goto _LL59;_tmpAA=((struct _tuple14)(_tmpA9.ReachableFL).val).f1;_LL58:
# 485
 for(0;_tmpA8 != 0;(_tmpA8=_tmpA8->tl,es=((struct Cyc_List_List*)_check_null(es))->tl)){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmpAA,(void*)_tmpA8->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp7A6;void*_tmp7A5;(_tmp7A5=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp7A6="expression may not be initialized",_tag_dyneither(_tmp7A6,sizeof(char),34))),_tag_dyneither(_tmp7A5,sizeof(void*),0)));}}
goto _LL56;_LL59: if((_tmpA9.BottomFL).tag != 1)goto _LL56;_LL5A:
 goto _LL56;_LL56:;}{
# 491
struct _tuple16 _tmp7A7;struct _tuple16 _tmpAE=(_tmp7A7.f1=_tmpA7,((_tmp7A7.f2=(env->fenv)->unknown_all,_tmp7A7)));_npop_handler(0);return _tmpAE;};}
# 481
;_pop_region(rgn);}
# 499
static void*Cyc_NewControlFlow_consume_zero_rval(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Dict_Dict new_dict,struct Cyc_CfFlowInfo_Place*p,struct Cyc_Absyn_Exp*e,void*new_rval){
# 506
int needs_unconsume=0;
void*_tmpAF=Cyc_CfFlowInfo_lookup_place(new_dict,p);
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmpAF,& needs_unconsume)){
struct Cyc_Core_Impossible_exn_struct _tmp7AD;const char*_tmp7AC;struct Cyc_Core_Impossible_exn_struct*_tmp7AB;(int)_throw((void*)((_tmp7AB=_cycalloc(sizeof(*_tmp7AB)),((_tmp7AB[0]=((_tmp7AD.tag=Cyc_Core_Impossible,((_tmp7AD.f1=((_tmp7AC="consume_zero_ral",_tag_dyneither(_tmp7AC,sizeof(char),17))),_tmp7AD)))),_tmp7AB)))));}else{
# 511
if(needs_unconsume)
return Cyc_CfFlowInfo_make_unique_consumed(env->fenv,(void*)_check_null(e->topt),e,env->iteration_num,new_rval);else{
# 515
return new_rval;}}}
# 528 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_notzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 534
union Cyc_CfFlowInfo_FlowInfo _tmpB3=outflow;struct Cyc_Dict_Dict _tmpB4;struct Cyc_List_List*_tmpB5;_LL5C: if((_tmpB3.BottomFL).tag != 1)goto _LL5E;_LL5D:
 return outflow;_LL5E: if((_tmpB3.ReachableFL).tag != 2)goto _LL5B;_tmpB4=((struct _tuple14)(_tmpB3.ReachableFL).val).f1;_tmpB5=((struct _tuple14)(_tmpB3.ReachableFL).val).f2;_LL5F: {
# 537
union Cyc_CfFlowInfo_AbsLVal _tmpB6=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpB7=_tmpB6;struct Cyc_CfFlowInfo_Place*_tmpB8;_LL61: if((_tmpB7.UnknownL).tag != 2)goto _LL63;_LL62:
# 541
 return outflow;_LL63: if((_tmpB7.PlaceL).tag != 1)goto _LL60;_tmpB8=(struct Cyc_CfFlowInfo_Place*)(_tmpB7.PlaceL).val;_LL64: {
# 545
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
for(0;names != 0;names=names->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B0;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7AF;nzval=(void*)((_tmp7AF=_region_malloc((env->fenv)->r,sizeof(*_tmp7AF)),((_tmp7AF[0]=((_tmp7B0.tag=8,((_tmp7B0.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7B0.f2=nzval,_tmp7B0)))))),_tmp7AF))));}
# 549
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpB4,_tmpB8,e,nzval);{
union Cyc_CfFlowInfo_FlowInfo _tmpBB=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpB4,env->all_changed,_tmpB8,nzval),_tmpB5);
# 554
return _tmpBB;};}_LL60:;}_LL5B:;}
# 563
static struct _tuple19 Cyc_NewControlFlow_splitzero(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*e,enum Cyc_CfFlowInfo_InitLevel il,struct Cyc_List_List*names){
# 566
union Cyc_CfFlowInfo_FlowInfo _tmpBC=outflow;struct Cyc_Dict_Dict _tmpBD;struct Cyc_List_List*_tmpBE;_LL66: if((_tmpBC.BottomFL).tag != 1)goto _LL68;_LL67: {
struct _tuple19 _tmp7B1;return(_tmp7B1.f1=outflow,((_tmp7B1.f2=outflow,_tmp7B1)));}_LL68: if((_tmpBC.ReachableFL).tag != 2)goto _LL65;_tmpBD=((struct _tuple14)(_tmpBC.ReachableFL).val).f1;_tmpBE=((struct _tuple14)(_tmpBC.ReachableFL).val).f2;_LL69: {
# 569
union Cyc_CfFlowInfo_AbsLVal _tmpC0=(Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,e)).f2;union Cyc_CfFlowInfo_AbsLVal _tmpC1=_tmpC0;struct Cyc_CfFlowInfo_Place*_tmpC2;_LL6B: if((_tmpC1.UnknownL).tag != 2)goto _LL6D;_LL6C: {
struct _tuple19 _tmp7B2;return(_tmp7B2.f1=outflow,((_tmp7B2.f2=outflow,_tmp7B2)));}_LL6D: if((_tmpC1.PlaceL).tag != 1)goto _LL6A;_tmpC2=(struct Cyc_CfFlowInfo_Place*)(_tmpC1.PlaceL).val;_LL6E: {
# 572
void*nzval=il == Cyc_CfFlowInfo_AllIL?(env->fenv)->notzeroall:(env->fenv)->notzerothis;
void*zval=(env->fenv)->zero;
for(0;names != 0;names=names->tl){
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B5;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B4;nzval=(void*)((_tmp7B4=_region_malloc((env->fenv)->r,sizeof(*_tmp7B4)),((_tmp7B4[0]=((_tmp7B5.tag=8,((_tmp7B5.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7B5.f2=nzval,_tmp7B5)))))),_tmp7B4))));}{
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7B8;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7B7;zval=(void*)((_tmp7B7=_region_malloc((env->fenv)->r,sizeof(*_tmp7B7)),((_tmp7B7[0]=((_tmp7B8.tag=8,((_tmp7B8.f1=(struct Cyc_Absyn_Vardecl*)names->hd,((_tmp7B8.f2=zval,_tmp7B8)))))),_tmp7B7))));};}
# 578
nzval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,nzval);
zval=Cyc_NewControlFlow_consume_zero_rval(env,_tmpBD,_tmpC2,e,zval);{
struct _tuple19 _tmp7B9;return
(_tmp7B9.f1=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,nzval),_tmpBE),((_tmp7B9.f2=
# 583
Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,e->loc,_tmpBD,env->all_changed,_tmpC2,zval),_tmpBE),_tmp7B9)));};}_LL6A:;}_LL65:;}
# 589
static struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_notzero_v={Cyc_CfFlowInfo_NotZero,0};
static struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct Cyc_NewControlFlow_mt_unknownz_v={Cyc_CfFlowInfo_UnknownZ,0};
# 597
static struct _tuple16 Cyc_NewControlFlow_anal_derefR(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Exp*e,void*r){
# 601
void*_tmpCB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpCC=_tmpCB;void*_tmpCE;union Cyc_Absyn_Constraint*_tmpCF;union Cyc_Absyn_Constraint*_tmpD0;_LL70: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC;if(_tmpCD->tag != 5)goto _LL72;else{_tmpCE=(_tmpCD->f1).elt_typ;_tmpCF=((_tmpCD->f1).ptr_atts).bounds;_tmpD0=((_tmpCD->f1).ptr_atts).zero_term;}}_LL71: {
# 603
union Cyc_CfFlowInfo_FlowInfo _tmpD1=f;struct Cyc_Dict_Dict _tmpD2;struct Cyc_List_List*_tmpD3;_LL75: if((_tmpD1.BottomFL).tag != 1)goto _LL77;_LL76: {
# 605
struct _tuple16 _tmp7BA;return(_tmp7BA.f1=f,((_tmp7BA.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7BA)));}_LL77: if((_tmpD1.ReachableFL).tag != 2)goto _LL74;_tmpD2=((struct _tuple14)(_tmpD1.ReachableFL).val).f1;_tmpD3=((struct _tuple14)(_tmpD1.ReachableFL).val).f2;_LL78:
# 608
 if(Cyc_Tcutil_is_bound_one(_tmpCF)){
struct _tuple15 _tmpD5=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmpD7;struct Cyc_List_List*_tmpD8;struct _tuple15 _tmpD6=_tmpD5;_tmpD7=_tmpD6.f1;_tmpD8=_tmpD6.f2;{
void*_tmpD9=_tmpD7;struct Cyc_Absyn_Vardecl*_tmpDB;void*_tmpDC;struct Cyc_CfFlowInfo_Place*_tmpE0;enum Cyc_CfFlowInfo_InitLevel _tmpE3;_LL7A: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpDA=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDA->tag != 8)goto _LL7C;else{_tmpDB=_tmpDA->f1;_tmpDC=(void*)_tmpDA->f2;}}_LL7B: {
# 612
struct Cyc_Core_Impossible_exn_struct _tmp7C0;const char*_tmp7BF;struct Cyc_Core_Impossible_exn_struct*_tmp7BE;(int)_throw((void*)((_tmp7BE=_cycalloc(sizeof(*_tmp7BE)),((_tmp7BE[0]=((_tmp7C0.tag=Cyc_Core_Impossible,((_tmp7C0.f1=((_tmp7BF="named location in anal_derefR",_tag_dyneither(_tmp7BF,sizeof(char),30))),_tmp7C0)))),_tmp7BE)))));}_LL7C: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmpDD=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDD->tag != 1)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmpDE=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDE->tag != 2)goto _LL80;}_LL7F:
# 615
{void*_tmpE7=e->annot;void*_tmpE8=_tmpE7;struct Cyc_List_List*_tmpEA;_LL89: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpE9=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpE8;if(_tmpE9->tag != Cyc_CfFlowInfo_NotZero)goto _LL8B;else{_tmpEA=_tmpE9->f1;}}_LL8A:
# 617
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpEA))goto _LL8C;
goto _LL88;_LL8B:;_LL8C:
# 621
{void*_tmpEB=e->r;void*_tmpEC=_tmpEB;_LL8E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpED=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEC;if(_tmpED->tag != 22)goto _LL90;}_LL8F:
# 623
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7C3;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7C2;e->annot=(void*)((_tmp7C2=_cycalloc(sizeof(*_tmp7C2)),((_tmp7C2[0]=((_tmp7C3.tag=Cyc_CfFlowInfo_NotZero,((_tmp7C3.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7C3)))),_tmp7C2))));}
goto _LL8D;_LL90:;_LL91:
# 626
 e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;
goto _LL8D;_LL8D:;}
# 629
goto _LL88;_LL88:;}
# 631
goto _LL79;_LL80: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpDF=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpDF->tag != 5)goto _LL82;else{_tmpE0=_tmpDF->f1;}}_LL81: {
# 635
int possibly_null=0;
{void*_tmpF0=e->annot;void*_tmpF1=_tmpF0;struct Cyc_List_List*_tmpF3;struct Cyc_List_List*_tmpF5;_LL93: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmpF2=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF2->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL95;else{_tmpF3=_tmpF2->f1;}}_LL94:
# 638
 possibly_null=1;
_tmpF5=_tmpF3;goto _LL96;_LL95: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmpF4=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmpF1;if(_tmpF4->tag != Cyc_CfFlowInfo_NotZero)goto _LL97;else{_tmpF5=_tmpF4->f1;}}_LL96:
# 641
 if(!Cyc_Relations_same_relns(_tmpD3,_tmpF5))goto _LL98;
goto _LL92;_LL97:;_LL98:
# 645
{void*_tmpF6=e->r;void*_tmpF7=_tmpF6;_LL9A: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpF8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpF7;if(_tmpF8->tag != 22)goto _LL9C;}_LL9B:
# 647
 if(possibly_null){
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7C6;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7C5;e->annot=(void*)((_tmp7C5=_cycalloc(sizeof(*_tmp7C5)),((_tmp7C5[0]=((_tmp7C6.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7C6.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7C6)))),_tmp7C5))));}else{
# 650
struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp7C9;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp7C8;e->annot=(void*)((_tmp7C8=_cycalloc(sizeof(*_tmp7C8)),((_tmp7C8[0]=((_tmp7C9.tag=Cyc_CfFlowInfo_NotZero,((_tmp7C9.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7C9)))),_tmp7C8))));}
goto _LL99;_LL9C:;_LL9D:
# 653
 if(possibly_null)
e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;else{
# 656
e->annot=(void*)& Cyc_NewControlFlow_mt_notzero_v;}
goto _LL99;_LL99:;}
# 659
goto _LL92;_LL92:;}{
# 661
struct _tuple16 _tmp7CA;return(_tmp7CA.f1=f,((_tmp7CA.f2=Cyc_CfFlowInfo_lookup_place(_tmpD2,_tmpE0),_tmp7CA)));};}_LL82: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmpE1=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE1->tag != 0)goto _LL84;}_LL83:
# 663
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple16 _tmp7CB;return(_tmp7CB.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7CB.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7CB)));};_LL84: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmpE2=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD9;if(_tmpE2->tag != 3)goto _LL86;else{_tmpE3=_tmpE2->f1;}}_LL85:
# 666
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmpE3,_tmpD8);goto _LL87;_LL86:;_LL87:
# 668
{void*_tmpFF=e->r;void*_tmp100=_tmpFF;_LL9F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp101=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp100;if(_tmp101->tag != 22)goto _LLA1;}_LLA0:
# 670
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7CE;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7CD;e->annot=(void*)((_tmp7CD=_cycalloc(sizeof(*_tmp7CD)),((_tmp7CD[0]=((_tmp7CE.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7CE.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7CE)))),_tmp7CD))));}
goto _LL9E;_LLA1:;_LLA2:
# 673
 e->annot=(void*)& Cyc_NewControlFlow_mt_unknownz_v;
goto _LL9E;_LL9E:;}
# 676
goto _LL79;_LL79:;};}else{
# 679
void*_tmp104=e->annot;void*_tmp105=_tmp104;struct Cyc_List_List*_tmp107;_LLA4: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp106=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp105;if(_tmp106->tag != Cyc_CfFlowInfo_UnknownZ)goto _LLA6;else{_tmp107=_tmp106->f1;}}_LLA5:
# 681
 if(!Cyc_Relations_same_relns(_tmpD3,_tmp107))goto _LLA7;
goto _LLA3;_LLA6:;_LLA7:
# 684
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp7D1;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp7D0;e->annot=(void*)((_tmp7D0=_cycalloc(sizeof(*_tmp7D0)),((_tmp7D0[0]=((_tmp7D1.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp7D1.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpD3),_tmp7D1)))),_tmp7D0))));}
goto _LLA3;_LLA3:;}{
# 688
enum Cyc_CfFlowInfo_InitLevel _tmp10A=Cyc_CfFlowInfo_initlevel(env->fenv,_tmpD2,r);switch(_tmp10A){case Cyc_CfFlowInfo_NoneIL: _LLA8: {
# 690
struct _tuple15 _tmp10B=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp10D;struct _tuple15 _tmp10C=_tmp10B;_tmp10D=_tmp10C.f1;
{void*_tmp10E=_tmp10D;_LLAB: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp10F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp10E;if(_tmp10F->tag != 7)goto _LLAD;}_LLAC:
# 693
{const char*_tmp7D4;void*_tmp7D3;(_tmp7D3=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7D4="attempt to dereference a consumed alias-free pointer",_tag_dyneither(_tmp7D4,sizeof(char),53))),_tag_dyneither(_tmp7D3,sizeof(void*),0)));}
goto _LLAA;_LLAD:;_LLAE: {
# 696
const char*_tmp7D7;void*_tmp7D6;(_tmp7D6=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp7D7="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp7D7,sizeof(char),46))),_tag_dyneither(_tmp7D6,sizeof(void*),0)));}_LLAA:;}
# 698
goto _LLA9;}case Cyc_CfFlowInfo_AllIL: _LLA9: {
# 700
struct _tuple16 _tmp7D8;return(_tmp7D8.f1=f,((_tmp7D8.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_all),_tmp7D8)));}case Cyc_CfFlowInfo_ThisIL: _LLAF: {
# 702
struct _tuple16 _tmp7D9;return(_tmp7D9.f1=f,((_tmp7D9.f2=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,_tmpCE,0,(env->fenv)->unknown_none),_tmp7D9)));}}};_LL74:;}_LL72:;_LL73: {
# 705
struct Cyc_Core_Impossible_exn_struct _tmp7DF;const char*_tmp7DE;struct Cyc_Core_Impossible_exn_struct*_tmp7DD;(int)_throw((void*)((_tmp7DD=_cycalloc(sizeof(*_tmp7DD)),((_tmp7DD[0]=((_tmp7DF.tag=Cyc_Core_Impossible,((_tmp7DF.f1=((_tmp7DE="right deref of non-pointer-type",_tag_dyneither(_tmp7DE,sizeof(char),32))),_tmp7DF)))),_tmp7DD)))));}_LL6F:;}
# 712
static struct Cyc_List_List*Cyc_NewControlFlow_add_subscript_reln(struct _RegionHandle*rgn,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 715
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
int e2_valid_op=Cyc_Relations_exp2relnop(e2,& n2);
# 718
{void*_tmp119=e1->r;void*_tmp11A=_tmp119;void*_tmp11C;_LLB2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp11A;if(_tmp11B->tag != 1)goto _LLB4;else{_tmp11C=(void*)_tmp11B->f2;}}_LLB3: {
# 720
struct Cyc_Absyn_Vardecl*_tmp11D=Cyc_Tcutil_nonesc_vardecl(_tmp11C);
if(_tmp11D != 0){
union Cyc_Relations_RelnOp n1=Cyc_Relations_RNumelts(_tmp11D);
if(e2_valid_op)
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,n1,relns);}
# 726
goto _LLB1;}_LLB4:;_LLB5:
 goto _LLB1;_LLB1:;}{
# 730
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound((void*)_check_null(e1->topt));
if(bound != 0){
union Cyc_Relations_RelnOp rbound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rbound))
relns=Cyc_Relations_add_relation(rgn,n2,Cyc_Relations_Rlt,rbound,relns);}
# 736
return relns;};}
# 744
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_restore_noconsume_arg(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*exp,unsigned int loc,void*old_rval){
# 749
struct _tuple18 _tmp11E=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,1,exp);union Cyc_CfFlowInfo_AbsLVal _tmp120;struct _tuple18 _tmp11F=_tmp11E;_tmp120=_tmp11F.f2;
{struct _tuple18 _tmp7E0;struct _tuple18 _tmp121=(_tmp7E0.f1=inflow,((_tmp7E0.f2=_tmp120,_tmp7E0)));struct _tuple18 _tmp122=_tmp121;struct Cyc_Dict_Dict _tmp123;struct Cyc_List_List*_tmp124;struct Cyc_CfFlowInfo_Place*_tmp125;_LLB7: if(((_tmp122.f1).ReachableFL).tag != 2)goto _LLB9;_tmp123=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f1;_tmp124=((struct _tuple14)((_tmp122.f1).ReachableFL).val).f2;if(((_tmp122.f2).PlaceL).tag != 1)goto _LLB9;_tmp125=(struct Cyc_CfFlowInfo_Place*)((_tmp122.f2).PlaceL).val;_LLB8: {
# 752
void*_tmp126=Cyc_CfFlowInfo_typ_to_absrval(env->fenv,(void*)_check_null(exp->topt),0,(env->fenv)->unknown_all);
# 754
struct _tuple15 _tmp127=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,old_rval);void*_tmp129;struct Cyc_List_List*_tmp12A;struct _tuple15 _tmp128=_tmp127;_tmp129=_tmp128.f1;_tmp12A=_tmp128.f2;
for(0;_tmp12A != 0;_tmp12A=_tmp12A->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7E3;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7E2;_tmp126=(void*)((_tmp7E2=_region_malloc((env->fenv)->r,sizeof(*_tmp7E2)),((_tmp7E2[0]=((_tmp7E3.tag=8,((_tmp7E3.f1=(struct Cyc_Absyn_Vardecl*)_tmp12A->hd,((_tmp7E3.f2=_tmp126,_tmp7E3)))))),_tmp7E2))));}
# 759
_tmp123=Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp123,env->all_changed,_tmp125,_tmp126);
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp123,_tmp124);
Cyc_NewControlFlow_update_tryflow(env,inflow);
# 765
goto _LLB6;}_LLB9: if(((_tmp122.f1).BottomFL).tag != 1)goto _LLBB;_LLBA:
 goto _LLB6;_LLBB:;_LLBC:
# 768
{const char*_tmp7E6;void*_tmp7E5;(_tmp7E5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7E6="noconsume() parameters must be l-values",_tag_dyneither(_tmp7E6,sizeof(char),40))),_tag_dyneither(_tmp7E5,sizeof(void*),0)));}
goto _LLB6;_LLB6:;}
# 771
return inflow;}
# 776
static struct _tuple16 Cyc_NewControlFlow_do_assign(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo outflow,struct Cyc_Absyn_Exp*lexp,union Cyc_CfFlowInfo_AbsLVal lval,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 784
union Cyc_CfFlowInfo_FlowInfo _tmp130=outflow;struct Cyc_Dict_Dict _tmp131;struct Cyc_List_List*_tmp132;_LLBE: if((_tmp130.BottomFL).tag != 1)goto _LLC0;_LLBF: {
# 786
struct _tuple16 _tmp7E7;return(_tmp7E7.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp7E7.f2=rval,_tmp7E7)));}_LLC0: if((_tmp130.ReachableFL).tag != 2)goto _LLBD;_tmp131=((struct _tuple14)(_tmp130.ReachableFL).val).f1;_tmp132=((struct _tuple14)(_tmp130.ReachableFL).val).f2;_LLC1: {
# 788
union Cyc_CfFlowInfo_AbsLVal _tmp134=lval;struct Cyc_CfFlowInfo_Place*_tmp135;_LLC3: if((_tmp134.PlaceL).tag != 1)goto _LLC5;_tmp135=(struct Cyc_CfFlowInfo_Place*)(_tmp134.PlaceL).val;_LLC4: {
# 790
struct Cyc_Dict_Dict _tmp136=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp131,env->all_changed,_tmp135,rval);
_tmp132=Cyc_Relations_reln_assign_exp(fenv->r,_tmp132,lexp,rexp);
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp136,_tmp132);
if(Cyc_NewControlFlow_warn_lose_unique  && 
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(lexp->topt))){
# 796
struct _tuple15 _tmp137=Cyc_CfFlowInfo_unname_rval(fenv->r,Cyc_CfFlowInfo_lookup_place(_tmp131,_tmp135));void*_tmp139;struct _tuple15 _tmp138=_tmp137;_tmp139=_tmp138.f1;{
void*_tmp13A=_tmp139;_LLC8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp13B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13B->tag != 3)goto _LLCA;else{if(_tmp13B->f1 != Cyc_CfFlowInfo_NoneIL)goto _LLCA;}}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp13C=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13C->tag != 7)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp13D=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp13A;if(_tmp13D->tag != 0)goto _LLCE;}_LLCD:
 goto _LLC7;_LLCE:;_LLCF:
# 802
{const char*_tmp7EA;void*_tmp7E9;(_tmp7E9=0,Cyc_Tcutil_warn(lexp->loc,((_tmp7EA="assignment may overwrite alias-free pointer(s)",_tag_dyneither(_tmp7EA,sizeof(char),47))),_tag_dyneither(_tmp7E9,sizeof(void*),0)));}
goto _LLC7;_LLC7:;};}
# 807
Cyc_NewControlFlow_update_tryflow(env,outflow);{
struct _tuple16 _tmp7EB;return(_tmp7EB.f1=outflow,((_tmp7EB.f2=rval,_tmp7EB)));};}_LLC5: if((_tmp134.UnknownL).tag != 2)goto _LLC2;_LLC6: {
# 810
struct _tuple16 _tmp7EC;return(_tmp7EC.f1=Cyc_NewControlFlow_use_Rval(env,rexp->loc,outflow,rval),((_tmp7EC.f2=rval,_tmp7EC)));}_LLC2:;}_LLBD:;}
# 817
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_do_initialize_var(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo f,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*rexp,void*rval,unsigned int loc){
# 823
union Cyc_CfFlowInfo_FlowInfo _tmp142=f;struct Cyc_Dict_Dict _tmp143;struct Cyc_List_List*_tmp144;_LLD1: if((_tmp142.BottomFL).tag != 1)goto _LLD3;_LLD2:
 return Cyc_CfFlowInfo_BottomFL();_LLD3: if((_tmp142.ReachableFL).tag != 2)goto _LLD0;_tmp143=((struct _tuple14)(_tmp142.ReachableFL).val).f1;_tmp144=((struct _tuple14)(_tmp142.ReachableFL).val).f2;_LLD4:
# 828
{struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp7F2;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp7F1;struct Cyc_CfFlowInfo_Place*_tmp7F0;_tmp143=Cyc_CfFlowInfo_assign_place(fenv,loc,_tmp143,env->all_changed,(
(_tmp7F0=_region_malloc(env->r,sizeof(*_tmp7F0)),((_tmp7F0->root=(void*)((_tmp7F2=_region_malloc(env->r,sizeof(*_tmp7F2)),((_tmp7F2[0]=((_tmp7F1.tag=0,((_tmp7F1.f1=vd,_tmp7F1)))),_tmp7F2)))),((_tmp7F0->fields=0,_tmp7F0)))))),rval);}
# 831
_tmp144=Cyc_Relations_reln_assign_var(env->r,_tmp144,vd,rexp);{
union Cyc_CfFlowInfo_FlowInfo _tmp148=Cyc_CfFlowInfo_ReachableFL(_tmp143,_tmp144);
Cyc_NewControlFlow_update_tryflow(env,_tmp148);
# 836
return _tmp148;};_LLD0:;}struct _tuple24{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 840
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_initialize_pat_vars(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*vds,int name_locs,unsigned int pat_loc,unsigned int exp_loc){
# 847
if(vds == 0)return inflow;{
# 850
struct Cyc_List_List*_tmp149=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(vds)).f1);
struct Cyc_List_List*es=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
if((*((struct _tuple24*)x->hd)).f1 == 0){struct Cyc_List_List*_tmp7F3;es=((_tmp7F3=_cycalloc(sizeof(*_tmp7F3)),((_tmp7F3->hd=(struct Cyc_Absyn_Exp*)_check_null((*((struct _tuple24*)x->hd)).f2),((_tmp7F3->tl=es,_tmp7F3))))));}}}
# 856
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,_tmp149,fenv->unknown_all,pat_loc,name_locs);
# 858
inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,es);
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
# 862
struct _tuple16 _tmp14B=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp14D;void*_tmp14E;struct _tuple16 _tmp14C=_tmp14B;_tmp14D=_tmp14C.f1;_tmp14E=_tmp14C.f2;
inflow=Cyc_NewControlFlow_use_Rval(env,exp_loc,_tmp14D,_tmp14E);}}{
# 870
struct Cyc_List_List*_tmp14F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(vds);
for(0;_tmp14F != 0;_tmp14F=_tmp14F->tl){
struct _tuple24*_tmp150=(struct _tuple24*)_tmp14F->hd;struct Cyc_Absyn_Vardecl**_tmp152;struct Cyc_Absyn_Exp*_tmp153;struct _tuple24*_tmp151=_tmp150;_tmp152=_tmp151->f1;_tmp153=_tmp151->f2;
if(_tmp152 != 0  && _tmp153 != 0){
if(_tmp153->topt == 0){
const char*_tmp7F7;void*_tmp7F6[1];struct Cyc_String_pa_PrintArg_struct _tmp7F5;(_tmp7F5.tag=0,((_tmp7F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp7F6[0]=& _tmp7F5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7F7="oops! pattern init expr %s has no type!\n",_tag_dyneither(_tmp7F7,sizeof(char),41))),_tag_dyneither(_tmp7F6,sizeof(void*),1)))))));}{
# 884
struct Cyc_List_List _tmp7F8;struct Cyc_List_List l=(_tmp7F8.hd=_tmp153,((_tmp7F8.tl=0,_tmp7F8)));
union Cyc_CfFlowInfo_FlowInfo _tmp157=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp158=Cyc_NewControlFlow_anal_Lexp(env,_tmp157,0,0,_tmp153);union Cyc_CfFlowInfo_AbsLVal _tmp15A;struct _tuple18 _tmp159=_tmp158;_tmp15A=_tmp159.f2;{
struct _tuple16 _tmp15B=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp157,_tmp153);union Cyc_CfFlowInfo_FlowInfo _tmp15D;void*_tmp15E;struct _tuple16 _tmp15C=_tmp15B;_tmp15D=_tmp15C.f1;_tmp15E=_tmp15C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp15F=_tmp15D;struct Cyc_Dict_Dict _tmp160;struct Cyc_List_List*_tmp161;_LLD6: if((_tmp15F.ReachableFL).tag != 2)goto _LLD8;_tmp160=((struct _tuple14)(_tmp15F.ReachableFL).val).f1;_tmp161=((struct _tuple14)(_tmp15F.ReachableFL).val).f2;_LLD7:
# 890
 if(name_locs){
union Cyc_CfFlowInfo_AbsLVal _tmp162=_tmp15A;struct Cyc_CfFlowInfo_Place*_tmp163;_LLDB: if((_tmp162.PlaceL).tag != 1)goto _LLDD;_tmp163=(struct Cyc_CfFlowInfo_Place*)(_tmp162.PlaceL).val;_LLDC:
# 893
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7FB;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7FA;_tmp15E=(void*)((_tmp7FA=_region_malloc(fenv->r,sizeof(*_tmp7FA)),((_tmp7FA[0]=((_tmp7FB.tag=8,((_tmp7FB.f1=*_tmp152,((_tmp7FB.f2=_tmp15E,_tmp7FB)))))),_tmp7FA))));}{
# 896
void*_tmp166=Cyc_CfFlowInfo_lookup_place(_tmp160,_tmp163);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp7FE;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp7FD;_tmp166=(void*)((_tmp7FD=_region_malloc(fenv->r,sizeof(*_tmp7FD)),((_tmp7FD[0]=((_tmp7FE.tag=8,((_tmp7FE.f1=*_tmp152,((_tmp7FE.f2=_tmp166,_tmp7FE)))))),_tmp7FD))));}
_tmp160=Cyc_CfFlowInfo_assign_place(fenv,exp_loc,_tmp160,env->all_changed,_tmp163,_tmp166);
_tmp15D=Cyc_CfFlowInfo_ReachableFL(_tmp160,_tmp161);
goto _LLDA;};_LLDD:;_LLDE:
# 903
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(void*)_check_null(_tmp153->topt)) && !
# 905
Cyc_Tcutil_is_noalias_pointer_or_aggr((env->fenv)->r,(*_tmp152)->type)){
# 907
const char*_tmp802;void*_tmp801[1];struct Cyc_String_pa_PrintArg_struct _tmp800;(_tmp800.tag=0,((_tmp800.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp153)),((_tmp801[0]=& _tmp800,Cyc_CfFlowInfo_aerr(exp_loc,((_tmp802="aliased pattern expression not an l-value: %s",_tag_dyneither(_tmp802,sizeof(char),46))),_tag_dyneither(_tmp801,sizeof(void*),1)))))));}_LLDA:;}
# 915
inflow=Cyc_NewControlFlow_do_initialize_var(fenv,env,_tmp15D,*_tmp152,_tmp153,_tmp15E,pat_loc);
goto _LLD5;_LLD8:;_LLD9:
# 918
 goto _LLD5;_LLD5:;};};};}}
# 923
return inflow;};};}
# 926
static int Cyc_NewControlFlow_is_local_var_rooted_path(struct Cyc_Absyn_Exp*e,int cast_ok){
# 928
void*_tmp16D=e->r;void*_tmp16E=_tmp16D;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp17C;struct Cyc_Absyn_Exp*_tmp17E;_LLE0: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp16F->tag != 1)goto _LLE2;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp170=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp16F->f2);if(_tmp170->tag != 4)goto _LLE2;}}_LLE1:
 goto _LLE3;_LLE2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp171->tag != 1)goto _LLE4;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp172=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp171->f2);if(_tmp172->tag != 3)goto _LLE4;}}_LLE3:
 goto _LLE5;_LLE4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp173->tag != 1)goto _LLE6;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp174=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp173->f2);if(_tmp174->tag != 5)goto _LLE6;}}_LLE5:
 return 1;_LLE6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp175->tag != 19)goto _LLE8;else{_tmp176=_tmp175->f1;}}_LLE7:
 _tmp178=_tmp176;goto _LLE9;_LLE8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp177->tag != 20)goto _LLEA;else{_tmp178=_tmp177->f1;}}_LLE9:
 _tmp17A=_tmp178;goto _LLEB;_LLEA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp179->tag != 21)goto _LLEC;else{_tmp17A=_tmp179->f1;}}_LLEB:
# 935
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17A,cast_ok);_LLEC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17B->tag != 22)goto _LLEE;else{_tmp17C=_tmp17B->f1;}}_LLED: {
# 937
void*_tmp17F=Cyc_Tcutil_compress((void*)_check_null(_tmp17C->topt));void*_tmp180=_tmp17F;_LLF3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp180;if(_tmp181->tag != 10)goto _LLF5;}_LLF4:
 return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17C,cast_ok);_LLF5:;_LLF6:
 return 0;_LLF2:;}_LLEE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp16E;if(_tmp17D->tag != 13)goto _LLF0;else{_tmp17E=_tmp17D->f2;}}_LLEF:
# 942
 if(cast_ok)return Cyc_NewControlFlow_is_local_var_rooted_path(_tmp17E,cast_ok);else{
return 0;}_LLF0:;_LLF1:
 return 0;_LLDF:;}struct _tuple25{union Cyc_CfFlowInfo_AbsLVal f1;union Cyc_CfFlowInfo_FlowInfo f2;};struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e);static void _tmp8B5(unsigned int*_tmp8B4,unsigned int*_tmp8B3,void***_tmp8B1,struct Cyc_List_List**_tmp386){for(*_tmp8B4=0;*_tmp8B4 < *_tmp8B3;(*_tmp8B4)++){
# 1529 "new_control_flow.cyc"
void*_tmp8AF;(*_tmp8B1)[*_tmp8B4]=(void*)((_tmp8AF=(void*)((struct Cyc_List_List*)_check_null(*_tmp386))->hd,((*_tmp386=(*_tmp386)->tl,_tmp8AF))));}}
# 949 "new_control_flow.cyc"
static struct _tuple16 Cyc_NewControlFlow_anal_Rexp(struct Cyc_NewControlFlow_AnalEnv*env,int copy_ctxt,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 953
struct Cyc_CfFlowInfo_FlowEnv*_tmp182=env->fenv;
struct Cyc_Dict_Dict d;
struct Cyc_List_List*relns;
# 966
{union Cyc_CfFlowInfo_FlowInfo _tmp183=inflow;struct Cyc_Dict_Dict _tmp184;struct Cyc_List_List*_tmp185;_LLF8: if((_tmp183.BottomFL).tag != 1)goto _LLFA;_LLF9: {
struct _tuple16 _tmp803;return(_tmp803.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp803.f2=_tmp182->unknown_all,_tmp803)));}_LLFA: if((_tmp183.ReachableFL).tag != 2)goto _LLF7;_tmp184=((struct _tuple14)(_tmp183.ReachableFL).val).f1;_tmp185=((struct _tuple14)(_tmp183.ReachableFL).val).f2;_LLFB:
 d=_tmp184;relns=_tmp185;_LLF7:;}
# 981 "new_control_flow.cyc"
if((copy_ctxt  && Cyc_NewControlFlow_is_local_var_rooted_path(e,0)) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
# 1002 "new_control_flow.cyc"
struct _tuple18 _tmp187=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,e);union Cyc_CfFlowInfo_FlowInfo _tmp189;union Cyc_CfFlowInfo_AbsLVal _tmp18A;struct _tuple18 _tmp188=_tmp187;_tmp189=_tmp188.f1;_tmp18A=_tmp188.f2;{
struct _tuple18 _tmp804;struct _tuple18 _tmp18B=(_tmp804.f1=_tmp189,((_tmp804.f2=_tmp18A,_tmp804)));struct _tuple18 _tmp18C=_tmp18B;struct Cyc_Dict_Dict _tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_CfFlowInfo_Place*_tmp18F;_LLFD: if(((_tmp18C.f1).ReachableFL).tag != 2)goto _LLFF;_tmp18D=((struct _tuple14)((_tmp18C.f1).ReachableFL).val).f1;_tmp18E=((struct _tuple14)((_tmp18C.f1).ReachableFL).val).f2;if(((_tmp18C.f2).PlaceL).tag != 1)goto _LLFF;_tmp18F=(struct Cyc_CfFlowInfo_Place*)((_tmp18C.f2).PlaceL).val;_LLFE: {
# 1005
void*_tmp190=Cyc_CfFlowInfo_lookup_place(_tmp18D,_tmp18F);
int needs_unconsume=0;
if(Cyc_CfFlowInfo_is_unique_consumed(e,env->iteration_num,_tmp190,& needs_unconsume)){
{const char*_tmp807;void*_tmp806;(_tmp806=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp807="expression attempts to copy a consumed alias-free value",_tag_dyneither(_tmp807,sizeof(char),56))),_tag_dyneither(_tmp806,sizeof(void*),0)));}{
struct _tuple16 _tmp808;return(_tmp808.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp808.f2=_tmp182->unknown_all,_tmp808)));};}else{
# 1011
if(needs_unconsume){
# 1013
struct _tuple16 _tmp809;return(_tmp809.f1=_tmp189,((_tmp809.f2=Cyc_CfFlowInfo_make_unique_unconsumed(_tmp182,_tmp190),_tmp809)));}else{
# 1016
void*_tmp195=Cyc_CfFlowInfo_make_unique_consumed(_tmp182,(void*)_check_null(e->topt),e,env->iteration_num,_tmp190);
struct Cyc_Dict_Dict _tmp196=Cyc_CfFlowInfo_assign_place(_tmp182,e->loc,_tmp18D,env->all_changed,_tmp18F,_tmp195);
# 1028
struct _tuple16 _tmp80A;return(_tmp80A.f1=Cyc_CfFlowInfo_ReachableFL(_tmp196,_tmp18E),((_tmp80A.f2=_tmp190,_tmp80A)));}}}_LLFF:;_LL100:
# 1030
 goto _LLFC;_LLFC:;};}{
# 1033
void*_tmp199=e->r;void*_tmp19A=_tmp199;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Vardecl*_tmp1B3;struct Cyc_Absyn_Vardecl*_tmp1B6;struct Cyc_Absyn_Vardecl*_tmp1B9;enum Cyc_Absyn_Primop _tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_Exp*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C4;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CB;struct Cyc_List_List*_tmp1CC;int _tmp1CE;struct Cyc_Absyn_Exp*_tmp1CF;void**_tmp1D0;struct Cyc_Absyn_Exp*_tmp1D1;int _tmp1D2;struct Cyc_Absyn_Exp*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_Absyn_Exp*_tmp1D7;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DE;struct _dyneither_ptr*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1E1;struct _dyneither_ptr*_tmp1E2;struct Cyc_Absyn_Exp*_tmp1E4;struct _dyneither_ptr*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E8;struct Cyc_Absyn_Exp*_tmp1E9;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct Cyc_Absyn_Exp*_tmp1F1;struct Cyc_Absyn_Exp*_tmp1F2;struct Cyc_List_List*_tmp1F4;struct Cyc_Absyn_Datatypedecl*_tmp1F5;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1FB;enum Cyc_Absyn_AggrKind _tmp1FC;struct Cyc_List_List*_tmp1FD;struct Cyc_List_List*_tmp200;struct Cyc_Absyn_Vardecl*_tmp202;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp204;int _tmp205;struct Cyc_Absyn_Exp*_tmp207;void*_tmp208;int _tmp209;struct Cyc_Absyn_Stmt*_tmp20B;_LL102: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19B->tag != 13)goto _LL104;else{_tmp19C=_tmp19B->f2;if(_tmp19B->f4 != Cyc_Absyn_NonNull_to_Null)goto _LL104;}}_LL103: {
# 1037
struct _tuple16 _tmp212=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp19C);union Cyc_CfFlowInfo_FlowInfo _tmp214;void*_tmp215;struct _tuple16 _tmp213=_tmp212;_tmp214=_tmp213.f1;_tmp215=_tmp213.f2;{
struct _tuple16 _tmp216=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp214,_tmp19C,_tmp215);union Cyc_CfFlowInfo_FlowInfo _tmp218;void*_tmp219;struct _tuple16 _tmp217=_tmp216;_tmp218=_tmp217.f1;_tmp219=_tmp217.f2;{
struct _tuple16 _tmp80B;return(_tmp80B.f1=_tmp218,((_tmp80B.f2=_tmp215,_tmp80B)));};};}_LL104: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp19D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19D->tag != 13)goto _LL106;else{_tmp19E=_tmp19D->f2;}}_LL105:
# 1043
 _tmp1A0=_tmp19E;goto _LL107;_LL106: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp19F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp19F->tag != 11)goto _LL108;else{_tmp1A0=_tmp19F->f1;}}_LL107:
 _tmp1A2=_tmp1A0;goto _LL109;_LL108: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A1->tag != 12)goto _LL10A;else{_tmp1A2=_tmp1A1->f1;}}_LL109:
 return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,inflow,_tmp1A2);_LL10A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A3=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A3->tag != 0)goto _LL10C;else{if(((_tmp1A3->f1).Null_c).tag != 1)goto _LL10C;}}_LL10B:
# 1047
 goto _LL10D;_LL10C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A4->tag != 0)goto _LL10E;else{if(((_tmp1A4->f1).Int_c).tag != 5)goto _LL10E;if(((struct _tuple7)((_tmp1A4->f1).Int_c).val).f2 != 0)goto _LL10E;}}_LL10D: {
struct _tuple16 _tmp80C;return(_tmp80C.f1=inflow,((_tmp80C.f2=_tmp182->zero,_tmp80C)));}_LL10E: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A5->tag != 0)goto _LL110;else{if(((_tmp1A5->f1).Int_c).tag != 5)goto _LL110;}}_LL10F:
 goto _LL111;_LL110: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1A6=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A6->tag != 1)goto _LL112;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp1A7=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp1A6->f2);if(_tmp1A7->tag != 2)goto _LL112;}}_LL111: {
struct _tuple16 _tmp80D;return(_tmp80D.f1=inflow,((_tmp80D.f2=_tmp182->notzeroall,_tmp80D)));}_LL112: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1A8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A8->tag != 30)goto _LL114;else{if(_tmp1A8->f1 != 0)goto _LL114;}}_LL113:
# 1052
 goto _LL115;_LL114: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1A9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1A9->tag != 0)goto _LL116;}_LL115:
 goto _LL117;_LL116: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AA->tag != 17)goto _LL118;}_LL117:
 goto _LL119;_LL118: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp1AB=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AB->tag != 16)goto _LL11A;}_LL119:
 goto _LL11B;_LL11A: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AC->tag != 18)goto _LL11C;}_LL11B:
 goto _LL11D;_LL11C: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp1AD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AD->tag != 32)goto _LL11E;}_LL11D:
 goto _LL11F;_LL11E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AE->tag != 31)goto _LL120;}_LL11F: {
struct _tuple16 _tmp80E;return(_tmp80E.f1=inflow,((_tmp80E.f2=_tmp182->unknown_all,_tmp80E)));}_LL120: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1AF=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1AF->tag != 1)goto _LL122;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp1B0=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp1AF->f2);if(_tmp1B0->tag != 1)goto _LL122;}}_LL121: {
# 1061
struct _tuple16 _tmp80F;return(_tmp80F.f1=inflow,((_tmp80F.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp80F)));}_LL122: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B1=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B1->tag != 1)goto _LL124;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1B2=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp1B1->f2);if(_tmp1B2->tag != 3)goto _LL124;else{_tmp1B3=_tmp1B2->f1;}}}_LL123:
# 1064
 _tmp1B6=_tmp1B3;goto _LL125;_LL124: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B4->tag != 1)goto _LL126;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp1B5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp1B4->f2);if(_tmp1B5->tag != 4)goto _LL126;else{_tmp1B6=_tmp1B5->f1;}}}_LL125:
 _tmp1B9=_tmp1B6;goto _LL127;_LL126: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp1B7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1B7->tag != 1)goto _LL128;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B8=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp1B7->f2);if(_tmp1B8->tag != 5)goto _LL128;else{_tmp1B9=_tmp1B8->f1;}}}_LL127: {
# 1068
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp810;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp810.tag=0,((_tmp810.f1=_tmp1B9,_tmp810)));
struct _tuple16 _tmp811;return(_tmp811.f1=inflow,((_tmp811.f2=((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(d,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot),_tmp811)));}_LL128: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1BA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BA->tag != 2)goto _LL12A;else{_tmp1BB=_tmp1BA->f1;_tmp1BC=_tmp1BA->f2;}}_LL129: {
# 1075
struct _tuple16 _tmp221=Cyc_NewControlFlow_anal_use_ints(env,inflow,_tmp1BC);union Cyc_CfFlowInfo_FlowInfo _tmp223;void*_tmp224;struct _tuple16 _tmp222=_tmp221;_tmp223=_tmp222.f1;_tmp224=_tmp222.f2;{
struct _tuple16 _tmp812;return(_tmp812.f1=_tmp223,((_tmp812.f2=_tmp224,_tmp812)));};}_LL12A: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp1BD=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BD->tag != 4)goto _LL12C;else{_tmp1BE=_tmp1BD->f1;}}_LL12B: {
# 1079
struct Cyc_List_List _tmp813;struct Cyc_List_List _tmp226=(_tmp813.hd=_tmp1BE,((_tmp813.tl=0,_tmp813)));
struct _tuple16 _tmp227=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp226);union Cyc_CfFlowInfo_FlowInfo _tmp229;struct _tuple16 _tmp228=_tmp227;_tmp229=_tmp228.f1;{
struct _tuple18 _tmp22A=Cyc_NewControlFlow_anal_Lexp(env,_tmp229,0,0,_tmp1BE);union Cyc_CfFlowInfo_AbsLVal _tmp22C;struct _tuple18 _tmp22B=_tmp22A;_tmp22C=_tmp22B.f2;
{struct _tuple25 _tmp814;struct _tuple25 _tmp22D=(_tmp814.f1=_tmp22C,((_tmp814.f2=_tmp229,_tmp814)));struct _tuple25 _tmp22E=_tmp22D;struct Cyc_CfFlowInfo_Place*_tmp22F;struct Cyc_Dict_Dict _tmp230;struct Cyc_List_List*_tmp231;_LL16B: if(((_tmp22E.f1).PlaceL).tag != 1)goto _LL16D;_tmp22F=(struct Cyc_CfFlowInfo_Place*)((_tmp22E.f1).PlaceL).val;if(((_tmp22E.f2).ReachableFL).tag != 2)goto _LL16D;_tmp230=((struct _tuple14)((_tmp22E.f2).ReachableFL).val).f1;_tmp231=((struct _tuple14)((_tmp22E.f2).ReachableFL).val).f2;_LL16C:
# 1084
 _tmp231=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp231,_tmp1BE);
_tmp229=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1BE->loc,_tmp230,env->all_changed,_tmp22F,_tmp182->unknown_all),_tmp231);
# 1089
goto _LL16A;_LL16D:;_LL16E:
 goto _LL16A;_LL16A:;}{
# 1092
struct _tuple16 _tmp815;return(_tmp815.f1=_tmp229,((_tmp815.f2=_tmp182->unknown_all,_tmp815)));};};}_LL12C: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1BF=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1BF->tag != 3)goto _LL12E;else{_tmp1C0=_tmp1BF->f1;if(_tmp1BF->f2 == 0)goto _LL12E;_tmp1C1=_tmp1BF->f3;}}_LL12D:
# 1095
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp818;void*_tmp817;(_tmp817=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp818="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp818,sizeof(char),62))),_tag_dyneither(_tmp817,sizeof(void*),0)));}{
struct _tuple16 _tmp819;return(_tmp819.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp819.f2=_tmp182->unknown_all,_tmp819)));};}{
# 1099
struct Cyc_List_List _tmp81A;struct Cyc_List_List _tmp238=(_tmp81A.hd=_tmp1C1,((_tmp81A.tl=0,_tmp81A)));
struct Cyc_List_List _tmp81B;struct Cyc_List_List _tmp239=(_tmp81B.hd=_tmp1C0,((_tmp81B.tl=& _tmp238,_tmp81B)));
struct _tuple16 _tmp23A=Cyc_NewControlFlow_anal_use_ints(env,inflow,& _tmp239);union Cyc_CfFlowInfo_FlowInfo _tmp23C;struct _tuple16 _tmp23B=_tmp23A;_tmp23C=_tmp23B.f1;{
struct _tuple18 _tmp23D=Cyc_NewControlFlow_anal_Lexp(env,_tmp23C,0,0,_tmp1C0);union Cyc_CfFlowInfo_AbsLVal _tmp23F;struct _tuple18 _tmp23E=_tmp23D;_tmp23F=_tmp23E.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp240=_tmp23C;struct Cyc_Dict_Dict _tmp241;struct Cyc_List_List*_tmp242;_LL170: if((_tmp240.ReachableFL).tag != 2)goto _LL172;_tmp241=((struct _tuple14)(_tmp240.ReachableFL).val).f1;_tmp242=((struct _tuple14)(_tmp240.ReachableFL).val).f2;_LL171:
# 1105
{union Cyc_CfFlowInfo_AbsLVal _tmp243=_tmp23F;struct Cyc_CfFlowInfo_Place*_tmp244;_LL175: if((_tmp243.PlaceL).tag != 1)goto _LL177;_tmp244=(struct Cyc_CfFlowInfo_Place*)(_tmp243.PlaceL).val;_LL176:
# 1109
 _tmp242=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp242,_tmp1C0);
_tmp241=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1C0->loc,_tmp241,env->all_changed,_tmp244,_tmp182->unknown_all);
# 1112
_tmp23C=Cyc_CfFlowInfo_ReachableFL(_tmp241,_tmp242);
# 1116
goto _LL174;_LL177: if((_tmp243.UnknownL).tag != 2)goto _LL174;_LL178:
# 1119
 goto _LL174;_LL174:;}
# 1121
goto _LL16F;_LL172:;_LL173:
 goto _LL16F;_LL16F:;}{
# 1124
struct _tuple16 _tmp81C;return(_tmp81C.f1=_tmp23C,((_tmp81C.f2=_tmp182->unknown_all,_tmp81C)));};};};_LL12E: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp1C2=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C2->tag != 3)goto _LL130;else{_tmp1C3=_tmp1C2->f1;if(_tmp1C2->f2 != 0)goto _LL130;_tmp1C4=_tmp1C2->f3;}}_LL12F:
# 1128
 if(copy_ctxt  && Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(e->topt))){
{const char*_tmp81F;void*_tmp81E;(_tmp81E=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp81F="cannot track alias-free pointers through multiple assignments",_tag_dyneither(_tmp81F,sizeof(char),62))),_tag_dyneither(_tmp81E,sizeof(void*),0)));}{
struct _tuple16 _tmp820;return(_tmp820.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp820.f2=_tmp182->unknown_all,_tmp820)));};}{
# 1132
struct Cyc_Dict_Dict*_tmp24B=env->all_changed;
# 1134
{struct Cyc_Absyn_Exp*_tmp821[2];inflow=Cyc_NewControlFlow_expand_unique_places(env,inflow,((_tmp821[1]=_tmp1C4,((_tmp821[0]=_tmp1C3,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(env->r,_tag_dyneither(_tmp821,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
while(1){
{struct Cyc_Dict_Dict*_tmp822;env->all_changed=((_tmp822=_region_malloc(env->r,sizeof(*_tmp822)),((_tmp822[0]=_tmp182->mt_place_set,_tmp822))));}{
struct _tuple18 _tmp24E=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1C3);union Cyc_CfFlowInfo_FlowInfo _tmp250;union Cyc_CfFlowInfo_AbsLVal _tmp251;struct _tuple18 _tmp24F=_tmp24E;_tmp250=_tmp24F.f1;_tmp251=_tmp24F.f2;{
struct Cyc_Dict_Dict _tmp252=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
{struct Cyc_Dict_Dict*_tmp823;env->all_changed=((_tmp823=_region_malloc(env->r,sizeof(*_tmp823)),((_tmp823[0]=_tmp182->mt_place_set,_tmp823))));}{
struct _tuple16 _tmp254=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C4);union Cyc_CfFlowInfo_FlowInfo _tmp256;void*_tmp257;struct _tuple16 _tmp255=_tmp254;_tmp256=_tmp255.f1;_tmp257=_tmp255.f2;{
struct Cyc_Dict_Dict _tmp258=*((struct Cyc_Dict_Dict*)_check_null(env->all_changed));
union Cyc_CfFlowInfo_FlowInfo _tmp259=Cyc_CfFlowInfo_after_flow(_tmp182,& _tmp252,_tmp250,_tmp256,_tmp252,_tmp258);
# 1145
union Cyc_CfFlowInfo_FlowInfo _tmp25A=Cyc_CfFlowInfo_join_flow(_tmp182,_tmp24B,inflow,_tmp259);
# 1148
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp25A,inflow)){
if(_tmp24B == 0)
env->all_changed=0;else{
# 1152
*((struct Cyc_Dict_Dict*)_check_null(env->all_changed))=Cyc_CfFlowInfo_union_place_set(*_tmp24B,
Cyc_CfFlowInfo_union_place_set(_tmp252,_tmp258,0),0);}
# 1155
return Cyc_NewControlFlow_do_assign(_tmp182,env,_tmp259,_tmp1C3,_tmp251,_tmp1C4,_tmp257,e->loc);}
# 1158
inflow=_tmp25A;};};};};}};_LL130: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1C5=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C5->tag != 8)goto _LL132;else{_tmp1C6=_tmp1C5->f1;_tmp1C7=_tmp1C5->f2;}}_LL131: {
# 1163
struct _tuple16 _tmp25B=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1C6);union Cyc_CfFlowInfo_FlowInfo _tmp25D;void*_tmp25E;struct _tuple16 _tmp25C=_tmp25B;_tmp25D=_tmp25C.f1;_tmp25E=_tmp25C.f2;
return Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp25D,_tmp1C7);}_LL132: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp1C8=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1C8->tag != 10)goto _LL134;else{_tmp1C9=_tmp1C8->f1;}}_LL133: {
# 1167
struct _tuple16 _tmp25F=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1C9);union Cyc_CfFlowInfo_FlowInfo _tmp261;void*_tmp262;struct _tuple16 _tmp260=_tmp25F;_tmp261=_tmp260.f1;_tmp262=_tmp260.f2;
Cyc_NewControlFlow_use_Rval(env,_tmp1C9->loc,_tmp261,_tmp262);{
struct _tuple16 _tmp824;return(_tmp824.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp824.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp824)));};}_LL134: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp1CA=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1CA->tag != 9)goto _LL136;else{_tmp1CB=_tmp1CA->f1;_tmp1CC=_tmp1CA->f2;}}_LL135: {
# 1172
struct _RegionHandle _tmp264=_new_region("temp");struct _RegionHandle*temp=& _tmp264;_push_region(temp);
{struct Cyc_List_List*_tmp825;struct _tuple23 _tmp265=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,((_tmp825=_region_malloc(temp,sizeof(*_tmp825)),((_tmp825->hd=_tmp1CB,((_tmp825->tl=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(temp,_tmp1CC),_tmp825)))))),0,1);union Cyc_CfFlowInfo_FlowInfo _tmp267;struct Cyc_List_List*_tmp268;struct _tuple23 _tmp266=_tmp265;_tmp267=_tmp266.f1;_tmp268=_tmp266.f2;{
# 1175
union Cyc_CfFlowInfo_FlowInfo _tmp269=Cyc_NewControlFlow_use_Rval(env,_tmp1CB->loc,_tmp267,(void*)((struct Cyc_List_List*)_check_null(_tmp268))->hd);
_tmp268=_tmp268->tl;{
# 1178
struct Cyc_List_List*init_params=0;
struct Cyc_List_List*nolive_unique_params=0;
struct Cyc_List_List*noconsume_params=0;
{void*_tmp26A=Cyc_Tcutil_compress((void*)_check_null(_tmp1CB->topt));void*_tmp26B=_tmp26A;void*_tmp26D;_LL17A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp26C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B;if(_tmp26C->tag != 5)goto _LL17C;else{_tmp26D=(_tmp26C->f1).elt_typ;}}_LL17B:
# 1183
{void*_tmp26E=Cyc_Tcutil_compress(_tmp26D);void*_tmp26F=_tmp26E;struct Cyc_List_List*_tmp271;_LL17F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp270=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp26F;if(_tmp270->tag != 9)goto _LL181;else{_tmp271=(_tmp270->f1).attributes;}}_LL180:
# 1185
 for(0;_tmp271 != 0;_tmp271=_tmp271->tl){
# 1187
void*_tmp272=(void*)_tmp271->hd;void*_tmp273=_tmp272;int _tmp275;int _tmp277;int _tmp279;_LL184: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp274=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp273;if(_tmp274->tag != 20)goto _LL186;else{_tmp275=_tmp274->f1;}}_LL185:
# 1189
{struct Cyc_List_List*_tmp826;init_params=((_tmp826=_region_malloc(temp,sizeof(*_tmp826)),((_tmp826->hd=(void*)_tmp275,((_tmp826->tl=init_params,_tmp826))))));}goto _LL183;_LL186: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp276=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp273;if(_tmp276->tag != 21)goto _LL188;else{_tmp277=_tmp276->f1;}}_LL187:
# 1191
{struct Cyc_List_List*_tmp827;nolive_unique_params=((_tmp827=_region_malloc(temp,sizeof(*_tmp827)),((_tmp827->hd=(void*)_tmp277,((_tmp827->tl=nolive_unique_params,_tmp827))))));}
goto _LL183;_LL188: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp278=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp273;if(_tmp278->tag != 22)goto _LL18A;else{_tmp279=_tmp278->f1;}}_LL189:
# 1195
{struct Cyc_List_List*_tmp828;noconsume_params=((_tmp828=_region_malloc(temp,sizeof(*_tmp828)),((_tmp828->hd=(void*)_tmp279,((_tmp828->tl=noconsume_params,_tmp828))))));}
goto _LL183;_LL18A:;_LL18B:
 goto _LL183;_LL183:;}
# 1199
goto _LL17E;_LL181:;_LL182: {
const char*_tmp82B;void*_tmp82A;(_tmp82A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp82B="anal_Rexp: bad function type",_tag_dyneither(_tmp82B,sizeof(char),29))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}_LL17E:;}
# 1202
goto _LL179;_LL17C:;_LL17D: {
const char*_tmp82E;void*_tmp82D;(_tmp82D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp82E="anal_Rexp: bad function type",_tag_dyneither(_tmp82E,sizeof(char),29))),_tag_dyneither(_tmp82D,sizeof(void*),0)));}_LL179:;}
# 1205
{int i=1;for(0;_tmp268 != 0;(((_tmp268=_tmp268->tl,_tmp1CC=((struct Cyc_List_List*)_check_null(_tmp1CC))->tl)),++ i)){
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(init_params,i)){
union Cyc_CfFlowInfo_FlowInfo _tmp281=_tmp267;struct Cyc_Dict_Dict _tmp282;_LL18D: if((_tmp281.BottomFL).tag != 1)goto _LL18F;_LL18E:
 goto _LL18C;_LL18F: if((_tmp281.ReachableFL).tag != 2)goto _LL18C;_tmp282=((struct _tuple14)(_tmp281.ReachableFL).val).f1;_LL190:
# 1210
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp282,(void*)_tmp268->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp831;void*_tmp830;(_tmp830=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,((_tmp831="expression may not be initialized",_tag_dyneither(_tmp831,sizeof(char),34))),_tag_dyneither(_tmp830,sizeof(void*),0)));}
{union Cyc_CfFlowInfo_FlowInfo _tmp285=_tmp269;struct Cyc_Dict_Dict _tmp286;struct Cyc_List_List*_tmp287;_LL192: if((_tmp285.BottomFL).tag != 1)goto _LL194;_LL193:
 goto _LL191;_LL194: if((_tmp285.ReachableFL).tag != 2)goto _LL191;_tmp286=((struct _tuple14)(_tmp285.ReachableFL).val).f1;_tmp287=((struct _tuple14)(_tmp285.ReachableFL).val).f2;_LL195: {
# 1217
struct Cyc_Dict_Dict _tmp288=Cyc_CfFlowInfo_escape_deref(_tmp182,_tmp286,env->all_changed,(void*)_tmp268->hd);
{void*_tmp289=(void*)_tmp268->hd;void*_tmp28A=_tmp289;struct Cyc_CfFlowInfo_Place*_tmp28C;_LL197: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp28B=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp28A;if(_tmp28B->tag != 5)goto _LL199;else{_tmp28C=_tmp28B->f1;}}_LL198:
# 1220
{void*_tmp28D=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->topt));void*_tmp28E=_tmp28D;void*_tmp290;_LL19C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28E;if(_tmp28F->tag != 5)goto _LL19E;else{_tmp290=(_tmp28F->f1).elt_typ;}}_LL19D:
# 1222
 _tmp288=Cyc_CfFlowInfo_assign_place(_tmp182,((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->loc,_tmp288,env->all_changed,_tmp28C,
# 1224
Cyc_CfFlowInfo_typ_to_absrval(_tmp182,_tmp290,0,_tmp182->esc_all));
# 1227
goto _LL19B;_LL19E:;_LL19F: {
const char*_tmp834;void*_tmp833;(_tmp833=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp834="anal_Rexp:bad type for initialized arg",_tag_dyneither(_tmp834,sizeof(char),39))),_tag_dyneither(_tmp833,sizeof(void*),0)));}_LL19B:;}
# 1230
goto _LL196;_LL199:;_LL19A:
 goto _LL196;_LL196:;}
# 1233
_tmp269=Cyc_CfFlowInfo_ReachableFL(_tmp288,_tmp287);
goto _LL191;}_LL191:;}
# 1236
goto _LL18C;_LL18C:;}else{
# 1239
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(nolive_unique_params,i))
# 1244
_tmp269=Cyc_NewControlFlow_use_nounique_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->topt),_tmp269,(void*)_tmp268->hd);else{
# 1254
_tmp269=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1CC))->hd)->loc,_tmp269,(void*)_tmp268->hd);
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(noconsume_params,i) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->topt),0))
_tmp269=Cyc_NewControlFlow_restore_noconsume_arg(env,_tmp269,(struct Cyc_Absyn_Exp*)_tmp1CC->hd,((struct Cyc_Absyn_Exp*)_tmp1CC->hd)->loc,(void*)_tmp268->hd);}}}}
# 1261
if(Cyc_Tcutil_is_noreturn((void*)_check_null(_tmp1CB->topt))){
struct _tuple16 _tmp835;struct _tuple16 _tmp294=(_tmp835.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp835.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp835)));_npop_handler(0);return _tmp294;}else{
# 1264
struct _tuple16 _tmp836;struct _tuple16 _tmp296=(_tmp836.f1=_tmp269,((_tmp836.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp836)));_npop_handler(0);return _tmp296;}};};}
# 1173
;_pop_region(temp);}_LL136: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp1CD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1CD->tag != 33)goto _LL138;else{_tmp1CE=(_tmp1CD->f1).is_calloc;_tmp1CF=(_tmp1CD->f1).rgn;_tmp1D0=(_tmp1CD->f1).elt_type;_tmp1D1=(_tmp1CD->f1).num_elts;_tmp1D2=(_tmp1CD->f1).fat_result;}}_LL137: {
# 1267
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp839;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp838;void*root=(void*)((_tmp838=_region_malloc(_tmp182->r,sizeof(*_tmp838)),((_tmp838[0]=((_tmp839.tag=1,((_tmp839.f1=_tmp1D1,((_tmp839.f2=(void*)_check_null(e->topt),_tmp839)))))),_tmp838))));
struct Cyc_CfFlowInfo_Place*_tmp83A;struct Cyc_CfFlowInfo_Place*place=(_tmp83A=_region_malloc(_tmp182->r,sizeof(*_tmp83A)),((_tmp83A->root=root,((_tmp83A->fields=0,_tmp83A)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp83D;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp83C;void*rval=(void*)((_tmp83C=_region_malloc(_tmp182->r,sizeof(*_tmp83C)),((_tmp83C[0]=((_tmp83D.tag=5,((_tmp83D.f1=place,_tmp83D)))),_tmp83C))));
void*place_val;
if(_tmp1D2)place_val=_tmp182->notzeroall;else{
if(_tmp1CE)place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,*((void**)_check_null(_tmp1D0)),0,_tmp182->zero);else{
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,*((void**)_check_null(_tmp1D0)),0,_tmp182->unknown_none);}}{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
if(_tmp1CF != 0){
struct _RegionHandle _tmp298=_new_region("temp");struct _RegionHandle*temp=& _tmp298;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp83E[2];struct _tuple23 _tmp299=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp83E[1]=_tmp1D1,((_tmp83E[0]=_tmp1CF,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp83E,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1278
union Cyc_CfFlowInfo_FlowInfo _tmp29B;struct Cyc_List_List*_tmp29C;struct _tuple23 _tmp29A=_tmp299;_tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;
# 1280
outflow=_tmp29B;}
# 1278
;_pop_region(temp);}else{
# 1283
struct _tuple16 _tmp29E=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D1);union Cyc_CfFlowInfo_FlowInfo _tmp2A0;struct _tuple16 _tmp29F=_tmp29E;_tmp2A0=_tmp29F.f1;
outflow=_tmp2A0;}{
# 1286
union Cyc_CfFlowInfo_FlowInfo _tmp2A1=outflow;struct Cyc_Dict_Dict _tmp2A2;struct Cyc_List_List*_tmp2A3;_LL1A1: if((_tmp2A1.BottomFL).tag != 1)goto _LL1A3;_LL1A2: {
struct _tuple16 _tmp83F;return(_tmp83F.f1=outflow,((_tmp83F.f2=rval,_tmp83F)));}_LL1A3: if((_tmp2A1.ReachableFL).tag != 2)goto _LL1A0;_tmp2A2=((struct _tuple14)(_tmp2A1.ReachableFL).val).f1;_tmp2A3=((struct _tuple14)(_tmp2A1.ReachableFL).val).f2;_LL1A4: {
# 1289
struct _tuple16 _tmp840;return(_tmp840.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2A2,root,place_val),_tmp2A3),((_tmp840.f2=rval,_tmp840)));}_LL1A0:;};};}_LL138: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp1D3=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D3->tag != 34)goto _LL13A;else{_tmp1D4=_tmp1D3->f1;_tmp1D5=_tmp1D3->f2;}}_LL139: {
# 1293
void*left_rval;
void*right_rval;
union Cyc_CfFlowInfo_FlowInfo outflow;
# 1299
struct _RegionHandle _tmp2AB=_new_region("temp");struct _RegionHandle*temp=& _tmp2AB;_push_region(temp);{
struct Cyc_Absyn_Exp*_tmp841[2];struct _tuple23 _tmp2AC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp841[1]=_tmp1D5,((_tmp841[0]=_tmp1D4,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp841,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,0);
# 1300
union Cyc_CfFlowInfo_FlowInfo _tmp2AE;struct Cyc_List_List*_tmp2AF;struct _tuple23 _tmp2AD=_tmp2AC;_tmp2AE=_tmp2AD.f1;_tmp2AF=_tmp2AD.f2;
# 1302
left_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF))->hd;
right_rval=(void*)((struct Cyc_List_List*)_check_null(_tmp2AF->tl))->hd;
outflow=_tmp2AE;}{
# 1307
struct _tuple18 _tmp2B1=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D4);union Cyc_CfFlowInfo_AbsLVal _tmp2B3;struct _tuple18 _tmp2B2=_tmp2B1;_tmp2B3=_tmp2B2.f2;{
struct _tuple18 _tmp2B4=Cyc_NewControlFlow_anal_Lexp(env,outflow,0,0,_tmp1D5);union Cyc_CfFlowInfo_AbsLVal _tmp2B6;struct _tuple18 _tmp2B5=_tmp2B4;_tmp2B6=_tmp2B5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp2B7=outflow;struct Cyc_Dict_Dict _tmp2B8;struct Cyc_List_List*_tmp2B9;_LL1A6: if((_tmp2B7.ReachableFL).tag != 2)goto _LL1A8;_tmp2B8=((struct _tuple14)(_tmp2B7.ReachableFL).val).f1;_tmp2B9=((struct _tuple14)(_tmp2B7.ReachableFL).val).f2;_LL1A7:
# 1311
{union Cyc_CfFlowInfo_AbsLVal _tmp2BA=_tmp2B3;struct Cyc_CfFlowInfo_Place*_tmp2BB;_LL1AB: if((_tmp2BA.PlaceL).tag != 1)goto _LL1AD;_tmp2BB=(struct Cyc_CfFlowInfo_Place*)(_tmp2BA.PlaceL).val;_LL1AC:
# 1313
 _tmp2B8=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1D4->loc,_tmp2B8,env->all_changed,_tmp2BB,right_rval);
# 1315
goto _LL1AA;_LL1AD: if((_tmp2BA.UnknownL).tag != 2)goto _LL1AA;_LL1AE:
# 1320
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp2B8,right_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp844;void*_tmp843;(_tmp843=0,Cyc_Tcutil_terr(_tmp1D5->loc,((_tmp844="expression may not be fully initialized",_tag_dyneither(_tmp844,sizeof(char),40))),_tag_dyneither(_tmp843,sizeof(void*),0)));}
goto _LL1AA;_LL1AA:;}
# 1324
{union Cyc_CfFlowInfo_AbsLVal _tmp2BE=_tmp2B6;struct Cyc_CfFlowInfo_Place*_tmp2BF;_LL1B0: if((_tmp2BE.PlaceL).tag != 1)goto _LL1B2;_tmp2BF=(struct Cyc_CfFlowInfo_Place*)(_tmp2BE.PlaceL).val;_LL1B1:
# 1326
 _tmp2B8=Cyc_CfFlowInfo_assign_place(_tmp182,_tmp1D5->loc,_tmp2B8,env->all_changed,_tmp2BF,left_rval);
# 1328
goto _LL1AF;_LL1B2: if((_tmp2BE.UnknownL).tag != 2)goto _LL1AF;_LL1B3:
# 1330
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp2B8,left_rval)!= Cyc_CfFlowInfo_AllIL){
const char*_tmp847;void*_tmp846;(_tmp846=0,Cyc_Tcutil_terr(_tmp1D4->loc,((_tmp847="expression may not be fully initialized",_tag_dyneither(_tmp847,sizeof(char),40))),_tag_dyneither(_tmp846,sizeof(void*),0)));}
goto _LL1AF;_LL1AF:;}
# 1335
_tmp2B9=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp2B9,_tmp1D4);
_tmp2B9=Cyc_Relations_reln_kill_exp(_tmp182->r,_tmp2B9,_tmp1D5);
# 1338
outflow=Cyc_CfFlowInfo_ReachableFL(_tmp2B8,_tmp2B9);
Cyc_NewControlFlow_update_tryflow(env,outflow);
goto _LL1A5;_LL1A8:;_LL1A9:
 goto _LL1A5;_LL1A5:;}{
# 1345
struct _tuple16 _tmp848;struct _tuple16 _tmp2C3=(_tmp848.f1=outflow,((_tmp848.f2=_tmp182->unknown_all,_tmp848)));_npop_handler(0);return _tmp2C3;};};};
# 1299
;_pop_region(temp);}_LL13A: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D6->tag != 15)goto _LL13C;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL13B: {
# 1348
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp84B;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp84A;void*root=(void*)((_tmp84A=_region_malloc(_tmp182->r,sizeof(*_tmp84A)),((_tmp84A[0]=((_tmp84B.tag=1,((_tmp84B.f1=_tmp1D8,((_tmp84B.f2=(void*)_check_null(e->topt),_tmp84B)))))),_tmp84A))));
struct Cyc_CfFlowInfo_Place*_tmp84C;struct Cyc_CfFlowInfo_Place*place=(_tmp84C=_region_malloc(_tmp182->r,sizeof(*_tmp84C)),((_tmp84C->root=root,((_tmp84C->fields=0,_tmp84C)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp84F;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp84E;void*rval=(void*)((_tmp84E=_region_malloc(_tmp182->r,sizeof(*_tmp84E)),((_tmp84E[0]=((_tmp84F.tag=5,((_tmp84F.f1=place,_tmp84F)))),_tmp84E))));
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
union Cyc_CfFlowInfo_FlowInfo outflow;
void*place_val;
if(_tmp1D7 != 0){
struct _RegionHandle _tmp2C4=_new_region("temp");struct _RegionHandle*temp=& _tmp2C4;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp850[2];struct _tuple23 _tmp2C5=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp850[1]=_tmp1D8,((_tmp850[0]=_tmp1D7,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp850,sizeof(struct Cyc_Absyn_Exp*),2)))))),1,1);
# 1356
union Cyc_CfFlowInfo_FlowInfo _tmp2C7;struct Cyc_List_List*_tmp2C8;struct _tuple23 _tmp2C6=_tmp2C5;_tmp2C7=_tmp2C6.f1;_tmp2C8=_tmp2C6.f2;
# 1358
outflow=_tmp2C7;
place_val=(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp2C8))->tl))->hd;}
# 1356
;_pop_region(temp);}else{
# 1362
struct _tuple16 _tmp2CA=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp1D8);union Cyc_CfFlowInfo_FlowInfo _tmp2CC;void*_tmp2CD;struct _tuple16 _tmp2CB=_tmp2CA;_tmp2CC=_tmp2CB.f1;_tmp2CD=_tmp2CB.f2;
outflow=_tmp2CC;
place_val=_tmp2CD;}{
# 1366
union Cyc_CfFlowInfo_FlowInfo _tmp2CE=outflow;struct Cyc_Dict_Dict _tmp2CF;struct Cyc_List_List*_tmp2D0;_LL1B5: if((_tmp2CE.BottomFL).tag != 1)goto _LL1B7;_LL1B6: {
struct _tuple16 _tmp851;return(_tmp851.f1=outflow,((_tmp851.f2=rval,_tmp851)));}_LL1B7: if((_tmp2CE.ReachableFL).tag != 2)goto _LL1B4;_tmp2CF=((struct _tuple14)(_tmp2CE.ReachableFL).val).f1;_tmp2D0=((struct _tuple14)(_tmp2CE.ReachableFL).val).f2;_LL1B8: {
# 1369
struct _tuple16 _tmp852;return(_tmp852.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp2CF,root,place_val),_tmp2D0),((_tmp852.f2=rval,_tmp852)));}_LL1B4:;};};}_LL13C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1D9=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1D9->tag != 14)goto _LL13E;else{_tmp1DA=_tmp1D9->f1;}}_LL13D: {
# 1373
struct _tuple18 _tmp2D8=Cyc_NewControlFlow_anal_Lexp(env,inflow,0,0,_tmp1DA);union Cyc_CfFlowInfo_FlowInfo _tmp2DA;union Cyc_CfFlowInfo_AbsLVal _tmp2DB;struct _tuple18 _tmp2D9=_tmp2D8;_tmp2DA=_tmp2D9.f1;_tmp2DB=_tmp2D9.f2;{
union Cyc_CfFlowInfo_AbsLVal _tmp2DC=_tmp2DB;struct Cyc_CfFlowInfo_Place*_tmp2DD;_LL1BA: if((_tmp2DC.UnknownL).tag != 2)goto _LL1BC;_LL1BB: {
struct _tuple16 _tmp853;return(_tmp853.f1=_tmp2DA,((_tmp853.f2=_tmp182->notzeroall,_tmp853)));}_LL1BC: if((_tmp2DC.PlaceL).tag != 1)goto _LL1B9;_tmp2DD=(struct Cyc_CfFlowInfo_Place*)(_tmp2DC.PlaceL).val;_LL1BD: {
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp859;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp858;struct _tuple16 _tmp857;return(_tmp857.f1=_tmp2DA,((_tmp857.f2=(void*)((_tmp859=_region_malloc(env->r,sizeof(*_tmp859)),((_tmp859[0]=((_tmp858.tag=5,((_tmp858.f1=_tmp2DD,_tmp858)))),_tmp859)))),_tmp857)));}_LL1B9:;};}_LL13E: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1DB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1DB->tag != 19)goto _LL140;else{_tmp1DC=_tmp1DB->f1;}}_LL13F: {
# 1380
struct _tuple16 _tmp2E2=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DC);union Cyc_CfFlowInfo_FlowInfo _tmp2E4;void*_tmp2E5;struct _tuple16 _tmp2E3=_tmp2E2;_tmp2E4=_tmp2E3.f1;_tmp2E5=_tmp2E3.f2;
return Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp2E4,_tmp1DC,_tmp2E5);}_LL140: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1DD=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1DD->tag != 20)goto _LL142;else{_tmp1DE=_tmp1DD->f1;_tmp1DF=_tmp1DD->f2;}}_LL141: {
# 1384
struct _tuple16 _tmp2E6=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1DE);union Cyc_CfFlowInfo_FlowInfo _tmp2E8;void*_tmp2E9;struct _tuple16 _tmp2E7=_tmp2E6;_tmp2E8=_tmp2E7.f1;_tmp2E9=_tmp2E7.f2;
if(_tmp1DE->topt == 0){
{const char*_tmp85D;void*_tmp85C[1];struct Cyc_String_pa_PrintArg_struct _tmp85B;(_tmp85B.tag=0,((_tmp85B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp85C[0]=& _tmp85B,Cyc_fprintf(Cyc_stderr,((_tmp85D="aggrmember exp %s\n",_tag_dyneither(_tmp85D,sizeof(char),19))),_tag_dyneither(_tmp85C,sizeof(void*),1)))))));}{
const char*_tmp861;void*_tmp860[1];struct Cyc_String_pa_PrintArg_struct _tmp85F;(_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1DE)),((_tmp860[0]=& _tmp85F,Cyc_fprintf(Cyc_stderr,((_tmp861="oops! %s.topt = null!\n",_tag_dyneither(_tmp861,sizeof(char),23))),_tag_dyneither(_tmp860,sizeof(void*),1)))))));};}{
# 1389
void*_tmp2F0=(void*)_check_null(_tmp1DE->topt);
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp2F0)){
# 1392
struct _tuple16 _tmp862;return(_tmp862.f1=_tmp2E8,((_tmp862.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp862)));}
# 1394
if(Cyc_Absyn_is_require_union_type(_tmp2F0))
Cyc_NewControlFlow_check_requires(_tmp1DE->loc,_tmp182->r,_tmp2F0,_tmp1DF,_tmp2E8);{
# 1397
struct _tuple15 _tmp2F2=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp2E9);void*_tmp2F4;struct _tuple15 _tmp2F3=_tmp2F2;_tmp2F4=_tmp2F3.f1;{
void*_tmp2F5=_tmp2F4;int _tmp2F7;int _tmp2F8;struct _dyneither_ptr _tmp2F9;_LL1BF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp2F6=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp2F5;if(_tmp2F6->tag != 6)goto _LL1C1;else{_tmp2F7=(_tmp2F6->f1).is_union;_tmp2F8=(_tmp2F6->f1).fieldnum;_tmp2F9=_tmp2F6->f2;}}_LL1C0: {
# 1400
int _tmp2FA=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1DE->topt),_tmp1DF);
if((_tmp2F7  && _tmp2F8 != - 1) && _tmp2F8 != _tmp2FA){
struct _tuple16 _tmp863;return(_tmp863.f1=_tmp2E8,((_tmp863.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),1,_tmp182->unknown_none),_tmp863)));}{
struct _tuple16 _tmp864;return(_tmp864.f1=_tmp2E8,((_tmp864.f2=*((void**)_check_dyneither_subscript(_tmp2F9,sizeof(void*),_tmp2FA)),_tmp864)));};}_LL1C1:;_LL1C2:
# 1405
{const char*_tmp867;void*_tmp866;(_tmp866=0,Cyc_fprintf(Cyc_stderr,((_tmp867="the bad rexp is :",_tag_dyneither(_tmp867,sizeof(char),18))),_tag_dyneither(_tmp866,sizeof(void*),0)));}
Cyc_CfFlowInfo_print_absrval(_tmp2F4);
{const char*_tmp86A;void*_tmp869;(_tmp869=0,Cyc_fprintf(Cyc_stderr,((_tmp86A="\n",_tag_dyneither(_tmp86A,sizeof(char),2))),_tag_dyneither(_tmp869,sizeof(void*),0)));}{
struct Cyc_Core_Impossible_exn_struct _tmp877;const char*_tmp876;void*_tmp875[1];struct Cyc_String_pa_PrintArg_struct _tmp874;struct Cyc_Core_Impossible_exn_struct*_tmp873;(int)_throw((void*)((_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873[0]=((_tmp877.tag=Cyc_Core_Impossible,((_tmp877.f1=(struct _dyneither_ptr)((_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp875[0]=& _tmp874,Cyc_aprintf(((_tmp876="anal_Rexp: AggrMember: %s",_tag_dyneither(_tmp876,sizeof(char),26))),_tag_dyneither(_tmp875,sizeof(void*),1)))))))),_tmp877)))),_tmp873)))));};_LL1BE:;};};};}_LL142: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp1E0=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E0->tag != 37)goto _LL144;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;}}_LL143: {
# 1415
struct _tuple16 _tmp306=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E1);union Cyc_CfFlowInfo_FlowInfo _tmp308;void*_tmp309;struct _tuple16 _tmp307=_tmp306;_tmp308=_tmp307.f1;_tmp309=_tmp307.f2;
# 1417
if(Cyc_Absyn_is_nontagged_nonrequire_union_type((void*)_check_null(_tmp1E1->topt))){
struct _tuple16 _tmp878;return(_tmp878.f1=_tmp308,((_tmp878.f2=_tmp182->unknown_all,_tmp878)));}{
struct _tuple15 _tmp30B=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp309);void*_tmp30D;struct _tuple15 _tmp30C=_tmp30B;_tmp30D=_tmp30C.f1;{
void*_tmp30E=_tmp30D;int _tmp310;int _tmp311;struct _dyneither_ptr _tmp312;_LL1C4: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp30F=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp30E;if(_tmp30F->tag != 6)goto _LL1C6;else{_tmp310=(_tmp30F->f1).is_union;_tmp311=(_tmp30F->f1).fieldnum;_tmp312=_tmp30F->f2;}}_LL1C5: {
# 1422
int _tmp313=Cyc_CfFlowInfo_get_field_index((void*)_check_null(_tmp1E1->topt),_tmp1E2);
if(_tmp310  && _tmp311 != - 1){
if(_tmp311 != _tmp313){
struct _tuple16 _tmp879;return(_tmp879.f1=_tmp308,((_tmp879.f2=_tmp182->zero,_tmp879)));}else{
# 1427
struct _tuple16 _tmp87A;return(_tmp87A.f1=_tmp308,((_tmp87A.f2=_tmp182->notzeroall,_tmp87A)));}}else{
# 1429
struct _tuple16 _tmp87B;return(_tmp87B.f1=_tmp308,((_tmp87B.f2=_tmp182->unknown_all,_tmp87B)));}}_LL1C6:;_LL1C7: {
# 1431
struct Cyc_Core_Impossible_exn_struct _tmp888;const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_PrintArg_struct _tmp885;struct Cyc_Core_Impossible_exn_struct*_tmp884;(int)_throw((void*)((_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=((_tmp888.tag=Cyc_Core_Impossible,((_tmp888.f1=(struct _dyneither_ptr)((_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp886[0]=& _tmp885,Cyc_aprintf(((_tmp887="anal_Rexp: TagCheck_e: %s",_tag_dyneither(_tmp887,sizeof(char),26))),_tag_dyneither(_tmp886,sizeof(void*),1)))))))),_tmp888)))),_tmp884)))));}_LL1C3:;};};}_LL144: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1E3=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E3->tag != 21)goto _LL146;else{_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}}_LL145: {
# 1436
struct _tuple16 _tmp31C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp1E4);union Cyc_CfFlowInfo_FlowInfo _tmp31E;void*_tmp31F;struct _tuple16 _tmp31D=_tmp31C;_tmp31E=_tmp31D.f1;_tmp31F=_tmp31D.f2;{
# 1439
struct _tuple16 _tmp320=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp31E,_tmp1E4,_tmp31F);union Cyc_CfFlowInfo_FlowInfo _tmp322;void*_tmp323;struct _tuple16 _tmp321=_tmp320;_tmp322=_tmp321.f1;_tmp323=_tmp321.f2;{
# 1442
void*_tmp324=Cyc_Tcutil_compress((void*)_check_null(_tmp1E4->topt));void*_tmp325=_tmp324;void*_tmp327;_LL1C9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp326=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp325;if(_tmp326->tag != 5)goto _LL1CB;else{_tmp327=(_tmp326->f1).elt_typ;}}_LL1CA:
# 1444
 if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp327)){
# 1446
struct _tuple16 _tmp889;return(_tmp889.f1=_tmp322,((_tmp889.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp889)));}
# 1448
if(Cyc_Absyn_is_require_union_type(_tmp327))
Cyc_NewControlFlow_check_requires(_tmp1E4->loc,_tmp182->r,_tmp327,_tmp1E5,_tmp31E);{
# 1451
struct _tuple15 _tmp329=Cyc_CfFlowInfo_unname_rval(_tmp182->r,_tmp323);void*_tmp32B;struct _tuple15 _tmp32A=_tmp329;_tmp32B=_tmp32A.f1;{
void*_tmp32C=_tmp32B;int _tmp32E;int _tmp32F;struct _dyneither_ptr _tmp330;_LL1CE: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp32D=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp32C;if(_tmp32D->tag != 6)goto _LL1D0;else{_tmp32E=(_tmp32D->f1).is_union;_tmp32F=(_tmp32D->f1).fieldnum;_tmp330=_tmp32D->f2;}}_LL1CF: {
# 1454
int _tmp331=Cyc_CfFlowInfo_get_field_index(_tmp327,_tmp1E5);
if((_tmp32E  && _tmp32F != - 1) && _tmp32F != _tmp331){
struct _tuple16 _tmp88A;return(_tmp88A.f1=_tmp322,((_tmp88A.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),1,_tmp182->unknown_none),_tmp88A)));}{
struct _tuple16 _tmp88B;return(_tmp88B.f1=_tmp322,((_tmp88B.f2=*((void**)_check_dyneither_subscript(_tmp330,sizeof(void*),_tmp331)),_tmp88B)));};}_LL1D0:;_LL1D1: {
struct Cyc_Core_Impossible_exn_struct _tmp891;const char*_tmp890;struct Cyc_Core_Impossible_exn_struct*_tmp88F;(int)_throw((void*)((_tmp88F=_cycalloc(sizeof(*_tmp88F)),((_tmp88F[0]=((_tmp891.tag=Cyc_Core_Impossible,((_tmp891.f1=((_tmp890="anal_Rexp: AggrArrow",_tag_dyneither(_tmp890,sizeof(char),21))),_tmp891)))),_tmp88F)))));}_LL1CD:;};};_LL1CB:;_LL1CC: {
# 1460
struct Cyc_Core_Impossible_exn_struct _tmp897;const char*_tmp896;struct Cyc_Core_Impossible_exn_struct*_tmp895;(int)_throw((void*)((_tmp895=_cycalloc(sizeof(*_tmp895)),((_tmp895[0]=((_tmp897.tag=Cyc_Core_Impossible,((_tmp897.f1=((_tmp896="anal_Rexp: AggrArrow ptr",_tag_dyneither(_tmp896,sizeof(char),25))),_tmp897)))),_tmp895)))));}_LL1C8:;};};}_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1E6=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1E6->tag != 5)goto _LL148;else{_tmp1E7=_tmp1E6->f1;_tmp1E8=_tmp1E6->f2;_tmp1E9=_tmp1E6->f3;}}_LL147: {
# 1464
struct _tuple19 _tmp33A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1E7);union Cyc_CfFlowInfo_FlowInfo _tmp33C;union Cyc_CfFlowInfo_FlowInfo _tmp33D;struct _tuple19 _tmp33B=_tmp33A;_tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;{
struct _tuple16 _tmp33E=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33C,_tmp1E8);
struct _tuple16 _tmp33F=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp33D,_tmp1E9);
# 1468
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp33E,_tmp33F);};}_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1EA=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1EA->tag != 6)goto _LL14A;else{_tmp1EB=_tmp1EA->f1;_tmp1EC=_tmp1EA->f2;}}_LL149: {
# 1471
struct _tuple19 _tmp340=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EB);union Cyc_CfFlowInfo_FlowInfo _tmp342;union Cyc_CfFlowInfo_FlowInfo _tmp343;struct _tuple19 _tmp341=_tmp340;_tmp342=_tmp341.f1;_tmp343=_tmp341.f2;{
struct _tuple16 _tmp344=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp342,_tmp1EC);union Cyc_CfFlowInfo_FlowInfo _tmp346;void*_tmp347;struct _tuple16 _tmp345=_tmp344;_tmp346=_tmp345.f1;_tmp347=_tmp345.f2;{
struct _tuple16 _tmp898;struct _tuple16 _tmp348=(_tmp898.f1=_tmp346,((_tmp898.f2=_tmp347,_tmp898)));
struct _tuple16 _tmp899;struct _tuple16 _tmp349=(_tmp899.f1=_tmp343,((_tmp899.f2=_tmp182->zero,_tmp899)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp348,_tmp349);};};}_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1ED=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1ED->tag != 7)goto _LL14C;else{_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;}}_LL14B: {
# 1478
struct _tuple19 _tmp34C=Cyc_NewControlFlow_anal_test(env,inflow,_tmp1EE);union Cyc_CfFlowInfo_FlowInfo _tmp34E;union Cyc_CfFlowInfo_FlowInfo _tmp34F;struct _tuple19 _tmp34D=_tmp34C;_tmp34E=_tmp34D.f1;_tmp34F=_tmp34D.f2;{
struct _tuple16 _tmp350=Cyc_NewControlFlow_anal_Rexp(env,copy_ctxt,_tmp34F,_tmp1EF);union Cyc_CfFlowInfo_FlowInfo _tmp352;void*_tmp353;struct _tuple16 _tmp351=_tmp350;_tmp352=_tmp351.f1;_tmp353=_tmp351.f2;{
struct _tuple16 _tmp89A;struct _tuple16 _tmp354=(_tmp89A.f1=_tmp352,((_tmp89A.f2=_tmp353,_tmp89A)));
struct _tuple16 _tmp89B;struct _tuple16 _tmp355=(_tmp89B.f1=_tmp34E,((_tmp89B.f2=_tmp182->notzeroall,_tmp89B)));
return Cyc_CfFlowInfo_join_flow_and_rval(_tmp182,env->all_changed,_tmp354,_tmp355);};};}_LL14C: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1F0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F0->tag != 22)goto _LL14E;else{_tmp1F1=_tmp1F0->f1;_tmp1F2=_tmp1F0->f2;}}_LL14D: {
# 1485
struct _RegionHandle _tmp358=_new_region("temp");struct _RegionHandle*temp=& _tmp358;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp89C[2];struct _tuple23 _tmp359=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp89C[1]=_tmp1F2,((_tmp89C[0]=_tmp1F1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp89C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1486
union Cyc_CfFlowInfo_FlowInfo _tmp35B;struct Cyc_List_List*_tmp35C;struct _tuple23 _tmp35A=_tmp359;_tmp35B=_tmp35A.f1;_tmp35C=_tmp35A.f2;{
# 1490
union Cyc_CfFlowInfo_FlowInfo _tmp35D=_tmp35B;
{union Cyc_CfFlowInfo_FlowInfo _tmp35E=_tmp35B;struct Cyc_Dict_Dict _tmp35F;struct Cyc_List_List*_tmp360;_LL1D3: if((_tmp35E.ReachableFL).tag != 2)goto _LL1D5;_tmp35F=((struct _tuple14)(_tmp35E.ReachableFL).val).f1;_tmp360=((struct _tuple14)(_tmp35E.ReachableFL).val).f2;_LL1D4:
# 1495
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp35F,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp35C))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp89F;void*_tmp89E;(_tmp89E=0,Cyc_CfFlowInfo_aerr(_tmp1F2->loc,((_tmp89F="expression may not be initialized",_tag_dyneither(_tmp89F,sizeof(char),34))),_tag_dyneither(_tmp89E,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp363=Cyc_NewControlFlow_add_subscript_reln(_tmp182->r,_tmp360,_tmp1F1,_tmp1F2);
if(_tmp360 != _tmp363)
_tmp35D=Cyc_CfFlowInfo_ReachableFL(_tmp35F,_tmp363);
goto _LL1D2;};_LL1D5:;_LL1D6:
 goto _LL1D2;_LL1D2:;}{
# 1503
void*_tmp364=Cyc_Tcutil_compress((void*)_check_null(_tmp1F1->topt));void*_tmp365=_tmp364;struct Cyc_List_List*_tmp367;union Cyc_Absyn_Constraint*_tmp369;_LL1D8: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp365;if(_tmp366->tag != 10)goto _LL1DA;else{_tmp367=_tmp366->f1;}}_LL1D9: {
# 1505
struct _tuple15 _tmp36A=Cyc_CfFlowInfo_unname_rval(_tmp182->r,(void*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd);void*_tmp36C;struct _tuple15 _tmp36B=_tmp36A;_tmp36C=_tmp36B.f1;{
void*_tmp36D=_tmp36C;struct _dyneither_ptr _tmp36F;_LL1DF: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp36E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp36D;if(_tmp36E->tag != 6)goto _LL1E1;else{_tmp36F=_tmp36E->f2;}}_LL1E0: {
# 1508
unsigned int i=(Cyc_Evexp_eval_const_uint_exp(_tmp1F2)).f1;
struct _tuple16 _tmp8A0;struct _tuple16 _tmp371=(_tmp8A0.f1=_tmp35D,((_tmp8A0.f2=*((void**)_check_dyneither_subscript(_tmp36F,sizeof(void*),(int)i)),_tmp8A0)));_npop_handler(0);return _tmp371;}_LL1E1:;_LL1E2: {
struct Cyc_Core_Impossible_exn_struct _tmp8A6;const char*_tmp8A5;struct Cyc_Core_Impossible_exn_struct*_tmp8A4;(int)_throw((void*)((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4[0]=((_tmp8A6.tag=Cyc_Core_Impossible,((_tmp8A6.f1=((_tmp8A5="anal_Rexp: Subscript",_tag_dyneither(_tmp8A5,sizeof(char),21))),_tmp8A6)))),_tmp8A4)))));}_LL1DE:;};}_LL1DA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp365;if(_tmp368->tag != 5)goto _LL1DC;else{_tmp369=((_tmp368->f1).ptr_atts).bounds;}}_LL1DB: {
# 1513
struct _tuple16 _tmp375=Cyc_NewControlFlow_anal_derefR(env,inflow,_tmp35B,_tmp1F1,(void*)((struct Cyc_List_List*)_check_null(_tmp35C))->hd);union Cyc_CfFlowInfo_FlowInfo _tmp377;void*_tmp378;struct _tuple16 _tmp376=_tmp375;_tmp377=_tmp376.f1;_tmp378=_tmp376.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp379=_tmp377;_LL1E4: if((_tmp379.BottomFL).tag != 1)goto _LL1E6;_LL1E5: {
struct _tuple16 _tmp8A7;struct _tuple16 _tmp37B=(_tmp8A7.f1=_tmp377,((_tmp8A7.f2=_tmp378,_tmp8A7)));_npop_handler(0);return _tmp37B;}_LL1E6:;_LL1E7: {
struct _tuple16 _tmp8A8;struct _tuple16 _tmp37D=(_tmp8A8.f1=_tmp35D,((_tmp8A8.f2=_tmp378,_tmp8A8)));_npop_handler(0);return _tmp37D;}_LL1E3:;};}_LL1DC:;_LL1DD: {
# 1518
struct Cyc_Core_Impossible_exn_struct _tmp8AE;const char*_tmp8AD;struct Cyc_Core_Impossible_exn_struct*_tmp8AC;(int)_throw((void*)((_tmp8AC=_cycalloc(sizeof(*_tmp8AC)),((_tmp8AC[0]=((_tmp8AE.tag=Cyc_Core_Impossible,((_tmp8AE.f1=((_tmp8AD="anal_Rexp: Subscript -- bad type",_tag_dyneither(_tmp8AD,sizeof(char),33))),_tmp8AE)))),_tmp8AC)))));}_LL1D7:;};};}
# 1486
;_pop_region(temp);}_LL14E: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp1F3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F3->tag != 30)goto _LL150;else{_tmp1F4=_tmp1F3->f1;_tmp1F5=_tmp1F3->f2;}}_LL14F:
# 1523
 _tmp1F7=_tmp1F4;goto _LL151;_LL150: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp1F6=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F6->tag != 23)goto _LL152;else{_tmp1F7=_tmp1F6->f1;}}_LL151: {
# 1525
struct _RegionHandle _tmp382=_new_region("temp");struct _RegionHandle*temp=& _tmp382;_push_region(temp);
{struct _tuple23 _tmp383=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp1F7,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp385;struct Cyc_List_List*_tmp386;struct _tuple23 _tmp384=_tmp383;_tmp385=_tmp384.f1;_tmp386=_tmp384.f2;{
unsigned int _tmp8B4;unsigned int _tmp8B3;struct _dyneither_ptr _tmp8B2;void**_tmp8B1;unsigned int _tmp8B0;struct _dyneither_ptr aggrdict=
(_tmp8B0=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp1F7),((_tmp8B1=(void**)_region_malloc(env->r,_check_times(sizeof(void*),_tmp8B0)),((_tmp8B2=_tag_dyneither(_tmp8B1,sizeof(void*),_tmp8B0),((((_tmp8B3=_tmp8B0,_tmp8B5(& _tmp8B4,& _tmp8B3,& _tmp8B1,& _tmp386))),_tmp8B2)))))));
# 1533
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8BF;struct Cyc_CfFlowInfo_UnionRInfo _tmp8BE;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8BD;struct _tuple16 _tmp8BC;struct _tuple16 _tmp38B=(_tmp8BC.f1=_tmp385,((_tmp8BC.f2=(void*)((_tmp8BF=_region_malloc(env->r,sizeof(*_tmp8BF)),((_tmp8BF[0]=((_tmp8BD.tag=6,((_tmp8BD.f1=((_tmp8BE.is_union=0,((_tmp8BE.fieldnum=- 1,_tmp8BE)))),((_tmp8BD.f2=aggrdict,_tmp8BD)))))),_tmp8BF)))),_tmp8BC)));_npop_handler(0);return _tmp38B;};}
# 1526
;_pop_region(temp);}_LL152: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp1F8=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1F8->tag != 29)goto _LL154;else{_tmp1F9=_tmp1F8->f2;}}_LL153: {
# 1536
struct Cyc_List_List*fs;
{void*_tmp391=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp392=_tmp391;struct Cyc_List_List*_tmp394;_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp393=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp392;if(_tmp393->tag != 12)goto _LL1EB;else{_tmp394=_tmp393->f2;}}_LL1EA:
 fs=_tmp394;goto _LL1E8;_LL1EB:;_LL1EC: {
struct Cyc_Core_Impossible_exn_struct _tmp8C5;const char*_tmp8C4;struct Cyc_Core_Impossible_exn_struct*_tmp8C3;(int)_throw((void*)((_tmp8C3=_cycalloc(sizeof(*_tmp8C3)),((_tmp8C3[0]=((_tmp8C5.tag=Cyc_Core_Impossible,((_tmp8C5.f1=((_tmp8C4="anal_Rexp:anon struct has bad type",_tag_dyneither(_tmp8C4,sizeof(char),35))),_tmp8C5)))),_tmp8C3)))));}_LL1E8:;}
# 1541
_tmp1FB=_tmp1F9;_tmp1FC=Cyc_Absyn_StructA;_tmp1FD=fs;goto _LL155;}_LL154: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FA=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FA->tag != 28)goto _LL156;else{_tmp1FB=_tmp1FA->f3;if(_tmp1FA->f4 == 0)goto _LL156;_tmp1FC=(_tmp1FA->f4)->kind;if((_tmp1FA->f4)->impl == 0)goto _LL156;_tmp1FD=((_tmp1FA->f4)->impl)->fields;}}_LL155: {
# 1543
void*exp_type=(void*)_check_null(e->topt);
struct _RegionHandle _tmp398=_new_region("temp");struct _RegionHandle*temp=& _tmp398;_push_region(temp);
{struct _tuple23 _tmp399=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp1FB),1,1);
# 1545
union Cyc_CfFlowInfo_FlowInfo _tmp39B;struct Cyc_List_List*_tmp39C;struct _tuple23 _tmp39A=_tmp399;_tmp39B=_tmp39A.f1;_tmp39C=_tmp39A.f2;{
# 1547
struct _dyneither_ptr aggrdict=
Cyc_CfFlowInfo_aggrfields_to_aggrdict(_tmp182,_tmp1FD,_tmp1FC == Cyc_Absyn_UnionA,_tmp182->unknown_all);
# 1550
int field_no=-1;
{int i=0;for(0;_tmp39C != 0;(((_tmp39C=_tmp39C->tl,_tmp1FB=_tmp1FB->tl)),++ i)){
struct Cyc_List_List*ds=(*((struct _tuple26*)((struct Cyc_List_List*)_check_null(_tmp1FB))->hd)).f1;for(0;ds != 0;ds=ds->tl){
void*_tmp39D=(void*)ds->hd;void*_tmp39E=_tmp39D;struct _dyneither_ptr*_tmp3A1;_LL1EE: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp39F=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp39E;if(_tmp39F->tag != 0)goto _LL1F0;}_LL1EF: {
struct Cyc_Core_Impossible_exn_struct _tmp8CB;const char*_tmp8CA;struct Cyc_Core_Impossible_exn_struct*_tmp8C9;(int)_throw((void*)((_tmp8C9=_cycalloc(sizeof(*_tmp8C9)),((_tmp8C9[0]=((_tmp8CB.tag=Cyc_Core_Impossible,((_tmp8CB.f1=((_tmp8CA="anal_Rexp:Aggregate_e",_tag_dyneither(_tmp8CA,sizeof(char),22))),_tmp8CB)))),_tmp8C9)))));}_LL1F0: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3A0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp39E;if(_tmp3A0->tag != 1)goto _LL1ED;else{_tmp3A1=_tmp3A0->f1;}}_LL1F1:
# 1557
 field_no=Cyc_CfFlowInfo_get_field_index_fs(_tmp1FD,_tmp3A1);
*((void**)_check_dyneither_subscript(aggrdict,sizeof(void*),field_no))=(void*)_tmp39C->hd;
# 1560
if(_tmp1FC == Cyc_Absyn_UnionA){
struct Cyc_Absyn_Exp*_tmp3A5=(*((struct _tuple26*)_tmp1FB->hd)).f2;
_tmp39B=Cyc_NewControlFlow_use_Rval(env,_tmp3A5->loc,_tmp39B,(void*)_tmp39C->hd);
# 1564
Cyc_NewControlFlow_check_requires(_tmp3A5->loc,_tmp182->r,exp_type,_tmp3A1,_tmp39B);}_LL1ED:;}}}{
# 1567
struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct _tmp8D1;struct Cyc_CfFlowInfo_UnionRInfo _tmp8D0;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp8CF;struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp3A6=(_tmp8CF=_region_malloc(env->r,sizeof(*_tmp8CF)),((_tmp8CF[0]=((_tmp8D1.tag=6,((_tmp8D1.f1=((_tmp8D0.is_union=_tmp1FC == Cyc_Absyn_UnionA,((_tmp8D0.fieldnum=field_no,_tmp8D0)))),((_tmp8D1.f2=aggrdict,_tmp8D1)))))),_tmp8CF)));
struct _tuple16 _tmp8D2;struct _tuple16 _tmp3A8=(_tmp8D2.f1=_tmp39B,((_tmp8D2.f2=(void*)_tmp3A6,_tmp8D2)));_npop_handler(0);return _tmp3A8;};};}
# 1545
;_pop_region(temp);}_LL156: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp1FE=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FE->tag != 28)goto _LL158;}_LL157: {
# 1571
struct Cyc_Core_Impossible_exn_struct _tmp8D8;const char*_tmp8D7;struct Cyc_Core_Impossible_exn_struct*_tmp8D6;(int)_throw((void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D8.tag=Cyc_Core_Impossible,((_tmp8D8.f1=((_tmp8D7="anal_Rexp:missing aggrdeclimpl",_tag_dyneither(_tmp8D7,sizeof(char),31))),_tmp8D8)))),_tmp8D6)))));}_LL158: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1FF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp1FF->tag != 25)goto _LL15A;else{_tmp200=_tmp1FF->f1;}}_LL159: {
# 1573
struct _RegionHandle _tmp3AF=_new_region("temp");struct _RegionHandle*temp=& _tmp3AF;_push_region(temp);
{struct Cyc_List_List*_tmp3B0=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple26*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Exp*(*)(struct _tuple26*))Cyc_Core_snd,_tmp200);
struct _tuple23 _tmp3B1=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp3B0,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp3B3;struct Cyc_List_List*_tmp3B4;struct _tuple23 _tmp3B2=_tmp3B1;_tmp3B3=_tmp3B2.f1;_tmp3B4=_tmp3B2.f2;
for(0;_tmp3B4 != 0;(_tmp3B4=_tmp3B4->tl,_tmp3B0=_tmp3B0->tl)){
_tmp3B3=Cyc_NewControlFlow_use_Rval(env,((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp3B0))->hd)->loc,_tmp3B3,(void*)_tmp3B4->hd);}{
struct _tuple16 _tmp8D9;struct _tuple16 _tmp3B6=(_tmp8D9.f1=_tmp3B3,((_tmp8D9.f2=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,(void*)_check_null(e->topt),0,_tmp182->unknown_all),_tmp8D9)));_npop_handler(0);return _tmp3B6;};}
# 1574
;_pop_region(temp);}_LL15A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp201=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp201->tag != 26)goto _LL15C;else{_tmp202=_tmp201->f1;_tmp203=_tmp201->f2;_tmp204=_tmp201->f3;_tmp205=_tmp201->f4;}}_LL15B: {
# 1582
struct _tuple16 _tmp3B7=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp203);union Cyc_CfFlowInfo_FlowInfo _tmp3B9;void*_tmp3BA;struct _tuple16 _tmp3B8=_tmp3B7;_tmp3B9=_tmp3B8.f1;_tmp3BA=_tmp3B8.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3BB=_tmp3B9;struct Cyc_Dict_Dict _tmp3BC;struct Cyc_List_List*_tmp3BD;_LL1F3: if((_tmp3BB.BottomFL).tag != 1)goto _LL1F5;_LL1F4: {
struct _tuple16 _tmp8DA;return(_tmp8DA.f1=_tmp3B9,((_tmp8DA.f2=_tmp182->unknown_all,_tmp8DA)));}_LL1F5: if((_tmp3BB.ReachableFL).tag != 2)goto _LL1F2;_tmp3BC=((struct _tuple14)(_tmp3BB.ReachableFL).val).f1;_tmp3BD=((struct _tuple14)(_tmp3BB.ReachableFL).val).f2;_LL1F6:
# 1586
 if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp3BC,_tmp3BA)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp8DD;void*_tmp8DC;(_tmp8DC=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8DD="expression may not be initialized",_tag_dyneither(_tmp8DD,sizeof(char),34))),_tag_dyneither(_tmp8DC,sizeof(void*),0)));}{
# 1590
struct Cyc_List_List*new_relns=_tmp3BD;
union Cyc_Relations_RelnOp n1=Cyc_Relations_RVar(_tmp202);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp203,& n2))
new_relns=Cyc_Relations_add_relation(env->r,n1,Cyc_Relations_Rlt,n2,_tmp3BD);
# 1596
if(_tmp3BD != new_relns)
_tmp3B9=Cyc_CfFlowInfo_ReachableFL(_tmp3BC,new_relns);{
# 1600
void*_tmp3C1=_tmp3BA;_LL1F8: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp3C2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C2->tag != 0)goto _LL1FA;}_LL1F9: {
struct _tuple16 _tmp8DE;return(_tmp8DE.f1=_tmp3B9,((_tmp8DE.f2=_tmp182->unknown_all,_tmp8DE)));}_LL1FA: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp3C3=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C3->tag != 2)goto _LL1FC;}_LL1FB:
 goto _LL1FD;_LL1FC: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp3C4=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C4->tag != 1)goto _LL1FE;}_LL1FD:
 goto _LL1FF;_LL1FE: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp3C5=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp3C1;if(_tmp3C5->tag != 5)goto _LL200;}_LL1FF: {
# 1605
struct Cyc_List_List _tmp8DF;struct Cyc_List_List _tmp3C7=(_tmp8DF.hd=_tmp202,((_tmp8DF.tl=0,_tmp8DF)));
_tmp3B9=Cyc_NewControlFlow_add_vars(_tmp182,_tmp3B9,& _tmp3C7,_tmp182->unknown_all,e->loc,0);{
# 1608
struct _tuple16 _tmp3C8=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B9,_tmp204);union Cyc_CfFlowInfo_FlowInfo _tmp3CA;void*_tmp3CB;struct _tuple16 _tmp3C9=_tmp3C8;_tmp3CA=_tmp3C9.f1;_tmp3CB=_tmp3C9.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3CC=_tmp3CA;struct Cyc_Dict_Dict _tmp3CD;_LL203: if((_tmp3CC.BottomFL).tag != 1)goto _LL205;_LL204: {
struct _tuple16 _tmp8E0;return(_tmp8E0.f1=_tmp3CA,((_tmp8E0.f2=_tmp182->unknown_all,_tmp8E0)));}_LL205: if((_tmp3CC.ReachableFL).tag != 2)goto _LL202;_tmp3CD=((struct _tuple14)(_tmp3CC.ReachableFL).val).f1;_LL206:
# 1612
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp3CD,_tmp3CB)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8E3;void*_tmp8E2;(_tmp8E2=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8E3="expression may not be initialized",_tag_dyneither(_tmp8E3,sizeof(char),34))),_tag_dyneither(_tmp8E2,sizeof(void*),0)));}{
struct _tuple16 _tmp8E4;return(_tmp8E4.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E4.f2=_tmp182->unknown_all,_tmp8E4)));};}_LL202:;}
# 1617
_tmp3B9=_tmp3CA;
goto _LL201;};}_LL200:;_LL201:
# 1620
 while(1){
struct Cyc_List_List _tmp8E5;struct Cyc_List_List _tmp3D3=(_tmp8E5.hd=_tmp202,((_tmp8E5.tl=0,_tmp8E5)));
_tmp3B9=Cyc_NewControlFlow_add_vars(_tmp182,_tmp3B9,& _tmp3D3,_tmp182->unknown_all,e->loc,0);{
struct _tuple16 _tmp3D4=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp3B9,_tmp204);union Cyc_CfFlowInfo_FlowInfo _tmp3D6;void*_tmp3D7;struct _tuple16 _tmp3D5=_tmp3D4;_tmp3D6=_tmp3D5.f1;_tmp3D7=_tmp3D5.f2;
{union Cyc_CfFlowInfo_FlowInfo _tmp3D8=_tmp3D6;struct Cyc_Dict_Dict _tmp3D9;_LL208: if((_tmp3D8.BottomFL).tag != 1)goto _LL20A;_LL209:
 goto _LL207;_LL20A: if((_tmp3D8.ReachableFL).tag != 2)goto _LL207;_tmp3D9=((struct _tuple14)(_tmp3D8.ReachableFL).val).f1;_LL20B:
# 1627
 if(Cyc_CfFlowInfo_initlevel(_tmp182,_tmp3D9,_tmp3D7)!= Cyc_CfFlowInfo_AllIL){
{const char*_tmp8E8;void*_tmp8E7;(_tmp8E7=0,Cyc_CfFlowInfo_aerr(_tmp203->loc,((_tmp8E8="expression may not be initialized",_tag_dyneither(_tmp8E8,sizeof(char),34))),_tag_dyneither(_tmp8E7,sizeof(void*),0)));}{
struct _tuple16 _tmp8E9;return(_tmp8E9.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp8E9.f2=_tmp182->unknown_all,_tmp8E9)));};}_LL207:;}{
# 1632
union Cyc_CfFlowInfo_FlowInfo _tmp3DD=Cyc_CfFlowInfo_join_flow(_tmp182,env->all_changed,_tmp3B9,_tmp3D6);
if(Cyc_CfFlowInfo_flow_lessthan_approx(_tmp3DD,_tmp3B9))
break;
_tmp3B9=_tmp3DD;};};}{
# 1637
struct _tuple16 _tmp8EA;return(_tmp8EA.f1=_tmp3B9,((_tmp8EA.f2=_tmp182->unknown_all,_tmp8EA)));};_LL1F7:;};};_LL1F2:;};}_LL15C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp206=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp206->tag != 27)goto _LL15E;else{_tmp207=_tmp206->f1;_tmp208=(void*)_tmp206->f2;_tmp209=_tmp206->f3;}}_LL15D: {
# 1643
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp8ED;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp8EC;void*root=(void*)((_tmp8EC=_region_malloc(_tmp182->r,sizeof(*_tmp8EC)),((_tmp8EC[0]=((_tmp8ED.tag=1,((_tmp8ED.f1=_tmp207,((_tmp8ED.f2=(void*)_check_null(e->topt),_tmp8ED)))))),_tmp8EC))));
struct Cyc_CfFlowInfo_Place*_tmp8EE;struct Cyc_CfFlowInfo_Place*place=(_tmp8EE=_region_malloc(_tmp182->r,sizeof(*_tmp8EE)),((_tmp8EE->root=root,((_tmp8EE->fields=0,_tmp8EE)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp8F1;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp8F0;void*rval=(void*)((_tmp8F0=_region_malloc(_tmp182->r,sizeof(*_tmp8F0)),((_tmp8F0[0]=((_tmp8F1.tag=5,((_tmp8F1.f1=place,_tmp8F1)))),_tmp8F0))));
void*place_val;
# 1651
place_val=Cyc_CfFlowInfo_typ_to_absrval(_tmp182,_tmp208,0,_tmp182->unknown_none);{
union Cyc_CfFlowInfo_FlowInfo outflow;
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);{
struct _tuple16 _tmp3E0=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp207);union Cyc_CfFlowInfo_FlowInfo _tmp3E2;struct _tuple16 _tmp3E1=_tmp3E0;_tmp3E2=_tmp3E1.f1;
outflow=_tmp3E2;{
union Cyc_CfFlowInfo_FlowInfo _tmp3E3=outflow;struct Cyc_Dict_Dict _tmp3E4;struct Cyc_List_List*_tmp3E5;_LL20D: if((_tmp3E3.BottomFL).tag != 1)goto _LL20F;_LL20E: {
struct _tuple16 _tmp8F2;return(_tmp8F2.f1=outflow,((_tmp8F2.f2=rval,_tmp8F2)));}_LL20F: if((_tmp3E3.ReachableFL).tag != 2)goto _LL20C;_tmp3E4=((struct _tuple14)(_tmp3E3.ReachableFL).val).f1;_tmp3E5=((struct _tuple14)(_tmp3E3.ReachableFL).val).f2;_LL210: {
# 1659
struct _tuple16 _tmp8F3;return(_tmp8F3.f1=Cyc_CfFlowInfo_ReachableFL(((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp3E4,root,place_val),_tmp3E5),((_tmp8F3.f2=rval,_tmp8F3)));}_LL20C:;};};};}_LL15E: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp20A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20A->tag != 36)goto _LL160;else{_tmp20B=_tmp20A->f1;}}_LL15F: {
# 1663
struct _tuple17 _tmp8F4;struct _tuple17 _tmp3ED=(_tmp8F4.f1=(env->fenv)->unknown_all,((_tmp8F4.f2=copy_ctxt,_tmp8F4)));
union Cyc_CfFlowInfo_FlowInfo _tmp3EE=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp20B,& _tmp3ED);
struct _tuple16 _tmp8F5;return(_tmp8F5.f1=_tmp3EE,((_tmp8F5.f2=_tmp3ED.f1,_tmp8F5)));}_LL160: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp20C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20C->tag != 1)goto _LL162;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp20D=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmp20C->f2);if(_tmp20D->tag != 0)goto _LL162;}}_LL161:
# 1667
 goto _LL163;_LL162: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp20E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20E->tag != 35)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp20F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp20F->tag != 24)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp210=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp210->tag != 38)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp211=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp19A;if(_tmp211->tag != 39)goto _LL101;}_LL169: {
# 1672
struct Cyc_Core_Impossible_exn_struct _tmp8FB;const char*_tmp8FA;struct Cyc_Core_Impossible_exn_struct*_tmp8F9;(int)_throw((void*)((_tmp8F9=_cycalloc(sizeof(*_tmp8F9)),((_tmp8F9[0]=((_tmp8FB.tag=Cyc_Core_Impossible,((_tmp8FB.f1=((_tmp8FA="anal_Rexp, unexpected exp form",_tag_dyneither(_tmp8FA,sizeof(char),31))),_tmp8FB)))),_tmp8F9)))));}_LL101:;};}
# 1682
static struct _tuple18 Cyc_NewControlFlow_anal_derefL(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e,union Cyc_CfFlowInfo_FlowInfo f,void*r,int passthrough_consumes,int expanded_place,struct Cyc_List_List*flds){
# 1691
struct Cyc_CfFlowInfo_FlowEnv*_tmp3F4=env->fenv;
void*_tmp3F5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3F6=_tmp3F5;void*_tmp3F8;union Cyc_Absyn_Constraint*_tmp3F9;union Cyc_Absyn_Constraint*_tmp3FA;_LL212: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6;if(_tmp3F7->tag != 5)goto _LL214;else{_tmp3F8=(_tmp3F7->f1).elt_typ;_tmp3F9=((_tmp3F7->f1).ptr_atts).bounds;_tmp3FA=((_tmp3F7->f1).ptr_atts).zero_term;}}_LL213: {
# 1694
union Cyc_CfFlowInfo_FlowInfo _tmp3FB=f;struct Cyc_Dict_Dict _tmp3FC;struct Cyc_List_List*_tmp3FD;_LL217: if((_tmp3FB.BottomFL).tag != 1)goto _LL219;_LL218: {
struct _tuple18 _tmp8FC;return(_tmp8FC.f1=f,((_tmp8FC.f2=Cyc_CfFlowInfo_UnknownL(),_tmp8FC)));}_LL219: if((_tmp3FB.ReachableFL).tag != 2)goto _LL216;_tmp3FC=((struct _tuple14)(_tmp3FB.ReachableFL).val).f1;_tmp3FD=((struct _tuple14)(_tmp3FB.ReachableFL).val).f2;_LL21A:
# 1698
 if(Cyc_Tcutil_is_bound_one(_tmp3F9)){
struct _tuple15 _tmp3FF=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp401;struct Cyc_List_List*_tmp402;struct _tuple15 _tmp400=_tmp3FF;_tmp401=_tmp400.f1;_tmp402=_tmp400.f2;
retry: {void*_tmp403=_tmp401;void*_tmp408;struct Cyc_List_List*_tmp409;enum Cyc_CfFlowInfo_InitLevel _tmp40C;_LL21C: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp404=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp404->tag != 8)goto _LL21E;}_LL21D: {
# 1702
struct Cyc_Core_Impossible_exn_struct _tmp902;const char*_tmp901;struct Cyc_Core_Impossible_exn_struct*_tmp900;(int)_throw((void*)((_tmp900=_cycalloc(sizeof(*_tmp900)),((_tmp900[0]=((_tmp902.tag=Cyc_Core_Impossible,((_tmp902.f1=((_tmp901="named location in anal_derefL",_tag_dyneither(_tmp901,sizeof(char),30))),_tmp902)))),_tmp900)))));}_LL21E: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp405=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp405->tag != 1)goto _LL220;}_LL21F:
 goto _LL221;_LL220: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp406=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp406->tag != 2)goto _LL222;}_LL221:
# 1705
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp905;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp904;e->annot=(void*)((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904[0]=((_tmp905.tag=Cyc_CfFlowInfo_NotZero,((_tmp905.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp905)))),_tmp904))));}goto _LL21B;_LL222: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp407=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp407->tag != 5)goto _LL224;else{_tmp408=(_tmp407->f1)->root;_tmp409=(_tmp407->f1)->fields;}}_LL223:
# 1708
 if(expanded_place){
# 1711
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp908;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp907;e->annot=(void*)((_tmp907=_cycalloc(sizeof(*_tmp907)),((_tmp907[0]=((_tmp908.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp908.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp908)))),_tmp907))));}else{
# 1714
void*_tmp414=e->annot;void*_tmp415=_tmp414;_LL22B: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp416=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp415;if(_tmp416->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL22D;}_LL22C:
# 1718
{struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp90B;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp90A;e->annot=(void*)((_tmp90A=_cycalloc(sizeof(*_tmp90A)),((_tmp90A[0]=((_tmp90B.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp90B.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp90B)))),_tmp90A))));}
goto _LL22A;_LL22D:;_LL22E:
# 1721
{struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct _tmp90E;struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp90D;e->annot=(void*)((_tmp90D=_cycalloc(sizeof(*_tmp90D)),((_tmp90D[0]=((_tmp90E.tag=Cyc_CfFlowInfo_NotZero,((_tmp90E.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp90E)))),_tmp90D))));}
goto _LL22A;_LL22A:;}{
# 1725
struct Cyc_CfFlowInfo_Place*_tmp911;struct _tuple18 _tmp910;return(_tmp910.f1=f,((_tmp910.f2=Cyc_CfFlowInfo_PlaceL(((_tmp911=_region_malloc(_tmp3F4->r,sizeof(*_tmp911)),((_tmp911->root=_tmp408,((_tmp911->fields=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp3F4->r,_tmp409,flds),_tmp911))))))),_tmp910)));};_LL224: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp40A=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp40A->tag != 0)goto _LL226;}_LL225:
# 1727
 e->annot=(void*)& Cyc_CfFlowInfo_IsZero_val;{
struct _tuple18 _tmp912;return(_tmp912.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp912.f2=Cyc_CfFlowInfo_UnknownL(),_tmp912)));};_LL226: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp40B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp403;if(_tmp40B->tag != 3)goto _LL228;else{_tmp40C=_tmp40B->f1;}}_LL227:
 f=Cyc_NewControlFlow_notzero(env,inflow,f,e,_tmp40C,_tmp402);goto _LL229;_LL228:;_LL229:
# 1732
 if(passthrough_consumes){
void*_tmp41E=_tmp401;void*_tmp420;_LL230: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp41F=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp41E;if(_tmp41F->tag != 7)goto _LL232;else{_tmp420=(void*)_tmp41F->f3;}}_LL231:
 _tmp401=_tmp420;goto retry;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 1738
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp915;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp914;e->annot=(void*)((_tmp914=_cycalloc(sizeof(*_tmp914)),((_tmp914[0]=((_tmp915.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp915.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp915)))),_tmp914))));};_LL21B:;}}else{
# 1742
struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct _tmp918;struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp917;e->annot=(void*)((_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917[0]=((_tmp918.tag=Cyc_CfFlowInfo_UnknownZ,((_tmp918.f1=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp3FD),_tmp918)))),_tmp917))));}
if(Cyc_CfFlowInfo_initlevel(_tmp3F4,_tmp3FC,r)== Cyc_CfFlowInfo_NoneIL){
struct _tuple15 _tmp425=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r);void*_tmp427;struct _tuple15 _tmp426=_tmp425;_tmp427=_tmp426.f1;{
void*_tmp428=_tmp427;_LL235: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp429=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp428;if(_tmp429->tag != 7)goto _LL237;}_LL236:
# 1747
{const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp91B="attempt to dereference an alias-free that has already been copied",_tag_dyneither(_tmp91B,sizeof(char),66))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}
goto _LL234;_LL237:;_LL238:
# 1750
{const char*_tmp91E;void*_tmp91D;(_tmp91D=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp91E="dereference of possibly uninitialized pointer",_tag_dyneither(_tmp91E,sizeof(char),46))),_tag_dyneither(_tmp91D,sizeof(void*),0)));}
goto _LL234;_LL234:;};}{
# 1754
struct _tuple18 _tmp91F;return(_tmp91F.f1=f,((_tmp91F.f2=Cyc_CfFlowInfo_UnknownL(),_tmp91F)));};_LL216:;}_LL214:;_LL215: {
# 1756
struct Cyc_Core_Impossible_exn_struct _tmp925;const char*_tmp924;struct Cyc_Core_Impossible_exn_struct*_tmp923;(int)_throw((void*)((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923[0]=((_tmp925.tag=Cyc_Core_Impossible,((_tmp925.f1=((_tmp924="left deref of non-pointer-type",_tag_dyneither(_tmp924,sizeof(char),31))),_tmp925)))),_tmp923)))));}_LL211:;}
# 1764
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp_rec(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*flds){
# 1767
struct Cyc_Dict_Dict d;
struct Cyc_CfFlowInfo_FlowEnv*_tmp432=env->fenv;
{union Cyc_CfFlowInfo_FlowInfo _tmp433=inflow;struct Cyc_Dict_Dict _tmp434;struct Cyc_List_List*_tmp435;_LL23A: if((_tmp433.BottomFL).tag != 1)goto _LL23C;_LL23B: {
struct _tuple18 _tmp926;return(_tmp926.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp926.f2=Cyc_CfFlowInfo_UnknownL(),_tmp926)));}_LL23C: if((_tmp433.ReachableFL).tag != 2)goto _LL239;_tmp434=((struct _tuple14)(_tmp433.ReachableFL).val).f1;_tmp435=((struct _tuple14)(_tmp433.ReachableFL).val).f2;_LL23D:
# 1772
 d=_tmp434;_LL239:;}{
# 1776
void*_tmp437=e->r;void*_tmp438=_tmp437;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Vardecl*_tmp441;struct Cyc_Absyn_Vardecl*_tmp444;struct Cyc_Absyn_Vardecl*_tmp447;struct Cyc_Absyn_Vardecl*_tmp44A;struct Cyc_Absyn_Exp*_tmp44C;struct _dyneither_ptr*_tmp44D;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp451;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_Absyn_Exp*_tmp454;struct _dyneither_ptr*_tmp455;_LL23F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp439=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp439->tag != 13)goto _LL241;else{_tmp43A=_tmp439->f2;}}_LL240:
 _tmp43C=_tmp43A;goto _LL242;_LL241: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp43B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43B->tag != 11)goto _LL243;else{_tmp43C=_tmp43B->f1;}}_LL242:
 _tmp43E=_tmp43C;goto _LL244;_LL243: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp43D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43D->tag != 12)goto _LL245;else{_tmp43E=_tmp43D->f1;}}_LL244:
 return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp43E,flds);_LL245: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp43F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp43F->tag != 1)goto _LL247;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp440=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp43F->f2);if(_tmp440->tag != 3)goto _LL247;else{_tmp441=_tmp440->f1;}}}_LL246:
# 1781
 _tmp444=_tmp441;goto _LL248;_LL247: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp442=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp442->tag != 1)goto _LL249;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp443=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp442->f2);if(_tmp443->tag != 4)goto _LL249;else{_tmp444=_tmp443->f1;}}}_LL248:
 _tmp447=_tmp444;goto _LL24A;_LL249: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp445=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp445->tag != 1)goto _LL24B;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp446=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp445->f2);if(_tmp446->tag != 5)goto _LL24B;else{_tmp447=_tmp446->f1;}}}_LL24A: {
# 1784
struct Cyc_CfFlowInfo_Place*_tmp930;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp92F;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp92E;struct _tuple18 _tmp92D;return(_tmp92D.f1=inflow,((_tmp92D.f2=Cyc_CfFlowInfo_PlaceL(((_tmp930=_region_malloc(env->r,sizeof(*_tmp930)),((_tmp930->root=(void*)((_tmp92E=_region_malloc(env->r,sizeof(*_tmp92E)),((_tmp92E[0]=((_tmp92F.tag=0,((_tmp92F.f1=_tmp447,_tmp92F)))),_tmp92E)))),((_tmp930->fields=flds,_tmp930))))))),_tmp92D)));}_LL24B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp448=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp448->tag != 1)goto _LL24D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp449=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp448->f2);if(_tmp449->tag != 1)goto _LL24D;else{_tmp44A=_tmp449->f1;}}}_LL24C: {
# 1786
struct _tuple18 _tmp931;return(_tmp931.f1=inflow,((_tmp931.f2=Cyc_CfFlowInfo_UnknownL(),_tmp931)));}_LL24D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp44B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp44B->tag != 21)goto _LL24F;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;}}_LL24E:
# 1789
{void*_tmp45B=Cyc_Tcutil_compress((void*)_check_null(_tmp44C->topt));void*_tmp45C=_tmp45B;void*_tmp45E;_LL258: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45C;if(_tmp45D->tag != 5)goto _LL25A;else{_tmp45E=(_tmp45D->f1).elt_typ;}}_LL259:
# 1791
 if(!Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp45E)){
Cyc_NewControlFlow_check_requires(_tmp44C->loc,_tmp432->r,_tmp45E,_tmp44D,inflow);{
struct Cyc_List_List*_tmp932;flds=((_tmp932=_region_malloc(env->r,sizeof(*_tmp932)),((_tmp932->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp45E,_tmp44D),((_tmp932->tl=flds,_tmp932))))));};}
# 1795
goto _LL257;_LL25A:;_LL25B: {
struct Cyc_Core_Impossible_exn_struct _tmp938;const char*_tmp937;struct Cyc_Core_Impossible_exn_struct*_tmp936;(int)_throw((void*)((_tmp936=_cycalloc(sizeof(*_tmp936)),((_tmp936[0]=((_tmp938.tag=Cyc_Core_Impossible,((_tmp938.f1=((_tmp937="anal_Lexp: AggrArrow ptr",_tag_dyneither(_tmp937,sizeof(char),25))),_tmp938)))),_tmp936)))));}_LL257:;}
# 1798
_tmp44F=_tmp44C;goto _LL250;_LL24F: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp44E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp44E->tag != 19)goto _LL251;else{_tmp44F=_tmp44E->f1;}}_LL250:
# 1801
 if(expand_unique  && Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp44F->topt),1)){
# 1803
struct _tuple18 _tmp463=
Cyc_NewControlFlow_anal_Lexp(env,inflow,
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp44F->topt),1),passthrough_consumes,_tmp44F);
# 1803
union Cyc_CfFlowInfo_FlowInfo _tmp465;union Cyc_CfFlowInfo_AbsLVal _tmp466;struct _tuple18 _tmp464=_tmp463;_tmp465=_tmp464.f1;_tmp466=_tmp464.f2;{
# 1807
struct _tuple18 _tmp939;struct _tuple18 _tmp467=(_tmp939.f1=_tmp465,((_tmp939.f2=_tmp466,_tmp939)));struct _tuple18 _tmp468=_tmp467;struct Cyc_Dict_Dict _tmp469;struct Cyc_List_List*_tmp46A;struct Cyc_CfFlowInfo_Place*_tmp46B;_LL25D: if(((_tmp468.f1).ReachableFL).tag != 2)goto _LL25F;_tmp469=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f1;_tmp46A=((struct _tuple14)((_tmp468.f1).ReachableFL).val).f2;if(((_tmp468.f2).PlaceL).tag != 1)goto _LL25F;_tmp46B=(struct Cyc_CfFlowInfo_Place*)((_tmp468.f2).PlaceL).val;_LL25E: {
# 1809
void*_tmp46C=Cyc_CfFlowInfo_lookup_place(_tmp469,_tmp46B);
struct _tuple15 _tmp46D=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp46C);void*_tmp46F;struct Cyc_List_List*_tmp470;struct _tuple15 _tmp46E=_tmp46D;_tmp46F=_tmp46E.f1;_tmp470=_tmp46E.f2;{
void*_tmp471=_tmp46F;struct Cyc_Absyn_Vardecl*_tmp473;void*_tmp474;_LL262: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp472=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp472->tag != 8)goto _LL264;else{_tmp473=_tmp472->f1;_tmp474=(void*)_tmp472->f2;}}_LL263: {
# 1813
struct Cyc_Core_Impossible_exn_struct _tmp93F;const char*_tmp93E;struct Cyc_Core_Impossible_exn_struct*_tmp93D;(int)_throw((void*)((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D[0]=((_tmp93F.tag=Cyc_Core_Impossible,((_tmp93F.f1=((_tmp93E="bad named location in anal_Lexp:deref",_tag_dyneither(_tmp93E,sizeof(char),38))),_tmp93F)))),_tmp93D)))));}_LL264: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp475=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp475->tag != 7)goto _LL266;else{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp476=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)((void*)_tmp475->f3);if(_tmp476->tag != 5)goto _LL266;}}_LL265:
 goto _LL267;_LL266: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp477=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp471;if(_tmp477->tag != 5)goto _LL268;}_LL267:
# 1818
 return Cyc_NewControlFlow_anal_derefL(env,_tmp465,_tmp44F,_tmp465,_tmp46C,passthrough_consumes,0,flds);_LL268:;_LL269: {
# 1825
enum Cyc_CfFlowInfo_InitLevel il=Cyc_CfFlowInfo_initlevel(_tmp432,_tmp469,_tmp46F);
void*leaf=il == Cyc_CfFlowInfo_AllIL?_tmp432->unknown_all: _tmp432->unknown_none;
void*new_rval=Cyc_CfFlowInfo_typ_to_absrval(_tmp432,Cyc_Tcutil_pointer_elt_type((void*)_check_null(_tmp44F->topt)),0,leaf);
struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct _tmp942;struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp941;void*new_root=(void*)((_tmp941=_region_malloc(_tmp432->r,sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=1,((_tmp942.f1=e,((_tmp942.f2=(void*)_check_null(e->topt),_tmp942)))))),_tmp941))));
struct Cyc_CfFlowInfo_Place*_tmp943;struct Cyc_CfFlowInfo_Place*place=(_tmp943=_region_malloc(_tmp432->r,sizeof(*_tmp943)),((_tmp943->root=new_root,((_tmp943->fields=0,_tmp943)))));
struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmp946;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp945;void*res=(void*)((_tmp945=_region_malloc(_tmp432->r,sizeof(*_tmp945)),((_tmp945[0]=((_tmp946.tag=5,((_tmp946.f1=place,_tmp946)))),_tmp945))));
for(0;_tmp470 != 0;_tmp470=_tmp470->tl){
struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp949;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp948;res=(void*)((_tmp948=_region_malloc(_tmp432->r,sizeof(*_tmp948)),((_tmp948[0]=((_tmp949.tag=8,((_tmp949.f1=(struct Cyc_Absyn_Vardecl*)_tmp470->hd,((_tmp949.f2=res,_tmp949)))))),_tmp948))));}
((int(*)(struct Cyc_Dict_Dict*set,struct Cyc_CfFlowInfo_Place*place,unsigned int loc))Cyc_CfFlowInfo_update_place_set)(env->all_changed,place,0);
_tmp469=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp469,new_root,new_rval);
_tmp469=Cyc_CfFlowInfo_assign_place(_tmp432,e->loc,_tmp469,env->all_changed,_tmp46B,res);{
union Cyc_CfFlowInfo_FlowInfo _tmp47D=Cyc_CfFlowInfo_ReachableFL(_tmp469,_tmp46A);
# 1843
return Cyc_NewControlFlow_anal_derefL(env,_tmp47D,_tmp44F,_tmp47D,res,passthrough_consumes,1,flds);};}_LL261:;};}_LL25F:;_LL260:
# 1846
 goto _LL25C;_LL25C:;};}{
# 1849
struct _tuple16 _tmp484=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp44F);union Cyc_CfFlowInfo_FlowInfo _tmp486;void*_tmp487;struct _tuple16 _tmp485=_tmp484;_tmp486=_tmp485.f1;_tmp487=_tmp485.f2;
return Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp44F,_tmp486,_tmp487,passthrough_consumes,0,flds);};_LL251: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp450=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp450->tag != 22)goto _LL253;else{_tmp451=_tmp450->f1;_tmp452=_tmp450->f2;}}_LL252: {
# 1853
void*_tmp488=Cyc_Tcutil_compress((void*)_check_null(_tmp451->topt));void*_tmp489=_tmp488;union Cyc_Absyn_Constraint*_tmp48C;_LL26B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp489;if(_tmp48A->tag != 10)goto _LL26D;}_LL26C: {
# 1855
unsigned int _tmp48D=(Cyc_Evexp_eval_const_uint_exp(_tmp452)).f1;
struct Cyc_List_List*_tmp94A;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp451,((_tmp94A=_region_malloc(env->r,sizeof(*_tmp94A)),((_tmp94A->hd=(void*)((int)_tmp48D),((_tmp94A->tl=flds,_tmp94A)))))));}_LL26D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489;if(_tmp48B->tag != 5)goto _LL26F;else{_tmp48C=((_tmp48B->f1).ptr_atts).bounds;}}_LL26E: {
# 1858
struct _RegionHandle _tmp48F=_new_region("temp");struct _RegionHandle*temp=& _tmp48F;_push_region(temp);
{struct Cyc_Absyn_Exp*_tmp94B[2];struct _tuple23 _tmp490=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,(
(_tmp94B[1]=_tmp452,((_tmp94B[0]=_tmp451,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(temp,_tag_dyneither(_tmp94B,sizeof(struct Cyc_Absyn_Exp*),2)))))),0,1);
# 1859
union Cyc_CfFlowInfo_FlowInfo _tmp492;struct Cyc_List_List*_tmp493;struct _tuple23 _tmp491=_tmp490;_tmp492=_tmp491.f1;_tmp493=_tmp491.f2;{
# 1861
union Cyc_CfFlowInfo_FlowInfo _tmp494=_tmp492;
{union Cyc_CfFlowInfo_FlowInfo _tmp495=_tmp492;struct Cyc_Dict_Dict _tmp496;struct Cyc_List_List*_tmp497;_LL272: if((_tmp495.ReachableFL).tag != 2)goto _LL274;_tmp496=((struct _tuple14)(_tmp495.ReachableFL).val).f1;_tmp497=((struct _tuple14)(_tmp495.ReachableFL).val).f2;_LL273:
# 1864
 if(Cyc_CfFlowInfo_initlevel(_tmp432,_tmp496,(void*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp493))->tl))->hd)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp94E;void*_tmp94D;(_tmp94D=0,Cyc_CfFlowInfo_aerr(_tmp452->loc,((_tmp94E="expression may not be initialized",_tag_dyneither(_tmp94E,sizeof(char),34))),_tag_dyneither(_tmp94D,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp49A=Cyc_NewControlFlow_add_subscript_reln(_tmp432->r,_tmp497,_tmp451,_tmp452);
if(_tmp497 != _tmp49A)
_tmp494=Cyc_CfFlowInfo_ReachableFL(_tmp496,_tmp49A);
goto _LL271;};_LL274:;_LL275:
 goto _LL271;_LL271:;}{
# 1872
struct _tuple18 _tmp49B=Cyc_NewControlFlow_anal_derefL(env,inflow,_tmp451,_tmp492,(void*)((struct Cyc_List_List*)_check_null(_tmp493))->hd,passthrough_consumes,0,flds);union Cyc_CfFlowInfo_FlowInfo _tmp49D;union Cyc_CfFlowInfo_AbsLVal _tmp49E;struct _tuple18 _tmp49C=_tmp49B;_tmp49D=_tmp49C.f1;_tmp49E=_tmp49C.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp49F=_tmp49D;_LL277: if((_tmp49F.BottomFL).tag != 1)goto _LL279;_LL278: {
struct _tuple18 _tmp94F;struct _tuple18 _tmp4A1=(_tmp94F.f1=_tmp49D,((_tmp94F.f2=_tmp49E,_tmp94F)));_npop_handler(0);return _tmp4A1;}_LL279:;_LL27A: {
struct _tuple18 _tmp950;struct _tuple18 _tmp4A3=(_tmp950.f1=_tmp494,((_tmp950.f2=_tmp49E,_tmp950)));_npop_handler(0);return _tmp4A3;}_LL276:;};};};}
# 1859
;_pop_region(temp);}_LL26F:;_LL270: {
# 1878
struct Cyc_Core_Impossible_exn_struct _tmp956;const char*_tmp955;struct Cyc_Core_Impossible_exn_struct*_tmp954;(int)_throw((void*)((_tmp954=_cycalloc(sizeof(*_tmp954)),((_tmp954[0]=((_tmp956.tag=Cyc_Core_Impossible,((_tmp956.f1=((_tmp955="anal_Lexp: Subscript -- bad type",_tag_dyneither(_tmp955,sizeof(char),33))),_tmp956)))),_tmp954)))));}_LL26A:;}_LL253: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp453=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp438;if(_tmp453->tag != 20)goto _LL255;else{_tmp454=_tmp453->f1;_tmp455=_tmp453->f2;}}_LL254: {
# 1882
void*_tmp4A8=(void*)_check_null(_tmp454->topt);
if(Cyc_Absyn_is_require_union_type(_tmp4A8))
Cyc_NewControlFlow_check_requires(_tmp454->loc,_tmp432->r,_tmp4A8,_tmp455,inflow);
# 1886
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(_tmp4A8)){
struct _tuple18 _tmp957;return(_tmp957.f1=inflow,((_tmp957.f2=Cyc_CfFlowInfo_UnknownL(),_tmp957)));}{
# 1889
struct Cyc_List_List*_tmp958;return Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,_tmp454,(
(_tmp958=_region_malloc(env->r,sizeof(*_tmp958)),((_tmp958->hd=(void*)Cyc_CfFlowInfo_get_field_index(_tmp4A8,_tmp455),((_tmp958->tl=flds,_tmp958)))))));};}_LL255:;_LL256: {
# 1892
struct _tuple18 _tmp959;return(_tmp959.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp959.f2=Cyc_CfFlowInfo_UnknownL(),_tmp959)));}_LL23E:;};}
# 1896
static struct _tuple18 Cyc_NewControlFlow_anal_Lexp(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,int expand_unique,int passthrough_consumes,struct Cyc_Absyn_Exp*e){
# 1899
struct _tuple18 _tmp4AC=Cyc_NewControlFlow_anal_Lexp_rec(env,inflow,expand_unique,passthrough_consumes,e,0);union Cyc_CfFlowInfo_FlowInfo _tmp4AE;union Cyc_CfFlowInfo_AbsLVal _tmp4AF;struct _tuple18 _tmp4AD=_tmp4AC;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;{
struct _tuple18 _tmp95A;return(_tmp95A.f1=_tmp4AE,((_tmp95A.f2=_tmp4AF,_tmp95A)));};}
# 1906
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_expand_unique_places(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*es){
# 1909
{struct Cyc_List_List*x=es;for(0;x != 0;x=x->tl){
if(Cyc_NewControlFlow_is_local_var_rooted_path((struct Cyc_Absyn_Exp*)x->hd,1) && 
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,(void*)_check_null(((struct Cyc_Absyn_Exp*)x->hd)->topt))){
struct _tuple18 _tmp4B1=Cyc_NewControlFlow_anal_Lexp(env,inflow,1,0,(struct Cyc_Absyn_Exp*)x->hd);union Cyc_CfFlowInfo_FlowInfo _tmp4B3;struct _tuple18 _tmp4B2=_tmp4B1;_tmp4B3=_tmp4B2.f1;
inflow=_tmp4B3;}}}
# 1917
return inflow;}
# 1923
static struct _tuple19 Cyc_NewControlFlow_anal_primop_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 1926
struct Cyc_CfFlowInfo_FlowEnv*_tmp4B4=env->fenv;
# 1929
void*r1;void*r2;
union Cyc_CfFlowInfo_FlowInfo f;
struct _RegionHandle _tmp4B5=_new_region("temp");struct _RegionHandle*temp=& _tmp4B5;_push_region(temp);{
struct _tuple23 _tmp4B6=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,es,0,0);union Cyc_CfFlowInfo_FlowInfo _tmp4B8;struct Cyc_List_List*_tmp4B9;struct _tuple23 _tmp4B7=_tmp4B6;_tmp4B8=_tmp4B7.f1;_tmp4B9=_tmp4B7.f2;
r1=(void*)((struct Cyc_List_List*)_check_null(_tmp4B9))->hd;
r2=(void*)((struct Cyc_List_List*)_check_null(_tmp4B9->tl))->hd;
f=_tmp4B8;}{
# 1939
union Cyc_CfFlowInfo_FlowInfo _tmp4BA=f;struct Cyc_Dict_Dict _tmp4BB;struct Cyc_List_List*_tmp4BC;_LL27C: if((_tmp4BA.BottomFL).tag != 1)goto _LL27E;_LL27D: {
struct _tuple19 _tmp95B;struct _tuple19 _tmp4BE=(_tmp95B.f1=f,((_tmp95B.f2=f,_tmp95B)));_npop_handler(0);return _tmp4BE;}_LL27E: if((_tmp4BA.ReachableFL).tag != 2)goto _LL27B;_tmp4BB=((struct _tuple14)(_tmp4BA.ReachableFL).val).f1;_tmp4BC=((struct _tuple14)(_tmp4BA.ReachableFL).val).f2;_LL27F: {
# 1942
struct Cyc_Absyn_Exp*_tmp4BF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
struct Cyc_Absyn_Exp*_tmp4C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4BB,r1)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp95E;void*_tmp95D;(_tmp95D=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)es->hd)->loc,((_tmp95E="expression may not be initialized",_tag_dyneither(_tmp95E,sizeof(char),34))),_tag_dyneither(_tmp95D,sizeof(void*),0)));}
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp4BB,r2)== Cyc_CfFlowInfo_NoneIL){
const char*_tmp961;void*_tmp960;(_tmp960=0,Cyc_CfFlowInfo_aerr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd)->loc,((_tmp961="expression may not be initialized",_tag_dyneither(_tmp961,sizeof(char),34))),_tag_dyneither(_tmp960,sizeof(void*),0)));}{
# 1949
union Cyc_CfFlowInfo_FlowInfo _tmp4C5=f;
union Cyc_CfFlowInfo_FlowInfo _tmp4C6=f;
# 1954
if(p == Cyc_Absyn_Eq  || p == Cyc_Absyn_Neq){
struct _tuple15 _tmp4C7=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r1);void*_tmp4C9;struct Cyc_List_List*_tmp4CA;struct _tuple15 _tmp4C8=_tmp4C7;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;{
struct _tuple15 _tmp4CB=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,r2);void*_tmp4CD;struct Cyc_List_List*_tmp4CE;struct _tuple15 _tmp4CC=_tmp4CB;_tmp4CD=_tmp4CC.f1;_tmp4CE=_tmp4CC.f2;{
struct _tuple0 _tmp962;struct _tuple0 _tmp4CF=(_tmp962.f1=_tmp4C9,((_tmp962.f2=_tmp4CD,_tmp962)));struct _tuple0 _tmp4D0=_tmp4CF;enum Cyc_CfFlowInfo_InitLevel _tmp4D2;enum Cyc_CfFlowInfo_InitLevel _tmp4D6;_LL281:{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D1=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D1->tag != 3)goto _LL283;else{_tmp4D2=_tmp4D1->f1;}}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D3=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D3->tag != 0)goto _LL283;};_LL282: {
# 1961
struct _tuple19 _tmp4E5=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4BF,_tmp4D2,_tmp4CA);union Cyc_CfFlowInfo_FlowInfo _tmp4E7;union Cyc_CfFlowInfo_FlowInfo _tmp4E8;struct _tuple19 _tmp4E6=_tmp4E5;_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;
switch(p){case Cyc_Absyn_Eq: _LL295:
 _tmp4C5=_tmp4E8;_tmp4C6=_tmp4E7;break;case Cyc_Absyn_Neq: _LL296:
 _tmp4C5=_tmp4E7;_tmp4C6=_tmp4E8;break;default: _LL297: {
struct Cyc_Core_Impossible_exn_struct _tmp968;const char*_tmp967;struct Cyc_Core_Impossible_exn_struct*_tmp966;(int)_throw((void*)((_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp968.tag=Cyc_Core_Impossible,((_tmp968.f1=((_tmp967="anal_test, zero-split",_tag_dyneither(_tmp967,sizeof(char),22))),_tmp968)))),_tmp966)))));}}
# 1967
goto _LL280;}_LL283:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D4->tag != 0)goto _LL285;}{struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp4D5=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D5->tag != 3)goto _LL285;else{_tmp4D6=_tmp4D5->f1;}};_LL284: {
# 1969
struct _tuple19 _tmp4EC=Cyc_NewControlFlow_splitzero(env,f,f,_tmp4C0,_tmp4D6,_tmp4CE);union Cyc_CfFlowInfo_FlowInfo _tmp4EE;union Cyc_CfFlowInfo_FlowInfo _tmp4EF;struct _tuple19 _tmp4ED=_tmp4EC;_tmp4EE=_tmp4ED.f1;_tmp4EF=_tmp4ED.f2;
switch(p){case Cyc_Absyn_Eq: _LL299:
 _tmp4C5=_tmp4EF;_tmp4C6=_tmp4EE;break;case Cyc_Absyn_Neq: _LL29A:
 _tmp4C5=_tmp4EE;_tmp4C6=_tmp4EF;break;default: _LL29B: {
struct Cyc_Core_Impossible_exn_struct _tmp96E;const char*_tmp96D;struct Cyc_Core_Impossible_exn_struct*_tmp96C;(int)_throw((void*)((_tmp96C=_cycalloc(sizeof(*_tmp96C)),((_tmp96C[0]=((_tmp96E.tag=Cyc_Core_Impossible,((_tmp96E.f1=((_tmp96D="anal_test, zero-split",_tag_dyneither(_tmp96D,sizeof(char),22))),_tmp96E)))),_tmp96C)))));}}
# 1975
goto _LL280;}_LL285:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D7=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D7->tag != 0)goto _LL287;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D8=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4D8->tag != 0)goto _LL287;};_LL286:
# 1977
 if(p == Cyc_Absyn_Eq)_tmp4C6=Cyc_CfFlowInfo_BottomFL();else{
_tmp4C5=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL287:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4D9=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4D9->tag != 0)goto _LL289;}{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4DA=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DA->tag != 1)goto _LL289;};_LL288:
 goto _LL28A;_LL289:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DB=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DB->tag != 0)goto _LL28B;}{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4DC=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DC->tag != 2)goto _LL28B;};_LL28A:
 goto _LL28C;_LL28B:{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4DD=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DD->tag != 0)goto _LL28D;}{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4DE=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4DE->tag != 5)goto _LL28D;};_LL28C:
 goto _LL28E;_LL28D:{struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp4DF=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4DF->tag != 1)goto _LL28F;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E0=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E0->tag != 0)goto _LL28F;};_LL28E:
 goto _LL290;_LL28F:{struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp4E1=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4E1->tag != 2)goto _LL291;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E2=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E2->tag != 0)goto _LL291;};_LL290:
 goto _LL292;_LL291:{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp4E3=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f1;if(_tmp4E3->tag != 5)goto _LL293;}{struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp4E4=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp4D0.f2;if(_tmp4E4->tag != 0)goto _LL293;};_LL292:
# 1986
 if(p == Cyc_Absyn_Neq)_tmp4C6=Cyc_CfFlowInfo_BottomFL();else{
_tmp4C5=Cyc_CfFlowInfo_BottomFL();}
goto _LL280;_LL293:;_LL294:
 goto _LL280;_LL280:;};};}
# 1997
{struct _tuple0 _tmp96F;struct _tuple0 _tmp4F4=(_tmp96F.f1=Cyc_Tcutil_compress((void*)_check_null(_tmp4BF->topt)),((_tmp96F.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp4C0->topt)),_tmp96F)));struct _tuple0 _tmp4F5=_tmp4F4;_LL29E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F5.f1;if(_tmp4F6->tag != 6)goto _LL2A0;else{if(_tmp4F6->f1 != Cyc_Absyn_Unsigned)goto _LL2A0;}}_LL29F:
 goto _LL2A1;_LL2A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F5.f2;if(_tmp4F7->tag != 6)goto _LL2A2;else{if(_tmp4F7->f1 != Cyc_Absyn_Unsigned)goto _LL2A2;}}_LL2A1:
 goto _LL2A3;_LL2A2: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F5.f1;if(_tmp4F8->tag != 19)goto _LL2A4;}_LL2A3:
 goto _LL2A5;_LL2A4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F5.f2;if(_tmp4F9->tag != 19)goto _LL2A6;}_LL2A5:
 goto _LL29D;_LL2A6:;_LL2A7: {
struct _tuple19 _tmp970;struct _tuple19 _tmp4FB=(_tmp970.f1=_tmp4C5,((_tmp970.f2=_tmp4C6,_tmp970)));_npop_handler(0);return _tmp4FB;}_LL29D:;}
# 2006
switch(p){case Cyc_Absyn_Eq: _LL2A8:
 goto _LL2A9;case Cyc_Absyn_Neq: _LL2A9: goto _LL2AA;case Cyc_Absyn_Gt: _LL2AA: goto _LL2AB;case Cyc_Absyn_Gte: _LL2AB: goto _LL2AC;case Cyc_Absyn_Lt: _LL2AC: goto _LL2AD;case Cyc_Absyn_Lte: _LL2AD: break;default: _LL2AE: {
struct _tuple19 _tmp971;struct _tuple19 _tmp4FE=(_tmp971.f1=_tmp4C5,((_tmp971.f2=_tmp4C6,_tmp971)));_npop_handler(0);return _tmp4FE;}}{
# 2011
struct _RegionHandle*_tmp4FF=(env->fenv)->r;
struct _tuple13 _tmp500=Cyc_Relations_primop2relation(_tmp4BF,p,_tmp4C0);struct Cyc_Absyn_Exp*_tmp502;enum Cyc_Relations_Relation _tmp503;struct Cyc_Absyn_Exp*_tmp504;struct _tuple13 _tmp501=_tmp500;_tmp502=_tmp501.f1;_tmp503=_tmp501.f2;_tmp504=_tmp501.f3;{
union Cyc_Relations_RelnOp n1=Cyc_Relations_RConst(0);
union Cyc_Relations_RelnOp n2=Cyc_Relations_RConst(0);
# 2016
if(Cyc_Relations_exp2relnop(_tmp502,& n1) && Cyc_Relations_exp2relnop(_tmp504,& n2)){
# 2018
struct Cyc_List_List*_tmp505=Cyc_Relations_add_relation(_tmp4FF,n1,_tmp503,n2,_tmp4BC);
# 2022
struct Cyc_List_List*_tmp506=Cyc_Relations_add_relation(_tmp4FF,n2,Cyc_Relations_flip_relation(_tmp503),n1,_tmp4BC);
struct _tuple19 _tmp972;struct _tuple19 _tmp507=(_tmp972.f1=_tmp4C5,((_tmp972.f2=_tmp4C6,_tmp972)));struct _tuple19 _tmp508=_tmp507;struct Cyc_Dict_Dict _tmp509;struct Cyc_Dict_Dict _tmp50A;struct Cyc_Dict_Dict _tmp50B;struct Cyc_Dict_Dict _tmp50C;_LL2B1: if(((_tmp508.f1).ReachableFL).tag != 2)goto _LL2B3;_tmp509=((struct _tuple14)((_tmp508.f1).ReachableFL).val).f1;if(((_tmp508.f2).ReachableFL).tag != 2)goto _LL2B3;_tmp50A=((struct _tuple14)((_tmp508.f2).ReachableFL).val).f1;_LL2B2: {
# 2025
struct _tuple19 _tmp973;struct _tuple19 _tmp50E=(_tmp973.f1=Cyc_CfFlowInfo_ReachableFL(_tmp509,_tmp505),((_tmp973.f2=Cyc_CfFlowInfo_ReachableFL(_tmp50A,_tmp506),_tmp973)));_npop_handler(0);return _tmp50E;}_LL2B3: if(((_tmp508.f2).ReachableFL).tag != 2)goto _LL2B5;_tmp50B=((struct _tuple14)((_tmp508.f2).ReachableFL).val).f1;_LL2B4: {
# 2027
struct _tuple19 _tmp974;struct _tuple19 _tmp510=(_tmp974.f1=_tmp4C5,((_tmp974.f2=Cyc_CfFlowInfo_ReachableFL(_tmp50B,_tmp506),_tmp974)));_npop_handler(0);return _tmp510;}_LL2B5: if(((_tmp508.f1).ReachableFL).tag != 2)goto _LL2B7;_tmp50C=((struct _tuple14)((_tmp508.f1).ReachableFL).val).f1;_LL2B6: {
# 2029
struct _tuple19 _tmp975;struct _tuple19 _tmp512=(_tmp975.f1=Cyc_CfFlowInfo_ReachableFL(_tmp50C,_tmp505),((_tmp975.f2=_tmp4C6,_tmp975)));_npop_handler(0);return _tmp512;}_LL2B7:;_LL2B8: {
# 2031
struct _tuple19 _tmp976;struct _tuple19 _tmp514=(_tmp976.f1=_tmp4C5,((_tmp976.f2=_tmp4C6,_tmp976)));_npop_handler(0);return _tmp514;}_LL2B0:;}else{
# 2034
struct _tuple19 _tmp977;struct _tuple19 _tmp517=(_tmp977.f1=_tmp4C5,((_tmp977.f2=_tmp4C6,_tmp977)));_npop_handler(0);return _tmp517;}};};};}_LL27B:;};
# 1931
;_pop_region(temp);}
# 2040
static struct _tuple19 Cyc_NewControlFlow_anal_test(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Exp*e){
# 2042
struct Cyc_CfFlowInfo_FlowEnv*_tmp518=env->fenv;
void*_tmp519=e->r;void*_tmp51A=_tmp519;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp520;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;enum Cyc_Absyn_Primop _tmp52B;struct Cyc_List_List*_tmp52C;_LL2BA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp51B->tag != 5)goto _LL2BC;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;_tmp51E=_tmp51B->f3;}}_LL2BB: {
# 2045
struct _tuple19 _tmp52D=Cyc_NewControlFlow_anal_test(env,inflow,_tmp51C);union Cyc_CfFlowInfo_FlowInfo _tmp52F;union Cyc_CfFlowInfo_FlowInfo _tmp530;struct _tuple19 _tmp52E=_tmp52D;_tmp52F=_tmp52E.f1;_tmp530=_tmp52E.f2;{
struct _tuple19 _tmp531=Cyc_NewControlFlow_anal_test(env,_tmp52F,_tmp51D);union Cyc_CfFlowInfo_FlowInfo _tmp533;union Cyc_CfFlowInfo_FlowInfo _tmp534;struct _tuple19 _tmp532=_tmp531;_tmp533=_tmp532.f1;_tmp534=_tmp532.f2;{
struct _tuple19 _tmp535=Cyc_NewControlFlow_anal_test(env,_tmp530,_tmp51E);union Cyc_CfFlowInfo_FlowInfo _tmp537;union Cyc_CfFlowInfo_FlowInfo _tmp538;struct _tuple19 _tmp536=_tmp535;_tmp537=_tmp536.f1;_tmp538=_tmp536.f2;{
struct _tuple19 _tmp978;return(_tmp978.f1=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp533,_tmp537),((_tmp978.f2=
Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp534,_tmp538),_tmp978)));};};};}_LL2BC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp51F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp51F->tag != 6)goto _LL2BE;else{_tmp520=_tmp51F->f1;_tmp521=_tmp51F->f2;}}_LL2BD: {
# 2051
struct _tuple19 _tmp53A=Cyc_NewControlFlow_anal_test(env,inflow,_tmp520);union Cyc_CfFlowInfo_FlowInfo _tmp53C;union Cyc_CfFlowInfo_FlowInfo _tmp53D;struct _tuple19 _tmp53B=_tmp53A;_tmp53C=_tmp53B.f1;_tmp53D=_tmp53B.f2;{
struct _tuple19 _tmp53E=Cyc_NewControlFlow_anal_test(env,_tmp53C,_tmp521);union Cyc_CfFlowInfo_FlowInfo _tmp540;union Cyc_CfFlowInfo_FlowInfo _tmp541;struct _tuple19 _tmp53F=_tmp53E;_tmp540=_tmp53F.f1;_tmp541=_tmp53F.f2;{
struct _tuple19 _tmp979;return(_tmp979.f1=_tmp540,((_tmp979.f2=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp53D,_tmp541),_tmp979)));};};}_LL2BE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp522->tag != 7)goto _LL2C0;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL2BF: {
# 2055
struct _tuple19 _tmp543=Cyc_NewControlFlow_anal_test(env,inflow,_tmp523);union Cyc_CfFlowInfo_FlowInfo _tmp545;union Cyc_CfFlowInfo_FlowInfo _tmp546;struct _tuple19 _tmp544=_tmp543;_tmp545=_tmp544.f1;_tmp546=_tmp544.f2;{
struct _tuple19 _tmp547=Cyc_NewControlFlow_anal_test(env,_tmp546,_tmp524);union Cyc_CfFlowInfo_FlowInfo _tmp549;union Cyc_CfFlowInfo_FlowInfo _tmp54A;struct _tuple19 _tmp548=_tmp547;_tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;{
struct _tuple19 _tmp97A;return(_tmp97A.f1=Cyc_CfFlowInfo_join_flow(_tmp518,env->all_changed,_tmp545,_tmp549),((_tmp97A.f2=_tmp54A,_tmp97A)));};};}_LL2C0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp525->tag != 8)goto _LL2C2;else{_tmp526=_tmp525->f1;_tmp527=_tmp525->f2;}}_LL2C1: {
# 2059
struct _tuple16 _tmp54C=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp526);union Cyc_CfFlowInfo_FlowInfo _tmp54E;void*_tmp54F;struct _tuple16 _tmp54D=_tmp54C;_tmp54E=_tmp54D.f1;_tmp54F=_tmp54D.f2;
return Cyc_NewControlFlow_anal_test(env,_tmp54E,_tmp527);}_LL2C2: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp528->tag != 2)goto _LL2C4;else{if(_tmp528->f1 != Cyc_Absyn_Not)goto _LL2C4;if(_tmp528->f2 == 0)goto _LL2C4;_tmp529=(struct Cyc_Absyn_Exp*)(_tmp528->f2)->hd;if((_tmp528->f2)->tl != 0)goto _LL2C4;}}_LL2C3: {
# 2062
struct _tuple19 _tmp550=Cyc_NewControlFlow_anal_test(env,inflow,_tmp529);union Cyc_CfFlowInfo_FlowInfo _tmp552;union Cyc_CfFlowInfo_FlowInfo _tmp553;struct _tuple19 _tmp551=_tmp550;_tmp552=_tmp551.f1;_tmp553=_tmp551.f2;{
struct _tuple19 _tmp97B;return(_tmp97B.f1=_tmp553,((_tmp97B.f2=_tmp552,_tmp97B)));};}_LL2C4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp51A;if(_tmp52A->tag != 2)goto _LL2C6;else{_tmp52B=_tmp52A->f1;_tmp52C=_tmp52A->f2;}}_LL2C5:
# 2065
 return Cyc_NewControlFlow_anal_primop_test(env,inflow,_tmp52B,_tmp52C);_LL2C6:;_LL2C7: {
# 2069
struct _tuple16 _tmp555=Cyc_NewControlFlow_anal_Rexp(env,0,inflow,e);union Cyc_CfFlowInfo_FlowInfo _tmp557;void*_tmp558;struct _tuple16 _tmp556=_tmp555;_tmp557=_tmp556.f1;_tmp558=_tmp556.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp559=_tmp557;struct Cyc_Dict_Dict _tmp55A;_LL2C9: if((_tmp559.BottomFL).tag != 1)goto _LL2CB;_LL2CA: {
struct _tuple19 _tmp97C;return(_tmp97C.f1=_tmp557,((_tmp97C.f2=_tmp557,_tmp97C)));}_LL2CB: if((_tmp559.ReachableFL).tag != 2)goto _LL2C8;_tmp55A=((struct _tuple14)(_tmp559.ReachableFL).val).f1;_LL2CC: {
# 2073
struct _tuple15 _tmp55C=Cyc_CfFlowInfo_unname_rval((env->fenv)->r,_tmp558);void*_tmp55E;struct Cyc_List_List*_tmp55F;struct _tuple15 _tmp55D=_tmp55C;_tmp55E=_tmp55D.f1;_tmp55F=_tmp55D.f2;{
void*_tmp560=_tmp55E;struct Cyc_Absyn_Vardecl*_tmp562;void*_tmp563;enum Cyc_CfFlowInfo_InitLevel _tmp56C;_LL2CE: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp561=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp561->tag != 8)goto _LL2D0;else{_tmp562=_tmp561->f1;_tmp563=(void*)_tmp561->f2;}}_LL2CF: {
# 2076
struct Cyc_Core_Impossible_exn_struct _tmp982;const char*_tmp981;struct Cyc_Core_Impossible_exn_struct*_tmp980;(int)_throw((void*)((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=((_tmp982.tag=Cyc_Core_Impossible,((_tmp982.f1=((_tmp981="anal_test: bad namedlocation",_tag_dyneither(_tmp981,sizeof(char),29))),_tmp982)))),_tmp980)))));}_LL2D0: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp564=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp564->tag != 0)goto _LL2D2;}_LL2D1: {
struct _tuple19 _tmp983;return(_tmp983.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp983.f2=_tmp557,_tmp983)));}_LL2D2: {struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*_tmp565=(struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp565->tag != 2)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp566=(struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp566->tag != 1)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmp567=(struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp567->tag != 5)goto _LL2D8;}_LL2D7: {
struct _tuple19 _tmp984;return(_tmp984.f1=_tmp557,((_tmp984.f2=Cyc_CfFlowInfo_BottomFL(),_tmp984)));}_LL2D8: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp568=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp568->tag != 3)goto _LL2DA;else{if(_tmp568->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DA;}}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp569=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp569->tag != 4)goto _LL2DC;else{if(_tmp569->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2DC;}}_LL2DB:
 goto _LL2DD;_LL2DC: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp56A=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56A->tag != 7)goto _LL2DE;}_LL2DD:
# 2084
{const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_CfFlowInfo_aerr(e->loc,((_tmp987="expression may not be initialized",_tag_dyneither(_tmp987,sizeof(char),34))),_tag_dyneither(_tmp986,sizeof(void*),0)));}{
struct _tuple19 _tmp988;return(_tmp988.f1=Cyc_CfFlowInfo_BottomFL(),((_tmp988.f2=Cyc_CfFlowInfo_BottomFL(),_tmp988)));};_LL2DE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp56B=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56B->tag != 3)goto _LL2E0;else{_tmp56C=_tmp56B->f1;}}_LL2DF:
 return Cyc_NewControlFlow_splitzero(env,inflow,_tmp557,e,_tmp56C,_tmp55F);_LL2E0: {struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp56D=(struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56D->tag != 4)goto _LL2E2;}_LL2E1: {
struct _tuple19 _tmp989;return(_tmp989.f1=_tmp557,((_tmp989.f2=_tmp557,_tmp989)));}_LL2E2: {struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp56E=(struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp560;if(_tmp56E->tag != 6)goto _LL2CD;}_LL2E3: {
struct Cyc_Core_Impossible_exn_struct _tmp98F;const char*_tmp98E;struct Cyc_Core_Impossible_exn_struct*_tmp98D;(int)_throw((void*)((_tmp98D=_cycalloc(sizeof(*_tmp98D)),((_tmp98D[0]=((_tmp98F.tag=Cyc_Core_Impossible,((_tmp98F.f1=((_tmp98E="anal_test",_tag_dyneither(_tmp98E,sizeof(char),10))),_tmp98F)))),_tmp98D)))));}_LL2CD:;};}_LL2C8:;};}_LL2B9:;}struct _tuple27{unsigned int f1;struct Cyc_NewControlFlow_AnalEnv*f2;struct Cyc_Dict_Dict f3;};
# 2094
static void Cyc_NewControlFlow_check_for_unused_unique(struct _tuple27*ckenv,void*root,void*rval){
# 2096
unsigned int _tmp57C;struct Cyc_NewControlFlow_AnalEnv*_tmp57D;struct Cyc_Dict_Dict _tmp57E;struct _tuple27*_tmp57B=ckenv;_tmp57C=_tmp57B->f1;_tmp57D=_tmp57B->f2;_tmp57E=_tmp57B->f3;{
void*_tmp57F=root;struct Cyc_Absyn_Vardecl*_tmp581;_LL2E5: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp580=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp57F;if(_tmp580->tag != 0)goto _LL2E7;else{_tmp581=_tmp580->f1;}}_LL2E6:
# 2099
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp57D->fenv)->r,_tmp581->type)){
struct _tuple15 _tmp582=Cyc_CfFlowInfo_unname_rval((_tmp57D->fenv)->r,rval);void*_tmp584;struct _tuple15 _tmp583=_tmp582;_tmp584=_tmp583.f1;{
void*_tmp585=_tmp584;_LL2EA: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp586=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp586->tag != 7)goto _LL2EC;}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp587=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp587->tag != 0)goto _LL2EE;}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp588=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp585;if(_tmp588->tag != 3)goto _LL2F0;else{if(_tmp588->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL2F0;}}_LL2EF:
 goto _LL2E9;_LL2F0:;_LL2F1:
# 2106
{const char*_tmp993;void*_tmp992[1];struct Cyc_String_pa_PrintArg_struct _tmp991;(_tmp991.tag=0,((_tmp991.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp581->name)),((_tmp992[0]=& _tmp991,Cyc_Tcutil_warn(_tmp57C,((_tmp993="unique pointers reachable from %s may become unreachable",_tag_dyneither(_tmp993,sizeof(char),57))),_tag_dyneither(_tmp992,sizeof(void*),1)))))));}
goto _LL2E9;_LL2E9:;};}
# 2110
goto _LL2E4;_LL2E7:;_LL2E8:
 goto _LL2E4;_LL2E4:;};}
# 2115
static void Cyc_NewControlFlow_check_init_params(unsigned int loc,struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo flow){
union Cyc_CfFlowInfo_FlowInfo _tmp58C=flow;struct Cyc_Dict_Dict _tmp58D;_LL2F3: if((_tmp58C.BottomFL).tag != 1)goto _LL2F5;_LL2F4:
 return;_LL2F5: if((_tmp58C.ReachableFL).tag != 2)goto _LL2F2;_tmp58D=((struct _tuple14)(_tmp58C.ReachableFL).val).f1;_LL2F6:
# 2119
{struct Cyc_List_List*_tmp58E=env->param_roots;for(0;_tmp58E != 0;_tmp58E=_tmp58E->tl){
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp58D,Cyc_CfFlowInfo_lookup_place(_tmp58D,(struct Cyc_CfFlowInfo_Place*)_tmp58E->hd))!= Cyc_CfFlowInfo_AllIL){
const char*_tmp996;void*_tmp995;(_tmp995=0,Cyc_CfFlowInfo_aerr(loc,((_tmp996="function may not initialize all the parameters with attribute 'initializes'",_tag_dyneither(_tmp996,sizeof(char),76))),_tag_dyneither(_tmp995,sizeof(void*),0)));}}}
# 2123
if(Cyc_NewControlFlow_warn_lose_unique){
struct _tuple27 _tmp997;struct _tuple27 _tmp591=(_tmp997.f1=loc,((_tmp997.f2=env,((_tmp997.f3=_tmp58D,_tmp997)))));
((void(*)(void(*f)(struct _tuple27*,void*,void*),struct _tuple27*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_NewControlFlow_check_for_unused_unique,& _tmp591,_tmp58D);}
# 2127
return;_LL2F2:;}
# 2136
static struct _tuple1 Cyc_NewControlFlow_get_unconsume_pat_vars(struct _RegionHandle*frgn,struct Cyc_List_List*vds){
# 2139
struct Cyc_List_List*_tmp593=0;
struct Cyc_List_List*_tmp594=0;
{struct Cyc_List_List*x=vds;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp595=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp597;struct Cyc_Absyn_Exp*_tmp598;struct _tuple24*_tmp596=_tmp595;_tmp597=_tmp596->f1;_tmp598=_tmp596->f2;
if((_tmp597 != 0  && _tmp598 != 0) && 
Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp598->topt),0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp99A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp999;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp599=(_tmp999=_region_malloc(frgn,sizeof(*_tmp999)),((_tmp999[0]=((_tmp99A.tag=0,((_tmp99A.f1=*_tmp597,_tmp99A)))),_tmp999)));
struct Cyc_CfFlowInfo_Place*_tmp99B;struct Cyc_CfFlowInfo_Place*_tmp59A=(_tmp99B=_region_malloc(frgn,sizeof(*_tmp99B)),((_tmp99B->root=(void*)_tmp599,((_tmp99B->fields=0,_tmp99B)))));
{struct Cyc_List_List*_tmp99C;_tmp593=((_tmp99C=_region_malloc(frgn,sizeof(*_tmp99C)),((_tmp99C->hd=_tmp59A,((_tmp99C->tl=_tmp593,_tmp99C))))));}{
struct Cyc_List_List*_tmp99D;_tmp594=((_tmp99D=_region_malloc(frgn,sizeof(*_tmp99D)),((_tmp99D->hd=_tmp598,((_tmp99D->tl=_tmp594,_tmp99D))))));};}}}{
# 2151
struct _tuple1 _tmp99E;return(_tmp99E.f1=_tmp593,((_tmp99E.f2=_tmp594,_tmp99E)));};}struct _tuple28{int f1;void*f2;};
# 2157
static struct _tuple28 Cyc_NewControlFlow_noconsume_place_ok(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_CfFlowInfo_Place*place,int do_unconsume,struct Cyc_Absyn_Vardecl*vd,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2164
union Cyc_CfFlowInfo_FlowInfo _tmp5A1=flow;struct Cyc_Dict_Dict _tmp5A2;_LL2F8: if((_tmp5A1.BottomFL).tag != 1)goto _LL2FA;_LL2F9: {
# 2166
const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A1="noconsume_place_ok: flow became Bottom!",_tag_dyneither(_tmp9A1,sizeof(char),40))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}_LL2FA: if((_tmp5A1.ReachableFL).tag != 2)goto _LL2F7;_tmp5A2=((struct _tuple14)(_tmp5A1.ReachableFL).val).f1;_LL2FB: {
# 2173
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Absyn_exp_unsigned_one;
int _tmp5A6=0;
int _tmp5A7=1;
void*_tmp5A8=Cyc_CfFlowInfo_lookup_place(_tmp5A2,place);
void*_tmp5A9=_tmp5A8;
# 2185
int varok=0;
{void*_tmp5AA=_tmp5A8;struct Cyc_Absyn_Vardecl*_tmp5AC;void*_tmp5AD;_LL2FD: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5AB=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5AA;if(_tmp5AB->tag != 8)goto _LL2FF;else{_tmp5AC=_tmp5AB->f1;_tmp5AD=(void*)_tmp5AB->f2;}}_LL2FE:
# 2188
 if(vd == _tmp5AC){
_tmp5A9=_tmp5AD;
# 2191
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
# 2193
if(Cyc_CfFlowInfo_is_unique_consumed(_tmp5A5,_tmp5A7,_tmp5A9,& _tmp5A6)){
if(!do_unconsume){
const char*_tmp9A5;void*_tmp9A4[1];struct Cyc_String_pa_PrintArg_struct _tmp9A3;(_tmp9A3.tag=0,((_tmp9A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2197
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9A4[0]=& _tmp9A3,Cyc_CfFlowInfo_aerr(loc,((_tmp9A5="function consumes parameter %s with attribute 'noconsume'",_tag_dyneither(_tmp9A5,sizeof(char),58))),_tag_dyneither(_tmp9A4,sizeof(void*),1)))))));}}else{
# 2200
if(Cyc_CfFlowInfo_initlevel(env->fenv,_tmp5A2,_tmp5A9)!= Cyc_CfFlowInfo_AllIL  && !do_unconsume){
const char*_tmp9A9;void*_tmp9A8[1];struct Cyc_String_pa_PrintArg_struct _tmp9A7;(_tmp9A7.tag=0,((_tmp9A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2203
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9A8[0]=& _tmp9A7,Cyc_CfFlowInfo_aerr(loc,((_tmp9A9="function consumes value pointed to by parameter %s, which has attribute 'noconsume'",_tag_dyneither(_tmp9A9,sizeof(char),84))),_tag_dyneither(_tmp9A8,sizeof(void*),1)))))));}else{
# 2205
varok=1;}}}else{
# 2208
varok=1;}}else{
# 2211
goto _LL300;}
goto _LL2FC;_LL2FF:;_LL300:
# 2215
 if(!Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type))
varok=1;else{
if(!do_unconsume){
const char*_tmp9AD;void*_tmp9AC[1];struct Cyc_String_pa_PrintArg_struct _tmp9AB;(_tmp9AB.tag=0,((_tmp9AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2220
Cyc_Absynpp_qvar2string(vd->name)),((_tmp9AC[0]=& _tmp9AB,Cyc_CfFlowInfo_aerr(loc,((_tmp9AD="function parameter %s with attribute 'noconsume' no longer set to its original value",_tag_dyneither(_tmp9AD,sizeof(char),85))),_tag_dyneither(_tmp9AC,sizeof(void*),1)))))));}}
goto _LL2FC;_LL2FC:;}{
# 2224
struct _tuple28 _tmp9AE;return(_tmp9AE.f1=varok,((_tmp9AE.f2=_tmp5A9,_tmp9AE)));};}_LL2F7:;}
# 2230
static struct Cyc_Absyn_Vardecl*Cyc_NewControlFlow_get_vd_from_place(struct Cyc_CfFlowInfo_Place*p){
void*_tmp5B9;struct Cyc_List_List*_tmp5BA;struct Cyc_CfFlowInfo_Place*_tmp5B8=p;_tmp5B9=_tmp5B8->root;_tmp5BA=_tmp5B8->fields;
if(_tmp5BA != 0){
const char*_tmp9B1;void*_tmp9B0;(_tmp9B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B1="unconsume_params: param to unconsume is non-variable",_tag_dyneither(_tmp9B1,sizeof(char),53))),_tag_dyneither(_tmp9B0,sizeof(void*),0)));}{
struct Cyc_Absyn_Vardecl*vd;
void*_tmp5BD=_tmp5B9;struct Cyc_Absyn_Vardecl*_tmp5BF;_LL302: {struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp5BE=(struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp5BD;if(_tmp5BE->tag != 0)goto _LL304;else{_tmp5BF=_tmp5BE->f1;}}_LL303:
 return _tmp5BF;_LL304:;_LL305: {
const char*_tmp9B4;void*_tmp9B3;(_tmp9B3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B4="unconsume_params: root is not a varroot",_tag_dyneither(_tmp9B4,sizeof(char),40))),_tag_dyneither(_tmp9B3,sizeof(void*),0)));}_LL301:;};}
# 2249 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_exp(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_Absyn_Exp*unconsume_exp,struct Cyc_Absyn_Vardecl*vd,int varok,void*ropt,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2257
{union Cyc_CfFlowInfo_FlowInfo _tmp5C2=flow;struct Cyc_Dict_Dict _tmp5C3;_LL307: if((_tmp5C2.BottomFL).tag != 1)goto _LL309;_LL308:
 return flow;_LL309: if((_tmp5C2.ReachableFL).tag != 2)goto _LL306;_tmp5C3=((struct _tuple14)(_tmp5C2.ReachableFL).val).f1;_LL30A: {
# 2264
struct _tuple18 _tmp5C4=Cyc_NewControlFlow_anal_Lexp(env,flow,0,1,unconsume_exp);union Cyc_CfFlowInfo_FlowInfo _tmp5C6;union Cyc_CfFlowInfo_AbsLVal _tmp5C7;struct _tuple18 _tmp5C5=_tmp5C4;_tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;
# 2267
{union Cyc_CfFlowInfo_AbsLVal _tmp5C8=_tmp5C7;struct Cyc_CfFlowInfo_Place*_tmp5C9;_LL30C: if((_tmp5C8.PlaceL).tag != 1)goto _LL30E;_tmp5C9=(struct Cyc_CfFlowInfo_Place*)(_tmp5C8.PlaceL).val;_LL30D: {
# 2271
void*_tmp5CA=Cyc_CfFlowInfo_lookup_place(_tmp5C3,_tmp5C9);
{void*_tmp5CB=_tmp5CA;struct Cyc_Absyn_Vardecl*_tmp5CD;void*_tmp5CE;_LL311: {struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp5CC=(struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5CB;if(_tmp5CC->tag != 8)goto _LL313;else{_tmp5CD=_tmp5CC->f1;_tmp5CE=(void*)_tmp5CC->f2;}}_LL312: {
# 2277
void*new_rval;
if(_tmp5CD == vd){
# 2281
if(varok){
# 2283
_tmp5CA=Cyc_CfFlowInfo_make_unique_unconsumed(env->fenv,_tmp5CE);
# 2288
if(ropt != 0){
# 2294
struct _tuple16 _tmp9B6;struct _tuple16 _tmp9B5;struct _tuple16 _tmp5CF=
Cyc_CfFlowInfo_join_flow_and_rval(env->fenv,env->all_changed,(
# 2297
(_tmp9B5.f1=_tmp5C6,((_tmp9B5.f2=_tmp5CA,_tmp9B5)))),(
(_tmp9B6.f1=_tmp5C6,((_tmp9B6.f2=ropt,_tmp9B6)))));
# 2294
union Cyc_CfFlowInfo_FlowInfo _tmp5D1;void*_tmp5D2;struct _tuple16 _tmp5D0=_tmp5CF;_tmp5D1=_tmp5D0.f1;_tmp5D2=_tmp5D0.f2;
# 2299
_tmp5C6=_tmp5D1;new_rval=_tmp5D2;}else{
# 2304
new_rval=_tmp5CA;}}else{
# 2307
new_rval=_tmp5CE;}
# 2309
{union Cyc_CfFlowInfo_FlowInfo _tmp5D5=_tmp5C6;struct Cyc_Dict_Dict _tmp5D6;struct Cyc_List_List*_tmp5D7;_LL316: if((_tmp5D5.ReachableFL).tag != 2)goto _LL318;_tmp5D6=((struct _tuple14)(_tmp5D5.ReachableFL).val).f1;_tmp5D7=((struct _tuple14)(_tmp5D5.ReachableFL).val).f2;_LL317:
# 2311
 flow=Cyc_CfFlowInfo_ReachableFL(Cyc_CfFlowInfo_assign_place(env->fenv,loc,_tmp5D6,env->all_changed,_tmp5C9,new_rval),_tmp5D7);
# 2315
goto _LL315;_LL318:;_LL319: {
# 2317
const char*_tmp9B9;void*_tmp9B8;(_tmp9B8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9B9="unconsume_params: joined flow became bot!",_tag_dyneither(_tmp9B9,sizeof(char),42))),_tag_dyneither(_tmp9B8,sizeof(void*),0)));}_LL315:;}
# 2319
goto _LL310;}else{
# 2321
goto _LL314;}
goto _LL310;}_LL313:;_LL314:
# 2328
 if(ropt != 0  && !
Cyc_Tcutil_is_noalias_pointer_or_aggr(env->r,vd->type)){
const char*_tmp9BD;void*_tmp9BC[1];struct Cyc_String_pa_PrintArg_struct _tmp9BB;(_tmp9BB.tag=0,((_tmp9BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(unconsume_exp)),((_tmp9BC[0]=& _tmp9BB,Cyc_CfFlowInfo_aerr(loc,((_tmp9BD="aliased expression %s was overwritten",_tag_dyneither(_tmp9BD,sizeof(char),38))),_tag_dyneither(_tmp9BC,sizeof(void*),1)))))));}
# 2345 "new_control_flow.cyc"
goto _LL310;_LL310:;}
# 2347
goto _LL30B;}_LL30E:;_LL30F:
# 2353
 goto _LL30B;_LL30B:;}
# 2355
goto _LL306;}_LL306:;}
# 2357
return flow;}
# 2370 "new_control_flow.cyc"
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_params(struct Cyc_NewControlFlow_AnalEnv*env,struct Cyc_List_List*consumed_vals,struct Cyc_List_List*unconsume_exps,int is_region_open,union Cyc_CfFlowInfo_FlowInfo flow,unsigned int loc){
# 2376
{union Cyc_CfFlowInfo_FlowInfo _tmp5DD=flow;_LL31B: if((_tmp5DD.BottomFL).tag != 1)goto _LL31D;_LL31C:
 return flow;_LL31D:;_LL31E:
 goto _LL31A;_LL31A:;}{
# 2380
int _tmp5DE=unconsume_exps != 0;
{struct Cyc_List_List*_tmp5DF=consumed_vals;for(0;_tmp5DF != 0;
(_tmp5DF=_tmp5DF->tl,
unconsume_exps != 0?unconsume_exps=unconsume_exps->tl: 0)){
# 2387
struct Cyc_Absyn_Vardecl*_tmp5E0=Cyc_NewControlFlow_get_vd_from_place((struct Cyc_CfFlowInfo_Place*)_tmp5DF->hd);
struct _tuple28 _tmp9BE;struct _tuple28 _tmp5E1=
is_region_open?(_tmp9BE.f1=1,((_tmp9BE.f2=0,_tmp9BE))):
 Cyc_NewControlFlow_noconsume_place_ok(env,(struct Cyc_CfFlowInfo_Place*)_tmp5DF->hd,_tmp5DE,_tmp5E0,flow,loc);
# 2388
int _tmp5E3;void*_tmp5E4;struct _tuple28 _tmp5E2=_tmp5E1;_tmp5E3=_tmp5E2.f1;_tmp5E4=_tmp5E2.f2;
# 2394
if(_tmp5DE)
flow=Cyc_NewControlFlow_unconsume_exp(env,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(unconsume_exps))->hd,_tmp5E0,_tmp5E3,_tmp5E4,flow,loc);}}
# 2397
Cyc_NewControlFlow_update_tryflow(env,flow);
return flow;};}struct _tuple29{int f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 2401
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_scs(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_List_List*scs,unsigned int exp_loc){
# 2404
struct Cyc_CfFlowInfo_FlowEnv*_tmp5E6=env->fenv;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp5E7=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Core_Opt*_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_Stmt*_tmp5EB;unsigned int _tmp5EC;struct Cyc_Absyn_Switch_clause*_tmp5E8=_tmp5E7;_tmp5E9=_tmp5E8->pat_vars;_tmp5EA=_tmp5E8->where_clause;_tmp5EB=_tmp5E8->body;_tmp5EC=_tmp5E8->loc;{
struct _tuple1 _tmp5ED=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5E9))->v);struct Cyc_List_List*_tmp5EF;struct Cyc_List_List*_tmp5F0;struct _tuple1 _tmp5EE=_tmp5ED;_tmp5EF=_tmp5EE.f1;_tmp5F0=_tmp5EE.f2;{
union Cyc_CfFlowInfo_FlowInfo clause_inflow=Cyc_NewControlFlow_initialize_pat_vars(env->fenv,env,inflow,(struct Cyc_List_List*)_tmp5E9->v,_tmp5EF != 0,_tmp5EC,exp_loc);
# 2412
union Cyc_CfFlowInfo_FlowInfo clause_outflow;
struct Cyc_List_List*_tmp5F1=env->unique_pat_vars;
{struct _tuple29*_tmp9C1;struct Cyc_List_List*_tmp9C0;env->unique_pat_vars=(
(_tmp9C0=_region_malloc(env->r,sizeof(*_tmp9C0)),((_tmp9C0->hd=((_tmp9C1=_region_malloc(env->r,sizeof(*_tmp9C1)),((_tmp9C1->f1=0,((_tmp9C1->f2=_tmp5EB,((_tmp9C1->f3=_tmp5EF,((_tmp9C1->f4=_tmp5F0,_tmp9C1)))))))))),((_tmp9C0->tl=_tmp5F1,_tmp9C0))))));}
# 2417
if(_tmp5EA != 0){
struct Cyc_Absyn_Exp*wexp=_tmp5EA;
struct _tuple19 _tmp5F4=Cyc_NewControlFlow_anal_test(env,clause_inflow,wexp);union Cyc_CfFlowInfo_FlowInfo _tmp5F6;union Cyc_CfFlowInfo_FlowInfo _tmp5F7;struct _tuple19 _tmp5F5=_tmp5F4;_tmp5F6=_tmp5F5.f1;_tmp5F7=_tmp5F5.f2;
inflow=_tmp5F7;
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,_tmp5F6,_tmp5EB,0);}else{
# 2423
clause_outflow=Cyc_NewControlFlow_anal_stmt(env,clause_inflow,_tmp5EB,0);}
# 2425
env->unique_pat_vars=_tmp5F1;{
union Cyc_CfFlowInfo_FlowInfo _tmp5F8=clause_outflow;_LL320: if((_tmp5F8.BottomFL).tag != 1)goto _LL322;_LL321:
 goto _LL31F;_LL322:;_LL323:
# 2430
 clause_outflow=Cyc_NewControlFlow_unconsume_params(env,_tmp5EF,_tmp5F0,0,clause_outflow,_tmp5EC);
# 2432
if(scs->tl == 0)
return clause_outflow;else{
# 2437
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->pat_vars))->v != 0){
const char*_tmp9C4;void*_tmp9C3;(_tmp9C3=0,Cyc_CfFlowInfo_aerr(_tmp5EB->loc,((_tmp9C4="switch clause may implicitly fallthru",_tag_dyneither(_tmp9C4,sizeof(char),38))),_tag_dyneither(_tmp9C3,sizeof(void*),0)));}else{
# 2440
const char*_tmp9C7;void*_tmp9C6;(_tmp9C6=0,Cyc_Tcutil_warn(_tmp5EB->loc,((_tmp9C7="switch clause may implicitly fallthru",_tag_dyneither(_tmp9C7,sizeof(char),38))),_tag_dyneither(_tmp9C6,sizeof(void*),0)));}
# 2442
Cyc_NewControlFlow_update_flow(env,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(scs->tl))->hd)->body,clause_outflow);}
# 2444
goto _LL31F;_LL31F:;};};};}
# 2447
return Cyc_CfFlowInfo_BottomFL();}struct _tuple30{struct Cyc_NewControlFlow_AnalEnv*f1;struct Cyc_Dict_Dict f2;unsigned int f3;};
# 2452
static void Cyc_NewControlFlow_check_dropped_unique_vd(struct _tuple30*vdenv,struct Cyc_Absyn_Vardecl*vd){
# 2454
struct Cyc_NewControlFlow_AnalEnv*_tmp5FE;struct Cyc_Dict_Dict _tmp5FF;unsigned int _tmp600;struct _tuple30*_tmp5FD=vdenv;_tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;_tmp600=_tmp5FD->f3;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((_tmp5FE->fenv)->r,vd->type)){
# 2457
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9C8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct vdroot=(_tmp9C8.tag=0,((_tmp9C8.f1=vd,_tmp9C8)));
# 2459
struct _tuple15 _tmp601=Cyc_CfFlowInfo_unname_rval((_tmp5FE->fenv)->r,
((void*(*)(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k))Cyc_Dict_lookup_other)(_tmp5FF,Cyc_CfFlowInfo_root_cmp,(void*)& vdroot));
# 2459
void*_tmp603;struct _tuple15 _tmp602=_tmp601;_tmp603=_tmp602.f1;{
# 2461
void*_tmp604=_tmp603;_LL325: {struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp605=(struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp604;if(_tmp605->tag != 7)goto _LL327;}_LL326:
 goto _LL328;_LL327: {struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp606=(struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*)_tmp604;if(_tmp606->tag != 0)goto _LL329;}_LL328:
 goto _LL32A;_LL329: {struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp607=(struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp604;if(_tmp607->tag != 3)goto _LL32B;else{if(_tmp607->f1 != Cyc_CfFlowInfo_NoneIL)goto _LL32B;}}_LL32A:
 goto _LL324;_LL32B:;_LL32C:
# 2466
{const char*_tmp9CC;void*_tmp9CB[1];struct Cyc_String_pa_PrintArg_struct _tmp9CA;(_tmp9CA.tag=0,((_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name)),((_tmp9CB[0]=& _tmp9CA,Cyc_Tcutil_warn(_tmp600,((_tmp9CC="unique pointers may still exist after variable %s goes out of scope",_tag_dyneither(_tmp9CC,sizeof(char),68))),_tag_dyneither(_tmp9CB,sizeof(void*),1)))))));}
# 2468
goto _LL324;_LL324:;};}}
# 2473
static void Cyc_NewControlFlow_check_dropped_unique(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
# 2475
{union Cyc_CfFlowInfo_FlowInfo _tmp60C=inflow;struct Cyc_Dict_Dict _tmp60D;_LL32E: if((_tmp60C.ReachableFL).tag != 2)goto _LL330;_tmp60D=((struct _tuple14)(_tmp60C.ReachableFL).val).f1;_LL32F: {
# 2477
struct _tuple30 _tmp9CD;struct _tuple30 _tmp60E=(_tmp9CD.f1=env,((_tmp9CD.f2=_tmp60D,((_tmp9CD.f3=decl->loc,_tmp9CD)))));
struct Cyc_CfFlowInfo_FlowEnv*_tmp60F=env->fenv;
{void*_tmp610=decl->r;void*_tmp611=_tmp610;struct Cyc_Absyn_Vardecl*_tmp613;struct Cyc_List_List*_tmp615;struct Cyc_List_List*_tmp617;_LL333: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp612=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp611;if(_tmp612->tag != 0)goto _LL335;else{_tmp613=_tmp612->f1;}}_LL334:
# 2481
 Cyc_NewControlFlow_check_dropped_unique_vd(& _tmp60E,_tmp613);
goto _LL332;_LL335: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp614=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp611;if(_tmp614->tag != 2)goto _LL337;else{if(_tmp614->f2 == 0)goto _LL337;_tmp615=(struct Cyc_List_List*)(_tmp614->f2)->v;}}_LL336: {
# 2484
struct _tuple1 _tmp618=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp615);struct Cyc_List_List*_tmp61A;struct _tuple1 _tmp619=_tmp618;_tmp61A=_tmp619.f1;{
struct Cyc_List_List*_tmp61B=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)(_tmp61A);
_tmp617=_tmp61B;goto _LL338;};}_LL337: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp616=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp611;if(_tmp616->tag != 3)goto _LL339;else{_tmp617=_tmp616->f1;}}_LL338:
# 2488
((void(*)(void(*f)(struct _tuple30*,struct Cyc_Absyn_Vardecl*),struct _tuple30*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_NewControlFlow_check_dropped_unique_vd,& _tmp60E,_tmp617);
goto _LL332;_LL339:;_LL33A:
 goto _LL332;_LL332:;}
# 2492
goto _LL32D;}_LL330:;_LL331:
 goto _LL32D;_LL32D:;}
# 2495
return;}
# 2501
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_unconsume_pat_vars(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*dest){
# 2504
int keep_going=1;
{struct Cyc_List_List*x=env->unique_pat_vars;for(0;x != 0  && keep_going;x=x->tl){
struct _tuple29*_tmp61D=(struct _tuple29*)x->hd;int _tmp61F;struct Cyc_Absyn_Stmt*_tmp620;struct Cyc_List_List*_tmp621;struct Cyc_List_List*_tmp622;struct _tuple29*_tmp61E=_tmp61D;_tmp61F=_tmp61E->f1;_tmp620=_tmp61E->f2;_tmp621=_tmp61E->f3;_tmp622=_tmp61E->f4;
keep_going=0;{
# 2512
struct Cyc_Absyn_Stmt*_tmp623=(Cyc_NewControlFlow_get_stmt_annot(dest))->encloser;
while(_tmp623 != _tmp620){
struct Cyc_Absyn_Stmt*_tmp624=(Cyc_NewControlFlow_get_stmt_annot(_tmp623))->encloser;
if(_tmp624 == _tmp623){
# 2518
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp621,_tmp622,_tmp61F,inflow,dest->loc);
keep_going=1;
break;}
# 2522
_tmp623=_tmp624;}};}}
# 2525
return inflow;}
# 2531
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_stmt(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Stmt*s,struct _tuple17*rval_opt){
# 2534
union Cyc_CfFlowInfo_FlowInfo outflow;
struct _tuple20 _tmp625=Cyc_NewControlFlow_pre_stmt_check(env,inflow,s);struct Cyc_NewControlFlow_CFStmtAnnot*_tmp627;union Cyc_CfFlowInfo_FlowInfo*_tmp628;struct _tuple20 _tmp626=_tmp625;_tmp627=_tmp626.f1;_tmp628=_tmp626.f2;
inflow=*_tmp628;{
struct Cyc_CfFlowInfo_FlowEnv*_tmp629=env->fenv;
# 2541
void*_tmp62A=s->r;void*_tmp62B=_tmp62A;struct Cyc_Absyn_Exp*_tmp630;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Stmt*_tmp634;struct Cyc_Absyn_Stmt*_tmp635;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_Absyn_Stmt*_tmp638;struct Cyc_Absyn_Stmt*_tmp639;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Stmt*_tmp63D;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Stmt*_tmp641;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Stmt*_tmp645;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Stmt*_tmp647;struct Cyc_Absyn_Stmt*_tmp648;struct Cyc_List_List*_tmp64A;struct Cyc_Absyn_Switch_clause*_tmp64B;struct Cyc_Absyn_Stmt*_tmp64D;struct Cyc_Absyn_Stmt*_tmp64F;struct Cyc_Absyn_Stmt*_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Stmt*_tmp656;struct Cyc_List_List*_tmp657;struct Cyc_List_List*_tmp65A;struct Cyc_Absyn_Exp*_tmp65B;unsigned int _tmp65C;struct Cyc_Absyn_Stmt*_tmp65D;struct Cyc_Absyn_Vardecl*_tmp660;struct Cyc_Absyn_Exp*_tmp661;unsigned int _tmp662;struct Cyc_Absyn_Stmt*_tmp663;struct Cyc_Absyn_Decl*_tmp665;struct Cyc_Absyn_Stmt*_tmp666;struct Cyc_Absyn_Stmt*_tmp668;struct Cyc_Absyn_Exp*_tmp66A;_LL33C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp62C=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp62C->tag != 0)goto _LL33E;}_LL33D:
 return inflow;_LL33E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp62D=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp62D->tag != 6)goto _LL340;else{if(_tmp62D->f1 != 0)goto _LL340;}}_LL33F:
# 2547
{union Cyc_CfFlowInfo_FlowInfo _tmp66B=inflow;_LL369: if((_tmp66B.ReachableFL).tag != 2)goto _LL36B;_LL36A:
# 2549
{void*_tmp66C=Cyc_Tcutil_compress(env->return_type);void*_tmp66D=_tmp66C;_LL36E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp66E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp66D;if(_tmp66E->tag != 0)goto _LL370;}_LL36F:
 goto _LL36D;_LL370: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp66F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp66D;if(_tmp66F->tag != 7)goto _LL372;}_LL371:
 goto _LL373;_LL372: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp670=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66D;if(_tmp670->tag != 6)goto _LL374;}_LL373:
# 2553
{const char*_tmp9D0;void*_tmp9CF;(_tmp9CF=0,Cyc_Tcutil_warn(s->loc,((_tmp9D0="break may cause function not to return a value",_tag_dyneither(_tmp9D0,sizeof(char),47))),_tag_dyneither(_tmp9CF,sizeof(void*),0)));}
goto _LL36D;_LL374:;_LL375:
# 2556
{const char*_tmp9D3;void*_tmp9D2;(_tmp9D2=0,Cyc_Tcutil_terr(s->loc,((_tmp9D3="break may cause function not to return a value",_tag_dyneither(_tmp9D3,sizeof(char),47))),_tag_dyneither(_tmp9D2,sizeof(void*),0)));}
goto _LL36D;_LL36D:;}
# 2559
goto _LL368;_LL36B:;_LL36C:
 goto _LL368;_LL368:;}
# 2562
goto _LL341;_LL340: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp62E=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp62E->tag != 3)goto _LL342;else{if(_tmp62E->f1 != 0)goto _LL342;}}_LL341:
# 2564
 if(env->noreturn){
const char*_tmp9D6;void*_tmp9D5;(_tmp9D5=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9D6="`noreturn' function might return",_tag_dyneither(_tmp9D6,sizeof(char),33))),_tag_dyneither(_tmp9D5,sizeof(void*),0)));}
Cyc_NewControlFlow_check_init_params(s->loc,env,inflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,inflow,s->loc);
return Cyc_CfFlowInfo_BottomFL();_LL342: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp62F=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp62F->tag != 3)goto _LL344;else{_tmp630=_tmp62F->f1;}}_LL343:
# 2570
 if(env->noreturn){
const char*_tmp9D9;void*_tmp9D8;(_tmp9D8=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9D9="`noreturn' function might return",_tag_dyneither(_tmp9D9,sizeof(char),33))),_tag_dyneither(_tmp9D8,sizeof(void*),0)));}{
struct _tuple16 _tmp679=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,(struct Cyc_Absyn_Exp*)_check_null(_tmp630));union Cyc_CfFlowInfo_FlowInfo _tmp67B;void*_tmp67C;struct _tuple16 _tmp67A=_tmp679;_tmp67B=_tmp67A.f1;_tmp67C=_tmp67A.f2;
_tmp67B=Cyc_NewControlFlow_use_Rval(env,_tmp630->loc,_tmp67B,_tmp67C);
Cyc_NewControlFlow_check_init_params(s->loc,env,_tmp67B);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,_tmp67B,s->loc);
return Cyc_CfFlowInfo_BottomFL();};_LL344: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp631=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp631->tag != 1)goto _LL346;else{_tmp632=_tmp631->f1;}}_LL345: {
# 2579
struct _tuple17*_tmp67D=rval_opt;void**_tmp67E;int _tmp67F;_LL377: if(_tmp67D == 0)goto _LL379;_tmp67E=(void**)& _tmp67D->f1;_tmp67F=_tmp67D->f2;_LL378: {
# 2581
struct _tuple16 _tmp680=Cyc_NewControlFlow_anal_Rexp(env,_tmp67F,inflow,_tmp632);union Cyc_CfFlowInfo_FlowInfo _tmp682;void*_tmp683;struct _tuple16 _tmp681=_tmp680;_tmp682=_tmp681.f1;_tmp683=_tmp681.f2;
*_tmp67E=_tmp683;
return _tmp682;}_LL379: if(_tmp67D != 0)goto _LL376;_LL37A:
# 2585
 return(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp632)).f1;_LL376:;}_LL346: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp633=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp633->tag != 2)goto _LL348;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;}}_LL347:
# 2589
 return Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp634,0),_tmp635,rval_opt);_LL348: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp636=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp636->tag != 4)goto _LL34A;else{_tmp637=_tmp636->f1;_tmp638=_tmp636->f2;_tmp639=_tmp636->f3;}}_LL349: {
# 2592
struct _tuple19 _tmp684=Cyc_NewControlFlow_anal_test(env,inflow,_tmp637);union Cyc_CfFlowInfo_FlowInfo _tmp686;union Cyc_CfFlowInfo_FlowInfo _tmp687;struct _tuple19 _tmp685=_tmp684;_tmp686=_tmp685.f1;_tmp687=_tmp685.f2;{
# 2599
union Cyc_CfFlowInfo_FlowInfo _tmp688=Cyc_NewControlFlow_anal_stmt(env,_tmp687,_tmp639,0);
union Cyc_CfFlowInfo_FlowInfo _tmp689=Cyc_NewControlFlow_anal_stmt(env,_tmp686,_tmp638,0);
return Cyc_CfFlowInfo_join_flow(_tmp629,env->all_changed,_tmp689,_tmp688);};}_LL34A: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp63A=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp63A->tag != 5)goto _LL34C;else{_tmp63B=(_tmp63A->f1).f1;_tmp63C=(_tmp63A->f1).f2;_tmp63D=_tmp63A->f2;}}_LL34B: {
# 2607
struct _tuple20 _tmp68A=Cyc_NewControlFlow_pre_stmt_check(env,inflow,_tmp63C);union Cyc_CfFlowInfo_FlowInfo*_tmp68C;struct _tuple20 _tmp68B=_tmp68A;_tmp68C=_tmp68B.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp68D=*_tmp68C;
struct _tuple19 _tmp68E=Cyc_NewControlFlow_anal_test(env,_tmp68D,_tmp63B);union Cyc_CfFlowInfo_FlowInfo _tmp690;union Cyc_CfFlowInfo_FlowInfo _tmp691;struct _tuple19 _tmp68F=_tmp68E;_tmp690=_tmp68F.f1;_tmp691=_tmp68F.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp692=Cyc_NewControlFlow_anal_stmt(env,_tmp690,_tmp63D,0);
Cyc_NewControlFlow_update_flow(env,_tmp63C,_tmp692);
return _tmp691;};};}_LL34C: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp63E=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp63E->tag != 14)goto _LL34E;else{_tmp63F=_tmp63E->f1;_tmp640=(_tmp63E->f2).f1;_tmp641=(_tmp63E->f2).f2;}}_LL34D: {
# 2617
union Cyc_CfFlowInfo_FlowInfo _tmp693=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp63F,0);
struct _tuple20 _tmp694=Cyc_NewControlFlow_pre_stmt_check(env,_tmp693,_tmp641);union Cyc_CfFlowInfo_FlowInfo*_tmp696;struct _tuple20 _tmp695=_tmp694;_tmp696=_tmp695.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp697=*_tmp696;
struct _tuple19 _tmp698=Cyc_NewControlFlow_anal_test(env,_tmp697,_tmp640);union Cyc_CfFlowInfo_FlowInfo _tmp69A;union Cyc_CfFlowInfo_FlowInfo _tmp69B;struct _tuple19 _tmp699=_tmp698;_tmp69A=_tmp699.f1;_tmp69B=_tmp699.f2;
Cyc_NewControlFlow_update_flow(env,_tmp63F,_tmp69A);
return _tmp69B;};}_LL34E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp642=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp642->tag != 9)goto _LL350;else{_tmp643=_tmp642->f1;_tmp644=(_tmp642->f2).f1;_tmp645=(_tmp642->f2).f2;_tmp646=(_tmp642->f3).f1;_tmp647=(_tmp642->f3).f2;_tmp648=_tmp642->f4;}}_LL34F: {
# 2626
union Cyc_CfFlowInfo_FlowInfo _tmp69C=(Cyc_NewControlFlow_anal_Rexp(env,0,inflow,_tmp643)).f1;
struct _tuple20 _tmp69D=Cyc_NewControlFlow_pre_stmt_check(env,_tmp69C,_tmp645);union Cyc_CfFlowInfo_FlowInfo*_tmp69F;struct _tuple20 _tmp69E=_tmp69D;_tmp69F=_tmp69E.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A0=*_tmp69F;
struct _tuple19 _tmp6A1=Cyc_NewControlFlow_anal_test(env,_tmp6A0,_tmp644);union Cyc_CfFlowInfo_FlowInfo _tmp6A3;union Cyc_CfFlowInfo_FlowInfo _tmp6A4;struct _tuple19 _tmp6A2=_tmp6A1;_tmp6A3=_tmp6A2.f1;_tmp6A4=_tmp6A2.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A5=Cyc_NewControlFlow_anal_stmt(env,_tmp6A3,_tmp648,0);
struct _tuple20 _tmp6A6=Cyc_NewControlFlow_pre_stmt_check(env,_tmp6A5,_tmp647);union Cyc_CfFlowInfo_FlowInfo*_tmp6A8;struct _tuple20 _tmp6A7=_tmp6A6;_tmp6A8=_tmp6A7.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6A9=*_tmp6A8;
union Cyc_CfFlowInfo_FlowInfo _tmp6AA=(Cyc_NewControlFlow_anal_Rexp(env,0,_tmp6A9,_tmp646)).f1;
Cyc_NewControlFlow_update_flow(env,_tmp645,_tmp6AA);
return _tmp6A4;};};};}_LL350: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp649=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp649->tag != 11)goto _LL352;else{_tmp64A=_tmp649->f1;if(_tmp649->f2 == 0)goto _LL352;_tmp64B=*_tmp649->f2;}}_LL351: {
# 2638
struct _RegionHandle _tmp6AB=_new_region("temp");struct _RegionHandle*temp=& _tmp6AB;_push_region(temp);
{struct _tuple23 _tmp6AC=Cyc_NewControlFlow_anal_unordered_Rexps(temp,env,inflow,_tmp64A,1,1);union Cyc_CfFlowInfo_FlowInfo _tmp6AE;struct Cyc_List_List*_tmp6AF;struct _tuple23 _tmp6AD=_tmp6AC;_tmp6AE=_tmp6AD.f1;_tmp6AF=_tmp6AD.f2;
# 2641
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,_tmp64B->body);{
# 2643
struct Cyc_List_List*_tmp6B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp64B->pat_vars))->v)).f1);
_tmp6AE=Cyc_NewControlFlow_add_vars(_tmp629,_tmp6AE,_tmp6B0,_tmp629->unknown_all,s->loc,0);
# 2646
{struct Cyc_List_List*x=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp64B->pat_vars))->v;for(0;x != 0;x=x->tl){
struct _tuple24*_tmp6B1=(struct _tuple24*)x->hd;struct Cyc_Absyn_Vardecl**_tmp6B3;struct Cyc_Absyn_Exp*_tmp6B4;struct _tuple24*_tmp6B2=_tmp6B1;_tmp6B3=_tmp6B2->f1;_tmp6B4=_tmp6B2->f2;
if(_tmp6B3 != 0){
_tmp6AE=Cyc_NewControlFlow_do_initialize_var(_tmp629,env,_tmp6AE,*_tmp6B3,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp64A))->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp6AF))->hd,s->loc);
_tmp6AF=_tmp6AF->tl;
_tmp64A=_tmp64A->tl;}}}
# 2654
Cyc_NewControlFlow_update_flow(env,_tmp64B->body,_tmp6AE);{
union Cyc_CfFlowInfo_FlowInfo _tmp6B5=Cyc_CfFlowInfo_BottomFL();_npop_handler(0);return _tmp6B5;};};}
# 2639
;_pop_region(temp);}_LL352: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp64C=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp64C->tag != 6)goto _LL354;else{_tmp64D=_tmp64C->f1;}}_LL353:
# 2657
 _tmp64F=_tmp64D;goto _LL355;_LL354: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp64E=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp64E->tag != 7)goto _LL356;else{_tmp64F=_tmp64E->f1;}}_LL355:
 _tmp651=_tmp64F;goto _LL357;_LL356: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp650=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp650->tag != 8)goto _LL358;else{_tmp651=_tmp650->f2;}}_LL357:
# 2662
 if(env->iteration_num == 1){
struct Cyc_Absyn_Stmt*_tmp6B6=_tmp627->encloser;
struct Cyc_Absyn_Stmt*_tmp6B7=(Cyc_NewControlFlow_get_stmt_annot((struct Cyc_Absyn_Stmt*)_check_null(_tmp651)))->encloser;
while(_tmp6B7 != _tmp6B6){
struct Cyc_Absyn_Stmt*_tmp6B8=(Cyc_NewControlFlow_get_stmt_annot(_tmp6B6))->encloser;
if(_tmp6B8 == _tmp6B6){
{const char*_tmp9DC;void*_tmp9DB;(_tmp9DB=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9DC="goto enters local scope or exception handler",_tag_dyneither(_tmp9DC,sizeof(char),45))),_tag_dyneither(_tmp9DB,sizeof(void*),0)));}
break;}
# 2671
_tmp6B6=_tmp6B8;}}
# 2675
inflow=Cyc_NewControlFlow_unconsume_pat_vars(env,inflow,(struct Cyc_Absyn_Stmt*)_check_null(_tmp651));
# 2677
Cyc_NewControlFlow_update_flow(env,_tmp651,inflow);
return Cyc_CfFlowInfo_BottomFL();_LL358: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp652=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp652->tag != 10)goto _LL35A;else{_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;}}_LL359:
# 2683
 return Cyc_NewControlFlow_anal_scs(env,inflow,_tmp654,_tmp653->loc);_LL35A: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp655=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp655->tag != 15)goto _LL35C;else{_tmp656=_tmp655->f1;_tmp657=_tmp655->f2;}}_LL35B: {
# 2688
int old_in_try=env->in_try;
union Cyc_CfFlowInfo_FlowInfo old_tryflow=env->tryflow;
env->in_try=1;
env->tryflow=inflow;{
union Cyc_CfFlowInfo_FlowInfo s1_outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp656,0);
union Cyc_CfFlowInfo_FlowInfo scs_inflow=env->tryflow;
# 2696
env->in_try=old_in_try;
env->tryflow=old_tryflow;
# 2699
Cyc_NewControlFlow_update_tryflow(env,scs_inflow);{
union Cyc_CfFlowInfo_FlowInfo scs_outflow=Cyc_NewControlFlow_anal_scs(env,scs_inflow,_tmp657,0);
{union Cyc_CfFlowInfo_FlowInfo _tmp6BB=scs_outflow;_LL37C: if((_tmp6BB.BottomFL).tag != 1)goto _LL37E;_LL37D:
 goto _LL37B;_LL37E:;_LL37F: {
const char*_tmp9DF;void*_tmp9DE;(_tmp9DE=0,Cyc_CfFlowInfo_aerr(s->loc,((_tmp9DF="last catch clause may implicitly fallthru",_tag_dyneither(_tmp9DF,sizeof(char),42))),_tag_dyneither(_tmp9DE,sizeof(void*),0)));}_LL37B:;}
# 2705
outflow=s1_outflow;
# 2707
return outflow;};};}_LL35C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp658=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp658->tag != 12)goto _LL35E;else{{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp659=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)(_tmp658->f1)->r;if(_tmp659->tag != 2)goto _LL35E;else{if(_tmp659->f2 == 0)goto _LL35E;_tmp65A=(struct Cyc_List_List*)(_tmp659->f2)->v;_tmp65B=_tmp659->f3;}}_tmp65C=(_tmp658->f1)->loc;_tmp65D=_tmp658->f2;}}_LL35D: {
# 2717
struct _tuple1 _tmp6BE=Cyc_NewControlFlow_get_unconsume_pat_vars((env->fenv)->r,_tmp65A);struct Cyc_List_List*_tmp6C0;struct Cyc_List_List*_tmp6C1;struct _tuple1 _tmp6BF=_tmp6BE;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;
inflow=Cyc_NewControlFlow_initialize_pat_vars(_tmp629,env,inflow,_tmp65A,_tmp6C0 != 0,_tmp65C,_tmp65B->loc);{
struct Cyc_List_List*_tmp6C2=env->unique_pat_vars;
{struct _tuple29*_tmp9E2;struct Cyc_List_List*_tmp9E1;env->unique_pat_vars=(
(_tmp9E1=_region_malloc(env->r,sizeof(*_tmp9E1)),((_tmp9E1->hd=((_tmp9E2=_region_malloc(env->r,sizeof(*_tmp9E2)),((_tmp9E2->f1=0,((_tmp9E2->f2=_tmp65D,((_tmp9E2->f3=_tmp6C0,((_tmp9E2->f4=_tmp6C1,_tmp9E2)))))))))),((_tmp9E1->tl=_tmp6C2,_tmp9E1))))));}
# 2725
inflow=Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp65D,rval_opt);
env->unique_pat_vars=_tmp6C2;
# 2729
inflow=Cyc_NewControlFlow_unconsume_params(env,_tmp6C0,_tmp6C1,0,inflow,_tmp65C);
# 2733
return inflow;};}_LL35E:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp65E=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp65E->tag != 12)goto _LL360;else{{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp65F=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)(_tmp65E->f1)->r;if(_tmp65F->tag != 4)goto _LL360;else{_tmp660=_tmp65F->f2;_tmp661=_tmp65F->f4;}}_tmp662=(_tmp65E->f1)->loc;_tmp663=_tmp65E->f2;}}if(!(_tmp661 != 0))goto _LL360;_LL35F: {
# 2745
struct Cyc_List_List _tmp9E3;struct Cyc_List_List l=(_tmp9E3.hd=_tmp661,((_tmp9E3.tl=0,_tmp9E3)));
union Cyc_CfFlowInfo_FlowInfo _tmp6C5=Cyc_NewControlFlow_expand_unique_places(env,inflow,& l);
struct _tuple18 _tmp6C6=Cyc_NewControlFlow_anal_Lexp(env,_tmp6C5,0,0,_tmp661);union Cyc_CfFlowInfo_AbsLVal _tmp6C8;struct _tuple18 _tmp6C7=_tmp6C6;_tmp6C8=_tmp6C7.f2;{
struct _tuple16 _tmp6C9=Cyc_NewControlFlow_anal_Rexp(env,1,_tmp6C5,_tmp661);union Cyc_CfFlowInfo_FlowInfo _tmp6CB;struct _tuple16 _tmp6CA=_tmp6C9;_tmp6CB=_tmp6CA.f1;{
struct Cyc_List_List*_tmp6CC=0;
struct Cyc_List_List*_tmp6CD=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp6CE=_tmp6CB;struct Cyc_Dict_Dict _tmp6CF;struct Cyc_List_List*_tmp6D0;_LL381: if((_tmp6CE.ReachableFL).tag != 2)goto _LL383;_tmp6CF=((struct _tuple14)(_tmp6CE.ReachableFL).val).f1;_tmp6D0=((struct _tuple14)(_tmp6CE.ReachableFL).val).f2;_LL382:
# 2753
{union Cyc_CfFlowInfo_AbsLVal _tmp6D1=_tmp6C8;struct Cyc_CfFlowInfo_Place*_tmp6D2;_LL386: if((_tmp6D1.PlaceL).tag != 1)goto _LL388;_tmp6D2=(struct Cyc_CfFlowInfo_Place*)(_tmp6D1.PlaceL).val;_LL387: {
# 2757
void*_tmp6D3=Cyc_CfFlowInfo_lookup_place(_tmp6CF,_tmp6D2);
{struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct _tmp9E6;struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp9E5;_tmp6D3=(void*)((_tmp9E5=_region_malloc(_tmp629->r,sizeof(*_tmp9E5)),((_tmp9E5[0]=((_tmp9E6.tag=8,((_tmp9E6.f1=_tmp660,((_tmp9E6.f2=_tmp6D3,_tmp9E6)))))),_tmp9E5))));}
_tmp6CF=Cyc_CfFlowInfo_assign_place(_tmp629,_tmp661->loc,_tmp6CF,env->all_changed,_tmp6D2,_tmp6D3);
# 2761
_tmp6CB=Cyc_CfFlowInfo_ReachableFL(_tmp6CF,_tmp6D0);{
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmp9E9;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp9E8;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp6D6=(_tmp9E8=_region_malloc(_tmp629->r,sizeof(*_tmp9E8)),((_tmp9E8[0]=((_tmp9E9.tag=0,((_tmp9E9.f1=_tmp660,_tmp9E9)))),_tmp9E8)));
struct Cyc_CfFlowInfo_Place*_tmp9EA;struct Cyc_CfFlowInfo_Place*_tmp6D7=(_tmp9EA=_region_malloc(_tmp629->r,sizeof(*_tmp9EA)),((_tmp9EA->root=(void*)_tmp6D6,((_tmp9EA->fields=0,_tmp9EA)))));
{struct Cyc_List_List*_tmp9EB;_tmp6CC=((_tmp9EB=_region_malloc(_tmp629->r,sizeof(*_tmp9EB)),((_tmp9EB->hd=_tmp6D7,((_tmp9EB->tl=_tmp6CC,_tmp9EB))))));}
{struct Cyc_List_List*_tmp9EC;_tmp6CD=((_tmp9EC=_region_malloc(_tmp629->r,sizeof(*_tmp9EC)),((_tmp9EC->hd=_tmp661,((_tmp9EC->tl=_tmp6CD,_tmp9EC))))));}
goto _LL385;};}_LL388:;_LL389:
# 2773
 goto _LL385;_LL385:;}
# 2775
goto _LL380;_LL383:;_LL384:
# 2777
 goto _LL380;_LL380:;}{
# 2780
struct Cyc_List_List _tmp9ED;struct Cyc_List_List _tmp6DD=(_tmp9ED.hd=_tmp660,((_tmp9ED.tl=0,_tmp9ED)));
_tmp6CB=Cyc_NewControlFlow_add_vars(_tmp629,_tmp6CB,& _tmp6DD,_tmp629->unknown_all,_tmp662,0);{
# 2783
struct Cyc_List_List*_tmp6DE=env->unique_pat_vars;
{struct _tuple29*_tmp9F0;struct Cyc_List_List*_tmp9EF;env->unique_pat_vars=(
(_tmp9EF=_region_malloc(env->r,sizeof(*_tmp9EF)),((_tmp9EF->hd=((_tmp9F0=_region_malloc(env->r,sizeof(*_tmp9F0)),((_tmp9F0->f1=1,((_tmp9F0->f2=_tmp663,((_tmp9F0->f3=_tmp6CC,((_tmp9F0->f4=_tmp6CD,_tmp9F0)))))))))),((_tmp9EF->tl=_tmp6DE,_tmp9EF))))));}
# 2789
_tmp6CB=Cyc_NewControlFlow_anal_stmt(env,_tmp6CB,_tmp663,rval_opt);
env->unique_pat_vars=_tmp6DE;
# 2793
_tmp6CB=Cyc_NewControlFlow_unconsume_params(env,_tmp6CC,_tmp6CD,1,_tmp6CB,_tmp662);
# 2797
return _tmp6CB;};};};};}_LL360: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp664=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp664->tag != 12)goto _LL362;else{_tmp665=_tmp664->f1;_tmp666=_tmp664->f2;}}_LL361:
# 2800
 outflow=Cyc_NewControlFlow_anal_stmt(env,Cyc_NewControlFlow_anal_decl(env,inflow,_tmp665),_tmp666,rval_opt);
if(Cyc_NewControlFlow_warn_lose_unique)
Cyc_NewControlFlow_check_dropped_unique(env,outflow,_tmp665);
return outflow;_LL362: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp667=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp667->tag != 13)goto _LL364;else{_tmp668=_tmp667->f2;}}_LL363:
# 2805
 return Cyc_NewControlFlow_anal_stmt(env,inflow,_tmp668,rval_opt);_LL364: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp669=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp62B;if(_tmp669->tag != 16)goto _LL366;else{_tmp66A=_tmp669->f1;}}_LL365: {
# 2808
struct _tuple16 _tmp6E3=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp66A);union Cyc_CfFlowInfo_FlowInfo _tmp6E5;void*_tmp6E6;struct _tuple16 _tmp6E4=_tmp6E3;_tmp6E5=_tmp6E4.f1;_tmp6E6=_tmp6E4.f2;{
union Cyc_CfFlowInfo_FlowInfo _tmp6E7=Cyc_NewControlFlow_use_Rval(env,_tmp66A->loc,_tmp6E5,_tmp6E6);
void*_tmp6E8=Cyc_Tcutil_compress((void*)_check_null(_tmp66A->topt));void*_tmp6E9=_tmp6E8;void*_tmp6EB;_LL38B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6EA=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6E9;if(_tmp6EA->tag != 15)goto _LL38D;else{_tmp6EB=(void*)_tmp6EA->f1;}}_LL38C:
# 2812
 return Cyc_CfFlowInfo_kill_flow_region(_tmp629,_tmp6E5,_tmp6EB);_LL38D:;_LL38E: {
struct Cyc_Core_Impossible_exn_struct _tmp9F6;const char*_tmp9F5;struct Cyc_Core_Impossible_exn_struct*_tmp9F4;(int)_throw((void*)((_tmp9F4=_cycalloc(sizeof(*_tmp9F4)),((_tmp9F4[0]=((_tmp9F6.tag=Cyc_Core_Impossible,((_tmp9F6.f1=((_tmp9F5="anal_stmt -- reset_region",_tag_dyneither(_tmp9F5,sizeof(char),26))),_tmp9F6)))),_tmp9F4)))));}_LL38A:;};}_LL366:;_LL367: {
# 2816
struct Cyc_Core_Impossible_exn_struct _tmp9FC;const char*_tmp9FB;struct Cyc_Core_Impossible_exn_struct*_tmp9FA;(int)_throw((void*)((_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA[0]=((_tmp9FC.tag=Cyc_Core_Impossible,((_tmp9FC.f1=((_tmp9FB="anal_stmt -- bad stmt syntax or unimplemented stmt form",_tag_dyneither(_tmp9FB,sizeof(char),56))),_tmp9FC)))),_tmp9FA)))));}_LL33B:;};}
# 2821
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd);
# 2824
static union Cyc_CfFlowInfo_FlowInfo Cyc_NewControlFlow_anal_decl(struct Cyc_NewControlFlow_AnalEnv*env,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Decl*decl){
struct Cyc_CfFlowInfo_FlowEnv*_tmp6F2=env->fenv;
void*_tmp6F3=decl->r;void*_tmp6F4=_tmp6F3;struct Cyc_Absyn_Vardecl*_tmp6F6;struct Cyc_List_List*_tmp6F8;struct Cyc_Absyn_Fndecl*_tmp6FA;struct Cyc_Absyn_Tvar*_tmp6FC;struct Cyc_Absyn_Vardecl*_tmp6FD;int _tmp6FE;struct Cyc_Absyn_Exp*_tmp6FF;_LL390: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6F5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp6F4;if(_tmp6F5->tag != 0)goto _LL392;else{_tmp6F6=_tmp6F5->f1;}}_LL391: {
# 2834
struct Cyc_List_List _tmp9FD;struct Cyc_List_List _tmp700=(_tmp9FD.hd=_tmp6F6,((_tmp9FD.tl=0,_tmp9FD)));
inflow=Cyc_NewControlFlow_add_vars(_tmp6F2,inflow,& _tmp700,_tmp6F2->unknown_none,decl->loc,0);{
struct Cyc_Absyn_Exp*_tmp701=_tmp6F6->initializer;
if(_tmp701 == 0)
return inflow;{
struct _tuple16 _tmp702=Cyc_NewControlFlow_anal_Rexp(env,1,inflow,_tmp701);union Cyc_CfFlowInfo_FlowInfo _tmp704;void*_tmp705;struct _tuple16 _tmp703=_tmp702;_tmp704=_tmp703.f1;_tmp705=_tmp703.f2;
return Cyc_NewControlFlow_do_initialize_var(_tmp6F2,env,_tmp704,_tmp6F6,_tmp701,_tmp705,decl->loc);};};}_LL392: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp6F7=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp6F4;if(_tmp6F7->tag != 3)goto _LL394;else{_tmp6F8=_tmp6F7->f1;}}_LL393:
# 2843
 return Cyc_NewControlFlow_add_vars(_tmp6F2,inflow,_tmp6F8,_tmp6F2->unknown_none,decl->loc,0);_LL394: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp6F9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp6F4;if(_tmp6F9->tag != 1)goto _LL396;else{_tmp6FA=_tmp6F9->f1;}}_LL395:
# 2846
 Cyc_NewControlFlow_check_nested_fun(_tmp6F2,inflow,_tmp6FA);{
void*t=(void*)_check_null(_tmp6FA->cached_typ);
struct Cyc_Absyn_Vardecl*_tmp707=(struct Cyc_Absyn_Vardecl*)_check_null(_tmp6FA->fn_vardecl);
# 2852
struct Cyc_List_List*_tmp9FE;return Cyc_NewControlFlow_add_vars(_tmp6F2,inflow,((_tmp9FE=_region_malloc(env->r,sizeof(*_tmp9FE)),((_tmp9FE->hd=_tmp707,((_tmp9FE->tl=0,_tmp9FE)))))),_tmp6F2->unknown_all,decl->loc,0);};_LL396: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp6FB=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp6F4;if(_tmp6FB->tag != 4)goto _LL398;else{_tmp6FC=_tmp6FB->f1;_tmp6FD=_tmp6FB->f2;_tmp6FE=_tmp6FB->f3;_tmp6FF=_tmp6FB->f4;}}_LL397:
# 2855
 if(_tmp6FF != 0){
struct Cyc_Core_Impossible_exn_struct _tmpA04;const char*_tmpA03;struct Cyc_Core_Impossible_exn_struct*_tmpA02;(int)_throw((void*)((_tmpA02=_cycalloc(sizeof(*_tmpA02)),((_tmpA02[0]=((_tmpA04.tag=Cyc_Core_Impossible,((_tmpA04.f1=((_tmpA03="found open expression in declaration!",_tag_dyneither(_tmpA03,sizeof(char),38))),_tmpA04)))),_tmpA02)))));}{
struct Cyc_List_List _tmpA05;struct Cyc_List_List _tmp70C=(_tmpA05.hd=_tmp6FD,((_tmpA05.tl=0,_tmpA05)));
return Cyc_NewControlFlow_add_vars(_tmp6F2,inflow,& _tmp70C,_tmp6F2->unknown_all,decl->loc,0);};_LL398:;_LL399: {
# 2861
struct Cyc_Core_Impossible_exn_struct _tmpA0B;const char*_tmpA0A;struct Cyc_Core_Impossible_exn_struct*_tmpA09;(int)_throw((void*)((_tmpA09=_cycalloc(sizeof(*_tmpA09)),((_tmpA09[0]=((_tmpA0B.tag=Cyc_Core_Impossible,((_tmpA0B.f1=((_tmpA0A="anal_decl: unexpected decl variant",_tag_dyneither(_tmpA0A,sizeof(char),35))),_tmpA0B)))),_tmpA09)))));}_LL38F:;}
# 2869
static void Cyc_NewControlFlow_check_fun(struct Cyc_Absyn_Fndecl*fd){
struct _RegionHandle _tmp711=_new_region("frgn");struct _RegionHandle*frgn=& _tmp711;_push_region(frgn);
{struct Cyc_CfFlowInfo_FlowEnv*fenv=Cyc_CfFlowInfo_new_flow_env(frgn);
Cyc_NewControlFlow_check_nested_fun(fenv,Cyc_CfFlowInfo_ReachableFL(fenv->mt_flowdict,0),fd);}
# 2871
;_pop_region(frgn);}
# 2876
static int Cyc_NewControlFlow_hash_ptr(void*s){
return(int)s;}
# 2880
static void Cyc_NewControlFlow_check_nested_fun(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo inflow,struct Cyc_Absyn_Fndecl*fd){
# 2884
struct _RegionHandle*_tmp712=fenv->r;
unsigned int _tmp713=(fd->body)->loc;
inflow=Cyc_NewControlFlow_add_vars(fenv,inflow,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,fenv->unknown_all,_tmp713,1);{
# 2890
struct Cyc_List_List*param_roots=0;
struct Cyc_List_List*noconsume_roots=0;
{union Cyc_CfFlowInfo_FlowInfo _tmp714=inflow;struct Cyc_Dict_Dict _tmp715;struct Cyc_List_List*_tmp716;_LL39B: if((_tmp714.BottomFL).tag != 1)goto _LL39D;_LL39C: {
const char*_tmpA0E;void*_tmpA0D;(_tmpA0D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA0E="check_fun",_tag_dyneither(_tmpA0E,sizeof(char),10))),_tag_dyneither(_tmpA0D,sizeof(void*),0)));}_LL39D: if((_tmp714.ReachableFL).tag != 2)goto _LL39A;_tmp715=((struct _tuple14)(_tmp714.ReachableFL).val).f1;_tmp716=((struct _tuple14)(_tmp714.ReachableFL).val).f2;_LL39E: {
# 2897
struct Cyc_List_List*atts;
{void*_tmp719=Cyc_Tcutil_compress((void*)_check_null(fd->cached_typ));void*_tmp71A=_tmp719;struct Cyc_List_List*_tmp71C;_LL3A0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp71B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp71A;if(_tmp71B->tag != 9)goto _LL3A2;else{_tmp71C=(_tmp71B->f1).attributes;}}_LL3A1:
 atts=_tmp71C;goto _LL39F;_LL3A2:;_LL3A3: {
const char*_tmpA11;void*_tmpA10;(_tmpA10=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA11="check_fun: non-function type cached with fndecl_t",_tag_dyneither(_tmpA11,sizeof(char),50))),_tag_dyneither(_tmpA10,sizeof(void*),0)));}_LL39F:;}
# 2904
for(0;atts != 0;atts=atts->tl){
void*_tmp71F=(void*)atts->hd;void*_tmp720=_tmp71F;int _tmp722;int _tmp724;int _tmp726;_LL3A5: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp721=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp720;if(_tmp721->tag != 21)goto _LL3A7;else{_tmp722=_tmp721->f1;}}_LL3A6: {
# 2907
struct Cyc_Absyn_Exp*bogus_exp=Cyc_Absyn_signed_int_exp(- 1,0);
struct Cyc_Absyn_Vardecl*_tmp727=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp722 - 1);
void*t=Cyc_Tcutil_compress(_tmp727->type);
void*elttype=Cyc_Tcutil_pointer_elt_type(t);
void*_tmp728=
Cyc_CfFlowInfo_make_unique_consumed(fenv,elttype,bogus_exp,- 1,
Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->unknown_all));
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA14;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA13;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp729=(_tmpA13=_region_malloc(_tmp712,sizeof(*_tmpA13)),((_tmpA13[0]=((_tmpA14.tag=2,((_tmpA14.f1=_tmp722,((_tmpA14.f2=t,_tmpA14)))))),_tmpA13)));
struct Cyc_CfFlowInfo_Place*_tmpA15;struct Cyc_CfFlowInfo_Place*_tmp72A=(_tmpA15=_region_malloc(_tmp712,sizeof(*_tmpA15)),((_tmpA15->root=(void*)_tmp729,((_tmpA15->fields=0,_tmpA15)))));
_tmp715=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp715,(void*)_tmp729,_tmp728);
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA1B;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA1A;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA18;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA17;_tmp715=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp715,(void*)((_tmpA17=_region_malloc(_tmp712,sizeof(*_tmpA17)),((_tmpA17[0]=((_tmpA18.tag=0,((_tmpA18.f1=_tmp727,_tmpA18)))),_tmpA17)))),(void*)((_tmpA1A=_region_malloc(_tmp712,sizeof(*_tmpA1A)),((_tmpA1A[0]=((_tmpA1B.tag=5,((_tmpA1B.f1=_tmp72A,_tmpA1B)))),_tmpA1A)))));}
goto _LL3A4;}_LL3A7: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp723=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp720;if(_tmp723->tag != 20)goto _LL3A9;else{_tmp724=_tmp723->f1;}}_LL3A8: {
# 2920
struct Cyc_Absyn_Vardecl*_tmp732=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp724 - 1);
void*elttype=Cyc_Tcutil_pointer_elt_type(_tmp732->type);
struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct _tmpA1E;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmpA1D;struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*_tmp733=(_tmpA1D=_region_malloc(_tmp712,sizeof(*_tmpA1D)),((_tmpA1D[0]=((_tmpA1E.tag=2,((_tmpA1E.f1=_tmp724,((_tmpA1E.f2=elttype,_tmpA1E)))))),_tmpA1D)));
struct Cyc_CfFlowInfo_Place*_tmpA1F;struct Cyc_CfFlowInfo_Place*_tmp734=(_tmpA1F=_region_malloc(_tmp712,sizeof(*_tmpA1F)),((_tmpA1F->root=(void*)_tmp733,((_tmpA1F->fields=0,_tmpA1F)))));
_tmp715=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp715,(void*)_tmp733,Cyc_CfFlowInfo_typ_to_absrval(fenv,elttype,0,fenv->esc_none));
{struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct _tmpA25;struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*_tmpA24;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA22;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA21;_tmp715=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp715,(void*)((_tmpA21=_region_malloc(_tmp712,sizeof(*_tmpA21)),((_tmpA21[0]=((_tmpA22.tag=0,((_tmpA22.f1=_tmp732,_tmpA22)))),_tmpA21)))),(void*)((_tmpA24=_region_malloc(_tmp712,sizeof(*_tmpA24)),((_tmpA24[0]=((_tmpA25.tag=5,((_tmpA25.f1=_tmp734,_tmpA25)))),_tmpA24)))));}
{struct Cyc_List_List*_tmpA26;param_roots=((_tmpA26=_region_malloc(_tmp712,sizeof(*_tmpA26)),((_tmpA26->hd=_tmp734,((_tmpA26->tl=param_roots,_tmpA26))))));}
goto _LL3A4;}_LL3A9: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp725=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp720;if(_tmp725->tag != 22)goto _LL3AB;else{_tmp726=_tmp725->f1;}}_LL3AA: {
# 2929
struct Cyc_Absyn_Vardecl*_tmp73D=((struct Cyc_Absyn_Vardecl*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v,_tmp726 - 1);
if(Cyc_Tcutil_is_noalias_pointer(_tmp73D->type,0)){
struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct _tmpA29;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmpA28;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*_tmp73E=(_tmpA28=_region_malloc(_tmp712,sizeof(*_tmpA28)),((_tmpA28[0]=((_tmpA29.tag=0,((_tmpA29.f1=_tmp73D,_tmpA29)))),_tmpA28)));
struct Cyc_CfFlowInfo_Place*_tmpA2A;struct Cyc_CfFlowInfo_Place*_tmp73F=(_tmpA2A=_region_malloc(_tmp712,sizeof(*_tmpA2A)),((_tmpA2A->root=(void*)_tmp73E,((_tmpA2A->fields=0,_tmpA2A)))));
struct Cyc_List_List*_tmpA2B;noconsume_roots=((_tmpA2B=_region_malloc(_tmp712,sizeof(*_tmpA2B)),((_tmpA2B->hd=_tmp73F,((_tmpA2B->tl=noconsume_roots,_tmpA2B))))));}
# 2935
goto _LL3A4;}_LL3AB:;_LL3AC:
 goto _LL3A4;_LL3A4:;}
# 2938
inflow=Cyc_CfFlowInfo_ReachableFL(_tmp715,_tmp716);}_LL39A:;}{
# 2941
int noreturn=Cyc_Tcutil_is_noreturn(Cyc_Tcutil_fndecl2typ(fd));
struct Cyc_Hashtable_Table*flow_table=
((struct Cyc_Hashtable_Table*(*)(struct _RegionHandle*r,int sz,int(*cmp)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*),int(*hash)(struct Cyc_Absyn_Stmt*)))Cyc_Hashtable_rcreate)(_tmp712,33,(int(*)(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*))Cyc_Core_ptrcmp,(int(*)(struct Cyc_Absyn_Stmt*s))Cyc_NewControlFlow_hash_ptr);
struct Cyc_NewControlFlow_AnalEnv*_tmpA2C;struct Cyc_NewControlFlow_AnalEnv*env=(_tmpA2C=_region_malloc(_tmp712,sizeof(*_tmpA2C)),((_tmpA2C->r=_tmp712,((_tmpA2C->fenv=fenv,((_tmpA2C->iterate_again=1,((_tmpA2C->iteration_num=0,((_tmpA2C->in_try=0,((_tmpA2C->tryflow=inflow,((_tmpA2C->all_changed=0,((_tmpA2C->noreturn=noreturn,((_tmpA2C->return_type=fd->ret_type,((_tmpA2C->unique_pat_vars=0,((_tmpA2C->param_roots=param_roots,((_tmpA2C->noconsume_params=noconsume_roots,((_tmpA2C->flow_table=flow_table,_tmpA2C)))))))))))))))))))))))))));
union Cyc_CfFlowInfo_FlowInfo outflow=inflow;
while(env->iterate_again  && !Cyc_CfFlowInfo_anal_error){
++ env->iteration_num;
# 2951
env->iterate_again=0;
outflow=Cyc_NewControlFlow_anal_stmt(env,inflow,fd->body,0);}{
# 2954
union Cyc_CfFlowInfo_FlowInfo _tmp744=outflow;_LL3AE: if((_tmp744.BottomFL).tag != 1)goto _LL3B0;_LL3AF:
 goto _LL3AD;_LL3B0:;_LL3B1:
# 2957
 Cyc_NewControlFlow_check_init_params(_tmp713,env,outflow);
Cyc_NewControlFlow_unconsume_params(env,env->noconsume_params,0,0,outflow,_tmp713);
# 2961
if(noreturn){
const char*_tmpA2F;void*_tmpA2E;(_tmpA2E=0,Cyc_CfFlowInfo_aerr(_tmp713,((_tmpA2F="`noreturn' function might (implicitly) return",_tag_dyneither(_tmpA2F,sizeof(char),46))),_tag_dyneither(_tmpA2E,sizeof(void*),0)));}else{
# 2964
void*_tmp747=Cyc_Tcutil_compress(fd->ret_type);void*_tmp748=_tmp747;_LL3B3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 0)goto _LL3B5;}_LL3B4:
 goto _LL3B2;_LL3B5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp74A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp748;if(_tmp74A->tag != 7)goto _LL3B7;}_LL3B6:
 goto _LL3B8;_LL3B7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp74B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp748;if(_tmp74B->tag != 6)goto _LL3B9;}_LL3B8:
# 2968
{const char*_tmpA32;void*_tmpA31;(_tmpA31=0,Cyc_Tcutil_warn(_tmp713,((_tmpA32="function may not return a value",_tag_dyneither(_tmpA32,sizeof(char),32))),_tag_dyneither(_tmpA31,sizeof(void*),0)));}goto _LL3B2;_LL3B9:;_LL3BA:
# 2970
{const char*_tmpA35;void*_tmpA34;(_tmpA34=0,Cyc_CfFlowInfo_aerr(_tmp713,((_tmpA35="function may not return a value",_tag_dyneither(_tmpA35,sizeof(char),32))),_tag_dyneither(_tmpA34,sizeof(void*),0)));}goto _LL3B2;_LL3B2:;}
# 2972
goto _LL3AD;_LL3AD:;};};};}
# 2976
void Cyc_NewControlFlow_cf_check(struct Cyc_List_List*ds){
for(0;ds != 0;ds=ds->tl){
Cyc_CfFlowInfo_anal_error=0;{
void*_tmp751=((struct Cyc_Absyn_Decl*)ds->hd)->r;void*_tmp752=_tmp751;struct Cyc_Absyn_Fndecl*_tmp754;struct Cyc_List_List*_tmp756;struct Cyc_List_List*_tmp758;struct Cyc_List_List*_tmp75A;_LL3BC: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp753=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp752;if(_tmp753->tag != 1)goto _LL3BE;else{_tmp754=_tmp753->f1;}}_LL3BD:
 Cyc_NewControlFlow_check_fun(_tmp754);goto _LL3BB;_LL3BE: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp755=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp752;if(_tmp755->tag != 11)goto _LL3C0;else{_tmp756=_tmp755->f1;}}_LL3BF:
 _tmp758=_tmp756;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp757=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp752;if(_tmp757->tag != 10)goto _LL3C2;else{_tmp758=_tmp757->f2;}}_LL3C1:
 _tmp75A=_tmp758;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp759=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp752;if(_tmp759->tag != 9)goto _LL3C4;else{_tmp75A=_tmp759->f2;}}_LL3C3:
 Cyc_NewControlFlow_cf_check(_tmp75A);goto _LL3BB;_LL3C4: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp75B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp752;if(_tmp75B->tag != 12)goto _LL3C6;}_LL3C5:
 goto _LL3BB;_LL3C6:;_LL3C7:
 goto _LL3BB;_LL3BB:;};}}
