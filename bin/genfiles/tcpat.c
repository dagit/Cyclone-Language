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
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 935
void*Cyc_Absyn_compress_kb(void*);
# 940
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 942
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 945
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 947
extern void*Cyc_Absyn_sint_typ;
# 949
void*Cyc_Absyn_float_typ(int);
# 951
extern void*Cyc_Absyn_empty_effect;
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1095
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1104
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
# 74
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
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
{const char*_tmp3E1;void*_tmp3E0;(_tmp3E0=0,Cyc_Tcutil_terr(p->loc,((_tmp3E1="struct tag used without arguments in pattern",_tag_dyneither(_tmp3E1,sizeof(char),45))),_tag_dyneither(_tmp3E0,sizeof(void*),0)));}
_npop_handler(1);return;_LL10: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp1C=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1C->tag != 2)goto _LL12;else{_tmp1D=_tmp1C->f1;_tmp1E=_tmp1C->f2;}}_LL11:
# 63
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp3E4;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3E3;p->r=(void*)((_tmp3E3=_cycalloc(sizeof(*_tmp3E3)),((_tmp3E3[0]=((_tmp3E4.tag=8,((_tmp3E4.f1=_tmp1D,((_tmp3E4.f2=_tmp1E,((_tmp3E4.f3=0,((_tmp3E4.f4=0,_tmp3E4)))))))))),_tmp3E3))));}_npop_handler(1);return;_LL12: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1F=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp1F->tag != 3)goto _LL14;else{_tmp20=_tmp1F->f1;_tmp21=_tmp1F->f2;}}_LL13:
# 65
{struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct _tmp3E7;struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp3E6;p->r=(void*)((_tmp3E6=_cycalloc(sizeof(*_tmp3E6)),((_tmp3E6[0]=((_tmp3E7.tag=13,((_tmp3E7.f1=_tmp20,((_tmp3E7.f2=_tmp21,_tmp3E7)))))),_tmp3E6))));}
_npop_handler(1);return;_LL14: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp22=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp1A;if(_tmp22->tag != 4)goto _LL16;else{_tmp23=(void*)_tmp22->f1;_tmp24=_tmp22->f2;}}_LL15:
# 68
{struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct _tmp3EA;struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp3E9;p->r=(void*)((_tmp3E9=_cycalloc(sizeof(*_tmp3E9)),((_tmp3E9[0]=((_tmp3EA.tag=14,((_tmp3EA.f1=_tmp23,((_tmp3EA.f2=_tmp24,_tmp3EA)))))),_tmp3E9))));}
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
{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp3ED;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp3EC;p->r=(void*)((_tmp3EC=_cycalloc(sizeof(*_tmp3EC)),((_tmp3EC[0]=((_tmp3ED.tag=1,((_tmp3ED.f1=Cyc_Absyn_new_vardecl(_tmp3,(void*)& Cyc_Absyn_VoidType_val,0),((_tmp3ED.f2=
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0),_tmp3ED)))))),_tmp3EC))));}
return;_LL22:;_LL23:
{const char*_tmp3F0;void*_tmp3EF;(_tmp3EF=0,Cyc_Tcutil_terr(p->loc,((_tmp3F0="qualified variable in pattern",_tag_dyneither(_tmp3F0,sizeof(char),30))),_tag_dyneither(_tmp3EF,sizeof(void*),0)));}return;_LL1D:;};_LL3: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp4=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp4->tag != 16)goto _LL5;else{_tmp5=_tmp4->f1;_tmp6=_tmp4->f2;_tmp7=_tmp4->f3;}}_LL4:
# 85
{struct _handler_cons _tmp38;_push_handler(& _tmp38);{int _tmp3A=0;if(setjmp(_tmp38.handler))_tmp3A=1;if(!_tmp3A){
{struct _RegionHandle _tmp3B=_new_region("r");struct _RegionHandle*r=& _tmp3B;_push_region(r);
{void*_tmp3C=Cyc_Tcenv_lookup_ordinary(r,te,p->loc,_tmp5,0);void*_tmp3D=_tmp3C;struct Cyc_Absyn_Aggrdecl*_tmp3F;struct Cyc_Absyn_Datatypedecl*_tmp41;struct Cyc_Absyn_Datatypefield*_tmp42;_LL25: {struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp3E=(struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp3E->tag != 1)goto _LL27;else{_tmp3F=_tmp3E->f1;}}_LL26: {
# 89
struct Cyc_List_List*_tmp46=0;
for(0;_tmp6 != 0;_tmp6=_tmp6->tl){
struct _tuple15*_tmp3F3;struct Cyc_List_List*_tmp3F2;_tmp46=((_tmp3F2=_cycalloc(sizeof(*_tmp3F2)),((_tmp3F2->hd=((_tmp3F3=_cycalloc(sizeof(*_tmp3F3)),((_tmp3F3->f1=0,((_tmp3F3->f2=(struct Cyc_Absyn_Pat*)_tmp6->hd,_tmp3F3)))))),((_tmp3F2->tl=_tmp46,_tmp3F2))))));}
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3FD;struct Cyc_Absyn_Aggrdecl**_tmp3FC;struct Cyc_Absyn_AggrInfo*_tmp3FB;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp3FA;p->r=(void*)((_tmp3FA=_cycalloc(sizeof(*_tmp3FA)),((_tmp3FA[0]=((_tmp3FD.tag=7,((_tmp3FD.f1=((_tmp3FB=_cycalloc(sizeof(*_tmp3FB)),((_tmp3FB->aggr_info=Cyc_Absyn_KnownAggr(((_tmp3FC=_cycalloc(sizeof(*_tmp3FC)),((_tmp3FC[0]=_tmp3F,_tmp3FC))))),((_tmp3FB->targs=0,_tmp3FB)))))),((_tmp3FD.f2=0,((_tmp3FD.f3=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp46),((_tmp3FD.f4=_tmp7,_tmp3FD)))))))))),_tmp3FA))));}
_npop_handler(1);return;}_LL27: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp40=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp40->tag != 2)goto _LL29;else{_tmp41=_tmp40->f1;_tmp42=_tmp40->f2;}}_LL28:
{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct _tmp400;struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp3FF;p->r=(void*)((_tmp3FF=_cycalloc(sizeof(*_tmp3FF)),((_tmp3FF[0]=((_tmp400.tag=8,((_tmp400.f1=_tmp41,((_tmp400.f2=_tmp42,((_tmp400.f3=_tmp6,((_tmp400.f4=_tmp7,_tmp400)))))))))),_tmp3FF))));}_npop_handler(1);return;_LL29: {struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp43=(struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp43->tag != 3)goto _LL2B;}_LL2A:
 goto _LL2C;_LL2B: {struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp44=(struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp44->tag != 4)goto _LL2D;}_LL2C:
# 98
{const char*_tmp403;void*_tmp402;(_tmp402=0,Cyc_Tcutil_terr(p->loc,((_tmp403="enum tag used with arguments in pattern",_tag_dyneither(_tmp403,sizeof(char),40))),_tag_dyneither(_tmp402,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(1);return;_LL2D: {struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp45=(struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp3D;if(_tmp45->tag != 0)goto _LL24;}_LL2E:
 goto _LL24;_LL24:;}
# 87
;_pop_region(r);}
# 86
;_pop_handler();}else{void*_tmp39=(void*)_exn_thrown;void*_tmp52=_tmp39;void*_tmp54;_LL30: {struct Cyc_Dict_Absent_exn_struct*_tmp53=(struct Cyc_Dict_Absent_exn_struct*)_tmp52;if(_tmp53->tag != Cyc_Dict_Absent)goto _LL32;}_LL31:
# 103
 goto _LL2F;_LL32: _tmp54=_tmp52;_LL33:(void)_throw(_tmp54);_LL2F:;}};}
{const char*_tmp407;void*_tmp406[1];struct Cyc_String_pa_PrintArg_struct _tmp405;(_tmp405.tag=0,((_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp5)),((_tmp406[0]=& _tmp405,Cyc_Tcutil_terr(p->loc,((_tmp407="%s is not a constructor in pattern",_tag_dyneither(_tmp407,sizeof(char),35))),_tag_dyneither(_tmp406,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;_LL5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp8=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp8->tag != 7)goto _LL7;else{if(_tmp8->f1 != 0)goto _LL7;_tmp9=_tmp8->f2;_tmpA=_tmp8->f3;_tmpB=_tmp8->f4;}}_LL6:
# 109
 if(topt == 0){
const char*_tmp40A;void*_tmp409;(_tmp409=0,Cyc_Tcutil_terr(p->loc,((_tmp40A="cannot determine pattern type",_tag_dyneither(_tmp40A,sizeof(char),30))),_tag_dyneither(_tmp409,sizeof(void*),0)));}{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp5A=t;struct Cyc_Absyn_AggrInfo _tmp5C;_LL35: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp5B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp5A;if(_tmp5B->tag != 11)goto _LL37;else{_tmp5C=_tmp5B->f1;}}_LL36:
# 114
{struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp410;struct Cyc_Absyn_AggrInfo*_tmp40F;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp40E;p->r=(void*)((_tmp40E=_cycalloc(sizeof(*_tmp40E)),((_tmp40E[0]=((_tmp410.tag=7,((_tmp410.f1=((_tmp40F=_cycalloc(sizeof(*_tmp40F)),((_tmp40F[0]=_tmp5C,_tmp40F)))),((_tmp410.f2=_tmp9,((_tmp410.f3=_tmpA,((_tmp410.f4=_tmpB,_tmp410)))))))))),_tmp40E))));}
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LL34;_LL37:;_LL38:
# 118
{const char*_tmp414;void*_tmp413[1];struct Cyc_String_pa_PrintArg_struct _tmp412;(_tmp412.tag=0,((_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp413[0]=& _tmp412,Cyc_Tcutil_terr(p->loc,((_tmp414="pattern expects aggregate type instead of %s",_tag_dyneither(_tmp414,sizeof(char),45))),_tag_dyneither(_tmp413,sizeof(void*),1)))))));}
# 121
goto _LL34;_LL34:;}
# 123
return;};_LL7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmpC->tag != 7)goto _LL9;else{if(_tmpC->f1 == 0)goto _LL9;if((((_tmpC->f1)->aggr_info).UnknownAggr).tag != 1)goto _LL9;_tmpD=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f1;_tmpE=((struct _tuple4)(((_tmpC->f1)->aggr_info).UnknownAggr).val).f2;_tmpF=(_tmpC->f1)->targs;_tmp10=(struct Cyc_List_List**)& _tmpC->f2;_tmp11=_tmpC->f3;_tmp12=_tmpC->f4;}}_LL8:
# 126
{struct _handler_cons _tmp63;_push_handler(& _tmp63);{int _tmp65=0;if(setjmp(_tmp63.handler))_tmp65=1;if(!_tmp65){
{struct Cyc_Absyn_Aggrdecl**_tmp66=Cyc_Tcenv_lookup_aggrdecl(te,p->loc,_tmpE);
struct Cyc_Absyn_Aggrdecl*_tmp67=*_tmp66;
if(_tmp67->impl == 0){
{const char*_tmp41C;void*_tmp41B[1];const char*_tmp41A;const char*_tmp419;struct Cyc_String_pa_PrintArg_struct _tmp418;(_tmp418.tag=0,((_tmp418.f1=(struct _dyneither_ptr)(
_tmp67->kind == Cyc_Absyn_StructA?(_tmp419="struct",_tag_dyneither(_tmp419,sizeof(char),7)):((_tmp41A="union",_tag_dyneither(_tmp41A,sizeof(char),6)))),((_tmp41B[0]=& _tmp418,Cyc_Tcutil_terr(p->loc,((_tmp41C="can't destructure an abstract %s",_tag_dyneither(_tmp41C,sizeof(char),33))),_tag_dyneither(_tmp41B,sizeof(void*),1)))))));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
_npop_handler(0);return;}{
# 135
int more_exists=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars)- ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp10);
if(more_exists < 0){
{const char*_tmp41F;void*_tmp41E;(_tmp41E=0,Cyc_Tcutil_terr(p->loc,((_tmp41F="too many existentially bound type variables in pattern",_tag_dyneither(_tmp41F,sizeof(char),55))),_tag_dyneither(_tmp41E,sizeof(void*),0)));}{
struct Cyc_List_List**_tmp6F=_tmp10;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp67->impl))->exist_vars);for(0;n != 0;-- n){
_tmp6F=&((struct Cyc_List_List*)_check_null(*_tmp6F))->tl;}}
*_tmp6F=0;};}else{
if(more_exists > 0){
# 144
struct Cyc_List_List*_tmp70=0;
for(0;more_exists != 0;-- more_exists){
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp425;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp424;struct Cyc_List_List*_tmp423;_tmp70=((_tmp423=_cycalloc(sizeof(*_tmp423)),((_tmp423->hd=Cyc_Tcutil_new_tvar((void*)((_tmp425=_cycalloc(sizeof(*_tmp425)),((_tmp425[0]=((_tmp424.tag=1,((_tmp424.f1=0,_tmp424)))),_tmp425))))),((_tmp423->tl=_tmp70,_tmp423))))));}
*_tmp10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp10,_tmp70);}}{
# 149
struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp42B;struct Cyc_Absyn_AggrInfo*_tmp42A;struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp429;p->r=(void*)((_tmp429=_cycalloc(sizeof(*_tmp429)),((_tmp429[0]=((_tmp42B.tag=7,((_tmp42B.f1=((_tmp42A=_cycalloc(sizeof(*_tmp42A)),((_tmp42A->aggr_info=Cyc_Absyn_KnownAggr(_tmp66),((_tmp42A->targs=_tmpF,_tmp42A)))))),((_tmp42B.f2=*_tmp10,((_tmp42B.f3=_tmp11,((_tmp42B.f4=_tmp12,_tmp42B)))))))))),_tmp429))));};};}
# 127
;_pop_handler();}else{void*_tmp64=(void*)_exn_thrown;void*_tmp78=_tmp64;void*_tmp7A;_LL3A: {struct Cyc_Dict_Absent_exn_struct*_tmp79=(struct Cyc_Dict_Absent_exn_struct*)_tmp78;if(_tmp79->tag != Cyc_Dict_Absent)goto _LL3C;}_LL3B:
# 152
{const char*_tmp42E;void*_tmp42D;(_tmp42D=0,Cyc_Tcutil_terr(p->loc,((_tmp42E="Non-aggregate name has designator patterns",_tag_dyneither(_tmp42E,sizeof(char),43))),_tag_dyneither(_tmp42D,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;
goto _LL39;_LL3C: _tmp7A=_tmp78;_LL3D:(void)_throw(_tmp7A);_LL39:;}};}
# 156
return;_LL9: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp13=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1;if(_tmp13->tag != 17)goto _LLB;else{_tmp14=_tmp13->f1;}}_LLA:
# 158
 Cyc_Tcexp_tcExp(te,0,_tmp14);
if(!Cyc_Tcutil_is_const_exp(_tmp14)){
{const char*_tmp431;void*_tmp430;(_tmp430=0,Cyc_Tcutil_terr(p->loc,((_tmp431="non-constant expression in case pattern",_tag_dyneither(_tmp431,sizeof(char),40))),_tag_dyneither(_tmp430,sizeof(void*),0)));}
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 163
struct _tuple14 _tmp7F=Cyc_Evexp_eval_const_uint_exp(_tmp14);unsigned int _tmp81;int _tmp82;struct _tuple14 _tmp80=_tmp7F;_tmp81=_tmp80.f1;_tmp82=_tmp80.f2;
{struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp434;struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp433;p->r=(void*)((_tmp433=_cycalloc_atomic(sizeof(*_tmp433)),((_tmp433[0]=((_tmp434.tag=10,((_tmp434.f1=Cyc_Absyn_None,((_tmp434.f2=(int)_tmp81,_tmp434)))))),_tmp433))));}
return;};_LLB:;_LLC:
 return;_LL0:;}
# 170
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 173
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;{
struct Cyc_Core_Opt*_tmp435;return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435->v=s,_tmp435)))));};}
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
struct _tuple16*_tmp438;struct Cyc_List_List*_tmp437;*v_result_ptr=((_tmp437=_cycalloc(sizeof(*_tmp437)),((_tmp437->hd=((_tmp438=_cycalloc(sizeof(*_tmp438)),((_tmp438->f1=vd,((_tmp438->f2=e,_tmp438)))))),((_tmp437->tl=*v_result_ptr,_tmp437))))));};}
# 201
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 203
struct _tuple1*_tmp8D;struct Cyc_List_List*_tmp8E;struct Cyc_Tcpat_TcPatResult _tmp8C=res1;_tmp8D=_tmp8C.tvars_and_bounds_opt;_tmp8E=_tmp8C.patvars;{
struct _tuple1*_tmp90;struct Cyc_List_List*_tmp91;struct Cyc_Tcpat_TcPatResult _tmp8F=res2;_tmp90=_tmp8F.tvars_and_bounds_opt;_tmp91=_tmp8F.patvars;
if(_tmp8D != 0  || _tmp90 != 0){
if(_tmp8D == 0){
struct _tuple1*_tmp439;_tmp8D=((_tmp439=_cycalloc(sizeof(*_tmp439)),((_tmp439->f1=0,((_tmp439->f2=0,_tmp439))))));}
if(_tmp90 == 0){
struct _tuple1*_tmp43A;_tmp90=((_tmp43A=_cycalloc(sizeof(*_tmp43A)),((_tmp43A->f1=0,((_tmp43A->f2=0,_tmp43A))))));}{
struct _tuple1*_tmp43D;struct Cyc_Tcpat_TcPatResult _tmp43C;return(_tmp43C.tvars_and_bounds_opt=((_tmp43D=_cycalloc(sizeof(*_tmp43D)),((_tmp43D->f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f1,(*_tmp90).f1),((_tmp43D->f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp8D).f2,(*_tmp90).f2),_tmp43D)))))),((_tmp43C.patvars=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp43C)));};}{
# 214
struct Cyc_Tcpat_TcPatResult _tmp43E;return(_tmp43E.tvars_and_bounds_opt=0,((_tmp43E.patvars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8E,_tmp91),_tmp43E)));};};}
# 217
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
struct Cyc_Absyn_Pat*_tmp43F;return(_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F->loc=loc,((_tmp43F->topt=0,((_tmp43F->r=(void*)& Cyc_Absyn_Wild_p_val,_tmp43F)))))));}
# 222
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
struct Cyc_Core_Opt*_tmp440;return Cyc_Tcutil_is_array(t)?
Cyc_Tcutil_promote_array(t,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp440=_cycalloc(sizeof(*_tmp440)),((_tmp440->v=Cyc_Tcenv_lookup_type_vars(te),_tmp440))))): rgn_opt,0): t;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;int f2;};
# 229
static struct _tuple17*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
struct _tuple17*_tmp441;return(_tmp441=_cycalloc(sizeof(*_tmp441)),((_tmp441->f1=tv,((_tmp441->f2=0,_tmp441)))));}struct _tuple18{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 233
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 236
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult _tmp442;struct Cyc_Tcpat_TcPatResult res=(_tmp442.tvars_and_bounds_opt=0,((_tmp442.patvars=0,_tmp442)));
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
const char*_tmp445;void*_tmp444;(_tmp444=0,Cyc_Tcutil_terr(p->loc,((_tmp445="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp445,sizeof(char),58))),_tag_dyneither(_tmp444,sizeof(void*),0)));}
goto _LL72;_LL75:;_LL76:
# 260
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp448;void*_tmp447;(_tmp447=0,Cyc_Tcutil_terr(p->loc,((_tmp448="pattern would point to an abstract member",_tag_dyneither(_tmp448,sizeof(char),42))),_tag_dyneither(_tmp447,sizeof(void*),0)));}
goto _LL72;_LL72:;}
# 264
{struct Cyc_Absyn_Vardecl**_tmp449;Cyc_Tcpat_set_vd(((_tmp449=_cycalloc(sizeof(*_tmp449)),((_tmp449[0]=_tmp9E,_tmp449)))),access_exp,& res.patvars,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));}
goto _LL45;_LL4A: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmpA0=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA0->tag != 2)goto _LL4C;else{_tmpA1=_tmpA0->f1;_tmpA2=_tmpA0->f2;}}_LL4B: {
# 267
struct _RegionHandle _tmpD1=_new_region("r2");struct _RegionHandle*r2=& _tmpD1;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp44A[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp44A[0]=_tmpA1,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r2,_tag_dyneither(_tmp44A,sizeof(struct Cyc_Absyn_Tvar*),1)))));
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp44B;res.tvars_and_bounds_opt=((_tmp44B=_cycalloc(sizeof(*_tmp44B)),((_tmp44B->f1=0,((_tmp44B->f2=0,_tmp44B))))));}
{struct _tuple17*_tmp44E;struct Cyc_List_List*_tmp44D;(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((_tmp44D=_cycalloc(sizeof(*_tmp44D)),((_tmp44D->hd=((_tmp44E=_cycalloc(sizeof(*_tmp44E)),((_tmp44E->f1=_tmpA1,((_tmp44E->f2=1,_tmp44E)))))),((_tmp44D->tl=0,_tmp44D)))))));}
Cyc_Tcutil_check_type(p->loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_tmk,1,_tmpA2->type);}
# 277
if(topt != 0)t=*topt;else{
t=Cyc_Tcpat_any_type(Cyc_Tcenv_lookup_type_vars(te),topt);}
{void*_tmpD6=Cyc_Tcutil_compress(t);void*_tmpD7=_tmpD6;_LL78: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD7;if(_tmpD8->tag != 8)goto _LL7A;}_LL79:
# 281
 if(rgn_pat == 0  || !allow_ref_pat){
const char*_tmp451;void*_tmp450;(_tmp450=0,Cyc_Tcutil_terr(p->loc,((_tmp451="array reference would point into unknown/unallowed region",_tag_dyneither(_tmp451,sizeof(char),58))),_tag_dyneither(_tmp450,sizeof(void*),0)));}
goto _LL77;_LL7A:;_LL7B:
# 285
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk)){
const char*_tmp454;void*_tmp453;(_tmp453=0,Cyc_Tcutil_terr(p->loc,((_tmp454="pattern would point to an abstract member",_tag_dyneither(_tmp454,sizeof(char),42))),_tag_dyneither(_tmp453,sizeof(void*),0)));}
goto _LL77;_LL77:;}
# 289
{struct Cyc_Absyn_Vardecl**_tmp455;Cyc_Tcpat_set_vd(((_tmp455=_cycalloc(sizeof(*_tmp455)),((_tmp455[0]=_tmpA2,_tmp455)))),access_exp,& res.patvars,_tmpA2->type);}
_npop_handler(0);goto _LL45;
# 267
;_pop_region(r2);}_LL4C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmpA3=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA3->tag != 3)goto _LL4E;else{_tmpA4=_tmpA3->f1;_tmpA5=_tmpA3->f2;}}_LL4D:
# 293
 res=Cyc_Tcpat_tcPatRec(te,_tmpA5,topt,rgn_pat,allow_ref_pat,access_exp);
t=(void*)_check_null(_tmpA5->topt);
if(!allow_ref_pat  || rgn_pat == 0){
{const char*_tmp458;void*_tmp457;(_tmp457=0,Cyc_Tcutil_terr(p->loc,((_tmp458="* pattern would point into an unknown/unallowed region",_tag_dyneither(_tmp458,sizeof(char),55))),_tag_dyneither(_tmp457,sizeof(void*),0)));}
goto _LL45;}else{
# 300
struct _RegionHandle _tmpE0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE0;_push_region(rgn);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t)){
const char*_tmp45B;void*_tmp45A;(_tmp45A=0,Cyc_Tcutil_terr(p->loc,((_tmp45B="* pattern cannot take the address of an alias-free path",_tag_dyneither(_tmp45B,sizeof(char),56))),_tag_dyneither(_tmp45A,sizeof(void*),0)));}
# 301
;_pop_region(rgn);}{
# 306
struct Cyc_Absyn_Exp*new_access_exp=0;
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp461;struct Cyc_Absyn_PtrInfo _tmp460;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45F;void*t2=(void*)((_tmp45F=_cycalloc(sizeof(*_tmp45F)),((_tmp45F[0]=((_tmp461.tag=5,((_tmp461.f1=((_tmp460.elt_typ=t,((_tmp460.elt_tq=Cyc_Absyn_empty_tqual(0),((_tmp460.ptr_atts=(
((_tmp460.ptr_atts).rgn=rgn_pat,(((_tmp460.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp460.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp460.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp460.ptr_atts).ptrloc=0,_tmp460.ptr_atts)))))))))),_tmp460)))))),_tmp461)))),_tmp45F))));
# 311
if((unsigned int)access_exp){
new_access_exp=Cyc_Absyn_address_exp(access_exp,0);
new_access_exp->topt=t2;}
# 315
{struct Cyc_Absyn_Vardecl**_tmp462;Cyc_Tcpat_set_vd(((_tmp462=_cycalloc(sizeof(*_tmp462)),((_tmp462[0]=_tmpA4,_tmp462)))),new_access_exp,& res.patvars,t2);}
goto _LL45;};_LL4E: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmpA6=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpA6->tag != 4)goto _LL50;else{_tmpA7=_tmpA6->f1;_tmpA8=_tmpA6->f2;}}_LL4F:
# 321
{struct Cyc_Absyn_Vardecl**_tmp463;Cyc_Tcpat_set_vd(((_tmp463=_cycalloc(sizeof(*_tmp463)),((_tmp463[0]=_tmpA8,_tmp463)))),access_exp,& res.patvars,_tmpA8->type);}{
# 325
struct _RegionHandle _tmpE8=_new_region("r2");struct _RegionHandle*r2=& _tmpE8;_push_region(r2);{
struct Cyc_Absyn_Tvar*_tmp464[1];Cyc_Tcenv_add_type_vars(r2,p->loc,te,((_tmp464[0]=_tmpA7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp464,sizeof(struct Cyc_Absyn_Tvar*),1)))));}
# 328
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp465;res.tvars_and_bounds_opt=((_tmp465=_cycalloc(sizeof(*_tmp465)),((_tmp465->f1=0,((_tmp465->f2=0,_tmp465))))));}
{struct _tuple17*_tmp468;struct Cyc_List_List*_tmp467;(*res.tvars_and_bounds_opt).f1=(
(_tmp467=_cycalloc(sizeof(*_tmp467)),((_tmp467->hd=((_tmp468=_cycalloc(sizeof(*_tmp468)),((_tmp468->f1=_tmpA7,((_tmp468->f2=0,_tmp468)))))),((_tmp467->tl=(*res.tvars_and_bounds_opt).f1,_tmp467))))));}
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
{struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp46B;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp46A;t=Cyc_Tcpat_num_type(topt,(void*)((_tmp46A=_cycalloc(sizeof(*_tmp46A)),((_tmp46A[0]=((_tmp46B.tag=13,((_tmp46B.f1=_tmpB0->name,((_tmp46B.f2=_tmpB0,_tmp46B)))))),_tmp46A)))));}
goto _LL45;_LL5C: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmpB1=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB1->tag != 14)goto _LL5E;else{_tmpB2=(void*)_tmpB1->f1;}}_LL5D:
 t=Cyc_Tcpat_num_type(topt,_tmpB2);goto _LL45;_LL5E: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmpB3=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB3->tag != 9)goto _LL60;}_LL5F:
# 345
 if(topt != 0){
void*_tmpEF=Cyc_Tcutil_compress(*topt);void*_tmpF0=_tmpEF;_LL7D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpF0;if(_tmpF1->tag != 5)goto _LL7F;}_LL7E:
 t=*topt;goto tcpat_end;_LL7F:;_LL80:
 goto _LL7C;_LL7C:;}{
# 350
struct Cyc_Core_Opt*_tmpF2=Cyc_Tcenv_lookup_opt_type_vars(te);
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp471;struct Cyc_Absyn_PtrInfo _tmp470;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp46F;t=(void*)((_tmp46F=_cycalloc(sizeof(*_tmp46F)),((_tmp46F[0]=((_tmp471.tag=5,((_tmp471.f1=((_tmp470.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmpF2),((_tmp470.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp470.ptr_atts=(
((_tmp470.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmpF2),(((_tmp470.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp470.ptr_atts).bounds=
# 355
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp470.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp470.ptr_atts).ptrloc=0,_tmp470.ptr_atts)))))))))),_tmp470)))))),_tmp471)))),_tmp46F))));}
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
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp47B;struct Cyc_Absyn_Datatypedecl**_tmp47A;struct Cyc_Absyn_DatatypeInfo _tmp479;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp478;void*new_type=(void*)((_tmp478=_cycalloc(sizeof(*_tmp478)),((_tmp478[0]=((_tmp47B.tag=3,((_tmp47B.f1=((_tmp479.datatype_info=Cyc_Absyn_KnownDatatype(((_tmp47A=_cycalloc(sizeof(*_tmp47A)),((_tmp47A[0]=_tmpFF,_tmp47A))))),((_tmp479.targs=_tmp101,_tmp479)))),_tmp47B)))),_tmp478))));
# 400
_tmpB5->topt=new_type;
{struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp481;struct Cyc_Absyn_PtrInfo _tmp480;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47F;t=(void*)((_tmp47F=_cycalloc(sizeof(*_tmp47F)),((_tmp47F[0]=((_tmp481.tag=5,((_tmp481.f1=((_tmp480.elt_typ=new_type,((_tmp480.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp480.ptr_atts=(
((_tmp480.ptr_atts).rgn=ptr_rgn,(((_tmp480.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp480.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp480.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp480.ptr_atts).ptrloc=0,_tmp480.ptr_atts)))))))))),_tmp480)))))),_tmp481)))),_tmp47F))));}
# 407
goto _LL86;};_LL89:;_LL8A:
# 409
 DONT_PROMOTE: {
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp487;struct Cyc_Absyn_PtrInfo _tmp486;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp485;t=(void*)((_tmp485=_cycalloc(sizeof(*_tmp485)),((_tmp485[0]=((_tmp487.tag=5,((_tmp487.f1=((_tmp486.elt_typ=(void*)_check_null(_tmpB5->topt),((_tmp486.elt_tq=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0),((_tmp486.ptr_atts=(
((_tmp486.ptr_atts).rgn=ptr_rgn,(((_tmp486.ptr_atts).nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp486.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp486.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp486.ptr_atts).ptrloc=0,_tmp486.ptr_atts)))))))))),_tmp486)))))),_tmp487)))),_tmp485))));}_LL86:;}
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
struct Cyc_List_List*_tmp488;wild_ps=((_tmp488=_cycalloc(sizeof(*_tmp488)),((_tmp488->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp488->tl=wild_ps,_tmp488))))));}}
*_tmpB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp10F,wild_ps);
_tmp10F=*_tmpB7;}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10F)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp113)){
const char*_tmp48B;void*_tmp48A;(_tmp48A=0,Cyc_Tcutil_warn(p->loc,((_tmp48B="unnecessary ... in tuple pattern",_tag_dyneither(_tmp48B,sizeof(char),33))),_tag_dyneither(_tmp48A,sizeof(void*),0)));}}}
# 442
goto _LL90;_LL93:;_LL94:
# 444
 goto _LL90;_LL90:;}else{
# 446
if(_tmpB8){
const char*_tmp48E;void*_tmp48D;(_tmp48D=0,Cyc_Tcutil_terr(p->loc,((_tmp48E="cannot determine missing fields for ... in tuple pattern",_tag_dyneither(_tmp48E,sizeof(char),57))),_tag_dyneither(_tmp48D,sizeof(void*),0)));}}
{int i=0;for(0;_tmp10F != 0;(_tmp10F=_tmp10F->tl,i ++)){
void**_tmp11B=0;
if(topt_ts != 0){
_tmp11B=&(*((struct _tuple18*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 454
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp){
struct _tuple7 _tmp491;union Cyc_Absyn_Cnst _tmp490;new_access_exp=Cyc_Absyn_subscript_exp(access_exp,
Cyc_Absyn_const_exp((((_tmp490.Int_c).val=((_tmp491.f1=Cyc_Absyn_Unsigned,((_tmp491.f2=i,_tmp491)))),(((_tmp490.Int_c).tag=5,_tmp490)))),0),0);}
# 459
res=Cyc_Tcpat_combine_results(res,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp10F->hd,_tmp11B,rgn_pat,allow_ref_pat,new_access_exp));
# 462
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp10F->hd)->topt;{
struct _tuple18*_tmp494;struct Cyc_List_List*_tmp493;pat_ts=((_tmp493=_cycalloc(sizeof(*_tmp493)),((_tmp493->hd=((_tmp494=_cycalloc(sizeof(*_tmp494)),((_tmp494->f1=Cyc_Absyn_empty_tqual(0),((_tmp494->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp10F->hd)->topt),_tmp494)))))),((_tmp493->tl=pat_ts,_tmp493))))));};};}}
# 465
{struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp497;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp496;t=(void*)((_tmp496=_cycalloc(sizeof(*_tmp496)),((_tmp496[0]=((_tmp497.tag=10,((_tmp497.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts),_tmp497)))),_tmp496))));}
goto _LL45;}_LL64: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmpB9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp9B;if(_tmpB9->tag != 7)goto _LL66;else{if(_tmpB9->f1 == 0)goto _LL66;if((((_tmpB9->f1)->aggr_info).KnownAggr).tag != 2)goto _LL66;_tmpBA=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpB9->f1)->aggr_info).KnownAggr).val);_tmpBB=(struct Cyc_List_List**)&(_tmpB9->f1)->targs;_tmpBC=_tmpB9->f2;_tmpBD=(struct Cyc_List_List**)& _tmpB9->f3;_tmpBE=_tmpB9->f4;}}_LL65: {
# 469
struct Cyc_List_List*_tmp122=*_tmpBD;
const char*_tmp499;const char*_tmp498;struct _dyneither_ptr aggr_str=_tmpBA->kind == Cyc_Absyn_StructA?(_tmp499="struct",_tag_dyneither(_tmp499,sizeof(char),7)):((_tmp498="union",_tag_dyneither(_tmp498,sizeof(char),6)));
if(_tmpBA->impl == 0){
{const char*_tmp49D;void*_tmp49C[1];struct Cyc_String_pa_PrintArg_struct _tmp49B;(_tmp49B.tag=0,((_tmp49B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp49C[0]=& _tmp49B,Cyc_Tcutil_terr(p->loc,((_tmp49D="can't destructure an abstract %s",_tag_dyneither(_tmp49D,sizeof(char),33))),_tag_dyneither(_tmp49C,sizeof(void*),1)))))));}
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
const char*_tmp4A0;void*_tmp49F;(_tmp49F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4A0="unconstrained existential type variable in aggregate",_tag_dyneither(_tmp4A0,sizeof(char),53))),_tag_dyneither(_tmp49F,sizeof(void*),0)));}_LL95:;}
# 505
{void*_tmp132=_tmp129;struct Cyc_Absyn_Kind*_tmp134;struct Cyc_Core_Opt**_tmp136;struct Cyc_Absyn_Kind*_tmp137;struct Cyc_Core_Opt**_tmp139;_LL9D: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp133=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp133->tag != 0)goto _LL9F;else{_tmp134=_tmp133->f1;}}_LL9E:
# 508
 if(!Cyc_Tcutil_kind_leq(k2,_tmp134))
error=1;
goto _LL9C;_LL9F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp135=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp135->tag != 2)goto _LLA1;else{_tmp136=(struct Cyc_Core_Opt**)& _tmp135->f1;_tmp137=_tmp135->f2;}}_LLA0:
 _tmp139=_tmp136;goto _LLA2;_LLA1: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp138=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp132;if(_tmp138->tag != 1)goto _LL9C;else{_tmp139=(struct Cyc_Core_Opt**)& _tmp138->f1;}}_LLA2:
{struct Cyc_Core_Opt*_tmp4A1;*_tmp139=((_tmp4A1=_cycalloc(sizeof(*_tmp4A1)),((_tmp4A1->v=_tmp12A,_tmp4A1))));}goto _LL9C;_LL9C:;}
# 514
if(error){
const char*_tmp4A7;void*_tmp4A6[3];struct Cyc_String_pa_PrintArg_struct _tmp4A5;struct Cyc_String_pa_PrintArg_struct _tmp4A4;struct Cyc_String_pa_PrintArg_struct _tmp4A3;(_tmp4A3.tag=0,((_tmp4A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 518
Cyc_Absynpp_kind2string(k2)),((_tmp4A4.tag=0,((_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmp129)),((_tmp4A5.tag=0,((_tmp4A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp4A6[0]=& _tmp4A5,((_tmp4A6[1]=& _tmp4A4,((_tmp4A6[2]=& _tmp4A3,Cyc_Tcutil_terr(p->loc,((_tmp4A7="type variable %s has kind %s but must have at least kind %s",_tag_dyneither(_tmp4A7,sizeof(char),60))),_tag_dyneither(_tmp4A6,sizeof(void*),3)))))))))))))))))));}{
# 520
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4AA;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4A9;void*vartype=(void*)((_tmp4A9=_cycalloc(sizeof(*_tmp4A9)),((_tmp4A9[0]=((_tmp4AA.tag=2,((_tmp4AA.f1=tv,_tmp4AA)))),_tmp4A9))));
{struct Cyc_List_List*_tmp4AB;_tmp127=((_tmp4AB=_region_malloc(rgn,sizeof(*_tmp4AB)),((_tmp4AB->hd=vartype,((_tmp4AB->tl=_tmp127,_tmp4AB))))));}
# 524
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable){
struct _tuple0*_tmp4AE;struct Cyc_List_List*_tmp4AD;outlives_constraints=(
(_tmp4AD=_cycalloc(sizeof(*_tmp4AD)),((_tmp4AD->hd=((_tmp4AE=_cycalloc(sizeof(*_tmp4AE)),((_tmp4AE->f1=Cyc_Absyn_empty_effect,((_tmp4AE->f2=vartype,_tmp4AE)))))),((_tmp4AD->tl=outlives_constraints,_tmp4AD))))));}else{
# 529
const char*_tmp4B1;void*_tmp4B0;(_tmp4B0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp4B1="opened existential had unique or top region kind",_tag_dyneither(_tmp4B1,sizeof(char),49))),_tag_dyneither(_tmp4B0,sizeof(void*),0)));}}};};}}
# 533
_tmp127=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp127);{
# 535
struct _RegionHandle _tmp147=_new_region("r2");struct _RegionHandle*r2=& _tmp147;_push_region(r2);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r2,p->loc,te,_tmpBC);
# 538
struct Cyc_List_List*_tmp148=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmp4B2;struct _tuple12 _tmp149=(_tmp4B2.f1=_tmp148,((_tmp4B2.f2=rgn,_tmp4B2)));
struct Cyc_List_List*_tmp14A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp149,_tmpBA->tvs);
struct Cyc_List_List*_tmp14B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->exist_vars,_tmp127);
struct Cyc_List_List*_tmp14C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp14A);
struct Cyc_List_List*_tmp14D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp14B);
struct Cyc_List_List*_tmp14E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp14A,_tmp14B);
# 546
if(_tmpBC != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0){
struct _tuple1*_tmp4B3;res.tvars_and_bounds_opt=((_tmp4B3=_cycalloc(sizeof(*_tmp4B3)),((_tmp4B3->f1=0,((_tmp4B3->f2=0,_tmp4B3))))));}
(*res.tvars_and_bounds_opt).f1=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f1,((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmpBC));
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);{
struct Cyc_List_List*_tmp150=0;
{struct Cyc_List_List*_tmp151=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->rgn_po;for(0;_tmp151 != 0;_tmp151=_tmp151->tl){
struct _tuple0*_tmp4B6;struct Cyc_List_List*_tmp4B5;_tmp150=((_tmp4B5=_cycalloc(sizeof(*_tmp4B5)),((_tmp4B5->hd=((_tmp4B6=_cycalloc(sizeof(*_tmp4B6)),((_tmp4B6->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp14E,(*((struct _tuple0*)_tmp151->hd)).f1),((_tmp4B6->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp14E,(*((struct _tuple0*)_tmp151->hd)).f2),_tmp4B6)))))),((_tmp4B5->tl=_tmp150,_tmp4B5))))));}}
# 558
_tmp150=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp150);
(*res.tvars_and_bounds_opt).f2=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmp150);};}
# 563
*_tmpBB=_tmp14C;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp4C0;struct Cyc_Absyn_Aggrdecl**_tmp4BF;struct Cyc_Absyn_AggrInfo _tmp4BE;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4BD;t=(void*)((_tmp4BD=_cycalloc(sizeof(*_tmp4BD)),((_tmp4BD[0]=((_tmp4C0.tag=11,((_tmp4C0.f1=((_tmp4BE.aggr_info=Cyc_Absyn_KnownAggr(((_tmp4BF=_cycalloc(sizeof(*_tmp4BF)),((_tmp4BF[0]=_tmpBA,_tmp4BF))))),((_tmp4BE.targs=*_tmpBB,_tmp4BE)))),_tmp4C0)))),_tmp4BD))));}
if(_tmpBE  && _tmpBA->kind == Cyc_Absyn_UnionA){
const char*_tmp4C3;void*_tmp4C2;(_tmp4C2=0,Cyc_Tcutil_warn(p->loc,((_tmp4C3="`...' pattern not allowed in union pattern",_tag_dyneither(_tmp4C3,sizeof(char),43))),_tag_dyneither(_tmp4C2,sizeof(void*),0)));}else{
if(_tmpBE){
# 569
int _tmp15A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp122);
int _tmp15B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBA->impl))->fields);
if(_tmp15A < _tmp15B){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmp15B - _tmp15A;++ i){
struct _tuple15*_tmp4C6;struct Cyc_List_List*_tmp4C5;wild_dps=((_tmp4C5=_cycalloc(sizeof(*_tmp4C5)),((_tmp4C5->hd=((_tmp4C6=_cycalloc(sizeof(*_tmp4C6)),((_tmp4C6->f1=0,((_tmp4C6->f2=Cyc_Tcpat_wild_pat(p->loc),_tmp4C6)))))),((_tmp4C5->tl=wild_dps,_tmp4C5))))));}}
*_tmpBD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp122,wild_dps);
_tmp122=*_tmpBD;}else{
if(_tmp15A == _tmp15B){
const char*_tmp4C9;void*_tmp4C8;(_tmp4C8=0,Cyc_Tcutil_warn(p->loc,((_tmp4C9="unnecessary ... in struct pattern",_tag_dyneither(_tmp4C9,sizeof(char),34))),_tag_dyneither(_tmp4C8,sizeof(void*),0)));}}}}{
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
const char*_tmp4D0;void*_tmp4CF[4];struct Cyc_String_pa_PrintArg_struct _tmp4CE;struct Cyc_String_pa_PrintArg_struct _tmp4CD;struct Cyc_String_pa_PrintArg_struct _tmp4CC;struct Cyc_String_pa_PrintArg_struct _tmp4CB;(_tmp4CB.tag=0,((_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 597
Cyc_Absynpp_typ2string((void*)_check_null(_tmp163->topt))),((_tmp4CC.tag=0,((_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 596
Cyc_Absynpp_typ2string(_tmp164)),((_tmp4CD.tag=0,((_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp4CE.tag=0,((_tmp4CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp162->name),((_tmp4CF[0]=& _tmp4CE,((_tmp4CF[1]=& _tmp4CD,((_tmp4CF[2]=& _tmp4CC,((_tmp4CF[3]=& _tmp4CB,Cyc_Tcutil_terr(p->loc,((_tmp4D0="field %s of %s pattern expects type %s != %s",_tag_dyneither(_tmp4D0,sizeof(char),45))),_tag_dyneither(_tmp4CF,sizeof(void*),4)))))))))))))))))))))))));}
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
struct _tuple12 _tmp4D1;struct _tuple12 _tmp171=(_tmp4D1.f1=_tmp170,((_tmp4D1.f2=rgn,_tmp4D1)));
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp171,_tmpC0->tvs);
struct Cyc_List_List*_tmp173=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp172);
{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4D7;struct Cyc_Absyn_DatatypeFieldInfo _tmp4D6;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4D5;t=(void*)((_tmp4D5=_cycalloc(sizeof(*_tmp4D5)),((_tmp4D5[0]=((_tmp4D7.tag=4,((_tmp4D7.f1=((_tmp4D6.field_info=Cyc_Absyn_KnownDatatypefield(_tmpC0,_tmpC1),((_tmp4D6.targs=_tmp173,_tmp4D6)))),_tmp4D7)))),_tmp4D5))));}
# 615
if(_tmpC3){
# 617
int _tmp177=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp16E);
int _tmp178=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmp177 < _tmp178){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp178 - _tmp177;++ i){
struct Cyc_List_List*_tmp4D8;wild_ps=((_tmp4D8=_cycalloc(sizeof(*_tmp4D8)),((_tmp4D8->hd=Cyc_Tcpat_wild_pat(p->loc),((_tmp4D8->tl=wild_ps,_tmp4D8))))));}}
*_tmpC2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp16E,wild_ps);
_tmp16E=*_tmpC2;}else{
if(_tmp177 == _tmp178){
const char*_tmp4DC;void*_tmp4DB[1];struct Cyc_String_pa_PrintArg_struct _tmp4DA;(_tmp4DA.tag=0,((_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC0->name)),((_tmp4DB[0]=& _tmp4DA,Cyc_Tcutil_warn(p->loc,((_tmp4DC="unnecessary ... in datatype field %s",_tag_dyneither(_tmp4DC,sizeof(char),37))),_tag_dyneither(_tmp4DB,sizeof(void*),1)))))));}}}
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
const char*_tmp4E2;void*_tmp4E1[3];struct Cyc_String_pa_PrintArg_struct _tmp4E0;struct Cyc_String_pa_PrintArg_struct _tmp4DF;struct Cyc_String_pa_PrintArg_struct _tmp4DE;(_tmp4DE.tag=0,((_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 646
Cyc_Absynpp_typ2string((void*)_check_null(_tmp17D->topt))),((_tmp4DF.tag=0,((_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 645
Cyc_Absynpp_typ2string(_tmp17E)),((_tmp4E0.tag=0,((_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4E1[0]=& _tmp4E0,((_tmp4E1[1]=& _tmp4DF,((_tmp4E1[2]=& _tmp4DE,Cyc_Tcutil_terr(_tmp17D->loc,((_tmp4E2="%s expects argument type %s, not %s",_tag_dyneither(_tmp4E2,sizeof(char),36))),_tag_dyneither(_tmp4E1,sizeof(void*),3)))))))))))))))))));}}
# 648
if(_tmp16E != 0){
const char*_tmp4E6;void*_tmp4E5[1];struct Cyc_String_pa_PrintArg_struct _tmp4E4;(_tmp4E4.tag=0,((_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4E5[0]=& _tmp4E4,Cyc_Tcutil_terr(p->loc,((_tmp4E6="too many arguments for datatype constructor %s",_tag_dyneither(_tmp4E6,sizeof(char),47))),_tag_dyneither(_tmp4E5,sizeof(void*),1)))))));}
if(tqts != 0){
const char*_tmp4EA;void*_tmp4E9[1];struct Cyc_String_pa_PrintArg_struct _tmp4E8;(_tmp4E8.tag=0,((_tmp4E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpC1->name)),((_tmp4E9[0]=& _tmp4E8,Cyc_Tcutil_terr(p->loc,((_tmp4EA="too few arguments for datatype constructor %s",_tag_dyneither(_tmp4EA,sizeof(char),46))),_tag_dyneither(_tmp4E9,sizeof(void*),1)))))));}}
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
if((struct Cyc_Absyn_Vardecl**)x->hd != 0){struct Cyc_List_List*_tmp4EB;_tmp191=((_tmp4EB=_region_malloc(r,sizeof(*_tmp4EB)),((_tmp4EB->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd)),((_tmp4EB->tl=_tmp191,_tmp4EB))))));}}}
{const char*_tmp4EC;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Tcpat_get_name,_tmp191),p->loc,(
(_tmp4EC="pattern contains a repeated variable",_tag_dyneither(_tmp4EC,sizeof(char),37))));}{
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
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 698
struct _tuple0 _tmp4ED;struct _tuple0 _tmp199=(_tmp4ED.f1=Cyc_Tcutil_compress(old_type),((_tmp4ED.f2=Cyc_Tcutil_compress(new_type),_tmp4ED)));struct _tuple0 _tmp19A=_tmp199;struct Cyc_Absyn_PtrInfo _tmp19C;struct Cyc_Absyn_PtrInfo _tmp19E;_LLA4:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A.f1;if(_tmp19B->tag != 5)goto _LLA6;else{_tmp19C=_tmp19B->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19A.f2;if(_tmp19D->tag != 5)goto _LLA6;else{_tmp19E=_tmp19D->f1;}};_LLA5: {
# 700
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4F3;struct Cyc_Absyn_PtrInfo _tmp4F2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4F1;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp19F=(_tmp4F1=_cycalloc(sizeof(*_tmp4F1)),((_tmp4F1[0]=((_tmp4F3.tag=5,((_tmp4F3.f1=((_tmp4F2.elt_typ=_tmp19C.elt_typ,((_tmp4F2.elt_tq=_tmp19E.elt_tq,((_tmp4F2.ptr_atts=(
((_tmp4F2.ptr_atts).rgn=(_tmp19C.ptr_atts).rgn,(((_tmp4F2.ptr_atts).nullable=(_tmp19E.ptr_atts).nullable,(((_tmp4F2.ptr_atts).bounds=(_tmp19E.ptr_atts).bounds,(((_tmp4F2.ptr_atts).zero_term=(_tmp19E.ptr_atts).zero_term,(((_tmp4F2.ptr_atts).ptrloc=(_tmp19C.ptr_atts).ptrloc,_tmp4F2.ptr_atts)))))))))),_tmp4F2)))))),_tmp4F3)))),_tmp4F1)));
# 706
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp19F,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp19F);}_LLA6:;_LLA7:
 return 0;_LLA3:;}
# 715
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 718
struct _tuple0*_tmp4FD;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4FC;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4FB;struct Cyc_List_List*_tmp4FA;struct Cyc_List_List*assump=(_tmp4FA=_cycalloc(sizeof(*_tmp4FA)),((_tmp4FA->hd=((_tmp4FD=_cycalloc(sizeof(*_tmp4FD)),((_tmp4FD->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp4FD->f2=(void*)((_tmp4FB=_cycalloc(sizeof(*_tmp4FB)),((_tmp4FB[0]=((_tmp4FC.tag=2,((_tmp4FC.f1=tv,_tmp4FC)))),_tmp4FB)))),_tmp4FD)))))),((_tmp4FA->tl=0,_tmp4FA)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 736 "tcpat.cyc"
struct _tuple0 _tmp4FE;struct _tuple0 _tmp1A4=(_tmp4FE.f1=Cyc_Tcutil_compress(old_type),((_tmp4FE.f2=Cyc_Tcutil_compress(new_type),_tmp4FE)));struct _tuple0 _tmp1A5=_tmp1A4;struct Cyc_Absyn_PtrInfo _tmp1A7;struct Cyc_Absyn_PtrInfo _tmp1A9;_LLA9:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5.f1;if(_tmp1A6->tag != 5)goto _LLAB;else{_tmp1A7=_tmp1A6->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A5.f2;if(_tmp1A8->tag != 5)goto _LLAB;else{_tmp1A9=_tmp1A8->f1;}};_LLAA:
 goto _LLA8;_LLAB:;_LLAC:
{const char*_tmp501;void*_tmp500;(_tmp500=0,Cyc_Tcutil_terr(loc,((_tmp501="alias requires pointer type",_tag_dyneither(_tmp501,sizeof(char),28))),_tag_dyneither(_tmp500,sizeof(void*),0)));}goto _LLA8;_LLA8:;}else{
# 741
const char*_tmp506;void*_tmp505[2];struct Cyc_String_pa_PrintArg_struct _tmp504;struct Cyc_String_pa_PrintArg_struct _tmp503;(_tmp503.tag=0,((_tmp503.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp504.tag=0,((_tmp504.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp505[0]=& _tmp504,((_tmp505[1]=& _tmp503,Cyc_Tcutil_terr(loc,((_tmp506="cannot alias value of type %s to type %s",_tag_dyneither(_tmp506,sizeof(char),41))),_tag_dyneither(_tmp505,sizeof(void*),2)))))))))))));}}
# 748
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 752
void*_tmp1B5=p->r;void*_tmp1B6=_tmp1B5;struct Cyc_Absyn_Pat*_tmp1B8;struct Cyc_Absyn_AggrInfo*_tmp1BA;struct Cyc_List_List*_tmp1BB;struct Cyc_List_List*_tmp1BC;struct Cyc_List_List*_tmp1BE;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Vardecl*_tmp1C2;struct Cyc_Absyn_Pat*_tmp1C3;struct Cyc_Absyn_Vardecl*_tmp1C5;struct Cyc_Absyn_Pat*_tmp1C6;struct Cyc_Absyn_Tvar*_tmp1C8;struct Cyc_Absyn_Vardecl*_tmp1C9;_LLAE: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp1B7=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1B7->tag != 6)goto _LLB0;else{_tmp1B8=_tmp1B7->f1;}}_LLAF: {
# 754
void*_tmp1CA=(void*)_check_null(p->topt);void*_tmp1CB=_tmp1CA;void*_tmp1CD;_LLBF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1CC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CB;if(_tmp1CC->tag != 5)goto _LLC1;else{_tmp1CD=((_tmp1CC->f1).ptr_atts).rgn;}}_LLC0:
# 756
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp1CD);
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1B8,Cyc_Tcutil_is_noalias_region(_tmp1CD,0),patvars);
return;_LLC1:;_LLC2: {
const char*_tmp509;void*_tmp508;(_tmp508=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp509="check_pat_regions: bad pointer type",_tag_dyneither(_tmp509,sizeof(char),36))),_tag_dyneither(_tmp508,sizeof(void*),0)));}_LLBE:;}_LLB0: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp1B9=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1B9->tag != 7)goto _LLB2;else{_tmp1BA=_tmp1B9->f1;_tmp1BB=_tmp1B9->f2;_tmp1BC=_tmp1B9->f3;}}_LLB1:
# 762
 for(0;_tmp1BC != 0;_tmp1BC=_tmp1BC->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple15*)_tmp1BC->hd)).f2,did_noalias_deref,patvars);}
return;_LLB2: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp1BD=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1BD->tag != 8)goto _LLB4;else{_tmp1BE=_tmp1BD->f3;}}_LLB3:
# 766
 did_noalias_deref=0;_tmp1C0=_tmp1BE;goto _LLB5;_LLB4: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp1BF=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1BF->tag != 5)goto _LLB6;else{_tmp1C0=_tmp1BF->f1;}}_LLB5:
# 768
 for(0;_tmp1C0 != 0;_tmp1C0=_tmp1C0->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp1C0->hd,did_noalias_deref,patvars);}
return;_LLB6: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp1C1=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C1->tag != 3)goto _LLB8;else{_tmp1C2=_tmp1C1->f1;_tmp1C3=_tmp1C1->f2;}}_LLB7:
# 772
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1D0=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1D2;struct Cyc_Absyn_Exp*_tmp1D3;struct _tuple16*_tmp1D1=_tmp1D0;_tmp1D2=_tmp1D1->f1;_tmp1D3=_tmp1D1->f2;
# 778
if((_tmp1D2 != 0  && *_tmp1D2 == _tmp1C2) && _tmp1D3 != 0){
{void*_tmp1D4=_tmp1D3->r;void*_tmp1D5=_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D7;_LLC4: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp1D6=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1D5;if(_tmp1D6->tag != 14)goto _LLC6;else{_tmp1D7=_tmp1D6->f1;}}_LLC5:
# 781
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcenv_get_fnrgn(te),(void*)_check_null(_tmp1D7->topt))){
# 783
const char*_tmp50C;void*_tmp50B;(_tmp50B=0,Cyc_Tcutil_terr(p->loc,((_tmp50C="reference pattern not allowed on alias-free pointers",_tag_dyneither(_tmp50C,sizeof(char),53))),_tag_dyneither(_tmp50B,sizeof(void*),0)));}
goto _LLC3;_LLC6:;_LLC7: {
# 786
const char*_tmp50F;void*_tmp50E;(_tmp50E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp50F="check_pat_regions: bad reference access expression",_tag_dyneither(_tmp50F,sizeof(char),51))),_tag_dyneither(_tmp50E,sizeof(void*),0)));}_LLC3:;}
# 788
break;}}}
# 791
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1C3,did_noalias_deref,patvars);
return;_LLB8: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1C4=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C4->tag != 1)goto _LLBA;else{_tmp1C5=_tmp1C4->f1;_tmp1C6=_tmp1C4->f2;}}_LLB9:
# 794
{void*_tmp1DC=p->topt;void*_tmp1DD=_tmp1DC;_LLC9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1DD;if(_tmp1DE == 0)goto _LLCB;else{if(_tmp1DE->tag != 8)goto _LLCB;}}_LLCA:
# 796
 if(did_noalias_deref){
{const char*_tmp512;void*_tmp511;(_tmp511=0,Cyc_Tcutil_terr(p->loc,((_tmp512="pattern to array would create alias of no-alias pointer",_tag_dyneither(_tmp512,sizeof(char),56))),_tag_dyneither(_tmp511,sizeof(void*),0)));}
return;}
# 800
goto _LLC8;_LLCB:;_LLCC:
 goto _LLC8;_LLC8:;}
# 803
Cyc_Tcpat_check_pat_regions_rec(te,_tmp1C6,did_noalias_deref,patvars);
return;_LLBA: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp1C7=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1B6;if(_tmp1C7->tag != 2)goto _LLBC;else{_tmp1C8=_tmp1C7->f1;_tmp1C9=_tmp1C7->f2;}}_LLBB:
# 806
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1E1=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E4;struct _tuple16*_tmp1E2=_tmp1E1;_tmp1E3=_tmp1E2->f1;_tmp1E4=_tmp1E2->f2;
# 810
if(_tmp1E3 != 0  && *_tmp1E3 == _tmp1C9){
if(_tmp1E4 == 0){
const char*_tmp515;void*_tmp514;(_tmp514=0,Cyc_Tcutil_terr(p->loc,((_tmp515="cannot alias pattern expression in datatype",_tag_dyneither(_tmp515,sizeof(char),44))),_tag_dyneither(_tmp514,sizeof(void*),0)));}else{
# 814
struct _RegionHandle _tmp1E7=_new_region("r");struct _RegionHandle*r=& _tmp1E7;_push_region(r);
{struct Cyc_Absyn_Tvar*_tmp516[1];struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,p->loc,te,((_tmp516[0]=_tmp1C8,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(r,_tag_dyneither(_tmp516,sizeof(struct Cyc_Absyn_Tvar*),1)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp519;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp518;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp518=_cycalloc(sizeof(*_tmp518)),((_tmp518[0]=((_tmp519.tag=2,((_tmp519.f1=_tmp1C8,_tmp519)))),_tmp518)))),0,1);}
# 818
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp1E4->topt),_tmp1C8,_tmp1C9->type,_tmp1E4);}
# 815
;_pop_region(r);}
# 822
break;}}}
# 825
goto _LLAD;_LLBC:;_LLBD:
 return;_LLAD:;}
# 841 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 843
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle _tmp1EB=_new_region("r");struct _RegionHandle*r=& _tmp1EB;_push_region(r);
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple16*_tmp1EC=(struct _tuple16*)x->hd;struct Cyc_Absyn_Vardecl**_tmp1EE;struct Cyc_Absyn_Exp*_tmp1EF;struct _tuple16*_tmp1ED=_tmp1EC;_tmp1EE=_tmp1ED->f1;_tmp1EF=_tmp1ED->f2;
if(_tmp1EF != 0){
struct Cyc_Absyn_Exp*_tmp1F0=_tmp1EF;
# 851
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(_tmp1F0->topt)) && !
Cyc_Tcutil_is_noalias_path(r,_tmp1F0)){
# 854
const char*_tmp526;void*_tmp525[1];const char*_tmp524;struct Cyc_String_pa_PrintArg_struct _tmp523;void*_tmp522[1];const char*_tmp521;struct Cyc_String_pa_PrintArg_struct _tmp520;(_tmp520.tag=0,((_tmp520.f1=(struct _dyneither_ptr)(
# 856
_tmp1EE != 0?(struct _dyneither_ptr)(
(_tmp523.tag=0,((_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1EE)->name)),((_tmp522[0]=& _tmp523,Cyc_aprintf(((_tmp521="for variable %s",_tag_dyneither(_tmp521,sizeof(char),16))),_tag_dyneither(_tmp522,sizeof(void*),1)))))))):(
(_tmp524="",_tag_dyneither(_tmp524,sizeof(char),1)))),((_tmp525[0]=& _tmp520,Cyc_Tcutil_terr(p->loc,((_tmp526="pattern %s dereferences a alias-free pointer from a non-unique path",_tag_dyneither(_tmp526,sizeof(char),68))),_tag_dyneither(_tmp525,sizeof(void*),1)))))));}}}}
# 845
;_pop_region(r);};}struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 902 "tcpat.cyc"
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){union Cyc_Tcpat_Name_value _tmp527;return((_tmp527.Name_v).val=s,(((_tmp527.Name_v).tag=1,_tmp527)));}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){union Cyc_Tcpat_Name_value _tmp528;return((_tmp528.Int_v).val=i,(((_tmp528.Int_v).tag=2,_tmp528)));}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;struct Cyc_Absyn_Pat*orig_pat;};
# 912
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 917
typedef void*Cyc_Tcpat_simple_pat_t;
# 920
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp1FA=c1->name;union Cyc_Tcpat_Name_value _tmp1FB=_tmp1FA;struct _dyneither_ptr _tmp1FC;int _tmp1FD;_LLCE: if((_tmp1FB.Name_v).tag != 1)goto _LLD0;_tmp1FC=(struct _dyneither_ptr)(_tmp1FB.Name_v).val;_LLCF: {
# 923
union Cyc_Tcpat_Name_value _tmp1FE=c2->name;union Cyc_Tcpat_Name_value _tmp1FF=_tmp1FE;struct _dyneither_ptr _tmp200;_LLD3: if((_tmp1FF.Name_v).tag != 1)goto _LLD5;_tmp200=(struct _dyneither_ptr)(_tmp1FF.Name_v).val;_LLD4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1FC,(struct _dyneither_ptr)_tmp200);_LLD5: if((_tmp1FF.Int_v).tag != 2)goto _LLD2;_LLD6:
 return - 1;_LLD2:;}_LLD0: if((_tmp1FB.Int_v).tag != 2)goto _LLCD;_tmp1FD=(int)(_tmp1FB.Int_v).val;_LLD1: {
# 928
union Cyc_Tcpat_Name_value _tmp201=c2->name;union Cyc_Tcpat_Name_value _tmp202=_tmp201;int _tmp203;_LLD8: if((_tmp202.Name_v).tag != 1)goto _LLDA;_LLD9:
 return 1;_LLDA: if((_tmp202.Int_v).tag != 2)goto _LLD7;_tmp203=(int)(_tmp202.Int_v).val;_LLDB:
 return _tmp1FD - _tmp203;_LLD7:;}_LLCD:;}
# 936
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(struct _RegionHandle*r){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(r,Cyc_Tcpat_compare_con);}
# 940
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 944
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp52B;struct Cyc_Tcpat_Con_s*_tmp52A;return(_tmp52A=_region_malloc(r,sizeof(*_tmp52A)),((_tmp52A->name=Cyc_Tcpat_Name_v(((_tmp52B="NULL",_tag_dyneither(_tmp52B,sizeof(char),5)))),((_tmp52A->arity=0,((_tmp52A->span=& Cyc_Tcpat_two_opt,((_tmp52A->orig_pat=p,_tmp52A)))))))));}
# 947
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp52E;struct Cyc_Tcpat_Con_s*_tmp52D;return(_tmp52D=_region_malloc(r,sizeof(*_tmp52D)),((_tmp52D->name=Cyc_Tcpat_Name_v(((_tmp52E="&",_tag_dyneither(_tmp52E,sizeof(char),2)))),((_tmp52D->arity=1,((_tmp52D->span=& Cyc_Tcpat_two_opt,((_tmp52D->orig_pat=p,_tmp52D)))))))));}
# 950
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
const char*_tmp531;struct Cyc_Tcpat_Con_s*_tmp530;return(_tmp530=_region_malloc(r,sizeof(*_tmp530)),((_tmp530->name=Cyc_Tcpat_Name_v(((_tmp531="&",_tag_dyneither(_tmp531,sizeof(char),2)))),((_tmp530->arity=1,((_tmp530->span=& Cyc_Tcpat_one_opt,((_tmp530->orig_pat=p,_tmp530)))))))));}
# 953
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp532;return(_tmp532=_region_malloc(r,sizeof(*_tmp532)),((_tmp532->name=Cyc_Tcpat_Int_v(i),((_tmp532->arity=0,((_tmp532->span=0,((_tmp532->orig_pat=p,_tmp532)))))))));}
# 956
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp533;return(_tmp533=_region_malloc(r,sizeof(*_tmp533)),((_tmp533->name=Cyc_Tcpat_Name_v(f),((_tmp533->arity=0,((_tmp533->span=0,((_tmp533->orig_pat=p,_tmp533)))))))));}
# 959
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_s*_tmp534;return(_tmp534=_region_malloc(r,sizeof(*_tmp534)),((_tmp534->name=Cyc_Tcpat_Int_v((int)c),((_tmp534->arity=0,((_tmp534->span=& Cyc_Tcpat_twofiftysix_opt,((_tmp534->orig_pat=p,_tmp534)))))))));}
# 962
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
const char*_tmp537;struct Cyc_Tcpat_Con_s*_tmp536;return(_tmp536=_region_malloc(r,sizeof(*_tmp536)),((_tmp536->name=Cyc_Tcpat_Name_v(((_tmp537="$",_tag_dyneither(_tmp537,sizeof(char),2)))),((_tmp536->arity=i,((_tmp536->span=& Cyc_Tcpat_one_opt,((_tmp536->orig_pat=p,_tmp536)))))))));}
# 967
static void*Cyc_Tcpat_null_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp53A;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp539;return(void*)((_tmp539=_region_malloc(r,sizeof(*_tmp539)),((_tmp539[0]=((_tmp53A.tag=1,((_tmp53A.f1=Cyc_Tcpat_null_con(r,p),((_tmp53A.f2=0,_tmp53A)))))),_tmp539))));}
# 970
static void*Cyc_Tcpat_int_pat(struct _RegionHandle*r,int i,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp53D;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp53C;return(void*)((_tmp53C=_region_malloc(r,sizeof(*_tmp53C)),((_tmp53C[0]=((_tmp53D.tag=1,((_tmp53D.f1=Cyc_Tcpat_int_con(r,i,p),((_tmp53D.f2=0,_tmp53D)))))),_tmp53C))));}
# 973
static void*Cyc_Tcpat_char_pat(struct _RegionHandle*r,char c,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp540;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp53F;return(void*)((_tmp53F=_region_malloc(r,sizeof(*_tmp53F)),((_tmp53F[0]=((_tmp540.tag=1,((_tmp540.f1=Cyc_Tcpat_char_con(r,c,p),((_tmp540.f2=0,_tmp540)))))),_tmp53F))));}
# 976
static void*Cyc_Tcpat_float_pat(struct _RegionHandle*r,struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp543;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp542;return(void*)((_tmp542=_region_malloc(r,sizeof(*_tmp542)),((_tmp542[0]=((_tmp543.tag=1,((_tmp543.f1=Cyc_Tcpat_float_con(r,f,p),((_tmp543.f2=0,_tmp543)))))),_tmp542))));}
# 979
static void*Cyc_Tcpat_null_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp549;struct Cyc_List_List*_tmp548;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp547;return(void*)((_tmp547=_region_malloc(r,sizeof(*_tmp547)),((_tmp547[0]=((_tmp549.tag=1,((_tmp549.f1=Cyc_Tcpat_null_ptr_con(r,p0),((_tmp549.f2=((_tmp548=_region_malloc(r,sizeof(*_tmp548)),((_tmp548->hd=p,((_tmp548->tl=0,_tmp548)))))),_tmp549)))))),_tmp547))));}
# 982
static void*Cyc_Tcpat_ptr_pat(struct _RegionHandle*r,void*p,struct Cyc_Absyn_Pat*p0){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp54F;struct Cyc_List_List*_tmp54E;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp54D;return(void*)((_tmp54D=_region_malloc(r,sizeof(*_tmp54D)),((_tmp54D[0]=((_tmp54F.tag=1,((_tmp54F.f1=Cyc_Tcpat_ptr_con(r,p0),((_tmp54F.f2=((_tmp54E=_region_malloc(r,sizeof(*_tmp54E)),((_tmp54E->hd=p,((_tmp54E->tl=0,_tmp54E)))))),_tmp54F)))))),_tmp54D))));}
# 985
static void*Cyc_Tcpat_tuple_pat(struct _RegionHandle*r,struct Cyc_List_List*ss,struct Cyc_Absyn_Pat*p){
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp552;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp551;return(void*)((_tmp551=_region_malloc(r,sizeof(*_tmp551)),((_tmp551[0]=((_tmp552.tag=1,((_tmp552.f1=Cyc_Tcpat_tuple_con(r,((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss),p),((_tmp552.f2=ss,_tmp552)))))),_tmp551))));}
# 988
static void*Cyc_Tcpat_con_pat(struct _RegionHandle*r,struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 990
struct Cyc_Tcpat_Con_s*_tmp553;struct Cyc_Tcpat_Con_s*c=(_tmp553=_region_malloc(r,sizeof(*_tmp553)),((_tmp553->name=Cyc_Tcpat_Name_v(con_name),((_tmp553->arity=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps),((_tmp553->span=span,((_tmp553->orig_pat=p,_tmp553)))))))));
struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp556;struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp555;return(void*)((_tmp555=_region_malloc(r,sizeof(*_tmp555)),((_tmp555[0]=((_tmp556.tag=1,((_tmp556.f1=c,((_tmp556.f2=ps,_tmp556)))))),_tmp555))));}
# 995
static void*Cyc_Tcpat_compile_pat(struct _RegionHandle*r,struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp222=p->r;void*_tmp223=_tmp222;enum Cyc_Absyn_Sign _tmp229;int _tmp22A;char _tmp22C;struct _dyneither_ptr _tmp22E;struct Cyc_Absyn_Pat*_tmp230;struct Cyc_Absyn_Pat*_tmp232;struct Cyc_Absyn_Pat*_tmp234;struct Cyc_Absyn_Datatypedecl*_tmp236;struct Cyc_Absyn_Datatypefield*_tmp237;struct Cyc_List_List*_tmp238;struct Cyc_List_List*_tmp23A;struct Cyc_Absyn_Aggrdecl*_tmp23C;struct Cyc_List_List*_tmp23D;struct Cyc_Absyn_Enumdecl*_tmp23F;struct Cyc_Absyn_Enumfield*_tmp240;void*_tmp242;struct Cyc_Absyn_Enumfield*_tmp243;_LLDD: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp224=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp224->tag != 0)goto _LLDF;}_LLDE:
 goto _LLE0;_LLDF: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp225=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp225->tag != 2)goto _LLE1;}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp226=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp226->tag != 4)goto _LLE3;}_LLE2:
{struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp559;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp558;s=(void*)((_tmp558=_region_malloc(r,sizeof(*_tmp558)),((_tmp558[0]=((_tmp559.tag=0,_tmp559)),_tmp558))));}goto _LLDC;_LLE3: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp227=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp227->tag != 9)goto _LLE5;}_LLE4:
 s=Cyc_Tcpat_null_pat(r,p);goto _LLDC;_LLE5: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp228=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp228->tag != 10)goto _LLE7;else{_tmp229=_tmp228->f1;_tmp22A=_tmp228->f2;}}_LLE6:
 s=Cyc_Tcpat_int_pat(r,_tmp22A,p);goto _LLDC;_LLE7: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp22B=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp22B->tag != 11)goto _LLE9;else{_tmp22C=_tmp22B->f1;}}_LLE8:
 s=Cyc_Tcpat_char_pat(r,_tmp22C,p);goto _LLDC;_LLE9: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp22D=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp22D->tag != 12)goto _LLEB;else{_tmp22E=_tmp22D->f1;}}_LLEA:
 s=Cyc_Tcpat_float_pat(r,_tmp22E,p);goto _LLDC;_LLEB: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp22F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp22F->tag != 1)goto _LLED;else{_tmp230=_tmp22F->f2;}}_LLEC:
 s=Cyc_Tcpat_compile_pat(r,_tmp230);goto _LLDC;_LLED: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp231=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp231->tag != 3)goto _LLEF;else{_tmp232=_tmp231->f2;}}_LLEE:
 s=Cyc_Tcpat_compile_pat(r,_tmp232);goto _LLDC;_LLEF: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp233=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp233->tag != 6)goto _LLF1;else{_tmp234=_tmp233->f1;}}_LLF0:
# 1008
{void*_tmp24A=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp24B=_tmp24A;union Cyc_Absyn_Constraint*_tmp24D;_LL104: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24B;if(_tmp24C->tag != 5)goto _LL106;else{_tmp24D=((_tmp24C->f1).ptr_atts).nullable;}}_LL105: {
# 1010
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp24E=_tmp24D;union Cyc_Absyn_Constraint*_tmp24F;int _tmp250;_LL109: if((_tmp24E->Forward_constr).tag != 2)goto _LL10B;_tmp24F=(union Cyc_Absyn_Constraint*)(_tmp24E->Forward_constr).val;_LL10A:
# 1015
*_tmp24D=*_tmp24F;
continue;_LL10B: if((_tmp24E->No_constr).tag != 3)goto _LL10D;_LL10C:
# 1018
{struct _union_Constraint_Eq_constr*_tmp55A;(_tmp55A=& _tmp24D->Eq_constr,((_tmp55A->tag=1,_tmp55A->val=0)));}
is_nullable=0;
still_working=0;
goto _LL108;_LL10D: if((_tmp24E->Eq_constr).tag != 1)goto _LL108;_tmp250=(int)(_tmp24E->Eq_constr).val;_LL10E:
# 1023
 is_nullable=_tmp250;
still_working=0;
goto _LL108;_LL108:;}{
# 1028
void*ss=Cyc_Tcpat_compile_pat(r,_tmp234);
if(is_nullable)
s=Cyc_Tcpat_null_ptr_pat(r,ss,p);else{
# 1032
s=Cyc_Tcpat_ptr_pat(r,ss,p);}
goto _LL103;};}_LL106:;_LL107: {
const char*_tmp55D;void*_tmp55C;(_tmp55C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp55D="expecting pointertype for pattern!",_tag_dyneither(_tmp55D,sizeof(char),35))),_tag_dyneither(_tmp55C,sizeof(void*),0)));}_LL103:;}
# 1036
goto _LLDC;_LLF1: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp235=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp235->tag != 8)goto _LLF3;else{_tmp236=_tmp235->f1;_tmp237=_tmp235->f2;_tmp238=_tmp235->f3;}}_LLF2: {
# 1038
int*span;
{void*_tmp254=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp255=_tmp254;_LL110: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp256=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp255;if(_tmp256->tag != 3)goto _LL112;}_LL111:
# 1041
 if(_tmp236->is_extensible)
span=0;else{
# 1044
int*_tmp55E;span=((_tmp55E=_region_malloc(r,sizeof(*_tmp55E)),((_tmp55E[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp236->fields))->v),_tmp55E))));}
goto _LL10F;_LL112: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp255;if(_tmp257->tag != 4)goto _LL114;}_LL113:
 span=& Cyc_Tcpat_one_opt;goto _LL10F;_LL114:;_LL115:
{const char*_tmp561;void*_tmp560;span=((_tmp560=0,((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp561="void datatype pattern has bad type",_tag_dyneither(_tmp561,sizeof(char),35))),_tag_dyneither(_tmp560,sizeof(void*),0))));}goto _LL10F;_LL10F:;}
# 1049
s=Cyc_Tcpat_con_pat(r,*(*_tmp237->name).f2,span,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp238),p);
goto _LLDC;}_LLF3: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp239=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp239->tag != 5)goto _LLF5;else{_tmp23A=_tmp239->f1;}}_LLF4:
# 1053
 s=Cyc_Tcpat_tuple_pat(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Pat*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_compile_pat,r,_tmp23A),p);goto _LLDC;_LLF5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp23B=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp23B->tag != 7)goto _LLF7;else{if(_tmp23B->f1 == 0)goto _LLF7;if((((_tmp23B->f1)->aggr_info).KnownAggr).tag != 2)goto _LLF7;_tmp23C=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp23B->f1)->aggr_info).KnownAggr).val);_tmp23D=_tmp23B->f3;}}_LLF6:
# 1058
 if(_tmp23C->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23C->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1062
const char*_tmp562;int found=Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,((_tmp562="",_tag_dyneither(_tmp562,sizeof(char),1))))== 0;
{struct Cyc_List_List*dlps0=_tmp23D;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple15*_tmp25B=(struct _tuple15*)dlps0->hd;struct Cyc_List_List*_tmp25D;struct Cyc_Absyn_Pat*_tmp25E;struct _tuple15*_tmp25C=_tmp25B;_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;{
struct Cyc_List_List*_tmp25F=_tmp25D;struct _dyneither_ptr*_tmp261;_LL117: if(_tmp25F == 0)goto _LL119;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp260=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)_tmp25F->hd);if(_tmp260->tag != 1)goto _LL119;else{_tmp261=_tmp260->f1;}}if(_tmp25F->tl != 0)goto _LL119;_LL118:
# 1067
 if(Cyc_strptrcmp(_tmp261,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
{struct Cyc_List_List*_tmp563;ps=((_tmp563=_region_malloc(r,sizeof(*_tmp563)),((_tmp563->hd=Cyc_Tcpat_compile_pat(r,_tmp25E),((_tmp563->tl=ps,_tmp563))))));}
found=1;}
# 1071
goto _LL116;_LL119:;_LL11A: {
const char*_tmp566;void*_tmp565;(_tmp565=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp566="bad designator(s)",_tag_dyneither(_tmp566,sizeof(char),18))),_tag_dyneither(_tmp565,sizeof(void*),0)));}_LL116:;};}}
# 1075
if(!found){
const char*_tmp569;void*_tmp568;(_tmp568=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp569="bad designator",_tag_dyneither(_tmp569,sizeof(char),15))),_tag_dyneither(_tmp568,sizeof(void*),0)));}}}
# 1078
s=Cyc_Tcpat_tuple_pat(r,ps,p);}else{
# 1081
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23C->impl))->tagged){
const char*_tmp56C;void*_tmp56B;(_tmp56B=0,Cyc_Tcutil_terr(p->loc,((_tmp56C="patterns on untagged unions not yet supported.",_tag_dyneither(_tmp56C,sizeof(char),47))),_tag_dyneither(_tmp56B,sizeof(void*),0)));}{
int*_tmp56D;int*span=(_tmp56D=_region_malloc(r,sizeof(*_tmp56D)),((_tmp56D[0]=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23C->impl))->fields),_tmp56D)));
struct Cyc_List_List*_tmp26A=_tmp23D;struct _dyneither_ptr*_tmp26C;struct Cyc_Absyn_Pat*_tmp26D;_LL11C: if(_tmp26A == 0)goto _LL11E;if(((struct _tuple15*)_tmp26A->hd)->f1 == 0)goto _LL11E;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp26B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp26A->hd)->f1)->hd);if(_tmp26B->tag != 1)goto _LL11E;else{_tmp26C=_tmp26B->f1;}}if((((struct _tuple15*)_tmp26A->hd)->f1)->tl != 0)goto _LL11E;_tmp26D=((struct _tuple15*)_tmp26A->hd)->f2;if(_tmp26A->tl != 0)goto _LL11E;_LL11D:
# 1086
{struct Cyc_List_List*_tmp56E;s=Cyc_Tcpat_con_pat(r,*_tmp26C,span,((_tmp56E=_region_malloc(r,sizeof(*_tmp56E)),((_tmp56E->hd=Cyc_Tcpat_compile_pat(r,_tmp26D),((_tmp56E->tl=0,_tmp56E)))))),p);}
goto _LL11B;_LL11E:;_LL11F: {
const char*_tmp571;void*_tmp570;(_tmp570=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp571="bad union pattern",_tag_dyneither(_tmp571,sizeof(char),18))),_tag_dyneither(_tmp570,sizeof(void*),0)));}_LL11B:;};}
# 1091
goto _LLDC;_LLF7: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp23E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp23E->tag != 13)goto _LLF9;else{_tmp23F=_tmp23E->f1;_tmp240=_tmp23E->f2;}}_LLF8:
# 1096
{void*_tmp272=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp273=_tmp272;_LL121: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp274=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp273;if(_tmp274->tag != 6)goto _LL123;}_LL122:
# 1100
 s=Cyc_Tcpat_con_pat(r,*(*_tmp240->name).f2,0,0,p);
goto _LL120;_LL123:;_LL124: {
# 1103
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23F->fields))->v);
{int*_tmp572;s=Cyc_Tcpat_con_pat(r,*(*_tmp240->name).f2,((_tmp572=_region_malloc(r,sizeof(*_tmp572)),((_tmp572[0]=span,_tmp572)))),0,p);}
goto _LL120;}_LL120:;}
# 1107
goto _LLDC;_LLF9: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp241=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp241->tag != 14)goto _LLFB;else{_tmp242=(void*)_tmp241->f1;_tmp243=_tmp241->f2;}}_LLFA: {
# 1112
struct Cyc_List_List*fields;
{void*_tmp276=Cyc_Tcutil_compress(_tmp242);void*_tmp277=_tmp276;struct Cyc_List_List*_tmp279;_LL126: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp278=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp277;if(_tmp278->tag != 14)goto _LL128;else{_tmp279=_tmp278->f1;}}_LL127:
 fields=_tmp279;goto _LL125;_LL128:;_LL129: {
const char*_tmp575;void*_tmp574;(_tmp574=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp575="bad type in AnonEnum_p",_tag_dyneither(_tmp575,sizeof(char),23))),_tag_dyneither(_tmp574,sizeof(void*),0)));}_LL125:;}
# 1119
{void*_tmp27C=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp27D=_tmp27C;_LL12B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp27E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp27D;if(_tmp27E->tag != 6)goto _LL12D;}_LL12C:
# 1123
 s=Cyc_Tcpat_con_pat(r,*(*_tmp243->name).f2,0,0,p);
goto _LL12A;_LL12D:;_LL12E: {
# 1126
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
{int*_tmp576;s=Cyc_Tcpat_con_pat(r,*(*_tmp243->name).f2,((_tmp576=_region_malloc(r,sizeof(*_tmp576)),((_tmp576[0]=span,_tmp576)))),0,p);}
goto _LL12A;}_LL12A:;}
# 1130
goto _LLDC;}_LLFB: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp244=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp244->tag != 15)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp245=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp245->tag != 16)goto _LLFF;}_LLFE:
 goto _LL100;_LLFF: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp246=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp246->tag != 7)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp247=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp223;if(_tmp247->tag != 17)goto _LLDC;}_LL102: {
struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp579;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp578;s=(void*)((_tmp578=_region_malloc(r,sizeof(*_tmp578)),((_tmp578[0]=((_tmp579.tag=0,_tmp579)),_tmp578))));}_LLDC:;}
# 1136
return s;}
# 1144
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1155
typedef void*Cyc_Tcpat_term_desc_t;
# 1159
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1166
typedef struct Cyc_List_List*Cyc_Tcpat_access_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;void*f3;void*f4;};
# 1175
typedef void*Cyc_Tcpat_decision_t;struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1180
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1183
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1185
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1188
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
const char*_tmp57C;void*_tmp57B;(_tmp57B=0,Cyc_fprintf(Cyc_stderr,((_tmp57C=" ",_tag_dyneither(_tmp57C,sizeof(char),2))),_tag_dyneither(_tmp57B,sizeof(void*),0)));}}
# 1194
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp284=c->name;
union Cyc_Tcpat_Name_value _tmp285=_tmp284;struct _dyneither_ptr _tmp286;int _tmp287;_LL130: if((_tmp285.Name_v).tag != 1)goto _LL132;_tmp286=(struct _dyneither_ptr)(_tmp285.Name_v).val;_LL131:
{const char*_tmp580;void*_tmp57F[1];struct Cyc_String_pa_PrintArg_struct _tmp57E;(_tmp57E.tag=0,((_tmp57E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp286),((_tmp57F[0]=& _tmp57E,Cyc_fprintf(Cyc_stderr,((_tmp580="%s",_tag_dyneither(_tmp580,sizeof(char),3))),_tag_dyneither(_tmp57F,sizeof(void*),1)))))));}goto _LL12F;_LL132: if((_tmp285.Int_v).tag != 2)goto _LL12F;_tmp287=(int)(_tmp285.Int_v).val;_LL133:
{const char*_tmp584;void*_tmp583[1];struct Cyc_Int_pa_PrintArg_struct _tmp582;(_tmp582.tag=1,((_tmp582.f1=(unsigned long)_tmp287,((_tmp583[0]=& _tmp582,Cyc_fprintf(Cyc_stderr,((_tmp584="%d",_tag_dyneither(_tmp584,sizeof(char),3))),_tag_dyneither(_tmp583,sizeof(void*),1)))))));}goto _LL12F;_LL12F:;}
# 1202
static void Cyc_Tcpat_print_dec_tree(void*d,void(*print_rhs)(void*),int tab){
void*_tmp28E=d;void*_tmp290;struct Cyc_List_List*_tmp293;struct Cyc_Tcpat_Con_s*_tmp294;void*_tmp295;void*_tmp296;_LL135: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp28F=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp28E;if(_tmp28F->tag != 1)goto _LL137;else{_tmp290=(void*)_tmp28F->f1;}}_LL136:
# 1205
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp587;void*_tmp586;(_tmp586=0,Cyc_fprintf(Cyc_stderr,((_tmp587="Success(",_tag_dyneither(_tmp587,sizeof(char),9))),_tag_dyneither(_tmp586,sizeof(void*),0)));}print_rhs(_tmp290);{const char*_tmp58A;void*_tmp589;(_tmp589=0,Cyc_fprintf(Cyc_stderr,((_tmp58A=")\n",_tag_dyneither(_tmp58A,sizeof(char),3))),_tag_dyneither(_tmp589,sizeof(void*),0)));}
goto _LL134;_LL137: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp291=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp28E;if(_tmp291->tag != 0)goto _LL139;}_LL138:
{const char*_tmp58D;void*_tmp58C;(_tmp58C=0,Cyc_fprintf(Cyc_stderr,((_tmp58D="Failure\n",_tag_dyneither(_tmp58D,sizeof(char),9))),_tag_dyneither(_tmp58C,sizeof(void*),0)));}goto _LL134;_LL139: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp292=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp28E;if(_tmp292->tag != 2)goto _LL134;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;_tmp295=(void*)_tmp292->f3;_tmp296=(void*)_tmp292->f4;}}_LL13A:
# 1210
 Cyc_Tcpat_print_tab(tab);
{const char*_tmp590;void*_tmp58F;(_tmp58F=0,Cyc_fprintf(Cyc_stderr,((_tmp590="Access[",_tag_dyneither(_tmp590,sizeof(char),8))),_tag_dyneither(_tmp58F,sizeof(void*),0)));}
for(0;_tmp293 != 0;_tmp293=_tmp293->tl){
{const char*_tmp594;void*_tmp593[1];struct Cyc_Int_pa_PrintArg_struct _tmp592;(_tmp592.tag=1,((_tmp592.f1=(unsigned long)((int)_tmp293->hd),((_tmp593[0]=& _tmp592,Cyc_fprintf(Cyc_stderr,((_tmp594="%d",_tag_dyneither(_tmp594,sizeof(char),3))),_tag_dyneither(_tmp593,sizeof(void*),1)))))));}
if(_tmp293->tl != 0){const char*_tmp597;void*_tmp596;(_tmp596=0,Cyc_fprintf(Cyc_stderr,((_tmp597=",",_tag_dyneither(_tmp597,sizeof(char),2))),_tag_dyneither(_tmp596,sizeof(void*),0)));}}
# 1216
{const char*_tmp59A;void*_tmp599;(_tmp599=0,Cyc_fprintf(Cyc_stderr,((_tmp59A="],",_tag_dyneither(_tmp59A,sizeof(char),3))),_tag_dyneither(_tmp599,sizeof(void*),0)));}
Cyc_Tcpat_print_con(_tmp294);
{const char*_tmp59D;void*_tmp59C;(_tmp59C=0,Cyc_fprintf(Cyc_stderr,((_tmp59D=",\n",_tag_dyneither(_tmp59D,sizeof(char),3))),_tag_dyneither(_tmp59C,sizeof(void*),0)));}
Cyc_Tcpat_print_dec_tree(_tmp295,print_rhs,tab + 7);
Cyc_Tcpat_print_dec_tree(_tmp296,print_rhs,tab + 7);_LL134:;}struct _tuple21{int f1;unsigned int f2;};
# 1224
static void Cyc_Tcpat_print_swrhs(struct _tuple21*x){
return;}
# 1232
static void*Cyc_Tcpat_add_neg(struct _RegionHandle*r,void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2A8=td;struct Cyc_Set_Set*_tmp2AA;_LL13C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2A9=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2A8;if(_tmp2A9->tag != 1)goto _LL13E;else{_tmp2AA=_tmp2A9->f1;}}_LL13D: {
# 1242
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5A0;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp59F;return(void*)((_tmp59F=_region_malloc(r,sizeof(*_tmp59F)),((_tmp59F[0]=((_tmp5A0.tag=1,((_tmp5A0.f1=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_rinsert)(r,_tmp2AA,c),_tmp5A0)))),_tmp59F))));}_LL13E: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2AB=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2A8;if(_tmp2AB->tag != 0)goto _LL13B;}_LL13F: {
const char*_tmp5A3;void*_tmp5A2;(_tmp5A2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5A3="add_neg called when td is Positive",_tag_dyneither(_tmp5A3,sizeof(char),35))),_tag_dyneither(_tmp5A2,sizeof(void*),0)));}_LL13B:;}
# 1249
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2B0=td;struct Cyc_Tcpat_Con_s*_tmp2B2;struct Cyc_Set_Set*_tmp2B4;_LL141: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2B1=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B0;if(_tmp2B1->tag != 0)goto _LL143;else{_tmp2B2=_tmp2B1->f1;}}_LL142:
# 1253
 if(Cyc_Tcpat_compare_con(c,_tmp2B2)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}_LL143: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2B3=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B0;if(_tmp2B3->tag != 1)goto _LL140;else{_tmp2B4=_tmp2B3->f1;}}_LL144:
# 1257
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2B4,c))return Cyc_Tcpat_No;else{
# 1260
if(c->span != 0  && *((int*)_check_null(c->span))== ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2B4)+ 1)
return Cyc_Tcpat_Yes;else{
# 1263
return Cyc_Tcpat_Maybe;}}_LL140:;}struct _tuple22{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1271
static struct Cyc_List_List*Cyc_Tcpat_augment(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc){
# 1273
struct Cyc_List_List*_tmp2B5=ctxt;struct Cyc_Tcpat_Con_s*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_List_List*_tmp2B8;_LL146: if(_tmp2B5 != 0)goto _LL148;_LL147:
 return 0;_LL148: if(_tmp2B5 == 0)goto _LL145;_tmp2B6=((struct _tuple22*)_tmp2B5->hd)->f1;_tmp2B7=((struct _tuple22*)_tmp2B5->hd)->f2;_tmp2B8=_tmp2B5->tl;_LL149: {
# 1276
struct _tuple22*_tmp5A9;struct Cyc_List_List*_tmp5A8;struct Cyc_List_List*_tmp5A7;return(_tmp5A7=_region_malloc(r,sizeof(*_tmp5A7)),((_tmp5A7->hd=(
(_tmp5A9=_region_malloc(r,sizeof(*_tmp5A9)),((_tmp5A9->f1=_tmp2B6,((_tmp5A9->f2=((_tmp5A8=_region_malloc(r,sizeof(*_tmp5A8)),((_tmp5A8->hd=dsc,((_tmp5A8->tl=_tmp2B7,_tmp5A8)))))),_tmp5A9)))))),((_tmp5A7->tl=_tmp2B8,_tmp5A7)))));}_LL145:;}
# 1284
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct _RegionHandle*r,struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2BC=ctxt;struct Cyc_Tcpat_Con_s*_tmp2BD;struct Cyc_List_List*_tmp2BE;struct Cyc_List_List*_tmp2BF;_LL14B: if(_tmp2BC != 0)goto _LL14D;_LL14C: {
const char*_tmp5AC;void*_tmp5AB;(_tmp5AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5AC="norm_context: empty context",_tag_dyneither(_tmp5AC,sizeof(char),28))),_tag_dyneither(_tmp5AB,sizeof(void*),0)));}_LL14D: if(_tmp2BC == 0)goto _LL14A;_tmp2BD=((struct _tuple22*)_tmp2BC->hd)->f1;_tmp2BE=((struct _tuple22*)_tmp2BC->hd)->f2;_tmp2BF=_tmp2BC->tl;_LL14E: {
# 1288
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp5AF;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp5AE;return Cyc_Tcpat_augment(r,_tmp2BF,(void*)((_tmp5AE=_region_malloc(r,sizeof(*_tmp5AE)),((_tmp5AE[0]=((_tmp5AF.tag=0,((_tmp5AF.f1=_tmp2BD,((_tmp5AF.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2BE),_tmp5AF)))))),_tmp5AE)))));}_LL14A:;}
# 1297
static void*Cyc_Tcpat_build_desc(struct _RegionHandle*r,struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1299
struct _tuple1 _tmp5B0;struct _tuple1 _tmp2C4=(_tmp5B0.f1=ctxt,((_tmp5B0.f2=work,_tmp5B0)));struct _tuple1 _tmp2C5=_tmp2C4;struct Cyc_Tcpat_Con_s*_tmp2C6;struct Cyc_List_List*_tmp2C7;struct Cyc_List_List*_tmp2C8;struct Cyc_List_List*_tmp2C9;struct Cyc_List_List*_tmp2CA;_LL150: if(_tmp2C5.f1 != 0)goto _LL152;if(_tmp2C5.f2 != 0)goto _LL152;_LL151:
 return dsc;_LL152: if(_tmp2C5.f1 != 0)goto _LL154;_LL153:
 goto _LL155;_LL154: if(_tmp2C5.f2 != 0)goto _LL156;_LL155: {
const char*_tmp5B3;void*_tmp5B2;(_tmp5B2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp5B3="build_desc: ctxt and work don't match",_tag_dyneither(_tmp5B3,sizeof(char),38))),_tag_dyneither(_tmp5B2,sizeof(void*),0)));}_LL156: if(_tmp2C5.f1 == 0)goto _LL14F;_tmp2C6=((struct _tuple22*)(_tmp2C5.f1)->hd)->f1;_tmp2C7=((struct _tuple22*)(_tmp2C5.f1)->hd)->f2;_tmp2C8=(_tmp2C5.f1)->tl;if(_tmp2C5.f2 == 0)goto _LL14F;_tmp2C9=((struct _tuple20*)(_tmp2C5.f2)->hd)->f3;_tmp2CA=(_tmp2C5.f2)->tl;_LL157: {
# 1304
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp5B9;struct Cyc_List_List*_tmp5B8;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp5B7;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2CD=(_tmp5B7=_region_malloc(r,sizeof(*_tmp5B7)),((_tmp5B7[0]=((_tmp5B9.tag=0,((_tmp5B9.f1=_tmp2C6,((_tmp5B9.f2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,_tmp2C7),(
(_tmp5B8=_region_malloc(r,sizeof(*_tmp5B8)),((_tmp5B8->hd=dsc,((_tmp5B8->tl=_tmp2C9,_tmp5B8))))))),_tmp5B9)))))),_tmp5B7)));
return Cyc_Tcpat_build_desc(r,_tmp2C8,(void*)_tmp2CD,_tmp2CA);}_LL14F:;}
# 1310
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules);
# 1317
static void*Cyc_Tcpat_or_match(struct _RegionHandle*r,void*dsc,struct Cyc_List_List*allmrules){
# 1319
struct Cyc_List_List*_tmp2D2=allmrules;void*_tmp2D3;void*_tmp2D4;struct Cyc_List_List*_tmp2D5;_LL159: if(_tmp2D2 != 0)goto _LL15B;_LL15A: {
struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp5BC;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp5BB;return(void*)((_tmp5BB=_cycalloc(sizeof(*_tmp5BB)),((_tmp5BB[0]=((_tmp5BC.tag=0,((_tmp5BC.f1=dsc,_tmp5BC)))),_tmp5BB))));}_LL15B: if(_tmp2D2 == 0)goto _LL158;_tmp2D3=((struct _tuple0*)_tmp2D2->hd)->f1;_tmp2D4=((struct _tuple0*)_tmp2D2->hd)->f2;_tmp2D5=_tmp2D2->tl;_LL15C:
# 1322
 return Cyc_Tcpat_match(r,_tmp2D3,0,dsc,0,0,_tmp2D4,_tmp2D5);_LL158:;}
# 1327
static void*Cyc_Tcpat_match_compile(struct _RegionHandle*r,struct Cyc_List_List*allmrules){
# 1329
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5BF;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5BE;return Cyc_Tcpat_or_match(r,(void*)((_tmp5BE=_region_malloc(r,sizeof(*_tmp5BE)),((_tmp5BE[0]=((_tmp5BF.tag=1,((_tmp5BF.f1=Cyc_Tcpat_empty_con_set(r),_tmp5BF)))),_tmp5BE)))),allmrules);}
# 1335
static void*Cyc_Tcpat_and_match(struct _RegionHandle*r,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1339
struct Cyc_List_List*_tmp2DA=work;struct Cyc_List_List*_tmp2DB;struct Cyc_List_List*_tmp2DC;struct Cyc_List_List*_tmp2DD;struct Cyc_List_List*_tmp2DE;struct Cyc_List_List*_tmp2DF;_LL15E: if(_tmp2DA != 0)goto _LL160;_LL15F: {
struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp5C2;struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp5C1;return(void*)((_tmp5C1=_region_malloc(r,sizeof(*_tmp5C1)),((_tmp5C1[0]=((_tmp5C2.tag=1,((_tmp5C2.f1=right_hand_side,_tmp5C2)))),_tmp5C1))));}_LL160: if(_tmp2DA == 0)goto _LL162;if(((struct _tuple20*)_tmp2DA->hd)->f1 != 0)goto _LL162;if(((struct _tuple20*)_tmp2DA->hd)->f2 != 0)goto _LL162;if(((struct _tuple20*)_tmp2DA->hd)->f3 != 0)goto _LL162;_tmp2DB=_tmp2DA->tl;_LL161:
# 1342
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_norm_context(r,ctx),_tmp2DB,right_hand_side,rules);_LL162: if(_tmp2DA == 0)goto _LL15D;_tmp2DC=((struct _tuple20*)_tmp2DA->hd)->f1;_tmp2DD=((struct _tuple20*)_tmp2DA->hd)->f2;_tmp2DE=((struct _tuple20*)_tmp2DA->hd)->f3;_tmp2DF=_tmp2DA->tl;_LL163:
# 1344
 if((_tmp2DC == 0  || _tmp2DD == 0) || _tmp2DE == 0){
const char*_tmp5C5;void*_tmp5C4;(_tmp5C4=0,Cyc_Tcutil_impos(((_tmp5C5="tcpat:and_match: malformed work frame",_tag_dyneither(_tmp5C5,sizeof(char),38))),_tag_dyneither(_tmp5C4,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp2E4=_tmp2DC;void*_tmp2E6;struct Cyc_List_List*_tmp2E7;struct Cyc_List_List*_tmp2E5=_tmp2E4;_tmp2E6=(void*)_tmp2E5->hd;_tmp2E7=_tmp2E5->tl;{
struct Cyc_List_List*_tmp2E8=_tmp2DD;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2EB;struct Cyc_List_List*_tmp2E9=_tmp2E8;_tmp2EA=(struct Cyc_List_List*)_tmp2E9->hd;_tmp2EB=_tmp2E9->tl;{
struct Cyc_List_List*_tmp2EC=_tmp2DE;void*_tmp2EE;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2ED=_tmp2EC;_tmp2EE=(void*)_tmp2ED->hd;_tmp2EF=_tmp2ED->tl;{
struct _tuple20*_tmp5C6;struct _tuple20*_tmp2F0=(_tmp5C6=_region_malloc(r,sizeof(*_tmp5C6)),((_tmp5C6->f1=_tmp2E7,((_tmp5C6->f2=_tmp2EB,((_tmp5C6->f3=_tmp2EF,_tmp5C6)))))));
struct Cyc_List_List*_tmp5C7;return Cyc_Tcpat_match(r,_tmp2E6,_tmp2EA,_tmp2EE,ctx,((_tmp5C7=_region_malloc(r,sizeof(*_tmp5C7)),((_tmp5C7->hd=_tmp2F0,((_tmp5C7->tl=_tmp2DF,_tmp5C7)))))),right_hand_side,rules);};};};};_LL15D:;}
# 1356
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,void*dsc){
# 1358
void*_tmp2F3=dsc;struct Cyc_Set_Set*_tmp2F5;struct Cyc_List_List*_tmp2F7;_LL165: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F4=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F3;if(_tmp2F4->tag != 1)goto _LL167;else{_tmp2F5=_tmp2F4->f1;}}_LL166: {
# 1363
struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5CA;struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp5C9;void*any=(void*)((_tmp5C9=_region_malloc(r,sizeof(*_tmp5C9)),((_tmp5C9[0]=((_tmp5CA.tag=1,((_tmp5CA.f1=Cyc_Tcpat_empty_con_set(r),_tmp5CA)))),_tmp5C9))));
struct Cyc_List_List*_tmp2F8=0;
{int i=0;for(0;i < pcon->arity;++ i){
struct Cyc_List_List*_tmp5CB;_tmp2F8=((_tmp5CB=_region_malloc(r,sizeof(*_tmp5CB)),((_tmp5CB->hd=any,((_tmp5CB->tl=_tmp2F8,_tmp5CB))))));}}
return _tmp2F8;}_LL167: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2F6=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F3;if(_tmp2F6->tag != 0)goto _LL164;else{_tmp2F7=_tmp2F6->f2;}}_LL168:
 return _tmp2F7;_LL164:;}struct _tuple23{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 1373
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _RegionHandle*_tmp2FD;struct Cyc_List_List*_tmp2FE;struct _tuple23*_tmp2FC=env;_tmp2FD=_tmp2FC->f1;_tmp2FE=_tmp2FC->f2;{
struct Cyc_List_List*_tmp5CC;return(_tmp5CC=_region_malloc(_tmp2FD,sizeof(*_tmp5CC)),((_tmp5CC->hd=(void*)(i + 1),((_tmp5CC->tl=_tmp2FE,_tmp5CC)))));};}
# 1377
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct _RegionHandle*r,struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
# 1379
struct _tuple23 _tmp5CD;struct _tuple23 _tmp300=(_tmp5CD.f1=r,((_tmp5CD.f2=obj,_tmp5CD)));
return((struct Cyc_List_List*(*)(struct _RegionHandle*r,int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_rtabulate_c)(r,pcon->arity,Cyc_Tcpat_getoarg,& _tmp300);}
# 1386
static void*Cyc_Tcpat_match(struct _RegionHandle*r,void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,void*right_hand_side,struct Cyc_List_List*rules){
# 1390
void*_tmp302=p;struct Cyc_Tcpat_Con_s*_tmp305;struct Cyc_List_List*_tmp306;_LL16A: {struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp303=(struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp302;if(_tmp303->tag != 0)goto _LL16C;}_LL16B:
# 1392
 return Cyc_Tcpat_and_match(r,Cyc_Tcpat_augment(r,ctx,dsc),work,right_hand_side,rules);_LL16C: {struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp304=(struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp302;if(_tmp304->tag != 1)goto _LL169;else{_tmp305=_tmp304->f1;_tmp306=_tmp304->f2;}}_LL16D: {
# 1394
enum Cyc_Tcpat_Answer _tmp307=Cyc_Tcpat_static_match(_tmp305,dsc);switch(_tmp307){case Cyc_Tcpat_Yes: _LL16E: {
# 1396
struct _tuple22*_tmp5D0;struct Cyc_List_List*_tmp5CF;struct Cyc_List_List*ctx2=(_tmp5CF=_region_malloc(r,sizeof(*_tmp5CF)),((_tmp5CF->hd=((_tmp5D0=_region_malloc(r,sizeof(*_tmp5D0)),((_tmp5D0->f1=_tmp305,((_tmp5D0->f2=0,_tmp5D0)))))),((_tmp5CF->tl=ctx,_tmp5CF)))));
struct _tuple20*_tmp5D1;struct _tuple20*work_frame=(_tmp5D1=_region_malloc(r,sizeof(*_tmp5D1)),((_tmp5D1->f1=_tmp306,((_tmp5D1->f2=Cyc_Tcpat_getoargs(r,_tmp305,obj),((_tmp5D1->f3=
Cyc_Tcpat_getdargs(r,_tmp305,dsc),_tmp5D1)))))));
struct Cyc_List_List*_tmp5D2;struct Cyc_List_List*work2=(_tmp5D2=_region_malloc(r,sizeof(*_tmp5D2)),((_tmp5D2->hd=work_frame,((_tmp5D2->tl=work,_tmp5D2)))));
return Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL16F:
# 1402
 return Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,dsc,work),rules);case Cyc_Tcpat_Maybe: _LL170: {
# 1404
struct _tuple22*_tmp5D5;struct Cyc_List_List*_tmp5D4;struct Cyc_List_List*ctx2=(_tmp5D4=_region_malloc(r,sizeof(*_tmp5D4)),((_tmp5D4->hd=((_tmp5D5=_region_malloc(r,sizeof(*_tmp5D5)),((_tmp5D5->f1=_tmp305,((_tmp5D5->f2=0,_tmp5D5)))))),((_tmp5D4->tl=ctx,_tmp5D4)))));
struct _tuple20*_tmp5D6;struct _tuple20*work_frame=(_tmp5D6=_region_malloc(r,sizeof(*_tmp5D6)),((_tmp5D6->f1=_tmp306,((_tmp5D6->f2=Cyc_Tcpat_getoargs(r,_tmp305,obj),((_tmp5D6->f3=
Cyc_Tcpat_getdargs(r,_tmp305,dsc),_tmp5D6)))))));
struct Cyc_List_List*_tmp5D7;struct Cyc_List_List*work2=(_tmp5D7=_region_malloc(r,sizeof(*_tmp5D7)),((_tmp5D7->hd=work_frame,((_tmp5D7->tl=work,_tmp5D7)))));
void*_tmp30C=Cyc_Tcpat_and_match(r,ctx2,work2,right_hand_side,rules);
void*_tmp30D=Cyc_Tcpat_or_match(r,Cyc_Tcpat_build_desc(r,ctx,Cyc_Tcpat_add_neg(r,dsc,_tmp305),work),rules);
# 1411
struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct _tmp5DA;struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp5D9;return(void*)((_tmp5D9=_region_malloc(r,sizeof(*_tmp5D9)),((_tmp5D9[0]=((_tmp5DA.tag=2,((_tmp5DA.f1=obj,((_tmp5DA.f2=_tmp305,((_tmp5DA.f3=_tmp30C,((_tmp5DA.f4=_tmp30D,_tmp5DA)))))))))),_tmp5D9))));}}}_LL169:;}
# 1421
static void Cyc_Tcpat_check_exhaust_overlap(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,void*,void*),void*env1,void(*rhs_appears)(void*,void*),void*env2,int*exhaust_done){
# 1427
void*_tmp314=d;void*_tmp316;void*_tmp318;void*_tmp31A;void*_tmp31B;_LL173: {struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp315=(struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp314;if(_tmp315->tag != 0)goto _LL175;else{_tmp316=(void*)_tmp315->f1;}}_LL174:
# 1429
 if(!(*exhaust_done)){not_exhaust(r,env1,_tmp316);*exhaust_done=1;}
goto _LL172;_LL175: {struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp317=(struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp314;if(_tmp317->tag != 1)goto _LL177;else{_tmp318=(void*)_tmp317->f1;}}_LL176:
 rhs_appears(env2,_tmp318);goto _LL172;_LL177: {struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*_tmp319=(struct Cyc_Tcpat_IfEq_Tcpat_Decision_struct*)_tmp314;if(_tmp319->tag != 2)goto _LL172;else{_tmp31A=(void*)_tmp319->f3;_tmp31B=(void*)_tmp319->f4;}}_LL178:
# 1433
 Cyc_Tcpat_check_exhaust_overlap(r,_tmp31A,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1435
Cyc_Tcpat_check_exhaust_overlap(r,_tmp31B,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1437
goto _LL172;_LL172:;}struct _tuple24{void*f1;struct _tuple21*f2;};struct _tuple25{struct _RegionHandle*f1;int*f2;};
# 1445
static struct _tuple24*Cyc_Tcpat_get_match(struct _tuple25*env,struct Cyc_Absyn_Switch_clause*swc){
# 1448
struct _RegionHandle*_tmp31D;int*_tmp31E;struct _tuple25*_tmp31C=env;_tmp31D=_tmp31C->f1;_tmp31E=_tmp31C->f2;{
void*sp0=Cyc_Tcpat_compile_pat(_tmp31D,swc->pattern);
struct _tuple21*_tmp5DB;struct _tuple21*rhs=(_tmp5DB=_region_malloc(_tmp31D,sizeof(*_tmp5DB)),((_tmp5DB->f1=0,((_tmp5DB->f2=(swc->pattern)->loc,_tmp5DB)))));
void*sp;
if(swc->where_clause != 0){
{struct Cyc_List_List*_tmp5DE;struct Cyc_List_List*_tmp5DD;sp=Cyc_Tcpat_tuple_pat(_tmp31D,((_tmp5DD=_region_malloc(_tmp31D,sizeof(*_tmp5DD)),((_tmp5DD->hd=sp0,((_tmp5DD->tl=(
(_tmp5DE=_region_malloc(_tmp31D,sizeof(*_tmp5DE)),((_tmp5DE->hd=Cyc_Tcpat_int_pat(_tmp31D,*_tmp31E,0),((_tmp5DE->tl=0,_tmp5DE)))))),_tmp5DD)))))),0);}
*_tmp31E=*_tmp31E + 1;}else{
# 1457
struct Cyc_List_List*_tmp5E8;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5E7;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp5E6;struct Cyc_List_List*_tmp5E5;sp=Cyc_Tcpat_tuple_pat(_tmp31D,((_tmp5E5=_region_malloc(_tmp31D,sizeof(*_tmp5E5)),((_tmp5E5->hd=sp0,((_tmp5E5->tl=((_tmp5E8=_region_malloc(_tmp31D,sizeof(*_tmp5E8)),((_tmp5E8->hd=(void*)((_tmp5E6=_region_malloc(_tmp31D,sizeof(*_tmp5E6)),((_tmp5E6[0]=((_tmp5E7.tag=0,_tmp5E7)),_tmp5E6)))),((_tmp5E8->tl=0,_tmp5E8)))))),_tmp5E5)))))),0);}{
struct _tuple24*_tmp5E9;return(_tmp5E9=_region_malloc(_tmp31D,sizeof(*_tmp5E9)),((_tmp5E9->f1=sp,((_tmp5E9->f2=rhs,_tmp5E9)))));};};}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1463
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1465
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(struct _RegionHandle*r,void*d){
void*_tmp328=d;struct Cyc_Tcpat_Con_s*_tmp32A;struct Cyc_List_List*_tmp32B;struct Cyc_Set_Set*_tmp32D;_LL17A: {struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp329=(struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp328;if(_tmp329->tag != 0)goto _LL17C;else{_tmp32A=_tmp329->f1;_tmp32B=_tmp329->f2;}}_LL17B: {
# 1469
union Cyc_Tcpat_Name_value _tmp32E=_tmp32A->name;
struct Cyc_Absyn_Pat*_tmp32F=_tmp32A->orig_pat;
if(_tmp32F == 0)
return Cyc_Tcpat_desc2string(r,(void*)((struct Cyc_List_List*)_check_null(_tmp32B))->hd);{
# 1474
void*_tmp330=_tmp32F->r;void*_tmp331=_tmp330;struct Cyc_Absyn_Vardecl*_tmp334;struct Cyc_Absyn_Vardecl*_tmp336;struct Cyc_Absyn_Tvar*_tmp338;struct Cyc_Absyn_Vardecl*_tmp339;struct Cyc_Absyn_Aggrdecl*_tmp33D;struct Cyc_List_List*_tmp33E;struct Cyc_Absyn_Datatypefield*_tmp340;int _tmp343;char _tmp345;struct _dyneither_ptr _tmp347;int _tmp348;struct Cyc_Absyn_Enumfield*_tmp34A;struct Cyc_Absyn_Enumfield*_tmp34C;struct Cyc_Absyn_Exp*_tmp34E;_LL17F: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp332=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp332->tag != 0)goto _LL181;}_LL180: {
const char*_tmp5EA;return(_tmp5EA="_",_tag_dyneither(_tmp5EA,sizeof(char),2));}_LL181: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp333=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp333->tag != 1)goto _LL183;else{_tmp334=_tmp333->f1;}}_LL182:
 return Cyc_Absynpp_qvar2string(_tmp334->name);_LL183: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp335=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp335->tag != 3)goto _LL185;else{_tmp336=_tmp335->f1;}}_LL184: {
const char*_tmp5EE;void*_tmp5ED[1];struct Cyc_String_pa_PrintArg_struct _tmp5EC;return(struct _dyneither_ptr)((_tmp5EC.tag=0,((_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp336->name)),((_tmp5ED[0]=& _tmp5EC,Cyc_aprintf(((_tmp5EE="*%s",_tag_dyneither(_tmp5EE,sizeof(char),4))),_tag_dyneither(_tmp5ED,sizeof(void*),1))))))));}_LL185: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp337=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp337->tag != 4)goto _LL187;else{_tmp338=_tmp337->f1;_tmp339=_tmp337->f2;}}_LL186: {
const char*_tmp5F3;void*_tmp5F2[2];struct Cyc_String_pa_PrintArg_struct _tmp5F1;struct Cyc_String_pa_PrintArg_struct _tmp5F0;return(struct _dyneither_ptr)((_tmp5F0.tag=0,((_tmp5F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp338->name),((_tmp5F1.tag=0,((_tmp5F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp339->name)),((_tmp5F2[0]=& _tmp5F1,((_tmp5F2[1]=& _tmp5F0,Cyc_aprintf(((_tmp5F3="%s<`%s>",_tag_dyneither(_tmp5F3,sizeof(char),8))),_tag_dyneither(_tmp5F2,sizeof(void*),2))))))))))))));}_LL187: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp33A=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp33A->tag != 5)goto _LL189;}_LL188: {
# 1480
const char*_tmp5F7;void*_tmp5F6[1];struct Cyc_String_pa_PrintArg_struct _tmp5F5;return(struct _dyneither_ptr)((_tmp5F5.tag=0,((_tmp5F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp32B)),((_tmp5F6[0]=& _tmp5F5,Cyc_aprintf(((_tmp5F7="$(%s)",_tag_dyneither(_tmp5F7,sizeof(char),6))),_tag_dyneither(_tmp5F6,sizeof(void*),1))))))));}_LL189: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp33B=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp33B->tag != 6)goto _LL18B;}_LL18A: {
# 1482
const char*_tmp5FB;void*_tmp5FA[1];struct Cyc_String_pa_PrintArg_struct _tmp5F9;return(struct _dyneither_ptr)((_tmp5F9.tag=0,((_tmp5F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp32B)),((_tmp5FA[0]=& _tmp5F9,Cyc_aprintf(((_tmp5FB="&%s",_tag_dyneither(_tmp5FB,sizeof(char),4))),_tag_dyneither(_tmp5FA,sizeof(void*),1))))))));}_LL18B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp33C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp33C->tag != 7)goto _LL18D;else{if(_tmp33C->f1 == 0)goto _LL18D;if((((_tmp33C->f1)->aggr_info).KnownAggr).tag != 2)goto _LL18D;_tmp33D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp33C->f1)->aggr_info).KnownAggr).val);_tmp33E=_tmp33C->f3;}}_LL18C:
# 1484
 if(_tmp33D->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp35D=_tmp33E;struct _dyneither_ptr*_tmp35F;_LL1A0: if(_tmp35D == 0)goto _LL1A2;if(((struct _tuple15*)_tmp35D->hd)->f1 == 0)goto _LL1A2;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp35E=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((void*)(((struct _tuple15*)_tmp35D->hd)->f1)->hd);if(_tmp35E->tag != 1)goto _LL1A2;else{_tmp35F=_tmp35E->f1;}};_LL1A1: {
# 1487
const char*_tmp601;void*_tmp600[3];struct Cyc_String_pa_PrintArg_struct _tmp5FF;struct Cyc_String_pa_PrintArg_struct _tmp5FE;struct Cyc_String_pa_PrintArg_struct _tmp5FD;return(struct _dyneither_ptr)((_tmp5FD.tag=0,((_tmp5FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(r,_tmp32B)),((_tmp5FE.tag=0,((_tmp5FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35F),((_tmp5FF.tag=0,((_tmp5FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1487
Cyc_Absynpp_qvar2string(_tmp33D->name)),((_tmp600[0]=& _tmp5FF,((_tmp600[1]=& _tmp5FE,((_tmp600[2]=& _tmp5FD,Cyc_aprintf(((_tmp601="%s{.%s = %s}",_tag_dyneither(_tmp601,sizeof(char),13))),_tag_dyneither(_tmp600,sizeof(void*),3))))))))))))))))))));}_LL1A2:;_LL1A3:
# 1489
 goto _LL19F;_LL19F:;}{
# 1492
const char*_tmp606;void*_tmp605[2];struct Cyc_String_pa_PrintArg_struct _tmp604;struct Cyc_String_pa_PrintArg_struct _tmp603;return(struct _dyneither_ptr)((_tmp603.tag=0,((_tmp603.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp32B)),((_tmp604.tag=0,((_tmp604.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp33D->name)),((_tmp605[0]=& _tmp604,((_tmp605[1]=& _tmp603,Cyc_aprintf(((_tmp606="%s{%s}",_tag_dyneither(_tmp606,sizeof(char),7))),_tag_dyneither(_tmp605,sizeof(void*),2))))))))))))));};_LL18D: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp33F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp33F->tag != 8)goto _LL18F;else{_tmp340=_tmp33F->f2;}}_LL18E:
# 1494
 if(_tmp32B == 0){
const char*_tmp60A;void*_tmp609[1];struct Cyc_String_pa_PrintArg_struct _tmp608;return(struct _dyneither_ptr)((_tmp608.tag=0,((_tmp608.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp340->name)),((_tmp609[0]=& _tmp608,Cyc_aprintf(((_tmp60A="%s",_tag_dyneither(_tmp60A,sizeof(char),3))),_tag_dyneither(_tmp609,sizeof(void*),1))))))));}else{
# 1497
const char*_tmp60F;void*_tmp60E[2];struct Cyc_String_pa_PrintArg_struct _tmp60D;struct Cyc_String_pa_PrintArg_struct _tmp60C;return(struct _dyneither_ptr)((_tmp60C.tag=0,((_tmp60C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(r,_tmp32B)),((_tmp60D.tag=0,((_tmp60D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp340->name)),((_tmp60E[0]=& _tmp60D,((_tmp60E[1]=& _tmp60C,Cyc_aprintf(((_tmp60F="%s(%s)",_tag_dyneither(_tmp60F,sizeof(char),7))),_tag_dyneither(_tmp60E,sizeof(void*),2))))))))))))));}_LL18F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp341=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp341->tag != 9)goto _LL191;}_LL190: {
const char*_tmp610;return(_tmp610="NULL",_tag_dyneither(_tmp610,sizeof(char),5));}_LL191: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp342=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp342->tag != 10)goto _LL193;else{_tmp343=_tmp342->f2;}}_LL192: {
const char*_tmp614;void*_tmp613[1];struct Cyc_Int_pa_PrintArg_struct _tmp612;return(struct _dyneither_ptr)((_tmp612.tag=1,((_tmp612.f1=(unsigned long)_tmp343,((_tmp613[0]=& _tmp612,Cyc_aprintf(((_tmp614="%d",_tag_dyneither(_tmp614,sizeof(char),3))),_tag_dyneither(_tmp613,sizeof(void*),1))))))));}_LL193: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp344=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp344->tag != 11)goto _LL195;else{_tmp345=_tmp344->f1;}}_LL194: {
const char*_tmp618;void*_tmp617[1];struct Cyc_Int_pa_PrintArg_struct _tmp616;return(struct _dyneither_ptr)((_tmp616.tag=1,((_tmp616.f1=(unsigned long)((int)_tmp345),((_tmp617[0]=& _tmp616,Cyc_aprintf(((_tmp618="%d",_tag_dyneither(_tmp618,sizeof(char),3))),_tag_dyneither(_tmp617,sizeof(void*),1))))))));}_LL195: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp346=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp346->tag != 12)goto _LL197;else{_tmp347=_tmp346->f1;_tmp348=_tmp346->f2;}}_LL196:
 return _tmp347;_LL197: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp349=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp349->tag != 13)goto _LL199;else{_tmp34A=_tmp349->f2;}}_LL198:
 _tmp34C=_tmp34A;goto _LL19A;_LL199: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp34B=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp34B->tag != 14)goto _LL19B;else{_tmp34C=_tmp34B->f2;}}_LL19A:
 return Cyc_Absynpp_qvar2string(_tmp34C->name);_LL19B: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp34D=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp331;if(_tmp34D->tag != 17)goto _LL19D;else{_tmp34E=_tmp34D->f1;}}_LL19C:
 return Cyc_Absynpp_exp2string(_tmp34E);_LL19D:;_LL19E:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL17E:;};}_LL17C: {struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp32C=(struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp328;if(_tmp32C->tag != 1)goto _LL179;else{_tmp32D=_tmp32C->f1;}}_LL17D:
# 1508
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp32D)){const char*_tmp619;return(_tmp619="_",_tag_dyneither(_tmp619,sizeof(char),2));}{
struct Cyc_Tcpat_Con_s*_tmp378=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp32D);
struct Cyc_Absyn_Pat*_tmp379=_tmp378->orig_pat;
# 1513
if(_tmp379 == 0)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
void*_tmp37A=Cyc_Tcutil_compress((void*)_check_null(_tmp379->topt));void*_tmp37B=_tmp37A;struct Cyc_Absyn_PtrAtts _tmp37F;struct Cyc_Absyn_Datatypedecl*_tmp381;struct Cyc_Absyn_Aggrdecl*_tmp383;_LL1A5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp37B;if(_tmp37C->tag != 6)goto _LL1A7;else{if(_tmp37C->f2 != Cyc_Absyn_Char_sz)goto _LL1A7;}}_LL1A6:
# 1517
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp384=Cyc_Tcpat_char_con(r,(char)i,_tmp379);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp32D,_tmp384)){
const char*_tmp61D;void*_tmp61C[1];struct Cyc_Int_pa_PrintArg_struct _tmp61B;return(struct _dyneither_ptr)((_tmp61B.tag=1,((_tmp61B.f1=(unsigned long)i,((_tmp61C[0]=& _tmp61B,Cyc_aprintf(((_tmp61D="%d",_tag_dyneither(_tmp61D,sizeof(char),3))),_tag_dyneither(_tmp61C,sizeof(void*),1))))))));}}}
# 1522
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp37D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp37B;if(_tmp37D->tag != 6)goto _LL1A9;}_LL1A8:
# 1525
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp388=Cyc_Tcpat_int_con(r,(int)i,_tmp379);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp32D,_tmp388)){
const char*_tmp621;void*_tmp620[1];struct Cyc_Int_pa_PrintArg_struct _tmp61F;return(struct _dyneither_ptr)((_tmp61F.tag=1,((_tmp61F.f1=(unsigned long)((int)i),((_tmp620[0]=& _tmp61F,Cyc_aprintf(((_tmp621="%d",_tag_dyneither(_tmp621,sizeof(char),3))),_tag_dyneither(_tmp620,sizeof(void*),1))))))));}}}
# 1530
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp37E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37B;if(_tmp37E->tag != 5)goto _LL1AB;else{_tmp37F=(_tmp37E->f1).ptr_atts;}}_LL1AA: {
# 1532
union Cyc_Absyn_Constraint*_tmp38C=_tmp37F.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp38C);
if(is_nullable){
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp32D,Cyc_Tcpat_null_con(r,_tmp379))){
const char*_tmp622;return(_tmp622="NULL",_tag_dyneither(_tmp622,sizeof(char),5));}}{
# 1538
const char*_tmp623;return(_tmp623="&_",_tag_dyneither(_tmp623,sizeof(char),3));};}_LL1AB: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp380=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp37B;if(_tmp380->tag != 3)goto _LL1AD;else{if((((_tmp380->f1).datatype_info).KnownDatatype).tag != 2)goto _LL1AD;_tmp381=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp380->f1).datatype_info).KnownDatatype).val);}}_LL1AC:
# 1540
 if(_tmp381->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp38F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp381->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38F);
for(0;(unsigned int)_tmp38F;_tmp38F=_tmp38F->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp38F->hd)->name).f2;
struct Cyc_List_List*_tmp390=((struct Cyc_Absyn_Datatypefield*)_tmp38F->hd)->typs;
struct Cyc_Tcpat_Con_s*_tmp624;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp32D,((_tmp624=_cycalloc(sizeof(*_tmp624)),((_tmp624->name=Cyc_Tcpat_Name_v(n),((_tmp624->arity=0,((_tmp624->span=0,((_tmp624->orig_pat=_tmp379,_tmp624)))))))))))){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp390)== 0)
return n;else{
# 1550
const char*_tmp628;void*_tmp627[1];struct Cyc_String_pa_PrintArg_struct _tmp626;return(struct _dyneither_ptr)((_tmp626.tag=0,((_tmp626.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp627[0]=& _tmp626,Cyc_aprintf(((_tmp628="%s(...)",_tag_dyneither(_tmp628,sizeof(char),8))),_tag_dyneither(_tmp627,sizeof(void*),1))))))));}}}
# 1553
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};_LL1AD:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp382=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp37B;if(_tmp382->tag != 11)goto _LL1AF;else{if((((_tmp382->f1).aggr_info).KnownAggr).tag != 2)goto _LL1AF;_tmp383=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp382->f1).aggr_info).KnownAggr).val);}}if(!(_tmp383->kind == Cyc_Absyn_UnionA))goto _LL1AF;_LL1AE: {
# 1555
struct Cyc_List_List*_tmp395=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp383->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp395);
struct _tuple2*_tmp396=_tmp383->name;struct _dyneither_ptr _tmp398;struct _tuple2*_tmp397=_tmp396;_tmp398=*_tmp397->f2;
for(0;(unsigned int)_tmp395;_tmp395=_tmp395->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp395->hd)->name;
struct Cyc_Tcpat_Con_s*_tmp629;if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp32D,((_tmp629=_cycalloc(sizeof(*_tmp629)),((_tmp629->name=Cyc_Tcpat_Name_v(n),((_tmp629->arity=0,((_tmp629->span=0,((_tmp629->orig_pat=_tmp379,_tmp629)))))))))))){
const char*_tmp62E;void*_tmp62D[2];struct Cyc_String_pa_PrintArg_struct _tmp62C;struct Cyc_String_pa_PrintArg_struct _tmp62B;return(struct _dyneither_ptr)((_tmp62B.tag=0,((_tmp62B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n),((_tmp62C.tag=0,((_tmp62C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp398),((_tmp62D[0]=& _tmp62C,((_tmp62D[1]=& _tmp62B,Cyc_aprintf(((_tmp62E="%s{.%s = _}",_tag_dyneither(_tmp62E,sizeof(char),12))),_tag_dyneither(_tmp62D,sizeof(void*),2))))))))))))));}}
# 1563
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL1AF:;_LL1B0:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);_LL1A4:;};};_LL179:;}
# 1568
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(struct _RegionHandle*r,void*d){
struct _dyneither_ptr*_tmp62F;return(_tmp62F=_cycalloc(sizeof(*_tmp62F)),((_tmp62F[0]=Cyc_Tcpat_desc2string(r,d),_tmp62F)));}
# 1571
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct _RegionHandle*r,struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp39F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _RegionHandle*,void*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_desc2stringptr,r,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3A0=_tmp39F;for(0;_tmp3A0 != 0;_tmp3A0=((struct Cyc_List_List*)_check_null(_tmp3A0))->tl){
if(_tmp3A0->tl != 0){
{struct Cyc_List_List*_tmp630;_tmp3A0->tl=((_tmp630=_cycalloc(sizeof(*_tmp630)),((_tmp630->hd=comma,((_tmp630->tl=_tmp3A0->tl,_tmp630))))));}
_tmp3A0=_tmp3A0->tl;}}}
# 1580
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp39F);}
# 1583
static void Cyc_Tcpat_not_exhaust_err(struct _RegionHandle*r,unsigned int loc,void*desc){
struct _handler_cons _tmp3A3;_push_handler(& _tmp3A3);{int _tmp3A5=0;if(setjmp(_tmp3A3.handler))_tmp3A5=1;if(!_tmp3A5){
{struct _dyneither_ptr _tmp3A6=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp634;void*_tmp633[1];struct Cyc_String_pa_PrintArg_struct _tmp632;(_tmp632.tag=0,((_tmp632.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A6),((_tmp633[0]=& _tmp632,Cyc_Tcutil_terr(loc,((_tmp634="patterns may not be exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp634,sizeof(char),53))),_tag_dyneither(_tmp633,sizeof(void*),1)))))));}
# 1585
;_pop_handler();}else{void*_tmp3A4=(void*)_exn_thrown;void*_tmp3AB=_tmp3A4;void*_tmp3AD;_LL1B2: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3AC=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3AB;if(_tmp3AC->tag != Cyc_Tcpat_Desc2string)goto _LL1B4;}_LL1B3:
# 1589
{const char*_tmp637;void*_tmp636;(_tmp636=0,Cyc_Tcutil_terr(loc,((_tmp637="patterns may not be exhaustive.",_tag_dyneither(_tmp637,sizeof(char),32))),_tag_dyneither(_tmp636,sizeof(void*),0)));}
goto _LL1B1;_LL1B4: _tmp3AD=_tmp3AB;_LL1B5:(void)_throw(_tmp3AD);_LL1B1:;}};}
# 1593
static void Cyc_Tcpat_rule_occurs(int dummy,struct _tuple21*rhs){
(*rhs).f1=1;}
# 1597
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1602
struct _RegionHandle _tmp3B0=_new_region("r");struct _RegionHandle*r=& _tmp3B0;_push_region(r);
{int _tmp3B1=0;
struct _tuple25 _tmp638;struct _tuple25 _tmp3B2=(_tmp638.f1=r,((_tmp638.f2=& _tmp3B1,_tmp638)));
struct Cyc_List_List*_tmp3B3=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _tuple25*,struct Cyc_Absyn_Switch_clause*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match,& _tmp3B2,swcs);
void*_tmp3B4=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3B3);
int _tmp3B5=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3B4,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3B5);
# 1610
for(0;_tmp3B3 != 0;_tmp3B3=_tmp3B3->tl){
struct _tuple24*_tmp3B6=(struct _tuple24*)_tmp3B3->hd;int _tmp3B8;unsigned int _tmp3B9;struct _tuple24*_tmp3B7=_tmp3B6;_tmp3B8=(_tmp3B7->f2)->f1;_tmp3B9=(_tmp3B7->f2)->f2;
if(!_tmp3B8){
{const char*_tmp63B;void*_tmp63A;(_tmp63A=0,Cyc_Tcutil_terr(_tmp3B9,((_tmp63B="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp63B,sizeof(char),74))),_tag_dyneither(_tmp63A,sizeof(void*),0)));}
# 1615
break;}}}
# 1603
;_pop_region(r);}
# 1625
static void Cyc_Tcpat_not_exhaust_warn(struct _RegionHandle*r,struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3BD;_push_handler(& _tmp3BD);{int _tmp3BF=0;if(setjmp(_tmp3BD.handler))_tmp3BF=1;if(!_tmp3BF){
{struct _dyneither_ptr _tmp3C0=Cyc_Tcpat_desc2string(r,desc);
const char*_tmp63F;void*_tmp63E[1];struct Cyc_String_pa_PrintArg_struct _tmp63D;(_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C0),((_tmp63E[0]=& _tmp63D,Cyc_Tcutil_warn((*pr).f1,((_tmp63F="pattern not exhaustive.\n\tmissing case for %s",_tag_dyneither(_tmp63F,sizeof(char),45))),_tag_dyneither(_tmp63E,sizeof(void*),1)))))));}
# 1628
;_pop_handler();}else{void*_tmp3BE=(void*)_exn_thrown;void*_tmp3C5=_tmp3BE;void*_tmp3C7;_LL1B7: {struct Cyc_Tcpat_Desc2string_exn_struct*_tmp3C6=(struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3C5;if(_tmp3C6->tag != Cyc_Tcpat_Desc2string)goto _LL1B9;}_LL1B8:
# 1632
{const char*_tmp642;void*_tmp641;(_tmp641=0,Cyc_Tcutil_warn((*pr).f1,((_tmp642="pattern not exhaustive.",_tag_dyneither(_tmp642,sizeof(char),24))),_tag_dyneither(_tmp641,sizeof(void*),0)));}
goto _LL1B6;_LL1B9: _tmp3C7=_tmp3C5;_LL1BA:(void)_throw(_tmp3C7);_LL1B6:;}};};}
# 1636
static void Cyc_Tcpat_dummy_fn(int i,int j){
return;}struct _tuple26{void*f1;int f2;};
# 1639
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p){
struct _RegionHandle _tmp3CA=_new_region("r");struct _RegionHandle*r=& _tmp3CA;_push_region(r);
{struct _tuple26*_tmp645;struct Cyc_List_List*_tmp644;struct Cyc_List_List*_tmp3CB=(_tmp644=_region_malloc(r,sizeof(*_tmp644)),((_tmp644->hd=((_tmp645=_region_malloc(r,sizeof(*_tmp645)),((_tmp645->f1=Cyc_Tcpat_compile_pat(r,p),((_tmp645->f2=0,_tmp645)))))),((_tmp644->tl=0,_tmp644)))));
void*_tmp3CC=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3CB);
struct _tuple14 _tmp646;struct _tuple14 _tmp3CD=(_tmp646.f1=loc,((_tmp646.f2=1,_tmp646)));
int _tmp3CE=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,int),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3CC,Cyc_Tcpat_not_exhaust_warn,& _tmp3CD,Cyc_Tcpat_dummy_fn,0,& _tmp3CE);{
# 1647
int _tmp3CF=_tmp3CD.f2;_npop_handler(0);return _tmp3CF;};}
# 1641
;_pop_region(r);}
# 1655
static struct _tuple24*Cyc_Tcpat_get_match2(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*swc){
# 1657
void*sp0=Cyc_Tcpat_compile_pat(r,swc->pattern);
struct _tuple21*_tmp647;struct _tuple21*rhs=(_tmp647=_region_malloc(r,sizeof(*_tmp647)),((_tmp647->f1=0,((_tmp647->f2=(swc->pattern)->loc,_tmp647)))));
struct _tuple24*_tmp648;return(_tmp648=_region_malloc(r,sizeof(*_tmp648)),((_tmp648->f1=sp0,((_tmp648->f2=rhs,_tmp648)))));}
# 1661
static void Cyc_Tcpat_not_exhaust_err2(struct _RegionHandle*r,unsigned int loc,void*d){;}
# 1664
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs){
# 1666
struct _RegionHandle _tmp3D5=_new_region("r");struct _RegionHandle*r=& _tmp3D5;_push_region(r);
{struct Cyc_List_List*_tmp3D6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple24*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcpat_get_match2,r,swcs);
void*_tmp3D7=((void*(*)(struct _RegionHandle*r,struct Cyc_List_List*allmrules))Cyc_Tcpat_match_compile)(r,_tmp3D6);
int _tmp3D8=0;
((void(*)(struct _RegionHandle*r,void*d,void(*not_exhaust)(struct _RegionHandle*,unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct _tuple21*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(r,_tmp3D7,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3D8);
# 1672
for(0;_tmp3D6 != 0;_tmp3D6=_tmp3D6->tl){
struct _tuple24*_tmp3D9=(struct _tuple24*)_tmp3D6->hd;int _tmp3DB;unsigned int _tmp3DC;struct _tuple24*_tmp3DA=_tmp3D9;_tmp3DB=(_tmp3DA->f2)->f1;_tmp3DC=(_tmp3DA->f2)->f2;
if(!_tmp3DB){
{const char*_tmp64B;void*_tmp64A;(_tmp64A=0,Cyc_Tcutil_terr(_tmp3DC,((_tmp64B="redundant pattern (check for misspelled constructors in earlier patterns)",_tag_dyneither(_tmp64B,sizeof(char),74))),_tag_dyneither(_tmp64A,sizeof(void*),0)));}
break;}}}
# 1667
;_pop_region(r);}
