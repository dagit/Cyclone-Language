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
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 885
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 912
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 935
void*Cyc_Absyn_compress_kb(void*);
# 940
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 942
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 945
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 947
extern void*Cyc_Absyn_sint_typ;
# 949
void*Cyc_Absyn_float_typ(int);
# 997
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1046
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
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1095
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1104
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1106
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1113
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1122
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1127
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1133
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1156
int Cyc_Absyn_fntype_att(void*a);
# 1176
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
void Cyc_Lex_leave_using();struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0] __attribute__((aligned )) ;};
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
typedef struct Cyc_Parse_Declarator Cyc_Parse_declarator_t;struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
typedef struct _tuple10*Cyc_Parse_declarators_t;struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
typedef struct _tuple12*Cyc_Parse_declarator_list_t;
# 147
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 151
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct Cyc_Parse_Declarator _tmp2;struct Cyc_Absyn_Exp*_tmp3;struct _tuple11 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;
{struct _tuple10*_tmp86A;declarators=((_tmp86A=_region_malloc(r,sizeof(*_tmp86A)),((_tmp86A->tl=declarators,((_tmp86A->hd=_tmp2,_tmp86A))))));}{
struct Cyc_List_List*_tmp86B;exprs=((_tmp86B=_region_malloc(r,sizeof(*_tmp86B)),((_tmp86B->hd=_tmp3,((_tmp86B->tl=exprs,_tmp86B))))));};}
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
struct Cyc_Position_Exit_exn_struct _tmp86E;struct Cyc_Position_Exit_exn_struct*_tmp86D;(int)_throw((void*)((_tmp86D=_cycalloc_atomic(sizeof(*_tmp86D)),((_tmp86D[0]=((_tmp86E.tag=Cyc_Position_Exit,_tmp86E)),_tmp86D)))));};}
# 186
static void Cyc_Parse_unimp(struct _dyneither_ptr msg,unsigned int sg){
{const char*_tmp873;void*_tmp872[2];struct Cyc_String_pa_PrintArg_struct _tmp871;struct Cyc_String_pa_PrintArg_struct _tmp870;(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(sg)),((_tmp872[0]=& _tmp871,((_tmp872[1]=& _tmp870,Cyc_fprintf(Cyc_stderr,((_tmp873="%s: Warning: Cyclone does not yet support %s\n",_tag_dyneither(_tmp873,sizeof(char),46))),_tag_dyneither(_tmp872,sizeof(void*),2)))))))))))));}
return;}
# 192
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 194
struct Cyc_Absyn_Tqual _tmpD;void*_tmpE;struct _tuple8*_tmpC=tqt;_tmpD=_tmpC->f2;_tmpE=_tmpC->f3;
if((_tmpD.print_const  || _tmpD.q_volatile) || _tmpD.q_restrict){
if(_tmpD.loc != 0)loc=_tmpD.loc;{
const char*_tmp876;void*_tmp875;(_tmp875=0,Cyc_Tcutil_warn(loc,((_tmp876="qualifier on type is ignored",_tag_dyneither(_tmp876,sizeof(char),29))),_tag_dyneither(_tmp875,sizeof(void*),0)));};}
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
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp879;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp878;bound=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp878=_cycalloc(sizeof(*_tmp878)),((_tmp878[0]=((_tmp879.tag=1,((_tmp879.f1=_tmp1A,_tmp879)))),_tmp878)))));}goto _LL0;_LLF: {struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmp1B=(struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*)_tmp12;if(_tmp1B->tag != 1)goto _LL0;else{_tmp1C=(void*)_tmp1B->f1;}}_LL10:
# 226
 rgn=_tmp1C;goto _LL0;_LL0:;}{
# 229
struct _tuple14 _tmp87A;return(_tmp87A.f1=nullable,((_tmp87A.f2=bound,((_tmp87A.f3=zeroterm,((_tmp87A.f4=rgn,_tmp87A)))))));};}
# 235
struct _tuple0*Cyc_Parse_gensym_enum(){
# 237
static int enum_counter=0;
struct _dyneither_ptr*_tmp887;const char*_tmp886;void*_tmp885[1];struct Cyc_Int_pa_PrintArg_struct _tmp884;struct _tuple0*_tmp883;return(_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883->f1=Cyc_Absyn_Rel_n(0),((_tmp883->f2=(
(_tmp887=_cycalloc(sizeof(*_tmp887)),((_tmp887[0]=(struct _dyneither_ptr)((_tmp884.tag=1,((_tmp884.f1=(unsigned long)enum_counter ++,((_tmp885[0]=& _tmp884,Cyc_aprintf(((_tmp886="__anonymous_enum_%d__",_tag_dyneither(_tmp886,sizeof(char),22))),_tag_dyneither(_tmp885,sizeof(void*),1)))))))),_tmp887)))),_tmp883)))));}struct _tuple15{struct _tuple0*f1;struct Cyc_Absyn_Tqual f2;void*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 242
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 247
struct _tuple0*_tmp26;struct Cyc_Absyn_Tqual _tmp27;void*_tmp28;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp2A;struct Cyc_Absyn_Exp*_tmp2B;struct Cyc_Absyn_Exp*_tmp2C;struct _tuple17*_tmp25=field_info;_tmp26=(_tmp25->f1)->f1;_tmp27=(_tmp25->f1)->f2;_tmp28=(_tmp25->f1)->f3;_tmp29=(_tmp25->f1)->f4;_tmp2A=(_tmp25->f1)->f5;_tmp2B=(_tmp25->f2)->f1;_tmp2C=(_tmp25->f2)->f2;
if(_tmp29 != 0){
const char*_tmp888;Cyc_Parse_err(((_tmp888="bad type params in struct field",_tag_dyneither(_tmp888,sizeof(char),32))),loc);}
if(Cyc_Absyn_is_qvar_qualified(_tmp26)){
const char*_tmp889;Cyc_Parse_err(((_tmp889="struct or union field cannot be qualified with a namespace",_tag_dyneither(_tmp889,sizeof(char),59))),loc);}{
struct Cyc_Absyn_Aggrfield*_tmp88A;return(_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A->name=(*_tmp26).f2,((_tmp88A->tq=_tmp27,((_tmp88A->type=_tmp28,((_tmp88A->width=_tmp2B,((_tmp88A->attributes=_tmp2A,((_tmp88A->requires_clause=_tmp2C,_tmp88A)))))))))))));};}
# 257
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp88B;return(_tmp88B.Signed_spec=0,((_tmp88B.Unsigned_spec=0,((_tmp88B.Short_spec=0,((_tmp88B.Long_spec=0,((_tmp88B.Long_Long_spec=0,((_tmp88B.Valid_type_spec=0,((_tmp88B.Type_spec=Cyc_Absyn_sint_typ,((_tmp88B.loc=loc,_tmp88B)))))))))))))));}
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
struct Cyc_String_pa_PrintArg_struct _tmp893;void*_tmp892[1];const char*_tmp891;struct _dyneither_ptr*_tmp890;struct _dyneither_ptr*nm=(_tmp890=_cycalloc(sizeof(*_tmp890)),((_tmp890[0]=(struct _dyneither_ptr)((_tmp893.tag=0,((_tmp893.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp39),((_tmp892[0]=& _tmp893,Cyc_aprintf(((_tmp891="`%s",_tag_dyneither(_tmp891,sizeof(char),4))),_tag_dyneither(_tmp892,sizeof(void*),1)))))))),_tmp890)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8A2;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8A1;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8A0;struct Cyc_Absyn_Tvar*_tmp89F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp89E;*_tmp42=(void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp8A2.tag=2,((_tmp8A2.f1=((_tmp89F=_cycalloc(sizeof(*_tmp89F)),((_tmp89F->name=nm,((_tmp89F->identity=- 1,((_tmp89F->kind=(void*)((_tmp8A1=_cycalloc(sizeof(*_tmp8A1)),((_tmp8A1[0]=((_tmp8A0.tag=0,((_tmp8A0.f1=& Cyc_Tcutil_ik,_tmp8A0)))),_tmp8A1)))),_tmp89F)))))))),_tmp8A2)))),_tmp89E))));}
goto _LL18;}_LL1B:;_LL1C:
 goto _LL18;_LL18:;}
# 325
{struct _tuple18*_tmp8A5;struct Cyc_List_List*_tmp8A4;_tmp36=((_tmp8A4=_cycalloc(sizeof(*_tmp8A4)),((_tmp8A4->hd=((_tmp8A5=_cycalloc(sizeof(*_tmp8A5)),((_tmp8A5->f1=_tmp39,((_tmp8A5->f2=_tmp3B,_tmp8A5)))))),((_tmp8A4->tl=_tmp36,_tmp8A4))))));}goto _LL11;_LL14: if(_tmp38->f1 == 0)goto _LL16;_tmp3C=*_tmp38->f1;{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp38->f3;if(_tmp3D->tag != 15)goto _LL16;else{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)((void*)_tmp3D->f1);if(_tmp3E->tag != 1)goto _LL16;else{_tmp3F=(void**)((void**)& _tmp3E->f2);}}};_LL15: {
# 329
struct Cyc_String_pa_PrintArg_struct _tmp8AD;void*_tmp8AC[1];const char*_tmp8AB;struct _dyneither_ptr*_tmp8AA;struct _dyneither_ptr*nm=(_tmp8AA=_cycalloc(sizeof(*_tmp8AA)),((_tmp8AA[0]=(struct _dyneither_ptr)((_tmp8AD.tag=0,((_tmp8AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C),((_tmp8AC[0]=& _tmp8AD,Cyc_aprintf(((_tmp8AB="`%s",_tag_dyneither(_tmp8AB,sizeof(char),4))),_tag_dyneither(_tmp8AC,sizeof(void*),1)))))))),_tmp8AA)));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8BC;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8BB;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmp8BA;struct Cyc_Absyn_Tvar*_tmp8B9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8B8;*_tmp3F=(void*)((_tmp8B8=_cycalloc(sizeof(*_tmp8B8)),((_tmp8B8[0]=((_tmp8BC.tag=2,((_tmp8BC.f1=((_tmp8B9=_cycalloc(sizeof(*_tmp8B9)),((_tmp8B9->name=nm,((_tmp8B9->identity=- 1,((_tmp8B9->kind=(void*)((_tmp8BB=_cycalloc(sizeof(*_tmp8BB)),((_tmp8BB[0]=((_tmp8BA.tag=0,((_tmp8BA.f1=& Cyc_Tcutil_rk,_tmp8BA)))),_tmp8BB)))),_tmp8B9)))))))),_tmp8BC)))),_tmp8B8))));}
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
{struct _tuple18*_tmp8BF;struct Cyc_List_List*_tmp8BE;_tmp57=((_tmp8BE=_cycalloc(sizeof(*_tmp8BE)),((_tmp8BE->hd=((_tmp8BF=_cycalloc(sizeof(*_tmp8BF)),((_tmp8BF->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,((_tmp8BF->f2=_tmp5B,_tmp8BF)))))),((_tmp8BE->tl=_tmp57,_tmp8BE))))));}goto _LL1D;_LL20:;_LL21:
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
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp8C2;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp8C1;return Cyc_Absyn_new_exp((void*)((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1[0]=((_tmp8C2.tag=38,((_tmp8C2.f1=Cyc_Tcutil_copy_type(_tmp66),_tmp8C2)))),_tmp8C1)))),e->loc);}}}
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
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp8C8;struct Cyc_Absyn_ArrayInfo _tmp8C7;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8C6;return(void*)((_tmp8C6=_cycalloc(sizeof(*_tmp8C6)),((_tmp8C6[0]=((_tmp8C8.tag=8,((_tmp8C8.f1=((_tmp8C7.elt_type=_tmp78,((_tmp8C7.tq=_tmp6C,((_tmp8C7.num_elts=nelts2,((_tmp8C7.zero_term=_tmp6E,((_tmp8C7.zt_loc=_tmp6F,_tmp8C7)))))))))),_tmp8C8)))),_tmp8C6))));}
goto _LL27;};}_LL2A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69;if(_tmp70->tag != 5)goto _LL2C;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2B: {
# 380
void*_tmp7C=Cyc_Parse_substitute_tags(tags,_tmp71);
union Cyc_Absyn_Constraint*_tmp7D=_tmp75;
{union Cyc_Absyn_Constraint _tmp7E=*_tmp75;union Cyc_Absyn_Constraint _tmp7F=_tmp7E;struct Cyc_Absyn_Exp*_tmp81;_LL2F: if((_tmp7F.Eq_constr).tag != 1)goto _LL31;{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp80=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)((void*)(_tmp7F.Eq_constr).val);if(_tmp80->tag != 1)goto _LL31;else{_tmp81=_tmp80->f1;}};_LL30:
# 384
 _tmp81=Cyc_Parse_substitute_tags_exp(tags,_tmp81);
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8CE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp8CD;union Cyc_Absyn_Constraint*_tmp8CC;_tmp7D=((_tmp8CC=_cycalloc(sizeof(*_tmp8CC)),(((_tmp8CC->Eq_constr).val=(void*)((_tmp8CE=_cycalloc(sizeof(*_tmp8CE)),((_tmp8CE[0]=((_tmp8CD.tag=1,((_tmp8CD.f1=_tmp81,_tmp8CD)))),_tmp8CE)))),(((_tmp8CC->Eq_constr).tag=1,_tmp8CC))))));}
goto _LL2E;_LL31:;_LL32:
 goto _LL2E;_LL2E:;}
# 389
if(_tmp7C != _tmp71  || _tmp7D != _tmp75){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8D4;struct Cyc_Absyn_PtrInfo _tmp8D3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8D2;return(void*)((_tmp8D2=_cycalloc(sizeof(*_tmp8D2)),((_tmp8D2[0]=((_tmp8D4.tag=5,((_tmp8D4.f1=((_tmp8D3.elt_typ=_tmp7C,((_tmp8D3.elt_tq=_tmp72,((_tmp8D3.ptr_atts=(((_tmp8D3.ptr_atts).rgn=_tmp73,(((_tmp8D3.ptr_atts).nullable=_tmp74,(((_tmp8D3.ptr_atts).bounds=_tmp7D,(((_tmp8D3.ptr_atts).zero_term=_tmp76,(((_tmp8D3.ptr_atts).ptrloc=_tmp77,_tmp8D3.ptr_atts)))))))))),_tmp8D3)))))),_tmp8D4)))),_tmp8D2))));}
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
struct _tuple19*_tmp8D5;return(_tmp8D5=_cycalloc(sizeof(*_tmp8D5)),((_tmp8D5->f1=(*t).f2,((_tmp8D5->f2=(*t).f3,_tmp8D5)))));}
# 413
static struct Cyc_Absyn_Vardecl*Cyc_Parse_decl2vardecl(struct Cyc_Absyn_Decl*d){
void*_tmp89=d->r;void*_tmp8A=_tmp89;struct Cyc_Absyn_Vardecl*_tmp8C;_LL34: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp8B=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp8A;if(_tmp8B->tag != 0)goto _LL36;else{_tmp8C=_tmp8B->f1;}}_LL35:
 return _tmp8C;_LL36:;_LL37: {
const char*_tmp8D8;void*_tmp8D7;(_tmp8D7=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(d->loc,((_tmp8D8="bad declaration in `forarray' statement",_tag_dyneither(_tmp8D8,sizeof(char),40))),_tag_dyneither(_tmp8D7,sizeof(void*),0)));}_LL33:;}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp8F=tm;_LL39: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp90=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp8F;if(_tmp90->tag != 4)goto _LL3B;}_LL3A:
 return 1;_LL3B:;_LL3C:
 return 0;_LL38:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
const char*_tmp8D9;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8D9="`H",_tag_dyneither(_tmp8D9,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
const char*_tmp8DA;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8DA="`U",_tag_dyneither(_tmp8DA,sizeof(char),3))))== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
const char*_tmp8DB;if(Cyc_zstrcmp((struct _dyneither_ptr)s,((_tmp8DB="`RC",_tag_dyneither(_tmp8DB,sizeof(char),4))))== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 437
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8E5;struct _dyneither_ptr*_tmp8E4;struct Cyc_Absyn_Tvar*_tmp8E3;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8E2;return(void*)((_tmp8E2=_cycalloc(sizeof(*_tmp8E2)),((_tmp8E2[0]=((_tmp8E5.tag=2,((_tmp8E5.f1=((_tmp8E3=_cycalloc(sizeof(*_tmp8E3)),((_tmp8E3->name=((_tmp8E4=_cycalloc(sizeof(*_tmp8E4)),((_tmp8E4[0]=s,_tmp8E4)))),((_tmp8E3->identity=- 1,((_tmp8E3->kind=k,_tmp8E3)))))))),_tmp8E5)))),_tmp8E2))));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_copy_tvar(struct Cyc_Absyn_Tvar*t){
void*k;
{void*_tmp98=Cyc_Absyn_compress_kb(t->kind);void*_tmp99=_tmp98;struct Cyc_Absyn_Kind*_tmp9C;void*_tmp9D;_LL3E: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL40;}_LL3F:
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp8E8;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E7;k=(void*)((_tmp8E7=_cycalloc(sizeof(*_tmp8E7)),((_tmp8E7[0]=((_tmp8E8.tag=1,((_tmp8E8.f1=0,_tmp8E8)))),_tmp8E7))));}goto _LL3D;_LL40: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 2)goto _LL42;else{_tmp9C=_tmp9B->f2;}}_LL41:
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8EB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8EA;k=(void*)((_tmp8EA=_cycalloc(sizeof(*_tmp8EA)),((_tmp8EA[0]=((_tmp8EB.tag=2,((_tmp8EB.f1=0,((_tmp8EB.f2=_tmp9C,_tmp8EB)))))),_tmp8EA))));}goto _LL3D;_LL42: _tmp9D=_tmp99;_LL43:
 k=_tmp9D;goto _LL3D;_LL3D:;}{
# 447
struct Cyc_Absyn_Tvar*_tmp8EC;return(_tmp8EC=_cycalloc(sizeof(*_tmp8EC)),((_tmp8EC->name=t->name,((_tmp8EC->identity=- 1,((_tmp8EC->kind=k,_tmp8EC)))))));};}
# 454
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmpA3=t;struct Cyc_Absyn_Tvar*_tmpA5;_LL45: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA3;if(_tmpA4->tag != 2)goto _LL47;else{_tmpA5=_tmpA4->f1;}}_LL46:
 return _tmpA5;_LL47:;_LL48: {
const char*_tmp8EF;void*_tmp8EE;(_tmp8EE=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8EF="expecting a list of type variables, not types",_tag_dyneither(_tmp8EF,sizeof(char),46))),_tag_dyneither(_tmp8EE,sizeof(void*),0)));}_LL44:;}
# 460
static void*Cyc_Parse_tvar2typ(struct Cyc_Absyn_Tvar*pr){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp8F2;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8F1;return(void*)((_tmp8F1=_cycalloc(sizeof(*_tmp8F1)),((_tmp8F1[0]=((_tmp8F2.tag=2,((_tmp8F2.f1=pr,_tmp8F2)))),_tmp8F1))));}
# 465
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmpAA=Cyc_Tcutil_compress(t);void*_tmpAB=_tmpAA;void**_tmpAD;_LL4A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpAB;if(_tmpAC->tag != 2)goto _LL4C;else{_tmpAD=(void**)&(_tmpAC->f1)->kind;}}_LL4B: {
# 468
void*_tmpAE=Cyc_Absyn_compress_kb(*_tmpAD);void*_tmpAF=_tmpAE;_LL4F: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL51;}_LL50:
# 470
 if(!leq)*_tmpAD=Cyc_Tcutil_kind_to_bound(k);else{
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp8F5;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F4;*_tmpAD=(void*)((_tmp8F4=_cycalloc(sizeof(*_tmp8F4)),((_tmp8F4[0]=((_tmp8F5.tag=2,((_tmp8F5.f1=0,((_tmp8F5.f2=k,_tmp8F5)))))),_tmp8F4))));}
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
{const char*_tmp8F8;void*_tmp8F7;(_tmp8F7=0,Cyc_Tcutil_warn(loc,((_tmp8F8="function declaration with both new- and old-style parameter declarations; ignoring old-style",_tag_dyneither(_tmp8F8,sizeof(char),93))),_tag_dyneither(_tmp8F7,sizeof(void*),0)));}
# 503
return tms;_LL5B: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpB9=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpB7;if(_tmpB9->tag != 0)goto _LL58;else{_tmpBA=_tmpB9->f1;}}_LL5C:
# 505
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpBA)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds)){
const char*_tmp8FB;void*_tmp8FA;(_tmp8FA=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp8FB="wrong number of parameter declarations in old-style function declaration",_tag_dyneither(_tmp8FB,sizeof(char),73))),_tag_dyneither(_tmp8FA,sizeof(void*),0)));}{
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
const char*_tmp8FE;void*_tmp8FD;(_tmp8FD=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp8FE="initializer found in parameter declaration",_tag_dyneither(_tmp8FE,sizeof(char),43))),_tag_dyneither(_tmp8FD,sizeof(void*),0)));}
if(Cyc_Absyn_is_qvar_qualified(_tmpC4->name)){
const char*_tmp901;void*_tmp900;(_tmp900=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp901="namespaces forbidden in parameter declarations",_tag_dyneither(_tmp901,sizeof(char),47))),_tag_dyneither(_tmp900,sizeof(void*),0)));}
{struct _tuple8*_tmp904;struct Cyc_List_List*_tmp903;rev_new_params=(
(_tmp903=_cycalloc(sizeof(*_tmp903)),((_tmp903->hd=((_tmp904=_cycalloc(sizeof(*_tmp904)),((_tmp904->f1=(*_tmpC4->name).f2,((_tmp904->f2=_tmpC4->tq,((_tmp904->f3=_tmpC4->type,_tmp904)))))))),((_tmp903->tl=rev_new_params,_tmp903))))));}
# 525
goto L;_LL60:;_LL61: {
const char*_tmp907;void*_tmp906;(_tmp906=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmpC0->loc,((_tmp907="nonvariable declaration in parameter type",_tag_dyneither(_tmp907,sizeof(char),42))),_tag_dyneither(_tmp906,sizeof(void*),0)));}_LL5D:;}
# 529
L: if(_tmpBF == 0){
const char*_tmp90B;void*_tmp90A[1];struct Cyc_String_pa_PrintArg_struct _tmp909;(_tmp909.tag=0,((_tmp909.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmpBA->hd)),((_tmp90A[0]=& _tmp909,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp90B="%s is not given a type",_tag_dyneither(_tmp90B,sizeof(char),23))),_tag_dyneither(_tmp90A,sizeof(void*),1)))))));}}{
# 532
struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp91A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp919;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmp918;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmp917;struct Cyc_List_List*_tmp916;return
(_tmp916=_region_malloc(yy,sizeof(*_tmp916)),((_tmp916->hd=(void*)((_tmp91A=_region_malloc(yy,sizeof(*_tmp91A)),((_tmp91A[0]=((_tmp917.tag=3,((_tmp917.f1=(void*)((_tmp919=_region_malloc(yy,sizeof(*_tmp919)),((_tmp919[0]=((_tmp918.tag=1,((_tmp918.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params),((_tmp918.f2=0,((_tmp918.f3=0,((_tmp918.f4=0,((_tmp918.f5=0,_tmp918)))))))))))),_tmp919)))),_tmp917)))),_tmp91A)))),((_tmp916->tl=0,_tmp916)))));};};_LL58:;}
# 539
goto _LL57;_LL56:;_LL57: {
struct Cyc_List_List*_tmp91B;return(_tmp91B=_region_malloc(yy,sizeof(*_tmp91B)),((_tmp91B->hd=(void*)tms->hd,((_tmp91B->tl=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc),_tmp91B)))));}_LL53:;};}
# 547
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 551
if(tds != 0){
struct Cyc_Parse_Declarator _tmp91C;d=((_tmp91C.id=d.id,((_tmp91C.tms=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc),_tmp91C))));}{
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
{const char*_tmp91D;Cyc_Parse_err(((_tmp91D="bad storage class on function",_tag_dyneither(_tmp91D,sizeof(char),30))),loc);}break;}}}{
# 573
void*_tmpD9=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmpDA=Cyc_Parse_apply_tms(tq,_tmpD9,atts,d.tms);struct Cyc_Absyn_Tqual _tmpDC;void*_tmpDD;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpDF;struct _tuple13 _tmpDB=_tmpDA;_tmpDC=_tmpDB.f1;_tmpDD=_tmpDB.f2;_tmpDE=_tmpDB.f3;_tmpDF=_tmpDB.f4;
# 578
if(_tmpDE != 0){
# 581
const char*_tmp920;void*_tmp91F;(_tmp91F=0,Cyc_Tcutil_warn(loc,((_tmp920="bad type params, ignoring",_tag_dyneither(_tmp920,sizeof(char),26))),_tag_dyneither(_tmp91F,sizeof(void*),0)));}{
# 583
void*_tmpE2=_tmpDD;struct Cyc_List_List*_tmpE4;void*_tmpE5;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE7;struct Cyc_List_List*_tmpE8;int _tmpE9;struct Cyc_Absyn_VarargInfo*_tmpEA;struct Cyc_List_List*_tmpEB;struct Cyc_List_List*_tmpEC;_LL67: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2;if(_tmpE3->tag != 9)goto _LL69;else{_tmpE4=(_tmpE3->f1).tvars;_tmpE5=(_tmpE3->f1).effect;_tmpE6=(_tmpE3->f1).ret_tqual;_tmpE7=(_tmpE3->f1).ret_typ;_tmpE8=(_tmpE3->f1).args;_tmpE9=(_tmpE3->f1).c_varargs;_tmpEA=(_tmpE3->f1).cyc_varargs;_tmpEB=(_tmpE3->f1).rgn_po;_tmpEC=(_tmpE3->f1).attributes;}}_LL68: {
# 586
struct Cyc_List_List*_tmpED=0;
{struct Cyc_List_List*_tmpEE=_tmpE8;for(0;_tmpEE != 0;_tmpEE=_tmpEE->tl){
struct _tuple8*_tmpEF=(struct _tuple8*)_tmpEE->hd;struct _dyneither_ptr*_tmpF1;struct Cyc_Absyn_Tqual _tmpF2;void*_tmpF3;struct _tuple8*_tmpF0=_tmpEF;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;
if(_tmpF1 == 0){
{const char*_tmp921;Cyc_Parse_err(((_tmp921="missing argument variable in function prototype",_tag_dyneither(_tmp921,sizeof(char),48))),loc);}{
struct _tuple8*_tmp924;struct Cyc_List_List*_tmp923;_tmpED=((_tmp923=_cycalloc(sizeof(*_tmp923)),((_tmp923->hd=((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->f1=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"?",sizeof(char),2),((_tmp924->f2=_tmpF2,((_tmp924->f3=_tmpF3,_tmp924)))))))),((_tmp923->tl=_tmpED,_tmp923))))));};}else{
# 593
struct _tuple8*_tmp927;struct Cyc_List_List*_tmp926;_tmpED=((_tmp926=_cycalloc(sizeof(*_tmp926)),((_tmp926->hd=((_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927->f1=_tmpF1,((_tmp927->f2=_tmpF2,((_tmp927->f3=_tmpF3,_tmp927)))))))),((_tmp926->tl=_tmpED,_tmp926))))));}}}{
# 597
struct Cyc_Absyn_Fndecl*_tmp928;return(_tmp928=_cycalloc(sizeof(*_tmp928)),((_tmp928->sc=sc,((_tmp928->name=d.id,((_tmp928->tvs=_tmpE4,((_tmp928->is_inline=is_inline,((_tmp928->effect=_tmpE5,((_tmp928->ret_tqual=_tmpE6,((_tmp928->ret_type=_tmpE7,((_tmp928->args=
# 600
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpED),((_tmp928->c_varargs=_tmpE9,((_tmp928->cyc_varargs=_tmpEA,((_tmp928->rgn_po=_tmpEB,((_tmp928->body=body,((_tmp928->cached_typ=0,((_tmp928->param_vardecls=0,((_tmp928->fn_vardecl=0,((_tmp928->attributes=
# 607
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpEC,_tmpDF),_tmp928)))))))))))))))))))))))))))))))));};}_LL69:;_LL6A: {
const char*_tmp92B;void*_tmp92A;(_tmp92A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp92B="declarator is not a function prototype",_tag_dyneither(_tmp92B,sizeof(char),39))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}_LL66:;};};};}static char _tmpFD[52]="at most one type may appear within a type specifier";
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
void*_tmp92C;(_tmp92C=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp92C,sizeof(void*),0)));}
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec){
void*_tmp92D;(_tmp92D=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp92D,sizeof(void*),0)));}
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec){
void*_tmp92E;(_tmp92E=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp92E,sizeof(void*),0)));}
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec){
void*_tmp92F;(_tmp92F=0,Cyc_Tcutil_warn(loc,Cyc_Parse_msg4,_tag_dyneither(_tmp92F,sizeof(void*),0)));}
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
const char*_tmp932;void*_tmp931;(_tmp931=0,Cyc_Tcutil_warn(loc,((_tmp932="missing type within specifier",_tag_dyneither(_tmp932,sizeof(char),30))),_tag_dyneither(_tmp931,sizeof(void*),0)));}
t=Cyc_Absyn_int_typ(sgn,sz);}else{
# 683
if(seen_sign){
void*_tmp107=t;enum Cyc_Absyn_Sign _tmp109;enum Cyc_Absyn_Size_of _tmp10A;_LL6C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp108=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp107;if(_tmp108->tag != 6)goto _LL6E;else{_tmp109=_tmp108->f1;_tmp10A=_tmp108->f2;}}_LL6D:
# 686
 if(_tmp109 != sgn)
t=Cyc_Absyn_int_typ(sgn,_tmp10A);
goto _LL6B;_LL6E:;_LL6F:
{const char*_tmp933;Cyc_Parse_err(((_tmp933="sign specification on non-integral type",_tag_dyneither(_tmp933,sizeof(char),40))),loc);}goto _LL6B;_LL6B:;}
# 691
if(seen_size){
void*_tmp10C=t;enum Cyc_Absyn_Sign _tmp10E;enum Cyc_Absyn_Size_of _tmp10F;_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp10D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp10C;if(_tmp10D->tag != 6)goto _LL73;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;}}_LL72:
# 694
 if(_tmp10F != sz)
t=Cyc_Absyn_int_typ(_tmp10E,sz);
goto _LL70;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp110=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp10C;if(_tmp110->tag != 7)goto _LL75;}_LL74:
# 698
 t=Cyc_Absyn_float_typ(2);goto _LL70;_LL75:;_LL76:
{const char*_tmp934;Cyc_Parse_err(((_tmp934="size qualifier on non-integral type",_tag_dyneither(_tmp934,sizeof(char),36))),loc);}goto _LL70;_LL70:;}}
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
struct _tuple15*_tmp937;struct Cyc_List_List*_tmp936;return(_tmp936=_region_malloc(r,sizeof(*_tmp936)),((_tmp936->hd=((_tmp937=_region_malloc(r,sizeof(*_tmp937)),((_tmp937->f1=_tmp112,((_tmp937->f2=_tmp115,((_tmp937->f3=_tmp116,((_tmp937->f4=_tmp117,((_tmp937->f5=_tmp118,_tmp937)))))))))))),((_tmp936->tl=0,_tmp936)))));}else{
# 718
struct _tuple15*_tmp93A;struct Cyc_List_List*_tmp939;return(_tmp939=_region_malloc(r,sizeof(*_tmp939)),((_tmp939->hd=((_tmp93A=_region_malloc(r,sizeof(*_tmp93A)),((_tmp93A->f1=_tmp112,((_tmp93A->f2=_tmp115,((_tmp93A->f3=_tmp116,((_tmp93A->f4=_tmp117,((_tmp93A->f5=_tmp118,_tmp93A)))))))))))),((_tmp939->tl=
Cyc_Parse_apply_tmss(r,tq,Cyc_Tcutil_copy_type(t),ds->tl,shared_atts),_tmp939)))));}};}
# 722
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 725
if(tms == 0){struct _tuple13 _tmp93B;return(_tmp93B.f1=tq,((_tmp93B.f2=t,((_tmp93B.f3=0,((_tmp93B.f4=atts,_tmp93B)))))));}{
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
struct Cyc_List_List*_tmp93C;fn_atts=((_tmp93C=_cycalloc(sizeof(*_tmp93C)),((_tmp93C->hd=(void*)as->hd,((_tmp93C->tl=fn_atts,_tmp93C))))));}else{
# 743
struct Cyc_List_List*_tmp93D;new_atts=((_tmp93D=_cycalloc(sizeof(*_tmp93D)),((_tmp93D->hd=(void*)as->hd,((_tmp93D->tl=new_atts,_tmp93D))))));}}}
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
const char*_tmp940;void*_tmp93F;(_tmp93F=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp13A,((_tmp940="function declaration without parameter types",_tag_dyneither(_tmp940,sizeof(char),45))),_tag_dyneither(_tmp93F,sizeof(void*),0)));}_LL84:;}_LL7E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp129=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp129->tag != 4)goto _LL80;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;}}_LL7F:
# 797
 if(tms->tl == 0){
struct _tuple13 _tmp941;return(_tmp941.f1=tq,((_tmp941.f2=t,((_tmp941.f3=_tmp12A,((_tmp941.f4=atts,_tmp941)))))));}{
# 802
const char*_tmp944;void*_tmp943;(_tmp943=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp12B,((_tmp944="type parameters must appear before function arguments in declarator",_tag_dyneither(_tmp944,sizeof(char),68))),_tag_dyneither(_tmp943,sizeof(void*),0)));};_LL80: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp12C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12C->tag != 2)goto _LL82;else{_tmp12D=_tmp12C->f1;_tmp12E=_tmp12C->f2;}}_LL81: {
# 805
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp94A;struct Cyc_Absyn_PtrInfo _tmp949;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp948;return Cyc_Parse_apply_tms(_tmp12E,(void*)((_tmp948=_cycalloc(sizeof(*_tmp948)),((_tmp948[0]=((_tmp94A.tag=5,((_tmp94A.f1=((_tmp949.elt_typ=t,((_tmp949.elt_tq=tq,((_tmp949.ptr_atts=_tmp12D,_tmp949)))))),_tmp94A)))),_tmp948)))),atts,tms->tl);}_LL82: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp12F=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp11F;if(_tmp12F->tag != 5)goto _LL77;else{_tmp130=_tmp12F->f1;_tmp131=_tmp12F->f2;}}_LL83:
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
struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct _tmp950;struct Cyc_Absyn_Typedefdecl*_tmp94F;struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp94E;return Cyc_Absyn_new_decl((void*)((_tmp94E=_cycalloc(sizeof(*_tmp94E)),((_tmp94E[0]=((_tmp950.tag=8,((_tmp950.f1=((_tmp94F=_cycalloc(sizeof(*_tmp94F)),((_tmp94F->name=_tmp151,((_tmp94F->tvs=_tmp154,((_tmp94F->kind=kind,((_tmp94F->defn=type,((_tmp94F->atts=_tmp155,((_tmp94F->tq=_tmp152,_tmp94F)))))))))))))),_tmp950)))),_tmp94E)))),loc);};};}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp953;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp952;return Cyc_Absyn_new_stmt((void*)((_tmp952=_cycalloc(sizeof(*_tmp952)),((_tmp952[0]=((_tmp953.tag=12,((_tmp953.f1=d,((_tmp953.f2=s,_tmp953)))))),_tmp952)))),d->loc);}
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
const char*_tmp954;Cyc_Parse_err(((_tmp954="inline is allowed only on function definitions",_tag_dyneither(_tmp954,sizeof(char),47))),loc);}{
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
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp95A;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp959;struct Cyc_List_List*_tmp958;struct Cyc_List_List*_tmp181=(_tmp958=_cycalloc(sizeof(*_tmp958)),((_tmp958->hd=Cyc_Absyn_new_decl((void*)((_tmp95A=_cycalloc(sizeof(*_tmp95A)),((_tmp95A[0]=((_tmp959.tag=5,((_tmp959.f1=_tmp169,_tmp959)))),_tmp95A)))),loc),((_tmp958->tl=0,_tmp958)))));_npop_handler(0);return _tmp181;};_LL9E: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16A->tag != 26)goto _LLA0;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp16B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp16A->f1)->r;if(_tmp16B->tag != 1)goto _LLA0;else{_tmp16C=_tmp16B->f1;}}}_LL9F:
# 910
 if(_tmp162 != 0){const char*_tmp95B;Cyc_Parse_err(((_tmp95B="attributes on enum not supported",_tag_dyneither(_tmp95B,sizeof(char),33))),loc);}
_tmp16C->sc=s;{
struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp961;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp960;struct Cyc_List_List*_tmp95F;struct Cyc_List_List*_tmp186=(_tmp95F=_cycalloc(sizeof(*_tmp95F)),((_tmp95F->hd=Cyc_Absyn_new_decl((void*)((_tmp961=_cycalloc(sizeof(*_tmp961)),((_tmp961[0]=((_tmp960.tag=7,((_tmp960.f1=_tmp16C,_tmp960)))),_tmp961)))),loc),((_tmp95F->tl=0,_tmp95F)))));_npop_handler(0);return _tmp186;};_LLA0: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp166;if(_tmp16D->tag != 26)goto _LLA2;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp16E=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp16D->f1)->r;if(_tmp16E->tag != 2)goto _LLA2;else{_tmp16F=_tmp16E->f1;}}}_LLA1:
# 914
 if(_tmp162 != 0){const char*_tmp962;Cyc_Parse_err(((_tmp962="attributes on datatypes not supported",_tag_dyneither(_tmp962,sizeof(char),38))),loc);}
_tmp16F->sc=s;{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp968;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp967;struct Cyc_List_List*_tmp966;struct Cyc_List_List*_tmp18B=(_tmp966=_cycalloc(sizeof(*_tmp966)),((_tmp966->hd=Cyc_Absyn_new_decl((void*)((_tmp968=_cycalloc(sizeof(*_tmp968)),((_tmp968[0]=((_tmp967.tag=6,((_tmp967.f1=_tmp16F,_tmp967)))),_tmp968)))),loc),((_tmp966->tl=0,_tmp966)))));_npop_handler(0);return _tmp18B;};_LLA2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp166;if(_tmp170->tag != 11)goto _LLA4;else{if((((_tmp170->f1).aggr_info).UnknownAggr).tag != 1)goto _LLA4;_tmp171=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f1;_tmp172=((struct _tuple2)(((_tmp170->f1).aggr_info).UnknownAggr).val).f2;_tmp173=(_tmp170->f1).targs;}}_LLA3: {
# 918
struct Cyc_List_List*_tmp18C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp173);
struct Cyc_Absyn_Aggrdecl*_tmp969;struct Cyc_Absyn_Aggrdecl*_tmp18D=(_tmp969=_cycalloc(sizeof(*_tmp969)),((_tmp969->kind=_tmp171,((_tmp969->sc=s,((_tmp969->name=_tmp172,((_tmp969->tvs=_tmp18C,((_tmp969->impl=0,((_tmp969->attributes=0,_tmp969)))))))))))));
if(_tmp162 != 0){const char*_tmp96A;Cyc_Parse_err(((_tmp96A="bad attributes on type declaration",_tag_dyneither(_tmp96A,sizeof(char),35))),loc);}{
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp970;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp96F;struct Cyc_List_List*_tmp96E;struct Cyc_List_List*_tmp192=(_tmp96E=_cycalloc(sizeof(*_tmp96E)),((_tmp96E->hd=Cyc_Absyn_new_decl((void*)((_tmp970=_cycalloc(sizeof(*_tmp970)),((_tmp970[0]=((_tmp96F.tag=5,((_tmp96F.f1=_tmp18D,_tmp96F)))),_tmp970)))),loc),((_tmp96E->tl=0,_tmp96E)))));_npop_handler(0);return _tmp192;};}_LLA4: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp174->tag != 3)goto _LLA6;else{if((((_tmp174->f1).datatype_info).KnownDatatype).tag != 2)goto _LLA6;_tmp175=(struct Cyc_Absyn_Datatypedecl**)(((_tmp174->f1).datatype_info).KnownDatatype).val;}}_LLA5:
# 923
 if(_tmp162 != 0){const char*_tmp971;Cyc_Parse_err(((_tmp971="bad attributes on datatype",_tag_dyneither(_tmp971,sizeof(char),27))),loc);}{
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp977;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp976;struct Cyc_List_List*_tmp975;struct Cyc_List_List*_tmp198=(_tmp975=_cycalloc(sizeof(*_tmp975)),((_tmp975->hd=Cyc_Absyn_new_decl((void*)((_tmp977=_cycalloc(sizeof(*_tmp977)),((_tmp977[0]=((_tmp976.tag=6,((_tmp976.f1=*_tmp175,_tmp976)))),_tmp977)))),loc),((_tmp975->tl=0,_tmp975)))));_npop_handler(0);return _tmp198;};_LLA6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp166;if(_tmp176->tag != 3)goto _LLA8;else{if((((_tmp176->f1).datatype_info).UnknownDatatype).tag != 1)goto _LLA8;_tmp177=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).name;_tmp178=((struct Cyc_Absyn_UnknownDatatypeInfo)(((_tmp176->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp179=(_tmp176->f1).targs;}}_LLA7: {
# 926
struct Cyc_List_List*_tmp199=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp179);
struct Cyc_Absyn_Decl*_tmp19A=Cyc_Absyn_datatype_decl(s,_tmp177,_tmp199,0,_tmp178,loc);
if(_tmp162 != 0){const char*_tmp978;Cyc_Parse_err(((_tmp978="bad attributes on datatype",_tag_dyneither(_tmp978,sizeof(char),27))),loc);}{
struct Cyc_List_List*_tmp979;struct Cyc_List_List*_tmp19D=(_tmp979=_cycalloc(sizeof(*_tmp979)),((_tmp979->hd=_tmp19A,((_tmp979->tl=0,_tmp979)))));_npop_handler(0);return _tmp19D;};}_LLA8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp166;if(_tmp17A->tag != 13)goto _LLAA;else{_tmp17B=_tmp17A->f1;}}_LLA9: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp97A;struct Cyc_Absyn_Enumdecl*_tmp19E=(_tmp97A=_cycalloc(sizeof(*_tmp97A)),((_tmp97A->sc=s,((_tmp97A->name=_tmp17B,((_tmp97A->fields=0,_tmp97A)))))));
if(_tmp162 != 0){const char*_tmp97B;Cyc_Parse_err(((_tmp97B="bad attributes on enum",_tag_dyneither(_tmp97B,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp985;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp984;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp983;struct Cyc_List_List*_tmp982;struct Cyc_List_List*_tmp1A4=(_tmp982=_cycalloc(sizeof(*_tmp982)),((_tmp982->hd=((_tmp985=_cycalloc(sizeof(*_tmp985)),((_tmp985->r=(void*)((_tmp983=_cycalloc(sizeof(*_tmp983)),((_tmp983[0]=((_tmp984.tag=7,((_tmp984.f1=_tmp19E,_tmp984)))),_tmp983)))),((_tmp985->loc=loc,_tmp985)))))),((_tmp982->tl=0,_tmp982)))));_npop_handler(0);return _tmp1A4;};}_LLAA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp166;if(_tmp17C->tag != 14)goto _LLAC;else{_tmp17D=_tmp17C->f1;}}_LLAB: {
# 937
struct Cyc_Core_Opt*_tmp988;struct Cyc_Absyn_Enumdecl*_tmp987;struct Cyc_Absyn_Enumdecl*_tmp1A6=(_tmp987=_cycalloc(sizeof(*_tmp987)),((_tmp987->sc=s,((_tmp987->name=Cyc_Parse_gensym_enum(),((_tmp987->fields=((_tmp988=_cycalloc(sizeof(*_tmp988)),((_tmp988->v=_tmp17D,_tmp988)))),_tmp987)))))));
if(_tmp162 != 0){const char*_tmp989;Cyc_Parse_err(((_tmp989="bad attributes on enum",_tag_dyneither(_tmp989,sizeof(char),23))),loc);}{
struct Cyc_Absyn_Decl*_tmp993;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct _tmp992;struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp991;struct Cyc_List_List*_tmp990;struct Cyc_List_List*_tmp1AC=(_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990->hd=((_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993->r=(void*)((_tmp991=_cycalloc(sizeof(*_tmp991)),((_tmp991[0]=((_tmp992.tag=7,((_tmp992.f1=_tmp1A6,_tmp992)))),_tmp991)))),((_tmp993->loc=loc,_tmp993)))))),((_tmp990->tl=0,_tmp990)))));_npop_handler(0);return _tmp1AC;};}_LLAC:;_LLAD:
{const char*_tmp994;Cyc_Parse_err(((_tmp994="missing declarator",_tag_dyneither(_tmp994,sizeof(char),19))),loc);}{struct Cyc_List_List*_tmp1B0=0;_npop_handler(0);return _tmp1B0;};_LL9B:;}else{
# 944
struct Cyc_List_List*_tmp1B1=Cyc_Parse_apply_tmss(mkrgn,_tmp160,_tmp165,declarators,_tmp162);
if(istypedef){
# 949
if(!exps_empty){
const char*_tmp995;Cyc_Parse_err(((_tmp995="initializer in typedef declaration",_tag_dyneither(_tmp995,sizeof(char),35))),loc);}{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp1B1);
struct Cyc_List_List*_tmp1B3=decls;_npop_handler(0);return _tmp1B3;};}else{
# 955
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp1B4=_tmp1B1;for(0;_tmp1B4 != 0;(_tmp1B4=_tmp1B4->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp1B5=(struct _tuple15*)_tmp1B4->hd;struct _tuple0*_tmp1B7;struct Cyc_Absyn_Tqual _tmp1B8;void*_tmp1B9;struct Cyc_List_List*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct _tuple15*_tmp1B6=_tmp1B5;_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;_tmp1B9=_tmp1B6->f3;_tmp1BA=_tmp1B6->f4;_tmp1BB=_tmp1B6->f5;
if(_tmp1BA != 0){
const char*_tmp998;void*_tmp997;(_tmp997=0,Cyc_Tcutil_warn(loc,((_tmp998="bad type params, ignoring",_tag_dyneither(_tmp998,sizeof(char),26))),_tag_dyneither(_tmp997,sizeof(void*),0)));}
if(exprs == 0){
const char*_tmp99B;void*_tmp99A;(_tmp99A=0,((int(*)(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(loc,((_tmp99B="unexpected NULL in parse!",_tag_dyneither(_tmp99B,sizeof(char),26))),_tag_dyneither(_tmp99A,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp1C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp1C1=Cyc_Absyn_new_vardecl(_tmp1B7,_tmp1B9,_tmp1C0);
_tmp1C1->tq=_tmp1B8;
_tmp1C1->sc=s;
_tmp1C1->attributes=_tmp1BB;{
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9A1;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp9A0;struct Cyc_Absyn_Decl*_tmp99F;struct Cyc_Absyn_Decl*_tmp1C2=(_tmp99F=_cycalloc(sizeof(*_tmp99F)),((_tmp99F->r=(void*)((_tmp9A1=_cycalloc(sizeof(*_tmp9A1)),((_tmp9A1[0]=((_tmp9A0.tag=0,((_tmp9A0.f1=_tmp1C1,_tmp9A0)))),_tmp9A1)))),((_tmp99F->loc=loc,_tmp99F)))));
struct Cyc_List_List*_tmp9A2;decls=((_tmp9A2=_cycalloc(sizeof(*_tmp9A2)),((_tmp9A2->hd=_tmp1C2,((_tmp9A2->tl=decls,_tmp9A2))))));};};}}{
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
{const char*_tmp9A7;void*_tmp9A6[2];struct Cyc_String_pa_PrintArg_struct _tmp9A5;struct Cyc_Int_pa_PrintArg_struct _tmp9A4;Cyc_Parse_err((struct _dyneither_ptr)((_tmp9A4.tag=1,((_tmp9A4.f1=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s)),((_tmp9A5.tag=0,((_tmp9A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp9A6[0]=& _tmp9A5,((_tmp9A6[1]=& _tmp9A4,Cyc_aprintf(((_tmp9A7="bad kind: %s; strlen=%d",_tag_dyneither(_tmp9A7,sizeof(char),24))),_tag_dyneither(_tmp9A6,sizeof(void*),2)))))))))))))),loc);}
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
{const char*_tmp9A8;Cyc_Parse_err(((_tmp9A8="cannot mix patterns and expressions in case",_tag_dyneither(_tmp9A8,sizeof(char),44))),p->loc);}
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
union Cyc_YYSTYPE Cyc_Int_tok(struct _tuple5 yy1){union Cyc_YYSTYPE _tmp9A9;return((_tmp9A9.Int_tok).val=yy1,(((_tmp9A9.Int_tok).tag=1,_tmp9A9)));}static char _tmp1ED[5]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){union Cyc_YYSTYPE _tmp9AA;return((_tmp9AA.Char_tok).val=yy1,(((_tmp9AA.Char_tok).tag=2,_tmp9AA)));}static char _tmp1F2[13]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){union Cyc_YYSTYPE _tmp9AB;return((_tmp9AB.String_tok).val=yy1,(((_tmp9AB.String_tok).tag=3,_tmp9AB)));}static char _tmp1F7[56]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){union Cyc_YYSTYPE _tmp9AC;return((_tmp9AC.YY1).val=yy1,(((_tmp9AC.YY1).tag=7,_tmp9AC)));}static char _tmp1FC[19]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9AD;return((_tmp9AD.YY2).val=yy1,(((_tmp9AD.YY2).tag=8,_tmp9AD)));}static char _tmp201[28]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9AE;return((_tmp9AE.YY3).val=yy1,(((_tmp9AE.YY3).tag=9,_tmp9AE)));}static char _tmp206[6]="exp_t";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9AF;return((_tmp9AF.YY4).val=yy1,(((_tmp9AF.YY4).tag=10,_tmp9AF)));}static char _tmp20B[10]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_Absyn_Exp*yy1){union Cyc_YYSTYPE _tmp9B0;return((_tmp9B0.YY5).val=yy1,(((_tmp9B0.YY5).tag=11,_tmp9B0)));}static char _tmp210[17]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY6(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B1;return((_tmp9B1.YY6).val=yy1,(((_tmp9B1.YY6).tag=12,_tmp9B1)));}static char _tmp215[47]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B2;return((_tmp9B2.YY7).val=yy1,(((_tmp9B2.YY7).tag=13,_tmp9B2)));}static char _tmp21A[9]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY8(enum Cyc_Absyn_Primop yy1){union Cyc_YYSTYPE _tmp9B3;return((_tmp9B3.YY8).val=yy1,(((_tmp9B3.YY8).tag=14,_tmp9B3)));}static char _tmp21F[19]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Core_Opt*yy1){union Cyc_YYSTYPE _tmp9B4;return((_tmp9B4.YY9).val=yy1,(((_tmp9B4.YY9).tag=15,_tmp9B4)));}static char _tmp224[7]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){union Cyc_YYSTYPE _tmp9B5;return((_tmp9B5.QualId_tok).val=yy1,(((_tmp9B5.QualId_tok).tag=5,_tmp9B5)));}static char _tmp229[7]="stmt_t";
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
union Cyc_YYSTYPE Cyc_YY10(struct Cyc_Absyn_Stmt*yy1){union Cyc_YYSTYPE _tmp9B6;return((_tmp9B6.YY10).val=yy1,(((_tmp9B6.YY10).tag=16,_tmp9B6)));}static char _tmp22E[27]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9B7;return((_tmp9B7.YY11).val=yy1,(((_tmp9B7.YY11).tag=17,_tmp9B7)));}static char _tmp233[6]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY12(struct Cyc_Absyn_Pat*yy1){union Cyc_YYSTYPE _tmp9B8;return((_tmp9B8.YY12).val=yy1,(((_tmp9B8.YY12).tag=18,_tmp9B8)));}static char _tmp238[28]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY13(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9B9;return((_tmp9B9.YY13).val=yy1,(((_tmp9B9.YY13).tag=19,_tmp9B9)));}static char _tmp23D[17]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY14(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BA;return((_tmp9BA.YY14).val=yy1,(((_tmp9BA.YY14).tag=20,_tmp9BA)));}static char _tmp242[36]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY15(struct _tuple23*yy1){union Cyc_YYSTYPE _tmp9BB;return((_tmp9BB.YY15).val=yy1,(((_tmp9BB.YY15).tag=21,_tmp9BB)));}static char _tmp247[47]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BC;return((_tmp9BC.YY16).val=yy1,(((_tmp9BC.YY16).tag=22,_tmp9BC)));}static char _tmp24C[58]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY17(struct _tuple22*yy1){union Cyc_YYSTYPE _tmp9BD;return((_tmp9BD.YY17).val=yy1,(((_tmp9BD.YY17).tag=23,_tmp9BD)));}static char _tmp251[9]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY18(struct Cyc_Absyn_Fndecl*yy1){union Cyc_YYSTYPE _tmp9BE;return((_tmp9BE.YY18).val=yy1,(((_tmp9BE.YY18).tag=24,_tmp9BE)));}static char _tmp256[18]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY19(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9BF;return((_tmp9BF.YY19).val=yy1,(((_tmp9BF.YY19).tag=25,_tmp9BF)));}static char _tmp25B[12]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY20(struct Cyc_Parse_Declaration_spec yy1){union Cyc_YYSTYPE _tmp9C0;return((_tmp9C0.YY20).val=yy1,(((_tmp9C0.YY20).tag=26,_tmp9C0)));}static char _tmp260[31]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY21(struct _tuple11 yy1){union Cyc_YYSTYPE _tmp9C1;return((_tmp9C1.YY21).val=yy1,(((_tmp9C1.YY21).tag=27,_tmp9C1)));}static char _tmp265[23]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY22(struct _tuple12*yy1){union Cyc_YYSTYPE _tmp9C2;return((_tmp9C2.YY22).val=yy1,(((_tmp9C2.YY22).tag=28,_tmp9C2)));}static char _tmp26A[19]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY23(enum Cyc_Parse_Storage_class*yy1){union Cyc_YYSTYPE _tmp9C3;return((_tmp9C3.YY23).val=yy1,(((_tmp9C3.YY23).tag=29,_tmp9C3)));}static char _tmp26F[17]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Parse_Type_specifier yy1){union Cyc_YYSTYPE _tmp9C4;return((_tmp9C4.YY24).val=yy1,(((_tmp9C4.YY24).tag=30,_tmp9C4)));}static char _tmp274[12]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY25(enum Cyc_Absyn_AggrKind yy1){union Cyc_YYSTYPE _tmp9C5;return((_tmp9C5.YY25).val=yy1,(((_tmp9C5.YY25).tag=31,_tmp9C5)));}static char _tmp279[8]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_Absyn_Tqual yy1){union Cyc_YYSTYPE _tmp9C6;return((_tmp9C6.YY26).val=yy1,(((_tmp9C6.YY26).tag=32,_tmp9C6)));}static char _tmp27E[23]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C7;return((_tmp9C7.YY27).val=yy1,(((_tmp9C7.YY27).tag=33,_tmp9C7)));}static char _tmp283[34]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY28(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C8;return((_tmp9C8.YY28).val=yy1,(((_tmp9C8.YY28).tag=34,_tmp9C8)));}static char _tmp288[33]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9C9;return((_tmp9C9.YY29).val=yy1,(((_tmp9C9.YY29).tag=35,_tmp9C9)));}static char _tmp28D[18]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Declarator yy1){union Cyc_YYSTYPE _tmp9CA;return((_tmp9CA.YY30).val=yy1,(((_tmp9CA.YY30).tag=36,_tmp9CA)));}static char _tmp292[45]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
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
union Cyc_YYSTYPE Cyc_YY31(struct _tuple24*yy1){union Cyc_YYSTYPE _tmp9CB;return((_tmp9CB.YY31).val=yy1,(((_tmp9CB.YY31).tag=37,_tmp9CB)));}static char _tmp297[57]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
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
union Cyc_YYSTYPE Cyc_YY32(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9CC;return((_tmp9CC.YY32).val=yy1,(((_tmp9CC.YY32).tag=38,_tmp9CC)));}static char _tmp29C[26]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Parse_Abstractdeclarator yy1){union Cyc_YYSTYPE _tmp9CD;return((_tmp9CD.YY33).val=yy1,(((_tmp9CD.YY33).tag=39,_tmp9CD)));}static char _tmp2A1[5]="bool";
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
union Cyc_YYSTYPE Cyc_YY34(int yy1){union Cyc_YYSTYPE _tmp9CE;return((_tmp9CE.YY34).val=yy1,(((_tmp9CE.YY34).tag=40,_tmp9CE)));}static char _tmp2A6[8]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY35(enum Cyc_Absyn_Scope yy1){union Cyc_YYSTYPE _tmp9CF;return((_tmp9CF.YY35).val=yy1,(((_tmp9CF.YY35).tag=41,_tmp9CF)));}static char _tmp2AB[16]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_Absyn_Datatypefield*yy1){union Cyc_YYSTYPE _tmp9D0;return((_tmp9D0.YY36).val=yy1,(((_tmp9D0.YY36).tag=42,_tmp9D0)));}static char _tmp2B0[27]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D1;return((_tmp9D1.YY37).val=yy1,(((_tmp9D1.YY37).tag=43,_tmp9D1)));}static char _tmp2B5[41]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY38(struct _tuple25 yy1){union Cyc_YYSTYPE _tmp9D2;return((_tmp9D2.YY38).val=yy1,(((_tmp9D2.YY38).tag=44,_tmp9D2)));}static char _tmp2BA[17]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D3;return((_tmp9D3.YY39).val=yy1,(((_tmp9D3.YY39).tag=45,_tmp9D3)));}static char _tmp2BF[31]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY40(struct _tuple8*yy1){union Cyc_YYSTYPE _tmp9D4;return((_tmp9D4.YY40).val=yy1,(((_tmp9D4.YY40).tag=46,_tmp9D4)));}static char _tmp2C4[42]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D5;return((_tmp9D5.YY41).val=yy1,(((_tmp9D5.YY41).tag=47,_tmp9D5)));}static char _tmp2C9[115]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY42(struct _tuple26*yy1){union Cyc_YYSTYPE _tmp9D6;return((_tmp9D6.YY42).val=yy1,(((_tmp9D6.YY42).tag=48,_tmp9D6)));}static char _tmp2CE[18]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D7;return((_tmp9D7.YY43).val=yy1,(((_tmp9D7.YY43).tag=49,_tmp9D7)));}static char _tmp2D3[24]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY44(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9D8;return((_tmp9D8.YY44).val=yy1,(((_tmp9D8.YY44).tag=50,_tmp9D8)));}static char _tmp2D8[13]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY45(void*yy1){union Cyc_YYSTYPE _tmp9D9;return((_tmp9D9.YY45).val=yy1,(((_tmp9D9.YY45).tag=51,_tmp9D9)));}static char _tmp2DD[7]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY46(struct Cyc_Absyn_Kind*yy1){union Cyc_YYSTYPE _tmp9DA;return((_tmp9DA.YY46).val=yy1,(((_tmp9DA.YY46).tag=52,_tmp9DA)));}static char _tmp2E2[7]="type_t";
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
union Cyc_YYSTYPE Cyc_YY47(void*yy1){union Cyc_YYSTYPE _tmp9DB;return((_tmp9DB.YY47).val=yy1,(((_tmp9DB.YY47).tag=53,_tmp9DB)));}static char _tmp2E7[23]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DC;return((_tmp9DC.YY48).val=yy1,(((_tmp9DC.YY48).tag=54,_tmp9DC)));}static char _tmp2EC[12]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){union Cyc_YYSTYPE _tmp9DD;return((_tmp9DD.YY49).val=yy1,(((_tmp9DD.YY49).tag=55,_tmp9DD)));}static char _tmp2F1[12]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_Absyn_Enumfield*yy1){union Cyc_YYSTYPE _tmp9DE;return((_tmp9DE.YY50).val=yy1,(((_tmp9DE.YY50).tag=56,_tmp9DE)));}static char _tmp2F6[23]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9DF;return((_tmp9DF.YY51).val=yy1,(((_tmp9DF.YY51).tag=57,_tmp9DF)));}static char _tmp2FB[11]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY52(void*yy1){union Cyc_YYSTYPE _tmp9E0;return((_tmp9E0.YY52).val=yy1,(((_tmp9E0.YY52).tag=58,_tmp9E0)));}static char _tmp300[31]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E1;return((_tmp9E1.YY53).val=yy1,(((_tmp9E1.YY53).tag=59,_tmp9E1)));}static char _tmp305[15]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY54(union Cyc_Absyn_Constraint*yy1){union Cyc_YYSTYPE _tmp9E2;return((_tmp9E2.YY54).val=yy1,(((_tmp9E2.YY54).tag=60,_tmp9E2)));}static char _tmp30A[45]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E3;return((_tmp9E3.YY55).val=yy1,(((_tmp9E3.YY55).tag=61,_tmp9E3)));}static char _tmp30F[20]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY56(void*yy1){union Cyc_YYSTYPE _tmp9E4;return((_tmp9E4.YY56).val=yy1,(((_tmp9E4.YY56).tag=62,_tmp9E4)));}static char _tmp314[21]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){union Cyc_YYSTYPE _tmp9E5;return((_tmp9E5.YY57).val=yy1,(((_tmp9E5.YY57).tag=63,_tmp9E5)));}static char _tmp319[21]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){union Cyc_YYSTYPE _tmp9E6;return((_tmp9E6.Asm_tok).val=yy1,(((_tmp9E6.Asm_tok).tag=6,_tmp9E6)));}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1186
typedef struct Cyc_Yyltype Cyc_yyltype;
# 1190
struct Cyc_Yyltype Cyc_yynewloc(){
struct Cyc_Yyltype _tmp9E7;return(_tmp9E7.timestamp=0,((_tmp9E7.first_line=0,((_tmp9E7.first_column=0,((_tmp9E7.last_line=0,((_tmp9E7.last_column=0,_tmp9E7)))))))));}
# 1193
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1204 "parse.y"
static short Cyc_yytranslate[374]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,145,2,2,129,143,140,2,126,127,134,137,122,141,131,142,2,2,2,2,2,2,2,2,2,2,130,119,124,123,125,136,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,132,2,133,139,128,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,120,138,121,144,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118};static char _tmp31E[2]="$";static char _tmp31F[6]="error";static char _tmp320[12]="$undefined.";static char _tmp321[5]="AUTO";static char _tmp322[9]="REGISTER";static char _tmp323[7]="STATIC";static char _tmp324[7]="EXTERN";static char _tmp325[8]="TYPEDEF";static char _tmp326[5]="VOID";static char _tmp327[5]="CHAR";static char _tmp328[6]="SHORT";static char _tmp329[4]="INT";static char _tmp32A[5]="LONG";static char _tmp32B[6]="FLOAT";static char _tmp32C[7]="DOUBLE";static char _tmp32D[7]="SIGNED";static char _tmp32E[9]="UNSIGNED";static char _tmp32F[6]="CONST";static char _tmp330[9]="VOLATILE";static char _tmp331[9]="RESTRICT";static char _tmp332[7]="STRUCT";static char _tmp333[6]="UNION";static char _tmp334[5]="CASE";static char _tmp335[8]="DEFAULT";static char _tmp336[7]="INLINE";static char _tmp337[7]="SIZEOF";static char _tmp338[9]="OFFSETOF";static char _tmp339[3]="IF";static char _tmp33A[5]="ELSE";static char _tmp33B[7]="SWITCH";static char _tmp33C[6]="WHILE";static char _tmp33D[3]="DO";static char _tmp33E[4]="FOR";static char _tmp33F[5]="GOTO";static char _tmp340[9]="CONTINUE";static char _tmp341[6]="BREAK";static char _tmp342[7]="RETURN";static char _tmp343[5]="ENUM";static char _tmp344[8]="NULL_kw";static char _tmp345[4]="LET";static char _tmp346[6]="THROW";static char _tmp347[4]="TRY";static char _tmp348[6]="CATCH";static char _tmp349[7]="EXPORT";static char _tmp34A[4]="NEW";static char _tmp34B[9]="ABSTRACT";static char _tmp34C[9]="FALLTHRU";static char _tmp34D[6]="USING";static char _tmp34E[10]="NAMESPACE";static char _tmp34F[9]="DATATYPE";static char _tmp350[8]="XTUNION";static char _tmp351[7]="TUNION";static char _tmp352[7]="MALLOC";static char _tmp353[8]="RMALLOC";static char _tmp354[7]="CALLOC";static char _tmp355[8]="RCALLOC";static char _tmp356[5]="SWAP";static char _tmp357[9]="REGION_T";static char _tmp358[6]="TAG_T";static char _tmp359[7]="REGION";static char _tmp35A[5]="RNEW";static char _tmp35B[8]="REGIONS";static char _tmp35C[13]="RESET_REGION";static char _tmp35D[16]="NOZEROTERM_QUAL";static char _tmp35E[14]="ZEROTERM_QUAL";static char _tmp35F[12]="REGION_QUAL";static char _tmp360[7]="PORTON";static char _tmp361[8]="PORTOFF";static char _tmp362[12]="DYNREGION_T";static char _tmp363[8]="NUMELTS";static char _tmp364[8]="VALUEOF";static char _tmp365[10]="VALUEOF_T";static char _tmp366[9]="TAGCHECK";static char _tmp367[13]="NUMELTS_QUAL";static char _tmp368[10]="THIN_QUAL";static char _tmp369[9]="FAT_QUAL";static char _tmp36A[13]="NOTNULL_QUAL";static char _tmp36B[14]="NULLABLE_QUAL";static char _tmp36C[14]="REQUIRES_QUAL";static char _tmp36D[12]="TAGGED_QUAL";static char _tmp36E[16]="EXTENSIBLE_QUAL";static char _tmp36F[15]="RESETABLE_QUAL";static char _tmp370[7]="PTR_OP";static char _tmp371[7]="INC_OP";static char _tmp372[7]="DEC_OP";static char _tmp373[8]="LEFT_OP";static char _tmp374[9]="RIGHT_OP";static char _tmp375[6]="LE_OP";static char _tmp376[6]="GE_OP";static char _tmp377[6]="EQ_OP";static char _tmp378[6]="NE_OP";static char _tmp379[7]="AND_OP";static char _tmp37A[6]="OR_OP";static char _tmp37B[11]="MUL_ASSIGN";static char _tmp37C[11]="DIV_ASSIGN";static char _tmp37D[11]="MOD_ASSIGN";static char _tmp37E[11]="ADD_ASSIGN";static char _tmp37F[11]="SUB_ASSIGN";static char _tmp380[12]="LEFT_ASSIGN";static char _tmp381[13]="RIGHT_ASSIGN";static char _tmp382[11]="AND_ASSIGN";static char _tmp383[11]="XOR_ASSIGN";static char _tmp384[10]="OR_ASSIGN";static char _tmp385[9]="ELLIPSIS";static char _tmp386[11]="LEFT_RIGHT";static char _tmp387[12]="COLON_COLON";static char _tmp388[11]="IDENTIFIER";static char _tmp389[17]="INTEGER_CONSTANT";static char _tmp38A[7]="STRING";static char _tmp38B[8]="WSTRING";static char _tmp38C[19]="CHARACTER_CONSTANT";static char _tmp38D[20]="WCHARACTER_CONSTANT";static char _tmp38E[18]="FLOATING_CONSTANT";static char _tmp38F[9]="TYPE_VAR";static char _tmp390[13]="TYPEDEF_NAME";static char _tmp391[16]="QUAL_IDENTIFIER";static char _tmp392[18]="QUAL_TYPEDEF_NAME";static char _tmp393[10]="ATTRIBUTE";static char _tmp394[4]="ASM";static char _tmp395[4]="';'";static char _tmp396[4]="'{'";static char _tmp397[4]="'}'";static char _tmp398[4]="','";static char _tmp399[4]="'='";static char _tmp39A[4]="'<'";static char _tmp39B[4]="'>'";static char _tmp39C[4]="'('";static char _tmp39D[4]="')'";static char _tmp39E[4]="'_'";static char _tmp39F[4]="'$'";static char _tmp3A0[4]="':'";static char _tmp3A1[4]="'.'";static char _tmp3A2[4]="'['";static char _tmp3A3[4]="']'";static char _tmp3A4[4]="'*'";static char _tmp3A5[4]="'@'";static char _tmp3A6[4]="'?'";static char _tmp3A7[4]="'+'";static char _tmp3A8[4]="'|'";static char _tmp3A9[4]="'^'";static char _tmp3AA[4]="'&'";static char _tmp3AB[4]="'-'";static char _tmp3AC[4]="'/'";static char _tmp3AD[4]="'%'";static char _tmp3AE[4]="'~'";static char _tmp3AF[4]="'!'";static char _tmp3B0[5]="prog";static char _tmp3B1[17]="translation_unit";static char _tmp3B2[12]="export_list";static char _tmp3B3[19]="export_list_values";static char _tmp3B4[21]="external_declaration";static char _tmp3B5[15]="optional_comma";static char _tmp3B6[20]="function_definition";static char _tmp3B7[21]="function_definition2";static char _tmp3B8[13]="using_action";static char _tmp3B9[15]="unusing_action";static char _tmp3BA[17]="namespace_action";static char _tmp3BB[19]="unnamespace_action";static char _tmp3BC[12]="declaration";static char _tmp3BD[19]="resetable_qual_opt";static char _tmp3BE[17]="declaration_list";static char _tmp3BF[23]="declaration_specifiers";static char _tmp3C0[24]="storage_class_specifier";static char _tmp3C1[15]="attributes_opt";static char _tmp3C2[11]="attributes";static char _tmp3C3[15]="attribute_list";static char _tmp3C4[10]="attribute";static char _tmp3C5[15]="type_specifier";static char _tmp3C6[25]="type_specifier_notypedef";static char _tmp3C7[5]="kind";static char _tmp3C8[15]="type_qualifier";static char _tmp3C9[15]="enum_specifier";static char _tmp3CA[11]="enum_field";static char _tmp3CB[22]="enum_declaration_list";static char _tmp3CC[26]="struct_or_union_specifier";static char _tmp3CD[16]="type_params_opt";static char _tmp3CE[16]="struct_or_union";static char _tmp3CF[24]="struct_declaration_list";static char _tmp3D0[25]="struct_declaration_list0";static char _tmp3D1[21]="init_declarator_list";static char _tmp3D2[22]="init_declarator_list0";static char _tmp3D3[16]="init_declarator";static char _tmp3D4[19]="struct_declaration";static char _tmp3D5[25]="specifier_qualifier_list";static char _tmp3D6[35]="notypedef_specifier_qualifier_list";static char _tmp3D7[23]="struct_declarator_list";static char _tmp3D8[24]="struct_declarator_list0";static char _tmp3D9[18]="struct_declarator";static char _tmp3DA[19]="datatype_specifier";static char _tmp3DB[14]="qual_datatype";static char _tmp3DC[19]="datatypefield_list";static char _tmp3DD[20]="datatypefield_scope";static char _tmp3DE[14]="datatypefield";static char _tmp3DF[11]="declarator";static char _tmp3E0[23]="declarator_withtypedef";static char _tmp3E1[18]="direct_declarator";static char _tmp3E2[30]="direct_declarator_withtypedef";static char _tmp3E3[8]="pointer";static char _tmp3E4[12]="one_pointer";static char _tmp3E5[14]="pointer_quals";static char _tmp3E6[13]="pointer_qual";static char _tmp3E7[23]="pointer_null_and_bound";static char _tmp3E8[14]="pointer_bound";static char _tmp3E9[18]="zeroterm_qual_opt";static char _tmp3EA[8]="rgn_opt";static char _tmp3EB[11]="tqual_list";static char _tmp3EC[20]="parameter_type_list";static char _tmp3ED[9]="type_var";static char _tmp3EE[16]="optional_effect";static char _tmp3EF[19]="optional_rgn_order";static char _tmp3F0[10]="rgn_order";static char _tmp3F1[16]="optional_inject";static char _tmp3F2[11]="effect_set";static char _tmp3F3[14]="atomic_effect";static char _tmp3F4[11]="region_set";static char _tmp3F5[15]="parameter_list";static char _tmp3F6[22]="parameter_declaration";static char _tmp3F7[16]="identifier_list";static char _tmp3F8[17]="identifier_list0";static char _tmp3F9[12]="initializer";static char _tmp3FA[18]="array_initializer";static char _tmp3FB[17]="initializer_list";static char _tmp3FC[12]="designation";static char _tmp3FD[16]="designator_list";static char _tmp3FE[11]="designator";static char _tmp3FF[10]="type_name";static char _tmp400[14]="any_type_name";static char _tmp401[15]="type_name_list";static char _tmp402[20]="abstract_declarator";static char _tmp403[27]="direct_abstract_declarator";static char _tmp404[10]="statement";static char _tmp405[18]="labeled_statement";static char _tmp406[21]="expression_statement";static char _tmp407[19]="compound_statement";static char _tmp408[16]="block_item_list";static char _tmp409[20]="selection_statement";static char _tmp40A[15]="switch_clauses";static char _tmp40B[20]="iteration_statement";static char _tmp40C[15]="jump_statement";static char _tmp40D[12]="exp_pattern";static char _tmp40E[20]="conditional_pattern";static char _tmp40F[19]="logical_or_pattern";static char _tmp410[20]="logical_and_pattern";static char _tmp411[21]="inclusive_or_pattern";static char _tmp412[21]="exclusive_or_pattern";static char _tmp413[12]="and_pattern";static char _tmp414[17]="equality_pattern";static char _tmp415[19]="relational_pattern";static char _tmp416[14]="shift_pattern";static char _tmp417[17]="additive_pattern";static char _tmp418[23]="multiplicative_pattern";static char _tmp419[13]="cast_pattern";static char _tmp41A[14]="unary_pattern";static char _tmp41B[16]="postfix_pattern";static char _tmp41C[16]="primary_pattern";static char _tmp41D[8]="pattern";static char _tmp41E[19]="tuple_pattern_list";static char _tmp41F[20]="tuple_pattern_list0";static char _tmp420[14]="field_pattern";static char _tmp421[19]="field_pattern_list";static char _tmp422[20]="field_pattern_list0";static char _tmp423[11]="expression";static char _tmp424[22]="assignment_expression";static char _tmp425[20]="assignment_operator";static char _tmp426[23]="conditional_expression";static char _tmp427[20]="constant_expression";static char _tmp428[22]="logical_or_expression";static char _tmp429[23]="logical_and_expression";static char _tmp42A[24]="inclusive_or_expression";static char _tmp42B[24]="exclusive_or_expression";static char _tmp42C[15]="and_expression";static char _tmp42D[20]="equality_expression";static char _tmp42E[22]="relational_expression";static char _tmp42F[17]="shift_expression";static char _tmp430[20]="additive_expression";static char _tmp431[26]="multiplicative_expression";static char _tmp432[16]="cast_expression";static char _tmp433[17]="unary_expression";static char _tmp434[15]="unary_operator";static char _tmp435[19]="postfix_expression";static char _tmp436[17]="field_expression";static char _tmp437[19]="primary_expression";static char _tmp438[25]="argument_expression_list";static char _tmp439[26]="argument_expression_list0";static char _tmp43A[9]="constant";static char _tmp43B[20]="qual_opt_identifier";static char _tmp43C[17]="qual_opt_typedef";static char _tmp43D[18]="struct_union_name";static char _tmp43E[11]="field_name";static char _tmp43F[12]="right_angle";
# 1555 "parse.y"
static struct _dyneither_ptr Cyc_yytname[290]={{_tmp31E,_tmp31E,_tmp31E + 2},{_tmp31F,_tmp31F,_tmp31F + 6},{_tmp320,_tmp320,_tmp320 + 12},{_tmp321,_tmp321,_tmp321 + 5},{_tmp322,_tmp322,_tmp322 + 9},{_tmp323,_tmp323,_tmp323 + 7},{_tmp324,_tmp324,_tmp324 + 7},{_tmp325,_tmp325,_tmp325 + 8},{_tmp326,_tmp326,_tmp326 + 5},{_tmp327,_tmp327,_tmp327 + 5},{_tmp328,_tmp328,_tmp328 + 6},{_tmp329,_tmp329,_tmp329 + 4},{_tmp32A,_tmp32A,_tmp32A + 5},{_tmp32B,_tmp32B,_tmp32B + 6},{_tmp32C,_tmp32C,_tmp32C + 7},{_tmp32D,_tmp32D,_tmp32D + 7},{_tmp32E,_tmp32E,_tmp32E + 9},{_tmp32F,_tmp32F,_tmp32F + 6},{_tmp330,_tmp330,_tmp330 + 9},{_tmp331,_tmp331,_tmp331 + 9},{_tmp332,_tmp332,_tmp332 + 7},{_tmp333,_tmp333,_tmp333 + 6},{_tmp334,_tmp334,_tmp334 + 5},{_tmp335,_tmp335,_tmp335 + 8},{_tmp336,_tmp336,_tmp336 + 7},{_tmp337,_tmp337,_tmp337 + 7},{_tmp338,_tmp338,_tmp338 + 9},{_tmp339,_tmp339,_tmp339 + 3},{_tmp33A,_tmp33A,_tmp33A + 5},{_tmp33B,_tmp33B,_tmp33B + 7},{_tmp33C,_tmp33C,_tmp33C + 6},{_tmp33D,_tmp33D,_tmp33D + 3},{_tmp33E,_tmp33E,_tmp33E + 4},{_tmp33F,_tmp33F,_tmp33F + 5},{_tmp340,_tmp340,_tmp340 + 9},{_tmp341,_tmp341,_tmp341 + 6},{_tmp342,_tmp342,_tmp342 + 7},{_tmp343,_tmp343,_tmp343 + 5},{_tmp344,_tmp344,_tmp344 + 8},{_tmp345,_tmp345,_tmp345 + 4},{_tmp346,_tmp346,_tmp346 + 6},{_tmp347,_tmp347,_tmp347 + 4},{_tmp348,_tmp348,_tmp348 + 6},{_tmp349,_tmp349,_tmp349 + 7},{_tmp34A,_tmp34A,_tmp34A + 4},{_tmp34B,_tmp34B,_tmp34B + 9},{_tmp34C,_tmp34C,_tmp34C + 9},{_tmp34D,_tmp34D,_tmp34D + 6},{_tmp34E,_tmp34E,_tmp34E + 10},{_tmp34F,_tmp34F,_tmp34F + 9},{_tmp350,_tmp350,_tmp350 + 8},{_tmp351,_tmp351,_tmp351 + 7},{_tmp352,_tmp352,_tmp352 + 7},{_tmp353,_tmp353,_tmp353 + 8},{_tmp354,_tmp354,_tmp354 + 7},{_tmp355,_tmp355,_tmp355 + 8},{_tmp356,_tmp356,_tmp356 + 5},{_tmp357,_tmp357,_tmp357 + 9},{_tmp358,_tmp358,_tmp358 + 6},{_tmp359,_tmp359,_tmp359 + 7},{_tmp35A,_tmp35A,_tmp35A + 5},{_tmp35B,_tmp35B,_tmp35B + 8},{_tmp35C,_tmp35C,_tmp35C + 13},{_tmp35D,_tmp35D,_tmp35D + 16},{_tmp35E,_tmp35E,_tmp35E + 14},{_tmp35F,_tmp35F,_tmp35F + 12},{_tmp360,_tmp360,_tmp360 + 7},{_tmp361,_tmp361,_tmp361 + 8},{_tmp362,_tmp362,_tmp362 + 12},{_tmp363,_tmp363,_tmp363 + 8},{_tmp364,_tmp364,_tmp364 + 8},{_tmp365,_tmp365,_tmp365 + 10},{_tmp366,_tmp366,_tmp366 + 9},{_tmp367,_tmp367,_tmp367 + 13},{_tmp368,_tmp368,_tmp368 + 10},{_tmp369,_tmp369,_tmp369 + 9},{_tmp36A,_tmp36A,_tmp36A + 13},{_tmp36B,_tmp36B,_tmp36B + 14},{_tmp36C,_tmp36C,_tmp36C + 14},{_tmp36D,_tmp36D,_tmp36D + 12},{_tmp36E,_tmp36E,_tmp36E + 16},{_tmp36F,_tmp36F,_tmp36F + 15},{_tmp370,_tmp370,_tmp370 + 7},{_tmp371,_tmp371,_tmp371 + 7},{_tmp372,_tmp372,_tmp372 + 7},{_tmp373,_tmp373,_tmp373 + 8},{_tmp374,_tmp374,_tmp374 + 9},{_tmp375,_tmp375,_tmp375 + 6},{_tmp376,_tmp376,_tmp376 + 6},{_tmp377,_tmp377,_tmp377 + 6},{_tmp378,_tmp378,_tmp378 + 6},{_tmp379,_tmp379,_tmp379 + 7},{_tmp37A,_tmp37A,_tmp37A + 6},{_tmp37B,_tmp37B,_tmp37B + 11},{_tmp37C,_tmp37C,_tmp37C + 11},{_tmp37D,_tmp37D,_tmp37D + 11},{_tmp37E,_tmp37E,_tmp37E + 11},{_tmp37F,_tmp37F,_tmp37F + 11},{_tmp380,_tmp380,_tmp380 + 12},{_tmp381,_tmp381,_tmp381 + 13},{_tmp382,_tmp382,_tmp382 + 11},{_tmp383,_tmp383,_tmp383 + 11},{_tmp384,_tmp384,_tmp384 + 10},{_tmp385,_tmp385,_tmp385 + 9},{_tmp386,_tmp386,_tmp386 + 11},{_tmp387,_tmp387,_tmp387 + 12},{_tmp388,_tmp388,_tmp388 + 11},{_tmp389,_tmp389,_tmp389 + 17},{_tmp38A,_tmp38A,_tmp38A + 7},{_tmp38B,_tmp38B,_tmp38B + 8},{_tmp38C,_tmp38C,_tmp38C + 19},{_tmp38D,_tmp38D,_tmp38D + 20},{_tmp38E,_tmp38E,_tmp38E + 18},{_tmp38F,_tmp38F,_tmp38F + 9},{_tmp390,_tmp390,_tmp390 + 13},{_tmp391,_tmp391,_tmp391 + 16},{_tmp392,_tmp392,_tmp392 + 18},{_tmp393,_tmp393,_tmp393 + 10},{_tmp394,_tmp394,_tmp394 + 4},{_tmp395,_tmp395,_tmp395 + 4},{_tmp396,_tmp396,_tmp396 + 4},{_tmp397,_tmp397,_tmp397 + 4},{_tmp398,_tmp398,_tmp398 + 4},{_tmp399,_tmp399,_tmp399 + 4},{_tmp39A,_tmp39A,_tmp39A + 4},{_tmp39B,_tmp39B,_tmp39B + 4},{_tmp39C,_tmp39C,_tmp39C + 4},{_tmp39D,_tmp39D,_tmp39D + 4},{_tmp39E,_tmp39E,_tmp39E + 4},{_tmp39F,_tmp39F,_tmp39F + 4},{_tmp3A0,_tmp3A0,_tmp3A0 + 4},{_tmp3A1,_tmp3A1,_tmp3A1 + 4},{_tmp3A2,_tmp3A2,_tmp3A2 + 4},{_tmp3A3,_tmp3A3,_tmp3A3 + 4},{_tmp3A4,_tmp3A4,_tmp3A4 + 4},{_tmp3A5,_tmp3A5,_tmp3A5 + 4},{_tmp3A6,_tmp3A6,_tmp3A6 + 4},{_tmp3A7,_tmp3A7,_tmp3A7 + 4},{_tmp3A8,_tmp3A8,_tmp3A8 + 4},{_tmp3A9,_tmp3A9,_tmp3A9 + 4},{_tmp3AA,_tmp3AA,_tmp3AA + 4},{_tmp3AB,_tmp3AB,_tmp3AB + 4},{_tmp3AC,_tmp3AC,_tmp3AC + 4},{_tmp3AD,_tmp3AD,_tmp3AD + 4},{_tmp3AE,_tmp3AE,_tmp3AE + 4},{_tmp3AF,_tmp3AF,_tmp3AF + 4},{_tmp3B0,_tmp3B0,_tmp3B0 + 5},{_tmp3B1,_tmp3B1,_tmp3B1 + 17},{_tmp3B2,_tmp3B2,_tmp3B2 + 12},{_tmp3B3,_tmp3B3,_tmp3B3 + 19},{_tmp3B4,_tmp3B4,_tmp3B4 + 21},{_tmp3B5,_tmp3B5,_tmp3B5 + 15},{_tmp3B6,_tmp3B6,_tmp3B6 + 20},{_tmp3B7,_tmp3B7,_tmp3B7 + 21},{_tmp3B8,_tmp3B8,_tmp3B8 + 13},{_tmp3B9,_tmp3B9,_tmp3B9 + 15},{_tmp3BA,_tmp3BA,_tmp3BA + 17},{_tmp3BB,_tmp3BB,_tmp3BB + 19},{_tmp3BC,_tmp3BC,_tmp3BC + 12},{_tmp3BD,_tmp3BD,_tmp3BD + 19},{_tmp3BE,_tmp3BE,_tmp3BE + 17},{_tmp3BF,_tmp3BF,_tmp3BF + 23},{_tmp3C0,_tmp3C0,_tmp3C0 + 24},{_tmp3C1,_tmp3C1,_tmp3C1 + 15},{_tmp3C2,_tmp3C2,_tmp3C2 + 11},{_tmp3C3,_tmp3C3,_tmp3C3 + 15},{_tmp3C4,_tmp3C4,_tmp3C4 + 10},{_tmp3C5,_tmp3C5,_tmp3C5 + 15},{_tmp3C6,_tmp3C6,_tmp3C6 + 25},{_tmp3C7,_tmp3C7,_tmp3C7 + 5},{_tmp3C8,_tmp3C8,_tmp3C8 + 15},{_tmp3C9,_tmp3C9,_tmp3C9 + 15},{_tmp3CA,_tmp3CA,_tmp3CA + 11},{_tmp3CB,_tmp3CB,_tmp3CB + 22},{_tmp3CC,_tmp3CC,_tmp3CC + 26},{_tmp3CD,_tmp3CD,_tmp3CD + 16},{_tmp3CE,_tmp3CE,_tmp3CE + 16},{_tmp3CF,_tmp3CF,_tmp3CF + 24},{_tmp3D0,_tmp3D0,_tmp3D0 + 25},{_tmp3D1,_tmp3D1,_tmp3D1 + 21},{_tmp3D2,_tmp3D2,_tmp3D2 + 22},{_tmp3D3,_tmp3D3,_tmp3D3 + 16},{_tmp3D4,_tmp3D4,_tmp3D4 + 19},{_tmp3D5,_tmp3D5,_tmp3D5 + 25},{_tmp3D6,_tmp3D6,_tmp3D6 + 35},{_tmp3D7,_tmp3D7,_tmp3D7 + 23},{_tmp3D8,_tmp3D8,_tmp3D8 + 24},{_tmp3D9,_tmp3D9,_tmp3D9 + 18},{_tmp3DA,_tmp3DA,_tmp3DA + 19},{_tmp3DB,_tmp3DB,_tmp3DB + 14},{_tmp3DC,_tmp3DC,_tmp3DC + 19},{_tmp3DD,_tmp3DD,_tmp3DD + 20},{_tmp3DE,_tmp3DE,_tmp3DE + 14},{_tmp3DF,_tmp3DF,_tmp3DF + 11},{_tmp3E0,_tmp3E0,_tmp3E0 + 23},{_tmp3E1,_tmp3E1,_tmp3E1 + 18},{_tmp3E2,_tmp3E2,_tmp3E2 + 30},{_tmp3E3,_tmp3E3,_tmp3E3 + 8},{_tmp3E4,_tmp3E4,_tmp3E4 + 12},{_tmp3E5,_tmp3E5,_tmp3E5 + 14},{_tmp3E6,_tmp3E6,_tmp3E6 + 13},{_tmp3E7,_tmp3E7,_tmp3E7 + 23},{_tmp3E8,_tmp3E8,_tmp3E8 + 14},{_tmp3E9,_tmp3E9,_tmp3E9 + 18},{_tmp3EA,_tmp3EA,_tmp3EA + 8},{_tmp3EB,_tmp3EB,_tmp3EB + 11},{_tmp3EC,_tmp3EC,_tmp3EC + 20},{_tmp3ED,_tmp3ED,_tmp3ED + 9},{_tmp3EE,_tmp3EE,_tmp3EE + 16},{_tmp3EF,_tmp3EF,_tmp3EF + 19},{_tmp3F0,_tmp3F0,_tmp3F0 + 10},{_tmp3F1,_tmp3F1,_tmp3F1 + 16},{_tmp3F2,_tmp3F2,_tmp3F2 + 11},{_tmp3F3,_tmp3F3,_tmp3F3 + 14},{_tmp3F4,_tmp3F4,_tmp3F4 + 11},{_tmp3F5,_tmp3F5,_tmp3F5 + 15},{_tmp3F6,_tmp3F6,_tmp3F6 + 22},{_tmp3F7,_tmp3F7,_tmp3F7 + 16},{_tmp3F8,_tmp3F8,_tmp3F8 + 17},{_tmp3F9,_tmp3F9,_tmp3F9 + 12},{_tmp3FA,_tmp3FA,_tmp3FA + 18},{_tmp3FB,_tmp3FB,_tmp3FB + 17},{_tmp3FC,_tmp3FC,_tmp3FC + 12},{_tmp3FD,_tmp3FD,_tmp3FD + 16},{_tmp3FE,_tmp3FE,_tmp3FE + 11},{_tmp3FF,_tmp3FF,_tmp3FF + 10},{_tmp400,_tmp400,_tmp400 + 14},{_tmp401,_tmp401,_tmp401 + 15},{_tmp402,_tmp402,_tmp402 + 20},{_tmp403,_tmp403,_tmp403 + 27},{_tmp404,_tmp404,_tmp404 + 10},{_tmp405,_tmp405,_tmp405 + 18},{_tmp406,_tmp406,_tmp406 + 21},{_tmp407,_tmp407,_tmp407 + 19},{_tmp408,_tmp408,_tmp408 + 16},{_tmp409,_tmp409,_tmp409 + 20},{_tmp40A,_tmp40A,_tmp40A + 15},{_tmp40B,_tmp40B,_tmp40B + 20},{_tmp40C,_tmp40C,_tmp40C + 15},{_tmp40D,_tmp40D,_tmp40D + 12},{_tmp40E,_tmp40E,_tmp40E + 20},{_tmp40F,_tmp40F,_tmp40F + 19},{_tmp410,_tmp410,_tmp410 + 20},{_tmp411,_tmp411,_tmp411 + 21},{_tmp412,_tmp412,_tmp412 + 21},{_tmp413,_tmp413,_tmp413 + 12},{_tmp414,_tmp414,_tmp414 + 17},{_tmp415,_tmp415,_tmp415 + 19},{_tmp416,_tmp416,_tmp416 + 14},{_tmp417,_tmp417,_tmp417 + 17},{_tmp418,_tmp418,_tmp418 + 23},{_tmp419,_tmp419,_tmp419 + 13},{_tmp41A,_tmp41A,_tmp41A + 14},{_tmp41B,_tmp41B,_tmp41B + 16},{_tmp41C,_tmp41C,_tmp41C + 16},{_tmp41D,_tmp41D,_tmp41D + 8},{_tmp41E,_tmp41E,_tmp41E + 19},{_tmp41F,_tmp41F,_tmp41F + 20},{_tmp420,_tmp420,_tmp420 + 14},{_tmp421,_tmp421,_tmp421 + 19},{_tmp422,_tmp422,_tmp422 + 20},{_tmp423,_tmp423,_tmp423 + 11},{_tmp424,_tmp424,_tmp424 + 22},{_tmp425,_tmp425,_tmp425 + 20},{_tmp426,_tmp426,_tmp426 + 23},{_tmp427,_tmp427,_tmp427 + 20},{_tmp428,_tmp428,_tmp428 + 22},{_tmp429,_tmp429,_tmp429 + 23},{_tmp42A,_tmp42A,_tmp42A + 24},{_tmp42B,_tmp42B,_tmp42B + 24},{_tmp42C,_tmp42C,_tmp42C + 15},{_tmp42D,_tmp42D,_tmp42D + 20},{_tmp42E,_tmp42E,_tmp42E + 22},{_tmp42F,_tmp42F,_tmp42F + 17},{_tmp430,_tmp430,_tmp430 + 20},{_tmp431,_tmp431,_tmp431 + 26},{_tmp432,_tmp432,_tmp432 + 16},{_tmp433,_tmp433,_tmp433 + 17},{_tmp434,_tmp434,_tmp434 + 15},{_tmp435,_tmp435,_tmp435 + 19},{_tmp436,_tmp436,_tmp436 + 17},{_tmp437,_tmp437,_tmp437 + 19},{_tmp438,_tmp438,_tmp438 + 25},{_tmp439,_tmp439,_tmp439 + 26},{_tmp43A,_tmp43A,_tmp43A + 9},{_tmp43B,_tmp43B,_tmp43B + 20},{_tmp43C,_tmp43C,_tmp43C + 17},{_tmp43D,_tmp43D,_tmp43D + 18},{_tmp43E,_tmp43E,_tmp43E + 11},{_tmp43F,_tmp43F,_tmp43F + 12}};
# 1609
static short Cyc_yyr1[524]={0,146,147,147,147,147,147,147,147,147,147,147,148,148,149,149,149,150,150,150,151,151,152,152,152,152,153,153,154,155,156,157,158,158,158,158,158,158,158,159,159,160,160,161,161,161,161,161,161,161,161,161,161,162,162,162,162,162,162,162,163,163,164,165,165,166,166,166,166,166,166,167,167,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,169,170,170,170,171,171,171,172,172,173,173,173,174,174,174,174,174,175,175,176,176,177,177,178,178,179,180,180,181,181,182,183,183,183,183,183,183,184,184,184,184,184,184,185,186,186,187,187,187,187,187,188,188,188,189,189,190,190,190,190,191,191,191,192,192,193,193,194,194,195,195,195,195,195,195,195,195,195,195,196,196,196,196,196,196,196,196,196,196,196,197,197,198,199,199,200,200,200,200,200,200,200,200,201,201,201,202,202,203,203,203,204,204,204,205,205,206,206,206,206,207,207,208,208,209,209,210,210,211,211,212,212,213,213,213,213,214,214,215,215,216,216,216,217,218,218,219,219,220,220,220,220,220,221,221,221,221,222,223,223,224,224,225,225,226,226,226,226,226,227,227,228,228,228,229,229,229,229,229,229,229,229,229,229,229,230,230,230,230,230,230,230,- 1,- 1,231,232,232,233,233,234,234,234,234,234,234,234,234,235,235,235,235,235,235,236,236,236,236,236,236,237,237,237,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,238,238,239,240,240,241,241,242,242,243,243,244,244,245,245,246,246,246,247,247,247,247,247,248,248,248,249,249,249,250,250,250,250,251,251,252,252,252,252,252,252,253,254,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,256,256,256,257,257,258,258,259,259,259,260,260,261,261,262,262,262,263,263,263,263,263,263,263,263,263,263,263,264,264,264,264,264,264,264,265,266,266,267,267,268,268,269,269,270,270,271,271,271,272,272,272,272,272,273,273,273,274,274,274,275,275,275,275,276,276,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,277,278,278,278,279,279,279,279,279,279,279,279,279,279,280,280,280,280,281,281,281,281,281,281,281,281,281,281,282,283,283,284,284,284,284,284,285,285,286,286,287,287,288,288,289,289};
# 1665
static short Cyc_yyr2[524]={0,1,2,3,5,3,5,6,7,3,3,0,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,7,4,9,0,1,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,4,4,8,1,2,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,1,2,0,3,3,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,4,5,4,4,2,1,1,3,4,4,5,4,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,3,5,4,4,2,1,1,1,1,1,1,5,0,5,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,3,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,6,7,1,1,3,3,1,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1721
static short Cyc_yydefact[1038]={0,19,53,54,55,56,58,73,76,77,78,79,80,81,82,83,97,98,99,115,116,49,0,0,59,0,0,150,90,94,0,0,0,0,0,0,40,514,216,516,515,517,0,0,74,0,202,202,201,1,0,17,0,0,18,0,0,43,51,45,71,47,84,85,0,86,0,39,161,0,186,189,87,165,113,57,56,50,0,101,513,0,514,509,510,511,512,113,0,378,0,0,0,0,239,0,380,381,28,30,0,0,0,0,0,0,0,151,0,0,0,0,0,0,0,199,200,2,0,0,0,0,0,32,0,121,122,124,44,52,46,48,117,518,519,113,113,39,41,39,0,21,0,218,0,174,162,187,196,195,0,0,193,194,197,198,207,189,0,72,0,57,105,0,103,0,514,389,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,498,499,478,0,0,0,0,0,481,479,480,0,406,408,422,430,432,434,436,438,440,443,448,451,454,458,0,460,482,497,496,0,390,388,35,0,0,113,0,0,0,131,127,129,258,260,0,0,9,10,0,0,113,520,521,217,96,0,0,166,75,237,0,234,0,3,0,5,0,0,0,33,0,0,39,21,0,118,119,144,112,0,148,0,0,0,0,0,0,0,0,0,0,0,0,514,291,293,0,301,295,0,299,281,282,283,0,284,285,286,0,42,21,124,20,22,265,0,224,240,0,0,220,218,0,204,0,0,0,209,60,208,190,0,0,106,102,0,0,382,0,0,403,0,0,0,0,254,399,404,0,401,0,468,0,424,458,0,425,426,0,0,0,0,0,0,0,0,0,461,462,39,0,0,0,464,465,463,0,379,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,412,413,414,415,416,417,418,419,420,421,411,0,466,0,488,489,0,0,0,501,0,113,396,397,0,394,0,241,0,0,0,0,261,0,232,132,137,133,135,128,130,218,0,267,259,268,523,522,0,89,93,0,91,95,111,66,65,0,63,167,218,236,163,267,238,175,176,0,88,203,29,0,31,0,37,0,0,123,125,243,242,21,23,108,120,0,0,0,139,140,142,0,113,113,156,0,0,0,0,0,514,0,39,0,330,331,332,0,0,334,0,0,39,0,302,296,124,300,294,292,24,0,173,225,0,0,0,231,219,226,170,0,0,0,220,172,206,205,168,204,0,0,210,61,114,0,107,429,104,100,392,393,257,0,400,253,255,387,0,0,0,0,244,249,0,0,0,0,0,0,0,0,0,0,0,0,0,500,507,0,506,407,431,0,433,435,437,439,441,442,446,447,444,445,449,450,452,453,455,456,457,410,409,487,484,0,486,0,0,0,384,0,391,34,0,385,0,262,0,138,134,136,0,220,0,204,0,269,0,218,0,280,264,0,113,0,0,0,131,0,113,0,218,0,185,164,235,4,6,0,0,25,0,143,126,144,0,0,220,149,158,157,0,0,152,0,0,0,309,0,0,0,0,0,0,329,333,0,0,0,0,297,290,39,26,266,218,0,228,0,0,221,0,171,224,212,169,192,191,210,188,0,7,0,0,256,402,405,467,0,0,245,0,250,470,0,0,0,0,474,477,0,0,0,0,0,459,503,0,0,485,483,0,0,395,398,386,263,233,276,0,270,271,204,0,0,220,204,0,92,220,0,0,0,62,64,0,177,0,0,220,0,204,0,0,0,141,146,145,117,147,159,156,156,0,0,0,0,0,0,0,0,0,0,0,0,0,309,335,0,0,298,27,220,0,229,227,0,220,0,211,0,8,383,493,0,492,0,246,251,0,0,0,0,427,428,487,486,505,0,508,423,502,504,0,275,273,279,278,0,272,204,117,0,69,67,68,178,184,181,0,183,179,204,0,36,0,0,155,154,303,309,0,0,0,0,0,0,337,338,340,342,344,346,348,350,353,358,361,364,368,370,376,377,0,39,306,315,0,0,0,0,0,0,0,0,0,0,336,287,214,230,222,213,218,13,0,14,469,0,0,252,471,0,0,476,475,490,0,277,274,0,0,182,180,0,110,0,0,0,309,0,374,0,0,371,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,372,310,0,317,0,0,0,325,0,0,0,0,0,0,308,0,220,12,15,0,495,494,0,0,0,491,109,0,38,160,304,305,0,0,0,0,309,311,341,0,343,345,347,349,351,352,356,357,354,355,359,360,362,363,365,366,367,0,316,318,319,0,327,326,0,321,0,0,0,223,215,16,0,0,0,0,0,307,373,0,369,312,0,39,320,328,322,323,0,248,247,472,0,70,0,339,309,313,324,0,375,314,473,0,0,0};
# 1828
static short Cyc_yydefgoto[144]={1035,49,688,881,50,300,51,284,52,456,53,458,54,55,139,56,57,527,226,442,443,227,60,240,228,62,162,163,63,159,64,261,262,124,125,126,263,229,424,472,473,474,65,66,647,648,649,67,475,68,447,69,70,156,157,71,115,523,315,685,607,72,608,517,676,509,513,514,418,308,248,93,94,548,464,549,330,331,332,230,301,302,609,430,287,288,289,290,291,292,764,293,294,842,843,844,845,846,847,848,849,850,851,852,853,854,855,856,857,333,409,410,334,335,336,295,197,396,198,533,199,200,201,202,203,204,205,206,207,208,209,210,211,212,790,213,564,565,214,215,74,882,241,434};
# 1846
static short Cyc_yypact[1038]={2760,- -32768,- -32768,- -32768,- -32768,- 71,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3413,235,3186,- -32768,- 5,- 29,- -32768,- 15,53,187,203,153,225,120,304,- -32768,- -32768,278,- -32768,- -32768,- -32768,270,673,297,279,296,296,- -32768,- -32768,2623,- -32768,566,599,- -32768,370,845,3413,3413,3413,- -32768,3413,- -32768,- -32768,358,- -32768,- 5,3332,150,68,612,760,- -32768,- -32768,307,330,361,- -32768,- 5,356,- -32768,6792,90,- -32768,- -32768,- -32768,- -32768,307,6187,- -32768,363,390,6792,418,421,422,- -32768,256,- -32768,- -32768,3559,3559,2623,2623,3559,6187,595,- -32768,135,386,696,442,135,3924,6187,- -32768,- -32768,- -32768,2623,2896,2623,2896,74,- -32768,457,456,- -32768,3250,- -32768,- -32768,- -32768,- -32768,3924,- -32768,- -32768,307,171,1772,- -32768,3332,845,476,3559,3486,4557,- -32768,150,- -32768,- -32768,- -32768,496,498,- -32768,- -32768,- -32768,- -32768,78,760,3559,- -32768,2896,- -32768,504,578,519,- 5,104,- -32768,6792,102,3079,6421,586,6187,6265,588,597,616,623,628,632,638,643,6499,6499,- -32768,- -32768,- -32768,2339,651,6577,6577,6577,- -32768,- -32768,- -32768,238,- -32768,- -32768,46,603,570,565,585,662,233,676,119,204,- -32768,905,6577,217,- 21,- -32768,617,6730,683,- -32768,- -32768,685,6187,307,6730,670,3705,3924,3997,3924,497,- -32768,6,6,- -32768,- -32768,8,326,307,- -32768,- -32768,- -32768,- -32768,48,671,- -32768,- -32768,521,335,- -32768,680,- -32768,682,- -32768,695,185,693,- -32768,696,4655,3332,476,697,3924,- -32768,546,708,- 5,719,728,58,733,4067,736,761,756,771,4753,4067,103,753,770,- -32768,- -32768,777,1915,1915,845,1915,- -32768,- -32768,- -32768,765,- -32768,- -32768,- -32768,295,- -32768,476,789,- -32768,- -32768,780,86,817,- -32768,9,799,795,484,804,703,800,3559,6187,- -32768,818,- -32768,- -32768,86,813,- 5,- -32768,6187,816,- -32768,814,821,- -32768,135,6187,6792,310,- -32768,- -32768,- -32768,827,820,2339,- -32768,3924,- -32768,- -32768,4165,- -32768,846,6187,6187,6187,6187,6187,6187,3924,740,2339,- -32768,- -32768,2058,822,355,6187,- -32768,- -32768,- -32768,6187,- -32768,6577,6187,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6187,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6187,- -32768,135,- -32768,- -32768,4848,135,6187,- -32768,828,307,- -32768,- -32768,826,832,6792,- -32768,556,3079,831,3559,- -32768,835,837,- -32768,3997,3997,3997,- -32768,- -32768,3043,4943,49,- -32768,277,- -32768,- -32768,9,- -32768,- -32768,3559,- -32768,- -32768,842,- -32768,839,836,848,- -32768,2970,- -32768,308,292,- -32768,- -32768,- -32768,3924,- -32768,- -32768,- -32768,2623,- -32768,2623,- -32768,861,847,- -32768,- -32768,- -32768,- -32768,476,- -32768,- -32768,- -32768,678,6187,854,852,- -32768,33,253,307,307,815,6187,6187,850,857,6187,810,952,1300,865,- -32768,- -32768,- -32768,568,944,- -32768,5041,6187,2201,2477,- -32768,- -32768,3250,- -32768,- -32768,- -32768,- -32768,3559,- -32768,- -32768,3924,862,3778,- -32768,- -32768,853,- -32768,9,867,3851,795,- -32768,- -32768,- -32768,- -32768,703,159,868,757,- -32768,- -32768,1453,- -32768,- -32768,- -32768,- -32768,3924,- -32768,- -32768,856,- -32768,- -32768,- -32768,- -32768,6691,881,870,904,- -32768,- -32768,723,4655,884,895,900,901,364,897,903,246,906,910,6343,- -32768,- -32768,907,913,- -32768,603,123,570,565,585,662,233,233,676,676,676,676,119,119,204,204,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,909,- -32768,- 53,3559,4459,- -32768,6766,- -32768,- -32768,911,- -32768,227,- -32768,3924,- -32768,- -32768,- -32768,914,795,916,703,894,277,3559,3632,5136,- -32768,- -32768,6,307,735,917,48,3116,918,307,3559,3486,5234,- -32768,308,- -32768,- -32768,- -32768,920,931,- -32768,762,- -32768,- -32768,546,6187,6187,795,- -32768,- -32768,- -32768,919,- 5,573,371,378,6187,834,389,4067,922,5332,5427,584,- -32768,- -32768,929,932,923,393,1915,- -32768,3332,- -32768,780,933,3559,- -32768,935,9,- -32768,928,- -32768,142,- -32768,- -32768,- -32768,- -32768,757,- -32768,954,- -32768,2623,969,- -32768,- -32768,- -32768,956,332,955,- -32768,4263,- -32768,- -32768,6187,1053,6187,6265,- -32768,- -32768,135,135,956,957,4459,- -32768,- -32768,6187,6187,- -32768,- -32768,86,749,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,958,- -32768,- -32768,703,86,959,795,703,948,- -32768,795,412,961,962,- -32768,- -32768,963,- -32768,86,965,795,966,703,949,6187,964,- -32768,- -32768,- -32768,3924,- -32768,968,35,815,4067,977,971,1000,970,980,4067,6187,5522,594,5617,611,5712,834,- -32768,983,984,- -32768,- -32768,795,283,- -32768,- -32768,991,795,3924,- -32768,197,- -32768,- -32768,- -32768,340,- -32768,6187,- -32768,- -32768,4655,978,982,987,- -32768,846,992,994,- -32768,772,- -32768,- -32768,- -32768,- -32768,4459,- -32768,- -32768,- -32768,- -32768,996,- -32768,703,3924,1007,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,997,- -32768,- -32768,703,419,- -32768,1004,3924,- -32768,- -32768,1090,834,1010,6655,1001,2477,6577,1003,- -32768,57,- -32768,1040,1005,767,819,249,829,377,317,- -32768,- -32768,- -32768,- -32768,1047,6577,2058,- -32768,- -32768,439,4067,465,5807,4067,488,5902,5997,613,1021,- -32768,- -32768,- -32768,- -32768,1024,- -32768,933,- -32768,1027,622,- -32768,475,151,- -32768,- -32768,3924,1124,- -32768,- -32768,- -32768,4361,- -32768,- -32768,1029,1030,- -32768,- -32768,1032,- -32768,516,4067,1033,834,2339,- -32768,3924,1028,- -32768,1629,6577,6187,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6577,6187,- -32768,- -32768,1037,- -32768,4067,4067,518,- -32768,4067,4067,536,4067,543,6092,- -32768,9,795,- -32768,- -32768,595,- -32768,- -32768,2477,1031,1036,- -32768,- -32768,1050,- -32768,- -32768,- -32768,- -32768,1038,1039,1041,6577,834,- -32768,603,158,570,565,565,662,233,233,676,676,676,676,119,119,204,204,- -32768,- -32768,- -32768,209,- -32768,- -32768,- -32768,4067,- -32768,- -32768,4067,- -32768,4067,4067,552,- -32768,- -32768,- -32768,1043,798,1044,3924,1052,- -32768,956,332,- -32768,- -32768,6187,1629,- -32768,- -32768,- -32768,- -32768,4067,- -32768,- -32768,- -32768,1054,- -32768,400,- -32768,834,- -32768,- -32768,1055,- -32768,- -32768,- -32768,1165,1167,- -32768};
# 1953
static short Cyc_yypgoto[144]={- -32768,98,- -32768,219,- -32768,- 212,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 22,- -32768,- 110,39,- -32768,- -32768,0,555,- -32768,32,- 171,1062,5,- -32768,- -32768,- 135,- -32768,349,1146,- 705,- -32768,- -32768,- -32768,926,924,759,462,- -32768,- -32768,544,- -32768,- -32768,164,- -32768,- -32768,271,- 224,1119,- 281,- 26,- -32768,1034,- -32768,- -32768,1142,- 453,- -32768,506,- 120,- 150,- 119,- 438,248,514,523,- 430,- 475,- 111,- 414,- 114,- -32768,- 245,- 165,- 552,- 326,- -32768,863,- 149,143,28,11,- 249,232,- -32768,- -32768,- 52,- 277,- -32768,- 228,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,55,973,- -32768,656,786,- -32768,247,582,- -32768,- 161,- 309,- 156,- 364,- 363,- 343,833,- 359,- 344,- 140,- 331,- 328,- 157,690,441,855,194,- -32768,- 382,- -32768,174,499,- 33,10,- 314,7};
# 1975
static short Cyc_yytable[6933]={58,567,247,617,569,61,316,499,500,343,502,572,340,463,537,141,550,259,344,590,538,58,446,306,307,570,61,573,574,309,323,134,59,360,361,362,674,75,631,357,645,646,718,110,147,138,579,580,467,831,58,581,582,59,397,61,422,58,58,58,77,58,61,61,61,440,61,58,145,363,510,681,61,134,135,260,419,99,95,560,716,680,59,404,588,505,677,297,591,59,59,59,431,59,431,671,128,129,130,59,131,37,58,58,631,138,140,61,61,100,40,641,896,664,405,285,237,296,58,58,58,58,38,61,61,61,61,58,723,511,436,432,61,432,59,59,167,58,365,58,19,20,61,433,61,433,145,218,117,912,59,59,59,59,441,512,- 153,727,804,59,58,532,638,642,37,61,140,630,532,59,725,59,431,40,37,426,286,101,140,612,254,427,366,40,481,531,318,482,544,519,545,38,59,913,43,630,168,96,255,612,233,234,557,428,559,754,314,466,506,- 240,168,432,- 240,451,169,325,250,251,252,253,448,624,494,324,583,584,585,421,169,495,326,451,423,575,576,577,578,296,476,435,429,238,437,231,232,363,624,235,508,239,422,422,422,714,636,96,379,449,319,58,380,305,285,285,61,285,96,42,550,- 218,761,408,- 218,363,142,811,143,104,408,815,363,954,144,512,58,58,682,58,1014,61,61,59,61,814,827,158,433,817,140,398,399,400,266,37,459,698,102,825,460,507,532,39,40,41,111,528,59,59,880,59,373,374,103,286,286,529,286,127,706,399,400,363,752,753,285,196,920,921,381,1015,875,37,96,401,96,878,382,383,402,403,40,105,236,107,722,78,58,375,376,37,363,61,419,895,433,364,512,39,40,41,879,795,401,922,923,899,222,707,403,470,791,243,223,108,550,539,689,247,59,776,96,668,801,802,42,286,109,96,37,741,428,613,112,614,711,113,39,40,41,615,876,298,625,741,504,451,114,363,445,448,433,421,421,421,427,42,423,423,423,122,616,158,626,540,627,358,170,451,238,789,628,328,329,451,476,623,239,629,363,669,160,928,794,438,419,525,58,452,58,929,930,61,453,61,37,658,597,883,413,161,637,884,39,40,41,666,165,363,132,138,532,532,562,795,265,267,363,58,59,216,59,703,61,363,730,731,217,58,759,73,363,58,61,486,96,760,61,744,745,493,1002,363,242,746,926,363,765,677,927,59,775,79,97,492,98,512,140,1032,298,59,58,884,684,59,818,61,140,219,799,819,140,363,73,220,872,221,900,800,970,73,886,972,778,806,632,73,633,501,975,601,783,363,59,133,794,136,934,795,73,244,953,414,973,974,976,977,256,164,257,618,97,238,952,933,358,96,439,363,96,239,625,97,936,982,983,299,555,476,984,985,358,73,73,305,451,133,518,451,904,73,363,476,616,568,476,940,777,73,73,73,73,717,312,426,313,734,320,37,687,427,629,46,47,48,968,39,40,41,452,73,363,729,322,961,285,993,296,445,794,670,592,720,37,427,743,46,47,48,363,73,39,40,41,996,164,363,58,97,58,97,998,61,470,61,363,598,471,964,363,1020,46,47,48,969,910,118,119,661,58,684,363,909,757,61,367,758,249,791,59,321,59,37,932,771,369,286,363,140,368,39,40,41,339,866,345,97,363,96,120,121,59,902,97,346,807,370,311,650,651,667,869,654,945,363,659,363,812,406,1028,955,1013,950,347,665,951,450,46,47,48,348,823,371,372,134,349,594,73,965,350,966,948,377,378,450,351,478,521,522,483,352,96,986,987,988,16,17,18,359,80,37,978,979,980,981,37,73,787,1029,40,411,42,412,39,40,41,42,416,512,444,43,1033,454,37,455,470,1004,461,46,47,48,1012,40,46,47,48,779,457,858,468,164,645,646,43,148,149,150,643,644,477,97,46,47,48,151,152,153,154,155,285,479,465,736,737,738,696,697,37,83,185,186,84,85,86,1027,480,40,762,763,1024,484,58,338,487,341,341,61,353,488,37,189,808,809,246,354,355,489,39,40,41,496,341,341,341,604,605,606,503,667,470,285,490,264,59,892,893,526,46,47,48,286,497,341,246,498,768,770,916,917,918,919,97,58,258,97,924,925,61,433,134,1022,363,833,834,508,465,516,515,551,552,553,554,520,556,524,530,42,96,534,365,535,655,563,543,59,450,566,536,450,542,561,286,37,593,595,596,73,602,73,600,603,40,384,619,621,123,620,586,634,735,450,622,43,635,639,640,450,652,653,587,46,47,48,656,563,660,420,662,425,672,690,675,835,694,285,678,683,829,862,385,386,387,388,389,390,391,392,393,394,693,611,695,699,341,863,865,58,868,700,871,341,61,264,701,702,704,838,839,728,395,73,705,709,721,708,712,713,715,750,80,885,755,724,97,726,739,742,749,59,766,772,774,773,305,782,286,341,780,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,786,788,710,563,797,792,563,816,828,830,803,810,813,196,820,821,822,81,824,826,832,97,935,836,837,939,860,861,873,874,877,887,166,83,888,889,84,85,86,938,897,40,942,944,903,890,87,891,450,894,898,901,840,908,89,90,905,914,465,911,91,962,450,841,931,450,92,193,946,915,194,195,947,756,949,956,958,960,959,358,963,967,990,1008,1006,1009,971,341,1007,1011,1021,1036,1010,1037,991,992,1003,1023,994,995,245,997,465,740,989,1025,106,1031,1034,462,751,246,469,73,146,116,785,317,1000,784,541,1001,415,733,781,692,599,1005,571,859,246,1026,0,558,0,0,748,246,0,0,0,0,0,0,0,0,0,0,0,0,0,1016,0,0,1017,0,1018,1019,0,0,563,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,341,1030,0,0,0,0,246,0,0,0,97,0,0,0,0,0,0,246,0,0,0,0,0,0,0,0,246,0,465,0,0,796,0,798,133,0,0,0,0,0,0,465,0,0,805,0,0,0,0,0,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,341,341,0,0,0,0,22,80,23,173,0,0,0,174,24,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,246,0,0,0,465,0,34,35,36,420,183,184,0,246,0,0,0,0,465,0,341,0,0,0,0,0,0,0,0,0,0,341,0,37,83,185,186,84,85,86,38,39,40,41,42,187,657,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,0,0,0,0,133,0,0,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,465,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,686,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,264,0,0,0,0,0,30,31,32,0,0,33,0,0,0,907,0,0,341,34,35,36,0,0,0,0,0,0,0,0,246,0,0,0,0,0,341,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,264,0,0,43,0,44,45,0,0,0,0,46,47,48,0,246,0,0,0,0,0,0,0,0,0,0,341,0,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,341,0,0,0,0,0,0,0,0,0,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,762,763,21,171,172,268,341,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,341,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,- 309,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,282,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,- 39,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,280,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,268,0,269,270,271,272,273,274,275,276,22,80,23,173,277,0,0,174,24,278,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,279,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,36,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,83,185,186,84,85,86,38,39,40,41,42,187,281,137,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,356,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,80,0,173,0,0,0,174,0,0,0,0,27,0,0,175,176,177,178,0,28,29,0,179,0,0,0,0,0,0,0,32,180,181,33,182,0,0,0,0,0,0,34,35,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,38,39,40,41,42,187,0,0,0,0,0,0,0,188,0,44,283,0,0,0,0,190,0,0,191,0,0,192,193,0,0,194,195,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,- 11,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,43,0,44,45,0,0,0,0,46,47,48,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,0,23,0,0,0,0,0,24,0,25,26,27,0,0,0,0,0,0,0,28,29,- 39,0,0,0,0,0,0,30,31,32,0,0,33,0,0,0,0,0,0,0,34,35,36,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,37,0,0,0,0,22,0,38,39,40,41,42,0,0,0,- 11,0,27,0,0,43,0,44,45,0,28,29,0,46,47,48,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,37,0,0,0,22,0,0,38,39,40,41,42,0,305,0,0,27,0,0,0,445,0,44,45,28,29,427,0,46,47,48,0,0,0,0,32,0,0,33,0,0,80,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,426,81,44,45,28,29,427,0,46,47,48,0,0,327,0,32,166,83,33,0,84,85,86,0,0,40,34,35,0,0,87,0,0,0,0,0,88,0,89,90,0,328,329,0,91,0,0,0,0,0,92,0,0,37,0,80,0,0,0,0,38,39,40,41,42,0,0,0,0,0,0,0,0,470,0,44,45,0,0,0,0,46,47,48,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,81,0,0,0,0,0,0,0,0,0,22,0,23,0,0,82,83,0,24,84,85,86,27,0,40,0,0,0,0,87,28,29,- 39,0,0,88,0,89,90,0,0,32,0,91,33,0,0,0,0,92,0,0,34,35,36,0,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,38,39,0,41,42,0,22,137,23,0,258,0,0,0,24,44,45,0,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,36,0,0,2,3,4,76,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,38,39,0,41,42,22,0,137,0,0,0,0,0,24,0,44,45,27,0,0,0,0,0,0,0,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,304,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,224,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,225,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,303,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,305,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,417,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,673,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,679,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,0,0,38,39,0,41,42,0,0,0,0,27,0,0,0,0,0,44,45,28,29,0,0,0,0,0,0,0,0,0,32,0,0,33,0,0,0,0,0,0,0,34,35,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,268,0,269,270,271,272,273,274,275,276,0,80,0,173,277,0,38,174,0,278,42,0,0,0,0,175,176,177,178,0,0,44,45,179,0,279,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,485,83,185,186,84,85,86,0,0,40,0,0,187,281,137,0,0,171,172,0,188,0,0,189,546,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,547,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,793,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,957,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,328,329,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,310,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,342,0,0,171,172,0,188,0,0,189,0,0,0,0,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,491,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,589,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,610,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,663,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,732,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,0,0,0,171,172,0,188,0,0,189,0,0,0,747,190,0,80,191,173,0,192,193,174,0,194,195,0,0,0,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,767,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,769,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,864,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,867,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,870,171,172,0,0,0,0,188,0,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,937,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,941,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,943,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,0,0,40,0,0,187,0,171,172,0,0,0,0,188,999,0,189,0,0,0,80,190,173,0,191,0,174,192,193,0,0,194,195,0,175,176,177,178,0,0,0,0,179,0,0,0,0,0,0,0,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,342,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,710,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,337,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,353,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,0,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,171,172,0,37,83,185,186,84,85,86,0,0,40,80,0,187,0,0,0,0,0,0,0,188,0,0,189,175,176,177,178,190,0,0,191,0,0,192,193,0,0,194,195,0,180,181,0,182,0,80,0,0,0,0,0,0,0,0,183,184,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,37,83,185,186,84,85,86,80,0,40,0,0,187,0,0,0,0,0,0,0,906,81,0,189,0,0,0,0,190,0,0,191,0,691,192,193,166,83,194,195,84,85,86,80,0,40,0,0,0,0,87,0,0,0,0,0,88,0,89,90,81,328,329,0,91,0,0,0,0,80,92,0,407,0,0,166,83,0,0,84,85,86,0,0,40,0,0,0,0,87,0,0,0,0,0,88,81,89,90,0,0,0,0,91,0,0,0,0,719,92,0,166,83,0,0,84,85,86,0,0,40,0,81,0,0,87,0,0,0,0,0,88,0,89,90,0,0,166,83,91,0,84,85,86,0,92,40,0,0,0,0,87,0,0,0,0,0,88,0,89,90,0,0,0,0,91,0,0,0,0,0,92};
# 2672
static short Cyc_yycheck[6933]={0,365,113,433,367,0,156,284,285,174,287,370,173,258,328,67,342,127,174,401,329,21,246,143,143,368,21,371,372,143,165,64,0,190,191,192,511,108,452,188,5,6,594,43,70,67,377,378,260,754,50,379,380,21,211,50,227,57,58,59,21,61,57,58,59,17,61,67,68,122,61,524,67,106,64,127,225,106,23,356,133,519,50,104,398,297,516,139,402,57,58,59,86,61,86,509,57,58,59,67,61,106,102,103,518,127,67,102,103,124,115,78,817,495,135,137,106,139,118,119,120,121,113,118,119,120,121,127,603,120,122,125,127,125,102,103,81,137,92,139,20,21,137,137,139,137,146,92,50,92,118,119,120,121,106,305,121,610,710,127,160,322,471,130,106,160,127,448,329,137,608,139,86,115,106,126,137,124,139,428,106,132,136,115,126,320,158,129,337,308,339,113,160,136,126,476,106,23,124,448,102,103,351,229,353,643,128,259,122,119,106,125,122,246,124,113,118,119,120,121,246,445,119,168,381,382,383,227,124,126,128,264,227,373,374,375,376,259,264,232,229,106,235,100,101,122,470,104,106,114,421,422,423,130,466,81,137,246,160,259,141,119,284,285,259,287,92,117,594,127,652,216,130,122,124,728,126,124,223,732,122,130,132,433,284,285,127,287,130,284,285,259,287,731,747,124,137,735,259,82,83,84,131,106,119,550,119,745,123,302,471,114,115,116,43,315,284,285,121,287,87,88,119,284,285,318,287,56,82,83,84,122,641,642,356,88,87,88,134,130,778,106,168,126,170,783,142,143,131,132,115,126,105,49,127,120,356,124,125,106,122,356,511,816,137,127,516,114,115,116,784,697,126,124,125,828,120,131,132,126,694,110,126,105,710,330,535,498,356,666,216,501,706,707,117,356,126,223,106,623,426,124,105,126,561,126,114,115,116,132,127,140,445,637,119,448,120,122,126,445,137,421,422,423,132,117,421,422,423,59,430,124,124,123,126,188,87,470,106,107,132,131,132,476,470,445,114,447,122,501,120,134,697,127,603,312,456,122,458,142,143,456,127,458,106,487,411,127,221,108,470,131,114,115,116,497,120,122,120,501,641,642,127,809,135,136,122,487,456,126,458,127,487,122,614,614,106,497,127,0,122,501,497,271,330,127,501,627,627,277,948,122,126,627,137,122,127,947,141,487,127,22,23,276,25,675,487,127,257,497,530,131,527,501,122,530,497,119,703,127,501,122,43,122,772,123,127,703,912,50,795,914,671,714,456,56,458,286,917,416,679,122,530,64,809,66,127,893,69,127,884,222,915,916,918,919,119,78,122,436,81,106,107,860,337,411,237,122,414,114,623,92,127,924,925,119,349,623,926,927,353,102,103,119,637,106,122,640,836,110,122,637,612,366,640,127,668,118,119,120,121,593,126,126,126,618,122,106,530,132,630,134,135,136,911,114,115,116,122,140,122,613,123,127,666,127,668,126,893,506,403,596,106,132,626,134,135,136,122,160,114,115,116,127,165,122,666,168,668,170,127,666,126,668,122,119,130,905,122,127,134,135,136,911,841,119,120,119,688,684,122,840,119,688,91,122,114,1011,666,121,668,106,859,119,139,666,122,668,138,114,115,116,126,119,126,216,122,543,119,120,688,832,223,126,717,140,144,480,481,497,119,484,119,122,487,122,729,120,1015,888,968,119,126,496,122,246,134,135,136,126,743,89,90,786,126,406,257,906,126,908,879,85,86,264,126,266,63,64,269,126,596,928,929,930,17,18,19,126,38,106,920,921,922,923,106,286,688,1015,115,106,117,106,114,115,116,117,126,947,127,126,1028,121,106,121,126,954,113,134,135,136,967,115,134,135,136,672,121,762,121,320,5,6,126,63,64,65,477,478,120,330,134,135,136,73,74,75,76,77,860,120,258,106,107,108,121,122,106,107,108,109,110,111,112,1014,126,115,22,23,1007,126,860,171,126,173,174,860,126,106,106,129,121,122,113,183,184,119,114,115,116,126,190,191,192,421,422,423,121,655,126,911,119,132,860,121,122,313,134,135,136,860,130,211,143,126,657,658,139,140,89,90,411,911,123,414,85,86,911,137,951,121,122,757,758,106,342,130,127,345,346,347,348,127,350,133,121,117,762,121,92,125,130,359,122,911,445,363,125,448,121,127,911,106,124,127,122,456,121,458,127,122,115,56,120,127,119,126,384,106,619,470,122,126,125,119,122,476,126,120,396,134,135,136,30,401,119,226,42,228,126,133,137,759,122,1015,127,127,749,765,93,94,95,96,97,98,99,100,101,102,127,427,106,127,322,766,767,1015,769,122,771,329,1015,262,122,122,127,25,26,133,123,530,127,121,121,127,127,122,127,106,38,792,121,127,543,127,127,127,126,1015,126,120,127,119,119,125,1015,365,121,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,120,106,120,495,25,124,498,133,133,119,127,127,127,840,127,127,127,91,127,127,126,596,864,120,127,867,130,121,119,119,113,127,106,107,126,122,110,111,112,866,107,115,869,870,28,127,120,127,623,127,127,121,126,126,128,129,120,91,550,130,134,903,637,137,91,640,140,141,121,138,144,145,122,648,121,25,121,119,122,906,121,127,119,107,127,121,913,471,126,122,121,0,127,0,936,937,951,127,940,941,112,943,594,622,931,127,34,127,127,257,640,426,262,688,69,47,684,157,945,679,331,947,223,615,675,543,414,954,369,762,445,1011,- 1,352,- 1,- 1,628,452,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,993,- 1,- 1,996,- 1,998,999,- 1,- 1,652,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,561,1020,- 1,- 1,- 1,- 1,498,- 1,- 1,- 1,762,- 1,- 1,- 1,- 1,- 1,- 1,509,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,518,- 1,697,- 1,- 1,700,- 1,702,786,- 1,- 1,- 1,- 1,- 1,- 1,710,- 1,- 1,713,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,641,642,- 1,- 1,- 1,- 1,37,38,39,40,- 1,- 1,- 1,44,45,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,614,- 1,- 1,- 1,795,- 1,79,80,81,623,83,84,- 1,627,- 1,- 1,- 1,- 1,809,- 1,703,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,714,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,- 1,- 1,- 1,951,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,893,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,43,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,754,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,838,- 1,- 1,841,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,784,- 1,- 1,- 1,- 1,- 1,859,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,817,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,- 1,832,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,912,- 1,914,915,916,917,918,919,920,921,922,923,924,925,926,927,928,929,930,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,967,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,1014,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,59,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,- 1,- 1,44,45,46,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,62,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,- 1,40,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,49,- 1,- 1,52,53,54,55,- 1,57,58,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,113,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,- 1,- 1,- 1,45,- 1,47,48,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,- 1,- 1,- 1,37,- 1,113,114,115,116,117,- 1,- 1,- 1,121,- 1,49,- 1,- 1,126,- 1,128,129,- 1,57,58,- 1,134,135,136,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,115,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,- 1,128,129,57,58,132,- 1,134,135,136,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,38,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,126,91,128,129,57,58,132,- 1,134,135,136,- 1,- 1,103,- 1,68,106,107,71,- 1,110,111,112,- 1,- 1,115,79,80,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,131,132,- 1,134,- 1,- 1,- 1,- 1,- 1,140,- 1,- 1,106,- 1,38,- 1,- 1,- 1,- 1,113,114,115,116,117,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,39,- 1,- 1,106,107,- 1,45,110,111,112,49,- 1,115,- 1,- 1,- 1,- 1,120,57,58,59,- 1,- 1,126,- 1,128,129,- 1,- 1,68,- 1,134,71,- 1,- 1,- 1,- 1,140,- 1,- 1,79,80,81,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,- 1,37,120,39,- 1,123,- 1,- 1,- 1,45,128,129,- 1,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,81,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,116,117,37,- 1,120,- 1,- 1,- 1,- 1,- 1,45,- 1,128,129,49,- 1,- 1,- 1,- 1,- 1,- 1,- 1,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,106,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,61,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,120,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,119,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,121,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,121,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,103,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,- 1,- 1,113,114,- 1,116,117,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,128,129,57,58,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,79,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,38,- 1,40,41,- 1,113,44,- 1,46,117,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,128,129,60,- 1,62,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,32,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,121,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,131,132,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,120,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,- 1,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,25,26,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,38,137,40,- 1,140,141,44,- 1,144,145,- 1,- 1,- 1,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,119,25,26,- 1,- 1,- 1,- 1,126,- 1,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,- 1,- 1,115,- 1,- 1,118,- 1,25,26,- 1,- 1,- 1,- 1,126,127,- 1,129,- 1,- 1,- 1,38,134,40,- 1,137,- 1,44,140,141,- 1,- 1,144,145,- 1,52,53,54,55,- 1,- 1,- 1,- 1,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,- 1,106,107,108,109,110,111,112,- 1,- 1,115,38,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,129,52,53,54,55,134,- 1,- 1,137,- 1,- 1,140,141,- 1,- 1,144,145,- 1,69,70,- 1,72,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,107,108,109,110,111,112,38,- 1,115,- 1,- 1,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,91,- 1,129,- 1,- 1,- 1,- 1,134,- 1,- 1,137,- 1,103,140,141,106,107,144,145,110,111,112,38,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,91,131,132,- 1,134,- 1,- 1,- 1,- 1,38,140,- 1,103,- 1,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,91,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,103,140,- 1,106,107,- 1,- 1,110,111,112,- 1,- 1,115,- 1,91,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,106,107,134,- 1,110,111,112,- 1,140,115,- 1,- 1,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,126,- 1,128,129,- 1,- 1,- 1,- 1,134,- 1,- 1,- 1,- 1,- 1,140};char Cyc_Yystack_overflow[17]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp4FE[8]="stdcall";static char _tmp4FF[6]="cdecl";static char _tmp500[9]="fastcall";static char _tmp501[9]="noreturn";static char _tmp502[6]="const";static char _tmp503[8]="aligned";static char _tmp504[7]="packed";static char _tmp505[7]="shared";static char _tmp506[7]="unused";static char _tmp507[5]="weak";static char _tmp508[10]="dllimport";static char _tmp509[10]="dllexport";static char _tmp50A[23]="no_instrument_function";static char _tmp50B[12]="constructor";static char _tmp50C[11]="destructor";static char _tmp50D[22]="no_check_memory_usage";static char _tmp50E[5]="pure";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf);static void _tmp9EF(union Cyc_YYSTYPE*yylval,unsigned int*_tmp9EE,unsigned int*_tmp9ED,union Cyc_YYSTYPE**_tmp9EC){for(*_tmp9EE=0;*_tmp9EE < *_tmp9ED;(*_tmp9EE)++){(*_tmp9EC)[*_tmp9EE]=(union Cyc_YYSTYPE)*yylval;}}static void _tmp9F4(unsigned int*_tmp9F3,unsigned int*_tmp9F2,struct Cyc_Yyltype**_tmp9F1){for(*_tmp9F3=0;*_tmp9F3 < *_tmp9F2;(*_tmp9F3)++){(*_tmp9F1)[*_tmp9F3]=(struct Cyc_Yyltype)
# 164 "cycbison.simple"
Cyc_yynewloc();}}static void _tmp9FB(int*yyssp_offset,struct _dyneither_ptr*yyss,unsigned int*_tmp9FA,unsigned int*_tmp9F9,short**_tmp9F7){for(*_tmp9FA=0;*_tmp9FA < *_tmp9F9;(*_tmp9FA)++){(*_tmp9F7)[*_tmp9FA]=(short)(
# 216
*_tmp9FA <= *yyssp_offset?*((short*)_check_dyneither_subscript(*yyss,sizeof(short),(int)*_tmp9FA)): 0);}}static void _tmpA01(struct _dyneither_ptr*yyvs,int*yyssp_offset,union Cyc_YYSTYPE*yylval,unsigned int*_tmpA00,unsigned int*_tmp9FF,union Cyc_YYSTYPE**_tmp9FD){for(*_tmpA00=0;*_tmpA00 < *_tmp9FF;(*_tmpA00)++){(*_tmp9FD)[*_tmpA00]=(union Cyc_YYSTYPE)(
# 219
*_tmpA00 <= *yyssp_offset?*((union Cyc_YYSTYPE*)_check_dyneither_subscript(*yyvs,sizeof(union Cyc_YYSTYPE),(int)*_tmpA00)):*yylval);}}static void _tmpA07(int*yyssp_offset,struct _dyneither_ptr*yyls,unsigned int*_tmpA06,unsigned int*_tmpA05,struct Cyc_Yyltype**_tmpA03){for(*_tmpA06=0;*_tmpA06 < *_tmpA05;(*_tmpA06)++){(*_tmpA03)[*_tmpA06]=(struct Cyc_Yyltype)(
# 222
*_tmpA06 <= *yyssp_offset?*((struct Cyc_Yyltype*)_check_dyneither_subscript(*yyls,sizeof(struct Cyc_Yyltype),(int)*_tmpA06)):
 Cyc_yynewloc());}}static void _tmpF10(unsigned int*_tmpF0F,unsigned int*_tmpF0E,char**_tmpF0C){for(*_tmpF0F=0;*_tmpF0F < *_tmpF0E;(*_tmpF0F)++){(*_tmpF0C)[*_tmpF0F]=(char)'\000';}}
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
union Cyc_YYSTYPE _tmp9E8;union Cyc_YYSTYPE yylval=((_tmp9E8.YYINITIALSVAL).val=0,(((_tmp9E8.YYINITIALSVAL).tag=64,_tmp9E8)));
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
short*_tmp9EA;unsigned int _tmp9E9;struct _dyneither_ptr yyss=(_tmp9E9=200,((_tmp9EA=_region_calloc(yyregion,sizeof(short),_tmp9E9),_tag_dyneither(_tmp9EA,sizeof(short),_tmp9E9))));
# 158
int yyvsp_offset;
unsigned int _tmp9EE;unsigned int _tmp9ED;union Cyc_YYSTYPE*_tmp9EC;unsigned int _tmp9EB;struct _dyneither_ptr yyvs=_tag_dyneither(((_tmp9EB=(unsigned int)200,((_tmp9EC=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9EB)),((((_tmp9ED=_tmp9EB,_tmp9EF(& yylval,& _tmp9EE,& _tmp9ED,& _tmp9EC))),_tmp9EC)))))),sizeof(union Cyc_YYSTYPE),(unsigned int)200);
# 163
int yylsp_offset;
unsigned int _tmp9F3;unsigned int _tmp9F2;struct Cyc_Yyltype*_tmp9F1;unsigned int _tmp9F0;struct _dyneither_ptr yyls=_tag_dyneither(((_tmp9F0=(unsigned int)200,((_tmp9F1=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmp9F0)),((((_tmp9F2=_tmp9F0,_tmp9F4(& _tmp9F3,& _tmp9F2,& _tmp9F1))),_tmp9F1)))))),sizeof(struct Cyc_Yyltype),(unsigned int)200);
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
{const char*_tmp9F5;Cyc_yyerror(((_tmp9F5="parser stack overflow",_tag_dyneither(_tmp9F5,sizeof(char),22))),yystate,yychar);}
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
unsigned int _tmp9FA;unsigned int _tmp9F9;struct _dyneither_ptr _tmp9F8;short*_tmp9F7;unsigned int _tmp9F6;struct _dyneither_ptr yyss1=(_tmp9F6=(unsigned int)yystacksize,((_tmp9F7=(short*)_region_malloc(yyregion,_check_times(sizeof(short),_tmp9F6)),((_tmp9F8=_tag_dyneither(_tmp9F7,sizeof(short),_tmp9F6),((((_tmp9F9=_tmp9F6,_tmp9FB(& yyssp_offset,& yyss,& _tmp9FA,& _tmp9F9,& _tmp9F7))),_tmp9F8)))))));
# 217
unsigned int _tmpA00;unsigned int _tmp9FF;struct _dyneither_ptr _tmp9FE;union Cyc_YYSTYPE*_tmp9FD;unsigned int _tmp9FC;struct _dyneither_ptr yyvs1=
(_tmp9FC=(unsigned int)yystacksize,((_tmp9FD=(union Cyc_YYSTYPE*)_region_malloc(yyregion,_check_times(sizeof(union Cyc_YYSTYPE),_tmp9FC)),((_tmp9FE=_tag_dyneither(_tmp9FD,sizeof(union Cyc_YYSTYPE),_tmp9FC),((((_tmp9FF=_tmp9FC,_tmpA01(& yyvs,& yyssp_offset,& yylval,& _tmpA00,& _tmp9FF,& _tmp9FD))),_tmp9FE)))))));
# 221
unsigned int _tmpA06;unsigned int _tmpA05;struct _dyneither_ptr _tmpA04;struct Cyc_Yyltype*_tmpA03;unsigned int _tmpA02;struct _dyneither_ptr yyls1=(_tmpA02=(unsigned int)yystacksize,((_tmpA03=(struct Cyc_Yyltype*)_region_malloc(yyregion,_check_times(sizeof(struct Cyc_Yyltype),_tmpA02)),((_tmpA04=_tag_dyneither(_tmpA03,sizeof(struct Cyc_Yyltype),_tmpA02),((((_tmpA05=_tmpA02,_tmpA07(& yyssp_offset,& yyls,& _tmpA06,& _tmpA05,& _tmpA03))),_tmpA04)))))));
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
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
if((yyn < 0  || yyn > 6932) || Cyc_yycheck[_check_known_subscript_notnull(6933,yyn)]!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6933,yyn)];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1037){
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
 yyn=(int)Cyc_yydefact[_check_known_subscript_notnull(1038,yystate)];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(524,yyn)];
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
{struct Cyc_Absyn_Decl*_tmpA11;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA10;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA0F;struct Cyc_List_List*_tmpA0E;yyval=Cyc_YY19(((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E->hd=((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11->r=(void*)((_tmpA0F=_cycalloc(sizeof(*_tmpA0F)),((_tmpA0F[0]=((_tmpA10.tag=10,((_tmpA10.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA10.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA10)))))),_tmpA0F)))),((_tmpA11->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA11)))))),((_tmpA0E->tl=0,_tmpA0E)))))));}
Cyc_Lex_leave_using();
# 1196
break;}case 4: _LL21C: {
# 1198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1197 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA1B;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct _tmpA1A;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA19;struct Cyc_List_List*_tmpA18;yyval=Cyc_YY19(((_tmpA18=_cycalloc(sizeof(*_tmpA18)),((_tmpA18->hd=((_tmpA1B=_cycalloc(sizeof(*_tmpA1B)),((_tmpA1B->r=(void*)((_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1A.tag=10,((_tmpA1A.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA1A.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA1A)))))),_tmpA19)))),((_tmpA1B->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA1B)))))),((_tmpA18->tl=Cyc_yyget_YY19(yyyyvsp[4]),_tmpA18)))))));}
break;}case 5: _LL21D: {
# 1200
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1202
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1200 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA2A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA29;struct _dyneither_ptr*_tmpA28;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA27;struct Cyc_List_List*_tmpA26;yyval=Cyc_YY19(((_tmpA26=_cycalloc(sizeof(*_tmpA26)),((_tmpA26->hd=((_tmpA2A=_cycalloc(sizeof(*_tmpA2A)),((_tmpA2A->r=(void*)((_tmpA27=_cycalloc(sizeof(*_tmpA27)),((_tmpA27[0]=((_tmpA29.tag=9,((_tmpA29.f1=((_tmpA28=_cycalloc(sizeof(*_tmpA28)),((_tmpA28[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpA28)))),((_tmpA29.f2=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA29)))))),_tmpA27)))),((_tmpA2A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA2A)))))),((_tmpA26->tl=0,_tmpA26)))))));}
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
{const char*_tmpA2B;nspace=((_tmpA2B="",_tag_dyneither(_tmpA2B,sizeof(char),1)));}goto _LL220;_LL223: _tmp471=_tmp46F;_LL224:(void)_throw(_tmp471);_LL220:;}};}
# 1210
{struct _handler_cons _tmp473;_push_handler(& _tmp473);{int _tmp475=0;if(setjmp(_tmp473.handler))_tmp475=1;if(!_tmp475){x=Cyc_yyget_YY19(yyyyvsp[2]);;_pop_handler();}else{void*_tmp474=(void*)_exn_thrown;void*_tmp477=_tmp474;void*_tmp479;_LL226: {struct Cyc_Core_Failure_exn_struct*_tmp478=(struct Cyc_Core_Failure_exn_struct*)_tmp477;if(_tmp478->tag != Cyc_Core_Failure)goto _LL228;}_LL227:
 x=0;goto _LL225;_LL228: _tmp479=_tmp477;_LL229:(void)_throw(_tmp479);_LL225:;}};}
# 1213
{struct _handler_cons _tmp47A;_push_handler(& _tmp47A);{int _tmp47C=0;if(setjmp(_tmp47A.handler))_tmp47C=1;if(!_tmp47C){y=Cyc_yyget_YY19(yyyyvsp[4]);;_pop_handler();}else{void*_tmp47B=(void*)_exn_thrown;void*_tmp47E=_tmp47B;void*_tmp480;_LL22B: {struct Cyc_Core_Failure_exn_struct*_tmp47F=(struct Cyc_Core_Failure_exn_struct*)_tmp47E;if(_tmp47F->tag != Cyc_Core_Failure)goto _LL22D;}_LL22C:
 y=0;goto _LL22A;_LL22D: _tmp480=_tmp47E;_LL22E:(void)_throw(_tmp480);_LL22A:;}};}
# 1216
{struct Cyc_Absyn_Decl*_tmpA3A;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct _tmpA39;struct _dyneither_ptr*_tmpA38;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA37;struct Cyc_List_List*_tmpA36;yyval=Cyc_YY19(((_tmpA36=_cycalloc(sizeof(*_tmpA36)),((_tmpA36->hd=((_tmpA3A=_cycalloc(sizeof(*_tmpA3A)),((_tmpA3A->r=(void*)((_tmpA37=_cycalloc(sizeof(*_tmpA37)),((_tmpA37[0]=((_tmpA39.tag=9,((_tmpA39.f1=((_tmpA38=_cycalloc(sizeof(*_tmpA38)),((_tmpA38[0]=nspace,_tmpA38)))),((_tmpA39.f2=x,_tmpA39)))))),_tmpA37)))),((_tmpA3A->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA3A)))))),((_tmpA36->tl=y,_tmpA36)))))));}
# 1218
break;}case 7: _LL21F: {
# 1220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1219 "parse.y"
struct _dyneither_ptr _tmp486=Cyc_yyget_String_tok(yyyyvsp[1]);
{const char*_tmpA3B;if(Cyc_strcmp((struct _dyneither_ptr)_tmp486,((_tmpA3B="C",_tag_dyneither(_tmpA3B,sizeof(char),2))))== 0){
struct Cyc_Absyn_Decl*_tmpA45;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct _tmpA44;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA43;struct Cyc_List_List*_tmpA42;yyval=Cyc_YY19(((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42->hd=((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45->r=(void*)((_tmpA43=_cycalloc(sizeof(*_tmpA43)),((_tmpA43[0]=((_tmpA44.tag=11,((_tmpA44.f1=Cyc_yyget_YY19(yyyyvsp[3]),_tmpA44)))),_tmpA43)))),((_tmpA45->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA45)))))),((_tmpA42->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA42)))))));}else{
# 1223
{const char*_tmpA46;if(Cyc_strcmp((struct _dyneither_ptr)_tmp486,((_tmpA46="C include",_tag_dyneither(_tmpA46,sizeof(char),10))))!= 0){
const char*_tmpA47;Cyc_Parse_err(((_tmpA47="expecting \"C\" or \"C include\"",_tag_dyneither(_tmpA47,sizeof(char),29))),(unsigned int)(yyyylsp[0]).first_line);}}{
# 1226
struct Cyc_Absyn_Decl*_tmpA51;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA50;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA4F;struct Cyc_List_List*_tmpA4E;yyval=Cyc_YY19(((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E->hd=((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51->r=(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=12,((_tmpA50.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA50.f2=0,_tmpA50)))))),_tmpA4F)))),((_tmpA51->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA51)))))),((_tmpA4E->tl=Cyc_yyget_YY19(yyyyvsp[5]),_tmpA4E)))))));};}}
# 1229
break;}case 8: _LL22F: {
# 1231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1230 "parse.y"
{const char*_tmpA52;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpA52="C include",_tag_dyneither(_tmpA52,sizeof(char),10))))!= 0){
const char*_tmpA53;Cyc_Parse_err(((_tmpA53="expecting \"C include\"",_tag_dyneither(_tmpA53,sizeof(char),22))),(unsigned int)(yyyylsp[0]).first_line);}}{
struct Cyc_List_List*exs=Cyc_yyget_YY55(yyyyvsp[5]);
{struct Cyc_Absyn_Decl*_tmpA5D;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct _tmpA5C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA5B;struct Cyc_List_List*_tmpA5A;yyval=Cyc_YY19(((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A->hd=((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D->r=(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=12,((_tmpA5C.f1=Cyc_yyget_YY19(yyyyvsp[3]),((_tmpA5C.f2=exs,_tmpA5C)))))),_tmpA5B)))),((_tmpA5D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA5D)))))),((_tmpA5A->tl=Cyc_yyget_YY19(yyyyvsp[6]),_tmpA5A)))))));}
# 1235
break;};}case 9: _LL230: {
# 1237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1236 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA60;struct Cyc_List_List*_tmpA5F;yyval=Cyc_YY19(((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F->hd=((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60->r=(void*)& Cyc_Absyn_Porton_d_val,((_tmpA60->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA60)))))),((_tmpA5F->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA5F)))))));}
break;}case 10: _LL231: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1238 "parse.y"
{struct Cyc_Absyn_Decl*_tmpA63;struct Cyc_List_List*_tmpA62;yyval=Cyc_YY19(((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62->hd=((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63->r=(void*)& Cyc_Absyn_Portoff_d_val,((_tmpA63->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpA63)))))),((_tmpA62->tl=Cyc_yyget_YY19(yyyyvsp[2]),_tmpA62)))))));}
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
{struct _tuple27*_tmpA66;struct Cyc_List_List*_tmpA65;yyval=Cyc_YY55(((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65->hd=((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA66->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA66->f3=0,_tmpA66)))))))),((_tmpA65->tl=0,_tmpA65)))))));}
break;}case 15: _LL236: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1249 "parse.y"
{struct _tuple27*_tmpA69;struct Cyc_List_List*_tmpA68;yyval=Cyc_YY55(((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68->hd=((_tmpA69=_cycalloc(sizeof(*_tmpA69)),((_tmpA69->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA69->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA69->f3=0,_tmpA69)))))))),((_tmpA68->tl=0,_tmpA68)))))));}
break;}case 16: _LL237: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1251 "parse.y"
{struct _tuple27*_tmpA6C;struct Cyc_List_List*_tmpA6B;yyval=Cyc_YY55(((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B->hd=((_tmpA6C=_cycalloc(sizeof(*_tmpA6C)),((_tmpA6C->f1=(unsigned int)(yyyylsp[0]).first_line,((_tmpA6C->f2=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpA6C->f3=0,_tmpA6C)))))))),((_tmpA6B->tl=Cyc_yyget_YY55(yyyyvsp[2]),_tmpA6B)))))));}
break;}case 17: _LL238: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1255 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA72;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpA71;struct Cyc_List_List*_tmpA70;yyval=Cyc_YY19(((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70->hd=Cyc_Absyn_new_decl((void*)((_tmpA72=_cycalloc(sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA71.tag=1,((_tmpA71.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpA71)))),_tmpA72)))),(unsigned int)(yyyylsp[0]).first_line),((_tmpA70->tl=0,_tmpA70)))))));}
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
{struct _dyneither_ptr*_tmpA73;Cyc_Lex_enter_namespace(((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpA73)))));}yyval=yyyyvsp[1];
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
{struct Cyc_List_List*_tmpA74;yyval=Cyc_YY19(((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74->hd=Cyc_Absyn_let_decl(Cyc_yyget_YY12(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line),((_tmpA74->tl=0,_tmpA74)))))));}
break;}case 35: _LL248: {
# 1324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1323 "parse.y"
struct Cyc_List_List*_tmp4AD=0;
{struct Cyc_List_List*_tmp4AE=Cyc_yyget_YY39(yyyyvsp[1]);for(0;_tmp4AE != 0;_tmp4AE=_tmp4AE->tl){
struct _dyneither_ptr*_tmp4AF=(struct _dyneither_ptr*)_tmp4AE->hd;
struct _tuple0*_tmpA75;struct _tuple0*qv=(_tmpA75=_cycalloc(sizeof(*_tmpA75)),((_tmpA75->f1=Cyc_Absyn_Rel_n(0),((_tmpA75->f2=_tmp4AF,_tmpA75)))));
struct Cyc_Absyn_Vardecl*_tmp4B0=Cyc_Absyn_new_vardecl(qv,Cyc_Absyn_wildtyp(0),0);
struct Cyc_List_List*_tmpA76;_tmp4AD=((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76->hd=_tmp4B0,((_tmpA76->tl=_tmp4AD,_tmpA76))))));}}
# 1330
_tmp4AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AD);
{struct Cyc_List_List*_tmpA77;yyval=Cyc_YY19(((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->hd=Cyc_Absyn_letv_decl(_tmp4AD,(unsigned int)(yyyylsp[0]).first_line),((_tmpA77->tl=0,_tmpA77)))))));}
# 1333
break;}case 36: _LL249: {
# 1335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 1337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 1336 "parse.y"
struct _dyneither_ptr _tmp4B4=Cyc_yyget_String_tok(yyyyvsp[3]);
# 1338
{const char*_tmpA78;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA78="`H",_tag_dyneither(_tmpA78,sizeof(char),3))))== 0){
const char*_tmpA7B;void*_tmpA7A;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA7A=0,Cyc_aprintf(((_tmpA7B="bad occurrence of heap region",_tag_dyneither(_tmpA7B,sizeof(char),30))),_tag_dyneither(_tmpA7A,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}
{const char*_tmpA7C;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4B4,((_tmpA7C="`U",_tag_dyneither(_tmpA7C,sizeof(char),3))))== 0){
const char*_tmpA7F;void*_tmpA7E;Cyc_Parse_err((struct _dyneither_ptr)((_tmpA7E=0,Cyc_aprintf(((_tmpA7F="bad occurrence of unique region",_tag_dyneither(_tmpA7F,sizeof(char),32))),_tag_dyneither(_tmpA7E,sizeof(void*),0)))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpA82;struct Cyc_Absyn_Tvar*_tmpA81;struct Cyc_Absyn_Tvar*tv=(_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->name=((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=_tmp4B4,_tmpA82)))),((_tmpA81->identity=- 1,((_tmpA81->kind=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA81)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpA85;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA84;void*t=(void*)((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84[0]=((_tmpA85.tag=2,((_tmpA85.f1=tv,_tmpA85)))),_tmpA84))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpA8B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA8A;struct _dyneither_ptr*_tmpA88;struct _tuple0*_tmpA87;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpA87=_cycalloc(sizeof(*_tmpA87)),((_tmpA87->f1=Cyc_Absyn_Loc_n,((_tmpA87->f2=((_tmpA88=_cycalloc(sizeof(*_tmpA88)),((_tmpA88[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpA88)))),_tmpA87)))))),(void*)((_tmpA8A=_cycalloc(sizeof(*_tmpA8A)),((_tmpA8A[0]=((_tmpA8B.tag=15,((_tmpA8B.f1=t,_tmpA8B)))),_tmpA8A)))),0);
{struct Cyc_List_List*_tmpA8C;yyval=Cyc_YY19(((_tmpA8C=_cycalloc(sizeof(*_tmpA8C)),((_tmpA8C->hd=Cyc_Absyn_region_decl(tv,vd,Cyc_yyget_YY34(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line),((_tmpA8C->tl=0,_tmpA8C)))))));}
# 1347
break;};}case 37: _LL24A: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1349 "parse.y"
int _tmp4C4=Cyc_yyget_YY34(yyyyvsp[0]);
struct _dyneither_ptr _tmp4C5=Cyc_yyget_String_tok(yyyyvsp[2]);
{const char*_tmpA8D;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C5,((_tmpA8D="H",_tag_dyneither(_tmpA8D,sizeof(char),2))))== 0){
const char*_tmpA8E;Cyc_Parse_err(((_tmpA8E="bad occurrence of heap region `H",_tag_dyneither(_tmpA8E,sizeof(char),33))),(unsigned int)(yyyylsp[2]).first_line);}}
{const char*_tmpA8F;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp4C5,((_tmpA8F="U",_tag_dyneither(_tmpA8F,sizeof(char),2))))== 0){
const char*_tmpA90;Cyc_Parse_err(((_tmpA90="bad occurrence of unique region `U",_tag_dyneither(_tmpA90,sizeof(char),35))),(unsigned int)(yyyylsp[2]).first_line);}}{
struct _dyneither_ptr*_tmpA9D;const char*_tmpA9C;void*_tmpA9B[1];struct Cyc_String_pa_PrintArg_struct _tmpA9A;struct Cyc_Absyn_Tvar*_tmpA99;struct Cyc_Absyn_Tvar*tv=(_tmpA99=_cycalloc(sizeof(*_tmpA99)),((_tmpA99->name=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=(struct _dyneither_ptr)((_tmpA9A.tag=0,((_tmpA9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4C5),((_tmpA9B[0]=& _tmpA9A,Cyc_aprintf(((_tmpA9C="`%s",_tag_dyneither(_tmpA9C,sizeof(char),4))),_tag_dyneither(_tmpA9B,sizeof(void*),1)))))))),_tmpA9D)))),((_tmpA99->identity=- 1,((_tmpA99->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpA99)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAA0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA9F;void*t=(void*)((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F[0]=((_tmpAA0.tag=2,((_tmpAA0.f1=tv,_tmpAA0)))),_tmpA9F))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpAA6;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpAA5;struct _dyneither_ptr*_tmpAA3;struct _tuple0*_tmpAA2;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->f1=Cyc_Absyn_Loc_n,((_tmpAA2->f2=((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=_tmp4C5,_tmpAA3)))),_tmpAA2)))))),(void*)((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5[0]=((_tmpAA6.tag=15,((_tmpAA6.f1=t,_tmpAA6)))),_tmpAA5)))),0);
# 1360
{struct Cyc_List_List*_tmpAA7;yyval=Cyc_YY19(((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->hd=Cyc_Absyn_region_decl(tv,vd,_tmp4C4,0,(unsigned int)(yyyylsp[0]).first_line),((_tmpAA7->tl=0,_tmpAA7)))))));}
# 1362
break;};}case 38: _LL24B: {
# 1364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1364 "parse.y"
struct _dyneither_ptr _tmp4D6=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _dyneither_ptr _tmp4D7=Cyc_yyget_String_tok(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4D8=Cyc_yyget_YY4(yyyyvsp[6]);
{const char*_tmpAA8;if(Cyc_strcmp((struct _dyneither_ptr)_tmp4D7,((_tmpAA8="open",_tag_dyneither(_tmpAA8,sizeof(char),5))))!= 0){const char*_tmpAA9;Cyc_Parse_err(((_tmpAA9="expecting `open'",_tag_dyneither(_tmpAA9,sizeof(char),17))),(unsigned int)(yyyylsp[3]).first_line);}}{
struct _dyneither_ptr*_tmpAB6;const char*_tmpAB5;void*_tmpAB4[1];struct Cyc_String_pa_PrintArg_struct _tmpAB3;struct Cyc_Absyn_Tvar*_tmpAB2;struct Cyc_Absyn_Tvar*tv=(_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->name=((_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6[0]=(struct _dyneither_ptr)((_tmpAB3.tag=0,((_tmpAB3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp4D6),((_tmpAB4[0]=& _tmpAB3,Cyc_aprintf(((_tmpAB5="`%s",_tag_dyneither(_tmpAB5,sizeof(char),4))),_tag_dyneither(_tmpAB4,sizeof(void*),1)))))))),_tmpAB6)))),((_tmpAB2->identity=- 1,((_tmpAB2->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmpAB2)))))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpAB9;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpAB8;void*t=(void*)((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8[0]=((_tmpAB9.tag=2,((_tmpAB9.f1=tv,_tmpAB9)))),_tmpAB8))));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpABF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpABE;struct _dyneither_ptr*_tmpABC;struct _tuple0*_tmpABB;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->f1=Cyc_Absyn_Loc_n,((_tmpABB->f2=((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=_tmp4D6,_tmpABC)))),_tmpABB)))))),(void*)((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABF.tag=15,((_tmpABF.f1=t,_tmpABF)))),_tmpABE)))),0);
# 1373
{struct Cyc_List_List*_tmpAC0;yyval=Cyc_YY19(((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->hd=Cyc_Absyn_region_decl(tv,vd,0,_tmp4D8,(unsigned int)(yyyylsp[0]).first_line),((_tmpAC0->tl=0,_tmpAC0)))))));}
# 1375
break;};}case 39: _LL24C:
# 1377
 yyval=Cyc_YY34(0);
break;case 40: _LL24D: {
# 1380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1379 "parse.y"
yyval=Cyc_YY34(1);
break;}case 41: _LL24E: {
# 1382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1384 "parse.y"
yyval=yyyyvsp[0];
break;}case 42: _LL24F: {
# 1387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1386 "parse.y"
yyval=Cyc_YY19(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY19(yyyyvsp[1])));
break;}case 43: _LL250: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Parse_Declaration_spec _tmpAC1;yyval=Cyc_YY20(((_tmpAC1.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAC1.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAC1.type_specs=
Cyc_Parse_empty_spec(0),((_tmpAC1.is_inline=0,((_tmpAC1.attributes=0,_tmpAC1)))))))))));}
break;}case 44: _LL251: {
# 1396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1395 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4E8=Cyc_yyget_YY20(yyyyvsp[1]);
if(_tmp4E8.sc != 0){
const char*_tmpAC4;void*_tmpAC3;(_tmpAC3=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpAC4="Only one storage class is allowed in a declaration",_tag_dyneither(_tmpAC4,sizeof(char),51))),_tag_dyneither(_tmpAC3,sizeof(void*),0)));}
# 1399
{struct Cyc_Parse_Declaration_spec _tmpAC5;yyval=Cyc_YY20(((_tmpAC5.sc=Cyc_yyget_YY23(yyyyvsp[0]),((_tmpAC5.tq=_tmp4E8.tq,((_tmpAC5.type_specs=_tmp4E8.type_specs,((_tmpAC5.is_inline=_tmp4E8.is_inline,((_tmpAC5.attributes=_tmp4E8.attributes,_tmpAC5)))))))))));}
# 1403
break;}case 45: _LL252: {
# 1405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1404 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAC6;yyval=Cyc_YY20(((_tmpAC6.sc=0,((_tmpAC6.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpAC6.type_specs=
Cyc_yyget_YY24(yyyyvsp[0]),((_tmpAC6.is_inline=0,((_tmpAC6.attributes=0,_tmpAC6)))))))))));}
break;}case 46: _LL253: {
# 1408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1407 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4ED=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAC7;yyval=Cyc_YY20(((_tmpAC7.sc=_tmp4ED.sc,((_tmpAC7.tq=_tmp4ED.tq,((_tmpAC7.type_specs=
Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,_tmp4ED.type_specs,Cyc_yyget_YY24(yyyyvsp[0])),((_tmpAC7.is_inline=_tmp4ED.is_inline,((_tmpAC7.attributes=_tmp4ED.attributes,_tmpAC7)))))))))));}
# 1413
break;}case 47: _LL254: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1414 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpAC8;yyval=Cyc_YY20(((_tmpAC8.sc=0,((_tmpAC8.tq=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpAC8.type_specs=Cyc_Parse_empty_spec(0),((_tmpAC8.is_inline=0,((_tmpAC8.attributes=0,_tmpAC8)))))))))));}
break;}case 48: _LL255: {
# 1417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1416 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F0=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpAC9;yyval=Cyc_YY20(((_tmpAC9.sc=_tmp4F0.sc,((_tmpAC9.tq=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp4F0.tq),((_tmpAC9.type_specs=_tmp4F0.type_specs,((_tmpAC9.is_inline=_tmp4F0.is_inline,((_tmpAC9.attributes=_tmp4F0.attributes,_tmpAC9)))))))))));}
# 1421
break;}case 49: _LL256: {
# 1423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1422 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpACA;yyval=Cyc_YY20(((_tmpACA.sc=0,((_tmpACA.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpACA.type_specs=
Cyc_Parse_empty_spec(0),((_tmpACA.is_inline=1,((_tmpACA.attributes=0,_tmpACA)))))))))));}
break;}case 50: _LL257: {
# 1426
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1428
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1425 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F3=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpACB;yyval=Cyc_YY20(((_tmpACB.sc=_tmp4F3.sc,((_tmpACB.tq=_tmp4F3.tq,((_tmpACB.type_specs=_tmp4F3.type_specs,((_tmpACB.is_inline=1,((_tmpACB.attributes=_tmp4F3.attributes,_tmpACB)))))))))));}
# 1429
break;}case 51: _LL258: {
# 1431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1430 "parse.y"
{struct Cyc_Parse_Declaration_spec _tmpACC;yyval=Cyc_YY20(((_tmpACC.sc=0,((_tmpACC.tq=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpACC.type_specs=
Cyc_Parse_empty_spec(0),((_tmpACC.is_inline=0,((_tmpACC.attributes=Cyc_yyget_YY48(yyyyvsp[0]),_tmpACC)))))))))));}
break;}case 52: _LL259: {
# 1434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1433 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp4F6=Cyc_yyget_YY20(yyyyvsp[1]);
{struct Cyc_Parse_Declaration_spec _tmpACD;yyval=Cyc_YY20(((_tmpACD.sc=_tmp4F6.sc,((_tmpACD.tq=_tmp4F6.tq,((_tmpACD.type_specs=_tmp4F6.type_specs,((_tmpACD.is_inline=_tmp4F6.is_inline,((_tmpACD.attributes=
# 1436
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp4F6.attributes),_tmpACD)))))))))));}
break;}case 53: _LL25A: {
# 1439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1440 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY23(& auto_sc);
break;}case 54: _LL25B: {
# 1444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1442 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY23(& register_sc);
break;}case 55: _LL25C: {
# 1446
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1448
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1444 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY23(& static_sc);
break;}case 56: _LL25D: {
# 1448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1446 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY23(& extern_sc);
break;}case 57: _LL25E: {
# 1450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1449 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
{const char*_tmpACE;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpACE="C",_tag_dyneither(_tmpACE,sizeof(char),2))))!= 0){
const char*_tmpACF;Cyc_Parse_err(((_tmpACF="only extern or extern \"C\" is allowed",_tag_dyneither(_tmpACF,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY23(& externC_sc);
# 1454
break;}case 58: _LL25F: {
# 1456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1454 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY23(& typedef_sc);
break;}case 59: _LL260: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1457 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY23(& abstract_sc);
break;}case 60: _LL261:
# 1461
 yyval=Cyc_YY48(0);
break;case 61: _LL262: {
# 1464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1464 "parse.y"
yyval=yyyyvsp[0];
break;}case 62: _LL263: {
# 1467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1469 "parse.y"
yyval=yyyyvsp[3];
break;}case 63: _LL264: {
# 1472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1473 "parse.y"
{struct Cyc_List_List*_tmpAD0;yyval=Cyc_YY48(((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAD0->tl=0,_tmpAD0)))))));}
break;}case 64: _LL265: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1474 "parse.y"
{struct Cyc_List_List*_tmpAD1;yyval=Cyc_YY48(((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->hd=Cyc_yyget_YY49(yyyyvsp[0]),((_tmpAD1->tl=Cyc_yyget_YY48(yyyyvsp[2]),_tmpAD1)))))));}
break;}case 65: _LL266: {
# 1477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1479 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6,- 1};
static struct _tuple28 att_map[17]={{{_tmp4FE,_tmp4FE,_tmp4FE + 8},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp4FF,_tmp4FF,_tmp4FF + 6},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp500,_tmp500,_tmp500 + 9},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp501,_tmp501,_tmp501 + 9},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp502,_tmp502,_tmp502 + 6},(void*)& Cyc_Absyn_Const_att_val},{{_tmp503,_tmp503,_tmp503 + 8},(void*)& att_aligned},{{_tmp504,_tmp504,_tmp504 + 7},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp505,_tmp505,_tmp505 + 7},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp506,_tmp506,_tmp506 + 7},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp507,_tmp507,_tmp507 + 5},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp508,_tmp508,_tmp508 + 10},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp509,_tmp509,_tmp509 + 10},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp50A,_tmp50A,_tmp50A + 23},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp50B,_tmp50B,_tmp50B + 12},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp50C,_tmp50C,_tmp50C + 11},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp50D,_tmp50D,_tmp50D + 22},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp50E,_tmp50E,_tmp50E + 5},(void*)& Cyc_Absyn_Pure_att_val}};
# 1499
struct _dyneither_ptr _tmp4FC=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1501
if((((_get_dyneither_size(_tmp4FC,sizeof(char))> 4  && ((const char*)_tmp4FC.curr)[0]== '_') && ((const char*)_tmp4FC.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp4FC,sizeof(char),(int)(_get_dyneither_size(_tmp4FC,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp4FC,sizeof(char),(int)(_get_dyneither_size(_tmp4FC,sizeof(char))- 3)))== '_')
# 1503
_tmp4FC=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp4FC,2,_get_dyneither_size(_tmp4FC,sizeof(char))- 5);{
int i=0;
for(0;i < 17;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp4FC,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(17,i)]).f1)== 0){
yyval=Cyc_YY49((att_map[_check_known_subscript_notnull(17,i)]).f2);
break;}}
# 1510
if(i == 17){
{const char*_tmpAD2;Cyc_Parse_err(((_tmpAD2="unrecognized attribute",_tag_dyneither(_tmpAD2,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
yyval=Cyc_YY49((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1515
break;};}case 66: _LL267: {
# 1517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1515 "parse.y"
yyval=Cyc_YY49((void*)& Cyc_Absyn_Const_att_val);
break;}case 67: _LL268: {
# 1518
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1520
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1517 "parse.y"
struct _dyneither_ptr _tmp510=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _tuple5 _tmp511=Cyc_yyget_Int_tok(yyyyvsp[2]);int _tmp513;struct _tuple5 _tmp512=_tmp511;_tmp513=_tmp512.f2;{
void*a;
{const char*_tmpAD4;const char*_tmpAD3;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAD4="regparm",_tag_dyneither(_tmpAD4,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAD3="__regparm__",_tag_dyneither(_tmpAD3,sizeof(char),12))))== 0){
if(_tmp513 < 0  || _tmp513 > 3){
const char*_tmpAD5;Cyc_Parse_err(((_tmpAD5="regparm requires value between 0 and 3",_tag_dyneither(_tmpAD5,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct _tmpAD8;struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmpAD7;a=(void*)((_tmpAD7=_cycalloc_atomic(sizeof(*_tmpAD7)),((_tmpAD7[0]=((_tmpAD8.tag=0,((_tmpAD8.f1=_tmp513,_tmpAD8)))),_tmpAD7))));};}else{
const char*_tmpADA;const char*_tmpAD9;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpADA="aligned",_tag_dyneither(_tmpADA,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAD9="__aligned__",_tag_dyneither(_tmpAD9,sizeof(char),12))))== 0){
if(_tmp513 < 0){const char*_tmpADB;Cyc_Parse_err(((_tmpADB="aligned requires positive power of two",_tag_dyneither(_tmpADB,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
unsigned int j=(unsigned int)_tmp513;
for(0;(j & 1)== 0;j=j >> 1){
;}
j=j >> 1;
if(j != 0){const char*_tmpADC;Cyc_Parse_err(((_tmpADC="aligned requires positive power of two",_tag_dyneither(_tmpADC,sizeof(char),39))),(unsigned int)(yyyylsp[2]).first_line);}{
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpADF;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpADE;a=(void*)((_tmpADE=_cycalloc_atomic(sizeof(*_tmpADE)),((_tmpADE[0]=((_tmpADF.tag=6,((_tmpADF.f1=_tmp513,_tmpADF)))),_tmpADE))));};};}else{
const char*_tmpAE1;const char*_tmpAE0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAE1="initializes",_tag_dyneither(_tmpAE1,sizeof(char),12))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAE0="__initializes__",_tag_dyneither(_tmpAE0,sizeof(char),16))))== 0){
struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct _tmpAE4;struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmpAE3;a=(void*)((_tmpAE3=_cycalloc_atomic(sizeof(*_tmpAE3)),((_tmpAE3[0]=((_tmpAE4.tag=20,((_tmpAE4.f1=_tmp513,_tmpAE4)))),_tmpAE3))));}else{
const char*_tmpAE6;const char*_tmpAE5;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAE6="noliveunique",_tag_dyneither(_tmpAE6,sizeof(char),13))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAE5="__noliveunique__",_tag_dyneither(_tmpAE5,sizeof(char),17))))== 0){
struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct _tmpAE9;struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmpAE8;a=(void*)((_tmpAE8=_cycalloc_atomic(sizeof(*_tmpAE8)),((_tmpAE8[0]=((_tmpAE9.tag=21,((_tmpAE9.f1=_tmp513,_tmpAE9)))),_tmpAE8))));}else{
const char*_tmpAEB;const char*_tmpAEA;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAEB="noconsume",_tag_dyneither(_tmpAEB,sizeof(char),10))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp510,((_tmpAEA="__noconsume__",_tag_dyneither(_tmpAEA,sizeof(char),14))))== 0){
struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct _tmpAEE;struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmpAED;a=(void*)((_tmpAED=_cycalloc_atomic(sizeof(*_tmpAED)),((_tmpAED[0]=((_tmpAEE.tag=22,((_tmpAEE.f1=_tmp513,_tmpAEE)))),_tmpAED))));}else{
# 1539
{const char*_tmpAEF;Cyc_Parse_err(((_tmpAEF="unrecognized attribute",_tag_dyneither(_tmpAEF,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}
# 1542
yyval=Cyc_YY49(a);
# 1544
break;};}case 68: _LL269: {
# 1546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1545 "parse.y"
struct _dyneither_ptr _tmp52C=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp52D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
{const char*_tmpAF1;const char*_tmpAF0;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpAF1="section",_tag_dyneither(_tmpAF1,sizeof(char),8))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp52C,((_tmpAF0="__section__",_tag_dyneither(_tmpAF0,sizeof(char),12))))== 0){
struct Cyc_Absyn_Section_att_Absyn_Attribute_struct _tmpAF4;struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmpAF3;a=(void*)((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=8,((_tmpAF4.f1=_tmp52D,_tmpAF4)))),_tmpAF3))));}else{
# 1551
{const char*_tmpAF5;Cyc_Parse_err(((_tmpAF5="unrecognized attribute",_tag_dyneither(_tmpAF5,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1554
yyval=Cyc_YY49(a);
# 1556
break;}case 69: _LL26A: {
# 1558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1557 "parse.y"
struct _dyneither_ptr _tmp533=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp534=Cyc_yyget_String_tok(yyyyvsp[2]);
void*a;
# 1561
{const char*_tmpAF6;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp533,((_tmpAF6="__mode__",_tag_dyneither(_tmpAF6,sizeof(char),9))))== 0){
struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct _tmpAF9;struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmpAF8;a=(void*)((_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8[0]=((_tmpAF9.tag=24,((_tmpAF9.f1=_tmp534,_tmpAF9)))),_tmpAF8))));}else{
# 1564
{const char*_tmpAFA;Cyc_Parse_err(((_tmpAFA="unrecognized attribute",_tag_dyneither(_tmpAFA,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}
# 1567
yyval=Cyc_YY49(a);
# 1569
break;}case 70: _LL26B: {
# 1571
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1573
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1570 "parse.y"
struct _dyneither_ptr _tmp539=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp53A=Cyc_yyget_String_tok(yyyyvsp[2]);
struct _tuple5 _tmp53B=Cyc_yyget_Int_tok(yyyyvsp[4]);int _tmp53D;struct _tuple5 _tmp53C=_tmp53B;_tmp53D=_tmp53C.f2;{
struct _tuple5 _tmp53E=Cyc_yyget_Int_tok(yyyyvsp[6]);int _tmp540;struct _tuple5 _tmp53F=_tmp53E;_tmp540=_tmp53F.f2;{
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
{const char*_tmpAFC;const char*_tmpAFB;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpAFC="format",_tag_dyneither(_tmpAFC,sizeof(char),7))))== 0  || Cyc_zstrcmp((struct _dyneither_ptr)_tmp539,((_tmpAFB="__format__",_tag_dyneither(_tmpAFB,sizeof(char),11))))== 0){
const char*_tmpAFD;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53A,((_tmpAFD="printf",_tag_dyneither(_tmpAFD,sizeof(char),7))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB00;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpAFF;a=(void*)((_tmpAFF=_cycalloc_atomic(sizeof(*_tmpAFF)),((_tmpAFF[0]=((_tmpB00.tag=19,((_tmpB00.f1=Cyc_Absyn_Printf_ft,((_tmpB00.f2=_tmp53D,((_tmpB00.f3=_tmp540,_tmpB00)))))))),_tmpAFF))));}else{
const char*_tmpB01;if(Cyc_zstrcmp((struct _dyneither_ptr)_tmp53A,((_tmpB01="scanf",_tag_dyneither(_tmpB01,sizeof(char),6))))== 0){
struct Cyc_Absyn_Format_att_Absyn_Attribute_struct _tmpB04;struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpB03;a=(void*)((_tmpB03=_cycalloc_atomic(sizeof(*_tmpB03)),((_tmpB03[0]=((_tmpB04.tag=19,((_tmpB04.f1=Cyc_Absyn_Scanf_ft,((_tmpB04.f2=_tmp53D,((_tmpB04.f3=_tmp540,_tmpB04)))))))),_tmpB03))));}else{
# 1581
const char*_tmpB05;Cyc_Parse_err(((_tmpB05="unrecognized format type",_tag_dyneither(_tmpB05,sizeof(char),25))),(unsigned int)(yyyylsp[2]).first_line);}}}else{
# 1583
const char*_tmpB06;Cyc_Parse_err(((_tmpB06="unrecognized attribute",_tag_dyneither(_tmpB06,sizeof(char),23))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY49(a);
# 1586
break;};};}case 71: _LL26C: {
# 1588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1595 "parse.y"
yyval=yyyyvsp[0];
break;}case 72: _LL26D: {
# 1598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1597 "parse.y"
{struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB09;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB08;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB08=_cycalloc(sizeof(*_tmpB08)),((_tmpB08[0]=((_tmpB09.tag=17,((_tmpB09.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB09.f2=Cyc_yyget_YY43(yyyyvsp[1]),((_tmpB09.f3=0,((_tmpB09.f4=0,_tmpB09)))))))))),_tmpB08)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 73: _LL26E: {
# 1600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1601 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 74: _LL26F: {
# 1604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1602 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(0,0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 75: _LL270: {
# 1605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1604 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY46(yyyyvsp[2])),0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 76: _LL271: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1605 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 77: _LL272: {
# 1608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1606 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 78: _LL273: {
# 1609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1607 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));
break;}case 79: _LL274: {
# 1610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1608 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 80: _LL275: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1609 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 81: _LL276: {
# 1612
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1614
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1610 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_Absyn_float_typ(1),(unsigned int)(yyyylsp[0]).first_line));
break;}case 82: _LL277: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1611 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 83: _LL278: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1612 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
break;}case 84: _LL279: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1613 "parse.y"
yyval=yyyyvsp[0];
break;}case 85: _LL27A: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1614 "parse.y"
yyval=yyyyvsp[0];
break;}case 86: _LL27B: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1616 "parse.y"
yyval=yyyyvsp[0];
break;}case 87: _LL27C: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1618 "parse.y"
yyval=Cyc_YY24(Cyc_Parse_type_spec(Cyc_yyget_YY47(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 88: _LL27D: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1621 "parse.y"
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB0C;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpB0B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B[0]=((_tmpB0C.tag=10,((_tmpB0C.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[2]).first_line,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[2]))),_tmpB0C)))),_tmpB0B)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1624
break;}case 89: _LL27E: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1625 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB0F;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB0E;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB0E=_cycalloc(sizeof(*_tmpB0E)),((_tmpB0E[0]=((_tmpB0F.tag=15,((_tmpB0F.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpB0F)))),_tmpB0E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 90: _LL27F: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1627 "parse.y"
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB12;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpB11;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11[0]=((_tmpB12.tag=15,((_tmpB12.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0),_tmpB12)))),_tmpB11)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1629
break;}case 91: _LL280: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1630 "parse.y"
void*_tmp553=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB15;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB14;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB14=_cycalloc(sizeof(*_tmpB14)),((_tmpB14[0]=((_tmpB15.tag=16,((_tmpB15.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpB15.f2=_tmp553,_tmpB15)))))),_tmpB14)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1633
break;}case 92: _LL281: {
# 1635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 1634 "parse.y"
{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB18;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpB17;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB17=_cycalloc(sizeof(*_tmpB17)),((_tmpB17[0]=((_tmpB18.tag=16,((_tmpB18.f1=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpB18.f2=Cyc_yyget_YY47(yyyyvsp[4]),_tmpB18)))))),_tmpB17)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 93: _LL282: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1636 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB1B;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB1A;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1A=_cycalloc(sizeof(*_tmpB1A)),((_tmpB1A[0]=((_tmpB1B.tag=19,((_tmpB1B.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpB1B)))),_tmpB1A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 94: _LL283: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1638 "parse.y"
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB1E;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpB1D;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D[0]=((_tmpB1E.tag=19,((_tmpB1E.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0),_tmpB1E)))),_tmpB1D)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 95: _LL284: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1640 "parse.y"
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB21;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB20;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB20=_cycalloc(sizeof(*_tmpB20)),((_tmpB20[0]=((_tmpB21.tag=18,((_tmpB21.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB21)))),_tmpB20)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 96: _LL285: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1645 "parse.y"
yyval=Cyc_YY46(Cyc_Parse_id_to_kind(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 97: _LL286: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1649 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0);
{struct Cyc_Absyn_Tqual _tmpB22;yyval=Cyc_YY26(((_tmpB22.print_const=1,((_tmpB22.q_volatile=0,((_tmpB22.q_restrict=0,((_tmpB22.real_const=1,((_tmpB22.loc=loc,_tmpB22)))))))))));}
break;}case 98: _LL287: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1651 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB23;yyval=Cyc_YY26(((_tmpB23.print_const=0,((_tmpB23.q_volatile=1,((_tmpB23.q_restrict=0,((_tmpB23.real_const=0,((_tmpB23.loc=0,_tmpB23)))))))))));}
break;}case 99: _LL288: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1652 "parse.y"
{struct Cyc_Absyn_Tqual _tmpB24;yyval=Cyc_YY26(((_tmpB24.print_const=0,((_tmpB24.q_volatile=0,((_tmpB24.q_restrict=1,((_tmpB24.real_const=0,((_tmpB24.loc=0,_tmpB24)))))))))));}
break;}case 100: _LL289: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB33;struct Cyc_Absyn_Enumdecl*_tmpB32;struct Cyc_Core_Opt*_tmpB31;struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct _tmpB30;struct Cyc_Absyn_TypeDecl*_tmpB2F;struct Cyc_Absyn_TypeDecl*_tmp561=(_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->r=(void*)((_tmpB33=_cycalloc(sizeof(*_tmpB33)),((_tmpB33[0]=((_tmpB30.tag=1,((_tmpB30.f1=((_tmpB32=_cycalloc(sizeof(*_tmpB32)),((_tmpB32->sc=Cyc_Absyn_Public,((_tmpB32->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB32->fields=((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31->v=Cyc_yyget_YY51(yyyyvsp[3]),_tmpB31)))),_tmpB32)))))))),_tmpB30)))),_tmpB33)))),((_tmpB2F->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB2F)))));
# 1660
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB36;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB35;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB35=_cycalloc(sizeof(*_tmpB35)),((_tmpB35[0]=((_tmpB36.tag=26,((_tmpB36.f1=_tmp561,((_tmpB36.f2=0,_tmpB36)))))),_tmpB35)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1662
break;}case 101: _LL28A: {
# 1664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1663 "parse.y"
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB39;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpB38;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38[0]=((_tmpB39.tag=13,((_tmpB39.f1=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB39.f2=0,_tmpB39)))))),_tmpB38)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 102: _LL28B: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1665 "parse.y"
{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB3C;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpB3B;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3C.tag=14,((_tmpB3C.f1=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB3C)))),_tmpB3B)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 103: _LL28C: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_Absyn_Enumfield*_tmpB3D;yyval=Cyc_YY50(((_tmpB3D=_cycalloc(sizeof(*_tmpB3D)),((_tmpB3D->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB3D->tag=0,((_tmpB3D->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB3D)))))))));}
break;}case 104: _LL28D: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1673 "parse.y"
{struct Cyc_Absyn_Enumfield*_tmpB3E;yyval=Cyc_YY50(((_tmpB3E=_cycalloc(sizeof(*_tmpB3E)),((_tmpB3E->name=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpB3E->tag=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB3E->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpB3E)))))))));}
break;}case 105: _LL28E: {
# 1676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1677 "parse.y"
{struct Cyc_List_List*_tmpB3F;yyval=Cyc_YY51(((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB3F->tl=0,_tmpB3F)))))));}
break;}case 106: _LL28F: {
# 1680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1678 "parse.y"
{struct Cyc_List_List*_tmpB40;yyval=Cyc_YY51(((_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB40->tl=0,_tmpB40)))))));}
break;}case 107: _LL290: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1679 "parse.y"
{struct Cyc_List_List*_tmpB41;yyval=Cyc_YY51(((_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41->hd=Cyc_yyget_YY50(yyyyvsp[0]),((_tmpB41->tl=Cyc_yyget_YY51(yyyyvsp[2]),_tmpB41)))))));}
break;}case 108: _LL291: {
# 1682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB44;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB43;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB43=_cycalloc(sizeof(*_tmpB43)),((_tmpB43[0]=((_tmpB44.tag=12,((_tmpB44.f1=Cyc_yyget_YY25(yyyyvsp[0]),((_tmpB44.f2=Cyc_yyget_YY27(yyyyvsp[2]),_tmpB44)))))),_tmpB43)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 109: _LL292: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 1691 "parse.y"
struct Cyc_List_List*_tmp574=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[3]).first_line,Cyc_yyget_YY43(yyyyvsp[3]));
struct Cyc_List_List*_tmp575=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[5]).first_line,Cyc_yyget_YY43(yyyyvsp[5]));
struct Cyc_Absyn_TypeDecl*_tmp576=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[2]),_tmp574,
Cyc_Absyn_aggrdecl_impl(_tmp575,Cyc_yyget_YY53(yyyyvsp[6]),Cyc_yyget_YY27(yyyyvsp[7]),1),0,(unsigned int)(yyyylsp[0]).first_line);
# 1696
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB47;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB46;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB46=_cycalloc(sizeof(*_tmpB46)),((_tmpB46[0]=((_tmpB47.tag=26,((_tmpB47.f1=_tmp576,((_tmpB47.f2=0,_tmpB47)))))),_tmpB46)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1698
break;}case 110: _LL293: {
# 1700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 1702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 1702 "parse.y"
struct Cyc_List_List*_tmp579=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_List_List*_tmp57A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[4]).first_line,Cyc_yyget_YY43(yyyyvsp[4]));
struct Cyc_Absyn_TypeDecl*_tmp57B=Cyc_Absyn_aggr_tdecl(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp579,
Cyc_Absyn_aggrdecl_impl(_tmp57A,Cyc_yyget_YY53(yyyyvsp[5]),Cyc_yyget_YY27(yyyyvsp[6]),0),0,(unsigned int)(yyyylsp[0]).first_line);
# 1707
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB4A;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB49;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=((_tmpB4A.tag=26,((_tmpB4A.f1=_tmp57B,((_tmpB4A.f2=0,_tmpB4A)))))),_tmpB49)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1709
break;}case 111: _LL294: {
# 1711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1710 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB54;struct Cyc_Core_Opt*_tmpB53;struct Cyc_Absyn_AggrInfo _tmpB52;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB51;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB51=_cycalloc(sizeof(*_tmpB51)),((_tmpB51[0]=((_tmpB54.tag=11,((_tmpB54.f1=((_tmpB52.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[1]),Cyc_yyget_QualId_tok(yyyyvsp[2]),((_tmpB53=_cycalloc_atomic(sizeof(*_tmpB53)),((_tmpB53->v=(void*)1,_tmpB53))))),((_tmpB52.targs=Cyc_yyget_YY43(yyyyvsp[3]),_tmpB52)))),_tmpB54)))),_tmpB51)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1713
break;}case 112: _LL295: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1714 "parse.y"
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB5A;struct Cyc_Absyn_AggrInfo _tmpB59;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpB58;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB58=_cycalloc(sizeof(*_tmpB58)),((_tmpB58[0]=((_tmpB5A.tag=11,((_tmpB5A.f1=((_tmpB59.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_yyget_YY25(yyyyvsp[0]),Cyc_yyget_QualId_tok(yyyyvsp[1]),0),((_tmpB59.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB59)))),_tmpB5A)))),_tmpB58)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1717
break;}case 113: _LL296:
# 1719
 yyval=Cyc_YY43(0);
break;case 114: _LL297: {
# 1722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1723 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[1])));
break;}case 115: _LL298: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1727 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_StructA);
break;}case 116: _LL299: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1728 "parse.y"
yyval=Cyc_YY25(Cyc_Absyn_UnionA);
break;}case 117: _LL29A:
# 1731
 yyval=Cyc_YY27(0);
break;case 118: _LL29B: {
# 1734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1735 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp585=Cyc_yyget_YY28(yyyyvsp[0]);for(0;_tmp585 != 0;_tmp585=_tmp585->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp585->hd,decls);}}{
# 1739
struct Cyc_List_List*_tmp586=Cyc_Parse_get_argrfield_tags(decls);
if(_tmp586 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp586,decls);
yyval=Cyc_YY27(decls);
# 1744
break;};}case 119: _LL29C: {
# 1746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB5B;yyval=Cyc_YY28(((_tmpB5B=_cycalloc(sizeof(*_tmpB5B)),((_tmpB5B->hd=Cyc_yyget_YY27(yyyyvsp[0]),((_tmpB5B->tl=0,_tmpB5B)))))));}
break;}case 120: _LL29D: {
# 1752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1751 "parse.y"
{struct Cyc_List_List*_tmpB5C;yyval=Cyc_YY28(((_tmpB5C=_cycalloc(sizeof(*_tmpB5C)),((_tmpB5C->hd=Cyc_yyget_YY27(yyyyvsp[1]),((_tmpB5C->tl=Cyc_yyget_YY28(yyyyvsp[0]),_tmpB5C)))))));}
break;}case 121: _LL29E: {
# 1754
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1756
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1755 "parse.y"
yyval=Cyc_YY22(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY22(yyyyvsp[0])));
break;}case 122: _LL29F: {
# 1758
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1760
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple12*_tmpB5D;yyval=Cyc_YY22(((_tmpB5D=_region_malloc(yyr,sizeof(*_tmpB5D)),((_tmpB5D->tl=0,((_tmpB5D->hd=Cyc_yyget_YY21(yyyyvsp[0]),_tmpB5D)))))));}
break;}case 123: _LL2A0: {
# 1764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1763 "parse.y"
{struct _tuple12*_tmpB5E;yyval=Cyc_YY22(((_tmpB5E=_region_malloc(yyr,sizeof(*_tmpB5E)),((_tmpB5E->tl=Cyc_yyget_YY22(yyyyvsp[0]),((_tmpB5E->hd=Cyc_yyget_YY21(yyyyvsp[2]),_tmpB5E)))))));}
break;}case 124: _LL2A1: {
# 1766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1768 "parse.y"
{struct _tuple11 _tmpB5F;yyval=Cyc_YY21(((_tmpB5F.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB5F.f2=0,_tmpB5F)))));}
break;}case 125: _LL2A2: {
# 1771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1770 "parse.y"
{struct _tuple11 _tmpB60;yyval=Cyc_YY21(((_tmpB60.f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB60.f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB60)))));}
break;}case 126: _LL2A3: {
# 1773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1776 "parse.y"
struct _RegionHandle _tmp58D=_new_region("temp");struct _RegionHandle*temp=& _tmp58D;_push_region(temp);{
struct _tuple25 _tmp58E=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Parse_Type_specifier _tmp591;struct Cyc_List_List*_tmp592;struct _tuple25 _tmp58F=_tmp58E;_tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_tmp592=_tmp58F.f3;
if(_tmp590.loc == 0)_tmp590.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp593=Cyc_yyget_YY32(yyyyvsp[1]);for(0;_tmp593 != 0;_tmp593=_tmp593->tl){
struct _tuple24*_tmp594=(struct _tuple24*)_tmp593->hd;struct Cyc_Parse_Declarator _tmp596;struct Cyc_Absyn_Exp*_tmp597;struct Cyc_Absyn_Exp*_tmp598;struct _tuple24*_tmp595=_tmp594;_tmp596=_tmp595->f1;_tmp597=_tmp595->f2;_tmp598=_tmp595->f3;
{struct _tuple10*_tmpB61;decls=((_tmpB61=_region_malloc(temp,sizeof(*_tmpB61)),((_tmpB61->tl=decls,((_tmpB61->hd=_tmp596,_tmpB61))))));}{
struct _tuple16*_tmpB64;struct Cyc_List_List*_tmpB63;widths_and_reqs=(
(_tmpB63=_region_malloc(temp,sizeof(*_tmpB63)),((_tmpB63->hd=((_tmpB64=_region_malloc(temp,sizeof(*_tmpB64)),((_tmpB64->f1=_tmp597,((_tmpB64->f2=_tmp598,_tmpB64)))))),((_tmpB63->tl=widths_and_reqs,_tmpB63))))));};}}
# 1787
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp59C=Cyc_Parse_speclist2typ(_tmp591,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,
Cyc_Parse_apply_tmss(temp,_tmp590,_tmp59C,decls,_tmp592),widths_and_reqs);
# 1793
yyval=Cyc_YY27(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp59D));};};}
# 1796
_npop_handler(0);break;
# 1776
;_pop_region(temp);}case 127: _LL2A4: {
# 1798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1804 "parse.y"
{struct _tuple25 _tmpB65;yyval=Cyc_YY38(((_tmpB65.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB65.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB65.f3=0,_tmpB65)))))));}
break;}case 128: _LL2A5: {
# 1807
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1809
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1806 "parse.y"
struct _tuple25 _tmp59F=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB66;yyval=Cyc_YY38(((_tmpB66.f1=_tmp59F.f1,((_tmpB66.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp59F.f2),((_tmpB66.f3=_tmp59F.f3,_tmpB66)))))));}
break;}case 129: _LL2A6: {
# 1809
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1811
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1808 "parse.y"
{struct _tuple25 _tmpB67;yyval=Cyc_YY38(((_tmpB67.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB67.f2=Cyc_Parse_empty_spec(0),((_tmpB67.f3=0,_tmpB67)))))));}
break;}case 130: _LL2A7: {
# 1811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1810 "parse.y"
struct _tuple25 _tmp5A2=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB68;yyval=Cyc_YY38(((_tmpB68.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5A2.f1),((_tmpB68.f2=_tmp5A2.f2,((_tmpB68.f3=_tmp5A2.f3,_tmpB68)))))));}
break;}case 131: _LL2A8: {
# 1814
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1816
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1813 "parse.y"
{struct _tuple25 _tmpB69;yyval=Cyc_YY38(((_tmpB69.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB69.f2=Cyc_Parse_empty_spec(0),((_tmpB69.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB69)))))));}
break;}case 132: _LL2A9: {
# 1816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1815 "parse.y"
struct _tuple25 _tmp5A5=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB6A;yyval=Cyc_YY38(((_tmpB6A.f1=_tmp5A5.f1,((_tmpB6A.f2=_tmp5A5.f2,((_tmpB6A.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp5A5.f3),_tmpB6A)))))));}
break;}case 133: _LL2AA: {
# 1818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple25 _tmpB6B;yyval=Cyc_YY38(((_tmpB6B.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB6B.f2=Cyc_yyget_YY24(yyyyvsp[0]),((_tmpB6B.f3=0,_tmpB6B)))))));}
break;}case 134: _LL2AB: {
# 1824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1823 "parse.y"
struct _tuple25 _tmp5A8=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB6C;yyval=Cyc_YY38(((_tmpB6C.f1=_tmp5A8.f1,((_tmpB6C.f2=Cyc_Parse_combine_specifiers((unsigned int)(yyyylsp[0]).first_line,Cyc_yyget_YY24(yyyyvsp[0]),_tmp5A8.f2),((_tmpB6C.f3=_tmp5A8.f3,_tmpB6C)))))));}
break;}case 135: _LL2AC: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1825 "parse.y"
{struct _tuple25 _tmpB6D;yyval=Cyc_YY38(((_tmpB6D.f1=Cyc_yyget_YY26(yyyyvsp[0]),((_tmpB6D.f2=Cyc_Parse_empty_spec(0),((_tmpB6D.f3=0,_tmpB6D)))))));}
break;}case 136: _LL2AD: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1827 "parse.y"
struct _tuple25 _tmp5AB=Cyc_yyget_YY38(yyyyvsp[1]);
{struct _tuple25 _tmpB6E;yyval=Cyc_YY38(((_tmpB6E.f1=Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),_tmp5AB.f1),((_tmpB6E.f2=_tmp5AB.f2,((_tmpB6E.f3=_tmp5AB.f3,_tmpB6E)))))));}
break;}case 137: _LL2AE: {
# 1831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1830 "parse.y"
{struct _tuple25 _tmpB6F;yyval=Cyc_YY38(((_tmpB6F.f1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line),((_tmpB6F.f2=Cyc_Parse_empty_spec(0),((_tmpB6F.f3=Cyc_yyget_YY48(yyyyvsp[0]),_tmpB6F)))))));}
break;}case 138: _LL2AF: {
# 1833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1832 "parse.y"
struct _tuple25 _tmp5AE=Cyc_yyget_YY38(yyyyvsp[1]);{struct _tuple25 _tmpB70;yyval=Cyc_YY38(((_tmpB70.f1=_tmp5AE.f1,((_tmpB70.f2=_tmp5AE.f2,((_tmpB70.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_yyget_YY48(yyyyvsp[0]),_tmp5AE.f3),_tmpB70)))))));}
break;}case 139: _LL2B0: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1836 "parse.y"
yyval=Cyc_YY32(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY32(yyyyvsp[0])));
break;}case 140: _LL2B1: {
# 1839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpB71;yyval=Cyc_YY32(((_tmpB71=_region_malloc(yyr,sizeof(*_tmpB71)),((_tmpB71->hd=Cyc_yyget_YY31(yyyyvsp[0]),((_tmpB71->tl=0,_tmpB71)))))));}
break;}case 141: _LL2B2: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1844 "parse.y"
{struct Cyc_List_List*_tmpB72;yyval=Cyc_YY32(((_tmpB72=_region_malloc(yyr,sizeof(*_tmpB72)),((_tmpB72->hd=Cyc_yyget_YY31(yyyyvsp[2]),((_tmpB72->tl=Cyc_yyget_YY32(yyyyvsp[0]),_tmpB72)))))));}
break;}case 142: _LL2B3: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1849 "parse.y"
{struct _tuple24*_tmpB73;yyval=Cyc_YY31(((_tmpB73=_region_malloc(yyr,sizeof(*_tmpB73)),((_tmpB73->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB73->f2=0,((_tmpB73->f3=0,_tmpB73)))))))));}
break;}case 143: _LL2B4: {
# 1852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1853 "parse.y"
{struct Cyc_Parse_Declarator _tmpB79;struct _tuple0*_tmpB78;struct _tuple24*_tmpB77;yyval=Cyc_YY31(((_tmpB77=_region_malloc(yyr,sizeof(*_tmpB77)),((_tmpB77->f1=((_tmpB79.id=((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78->f1=Cyc_Absyn_Rel_n(0),((_tmpB78->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB78)))))),((_tmpB79.tms=0,_tmpB79)))),((_tmpB77->f2=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpB77->f3=0,_tmpB77)))))))));}
# 1855
break;}case 144: _LL2B5:
# 1859
{struct Cyc_Parse_Declarator _tmpB7F;struct _tuple0*_tmpB7E;struct _tuple24*_tmpB7D;yyval=Cyc_YY31(((_tmpB7D=_region_malloc(yyr,sizeof(*_tmpB7D)),((_tmpB7D->f1=((_tmpB7F.id=((_tmpB7E=_cycalloc(sizeof(*_tmpB7E)),((_tmpB7E->f1=Cyc_Absyn_Rel_n(0),((_tmpB7E->f2=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"",sizeof(char),1),_tmpB7E)))))),((_tmpB7F.tms=0,_tmpB7F)))),((_tmpB7D->f2=0,((_tmpB7D->f3=0,_tmpB7D)))))))));}
# 1861
break;case 145: _LL2B6: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1861 "parse.y"
{struct _tuple24*_tmpB80;yyval=Cyc_YY31(((_tmpB80=_region_malloc(yyr,sizeof(*_tmpB80)),((_tmpB80->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB80->f2=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpB80->f3=0,_tmpB80)))))))));}
break;}case 146: _LL2B7: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1863 "parse.y"
{struct _tuple24*_tmpB81;yyval=Cyc_YY31(((_tmpB81=_region_malloc(yyr,sizeof(*_tmpB81)),((_tmpB81->f1=Cyc_yyget_YY30(yyyyvsp[0]),((_tmpB81->f2=0,((_tmpB81->f3=Cyc_yyget_YY4(yyyyvsp[2]),_tmpB81)))))))));}
break;}case 147: _LL2B8: {
# 1866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 1868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
int _tmp5BD=Cyc_yyget_YY34(yyyyvsp[0]);
struct Cyc_List_List*_tmp5BE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
struct Cyc_Core_Opt*_tmpB82;struct Cyc_Absyn_TypeDecl*_tmp5BF=Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,Cyc_yyget_QualId_tok(yyyyvsp[1]),_tmp5BE,((_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82->v=Cyc_yyget_YY37(yyyyvsp[4]),_tmpB82)))),_tmp5BD,(unsigned int)(yyyylsp[0]).first_line);
# 1873
{struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct _tmpB85;struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB84;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB84=_cycalloc(sizeof(*_tmpB84)),((_tmpB84[0]=((_tmpB85.tag=26,((_tmpB85.f1=_tmp5BF,((_tmpB85.f2=0,_tmpB85)))))),_tmpB84)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1875
break;}case 148: _LL2B9: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1876 "parse.y"
int _tmp5C3=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB8F;struct Cyc_Absyn_UnknownDatatypeInfo _tmpB8E;struct Cyc_Absyn_DatatypeInfo _tmpB8D;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB8C;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB8C=_cycalloc(sizeof(*_tmpB8C)),((_tmpB8C[0]=((_tmpB8F.tag=3,((_tmpB8F.f1=((_tmpB8D.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpB8E.name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB8E.is_extensible=_tmp5C3,_tmpB8E))))),((_tmpB8D.targs=Cyc_yyget_YY43(yyyyvsp[2]),_tmpB8D)))),_tmpB8F)))),_tmpB8C)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1879
break;}case 149: _LL2BA: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1880 "parse.y"
int _tmp5C8=Cyc_yyget_YY34(yyyyvsp[0]);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB99;struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmpB98;struct Cyc_Absyn_DatatypeFieldInfo _tmpB97;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpB96;yyval=Cyc_YY24(Cyc_Parse_type_spec((void*)((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96[0]=((_tmpB99.tag=4,((_tmpB99.f1=((_tmpB97.field_info=
Cyc_Absyn_UnknownDatatypefield(((_tmpB98.datatype_name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB98.field_name=Cyc_yyget_QualId_tok(yyyyvsp[3]),((_tmpB98.is_extensible=_tmp5C8,_tmpB98))))))),((_tmpB97.targs=Cyc_yyget_YY43(yyyyvsp[4]),_tmpB97)))),_tmpB99)))),_tmpB96)))),(unsigned int)(yyyylsp[0]).first_line));}
# 1885
break;}case 150: _LL2BB: {
# 1887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1888 "parse.y"
yyval=Cyc_YY34(0);
break;}case 151: _LL2BC: {
# 1891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1889 "parse.y"
yyval=Cyc_YY34(1);
break;}case 152: _LL2BD: {
# 1892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1893 "parse.y"
{struct Cyc_List_List*_tmpB9A;yyval=Cyc_YY37(((_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB9A->tl=0,_tmpB9A)))))));}
break;}case 153: _LL2BE: {
# 1896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1894 "parse.y"
{struct Cyc_List_List*_tmpB9B;yyval=Cyc_YY37(((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB9B->tl=0,_tmpB9B)))))));}
break;}case 154: _LL2BF: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1895 "parse.y"
{struct Cyc_List_List*_tmpB9C;yyval=Cyc_YY37(((_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB9C->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB9C)))))));}
break;}case 155: _LL2C0: {
# 1898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1896 "parse.y"
{struct Cyc_List_List*_tmpB9D;yyval=Cyc_YY37(((_tmpB9D=_cycalloc(sizeof(*_tmpB9D)),((_tmpB9D->hd=Cyc_yyget_YY36(yyyyvsp[0]),((_tmpB9D->tl=Cyc_yyget_YY37(yyyyvsp[2]),_tmpB9D)))))));}
break;}case 156: _LL2C1:
# 1899
 yyval=Cyc_YY35(Cyc_Absyn_Public);
break;case 157: _LL2C2: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1901 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Extern);
break;}case 158: _LL2C3: {
# 1904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1902 "parse.y"
yyval=Cyc_YY35(Cyc_Absyn_Static);
break;}case 159: _LL2C4: {
# 1905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1906 "parse.y"
{struct Cyc_Absyn_Datatypefield*_tmpB9E;yyval=Cyc_YY36(((_tmpB9E=_cycalloc(sizeof(*_tmpB9E)),((_tmpB9E->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB9E->typs=0,((_tmpB9E->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB9E->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB9E)))))))))));}
break;}case 160: _LL2C5: {
# 1909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1908 "parse.y"
struct Cyc_List_List*_tmp5D2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,(unsigned int)(yyyylsp[3]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[3])));
{struct Cyc_Absyn_Datatypefield*_tmpB9F;yyval=Cyc_YY36(((_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->name=Cyc_yyget_QualId_tok(yyyyvsp[1]),((_tmpB9F->typs=_tmp5D2,((_tmpB9F->loc=(unsigned int)(yyyylsp[0]).first_line,((_tmpB9F->sc=Cyc_yyget_YY35(yyyyvsp[0]),_tmpB9F)))))))))));}
break;}case 161: _LL2C6: {
# 1912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1914 "parse.y"
yyval=yyyyvsp[0];
break;}case 162: _LL2C7: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1916 "parse.y"
struct Cyc_Parse_Declarator _tmp5D4=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBA0;yyval=Cyc_YY30(((_tmpBA0.id=_tmp5D4.id,((_tmpBA0.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5D4.tms),_tmpBA0)))));}
break;}case 163: _LL2C8: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 164: _LL2C9: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1925 "parse.y"
struct Cyc_Parse_Declarator _tmp5D6=Cyc_yyget_YY30(yyyyvsp[1]);
{struct Cyc_Parse_Declarator _tmpBA1;yyval=Cyc_YY30(((_tmpBA1.id=_tmp5D6.id,((_tmpBA1.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),_tmp5D6.tms),_tmpBA1)))));}
break;}case 165: _LL2CA: {
# 1929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1931 "parse.y"
{struct Cyc_Parse_Declarator _tmpBA2;yyval=Cyc_YY30(((_tmpBA2.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpBA2.tms=0,_tmpBA2)))));}
break;}case 166: _LL2CB: {
# 1934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1933 "parse.y"
yyval=yyyyvsp[1];
break;}case 167: _LL2CC: {
# 1936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1937 "parse.y"
struct Cyc_Parse_Declarator _tmp5D9=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpBA8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpBA7;struct Cyc_List_List*_tmpBA6;_tmp5D9.tms=((_tmpBA6=_region_malloc(yyr,sizeof(*_tmpBA6)),((_tmpBA6->hd=(void*)((_tmpBA8=_region_malloc(yyr,sizeof(*_tmpBA8)),((_tmpBA8[0]=((_tmpBA7.tag=5,((_tmpBA7.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpBA7.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpBA7)))))),_tmpBA8)))),((_tmpBA6->tl=_tmp5D9.tms,_tmpBA6))))));}
yyval=yyyyvsp[2];
# 1941
break;}case 168: _LL2CD: {
# 1943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1942 "parse.y"
{struct Cyc_List_List*_tmpBB2;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpBB1;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpBB0;struct Cyc_Parse_Declarator _tmpBAF;yyval=Cyc_YY30(((_tmpBAF.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBAF.tms=((_tmpBB2=_region_malloc(yyr,sizeof(*_tmpBB2)),((_tmpBB2->hd=(void*)((_tmpBB0=_region_malloc(yyr,sizeof(*_tmpBB0)),((_tmpBB0[0]=((_tmpBB1.tag=0,((_tmpBB1.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpBB1.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpBB1)))))),_tmpBB0)))),((_tmpBB2->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBB2)))))),_tmpBAF)))));}
break;}case 169: _LL2CE: {
# 1945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1944 "parse.y"
{struct Cyc_List_List*_tmpBBC;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpBBB;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpBBA;struct Cyc_Parse_Declarator _tmpBB9;yyval=Cyc_YY30(((_tmpBB9.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBB9.tms=(
(_tmpBBC=_region_malloc(yyr,sizeof(*_tmpBBC)),((_tmpBBC->hd=(void*)((_tmpBBA=_region_malloc(yyr,sizeof(*_tmpBBA)),((_tmpBBA[0]=((_tmpBBB.tag=1,((_tmpBBB.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpBBB.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpBBB.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpBBB)))))))),_tmpBBA)))),((_tmpBBC->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBBC)))))),_tmpBB9)))));}
break;}case 170: _LL2CF: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1947 "parse.y"
struct _tuple26*_tmp5E5=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp5E7;int _tmp5E8;struct Cyc_Absyn_VarargInfo*_tmp5E9;void*_tmp5EA;struct Cyc_List_List*_tmp5EB;struct _tuple26*_tmp5E6=_tmp5E5;_tmp5E7=_tmp5E6->f1;_tmp5E8=_tmp5E6->f2;_tmp5E9=_tmp5E6->f3;_tmp5EA=_tmp5E6->f4;_tmp5EB=_tmp5E6->f5;
{struct Cyc_List_List*_tmpBD1;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBD0;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBCF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBCE;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBCD;struct Cyc_Parse_Declarator _tmpBCC;yyval=Cyc_YY30(((_tmpBCC.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBCC.tms=((_tmpBD1=_region_malloc(yyr,sizeof(*_tmpBD1)),((_tmpBD1->hd=(void*)((_tmpBCD=_region_malloc(yyr,sizeof(*_tmpBCD)),((_tmpBCD[0]=((_tmpBD0.tag=3,((_tmpBD0.f1=(void*)((_tmpBCE=_region_malloc(yyr,sizeof(*_tmpBCE)),((_tmpBCE[0]=((_tmpBCF.tag=1,((_tmpBCF.f1=_tmp5E7,((_tmpBCF.f2=_tmp5E8,((_tmpBCF.f3=_tmp5E9,((_tmpBCF.f4=_tmp5EA,((_tmpBCF.f5=_tmp5EB,_tmpBCF)))))))))))),_tmpBCE)))),_tmpBD0)))),_tmpBCD)))),((_tmpBD1->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBD1)))))),_tmpBCC)))));}
# 1950
break;}case 171: _LL2D0: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1951 "parse.y"
{struct Cyc_List_List*_tmpBE6;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBE5;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpBE4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpBE3;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBE2;struct Cyc_Parse_Declarator _tmpBE1;yyval=Cyc_YY30(((_tmpBE1.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBE1.tms=(
(_tmpBE6=_region_malloc(yyr,sizeof(*_tmpBE6)),((_tmpBE6->hd=(void*)((_tmpBE2=_region_malloc(yyr,sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE5.tag=3,((_tmpBE5.f1=(void*)((_tmpBE3=_region_malloc(yyr,sizeof(*_tmpBE3)),((_tmpBE3[0]=((_tmpBE4.tag=1,((_tmpBE4.f1=0,((_tmpBE4.f2=0,((_tmpBE4.f3=0,((_tmpBE4.f4=
# 1954
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpBE4.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpBE4)))))))))))),_tmpBE3)))),_tmpBE5)))),_tmpBE2)))),((_tmpBE6->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBE6)))))),_tmpBE1)))));}
# 1957
break;}case 172: _LL2D1: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1958 "parse.y"
{struct Cyc_List_List*_tmpBFB;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpBFA;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpBF9;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpBF8;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpBF7;struct Cyc_Parse_Declarator _tmpBF6;yyval=Cyc_YY30(((_tmpBF6.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpBF6.tms=((_tmpBFB=_region_malloc(yyr,sizeof(*_tmpBFB)),((_tmpBFB->hd=(void*)((_tmpBF7=_region_malloc(yyr,sizeof(*_tmpBF7)),((_tmpBF7[0]=((_tmpBFA.tag=3,((_tmpBFA.f1=(void*)((_tmpBF8=_region_malloc(yyr,sizeof(*_tmpBF8)),((_tmpBF8[0]=((_tmpBF9.tag=0,((_tmpBF9.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpBF9.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpBF9)))))),_tmpBF8)))),_tmpBFA)))),_tmpBF7)))),((_tmpBFB->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpBFB)))))),_tmpBF6)))));}
break;}case 173: _LL2D2: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1961 "parse.y"
struct Cyc_List_List*_tmp5FE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpC05;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC04;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC03;struct Cyc_Parse_Declarator _tmpC02;yyval=Cyc_YY30(((_tmpC02.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpC02.tms=((_tmpC05=_region_malloc(yyr,sizeof(*_tmpC05)),((_tmpC05->hd=(void*)((_tmpC03=_region_malloc(yyr,sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=4,((_tmpC04.f1=_tmp5FE,((_tmpC04.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC04.f3=0,_tmpC04)))))))),_tmpC03)))),((_tmpC05->tl=(Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpC05)))))),_tmpC02)))));}
# 1964
break;}case 174: _LL2D3: {
# 1966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 1968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 1965 "parse.y"
{struct Cyc_List_List*_tmpC0F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC0E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC0D;struct Cyc_Parse_Declarator _tmpC0C;yyval=Cyc_YY30(((_tmpC0C.id=(Cyc_yyget_YY30(yyyyvsp[0])).id,((_tmpC0C.tms=((_tmpC0F=_region_malloc(yyr,sizeof(*_tmpC0F)),((_tmpC0F->hd=(void*)((_tmpC0D=_region_malloc(yyr,sizeof(*_tmpC0D)),((_tmpC0D[0]=((_tmpC0E.tag=5,((_tmpC0E.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC0E.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC0E)))))),_tmpC0D)))),((_tmpC0F->tl=(
Cyc_yyget_YY30(yyyyvsp[0])).tms,_tmpC0F)))))),_tmpC0C)))));}
# 1968
break;}case 175: _LL2D4: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1979 "parse.y"
{struct Cyc_Parse_Declarator _tmpC10;yyval=Cyc_YY30(((_tmpC10.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC10.tms=0,_tmpC10)))));}
break;}case 176: _LL2D5: {
# 1982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 1984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 1981 "parse.y"
{struct Cyc_Parse_Declarator _tmpC11;yyval=Cyc_YY30(((_tmpC11.id=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpC11.tms=0,_tmpC11)))));}
break;}case 177: _LL2D6: {
# 1984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 1986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 1983 "parse.y"
yyval=yyyyvsp[1];
break;}case 178: _LL2D7: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1987 "parse.y"
struct Cyc_Parse_Declarator _tmp609=Cyc_yyget_YY30(yyyyvsp[2]);
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC17;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC16;struct Cyc_List_List*_tmpC15;_tmp609.tms=((_tmpC15=_region_malloc(yyr,sizeof(*_tmpC15)),((_tmpC15->hd=(void*)((_tmpC17=_region_malloc(yyr,sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC16.tag=5,((_tmpC16.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC16.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC16)))))),_tmpC17)))),((_tmpC15->tl=_tmp609.tms,_tmpC15))))));}
yyval=yyyyvsp[2];
# 1991
break;}case 179: _LL2D8: {
# 1993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 1995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 1992 "parse.y"
struct Cyc_Parse_Declarator _tmp60D=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC21;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpC20;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpC1F;struct Cyc_Parse_Declarator _tmpC1E;yyval=Cyc_YY30(((_tmpC1E.id=_tmp60D.id,((_tmpC1E.tms=(
(_tmpC21=_region_malloc(yyr,sizeof(*_tmpC21)),((_tmpC21->hd=(void*)((_tmpC1F=_region_malloc(yyr,sizeof(*_tmpC1F)),((_tmpC1F[0]=((_tmpC20.tag=0,((_tmpC20.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpC20.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpC20)))))),_tmpC1F)))),((_tmpC21->tl=_tmp60D.tms,_tmpC21)))))),_tmpC1E)))));}
break;}case 180: _LL2D9: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 1996 "parse.y"
struct Cyc_Parse_Declarator _tmp612=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC2B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpC2A;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpC29;struct Cyc_Parse_Declarator _tmpC28;yyval=Cyc_YY30(((_tmpC28.id=_tmp612.id,((_tmpC28.tms=(
(_tmpC2B=_region_malloc(yyr,sizeof(*_tmpC2B)),((_tmpC2B->hd=(void*)((_tmpC29=_region_malloc(yyr,sizeof(*_tmpC29)),((_tmpC29[0]=((_tmpC2A.tag=1,((_tmpC2A.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpC2A.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpC2A.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpC2A)))))))),_tmpC29)))),((_tmpC2B->tl=_tmp612.tms,_tmpC2B)))))),_tmpC28)))));}
# 2000
break;}case 181: _LL2DA: {
# 2002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2001 "parse.y"
struct _tuple26*_tmp617=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp619;int _tmp61A;struct Cyc_Absyn_VarargInfo*_tmp61B;void*_tmp61C;struct Cyc_List_List*_tmp61D;struct _tuple26*_tmp618=_tmp617;_tmp619=_tmp618->f1;_tmp61A=_tmp618->f2;_tmp61B=_tmp618->f3;_tmp61C=_tmp618->f4;_tmp61D=_tmp618->f5;{
struct Cyc_Parse_Declarator _tmp61E=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC40;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC3F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC3E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC3D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC3C;struct Cyc_Parse_Declarator _tmpC3B;yyval=Cyc_YY30(((_tmpC3B.id=_tmp61E.id,((_tmpC3B.tms=((_tmpC40=_region_malloc(yyr,sizeof(*_tmpC40)),((_tmpC40->hd=(void*)((_tmpC3C=_region_malloc(yyr,sizeof(*_tmpC3C)),((_tmpC3C[0]=((_tmpC3F.tag=3,((_tmpC3F.f1=(void*)((_tmpC3D=_region_malloc(yyr,sizeof(*_tmpC3D)),((_tmpC3D[0]=((_tmpC3E.tag=1,((_tmpC3E.f1=_tmp619,((_tmpC3E.f2=_tmp61A,((_tmpC3E.f3=_tmp61B,((_tmpC3E.f4=_tmp61C,((_tmpC3E.f5=_tmp61D,_tmpC3E)))))))))))),_tmpC3D)))),_tmpC3F)))),_tmpC3C)))),((_tmpC40->tl=_tmp61E.tms,_tmpC40)))))),_tmpC3B)))));}
# 2005
break;};}case 182: _LL2DB: {
# 2007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2006 "parse.y"
struct Cyc_Parse_Declarator _tmp625=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC55;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC54;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpC53;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpC52;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC51;struct Cyc_Parse_Declarator _tmpC50;yyval=Cyc_YY30(((_tmpC50.id=_tmp625.id,((_tmpC50.tms=(
(_tmpC55=_region_malloc(yyr,sizeof(*_tmpC55)),((_tmpC55->hd=(void*)((_tmpC51=_region_malloc(yyr,sizeof(*_tmpC51)),((_tmpC51[0]=((_tmpC54.tag=3,((_tmpC54.f1=(void*)((_tmpC52=_region_malloc(yyr,sizeof(*_tmpC52)),((_tmpC52[0]=((_tmpC53.tag=1,((_tmpC53.f1=0,((_tmpC53.f2=0,((_tmpC53.f3=0,((_tmpC53.f4=
# 2010
Cyc_yyget_YY52(yyyyvsp[2]),((_tmpC53.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpC53)))))))))))),_tmpC52)))),_tmpC54)))),_tmpC51)))),((_tmpC55->tl=_tmp625.tms,_tmpC55)))))),_tmpC50)))));}
# 2013
break;}case 183: _LL2DC: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp62C=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC6A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpC69;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct _tmpC68;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmpC67;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpC66;struct Cyc_Parse_Declarator _tmpC65;yyval=Cyc_YY30(((_tmpC65.id=_tmp62C.id,((_tmpC65.tms=((_tmpC6A=_region_malloc(yyr,sizeof(*_tmpC6A)),((_tmpC6A->hd=(void*)((_tmpC66=_region_malloc(yyr,sizeof(*_tmpC66)),((_tmpC66[0]=((_tmpC69.tag=3,((_tmpC69.f1=(void*)((_tmpC67=_region_malloc(yyr,sizeof(*_tmpC67)),((_tmpC67[0]=((_tmpC68.tag=0,((_tmpC68.f1=Cyc_yyget_YY39(yyyyvsp[2]),((_tmpC68.f2=(unsigned int)(yyyylsp[0]).first_line,_tmpC68)))))),_tmpC67)))),_tmpC69)))),_tmpC66)))),((_tmpC6A->tl=_tmp62C.tms,_tmpC6A)))))),_tmpC65)))));}
break;}case 184: _LL2DD: {
# 2018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2018 "parse.y"
struct Cyc_List_List*_tmp633=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
struct Cyc_Parse_Declarator _tmp634=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC74;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpC73;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpC72;struct Cyc_Parse_Declarator _tmpC71;yyval=Cyc_YY30(((_tmpC71.id=_tmp634.id,((_tmpC71.tms=((_tmpC74=_region_malloc(yyr,sizeof(*_tmpC74)),((_tmpC74->hd=(void*)((_tmpC72=_region_malloc(yyr,sizeof(*_tmpC72)),((_tmpC72[0]=((_tmpC73.tag=4,((_tmpC73.f1=_tmp633,((_tmpC73.f2=(unsigned int)(yyyylsp[0]).first_line,((_tmpC73.f3=0,_tmpC73)))))))),_tmpC72)))),((_tmpC74->tl=_tmp634.tms,_tmpC74)))))),_tmpC71)))));}
# 2022
break;}case 185: _LL2DE: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2023 "parse.y"
struct Cyc_Parse_Declarator _tmp639=Cyc_yyget_YY30(yyyyvsp[0]);
{struct Cyc_List_List*_tmpC7E;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC7D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC7C;struct Cyc_Parse_Declarator _tmpC7B;yyval=Cyc_YY30(((_tmpC7B.id=_tmp639.id,((_tmpC7B.tms=((_tmpC7E=_region_malloc(yyr,sizeof(*_tmpC7E)),((_tmpC7E->hd=(void*)((_tmpC7C=_region_malloc(yyr,sizeof(*_tmpC7C)),((_tmpC7C[0]=((_tmpC7D.tag=5,((_tmpC7D.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpC7D.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpC7D)))))),_tmpC7C)))),((_tmpC7E->tl=_tmp639.tms,_tmpC7E)))))),_tmpC7B)))));}
# 2026
break;}case 186: _LL2DF: {
# 2028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2036 "parse.y"
yyval=yyyyvsp[0];
break;}case 187: _LL2E0: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2037 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),Cyc_yyget_YY29(yyyyvsp[1])));
break;}case 188: _LL2E1: {
# 2040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2041 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY48(yyyyvsp[3])!= 0){
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC84;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpC83;struct Cyc_List_List*_tmpC82;ans=((_tmpC82=_region_malloc(yyr,sizeof(*_tmpC82)),((_tmpC82->hd=(void*)((_tmpC84=_region_malloc(yyr,sizeof(*_tmpC84)),((_tmpC84[0]=((_tmpC83.tag=5,((_tmpC83.f1=(unsigned int)(yyyylsp[3]).first_line,((_tmpC83.f2=Cyc_yyget_YY48(yyyyvsp[3]),_tmpC83)))))),_tmpC84)))),((_tmpC82->tl=ans,_tmpC82))))));}{
# 2045
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp641=*Cyc_yyget_YY1(yyyyvsp[0]);unsigned int _tmp643;union Cyc_Absyn_Constraint*_tmp644;union Cyc_Absyn_Constraint*_tmp645;struct _tuple21 _tmp642=_tmp641;_tmp643=_tmp642.f1;_tmp644=_tmp642.f2;_tmp645=_tmp642.f3;
if(Cyc_Absyn_porting_c_code){
struct Cyc_Absyn_PtrLoc*_tmpC85;ptrloc=((_tmpC85=_cycalloc_atomic(sizeof(*_tmpC85)),((_tmpC85->ptr_loc=_tmp643,((_tmpC85->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,((_tmpC85->zt_loc=(unsigned int)(yyyylsp[1]).first_line,_tmpC85))))))));}{
# 2050
struct _tuple14 _tmp647=Cyc_Parse_collapse_pointer_quals(_tmp643,_tmp644,_tmp645,Cyc_yyget_YY47(yyyyvsp[2]),Cyc_yyget_YY57(yyyyvsp[1]));union Cyc_Absyn_Constraint*_tmp649;union Cyc_Absyn_Constraint*_tmp64A;union Cyc_Absyn_Constraint*_tmp64B;void*_tmp64C;struct _tuple14 _tmp648=_tmp647;_tmp649=_tmp648.f1;_tmp64A=_tmp648.f2;_tmp64B=_tmp648.f3;_tmp64C=_tmp648.f4;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC8F;struct Cyc_Absyn_PtrAtts _tmpC8E;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpC8D;struct Cyc_List_List*_tmpC8C;ans=((_tmpC8C=_region_malloc(yyr,sizeof(*_tmpC8C)),((_tmpC8C->hd=(void*)((_tmpC8F=_region_malloc(yyr,sizeof(*_tmpC8F)),((_tmpC8F[0]=((_tmpC8D.tag=2,((_tmpC8D.f1=((_tmpC8E.rgn=_tmp64C,((_tmpC8E.nullable=_tmp649,((_tmpC8E.bounds=_tmp64A,((_tmpC8E.zero_term=_tmp64B,((_tmpC8E.ptrloc=ptrloc,_tmpC8E)))))))))),((_tmpC8D.f2=Cyc_yyget_YY26(yyyyvsp[4]),_tmpC8D)))))),_tmpC8F)))),((_tmpC8C->tl=ans,_tmpC8C))))));}
yyval=Cyc_YY29(ans);
# 2054
break;};};}case 189: _LL2E2:
# 2056
 yyval=Cyc_YY57(0);
break;case 190: _LL2E3: {
# 2059
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2061
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2057 "parse.y"
{struct Cyc_List_List*_tmpC90;yyval=Cyc_YY57(((_tmpC90=_region_malloc(yyr,sizeof(*_tmpC90)),((_tmpC90->hd=Cyc_yyget_YY56(yyyyvsp[0]),((_tmpC90->tl=Cyc_yyget_YY57(yyyyvsp[1]),_tmpC90)))))));}
break;}case 191: _LL2E4: {
# 2060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2062 "parse.y"
{struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct _tmpC93;struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct*_tmpC92;yyval=Cyc_YY56((void*)((_tmpC92=_region_malloc(yyr,sizeof(*_tmpC92)),((_tmpC92[0]=((_tmpC93.tag=0,((_tmpC93.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpC93)))),_tmpC92)))));}
break;}case 192: _LL2E5: {
# 2065
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2067
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2064 "parse.y"
{struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct _tmpC96;struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct*_tmpC95;yyval=Cyc_YY56((void*)((_tmpC95=_region_malloc(yyr,sizeof(*_tmpC95)),((_tmpC95[0]=((_tmpC96.tag=1,((_tmpC96.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpC96)))),_tmpC95)))));}
break;}case 193: _LL2E6: {
# 2067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2066 "parse.y"
{struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct _tmpC99;struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct*_tmpC98;yyval=Cyc_YY56((void*)((_tmpC98=_region_malloc(yyr,sizeof(*_tmpC98)),((_tmpC98[0]=((_tmpC99.tag=2,_tmpC99)),_tmpC98)))));}
break;}case 194: _LL2E7: {
# 2069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2068 "parse.y"
{struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct _tmpC9C;struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct*_tmpC9B;yyval=Cyc_YY56((void*)((_tmpC9B=_region_malloc(yyr,sizeof(*_tmpC9B)),((_tmpC9B[0]=((_tmpC9C.tag=3,_tmpC9C)),_tmpC9B)))));}
break;}case 195: _LL2E8: {
# 2071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2070 "parse.y"
{struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpC9F;struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpC9E;yyval=Cyc_YY56((void*)((_tmpC9E=_region_malloc(yyr,sizeof(*_tmpC9E)),((_tmpC9E[0]=((_tmpC9F.tag=4,_tmpC9F)),_tmpC9E)))));}
break;}case 196: _LL2E9: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2072 "parse.y"
{struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct _tmpCA2;struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct*_tmpCA1;yyval=Cyc_YY56((void*)((_tmpCA1=_region_malloc(yyr,sizeof(*_tmpCA1)),((_tmpCA1[0]=((_tmpCA2.tag=5,_tmpCA2)),_tmpCA1)))));}
break;}case 197: _LL2EA: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2074 "parse.y"
{struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct _tmpCA5;struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct*_tmpCA4;yyval=Cyc_YY56((void*)((_tmpCA4=_region_malloc(yyr,sizeof(*_tmpCA4)),((_tmpCA4[0]=((_tmpCA5.tag=6,_tmpCA5)),_tmpCA4)))));}
break;}case 198: _LL2EB: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2076 "parse.y"
{struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct _tmpCA8;struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct*_tmpCA7;yyval=Cyc_YY56((void*)((_tmpCA7=_region_malloc(yyr,sizeof(*_tmpCA7)),((_tmpCA7[0]=((_tmpCA8.tag=7,_tmpCA8)),_tmpCA7)))));}
break;}case 199: _LL2EC: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2082 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCA9;yyval=Cyc_YY1(((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9->f1=loc,((_tmpCA9->f2=Cyc_Absyn_true_conref,((_tmpCA9->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCA9)))))))));}
# 2085
break;}case 200: _LL2ED: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2086 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCAA;yyval=Cyc_YY1(((_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->f1=loc,((_tmpCAA->f2=Cyc_Absyn_false_conref,((_tmpCAA->f3=Cyc_yyget_YY2(yyyyvsp[1]),_tmpCAA)))))))));}
# 2089
break;}case 201: _LL2EE: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2090 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
{struct _tuple21*_tmpCAB;yyval=Cyc_YY1(((_tmpCAB=_cycalloc(sizeof(*_tmpCAB)),((_tmpCAB->f1=loc,((_tmpCAB->f2=Cyc_Absyn_true_conref,((_tmpCAB->f3=Cyc_Absyn_bounds_dyneither_conref,_tmpCAB)))))))));}
# 2093
break;}case 202: _LL2EF:
# 2095
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one_conref);
break;case 203: _LL2F0: {
# 2098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2096 "parse.y"
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpCAE;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpCAD;yyval=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmpCAD=_cycalloc(sizeof(*_tmpCAD)),((_tmpCAD[0]=((_tmpCAE.tag=1,((_tmpCAE.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpCAE)))),_tmpCAD))))));}
break;}case 204: _LL2F1:
# 2099
 yyval=Cyc_YY54(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());
break;case 205: _LL2F2: {
# 2102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2100 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_true_conref);
break;}case 206: _LL2F3: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2101 "parse.y"
yyval=Cyc_YY54(Cyc_Absyn_false_conref);
break;}case 207: _LL2F4:
# 2104
 yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;case 208: _LL2F5: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2107 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
break;}case 209: _LL2F6: {
# 2110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2108 "parse.y"
yyval=Cyc_YY47(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
break;}case 210: _LL2F7:
# 2111
 yyval=Cyc_YY26(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
break;case 211: _LL2F8: {
# 2114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2113 "parse.y"
yyval=Cyc_YY26(Cyc_Absyn_combine_tqual(Cyc_yyget_YY26(yyyyvsp[0]),Cyc_yyget_YY26(yyyyvsp[1])));
break;}case 212: _LL2F9: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2118 "parse.y"
{struct _tuple26*_tmpCAF;yyval=Cyc_YY42(((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpCAF->f2=0,((_tmpCAF->f3=0,((_tmpCAF->f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpCAF->f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpCAF)))))))))))));}
break;}case 213: _LL2FA: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2120 "parse.y"
{struct _tuple26*_tmpCB0;yyval=Cyc_YY42(((_tmpCB0=_cycalloc(sizeof(*_tmpCB0)),((_tmpCB0->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpCB0->f2=1,((_tmpCB0->f3=0,((_tmpCB0->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpCB0->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCB0)))))))))))));}
break;}case 214: _LL2FB: {
# 2123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2123 "parse.y"
struct _tuple8*_tmp669=Cyc_yyget_YY40(yyyyvsp[2]);struct _dyneither_ptr*_tmp66B;struct Cyc_Absyn_Tqual _tmp66C;void*_tmp66D;struct _tuple8*_tmp66A=_tmp669;_tmp66B=_tmp66A->f1;_tmp66C=_tmp66A->f2;_tmp66D=_tmp66A->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCB1;struct Cyc_Absyn_VarargInfo*_tmp66E=(_tmpCB1=_cycalloc(sizeof(*_tmpCB1)),((_tmpCB1->name=_tmp66B,((_tmpCB1->tq=_tmp66C,((_tmpCB1->type=_tmp66D,((_tmpCB1->inject=Cyc_yyget_YY34(yyyyvsp[1]),_tmpCB1)))))))));
{struct _tuple26*_tmpCB2;yyval=Cyc_YY42(((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2->f1=0,((_tmpCB2->f2=0,((_tmpCB2->f3=_tmp66E,((_tmpCB2->f4=Cyc_yyget_YY52(yyyyvsp[3]),((_tmpCB2->f5=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCB2)))))))))))));}
# 2127
break;};}case 215: _LL2FC: {
# 2129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2129 "parse.y"
struct _tuple8*_tmp671=Cyc_yyget_YY40(yyyyvsp[4]);struct _dyneither_ptr*_tmp673;struct Cyc_Absyn_Tqual _tmp674;void*_tmp675;struct _tuple8*_tmp672=_tmp671;_tmp673=_tmp672->f1;_tmp674=_tmp672->f2;_tmp675=_tmp672->f3;{
struct Cyc_Absyn_VarargInfo*_tmpCB3;struct Cyc_Absyn_VarargInfo*_tmp676=(_tmpCB3=_cycalloc(sizeof(*_tmpCB3)),((_tmpCB3->name=_tmp673,((_tmpCB3->tq=_tmp674,((_tmpCB3->type=_tmp675,((_tmpCB3->inject=Cyc_yyget_YY34(yyyyvsp[3]),_tmpCB3)))))))));
{struct _tuple26*_tmpCB4;yyval=Cyc_YY42(((_tmpCB4=_cycalloc(sizeof(*_tmpCB4)),((_tmpCB4->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])),((_tmpCB4->f2=0,((_tmpCB4->f3=_tmp676,((_tmpCB4->f4=Cyc_yyget_YY52(yyyyvsp[5]),((_tmpCB4->f5=Cyc_yyget_YY53(yyyyvsp[6]),_tmpCB4)))))))))))));}
# 2133
break;};}case 216: _LL2FD: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2137 "parse.y"
{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpCB7;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpCB6;yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),(void*)((_tmpCB6=_cycalloc(sizeof(*_tmpCB6)),((_tmpCB6[0]=((_tmpCB7.tag=1,((_tmpCB7.f1=0,_tmpCB7)))),_tmpCB6))))));}
break;}case 217: _LL2FE: {
# 2140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2138 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[0]),Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY46(yyyyvsp[2]))));
break;}case 218: _LL2FF:
# 2141
 yyval=Cyc_YY52(0);
break;case 219: _LL300: {
# 2144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2142 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCBA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCB9;yyval=Cyc_YY52((void*)((_tmpCB9=_cycalloc(sizeof(*_tmpCB9)),((_tmpCB9[0]=((_tmpCBA.tag=24,((_tmpCBA.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpCBA)))),_tmpCB9)))));}
break;}case 220: _LL301:
# 2145
 yyval=Cyc_YY53(0);
break;case 221: _LL302: {
# 2148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2147 "parse.y"
yyval=yyyyvsp[1];
break;}case 222: _LL303: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2155 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCBD;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCBC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp67D=(_tmpCBC=_cycalloc(sizeof(*_tmpCBC)),((_tmpCBC[0]=((_tmpCBD.tag=2,((_tmpCBD.f1=0,((_tmpCBD.f2=& Cyc_Tcutil_trk,_tmpCBD)))))),_tmpCBC)));
struct _dyneither_ptr _tmp67E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp67F=Cyc_Parse_id2type(_tmp67E,(void*)_tmp67D);
{struct _tuple29*_tmpCC7;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCC6;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCC5;struct Cyc_List_List*_tmpCC4;yyval=Cyc_YY53(((_tmpCC4=_cycalloc(sizeof(*_tmpCC4)),((_tmpCC4->hd=((_tmpCC7=_cycalloc(sizeof(*_tmpCC7)),((_tmpCC7->f1=(void*)((_tmpCC5=_cycalloc(sizeof(*_tmpCC5)),((_tmpCC5[0]=((_tmpCC6.tag=24,((_tmpCC6.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCC6)))),_tmpCC5)))),((_tmpCC7->f2=_tmp67F,_tmpCC7)))))),((_tmpCC4->tl=0,_tmpCC4)))))));}
# 2160
break;}case 223: _LL304: {
# 2162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2162 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpCCA;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCC9;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp686=(_tmpCC9=_cycalloc(sizeof(*_tmpCC9)),((_tmpCC9[0]=((_tmpCCA.tag=2,((_tmpCCA.f1=0,((_tmpCCA.f2=& Cyc_Tcutil_trk,_tmpCCA)))))),_tmpCC9)));
struct _dyneither_ptr _tmp687=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp688=Cyc_Parse_id2type(_tmp687,(void*)_tmp686);
{struct _tuple29*_tmpCD4;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpCD3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpCD2;struct Cyc_List_List*_tmpCD1;yyval=Cyc_YY53(((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1->hd=((_tmpCD4=_cycalloc(sizeof(*_tmpCD4)),((_tmpCD4->f1=(void*)((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD3.tag=24,((_tmpCD3.f1=Cyc_yyget_YY43(yyyyvsp[0]),_tmpCD3)))),_tmpCD2)))),((_tmpCD4->f2=_tmp688,_tmpCD4)))))),((_tmpCD1->tl=Cyc_yyget_YY53(yyyyvsp[4]),_tmpCD1)))))));}
# 2167
break;}case 224: _LL305:
# 2169
 yyval=Cyc_YY34(0);
break;case 225: _LL306: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2173 "parse.y"
{const char*_tmpCD5;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpCD5="inject",_tag_dyneither(_tmpCD5,sizeof(char),7))))!= 0){
const char*_tmpCD6;Cyc_Parse_err(((_tmpCD6="missing type in function declaration",_tag_dyneither(_tmpCD6,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}}
yyval=Cyc_YY34(1);
# 2177
break;}case 226: _LL307: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2180 "parse.y"
yyval=yyyyvsp[0];
break;}case 227: _LL308: {
# 2183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2181 "parse.y"
yyval=Cyc_YY43(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY43(yyyyvsp[0]),Cyc_yyget_YY43(yyyyvsp[2])));
break;}case 228: _LL309: {
# 2184
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2186
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2185 "parse.y"
yyval=Cyc_YY43(0);
break;}case 229: _LL30A: {
# 2188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2186 "parse.y"
yyval=yyyyvsp[1];
break;}case 230: _LL30B: {
# 2189
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2191
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2188 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpCDC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpCDB;struct Cyc_List_List*_tmpCDA;yyval=Cyc_YY43(((_tmpCDA=_cycalloc(sizeof(*_tmpCDA)),((_tmpCDA->hd=(void*)((_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC[0]=((_tmpCDB.tag=25,((_tmpCDB.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpCDB)))),_tmpCDC)))),((_tmpCDA->tl=0,_tmpCDA)))))));}
break;}case 231: _LL30C: {
# 2191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2190 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY47(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
{struct Cyc_List_List*_tmpCDD;yyval=Cyc_YY43(((_tmpCDD=_cycalloc(sizeof(*_tmpCDD)),((_tmpCDD->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpCDD->tl=0,_tmpCDD)))))));}
# 2193
break;}case 232: _LL30D: {
# 2195
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2197
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2210 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCE3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCE2;struct Cyc_List_List*_tmpCE1;yyval=Cyc_YY43(((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1->hd=(void*)((_tmpCE3=_cycalloc(sizeof(*_tmpCE3)),((_tmpCE3[0]=((_tmpCE2.tag=23,((_tmpCE2.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCE2)))),_tmpCE3)))),((_tmpCE1->tl=0,_tmpCE1)))))));}
break;}case 233: _LL30E: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2212 "parse.y"
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpCE9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpCE8;struct Cyc_List_List*_tmpCE7;yyval=Cyc_YY43(((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7->hd=(void*)((_tmpCE9=_cycalloc(sizeof(*_tmpCE9)),((_tmpCE9[0]=((_tmpCE8.tag=23,((_tmpCE8.f1=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line),_tmpCE8)))),_tmpCE9)))),((_tmpCE7->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpCE7)))))));}
break;}case 234: _LL30F: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpCEA;yyval=Cyc_YY41(((_tmpCEA=_cycalloc(sizeof(*_tmpCEA)),((_tmpCEA->hd=Cyc_yyget_YY40(yyyyvsp[0]),((_tmpCEA->tl=0,_tmpCEA)))))));}
break;}case 235: _LL310: {
# 2221
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2220 "parse.y"
{struct Cyc_List_List*_tmpCEB;yyval=Cyc_YY41(((_tmpCEB=_cycalloc(sizeof(*_tmpCEB)),((_tmpCEB->hd=Cyc_yyget_YY40(yyyyvsp[2]),((_tmpCEB->tl=Cyc_yyget_YY41(yyyyvsp[0]),_tmpCEB)))))));}
break;}case 236: _LL311: {
# 2223
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2225
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
struct _tuple25 _tmp69D=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp69F;struct Cyc_Parse_Type_specifier _tmp6A0;struct Cyc_List_List*_tmp6A1;struct _tuple25 _tmp69E=_tmp69D;_tmp69F=_tmp69E.f1;_tmp6A0=_tmp69E.f2;_tmp6A1=_tmp69E.f3;
if(_tmp69F.loc == 0)_tmp69F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp6A2=Cyc_yyget_YY30(yyyyvsp[1]);struct _tuple0*_tmp6A4;struct Cyc_List_List*_tmp6A5;struct Cyc_Parse_Declarator _tmp6A3=_tmp6A2;_tmp6A4=_tmp6A3.id;_tmp6A5=_tmp6A3.tms;{
void*_tmp6A6=Cyc_Parse_speclist2typ(_tmp6A0,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp6A7=Cyc_Parse_apply_tms(_tmp69F,_tmp6A6,_tmp6A1,_tmp6A5);struct Cyc_Absyn_Tqual _tmp6A9;void*_tmp6AA;struct Cyc_List_List*_tmp6AB;struct Cyc_List_List*_tmp6AC;struct _tuple13 _tmp6A8=_tmp6A7;_tmp6A9=_tmp6A8.f1;_tmp6AA=_tmp6A8.f2;_tmp6AB=_tmp6A8.f3;_tmp6AC=_tmp6A8.f4;
if(_tmp6AB != 0){
const char*_tmpCEC;Cyc_Parse_err(((_tmpCEC="parameter with bad type params",_tag_dyneither(_tmpCEC,sizeof(char),31))),(unsigned int)(yyyylsp[1]).first_line);}
if(Cyc_Absyn_is_qvar_qualified(_tmp6A4)){
const char*_tmpCED;Cyc_Parse_err(((_tmpCED="parameter cannot be qualified with a namespace",_tag_dyneither(_tmpCED,sizeof(char),47))),(unsigned int)(yyyylsp[0]).first_line);}{
struct _dyneither_ptr*idopt=(*_tmp6A4).f2;
if(_tmp6AC != 0){
const char*_tmpCF0;void*_tmpCEF;(_tmpCEF=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCF0="extra attributes on parameter, ignoring",_tag_dyneither(_tmpCF0,sizeof(char),40))),_tag_dyneither(_tmpCEF,sizeof(void*),0)));}
{struct _tuple8*_tmpCF1;yyval=Cyc_YY40(((_tmpCF1=_cycalloc(sizeof(*_tmpCF1)),((_tmpCF1->f1=idopt,((_tmpCF1->f2=_tmp6A9,((_tmpCF1->f3=_tmp6AA,_tmpCF1)))))))));}
# 2240
break;};};};}case 237: _LL312: {
# 2242
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2244
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2241 "parse.y"
struct _tuple25 _tmp6B2=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6B4;struct Cyc_Parse_Type_specifier _tmp6B5;struct Cyc_List_List*_tmp6B6;struct _tuple25 _tmp6B3=_tmp6B2;_tmp6B4=_tmp6B3.f1;_tmp6B5=_tmp6B3.f2;_tmp6B6=_tmp6B3.f3;
if(_tmp6B4.loc == 0)_tmp6B4.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6B7=Cyc_Parse_speclist2typ(_tmp6B5,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6B6 != 0){
const char*_tmpCF4;void*_tmpCF3;(_tmpCF3=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCF4="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCF4,sizeof(char),38))),_tag_dyneither(_tmpCF3,sizeof(void*),0)));}
{struct _tuple8*_tmpCF5;yyval=Cyc_YY40(((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->f1=0,((_tmpCF5->f2=_tmp6B4,((_tmpCF5->f3=_tmp6B7,_tmpCF5)))))))));}
# 2248
break;};}case 238: _LL313: {
# 2250
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2252
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2249 "parse.y"
struct _tuple25 _tmp6BB=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6BD;struct Cyc_Parse_Type_specifier _tmp6BE;struct Cyc_List_List*_tmp6BF;struct _tuple25 _tmp6BC=_tmp6BB;_tmp6BD=_tmp6BC.f1;_tmp6BE=_tmp6BC.f2;_tmp6BF=_tmp6BC.f3;
if(_tmp6BD.loc == 0)_tmp6BD.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp6C0=Cyc_Parse_speclist2typ(_tmp6BE,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6C1=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6C2=Cyc_Parse_apply_tms(_tmp6BD,_tmp6C0,_tmp6BF,_tmp6C1);struct Cyc_Absyn_Tqual _tmp6C4;void*_tmp6C5;struct Cyc_List_List*_tmp6C6;struct Cyc_List_List*_tmp6C7;struct _tuple13 _tmp6C3=_tmp6C2;_tmp6C4=_tmp6C3.f1;_tmp6C5=_tmp6C3.f2;_tmp6C6=_tmp6C3.f3;_tmp6C7=_tmp6C3.f4;
if(_tmp6C6 != 0){
const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCF8="bad type parameters on formal argument, ignoring",_tag_dyneither(_tmpCF8,sizeof(char),49))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}
# 2257
if(_tmp6C7 != 0){
const char*_tmpCFB;void*_tmpCFA;(_tmpCFA=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpCFB="bad attributes on parameter, ignoring",_tag_dyneither(_tmpCFB,sizeof(char),38))),_tag_dyneither(_tmpCFA,sizeof(void*),0)));}
{struct _tuple8*_tmpCFC;yyval=Cyc_YY40(((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->f1=0,((_tmpCFC->f2=_tmp6C4,((_tmpCFC->f3=_tmp6C5,_tmpCFC)))))))));}
# 2261
break;};}case 239: _LL314: {
# 2263
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2265
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2265 "parse.y"
yyval=Cyc_YY39(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY39(yyyyvsp[0])));
break;}case 240: _LL315: {
# 2268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2269 "parse.y"
{struct _dyneither_ptr*_tmpCFF;struct Cyc_List_List*_tmpCFE;yyval=Cyc_YY39(((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpCFF)))),((_tmpCFE->tl=0,_tmpCFE)))))));}
break;}case 241: _LL316: {
# 2272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2271 "parse.y"
{struct _dyneither_ptr*_tmpD02;struct Cyc_List_List*_tmpD01;yyval=Cyc_YY39(((_tmpD01=_cycalloc(sizeof(*_tmpD01)),((_tmpD01->hd=((_tmpD02=_cycalloc(sizeof(*_tmpD02)),((_tmpD02[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD02)))),((_tmpD01->tl=Cyc_yyget_YY39(yyyyvsp[0]),_tmpD01)))))));}
break;}case 242: _LL317: {
# 2274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2275 "parse.y"
yyval=yyyyvsp[0];
break;}case 243: _LL318: {
# 2278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2276 "parse.y"
yyval=yyyyvsp[0];
break;}case 244: _LL319: {
# 2279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2281 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD05;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD04;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpD04=_cycalloc(sizeof(*_tmpD04)),((_tmpD04[0]=((_tmpD05.tag=35,((_tmpD05.f1=0,((_tmpD05.f2=0,_tmpD05)))))),_tmpD04)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 245: _LL31A: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2283 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD08;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD07;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07[0]=((_tmpD08.tag=35,((_tmpD08.f1=0,((_tmpD08.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpD08)))))),_tmpD07)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 246: _LL31B: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2285 "parse.y"
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpD0B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpD0A;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpD0A=_cycalloc(sizeof(*_tmpD0A)),((_tmpD0A[0]=((_tmpD0B.tag=35,((_tmpD0B.f1=0,((_tmpD0B.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[1])),_tmpD0B)))))),_tmpD0A)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 247: _LL31C: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2287 "parse.y"
struct _dyneither_ptr*_tmpD0E;struct _tuple0*_tmpD0D;struct Cyc_Absyn_Vardecl*_tmp6D7=Cyc_Absyn_new_vardecl(((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D->f1=Cyc_Absyn_Loc_n,((_tmpD0D->f2=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpD0E)))),_tmpD0D)))))),Cyc_Absyn_uint_typ,
Cyc_Absyn_uint_exp(0,(unsigned int)(yyyylsp[2]).first_line));
# 2290
(_tmp6D7->tq).real_const=1;
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpD11;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpD10;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10[0]=((_tmpD11.tag=26,((_tmpD11.f1=_tmp6D7,((_tmpD11.f2=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpD11.f3=Cyc_yyget_YY4(yyyyvsp[6]),((_tmpD11.f4=0,_tmpD11)))))))))),_tmpD10)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2293
break;}case 248: _LL31D: {
# 2295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2294 "parse.y"
void*_tmp6DC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpD14;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpD13;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13[0]=((_tmpD14.tag=27,((_tmpD14.f1=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpD14.f2=_tmp6DC,((_tmpD14.f3=0,_tmpD14)))))))),_tmpD13)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2297
break;}case 249: _LL31E: {
# 2299
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct _tuple30*_tmpD17;struct Cyc_List_List*_tmpD16;yyval=Cyc_YY7(((_tmpD16=_cycalloc(sizeof(*_tmpD16)),((_tmpD16->hd=((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->f1=0,((_tmpD17->f2=Cyc_yyget_YY4(yyyyvsp[0]),_tmpD17)))))),((_tmpD16->tl=0,_tmpD16)))))));}
break;}case 250: _LL31F: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2304 "parse.y"
{struct _tuple30*_tmpD1A;struct Cyc_List_List*_tmpD19;yyval=Cyc_YY7(((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpD1A->f2=Cyc_yyget_YY4(yyyyvsp[1]),_tmpD1A)))))),((_tmpD19->tl=0,_tmpD19)))))));}
break;}case 251: _LL320: {
# 2307
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2309
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2306 "parse.y"
{struct _tuple30*_tmpD1D;struct Cyc_List_List*_tmpD1C;yyval=Cyc_YY7(((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=0,((_tmpD1D->f2=Cyc_yyget_YY4(yyyyvsp[2]),_tmpD1D)))))),((_tmpD1C->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpD1C)))))));}
break;}case 252: _LL321: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2308 "parse.y"
{struct _tuple30*_tmpD20;struct Cyc_List_List*_tmpD1F;yyval=Cyc_YY7(((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->hd=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->f1=Cyc_yyget_YY44(yyyyvsp[2]),((_tmpD20->f2=Cyc_yyget_YY4(yyyyvsp[3]),_tmpD20)))))),((_tmpD1F->tl=Cyc_yyget_YY7(yyyyvsp[0]),_tmpD1F)))))));}
break;}case 253: _LL322: {
# 2311
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2313
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2312 "parse.y"
yyval=Cyc_YY44(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY44(yyyyvsp[0])));
break;}case 254: _LL323: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2317 "parse.y"
{struct Cyc_List_List*_tmpD21;yyval=Cyc_YY44(((_tmpD21=_cycalloc(sizeof(*_tmpD21)),((_tmpD21->hd=Cyc_yyget_YY45(yyyyvsp[0]),((_tmpD21->tl=0,_tmpD21)))))));}
break;}case 255: _LL324: {
# 2320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2318 "parse.y"
{struct Cyc_List_List*_tmpD22;yyval=Cyc_YY44(((_tmpD22=_cycalloc(sizeof(*_tmpD22)),((_tmpD22->hd=Cyc_yyget_YY45(yyyyvsp[1]),((_tmpD22->tl=Cyc_yyget_YY44(yyyyvsp[0]),_tmpD22)))))));}
break;}case 256: _LL325: {
# 2321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2322 "parse.y"
{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD25;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD24;yyval=Cyc_YY45((void*)((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24[0]=((_tmpD25.tag=0,((_tmpD25.f1=Cyc_yyget_YY4(yyyyvsp[1]),_tmpD25)))),_tmpD24)))));}
break;}case 257: _LL326: {
# 2325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2323 "parse.y"
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD2B;struct _dyneither_ptr*_tmpD2A;struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD29;yyval=Cyc_YY45((void*)((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29[0]=((_tmpD2B.tag=1,((_tmpD2B.f1=((_tmpD2A=_cycalloc(sizeof(*_tmpD2A)),((_tmpD2A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpD2A)))),_tmpD2B)))),_tmpD29)))));}
break;}case 258: _LL327: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2328 "parse.y"
struct _tuple25 _tmp6EE=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6F0;struct Cyc_Parse_Type_specifier _tmp6F1;struct Cyc_List_List*_tmp6F2;struct _tuple25 _tmp6EF=_tmp6EE;_tmp6F0=_tmp6EF.f1;_tmp6F1=_tmp6EF.f2;_tmp6F2=_tmp6EF.f3;{
void*_tmp6F3=Cyc_Parse_speclist2typ(_tmp6F1,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp6F2 != 0){
const char*_tmpD2E;void*_tmpD2D;(_tmpD2D=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[0]).first_line,((_tmpD2E="ignoring attributes in type",_tag_dyneither(_tmpD2E,sizeof(char),28))),_tag_dyneither(_tmpD2D,sizeof(void*),0)));}
{struct _tuple8*_tmpD2F;yyval=Cyc_YY40(((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->f1=0,((_tmpD2F->f2=_tmp6F0,((_tmpD2F->f3=_tmp6F3,_tmpD2F)))))))));}
# 2334
break;};}case 259: _LL328: {
# 2336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2335 "parse.y"
struct _tuple25 _tmp6F7=Cyc_yyget_YY38(yyyyvsp[0]);struct Cyc_Absyn_Tqual _tmp6F9;struct Cyc_Parse_Type_specifier _tmp6FA;struct Cyc_List_List*_tmp6FB;struct _tuple25 _tmp6F8=_tmp6F7;_tmp6F9=_tmp6F8.f1;_tmp6FA=_tmp6F8.f2;_tmp6FB=_tmp6F8.f3;{
void*_tmp6FC=Cyc_Parse_speclist2typ(_tmp6FA,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp6FD=(Cyc_yyget_YY33(yyyyvsp[1])).tms;
struct _tuple13 _tmp6FE=Cyc_Parse_apply_tms(_tmp6F9,_tmp6FC,_tmp6FB,_tmp6FD);
if(_tmp6FE.f3 != 0){
# 2341
const char*_tmpD32;void*_tmpD31;(_tmpD31=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD32="bad type params, ignoring",_tag_dyneither(_tmpD32,sizeof(char),26))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}
if(_tmp6FE.f4 != 0){
const char*_tmpD35;void*_tmpD34;(_tmpD34=0,Cyc_Tcutil_warn((unsigned int)(yyyylsp[1]).first_line,((_tmpD35="bad specifiers, ignoring",_tag_dyneither(_tmpD35,sizeof(char),25))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}
{struct _tuple8*_tmpD36;yyval=Cyc_YY40(((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=0,((_tmpD36->f2=_tmp6FE.f1,((_tmpD36->f3=_tmp6FE.f2,_tmpD36)))))))));}
# 2346
break;};}case 260: _LL329: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2349 "parse.y"
yyval=Cyc_YY47(Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 261: _LL32A: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2350 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD39;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD38;yyval=Cyc_YY47((void*)((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38[0]=((_tmpD39.tag=24,((_tmpD39.f1=0,_tmpD39)))),_tmpD38)))));}
break;}case 262: _LL32B: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2351 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD3C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD3B;yyval=Cyc_YY47((void*)((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B[0]=((_tmpD3C.tag=24,((_tmpD3C.f1=Cyc_yyget_YY43(yyyyvsp[1]),_tmpD3C)))),_tmpD3B)))));}
break;}case 263: _LL32C: {
# 2354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2352 "parse.y"
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD3F;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpD3E;yyval=Cyc_YY47((void*)((_tmpD3E=_cycalloc(sizeof(*_tmpD3E)),((_tmpD3E[0]=((_tmpD3F.tag=25,((_tmpD3F.f1=Cyc_yyget_YY47(yyyyvsp[2]),_tmpD3F)))),_tmpD3E)))));}
break;}case 264: _LL32D: {
# 2355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2353 "parse.y"
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD45;struct Cyc_List_List*_tmpD44;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpD43;yyval=Cyc_YY47((void*)((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=((_tmpD45.tag=24,((_tmpD45.f1=((_tmpD44=_cycalloc(sizeof(*_tmpD44)),((_tmpD44->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD44->tl=Cyc_yyget_YY43(yyyyvsp[2]),_tmpD44)))))),_tmpD45)))),_tmpD43)))));}
break;}case 265: _LL32E: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpD46;yyval=Cyc_YY43(((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->hd=Cyc_yyget_YY47(yyyyvsp[0]),((_tmpD46->tl=0,_tmpD46)))))));}
break;}case 266: _LL32F: {
# 2362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2360 "parse.y"
{struct Cyc_List_List*_tmpD47;yyval=Cyc_YY43(((_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=Cyc_yyget_YY47(yyyyvsp[2]),((_tmpD47->tl=Cyc_yyget_YY43(yyyyvsp[0]),_tmpD47)))))));}
break;}case 267: _LL330: {
# 2363
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2365
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2365 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD48;yyval=Cyc_YY33(((_tmpD48.tms=Cyc_yyget_YY29(yyyyvsp[0]),_tmpD48)));}
break;}case 268: _LL331: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2367 "parse.y"
yyval=yyyyvsp[0];
break;}case 269: _LL332: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2369 "parse.y"
{struct Cyc_Parse_Abstractdeclarator _tmpD49;yyval=Cyc_YY33(((_tmpD49.tms=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_yyget_YY29(yyyyvsp[0]),(Cyc_yyget_YY33(yyyyvsp[1])).tms),_tmpD49)));}
break;}case 270: _LL333: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2374 "parse.y"
yyval=yyyyvsp[1];
break;}case 271: _LL334: {
# 2377
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2379
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2376 "parse.y"
{struct Cyc_List_List*_tmpD53;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD52;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD51;struct Cyc_Parse_Abstractdeclarator _tmpD50;yyval=Cyc_YY33(((_tmpD50.tms=((_tmpD53=_region_malloc(yyr,sizeof(*_tmpD53)),((_tmpD53->hd=(void*)((_tmpD51=_region_malloc(yyr,sizeof(*_tmpD51)),((_tmpD51[0]=((_tmpD52.tag=0,((_tmpD52.f1=Cyc_yyget_YY54(yyyyvsp[2]),((_tmpD52.f2=(unsigned int)(yyyylsp[2]).first_line,_tmpD52)))))),_tmpD51)))),((_tmpD53->tl=0,_tmpD53)))))),_tmpD50)));}
break;}case 272: _LL335: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2378 "parse.y"
{struct Cyc_List_List*_tmpD5D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpD5C;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD5B;struct Cyc_Parse_Abstractdeclarator _tmpD5A;yyval=Cyc_YY33(((_tmpD5A.tms=((_tmpD5D=_region_malloc(yyr,sizeof(*_tmpD5D)),((_tmpD5D->hd=(void*)((_tmpD5B=_region_malloc(yyr,sizeof(*_tmpD5B)),((_tmpD5B[0]=((_tmpD5C.tag=0,((_tmpD5C.f1=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD5C.f2=(unsigned int)(yyyylsp[3]).first_line,_tmpD5C)))))),_tmpD5B)))),((_tmpD5D->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD5D)))))),_tmpD5A)));}
break;}case 273: _LL336: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2380 "parse.y"
{struct Cyc_List_List*_tmpD67;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD66;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD65;struct Cyc_Parse_Abstractdeclarator _tmpD64;yyval=Cyc_YY33(((_tmpD64.tms=((_tmpD67=_region_malloc(yyr,sizeof(*_tmpD67)),((_tmpD67->hd=(void*)((_tmpD65=_region_malloc(yyr,sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD66.tag=1,((_tmpD66.f1=Cyc_yyget_YY4(yyyyvsp[1]),((_tmpD66.f2=Cyc_yyget_YY54(yyyyvsp[3]),((_tmpD66.f3=(unsigned int)(yyyylsp[3]).first_line,_tmpD66)))))))),_tmpD65)))),((_tmpD67->tl=0,_tmpD67)))))),_tmpD64)));}
break;}case 274: _LL337: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2382 "parse.y"
{struct Cyc_List_List*_tmpD71;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpD70;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD6F;struct Cyc_Parse_Abstractdeclarator _tmpD6E;yyval=Cyc_YY33(((_tmpD6E.tms=((_tmpD71=_region_malloc(yyr,sizeof(*_tmpD71)),((_tmpD71->hd=(void*)((_tmpD6F=_region_malloc(yyr,sizeof(*_tmpD6F)),((_tmpD6F[0]=((_tmpD70.tag=1,((_tmpD70.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpD70.f2=Cyc_yyget_YY54(yyyyvsp[4]),((_tmpD70.f3=(unsigned int)(yyyylsp[4]).first_line,_tmpD70)))))))),_tmpD6F)))),((_tmpD71->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpD71)))))),_tmpD6E)));}
# 2385
break;}case 275: _LL338: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2386 "parse.y"
{struct Cyc_List_List*_tmpD86;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD85;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD84;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD83;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD82;struct Cyc_Parse_Abstractdeclarator _tmpD81;yyval=Cyc_YY33(((_tmpD81.tms=((_tmpD86=_region_malloc(yyr,sizeof(*_tmpD86)),((_tmpD86->hd=(void*)((_tmpD82=_region_malloc(yyr,sizeof(*_tmpD82)),((_tmpD82[0]=((_tmpD85.tag=3,((_tmpD85.f1=(void*)((_tmpD83=_region_malloc(yyr,sizeof(*_tmpD83)),((_tmpD83[0]=((_tmpD84.tag=1,((_tmpD84.f1=0,((_tmpD84.f2=0,((_tmpD84.f3=0,((_tmpD84.f4=Cyc_yyget_YY52(yyyyvsp[1]),((_tmpD84.f5=Cyc_yyget_YY53(yyyyvsp[2]),_tmpD84)))))))))))),_tmpD83)))),_tmpD85)))),_tmpD82)))),((_tmpD86->tl=0,_tmpD86)))))),_tmpD81)));}
# 2388
break;}case 276: _LL339: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2389 "parse.y"
struct _tuple26*_tmp727=Cyc_yyget_YY42(yyyyvsp[1]);struct Cyc_List_List*_tmp729;int _tmp72A;struct Cyc_Absyn_VarargInfo*_tmp72B;void*_tmp72C;struct Cyc_List_List*_tmp72D;struct _tuple26*_tmp728=_tmp727;_tmp729=_tmp728->f1;_tmp72A=_tmp728->f2;_tmp72B=_tmp728->f3;_tmp72C=_tmp728->f4;_tmp72D=_tmp728->f5;
{struct Cyc_List_List*_tmpD9B;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpD9A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpD99;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD98;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD97;struct Cyc_Parse_Abstractdeclarator _tmpD96;yyval=Cyc_YY33(((_tmpD96.tms=((_tmpD9B=_region_malloc(yyr,sizeof(*_tmpD9B)),((_tmpD9B->hd=(void*)((_tmpD97=_region_malloc(yyr,sizeof(*_tmpD97)),((_tmpD97[0]=((_tmpD9A.tag=3,((_tmpD9A.f1=(void*)((_tmpD98=_region_malloc(yyr,sizeof(*_tmpD98)),((_tmpD98[0]=((_tmpD99.tag=1,((_tmpD99.f1=_tmp729,((_tmpD99.f2=_tmp72A,((_tmpD99.f3=_tmp72B,((_tmpD99.f4=_tmp72C,((_tmpD99.f5=_tmp72D,_tmpD99)))))))))))),_tmpD98)))),_tmpD9A)))),_tmpD97)))),((_tmpD9B->tl=0,_tmpD9B)))))),_tmpD96)));}
# 2392
break;}case 277: _LL33A: {
# 2394
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2396
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2393 "parse.y"
{struct Cyc_List_List*_tmpDB0;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDAF;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDAE;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDAD;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDAC;struct Cyc_Parse_Abstractdeclarator _tmpDAB;yyval=Cyc_YY33(((_tmpDAB.tms=((_tmpDB0=_region_malloc(yyr,sizeof(*_tmpDB0)),((_tmpDB0->hd=(void*)((_tmpDAC=_region_malloc(yyr,sizeof(*_tmpDAC)),((_tmpDAC[0]=((_tmpDAF.tag=3,((_tmpDAF.f1=(void*)((_tmpDAD=_region_malloc(yyr,sizeof(*_tmpDAD)),((_tmpDAD[0]=((_tmpDAE.tag=1,((_tmpDAE.f1=0,((_tmpDAE.f2=0,((_tmpDAE.f3=0,((_tmpDAE.f4=Cyc_yyget_YY52(yyyyvsp[2]),((_tmpDAE.f5=Cyc_yyget_YY53(yyyyvsp[3]),_tmpDAE)))))))))))),_tmpDAD)))),_tmpDAF)))),_tmpDAC)))),((_tmpDB0->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDB0)))))),_tmpDAB)));}
# 2396
break;}case 278: _LL33B: {
# 2398
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2397 "parse.y"
struct _tuple26*_tmp73A=Cyc_yyget_YY42(yyyyvsp[2]);struct Cyc_List_List*_tmp73C;int _tmp73D;struct Cyc_Absyn_VarargInfo*_tmp73E;void*_tmp73F;struct Cyc_List_List*_tmp740;struct _tuple26*_tmp73B=_tmp73A;_tmp73C=_tmp73B->f1;_tmp73D=_tmp73B->f2;_tmp73E=_tmp73B->f3;_tmp73F=_tmp73B->f4;_tmp740=_tmp73B->f5;
{struct Cyc_List_List*_tmpDC5;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpDC4;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpDC3;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDC2;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDC1;struct Cyc_Parse_Abstractdeclarator _tmpDC0;yyval=Cyc_YY33(((_tmpDC0.tms=((_tmpDC5=_region_malloc(yyr,sizeof(*_tmpDC5)),((_tmpDC5->hd=(void*)((_tmpDC1=_region_malloc(yyr,sizeof(*_tmpDC1)),((_tmpDC1[0]=((_tmpDC4.tag=3,((_tmpDC4.f1=(void*)((_tmpDC2=_region_malloc(yyr,sizeof(*_tmpDC2)),((_tmpDC2[0]=((_tmpDC3.tag=1,((_tmpDC3.f1=_tmp73C,((_tmpDC3.f2=_tmp73D,((_tmpDC3.f3=_tmp73E,((_tmpDC3.f4=_tmp73F,((_tmpDC3.f5=_tmp740,_tmpDC3)))))))))))),_tmpDC2)))),_tmpDC4)))),_tmpDC1)))),((_tmpDC5->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDC5)))))),_tmpDC0)));}
# 2401
break;}case 279: _LL33C: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2403 "parse.y"
struct Cyc_List_List*_tmp747=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[2])));
{struct Cyc_List_List*_tmpDCF;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpDCE;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpDCD;struct Cyc_Parse_Abstractdeclarator _tmpDCC;yyval=Cyc_YY33(((_tmpDCC.tms=((_tmpDCF=_region_malloc(yyr,sizeof(*_tmpDCF)),((_tmpDCF->hd=(void*)((_tmpDCD=_region_malloc(yyr,sizeof(*_tmpDCD)),((_tmpDCD[0]=((_tmpDCE.tag=4,((_tmpDCE.f1=_tmp747,((_tmpDCE.f2=(unsigned int)(yyyylsp[1]).first_line,((_tmpDCE.f3=0,_tmpDCE)))))))),_tmpDCD)))),((_tmpDCF->tl=(
Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDCF)))))),_tmpDCC)));}
# 2407
break;}case 280: _LL33D: {
# 2409
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2411
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2408 "parse.y"
{struct Cyc_List_List*_tmpDD9;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpDD8;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpDD7;struct Cyc_Parse_Abstractdeclarator _tmpDD6;yyval=Cyc_YY33(((_tmpDD6.tms=((_tmpDD9=_region_malloc(yyr,sizeof(*_tmpDD9)),((_tmpDD9->hd=(void*)((_tmpDD7=_region_malloc(yyr,sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDD8.tag=5,((_tmpDD8.f1=(unsigned int)(yyyylsp[1]).first_line,((_tmpDD8.f2=Cyc_yyget_YY48(yyyyvsp[1]),_tmpDD8)))))),_tmpDD7)))),((_tmpDD9->tl=(Cyc_yyget_YY33(yyyyvsp[0])).tms,_tmpDD9)))))),_tmpDD6)));}
# 2410
break;}case 281: _LL33E: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2414 "parse.y"
yyval=yyyyvsp[0];
break;}case 282: _LL33F: {
# 2417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2415 "parse.y"
yyval=yyyyvsp[0];
break;}case 283: _LL340: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2416 "parse.y"
yyval=yyyyvsp[0];
break;}case 284: _LL341: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2417 "parse.y"
yyval=yyyyvsp[0];
break;}case 285: _LL342: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2418 "parse.y"
yyval=yyyyvsp[0];
break;}case 286: _LL343: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2419 "parse.y"
yyval=yyyyvsp[0];
break;}case 287: _LL344: {
# 2422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2422 "parse.y"
{struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct _tmpDDC;struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmpDDB;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDDB=_cycalloc(sizeof(*_tmpDDB)),((_tmpDDB[0]=((_tmpDDC.tag=16,((_tmpDDC.f1=Cyc_yyget_YY4(yyyyvsp[2]),_tmpDDC)))),_tmpDDB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 288: _LL345:
# 2425
 yyval=Cyc_YY5(0);
break;case 289: _LL346: {
# 2428
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2428 "parse.y"
{const char*_tmpDDD;if(Cyc_zstrcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpDDD="open",_tag_dyneither(_tmpDDD,sizeof(char),5))))!= 0){
const char*_tmpDDE;Cyc_Parse_err(((_tmpDDE="expecting `open'",_tag_dyneither(_tmpDDE,sizeof(char),17))),(unsigned int)(yyyylsp[1]).first_line);}}
yyval=Cyc_YY5(Cyc_yyget_YY4(yyyyvsp[3]));
# 2432
break;}case 290: _LL347: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDE4;struct _dyneither_ptr*_tmpDE3;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDE2;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2[0]=((_tmpDE4.tag=13,((_tmpDE4.f1=((_tmpDE3=_cycalloc(sizeof(*_tmpDE3)),((_tmpDE3[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDE3)))),((_tmpDE4.f2=Cyc_yyget_YY10(yyyyvsp[2]),_tmpDE4)))))),_tmpDE2)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 291: _LL348: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2441 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 292: _LL349: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2442 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_exp_stmt(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 293: _LL34A: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2447 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 294: _LL34B: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2448 "parse.y"
yyval=yyyyvsp[1];
break;}case 295: _LL34C: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2453 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line)));
break;}case 296: _LL34D: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2454 "parse.y"
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1])));
break;}case 297: _LL34E: {
# 2457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2455 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDEA;struct _dyneither_ptr*_tmpDE9;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDE8;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8[0]=((_tmpDEA.tag=13,((_tmpDEA.f1=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDE9)))),((_tmpDEA.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_Absyn_skip_stmt(0)),_tmpDEA)))))),_tmpDE8)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2457
break;}case 298: _LL34F: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2457 "parse.y"
{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmpDF0;struct _dyneither_ptr*_tmpDEF;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmpDEE;yyval=Cyc_YY10(Cyc_Absyn_new_stmt((void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDF0.tag=13,((_tmpDF0.f1=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpDEF)))),((_tmpDF0.f2=Cyc_Parse_flatten_declarations(Cyc_yyget_YY19(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[3])),_tmpDF0)))))),_tmpDEE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2459
break;}case 299: _LL350: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2459 "parse.y"
yyval=yyyyvsp[0];
break;}case 300: _LL351: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2460 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_seq_stmt(Cyc_yyget_YY10(yyyyvsp[0]),Cyc_yyget_YY10(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 301: _LL352: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2461 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDF3;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDF2;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF3.tag=1,((_tmpDF3.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDF3)))),_tmpDF2)))),(unsigned int)(yyyylsp[0]).first_line),
Cyc_Absyn_skip_stmt(0)));}
break;}case 302: _LL353: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2464 "parse.y"
{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct _tmpDF6;struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpDF5;yyval=Cyc_YY10(Cyc_Parse_flatten_decl(Cyc_Absyn_new_decl((void*)((_tmpDF5=_cycalloc(sizeof(*_tmpDF5)),((_tmpDF5[0]=((_tmpDF6.tag=1,((_tmpDF6.f1=Cyc_yyget_YY18(yyyyvsp[0]),_tmpDF6)))),_tmpDF5)))),(unsigned int)(yyyylsp[0]).first_line),Cyc_yyget_YY10(yyyyvsp[1])));}
break;}case 303: _LL354: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2469 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_Absyn_skip_stmt(0),(unsigned int)(yyyylsp[0]).first_line));
break;}case 304: _LL355: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2471 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_ifthenelse_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 305: _LL356: {
# 2474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY11(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 306: _LL357: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2480 "parse.y"
struct Cyc_Absyn_Exp*_tmp761=Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp761,Cyc_yyget_YY11(yyyyvsp[3]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 307: _LL358: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2484 "parse.y"
struct Cyc_Absyn_Exp*_tmp762=Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[3]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY10(Cyc_Absyn_switch_stmt(_tmp762,Cyc_yyget_YY11(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
# 2487
break;}case 308: _LL359: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2490 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_trycatch_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY11(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 309: _LL35A:
# 2493
 yyval=Cyc_YY11(0);
break;case 310: _LL35B: {
# 2496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2506 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDF9;struct Cyc_List_List*_tmpDF8;yyval=Cyc_YY11(((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8->hd=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->pattern=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line),((_tmpDF9->pat_vars=0,((_tmpDF9->where_clause=0,((_tmpDF9->body=
Cyc_yyget_YY10(yyyyvsp[2]),((_tmpDF9->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDF9)))))))))))),((_tmpDF8->tl=0,_tmpDF8)))))));}
# 2509
break;}case 311: _LL35C: {
# 2511
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2513
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2510 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDFC;struct Cyc_List_List*_tmpDFB;yyval=Cyc_YY11(((_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->hd=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDFC->pat_vars=0,((_tmpDFC->where_clause=0,((_tmpDFC->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line),((_tmpDFC->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDFC)))))))))))),((_tmpDFB->tl=
Cyc_yyget_YY11(yyyyvsp[3]),_tmpDFB)))))));}
break;}case 312: _LL35D: {
# 2515
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2517
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2514 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpDFF;struct Cyc_List_List*_tmpDFE;yyval=Cyc_YY11(((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpDFF->pat_vars=0,((_tmpDFF->where_clause=0,((_tmpDFF->body=Cyc_yyget_YY10(yyyyvsp[3]),((_tmpDFF->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpDFF)))))))))))),((_tmpDFE->tl=Cyc_yyget_YY11(yyyyvsp[4]),_tmpDFE)))))));}
break;}case 313: _LL35E: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2516 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE02;struct Cyc_List_List*_tmpE01;yyval=Cyc_YY11(((_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->hd=((_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpE02->pat_vars=0,((_tmpE02->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpE02->body=
Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line),((_tmpE02->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE02)))))))))))),((_tmpE01->tl=
Cyc_yyget_YY11(yyyyvsp[5]),_tmpE01)))))));}
break;}case 314: _LL35F: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2520 "parse.y"
{struct Cyc_Absyn_Switch_clause*_tmpE05;struct Cyc_List_List*_tmpE04;yyval=Cyc_YY11(((_tmpE04=_cycalloc(sizeof(*_tmpE04)),((_tmpE04->hd=((_tmpE05=_cycalloc(sizeof(*_tmpE05)),((_tmpE05->pattern=Cyc_yyget_YY12(yyyyvsp[1]),((_tmpE05->pat_vars=0,((_tmpE05->where_clause=Cyc_yyget_YY4(yyyyvsp[3]),((_tmpE05->body=Cyc_yyget_YY10(yyyyvsp[5]),((_tmpE05->loc=(unsigned int)(yyyylsp[0]).first_line,_tmpE05)))))))))))),((_tmpE04->tl=Cyc_yyget_YY11(yyyyvsp[6]),_tmpE04)))))));}
break;}case 315: _LL360: {
# 2523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2527 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_while_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY10(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 316: _LL361: {
# 2530
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2532
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2531 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_do_stmt(Cyc_yyget_YY10(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 317: _LL362: {
# 2534
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2535 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 318: _LL363: {
# 2539
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2541
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2538 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 319: _LL364: {
# 2542
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2544
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2541 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 320: _LL365: {
# 2545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2544 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 321: _LL366: {
# 2548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2547 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 322: _LL367: {
# 2551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2550 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 323: _LL368: {
# 2554
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2556
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2553 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 324: _LL369: {
# 2557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 2556 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_for_stmt(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),Cyc_yyget_YY4(yyyyvsp[6]),
Cyc_yyget_YY10(yyyyvsp[8]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 325: _LL36A: {
# 2560
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2559 "parse.y"
struct Cyc_List_List*_tmp76D=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp76E=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[5]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp76D,_tmp76E));
# 2564
break;}case 326: _LL36B: {
# 2566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2565 "parse.y"
struct Cyc_List_List*_tmp76F=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp770=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_Absyn_false_exp(0),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp76F,_tmp770));
# 2570
break;}case 327: _LL36C: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 2571 "parse.y"
struct Cyc_List_List*_tmp771=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp772=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_Absyn_true_exp(0),Cyc_yyget_YY4(yyyyvsp[4]),
Cyc_yyget_YY10(yyyyvsp[6]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp771,_tmp772));
# 2576
break;}case 328: _LL36D: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8);
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8);
# 2577 "parse.y"
struct Cyc_List_List*_tmp773=Cyc_yyget_YY19(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp774=Cyc_Absyn_for_stmt(Cyc_Absyn_false_exp(0),Cyc_yyget_YY4(yyyyvsp[3]),Cyc_yyget_YY4(yyyyvsp[5]),
Cyc_yyget_YY10(yyyyvsp[7]),(unsigned int)(yyyylsp[0]).first_line);
yyval=Cyc_YY10(Cyc_Parse_flatten_declarations(_tmp773,_tmp774));
# 2582
break;}case 329: _LL36E: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
{struct _dyneither_ptr*_tmpE06;yyval=Cyc_YY10(Cyc_Absyn_goto_stmt(((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE06)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 330: _LL36F: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2588 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 331: _LL370: {
# 2591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2589 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
break;}case 332: _LL371: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2590 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 333: _LL372: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2591 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_return_stmt(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 334: _LL373: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2593 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 335: _LL374: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2594 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 336: _LL375: {
# 2597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2596 "parse.y"
yyval=Cyc_YY10(Cyc_Absyn_fallthru_stmt(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 337: _LL376: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2605 "parse.y"
yyval=yyyyvsp[0];
break;}case 338: _LL377: {
# 2608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2608 "parse.y"
yyval=yyyyvsp[0];
break;}case 339: _LL378: {
# 2611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2610 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_conditional_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 340: _LL379: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2613 "parse.y"
yyval=yyyyvsp[0];
break;}case 341: _LL37A: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2615 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_or_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 342: _LL37B: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2618 "parse.y"
yyval=yyyyvsp[0];
break;}case 343: _LL37C: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2620 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_and_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 344: _LL37D: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2623 "parse.y"
yyval=yyyyvsp[0];
break;}case 345: _LL37E: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2625 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 346: _LL37F: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2628 "parse.y"
yyval=yyyyvsp[0];
break;}case 347: _LL380: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2630 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 348: _LL381: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2633 "parse.y"
yyval=yyyyvsp[0];
break;}case 349: _LL382: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2635 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 350: _LL383: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2638 "parse.y"
yyval=yyyyvsp[0];
break;}case 351: _LL384: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2640 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_eq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 352: _LL385: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2642 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_neq_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 353: _LL386: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2645 "parse.y"
yyval=yyyyvsp[0];
break;}case 354: _LL387: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2647 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 355: _LL388: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2649 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gt_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 356: _LL389: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2651 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_lte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 357: _LL38A: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2653 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_gte_exp(Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 358: _LL38B: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2656 "parse.y"
yyval=yyyyvsp[0];
break;}case 359: _LL38C: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2658 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 360: _LL38D: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2660 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 361: _LL38E: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2663 "parse.y"
yyval=yyyyvsp[0];
break;}case 362: _LL38F: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2665 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 363: _LL390: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2667 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 364: _LL391: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2670 "parse.y"
yyval=yyyyvsp[0];
break;}case 365: _LL392: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2672 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 366: _LL393: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2674 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 367: _LL394: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2676 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_Parse_pat2exp(Cyc_yyget_YY12(yyyyvsp[0])),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 368: _LL395: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2679 "parse.y"
yyval=yyyyvsp[0];
break;}case 369: _LL396: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2681 "parse.y"
void*_tmp776=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_cast_exp(_tmp776,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line)));
# 2684
break;}case 370: _LL397: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2687 "parse.y"
yyval=yyyyvsp[0];
break;}case 371: _LL398: {
# 2690
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2692
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2690 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 372: _LL399: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2692 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 373: _LL39A: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2694 "parse.y"
void*_tmp777=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp777,(unsigned int)(yyyylsp[0]).first_line)));
# 2697
break;}case 374: _LL39B: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2698 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 375: _LL39C: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2700 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_Absyn_offsetof_exp((*Cyc_yyget_YY40(yyyyvsp[2])).f3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line)));
break;}case 376: _LL39D: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2705 "parse.y"
yyval=yyyyvsp[0];
break;}case 377: _LL39E: {
# 2708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2714 "parse.y"
yyval=yyyyvsp[0];
break;}case 378: _LL39F: {
# 2717
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2719
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2719 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
break;}case 379: _LL3A0: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2721 "parse.y"
yyval=Cyc_YY12(Cyc_Absyn_exp_pat(Cyc_yyget_YY4(yyyyvsp[1])));
break;}case 380: _LL3A1: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2723 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_YY4(yyyyvsp[0]);
{void*_tmp778=e->r;void*_tmp779=_tmp778;enum Cyc_Absyn_Sign _tmp77B;char _tmp77C;enum Cyc_Absyn_Sign _tmp77E;short _tmp77F;enum Cyc_Absyn_Sign _tmp781;int _tmp782;struct _dyneither_ptr _tmp784;int _tmp785;_LL3A4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp77A->tag != 0)goto _LL3A6;else{if(((_tmp77A->f1).Char_c).tag != 2)goto _LL3A6;_tmp77B=((struct _tuple3)((_tmp77A->f1).Char_c).val).f1;_tmp77C=((struct _tuple3)((_tmp77A->f1).Char_c).val).f2;}}_LL3A5:
# 2727
{struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct _tmpE09;struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpE08;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE08=_cycalloc_atomic(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE09.tag=11,((_tmpE09.f1=_tmp77C,_tmpE09)))),_tmpE08)))),e->loc));}goto _LL3A3;_LL3A6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp77D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp77D->tag != 0)goto _LL3A8;else{if(((_tmp77D->f1).Short_c).tag != 4)goto _LL3A8;_tmp77E=((struct _tuple4)((_tmp77D->f1).Short_c).val).f1;_tmp77F=((struct _tuple4)((_tmp77D->f1).Short_c).val).f2;}}_LL3A7:
# 2729
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE0C;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE0B;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0B=_cycalloc_atomic(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=10,((_tmpE0C.f1=_tmp77E,((_tmpE0C.f2=(int)_tmp77F,_tmpE0C)))))),_tmpE0B)))),e->loc));}goto _LL3A3;_LL3A8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp780=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp780->tag != 0)goto _LL3AA;else{if(((_tmp780->f1).Int_c).tag != 5)goto _LL3AA;_tmp781=((struct _tuple5)((_tmp780->f1).Int_c).val).f1;_tmp782=((struct _tuple5)((_tmp780->f1).Int_c).val).f2;}}_LL3A9:
# 2731
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmpE0F;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpE0E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE0E=_cycalloc_atomic(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0F.tag=10,((_tmpE0F.f1=_tmp781,((_tmpE0F.f2=_tmp782,_tmpE0F)))))),_tmpE0E)))),e->loc));}goto _LL3A3;_LL3AA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp783=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp783->tag != 0)goto _LL3AC;else{if(((_tmp783->f1).Float_c).tag != 7)goto _LL3AC;_tmp784=((struct _tuple7)((_tmp783->f1).Float_c).val).f1;_tmp785=((struct _tuple7)((_tmp783->f1).Float_c).val).f2;}}_LL3AB:
# 2733
{struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct _tmpE12;struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpE11;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE11=_cycalloc(sizeof(*_tmpE11)),((_tmpE11[0]=((_tmpE12.tag=12,((_tmpE12.f1=_tmp784,((_tmpE12.f2=_tmp785,_tmpE12)))))),_tmpE11)))),e->loc));}goto _LL3A3;_LL3AC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp786=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp786->tag != 0)goto _LL3AE;else{if(((_tmp786->f1).Null_c).tag != 1)goto _LL3AE;}}_LL3AD:
# 2735
 yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL3A3;_LL3AE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp787=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp787->tag != 0)goto _LL3B0;else{if(((_tmp787->f1).String_c).tag != 8)goto _LL3B0;}}_LL3AF:
# 2737
{const char*_tmpE13;Cyc_Parse_err(((_tmpE13="strings cannot occur within patterns",_tag_dyneither(_tmpE13,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A3;_LL3B0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp788=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp788->tag != 0)goto _LL3B2;else{if(((_tmp788->f1).Wstring_c).tag != 9)goto _LL3B2;}}_LL3B1:
# 2739
{const char*_tmpE14;Cyc_Parse_err(((_tmpE14="strings cannot occur within patterns",_tag_dyneither(_tmpE14,sizeof(char),37))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A3;_LL3B2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp789=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp779;if(_tmp789->tag != 0)goto _LL3B4;else{if(((_tmp789->f1).LongLong_c).tag != 6)goto _LL3B4;}}_LL3B3:
# 2741
{const char*_tmpE15;Cyc_Parse_unimp(((_tmpE15="long long's in patterns",_tag_dyneither(_tmpE15,sizeof(char),24))),(unsigned int)(yyyylsp[0]).first_line);}goto _LL3A3;_LL3B4:;_LL3B5: {
# 2743
const char*_tmpE16;Cyc_Parse_err(((_tmpE16="bad constant in case",_tag_dyneither(_tmpE16,sizeof(char),21))),(unsigned int)(yyyylsp[0]).first_line);}_LL3A3:;}
# 2746
break;}case 381: _LL3A2: {
# 2748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2747 "parse.y"
{struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct _tmpE19;struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpE18;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18[0]=((_tmpE19.tag=15,((_tmpE19.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),_tmpE19)))),_tmpE18)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 382: _LL3B6: {
# 2750
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2752
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2749 "parse.y"
{const char*_tmpE1A;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]),((_tmpE1A="as",_tag_dyneither(_tmpE1A,sizeof(char),3))))!= 0){
const char*_tmpE1B;Cyc_Parse_err(((_tmpE1B="expecting `as'",_tag_dyneither(_tmpE1B,sizeof(char),15))),(unsigned int)(yyyylsp[1]).first_line);}}
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE25;struct _dyneither_ptr*_tmpE24;struct _tuple0*_tmpE23;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE22;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22[0]=((_tmpE25.tag=1,((_tmpE25.f1=Cyc_Absyn_new_vardecl(((_tmpE23=_cycalloc(sizeof(*_tmpE23)),((_tmpE23->f1=Cyc_Absyn_Loc_n,((_tmpE23->f2=((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE24)))),_tmpE23)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE25.f2=
Cyc_yyget_YY12(yyyyvsp[2]),_tmpE25)))))),_tmpE22)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2754
break;}case 383: _LL3B7: {
# 2756
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2758
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2755 "parse.y"
{const char*_tmpE26;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]),((_tmpE26="alias",_tag_dyneither(_tmpE26,sizeof(char),6))))!= 0){
const char*_tmpE27;Cyc_Parse_err(((_tmpE27="expecting `alias'",_tag_dyneither(_tmpE27,sizeof(char),18))),(unsigned int)(yyyylsp[1]).first_line);}}{
int _tmp7A0=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpE2F;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct _tmpE2E;struct _dyneither_ptr*_tmpE2D;struct Cyc_Absyn_Tvar*_tmpE2C;struct Cyc_Absyn_Tvar*tv=(_tmpE2C=_cycalloc(sizeof(*_tmpE2C)),((_tmpE2C->name=((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpE2D)))),((_tmpE2C->identity=- 1,((_tmpE2C->kind=(void*)((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F[0]=((_tmpE2E.tag=0,((_tmpE2E.f1=& Cyc_Tcutil_rk,_tmpE2E)))),_tmpE2F)))),_tmpE2C)))))));
struct _dyneither_ptr*_tmpE32;struct _tuple0*_tmpE31;struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->f1=Cyc_Absyn_Loc_n,((_tmpE31->f2=((_tmpE32=_cycalloc(sizeof(*_tmpE32)),((_tmpE32[0]=Cyc_yyget_String_tok(yyyyvsp[5]),_tmpE32)))),_tmpE31)))))),
Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[4]),(unsigned int)(yyyylsp[4]).first_line),0);
{struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct _tmpE35;struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpE34;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE34=_cycalloc(sizeof(*_tmpE34)),((_tmpE34[0]=((_tmpE35.tag=2,((_tmpE35.f1=tv,((_tmpE35.f2=vd,_tmpE35)))))),_tmpE34)))),(unsigned int)_tmp7A0));}
# 2763
break;};}case 384: _LL3B8: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2764 "parse.y"
struct _tuple22 _tmp7A9=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp7AB;int _tmp7AC;struct _tuple22 _tmp7AA=_tmp7A9;_tmp7AB=_tmp7AA.f1;_tmp7AC=_tmp7AA.f2;
{struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct _tmpE38;struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpE37;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE37=_cycalloc(sizeof(*_tmpE37)),((_tmpE37[0]=((_tmpE38.tag=5,((_tmpE38.f1=_tmp7AB,((_tmpE38.f2=_tmp7AC,_tmpE38)))))),_tmpE37)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2767
break;}case 385: _LL3B9: {
# 2769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2768 "parse.y"
struct _tuple22 _tmp7AF=*Cyc_yyget_YY13(yyyyvsp[2]);struct Cyc_List_List*_tmp7B1;int _tmp7B2;struct _tuple22 _tmp7B0=_tmp7AF;_tmp7B1=_tmp7B0.f1;_tmp7B2=_tmp7B0.f2;
{struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct _tmpE3B;struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpE3A;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE3A=_cycalloc(sizeof(*_tmpE3A)),((_tmpE3A[0]=((_tmpE3B.tag=16,((_tmpE3B.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpE3B.f2=_tmp7B1,((_tmpE3B.f3=_tmp7B2,_tmpE3B)))))))),_tmpE3A)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2771
break;}case 386: _LL3BA: {
# 2773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2772 "parse.y"
struct _tuple22 _tmp7B5=*Cyc_yyget_YY17(yyyyvsp[3]);struct Cyc_List_List*_tmp7B7;int _tmp7B8;struct _tuple22 _tmp7B6=_tmp7B5;_tmp7B7=_tmp7B6.f1;_tmp7B8=_tmp7B6.f2;{
struct Cyc_List_List*_tmp7B9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[2]).first_line,Cyc_yyget_YY43(yyyyvsp[2]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE41;struct Cyc_Absyn_AggrInfo*_tmpE40;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE3F;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F[0]=((_tmpE41.tag=7,((_tmpE41.f1=((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0),((_tmpE40->targs=0,_tmpE40)))))),((_tmpE41.f2=_tmp7B9,((_tmpE41.f3=_tmp7B7,((_tmpE41.f4=_tmp7B8,_tmpE41)))))))))),_tmpE3F)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2777
break;};}case 387: _LL3BB: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2778 "parse.y"
struct _tuple22 _tmp7BD=*Cyc_yyget_YY17(yyyyvsp[2]);struct Cyc_List_List*_tmp7BF;int _tmp7C0;struct _tuple22 _tmp7BE=_tmp7BD;_tmp7BF=_tmp7BE.f1;_tmp7C0=_tmp7BE.f2;{
struct Cyc_List_List*_tmp7C1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,(unsigned int)(yyyylsp[1]).first_line,Cyc_yyget_YY43(yyyyvsp[1]));
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmpE44;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpE43;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE43=_cycalloc(sizeof(*_tmpE43)),((_tmpE43[0]=((_tmpE44.tag=7,((_tmpE44.f1=0,((_tmpE44.f2=_tmp7C1,((_tmpE44.f3=_tmp7BF,((_tmpE44.f4=_tmp7C0,_tmpE44)))))))))),_tmpE43)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2782
break;};}case 388: _LL3BC: {
# 2784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2783 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE47;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE46;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE46=_cycalloc(sizeof(*_tmpE46)),((_tmpE46[0]=((_tmpE47.tag=6,((_tmpE47.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE47)))),_tmpE46)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 389: _LL3BD: {
# 2786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2785 "parse.y"
{struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE51;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct _tmpE50;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE4F;struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpE4E;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE4E=_cycalloc(sizeof(*_tmpE4E)),((_tmpE4E[0]=((_tmpE51.tag=6,((_tmpE51.f1=Cyc_Absyn_new_pat((void*)((_tmpE4F=_cycalloc(sizeof(*_tmpE4F)),((_tmpE4F[0]=((_tmpE50.tag=6,((_tmpE50.f1=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE50)))),_tmpE4F)))),(unsigned int)(yyyylsp[0]).first_line),_tmpE51)))),_tmpE4E)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 390: _LL3BE: {
# 2788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2787 "parse.y"
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE5B;struct _dyneither_ptr*_tmpE5A;struct _tuple0*_tmpE59;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE58;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE58=_cycalloc(sizeof(*_tmpE58)),((_tmpE58[0]=((_tmpE5B.tag=3,((_tmpE5B.f1=Cyc_Absyn_new_vardecl(((_tmpE59=_cycalloc(sizeof(*_tmpE59)),((_tmpE59->f1=Cyc_Absyn_Loc_n,((_tmpE59->f2=((_tmpE5A=_cycalloc(sizeof(*_tmpE5A)),((_tmpE5A[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE5A)))),_tmpE59)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE5B.f2=
# 2789
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line),_tmpE5B)))))),_tmpE58)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2791
break;}case 391: _LL3BF: {
# 2793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2792 "parse.y"
{const char*_tmpE5C;if(Cyc_strcmp((struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]),((_tmpE5C="as",_tag_dyneither(_tmpE5C,sizeof(char),3))))!= 0){
const char*_tmpE5D;Cyc_Parse_err(((_tmpE5D="expecting `as'",_tag_dyneither(_tmpE5D,sizeof(char),15))),(unsigned int)(yyyylsp[2]).first_line);}}
{struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct _tmpE67;struct _dyneither_ptr*_tmpE66;struct _tuple0*_tmpE65;struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpE64;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE64=_cycalloc(sizeof(*_tmpE64)),((_tmpE64[0]=((_tmpE67.tag=3,((_tmpE67.f1=Cyc_Absyn_new_vardecl(((_tmpE65=_cycalloc(sizeof(*_tmpE65)),((_tmpE65->f1=Cyc_Absyn_Loc_n,((_tmpE65->f2=((_tmpE66=_cycalloc(sizeof(*_tmpE66)),((_tmpE66[0]=Cyc_yyget_String_tok(yyyyvsp[1]),_tmpE66)))),_tmpE65)))))),(void*)& Cyc_Absyn_VoidType_val,0),((_tmpE67.f2=
# 2796
Cyc_yyget_YY12(yyyyvsp[3]),_tmpE67)))))),_tmpE64)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2798
break;}case 392: _LL3C0: {
# 2800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2799 "parse.y"
void*_tmp7D4=Cyc_Parse_id2type(Cyc_yyget_String_tok(yyyyvsp[2]),Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE78;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE77;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE76;struct _dyneither_ptr*_tmpE75;struct _tuple0*_tmpE74;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE73;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE73=_cycalloc(sizeof(*_tmpE73)),((_tmpE73[0]=((_tmpE78.tag=4,((_tmpE78.f1=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp7D4),((_tmpE78.f2=
Cyc_Absyn_new_vardecl(((_tmpE74=_cycalloc(sizeof(*_tmpE74)),((_tmpE74->f1=Cyc_Absyn_Loc_n,((_tmpE74->f2=((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE75)))),_tmpE74)))))),(void*)(
(_tmpE76=_cycalloc(sizeof(*_tmpE76)),((_tmpE76[0]=((_tmpE77.tag=19,((_tmpE77.f1=_tmp7D4,_tmpE77)))),_tmpE76)))),0),_tmpE78)))))),_tmpE73)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2804
break;}case 393: _LL3C1: {
# 2806
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2805 "parse.y"
struct Cyc_Absyn_Tvar*_tmp7DB=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
{struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct _tmpE94;struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpE93;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE92;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE91;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE90;struct _dyneither_ptr*_tmpE8F;struct _tuple0*_tmpE8E;struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpE8D;yyval=Cyc_YY12(Cyc_Absyn_new_pat((void*)((_tmpE8D=_cycalloc(sizeof(*_tmpE8D)),((_tmpE8D[0]=((_tmpE94.tag=4,((_tmpE94.f1=_tmp7DB,((_tmpE94.f2=
Cyc_Absyn_new_vardecl(((_tmpE8E=_cycalloc(sizeof(*_tmpE8E)),((_tmpE8E->f1=Cyc_Absyn_Loc_n,((_tmpE8E->f2=((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpE8F)))),_tmpE8E)))))),(void*)(
(_tmpE90=_cycalloc(sizeof(*_tmpE90)),((_tmpE90[0]=((_tmpE93.tag=19,((_tmpE93.f1=(void*)((_tmpE91=_cycalloc(sizeof(*_tmpE91)),((_tmpE91[0]=((_tmpE92.tag=2,((_tmpE92.f1=_tmp7DB,_tmpE92)))),_tmpE91)))),_tmpE93)))),_tmpE90)))),0),_tmpE94)))))),_tmpE8D)))),(unsigned int)(yyyylsp[0]).first_line));}
# 2810
break;}case 394: _LL3C2: {
# 2812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2813 "parse.y"
{struct _tuple22*_tmpE95;yyval=Cyc_YY13(((_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE95->f2=0,_tmpE95)))))));}
break;}case 395: _LL3C3: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2814 "parse.y"
{struct _tuple22*_tmpE96;yyval=Cyc_YY13(((_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY14(yyyyvsp[0])),((_tmpE96->f2=1,_tmpE96)))))));}
break;}case 396: _LL3C4: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2815 "parse.y"
{struct _tuple22*_tmpE97;yyval=Cyc_YY13(((_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->f1=0,((_tmpE97->f2=1,_tmpE97)))))));}
break;}case 397: _LL3C5: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2820 "parse.y"
{struct Cyc_List_List*_tmpE98;yyval=Cyc_YY14(((_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98->hd=Cyc_yyget_YY12(yyyyvsp[0]),((_tmpE98->tl=0,_tmpE98)))))));}
break;}case 398: _LL3C6: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2822 "parse.y"
{struct Cyc_List_List*_tmpE99;yyval=Cyc_YY14(((_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->hd=Cyc_yyget_YY12(yyyyvsp[2]),((_tmpE99->tl=Cyc_yyget_YY14(yyyyvsp[0]),_tmpE99)))))));}
break;}case 399: _LL3C7: {
# 2825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2827 "parse.y"
{struct _tuple23*_tmpE9A;yyval=Cyc_YY15(((_tmpE9A=_cycalloc(sizeof(*_tmpE9A)),((_tmpE9A->f1=0,((_tmpE9A->f2=Cyc_yyget_YY12(yyyyvsp[0]),_tmpE9A)))))));}
break;}case 400: _LL3C8: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2829 "parse.y"
{struct _tuple23*_tmpE9B;yyval=Cyc_YY15(((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B->f1=Cyc_yyget_YY44(yyyyvsp[0]),((_tmpE9B->f2=Cyc_yyget_YY12(yyyyvsp[1]),_tmpE9B)))))));}
break;}case 401: _LL3C9: {
# 2832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2832 "parse.y"
{struct _tuple22*_tmpE9C;yyval=Cyc_YY17(((_tmpE9C=_cycalloc(sizeof(*_tmpE9C)),((_tmpE9C->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE9C->f2=0,_tmpE9C)))))));}
break;}case 402: _LL3CA: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2833 "parse.y"
{struct _tuple22*_tmpE9D;yyval=Cyc_YY17(((_tmpE9D=_cycalloc(sizeof(*_tmpE9D)),((_tmpE9D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY16(yyyyvsp[0])),((_tmpE9D->f2=1,_tmpE9D)))))));}
break;}case 403: _LL3CB: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2834 "parse.y"
{struct _tuple22*_tmpE9E;yyval=Cyc_YY17(((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E->f1=0,((_tmpE9E->f2=1,_tmpE9E)))))));}
break;}case 404: _LL3CC: {
# 2837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2839 "parse.y"
{struct Cyc_List_List*_tmpE9F;yyval=Cyc_YY16(((_tmpE9F=_cycalloc(sizeof(*_tmpE9F)),((_tmpE9F->hd=Cyc_yyget_YY15(yyyyvsp[0]),((_tmpE9F->tl=0,_tmpE9F)))))));}
break;}case 405: _LL3CD: {
# 2842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2841 "parse.y"
{struct Cyc_List_List*_tmpEA0;yyval=Cyc_YY16(((_tmpEA0=_cycalloc(sizeof(*_tmpEA0)),((_tmpEA0->hd=Cyc_yyget_YY15(yyyyvsp[2]),((_tmpEA0->tl=Cyc_yyget_YY16(yyyyvsp[0]),_tmpEA0)))))));}
break;}case 406: _LL3CE: {
# 2844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=yyyyvsp[0];
break;}case 407: _LL3CF: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2849 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_seq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 408: _LL3D0: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2854 "parse.y"
yyval=yyyyvsp[0];
break;}case 409: _LL3D1: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2856 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_assignop_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY9(yyyyvsp[1]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 410: _LL3D2: {
# 2859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2858 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_swap_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 411: _LL3D3: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2862 "parse.y"
yyval=Cyc_YY9(0);
break;}case 412: _LL3D4: {
# 2865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2863 "parse.y"
{struct Cyc_Core_Opt*_tmpEA1;yyval=Cyc_YY9(((_tmpEA1=_cycalloc_atomic(sizeof(*_tmpEA1)),((_tmpEA1->v=(void*)Cyc_Absyn_Times,_tmpEA1)))));}
break;}case 413: _LL3D5: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2864 "parse.y"
{struct Cyc_Core_Opt*_tmpEA2;yyval=Cyc_YY9(((_tmpEA2=_cycalloc_atomic(sizeof(*_tmpEA2)),((_tmpEA2->v=(void*)Cyc_Absyn_Div,_tmpEA2)))));}
break;}case 414: _LL3D6: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2865 "parse.y"
{struct Cyc_Core_Opt*_tmpEA3;yyval=Cyc_YY9(((_tmpEA3=_cycalloc_atomic(sizeof(*_tmpEA3)),((_tmpEA3->v=(void*)Cyc_Absyn_Mod,_tmpEA3)))));}
break;}case 415: _LL3D7: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2866 "parse.y"
{struct Cyc_Core_Opt*_tmpEA4;yyval=Cyc_YY9(((_tmpEA4=_cycalloc_atomic(sizeof(*_tmpEA4)),((_tmpEA4->v=(void*)Cyc_Absyn_Plus,_tmpEA4)))));}
break;}case 416: _LL3D8: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2867 "parse.y"
{struct Cyc_Core_Opt*_tmpEA5;yyval=Cyc_YY9(((_tmpEA5=_cycalloc_atomic(sizeof(*_tmpEA5)),((_tmpEA5->v=(void*)Cyc_Absyn_Minus,_tmpEA5)))));}
break;}case 417: _LL3D9: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2868 "parse.y"
{struct Cyc_Core_Opt*_tmpEA6;yyval=Cyc_YY9(((_tmpEA6=_cycalloc_atomic(sizeof(*_tmpEA6)),((_tmpEA6->v=(void*)Cyc_Absyn_Bitlshift,_tmpEA6)))));}
break;}case 418: _LL3DA: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2869 "parse.y"
{struct Cyc_Core_Opt*_tmpEA7;yyval=Cyc_YY9(((_tmpEA7=_cycalloc_atomic(sizeof(*_tmpEA7)),((_tmpEA7->v=(void*)Cyc_Absyn_Bitlrshift,_tmpEA7)))));}
break;}case 419: _LL3DB: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2870 "parse.y"
{struct Cyc_Core_Opt*_tmpEA8;yyval=Cyc_YY9(((_tmpEA8=_cycalloc_atomic(sizeof(*_tmpEA8)),((_tmpEA8->v=(void*)Cyc_Absyn_Bitand,_tmpEA8)))));}
break;}case 420: _LL3DC: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2871 "parse.y"
{struct Cyc_Core_Opt*_tmpEA9;yyval=Cyc_YY9(((_tmpEA9=_cycalloc_atomic(sizeof(*_tmpEA9)),((_tmpEA9->v=(void*)Cyc_Absyn_Bitxor,_tmpEA9)))));}
break;}case 421: _LL3DD: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2872 "parse.y"
{struct Cyc_Core_Opt*_tmpEAA;yyval=Cyc_YY9(((_tmpEAA=_cycalloc_atomic(sizeof(*_tmpEAA)),((_tmpEAA->v=(void*)Cyc_Absyn_Bitor,_tmpEAA)))));}
break;}case 422: _LL3DE: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2877 "parse.y"
yyval=yyyyvsp[0];
break;}case 423: _LL3DF: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2879 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_conditional_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 424: _LL3E0: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2882 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_throw_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 425: _LL3E1: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2885 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 426: _LL3E2: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2887 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(0,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 427: _LL3E3: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2889 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 428: _LL3E4: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 2891 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_New_exp(Cyc_yyget_YY4(yyyyvsp[2]),Cyc_yyget_YY4(yyyyvsp[4]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 429: _LL3E5: {
# 2894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2895 "parse.y"
yyval=yyyyvsp[0];
break;}case 430: _LL3E6: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2899 "parse.y"
yyval=yyyyvsp[0];
break;}case 431: _LL3E7: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2901 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_or_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 432: _LL3E8: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2905 "parse.y"
yyval=yyyyvsp[0];
break;}case 433: _LL3E9: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2907 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_and_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 434: _LL3EA: {
# 2910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2911 "parse.y"
yyval=yyyyvsp[0];
break;}case 435: _LL3EB: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2913 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 436: _LL3EC: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2917 "parse.y"
yyval=yyyyvsp[0];
break;}case 437: _LL3ED: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2919 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 438: _LL3EE: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2923 "parse.y"
yyval=yyyyvsp[0];
break;}case 439: _LL3EF: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2925 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 440: _LL3F0: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2929 "parse.y"
yyval=yyyyvsp[0];
break;}case 441: _LL3F1: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2931 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_eq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 442: _LL3F2: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2933 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_neq_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 443: _LL3F3: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2937 "parse.y"
yyval=yyyyvsp[0];
break;}case 444: _LL3F4: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2939 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 445: _LL3F5: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2941 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gt_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 446: _LL3F6: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2943 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_lte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 447: _LL3F7: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2945 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_gte_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 448: _LL3F8: {
# 2948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2949 "parse.y"
yyval=yyyyvsp[0];
break;}case 449: _LL3F9: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2951 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 450: _LL3FA: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2953 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 451: _LL3FB: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2957 "parse.y"
yyval=yyyyvsp[0];
break;}case 452: _LL3FC: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2959 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 453: _LL3FD: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2961 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 454: _LL3FE: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2965 "parse.y"
yyval=yyyyvsp[0];
break;}case 455: _LL3FF: {
# 2968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2967 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 456: _LL400: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2969 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 457: _LL401: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 2971 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 458: _LL402: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2975 "parse.y"
yyval=yyyyvsp[0];
break;}case 459: _LL403: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2977 "parse.y"
void*_tmp7FA=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[1]),(unsigned int)(yyyylsp[1]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_cast_exp(_tmp7FA,Cyc_yyget_YY4(yyyyvsp[3]),1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line));
# 2980
break;}case 460: _LL404: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 2983 "parse.y"
yyval=yyyyvsp[0];
break;}case 461: _LL405: {
# 2986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2984 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_inc_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 462: _LL406: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2985 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_pre_dec_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 463: _LL407: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2986 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_address_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 464: _LL408: {
# 2989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2987 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 465: _LL409: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2988 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 466: _LL40A: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2989 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_prim1_exp(Cyc_yyget_YY8(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 467: _LL40B: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 2991 "parse.y"
void*_tmp7FB=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_sizeoftyp_exp(_tmp7FB,(unsigned int)(yyyylsp[0]).first_line));
# 2994
break;}case 468: _LL40C: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 2994 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_sizeofexp_exp(Cyc_yyget_YY4(yyyyvsp[1]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 469: _LL40D: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 2996 "parse.y"
void*_tmp7FC=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_offsetof_exp(_tmp7FC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4])),(unsigned int)(yyyylsp[0]).first_line));
# 2999
break;}case 470: _LL40E: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3001 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEB0;struct Cyc_Absyn_MallocInfo _tmpEAF;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEAE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEAE=_cycalloc(sizeof(*_tmpEAE)),((_tmpEAE[0]=((_tmpEB0.tag=33,((_tmpEB0.f1=((_tmpEAF.is_calloc=0,((_tmpEAF.rgn=0,((_tmpEAF.elt_type=0,((_tmpEAF.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEAF.fat_result=0,_tmpEAF)))))))))),_tmpEB0)))),_tmpEAE)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3003
break;}case 471: _LL40F: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3004 "parse.y"
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEB6;struct Cyc_Absyn_MallocInfo _tmpEB5;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEB4;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEB4=_cycalloc(sizeof(*_tmpEB4)),((_tmpEB4[0]=((_tmpEB6.tag=33,((_tmpEB6.f1=((_tmpEB5.is_calloc=0,((_tmpEB5.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEB5.elt_type=0,((_tmpEB5.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpEB5.fat_result=0,_tmpEB5)))))))))),_tmpEB6)))),_tmpEB4)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3006
break;}case 472: _LL410: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9);
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9);
# 3007 "parse.y"
void*_tmp803=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[6]),(unsigned int)(yyyylsp[6]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpEC0;void**_tmpEBF;struct Cyc_Absyn_MallocInfo _tmpEBE;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEBD;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEBD=_cycalloc(sizeof(*_tmpEBD)),((_tmpEBD[0]=((_tmpEC0.tag=33,((_tmpEC0.f1=((_tmpEBE.is_calloc=1,((_tmpEBE.rgn=0,((_tmpEBE.elt_type=((_tmpEBF=_cycalloc(sizeof(*_tmpEBF)),((_tmpEBF[0]=_tmp803,_tmpEBF)))),((_tmpEBE.num_elts=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEBE.fat_result=0,_tmpEBE)))))))))),_tmpEC0)))),_tmpEBD)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3010
break;}case 473: _LL411: {
# 3012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11);
# 3014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11);
# 3012 "parse.y"
void*_tmp808=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[8]),(unsigned int)(yyyylsp[8]).first_line);
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpECA;void**_tmpEC9;struct Cyc_Absyn_MallocInfo _tmpEC8;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEC7;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEC7=_cycalloc(sizeof(*_tmpEC7)),((_tmpEC7[0]=((_tmpECA.tag=33,((_tmpECA.f1=((_tmpEC8.is_calloc=1,((_tmpEC8.rgn=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpEC8.elt_type=((_tmpEC9=_cycalloc(sizeof(*_tmpEC9)),((_tmpEC9[0]=_tmp808,_tmpEC9)))),((_tmpEC8.num_elts=Cyc_yyget_YY4(yyyyvsp[4]),((_tmpEC8.fat_result=0,_tmpEC8)))))))))),_tmpECA)))),_tmpEC7)))),(unsigned int)(yyyylsp[0]).first_line));}
# 3015
break;}case 474: _LL412: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3016 "parse.y"
{struct Cyc_Absyn_Exp*_tmpECB[1];yyval=Cyc_YY4(Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,((_tmpECB[0]=Cyc_yyget_YY4(yyyyvsp[2]),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpECB,sizeof(struct Cyc_Absyn_Exp*),1)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 475: _LL413: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3018 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpED1;struct _dyneither_ptr*_tmpED0;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpECF;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED1.tag=37,((_tmpED1.f1=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpED1.f2=((_tmpED0=_cycalloc(sizeof(*_tmpED0)),((_tmpED0[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpED0)))),_tmpED1)))))),_tmpECF)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 476: _LL414: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3020 "parse.y"
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpED7;struct _dyneither_ptr*_tmpED6;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpED5;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED7.tag=37,((_tmpED7.f1=Cyc_Absyn_deref_exp(Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line),((_tmpED7.f2=((_tmpED6=_cycalloc(sizeof(*_tmpED6)),((_tmpED6[0]=Cyc_yyget_String_tok(yyyyvsp[4]),_tmpED6)))),_tmpED7)))))),_tmpED5)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 477: _LL415: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3022 "parse.y"
void*_tmp814=Cyc_Parse_type_name_to_type(Cyc_yyget_YY40(yyyyvsp[2]),(unsigned int)(yyyylsp[2]).first_line);
yyval=Cyc_YY4(Cyc_Absyn_valueof_exp(_tmp814,(unsigned int)(yyyylsp[0]).first_line));
break;}case 478: _LL416: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3025 "parse.y"
struct _tuple20 _tmp815=Cyc_yyget_Asm_tok(yyyyvsp[0]);int _tmp817;struct _dyneither_ptr _tmp818;struct _tuple20 _tmp816=_tmp815;_tmp817=_tmp816.f1;_tmp818=_tmp816.f2;
yyval=Cyc_YY4(Cyc_Absyn_asm_exp(_tmp817,_tmp818,(unsigned int)(yyyylsp[0]).first_line));
break;}case 479: _LL417: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3030 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Bitnot);
break;}case 480: _LL418: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3031 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Not);
break;}case 481: _LL419: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3032 "parse.y"
yyval=Cyc_YY8(Cyc_Absyn_Minus);
break;}case 482: _LL41A: {
# 3035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3037 "parse.y"
yyval=yyyyvsp[0];
break;}case 483: _LL41B: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3039 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_subscript_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY4(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 484: _LL41C: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3041 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),0,(unsigned int)(yyyylsp[0]).first_line));
break;}case 485: _LL41D: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3043 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_unknowncall_exp(Cyc_yyget_YY4(yyyyvsp[0]),Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 486: _LL41E: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3045 "parse.y"
{struct _dyneither_ptr*_tmpED8;yyval=Cyc_YY4(Cyc_Absyn_aggrmember_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED8)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 487: _LL41F: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3047 "parse.y"
{struct _dyneither_ptr*_tmpED9;yyval=Cyc_YY4(Cyc_Absyn_aggrarrow_exp(Cyc_yyget_YY4(yyyyvsp[0]),((_tmpED9=_cycalloc(sizeof(*_tmpED9)),((_tmpED9[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpED9)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 488: _LL420: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3049 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_inc_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 489: _LL421: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3051 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_post_dec_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 490: _LL422: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6);
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6);
# 3053 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEDC;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEDB;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=24,((_tmpEDC.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEDC.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEDC)))))),_tmpEDB)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 491: _LL423: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7);
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7);
# 3055 "parse.y"
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpEDF;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEDE;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDF.tag=24,((_tmpEDF.f1=Cyc_yyget_YY40(yyyyvsp[1]),((_tmpEDF.f2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[4])),_tmpEDF)))))),_tmpEDE)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 492: _LL424: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3060 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEE9;struct _dyneither_ptr*_tmpEE8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEE7;struct Cyc_List_List*_tmpEE6;yyval=Cyc_YY3(((_tmpEE6=_cycalloc(sizeof(*_tmpEE6)),((_tmpEE6->hd=(void*)((_tmpEE9=_cycalloc(sizeof(*_tmpEE9)),((_tmpEE9[0]=((_tmpEE7.tag=0,((_tmpEE7.f1=((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpEE8)))),_tmpEE7)))),_tmpEE9)))),((_tmpEE6->tl=0,_tmpEE6)))))));}
break;}case 493: _LL425: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3063 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEEF;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEEE;struct Cyc_List_List*_tmpEED;yyval=Cyc_YY3(((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED->hd=(void*)((_tmpEEF=_cycalloc_atomic(sizeof(*_tmpEEF)),((_tmpEEF[0]=((_tmpEEE.tag=1,((_tmpEEE.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[0])).f2,_tmpEEE)))),_tmpEEF)))),((_tmpEED->tl=0,_tmpEED)))))));}
break;}case 494: _LL426: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3065 "parse.y"
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpEF9;struct _dyneither_ptr*_tmpEF8;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpEF7;struct Cyc_List_List*_tmpEF6;yyval=Cyc_YY3(((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6->hd=(void*)((_tmpEF9=_cycalloc(sizeof(*_tmpEF9)),((_tmpEF9[0]=((_tmpEF7.tag=0,((_tmpEF7.f1=((_tmpEF8=_cycalloc(sizeof(*_tmpEF8)),((_tmpEF8[0]=Cyc_yyget_String_tok(yyyyvsp[2]),_tmpEF8)))),_tmpEF7)))),_tmpEF9)))),((_tmpEF6->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpEF6)))))));}
break;}case 495: _LL427: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3068 "parse.y"
{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmpEFF;struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct _tmpEFE;struct Cyc_List_List*_tmpEFD;yyval=Cyc_YY3(((_tmpEFD=_cycalloc(sizeof(*_tmpEFD)),((_tmpEFD->hd=(void*)((_tmpEFF=_cycalloc_atomic(sizeof(*_tmpEFF)),((_tmpEFF[0]=((_tmpEFE.tag=1,((_tmpEFE.f1=(unsigned int)(Cyc_yyget_Int_tok(yyyyvsp[2])).f2,_tmpEFE)))),_tmpEFF)))),((_tmpEFD->tl=Cyc_yyget_YY3(yyyyvsp[0]),_tmpEFD)))))));}
break;}case 496: _LL428: {
# 3071
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3073
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
yyval=Cyc_YY4(Cyc_Absyn_unknownid_exp(Cyc_yyget_QualId_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 497: _LL429: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3076 "parse.y"
yyval=yyyyvsp[0];
break;}case 498: _LL42A: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3078 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_string_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 499: _LL42B: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3080 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wstring_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 500: _LL42C: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3082 "parse.y"
yyval=yyyyvsp[1];
break;}case 501: _LL42D: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2);
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2);
# 3087 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_noinstantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 502: _LL42E: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3089 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_instantiate_exp(Cyc_yyget_YY4(yyyyvsp[0]),((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY43(yyyyvsp[3])),(unsigned int)(yyyylsp[0]).first_line));
break;}case 503: _LL42F: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4);
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4);
# 3092 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_tuple_exp(Cyc_yyget_YY6(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 504: _LL430: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3095 "parse.y"
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF02;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF01;yyval=Cyc_YY4(Cyc_Absyn_new_exp((void*)((_tmpF01=_cycalloc(sizeof(*_tmpF01)),((_tmpF01[0]=((_tmpF02.tag=28,((_tmpF02.f1=Cyc_yyget_QualId_tok(yyyyvsp[0]),((_tmpF02.f2=Cyc_yyget_YY43(yyyyvsp[2]),((_tmpF02.f3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY7(yyyyvsp[3])),((_tmpF02.f4=0,_tmpF02)))))))))),_tmpF01)))),(unsigned int)(yyyylsp[0]).first_line));}
break;}case 505: _LL431: {
# 3098
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5);
# 3100
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5);
# 3098 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_stmt_exp(Cyc_yyget_YY10(yyyyvsp[2]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 506: _LL432: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3102 "parse.y"
yyval=Cyc_YY6(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY6(yyyyvsp[0])));
break;}case 507: _LL433: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
{struct Cyc_List_List*_tmpF03;yyval=Cyc_YY6(((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03->hd=Cyc_yyget_YY4(yyyyvsp[0]),((_tmpF03->tl=0,_tmpF03)))))));}
break;}case 508: _LL434: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3);
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3);
# 3110 "parse.y"
{struct Cyc_List_List*_tmpF04;yyval=Cyc_YY6(((_tmpF04=_cycalloc(sizeof(*_tmpF04)),((_tmpF04->hd=Cyc_yyget_YY4(yyyyvsp[2]),((_tmpF04->tl=Cyc_yyget_YY6(yyyyvsp[0]),_tmpF04)))))));}
break;}case 509: _LL435: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
struct _tuple5 _tmp831=Cyc_yyget_Int_tok(yyyyvsp[0]);yyval=Cyc_YY4(Cyc_Absyn_int_exp(_tmp831.f1,_tmp831.f2,(unsigned int)(yyyylsp[0]).first_line));
break;}case 510: _LL436: {
# 3119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3117 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_char_exp(Cyc_yyget_Char_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 511: _LL437: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3118 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_wchar_exp(Cyc_yyget_String_tok(yyyyvsp[0]),(unsigned int)(yyyylsp[0]).first_line));
break;}case 512: _LL438: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3120 "parse.y"
struct _dyneither_ptr _tmp832=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp832);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp832,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3128
yyval=Cyc_YY4(Cyc_Absyn_float_exp(_tmp832,i,(unsigned int)(yyyylsp[0]).first_line));
# 3130
break;}case 513: _LL439: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3131 "parse.y"
yyval=Cyc_YY4(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
break;}case 514: _LL43A: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3135 "parse.y"
{struct _dyneither_ptr*_tmpF07;struct _tuple0*_tmpF06;yyval=Cyc_QualId_tok(((_tmpF06=_cycalloc(sizeof(*_tmpF06)),((_tmpF06->f1=Cyc_Absyn_Rel_n(0),((_tmpF06->f2=((_tmpF07=_cycalloc(sizeof(*_tmpF07)),((_tmpF07[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF07)))),_tmpF06)))))));}
break;}case 515: _LL43B: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3136 "parse.y"
yyval=yyyyvsp[0];
break;}case 516: _LL43C: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3139 "parse.y"
{struct _dyneither_ptr*_tmpF0A;struct _tuple0*_tmpF09;yyval=Cyc_QualId_tok(((_tmpF09=_cycalloc(sizeof(*_tmpF09)),((_tmpF09->f1=Cyc_Absyn_Rel_n(0),((_tmpF09->f2=((_tmpF0A=_cycalloc(sizeof(*_tmpF0A)),((_tmpF0A[0]=Cyc_yyget_String_tok(yyyyvsp[0]),_tmpF0A)))),_tmpF09)))))));}
break;}case 517: _LL43D: {
# 3142
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3144
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3140 "parse.y"
yyval=yyyyvsp[0];
break;}case 518: _LL43E: {
# 3143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3145 "parse.y"
yyval=yyyyvsp[0];
break;}case 519: _LL43F: {
# 3148
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3150
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3146 "parse.y"
yyval=yyyyvsp[0];
break;}case 520: _LL440: {
# 3149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3149 "parse.y"
yyval=yyyyvsp[0];
break;}case 521: _LL441: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3150 "parse.y"
yyval=yyyyvsp[0];
break;}case 522: _LL442: {
# 3153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3155 "parse.y"
break;}case 523: _LL443: {
# 3157
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1);
# 3159
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1);
# 3155 "parse.y"
yylex_buf->lex_curr_pos -=1;
break;}default: _LL444:
# 3160
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
yyn=(int)Cyc_yyr1[_check_known_subscript_notnull(524,yyn)];
# 413
yystate=Cyc_yypgoto[_check_known_subscript_notnull(144,yyn - 146)]+ *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));
if((yystate >= 0  && yystate <= 6932) && Cyc_yycheck[_check_known_subscript_notnull(6933,yystate)]== *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset)))
yystate=(int)Cyc_yytable[_check_known_subscript_notnull(6933,yystate)];else{
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
yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
# 431
if(yyn > - 32768  && yyn < 6932){
# 433
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 437
count=0;
# 439
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 441
if(Cyc_yycheck[_check_known_subscript_notnull(6933,x + yyn)]== x)
(sze +=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)])+ 15,count ++);}
{unsigned int _tmpF0F;unsigned int _tmpF0E;struct _dyneither_ptr _tmpF0D;char*_tmpF0C;unsigned int _tmpF0B;msg=((_tmpF0B=(unsigned int)(sze + 15),((_tmpF0C=(char*)_region_malloc(yyregion,_check_times(sizeof(char),_tmpF0B + 1)),((_tmpF0D=_tag_dyneither(_tmpF0C,sizeof(char),_tmpF0B + 1),((((_tmpF0E=_tmpF0B,((_tmpF10(& _tmpF0F,& _tmpF0E,& _tmpF0C),_tmpF0C[_tmpF0E]=(char)0)))),_tmpF0D))))))));}
{const char*_tmpF11;Cyc_strcpy(msg,((_tmpF11="parse error",_tag_dyneither(_tmpF11,sizeof(char),12))));}
# 446
if(count < 5){
# 448
count=0;
for(x=yyn < 0?- yyn: 0;x < 290 / sizeof(char*);++ x){
# 451
if(Cyc_yycheck[_check_known_subscript_notnull(6933,x + yyn)]== x){
# 453
{const char*_tmpF13;const char*_tmpF12;Cyc_strcat(msg,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)(
(_tmpF13=", expecting `",_tag_dyneither(_tmpF13,sizeof(char),14))):(struct _dyneither_ptr)(
(_tmpF12=" or `",_tag_dyneither(_tmpF12,sizeof(char),6)))));}
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(290,x)]);
{const char*_tmpF14;Cyc_strcat(msg,((_tmpF14="'",_tag_dyneither(_tmpF14,sizeof(char),2))));}
++ count;}}}
# 461
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 465
const char*_tmpF15;Cyc_yyerror(((_tmpF15="parse error",_tag_dyneither(_tmpF15,sizeof(char),12))),yystate,yychar);}}
# 467
goto yyerrlab1;
# 469
yyerrlab1:
# 471
 if(yyerrstatus == 3){
# 476
if(yychar == 0){
int _tmp840=1;_npop_handler(0);return _tmp840;}
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
 if(yyssp_offset == 0){int _tmp841=1;_npop_handler(0);return _tmp841;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 511
-- yylsp_offset;
# 526 "cyc_bison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1038,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 530
yyn +=1;
if((yyn < 0  || yyn > 6932) || Cyc_yycheck[_check_known_subscript_notnull(6933,yyn)]!= 1)goto yyerrdefault;
# 533
yyn=(int)Cyc_yytable[_check_known_subscript_notnull(6933,yyn)];
if(yyn < 0){
# 536
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 540
if(yyn == 0)goto yyerrpop;}
# 542
if(yyn == 1037){
int _tmp842=0;_npop_handler(0);return _tmp842;}
# 550
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 552
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 555
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3158 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp84C=v;int _tmp84D;char _tmp84E;struct _dyneither_ptr _tmp84F;union Cyc_Absyn_Nmspace _tmp850;struct _dyneither_ptr*_tmp851;_LL447: if((_tmp84C.Int_tok).tag != 1)goto _LL449;_tmp84D=((struct _tuple5)(_tmp84C.Int_tok).val).f2;_LL448:
{const char*_tmpF19;void*_tmpF18[1];struct Cyc_Int_pa_PrintArg_struct _tmpF17;(_tmpF17.tag=1,((_tmpF17.f1=(unsigned long)_tmp84D,((_tmpF18[0]=& _tmpF17,Cyc_fprintf(Cyc_stderr,((_tmpF19="%d",_tag_dyneither(_tmpF19,sizeof(char),3))),_tag_dyneither(_tmpF18,sizeof(void*),1)))))));}goto _LL446;_LL449: if((_tmp84C.Char_tok).tag != 2)goto _LL44B;_tmp84E=(char)(_tmp84C.Char_tok).val;_LL44A:
{const char*_tmpF1D;void*_tmpF1C[1];struct Cyc_Int_pa_PrintArg_struct _tmpF1B;(_tmpF1B.tag=1,((_tmpF1B.f1=(unsigned long)((int)_tmp84E),((_tmpF1C[0]=& _tmpF1B,Cyc_fprintf(Cyc_stderr,((_tmpF1D="%c",_tag_dyneither(_tmpF1D,sizeof(char),3))),_tag_dyneither(_tmpF1C,sizeof(void*),1)))))));}goto _LL446;_LL44B: if((_tmp84C.String_tok).tag != 3)goto _LL44D;_tmp84F=(struct _dyneither_ptr)(_tmp84C.String_tok).val;_LL44C:
{const char*_tmpF21;void*_tmpF20[1];struct Cyc_String_pa_PrintArg_struct _tmpF1F;(_tmpF1F.tag=0,((_tmpF1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp84F),((_tmpF20[0]=& _tmpF1F,Cyc_fprintf(Cyc_stderr,((_tmpF21="\"%s\"",_tag_dyneither(_tmpF21,sizeof(char),5))),_tag_dyneither(_tmpF20,sizeof(void*),1)))))));}goto _LL446;_LL44D: if((_tmp84C.QualId_tok).tag != 5)goto _LL44F;_tmp850=((struct _tuple0*)(_tmp84C.QualId_tok).val)->f1;_tmp851=((struct _tuple0*)(_tmp84C.QualId_tok).val)->f2;_LL44E: {
# 3164
struct Cyc_List_List*_tmp85B=0;
{union Cyc_Absyn_Nmspace _tmp85C=_tmp850;struct Cyc_List_List*_tmp85D;struct Cyc_List_List*_tmp85E;struct Cyc_List_List*_tmp85F;_LL452: if((_tmp85C.Rel_n).tag != 1)goto _LL454;_tmp85D=(struct Cyc_List_List*)(_tmp85C.Rel_n).val;_LL453:
 _tmp85B=_tmp85D;goto _LL451;_LL454: if((_tmp85C.Abs_n).tag != 2)goto _LL456;_tmp85E=(struct Cyc_List_List*)(_tmp85C.Abs_n).val;_LL455:
 _tmp85B=_tmp85E;goto _LL451;_LL456: if((_tmp85C.C_n).tag != 3)goto _LL458;_tmp85F=(struct Cyc_List_List*)(_tmp85C.C_n).val;_LL457:
 _tmp85B=_tmp85F;goto _LL451;_LL458: if((_tmp85C.Loc_n).tag != 4)goto _LL451;_LL459:
 goto _LL451;_LL451:;}
# 3171
for(0;_tmp85B != 0;_tmp85B=_tmp85B->tl){
const char*_tmpF25;void*_tmpF24[1];struct Cyc_String_pa_PrintArg_struct _tmpF23;(_tmpF23.tag=0,((_tmpF23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp85B->hd)),((_tmpF24[0]=& _tmpF23,Cyc_fprintf(Cyc_stderr,((_tmpF25="%s::",_tag_dyneither(_tmpF25,sizeof(char),5))),_tag_dyneither(_tmpF24,sizeof(void*),1)))))));}
{const char*_tmpF29;void*_tmpF28[1];struct Cyc_String_pa_PrintArg_struct _tmpF27;(_tmpF27.tag=0,((_tmpF27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp851),((_tmpF28[0]=& _tmpF27,Cyc_fprintf(Cyc_stderr,((_tmpF29="%s::",_tag_dyneither(_tmpF29,sizeof(char),5))),_tag_dyneither(_tmpF28,sizeof(void*),1)))))));}
goto _LL446;}_LL44F:;_LL450:
{const char*_tmpF2C;void*_tmpF2B;(_tmpF2B=0,Cyc_fprintf(Cyc_stderr,((_tmpF2C="?",_tag_dyneither(_tmpF2C,sizeof(char),2))),_tag_dyneither(_tmpF2B,sizeof(void*),0)));}goto _LL446;_LL446:;}
# 3180
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp868=_new_region("yyr");struct _RegionHandle*yyr=& _tmp868;_push_region(yyr);
Cyc_yyparse(yyr,Cyc_Lexing_from_file(f));{
struct Cyc_List_List*_tmp869=Cyc_Parse_parse_result;_npop_handler(0);return _tmp869;};
# 3183
;_pop_region(yyr);};}
