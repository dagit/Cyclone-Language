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
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 129
void*Cyc_Core_identity(void*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 54 "string.h"
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 39
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 43 "fn.h"
typedef struct Cyc_Fn_Function*Cyc_Fn_fn_t;
# 48
struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*f)(void*,void*),void*x);
# 51
struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*f)(void*));
# 54
void*Cyc_Fn_apply(struct Cyc_Fn_Function*f,void*x);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);struct _tuple0{struct _dyneither_ptr f1;struct Cyc_List_List*f2;};
# 56
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 74
struct Cyc_PP_Doc*Cyc_PP_oline_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 88
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 97
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist);
# 100
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs);
# 103
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2);
# 105
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 126
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 43 "pp.cyc"
typedef int Cyc_PP_dummy_t;struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 50
typedef void*Cyc_PP_alist_t;
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0,0};struct _tuple1{void*f1;void*f2;};
# 54
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple1 _tmpB0;struct _tuple1 _tmp2=(_tmpB0.f1=a1,((_tmpB0.f2=a2,_tmpB0)));struct _tuple1 _tmp3=_tmp2;_LL1: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp4=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f1;if(_tmp4->tag != 0)goto _LL3;}_LL2:
 return a2;_LL3: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp5=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp3.f2;if(_tmp5->tag != 0)goto _LL5;}_LL4:
 return a1;_LL5:;_LL6: {
struct Cyc_PP_Append_PP_Alist_struct _tmpB3;struct Cyc_PP_Append_PP_Alist_struct*_tmpB2;return(void*)((_tmpB2=_cycalloc(sizeof(*_tmpB2)),((_tmpB2[0]=((_tmpB3.tag=2,((_tmpB3.f1=a1,((_tmpB3.f2=a2,_tmpB3)))))),_tmpB2))));}_LL0:;}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_tmp9=y;void*_tmpC;void*_tmpE;void*_tmpF;_LL8: {struct Cyc_PP_Empty_PP_Alist_struct*_tmpA=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp9;if(_tmpA->tag != 0)goto _LLA;}_LL9:
 return l;_LLA: {struct Cyc_PP_Single_PP_Alist_struct*_tmpB=(struct Cyc_PP_Single_PP_Alist_struct*)_tmp9;if(_tmpB->tag != 1)goto _LLC;else{_tmpC=(void*)_tmpB->f1;}}_LLB: {
struct Cyc_List_List*_tmpB4;return(_tmpB4=_cycalloc(sizeof(*_tmpB4)),((_tmpB4->hd=_tmpC,((_tmpB4->tl=l,_tmpB4)))));}_LLC: {struct Cyc_PP_Append_PP_Alist_struct*_tmpD=(struct Cyc_PP_Append_PP_Alist_struct*)_tmp9;if(_tmpD->tag != 2)goto _LL7;else{_tmpE=(void*)_tmpD->f1;_tmpF=(void*)_tmpD->f2;}}_LLD:
 return Cyc_PP_list_of_alist_f(_tmpE,Cyc_PP_list_of_alist_f(_tmpF,l));_LL7:;}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*_tmp11=((struct Cyc_Xarray_Xarray*(*)(int,void*))Cyc_Xarray_create)(16,al);
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp11,al);{
int last=0;
while(last >= 0){
void*_tmp12=((void*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp11,last);void*_tmp13=_tmp12;struct _dyneither_ptr*_tmp16;void*_tmp18;void*_tmp19;_LLF: {struct Cyc_PP_Empty_PP_Alist_struct*_tmp14=(struct Cyc_PP_Empty_PP_Alist_struct*)_tmp13;if(_tmp14->tag != 0)goto _LL11;}_LL10:
 -- last;goto _LLE;_LL11: {struct Cyc_PP_Single_PP_Alist_struct*_tmp15=(struct Cyc_PP_Single_PP_Alist_struct*)_tmp13;if(_tmp15->tag != 1)goto _LL13;else{_tmp16=(struct _dyneither_ptr*)_tmp15->f1;}}_LL12:
 -- last;{const char*_tmpB8;void*_tmpB7[1];struct Cyc_String_pa_PrintArg_struct _tmpB6;(_tmpB6.tag=0,((_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp16),((_tmpB7[0]=& _tmpB6,Cyc_fprintf(f,((_tmpB8="%s",_tag_dyneither(_tmpB8,sizeof(char),3))),_tag_dyneither(_tmpB7,sizeof(void*),1)))))));}goto _LLE;_LL13: {struct Cyc_PP_Append_PP_Alist_struct*_tmp17=(struct Cyc_PP_Append_PP_Alist_struct*)_tmp13;if(_tmp17->tag != 2)goto _LLE;else{_tmp18=(void*)_tmp17->f1;_tmp19=(void*)_tmp17->f2;}}_LL14:
# 107
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp11,last,_tmp19);
if(last == ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp11)- 1)
((void(*)(struct Cyc_Xarray_Xarray*,void*))Cyc_Xarray_add)(_tmp11,_tmp18);else{
# 111
((void(*)(struct Cyc_Xarray_Xarray*,int,void*))Cyc_Xarray_set)(_tmp11,last + 1,_tmp18);}
++ last;
goto _LLE;_LLE:;}
# 116
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp11);};}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Ppstate*_tmpB9;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpB9=_cycalloc_atomic(sizeof(*_tmpB9)),((_tmpB9->ci=0,((_tmpB9->cc=0,((_tmpB9->cl=1,((_tmpB9->pw=w,((_tmpB9->epw=w,_tmpB9)))))))))))));
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpBA;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpBA=_cycalloc_atomic(sizeof(*_tmpBA)),((_tmpBA->ci=0,((_tmpBA->cc=0,((_tmpBA->cl=1,((_tmpBA->pw=w,((_tmpBA->epw=w,_tmpBA)))))))))))));
const char*_tmpBB;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpBB="",_tag_dyneither(_tmpBB,sizeof(char),1))));}
# 133
struct _tuple0*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Ppstate*_tmpBC;struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,((_tmpBC=_cycalloc_atomic(sizeof(*_tmpBC)),((_tmpBC->ci=0,((_tmpBC->cc=0,((_tmpBC->cl=1,((_tmpBC->pw=w,((_tmpBC->epw=w,_tmpBC)))))))))))));
const char*_tmpBF;struct _tuple0*_tmpBE;return(_tmpBE=_cycalloc(sizeof(*_tmpBE)),((_tmpBE->f1=(struct _dyneither_ptr)
Cyc_str_sepstr(((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->ppout),((_tmpBF="",_tag_dyneither(_tmpBF,sizeof(char),1)))),((_tmpBE->f2=
((struct Cyc_List_List*(*)(void*x))Cyc_PP_list_of_alist)(o->links),_tmpBE)))));}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _dyneither_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl == 0){
struct Cyc_Core_Opt*_tmpC0;Cyc_PP_bhashtbl=((_tmpC0=_cycalloc(sizeof(*_tmpC0)),((_tmpC0->v=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(int,int),int(*hash)(int)))Cyc_Hashtable_create)(61,Cyc_Core_intcmp,(int(*)(int))Cyc_Core_identity),_tmpC0))));}
if(i < 0){struct Cyc_Core_Failure_exn_struct _tmpC6;const char*_tmpC5;struct Cyc_Core_Failure_exn_struct*_tmpC4;(int)_throw((void*)((_tmpC4=_cycalloc(sizeof(*_tmpC4)),((_tmpC4[0]=((_tmpC6.tag=Cyc_Core_Failure,((_tmpC6.f1=((_tmpC5="nlblanks",_tag_dyneither(_tmpC5,sizeof(char),9))),_tmpC6)))),_tmpC4)))));}{
struct _handler_cons _tmp27;_push_handler(& _tmp27);{int _tmp29=0;if(setjmp(_tmp27.handler))_tmp29=1;if(!_tmp29){
{struct _dyneither_ptr _tmp2A=*((struct _dyneither_ptr*(*)(struct Cyc_Hashtable_Table*t,int key))Cyc_Hashtable_lookup)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i);_npop_handler(0);return _tmp2A;};_pop_handler();}else{void*_tmp28=(void*)_exn_thrown;void*_tmp2C=_tmp28;void*_tmp2E;_LL16: {struct Cyc_Core_Not_found_exn_struct*_tmp2D=(struct Cyc_Core_Not_found_exn_struct*)_tmp2C;if(_tmp2D->tag != Cyc_Core_Not_found)goto _LL18;}_LL17:
# 152
 if(!Cyc_PP_tex_output){
int _tmp2F=i / 8;
int _tmp30=i % 8;
int _tmp31=(2 + _tmp2F)+ _tmp30;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)_tmp31);
{char _tmpC9;char _tmpC8;struct _dyneither_ptr _tmpC7;(_tmpC7=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpC8=*((char*)_check_dyneither_subscript(_tmpC7,sizeof(char),0)),((_tmpC9='\n',((_get_dyneither_size(_tmpC7,sizeof(char))== 1  && (_tmpC8 == '\000'  && _tmpC9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpC7.curr)=_tmpC9)))))));}
{int j=0;for(0;j < _tmp2F;++ j){
char _tmpCC;char _tmpCB;struct _dyneither_ptr _tmpCA;(_tmpCA=_dyneither_ptr_plus(nlb,sizeof(char),j + 1),((_tmpCB=*((char*)_check_dyneither_subscript(_tmpCA,sizeof(char),0)),((_tmpCC='\t',((_get_dyneither_size(_tmpCA,sizeof(char))== 1  && (_tmpCB == '\000'  && _tmpCC != '\000')?_throw_arraybounds(): 1,*((char*)_tmpCA.curr)=_tmpCC)))))));}}
{int j=0;for(0;j < _tmp30;++ j){
char _tmpCF;char _tmpCE;struct _dyneither_ptr _tmpCD;(_tmpCD=_dyneither_ptr_plus(nlb,sizeof(char),(j + 1)+ _tmp2F),((_tmpCE=*((char*)_check_dyneither_subscript(_tmpCD,sizeof(char),0)),((_tmpCF=' ',((_get_dyneither_size(_tmpCD,sizeof(char))== 1  && (_tmpCE == '\000'  && _tmpCF != '\000')?_throw_arraybounds(): 1,*((char*)_tmpCD.curr)=_tmpCF)))))));}}
{struct _dyneither_ptr*_tmpD0;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpD0=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpD0[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)nlb),_tmpD0)))));}
return(struct _dyneither_ptr)nlb;}else{
# 168
int _tmp3C=3 + i;
struct _dyneither_ptr nlb=Cyc_Core_new_string((unsigned int)(_tmp3C + 1));
{char _tmpD3;char _tmpD2;struct _dyneither_ptr _tmpD1;(_tmpD1=_dyneither_ptr_plus(nlb,sizeof(char),0),((_tmpD2=*((char*)_check_dyneither_subscript(_tmpD1,sizeof(char),0)),((_tmpD3='\\',((_get_dyneither_size(_tmpD1,sizeof(char))== 1  && (_tmpD2 == '\000'  && _tmpD3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD1.curr)=_tmpD3)))))));}
{char _tmpD6;char _tmpD5;struct _dyneither_ptr _tmpD4;(_tmpD4=_dyneither_ptr_plus(nlb,sizeof(char),1),((_tmpD5=*((char*)_check_dyneither_subscript(_tmpD4,sizeof(char),0)),((_tmpD6='\\',((_get_dyneither_size(_tmpD4,sizeof(char))== 1  && (_tmpD5 == '\000'  && _tmpD6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD4.curr)=_tmpD6)))))));}
{char _tmpD9;char _tmpD8;struct _dyneither_ptr _tmpD7;(_tmpD7=_dyneither_ptr_plus(nlb,sizeof(char),2),((_tmpD8=*((char*)_check_dyneither_subscript(_tmpD7,sizeof(char),0)),((_tmpD9='\n',((_get_dyneither_size(_tmpD7,sizeof(char))== 1  && (_tmpD8 == '\000'  && _tmpD9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpD7.curr)=_tmpD9)))))));}
{int j=3;for(0;j < _tmp3C;++ j){
char _tmpDC;char _tmpDB;struct _dyneither_ptr _tmpDA;(_tmpDA=_dyneither_ptr_plus(nlb,sizeof(char),j),((_tmpDB=*((char*)_check_dyneither_subscript(_tmpDA,sizeof(char),0)),((_tmpDC='~',((_get_dyneither_size(_tmpDA,sizeof(char))== 1  && (_tmpDB == '\000'  && _tmpDC != '\000')?_throw_arraybounds(): 1,*((char*)_tmpDA.curr)=_tmpDC)))))));}}
{struct _dyneither_ptr*_tmpDD;((void(*)(struct Cyc_Hashtable_Table*t,int key,struct _dyneither_ptr*val))Cyc_Hashtable_insert)((struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_bhashtbl))->v,i,((_tmpDD=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpDD[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)nlb),_tmpDD)))));}
return(struct _dyneither_ptr)nlb;}_LL18: _tmp2E=_tmp2C;_LL19:(void)_throw(_tmp2E);_LL15:;}};};}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f1;struct _dyneither_ptr f2;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
int _tmp4B;struct _dyneither_ptr _tmp4C;struct _tuple2*_tmp4A=clo;_tmp4B=_tmp4A->f1;_tmp4C=_tmp4A->f2;{
struct Cyc_PP_Single_PP_Alist_struct*_tmpE7;struct _dyneither_ptr*_tmpE6;struct Cyc_PP_Single_PP_Alist_struct _tmpE5;struct Cyc_PP_Out*_tmpE4;return(_tmpE4=_cycalloc(sizeof(*_tmpE4)),((_tmpE4->newcc=st->cc + _tmp4B,((_tmpE4->newcl=st->cl,((_tmpE4->ppout=(void*)(
# 190
(_tmpE7=_cycalloc(sizeof(*_tmpE7)),((_tmpE7[0]=((_tmpE5.tag=1,((_tmpE5.f1=((_tmpE6=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmpE6[0]=(struct _dyneither_ptr)_tmp4C,_tmpE6)))),_tmpE5)))),_tmpE7)))),((_tmpE4->links=(void*)& Cyc_PP_Empty_link,_tmpE4)))))))));};}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _dyneither_ptr s){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)s);
struct _tuple2*_tmpEA;struct Cyc_PP_Doc*_tmpE9;return
(_tmpE9=_cycalloc(sizeof(*_tmpE9)),((_tmpE9->mwo=slen,((_tmpE9->mw=Cyc_PP_infinity,((_tmpE9->f=
# 198
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 200
(_tmpEA=_cycalloc(sizeof(*_tmpEA)),((_tmpEA->f1=slen,((_tmpEA->f2=s,_tmpEA))))))),_tmpE9)))))));}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpEB;Cyc_PP_str_hashtbl=((_tmpEB=_cycalloc(sizeof(*_tmpEB)),((_tmpEB->v=t,_tmpEB))));};}else{
# 208
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl))->v;}{
# 210
struct _handler_cons _tmp54;_push_handler(& _tmp54);{int _tmp56=0;if(setjmp(_tmp54.handler))_tmp56=1;if(!_tmp56){
{struct _dyneither_ptr*_tmpEC;struct Cyc_PP_Doc*_tmp58=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpEC=_cycalloc(sizeof(*_tmpEC)),((_tmpEC[0]=s,_tmpEC)))));_npop_handler(0);return _tmp58;};_pop_handler();}else{void*_tmp55=(void*)_exn_thrown;void*_tmp5A=_tmp55;void*_tmp5C;_LL1B: {struct Cyc_Core_Not_found_exn_struct*_tmp5B=(struct Cyc_Core_Not_found_exn_struct*)_tmp5A;if(_tmp5B->tag != Cyc_Core_Not_found)goto _LL1D;}_LL1C: {
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
{struct _dyneither_ptr*_tmpED;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpED=_cycalloc(sizeof(*_tmpED)),((_tmpED[0]=s,_tmpED)))),d);}
return d;}_LL1D: _tmp5C=_tmp5A;_LL1E:(void)_throw(_tmp5C);_LL1A:;}};};}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _dyneither_ptr s,int slen){
struct _tuple2*_tmpF0;struct Cyc_PP_Doc*_tmpEF;return
(_tmpEF=_cycalloc(sizeof(*_tmpEF)),((_tmpEF->mwo=slen,((_tmpEF->mw=Cyc_PP_infinity,((_tmpEF->f=
# 228
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*x))Cyc_Fn_make_fn)(Cyc_PP_text_doc_f,(
# 230
(_tmpF0=_cycalloc(sizeof(*_tmpF0)),((_tmpF0->f1=slen,((_tmpF0->f2=s,_tmpF0))))))),_tmpEF)))))));}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2 == 0){
t=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);{
struct Cyc_Core_Opt*_tmpF1;Cyc_PP_str_hashtbl2=((_tmpF1=_cycalloc(sizeof(*_tmpF1)),((_tmpF1->v=t,_tmpF1))));};}else{
# 239
t=(struct Cyc_Hashtable_Table*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_str_hashtbl2))->v;}{
# 241
struct _handler_cons _tmp61;_push_handler(& _tmp61);{int _tmp63=0;if(setjmp(_tmp61.handler))_tmp63=1;if(!_tmp63){
{struct _dyneither_ptr*_tmpF2;struct Cyc_PP_Doc*_tmp65=((struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,((_tmpF2=_cycalloc(sizeof(*_tmpF2)),((_tmpF2[0]=s,_tmpF2)))));_npop_handler(0);return _tmp65;};_pop_handler();}else{void*_tmp62=(void*)_exn_thrown;void*_tmp67=_tmp62;void*_tmp69;_LL20: {struct Cyc_Core_Not_found_exn_struct*_tmp68=(struct Cyc_Core_Not_found_exn_struct*)_tmp67;if(_tmp68->tag != Cyc_Core_Not_found)goto _LL22;}_LL21: {
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
{struct _dyneither_ptr*_tmpF3;((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_PP_Doc*val))Cyc_Hashtable_insert)(t,((_tmpF3=_cycalloc(sizeof(*_tmpF3)),((_tmpF3[0]=s,_tmpF3)))),d);}
return d;}_LL22: _tmp69=_tmp67;_LL23:(void)_throw(_tmp69);_LL1F:;}};};}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc(){
if(Cyc_PP_nil_doc_opt == 0){
const char*_tmpF6;struct Cyc_Core_Opt*_tmpF5;Cyc_PP_nil_doc_opt=((_tmpF5=_cycalloc(sizeof(*_tmpF5)),((_tmpF5->v=Cyc_PP_text(((_tmpF6="",_tag_dyneither(_tmpF6,sizeof(char),1)))),_tmpF5))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_nil_doc_opt))->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc(){
if(Cyc_PP_blank_doc_opt == 0){
const char*_tmpF9;struct Cyc_Core_Opt*_tmpF8;Cyc_PP_blank_doc_opt=((_tmpF8=_cycalloc(sizeof(*_tmpF8)),((_tmpF8->v=Cyc_PP_text(((_tmpF9="",_tag_dyneither(_tmpF9,sizeof(char),1)))),_tmpF8))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_blank_doc_opt))->v;}struct _tuple3{int f1;struct _dyneither_ptr f2;struct _dyneither_ptr f3;};struct _tuple4{int f1;int f2;int f3;struct _dyneither_ptr f4;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
int _tmp70;struct _dyneither_ptr _tmp71;struct _dyneither_ptr _tmp72;struct _tuple3*_tmp6F=clo;_tmp70=_tmp6F->f1;_tmp71=_tmp6F->f2;_tmp72=_tmp6F->f3;{
struct Cyc_PP_Single_PP_Alist_struct*_tmp10C;struct _tuple4*_tmp10B;struct Cyc_PP_Single_PP_Alist_struct _tmp10A;struct Cyc_PP_Single_PP_Alist_struct*_tmp109;struct _dyneither_ptr*_tmp108;struct Cyc_PP_Single_PP_Alist_struct _tmp107;struct Cyc_PP_Out*_tmp106;return(_tmp106=_cycalloc(sizeof(*_tmp106)),((_tmp106->newcc=st->cc + _tmp70,((_tmp106->newcl=st->cl,((_tmp106->ppout=(void*)(
# 274
(_tmp109=_cycalloc(sizeof(*_tmp109)),((_tmp109[0]=((_tmp107.tag=1,((_tmp107.f1=((_tmp108=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp108[0]=(struct _dyneither_ptr)_tmp71,_tmp108)))),_tmp107)))),_tmp109)))),((_tmp106->links=(void*)(
(_tmp10C=_cycalloc(sizeof(*_tmp10C)),((_tmp10C[0]=((_tmp10A.tag=1,((_tmp10A.f1=((_tmp10B=_cycalloc(sizeof(*_tmp10B)),((_tmp10B->f1=st->cl,((_tmp10B->f2=st->cc,((_tmp10B->f3=_tmp70,((_tmp10B->f4=_tmp72,_tmp10B)))))))))),_tmp10A)))),_tmp10C)))),_tmp106)))))))));};}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _dyneither_ptr shrt,struct _dyneither_ptr full){
int slen=(int)Cyc_strlen((struct _dyneither_ptr)shrt);
struct _tuple3*_tmp10F;struct Cyc_PP_Doc*_tmp10E;return(_tmp10E=_cycalloc(sizeof(*_tmp10E)),((_tmp10E->mwo=slen,((_tmp10E->mw=Cyc_PP_infinity,((_tmp10E->f=
# 281
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*x))Cyc_Fn_make_fn)(Cyc_PP_hyperlink_f,((_tmp10F=_cycalloc(sizeof(*_tmp10F)),((_tmp10F->f1=slen,((_tmp10F->f2=shrt,((_tmp10F->f3=full,_tmp10F))))))))),_tmp10E)))))));}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Single_PP_Alist_struct*_tmp119;struct _dyneither_ptr*_tmp118;struct Cyc_PP_Single_PP_Alist_struct _tmp117;struct Cyc_PP_Out*_tmp116;return(_tmp116=_cycalloc(sizeof(*_tmp116)),((_tmp116->newcc=st->ci,((_tmp116->newcl=st->cl + 1,((_tmp116->ppout=(void*)(
# 288
(_tmp119=_cycalloc(sizeof(*_tmp119)),((_tmp119[0]=((_tmp117.tag=1,((_tmp117.f1=((_tmp118=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp118[0]=(struct _dyneither_ptr)Cyc_PP_nlblanks(st->ci),_tmp118)))),_tmp117)))),_tmp119)))),((_tmp116->links=(void*)& Cyc_PP_Empty_link,_tmp116)))))))));}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc(){
if(Cyc_PP_line_doc_opt == 0){
struct Cyc_PP_Doc*_tmp11C;struct Cyc_Core_Opt*_tmp11B;Cyc_PP_line_doc_opt=((_tmp11B=_cycalloc(sizeof(*_tmp11B)),((_tmp11B->v=((_tmp11C=_cycalloc(sizeof(*_tmp11C)),((_tmp11C->mwo=0,((_tmp11C->mw=0,((_tmp11C->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn)(Cyc_PP_line_f),_tmp11C)))))))),_tmp11B))));}
return(struct Cyc_PP_Doc*)((struct Cyc_Core_Opt*)_check_null(Cyc_PP_line_doc_opt))->v;}struct _tuple5{int f1;struct Cyc_PP_Doc*f2;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
int _tmp83;struct Cyc_PP_Doc*_tmp84;struct _tuple5*_tmp82=clo;_tmp83=_tmp82->f1;_tmp84=_tmp82->f2;{
struct Cyc_PP_Ppstate*_tmp11D;struct Cyc_PP_Ppstate*st2=
(_tmp11D=_cycalloc_atomic(sizeof(*_tmp11D)),((_tmp11D->ci=st->ci + _tmp83,((_tmp11D->cc=st->cc,((_tmp11D->cl=st->cl,((_tmp11D->pw=st->pw,((_tmp11D->epw=st->epw,_tmp11D)))))))))));
# 305
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp84->f,st2);};}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
struct _tuple5*_tmp120;struct Cyc_PP_Doc*_tmp11F;return(_tmp11F=_cycalloc(sizeof(*_tmp11F)),((_tmp11F->mwo=d->mwo,((_tmp11F->mw=d->mw,((_tmp11F->f=
# 310
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*x))Cyc_Fn_make_fn)(Cyc_PP_nest_f,(
# 312
(_tmp120=_cycalloc(sizeof(*_tmp120)),((_tmp120->f1=k,((_tmp120->f2=d,_tmp120))))))),_tmp11F)))))));}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f1;struct Cyc_PP_Doc*f2;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Doc*_tmp89;struct Cyc_PP_Doc*_tmp8A;struct _tuple6*_tmp88=clo;_tmp89=_tmp88->f1;_tmp8A=_tmp88->f2;{
int epw2=Cyc_PP_max(st->pw - _tmp8A->mw,st->epw - _tmp89->mwo);
struct Cyc_PP_Ppstate*_tmp121;struct Cyc_PP_Ppstate*st2=(_tmp121=_cycalloc_atomic(sizeof(*_tmp121)),((_tmp121->ci=st->ci,((_tmp121->cc=st->cc,((_tmp121->cl=st->cl,((_tmp121->pw=st->pw,((_tmp121->epw=epw2,_tmp121)))))))))));
struct Cyc_PP_Out*o1=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp89->f,st2);
struct Cyc_PP_Ppstate*_tmp122;struct Cyc_PP_Ppstate*st3=(_tmp122=_cycalloc_atomic(sizeof(*_tmp122)),((_tmp122->ci=st->ci,((_tmp122->cc=o1->newcc,((_tmp122->cl=o1->newcl,((_tmp122->pw=st->pw,((_tmp122->epw=epw2,_tmp122)))))))))));
struct Cyc_PP_Out*o2=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmp8A->f,st3);
struct Cyc_PP_Out*_tmp123;struct Cyc_PP_Out*o3=(_tmp123=_cycalloc(sizeof(*_tmp123)),((_tmp123->newcc=o2->newcc,((_tmp123->newcl=o2->newcl,((_tmp123->ppout=
# 334
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->ppout,o2->ppout),((_tmp123->links=
((void*(*)(void*a1,void*a2))Cyc_PP_append)(o1->links,o2->links),_tmp123)))))))));
return o3;};}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp126;struct Cyc_PP_Doc*_tmp125;return(_tmp125=_cycalloc(sizeof(*_tmp125)),((_tmp125->mwo=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo),((_tmp125->mw=
Cyc_PP_min(d1->mw,d1->mwo + d2->mw),((_tmp125->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_concat_f,((_tmp126=_cycalloc(sizeof(*_tmp126)),((_tmp126->f1=d1,((_tmp126->f2=d2,_tmp126))))))),_tmp125)))))));}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr l){
struct Cyc_PP_Doc*_tmp90=Cyc_PP_nil_doc();
{int i=(int)(_get_dyneither_size(l,sizeof(struct Cyc_PP_Doc*))- 1);for(0;i >= 0;-- i){
_tmp90=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_dyneither_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),_tmp90);}}
# 348
return _tmp90;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*_tmp91=ds0;for(0;_tmp91 != 0;_tmp91=_tmp91->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_tmp91->hd;
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp127;st=((_tmp127=_cycalloc_atomic(sizeof(*_tmp127)),((_tmp127->ci=st->ci,((_tmp127->cc=o->newcc,((_tmp127->cl=o->newcl,((_tmp127->pw=st->pw,((_tmp127->epw=st->epw - d->mwo,_tmp127))))))))))));}{
struct Cyc_List_List*_tmp128;os=((_tmp128=_cycalloc(sizeof(*_tmp128)),((_tmp128->hd=o,((_tmp128->tl=os,_tmp128))))));};}}{
# 361
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 369
struct Cyc_PP_Out*_tmp129;return(_tmp129=_cycalloc(sizeof(*_tmp129)),((_tmp129->newcc=newcc,((_tmp129->newcl=newcl,((_tmp129->ppout=s,((_tmp129->links=links,_tmp129)))))))));};};}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(doclist))->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*_tmp95=doclist;for(0;_tmp95 != 0;_tmp95=_tmp95->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp95->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)_tmp95->hd)->mw);
mw=mw2;
mwo=mwo2;}}{
# 387
struct Cyc_PP_Doc*_tmp12A;return(_tmp12A=_cycalloc(sizeof(*_tmp12A)),((_tmp12A->mwo=mw,((_tmp12A->mw=mwo,((_tmp12A->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*x))Cyc_Fn_make_fn)(Cyc_PP_long_cats_f,orig),_tmp12A)))))));};};}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist == 0)return Cyc_PP_nil_doc();else{
if(doclist->tl == 0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return Cyc_PP_concat((struct Cyc_PP_Doc*)doclist->hd,Cyc_PP_cats(doclist->tl));}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _dyneither_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _dyneither_ptr docs=*docs_ptr;
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st);
{struct Cyc_PP_Ppstate*_tmp12B;st=((_tmp12B=_cycalloc_atomic(sizeof(*_tmp12B)),((_tmp12B->ci=st->ci,((_tmp12B->cc=o->newcc,((_tmp12B->cl=o->newcl,((_tmp12B->pw=st->pw,((_tmp12B->epw=st->epw - d->mwo,_tmp12B))))))))))));}{
struct Cyc_List_List*_tmp12C;os=((_tmp12C=_cycalloc(sizeof(*_tmp12C)),((_tmp12C->hd=o,((_tmp12C->tl=os,_tmp12C))))));};}}{
# 408
int newcc=((struct Cyc_PP_Out*)((struct Cyc_List_List*)_check_null(os))->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(0;os != 0;os=os->tl){
s=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=((void*(*)(void*a1,void*a2))Cyc_PP_append)(((struct Cyc_PP_Out*)os->hd)->links,links);}{
# 416
struct Cyc_PP_Out*_tmp12D;return(_tmp12D=_cycalloc(sizeof(*_tmp12D)),((_tmp12D->newcc=newcc,((_tmp12D->newcl=newcl,((_tmp12D->ppout=s,((_tmp12D->links=links,_tmp12D)))))))));};};}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _dyneither_ptr docs){
int sz=(int)_get_dyneither_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz == 0){
struct Cyc_Core_Failure_exn_struct _tmp133;const char*_tmp132;struct Cyc_Core_Failure_exn_struct*_tmp131;(int)_throw((void*)((_tmp131=_cycalloc(sizeof(*_tmp131)),((_tmp131[0]=((_tmp133.tag=Cyc_Core_Failure,((_tmp133.f1=((_tmp132="cats_arr -- size zero array",_tag_dyneither(_tmp132,sizeof(char),28))),_tmp133)))),_tmp131)))));}{
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),0));
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_dyneither_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mw);
mw=mw2;
mwo=mwo2;}}{
# 432
struct _dyneither_ptr*_tmp136;struct Cyc_PP_Doc*_tmp135;return(_tmp135=_cycalloc(sizeof(*_tmp135)),((_tmp135->mwo=mw,((_tmp135->mw=mwo,((_tmp135->f=((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _dyneither_ptr*,struct Cyc_PP_Ppstate*),struct _dyneither_ptr*x))Cyc_Fn_make_fn)(Cyc_PP_cats_arr_f,((_tmp136=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp136[0]=(struct _dyneither_ptr)docs,_tmp136))))),_tmp135)))))));};};}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Doc*_tmpA0;struct Cyc_PP_Doc*_tmpA1;struct _tuple6*_tmp9F=clo;_tmpA0=_tmp9F->f1;_tmpA1=_tmp9F->f2;{
int dfits=st->cc + _tmpA0->mwo <= st->epw  || st->cc + _tmpA0->mw <= st->pw;
# 439
if(dfits)return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA0->f,st);else{
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(_tmpA1->f,st);}};}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
struct _tuple6*_tmp139;struct Cyc_PP_Doc*_tmp138;return(_tmp138=_cycalloc(sizeof(*_tmp138)),((_tmp138->mwo=Cyc_PP_min(d->mwo,d2->mwo),((_tmp138->mw=
Cyc_PP_min(d->mw,d2->mw),((_tmp138->f=
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*x))Cyc_Fn_make_fn)(Cyc_PP_doc_union_f,((_tmp139=_cycalloc(sizeof(*_tmp139)),((_tmp139->f1=d,((_tmp139->f2=d2,_tmp139))))))),_tmp138)))))));}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc(){
return Cyc_PP_doc_union(Cyc_PP_nil_doc(),Cyc_PP_line_doc());}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*_tmp13A;struct Cyc_PP_Ppstate*st2=(_tmp13A=_cycalloc_atomic(sizeof(*_tmp13A)),((_tmp13A->ci=st->cc,((_tmp13A->cc=st->cc,((_tmp13A->cl=st->cl,((_tmp13A->pw=st->pw,((_tmp13A->epw=st->epw,_tmp13A)))))))))));
return((struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*f,struct Cyc_PP_Ppstate*x))Cyc_Fn_apply)(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*_tmp13B;struct Cyc_PP_Doc*d2=
(_tmp13B=_cycalloc(sizeof(*_tmp13B)),((_tmp13B->mwo=d->mwo,((_tmp13B->mw=d->mw,((_tmp13B->f=
# 461
((struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*f)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*x))Cyc_Fn_make_fn)(Cyc_PP_tab_f,d),_tmp13B)))))));
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
struct Cyc_PP_Doc*_tmp13C[4];return(_tmp13C[3]=Cyc_PP_ppseq_f(pp,sep,l->tl),((_tmp13C[2]=Cyc_PP_oline_doc(),((_tmp13C[1]=Cyc_PP_text(sep),((_tmp13C[0]=pp(l->hd),Cyc_PP_cat(_tag_dyneither(_tmp13C,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*_tmpA7=l;
while(_tmpA7->tl != 0){
struct Cyc_List_List*_tmpA8=_tmpA7->tl;
{struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp13E;_tmpA7->tl=((_tmp13E=_cycalloc(sizeof(*_tmp13E)),((_tmp13E->hd=sep2,((_tmp13E->tl=((_tmp13F=_cycalloc(sizeof(*_tmp13F)),((_tmp13F->hd=oline,((_tmp13F->tl=_tmpA8,_tmp13F)))))),_tmp13E))))));}
_tmpA7=_tmpA8;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return pp(l->hd);else{
struct Cyc_PP_Doc*_tmp140[4];return(_tmp140[3]=Cyc_PP_ppseql_f(pp,sep,l->tl),((_tmp140[2]=Cyc_PP_line_doc(),((_tmp140[1]=Cyc_PP_text(sep),((_tmp140[0]=pp(l->hd),Cyc_PP_cat(_tag_dyneither(_tmp140,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _dyneither_ptr sep,struct Cyc_List_List*l){
if(l == 0)return Cyc_PP_nil_doc();else{
if(l->tl == 0)return(struct Cyc_PP_Doc*)l->hd;else{
struct Cyc_PP_Doc*_tmp141[4];return(_tmp141[3]=Cyc_PP_seql_f(sep,l->tl),((_tmp141[2]=Cyc_PP_line_doc(),((_tmp141[1]=Cyc_PP_text(sep),((_tmp141[0]=(struct Cyc_PP_Doc*)l->hd,Cyc_PP_cat(_tag_dyneither(_tmp141,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp142[3];return(_tmp142[2]=
# 524
Cyc_PP_text(stop),((_tmp142[1]=
# 523
Cyc_PP_seq(sep,ss),((_tmp142[0]=
# 522
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp142,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
if(ss == 0)return Cyc_PP_nil_doc();else{
struct Cyc_PP_Doc*_tmp143[3];return(_tmp143[2]=
# 532
Cyc_PP_text(stop),((_tmp143[1]=
# 531
Cyc_PP_seq(sep,ss),((_tmp143[0]=
# 530
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp143,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp144[3];return(_tmp144[2]=
# 538
Cyc_PP_text(stop),((_tmp144[1]=
# 537
Cyc_PP_seql(sep,ss),((_tmp144[0]=
# 536
Cyc_PP_text(start),Cyc_PP_cat(_tag_dyneither(_tmp144,sizeof(struct Cyc_PP_Doc*),3)))))));}
