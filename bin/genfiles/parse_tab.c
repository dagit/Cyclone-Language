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
# 280 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 285
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 292
return 0;
return dyn.curr;};}extern char Cyc_Lexing_Error[6];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 57 "lexing.h"
typedef struct Cyc_Lexing_lexbuf*Cyc_Lexing_Lexbuf;struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};
# 64
typedef struct Cyc_Lexing_function_lexbuf_state*Cyc_Lexing_Function_lexbuf_state;struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 74
typedef struct Cyc_Lexing_lex_tables*Cyc_Lexing_LexTables;
# 80
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 107 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 55
struct Cyc_Position_Error*Cyc_Position_mk_err_parse(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 338
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 488
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 884
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 911
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 919
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 934
void*Cyc_Absyn_compress_kb(void*);
# 939
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 941
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 944
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 946
extern void*Cyc_Absyn_sint_typ;
# 948
void*Cyc_Absyn_float_typ(int);
# 996
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1094
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1098
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,unsigned int);
# 1103
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1105
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1112
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1121
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1126
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1132
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1155
int Cyc_Absyn_fntype_att(void*a);
# 1175
extern int Cyc_Absyn_porting_c_code;struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 42 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 117
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 122
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 129
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 309 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
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
# 71 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;};
# 91 "parse.y"
typedef struct Cyc_Parse_FlatList*Cyc_Parse_flat_list_t;
# 93
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 96
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 105
return first;}}
# 110
int Cyc_Parse_no_register=0;struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 123
typedef struct Cyc_Parse_Type_specifier Cyc_Parse_type_specifier_t;
# 125
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0,Cyc_Parse_Extern_sc  = 1,Cyc_Parse_ExternC_sc  = 2,Cyc_Parse_Static_sc  = 3,Cyc_Parse_Auto_sc  = 4,Cyc_Parse_Register_sc  = 5,Cyc_Parse_Abstract_sc  = 6};
# 128
typedef enum Cyc_Parse_Storage_class Cyc_Parse_storage_class_t;struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};
# 137
typedef struct Cyc_Parse_Declaration_spec Cyc_Parse_decl_spec_t;struct Cyc_Parse_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};
# 143
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 147
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 151
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp859;declarators=((_tmp859=_region_malloc(r,sizeof(*_tmp859)),((_tmp859->tl=declarators,((_tmp859->hd=_tmp2,_tmp859))))));}{
struct Cyc_List_List*_tmp85A;exprs=((_tmp85A=_region_malloc(r,sizeof(*_tmp85A)),((_tmp85A->hd=_tmp3,((_tmp85A->tl=exprs,_tmp85A))))));};}
# 158
*es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);
*decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 165
typedef struct Cyc_Parse_Abstractdeclarator Cyc_Parse_abstractdeclarator_t;
# 168
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 170
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 175
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 178
static void Cyc_Parse_err(struct _dyneither_ptr msg,unsigned int sg){
Cyc_Position_post_error(Cyc_Position_mk_err_parse(sg,msg));}
# 181
static void*Cyc_Parse_parse_abort(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 183
Cyc_Parse_err((struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap),sg);{
struct Cyc_Position_Exit_exn_struct _tmp85D;struct Cyc_Position_Exit_exn_struct*_tmp85C;(int)_throw((void*)((_tmp85C=_cycalloc_atomic(sizeof(*_tmp85C)),((_tmp85C[0]=((_tmp85D.tag=Cyc_Position_Exit,_tmp85D)),_tmp85C)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp862;void*_tmp861[2];struct Cyc_String_pa_PrintArg_struct _tmp860;struct Cyc_String_pa_PrintArg_struct _tmp85F;(_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp860.tag=0,((_tmp860.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp861[0]=& _tmp860,((_tmp861[1]=& _tmp85F,Cyc_fprintf(Cyc_stderr,((_tmp862="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp862,sizeof(char),46))),_tag_dyneither(_tmp861,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp865;void*_tmp864;(_tmp864=0,Cyc_Tcutil_warn(loc,((_tmp865="qualifier on type is ignored",_tag_dyneither(_tmp865,sizeof(char),29))),_tag_dyneither(_tmp864,sizeof(void*),0)));};}
# 199
return _tmpE;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 202
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 208
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmp11=(void*)pqs->hd;void*_tmp12=_tmp11;struct Cyc_Absyn_Exp*_tmp1A;void*_tmp1C;_LL1: {struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp13=(struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp13->tag != 4)goto _LL3;}_LL2:
# 212
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;_LL3: {struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmp14=(struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp14->tag != 5)goto _LL5;}_LL4:
# 214
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;_LL5: {struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmp15=(struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp15->tag != 7)goto _LL7;}_LL6:
# 216
 nullable=Cyc_Absyn_true_conref;goto _LL0;_LL7: {struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmp16=(struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp16->tag != 6)goto _LL9;}_LL8:
# 218
 nullable=Cyc_Absyn_false_conref;goto _LL0;_LL9: {struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmp17=(struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp17->tag != 3)goto _LLB;}_LLA:
# 220
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;_LLB: {struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmp18=(struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp18->tag != 2)goto _LLD;}_LLC:
# 222
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;_LLD: {struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmp19=(struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp19->tag != 0)goto _LLF;else{_tmp1A=_tmp19->f1;}}_LLE:
# 224
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp868;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp867;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp867=_cycalloc(sizeof(*_tmp867)),((_tmp867[0]=((_tmp868.tag=1,((_tmp868.f1=_tmp1A,_tmp868)))),_tmp867)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 226
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp869;return(_tmp869.f1=nullable,((_tmp869.f2=bound,((_tmp869.f3=zeroterm,((_tmp869.f4=rgn,_tmp869)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp876;const char*_tmp875;void*_tmp874[1];struct Cyc_Int_pa_PrintArg_struct _tmp873;struct _tuple0*_tmp872;return(_tmp872=_cycalloc(sizeof(*_tmp872)),((_tmp872->f1=Cyc_Absyn_Rel_n(0),((_tmp872->f2=(
(_tmp876=_cycalloc(sizeof(*_tmp876)),((_tmp876[0]=(struct _dyneither_ptr)((_tmp873.tag=1,((_tmp873.f1=(unsigned long)enum_counter ++,((_tmp874[0]=& _tmp873,Cyc_aprintf(((_tmp875="__anonymous_enum_%d__",_tag_dyneither(_tmp875,sizeof(char),22))),_tag_dyneither(_tmp874,sizeof(void*),1)))))))),_tmp876)))),_tmp872)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp877;Cyc_Parse_err(((_tmp877="bad type params in struct field",_tag_dyneither(_tmp877,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp878;Cyc_Parse_err(((_tmp878="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp878,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp879;return(_tmp879=_cycalloc(sizeof(*_tmp879)),((_tmp879->name=(*_tmp26).f2,((_tmp879->tq=_tmp27,((_tmp879->type=_tmp28,((_tmp879->width=_tmp2B,((_tmp879->attributes=_tmp2A,((_tmp879->requires_clause=_tmp2C,_tmp879)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp87A;return(_tmp87A.Signed_spec=0,((_tmp87A.Unsigned_spec=0,((_tmp87A.Short_spec=0,((_tmp87A.Long_spec=0,((_tmp87A.Long_Long_spec=0,((_tmp87A.Valid_type_spec=0,((_tmp87A.Type_spec=Cyc_Absyn_sint_typ,((_tmp87A.loc=loc,_tmp87A)))))))))))))));}
# 268
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp31=Cyc_Parse_empty_spec(loc);
_tmp31.Type_spec=t;
_tmp31.Valid_type_spec=1;
return _tmp31;}
# 274
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp32=Cyc_Parse_empty_spec(loc);
_tmp32.Signed_spec=1;
return _tmp32;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp33=Cyc_Parse_empty_spec(loc);
_tmp33.Unsigned_spec=1;
return _tmp33;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp34=Cyc_Parse_empty_spec(loc);
_tmp34.Short_spec=1;
return _tmp34;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp35=Cyc_Parse_empty_spec(loc);
_tmp35.Long_spec=1;
return _tmp35;}
# 296
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 298
return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 311 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp36=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp37=(struct _tuple8*)x->hd;struct _tuple8*_tmp38=_tmp37;struct _dyneither_ptr*_tmp39;void*_tmp3B;struct _dyneither_ptr _tmp3C;void**_tmp3F;_LL12: _tmp39=_tmp38->f1;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3A->tag != 19)goto _LL14;else{_tmp3B=(void*)_tmp3A->f1;}};if(!(_tmp39 != 0))goto _LL14;_LL13:
# 316
{void*_tmp40=_tmp3B;void**_tmp42;_LL19: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp41=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40;if(_tmp41->tag != 1)goto _LL1B;else{_tmp42=(void**)((void**)& _tmp41->f2);}}_LL1A: {
# 320
struct Cyc_String_pa_PrintArg_struct _tmp882;void*_tmp881[1];const char*_tmp880;struct _dyneither_ptr*_tmp87F;struct _dyneither_ptr*nm=(_tmp87F=_cycalloc(sizeof(*_tmp87F)),((_tmp87F[0]=(struct _dyneither_ptr)((_tmp882.tag=0,((_tmp882.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp881[0]=& _tmp882,Cyc_aprintf(((_tmp880="`%s",_tag_dyneither(_tmp880,sizeof(char),4))),_tag_dyneither(_tmp881,sizeof(void*),1)))))))),_tmp87F)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp891;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp890;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp88F;struct Cyc_Absyn_Tvar*_tmp88E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp88D;*_tmp42=(void*)((_tmp88D=_cycalloc(sizeof(*_tmp88D)),((_tmp88D[0]=((_tmp891.tag=2,((_tmp891.f1=((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E->name=nm,((_tmp88E->identity=- 1,((_tmp88E->kind=(void*)((_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=((_tmp88F.tag=0,((_tmp88F.f1=& Cyc_Tcutil_ik,_tmp88F)))),_tmp890)))),_tmp88E)))))))),_tmp891)))),_tmp88D))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp894;struct Cyc_List_List*_tmp893;_tmp36=((_tmp893=_cycalloc(sizeof(*_tmp893)),((_tmp893->hd=((_tmp894=_cycalloc(sizeof(*_tmp894)),((_tmp894->f1=_tmp39,((_tmp894->f2=_tmp3B,_tmp894)))))),((_tmp893->tl=_tmp36,_tmp893))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp89C;void*_tmp89B[1];const char*_tmp89A;struct _dyneither_ptr*_tmp899;struct _dyneither_ptr*nm=(_tmp899=_cycalloc(sizeof(*_tmp899)),((_tmp899[0]=(struct _dyneither_ptr)((_tmp89C.tag=0,((_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp89B[0]=& _tmp89C,Cyc_aprintf(((_tmp89A="`%s",_tag_dyneither(_tmp89A,sizeof(char),4))),_tag_dyneither(_tmp89B,sizeof(void*),1)))))))),_tmp899)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8AB;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8AA;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8A9;struct Cyc_Absyn_Tvar*_tmp8A8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8A7;*_tmp3F=(void*)((_tmp8A7=_cycalloc(sizeof(*_tmp8A7)),((_tmp8A7[0]=((_tmp8AB.tag=2,((_tmp8AB.f1=((_tmp8A8=_cycalloc(sizeof(*_tmp8A8)),((_tmp8A8->name=nm,((_tmp8A8->identity=- 1,((_tmp8A8->kind=(void*)((_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=((_tmp8A9.tag=0,((_tmp8A9.f1=& Cyc_Tcutil_rk,_tmp8A9)))),_tmp8AA)))),_tmp8A8)))))))),_tmp8AB)))),_tmp8A7))));}
goto _LL11;}_LL16:;_LL17:
 goto _LL11;_LL11:;}
# 335
return _tmp36;}
# 339
static struct Cyc_List_List*Cyc_Parse_get_argrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp57=0;
for(0;x != 0;x=x->tl){
void*_tmp58=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp59=_tmp58;void*_tmp5B;_LL1E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59;if(_tmp5A->tag != 19)goto _LL20;else{_tmp5B=(void*)_tmp5A->f1;}}_LL1F:
# 344
{struct _tuple18*_tmp8AE;struct Cyc_List_List*_tmp8AD;_tmp57=((_tmp8AD=_cycalloc(sizeof(*_tmp8AD)),((_tmp8AD->hd=((_tmp8AE=_cycalloc(sizeof(*_tmp8AE)),((_tmp8AE->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8AE->f2=_tmp5B,_tmp8AE)))))),((_tmp8AD->tl=_tmp57,_tmp8AD))))));}goto _LL1D;_LL20:;_LL21:
 goto _LL1D;_LL1D:;}
# 348
return _tmp57;}
# 352
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp5E=e->r;void*_tmp5F=_tmp5E;struct _dyneither_ptr*_tmp61;_LL23: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp60=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5F;if(_tmp60->tag != 1)goto _LL25;else{if((((_tmp60->f1)->f1).Rel_n).tag != 1)goto _LL25;if((struct Cyc_List_List*)(((_tmp60->f1)->f1).Rel_n).val != 0)goto _LL25;_tmp61=(_tmp60->f1)->f2;}}_LL24:
# 355
{struct Cyc_List_List*_tmp62=tags;for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct _tuple18*_tmp63=(struct _tuple18*)_tmp62->hd;struct _dyneither_ptr*_tmp65;void*_tmp66;struct _tuple18*_tmp64=_tmp63;_tmp65=_tmp64->f1;_tmp66=_tmp64->f2;
if(Cyc_strptrcmp(_tmp65,_tmp61)== 0){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8B1;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8B0;return Cyc_Absyn_new_exp((void*)((_tmp8B0=_cycalloc(sizeof(*_tmp8B0)),((_tmp8B0[0]=((_tmp8B1.tag=38,((_tmp8B1.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8B1)))),_tmp8B0)))),e->loc);}}}
# 360
goto _LL22;_LL25:;_LL26:
 goto _LL22;_LL22:;}
# 363
return e;}
# 368
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp69=t;void*_tmp6B;struct Cyc_Absyn_Tqual _tmp6C;struct Cyc_Absyn_Exp*_tmp6D;union Cyc_Absyn_Constraint*_tmp6E;unsigned int _tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;_LL28: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69;if(_tmp6A->tag != 8)goto _LL2A;else{_tmp6B=(_tmp6A->f1).elt_type;_tmp6C=(_tmp6A->f1).tq;_tmp6D=(_tmp6A->f1).num_elts;_tmp6E=(_tmp6A->f1).zero_term;_tmp6F=(_tmp6A->f1).zt_loc;}}_LL29: {
# 371
struct Cyc_Absyn_Exp*nelts2=_tmp6D;
if(_tmp6D != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp6D);{
# 375
void*_tmp78=Cyc_Parse_substitute_tags(tags,_tmp6B);
if(_tmp6D != nelts2  || _tmp6B != _tmp78){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8B7;struct Cyc_Absyn_ArrayInfo _tmp8B6;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B5;return(void*)((_tmp8B5=_cycalloc(sizeof(*_tmp8B5)),((_tmp8B5[0]=((_tmp8B7.tag=8,((_tmp8B7.f1=((_tmp8B6.elt_type=_tmp78,((_tmp8B6.tq=_tmp6C,((_tmp8B6.num_elts=nelts2,((_tmp8B6.zero_term=_tmp6E,((_tmp8B6.zt_loc=_tmp6F,_tmp8B6)))))))))),_tmp8B7)))),_tmp8B5))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 384
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8BD;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8BC;union Cyc_Absyn_Constraint*_tmp8BB;_tmp7D=((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),(((_tmp8BB->Eq_constr).val=(void*)((_tmp8BD=_cycalloc(sizeof(*_tmp8BD)),((_tmp8BD[0]=((_tmp8BC.tag=1,((_tmp8BC.f1=_tmp81,_tmp8BC)))),_tmp8BD)))),(((_tmp8BB->Eq_constr).tag=1,_tmp8BB))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8C3;struct Cyc_Absyn_PtrInfo _tmp8C2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C1;return(void*)((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C3.tag=5,((_tmp8C3.f1=((_tmp8C2.elt_typ=_tmp7C,((_tmp8C2.elt_tq=_tmp72,((_tmp8C2.ptr_atts=(((_tmp8C2.ptr_atts).rgn=_tmp73,(((_tmp8C2.ptr_atts).nullable=_tmp74,(((_tmp8C2.ptr_atts).bounds=_tmp7D,(((_tmp8C2.ptr_atts).zero_term=_tmp76,(((_tmp8C2.ptr_atts).ptrloc=_tmp77,_tmp8C2.ptr_atts)))))))))),_tmp8C2)))))),_tmp8C3)))),_tmp8C1))));}
goto _LL27;}_LL2C:;_LL2D:
# 394
 goto _LL27;_LL27:;}
# 396
return t;}
# 401
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
x->type=Cyc_Parse_substitute_tags(tags,x->type);}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 408
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 410
struct _tuple19*_tmp8C4;return(_tmp8C4=_cycalloc(sizeof(*_tmp8C4)),((_tmp8C4->f1=(*t).f2,((_tmp8C4->f2=(*t).f3,_tmp8C4)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8C7;void*_tmp8C6;(_tmp8C6=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8C7="bad declaration in `forarray' statement",_tag_dyneither(_tmp8C7,sizeof(char),40))),_tag_dyneither(_tmp8C6,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8C8;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C8="`H",_tag_dyneither(_tmp8C8,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8C9;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8C9="`U",_tag_dyneither(_tmp8C9,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8CA;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8CA="`RC",_tag_dyneither(_tmp8CA,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8D4;struct _dyneither_ptr*_tmp8D3;struct Cyc_Absyn_Tvar*_tmp8D2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8D1;return(void*)((_tmp8D1=_cycalloc(sizeof(*_tmp8D1)),((_tmp8D1[0]=((_tmp8D4.tag=2,((_tmp8D4.f1=((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2->name=((_tmp8D3=_cycalloc(sizeof(*_tmp8D3)),((_tmp8D3[0]=s,_tmp8D3)))),((_tmp8D2->identity=- 1,((_tmp8D2->kind=k,_tmp8D2)))))))),_tmp8D4)))),_tmp8D1))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8D7;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8D6;k=(void*)((_tmp8D6=_cycalloc(sizeof(*_tmp8D6)),((_tmp8D6[0]=((_tmp8D7.tag=1,((_tmp8D7.f1=0,_tmp8D7)))),_tmp8D6))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8DA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8D9;k=(void*)((_tmp8D9=_cycalloc(sizeof(*_tmp8D9)),((_tmp8D9[0]=((_tmp8DA.tag=2,((_tmp8DA.f1=0,((_tmp8DA.f2=_tmp9C,_tmp8DA)))))),_tmp8D9))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8DB;return(_tmp8DB=_cycalloc(sizeof(*_tmp8DB)),((_tmp8DB->name=t->name,((_tmp8DB->identity=- 1,((_tmp8DB->kind=k,_tmp8DB)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp8DE;void*_tmp8DD;(_tmp8DD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8DE="expecting a list of type variables, not types",_tag_dyneither(_tmp8DE,sizeof(char),46))),_tag_dyneither(_tmp8DD,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E1;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E0;return(void*)((_tmp8E0=_cycalloc(sizeof(*_tmp8E0)),((_tmp8E0[0]=((_tmp8E1.tag=2,((_tmp8E1.f1=pr,_tmp8E1)))),_tmp8E0))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 468
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8E4;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8E3;*_tmpAD=(void*)((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3[0]=((_tmp8E4.tag=2,((_tmp8E4.f1=0,((_tmp8E4.f2=k,_tmp8E4)))))),_tmp8E3))));}
return;_LL51:;_LL52:
 return;_LL4E:;}_LL4C:;_LL4D:
# 475
 return;_LL49:;}
# 480
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 486
if(tds == 0)return tms;
# 491
if(tms == 0)return 0;{
# 493
void*_tmpB3=(void*)tms->hd;void*_tmpB4=_tmpB3;void*_tmpB6;_LL54: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpB5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpB4;if(_tmpB5->tag != 3)goto _LL56;else{_tmpB6=(void*)_tmpB5->f1;}}_LL55:
# 496
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 499
void*_tmpB7=_tmpB6;struct Cyc_List_List*_tmpBA;_LL59: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpB8=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL5B;}_LL5A:
# 501
{const char*_tmp8E7;void*_tmp8E6;(_tmp8E6=0,Cyc_Tcutil_warn(loc,((_tmp8E7="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8E7,sizeof(char),93))),_tag_dyneither(_tmp8E6,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8EA;void*_tmp8E9;(_tmp8E9=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8EA="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8EA,sizeof(char),73))),_tag_dyneither(_tmp8E9,sizeof(void*),0)));}{
# 509
struct Cyc_List_List*rev_new_params=0;
for(0;_tmpBA != 0;_tmpBA=_tmpBA->tl){
struct Cyc_List_List*_tmpBF=tds;
for(0;_tmpBF != 0;_tmpBF=_tmpBF->tl){
struct Cyc_Absyn_Decl*_tmpC0=(struct Cyc_Absyn_Decl*)_tmpBF->hd;
void*_tmpC1=_tmpC0->r;void*_tmpC2=_tmpC1;struct Cyc_Absyn_Vardecl*_tmpC4;_LL5E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpC2;if(_tmpC3->tag != 0)goto _LL60;else{_tmpC4=_tmpC3->f1;}}_LL5F:
# 516
 if(Cyc_zstrptrcmp((*_tmpC4->name).f2,(struct _dyneither_ptr*)_tmpBA->hd)!= 0)
continue;
if(_tmpC4->initializer != 0){
const char*_tmp8ED;void*_tmp8EC;(_tmp8EC=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8ED="initializer found in parameter declaration",_tag_dyneither(_tmp8ED,sizeof(char),43))),_tag_dyneither(_tmp8EC,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp8F0;void*_tmp8EF;(_tmp8EF=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F0="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp8F0,sizeof(char),47))),_tag_dyneither(_tmp8EF,sizeof(void*),0)));}
{struct _tuple8*_tmp8F3;struct Cyc_List_List*_tmp8F2;rev_new_params=(
(_tmp8F2=_cycalloc(sizeof(*_tmp8F2)),((_tmp8F2->hd=((_tmp8F3=_cycalloc(sizeof(*_tmp8F3)),((_tmp8F3->f1=(*_tmpC4->name).f2,((_tmp8F3->f2=_tmpC4->tq,((_tmp8F3->f3=_tmpC4->type,_tmp8F3)))))))),((_tmp8F2->tl=rev_new_params,_tmp8F2))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp8F6;void*_tmp8F5;(_tmp8F5=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8F6="nonvariable declaration in parameter type",_tag_dyneither(_tmp8F6,sizeof(char),42))),_tag_dyneither(_tmp8F5,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpBF == 0){
const char*_tmp8FA;void*_tmp8F9[1];struct Cyc_String_pa_PrintArg_struct _tmp8F8;(_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp8F9[0]=& _tmp8F8,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8FA="%s is not given a type",_tag_dyneither(_tmp8FA,sizeof(char),23))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp909;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp908;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp907;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp906;struct Cyc_List_List*_tmp905;return
(_tmp905=_region_malloc(yy,sizeof(*_tmp905)),((_tmp905->hd=(void*)((_tmp909=_region_malloc(yy,sizeof(*_tmp909)),((_tmp909[0]=((_tmp906.tag=3,((_tmp906.f1=(void*)((_tmp908=_region_malloc(yy,sizeof(*_tmp908)),((_tmp908[0]=((_tmp907.tag=1,((_tmp907.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp907.f2=0,((_tmp907.f3=0,((_tmp907.f4=0,((_tmp907.f5=0,_tmp907)))))))))))),_tmp908)))),_tmp906)))),_tmp909)))),((_tmp905->tl=0,_tmp905)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp90A;return(_tmp90A=_region_malloc(yy,sizeof(*_tmp90A)),((_tmp90A->hd=(void*)tms->hd,((_tmp90A->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp90A)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp90B;d=((_tmp90B.id=d.id,((_tmp90B.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp90B))));}{
# 554
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 560
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 566
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmpD7=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));switch(_tmpD7){case Cyc_Parse_Extern_sc: _LL62:
 sc=Cyc_Absyn_Extern;break;case Cyc_Parse_Static_sc: _LL63:
 sc=Cyc_Absyn_Static;break;default: _LL64:
{const char*_tmp90C;Cyc_Parse_err(((_tmp90C="bad storage class on function",_tag_dyneither(_tmp90C,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 578
if(_tmpDE != 0){
# 581
const char*_tmp90F;void*_tmp90E;(_tmp90E=0,Cyc_Tcutil_warn(loc,((_tmp90F="bad type params, ignoring",_tag_dyneither(_tmp90F,sizeof(char),26))),_tag_dyneither(_tmp90E,sizeof(void*),0)));}{
# 583
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpED=0;
{struct Cyc_List_List*_tmpEE=_tmpE8;for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)_tmpEE->hd;struct _dyneither_ptr*_tmpF1;struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct _tuple8*_tmpF0=_tmpEF;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;
if(_tmpF1 == 0){
{const char*_tmp910;Cyc_Parse_err(((_tmp910="missing argument variable in function prototype",_tag_dyneither(_tmp910,sizeof(char),48))),loc);}{
struct _tuple8*_tmp913;struct Cyc_List_List*_tmp912;_tmpED=((_tmp912=_cycalloc(sizeof(*_tmp912)),((_tmp912->hd=((_tmp913=_cycalloc(sizeof(*_tmp913)),((_tmp913->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp913->f2=_tmpF2,((_tmp913->f3=_tmpF3,_tmp913)))))))),((_tmp912->tl=_tmpED,_tmp912))))));};}else{
# 593
struct _tuple8*_tmp916;struct Cyc_List_List*_tmp915;_tmpED=((_tmp915=_cycalloc(sizeof(*_tmp915)),((_tmp915->hd=((_tmp916=_cycalloc(sizeof(*_tmp916)),((_tmp916->f1=_tmpF1,((_tmp916->f2=_tmpF2,((_tmp916->f3=_tmpF3,_tmp916)))))))),((_tmp915->tl=_tmpED,_tmp915))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp917;return(_tmp917=_cycalloc(sizeof(*_tmp917)),((_tmp917->sc=sc,((_tmp917->name=d.id,((_tmp917->tvs=_tmpE4,((_tmp917->is_inline=is_inline,((_tmp917->effect=_tmpE5,((_tmp917->ret_tqual=_tmpE6,((_tmp917->ret_type=_tmpE7,((_tmp917->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpED),((_tmp917->c_varargs=_tmpE9,((_tmp917->cyc_varargs=_tmpEA,((_tmp917->rgn_po=_tmpEB,((_tmp917->body=body,((_tmp917->cached_typ=0,((_tmp917->param_vardecls=0,((_tmp917->fn_vardecl=0,((_tmp917->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),_tmp917)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp91A;void*_tmp919;(_tmp919=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp91A="declarator is not a function prototype",_tag_dyneither(_tmp91A,sizeof(char),39))),_tag_dyneither(_tmp919,sizeof(void*),0)));}_LL66:;};};};}static char _tmpFD[52]="at most one type may appear within a type specifier";
# 612
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpFD,_tmpFD,_tmpFD + 52};static char _tmpFE[63]="const or volatile may appear only once within a type specifier";
# 614
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpFE,_tmpFE,_tmpFE + 63};static char _tmpFF[50]="type specifier includes more than one declaration";
# 616
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpFF,_tmpFF,_tmpFF + 50};static char _tmp100[60]="sign specifier may appear only once within a type specifier";
# 618
static struct _dyneither_ptr Cyc_Parse_msg4={_tmp100,_tmp100,_tmp100 + 60};
# 625
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 628
if(s1.Signed_spec  && s2.Signed_spec){
void*_tmp91B;(_tmp91B=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91B,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp91C;(_tmp91C=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91C,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp91D;(_tmp91D=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91D,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp91E;(_tmp91E=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp91E,sizeof(void*),0)));}
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
Cyc_Parse_err(Cyc_Parse_msg1,loc);else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 650
return s1;}
# 656
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 659
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 666
if(ts.Signed_spec  && ts.Unsigned_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
Cyc_Parse_err(Cyc_Parse_msg4,loc);
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 678
if(!seen_type){
if(!seen_sign  && !seen_size){
const char*_tmp921;void*_tmp920;(_tmp920=0,Cyc_Tcutil_warn(loc,((_tmp921="missing type within specifier",_tag_dyneither(_tmp921,sizeof(char),30))),_tag_dyneither(_tmp920,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmp107=t;enum Cyc_Absyn_Sign _tmp109;enum Cyc_Absyn_Size_of _tmp10A;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp107;if(_tmp108->tag != 6)goto _LL6E;else{_tmp109=_tmp108->f1;_tmp10A=_tmp108->f2;}}_LL6D:
# 686
 if(_tmp109 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10A);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp922;Cyc_Parse_err(((_tmp922="sign specification on non-integral type",_tag_dyneither(_tmp922,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmp10C=t;enum Cyc_Absyn_Sign _tmp10E;enum Cyc_Absyn_Size_of _tmp10F;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10C;if(_tmp10D->tag != 6)goto _LL73;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;}}_LL72:
# 694
 if(_tmp10F != sz)
t=Cyc_Absyn_int_typ(_tmp10E,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp110=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp10C;if(_tmp110->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp923;Cyc_Parse_err(((_tmp923="size qualifier on non-integral type",_tag_dyneither(_tmp923,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
# 702
return t;}
# 705
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 709
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmp112=d.id;
struct _tuple13 _tmp113=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct Cyc_Absyn_Tqual _tmp115;void*_tmp116;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct _tuple13 _tmp114=_tmp113;_tmp115=_tmp114.f1;_tmp116=_tmp114.f2;_tmp117=_tmp114.f3;_tmp118=_tmp114.f4;
# 715
if(ds->tl == 0){
struct _tuple15*_tmp926;struct Cyc_List_List*_tmp925;return(_tmp925=_region_malloc(r,sizeof(*_tmp925)),((_tmp925->hd=((_tmp926=_region_malloc(r,sizeof(*_tmp926)),((_tmp926->f1=_tmp112,((_tmp926->f2=_tmp115,((_tmp926->f3=_tmp116,((_tmp926->f4=_tmp117,((_tmp926->f5=_tmp118,_tmp926)))))))))))),((_tmp925->tl=0,_tmp925)))));}else{
# 718
struct _tuple15*_tmp929;struct Cyc_List_List*_tmp928;return(_tmp928=_region_malloc(r,sizeof(*_tmp928)),((_tmp928->hd=((_tmp929=_region_malloc(r,sizeof(*_tmp929)),((_tmp929->f1=_tmp112,((_tmp929->f2=_tmp115,((_tmp929->f3=_tmp116,((_tmp929->f4=_tmp117,((_tmp929->f5=_tmp118,_tmp929)))))))))))),((_tmp928->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp928)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp92A;return(_tmp92A.f1=tq,((_tmp92A.f2=t,((_tmp92A.f3=0,((_tmp92A.f4=atts,_tmp92A)))))));}{
void*_tmp11E=(void*)tms->hd;void*_tmp11F=_tmp11E;union Cyc_Absyn_Constraint*_tmp121;unsigned int _tmp122;struct Cyc_Absyn_Exp*_tmp124;union Cyc_Absyn_Constraint*_tmp125;unsigned int _tmp126;void*_tmp128;struct Cyc_List_List*_tmp12A;unsigned int _tmp12B;struct Cyc_Absyn_PtrAtts _tmp12D;struct Cyc_Absyn_Tqual _tmp12E;unsigned int _tmp130;struct Cyc_List_List*_tmp131;_LL78: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp120=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LL7A;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL79:
# 728
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,0,_tmp121,_tmp122),atts,tms->tl);_LL7A: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp123=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp123->tag != 1)goto _LL7C;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;_tmp126=_tmp123->f3;}}_LL7B:
# 731
 return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(0),
Cyc_Absyn_array_typ(t,tq,_tmp124,_tmp125,_tmp126),atts,tms->tl);_LL7C: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp127=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp127->tag != 3)goto _LL7E;else{_tmp128=(void*)_tmp127->f1;}}_LL7D: {
# 734
void*_tmp132=_tmp128;struct Cyc_List_List*_tmp134;int _tmp135;struct Cyc_Absyn_VarargInfo*_tmp136;void*_tmp137;struct Cyc_List_List*_tmp138;unsigned int _tmp13A;_LL85: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp133=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp132;if(_tmp133->tag != 1)goto _LL87;else{_tmp134=_tmp133->f1;_tmp135=_tmp133->f2;_tmp136=_tmp133->f3;_tmp137=(void*)_tmp133->f4;_tmp138=_tmp133->f5;}}_LL86: {
# 736
struct Cyc_List_List*typvars=0;
# 738
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd)){
struct Cyc_List_List*_tmp92B;fn_atts=((_tmp92B=_cycalloc(sizeof(*_tmp92B)),((_tmp92B->hd=(void*)as->hd,((_tmp92B->tl=fn_atts,_tmp92B))))));}else{
# 743
struct Cyc_List_List*_tmp92C;new_atts=((_tmp92C=_cycalloc(sizeof(*_tmp92C)),((_tmp92C->hd=(void*)as->hd,((_tmp92C->tl=new_atts,_tmp92C))))));}}}
# 746
if(tms->tl != 0){
void*_tmp13D=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp13E=_tmp13D;struct Cyc_List_List*_tmp140;_LL8A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp13F=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp13E;if(_tmp13F->tag != 4)goto _LL8C;else{_tmp140=_tmp13F->f1;}}_LL8B:
# 749
 typvars=_tmp140;
tms=tms->tl;
goto _LL89;_LL8C:;_LL8D:
# 753
 goto _LL89;_LL89:;}
# 757
if(((((!_tmp135  && _tmp136 == 0) && _tmp134 != 0) && _tmp134->tl == 0) && (*((struct _tuple8*)_tmp134->hd)).f1 == 0) && (*((struct _tuple8*)_tmp134->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 762
_tmp134=0;{
# 765
struct Cyc_List_List*_tmp141=Cyc_Parse_get_arg_tags(_tmp134);
# 767
if(_tmp141 != 0)
t=Cyc_Parse_substitute_tags(_tmp141,t);
t=Cyc_Parse_array2ptr(t,0);
# 772
{struct Cyc_List_List*_tmp142=_tmp134;for(0;_tmp142 != 0;_tmp142=_tmp142->tl){
struct _tuple8*_tmp143=(struct _tuple8*)_tmp142->hd;struct _dyneither_ptr*_tmp145;struct Cyc_Absyn_Tqual _tmp146;void**_tmp147;struct _tuple8*_tmp144=_tmp143;_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;_tmp147=(void**)& _tmp144->f3;
if(_tmp141 != 0)
*_tmp147=Cyc_Parse_substitute_tags(_tmp141,*_tmp147);
*_tmp147=Cyc_Parse_array2ptr(*_tmp147,1);}}
# 784
return Cyc_Parse_apply_tms(Cyc_Absyn_empty_tqual(tq.loc),
Cyc_Absyn_function_typ(typvars,_tmp137,tq,t,_tmp134,_tmp135,_tmp136,_tmp138,fn_atts),new_atts,((struct Cyc_List_List*)_check_null(tms))->tl);};}_LL87: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp139=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp132;if(_tmp139->tag != 0)goto _LL84;else{_tmp13A=_tmp139->f2;}}_LL88: {
# 790
const char*_tmp92F;void*_tmp92E;(_tmp92E=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp13A,((_tmp92F="function declaration without parameter types",_tag_dyneither(_tmp92F,sizeof(char),45))),_tag_dyneither(_tmp92E,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp129=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp129->tag != 4)goto _LL80;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp930;return(_tmp930.f1=tq,((_tmp930.f2=t,((_tmp930.f3=_tmp12A,((_tmp930.f4=atts,_tmp930)))))));}{
# 802
const char*_tmp933;void*_tmp932;(_tmp932=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12B,((_tmp933="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp933,sizeof(char),68))),_tag_dyneither(_tmp932,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp12C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12C->tag != 2)goto _LL82;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp939;struct Cyc_Absyn_PtrInfo _tmp938;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp937;return Cyc_Parse_apply_tms(_tmp12E,(void*)((_tmp937=_cycalloc(sizeof(*_tmp937)),((_tmp937[0]=((_tmp939.tag=5,((_tmp939.f1=((_tmp938.elt_typ=t,((_tmp938.elt_tq=tq,((_tmp938.ptr_atts=_tmp12D,_tmp938)))))),_tmp939)))),_tmp937)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp12F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12F->tag != 5)goto _LL77;else{_tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;}}_LL83:
# 810
 return Cyc_Parse_apply_tms(tq,t,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp131),tms->tl);_LL77:;};}
# 816
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 825
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple0*_tmp151;struct Cyc_Absyn_Tqual _tmp152;void*_tmp153;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp155;struct _tuple15*_tmp150=t;_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;_tmp153=_tmp150->f3;_tmp154=_tmp150->f4;_tmp155=_tmp150->f5;
# 828
Cyc_Lex_register_typedef(_tmp151);{
# 830
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp156=_tmp153;struct Cyc_Core_Opt*_tmp158;_LL8F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 1)goto _LL91;else{_tmp158=_tmp157->f1;}}_LL90:
# 834
 type=0;
if(_tmp158 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp158;}
goto _LL8E;_LL91:;_LL92:
 kind=0;type=_tmp153;goto _LL8E;_LL8E:;}{
# 840
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp93F;struct Cyc_Absyn_Typedefdecl*_tmp93E;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp93D;return Cyc_Absyn_new_decl((void*)((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D[0]=((_tmp93F.tag=8,((_tmp93F.f1=((_tmp93E=_cycalloc(sizeof(*_tmp93E)),((_tmp93E->name=_tmp151,((_tmp93E->tvs=_tmp154,((_tmp93E->kind=kind,((_tmp93E->defn=type,((_tmp93E->atts=_tmp155,((_tmp93E->tq=_tmp152,_tmp93E)))))))))))))),_tmp93F)))),_tmp93D)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp942;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp941;return Cyc_Absyn_new_stmt((void*)((_tmp941=_cycalloc(sizeof(*_tmp941)),((_tmp941[0]=((_tmp942.tag=12,((_tmp942.f1=d,((_tmp942.f2=s,_tmp942)))))),_tmp941)))),d->loc);}
# 853
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 862
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 866
struct _RegionHandle _tmp15E=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp15E;_push_region(mkrgn);
{struct Cyc_Absyn_Tqual _tmp160;struct Cyc_Parse_Type_specifier _tmp161;struct Cyc_List_List*_tmp162;struct Cyc_Parse_Declaration_spec _tmp15F=ds;_tmp160=_tmp15F.tq;_tmp161=_tmp15F.type_specs;_tmp162=_tmp15F.attributes;
if(_tmp160.loc == 0)_tmp160.loc=tqual_loc;
if(ds.is_inline){
const char*_tmp943;Cyc_Parse_err(((_tmp943="inline is allowed only on function definitions",_tag_dyneither(_tmp943,sizeof(char),47))),loc);}{
# 872
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp164=*ds.sc;switch(_tmp164){case Cyc_Parse_Typedef_sc: _LL93:
 istypedef=1;break;case Cyc_Parse_Extern_sc: _LL94:
 s=Cyc_Absyn_Extern;break;case Cyc_Parse_ExternC_sc: _LL95:
 s=Cyc_Absyn_ExternC;break;case Cyc_Parse_Static_sc: _LL96:
 s=Cyc_Absyn_Static;break;case Cyc_Parse_Auto_sc: _LL97:
 s=Cyc_Absyn_Public;break;case Cyc_Parse_Register_sc: _LL98:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}break;case Cyc_Parse_Abstract_sc: _LL99:
 s=Cyc_Absyn_Abstract;break;}}{
# 888
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 892
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 900
void*_tmp165=Cyc_Parse_collapse_type_specifiers(_tmp161,loc);
if(declarators == 0){
# 904
void*_tmp166=_tmp165;struct Cyc_Absyn_Aggrdecl*_tmp169;struct Cyc_Absyn_Enumdecl*_tmp16C;struct Cyc_Absyn_Datatypedecl*_tmp16F;enum Cyc_Absyn_AggrKind _tmp171;struct _tuple0*_tmp172;struct Cyc_List_List*_tmp173;struct Cyc_Absyn_Datatypedecl**_tmp175;struct _tuple0*_tmp177;int _tmp178;struct Cyc_List_List*_tmp179;struct _tuple0*_tmp17B;struct Cyc_List_List*_tmp17D;_LL9C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp167=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp167->tag != 26)goto _LL9E;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp168=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp167->f1)->r;if(_tmp168->tag != 0)goto _LL9E;else{_tmp169=_tmp168->f1;}}}_LL9D:
# 906
 _tmp169->attributes=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp169->attributes,_tmp162);
_tmp169->sc=s;{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp949;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp948;struct Cyc_List_List*_tmp947;struct Cyc_List_List*_tmp181=(_tmp947=_cycalloc(sizeof(*_tmp947)),((_tmp947->hd=Cyc_Absyn_new_decl((void*)((_tmp949=_cycalloc(sizeof(*_tmp949)),((_tmp949[0]=((_tmp948.tag=5,((_tmp948.f1=_tmp169,_tmp948)))),_tmp949)))),loc),((_tmp947->tl=0,_tmp947)))));_npop_handler(0);return _tmp181;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16A->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp16B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp16A->f1)->r;if(_tmp16B->tag != 1)goto _LLA0;else{_tmp16C=_tmp16B->f1;}}}_LL9F:
# 910
 if(_tmp162 != 0){const char*_tmp94A;Cyc_Parse_err(((_tmp94A="attributes on enum not supported",_tag_dyneither(_tmp94A,sizeof(char),33))),loc);}
_tmp16C->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp950;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp94F;struct Cyc_List_List*_tmp94E;struct Cyc_List_List*_tmp186=(_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E->hd=Cyc_Absyn_new_decl((void*)((_tmp950=_cycalloc(sizeof(*_tmp950)),((_tmp950[0]=((_tmp94F.tag=7,((_tmp94F.f1=_tmp16C,_tmp94F)))),_tmp950)))),loc),((_tmp94E->tl=0,_tmp94E)))));_npop_handler(0);return _tmp186;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16D->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 2)goto _LLA2;else{_tmp16F=_tmp16E->f1;}}}_LLA1:
# 914
 if(_tmp162 != 0){const char*_tmp951;Cyc_Parse_err(((_tmp951="attributes on datatypes not supported",_tag_dyneither(_tmp951,sizeof(char),38))),loc);}
_tmp16F->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp957;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp956;struct Cyc_List_List*_tmp955;struct Cyc_List_List*_tmp18B=(_tmp955=_cycalloc(sizeof(*_tmp955)),((_tmp955->hd=Cyc_Absyn_new_decl((void*)((_tmp957=_cycalloc(sizeof(*_tmp957)),((_tmp957[0]=((_tmp956.tag=6,((_tmp956.f1=_tmp16F,_tmp956)))),_tmp957)))),loc),((_tmp955->tl=0,_tmp955)))));_npop_handler(0);return _tmp18B;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166;if(_tmp170->tag != 11)goto _LLA4;else{if((((_tmp170->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp171=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f1;_tmp172=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f2;_tmp173=(_tmp170->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp173);
struct Cyc_Absyn_Aggrdecl*_tmp958;struct Cyc_Absyn_Aggrdecl*_tmp18D=(_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->kind=_tmp171,((_tmp958->sc=s,((_tmp958->name=_tmp172,((_tmp958->tvs=_tmp18C,((_tmp958->impl=0,((_tmp958->attributes=0,_tmp958)))))))))))));
if(_tmp162 != 0){const char*_tmp959;Cyc_Parse_err(((_tmp959="bad attributes on type declaration",_tag_dyneither(_tmp959,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp95F;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp95E;struct Cyc_List_List*_tmp95D;struct Cyc_List_List*_tmp192=(_tmp95D=_cycalloc(sizeof(*_tmp95D)),((_tmp95D->hd=Cyc_Absyn_new_decl((void*)((_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F[0]=((_tmp95E.tag=5,((_tmp95E.f1=_tmp18D,_tmp95E)))),_tmp95F)))),loc),((_tmp95D->tl=0,_tmp95D)))));_npop_handler(0);return _tmp192;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp174->tag != 3)goto _LLA6;else{if((((_tmp174->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp175=(struct Cyc_Absyn_Datatypedecl**)(((_tmp174->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp162 != 0){const char*_tmp960;Cyc_Parse_err(((_tmp960="bad attributes on datatype",_tag_dyneither(_tmp960,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp966;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp965;struct Cyc_List_List*_tmp964;struct Cyc_List_List*_tmp198=(_tmp964=_cycalloc(sizeof(*_tmp964)),((_tmp964->hd=Cyc_Absyn_new_decl((void*)((_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966[0]=((_tmp965.tag=6,((_tmp965.f1=*_tmp175,_tmp965)))),_tmp966)))),loc),((_tmp964->tl=0,_tmp964)))));_npop_handler(0);return _tmp198;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp176->tag != 3)goto _LLA8;else{if((((_tmp176->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp177=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).name;_tmp178=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp179=(_tmp176->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Decl*_tmp19A=Cyc_Absyn_datatype_decl(s,_tmp177,_tmp199,0,_tmp178,loc);
if(_tmp162 != 0){const char*_tmp967;Cyc_Parse_err(((_tmp967="bad attributes on datatype",_tag_dyneither(_tmp967,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp968;struct Cyc_List_List*_tmp19D=(_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968->hd=_tmp19A,((_tmp968->tl=0,_tmp968)))));_npop_handler(0);return _tmp19D;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp166;if(_tmp17A->tag != 13)goto _LLAA;else{_tmp17B=_tmp17A->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp969;struct Cyc_Absyn_Enumdecl*_tmp19E=(_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->sc=s,((_tmp969->name=_tmp17B,((_tmp969->fields=0,_tmp969)))))));
if(_tmp162 != 0){const char*_tmp96A;Cyc_Parse_err(((_tmp96A="bad attributes on enum",_tag_dyneither(_tmp96A,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp974;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp973;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp972;struct Cyc_List_List*_tmp971;struct Cyc_List_List*_tmp1A4=(_tmp971=_cycalloc(sizeof(*_tmp971)),((_tmp971->hd=((_tmp974=_cycalloc(sizeof(*_tmp974)),((_tmp974->r=(void*)((_tmp972=_cycalloc(sizeof(*_tmp972)),((_tmp972[0]=((_tmp973.tag=7,((_tmp973.f1=_tmp19E,_tmp973)))),_tmp972)))),((_tmp974->loc=loc,_tmp974)))))),((_tmp971->tl=0,_tmp971)))));_npop_handler(0);return _tmp1A4;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp166;if(_tmp17C->tag != 14)goto _LLAC;else{_tmp17D=_tmp17C->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp977;struct Cyc_Absyn_Enumdecl*_tmp976;struct Cyc_Absyn_Enumdecl*_tmp1A6=(_tmp976=_cycalloc(sizeof(*_tmp976)),((_tmp976->sc=s,((_tmp976->name=Cyc_Parse_gensym_enum(),((_tmp976->fields=((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977->v=_tmp17D,_tmp977)))),_tmp976)))))));
if(_tmp162 != 0){const char*_tmp978;Cyc_Parse_err(((_tmp978="bad attributes on enum",_tag_dyneither(_tmp978,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp982;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp981;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp980;struct Cyc_List_List*_tmp97F;struct Cyc_List_List*_tmp1AC=(_tmp97F=_cycalloc(sizeof(*_tmp97F)),((_tmp97F->hd=((_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->r=(void*)((_tmp980=_cycalloc(sizeof(*_tmp980)),((_tmp980[0]=((_tmp981.tag=7,((_tmp981.f1=_tmp1A6,_tmp981)))),_tmp980)))),((_tmp982->loc=loc,_tmp982)))))),((_tmp97F->tl=0,_tmp97F)))));_npop_handler(0);return _tmp1AC;};}_LLAC:;_LLAD:
{const char*_tmp983;Cyc_Parse_err(((_tmp983="missing declarator",_tag_dyneither(_tmp983,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B0=0;_npop_handler(0);return _tmp1B0;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp1B1=Cyc_Parse_apply_tmss(mkrgn,_tmp160,_tmp165,declarators,_tmp162);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp984;Cyc_Parse_err(((_tmp984="initializer in typedef declaration",_tag_dyneither(_tmp984,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B1);
struct Cyc_List_List*_tmp1B3=decls;_npop_handler(0);return _tmp1B3;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1B4=_tmp1B1;for(0;_tmp1B4 != 0;(_tmp1B4=_tmp1B4->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1B5=(struct _tuple15*)_tmp1B4->hd;struct _tuple0*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct _tuple15*_tmp1B6=_tmp1B5;_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;_tmp1B9=_tmp1B6->f3;_tmp1BA=_tmp1B6->f4;_tmp1BB=_tmp1B6->f5;
if(_tmp1BA != 0){
const char*_tmp987;void*_tmp986;(_tmp986=0,Cyc_Tcutil_warn(loc,((_tmp987="bad type params, ignoring",_tag_dyneither(_tmp987,sizeof(char),26))),_tag_dyneither(_tmp986,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp98A;void*_tmp989;(_tmp989=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp98A="unexpected NULL in parse!",_tag_dyneither(_tmp98A,sizeof(char),26))),_tag_dyneither(_tmp989,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C1=Cyc_Absyn_new_vardecl(_tmp1B7,_tmp1B9,_tmp1C0);
_tmp1C1->tq=_tmp1B8;
_tmp1C1->sc=s;
_tmp1C1->attributes=_tmp1BB;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp990;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp98F;struct Cyc_Absyn_Decl*_tmp98E;struct Cyc_Absyn_Decl*_tmp1C2=(_tmp98E=_cycalloc(sizeof(*_tmp98E)),((_tmp98E->r=(void*)((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990[0]=((_tmp98F.tag=0,((_tmp98F.f1=_tmp1C1,_tmp98F)))),_tmp990)))),((_tmp98E->loc=loc,_tmp98E)))));
struct Cyc_List_List*_tmp991;decls=((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991->hd=_tmp1C2,((_tmp991->tl=decls,_tmp991))))));};};}}{
# 970
struct Cyc_List_List*_tmp1C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp1C7;};}}};};};};}
# 867
;_pop_region(mkrgn);}
# 977
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp1C8=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));switch(_tmp1C8){case 'A': _LLAE:
 return& Cyc_Tcutil_ak;case 'M': _LLAF:
 return& Cyc_Tcutil_mk;case 'B': _LLB0:
 return& Cyc_Tcutil_bk;case 'R': _LLB1:
 return& Cyc_Tcutil_rk;case 'E': _LLB2:
 return& Cyc_Tcutil_ek;case 'I': _LLB3:
 return& Cyc_Tcutil_ik;case 'U': _LLB4:
# 987
{char _tmp1C9=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1C9){case 'R': _LLB6:
 return& Cyc_Tcutil_urk;case 'A': _LLB7:
 return& Cyc_Tcutil_uak;case 'M': _LLB8:
 return& Cyc_Tcutil_umk;case 'B': _LLB9:
 return& Cyc_Tcutil_ubk;default: _LLBA:
 break;}}
# 994
break;case 'T': _LLB5:
# 996
{char _tmp1CA=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));switch(_tmp1CA){case 'R': _LLBD:
 return& Cyc_Tcutil_trk;case 'A': _LLBE:
 return& Cyc_Tcutil_tak;case 'M': _LLBF:
 return& Cyc_Tcutil_tmk;case 'B': _LLC0:
 return& Cyc_Tcutil_tbk;default: _LLC1:
 break;}}
# 1003
break;default: _LLBC:
 break;}}
# 1006
{const char*_tmp996;void*_tmp995[2];struct Cyc_String_pa_PrintArg_struct _tmp994;struct Cyc_Int_pa_PrintArg_struct _tmp993;Cyc_Parse_err((struct _dyneither_ptr)((_tmp993.tag=1,((_tmp993.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp994.tag=0,((_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp995[0]=& _tmp994,((_tmp995[1]=& _tmp993,Cyc_aprintf(((_tmp996="bad kind: %s; strlen=%d",_tag_dyneither(_tmp996,sizeof(char),24))),_tag_dyneither(_tmp995,sizeof(void*),2)))))))))))))),loc);}
return& Cyc_Tcutil_bk;}
# 1011
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp1CF=p->r;void*_tmp1D0=_tmp1CF;struct _tuple0*_tmp1D2;struct Cyc_Absyn_Vardecl*_tmp1D4;struct Cyc_Absyn_Pat*_tmp1D7;enum Cyc_Absyn_Sign _tmp1DA;int _tmp1DB;char _tmp1DD;struct _dyneither_ptr _tmp1DF;int _tmp1E0;struct _tuple0*_tmp1E2;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E5;_LLC5: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp1D1=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D1->tag != 15)goto _LLC7;else{_tmp1D2=_tmp1D1->f1;}}_LLC6:
 return Cyc_Absyn_unknownid_exp(_tmp1D2,p->loc);_LLC7: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1D3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D3->tag != 3)goto _LLC9;else{_tmp1D4=_tmp1D3->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp1D5=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp1D3->f2)->r;if(_tmp1D5->tag != 0)goto _LLC9;};}}_LLC8:
# 1015
 return Cyc_Absyn_deref_exp(Cyc_Absyn_unknownid_exp(_tmp1D4->name,p->loc),p->loc);_LLC9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1D6=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D6->tag != 6)goto _LLCB;else{_tmp1D7=_tmp1D6->f1;}}_LLCA:
 return Cyc_Absyn_address_exp(Cyc_Parse_pat2exp(_tmp1D7),p->loc);_LLCB: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp1D8=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D8->tag != 9)goto _LLCD;}_LLCC:
 return Cyc_Absyn_null_exp(p->loc);_LLCD: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1D9->tag != 10)goto _LLCF;else{_tmp1DA=_tmp1D9->f1;_tmp1DB=_tmp1D9->f2;}}_LLCE:
 return Cyc_Absyn_int_exp(_tmp1DA,_tmp1DB,p->loc);_LLCF: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp1DC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DC->tag != 11)goto _LLD1;else{_tmp1DD=_tmp1DC->f1;}}_LLD0:
 return Cyc_Absyn_char_exp(_tmp1DD,p->loc);_LLD1: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp1DE=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1DE->tag != 12)goto _LLD3;else{_tmp1DF=_tmp1DE->f1;_tmp1E0=_tmp1DE->f2;}}_LLD2:
 return Cyc_Absyn_float_exp(_tmp1DF,_tmp1E0,p->loc);_LLD3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp1E1=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E1->tag != 16)goto _LLD5;else{_tmp1E2=_tmp1E1->f1;_tmp1E3=_tmp1E1->f2;if(_tmp1E1->f3 != 0)goto _LLD5;}}_LLD4: {
# 1022
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1E2,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1E3);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}_LLD5: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp1E4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D0;if(_tmp1E4->tag != 17)goto _LLD7;else{_tmp1E5=_tmp1E4->f1;}}_LLD6:
 return _tmp1E5;_LLD7:;_LLD8:
# 1027
{const char*_tmp997;Cyc_Parse_err(((_tmp997="cannot mix patterns and expressions in case",_tag_dyneither(_tmp997,sizeof(char),44))),p->loc);}
return Cyc_Absyn_null_exp(p->loc);_LLC4:;}struct _union_YYSTYPE_Int_tok{int tag;struct _tuple5 val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY6{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY8{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY10{int tag;struct Cyc_Absyn_Stmt*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY12{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY13{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY14{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY15{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY18{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY19{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY20{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY21{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY22{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY23{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY25{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY31{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY32{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY34{int tag;int val;};struct _union_YYSTYPE_YY35{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY38{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY40{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY42{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY54{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1E8[14]="$(sign_t,int)";
# 1080 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1E8,_tmp1E8,_tmp1E8 + 14}};
# 1082
struct _tuple5 Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
struct _tuple5 yyzzz;
{union Cyc_YYSTYPE _tmp1E9=yy1;struct _tuple5 _tmp1EA;_LLDA: if((_tmp1E9.Int_tok).tag != 1)goto _LLDC;_tmp1EA=(struct _tuple5)(_tmp1E9.Int_tok).val;_LLDB:
# 1086
 yyzzz=_tmp1EA;
goto _LLD9;_LLDC:;_LLDD:
(int)_throw((void*)& Cyc_yyfail_Int_tok);_LLD9:;}
# 1090
return yyzzz;}
# 1092
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp998;return((_tmp998.Int_tok).val=yy1,(((_tmp998.Int_tok).tag=1,_tmp998)));}static char _tmp1ED[5]="char";
# 1081 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 5}};
# 1083
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;char _tmp1EF;_LLDF: if((_tmp1EE.Char_tok).tag != 2)goto _LLE1;_tmp1EF=(char)(_tmp1EE.Char_tok).val;_LLE0:
# 1087
 yyzzz=_tmp1EF;
goto _LLDE;_LLE1:;_LLE2:
(int)_throw((void*)& Cyc_yyfail_Char_tok);_LLDE:;}
# 1091
return yyzzz;}
# 1093
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp999;return((_tmp999.Char_tok).val=yy1,(((_tmp999.Char_tok).tag=2,_tmp999)));}static char _tmp1F2[13]="string_t<`H>";
# 1082 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 13}};
# 1084
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct _dyneither_ptr _tmp1F4;_LLE4: if((_tmp1F3.String_tok).tag != 3)goto _LLE6;_tmp1F4=(struct _dyneither_ptr)(_tmp1F3.String_tok).val;_LLE5:
# 1088
 yyzzz=_tmp1F4;
goto _LLE3;_LLE6:;_LLE7:
(int)_throw((void*)& Cyc_yyfail_String_tok);_LLE3:;}
# 1092
return yyzzz;}
# 1094
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp99A;return((_tmp99A.String_tok).val=yy1,(((_tmp99A.String_tok).tag=3,_tmp99A)));}static char _tmp1F7[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1085 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1F7,_tmp1F7,_tmp1F7 + 56}};
# 1087
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1F8=yy1;struct _tuple21*_tmp1F9;_LLE9: if((_tmp1F8.YY1).tag != 7)goto _LLEB;_tmp1F9=(struct _tuple21*)(_tmp1F8.YY1).val;_LLEA:
# 1091
 yyzzz=_tmp1F9;
goto _LLE8;_LLEB:;_LLEC:
(int)_throw((void*)& Cyc_yyfail_YY1);_LLE8:;}
# 1095
return yyzzz;}
# 1097
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp99B;return((_tmp99B.YY1).val=yy1,(((_tmp99B.YY1).tag=7,_tmp99B)));}static char _tmp1FC[19]="conref_t<bounds_t>";
# 1086 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 19}};
# 1088
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;union Cyc_Absyn_Constraint*_tmp1FE;_LLEE: if((_tmp1FD.YY2).tag != 8)goto _LLF0;_tmp1FE=(union Cyc_Absyn_Constraint*)(_tmp1FD.YY2).val;_LLEF:
# 1092
 yyzzz=_tmp1FE;
goto _LLED;_LLF0:;_LLF1:
(int)_throw((void*)& Cyc_yyfail_YY2);_LLED:;}
# 1096
return yyzzz;}
# 1098
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp99C;return((_tmp99C.YY2).val=yy1,(((_tmp99C.YY2).tag=8,_tmp99C)));}static char _tmp201[28]="list_t<offsetof_field_t,`H>";
# 1087 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 28}};
# 1089
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;_LLF3: if((_tmp202.YY3).tag != 9)goto _LLF5;_tmp203=(struct Cyc_List_List*)(_tmp202.YY3).val;_LLF4:
# 1093
 yyzzz=_tmp203;
goto _LLF2;_LLF5:;_LLF6:
(int)_throw((void*)& Cyc_yyfail_YY3);_LLF2:;}
# 1097
return yyzzz;}
# 1099
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp99D;return((_tmp99D.YY3).val=yy1,(((_tmp99D.YY3).tag=9,_tmp99D)));}static char _tmp206[6]="exp_t";
# 1088 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp206,_tmp206,_tmp206 + 6}};
# 1090
struct Cyc_Absyn_Exp*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp207=yy1;struct Cyc_Absyn_Exp*_tmp208;_LLF8: if((_tmp207.YY4).tag != 10)goto _LLFA;_tmp208=(struct Cyc_Absyn_Exp*)(_tmp207.YY4).val;_LLF9:
# 1094
 yyzzz=_tmp208;
goto _LLF7;_LLFA:;_LLFB:
(int)_throw((void*)& Cyc_yyfail_YY4);_LLF7:;}
# 1098
return yyzzz;}
# 1100
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp99E;return((_tmp99E.YY4).val=yy1,(((_tmp99E.YY4).tag=10,_tmp99E)));}static char _tmp20B[10]="exp_opt_t";
# 1096 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 10}};
# 1098
struct Cyc_Absyn_Exp*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Absyn_Exp*_tmp20D;_LLFD: if((_tmp20C.YY5).tag != 11)goto _LLFF;_tmp20D=(struct Cyc_Absyn_Exp*)(_tmp20C.YY5).val;_LLFE:
# 1102
 yyzzz=_tmp20D;
goto _LLFC;_LLFF:;_LL100:
(int)_throw((void*)& Cyc_yyfail_YY5);_LLFC:;}
# 1106
return yyzzz;}
# 1108
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp99F;return((_tmp99F.YY5).val=yy1,(((_tmp99F.YY5).tag=11,_tmp99F)));}static char _tmp210[17]="list_t<exp_t,`H>";
# 1097 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 17}};
# 1099
struct Cyc_List_List*Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct Cyc_List_List*_tmp212;_LL102: if((_tmp211.YY6).tag != 12)goto _LL104;_tmp212=(struct Cyc_List_List*)(_tmp211.YY6).val;_LL103:
# 1103
 yyzzz=_tmp212;
goto _LL101;_LL104:;_LL105:
(int)_throw((void*)& Cyc_yyfail_YY6);_LL101:;}
# 1107
return yyzzz;}
# 1109
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A0;return((_tmp9A0.YY6).val=yy1,(((_tmp9A0.YY6).tag=12,_tmp9A0)));}static char _tmp215[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1098 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp215,_tmp215,_tmp215 + 47}};
# 1100
struct Cyc_List_List*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp216=yy1;struct Cyc_List_List*_tmp217;_LL107: if((_tmp216.YY7).tag != 13)goto _LL109;_tmp217=(struct Cyc_List_List*)(_tmp216.YY7).val;_LL108:
# 1104
 yyzzz=_tmp217;
goto _LL106;_LL109:;_LL10A:
(int)_throw((void*)& Cyc_yyfail_YY7);_LL106:;}
# 1108
return yyzzz;}
# 1110
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A1;return((_tmp9A1.YY7).val=yy1,(((_tmp9A1.YY7).tag=13,_tmp9A1)));}static char _tmp21A[9]="primop_t";
# 1099 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 9}};
# 1101
enum Cyc_Absyn_Primop Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;enum Cyc_Absyn_Primop _tmp21C;_LL10C: if((_tmp21B.YY8).tag != 14)goto _LL10E;_tmp21C=(enum Cyc_Absyn_Primop)(_tmp21B.YY8).val;_LL10D:
# 1105
 yyzzz=_tmp21C;
goto _LL10B;_LL10E:;_LL10F:
(int)_throw((void*)& Cyc_yyfail_YY8);_LL10B:;}
# 1109
return yyzzz;}
# 1111
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9A2;return((_tmp9A2.YY8).val=yy1,(((_tmp9A2.YY8).tag=14,_tmp9A2)));}static char _tmp21F[19]="opt_t<primop_t,`H>";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 19}};
# 1102
struct Cyc_Core_Opt*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_Core_Opt*_tmp221;_LL111: if((_tmp220.YY9).tag != 15)goto _LL113;_tmp221=(struct Cyc_Core_Opt*)(_tmp220.YY9).val;_LL112:
# 1106
 yyzzz=_tmp221;
goto _LL110;_LL113:;_LL114:
(int)_throw((void*)& Cyc_yyfail_YY9);_LL110:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9A3;return((_tmp9A3.YY9).val=yy1,(((_tmp9A3.YY9).tag=15,_tmp9A3)));}static char _tmp224[7]="qvar_t";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp224,_tmp224,_tmp224 + 7}};
# 1103
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp225=yy1;struct _tuple0*_tmp226;_LL116: if((_tmp225.QualId_tok).tag != 5)goto _LL118;_tmp226=(struct _tuple0*)(_tmp225.QualId_tok).val;_LL117:
# 1107
 yyzzz=_tmp226;
goto _LL115;_LL118:;_LL119:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);_LL115:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9A4;return((_tmp9A4.QualId_tok).val=yy1,(((_tmp9A4.QualId_tok).tag=5,_tmp9A4)));}static char _tmp229[7]="stmt_t";
# 1104 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 7}};
# 1106
struct Cyc_Absyn_Stmt*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_Absyn_Stmt*_tmp22B;_LL11B: if((_tmp22A.YY10).tag != 16)goto _LL11D;_tmp22B=(struct Cyc_Absyn_Stmt*)(_tmp22A.YY10).val;_LL11C:
# 1110
 yyzzz=_tmp22B;
goto _LL11A;_LL11D:;_LL11E:
(int)_throw((void*)& Cyc_yyfail_YY10);_LL11A:;}
# 1114
return yyzzz;}
# 1116
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9A5;return((_tmp9A5.YY10).val=yy1,(((_tmp9A5.YY10).tag=16,_tmp9A5)));}static char _tmp22E[27]="list_t<switch_clause_t,`H>";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 27}};
# 1110
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct Cyc_List_List*_tmp230;_LL120: if((_tmp22F.YY11).tag != 17)goto _LL122;_tmp230=(struct Cyc_List_List*)(_tmp22F.YY11).val;_LL121:
# 1114
 yyzzz=_tmp230;
goto _LL11F;_LL122:;_LL123:
(int)_throw((void*)& Cyc_yyfail_YY11);_LL11F:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A6;return((_tmp9A6.YY11).val=yy1,(((_tmp9A6.YY11).tag=17,_tmp9A6)));}static char _tmp233[6]="pat_t";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp233,_tmp233,_tmp233 + 6}};
# 1111
struct Cyc_Absyn_Pat*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp234=yy1;struct Cyc_Absyn_Pat*_tmp235;_LL125: if((_tmp234.YY12).tag != 18)goto _LL127;_tmp235=(struct Cyc_Absyn_Pat*)(_tmp234.YY12).val;_LL126:
# 1115
 yyzzz=_tmp235;
goto _LL124;_LL127:;_LL128:
(int)_throw((void*)& Cyc_yyfail_YY12);_LL124:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9A7;return((_tmp9A7.YY12).val=yy1,(((_tmp9A7.YY12).tag=18,_tmp9A7)));}static char _tmp238[28]="$(list_t<pat_t,`H>,bool)@`H";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 28}};
# 1116
struct _tuple22*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct _tuple22*_tmp23A;_LL12A: if((_tmp239.YY13).tag != 19)goto _LL12C;_tmp23A=(struct _tuple22*)(_tmp239.YY13).val;_LL12B:
# 1120
 yyzzz=_tmp23A;
goto _LL129;_LL12C:;_LL12D:
(int)_throw((void*)& Cyc_yyfail_YY13);_LL129:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9A8;return((_tmp9A8.YY13).val=yy1,(((_tmp9A8.YY13).tag=19,_tmp9A8)));}static char _tmp23D[17]="list_t<pat_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 17}};
# 1117
struct Cyc_List_List*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_List_List*_tmp23F;_LL12F: if((_tmp23E.YY14).tag != 20)goto _LL131;_tmp23F=(struct Cyc_List_List*)(_tmp23E.YY14).val;_LL130:
# 1121
 yyzzz=_tmp23F;
goto _LL12E;_LL131:;_LL132:
(int)_throw((void*)& Cyc_yyfail_YY14);_LL12E:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.YY14).val=yy1,(((_tmp9A9.YY14).tag=20,_tmp9A9)));}static char _tmp242[36]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp242,_tmp242,_tmp242 + 36}};
# 1118
struct _tuple23*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp243=yy1;struct _tuple23*_tmp244;_LL134: if((_tmp243.YY15).tag != 21)goto _LL136;_tmp244=(struct _tuple23*)(_tmp243.YY15).val;_LL135:
# 1122
 yyzzz=_tmp244;
goto _LL133;_LL136:;_LL137:
(int)_throw((void*)& Cyc_yyfail_YY15);_LL133:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.YY15).val=yy1,(((_tmp9AA.YY15).tag=21,_tmp9AA)));}static char _tmp247[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 47}};
# 1119
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*_tmp249;_LL139: if((_tmp248.YY16).tag != 22)goto _LL13B;_tmp249=(struct Cyc_List_List*)(_tmp248.YY16).val;_LL13A:
# 1123
 yyzzz=_tmp249;
goto _LL138;_LL13B:;_LL13C:
(int)_throw((void*)& Cyc_yyfail_YY16);_LL138:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.YY16).val=yy1,(((_tmp9AB.YY16).tag=22,_tmp9AB)));}static char _tmp24C[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 58}};
# 1120
struct _tuple22*Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;struct _tuple22*_tmp24E;_LL13E: if((_tmp24D.YY17).tag != 23)goto _LL140;_tmp24E=(struct _tuple22*)(_tmp24D.YY17).val;_LL13F:
# 1124
 yyzzz=_tmp24E;
goto _LL13D;_LL140:;_LL141:
(int)_throw((void*)& Cyc_yyfail_YY17);_LL13D:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY17).val=yy1,(((_tmp9AC.YY17).tag=23,_tmp9AC)));}static char _tmp251[9]="fndecl_t";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp251,_tmp251,_tmp251 + 9}};
# 1121
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp252=yy1;struct Cyc_Absyn_Fndecl*_tmp253;_LL143: if((_tmp252.YY18).tag != 24)goto _LL145;_tmp253=(struct Cyc_Absyn_Fndecl*)(_tmp252.YY18).val;_LL144:
# 1125
 yyzzz=_tmp253;
goto _LL142;_LL145:;_LL146:
(int)_throw((void*)& Cyc_yyfail_YY18);_LL142:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY18).val=yy1,(((_tmp9AD.YY18).tag=24,_tmp9AD)));}static char _tmp256[18]="list_t<decl_t,`H>";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 18}};
# 1122
struct Cyc_List_List*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct Cyc_List_List*_tmp258;_LL148: if((_tmp257.YY19).tag != 25)goto _LL14A;_tmp258=(struct Cyc_List_List*)(_tmp257.YY19).val;_LL149:
# 1126
 yyzzz=_tmp258;
goto _LL147;_LL14A:;_LL14B:
(int)_throw((void*)& Cyc_yyfail_YY19);_LL147:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY19).val=yy1,(((_tmp9AE.YY19).tag=25,_tmp9AE)));}static char _tmp25B[12]="decl_spec_t";
# 1122 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 12}};
# 1124
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_Parse_Declaration_spec _tmp25D;_LL14D: if((_tmp25C.YY20).tag != 26)goto _LL14F;_tmp25D=(struct Cyc_Parse_Declaration_spec)(_tmp25C.YY20).val;_LL14E:
# 1128
 yyzzz=_tmp25D;
goto _LL14C;_LL14F:;_LL150:
(int)_throw((void*)& Cyc_yyfail_YY20);_LL14C:;}
# 1132
return yyzzz;}
# 1134
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY20).val=yy1,(((_tmp9AF.YY20).tag=26,_tmp9AF)));}static char _tmp260[31]="$(declarator_t<`yy>,exp_opt_t)";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp260,_tmp260,_tmp260 + 31}};
# 1125
struct _tuple11 Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp261=yy1;struct _tuple11 _tmp262;_LL152: if((_tmp261.YY21).tag != 27)goto _LL154;_tmp262=(struct _tuple11)(_tmp261.YY21).val;_LL153:
# 1129
 yyzzz=_tmp262;
goto _LL151;_LL154:;_LL155:
(int)_throw((void*)& Cyc_yyfail_YY21);_LL151:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY21).val=yy1,(((_tmp9B0.YY21).tag=27,_tmp9B0)));}static char _tmp265[23]="declarator_list_t<`yy>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp265,_tmp265,_tmp265 + 23}};
# 1126
struct _tuple12*Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp266=yy1;struct _tuple12*_tmp267;_LL157: if((_tmp266.YY22).tag != 28)goto _LL159;_tmp267=(struct _tuple12*)(_tmp266.YY22).val;_LL158:
# 1130
 yyzzz=_tmp267;
goto _LL156;_LL159:;_LL15A:
(int)_throw((void*)& Cyc_yyfail_YY22);_LL156:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY22).val=yy1,(((_tmp9B1.YY22).tag=28,_tmp9B1)));}static char _tmp26A[19]="storage_class_t@`H";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp26A,_tmp26A,_tmp26A + 19}};
# 1127
enum Cyc_Parse_Storage_class*Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp26B=yy1;enum Cyc_Parse_Storage_class*_tmp26C;_LL15C: if((_tmp26B.YY23).tag != 29)goto _LL15E;_tmp26C=(enum Cyc_Parse_Storage_class*)(_tmp26B.YY23).val;_LL15D:
# 1131
 yyzzz=_tmp26C;
goto _LL15B;_LL15E:;_LL15F:
(int)_throw((void*)& Cyc_yyfail_YY23);_LL15B:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY23).val=yy1,(((_tmp9B2.YY23).tag=29,_tmp9B2)));}static char _tmp26F[17]="type_specifier_t";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp26F,_tmp26F,_tmp26F + 17}};
# 1128
struct Cyc_Parse_Type_specifier Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp270=yy1;struct Cyc_Parse_Type_specifier _tmp271;_LL161: if((_tmp270.YY24).tag != 30)goto _LL163;_tmp271=(struct Cyc_Parse_Type_specifier)(_tmp270.YY24).val;_LL162:
# 1132
 yyzzz=_tmp271;
goto _LL160;_LL163:;_LL164:
(int)_throw((void*)& Cyc_yyfail_YY24);_LL160:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY24).val=yy1,(((_tmp9B3.YY24).tag=30,_tmp9B3)));}static char _tmp274[12]="aggr_kind_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp274,_tmp274,_tmp274 + 12}};
# 1130
enum Cyc_Absyn_AggrKind Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp275=yy1;enum Cyc_Absyn_AggrKind _tmp276;_LL166: if((_tmp275.YY25).tag != 31)goto _LL168;_tmp276=(enum Cyc_Absyn_AggrKind)(_tmp275.YY25).val;_LL167:
# 1134
 yyzzz=_tmp276;
goto _LL165;_LL168:;_LL169:
(int)_throw((void*)& Cyc_yyfail_YY25);_LL165:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY25).val=yy1,(((_tmp9B4.YY25).tag=31,_tmp9B4)));}static char _tmp279[8]="tqual_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp279,_tmp279,_tmp279 + 8}};
# 1131
struct Cyc_Absyn_Tqual Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp27A=yy1;struct Cyc_Absyn_Tqual _tmp27B;_LL16B: if((_tmp27A.YY26).tag != 32)goto _LL16D;_tmp27B=(struct Cyc_Absyn_Tqual)(_tmp27A.YY26).val;_LL16C:
# 1135
 yyzzz=_tmp27B;
goto _LL16A;_LL16D:;_LL16E:
(int)_throw((void*)& Cyc_yyfail_YY26);_LL16A:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.YY26).val=yy1,(((_tmp9B5.YY26).tag=32,_tmp9B5)));}static char _tmp27E[23]="list_t<aggrfield_t,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp27E,_tmp27E,_tmp27E + 23}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp27F=yy1;struct Cyc_List_List*_tmp280;_LL170: if((_tmp27F.YY27).tag != 33)goto _LL172;_tmp280=(struct Cyc_List_List*)(_tmp27F.YY27).val;_LL171:
# 1136
 yyzzz=_tmp280;
goto _LL16F;_LL172:;_LL173:
(int)_throw((void*)& Cyc_yyfail_YY27);_LL16F:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY27).val=yy1,(((_tmp9B6.YY27).tag=33,_tmp9B6)));}static char _tmp283[34]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp283,_tmp283,_tmp283 + 34}};
# 1133
struct Cyc_List_List*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp284=yy1;struct Cyc_List_List*_tmp285;_LL175: if((_tmp284.YY28).tag != 34)goto _LL177;_tmp285=(struct Cyc_List_List*)(_tmp284.YY28).val;_LL176:
# 1137
 yyzzz=_tmp285;
goto _LL174;_LL177:;_LL178:
(int)_throw((void*)& Cyc_yyfail_YY28);_LL174:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY28).val=yy1,(((_tmp9B7.YY28).tag=34,_tmp9B7)));}static char _tmp288[33]="list_t<type_modifier_t<`yy>,`yy>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp288,_tmp288,_tmp288 + 33}};
# 1134
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp289=yy1;struct Cyc_List_List*_tmp28A;_LL17A: if((_tmp289.YY29).tag != 35)goto _LL17C;_tmp28A=(struct Cyc_List_List*)(_tmp289.YY29).val;_LL17B:
# 1138
 yyzzz=_tmp28A;
goto _LL179;_LL17C:;_LL17D:
(int)_throw((void*)& Cyc_yyfail_YY29);_LL179:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY29).val=yy1,(((_tmp9B8.YY29).tag=35,_tmp9B8)));}static char _tmp28D[18]="declarator_t<`yy>";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp28D,_tmp28D,_tmp28D + 18}};
# 1135
struct Cyc_Parse_Declarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp28E=yy1;struct Cyc_Parse_Declarator _tmp28F;_LL17F: if((_tmp28E.YY30).tag != 36)goto _LL181;_tmp28F=(struct Cyc_Parse_Declarator)(_tmp28E.YY30).val;_LL180:
# 1139
 yyzzz=_tmp28F;
goto _LL17E;_LL181:;_LL182:
(int)_throw((void*)& Cyc_yyfail_YY30);_LL17E:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY30).val=yy1,(((_tmp9B9.YY30).tag=36,_tmp9B9)));}static char _tmp292[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp292,_tmp292,_tmp292 + 45}};
# 1136
struct _tuple24*Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp293=yy1;struct _tuple24*_tmp294;_LL184: if((_tmp293.YY31).tag != 37)goto _LL186;_tmp294=(struct _tuple24*)(_tmp293.YY31).val;_LL185:
# 1140
 yyzzz=_tmp294;
goto _LL183;_LL186:;_LL187:
(int)_throw((void*)& Cyc_yyfail_YY31);_LL183:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY31).val=yy1,(((_tmp9BA.YY31).tag=37,_tmp9BA)));}static char _tmp297[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp297,_tmp297,_tmp297 + 57}};
# 1137
struct Cyc_List_List*Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp298=yy1;struct Cyc_List_List*_tmp299;_LL189: if((_tmp298.YY32).tag != 38)goto _LL18B;_tmp299=(struct Cyc_List_List*)(_tmp298.YY32).val;_LL18A:
# 1141
 yyzzz=_tmp299;
goto _LL188;_LL18B:;_LL18C:
(int)_throw((void*)& Cyc_yyfail_YY32);_LL188:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY32).val=yy1,(((_tmp9BB.YY32).tag=38,_tmp9BB)));}static char _tmp29C[26]="abstractdeclarator_t<`yy>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp29C,_tmp29C,_tmp29C + 26}};
# 1138
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp29D=yy1;struct Cyc_Parse_Abstractdeclarator _tmp29E;_LL18E: if((_tmp29D.YY33).tag != 39)goto _LL190;_tmp29E=(struct Cyc_Parse_Abstractdeclarator)(_tmp29D.YY33).val;_LL18F:
# 1142
 yyzzz=_tmp29E;
goto _LL18D;_LL190:;_LL191:
(int)_throw((void*)& Cyc_yyfail_YY33);_LL18D:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY33).val=yy1,(((_tmp9BC.YY33).tag=39,_tmp9BC)));}static char _tmp2A1[5]="bool";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp2A1,_tmp2A1,_tmp2A1 + 5}};
# 1139
int Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp2A2=yy1;int _tmp2A3;_LL193: if((_tmp2A2.YY34).tag != 40)goto _LL195;_tmp2A3=(int)(_tmp2A2.YY34).val;_LL194:
# 1143
 yyzzz=_tmp2A3;
goto _LL192;_LL195:;_LL196:
(int)_throw((void*)& Cyc_yyfail_YY34);_LL192:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY34).val=yy1,(((_tmp9BD.YY34).tag=40,_tmp9BD)));}static char _tmp2A6[8]="scope_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp2A6,_tmp2A6,_tmp2A6 + 8}};
# 1140
enum Cyc_Absyn_Scope Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp2A7=yy1;enum Cyc_Absyn_Scope _tmp2A8;_LL198: if((_tmp2A7.YY35).tag != 41)goto _LL19A;_tmp2A8=(enum Cyc_Absyn_Scope)(_tmp2A7.YY35).val;_LL199:
# 1144
 yyzzz=_tmp2A8;
goto _LL197;_LL19A:;_LL19B:
(int)_throw((void*)& Cyc_yyfail_YY35);_LL197:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY35).val=yy1,(((_tmp9BE.YY35).tag=41,_tmp9BE)));}static char _tmp2AB[16]="datatypefield_t";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp2AB,_tmp2AB,_tmp2AB + 16}};
# 1141
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp2AC=yy1;struct Cyc_Absyn_Datatypefield*_tmp2AD;_LL19D: if((_tmp2AC.YY36).tag != 42)goto _LL19F;_tmp2AD=(struct Cyc_Absyn_Datatypefield*)(_tmp2AC.YY36).val;_LL19E:
# 1145
 yyzzz=_tmp2AD;
goto _LL19C;_LL19F:;_LL1A0:
(int)_throw((void*)& Cyc_yyfail_YY36);_LL19C:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY36).val=yy1,(((_tmp9BF.YY36).tag=42,_tmp9BF)));}static char _tmp2B0[27]="list_t<datatypefield_t,`H>";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp2B0,_tmp2B0,_tmp2B0 + 27}};
# 1142
struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2B1=yy1;struct Cyc_List_List*_tmp2B2;_LL1A2: if((_tmp2B1.YY37).tag != 43)goto _LL1A4;_tmp2B2=(struct Cyc_List_List*)(_tmp2B1.YY37).val;_LL1A3:
# 1146
 yyzzz=_tmp2B2;
goto _LL1A1;_LL1A4:;_LL1A5:
(int)_throw((void*)& Cyc_yyfail_YY37);_LL1A1:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY37).val=yy1,(((_tmp9C0.YY37).tag=43,_tmp9C0)));}static char _tmp2B5[41]="$(tqual_t,type_specifier_t,attributes_t)";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp2B5,_tmp2B5,_tmp2B5 + 41}};
# 1143
struct _tuple25 Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp2B6=yy1;struct _tuple25 _tmp2B7;_LL1A7: if((_tmp2B6.YY38).tag != 44)goto _LL1A9;_tmp2B7=(struct _tuple25)(_tmp2B6.YY38).val;_LL1A8:
# 1147
 yyzzz=_tmp2B7;
goto _LL1A6;_LL1A9:;_LL1AA:
(int)_throw((void*)& Cyc_yyfail_YY38);_LL1A6:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY38).val=yy1,(((_tmp9C1.YY38).tag=44,_tmp9C1)));}static char _tmp2BA[17]="list_t<var_t,`H>";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp2BA,_tmp2BA,_tmp2BA + 17}};
# 1144
struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2BB=yy1;struct Cyc_List_List*_tmp2BC;_LL1AC: if((_tmp2BB.YY39).tag != 45)goto _LL1AE;_tmp2BC=(struct Cyc_List_List*)(_tmp2BB.YY39).val;_LL1AD:
# 1148
 yyzzz=_tmp2BC;
goto _LL1AB;_LL1AE:;_LL1AF:
(int)_throw((void*)& Cyc_yyfail_YY39);_LL1AB:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY39).val=yy1,(((_tmp9C2.YY39).tag=45,_tmp9C2)));}static char _tmp2BF[31]="$(var_opt_t,tqual_t,type_t)@`H";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp2BF,_tmp2BF,_tmp2BF + 31}};
# 1145
struct _tuple8*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp2C0=yy1;struct _tuple8*_tmp2C1;_LL1B1: if((_tmp2C0.YY40).tag != 46)goto _LL1B3;_tmp2C1=(struct _tuple8*)(_tmp2C0.YY40).val;_LL1B2:
# 1149
 yyzzz=_tmp2C1;
goto _LL1B0;_LL1B3:;_LL1B4:
(int)_throw((void*)& Cyc_yyfail_YY40);_LL1B0:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY40).val=yy1,(((_tmp9C3.YY40).tag=46,_tmp9C3)));}static char _tmp2C4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp2C4,_tmp2C4,_tmp2C4 + 42}};
# 1146
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2C5=yy1;struct Cyc_List_List*_tmp2C6;_LL1B6: if((_tmp2C5.YY41).tag != 47)goto _LL1B8;_tmp2C6=(struct Cyc_List_List*)(_tmp2C5.YY41).val;_LL1B7:
# 1150
 yyzzz=_tmp2C6;
goto _LL1B5;_LL1B8:;_LL1B9:
(int)_throw((void*)& Cyc_yyfail_YY41);_LL1B5:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY41).val=yy1,(((_tmp9C4.YY41).tag=47,_tmp9C4)));}static char _tmp2C9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp2C9,_tmp2C9,_tmp2C9 + 115}};
# 1147
struct _tuple26*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp2CA=yy1;struct _tuple26*_tmp2CB;_LL1BB: if((_tmp2CA.YY42).tag != 48)goto _LL1BD;_tmp2CB=(struct _tuple26*)(_tmp2CA.YY42).val;_LL1BC:
# 1151
 yyzzz=_tmp2CB;
goto _LL1BA;_LL1BD:;_LL1BE:
(int)_throw((void*)& Cyc_yyfail_YY42);_LL1BA:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY42).val=yy1,(((_tmp9C5.YY42).tag=48,_tmp9C5)));}static char _tmp2CE[18]="list_t<type_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp2CE,_tmp2CE,_tmp2CE + 18}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2CF=yy1;struct Cyc_List_List*_tmp2D0;_LL1C0: if((_tmp2CF.YY43).tag != 49)goto _LL1C2;_tmp2D0=(struct Cyc_List_List*)(_tmp2CF.YY43).val;_LL1C1:
# 1152
 yyzzz=_tmp2D0;
goto _LL1BF;_LL1C2:;_LL1C3:
(int)_throw((void*)& Cyc_yyfail_YY43);_LL1BF:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY43).val=yy1,(((_tmp9C6.YY43).tag=49,_tmp9C6)));}static char _tmp2D3[24]="list_t<designator_t,`H>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp2D3,_tmp2D3,_tmp2D3 + 24}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2D4=yy1;struct Cyc_List_List*_tmp2D5;_LL1C5: if((_tmp2D4.YY44).tag != 50)goto _LL1C7;_tmp2D5=(struct Cyc_List_List*)(_tmp2D4.YY44).val;_LL1C6:
# 1154
 yyzzz=_tmp2D5;
goto _LL1C4;_LL1C7:;_LL1C8:
(int)_throw((void*)& Cyc_yyfail_YY44);_LL1C4:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY44).val=yy1,(((_tmp9C7.YY44).tag=50,_tmp9C7)));}static char _tmp2D8[13]="designator_t";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp2D8,_tmp2D8,_tmp2D8 + 13}};
# 1151
void*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2D9=yy1;void*_tmp2DA;_LL1CA: if((_tmp2D9.YY45).tag != 51)goto _LL1CC;_tmp2DA=(void*)(_tmp2D9.YY45).val;_LL1CB:
# 1155
 yyzzz=_tmp2DA;
goto _LL1C9;_LL1CC:;_LL1CD:
(int)_throw((void*)& Cyc_yyfail_YY45);_LL1C9:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY45).val=yy1,(((_tmp9C8.YY45).tag=51,_tmp9C8)));}static char _tmp2DD[7]="kind_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp2DD,_tmp2DD,_tmp2DD + 7}};
# 1152
struct Cyc_Absyn_Kind*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp2DE=yy1;struct Cyc_Absyn_Kind*_tmp2DF;_LL1CF: if((_tmp2DE.YY46).tag != 52)goto _LL1D1;_tmp2DF=(struct Cyc_Absyn_Kind*)(_tmp2DE.YY46).val;_LL1D0:
# 1156
 yyzzz=_tmp2DF;
goto _LL1CE;_LL1D1:;_LL1D2:
(int)_throw((void*)& Cyc_yyfail_YY46);_LL1CE:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY46).val=yy1,(((_tmp9C9.YY46).tag=52,_tmp9C9)));}static char _tmp2E2[7]="type_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp2E2,_tmp2E2,_tmp2E2 + 7}};
# 1153
void*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2E3=yy1;void*_tmp2E4;_LL1D4: if((_tmp2E3.YY47).tag != 53)goto _LL1D6;_tmp2E4=(void*)(_tmp2E3.YY47).val;_LL1D5:
# 1157
 yyzzz=_tmp2E4;
goto _LL1D3;_LL1D6:;_LL1D7:
(int)_throw((void*)& Cyc_yyfail_YY47);_LL1D3:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY47).val=yy1,(((_tmp9CA.YY47).tag=53,_tmp9CA)));}static char _tmp2E7[23]="list_t<attribute_t,`H>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp2E7,_tmp2E7,_tmp2E7 + 23}};
# 1154
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2E8=yy1;struct Cyc_List_List*_tmp2E9;_LL1D9: if((_tmp2E8.YY48).tag != 54)goto _LL1DB;_tmp2E9=(struct Cyc_List_List*)(_tmp2E8.YY48).val;_LL1DA:
# 1158
 yyzzz=_tmp2E9;
goto _LL1D8;_LL1DB:;_LL1DC:
(int)_throw((void*)& Cyc_yyfail_YY48);_LL1D8:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY48).val=yy1,(((_tmp9CB.YY48).tag=54,_tmp9CB)));}static char _tmp2EC[12]="attribute_t";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp2EC,_tmp2EC,_tmp2EC + 12}};
# 1155
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2ED=yy1;void*_tmp2EE;_LL1DE: if((_tmp2ED.YY49).tag != 55)goto _LL1E0;_tmp2EE=(void*)(_tmp2ED.YY49).val;_LL1DF:
# 1159
 yyzzz=_tmp2EE;
goto _LL1DD;_LL1E0:;_LL1E1:
(int)_throw((void*)& Cyc_yyfail_YY49);_LL1DD:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY49).val=yy1,(((_tmp9CC.YY49).tag=55,_tmp9CC)));}static char _tmp2F1[12]="enumfield_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp2F1,_tmp2F1,_tmp2F1 + 12}};
# 1156
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp2F2=yy1;struct Cyc_Absyn_Enumfield*_tmp2F3;_LL1E3: if((_tmp2F2.YY50).tag != 56)goto _LL1E5;_tmp2F3=(struct Cyc_Absyn_Enumfield*)(_tmp2F2.YY50).val;_LL1E4:
# 1160
 yyzzz=_tmp2F3;
goto _LL1E2;_LL1E5:;_LL1E6:
(int)_throw((void*)& Cyc_yyfail_YY50);_LL1E2:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY50).val=yy1,(((_tmp9CD.YY50).tag=56,_tmp9CD)));}static char _tmp2F6[23]="list_t<enumfield_t,`H>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp2F6,_tmp2F6,_tmp2F6 + 23}};
# 1157
struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp2F7=yy1;struct Cyc_List_List*_tmp2F8;_LL1E8: if((_tmp2F7.YY51).tag != 57)goto _LL1EA;_tmp2F8=(struct Cyc_List_List*)(_tmp2F7.YY51).val;_LL1E9:
# 1161
 yyzzz=_tmp2F8;
goto _LL1E7;_LL1EA:;_LL1EB:
(int)_throw((void*)& Cyc_yyfail_YY51);_LL1E7:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY51).val=yy1,(((_tmp9CE.YY51).tag=57,_tmp9CE)));}static char _tmp2FB[11]="type_opt_t";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp2FB,_tmp2FB,_tmp2FB + 11}};
# 1158
void*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp2FC=yy1;void*_tmp2FD;_LL1ED: if((_tmp2FC.YY52).tag != 58)goto _LL1EF;_tmp2FD=(void*)(_tmp2FC.YY52).val;_LL1EE:
# 1162
 yyzzz=_tmp2FD;
goto _LL1EC;_LL1EF:;_LL1F0:
(int)_throw((void*)& Cyc_yyfail_YY52);_LL1EC:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY52).val=yy1,(((_tmp9CF.YY52).tag=58,_tmp9CF)));}static char _tmp300[31]="list_t<$(type_t,type_t)@`H,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp300,_tmp300,_tmp300 + 31}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp301=yy1;struct Cyc_List_List*_tmp302;_LL1F2: if((_tmp301.YY53).tag != 59)goto _LL1F4;_tmp302=(struct Cyc_List_List*)(_tmp301.YY53).val;_LL1F3:
# 1163
 yyzzz=_tmp302;
goto _LL1F1;_LL1F4:;_LL1F5:
(int)_throw((void*)& Cyc_yyfail_YY53);_LL1F1:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY53).val=yy1,(((_tmp9D0.YY53).tag=59,_tmp9D0)));}static char _tmp305[15]="conref_t<bool>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp305,_tmp305,_tmp305 + 15}};
# 1160
union Cyc_Absyn_Constraint*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp306=yy1;union Cyc_Absyn_Constraint*_tmp307;_LL1F7: if((_tmp306.YY54).tag != 60)goto _LL1F9;_tmp307=(union Cyc_Absyn_Constraint*)(_tmp306.YY54).val;_LL1F8:
# 1164
 yyzzz=_tmp307;
goto _LL1F6;_LL1F9:;_LL1FA:
(int)_throw((void*)& Cyc_yyfail_YY54);_LL1F6:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY54).val=yy1,(((_tmp9D1.YY54).tag=60,_tmp9D1)));}static char _tmp30A[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp30A,_tmp30A,_tmp30A + 45}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp30B=yy1;struct Cyc_List_List*_tmp30C;_LL1FC: if((_tmp30B.YY55).tag != 61)goto _LL1FE;_tmp30C=(struct Cyc_List_List*)(_tmp30B.YY55).val;_LL1FD:
# 1165
 yyzzz=_tmp30C;
goto _LL1FB;_LL1FE:;_LL1FF:
(int)_throw((void*)& Cyc_yyfail_YY55);_LL1FB:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY55).val=yy1,(((_tmp9D2.YY55).tag=61,_tmp9D2)));}static char _tmp30F[20]="pointer_qual_t<`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY56={Cyc_Core_Failure,{_tmp30F,_tmp30F,_tmp30F + 20}};
# 1162
void*Cyc_yyget_YY56(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp310=yy1;void*_tmp311;_LL201: if((_tmp310.YY56).tag != 62)goto _LL203;_tmp311=(void*)(_tmp310.YY56).val;_LL202:
# 1166
 yyzzz=_tmp311;
goto _LL200;_LL203:;_LL204:
(int)_throw((void*)& Cyc_yyfail_YY56);_LL200:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY56).val=yy1,(((_tmp9D3.YY56).tag=62,_tmp9D3)));}static char _tmp314[21]="pointer_quals_t<`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY57={Cyc_Core_Failure,{_tmp314,_tmp314,_tmp314 + 21}};
# 1163
struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp315=yy1;struct Cyc_List_List*_tmp316;_LL206: if((_tmp315.YY57).tag != 63)goto _LL208;_tmp316=(struct Cyc_List_List*)(_tmp315.YY57).val;_LL207:
# 1167
 yyzzz=_tmp316;
goto _LL205;_LL208:;_LL209:
(int)_throw((void*)& Cyc_yyfail_YY57);_LL205:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY57).val=yy1,(((_tmp9D4.YY57).tag=63,_tmp9D4)));}static char _tmp319[21]="$(bool,string_t<`H>)";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp319,_tmp319,_tmp319 + 21}};
# 1164
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp31A=yy1;struct _tuple20 _tmp31B;_LL20B: if((_tmp31A.Asm_tok).tag != 6)goto _LL20D;_tmp31B=(struct _tuple20)(_tmp31A.Asm_tok).val;_LL20C:
# 1168
 yyzzz=_tmp31B;
goto _LL20A;_LL20D:;_LL20E:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);_LL20A:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.Asm_tok).val=yy1,(((_tmp9D5.Asm_tok).tag=6,_tmp9D5)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1186
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1190
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9D6;return(_tmp9D6.timestamp=0,((_tmp9D6.first_line=0,((_tmp9D6.first_column=0,((_tmp9D6.last_line=0,((_tmp9D6.last_column=0,_tmp9D6)))))))));}
# 1193
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1204 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp31E[2]="$";static char _tmp31F[6]="error";static char _tmp320[12]="$undefined.";static char _tmp321[5]="AUTO";static char _tmp322[9]="REGISTER";static char _tmp323[7]="STATIC";static char _tmp324[7]="EXTERN";static char _tmp325[8]="TYPEDEF";static char _tmp326[5]="VOID";static char _tmp327[5]="CHAR";static char _tmp328[6]="SHORT";static char _tmp329[4]="INT";static char _tmp32A[5]="LONG";static char _tmp32B[6]="FLOAT";static char _tmp32C[7]="DOUBLE";static char _tmp32D[7]="SIGNED";static char _tmp32E[9]="UNSIGNED";static char _tmp32F[6]="CONST";static char _tmp330[9]="VOLATILE";static char _tmp331[9]="RESTRICT";static char _tmp332[7]="STRUCT";static char _tmp333[6]="UNION";static char _tmp334[5]="CASE";static char _tmp335[8]="DEFAULT";static char _tmp336[7]="INLINE";static char _tmp337[7]="SIZEOF";static char _tmp338[9]="OFFSETOF";static char _tmp339[3]="IF";static char _tmp33A[5]="ELSE";static char _tmp33B[7]="SWITCH";static char _tmp33C[6]="WHILE";static char _tmp33D[3]="DO";static char _tmp33E[4]="FOR";static char _tmp33F[5]="GOTO";static char _tmp340[9]="CONTINUE";static char _tmp341[6]="BREAK";static char _tmp342[7]="RETURN";static char _tmp343[5]="ENUM";static char _tmp344[8]="NULL_kw";static char _tmp345[4]="LET";static char _tmp346[6]="THROW";static char _tmp347[4]="TRY";static char _tmp348[6]="CATCH";static char _tmp349[7]="EXPORT";static char _tmp34A[4]="NEW";static char _tmp34B[9]="ABSTRACT";static char _tmp34C[9]="FALLTHRU";static char _tmp34D[6]="USING";static char _tmp34E[10]="NAMESPACE";static char _tmp34F[9]="DATATYPE";static char _tmp350[8]="XTUNION";static char _tmp351[7]="TUNION";static char _tmp352[7]="MALLOC";static char _tmp353[8]="RMALLOC";static char _tmp354[7]="CALLOC";static char _tmp355[8]="RCALLOC";static char _tmp356[5]="SWAP";static char _tmp357[9]="REGION_T";static char _tmp358[6]="TAG_T";static char _tmp359[7]="REGION";static char _tmp35A[5]="RNEW";static char _tmp35B[8]="REGIONS";static char _tmp35C[13]="RESET_REGION";static char _tmp35D[16]="NOZEROTERM_QUAL";static char _tmp35E[14]="ZEROTERM_QUAL";static char _tmp35F[12]="REGION_QUAL";static char _tmp360[7]="PORTON";static char _tmp361[8]="PORTOFF";static char _tmp362[12]="DYNREGION_T";static char _tmp363[8]="NUMELTS";static char _tmp364[8]="VALUEOF";static char _tmp365[10]="VALUEOF_T";static char _tmp366[9]="TAGCHECK";static char _tmp367[13]="NUMELTS_QUAL";static char _tmp368[10]="THIN_QUAL";static char _tmp369[9]="FAT_QUAL";static char _tmp36A[13]="NOTNULL_QUAL";static char _tmp36B[14]="NULLABLE_QUAL";static char _tmp36C[14]="REQUIRES_QUAL";static char _tmp36D[12]="TAGGED_QUAL";static char _tmp36E[16]="EXTENSIBLE_QUAL";static char _tmp36F[15]="RESETABLE_QUAL";static char _tmp370[7]="PTR_OP";static char _tmp371[7]="INC_OP";static char _tmp372[7]="DEC_OP";static char _tmp373[8]="LEFT_OP";static char _tmp374[9]="RIGHT_OP";static char _tmp375[6]="LE_OP";static char _tmp376[6]="GE_OP";static char _tmp377[6]="EQ_OP";static char _tmp378[6]="NE_OP";static char _tmp379[7]="AND_OP";static char _tmp37A[6]="OR_OP";static char _tmp37B[11]="MUL_ASSIGN";static char _tmp37C[11]="DIV_ASSIGN";static char _tmp37D[11]="MOD_ASSIGN";static char _tmp37E[11]="ADD_ASSIGN";static char _tmp37F[11]="SUB_ASSIGN";static char _tmp380[12]="LEFT_ASSIGN";static char _tmp381[13]="RIGHT_ASSIGN";static char _tmp382[11]="AND_ASSIGN";static char _tmp383[11]="XOR_ASSIGN";static char _tmp384[10]="OR_ASSIGN";static char _tmp385[9]="ELLIPSIS";static char _tmp386[11]="LEFT_RIGHT";static char _tmp387[12]="COLON_COLON";static char _tmp388[11]="IDENTIFIER";static char _tmp389[17]="INTEGER_CONSTANT";static char _tmp38A[7]="STRING";static char _tmp38B[8]="WSTRING";static char _tmp38C[19]="CHARACTER_CONSTANT";static char _tmp38D[20]="WCHARACTER_CONSTANT";static char _tmp38E[18]="FLOATING_CONSTANT";static char _tmp38F[9]="TYPE_VAR";static char _tmp390[13]="TYPEDEF_NAME";static char _tmp391[16]="QUAL_IDENTIFIER";static char _tmp392[18]="QUAL_TYPEDEF_NAME";static char _tmp393[10]="ATTRIBUTE";static char _tmp394[4]="ASM";static char _tmp395[4]="';'";static char _tmp396[4]="'{'";static char _tmp397[4]="'}'";static char _tmp398[4]="','";static char _tmp399[4]="'='";static char _tmp39A[4]="'<'";static char _tmp39B[4]="'>'";static char _tmp39C[4]="'('";static char _tmp39D[4]="')'";static char _tmp39E[4]="'_'";static char _tmp39F[4]="'$'";static char _tmp3A0[4]="':'";static char _tmp3A1[4]="'.'";static char _tmp3A2[4]="'['";static char _tmp3A3[4]="']'";static char _tmp3A4[4]="'*'";static char _tmp3A5[4]="'@'";static char _tmp3A6[4]="'?'";static char _tmp3A7[4]="'+'";static char _tmp3A8[4]="'|'";static char _tmp3A9[4]="'^'";static char _tmp3AA[4]="'&'";static char _tmp3AB[4]="'-'";static char _tmp3AC[4]="'/'";static char _tmp3AD[4]="'%'";static char _tmp3AE[4]="'~'";static char _tmp3AF[4]="'!'";static char _tmp3B0[5]="prog";static char _tmp3B1[17]="translation_unit";static char _tmp3B2[12]="export_list";static char _tmp3B3[19]="export_list_values";static char _tmp3B4[21]="external_declaration";static char _tmp3B5[15]="optional_comma";static char _tmp3B6[20]="function_definition";static char _tmp3B7[21]="function_definition2";static char _tmp3B8[13]="using_action";static char _tmp3B9[15]="unusing_action";static char _tmp3BA[17]="namespace_action";static char _tmp3BB[19]="unnamespace_action";static char _tmp3BC[12]="declaration";static char _tmp3BD[19]="resetable_qual_opt";static char _tmp3BE[17]="declaration_list";static char _tmp3BF[23]="declaration_specifiers";static char _tmp3C0[24]="storage_class_specifier";static char _tmp3C1[15]="attributes_opt";static char _tmp3C2[11]="attributes";static char _tmp3C3[15]="attribute_list";static char _tmp3C4[10]="attribute";static char _tmp3C5[15]="type_specifier";static char _tmp3C6[25]="type_specifier_notypedef";static char _tmp3C7[5]="kind";static char _tmp3C8[15]="type_qualifier";static char _tmp3C9[15]="enum_specifier";static char _tmp3CA[11]="enum_field";static char _tmp3CB[22]="enum_declaration_list";static char _tmp3CC[26]="struct_or_union_specifier";static char _tmp3CD[16]="type_params_opt";static char _tmp3CE[16]="struct_or_union";static char _tmp3CF[24]="struct_declaration_list";static char _tmp3D0[25]="struct_declaration_list0";static char _tmp3D1[21]="init_declarator_list";static char _tmp3D2[22]="init_declarator_list0";static char _tmp3D3[16]="init_declarator";static char _tmp3D4[19]="struct_declaration";static char _tmp3D5[25]="specifier_qualifier_list";static char _tmp3D6[35]="notypedef_specifier_qualifier_list";static char _tmp3D7[23]="struct_declarator_list";static char _tmp3D8[24]="struct_declarator_list0";static char _tmp3D9[18]="struct_declarator";static char _tmp3DA[19]="datatype_specifier";static char _tmp3DB[14]="qual_datatype";static char _tmp3DC[19]="datatypefield_list";static char _tmp3DD[20]="datatypefield_scope";static char _tmp3DE[14]="datatypefield";static char _tmp3DF[11]="declarator";static char _tmp3E0[23]="declarator_withtypedef";static char _tmp3E1[18]="direct_declarator";static char _tmp3E2[30]="direct_declarator_withtypedef";static char _tmp3E3[8]="pointer";static char _tmp3E4[12]="one_pointer";static char _tmp3E5[14]="pointer_quals";static char _tmp3E6[13]="pointer_qual";static char _tmp3E7[23]="pointer_null_and_bound";static char _tmp3E8[14]="pointer_bound";static char _tmp3E9[18]="zeroterm_qual_opt";static char _tmp3EA[8]="rgn_opt";static char _tmp3EB[11]="tqual_list";static char _tmp3EC[20]="parameter_type_list";static char _tmp3ED[9]="type_var";static char _tmp3EE[16]="optional_effect";static char _tmp3EF[19]="optional_rgn_order";static char _tmp3F0[10]="rgn_order";static char _tmp3F1[16]="optional_inject";static char _tmp3F2[11]="effect_set";static char _tmp3F3[14]="atomic_effect";static char _tmp3F4[11]="region_set";static char _tmp3F5[15]="parameter_list";static char _tmp3F6[22]="parameter_declaration";static char _tmp3F7[16]="identifier_list";static char _tmp3F8[17]="identifier_list0";static char _tmp3F9[12]="initializer";static char _tmp3FA[18]="array_initializer";static char _tmp3FB[17]="initializer_list";static char _tmp3FC[12]="designation";static char _tmp3FD[16]="designator_list";static char _tmp3FE[11]="designator";static char _tmp3FF[10]="type_name";static char _tmp400[14]="any_type_name";static char _tmp401[15]="type_name_list";static char _tmp402[20]="abstract_declarator";static char _tmp403[27]="direct_abstract_declarator";static char _tmp404[10]="statement";static char _tmp405[18]="labeled_statement";static char _tmp406[21]="expression_statement";static char _tmp407[19]="compound_statement";static char _tmp408[16]="block_item_list";static char _tmp409[20]="selection_statement";static char _tmp40A[15]="switch_clauses";static char _tmp40B[20]="iteration_statement";static char _tmp40C[15]="jump_statement";static char _tmp40D[12]="exp_pattern";static char _tmp40E[20]="conditional_pattern";static char _tmp40F[19]="logical_or_pattern";static char _tmp410[20]="logical_and_pattern";static char _tmp411[21]="inclusive_or_pattern";static char _tmp412[21]="exclusive_or_pattern";static char _tmp413[12]="and_pattern";static char _tmp414[17]="equality_pattern";static char _tmp415[19]="relational_pattern";static char _tmp416[14]="shift_pattern";static char _tmp417[17]="additive_pattern";static char _tmp418[23]="multiplicative_pattern";static char _tmp419[13]="cast_pattern";static char _tmp41A[14]="unary_pattern";static char _tmp41B[16]="postfix_pattern";static char _tmp41C[16]="primary_pattern";static char _tmp41D[8]="pattern";static char _tmp41E[19]="tuple_pattern_list";static char _tmp41F[20]="tuple_pattern_list0";static char _tmp420[14]="field_pattern";static char _tmp421[19]="field_pattern_list";static char _tmp422[20]="field_pattern_list0";static char _tmp423[11]="expression";static char _tmp424[22]="assignment_expression";static char _tmp425[20]="assignment_operator";static char _tmp426[23]="conditional_expression";static char _tmp427[20]="constant_expression";static char _tmp428[22]="logical_or_expression";static char _tmp429[23]="logical_and_expression";static char _tmp42A[24]="inclusive_or_expression";static char _tmp42B[24]="exclusive_or_expression";static char _tmp42C[15]="and_expression";static char _tmp42D[20]="equality_expression";static char _tmp42E[22]="relational_expression";static char _tmp42F[17]="shift_expression";static char _tmp430[20]="additive_expression";static char _tmp431[26]="multiplicative_expression";static char _tmp432[16]="cast_expression";static char _tmp433[17]="unary_expression";static char _tmp434[15]="unary_operator";static char _tmp435[19]="postfix_expression";static char _tmp436[17]="field_expression";static char _tmp437[19]="primary_expression";static char _tmp438[25]="argument_expression_list";static char _tmp439[26]="argument_expression_list0";static char _tmp43A[9]="constant";static char _tmp43B[20]="qual_opt_identifier";static char _tmp43C[17]="qual_opt_typedef";static char _tmp43D[18]="struct_union_name";static char _tmp43E[11]="field_name";static char _tmp43F[12]="right_angle";
# 1554 "parse.y"
static struct _dyneither_ptr Cyc_yytname[290]={{_tmp31E,_tmp31E,_tmp31E + 2},{_tmp31F,_tmp31F,_tmp31F + 6},{_tmp320,_tmp320,_tmp320 + 12},{_tmp321,_tmp321,_tmp321 + 5},{_tmp322,_tmp322,_tmp322 + 9},{_tmp323,_tmp323,_tmp323 + 7},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 7},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 6},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 3},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 3},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 9},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 4},{_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 4},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 7},{_tmp34A,_tmp34A,_tmp34A + 4},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 9},{_tmp34D,_tmp34D,_tmp34D + 6},{_tmp34E,_tmp34E,_tmp34E + 10},{_tmp34F,_tmp34F,_tmp34F + 9},{_tmp350,_tmp350,_tmp350 + 8},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 8},{_tmp356,_tmp356,_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 9},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 8},{_tmp35C,_tmp35C,_tmp35C + 13},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 8},{_tmp362,_tmp362,_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 8},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,_tmp365 + 10},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 13},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 13},{_tmp36B,_tmp36B,_tmp36B + 14},{_tmp36C,_tmp36C,_tmp36C + 14},{_tmp36D,_tmp36D,_tmp36D + 12},{_tmp36E,_tmp36E,_tmp36E + 16},{_tmp36F,_tmp36F,_tmp36F + 15},{_tmp370,_tmp370,_tmp370 + 7},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 7},{_tmp373,_tmp373,_tmp373 + 8},{_tmp374,_tmp374,_tmp374 + 9},{_tmp375,_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,_tmp377 + 6},{_tmp378,_tmp378,_tmp378 + 6},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 11},{_tmp37C,_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D,_tmp37D + 11},{_tmp37E,_tmp37E,_tmp37E + 11},{_tmp37F,_tmp37F,_tmp37F + 11},{_tmp380,_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 13},{_tmp382,_tmp382,_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 11},{_tmp384,_tmp384,_tmp384 + 10},{_tmp385,_tmp385,_tmp385 + 9},{_tmp386,_tmp386,_tmp386 + 11},{_tmp387,_tmp387,_tmp387 + 12},{_tmp388,_tmp388,_tmp388 + 11},{_tmp389,_tmp389,_tmp389 + 17},{_tmp38A,_tmp38A,_tmp38A + 7},{_tmp38B,_tmp38B,_tmp38B + 8},{_tmp38C,_tmp38C,_tmp38C + 19},{_tmp38D,_tmp38D,_tmp38D + 20},{_tmp38E,_tmp38E,_tmp38E + 18},{_tmp38F,_tmp38F,_tmp38F + 9},{_tmp390,_tmp390,_tmp390 + 13},{_tmp391,_tmp391,_tmp391 + 16},{_tmp392,_tmp392,_tmp392 + 18},{_tmp393,_tmp393,_tmp393 + 10},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 4},{_tmp398,_tmp398,_tmp398 + 4},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 4},{_tmp3A1,_tmp3A1,_tmp3A1 + 4},{_tmp3A2,_tmp3A2,_tmp3A2 + 4},{_tmp3A3,_tmp3A3,_tmp3A3 + 4},{_tmp3A4,_tmp3A4,_tmp3A4 + 4},{_tmp3A5,_tmp3A5,_tmp3A5 + 4},{_tmp3A6,_tmp3A6,_tmp3A6 + 4},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 5},{_tmp3B1,_tmp3B1,_tmp3B1 + 17},{_tmp3B2,_tmp3B2,_tmp3B2 + 12},{_tmp3B3,_tmp3B3,_tmp3B3 + 19},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 15},{_tmp3B6,_tmp3B6,_tmp3B6 + 20},{_tmp3B7,_tmp3B7,_tmp3B7 + 21},{_tmp3B8,_tmp3B8,_tmp3B8 + 13},{_tmp3B9,_tmp3B9,_tmp3B9 + 15},{_tmp3BA,_tmp3BA,_tmp3BA + 17},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 12},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 17},{_tmp3BF,_tmp3BF,_tmp3BF + 23},{_tmp3C0,_tmp3C0,_tmp3C0 + 24},{_tmp3C1,_tmp3C1,_tmp3C1 + 15},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 15},{_tmp3C4,_tmp3C4,_tmp3C4 + 10},{_tmp3C5,_tmp3C5,_tmp3C5 + 15},{_tmp3C6,_tmp3C6,_tmp3C6 + 25},{_tmp3C7,_tmp3C7,_tmp3C7 + 5},{_tmp3C8,_tmp3C8,_tmp3C8 + 15},{_tmp3C9,_tmp3C9,_tmp3C9 + 15},{_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 22},{_tmp3CC,_tmp3CC,_tmp3CC + 26},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 16},{_tmp3CF,_tmp3CF,_tmp3CF + 24},{_tmp3D0,_tmp3D0,_tmp3D0 + 25},{_tmp3D1,_tmp3D1,_tmp3D1 + 21},{_tmp3D2,_tmp3D2,_tmp3D2 + 22},{_tmp3D3,_tmp3D3,_tmp3D3 + 16},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 25},{_tmp3D6,_tmp3D6,_tmp3D6 + 35},{_tmp3D7,_tmp3D7,_tmp3D7 + 23},{_tmp3D8,_tmp3D8,_tmp3D8 + 24},{_tmp3D9,_tmp3D9,_tmp3D9 + 18},{_tmp3DA,_tmp3DA,_tmp3DA + 19},{_tmp3DB,_tmp3DB,_tmp3DB + 14},{_tmp3DC,_tmp3DC,_tmp3DC + 19},{_tmp3DD,_tmp3DD,_tmp3DD + 20},{_tmp3DE,_tmp3DE,_tmp3DE + 14},{_tmp3DF,_tmp3DF,_tmp3DF + 11},{_tmp3E0,_tmp3E0,_tmp3E0 + 23},{_tmp3E1,_tmp3E1,_tmp3E1 + 18},{_tmp3E2,_tmp3E2,_tmp3E2 + 30},{_tmp3E3,_tmp3E3,_tmp3E3 + 8},{_tmp3E4,_tmp3E4,_tmp3E4 + 12},{_tmp3E5,_tmp3E5,_tmp3E5 + 14},{_tmp3E6,_tmp3E6,_tmp3E6 + 13},{_tmp3E7,_tmp3E7,_tmp3E7 + 23},{_tmp3E8,_tmp3E8,_tmp3E8 + 14},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 8},{_tmp3EB,_tmp3EB,_tmp3EB + 11},{_tmp3EC,_tmp3EC,_tmp3EC + 20},{_tmp3ED,_tmp3ED,_tmp3ED + 9},{_tmp3EE,_tmp3EE,_tmp3EE + 16},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 10},{_tmp3F1,_tmp3F1,_tmp3F1 + 16},{_tmp3F2,_tmp3F2,_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 14},{_tmp3F4,_tmp3F4,_tmp3F4 + 11},{_tmp3F5,_tmp3F5,_tmp3F5 + 15},{_tmp3F6,_tmp3F6,_tmp3F6 + 22},{_tmp3F7,_tmp3F7,_tmp3F7 + 16},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 12},{_tmp3FA,_tmp3FA,_tmp3FA + 18},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 12},{_tmp3FD,_tmp3FD,_tmp3FD + 16},{_tmp3FE,_tmp3FE,_tmp3FE + 11},{_tmp3FF,_tmp3FF,_tmp3FF + 10},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 15},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 27},{_tmp404,_tmp404,_tmp404 + 10},{_tmp405,_tmp405,_tmp405 + 18},{_tmp406,_tmp406,_tmp406 + 21},{_tmp407,_tmp407,_tmp407 + 19},{_tmp408,_tmp408,_tmp408 + 16},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 15},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 12},{_tmp40E,_tmp40E,_tmp40E + 20},{_tmp40F,_tmp40F,_tmp40F + 19},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 21},{_tmp412,_tmp412,_tmp412 + 21},{_tmp413,_tmp413,_tmp413 + 12},{_tmp414,_tmp414,_tmp414 + 17},{_tmp415,_tmp415,_tmp415 + 19},{_tmp416,_tmp416,_tmp416 + 14},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 23},{_tmp419,_tmp419,_tmp419 + 13},{_tmp41A,_tmp41A,_tmp41A + 14},{_tmp41B,_tmp41B,_tmp41B + 16},{_tmp41C,_tmp41C,_tmp41C + 16},{_tmp41D,_tmp41D,_tmp41D + 8},{_tmp41E,_tmp41E,_tmp41E + 19},{_tmp41F,_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 14},{_tmp421,_tmp421,_tmp421 + 19},{_tmp422,_tmp422,_tmp422 + 20},{_tmp423,_tmp423,_tmp423 + 11},{_tmp424,_tmp424,_tmp424 + 22},{_tmp425,_tmp425,_tmp425 + 20},{_tmp426,_tmp426,_tmp426 + 23},{_tmp427,_tmp427,_tmp427 + 20},{_tmp428,_tmp428,_tmp428 + 22},{_tmp429,_tmp429,_tmp429 + 23},{_tmp42A,_tmp42A,_tmp42A + 24},{_tmp42B,_tmp42B,_tmp42B + 24},{_tmp42C,_tmp42C,_tmp42C + 15},{_tmp42D,_tmp42D,_tmp42D + 20},{_tmp42E,_tmp42E,_tmp42E + 22},{_tmp42F,_tmp42F,_tmp42F + 17},{_tmp430,_tmp430,_tmp430 + 20},{_tmp431,_tmp431,_tmp431 + 26},{_tmp432,_tmp432,_tmp432 + 16},{_tmp433,_tmp433,_tmp433 + 17},{_tmp434,_tmp434,_tmp434 + 15},{_tmp435,_tmp435,_tmp435 + 19},{_tmp436,_tmp436,_tmp436 + 17},{_tmp437,_tmp437,_tmp437 + 19},{_tmp438,_tmp438,_tmp438 + 25},{_tmp439,_tmp439,_tmp439 + 26},{_tmp43A,_tmp43A,_tmp43A + 9},{_tmp43B,_tmp43B,_tmp43B + 20},{_tmp43C,_tmp43C,_tmp43C + 17},{_tmp43D,_tmp43D,_tmp43D + 18},{_tmp43E,_tmp43E,_tmp43E + 11},{_tmp43F,_tmp43F,_tmp43F + 12}};
# 1608
static short Cyc_yyr1[523]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,152,152,152,152,153,153,154,155,156,157,158,158,158,158,158,158,159,159,160,160,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,163,163,164,165,165,166,166,166,166,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,170,170,170,171,171,171,172,172,173,173,173,174,174,174,174,174,175,175,176,176,177,177,178,178,179,180,180,181,181,182,183,183,183,183,183,183,184,184,184,184,184,184,185,186,186,187,187,187,187,187,188,188,188,189,189,190,190,190,190,191,191,191,192,192,193,193,194,194,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,196,197,197,198,199,199,200,200,200,200,200,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,214,215,215,216,216,216,217,218,218,219,219,220,220,220,220,220,221,221,221,221,222,223,223,224,224,225,225,226,226,226,226,226,227,227,228,228,228,229,229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,230,- 1,- 1,231,232,232,233,233,234,234,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,281,281,281,281,281,281,281,281,281,281,282,283,283,284,284,284,284,284,285,285,286,286,287,287,288,288,289,289};
# 1664
static short Cyc_yyr2[523]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1720
static short Cyc_yydefact[1032]={0,19,52,53,54,55,57,72,75,76,77,78,79,80,81,82,96,97,98,114,115,48,0,0,58,0,0,149,89,93,0,0,0,0,0,0,39,513,215,515,514,516,0,0,73,0,201,201,200,1,0,17,0,0,18,0,0,42,50,44,70,46,83,84,0,85,0,38,160,0,185,188,86,164,112,56,55,49,0,100,512,0,513,508,509,510,511,112,0,377,0,0,0,0,238,0,379,380,28,30,0,0,0,0,0,0,0,150,0,0,0,0,0,0,0,198,199,2,0,0,0,0,0,32,0,120,121,123,43,51,45,47,116,517,518,112,112,38,40,38,0,21,0,217,0,173,161,186,195,194,0,0,192,193,196,197,206,188,0,71,0,56,104,0,102,0,513,388,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,497,498,477,0,0,0,0,0,480,478,479,0,405,407,421,429,431,433,435,437,439,442,447,450,453,457,0,459,481,496,495,0,389,387,35,0,0,112,0,0,0,130,126,128,257,259,0,0,9,10,0,0,112,519,520,216,95,0,0,165,74,236,0,233,0,3,0,5,0,0,0,33,0,0,38,21,0,117,118,143,111,0,147,0,0,0,0,0,0,0,0,0,0,0,0,513,290,292,0,300,294,0,298,280,281,282,0,283,284,285,0,41,21,123,20,22,264,0,223,239,0,0,219,217,0,203,0,0,0,208,59,207,189,0,0,105,101,0,0,381,0,0,402,0,0,0,0,253,398,403,0,400,0,467,0,423,457,0,424,425,0,0,0,0,0,0,0,0,0,460,461,38,0,0,0,463,464,462,0,378,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,411,412,413,414,415,416,417,418,419,420,410,0,465,0,487,488,0,0,0,500,0,112,395,396,0,393,0,240,0,0,0,0,260,0,231,131,136,132,134,127,129,217,0,266,258,267,522,521,0,88,92,0,90,94,110,65,64,0,62,166,217,235,162,266,237,174,175,0,87,202,29,0,31,0,37,0,122,124,242,241,21,23,107,119,0,0,0,138,139,141,0,112,112,155,0,0,0,0,0,513,0,38,0,329,330,331,0,0,333,0,0,38,0,301,295,123,299,293,291,24,0,172,224,0,0,0,230,218,225,169,0,0,0,219,171,205,204,167,203,0,0,209,60,113,0,106,428,103,99,391,392,256,0,399,252,254,386,0,0,0,0,243,248,0,0,0,0,0,0,0,0,0,0,0,0,0,499,506,0,505,406,430,0,432,434,436,438,440,441,445,446,443,444,448,449,451,452,454,455,456,409,408,486,483,0,485,0,0,0,383,0,390,34,0,384,0,261,0,137,133,135,0,219,0,203,0,268,0,217,0,279,263,0,112,0,0,0,130,0,112,0,217,0,184,163,234,4,6,0,25,0,142,125,143,0,0,219,148,157,156,0,0,151,0,0,0,308,0,0,0,0,0,0,328,332,0,0,0,0,296,289,38,26,265,217,0,227,0,0,220,0,170,223,211,168,191,190,209,187,0,7,0,0,255,401,404,466,0,0,244,0,249,469,0,0,0,0,473,476,0,0,0,0,0,458,502,0,0,484,482,0,0,394,397,385,262,232,275,0,269,270,203,0,0,219,203,0,91,219,0,0,0,61,63,0,176,0,0,219,0,203,0,0,140,145,144,116,146,158,155,155,0,0,0,0,0,0,0,0,0,0,0,0,0,308,334,0,0,297,27,219,0,228,226,0,219,0,210,0,8,382,492,0,491,0,245,250,0,0,0,0,426,427,486,485,504,0,507,422,501,503,0,274,272,278,277,0,271,203,116,0,68,66,67,177,183,180,0,182,178,203,36,0,0,154,153,302,308,0,0,0,0,0,0,336,337,339,341,343,345,347,349,352,357,360,363,367,369,375,376,0,38,305,314,0,0,0,0,0,0,0,0,0,0,335,286,213,229,221,212,217,13,0,14,468,0,0,251,470,0,0,475,474,489,0,276,273,0,0,181,179,109,0,0,0,308,0,373,0,0,370,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,371,309,0,316,0,0,0,324,0,0,0,0,0,0,307,0,219,12,15,0,494,493,0,0,0,490,108,0,159,303,304,0,0,0,0,308,310,340,0,342,344,346,348,350,351,355,356,353,354,358,359,361,362,364,365,366,0,315,317,318,0,326,325,0,320,0,0,0,222,214,16,0,0,0,0,0,306,372,0,368,311,0,38,319,327,321,322,0,247,246,471,0,69,0,338,308,312,323,0,374,313,472,0,0,0};
# 1827
static short Cyc_yydefgoto[144]={1029,49,686,877,50,300,51,284,52,456,53,458,54,55,139,56,57,526,226,442,443,227,60,240,228,62,162,163,63,159,64,261,262,124,125,126,263,229,424,471,472,473,65,66,645,646,647,67,474,68,447,69,70,156,157,71,115,522,315,683,606,72,607,516,674,508,512,513,418,308,248,93,94,547,463,548,330,331,332,230,301,302,608,430,287,288,289,290,291,292,761,293,294,838,839,840,841,842,843,844,845,846,847,848,849,850,851,852,853,333,409,410,334,335,336,295,197,396,198,532,199,200,201,202,203,204,205,206,207,208,209,210,211,212,787,213,563,564,214,215,74,878,241,434};
# 1845
static short Cyc_yypact[1032]={2936,- -32768,- -32768,- -32768,- -32768,- 77,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3507,335,1018,- -32768,88,- 14,- -32768,49,65,136,233,86,196,95,254,- -32768,- -32768,259,- -32768,- -32768,- -32768,250,522,298,318,348,348,- -32768,- -32768,2799,- -32768,270,295,- -32768,403,600,3507,3507,3507,- -32768,3507,- -32768,- -32768,351,- -32768,88,3426,231,162,578,1164,- -32768,- -32768,352,362,402,- -32768,88,375,- -32768,1408,129,- -32768,- -32768,- -32768,- -32768,352,6281,- -32768,394,416,1408,405,441,434,- -32768,154,- -32768,- -32768,3653,3653,2799,2799,3653,6281,710,- -32768,144,483,763,440,144,4018,6281,- -32768,- -32768,- -32768,2799,3072,2799,3072,146,- -32768,486,501,- -32768,1163,- -32768,- -32768,- -32768,- -32768,4018,- -32768,- -32768,352,177,1805,- -32768,3426,600,513,3653,3580,4651,- -32768,231,- -32768,- -32768,- -32768,514,516,- -32768,- -32768,- -32768,- -32768,91,1164,3653,- -32768,3072,- -32768,530,523,549,88,203,- -32768,1408,165,1492,6515,537,6281,6359,548,556,571,582,598,611,625,636,6593,6593,- -32768,- -32768,- -32768,2515,640,6671,6671,6671,- -32768,- -32768,- -32768,332,- -32768,- -32768,- 27,687,645,647,653,643,175,339,194,263,- -32768,1248,6671,134,- 58,- -32768,680,841,703,- -32768,- -32768,705,6281,352,841,702,3799,4018,4091,4018,575,- -32768,97,97,- -32768,- -32768,45,370,352,- -32768,- -32768,- -32768,- -32768,34,707,- -32768,- -32768,614,377,- -32768,720,- -32768,723,- -32768,734,719,746,- -32768,763,4749,3426,513,752,4018,- -32768,661,754,88,755,750,94,751,4161,757,779,767,771,4847,4161,314,765,762,- -32768,- -32768,770,1948,1948,600,1948,- -32768,- -32768,- -32768,772,- -32768,- -32768,- -32768,19,- -32768,513,778,- -32768,- -32768,766,76,794,- -32768,23,780,774,68,781,735,784,3653,6281,- -32768,788,- -32768,- -32768,76,789,88,- -32768,6281,798,- -32768,795,799,- -32768,144,6281,1408,347,- -32768,- -32768,- -32768,804,806,2515,- -32768,4018,- -32768,- -32768,4259,- -32768,838,6281,6281,6281,6281,6281,6281,4018,889,2515,- -32768,- -32768,2091,808,387,6281,- -32768,- -32768,- -32768,6281,- -32768,6671,6281,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6281,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6281,- -32768,144,- -32768,- -32768,4942,144,6281,- -32768,810,352,- -32768,- -32768,813,809,1408,- -32768,389,1492,819,3653,- -32768,816,827,- -32768,4091,4091,4091,- -32768,- -32768,3219,5037,311,- -32768,245,- -32768,- -32768,23,- -32768,- -32768,3653,- -32768,- -32768,834,- -32768,831,835,836,- -32768,3146,- -32768,255,717,- -32768,- -32768,- -32768,4018,- -32768,- -32768,- -32768,2799,- -32768,2799,- -32768,839,- -32768,- -32768,- -32768,- -32768,513,- -32768,- -32768,- -32768,654,6281,846,844,- -32768,3,294,352,352,768,6281,6281,842,852,6281,847,944,2377,857,- -32768,- -32768,- -32768,437,940,- -32768,5135,6281,2234,2653,- -32768,- -32768,1163,- -32768,- -32768,- -32768,- -32768,3653,- -32768,- -32768,4018,860,3872,- -32768,- -32768,850,- -32768,23,856,3945,774,- -32768,- -32768,- -32768,- -32768,735,69,861,726,- -32768,- -32768,1316,- -32768,- -32768,- -32768,- -32768,4018,- -32768,- -32768,858,- -32768,- -32768,- -32768,- -32768,3255,862,870,888,- -32768,- -32768,684,4749,875,881,883,884,399,880,885,228,886,887,6437,- -32768,- -32768,890,897,- -32768,687,139,645,647,653,643,175,175,339,339,339,339,194,194,263,263,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,893,- -32768,- 28,3653,4553,- -32768,955,- -32768,- -32768,901,- -32768,99,- -32768,4018,- -32768,- -32768,- -32768,913,774,915,735,877,245,3653,3726,5230,- -32768,- -32768,97,352,651,916,34,3292,918,352,3653,3580,5328,- -32768,255,- -32768,- -32768,- -32768,905,- -32768,807,- -32768,- -32768,661,6281,6281,774,- -32768,- -32768,- -32768,926,88,480,455,464,6281,791,479,4161,924,5426,5521,494,- -32768,- -32768,931,933,927,481,1948,- -32768,3426,- -32768,766,934,3653,- -32768,938,23,- -32768,935,- -32768,214,- -32768,- -32768,- -32768,- -32768,726,- -32768,943,- -32768,2799,963,- -32768,- -32768,- -32768,952,268,949,- -32768,4357,- -32768,- -32768,6281,1049,6281,6359,- -32768,- -32768,144,144,952,950,4553,- -32768,- -32768,6281,6281,- -32768,- -32768,76,696,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,951,- -32768,- -32768,735,76,953,774,735,946,- -32768,774,493,958,959,- -32768,- -32768,964,- -32768,76,966,774,967,735,965,957,- -32768,- -32768,- -32768,4018,- -32768,956,74,768,4161,976,972,1368,971,981,4161,6281,5616,540,5711,573,5806,791,- -32768,988,992,- -32768,- -32768,774,102,- -32768,- -32768,999,774,4018,- -32768,278,- -32768,- -32768,- -32768,222,- -32768,6281,- -32768,- -32768,4749,991,989,997,- -32768,838,993,995,- -32768,700,- -32768,- -32768,- -32768,- -32768,4553,- -32768,- -32768,- -32768,- -32768,996,- -32768,735,4018,1020,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,1004,- -32768,- -32768,735,- -32768,1011,4018,- -32768,- -32768,1106,791,1015,6749,1010,2653,6671,1009,- -32768,- 23,- -32768,1050,1002,697,761,242,777,323,341,- -32768,- -32768,- -32768,- -32768,1052,6671,2091,- -32768,- -32768,497,4161,500,5901,4161,504,5996,6091,580,1024,- -32768,- -32768,- -32768,- -32768,1026,- -32768,934,- -32768,1028,603,- -32768,435,184,- -32768,- -32768,4018,1126,- -32768,- -32768,- -32768,4455,- -32768,- -32768,1034,1035,- -32768,- -32768,- -32768,527,4161,1042,791,2515,- -32768,4018,1029,- -32768,1662,6671,6281,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6671,6281,- -32768,- -32768,1045,- -32768,4161,4161,538,- -32768,4161,4161,554,4161,566,6186,- -32768,23,774,- -32768,- -32768,710,- -32768,- -32768,2653,1038,1062,- -32768,- -32768,1084,- -32768,- -32768,- -32768,1071,1067,1073,6671,791,- -32768,687,212,645,647,647,643,175,175,339,339,339,339,194,194,263,263,- -32768,- -32768,- -32768,215,- -32768,- -32768,- -32768,4161,- -32768,- -32768,4161,- -32768,4161,4161,567,- -32768,- -32768,- -32768,1075,744,1070,4018,1072,- -32768,952,268,- -32768,- -32768,6281,1662,- -32768,- -32768,- -32768,- -32768,4161,- -32768,- -32768,- -32768,1076,- -32768,371,- -32768,791,- -32768,- -32768,1077,- -32768,- -32768,- -32768,1198,1205,- -32768};
# 1952
static short Cyc_yypgoto[144]={- -32768,122,- -32768,260,- -32768,- 186,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 54,- -32768,- 98,39,- -32768,- -32768,0,586,- -32768,32,- 126,1097,5,- -32768,- -32768,- 132,- -32768,121,1176,- 743,- -32768,- -32768,- -32768,954,962,442,342,- -32768,- -32768,577,- -32768,- -32768,116,- -32768,- -32768,393,- 224,1144,- 399,8,- -32768,1060,- -32768,- -32768,1171,- 394,- -32768,544,- 119,- 150,- 133,- 462,277,553,560,- 429,- 480,- 112,- 413,- 120,- -32768,- 244,- 171,- 551,- 317,- -32768,904,213,- 66,- 140,- 65,- 274,- 78,- -32768,- -32768,- 52,- 268,- -32768,- 715,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,157,1013,- -32768,704,833,- -32768,324,758,- -32768,- 166,- 293,- 154,- 356,- 326,- 316,876,- 325,- 360,- 162,- 350,- 332,- 43,656,489,899,244,- -32768,- 364,- -32768,105,515,- 62,6,- 288,- 74};
# 1974
static short Cyc_yytable[6895]={58,247,134,343,616,61,316,340,827,566,307,572,573,138,462,141,498,499,318,501,344,58,446,309,306,549,61,578,579,259,672,75,59,323,231,232,537,589,235,630,536,568,716,110,134,571,404,580,581,629,58,440,569,59,868,61,678,58,58,58,77,58,61,61,61,365,61,58,145,907,135,892,61,138,466,260,629,405,147,643,644,639,59,285,509,296,675,297,559,59,59,59,99,59,363,669,128,129,130,59,131,422,58,58,630,714,140,61,61,366,587,504,237,908,590,19,20,899,58,58,58,58,721,61,61,61,61,58,96,679,662,431,61,640,59,59,38,58,503,58,441,363,61,510,61,723,145,360,361,362,59,59,59,59,611,511,531,801,435,59,58,437,431,531,429,61,140,436,397,59,432,59,117,100,611,518,286,636,140,751,95,449,433,431,451,958,96,305,530,101,517,963,59,485,37,- 152,680,96,505,492,37,432,451,40,38,296,433,465,170,40,104,574,575,576,577,725,398,399,400,314,480,623,432,481,233,234,720,421,506,872,285,285,423,285,433,168,433,428,167,433,250,251,252,253,528,623,524,1007,- 239,218,238,- 239,254,169,448,102,265,267,239,58,401,363,373,374,61,402,403,811,37,712,255,814,475,96,222,96,549,40,325,634,223,822,319,511,58,58,758,58,43,61,61,59,61,326,1023,422,422,422,140,375,376,158,285,107,531,696,363,1027,266,168,704,399,400,871,949,527,59,59,874,59,507,96,105,286,286,324,286,169,96,915,916,379,808,305,363,380,812,363,582,583,584,- 217,1008,414,- 217,1009,749,750,42,879,600,824,103,880,401,142,58,143,439,705,403,61,42,144,108,511,917,918,875,612,617,613,42,408,238,786,109,614,792,625,408,626,239,624,37,247,451,627,59,118,119,549,39,40,41,286,773,381,739,876,37,357,666,112,788,382,383,451,39,40,41,739,196,451,120,121,798,799,665,891,469,421,421,421,377,378,423,423,423,236,615,895,656,493,428,96,111,426,419,668,494,37,664,427,113,622,138,628,667,127,40,791,715,448,363,78,58,37,58,364,921,61,122,61,922,39,40,41,114,635,539,132,727,531,531,923,158,475,328,329,729,996,160,924,925,741,58,538,59,792,59,61,363,743,728,165,58,438,1026,452,58,61,880,243,453,61,744,742,597,363,161,363,358,675,561,73,96,709,59,96,216,363,217,511,219,140,701,593,59,58,796,682,59,298,61,140,775,79,97,140,98,238,947,732,780,413,803,797,882,239,543,964,544,970,971,246,659,221,73,363,624,59,791,220,556,73,558,244,596,976,977,73,792,451,264,665,451,363,631,133,632,136,756,966,73,246,363,969,928,978,979,757,948,164,967,968,97,641,642,754,491,363,755,363,776,256,762,97,772,242,285,615,296,768,774,815,363,73,73,363,816,133,363,257,929,73,363,931,37,628,475,935,299,73,73,73,73,40,962,42,312,804,313,475,321,791,475,96,43,452,298,685,320,809,955,73,46,47,48,862,363,358,363,339,58,987,58,820,420,61,425,61,322,554,345,73,363,358,831,500,164,990,346,97,858,97,58,682,363,363,567,61,865,992,1014,363,59,347,59,940,96,426,363,286,264,140,37,427,348,46,47,48,46,47,48,40,897,788,59,123,37,134,945,419,349,946,43,591,39,40,41,97,371,372,46,47,48,350,97,733,445,1022,943,16,17,18,427,687,46,47,48,351,718,972,973,974,975,734,735,736,37,450,352,603,604,605,359,37,39,40,41,42,73,643,644,39,40,41,367,450,469,477,930,368,482,934,369,469,46,47,48,470,511,370,905,46,47,48,520,521,406,73,285,648,649,694,695,652,784,411,657,412,927,759,760,419,37,805,806,663,956,888,889,37,39,40,41,338,416,341,341,39,40,41,444,164,911,912,459,354,355,454,1021,445,455,97,341,341,341,427,913,914,285,985,986,457,58,988,989,460,991,61,919,920,96,1016,363,341,246,37,829,830,249,467,476,478,479,483,40,80,980,981,982,486,134,487,488,246,59,43,489,495,496,502,246,286,497,46,47,48,507,258,311,433,515,42,58,514,519,1010,529,61,1011,37,1012,1013,854,523,1006,533,534,39,40,41,535,541,97,80,542,97,365,595,81,469,592,560,1024,601,59,246,594,46,47,48,407,286,599,166,83,602,246,84,85,86,618,285,40,619,621,246,450,87,620,450,633,637,638,88,650,89,90,73,651,73,654,91,658,653,341,765,767,92,660,676,450,341,670,673,681,691,450,688,692,80,693,37,83,185,186,84,85,86,697,698,40,699,700,702,707,58,726,747,703,706,61,353,464,710,189,711,713,341,719,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,722,59,724,737,73,740,81,752,286,904,763,769,770,305,771,246,80,97,717,777,779,166,83,783,420,84,85,86,246,785,40,525,708,789,794,87,826,800,807,813,810,88,828,89,90,817,818,859,861,91,864,819,867,821,823,92,832,950,825,833,464,856,857,550,551,552,553,869,555,81,97,870,873,881,959,884,960,562,883,885,886,565,887,890,82,83,341,893,84,85,86,894,896,40,898,900,903,450,87,906,910,909,585,926,88,941,89,90,942,944,450,951,91,450,586,953,961,954,92,562,196,753,998,957,984,1000,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,610,933,21,1001,937,939,1002,1003,264,1004,1005,1015,1017,1030,1019,22,73,23,1025,1028,1031,997,738,24,245,106,461,27,146,1018,748,341,317,116,995,28,29,- 38,246,468,358,782,148,149,150,781,32,965,778,33,540,415,151,152,153,154,155,34,35,36,570,690,598,855,1020,983,557,562,0,0,562,264,0,0,0,0,0,0,0,994,0,0,0,0,0,246,0,0,999,97,0,38,39,0,41,42,0,0,137,0,0,258,0,0,0,0,44,45,0,0,341,341,0,133,0,0,0,0,0,384,0,0,464,0,0,0,0,0,0,0,0,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,385,386,387,388,389,390,391,392,393,394,464,0,22,0,23,0,341,0,684,0,24,0,25,26,27,0,0,341,0,0,395,731,28,29,- 38,0,0,0,0,0,0,30,31,32,746,0,33,0,0,0,0,0,834,835,34,35,36,0,0,0,0,0,0,0,0,80,0,562,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,80,0,0,0,46,47,48,464,0,0,793,0,795,81,0,133,0,0,0,0,464,0,0,802,0,0,0,0,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,902,0,0,341,836,0,89,90,0,81,0,0,91,0,0,837,0,0,92,193,0,341,194,195,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,80,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92,0,464,0,0,0,0,0,0,0,0,0,0,0,0,341,464,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,0,81,0,0,0,0,0,0,0,0,0,0,0,327,0,0,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,0,0,0,341,88,0,89,90,0,328,329,0,91,0,0,0,0,0,92,0,0,0,0,0,0,0,0,0,0,0,0,0,0,464,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,341,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,759,760,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,- 308,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,282,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,- 38,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,484,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,0,0,0,0,0,0,22,80,23,173,0,0,0,174,24,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,655,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,356,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 38,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,22,0,38,39,40,41,42,0,0,0,- 11,0,27,0,0,43,0,44,45,0,28,29,0,46,47,48,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,37,0,0,0,22,0,0,38,39,40,41,42,0,305,0,0,27,0,0,0,445,0,44,45,28,29,427,0,46,47,48,0,0,0,0,32,0,0,33,0,0,80,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,426,81,44,45,28,29,427,0,46,47,48,0,0,689,0,32,166,83,33,0,84,85,86,0,0,40,34,35,0,0,87,0,0,0,0,0,88,0,89,90,0,328,329,0,91,0,0,0,0,0,92,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,469,0,44,45,0,0,0,0,46,47,48,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,0,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,137,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,304,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,224,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,225,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,417,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,671,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,677,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,268,0,269,270,271,272,273,274,275,276,0,80,0,173,277,0,38,174,0,278,42,0,0,0,0,175,176,177,178,0,0,44,45,179,0,279,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,484,83,185,186,84,85,86,0,0,40,0,0,187,281,137,0,0,171,172,0,188,0,0,189,545,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,546,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,790,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,952,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,310,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,490,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,588,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,609,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,661,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,730,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,745,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,764,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,766,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,860,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,863,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,866,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,932,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,936,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,938,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,993,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,342,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,708,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,337,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,353,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,0,0,0,901,0,0,189,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195};
# 2667
static short Cyc_yycheck[6895]={0,113,64,174,433,0,156,173,751,365,143,371,372,67,258,67,284,285,158,287,174,21,246,143,143,342,21,377,378,127,510,108,0,165,100,101,329,401,104,452,328,367,593,43,106,370,104,379,380,448,50,17,368,21,769,50,518,57,58,59,21,61,57,58,59,92,61,67,68,92,64,814,67,127,260,127,475,135,70,5,6,78,50,137,61,139,515,139,356,57,58,59,106,61,122,508,57,58,59,67,61,227,102,103,517,133,67,102,103,136,398,297,106,136,402,20,21,832,118,119,120,121,602,118,119,120,121,127,23,523,494,86,127,130,102,103,113,137,119,139,106,122,137,120,139,607,146,190,191,192,118,119,120,121,428,305,322,708,232,127,160,235,86,329,229,160,127,122,211,137,125,139,50,124,448,308,137,470,139,641,23,246,137,86,246,900,81,119,320,124,122,906,160,271,106,121,127,92,122,277,106,125,264,115,113,259,137,259,87,115,124,373,374,375,376,609,82,83,84,128,126,445,125,129,102,103,127,227,302,127,284,285,227,287,137,106,137,229,81,137,118,119,120,121,318,469,312,962,119,92,106,122,106,124,246,119,135,136,114,259,126,122,87,88,259,131,132,729,106,130,124,733,264,168,120,170,593,115,113,465,126,743,160,433,284,285,650,287,126,284,285,259,287,128,1009,421,422,423,259,124,125,124,356,49,470,549,122,1022,131,106,82,83,84,775,130,315,284,285,780,287,106,216,126,284,285,168,287,124,223,87,88,137,726,119,122,141,730,122,381,382,383,127,130,222,130,130,639,640,117,127,416,745,119,131,126,124,356,126,237,131,132,356,117,132,105,515,124,125,781,124,436,126,117,216,106,107,126,132,695,124,223,126,114,445,106,497,448,132,356,119,120,708,114,115,116,356,664,134,622,121,106,188,500,105,692,142,143,469,114,115,116,635,88,475,119,120,704,705,496,813,126,421,422,423,85,86,421,422,423,105,430,825,486,119,426,330,43,126,225,505,126,106,496,132,126,445,500,447,500,56,115,695,592,445,122,120,456,106,458,127,137,456,59,458,141,114,115,116,120,469,123,120,612,639,640,134,124,469,131,132,613,943,120,142,143,625,486,330,456,806,458,486,122,626,613,120,496,127,127,122,500,496,131,110,127,500,626,626,119,122,108,122,188,942,127,0,411,560,486,414,126,122,106,673,119,486,127,406,496,529,701,526,500,140,529,496,669,22,23,500,25,106,107,617,677,221,712,701,792,114,337,907,339,913,914,113,119,123,43,122,622,529,806,122,351,50,353,127,411,919,920,56,889,635,132,653,638,122,456,64,458,66,127,909,69,143,122,912,856,921,922,127,880,78,910,911,81,476,477,119,276,122,122,122,670,119,127,92,127,126,664,611,666,119,666,122,122,102,103,122,127,106,122,122,127,110,122,127,106,629,622,127,119,118,119,120,121,115,906,117,126,715,126,635,121,889,638,542,126,122,257,529,122,727,127,140,134,135,136,119,122,337,122,126,664,127,666,741,226,664,228,666,123,349,126,160,122,353,756,286,165,127,126,168,762,170,686,682,122,122,366,686,119,127,127,122,664,126,666,119,595,126,122,664,262,666,106,132,126,134,135,136,134,135,136,115,828,1005,686,119,106,783,119,510,126,122,126,403,114,115,116,216,89,90,134,135,136,126,223,618,126,1009,875,17,18,19,132,534,134,135,136,126,595,915,916,917,918,106,107,108,106,246,126,421,422,423,126,106,114,115,116,117,257,5,6,114,115,116,91,264,126,266,860,138,269,863,139,126,134,135,136,130,942,140,837,134,135,136,63,64,120,286,856,479,480,121,122,483,686,106,486,106,855,22,23,602,106,121,122,495,898,121,122,106,114,115,116,171,126,173,174,114,115,116,127,320,139,140,119,183,184,121,1008,126,121,330,190,191,192,132,89,90,906,931,932,121,856,935,936,113,938,856,85,86,759,121,122,211,426,106,754,755,114,121,120,120,126,126,115,38,923,924,925,126,946,106,119,445,856,126,119,126,130,121,452,856,126,134,135,136,106,123,144,137,130,117,906,127,127,987,121,906,990,106,992,993,759,133,961,121,125,114,115,116,125,121,411,38,122,414,92,122,91,126,124,127,1014,121,906,497,127,134,135,136,103,906,127,106,107,122,508,110,111,112,120,1009,115,126,122,517,445,120,127,448,125,119,122,126,126,128,129,456,120,458,30,134,119,130,322,655,656,140,42,127,469,329,126,137,127,127,475,133,122,38,106,106,107,108,109,110,111,112,127,122,115,122,122,127,121,1009,133,106,127,127,1009,126,258,127,129,122,127,365,121,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,127,1009,127,127,529,127,91,121,1009,836,126,120,119,119,127,613,38,542,103,121,125,106,107,120,622,110,111,112,626,106,115,313,120,124,25,120,119,127,127,133,127,126,126,128,129,127,127,763,764,134,766,127,768,127,127,140,120,884,133,127,342,130,121,345,346,347,348,119,350,91,595,119,113,789,901,126,903,359,127,122,127,363,127,127,106,107,470,107,110,111,112,127,121,115,28,120,126,622,120,130,138,91,384,91,126,121,128,129,122,121,635,25,134,638,396,121,127,122,140,401,836,646,949,121,119,127,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,427,862,24,126,865,866,107,121,751,127,122,121,127,0,127,37,686,39,127,127,0,946,621,45,112,34,257,49,69,1001,638,560,157,47,942,57,58,59,781,262,901,682,63,64,65,677,68,908,673,71,331,223,73,74,75,76,77,79,80,81,369,542,414,759,1005,926,352,494,- 1,- 1,497,814,- 1,- 1,- 1,- 1,- 1,- 1,- 1,940,- 1,- 1,- 1,- 1,- 1,828,- 1,- 1,949,759,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,129,- 1,- 1,639,640,- 1,783,- 1,- 1,- 1,- 1,- 1,56,- 1,- 1,549,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,93,94,95,96,97,98,99,100,101,102,593,- 1,37,- 1,39,- 1,701,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,712,- 1,- 1,123,614,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,627,- 1,71,- 1,- 1,- 1,- 1,- 1,25,26,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,38,- 1,650,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,38,- 1,- 1,- 1,134,135,136,695,- 1,- 1,698,- 1,700,91,- 1,946,- 1,- 1,- 1,- 1,708,- 1,- 1,711,- 1,- 1,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,834,- 1,- 1,837,126,- 1,128,129,- 1,91,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,855,144,145,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,38,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,792,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,907,806,909,910,911,912,913,914,915,916,917,918,919,920,921,922,923,924,925,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,961,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,889,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1008,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,71,- 1,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,121,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,121,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,113,44,- 1,46,117,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,128,129,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,32,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4ED[8]="stdcall";static char _tmp4EE[6]="cdecl";static char _tmp4EF[9]="fastcall";static char _tmp4F0[9]="noreturn";static char _tmp4F1[6]="const";static char _tmp4F2[8]="aligned";static char _tmp4F3[7]="packed";static char _tmp4F4[7]="shared";static char _tmp4F5[7]="unused";static char _tmp4F6[5]="weak";static char _tmp4F7[10]="dllimport";static char _tmp4F8[10]="dllexport";static char _tmp4F9[23]="no_instrument_function";static char _tmp4FA[12]="constructor";static char _tmp4FB[11]="destructor";static char _tmp4FC[22]="no_check_memory_usage";static char _tmp4FD[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9DE(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9DD,unsigned int*_tmp9DC,union Cyc_YYSTYPE**_tmp9DB){for(*_tmp9DD=0;*_tmp9DD < *_tmp9DC;(*_tmp9DD)++){(*_tmp9DB)[*_tmp9DD]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9E3(unsigned int*_tmp9E2,unsigned int*_tmp9E1,struct Cyc_Yyltype**_tmp9E0){for(*_tmp9E2=0;*_tmp9E2 < *_tmp9E1;(*_tmp9E2)++){(*_tmp9E0)[*_tmp9E2]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9EA(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9E9,unsigned int*_tmp9E8,short**_tmp9E6){for(*_tmp9E9=0;*_tmp9E9 < *_tmp9E8;(*_tmp9E9)++){(*_tmp9E6)[*_tmp9E9]=(short)(
# 216
*_tmp9E9 <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9E9)): 0);}}static void _tmp9F0(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmp9EF,unsigned int*_tmp9EE,union Cyc_YYSTYPE**_tmp9EC){for(*_tmp9EF=0;*_tmp9EF < *_tmp9EE;(*_tmp9EF)++){(*_tmp9EC)[*_tmp9EF]=(union Cyc_YYSTYPE)(
# 219
*_tmp9EF <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmp9EF)):*yylval);}}static void _tmp9F6(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmp9F5,unsigned int*_tmp9F4,struct Cyc_Yyltype**_tmp9F2){for(*_tmp9F5=0;*_tmp9F5 < *_tmp9F4;(*_tmp9F5)++){(*_tmp9F2)[*_tmp9F5]=(struct Cyc_Yyltype)(
# 222
*_tmp9F5 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmp9F5)):
 Cyc_yynewloc());}}static void _tmpEE6(unsigned int*_tmpEE5,unsigned int*_tmpEE4,char**_tmpEE2){for(*_tmpEE5=0;*_tmpEE5 < *_tmpEE4;(*_tmpEE5)++){(*_tmpEE2)[*_tmpEE5]=(char)'\000';}}
# 137
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp441=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp441;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE _tmp9D7;union Cyc_YYSTYPE yylval=((_tmp9D7.YYINITIALSVAL).val=0,(((_tmp9D7.YYINITIALSVAL).tag=64,_tmp9D7)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9D9;unsigned int _tmp9D8;struct _dyneither_ptr yyss=(_tmp9D8=200,((_tmp9D9=_region_calloc(yyregion,sizeof(short),_tmp9D8),_tag_dyneither(_tmp9D9,sizeof(short),_tmp9D8))));
# 158
int yyvsp_offset;
unsigned int _tmp9DD;unsigned int _tmp9DC;union Cyc_YYSTYPE*_tmp9DB;unsigned int _tmp9DA;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9DA=(unsigned int)200,((_tmp9DB=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9DA)),((((_tmp9DC=_tmp9DA,_tmp9DE(& yylval,& _tmp9DD,& _tmp9DC,& _tmp9DB))),_tmp9DB)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9E2;unsigned int _tmp9E1;struct Cyc_Yyltype*_tmp9E0;unsigned int _tmp9DF;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9DF=(unsigned int)200,((_tmp9E0=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9DF)),((((_tmp9E1=_tmp9DF,_tmp9E3(& _tmp9E2,& _tmp9E1,& _tmp9E0))),_tmp9E0)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
{const char*_tmp9E4;Cyc_yyerror(((_tmp9E4="parser stack overflow",_tag_dyneither(_tmp9E4,sizeof(char),22))));}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9E9;unsigned int _tmp9E8;struct _dyneither_ptr _tmp9E7;short*_tmp9E6;unsigned int _tmp9E5;struct _dyneither_ptr yyss1=(_tmp9E5=(unsigned int)yystacksize,((_tmp9E6=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9E5)),((_tmp9E7=_tag_dyneither(_tmp9E6,sizeof(short),_tmp9E5),((((_tmp9E8=_tmp9E5,_tmp9EA(& yyssp_offset,& yyss,& _tmp9E9,& _tmp9E8,& _tmp9E6))),_tmp9E7)))))));
# 217
unsigned int _tmp9EF;unsigned int _tmp9EE;struct _dyneither_ptr _tmp9ED;union Cyc_YYSTYPE*_tmp9EC;unsigned int _tmp9EB;struct _dyneither_ptr yyvs1=
(_tmp9EB=(unsigned int)yystacksize,((_tmp9EC=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9EB)),((_tmp9ED=_tag_dyneither(_tmp9EC,sizeof(union Cyc_YYSTYPE),_tmp9EB),((((_tmp9EE=_tmp9EB,_tmp9F0(& yyvs,& yyssp_offset,& yylval,& _tmp9EF,& _tmp9EE,& _tmp9EC))),_tmp9ED)))))));
# 221
unsigned int _tmp9F5;unsigned int _tmp9F4;struct _dyneither_ptr _tmp9F3;struct Cyc_Yyltype*_tmp9F2;unsigned int _tmp9F1;struct _dyneither_ptr yyls1=(_tmp9F1=(unsigned int)yystacksize,((_tmp9F2=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9F1)),((_tmp9F3=_tag_dyneither(_tmp9F2,sizeof(struct Cyc_Yyltype),_tmp9F1),((((_tmp9F4=_tmp9F1,_tmp9F6(& yyssp_offset,& yyls,& _tmp9F5,& _tmp9F4,& _tmp9F2))),_tmp9F3)))))));
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 373?(int)Cyc_yytranslate[_check_known_subscript_notnull(374,yychar)]: 290;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 6894) || Cyc_yycheck[_check_known_subscript_notnull(6895,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6895,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1031){
int _tmp44F=0;_npop_handler(0);return _tmp44F;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1032,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(523,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
switch(yyn){case 1: _LL20F: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1169 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY19(yyyyvsp[0]);
# 1172
break;}case 2: _LL210: {
# 1174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1175 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp450;_push_handler(& _tmp450);{int _tmp452=0;if(setjmp(_tmp450.handler))_tmp452=1;if(!_tmp452){
x=Cyc_yyget_YY19(yyyyvsp[0]);;_pop_handler();}else{void*_tmp451=(void*)_exn_thrown;void*_tmp454=_tmp451;void*_tmp456;_LL213: {struct Cyc_Core_Failure_exn_struct*_tmp455=(struct Cyc_Core_Failure_exn_struct*)_tmp454;if(_tmp455->tag != Cyc_Core_Failure)goto _LL215;}_LL214:
# 1180
 x=0;goto _LL212;_LL215: _tmp456=_tmp454;_LL216:(void)_throw(_tmp456);_LL212:;}};}
# 1182
{struct _handler_cons _tmp457;_push_handler(& _tmp457);{int _tmp459=0;if(setjmp(_tmp457.handler))_tmp459=1;if(!_tmp459){
y=Cyc_yyget_YY19(yyyyvsp[1]);;_pop_handler();}else{void*_tmp458=(void*)_exn_thrown;void*_tmp45B=_tmp458;void*_tmp45D;_LL218: {struct Cyc_Core_Failure_exn_struct*_tmp45C=(struct Cyc_Core_Failure_exn_struct*)_tmp45B;if(_tmp45C->tag != Cyc_Core_Failure)goto _LL21A;}_LL219:
# 1186
 y=0;goto _LL217;_LL21A: _tmp45D=_tmp45B;_LL21B:(void)_throw(_tmp45D);_LL217:;}};}
# 1188
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1190
break;}case 3: _LL211: {
# 1192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1193 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA00;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmp9FF;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9FE;struct Cyc_List_List*_tmp9FD;yyval=Cyc_YY19(((_tmp9FD=_cycalloc(sizeof(*_tmp9FD)),((_tmp9FD->hd=((_tmpA00=_cycalloc(sizeof(*_tmpA00)),((_tmpA00->r=(void*)((_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=10,((_tmp9FF.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmp9FF.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmp9FF)))))),_tmp9FE)))),((_tmpA00->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA00)))))),((_tmp9FD->tl=0,_tmp9FD)))))));}
Cyc_Lex_leave_using();
# 1196
break;}case 4: _LL21C: {
# 1198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1197 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA0A;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA09;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA08;struct Cyc_List_List*_tmpA07;yyval=Cyc_YY19(((_tmpA07=_cycalloc(sizeof(*_tmpA07)),((_tmpA07->hd=((_tmpA0A=_cycalloc(sizeof(*_tmpA0A)),((_tmpA0A->r=(void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=10,((_tmpA09.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA09.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA09)))))),_tmpA08)))),((_tmpA0A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA0A)))))),((_tmpA07->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmpA07)))))));}
break;}case 5: _LL21D: {
# 1200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1200 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA19;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA18;struct _dyneither_ptr*_tmpA17;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA16;struct Cyc_List_List*_tmpA15;yyval=Cyc_YY19(((_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->hd=((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19->r=(void*)((_tmpA16=_cycalloc(sizeof(*_tmpA16)),((_tmpA16[0]=((_tmpA18.tag=9,((_tmpA18.f1=((_tmpA17=_cycalloc(sizeof(*_tmpA17)),((_tmpA17[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA17)))),((_tmpA18.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA18)))))),_tmpA16)))),((_tmpA19->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA19)))))),((_tmpA15->tl=0,_tmpA15)))))));}
Cyc_Lex_leave_namespace();
# 1203
break;}case 6: _LL21E: {
# 1205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1205 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp46B;_push_handler(& _tmp46B);{int _tmp46D=0;if(setjmp(_tmp46B.handler))_tmp46D=1;if(!_tmp46D){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp46C=(void*)_exn_thrown;void*_tmp46F=_tmp46C;void*_tmp471;_LL221: {struct Cyc_Core_Failure_exn_struct*_tmp470=(struct Cyc_Core_Failure_exn_struct*)_tmp46F;if(_tmp470->tag != Cyc_Core_Failure)goto _LL223;}_LL222:
{const char*_tmpA1A;nspace=((_tmpA1A="",_tag_dyneither(_tmpA1A,sizeof(char),1)));}goto _LL220;_LL223: _tmp471=_tmp46F;_LL224:(void)_throw(_tmp471);_LL220:;}};}
# 1210
{struct _handler_cons _tmp473;_push_handler(& _tmp473);{int _tmp475=0;if(setjmp(_tmp473.handler))_tmp475=1;if(!_tmp475){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp474=(void*)_exn_thrown;void*_tmp477=_tmp474;void*_tmp479;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp478=(struct Cyc_Core_Failure_exn_struct*)_tmp477;if(_tmp478->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp479=_tmp477;_LL229:(void)_throw(_tmp479);_LL225:;}};}
# 1213
{struct _handler_cons _tmp47A;_push_handler(& _tmp47A);{int _tmp47C=0;if(setjmp(_tmp47A.handler))_tmp47C=1;if(!_tmp47C){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp47B=(void*)_exn_thrown;void*_tmp47E=_tmp47B;void*_tmp480;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp47F=(struct Cyc_Core_Failure_exn_struct*)_tmp47E;if(_tmp47F->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp480=_tmp47E;_LL22E:(void)_throw(_tmp480);_LL22A:;}};}
# 1216
{struct Cyc_Absyn_Decl*_tmpA29;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA28;struct _dyneither_ptr*_tmpA27;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA26;struct Cyc_List_List*_tmpA25;yyval=Cyc_YY19(((_tmpA25=_cycalloc(sizeof(*_tmpA25)),((_tmpA25->hd=((_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29->r=(void*)((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26[0]=((_tmpA28.tag=9,((_tmpA28.f1=((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=nspace,_tmpA27)))),((_tmpA28.f2=x,_tmpA28)))))),_tmpA26)))),((_tmpA29->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA29)))))),((_tmpA25->tl=y,_tmpA25)))))));}
# 1218
break;}case 7: _LL21F: {
# 1220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1219 "parse.y"
struct _dyneither_ptr _tmp486=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmpA2A;if(Cyc_strcmp((struct _dyneither_ptr)_tmp486,((_tmpA2A="C",_tag_dyneither(_tmpA2A,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA34;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA33;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA32;struct Cyc_List_List*_tmpA31;yyval=Cyc_YY19(((_tmpA31=_cycalloc(sizeof(*_tmpA31)),((_tmpA31->hd=((_tmpA34=_cycalloc(sizeof(*_tmpA34)),((_tmpA34->r=(void*)((_tmpA32=_cycalloc(sizeof(*_tmpA32)),((_tmpA32[0]=((_tmpA33.tag=11,((_tmpA33.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA33)))),_tmpA32)))),((_tmpA34->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA34)))))),((_tmpA31->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA31)))))));}else{
# 1223
{const char*_tmpA35;if(Cyc_strcmp((struct _dyneither_ptr)_tmp486,((_tmpA35="C include",_tag_dyneither(_tmpA35,sizeof(char),10))))!= 0){
const char*_tmpA36;Cyc_Parse_err(((_tmpA36="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA36,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1226
struct Cyc_Absyn_Decl*_tmpA40;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA3F;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA3E;struct Cyc_List_List*_tmpA3D;yyval=Cyc_YY19(((_tmpA3D=_cycalloc(sizeof(*_tmpA3D)),((_tmpA3D->hd=((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40->r=(void*)((_tmpA3E=_cycalloc(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=12,((_tmpA3F.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA3F.f2=0,_tmpA3F)))))),_tmpA3E)))),((_tmpA40->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA40)))))),((_tmpA3D->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA3D)))))));};}}
# 1229
break;}case 8: _LL22F: {
# 1231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1230 "parse.y"
{const char*_tmpA41;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA41="C include",_tag_dyneither(_tmpA41,sizeof(char),10))))!= 0){
const char*_tmpA42;Cyc_Parse_err(((_tmpA42="expecting \"C include\"",_tag_dyneither(_tmpA42,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA4C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA4B;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA4A;struct Cyc_List_List*_tmpA49;yyval=Cyc_YY19(((_tmpA49=_cycalloc(sizeof(*_tmpA49)),((_tmpA49->hd=((_tmpA4C=_cycalloc(sizeof(*_tmpA4C)),((_tmpA4C->r=(void*)((_tmpA4A=_cycalloc(sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=12,((_tmpA4B.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA4B.f2=exs,_tmpA4B)))))),_tmpA4A)))),((_tmpA4C->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4C)))))),((_tmpA49->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA49)))))));}
# 1235
break;};}case 9: _LL230: {
# 1237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1236 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA4F;struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY19(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA4F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA4F)))))),((_tmpA4E->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA4E)))))));}
break;}case 10: _LL231: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1238 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA52;struct Cyc_List_List*_tmpA51;yyval=Cyc_YY19(((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->hd=((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA52->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA52)))))),((_tmpA51->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA51)))))));}
break;}case 11: _LL232:
# 1241
 yyval=Cyc_YY19(0);
break;case 12: _LL233: {
# 1244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1243 "parse.y"
yyval=yyyyvsp[2];
break;}case 13: _LL234: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1244 "parse.y"
yyval=Cyc_YY55(0);
break;}case 14: _LL235: {
# 1247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1248 "parse.y"
{struct _tuple27*_tmpA55;struct Cyc_List_List*_tmpA54;yyval=Cyc_YY55(((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54->hd=((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA55->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA55->f3=0,_tmpA55)))))))),((_tmpA54->tl=0,_tmpA54)))))));}
break;}case 15: _LL236: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1249 "parse.y"
{struct _tuple27*_tmpA58;struct Cyc_List_List*_tmpA57;yyval=Cyc_YY55(((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57->hd=((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA58->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA58->f3=0,_tmpA58)))))))),((_tmpA57->tl=0,_tmpA57)))))));}
break;}case 16: _LL237: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1251 "parse.y"
{struct _tuple27*_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY55(((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA5B->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA5B->f3=0,_tmpA5B)))))))),((_tmpA5A->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA5A)))))));}
break;}case 17: _LL238: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1255 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA61;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA60;struct Cyc_List_List*_tmpA5F;yyval=Cyc_YY19(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=Cyc_Absyn_new_decl((void*)((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA60.tag=1,((_tmpA60.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA60)))),_tmpA61)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA5F->tl=0,_tmpA5F)))))));}
break;}case 18: _LL239: {
# 1258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1256 "parse.y"
yyval=yyyyvsp[0];
break;}case 19: _LL23A: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1257 "parse.y"
yyval=Cyc_YY19(0);
break;}case 22: _LL23B: {
# 1260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1266 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),0,Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 23: _LL23C: {
# 1269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1268 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A5=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A5,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 24: _LL23D: {
# 1272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1281 "parse.y"
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,0,Cyc_yyget_YY30(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1]),Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 25: _LL23E: {
# 1284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1283 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A6=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A6,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 26: _LL23F: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1291 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A7=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A7,Cyc_yyget_YY30(yyyyvsp[1]),0,Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 27: _LL240: {
# 1295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1294 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4A8=Cyc_yyget_YY20(yyyyvsp[0]);
yyval=Cyc_YY18(Cyc_Parse_make_function(yyr,& _tmp4A8,Cyc_yyget_YY30(yyyyvsp[1]),Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 28: _LL241: {
# 1298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1299 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
break;}case 29: _LL242: {
# 1302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1302 "parse.y"
Cyc_Lex_leave_using();
break;}case 30: _LL243: {
# 1305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1305 "parse.y"
{struct _dyneither_ptr*_tmpA62;Cyc_Lex_enter_namespace(((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA62)))));}yyval=yyyyvsp[1];
break;}case 31: _LL244: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1308 "parse.y"
Cyc_Lex_leave_namespace();
break;}case 32: _LL245: {
# 1311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
int _tmp4AA=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),0,(unsigned int)_tmp4AA,(unsigned int)_tmp4AA));
break;}case 33: _LL246: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1317 "parse.y"
int _tmp4AB=(yyyylsp[0]).first_line;
yyval=Cyc_YY19(Cyc_Parse_make_declarations(Cyc_yyget_YY20(yyyyvsp[0]),Cyc_yyget_YY22(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line,(unsigned int)_tmp4AB));
break;}case 34: _LL247: {
# 1321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1321 "parse.y"
{struct Cyc_List_List*_tmpA63;yyval=Cyc_YY19(((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA63->tl=0,_tmpA63)))))));}
break;}case 35: _LL248: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323 "parse.y"
struct Cyc_List_List*_tmp4AD=0;
{struct Cyc_List_List*_tmp4AE=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp4AE != 0;_tmp4AE=_tmp4AE->tl){
struct _dyneither_ptr*_tmp4AF=(struct _dyneither_ptr*)_tmp4AE->hd;
struct _tuple0*_tmpA64;struct _tuple0*qv=(_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64->f1=Cyc_Absyn_Rel_n(0),((_tmpA64->f2=_tmp4AF,_tmpA64)))));
struct Cyc_Absyn_Vardecl*_tmp4B0=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA65;_tmp4AD=((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=_tmp4B0,((_tmpA65->tl=_tmp4AD,_tmpA65))))));}}
# 1330
_tmp4AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AD);
{struct Cyc_List_List*_tmpA66;yyval=Cyc_YY19(((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->hd=Cyc_Absyn_letv_decl(_tmp4AD,(unsigned int)(yyyylsp[0]).first_line),((_tmpA66->tl=0,_tmpA66)))))));}
# 1333
break;}case 36: _LL249: {
# 1335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336 "parse.y"
struct _dyneither_ptr _tmp4B4=Cyc_yyget_String_tok(yyyyvsp[3]);
{const char*_tmpA67;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA67="`H",_tag_dyneither(_tmpA67,sizeof(char),3))))== 0){
const char*_tmpA6A;void*_tmpA69;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA69=0,Cyc_aprintf(((_tmpA6A="bad occurrence of heap region",_tag_dyneither(_tmpA6A,sizeof(char),30))),_tag_dyneither(_tmpA69,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA6B;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA6B="`U",_tag_dyneither(_tmpA6B,sizeof(char),3))))== 0){
const char*_tmpA6E;void*_tmpA6D;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA6D=0,Cyc_aprintf(((_tmpA6E="bad occurrence of unique region",_tag_dyneither(_tmpA6E,sizeof(char),32))),_tag_dyneither(_tmpA6D,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA71;struct Cyc_Absyn_Tvar*_tmpA70;struct Cyc_Absyn_Tvar*tv=(_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->name=((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=_tmp4B4,_tmpA71)))),((_tmpA70->identity=- 1,((_tmpA70->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA70)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA74;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA73;void*t=(void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=2,((_tmpA74.f1=tv,_tmpA74)))),_tmpA73))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA7A;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA79;struct _dyneither_ptr*_tmpA77;struct _tuple0*_tmpA76;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->f1=Cyc_Absyn_Loc_n,((_tmpA76->f2=((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA77)))),_tmpA76)))))),(void*)((_tmpA79=_cycalloc(sizeof(*_tmpA79)),((_tmpA79[0]=((_tmpA7A.tag=15,((_tmpA7A.f1=t,_tmpA7A)))),_tmpA79)))),0);
{struct Cyc_List_List*_tmpA7B;yyval=Cyc_YY19(((_tmpA7B=_cycalloc(sizeof(*_tmpA7B)),((_tmpA7B->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA7B->tl=0,_tmpA7B)))))));}
# 1346
break;};}case 37: _LL24A: {
# 1348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1348 "parse.y"
int _tmp4C4=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4C5=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpA7C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C5,((_tmpA7C="H",_tag_dyneither(_tmpA7C,sizeof(char),2))))== 0){
const char*_tmpA7D;Cyc_Parse_err(((_tmpA7D="bad occurrence of heap region `H",_tag_dyneither(_tmpA7D,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA7E;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C5,((_tmpA7E="U",_tag_dyneither(_tmpA7E,sizeof(char),2))))== 0){
const char*_tmpA7F;Cyc_Parse_err(((_tmpA7F="bad occurrence of unique region `U",_tag_dyneither(_tmpA7F,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpA8C;const char*_tmpA8B;void*_tmpA8A[1];struct Cyc_String_pa_PrintArg_struct _tmpA89;struct Cyc_Absyn_Tvar*_tmpA88;struct Cyc_Absyn_Tvar*tv=(_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88->name=((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C[0]=(struct _dyneither_ptr)((_tmpA89.tag=0,((_tmpA89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C5),((_tmpA8A[0]=& _tmpA89,Cyc_aprintf(((_tmpA8B="`%s",_tag_dyneither(_tmpA8B,sizeof(char),4))),_tag_dyneither(_tmpA8A,sizeof(void*),1)))))))),_tmpA8C)))),((_tmpA88->identity=- 1,((_tmpA88->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA88)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA8F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA8E;void*t=(void*)((_tmpA8E=_cycalloc(sizeof(*_tmpA8E)),((_tmpA8E[0]=((_tmpA8F.tag=2,((_tmpA8F.f1=tv,_tmpA8F)))),_tmpA8E))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA95;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA94;struct _dyneither_ptr*_tmpA92;struct _tuple0*_tmpA91;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA91=_cycalloc(sizeof(*_tmpA91)),((_tmpA91->f1=Cyc_Absyn_Loc_n,((_tmpA91->f2=((_tmpA92=_cycalloc(sizeof(*_tmpA92)),((_tmpA92[0]=_tmp4C5,_tmpA92)))),_tmpA91)))))),(void*)((_tmpA94=_cycalloc(sizeof(*_tmpA94)),((_tmpA94[0]=((_tmpA95.tag=15,((_tmpA95.f1=t,_tmpA95)))),_tmpA94)))),0);
# 1359
{struct Cyc_List_List*_tmpA96;yyval=Cyc_YY19(((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4C4,(unsigned int)(yyyylsp[0]).first_line),((_tmpA96->tl=0,_tmpA96)))))));}
# 1361
break;};}case 38: _LL24B:
# 1363
 yyval=Cyc_YY34(0);
break;case 39: _LL24C: {
# 1366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1365 "parse.y"
yyval=Cyc_YY34(1);
break;}case 40: _LL24D: {
# 1368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1370 "parse.y"
yyval=yyyyvsp[0];
break;}case 41: _LL24E: {
# 1373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1372 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 42: _LL24F: {
# 1375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpA97;yyval=Cyc_YY20(((_tmpA97.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA97.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA97.type_specs=
Cyc_Parse_empty_spec(0),((_tmpA97.is_inline=0,((_tmpA97.attributes=0,_tmpA97)))))))))));}
break;}case 43: _LL250: {
# 1382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1381 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4D7=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4D7.sc != 0){
const char*_tmpA9A;void*_tmpA99;(_tmpA99=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpA9A="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpA9A,sizeof(char),51))),_tag_dyneither(_tmpA99,sizeof(void*),0)));}
# 1385
{struct Cyc_Parse_Declaration_spec _tmpA9B;yyval=Cyc_YY20(((_tmpA9B.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpA9B.tq=_tmp4D7.tq,((_tmpA9B.type_specs=_tmp4D7.type_specs,((_tmpA9B.is_inline=_tmp4D7.is_inline,((_tmpA9B.attributes=_tmp4D7.attributes,_tmpA9B)))))))))));}
# 1389
break;}case 44: _LL251: {
# 1391
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1390 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA9C;yyval=Cyc_YY20(((_tmpA9C.sc=0,((_tmpA9C.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpA9C.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpA9C.is_inline=0,((_tmpA9C.attributes=0,_tmpA9C)))))))))));}
break;}case 45: _LL252: {
# 1394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1393 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4DC=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA9D;yyval=Cyc_YY20(((_tmpA9D.sc=_tmp4DC.sc,((_tmpA9D.tq=_tmp4DC.tq,((_tmpA9D.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4DC.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpA9D.is_inline=_tmp4DC.is_inline,((_tmpA9D.attributes=_tmp4DC.attributes,_tmpA9D)))))))))));}
# 1399
break;}case 46: _LL253: {
# 1401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1400 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpA9E;yyval=Cyc_YY20(((_tmpA9E.sc=0,((_tmpA9E.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpA9E.type_specs=Cyc_Parse_empty_spec(0),((_tmpA9E.is_inline=0,((_tmpA9E.attributes=0,_tmpA9E)))))))))));}
break;}case 47: _LL254: {
# 1403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1402 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4DF=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpA9F;yyval=Cyc_YY20(((_tmpA9F.sc=_tmp4DF.sc,((_tmpA9F.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4DF.tq),((_tmpA9F.type_specs=_tmp4DF.type_specs,((_tmpA9F.is_inline=_tmp4DF.is_inline,((_tmpA9F.attributes=_tmp4DF.attributes,_tmpA9F)))))))))));}
# 1407
break;}case 48: _LL255: {
# 1409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1408 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA0;yyval=Cyc_YY20(((_tmpAA0.sc=0,((_tmpAA0.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA0.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAA0.is_inline=1,((_tmpAA0.attributes=0,_tmpAA0)))))))))));}
break;}case 49: _LL256: {
# 1412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1411 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E2=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA1;yyval=Cyc_YY20(((_tmpAA1.sc=_tmp4E2.sc,((_tmpAA1.tq=_tmp4E2.tq,((_tmpAA1.type_specs=_tmp4E2.type_specs,((_tmpAA1.is_inline=1,((_tmpAA1.attributes=_tmp4E2.attributes,_tmpAA1)))))))))));}
# 1415
break;}case 50: _LL257: {
# 1417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1416 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAA2;yyval=Cyc_YY20(((_tmpAA2.sc=0,((_tmpAA2.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAA2.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAA2.is_inline=0,((_tmpAA2.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpAA2)))))))))));}
break;}case 51: _LL258: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1419 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E5=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAA3;yyval=Cyc_YY20(((_tmpAA3.sc=_tmp4E5.sc,((_tmpAA3.tq=_tmp4E5.tq,((_tmpAA3.type_specs=_tmp4E5.type_specs,((_tmpAA3.is_inline=_tmp4E5.is_inline,((_tmpAA3.attributes=
# 1422
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4E5.attributes),_tmpAA3)))))))))));}
break;}case 52: _LL259: {
# 1425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1426 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 53: _LL25A: {
# 1430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1428 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 54: _LL25B: {
# 1432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1430 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 55: _LL25C: {
# 1434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1432 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 56: _LL25D: {
# 1436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1435 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpAA4;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpAA4="C",_tag_dyneither(_tmpAA4,sizeof(char),2))))!= 0){
const char*_tmpAA5;Cyc_Parse_err(((_tmpAA5="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpAA5,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1440
break;}case 57: _LL25E: {
# 1442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1440 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 58: _LL25F: {
# 1444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1443 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 59: _LL260:
# 1447
 yyval=Cyc_YY48(0);
break;case 60: _LL261: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1450 "parse.y"
yyval=yyyyvsp[0];
break;}case 61: _LL262: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1455 "parse.y"
yyval=yyyyvsp[3];
break;}case 62: _LL263: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1459 "parse.y"
{struct Cyc_List_List*_tmpAA6;yyval=Cyc_YY48(((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAA6->tl=0,_tmpAA6)))))));}
break;}case 63: _LL264: {
# 1462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1460 "parse.y"
{struct Cyc_List_List*_tmpAA7;yyval=Cyc_YY48(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAA7->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAA7)))))));}
break;}case 64: _LL265: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1465 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4ED,_tmp4ED,_tmp4ED + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4EE,_tmp4EE,_tmp4EE + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp4EF,_tmp4EF,_tmp4EF + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp4F0,_tmp4F0,_tmp4F0 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp4F1,_tmp4F1,_tmp4F1 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp4F2,_tmp4F2,_tmp4F2 + 8},(void*)& att_aligned},{{_tmp4F3,_tmp4F3,_tmp4F3 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp4F4,_tmp4F4,_tmp4F4 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp4F5,_tmp4F5,_tmp4F5 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp4F6,_tmp4F6,_tmp4F6 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp4F7,_tmp4F7,_tmp4F7 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp4F8,_tmp4F8,_tmp4F8 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp4F9,_tmp4F9,_tmp4F9 + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp4FA,_tmp4FA,_tmp4FA + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp4FB,_tmp4FB,_tmp4FB + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp4FC,_tmp4FC,_tmp4FC + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp4FD,_tmp4FD,_tmp4FD + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1485
struct _dyneither_ptr _tmp4EB=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1487
if((((_get_dyneither_size(_tmp4EB,sizeof(char))> 4  && ((const char*)_tmp4EB.curr)[0]== '_') && ((const char*)_tmp4EB.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4EB,sizeof(char),(int)(_get_dyneither_size(_tmp4EB,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4EB,sizeof(char),(int)(_get_dyneither_size(_tmp4EB,sizeof(char))- 3)))== '_')
# 1489
_tmp4EB=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4EB,2,_get_dyneither_size(_tmp4EB,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4EB,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1496
if(i == 17){
{const char*_tmpAA8;Cyc_Parse_err(((_tmpAA8="unrecognized attribute",_tag_dyneither(_tmpAA8,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1501
break;};}case 65: _LL266: {
# 1503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1501 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 66: _LL267: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1503 "parse.y"
struct _dyneither_ptr _tmp4FF=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _tmp500=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp502;struct _tuple5 _tmp501=_tmp500;_tmp502=_tmp501.f2;{
void*a;
{const char*_tmpAAA;const char*_tmpAA9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAAA="regparm",_tag_dyneither(_tmpAAA,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAA9="__regparm__",_tag_dyneither(_tmpAA9,sizeof(char),12))))== 0){
if(_tmp502 < 0  || _tmp502 > 3){
const char*_tmpAAB;Cyc_Parse_err(((_tmpAAB="regparm requires value between 0 and 3",_tag_dyneither(_tmpAAB,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAAE;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAAD;a=(void*)((_tmpAAD=_cycalloc_atomic(sizeof(*_tmpAAD)),((_tmpAAD[0]=((_tmpAAE.tag=0,((_tmpAAE.f1=_tmp502,_tmpAAE)))),_tmpAAD))));};}else{
const char*_tmpAB0;const char*_tmpAAF;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAB0="aligned",_tag_dyneither(_tmpAB0,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAAF="__aligned__",_tag_dyneither(_tmpAAF,sizeof(char),12))))== 0){
if(_tmp502 < 0){const char*_tmpAB1;Cyc_Parse_err(((_tmpAB1="aligned requires positive power of two",_tag_dyneither(_tmpAB1,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp502;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpAB2;Cyc_Parse_err(((_tmpAB2="aligned requires positive power of two",_tag_dyneither(_tmpAB2,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAB5;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAB4;a=(void*)((_tmpAB4=_cycalloc_atomic(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB5.tag=6,((_tmpAB5.f1=_tmp502,_tmpAB5)))),_tmpAB4))));};};}else{
const char*_tmpAB7;const char*_tmpAB6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAB7="initializes",_tag_dyneither(_tmpAB7,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAB6="__initializes__",_tag_dyneither(_tmpAB6,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpABA;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpAB9;a=(void*)((_tmpAB9=_cycalloc_atomic(sizeof(*_tmpAB9)),((_tmpAB9[0]=((_tmpABA.tag=20,((_tmpABA.f1=_tmp502,_tmpABA)))),_tmpAB9))));}else{
const char*_tmpABC;const char*_tmpABB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpABC="noliveunique",_tag_dyneither(_tmpABC,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpABB="__noliveunique__",_tag_dyneither(_tmpABB,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpABF;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpABE;a=(void*)((_tmpABE=_cycalloc_atomic(sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABF.tag=21,((_tmpABF.f1=_tmp502,_tmpABF)))),_tmpABE))));}else{
const char*_tmpAC1;const char*_tmpAC0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAC1="noconsume",_tag_dyneither(_tmpAC1,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp4FF,((_tmpAC0="__noconsume__",_tag_dyneither(_tmpAC0,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAC4;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAC3;a=(void*)((_tmpAC3=_cycalloc_atomic(sizeof(*_tmpAC3)),((_tmpAC3[0]=((_tmpAC4.tag=22,((_tmpAC4.f1=_tmp502,_tmpAC4)))),_tmpAC3))));}else{
# 1525
{const char*_tmpAC5;Cyc_Parse_err(((_tmpAC5="unrecognized attribute",_tag_dyneither(_tmpAC5,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1528
yyval=Cyc_YY49(a);
# 1530
break;};}case 67: _LL268: {
# 1532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1531 "parse.y"
struct _dyneither_ptr _tmp51B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp51C=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAC7;const char*_tmpAC6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAC7="section",_tag_dyneither(_tmpAC7,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp51B,((_tmpAC6="__section__",_tag_dyneither(_tmpAC6,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpACA;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAC9;a=(void*)((_tmpAC9=_cycalloc(sizeof(*_tmpAC9)),((_tmpAC9[0]=((_tmpACA.tag=8,((_tmpACA.f1=_tmp51C,_tmpACA)))),_tmpAC9))));}else{
# 1537
{const char*_tmpACB;Cyc_Parse_err(((_tmpACB="unrecognized attribute",_tag_dyneither(_tmpACB,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1540
yyval=Cyc_YY49(a);
# 1542
break;}case 68: _LL269: {
# 1544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1543 "parse.y"
struct _dyneither_ptr _tmp522=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp523=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1547
{const char*_tmpACC;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp522,((_tmpACC="__mode__",_tag_dyneither(_tmpACC,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpACF;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpACE;a=(void*)((_tmpACE=_cycalloc(sizeof(*_tmpACE)),((_tmpACE[0]=((_tmpACF.tag=24,((_tmpACF.f1=_tmp523,_tmpACF)))),_tmpACE))));}else{
# 1550
{const char*_tmpAD0;Cyc_Parse_err(((_tmpAD0="unrecognized attribute",_tag_dyneither(_tmpAD0,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1553
yyval=Cyc_YY49(a);
# 1555
break;}case 69: _LL26A: {
# 1557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1556 "parse.y"
struct _dyneither_ptr _tmp528=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp529=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _tmp52A=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp52C;struct _tuple5 _tmp52B=_tmp52A;_tmp52C=_tmp52B.f2;{
struct _tuple5 _tmp52D=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp52F;struct _tuple5 _tmp52E=_tmp52D;_tmp52F=_tmp52E.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAD2;const char*_tmpAD1;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp528,((_tmpAD2="format",_tag_dyneither(_tmpAD2,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp528,((_tmpAD1="__format__",_tag_dyneither(_tmpAD1,sizeof(char),11))))== 0){
const char*_tmpAD3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp529,((_tmpAD3="printf",_tag_dyneither(_tmpAD3,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpAD6;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAD5;a=(void*)((_tmpAD5=_cycalloc_atomic(sizeof(*_tmpAD5)),((_tmpAD5[0]=((_tmpAD6.tag=19,((_tmpAD6.f1=Cyc_Absyn_Printf_ft,((_tmpAD6.f2=_tmp52C,((_tmpAD6.f3=_tmp52F,_tmpAD6)))))))),_tmpAD5))));}else{
const char*_tmpAD7;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp529,((_tmpAD7="scanf",_tag_dyneither(_tmpAD7,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpADA;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAD9;a=(void*)((_tmpAD9=_cycalloc_atomic(sizeof(*_tmpAD9)),((_tmpAD9[0]=((_tmpADA.tag=19,((_tmpADA.f1=Cyc_Absyn_Scanf_ft,((_tmpADA.f2=_tmp52C,((_tmpADA.f3=_tmp52F,_tmpADA)))))))),_tmpAD9))));}else{
# 1567
const char*_tmpADB;Cyc_Parse_err(((_tmpADB="unrecognized format type",_tag_dyneither(_tmpADB,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1569
const char*_tmpADC;Cyc_Parse_err(((_tmpADC="unrecognized attribute",_tag_dyneither(_tmpADC,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1572
break;};};}case 70: _LL26B: {
# 1574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1581 "parse.y"
yyval=yyyyvsp[0];
break;}case 71: _LL26C: {
# 1584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1583 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpADF;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpADE;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE[0]=((_tmpADF.tag=17,((_tmpADF.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpADF.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpADF.f3=0,((_tmpADF.f4=0,_tmpADF)))))))))),_tmpADE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 72: _LL26D: {
# 1586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1587 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 73: _LL26E: {
# 1590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1588 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL26F: {
# 1591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1590 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL270: {
# 1593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1591 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL271: {
# 1594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1592 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL272: {
# 1595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1593 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL273: {
# 1596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1594 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL274: {
# 1597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1595 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL275: {
# 1598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1596 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL276: {
# 1599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1597 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL277: {
# 1600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1598 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL278: {
# 1601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1599 "parse.y"
yyval=yyyyvsp[0];
break;}case 84: _LL279: {
# 1602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1600 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27A: {
# 1603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1602 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL27B: {
# 1605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1604 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 87: _LL27C: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1607 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpAE2;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAE1;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1[0]=((_tmpAE2.tag=10,((_tmpAE2.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpAE2)))),_tmpAE1)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1610
break;}case 88: _LL27D: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1611 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAE5;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAE4;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4[0]=((_tmpAE5.tag=15,((_tmpAE5.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAE5)))),_tmpAE4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 89: _LL27E: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAE8;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAE7;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7[0]=((_tmpAE8.tag=15,((_tmpAE8.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpAE8)))),_tmpAE7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1615
break;}case 90: _LL27F: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1616 "parse.y"
void*_tmp542=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAEB;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAEA;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=((_tmpAEB.tag=16,((_tmpAEB.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAEB.f2=_tmp542,_tmpAEB)))))),_tmpAEA)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1619
break;}case 91: _LL280: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1620 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAED;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAED=_cycalloc(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=16,((_tmpAEE.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpAEE.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpAEE)))))),_tmpAED)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 92: _LL281: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1622 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAF1;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF0;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF1.tag=19,((_tmpAF1.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpAF1)))),_tmpAF0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL282: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1624 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpAF4;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAF3;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=19,((_tmpAF4.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpAF4)))),_tmpAF3)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL283: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1626 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpAF7;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAF6;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=18,((_tmpAF7.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpAF7)))),_tmpAF6)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL284: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1631 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 96: _LL285: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1635 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpAF8;yyval=Cyc_YY26(((_tmpAF8.print_const=1,((_tmpAF8.q_volatile=0,((_tmpAF8.q_restrict=0,((_tmpAF8.real_const=1,((_tmpAF8.loc=loc,_tmpAF8)))))))))));}
break;}case 97: _LL286: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1637 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAF9;yyval=Cyc_YY26(((_tmpAF9.print_const=0,((_tmpAF9.q_volatile=1,((_tmpAF9.q_restrict=0,((_tmpAF9.real_const=0,((_tmpAF9.loc=0,_tmpAF9)))))))))));}
break;}case 98: _LL287: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1638 "parse.y"
{struct Cyc_Absyn_Tqual _tmpAFA;yyval=Cyc_YY26(((_tmpAFA.print_const=0,((_tmpAFA.q_volatile=0,((_tmpAFA.q_restrict=1,((_tmpAFA.real_const=0,((_tmpAFA.loc=0,_tmpAFA)))))))))));}
break;}case 99: _LL288: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB09;struct Cyc_Absyn_Enumdecl*_tmpB08;struct Cyc_Core_Opt*_tmpB07;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB06;struct Cyc_Absyn_TypeDecl*_tmpB05;struct Cyc_Absyn_TypeDecl*_tmp550=(_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05->r=(void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB06.tag=1,((_tmpB06.f1=((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08->sc=Cyc_Absyn_Public,((_tmpB08->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB08->fields=((_tmpB07=_cycalloc(sizeof(*_tmpB07)),((_tmpB07->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB07)))),_tmpB08)))))))),_tmpB06)))),_tmpB09)))),((_tmpB05->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB05)))));
# 1646
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB0C;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB0B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0C.tag=26,((_tmpB0C.f1=_tmp550,((_tmpB0C.f2=0,_tmpB0C)))))),_tmpB0B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1648
break;}case 100: _LL289: {
# 1650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1649 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB0F;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB0E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=13,((_tmpB0F.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB0F.f2=0,_tmpB0F)))))),_tmpB0E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 101: _LL28A: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1651 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB12;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB11;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=((_tmpB12.tag=14,((_tmpB12.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB12)))),_tmpB11)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL28B: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB13;yyval=Cyc_YY50(((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB13->tag=0,((_tmpB13->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB13)))))))));}
break;}case 103: _LL28C: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1659 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB14;yyval=Cyc_YY50(((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB14->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB14->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB14)))))))));}
break;}case 104: _LL28D: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1663 "parse.y"
{struct Cyc_List_List*_tmpB15;yyval=Cyc_YY51(((_tmpB15=_cycalloc(sizeof(*_tmpB15)),((_tmpB15->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB15->tl=0,_tmpB15)))))));}
break;}case 105: _LL28E: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1664 "parse.y"
{struct Cyc_List_List*_tmpB16;yyval=Cyc_YY51(((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB16->tl=0,_tmpB16)))))));}
break;}case 106: _LL28F: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1665 "parse.y"
{struct Cyc_List_List*_tmpB17;yyval=Cyc_YY51(((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB17->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB17)))))));}
break;}case 107: _LL290: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB1A;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB19;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1A.tag=12,((_tmpB1A.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB1A.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpB1A)))))),_tmpB19)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 108: _LL291: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1677 "parse.y"
struct Cyc_List_List*_tmp563=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp565=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp563,
Cyc_Absyn_aggrdecl_impl(_tmp564,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1682
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB1D;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1C;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1C=_cycalloc(sizeof(*_tmpB1C)),((_tmpB1C[0]=((_tmpB1D.tag=26,((_tmpB1D.f1=_tmp565,((_tmpB1D.f2=0,_tmpB1D)))))),_tmpB1C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1684
break;}case 109: _LL292: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1688 "parse.y"
struct Cyc_List_List*_tmp568=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp569=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp56A=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp568,
Cyc_Absyn_aggrdecl_impl(_tmp569,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1693
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB20;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB1F;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F[0]=((_tmpB20.tag=26,((_tmpB20.f1=_tmp56A,((_tmpB20.f2=0,_tmpB20)))))),_tmpB1F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1695
break;}case 110: _LL293: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1696 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB2A;struct Cyc_Core_Opt*_tmpB29;struct Cyc_Absyn_AggrInfo _tmpB28;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB27;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB27=_cycalloc(sizeof(*_tmpB27)),((_tmpB27[0]=((_tmpB2A.tag=11,((_tmpB2A.f1=((_tmpB28.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB29=_cycalloc_atomic(sizeof(*_tmpB29)),((_tmpB29->v=(void*)1,_tmpB29))))),((_tmpB28.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB28)))),_tmpB2A)))),_tmpB27)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1699
break;}case 111: _LL294: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1700 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB30;struct Cyc_Absyn_AggrInfo _tmpB2F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB2E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E[0]=((_tmpB30.tag=11,((_tmpB30.f1=((_tmpB2F.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB2F.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB2F)))),_tmpB30)))),_tmpB2E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1703
break;}case 112: _LL295:
# 1705
 yyval=Cyc_YY43(0);
break;case 113: _LL296: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1709 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 114: _LL297: {
# 1712
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1714
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1713 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 115: _LL298: {
# 1716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1714 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 116: _LL299:
# 1717
 yyval=Cyc_YY27(0);
break;case 117: _LL29A: {
# 1720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1721 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp574=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp574 != 0;_tmp574=_tmp574->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp574->hd,decls);}}{
# 1725
struct Cyc_List_List*_tmp575=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp575 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp575,decls);
yyval=Cyc_YY27(decls);
# 1730
break;};}case 118: _LL29B: {
# 1732
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1734
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB31;yyval=Cyc_YY28(((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB31->tl=0,_tmpB31)))))));}
break;}case 119: _LL29C: {
# 1738
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1740
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1737 "parse.y"
{struct Cyc_List_List*_tmpB32;yyval=Cyc_YY28(((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB32->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB32)))))));}
break;}case 120: _LL29D: {
# 1740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1741 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 121: _LL29E: {
# 1744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB33;yyval=Cyc_YY22(((_tmpB33=_region_malloc(yyr,sizeof(*_tmpB33)),((_tmpB33->tl=0,((_tmpB33->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB33)))))));}
break;}case 122: _LL29F: {
# 1750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1749 "parse.y"
{struct _tuple12*_tmpB34;yyval=Cyc_YY22(((_tmpB34=_region_malloc(yyr,sizeof(*_tmpB34)),((_tmpB34->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB34->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB34)))))));}
break;}case 123: _LL2A0: {
# 1752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1754 "parse.y"
{struct _tuple11 _tmpB35;yyval=Cyc_YY21(((_tmpB35.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB35.f2=0,_tmpB35)))));}
break;}case 124: _LL2A1: {
# 1757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1756 "parse.y"
{struct _tuple11 _tmpB36;yyval=Cyc_YY21(((_tmpB36.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB36.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB36)))));}
break;}case 125: _LL2A2: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1762 "parse.y"
struct _RegionHandle _tmp57C=_new_region("temp");struct _RegionHandle*temp=& _tmp57C;_push_region(temp);{
struct _tuple25 _tmp57D=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp57F;struct Cyc_Parse_Type_specifier _tmp580;struct Cyc_List_List*_tmp581;struct _tuple25 _tmp57E=_tmp57D;_tmp57F=_tmp57E.f1;_tmp580=_tmp57E.f2;_tmp581=_tmp57E.f3;
if(_tmp57F.loc == 0)_tmp57F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp582=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp582 != 0;_tmp582=_tmp582->tl){
struct _tuple24*_tmp583=(struct _tuple24*)_tmp582->hd;struct Cyc_Parse_Declarator _tmp585;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Exp*_tmp587;struct _tuple24*_tmp584=_tmp583;_tmp585=_tmp584->f1;_tmp586=_tmp584->f2;_tmp587=_tmp584->f3;
{struct _tuple10*_tmpB37;decls=((_tmpB37=_region_malloc(temp,sizeof(*_tmpB37)),((_tmpB37->tl=decls,((_tmpB37->hd=_tmp585,_tmpB37))))));}{
struct _tuple16*_tmpB3A;struct Cyc_List_List*_tmpB39;widths_and_reqs=(
(_tmpB39=_region_malloc(temp,sizeof(*_tmpB39)),((_tmpB39->hd=((_tmpB3A=_region_malloc(temp,sizeof(*_tmpB3A)),((_tmpB3A->f1=_tmp586,((_tmpB3A->f2=_tmp587,_tmpB3A)))))),((_tmpB39->tl=widths_and_reqs,_tmpB39))))));};}}
# 1773
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp58B=Cyc_Parse_speclist2typ(_tmp580,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp58C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp57F,_tmp58B,decls,_tmp581),widths_and_reqs);
# 1779
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp58C));};};}
# 1782
_npop_handler(0);break;
# 1762
;_pop_region(temp);}case 126: _LL2A3: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1790 "parse.y"
{struct _tuple25 _tmpB3B;yyval=Cyc_YY38(((_tmpB3B.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB3B.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB3B.f3=0,_tmpB3B)))))));}
break;}case 127: _LL2A4: {
# 1793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1792 "parse.y"
struct _tuple25 _tmp58E=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB3C;yyval=Cyc_YY38(((_tmpB3C.f1=_tmp58E.f1,((_tmpB3C.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp58E.f2),((_tmpB3C.f3=_tmp58E.f3,_tmpB3C)))))));}
break;}case 128: _LL2A5: {
# 1795
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1797
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1794 "parse.y"
{struct _tuple25 _tmpB3D;yyval=Cyc_YY38(((_tmpB3D.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB3D.f2=Cyc_Parse_empty_spec(0),((_tmpB3D.f3=0,_tmpB3D)))))));}
break;}case 129: _LL2A6: {
# 1797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1796 "parse.y"
struct _tuple25 _tmp591=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB3E;yyval=Cyc_YY38(((_tmpB3E.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp591.f1),((_tmpB3E.f2=_tmp591.f2,((_tmpB3E.f3=_tmp591.f3,_tmpB3E)))))));}
break;}case 130: _LL2A7: {
# 1800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1799 "parse.y"
{struct _tuple25 _tmpB3F;yyval=Cyc_YY38(((_tmpB3F.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB3F.f2=Cyc_Parse_empty_spec(0),((_tmpB3F.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB3F)))))));}
break;}case 131: _LL2A8: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1801 "parse.y"
struct _tuple25 _tmp594=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB40;yyval=Cyc_YY38(((_tmpB40.f1=_tmp594.f1,((_tmpB40.f2=_tmp594.f2,((_tmpB40.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp594.f3),_tmpB40)))))));}
break;}case 132: _LL2A9: {
# 1804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB41;yyval=Cyc_YY38(((_tmpB41.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB41.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB41.f3=0,_tmpB41)))))));}
break;}case 133: _LL2AA: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1809 "parse.y"
struct _tuple25 _tmp597=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB42;yyval=Cyc_YY38(((_tmpB42.f1=_tmp597.f1,((_tmpB42.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp597.f2),((_tmpB42.f3=_tmp597.f3,_tmpB42)))))));}
break;}case 134: _LL2AB: {
# 1812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1811 "parse.y"
{struct _tuple25 _tmpB43;yyval=Cyc_YY38(((_tmpB43.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB43.f2=Cyc_Parse_empty_spec(0),((_tmpB43.f3=0,_tmpB43)))))));}
break;}case 135: _LL2AC: {
# 1814
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1813 "parse.y"
struct _tuple25 _tmp59A=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB44;yyval=Cyc_YY38(((_tmpB44.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp59A.f1),((_tmpB44.f2=_tmp59A.f2,((_tmpB44.f3=_tmp59A.f3,_tmpB44)))))));}
break;}case 136: _LL2AD: {
# 1817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1816 "parse.y"
{struct _tuple25 _tmpB45;yyval=Cyc_YY38(((_tmpB45.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB45.f2=Cyc_Parse_empty_spec(0),((_tmpB45.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB45)))))));}
break;}case 137: _LL2AE: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1818 "parse.y"
struct _tuple25 _tmp59D=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB46;yyval=Cyc_YY38(((_tmpB46.f1=_tmp59D.f1,((_tmpB46.f2=_tmp59D.f2,((_tmpB46.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp59D.f3),_tmpB46)))))));}
break;}case 138: _LL2AF: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1822 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 139: _LL2B0: {
# 1825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB47;yyval=Cyc_YY32(((_tmpB47=_region_malloc(yyr,sizeof(*_tmpB47)),((_tmpB47->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB47->tl=0,_tmpB47)))))));}
break;}case 140: _LL2B1: {
# 1831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1830 "parse.y"
{struct Cyc_List_List*_tmpB48;yyval=Cyc_YY32(((_tmpB48=_region_malloc(yyr,sizeof(*_tmpB48)),((_tmpB48->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB48->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB48)))))));}
break;}case 141: _LL2B2: {
# 1833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1835 "parse.y"
{struct _tuple24*_tmpB49;yyval=Cyc_YY31(((_tmpB49=_region_malloc(yyr,sizeof(*_tmpB49)),((_tmpB49->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB49->f2=0,((_tmpB49->f3=0,_tmpB49)))))))));}
break;}case 142: _LL2B3: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1839 "parse.y"
{struct Cyc_Parse_Declarator _tmpB4F;struct _tuple0*_tmpB4E;struct _tuple24*_tmpB4D;yyval=Cyc_YY31(((_tmpB4D=_region_malloc(yyr,sizeof(*_tmpB4D)),((_tmpB4D->f1=((_tmpB4F.id=((_tmpB4E=_cycalloc(sizeof(*_tmpB4E)),((_tmpB4E->f1=Cyc_Absyn_Rel_n(0),((_tmpB4E->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB4E)))))),((_tmpB4F.tms=0,_tmpB4F)))),((_tmpB4D->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB4D->f3=0,_tmpB4D)))))))));}
# 1841
break;}case 143: _LL2B4:
# 1845
{struct Cyc_Parse_Declarator _tmpB55;struct _tuple0*_tmpB54;struct _tuple24*_tmpB53;yyval=Cyc_YY31(((_tmpB53=_region_malloc(yyr,sizeof(*_tmpB53)),((_tmpB53->f1=((_tmpB55.id=((_tmpB54=_cycalloc(sizeof(*_tmpB54)),((_tmpB54->f1=Cyc_Absyn_Rel_n(0),((_tmpB54->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB54)))))),((_tmpB55.tms=0,_tmpB55)))),((_tmpB53->f2=0,((_tmpB53->f3=0,_tmpB53)))))))));}
# 1847
break;case 144: _LL2B5: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1847 "parse.y"
{struct _tuple24*_tmpB56;yyval=Cyc_YY31(((_tmpB56=_region_malloc(yyr,sizeof(*_tmpB56)),((_tmpB56->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB56->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB56->f3=0,_tmpB56)))))))));}
break;}case 145: _LL2B6: {
# 1850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1849 "parse.y"
{struct _tuple24*_tmpB57;yyval=Cyc_YY31(((_tmpB57=_region_malloc(yyr,sizeof(*_tmpB57)),((_tmpB57->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB57->f2=0,((_tmpB57->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB57)))))))));}
break;}case 146: _LL2B7: {
# 1852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp5AC=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp5AD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB58;struct Cyc_Absyn_TypeDecl*_tmp5AE=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5AD,((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB58)))),_tmp5AC,(unsigned int)(yyyylsp[0]).first_line);
# 1859
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB5B;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5A;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB5A=_cycalloc(sizeof(*_tmpB5A)),((_tmpB5A[0]=((_tmpB5B.tag=26,((_tmpB5B.f1=_tmp5AE,((_tmpB5B.f2=0,_tmpB5B)))))),_tmpB5A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1861
break;}case 147: _LL2B8: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1862 "parse.y"
int _tmp5B2=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB65;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB64;struct Cyc_Absyn_DatatypeInfo _tmpB63;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB62;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB65.tag=3,((_tmpB65.f1=((_tmpB63.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB64.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB64.is_extensible=_tmp5B2,_tmpB64))))),((_tmpB63.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB63)))),_tmpB65)))),_tmpB62)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1865
break;}case 148: _LL2B9: {
# 1867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1866 "parse.y"
int _tmp5B7=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB6F;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB6E;struct Cyc_Absyn_DatatypeFieldInfo _tmpB6D;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB6C;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB6C=_cycalloc(sizeof(*_tmpB6C)),((_tmpB6C[0]=((_tmpB6F.tag=4,((_tmpB6F.f1=((_tmpB6D.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB6E.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB6E.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB6E.is_extensible=_tmp5B7,_tmpB6E))))))),((_tmpB6D.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB6D)))),_tmpB6F)))),_tmpB6C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1871
break;}case 149: _LL2BA: {
# 1873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1874 "parse.y"
yyval=Cyc_YY34(0);
break;}case 150: _LL2BB: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1875 "parse.y"
yyval=Cyc_YY34(1);
break;}case 151: _LL2BC: {
# 1878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1879 "parse.y"
{struct Cyc_List_List*_tmpB70;yyval=Cyc_YY37(((_tmpB70=_cycalloc(sizeof(*_tmpB70)),((_tmpB70->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB70->tl=0,_tmpB70)))))));}
break;}case 152: _LL2BD: {
# 1882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1880 "parse.y"
{struct Cyc_List_List*_tmpB71;yyval=Cyc_YY37(((_tmpB71=_cycalloc(sizeof(*_tmpB71)),((_tmpB71->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB71->tl=0,_tmpB71)))))));}
break;}case 153: _LL2BE: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1881 "parse.y"
{struct Cyc_List_List*_tmpB72;yyval=Cyc_YY37(((_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB72->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB72)))))));}
break;}case 154: _LL2BF: {
# 1884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1882 "parse.y"
{struct Cyc_List_List*_tmpB73;yyval=Cyc_YY37(((_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB73->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB73)))))));}
break;}case 155: _LL2C0:
# 1885
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 156: _LL2C1: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1887 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 157: _LL2C2: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1888 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 158: _LL2C3: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1892 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB74;yyval=Cyc_YY36(((_tmpB74=_cycalloc(sizeof(*_tmpB74)),((_tmpB74->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB74->typs=0,((_tmpB74->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB74->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB74)))))))))));}
break;}case 159: _LL2C4: {
# 1895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1894 "parse.y"
struct Cyc_List_List*_tmp5C1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB75;yyval=Cyc_YY36(((_tmpB75=_cycalloc(sizeof(*_tmpB75)),((_tmpB75->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB75->typs=_tmp5C1,((_tmpB75->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB75->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB75)))))))))));}
break;}case 160: _LL2C5: {
# 1898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1900 "parse.y"
yyval=yyyyvsp[0];
break;}case 161: _LL2C6: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1902 "parse.y"
struct Cyc_Parse_Declarator _tmp5C3=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB76;yyval=Cyc_YY30(((_tmpB76.id=_tmp5C3.id,((_tmpB76.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5C3.tms),_tmpB76)))));}
break;}case 162: _LL2C7: {
# 1906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 163: _LL2C8: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1911 "parse.y"
struct Cyc_Parse_Declarator _tmp5C5=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpB77;yyval=Cyc_YY30(((_tmpB77.id=_tmp5C5.id,((_tmpB77.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5C5.tms),_tmpB77)))));}
break;}case 164: _LL2C9: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1917 "parse.y"
{struct Cyc_Parse_Declarator _tmpB78;yyval=Cyc_YY30(((_tmpB78.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB78.tms=0,_tmpB78)))));}
break;}case 165: _LL2CA: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1919 "parse.y"
yyval=yyyyvsp[1];
break;}case 166: _LL2CB: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1923 "parse.y"
struct Cyc_Parse_Declarator _tmp5C8=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpB7E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpB7D;struct Cyc_List_List*_tmpB7C;_tmp5C8.tms=((_tmpB7C=_region_malloc(yyr,sizeof(*_tmpB7C)),((_tmpB7C->hd=(void*)((_tmpB7E=_region_malloc(yyr,sizeof(*_tmpB7E)),((_tmpB7E[0]=((_tmpB7D.tag=5,((_tmpB7D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpB7D.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpB7D)))))),_tmpB7E)))),((_tmpB7C->tl=_tmp5C8.tms,_tmpB7C))))));}
yyval=yyyyvsp[2];
# 1927
break;}case 167: _LL2CC: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1928 "parse.y"
{struct Cyc_List_List*_tmpB88;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpB87;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpB86;struct Cyc_Parse_Declarator _tmpB85;yyval=Cyc_YY30(((_tmpB85.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB85.tms=((_tmpB88=_region_malloc(yyr,sizeof(*_tmpB88)),((_tmpB88->hd=(void*)((_tmpB86=_region_malloc(yyr,sizeof(*_tmpB86)),((_tmpB86[0]=((_tmpB87.tag=0,((_tmpB87.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpB87.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpB87)))))),_tmpB86)))),((_tmpB88->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB88)))))),_tmpB85)))));}
break;}case 168: _LL2CD: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1930 "parse.y"
{struct Cyc_List_List*_tmpB92;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpB91;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpB90;struct Cyc_Parse_Declarator _tmpB8F;yyval=Cyc_YY30(((_tmpB8F.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpB8F.tms=(
(_tmpB92=_region_malloc(yyr,sizeof(*_tmpB92)),((_tmpB92->hd=(void*)((_tmpB90=_region_malloc(yyr,sizeof(*_tmpB90)),((_tmpB90[0]=((_tmpB91.tag=1,((_tmpB91.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB91.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpB91.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpB91)))))))),_tmpB90)))),((_tmpB92->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpB92)))))),_tmpB8F)))));}
break;}case 169: _LL2CE: {
# 1934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1933 "parse.y"
struct _tuple26*_tmp5D4=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5D6;int _tmp5D7;struct Cyc_Absyn_VarargInfo*_tmp5D8;void*_tmp5D9;struct Cyc_List_List*_tmp5DA;struct _tuple26*_tmp5D5=_tmp5D4;_tmp5D6=_tmp5D5->f1;_tmp5D7=_tmp5D5->f2;_tmp5D8=_tmp5D5->f3;_tmp5D9=_tmp5D5->f4;_tmp5DA=_tmp5D5->f5;
{struct Cyc_List_List*_tmpBA7;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBA6;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBA5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBA4;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBA3;struct Cyc_Parse_Declarator _tmpBA2;yyval=Cyc_YY30(((_tmpBA2.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBA2.tms=((_tmpBA7=_region_malloc(yyr,sizeof(*_tmpBA7)),((_tmpBA7->hd=(void*)((_tmpBA3=_region_malloc(yyr,sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA6.tag=3,((_tmpBA6.f1=(void*)((_tmpBA4=_region_malloc(yyr,sizeof(*_tmpBA4)),((_tmpBA4[0]=((_tmpBA5.tag=1,((_tmpBA5.f1=_tmp5D6,((_tmpBA5.f2=_tmp5D7,((_tmpBA5.f3=_tmp5D8,((_tmpBA5.f4=_tmp5D9,((_tmpBA5.f5=_tmp5DA,_tmpBA5)))))))))))),_tmpBA4)))),_tmpBA6)))),_tmpBA3)))),((_tmpBA7->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBA7)))))),_tmpBA2)))));}
# 1936
break;}case 170: _LL2CF: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1937 "parse.y"
{struct Cyc_List_List*_tmpBBC;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBBB;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBBA;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBB9;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBB8;struct Cyc_Parse_Declarator _tmpBB7;yyval=Cyc_YY30(((_tmpBB7.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB7.tms=(
(_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)((_tmpBB8=_region_malloc(yyr,sizeof(*_tmpBB8)),((_tmpBB8[0]=((_tmpBBB.tag=3,((_tmpBBB.f1=(void*)((_tmpBB9=_region_malloc(yyr,sizeof(*_tmpBB9)),((_tmpBB9[0]=((_tmpBBA.tag=1,((_tmpBBA.f1=0,((_tmpBBA.f2=0,((_tmpBBA.f3=0,((_tmpBBA.f4=
# 1940
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpBBA.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpBBA)))))))))))),_tmpBB9)))),_tmpBBB)))),_tmpBB8)))),((_tmpBBC->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBBC)))))),_tmpBB7)))));}
# 1943
break;}case 171: _LL2D0: {
# 1945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1944 "parse.y"
{struct Cyc_List_List*_tmpBD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD0;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBCF;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBCE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBCD;struct Cyc_Parse_Declarator _tmpBCC;yyval=Cyc_YY30(((_tmpBCC.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBCC.tms=((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1->hd=(void*)((_tmpBCD=_region_malloc(yyr,sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBD0.tag=3,((_tmpBD0.f1=(void*)((_tmpBCE=_region_malloc(yyr,sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBCF.tag=0,((_tmpBCF.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBCF.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBCF)))))),_tmpBCE)))),_tmpBD0)))),_tmpBCD)))),((_tmpBD1->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBD1)))))),_tmpBCC)))));}
break;}case 172: _LL2D1: {
# 1947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1947 "parse.y"
struct Cyc_List_List*_tmp5ED=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpBDB;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpBDA;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpBD9;struct Cyc_Parse_Declarator _tmpBD8;yyval=Cyc_YY30(((_tmpBD8.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBD8.tms=((_tmpBDB=_region_malloc(yyr,sizeof(*_tmpBDB)),((_tmpBDB->hd=(void*)((_tmpBD9=_region_malloc(yyr,sizeof(*_tmpBD9)),((_tmpBD9[0]=((_tmpBDA.tag=4,((_tmpBDA.f1=_tmp5ED,((_tmpBDA.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpBDA.f3=0,_tmpBDA)))))))),_tmpBD9)))),((_tmpBDB->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBDB)))))),_tmpBD8)))));}
# 1950
break;}case 173: _LL2D2: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1951 "parse.y"
{struct Cyc_List_List*_tmpBE5;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBE4;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBE3;struct Cyc_Parse_Declarator _tmpBE2;yyval=Cyc_YY30(((_tmpBE2.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBE2.tms=((_tmpBE5=_region_malloc(yyr,sizeof(*_tmpBE5)),((_tmpBE5->hd=(void*)((_tmpBE3=_region_malloc(yyr,sizeof(*_tmpBE3)),((_tmpBE3[0]=((_tmpBE4.tag=5,((_tmpBE4.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBE4.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBE4)))))),_tmpBE3)))),((_tmpBE5->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBE5)))))),_tmpBE2)))));}
# 1954
break;}case 174: _LL2D3: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1965 "parse.y"
{struct Cyc_Parse_Declarator _tmpBE6;yyval=Cyc_YY30(((_tmpBE6.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBE6.tms=0,_tmpBE6)))));}
break;}case 175: _LL2D4: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1967 "parse.y"
{struct Cyc_Parse_Declarator _tmpBE7;yyval=Cyc_YY30(((_tmpBE7.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBE7.tms=0,_tmpBE7)))));}
break;}case 176: _LL2D5: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1969 "parse.y"
yyval=yyyyvsp[1];
break;}case 177: _LL2D6: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1973 "parse.y"
struct Cyc_Parse_Declarator _tmp5F8=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBED;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBEC;struct Cyc_List_List*_tmpBEB;_tmp5F8.tms=((_tmpBEB=_region_malloc(yyr,sizeof(*_tmpBEB)),((_tmpBEB->hd=(void*)((_tmpBED=_region_malloc(yyr,sizeof(*_tmpBED)),((_tmpBED[0]=((_tmpBEC.tag=5,((_tmpBEC.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBEC.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBEC)))))),_tmpBED)))),((_tmpBEB->tl=_tmp5F8.tms,_tmpBEB))))));}
yyval=yyyyvsp[2];
# 1977
break;}case 178: _LL2D7: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1978 "parse.y"
struct Cyc_Parse_Declarator _tmp5FC=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpBF7;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBF6;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBF5;struct Cyc_Parse_Declarator _tmpBF4;yyval=Cyc_YY30(((_tmpBF4.id=_tmp5FC.id,((_tmpBF4.tms=(
(_tmpBF7=_region_malloc(yyr,sizeof(*_tmpBF7)),((_tmpBF7->hd=(void*)((_tmpBF5=_region_malloc(yyr,sizeof(*_tmpBF5)),((_tmpBF5[0]=((_tmpBF6.tag=0,((_tmpBF6.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBF6.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBF6)))))),_tmpBF5)))),((_tmpBF7->tl=_tmp5FC.tms,_tmpBF7)))))),_tmpBF4)))));}
break;}case 179: _LL2D8: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1982 "parse.y"
struct Cyc_Parse_Declarator _tmp601=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC01;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC00;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBFF;struct Cyc_Parse_Declarator _tmpBFE;yyval=Cyc_YY30(((_tmpBFE.id=_tmp601.id,((_tmpBFE.tms=(
(_tmpC01=_region_malloc(yyr,sizeof(*_tmpC01)),((_tmpC01->hd=(void*)((_tmpBFF=_region_malloc(yyr,sizeof(*_tmpBFF)),((_tmpBFF[0]=((_tmpC00.tag=1,((_tmpC00.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpC00.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpC00.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC00)))))))),_tmpBFF)))),((_tmpC01->tl=_tmp601.tms,_tmpC01)))))),_tmpBFE)))));}
# 1986
break;}case 180: _LL2D9: {
# 1988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1987 "parse.y"
struct _tuple26*_tmp606=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp608;int _tmp609;struct Cyc_Absyn_VarargInfo*_tmp60A;void*_tmp60B;struct Cyc_List_List*_tmp60C;struct _tuple26*_tmp607=_tmp606;_tmp608=_tmp607->f1;_tmp609=_tmp607->f2;_tmp60A=_tmp607->f3;_tmp60B=_tmp607->f4;_tmp60C=_tmp607->f5;{
struct Cyc_Parse_Declarator _tmp60D=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC16;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC15;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC14;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC13;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC12;struct Cyc_Parse_Declarator _tmpC11;yyval=Cyc_YY30(((_tmpC11.id=_tmp60D.id,((_tmpC11.tms=((_tmpC16=_region_malloc(yyr,sizeof(*_tmpC16)),((_tmpC16->hd=(void*)((_tmpC12=_region_malloc(yyr,sizeof(*_tmpC12)),((_tmpC12[0]=((_tmpC15.tag=3,((_tmpC15.f1=(void*)((_tmpC13=_region_malloc(yyr,sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC14.tag=1,((_tmpC14.f1=_tmp608,((_tmpC14.f2=_tmp609,((_tmpC14.f3=_tmp60A,((_tmpC14.f4=_tmp60B,((_tmpC14.f5=_tmp60C,_tmpC14)))))))))))),_tmpC13)))),_tmpC15)))),_tmpC12)))),((_tmpC16->tl=_tmp60D.tms,_tmpC16)))))),_tmpC11)))));}
# 1991
break;};}case 181: _LL2DA: {
# 1993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1992 "parse.y"
struct Cyc_Parse_Declarator _tmp614=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC2A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC29;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC28;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC27;struct Cyc_Parse_Declarator _tmpC26;yyval=Cyc_YY30(((_tmpC26.id=_tmp614.id,((_tmpC26.tms=(
(_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)((_tmpC27=_region_malloc(yyr,sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC2A.tag=3,((_tmpC2A.f1=(void*)((_tmpC28=_region_malloc(yyr,sizeof(*_tmpC28)),((_tmpC28[0]=((_tmpC29.tag=1,((_tmpC29.f1=0,((_tmpC29.f2=0,((_tmpC29.f3=0,((_tmpC29.f4=
# 1996
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC29.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC29)))))))))))),_tmpC28)))),_tmpC2A)))),_tmpC27)))),((_tmpC2B->tl=_tmp614.tms,_tmpC2B)))))),_tmpC26)))));}
# 1999
break;}case 182: _LL2DB: {
# 2001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2000 "parse.y"
struct Cyc_Parse_Declarator _tmp61B=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC40;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC3F;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC3E;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC3D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC3C;struct Cyc_Parse_Declarator _tmpC3B;yyval=Cyc_YY30(((_tmpC3B.id=_tmp61B.id,((_tmpC3B.tms=((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40->hd=(void*)((_tmpC3C=_region_malloc(yyr,sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3F.tag=3,((_tmpC3F.f1=(void*)((_tmpC3D=_region_malloc(yyr,sizeof(*_tmpC3D)),((_tmpC3D[0]=((_tmpC3E.tag=0,((_tmpC3E.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC3E.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC3E)))))),_tmpC3D)))),_tmpC3F)))),_tmpC3C)))),((_tmpC40->tl=_tmp61B.tms,_tmpC40)))))),_tmpC3B)))));}
break;}case 183: _LL2DC: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2004 "parse.y"
struct Cyc_List_List*_tmp622=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp623=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC4A;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC49;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC48;struct Cyc_Parse_Declarator _tmpC47;yyval=Cyc_YY30(((_tmpC47.id=_tmp623.id,((_tmpC47.tms=((_tmpC4A=_region_malloc(yyr,sizeof(*_tmpC4A)),((_tmpC4A->hd=(void*)((_tmpC48=_region_malloc(yyr,sizeof(*_tmpC48)),((_tmpC48[0]=((_tmpC49.tag=4,((_tmpC49.f1=_tmp622,((_tmpC49.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC49.f3=0,_tmpC49)))))))),_tmpC48)))),((_tmpC4A->tl=_tmp623.tms,_tmpC4A)))))),_tmpC47)))));}
# 2008
break;}case 184: _LL2DD: {
# 2010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp628=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC54;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC53;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC52;struct Cyc_Parse_Declarator _tmpC51;yyval=Cyc_YY30(((_tmpC51.id=_tmp628.id,((_tmpC51.tms=((_tmpC54=_region_malloc(yyr,sizeof(*_tmpC54)),((_tmpC54->hd=(void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=5,((_tmpC53.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC53.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC53)))))),_tmpC52)))),((_tmpC54->tl=_tmp628.tms,_tmpC54)))))),_tmpC51)))));}
# 2012
break;}case 185: _LL2DE: {
# 2014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2022 "parse.y"
yyval=yyyyvsp[0];
break;}case 186: _LL2DF: {
# 2025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2023 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 187: _LL2E0: {
# 2026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2027 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC5A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC59;struct Cyc_List_List*_tmpC58;ans=((_tmpC58=_region_malloc(yyr,sizeof(*_tmpC58)),((_tmpC58->hd=(void*)((_tmpC5A=_region_malloc(yyr,sizeof(*_tmpC5A)),((_tmpC5A[0]=((_tmpC59.tag=5,((_tmpC59.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC59.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC59)))))),_tmpC5A)))),((_tmpC58->tl=ans,_tmpC58))))));}{
# 2031
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp630=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp632;union Cyc_Absyn_Constraint*_tmp633;union Cyc_Absyn_Constraint*_tmp634;struct _tuple21 _tmp631=_tmp630;_tmp632=_tmp631.f1;_tmp633=_tmp631.f2;_tmp634=_tmp631.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC5B;ptrloc=((_tmpC5B=_cycalloc_atomic(sizeof(*_tmpC5B)),((_tmpC5B->ptr_loc=_tmp632,((_tmpC5B->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC5B->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC5B))))))));}{
# 2036
struct _tuple14 _tmp636=Cyc_Parse_collapse_pointer_quals(_tmp632,_tmp633,_tmp634,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp638;union Cyc_Absyn_Constraint*_tmp639;union Cyc_Absyn_Constraint*_tmp63A;void*_tmp63B;struct _tuple14 _tmp637=_tmp636;_tmp638=_tmp637.f1;_tmp639=_tmp637.f2;_tmp63A=_tmp637.f3;_tmp63B=_tmp637.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC65;struct Cyc_Absyn_PtrAtts _tmpC64;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC63;struct Cyc_List_List*_tmpC62;ans=((_tmpC62=_region_malloc(yyr,sizeof(*_tmpC62)),((_tmpC62->hd=(void*)((_tmpC65=_region_malloc(yyr,sizeof(*_tmpC65)),((_tmpC65[0]=((_tmpC63.tag=2,((_tmpC63.f1=((_tmpC64.rgn=_tmp63B,((_tmpC64.nullable=_tmp638,((_tmpC64.bounds=_tmp639,((_tmpC64.zero_term=_tmp63A,((_tmpC64.ptrloc=ptrloc,_tmpC64)))))))))),((_tmpC63.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC63)))))),_tmpC65)))),((_tmpC62->tl=ans,_tmpC62))))));}
yyval=Cyc_YY29(ans);
# 2040
break;};};}case 188: _LL2E1:
# 2042
 yyval=Cyc_YY57(0);
break;case 189: _LL2E2: {
# 2045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2043 "parse.y"
{struct Cyc_List_List*_tmpC66;yyval=Cyc_YY57(((_tmpC66=_region_malloc(yyr,sizeof(*_tmpC66)),((_tmpC66->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC66->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC66)))))));}
break;}case 190: _LL2E3: {
# 2046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2048 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC69;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC68;yyval=Cyc_YY56((void*)((_tmpC68=_region_malloc(yyr,sizeof(*_tmpC68)),((_tmpC68[0]=((_tmpC69.tag=0,((_tmpC69.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC69)))),_tmpC68)))));}
break;}case 191: _LL2E4: {
# 2051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2050 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC6C;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC6B;yyval=Cyc_YY56((void*)((_tmpC6B=_region_malloc(yyr,sizeof(*_tmpC6B)),((_tmpC6B[0]=((_tmpC6C.tag=1,((_tmpC6C.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC6C)))),_tmpC6B)))));}
break;}case 192: _LL2E5: {
# 2053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2052 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC6F;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC6E;yyval=Cyc_YY56((void*)((_tmpC6E=_region_malloc(yyr,sizeof(*_tmpC6E)),((_tmpC6E[0]=((_tmpC6F.tag=2,_tmpC6F)),_tmpC6E)))));}
break;}case 193: _LL2E6: {
# 2055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2054 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC72;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC71;yyval=Cyc_YY56((void*)((_tmpC71=_region_malloc(yyr,sizeof(*_tmpC71)),((_tmpC71[0]=((_tmpC72.tag=3,_tmpC72)),_tmpC71)))));}
break;}case 194: _LL2E7: {
# 2057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2056 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC75;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC74;yyval=Cyc_YY56((void*)((_tmpC74=_region_malloc(yyr,sizeof(*_tmpC74)),((_tmpC74[0]=((_tmpC75.tag=4,_tmpC75)),_tmpC74)))));}
break;}case 195: _LL2E8: {
# 2059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2058 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC78;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC77;yyval=Cyc_YY56((void*)((_tmpC77=_region_malloc(yyr,sizeof(*_tmpC77)),((_tmpC77[0]=((_tmpC78.tag=5,_tmpC78)),_tmpC77)))));}
break;}case 196: _LL2E9: {
# 2061
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2063
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2060 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpC7B;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpC7A;yyval=Cyc_YY56((void*)((_tmpC7A=_region_malloc(yyr,sizeof(*_tmpC7A)),((_tmpC7A[0]=((_tmpC7B.tag=6,_tmpC7B)),_tmpC7A)))));}
break;}case 197: _LL2EA: {
# 2063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2062 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpC7E;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpC7D;yyval=Cyc_YY56((void*)((_tmpC7D=_region_malloc(yyr,sizeof(*_tmpC7D)),((_tmpC7D[0]=((_tmpC7E.tag=7,_tmpC7E)),_tmpC7D)))));}
break;}case 198: _LL2EB: {
# 2065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2068 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC7F;yyval=Cyc_YY1(((_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->f1=loc,((_tmpC7F->f2=Cyc_Absyn_true_conref,((_tmpC7F->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC7F)))))))));}
# 2071
break;}case 199: _LL2EC: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2072 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC80;yyval=Cyc_YY1(((_tmpC80=_cycalloc(sizeof(*_tmpC80)),((_tmpC80->f1=loc,((_tmpC80->f2=Cyc_Absyn_false_conref,((_tmpC80->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpC80)))))))));}
# 2075
break;}case 200: _LL2ED: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpC81;yyval=Cyc_YY1(((_tmpC81=_cycalloc(sizeof(*_tmpC81)),((_tmpC81->f1=loc,((_tmpC81->f2=Cyc_Absyn_true_conref,((_tmpC81->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpC81)))))))));}
# 2079
break;}case 201: _LL2EE:
# 2081
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 202: _LL2EF: {
# 2084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2082 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpC84;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpC83;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpC83=_cycalloc(sizeof(*_tmpC83)),((_tmpC83[0]=((_tmpC84.tag=1,((_tmpC84.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpC84)))),_tmpC83))))));}
break;}case 203: _LL2F0:
# 2085
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 204: _LL2F1: {
# 2088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2086 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 205: _LL2F2: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2087 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 206: _LL2F3:
# 2090
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 207: _LL2F4: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2093 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 208: _LL2F5: {
# 2096
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2098
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2094 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 209: _LL2F6:
# 2097
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 210: _LL2F7: {
# 2100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2099 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 211: _LL2F8: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2104 "parse.y"
{struct _tuple26*_tmpC85;yyval=Cyc_YY42(((_tmpC85=_cycalloc(sizeof(*_tmpC85)),((_tmpC85->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC85->f2=0,((_tmpC85->f3=0,((_tmpC85->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpC85->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpC85)))))))))))));}
break;}case 212: _LL2F9: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2106 "parse.y"
{struct _tuple26*_tmpC86;yyval=Cyc_YY42(((_tmpC86=_cycalloc(sizeof(*_tmpC86)),((_tmpC86->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC86->f2=1,((_tmpC86->f3=0,((_tmpC86->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC86->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC86)))))))))))));}
break;}case 213: _LL2FA: {
# 2109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2109 "parse.y"
struct _tuple8*_tmp658=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp65A;struct Cyc_Absyn_Tqual _tmp65B;void*_tmp65C;struct _tuple8*_tmp659=_tmp658;_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;_tmp65C=_tmp659->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC87;struct Cyc_Absyn_VarargInfo*_tmp65D=(_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->name=_tmp65A,((_tmpC87->tq=_tmp65B,((_tmpC87->type=_tmp65C,((_tmpC87->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpC87)))))))));
{struct _tuple26*_tmpC88;yyval=Cyc_YY42(((_tmpC88=_cycalloc(sizeof(*_tmpC88)),((_tmpC88->f1=0,((_tmpC88->f2=0,((_tmpC88->f3=_tmp65D,((_tmpC88->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpC88->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpC88)))))))))))));}
# 2113
break;};}case 214: _LL2FB: {
# 2115
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2117
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2115 "parse.y"
struct _tuple8*_tmp660=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp662;struct Cyc_Absyn_Tqual _tmp663;void*_tmp664;struct _tuple8*_tmp661=_tmp660;_tmp662=_tmp661->f1;_tmp663=_tmp661->f2;_tmp664=_tmp661->f3;{
struct Cyc_Absyn_VarargInfo*_tmpC89;struct Cyc_Absyn_VarargInfo*_tmp665=(_tmpC89=_cycalloc(sizeof(*_tmpC89)),((_tmpC89->name=_tmp662,((_tmpC89->tq=_tmp663,((_tmpC89->type=_tmp664,((_tmpC89->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpC89)))))))));
{struct _tuple26*_tmpC8A;yyval=Cyc_YY42(((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpC8A->f2=0,((_tmpC8A->f3=_tmp665,((_tmpC8A->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpC8A->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpC8A)))))))))))));}
# 2119
break;};}case 215: _LL2FC: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2123 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpC8D;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC8C;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C[0]=((_tmpC8D.tag=1,((_tmpC8D.f1=0,_tmpC8D)))),_tmpC8C))))));}
break;}case 216: _LL2FD: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2124 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 217: _LL2FE:
# 2127
 yyval=Cyc_YY52(0);
break;case 218: _LL2FF: {
# 2130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2128 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC90;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC8F;yyval=Cyc_YY52((void*)((_tmpC8F=_cycalloc(sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC90.tag=24,((_tmpC90.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpC90)))),_tmpC8F)))));}
break;}case 219: _LL300:
# 2131
 yyval=Cyc_YY53(0);
break;case 220: _LL301: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2133 "parse.y"
yyval=yyyyvsp[1];
break;}case 221: _LL302: {
# 2136
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2138
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2141 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpC93;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC92;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp66C=(_tmpC92=_cycalloc(sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC93.tag=2,((_tmpC93.f1=0,((_tmpC93.f2=& Cyc_Tcutil_trk,_tmpC93)))))),_tmpC92)));
struct _dyneither_ptr _tmp66D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp66E=Cyc_Parse_id2type(_tmp66D,(void*)_tmp66C);
{struct _tuple29*_tmpC9D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC9C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC9B;struct Cyc_List_List*_tmpC9A;yyval=Cyc_YY53(((_tmpC9A=_cycalloc(sizeof(*_tmpC9A)),((_tmpC9A->hd=((_tmpC9D=_cycalloc(sizeof(*_tmpC9D)),((_tmpC9D->f1=(void*)((_tmpC9B=_cycalloc(sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=24,((_tmpC9C.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpC9C)))),_tmpC9B)))),((_tmpC9D->f2=_tmp66E,_tmpC9D)))))),((_tmpC9A->tl=0,_tmpC9A)))))));}
# 2146
break;}case 222: _LL303: {
# 2148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2148 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCA0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC9F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp675=(_tmpC9F=_cycalloc(sizeof(*_tmpC9F)),((_tmpC9F[0]=((_tmpCA0.tag=2,((_tmpCA0.f1=0,((_tmpCA0.f2=& Cyc_Tcutil_trk,_tmpCA0)))))),_tmpC9F)));
struct _dyneither_ptr _tmp676=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp677=Cyc_Parse_id2type(_tmp676,(void*)_tmp675);
{struct _tuple29*_tmpCAA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCA9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCA8;struct Cyc_List_List*_tmpCA7;yyval=Cyc_YY53(((_tmpCA7=_cycalloc(sizeof(*_tmpCA7)),((_tmpCA7->hd=((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->f1=(void*)((_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8[0]=((_tmpCA9.tag=24,((_tmpCA9.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCA9)))),_tmpCA8)))),((_tmpCAA->f2=_tmp677,_tmpCAA)))))),((_tmpCA7->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCA7)))))));}
# 2153
break;}case 223: _LL304:
# 2155
 yyval=Cyc_YY34(0);
break;case 224: _LL305: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2159 "parse.y"
{const char*_tmpCAB;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCAB="inject",_tag_dyneither(_tmpCAB,sizeof(char),7))))!= 0){
const char*_tmpCAC;Cyc_Parse_err(((_tmpCAC="missing type in function declaration",_tag_dyneither(_tmpCAC,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2163
break;}case 225: _LL306: {
# 2165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2166 "parse.y"
yyval=yyyyvsp[0];
break;}case 226: _LL307: {
# 2169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2167 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 227: _LL308: {
# 2170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2171 "parse.y"
yyval=Cyc_YY43(0);
break;}case 228: _LL309: {
# 2174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2172 "parse.y"
yyval=yyyyvsp[1];
break;}case 229: _LL30A: {
# 2175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2174 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCB2;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCB1;struct Cyc_List_List*_tmpCB0;yyval=Cyc_YY43(((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->hd=(void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB1.tag=25,((_tmpCB1.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCB1)))),_tmpCB2)))),((_tmpCB0->tl=0,_tmpCB0)))))));}
break;}case 230: _LL30B: {
# 2177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2176 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCB3;yyval=Cyc_YY43(((_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCB3->tl=0,_tmpCB3)))))));}
# 2179
break;}case 231: _LL30C: {
# 2181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2196 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCB9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCB8;struct Cyc_List_List*_tmpCB7;yyval=Cyc_YY43(((_tmpCB7=_cycalloc(sizeof(*_tmpCB7)),((_tmpCB7->hd=(void*)((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCB8.tag=23,((_tmpCB8.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCB8)))),_tmpCB9)))),((_tmpCB7->tl=0,_tmpCB7)))))));}
break;}case 232: _LL30D: {
# 2199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2198 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCBF;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCBE;struct Cyc_List_List*_tmpCBD;yyval=Cyc_YY43(((_tmpCBD=_cycalloc(sizeof(*_tmpCBD)),((_tmpCBD->hd=(void*)((_tmpCBF=_cycalloc(sizeof(*_tmpCBF)),((_tmpCBF[0]=((_tmpCBE.tag=23,((_tmpCBE.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCBE)))),_tmpCBF)))),((_tmpCBD->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCBD)))))));}
break;}case 233: _LL30E: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCC0;yyval=Cyc_YY41(((_tmpCC0=_cycalloc(sizeof(*_tmpCC0)),((_tmpCC0->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpCC0->tl=0,_tmpCC0)))))));}
break;}case 234: _LL30F: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2206 "parse.y"
{struct Cyc_List_List*_tmpCC1;yyval=Cyc_YY41(((_tmpCC1=_cycalloc(sizeof(*_tmpCC1)),((_tmpCC1->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpCC1->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCC1)))))));}
break;}case 235: _LL310: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp68C=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp68E;struct Cyc_Parse_Type_specifier _tmp68F;struct Cyc_List_List*_tmp690;struct _tuple25 _tmp68D=_tmp68C;_tmp68E=_tmp68D.f1;_tmp68F=_tmp68D.f2;_tmp690=_tmp68D.f3;
if(_tmp68E.loc == 0)_tmp68E.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp691=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp693;struct Cyc_List_List*_tmp694;struct Cyc_Parse_Declarator _tmp692=_tmp691;_tmp693=_tmp692.id;_tmp694=_tmp692.tms;{
void*_tmp695=Cyc_Parse_speclist2typ(_tmp68F,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp696=Cyc_Parse_apply_tms(_tmp68E,_tmp695,_tmp690,_tmp694);struct Cyc_Absyn_Tqual _tmp698;void*_tmp699;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp69B;struct _tuple13 _tmp697=_tmp696;_tmp698=_tmp697.f1;_tmp699=_tmp697.f2;_tmp69A=_tmp697.f3;_tmp69B=_tmp697.f4;
if(_tmp69A != 0){
const char*_tmpCC2;Cyc_Parse_err(((_tmpCC2="parameter with bad type params",_tag_dyneither(_tmpCC2,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp693)){
const char*_tmpCC3;Cyc_Parse_err(((_tmpCC3="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCC3,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp693).f2;
if(_tmp69B != 0){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCC6="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCC6,sizeof(char),40))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}
{struct _tuple8*_tmpCC7;yyval=Cyc_YY40(((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->f1=idopt,((_tmpCC7->f2=_tmp698,((_tmpCC7->f3=_tmp699,_tmpCC7)))))))));}
# 2226
break;};};};}case 236: _LL311: {
# 2228
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2230
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2227 "parse.y"
struct _tuple25 _tmp6A1=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6A3;struct Cyc_Parse_Type_specifier _tmp6A4;struct Cyc_List_List*_tmp6A5;struct _tuple25 _tmp6A2=_tmp6A1;_tmp6A3=_tmp6A2.f1;_tmp6A4=_tmp6A2.f2;_tmp6A5=_tmp6A2.f3;
if(_tmp6A3.loc == 0)_tmp6A3.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6A6=Cyc_Parse_speclist2typ(_tmp6A4,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6A5 != 0){
const char*_tmpCCA;void*_tmpCC9;(_tmpCC9=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCA="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCCA,sizeof(char),38))),_tag_dyneither(_tmpCC9,sizeof(void*),0)));}
{struct _tuple8*_tmpCCB;yyval=Cyc_YY40(((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB->f1=0,((_tmpCCB->f2=_tmp6A3,((_tmpCCB->f3=_tmp6A6,_tmpCCB)))))))));}
# 2234
break;};}case 237: _LL312: {
# 2236
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2238
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2235 "parse.y"
struct _tuple25 _tmp6AA=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6AC;struct Cyc_Parse_Type_specifier _tmp6AD;struct Cyc_List_List*_tmp6AE;struct _tuple25 _tmp6AB=_tmp6AA;_tmp6AC=_tmp6AB.f1;_tmp6AD=_tmp6AB.f2;_tmp6AE=_tmp6AB.f3;
if(_tmp6AC.loc == 0)_tmp6AC.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6AF=Cyc_Parse_speclist2typ(_tmp6AD,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6B0=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6B1=Cyc_Parse_apply_tms(_tmp6AC,_tmp6AF,_tmp6AE,_tmp6B0);struct Cyc_Absyn_Tqual _tmp6B3;void*_tmp6B4;struct Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple13 _tmp6B2=_tmp6B1;_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;_tmp6B5=_tmp6B2.f3;_tmp6B6=_tmp6B2.f4;
if(_tmp6B5 != 0){
const char*_tmpCCE;void*_tmpCCD;(_tmpCCD=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCCE="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCCE,sizeof(char),49))),_tag_dyneither(_tmpCCD,sizeof(void*),0)));}
# 2243
if(_tmp6B6 != 0){
const char*_tmpCD1;void*_tmpCD0;(_tmpCD0=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCD1="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCD1,sizeof(char),38))),_tag_dyneither(_tmpCD0,sizeof(void*),0)));}
{struct _tuple8*_tmpCD2;yyval=Cyc_YY40(((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2->f1=0,((_tmpCD2->f2=_tmp6B3,((_tmpCD2->f3=_tmp6B4,_tmpCD2)))))))));}
# 2247
break;};}case 238: _LL313: {
# 2249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2251 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 239: _LL314: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2255 "parse.y"
{struct _dyneither_ptr*_tmpCD5;struct Cyc_List_List*_tmpCD4;yyval=Cyc_YY39(((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->hd=((_tmpCD5=_cycalloc(sizeof(*_tmpCD5)),((_tmpCD5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCD5)))),((_tmpCD4->tl=0,_tmpCD4)))))));}
break;}case 240: _LL315: {
# 2258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2257 "parse.y"
{struct _dyneither_ptr*_tmpCD8;struct Cyc_List_List*_tmpCD7;yyval=Cyc_YY39(((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7->hd=((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCD8)))),((_tmpCD7->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpCD7)))))));}
break;}case 241: _LL316: {
# 2260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2261 "parse.y"
yyval=yyyyvsp[0];
break;}case 242: _LL317: {
# 2264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2262 "parse.y"
yyval=yyyyvsp[0];
break;}case 243: _LL318: {
# 2265
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2267
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2267 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCDB;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCDA;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA[0]=((_tmpCDB.tag=35,((_tmpCDB.f1=0,((_tmpCDB.f2=0,_tmpCDB)))))),_tmpCDA)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 244: _LL319: {
# 2270
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2272
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2269 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCDE;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCDD;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD[0]=((_tmpCDE.tag=35,((_tmpCDE.f1=0,((_tmpCDE.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCDE)))))),_tmpCDD)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31A: {
# 2272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2271 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpCE1;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCE0;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCE0=_cycalloc(sizeof(*_tmpCE0)),((_tmpCE0[0]=((_tmpCE1.tag=35,((_tmpCE1.f1=0,((_tmpCE1.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpCE1)))))),_tmpCE0)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL31B: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2273 "parse.y"
struct _dyneither_ptr*_tmpCE4;struct _tuple0*_tmpCE3;struct Cyc_Absyn_Vardecl*_tmp6C6=Cyc_Absyn_new_vardecl(((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3->f1=Cyc_Absyn_Loc_n,((_tmpCE3->f2=((_tmpCE4=_cycalloc(sizeof(*_tmpCE4)),((_tmpCE4[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpCE4)))),_tmpCE3)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2276
(_tmp6C6->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpCE7;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpCE6;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCE6=_cycalloc(sizeof(*_tmpCE6)),((_tmpCE6[0]=((_tmpCE7.tag=26,((_tmpCE7.f1=_tmp6C6,((_tmpCE7.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCE7.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpCE7.f4=0,_tmpCE7)))))))))),_tmpCE6)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2279
break;}case 247: _LL31C: {
# 2281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2280 "parse.y"
void*_tmp6CB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpCEA;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpCE9;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCEA.tag=27,((_tmpCEA.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpCEA.f2=_tmp6CB,((_tmpCEA.f3=0,_tmpCEA)))))))),_tmpCE9)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2283
break;}case 248: _LL31D: {
# 2285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpCED;struct Cyc_List_List*_tmpCEC;yyval=Cyc_YY7(((_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=((_tmpCED=_cycalloc(sizeof(*_tmpCED)),((_tmpCED->f1=0,((_tmpCED->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpCED)))))),((_tmpCEC->tl=0,_tmpCEC)))))));}
break;}case 249: _LL31E: {
# 2291
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2293
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2290 "parse.y"
{struct _tuple30*_tmpCF0;struct Cyc_List_List*_tmpCEF;yyval=Cyc_YY7(((_tmpCEF=_cycalloc(sizeof(*_tmpCEF)),((_tmpCEF->hd=((_tmpCF0=_cycalloc(sizeof(*_tmpCF0)),((_tmpCF0->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpCF0->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCF0)))))),((_tmpCEF->tl=0,_tmpCEF)))))));}
break;}case 250: _LL31F: {
# 2293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2292 "parse.y"
{struct _tuple30*_tmpCF3;struct Cyc_List_List*_tmpCF2;yyval=Cyc_YY7(((_tmpCF2=_cycalloc(sizeof(*_tmpCF2)),((_tmpCF2->hd=((_tmpCF3=_cycalloc(sizeof(*_tmpCF3)),((_tmpCF3->f1=0,((_tmpCF3->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpCF3)))))),((_tmpCF2->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCF2)))))));}
break;}case 251: _LL320: {
# 2295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2294 "parse.y"
{struct _tuple30*_tmpCF6;struct Cyc_List_List*_tmpCF5;yyval=Cyc_YY7(((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpCF6->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpCF6)))))),((_tmpCF5->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpCF5)))))));}
break;}case 252: _LL321: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2298 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 253: _LL322: {
# 2301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2303 "parse.y"
{struct Cyc_List_List*_tmpCF7;yyval=Cyc_YY44(((_tmpCF7=_cycalloc(sizeof(*_tmpCF7)),((_tmpCF7->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpCF7->tl=0,_tmpCF7)))))));}
break;}case 254: _LL323: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2304 "parse.y"
{struct Cyc_List_List*_tmpCF8;yyval=Cyc_YY44(((_tmpCF8=_cycalloc(sizeof(*_tmpCF8)),((_tmpCF8->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpCF8->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpCF8)))))));}
break;}case 255: _LL324: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2308 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpCFB;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCFA;yyval=Cyc_YY45((void*)((_tmpCFA=_cycalloc(sizeof(*_tmpCFA)),((_tmpCFA[0]=((_tmpCFB.tag=0,((_tmpCFB.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCFB)))),_tmpCFA)))));}
break;}case 256: _LL325: {
# 2311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2309 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD01;struct _dyneither_ptr*_tmpD00;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCFF;yyval=Cyc_YY45((void*)((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF[0]=((_tmpD01.tag=1,((_tmpD01.f1=((_tmpD00=_cycalloc(sizeof(*_tmpD00)),((_tmpD00[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD00)))),_tmpD01)))),_tmpCFF)))));}
break;}case 257: _LL326: {
# 2312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2314 "parse.y"
struct _tuple25 _tmp6DD=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6DF;struct Cyc_Parse_Type_specifier _tmp6E0;struct Cyc_List_List*_tmp6E1;struct _tuple25 _tmp6DE=_tmp6DD;_tmp6DF=_tmp6DE.f1;_tmp6E0=_tmp6DE.f2;_tmp6E1=_tmp6DE.f3;{
void*_tmp6E2=Cyc_Parse_speclist2typ(_tmp6E0,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6E1 != 0){
const char*_tmpD04;void*_tmpD03;(_tmpD03=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD04="ignoring attributes in type",_tag_dyneither(_tmpD04,sizeof(char),28))),_tag_dyneither(_tmpD03,sizeof(void*),0)));}
{struct _tuple8*_tmpD05;yyval=Cyc_YY40(((_tmpD05=_cycalloc(sizeof(*_tmpD05)),((_tmpD05->f1=0,((_tmpD05->f2=_tmp6DF,((_tmpD05->f3=_tmp6E2,_tmpD05)))))))));}
# 2320
break;};}case 258: _LL327: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2321 "parse.y"
struct _tuple25 _tmp6E6=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6E8;struct Cyc_Parse_Type_specifier _tmp6E9;struct Cyc_List_List*_tmp6EA;struct _tuple25 _tmp6E7=_tmp6E6;_tmp6E8=_tmp6E7.f1;_tmp6E9=_tmp6E7.f2;_tmp6EA=_tmp6E7.f3;{
void*_tmp6EB=Cyc_Parse_speclist2typ(_tmp6E9,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6EC=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6ED=Cyc_Parse_apply_tms(_tmp6E8,_tmp6EB,_tmp6EA,_tmp6EC);
if(_tmp6ED.f3 != 0){
# 2327
const char*_tmpD08;void*_tmpD07;(_tmpD07=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD08="bad type params, ignoring",_tag_dyneither(_tmpD08,sizeof(char),26))),_tag_dyneither(_tmpD07,sizeof(void*),0)));}
if(_tmp6ED.f4 != 0){
const char*_tmpD0B;void*_tmpD0A;(_tmpD0A=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD0B="bad specifiers, ignoring",_tag_dyneither(_tmpD0B,sizeof(char),25))),_tag_dyneither(_tmpD0A,sizeof(void*),0)));}
{struct _tuple8*_tmpD0C;yyval=Cyc_YY40(((_tmpD0C=_cycalloc(sizeof(*_tmpD0C)),((_tmpD0C->f1=0,((_tmpD0C->f2=_tmp6ED.f1,((_tmpD0C->f3=_tmp6ED.f2,_tmpD0C)))))))));}
# 2332
break;};}case 259: _LL328: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2335 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 260: _LL329: {
# 2338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2336 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD0F;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD0E;yyval=Cyc_YY47((void*)((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=((_tmpD0F.tag=24,((_tmpD0F.f1=0,_tmpD0F)))),_tmpD0E)))));}
break;}case 261: _LL32A: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2337 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD12;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD11;yyval=Cyc_YY47((void*)((_tmpD11=_cycalloc(sizeof(*_tmpD11)),((_tmpD11[0]=((_tmpD12.tag=24,((_tmpD12.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpD12)))),_tmpD11)))));}
break;}case 262: _LL32B: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2338 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD15;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD14;yyval=Cyc_YY47((void*)((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14[0]=((_tmpD15.tag=25,((_tmpD15.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpD15)))),_tmpD14)))));}
break;}case 263: _LL32C: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2339 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD1B;struct Cyc_List_List*_tmpD1A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD19;yyval=Cyc_YY47((void*)((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19[0]=((_tmpD1B.tag=24,((_tmpD1B.f1=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD1A->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpD1A)))))),_tmpD1B)))),_tmpD19)))));}
break;}case 264: _LL32D: {
# 2342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD1C;yyval=Cyc_YY43(((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD1C->tl=0,_tmpD1C)))))));}
break;}case 265: _LL32E: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2346 "parse.y"
{struct Cyc_List_List*_tmpD1D;yyval=Cyc_YY43(((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpD1D->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpD1D)))))));}
break;}case 266: _LL32F: {
# 2349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2351 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD1E;yyval=Cyc_YY33(((_tmpD1E.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpD1E)));}
break;}case 267: _LL330: {
# 2354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2353 "parse.y"
yyval=yyyyvsp[0];
break;}case 268: _LL331: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2355 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD1F;yyval=Cyc_YY33(((_tmpD1F.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpD1F)));}
break;}case 269: _LL332: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2360 "parse.y"
yyval=yyyyvsp[1];
break;}case 270: _LL333: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2362 "parse.y"
{struct Cyc_List_List*_tmpD29;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD28;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD27;struct Cyc_Parse_Abstractdeclarator _tmpD26;yyval=Cyc_YY33(((_tmpD26.tms=((_tmpD29=_region_malloc(yyr,sizeof(*_tmpD29)),((_tmpD29->hd=(void*)((_tmpD27=_region_malloc(yyr,sizeof(*_tmpD27)),((_tmpD27[0]=((_tmpD28.tag=0,((_tmpD28.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD28.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD28)))))),_tmpD27)))),((_tmpD29->tl=0,_tmpD29)))))),_tmpD26)));}
break;}case 271: _LL334: {
# 2365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2364 "parse.y"
{struct Cyc_List_List*_tmpD33;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD32;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD31;struct Cyc_Parse_Abstractdeclarator _tmpD30;yyval=Cyc_YY33(((_tmpD30.tms=((_tmpD33=_region_malloc(yyr,sizeof(*_tmpD33)),((_tmpD33->hd=(void*)((_tmpD31=_region_malloc(yyr,sizeof(*_tmpD31)),((_tmpD31[0]=((_tmpD32.tag=0,((_tmpD32.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD32.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD32)))))),_tmpD31)))),((_tmpD33->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD33)))))),_tmpD30)));}
break;}case 272: _LL335: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2366 "parse.y"
{struct Cyc_List_List*_tmpD3D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD3C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3B;struct Cyc_Parse_Abstractdeclarator _tmpD3A;yyval=Cyc_YY33(((_tmpD3A.tms=((_tmpD3D=_region_malloc(yyr,sizeof(*_tmpD3D)),((_tmpD3D->hd=(void*)((_tmpD3B=_region_malloc(yyr,sizeof(*_tmpD3B)),((_tmpD3B[0]=((_tmpD3C.tag=1,((_tmpD3C.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD3C.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD3C.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD3C)))))))),_tmpD3B)))),((_tmpD3D->tl=0,_tmpD3D)))))),_tmpD3A)));}
break;}case 273: _LL336: {
# 2369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2368 "parse.y"
{struct Cyc_List_List*_tmpD47;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD46;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD45;struct Cyc_Parse_Abstractdeclarator _tmpD44;yyval=Cyc_YY33(((_tmpD44.tms=((_tmpD47=_region_malloc(yyr,sizeof(*_tmpD47)),((_tmpD47->hd=(void*)((_tmpD45=_region_malloc(yyr,sizeof(*_tmpD45)),((_tmpD45[0]=((_tmpD46.tag=1,((_tmpD46.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD46.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD46.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD46)))))))),_tmpD45)))),((_tmpD47->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD47)))))),_tmpD44)));}
# 2371
break;}case 274: _LL337: {
# 2373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2372 "parse.y"
{struct Cyc_List_List*_tmpD5C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD5B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD5A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD59;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD58;struct Cyc_Parse_Abstractdeclarator _tmpD57;yyval=Cyc_YY33(((_tmpD57.tms=((_tmpD5C=_region_malloc(yyr,sizeof(*_tmpD5C)),((_tmpD5C->hd=(void*)((_tmpD58=_region_malloc(yyr,sizeof(*_tmpD58)),((_tmpD58[0]=((_tmpD5B.tag=3,((_tmpD5B.f1=(void*)((_tmpD59=_region_malloc(yyr,sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5A.tag=1,((_tmpD5A.f1=0,((_tmpD5A.f2=0,((_tmpD5A.f3=0,((_tmpD5A.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD5A.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD5A)))))))))))),_tmpD59)))),_tmpD5B)))),_tmpD58)))),((_tmpD5C->tl=0,_tmpD5C)))))),_tmpD57)));}
# 2374
break;}case 275: _LL338: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2375 "parse.y"
struct _tuple26*_tmp716=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp718;int _tmp719;struct Cyc_Absyn_VarargInfo*_tmp71A;void*_tmp71B;struct Cyc_List_List*_tmp71C;struct _tuple26*_tmp717=_tmp716;_tmp718=_tmp717->f1;_tmp719=_tmp717->f2;_tmp71A=_tmp717->f3;_tmp71B=_tmp717->f4;_tmp71C=_tmp717->f5;
{struct Cyc_List_List*_tmpD71;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD70;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD6F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD6E;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD6D;struct Cyc_Parse_Abstractdeclarator _tmpD6C;yyval=Cyc_YY33(((_tmpD6C.tms=((_tmpD71=_region_malloc(yyr,sizeof(*_tmpD71)),((_tmpD71->hd=(void*)((_tmpD6D=_region_malloc(yyr,sizeof(*_tmpD6D)),((_tmpD6D[0]=((_tmpD70.tag=3,((_tmpD70.f1=(void*)((_tmpD6E=_region_malloc(yyr,sizeof(*_tmpD6E)),((_tmpD6E[0]=((_tmpD6F.tag=1,((_tmpD6F.f1=_tmp718,((_tmpD6F.f2=_tmp719,((_tmpD6F.f3=_tmp71A,((_tmpD6F.f4=_tmp71B,((_tmpD6F.f5=_tmp71C,_tmpD6F)))))))))))),_tmpD6E)))),_tmpD70)))),_tmpD6D)))),((_tmpD71->tl=0,_tmpD71)))))),_tmpD6C)));}
# 2378
break;}case 276: _LL339: {
# 2380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2379 "parse.y"
{struct Cyc_List_List*_tmpD86;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD85;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD84;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD83;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD82;struct Cyc_Parse_Abstractdeclarator _tmpD81;yyval=Cyc_YY33(((_tmpD81.tms=((_tmpD86=_region_malloc(yyr,sizeof(*_tmpD86)),((_tmpD86->hd=(void*)((_tmpD82=_region_malloc(yyr,sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD85.tag=3,((_tmpD85.f1=(void*)((_tmpD83=_region_malloc(yyr,sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=1,((_tmpD84.f1=0,((_tmpD84.f2=0,((_tmpD84.f3=0,((_tmpD84.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpD84.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpD84)))))))))))),_tmpD83)))),_tmpD85)))),_tmpD82)))),((_tmpD86->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD86)))))),_tmpD81)));}
# 2382
break;}case 277: _LL33A: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2383 "parse.y"
struct _tuple26*_tmp729=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp72B;int _tmp72C;struct Cyc_Absyn_VarargInfo*_tmp72D;void*_tmp72E;struct Cyc_List_List*_tmp72F;struct _tuple26*_tmp72A=_tmp729;_tmp72B=_tmp72A->f1;_tmp72C=_tmp72A->f2;_tmp72D=_tmp72A->f3;_tmp72E=_tmp72A->f4;_tmp72F=_tmp72A->f5;
{struct Cyc_List_List*_tmpD9B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD9A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD99;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD98;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD97;struct Cyc_Parse_Abstractdeclarator _tmpD96;yyval=Cyc_YY33(((_tmpD96.tms=((_tmpD9B=_region_malloc(yyr,sizeof(*_tmpD9B)),((_tmpD9B->hd=(void*)((_tmpD97=_region_malloc(yyr,sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD9A.tag=3,((_tmpD9A.f1=(void*)((_tmpD98=_region_malloc(yyr,sizeof(*_tmpD98)),((_tmpD98[0]=((_tmpD99.tag=1,((_tmpD99.f1=_tmp72B,((_tmpD99.f2=_tmp72C,((_tmpD99.f3=_tmp72D,((_tmpD99.f4=_tmp72E,((_tmpD99.f5=_tmp72F,_tmpD99)))))))))))),_tmpD98)))),_tmpD9A)))),_tmpD97)))),((_tmpD9B->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD9B)))))),_tmpD96)));}
# 2387
break;}case 278: _LL33B: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2389 "parse.y"
struct Cyc_List_List*_tmp736=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDA5;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDA4;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDA3;struct Cyc_Parse_Abstractdeclarator _tmpDA2;yyval=Cyc_YY33(((_tmpDA2.tms=((_tmpDA5=_region_malloc(yyr,sizeof(*_tmpDA5)),((_tmpDA5->hd=(void*)((_tmpDA3=_region_malloc(yyr,sizeof(*_tmpDA3)),((_tmpDA3[0]=((_tmpDA4.tag=4,((_tmpDA4.f1=_tmp736,((_tmpDA4.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDA4.f3=0,_tmpDA4)))))))),_tmpDA3)))),((_tmpDA5->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDA5)))))),_tmpDA2)));}
# 2393
break;}case 279: _LL33C: {
# 2395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2394 "parse.y"
{struct Cyc_List_List*_tmpDAF;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDAE;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDAD;struct Cyc_Parse_Abstractdeclarator _tmpDAC;yyval=Cyc_YY33(((_tmpDAC.tms=((_tmpDAF=_region_malloc(yyr,sizeof(*_tmpDAF)),((_tmpDAF->hd=(void*)((_tmpDAD=_region_malloc(yyr,sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=5,((_tmpDAE.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDAE.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpDAE)))))),_tmpDAD)))),((_tmpDAF->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDAF)))))),_tmpDAC)));}
# 2396
break;}case 280: _LL33D: {
# 2398
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2400 "parse.y"
yyval=yyyyvsp[0];
break;}case 281: _LL33E: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2401 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL33F: {
# 2404
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2406
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2402 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL340: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2403 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL341: {
# 2406
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2404 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL342: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2405 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL343: {
# 2408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2408 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDB2;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDB1;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1[0]=((_tmpDB2.tag=16,((_tmpDB2.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpDB2)))),_tmpDB1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 287: _LL344:
# 2411
 yyval=Cyc_YY5(0);
break;case 288: _LL345: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2414 "parse.y"
{const char*_tmpDB3;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDB3="open",_tag_dyneither(_tmpDB3,sizeof(char),5))))!= 0){
const char*_tmpDB4;Cyc_Parse_err(((_tmpDB4="expecting `open'",_tag_dyneither(_tmpDB4,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2418
break;}case 289: _LL346: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDBA;struct _dyneither_ptr*_tmpDB9;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDB8;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8[0]=((_tmpDBA.tag=13,((_tmpDBA.f1=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDB9)))),((_tmpDBA.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpDBA)))))),_tmpDB8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 290: _LL347: {
# 2426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2427 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 291: _LL348: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2428 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL349: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2433 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34A: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2434 "parse.y"
yyval=yyyyvsp[1];
break;}case 294: _LL34B: {
# 2437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2439 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 295: _LL34C: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2440 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 296: _LL34D: {
# 2443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2441 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDC0;struct _dyneither_ptr*_tmpDBF;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDBE;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE[0]=((_tmpDC0.tag=13,((_tmpDC0.f1=((_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDBF)))),((_tmpDC0.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDC0)))))),_tmpDBE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2443
break;}case 297: _LL34E: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2443 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDC6;struct _dyneither_ptr*_tmpDC5;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDC4;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4[0]=((_tmpDC6.tag=13,((_tmpDC6.f1=((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDC5)))),((_tmpDC6.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDC6)))))),_tmpDC4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2445
break;}case 298: _LL34F: {
# 2447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2445 "parse.y"
yyval=yyyyvsp[0];
break;}case 299: _LL350: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2446 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 300: _LL351: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2447 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDC9;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDC8;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDC8=_cycalloc(sizeof(*_tmpDC8)),((_tmpDC8[0]=((_tmpDC9.tag=1,((_tmpDC9.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDC9)))),_tmpDC8)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 301: _LL352: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2450 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDCC;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDCB;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCC.tag=1,((_tmpDCC.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDCC)))),_tmpDCB)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 302: _LL353: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2455 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 303: _LL354: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2457 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL355: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL356: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2466 "parse.y"
struct Cyc_Absyn_Exp*_tmp750=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp750,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL357: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2470 "parse.y"
struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp751,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2473
break;}case 307: _LL358: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2476 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 308: _LL359:
# 2479
 yyval=Cyc_YY11(0);
break;case 309: _LL35A: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2492 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDCF;struct Cyc_List_List*_tmpDCE;yyval=Cyc_YY11(((_tmpDCE=_cycalloc(sizeof(*_tmpDCE)),((_tmpDCE->hd=((_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDCF->pat_vars=0,((_tmpDCF->where_clause=0,((_tmpDCF->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDCF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDCF)))))))))))),((_tmpDCE->tl=0,_tmpDCE)))))));}
# 2495
break;}case 310: _LL35B: {
# 2497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2496 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD2;struct Cyc_List_List*_tmpDD1;yyval=Cyc_YY11(((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->hd=((_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDD2->pat_vars=0,((_tmpDD2->where_clause=0,((_tmpDD2->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDD2->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD2)))))))))))),((_tmpDD1->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDD1)))))));}
break;}case 311: _LL35C: {
# 2501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2500 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD5;struct Cyc_List_List*_tmpDD4;yyval=Cyc_YY11(((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->hd=((_tmpDD5=_cycalloc(sizeof(*_tmpDD5)),((_tmpDD5->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDD5->pat_vars=0,((_tmpDD5->where_clause=0,((_tmpDD5->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDD5->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD5)))))))))))),((_tmpDD4->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDD4)))))));}
break;}case 312: _LL35D: {
# 2503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2502 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDD8;struct Cyc_List_List*_tmpDD7;yyval=Cyc_YY11(((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->hd=((_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDD8->pat_vars=0,((_tmpDD8->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDD8->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpDD8->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDD8)))))))))))),((_tmpDD7->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpDD7)))))));}
break;}case 313: _LL35E: {
# 2507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2506 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDDB;struct Cyc_List_List*_tmpDDA;yyval=Cyc_YY11(((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->hd=((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDDB->pat_vars=0,((_tmpDDB->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpDDB->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpDDB->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDDB)))))))))))),((_tmpDDA->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpDDA)))))));}
break;}case 314: _LL35F: {
# 2509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2513 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 315: _LL360: {
# 2516
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2518
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2517 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL361: {
# 2520
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2522
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2521 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL362: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2524 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL363: {
# 2528
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2530
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2527 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL364: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2530 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL365: {
# 2534
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2533 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL366: {
# 2537
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2539
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2536 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL367: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2539 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL368: {
# 2543
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2545
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2542 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL369: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2545 "parse.y"
struct Cyc_List_List*_tmp75C=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75D=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp75C,_tmp75D));
# 2550
break;}case 325: _LL36A: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2551 "parse.y"
struct Cyc_List_List*_tmp75E=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp75F=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp75E,_tmp75F));
# 2556
break;}case 326: _LL36B: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2557 "parse.y"
struct Cyc_List_List*_tmp760=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp761=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp760,_tmp761));
# 2562
break;}case 327: _LL36C: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2563 "parse.y"
struct Cyc_List_List*_tmp762=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp763=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp762,_tmp763));
# 2568
break;}case 328: _LL36D: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpDDC;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpDDC)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 329: _LL36E: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2574 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 330: _LL36F: {
# 2577
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2575 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL370: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2576 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL371: {
# 2579
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2581
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2577 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL372: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2579 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL373: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2580 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL374: {
# 2583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2582 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL375: {
# 2585
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2587
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2591 "parse.y"
yyval=yyyyvsp[0];
break;}case 337: _LL376: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2594 "parse.y"
yyval=yyyyvsp[0];
break;}case 338: _LL377: {
# 2597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2596 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 339: _LL378: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2599 "parse.y"
yyval=yyyyvsp[0];
break;}case 340: _LL379: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2601 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 341: _LL37A: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2604 "parse.y"
yyval=yyyyvsp[0];
break;}case 342: _LL37B: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2606 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 343: _LL37C: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2609 "parse.y"
yyval=yyyyvsp[0];
break;}case 344: _LL37D: {
# 2612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2611 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 345: _LL37E: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2614 "parse.y"
yyval=yyyyvsp[0];
break;}case 346: _LL37F: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2616 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 347: _LL380: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2619 "parse.y"
yyval=yyyyvsp[0];
break;}case 348: _LL381: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2621 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 349: _LL382: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2624 "parse.y"
yyval=yyyyvsp[0];
break;}case 350: _LL383: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2626 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 351: _LL384: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2628 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL385: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2631 "parse.y"
yyval=yyyyvsp[0];
break;}case 353: _LL386: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2633 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 354: _LL387: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2635 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL388: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2637 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL389: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2639 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38A: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2642 "parse.y"
yyval=yyyyvsp[0];
break;}case 358: _LL38B: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2644 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 359: _LL38C: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2646 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL38D: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2649 "parse.y"
yyval=yyyyvsp[0];
break;}case 361: _LL38E: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2651 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 362: _LL38F: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2653 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL390: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2656 "parse.y"
yyval=yyyyvsp[0];
break;}case 364: _LL391: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2658 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 365: _LL392: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2660 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL393: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2662 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL394: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2665 "parse.y"
yyval=yyyyvsp[0];
break;}case 368: _LL395: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2667 "parse.y"
void*_tmp765=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp765,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2670
break;}case 369: _LL396: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2673 "parse.y"
yyval=yyyyvsp[0];
break;}case 370: _LL397: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2676 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 371: _LL398: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2678 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 372: _LL399: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2680 "parse.y"
void*_tmp766=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp766,(unsigned int)(yyyylsp[0]).first_line)));
# 2683
break;}case 373: _LL39A: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2684 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 374: _LL39B: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2686 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL39C: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2691 "parse.y"
yyval=yyyyvsp[0];
break;}case 376: _LL39D: {
# 2694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2700 "parse.y"
yyval=yyyyvsp[0];
break;}case 377: _LL39E: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2705 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 378: _LL39F: {
# 2708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2707 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 379: _LL3A0: {
# 2710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2709 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp767=e->r;void*_tmp768=_tmp767;enum Cyc_Absyn_Sign _tmp76A;char _tmp76B;enum Cyc_Absyn_Sign _tmp76D;short _tmp76E;enum Cyc_Absyn_Sign _tmp770;int _tmp771;struct _dyneither_ptr _tmp773;int _tmp774;_LL3A3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp769=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp769->tag != 0)goto _LL3A5;else{if(((_tmp769->f1).Char_c).tag != 2)goto _LL3A5;_tmp76A=((struct _tuple3)((_tmp769->f1).Char_c).val).f1;_tmp76B=((struct _tuple3)((_tmp769->f1).Char_c).val).f2;}}_LL3A4:
# 2713
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpDDF;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpDDE;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDDE=_cycalloc_atomic(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDF.tag=11,((_tmpDDF.f1=_tmp76B,_tmpDDF)))),_tmpDDE)))),e->loc));}goto _LL3A2;_LL3A5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp76C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp76C->tag != 0)goto _LL3A7;else{if(((_tmp76C->f1).Short_c).tag != 4)goto _LL3A7;_tmp76D=((struct _tuple4)((_tmp76C->f1).Short_c).val).f1;_tmp76E=((struct _tuple4)((_tmp76C->f1).Short_c).val).f2;}}_LL3A6:
# 2715
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDE2;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDE1;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE1=_cycalloc_atomic(sizeof(*_tmpDE1)),((_tmpDE1[0]=((_tmpDE2.tag=10,((_tmpDE2.f1=_tmp76D,((_tmpDE2.f2=(int)_tmp76E,_tmpDE2)))))),_tmpDE1)))),e->loc));}goto _LL3A2;_LL3A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp76F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp76F->tag != 0)goto _LL3A9;else{if(((_tmp76F->f1).Int_c).tag != 5)goto _LL3A9;_tmp770=((struct _tuple5)((_tmp76F->f1).Int_c).val).f1;_tmp771=((struct _tuple5)((_tmp76F->f1).Int_c).val).f2;}}_LL3A8:
# 2717
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpDE5;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpDE4;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE4=_cycalloc_atomic(sizeof(*_tmpDE4)),((_tmpDE4[0]=((_tmpDE5.tag=10,((_tmpDE5.f1=_tmp770,((_tmpDE5.f2=_tmp771,_tmpDE5)))))),_tmpDE4)))),e->loc));}goto _LL3A2;_LL3A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp772=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp772->tag != 0)goto _LL3AB;else{if(((_tmp772->f1).Float_c).tag != 7)goto _LL3AB;_tmp773=((struct _tuple7)((_tmp772->f1).Float_c).val).f1;_tmp774=((struct _tuple7)((_tmp772->f1).Float_c).val).f2;}}_LL3AA:
# 2719
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpDE8;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpDE7;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE8.tag=12,((_tmpDE8.f1=_tmp773,((_tmpDE8.f2=_tmp774,_tmpDE8)))))),_tmpDE7)))),e->loc));}goto _LL3A2;_LL3AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp775=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp775->tag != 0)goto _LL3AD;else{if(((_tmp775->f1).Null_c).tag != 1)goto _LL3AD;}}_LL3AC:
# 2721
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A2;_LL3AD: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp776=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp776->tag != 0)goto _LL3AF;else{if(((_tmp776->f1).String_c).tag != 8)goto _LL3AF;}}_LL3AE:
# 2723
{const char*_tmpDE9;Cyc_Parse_err(((_tmpDE9="strings cannot occur within patterns",_tag_dyneither(_tmpDE9,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3AF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp777=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp777->tag != 0)goto _LL3B1;else{if(((_tmp777->f1).Wstring_c).tag != 9)goto _LL3B1;}}_LL3B0:
# 2725
{const char*_tmpDEA;Cyc_Parse_err(((_tmpDEA="strings cannot occur within patterns",_tag_dyneither(_tmpDEA,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3B1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp778=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp768;if(_tmp778->tag != 0)goto _LL3B3;else{if(((_tmp778->f1).LongLong_c).tag != 6)goto _LL3B3;}}_LL3B2:
# 2727
{const char*_tmpDEB;Cyc_Parse_unimp(((_tmpDEB="long long's in patterns",_tag_dyneither(_tmpDEB,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A2;_LL3B3:;_LL3B4: {
# 2729
const char*_tmpDEC;Cyc_Parse_err(((_tmpDEC="bad constant in case",_tag_dyneither(_tmpDEC,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A2:;}
# 2732
break;}case 380: _LL3A1: {
# 2734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2733 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpDEF;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpDEE;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDEF.tag=15,((_tmpDEF.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpDEF)))),_tmpDEE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 381: _LL3B5: {
# 2736
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2738
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2735 "parse.y"
{const char*_tmpDF0;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDF0="as",_tag_dyneither(_tmpDF0,sizeof(char),3))))!= 0){
const char*_tmpDF1;Cyc_Parse_err(((_tmpDF1="expecting `as'",_tag_dyneither(_tmpDF1,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDFB;struct _dyneither_ptr*_tmpDFA;struct _tuple0*_tmpDF9;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDF8;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDFB.tag=1,((_tmpDFB.f1=Cyc_Absyn_new_vardecl(((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->f1=Cyc_Absyn_Loc_n,((_tmpDF9->f2=((_tmpDFA=_cycalloc(sizeof(*_tmpDFA)),((_tmpDFA[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDFA)))),_tmpDF9)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpDFB.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpDFB)))))),_tmpDF8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2740
break;}case 382: _LL3B6: {
# 2742
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2744
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2741 "parse.y"
{const char*_tmpDFC;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpDFC="alias",_tag_dyneither(_tmpDFC,sizeof(char),6))))!= 0){
const char*_tmpDFD;Cyc_Parse_err(((_tmpDFD="expecting `alias'",_tag_dyneither(_tmpDFD,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp78F=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE05;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE04;struct _dyneither_ptr*_tmpE03;struct Cyc_Absyn_Tvar*_tmpE02;struct Cyc_Absyn_Tvar*tv=(_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->name=((_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE03)))),((_tmpE02->identity=- 1,((_tmpE02->kind=(void*)((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05[0]=((_tmpE04.tag=0,((_tmpE04.f1=& Cyc_Tcutil_rk,_tmpE04)))),_tmpE05)))),_tmpE02)))))));
struct _dyneither_ptr*_tmpE08;struct _tuple0*_tmpE07;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->f1=Cyc_Absyn_Loc_n,((_tmpE07->f2=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE08)))),_tmpE07)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE0B;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE0A;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A[0]=((_tmpE0B.tag=2,((_tmpE0B.f1=tv,((_tmpE0B.f2=vd,_tmpE0B)))))),_tmpE0A)))),(unsigned int)_tmp78F));}
# 2749
break;};}case 383: _LL3B7: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2750 "parse.y"
struct _tuple22 _tmp798=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp79A;int _tmp79B;struct _tuple22 _tmp799=_tmp798;_tmp79A=_tmp799.f1;_tmp79B=_tmp799.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE0E;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE0D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D[0]=((_tmpE0E.tag=5,((_tmpE0E.f1=_tmp79A,((_tmpE0E.f2=_tmp79B,_tmpE0E)))))),_tmpE0D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2753
break;}case 384: _LL3B8: {
# 2755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2754 "parse.y"
struct _tuple22 _tmp79E=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp7A0;int _tmp7A1;struct _tuple22 _tmp79F=_tmp79E;_tmp7A0=_tmp79F.f1;_tmp7A1=_tmp79F.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE11;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE10;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE10=_cycalloc(sizeof(*_tmpE10)),((_tmpE10[0]=((_tmpE11.tag=16,((_tmpE11.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE11.f2=_tmp7A0,((_tmpE11.f3=_tmp7A1,_tmpE11)))))))),_tmpE10)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2757
break;}case 385: _LL3B9: {
# 2759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2758 "parse.y"
struct _tuple22 _tmp7A4=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp7A6;int _tmp7A7;struct _tuple22 _tmp7A5=_tmp7A4;_tmp7A6=_tmp7A5.f1;_tmp7A7=_tmp7A5.f2;{
struct Cyc_List_List*_tmp7A8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE17;struct Cyc_Absyn_AggrInfo*_tmpE16;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE15;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE15=_cycalloc(sizeof(*_tmpE15)),((_tmpE15[0]=((_tmpE17.tag=7,((_tmpE17.f1=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE16->targs=0,_tmpE16)))))),((_tmpE17.f2=_tmp7A8,((_tmpE17.f3=_tmp7A6,((_tmpE17.f4=_tmp7A7,_tmpE17)))))))))),_tmpE15)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2763
break;};}case 386: _LL3BA: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2764 "parse.y"
struct _tuple22 _tmp7AC=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp7AE;int _tmp7AF;struct _tuple22 _tmp7AD=_tmp7AC;_tmp7AE=_tmp7AD.f1;_tmp7AF=_tmp7AD.f2;{
struct Cyc_List_List*_tmp7B0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE1A;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE19;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE19=_cycalloc(sizeof(*_tmpE19)),((_tmpE19[0]=((_tmpE1A.tag=7,((_tmpE1A.f1=0,((_tmpE1A.f2=_tmp7B0,((_tmpE1A.f3=_tmp7AE,((_tmpE1A.f4=_tmp7AF,_tmpE1A)))))))))),_tmpE19)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2768
break;};}case 387: _LL3BB: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2769 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE1D;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE1C;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE1C=_cycalloc(sizeof(*_tmpE1C)),((_tmpE1C[0]=((_tmpE1D.tag=6,((_tmpE1D.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE1D)))),_tmpE1C)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 388: _LL3BC: {
# 2772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2771 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE27;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE26;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE25;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE24;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=((_tmpE27.tag=6,((_tmpE27.f1=Cyc_Absyn_new_pat((void*)((_tmpE25=_cycalloc(sizeof(*_tmpE25)),((_tmpE25[0]=((_tmpE26.tag=6,((_tmpE26.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE26)))),_tmpE25)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE27)))),_tmpE24)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3BD: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2773 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE31;struct _dyneither_ptr*_tmpE30;struct _tuple0*_tmpE2F;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE2E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E[0]=((_tmpE31.tag=3,((_tmpE31.f1=Cyc_Absyn_new_vardecl(((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->f1=Cyc_Absyn_Loc_n,((_tmpE2F->f2=((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE30)))),_tmpE2F)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE31.f2=
# 2775
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE31)))))),_tmpE2E)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2777
break;}case 390: _LL3BE: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2778 "parse.y"
{const char*_tmpE32;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE32="as",_tag_dyneither(_tmpE32,sizeof(char),3))))!= 0){
const char*_tmpE33;Cyc_Parse_err(((_tmpE33="expecting `as'",_tag_dyneither(_tmpE33,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE3D;struct _dyneither_ptr*_tmpE3C;struct _tuple0*_tmpE3B;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE3A;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3D.tag=3,((_tmpE3D.f1=Cyc_Absyn_new_vardecl(((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->f1=Cyc_Absyn_Loc_n,((_tmpE3B->f2=((_tmpE3C=_cycalloc(sizeof(*_tmpE3C)),((_tmpE3C[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE3C)))),_tmpE3B)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE3D.f2=
# 2782
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE3D)))))),_tmpE3A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2784
break;}case 391: _LL3BF: {
# 2786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2785 "parse.y"
void*_tmp7C3=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE4E;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE4D;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE4C;struct _dyneither_ptr*_tmpE4B;struct _tuple0*_tmpE4A;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE49;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49[0]=((_tmpE4E.tag=4,((_tmpE4E.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7C3),((_tmpE4E.f2=
Cyc_Absyn_new_vardecl(((_tmpE4A=_cycalloc(sizeof(*_tmpE4A)),((_tmpE4A->f1=Cyc_Absyn_Loc_n,((_tmpE4A->f2=((_tmpE4B=_cycalloc(sizeof(*_tmpE4B)),((_tmpE4B[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE4B)))),_tmpE4A)))))),(void*)(
(_tmpE4C=_cycalloc(sizeof(*_tmpE4C)),((_tmpE4C[0]=((_tmpE4D.tag=19,((_tmpE4D.f1=_tmp7C3,_tmpE4D)))),_tmpE4C)))),0),_tmpE4E)))))),_tmpE49)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2790
break;}case 392: _LL3C0: {
# 2792
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2791 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7CA=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE6A;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE69;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE68;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE67;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE66;struct _dyneither_ptr*_tmpE65;struct _tuple0*_tmpE64;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE63;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE63=_cycalloc(sizeof(*_tmpE63)),((_tmpE63[0]=((_tmpE6A.tag=4,((_tmpE6A.f1=_tmp7CA,((_tmpE6A.f2=
Cyc_Absyn_new_vardecl(((_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64->f1=Cyc_Absyn_Loc_n,((_tmpE64->f2=((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE65)))),_tmpE64)))))),(void*)(
(_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=((_tmpE69.tag=19,((_tmpE69.f1=(void*)((_tmpE67=_cycalloc(sizeof(*_tmpE67)),((_tmpE67[0]=((_tmpE68.tag=2,((_tmpE68.f1=_tmp7CA,_tmpE68)))),_tmpE67)))),_tmpE69)))),_tmpE66)))),0),_tmpE6A)))))),_tmpE63)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2796
break;}case 393: _LL3C1: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2799 "parse.y"
{struct _tuple22*_tmpE6B;yyval=Cyc_YY13(((_tmpE6B=_cycalloc(sizeof(*_tmpE6B)),((_tmpE6B->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE6B->f2=0,_tmpE6B)))))));}
break;}case 394: _LL3C2: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2800 "parse.y"
{struct _tuple22*_tmpE6C;yyval=Cyc_YY13(((_tmpE6C=_cycalloc(sizeof(*_tmpE6C)),((_tmpE6C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE6C->f2=1,_tmpE6C)))))));}
break;}case 395: _LL3C3: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2801 "parse.y"
{struct _tuple22*_tmpE6D;yyval=Cyc_YY13(((_tmpE6D=_cycalloc(sizeof(*_tmpE6D)),((_tmpE6D->f1=0,((_tmpE6D->f2=1,_tmpE6D)))))));}
break;}case 396: _LL3C4: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2806 "parse.y"
{struct Cyc_List_List*_tmpE6E;yyval=Cyc_YY14(((_tmpE6E=_cycalloc(sizeof(*_tmpE6E)),((_tmpE6E->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE6E->tl=0,_tmpE6E)))))));}
break;}case 397: _LL3C5: {
# 2809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2808 "parse.y"
{struct Cyc_List_List*_tmpE6F;yyval=Cyc_YY14(((_tmpE6F=_cycalloc(sizeof(*_tmpE6F)),((_tmpE6F->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE6F->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE6F)))))));}
break;}case 398: _LL3C6: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2813 "parse.y"
{struct _tuple23*_tmpE70;yyval=Cyc_YY15(((_tmpE70=_cycalloc(sizeof(*_tmpE70)),((_tmpE70->f1=0,((_tmpE70->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE70)))))));}
break;}case 399: _LL3C7: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2815 "parse.y"
{struct _tuple23*_tmpE71;yyval=Cyc_YY15(((_tmpE71=_cycalloc(sizeof(*_tmpE71)),((_tmpE71->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE71->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE71)))))));}
break;}case 400: _LL3C8: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2818 "parse.y"
{struct _tuple22*_tmpE72;yyval=Cyc_YY17(((_tmpE72=_cycalloc(sizeof(*_tmpE72)),((_tmpE72->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE72->f2=0,_tmpE72)))))));}
break;}case 401: _LL3C9: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2819 "parse.y"
{struct _tuple22*_tmpE73;yyval=Cyc_YY17(((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE73->f2=1,_tmpE73)))))));}
break;}case 402: _LL3CA: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820 "parse.y"
{struct _tuple22*_tmpE74;yyval=Cyc_YY17(((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=0,((_tmpE74->f2=1,_tmpE74)))))));}
break;}case 403: _LL3CB: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2825 "parse.y"
{struct Cyc_List_List*_tmpE75;yyval=Cyc_YY16(((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE75->tl=0,_tmpE75)))))));}
break;}case 404: _LL3CC: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2827 "parse.y"
{struct Cyc_List_List*_tmpE76;yyval=Cyc_YY16(((_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpE76->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpE76)))))));}
break;}case 405: _LL3CD: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 406: _LL3CE: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2835 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 407: _LL3CF: {
# 2838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2840 "parse.y"
yyval=yyyyvsp[0];
break;}case 408: _LL3D0: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2842 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 409: _LL3D1: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2844 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3D2: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2848 "parse.y"
yyval=Cyc_YY9(0);
break;}case 411: _LL3D3: {
# 2851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2849 "parse.y"
{struct Cyc_Core_Opt*_tmpE77;yyval=Cyc_YY9(((_tmpE77=_cycalloc_atomic(sizeof(*_tmpE77)),((_tmpE77->v=(void*)Cyc_Absyn_Times,_tmpE77)))));}
break;}case 412: _LL3D4: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2850 "parse.y"
{struct Cyc_Core_Opt*_tmpE78;yyval=Cyc_YY9(((_tmpE78=_cycalloc_atomic(sizeof(*_tmpE78)),((_tmpE78->v=(void*)Cyc_Absyn_Div,_tmpE78)))));}
break;}case 413: _LL3D5: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2851 "parse.y"
{struct Cyc_Core_Opt*_tmpE79;yyval=Cyc_YY9(((_tmpE79=_cycalloc_atomic(sizeof(*_tmpE79)),((_tmpE79->v=(void*)Cyc_Absyn_Mod,_tmpE79)))));}
break;}case 414: _LL3D6: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2852 "parse.y"
{struct Cyc_Core_Opt*_tmpE7A;yyval=Cyc_YY9(((_tmpE7A=_cycalloc_atomic(sizeof(*_tmpE7A)),((_tmpE7A->v=(void*)Cyc_Absyn_Plus,_tmpE7A)))));}
break;}case 415: _LL3D7: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2853 "parse.y"
{struct Cyc_Core_Opt*_tmpE7B;yyval=Cyc_YY9(((_tmpE7B=_cycalloc_atomic(sizeof(*_tmpE7B)),((_tmpE7B->v=(void*)Cyc_Absyn_Minus,_tmpE7B)))));}
break;}case 416: _LL3D8: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
{struct Cyc_Core_Opt*_tmpE7C;yyval=Cyc_YY9(((_tmpE7C=_cycalloc_atomic(sizeof(*_tmpE7C)),((_tmpE7C->v=(void*)Cyc_Absyn_Bitlshift,_tmpE7C)))));}
break;}case 417: _LL3D9: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2855 "parse.y"
{struct Cyc_Core_Opt*_tmpE7D;yyval=Cyc_YY9(((_tmpE7D=_cycalloc_atomic(sizeof(*_tmpE7D)),((_tmpE7D->v=(void*)Cyc_Absyn_Bitlrshift,_tmpE7D)))));}
break;}case 418: _LL3DA: {
# 2858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2856 "parse.y"
{struct Cyc_Core_Opt*_tmpE7E;yyval=Cyc_YY9(((_tmpE7E=_cycalloc_atomic(sizeof(*_tmpE7E)),((_tmpE7E->v=(void*)Cyc_Absyn_Bitand,_tmpE7E)))));}
break;}case 419: _LL3DB: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2857 "parse.y"
{struct Cyc_Core_Opt*_tmpE7F;yyval=Cyc_YY9(((_tmpE7F=_cycalloc_atomic(sizeof(*_tmpE7F)),((_tmpE7F->v=(void*)Cyc_Absyn_Bitxor,_tmpE7F)))));}
break;}case 420: _LL3DC: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2858 "parse.y"
{struct Cyc_Core_Opt*_tmpE80;yyval=Cyc_YY9(((_tmpE80=_cycalloc_atomic(sizeof(*_tmpE80)),((_tmpE80->v=(void*)Cyc_Absyn_Bitor,_tmpE80)))));}
break;}case 421: _LL3DD: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2863 "parse.y"
yyval=yyyyvsp[0];
break;}case 422: _LL3DE: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2865 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 423: _LL3DF: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2868 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E0: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2871 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E1: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2873 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E2: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2875 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E3: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2877 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E4: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2881 "parse.y"
yyval=yyyyvsp[0];
break;}case 429: _LL3E5: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2885 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3E6: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2887 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 431: _LL3E7: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2891 "parse.y"
yyval=yyyyvsp[0];
break;}case 432: _LL3E8: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2893 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 433: _LL3E9: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2897 "parse.y"
yyval=yyyyvsp[0];
break;}case 434: _LL3EA: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2899 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 435: _LL3EB: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2903 "parse.y"
yyval=yyyyvsp[0];
break;}case 436: _LL3EC: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2905 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 437: _LL3ED: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2909 "parse.y"
yyval=yyyyvsp[0];
break;}case 438: _LL3EE: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2911 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 439: _LL3EF: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2915 "parse.y"
yyval=yyyyvsp[0];
break;}case 440: _LL3F0: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2917 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 441: _LL3F1: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2919 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F2: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=yyyyvsp[0];
break;}case 443: _LL3F3: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2925 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 444: _LL3F4: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2927 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3F5: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2929 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3F6: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2931 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F7: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2935 "parse.y"
yyval=yyyyvsp[0];
break;}case 448: _LL3F8: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2937 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 449: _LL3F9: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2939 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FA: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2943 "parse.y"
yyval=yyyyvsp[0];
break;}case 451: _LL3FB: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2945 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 452: _LL3FC: {
# 2948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2947 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL3FD: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2951 "parse.y"
yyval=yyyyvsp[0];
break;}case 454: _LL3FE: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2953 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 455: _LL3FF: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2955 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL400: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2957 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL401: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2961 "parse.y"
yyval=yyyyvsp[0];
break;}case 458: _LL402: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2963 "parse.y"
void*_tmp7E9=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7E9,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2966
break;}case 459: _LL403: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2969 "parse.y"
yyval=yyyyvsp[0];
break;}case 460: _LL404: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2970 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 461: _LL405: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2971 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL406: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2972 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL407: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2973 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL408: {
# 2976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2974 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL409: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2975 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40A: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2977 "parse.y"
void*_tmp7EA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7EA,(unsigned int)(yyyylsp[0]).first_line));
# 2980
break;}case 467: _LL40B: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2980 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 468: _LL40C: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2982 "parse.y"
void*_tmp7EB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7EB,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2985
break;}case 469: _LL40D: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2987 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE86;struct Cyc_Absyn_MallocInfo _tmpE85;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE84;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE84=_cycalloc(sizeof(*_tmpE84)),((_tmpE84[0]=((_tmpE86.tag=33,((_tmpE86.f1=((_tmpE85.is_calloc=0,((_tmpE85.rgn=0,((_tmpE85.elt_type=0,((_tmpE85.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE85.fat_result=0,_tmpE85)))))))))),_tmpE86)))),_tmpE84)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2989
break;}case 470: _LL40E: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2990 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE8C;struct Cyc_Absyn_MallocInfo _tmpE8B;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE8A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE8A=_cycalloc(sizeof(*_tmpE8A)),((_tmpE8A[0]=((_tmpE8C.tag=33,((_tmpE8C.f1=((_tmpE8B.is_calloc=0,((_tmpE8B.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE8B.elt_type=0,((_tmpE8B.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE8B.fat_result=0,_tmpE8B)))))))))),_tmpE8C)))),_tmpE8A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2992
break;}case 471: _LL40F: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2993 "parse.y"
void*_tmp7F2=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpE96;void**_tmpE95;struct Cyc_Absyn_MallocInfo _tmpE94;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE93;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE93=_cycalloc(sizeof(*_tmpE93)),((_tmpE93[0]=((_tmpE96.tag=33,((_tmpE96.f1=((_tmpE94.is_calloc=1,((_tmpE94.rgn=0,((_tmpE94.elt_type=((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95[0]=_tmp7F2,_tmpE95)))),((_tmpE94.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE94.fat_result=0,_tmpE94)))))))))),_tmpE96)))),_tmpE93)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2996
break;}case 472: _LL410: {
# 2998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 2998 "parse.y"
void*_tmp7F7=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEA0;void**_tmpE9F;struct Cyc_Absyn_MallocInfo _tmpE9E;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpE9D;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D[0]=((_tmpEA0.tag=33,((_tmpEA0.f1=((_tmpE9E.is_calloc=1,((_tmpE9E.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpE9E.elt_type=((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F[0]=_tmp7F7,_tmpE9F)))),((_tmpE9E.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpE9E.fat_result=0,_tmpE9E)))))))))),_tmpEA0)))),_tmpE9D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3001
break;}case 473: _LL411: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3002 "parse.y"
{struct Cyc_Absyn_Exp*_tmpEA1[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpEA1[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpEA1,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 474: _LL412: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3004 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEA7;struct _dyneither_ptr*_tmpEA6;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEA5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEA5=_cycalloc(sizeof(*_tmpEA5)),((_tmpEA5[0]=((_tmpEA7.tag=37,((_tmpEA7.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEA7.f2=((_tmpEA6=_cycalloc(sizeof(*_tmpEA6)),((_tmpEA6[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEA6)))),_tmpEA7)))))),_tmpEA5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL413: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3006 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpEAD;struct _dyneither_ptr*_tmpEAC;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpEAB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEAB=_cycalloc(sizeof(*_tmpEAB)),((_tmpEAB[0]=((_tmpEAD.tag=37,((_tmpEAD.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpEAD.f2=((_tmpEAC=_cycalloc(sizeof(*_tmpEAC)),((_tmpEAC[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpEAC)))),_tmpEAD)))))),_tmpEAB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL414: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3008 "parse.y"
void*_tmp803=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp803,(unsigned int)(yyyylsp[0]).first_line));
break;}case 477: _LL415: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3011 "parse.y"
struct _tuple20 _tmp804=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp806;struct _dyneither_ptr _tmp807;struct _tuple20 _tmp805=_tmp804;_tmp806=_tmp805.f1;_tmp807=_tmp805.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp806,_tmp807,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL416: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3016 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 479: _LL417: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3017 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 480: _LL418: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3018 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 481: _LL419: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3023 "parse.y"
yyval=yyyyvsp[0];
break;}case 482: _LL41A: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3025 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 483: _LL41B: {
# 3028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3027 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL41C: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3029 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL41D: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3031 "parse.y"
{struct _dyneither_ptr*_tmpEAE;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEAE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 486: _LL41E: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3033 "parse.y"
{struct _dyneither_ptr*_tmpEAF;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEAF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL41F: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3035 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 488: _LL420: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3037 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL421: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3039 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB2;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB1;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB1=_cycalloc(sizeof(*_tmpEB1)),((_tmpEB1[0]=((_tmpEB2.tag=24,((_tmpEB2.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB2.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB2)))))),_tmpEB1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 490: _LL422: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3041 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEB5;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB5.tag=24,((_tmpEB5.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEB5.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEB5)))))),_tmpEB4)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL423: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3046 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEBF;struct _dyneither_ptr*_tmpEBE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEBD;struct Cyc_List_List*_tmpEBC;yyval=Cyc_YY3(((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC->hd=(void*)((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=((_tmpEBD.tag=0,((_tmpEBD.f1=((_tmpEBE=_cycalloc(sizeof(*_tmpEBE)),((_tmpEBE[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEBE)))),_tmpEBD)))),_tmpEBF)))),((_tmpEBC->tl=0,_tmpEBC)))))));}
break;}case 492: _LL424: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3049 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEC5;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEC4;struct Cyc_List_List*_tmpEC3;yyval=Cyc_YY3(((_tmpEC3=_cycalloc(sizeof(*_tmpEC3)),((_tmpEC3->hd=(void*)((_tmpEC5=_cycalloc_atomic(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC4.tag=1,((_tmpEC4.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpEC4)))),_tmpEC5)))),((_tmpEC3->tl=0,_tmpEC3)))))));}
break;}case 493: _LL425: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3051 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpECF;struct _dyneither_ptr*_tmpECE;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpECD;struct Cyc_List_List*_tmpECC;yyval=Cyc_YY3(((_tmpECC=_cycalloc(sizeof(*_tmpECC)),((_tmpECC->hd=(void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpECD.tag=0,((_tmpECD.f1=((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpECE)))),_tmpECD)))),_tmpECF)))),((_tmpECC->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpECC)))))));}
break;}case 494: _LL426: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3054 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpED5;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpED4;struct Cyc_List_List*_tmpED3;yyval=Cyc_YY3(((_tmpED3=_cycalloc(sizeof(*_tmpED3)),((_tmpED3->hd=(void*)((_tmpED5=_cycalloc_atomic(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED4.tag=1,((_tmpED4.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpED4)))),_tmpED5)))),((_tmpED3->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpED3)))))));}
break;}case 495: _LL427: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 496: _LL428: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3062 "parse.y"
yyval=yyyyvsp[0];
break;}case 497: _LL429: {
# 3065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3064 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 498: _LL42A: {
# 3067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3066 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL42B: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3068 "parse.y"
yyval=yyyyvsp[1];
break;}case 500: _LL42C: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3073 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 501: _LL42D: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3075 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL42E: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3078 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL42F: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3081 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpED8;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpED7;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpED7=_cycalloc(sizeof(*_tmpED7)),((_tmpED7[0]=((_tmpED8.tag=28,((_tmpED8.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpED8.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpED8.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpED8.f4=0,_tmpED8)))))))))),_tmpED7)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 504: _LL430: {
# 3084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3084 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 505: _LL431: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3088 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 506: _LL432: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpED9;yyval=Cyc_YY6(((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpED9->tl=0,_tmpED9)))))));}
break;}case 507: _LL433: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3096 "parse.y"
{struct Cyc_List_List*_tmpEDA;yyval=Cyc_YY6(((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEDA->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpEDA)))))));}
break;}case 508: _LL434: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp820=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp820.f1,_tmp820.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 509: _LL435: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3103 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL436: {
# 3106
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3108
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3104 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL437: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3106 "parse.y"
struct _dyneither_ptr _tmp821=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp821);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp821,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3114
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp821,i,(unsigned int)(yyyylsp[0]).first_line));
# 3116
break;}case 512: _LL438: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 513: _LL439: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3121 "parse.y"
{struct _dyneither_ptr*_tmpEDD;struct _tuple0*_tmpEDC;yyval=Cyc_QualId_tok(((_tmpEDC=_cycalloc(sizeof(*_tmpEDC)),((_tmpEDC->f1=Cyc_Absyn_Rel_n(0),((_tmpEDC->f2=((_tmpEDD=_cycalloc(sizeof(*_tmpEDD)),((_tmpEDD[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEDD)))),_tmpEDC)))))));}
break;}case 514: _LL43A: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3122 "parse.y"
yyval=yyyyvsp[0];
break;}case 515: _LL43B: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3125 "parse.y"
{struct _dyneither_ptr*_tmpEE0;struct _tuple0*_tmpEDF;yyval=Cyc_QualId_tok(((_tmpEDF=_cycalloc(sizeof(*_tmpEDF)),((_tmpEDF->f1=Cyc_Absyn_Rel_n(0),((_tmpEDF->f2=((_tmpEE0=_cycalloc(sizeof(*_tmpEE0)),((_tmpEE0[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE0)))),_tmpEDF)))))));}
break;}case 516: _LL43C: {
# 3128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3126 "parse.y"
yyval=yyyyvsp[0];
break;}case 517: _LL43D: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL43E: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3132 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL43F: {
# 3135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL440: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL441: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3141 "parse.y"
break;}case 522: _LL442: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3141 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL443:
# 3146
 break;}
# 371 "cyc_bison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 374
yylsp_offset -=yylen;
# 388 "cyc_bison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 391
++ yylsp_offset;
if(yylen == 0){
# 394
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 401
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 411
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(523,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6894) && Cyc_yycheck[_check_known_subscript_notnull(6895,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6895,yystate)];else{
# 417
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(144,yyn - 146)];}
# 419
goto yynewstate;
# 421
yyerrlab:
# 423
 if(yyerrstatus == 0){
# 426
++ yynerrs;
# 429
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
# 431
if(yyn > - 32768  && yyn < 6894){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6895,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)])+ 15,count ++);}
{unsigned int _tmpEE5;unsigned int _tmpEE4;struct _dyneither_ptr _tmpEE3;char*_tmpEE2;unsigned int _tmpEE1;msg=((_tmpEE1=(unsigned int)(sze + 15),((_tmpEE2=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpEE1 + 1)),((_tmpEE3=_tag_dyneither(_tmpEE2,sizeof(char),_tmpEE1 + 1),((((_tmpEE4=_tmpEE1,((_tmpEE6(& _tmpEE5,& _tmpEE4,& _tmpEE2),_tmpEE2[_tmpEE4]=(char)0)))),_tmpEE3))))))));}
{const char*_tmpEE7;Cyc_strcpy(msg,((_tmpEE7="parse error",_tag_dyneither(_tmpEE7,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6895,x + yyn)]== x){
# 453
{const char*_tmpEE9;const char*_tmpEE8;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpEE9=", expecting `",_tag_dyneither(_tmpEE9,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpEE8=" or `",_tag_dyneither(_tmpEE8,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)]);
{const char*_tmpEEA;Cyc_strcat(msg,((_tmpEEA="'",_tag_dyneither(_tmpEEA,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg);}else{
# 465
const char*_tmpEEB;Cyc_yyerror(((_tmpEEB="parse error",_tag_dyneither(_tmpEEB,sizeof(char),12))));}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp82F=1;_npop_handler(0);return _tmp82F;}
# 485
yychar=-2;}
# 491
yyerrstatus=3;
# 493
goto yyerrhandle;
# 495
yyerrdefault:
# 505 "cyc_bison.simple"
 yyerrpop:
# 507
 if(yyssp_offset == 0){int _tmp830=1;_npop_handler(0);return _tmp830;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1032,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6894) || Cyc_yycheck[_check_known_subscript_notnull(6895,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6895,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1031){
int _tmp831=0;_npop_handler(0);return _tmp831;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3144 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp83B=v;int _tmp83C;char _tmp83D;struct _dyneither_ptr _tmp83E;union Cyc_Absyn_Nmspace _tmp83F;struct _dyneither_ptr*_tmp840;_LL446: if((_tmp83B.Int_tok).tag != 1)goto _LL448;_tmp83C=((struct _tuple5)(_tmp83B.Int_tok).val).f2;_LL447:
{const char*_tmpEEF;void*_tmpEEE[1];struct Cyc_Int_pa_PrintArg_struct _tmpEED;(_tmpEED.tag=1,((_tmpEED.f1=(unsigned long)_tmp83C,((_tmpEEE[0]=& _tmpEED,Cyc_fprintf(Cyc_stderr,((_tmpEEF="%d",_tag_dyneither(_tmpEEF,sizeof(char),3))),_tag_dyneither(_tmpEEE,sizeof(void*),1)))))));}goto _LL445;_LL448: if((_tmp83B.Char_tok).tag != 2)goto _LL44A;_tmp83D=(char)(_tmp83B.Char_tok).val;_LL449:
{const char*_tmpEF3;void*_tmpEF2[1];struct Cyc_Int_pa_PrintArg_struct _tmpEF1;(_tmpEF1.tag=1,((_tmpEF1.f1=(unsigned long)((int)_tmp83D),((_tmpEF2[0]=& _tmpEF1,Cyc_fprintf(Cyc_stderr,((_tmpEF3="%c",_tag_dyneither(_tmpEF3,sizeof(char),3))),_tag_dyneither(_tmpEF2,sizeof(void*),1)))))));}goto _LL445;_LL44A: if((_tmp83B.String_tok).tag != 3)goto _LL44C;_tmp83E=(struct _dyneither_ptr)(_tmp83B.String_tok).val;_LL44B:
{const char*_tmpEF7;void*_tmpEF6[1];struct Cyc_String_pa_PrintArg_struct _tmpEF5;(_tmpEF5.tag=0,((_tmpEF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp83E),((_tmpEF6[0]=& _tmpEF5,Cyc_fprintf(Cyc_stderr,((_tmpEF7="\"%s\"",_tag_dyneither(_tmpEF7,sizeof(char),5))),_tag_dyneither(_tmpEF6,sizeof(void*),1)))))));}goto _LL445;_LL44C: if((_tmp83B.QualId_tok).tag != 5)goto _LL44E;_tmp83F=((struct _tuple0*)(_tmp83B.QualId_tok).val)->f1;_tmp840=((struct _tuple0*)(_tmp83B.QualId_tok).val)->f2;_LL44D: {
# 3150
struct Cyc_List_List*_tmp84A=0;
{union Cyc_Absyn_Nmspace _tmp84B=_tmp83F;struct Cyc_List_List*_tmp84C;struct Cyc_List_List*_tmp84D;struct Cyc_List_List*_tmp84E;_LL451: if((_tmp84B.Rel_n).tag != 1)goto _LL453;_tmp84C=(struct Cyc_List_List*)(_tmp84B.Rel_n).val;_LL452:
 _tmp84A=_tmp84C;goto _LL450;_LL453: if((_tmp84B.Abs_n).tag != 2)goto _LL455;_tmp84D=(struct Cyc_List_List*)(_tmp84B.Abs_n).val;_LL454:
 _tmp84A=_tmp84D;goto _LL450;_LL455: if((_tmp84B.C_n).tag != 3)goto _LL457;_tmp84E=(struct Cyc_List_List*)(_tmp84B.C_n).val;_LL456:
 _tmp84A=_tmp84E;goto _LL450;_LL457: if((_tmp84B.Loc_n).tag != 4)goto _LL450;_LL458:
 goto _LL450;_LL450:;}
# 3157
for(0;_tmp84A != 0;_tmp84A=_tmp84A->tl){
const char*_tmpEFB;void*_tmpEFA[1];struct Cyc_String_pa_PrintArg_struct _tmpEF9;(_tmpEF9.tag=0,((_tmpEF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp84A->hd)),((_tmpEFA[0]=& _tmpEF9,Cyc_fprintf(Cyc_stderr,((_tmpEFB="%s::",_tag_dyneither(_tmpEFB,sizeof(char),5))),_tag_dyneither(_tmpEFA,sizeof(void*),1)))))));}
{const char*_tmpEFF;void*_tmpEFE[1];struct Cyc_String_pa_PrintArg_struct _tmpEFD;(_tmpEFD.tag=0,((_tmpEFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp840),((_tmpEFE[0]=& _tmpEFD,Cyc_fprintf(Cyc_stderr,((_tmpEFF="%s::",_tag_dyneither(_tmpEFF,sizeof(char),5))),_tag_dyneither(_tmpEFE,sizeof(void*),1)))))));}
goto _LL445;}_LL44E:;_LL44F:
{const char*_tmpF02;void*_tmpF01;(_tmpF01=0,Cyc_fprintf(Cyc_stderr,((_tmpF02="?",_tag_dyneither(_tmpF02,sizeof(char),2))),_tag_dyneither(_tmpF01,sizeof(void*),0)));}goto _LL445;_LL445:;}
# 3166
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp857=_new_region("yyr");struct _RegionHandle*yyr=& _tmp857;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp858=Cyc_Parse_parse_result;_npop_handler(0);return _tmp858;};
# 3169
;_pop_region(yyr);};}
