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
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 914 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 923
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 926
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 939
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 941
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 944
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 946
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 948
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 958
void*Cyc_Absyn_exn_typ();
# 969
extern void*Cyc_Absyn_bounds_one;
# 974
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 977
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 980
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 985
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 996
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1068
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1142
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1144
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);
# 1161
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 104
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 125 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 139
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 173
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 179
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 214
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 221
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 80
int Cyc_Tcutil_coerceable(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 106
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 118
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 137
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 238 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 298
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 34 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 37
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 52 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 56
{void*_tmp6C4;(_tmp6C4=0,Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp6C4,sizeof(void*),0)));}
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 60
return*topt;}}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 71 "tcexp.cyc"
static struct _tuple17*Cyc_Tcexp_make_struct_arg(struct Cyc_Absyn_Exp*e){
struct _tuple17*_tmp6C5;return(_tmp6C5=_cycalloc(sizeof(*_tmp6C5)),((_tmp6C5->f1=0,((_tmp6C5->f2=e,_tmp6C5)))));}
# 76
static int Cyc_Tcexp_resolve_unknown_fn(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*es){
void*_tmp2=e1->r;void*_tmp3=_tmp2;struct _tuple1*_tmp5;void**_tmp6;_LL1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3;if(_tmp4->tag != 1)goto _LL3;else{_tmp5=_tmp4->f1;_tmp6=(void**)((void**)& _tmp4->f2);}}_LL2: {
# 79
struct _handler_cons _tmp7;_push_handler(& _tmp7);{int _tmp9=0;if(setjmp(_tmp7.handler))_tmp9=1;if(!_tmp9){
{struct _RegionHandle _tmpA=_new_region("r");struct _RegionHandle*r=& _tmpA;_push_region(r);
{void*_tmpB=Cyc_Tcenv_lookup_ordinary(r,te,e1->loc,_tmp5,1);void*_tmpC=_tmpB;void*_tmp10;struct Cyc_Absyn_Datatypedecl*_tmp12;struct Cyc_Absyn_Datatypefield*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp15;_LL6: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpD=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpD->tag != 0)goto _LL8;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpD->f1);if(_tmpE->tag != 0)goto _LL8;}}_LL7:
# 83
{const char*_tmp6C9;void*_tmp6C8[1];struct Cyc_String_pa_PrintArg_struct _tmp6C7;(_tmp6C7.tag=0,((_tmp6C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6C8[0]=& _tmp6C7,Cyc_Tcutil_terr(e1->loc,((_tmp6C9="undeclared identifier %s",_tag_dyneither(_tmp6C9,sizeof(char),25))),_tag_dyneither(_tmp6C8,sizeof(void*),1)))))));}{
int _tmp1B=0;_npop_handler(1);return _tmp1B;};_LL8: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpC;if(_tmpF->tag != 0)goto _LLA;else{_tmp10=(void*)_tmpF->f1;}}_LL9:
# 86
*_tmp6=_tmp10;{
int _tmp1C=1;_npop_handler(1);return _tmp1C;};_LLA: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp11=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp11->tag != 2)goto _LLC;else{_tmp12=_tmp11->f1;_tmp13=_tmp11->f2;}}_LLB:
# 89
 if(_tmp13->typs == 0){
# 91
{const char*_tmp6CD;void*_tmp6CC[1];struct Cyc_String_pa_PrintArg_struct _tmp6CB;(_tmp6CB.tag=0,((_tmp6CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp13->name)),((_tmp6CC[0]=& _tmp6CB,Cyc_Tcutil_terr(e->loc,((_tmp6CD="%s is a constant, not a function",_tag_dyneither(_tmp6CD,sizeof(char),33))),_tag_dyneither(_tmp6CC,sizeof(void*),1)))))));}{
int _tmp20=0;_npop_handler(1);return _tmp20;};}
# 95
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp6D0;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp6CF;e->r=(void*)((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF[0]=((_tmp6D0.tag=30,((_tmp6D0.f1=es,((_tmp6D0.f2=_tmp12,((_tmp6D0.f3=_tmp13,_tmp6D0)))))))),_tmp6CF))));}{
int _tmp23=1;_npop_handler(1);return _tmp23;};_LLC: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp14=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp14->tag != 1)goto _LLE;else{_tmp15=_tmp14->f1;}}_LLD: {
# 98
struct Cyc_List_List*_tmp24=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcexp_make_struct_arg,es);
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6D3;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6D2;e->r=(void*)((_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2[0]=((_tmp6D3.tag=28,((_tmp6D3.f1=_tmp15->name,((_tmp6D3.f2=0,((_tmp6D3.f3=_tmp24,((_tmp6D3.f4=_tmp15,_tmp6D3)))))))))),_tmp6D2))));}{
int _tmp27=1;_npop_handler(1);return _tmp27;};}_LLE: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp16=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp16->tag != 4)goto _LL10;}_LLF:
 goto _LL11;_LL10: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp17=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpC;if(_tmp17->tag != 3)goto _LL5;}_LL11:
# 103
{const char*_tmp6D7;void*_tmp6D6[1];struct Cyc_String_pa_PrintArg_struct _tmp6D5;(_tmp6D5.tag=0,((_tmp6D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6D6[0]=& _tmp6D5,Cyc_Tcutil_terr(e->loc,((_tmp6D7="%s is an enum constructor, not a function",_tag_dyneither(_tmp6D7,sizeof(char),42))),_tag_dyneither(_tmp6D6,sizeof(void*),1)))))));}{
int _tmp2B=0;_npop_handler(1);return _tmp2B;};_LL5:;}
# 81
;_pop_region(r);}
# 80
;_pop_handler();}else{void*_tmp8=(void*)_exn_thrown;void*_tmp2D=_tmp8;void*_tmp2F;_LL13: {struct Cyc_Dict_Absent_exn_struct*_tmp2E=(struct Cyc_Dict_Absent_exn_struct*)_tmp2D;if(_tmp2E->tag != Cyc_Dict_Absent)goto _LL15;}_LL14:
# 108
{const char*_tmp6DB;void*_tmp6DA[1];struct Cyc_String_pa_PrintArg_struct _tmp6D9;(_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp6DA[0]=& _tmp6D9,Cyc_Tcutil_terr(e1->loc,((_tmp6DB="undeclared identifier %s",_tag_dyneither(_tmp6DB,sizeof(char),25))),_tag_dyneither(_tmp6DA,sizeof(void*),1)))))));}
return 0;_LL15: _tmp2F=_tmp2D;_LL16:(void)_throw(_tmp2F);_LL12:;}};}_LL3:;_LL4:
# 112
 return 1;_LL0:;}
# 116
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 120
if(topt == 0){
# 122
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6DE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6DD;e->r=(void*)((_tmp6DD=_cycalloc(sizeof(*_tmp6DD)),((_tmp6DD[0]=((_tmp6DE.tag=25,((_tmp6DE.f1=des,_tmp6DE)))),_tmp6DD))));}
return;}{
# 125
void*t=*topt;
void*_tmp35=Cyc_Tcutil_compress(t);void*_tmp36=_tmp35;union Cyc_Absyn_AggrInfoU _tmp38;void*_tmp3A;struct Cyc_Absyn_Tqual _tmp3B;_LL18: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36;if(_tmp37->tag != 11)goto _LL1A;else{_tmp38=(_tmp37->f1).aggr_info;}}_LL19:
# 128
{union Cyc_Absyn_AggrInfoU _tmp3D=_tmp38;struct Cyc_Absyn_Aggrdecl*_tmp3E;_LL21: if((_tmp3D.UnknownAggr).tag != 1)goto _LL23;_LL22: {
const char*_tmp6E1;void*_tmp6E0;(_tmp6E0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp6E1="struct type not properly set",_tag_dyneither(_tmp6E1,sizeof(char),29))),_tag_dyneither(_tmp6E0,sizeof(void*),0)));}_LL23: if((_tmp3D.KnownAggr).tag != 2)goto _LL20;_tmp3E=*((struct Cyc_Absyn_Aggrdecl**)(_tmp3D.KnownAggr).val);_LL24: {
struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp6E4;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6E3;e->r=(void*)((_tmp6E3=_cycalloc(sizeof(*_tmp6E3)),((_tmp6E3[0]=((_tmp6E4.tag=28,((_tmp6E4.f1=_tmp3E->name,((_tmp6E4.f2=0,((_tmp6E4.f3=des,((_tmp6E4.f4=_tmp3E,_tmp6E4)))))))))),_tmp6E3))));}_LL20:;}
# 132
goto _LL17;_LL1A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36;if(_tmp39->tag != 8)goto _LL1C;else{_tmp3A=(_tmp39->f1).elt_type;_tmp3B=(_tmp39->f1).tq;}}_LL1B:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6E7;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6E6;e->r=(void*)((_tmp6E6=_cycalloc(sizeof(*_tmp6E6)),((_tmp6E6[0]=((_tmp6E7.tag=25,((_tmp6E7.f1=des,_tmp6E7)))),_tmp6E6))));}goto _LL17;_LL1C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp36;if(_tmp3C->tag != 12)goto _LL1E;}_LL1D:
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp6EA;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp6E9;e->r=(void*)((_tmp6E9=_cycalloc(sizeof(*_tmp6E9)),((_tmp6E9[0]=((_tmp6EA.tag=29,((_tmp6EA.f1=t,((_tmp6EA.f2=des,_tmp6EA)))))),_tmp6E9))));}goto _LL17;_LL1E:;_LL1F:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp6ED;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp6EC;e->r=(void*)((_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC[0]=((_tmp6ED.tag=25,((_tmp6ED.f1=des,_tmp6ED)))),_tmp6EC))));}goto _LL17;_LL17:;};}
# 142
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 146
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 152
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcutil_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp6F2;void*_tmp6F1[2];struct Cyc_String_pa_PrintArg_struct _tmp6F0;struct Cyc_String_pa_PrintArg_struct _tmp6EF;(_tmp6EF.tag=0,((_tmp6EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp6F0.tag=0,((_tmp6F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part),((_tmp6F1[0]=& _tmp6F0,((_tmp6F1[1]=& _tmp6EF,Cyc_Tcutil_terr(e->loc,((_tmp6F2="test of %s has type %s instead of integral or pointer type",_tag_dyneither(_tmp6F2,sizeof(char),59))),_tag_dyneither(_tmp6F1,sizeof(void*),2)))))))))))));}
# 161
{void*_tmp4D=e->r;void*_tmp4E=_tmp4D;enum Cyc_Absyn_Primop _tmp50;struct Cyc_List_List*_tmp51;_LL26: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4E;if(_tmp4F->tag != 2)goto _LL28;else{_tmp50=_tmp4F->f1;_tmp51=_tmp4F->f2;}}_LL27:
# 163
 if(_tmp50 == Cyc_Absyn_Eq  || _tmp50 == Cyc_Absyn_Neq){
struct _tuple0 _tmp6F3;struct _tuple0 _tmp52=(_tmp6F3.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51))->hd)->topt),((_tmp6F3.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp51))->tl))->hd)->topt),_tmp6F3)));struct _tuple0 _tmp53=_tmp52;void*_tmp55;void*_tmp57;_LL2B:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp54=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f1;if(_tmp54->tag != 15)goto _LL2D;else{_tmp55=(void*)_tmp54->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp56=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp53.f2;if(_tmp56->tag != 15)goto _LL2D;else{_tmp57=(void*)_tmp56->f1;}};_LL2C: {
# 166
struct _tuple0*_tmp6F6;struct Cyc_Tcexp_TestEnv _tmp6F5;return(_tmp6F5.eq=((_tmp6F6=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp6F6)),((_tmp6F6->f1=_tmp55,((_tmp6F6->f2=_tmp57,_tmp6F6)))))),((_tmp6F5.isTrue=_tmp50 == Cyc_Absyn_Eq,_tmp6F5)));}_LL2D:;_LL2E:
# 168
 goto _LL2A;_LL2A:;}
# 171
goto _LL25;_LL28:;_LL29:
 goto _LL25;_LL25:;}{
# 174
struct Cyc_Tcexp_TestEnv _tmp6F7;return(_tmp6F7.eq=0,((_tmp6F7.isTrue=0,_tmp6F7)));};}
# 195 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 200
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp5C=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp5D=_tmp5C;enum Cyc_Absyn_Sign _tmp5E;enum Cyc_Absyn_Sign _tmp5F;int _tmp60;enum Cyc_Absyn_Sign _tmp61;int _tmp62;struct _dyneither_ptr _tmp63;struct _dyneither_ptr _tmp64;_LL30: if((_tmp5D.Char_c).tag != 2)goto _LL32;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Signed)goto _LL32;_LL31:
 t=Cyc_Absyn_schar_typ;goto _LL2F;_LL32: if((_tmp5D.Char_c).tag != 2)goto _LL34;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL34;_LL33:
 t=Cyc_Absyn_uchar_typ;goto _LL2F;_LL34: if((_tmp5D.Char_c).tag != 2)goto _LL36;if(((struct _tuple4)(_tmp5D.Char_c).val).f1 != Cyc_Absyn_None)goto _LL36;_LL35:
 t=Cyc_Absyn_char_typ;goto _LL2F;_LL36: if((_tmp5D.Wchar_c).tag != 3)goto _LL38;_LL37:
 t=Cyc_Absyn_wchar_typ();goto _LL2F;_LL38: if((_tmp5D.Short_c).tag != 4)goto _LL3A;_tmp5E=((struct _tuple5)(_tmp5D.Short_c).val).f1;_LL39:
# 210
 t=_tmp5E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL2F;_LL3A: if((_tmp5D.LongLong_c).tag != 6)goto _LL3C;_tmp5F=((struct _tuple7)(_tmp5D.LongLong_c).val).f1;_LL3B:
# 212
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL2F;_LL3C: if((_tmp5D.Float_c).tag != 7)goto _LL3E;_tmp60=((struct _tuple8)(_tmp5D.Float_c).val).f2;_LL3D:
 t=Cyc_Absyn_float_typ(_tmp60);goto _LL2F;_LL3E: if((_tmp5D.Int_c).tag != 5)goto _LL40;_tmp61=((struct _tuple6)(_tmp5D.Int_c).val).f1;_tmp62=((struct _tuple6)(_tmp5D.Int_c).val).f2;_LL3F:
# 215
 if(topt == 0)
t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 222
void*_tmp65=Cyc_Tcutil_compress(*topt);void*_tmp66=_tmp65;enum Cyc_Absyn_Sign _tmp68;enum Cyc_Absyn_Sign _tmp6A;enum Cyc_Absyn_Sign _tmp6C;enum Cyc_Absyn_Sign _tmp6E;void*_tmp70;struct Cyc_Absyn_Tqual _tmp71;void*_tmp72;union Cyc_Absyn_Constraint*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;void*_tmp77;_LL47: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp67->tag != 6)goto _LL49;else{_tmp68=_tmp67->f1;if(_tmp67->f2 != Cyc_Absyn_Char_sz)goto _LL49;}}_LL48:
# 224
 switch(_tmp68){case Cyc_Absyn_Unsigned: _LL55:
 t=Cyc_Absyn_uchar_typ;break;case Cyc_Absyn_Signed: _LL56:
 t=Cyc_Absyn_schar_typ;break;case Cyc_Absyn_None: _LL57:
 t=Cyc_Absyn_char_typ;break;}
# 229
*c=Cyc_Absyn_Char_c(_tmp68,(char)_tmp62);
goto _LL46;_LL49: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp69=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp69->tag != 6)goto _LL4B;else{_tmp6A=_tmp69->f1;if(_tmp69->f2 != Cyc_Absyn_Short_sz)goto _LL4B;}}_LL4A:
# 232
 t=_tmp6A == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp6A,(short)_tmp62);
goto _LL46;_LL4B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6B->tag != 6)goto _LL4D;else{_tmp6C=_tmp6B->f1;if(_tmp6B->f2 != Cyc_Absyn_Int_sz)goto _LL4D;}}_LL4C:
# 236
 t=_tmp6C == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp66;if(_tmp6D->tag != 6)goto _LL4F;else{_tmp6E=_tmp6D->f1;if(_tmp6D->f2 != Cyc_Absyn_Long_sz)goto _LL4F;}}_LL4E:
# 239
 t=_tmp6E == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL4F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66;if(_tmp6F->tag != 5)goto _LL51;else{_tmp70=(_tmp6F->f1).elt_typ;_tmp71=(_tmp6F->f1).elt_tq;_tmp72=((_tmp6F->f1).ptr_atts).rgn;_tmp73=((_tmp6F->f1).ptr_atts).nullable;_tmp74=((_tmp6F->f1).ptr_atts).bounds;_tmp75=((_tmp6F->f1).ptr_atts).zero_term;}}if(!(_tmp62 == 0))goto _LL51;_LL50: {
# 243
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp73))return*topt;{
struct Cyc_List_List*_tmp78=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6FD;struct Cyc_Absyn_PtrInfo _tmp6FC;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6FB;t=(void*)((_tmp6FB=_cycalloc(sizeof(*_tmp6FB)),((_tmp6FB[0]=((_tmp6FD.tag=5,((_tmp6FD.f1=((_tmp6FC.elt_typ=_tmp70,((_tmp6FC.elt_tq=_tmp71,((_tmp6FC.ptr_atts=(
((_tmp6FC.ptr_atts).rgn=_tmp72,(((_tmp6FC.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp6FC.ptr_atts).bounds=_tmp74,(((_tmp6FC.ptr_atts).zero_term=_tmp75,(((_tmp6FC.ptr_atts).ptrloc=0,_tmp6FC.ptr_atts)))))))))),_tmp6FC)))))),_tmp6FD)))),_tmp6FB))));}
goto _LL46;};}_LL51: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66;if(_tmp76->tag != 19)goto _LL53;else{_tmp77=(void*)_tmp76->f1;}}_LL52: {
# 251
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp700;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6FF;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7D=(_tmp6FF=_cycalloc(sizeof(*_tmp6FF)),((_tmp6FF[0]=((_tmp700.tag=18,((_tmp700.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0),_tmp700)))),_tmp6FF)));
# 258
{struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp703;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp702;t=(void*)((_tmp702=_cycalloc(sizeof(*_tmp702)),((_tmp702[0]=((_tmp703.tag=19,((_tmp703.f1=(void*)_tmp7D,_tmp703)))),_tmp702))));}
goto _LL46;}_LL53:;_LL54:
# 261
 t=_tmp61 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL46;_LL46:;}
# 264
goto _LL2F;_LL40: if((_tmp5D.String_c).tag != 8)goto _LL42;_tmp63=(struct _dyneither_ptr)(_tmp5D.String_c).val;_LL41:
# 266
 string_numelts=(int)_get_dyneither_size(_tmp63,sizeof(char));
_tmp64=_tmp63;goto _LL43;_LL42: if((_tmp5D.Wstring_c).tag != 9)goto _LL44;_tmp64=(struct _dyneither_ptr)(_tmp5D.Wstring_c).val;_LL43:
# 269
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp64);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 273
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 278
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp706;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp705;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp82=(_tmp705=_cycalloc(sizeof(*_tmp705)),((_tmp705[0]=((_tmp706.tag=1,((_tmp706.f1=elen,_tmp706)))),_tmp705)));
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
# 281
if(topt != 0){
void*_tmp83=Cyc_Tcutil_compress(*topt);void*_tmp84=_tmp83;struct Cyc_Absyn_Tqual _tmp86;_LL5A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp84;if(_tmp85->tag != 8)goto _LL5C;else{_tmp86=(_tmp85->f1).tq;}}_LL5B:
# 286
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp86,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);_LL5C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp84;if(_tmp87->tag != 5)goto _LL5E;}_LL5D:
# 288
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 294
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp82,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 302
goto _LL59;_LL5E:;_LL5F:
 goto _LL59;_LL59:;}
# 306
return t;};};_LL44: if((_tmp5D.Null_c).tag != 1)goto _LL2F;_LL45:
# 308
 if(topt != 0){
void*_tmp8A=Cyc_Tcutil_compress(*topt);void*_tmp8B=_tmp8A;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8F;union Cyc_Absyn_Constraint*_tmp90;union Cyc_Absyn_Constraint*_tmp91;union Cyc_Absyn_Constraint*_tmp92;_LL61: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8B;if(_tmp8C->tag != 5)goto _LL63;else{_tmp8D=(_tmp8C->f1).elt_typ;_tmp8E=(_tmp8C->f1).elt_tq;_tmp8F=((_tmp8C->f1).ptr_atts).rgn;_tmp90=((_tmp8C->f1).ptr_atts).nullable;_tmp91=((_tmp8C->f1).ptr_atts).bounds;_tmp92=((_tmp8C->f1).ptr_atts).zero_term;}}_LL62:
# 312
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp90))return*topt;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp70C;struct Cyc_Absyn_PtrInfo _tmp70B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70A;return(void*)((_tmp70A=_cycalloc(sizeof(*_tmp70A)),((_tmp70A[0]=((_tmp70C.tag=5,((_tmp70C.f1=((_tmp70B.elt_typ=_tmp8D,((_tmp70B.elt_tq=_tmp8E,((_tmp70B.ptr_atts=(((_tmp70B.ptr_atts).rgn=_tmp8F,(((_tmp70B.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp70B.ptr_atts).bounds=_tmp91,(((_tmp70B.ptr_atts).zero_term=_tmp92,(((_tmp70B.ptr_atts).ptrloc=0,_tmp70B.ptr_atts)))))))))),_tmp70B)))))),_tmp70C)))),_tmp70A))));};_LL63:;_LL64:
# 315
 goto _LL60;_LL60:;}{
# 318
struct Cyc_List_List*_tmp96=Cyc_Tcenv_lookup_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp71B;struct Cyc_Core_Opt*_tmp71A;struct Cyc_Core_Opt*_tmp719;struct Cyc_Absyn_PtrInfo _tmp718;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp717;t=(void*)((_tmp717=_cycalloc(sizeof(*_tmp717)),((_tmp717[0]=((_tmp71B.tag=5,((_tmp71B.f1=((_tmp718.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,((_tmp719=_cycalloc(sizeof(*_tmp719)),((_tmp719->v=_tmp96,_tmp719))))),((_tmp718.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp718.ptr_atts=(
((_tmp718.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp71A=_cycalloc(sizeof(*_tmp71A)),((_tmp71A->v=_tmp96,_tmp71A))))),(((_tmp718.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp718.ptr_atts).bounds=
# 323
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp718.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp718.ptr_atts).ptrloc=0,_tmp718.ptr_atts)))))))))),_tmp718)))))),_tmp71B)))),_tmp717))));}
# 325
goto _LL2F;};_LL2F:;}
# 327
return t;}
# 331
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 336
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _tuple1*q,void**b){
# 340
void*_tmp9C=*((void**)_check_null(b));void*_tmp9D=_tmp9C;struct Cyc_Absyn_Vardecl*_tmpA0;struct Cyc_Absyn_Fndecl*_tmpA2;struct Cyc_Absyn_Vardecl*_tmpA4;struct Cyc_Absyn_Vardecl*_tmpA6;struct Cyc_Absyn_Vardecl*_tmpA8;_LL66: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmp9E=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9E->tag != 0)goto _LL68;}_LL67: {
# 342
struct _handler_cons _tmpA9;_push_handler(& _tmpA9);{int _tmpAB=0;if(setjmp(_tmpA9.handler))_tmpAB=1;if(!_tmpAB){
{struct _RegionHandle _tmpAC=_new_region("r");struct _RegionHandle*r=& _tmpAC;_push_region(r);
{void*_tmpAD=Cyc_Tcenv_lookup_ordinary(r,te,e->loc,q,1);void*_tmpAE=_tmpAD;void*_tmpB2;struct Cyc_Absyn_Enumdecl*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB5;void*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Datatypedecl*_tmpBA;struct Cyc_Absyn_Datatypefield*_tmpBB;_LL73: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpAF=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpAF->tag != 0)goto _LL75;else{struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpB0=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((void*)_tmpAF->f1);if(_tmpB0->tag != 0)goto _LL75;}}_LL74: {
# 346
const char*_tmp71F;void*_tmp71E[1];struct Cyc_String_pa_PrintArg_struct _tmp71D;void*_tmpC0=(_tmp71D.tag=0,((_tmp71D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp71E[0]=& _tmp71D,Cyc_Tcexp_expr_err(te,loc,0,((_tmp71F="undeclared identifier %s",_tag_dyneither(_tmp71F,sizeof(char),25))),_tag_dyneither(_tmp71E,sizeof(void*),1)))))));_npop_handler(1);return _tmpC0;}_LL75: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB1=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB1->tag != 0)goto _LL77;else{_tmpB2=(void*)_tmpB1->f1;}}_LL76:
# 348
*b=_tmpB2;{
void*_tmpC1=Cyc_Tcexp_tcVar(te,loc,topt,e,q,b);_npop_handler(1);return _tmpC1;};_LL77: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmpB3=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB3->tag != 3)goto _LL79;else{_tmpB4=_tmpB3->f1;_tmpB5=_tmpB3->f2;}}_LL78:
# 351
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmp722;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp721;e->r=(void*)((_tmp721=_cycalloc(sizeof(*_tmp721)),((_tmp721[0]=((_tmp722.tag=31,((_tmp722.f1=_tmpB5->name,((_tmp722.f2=_tmpB4,((_tmp722.f3=_tmpB5,_tmp722)))))))),_tmp721))));}{
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp725;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp724;void*_tmpC6=(void*)((_tmp724=_cycalloc(sizeof(*_tmp724)),((_tmp724[0]=((_tmp725.tag=13,((_tmp725.f1=_tmpB4->name,((_tmp725.f2=_tmpB4,_tmp725)))))),_tmp724))));_npop_handler(1);return _tmpC6;};_LL79: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmpB6=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB6->tag != 4)goto _LL7B;else{_tmpB7=(void*)_tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL7A:
# 354
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp728;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp727;e->r=(void*)((_tmp727=_cycalloc(sizeof(*_tmp727)),((_tmp727[0]=((_tmp728.tag=32,((_tmp728.f1=_tmpB8->name,((_tmp728.f2=_tmpB7,((_tmp728.f3=_tmpB8,_tmp728)))))))),_tmp727))));}{
void*_tmpC9=_tmpB7;_npop_handler(1);return _tmpC9;};_LL7B: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpB9=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpB9->tag != 2)goto _LL7D;else{_tmpBA=_tmpB9->f1;_tmpBB=_tmpB9->f2;}}_LL7C:
# 357
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp72B;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp72A;e->r=(void*)((_tmp72A=_cycalloc(sizeof(*_tmp72A)),((_tmp72A[0]=((_tmp72B.tag=30,((_tmp72B.f1=0,((_tmp72B.f2=_tmpBA,((_tmp72B.f3=_tmpBB,_tmp72B)))))))),_tmp72A))));}{
void*_tmpCC=Cyc_Tcexp_tcDatatype(te,loc,topt,e,0,_tmpBA,_tmpBB);_npop_handler(1);return _tmpCC;};_LL7D: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmpBC=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmpAE;if(_tmpBC->tag != 1)goto _LL72;}_LL7E: {
# 360
const char*_tmp72F;void*_tmp72E[1];struct Cyc_String_pa_PrintArg_struct _tmp72D;void*_tmpD0=(_tmp72D.tag=0,((_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp72E[0]=& _tmp72D,Cyc_Tcexp_expr_err(te,loc,0,((_tmp72F="bad occurrence of type name %s",_tag_dyneither(_tmp72F,sizeof(char),31))),_tag_dyneither(_tmp72E,sizeof(void*),1)))))));_npop_handler(1);return _tmpD0;}_LL72:;}
# 344
;_pop_region(r);}
# 343
;_pop_handler();}else{void*_tmpAA=(void*)_exn_thrown;void*_tmpD2=_tmpAA;void*_tmpD4;_LL80: {struct Cyc_Dict_Absent_exn_struct*_tmpD3=(struct Cyc_Dict_Absent_exn_struct*)_tmpD2;if(_tmpD3->tag != Cyc_Dict_Absent)goto _LL82;}_LL81: {
# 365
const char*_tmp733;void*_tmp732[1];struct Cyc_String_pa_PrintArg_struct _tmp731;return(_tmp731.tag=0,((_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(q)),((_tmp732[0]=& _tmp731,Cyc_Tcexp_expr_err(te,loc,0,((_tmp733="undeclared identifier %s",_tag_dyneither(_tmp733,sizeof(char),25))),_tag_dyneither(_tmp732,sizeof(void*),1)))))));}_LL82: _tmpD4=_tmpD2;_LL83:(void)_throw(_tmpD4);_LL7F:;}};}_LL68: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp9F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp9D;if(_tmp9F->tag != 1)goto _LL6A;else{_tmpA0=_tmp9F->f1;}}_LL69:
# 368
*q=*_tmpA0->name;
return _tmpA0->type;_LL6A: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA1=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA1->tag != 2)goto _LL6C;else{_tmpA2=_tmpA1->f1;}}_LL6B:
*q=*_tmpA2->name;return Cyc_Tcutil_fndecl2typ(_tmpA2);_LL6C: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpA3=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA3->tag != 5)goto _LL6E;else{_tmpA4=_tmpA3->f1;}}_LL6D:
 _tmpA6=_tmpA4;goto _LL6F;_LL6E: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA5=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA5->tag != 4)goto _LL70;else{_tmpA6=_tmpA5->f1;}}_LL6F:
 _tmpA8=_tmpA6;goto _LL71;_LL70: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpA7=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp9D;if(_tmpA7->tag != 3)goto _LL65;else{_tmpA8=_tmpA7->f1;}}_LL71:
# 374
{union Cyc_Absyn_Nmspace _tmp734;(*q).f1=(((_tmp734.Loc_n).val=0,(((_tmp734.Loc_n).tag=4,_tmp734))));}
if(te->allow_valueof){
void*_tmpD9=Cyc_Tcutil_compress(_tmpA8->type);void*_tmpDA=_tmpD9;void*_tmpDC;_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpDB=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpDA;if(_tmpDB->tag != 19)goto _LL87;else{_tmpDC=(void*)_tmpDB->f1;}}_LL86:
# 378
{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp737;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp736;e->r=(void*)((_tmp736=_cycalloc(sizeof(*_tmp736)),((_tmp736[0]=((_tmp737.tag=38,((_tmp737.f1=_tmpDC,_tmp737)))),_tmp736))));}
goto _LL84;_LL87:;_LL88:
 goto _LL84;_LL84:;}
# 383
return _tmpA8->type;_LL65:;}
# 387
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 396
struct Cyc_List_List*desc_types;
{void*_tmpDF=fmt->r;void*_tmpE0=_tmpDF;struct _dyneither_ptr _tmpE2;struct _dyneither_ptr _tmpE5;_LL8A: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE1->tag != 0)goto _LL8C;else{if(((_tmpE1->f1).String_c).tag != 8)goto _LL8C;_tmpE2=(struct _dyneither_ptr)((_tmpE1->f1).String_c).val;}}_LL8B:
 _tmpE5=_tmpE2;goto _LL8D;_LL8C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE0;if(_tmpE3->tag != 13)goto _LL8E;else{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpE4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(_tmpE3->f2)->r;if(_tmpE4->tag != 0)goto _LL8E;else{if(((_tmpE4->f1).String_c).tag != 8)goto _LL8E;_tmpE5=(struct _dyneither_ptr)((_tmpE4->f1).String_c).val;}}}_LL8D:
# 400
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmpE5,fmt->loc);goto _LL89;_LL8E:;_LL8F:
# 404
 if(opt_args != 0){
struct Cyc_List_List*_tmpE6=(struct Cyc_List_List*)opt_args->v;
for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmpE6->hd);{
struct _RegionHandle _tmpE7=_new_region("temp");struct _RegionHandle*temp=& _tmpE7;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmpE6->hd)){
const char*_tmp73A;void*_tmp739;(_tmp739=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpE6->hd)->loc,((_tmp73A="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp73A,sizeof(char),49))),_tag_dyneither(_tmp739,sizeof(void*),0)));}
# 409
;_pop_region(temp);};}}
# 415
return;_LL89:;}
# 417
if(opt_args != 0){
struct Cyc_List_List*_tmpEA=(struct Cyc_List_List*)opt_args->v;
# 420
for(0;desc_types != 0  && _tmpEA != 0;
(((desc_types=desc_types->tl,_tmpEA=_tmpEA->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmpEA->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
{const char*_tmp73F;void*_tmp73E[2];struct Cyc_String_pa_PrintArg_struct _tmp73D;struct Cyc_String_pa_PrintArg_struct _tmp73C;(_tmp73C.tag=0,((_tmp73C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp73D.tag=0,((_tmp73D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp73E[0]=& _tmp73D,((_tmp73E[1]=& _tmp73C,Cyc_Tcutil_terr(e->loc,((_tmp73F="descriptor has type %s but argument has type %s",_tag_dyneither(_tmp73F,sizeof(char),48))),_tag_dyneither(_tmp73E,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 431
if(alias_coercion){
struct Cyc_List_List*_tmp740;*alias_arg_exps=((_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740->hd=(void*)arg_cnt,((_tmp740->tl=*alias_arg_exps,_tmp740))))));}{
struct _RegionHandle _tmpF0=_new_region("temp");struct _RegionHandle*temp=& _tmpF0;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp743;void*_tmp742;(_tmp742=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp743="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp743,sizeof(char),49))),_tag_dyneither(_tmp742,sizeof(void*),0)));}
# 434
;_pop_region(temp);};}
# 439
if(desc_types != 0){
const char*_tmp746;void*_tmp745;(_tmp745=0,Cyc_Tcutil_terr(fmt->loc,((_tmp746="too few arguments",_tag_dyneither(_tmp746,sizeof(char),18))),_tag_dyneither(_tmp745,sizeof(void*),0)));}
if(_tmpEA != 0){
{const char*_tmp749;void*_tmp748;(_tmp748=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmpEA->hd)->loc,((_tmp749="too many arguments",_tag_dyneither(_tmp749,sizeof(char),19))),_tag_dyneither(_tmp748,sizeof(void*),0)));}{
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp74C;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp74B;(int)_throw((void*)((_tmp74B=_cycalloc_atomic(sizeof(*_tmp74B)),((_tmp74B[0]=((_tmp74C.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp74C)),_tmp74B)))));};}}}
# 447
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 449
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
switch(p){case Cyc_Absyn_Plus: _LL90:
 goto _LL91;case Cyc_Absyn_Minus: _LL91:
# 453
 if(!Cyc_Tcutil_is_numeric(e)){
const char*_tmp750;void*_tmp74F[1];struct Cyc_String_pa_PrintArg_struct _tmp74E;(_tmp74E.tag=0,((_tmp74E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp74F[0]=& _tmp74E,Cyc_Tcutil_terr(loc,((_tmp750="expecting numeric type but found %s",_tag_dyneither(_tmp750,sizeof(char),36))),_tag_dyneither(_tmp74F,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL92:
# 457
 Cyc_Tcutil_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e)){
const char*_tmp754;void*_tmp753[1];struct Cyc_String_pa_PrintArg_struct _tmp752;(_tmp752.tag=0,((_tmp752.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp753[0]=& _tmp752,Cyc_Tcutil_terr(loc,((_tmp754="expecting integral or * type but found %s",_tag_dyneither(_tmp754,sizeof(char),42))),_tag_dyneither(_tmp753,sizeof(void*),1)))))));}
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL93:
# 462
 if(!Cyc_Tcutil_is_integral(e)){
const char*_tmp758;void*_tmp757[1];struct Cyc_String_pa_PrintArg_struct _tmp756;(_tmp756.tag=0,((_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp757[0]=& _tmp756,Cyc_Tcutil_terr(loc,((_tmp758="expecting integral type but found %s",_tag_dyneither(_tmp758,sizeof(char),37))),_tag_dyneither(_tmp757,sizeof(void*),1)))))));}
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL94:
# 466
{void*_tmp102=t;union Cyc_Absyn_Constraint*_tmp105;_LL97: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp103=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp102;if(_tmp103->tag != 8)goto _LL99;}_LL98:
 goto _LL96;_LL99: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp104=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp102;if(_tmp104->tag != 5)goto _LL9B;else{_tmp105=((_tmp104->f1).ptr_atts).bounds;}}_LL9A:
# 469
{void*_tmp106=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp105);void*_tmp107=_tmp106;struct Cyc_Absyn_Exp*_tmp10A;_LL9E: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp108=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp107;if(_tmp108->tag != 0)goto _LLA0;}_LL9F:
 goto _LL9D;_LLA0: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp109=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp107;if(_tmp109->tag != 1)goto _LL9D;else{_tmp10A=_tmp109->f1;}}_LLA1:
# 472
 if(!Cyc_Evexp_c_can_eval(_tmp10A)){
const char*_tmp75B;void*_tmp75A;(_tmp75A=0,Cyc_Tcutil_terr(loc,((_tmp75B="cannot apply numelts to a pointer with abstract bounds",_tag_dyneither(_tmp75B,sizeof(char),55))),_tag_dyneither(_tmp75A,sizeof(void*),0)));}
goto _LL9D;_LL9D:;}
# 476
goto _LL96;_LL9B:;_LL9C: {
# 478
const char*_tmp75F;void*_tmp75E[1];struct Cyc_String_pa_PrintArg_struct _tmp75D;(_tmp75D.tag=0,((_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp75E[0]=& _tmp75D,Cyc_Tcutil_terr(loc,((_tmp75F="numelts requires pointer or array type, not %s",_tag_dyneither(_tmp75F,sizeof(char),47))),_tag_dyneither(_tmp75E,sizeof(void*),1)))))));}_LL96:;}
# 480
return Cyc_Absyn_uint_typ;default: _LL95: {
const char*_tmp762;void*_tmp761;(_tmp761=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp762="Non-unary primop",_tag_dyneither(_tmp762,sizeof(char),17))),_tag_dyneither(_tmp761,sizeof(void*),0)));}}}
# 486
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 489
if(!checker(e1)){
{const char*_tmp766;void*_tmp765[1];struct Cyc_String_pa_PrintArg_struct _tmp764;(_tmp764.tag=0,((_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp765[0]=& _tmp764,Cyc_Tcutil_terr(e1->loc,((_tmp766="type %s cannot be used here",_tag_dyneither(_tmp766,sizeof(char),28))),_tag_dyneither(_tmp765,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 493
if(!checker(e2)){
{const char*_tmp76A;void*_tmp769[1];struct Cyc_String_pa_PrintArg_struct _tmp768;(_tmp768.tag=0,((_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp769[0]=& _tmp768,Cyc_Tcutil_terr(e2->loc,((_tmp76A="type %s cannot be used here",_tag_dyneither(_tmp76A,sizeof(char),28))),_tag_dyneither(_tmp769,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 497
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 502
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp118=t1;void*_tmp11A;struct Cyc_Absyn_Tqual _tmp11B;void*_tmp11C;union Cyc_Absyn_Constraint*_tmp11D;union Cyc_Absyn_Constraint*_tmp11E;union Cyc_Absyn_Constraint*_tmp11F;_LLA4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp119=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp118;if(_tmp119->tag != 5)goto _LLA6;else{_tmp11A=(_tmp119->f1).elt_typ;_tmp11B=(_tmp119->f1).elt_tq;_tmp11C=((_tmp119->f1).ptr_atts).rgn;_tmp11D=((_tmp119->f1).ptr_atts).nullable;_tmp11E=((_tmp119->f1).ptr_atts).bounds;_tmp11F=((_tmp119->f1).ptr_atts).zero_term;}}_LLA5:
# 507
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp11A),& Cyc_Tcutil_tmk)){
const char*_tmp76D;void*_tmp76C;(_tmp76C=0,Cyc_Tcutil_terr(e1->loc,((_tmp76D="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp76D,sizeof(char),50))),_tag_dyneither(_tmp76C,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp770;void*_tmp76F;(_tmp76F=0,Cyc_Tcutil_terr(e1->loc,((_tmp770="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp770,sizeof(char),54))),_tag_dyneither(_tmp76F,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
const char*_tmp774;void*_tmp773[1];struct Cyc_String_pa_PrintArg_struct _tmp772;(_tmp772.tag=0,((_tmp772.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp773[0]=& _tmp772,Cyc_Tcutil_terr(e2->loc,((_tmp774="expecting int but found %s",_tag_dyneither(_tmp774,sizeof(char),27))),_tag_dyneither(_tmp773,sizeof(void*),1)))))));}{
void*_tmp127=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp11E);void*_tmp128=_tmp127;struct Cyc_Absyn_Exp*_tmp12B;_LLA9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp129=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp128;if(_tmp129->tag != 0)goto _LLAB;}_LLAA:
 return t1;_LLAB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp128;if(_tmp12A->tag != 1)goto _LLA8;else{_tmp12B=_tmp12A->f1;}}_LLAC:
# 518
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp11F)){
const char*_tmp777;void*_tmp776;(_tmp776=0,Cyc_Tcutil_warn(e1->loc,((_tmp777="pointer arithmetic on thin, zero-terminated pointer may be expensive.",_tag_dyneither(_tmp777,sizeof(char),70))),_tag_dyneither(_tmp776,sizeof(void*),0)));}{
# 526
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp77D;struct Cyc_Absyn_PtrInfo _tmp77C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp77B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12E=(_tmp77B=_cycalloc(sizeof(*_tmp77B)),((_tmp77B[0]=((_tmp77D.tag=5,((_tmp77D.f1=((_tmp77C.elt_typ=_tmp11A,((_tmp77C.elt_tq=_tmp11B,((_tmp77C.ptr_atts=(
((_tmp77C.ptr_atts).rgn=_tmp11C,(((_tmp77C.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp77C.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp77C.ptr_atts).zero_term=_tmp11F,(((_tmp77C.ptr_atts).ptrloc=0,_tmp77C.ptr_atts)))))))))),_tmp77C)))))),_tmp77D)))),_tmp77B)));
# 530
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp12E,Cyc_Absyn_Other_coercion);
return(void*)_tmp12E;};_LLA8:;};_LLA6:;_LLA7:
# 533
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);_LLA3:;}
# 538
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
{const char*_tmp782;void*_tmp781[2];struct Cyc_String_pa_PrintArg_struct _tmp780;struct Cyc_String_pa_PrintArg_struct _tmp77F;(_tmp77F.tag=0,((_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 548
Cyc_Absynpp_typ2string(t2)),((_tmp780.tag=0,((_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp781[0]=& _tmp780,((_tmp781[1]=& _tmp77F,Cyc_Tcutil_terr(e1->loc,((_tmp782="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp782,sizeof(char),59))),_tag_dyneither(_tmp781,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 551
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
{const char*_tmp787;void*_tmp786[2];struct Cyc_String_pa_PrintArg_struct _tmp785;struct Cyc_String_pa_PrintArg_struct _tmp784;(_tmp784.tag=0,((_tmp784.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 556
Cyc_Absynpp_typ2string(t2)),((_tmp785.tag=0,((_tmp785.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp786[0]=& _tmp785,((_tmp786[1]=& _tmp784,Cyc_Tcutil_terr(e1->loc,((_tmp787="pointer arithmetic on values of different types (%s != %s)",_tag_dyneither(_tmp787,sizeof(char),59))),_tag_dyneither(_tmp786,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 560
{const char*_tmp78A;void*_tmp789;(_tmp789=0,Cyc_Tcutil_warn(e1->loc,((_tmp78A="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp78A,sizeof(char),60))),_tag_dyneither(_tmp789,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 564
return Cyc_Absyn_sint_typ;}else{
# 566
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk)){
const char*_tmp78D;void*_tmp78C;(_tmp78C=0,Cyc_Tcutil_terr(e1->loc,((_tmp78D="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp78D,sizeof(char),50))),_tag_dyneither(_tmp78C,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t1,0)){
const char*_tmp790;void*_tmp78F;(_tmp78F=0,Cyc_Tcutil_terr(e1->loc,((_tmp790="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp790,sizeof(char),54))),_tag_dyneither(_tmp78F,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
{const char*_tmp795;void*_tmp794[2];struct Cyc_String_pa_PrintArg_struct _tmp793;struct Cyc_String_pa_PrintArg_struct _tmp792;(_tmp792.tag=0,((_tmp792.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp793.tag=0,((_tmp793.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp794[0]=& _tmp793,((_tmp794[1]=& _tmp792,Cyc_Tcutil_terr(e2->loc,((_tmp795="expecting either %s or int but found %s",_tag_dyneither(_tmp795,sizeof(char),40))),_tag_dyneither(_tmp794,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 575
return t1;}}}
# 579
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
{const char*_tmp798;void*_tmp797;(_tmp797=0,Cyc_Tcutil_warn(e1->loc,((_tmp798="coercing fat pointer to thin pointer to support subtraction",_tag_dyneither(_tmp798,sizeof(char),60))),_tag_dyneither(_tmp797,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 589
{const char*_tmp79B;void*_tmp79A;(_tmp79A=0,Cyc_Tcutil_warn(e1->loc,((_tmp79B="thin pointer subtraction!",_tag_dyneither(_tmp79B,sizeof(char),26))),_tag_dyneither(_tmp79A,sizeof(void*),0)));}
return Cyc_Absyn_sint_typ;}
# 592
{const char*_tmp79E;void*_tmp79D;(_tmp79D=0,Cyc_Tcutil_warn(e1->loc,((_tmp79E="coercing thin pointer to integer to support subtraction",_tag_dyneither(_tmp79E,sizeof(char),56))),_tag_dyneither(_tmp79D,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 595
if(Cyc_Tcutil_is_pointer_type(t2)){
{const char*_tmp7A1;void*_tmp7A0;(_tmp7A0=0,Cyc_Tcutil_warn(e1->loc,((_tmp7A1="coercing pointer to integer to support subtraction",_tag_dyneither(_tmp7A1,sizeof(char),51))),_tag_dyneither(_tmp7A0,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 600
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 603
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 611
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 616
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 629
pointer_cmp: {
struct _tuple0 _tmp7A2;struct _tuple0 _tmp14C=(_tmp7A2.f1=Cyc_Tcutil_compress(t1),((_tmp7A2.f2=Cyc_Tcutil_compress(t2),_tmp7A2)));struct _tuple0 _tmp14D=_tmp14C;void*_tmp14F;void*_tmp151;_LLAE:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp14E->tag != 5)goto _LLB0;else{_tmp14F=(_tmp14E->f1).elt_typ;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp150->tag != 5)goto _LLB0;else{_tmp151=(_tmp150->f1).elt_typ;}};_LLAF:
# 633
 if(Cyc_Tcutil_unify(_tmp14F,_tmp151))return Cyc_Absyn_sint_typ;goto _LLAD;_LLB0:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp152=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f1;if(_tmp152->tag != 15)goto _LLB2;}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp153=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp14D.f2;if(_tmp153->tag != 15)goto _LLB2;};_LLB1:
# 635
 return Cyc_Absyn_sint_typ;_LLB2:;_LLB3:
 goto _LLAD;_LLAD:;}
# 638
{const char*_tmp7A7;void*_tmp7A6[2];struct Cyc_String_pa_PrintArg_struct _tmp7A5;struct Cyc_String_pa_PrintArg_struct _tmp7A4;(_tmp7A4.tag=0,((_tmp7A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7A5.tag=0,((_tmp7A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7A6[0]=& _tmp7A5,((_tmp7A6[1]=& _tmp7A4,Cyc_Tcutil_terr(loc,((_tmp7A7="comparison not allowed between %s and %s",_tag_dyneither(_tmp7A7,sizeof(char),41))),_tag_dyneither(_tmp7A6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 645
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 647
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 653
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmp7A8;struct _tuple0 _tmp159=(_tmp7A8.f1=t1,((_tmp7A8.f2=t2,_tmp7A8)));struct _tuple0 _tmp15A=_tmp159;void*_tmp15C;void*_tmp15E;_LLB5:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f1;if(_tmp15B->tag != 15)goto _LLB7;else{_tmp15C=(void*)_tmp15B->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp15D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp15A.f2;if(_tmp15D->tag != 15)goto _LLB7;else{_tmp15E=(void*)_tmp15D->f1;}};_LLB6:
# 658
 return Cyc_Absyn_sint_typ;_LLB7:;_LLB8:
 goto _LLB4;_LLB4:;}
# 661
{const char*_tmp7AD;void*_tmp7AC[2];struct Cyc_String_pa_PrintArg_struct _tmp7AB;struct Cyc_String_pa_PrintArg_struct _tmp7AA;(_tmp7AA.tag=0,((_tmp7AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7AB.tag=0,((_tmp7AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7AC[0]=& _tmp7AB,((_tmp7AC[1]=& _tmp7AA,Cyc_Tcutil_terr(loc,((_tmp7AD="comparison not allowed between %s and %s",_tag_dyneither(_tmp7AD,sizeof(char),41))),_tag_dyneither(_tmp7AC,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 669
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 671
switch(p){case Cyc_Absyn_Plus: _LLB9:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLBA:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLBB:
# 675
 goto _LLBC;case Cyc_Absyn_Div: _LLBC:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLBD:
# 678
 goto _LLBE;case Cyc_Absyn_Bitand: _LLBE:
 goto _LLBF;case Cyc_Absyn_Bitor: _LLBF:
 goto _LLC0;case Cyc_Absyn_Bitxor: _LLC0:
 goto _LLC1;case Cyc_Absyn_Bitlshift: _LLC1:
 goto _LLC2;case Cyc_Absyn_Bitlrshift: _LLC2:
 goto _LLC3;case Cyc_Absyn_Bitarshift: _LLC3:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLC4:
# 688
 goto _LLC5;case Cyc_Absyn_Neq: _LLC5:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLC6:
# 691
 goto _LLC7;case Cyc_Absyn_Lt: _LLC7:
 goto _LLC8;case Cyc_Absyn_Gte: _LLC8:
 goto _LLC9;case Cyc_Absyn_Lte: _LLC9:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLCA: {
# 696
const char*_tmp7B0;void*_tmp7AF;(_tmp7AF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp7B0="bad binary primop",_tag_dyneither(_tmp7B0,sizeof(char),18))),_tag_dyneither(_tmp7AF,sizeof(void*),0)));}}}
# 700
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 708
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmp167=Cyc_Tcexp_tcExp(te,topt,_tmp166);
if(!Cyc_Tcutil_is_numeric(_tmp166)){
const char*_tmp7B4;void*_tmp7B3[1];struct Cyc_String_pa_PrintArg_struct _tmp7B2;(_tmp7B2.tag=0,((_tmp7B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp167)),((_tmp7B3[0]=& _tmp7B2,Cyc_Tcutil_terr(_tmp166->loc,((_tmp7B4="expecting numeric type but found %s",_tag_dyneither(_tmp7B4,sizeof(char),36))),_tag_dyneither(_tmp7B3,sizeof(void*),1)))))));}
return _tmp167;}
# 715
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmp16B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);switch(_tmp16B){case 0: _LLCC: {
const char*_tmp7B7;void*_tmp7B6;return(_tmp7B6=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7B7="primitive operator has 0 arguments",_tag_dyneither(_tmp7B7,sizeof(char),35))),_tag_dyneither(_tmp7B6,sizeof(void*),0)));}case 1: _LLCD:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);break;case 2: _LLCE:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);break;default: _LLCF: {
const char*_tmp7BA;void*_tmp7B9;return(_tmp7B9=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7BA="primitive operator has > 2 arguments",_tag_dyneither(_tmp7BA,sizeof(char),37))),_tag_dyneither(_tmp7B9,sizeof(void*),0)));}}}
# 723
return t;};}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};
# 726
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmp170=Cyc_Tcutil_compress(t);
void*_tmp171=_tmp170;struct Cyc_Absyn_Aggrdecl*_tmp173;struct Cyc_List_List*_tmp175;struct Cyc_Absyn_Datatypefield*_tmp177;void*_tmp179;struct Cyc_Absyn_Tqual _tmp17A;struct Cyc_List_List*_tmp17C;_LLD2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp171;if(_tmp172->tag != 11)goto _LLD4;else{if((((_tmp172->f1).aggr_info).KnownAggr).tag != 2)goto _LLD4;_tmp173=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp172->f1).aggr_info).KnownAggr).val);}}_LLD3:
# 730
 if(_tmp173->impl == 0){
{const char*_tmp7BD;void*_tmp7BC;(_tmp7BC=0,Cyc_Tcutil_terr(loc,((_tmp7BD="attempt to write an abstract aggregate",_tag_dyneither(_tmp7BD,sizeof(char),39))),_tag_dyneither(_tmp7BC,sizeof(void*),0)));}
return 0;}else{
# 734
_tmp175=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp173->impl))->fields;goto _LLD5;}_LLD4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp174=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp171;if(_tmp174->tag != 12)goto _LLD6;else{_tmp175=_tmp174->f2;}}_LLD5:
# 736
 for(0;_tmp175 != 0;_tmp175=_tmp175->tl){
struct Cyc_Absyn_Aggrfield*_tmp17F=(struct Cyc_Absyn_Aggrfield*)_tmp175->hd;
if((_tmp17F->tq).real_const){
{const char*_tmp7C1;void*_tmp7C0[1];struct Cyc_String_pa_PrintArg_struct _tmp7BF;(_tmp7BF.tag=0,((_tmp7BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp17F->name),((_tmp7C0[0]=& _tmp7BF,Cyc_Tcutil_terr(loc,((_tmp7C1="attempt to over-write an aggregate with const member %s",_tag_dyneither(_tmp7C1,sizeof(char),56))),_tag_dyneither(_tmp7C0,sizeof(void*),1)))))));}
return 0;}
# 742
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp17F->type))return 0;}
# 744
return 1;_LLD6: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp171;if(_tmp176->tag != 4)goto _LLD8;else{if((((_tmp176->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD8;_tmp177=((struct _tuple2)(((_tmp176->f1).field_info).KnownDatatypefield).val).f2;}}_LLD7:
# 746
{struct Cyc_List_List*_tmp183=_tmp177->typs;for(0;_tmp183 != 0;_tmp183=_tmp183->tl){
struct _tuple18*_tmp184=(struct _tuple18*)_tmp183->hd;struct Cyc_Absyn_Tqual _tmp186;void*_tmp187;struct _tuple18*_tmp185=_tmp184;_tmp186=_tmp185->f1;_tmp187=_tmp185->f2;
if(_tmp186.real_const){
{const char*_tmp7C5;void*_tmp7C4[1];struct Cyc_String_pa_PrintArg_struct _tmp7C3;(_tmp7C3.tag=0,((_tmp7C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp177->name)),((_tmp7C4[0]=& _tmp7C3,Cyc_Tcutil_terr(loc,((_tmp7C5="attempt to over-write a datatype field (%s) with a const member",_tag_dyneither(_tmp7C5,sizeof(char),64))),_tag_dyneither(_tmp7C4,sizeof(void*),1)))))));}
return 0;}
# 752
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp187))return 0;}}
# 754
return 1;_LLD8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp171;if(_tmp178->tag != 8)goto _LLDA;else{_tmp179=(_tmp178->f1).elt_type;_tmp17A=(_tmp178->f1).tq;}}_LLD9:
# 756
 if(_tmp17A.real_const){
{const char*_tmp7C8;void*_tmp7C7;(_tmp7C7=0,Cyc_Tcutil_terr(loc,((_tmp7C8="attempt to over-write a const array",_tag_dyneither(_tmp7C8,sizeof(char),36))),_tag_dyneither(_tmp7C7,sizeof(void*),0)));}
return 0;}
# 760
return Cyc_Tcexp_check_writable_aggr(loc,_tmp179);_LLDA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp171;if(_tmp17B->tag != 10)goto _LLDC;else{_tmp17C=_tmp17B->f1;}}_LLDB:
# 762
 for(0;_tmp17C != 0;_tmp17C=_tmp17C->tl){
struct _tuple18*_tmp18D=(struct _tuple18*)_tmp17C->hd;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp190;struct _tuple18*_tmp18E=_tmp18D;_tmp18F=_tmp18E->f1;_tmp190=_tmp18E->f2;
if(_tmp18F.real_const){
{const char*_tmp7CB;void*_tmp7CA;(_tmp7CA=0,Cyc_Tcutil_terr(loc,((_tmp7CB="attempt to over-write a tuple field with a const member",_tag_dyneither(_tmp7CB,sizeof(char),56))),_tag_dyneither(_tmp7CA,sizeof(void*),0)));}
return 0;}
# 768
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp190))return 0;}
# 770
return 1;_LLDC:;_LLDD:
 return 1;_LLD1:;}
# 778
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 781
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp193=e->r;void*_tmp194=_tmp193;struct Cyc_Absyn_Vardecl*_tmp197;struct Cyc_Absyn_Vardecl*_tmp19A;struct Cyc_Absyn_Vardecl*_tmp19D;struct Cyc_Absyn_Vardecl*_tmp1A0;struct Cyc_Absyn_Exp*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_Absyn_Exp*_tmp1A5;struct _dyneither_ptr*_tmp1A6;struct Cyc_Absyn_Exp*_tmp1A8;struct _dyneither_ptr*_tmp1A9;struct Cyc_Absyn_Exp*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AF;_LLDF: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp195->tag != 1)goto _LLE1;else{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp196=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((void*)_tmp195->f2);if(_tmp196->tag != 3)goto _LLE1;else{_tmp197=_tmp196->f1;}}}_LLE0:
 _tmp19A=_tmp197;goto _LLE2;_LLE1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp198->tag != 1)goto _LLE3;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp199=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp198->f2);if(_tmp199->tag != 4)goto _LLE3;else{_tmp19A=_tmp199->f1;}}}_LLE2:
 _tmp19D=_tmp19A;goto _LLE4;_LLE3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19B->tag != 1)goto _LLE5;else{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp19C=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((void*)_tmp19B->f2);if(_tmp19C->tag != 5)goto _LLE5;else{_tmp19D=_tmp19C->f1;}}}_LLE4:
 _tmp1A0=_tmp19D;goto _LLE6;_LLE5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp19E->tag != 1)goto _LLE7;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp19F=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp19E->f2);if(_tmp19F->tag != 1)goto _LLE7;else{_tmp1A0=_tmp19F->f1;}}}_LLE6:
 if(!(_tmp1A0->tq).real_const)return;goto _LLDE;_LLE7: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp1A1=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A1->tag != 22)goto _LLE9;else{_tmp1A2=_tmp1A1->f1;_tmp1A3=_tmp1A1->f2;}}_LLE8:
# 788
{void*_tmp1B0=Cyc_Tcutil_compress((void*)_check_null(_tmp1A2->topt));void*_tmp1B1=_tmp1B0;struct Cyc_Absyn_Tqual _tmp1B3;struct Cyc_Absyn_Tqual _tmp1B5;struct Cyc_List_List*_tmp1B7;_LLF6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B2->tag != 5)goto _LLF8;else{_tmp1B3=(_tmp1B2->f1).elt_tq;}}_LLF7:
 _tmp1B5=_tmp1B3;goto _LLF9;_LLF8: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1B4=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B4->tag != 8)goto _LLFA;else{_tmp1B5=(_tmp1B4->f1).tq;}}_LLF9:
 if(!_tmp1B5.real_const)return;goto _LLF5;_LLFA: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1B6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B6->tag != 10)goto _LLFC;else{_tmp1B7=_tmp1B6->f1;}}_LLFB: {
# 792
struct _tuple15 _tmp1B8=Cyc_Evexp_eval_const_uint_exp(_tmp1A3);unsigned int _tmp1BA;int _tmp1BB;struct _tuple15 _tmp1B9=_tmp1B8;_tmp1BA=_tmp1B9.f1;_tmp1BB=_tmp1B9.f2;
if(!_tmp1BB){
{const char*_tmp7CE;void*_tmp7CD;(_tmp7CD=0,Cyc_Tcutil_terr(e->loc,((_tmp7CE="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp7CE,sizeof(char),47))),_tag_dyneither(_tmp7CD,sizeof(void*),0)));}
return;}
# 797
{struct _handler_cons _tmp1BE;_push_handler(& _tmp1BE);{int _tmp1C0=0;if(setjmp(_tmp1BE.handler))_tmp1C0=1;if(!_tmp1C0){
{struct _tuple18*_tmp1C1=((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp1B7,(int)_tmp1BA);struct Cyc_Absyn_Tqual _tmp1C3;struct _tuple18*_tmp1C2=_tmp1C1;_tmp1C3=_tmp1C2->f1;
if(!_tmp1C3.real_const){_npop_handler(0);return;}}
# 798
;_pop_handler();}else{void*_tmp1BF=(void*)_exn_thrown;void*_tmp1C5=_tmp1BF;void*_tmp1C7;_LLFF: {struct Cyc_List_Nth_exn_struct*_tmp1C6=(struct Cyc_List_Nth_exn_struct*)_tmp1C5;if(_tmp1C6->tag != Cyc_List_Nth)goto _LL101;}_LL100:
# 800
 return;_LL101: _tmp1C7=_tmp1C5;_LL102:(void)_throw(_tmp1C7);_LLFE:;}};}
goto _LLF5;}_LLFC:;_LLFD:
 goto _LLF5;_LLF5:;}
# 804
goto _LLDE;_LLE9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp1A4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A4->tag != 20)goto _LLEB;else{_tmp1A5=_tmp1A4->f1;_tmp1A6=_tmp1A4->f2;}}_LLEA:
# 806
{void*_tmp1C8=Cyc_Tcutil_compress((void*)_check_null(_tmp1A5->topt));void*_tmp1C9=_tmp1C8;struct Cyc_Absyn_Aggrdecl**_tmp1CB;struct Cyc_List_List*_tmp1CD;_LL104: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1CA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CA->tag != 11)goto _LL106;else{if((((_tmp1CA->f1).aggr_info).KnownAggr).tag != 2)goto _LL106;_tmp1CB=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1CA->f1).aggr_info).KnownAggr).val;}}_LL105: {
# 808
struct Cyc_Absyn_Aggrfield*sf=
_tmp1CB == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1CB,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL106: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1CC=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C9;if(_tmp1CC->tag != 12)goto _LL108;else{_tmp1CD=_tmp1CC->f2;}}_LL107: {
# 813
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1CD,_tmp1A6);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL103;}_LL108:;_LL109:
 goto _LL103;_LL103:;}
# 818
goto _LLDE;_LLEB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A7=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1A7->tag != 21)goto _LLED;else{_tmp1A8=_tmp1A7->f1;_tmp1A9=_tmp1A7->f2;}}_LLEC:
# 820
{void*_tmp1CE=Cyc_Tcutil_compress((void*)_check_null(_tmp1A8->topt));void*_tmp1CF=_tmp1CE;void*_tmp1D1;_LL10B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CF;if(_tmp1D0->tag != 5)goto _LL10D;else{_tmp1D1=(_tmp1D0->f1).elt_typ;}}_LL10C:
# 822
{void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D1);void*_tmp1D3=_tmp1D2;struct Cyc_Absyn_Aggrdecl**_tmp1D5;struct Cyc_List_List*_tmp1D7;_LL110: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D4->tag != 11)goto _LL112;else{if((((_tmp1D4->f1).aggr_info).KnownAggr).tag != 2)goto _LL112;_tmp1D5=(struct Cyc_Absyn_Aggrdecl**)(((_tmp1D4->f1).aggr_info).KnownAggr).val;}}_LL111: {
# 824
struct Cyc_Absyn_Aggrfield*sf=
_tmp1D5 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp1D5,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL112: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1D3;if(_tmp1D6->tag != 12)goto _LL114;else{_tmp1D7=_tmp1D6->f2;}}_LL113: {
# 829
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp1D7,_tmp1A9);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10F;}_LL114:;_LL115:
 goto _LL10F;_LL10F:;}
# 834
goto _LL10A;_LL10D:;_LL10E:
 goto _LL10A;_LL10A:;}
# 837
goto _LLDE;_LLED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp1AA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AA->tag != 19)goto _LLEF;else{_tmp1AB=_tmp1AA->f1;}}_LLEE:
# 839
{void*_tmp1D8=Cyc_Tcutil_compress((void*)_check_null(_tmp1AB->topt));void*_tmp1D9=_tmp1D8;struct Cyc_Absyn_Tqual _tmp1DB;struct Cyc_Absyn_Tqual _tmp1DD;_LL117: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DA->tag != 5)goto _LL119;else{_tmp1DB=(_tmp1DA->f1).elt_tq;}}_LL118:
 _tmp1DD=_tmp1DB;goto _LL11A;_LL119: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D9;if(_tmp1DC->tag != 8)goto _LL11B;else{_tmp1DD=(_tmp1DC->f1).tq;}}_LL11A:
 if(!_tmp1DD.real_const)return;goto _LL116;_LL11B:;_LL11C:
 goto _LL116;_LL116:;}
# 844
goto _LLDE;_LLEF: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp1AC=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AC->tag != 11)goto _LLF1;else{_tmp1AD=_tmp1AC->f1;}}_LLF0:
 _tmp1AF=_tmp1AD;goto _LLF2;_LLF1: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp1AE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp194;if(_tmp1AE->tag != 12)goto _LLF3;else{_tmp1AF=_tmp1AE->f1;}}_LLF2:
 Cyc_Tcexp_check_writable(te,_tmp1AF);return;_LLF3:;_LLF4:
 goto _LLDE;_LLDE:;}{
# 849
const char*_tmp7D2;void*_tmp7D1[1];struct Cyc_String_pa_PrintArg_struct _tmp7D0;(_tmp7D0.tag=0,((_tmp7D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp7D1[0]=& _tmp7D0,Cyc_Tcutil_terr(e->loc,((_tmp7D2="attempt to write a const location: %s",_tag_dyneither(_tmp7D2,sizeof(char),38))),_tag_dyneither(_tmp7D1,sizeof(void*),1)))))));};}
# 852
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 855
struct _RegionHandle _tmp1E1=_new_region("temp");struct _RegionHandle*temp=& _tmp1E1;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 858
if(!Cyc_Absyn_is_lvalue(e)){
const char*_tmp7D5;void*_tmp7D4;void*_tmp1E4=(_tmp7D4=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7D5="increment/decrement of non-lvalue",_tag_dyneither(_tmp7D5,sizeof(char),34))),_tag_dyneither(_tmp7D4,sizeof(void*),0)));_npop_handler(0);return _tmp1E4;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 863
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk)){
const char*_tmp7D8;void*_tmp7D7;(_tmp7D7=0,Cyc_Tcutil_terr(e->loc,((_tmp7D8="can't perform arithmetic on abstract pointer type",_tag_dyneither(_tmp7D8,sizeof(char),50))),_tag_dyneither(_tmp7D7,sizeof(void*),0)));}
if(Cyc_Tcutil_is_noalias_pointer(t,0)){
const char*_tmp7DB;void*_tmp7DA;(_tmp7DA=0,Cyc_Tcutil_terr(e->loc,((_tmp7DB="can't perform arithmetic on non-aliasing pointer type",_tag_dyneither(_tmp7DB,sizeof(char),54))),_tag_dyneither(_tmp7DA,sizeof(void*),0)));}}else{
# 872
const char*_tmp7DF;void*_tmp7DE[1];struct Cyc_String_pa_PrintArg_struct _tmp7DD;(_tmp7DD.tag=0,((_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp7DE[0]=& _tmp7DD,Cyc_Tcutil_terr(e->loc,((_tmp7DF="expecting arithmetic or ? type but found %s",_tag_dyneither(_tmp7DF,sizeof(char),44))),_tag_dyneither(_tmp7DE,sizeof(void*),1)))))));}}{
# 874
void*_tmp1EC=t;_npop_handler(0);return _tmp1EC;};};
# 855
;_pop_region(temp);}
# 878
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 880
struct _RegionHandle _tmp1ED=_new_region("r");struct _RegionHandle*r=& _tmp1ED;_push_region(r);{
const char*_tmp7E0;Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,((_tmp7E0="conditional expression",_tag_dyneither(_tmp7E0,sizeof(char),23))));}
# 883
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 889
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp7E1;struct Cyc_List_List _tmp1EF=(_tmp7E1.hd=e3,((_tmp7E1.tl=0,_tmp7E1)));
struct Cyc_List_List _tmp7E2;struct Cyc_List_List _tmp1F0=(_tmp7E2.hd=e2,((_tmp7E2.tl=& _tmp1EF,_tmp7E2)));
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp1F0)){
{const char*_tmp7E7;void*_tmp7E6[2];struct Cyc_String_pa_PrintArg_struct _tmp7E5;struct Cyc_String_pa_PrintArg_struct _tmp7E4;(_tmp7E4.tag=0,((_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt))),((_tmp7E5.tag=0,((_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt))),((_tmp7E6[0]=& _tmp7E5,((_tmp7E6[1]=& _tmp7E4,Cyc_Tcutil_terr(loc,((_tmp7E7="conditional clause types do not match: %s != %s",_tag_dyneither(_tmp7E7,sizeof(char),48))),_tag_dyneither(_tmp7E6,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 897
void*_tmp1F5=t;_npop_handler(0);return _tmp1F5;};};};
# 880
;_pop_region(r);}
# 901
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 903
{const char*_tmp7E8;Cyc_Tcexp_tcTest(te,e1,((_tmp7E8="logical-and expression",_tag_dyneither(_tmp7E8,sizeof(char),23))));}
{const char*_tmp7E9;Cyc_Tcexp_tcTest(te,e2,((_tmp7E9="logical-and expression",_tag_dyneither(_tmp7E9,sizeof(char),23))));}
return Cyc_Absyn_sint_typ;}
# 909
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 911
{const char*_tmp7EA;Cyc_Tcexp_tcTest(te,e1,((_tmp7EA="logical-or expression",_tag_dyneither(_tmp7EA,sizeof(char),22))));}
{const char*_tmp7EB;Cyc_Tcexp_tcTest(te,e2,((_tmp7EB="logical-or expression",_tag_dyneither(_tmp7EB,sizeof(char),22))));}
return Cyc_Absyn_sint_typ;}
# 917
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 924
struct _RegionHandle _tmp1FC=_new_region("r");struct _RegionHandle*r=& _tmp1FC;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);{
# 927
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 931
{void*_tmp1FD=Cyc_Tcutil_compress(t1);void*_tmp1FE=_tmp1FD;_LL11E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1FF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1FE;if(_tmp1FF->tag != 8)goto _LL120;}_LL11F:
{const char*_tmp7EE;void*_tmp7ED;(_tmp7ED=0,Cyc_Tcutil_terr(loc,((_tmp7EE="cannot assign to an array",_tag_dyneither(_tmp7EE,sizeof(char),26))),_tag_dyneither(_tmp7ED,sizeof(void*),0)));}goto _LL11D;_LL120:;_LL121:
 goto _LL11D;_LL11D:;}
# 936
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk)){
const char*_tmp7F1;void*_tmp7F0;(_tmp7F0=0,Cyc_Tcutil_terr(loc,((_tmp7F1="type is abstract (can't determine size).",_tag_dyneither(_tmp7F1,sizeof(char),41))),_tag_dyneither(_tmp7F0,sizeof(void*),0)));}
# 940
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp7F4;void*_tmp7F3;void*_tmp206=(_tmp7F3=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7F4="assignment to non-lvalue",_tag_dyneither(_tmp7F4,sizeof(char),25))),_tag_dyneither(_tmp7F3,sizeof(void*),0)));_npop_handler(0);return _tmp206;}
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
struct _RegionHandle _tmp207=_new_region("temp");struct _RegionHandle*temp=& _tmp207;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2)){
const char*_tmp7F7;void*_tmp7F6;(_tmp7F6=0,Cyc_Tcutil_terr(e2->loc,((_tmp7F7="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp7F7,sizeof(char),49))),_tag_dyneither(_tmp7F6,sizeof(void*),0)));}
# 948
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
const char*_tmp7FC;void*_tmp7FB[2];struct Cyc_String_pa_PrintArg_struct _tmp7FA;struct Cyc_String_pa_PrintArg_struct _tmp7F9;void*_tmp20A=(_tmp7F9.tag=0,((_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp7FA.tag=0,((_tmp7FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp7FB[0]=& _tmp7FA,((_tmp7FB[1]=& _tmp7F9,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp7FC="type mismatch: %s != %s",_tag_dyneither(_tmp7FC,sizeof(char),24))),_tag_dyneither(_tmp7FB,sizeof(void*),2)))))))))))));
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();{
void*_tmp20B=_tmp20A;_npop_handler(1);return _tmp20B;};}
# 944
;_pop_region(temp);}else{
# 956
enum Cyc_Absyn_Primop _tmp210=(enum Cyc_Absyn_Primop)po->v;
void*_tmp211=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp210,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp211,t1) || Cyc_Tcutil_coerceable(_tmp211) && Cyc_Tcutil_coerceable(t1))){
const char*_tmp801;void*_tmp800[2];struct Cyc_String_pa_PrintArg_struct _tmp7FF;struct Cyc_String_pa_PrintArg_struct _tmp7FE;void*_tmp212=
(_tmp7FE.tag=0,((_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 962
Cyc_Absynpp_typ2string(t2)),((_tmp7FF.tag=0,((_tmp7FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 961
Cyc_Absynpp_typ2string(t1)),((_tmp800[0]=& _tmp7FF,((_tmp800[1]=& _tmp7FE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp801="Cannot use this operator in an assignment when the arguments have types %s and %s",_tag_dyneither(_tmp801,sizeof(char),82))),_tag_dyneither(_tmp800,sizeof(void*),2)))))))))))));
# 963
Cyc_Tcutil_unify(_tmp211,t1);
Cyc_Tcutil_explain_failure();{
void*_tmp213=_tmp212;_npop_handler(0);return _tmp213;};}{
# 967
void*_tmp218=_tmp211;_npop_handler(0);return _tmp218;};}{
# 969
void*_tmp219=t1;_npop_handler(0);return _tmp219;};};};
# 924
;_pop_region(r);}
# 973
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _RegionHandle _tmp21A=_new_region("r");struct _RegionHandle*r=& _tmp21A;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);{
# 978
void*_tmp21B=(void*)_check_null(e2->topt);_npop_handler(0);return _tmp21B;};
# 974
;_pop_region(r);}
# 982
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 986
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 989
{void*_tmp21C=Cyc_Tcutil_compress(t1);void*_tmp21D=_tmp21C;_LL123: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp21D;if(_tmp21E->tag != 7)goto _LL125;else{if(_tmp21E->f1 != 0)goto _LL125;}}_LL124:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL122;_LL125: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp21F->tag != 6)goto _LL127;else{if(_tmp21F->f2 != Cyc_Absyn_Char_sz)goto _LL127;}}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp21D;if(_tmp220->tag != 6)goto _LL129;else{if(_tmp220->f2 != Cyc_Absyn_Short_sz)goto _LL129;}}_LL128:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL122;_LL129:;_LL12A:
 goto _LL122;_LL122:;}
# 996
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp221=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp223;struct Cyc_List_List*_tmp224;unsigned int _tmp225;enum Cyc_Absyn_Scope _tmp226;struct Cyc_Absyn_Datatypefield _tmp222=_tmp221;_tmp223=_tmp222.name;_tmp224=_tmp222.typs;_tmp225=_tmp222.loc;_tmp226=_tmp222.sc;
# 999
if(_tmp224 == 0  || _tmp224->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp224->hd)).f2);
# 1002
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1007
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp227=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct _tuple1*_tmp229;struct Cyc_List_List*_tmp22A;unsigned int _tmp22B;enum Cyc_Absyn_Scope _tmp22C;struct Cyc_Absyn_Datatypefield _tmp228=_tmp227;_tmp229=_tmp228.name;_tmp22A=_tmp228.typs;_tmp22B=_tmp228.loc;_tmp22C=_tmp228.sc;
# 1010
if(_tmp22A == 0  || _tmp22A->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple18*)_tmp22A->hd)).f2);
# 1014
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 1019
{const char*_tmp806;void*_tmp805[2];struct Cyc_String_pa_PrintArg_struct _tmp804;struct Cyc_String_pa_PrintArg_struct _tmp803;(_tmp803.tag=0,((_tmp803.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp804.tag=0,((_tmp804.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu)),((_tmp805[0]=& _tmp804,((_tmp805[1]=& _tmp803,Cyc_Tcutil_terr(e->loc,((_tmp806="can't find a field in %s to inject a value of type %s",_tag_dyneither(_tmp806,sizeof(char),54))),_tag_dyneither(_tmp805,sizeof(void*),2)))))))))))));}
return 0;}
# 1025
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 1031
struct Cyc_List_List*_tmp231=args;
int _tmp232=0;
struct _RegionHandle _tmp233=_new_region("ter");struct _RegionHandle*ter=& _tmp233;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp234=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp235=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp234);
Cyc_Tcexp_tcExp(_tmp235,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 1041
void*_tmp236=t;void*_tmp238;struct Cyc_Absyn_Tqual _tmp239;void*_tmp23A;union Cyc_Absyn_Constraint*_tmp23B;union Cyc_Absyn_Constraint*_tmp23C;union Cyc_Absyn_Constraint*_tmp23D;_LL12C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp236;if(_tmp237->tag != 5)goto _LL12E;else{_tmp238=(_tmp237->f1).elt_typ;_tmp239=(_tmp237->f1).elt_tq;_tmp23A=((_tmp237->f1).ptr_atts).rgn;_tmp23B=((_tmp237->f1).ptr_atts).nullable;_tmp23C=((_tmp237->f1).ptr_atts).bounds;_tmp23D=((_tmp237->f1).ptr_atts).zero_term;}}_LL12D:
# 1046
 Cyc_Tcenv_check_rgn_accessible(_tmp235,loc,_tmp23A);
# 1048
Cyc_Tcutil_check_nonzero_bound(loc,_tmp23C);{
void*_tmp23E=Cyc_Tcutil_compress(_tmp238);void*_tmp23F=_tmp23E;struct Cyc_List_List*_tmp241;void*_tmp242;struct Cyc_Absyn_Tqual _tmp243;void*_tmp244;struct Cyc_List_List*_tmp245;int _tmp246;struct Cyc_Absyn_VarargInfo*_tmp247;struct Cyc_List_List*_tmp248;struct Cyc_List_List*_tmp249;_LL131: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23F;if(_tmp240->tag != 9)goto _LL133;else{_tmp241=(_tmp240->f1).tvars;_tmp242=(_tmp240->f1).effect;_tmp243=(_tmp240->f1).ret_tqual;_tmp244=(_tmp240->f1).ret_typ;_tmp245=(_tmp240->f1).args;_tmp246=(_tmp240->f1).c_varargs;_tmp247=(_tmp240->f1).cyc_varargs;_tmp248=(_tmp240->f1).rgn_po;_tmp249=(_tmp240->f1).attributes;}}_LL132:
# 1052
 if(_tmp241 != 0  || _tmp248 != 0){
const char*_tmp809;void*_tmp808;(_tmp808=0,Cyc_Tcutil_terr(e->loc,((_tmp809="function should have been instantiated prior to use -- probably a compiler bug",_tag_dyneither(_tmp809,sizeof(char),79))),_tag_dyneither(_tmp808,sizeof(void*),0)));}
# 1056
if(topt != 0)Cyc_Tcutil_unify(_tmp244,*topt);
# 1058
while(_tmp231 != 0  && _tmp245 != 0){
# 1060
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
void*t2=(*((struct _tuple9*)_tmp245->hd)).f3;
Cyc_Tcexp_tcExp(_tmp235,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,t2,& alias_coercion)){
const char*_tmp80A;struct _dyneither_ptr s0=(_tmp80A="actual argument has type ",_tag_dyneither(_tmp80A,sizeof(char),26));
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
const char*_tmp80B;struct _dyneither_ptr s2=(_tmp80B=" but formal has type ",_tag_dyneither(_tmp80B,sizeof(char),22));
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70){
struct Cyc_String_pa_PrintArg_struct _tmp819;struct Cyc_String_pa_PrintArg_struct _tmp818;struct Cyc_String_pa_PrintArg_struct _tmp817;struct Cyc_String_pa_PrintArg_struct _tmp816;void*_tmp815[4];const char*_tmp814;void*_tmp813;(_tmp813=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp819.tag=0,((_tmp819.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp818.tag=0,((_tmp818.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp817.tag=0,((_tmp817.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp816.tag=0,((_tmp816.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp815[0]=& _tmp816,((_tmp815[1]=& _tmp817,((_tmp815[2]=& _tmp818,((_tmp815[3]=& _tmp819,Cyc_aprintf(((_tmp814="%s\n\t%s\n%s\n\t%s.",_tag_dyneither(_tmp814,sizeof(char),15))),_tag_dyneither(_tmp815,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp813,sizeof(void*),0)));}else{
# 1072
struct Cyc_String_pa_PrintArg_struct _tmp827;struct Cyc_String_pa_PrintArg_struct _tmp826;struct Cyc_String_pa_PrintArg_struct _tmp825;struct Cyc_String_pa_PrintArg_struct _tmp824;void*_tmp823[4];const char*_tmp822;void*_tmp821;(_tmp821=0,Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)((_tmp827.tag=0,((_tmp827.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3),((_tmp826.tag=0,((_tmp826.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmp825.tag=0,((_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmp824.tag=0,((_tmp824.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0),((_tmp823[0]=& _tmp824,((_tmp823[1]=& _tmp825,((_tmp823[2]=& _tmp826,((_tmp823[3]=& _tmp827,Cyc_aprintf(((_tmp822="%s%s%s%s.",_tag_dyneither(_tmp822,sizeof(char),10))),_tag_dyneither(_tmp823,sizeof(void*),4)))))))))))))))))))))))))),_tag_dyneither(_tmp821,sizeof(void*),0)));}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1077
if(alias_coercion){
struct Cyc_List_List*_tmp828;*alias_arg_exps=((_tmp828=_cycalloc(sizeof(*_tmp828)),((_tmp828->hd=(void*)_tmp232,((_tmp828->tl=*alias_arg_exps,_tmp828))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp82B;void*_tmp82A;(_tmp82A=0,Cyc_Tcutil_terr(e1->loc,((_tmp82B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp82B,sizeof(char),49))),_tag_dyneither(_tmp82A,sizeof(void*),0)));}
_tmp231=_tmp231->tl;
_tmp245=_tmp245->tl;
++ _tmp232;}{
# 1088
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp249;for(0;a != 0;a=a->tl){
void*_tmp25F=(void*)a->hd;void*_tmp260=_tmp25F;enum Cyc_Absyn_Format_Type _tmp262;int _tmp263;int _tmp264;_LL136: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp261=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp260;if(_tmp261->tag != 19)goto _LL138;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;_tmp264=_tmp261->f3;}}_LL137:
# 1092
{struct _handler_cons _tmp265;_push_handler(& _tmp265);{int _tmp267=0;if(setjmp(_tmp265.handler))_tmp267=1;if(!_tmp267){
# 1094
{struct Cyc_Absyn_Exp*_tmp268=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp263 - 1);
# 1096
struct Cyc_Core_Opt*fmt_args;
if(_tmp264 == 0)
fmt_args=0;else{
# 1100
struct Cyc_Core_Opt*_tmp82C;fmt_args=((_tmp82C=_cycalloc(sizeof(*_tmp82C)),((_tmp82C->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp264 - 1),_tmp82C))));}
args_already_checked=1;{
struct _RegionHandle _tmp26A=_new_region("temp");struct _RegionHandle*temp=& _tmp26A;_push_region(temp);
switch(_tmp262){case Cyc_Absyn_Printf_ft: _LL13A:
# 1105
 Cyc_Tcexp_check_format_args(_tmp235,_tmp268,fmt_args,_tmp264 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1108
break;case Cyc_Absyn_Scanf_ft: _LL13B:
# 1110
 Cyc_Tcexp_check_format_args(_tmp235,_tmp268,fmt_args,_tmp264 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1113
break;}
# 1103
;_pop_region(temp);};}
# 1094
;_pop_handler();}else{void*_tmp266=(void*)_exn_thrown;void*_tmp26C=_tmp266;void*_tmp26E;_LL13E: {struct Cyc_List_Nth_exn_struct*_tmp26D=(struct Cyc_List_Nth_exn_struct*)_tmp26C;if(_tmp26D->tag != Cyc_List_Nth)goto _LL140;}_LL13F:
# 1117
{const char*_tmp82F;void*_tmp82E;(_tmp82E=0,Cyc_Tcutil_terr(loc,((_tmp82F="bad format arguments",_tag_dyneither(_tmp82F,sizeof(char),21))),_tag_dyneither(_tmp82E,sizeof(void*),0)));}goto _LL13D;_LL140: _tmp26E=_tmp26C;_LL141:(void)_throw(_tmp26E);_LL13D:;}};}
# 1119
goto _LL135;_LL138:;_LL139:
 goto _LL135;_LL135:;}}
# 1123
if(_tmp245 != 0){const char*_tmp832;void*_tmp831;(_tmp831=0,Cyc_Tcutil_terr(loc,((_tmp832="too few arguments for function",_tag_dyneither(_tmp832,sizeof(char),31))),_tag_dyneither(_tmp831,sizeof(void*),0)));}else{
# 1125
if((_tmp231 != 0  || _tmp246) || _tmp247 != 0){
if(_tmp246)
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
Cyc_Tcexp_tcExp(_tmp235,0,(struct Cyc_Absyn_Exp*)_tmp231->hd);}else{
if(_tmp247 == 0){
const char*_tmp835;void*_tmp834;(_tmp834=0,Cyc_Tcutil_terr(loc,((_tmp835="too many arguments for function",_tag_dyneither(_tmp835,sizeof(char),32))),_tag_dyneither(_tmp834,sizeof(void*),0)));}else{
# 1132
struct Cyc_Absyn_VarargInfo _tmp275=*_tmp247;void*_tmp277;int _tmp278;struct Cyc_Absyn_VarargInfo _tmp276=_tmp275;_tmp277=_tmp276.type;_tmp278=_tmp276.inject;{
struct Cyc_Absyn_VarargCallInfo*_tmp836;struct Cyc_Absyn_VarargCallInfo*_tmp279=(_tmp836=_cycalloc(sizeof(*_tmp836)),((_tmp836->num_varargs=0,((_tmp836->injectors=0,((_tmp836->vai=_tmp247,_tmp836)))))));
# 1136
*vararg_call_info=_tmp279;
# 1138
if(!_tmp278)
# 1140
for(0;_tmp231 != 0;(_tmp231=_tmp231->tl,_tmp232 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
++ _tmp279->num_varargs;
Cyc_Tcexp_tcExp(_tmp235,& _tmp277,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp235,e1,_tmp277,& alias_coercion)){
{const char*_tmp83B;void*_tmp83A[2];struct Cyc_String_pa_PrintArg_struct _tmp839;struct Cyc_String_pa_PrintArg_struct _tmp838;(_tmp838.tag=0,((_tmp838.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt))),((_tmp839.tag=0,((_tmp839.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp277)),((_tmp83A[0]=& _tmp839,((_tmp83A[1]=& _tmp838,Cyc_Tcutil_terr(loc,((_tmp83B="vararg requires type %s but argument has type %s",_tag_dyneither(_tmp83B,sizeof(char),49))),_tag_dyneither(_tmp83A,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}
# 1150
if(alias_coercion){
struct Cyc_List_List*_tmp83C;*alias_arg_exps=((_tmp83C=_cycalloc(sizeof(*_tmp83C)),((_tmp83C->hd=(void*)_tmp232,((_tmp83C->tl=*alias_arg_exps,_tmp83C))))));}
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp277) && !
Cyc_Tcutil_is_noalias_path(ter,e1)){
const char*_tmp83F;void*_tmp83E;(_tmp83E=0,Cyc_Tcutil_terr(e1->loc,((_tmp83F="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp83F,sizeof(char),49))),_tag_dyneither(_tmp83E,sizeof(void*),0)));}}else{
# 1159
void*_tmp281=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp277));void*_tmp282=_tmp281;struct Cyc_Absyn_Datatypedecl*_tmp284;struct Cyc_List_List*_tmp285;_LL143: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp283=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp282;if(_tmp283->tag != 3)goto _LL145;else{if((((_tmp283->f1).datatype_info).KnownDatatype).tag != 2)goto _LL145;_tmp284=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp283->f1).datatype_info).KnownDatatype).val);_tmp285=(_tmp283->f1).targs;}}_LL144: {
# 1163
struct Cyc_Absyn_Datatypedecl*_tmp286=*Cyc_Tcenv_lookup_datatypedecl(_tmp235,loc,_tmp284->name);
struct Cyc_List_List*fields=0;
if(_tmp286->fields == 0){
const char*_tmp843;void*_tmp842[1];struct Cyc_String_pa_PrintArg_struct _tmp841;(_tmp841.tag=0,((_tmp841.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp277)),((_tmp842[0]=& _tmp841,Cyc_Tcutil_terr(loc,((_tmp843="can't inject into abstract datatype %s",_tag_dyneither(_tmp843,sizeof(char),39))),_tag_dyneither(_tmp842,sizeof(void*),1)))))));}else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp286->fields))->v;}
# 1175
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp277),Cyc_Tcenv_curr_rgn(_tmp235))){
const char*_tmp848;void*_tmp847[2];struct Cyc_String_pa_PrintArg_struct _tmp846;struct Cyc_String_pa_PrintArg_struct _tmp845;(_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp235))),((_tmp846.tag=0,((_tmp846.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp277)),((_tmp847[0]=& _tmp846,((_tmp847[1]=& _tmp845,Cyc_Tcutil_terr(loc,((_tmp848="can't unify pointer region for %s to curr_rgn %s",_tag_dyneither(_tmp848,sizeof(char),49))),_tag_dyneither(_tmp847,sizeof(void*),2)))))))))))));}{
# 1179
struct _RegionHandle _tmp28E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp28E;_push_region(rgn);{
struct Cyc_List_List*_tmp28F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp286->tvs,_tmp285);
for(0;_tmp231 != 0;_tmp231=_tmp231->tl){
++ _tmp279->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp231->hd;
# 1185
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp235,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1)){
const char*_tmp84B;void*_tmp84A;(_tmp84A=0,Cyc_Tcutil_terr(e1->loc,((_tmp84B="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp84B,sizeof(char),49))),_tag_dyneither(_tmp84A,sizeof(void*),0)));}}{
# 1191
struct Cyc_Absyn_Datatypefield*_tmp292=Cyc_Tcexp_tcInjection(_tmp235,e1,Cyc_Tcutil_pointer_elt_type(_tmp277),rgn,_tmp28F,fields);
# 1193
if(_tmp292 != 0){
struct Cyc_List_List*_tmp84C;_tmp279->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp279->injectors,(
(_tmp84C=_cycalloc(sizeof(*_tmp84C)),((_tmp84C->hd=_tmp292,((_tmp84C->tl=0,_tmp84C)))))));}};};}}
# 1199
_npop_handler(0);goto _LL142;
# 1179
;_pop_region(rgn);};}_LL145:;_LL146:
# 1200
{const char*_tmp84F;void*_tmp84E;(_tmp84E=0,Cyc_Tcutil_terr(loc,((_tmp84F="bad inject vararg type",_tag_dyneither(_tmp84F,sizeof(char),23))),_tag_dyneither(_tmp84E,sizeof(void*),0)));}goto _LL142;_LL142:;}};}}}}
# 1205
if(*alias_arg_exps == 0)
# 1214 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp235,loc,(void*)_check_null(_tmp242));{
# 1216
void*_tmp297=_tmp244;_npop_handler(0);return _tmp297;};};_LL133:;_LL134: {
const char*_tmp853;void*_tmp852[1];struct Cyc_String_pa_PrintArg_struct _tmp851;void*_tmp29B=(_tmp851.tag=0,((_tmp851.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp852[0]=& _tmp851,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp853="expected pointer to function but found %s",_tag_dyneither(_tmp853,sizeof(char),42))),_tag_dyneither(_tmp852,sizeof(void*),1)))))));_npop_handler(0);return _tmp29B;}_LL130:;};_LL12E:;_LL12F: {
# 1219
const char*_tmp857;void*_tmp856[1];struct Cyc_String_pa_PrintArg_struct _tmp855;void*_tmp29F=(_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp856[0]=& _tmp855,Cyc_Tcexp_expr_err(_tmp235,loc,topt,((_tmp857="expected pointer to function but found %s",_tag_dyneither(_tmp857,sizeof(char),42))),_tag_dyneither(_tmp856,sizeof(void*),1)))))));_npop_handler(0);return _tmp29F;}_LL12B:;};}
# 1034 "tcexp.cyc"
;_pop_region(ter);}
# 1225 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp2A0=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp2A0,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp2A0,& bogus)){
const char*_tmp85C;void*_tmp85B[2];struct Cyc_String_pa_PrintArg_struct _tmp85A;struct Cyc_String_pa_PrintArg_struct _tmp859;(_tmp859.tag=0,((_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp85A.tag=0,((_tmp85A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1230
Cyc_Absynpp_typ2string(_tmp2A0)),((_tmp85B[0]=& _tmp85A,((_tmp85B[1]=& _tmp859,Cyc_Tcutil_terr(loc,((_tmp85C="expected %s but found %s",_tag_dyneither(_tmp85C,sizeof(char),25))),_tag_dyneither(_tmp85B,sizeof(void*),2)))))))))))));}
# 1232
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1237
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1239
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1241
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp2A5=t1;void*_tmp2A7;struct Cyc_Absyn_Tqual _tmp2A8;void*_tmp2A9;union Cyc_Absyn_Constraint*_tmp2AA;union Cyc_Absyn_Constraint*_tmp2AB;union Cyc_Absyn_Constraint*_tmp2AC;_LL148: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A5;if(_tmp2A6->tag != 5)goto _LL14A;else{_tmp2A7=(_tmp2A6->f1).elt_typ;_tmp2A8=(_tmp2A6->f1).elt_tq;_tmp2A9=((_tmp2A6->f1).ptr_atts).rgn;_tmp2AA=((_tmp2A6->f1).ptr_atts).nullable;_tmp2AB=((_tmp2A6->f1).ptr_atts).bounds;_tmp2AC=((_tmp2A6->f1).ptr_atts).zero_term;}}_LL149:
# 1245
{void*_tmp2AD=Cyc_Tcutil_compress(_tmp2A7);void*_tmp2AE=_tmp2AD;struct Cyc_List_List*_tmp2B0;void*_tmp2B1;struct Cyc_Absyn_Tqual _tmp2B2;void*_tmp2B3;struct Cyc_List_List*_tmp2B4;int _tmp2B5;struct Cyc_Absyn_VarargInfo*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B8;_LL14D: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2AF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2AE;if(_tmp2AF->tag != 9)goto _LL14F;else{_tmp2B0=(_tmp2AF->f1).tvars;_tmp2B1=(_tmp2AF->f1).effect;_tmp2B2=(_tmp2AF->f1).ret_tqual;_tmp2B3=(_tmp2AF->f1).ret_typ;_tmp2B4=(_tmp2AF->f1).args;_tmp2B5=(_tmp2AF->f1).c_varargs;_tmp2B6=(_tmp2AF->f1).cyc_varargs;_tmp2B7=(_tmp2AF->f1).rgn_po;_tmp2B8=(_tmp2AF->f1).attributes;}}_LL14E: {
# 1247
struct _RegionHandle _tmp2B9=_new_region("temp");struct _RegionHandle*temp=& _tmp2B9;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1250
for(0;ts != 0  && _tmp2B0 != 0;(ts=ts->tl,_tmp2B0=_tmp2B0->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp2B0->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple13*_tmp85F;struct Cyc_List_List*_tmp85E;instantiation=(
(_tmp85E=_region_malloc(temp,sizeof(*_tmp85E)),((_tmp85E->hd=((_tmp85F=_region_malloc(temp,sizeof(*_tmp85F)),((_tmp85F->f1=(struct Cyc_Absyn_Tvar*)_tmp2B0->hd,((_tmp85F->f2=(void*)ts->hd,_tmp85F)))))),((_tmp85E->tl=instantiation,_tmp85E))))));};}
# 1257
if(ts != 0){
const char*_tmp862;void*_tmp861;void*_tmp2BE=
(_tmp861=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp862="too many type variables in instantiation",_tag_dyneither(_tmp862,sizeof(char),41))),_tag_dyneither(_tmp861,sizeof(void*),0)));_npop_handler(0);return _tmp2BE;}
# 1263
if(_tmp2B0 == 0){
_tmp2B7=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp2B7);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp2B7);
_tmp2B7=0;}{
# 1268
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp868;struct Cyc_Absyn_FnInfo _tmp867;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp866;void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)(
(_tmp866=_cycalloc(sizeof(*_tmp866)),((_tmp866[0]=((_tmp868.tag=9,((_tmp868.f1=((_tmp867.tvars=_tmp2B0,((_tmp867.effect=_tmp2B1,((_tmp867.ret_tqual=_tmp2B2,((_tmp867.ret_typ=_tmp2B3,((_tmp867.args=_tmp2B4,((_tmp867.c_varargs=_tmp2B5,((_tmp867.cyc_varargs=_tmp2B6,((_tmp867.rgn_po=_tmp2B7,((_tmp867.attributes=_tmp2B8,_tmp867)))))))))))))))))),_tmp868)))),_tmp866)))));
# 1272
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp86E;struct Cyc_Absyn_PtrInfo _tmp86D;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp86C;void*_tmp2C2=(void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp86E.tag=5,((_tmp86E.f1=((_tmp86D.elt_typ=new_fn_typ,((_tmp86D.elt_tq=_tmp2A8,((_tmp86D.ptr_atts=(((_tmp86D.ptr_atts).rgn=_tmp2A9,(((_tmp86D.ptr_atts).nullable=_tmp2AA,(((_tmp86D.ptr_atts).bounds=_tmp2AB,(((_tmp86D.ptr_atts).zero_term=_tmp2AC,(((_tmp86D.ptr_atts).ptrloc=0,_tmp86D.ptr_atts)))))))))),_tmp86D)))))),_tmp86E)))),_tmp86C))));_npop_handler(0);return _tmp2C2;};}
# 1248
;_pop_region(temp);}_LL14F:;_LL150:
# 1274
 goto _LL14C;_LL14C:;}
# 1276
goto _LL147;_LL14A:;_LL14B:
 goto _LL147;_LL147:;}{
# 1279
const char*_tmp872;void*_tmp871[1];struct Cyc_String_pa_PrintArg_struct _tmp870;return(_tmp870.tag=0,((_tmp870.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1)),((_tmp871[0]=& _tmp870,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp872="expecting polymorphic type but found %s",_tag_dyneither(_tmp872,sizeof(char),40))),_tag_dyneither(_tmp871,sizeof(void*),1)))))));};};}
# 1284
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1286
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1290
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1295
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1302
Cyc_Tcutil_unify(t2,t);{
const char*_tmp877;void*_tmp876[2];struct Cyc_String_pa_PrintArg_struct _tmp875;struct Cyc_String_pa_PrintArg_struct _tmp874;void*_tmp2C9=(_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2)),((_tmp876[0]=& _tmp875,((_tmp876[1]=& _tmp874,Cyc_Tcexp_expr_err(te,loc,& t,((_tmp877="cannot cast %s to %s",_tag_dyneither(_tmp877,sizeof(char),21))),_tag_dyneither(_tmp876,sizeof(void*),2)))))))))))));
Cyc_Tcutil_explain_failure();
return _tmp2C9;};}}}
# 1312
{struct _tuple0 _tmp878;struct _tuple0 _tmp2CE=(_tmp878.f1=e->r,((_tmp878.f2=Cyc_Tcutil_compress(t),_tmp878)));struct _tuple0 _tmp2CF=_tmp2CE;int _tmp2D1;union Cyc_Absyn_Constraint*_tmp2D3;union Cyc_Absyn_Constraint*_tmp2D4;union Cyc_Absyn_Constraint*_tmp2D5;_LL152:{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2D0=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp2CF.f1;if(_tmp2D0->tag != 33)goto _LL154;else{_tmp2D1=(_tmp2D0->f1).fat_result;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CF.f2;if(_tmp2D2->tag != 5)goto _LL154;else{_tmp2D3=((_tmp2D2->f1).ptr_atts).nullable;_tmp2D4=((_tmp2D2->f1).ptr_atts).bounds;_tmp2D5=((_tmp2D2->f1).ptr_atts).zero_term;}};_LL153:
# 1316
 if((_tmp2D1  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2D5)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2D3)){
void*_tmp2D6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2D4);void*_tmp2D7=_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D9;_LL157: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2D8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D7;if(_tmp2D8->tag != 1)goto _LL159;else{_tmp2D9=_tmp2D8->f1;}}_LL158:
# 1319
 if((Cyc_Evexp_eval_const_uint_exp(_tmp2D9)).f1 == 1){
const char*_tmp87B;void*_tmp87A;(_tmp87A=0,Cyc_Tcutil_warn(loc,((_tmp87B="cast from ? pointer to * pointer will lose size information",_tag_dyneither(_tmp87B,sizeof(char),60))),_tag_dyneither(_tmp87A,sizeof(void*),0)));}
goto _LL156;_LL159:;_LL15A:
 goto _LL156;_LL156:;}
# 1325
goto _LL151;_LL154:;_LL155:
 goto _LL151;_LL151:;}
# 1328
return t;};}
# 1332
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1334
void**_tmp2DD=0;
struct Cyc_Absyn_Tqual _tmp2DE=Cyc_Absyn_empty_tqual(0);
int _tmp2DF=0;
if(topt != 0){
void*_tmp2E0=Cyc_Tcutil_compress(*topt);void*_tmp2E1=_tmp2E0;void**_tmp2E3;struct Cyc_Absyn_Tqual _tmp2E4;union Cyc_Absyn_Constraint*_tmp2E5;_LL15C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1;if(_tmp2E2->tag != 5)goto _LL15E;else{_tmp2E3=(void**)&(_tmp2E2->f1).elt_typ;_tmp2E4=(_tmp2E2->f1).elt_tq;_tmp2E5=((_tmp2E2->f1).ptr_atts).nullable;}}_LL15D:
# 1340
 _tmp2DD=_tmp2E3;
_tmp2DE=_tmp2E4;
_tmp2DF=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp2E5);
goto _LL15B;_LL15E:;_LL15F:
 goto _LL15B;_LL15B:;}{
# 1354
struct _RegionHandle _tmp2E6=_new_region("r");struct _RegionHandle*r=& _tmp2E6;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp2DD,e);
# 1358
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt))){
const char*_tmp87E;void*_tmp87D;(_tmp87D=0,Cyc_Tcutil_terr(e->loc,((_tmp87E="Cannot take the address of an alias-free path",_tag_dyneither(_tmp87E,sizeof(char),46))),_tag_dyneither(_tmp87D,sizeof(void*),0)));}
# 1365
{void*_tmp2E9=e->r;void*_tmp2EA=_tmp2E9;struct Cyc_Absyn_Exp*_tmp2EC;struct Cyc_Absyn_Exp*_tmp2ED;_LL161: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2EB=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2EA;if(_tmp2EB->tag != 22)goto _LL163;else{_tmp2EC=_tmp2EB->f1;_tmp2ED=_tmp2EB->f2;}}_LL162:
# 1367
{void*_tmp2EE=Cyc_Tcutil_compress((void*)_check_null(_tmp2EC->topt));void*_tmp2EF=_tmp2EE;_LL166: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2EF;if(_tmp2F0->tag != 10)goto _LL168;}_LL167:
 goto _LL165;_LL168:;_LL169:
# 1372
 e0->r=(Cyc_Absyn_add_exp(_tmp2EC,_tmp2ED,0))->r;{
void*_tmp2F1=Cyc_Tcexp_tcPlus(te,_tmp2EC,_tmp2ED);_npop_handler(0);return _tmp2F1;};_LL165:;}
# 1375
goto _LL160;_LL163:;_LL164:
 goto _LL160;_LL160:;}
# 1380
{void*_tmp2F2=e->r;void*_tmp2F3=_tmp2F2;_LL16B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2F4=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2F4->tag != 20)goto _LL16D;else{if(_tmp2F4->f3 != 1)goto _LL16D;}}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2F5=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2F3;if(_tmp2F5->tag != 21)goto _LL16F;else{if(_tmp2F5->f3 != 1)goto _LL16F;}}_LL16E:
# 1383
{const char*_tmp881;void*_tmp880;(_tmp880=0,Cyc_Tcutil_terr(e->loc,((_tmp881="cannot take the address of a @tagged union member",_tag_dyneither(_tmp881,sizeof(char),50))),_tag_dyneither(_tmp880,sizeof(void*),0)));}
goto _LL16A;_LL16F:;_LL170:
 goto _LL16A;_LL16A:;}{
# 1389
struct _tuple14 _tmp2F8=Cyc_Tcutil_addressof_props(te,e);int _tmp2FA;void*_tmp2FB;struct _tuple14 _tmp2F9=_tmp2F8;_tmp2FA=_tmp2F9.f1;_tmp2FB=_tmp2F9.f2;
# 1391
if(Cyc_Tcutil_is_noalias_region(_tmp2FB,0)){
const char*_tmp884;void*_tmp883;(_tmp883=0,Cyc_Tcutil_terr(e->loc,((_tmp884="using & would manufacture an alias to an alias-free pointer",_tag_dyneither(_tmp884,sizeof(char),60))),_tag_dyneither(_tmp883,sizeof(void*),0)));}{
# 1394
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp2FA){
tq.print_const=1;
tq.real_const=1;}{
# 1400
void*t=_tmp2DF?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp2FB,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp2FB,tq,Cyc_Absyn_false_conref);
void*_tmp2FE=t;_npop_handler(0);return _tmp2FE;};};};
# 1354
;_pop_region(r);};}
# 1407
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1411
return Cyc_Absyn_uint_typ;
# 1413
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t)){
const char*_tmp888;void*_tmp887[1];struct Cyc_String_pa_PrintArg_struct _tmp886;(_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp887[0]=& _tmp886,Cyc_Tcutil_terr(loc,((_tmp888="sizeof applied to type %s, which has unknown size here",_tag_dyneither(_tmp888,sizeof(char),55))),_tag_dyneither(_tmp887,sizeof(void*),1)))))));}
if(topt != 0){
void*_tmp302=Cyc_Tcutil_compress(*topt);void*_tmp303=_tmp302;void*_tmp304;void*_tmp306;_LL172: _tmp304=_tmp303;{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp305=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp304;if(_tmp305->tag != 19)goto _LL174;else{_tmp306=(void*)_tmp305->f1;}};_LL173: {
# 1422
struct Cyc_Absyn_Exp*_tmp307=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp88B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp88A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp308=(_tmp88A=_cycalloc(sizeof(*_tmp88A)),((_tmp88A[0]=((_tmp88B.tag=18,((_tmp88B.f1=_tmp307,_tmp88B)))),_tmp88A)));
if(Cyc_Tcutil_unify(_tmp306,(void*)_tmp308))return _tmp304;
goto _LL171;}_LL174:;_LL175:
 goto _LL171;_LL171:;}
# 1428
return Cyc_Absyn_uint_typ;}
# 1431
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1438
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1440
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp30B=fs;
void*_tmp30C=t_orig;
for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
void*_tmp30D=(void*)_tmp30B->hd;
void*_tmp30E=_tmp30D;struct _dyneither_ptr*_tmp310;unsigned int _tmp312;_LL177: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp30F=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp30E;if(_tmp30F->tag != 0)goto _LL179;else{_tmp310=_tmp30F->f1;}}_LL178: {
# 1448
int bad_type=1;
{void*_tmp313=Cyc_Tcutil_compress(_tmp30C);void*_tmp314=_tmp313;struct Cyc_Absyn_Aggrdecl**_tmp316;struct Cyc_List_List*_tmp318;_LL17C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp314;if(_tmp315->tag != 11)goto _LL17E;else{if((((_tmp315->f1).aggr_info).KnownAggr).tag != 2)goto _LL17E;_tmp316=(struct Cyc_Absyn_Aggrdecl**)(((_tmp315->f1).aggr_info).KnownAggr).val;}}_LL17D:
# 1451
 if((*_tmp316)->impl == 0)goto _LL17B;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp310,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp316)->impl))->fields);
if(!((unsigned int)t2)){
const char*_tmp88F;void*_tmp88E[1];struct Cyc_String_pa_PrintArg_struct _tmp88D;(_tmp88D.tag=0,((_tmp88D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp310),((_tmp88E[0]=& _tmp88D,Cyc_Tcutil_terr(loc,((_tmp88F="no field of struct/union has name %s",_tag_dyneither(_tmp88F,sizeof(char),37))),_tag_dyneither(_tmp88E,sizeof(void*),1)))))));}else{
# 1456
_tmp30C=t2;}
bad_type=0;
goto _LL17B;};_LL17E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp314;if(_tmp317->tag != 12)goto _LL180;else{_tmp318=_tmp317->f2;}}_LL17F: {
# 1460
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp310,_tmp318);
if(!((unsigned int)t2)){
const char*_tmp893;void*_tmp892[1];struct Cyc_String_pa_PrintArg_struct _tmp891;(_tmp891.tag=0,((_tmp891.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp310),((_tmp892[0]=& _tmp891,Cyc_Tcutil_terr(loc,((_tmp893="no field of struct/union has name %s",_tag_dyneither(_tmp893,sizeof(char),37))),_tag_dyneither(_tmp892,sizeof(void*),1)))))));}else{
# 1464
_tmp30C=t2;}
bad_type=0;
goto _LL17B;}_LL180:;_LL181:
 goto _LL17B;_LL17B:;}
# 1469
if(bad_type){
if(_tmp30B == fs){
const char*_tmp897;void*_tmp896[1];struct Cyc_String_pa_PrintArg_struct _tmp895;(_tmp895.tag=0,((_tmp895.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp30C)),((_tmp896[0]=& _tmp895,Cyc_Tcutil_terr(loc,((_tmp897="%s is not a known struct/union type",_tag_dyneither(_tmp897,sizeof(char),36))),_tag_dyneither(_tmp896,sizeof(void*),1)))))));}else{
# 1473
const char*_tmp89B;void*_tmp89A[1];struct Cyc_String_pa_PrintArg_struct _tmp899;struct _dyneither_ptr _tmp322=(_tmp899.tag=0,((_tmp899.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig)),((_tmp89A[0]=& _tmp899,Cyc_aprintf(((_tmp89B="(%s)",_tag_dyneither(_tmp89B,sizeof(char),5))),_tag_dyneither(_tmp89A,sizeof(void*),1)))))));
struct Cyc_List_List*x;
for(x=fs;x != _tmp30B;x=x->tl){
void*_tmp323=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp324=_tmp323;struct _dyneither_ptr*_tmp326;unsigned int _tmp328;_LL183: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp325=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp324;if(_tmp325->tag != 0)goto _LL185;else{_tmp326=_tmp325->f1;}}_LL184:
# 1478
{const char*_tmp8A0;void*_tmp89F[2];struct Cyc_String_pa_PrintArg_struct _tmp89E;struct Cyc_String_pa_PrintArg_struct _tmp89D;_tmp322=((_tmp89D.tag=0,((_tmp89D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp326),((_tmp89E.tag=0,((_tmp89E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp322),((_tmp89F[0]=& _tmp89E,((_tmp89F[1]=& _tmp89D,Cyc_aprintf(((_tmp8A0="%s.%s",_tag_dyneither(_tmp8A0,sizeof(char),6))),_tag_dyneither(_tmp89F,sizeof(void*),2))))))))))))));}goto _LL182;_LL185: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp327=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp324;if(_tmp327->tag != 1)goto _LL182;else{_tmp328=_tmp327->f1;}}_LL186:
# 1480
{const char*_tmp8A5;void*_tmp8A4[2];struct Cyc_String_pa_PrintArg_struct _tmp8A3;struct Cyc_Int_pa_PrintArg_struct _tmp8A2;_tmp322=((_tmp8A2.tag=1,((_tmp8A2.f1=(unsigned long)((int)_tmp328),((_tmp8A3.tag=0,((_tmp8A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp322),((_tmp8A4[0]=& _tmp8A3,((_tmp8A4[1]=& _tmp8A2,Cyc_aprintf(((_tmp8A5="%s.%d",_tag_dyneither(_tmp8A5,sizeof(char),6))),_tag_dyneither(_tmp8A4,sizeof(void*),2))))))))))))));}goto _LL182;_LL182:;}{
# 1482
const char*_tmp8AA;void*_tmp8A9[2];struct Cyc_String_pa_PrintArg_struct _tmp8A8;struct Cyc_String_pa_PrintArg_struct _tmp8A7;(_tmp8A7.tag=0,((_tmp8A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp30C)),((_tmp8A8.tag=0,((_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp322),((_tmp8A9[0]=& _tmp8A8,((_tmp8A9[1]=& _tmp8A7,Cyc_Tcutil_terr(loc,((_tmp8AA="%s == %s is not a struct/union type",_tag_dyneither(_tmp8AA,sizeof(char),36))),_tag_dyneither(_tmp8A9,sizeof(void*),2)))))))))))));};}}
# 1485
goto _LL176;}_LL179: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp311=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp30E;if(_tmp311->tag != 1)goto _LL176;else{_tmp312=_tmp311->f1;}}_LL17A: {
# 1487
int bad_type=1;
{void*_tmp338=Cyc_Tcutil_compress(_tmp30C);void*_tmp339=_tmp338;struct Cyc_Absyn_Aggrdecl**_tmp33B;struct Cyc_List_List*_tmp33D;struct Cyc_List_List*_tmp33F;struct Cyc_Absyn_Datatypefield*_tmp341;_LL188: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp339;if(_tmp33A->tag != 11)goto _LL18A;else{if((((_tmp33A->f1).aggr_info).KnownAggr).tag != 2)goto _LL18A;_tmp33B=(struct Cyc_Absyn_Aggrdecl**)(((_tmp33A->f1).aggr_info).KnownAggr).val;}}_LL189:
# 1490
 if((*_tmp33B)->impl == 0)
goto _LL187;
_tmp33D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp33B)->impl))->fields;goto _LL18B;_LL18A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp339;if(_tmp33C->tag != 12)goto _LL18C;else{_tmp33D=_tmp33C->f2;}}_LL18B:
# 1494
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33D)<= _tmp312){
const char*_tmp8AF;void*_tmp8AE[2];struct Cyc_Int_pa_PrintArg_struct _tmp8AD;struct Cyc_Int_pa_PrintArg_struct _tmp8AC;(_tmp8AC.tag=1,((_tmp8AC.f1=(unsigned long)((int)_tmp312),((_tmp8AD.tag=1,((_tmp8AD.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33D),((_tmp8AE[0]=& _tmp8AD,((_tmp8AE[1]=& _tmp8AC,Cyc_Tcutil_terr(loc,((_tmp8AF="struct/union has too few components: %d <= %d",_tag_dyneither(_tmp8AF,sizeof(char),46))),_tag_dyneither(_tmp8AE,sizeof(void*),2)))))))))))));}else{
# 1498
_tmp30C=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp33D,(int)_tmp312))->type;}
bad_type=0;
goto _LL187;_LL18C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp339;if(_tmp33E->tag != 10)goto _LL18E;else{_tmp33F=_tmp33E->f1;}}_LL18D:
# 1502
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33F)<= _tmp312){
const char*_tmp8B4;void*_tmp8B3[2];struct Cyc_Int_pa_PrintArg_struct _tmp8B2;struct Cyc_Int_pa_PrintArg_struct _tmp8B1;(_tmp8B1.tag=1,((_tmp8B1.f1=(unsigned long)((int)_tmp312),((_tmp8B2.tag=1,((_tmp8B2.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp33F),((_tmp8B3[0]=& _tmp8B2,((_tmp8B3[1]=& _tmp8B1,Cyc_Tcutil_terr(loc,((_tmp8B4="tuple has too few components: %d <= %d",_tag_dyneither(_tmp8B4,sizeof(char),39))),_tag_dyneither(_tmp8B3,sizeof(void*),2)))))))))))));}else{
# 1506
_tmp30C=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp33F,(int)_tmp312)).f2;}
bad_type=0;
goto _LL187;_LL18E: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp339;if(_tmp340->tag != 4)goto _LL190;else{if((((_tmp340->f1).field_info).KnownDatatypefield).tag != 2)goto _LL190;_tmp341=((struct _tuple2)(((_tmp340->f1).field_info).KnownDatatypefield).val).f2;}}_LL18F:
# 1510
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp341->typs)< _tmp312){
const char*_tmp8B9;void*_tmp8B8[2];struct Cyc_Int_pa_PrintArg_struct _tmp8B7;struct Cyc_Int_pa_PrintArg_struct _tmp8B6;(_tmp8B6.tag=1,((_tmp8B6.f1=(unsigned long)((int)_tmp312),((_tmp8B7.tag=1,((_tmp8B7.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp341->typs),((_tmp8B8[0]=& _tmp8B7,((_tmp8B8[1]=& _tmp8B6,Cyc_Tcutil_terr(loc,((_tmp8B9="datatype field has too few components: %d < %d",_tag_dyneither(_tmp8B9,sizeof(char),47))),_tag_dyneither(_tmp8B8,sizeof(void*),2)))))))))))));}else{
# 1514
if(_tmp312 != 0)
_tmp30C=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp341->typs,(int)(_tmp312 - 1))).f2;else{
if(_tmp30B->tl != 0){
const char*_tmp8BC;void*_tmp8BB;(_tmp8BB=0,Cyc_Tcutil_terr(loc,((_tmp8BC="datatype field index 0 refers to the tag; cannot be indexed through",_tag_dyneither(_tmp8BC,sizeof(char),68))),_tag_dyneither(_tmp8BB,sizeof(void*),0)));}}}
# 1519
bad_type=0;
goto _LL187;_LL190:;_LL191:
 goto _LL187;_LL187:;}
# 1523
if(bad_type){
const char*_tmp8C0;void*_tmp8BF[1];struct Cyc_String_pa_PrintArg_struct _tmp8BE;(_tmp8BE.tag=0,((_tmp8BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp30C)),((_tmp8BF[0]=& _tmp8BE,Cyc_Tcutil_terr(loc,((_tmp8C0="%s is not a known type",_tag_dyneither(_tmp8C0,sizeof(char),23))),_tag_dyneither(_tmp8BF,sizeof(void*),1)))))));}
goto _LL176;}_LL176:;}
# 1528
return Cyc_Absyn_uint_typ;};}
# 1532
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp353=_new_region("r");struct _RegionHandle*r=& _tmp353;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp354=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp354,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp355=t;void*_tmp358;void*_tmp359;union Cyc_Absyn_Constraint*_tmp35A;union Cyc_Absyn_Constraint*_tmp35B;_LL193: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp356=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp355;if(_tmp356->tag != 1)goto _LL195;}_LL194: {
# 1539
struct Cyc_List_List*_tmp35C=Cyc_Tcenv_lookup_type_vars(_tmp354);
struct Cyc_Core_Opt*_tmp8C1;void*_tmp35D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,((_tmp8C1=_cycalloc(sizeof(*_tmp8C1)),((_tmp8C1->v=_tmp35C,_tmp8C1)))));
struct Cyc_Core_Opt*_tmp8C2;void*_tmp35E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,((_tmp8C2=_cycalloc(sizeof(*_tmp8C2)),((_tmp8C2->v=_tmp35C,_tmp8C2)))));
union Cyc_Absyn_Constraint*_tmp35F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp360=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp8C3;struct Cyc_Absyn_PtrAtts _tmp361=(_tmp8C3.rgn=_tmp35E,((_tmp8C3.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),((_tmp8C3.bounds=_tmp35F,((_tmp8C3.zero_term=_tmp360,((_tmp8C3.ptrloc=0,_tmp8C3)))))))));
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp8C9;struct Cyc_Absyn_PtrInfo _tmp8C8;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8C7;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp362=(_tmp8C7=_cycalloc(sizeof(*_tmp8C7)),((_tmp8C7[0]=((_tmp8C9.tag=5,((_tmp8C9.f1=((_tmp8C8.elt_typ=_tmp35D,((_tmp8C8.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp8C8.ptr_atts=_tmp361,_tmp8C8)))))),_tmp8C9)))),_tmp8C7)));
Cyc_Tcutil_unify(t,(void*)_tmp362);
_tmp358=_tmp35D;_tmp359=_tmp35E;_tmp35A=_tmp35F;_tmp35B=_tmp360;goto _LL196;}_LL195: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp355;if(_tmp357->tag != 5)goto _LL197;else{_tmp358=(_tmp357->f1).elt_typ;_tmp359=((_tmp357->f1).ptr_atts).rgn;_tmp35A=((_tmp357->f1).ptr_atts).bounds;_tmp35B=((_tmp357->f1).ptr_atts).zero_term;}}_LL196:
# 1549
 Cyc_Tcenv_check_rgn_accessible(_tmp354,loc,_tmp359);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp35A);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp358),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp354)){
const char*_tmp8CC;void*_tmp8CB;(_tmp8CB=0,Cyc_Tcutil_terr(loc,((_tmp8CC="can't dereference abstract pointer type",_tag_dyneither(_tmp8CC,sizeof(char),40))),_tag_dyneither(_tmp8CB,sizeof(void*),0)));}{
void*_tmp36B=_tmp358;_npop_handler(0);return _tmp36B;};_LL197:;_LL198: {
# 1555
const char*_tmp8D0;void*_tmp8CF[1];struct Cyc_String_pa_PrintArg_struct _tmp8CE;void*_tmp36F=(_tmp8CE.tag=0,((_tmp8CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp8CF[0]=& _tmp8CE,Cyc_Tcexp_expr_err(_tmp354,loc,topt,((_tmp8D0="expecting *, @, or ? type but found %s",_tag_dyneither(_tmp8D0,sizeof(char),39))),_tag_dyneither(_tmp8CF,sizeof(void*),1)))))));_npop_handler(0);return _tmp36F;}_LL192:;};}
# 1534
;_pop_region(r);}
# 1561
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1565
struct _RegionHandle _tmp370=_new_region("r");struct _RegionHandle*r=& _tmp370;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1569
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp371=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp372=_tmp371;struct Cyc_Absyn_Aggrdecl*_tmp374;struct Cyc_List_List*_tmp375;enum Cyc_Absyn_AggrKind _tmp377;struct Cyc_List_List*_tmp378;_LL19A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp372;if(_tmp373->tag != 11)goto _LL19C;else{if((((_tmp373->f1).aggr_info).KnownAggr).tag != 2)goto _LL19C;_tmp374=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp373->f1).aggr_info).KnownAggr).val);_tmp375=(_tmp373->f1).targs;}}_LL19B: {
# 1572
struct Cyc_Absyn_Aggrfield*_tmp379=Cyc_Absyn_lookup_decl_field(_tmp374,f);
if(_tmp379 == 0){
const char*_tmp8D5;void*_tmp8D4[2];struct Cyc_String_pa_PrintArg_struct _tmp8D3;struct Cyc_String_pa_PrintArg_struct _tmp8D2;void*_tmp37E=(_tmp8D2.tag=0,((_tmp8D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D3.tag=0,((_tmp8D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp374->name)),((_tmp8D4[0]=& _tmp8D3,((_tmp8D4[1]=& _tmp8D2,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D5="%s has no %s member",_tag_dyneither(_tmp8D5,sizeof(char),20))),_tag_dyneither(_tmp8D4,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp37E;}
# 1577
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp374->impl))->tagged)*is_tagged=1;{
void*t2=_tmp379->type;
if(_tmp375 != 0){
struct _RegionHandle _tmp37F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp37F;_push_region(rgn);
{struct Cyc_List_List*_tmp380=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp374->tvs,_tmp375);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp380,_tmp379->type);}
# 1581
;_pop_region(rgn);}
# 1587
if((((_tmp374->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp374->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp379->requires_clause == 0){
const char*_tmp8D9;void*_tmp8D8[1];struct Cyc_String_pa_PrintArg_struct _tmp8D7;void*_tmp384=(_tmp8D7.tag=0,((_tmp8D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8D8[0]=& _tmp8D7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8D9="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8D9,sizeof(char),56))),_tag_dyneither(_tmp8D8,sizeof(void*),1)))))));_npop_handler(0);return _tmp384;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp374->impl))->exist_vars != 0){
# 1592
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8DD;void*_tmp8DC[1];struct Cyc_String_pa_PrintArg_struct _tmp8DB;void*_tmp388=(_tmp8DB.tag=0,((_tmp8DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8DC[0]=& _tmp8DB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8DD="must use pattern-matching to access field %s\n\tdue to existential type variables.",_tag_dyneither(_tmp8DD,sizeof(char),81))),_tag_dyneither(_tmp8DC,sizeof(void*),1)))))));_npop_handler(0);return _tmp388;}}{
# 1595
void*_tmp389=t2;_npop_handler(0);return _tmp389;};};}_LL19C: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp372;if(_tmp376->tag != 12)goto _LL19E;else{_tmp377=_tmp376->f1;_tmp378=_tmp376->f2;}}_LL19D: {
# 1597
struct Cyc_Absyn_Aggrfield*_tmp38A=Cyc_Absyn_lookup_field(_tmp378,f);
if(_tmp38A == 0){
const char*_tmp8E1;void*_tmp8E0[1];struct Cyc_String_pa_PrintArg_struct _tmp8DF;void*_tmp38E=(_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E0[0]=& _tmp8DF,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E1="type has no %s member",_tag_dyneither(_tmp8E1,sizeof(char),22))),_tag_dyneither(_tmp8E0,sizeof(void*),1)))))));_npop_handler(0);return _tmp38E;}
# 1602
if(((_tmp377 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp38A->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp38A->requires_clause == 0){
# 1604
const char*_tmp8E5;void*_tmp8E4[1];struct Cyc_String_pa_PrintArg_struct _tmp8E3;void*_tmp392=(_tmp8E3.tag=0,((_tmp8E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8E4[0]=& _tmp8E3,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E5="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8E5,sizeof(char),56))),_tag_dyneither(_tmp8E4,sizeof(void*),1)))))));_npop_handler(0);return _tmp392;}{
void*_tmp393=_tmp38A->type;_npop_handler(0);return _tmp393;};}_LL19E:;_LL19F: {
# 1607
const char*_tmp8E9;void*_tmp8E8[1];struct Cyc_String_pa_PrintArg_struct _tmp8E7;void*_tmp397=(_tmp8E7.tag=0,((_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp8E8[0]=& _tmp8E7,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8E9="expecting struct or union, found %s",_tag_dyneither(_tmp8E9,sizeof(char),36))),_tag_dyneither(_tmp8E8,sizeof(void*),1)))))));_npop_handler(0);return _tmp397;}_LL199:;};
# 1565
;_pop_region(r);}
# 1613
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1616
struct _RegionHandle _tmp398=_new_region("r");struct _RegionHandle*r=& _tmp398;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1621
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp399=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp39A=_tmp399;void*_tmp39C;void*_tmp39D;union Cyc_Absyn_Constraint*_tmp39E;union Cyc_Absyn_Constraint*_tmp39F;_LL1A1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp39B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39A;if(_tmp39B->tag != 5)goto _LL1A3;else{_tmp39C=(_tmp39B->f1).elt_typ;_tmp39D=((_tmp39B->f1).ptr_atts).rgn;_tmp39E=((_tmp39B->f1).ptr_atts).bounds;_tmp39F=((_tmp39B->f1).ptr_atts).zero_term;}}_LL1A2:
# 1624
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp39E);
{void*_tmp3A0=Cyc_Tcutil_compress(_tmp39C);void*_tmp3A1=_tmp3A0;struct Cyc_Absyn_Aggrdecl*_tmp3A3;struct Cyc_List_List*_tmp3A4;enum Cyc_Absyn_AggrKind _tmp3A6;struct Cyc_List_List*_tmp3A7;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3A2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3A1;if(_tmp3A2->tag != 11)goto _LL1A8;else{if((((_tmp3A2->f1).aggr_info).KnownAggr).tag != 2)goto _LL1A8;_tmp3A3=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp3A2->f1).aggr_info).KnownAggr).val);_tmp3A4=(_tmp3A2->f1).targs;}}_LL1A7: {
# 1627
struct Cyc_Absyn_Aggrfield*_tmp3A8=Cyc_Absyn_lookup_decl_field(_tmp3A3,f);
if(_tmp3A8 == 0){
const char*_tmp8EE;void*_tmp8ED[2];struct Cyc_String_pa_PrintArg_struct _tmp8EC;struct Cyc_String_pa_PrintArg_struct _tmp8EB;void*_tmp3AD=(_tmp8EB.tag=0,((_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8EC.tag=0,((_tmp8EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp3A3->name)),((_tmp8ED[0]=& _tmp8EC,((_tmp8ED[1]=& _tmp8EB,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8EE="type %s has no %s member",_tag_dyneither(_tmp8EE,sizeof(char),25))),_tag_dyneither(_tmp8ED,sizeof(void*),2)))))))))))));_npop_handler(0);return _tmp3AD;}
# 1632
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A3->impl))->tagged)*is_tagged=1;{
void*t3=_tmp3A8->type;
if(_tmp3A4 != 0){
struct _RegionHandle _tmp3AE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3AE;_push_region(rgn);
{struct Cyc_List_List*_tmp3AF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp3A3->tvs,_tmp3A4);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp3AF,_tmp3A8->type);}
# 1636
;_pop_region(rgn);}{
# 1640
struct Cyc_Absyn_Kind*_tmp3B0=Cyc_Tcutil_typ_kind(t3);
# 1643
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp3B0) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp3B1=Cyc_Tcutil_compress(t3);void*_tmp3B2=_tmp3B1;_LL1AD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B2;if(_tmp3B3->tag != 8)goto _LL1AF;}_LL1AE:
 goto _LL1AC;_LL1AF:;_LL1B0: {
# 1647
const char*_tmp8F2;void*_tmp8F1[1];struct Cyc_String_pa_PrintArg_struct _tmp8F0;void*_tmp3B7=(_tmp8F0.tag=0,((_tmp8F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F1[0]=& _tmp8F0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F2="cannot get member %s since its type is abstract",_tag_dyneither(_tmp8F2,sizeof(char),48))),_tag_dyneither(_tmp8F1,sizeof(void*),1)))))));_npop_handler(0);return _tmp3B7;}_LL1AC:;}
# 1652
if((((_tmp3A3->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A3->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp3A8->requires_clause == 0){
# 1655
const char*_tmp8F6;void*_tmp8F5[1];struct Cyc_String_pa_PrintArg_struct _tmp8F4;void*_tmp3BB=(_tmp8F4.tag=0,((_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F5[0]=& _tmp8F4,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8F6="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp8F6,sizeof(char),56))),_tag_dyneither(_tmp8F5,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BB;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A3->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
const char*_tmp8FA;void*_tmp8F9[1];struct Cyc_String_pa_PrintArg_struct _tmp8F8;void*_tmp3BF=(_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8F9[0]=& _tmp8F8,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FA="must use pattern-matching to access field %s\n\tdue to extistential types",_tag_dyneither(_tmp8FA,sizeof(char),72))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))));_npop_handler(0);return _tmp3BF;}}{
# 1662
void*_tmp3C0=t3;_npop_handler(0);return _tmp3C0;};};};}_LL1A8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3A5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3A1;if(_tmp3A5->tag != 12)goto _LL1AA;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;}}_LL1A9: {
# 1664
struct Cyc_Absyn_Aggrfield*_tmp3C1=Cyc_Absyn_lookup_field(_tmp3A7,f);
if(_tmp3C1 == 0){
const char*_tmp8FE;void*_tmp8FD[1];struct Cyc_String_pa_PrintArg_struct _tmp8FC;void*_tmp3C5=(_tmp8FC.tag=0,((_tmp8FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp8FD[0]=& _tmp8FC,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp8FE="type has no %s field",_tag_dyneither(_tmp8FE,sizeof(char),21))),_tag_dyneither(_tmp8FD,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C5;}
# 1669
if((_tmp3A6 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp3C1->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
const char*_tmp902;void*_tmp901[1];struct Cyc_String_pa_PrintArg_struct _tmp900;void*_tmp3C9=(_tmp900.tag=0,((_tmp900.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmp901[0]=& _tmp900,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp902="cannot read union member %s since it is not `bits-only'",_tag_dyneither(_tmp902,sizeof(char),56))),_tag_dyneither(_tmp901,sizeof(void*),1)))))));_npop_handler(0);return _tmp3C9;}{
void*_tmp3CA=_tmp3C1->type;_npop_handler(0);return _tmp3CA;};}_LL1AA:;_LL1AB:
 goto _LL1A5;_LL1A5:;}
# 1674
goto _LL1A0;_LL1A3:;_LL1A4:
 goto _LL1A0;_LL1A0:;}{
# 1677
const char*_tmp906;void*_tmp905[1];struct Cyc_String_pa_PrintArg_struct _tmp904;void*_tmp3CE=(_tmp904.tag=0,((_tmp904.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp905[0]=& _tmp904,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp906="expecting struct or union pointer, found %s",_tag_dyneither(_tmp906,sizeof(char),44))),_tag_dyneither(_tmp905,sizeof(void*),1)))))));_npop_handler(0);return _tmp3CE;};
# 1616
;_pop_region(r);}
# 1682
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1684
struct _tuple15 _tmp3CF=Cyc_Evexp_eval_const_uint_exp(index);unsigned int _tmp3D1;int _tmp3D2;struct _tuple15 _tmp3D0=_tmp3CF;_tmp3D1=_tmp3D0.f1;_tmp3D2=_tmp3D0.f2;
if(!_tmp3D2){
const char*_tmp909;void*_tmp908;return(_tmp908=0,Cyc_Tcexp_expr_err(te,loc,0,((_tmp909="tuple projection cannot use sizeof or offsetof",_tag_dyneither(_tmp909,sizeof(char),47))),_tag_dyneither(_tmp908,sizeof(void*),0)));}{
# 1688
struct _handler_cons _tmp3D5;_push_handler(& _tmp3D5);{int _tmp3D7=0;if(setjmp(_tmp3D5.handler))_tmp3D7=1;if(!_tmp3D7){
{void*_tmp3D8=(*((struct _tuple18*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp3D1)).f2;_npop_handler(0);return _tmp3D8;};_pop_handler();}else{void*_tmp3D6=(void*)_exn_thrown;void*_tmp3DA=_tmp3D6;void*_tmp3DC;_LL1B2: {struct Cyc_List_Nth_exn_struct*_tmp3DB=(struct Cyc_List_Nth_exn_struct*)_tmp3DA;if(_tmp3DB->tag != Cyc_List_Nth)goto _LL1B4;}_LL1B3: {
# 1691
const char*_tmp90E;void*_tmp90D[2];struct Cyc_Int_pa_PrintArg_struct _tmp90C;struct Cyc_Int_pa_PrintArg_struct _tmp90B;return(_tmp90B.tag=1,((_tmp90B.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts),((_tmp90C.tag=1,((_tmp90C.f1=(unsigned long)((int)_tmp3D1),((_tmp90D[0]=& _tmp90C,((_tmp90D[1]=& _tmp90B,Cyc_Tcexp_expr_err(te,loc,0,((_tmp90E="index is %d but tuple has only %d fields",_tag_dyneither(_tmp90E,sizeof(char),41))),_tag_dyneither(_tmp90D,sizeof(void*),2)))))))))))));}_LL1B4: _tmp3DC=_tmp3DA;_LL1B5:(void)_throw(_tmp3DC);_LL1B1:;}};};}
# 1696
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1698
struct _RegionHandle _tmp3E1=_new_region("r");struct _RegionHandle*r=& _tmp3E1;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp3E2=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3E2),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp3E2),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp3E2,e2)){
const char*_tmp912;void*_tmp911[1];struct Cyc_String_pa_PrintArg_struct _tmp910;void*_tmp3E6=(_tmp910.tag=0,((_tmp910.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp911[0]=& _tmp910,Cyc_Tcexp_expr_err(_tmp3E2,e2->loc,topt,((_tmp912="expecting int subscript, found %s",_tag_dyneither(_tmp912,sizeof(char),34))),_tag_dyneither(_tmp911,sizeof(void*),1)))))));_npop_handler(0);return _tmp3E6;}{
# 1709
void*_tmp3E7=t1;void*_tmp3E9;struct Cyc_Absyn_Tqual _tmp3EA;void*_tmp3EB;union Cyc_Absyn_Constraint*_tmp3EC;union Cyc_Absyn_Constraint*_tmp3ED;struct Cyc_List_List*_tmp3EF;_LL1B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp3E8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E7;if(_tmp3E8->tag != 5)goto _LL1B9;else{_tmp3E9=(_tmp3E8->f1).elt_typ;_tmp3EA=(_tmp3E8->f1).elt_tq;_tmp3EB=((_tmp3E8->f1).ptr_atts).rgn;_tmp3EC=((_tmp3E8->f1).ptr_atts).bounds;_tmp3ED=((_tmp3E8->f1).ptr_atts).zero_term;}}_LL1B8:
# 1713
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3ED)){
int emit_warning=1;
{void*_tmp3F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp3EC);void*_tmp3F1=_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F3;_LL1BE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3F2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F1;if(_tmp3F2->tag != 1)goto _LL1C0;else{_tmp3F3=_tmp3F2->f1;}}_LL1BF:
# 1717
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp3F5=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp3F7;int _tmp3F8;struct _tuple15 _tmp3F6=_tmp3F5;_tmp3F7=_tmp3F6.f1;_tmp3F8=_tmp3F6.f2;
if(_tmp3F8){
struct _tuple15 _tmp3F9=Cyc_Evexp_eval_const_uint_exp(_tmp3F3);unsigned int _tmp3FB;int _tmp3FC;struct _tuple15 _tmp3FA=_tmp3F9;_tmp3FB=_tmp3FA.f1;_tmp3FC=_tmp3FA.f2;
if(_tmp3FC  && _tmp3FB > _tmp3F7)emit_warning=0;}}
# 1724
goto _LL1BD;_LL1C0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp3F4=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3F1;if(_tmp3F4->tag != 0)goto _LL1BD;}_LL1C1:
 emit_warning=0;goto _LL1BD;_LL1BD:;}
# 1727
if(emit_warning){
const char*_tmp915;void*_tmp914;(_tmp914=0,Cyc_Tcutil_warn(e2->loc,((_tmp915="subscript on thin, zero-terminated pointer could be expensive.",_tag_dyneither(_tmp915,sizeof(char),63))),_tag_dyneither(_tmp914,sizeof(void*),0)));}}else{
# 1731
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp3FF=Cyc_Evexp_eval_const_uint_exp(e2);unsigned int _tmp401;int _tmp402;struct _tuple15 _tmp400=_tmp3FF;_tmp401=_tmp400.f1;_tmp402=_tmp400.f2;
if(_tmp402)
Cyc_Tcutil_check_bound(loc,_tmp401,_tmp3EC);}else{
# 1737
if(Cyc_Tcutil_is_bound_one(_tmp3EC) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3ED)){
const char*_tmp918;void*_tmp917;(_tmp917=0,Cyc_Tcutil_warn(e1->loc,((_tmp918="subscript applied to pointer to one object",_tag_dyneither(_tmp918,sizeof(char),43))),_tag_dyneither(_tmp917,sizeof(void*),0)));}
Cyc_Tcutil_check_nonzero_bound(loc,_tmp3EC);}}
# 1742
Cyc_Tcenv_check_rgn_accessible(_tmp3E2,loc,_tmp3EB);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp3E9),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp3E2)){
const char*_tmp91B;void*_tmp91A;(_tmp91A=0,Cyc_Tcutil_terr(e1->loc,((_tmp91B="can't subscript an abstract pointer",_tag_dyneither(_tmp91B,sizeof(char),36))),_tag_dyneither(_tmp91A,sizeof(void*),0)));}{
void*_tmp407=_tmp3E9;_npop_handler(0);return _tmp407;};_LL1B9: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3EE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E7;if(_tmp3EE->tag != 10)goto _LL1BB;else{_tmp3EF=_tmp3EE->f1;}}_LL1BA: {
void*_tmp408=Cyc_Tcexp_ithTupleType(_tmp3E2,loc,_tmp3EF,e2);_npop_handler(0);return _tmp408;}_LL1BB:;_LL1BC: {
const char*_tmp91F;void*_tmp91E[1];struct Cyc_String_pa_PrintArg_struct _tmp91D;void*_tmp40C=(_tmp91D.tag=0,((_tmp91D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp91E[0]=& _tmp91D,Cyc_Tcexp_expr_err(_tmp3E2,loc,topt,((_tmp91F="subscript applied to %s",_tag_dyneither(_tmp91F,sizeof(char),24))),_tag_dyneither(_tmp91E,sizeof(void*),1)))))));_npop_handler(0);return _tmp40C;}_LL1B6:;};};}
# 1699
;_pop_region(r);}
# 1753
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp40D=Cyc_Tcutil_compress(*topt);void*_tmp40E=_tmp40D;struct Cyc_List_List*_tmp410;_LL1C3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp40F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40E;if(_tmp40F->tag != 10)goto _LL1C5;else{_tmp410=_tmp40F->f1;}}_LL1C4:
# 1759
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp410)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1762
goto _LL1C2;
# 1764
for(0;es != 0;(es=es->tl,_tmp410=_tmp410->tl)){
struct _RegionHandle _tmp411=_new_region("r");struct _RegionHandle*r=& _tmp411;_push_region(r);
{int bogus=0;
void*_tmp412=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp410))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp412,(struct Cyc_Absyn_Exp*)es->hd);
# 1770
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple18*)_tmp410->hd)).f2,& bogus);{
struct _tuple18*_tmp922;struct Cyc_List_List*_tmp921;fields=((_tmp921=_cycalloc(sizeof(*_tmp921)),((_tmp921->hd=((_tmp922=_cycalloc(sizeof(*_tmp922)),((_tmp922->f1=(*((struct _tuple18*)_tmp410->hd)).f1,((_tmp922->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp922)))))),((_tmp921->tl=fields,_tmp921))))));};}
# 1766
;_pop_region(r);}
# 1774
done=1;
goto _LL1C2;_LL1C5:;_LL1C6:
 goto _LL1C2;_LL1C2:;}
# 1778
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp415=_new_region("r");struct _RegionHandle*r=& _tmp415;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);{
struct _tuple18*_tmp925;struct Cyc_List_List*_tmp924;fields=((_tmp924=_cycalloc(sizeof(*_tmp924)),((_tmp924->hd=((_tmp925=_cycalloc(sizeof(*_tmp925)),((_tmp925->f1=Cyc_Absyn_empty_tqual(0),((_tmp925->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt),_tmp925)))))),((_tmp924->tl=fields,_tmp924))))));};
# 1781
;_pop_region(r);}{
# 1785
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp928;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp927;return(void*)((_tmp927=_cycalloc(sizeof(*_tmp927)),((_tmp927[0]=((_tmp928.tag=10,((_tmp928.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmp928)))),_tmp927))));};}
# 1789
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1792
const char*_tmp92B;void*_tmp92A;return(_tmp92A=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp92B="tcCompoundLit",_tag_dyneither(_tmp92B,sizeof(char),14))),_tag_dyneither(_tmp92A,sizeof(void*),0)));}
# 1802 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1805
void*res_t2;
struct _RegionHandle _tmp41C=_new_region("r");struct _RegionHandle*r=& _tmp41C;_push_region(r);{
int _tmp41D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp92E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp92D;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp41E=(_tmp92D=_cycalloc(sizeof(*_tmp92D)),((_tmp92D[0]=((_tmp92E.tag=0,((_tmp92E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp41D),_tmp92E)))),_tmp92D)));
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp41E,loc);
# 1814
if(zero_term){
struct Cyc_Absyn_Exp*_tmp41F=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp41D - 1);
if(!Cyc_Tcutil_is_zero(_tmp41F)){
const char*_tmp931;void*_tmp930;(_tmp930=0,Cyc_Tcutil_terr(_tmp41F->loc,((_tmp931="zero-terminated array doesn't end with zero.",_tag_dyneither(_tmp931,sizeof(char),45))),_tag_dyneither(_tmp930,sizeof(void*),0)));}}
# 1819
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1824
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1827
if(!Cyc_Tcutil_coerce_list(te,res,es)){
# 1829
const char*_tmp935;void*_tmp934[1];struct Cyc_String_pa_PrintArg_struct _tmp933;(_tmp933.tag=0,((_tmp933.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res)),((_tmp934[0]=& _tmp933,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,((_tmp935="elements of array do not all have the same type (%s)",_tag_dyneither(_tmp935,sizeof(char),53))),_tag_dyneither(_tmp934,sizeof(void*),1)))))));}}
# 1833
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple17*)des->hd)).f1;
if(ds != 0){
# 1838
void*_tmp427=(void*)ds->hd;void*_tmp428=_tmp427;struct Cyc_Absyn_Exp*_tmp42B;_LL1C8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp429=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp428;if(_tmp429->tag != 1)goto _LL1CA;}_LL1C9:
# 1840
{const char*_tmp938;void*_tmp937;(_tmp937=0,Cyc_Tcutil_terr(loc,((_tmp938="only array index designators are supported",_tag_dyneither(_tmp938,sizeof(char),43))),_tag_dyneither(_tmp937,sizeof(void*),0)));}
goto _LL1C7;_LL1CA: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp42A=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp428;if(_tmp42A->tag != 0)goto _LL1C7;else{_tmp42B=_tmp42A->f1;}}_LL1CB:
# 1843
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp42B);{
struct _tuple15 _tmp42E=Cyc_Evexp_eval_const_uint_exp(_tmp42B);unsigned int _tmp430;int _tmp431;struct _tuple15 _tmp42F=_tmp42E;_tmp430=_tmp42F.f1;_tmp431=_tmp42F.f2;
if(!_tmp431){
const char*_tmp93B;void*_tmp93A;(_tmp93A=0,Cyc_Tcutil_terr(_tmp42B->loc,((_tmp93B="index designator cannot use sizeof or offsetof",_tag_dyneither(_tmp93B,sizeof(char),47))),_tag_dyneither(_tmp93A,sizeof(void*),0)));}else{
if(_tmp430 != offset){
const char*_tmp940;void*_tmp93F[2];struct Cyc_Int_pa_PrintArg_struct _tmp93E;struct Cyc_Int_pa_PrintArg_struct _tmp93D;(_tmp93D.tag=1,((_tmp93D.f1=(unsigned long)((int)_tmp430),((_tmp93E.tag=1,((_tmp93E.f1=(unsigned long)offset,((_tmp93F[0]=& _tmp93E,((_tmp93F[1]=& _tmp93D,Cyc_Tcutil_terr(_tmp42B->loc,((_tmp940="expecting index designator %d but found %d",_tag_dyneither(_tmp940,sizeof(char),43))),_tag_dyneither(_tmp93F,sizeof(void*),2)))))))))))));}}
# 1850
goto _LL1C7;};_LL1C7:;}}}{
# 1854
void*_tmp438=res_t2;_npop_handler(0);return _tmp438;};
# 1806
;_pop_region(r);}
# 1858
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1861
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp439=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp43A=_tmp439;void*_tmp43C;_LL1CD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp43B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43A;if(_tmp43B->tag != 19)goto _LL1CF;else{_tmp43C=(void*)_tmp43B->f1;}}_LL1CE:
# 1866
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp43D=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp43C,0),0,Cyc_Absyn_No_coercion,0);
_tmp43D->topt=bound->topt;
# 1871
bound=_tmp43D;}
# 1873
goto _LL1CC;_LL1CF:;_LL1D0:
# 1875
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp944;void*_tmp943[1];struct Cyc_String_pa_PrintArg_struct _tmp942;(_tmp942.tag=0,((_tmp942.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp943[0]=& _tmp942,Cyc_Tcutil_terr(bound->loc,((_tmp944="expecting unsigned int, found %s",_tag_dyneither(_tmp944,sizeof(char),33))),_tag_dyneither(_tmp943,sizeof(void*),1)))))));}_LL1CC:;}
# 1880
if(!(vd->tq).real_const){
const char*_tmp947;void*_tmp946;(_tmp946=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp947="comprehension index variable is not declared const!",_tag_dyneither(_tmp947,sizeof(char),52))),_tag_dyneither(_tmp946,sizeof(void*),0)));}{
# 1883
struct _RegionHandle _tmp443=_new_region("r");struct _RegionHandle*r=& _tmp443;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0){
te2=Cyc_Tcenv_new_block(r,loc,te2);
te2=Cyc_Tcenv_add_local_var(r,loc,te2,vd);}{
# 1889
struct Cyc_Tcenv_Tenv*_tmp444=te2;
void**_tmp445=0;
struct Cyc_Absyn_Tqual*_tmp446=0;
union Cyc_Absyn_Constraint**_tmp447=0;
# 1894
if(topt != 0){
void*_tmp448=Cyc_Tcutil_compress(*topt);void*_tmp449=_tmp448;struct Cyc_Absyn_PtrInfo _tmp44B;void*_tmp44D;struct Cyc_Absyn_Tqual _tmp44E;struct Cyc_Absyn_Exp*_tmp44F;union Cyc_Absyn_Constraint*_tmp450;_LL1D2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp449;if(_tmp44A->tag != 5)goto _LL1D4;else{_tmp44B=_tmp44A->f1;}}_LL1D3:
# 1897
{void**_tmp948;_tmp445=((_tmp948=_region_malloc(r,sizeof(*_tmp948)),((_tmp948[0]=_tmp44B.elt_typ,_tmp948))));}
{struct Cyc_Absyn_Tqual*_tmp949;_tmp446=((_tmp949=_region_malloc(r,sizeof(*_tmp949)),((_tmp949[0]=_tmp44B.elt_tq,_tmp949))));}
{union Cyc_Absyn_Constraint**_tmp94A;_tmp447=((_tmp94A=_region_malloc(r,sizeof(*_tmp94A)),((_tmp94A[0]=(_tmp44B.ptr_atts).zero_term,_tmp94A))));}
goto _LL1D1;_LL1D4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp449;if(_tmp44C->tag != 8)goto _LL1D6;else{_tmp44D=(_tmp44C->f1).elt_type;_tmp44E=(_tmp44C->f1).tq;_tmp44F=(_tmp44C->f1).num_elts;_tmp450=(_tmp44C->f1).zero_term;}}_LL1D5:
# 1902
{void**_tmp94B;_tmp445=((_tmp94B=_region_malloc(r,sizeof(*_tmp94B)),((_tmp94B[0]=_tmp44D,_tmp94B))));}
{struct Cyc_Absyn_Tqual*_tmp94C;_tmp446=((_tmp94C=_region_malloc(r,sizeof(*_tmp94C)),((_tmp94C[0]=_tmp44E,_tmp94C))));}
{union Cyc_Absyn_Constraint**_tmp94D;_tmp447=((_tmp94D=_region_malloc(r,sizeof(*_tmp94D)),((_tmp94D[0]=_tmp450,_tmp94D))));}
goto _LL1D1;_LL1D6:;_LL1D7:
# 1907
 goto _LL1D1;_LL1D1:;}{
# 1910
void*t=Cyc_Tcexp_tcExp(_tmp444,_tmp445,body);
# 1912
struct _RegionHandle _tmp457=_new_region("temp");struct _RegionHandle*temp=& _tmp457;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body)){
const char*_tmp950;void*_tmp94F;(_tmp94F=0,Cyc_Tcutil_terr(body->loc,((_tmp950="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp950,sizeof(char),49))),_tag_dyneither(_tmp94F,sizeof(void*),0)));}
# 1916
if(_tmp444->le == 0){
# 1918
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp953;void*_tmp952;(_tmp952=0,Cyc_Tcutil_terr(bound->loc,((_tmp953="bound is not constant",_tag_dyneither(_tmp953,sizeof(char),22))),_tag_dyneither(_tmp952,sizeof(void*),0)));}
if(!Cyc_Tcutil_is_const_exp(body)){
const char*_tmp956;void*_tmp955;(_tmp955=0,Cyc_Tcutil_terr(bound->loc,((_tmp956="body is not constant",_tag_dyneither(_tmp956,sizeof(char),21))),_tag_dyneither(_tmp955,sizeof(void*),0)));}}
# 1923
if(_tmp447 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp447)){
# 1926
struct Cyc_Absyn_Exp*_tmp45E=Cyc_Absyn_uint_exp(1,0);_tmp45E->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp45E,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1930
struct _RegionHandle _tmp45F=_new_region("r");struct _RegionHandle*r=& _tmp45F;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body)){
const char*_tmp959;void*_tmp958;(_tmp958=0,Cyc_Tcutil_terr(body->loc,((_tmp959="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp959,sizeof(char),49))),_tag_dyneither(_tmp958,sizeof(void*),0)));}{
# 1936
void*_tmp462=Cyc_Absyn_array_typ(t,_tmp446 == 0?Cyc_Absyn_empty_tqual(0):*_tmp446,bound,
_tmp447 == 0?Cyc_Absyn_false_conref:*_tmp447,0);
void*_tmp463=_tmp462;_npop_handler(2);return _tmp463;};
# 1930
;_pop_region(r);};
# 1912
;_pop_region(temp);};};}
# 1884
;_pop_region(r);};}
# 1943
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1946
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp464=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp465=_tmp464;void*_tmp467;_LL1D9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp465;if(_tmp466->tag != 19)goto _LL1DB;else{_tmp467=(void*)_tmp466->f1;}}_LL1DA:
# 1951
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp468=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp467,0),0,Cyc_Absyn_No_coercion,0);
_tmp468->topt=bound->topt;
# 1956
bound=_tmp468;}
# 1958
goto _LL1D8;_LL1DB:;_LL1DC:
# 1960
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound)){
const char*_tmp95D;void*_tmp95C[1];struct Cyc_String_pa_PrintArg_struct _tmp95B;(_tmp95B.tag=0,((_tmp95B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt))),((_tmp95C[0]=& _tmp95B,Cyc_Tcutil_terr(bound->loc,((_tmp95D="expecting unsigned int, found %s",_tag_dyneither(_tmp95D,sizeof(char),33))),_tag_dyneither(_tmp95C,sizeof(void*),1)))))));}_LL1D8:;}{
# 1965
struct _RegionHandle _tmp46C=_new_region("r");struct _RegionHandle*r=& _tmp46C;_push_region(r);
{void**_tmp46D=0;
struct Cyc_Absyn_Tqual*_tmp46E=0;
union Cyc_Absyn_Constraint**_tmp46F=0;
# 1970
if(topt != 0){
void*_tmp470=Cyc_Tcutil_compress(*topt);void*_tmp471=_tmp470;struct Cyc_Absyn_PtrInfo _tmp473;void*_tmp475;struct Cyc_Absyn_Tqual _tmp476;struct Cyc_Absyn_Exp*_tmp477;union Cyc_Absyn_Constraint*_tmp478;_LL1DE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp472=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp471;if(_tmp472->tag != 5)goto _LL1E0;else{_tmp473=_tmp472->f1;}}_LL1DF:
# 1973
{void**_tmp95E;_tmp46D=((_tmp95E=_region_malloc(r,sizeof(*_tmp95E)),((_tmp95E[0]=_tmp473.elt_typ,_tmp95E))));}
{struct Cyc_Absyn_Tqual*_tmp95F;_tmp46E=((_tmp95F=_region_malloc(r,sizeof(*_tmp95F)),((_tmp95F[0]=_tmp473.elt_tq,_tmp95F))));}
{union Cyc_Absyn_Constraint**_tmp960;_tmp46F=((_tmp960=_region_malloc(r,sizeof(*_tmp960)),((_tmp960[0]=(_tmp473.ptr_atts).zero_term,_tmp960))));}
goto _LL1DD;_LL1E0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp471;if(_tmp474->tag != 8)goto _LL1E2;else{_tmp475=(_tmp474->f1).elt_type;_tmp476=(_tmp474->f1).tq;_tmp477=(_tmp474->f1).num_elts;_tmp478=(_tmp474->f1).zero_term;}}_LL1E1:
# 1978
{void**_tmp961;_tmp46D=((_tmp961=_region_malloc(r,sizeof(*_tmp961)),((_tmp961[0]=_tmp475,_tmp961))));}
{struct Cyc_Absyn_Tqual*_tmp962;_tmp46E=((_tmp962=_region_malloc(r,sizeof(*_tmp962)),((_tmp962[0]=_tmp476,_tmp962))));}
{union Cyc_Absyn_Constraint**_tmp963;_tmp46F=((_tmp963=_region_malloc(r,sizeof(*_tmp963)),((_tmp963[0]=_tmp478,_tmp963))));}
goto _LL1DD;_LL1E2:;_LL1E3:
# 1983
 goto _LL1DD;_LL1DD:;}
# 1986
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,t);
if(_tmp46D != 0)Cyc_Tcutil_unify(*_tmp46D,t);
# 1989
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound)){
const char*_tmp966;void*_tmp965;(_tmp965=0,Cyc_Tcutil_terr(bound->loc,((_tmp966="bound is not constant",_tag_dyneither(_tmp966,sizeof(char),22))),_tag_dyneither(_tmp965,sizeof(void*),0)));}}
# 1993
if(_tmp46F != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp46F)){
# 1996
struct Cyc_Absyn_Exp*_tmp481=Cyc_Absyn_uint_exp(1,0);_tmp481->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp481,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;{
# 2000
const char*_tmp969;void*_tmp968;(_tmp968=0,Cyc_Tcutil_terr(loc,((_tmp969="non-initializing comprehensions do not currently support @zeroterm arrays",_tag_dyneither(_tmp969,sizeof(char),74))),_tag_dyneither(_tmp968,sizeof(void*),0)));};}{
# 2003
void*_tmp484=Cyc_Absyn_array_typ(t,_tmp46E == 0?Cyc_Absyn_empty_tqual(0):*_tmp46E,bound,
_tmp46F == 0?Cyc_Absyn_false_conref:*_tmp46F,0);
void*_tmp485=_tmp484;_npop_handler(0);return _tmp485;};}
# 1966
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 2019 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 2023
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);{
struct Cyc_Absyn_Aggrdecl**_tmp96A;adptr=((_tmp96A=_cycalloc(sizeof(*_tmp96A)),((_tmp96A[0]=ad,_tmp96A))));};}else{
# 2029
{struct _handler_cons _tmp487;_push_handler(& _tmp487);{int _tmp489=0;if(setjmp(_tmp487.handler))_tmp489=1;if(!_tmp489){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 2029
;_pop_handler();}else{void*_tmp488=(void*)_exn_thrown;void*_tmp48B=_tmp488;void*_tmp48D;_LL1E5: {struct Cyc_Dict_Absent_exn_struct*_tmp48C=(struct Cyc_Dict_Absent_exn_struct*)_tmp48B;if(_tmp48C->tag != Cyc_Dict_Absent)goto _LL1E7;}_LL1E6:
# 2032
{const char*_tmp96E;void*_tmp96D[1];struct Cyc_String_pa_PrintArg_struct _tmp96C;(_tmp96C.tag=0,((_tmp96C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp96D[0]=& _tmp96C,Cyc_Tcutil_terr(loc,((_tmp96E="unbound struct/union name %s",_tag_dyneither(_tmp96E,sizeof(char),29))),_tag_dyneither(_tmp96D,sizeof(void*),1)))))));}
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;_LL1E7: _tmp48D=_tmp48B;_LL1E8:(void)_throw(_tmp48D);_LL1E4:;}};}
# 2035
*ad_opt=ad;
*tn=ad->name;}
# 2038
if(ad->impl == 0){
{const char*_tmp976;void*_tmp975[1];const char*_tmp974;const char*_tmp973;struct Cyc_String_pa_PrintArg_struct _tmp972;(_tmp972.tag=0,((_tmp972.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?(_tmp973="struct",_tag_dyneither(_tmp973,sizeof(char),7)):((_tmp974="union",_tag_dyneither(_tmp974,sizeof(char),6)))),((_tmp975[0]=& _tmp972,Cyc_Tcutil_terr(loc,((_tmp976="can't construct abstract %s",_tag_dyneither(_tmp976,sizeof(char),28))),_tag_dyneither(_tmp975,sizeof(void*),1)))))));}
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 2042
struct _RegionHandle _tmp496=_new_region("rgn");struct _RegionHandle*rgn=& _tmp496;_push_region(rgn);
# 2047
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp497=Cyc_Tcenv_new_status(te);
if(_tmp497 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 2056
te2=Cyc_Tcenv_set_new_status(rgn,_tmp497,te);}{
# 2058
struct _tuple12 _tmp977;struct _tuple12 _tmp498=(_tmp977.f1=Cyc_Tcenv_lookup_type_vars(te2),((_tmp977.f2=rgn,_tmp977)));
struct Cyc_List_List*_tmp499=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp498,ad->tvs);
struct Cyc_List_List*_tmp49A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp498,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp49B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp499);
struct Cyc_List_List*_tmp49C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp49A);
struct Cyc_List_List*_tmp49D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp499,_tmp49A);
void*res_typ;
# 2069
if(topt != 0){
void*_tmp49E=Cyc_Tcutil_compress(*topt);void*_tmp49F=_tmp49E;struct Cyc_Absyn_Aggrdecl**_tmp4A1;struct Cyc_List_List*_tmp4A2;_LL1EA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp49F;if(_tmp4A0->tag != 11)goto _LL1EC;else{if((((_tmp4A0->f1).aggr_info).KnownAggr).tag != 2)goto _LL1EC;_tmp4A1=(struct Cyc_Absyn_Aggrdecl**)(((_tmp4A0->f1).aggr_info).KnownAggr).val;_tmp4A2=(_tmp4A0->f1).targs;}}_LL1EB:
# 2072
 if(*_tmp4A1 == *adptr){
{struct Cyc_List_List*_tmp4A3=_tmp49B;for(0;_tmp4A3 != 0  && _tmp4A2 != 0;
(_tmp4A3=_tmp4A3->tl,_tmp4A2=_tmp4A2->tl)){
Cyc_Tcutil_unify((void*)_tmp4A3->hd,(void*)_tmp4A2->hd);}}
# 2077
res_typ=*topt;
goto _LL1E9;}
# 2080
goto _LL1ED;_LL1EC:;_LL1ED: {
# 2082
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp97D;struct Cyc_Absyn_AggrInfo _tmp97C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp97B;res_typ=(void*)((_tmp97B=_cycalloc(sizeof(*_tmp97B)),((_tmp97B[0]=((_tmp97D.tag=11,((_tmp97D.f1=((_tmp97C.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp97C.targs=_tmp49B,_tmp97C)))),_tmp97D)))),_tmp97B))));}_LL1E9:;}else{
# 2085
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp983;struct Cyc_Absyn_AggrInfo _tmp982;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp981;res_typ=(void*)((_tmp981=_cycalloc(sizeof(*_tmp981)),((_tmp981[0]=((_tmp983.tag=11,((_tmp983.f1=((_tmp982.aggr_info=Cyc_Absyn_KnownAggr(adptr),((_tmp982.targs=_tmp49B,_tmp982)))),_tmp983)))),_tmp981))));}{
# 2088
struct Cyc_List_List*_tmp4AA=*ts;
struct Cyc_List_List*_tmp4AB=_tmp49C;
while(_tmp4AA != 0  && _tmp4AB != 0){
# 2092
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,(void*)_tmp4AA->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp4AA->hd);
Cyc_Tcutil_unify((void*)_tmp4AA->hd,(void*)_tmp4AB->hd);
_tmp4AA=_tmp4AA->tl;
_tmp4AB=_tmp4AB->tl;}
# 2098
if(_tmp4AA != 0){
const char*_tmp986;void*_tmp985;(_tmp985=0,Cyc_Tcutil_terr(loc,((_tmp986="too many explicit witness types",_tag_dyneither(_tmp986,sizeof(char),32))),_tag_dyneither(_tmp985,sizeof(void*),0)));}
# 2101
*ts=_tmp49C;{
# 2104
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4AE=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4B0;struct Cyc_Absyn_Exp*_tmp4B1;struct _tuple19*_tmp4AF=_tmp4AE;_tmp4B0=_tmp4AF->f1;_tmp4B1=_tmp4AF->f2;{
void*_tmp4B2=Cyc_Tcutil_rsubstitute(rgn,_tmp49D,_tmp4B0->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp4B2,_tmp4B1);
# 2115
if(!Cyc_Tcutil_coerce_arg(te2,_tmp4B1,_tmp4B2,& bogus)){
{const char*_tmp992;void*_tmp991[5];struct Cyc_String_pa_PrintArg_struct _tmp990;const char*_tmp98F;const char*_tmp98E;struct Cyc_String_pa_PrintArg_struct _tmp98D;struct Cyc_String_pa_PrintArg_struct _tmp98C;struct Cyc_String_pa_PrintArg_struct _tmp98B;struct Cyc_String_pa_PrintArg_struct _tmp98A;(_tmp98A.tag=0,((_tmp98A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2119
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4B1->topt))),((_tmp98B.tag=0,((_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2118
Cyc_Absynpp_typ2string(_tmp4B2)),((_tmp98C.tag=0,((_tmp98C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn)),((_tmp98D.tag=0,((_tmp98D.f1=(struct _dyneither_ptr)(
# 2117
ad->kind == Cyc_Absyn_StructA?(_tmp98E="struct",_tag_dyneither(_tmp98E,sizeof(char),7)):((_tmp98F="union",_tag_dyneither(_tmp98F,sizeof(char),6)))),((_tmp990.tag=0,((_tmp990.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4B0->name),((_tmp991[0]=& _tmp990,((_tmp991[1]=& _tmp98D,((_tmp991[2]=& _tmp98C,((_tmp991[3]=& _tmp98B,((_tmp991[4]=& _tmp98A,Cyc_Tcutil_terr(_tmp4B1->loc,((_tmp992="field %s of %s %s expects type %s != %s",_tag_dyneither(_tmp992,sizeof(char),40))),_tag_dyneither(_tmp991,sizeof(void*),5)))))))))))))))))))))))))))))));}
# 2120
Cyc_Tcutil_explain_failure();}};}{
# 2124
struct Cyc_List_List*_tmp4BC=0;
{struct Cyc_List_List*_tmp4BD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp4BD != 0;_tmp4BD=_tmp4BD->tl){
struct _tuple0*_tmp995;struct Cyc_List_List*_tmp994;_tmp4BC=((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994->hd=((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp49D,(*((struct _tuple0*)_tmp4BD->hd)).f1),((_tmp995->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp49D,(*((struct _tuple0*)_tmp4BD->hd)).f2),_tmp995)))))),((_tmp994->tl=_tmp4BC,_tmp994))))));}}
# 2129
_tmp4BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4BC);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp4BC);{
void*_tmp4C0=res_typ;_npop_handler(0);return _tmp4C0;};};};};};}
# 2047
;_pop_region(rgn);};}
# 2136
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2138
struct _RegionHandle _tmp4C2=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4C2;_push_region(rgn);{
void*_tmp4C3=Cyc_Tcutil_compress(ts);void*_tmp4C4=_tmp4C3;enum Cyc_Absyn_AggrKind _tmp4C6;struct Cyc_List_List*_tmp4C7;_LL1EF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C4;if(_tmp4C5->tag != 12)goto _LL1F1;else{_tmp4C6=_tmp4C5->f1;_tmp4C7=_tmp4C5->f2;}}_LL1F0:
# 2141
 if(_tmp4C6 == Cyc_Absyn_UnionA){
const char*_tmp998;void*_tmp997;(_tmp997=0,Cyc_Tcutil_terr(loc,((_tmp998="expecting struct but found union",_tag_dyneither(_tmp998,sizeof(char),33))),_tag_dyneither(_tmp997,sizeof(void*),0)));}{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp4C7);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp4CA=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CD;struct _tuple19*_tmp4CB=_tmp4CA;_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp4CC->type,_tmp4CD);
# 2150
if(!Cyc_Tcutil_coerce_arg(te,_tmp4CD,_tmp4CC->type,& bogus)){
{const char*_tmp99E;void*_tmp99D[3];struct Cyc_String_pa_PrintArg_struct _tmp99C;struct Cyc_String_pa_PrintArg_struct _tmp99B;struct Cyc_String_pa_PrintArg_struct _tmp99A;(_tmp99A.tag=0,((_tmp99A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2153
Cyc_Absynpp_typ2string((void*)_check_null(_tmp4CD->topt))),((_tmp99B.tag=0,((_tmp99B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2152
Cyc_Absynpp_typ2string(_tmp4CC->type)),((_tmp99C.tag=0,((_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4CC->name),((_tmp99D[0]=& _tmp99C,((_tmp99D[1]=& _tmp99B,((_tmp99D[2]=& _tmp99A,Cyc_Tcutil_terr(_tmp4CD->loc,((_tmp99E="field %s of struct expects type %s != %s",_tag_dyneither(_tmp99E,sizeof(char),41))),_tag_dyneither(_tmp99D,sizeof(void*),3)))))))))))))))))));}
# 2154
Cyc_Tcutil_explain_failure();}}
# 2157
goto _LL1EE;};_LL1F1:;_LL1F2: {
const char*_tmp9A1;void*_tmp9A0;(_tmp9A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9A1="tcAnonStruct: wrong type",_tag_dyneither(_tmp9A1,sizeof(char),25))),_tag_dyneither(_tmp9A0,sizeof(void*),0)));}_LL1EE:;}{
# 2161
void*_tmp4D5=ts;_npop_handler(0);return _tmp4D5;};
# 2138
;_pop_region(rgn);}
# 2165
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2168
struct _RegionHandle _tmp4D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4D6;_push_region(rgn);
{struct _tuple12 _tmp9A2;struct _tuple12 _tmp4D7=(_tmp9A2.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmp9A2.f2=rgn,_tmp9A2)));
struct Cyc_List_List*_tmp4D8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4D7,tud->tvs);
struct Cyc_List_List*_tmp4D9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp4D8);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp9A8;struct Cyc_Absyn_DatatypeFieldInfo _tmp9A7;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9A6;void*res=(void*)(
(_tmp9A6=_cycalloc(sizeof(*_tmp9A6)),((_tmp9A6[0]=((_tmp9A8.tag=4,((_tmp9A8.f1=((_tmp9A7.field_info=Cyc_Absyn_KnownDatatypefield(tud,tuf),((_tmp9A7.targs=_tmp4D9,_tmp9A7)))),_tmp9A8)))),_tmp9A6))));
# 2176
if(topt != 0){
void*_tmp4DA=Cyc_Tcutil_compress(*topt);void*_tmp4DB=_tmp4DA;_LL1F4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4DC=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4DB;if(_tmp4DC->tag != 4)goto _LL1F6;}_LL1F5:
 Cyc_Tcutil_unify(*topt,res);goto _LL1F3;_LL1F6:;_LL1F7:
 goto _LL1F3;_LL1F3:;}{
# 2182
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple18*)ts->hd)).f2;
if(_tmp4D8 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp4D8,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp9B0;void*_tmp9AF[3];struct Cyc_String_pa_PrintArg_struct _tmp9AE;struct Cyc_String_pa_PrintArg_struct _tmp9AD;const char*_tmp9AC;struct Cyc_String_pa_PrintArg_struct _tmp9AB;(_tmp9AB.tag=0,((_tmp9AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2193
e->topt == 0?(struct _dyneither_ptr)((_tmp9AC="?",_tag_dyneither(_tmp9AC,sizeof(char),2))): Cyc_Absynpp_typ2string((void*)_check_null(e->topt)))),((_tmp9AD.tag=0,((_tmp9AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2192
Cyc_Absynpp_typ2string(t)),((_tmp9AE.tag=0,((_tmp9AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9AF[0]=& _tmp9AE,((_tmp9AF[1]=& _tmp9AD,((_tmp9AF[2]=& _tmp9AB,Cyc_Tcutil_terr(e->loc,((_tmp9B0="datatype constructor %s expects argument of type %s but this argument has type %s",_tag_dyneither(_tmp9B0,sizeof(char),82))),_tag_dyneither(_tmp9AF,sizeof(void*),3)))))))))))))))))));}
# 2194
Cyc_Tcutil_explain_failure();}}
# 2197
if(es != 0){
const char*_tmp9B4;void*_tmp9B3[1];struct Cyc_String_pa_PrintArg_struct _tmp9B2;void*_tmp4E6=(_tmp9B2.tag=0,((_tmp9B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2200
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9B3[0]=& _tmp9B2,Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,((_tmp9B4="too many arguments for datatype constructor %s",_tag_dyneither(_tmp9B4,sizeof(char),47))),_tag_dyneither(_tmp9B3,sizeof(void*),1)))))));_npop_handler(0);return _tmp4E6;}
if(ts != 0){
const char*_tmp9B8;void*_tmp9B7[1];struct Cyc_String_pa_PrintArg_struct _tmp9B6;void*_tmp4EA=(_tmp9B6.tag=0,((_tmp9B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name)),((_tmp9B7[0]=& _tmp9B6,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9B8="too few arguments for datatype constructor %s",_tag_dyneither(_tmp9B8,sizeof(char),46))),_tag_dyneither(_tmp9B7,sizeof(void*),1)))))));_npop_handler(0);return _tmp4EA;}{
void*_tmp4EB=res;_npop_handler(0);return _tmp4EB;};};}
# 2169
;_pop_region(rgn);}
# 2208
static int Cyc_Tcexp_zeroable_type(void*t){
void*_tmp4F0=Cyc_Tcutil_compress(t);void*_tmp4F1=_tmp4F0;union Cyc_Absyn_Constraint*_tmp4F8;void*_tmp4FC;struct Cyc_List_List*_tmp4FF;union Cyc_Absyn_AggrInfoU _tmp501;struct Cyc_List_List*_tmp502;struct Cyc_List_List*_tmp506;_LL1F9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp4F2=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F2->tag != 0)goto _LL1FB;}_LL1FA:
 return 1;_LL1FB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp4F3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4F1;if(_tmp4F3->tag != 1)goto _LL1FD;}_LL1FC:
 goto _LL1FE;_LL1FD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F4->tag != 2)goto _LL1FF;}_LL1FE:
 goto _LL200;_LL1FF: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F5->tag != 3)goto _LL201;}_LL200:
 goto _LL202;_LL201: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4F6=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F6->tag != 4)goto _LL203;}_LL202:
 return 0;_LL203: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F7->tag != 5)goto _LL205;else{_tmp4F8=((_tmp4F7->f1).ptr_atts).nullable;}}_LL204:
# 2216
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp4F8);_LL205: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp4F9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp4F1;if(_tmp4F9->tag != 6)goto _LL207;}_LL206:
 goto _LL208;_LL207: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp4FA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp4F1;if(_tmp4FA->tag != 7)goto _LL209;}_LL208:
 return 1;_LL209: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp4FB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4F1;if(_tmp4FB->tag != 8)goto _LL20B;else{_tmp4FC=(_tmp4FB->f1).elt_type;}}_LL20A:
 return Cyc_Tcexp_zeroable_type(_tmp4FC);_LL20B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4FD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4F1;if(_tmp4FD->tag != 9)goto _LL20D;}_LL20C:
 return 0;_LL20D: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4FE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4F1;if(_tmp4FE->tag != 10)goto _LL20F;else{_tmp4FF=_tmp4FE->f1;}}_LL20E:
# 2222
 for(0;(unsigned int)_tmp4FF;_tmp4FF=_tmp4FF->tl){
if(!Cyc_Tcexp_zeroable_type((*((struct _tuple18*)_tmp4FF->hd)).f2))return 0;}
return 1;_LL20F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp500=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4F1;if(_tmp500->tag != 11)goto _LL211;else{_tmp501=(_tmp500->f1).aggr_info;_tmp502=(_tmp500->f1).targs;}}_LL210: {
# 2226
struct Cyc_Absyn_Aggrdecl*_tmp513=Cyc_Absyn_get_known_aggrdecl(_tmp501);
if(_tmp513->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp513->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp514=_new_region("r");struct _RegionHandle*r=& _tmp514;_push_region(r);
{struct Cyc_List_List*_tmp515=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp513->tvs,_tmp502);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp513->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcexp_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp515,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp516=0;_npop_handler(0);return _tmp516;}}}{
int _tmp517=1;_npop_handler(0);return _tmp517;};}
# 2230
;_pop_region(r);};}_LL211: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp503=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4F1;if(_tmp503->tag != 13)goto _LL213;}_LL212:
# 2235
 return 1;_LL213: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp504=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4F1;if(_tmp504->tag != 19)goto _LL215;}_LL214:
 return 1;_LL215: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp505=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4F1;if(_tmp505->tag != 12)goto _LL217;else{_tmp506=_tmp505->f2;}}_LL216:
# 2238
 for(0;_tmp506 != 0;_tmp506=_tmp506->tl){
if(!Cyc_Tcexp_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp506->hd)->type))return 0;}
return 1;_LL217: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp507=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4F1;if(_tmp507->tag != 14)goto _LL219;}_LL218:
 return 1;_LL219: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp508=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4F1;if(_tmp508->tag != 17)goto _LL21B;}_LL21A:
 return 0;_LL21B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp509=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4F1;if(_tmp509->tag != 16)goto _LL21D;}_LL21C:
 return 0;_LL21D: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp50A=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4F1;if(_tmp50A->tag != 15)goto _LL21F;}_LL21E:
 return 0;_LL21F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp50B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4F1;if(_tmp50B->tag != 26)goto _LL221;}_LL220:
# 2246
 goto _LL222;_LL221: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp50C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4F1;if(_tmp50C->tag != 18)goto _LL223;}_LL222:
 goto _LL224;_LL223: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp50D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4F1;if(_tmp50D->tag != 20)goto _LL225;}_LL224:
 goto _LL226;_LL225: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp50E=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp4F1;if(_tmp50E->tag != 21)goto _LL227;}_LL226:
 goto _LL228;_LL227: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp50F=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp4F1;if(_tmp50F->tag != 22)goto _LL229;}_LL228:
 goto _LL22A;_LL229: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp510=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4F1;if(_tmp510->tag != 23)goto _LL22B;}_LL22A:
 goto _LL22C;_LL22B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4F1;if(_tmp511->tag != 24)goto _LL22D;}_LL22C:
 goto _LL22E;_LL22D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4F1;if(_tmp512->tag != 25)goto _LL1F8;}_LL22E: {
const char*_tmp9BC;void*_tmp9BB[1];struct Cyc_String_pa_PrintArg_struct _tmp9BA;(_tmp9BA.tag=0,((_tmp9BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9BB[0]=& _tmp9BA,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9BC="bad type `%s' in zeroable type",_tag_dyneither(_tmp9BC,sizeof(char),31))),_tag_dyneither(_tmp9BB,sizeof(void*),1)))))));}_LL1F8:;}
# 2257
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2259
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
# 2261
if(topt != 0){
void*_tmp51B=Cyc_Tcutil_compress(*topt);void*_tmp51C=_tmp51B;void*_tmp51E;_LL230: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51C;if(_tmp51D->tag != 5)goto _LL232;else{_tmp51E=(_tmp51D->f1).elt_typ;}}_LL231:
# 2264
 Cyc_Tcutil_unify(_tmp51E,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcexp_zeroable_type(t))return;
goto _LL22F;_LL232:;_LL233:
 goto _LL22F;_LL22F:;}{
# 2270
const char*_tmp9C5;void*_tmp9C4[2];const char*_tmp9C3;const char*_tmp9C2;struct Cyc_String_pa_PrintArg_struct _tmp9C1;struct Cyc_String_pa_PrintArg_struct _tmp9C0;(_tmp9C0.tag=0,((_tmp9C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp9C1.tag=0,((_tmp9C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)((_tmp9C2="calloc",_tag_dyneither(_tmp9C2,sizeof(char),7))):(struct _dyneither_ptr)((_tmp9C3="malloc",_tag_dyneither(_tmp9C3,sizeof(char),7))))),((_tmp9C4[0]=& _tmp9C1,((_tmp9C4[1]=& _tmp9C0,Cyc_Tcutil_terr(loc,((_tmp9C5="%s cannot be used with type %s\n\t(type needs initialization)",_tag_dyneither(_tmp9C5,sizeof(char),60))),_tag_dyneither(_tmp9C4,sizeof(void*),2)))))))))))));};}
# 2274
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2276
enum Cyc_Absyn_AliasQual _tmp525=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;switch(_tmp525){case Cyc_Absyn_Unique: _LL234:
 return(void*)& Cyc_Absyn_UniqueRgn_val;default: _LL235:
 return(void*)& Cyc_Absyn_HeapRgn_val;}}
# 2282
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2287
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2290
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9C8;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9C7;void*expected_type=(void*)(
(_tmp9C7=_cycalloc(sizeof(*_tmp9C7)),((_tmp9C7[0]=((_tmp9C8.tag=15,((_tmp9C8.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te)),_tmp9C8)))),_tmp9C7))));
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp526=Cyc_Tcutil_compress(handle_type);void*_tmp527=_tmp526;void*_tmp529;_LL238: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp527;if(_tmp528->tag != 15)goto _LL23A;else{_tmp529=(void*)_tmp528->f1;}}_LL239:
# 2295
 rgn=_tmp529;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL237;_LL23A:;_LL23B:
# 2299
{const char*_tmp9CC;void*_tmp9CB[1];struct Cyc_String_pa_PrintArg_struct _tmp9CA;(_tmp9CA.tag=0,((_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmp9CB[0]=& _tmp9CA,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,((_tmp9CC="expecting region_t type but found %s",_tag_dyneither(_tmp9CC,sizeof(char),37))),_tag_dyneither(_tmp9CB,sizeof(void*),1)))))));}
goto _LL237;_LL237:;}else{
# 2306
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2313
{struct _RegionHandle _tmp52F=_new_region("r");struct _RegionHandle*r=& _tmp52F;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2324 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0){const char*_tmp9CF;void*_tmp9CE;(_tmp9CE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp9CF="calloc with empty type",_tag_dyneither(_tmp9CF,sizeof(char),23))),_tag_dyneither(_tmp9CE,sizeof(void*),0)));}
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2336
void*er=(*e)->r;
retry_sizeof: {
void*_tmp532=er;void*_tmp534;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp537;_LL23D: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp532;if(_tmp533->tag != 16)goto _LL23F;else{_tmp534=(void*)_tmp533->f1;}}_LL23E:
# 2340
 elt_type=_tmp534;
{void**_tmp9D0;*t=((_tmp9D0=_cycalloc(sizeof(*_tmp9D0)),((_tmp9D0[0]=elt_type,_tmp9D0))));}
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL23C;_LL23F: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp532;if(_tmp535->tag != 2)goto _LL241;else{if(_tmp535->f1 != Cyc_Absyn_Times)goto _LL241;if(_tmp535->f2 == 0)goto _LL241;_tmp536=(struct Cyc_Absyn_Exp*)(_tmp535->f2)->hd;if((_tmp535->f2)->tl == 0)goto _LL241;_tmp537=(struct Cyc_Absyn_Exp*)((_tmp535->f2)->tl)->hd;if(((_tmp535->f2)->tl)->tl != 0)goto _LL241;}}_LL240:
# 2347
{struct _tuple0 _tmp9D1;struct _tuple0 _tmp539=(_tmp9D1.f1=_tmp536->r,((_tmp9D1.f2=_tmp537->r,_tmp9D1)));struct _tuple0 _tmp53A=_tmp539;void*_tmp53C;void*_tmp53E;_LL244: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp53B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp53A.f1;if(_tmp53B->tag != 16)goto _LL246;else{_tmp53C=(void*)_tmp53B->f1;}}_LL245:
# 2349
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp53C);
elt_type=_tmp53C;
{void**_tmp9D2;*t=((_tmp9D2=_cycalloc(sizeof(*_tmp9D2)),((_tmp9D2[0]=elt_type,_tmp9D2))));}
num_elts=_tmp537;
one_elt=0;
goto _LL243;_LL246: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp53A.f2;if(_tmp53D->tag != 16)goto _LL248;else{_tmp53E=(void*)_tmp53D->f1;}}_LL247:
# 2356
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp53E);
elt_type=_tmp53E;
{void**_tmp9D3;*t=((_tmp9D3=_cycalloc(sizeof(*_tmp9D3)),((_tmp9D3[0]=elt_type,_tmp9D3))));}
num_elts=_tmp536;
one_elt=0;
goto _LL243;_LL248:;_LL249:
 goto No_sizeof;_LL243:;}
# 2364
goto _LL23C;_LL241:;_LL242:
# 2366
 No_sizeof: {
# 2369
struct Cyc_Absyn_Exp*_tmp542=*e;
{void*_tmp543=_tmp542->r;void*_tmp544=_tmp543;struct Cyc_Absyn_Exp*_tmp546;_LL24B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp544;if(_tmp545->tag != 13)goto _LL24D;else{_tmp546=_tmp545->f2;}}_LL24C:
 _tmp542=_tmp546;goto _LL24A;_LL24D:;_LL24E:
 goto _LL24A;_LL24A:;}
# 2374
{void*_tmp547=Cyc_Tcutil_compress((void*)_check_null(_tmp542->topt));void*_tmp548=_tmp547;void*_tmp54A;_LL250: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp548;if(_tmp549->tag != 19)goto _LL252;else{_tmp54A=(void*)_tmp549->f1;}}_LL251:
# 2376
{void*_tmp54B=Cyc_Tcutil_compress(_tmp54A);void*_tmp54C=_tmp54B;struct Cyc_Absyn_Exp*_tmp54E;_LL255: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp54D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp54C;if(_tmp54D->tag != 18)goto _LL257;else{_tmp54E=_tmp54D->f1;}}_LL256:
# 2378
 er=_tmp54E->r;goto retry_sizeof;_LL257:;_LL258:
 goto _LL254;_LL254:;}
# 2381
goto _LL24F;_LL252:;_LL253:
 goto _LL24F;_LL24F:;}
# 2384
elt_type=Cyc_Absyn_char_typ;
{void**_tmp9D4;*t=((_tmp9D4=_cycalloc(sizeof(*_tmp9D4)),((_tmp9D4[0]=elt_type,_tmp9D4))));}
num_elts=*e;
one_elt=0;}
# 2389
goto _LL23C;_LL23C:;}}
# 2393
*is_fat=!one_elt;
# 2396
{void*_tmp550=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp552;_LL25A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp551=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp550;if(_tmp551->tag != 11)goto _LL25C;else{if((((_tmp551->f1).aggr_info).KnownAggr).tag != 2)goto _LL25C;_tmp552=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp551->f1).aggr_info).KnownAggr).val);}}_LL25B:
# 2398
 if(_tmp552->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp552->impl))->exist_vars != 0){
const char*_tmp9D7;void*_tmp9D6;(_tmp9D6=0,Cyc_Tcutil_terr(loc,((_tmp9D7="malloc with existential types not yet implemented",_tag_dyneither(_tmp9D7,sizeof(char),50))),_tag_dyneither(_tmp9D6,sizeof(void*),0)));}
goto _LL259;_LL25C:;_LL25D:
 goto _LL259;_LL259:;}{
# 2405
void*(*_tmp555)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp556=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp557=Cyc_Tcutil_compress(*topt);void*_tmp558=_tmp557;union Cyc_Absyn_Constraint*_tmp55A;union Cyc_Absyn_Constraint*_tmp55B;union Cyc_Absyn_Constraint*_tmp55C;_LL25F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp558;if(_tmp559->tag != 5)goto _LL261;else{_tmp55A=((_tmp559->f1).ptr_atts).nullable;_tmp55B=((_tmp559->f1).ptr_atts).bounds;_tmp55C=((_tmp559->f1).ptr_atts).zero_term;}}_LL260:
# 2410
 _tmp556=_tmp55C;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55A))
_tmp555=Cyc_Absyn_star_typ;
# 2415
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55C) && !(*is_calloc)){
{const char*_tmp9DA;void*_tmp9D9;(_tmp9D9=0,Cyc_Tcutil_warn(loc,((_tmp9DA="converting malloc to calloc to ensure zero-termination",_tag_dyneither(_tmp9DA,sizeof(char),55))),_tag_dyneither(_tmp9D9,sizeof(void*),0)));}
*is_calloc=1;}
# 2421
{void*_tmp55F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55B);void*_tmp560=_tmp55F;struct Cyc_Absyn_Exp*_tmp563;_LL264: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp561=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp560;if(_tmp561->tag != 0)goto _LL266;}_LL265:
 goto _LL263;_LL266:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp562=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp560;if(_tmp562->tag != 1)goto _LL268;else{_tmp563=_tmp562->f1;}}if(!(!one_elt))goto _LL268;_LL267: {
# 2424
int _tmp564=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp564  && Cyc_Evexp_same_const_exp(_tmp563,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp55B),_tmp556);}
# 2430
{void*_tmp565=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp566=_tmp565;void*_tmp568;_LL26B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp566;if(_tmp567->tag != 19)goto _LL26D;else{_tmp568=(void*)_tmp567->f1;}}_LL26C: {
# 2432
struct Cyc_Absyn_Exp*_tmp569=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp568,0),0,Cyc_Absyn_No_coercion,0);
# 2434
if(Cyc_Evexp_same_const_exp(_tmp569,_tmp563)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp55B),_tmp556);}
# 2439
goto _LL26A;}_LL26D:;_LL26E:
 goto _LL26A;_LL26A:;}
# 2442
goto _LL263;}_LL268:;_LL269:
 goto _LL263;_LL263:;}
# 2445
goto _LL25E;_LL261:;_LL262:
 goto _LL25E;_LL25E:;}
# 2448
if(!one_elt)_tmp555=Cyc_Absyn_dyneither_typ;
return _tmp555(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp556);};};}
# 2453
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2459
struct _RegionHandle _tmp56A=_new_region("r");struct _RegionHandle*r=& _tmp56A;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp56B=Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te));
Cyc_Tcexp_tcExpNoPromote(_tmp56B,0,e1);{
void*_tmp56C=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp56B,& _tmp56C,e2);};}{
# 2465
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2468
{void*_tmp56D=Cyc_Tcutil_compress(t1);void*_tmp56E=_tmp56D;_LL270: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56E;if(_tmp56F->tag != 8)goto _LL272;}_LL271:
{const char*_tmp9DD;void*_tmp9DC;(_tmp9DC=0,Cyc_Tcutil_terr(loc,((_tmp9DD="cannot assign to an array",_tag_dyneither(_tmp9DD,sizeof(char),26))),_tag_dyneither(_tmp9DC,sizeof(void*),0)));}goto _LL26F;_LL272:;_LL273:
 goto _LL26F;_LL26F:;}
# 2473
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1)){
const char*_tmp9E0;void*_tmp9DF;(_tmp9DF=0,Cyc_Tcutil_terr(loc,((_tmp9E0="Swap not allowed for non-pointer or non-word-sized types.",_tag_dyneither(_tmp9E0,sizeof(char),58))),_tag_dyneither(_tmp9DF,sizeof(void*),0)));}
# 2477
if(!Cyc_Absyn_is_lvalue(e1)){
const char*_tmp9E3;void*_tmp9E2;void*_tmp576=(_tmp9E2=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9E3="swap non-lvalue",_tag_dyneither(_tmp9E3,sizeof(char),16))),_tag_dyneither(_tmp9E2,sizeof(void*),0)));_npop_handler(0);return _tmp576;}
if(!Cyc_Absyn_is_lvalue(e2)){
const char*_tmp9E6;void*_tmp9E5;void*_tmp579=(_tmp9E5=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9E6="swap non-lvalue",_tag_dyneither(_tmp9E6,sizeof(char),16))),_tag_dyneither(_tmp9E5,sizeof(void*),0)));_npop_handler(0);return _tmp579;}{
# 2482
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9E9;void*_tmp9E8;void*_tmp57C=(_tmp9E8=0,Cyc_Tcexp_expr_err(te,e1->loc,topt,((_tmp9E9="swap value in zeroterm array",_tag_dyneither(_tmp9E9,sizeof(char),29))),_tag_dyneither(_tmp9E8,sizeof(void*),0)));_npop_handler(0);return _tmp57C;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
const char*_tmp9EC;void*_tmp9EB;void*_tmp57F=(_tmp9EB=0,Cyc_Tcexp_expr_err(te,e2->loc,topt,((_tmp9EC="swap value in zeroterm array",_tag_dyneither(_tmp9EC,sizeof(char),29))),_tag_dyneither(_tmp9EB,sizeof(void*),0)));_npop_handler(0);return _tmp57F;}
# 2489
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
const char*_tmp9F1;void*_tmp9F0[2];struct Cyc_String_pa_PrintArg_struct _tmp9EF;struct Cyc_String_pa_PrintArg_struct _tmp9EE;void*_tmp580=(_tmp9EE.tag=0,((_tmp9EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp9EF.tag=0,((_tmp9EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp9F0[0]=& _tmp9EF,((_tmp9F0[1]=& _tmp9EE,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9F1="type mismatch: %s != %s",_tag_dyneither(_tmp9F1,sizeof(char),24))),_tag_dyneither(_tmp9F0,sizeof(void*),2)))))))))))));
void*_tmp581=_tmp580;_npop_handler(0);return _tmp581;}{
# 2496
void*_tmp586=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp586;};};};
# 2459
;_pop_region(r);}
# 2500
int Cyc_Tcexp_in_stmt_exp=0;
# 2502
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
struct _RegionHandle _tmp587=_new_region("r");struct _RegionHandle*r=& _tmp587;_push_region(r);{
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_encloser(r,te,s)),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2509
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 2511
while(1){
void*_tmp588=s->r;void*_tmp589=_tmp588;struct Cyc_Absyn_Exp*_tmp58B;struct Cyc_Absyn_Stmt*_tmp58D;struct Cyc_Absyn_Stmt*_tmp58E;struct Cyc_Absyn_Decl*_tmp590;struct Cyc_Absyn_Stmt*_tmp591;_LL275: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp58A=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp589;if(_tmp58A->tag != 1)goto _LL277;else{_tmp58B=_tmp58A->f1;}}_LL276: {
# 2515
void*_tmp592=(void*)_check_null(_tmp58B->topt);
if(!Cyc_Tcutil_unify(_tmp592,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
{const char*_tmp9F5;void*_tmp9F4[1];struct Cyc_String_pa_PrintArg_struct _tmp9F3;(_tmp9F3.tag=0,((_tmp9F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp592)),((_tmp9F4[0]=& _tmp9F3,Cyc_Tcutil_terr(loc,((_tmp9F5="statement expression returns type %s",_tag_dyneither(_tmp9F5,sizeof(char),37))),_tag_dyneither(_tmp9F4,sizeof(void*),1)))))));}
Cyc_Tcutil_explain_failure();}{
# 2521
void*_tmp596=_tmp592;_npop_handler(0);return _tmp596;};}_LL277: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp58C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp589;if(_tmp58C->tag != 2)goto _LL279;else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;}}_LL278:
 s=_tmp58E;continue;_LL279: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp58F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp589;if(_tmp58F->tag != 12)goto _LL27B;else{_tmp590=_tmp58F->f1;_tmp591=_tmp58F->f2;}}_LL27A:
 s=_tmp591;continue;_LL27B:;_LL27C: {
# 2525
const char*_tmp9F8;void*_tmp9F7;void*_tmp599=(_tmp9F7=0,Cyc_Tcexp_expr_err(te,loc,topt,((_tmp9F8="statement expression must end with expression",_tag_dyneither(_tmp9F8,sizeof(char),46))),_tag_dyneither(_tmp9F7,sizeof(void*),0)));_npop_handler(0);return _tmp599;}_LL274:;}
# 2503
;_pop_region(r);}
# 2530
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
struct _RegionHandle _tmp59A=_new_region("r");struct _RegionHandle*r=& _tmp59A;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));
# 2535
{void*_tmp59B=t;struct Cyc_Absyn_Aggrdecl*_tmp59D;_LL27E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp59B;if(_tmp59C->tag != 11)goto _LL280;else{if((((_tmp59C->f1).aggr_info).KnownAggr).tag != 2)goto _LL280;_tmp59D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp59C->f1).aggr_info).KnownAggr).val);}}_LL27F:
# 2537
 if((_tmp59D->kind == Cyc_Absyn_UnionA  && _tmp59D->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp59D->impl))->tagged)goto _LL27D;
goto _LL281;_LL280:;_LL281:
# 2540
{const char*_tmp9FC;void*_tmp9FB[1];struct Cyc_String_pa_PrintArg_struct _tmp9FA;(_tmp9FA.tag=0,((_tmp9FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp9FB[0]=& _tmp9FA,Cyc_Tcutil_terr(loc,((_tmp9FC="expecting @tagged union but found %s",_tag_dyneither(_tmp9FC,sizeof(char),37))),_tag_dyneither(_tmp9FB,sizeof(void*),1)))))));}
goto _LL27D;_LL27D:;}{
# 2543
void*_tmp5A1=Cyc_Absyn_uint_typ;_npop_handler(0);return _tmp5A1;};
# 2532
;_pop_region(r);}
# 2547
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2551
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp5A2=_new_region("r");struct _RegionHandle*r=& _tmp5A2;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp5A3=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2556
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp9FF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9FE;void*expected_type=(void*)(
(_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE[0]=((_tmp9FF.tag=15,((_tmp9FF.f1=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp5A3)),_tmp9FF)))),_tmp9FE))));
void*handle_type=Cyc_Tcexp_tcExp(_tmp5A3,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp5A4=Cyc_Tcutil_compress(handle_type);void*_tmp5A5=_tmp5A4;void*_tmp5A7;_LL283: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A5;if(_tmp5A6->tag != 15)goto _LL285;else{_tmp5A7=(void*)_tmp5A6->f1;}}_LL284:
# 2561
 rgn=_tmp5A7;
Cyc_Tcenv_check_rgn_accessible(_tmp5A3,loc,rgn);
goto _LL282;_LL285:;_LL286:
# 2565
{const char*_tmpA03;void*_tmpA02[1];struct Cyc_String_pa_PrintArg_struct _tmpA01;(_tmpA01.tag=0,((_tmpA01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type)),((_tmpA02[0]=& _tmpA01,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,((_tmpA03="expecting region_t type but found %s",_tag_dyneither(_tmpA03,sizeof(char),37))),_tag_dyneither(_tmpA02,sizeof(void*),1)))))));}
goto _LL282;_LL282:;}else{
# 2572
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2580
void*_tmp5AD=e1->r;void*_tmp5AE=_tmp5AD;struct Cyc_Core_Opt*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct Cyc_List_List*_tmp5B5;_LL288: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5AF=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5AF->tag != 26)goto _LL28A;}_LL289:
 goto _LL28B;_LL28A: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5B0=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5B0->tag != 27)goto _LL28C;}_LL28B: {
# 2585
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp5A3,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA06;void*_tmpA05;(_tmpA05=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA06="tcNew: comprehension returned non-array type",_tag_dyneither(_tmpA06,sizeof(char),45))),_tag_dyneither(_tmpA05,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5A3,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5A3,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2596
void*_tmp5BA=res_typ;_npop_handler(0);return _tmp5BA;};}_LL28C: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp5B1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5B1->tag != 35)goto _LL28E;else{_tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;}}_LL28D:
# 2598
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpA09;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpA08;e1->r=(void*)((_tmpA08=_cycalloc(sizeof(*_tmpA08)),((_tmpA08[0]=((_tmpA09.tag=25,((_tmpA09.f1=_tmp5B3,_tmpA09)))),_tmpA08))));}
_tmp5B5=_tmp5B3;goto _LL28F;_LL28E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5B4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5B4->tag != 25)goto _LL290;else{_tmp5B5=_tmp5B4->f1;}}_LL28F: {
# 2601
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp5BD=Cyc_Tcutil_compress(*topt);void*_tmp5BE=_tmp5BD;void**_tmp5C0;struct Cyc_Absyn_Tqual _tmp5C1;union Cyc_Absyn_Constraint*_tmp5C2;_LL297: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5BF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BE;if(_tmp5BF->tag != 5)goto _LL299;else{_tmp5C0=(void**)&(_tmp5BF->f1).elt_typ;_tmp5C1=(_tmp5BF->f1).elt_tq;_tmp5C2=((_tmp5BF->f1).ptr_atts).zero_term;}}_LL298:
# 2607
 elt_typ_opt=_tmp5C0;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5C2);
goto _LL296;_LL299:;_LL29A:
 goto _LL296;_LL296:;}{
# 2613
void*res_typ=Cyc_Tcexp_tcArray(_tmp5A3,e1->loc,elt_typ_opt,0,zero_term,_tmp5B5);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ)){
const char*_tmpA0C;void*_tmpA0B;(_tmpA0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA0C="tcExpNoPromote on Array_e returned non-array type",_tag_dyneither(_tmpA0C,sizeof(char),50))),_tag_dyneither(_tmpA0B,sizeof(void*),0)));}
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2622
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5A3,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5A3,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2628
void*_tmp5C5=res_typ;_npop_handler(0);return _tmp5C5;};};}_LL290: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5B6->tag != 0)goto _LL292;else{if(((_tmp5B6->f1).String_c).tag != 8)goto _LL292;}}_LL291: {
# 2633
void*_tmp5C6=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2635
void*_tmp5C7=Cyc_Tcexp_tcExp(_tmp5A3,& _tmp5C6,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA0F;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA0E;void*_tmp5CA=Cyc_Absyn_atb_typ(_tmp5C7,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA0E=_cycalloc(sizeof(*_tmpA0E)),((_tmpA0E[0]=((_tmpA0F.tag=1,((_tmpA0F.f1=Cyc_Absyn_uint_exp(1,0),_tmpA0F)))),_tmpA0E)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5CA;}_LL292: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5B7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5AE;if(_tmp5B7->tag != 0)goto _LL294;else{if(((_tmp5B7->f1).Wstring_c).tag != 9)goto _LL294;}}_LL293: {
# 2640
void*_tmp5CB=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2642
void*_tmp5CC=Cyc_Tcexp_tcExp(_tmp5A3,& _tmp5CB,e1);
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpA12;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA11;void*_tmp5CF=Cyc_Absyn_atb_typ(_tmp5CC,rgn,Cyc_Absyn_empty_tqual(0),(void*)((_tmpA11=_cycalloc(sizeof(*_tmpA11)),((_tmpA11[0]=((_tmpA12.tag=1,((_tmpA12.f1=Cyc_Absyn_uint_exp(1,0),_tmpA12)))),_tmpA11)))),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp5CF;}_LL294:;_LL295:
# 2649
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp5D0=Cyc_Tcutil_compress(*topt);void*_tmp5D1=_tmp5D0;void**_tmp5D3;struct Cyc_Absyn_Tqual _tmp5D4;_LL29C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1;if(_tmp5D2->tag != 5)goto _LL29E;else{_tmp5D3=(void**)&(_tmp5D2->f1).elt_typ;_tmp5D4=(_tmp5D2->f1).elt_tq;}}_LL29D:
# 2655
 topt2=_tmp5D3;goto _LL29B;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5D1;if(_tmp5D5->tag != 3)goto _LL2A0;}_LL29F:
# 2659
 bogus=*topt;
topt2=& bogus;
goto _LL29B;_LL2A0:;_LL2A1:
 goto _LL29B;_LL29B:;}{
# 2665
void*telt=Cyc_Tcexp_tcExp(_tmp5A3,topt2,e1);
# 2667
struct _RegionHandle _tmp5D6=_new_region("temp");struct _RegionHandle*temp=& _tmp5D6;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1)){
const char*_tmpA15;void*_tmpA14;(_tmpA14=0,Cyc_Tcutil_terr(e1->loc,((_tmpA15="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA15,sizeof(char),49))),_tag_dyneither(_tmpA14,sizeof(void*),0)));}{
# 2672
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA1B;struct Cyc_Absyn_PtrInfo _tmpA1A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA19;void*res_typ=(void*)(
(_tmpA19=_cycalloc(sizeof(*_tmpA19)),((_tmpA19[0]=((_tmpA1B.tag=5,((_tmpA1B.f1=((_tmpA1A.elt_typ=telt,((_tmpA1A.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmpA1A.ptr_atts=(
((_tmpA1A.ptr_atts).rgn=rgn,(((_tmpA1A.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmpA1A.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmpA1A.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmpA1A.ptr_atts).ptrloc=0,_tmpA1A.ptr_atts)))))))))),_tmpA1A)))))),_tmpA1B)))),_tmpA19))));
# 2677
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp5A3,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp5A3,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2684
void*_tmp5D9=res_typ;_npop_handler(1);return _tmp5D9;};};
# 2667
;_pop_region(temp);};}_LL287:;};}
# 2553
;_pop_region(r);}
# 2691
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2701
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2705
struct _RegionHandle _tmp5DD=_new_region("temp");struct _RegionHandle*temp=& _tmp5DD;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
# 2711
const char*_tmpA1E;void*_tmpA1D;(_tmpA1D=0,Cyc_Tcutil_terr(e->loc,((_tmpA1E="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmpA1E,sizeof(char),49))),_tag_dyneither(_tmpA1D,sizeof(void*),0)));}{
# 2714
void*_tmp5E0=e->r;void*_tmp5E1=_tmp5E0;_LL2A3: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp5E2=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5E1;if(_tmp5E2->tag != 25)goto _LL2A5;}_LL2A4:
 goto _LL2A6;_LL2A5: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp5E3=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5E1;if(_tmp5E3->tag != 26)goto _LL2A7;}_LL2A6:
 goto _LL2A8;_LL2A7: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp5E4=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5E1;if(_tmp5E4->tag != 27)goto _LL2A9;}_LL2A8:
 goto _LL2AA;_LL2A9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5E5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E1;if(_tmp5E5->tag != 0)goto _LL2AB;else{if(((_tmp5E5->f1).Wstring_c).tag != 9)goto _LL2AB;}}_LL2AA:
 goto _LL2AC;_LL2AB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp5E6=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E1;if(_tmp5E6->tag != 0)goto _LL2AD;else{if(((_tmp5E6->f1).String_c).tag != 8)goto _LL2AD;}}_LL2AC: {
void*_tmp5E7=t;_npop_handler(0);return _tmp5E7;}_LL2AD:;_LL2AE:
# 2721
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2724
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}{
# 2727
void*_tmp5E8=t;_npop_handler(0);return _tmp5E8;};_LL2A2:;};
# 2705
;_pop_region(temp);}
# 2738 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp5E9=e->r;void*_tmp5EA=_tmp5E9;struct Cyc_Absyn_Exp*_tmp5EC;_LL2B0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp5EB=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5EA;if(_tmp5EB->tag != 11)goto _LL2B2;else{_tmp5EC=_tmp5EB->f1;}}_LL2B1:
# 2742
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp5EC);
_tmp5EC->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp5EC->topt),0);
e->topt=_tmp5EC->topt;
goto _LL2AF;_LL2B2:;_LL2B3:
# 2748
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2751
{void*_tmp5ED=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5EE=_tmp5ED;void*_tmp5F0;struct Cyc_Absyn_Tqual _tmp5F1;void*_tmp5F2;union Cyc_Absyn_Constraint*_tmp5F3;union Cyc_Absyn_Constraint*_tmp5F4;union Cyc_Absyn_Constraint*_tmp5F5;_LL2B5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EE;if(_tmp5EF->tag != 5)goto _LL2B7;else{_tmp5F0=(_tmp5EF->f1).elt_typ;_tmp5F1=(_tmp5EF->f1).elt_tq;_tmp5F2=((_tmp5EF->f1).ptr_atts).rgn;_tmp5F3=((_tmp5EF->f1).ptr_atts).nullable;_tmp5F4=((_tmp5EF->f1).ptr_atts).bounds;_tmp5F5=((_tmp5EF->f1).ptr_atts).zero_term;}}_LL2B6:
# 2753
{void*_tmp5F6=Cyc_Tcutil_compress(_tmp5F0);void*_tmp5F7=_tmp5F6;struct Cyc_List_List*_tmp5F9;void*_tmp5FA;struct Cyc_Absyn_Tqual _tmp5FB;void*_tmp5FC;struct Cyc_List_List*_tmp5FD;int _tmp5FE;struct Cyc_Absyn_VarargInfo*_tmp5FF;struct Cyc_List_List*_tmp600;struct Cyc_List_List*_tmp601;_LL2BA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5F7;if(_tmp5F8->tag != 9)goto _LL2BC;else{_tmp5F9=(_tmp5F8->f1).tvars;_tmp5FA=(_tmp5F8->f1).effect;_tmp5FB=(_tmp5F8->f1).ret_tqual;_tmp5FC=(_tmp5F8->f1).ret_typ;_tmp5FD=(_tmp5F8->f1).args;_tmp5FE=(_tmp5F8->f1).c_varargs;_tmp5FF=(_tmp5F8->f1).cyc_varargs;_tmp600=(_tmp5F8->f1).rgn_po;_tmp601=(_tmp5F8->f1).attributes;}}_LL2BB:
# 2755
 if(_tmp5F9 != 0){
struct _RegionHandle _tmp602=_new_region("rgn");struct _RegionHandle*rgn=& _tmp602;_push_region(rgn);
{struct _tuple12 _tmpA1F;struct _tuple12 _tmp603=(_tmpA1F.f1=Cyc_Tcenv_lookup_type_vars(te),((_tmpA1F.f2=rgn,_tmpA1F)));
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp603,_tmp5F9);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2764
_tmp600=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp600);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp600);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpA25;struct Cyc_Absyn_FnInfo _tmpA24;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA23;void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmpA23=_cycalloc(sizeof(*_tmpA23)),((_tmpA23[0]=((_tmpA25.tag=9,((_tmpA25.f1=((_tmpA24.tvars=0,((_tmpA24.effect=_tmp5FA,((_tmpA24.ret_tqual=_tmp5FB,((_tmpA24.ret_typ=_tmp5FC,((_tmpA24.args=_tmp5FD,((_tmpA24.c_varargs=_tmp5FE,((_tmpA24.cyc_varargs=_tmp5FF,((_tmpA24.rgn_po=0,((_tmpA24.attributes=_tmp601,_tmpA24)))))))))))))))))),_tmpA25)))),_tmpA23)))));
# 2770
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpA2B;struct Cyc_Absyn_PtrInfo _tmpA2A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA29;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp604=(_tmpA29=_cycalloc(sizeof(*_tmpA29)),((_tmpA29[0]=((_tmpA2B.tag=5,((_tmpA2B.f1=((_tmpA2A.elt_typ=ftyp,((_tmpA2A.elt_tq=_tmp5F1,((_tmpA2A.ptr_atts=(((_tmpA2A.ptr_atts).rgn=_tmp5F2,(((_tmpA2A.ptr_atts).nullable=_tmp5F3,(((_tmpA2A.ptr_atts).bounds=_tmp5F4,(((_tmpA2A.ptr_atts).zero_term=_tmp5F5,(((_tmpA2A.ptr_atts).ptrloc=0,_tmpA2A.ptr_atts)))))))))),_tmpA2A)))))),_tmpA2B)))),_tmpA29)));
# 2772
struct Cyc_Absyn_Exp*_tmp605=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmpA2E;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpA2D;e->r=(void*)((_tmpA2D=_cycalloc(sizeof(*_tmpA2D)),((_tmpA2D[0]=((_tmpA2E.tag=12,((_tmpA2E.f1=_tmp605,((_tmpA2E.f2=ts,_tmpA2E)))))),_tmpA2D))));}
e->topt=(void*)_tmp604;};}
# 2757
;_pop_region(rgn);}
# 2777
goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 2780
goto _LL2B4;_LL2B7:;_LL2B8:
 goto _LL2B4;_LL2B4:;}
# 2783
goto _LL2AF;_LL2AF:;}
# 2785
return(void*)_check_null(e->topt);}
# 2793
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2795
struct _RegionHandle _tmp60F=_new_region("r");struct _RegionHandle*r=& _tmp60F;_push_region(r);
{struct Cyc_List_List*_tmp610=0;
# 2798
{void*_tmp611=fn_exp->r;void*_tmp612=_tmp611;struct Cyc_List_List*_tmp614;_LL2BF: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp612;if(_tmp613->tag != 9)goto _LL2C1;else{_tmp614=_tmp613->f2;}}_LL2C0:
# 2800
{void*_tmp615=e->r;void*_tmp616=_tmp615;struct Cyc_List_List*_tmp618;_LL2C4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp617=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp616;if(_tmp617->tag != 9)goto _LL2C6;else{_tmp618=_tmp617->f2;}}_LL2C5: {
# 2802
struct Cyc_List_List*_tmp619=alias_arg_exps;
int _tmp61A=0;
while(_tmp619 != 0){
while(_tmp61A != (int)_tmp619->hd){
if(_tmp614 == 0){
struct Cyc_Int_pa_PrintArg_struct _tmpA38;struct Cyc_Int_pa_PrintArg_struct _tmpA37;void*_tmpA36[2];const char*_tmpA35;void*_tmpA34;(_tmpA34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpA38.tag=1,((_tmpA38.f1=(unsigned long)((int)_tmp619->hd),((_tmpA37.tag=1,((_tmpA37.f1=(unsigned long)_tmp61A,((_tmpA36[0]=& _tmpA37,((_tmpA36[1]=& _tmpA38,Cyc_aprintf(((_tmpA35="bad count %d/%d for alias coercion!",_tag_dyneither(_tmpA35,sizeof(char),36))),_tag_dyneither(_tmpA36,sizeof(void*),2)))))))))))))),_tag_dyneither(_tmpA34,sizeof(void*),0)));}
# 2809
++ _tmp61A;
_tmp614=_tmp614->tl;
_tmp618=((struct Cyc_List_List*)_check_null(_tmp618))->tl;}{
# 2814
struct _tuple11 _tmp620=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp614))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp618))->hd)->topt)));struct Cyc_Absyn_Decl*_tmp622;struct Cyc_Absyn_Exp*_tmp623;struct _tuple11 _tmp621=_tmp620;_tmp622=_tmp621.f1;_tmp623=_tmp621.f2;
_tmp614->hd=(void*)_tmp623;
{struct Cyc_List_List*_tmpA39;_tmp610=((_tmpA39=_region_malloc(r,sizeof(*_tmpA39)),((_tmpA39->hd=_tmp622,((_tmpA39->tl=_tmp610,_tmpA39))))));}
_tmp619=_tmp619->tl;};}
# 2819
goto _LL2C3;}_LL2C6:;_LL2C7: {
const char*_tmpA3C;void*_tmpA3B;(_tmpA3B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA3C="not a function call!",_tag_dyneither(_tmpA3C,sizeof(char),21))),_tag_dyneither(_tmpA3B,sizeof(void*),0)));}_LL2C3:;}
# 2822
goto _LL2BE;_LL2C1:;_LL2C2: {
const char*_tmpA3F;void*_tmpA3E;(_tmpA3E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpA3F="not a function call!",_tag_dyneither(_tmpA3F,sizeof(char),21))),_tag_dyneither(_tmpA3E,sizeof(void*),0)));}_LL2BE:;}
# 2827
while(_tmp610 != 0){
struct Cyc_Absyn_Decl*_tmp629=(struct Cyc_Absyn_Decl*)_tmp610->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp629,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp610=_tmp610->tl;}
# 2834
e->topt=0;
e->r=fn_exp->r;}
# 2796
;_pop_region(r);}
# 2841
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2845
{void*_tmp62A=e->r;void*_tmp62B=_tmp62A;struct Cyc_Absyn_Exp*_tmp62D;struct Cyc_Absyn_Exp*_tmp62F;struct Cyc_List_List*_tmp630;struct Cyc_Absyn_VarargCallInfo**_tmp631;int*_tmp632;struct Cyc_Core_Opt*_tmp634;struct Cyc_List_List*_tmp635;union Cyc_Absyn_Cnst*_tmp637;struct _tuple1*_tmp639;void**_tmp63A;enum Cyc_Absyn_Primop _tmp63C;struct Cyc_List_List*_tmp63D;struct Cyc_Absyn_Exp*_tmp63F;enum Cyc_Absyn_Incrementor _tmp640;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Core_Opt*_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64B;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64E;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_VarargCallInfo**_tmp655;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_List_List*_tmp65A;void*_tmp65C;struct Cyc_Absyn_Exp*_tmp65D;enum Cyc_Absyn_Coercion*_tmp65E;struct Cyc_Absyn_Exp*_tmp660;struct Cyc_Absyn_Exp**_tmp662;struct Cyc_Absyn_Exp*_tmp663;struct Cyc_Absyn_Exp*_tmp665;void*_tmp667;void*_tmp669;struct Cyc_List_List*_tmp66A;struct Cyc_Absyn_Exp*_tmp66C;struct Cyc_Absyn_Exp*_tmp66E;struct _dyneither_ptr*_tmp66F;int*_tmp670;int*_tmp671;struct Cyc_Absyn_Exp*_tmp673;struct _dyneither_ptr*_tmp674;int*_tmp675;int*_tmp676;struct Cyc_Absyn_Exp*_tmp678;struct Cyc_Absyn_Exp*_tmp679;struct Cyc_List_List*_tmp67B;struct _tuple9*_tmp67D;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp680;struct Cyc_Absyn_Stmt*_tmp682;struct Cyc_Absyn_Vardecl*_tmp684;struct Cyc_Absyn_Exp*_tmp685;struct Cyc_Absyn_Exp*_tmp686;int*_tmp687;struct Cyc_Absyn_Exp*_tmp689;void*_tmp68A;int*_tmp68B;struct _tuple1**_tmp68D;struct Cyc_List_List**_tmp68E;struct Cyc_List_List*_tmp68F;struct Cyc_Absyn_Aggrdecl**_tmp690;void*_tmp692;struct Cyc_List_List*_tmp693;struct Cyc_List_List*_tmp695;struct Cyc_Absyn_Datatypedecl*_tmp696;struct Cyc_Absyn_Datatypefield*_tmp697;struct _tuple1**_tmp699;struct Cyc_Absyn_Enumdecl*_tmp69A;struct Cyc_Absyn_Enumfield*_tmp69B;struct _tuple1**_tmp69D;void*_tmp69E;struct Cyc_Absyn_Enumfield*_tmp69F;int*_tmp6A1;struct Cyc_Absyn_Exp**_tmp6A2;void***_tmp6A3;struct Cyc_Absyn_Exp**_tmp6A4;int*_tmp6A5;struct Cyc_Absyn_Exp*_tmp6A7;struct Cyc_Absyn_Exp*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6AA;struct _dyneither_ptr*_tmp6AB;void*_tmp6AD;_LL2C9: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp62C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp62C->tag != 11)goto _LL2CB;else{_tmp62D=_tmp62C->f1;}}_LL2CA:
# 2850
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp62D);
return;_LL2CB:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp62E->tag != 9)goto _LL2CD;else{_tmp62F=_tmp62E->f1;_tmp630=_tmp62E->f2;_tmp631=(struct Cyc_Absyn_VarargCallInfo**)& _tmp62E->f3;_tmp632=(int*)& _tmp62E->f4;}}if(!(!(*_tmp632)))goto _LL2CD;_LL2CC:
# 2856
 if(Cyc_Tcexp_resolve_unknown_fn(te,e,_tmp62F,_tmp630)){
*_tmp632=1;
Cyc_Tcexp_tcExpNoInst(te,topt,e);}else{
# 2860
if(topt != 0)e->topt=*topt;else{
e->topt=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}
# 2863
return;_LL2CD: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp633=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp633->tag != 35)goto _LL2CF;else{_tmp634=_tmp633->f1;_tmp635=_tmp633->f2;}}_LL2CE:
# 2867
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp635);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;_LL2CF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp636=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp636->tag != 0)goto _LL2D1;else{_tmp637=(union Cyc_Absyn_Cnst*)& _tmp636->f1;}}_LL2D0:
# 2872
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp637,e);goto _LL2C8;_LL2D1: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp638=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp638->tag != 1)goto _LL2D3;else{_tmp639=_tmp638->f1;_tmp63A=(void**)((void**)& _tmp638->f2);}}_LL2D2:
# 2874
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp639,_tmp63A);goto _LL2C8;_LL2D3: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp63B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp63B->tag != 2)goto _LL2D5;else{_tmp63C=_tmp63B->f1;_tmp63D=_tmp63B->f2;}}_LL2D4:
# 2876
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp63C,_tmp63D);goto _LL2C8;_LL2D5: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp63E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp63E->tag != 4)goto _LL2D7;else{_tmp63F=_tmp63E->f1;_tmp640=_tmp63E->f2;}}_LL2D6:
# 2878
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp63F,_tmp640);goto _LL2C8;_LL2D7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp641=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp641->tag != 3)goto _LL2D9;else{_tmp642=_tmp641->f1;_tmp643=_tmp641->f2;_tmp644=_tmp641->f3;}}_LL2D8:
# 2880
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp642,_tmp643,_tmp644);goto _LL2C8;_LL2D9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp645=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp645->tag != 5)goto _LL2DB;else{_tmp646=_tmp645->f1;_tmp647=_tmp645->f2;_tmp648=_tmp645->f3;}}_LL2DA:
# 2882
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp646,_tmp647,_tmp648);goto _LL2C8;_LL2DB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp649=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp649->tag != 6)goto _LL2DD;else{_tmp64A=_tmp649->f1;_tmp64B=_tmp649->f2;}}_LL2DC:
# 2884
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp64A,_tmp64B);goto _LL2C8;_LL2DD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp64C->tag != 7)goto _LL2DF;else{_tmp64D=_tmp64C->f1;_tmp64E=_tmp64C->f2;}}_LL2DE:
# 2886
 t=Cyc_Tcexp_tcOr(te,loc,_tmp64D,_tmp64E);goto _LL2C8;_LL2DF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp64F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp64F->tag != 8)goto _LL2E1;else{_tmp650=_tmp64F->f1;_tmp651=_tmp64F->f2;}}_LL2E0:
# 2888
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp650,_tmp651);goto _LL2C8;_LL2E1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp652=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp652->tag != 9)goto _LL2E3;else{_tmp653=_tmp652->f1;_tmp654=_tmp652->f2;_tmp655=(struct Cyc_Absyn_VarargCallInfo**)& _tmp652->f3;}}_LL2E2: {
# 2894
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp6AF;_push_handler(& _tmp6AF);{int _tmp6B1=0;if(setjmp(_tmp6AF.handler))_tmp6B1=1;if(!_tmp6B1){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp6B0=(void*)_exn_thrown;void*_tmp6B3=_tmp6B0;struct _dyneither_ptr _tmp6B5;void*_tmp6B6;_LL31C: {struct Cyc_Core_Failure_exn_struct*_tmp6B4=(struct Cyc_Core_Failure_exn_struct*)_tmp6B3;if(_tmp6B4->tag != Cyc_Core_Failure)goto _LL31E;else{_tmp6B5=_tmp6B4->f1;}}_LL31D:
# 2901
 ok=0;
fn_exp=e;
goto _LL31B;_LL31E: _tmp6B6=_tmp6B3;_LL31F:(void)_throw(_tmp6B6);_LL31B:;}};}
# 2905
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp653,_tmp654,_tmp655,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2912
goto _LL2C8;}_LL2E3: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp656=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp656->tag != 10)goto _LL2E5;else{_tmp657=_tmp656->f1;}}_LL2E4:
# 2914
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp657);goto _LL2C8;_LL2E5: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp658=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp658->tag != 12)goto _LL2E7;else{_tmp659=_tmp658->f1;_tmp65A=_tmp658->f2;}}_LL2E6:
# 2916
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp659,_tmp65A);goto _LL2C8;_LL2E7: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp65B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp65B->tag != 13)goto _LL2E9;else{_tmp65C=(void*)_tmp65B->f1;_tmp65D=_tmp65B->f2;_tmp65E=(enum Cyc_Absyn_Coercion*)& _tmp65B->f4;}}_LL2E8:
# 2918
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp65C,_tmp65D,_tmp65E);goto _LL2C8;_LL2E9: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp65F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp65F->tag != 14)goto _LL2EB;else{_tmp660=_tmp65F->f1;}}_LL2EA:
# 2920
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp660);goto _LL2C8;_LL2EB: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp661=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp661->tag != 15)goto _LL2ED;else{_tmp662=(struct Cyc_Absyn_Exp**)& _tmp661->f1;_tmp663=_tmp661->f2;}}_LL2EC:
# 2922
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp662,e,_tmp663);goto _LL2C8;_LL2ED: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp664->tag != 17)goto _LL2EF;else{_tmp665=_tmp664->f1;}}_LL2EE: {
# 2924
void*_tmp6B7=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp665);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp6B7);goto _LL2C8;}_LL2EF: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp666=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp666->tag != 16)goto _LL2F1;else{_tmp667=(void*)_tmp666->f1;}}_LL2F0:
# 2927
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp667);goto _LL2C8;_LL2F1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp668=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp668->tag != 18)goto _LL2F3;else{_tmp669=(void*)_tmp668->f1;_tmp66A=_tmp668->f2;}}_LL2F2:
# 2929
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp669,_tmp66A);goto _LL2C8;_LL2F3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp66B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp66B->tag != 19)goto _LL2F5;else{_tmp66C=_tmp66B->f1;}}_LL2F4:
# 2931
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp66C);goto _LL2C8;_LL2F5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp66D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp66D->tag != 20)goto _LL2F7;else{_tmp66E=_tmp66D->f1;_tmp66F=_tmp66D->f2;_tmp670=(int*)& _tmp66D->f3;_tmp671=(int*)& _tmp66D->f4;}}_LL2F6:
# 2933
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp66E,_tmp66F,_tmp670,_tmp671);goto _LL2C8;_LL2F7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp672->tag != 21)goto _LL2F9;else{_tmp673=_tmp672->f1;_tmp674=_tmp672->f2;_tmp675=(int*)& _tmp672->f3;_tmp676=(int*)& _tmp672->f4;}}_LL2F8:
# 2935
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp673,_tmp674,_tmp675,_tmp676);goto _LL2C8;_LL2F9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp677=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp677->tag != 22)goto _LL2FB;else{_tmp678=_tmp677->f1;_tmp679=_tmp677->f2;}}_LL2FA:
# 2937
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp678,_tmp679);goto _LL2C8;_LL2FB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp67A=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp67A->tag != 23)goto _LL2FD;else{_tmp67B=_tmp67A->f1;}}_LL2FC:
# 2939
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp67B);goto _LL2C8;_LL2FD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp67C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp67C->tag != 24)goto _LL2FF;else{_tmp67D=_tmp67C->f1;_tmp67E=_tmp67C->f2;}}_LL2FE:
# 2941
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp67D,_tmp67E);goto _LL2C8;_LL2FF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp67F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp67F->tag != 25)goto _LL301;else{_tmp680=_tmp67F->f1;}}_LL300: {
# 2945
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp6B8=Cyc_Tcutil_compress(*topt);void*_tmp6B9=_tmp6B8;void**_tmp6BB;struct Cyc_Absyn_Tqual*_tmp6BC;union Cyc_Absyn_Constraint*_tmp6BD;_LL321: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6BA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6B9;if(_tmp6BA->tag != 8)goto _LL323;else{_tmp6BB=(void**)&(_tmp6BA->f1).elt_type;_tmp6BC=(struct Cyc_Absyn_Tqual*)&(_tmp6BA->f1).tq;_tmp6BD=(_tmp6BA->f1).zero_term;}}_LL322:
# 2951
 elt_topt=_tmp6BB;
elt_tqopt=_tmp6BC;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6BD);
goto _LL320;_LL323:;_LL324:
 goto _LL320;_LL320:;}
# 2958
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp680);goto _LL2C8;}_LL301: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp681=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp681->tag != 36)goto _LL303;else{_tmp682=_tmp681->f1;}}_LL302:
# 2960
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp682);goto _LL2C8;_LL303: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp683=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp683->tag != 26)goto _LL305;else{_tmp684=_tmp683->f1;_tmp685=_tmp683->f2;_tmp686=_tmp683->f3;_tmp687=(int*)& _tmp683->f4;}}_LL304:
# 2962
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp684,_tmp685,_tmp686,_tmp687);goto _LL2C8;_LL305: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp688=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp688->tag != 27)goto _LL307;else{_tmp689=_tmp688->f1;_tmp68A=(void*)_tmp688->f2;_tmp68B=(int*)& _tmp688->f3;}}_LL306:
# 2964
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp689,_tmp68A,_tmp68B);goto _LL2C8;_LL307: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp68C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp68C->tag != 28)goto _LL309;else{_tmp68D=(struct _tuple1**)& _tmp68C->f1;_tmp68E=(struct Cyc_List_List**)& _tmp68C->f2;_tmp68F=_tmp68C->f3;_tmp690=(struct Cyc_Absyn_Aggrdecl**)& _tmp68C->f4;}}_LL308:
# 2966
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp68D,_tmp68E,_tmp68F,_tmp690);goto _LL2C8;_LL309: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp691=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp691->tag != 29)goto _LL30B;else{_tmp692=(void*)_tmp691->f1;_tmp693=_tmp691->f2;}}_LL30A:
# 2968
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp692,_tmp693);goto _LL2C8;_LL30B: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp694=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp694->tag != 30)goto _LL30D;else{_tmp695=_tmp694->f1;_tmp696=_tmp694->f2;_tmp697=_tmp694->f3;}}_LL30C:
# 2970
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp695,_tmp696,_tmp697);goto _LL2C8;_LL30D: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp698=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp698->tag != 31)goto _LL30F;else{_tmp699=(struct _tuple1**)& _tmp698->f1;_tmp69A=_tmp698->f2;_tmp69B=_tmp698->f3;}}_LL30E:
# 2972
*_tmp699=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp69B))->name;
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpA42;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA41;t=(void*)((_tmpA41=_cycalloc(sizeof(*_tmpA41)),((_tmpA41[0]=((_tmpA42.tag=13,((_tmpA42.f1=((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp69A))->name,((_tmpA42.f2=_tmp69A,_tmpA42)))))),_tmpA41))));}goto _LL2C8;_LL30F: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp69C=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp69C->tag != 32)goto _LL311;else{_tmp69D=(struct _tuple1**)& _tmp69C->f1;_tmp69E=(void*)_tmp69C->f2;_tmp69F=_tmp69C->f3;}}_LL310:
# 2975
*_tmp69D=((struct Cyc_Absyn_Enumfield*)_check_null(_tmp69F))->name;
t=_tmp69E;goto _LL2C8;_LL311: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp6A0=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp6A0->tag != 33)goto _LL313;else{_tmp6A1=(int*)&(_tmp6A0->f1).is_calloc;_tmp6A2=(struct Cyc_Absyn_Exp**)&(_tmp6A0->f1).rgn;_tmp6A3=(void***)&(_tmp6A0->f1).elt_type;_tmp6A4=(struct Cyc_Absyn_Exp**)&(_tmp6A0->f1).num_elts;_tmp6A5=(int*)&(_tmp6A0->f1).fat_result;}}_LL312:
# 2978
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp6A2,_tmp6A3,_tmp6A4,_tmp6A1,_tmp6A5);goto _LL2C8;_LL313: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp6A6=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp6A6->tag != 34)goto _LL315;else{_tmp6A7=_tmp6A6->f1;_tmp6A8=_tmp6A6->f2;}}_LL314:
# 2980
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp6A7,_tmp6A8);goto _LL2C8;_LL315: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp6A9=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp6A9->tag != 37)goto _LL317;else{_tmp6AA=_tmp6A9->f1;_tmp6AB=_tmp6A9->f2;}}_LL316:
# 2982
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp6AA,_tmp6AB);goto _LL2C8;_LL317: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6AC=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp6AC->tag != 38)goto _LL319;else{_tmp6AD=(void*)_tmp6AC->f1;}}_LL318:
# 2984
 if(!te->allow_valueof){
const char*_tmpA45;void*_tmpA44;(_tmpA44=0,Cyc_Tcutil_terr(e->loc,((_tmpA45="valueof(-) can only occur within types",_tag_dyneither(_tmpA45,sizeof(char),39))),_tag_dyneither(_tmpA44,sizeof(void*),0)));}
# 2992
t=Cyc_Absyn_sint_typ;
goto _LL2C8;_LL319: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp6AE=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp62B;if(_tmp6AE->tag != 39)goto _LL2C8;}_LL31A:
# 2995
{const char*_tmpA48;void*_tmpA47;(_tmpA47=0,Cyc_Tcutil_terr(e->loc,((_tmpA48="asm expressions cannot occur within Cyclone code.",_tag_dyneither(_tmpA48,sizeof(char),50))),_tag_dyneither(_tmpA47,sizeof(void*),0)));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));_LL2C8:;}
# 2998
e->topt=t;}
