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
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 381
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 322
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 431 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 433
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 919
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 923
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 929
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 932
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 937
void*Cyc_Absyn_compress_kb(void*);
# 942
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 944
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 947
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 949
extern void*Cyc_Absyn_sint_typ;
# 951
void*Cyc_Absyn_float_typ(int);
# 953
extern void*Cyc_Absyn_empty_effect;
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1097
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1106
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_RgnOrder_RgnPO;
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
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 69
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 80
int Cyc_Tcutil_coerceable(void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 130
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 193
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 238 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 241
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 58
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 60
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 62
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 52 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct _tuple2*_tmp3;struct _tuple2*_tmp5;struct Cyc_List_List*_tmp6;int _tmp7;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;int _tmpB;enum Cyc_Absyn_AggrKind _tmpD;struct _tuple2*_tmpE;struct Cyc_List_List*_tmpF;struct Cyc_List_List**_tmp10;struct Cyc_List_List*_tmp11;int _tmp12;struct Cyc_Absyn_Exp*_tmp14;_LL1: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp2->tag != 15)goto _LL3;else{_tmp3=_tmp2->f1;}}_LL2:
# 55
{struct _handler_cons _tmp15;_push_handler(& _tmp15);{int _tmp17=0;if(setjmp(_tmp15.handler))_tmp17=1;if(!_tmp17){
{struct _RegionHandle _tmp18=_new_region("r");struct _RegionHandle*r=& _tmp18;_push_region(r);
{void*_tmp19=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp3,0);void*_tmp1A=_tmp19;struct Cyc_Absyn_Datatypedecl*_tmp1D;struct Cyc_Absyn_Datatypefield*_tmp1E;struct Cyc_Absyn_Enumdecl*_tmp20;struct Cyc_Absyn_Enumfield*_tmp21;void*_tmp23;struct Cyc_Absyn_Enumfield*_tmp24;_LLE: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp1B=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1B->tag != 1)goto _LL10;}_LLF:
# 59
 p->r=(void*)& Cyc_Absyn_Wild_p_val;
{const char*_tmp3D6;void*_tmp3D5;(_tmp3D5=0,Cyc_Tcutil_terr(p->loc,((_tmp3D6="struct tag used without arguments in pattern",_tag_dyneither(_tmp3D6,sizeof(char),45))),_tag_dyneither(_tmp3D5,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1C->tag != 2)goto _LL12;else{_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3D9;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3D8;p->r=(void*)((_tmp3D8=_cycalloc(sizeof(*_tmp3D8)),((_tmp3D8[0]=((_tmp3D9.tag=8,((_tmp3D9.f1=_tmp1D,((_tmp3D9.f2=_tmp1E,((_tmp3D9.f3=0,((_tmp3D9.f4=0,_tmp3D9)))))))))),_tmp3D8))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1F->tag != 3)goto _LL14;else{_tmp20=_tmp1F->f1;_tmp21=_tmp1F->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3DC;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp3DB;p->r=(void*)((_tmp3DB=_cycalloc(sizeof(*_tmp3DB)),((_tmp3DB[0]=((_tmp3DC.tag=13,((_tmp3DC.f1=_tmp20,((_tmp3DC.f2=_tmp21,_tmp3DC)))))),_tmp3DB))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp22=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp22->tag != 4)goto _LL16;else{_tmp23=(void*)_tmp22->f1;_tmp24=_tmp22->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3DF;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp3DE;p->r=(void*)((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE[0]=((_tmp3DF.tag=14,((_tmp3DF.f1=_tmp23,((_tmp3DF.f2=_tmp24,_tmp3DF)))))),_tmp3DE))));}
_npop_handler(1);return;_LL16: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp25=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp25->tag != 0)goto _LLD;}_LL17:
 goto _LLD;_LLD:;}
# 57
;_pop_region(r);}
# 56
;_pop_handler();}else{void*_tmp16=(void*)_exn_thrown;void*_tmp2F=_tmp16;void*_tmp31;_LL19: {struct Cyc_Dict_Absent_exn_struct*_tmp30=(struct Cyc_Dict_Absent_exn_struct*)_tmp2F;if(_tmp30->tag != Cyc_Dict_Absent)goto _LL1B;}_LL1A:
# 73
 goto _LL18;_LL1B: _tmp31=_tmp2F;_LL1C:(void)_throw(_tmp31);_LL18:;}};}{
# 75
union Cyc_Absyn_Nmspace _tmp32=(*_tmp3).f1;union Cyc_Absyn_Nmspace _tmp33=_tmp32;_LL1E: if((_tmp33.Loc_n).tag != 4)goto _LL20;_LL1F:
 goto _LL21;_LL20: if((_tmp33.Rel_n).tag != 1)goto _LL22;if((struct Cyc_List_List*)(_tmp33.Rel_n).val != 0)goto _LL22;_LL21:
# 78
(*_tmp3).f1=Cyc_Absyn_Loc_n;
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3E2;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp3E1;p->r=(void*)((_tmp3E1=_cycalloc(sizeof(*_tmp3E1)),((_tmp3E1[0]=((_tmp3E2.tag=1,((_tmp3E2.f1=Cyc_Absyn_new_vardecl(_tmp3,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp3E2.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3E2)))))),_tmp3E1))));}
return;_LL22:;_LL23:
{const char*_tmp3E5;void*_tmp3E4;(_tmp3E4=0,Cyc_Tcutil_terr(p->loc,((_tmp3E5="qualified variable in pattern",_tag_dyneither(_tmp3E5,sizeof(char),30))),_tag_dyneither(_tmp3E4,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp4->tag != 16)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;_tmp7=_tmp4->f3;}}_LL4:
# 85
{struct _handler_cons _tmp38;_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!_tmp3A){
{struct _RegionHandle _tmp3B=_new_region("r");struct _RegionHandle*r=& _tmp3B;_push_region(r);
{void*_tmp3C=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp5,0);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Aggrdecl*_tmp3F;struct Cyc_Absyn_Datatypedecl*_tmp41;struct Cyc_Absyn_Datatypefield*_tmp42;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3E=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL27;else{_tmp3F=_tmp3E->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp46=0;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct _tuple15*_tmp3E8;struct Cyc_List_List*_tmp3E7;_tmp46=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=((_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8->f1=0,((_tmp3E8->f2=(struct Cyc_Absyn_Pat*)_tmp6->hd,_tmp3E8)))))),((_tmp3E7->tl=_tmp46,_tmp3E7))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3F2;struct Cyc_Absyn_Aggrdecl**_tmp3F1;struct Cyc_Absyn_AggrInfo*_tmp3F0;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3EF;p->r=(void*)((_tmp3EF=_cycalloc(sizeof(*_tmp3EF)),((_tmp3EF[0]=((_tmp3F2.tag=7,((_tmp3F2.f1=((_tmp3F0=_cycalloc(sizeof(*_tmp3F0)),((_tmp3F0->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3F1=_cycalloc(sizeof(*_tmp3F1)),((_tmp3F1[0]=_tmp3F,_tmp3F1))))),((_tmp3F0->targs=0,_tmp3F0)))))),((_tmp3F2.f2=0,((_tmp3F2.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46),((_tmp3F2.f4=_tmp7,_tmp3F2)))))))))),_tmp3EF))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp40->tag != 2)goto _LL29;else{_tmp41=_tmp40->f1;_tmp42=_tmp40->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3F5;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3F4;p->r=(void*)((_tmp3F4=_cycalloc(sizeof(*_tmp3F4)),((_tmp3F4[0]=((_tmp3F5.tag=8,((_tmp3F5.f1=_tmp41,((_tmp3F5.f2=_tmp42,((_tmp3F5.f3=_tmp6,((_tmp3F5.f4=_tmp7,_tmp3F5)))))))))),_tmp3F4))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp43=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp43->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp44=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp44->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp3F8;void*_tmp3F7;(_tmp3F7=0,Cyc_Tcutil_terr(p->loc,((_tmp3F8="enum tag used with arguments in pattern",_tag_dyneither(_tmp3F8,sizeof(char),40))),_tag_dyneither(_tmp3F7,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp45=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp45->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp39=(void*)_exn_thrown;void*_tmp52=_tmp39;void*_tmp54;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp53=(struct Cyc_Dict_Absent_exn_struct*)_tmp52;if(_tmp53->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp54=_tmp52;_LL33:(void)_throw(_tmp54);_LL2F:;}};}
{const char*_tmp3FC;void*_tmp3FB[1];struct Cyc_String_pa_PrintArg_struct _tmp3FA;(_tmp3FA.tag=0,((_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp3FB[0]=& _tmp3FA,Cyc_Tcutil_terr(p->loc,((_tmp3FC="%s is not a constructor in pattern",_tag_dyneither(_tmp3FC,sizeof(char),35))),_tag_dyneither(_tmp3FB,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp8->tag != 7)goto _LL7;else{if(_tmp8->f1 != 0)goto _LL7;_tmp9=_tmp8->f2;_tmpA=_tmp8->f3;_tmpB=_tmp8->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp3FF;void*_tmp3FE;(_tmp3FE=0,Cyc_Tcutil_terr(p->loc,((_tmp3FF="cannot determine pattern type",_tag_dyneither(_tmp3FF,sizeof(char),30))),_tag_dyneither(_tmp3FE,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp5A=t;struct Cyc_Absyn_AggrInfo _tmp5C;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A;if(_tmp5B->tag != 11)goto _LL37;else{_tmp5C=_tmp5B->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp405;struct Cyc_Absyn_AggrInfo*_tmp404;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp403;p->r=(void*)((_tmp403=_cycalloc(sizeof(*_tmp403)),((_tmp403[0]=((_tmp405.tag=7,((_tmp405.f1=((_tmp404=_cycalloc(sizeof(*_tmp404)),((_tmp404[0]=_tmp5C,_tmp404)))),((_tmp405.f2=_tmp9,((_tmp405.f3=_tmpA,((_tmp405.f4=_tmpB,_tmp405)))))))))),_tmp403))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp409;void*_tmp408[1];struct Cyc_String_pa_PrintArg_struct _tmp407;(_tmp407.tag=0,((_tmp407.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp408[0]=& _tmp407,Cyc_Tcutil_terr(p->loc,((_tmp409="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp409,sizeof(char),45))),_tag_dyneither(_tmp408,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmpC->tag != 7)goto _LL9;else{if(_tmpC->f1 == 0)goto _LL9;if((((_tmpC->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpD=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f1;_tmpE=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f2;_tmpF=(_tmpC->f1)->targs;_tmp10=(struct Cyc_List_List**)& _tmpC->f2;_tmp11=_tmpC->f3;_tmp12=_tmpC->f4;}}_LL8:
# 126
{struct _handler_cons _tmp63;_push_handler(& _tmp63);{int _tmp65=0;if(setjmp(_tmp63.handler))_tmp65=1;if(!_tmp65){
{struct Cyc_Absyn_Aggrdecl**_tmp66=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);
struct Cyc_Absyn_Aggrdecl*_tmp67=*_tmp66;
if(_tmp67->impl == 0){
{const char*_tmp411;void*_tmp410[1];const char*_tmp40F;const char*_tmp40E;struct Cyc_String_pa_PrintArg_struct _tmp40D;(_tmp40D.tag=0,((_tmp40D.f1=(struct _dyneither_ptr)(
_tmp67->kind == Cyc_Absyn_StructA?(_tmp40E="struct",_tag_dyneither(_tmp40E,sizeof(char),7)):((_tmp40F="union",_tag_dyneither(_tmp40F,sizeof(char),6)))),((_tmp410[0]=& _tmp40D,Cyc_Tcutil_terr(p->loc,((_tmp411="can't destructure an abstract %s",_tag_dyneither(_tmp411,sizeof(char),33))),_tag_dyneither(_tmp410,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp10);
if(more_exists < 0){
{const char*_tmp414;void*_tmp413;(_tmp413=0,Cyc_Tcutil_terr(p->loc,((_tmp414="too many existentially bound type variables in pattern",_tag_dyneither(_tmp414,sizeof(char),55))),_tag_dyneither(_tmp413,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6F=_tmp10;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6F=&((struct Cyc_List_List*)_check_null(*_tmp6F))->tl;}}
*_tmp6F=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp70=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp41A;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp419;struct Cyc_List_List*_tmp418;_tmp70=((_tmp418=_cycalloc(sizeof(*_tmp418)),((_tmp418->hd=Cyc_Tcutil_new_tvar((void*)((_tmp41A=_cycalloc(sizeof(*_tmp41A)),((_tmp41A[0]=((_tmp419.tag=1,((_tmp419.f1=0,_tmp419)))),_tmp41A))))),((_tmp418->tl=_tmp70,_tmp418))))));}
*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp10,_tmp70);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp420;struct Cyc_Absyn_AggrInfo*_tmp41F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp41E;p->r=(void*)((_tmp41E=_cycalloc(sizeof(*_tmp41E)),((_tmp41E[0]=((_tmp420.tag=7,((_tmp420.f1=((_tmp41F=_cycalloc(sizeof(*_tmp41F)),((_tmp41F->aggr_info=Cyc_Absyn_KnownAggr(_tmp66),((_tmp41F->targs=_tmpF,_tmp41F)))))),((_tmp420.f2=*_tmp10,((_tmp420.f3=_tmp11,((_tmp420.f4=_tmp12,_tmp420)))))))))),_tmp41E))));};};}
# 127
;_pop_handler();}else{void*_tmp64=(void*)_exn_thrown;void*_tmp78=_tmp64;void*_tmp7A;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp423;void*_tmp422;(_tmp422=0,Cyc_Tcutil_terr(p->loc,((_tmp423="Non-aggregate name has designator patterns",_tag_dyneither(_tmp423,sizeof(char),43))),_tag_dyneither(_tmp422,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp7A=_tmp78;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp13=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp13->tag != 17)goto _LLB;else{_tmp14=_tmp13->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp14);
if(!Cyc_Tcutil_is_const_exp(_tmp14)){
{const char*_tmp426;void*_tmp425;(_tmp425=0,Cyc_Tcutil_terr(p->loc,((_tmp426="non-constant expression in case pattern",_tag_dyneither(_tmp426,sizeof(char),40))),_tag_dyneither(_tmp425,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp7F=Cyc_Evexp_eval_const_uint_exp(_tmp14);unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=_tmp7F;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp429;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp428;p->r=(void*)((_tmp428=_cycalloc_atomic(sizeof(*_tmp428)),((_tmp428[0]=((_tmp429.tag=10,((_tmp429.f1=Cyc_Absyn_None,((_tmp429.f2=(int)_tmp81,_tmp429)))))),_tmp428))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp42A;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A->v=s,_tmp42A)))));};}
# 178
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 182
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 185
{void*_tmp86=Cyc_Tcutil_compress(numt);void*_tmp87=_tmp86;_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp87;if(_tmp88->tag != 13)goto _LL41;}_LL40:
 goto _LL42;_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp87;if(_tmp89->tag != 14)goto _LL43;}_LL42:
 if(topt != 0)return*topt;goto _LL3E;_LL43:;_LL44:
 goto _LL3E;_LL3E:;}
# 190
return numt;}struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 193
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 195
if(vd != 0){
(*vd)->type=t;
(*vd)->tq=Cyc_Absyn_empty_tqual(0);}{
# 199
struct _tuple16*_tmp42D;struct Cyc_List_List*_tmp42C;*v_result_ptr=((_tmp42C=_cycalloc(sizeof(*_tmp42C)),((_tmp42C->hd=((_tmp42D=_cycalloc(sizeof(*_tmp42D)),((_tmp42D->f1=vd,((_tmp42D->f2=e,_tmp42D)))))),((_tmp42C->tl=*v_result_ptr,_tmp42C))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=res1;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;{
struct _tuple1*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_Tcpat_TcPatResult _tmp8F=res2;_tmp90=_tmp8F.tvars_and_bounds_opt;_tmp91=_tmp8F.patvars;
if(_tmp8D != 0  || _tmp90 != 0){
if(_tmp8D == 0){
struct _tuple1*_tmp42E;_tmp8D=((_tmp42E=_cycalloc(sizeof(*_tmp42E)),((_tmp42E->f1=0,((_tmp42E->f2=0,_tmp42E))))));}
if(_tmp90 == 0){
struct _tuple1*_tmp42F;_tmp90=((_tmp42F=_cycalloc(sizeof(*_tmp42F)),((_tmp42F->f1=0,((_tmp42F->f2=0,_tmp42F))))));}{
struct _tuple1*_tmp432;struct Cyc_Tcpat_TcPatResult _tmp431;return(_tmp431.tvars_and_bounds_opt=((_tmp432=_cycalloc(sizeof(*_tmp432)),((_tmp432->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f1,(*_tmp90).f1),((_tmp432->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f2,(*_tmp90).f2),_tmp432)))))),((_tmp431.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp431)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp433;return(_tmp433.tvars_and_bounds_opt=0,((_tmp433.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp433)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp434;return(_tmp434=_cycalloc(sizeof(*_tmp434)),((_tmp434->loc=loc,((_tmp434->topt=0,((_tmp434->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp434)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp435;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435->v=Cyc_Tcenv_lookup_type_vars(te),_tmp435))))): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 229
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
struct _tuple17*_tmp436;return(_tmp436=_cycalloc(sizeof(*_tmp436)),((_tmp436->f1=tv,((_tmp436->f2=0,_tmp436)))));}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 233
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 236
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp437;struct Cyc_Tcpat_TcPatResult res=(_tmp437.tvars_and_bounds_opt=0,((_tmp437.patvars=0,_tmp437)));
# 241
{void*_tmp9A=p->r;void*_tmp9B=_tmp9A;struct Cyc_Absyn_Vardecl*_tmp9E;struct Cyc_Absyn_Pat*_tmp9F;struct Cyc_Absyn_Tvar*_tmpA1;struct Cyc_Absyn_Vardecl*_tmpA2;struct Cyc_Absyn_Vardecl*_tmpA4;struct Cyc_Absyn_Pat*_tmpA5;struct Cyc_Absyn_Tvar*_tmpA7;struct Cyc_Absyn_Vardecl*_tmpA8;int _tmpAE;struct Cyc_Absyn_Enumdecl*_tmpB0;void*_tmpB2;struct Cyc_Absyn_Pat*_tmpB5;struct Cyc_List_List**_tmpB7;int _tmpB8;struct Cyc_Absyn_Aggrdecl*_tmpBA;struct Cyc_List_List**_tmpBB;struct Cyc_List_List*_tmpBC;struct Cyc_List_List**_tmpBD;int _tmpBE;struct Cyc_Absyn_Datatypedecl*_tmpC0;struct Cyc_Absyn_Datatypefield*_tmpC1;struct Cyc_List_List**_tmpC2;int _tmpC3;_LL46: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp9C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9C->tag != 0)goto _LL48;}_LL47:
# 244
 if(topt != 0)
t=*topt;else{
# 247
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
goto _LL45;_LL48: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmp9D->tag != 1)goto _LL4A;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LL49:
# 250
 res=Cyc_Tcpat_tcPatRec(te,_tmp9F,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmp9F->topt);
# 254
{void*_tmpC9=Cyc_Tcutil_compress(t);void*_tmpCA=_tmpC9;_LL73: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCA;if(_tmpCB->tag != 8)goto _LL75;}_LL74:
# 256
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp43A;void*_tmp439;(_tmp439=0,Cyc_Tcutil_terr(p->loc,((_tmp43A="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp43A,sizeof(char),58))),_tag_dyneither(_tmp439,sizeof(void*),0)));}
goto _LL72;_LL75:;_LL76:
# 260
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp43D;void*_tmp43C;(_tmp43C=0,Cyc_Tcutil_terr(p->loc,((_tmp43D="pattern would point to an abstract member",_tag_dyneither(_tmp43D,sizeof(char),42))),_tag_dyneither(_tmp43C,sizeof(void*),0)));}
goto _LL72;_LL72:;}
# 264
{struct Cyc_Absyn_Vardecl**_tmp43E;Cyc_Tcpat_set_vd(((_tmp43E=_cycalloc(sizeof(*_tmp43E)),((_tmp43E[0]=_tmp9E,_tmp43E)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL45;_LL4A: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA0=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA0->tag != 2)goto _LL4C;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;}}_LL4B: {
# 267
struct _RegionHandle _tmpD1=_new_region("r2");struct _RegionHandle*r2=& _tmpD1;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp43F[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp43F[0]=_tmpA1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp43F,sizeof(struct Cyc_Absyn_Tvar*),1)))));
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp440;res.tvars_and_bounds_opt=((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->f1=0,((_tmp440->f2=0,_tmp440))))));}
{struct _tuple17*_tmp443;struct Cyc_List_List*_tmp442;(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((_tmp442=_cycalloc(sizeof(*_tmp442)),((_tmp442->hd=((_tmp443=_cycalloc(sizeof(*_tmp443)),((_tmp443->f1=_tmpA1,((_tmp443->f2=1,_tmp443)))))),((_tmp442->tl=0,_tmp442)))))));}
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,_tmpA2->type);}
# 277
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmpD6=Cyc_Tcutil_compress(t);void*_tmpD7=_tmpD6;_LL78: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 8)goto _LL7A;}_LL79:
# 281
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp446;void*_tmp445;(_tmp445=0,Cyc_Tcutil_terr(p->loc,((_tmp446="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp446,sizeof(char),58))),_tag_dyneither(_tmp445,sizeof(void*),0)));}
goto _LL77;_LL7A:;_LL7B:
# 285
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp449;void*_tmp448;(_tmp448=0,Cyc_Tcutil_terr(p->loc,((_tmp449="pattern would point to an abstract member",_tag_dyneither(_tmp449,sizeof(char),42))),_tag_dyneither(_tmp448,sizeof(void*),0)));}
goto _LL77;_LL77:;}
# 289
{struct Cyc_Absyn_Vardecl**_tmp44A;Cyc_Tcpat_set_vd(((_tmp44A=_cycalloc(sizeof(*_tmp44A)),((_tmp44A[0]=_tmpA2,_tmp44A)))),access_exp,& res.patvars,_tmpA2->type);}
_npop_handler(0);goto _LL45;
# 267
;_pop_region(r2);}_LL4C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA3->tag != 3)goto _LL4E;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;}}_LL4D:
# 293
 res=Cyc_Tcpat_tcPatRec(te,_tmpA5,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA5->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp44D;void*_tmp44C;(_tmp44C=0,Cyc_Tcutil_terr(p->loc,((_tmp44D="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp44D,sizeof(char),55))),_tag_dyneither(_tmp44C,sizeof(void*),0)));}
goto _LL45;}else{
# 300
struct _RegionHandle _tmpE0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE0;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp450;void*_tmp44F;(_tmp44F=0,Cyc_Tcutil_terr(p->loc,((_tmp450="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp450,sizeof(char),56))),_tag_dyneither(_tmp44F,sizeof(void*),0)));}
# 301
;_pop_region(rgn);}{
# 306
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp456;struct Cyc_Absyn_PtrInfo _tmp455;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp454;void*t2=(void*)((_tmp454=_cycalloc(sizeof(*_tmp454)),((_tmp454[0]=((_tmp456.tag=5,((_tmp456.f1=((_tmp455.elt_typ=t,((_tmp455.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp455.ptr_atts=(
((_tmp455.ptr_atts).rgn=rgn_pat,(((_tmp455.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp455.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp455.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp455.ptr_atts).ptrloc=0,_tmp455.ptr_atts)))))))))),_tmp455)))))),_tmp456)))),_tmp454))));
# 311
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 315
{struct Cyc_Absyn_Vardecl**_tmp457;Cyc_Tcpat_set_vd(((_tmp457=_cycalloc(sizeof(*_tmp457)),((_tmp457[0]=_tmpA4,_tmp457)))),new_access_exp,& res.patvars,t2);}
goto _LL45;};_LL4E: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA6->tag != 4)goto _LL50;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}_LL4F:
# 321
{struct Cyc_Absyn_Vardecl**_tmp458;Cyc_Tcpat_set_vd(((_tmp458=_cycalloc(sizeof(*_tmp458)),((_tmp458[0]=_tmpA8,_tmp458)))),access_exp,& res.patvars,_tmpA8->type);}{
# 325
struct _RegionHandle _tmpE8=_new_region("r2");struct _RegionHandle*r2=& _tmpE8;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp459[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp459[0]=_tmpA7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp459,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 328
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp45A;res.tvars_and_bounds_opt=((_tmp45A=_cycalloc(sizeof(*_tmp45A)),((_tmp45A->f1=0,((_tmp45A->f2=0,_tmp45A))))));}
{struct _tuple17*_tmp45D;struct Cyc_List_List*_tmp45C;(*res.tvars_and_bounds_opt).f1=(
(_tmp45C=_cycalloc(sizeof(*_tmp45C)),((_tmp45C->hd=((_tmp45D=_cycalloc(sizeof(*_tmp45D)),((_tmp45D->f1=_tmpA7,((_tmp45D->f2=0,_tmp45D)))))),((_tmp45C->tl=(*res.tvars_and_bounds_opt).f1,_tmp45C))))));}
t=Cyc_Absyn_uint_typ;
_npop_handler(0);goto _LL45;
# 325
;_pop_region(r2);};_LL50: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpA9=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA9->tag != 10)goto _LL52;else{if(_tmpA9->f1 != Cyc_Absyn_Unsigned)goto _LL52;}}_LL51:
# 335
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);goto _LL45;_LL52: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAA=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAA->tag != 10)goto _LL54;else{if(_tmpAA->f1 != Cyc_Absyn_None)goto _LL54;}}_LL53:
 goto _LL55;_LL54: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmpAB=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAB->tag != 10)goto _LL56;else{if(_tmpAB->f1 != Cyc_Absyn_Signed)goto _LL56;}}_LL55:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);goto _LL45;_LL56: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmpAC=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAC->tag != 11)goto _LL58;}_LL57:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);goto _LL45;_LL58: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmpAD=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAD->tag != 12)goto _LL5A;else{_tmpAE=_tmpAD->f2;}}_LL59:
 t=Cyc_Tcpat_num_type(topt,Cyc_Absyn_float_typ(_tmpAE));goto _LL45;_LL5A: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmpAF=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpAF->tag != 13)goto _LL5C;else{_tmpB0=_tmpAF->f1;}}_LL5B:
# 341
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp460;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp45F;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp45F=_cycalloc(sizeof(*_tmp45F)),((_tmp45F[0]=((_tmp460.tag=13,((_tmp460.f1=_tmpB0->name,((_tmp460.f2=_tmpB0,_tmp460)))))),_tmp45F)))));}
goto _LL45;_LL5C: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB1->tag != 14)goto _LL5E;else{_tmpB2=(void*)_tmpB1->f1;}}_LL5D:
 t=Cyc_Tcpat_num_type(topt,_tmpB2);goto _LL45;_LL5E: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB3=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB3->tag != 9)goto _LL60;}_LL5F:
# 345
 if(topt != 0){
void*_tmpEF=Cyc_Tcutil_compress(*topt);void*_tmpF0=_tmpEF;_LL7D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF0;if(_tmpF1->tag != 5)goto _LL7F;}_LL7E:
 t=*topt;goto tcpat_end;_LL7F:;_LL80:
 goto _LL7C;_LL7C:;}{
# 350
struct Cyc_Core_Opt*_tmpF2=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp466;struct Cyc_Absyn_PtrInfo _tmp465;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp464;t=(void*)((_tmp464=_cycalloc(sizeof(*_tmp464)),((_tmp464[0]=((_tmp466.tag=5,((_tmp466.f1=((_tmp465.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpF2),((_tmp465.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp465.ptr_atts=(
((_tmp465.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpF2),(((_tmp465.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp465.ptr_atts).bounds=
# 355
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp465.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp465.ptr_atts).ptrloc=0,_tmp465.ptr_atts)))))))))),_tmp465)))))),_tmp466)))),_tmp464))));}
# 358
goto _LL45;};_LL60: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmpB4=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB4->tag != 6)goto _LL62;else{_tmpB5=_tmpB4->f1;}}_LL61: {
# 363
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmpF6=0;
int elt_const=0;
if(topt != 0){
void*_tmpF7=Cyc_Tcutil_compress(*topt);void*_tmpF8=_tmpF7;void*_tmpFA;struct Cyc_Absyn_Tqual _tmpFB;_LL82: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF8;if(_tmpF9->tag != 5)goto _LL84;else{_tmpFA=(_tmpF9->f1).elt_typ;_tmpFB=(_tmpF9->f1).elt_tq;}}_LL83:
# 369
 inner_typ=_tmpFA;
_tmpF6=& inner_typ;
elt_const=_tmpFB.real_const;
goto _LL81;_LL84:;_LL85:
 goto _LL81;_LL81:;}{
# 378
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)new_access_exp=Cyc_Absyn_deref_exp(access_exp,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmpB5,_tmpF6,ptr_rgn,1,new_access_exp));
# 387
{void*_tmpFC=Cyc_Tcutil_compress((void*)_check_null(_tmpB5->topt));void*_tmpFD=_tmpFC;struct Cyc_Absyn_Datatypedecl*_tmpFF;struct Cyc_Absyn_Datatypefield*_tmp100;struct Cyc_List_List*_tmp101;_LL87: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpFE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpFD;if(_tmpFE->tag != 4)goto _LL89;else{if((((_tmpFE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL89;_tmpFF=((struct _tuple3)(((_tmpFE->f1).field_info).KnownDatatypefield).val).f1;_tmp100=((struct _tuple3)(((_tmpFE->f1).field_info).KnownDatatypefield).val).f2;_tmp101=(_tmpFE->f1).targs;}}_LL88:
# 391
{void*_tmp102=Cyc_Tcutil_compress(inner_typ);void*_tmp103=_tmp102;_LL8C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp104=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp103;if(_tmp104->tag != 4)goto _LL8E;}_LL8D:
# 393
 goto DONT_PROMOTE;_LL8E:;_LL8F:
 goto _LL8B;_LL8B:;}{
# 397
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp470;struct Cyc_Absyn_Datatypedecl**_tmp46F;struct Cyc_Absyn_DatatypeInfo _tmp46E;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp46D;void*new_type=(void*)((_tmp46D=_cycalloc(sizeof(*_tmp46D)),((_tmp46D[0]=((_tmp470.tag=3,((_tmp470.f1=((_tmp46E.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=_tmpFF,_tmp46F))))),((_tmp46E.targs=_tmp101,_tmp46E)))),_tmp470)))),_tmp46D))));
# 400
_tmpB5->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp476;struct Cyc_Absyn_PtrInfo _tmp475;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp474;t=(void*)((_tmp474=_cycalloc(sizeof(*_tmp474)),((_tmp474[0]=((_tmp476.tag=5,((_tmp476.f1=((_tmp475.elt_typ=new_type,((_tmp475.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp475.ptr_atts=(
((_tmp475.ptr_atts).rgn=ptr_rgn,(((_tmp475.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp475.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp475.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp475.ptr_atts).ptrloc=0,_tmp475.ptr_atts)))))))))),_tmp475)))))),_tmp476)))),_tmp474))));}
# 407
goto _LL86;};_LL89:;_LL8A:
# 409
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp47C;struct Cyc_Absyn_PtrInfo _tmp47B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47A;t=(void*)((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A[0]=((_tmp47C.tag=5,((_tmp47C.f1=((_tmp47B.elt_typ=(void*)_check_null(_tmpB5->topt),((_tmp47B.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp47B.ptr_atts=(
((_tmp47B.ptr_atts).rgn=ptr_rgn,(((_tmp47B.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp47B.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp47B.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp47B.ptr_atts).ptrloc=0,_tmp47B.ptr_atts)))))))))),_tmp47B)))))),_tmp47C)))),_tmp47A))));}_LL86:;}
# 417
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpB5->topt;
goto _LL45;};}_LL62: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmpB6=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB6->tag != 5)goto _LL64;else{_tmpB7=(struct Cyc_List_List**)& _tmpB6->f1;_tmpB8=_tmpB6->f2;}}_LL63: {
# 421
struct Cyc_List_List*_tmp10F=*_tmpB7;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp110=Cyc_Tcutil_compress(*topt);void*_tmp111=_tmp110;struct Cyc_List_List*_tmp113;_LL91: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp112=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp111;if(_tmp112->tag != 10)goto _LL93;else{_tmp113=_tmp112->f1;}}_LL92:
# 427
 topt_ts=_tmp113;
if(_tmpB8){
# 430
int _tmp114=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10F);
int _tmp115=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp113);
if(_tmp114 < _tmp115){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp115 - _tmp114;++ i){
struct Cyc_List_List*_tmp47D;wild_ps=((_tmp47D=_cycalloc(sizeof(*_tmp47D)),((_tmp47D->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp47D->tl=wild_ps,_tmp47D))))));}}
*_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10F,wild_ps);
_tmp10F=*_tmpB7;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10F)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp113)){
const char*_tmp480;void*_tmp47F;(_tmp47F=0,Cyc_Tcutil_warn(p->loc,((_tmp480="unnecessary ... in tuple pattern",_tag_dyneither(_tmp480,sizeof(char),33))),_tag_dyneither(_tmp47F,sizeof(void*),0)));}}}
# 442
goto _LL90;_LL93:;_LL94:
# 444
 goto _LL90;_LL90:;}else{
# 446
if(_tmpB8){
const char*_tmp483;void*_tmp482;(_tmp482=0,Cyc_Tcutil_terr(p->loc,((_tmp483="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp483,sizeof(char),57))),_tag_dyneither(_tmp482,sizeof(void*),0)));}}
{int i=0;for(0;_tmp10F != 0;(_tmp10F=_tmp10F->tl,i ++)){
void**_tmp11B=0;
if(topt_ts != 0){
_tmp11B=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 454
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp486;union Cyc_Absyn_Cnst _tmp485;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp485.Int_c).val=((_tmp486.f1=Cyc_Absyn_Unsigned,((_tmp486.f2=i,_tmp486)))),(((_tmp485.Int_c).tag=5,_tmp485)))),0),0);}
# 459
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp10F->hd,_tmp11B,rgn_pat,allow_ref_pat,new_access_exp));
# 462
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp10F->hd)->topt;{
struct _tuple18*_tmp489;struct Cyc_List_List*_tmp488;pat_ts=((_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=((_tmp489=_cycalloc(sizeof(*_tmp489)),((_tmp489->f1=Cyc_Absyn_empty_tqual(0),((_tmp489->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp10F->hd)->topt),_tmp489)))))),((_tmp488->tl=pat_ts,_tmp488))))));};};}}
# 465
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp48C;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp48B;t=(void*)((_tmp48B=_cycalloc(sizeof(*_tmp48B)),((_tmp48B[0]=((_tmp48C.tag=10,((_tmp48C.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp48C)))),_tmp48B))));}
goto _LL45;}_LL64: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB9->tag != 7)goto _LL66;else{if(_tmpB9->f1 == 0)goto _LL66;if((((_tmpB9->f1)->aggr_info).KnownAggr).tag != 2)goto _LL66;_tmpBA=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB9->f1)->aggr_info).KnownAggr).val);_tmpBB=(struct Cyc_List_List**)&(_tmpB9->f1)->targs;_tmpBC=_tmpB9->f2;_tmpBD=(struct Cyc_List_List**)& _tmpB9->f3;_tmpBE=_tmpB9->f4;}}_LL65: {
# 469
struct Cyc_List_List*_tmp122=*_tmpBD;
const char*_tmp48E;const char*_tmp48D;struct _dyneither_ptr aggr_str=_tmpBA->kind == Cyc_Absyn_StructA?(_tmp48E="struct",_tag_dyneither(_tmp48E,sizeof(char),7)):((_tmp48D="union",_tag_dyneither(_tmp48D,sizeof(char),6)));
if(_tmpBA->impl == 0){
{const char*_tmp492;void*_tmp491[1];struct Cyc_String_pa_PrintArg_struct _tmp490;(_tmp490.tag=0,((_tmp490.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp491[0]=& _tmp490,Cyc_Tcutil_terr(p->loc,((_tmp492="can't destructure an abstract %s",_tag_dyneither(_tmp492,sizeof(char),33))),_tag_dyneither(_tmp491,sizeof(void*),1)))))));}
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));
goto _LL45;}
# 478
if(_tmpBA->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->tagged)
allow_ref_pat=0;
if(_tmpBC != 0){
# 484
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}{
# 487
struct _RegionHandle _tmp126=_new_region("rgn");struct _RegionHandle*rgn=& _tmp126;_push_region(rgn);{
# 489
struct Cyc_List_List*_tmp127=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmp128=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->exist_vars;
{struct Cyc_List_List*t=_tmpBC;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp128))->hd;
_tmp128=_tmp128->tl;{
void*_tmp129=Cyc_Absyn_compress_kb(tv->kind);
void*_tmp12A=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmp12B=_tmp12A;struct Cyc_Absyn_Kind*_tmp12D;struct Cyc_Absyn_Kind*_tmp12F;_LL96: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp12B;if(_tmp12C->tag != 2)goto _LL98;else{_tmp12D=_tmp12C->f2;}}_LL97:
 _tmp12F=_tmp12D;goto _LL99;_LL98: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp12E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp12B;if(_tmp12E->tag != 0)goto _LL9A;else{_tmp12F=_tmp12E->f1;}}_LL99:
 k2=_tmp12F;goto _LL95;_LL9A:;_LL9B: {
const char*_tmp495;void*_tmp494;(_tmp494=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp495="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp495,sizeof(char),53))),_tag_dyneither(_tmp494,sizeof(void*),0)));}_LL95:;}
# 505
{void*_tmp132=_tmp129;struct Cyc_Absyn_Kind*_tmp134;struct Cyc_Core_Opt**_tmp136;struct Cyc_Absyn_Kind*_tmp137;struct Cyc_Core_Opt**_tmp139;_LL9D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp133=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp133->tag != 0)goto _LL9F;else{_tmp134=_tmp133->f1;}}_LL9E:
# 508
 if(!Cyc_Tcutil_kind_leq(k2,_tmp134))
error=1;
goto _LL9C;_LL9F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp135=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp135->tag != 2)goto _LLA1;else{_tmp136=(struct Cyc_Core_Opt**)& _tmp135->f1;_tmp137=_tmp135->f2;}}_LLA0:
 _tmp139=_tmp136;goto _LLA2;_LLA1: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp138=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp138->tag != 1)goto _LL9C;else{_tmp139=(struct Cyc_Core_Opt**)& _tmp138->f1;}}_LLA2:
{struct Cyc_Core_Opt*_tmp496;*_tmp139=((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496->v=_tmp12A,_tmp496))));}goto _LL9C;_LL9C:;}
# 514
if(error){
const char*_tmp49C;void*_tmp49B[3];struct Cyc_String_pa_PrintArg_struct _tmp49A;struct Cyc_String_pa_PrintArg_struct _tmp499;struct Cyc_String_pa_PrintArg_struct _tmp498;(_tmp498.tag=0,((_tmp498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 518
Cyc_Absynpp_kind2string(k2)),((_tmp499.tag=0,((_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp129)),((_tmp49A.tag=0,((_tmp49A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp49B[0]=& _tmp49A,((_tmp49B[1]=& _tmp499,((_tmp49B[2]=& _tmp498,Cyc_Tcutil_terr(p->loc,((_tmp49C="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp49C,sizeof(char),60))),_tag_dyneither(_tmp49B,sizeof(void*),3)))))))))))))))))));}{
# 520
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp49F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49E;void*vartype=(void*)((_tmp49E=_cycalloc(sizeof(*_tmp49E)),((_tmp49E[0]=((_tmp49F.tag=2,((_tmp49F.f1=tv,_tmp49F)))),_tmp49E))));
{struct Cyc_List_List*_tmp4A0;_tmp127=((_tmp4A0=_region_malloc(rgn,sizeof(*_tmp4A0)),((_tmp4A0->hd=vartype,((_tmp4A0->tl=_tmp127,_tmp4A0))))));}
# 524
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp4A3;struct Cyc_List_List*_tmp4A2;outlives_constraints=(
(_tmp4A2=_cycalloc(sizeof(*_tmp4A2)),((_tmp4A2->hd=((_tmp4A3=_cycalloc(sizeof(*_tmp4A3)),((_tmp4A3->f1=Cyc_Absyn_empty_effect,((_tmp4A3->f2=vartype,_tmp4A3)))))),((_tmp4A2->tl=outlives_constraints,_tmp4A2))))));}else{
# 529
const char*_tmp4A6;void*_tmp4A5;(_tmp4A5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A6="opened existential had unique or top region kind",_tag_dyneither(_tmp4A6,sizeof(char),49))),_tag_dyneither(_tmp4A5,sizeof(void*),0)));}}};};}}
# 533
_tmp127=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp127);{
# 535
struct _RegionHandle _tmp147=_new_region("r2");struct _RegionHandle*r2=& _tmp147;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBC);
# 538
struct Cyc_List_List*_tmp148=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp4A7;struct _tuple12 _tmp149=(_tmp4A7.f1=_tmp148,((_tmp4A7.f2=rgn,_tmp4A7)));
struct Cyc_List_List*_tmp14A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp149,_tmpBA->tvs);
struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->exist_vars,_tmp127);
struct Cyc_List_List*_tmp14C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp14A);
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp14B);
struct Cyc_List_List*_tmp14E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp14A,_tmp14B);
# 546
if(_tmpBC != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp4A8;res.tvars_and_bounds_opt=((_tmp4A8=_cycalloc(sizeof(*_tmp4A8)),((_tmp4A8->f1=0,((_tmp4A8->f2=0,_tmp4A8))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpBC));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp150=0;
{struct Cyc_List_List*_tmp151=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct _tuple0*_tmp4AB;struct Cyc_List_List*_tmp4AA;_tmp150=((_tmp4AA=_cycalloc(sizeof(*_tmp4AA)),((_tmp4AA->hd=((_tmp4AB=_cycalloc(sizeof(*_tmp4AB)),((_tmp4AB->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp14E,(*((struct _tuple0*)_tmp151->hd)).f1),((_tmp4AB->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp14E,(*((struct _tuple0*)_tmp151->hd)).f2),_tmp4AB)))))),((_tmp4AA->tl=_tmp150,_tmp4AA))))));}}
# 558
_tmp150=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp150);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp150);};}
# 563
*_tmpBB=_tmp14C;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp4B5;struct Cyc_Absyn_Aggrdecl**_tmp4B4;struct Cyc_Absyn_AggrInfo _tmp4B3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B2;t=(void*)((_tmp4B2=_cycalloc(sizeof(*_tmp4B2)),((_tmp4B2[0]=((_tmp4B5.tag=11,((_tmp4B5.f1=((_tmp4B3.aggr_info=Cyc_Absyn_KnownAggr(((_tmp4B4=_cycalloc(sizeof(*_tmp4B4)),((_tmp4B4[0]=_tmpBA,_tmp4B4))))),((_tmp4B3.targs=*_tmpBB,_tmp4B3)))),_tmp4B5)))),_tmp4B2))));}
if(_tmpBE  && _tmpBA->kind == Cyc_Absyn_UnionA){
const char*_tmp4B8;void*_tmp4B7;(_tmp4B7=0,Cyc_Tcutil_warn(p->loc,((_tmp4B8="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp4B8,sizeof(char),43))),_tag_dyneither(_tmp4B7,sizeof(void*),0)));}else{
if(_tmpBE){
# 569
int _tmp15A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp122);
int _tmp15B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields);
if(_tmp15A < _tmp15B){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp15B - _tmp15A;++ i){
struct _tuple15*_tmp4BB;struct Cyc_List_List*_tmp4BA;wild_dps=((_tmp4BA=_cycalloc(sizeof(*_tmp4BA)),((_tmp4BA->hd=((_tmp4BB=_cycalloc(sizeof(*_tmp4BB)),((_tmp4BB->f1=0,((_tmp4BB->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp4BB)))))),((_tmp4BA->tl=wild_dps,_tmp4BA))))));}}
*_tmpBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp122,wild_dps);
_tmp122=*_tmpBD;}else{
if(_tmp15A == _tmp15B){
const char*_tmp4BE;void*_tmp4BD;(_tmp4BD=0,Cyc_Tcutil_warn(p->loc,((_tmp4BE="unnecessary ... in struct pattern",_tag_dyneither(_tmp4BE,sizeof(char),34))),_tag_dyneither(_tmp4BD,sizeof(void*),0)));}}}}{
# 580
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp122,_tmpBA->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmp160=(struct _tuple19*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp162;struct Cyc_Absyn_Pat*_tmp163;struct _tuple19*_tmp161=_tmp160;_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;{
void*_tmp164=Cyc_Tcutil_rsubstitute(rgn,_tmp14E,_tmp162->type);
# 586
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
new_access_exp=Cyc_Absyn_aggrmember_exp(access_exp,_tmp162->name,0);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te2,_tmp163,& _tmp164,rgn_pat,allow_ref_pat,new_access_exp));
# 594
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp163->topt),_tmp164)){
const char*_tmp4C5;void*_tmp4C4[4];struct Cyc_String_pa_PrintArg_struct _tmp4C3;struct Cyc_String_pa_PrintArg_struct _tmp4C2;struct Cyc_String_pa_PrintArg_struct _tmp4C1;struct Cyc_String_pa_PrintArg_struct _tmp4C0;(_tmp4C0.tag=0,((_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 597
Cyc_Absynpp_typ2string((void*)_check_null(_tmp163->topt))),((_tmp4C1.tag=0,((_tmp4C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 596
Cyc_Absynpp_typ2string(_tmp164)),((_tmp4C2.tag=0,((_tmp4C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4C3.tag=0,((_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp162->name),((_tmp4C4[0]=& _tmp4C3,((_tmp4C4[1]=& _tmp4C2,((_tmp4C4[2]=& _tmp4C1,((_tmp4C4[3]=& _tmp4C0,Cyc_Tcutil_terr(p->loc,((_tmp4C5="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp4C5,sizeof(char),45))),_tag_dyneither(_tmp4C4,sizeof(void*),4)))))))))))))))))))))))));}
# 598
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp163->topt;};}};}
# 536
;_pop_region(r2);};}
# 602
_npop_handler(0);goto _LL45;
# 487
;_pop_region(rgn);};}_LL66: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmpBF=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpBF->tag != 8)goto _LL68;else{_tmpC0=_tmpBF->f1;_tmpC1=_tmpBF->f2;_tmpC2=(struct Cyc_List_List**)& _tmpBF->f3;_tmpC3=_tmpBF->f4;}}_LL67: {
# 605
struct Cyc_List_List*_tmp16E=*_tmpC2;
struct _RegionHandle _tmp16F=_new_region("rgn");struct _RegionHandle*rgn=& _tmp16F;_push_region(rgn);{
struct Cyc_List_List*tqts=_tmpC1->typs;
# 609
struct Cyc_List_List*_tmp170=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmp4C6;struct _tuple12 _tmp171=(_tmp4C6.f1=_tmp170,((_tmp4C6.f2=rgn,_tmp4C6)));
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp171,_tmpC0->tvs);
struct Cyc_List_List*_tmp173=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp172);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4CC;struct Cyc_Absyn_DatatypeFieldInfo _tmp4CB;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4CA;t=(void*)((_tmp4CA=_cycalloc(sizeof(*_tmp4CA)),((_tmp4CA[0]=((_tmp4CC.tag=4,((_tmp4CC.f1=((_tmp4CB.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((_tmp4CB.targs=_tmp173,_tmp4CB)))),_tmp4CC)))),_tmp4CA))));}
# 615
if(_tmpC3){
# 617
int _tmp177=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp16E);
int _tmp178=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp177 < _tmp178){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp178 - _tmp177;++ i){
struct Cyc_List_List*_tmp4CD;wild_ps=((_tmp4CD=_cycalloc(sizeof(*_tmp4CD)),((_tmp4CD->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4CD->tl=wild_ps,_tmp4CD))))));}}
*_tmpC2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp16E,wild_ps);
_tmp16E=*_tmpC2;}else{
if(_tmp177 == _tmp178){
const char*_tmp4D1;void*_tmp4D0[1];struct Cyc_String_pa_PrintArg_struct _tmp4CF;(_tmp4CF.tag=0,((_tmp4CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp4D0[0]=& _tmp4CF,Cyc_Tcutil_warn(p->loc,((_tmp4D1="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4D1,sizeof(char),37))),_tag_dyneither(_tmp4D0,sizeof(void*),1)))))));}}}
# 629
for(0;_tmp16E != 0  && tqts != 0;(_tmp16E=_tmp16E->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmp17D=(struct Cyc_Absyn_Pat*)_tmp16E->hd;
# 633
void*_tmp17E=Cyc_Tcutil_rsubstitute(rgn,_tmp172,(*((struct _tuple18*)tqts->hd)).f2);
# 636
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,_tmp17D,& _tmp17E,rgn_pat,allow_ref_pat,0));
# 643
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp17D->topt),_tmp17E)){
const char*_tmp4D7;void*_tmp4D6[3];struct Cyc_String_pa_PrintArg_struct _tmp4D5;struct Cyc_String_pa_PrintArg_struct _tmp4D4;struct Cyc_String_pa_PrintArg_struct _tmp4D3;(_tmp4D3.tag=0,((_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 646
Cyc_Absynpp_typ2string((void*)_check_null(_tmp17D->topt))),((_tmp4D4.tag=0,((_tmp4D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 645
Cyc_Absynpp_typ2string(_tmp17E)),((_tmp4D5.tag=0,((_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4D6[0]=& _tmp4D5,((_tmp4D6[1]=& _tmp4D4,((_tmp4D6[2]=& _tmp4D3,Cyc_Tcutil_terr(_tmp17D->loc,((_tmp4D7="%s expects argument type %s, not %s",_tag_dyneither(_tmp4D7,sizeof(char),36))),_tag_dyneither(_tmp4D6,sizeof(void*),3)))))))))))))))))));}}
# 648
if(_tmp16E != 0){
const char*_tmp4DB;void*_tmp4DA[1];struct Cyc_String_pa_PrintArg_struct _tmp4D9;(_tmp4D9.tag=0,((_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4DA[0]=& _tmp4D9,Cyc_Tcutil_terr(p->loc,((_tmp4DB="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4DB,sizeof(char),47))),_tag_dyneither(_tmp4DA,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp4DF;void*_tmp4DE[1];struct Cyc_String_pa_PrintArg_struct _tmp4DD;(_tmp4DD.tag=0,((_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4DE[0]=& _tmp4DD,Cyc_Tcutil_terr(p->loc,((_tmp4DF="too few arguments for datatype constructor %s",_tag_dyneither(_tmp4DF,sizeof(char),46))),_tag_dyneither(_tmp4DE,sizeof(void*),1)))))));}}
# 655
_npop_handler(0);goto _LL45;
# 606
;_pop_region(rgn);}_LL68: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC4->tag != 7)goto _LL6A;else{if(_tmpC4->f1 != 0)goto _LL6A;}}_LL69:
# 657
 goto _LL6B;_LL6A: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC5->tag != 7)goto _LL6C;else{if(_tmpC5->f1 == 0)goto _LL6C;if((((_tmpC5->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL6C;}}_LL6B:
 goto _LL6D;_LL6C: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmpC6=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC6->tag != 15)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmpC7=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC7->tag != 17)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmpC8=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpC8->tag != 16)goto _LL45;}_LL71:
# 662
 t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));goto _LL45;_LL45:;}
# 664
tcpat_end:
 p->topt=t;
return res;};}
# 669
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 671
struct Cyc_Tcpat_TcPatResult _tmp18C=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 673
struct _RegionHandle _tmp18D=_new_region("r");struct _RegionHandle*r=& _tmp18D;_push_region(r);{
struct _tuple1 _tmp18E=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(r,r,_tmp18C.patvars);struct Cyc_List_List*_tmp190;struct _tuple1 _tmp18F=_tmp18E;_tmp190=_tmp18F.f1;{
struct Cyc_List_List*_tmp191=0;
{struct Cyc_List_List*x=_tmp190;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp4E0;_tmp191=((_tmp4E0=_region_malloc(r,sizeof(*_tmp4E0)),((_tmp4E0->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp4E0->tl=_tmp191,_tmp4E0))))));}}}
{const char*_tmp4E1;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp191),p->loc,(
(_tmp4E1="pattern contains a repeated variable",_tag_dyneither(_tmp4E1,sizeof(char),37))));}{
# 683
struct Cyc_List_List*x=_tmp18C.patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp194=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp196;struct Cyc_Absyn_Exp**_tmp197;struct _tuple16*_tmp195=_tmp194;_tmp196=_tmp195->f1;_tmp197=(struct Cyc_Absyn_Exp**)& _tmp195->f2;
if(*_tmp197 != 0  && *_tmp197 != pat_var_exp)
*_tmp197=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp197));}};};}{
# 689
struct Cyc_Tcpat_TcPatResult _tmp198=_tmp18C;_npop_handler(0);return _tmp198;};
# 673
;_pop_region(r);}
# 695
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 698
struct _tuple0*_tmp4EB;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4EA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4E9;struct Cyc_List_List*_tmp4E8;struct Cyc_List_List*assump=(_tmp4E8=_cycalloc(sizeof(*_tmp4E8)),((_tmp4E8->hd=((_tmp4EB=_cycalloc(sizeof(*_tmp4EB)),((_tmp4EB->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp4EB->f2=(void*)((_tmp4E9=_cycalloc(sizeof(*_tmp4E9)),((_tmp4E9[0]=((_tmp4EA.tag=2,((_tmp4EA.f1=tv,_tmp4EA)))),_tmp4E9)))),_tmp4EB)))))),((_tmp4E8->tl=0,_tmp4E8)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 710 "tcpat.cyc"
struct _tuple0 _tmp4EC;struct _tuple0 _tmp199=(_tmp4EC.f1=Cyc_Tcutil_compress(new_type),((_tmp4EC.f2=Cyc_Tcutil_compress(new_type),_tmp4EC)));struct _tuple0 _tmp19A=_tmp199;struct Cyc_Absyn_PtrInfo _tmp19C;struct Cyc_Absyn_PtrInfo _tmp19E;_LLA4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A.f1;if(_tmp19B->tag != 5)goto _LLA6;else{_tmp19C=_tmp19B->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A.f2;if(_tmp19D->tag != 5)goto _LLA6;else{_tmp19E=_tmp19D->f1;}};_LLA5:
 goto _LLA3;_LLA6:;_LLA7:
{const char*_tmp4EF;void*_tmp4EE;(_tmp4EE=0,Cyc_Tcutil_terr(loc,((_tmp4EF="alias requires pointer type",_tag_dyneither(_tmp4EF,sizeof(char),28))),_tag_dyneither(_tmp4EE,sizeof(void*),0)));}goto _LLA3;_LLA3:;}else{
# 715
const char*_tmp4F4;void*_tmp4F3[2];struct Cyc_String_pa_PrintArg_struct _tmp4F2;struct Cyc_String_pa_PrintArg_struct _tmp4F1;(_tmp4F1.tag=0,((_tmp4F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp4F2.tag=0,((_tmp4F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp4F3[0]=& _tmp4F2,((_tmp4F3[1]=& _tmp4F1,Cyc_Tcutil_terr(loc,((_tmp4F4="cannot alias value of type %s to type %s",_tag_dyneither(_tmp4F4,sizeof(char),41))),_tag_dyneither(_tmp4F3,sizeof(void*),2)))))))))))));}}
# 722
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 726
void*_tmp1AA=p->r;void*_tmp1AB=_tmp1AA;struct Cyc_Absyn_Pat*_tmp1AD;struct Cyc_Absyn_AggrInfo*_tmp1AF;struct Cyc_List_List*_tmp1B0;struct Cyc_List_List*_tmp1B1;struct Cyc_List_List*_tmp1B3;struct Cyc_List_List*_tmp1B5;struct Cyc_Absyn_Vardecl*_tmp1B7;struct Cyc_Absyn_Pat*_tmp1B8;struct Cyc_Absyn_Vardecl*_tmp1BA;struct Cyc_Absyn_Pat*_tmp1BB;struct Cyc_Absyn_Tvar*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BE;_LLA9: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1AC=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1AC->tag != 6)goto _LLAB;else{_tmp1AD=_tmp1AC->f1;}}_LLAA: {
# 728
void*_tmp1BF=(void*)_check_null(p->topt);void*_tmp1C0=_tmp1BF;void*_tmp1C2;_LLBA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C0;if(_tmp1C1->tag != 5)goto _LLBC;else{_tmp1C2=((_tmp1C1->f1).ptr_atts).rgn;}}_LLBB:
# 730
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp1C2);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1AD,Cyc_Tcutil_is_noalias_region(_tmp1C2,0),patvars);
return;_LLBC:;_LLBD: {
const char*_tmp4F7;void*_tmp4F6;(_tmp4F6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4F7="check_pat_regions: bad pointer type",_tag_dyneither(_tmp4F7,sizeof(char),36))),_tag_dyneither(_tmp4F6,sizeof(void*),0)));}_LLB9:;}_LLAB: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1AE=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1AE->tag != 7)goto _LLAD;else{_tmp1AF=_tmp1AE->f1;_tmp1B0=_tmp1AE->f2;_tmp1B1=_tmp1AE->f3;}}_LLAC:
# 736
 for(0;_tmp1B1 != 0;_tmp1B1=_tmp1B1->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp1B1->hd)).f2,did_noalias_deref,patvars);}
return;_LLAD: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1B2=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1B2->tag != 8)goto _LLAF;else{_tmp1B3=_tmp1B2->f3;}}_LLAE:
# 740
 did_noalias_deref=0;_tmp1B5=_tmp1B3;goto _LLB0;_LLAF: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1B4=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1B4->tag != 5)goto _LLB1;else{_tmp1B5=_tmp1B4->f1;}}_LLB0:
# 742
 for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp1B5->hd,did_noalias_deref,patvars);}
return;_LLB1: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1B6=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1B6->tag != 3)goto _LLB3;else{_tmp1B7=_tmp1B6->f1;_tmp1B8=_tmp1B6->f2;}}_LLB2:
# 746
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1C5=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C8;struct _tuple16*_tmp1C6=_tmp1C5;_tmp1C7=_tmp1C6->f1;_tmp1C8=_tmp1C6->f2;
# 752
if((_tmp1C7 != 0  && *_tmp1C7 == _tmp1B7) && _tmp1C8 != 0){
{void*_tmp1C9=_tmp1C8->r;void*_tmp1CA=_tmp1C9;struct Cyc_Absyn_Exp*_tmp1CC;_LLBF: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1CB=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1CA;if(_tmp1CB->tag != 14)goto _LLC1;else{_tmp1CC=_tmp1CB->f1;}}_LLC0:
# 755
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp1CC->topt))){
# 757
const char*_tmp4FA;void*_tmp4F9;(_tmp4F9=0,Cyc_Tcutil_terr(p->loc,((_tmp4FA="reference pattern not allowed on alias-free pointers",_tag_dyneither(_tmp4FA,sizeof(char),53))),_tag_dyneither(_tmp4F9,sizeof(void*),0)));}
goto _LLBE;_LLC1:;_LLC2: {
# 760
const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4FD="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp4FD,sizeof(char),51))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}_LLBE:;}
# 762
break;}}}
# 765
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1B8,did_noalias_deref,patvars);
return;_LLB3: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1B9=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1B9->tag != 1)goto _LLB5;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;}}_LLB4:
# 768
{void*_tmp1D1=p->topt;void*_tmp1D2=_tmp1D1;_LLC4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1D3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1D2;if(_tmp1D3 == 0)goto _LLC6;else{if(_tmp1D3->tag != 8)goto _LLC6;}}_LLC5:
# 770
 if(did_noalias_deref){
{const char*_tmp500;void*_tmp4FF;(_tmp4FF=0,Cyc_Tcutil_terr(p->loc,((_tmp500="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp500,sizeof(char),56))),_tag_dyneither(_tmp4FF,sizeof(void*),0)));}
return;}
# 774
goto _LLC3;_LLC6:;_LLC7:
 goto _LLC3;_LLC3:;}
# 777
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1BB,did_noalias_deref,patvars);
return;_LLB5: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp1BC=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1AB;if(_tmp1BC->tag != 2)goto _LLB7;else{_tmp1BD=_tmp1BC->f1;_tmp1BE=_tmp1BC->f2;}}_LLB6:
# 780
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1D6=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1D8;struct Cyc_Absyn_Exp*_tmp1D9;struct _tuple16*_tmp1D7=_tmp1D6;_tmp1D8=_tmp1D7->f1;_tmp1D9=_tmp1D7->f2;
# 784
if(_tmp1D8 != 0  && *_tmp1D8 == _tmp1BE){
if(_tmp1D9 == 0){
const char*_tmp503;void*_tmp502;(_tmp502=0,Cyc_Tcutil_terr(p->loc,((_tmp503="cannot alias pattern expression in datatype",_tag_dyneither(_tmp503,sizeof(char),44))),_tag_dyneither(_tmp502,sizeof(void*),0)));}else{
# 788
struct _RegionHandle _tmp1DC=_new_region("r");struct _RegionHandle*r=& _tmp1DC;_push_region(r);
{struct Cyc_Absyn_Tvar*_tmp504[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,((_tmp504[0]=_tmp1BD,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp504,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp507;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp506;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp506=_cycalloc(sizeof(*_tmp506)),((_tmp506[0]=((_tmp507.tag=2,((_tmp507.f1=_tmp1BD,_tmp507)))),_tmp506)))),0,1);}
# 792
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp1D9->topt),_tmp1BD,_tmp1BE->type,_tmp1D9);}
# 789
;_pop_region(r);}
# 796
break;}}}
# 799
goto _LLA8;_LLB7:;_LLB8:
 return;_LLA8:;}
# 815 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 817
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1E0=_new_region("r");struct _RegionHandle*r=& _tmp1E0;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1E1=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct _tuple16*_tmp1E2=_tmp1E1;_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;
if(_tmp1E4 != 0){
struct Cyc_Absyn_Exp*_tmp1E5=_tmp1E4;
# 825
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1E5->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1E5)){
# 828
const char*_tmp514;void*_tmp513[1];const char*_tmp512;struct Cyc_String_pa_PrintArg_struct _tmp511;void*_tmp510[1];const char*_tmp50F;struct Cyc_String_pa_PrintArg_struct _tmp50E;(_tmp50E.tag=0,((_tmp50E.f1=(struct _dyneither_ptr)(
# 830
_tmp1E3 != 0?(struct _dyneither_ptr)(
(_tmp511.tag=0,((_tmp511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1E3)->name)),((_tmp510[0]=& _tmp511,Cyc_aprintf(((_tmp50F="for variable %s",_tag_dyneither(_tmp50F,sizeof(char),16))),_tag_dyneither(_tmp510,sizeof(void*),1)))))))):(
(_tmp512="",_tag_dyneither(_tmp512,sizeof(char),1)))),((_tmp513[0]=& _tmp50E,Cyc_Tcutil_terr(p->loc,((_tmp514="pattern %s dereferences a alias-free pointer from a non-unique path",_tag_dyneither(_tmp514,sizeof(char),68))),_tag_dyneither(_tmp513,sizeof(void*),1)))))));}}}}
# 819
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 876 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp515;return((_tmp515.Name_v).val=s,(((_tmp515.Name_v).tag=1,_tmp515)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp516;return((_tmp516.Int_v).val=i,(((_tmp516.Int_v).tag=2,_tmp516)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 886
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 891
typedef void*Cyc_Tcpat_simple_pat_t;
# 894
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1EF=c1->name;union Cyc_Tcpat_Name_value _tmp1F0=_tmp1EF;struct _dyneither_ptr _tmp1F1;int _tmp1F2;_LLC9: if((_tmp1F0.Name_v).tag != 1)goto _LLCB;_tmp1F1=(struct _dyneither_ptr)(_tmp1F0.Name_v).val;_LLCA: {
# 897
union Cyc_Tcpat_Name_value _tmp1F3=c2->name;union Cyc_Tcpat_Name_value _tmp1F4=_tmp1F3;struct _dyneither_ptr _tmp1F5;_LLCE: if((_tmp1F4.Name_v).tag != 1)goto _LLD0;_tmp1F5=(struct _dyneither_ptr)(_tmp1F4.Name_v).val;_LLCF:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1F1,(struct _dyneither_ptr)_tmp1F5);_LLD0: if((_tmp1F4.Int_v).tag != 2)goto _LLCD;_LLD1:
 return - 1;_LLCD:;}_LLCB: if((_tmp1F0.Int_v).tag != 2)goto _LLC8;_tmp1F2=(int)(_tmp1F0.Int_v).val;_LLCC: {
# 902
union Cyc_Tcpat_Name_value _tmp1F6=c2->name;union Cyc_Tcpat_Name_value _tmp1F7=_tmp1F6;int _tmp1F8;_LLD3: if((_tmp1F7.Name_v).tag != 1)goto _LLD5;_LLD4:
 return 1;_LLD5: if((_tmp1F7.Int_v).tag != 2)goto _LLD2;_tmp1F8=(int)(_tmp1F7.Int_v).val;_LLD6:
 return _tmp1F2 - _tmp1F8;_LLD2:;}_LLC8:;}
# 910
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 914
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 918
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp519;struct Cyc_Tcpat_Con_s*_tmp518;return(_tmp518=_region_malloc(r,sizeof(*_tmp518)),((_tmp518->name=Cyc_Tcpat_Name_v(((_tmp519="NULL",_tag_dyneither(_tmp519,sizeof(char),5)))),((_tmp518->arity=0,((_tmp518->span=& Cyc_Tcpat_two_opt,((_tmp518->orig_pat=p,_tmp518)))))))));}
# 921
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp51C;struct Cyc_Tcpat_Con_s*_tmp51B;return(_tmp51B=_region_malloc(r,sizeof(*_tmp51B)),((_tmp51B->name=Cyc_Tcpat_Name_v(((_tmp51C="&",_tag_dyneither(_tmp51C,sizeof(char),2)))),((_tmp51B->arity=1,((_tmp51B->span=& Cyc_Tcpat_two_opt,((_tmp51B->orig_pat=p,_tmp51B)))))))));}
# 924
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp51F;struct Cyc_Tcpat_Con_s*_tmp51E;return(_tmp51E=_region_malloc(r,sizeof(*_tmp51E)),((_tmp51E->name=Cyc_Tcpat_Name_v(((_tmp51F="&",_tag_dyneither(_tmp51F,sizeof(char),2)))),((_tmp51E->arity=1,((_tmp51E->span=& Cyc_Tcpat_one_opt,((_tmp51E->orig_pat=p,_tmp51E)))))))));}
# 927
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp520;return(_tmp520=_region_malloc(r,sizeof(*_tmp520)),((_tmp520->name=Cyc_Tcpat_Int_v(i),((_tmp520->arity=0,((_tmp520->span=0,((_tmp520->orig_pat=p,_tmp520)))))))));}
# 930
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp521;return(_tmp521=_region_malloc(r,sizeof(*_tmp521)),((_tmp521->name=Cyc_Tcpat_Name_v(f),((_tmp521->arity=0,((_tmp521->span=0,((_tmp521->orig_pat=p,_tmp521)))))))));}
# 933
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp522;return(_tmp522=_region_malloc(r,sizeof(*_tmp522)),((_tmp522->name=Cyc_Tcpat_Int_v((int)c),((_tmp522->arity=0,((_tmp522->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp522->orig_pat=p,_tmp522)))))))));}
# 936
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp525;struct Cyc_Tcpat_Con_s*_tmp524;return(_tmp524=_region_malloc(r,sizeof(*_tmp524)),((_tmp524->name=Cyc_Tcpat_Name_v(((_tmp525="$",_tag_dyneither(_tmp525,sizeof(char),2)))),((_tmp524->arity=i,((_tmp524->span=& Cyc_Tcpat_one_opt,((_tmp524->orig_pat=p,_tmp524)))))))));}
# 941
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp528;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp527;return(void*)((_tmp527=_region_malloc(r,sizeof(*_tmp527)),((_tmp527[0]=((_tmp528.tag=1,((_tmp528.f1=Cyc_Tcpat_null_con(r,p),((_tmp528.f2=0,_tmp528)))))),_tmp527))));}
# 944
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp52B;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp52A;return(void*)((_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A[0]=((_tmp52B.tag=1,((_tmp52B.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp52B.f2=0,_tmp52B)))))),_tmp52A))));}
# 947
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp52E;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp52D;return(void*)((_tmp52D=_region_malloc(r,sizeof(*_tmp52D)),((_tmp52D[0]=((_tmp52E.tag=1,((_tmp52E.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp52E.f2=0,_tmp52E)))))),_tmp52D))));}
# 950
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp531;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp530;return(void*)((_tmp530=_region_malloc(r,sizeof(*_tmp530)),((_tmp530[0]=((_tmp531.tag=1,((_tmp531.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp531.f2=0,_tmp531)))))),_tmp530))));}
# 953
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp537;struct Cyc_List_List*_tmp536;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp535;return(void*)((_tmp535=_region_malloc(r,sizeof(*_tmp535)),((_tmp535[0]=((_tmp537.tag=1,((_tmp537.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp537.f2=((_tmp536=_region_malloc(r,sizeof(*_tmp536)),((_tmp536->hd=p,((_tmp536->tl=0,_tmp536)))))),_tmp537)))))),_tmp535))));}
# 956
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp53D;struct Cyc_List_List*_tmp53C;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp53B;return(void*)((_tmp53B=_region_malloc(r,sizeof(*_tmp53B)),((_tmp53B[0]=((_tmp53D.tag=1,((_tmp53D.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp53D.f2=((_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C->hd=p,((_tmp53C->tl=0,_tmp53C)))))),_tmp53D)))))),_tmp53B))));}
# 959
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp540;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp53F;return(void*)((_tmp53F=_region_malloc(r,sizeof(*_tmp53F)),((_tmp53F[0]=((_tmp540.tag=1,((_tmp540.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp540.f2=ss,_tmp540)))))),_tmp53F))));}
# 962
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 964
struct Cyc_Tcpat_Con_s*_tmp541;struct Cyc_Tcpat_Con_s*c=(_tmp541=_region_malloc(r,sizeof(*_tmp541)),((_tmp541->name=Cyc_Tcpat_Name_v(con_name),((_tmp541->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp541->span=span,((_tmp541->orig_pat=p,_tmp541)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp544;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp543;return(void*)((_tmp543=_region_malloc(r,sizeof(*_tmp543)),((_tmp543[0]=((_tmp544.tag=1,((_tmp544.f1=c,((_tmp544.f2=ps,_tmp544)))))),_tmp543))));}
# 969
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp217=p->r;void*_tmp218=_tmp217;enum Cyc_Absyn_Sign _tmp21E;int _tmp21F;char _tmp221;struct _dyneither_ptr _tmp223;struct Cyc_Absyn_Pat*_tmp225;struct Cyc_Absyn_Pat*_tmp227;struct Cyc_Absyn_Pat*_tmp229;struct Cyc_Absyn_Datatypedecl*_tmp22B;struct Cyc_Absyn_Datatypefield*_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_List_List*_tmp22F;struct Cyc_Absyn_Aggrdecl*_tmp231;struct Cyc_List_List*_tmp232;struct Cyc_Absyn_Enumdecl*_tmp234;struct Cyc_Absyn_Enumfield*_tmp235;void*_tmp237;struct Cyc_Absyn_Enumfield*_tmp238;_LLD8: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp219=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp219->tag != 0)goto _LLDA;}_LLD9:
 goto _LLDB;_LLDA: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp21A=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp21A->tag != 2)goto _LLDC;}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp21B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp21B->tag != 4)goto _LLDE;}_LLDD:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp547;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp546;s=(void*)((_tmp546=_region_malloc(r,sizeof(*_tmp546)),((_tmp546[0]=((_tmp547.tag=0,_tmp547)),_tmp546))));}goto _LLD7;_LLDE: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp21C=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp21C->tag != 9)goto _LLE0;}_LLDF:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLD7;_LLE0: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp21D=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp21D->tag != 10)goto _LLE2;else{_tmp21E=_tmp21D->f1;_tmp21F=_tmp21D->f2;}}_LLE1:
 s=Cyc_Tcpat_int_pat(r,_tmp21F,p);goto _LLD7;_LLE2: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp220=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp220->tag != 11)goto _LLE4;else{_tmp221=_tmp220->f1;}}_LLE3:
 s=Cyc_Tcpat_char_pat(r,_tmp221,p);goto _LLD7;_LLE4: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp222=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp222->tag != 12)goto _LLE6;else{_tmp223=_tmp222->f1;}}_LLE5:
 s=Cyc_Tcpat_float_pat(r,_tmp223,p);goto _LLD7;_LLE6: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp224=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp224->tag != 1)goto _LLE8;else{_tmp225=_tmp224->f2;}}_LLE7:
 s=Cyc_Tcpat_compile_pat(r,_tmp225);goto _LLD7;_LLE8: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp226=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp226->tag != 3)goto _LLEA;else{_tmp227=_tmp226->f2;}}_LLE9:
 s=Cyc_Tcpat_compile_pat(r,_tmp227);goto _LLD7;_LLEA: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp228=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp228->tag != 6)goto _LLEC;else{_tmp229=_tmp228->f1;}}_LLEB:
# 982
{void*_tmp23F=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp240=_tmp23F;union Cyc_Absyn_Constraint*_tmp242;_LLFF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp241=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp240;if(_tmp241->tag != 5)goto _LL101;else{_tmp242=((_tmp241->f1).ptr_atts).nullable;}}_LL100: {
# 984
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp243=_tmp242;union Cyc_Absyn_Constraint*_tmp244;int _tmp245;_LL104: if((_tmp243->Forward_constr).tag != 2)goto _LL106;_tmp244=(union Cyc_Absyn_Constraint*)(_tmp243->Forward_constr).val;_LL105:
# 989
*_tmp242=*_tmp244;
continue;_LL106: if((_tmp243->No_constr).tag != 3)goto _LL108;_LL107:
# 992
{struct _union_Constraint_Eq_constr*_tmp548;(_tmp548=& _tmp242->Eq_constr,((_tmp548->tag=1,_tmp548->val=0)));}
is_nullable=0;
still_working=0;
goto _LL103;_LL108: if((_tmp243->Eq_constr).tag != 1)goto _LL103;_tmp245=(int)(_tmp243->Eq_constr).val;_LL109:
# 997
 is_nullable=_tmp245;
still_working=0;
goto _LL103;_LL103:;}{
# 1002
void*ss=Cyc_Tcpat_compile_pat(r,_tmp229);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 1006
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LLFE;};}_LL101:;_LL102: {
const char*_tmp54B;void*_tmp54A;(_tmp54A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp54B="expecting pointertype for pattern!",_tag_dyneither(_tmp54B,sizeof(char),35))),_tag_dyneither(_tmp54A,sizeof(void*),0)));}_LLFE:;}
# 1010
goto _LLD7;_LLEC: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp22A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp22A->tag != 8)goto _LLEE;else{_tmp22B=_tmp22A->f1;_tmp22C=_tmp22A->f2;_tmp22D=_tmp22A->f3;}}_LLED: {
# 1012
int*span;
{void*_tmp249=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp24A=_tmp249;_LL10B: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp24B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp24A;if(_tmp24B->tag != 3)goto _LL10D;}_LL10C:
# 1015
 if(_tmp22B->is_extensible)
span=0;else{
# 1018
int*_tmp54C;span=((_tmp54C=_region_malloc(r,sizeof(*_tmp54C)),((_tmp54C[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22B->fields))->v),_tmp54C))));}
goto _LL10A;_LL10D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp24A;if(_tmp24C->tag != 4)goto _LL10F;}_LL10E:
 span=& Cyc_Tcpat_one_opt;goto _LL10A;_LL10F:;_LL110:
{const char*_tmp54F;void*_tmp54E;span=((_tmp54E=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp54F="void datatype pattern has bad type",_tag_dyneither(_tmp54F,sizeof(char),35))),_tag_dyneither(_tmp54E,sizeof(void*),0))));}goto _LL10A;_LL10A:;}
# 1023
s=Cyc_Tcpat_con_pat(r,*(*_tmp22C->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp22D),p);
goto _LLD7;}_LLEE: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp22E=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp22E->tag != 5)goto _LLF0;else{_tmp22F=_tmp22E->f1;}}_LLEF:
# 1027
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp22F),p);goto _LLD7;_LLF0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp230=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp230->tag != 7)goto _LLF2;else{if(_tmp230->f1 == 0)goto _LLF2;if((((_tmp230->f1)->aggr_info).KnownAggr).tag != 2)goto _LLF2;_tmp231=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp230->f1)->aggr_info).KnownAggr).val);_tmp232=_tmp230->f3;}}_LLF1:
# 1032
 if(_tmp231->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1036
const char*_tmp550;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp550="",_tag_dyneither(_tmp550,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp232;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp250=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp252;struct Cyc_Absyn_Pat*_tmp253;struct _tuple15*_tmp251=_tmp250;_tmp252=_tmp251->f1;_tmp253=_tmp251->f2;{
struct Cyc_List_List*_tmp254=_tmp252;struct _dyneither_ptr*_tmp256;_LL112: if(_tmp254 == 0)goto _LL114;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp255=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp254->hd);if(_tmp255->tag != 1)goto _LL114;else{_tmp256=_tmp255->f1;}}if(_tmp254->tl != 0)goto _LL114;_LL113:
# 1041
 if(Cyc_strptrcmp(_tmp256,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp551;ps=((_tmp551=_region_malloc(r,sizeof(*_tmp551)),((_tmp551->hd=Cyc_Tcpat_compile_pat(r,_tmp253),((_tmp551->tl=ps,_tmp551))))));}
found=1;}
# 1045
goto _LL111;_LL114:;_LL115: {
const char*_tmp554;void*_tmp553;(_tmp553=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp554="bad designator(s)",_tag_dyneither(_tmp554,sizeof(char),18))),_tag_dyneither(_tmp553,sizeof(void*),0)));}_LL111:;};}}
# 1049
if(!found){
const char*_tmp557;void*_tmp556;(_tmp556=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp557="bad designator",_tag_dyneither(_tmp557,sizeof(char),15))),_tag_dyneither(_tmp556,sizeof(void*),0)));}}}
# 1052
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 1055
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->tagged){
const char*_tmp55A;void*_tmp559;(_tmp559=0,Cyc_Tcutil_terr(p->loc,((_tmp55A="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp55A,sizeof(char),47))),_tag_dyneither(_tmp559,sizeof(void*),0)));}{
int*_tmp55B;int*span=(_tmp55B=_region_malloc(r,sizeof(*_tmp55B)),((_tmp55B[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp231->impl))->fields),_tmp55B)));
struct Cyc_List_List*_tmp25F=_tmp232;struct _dyneither_ptr*_tmp261;struct Cyc_Absyn_Pat*_tmp262;_LL117: if(_tmp25F == 0)goto _LL119;if(((struct _tuple15*)_tmp25F->hd)->f1 == 0)goto _LL119;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp260=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp25F->hd)->f1)->hd);if(_tmp260->tag != 1)goto _LL119;else{_tmp261=_tmp260->f1;}}if((((struct _tuple15*)_tmp25F->hd)->f1)->tl != 0)goto _LL119;_tmp262=((struct _tuple15*)_tmp25F->hd)->f2;if(_tmp25F->tl != 0)goto _LL119;_LL118:
# 1060
{struct Cyc_List_List*_tmp55C;s=Cyc_Tcpat_con_pat(r,*_tmp261,span,((_tmp55C=_region_malloc(r,sizeof(*_tmp55C)),((_tmp55C->hd=Cyc_Tcpat_compile_pat(r,_tmp262),((_tmp55C->tl=0,_tmp55C)))))),p);}
goto _LL116;_LL119:;_LL11A: {
const char*_tmp55F;void*_tmp55E;(_tmp55E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp55F="bad union pattern",_tag_dyneither(_tmp55F,sizeof(char),18))),_tag_dyneither(_tmp55E,sizeof(void*),0)));}_LL116:;};}
# 1065
goto _LLD7;_LLF2: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp233=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp233->tag != 13)goto _LLF4;else{_tmp234=_tmp233->f1;_tmp235=_tmp233->f2;}}_LLF3:
# 1070
{void*_tmp267=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp268=_tmp267;_LL11C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp268;if(_tmp269->tag != 6)goto _LL11E;}_LL11D:
# 1074
 s=Cyc_Tcpat_con_pat(r,*(*_tmp235->name).f2,0,0,p);
goto _LL11B;_LL11E:;_LL11F: {
# 1077
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp234->fields))->v);
{int*_tmp560;s=Cyc_Tcpat_con_pat(r,*(*_tmp235->name).f2,((_tmp560=_region_malloc(r,sizeof(*_tmp560)),((_tmp560[0]=span,_tmp560)))),0,p);}
goto _LL11B;}_LL11B:;}
# 1081
goto _LLD7;_LLF4: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp236=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp236->tag != 14)goto _LLF6;else{_tmp237=(void*)_tmp236->f1;_tmp238=_tmp236->f2;}}_LLF5: {
# 1086
struct Cyc_List_List*fields;
{void*_tmp26B=Cyc_Tcutil_compress(_tmp237);void*_tmp26C=_tmp26B;struct Cyc_List_List*_tmp26E;_LL121: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp26C;if(_tmp26D->tag != 14)goto _LL123;else{_tmp26E=_tmp26D->f1;}}_LL122:
 fields=_tmp26E;goto _LL120;_LL123:;_LL124: {
const char*_tmp563;void*_tmp562;(_tmp562=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp563="bad type in AnonEnum_p",_tag_dyneither(_tmp563,sizeof(char),23))),_tag_dyneither(_tmp562,sizeof(void*),0)));}_LL120:;}
# 1093
{void*_tmp271=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp272=_tmp271;_LL126: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp272;if(_tmp273->tag != 6)goto _LL128;}_LL127:
# 1097
 s=Cyc_Tcpat_con_pat(r,*(*_tmp238->name).f2,0,0,p);
goto _LL125;_LL128:;_LL129: {
# 1100
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp564;s=Cyc_Tcpat_con_pat(r,*(*_tmp238->name).f2,((_tmp564=_region_malloc(r,sizeof(*_tmp564)),((_tmp564[0]=span,_tmp564)))),0,p);}
goto _LL125;}_LL125:;}
# 1104
goto _LLD7;}_LLF6: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp239=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp239->tag != 15)goto _LLF8;}_LLF7:
 goto _LLF9;_LLF8: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp23A=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp23A->tag != 16)goto _LLFA;}_LLF9:
 goto _LLFB;_LLFA: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp23B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp23B->tag != 7)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp23C=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp218;if(_tmp23C->tag != 17)goto _LLD7;}_LLFD: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp567;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp566;s=(void*)((_tmp566=_region_malloc(r,sizeof(*_tmp566)),((_tmp566[0]=((_tmp567.tag=0,_tmp567)),_tmp566))));}_LLD7:;}
# 1110
return s;}
# 1118
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1129
typedef void*Cyc_Tcpat_term_desc_t;
# 1133
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1140
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1149
typedef void*Cyc_Tcpat_decision_t;struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1154
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1157
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1159
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1162
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp56A;void*_tmp569;(_tmp569=0,Cyc_fprintf(Cyc_stderr,((_tmp56A=" ",_tag_dyneither(_tmp56A,sizeof(char),2))),_tag_dyneither(_tmp569,sizeof(void*),0)));}}
# 1168
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp279=c->name;
union Cyc_Tcpat_Name_value _tmp27A=_tmp279;struct _dyneither_ptr _tmp27B;int _tmp27C;_LL12B: if((_tmp27A.Name_v).tag != 1)goto _LL12D;_tmp27B=(struct _dyneither_ptr)(_tmp27A.Name_v).val;_LL12C:
{const char*_tmp56E;void*_tmp56D[1];struct Cyc_String_pa_PrintArg_struct _tmp56C;(_tmp56C.tag=0,((_tmp56C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp27B),((_tmp56D[0]=& _tmp56C,Cyc_fprintf(Cyc_stderr,((_tmp56E="%s",_tag_dyneither(_tmp56E,sizeof(char),3))),_tag_dyneither(_tmp56D,sizeof(void*),1)))))));}goto _LL12A;_LL12D: if((_tmp27A.Int_v).tag != 2)goto _LL12A;_tmp27C=(int)(_tmp27A.Int_v).val;_LL12E:
{const char*_tmp572;void*_tmp571[1];struct Cyc_Int_pa_PrintArg_struct _tmp570;(_tmp570.tag=1,((_tmp570.f1=(unsigned long)_tmp27C,((_tmp571[0]=& _tmp570,Cyc_fprintf(Cyc_stderr,((_tmp572="%d",_tag_dyneither(_tmp572,sizeof(char),3))),_tag_dyneither(_tmp571,sizeof(void*),1)))))));}goto _LL12A;_LL12A:;}
# 1176
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp283=d;void*_tmp285;struct Cyc_List_List*_tmp288;struct Cyc_Tcpat_Con_s*_tmp289;void*_tmp28A;void*_tmp28B;_LL130: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp284=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp283;if(_tmp284->tag != 1)goto _LL132;else{_tmp285=(void*)_tmp284->f1;}}_LL131:
# 1179
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp575;void*_tmp574;(_tmp574=0,Cyc_fprintf(Cyc_stderr,((_tmp575="Success(",_tag_dyneither(_tmp575,sizeof(char),9))),_tag_dyneither(_tmp574,sizeof(void*),0)));}print_rhs(_tmp285);{const char*_tmp578;void*_tmp577;(_tmp577=0,Cyc_fprintf(Cyc_stderr,((_tmp578=")\n",_tag_dyneither(_tmp578,sizeof(char),3))),_tag_dyneither(_tmp577,sizeof(void*),0)));}
goto _LL12F;_LL132: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp286=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp283;if(_tmp286->tag != 0)goto _LL134;}_LL133:
{const char*_tmp57B;void*_tmp57A;(_tmp57A=0,Cyc_fprintf(Cyc_stderr,((_tmp57B="Failure\n",_tag_dyneither(_tmp57B,sizeof(char),9))),_tag_dyneither(_tmp57A,sizeof(void*),0)));}goto _LL12F;_LL134: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp287=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp283;if(_tmp287->tag != 2)goto _LL12F;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;_tmp28A=(void*)_tmp287->f3;_tmp28B=(void*)_tmp287->f4;}}_LL135:
# 1184
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp57E;void*_tmp57D;(_tmp57D=0,Cyc_fprintf(Cyc_stderr,((_tmp57E="Access[",_tag_dyneither(_tmp57E,sizeof(char),8))),_tag_dyneither(_tmp57D,sizeof(void*),0)));}
for(0;_tmp288 != 0;_tmp288=_tmp288->tl){
{const char*_tmp582;void*_tmp581[1];struct Cyc_Int_pa_PrintArg_struct _tmp580;(_tmp580.tag=1,((_tmp580.f1=(unsigned long)((int)_tmp288->hd),((_tmp581[0]=& _tmp580,Cyc_fprintf(Cyc_stderr,((_tmp582="%d",_tag_dyneither(_tmp582,sizeof(char),3))),_tag_dyneither(_tmp581,sizeof(void*),1)))))));}
if(_tmp288->tl != 0){const char*_tmp585;void*_tmp584;(_tmp584=0,Cyc_fprintf(Cyc_stderr,((_tmp585=",",_tag_dyneither(_tmp585,sizeof(char),2))),_tag_dyneither(_tmp584,sizeof(void*),0)));}}
# 1190
{const char*_tmp588;void*_tmp587;(_tmp587=0,Cyc_fprintf(Cyc_stderr,((_tmp588="],",_tag_dyneither(_tmp588,sizeof(char),3))),_tag_dyneither(_tmp587,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp289);
{const char*_tmp58B;void*_tmp58A;(_tmp58A=0,Cyc_fprintf(Cyc_stderr,((_tmp58B=",\n",_tag_dyneither(_tmp58B,sizeof(char),3))),_tag_dyneither(_tmp58A,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp28A,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp28B,print_rhs,tab + 7);_LL12F:;}struct _tuple21{int f1;unsigned int f2;};
# 1198
static void Cyc_Tcpat_print_swrhs(struct _tuple21*x){
return;}
# 1206
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp29D=td;struct Cyc_Set_Set*_tmp29F;_LL137: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp29E=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp29D;if(_tmp29E->tag != 1)goto _LL139;else{_tmp29F=_tmp29E->f1;}}_LL138: {
# 1216
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp58E;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp58D;return(void*)((_tmp58D=_region_malloc(r,sizeof(*_tmp58D)),((_tmp58D[0]=((_tmp58E.tag=1,((_tmp58E.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp29F,c),_tmp58E)))),_tmp58D))));}_LL139: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A0=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp29D;if(_tmp2A0->tag != 0)goto _LL136;}_LL13A: {
const char*_tmp591;void*_tmp590;(_tmp590=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp591="add_neg called when td is Positive",_tag_dyneither(_tmp591,sizeof(char),35))),_tag_dyneither(_tmp590,sizeof(void*),0)));}_LL136:;}
# 1223
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2A5=td;struct Cyc_Tcpat_Con_s*_tmp2A7;struct Cyc_Set_Set*_tmp2A9;_LL13C: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2A6=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A5;if(_tmp2A6->tag != 0)goto _LL13E;else{_tmp2A7=_tmp2A6->f1;}}_LL13D:
# 1227
 if(Cyc_Tcpat_compare_con(c,_tmp2A7)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL13E: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A8=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A5;if(_tmp2A8->tag != 1)goto _LL13B;else{_tmp2A9=_tmp2A8->f1;}}_LL13F:
# 1231
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2A9,c))return Cyc_Tcpat_No;else{
# 1234
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2A9)+ 1)
return Cyc_Tcpat_Yes;else{
# 1237
return Cyc_Tcpat_Maybe;}}_LL13B:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1245
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1247
struct Cyc_List_List*_tmp2AA=ctxt;struct Cyc_Tcpat_Con_s*_tmp2AB;struct Cyc_List_List*_tmp2AC;struct Cyc_List_List*_tmp2AD;_LL141: if(_tmp2AA != 0)goto _LL143;_LL142:
 return 0;_LL143: if(_tmp2AA == 0)goto _LL140;_tmp2AB=((struct _tuple22*)_tmp2AA->hd)->f1;_tmp2AC=((struct _tuple22*)_tmp2AA->hd)->f2;_tmp2AD=_tmp2AA->tl;_LL144: {
# 1250
struct _tuple22*_tmp597;struct Cyc_List_List*_tmp596;struct Cyc_List_List*_tmp595;return(_tmp595=_region_malloc(r,sizeof(*_tmp595)),((_tmp595->hd=(
(_tmp597=_region_malloc(r,sizeof(*_tmp597)),((_tmp597->f1=_tmp2AB,((_tmp597->f2=((_tmp596=_region_malloc(r,sizeof(*_tmp596)),((_tmp596->hd=dsc,((_tmp596->tl=_tmp2AC,_tmp596)))))),_tmp597)))))),((_tmp595->tl=_tmp2AD,_tmp595)))));}_LL140:;}
# 1258
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2B1=ctxt;struct Cyc_Tcpat_Con_s*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B4;_LL146: if(_tmp2B1 != 0)goto _LL148;_LL147: {
const char*_tmp59A;void*_tmp599;(_tmp599=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp59A="norm_context: empty context",_tag_dyneither(_tmp59A,sizeof(char),28))),_tag_dyneither(_tmp599,sizeof(void*),0)));}_LL148: if(_tmp2B1 == 0)goto _LL145;_tmp2B2=((struct _tuple22*)_tmp2B1->hd)->f1;_tmp2B3=((struct _tuple22*)_tmp2B1->hd)->f2;_tmp2B4=_tmp2B1->tl;_LL149: {
# 1262
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp59D;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp59C;return Cyc_Tcpat_augment(r,_tmp2B4,(void*)((_tmp59C=_region_malloc(r,sizeof(*_tmp59C)),((_tmp59C[0]=((_tmp59D.tag=0,((_tmp59D.f1=_tmp2B2,((_tmp59D.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2B3),_tmp59D)))))),_tmp59C)))));}_LL145:;}
# 1271
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1273
struct _tuple1 _tmp59E;struct _tuple1 _tmp2B9=(_tmp59E.f1=ctxt,((_tmp59E.f2=work,_tmp59E)));struct _tuple1 _tmp2BA=_tmp2B9;struct Cyc_Tcpat_Con_s*_tmp2BB;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2BF;_LL14B: if(_tmp2BA.f1 != 0)goto _LL14D;if(_tmp2BA.f2 != 0)goto _LL14D;_LL14C:
 return dsc;_LL14D: if(_tmp2BA.f1 != 0)goto _LL14F;_LL14E:
 goto _LL150;_LL14F: if(_tmp2BA.f2 != 0)goto _LL151;_LL150: {
const char*_tmp5A1;void*_tmp5A0;(_tmp5A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5A1="build_desc: ctxt and work don't match",_tag_dyneither(_tmp5A1,sizeof(char),38))),_tag_dyneither(_tmp5A0,sizeof(void*),0)));}_LL151: if(_tmp2BA.f1 == 0)goto _LL14A;_tmp2BB=((struct _tuple22*)(_tmp2BA.f1)->hd)->f1;_tmp2BC=((struct _tuple22*)(_tmp2BA.f1)->hd)->f2;_tmp2BD=(_tmp2BA.f1)->tl;if(_tmp2BA.f2 == 0)goto _LL14A;_tmp2BE=((struct _tuple20*)(_tmp2BA.f2)->hd)->f3;_tmp2BF=(_tmp2BA.f2)->tl;_LL152: {
# 1278
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp5A7;struct Cyc_List_List*_tmp5A6;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp5A5;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C2=(_tmp5A5=_region_malloc(r,sizeof(*_tmp5A5)),((_tmp5A5[0]=((_tmp5A7.tag=0,((_tmp5A7.f1=_tmp2BB,((_tmp5A7.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2BC),(
(_tmp5A6=_region_malloc(r,sizeof(*_tmp5A6)),((_tmp5A6->hd=dsc,((_tmp5A6->tl=_tmp2BE,_tmp5A6))))))),_tmp5A7)))))),_tmp5A5)));
return Cyc_Tcpat_build_desc(r,_tmp2BD,(void*)_tmp2C2,_tmp2BF);}_LL14A:;}
# 1284
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1291
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1293
struct Cyc_List_List*_tmp2C7=allmrules;void*_tmp2C8;void*_tmp2C9;struct Cyc_List_List*_tmp2CA;_LL154: if(_tmp2C7 != 0)goto _LL156;_LL155: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp5AA;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp5A9;return(void*)((_tmp5A9=_cycalloc(sizeof(*_tmp5A9)),((_tmp5A9[0]=((_tmp5AA.tag=0,((_tmp5AA.f1=dsc,_tmp5AA)))),_tmp5A9))));}_LL156: if(_tmp2C7 == 0)goto _LL153;_tmp2C8=((struct _tuple0*)_tmp2C7->hd)->f1;_tmp2C9=((struct _tuple0*)_tmp2C7->hd)->f2;_tmp2CA=_tmp2C7->tl;_LL157:
# 1296
 return Cyc_Tcpat_match(r,_tmp2C8,0,dsc,0,0,_tmp2C9,_tmp2CA);_LL153:;}
# 1301
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1303
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5AD;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5AC;return Cyc_Tcpat_or_match(r,(void*)((_tmp5AC=_region_malloc(r,sizeof(*_tmp5AC)),((_tmp5AC[0]=((_tmp5AD.tag=1,((_tmp5AD.f1=Cyc_Tcpat_empty_con_set(r),_tmp5AD)))),_tmp5AC)))),allmrules);}
# 1309
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1313
struct Cyc_List_List*_tmp2CF=work;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_List_List*_tmp2D2;struct Cyc_List_List*_tmp2D3;struct Cyc_List_List*_tmp2D4;_LL159: if(_tmp2CF != 0)goto _LL15B;_LL15A: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp5B0;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp5AF;return(void*)((_tmp5AF=_region_malloc(r,sizeof(*_tmp5AF)),((_tmp5AF[0]=((_tmp5B0.tag=1,((_tmp5B0.f1=right_hand_side,_tmp5B0)))),_tmp5AF))));}_LL15B: if(_tmp2CF == 0)goto _LL15D;if(((struct _tuple20*)_tmp2CF->hd)->f1 != 0)goto _LL15D;if(((struct _tuple20*)_tmp2CF->hd)->f2 != 0)goto _LL15D;if(((struct _tuple20*)_tmp2CF->hd)->f3 != 0)goto _LL15D;_tmp2D0=_tmp2CF->tl;_LL15C:
# 1316
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp2D0,right_hand_side,rules);_LL15D: if(_tmp2CF == 0)goto _LL158;_tmp2D1=((struct _tuple20*)_tmp2CF->hd)->f1;_tmp2D2=((struct _tuple20*)_tmp2CF->hd)->f2;_tmp2D3=((struct _tuple20*)_tmp2CF->hd)->f3;_tmp2D4=_tmp2CF->tl;_LL15E:
# 1318
 if((_tmp2D1 == 0  || _tmp2D2 == 0) || _tmp2D3 == 0){
const char*_tmp5B3;void*_tmp5B2;(_tmp5B2=0,Cyc_Tcutil_impos(((_tmp5B3="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp5B3,sizeof(char),38))),_tag_dyneither(_tmp5B2,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2D9=_tmp2D1;void*_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_List_List*_tmp2DA=_tmp2D9;_tmp2DB=(void*)_tmp2DA->hd;_tmp2DC=_tmp2DA->tl;{
struct Cyc_List_List*_tmp2DD=_tmp2D2;struct Cyc_List_List*_tmp2DF;struct Cyc_List_List*_tmp2E0;struct Cyc_List_List*_tmp2DE=_tmp2DD;_tmp2DF=(struct Cyc_List_List*)_tmp2DE->hd;_tmp2E0=_tmp2DE->tl;{
struct Cyc_List_List*_tmp2E1=_tmp2D3;void*_tmp2E3;struct Cyc_List_List*_tmp2E4;struct Cyc_List_List*_tmp2E2=_tmp2E1;_tmp2E3=(void*)_tmp2E2->hd;_tmp2E4=_tmp2E2->tl;{
struct _tuple20*_tmp5B4;struct _tuple20*_tmp2E5=(_tmp5B4=_region_malloc(r,sizeof(*_tmp5B4)),((_tmp5B4->f1=_tmp2DC,((_tmp5B4->f2=_tmp2E0,((_tmp5B4->f3=_tmp2E4,_tmp5B4)))))));
struct Cyc_List_List*_tmp5B5;return Cyc_Tcpat_match(r,_tmp2DB,_tmp2DF,_tmp2E3,ctx,((_tmp5B5=_region_malloc(r,sizeof(*_tmp5B5)),((_tmp5B5->hd=_tmp2E5,((_tmp5B5->tl=_tmp2D4,_tmp5B5)))))),right_hand_side,rules);};};};};_LL158:;}
# 1330
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1332
void*_tmp2E8=dsc;struct Cyc_Set_Set*_tmp2EA;struct Cyc_List_List*_tmp2EC;_LL160: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2E9=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2E8;if(_tmp2E9->tag != 1)goto _LL162;else{_tmp2EA=_tmp2E9->f1;}}_LL161: {
# 1337
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5B8;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5B7;void*any=(void*)((_tmp5B7=_region_malloc(r,sizeof(*_tmp5B7)),((_tmp5B7[0]=((_tmp5B8.tag=1,((_tmp5B8.f1=Cyc_Tcpat_empty_con_set(r),_tmp5B8)))),_tmp5B7))));
struct Cyc_List_List*_tmp2ED=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp5B9;_tmp2ED=((_tmp5B9=_region_malloc(r,sizeof(*_tmp5B9)),((_tmp5B9->hd=any,((_tmp5B9->tl=_tmp2ED,_tmp5B9))))));}}
return _tmp2ED;}_LL162: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2EB=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2E8;if(_tmp2EB->tag != 0)goto _LL15F;else{_tmp2EC=_tmp2EB->f2;}}_LL163:
 return _tmp2EC;_LL15F:;}struct _tuple23{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1347
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _RegionHandle*_tmp2F2;struct Cyc_List_List*_tmp2F3;struct _tuple23*_tmp2F1=env;_tmp2F2=_tmp2F1->f1;_tmp2F3=_tmp2F1->f2;{
struct Cyc_List_List*_tmp5BA;return(_tmp5BA=_region_malloc(_tmp2F2,sizeof(*_tmp5BA)),((_tmp5BA->hd=(void*)(i + 1),((_tmp5BA->tl=_tmp2F3,_tmp5BA)))));};}
# 1351
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1353
struct _tuple23 _tmp5BB;struct _tuple23 _tmp2F5=(_tmp5BB.f1=r,((_tmp5BB.f2=obj,_tmp5BB)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp2F5);}
# 1360
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1364
void*_tmp2F7=p;struct Cyc_Tcpat_Con_s*_tmp2FA;struct Cyc_List_List*_tmp2FB;_LL165: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp2F8=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp2F7;if(_tmp2F8->tag != 0)goto _LL167;}_LL166:
# 1366
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL167: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp2F9=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp2F7;if(_tmp2F9->tag != 1)goto _LL164;else{_tmp2FA=_tmp2F9->f1;_tmp2FB=_tmp2F9->f2;}}_LL168: {
# 1368
enum Cyc_Tcpat_Answer _tmp2FC=Cyc_Tcpat_static_match(_tmp2FA,dsc);switch(_tmp2FC){case Cyc_Tcpat_Yes: _LL169: {
# 1370
struct _tuple22*_tmp5BE;struct Cyc_List_List*_tmp5BD;struct Cyc_List_List*ctx2=(_tmp5BD=_region_malloc(r,sizeof(*_tmp5BD)),((_tmp5BD->hd=((_tmp5BE=_region_malloc(r,sizeof(*_tmp5BE)),((_tmp5BE->f1=_tmp2FA,((_tmp5BE->f2=0,_tmp5BE)))))),((_tmp5BD->tl=ctx,_tmp5BD)))));
struct _tuple20*_tmp5BF;struct _tuple20*work_frame=(_tmp5BF=_region_malloc(r,sizeof(*_tmp5BF)),((_tmp5BF->f1=_tmp2FB,((_tmp5BF->f2=Cyc_Tcpat_getoargs(r,_tmp2FA,obj),((_tmp5BF->f3=
Cyc_Tcpat_getdargs(r,_tmp2FA,dsc),_tmp5BF)))))));
struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*work2=(_tmp5C0=_region_malloc(r,sizeof(*_tmp5C0)),((_tmp5C0->hd=work_frame,((_tmp5C0->tl=work,_tmp5C0)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL16A:
# 1376
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL16B: {
# 1378
struct _tuple22*_tmp5C3;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*ctx2=(_tmp5C2=_region_malloc(r,sizeof(*_tmp5C2)),((_tmp5C2->hd=((_tmp5C3=_region_malloc(r,sizeof(*_tmp5C3)),((_tmp5C3->f1=_tmp2FA,((_tmp5C3->f2=0,_tmp5C3)))))),((_tmp5C2->tl=ctx,_tmp5C2)))));
struct _tuple20*_tmp5C4;struct _tuple20*work_frame=(_tmp5C4=_region_malloc(r,sizeof(*_tmp5C4)),((_tmp5C4->f1=_tmp2FB,((_tmp5C4->f2=Cyc_Tcpat_getoargs(r,_tmp2FA,obj),((_tmp5C4->f3=
Cyc_Tcpat_getdargs(r,_tmp2FA,dsc),_tmp5C4)))))));
struct Cyc_List_List*_tmp5C5;struct Cyc_List_List*work2=(_tmp5C5=_region_malloc(r,sizeof(*_tmp5C5)),((_tmp5C5->hd=work_frame,((_tmp5C5->tl=work,_tmp5C5)))));
void*_tmp301=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp302=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp2FA),work),rules);
# 1385
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp5C8;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp5C7;return(void*)((_tmp5C7=_region_malloc(r,sizeof(*_tmp5C7)),((_tmp5C7[0]=((_tmp5C8.tag=2,((_tmp5C8.f1=obj,((_tmp5C8.f2=_tmp2FA,((_tmp5C8.f3=_tmp301,((_tmp5C8.f4=_tmp302,_tmp5C8)))))))))),_tmp5C7))));}}}_LL164:;}
# 1395
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1401
void*_tmp309=d;void*_tmp30B;void*_tmp30D;void*_tmp30F;void*_tmp310;_LL16E: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp30A=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp309;if(_tmp30A->tag != 0)goto _LL170;else{_tmp30B=(void*)_tmp30A->f1;}}_LL16F:
# 1403
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp30B);*exhaust_done=1;}
goto _LL16D;_LL170: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp30C=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp309;if(_tmp30C->tag != 1)goto _LL172;else{_tmp30D=(void*)_tmp30C->f1;}}_LL171:
 rhs_appears(env2,_tmp30D);goto _LL16D;_LL172: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp30E=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp309;if(_tmp30E->tag != 2)goto _LL16D;else{_tmp30F=(void*)_tmp30E->f3;_tmp310=(void*)_tmp30E->f4;}}_LL173:
# 1407
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp30F,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1409
Cyc_Tcpat_check_exhaust_overlap(r,_tmp310,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1411
goto _LL16D;_LL16D:;}struct _tuple24{void*f1;struct _tuple21*f2;};struct _tuple25{struct _RegionHandle*f1;int*f2;};
# 1419
static struct _tuple24*Cyc_Tcpat_get_match(struct _tuple25*env,struct Cyc_Absyn_Switch_clause*swc){
# 1422
struct _RegionHandle*_tmp312;int*_tmp313;struct _tuple25*_tmp311=env;_tmp312=_tmp311->f1;_tmp313=_tmp311->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp312,swc->pattern);
struct _tuple21*_tmp5C9;struct _tuple21*rhs=(_tmp5C9=_region_malloc(_tmp312,sizeof(*_tmp5C9)),((_tmp5C9->f1=0,((_tmp5C9->f2=(swc->pattern)->loc,_tmp5C9)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp5CC;struct Cyc_List_List*_tmp5CB;sp=Cyc_Tcpat_tuple_pat(_tmp312,((_tmp5CB=_region_malloc(_tmp312,sizeof(*_tmp5CB)),((_tmp5CB->hd=sp0,((_tmp5CB->tl=(
(_tmp5CC=_region_malloc(_tmp312,sizeof(*_tmp5CC)),((_tmp5CC->hd=Cyc_Tcpat_int_pat(_tmp312,*_tmp313,0),((_tmp5CC->tl=0,_tmp5CC)))))),_tmp5CB)))))),0);}
*_tmp313=*_tmp313 + 1;}else{
# 1431
struct Cyc_List_List*_tmp5D6;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5D5;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp5D4;struct Cyc_List_List*_tmp5D3;sp=Cyc_Tcpat_tuple_pat(_tmp312,((_tmp5D3=_region_malloc(_tmp312,sizeof(*_tmp5D3)),((_tmp5D3->hd=sp0,((_tmp5D3->tl=((_tmp5D6=_region_malloc(_tmp312,sizeof(*_tmp5D6)),((_tmp5D6->hd=(void*)((_tmp5D4=_region_malloc(_tmp312,sizeof(*_tmp5D4)),((_tmp5D4[0]=((_tmp5D5.tag=0,_tmp5D5)),_tmp5D4)))),((_tmp5D6->tl=0,_tmp5D6)))))),_tmp5D3)))))),0);}{
struct _tuple24*_tmp5D7;return(_tmp5D7=_region_malloc(_tmp312,sizeof(*_tmp5D7)),((_tmp5D7->f1=sp,((_tmp5D7->f2=rhs,_tmp5D7)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1437
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1439
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp31D=d;struct Cyc_Tcpat_Con_s*_tmp31F;struct Cyc_List_List*_tmp320;struct Cyc_Set_Set*_tmp322;_LL175: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp31E=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp31D;if(_tmp31E->tag != 0)goto _LL177;else{_tmp31F=_tmp31E->f1;_tmp320=_tmp31E->f2;}}_LL176: {
# 1443
union Cyc_Tcpat_Name_value _tmp323=_tmp31F->name;
struct Cyc_Absyn_Pat*_tmp324=_tmp31F->orig_pat;
if(_tmp324 == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp320))->hd);{
# 1448
void*_tmp325=_tmp324->r;void*_tmp326=_tmp325;struct Cyc_Absyn_Vardecl*_tmp329;struct Cyc_Absyn_Vardecl*_tmp32B;struct Cyc_Absyn_Tvar*_tmp32D;struct Cyc_Absyn_Vardecl*_tmp32E;struct Cyc_Absyn_Aggrdecl*_tmp332;struct Cyc_List_List*_tmp333;struct Cyc_Absyn_Datatypefield*_tmp335;int _tmp338;char _tmp33A;struct _dyneither_ptr _tmp33C;int _tmp33D;struct Cyc_Absyn_Enumfield*_tmp33F;struct Cyc_Absyn_Enumfield*_tmp341;struct Cyc_Absyn_Exp*_tmp343;_LL17A: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp327=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp327->tag != 0)goto _LL17C;}_LL17B: {
const char*_tmp5D8;return(_tmp5D8="_",_tag_dyneither(_tmp5D8,sizeof(char),2));}_LL17C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp328=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp328->tag != 1)goto _LL17E;else{_tmp329=_tmp328->f1;}}_LL17D:
 return Cyc_Absynpp_qvar2string(_tmp329->name);_LL17E: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp32A=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp32A->tag != 3)goto _LL180;else{_tmp32B=_tmp32A->f1;}}_LL17F: {
const char*_tmp5DC;void*_tmp5DB[1];struct Cyc_String_pa_PrintArg_struct _tmp5DA;return(struct _dyneither_ptr)((_tmp5DA.tag=0,((_tmp5DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp32B->name)),((_tmp5DB[0]=& _tmp5DA,Cyc_aprintf(((_tmp5DC="*%s",_tag_dyneither(_tmp5DC,sizeof(char),4))),_tag_dyneither(_tmp5DB,sizeof(void*),1))))))));}_LL180: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp32C=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp32C->tag != 4)goto _LL182;else{_tmp32D=_tmp32C->f1;_tmp32E=_tmp32C->f2;}}_LL181: {
const char*_tmp5E1;void*_tmp5E0[2];struct Cyc_String_pa_PrintArg_struct _tmp5DF;struct Cyc_String_pa_PrintArg_struct _tmp5DE;return(struct _dyneither_ptr)((_tmp5DE.tag=0,((_tmp5DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp32D->name),((_tmp5DF.tag=0,((_tmp5DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp32E->name)),((_tmp5E0[0]=& _tmp5DF,((_tmp5E0[1]=& _tmp5DE,Cyc_aprintf(((_tmp5E1="%s<`%s>",_tag_dyneither(_tmp5E1,sizeof(char),8))),_tag_dyneither(_tmp5E0,sizeof(void*),2))))))))))))));}_LL182: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp32F=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp32F->tag != 5)goto _LL184;}_LL183: {
# 1454
const char*_tmp5E5;void*_tmp5E4[1];struct Cyc_String_pa_PrintArg_struct _tmp5E3;return(struct _dyneither_ptr)((_tmp5E3.tag=0,((_tmp5E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp320)),((_tmp5E4[0]=& _tmp5E3,Cyc_aprintf(((_tmp5E5="$(%s)",_tag_dyneither(_tmp5E5,sizeof(char),6))),_tag_dyneither(_tmp5E4,sizeof(void*),1))))))));}_LL184: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp330=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp330->tag != 6)goto _LL186;}_LL185: {
# 1456
const char*_tmp5E9;void*_tmp5E8[1];struct Cyc_String_pa_PrintArg_struct _tmp5E7;return(struct _dyneither_ptr)((_tmp5E7.tag=0,((_tmp5E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp320)),((_tmp5E8[0]=& _tmp5E7,Cyc_aprintf(((_tmp5E9="&%s",_tag_dyneither(_tmp5E9,sizeof(char),4))),_tag_dyneither(_tmp5E8,sizeof(void*),1))))))));}_LL186: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp331=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp331->tag != 7)goto _LL188;else{if(_tmp331->f1 == 0)goto _LL188;if((((_tmp331->f1)->aggr_info).KnownAggr).tag != 2)goto _LL188;_tmp332=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp331->f1)->aggr_info).KnownAggr).val);_tmp333=_tmp331->f3;}}_LL187:
# 1458
 if(_tmp332->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp352=_tmp333;struct _dyneither_ptr*_tmp354;_LL19B: if(_tmp352 == 0)goto _LL19D;if(((struct _tuple15*)_tmp352->hd)->f1 == 0)goto _LL19D;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp353=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp352->hd)->f1)->hd);if(_tmp353->tag != 1)goto _LL19D;else{_tmp354=_tmp353->f1;}};_LL19C: {
# 1461
const char*_tmp5EF;void*_tmp5EE[3];struct Cyc_String_pa_PrintArg_struct _tmp5ED;struct Cyc_String_pa_PrintArg_struct _tmp5EC;struct Cyc_String_pa_PrintArg_struct _tmp5EB;return(struct _dyneither_ptr)((_tmp5EB.tag=0,((_tmp5EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp320)),((_tmp5EC.tag=0,((_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp354),((_tmp5ED.tag=0,((_tmp5ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1461
Cyc_Absynpp_qvar2string(_tmp332->name)),((_tmp5EE[0]=& _tmp5ED,((_tmp5EE[1]=& _tmp5EC,((_tmp5EE[2]=& _tmp5EB,Cyc_aprintf(((_tmp5EF="%s{.%s = %s}",_tag_dyneither(_tmp5EF,sizeof(char),13))),_tag_dyneither(_tmp5EE,sizeof(void*),3))))))))))))))))))));}_LL19D:;_LL19E:
# 1463
 goto _LL19A;_LL19A:;}{
# 1466
const char*_tmp5F4;void*_tmp5F3[2];struct Cyc_String_pa_PrintArg_struct _tmp5F2;struct Cyc_String_pa_PrintArg_struct _tmp5F1;return(struct _dyneither_ptr)((_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp320)),((_tmp5F2.tag=0,((_tmp5F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp332->name)),((_tmp5F3[0]=& _tmp5F2,((_tmp5F3[1]=& _tmp5F1,Cyc_aprintf(((_tmp5F4="%s{%s}",_tag_dyneither(_tmp5F4,sizeof(char),7))),_tag_dyneither(_tmp5F3,sizeof(void*),2))))))))))))));};_LL188: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp334=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp334->tag != 8)goto _LL18A;else{_tmp335=_tmp334->f2;}}_LL189:
# 1468
 if(_tmp320 == 0){
const char*_tmp5F8;void*_tmp5F7[1];struct Cyc_String_pa_PrintArg_struct _tmp5F6;return(struct _dyneither_ptr)((_tmp5F6.tag=0,((_tmp5F6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp335->name)),((_tmp5F7[0]=& _tmp5F6,Cyc_aprintf(((_tmp5F8="%s",_tag_dyneither(_tmp5F8,sizeof(char),3))),_tag_dyneither(_tmp5F7,sizeof(void*),1))))))));}else{
# 1471
const char*_tmp5FD;void*_tmp5FC[2];struct Cyc_String_pa_PrintArg_struct _tmp5FB;struct Cyc_String_pa_PrintArg_struct _tmp5FA;return(struct _dyneither_ptr)((_tmp5FA.tag=0,((_tmp5FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp320)),((_tmp5FB.tag=0,((_tmp5FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp335->name)),((_tmp5FC[0]=& _tmp5FB,((_tmp5FC[1]=& _tmp5FA,Cyc_aprintf(((_tmp5FD="%s(%s)",_tag_dyneither(_tmp5FD,sizeof(char),7))),_tag_dyneither(_tmp5FC,sizeof(void*),2))))))))))))));}_LL18A: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp336=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp336->tag != 9)goto _LL18C;}_LL18B: {
const char*_tmp5FE;return(_tmp5FE="NULL",_tag_dyneither(_tmp5FE,sizeof(char),5));}_LL18C: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp337=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp337->tag != 10)goto _LL18E;else{_tmp338=_tmp337->f2;}}_LL18D: {
const char*_tmp602;void*_tmp601[1];struct Cyc_Int_pa_PrintArg_struct _tmp600;return(struct _dyneither_ptr)((_tmp600.tag=1,((_tmp600.f1=(unsigned long)_tmp338,((_tmp601[0]=& _tmp600,Cyc_aprintf(((_tmp602="%d",_tag_dyneither(_tmp602,sizeof(char),3))),_tag_dyneither(_tmp601,sizeof(void*),1))))))));}_LL18E: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp339=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp339->tag != 11)goto _LL190;else{_tmp33A=_tmp339->f1;}}_LL18F: {
const char*_tmp606;void*_tmp605[1];struct Cyc_Int_pa_PrintArg_struct _tmp604;return(struct _dyneither_ptr)((_tmp604.tag=1,((_tmp604.f1=(unsigned long)((int)_tmp33A),((_tmp605[0]=& _tmp604,Cyc_aprintf(((_tmp606="%d",_tag_dyneither(_tmp606,sizeof(char),3))),_tag_dyneither(_tmp605,sizeof(void*),1))))))));}_LL190: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp33B=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp33B->tag != 12)goto _LL192;else{_tmp33C=_tmp33B->f1;_tmp33D=_tmp33B->f2;}}_LL191:
 return _tmp33C;_LL192: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp33E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp33E->tag != 13)goto _LL194;else{_tmp33F=_tmp33E->f2;}}_LL193:
 _tmp341=_tmp33F;goto _LL195;_LL194: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp340=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp340->tag != 14)goto _LL196;else{_tmp341=_tmp340->f2;}}_LL195:
 return Cyc_Absynpp_qvar2string(_tmp341->name);_LL196: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp342=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp326;if(_tmp342->tag != 17)goto _LL198;else{_tmp343=_tmp342->f1;}}_LL197:
 return Cyc_Absynpp_exp2string(_tmp343);_LL198:;_LL199:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL179:;};}_LL177: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp321=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp31D;if(_tmp321->tag != 1)goto _LL174;else{_tmp322=_tmp321->f1;}}_LL178:
# 1482
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp322)){const char*_tmp607;return(_tmp607="_",_tag_dyneither(_tmp607,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp36D=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp322);
struct Cyc_Absyn_Pat*_tmp36E=_tmp36D->orig_pat;
# 1487
if(_tmp36E == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp36F=Cyc_Tcutil_compress((void*)_check_null(_tmp36E->topt));void*_tmp370=_tmp36F;struct Cyc_Absyn_PtrAtts _tmp374;struct Cyc_Absyn_Datatypedecl*_tmp376;struct Cyc_Absyn_Aggrdecl*_tmp378;_LL1A0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp370;if(_tmp371->tag != 6)goto _LL1A2;else{if(_tmp371->f2 != Cyc_Absyn_Char_sz)goto _LL1A2;}}_LL1A1:
# 1491
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp379=Cyc_Tcpat_char_con(r,(char)i,_tmp36E);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp322,_tmp379)){
const char*_tmp60B;void*_tmp60A[1];struct Cyc_Int_pa_PrintArg_struct _tmp609;return(struct _dyneither_ptr)((_tmp609.tag=1,((_tmp609.f1=(unsigned long)i,((_tmp60A[0]=& _tmp609,Cyc_aprintf(((_tmp60B="%d",_tag_dyneither(_tmp60B,sizeof(char),3))),_tag_dyneither(_tmp60A,sizeof(void*),1))))))));}}}
# 1496
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp370;if(_tmp372->tag != 6)goto _LL1A4;}_LL1A3:
# 1499
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp37D=Cyc_Tcpat_int_con(r,(int)i,_tmp36E);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp322,_tmp37D)){
const char*_tmp60F;void*_tmp60E[1];struct Cyc_Int_pa_PrintArg_struct _tmp60D;return(struct _dyneither_ptr)((_tmp60D.tag=1,((_tmp60D.f1=(unsigned long)((int)i),((_tmp60E[0]=& _tmp60D,Cyc_aprintf(((_tmp60F="%d",_tag_dyneither(_tmp60F,sizeof(char),3))),_tag_dyneither(_tmp60E,sizeof(void*),1))))))));}}}
# 1504
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp370;if(_tmp373->tag != 5)goto _LL1A6;else{_tmp374=(_tmp373->f1).ptr_atts;}}_LL1A5: {
# 1506
union Cyc_Absyn_Constraint*_tmp381=_tmp374.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp381);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp322,Cyc_Tcpat_null_con(r,_tmp36E))){
const char*_tmp610;return(_tmp610="NULL",_tag_dyneither(_tmp610,sizeof(char),5));}}{
# 1512
const char*_tmp611;return(_tmp611="&_",_tag_dyneither(_tmp611,sizeof(char),3));};}_LL1A6: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp370;if(_tmp375->tag != 3)goto _LL1A8;else{if((((_tmp375->f1).datatype_info).KnownDatatype).tag != 2)goto _LL1A8;_tmp376=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp375->f1).datatype_info).KnownDatatype).val);}}_LL1A7:
# 1514
 if(_tmp376->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp384=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp376->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp384);
for(0;(unsigned int)_tmp384;_tmp384=_tmp384->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp384->hd)->name).f2;
struct Cyc_List_List*_tmp385=((struct Cyc_Absyn_Datatypefield*)_tmp384->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp612;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp322,((_tmp612=_cycalloc(sizeof(*_tmp612)),((_tmp612->name=Cyc_Tcpat_Name_v(n),((_tmp612->arity=0,((_tmp612->span=0,((_tmp612->orig_pat=_tmp36E,_tmp612)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp385)== 0)
return n;else{
# 1524
const char*_tmp616;void*_tmp615[1];struct Cyc_String_pa_PrintArg_struct _tmp614;return(struct _dyneither_ptr)((_tmp614.tag=0,((_tmp614.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp615[0]=& _tmp614,Cyc_aprintf(((_tmp616="%s(...)",_tag_dyneither(_tmp616,sizeof(char),8))),_tag_dyneither(_tmp615,sizeof(void*),1))))))));}}}
# 1527
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL1A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp377=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp370;if(_tmp377->tag != 11)goto _LL1AA;else{if((((_tmp377->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AA;_tmp378=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp377->f1).aggr_info).KnownAggr).val);}}if(!(_tmp378->kind == Cyc_Absyn_UnionA))goto _LL1AA;_LL1A9: {
# 1529
struct Cyc_List_List*_tmp38A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp378->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38A);
struct _tuple2*_tmp38B=_tmp378->name;struct _dyneither_ptr _tmp38D;struct _tuple2*_tmp38C=_tmp38B;_tmp38D=*_tmp38C->f2;
for(0;(unsigned int)_tmp38A;_tmp38A=_tmp38A->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp38A->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp617;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp322,((_tmp617=_cycalloc(sizeof(*_tmp617)),((_tmp617->name=Cyc_Tcpat_Name_v(n),((_tmp617->arity=0,((_tmp617->span=0,((_tmp617->orig_pat=_tmp36E,_tmp617)))))))))))){
const char*_tmp61C;void*_tmp61B[2];struct Cyc_String_pa_PrintArg_struct _tmp61A;struct Cyc_String_pa_PrintArg_struct _tmp619;return(struct _dyneither_ptr)((_tmp619.tag=0,((_tmp619.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp61A.tag=0,((_tmp61A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp38D),((_tmp61B[0]=& _tmp61A,((_tmp61B[1]=& _tmp619,Cyc_aprintf(((_tmp61C="%s{.%s = _}",_tag_dyneither(_tmp61C,sizeof(char),12))),_tag_dyneither(_tmp61B,sizeof(void*),2))))))))))))));}}
# 1537
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL1AA:;_LL1AB:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL19F:;};};_LL174:;}
# 1542
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp61D;return(_tmp61D=_cycalloc(sizeof(*_tmp61D)),((_tmp61D[0]=Cyc_Tcpat_desc2string(r,d),_tmp61D)));}
# 1545
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp394=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp395=_tmp394;for(0;_tmp395 != 0;_tmp395=((struct Cyc_List_List*)_check_null(_tmp395))->tl){
if(_tmp395->tl != 0){
{struct Cyc_List_List*_tmp61E;_tmp395->tl=((_tmp61E=_cycalloc(sizeof(*_tmp61E)),((_tmp61E->hd=comma,((_tmp61E->tl=_tmp395->tl,_tmp61E))))));}
_tmp395=_tmp395->tl;}}}
# 1554
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp394);}
# 1557
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp398;_push_handler(& _tmp398);{int _tmp39A=0;if(setjmp(_tmp398.handler))_tmp39A=1;if(!_tmp39A){
{struct _dyneither_ptr _tmp39B=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp622;void*_tmp621[1];struct Cyc_String_pa_PrintArg_struct _tmp620;(_tmp620.tag=0,((_tmp620.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39B),((_tmp621[0]=& _tmp620,Cyc_Tcutil_terr(loc,((_tmp622="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp622,sizeof(char),53))),_tag_dyneither(_tmp621,sizeof(void*),1)))))));}
# 1559
;_pop_handler();}else{void*_tmp399=(void*)_exn_thrown;void*_tmp3A0=_tmp399;void*_tmp3A2;_LL1AD: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3A1=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3A0;if(_tmp3A1->tag != Cyc_Tcpat_Desc2string)goto _LL1AF;}_LL1AE:
# 1563
{const char*_tmp625;void*_tmp624;(_tmp624=0,Cyc_Tcutil_terr(loc,((_tmp625="patterns may not be exhaustive.",_tag_dyneither(_tmp625,sizeof(char),32))),_tag_dyneither(_tmp624,sizeof(void*),0)));}
goto _LL1AC;_LL1AF: _tmp3A2=_tmp3A0;_LL1B0:(void)_throw(_tmp3A2);_LL1AC:;}};}
# 1567
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple21*rhs){
(*rhs).f1=1;}
# 1571
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1576
struct _RegionHandle _tmp3A5=_new_region("r");struct _RegionHandle*r=& _tmp3A5;_push_region(r);
{int _tmp3A6=0;
struct _tuple25 _tmp626;struct _tuple25 _tmp3A7=(_tmp626.f1=r,((_tmp626.f2=& _tmp3A6,_tmp626)));
struct Cyc_List_List*_tmp3A8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _tuple25*,struct Cyc_Absyn_Switch_clause*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp3A7,swcs);
void*_tmp3A9=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3A8);
int _tmp3AA=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3A9,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3AA);
# 1584
for(0;_tmp3A8 != 0;_tmp3A8=_tmp3A8->tl){
struct _tuple24*_tmp3AB=(struct _tuple24*)_tmp3A8->hd;int _tmp3AD;unsigned int _tmp3AE;struct _tuple24*_tmp3AC=_tmp3AB;_tmp3AD=(_tmp3AC->f2)->f1;_tmp3AE=(_tmp3AC->f2)->f2;
if(!_tmp3AD){
{const char*_tmp629;void*_tmp628;(_tmp628=0,Cyc_Tcutil_terr(_tmp3AE,((_tmp629="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp629,sizeof(char),74))),_tag_dyneither(_tmp628,sizeof(void*),0)));}
# 1589
break;}}}
# 1577
;_pop_region(r);}
# 1599
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3B2;_push_handler(& _tmp3B2);{int _tmp3B4=0;if(setjmp(_tmp3B2.handler))_tmp3B4=1;if(!_tmp3B4){
{struct _dyneither_ptr _tmp3B5=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp62D;void*_tmp62C[1];struct Cyc_String_pa_PrintArg_struct _tmp62B;(_tmp62B.tag=0,((_tmp62B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B5),((_tmp62C[0]=& _tmp62B,Cyc_Tcutil_warn((*pr).f1,((_tmp62D="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp62D,sizeof(char),45))),_tag_dyneither(_tmp62C,sizeof(void*),1)))))));}
# 1602
;_pop_handler();}else{void*_tmp3B3=(void*)_exn_thrown;void*_tmp3BA=_tmp3B3;void*_tmp3BC;_LL1B2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3BB=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3BA;if(_tmp3BB->tag != Cyc_Tcpat_Desc2string)goto _LL1B4;}_LL1B3:
# 1606
{const char*_tmp630;void*_tmp62F;(_tmp62F=0,Cyc_Tcutil_warn((*pr).f1,((_tmp630="pattern not exhaustive.",_tag_dyneither(_tmp630,sizeof(char),24))),_tag_dyneither(_tmp62F,sizeof(void*),0)));}
goto _LL1B1;_LL1B4: _tmp3BC=_tmp3BA;_LL1B5:(void)_throw(_tmp3BC);_LL1B1:;}};};}
# 1610
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple26{void*f1;int f2;};
# 1613
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp3BF=_new_region("r");struct _RegionHandle*r=& _tmp3BF;_push_region(r);
{struct _tuple26*_tmp633;struct Cyc_List_List*_tmp632;struct Cyc_List_List*_tmp3C0=(_tmp632=_region_malloc(r,sizeof(*_tmp632)),((_tmp632->hd=((_tmp633=_region_malloc(r,sizeof(*_tmp633)),((_tmp633->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp633->f2=0,_tmp633)))))),((_tmp632->tl=0,_tmp632)))));
void*_tmp3C1=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3C0);
struct _tuple14 _tmp634;struct _tuple14 _tmp3C2=(_tmp634.f1=loc,((_tmp634.f2=1,_tmp634)));
int _tmp3C3=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3C1,Cyc_Tcpat_not_exhaust_warn,& _tmp3C2,Cyc_Tcpat_dummy_fn,0,& _tmp3C3);{
# 1621
int _tmp3C4=_tmp3C2.f2;_npop_handler(0);return _tmp3C4;};}
# 1615
;_pop_region(r);}
# 1629
static struct _tuple24*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1631
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple21*_tmp635;struct _tuple21*rhs=(_tmp635=_region_malloc(r,sizeof(*_tmp635)),((_tmp635->f1=0,((_tmp635->f2=(swc->pattern)->loc,_tmp635)))));
struct _tuple24*_tmp636;return(_tmp636=_region_malloc(r,sizeof(*_tmp636)),((_tmp636->f1=sp0,((_tmp636->f2=rhs,_tmp636)))));}
# 1635
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1638
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1640
struct _RegionHandle _tmp3CA=_new_region("r");struct _RegionHandle*r=& _tmp3CA;_push_region(r);
{struct Cyc_List_List*_tmp3CB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp3CC=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3CB);
int _tmp3CD=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3CC,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3CD);
# 1646
for(0;_tmp3CB != 0;_tmp3CB=_tmp3CB->tl){
struct _tuple24*_tmp3CE=(struct _tuple24*)_tmp3CB->hd;int _tmp3D0;unsigned int _tmp3D1;struct _tuple24*_tmp3CF=_tmp3CE;_tmp3D0=(_tmp3CF->f2)->f1;_tmp3D1=(_tmp3CF->f2)->f2;
if(!_tmp3D0){
{const char*_tmp639;void*_tmp638;(_tmp638=0,Cyc_Tcutil_terr(_tmp3D1,((_tmp639="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp639,sizeof(char),74))),_tag_dyneither(_tmp638,sizeof(void*),0)));}
break;}}}
# 1641
;_pop_region(r);}
