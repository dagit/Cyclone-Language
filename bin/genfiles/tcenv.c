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
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 279 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 54
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_SlowDict_Dict;
# 40 "slowdict.h"
typedef struct Cyc_SlowDict_Dict*Cyc_SlowDict_dict_t;extern char Cyc_SlowDict_Present[8];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Relations_Reln;
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 765 "absyn.h"
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 928
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 951
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 971
extern void*Cyc_Absyn_empty_effect;
# 973
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 1006
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_RgnOrder_RgnPO;
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
# 86
extern struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val;
extern struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val;
extern struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val;
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 100
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 102
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 118 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*);
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 123
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 126
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*,unsigned int,struct _dyneither_ptr*,struct Cyc_List_List*);
# 128
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 140
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 142
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 145
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 148
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 150
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 154
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 158
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 165
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 176
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 182
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 187
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 191
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 195
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 201
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 209
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 211
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 213
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 215
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 217
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 221
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 224
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 227
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 230
extern int Cyc_Tcenv_warn_override;
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 148
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 242 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 87 "tcenv.cyc"
int Cyc_Tcenv_warn_override=0;char Cyc_Tcenv_Env_error[10]="Env_error";
# 90
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 92
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
# 95
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;
# 147
struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct Cyc_Tcenv_NotLoop_j_val={0};
struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_CaseEnd_j_val={1};
struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct Cyc_Tcenv_FnEnd_j_val={2};
# 152
typedef const struct _tuple10*Cyc_Tcenv_ftclause_t;struct Cyc_Tcenv_CtrlEnv{struct _RegionHandle*ctrl_rgn;void*continue_stmt;void*break_stmt;const struct _tuple10*fallthru_clause;void*next_stmt;int try_depth;};
# 166
typedef struct Cyc_Tcenv_CtrlEnv*Cyc_Tcenv_ctrl_env_t;struct Cyc_Tcenv_SharedFenv{struct _RegionHandle*frgn;void*return_typ;struct Cyc_Dict_Dict seen_labels;struct Cyc_Dict_Dict needed_labels;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_Bindings{struct _dyneither_ptr*v;void*b;const struct Cyc_Tcenv_Bindings*tl;};
# 194 "tcenv.cyc"
typedef const struct Cyc_Tcenv_Bindings*const Cyc_Tcenv_bindings_t;
typedef const struct Cyc_Tcenv_Bindings*Cyc_Tcenv_bnds_t;struct Cyc_Tcenv_FenvFlags{int in_notreadctxt;int in_lhs;int in_new;int abstract_ok;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct Cyc_Tcenv_Bindings*locals;struct Cyc_Absyn_Stmt*encloser;struct Cyc_Tcenv_CtrlEnv*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;struct _RegionHandle*fnrgn;};char Cyc_Tcenv_NoBinding[10]="NoBinding";struct Cyc_Tcenv_NoBinding_exn_struct{char*tag;};
# 229 "tcenv.cyc"
struct Cyc_Tcenv_NoBinding_exn_struct Cyc_Tcenv_NoBinding_val={Cyc_Tcenv_NoBinding};
# 232
void*Cyc_Tcenv_lookup_binding(const struct Cyc_Tcenv_Bindings*bs,struct _dyneither_ptr*v){
for(0;(unsigned int)bs;bs=bs->tl){
if(Cyc_strptrcmp(v,bs->v)== 0)return bs->b;}
# 236
(int)_throw((void*)& Cyc_Tcenv_NoBinding_val);}
# 241
struct Cyc_Tcenv_Genv*Cyc_Tcenv_empty_genv(struct _RegionHandle*r){
struct Cyc_Tcenv_Genv*_tmp3CA;return(_tmp3CA=_region_malloc(r,sizeof(*_tmp3CA)),((_tmp3CA->grgn=r,((_tmp3CA->namespaces=
((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_rempty)(r,Cyc_strptrcmp),((_tmp3CA->aggrdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3CA->datatypedecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3CA->enumdecls=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3CA->typedefs=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3CA->ordinaries=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp3CA->availables=0,_tmp3CA)))))))))))))))));}struct _tuple12{void*f1;int f2;};
# 252
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(struct _RegionHandle*r){
# 254
{struct Cyc_Core_Opt*_tmp3CB;Cyc_Tcutil_empty_var_set=((_tmp3CB=_cycalloc(sizeof(*_tmp3CB)),((_tmp3CB->v=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp),_tmp3CB))));}{
# 256
struct Cyc_Tcenv_Genv*_tmp7=Cyc_Tcenv_empty_genv(r);
{struct Cyc_Absyn_Datatypedecl**_tmp3CC;_tmp7->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp7->datatypedecls,(*Cyc_Absyn_exn_name).f2,((_tmp3CC=_cycalloc(sizeof(*_tmp3CC)),((_tmp3CC[0]=Cyc_Absyn_exn_tud,_tmp3CC)))));}
{struct Cyc_List_List*_tmp9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(Cyc_Absyn_exn_tud->fields))->v;for(0;_tmp9 != 0;_tmp9=_tmp9->tl){
struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp3D2;struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp3D1;struct _tuple12*_tmp3D0;_tmp7->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple12*v))Cyc_Dict_insert)(_tmp7->ordinaries,(*((struct Cyc_Absyn_Datatypefield*)_tmp9->hd)->name).f2,(
(_tmp3D0=_region_malloc(r,sizeof(*_tmp3D0)),((_tmp3D0->f1=(void*)(
(_tmp3D2=_cycalloc(sizeof(*_tmp3D2)),((_tmp3D2[0]=((_tmp3D1.tag=2,((_tmp3D1.f1=Cyc_Absyn_exn_tud,((_tmp3D1.f2=(struct Cyc_Absyn_Datatypefield*)_tmp9->hd,_tmp3D1)))))),_tmp3D2)))),((_tmp3D0->f2=1,_tmp3D0)))))));}}{
# 263
struct Cyc_Dict_Dict ae=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,struct Cyc_Tcenv_Genv*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*)))Cyc_Dict_rempty)(r,Cyc_Absyn_varlist_cmp),0,_tmp7);
struct Cyc_Tcenv_Tenv*_tmp3D3;return(_tmp3D3=_region_malloc(r,sizeof(*_tmp3D3)),((_tmp3D3->ns=0,((_tmp3D3->ae=ae,((_tmp3D3->le=0,((_tmp3D3->allow_valueof=0,_tmp3D3)))))))));};};}
# 269
static struct Cyc_Tcenv_Genv*Cyc_Tcenv_lookup_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 271
return((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,Cyc_Tcenv_resolve_namespace(te,loc,n,ns));}
# 275
static struct Cyc_List_List*Cyc_Tcenv_outer_namespace(struct Cyc_List_List*ns){
if(ns == 0){
const char*_tmp3D4;return((struct Cyc_List_List*(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3D4="outer_namespace",_tag_dyneither(_tmp3D4,sizeof(char),16))));}
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);}
# 281
static int Cyc_Tcenv_same_namespace(struct Cyc_List_List*n1,struct Cyc_List_List*n2){
if(n1 == n2)return 1;
for(0;n1 != 0;n1=n1->tl){
if(n2 == 0)return 0;
if(Cyc_strptrcmp((struct _dyneither_ptr*)n1->hd,(struct _dyneither_ptr*)n2->hd)!= 0)return 0;
n2=n2->tl;}
# 288
if(n2 != 0)return 0;
return 1;}
# 294
static void Cyc_Tcenv_check_repeat(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,struct Cyc_List_List*cns,struct Cyc_List_List*nss){
# 299
for(0;nss != 0;nss=nss->tl){
if(!Cyc_Tcenv_same_namespace(cns,(struct Cyc_List_List*)nss->hd)){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)nss->hd);
struct _handler_cons _tmpF;_push_handler(& _tmpF);{int _tmp11=0;if(setjmp(_tmpF.handler))_tmp11=1;if(!_tmp11){
lookup(ge2,v,0);{
const char*_tmp3D8;void*_tmp3D7[1];struct Cyc_String_pa_PrintArg_struct _tmp3D6;(_tmp3D6.tag=0,((_tmp3D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp3D7[0]=& _tmp3D6,Cyc_Tcutil_terr(loc,((_tmp3D8="%s is ambiguous",_tag_dyneither(_tmp3D8,sizeof(char),16))),_tag_dyneither(_tmp3D7,sizeof(void*),1)))))));};
# 303
;_pop_handler();}else{void*_tmp10=(void*)_exn_thrown;void*_tmp16=_tmp10;void*_tmp18;_LL1: {struct Cyc_Dict_Absent_exn_struct*_tmp17=(struct Cyc_Dict_Absent_exn_struct*)_tmp16;if(_tmp17->tag != Cyc_Dict_Absent)goto _LL3;}_LL2:
# 305
 goto _LL0;_LL3: _tmp18=_tmp16;_LL4:(void)_throw(_tmp18);_LL0:;}};}}
# 307
return;}
# 310
static void*Cyc_Tcenv_scoped_lookup(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use){
struct Cyc_List_List*cns=te->ns;
while(1){
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,cns);
# 315
{struct _handler_cons _tmp19;_push_handler(& _tmp19);{int _tmp1B=0;if(setjmp(_tmp19.handler))_tmp1B=1;if(!_tmp1B){
{void*result=lookup(ge,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,cns,ge->availables);{
void*_tmp1C=result;_npop_handler(0);return _tmp1C;};}
# 316
;_pop_handler();}else{void*_tmp1A=(void*)_exn_thrown;void*_tmp1E=_tmp1A;void*_tmp20;_LL6: {struct Cyc_Dict_Absent_exn_struct*_tmp1F=(struct Cyc_Dict_Absent_exn_struct*)_tmp1E;if(_tmp1F->tag != Cyc_Dict_Absent)goto _LL8;}_LL7:
# 319
 goto _LL5;_LL8: _tmp20=_tmp1E;_LL9:(void)_throw(_tmp20);_LL5:;}};}
# 321
{struct Cyc_List_List*_tmp21=ge->availables;for(0;_tmp21 != 0;_tmp21=_tmp21->tl){
struct Cyc_Tcenv_Genv*ge2=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp21->hd);
struct _handler_cons _tmp22;_push_handler(& _tmp22);{int _tmp24=0;if(setjmp(_tmp22.handler))_tmp24=1;if(!_tmp24){
{void*result=lookup(ge2,v,is_use);
Cyc_Tcenv_check_repeat(te,loc,lookup,v,(struct Cyc_List_List*)_tmp21->hd,_tmp21->tl);{
void*_tmp25=result;_npop_handler(0);return _tmp25;};}
# 324
;_pop_handler();}else{void*_tmp23=(void*)_exn_thrown;void*_tmp27=_tmp23;void*_tmp29;_LLB: {struct Cyc_Dict_Absent_exn_struct*_tmp28=(struct Cyc_Dict_Absent_exn_struct*)_tmp27;if(_tmp28->tag != Cyc_Dict_Absent)goto _LLD;}_LLC:
# 327
 goto _LLA;_LLD: _tmp29=_tmp27;_LLE:(void)_throw(_tmp29);_LLA:;}};}}
# 329
if(cns == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3DB;struct Cyc_Dict_Absent_exn_struct*_tmp3DA;(int)_throw((void*)((_tmp3DA=_cycalloc_atomic(sizeof(*_tmp3DA)),((_tmp3DA[0]=((_tmp3DB.tag=Cyc_Dict_Absent,_tmp3DB)),_tmp3DA)))));}
# 332
cns=Cyc_Tcenv_outer_namespace(cns);}}
# 336
static void*Cyc_Tcenv_lookup_ordinary_global_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int is_use){
struct Cyc_Dict_Dict _tmp2C=ge->ordinaries;
struct _tuple12*ans=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp2C,v);
if(is_use)
(*ans).f2=1;
return(*ans).f1;}
# 343
static void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
union Cyc_Absyn_Nmspace _tmp2E;struct _dyneither_ptr*_tmp2F;struct _tuple0*_tmp2D=q;_tmp2E=_tmp2D->f1;_tmp2F=_tmp2D->f2;{
union Cyc_Absyn_Nmspace _tmp30=_tmp2E;struct _dyneither_ptr*_tmp31;struct Cyc_List_List*_tmp32;struct Cyc_List_List*_tmp33;struct Cyc_List_List*_tmp34;_LL10: if((_tmp30.Loc_n).tag != 4)goto _LL12;_LL11:
 goto _LL13;_LL12: if((_tmp30.Rel_n).tag != 1)goto _LL14;if((struct Cyc_List_List*)(_tmp30.Rel_n).val != 0)goto _LL14;_LL13:
# 348
 return((void*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_ordinary_global_f,_tmp2F,is_use);_LL14: if((_tmp30.Rel_n).tag != 1)goto _LL16;if((struct Cyc_List_List*)(_tmp30.Rel_n).val == 0)goto _LL16;_tmp31=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp30.Rel_n).val)->hd;_tmp32=((struct Cyc_List_List*)(_tmp30.Rel_n).val)->tl;_LL15: {
# 350
struct Cyc_Tcenv_Genv*_tmp35=Cyc_Tcenv_lookup_namespace(te,loc,_tmp31,_tmp32);
return Cyc_Tcenv_lookup_ordinary_global_f(_tmp35,_tmp2F,is_use);}_LL16: if((_tmp30.C_n).tag != 3)goto _LL18;_tmp33=(struct Cyc_List_List*)(_tmp30.C_n).val;_LL17:
 _tmp34=_tmp33;goto _LL19;_LL18: if((_tmp30.Abs_n).tag != 2)goto _LLF;_tmp34=(struct Cyc_List_List*)(_tmp30.Abs_n).val;_LL19:
# 354
 return Cyc_Tcenv_lookup_ordinary_global_f(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp34),_tmp2F,is_use);_LLF:;};}
# 359
struct Cyc_List_List*Cyc_Tcenv_resolve_namespace(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr*n,struct Cyc_List_List*ns){
# 361
struct Cyc_List_List*_tmp36=te->ns;
struct _RegionHandle _tmp37=_new_region("temp");struct _RegionHandle*temp=& _tmp37;_push_region(temp);
{struct Cyc_List_List*_tmp38=0;
while(1){
struct Cyc_Tcenv_Genv*_tmp39=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp36);
struct Cyc_List_List*_tmp3A=_tmp39->availables;
struct Cyc_Set_Set*_tmp3B=_tmp39->namespaces;
# 369
{struct Cyc_List_List*_tmp3C=_tmp3A;for(0;_tmp3C != 0;_tmp3C=_tmp3C->tl){
struct Cyc_Set_Set*_tmp3D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,(struct Cyc_List_List*)_tmp3C->hd))->namespaces;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3D,n)){
struct Cyc_List_List*_tmp3DE;struct Cyc_List_List*_tmp3DD;_tmp38=((_tmp3DD=_region_malloc(temp,sizeof(*_tmp3DD)),((_tmp3DD->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE->hd=n,((_tmp3DE->tl=ns,_tmp3DE))))))),((_tmp3DD->tl=_tmp38,_tmp3DD))))));}}}
# 376
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp3B,n)){
struct Cyc_List_List*_tmp3E1;struct Cyc_List_List*_tmp3E0;_tmp38=((_tmp3E0=_region_malloc(temp,sizeof(*_tmp3E0)),((_tmp3E0->hd=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36,((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1->hd=n,((_tmp3E1->tl=ns,_tmp3E1))))))),((_tmp3E0->tl=_tmp38,_tmp3E0))))));}
# 382
if(_tmp38 != 0){
if(_tmp38->tl != 0){
const char*_tmp3E5;void*_tmp3E4[1];struct Cyc_String_pa_PrintArg_struct _tmp3E3;(_tmp3E3.tag=0,((_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*n),((_tmp3E4[0]=& _tmp3E3,Cyc_Tcutil_terr(loc,((_tmp3E5="%s is ambiguous",_tag_dyneither(_tmp3E5,sizeof(char),16))),_tag_dyneither(_tmp3E4,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp45=(struct Cyc_List_List*)_tmp38->hd;_npop_handler(0);return _tmp45;};}
# 387
if(_tmp36 == 0){
struct Cyc_Dict_Absent_exn_struct _tmp3E8;struct Cyc_Dict_Absent_exn_struct*_tmp3E7;(int)_throw((void*)((_tmp3E7=_cycalloc_atomic(sizeof(*_tmp3E7)),((_tmp3E7[0]=((_tmp3E8.tag=Cyc_Dict_Absent,_tmp3E8)),_tmp3E7)))));}
# 390
_tmp36=Cyc_Tcenv_outer_namespace(_tmp36);}}
# 363
;_pop_region(temp);}
# 398
static struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp48=ge->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp48,v);}
# 402
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp4A;struct _dyneither_ptr*_tmp4B;struct _tuple0*_tmp49=q;_tmp4A=_tmp49->f1;_tmp4B=_tmp49->f2;{
union Cyc_Absyn_Nmspace _tmp4C=_tmp4A;struct Cyc_List_List*_tmp4D;struct Cyc_List_List*_tmp4E;struct _dyneither_ptr*_tmp4F;struct Cyc_List_List*_tmp50;_LL1B: if((_tmp4C.Loc_n).tag != 4)goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if((_tmp4C.Rel_n).tag != 1)goto _LL1F;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val != 0)goto _LL1F;_LL1E:
# 407
 return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_aggrdecl_f,_tmp4B,0);_LL1F: if((_tmp4C.C_n).tag != 3)goto _LL21;_tmp4D=(struct Cyc_List_List*)(_tmp4C.C_n).val;_LL20:
 _tmp4E=_tmp4D;goto _LL22;_LL21: if((_tmp4C.Abs_n).tag != 2)goto _LL23;_tmp4E=(struct Cyc_List_List*)(_tmp4C.Abs_n).val;_LL22: {
# 410
struct Cyc_Dict_Dict _tmp51=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp4E))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp51,_tmp4B);}_LL23: if((_tmp4C.Rel_n).tag != 1)goto _LL1A;if((struct Cyc_List_List*)(_tmp4C.Rel_n).val == 0)goto _LL1A;_tmp4F=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->hd;_tmp50=((struct Cyc_List_List*)(_tmp4C.Rel_n).val)->tl;_LL24: {
# 413
struct Cyc_Dict_Dict _tmp52=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp4F,_tmp50))->aggrdecls;
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp52,_tmp4B);}_LL1A:;};}
# 418
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp53=ge->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp53,v);}
# 422
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp55;struct _dyneither_ptr*_tmp56;struct _tuple0*_tmp54=q;_tmp55=_tmp54->f1;_tmp56=_tmp54->f2;{
union Cyc_Absyn_Nmspace _tmp57=_tmp55;struct _dyneither_ptr*_tmp58;struct Cyc_List_List*_tmp59;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp5B;_LL26: if((_tmp57.Loc_n).tag != 4)goto _LL28;_LL27:
 goto _LL29;_LL28: if((_tmp57.Rel_n).tag != 1)goto _LL2A;if((struct Cyc_List_List*)(_tmp57.Rel_n).val != 0)goto _LL2A;_LL29:
# 427
 return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_datatypedecl_f,_tmp56,0);_LL2A: if((_tmp57.Rel_n).tag != 1)goto _LL2C;if((struct Cyc_List_List*)(_tmp57.Rel_n).val == 0)goto _LL2C;_tmp58=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp57.Rel_n).val)->hd;_tmp59=((struct Cyc_List_List*)(_tmp57.Rel_n).val)->tl;_LL2B: {
# 429
struct Cyc_Dict_Dict _tmp5C=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp58,_tmp59))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5C,_tmp56);}_LL2C: if((_tmp57.C_n).tag != 3)goto _LL2E;_tmp5A=(struct Cyc_List_List*)(_tmp57.C_n).val;_LL2D:
 _tmp5B=_tmp5A;goto _LL2F;_LL2E: if((_tmp57.Abs_n).tag != 2)goto _LL25;_tmp5B=(struct Cyc_List_List*)(_tmp57.Abs_n).val;_LL2F: {
# 433
struct Cyc_Dict_Dict _tmp5D=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp5B))->datatypedecls;
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp5D,_tmp56);}_LL25:;};}
# 438
static struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_xdatatypedecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(ge->datatypedecls,v);}
# 443
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp5F;struct _dyneither_ptr*_tmp60;struct _tuple0*_tmp5E=q;_tmp5F=_tmp5E->f1;_tmp60=_tmp5E->f2;{
union Cyc_Absyn_Nmspace _tmp61=_tmp5F;struct _dyneither_ptr*_tmp62;struct Cyc_List_List*_tmp63;struct Cyc_List_List*_tmp64;struct Cyc_List_List*_tmp65;_LL31: if((_tmp61.Rel_n).tag != 1)goto _LL33;if((struct Cyc_List_List*)(_tmp61.Rel_n).val != 0)goto _LL33;_LL32: {
# 448
struct _handler_cons _tmp66;_push_handler(& _tmp66);{int _tmp68=0;if(setjmp(_tmp66.handler))_tmp68=1;if(!_tmp68){{struct Cyc_Absyn_Datatypedecl***_tmp3E9;struct Cyc_Absyn_Datatypedecl***_tmp6A=(_tmp3E9=_region_malloc(r,sizeof(*_tmp3E9)),((_tmp3E9[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_xdatatypedecl_f,_tmp60,0),_tmp3E9)));_npop_handler(0);return _tmp6A;};_pop_handler();}else{void*_tmp67=(void*)_exn_thrown;void*_tmp6C=_tmp67;void*_tmp6E;_LL3C: {struct Cyc_Dict_Absent_exn_struct*_tmp6D=(struct Cyc_Dict_Absent_exn_struct*)_tmp6C;if(_tmp6D->tag != Cyc_Dict_Absent)goto _LL3E;}_LL3D:
 return 0;_LL3E: _tmp6E=_tmp6C;_LL3F:(void)_throw(_tmp6E);_LL3B:;}};}_LL33: if((_tmp61.Loc_n).tag != 4)goto _LL35;_LL34:
# 451
{const char*_tmp3EC;void*_tmp3EB;(_tmp3EB=0,Cyc_Tcutil_terr(loc,((_tmp3EC="lookup_xdatatypedecl: impossible",_tag_dyneither(_tmp3EC,sizeof(char),33))),_tag_dyneither(_tmp3EB,sizeof(void*),0)));}
return 0;_LL35: if((_tmp61.Rel_n).tag != 1)goto _LL37;if((struct Cyc_List_List*)(_tmp61.Rel_n).val == 0)goto _LL37;_tmp62=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp61.Rel_n).val)->hd;_tmp63=((struct Cyc_List_List*)(_tmp61.Rel_n).val)->tl;_LL36: {
# 458
struct Cyc_Tcenv_Genv*ge;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){ge=Cyc_Tcenv_lookup_namespace(te,loc,_tmp62,_tmp63);;_pop_handler();}else{void*_tmp72=(void*)_exn_thrown;void*_tmp75=_tmp72;void*_tmp77;_LL41: {struct Cyc_Dict_Absent_exn_struct*_tmp76=(struct Cyc_Dict_Absent_exn_struct*)_tmp75;if(_tmp76->tag != Cyc_Dict_Absent)goto _LL43;}_LL42:
# 462
{const char*_tmp3EF;void*_tmp3EE;(_tmp3EE=0,Cyc_Tcutil_terr(loc,((_tmp3EF="bad qualified name for @extensible datatype",_tag_dyneither(_tmp3EF,sizeof(char),44))),_tag_dyneither(_tmp3EE,sizeof(void*),0)));}{
struct Cyc_Dict_Absent_exn_struct _tmp3F2;struct Cyc_Dict_Absent_exn_struct*_tmp3F1;(int)_throw((void*)((_tmp3F1=_cycalloc_atomic(sizeof(*_tmp3F1)),((_tmp3F1[0]=((_tmp3F2.tag=Cyc_Dict_Absent,_tmp3F2)),_tmp3F1)))));};_LL43: _tmp77=_tmp75;_LL44:(void)_throw(_tmp77);_LL40:;}};}{
# 465
struct Cyc_Dict_Dict _tmp7C=ge->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3F3;return(_tmp3F3=_region_malloc(r,sizeof(*_tmp3F3)),((_tmp3F3[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7C,_tmp60),_tmp3F3)));};}_LL37: if((_tmp61.C_n).tag != 3)goto _LL39;_tmp64=(struct Cyc_List_List*)(_tmp61.C_n).val;_LL38:
 _tmp65=_tmp64;goto _LL3A;_LL39: if((_tmp61.Abs_n).tag != 2)goto _LL30;_tmp65=(struct Cyc_List_List*)(_tmp61.Abs_n).val;_LL3A: {
# 469
struct Cyc_Dict_Dict _tmp7E=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp65))->datatypedecls;
struct Cyc_Absyn_Datatypedecl***_tmp3F4;return(_tmp3F4=_region_malloc(r,sizeof(*_tmp3F4)),((_tmp3F4[0]=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp7E,_tmp60),_tmp3F4)));}_LL30:;};}
# 474
static struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp80=ge->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp80,v);}
# 478
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp82;struct _dyneither_ptr*_tmp83;struct _tuple0*_tmp81=q;_tmp82=_tmp81->f1;_tmp83=_tmp81->f2;{
union Cyc_Absyn_Nmspace _tmp84=_tmp82;struct _dyneither_ptr*_tmp85;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp88;_LL46: if((_tmp84.Loc_n).tag != 4)goto _LL48;_LL47:
 goto _LL49;_LL48: if((_tmp84.Rel_n).tag != 1)goto _LL4A;if((struct Cyc_List_List*)(_tmp84.Rel_n).val != 0)goto _LL4A;_LL49:
# 483
 return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl**(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_enumdecl_f,_tmp83,0);_LL4A: if((_tmp84.Rel_n).tag != 1)goto _LL4C;if((struct Cyc_List_List*)(_tmp84.Rel_n).val == 0)goto _LL4C;_tmp85=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp84.Rel_n).val)->hd;_tmp86=((struct Cyc_List_List*)(_tmp84.Rel_n).val)->tl;_LL4B: {
# 485
struct Cyc_Dict_Dict _tmp89=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp85,_tmp86))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp89,_tmp83);}_LL4C: if((_tmp84.C_n).tag != 3)goto _LL4E;_tmp87=(struct Cyc_List_List*)(_tmp84.C_n).val;_LL4D:
 _tmp88=_tmp87;goto _LL4F;_LL4E: if((_tmp84.Abs_n).tag != 2)goto _LL45;_tmp88=(struct Cyc_List_List*)(_tmp84.Abs_n).val;_LL4F: {
# 489
struct Cyc_Dict_Dict _tmp8A=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp88))->enumdecls;
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8A,_tmp83);}_LL45:;};}
# 494
static struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl_f(struct Cyc_Tcenv_Genv*ge,struct _dyneither_ptr*v,int ignore){
struct Cyc_Dict_Dict _tmp8B=ge->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp8B,v);}
# 498
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
union Cyc_Absyn_Nmspace _tmp8D;struct _dyneither_ptr*_tmp8E;struct _tuple0*_tmp8C=q;_tmp8D=_tmp8C->f1;_tmp8E=_tmp8C->f2;{
union Cyc_Absyn_Nmspace _tmp8F=_tmp8D;struct _dyneither_ptr*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_List_List*_tmp92;struct Cyc_List_List*_tmp93;_LL51: if((_tmp8F.Loc_n).tag != 4)goto _LL53;_LL52:
 goto _LL54;_LL53: if((_tmp8F.Rel_n).tag != 1)goto _LL55;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val != 0)goto _LL55;_LL54:
# 503
 return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Tcenv_Genv*,struct _dyneither_ptr*,int),struct _dyneither_ptr*v,int is_use))Cyc_Tcenv_scoped_lookup)(te,loc,Cyc_Tcenv_lookup_typedefdecl_f,_tmp8E,0);_LL55: if((_tmp8F.Rel_n).tag != 1)goto _LL57;if((struct Cyc_List_List*)(_tmp8F.Rel_n).val == 0)goto _LL57;_tmp90=(struct _dyneither_ptr*)((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->hd;_tmp91=((struct Cyc_List_List*)(_tmp8F.Rel_n).val)->tl;_LL56: {
# 505
struct Cyc_Dict_Dict _tmp94=(Cyc_Tcenv_lookup_namespace(te,loc,_tmp90,_tmp91))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp94,_tmp8E);}_LL57: if((_tmp8F.C_n).tag != 3)goto _LL59;_tmp92=(struct Cyc_List_List*)(_tmp8F.C_n).val;_LL58:
 _tmp93=_tmp92;goto _LL5A;_LL59: if((_tmp8F.Abs_n).tag != 2)goto _LL50;_tmp93=(struct Cyc_List_List*)(_tmp8F.Abs_n).val;_LL5A: {
# 509
struct Cyc_Dict_Dict _tmp95=(((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,_tmp93))->typedefs;
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(_tmp95,_tmp8E);}_LL50:;};}
# 516
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp96=te->le;
if(_tmp96 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp96;}
# 522
struct _RegionHandle*Cyc_Tcenv_coerce_heap_region(){
return Cyc_Core_heap_region;}
# 526
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp97=te->le;
if(_tmp97 != 0){
struct Cyc_Tcenv_Fenv*_tmp98=_tmp97;struct _RegionHandle*_tmp9A;struct Cyc_Tcenv_Fenv*_tmp99=_tmp98;_tmp9A=_tmp99->fnrgn;
return _tmp9A;}
# 532
return Cyc_Tcenv_coerce_heap_region();}
# 535
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0){const char*_tmp3F5;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp3F5="put_fenv",_tag_dyneither(_tmp3F5,sizeof(char),9))));}{
struct Cyc_Tcenv_Tenv*_tmp3F6;return(_tmp3F6=_region_malloc(l,sizeof(*_tmp3F6)),((_tmp3F6->ns=te->ns,((_tmp3F6->ae=te->ae,((_tmp3F6->le=fe,((_tmp3F6->allow_valueof=te->allow_valueof,_tmp3F6)))))))));};}
# 540
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct _RegionHandle*l,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmp3F7;return(_tmp3F7=_region_malloc(l,sizeof(*_tmp3F7)),((_tmp3F7->ns=te->ns,((_tmp3F7->ae=te->ae,((_tmp3F7->le=0,((_tmp3F7->allow_valueof=te->allow_valueof,_tmp3F7)))))))));}
# 544
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_old_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 546
struct Cyc_Tcenv_Fenv _tmp9E=*f;struct Cyc_Tcenv_SharedFenv*_tmpA0;struct Cyc_List_List*_tmpA1;struct Cyc_RgnOrder_RgnPO*_tmpA2;const struct Cyc_Tcenv_Bindings*_tmpA3;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Tcenv_CtrlEnv*_tmpA5;void*_tmpA6;void*_tmpA7;struct Cyc_Tcenv_FenvFlags _tmpA8;struct _RegionHandle*_tmpA9;struct Cyc_Tcenv_Fenv _tmp9F=_tmp9E;_tmpA0=_tmp9F.shared;_tmpA1=_tmp9F.type_vars;_tmpA2=_tmp9F.region_order;_tmpA3=_tmp9F.locals;_tmpA4=_tmp9F.encloser;_tmpA5=_tmp9F.ctrl_env;_tmpA6=_tmp9F.capability;_tmpA7=_tmp9F.curr_rgn;_tmpA8=_tmp9F.flags;_tmpA9=_tmp9F.fnrgn;{
# 548
struct Cyc_Tcenv_Fenv*_tmp3F8;return(_tmp3F8=_region_malloc(l,sizeof(*_tmp3F8)),((_tmp3F8->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpA0,((_tmp3F8->type_vars=(struct Cyc_List_List*)_tmpA1,((_tmp3F8->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpA2,((_tmp3F8->locals=(const struct Cyc_Tcenv_Bindings*)_tmpA3,((_tmp3F8->encloser=(struct Cyc_Absyn_Stmt*)_tmpA4,((_tmp3F8->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpA5,((_tmp3F8->capability=(void*)_tmpA6,((_tmp3F8->curr_rgn=(void*)_tmpA7,((_tmp3F8->flags=(struct Cyc_Tcenv_FenvFlags)_tmpA8,((_tmp3F8->fnrgn=(struct _RegionHandle*)l,_tmp3F8)))))))))))))))))))));};}
# 552
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_copy_fenv_new_ctrl(struct _RegionHandle*l,struct Cyc_Tcenv_Fenv*f){
# 554
struct Cyc_Tcenv_Fenv _tmpAB=*f;struct Cyc_Tcenv_SharedFenv*_tmpAD;struct Cyc_List_List*_tmpAE;struct Cyc_RgnOrder_RgnPO*_tmpAF;const struct Cyc_Tcenv_Bindings*_tmpB0;struct Cyc_Absyn_Stmt*_tmpB1;struct Cyc_Tcenv_CtrlEnv*_tmpB2;void*_tmpB3;void*_tmpB4;struct Cyc_Tcenv_FenvFlags _tmpB5;struct Cyc_Tcenv_Fenv _tmpAC=_tmpAB;_tmpAD=_tmpAC.shared;_tmpAE=_tmpAC.type_vars;_tmpAF=_tmpAC.region_order;_tmpB0=_tmpAC.locals;_tmpB1=_tmpAC.encloser;_tmpB2=_tmpAC.ctrl_env;_tmpB3=_tmpAC.capability;_tmpB4=_tmpAC.curr_rgn;_tmpB5=_tmpAC.flags;{
# 557
struct Cyc_Tcenv_CtrlEnv _tmpB6=*_tmpB2;struct _RegionHandle*_tmpB8;void*_tmpB9;void*_tmpBA;const struct _tuple10*_tmpBB;void*_tmpBC;int _tmpBD;struct Cyc_Tcenv_CtrlEnv _tmpB7=_tmpB6;_tmpB8=_tmpB7.ctrl_rgn;_tmpB9=_tmpB7.continue_stmt;_tmpBA=_tmpB7.break_stmt;_tmpBB=_tmpB7.fallthru_clause;_tmpBC=_tmpB7.next_stmt;_tmpBD=_tmpB7.try_depth;{
struct Cyc_Tcenv_CtrlEnv*_tmp3F9;struct Cyc_Tcenv_CtrlEnv*_tmpBE=
(_tmp3F9=_region_malloc(l,sizeof(*_tmp3F9)),((_tmp3F9->ctrl_rgn=_tmpB8,((_tmp3F9->continue_stmt=_tmpB9,((_tmp3F9->break_stmt=_tmpBA,((_tmp3F9->fallthru_clause=_tmpBB,((_tmp3F9->next_stmt=_tmpBC,((_tmp3F9->try_depth=_tmpBD,_tmp3F9)))))))))))));
struct Cyc_Tcenv_Fenv*_tmp3FA;return(_tmp3FA=_region_malloc(l,sizeof(*_tmp3FA)),((_tmp3FA->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpAD,((_tmp3FA->type_vars=(struct Cyc_List_List*)_tmpAE,((_tmp3FA->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpAF,((_tmp3FA->locals=(const struct Cyc_Tcenv_Bindings*)_tmpB0,((_tmp3FA->encloser=(struct Cyc_Absyn_Stmt*)_tmpB1,((_tmp3FA->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpBE,((_tmp3FA->capability=(void*)_tmpB3,((_tmp3FA->curr_rgn=(void*)_tmpB4,((_tmp3FA->flags=(struct Cyc_Tcenv_FenvFlags)_tmpB5,((_tmp3FA->fnrgn=(struct _RegionHandle*)l,_tmp3FA)))))))))))))))))))));};};}
# 566
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
const char*_tmp3FB;struct Cyc_Tcenv_Fenv*_tmpC1=Cyc_Tcenv_get_fenv(te,((_tmp3FB="return_typ",_tag_dyneither(_tmp3FB,sizeof(char),11))));struct Cyc_Tcenv_SharedFenv*_tmpC3;struct Cyc_Tcenv_Fenv*_tmpC2=_tmpC1;_tmpC3=_tmpC2->shared;{
struct Cyc_Tcenv_SharedFenv _tmpC4=*_tmpC3;void*_tmpC6;struct Cyc_Tcenv_SharedFenv _tmpC5=_tmpC4;_tmpC6=_tmpC5.return_typ;
return _tmpC6;};}
# 572
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpC8=te->le;
if(te->le == 0)return 0;{
struct Cyc_Tcenv_Fenv _tmpC9=*((struct Cyc_Tcenv_Fenv*)_check_null(_tmpC8));struct Cyc_List_List*_tmpCB;struct Cyc_Tcenv_Fenv _tmpCA=_tmpC9;_tmpCB=_tmpCA.type_vars;
return _tmpCB;};}
# 579
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Core_Opt*_tmp3FC;return(_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC->v=Cyc_Tcenv_lookup_type_vars(te),_tmp3FC)));}
# 583
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
# 585
const char*_tmp3FD;struct Cyc_Tcenv_Fenv*_tmpCD=
# 587
Cyc_Tcenv_get_fenv(te,((_tmp3FD="add_type_vars",_tag_dyneither(_tmp3FD,sizeof(char),14))));
# 585
struct Cyc_Tcenv_SharedFenv*_tmpCF;struct Cyc_List_List*_tmpD0;struct Cyc_RgnOrder_RgnPO*_tmpD1;const struct Cyc_Tcenv_Bindings*_tmpD2;struct Cyc_Absyn_Stmt*_tmpD3;struct Cyc_Tcenv_CtrlEnv*_tmpD4;void*_tmpD5;void*_tmpD6;struct Cyc_Tcenv_FenvFlags _tmpD7;struct Cyc_Tcenv_Fenv*_tmpCE=_tmpCD;_tmpCF=_tmpCE->shared;_tmpD0=_tmpCE->type_vars;_tmpD1=_tmpCE->region_order;_tmpD2=_tmpCE->locals;_tmpD3=_tmpCE->encloser;_tmpD4=_tmpCE->ctrl_env;_tmpD5=_tmpCE->capability;_tmpD6=_tmpCE->curr_rgn;_tmpD7=_tmpCE->flags;
# 589
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmpD0);
Cyc_Tcutil_check_unique_tvars(loc,_tmpD8);{
struct Cyc_Tcenv_Fenv*_tmp3FE;struct Cyc_Tcenv_Fenv*_tmpD9=(_tmp3FE=_region_malloc(r,sizeof(*_tmp3FE)),((_tmp3FE->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpCF,((_tmp3FE->type_vars=(struct Cyc_List_List*)_tmpD8,((_tmp3FE->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmpD1,((_tmp3FE->locals=(const struct Cyc_Tcenv_Bindings*)_tmpD2,((_tmp3FE->encloser=(struct Cyc_Absyn_Stmt*)_tmpD3,((_tmp3FE->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmpD4,((_tmp3FE->capability=(void*)_tmpD5,((_tmp3FE->curr_rgn=(void*)_tmpD6,((_tmp3FE->flags=(struct Cyc_Tcenv_FenvFlags)_tmpD7,((_tmp3FE->fnrgn=(struct _RegionHandle*)r,_tmp3FE)))))))))))))))))))));
# 594
return Cyc_Tcenv_put_fenv(r,te,_tmpD9);};};}
# 597
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpDC=te->le;
if(_tmpDC == 0)
return Cyc_Tcenv_put_emptyfenv(r,te);else{
# 602
struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,_tmpDC);
return Cyc_Tcenv_put_fenv(r,te,fe);}}
# 607
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 609
struct _dyneither_ptr*_tmpDD=(*vd->name).f2;
{union Cyc_Absyn_Nmspace _tmpDE=(*vd->name).f1;union Cyc_Absyn_Nmspace _tmpDF=_tmpDE;_LL5C: if((_tmpDF.Loc_n).tag != 4)goto _LL5E;_LL5D:
 goto _LL5B;_LL5E:;_LL5F: {
# 614
struct Cyc_Core_Impossible_exn_struct _tmp404;const char*_tmp403;struct Cyc_Core_Impossible_exn_struct*_tmp402;(int)_throw((void*)((_tmp402=_cycalloc(sizeof(*_tmp402)),((_tmp402[0]=((_tmp404.tag=Cyc_Core_Impossible,((_tmp404.f1=((_tmp403="add_local_var: called with Rel_n",_tag_dyneither(_tmp403,sizeof(char),33))),_tmp404)))),_tmp402)))));}_LL5B:;}
# 616
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmpE3;_push_handler(& _tmpE3);{int _tmpE5=0;if(setjmp(_tmpE3.handler))_tmpE5=1;if(!_tmpE5){
{void*_tmpE6=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmpE7=_tmpE6;_LL61: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpE8=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpE8->tag != 0)goto _LL63;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE9=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmpE8->f1);if(_tmpE9->tag != 4)goto _LL63;}}_LL62:
# 620
{const char*_tmp407;void*_tmp406;(_tmp406=0,Cyc_Tcutil_warn(loc,((_tmp407="declaration hides local",_tag_dyneither(_tmp407,sizeof(char),24))),_tag_dyneither(_tmp406,sizeof(void*),0)));}goto _LL60;_LL63: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEA=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEA->tag != 0)goto _LL65;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpEB=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpEA->f1);if(_tmpEB->tag != 1)goto _LL65;}}_LL64:
{const char*_tmp40A;void*_tmp409;(_tmp409=0,Cyc_Tcutil_warn(loc,((_tmp40A="declaration hides global",_tag_dyneither(_tmp40A,sizeof(char),25))),_tag_dyneither(_tmp409,sizeof(void*),0)));}goto _LL60;_LL65: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEC=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEC->tag != 0)goto _LL67;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpED=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmpEC->f1);if(_tmpED->tag != 5)goto _LL67;}}_LL66:
{const char*_tmp40D;void*_tmp40C;(_tmp40C=0,Cyc_Tcutil_warn(loc,((_tmp40D="declaration hides pattern variable",_tag_dyneither(_tmp40D,sizeof(char),35))),_tag_dyneither(_tmp40C,sizeof(void*),0)));}goto _LL60;_LL67: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpEE=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpE7;if(_tmpEE->tag != 0)goto _LL69;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpEF=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmpEE->f1);if(_tmpEF->tag != 3)goto _LL69;}}_LL68:
{const char*_tmp410;void*_tmp40F;(_tmp40F=0,Cyc_Tcutil_warn(loc,((_tmp410="declaration hides parameter",_tag_dyneither(_tmp410,sizeof(char),28))),_tag_dyneither(_tmp40F,sizeof(void*),0)));}goto _LL60;_LL69:;_LL6A:
 goto _LL60;_LL60:;}
# 618
;_pop_handler();}else{void*_tmpE4=(void*)_exn_thrown;void*_tmpF9=_tmpE4;void*_tmpFB;_LL6C: {struct Cyc_Dict_Absent_exn_struct*_tmpFA=(struct Cyc_Dict_Absent_exn_struct*)_tmpF9;if(_tmpFA->tag != Cyc_Dict_Absent)goto _LL6E;}_LL6D:
# 626
 goto _LL6B;_LL6E: _tmpFB=_tmpF9;_LL6F:(void)_throw(_tmpFB);_LL6B:;}};}{
const char*_tmp411;struct Cyc_Tcenv_Fenv*_tmpFC=
# 629
Cyc_Tcenv_get_fenv(te,((_tmp411="add_local_var",_tag_dyneither(_tmp411,sizeof(char),14))));
# 627
struct Cyc_Tcenv_SharedFenv*_tmpFE;struct Cyc_List_List*_tmpFF;struct Cyc_RgnOrder_RgnPO*_tmp100;const struct Cyc_Tcenv_Bindings*_tmp101;struct Cyc_Absyn_Stmt*_tmp102;struct Cyc_Tcenv_CtrlEnv*_tmp103;void*_tmp104;void*_tmp105;struct Cyc_Tcenv_FenvFlags _tmp106;struct Cyc_Tcenv_Fenv*_tmpFD=_tmpFC;_tmpFE=_tmpFD->shared;_tmpFF=_tmpFD->type_vars;_tmp100=_tmpFD->region_order;_tmp101=_tmpFD->locals;_tmp102=_tmpFD->encloser;_tmp103=_tmpFD->ctrl_env;_tmp104=_tmpFD->capability;_tmp105=_tmpFD->curr_rgn;_tmp106=_tmpFD->flags;{
# 630
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp414;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp413;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp107=(_tmp413=_cycalloc(sizeof(*_tmp413)),((_tmp413[0]=((_tmp414.tag=4,((_tmp414.f1=vd,_tmp414)))),_tmp413)));
struct Cyc_Tcenv_Bindings*_tmp415;struct Cyc_Tcenv_Bindings*_tmp108=(_tmp415=_region_malloc(r,sizeof(*_tmp415)),((_tmp415->v=_tmpDD,((_tmp415->b=(void*)_tmp107,((_tmp415->tl=_tmp101,_tmp415)))))));
struct Cyc_Tcenv_Fenv*_tmp416;struct Cyc_Tcenv_Fenv*_tmp109=(_tmp416=_region_malloc(r,sizeof(*_tmp416)),((_tmp416->shared=(struct Cyc_Tcenv_SharedFenv*)_tmpFE,((_tmp416->type_vars=(struct Cyc_List_List*)_tmpFF,((_tmp416->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp100,((_tmp416->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp108),((_tmp416->encloser=(struct Cyc_Absyn_Stmt*)_tmp102,((_tmp416->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp103,((_tmp416->capability=(void*)_tmp104,((_tmp416->curr_rgn=(void*)_tmp105,((_tmp416->flags=(struct Cyc_Tcenv_FenvFlags)_tmp106,((_tmp416->fnrgn=(struct _RegionHandle*)r,_tmp416)))))))))))))))))))));
# 634
return Cyc_Tcenv_put_fenv(r,te,_tmp109);};};}
# 637
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*r,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
# 639
struct Cyc_Tcenv_Fenv*_tmp10F=te->le;
if(_tmp10F == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp110=*_tmp10F;struct Cyc_Tcenv_SharedFenv*_tmp112;struct Cyc_List_List*_tmp113;struct Cyc_RgnOrder_RgnPO*_tmp114;const struct Cyc_Tcenv_Bindings*_tmp115;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Tcenv_CtrlEnv*_tmp117;void*_tmp118;void*_tmp119;int _tmp11A;int _tmp11B;int _tmp11C;struct Cyc_Tcenv_Fenv _tmp111=_tmp110;_tmp112=_tmp111.shared;_tmp113=_tmp111.type_vars;_tmp114=_tmp111.region_order;_tmp115=_tmp111.locals;_tmp116=_tmp111.encloser;_tmp117=_tmp111.ctrl_env;_tmp118=_tmp111.capability;_tmp119=_tmp111.curr_rgn;_tmp11A=(_tmp111.flags).in_notreadctxt;_tmp11B=(_tmp111.flags).in_lhs;_tmp11C=(_tmp111.flags).abstract_ok;{
# 644
struct Cyc_Tcenv_Fenv*_tmp417;struct Cyc_Tcenv_Fenv*_tmp11D=(_tmp417=_region_malloc(r,sizeof(*_tmp417)),((_tmp417->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp112,((_tmp417->type_vars=(struct Cyc_List_List*)_tmp113,((_tmp417->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp114,((_tmp417->locals=(const struct Cyc_Tcenv_Bindings*)_tmp115,((_tmp417->encloser=(struct Cyc_Absyn_Stmt*)_tmp116,((_tmp417->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp117,((_tmp417->capability=(void*)_tmp118,((_tmp417->curr_rgn=(void*)_tmp119,((_tmp417->flags=(struct Cyc_Tcenv_FenvFlags)(
# 646
((_tmp417->flags).in_notreadctxt=_tmp11A,(((_tmp417->flags).in_lhs=_tmp11B,(((_tmp417->flags).in_new=(int)status,(((_tmp417->flags).abstract_ok=_tmp11C,_tmp417->flags)))))))),((_tmp417->fnrgn=(struct _RegionHandle*)r,_tmp417)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp11D);};};}
# 650
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp11F=te->le;
if(_tmp11F == 0)return Cyc_Tcenv_NoneNew;{
struct Cyc_Tcenv_Fenv*_tmp120=_tmp11F;int _tmp122;struct Cyc_Tcenv_Fenv*_tmp121=_tmp120;_tmp122=(_tmp121->flags).in_new;
return(enum Cyc_Tcenv_NewStatus)_tmp122;};}
# 657
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp123=te->le;
if(_tmp123 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp124=_tmp123;int _tmp126;struct Cyc_Tcenv_Fenv*_tmp125=_tmp124;_tmp126=(_tmp125->flags).abstract_ok;
return _tmp126;};}
# 664
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 666
struct Cyc_Tcenv_Fenv*_tmp127=te->le;
if(_tmp127 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp128=*_tmp127;struct Cyc_Tcenv_SharedFenv*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_RgnOrder_RgnPO*_tmp12C;const struct Cyc_Tcenv_Bindings*_tmp12D;struct Cyc_Absyn_Stmt*_tmp12E;struct Cyc_Tcenv_CtrlEnv*_tmp12F;void*_tmp130;void*_tmp131;int _tmp132;int _tmp133;int _tmp134;struct Cyc_Tcenv_Fenv _tmp129=_tmp128;_tmp12A=_tmp129.shared;_tmp12B=_tmp129.type_vars;_tmp12C=_tmp129.region_order;_tmp12D=_tmp129.locals;_tmp12E=_tmp129.encloser;_tmp12F=_tmp129.ctrl_env;_tmp130=_tmp129.capability;_tmp131=_tmp129.curr_rgn;_tmp132=(_tmp129.flags).in_notreadctxt;_tmp133=(_tmp129.flags).in_lhs;_tmp134=(_tmp129.flags).in_new;{
# 671
struct Cyc_Tcenv_Fenv*_tmp418;struct Cyc_Tcenv_Fenv*_tmp135=(_tmp418=_region_malloc(r,sizeof(*_tmp418)),((_tmp418->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp12A,((_tmp418->type_vars=(struct Cyc_List_List*)_tmp12B,((_tmp418->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp12C,((_tmp418->locals=(const struct Cyc_Tcenv_Bindings*)_tmp12D,((_tmp418->encloser=(struct Cyc_Absyn_Stmt*)_tmp12E,((_tmp418->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp12F,((_tmp418->capability=(void*)_tmp130,((_tmp418->curr_rgn=(void*)_tmp131,((_tmp418->flags=(struct Cyc_Tcenv_FenvFlags)(
# 673
((_tmp418->flags).in_notreadctxt=_tmp132,(((_tmp418->flags).in_lhs=_tmp133,(((_tmp418->flags).in_new=_tmp134,(((_tmp418->flags).abstract_ok=1,_tmp418->flags)))))))),((_tmp418->fnrgn=(struct _RegionHandle*)r,_tmp418)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp135);};};}
# 677
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
# 679
struct Cyc_Tcenv_Fenv*_tmp137=te->le;
if(_tmp137 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp138=*_tmp137;struct Cyc_Tcenv_SharedFenv*_tmp13A;struct Cyc_List_List*_tmp13B;struct Cyc_RgnOrder_RgnPO*_tmp13C;const struct Cyc_Tcenv_Bindings*_tmp13D;struct Cyc_Absyn_Stmt*_tmp13E;struct Cyc_Tcenv_CtrlEnv*_tmp13F;void*_tmp140;void*_tmp141;int _tmp142;int _tmp143;int _tmp144;struct Cyc_Tcenv_Fenv _tmp139=_tmp138;_tmp13A=_tmp139.shared;_tmp13B=_tmp139.type_vars;_tmp13C=_tmp139.region_order;_tmp13D=_tmp139.locals;_tmp13E=_tmp139.encloser;_tmp13F=_tmp139.ctrl_env;_tmp140=_tmp139.capability;_tmp141=_tmp139.curr_rgn;_tmp142=(_tmp139.flags).in_notreadctxt;_tmp143=(_tmp139.flags).in_lhs;_tmp144=(_tmp139.flags).in_new;{
# 684
struct Cyc_Tcenv_Fenv*_tmp419;struct Cyc_Tcenv_Fenv*_tmp145=(_tmp419=_region_malloc(r,sizeof(*_tmp419)),((_tmp419->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp13A,((_tmp419->type_vars=(struct Cyc_List_List*)_tmp13B,((_tmp419->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp13C,((_tmp419->locals=(const struct Cyc_Tcenv_Bindings*)_tmp13D,((_tmp419->encloser=(struct Cyc_Absyn_Stmt*)_tmp13E,((_tmp419->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp13F,((_tmp419->capability=(void*)_tmp140,((_tmp419->curr_rgn=(void*)_tmp141,((_tmp419->flags=(struct Cyc_Tcenv_FenvFlags)(
# 686
((_tmp419->flags).in_notreadctxt=_tmp142,(((_tmp419->flags).in_lhs=_tmp143,(((_tmp419->flags).in_new=_tmp144,(((_tmp419->flags).abstract_ok=0,_tmp419->flags)))))))),((_tmp419->fnrgn=(struct _RegionHandle*)r,_tmp419)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp145);};};}
# 690
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp147=te->le;
if(_tmp147 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp148=*_tmp147;struct Cyc_Tcenv_SharedFenv*_tmp14A;struct Cyc_List_List*_tmp14B;struct Cyc_RgnOrder_RgnPO*_tmp14C;const struct Cyc_Tcenv_Bindings*_tmp14D;struct Cyc_Absyn_Stmt*_tmp14E;struct Cyc_Tcenv_CtrlEnv*_tmp14F;void*_tmp150;void*_tmp151;int _tmp152;int _tmp153;int _tmp154;struct Cyc_Tcenv_Fenv _tmp149=_tmp148;_tmp14A=_tmp149.shared;_tmp14B=_tmp149.type_vars;_tmp14C=_tmp149.region_order;_tmp14D=_tmp149.locals;_tmp14E=_tmp149.encloser;_tmp14F=_tmp149.ctrl_env;_tmp150=_tmp149.capability;_tmp151=_tmp149.curr_rgn;_tmp152=(_tmp149.flags).in_lhs;_tmp153=(_tmp149.flags).in_new;_tmp154=(_tmp149.flags).abstract_ok;{
# 696
struct Cyc_Tcenv_Fenv*_tmp41A;struct Cyc_Tcenv_Fenv*_tmp155=(_tmp41A=_region_malloc(r,sizeof(*_tmp41A)),((_tmp41A->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp14A,((_tmp41A->type_vars=(struct Cyc_List_List*)_tmp14B,((_tmp41A->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp14C,((_tmp41A->locals=(const struct Cyc_Tcenv_Bindings*)_tmp14D,((_tmp41A->encloser=(struct Cyc_Absyn_Stmt*)_tmp14E,((_tmp41A->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp14F,((_tmp41A->capability=(void*)_tmp150,((_tmp41A->curr_rgn=(void*)_tmp151,((_tmp41A->flags=(struct Cyc_Tcenv_FenvFlags)(
# 698
((_tmp41A->flags).in_notreadctxt=1,(((_tmp41A->flags).in_lhs=_tmp152,(((_tmp41A->flags).in_new=_tmp153,(((_tmp41A->flags).abstract_ok=_tmp154,_tmp41A->flags)))))))),((_tmp41A->fnrgn=(struct _RegionHandle*)r,_tmp41A)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp155);};};}
# 702
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp157=te->le;
if(_tmp157 == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp158=*_tmp157;struct Cyc_Tcenv_SharedFenv*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_RgnOrder_RgnPO*_tmp15C;const struct Cyc_Tcenv_Bindings*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15E;struct Cyc_Tcenv_CtrlEnv*_tmp15F;void*_tmp160;void*_tmp161;int _tmp162;int _tmp163;int _tmp164;struct Cyc_Tcenv_Fenv _tmp159=_tmp158;_tmp15A=_tmp159.shared;_tmp15B=_tmp159.type_vars;_tmp15C=_tmp159.region_order;_tmp15D=_tmp159.locals;_tmp15E=_tmp159.encloser;_tmp15F=_tmp159.ctrl_env;_tmp160=_tmp159.capability;_tmp161=_tmp159.curr_rgn;_tmp162=(_tmp159.flags).in_lhs;_tmp163=(_tmp159.flags).in_new;_tmp164=(_tmp159.flags).abstract_ok;{
# 708
struct Cyc_Tcenv_Fenv*_tmp41B;struct Cyc_Tcenv_Fenv*_tmp165=(_tmp41B=_region_malloc(r,sizeof(*_tmp41B)),((_tmp41B->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp15A,((_tmp41B->type_vars=(struct Cyc_List_List*)_tmp15B,((_tmp41B->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp15C,((_tmp41B->locals=(const struct Cyc_Tcenv_Bindings*)_tmp15D,((_tmp41B->encloser=(struct Cyc_Absyn_Stmt*)_tmp15E,((_tmp41B->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp15F,((_tmp41B->capability=(void*)_tmp160,((_tmp41B->curr_rgn=(void*)_tmp161,((_tmp41B->flags=(struct Cyc_Tcenv_FenvFlags)(
# 710
((_tmp41B->flags).in_notreadctxt=0,(((_tmp41B->flags).in_lhs=_tmp162,(((_tmp41B->flags).in_new=_tmp163,(((_tmp41B->flags).abstract_ok=_tmp164,_tmp41B->flags)))))))),((_tmp41B->fnrgn=(struct _RegionHandle*)r,_tmp41B)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp165);};};}
# 714
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp167=te->le;
if(_tmp167 == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp168=_tmp167;int _tmp16A;struct Cyc_Tcenv_Fenv*_tmp169=_tmp168;_tmp16A=(_tmp169->flags).in_notreadctxt;
return _tmp16A;};}
# 721
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp16B=te->le;
if(_tmp16B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp16C=*_tmp16B;struct Cyc_Tcenv_SharedFenv*_tmp16E;struct Cyc_List_List*_tmp16F;struct Cyc_RgnOrder_RgnPO*_tmp170;const struct Cyc_Tcenv_Bindings*_tmp171;struct Cyc_Absyn_Stmt*_tmp172;struct Cyc_Tcenv_CtrlEnv*_tmp173;void*_tmp174;void*_tmp175;int _tmp176;int _tmp177;int _tmp178;struct Cyc_Tcenv_Fenv _tmp16D=_tmp16C;_tmp16E=_tmp16D.shared;_tmp16F=_tmp16D.type_vars;_tmp170=_tmp16D.region_order;_tmp171=_tmp16D.locals;_tmp172=_tmp16D.encloser;_tmp173=_tmp16D.ctrl_env;_tmp174=_tmp16D.capability;_tmp175=_tmp16D.curr_rgn;_tmp176=(_tmp16D.flags).in_notreadctxt;_tmp177=(_tmp16D.flags).in_new;_tmp178=(_tmp16D.flags).abstract_ok;{
# 727
struct Cyc_Tcenv_Fenv*_tmp41C;struct Cyc_Tcenv_Fenv*_tmp179=(_tmp41C=_region_malloc(r,sizeof(*_tmp41C)),((_tmp41C->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp16E,((_tmp41C->type_vars=(struct Cyc_List_List*)_tmp16F,((_tmp41C->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp170,((_tmp41C->locals=(const struct Cyc_Tcenv_Bindings*)_tmp171,((_tmp41C->encloser=(struct Cyc_Absyn_Stmt*)_tmp172,((_tmp41C->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp173,((_tmp41C->capability=(void*)_tmp174,((_tmp41C->curr_rgn=(void*)_tmp175,((_tmp41C->flags=(struct Cyc_Tcenv_FenvFlags)(
# 729
((_tmp41C->flags).in_notreadctxt=_tmp176,(((_tmp41C->flags).in_lhs=1,(((_tmp41C->flags).in_new=_tmp177,(((_tmp41C->flags).abstract_ok=_tmp178,_tmp41C->flags)))))))),((_tmp41C->fnrgn=(struct _RegionHandle*)r,_tmp41C)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp179);};};}
# 733
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17B=te->le;
if(_tmp17B == 0)return Cyc_Tcenv_put_emptyfenv(r,te);{
struct Cyc_Tcenv_Fenv _tmp17C=*_tmp17B;struct Cyc_Tcenv_SharedFenv*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_RgnOrder_RgnPO*_tmp180;const struct Cyc_Tcenv_Bindings*_tmp181;struct Cyc_Absyn_Stmt*_tmp182;struct Cyc_Tcenv_CtrlEnv*_tmp183;void*_tmp184;void*_tmp185;int _tmp186;int _tmp187;int _tmp188;struct Cyc_Tcenv_Fenv _tmp17D=_tmp17C;_tmp17E=_tmp17D.shared;_tmp17F=_tmp17D.type_vars;_tmp180=_tmp17D.region_order;_tmp181=_tmp17D.locals;_tmp182=_tmp17D.encloser;_tmp183=_tmp17D.ctrl_env;_tmp184=_tmp17D.capability;_tmp185=_tmp17D.curr_rgn;_tmp186=(_tmp17D.flags).in_notreadctxt;_tmp187=(_tmp17D.flags).in_new;_tmp188=(_tmp17D.flags).abstract_ok;{
# 739
struct Cyc_Tcenv_Fenv*_tmp41D;struct Cyc_Tcenv_Fenv*_tmp189=(_tmp41D=_region_malloc(r,sizeof(*_tmp41D)),((_tmp41D->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp17E,((_tmp41D->type_vars=(struct Cyc_List_List*)_tmp17F,((_tmp41D->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp180,((_tmp41D->locals=(const struct Cyc_Tcenv_Bindings*)_tmp181,((_tmp41D->encloser=(struct Cyc_Absyn_Stmt*)_tmp182,((_tmp41D->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp183,((_tmp41D->capability=(void*)_tmp184,((_tmp41D->curr_rgn=(void*)_tmp185,((_tmp41D->flags=(struct Cyc_Tcenv_FenvFlags)(
# 741
((_tmp41D->flags).in_notreadctxt=_tmp186,(((_tmp41D->flags).in_lhs=0,(((_tmp41D->flags).in_new=_tmp187,(((_tmp41D->flags).abstract_ok=_tmp188,_tmp41D->flags)))))))),((_tmp41D->fnrgn=(struct _RegionHandle*)r,_tmp41D)))))))))))))))))))));
return Cyc_Tcenv_put_fenv(r,te,_tmp189);};};}
# 745
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18B=te->le;
if(_tmp18B == 0)return 0;{
struct Cyc_Tcenv_Fenv*_tmp18C=_tmp18B;int _tmp18E;struct Cyc_Tcenv_Fenv*_tmp18D=_tmp18C;_tmp18E=(_tmp18D->flags).in_lhs;
return _tmp18E;};}
# 753
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Vardecl*vd){
# 755
if(Cyc_Tcenv_warn_override){
struct _handler_cons _tmp18F;_push_handler(& _tmp18F);{int _tmp191=0;if(setjmp(_tmp18F.handler))_tmp191=1;if(!_tmp191){
{void*_tmp192=Cyc_Tcenv_lookup_ordinary(r,te,loc,vd->name,1);void*_tmp193=_tmp192;_LL71: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp194=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp194->tag != 0)goto _LL73;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp195=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp194->f1);if(_tmp195->tag != 4)goto _LL73;}}_LL72:
# 759
{const char*_tmp420;void*_tmp41F;(_tmp41F=0,Cyc_Tcutil_warn(loc,((_tmp420="pattern variable hides local",_tag_dyneither(_tmp420,sizeof(char),29))),_tag_dyneither(_tmp41F,sizeof(void*),0)));}goto _LL70;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp196=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp196->tag != 0)goto _LL75;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp197=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp196->f1);if(_tmp197->tag != 1)goto _LL75;}}_LL74:
{const char*_tmp423;void*_tmp422;(_tmp422=0,Cyc_Tcutil_warn(loc,((_tmp423="pattern variable hides global",_tag_dyneither(_tmp423,sizeof(char),30))),_tag_dyneither(_tmp422,sizeof(void*),0)));}goto _LL70;_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp198=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp198->tag != 0)goto _LL77;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp198->f1);if(_tmp199->tag != 5)goto _LL77;}}_LL76:
{const char*_tmp426;void*_tmp425;(_tmp425=0,Cyc_Tcutil_warn(loc,((_tmp426="pattern variable hides pattern variable",_tag_dyneither(_tmp426,sizeof(char),40))),_tag_dyneither(_tmp425,sizeof(void*),0)));}goto _LL70;_LL77: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp19A=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp193;if(_tmp19A->tag != 0)goto _LL79;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp19B=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp19A->f1);if(_tmp19B->tag != 3)goto _LL79;}}_LL78:
{const char*_tmp429;void*_tmp428;(_tmp428=0,Cyc_Tcutil_warn(loc,((_tmp429="pattern variable hides parameter",_tag_dyneither(_tmp429,sizeof(char),33))),_tag_dyneither(_tmp428,sizeof(void*),0)));}goto _LL70;_LL79:;_LL7A:
 goto _LL70;_LL70:;}
# 757
;_pop_handler();}else{void*_tmp190=(void*)_exn_thrown;void*_tmp1A5=_tmp190;void*_tmp1A7;_LL7C: {struct Cyc_Dict_Absent_exn_struct*_tmp1A6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1A5;if(_tmp1A6->tag != Cyc_Dict_Absent)goto _LL7E;}_LL7D:
# 765
 goto _LL7B;_LL7E: _tmp1A7=_tmp1A5;_LL7F:(void)_throw(_tmp1A7);_LL7B:;}};}{
struct _dyneither_ptr*_tmp1A8=(*vd->name).f2;
const char*_tmp42A;struct Cyc_Tcenv_Fenv*_tmp1A9=
# 769
Cyc_Tcenv_get_fenv(te,((_tmp42A="add_pat_var",_tag_dyneither(_tmp42A,sizeof(char),12))));
# 767
struct Cyc_Tcenv_SharedFenv*_tmp1AB;struct Cyc_List_List*_tmp1AC;struct Cyc_RgnOrder_RgnPO*_tmp1AD;const struct Cyc_Tcenv_Bindings*_tmp1AE;struct Cyc_Absyn_Stmt*_tmp1AF;struct Cyc_Tcenv_CtrlEnv*_tmp1B0;void*_tmp1B1;void*_tmp1B2;struct Cyc_Tcenv_FenvFlags _tmp1B3;struct Cyc_Tcenv_Fenv*_tmp1AA=_tmp1A9;_tmp1AB=_tmp1AA->shared;_tmp1AC=_tmp1AA->type_vars;_tmp1AD=_tmp1AA->region_order;_tmp1AE=_tmp1AA->locals;_tmp1AF=_tmp1AA->encloser;_tmp1B0=_tmp1AA->ctrl_env;_tmp1B1=_tmp1AA->capability;_tmp1B2=_tmp1AA->curr_rgn;_tmp1B3=_tmp1AA->flags;{
# 770
struct Cyc_Absyn_Pat_b_Absyn_Binding_struct _tmp42D;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp42C;struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp1B4=(_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C[0]=((_tmp42D.tag=5,((_tmp42D.f1=vd,_tmp42D)))),_tmp42C)));
struct Cyc_Tcenv_Bindings*_tmp42E;struct Cyc_Tcenv_Bindings*_tmp1B5=(_tmp42E=_region_malloc(r,sizeof(*_tmp42E)),((_tmp42E->v=_tmp1A8,((_tmp42E->b=(void*)_tmp1B4,((_tmp42E->tl=_tmp1AE,_tmp42E)))))));
struct Cyc_Tcenv_Fenv*_tmp42F;struct Cyc_Tcenv_Fenv*_tmp1B6=(_tmp42F=_region_malloc(r,sizeof(*_tmp42F)),((_tmp42F->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp1AB,((_tmp42F->type_vars=(struct Cyc_List_List*)_tmp1AC,((_tmp42F->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp1AD,((_tmp42F->locals=(const struct Cyc_Tcenv_Bindings*)((const struct Cyc_Tcenv_Bindings*)_tmp1B5),((_tmp42F->encloser=(struct Cyc_Absyn_Stmt*)_tmp1AF,((_tmp42F->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp1B0,((_tmp42F->capability=(void*)_tmp1B1,((_tmp42F->curr_rgn=(void*)_tmp1B2,((_tmp42F->flags=(struct Cyc_Tcenv_FenvFlags)_tmp1B3,((_tmp42F->fnrgn=(struct _RegionHandle*)r,_tmp42F)))))))))))))))))))));
# 774
return Cyc_Tcenv_put_fenv(r,te,_tmp1B6);};};}
# 777
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct Cyc_Tcenv_Fenv*_tmp1BC=te->le;
union Cyc_Absyn_Nmspace _tmp1BE;struct _dyneither_ptr*_tmp1BF;struct _tuple0*_tmp1BD=q;_tmp1BE=_tmp1BD->f1;_tmp1BF=_tmp1BD->f2;{
union Cyc_Absyn_Nmspace _tmp1C0=_tmp1BE;_LL81: if((_tmp1C0.Loc_n).tag != 4)goto _LL83;_LL82:
# 782
 if(_tmp1BC == 0){struct Cyc_Dict_Absent_exn_struct _tmp432;struct Cyc_Dict_Absent_exn_struct*_tmp431;(int)_throw((void*)((_tmp431=_cycalloc_atomic(sizeof(*_tmp431)),((_tmp431[0]=((_tmp432.tag=Cyc_Dict_Absent,_tmp432)),_tmp431)))));}
goto _LL84;_LL83: if((_tmp1C0.Rel_n).tag != 1)goto _LL85;if((struct Cyc_List_List*)(_tmp1C0.Rel_n).val != 0)goto _LL85;if(!(_tmp1BC != 0))goto _LL85;_LL84: {
# 785
struct Cyc_Tcenv_Fenv*_tmp1C3=_tmp1BC;const struct Cyc_Tcenv_Bindings*_tmp1C5;struct Cyc_Tcenv_Fenv*_tmp1C4=_tmp1C3;_tmp1C5=_tmp1C4->locals;{
# 787
struct _handler_cons _tmp1C6;_push_handler(& _tmp1C6);{int _tmp1C8=0;if(setjmp(_tmp1C6.handler))_tmp1C8=1;if(!_tmp1C8){
{struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp435;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp434;void*_tmp1CB=(void*)((_tmp434=_region_malloc(r,sizeof(*_tmp434)),((_tmp434[0]=((_tmp435.tag=0,((_tmp435.f1=Cyc_Tcenv_lookup_binding(_tmp1C5,_tmp1BF),_tmp435)))),_tmp434))));_npop_handler(0);return _tmp1CB;};_pop_handler();}else{void*_tmp1C7=(void*)_exn_thrown;void*_tmp1CD=_tmp1C7;void*_tmp1CF;_LL88: {struct Cyc_Tcenv_NoBinding_exn_struct*_tmp1CE=(struct Cyc_Tcenv_NoBinding_exn_struct*)_tmp1CD;if(_tmp1CE->tag != Cyc_Tcenv_NoBinding)goto _LL8A;}_LL89:
 return Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_LL8A: _tmp1CF=_tmp1CD;_LL8B:(void)_throw(_tmp1CF);_LL87:;}};};}_LL85:;_LL86: {
# 791
struct _handler_cons _tmp1D0;_push_handler(& _tmp1D0);{int _tmp1D2=0;if(setjmp(_tmp1D0.handler))_tmp1D2=1;if(!_tmp1D2){{void*_tmp1D3=Cyc_Tcenv_lookup_ordinary_global(te,loc,q,is_use);_npop_handler(0);return _tmp1D3;};_pop_handler();}else{void*_tmp1D1=(void*)_exn_thrown;void*_tmp1D5=_tmp1D1;void*_tmp1D7;_LL8D: {struct Cyc_Dict_Absent_exn_struct*_tmp1D6=(struct Cyc_Dict_Absent_exn_struct*)_tmp1D5;if(_tmp1D6->tag != Cyc_Dict_Absent)goto _LL8F;}_LL8E: {
# 795
struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp438;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp437;return(void*)((_tmp437=_region_malloc(r,sizeof(*_tmp437)),((_tmp437[0]=((_tmp438.tag=0,((_tmp438.f1=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp438)))),_tmp437))));}_LL8F: _tmp1D7=_tmp1D5;_LL90:(void)_throw(_tmp1D7);_LL8C:;}};}_LL80:;};}
# 800
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp439;struct Cyc_Tcenv_Fenv*_tmp1DA=
Cyc_Tcenv_get_fenv(te,((_tmp439="process_continue",_tag_dyneither(_tmp439,sizeof(char),17))));
# 801
struct Cyc_Tcenv_CtrlEnv*_tmp1DC;struct Cyc_Tcenv_Fenv*_tmp1DB=_tmp1DA;_tmp1DC=_tmp1DB->ctrl_env;{
# 803
void*_tmp1DD=_tmp1DC->continue_stmt;void*_tmp1DE=_tmp1DD;struct Cyc_Absyn_Stmt*_tmp1E0;_LL92: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1DF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1DF->tag != 3)goto _LL94;else{_tmp1E0=_tmp1DF->f1;}}_LL93:
# 805
{struct Cyc_List_List*_tmp43A;_tmp1E0->non_local_preds=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->hd=s,((_tmp43A->tl=_tmp1E0->non_local_preds,_tmp43A))))));}
*sopt=_tmp1E0;
return;_LL94: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1E1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E1->tag != 0)goto _LL96;}_LL95:
{const char*_tmp43D;void*_tmp43C;(_tmp43C=0,Cyc_Tcutil_terr(s->loc,((_tmp43D="continue not in a loop",_tag_dyneither(_tmp43D,sizeof(char),23))),_tag_dyneither(_tmp43C,sizeof(void*),0)));}return;_LL96: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1E2=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E2->tag != 1)goto _LL98;}_LL97:
 goto _LL99;_LL98: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1E3=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1DE;if(_tmp1E3->tag != 2)goto _LL91;}_LL99: {
const char*_tmp43E;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp43E="bad continue destination",_tag_dyneither(_tmp43E,sizeof(char),25))));}_LL91:;};}
# 813
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp43F;struct Cyc_Tcenv_Fenv*_tmp1E9=
Cyc_Tcenv_get_fenv(te,((_tmp43F="process_break",_tag_dyneither(_tmp43F,sizeof(char),14))));
# 814
struct Cyc_Tcenv_CtrlEnv*_tmp1EB;struct Cyc_Tcenv_SharedFenv*_tmp1EC;struct Cyc_Tcenv_Fenv*_tmp1EA=_tmp1E9;_tmp1EB=_tmp1EA->ctrl_env;_tmp1EC=_tmp1EA->shared;{
# 816
void*_tmp1ED=_tmp1EB->break_stmt;void*_tmp1EE=_tmp1ED;struct Cyc_Absyn_Stmt*_tmp1F0;_LL9B: {struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1EF=(struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1EF->tag != 3)goto _LL9D;else{_tmp1F0=_tmp1EF->f1;}}_LL9C:
# 818
{struct Cyc_List_List*_tmp440;_tmp1F0->non_local_preds=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->hd=s,((_tmp440->tl=_tmp1F0->non_local_preds,_tmp440))))));}
*sopt=_tmp1F0;
return;_LL9D: {struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*_tmp1F1=(struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F1->tag != 0)goto _LL9F;}_LL9E:
# 822
{const char*_tmp443;void*_tmp442;(_tmp442=0,Cyc_Tcutil_terr(s->loc,((_tmp443="break not in a loop or switch",_tag_dyneither(_tmp443,sizeof(char),30))),_tag_dyneither(_tmp442,sizeof(void*),0)));}
return;_LL9F: {struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*_tmp1F2=(struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F2->tag != 2)goto _LLA1;}_LLA0:
# 842 "tcenv.cyc"
 return;_LLA1: {struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*_tmp1F3=(struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct*)_tmp1EE;if(_tmp1F3->tag != 1)goto _LL9A;}_LLA2:
# 847
{const char*_tmp446;void*_tmp445;(_tmp445=0,Cyc_Tcutil_terr(s->loc,((_tmp446="break causes outer switch clause to implicitly fallthru",_tag_dyneither(_tmp446,sizeof(char),56))),_tag_dyneither(_tmp445,sizeof(void*),0)));}
return;_LL9A:;};}
# 851
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct _dyneither_ptr*l,struct Cyc_Absyn_Stmt**sopt){
const char*_tmp447;struct Cyc_Tcenv_Fenv*_tmp1FA=Cyc_Tcenv_get_fenv(te,((_tmp447="process_goto",_tag_dyneither(_tmp447,sizeof(char),13))));struct Cyc_Tcenv_SharedFenv*_tmp1FC;struct Cyc_Tcenv_Fenv*_tmp1FB=_tmp1FA;_tmp1FC=_tmp1FB->shared;{
struct Cyc_Absyn_Stmt**sopt2=((struct Cyc_Absyn_Stmt**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FC->seen_labels,l);
if(sopt2 == 0){
struct Cyc_Dict_Dict _tmp1FD=_tmp1FC->needed_labels;
struct Cyc_List_List**slopt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(_tmp1FD,l);
struct _RegionHandle*frgn=_tmp1FC->frgn;
if(slopt == 0){
struct Cyc_List_List**_tmp448;slopt=((_tmp448=_region_malloc(frgn,sizeof(*_tmp448)),((_tmp448[0]=0,_tmp448))));}{
struct Cyc_List_List*_tmp449;struct Cyc_List_List*new_needed=(_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449->hd=s,((_tmp449->tl=*slopt,_tmp449)))));
_tmp1FC->needed_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp1FD,l,new_needed);};}else{
# 863
struct Cyc_Absyn_Stmt*s=*sopt2;
{struct Cyc_List_List*_tmp44A;s->non_local_preds=((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A->hd=s,((_tmp44A->tl=s->non_local_preds,_tmp44A))))));}
*sopt=s;}};}
# 869
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 871
const char*_tmp44B;struct Cyc_Tcenv_Fenv*_tmp202=Cyc_Tcenv_get_fenv(te,((_tmp44B="process_fallthru",_tag_dyneither(_tmp44B,sizeof(char),17))));struct Cyc_Tcenv_CtrlEnv*_tmp204;struct Cyc_Tcenv_Fenv*_tmp203=_tmp202;_tmp204=_tmp203->ctrl_env;{
const struct _tuple10*_tmp205=_tmp204->fallthru_clause;
if(_tmp205 != (const struct _tuple10*)0){
{struct Cyc_List_List*_tmp44C;(((*_tmp205).f1)->body)->non_local_preds=(
(_tmp44C=_cycalloc(sizeof(*_tmp44C)),((_tmp44C->hd=s,((_tmp44C->tl=(((*_tmp205).f1)->body)->non_local_preds,_tmp44C))))));}{
struct Cyc_Absyn_Switch_clause**_tmp44D;*clauseopt=((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D[0]=(*_tmp205).f1,_tmp44D))));};}
# 878
return _tmp205;};}
# 881
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 885
const char*_tmp44E;struct Cyc_Tcenv_Fenv*_tmp209=
# 887
Cyc_Tcenv_get_fenv(te,((_tmp44E="set_fallthru",_tag_dyneither(_tmp44E,sizeof(char),13))));
# 885
struct Cyc_Tcenv_SharedFenv*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_RgnOrder_RgnPO*_tmp20D;const struct Cyc_Tcenv_Bindings*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Tcenv_CtrlEnv*_tmp210;void*_tmp211;void*_tmp212;struct Cyc_Tcenv_FenvFlags _tmp213;struct Cyc_Tcenv_Fenv*_tmp20A=_tmp209;_tmp20B=_tmp20A->shared;_tmp20C=_tmp20A->type_vars;_tmp20D=_tmp20A->region_order;_tmp20E=_tmp20A->locals;_tmp20F=_tmp20A->encloser;_tmp210=_tmp20A->ctrl_env;_tmp211=_tmp20A->capability;_tmp212=_tmp20A->curr_rgn;_tmp213=_tmp20A->flags;{
# 888
struct _RegionHandle*_tmp215;void*_tmp216;void*_tmp217;const struct _tuple10*_tmp218;void*_tmp219;int _tmp21A;struct Cyc_Tcenv_CtrlEnv*_tmp214=_tmp210;_tmp215=_tmp214->ctrl_rgn;_tmp216=_tmp214->continue_stmt;_tmp217=_tmp214->break_stmt;_tmp218=_tmp214->fallthru_clause;_tmp219=_tmp214->next_stmt;_tmp21A=_tmp214->try_depth;{
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
struct Cyc_List_List*_tmp44F;ft_typ=((_tmp44F=_region_malloc(_tmp215,sizeof(*_tmp44F)),((_tmp44F->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type,((_tmp44F->tl=ft_typ,_tmp44F))))));}{
const struct Cyc_Tcenv_CList*_tmp21C=(const struct Cyc_Tcenv_CList*)((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);
struct _tuple10*_tmp452;struct Cyc_Tcenv_CtrlEnv*_tmp451;struct Cyc_Tcenv_CtrlEnv*_tmp21D=
(_tmp451=_region_malloc(r,sizeof(*_tmp451)),((_tmp451->ctrl_rgn=_tmp215,((_tmp451->continue_stmt=_tmp216,((_tmp451->break_stmt=_tmp217,((_tmp451->fallthru_clause=(const struct _tuple10*)((_tmp452=_region_malloc(_tmp215,sizeof(*_tmp452)),((_tmp452->f1=clause,((_tmp452->f2=new_tvs,((_tmp452->f3=_tmp21C,_tmp452)))))))),((_tmp451->next_stmt=_tmp219,((_tmp451->try_depth=_tmp21A,_tmp451)))))))))))));
# 896
struct Cyc_Tcenv_Fenv*_tmp453;struct Cyc_Tcenv_Fenv*_tmp21E=(_tmp453=_region_malloc(r,sizeof(*_tmp453)),((_tmp453->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp20B,((_tmp453->type_vars=(struct Cyc_List_List*)_tmp20C,((_tmp453->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp20D,((_tmp453->locals=(const struct Cyc_Tcenv_Bindings*)_tmp20E,((_tmp453->encloser=(struct Cyc_Absyn_Stmt*)_tmp20F,((_tmp453->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp21D,((_tmp453->capability=(void*)_tmp211,((_tmp453->curr_rgn=(void*)_tmp212,((_tmp453->flags=(struct Cyc_Tcenv_FenvFlags)_tmp213,((_tmp453->fnrgn=(struct _RegionHandle*)r,_tmp453)))))))))))))))))))));
# 900
return Cyc_Tcenv_put_fenv(r,te,_tmp21E);};};};}
# 903
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp454;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp454="clear_fallthru",_tag_dyneither(_tmp454,sizeof(char),15)))));
struct Cyc_Tcenv_CtrlEnv*_tmp224;struct Cyc_Tcenv_Fenv*_tmp223=fe;_tmp224=_tmp223->ctrl_env;
_tmp224->fallthru_clause=0;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 910
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest){
# 912
const char*_tmp455;struct Cyc_Tcenv_Fenv*_tmp226=
# 914
Cyc_Tcenv_get_fenv(te,((_tmp455="set_in_loop",_tag_dyneither(_tmp455,sizeof(char),12))));
# 912
struct Cyc_Tcenv_SharedFenv*_tmp228;struct Cyc_List_List*_tmp229;struct Cyc_RgnOrder_RgnPO*_tmp22A;const struct Cyc_Tcenv_Bindings*_tmp22B;struct Cyc_Absyn_Stmt*_tmp22C;struct Cyc_Tcenv_CtrlEnv*_tmp22D;void*_tmp22E;void*_tmp22F;struct Cyc_Tcenv_FenvFlags _tmp230;struct Cyc_Tcenv_Fenv*_tmp227=_tmp226;_tmp228=_tmp227->shared;_tmp229=_tmp227->type_vars;_tmp22A=_tmp227->region_order;_tmp22B=_tmp227->locals;_tmp22C=_tmp227->encloser;_tmp22D=_tmp227->ctrl_env;_tmp22E=_tmp227->capability;_tmp22F=_tmp227->curr_rgn;_tmp230=_tmp227->flags;{
# 915
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp460;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp45F;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp45E;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp45D;struct Cyc_Tcenv_CtrlEnv*_tmp45C;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp45C=_region_malloc(r,sizeof(*_tmp45C)),((_tmp45C->ctrl_rgn=r,((_tmp45C->continue_stmt=(void*)(
(_tmp45E=_region_malloc(r,sizeof(*_tmp45E)),((_tmp45E[0]=((_tmp45D.tag=3,((_tmp45D.f1=continue_dest,_tmp45D)))),_tmp45E)))),((_tmp45C->break_stmt=_tmp22D->next_stmt,((_tmp45C->next_stmt=(void*)(
# 919
(_tmp460=_region_malloc(r,sizeof(*_tmp460)),((_tmp460[0]=((_tmp45F.tag=3,((_tmp45F.f1=continue_dest,_tmp45F)))),_tmp460)))),((_tmp45C->fallthru_clause=_tmp22D->fallthru_clause,((_tmp45C->try_depth=_tmp22D->try_depth,_tmp45C)))))))))))));
# 922
struct Cyc_Tcenv_Fenv*_tmp461;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp461=_region_malloc(r,sizeof(*_tmp461)),((_tmp461->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp228,((_tmp461->type_vars=(struct Cyc_List_List*)_tmp229,((_tmp461->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp22A,((_tmp461->locals=(const struct Cyc_Tcenv_Bindings*)_tmp22B,((_tmp461->encloser=(struct Cyc_Absyn_Stmt*)_tmp22C,((_tmp461->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp461->capability=(void*)_tmp22E,((_tmp461->curr_rgn=(void*)_tmp22F,((_tmp461->flags=(struct Cyc_Tcenv_FenvFlags)_tmp230,((_tmp461->fnrgn=(struct _RegionHandle*)r,_tmp461)))))))))))))))))))));
# 925
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 928
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp462;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp462="enter_try",_tag_dyneither(_tmp462,sizeof(char),10)))));
struct Cyc_Tcenv_CtrlEnv*_tmp239;struct Cyc_Tcenv_Fenv*_tmp238=fe;_tmp239=_tmp238->ctrl_env;
++ _tmp239->try_depth;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 934
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te){
const char*_tmp463;struct Cyc_Tcenv_Fenv*_tmp23B=Cyc_Tcenv_get_fenv(te,((_tmp463="get_try_depth",_tag_dyneither(_tmp463,sizeof(char),14))));struct Cyc_Tcenv_CtrlEnv*_tmp23D;struct Cyc_Tcenv_Fenv*_tmp23C=_tmp23B;_tmp23D=_tmp23C->ctrl_env;
return _tmp23D->try_depth;}
# 941
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te){
const char*_tmp464;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_new_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp464="set_in_switch",_tag_dyneither(_tmp464,sizeof(char),14)))));
struct Cyc_Tcenv_CtrlEnv*_tmp240;struct Cyc_Tcenv_Fenv*_tmp23F=fe;_tmp240=_tmp23F->ctrl_env;
_tmp240->break_stmt=_tmp240->next_stmt;
_tmp240->next_stmt=(void*)& Cyc_Tcenv_CaseEnd_j_val;
return Cyc_Tcenv_put_fenv(r,te,fe);}
# 949
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*j){
# 951
const char*_tmp465;struct Cyc_Tcenv_Fenv _tmp242=*
# 953
Cyc_Tcenv_get_fenv(te,((_tmp465="set_next",_tag_dyneither(_tmp465,sizeof(char),9))));
# 951
struct Cyc_Tcenv_SharedFenv*_tmp244;struct Cyc_List_List*_tmp245;struct Cyc_RgnOrder_RgnPO*_tmp246;const struct Cyc_Tcenv_Bindings*_tmp247;struct Cyc_Absyn_Stmt*_tmp248;struct Cyc_Tcenv_CtrlEnv*_tmp249;void*_tmp24A;void*_tmp24B;struct Cyc_Tcenv_FenvFlags _tmp24C;struct Cyc_Tcenv_Fenv _tmp243=_tmp242;_tmp244=_tmp243.shared;_tmp245=_tmp243.type_vars;_tmp246=_tmp243.region_order;_tmp247=_tmp243.locals;_tmp248=_tmp243.encloser;_tmp249=_tmp243.ctrl_env;_tmp24A=_tmp243.capability;_tmp24B=_tmp243.curr_rgn;_tmp24C=_tmp243.flags;{
# 954
struct Cyc_Tcenv_CtrlEnv*_tmp466;struct Cyc_Tcenv_CtrlEnv*new_cenv=
(_tmp466=_region_malloc(r,sizeof(*_tmp466)),((_tmp466->ctrl_rgn=r,((_tmp466->continue_stmt=_tmp249->continue_stmt,((_tmp466->break_stmt=_tmp249->break_stmt,((_tmp466->next_stmt=j,((_tmp466->fallthru_clause=_tmp249->fallthru_clause,((_tmp466->try_depth=_tmp249->try_depth,_tmp466)))))))))))));
# 962
struct Cyc_Tcenv_Fenv*_tmp467;struct Cyc_Tcenv_Fenv*new_fenv=
(_tmp467=_region_malloc(r,sizeof(*_tmp467)),((_tmp467->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp244,((_tmp467->type_vars=(struct Cyc_List_List*)_tmp245,((_tmp467->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp246,((_tmp467->locals=(const struct Cyc_Tcenv_Bindings*)_tmp247,((_tmp467->encloser=(struct Cyc_Absyn_Stmt*)_tmp248,((_tmp467->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)new_cenv,((_tmp467->capability=(void*)_tmp24A,((_tmp467->curr_rgn=(void*)_tmp24B,((_tmp467->flags=(struct Cyc_Tcenv_FenvFlags)_tmp24C,((_tmp467->fnrgn=(struct _RegionHandle*)r,_tmp467)))))))))))))))))))));
# 965
return Cyc_Tcenv_put_fenv(r,te,new_fenv);};}
# 968
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
const char*_tmp468;struct Cyc_Tcenv_Fenv*_tmp250=Cyc_Tcenv_get_fenv(te,((_tmp468="add_label",_tag_dyneither(_tmp468,sizeof(char),10))));struct Cyc_Tcenv_SharedFenv*_tmp252;struct Cyc_Tcenv_Fenv*_tmp251=_tmp250;_tmp252=_tmp251->shared;{
struct Cyc_Dict_Dict needed=_tmp252->needed_labels;
struct Cyc_List_List**sl_opt=((struct Cyc_List_List**(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup_opt)(needed,v);
struct _RegionHandle*frgn=_tmp252->frgn;
if(sl_opt != 0){
_tmp252->needed_labels=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete)(frgn,needed,v);{
# 976
struct Cyc_List_List*_tmp253=*sl_opt;
s->non_local_preds=_tmp253;
for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
void*_tmp254=((struct Cyc_Absyn_Stmt*)_tmp253->hd)->r;void*_tmp255=_tmp254;struct Cyc_Absyn_Stmt**_tmp257;_LLA4: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp256=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp255;if(_tmp256->tag != 8)goto _LLA6;else{_tmp257=(struct Cyc_Absyn_Stmt**)& _tmp256->f2;}}_LLA5:
*_tmp257=s;goto _LLA3;_LLA6:;_LLA7:
{const char*_tmp469;((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(((_tmp469="Tcenv: add_label backpatching of non-goto",_tag_dyneither(_tmp469,sizeof(char),42))));}goto _LLA3;_LLA3:;}};}
# 984
if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(_tmp252->seen_labels,v)){
const char*_tmp46D;void*_tmp46C[1];struct Cyc_String_pa_PrintArg_struct _tmp46B;(_tmp46B.tag=0,((_tmp46B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*v),((_tmp46C[0]=& _tmp46B,Cyc_Tcutil_terr(s->loc,((_tmp46D="Repeated label: %s",_tag_dyneither(_tmp46D,sizeof(char),19))),_tag_dyneither(_tmp46C,sizeof(void*),1)))))));}
_tmp252->seen_labels=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Stmt*v))Cyc_Dict_insert)(_tmp252->seen_labels,v,s);
return te;};}
# 990
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*te){
const char*_tmp46E;struct Cyc_Tcenv_Fenv*_tmp25D=Cyc_Tcenv_get_fenv(te,((_tmp46E="all_labels_resolved",_tag_dyneither(_tmp46E,sizeof(char),20))));struct Cyc_Tcenv_SharedFenv*_tmp25F;struct Cyc_Tcenv_Fenv*_tmp25E=_tmp25D;_tmp25F=_tmp25E->shared;
return((int(*)(struct Cyc_Dict_Dict d))Cyc_Dict_is_empty)(_tmp25F->needed_labels);}
# 995
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*te){
const char*_tmp46F;struct Cyc_Tcenv_Fenv*_tmp261=Cyc_Tcenv_get_fenv(te,((_tmp46F="get_encloser",_tag_dyneither(_tmp46F,sizeof(char),13))));struct Cyc_Absyn_Stmt*_tmp263;struct Cyc_Tcenv_Fenv*_tmp262=_tmp261;_tmp263=_tmp262->encloser;
return _tmp263;}
# 1000
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*r2,struct Cyc_Tcenv_Tenv*te){
if(te->le == 0){
struct Cyc_Tcenv_Tenv*_tmp470;return(_tmp470=_region_malloc(r2,sizeof(*_tmp470)),((_tmp470->ns=te->ns,((_tmp470->ae=te->ae,((_tmp470->le=0,((_tmp470->allow_valueof=1,_tmp470)))))))));}{
struct Cyc_Tcenv_Fenv _tmp266=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));struct Cyc_Tcenv_SharedFenv*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_RgnOrder_RgnPO*_tmp26A;const struct Cyc_Tcenv_Bindings*_tmp26B;struct Cyc_Absyn_Stmt*_tmp26C;struct Cyc_Tcenv_CtrlEnv*_tmp26D;void*_tmp26E;void*_tmp26F;struct Cyc_Tcenv_FenvFlags _tmp270;struct Cyc_Tcenv_Fenv _tmp267=_tmp266;_tmp268=_tmp267.shared;_tmp269=_tmp267.type_vars;_tmp26A=_tmp267.region_order;_tmp26B=_tmp267.locals;_tmp26C=_tmp267.encloser;_tmp26D=_tmp267.ctrl_env;_tmp26E=_tmp267.capability;_tmp26F=_tmp267.curr_rgn;_tmp270=_tmp267.flags;{
struct Cyc_Tcenv_Fenv*_tmp471;struct Cyc_Tcenv_Fenv*_tmp271=(_tmp471=_region_malloc(r2,sizeof(*_tmp471)),((_tmp471->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp268,((_tmp471->type_vars=(struct Cyc_List_List*)_tmp269,((_tmp471->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp26A,((_tmp471->locals=(const struct Cyc_Tcenv_Bindings*)_tmp26B,((_tmp471->encloser=(struct Cyc_Absyn_Stmt*)_tmp26C,((_tmp471->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp26D,((_tmp471->capability=(void*)_tmp26E,((_tmp471->curr_rgn=(void*)_tmp26F,((_tmp471->flags=(struct Cyc_Tcenv_FenvFlags)_tmp270,((_tmp471->fnrgn=(struct _RegionHandle*)r2,_tmp471)))))))))))))))))))));
struct Cyc_Tcenv_Tenv*_tmp472;return(_tmp472=_region_malloc(r2,sizeof(*_tmp472)),((_tmp472->ns=te->ns,((_tmp472->ae=te->ae,((_tmp472->le=_tmp271,((_tmp472->allow_valueof=1,_tmp472)))))))));};};}
# 1008
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
# 1010
const char*_tmp473;struct Cyc_Tcenv_Fenv _tmp274=*
# 1012
Cyc_Tcenv_get_fenv(te,((_tmp473="set_encloser",_tag_dyneither(_tmp473,sizeof(char),13))));
# 1010
struct Cyc_Tcenv_SharedFenv*_tmp276;struct Cyc_List_List*_tmp277;struct Cyc_RgnOrder_RgnPO*_tmp278;const struct Cyc_Tcenv_Bindings*_tmp279;struct Cyc_Absyn_Stmt*_tmp27A;struct Cyc_Tcenv_CtrlEnv*_tmp27B;void*_tmp27C;void*_tmp27D;struct Cyc_Tcenv_FenvFlags _tmp27E;struct Cyc_Tcenv_Fenv _tmp275=_tmp274;_tmp276=_tmp275.shared;_tmp277=_tmp275.type_vars;_tmp278=_tmp275.region_order;_tmp279=_tmp275.locals;_tmp27A=_tmp275.encloser;_tmp27B=_tmp275.ctrl_env;_tmp27C=_tmp275.capability;_tmp27D=_tmp275.curr_rgn;_tmp27E=_tmp275.flags;{
# 1013
struct Cyc_Tcenv_Fenv*_tmp474;struct Cyc_Tcenv_Fenv*_tmp27F=
(_tmp474=_region_malloc(r,sizeof(*_tmp474)),((_tmp474->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp276,((_tmp474->type_vars=(struct Cyc_List_List*)_tmp277,((_tmp474->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp278,((_tmp474->locals=(const struct Cyc_Tcenv_Bindings*)_tmp279,((_tmp474->encloser=(struct Cyc_Absyn_Stmt*)s,((_tmp474->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp27B,((_tmp474->capability=(void*)_tmp27C,((_tmp474->curr_rgn=(void*)_tmp27D,((_tmp474->flags=(struct Cyc_Tcenv_FenvFlags)_tmp27E,((_tmp474->fnrgn=(struct _RegionHandle*)r,_tmp474)))))))))))))))))))));
# 1016
return Cyc_Tcenv_put_fenv(r,te,_tmp27F);};}
# 1019
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*rgn,int resetable,int opened){
# 1022
const char*_tmp475;struct Cyc_Tcenv_Fenv _tmp282=*
# 1024
Cyc_Tcenv_get_fenv(te,((_tmp475="add_region",_tag_dyneither(_tmp475,sizeof(char),11))));
# 1022
struct Cyc_Tcenv_SharedFenv*_tmp284;struct Cyc_List_List*_tmp285;struct Cyc_RgnOrder_RgnPO*_tmp286;const struct Cyc_Tcenv_Bindings*_tmp287;struct Cyc_Absyn_Stmt*_tmp288;struct Cyc_Tcenv_CtrlEnv*_tmp289;void*_tmp28A;void*_tmp28B;struct Cyc_Tcenv_FenvFlags _tmp28C;struct Cyc_Tcenv_Fenv _tmp283=_tmp282;_tmp284=_tmp283.shared;_tmp285=_tmp283.type_vars;_tmp286=_tmp283.region_order;_tmp287=_tmp283.locals;_tmp288=_tmp283.encloser;_tmp289=_tmp283.ctrl_env;_tmp28A=_tmp283.capability;_tmp28B=_tmp283.curr_rgn;_tmp28C=_tmp283.flags;
# 1025
{void*_tmp28D=Cyc_Tcutil_compress(rgn);void*_tmp28E=_tmp28D;struct Cyc_Absyn_Tvar*_tmp290;_LLA9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp28F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp28E;if(_tmp28F->tag != 2)goto _LLAB;else{_tmp290=_tmp28F->f1;}}_LLAA:
# 1027
 _tmp286=Cyc_RgnOrder_add_youngest(_tmp284->frgn,_tmp286,_tmp290,resetable,opened);
# 1029
goto _LLA8;_LLAB:;_LLAC:
 goto _LLA8;_LLA8:;}
# 1032
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp488;struct Cyc_List_List*_tmp487;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp486;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp485;struct Cyc_List_List*_tmp484;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp483;_tmp28A=(void*)((_tmp483=_cycalloc(sizeof(*_tmp483)),((_tmp483[0]=((_tmp488.tag=24,((_tmp488.f1=((_tmp484=_cycalloc(sizeof(*_tmp484)),((_tmp484->hd=(void*)((_tmp486=_cycalloc(sizeof(*_tmp486)),((_tmp486[0]=((_tmp485.tag=23,((_tmp485.f1=rgn,_tmp485)))),_tmp486)))),((_tmp484->tl=(
(_tmp487=_cycalloc(sizeof(*_tmp487)),((_tmp487->hd=_tmp28A,((_tmp487->tl=0,_tmp487)))))),_tmp484)))))),_tmp488)))),_tmp483))));}{
struct Cyc_Tcenv_Fenv*_tmp489;struct Cyc_Tcenv_Fenv*_tmp297=
(_tmp489=_region_malloc(r,sizeof(*_tmp489)),((_tmp489->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp284,((_tmp489->type_vars=(struct Cyc_List_List*)_tmp285,((_tmp489->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp286,((_tmp489->locals=(const struct Cyc_Tcenv_Bindings*)_tmp287,((_tmp489->encloser=(struct Cyc_Absyn_Stmt*)_tmp288,((_tmp489->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp289,((_tmp489->capability=(void*)_tmp28A,((_tmp489->curr_rgn=(void*)_tmp28B,((_tmp489->flags=(struct Cyc_Tcenv_FenvFlags)_tmp28C,((_tmp489->fnrgn=(struct _RegionHandle*)r,_tmp489)))))))))))))))))))));
# 1037
return Cyc_Tcenv_put_fenv(r,te,_tmp297);};}
# 1040
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
# 1042
const char*_tmp48A;struct Cyc_Tcenv_Fenv _tmp29A=*
# 1044
Cyc_Tcenv_get_fenv(te,((_tmp48A="new_named_block",_tag_dyneither(_tmp48A,sizeof(char),16))));
# 1042
struct Cyc_Tcenv_SharedFenv*_tmp29C;struct Cyc_List_List*_tmp29D;struct Cyc_RgnOrder_RgnPO*_tmp29E;const struct Cyc_Tcenv_Bindings*_tmp29F;struct Cyc_Absyn_Stmt*_tmp2A0;struct Cyc_Tcenv_CtrlEnv*_tmp2A1;void*_tmp2A2;void*_tmp2A3;struct Cyc_Tcenv_FenvFlags _tmp2A4;struct Cyc_Tcenv_Fenv _tmp29B=_tmp29A;_tmp29C=_tmp29B.shared;_tmp29D=_tmp29B.type_vars;_tmp29E=_tmp29B.region_order;_tmp29F=_tmp29B.locals;_tmp2A0=_tmp29B.encloser;_tmp2A1=_tmp29B.ctrl_env;_tmp2A2=_tmp29B.capability;_tmp2A3=_tmp29B.curr_rgn;_tmp2A4=_tmp29B.flags;{
# 1046
const char*_tmp48B;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_copy_fenv_old_ctrl(r,Cyc_Tcenv_get_fenv(te,((_tmp48B="new_block",_tag_dyneither(_tmp48B,sizeof(char),10)))));
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp48E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp48D;void*block_typ=(void*)((_tmp48D=_cycalloc(sizeof(*_tmp48D)),((_tmp48D[0]=((_tmp48E.tag=2,((_tmp48E.f1=block_rgn,_tmp48E)))),_tmp48D))));
{struct Cyc_List_List*_tmp48F;_tmp29D=((_tmp48F=_cycalloc(sizeof(*_tmp48F)),((_tmp48F->hd=block_rgn,((_tmp48F->tl=_tmp29D,_tmp48F))))));}
Cyc_Tcutil_check_unique_tvars(loc,_tmp29D);
_tmp29E=Cyc_RgnOrder_add_youngest(_tmp29C->frgn,_tmp29E,block_rgn,0,0);
{struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4A2;struct Cyc_List_List*_tmp4A1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4A0;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp49F;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49D;_tmp2A2=(void*)((_tmp49D=_cycalloc(sizeof(*_tmp49D)),((_tmp49D[0]=((_tmp4A2.tag=24,((_tmp4A2.f1=((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E->hd=(void*)((_tmp4A0=_cycalloc(sizeof(*_tmp4A0)),((_tmp4A0[0]=((_tmp49F.tag=23,((_tmp49F.f1=block_typ,_tmp49F)))),_tmp4A0)))),((_tmp49E->tl=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->hd=_tmp2A2,((_tmp4A1->tl=0,_tmp4A1)))))),_tmp49E)))))),_tmp4A2)))),_tmp49D))));}
_tmp2A3=block_typ;{
struct Cyc_Tcenv_Fenv*_tmp4A3;struct Cyc_Tcenv_Fenv*_tmp2AC=
(_tmp4A3=_region_malloc(r,sizeof(*_tmp4A3)),((_tmp4A3->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp29C,((_tmp4A3->type_vars=(struct Cyc_List_List*)_tmp29D,((_tmp4A3->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp29E,((_tmp4A3->locals=(const struct Cyc_Tcenv_Bindings*)_tmp29F,((_tmp4A3->encloser=(struct Cyc_Absyn_Stmt*)_tmp2A0,((_tmp4A3->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2A1,((_tmp4A3->capability=(void*)_tmp2A2,((_tmp4A3->curr_rgn=(void*)_tmp2A3,((_tmp4A3->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2A4,((_tmp4A3->fnrgn=(struct _RegionHandle*)r,_tmp4A3)))))))))))))))))))));
# 1056
return Cyc_Tcenv_put_fenv(r,te,_tmp2AC);};};}
# 1059
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0,& Cyc_Tcutil_rk};
# 1061
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*r,unsigned int loc,struct Cyc_Tcenv_Tenv*te){
# 1063
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(r,loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 1069
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 1073
const char*_tmp4A4;struct Cyc_Tcenv_Fenv _tmp2B3=*
# 1075
Cyc_Tcenv_get_fenv(te,((_tmp4A4="new_outlives_constraints",_tag_dyneither(_tmp4A4,sizeof(char),25))));
# 1073
struct Cyc_Tcenv_SharedFenv*_tmp2B5;struct Cyc_List_List*_tmp2B6;struct Cyc_RgnOrder_RgnPO*_tmp2B7;const struct Cyc_Tcenv_Bindings*_tmp2B8;struct Cyc_Absyn_Stmt*_tmp2B9;struct Cyc_Tcenv_CtrlEnv*_tmp2BA;void*_tmp2BB;void*_tmp2BC;struct Cyc_Tcenv_FenvFlags _tmp2BD;struct Cyc_Tcenv_Fenv _tmp2B4=_tmp2B3;_tmp2B5=_tmp2B4.shared;_tmp2B6=_tmp2B4.type_vars;_tmp2B7=_tmp2B4.region_order;_tmp2B8=_tmp2B4.locals;_tmp2B9=_tmp2B4.encloser;_tmp2BA=_tmp2B4.ctrl_env;_tmp2BB=_tmp2B4.capability;_tmp2BC=_tmp2B4.curr_rgn;_tmp2BD=_tmp2B4.flags;
# 1076
for(0;cs != 0;cs=cs->tl){
_tmp2B7=
Cyc_RgnOrder_add_outlives_constraint(_tmp2B5->frgn,_tmp2B7,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);}{
# 1081
struct Cyc_Tcenv_Fenv*_tmp4A5;struct Cyc_Tcenv_Fenv*_tmp2BE=
(_tmp4A5=_region_malloc(r,sizeof(*_tmp4A5)),((_tmp4A5->shared=(struct Cyc_Tcenv_SharedFenv*)_tmp2B5,((_tmp4A5->type_vars=(struct Cyc_List_List*)_tmp2B6,((_tmp4A5->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp2B7,((_tmp4A5->locals=(const struct Cyc_Tcenv_Bindings*)_tmp2B8,((_tmp4A5->encloser=(struct Cyc_Absyn_Stmt*)_tmp2B9,((_tmp4A5->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)_tmp2BA,((_tmp4A5->capability=(void*)_tmp2BB,((_tmp4A5->curr_rgn=(void*)_tmp2BC,((_tmp4A5->flags=(struct Cyc_Tcenv_FenvFlags)_tmp2BD,((_tmp4A5->fnrgn=(struct _RegionHandle*)r,_tmp4A5)))))))))))))))))))));
# 1084
return Cyc_Tcenv_put_fenv(r,te,_tmp2BE);};}
# 1087
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 1095
void*_tmp2C1=Cyc_Tcutil_compress(r1);
void*_tmp2C2=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp2C3=Cyc_Tcutil_typ_kind(_tmp2C1);
struct Cyc_Absyn_Kind*_tmp2C4=Cyc_Tcutil_typ_kind(_tmp2C2);
# 1101
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp2C3,_tmp2C4);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp2C4,_tmp2C3);
if(!r1_le_r2  && !r2_le_r1){
{const char*_tmp4AC;void*_tmp4AB[4];struct Cyc_String_pa_PrintArg_struct _tmp4AA;struct Cyc_String_pa_PrintArg_struct _tmp4A9;struct Cyc_String_pa_PrintArg_struct _tmp4A8;struct Cyc_String_pa_PrintArg_struct _tmp4A7;(_tmp4A7.tag=0,((_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp2C4)),((_tmp4A8.tag=0,((_tmp4A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp2C3)),((_tmp4A9.tag=0,((_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C2)),((_tmp4AA.tag=0,((_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp2C1)),((_tmp4AB[0]=& _tmp4AA,((_tmp4AB[1]=& _tmp4A9,((_tmp4AB[2]=& _tmp4A8,((_tmp4AB[3]=& _tmp4A7,Cyc_Tcutil_terr(loc,((_tmp4AC="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n",_tag_dyneither(_tmp4AC,sizeof(char),82))),_tag_dyneither(_tmp4AB,sizeof(void*),4)))))))))))))))))))))))));}
return Cyc_Tcenv_new_outlives_constraints(r,te,0,loc);}else{
# 1108
if(r1_le_r2  && !r2_le_r1){
struct _tuple11 _tmp2CB=Cyc_Tcutil_swap_kind(_tmp2C2,Cyc_Tcutil_kind_to_bound(_tmp2C3));struct Cyc_Absyn_Tvar*_tmp2CD;void*_tmp2CE;struct _tuple11 _tmp2CC=_tmp2CB;_tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;{
struct _tuple11*_tmp4AD;*oldtv=((_tmp4AD=_region_malloc(r,sizeof(*_tmp4AD)),((_tmp4AD->f1=_tmp2CD,((_tmp4AD->f2=_tmp2CE,_tmp4AD))))));};}else{
# 1112
if(!r1_le_r2  && r2_le_r1){
struct _tuple11 _tmp2D0=Cyc_Tcutil_swap_kind(_tmp2C1,Cyc_Tcutil_kind_to_bound(_tmp2C4));struct Cyc_Absyn_Tvar*_tmp2D2;void*_tmp2D3;struct _tuple11 _tmp2D1=_tmp2D0;_tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;{
struct _tuple11*_tmp4AE;*oldtv=((_tmp4AE=_region_malloc(r,sizeof(*_tmp4AE)),((_tmp4AE->f1=_tmp2D2,((_tmp4AE->f2=_tmp2D3,_tmp4AE))))));};}}}{
# 1118
struct _RegionHandle*_tmp2D5=Cyc_Tcenv_get_fnrgn(te);
struct Cyc_List_List*_tmp2D6=0;
if((_tmp2C1 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2C1 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2C1 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4B1;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4B0;void*eff1=(void*)((_tmp4B0=_cycalloc(sizeof(*_tmp4B0)),((_tmp4B0[0]=((_tmp4B1.tag=23,((_tmp4B1.f1=_tmp2C1,_tmp4B1)))),_tmp4B0))));
struct _tuple13*_tmp4B4;struct Cyc_List_List*_tmp4B3;_tmp2D6=((_tmp4B3=_region_malloc(_tmp2D5,sizeof(*_tmp4B3)),((_tmp4B3->hd=((_tmp4B4=_region_malloc(_tmp2D5,sizeof(*_tmp4B4)),((_tmp4B4->f1=eff1,((_tmp4B4->f2=_tmp2C2,_tmp4B4)))))),((_tmp4B3->tl=_tmp2D6,_tmp4B3))))));}
# 1124
if((_tmp2C2 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp2C2 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp2C2 != (void*)& Cyc_Absyn_RefCntRgn_val){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4B7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4B6;void*eff2=(void*)((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6[0]=((_tmp4B7.tag=23,((_tmp4B7.f1=_tmp2C2,_tmp4B7)))),_tmp4B6))));
struct _tuple13*_tmp4BA;struct Cyc_List_List*_tmp4B9;_tmp2D6=((_tmp4B9=_region_malloc(_tmp2D5,sizeof(*_tmp4B9)),((_tmp4B9->hd=((_tmp4BA=_region_malloc(_tmp2D5,sizeof(*_tmp4BA)),((_tmp4BA->f1=eff2,((_tmp4BA->f2=_tmp2C1,_tmp4BA)))))),((_tmp4B9->tl=_tmp2D6,_tmp4B9))))));}
# 1129
return Cyc_Tcenv_new_outlives_constraints(_tmp2D5,te,_tmp2D6,loc);};}
# 1132
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2DF=te->le;
if(_tmp2DF == 0)return(void*)& Cyc_Absyn_HeapRgn_val;{
struct Cyc_Tcenv_Fenv*fe=_tmp2DF;
void*_tmp2E1;struct Cyc_Tcenv_Fenv*_tmp2E0=fe;_tmp2E1=_tmp2E0->curr_rgn;
return _tmp2E1;};}
# 1143
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
const char*_tmp4BB;struct Cyc_Tcenv_Fenv*fe=Cyc_Tcenv_get_fenv(te,((_tmp4BB="check_rgn_accessible",_tag_dyneither(_tmp4BB,sizeof(char),21))));
void*_tmp2E3;struct Cyc_RgnOrder_RgnPO*_tmp2E4;struct Cyc_Tcenv_Fenv*_tmp2E2=fe;_tmp2E3=_tmp2E2->capability;_tmp2E4=_tmp2E2->region_order;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp2E3) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp2E3))
return;
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4BE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4BD;if(Cyc_RgnOrder_eff_outlives_eff(_tmp2E4,(void*)((_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD[0]=((_tmp4BE.tag=23,((_tmp4BE.f1=rgn,_tmp4BE)))),_tmp4BD)))),_tmp2E3))
return;}{
const char*_tmp4C2;void*_tmp4C1[1];struct Cyc_String_pa_PrintArg_struct _tmp4C0;(_tmp4C0.tag=0,((_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4C1[0]=& _tmp4C0,Cyc_Tcutil_terr(loc,((_tmp4C2="Expression accesses unavailable region %s",_tag_dyneither(_tmp4C2,sizeof(char),42))),_tag_dyneither(_tmp4C1,sizeof(void*),1)))))));};}
# 1155
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);{
const char*_tmp4C3;struct Cyc_Tcenv_Fenv*_tmp2EB=
Cyc_Tcenv_get_fenv(te,((_tmp4C3="check_rgn_resetable",_tag_dyneither(_tmp4C3,sizeof(char),20))));
# 1157
struct Cyc_RgnOrder_RgnPO*_tmp2ED;struct Cyc_Tcenv_Fenv*_tmp2EC=_tmp2EB;_tmp2ED=_tmp2EC->region_order;{
# 1159
void*_tmp2EE=Cyc_Tcutil_compress(rgn);void*_tmp2EF=_tmp2EE;struct Cyc_Absyn_Tvar*_tmp2F1;_LLAE: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2EF;if(_tmp2F0->tag != 2)goto _LLB0;else{_tmp2F1=_tmp2F0->f1;}}_LLAF:
# 1161
 if(!Cyc_RgnOrder_is_region_resetable(_tmp2ED,_tmp2F1)){
const char*_tmp4C7;void*_tmp4C6[1];struct Cyc_String_pa_PrintArg_struct _tmp4C5;(_tmp4C5.tag=0,((_tmp4C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn)),((_tmp4C6[0]=& _tmp4C5,Cyc_Tcutil_terr(loc,((_tmp4C7="Region %s is not resetable",_tag_dyneither(_tmp4C7,sizeof(char),27))),_tag_dyneither(_tmp4C6,sizeof(void*),1)))))));}
return;_LLB0:;_LLB1: {
# 1165
const char*_tmp4CA;void*_tmp4C9;(_tmp4C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4CA="check_rgn_resetable",_tag_dyneither(_tmp4CA,sizeof(char),20))),_tag_dyneither(_tmp4C9,sizeof(void*),0)));}_LLAD:;};};}
# 1172
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp2F8=te->le;
rt_a=Cyc_Tcutil_compress(rt_a);
rt_b=Cyc_Tcutil_compress(rt_b);
# 1177
if(_tmp2F8 == 0){
void*_tmp2F9=rt_a;_LLB3: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp2FA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2F9;if(_tmp2FA->tag != 22)goto _LLB5;}_LLB4:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;_LLB5: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp2FB=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2F9;if(_tmp2FB->tag != 21)goto _LLB7;}_LLB6:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;_LLB7: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2F9;if(_tmp2FC->tag != 20)goto _LLB9;}_LLB8:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;_LLB9:;_LLBA:
 return 0;_LLB2:;}{
# 1185
struct Cyc_Tcenv_Fenv*fe=_tmp2F8;
struct Cyc_RgnOrder_RgnPO*_tmp2FE;struct Cyc_Tcenv_Fenv*_tmp2FD=fe;_tmp2FE=_tmp2FD->region_order;{
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4CD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4CC;int _tmp2FF=Cyc_RgnOrder_effect_outlives(_tmp2FE,(void*)((_tmp4CC=_cycalloc(sizeof(*_tmp4CC)),((_tmp4CC[0]=((_tmp4CD.tag=23,((_tmp4CD.f1=rt_a,_tmp4CD)))),_tmp4CC)))),rt_b);
# 1191
return _tmp2FF;};};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 1196
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
const char*_tmp4CE;struct Cyc_Tcenv_Fenv*_tmp302=
Cyc_Tcenv_get_fenv(te,((_tmp4CE="check_effect_accessible",_tag_dyneither(_tmp4CE,sizeof(char),24))));
# 1197
void*_tmp304;struct Cyc_RgnOrder_RgnPO*_tmp305;struct Cyc_Tcenv_SharedFenv*_tmp306;struct Cyc_Tcenv_Fenv*_tmp303=_tmp302;_tmp304=_tmp303->capability;_tmp305=_tmp303->region_order;_tmp306=_tmp303->shared;
# 1199
if(Cyc_Tcutil_subset_effect(0,eff,_tmp304))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp305,eff,_tmp304))
return;{
struct _RegionHandle*frgn=_tmp306->frgn;
struct _tuple14*_tmp4D1;struct Cyc_List_List*_tmp4D0;_tmp306->delayed_effect_checks=(
(_tmp4D0=_region_malloc(frgn,sizeof(*_tmp4D0)),((_tmp4D0->hd=((_tmp4D1=_region_malloc(frgn,sizeof(*_tmp4D1)),((_tmp4D1->f1=_tmp304,((_tmp4D1->f2=eff,((_tmp4D1->f3=_tmp305,((_tmp4D1->f4=loc,_tmp4D1)))))))))),((_tmp4D0->tl=_tmp306->delayed_effect_checks,_tmp4D0))))));};}
# 1208
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4D2;struct Cyc_Tcenv_Fenv*_tmp30A=
Cyc_Tcenv_get_fenv(te,((_tmp4D2="check_delayed_effects",_tag_dyneither(_tmp4D2,sizeof(char),22))));
# 1209
struct Cyc_Tcenv_SharedFenv*_tmp30C;struct Cyc_Tcenv_Fenv*_tmp30B=_tmp30A;_tmp30C=_tmp30B->shared;{
# 1211
struct Cyc_List_List*_tmp30D=_tmp30C->delayed_effect_checks;
for(0;_tmp30D != 0;_tmp30D=_tmp30D->tl){
struct _tuple14*_tmp30E=(struct _tuple14*)_tmp30D->hd;void*_tmp310;void*_tmp311;struct Cyc_RgnOrder_RgnPO*_tmp312;unsigned int _tmp313;struct _tuple14*_tmp30F=_tmp30E;_tmp310=_tmp30F->f1;_tmp311=_tmp30F->f2;_tmp312=_tmp30F->f3;_tmp313=_tmp30F->f4;
if(Cyc_Tcutil_subset_effect(1,_tmp311,_tmp310))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp312,_tmp311,_tmp310))
continue;{
const char*_tmp4D7;void*_tmp4D6[2];struct Cyc_String_pa_PrintArg_struct _tmp4D5;struct Cyc_String_pa_PrintArg_struct _tmp4D4;(_tmp4D4.tag=0,((_tmp4D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp311)),((_tmp4D5.tag=0,((_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp310)),((_tmp4D6[0]=& _tmp4D5,((_tmp4D6[1]=& _tmp4D4,Cyc_Tcutil_terr(_tmp313,((_tmp4D7="Capability \n%s\ndoes not cover function's effect\n%s",_tag_dyneither(_tmp4D7,sizeof(char),51))),_tag_dyneither(_tmp4D6,sizeof(void*),2)))))))))))));};}};}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 1227
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 1229
struct Cyc_Tcenv_Fenv*_tmp319=te->le;
if(_tmp319 == 0){
# 1232
for(0;po != 0;po=po->tl){
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4DA;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D9;if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !
Cyc_Tcutil_subset_effect(1,(void*)((_tmp4D9=_cycalloc(sizeof(*_tmp4D9)),((_tmp4D9[0]=((_tmp4DA.tag=23,((_tmp4DA.f1=(*((struct _tuple13*)po->hd)).f2,_tmp4DA)))),_tmp4D9)))),Cyc_Absyn_empty_effect)){
const char*_tmp4DD;void*_tmp4DC;(_tmp4DC=0,Cyc_Tcutil_terr(loc,((_tmp4DD="the required region ordering is not satisfied here",_tag_dyneither(_tmp4DD,sizeof(char),51))),_tag_dyneither(_tmp4DC,sizeof(void*),0)));}}
return;}{
# 1238
struct Cyc_Tcenv_Fenv*_tmp31E=_tmp319;struct Cyc_RgnOrder_RgnPO*_tmp320;struct Cyc_Tcenv_SharedFenv*_tmp321;struct Cyc_Tcenv_Fenv*_tmp31F=_tmp31E;_tmp320=_tmp31F->region_order;_tmp321=_tmp31F->shared;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp320,po,(void*)& Cyc_Absyn_HeapRgn_val,0)){
struct _tuple15*_tmp4E0;struct Cyc_List_List*_tmp4DF;_tmp321->delayed_constraint_checks=(
(_tmp4DF=_region_malloc(_tmp321->frgn,sizeof(*_tmp4DF)),((_tmp4DF->hd=((_tmp4E0=_region_malloc(_tmp321->frgn,sizeof(*_tmp4E0)),((_tmp4E0->f1=_tmp320,((_tmp4E0->f2=po,((_tmp4E0->f3=loc,_tmp4E0)))))))),((_tmp4DF->tl=_tmp321->delayed_constraint_checks,_tmp4DF))))));}};}
# 1245
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
const char*_tmp4E1;struct Cyc_Tcenv_Fenv*_tmp324=
Cyc_Tcenv_get_fenv(te,((_tmp4E1="check_delayed_constraints",_tag_dyneither(_tmp4E1,sizeof(char),26))));
# 1246
struct Cyc_Tcenv_SharedFenv*_tmp326;struct Cyc_Tcenv_Fenv*_tmp325=_tmp324;_tmp326=_tmp325->shared;{
# 1248
struct Cyc_List_List*_tmp327=_tmp326->delayed_constraint_checks;
for(0;_tmp327 != 0;_tmp327=_tmp327->tl){
struct _tuple15*_tmp328=(struct _tuple15*)_tmp327->hd;struct Cyc_RgnOrder_RgnPO*_tmp32A;struct Cyc_List_List*_tmp32B;unsigned int _tmp32C;struct _tuple15*_tmp329=_tmp328;_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;_tmp32C=_tmp329->f3;
if(!Cyc_RgnOrder_satisfies_constraints(_tmp32A,_tmp32B,(void*)& Cyc_Absyn_HeapRgn_val,1)){
const char*_tmp4E4;void*_tmp4E3;(_tmp4E3=0,Cyc_Tcutil_terr(_tmp32C,((_tmp4E4="the required region ordering is not satisfied here",_tag_dyneither(_tmp4E4,sizeof(char),51))),_tag_dyneither(_tmp4E3,sizeof(void*),0)));}}};}
# 1256
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*r,unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*locals=0;
struct _dyneither_ptr*_tmp4F1;const char*_tmp4F0;void*_tmp4EF[1];struct Cyc_String_pa_PrintArg_struct _tmp4EE;struct Cyc_Absyn_Tvar*_tmp4ED;struct Cyc_Absyn_Tvar*rgn0=
(_tmp4ED=_cycalloc(sizeof(*_tmp4ED)),((_tmp4ED->name=((_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=(struct _dyneither_ptr)((_tmp4EE.tag=0,((_tmp4EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp4EF[0]=& _tmp4EE,Cyc_aprintf(((_tmp4F0="`%s",_tag_dyneither(_tmp4F0,sizeof(char),4))),_tag_dyneither(_tmp4EF,sizeof(void*),1)))))))),_tmp4F1)))),((_tmp4ED->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp4ED->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp4ED)))))));
struct Cyc_List_List*_tmp4F2;struct Cyc_List_List*_tmp330=(_tmp4F2=_cycalloc(sizeof(*_tmp4F2)),((_tmp4F2->hd=rgn0,((_tmp4F2->tl=fd->tvs,_tmp4F2)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp330);{
# 1264
struct Cyc_RgnOrder_RgnPO*_tmp331=Cyc_RgnOrder_initial_fn_po(r,fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
# 1266
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4F5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F4;void*param_rgn=(void*)((_tmp4F4=_cycalloc(sizeof(*_tmp4F4)),((_tmp4F4[0]=((_tmp4F5.tag=2,((_tmp4F5.f1=rgn0,_tmp4F5)))),_tmp4F4))));
struct Cyc_List_List*_tmp332=0;
{struct Cyc_List_List*_tmp333=fd->args;for(0;_tmp333 != 0;_tmp333=_tmp333->tl){
struct Cyc_Absyn_Vardecl _tmp4FB;struct _tuple0*_tmp4FA;struct Cyc_Absyn_Vardecl*_tmp4F9;struct Cyc_Absyn_Vardecl*_tmp334=(_tmp4F9=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp4F9[0]=(struct Cyc_Absyn_Vardecl)((_tmp4FB.sc=Cyc_Absyn_Public,((_tmp4FB.name=(
(_tmp4FA=_cycalloc(sizeof(*_tmp4FA)),((_tmp4FA->f1=Cyc_Absyn_Loc_n,((_tmp4FA->f2=(*((struct _tuple8*)_tmp333->hd)).f1,_tmp4FA)))))),((_tmp4FB.tq=(*((struct _tuple8*)_tmp333->hd)).f2,((_tmp4FB.type=(*((struct _tuple8*)_tmp333->hd)).f3,((_tmp4FB.initializer=0,((_tmp4FB.rgn=param_rgn,((_tmp4FB.attributes=0,((_tmp4FB.escapes=0,_tmp4FB)))))))))))))))),_tmp4F9)));
# 1276
{struct Cyc_List_List _tmp4FE;struct Cyc_List_List*_tmp4FD;_tmp332=((_tmp4FD=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp4FD[0]=(struct Cyc_List_List)((_tmp4FE.hd=_tmp334,((_tmp4FE.tl=_tmp332,_tmp4FE)))),_tmp4FD))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp501;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp500;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp337=(_tmp500=_cycalloc(sizeof(*_tmp500)),((_tmp500[0]=((_tmp501.tag=3,((_tmp501.f1=_tmp334,_tmp501)))),_tmp500)));
struct _dyneither_ptr*_tmp338=(*((struct _tuple8*)_tmp333->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp502;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp502=_region_malloc(r,sizeof(*_tmp502)),((_tmp502->v=_tmp338,((_tmp502->b=(void*)_tmp337,((_tmp502->tl=locals,_tmp502))))))));};}}
# 1281
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp33F=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp341;struct Cyc_Absyn_Tqual _tmp342;void*_tmp343;int _tmp344;struct Cyc_Absyn_VarargInfo _tmp340=_tmp33F;_tmp341=_tmp340.name;_tmp342=_tmp340.tq;_tmp343=_tmp340.type;_tmp344=_tmp340.inject;
if(_tmp341 != 0){
void*_tmp345=Cyc_Absyn_dyneither_typ(_tmp343,param_rgn,_tmp342,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp508;struct _tuple0*_tmp507;struct Cyc_Absyn_Vardecl*_tmp506;struct Cyc_Absyn_Vardecl*_tmp346=(_tmp506=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp506[0]=(struct Cyc_Absyn_Vardecl)((_tmp508.sc=Cyc_Absyn_Public,((_tmp508.name=(
(_tmp507=_cycalloc(sizeof(*_tmp507)),((_tmp507->f1=Cyc_Absyn_Loc_n,((_tmp507->f2=_tmp341,_tmp507)))))),((_tmp508.tq=
Cyc_Absyn_empty_tqual(0),((_tmp508.type=_tmp345,((_tmp508.initializer=0,((_tmp508.rgn=param_rgn,((_tmp508.attributes=0,((_tmp508.escapes=0,_tmp508)))))))))))))))),_tmp506)));
# 1292
{struct Cyc_List_List*_tmp509;_tmp332=((_tmp509=_cycalloc(sizeof(*_tmp509)),((_tmp509->hd=_tmp346,((_tmp509->tl=_tmp332,_tmp509))))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp50C;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp50B;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp348=(_tmp50B=_cycalloc(sizeof(*_tmp50B)),((_tmp50B[0]=((_tmp50C.tag=3,((_tmp50C.f1=_tmp346,_tmp50C)))),_tmp50B)));
struct _dyneither_ptr*_tmp349=_tmp341;
struct Cyc_Tcenv_Bindings*_tmp50D;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp50D=_region_malloc(r,sizeof(*_tmp50D)),((_tmp50D->v=_tmp349,((_tmp50D->b=(void*)_tmp348,((_tmp50D->tl=locals,_tmp50D))))))));};}else{
# 1297
const char*_tmp510;void*_tmp50F;(_tmp50F=0,Cyc_Tcutil_terr(loc,((_tmp510="missing name for varargs",_tag_dyneither(_tmp510,sizeof(char),25))),_tag_dyneither(_tmp50F,sizeof(void*),0)));}}
# 1299
{struct Cyc_Core_Opt _tmp513;struct Cyc_Core_Opt*_tmp512;fd->param_vardecls=((_tmp512=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp512[0]=(struct Cyc_Core_Opt)((_tmp513.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp332),_tmp513)),_tmp512))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp52F;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp52E;struct Cyc_List_List*_tmp52D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp52C;struct Cyc_Tcenv_CtrlEnv*_tmp52B;struct Cyc_Tcenv_SharedFenv*_tmp52A;struct Cyc_Tcenv_Fenv*_tmp529;return(_tmp529=_region_malloc(r,sizeof(*_tmp529)),((_tmp529->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A->frgn=r,((_tmp52A->return_typ=fd->ret_type,((_tmp52A->seen_labels=
# 1304
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp52A->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp52A->delayed_effect_checks=0,((_tmp52A->delayed_constraint_checks=0,_tmp52A)))))))))))))),((_tmp529->type_vars=(struct Cyc_List_List*)_tmp330,((_tmp529->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp331,((_tmp529->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp529->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp529->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1313
(_tmp52B=_region_malloc(r,sizeof(*_tmp52B)),((_tmp52B->ctrl_rgn=r,((_tmp52B->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp52B->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp52B->fallthru_clause=0,((_tmp52B->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp52B->try_depth=0,_tmp52B)))))))))))))),((_tmp529->capability=(void*)((void*)(
(_tmp531=_cycalloc(sizeof(*_tmp531)),((_tmp531[0]=((_tmp52C.tag=24,((_tmp52C.f1=((_tmp530=_cycalloc(sizeof(*_tmp530)),((_tmp530->hd=(void*)((_tmp52E=_cycalloc(sizeof(*_tmp52E)),((_tmp52E[0]=((_tmp52F.tag=23,((_tmp52F.f1=param_rgn,_tmp52F)))),_tmp52E)))),((_tmp530->tl=(
(_tmp52D=_cycalloc(sizeof(*_tmp52D)),((_tmp52D->hd=(void*)_check_null(fd->effect),((_tmp52D->tl=0,_tmp52D)))))),_tmp530)))))),_tmp52C)))),_tmp531))))),((_tmp529->curr_rgn=(void*)param_rgn,((_tmp529->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1317
((_tmp529->flags).in_notreadctxt=0,(((_tmp529->flags).in_lhs=0,(((_tmp529->flags).in_new=0,(((_tmp529->flags).abstract_ok=0,_tmp529->flags)))))))),((_tmp529->fnrgn=(struct _RegionHandle*)r,_tmp529)))))))))))))))))))));};};}
# 1322
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*r,void*ret_type,struct Cyc_List_List*args){
# 1325
const struct Cyc_Tcenv_Bindings*locals=0;
struct Cyc_Absyn_Tvar*_tmp532;struct Cyc_Absyn_Tvar*rgn0=(_tmp532=_cycalloc(sizeof(*_tmp532)),((_tmp532->name=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6),((_tmp532->identity=Cyc_Tcutil_new_tvar_id(),((_tmp532->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp532)))))));
struct Cyc_List_List*_tmp533;struct Cyc_List_List*_tmp365=(_tmp533=_cycalloc(sizeof(*_tmp533)),((_tmp533->hd=rgn0,((_tmp533->tl=0,_tmp533)))));
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp536;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp535;struct Cyc_RgnOrder_RgnPO*_tmp366=Cyc_RgnOrder_initial_fn_po(r,0,0,(void*)((_tmp535=_cycalloc(sizeof(*_tmp535)),((_tmp535[0]=((_tmp536.tag=24,((_tmp536.f1=0,_tmp536)))),_tmp535)))),rgn0,0);
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp539;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp538;void*param_rgn=(void*)((_tmp538=_cycalloc(sizeof(*_tmp538)),((_tmp538[0]=((_tmp539.tag=2,((_tmp539.f1=rgn0,_tmp539)))),_tmp538))));
struct Cyc_List_List*_tmp367=0;
for(0;args != 0;args=args->tl){
struct _tuple8 _tmp368=*((struct _tuple8*)args->hd);struct _dyneither_ptr*_tmp36A;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36C;struct _tuple8 _tmp369=_tmp368;_tmp36A=_tmp369.f1;_tmp36B=_tmp369.f2;_tmp36C=_tmp369.f3;
if(_tmp36A != 0){
struct Cyc_Absyn_Vardecl _tmp53F;struct _tuple0*_tmp53E;struct Cyc_Absyn_Vardecl*_tmp53D;struct Cyc_Absyn_Vardecl*_tmp36D=(_tmp53D=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp53D[0]=(struct Cyc_Absyn_Vardecl)((_tmp53F.sc=Cyc_Absyn_Public,((_tmp53F.name=(
(_tmp53E=_cycalloc(sizeof(*_tmp53E)),((_tmp53E->f1=Cyc_Absyn_Loc_n,((_tmp53E->f2=_tmp36A,_tmp53E)))))),((_tmp53F.tq=_tmp36B,((_tmp53F.type=_tmp36C,((_tmp53F.initializer=0,((_tmp53F.rgn=param_rgn,((_tmp53F.attributes=0,((_tmp53F.escapes=0,_tmp53F)))))))))))))))),_tmp53D)));
# 1341
{struct Cyc_List_List _tmp542;struct Cyc_List_List*_tmp541;_tmp367=((_tmp541=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp541[0]=(struct Cyc_List_List)((_tmp542.hd=_tmp36D,((_tmp542.tl=_tmp367,_tmp542)))),_tmp541))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp545;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp544;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp370=(_tmp544=_cycalloc(sizeof(*_tmp544)),((_tmp544[0]=((_tmp545.tag=3,((_tmp545.f1=_tmp36D,_tmp545)))),_tmp544)));
struct _dyneither_ptr*_tmp371=_tmp36A;
struct Cyc_Tcenv_Bindings*_tmp546;locals=(const struct Cyc_Tcenv_Bindings*)((_tmp546=_region_malloc(r,sizeof(*_tmp546)),((_tmp546->v=_tmp371,((_tmp546->b=(void*)_tmp370,((_tmp546->tl=locals,_tmp546))))))));};}}{
# 1347
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp55F;struct Cyc_List_List*_tmp55E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp55D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp55C;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp55B;struct Cyc_Tcenv_CtrlEnv*_tmp55A;struct Cyc_Tcenv_SharedFenv*_tmp559;struct Cyc_Tcenv_Fenv*_tmp558;return(_tmp558=_region_malloc(r,sizeof(*_tmp558)),((_tmp558->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp559=_region_malloc(r,sizeof(*_tmp559)),((_tmp559->frgn=r,((_tmp559->return_typ=ret_type,((_tmp559->seen_labels=
# 1351
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp559->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(r,Cyc_strptrcmp),((_tmp559->delayed_effect_checks=0,((_tmp559->delayed_constraint_checks=0,_tmp559)))))))))))))),((_tmp558->type_vars=(struct Cyc_List_List*)_tmp365,((_tmp558->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp366,((_tmp558->locals=(const struct Cyc_Tcenv_Bindings*)locals,((_tmp558->encloser=(struct Cyc_Absyn_Stmt*)
# 1359
Cyc_Absyn_skip_stmt(0),((_tmp558->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
(_tmp55A=_region_malloc(r,sizeof(*_tmp55A)),((_tmp55A->ctrl_rgn=r,((_tmp55A->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp55A->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp55A->fallthru_clause=0,((_tmp55A->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp55A->try_depth=0,_tmp55A)))))))))))))),((_tmp558->capability=(void*)((void*)(
(_tmp55F=_cycalloc(sizeof(*_tmp55F)),((_tmp55F[0]=((_tmp55B.tag=24,((_tmp55B.f1=((_tmp55E=_cycalloc(sizeof(*_tmp55E)),((_tmp55E->hd=(void*)((_tmp55C=_cycalloc(sizeof(*_tmp55C)),((_tmp55C[0]=((_tmp55D.tag=23,((_tmp55D.f1=param_rgn,_tmp55D)))),_tmp55C)))),((_tmp55E->tl=0,_tmp55E)))))),_tmp55B)))),_tmp55F))))),((_tmp558->curr_rgn=(void*)param_rgn,((_tmp558->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1363
((_tmp558->flags).in_notreadctxt=0,(((_tmp558->flags).in_lhs=0,(((_tmp558->flags).in_new=0,(((_tmp558->flags).abstract_ok=0,_tmp558->flags)))))))),((_tmp558->fnrgn=(struct _RegionHandle*)r,_tmp558)))))))))))))))))))));};}
# 1368
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
const struct Cyc_Tcenv_Bindings*_tmp388;struct Cyc_RgnOrder_RgnPO*_tmp389;struct Cyc_List_List*_tmp38A;struct Cyc_Tcenv_SharedFenv*_tmp38B;struct _RegionHandle*_tmp38C;struct Cyc_Tcenv_Fenv*_tmp387=old_fenv;_tmp388=_tmp387->locals;_tmp389=_tmp387->region_order;_tmp38A=_tmp387->type_vars;_tmp38B=_tmp387->shared;_tmp38C=_tmp387->fnrgn;{
# 1371
struct _RegionHandle*_tmp38D=_tmp38B->frgn;
const struct Cyc_Tcenv_Bindings*_tmp38E=_tmp388;
struct _dyneither_ptr*_tmp56C;const char*_tmp56B;void*_tmp56A[1];struct Cyc_String_pa_PrintArg_struct _tmp569;struct Cyc_Absyn_Tvar*_tmp568;struct Cyc_Absyn_Tvar*rgn0=
(_tmp568=_cycalloc(sizeof(*_tmp568)),((_tmp568->name=((_tmp56C=_cycalloc(sizeof(*_tmp56C)),((_tmp56C[0]=(struct _dyneither_ptr)((_tmp569.tag=0,((_tmp569.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2),((_tmp56A[0]=& _tmp569,Cyc_aprintf(((_tmp56B="`%s",_tag_dyneither(_tmp56B,sizeof(char),4))),_tag_dyneither(_tmp56A,sizeof(void*),1)))))))),_tmp56C)))),((_tmp568->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp568->kind=(void*)& Cyc_Tcenv_rgn_kb,_tmp568)))))));
# 1377
{struct Cyc_List_List*_tmp38F=fd->tvs;for(0;_tmp38F != 0;_tmp38F=_tmp38F->tl){
struct Cyc_Absyn_Kind*_tmp390=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp38F->hd,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmp392;enum Cyc_Absyn_AliasQual _tmp393;struct Cyc_Absyn_Kind*_tmp391=_tmp390;_tmp392=_tmp391->kind;_tmp393=_tmp391->aliasqual;
if(_tmp392 == Cyc_Absyn_RgnKind){
if(_tmp393 == Cyc_Absyn_Aliasable)
_tmp389=Cyc_RgnOrder_add_youngest(_tmp38D,_tmp389,(struct Cyc_Absyn_Tvar*)_tmp38F->hd,0,0);else{
# 1383
const char*_tmp56F;void*_tmp56E;(_tmp56E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp56F="non-intuitionistic tvar in nested_fenv",_tag_dyneither(_tmp56F,sizeof(char),39))),_tag_dyneither(_tmp56E,sizeof(void*),0)));}}}}
# 1385
_tmp389=Cyc_RgnOrder_add_youngest(_tmp38D,_tmp389,rgn0,0,0);{
struct Cyc_List_List*_tmp570;struct Cyc_List_List*_tmp396=(_tmp570=_cycalloc(sizeof(*_tmp570)),((_tmp570->hd=rgn0,((_tmp570->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmp38A),_tmp570)))));
Cyc_Tcutil_check_unique_tvars(loc,_tmp396);{
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp573;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp572;void*param_rgn=(void*)((_tmp572=_cycalloc(sizeof(*_tmp572)),((_tmp572[0]=((_tmp573.tag=2,((_tmp573.f1=rgn0,_tmp573)))),_tmp572))));
struct Cyc_List_List*_tmp397=0;
{struct Cyc_List_List*_tmp398=fd->args;for(0;_tmp398 != 0;_tmp398=_tmp398->tl){
struct Cyc_Absyn_Vardecl _tmp579;struct _tuple0*_tmp578;struct Cyc_Absyn_Vardecl*_tmp577;struct Cyc_Absyn_Vardecl*_tmp399=(_tmp577=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp577[0]=(struct Cyc_Absyn_Vardecl)((_tmp579.sc=Cyc_Absyn_Public,((_tmp579.name=(
(_tmp578=_cycalloc(sizeof(*_tmp578)),((_tmp578->f1=Cyc_Absyn_Loc_n,((_tmp578->f2=(*((struct _tuple8*)_tmp398->hd)).f1,_tmp578)))))),((_tmp579.tq=(*((struct _tuple8*)_tmp398->hd)).f2,((_tmp579.type=(*((struct _tuple8*)_tmp398->hd)).f3,((_tmp579.initializer=0,((_tmp579.rgn=param_rgn,((_tmp579.attributes=0,((_tmp579.escapes=0,_tmp579)))))))))))))))),_tmp577)));
# 1398
{struct Cyc_List_List _tmp57C;struct Cyc_List_List*_tmp57B;_tmp397=((_tmp57B=_cycalloc(sizeof(struct Cyc_List_List)* 1),((_tmp57B[0]=(struct Cyc_List_List)((_tmp57C.hd=_tmp399,((_tmp57C.tl=_tmp397,_tmp57C)))),_tmp57B))));}{
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp57F;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp57E;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp39C=(_tmp57E=_cycalloc(sizeof(*_tmp57E)),((_tmp57E[0]=((_tmp57F.tag=3,((_tmp57F.f1=_tmp399,_tmp57F)))),_tmp57E)));
struct _dyneither_ptr*_tmp39D=(*((struct _tuple8*)_tmp398->hd)).f1;
struct Cyc_Tcenv_Bindings*_tmp580;_tmp38E=(const struct Cyc_Tcenv_Bindings*)((_tmp580=_region_malloc(_tmp38D,sizeof(*_tmp580)),((_tmp580->v=_tmp39D,((_tmp580->b=(void*)_tmp39C,((_tmp580->tl=_tmp38E,_tmp580))))))));};}}
# 1403
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3A4=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct _dyneither_ptr*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A7;void*_tmp3A8;int _tmp3A9;struct Cyc_Absyn_VarargInfo _tmp3A5=_tmp3A4;_tmp3A6=_tmp3A5.name;_tmp3A7=_tmp3A5.tq;_tmp3A8=_tmp3A5.type;_tmp3A9=_tmp3A5.inject;
if(_tmp3A6 != 0){
void*_tmp3AA=Cyc_Absyn_dyneither_typ(_tmp3A8,param_rgn,_tmp3A7,Cyc_Absyn_false_conref);
struct Cyc_Absyn_Vardecl _tmp586;struct _tuple0*_tmp585;struct Cyc_Absyn_Vardecl*_tmp584;struct Cyc_Absyn_Vardecl*_tmp3AB=(_tmp584=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)* 1),((_tmp584[0]=(struct Cyc_Absyn_Vardecl)((_tmp586.sc=Cyc_Absyn_Public,((_tmp586.name=(
(_tmp585=_cycalloc(sizeof(*_tmp585)),((_tmp585->f1=Cyc_Absyn_Loc_n,((_tmp585->f2=_tmp3A6,_tmp585)))))),((_tmp586.tq=
Cyc_Absyn_empty_tqual(0),((_tmp586.type=_tmp3AA,((_tmp586.initializer=0,((_tmp586.rgn=param_rgn,((_tmp586.attributes=0,((_tmp586.escapes=0,_tmp586)))))))))))))))),_tmp584)));
# 1414
{struct Cyc_List_List*_tmp587;_tmp397=((_tmp587=_cycalloc(sizeof(*_tmp587)),((_tmp587->hd=_tmp3AB,((_tmp587->tl=_tmp397,_tmp587))))));}{
struct _dyneither_ptr*_tmp3AD=_tmp3A6;
struct Cyc_Absyn_Param_b_Absyn_Binding_struct _tmp58A;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp589;struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp3AE=(_tmp589=_cycalloc(sizeof(*_tmp589)),((_tmp589[0]=((_tmp58A.tag=3,((_tmp58A.f1=_tmp3AB,_tmp58A)))),_tmp589)));
struct Cyc_Tcenv_Bindings*_tmp58B;_tmp38E=(const struct Cyc_Tcenv_Bindings*)((_tmp58B=_region_malloc(_tmp38D,sizeof(*_tmp58B)),((_tmp58B->v=_tmp3AD,((_tmp58B->b=(void*)_tmp3AE,((_tmp58B->tl=_tmp38E,_tmp58B))))))));};}else{
# 1419
const char*_tmp58E;void*_tmp58D;(_tmp58D=0,Cyc_Tcutil_terr(loc,((_tmp58E="missing name for varargs",_tag_dyneither(_tmp58E,sizeof(char),25))),_tag_dyneither(_tmp58D,sizeof(void*),0)));}}
# 1421
{struct Cyc_Core_Opt _tmp591;struct Cyc_Core_Opt*_tmp590;fd->param_vardecls=((_tmp590=_cycalloc(sizeof(struct Cyc_Core_Opt)* 1),((_tmp590[0]=(struct Cyc_Core_Opt)((_tmp591.v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp397),_tmp591)),_tmp590))));}{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5AF;struct Cyc_List_List*_tmp5AE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp5AD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5AC;struct Cyc_List_List*_tmp5AB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp5AA;struct Cyc_Tcenv_CtrlEnv*_tmp5A9;struct Cyc_Tcenv_SharedFenv*_tmp5A8;struct Cyc_Tcenv_Fenv*_tmp5A7;return(_tmp5A7=_region_malloc(_tmp38D,sizeof(*_tmp5A7)),((_tmp5A7->shared=(struct Cyc_Tcenv_SharedFenv*)(
(_tmp5A8=_region_malloc(_tmp38D,sizeof(*_tmp5A8)),((_tmp5A8->frgn=_tmp38D,((_tmp5A8->return_typ=fd->ret_type,((_tmp5A8->seen_labels=
# 1426
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp38D,Cyc_strptrcmp),((_tmp5A8->needed_labels=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(_tmp38D,Cyc_strptrcmp),((_tmp5A8->delayed_effect_checks=0,((_tmp5A8->delayed_constraint_checks=0,_tmp5A8)))))))))))))),((_tmp5A7->type_vars=(struct Cyc_List_List*)_tmp396,((_tmp5A7->region_order=(struct Cyc_RgnOrder_RgnPO*)_tmp389,((_tmp5A7->locals=(const struct Cyc_Tcenv_Bindings*)_tmp38E,((_tmp5A7->encloser=(struct Cyc_Absyn_Stmt*)fd->body,((_tmp5A7->ctrl_env=(struct Cyc_Tcenv_CtrlEnv*)(
# 1435
(_tmp5A9=_region_malloc(_tmp38D,sizeof(*_tmp5A9)),((_tmp5A9->ctrl_rgn=_tmp38D,((_tmp5A9->continue_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp5A9->break_stmt=(void*)& Cyc_Tcenv_NotLoop_j_val,((_tmp5A9->fallthru_clause=0,((_tmp5A9->next_stmt=(void*)& Cyc_Tcenv_FnEnd_j_val,((_tmp5A9->try_depth=0,_tmp5A9)))))))))))))),((_tmp5A7->capability=(void*)((void*)(
(_tmp5AF=_cycalloc(sizeof(*_tmp5AF)),((_tmp5AF[0]=((_tmp5AA.tag=24,((_tmp5AA.f1=((_tmp5AE=_cycalloc(sizeof(*_tmp5AE)),((_tmp5AE->hd=(void*)((_tmp5AC=_cycalloc(sizeof(*_tmp5AC)),((_tmp5AC[0]=((_tmp5AD.tag=23,((_tmp5AD.f1=param_rgn,_tmp5AD)))),_tmp5AC)))),((_tmp5AE->tl=(
(_tmp5AB=_cycalloc(sizeof(*_tmp5AB)),((_tmp5AB->hd=(void*)_check_null(fd->effect),((_tmp5AB->tl=0,_tmp5AB)))))),_tmp5AE)))))),_tmp5AA)))),_tmp5AF))))),((_tmp5A7->curr_rgn=(void*)param_rgn,((_tmp5A7->flags=(struct Cyc_Tcenv_FenvFlags)(
# 1439
((_tmp5A7->flags).in_notreadctxt=0,(((_tmp5A7->flags).in_lhs=0,(((_tmp5A7->flags).in_new=0,(((_tmp5A7->flags).abstract_ok=0,_tmp5A7->flags)))))))),((_tmp5A7->fnrgn=(struct _RegionHandle*)_tmp38C,_tmp5A7)))))))))))))))))))));};};};};}
