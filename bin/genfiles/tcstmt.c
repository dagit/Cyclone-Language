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
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 123
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
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
# 933
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 942
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 961
void*Cyc_Absyn_exn_typ();
# 983
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 999
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple2*,unsigned int);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1097
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1102
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1106
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1156
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1180
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 64 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 121 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 145
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 147
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_pat_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 155
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_loop(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*continue_dest);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_in_switch(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 162
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_next(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,void*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_try(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_get_try_depth(struct Cyc_Tcenv_Tenv*te);
# 184
void Cyc_Tcenv_process_continue(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_break(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt**);
void Cyc_Tcenv_process_goto(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt**);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;const struct Cyc_Tcenv_CList*f3;};
# 188
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 192
struct Cyc_Absyn_Stmt*Cyc_Tcenv_get_encloser(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_encloser(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*);
# 195
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_label(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr*,struct Cyc_Absyn_Stmt*);
int Cyc_Tcenv_all_labels_resolved(struct Cyc_Tcenv_Tenv*);
# 198
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*name);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 206
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,void*r,int resetable,int opened);
# 210
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 212
void Cyc_Tcenv_check_rgn_resetable(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 224
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 73
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 120
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 164
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 230 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 311
int Cyc_Tcutil_new_tvar_id();
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 42
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 54 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 58
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 60
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);
# 62
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p);
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_set_encloser(struct Cyc_Absyn_Stmt*enclosee,struct Cyc_Absyn_Stmt*encloser);
# 27 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
# 56 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp3;struct Cyc_Absyn_Exp*_tmp5;_LL1: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1;if(_tmp2->tag != 4)goto _LL3;else{_tmp3=_tmp2->f1;if(_tmp2->f2 != Cyc_Absyn_PostInc)goto _LL3;}}_LL2:
# 59
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp18B;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp18A;e->r=(void*)((_tmp18A=_cycalloc(sizeof(*_tmp18A)),((_tmp18A[0]=((_tmp18B.tag=4,((_tmp18B.f1=_tmp3,((_tmp18B.f2=Cyc_Absyn_PreInc,_tmp18B)))))),_tmp18A))));}goto _LL0;_LL3: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1;if(_tmp4->tag != 4)goto _LL5;else{_tmp5=_tmp4->f1;if(_tmp4->f2 != Cyc_Absyn_PostDec)goto _LL5;}}_LL4:
# 61
{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp18E;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp18D;e->r=(void*)((_tmp18D=_cycalloc(sizeof(*_tmp18D)),((_tmp18D[0]=((_tmp18E.tag=4,((_tmp18E.f1=_tmp5,((_tmp18E.f2=Cyc_Absyn_PreDec,_tmp18E)))))),_tmp18D))));}goto _LL0;_LL5:;_LL6:
 goto _LL0;_LL0:;}
# 66
static void Cyc_Tcstmt_decorate_stmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s){
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
s->try_depth=Cyc_Tcenv_get_try_depth(te);}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};
# 72
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 74
struct _tuple1*_tmpB;struct Cyc_List_List*_tmpC;struct Cyc_Tcpat_TcPatResult _tmpA=pat_res;_tmpB=_tmpA.tvars_and_bounds_opt;_tmpC=_tmpA.patvars;{
struct _tuple1 _tmpD=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpC);struct Cyc_List_List*_tmpF;struct _tuple1 _tmpE=_tmpD;_tmpF=_tmpE.f1;{
struct Cyc_List_List*_tmp10=_tmpB == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpB).f1);
struct Cyc_List_List*_tmp11=_tmpB == 0?0:(*_tmpB).f2;
struct Cyc_List_List*_tmp12=_tmpB == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*_tmpB).f1));
struct _RegionHandle _tmp13=_new_region("r");struct _RegionHandle*r=& _tmp13;_push_region(r);{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(r,loc,te,_tmp10);
for(0;_tmp12 != 0;_tmp12=_tmp12->tl){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp191;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp190;te2=Cyc_Tcenv_add_region(r,te2,(void*)((_tmp190=_cycalloc(sizeof(*_tmp190)),((_tmp190[0]=((_tmp191.tag=2,((_tmp191.f1=(struct Cyc_Absyn_Tvar*)_tmp12->hd,_tmp191)))),_tmp190)))),0,1);}
te2=Cyc_Tcenv_new_outlives_constraints(r,te2,_tmp11,loc);
if(new_block)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
void*_tmp16=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp17=_tmpF;for(0;_tmp17 != 0;_tmp17=_tmp17->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp17->hd != 0){
te2=Cyc_Tcenv_add_pat_var(r,loc,te2,*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp17->hd)));
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp17->hd)))->rgn=_tmp16;}}}
# 93
if(where_opt != 0){
const char*_tmp192;Cyc_Tcexp_tcTest(te2,where_opt,((_tmp192="switch clause guard",_tag_dyneither(_tmp192,sizeof(char),20))));}
# 96
if(_tmpF != 0)
te2=Cyc_Tcenv_set_encloser(r,te2,s);
Cyc_Tcstmt_tcStmt(te2,s,0);};}
# 101
if(_tmpF != 0)
Cyc_NewControlFlow_set_encloser(s,Cyc_Tcenv_get_encloser(te));
# 79
;_pop_region(r);};};}
# 105
static struct Cyc_List_List*Cyc_Tcstmt_cmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Tcenv_CList*const x){
# 107
if(x == (const struct Cyc_Tcenv_CList*)0)return 0;else{
struct Cyc_List_List*_tmp193;return(_tmp193=_region_malloc(r,sizeof(*_tmp193)),((_tmp193->hd=f(env,x->hd),((_tmp193->tl=Cyc_Tcstmt_cmap_c(r,f,env,x->tl),_tmp193)))));}}
# 114
static void Cyc_Tcstmt_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 117
struct _tuple0*_tmp19D;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp19C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp19B;struct Cyc_List_List*_tmp19A;struct Cyc_List_List*assump=(_tmp19A=_cycalloc(sizeof(*_tmp19A)),((_tmp19A->hd=((_tmp19D=_cycalloc(sizeof(*_tmp19D)),((_tmp19D->f1=(void*)& Cyc_Absyn_UniqueRgn_val,((_tmp19D->f2=(void*)((_tmp19B=_cycalloc(sizeof(*_tmp19B)),((_tmp19B[0]=((_tmp19C.tag=2,((_tmp19C.f1=tv,_tmp19C)))),_tmp19B)))),_tmp19D)))))),((_tmp19A->tl=0,_tmp19A)))));
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 129 "tcstmt.cyc"
struct _tuple0 _tmp19E;struct _tuple0 _tmp1A=(_tmp19E.f1=Cyc_Tcutil_compress(new_type),((_tmp19E.f2=Cyc_Tcutil_compress(new_type),_tmp19E)));struct _tuple0 _tmp1B=_tmp1A;struct Cyc_Absyn_PtrInfo _tmp1D;struct Cyc_Absyn_PtrInfo _tmp1F;_LL8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B.f1;if(_tmp1C->tag != 5)goto _LLA;else{_tmp1D=_tmp1C->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B.f2;if(_tmp1E->tag != 5)goto _LLA;else{_tmp1F=_tmp1E->f1;}};_LL9:
 goto _LL7;_LLA:;_LLB:
{const char*_tmp1A1;void*_tmp1A0;(_tmp1A0=0,Cyc_Tcutil_terr(loc,((_tmp1A1="alias requires pointer type",_tag_dyneither(_tmp1A1,sizeof(char),28))),_tag_dyneither(_tmp1A0,sizeof(void*),0)));}goto _LL7;_LL7:;}else{
# 134
const char*_tmp1A6;void*_tmp1A5[2];struct Cyc_String_pa_PrintArg_struct _tmp1A4;struct Cyc_String_pa_PrintArg_struct _tmp1A3;(_tmp1A3.tag=0,((_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type)),((_tmp1A4.tag=0,((_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type)),((_tmp1A5[0]=& _tmp1A4,((_tmp1A5[1]=& _tmp1A3,Cyc_Tcutil_terr(loc,((_tmp1A6="cannot alias value of type %s to type %s",_tag_dyneither(_tmp1A6,sizeof(char),41))),_tag_dyneither(_tmp1A5,sizeof(void*),2)))))))))))));}}
# 141
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 144
struct _RegionHandle _tmp2B=_new_region("r");struct _RegionHandle*r=& _tmp2B;_push_region(r);
{struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp2C=Cyc_Tcenv_add_region_equality(r,te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp2C,s0,new_block);
if(tk != 0)
((*((struct _tuple13*)_check_null(tk))).f1)->kind=(*((struct _tuple13*)_check_null(tk))).f2;}
# 145
;_pop_region(r);}
# 153
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp2D=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _dyneither_ptr*_tmp1B3;const char*_tmp1B2;void*_tmp1B1[1];struct Cyc_Int_pa_PrintArg_struct _tmp1B0;struct _tuple2*_tmp1AF;struct _tuple2*res=(_tmp1AF=_cycalloc(sizeof(*_tmp1AF)),((_tmp1AF->f1=Cyc_Absyn_Loc_n,((_tmp1AF->f2=((_tmp1B3=_cycalloc(sizeof(*_tmp1B3)),((_tmp1B3[0]=(struct _dyneither_ptr)((_tmp1B0.tag=1,((_tmp1B0.f1=(unsigned int)_tmp2D,((_tmp1B1[0]=& _tmp1B0,Cyc_aprintf(((_tmp1B2="_stmttmp%X",_tag_dyneither(_tmp1B2,sizeof(char),11))),_tag_dyneither(_tmp1B1,sizeof(void*),1)))))))),_tmp1B3)))),_tmp1AF)))));
return res;}
# 167 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 169
Cyc_Tcstmt_decorate_stmt(te,s0);{
# 171
void*_tmp33=s0->r;void*_tmp34=_tmp33;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Stmt*_tmp39;struct Cyc_Absyn_Stmt*_tmp3A;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Stmt*_tmp3F;struct Cyc_Absyn_Stmt*_tmp40;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_Absyn_Stmt*_tmp43;struct Cyc_Absyn_Stmt*_tmp44;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Stmt*_tmp48;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_Absyn_Stmt*_tmp4A;struct Cyc_Absyn_Stmt*_tmp4B;struct Cyc_Absyn_Stmt*_tmp4D;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Stmt*_tmp4F;struct Cyc_Absyn_Stmt**_tmp51;struct Cyc_Absyn_Stmt**_tmp53;struct _dyneither_ptr*_tmp55;struct Cyc_Absyn_Stmt**_tmp56;struct Cyc_List_List*_tmp58;struct Cyc_Absyn_Switch_clause***_tmp59;struct _dyneither_ptr*_tmp5B;struct Cyc_Absyn_Stmt*_tmp5C;struct Cyc_Absyn_Exp**_tmp5E;struct Cyc_List_List*_tmp5F;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_Decl*_tmp64;struct Cyc_Absyn_Stmt*_tmp65;struct Cyc_Absyn_Exp*_tmp67;_LLD: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp35=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp35->tag != 0)goto _LLF;}_LLE:
# 173
 return;_LLF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp36=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp36->tag != 1)goto _LL11;else{_tmp37=_tmp36->f1;}}_LL10:
# 176
 Cyc_Tcexp_tcExp(te,0,_tmp37);
if(!Cyc_Tcexp_in_stmt_exp)
Cyc_Tcstmt_simplify_unused_result_exp(_tmp37);
return;_LL11: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp38=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp38->tag != 2)goto _LL13;else{_tmp39=_tmp38->f1;_tmp3A=_tmp38->f2;}}_LL12: {
# 182
struct _RegionHandle _tmp68=_new_region("r");struct _RegionHandle*r=& _tmp68;_push_region(r);{
struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct _tmp1B6;struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct*_tmp1B5;struct Cyc_Tcenv_Tenv*_tmp69=Cyc_Tcenv_set_next(r,te,(void*)((_tmp1B5=_cycalloc(sizeof(*_tmp1B5)),((_tmp1B5[0]=((_tmp1B6.tag=3,((_tmp1B6.f1=_tmp3A,_tmp1B6)))),_tmp1B5)))));
Cyc_Tcstmt_tcStmt(_tmp69,_tmp39,1);}
# 186
Cyc_Tcstmt_tcStmt(te,_tmp3A,1);
_npop_handler(0);return;
# 182
;_pop_region(r);}_LL13: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3B=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp3B->tag != 3)goto _LL15;else{_tmp3C=_tmp3B->f1;}}_LL14: {
# 207 "tcstmt.cyc"
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp3C == 0){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_tmp6D=_tmp6C;_LL30: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6D;if(_tmp6E->tag != 0)goto _LL32;}_LL31:
 goto _LL2F;_LL32: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6D;if(_tmp6F->tag != 7)goto _LL34;}_LL33:
 goto _LL35;_LL34: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp70->tag != 6)goto _LL36;}_LL35:
# 213
{const char*_tmp1BA;void*_tmp1B9[1];struct Cyc_String_pa_PrintArg_struct _tmp1B8;(_tmp1B8.tag=0,((_tmp1B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1B9[0]=& _tmp1B8,Cyc_Tcutil_warn(s0->loc,((_tmp1BA="should return a value of type %s",_tag_dyneither(_tmp1BA,sizeof(char),33))),_tag_dyneither(_tmp1B9,sizeof(void*),1)))))));}
goto _LL2F;_LL36:;_LL37:
# 216
{const char*_tmp1BE;void*_tmp1BD[1];struct Cyc_String_pa_PrintArg_struct _tmp1BC;(_tmp1BC.tag=0,((_tmp1BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1BD[0]=& _tmp1BC,Cyc_Tcutil_terr(s0->loc,((_tmp1BE="must return a value of type %s",_tag_dyneither(_tmp1BE,sizeof(char),31))),_tag_dyneither(_tmp1BD,sizeof(void*),1)))))));}
goto _LL2F;_LL2F:;}else{
# 221
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp3C;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
{const char*_tmp1C3;void*_tmp1C2[2];struct Cyc_String_pa_PrintArg_struct _tmp1C1;struct Cyc_String_pa_PrintArg_struct _tmp1C0;(_tmp1C0.tag=0,((_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp1C1.tag=0,((_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt))),((_tmp1C2[0]=& _tmp1C1,((_tmp1C2[1]=& _tmp1C0,Cyc_Tcutil_terr(s0->loc,((_tmp1C3="returns value of type %s but requires %s",_tag_dyneither(_tmp1C3,sizeof(char),41))),_tag_dyneither(_tmp1C2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 229
struct _RegionHandle _tmp7B=_new_region("temp");struct _RegionHandle*temp=& _tmp7B;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e)){
const char*_tmp1C6;void*_tmp1C5;(_tmp1C5=0,Cyc_Tcutil_terr(e->loc,((_tmp1C6="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1C6,sizeof(char),49))),_tag_dyneither(_tmp1C5,sizeof(void*),0)));}
# 230
;_pop_region(temp);};}
# 234
return;}_LL15: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3D=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp3D->tag != 4)goto _LL17;else{_tmp3E=_tmp3D->f1;_tmp3F=_tmp3D->f2;_tmp40=_tmp3D->f3;}}_LL16: {
# 237
const char*_tmp1C7;struct Cyc_Tcexp_TestEnv _tmp7E=Cyc_Tcexp_tcTest(te,_tmp3E,((_tmp1C7="if statement",_tag_dyneither(_tmp1C7,sizeof(char),13))));struct _tuple0*_tmp80;int _tmp81;struct Cyc_Tcexp_TestEnv _tmp7F=_tmp7E;_tmp80=_tmp7F.eq;_tmp81=_tmp7F.isTrue;
{struct _tuple0*_tmp82=_tmp80;void*_tmp83;void*_tmp84;_LL39: if(_tmp82 == 0)goto _LL3B;_tmp83=_tmp82->f1;_tmp84=_tmp82->f2;_LL3A:
# 240
 if(_tmp81){
Cyc_Tcstmt_tcStmtRefine(te,_tmp3F,_tmp83,_tmp84,1);
Cyc_Tcstmt_tcStmt(te,_tmp40,1);}else{
# 245
Cyc_Tcstmt_tcStmt(te,_tmp3F,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp40,_tmp83,_tmp84,1);}
# 248
goto _LL38;_LL3B:;_LL3C:
# 250
 Cyc_Tcstmt_tcStmt(te,_tmp3F,1);
Cyc_Tcstmt_tcStmt(te,_tmp40,1);
goto _LL38;_LL38:;}
# 254
return;}_LL17: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp41=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp41->tag != 5)goto _LL19;else{_tmp42=(_tmp41->f1).f1;_tmp43=(_tmp41->f1).f2;_tmp44=_tmp41->f2;}}_LL18:
# 257
 Cyc_Tcstmt_decorate_stmt(te,_tmp43);
{const char*_tmp1C8;Cyc_Tcexp_tcTest(te,_tmp42,((_tmp1C8="while loop",_tag_dyneither(_tmp1C8,sizeof(char),11))));}{
struct _RegionHandle _tmp87=_new_region("r");struct _RegionHandle*r=& _tmp87;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp43),_tmp44,1);
# 262
_npop_handler(0);return;
# 259
;_pop_region(r);};_LL19: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp45=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp45->tag != 9)goto _LL1B;else{_tmp46=_tmp45->f1;_tmp47=(_tmp45->f2).f1;_tmp48=(_tmp45->f2).f2;_tmp49=(_tmp45->f3).f1;_tmp4A=(_tmp45->f3).f2;_tmp4B=_tmp45->f4;}}_LL1A:
# 265
 Cyc_Tcstmt_decorate_stmt(te,_tmp48);
Cyc_Tcstmt_decorate_stmt(te,_tmp4A);
Cyc_Tcexp_tcExp(te,0,_tmp46);
{const char*_tmp1C9;Cyc_Tcexp_tcTest(te,_tmp47,((_tmp1C9="for loop",_tag_dyneither(_tmp1C9,sizeof(char),9))));}{
struct _RegionHandle _tmp89=_new_region("r");struct _RegionHandle*r=& _tmp89;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp8A=Cyc_Tcenv_set_in_loop(r,te,_tmp4A);
Cyc_Tcstmt_tcStmt(_tmp8A,_tmp4B,1);
Cyc_Tcexp_tcExp(_tmp8A,0,_tmp49);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp49);}
# 275
_npop_handler(0);return;
# 269
;_pop_region(r);};_LL1B: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp4C=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp4C->tag != 14)goto _LL1D;else{_tmp4D=_tmp4C->f1;_tmp4E=(_tmp4C->f2).f1;_tmp4F=(_tmp4C->f2).f2;}}_LL1C:
# 278
 Cyc_Tcstmt_decorate_stmt(te,_tmp4F);{
struct _RegionHandle _tmp8B=_new_region("r");struct _RegionHandle*r=& _tmp8B;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_in_loop(r,te,_tmp4F),_tmp4D,1);
# 282
{const char*_tmp1CA;Cyc_Tcexp_tcTest(te,_tmp4E,((_tmp1CA="do loop",_tag_dyneither(_tmp1CA,sizeof(char),8))));}
_npop_handler(0);return;
# 279
;_pop_region(r);};_LL1D: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp50=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp50->tag != 6)goto _LL1F;else{_tmp51=(struct Cyc_Absyn_Stmt**)& _tmp50->f1;}}_LL1E:
# 287
 Cyc_Tcenv_process_break(te,s0,_tmp51);return;_LL1F: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp52=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp52->tag != 7)goto _LL21;else{_tmp53=(struct Cyc_Absyn_Stmt**)& _tmp52->f1;}}_LL20:
 Cyc_Tcenv_process_continue(te,s0,_tmp53);return;_LL21: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp54=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp54->tag != 8)goto _LL23;else{_tmp55=_tmp54->f1;_tmp56=(struct Cyc_Absyn_Stmt**)& _tmp54->f2;}}_LL22:
 Cyc_Tcenv_process_goto(te,s0,_tmp55,_tmp56);return;_LL23: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp57=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp57->tag != 11)goto _LL25;else{_tmp58=_tmp57->f1;_tmp59=(struct Cyc_Absyn_Switch_clause***)& _tmp57->f2;}}_LL24: {
# 292
const struct _tuple12*_tmp8D=Cyc_Tcenv_process_fallthru(te,s0,_tmp59);
if(_tmp8D == (const struct _tuple12*)0){
{const char*_tmp1CD;void*_tmp1CC;(_tmp1CC=0,Cyc_Tcutil_terr(s0->loc,((_tmp1CD="fallthru not in a non-last case",_tag_dyneither(_tmp1CD,sizeof(char),32))),_tag_dyneither(_tmp1CC,sizeof(void*),0)));}
return;}{
# 297
struct Cyc_List_List*_tmp90=(*_tmp8D).f2;
const struct Cyc_Tcenv_CList*_tmp91=(*_tmp8D).f3;
struct Cyc_List_List*instantiation=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,Cyc_Tcenv_lookup_type_vars(te),_tmp90);
struct Cyc_List_List*_tmp92=((struct Cyc_List_List*(*)(struct _RegionHandle*r,void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,const struct Cyc_Tcenv_CList*x))Cyc_Tcstmt_cmap_c)(Cyc_Core_heap_region,Cyc_Tcutil_substitute,instantiation,_tmp91);
for(0;_tmp92 != 0  && _tmp58 != 0;(_tmp92=_tmp92->tl,_tmp58=_tmp58->tl)){
# 304
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp58->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp58->hd,(void*)_tmp92->hd,& bogus)){
{const char*_tmp1D2;void*_tmp1D1[2];struct Cyc_String_pa_PrintArg_struct _tmp1D0;struct Cyc_String_pa_PrintArg_struct _tmp1CF;(_tmp1CF.tag=0,((_tmp1CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 309
Cyc_Absynpp_typ2string((void*)_tmp92->hd)),((_tmp1D0.tag=0,((_tmp1D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp58->hd)->topt))),((_tmp1D1[0]=& _tmp1D0,((_tmp1D1[1]=& _tmp1CF,Cyc_Tcutil_terr(s0->loc,((_tmp1D2="fallthru argument has type %s but pattern variable has type %s",_tag_dyneither(_tmp1D2,sizeof(char),63))),_tag_dyneither(_tmp1D1,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}{
# 313
struct _RegionHandle _tmp97=_new_region("temp");struct _RegionHandle*temp=& _tmp97;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_tmp92->hd) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp58->hd)){
const char*_tmp1D5;void*_tmp1D4;(_tmp1D4=0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp58->hd)->loc,((_tmp1D5="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1D5,sizeof(char),49))),_tag_dyneither(_tmp1D4,sizeof(void*),0)));}
# 314
;_pop_region(temp);};}
# 319
if(_tmp58 != 0){
const char*_tmp1D8;void*_tmp1D7;(_tmp1D7=0,Cyc_Tcutil_terr(s0->loc,((_tmp1D8="too many arguments in explicit fallthru",_tag_dyneither(_tmp1D8,sizeof(char),40))),_tag_dyneither(_tmp1D7,sizeof(void*),0)));}
if(_tmp92 != 0){
const char*_tmp1DB;void*_tmp1DA;(_tmp1DA=0,Cyc_Tcutil_terr(s0->loc,((_tmp1DB="too few arguments in explicit fallthru",_tag_dyneither(_tmp1DB,sizeof(char),39))),_tag_dyneither(_tmp1DA,sizeof(void*),0)));}
return;};}_LL25: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5A=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp5A->tag != 13)goto _LL27;else{_tmp5B=_tmp5A->f1;_tmp5C=_tmp5A->f2;}}_LL26: {
# 331
struct _RegionHandle _tmp9E=_new_region("r");struct _RegionHandle*r=& _tmp9E;_push_region(r);{
struct _dyneither_ptr*_tmp1E8;const char*_tmp1E7;void*_tmp1E6[1];struct Cyc_String_pa_PrintArg_struct _tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E4;Cyc_Tcstmt_tcStmt(Cyc_Tcenv_new_named_block(r,s0->loc,Cyc_Tcenv_add_label(te,_tmp5B,_tmp5C),(
(_tmp1E4=_cycalloc(sizeof(*_tmp1E4)),((_tmp1E4->name=((_tmp1E8=_cycalloc(sizeof(*_tmp1E8)),((_tmp1E8[0]=(struct _dyneither_ptr)((_tmp1E5.tag=0,((_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp5B),((_tmp1E6[0]=& _tmp1E5,Cyc_aprintf(((_tmp1E7="`%s",_tag_dyneither(_tmp1E7,sizeof(char),4))),_tag_dyneither(_tmp1E6,sizeof(void*),1)))))))),_tmp1E8)))),((_tmp1E4->identity=
Cyc_Tcutil_new_tvar_id(),((_tmp1E4->kind=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk),_tmp1E4))))))))),_tmp5C,0);}
# 337
_npop_handler(0);return;
# 331
;_pop_region(r);}_LL27: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5D=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp5D->tag != 10)goto _LL29;else{_tmp5E=(struct Cyc_Absyn_Exp**)& _tmp5D->f1;_tmp5F=_tmp5D->f2;}}_LL28: {
# 343
struct Cyc_Absyn_Exp*_tmpA4=*_tmp5E;
if(!Cyc_Tcutil_is_var_exp(_tmpA4)){
struct _tuple2*_tmpA5=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmpA6=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1EB;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp1EA;struct Cyc_Absyn_Decl*_tmpA7=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp1EA=_cycalloc(sizeof(*_tmp1EA)),((_tmp1EA[0]=((_tmp1EB.tag=1,((_tmp1EB.f1=Cyc_Absyn_new_vardecl(_tmpA5,
Cyc_Absyn_new_evar(0,0),0),((_tmp1EB.f2=
# 350
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpA4->loc),_tmp1EB)))))),_tmp1EA)))),_tmpA4->loc),_tmpA4,s0->loc);
# 353
struct Cyc_Absyn_Stmt*_tmpA8=Cyc_Absyn_decl_stmt(_tmpA7,_tmpA6,s0->loc);
# 355
s0->r=_tmpA8->r;
*_tmp5E=Cyc_Absyn_var_exp(_tmpA5,_tmpA4->loc);
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 361
Cyc_Tcexp_tcExp(te,0,_tmpA4);{
void*_tmpAB=(void*)_check_null(_tmpA4->topt);
# 366
struct _RegionHandle _tmpAC=_new_region("r");struct _RegionHandle*r=& _tmpAC;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpAB) && !Cyc_Tcutil_is_noalias_path(r,_tmpA4)){
const char*_tmp1EE;void*_tmp1ED;(_tmp1ED=0,Cyc_Tcutil_terr(_tmpA4->loc,((_tmp1EE="Cannot consume non-unique paths; do swap instead",_tag_dyneither(_tmp1EE,sizeof(char),49))),_tag_dyneither(_tmp1ED,sizeof(void*),0)));}{
struct Cyc_Tcenv_Tenv*_tmpAF=Cyc_Tcenv_set_in_switch(r,te);
_tmpAF=Cyc_Tcenv_clear_fallthru(r,_tmpAF);{
struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5F);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmpB0=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmpB1=Cyc_Tcpat_tcPat(_tmpAF,_tmpB0,& _tmpAB,_tmpA4);
struct Cyc_List_List*_tmpB2=_tmpB1.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpB1.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpB0->topt),_tmpAB)){
{const char*_tmp1F3;void*_tmp1F2[2];struct Cyc_String_pa_PrintArg_struct _tmp1F1;struct Cyc_String_pa_PrintArg_struct _tmp1F0;(_tmp1F0.tag=0,((_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB0->topt))),((_tmp1F1.tag=0,((_tmp1F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpAB)),((_tmp1F2[0]=& _tmp1F1,((_tmp1F2[1]=& _tmp1F0,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,((_tmp1F3="switch on type %s, but case expects type %s",_tag_dyneither(_tmp1F3,sizeof(char),44))),_tag_dyneither(_tmp1F2,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_explain_failure();}else{
# 382
Cyc_Tcpat_check_pat_regions(_tmpAF,_tmpB0,_tmpB1.patvars);}
{struct Cyc_Core_Opt*_tmp1F4;((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=((_tmp1F4=_cycalloc(sizeof(*_tmp1F4)),((_tmp1F4->v=_tmpB1.patvars,_tmp1F4))));}
# 385
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmpAF,_tmpB1,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 387
if(_tmpB1.tvars_and_bounds_opt != 0  && (*_tmpB1.tvars_and_bounds_opt).f2 != 0)
# 389
_tmpAF=Cyc_Tcenv_clear_fallthru(r,_tmpAF);else{
# 391
struct Cyc_List_List*_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpB1.patvars)).f1);
_tmpB8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpB8);
_tmpAF=Cyc_Tcenv_set_fallthru(r,_tmpAF,_tmpB2,_tmpB8,(struct Cyc_Absyn_Switch_clause*)scs->hd);}}};};
# 397
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp5F);
_npop_handler(0);return;
# 366
;_pop_region(r);};}_LL29: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp60=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp60->tag != 15)goto _LL2B;else{_tmp61=_tmp60->f1;_tmp62=_tmp60->f2;}}_LL2A:
# 405
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1F7;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1F6;_tmp61->r=(Cyc_Absyn_new_stmt((void*)((_tmp1F6=_cycalloc(sizeof(*_tmp1F6)),((_tmp1F6[0]=((_tmp1F7.tag=2,((_tmp1F7.f1=Cyc_Absyn_new_stmt(_tmp61->r,_tmp61->loc),((_tmp1F7.f2=Cyc_Absyn_skip_stmt(_tmp61->loc),_tmp1F7)))))),_tmp1F6)))),_tmp61->loc))->r;}{
# 409
struct _RegionHandle _tmpBB=_new_region("r");struct _RegionHandle*r=& _tmpBB;_push_region(r);
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(r,Cyc_Tcenv_enter_try(r,te),s0),_tmp61,1);{
# 413
struct _RegionHandle _tmpBC=_new_region("r2");struct _RegionHandle*r2=& _tmpBC;_push_region(r2);{
# 415
struct Cyc_Tcenv_Tenv*_tmpBD=Cyc_Tcenv_set_in_switch(r2,te);
_tmpBD=Cyc_Tcenv_clear_fallthru(r2,_tmpBD);{
struct Cyc_List_List*_tmpBE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp62);for(0;_tmpBE != 0;_tmpBE=_tmpBE->tl){
struct Cyc_Absyn_Pat*_tmpBF=((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->pattern;
void*_tmpC0=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmpC1=Cyc_Tcpat_tcPat(_tmpBD,_tmpBF,& _tmpC0,0);
# 423
struct Cyc_List_List*_tmpC2=_tmpC1.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmpC1.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpBF->topt),_tmpC0)){
const char*_tmp1FB;void*_tmp1FA[1];struct Cyc_String_pa_PrintArg_struct _tmp1F9;(_tmp1F9.tag=0,((_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpBF->topt))),((_tmp1FA[0]=& _tmp1F9,Cyc_Tcutil_terr(((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->loc,((_tmp1FB="expected datatype exn@ but found %s",_tag_dyneither(_tmp1FB,sizeof(char),36))),_tag_dyneither(_tmp1FA,sizeof(void*),1)))))));}else{
# 429
Cyc_Tcpat_check_pat_regions(_tmpBD,_tmpBF,_tmpC1.patvars);}
{struct Cyc_Core_Opt*_tmp1FC;((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->pat_vars=((_tmp1FC=_cycalloc(sizeof(*_tmp1FC)),((_tmp1FC->v=_tmpC1.patvars,_tmp1FC))));}
# 432
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->loc,_tmpBD,_tmpC1,((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmpBE->hd)->where_clause,1);
# 434
if(_tmpC1.tvars_and_bounds_opt != 0  && (*_tmpC1.tvars_and_bounds_opt).f2 != 0)
# 436
_tmpBD=Cyc_Tcenv_clear_fallthru(r2,_tmpBD);else{
# 438
struct Cyc_List_List*_tmpC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmpC1.patvars)).f1);
_tmpC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC7);
_tmpBD=Cyc_Tcenv_set_fallthru(r2,_tmpBD,_tmpC2,_tmpC7,(struct Cyc_Absyn_Switch_clause*)_tmpBE->hd);}}};}
# 444
Cyc_Tcpat_check_catch_overlap(s0->loc,te,_tmp62);
_npop_handler(1);return;
# 413
;_pop_region(r2);};
# 409
;_pop_region(r);};_LL2B: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp63=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp63->tag != 12)goto _LL2D;else{_tmp64=_tmp63->f1;_tmp65=_tmp63->f2;}}_LL2C: {
# 448
struct _dyneither_ptr unimp_msg_part;
struct _RegionHandle _tmpC8=_new_region("decl_rgn");struct _RegionHandle*decl_rgn=& _tmpC8;_push_region(decl_rgn);{
struct Cyc_Tcenv_Tenv*te2;
if(new_block)
te2=Cyc_Tcenv_new_block(decl_rgn,s0->loc,te);else{
# 454
te2=Cyc_Tcenv_copy_tenv(decl_rgn,te);}{
struct Cyc_Tcenv_Tenv*_tmpC9=te2;
void*_tmpCA=_tmp64->r;void*_tmpCB=_tmpCA;struct Cyc_Absyn_Vardecl*_tmpCD;struct Cyc_Absyn_Pat*_tmpCF;struct Cyc_Core_Opt**_tmpD0;struct Cyc_Absyn_Exp**_tmpD1;struct Cyc_List_List*_tmpD3;struct Cyc_Absyn_Tvar*_tmpD5;struct Cyc_Absyn_Vardecl*_tmpD6;int _tmpD7;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Fndecl*_tmpDA;struct _dyneither_ptr*_tmpDC;struct Cyc_List_List*_tmpDD;struct _tuple2*_tmpDF;struct Cyc_List_List*_tmpE0;_LL3E: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpCC=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpCC->tag != 0)goto _LL40;else{_tmpCD=_tmpCC->f1;}}_LL3F: {
# 458
enum Cyc_Absyn_Scope _tmpEA;union Cyc_Absyn_Nmspace _tmpEB;struct _dyneither_ptr*_tmpEC;struct Cyc_Absyn_Tqual _tmpED;void*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;void**_tmpF0;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Vardecl*_tmpE9=_tmpCD;_tmpEA=_tmpE9->sc;_tmpEB=(_tmpE9->name)->f1;_tmpEC=(_tmpE9->name)->f2;_tmpED=_tmpE9->tq;_tmpEE=_tmpE9->type;_tmpEF=_tmpE9->initializer;_tmpF0=(void**)& _tmpE9->rgn;_tmpF1=_tmpE9->attributes;{
void*_tmpF2=Cyc_Tcenv_curr_rgn(_tmpC9);
int is_local;
switch(_tmpEA){case Cyc_Absyn_Static: _LL5C:
 goto _LL5D;case Cyc_Absyn_Extern: _LL5D:
 goto _LL5E;case Cyc_Absyn_ExternC: _LL5E:
# 465
 _tmpCD->escapes=1;
is_local=0;break;case Cyc_Absyn_Abstract: _LL5F:
# 468
{const char*_tmp1FF;void*_tmp1FE;(_tmp1FE=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp1FF="bad abstract scope for local variable",_tag_dyneither(_tmp1FF,sizeof(char),38))),_tag_dyneither(_tmp1FE,sizeof(void*),0)));}
goto _LL60;case Cyc_Absyn_Register: _LL60:
 goto _LL61;case Cyc_Absyn_Public: _LL61:
 is_local=1;break;}
# 474
*_tmpF0=is_local?_tmpF2:(void*)& Cyc_Absyn_HeapRgn_val;
{union Cyc_Absyn_Nmspace _tmpF5=_tmpEB;_LL64: if((_tmpF5.Loc_n).tag != 4)goto _LL66;_LL65:
 goto _LL63;_LL66: if((_tmpF5.Rel_n).tag != 1)goto _LL68;if((struct Cyc_List_List*)(_tmpF5.Rel_n).val != 0)goto _LL68;_LL67:
# 478
(*_tmpCD->name).f1=Cyc_Absyn_Loc_n;
goto _LL63;_LL68: if((_tmpF5.C_n).tag != 3)goto _LL6A;_LL69:
# 482
 goto _LL6B;_LL6A: if((_tmpF5.Abs_n).tag != 2)goto _LL6C;_LL6B:
 goto _LL6D;_LL6C:;_LL6D:
# 485
{const char*_tmp202;void*_tmp201;(_tmp201=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp202="cannot declare a qualified local variable",_tag_dyneither(_tmp202,sizeof(char),42))),_tag_dyneither(_tmp201,sizeof(void*),0)));}
(*_tmpCD->name).f1=Cyc_Absyn_Loc_n;
goto _LL63;_LL63:;}
# 490
{void*_tmpF8=Cyc_Tcutil_compress(_tmpEE);void*_tmpF9=_tmpF8;void*_tmpFC;struct Cyc_Absyn_Tqual _tmpFD;union Cyc_Absyn_Constraint*_tmpFE;unsigned int _tmpFF;_LL6F:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpFA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpF9;if(_tmpFA->tag != 9)goto _LL71;}if(!is_local)goto _LL71;_LL70:
# 493
 _tmpCD->escapes=1;
_tmpEA=Cyc_Absyn_Extern;
is_local=0;
goto _LL6E;_LL71:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpFB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF9;if(_tmpFB->tag != 8)goto _LL73;else{_tmpFC=(_tmpFB->f1).elt_type;_tmpFD=(_tmpFB->f1).tq;if((_tmpFB->f1).num_elts != 0)goto _LL73;_tmpFE=(_tmpFB->f1).zero_term;_tmpFF=(_tmpFB->f1).zt_loc;}}if(!(_tmpCD->initializer != 0))goto _LL73;_LL72:
# 498
{void*_tmp100=((struct Cyc_Absyn_Exp*)_check_null(_tmpCD->initializer))->r;void*_tmp101=_tmp100;struct _dyneither_ptr _tmp103;struct _dyneither_ptr _tmp105;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_List_List*_tmp10B;struct Cyc_List_List*_tmp10D;_LL76: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp102->tag != 0)goto _LL78;else{if(((_tmp102->f1).String_c).tag != 8)goto _LL78;_tmp103=(struct _dyneither_ptr)((_tmp102->f1).String_c).val;}}_LL77:
# 500
 _tmpEE=(_tmpCD->type=Cyc_Absyn_array_typ(_tmpFC,_tmpFD,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp103,sizeof(char)),0),_tmpFE,_tmpFF));
goto _LL75;_LL78: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp104->tag != 0)goto _LL7A;else{if(((_tmp104->f1).Wstring_c).tag != 9)goto _LL7A;_tmp105=(struct _dyneither_ptr)((_tmp104->f1).Wstring_c).val;}}_LL79:
# 503
 _tmpEE=(_tmpCD->type=Cyc_Absyn_array_typ(_tmpFC,_tmpFD,Cyc_Absyn_uint_exp(1,0),_tmpFE,_tmpFF));
goto _LL75;_LL7A: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp106->tag != 26)goto _LL7C;else{_tmp107=_tmp106->f2;}}_LL7B:
 _tmp109=_tmp107;goto _LL7D;_LL7C: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp108->tag != 27)goto _LL7E;else{_tmp109=_tmp108->f1;}}_LL7D:
# 508
 _tmpEE=(_tmpCD->type=Cyc_Absyn_array_typ(_tmpFC,_tmpFD,_tmp109,_tmpFE,_tmpFF));
goto _LL75;_LL7E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp10A->tag != 35)goto _LL80;else{_tmp10B=_tmp10A->f2;}}_LL7F:
 _tmp10D=_tmp10B;goto _LL81;_LL80: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp10C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp101;if(_tmp10C->tag != 25)goto _LL82;else{_tmp10D=_tmp10C->f1;}}_LL81:
# 512
 _tmpEE=(_tmpCD->type=Cyc_Absyn_array_typ(_tmpFC,_tmpFD,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10D),0),_tmpFE,_tmpFF));
# 514
goto _LL75;_LL82:;_LL83:
 goto _LL75;_LL75:;}
# 517
goto _LL6E;_LL73:;_LL74:
 goto _LL6E;_LL6E:;}{
# 524
struct Cyc_List_List*_tmp10E=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpC9);
int _tmp10F=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpC9,_tmp10E,& Cyc_Tcutil_tmk,_tmp10F,_tmpEE);
(_tmpCD->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmpCD->tq).print_const,_tmpEE);{
# 529
struct Cyc_Tcenv_Tenv*_tmp110=Cyc_Tcenv_add_local_var(decl_rgn,_tmp65->loc,_tmpC9,_tmpCD);
if(_tmpEA == Cyc_Absyn_Extern  || _tmpEA == Cyc_Absyn_ExternC){
const char*_tmp205;void*_tmp204;(_tmp204=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp205="extern declarations are not yet supported within functions",_tag_dyneither(_tmp205,sizeof(char),59))),_tag_dyneither(_tmp204,sizeof(void*),0)));}
if(_tmpEF != 0){
Cyc_Tcexp_tcExpInitializer(_tmp110,& _tmpEE,_tmpEF);
# 535
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpEF)){
const char*_tmp208;void*_tmp207;(_tmp207=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp208="initializer for static variable needs to be a constant expression",_tag_dyneither(_tmp208,sizeof(char),66))),_tag_dyneither(_tmp207,sizeof(void*),0)));}
if(!Cyc_Tcutil_coerce_assign(_tmp110,_tmpEF,_tmpEE)){
struct _dyneither_ptr _tmp115=*_tmpEC;
struct _dyneither_ptr _tmp116=Cyc_Absynpp_typ2string(_tmpEE);
struct _dyneither_ptr _tmp117=Cyc_Absynpp_typ2string((void*)_check_null(_tmpEF->topt));
if(((_get_dyneither_size(_tmp115,sizeof(char))+ _get_dyneither_size(_tmp116,sizeof(char)))+ _get_dyneither_size(_tmp117,sizeof(char)))+ 50 < 80){
const char*_tmp20E;void*_tmp20D[3];struct Cyc_String_pa_PrintArg_struct _tmp20C;struct Cyc_String_pa_PrintArg_struct _tmp20B;struct Cyc_String_pa_PrintArg_struct _tmp20A;(_tmp20A.tag=0,((_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp117),((_tmp20B.tag=0,((_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp116),((_tmp20C.tag=0,((_tmp20C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115),((_tmp20D[0]=& _tmp20C,((_tmp20D[1]=& _tmp20B,((_tmp20D[2]=& _tmp20A,Cyc_Tcutil_terr(_tmp64->loc,((_tmp20E="%s was declared with type %s but initialized with type %s.",_tag_dyneither(_tmp20E,sizeof(char),59))),_tag_dyneither(_tmp20D,sizeof(void*),3)))))))))))))))))));}else{
# 544
if((_get_dyneither_size(_tmp115,sizeof(char))+ _get_dyneither_size(_tmp116,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmp117,sizeof(char))+ 25 < 80){
const char*_tmp214;void*_tmp213[3];struct Cyc_String_pa_PrintArg_struct _tmp212;struct Cyc_String_pa_PrintArg_struct _tmp211;struct Cyc_String_pa_PrintArg_struct _tmp210;(_tmp210.tag=0,((_tmp210.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp117),((_tmp211.tag=0,((_tmp211.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp116),((_tmp212.tag=0,((_tmp212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115),((_tmp213[0]=& _tmp212,((_tmp213[1]=& _tmp211,((_tmp213[2]=& _tmp210,Cyc_Tcutil_terr(_tmp64->loc,((_tmp214="%s was declared with type %s\n but initialized with type %s.",_tag_dyneither(_tmp214,sizeof(char),60))),_tag_dyneither(_tmp213,sizeof(void*),3)))))))))))))))))));}else{
# 549
const char*_tmp21A;void*_tmp219[3];struct Cyc_String_pa_PrintArg_struct _tmp218;struct Cyc_String_pa_PrintArg_struct _tmp217;struct Cyc_String_pa_PrintArg_struct _tmp216;(_tmp216.tag=0,((_tmp216.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp117),((_tmp217.tag=0,((_tmp217.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp116),((_tmp218.tag=0,((_tmp218.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115),((_tmp219[0]=& _tmp218,((_tmp219[1]=& _tmp217,((_tmp219[2]=& _tmp216,Cyc_Tcutil_terr(_tmp64->loc,((_tmp21A="%s declared with type \n%s\n but initialized with type \n%s.",_tag_dyneither(_tmp21A,sizeof(char),58))),_tag_dyneither(_tmp219,sizeof(void*),3)))))))))))))))))));}}
# 551
Cyc_Tcutil_unify(_tmpEE,(void*)_check_null(_tmpEF->topt));
Cyc_Tcutil_explain_failure();}}
# 555
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp110,s0),_tmp65,0);
_npop_handler(0);return;};};};}_LL40: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpCE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpCE->tag != 2)goto _LL42;else{_tmpCF=_tmpCE->f1;_tmpD0=(struct Cyc_Core_Opt**)& _tmpCE->f2;_tmpD1=(struct Cyc_Absyn_Exp**)& _tmpCE->f3;}}_LL41: {
# 563
struct Cyc_Absyn_Exp*_tmp127=*_tmpD1;
{void*_tmp128=_tmpCF->r;void*_tmp129=_tmp128;_LL85: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp12A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp129;if(_tmp12A->tag != 1)goto _LL87;}_LL86:
 goto _LL88;_LL87: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp12B=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp129;if(_tmp12B->tag != 2)goto _LL89;}_LL88:
 goto _LL8A;_LL89: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp12C=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp129;if(_tmp12C->tag != 15)goto _LL8B;}_LL8A:
 goto _LL84;_LL8B:;_LL8C:
# 569
 if(!Cyc_Tcutil_is_var_exp(_tmp127)){
struct _tuple2*_tmp12D=Cyc_Tcstmt_stmt_temp_var();
struct Cyc_Absyn_Stmt*_tmp12E=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp21D;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp21C;struct Cyc_Absyn_Decl*_tmp12F=Cyc_Absyn_let_decl(Cyc_Absyn_new_pat((void*)((_tmp21C=_cycalloc(sizeof(*_tmp21C)),((_tmp21C[0]=((_tmp21D.tag=1,((_tmp21D.f1=Cyc_Absyn_new_vardecl(_tmp12D,
Cyc_Absyn_new_evar(0,0),0),((_tmp21D.f2=
# 575
Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp127->loc),_tmp21D)))))),_tmp21C)))),_tmp127->loc),_tmp127,s0->loc);
# 578
struct Cyc_Absyn_Stmt*_tmp130=Cyc_Absyn_decl_stmt(_tmp12F,_tmp12E,s0->loc);
# 580
s0->r=_tmp130->r;
*_tmpD1=Cyc_Absyn_var_exp(_tmp12D,_tmp127->loc);
Cyc_Tcstmt_tcStmt(_tmpC9,s0,new_block);
_npop_handler(0);return;}_LL84:;}
# 586
Cyc_Tcexp_tcExpInitializer(_tmpC9,0,_tmp127);{
# 588
void*pat_type=(void*)_check_null(_tmp127->topt);
# 590
struct Cyc_Tcpat_TcPatResult _tmp133=Cyc_Tcpat_tcPat(_tmpC9,_tmpCF,& pat_type,_tmp127);
{struct Cyc_Core_Opt*_tmp21E;*_tmpD0=((_tmp21E=_cycalloc(sizeof(*_tmp21E)),((_tmp21E->v=_tmp133.patvars,_tmp21E))));}
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpCF->topt),(void*)_check_null(_tmp127->topt)) && !
Cyc_Tcutil_coerce_assign(_tmpC9,_tmp127,(void*)_check_null(_tmpCF->topt))){
{const char*_tmp223;void*_tmp222[2];struct Cyc_String_pa_PrintArg_struct _tmp221;struct Cyc_String_pa_PrintArg_struct _tmp220;(_tmp220.tag=0,((_tmp220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp127->topt))),((_tmp221.tag=0,((_tmp221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpCF->topt))),((_tmp222[0]=& _tmp221,((_tmp222[1]=& _tmp220,Cyc_Tcutil_terr(_tmp64->loc,((_tmp223="pattern type %s does not match definition type %s",_tag_dyneither(_tmp223,sizeof(char),50))),_tag_dyneither(_tmp222,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unify((void*)_check_null(_tmpCF->topt),(void*)_check_null(_tmp127->topt));
Cyc_Tcutil_explain_failure();}else{
# 600
Cyc_Tcpat_check_pat_regions(_tmpC9,_tmpCF,_tmp133.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmpCF->loc,_tmpC9,_tmpCF);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpC9,_tmp133,_tmp65,0,0);
_npop_handler(0);return;};}_LL42: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpD2=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpD2->tag != 3)goto _LL44;else{_tmpD3=_tmpD2->f1;}}_LL43: {
# 606
void*_tmp139=Cyc_Tcenv_curr_rgn(_tmpC9);
struct Cyc_Tcenv_Tenv*_tmp13A=_tmpC9;
for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
struct Cyc_Absyn_Vardecl*_tmp13B=(struct Cyc_Absyn_Vardecl*)_tmpD3->hd;
union Cyc_Absyn_Nmspace _tmp13D;void*_tmp13E;void**_tmp13F;struct Cyc_Absyn_Vardecl*_tmp13C=_tmp13B;_tmp13D=(_tmp13C->name)->f1;_tmp13E=_tmp13C->type;_tmp13F=(void**)& _tmp13C->rgn;
*_tmp13F=_tmp139;
{union Cyc_Absyn_Nmspace _tmp140=_tmp13D;struct Cyc_List_List*_tmp141;_LL8E: if((_tmp140.Loc_n).tag != 4)goto _LL90;_LL8F:
 goto _LL8D;_LL90: if((_tmp140.Rel_n).tag != 1)goto _LL92;if((struct Cyc_List_List*)(_tmp140.Rel_n).val != 0)goto _LL92;_LL91:
# 615
(*_tmp13B->name).f1=Cyc_Absyn_Loc_n;
goto _LL8D;_LL92: if((_tmp140.Abs_n).tag != 2)goto _LL94;_tmp141=(struct Cyc_List_List*)(_tmp140.Abs_n).val;_LL93: {
const char*_tmp226;void*_tmp225;(int)_throw(((_tmp225=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp226="tcstmt: Abs_n declaration",_tag_dyneither(_tmp226,sizeof(char),26))),_tag_dyneither(_tmp225,sizeof(void*),0)))));}_LL94:;_LL95:
# 621
{const char*_tmp229;void*_tmp228;(_tmp228=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp229="cannot declare a qualified local variable",_tag_dyneither(_tmp229,sizeof(char),42))),_tag_dyneither(_tmp228,sizeof(void*),0)));}
goto _LL8D;_LL8D:;}
# 624
Cyc_Tcutil_check_type(s0->loc,_tmp13A,Cyc_Tcenv_lookup_type_vars(_tmp13A),& Cyc_Tcutil_tmk,1,_tmp13E);
_tmp13A=Cyc_Tcenv_add_local_var(decl_rgn,_tmp65->loc,_tmp13A,_tmp13B);}
# 627
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmp13A,s0),_tmp65,0);
_npop_handler(0);return;}_LL44: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpD4=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpD4->tag != 4)goto _LL46;else{_tmpD5=_tmpD4->f1;_tmpD6=_tmpD4->f2;_tmpD7=_tmpD4->f3;_tmpD8=_tmpD4->f4;}}_LL45:
# 633
{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp22C;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp22B;_tmp65->r=(Cyc_Absyn_new_stmt((void*)((_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B[0]=((_tmp22C.tag=2,((_tmp22C.f1=Cyc_Absyn_new_stmt(_tmp65->r,_tmp65->loc),((_tmp22C.f2=Cyc_Absyn_skip_stmt(_tmp65->loc),_tmp22C)))))),_tmp22B)))),_tmp65->loc))->r;}{
# 635
struct Cyc_Tcenv_Tenv*_tmp148=_tmpC9;
void**_tmp14A;void**_tmp14B;struct Cyc_Absyn_Vardecl*_tmp149=_tmpD6;_tmp14A=(void**)& _tmp149->type;_tmp14B=(void**)& _tmp149->rgn;{
void*_tmp14C=Cyc_Tcenv_curr_rgn(_tmpC9);
*_tmp14B=_tmp14C;{
void*rgn_typ;
if(_tmpD8 != 0){
{const char*_tmp22F;void*_tmp22E;(_tmp22E=0,Cyc_Tcutil_warn(s0->loc,((_tmp22F="dynamic regions using open are deprecated -- use keys instead",_tag_dyneither(_tmp22F,sizeof(char),62))),_tag_dyneither(_tmp22E,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*open_exp=_tmpD8;
struct Cyc_List_List*_tmp14F=Cyc_Tcenv_lookup_type_vars(_tmpC9);
{struct Cyc_Core_Opt*_tmp230;rgn_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp230=_cycalloc(sizeof(*_tmp230)),((_tmp230->v=_tmp14F,_tmp230)))));}{
struct Cyc_Core_Opt*_tmp231;void*rgn2_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231->v=_tmp14F,_tmp231)))));
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp234;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp233;void*expected=(void*)((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233[0]=((_tmp234.tag=16,((_tmp234.f1=rgn_typ,((_tmp234.f2=rgn2_typ,_tmp234)))))),_tmp233))));
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp237;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp236;*_tmp14A=(void*)((_tmp236=_cycalloc(sizeof(*_tmp236)),((_tmp236[0]=((_tmp237.tag=15,((_tmp237.f1=rgn_typ,_tmp237)))),_tmp236))));}
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(_tmpC9,& expected,open_exp),expected)){
{const char*_tmp23C;void*_tmp23B[2];struct Cyc_String_pa_PrintArg_struct _tmp23A;struct Cyc_String_pa_PrintArg_struct _tmp239;(_tmp239.tag=0,((_tmp239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 651
Cyc_Absynpp_typ2string((void*)_check_null(open_exp->topt))),((_tmp23A.tag=0,((_tmp23A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 650
Cyc_Absynpp_typ2string(expected)),((_tmp23B[0]=& _tmp23A,((_tmp23B[1]=& _tmp239,Cyc_Tcutil_terr(open_exp->loc,((_tmp23C="expecting %s but found %s",_tag_dyneither(_tmp23C,sizeof(char),26))),_tag_dyneither(_tmp23B,sizeof(void*),2)))))))))))));}{
# 652
struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct _tmp23F;struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct*_tmp23E;(int)_throw((void*)((_tmp23E=_cycalloc_atomic(sizeof(*_tmp23E)),((_tmp23E[0]=((_tmp23F.tag=Cyc_Tcutil_AbortTypeCheckingFunction,_tmp23F)),_tmp23E)))));};}
# 655
Cyc_Tcenv_check_rgn_accessible(_tmpC9,open_exp->loc,rgn2_typ);
# 657
_tmp148=Cyc_Tcenv_add_region(decl_rgn,_tmp148,rgn_typ,0,1);};};}else{
# 659
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp242;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp241;rgn_typ=(void*)((_tmp241=_cycalloc(sizeof(*_tmp241)),((_tmp241[0]=((_tmp242.tag=2,((_tmp242.f1=_tmpD5,_tmp242)))),_tmp241))));}
{struct Cyc_List_List*_tmp243;_tmp148=Cyc_Tcenv_add_type_vars(decl_rgn,s0->loc,_tmp148,((_tmp243=_cycalloc(sizeof(*_tmp243)),((_tmp243->hd=_tmpD5,((_tmp243->tl=0,_tmp243)))))));}
_tmp148=Cyc_Tcenv_add_region(decl_rgn,_tmp148,rgn_typ,_tmpD7,1);}
# 663
Cyc_Tcutil_check_type(s0->loc,_tmpC9,Cyc_Tcenv_lookup_type_vars(_tmp148),& Cyc_Tcutil_bk,1,*_tmp14A);
{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp246;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp245;if(!Cyc_Tcutil_unify(*_tmp14A,(void*)((_tmp245=_cycalloc(sizeof(*_tmp245)),((_tmp245[0]=((_tmp246.tag=15,((_tmp246.f1=rgn_typ,_tmp246)))),_tmp245)))))){
const char*_tmp249;void*_tmp248;(_tmp248=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp249="region stmt: type of region handle is wrong!",_tag_dyneither(_tmp249,sizeof(char),45))),_tag_dyneither(_tmp248,sizeof(void*),0)));}}
# 668
if(!Cyc_Absyn_no_regions)
_tmp148=Cyc_Tcenv_enter_try(decl_rgn,_tmp148);
# 671
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,Cyc_Tcenv_add_local_var(decl_rgn,_tmp65->loc,_tmp148,_tmpD6),s0),_tmp65,0);
_npop_handler(0);return;};};};_LL46: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpD9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpD9->tag != 1)goto _LL48;else{_tmpDA=_tmpD9->f1;}}_LL47: {
# 720 "tcstmt.cyc"
void*_tmp163=Cyc_Tcenv_curr_rgn(_tmpC9);
if(_tmpDA->sc != Cyc_Absyn_Public){
const char*_tmp24C;void*_tmp24B;(_tmp24B=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp24C="bad storage class for inner function",_tag_dyneither(_tmp24C,sizeof(char),37))),_tag_dyneither(_tmp24B,sizeof(void*),0)));}
# 724
{union Cyc_Absyn_Nmspace _tmp166=(*_tmpDA->name).f1;union Cyc_Absyn_Nmspace _tmp167=_tmp166;struct Cyc_List_List*_tmp168;_LL97: if((_tmp167.Rel_n).tag != 1)goto _LL99;if((struct Cyc_List_List*)(_tmp167.Rel_n).val != 0)goto _LL99;_LL98:
 goto _LL96;_LL99: if((_tmp167.Abs_n).tag != 2)goto _LL9B;_tmp168=(struct Cyc_List_List*)(_tmp167.Abs_n).val;_LL9A: {
const char*_tmp24F;void*_tmp24E;(_tmp24E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp24F="tc: Abs_n in tcStmt var decl",_tag_dyneither(_tmp24F,sizeof(char),29))),_tag_dyneither(_tmp24E,sizeof(void*),0)));}_LL9B:;_LL9C: {
const char*_tmp252;void*_tmp251;(_tmp251=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp252="explicit namespace not allowed on inner function declaration",_tag_dyneither(_tmp252,sizeof(char),61))),_tag_dyneither(_tmp251,sizeof(void*),0)));}_LL96:;}
# 729
(*_tmpDA->name).f1=Cyc_Absyn_Loc_n;
Cyc_Tcutil_check_fndecl_valid_type(_tmp64->loc,_tmpC9,_tmpDA);{
void*t=Cyc_Tcutil_fndecl2typ(_tmpDA);
_tmpDA->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,_tmpDA->attributes);
# 734
{struct Cyc_List_List*atts=_tmpDA->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp16D=(void*)atts->hd;void*_tmp16E=_tmp16D;_LL9E: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp16F=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp16E;if(_tmp16F->tag != 7)goto _LLA0;}_LL9F:
 goto _LLA1;_LLA0: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp170=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp16E;if(_tmp170->tag != 6)goto _LLA2;}_LLA1:
{const char*_tmp256;void*_tmp255[1];struct Cyc_String_pa_PrintArg_struct _tmp254;(_tmp254.tag=0,((_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp255[0]=& _tmp254,Cyc_Tcutil_terr(_tmp64->loc,((_tmp256="bad attribute %s for function",_tag_dyneither(_tmp256,sizeof(char),30))),_tag_dyneither(_tmp255,sizeof(void*),1)))))));}
goto _LL9D;_LLA2:;_LLA3:
 goto _LL9D;_LL9D:;}}{
# 742
struct Cyc_Absyn_Vardecl*_tmp257;struct Cyc_Absyn_Vardecl*vd=(_tmp257=_cycalloc(sizeof(*_tmp257)),((_tmp257->sc=_tmpDA->sc,((_tmp257->name=_tmpDA->name,((_tmp257->tq=Cyc_Absyn_const_tqual(0),((_tmp257->type=
Cyc_Absyn_at_typ(t,_tmp163,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),((_tmp257->initializer=0,((_tmp257->rgn=_tmp163,((_tmp257->attributes=0,((_tmp257->escapes=0,_tmp257)))))))))))))))));
# 746
_tmpDA->fn_vardecl=vd;
_tmpC9=Cyc_Tcenv_add_local_var(decl_rgn,_tmp64->loc,_tmpC9,vd);{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpC9->le);
_tmpC9->le=Cyc_Tcenv_nested_fenv(_tmp64->loc,old_fenv,_tmpDA);
Cyc_Tcstmt_tcStmt(_tmpC9,_tmpDA->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpC9);
Cyc_Tcenv_check_delayed_constraints(_tmpC9);
if(!Cyc_Tcenv_all_labels_resolved(_tmpC9)){
const char*_tmp25A;void*_tmp259;(_tmp259=0,Cyc_Tcutil_terr(_tmp64->loc,((_tmp25A="function has goto statements to undefined labels",_tag_dyneither(_tmp25A,sizeof(char),49))),_tag_dyneither(_tmp259,sizeof(void*),0)));}
_tmpC9->le=old_fenv;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_set_encloser(decl_rgn,_tmpC9,s0),_tmp65,0);
_npop_handler(0);return;};};};}_LL48: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpDB=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpDB->tag != 9)goto _LL4A;else{_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;}}_LL49:
{const char*_tmp25B;unimp_msg_part=((_tmp25B="namespace",_tag_dyneither(_tmp25B,sizeof(char),10)));}goto _LL3D;_LL4A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpDE=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpDE->tag != 10)goto _LL4C;else{_tmpDF=_tmpDE->f1;_tmpE0=_tmpDE->f2;}}_LL4B:
{const char*_tmp25C;unimp_msg_part=((_tmp25C="using",_tag_dyneither(_tmp25C,sizeof(char),6)));}goto _LL3D;_LL4C: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE1->tag != 5)goto _LL4E;}_LL4D:
{const char*_tmp25D;unimp_msg_part=((_tmp25D="type",_tag_dyneither(_tmp25D,sizeof(char),5)));}goto _LL3D;_LL4E: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpE2=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE2->tag != 6)goto _LL50;}_LL4F:
{const char*_tmp25E;unimp_msg_part=((_tmp25E="datatype",_tag_dyneither(_tmp25E,sizeof(char),9)));}goto _LL3D;_LL50: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpE3=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE3->tag != 7)goto _LL52;}_LL51:
{const char*_tmp25F;unimp_msg_part=((_tmp25F="enum",_tag_dyneither(_tmp25F,sizeof(char),5)));}goto _LL3D;_LL52: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpE4=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE4->tag != 8)goto _LL54;}_LL53:
{const char*_tmp260;unimp_msg_part=((_tmp260="typedef",_tag_dyneither(_tmp260,sizeof(char),8)));}goto _LL3D;_LL54: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpE5=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE5->tag != 11)goto _LL56;}_LL55:
{const char*_tmp261;unimp_msg_part=((_tmp261="extern \"C\"",_tag_dyneither(_tmp261,sizeof(char),11)));}goto _LL3D;_LL56: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpE6=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE6->tag != 12)goto _LL58;}_LL57:
# 766
{const char*_tmp262;unimp_msg_part=((_tmp262="extern \"C include\"",_tag_dyneither(_tmp262,sizeof(char),19)));}goto _LL3D;_LL58: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpE7=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE7->tag != 13)goto _LL5A;}_LL59:
{const char*_tmp263;unimp_msg_part=((_tmp263="__cyclone_port_on__",_tag_dyneither(_tmp263,sizeof(char),20)));}goto _LL3D;_LL5A: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpE8=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpCB;if(_tmpE8->tag != 14)goto _LL3D;}_LL5B:
{const char*_tmp264;unimp_msg_part=((_tmp264="__cyclone_port_off__",_tag_dyneither(_tmp264,sizeof(char),21)));}goto _LL3D;_LL3D:;};}{
# 771
const char*_tmp268;void*_tmp267[1];struct Cyc_String_pa_PrintArg_struct _tmp266;(int)_throw(((_tmp266.tag=0,((_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part),((_tmp267[0]=& _tmp266,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp268="tcStmt: nested %s declarations unimplemented",_tag_dyneither(_tmp268,sizeof(char),45))),_tag_dyneither(_tmp267,sizeof(void*),1)))))))));};
# 449 "tcstmt.cyc"
;_pop_region(decl_rgn);}_LL2D: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp66=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp34;if(_tmp66->tag != 16)goto _LLC;else{_tmp67=_tmp66->f1;}}_LL2E: {
# 775 "tcstmt.cyc"
void*rgn_type=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp26B;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp26A;void*etype=(void*)((_tmp26A=_cycalloc(sizeof(*_tmp26A)),((_tmp26A[0]=((_tmp26B.tag=15,((_tmp26B.f1=rgn_type,_tmp26B)))),_tmp26A))));
if(!Cyc_Tcutil_unify(Cyc_Tcexp_tcExp(te,& etype,_tmp67),etype)){
const char*_tmp26F;void*_tmp26E[1];struct Cyc_String_pa_PrintArg_struct _tmp26D;(_tmp26D.tag=0,((_tmp26D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp67->topt))),((_tmp26E[0]=& _tmp26D,Cyc_Tcutil_terr(_tmp67->loc,((_tmp26F="expecting region_t but found %s",_tag_dyneither(_tmp26F,sizeof(char),32))),_tag_dyneither(_tmp26E,sizeof(void*),1)))))));}
# 780
Cyc_Tcenv_check_rgn_resetable(te,s0->loc,rgn_type);
return;}_LLC:;};}
