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
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
# 56
struct Cyc_Position_Error*Cyc_Position_mk_err_elab(unsigned int,struct _dyneither_ptr);extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};
# 62
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 251
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 286
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 333
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 352
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 364
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 537
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 927 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 929
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 944
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 949
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 955
void*Cyc_Absyn_compress_kb(void*);
# 960
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 965
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 967
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 971
extern void*Cyc_Absyn_empty_effect;
# 981
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 990
extern void*Cyc_Absyn_bounds_one;
# 995
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1083
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1123
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1170
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1172
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1174
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1176
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1178
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1182
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1184
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 66
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 123 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 128
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 132
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 137
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_local_var(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Vardecl*);
# 154
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 209
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 221
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void Cyc_Tcutil_flush_warnings();extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 53
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 58
void*Cyc_Tcutil_copy_type(void*t);
# 61
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
int Cyc_Tcutil_is_arithmetic_type(void*);
# 83
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 85
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 87
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 91
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 93
extern int Cyc_Tcutil_warn_alias_coerce;
# 96
extern int Cyc_Tcutil_warn_region_coerce;
# 99
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
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
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 134
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 144
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 148
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 153
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 155
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 159
void Cyc_Tcutil_explain_failure();
# 161
int Cyc_Tcutil_unify(void*,void*);
# 163
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 171
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 175
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 179
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 182
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 186
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 188
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 192
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 198
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 202
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*);
# 228 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 230
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 238
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,void*);
# 241
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 248
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 250
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 252
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 254
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 256
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 270
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 281
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 284
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 289
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 294
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 298
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
void*Cyc_Tcutil_normalize_effect(void*e);
# 309
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 311
int Cyc_Tcutil_new_tvar_id();
# 313
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 316
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 321
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 325
int Cyc_Tcutil_bits_only(void*t);
# 328
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 331
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 338
int Cyc_Tcutil_supports_default(void*);
# 342
int Cyc_Tcutil_admits_zero(void*t);
# 345
int Cyc_Tcutil_is_noreturn(void*);
# 349
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 353
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 357
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 360
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 32 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 39
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,struct Cyc_Tcenv_Genv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 71
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 85
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 103
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 122
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 47 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 49
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 53
int Cyc_Tcutil_warn_region_coerce=0;
# 56
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 61
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 65
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 70
{const char*_tmpE9F;if(Cyc_strcmp(((_tmpE9F="(qualifiers don't match)",_tag_dyneither(_tmpE9F,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpEA3;void*_tmpEA2[1];struct Cyc_String_pa_PrintArg_struct _tmpEA1;(_tmpEA1.tag=0,((_tmpEA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEA2[0]=& _tmpEA1,Cyc_fprintf(Cyc_stderr,((_tmpEA3="  %s\n",_tag_dyneither(_tmpEA3,sizeof(char),6))),_tag_dyneither(_tmpEA2,sizeof(void*),1)))))));}
return;}}
# 75
{const char*_tmpEA4;if(Cyc_strcmp(((_tmpEA4="(function effects do not match)",_tag_dyneither(_tmpEA4,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 80
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpEA8;void*_tmpEA7[1];struct Cyc_String_pa_PrintArg_struct _tmpEA6;(_tmpEA6.tag=0,((_tmpEA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpEA7[0]=& _tmpEA6,Cyc_fprintf(Cyc_stderr,((_tmpEA8="  %s",_tag_dyneither(_tmpEA8,sizeof(char),5))),_tag_dyneither(_tmpEA7,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpEAB;void*_tmpEAA;(_tmpEAA=0,Cyc_fprintf(Cyc_stderr,((_tmpEAB="\n\t",_tag_dyneither(_tmpEAB,sizeof(char),3))),_tag_dyneither(_tmpEAA,sizeof(void*),0)));}
pos=8;}else{
# 89
{const char*_tmpEAE;void*_tmpEAD;(_tmpEAD=0,Cyc_fprintf(Cyc_stderr,((_tmpEAE=" ",_tag_dyneither(_tmpEAE,sizeof(char),2))),_tag_dyneither(_tmpEAD,sizeof(void*),0)));}
++ pos;}
# 92
{const char*_tmpEB1;void*_tmpEB0;(_tmpEB0=0,Cyc_fprintf(Cyc_stderr,((_tmpEB1="and ",_tag_dyneither(_tmpEB1,sizeof(char),5))),_tag_dyneither(_tmpEB0,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpEB4;void*_tmpEB3;(_tmpEB3=0,Cyc_fprintf(Cyc_stderr,((_tmpEB4="\n\t",_tag_dyneither(_tmpEB4,sizeof(char),3))),_tag_dyneither(_tmpEB3,sizeof(void*),0)));}
pos=8;}
# 98
{const char*_tmpEB8;void*_tmpEB7[1];struct Cyc_String_pa_PrintArg_struct _tmpEB6;(_tmpEB6.tag=0,((_tmpEB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpEB7[0]=& _tmpEB6,Cyc_fprintf(Cyc_stderr,((_tmpEB8="%s ",_tag_dyneither(_tmpEB8,sizeof(char),4))),_tag_dyneither(_tmpEB7,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpEBB;void*_tmpEBA;(_tmpEBA=0,Cyc_fprintf(Cyc_stderr,((_tmpEBB="\n\t",_tag_dyneither(_tmpEBB,sizeof(char),3))),_tag_dyneither(_tmpEBA,sizeof(void*),0)));}
pos=8;}
# 104
{const char*_tmpEBE;void*_tmpEBD;(_tmpEBD=0,Cyc_fprintf(Cyc_stderr,((_tmpEBE="are not compatible. ",_tag_dyneither(_tmpEBE,sizeof(char),21))),_tag_dyneither(_tmpEBD,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpEC1;void*_tmpEC0;(_tmpEC0=0,Cyc_fprintf(Cyc_stderr,((_tmpEC1="\n\t",_tag_dyneither(_tmpEC1,sizeof(char),3))),_tag_dyneither(_tmpEC0,sizeof(void*),0)));}{
# 110
const char*_tmpEC5;void*_tmpEC4[1];struct Cyc_String_pa_PrintArg_struct _tmpEC3;(_tmpEC3.tag=0,((_tmpEC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpEC4[0]=& _tmpEC3,Cyc_fprintf(Cyc_stderr,((_tmpEC5="%s",_tag_dyneither(_tmpEC5,sizeof(char),3))),_tag_dyneither(_tmpEC4,sizeof(void*),1)))))));};}
# 112
{const char*_tmpEC8;void*_tmpEC7;(_tmpEC7=0,Cyc_fprintf(Cyc_stderr,((_tmpEC8="\n",_tag_dyneither(_tmpEC8,sizeof(char),2))),_tag_dyneither(_tmpEC7,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 116
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 119
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 122
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 125
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpECC;void*_tmpECB[1];struct Cyc_String_pa_PrintArg_struct _tmpECA;(_tmpECA.tag=0,((_tmpECA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpECB[0]=& _tmpECA,Cyc_fprintf(Cyc_stderr,((_tmpECC="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpECC,sizeof(char),36))),_tag_dyneither(_tmpECB,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpECF;struct Cyc_Core_Impossible_exn_struct*_tmpECE;(int)_throw((void*)((_tmpECE=_cycalloc(sizeof(*_tmpECE)),((_tmpECE[0]=((_tmpECF.tag=Cyc_Core_Impossible,((_tmpECF.f1=msg,_tmpECF)))),_tmpECE)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 133
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 137
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpED4;void*_tmpED3[2];struct Cyc_String_pa_PrintArg_struct _tmpED2;struct Cyc_String_pa_PrintArg_struct _tmpED1;(_tmpED1.tag=0,((_tmpED1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpED2.tag=0,((_tmpED2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpED3[0]=& _tmpED2,((_tmpED3[1]=& _tmpED1,Cyc_fprintf(Cyc_stderr,((_tmpED4="%s::%s ",_tag_dyneither(_tmpED4,sizeof(char),8))),_tag_dyneither(_tmpED3,sizeof(void*),2)))))))))))));}
{const char*_tmpED7;void*_tmpED6;(_tmpED6=0,Cyc_fprintf(Cyc_stderr,((_tmpED7="\n",_tag_dyneither(_tmpED7,sizeof(char),2))),_tag_dyneither(_tmpED6,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 144
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 149
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 152
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpED8;Cyc_Tcutil_warning_segs=((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8->hd=(void*)sg,((_tmpED8->tl=Cyc_Tcutil_warning_segs,_tmpED8))))));}{
struct _dyneither_ptr*_tmpEDB;struct Cyc_List_List*_tmpEDA;Cyc_Tcutil_warning_msgs=((_tmpEDA=_cycalloc(sizeof(*_tmpEDA)),((_tmpEDA->hd=((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=msg,_tmpEDB)))),((_tmpEDA->tl=Cyc_Tcutil_warning_msgs,_tmpEDA))))));};}
# 156
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpEDE;void*_tmpEDD;(_tmpEDD=0,Cyc_fprintf(Cyc_stderr,((_tmpEDE="***Warnings***\n",_tag_dyneither(_tmpEDE,sizeof(char),16))),_tag_dyneither(_tmpEDD,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpEE3;void*_tmpEE2[2];struct Cyc_String_pa_PrintArg_struct _tmpEE1;struct Cyc_String_pa_PrintArg_struct _tmpEE0;(_tmpEE0.tag=0,((_tmpEE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpEE1.tag=0,((_tmpEE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpEE2[0]=& _tmpEE1,((_tmpEE2[1]=& _tmpEE0,Cyc_fprintf(Cyc_stderr,((_tmpEE3="%s: %s\n",_tag_dyneither(_tmpEE3,sizeof(char),8))),_tag_dyneither(_tmpEE2,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 168
{const char*_tmpEE6;void*_tmpEE5;(_tmpEE5=0,Cyc_fprintf(Cyc_stderr,((_tmpEE6="**************\n",_tag_dyneither(_tmpEE6,sizeof(char),16))),_tag_dyneither(_tmpEE5,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 177
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 182
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void**_tmp3B;void**_tmp3D;struct Cyc_Absyn_Exp*_tmp3F;void*_tmp41;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 1)goto _LL3;else{if((void*)_tmp38->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp39->tag != 17)goto _LL5;else{if((void*)_tmp39->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp3A->tag != 17)goto _LL7;else{_tmp3B=(void**)((void**)& _tmp3A->f4);}}_LL6: {
# 187
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp3C->tag != 1)goto _LL9;else{_tmp3D=(void**)((void**)& _tmp3C->f2);}}_LL8: {
# 193
void*ta=(void*)_check_null(*_tmp3D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3D=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37;if(_tmp3E->tag != 18)goto _LLB;else{_tmp3F=_tmp3E->f1;}}_LLA:
# 199
 Cyc_Evexp_eval_const_uint_exp(_tmp3F);{
void*_tmp42=_tmp3F->r;void*_tmp43=_tmp42;void*_tmp45;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp44=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp43;if(_tmp44->tag != 38)goto _LL12;else{_tmp45=(void*)_tmp44->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp45);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37;if(_tmp40->tag != 26)goto _LLD;else{if(_tmp40->f2 == 0)goto _LLD;_tmp41=*_tmp40->f2;}}_LLC:
# 205
 return Cyc_Tcutil_compress(_tmp41);_LLD:;_LLE:
 return t;_LL0:;}
# 213
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 217
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp46=cptr;void*_tmp47;union Cyc_Absyn_Constraint*_tmp48;_LL15: if((_tmp46->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp46->Eq_constr).tag != 1)goto _LL19;_tmp47=(void*)(_tmp46->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp47);_LL19: if((_tmp46->Forward_constr).tag != 2)goto _LL14;_tmp48=(union Cyc_Absyn_Constraint*)(_tmp46->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp48);_LL14:;}
# 224
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp49=Cyc_Absyn_compress_kb(kb);void*_tmp4A=_tmp49;struct Cyc_Absyn_Kind*_tmp4D;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4B->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpEE9;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpEE8;return(void*)((_tmpEE8=_cycalloc(sizeof(*_tmpEE8)),((_tmpEE8[0]=((_tmpEE9.tag=1,((_tmpEE9.f1=0,_tmpEE9)))),_tmpEE8))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4C->tag != 2)goto _LL20;else{_tmp4D=_tmp4C->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpEEC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpEEB;return(void*)((_tmpEEB=_cycalloc(sizeof(*_tmpEEB)),((_tmpEEB[0]=((_tmpEEC.tag=2,((_tmpEEC.f1=0,((_tmpEEC.f2=_tmp4D,_tmpEEC)))))),_tmpEEB))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 231
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 233
struct Cyc_Absyn_Tvar*_tmpEED;return(_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED->name=tv->name,((_tmpEED->identity=- 1,((_tmpEED->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpEED)))))));}
# 235
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 237
struct _dyneither_ptr*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;struct _tuple10*_tmp53=arg;_tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_tmp56=_tmp53->f3;{
struct _tuple10*_tmpEEE;return(_tmpEEE=_cycalloc(sizeof(*_tmpEEE)),((_tmpEEE->f1=_tmp54,((_tmpEEE->f2=_tmp55,((_tmpEEE->f3=Cyc_Tcutil_copy_type(_tmp56),_tmpEEE)))))));};}
# 240
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple12*_tmp58=arg;_tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;{
struct _tuple12*_tmpEEF;return(_tmpEEF=_cycalloc(sizeof(*_tmpEEF)),((_tmpEEF->f1=_tmp59,((_tmpEEF->f2=Cyc_Tcutil_copy_type(_tmp5A),_tmpEEF)))));};}
# 244
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpEF0;return(_tmpEF0=_cycalloc(sizeof(*_tmpEF0)),((_tmpEF0->name=f->name,((_tmpEF0->tq=f->tq,((_tmpEF0->type=Cyc_Tcutil_copy_type(f->type),((_tmpEF0->width=f->width,((_tmpEF0->attributes=f->attributes,((_tmpEF0->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause),_tmpEF0)))))))))))));}
# 250
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5E;void*_tmp5F;struct _tuple0*_tmp5D=x;_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;{
struct _tuple0*_tmpEF1;return(_tmpEF1=_cycalloc(sizeof(*_tmpEF1)),((_tmpEF1->f1=Cyc_Tcutil_copy_type(_tmp5E),((_tmpEF1->f2=Cyc_Tcutil_copy_type(_tmp5F),_tmpEF1)))));};}
# 254
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpEF2;return(_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2->name=f->name,((_tmpEF2->tag=f->tag,((_tmpEF2->loc=f->loc,_tmpEF2)))))));}
# 257
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEF5;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEF4;return(void*)((_tmpEF4=_cycalloc(sizeof(*_tmpEF4)),((_tmpEF4[0]=((_tmpEF5.tag=2,((_tmpEF5.f1=Cyc_Tcutil_copy_tvar(t),_tmpEF5)))),_tmpEF4))));}
# 261
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp64=Cyc_Tcutil_compress(t);void*_tmp65=_tmp64;struct Cyc_Absyn_Tvar*_tmp69;union Cyc_Absyn_DatatypeInfoU _tmp6B;struct Cyc_List_List*_tmp6C;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;struct Cyc_Absyn_Exp*_tmp7D;union Cyc_Absyn_Constraint*_tmp7E;unsigned int _tmp7F;struct Cyc_List_List*_tmp81;void*_tmp82;struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;struct Cyc_List_List*_tmp85;int _tmp86;struct Cyc_Absyn_VarargInfo*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_List_List*_tmp8D;struct Cyc_List_List*_tmp8F;enum Cyc_Absyn_AggrKind _tmp91;struct _tuple2*_tmp92;struct Cyc_Core_Opt*_tmp93;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Aggrdecl**_tmp96;struct Cyc_List_List*_tmp97;enum Cyc_Absyn_AggrKind _tmp99;struct Cyc_List_List*_tmp9A;struct _tuple2*_tmp9C;struct Cyc_Absyn_Enumdecl*_tmp9D;struct Cyc_List_List*_tmp9F;void*_tmpA1;struct Cyc_Absyn_Exp*_tmpA3;void*_tmpA5;void*_tmpA7;void*_tmpA8;struct _tuple2*_tmpAA;struct Cyc_List_List*_tmpAB;struct Cyc_Absyn_Typedefdecl*_tmpAC;void*_tmpB1;struct Cyc_List_List*_tmpB3;void*_tmpB5;struct Cyc_Absyn_Aggrdecl*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpBB;struct Cyc_Absyn_Datatypedecl*_tmpBE;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65;if(_tmp66->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp65;if(_tmp67->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65;if(_tmp68->tag != 2)goto _LL29;else{_tmp69=_tmp68->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpEF8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpEF7;return(void*)((_tmpEF7=_cycalloc(sizeof(*_tmpEF7)),((_tmpEF7[0]=((_tmpEF8.tag=2,((_tmpEF8.f1=Cyc_Tcutil_copy_tvar(_tmp69),_tmpEF8)))),_tmpEF7))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65;if(_tmp6A->tag != 3)goto _LL2B;else{_tmp6B=(_tmp6A->f1).datatype_info;_tmp6C=(_tmp6A->f1).targs;}}_LL2A: {
# 267
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEFE;struct Cyc_Absyn_DatatypeInfo _tmpEFD;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEFC;return(void*)((_tmpEFC=_cycalloc(sizeof(*_tmpEFC)),((_tmpEFC[0]=((_tmpEFE.tag=3,((_tmpEFE.f1=((_tmpEFD.datatype_info=_tmp6B,((_tmpEFD.targs=Cyc_Tcutil_copy_types(_tmp6C),_tmpEFD)))),_tmpEFE)))),_tmpEFC))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65;if(_tmp6D->tag != 4)goto _LL2D;else{_tmp6E=(_tmp6D->f1).field_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2C: {
# 269
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpF04;struct Cyc_Absyn_DatatypeFieldInfo _tmpF03;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpF02;return(void*)((_tmpF02=_cycalloc(sizeof(*_tmpF02)),((_tmpF02[0]=((_tmpF04.tag=4,((_tmpF04.f1=((_tmpF03.field_info=_tmp6E,((_tmpF03.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpF03)))),_tmpF04)))),_tmpF02))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65;if(_tmp70->tag != 5)goto _LL2F;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2E: {
# 271
void*_tmpC7=Cyc_Tcutil_copy_type(_tmp71);
void*_tmpC8=Cyc_Tcutil_copy_type(_tmp73);
union Cyc_Absyn_Constraint*_tmpC9=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp74);
struct Cyc_Absyn_Tqual _tmpCA=_tmp72;
# 276
union Cyc_Absyn_Constraint*_tmpCB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp75);
union Cyc_Absyn_Constraint*_tmpCC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp76);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpF0A;struct Cyc_Absyn_PtrInfo _tmpF09;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpF08;return(void*)((_tmpF08=_cycalloc(sizeof(*_tmpF08)),((_tmpF08[0]=((_tmpF0A.tag=5,((_tmpF0A.f1=((_tmpF09.elt_typ=_tmpC7,((_tmpF09.elt_tq=_tmpCA,((_tmpF09.ptr_atts=(((_tmpF09.ptr_atts).rgn=_tmpC8,(((_tmpF09.ptr_atts).nullable=_tmpC9,(((_tmpF09.ptr_atts).bounds=_tmpCB,(((_tmpF09.ptr_atts).zero_term=_tmpCC,(((_tmpF09.ptr_atts).ptrloc=_tmp77,_tmpF09.ptr_atts)))))))))),_tmpF09)))))),_tmpF0A)))),_tmpF08))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp78->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65;if(_tmp79->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65;if(_tmp7A->tag != 8)goto _LL35;else{_tmp7B=(_tmp7A->f1).elt_type;_tmp7C=(_tmp7A->f1).tq;_tmp7D=(_tmp7A->f1).num_elts;_tmp7E=(_tmp7A->f1).zero_term;_tmp7F=(_tmp7A->f1).zt_loc;}}_LL34: {
# 282
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp7D);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpF10;struct Cyc_Absyn_ArrayInfo _tmpF0F;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpF0E;return(void*)((_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E[0]=((_tmpF10.tag=8,((_tmpF10.f1=((_tmpF0F.elt_type=Cyc_Tcutil_copy_type(_tmp7B),((_tmpF0F.tq=_tmp7C,((_tmpF0F.num_elts=eopt2,((_tmpF0F.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7E),((_tmpF0F.zt_loc=_tmp7F,_tmpF0F)))))))))),_tmpF10)))),_tmpF0E))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65;if(_tmp80->tag != 9)goto _LL37;else{_tmp81=(_tmp80->f1).tvars;_tmp82=(_tmp80->f1).effect;_tmp83=(_tmp80->f1).ret_tqual;_tmp84=(_tmp80->f1).ret_typ;_tmp85=(_tmp80->f1).args;_tmp86=(_tmp80->f1).c_varargs;_tmp87=(_tmp80->f1).cyc_varargs;_tmp88=(_tmp80->f1).rgn_po;_tmp89=(_tmp80->f1).attributes;_tmp8A=(_tmp80->f1).requires_clause;_tmp8B=(_tmp80->f1).requires_relns;_tmp8C=(_tmp80->f1).ensures_clause;_tmp8D=(_tmp80->f1).ensures_relns;}}_LL36: {
# 286
struct Cyc_List_List*_tmpD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp81);
void*effopt2=_tmp82 == 0?0: Cyc_Tcutil_copy_type(_tmp82);
void*_tmpD4=Cyc_Tcutil_copy_type(_tmp84);
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp85);
int _tmpD6=_tmp86;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp87 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp87;
struct Cyc_Absyn_VarargInfo*_tmpF11;cyc_varargs2=((_tmpF11=_cycalloc(sizeof(*_tmpF11)),((_tmpF11->name=cv->name,((_tmpF11->tq=cv->tq,((_tmpF11->type=Cyc_Tcutil_copy_type(cv->type),((_tmpF11->inject=cv->inject,_tmpF11))))))))));}{
# 297
struct Cyc_List_List*_tmpD8=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp88);
struct Cyc_List_List*_tmpD9=_tmp89;
struct Cyc_Absyn_Exp*_tmpDA=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8A);
struct Cyc_List_List*_tmpDB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8B);
struct Cyc_Absyn_Exp*_tmpDC=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp8C);
struct Cyc_List_List*_tmpDD=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp8D);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpF17;struct Cyc_Absyn_FnInfo _tmpF16;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpF15;return(void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF17.tag=9,((_tmpF17.f1=((_tmpF16.tvars=_tmpD3,((_tmpF16.effect=effopt2,((_tmpF16.ret_tqual=_tmp83,((_tmpF16.ret_typ=_tmpD4,((_tmpF16.args=_tmpD5,((_tmpF16.c_varargs=_tmpD6,((_tmpF16.cyc_varargs=cyc_varargs2,((_tmpF16.rgn_po=_tmpD8,((_tmpF16.attributes=_tmpD9,((_tmpF16.requires_clause=_tmpDA,((_tmpF16.requires_relns=_tmpDB,((_tmpF16.ensures_clause=_tmpDC,((_tmpF16.ensures_relns=_tmpDD,_tmpF16)))))))))))))))))))))))))),_tmpF17)))),_tmpF15))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65;if(_tmp8E->tag != 10)goto _LL39;else{_tmp8F=_tmp8E->f1;}}_LL38: {
# 306
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpF1A;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpF19;return(void*)((_tmpF19=_cycalloc(sizeof(*_tmpF19)),((_tmpF19[0]=((_tmpF1A.tag=10,((_tmpF1A.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8F),_tmpF1A)))),_tmpF19))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp90->tag != 11)goto _LL3B;else{if((((_tmp90->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp91=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f1;_tmp92=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f2;_tmp93=((struct _tuple4)(((_tmp90->f1).aggr_info).UnknownAggr).val).f3;_tmp94=(_tmp90->f1).targs;}}_LL3A: {
# 308
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF20;struct Cyc_Absyn_AggrInfo _tmpF1F;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF1E;return(void*)((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E[0]=((_tmpF20.tag=11,((_tmpF20.f1=((_tmpF1F.aggr_info=Cyc_Absyn_UnknownAggr(_tmp91,_tmp92,_tmp93),((_tmpF1F.targs=Cyc_Tcutil_copy_types(_tmp94),_tmpF1F)))),_tmpF20)))),_tmpF1E))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp95->tag != 11)goto _LL3D;else{if((((_tmp95->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp96=(struct Cyc_Absyn_Aggrdecl**)(((_tmp95->f1).aggr_info).KnownAggr).val;_tmp97=(_tmp95->f1).targs;}}_LL3C: {
# 310
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF26;struct Cyc_Absyn_AggrInfo _tmpF25;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF24;return(void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF26.tag=11,((_tmpF26.f1=((_tmpF25.aggr_info=Cyc_Absyn_KnownAggr(_tmp96),((_tmpF25.targs=Cyc_Tcutil_copy_types(_tmp97),_tmpF25)))),_tmpF26)))),_tmpF24))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65;if(_tmp98->tag != 12)goto _LL3F;else{_tmp99=_tmp98->f1;_tmp9A=_tmp98->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpF29;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpF28;return(void*)((_tmpF28=_cycalloc(sizeof(*_tmpF28)),((_tmpF28[0]=((_tmpF29.tag=12,((_tmpF29.f1=_tmp99,((_tmpF29.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp9A),_tmpF29)))))),_tmpF28))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp65;if(_tmp9B->tag != 13)goto _LL41;else{_tmp9C=_tmp9B->f1;_tmp9D=_tmp9B->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF2C;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF2B;return(void*)((_tmpF2B=_cycalloc(sizeof(*_tmpF2B)),((_tmpF2B[0]=((_tmpF2C.tag=13,((_tmpF2C.f1=_tmp9C,((_tmpF2C.f2=_tmp9D,_tmpF2C)))))),_tmpF2B))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp65;if(_tmp9E->tag != 14)goto _LL43;else{_tmp9F=_tmp9E->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpF2F;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpF2E;return(void*)((_tmpF2E=_cycalloc(sizeof(*_tmpF2E)),((_tmpF2E[0]=((_tmpF2F.tag=14,((_tmpF2F.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9F),_tmpF2F)))),_tmpF2E))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65;if(_tmpA0->tag != 19)goto _LL45;else{_tmpA1=(void*)_tmpA0->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpF32;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpF31;return(void*)((_tmpF31=_cycalloc(sizeof(*_tmpF31)),((_tmpF31[0]=((_tmpF32.tag=19,((_tmpF32.f1=Cyc_Tcutil_copy_type(_tmpA1),_tmpF32)))),_tmpF31))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65;if(_tmpA2->tag != 18)goto _LL47;else{_tmpA3=_tmpA2->f1;}}_LL46: {
# 317
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF35;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF34;return(void*)((_tmpF34=_cycalloc(sizeof(*_tmpF34)),((_tmpF34[0]=((_tmpF35.tag=18,((_tmpF35.f1=_tmpA3,_tmpF35)))),_tmpF34))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65;if(_tmpA4->tag != 15)goto _LL49;else{_tmpA5=(void*)_tmpA4->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpF38;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpF37;return(void*)((_tmpF37=_cycalloc(sizeof(*_tmpF37)),((_tmpF37[0]=((_tmpF38.tag=15,((_tmpF38.f1=Cyc_Tcutil_copy_type(_tmpA5),_tmpF38)))),_tmpF37))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA6=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65;if(_tmpA6->tag != 16)goto _LL4B;else{_tmpA7=(void*)_tmpA6->f1;_tmpA8=(void*)_tmpA6->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpF3B;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpF3A;return(void*)((_tmpF3A=_cycalloc(sizeof(*_tmpF3A)),((_tmpF3A[0]=((_tmpF3B.tag=16,((_tmpF3B.f1=Cyc_Tcutil_copy_type(_tmpA7),((_tmpF3B.f2=Cyc_Tcutil_copy_type(_tmpA8),_tmpF3B)))))),_tmpF3A))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65;if(_tmpA9->tag != 17)goto _LL4D;else{_tmpAA=_tmpA9->f1;_tmpAB=_tmpA9->f2;_tmpAC=_tmpA9->f3;}}_LL4C: {
# 321
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpF3E;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpF3D;return(void*)((_tmpF3D=_cycalloc(sizeof(*_tmpF3D)),((_tmpF3D[0]=((_tmpF3E.tag=17,((_tmpF3E.f1=_tmpAA,((_tmpF3E.f2=Cyc_Tcutil_copy_types(_tmpAB),((_tmpF3E.f3=_tmpAC,((_tmpF3E.f4=0,_tmpF3E)))))))))),_tmpF3D))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAD=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp65;if(_tmpAD->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp65;if(_tmpAE->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65;if(_tmpAF->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp65;if(_tmpB0->tag != 23)goto _LL55;else{_tmpB1=(void*)_tmpB0->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF41;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF40;return(void*)((_tmpF40=_cycalloc(sizeof(*_tmpF40)),((_tmpF40[0]=((_tmpF41.tag=23,((_tmpF41.f1=Cyc_Tcutil_copy_type(_tmpB1),_tmpF41)))),_tmpF40))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65;if(_tmpB2->tag != 24)goto _LL57;else{_tmpB3=_tmpB2->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF44;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF43;return(void*)((_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43[0]=((_tmpF44.tag=24,((_tmpF44.f1=Cyc_Tcutil_copy_types(_tmpB3),_tmpF44)))),_tmpF43))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65;if(_tmpB4->tag != 25)goto _LL59;else{_tmpB5=(void*)_tmpB4->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF47;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF46;return(void*)((_tmpF46=_cycalloc(sizeof(*_tmpF46)),((_tmpF46[0]=((_tmpF47.tag=25,((_tmpF47.f1=Cyc_Tcutil_copy_type(_tmpB5),_tmpF47)))),_tmpF46))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB6->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB7=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB6->f1)->r;if(_tmpB7->tag != 0)goto _LL5B;else{_tmpB8=_tmpB7->f1;}}}_LL5A: {
# 330
struct Cyc_List_List*_tmpFF=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB8->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpF4D;struct Cyc_Absyn_AggrInfo _tmpF4C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF4B;return(void*)((_tmpF4B=_cycalloc(sizeof(*_tmpF4B)),((_tmpF4B[0]=((_tmpF4D.tag=11,((_tmpF4D.f1=((_tmpF4C.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB8->kind,_tmpB8->name,0),((_tmpF4C.targs=_tmpFF,_tmpF4C)))),_tmpF4D)))),_tmpF4B))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB9->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpBA=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB9->f1)->r;if(_tmpBA->tag != 1)goto _LL5D;else{_tmpBB=_tmpBA->f1;}}}_LL5C: {
# 333
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpF50;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpF4F;return(void*)((_tmpF4F=_cycalloc(sizeof(*_tmpF4F)),((_tmpF4F[0]=((_tmpF50.tag=13,((_tmpF50.f1=_tmpBB->name,((_tmpF50.f2=0,_tmpF50)))))),_tmpF4F))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpBC->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpBD=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpBC->f1)->r;if(_tmpBD->tag != 2)goto _LL22;else{_tmpBE=_tmpBD->f1;}}}_LL5E: {
# 335
struct Cyc_List_List*_tmp105=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBE->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF5A;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF59;struct Cyc_Absyn_DatatypeInfo _tmpF58;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpF57;return(void*)((_tmpF57=_cycalloc(sizeof(*_tmpF57)),((_tmpF57[0]=((_tmpF5A.tag=3,((_tmpF5A.f1=((_tmpF58.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpF59.name=_tmpBE->name,((_tmpF59.is_extensible=0,_tmpF59))))),((_tmpF58.targs=_tmp105,_tmpF58)))),_tmpF5A)))),_tmpF57))));}_LL22:;}
# 350 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp10A=d;struct Cyc_Absyn_Exp*_tmp10C;struct _dyneither_ptr*_tmp10E;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp10B=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp10A;if(_tmp10B->tag != 0)goto _LL62;else{_tmp10C=_tmp10B->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpF5D;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpF5C;return(void*)((_tmpF5C=_cycalloc(sizeof(*_tmpF5C)),((_tmpF5C[0]=((_tmpF5D.tag=0,((_tmpF5D.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp10C),_tmpF5D)))),_tmpF5C))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp10D=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp10A;if(_tmp10D->tag != 1)goto _LL5F;else{_tmp10E=_tmp10D->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 356
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 358
struct _tuple19*_tmpF5E;return(_tmpF5E=_cycalloc(sizeof(*_tmpF5E)),((_tmpF5E->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpF5E->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpF5E)))));}
# 361
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp112=preserve_types;
{void*_tmp113=e->r;void*_tmp114=_tmp113;union Cyc_Absyn_Cnst _tmp116;struct _tuple2*_tmp118;void*_tmp119;enum Cyc_Absyn_Primop _tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Core_Opt*_tmp11F;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp122;enum Cyc_Absyn_Incrementor _tmp123;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_VarargCallInfo*_tmp134;int _tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_List_List*_tmp13C;void*_tmp13E;struct Cyc_Absyn_Exp*_tmp13F;int _tmp140;enum Cyc_Absyn_Coercion _tmp141;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;void*_tmp148;struct Cyc_Absyn_Exp*_tmp14A;void*_tmp14C;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct _dyneither_ptr*_tmp152;int _tmp153;int _tmp154;struct Cyc_Absyn_Exp*_tmp156;struct _dyneither_ptr*_tmp157;int _tmp158;int _tmp159;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_List_List*_tmp15E;struct _dyneither_ptr*_tmp160;struct Cyc_Absyn_Tqual _tmp161;void*_tmp162;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp165;struct Cyc_Absyn_Vardecl*_tmp167;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp169;int _tmp16A;struct Cyc_Absyn_Exp*_tmp16C;void*_tmp16D;int _tmp16E;struct _tuple2*_tmp170;struct Cyc_List_List*_tmp171;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Aggrdecl*_tmp173;void*_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp178;struct Cyc_Absyn_Datatypedecl*_tmp179;struct Cyc_Absyn_Datatypefield*_tmp17A;struct _tuple2*_tmp17C;struct Cyc_Absyn_Enumdecl*_tmp17D;struct Cyc_Absyn_Enumfield*_tmp17E;struct _tuple2*_tmp180;void*_tmp181;struct Cyc_Absyn_Enumfield*_tmp182;int _tmp184;struct Cyc_Absyn_Exp*_tmp185;void**_tmp186;struct Cyc_Absyn_Exp*_tmp187;int _tmp188;struct Cyc_Absyn_Exp*_tmp18A;struct Cyc_Absyn_Exp*_tmp18B;struct Cyc_Core_Opt*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_Absyn_Exp*_tmp191;struct _dyneither_ptr*_tmp192;void*_tmp194;int _tmp196;struct _dyneither_ptr _tmp197;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp115->tag != 0)goto _LL67;else{_tmp116=_tmp115->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp116,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp117->tag != 1)goto _LL69;else{_tmp118=_tmp117->f1;_tmp119=(void*)_tmp117->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp118,_tmp119,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp11A->tag != 2)goto _LL6B;else{_tmp11B=_tmp11A->f1;_tmp11C=_tmp11A->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp11B,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp11C),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp11D->tag != 3)goto _LL6D;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;_tmp120=_tmp11D->f3;}}_LL6C:
# 369
{struct Cyc_Core_Opt*_tmpF5F;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp11E),(unsigned int)_tmp11F?(_tmpF5F=_cycalloc_atomic(sizeof(*_tmpF5F)),((_tmpF5F->v=(void*)((enum Cyc_Absyn_Primop)_tmp11F->v),_tmpF5F))): 0,Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp120),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp121->tag != 4)goto _LL6F;else{_tmp122=_tmp121->f1;_tmp123=_tmp121->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp122),_tmp123,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp124->tag != 5)goto _LL71;else{_tmp125=_tmp124->f1;_tmp126=_tmp124->f2;_tmp127=_tmp124->f3;}}_LL70:
# 373
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp125),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp126),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp127),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp128->tag != 6)goto _LL73;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp129),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12A),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp12B->tag != 7)goto _LL75;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12C),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12D),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp12E->tag != 8)goto _LL77;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp12F),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp130),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp131->tag != 9)goto _LL79;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;_tmp134=_tmp131->f3;_tmp135=_tmp131->f4;}}_LL78:
# 378
{struct Cyc_Absyn_VarargCallInfo*_tmp199=_tmp134;int _tmp19A;struct Cyc_List_List*_tmp19B;struct Cyc_Absyn_VarargInfo*_tmp19C;_LLB6: if(_tmp199 == 0)goto _LLB8;_tmp19A=_tmp199->num_varargs;_tmp19B=_tmp199->injectors;_tmp19C=_tmp199->vai;_LLB7: {
# 380
struct _dyneither_ptr*_tmp19E;struct Cyc_Absyn_Tqual _tmp19F;void*_tmp1A0;int _tmp1A1;struct Cyc_Absyn_VarargInfo*_tmp19D=_tmp19C;_tmp19E=_tmp19D->name;_tmp19F=_tmp19D->tq;_tmp1A0=_tmp19D->type;_tmp1A1=_tmp19D->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF69;struct Cyc_Absyn_VarargInfo*_tmpF68;struct Cyc_Absyn_VarargCallInfo*_tmpF67;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF66;new_e=Cyc_Absyn_new_exp((void*)((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66[0]=((_tmpF69.tag=9,((_tmpF69.f1=
Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp132),((_tmpF69.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp133),((_tmpF69.f3=(
(_tmpF67=_cycalloc(sizeof(*_tmpF67)),((_tmpF67->num_varargs=_tmp19A,((_tmpF67->injectors=_tmp19B,((_tmpF67->vai=(
(_tmpF68=_cycalloc(sizeof(*_tmpF68)),((_tmpF68->name=_tmp19E,((_tmpF68->tq=_tmp19F,((_tmpF68->type=Cyc_Tcutil_copy_type(_tmp1A0),((_tmpF68->inject=_tmp1A1,_tmpF68)))))))))),_tmpF67)))))))),((_tmpF69.f4=_tmp135,_tmpF69)))))))))),_tmpF66)))),e->loc);}
# 386
goto _LLB5;}_LLB8:;_LLB9:
# 388
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF6C;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF6B;new_e=Cyc_Absyn_new_exp((void*)((_tmpF6B=_cycalloc(sizeof(*_tmpF6B)),((_tmpF6B[0]=((_tmpF6C.tag=9,((_tmpF6C.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp132),((_tmpF6C.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp133),((_tmpF6C.f3=_tmp134,((_tmpF6C.f4=_tmp135,_tmpF6C)))))))))),_tmpF6B)))),e->loc);}
goto _LLB5;_LLB5:;}
# 391
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp136->tag != 10)goto _LL7B;else{_tmp137=_tmp136->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp137),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp138->tag != 11)goto _LL7D;else{_tmp139=_tmp138->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp139),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp13A->tag != 12)goto _LL7F;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;}}_LL7E:
# 396
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp13B),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp13C),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp13D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp13D->tag != 13)goto _LL81;else{_tmp13E=(void*)_tmp13D->f1;_tmp13F=_tmp13D->f2;_tmp140=_tmp13D->f3;_tmp141=_tmp13D->f4;}}_LL80:
# 399
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp13F),_tmp140,_tmp141,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp142=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp142->tag != 14)goto _LL83;else{_tmp143=_tmp142->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp143),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp144->tag != 15)goto _LL85;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}}_LL84: {
# 402
struct Cyc_Absyn_Exp*eo1=_tmp145;if(_tmp145 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp145);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp146),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp147->tag != 16)goto _LL87;else{_tmp148=(void*)_tmp147->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp148),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp149=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp149->tag != 17)goto _LL89;else{_tmp14A=_tmp149->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp14A),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp14B->tag != 18)goto _LL8B;else{_tmp14C=(void*)_tmp14B->f1;_tmp14D=_tmp14B->f2;}}_LL8A:
# 409
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp14C),_tmp14D,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp14E->tag != 19)goto _LL8D;else{_tmp14F=_tmp14E->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp14F),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp150->tag != 20)goto _LL8F;else{_tmp151=_tmp150->f1;_tmp152=_tmp150->f2;_tmp153=_tmp150->f3;_tmp154=_tmp150->f4;}}_LL8E:
# 412
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF6F;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF6E;new_e=Cyc_Absyn_new_exp((void*)((_tmpF6E=_cycalloc(sizeof(*_tmpF6E)),((_tmpF6E[0]=((_tmpF6F.tag=20,((_tmpF6F.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp151),((_tmpF6F.f2=_tmp152,((_tmpF6F.f3=_tmp153,((_tmpF6F.f4=_tmp154,_tmpF6F)))))))))),_tmpF6E)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp155->tag != 21)goto _LL91;else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;_tmp158=_tmp155->f3;_tmp159=_tmp155->f4;}}_LL90:
# 414
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF72;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF71;new_e=Cyc_Absyn_new_exp((void*)((_tmpF71=_cycalloc(sizeof(*_tmpF71)),((_tmpF71[0]=((_tmpF72.tag=21,((_tmpF72.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp156),((_tmpF72.f2=_tmp157,((_tmpF72.f3=_tmp158,((_tmpF72.f4=_tmp159,_tmpF72)))))))))),_tmpF71)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp15A->tag != 22)goto _LL93;else{_tmp15B=_tmp15A->f1;_tmp15C=_tmp15A->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp15B),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp15C),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp15D->tag != 23)goto _LL95;else{_tmp15E=_tmp15D->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp15E),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp15F->tag != 24)goto _LL97;else{_tmp160=(_tmp15F->f1)->f1;_tmp161=(_tmp15F->f1)->f2;_tmp162=(_tmp15F->f1)->f3;_tmp163=_tmp15F->f2;}}_LL96: {
# 419
struct _dyneither_ptr*vopt1=_tmp160;
if(_tmp160 != 0)vopt1=_tmp160;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF78;struct _tuple10*_tmpF77;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF76;new_e=Cyc_Absyn_new_exp((void*)((_tmpF76=_cycalloc(sizeof(*_tmpF76)),((_tmpF76[0]=((_tmpF78.tag=24,((_tmpF78.f1=((_tmpF77=_cycalloc(sizeof(*_tmpF77)),((_tmpF77->f1=vopt1,((_tmpF77->f2=_tmp161,((_tmpF77->f3=Cyc_Tcutil_copy_type(_tmp162),_tmpF77)))))))),((_tmpF78.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp163),_tmpF78)))))),_tmpF76)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp164->tag != 25)goto _LL99;else{_tmp165=_tmp164->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF7B;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF7A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7A=_cycalloc(sizeof(*_tmpF7A)),((_tmpF7A[0]=((_tmpF7B.tag=25,((_tmpF7B.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp165),_tmpF7B)))),_tmpF7A)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp166=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp166->tag != 26)goto _LL9B;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;_tmp169=_tmp166->f3;_tmp16A=_tmp166->f4;}}_LL9A:
# 427
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF7E;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF7D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((_tmpF7E.tag=26,((_tmpF7E.f1=_tmp167,((_tmpF7E.f2=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp168),((_tmpF7E.f3=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp169),((_tmpF7E.f4=_tmp16A,_tmpF7E)))))))))),_tmpF7D)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp16B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp16B->tag != 27)goto _LL9D;else{_tmp16C=_tmp16B->f1;_tmp16D=(void*)_tmp16B->f2;_tmp16E=_tmp16B->f3;}}_LL9C:
# 430
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF81;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF80;new_e=Cyc_Absyn_new_exp((void*)((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF81.tag=27,((_tmpF81.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp16C),((_tmpF81.f2=Cyc_Tcutil_copy_type(_tmp16D),((_tmpF81.f3=_tmp16E,_tmpF81)))))))),_tmpF80)))),_tmp16C->loc);}
# 432
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp16F->tag != 28)goto _LL9F;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;_tmp173=_tmp16F->f4;}}_LL9E:
# 434
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF84;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF83;new_e=Cyc_Absyn_new_exp((void*)((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF84.tag=28,((_tmpF84.f1=_tmp170,((_tmpF84.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp171),((_tmpF84.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp172),((_tmpF84.f4=_tmp173,_tmpF84)))))))))),_tmpF83)))),e->loc);}
# 436
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp174->tag != 29)goto _LLA1;else{_tmp175=(void*)_tmp174->f1;_tmp176=_tmp174->f2;}}_LLA0:
# 438
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpF87;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF86;new_e=Cyc_Absyn_new_exp((void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((_tmpF87.tag=29,((_tmpF87.f1=Cyc_Tcutil_copy_type(_tmp175),((_tmpF87.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp176),_tmpF87)))))),_tmpF86)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp177->tag != 30)goto _LLA3;else{_tmp178=_tmp177->f1;_tmp179=_tmp177->f2;_tmp17A=_tmp177->f3;}}_LLA2:
# 441
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpF8A;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF89;new_e=Cyc_Absyn_new_exp((void*)((_tmpF89=_cycalloc(sizeof(*_tmpF89)),((_tmpF89[0]=((_tmpF8A.tag=30,((_tmpF8A.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp112,_tmp178),((_tmpF8A.f2=_tmp179,((_tmpF8A.f3=_tmp17A,_tmpF8A)))))))),_tmpF89)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp17B->tag != 31)goto _LLA5;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp17F->tag != 32)goto _LLA7;else{_tmp180=_tmp17F->f1;_tmp181=(void*)_tmp17F->f2;_tmp182=_tmp17F->f3;}}_LLA6:
# 445
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpF8D;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF8C;new_e=Cyc_Absyn_new_exp((void*)((_tmpF8C=_cycalloc(sizeof(*_tmpF8C)),((_tmpF8C[0]=((_tmpF8D.tag=32,((_tmpF8D.f1=_tmp180,((_tmpF8D.f2=Cyc_Tcutil_copy_type(_tmp181),((_tmpF8D.f3=_tmp182,_tmpF8D)))))))),_tmpF8C)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp183->tag != 33)goto _LLA9;else{_tmp184=(_tmp183->f1).is_calloc;_tmp185=(_tmp183->f1).rgn;_tmp186=(_tmp183->f1).elt_type;_tmp187=(_tmp183->f1).num_elts;_tmp188=(_tmp183->f1).fat_result;}}_LLA8: {
# 448
struct Cyc_Absyn_Exp*_tmp1BD=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp185;if(_tmp185 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp185);{
void**t1=_tmp186;if(_tmp186 != 0){void**_tmpF8E;t1=((_tmpF8E=_cycalloc(sizeof(*_tmpF8E)),((_tmpF8E[0]=Cyc_Tcutil_copy_type(*_tmp186),_tmpF8E))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF94;struct Cyc_Absyn_MallocInfo _tmpF93;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF92;_tmp1BD->r=(void*)((_tmpF92=_cycalloc(sizeof(*_tmpF92)),((_tmpF92[0]=((_tmpF94.tag=33,((_tmpF94.f1=((_tmpF93.is_calloc=_tmp184,((_tmpF93.rgn=r1,((_tmpF93.elt_type=t1,((_tmpF93.num_elts=_tmp187,((_tmpF93.fat_result=_tmp188,_tmpF93)))))))))),_tmpF94)))),_tmpF92))));}
new_e=_tmp1BD;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp189=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp189->tag != 34)goto _LLAB;else{_tmp18A=_tmp189->f1;_tmp18B=_tmp189->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp18A),Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp18B),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp18C->tag != 35)goto _LLAD;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;}}_LLAC: {
# 456
struct Cyc_Core_Opt*nopt1=_tmp18D;
if(_tmp18D != 0){struct Cyc_Core_Opt*_tmpF95;nopt1=((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((_tmpF95->v=(struct _tuple2*)_tmp18D->v,_tmpF95))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpF98;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF97;new_e=Cyc_Absyn_new_exp((void*)((_tmpF97=_cycalloc(sizeof(*_tmpF97)),((_tmpF97[0]=((_tmpF98.tag=35,((_tmpF98.f1=nopt1,((_tmpF98.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp112,_tmp18E),_tmpF98)))))),_tmpF97)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp18F->tag != 36)goto _LLAF;}_LLAE: {
# 461
struct Cyc_Core_Failure_exn_struct _tmpF9E;const char*_tmpF9D;struct Cyc_Core_Failure_exn_struct*_tmpF9C;(int)_throw((void*)((_tmpF9C=_cycalloc(sizeof(*_tmpF9C)),((_tmpF9C[0]=((_tmpF9E.tag=Cyc_Core_Failure,((_tmpF9E.f1=((_tmpF9D="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpF9D,sizeof(char),45))),_tmpF9E)))),_tmpF9C)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp190=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp190->tag != 37)goto _LLB1;else{_tmp191=_tmp190->f1;_tmp192=_tmp190->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpFA1;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpFA0;new_e=Cyc_Absyn_new_exp((void*)((_tmpFA0=_cycalloc(sizeof(*_tmpFA0)),((_tmpFA0[0]=((_tmpFA1.tag=37,((_tmpFA1.f1=Cyc_Tcutil_deep_copy_exp(_tmp112,_tmp191),((_tmpFA1.f2=_tmp192,_tmpFA1)))))),_tmpFA0)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp193->tag != 38)goto _LLB3;else{_tmp194=(void*)_tmp193->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp194),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp195=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp114;if(_tmp195->tag != 39)goto _LL64;else{_tmp196=_tmp195->f1;_tmp197=_tmp195->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp196,_tmp197,e->loc);goto _LL64;_LL64:;}
# 469
if(preserve_types)new_e->topt=e->topt;
return new_e;}
# 473
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 486 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1CB;enum Cyc_Absyn_AliasQual _tmp1CC;struct Cyc_Absyn_Kind*_tmp1CA=ka1;_tmp1CB=_tmp1CA->kind;_tmp1CC=_tmp1CA->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1CE;enum Cyc_Absyn_AliasQual _tmp1CF;struct Cyc_Absyn_Kind*_tmp1CD=ka2;_tmp1CE=_tmp1CD->kind;_tmp1CF=_tmp1CD->aliasqual;
# 490
if(_tmp1CB != _tmp1CE){
struct _tuple20 _tmpFA2;struct _tuple20 _tmp1D0=(_tmpFA2.f1=_tmp1CB,((_tmpFA2.f2=_tmp1CE,_tmpFA2)));struct _tuple20 _tmp1D1=_tmp1D0;_LLBB: if(_tmp1D1.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1D1.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1D1.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1D1.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1D1.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1D1.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 499
if(_tmp1CC != _tmp1CF){
struct _tuple21 _tmpFA3;struct _tuple21 _tmp1D3=(_tmpFA3.f1=_tmp1CC,((_tmpFA3.f2=_tmp1CF,_tmpFA3)));struct _tuple21 _tmp1D4=_tmp1D3;_LLC4: if(_tmp1D4.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1D4.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1D4.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1D4.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 506
return 1;};}
# 509
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1D6=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1D7=_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Absyn_Kind*_tmp1DB;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D7;if(_tmp1D8->tag != 0)goto _LLCD;else{_tmp1D9=_tmp1D8->f1;}}_LLCC:
 return _tmp1D9;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D7;if(_tmp1DA->tag != 2)goto _LLCF;else{_tmp1DB=_tmp1DA->f2;}}_LLCE:
 return _tmp1DB;_LLCF:;_LLD0:
# 514
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpFA6;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpFA5;tv->kind=(void*)((_tmpFA5=_cycalloc(sizeof(*_tmpFA5)),((_tmpFA5[0]=((_tmpFA6.tag=2,((_tmpFA6.f1=0,((_tmpFA6.f2=def,_tmpFA6)))))),_tmpFA5))));}
return def;_LLCA:;}
# 519
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpFA7;struct _tuple0 _tmp1DE=(_tmpFA7.f1=Cyc_Absyn_compress_kb(kb1),((_tmpFA7.f2=Cyc_Absyn_compress_kb(kb2),_tmpFA7)));struct _tuple0 _tmp1DF=_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E1;struct Cyc_Absyn_Kind*_tmp1E3;struct Cyc_Core_Opt**_tmp1E5;struct Cyc_Absyn_Kind*_tmp1E6;struct Cyc_Absyn_Kind*_tmp1E8;struct Cyc_Absyn_Kind*_tmp1EA;struct Cyc_Core_Opt**_tmp1EC;struct Cyc_Absyn_Kind*_tmp1ED;struct Cyc_Core_Opt**_tmp1EF;struct Cyc_Absyn_Kind*_tmp1F0;struct Cyc_Core_Opt**_tmp1F2;struct Cyc_Absyn_Kind*_tmp1F3;struct Cyc_Core_Opt**_tmp1F5;void*_tmp1F6;void*_tmp1F7;struct Cyc_Core_Opt**_tmp1F9;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1DF.f1;if(_tmp1E0->tag != 0)goto _LLD4;else{_tmp1E1=_tmp1E0->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1DF.f2;if(_tmp1E2->tag != 0)goto _LLD4;else{_tmp1E3=_tmp1E2->f1;}};_LLD3:
 return _tmp1E1 == _tmp1E3;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1DF.f1;if(_tmp1E4->tag != 2)goto _LLD6;else{_tmp1E5=(struct Cyc_Core_Opt**)& _tmp1E4->f1;_tmp1E6=_tmp1E4->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1DF.f2;if(_tmp1E7->tag != 0)goto _LLD6;else{_tmp1E8=_tmp1E7->f1;}};_LLD5:
# 523
 if(Cyc_Tcutil_kind_leq(_tmp1E8,_tmp1E6)){
{struct Cyc_Core_Opt*_tmpFA8;*_tmp1E5=((_tmpFA8=_cycalloc(sizeof(*_tmpFA8)),((_tmpFA8->v=kb2,_tmpFA8))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1DF.f1;if(_tmp1E9->tag != 0)goto _LLD8;else{_tmp1EA=_tmp1E9->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1DF.f2;if(_tmp1EB->tag != 2)goto _LLD8;else{_tmp1EC=(struct Cyc_Core_Opt**)& _tmp1EB->f1;_tmp1ED=_tmp1EB->f2;}};_LLD7:
# 528
 if(Cyc_Tcutil_kind_leq(_tmp1EA,_tmp1ED)){
{struct Cyc_Core_Opt*_tmpFA9;*_tmp1EC=((_tmpFA9=_cycalloc(sizeof(*_tmpFA9)),((_tmpFA9->v=kb1,_tmpFA9))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1EE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1DF.f1;if(_tmp1EE->tag != 2)goto _LLDA;else{_tmp1EF=(struct Cyc_Core_Opt**)& _tmp1EE->f1;_tmp1F0=_tmp1EE->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1F1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1DF.f2;if(_tmp1F1->tag != 2)goto _LLDA;else{_tmp1F2=(struct Cyc_Core_Opt**)& _tmp1F1->f1;_tmp1F3=_tmp1F1->f2;}};_LLD9:
# 533
 if(Cyc_Tcutil_kind_leq(_tmp1F0,_tmp1F3)){
{struct Cyc_Core_Opt*_tmpFAA;*_tmp1F2=((_tmpFAA=_cycalloc(sizeof(*_tmpFAA)),((_tmpFAA->v=kb1,_tmpFAA))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1F3,_tmp1F0)){
{struct Cyc_Core_Opt*_tmpFAB;*_tmp1EF=((_tmpFAB=_cycalloc(sizeof(*_tmpFAB)),((_tmpFAB->v=kb2,_tmpFAB))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1DF.f1;if(_tmp1F4->tag != 1)goto _LLDC;else{_tmp1F5=(struct Cyc_Core_Opt**)& _tmp1F4->f1;}}_tmp1F6=_tmp1DF.f2;_LLDB:
 _tmp1F7=_tmp1F6;_tmp1F9=_tmp1F5;goto _LLDD;_LLDC: _tmp1F7=_tmp1DF.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1DF.f2;if(_tmp1F8->tag != 1)goto _LLD1;else{_tmp1F9=(struct Cyc_Core_Opt**)& _tmp1F8->f1;}};_LLDD:
# 542
{struct Cyc_Core_Opt*_tmpFAC;*_tmp1F9=((_tmpFAC=_cycalloc(sizeof(*_tmpFAC)),((_tmpFAC->v=_tmp1F7,_tmpFAC))));}
return 1;_LLD1:;}
# 547
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp200=Cyc_Tcutil_compress(t);void*_tmp201=_tmp200;struct Cyc_Absyn_Tvar*_tmp203;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp201;if(_tmp202->tag != 2)goto _LLE1;else{_tmp203=_tmp202->f1;}}_LLE0: {
# 550
void*_tmp204=_tmp203->kind;
_tmp203->kind=kb;{
struct _tuple16 _tmpFAD;return(_tmpFAD.f1=_tmp203,((_tmpFAD.f2=_tmp204,_tmpFAD)));};}_LLE1:;_LLE2: {
# 554
const char*_tmpFB1;void*_tmpFB0[1];struct Cyc_String_pa_PrintArg_struct _tmpFAF;(_tmpFAF.tag=0,((_tmpFAF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFB0[0]=& _tmpFAF,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB1="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpFB1,sizeof(char),40))),_tag_dyneither(_tmpFB0,sizeof(void*),1)))))));}_LLDE:;}
# 560
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 562
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 566
struct _RegionHandle _tmp209=_new_region("temp");struct _RegionHandle*temp=& _tmp209;_push_region(temp);
{struct Cyc_List_List*_tmp20A=0;
# 569
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp20B=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp20C=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp20D=Cyc_Tcutil_tvar_kind(_tmp20B,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp20E=_tmp20D;_LLE4: if(_tmp20E->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp20E->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 575
{struct _tuple16*_tmpFB4;struct Cyc_List_List*_tmpFB3;_tmp20A=((_tmpFB3=_region_malloc(temp,sizeof(*_tmpFB3)),((_tmpFB3->hd=((_tmpFB4=_region_malloc(temp,sizeof(*_tmpFB4)),((_tmpFB4->f1=_tmp20B,((_tmpFB4->f2=_tmp20C,_tmpFB4)))))),((_tmpFB3->tl=_tmp20A,_tmpFB3))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 579
if(_tmp20A != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp20A),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 567
;_pop_region(temp);}
# 584
return k;}
# 591
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp211=Cyc_Tcutil_compress(t);void*_tmp212=_tmp211;struct Cyc_Core_Opt*_tmp214;struct Cyc_Absyn_Tvar*_tmp216;enum Cyc_Absyn_Size_of _tmp219;struct Cyc_Absyn_Datatypedecl*_tmp225;struct Cyc_Absyn_Datatypefield*_tmp226;enum Cyc_Absyn_AggrKind _tmp22A;struct Cyc_List_List*_tmp22B;struct Cyc_Absyn_AggrdeclImpl*_tmp22C;struct Cyc_List_List*_tmp22D;struct Cyc_Absyn_PtrInfo _tmp230;struct Cyc_Absyn_Exp*_tmp234;struct Cyc_Absyn_Typedefdecl*_tmp237;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp212;if(_tmp213->tag != 1)goto _LLED;else{_tmp214=_tmp213->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp214))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp212;if(_tmp215->tag != 2)goto _LLEF;else{_tmp216=_tmp215->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp216,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp212;if(_tmp217->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp212;if(_tmp218->tag != 6)goto _LLF3;else{_tmp219=_tmp218->f2;}}_LLF2:
# 597
 return(_tmp219 == Cyc_Absyn_Int_sz  || _tmp219 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp212;if(_tmp21A->tag != 7)goto _LLF5;}_LLF4:
# 599
 return& Cyc_Tcutil_mk;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp212;if(_tmp21B->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_ak;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp212;if(_tmp21C->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp21D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp212;if(_tmp21D->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp21E=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp212;if(_tmp21E->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp212;if(_tmp21F->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp212;if(_tmp220->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp212;if(_tmp221->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp212;if(_tmp222->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp212;if(_tmp223->tag != 3)goto _LL107;}_LL106:
# 610
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp212;if(_tmp224->tag != 4)goto _LL109;else{if((((_tmp224->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp225=((struct _tuple3)(((_tmp224->f1).field_info).KnownDatatypefield).val).f1;_tmp226=((struct _tuple3)(((_tmp224->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 612
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp212;if(_tmp227->tag != 4)goto _LL10B;else{if((((_tmp227->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 614
const char*_tmpFB7;void*_tmpFB6;(_tmpFB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFB7="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpFB7,sizeof(char),39))),_tag_dyneither(_tmpFB6,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp212;if(_tmp228->tag != 11)goto _LL10D;else{if((((_tmp228->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 617
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp212;if(_tmp229->tag != 11)goto _LL10F;else{if((((_tmp229->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp22A=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp229->f1).aggr_info).KnownAggr).val))->kind;_tmp22B=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp229->f1).aggr_info).KnownAggr).val))->tvs;_tmp22C=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp229->f1).aggr_info).KnownAggr).val))->impl;_tmp22D=(_tmp229->f1).targs;}}_LL10E:
# 619
 if(_tmp22C == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp243=_tmp22C->fields;
if(_tmp243 == 0)return& Cyc_Tcutil_mk;
# 623
if(_tmp22A == Cyc_Absyn_StructA){
for(0;_tmp243->tl != 0;_tmp243=_tmp243->tl){;}{
void*_tmp244=((struct Cyc_Absyn_Aggrfield*)_tmp243->hd)->type;
struct Cyc_Absyn_Kind*_tmp245=Cyc_Tcutil_field_kind(_tmp244,_tmp22D,_tmp22B);
if(_tmp245 == & Cyc_Tcutil_ak  || _tmp245 == & Cyc_Tcutil_tak)return _tmp245;};}else{
# 631
for(0;_tmp243 != 0;_tmp243=_tmp243->tl){
void*_tmp246=((struct Cyc_Absyn_Aggrfield*)_tmp243->hd)->type;
struct Cyc_Absyn_Kind*_tmp247=Cyc_Tcutil_field_kind(_tmp246,_tmp22D,_tmp22B);
if(_tmp247 == & Cyc_Tcutil_ak  || _tmp247 == & Cyc_Tcutil_tak)return _tmp247;}}
# 637
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp212;if(_tmp22E->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp212;if(_tmp22F->tag != 5)goto _LL113;else{_tmp230=_tmp22F->f1;}}_LL112: {
# 640
void*_tmp248=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp230.ptr_atts).bounds);void*_tmp249=_tmp248;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp24A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LL12C;}_LL12B: {
# 642
enum Cyc_Absyn_AliasQual _tmp24C=(Cyc_Tcutil_typ_kind((_tmp230.ptr_atts).rgn))->aliasqual;switch(_tmp24C){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp24B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp249;if(_tmp24B->tag != 1)goto _LL129;}_LL12D: {
# 648
enum Cyc_Absyn_AliasQual _tmp24D=(Cyc_Tcutil_typ_kind((_tmp230.ptr_atts).rgn))->aliasqual;switch(_tmp24D){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp212;if(_tmp231->tag != 18)goto _LL115;}_LL114:
# 654
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp212;if(_tmp232->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp212;if(_tmp233->tag != 8)goto _LL119;else{_tmp234=(_tmp233->f1).num_elts;}}_LL118:
# 657
 if(_tmp234 == 0  || Cyc_Tcutil_is_const_exp(_tmp234))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp212;if(_tmp235->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp212;if(_tmp236->tag != 17)goto _LL11D;else{_tmp237=_tmp236->f3;}}_LL11C:
# 661
 if(_tmp237 == 0  || _tmp237->kind == 0){
const char*_tmpFBB;void*_tmpFBA[1];struct Cyc_String_pa_PrintArg_struct _tmpFB9;(_tmpFB9.tag=0,((_tmpFB9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpFBA[0]=& _tmpFB9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFBB="typ_kind: typedef found: %s",_tag_dyneither(_tmpFBB,sizeof(char),28))),_tag_dyneither(_tmpFBA,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp237->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp212;if(_tmp238->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp239=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp238->f1)->r;if(_tmp239->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp212;if(_tmp23A->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp23B=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp23A->f1)->r;if(_tmp23B->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp212;if(_tmp23C->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp23D=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp23C->f1)->r;if(_tmp23D->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp212;if(_tmp23E->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp23F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp212;if(_tmp23F->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp212;if(_tmp240->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 673
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 678
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp251;_push_handler(& _tmp251);{int _tmp253=0;if(setjmp(_tmp251.handler))_tmp253=1;if(!_tmp253){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp254=1;_npop_handler(0);return _tmp254;};
# 680
;_pop_handler();}else{void*_tmp252=(void*)_exn_thrown;void*_tmp256=_tmp252;void*_tmp258;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp257=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp256;if(_tmp257->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 682
 return 0;_LL139: _tmp258=_tmp256;_LL13A:(void)_throw(_tmp258);_LL136:;}};}
# 687
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp259=t;struct Cyc_Absyn_Tvar*_tmp25B;void*_tmp25D;struct Cyc_Core_Opt**_tmp25E;struct Cyc_Absyn_PtrInfo _tmp260;void*_tmp262;struct Cyc_List_List*_tmp264;void*_tmp265;struct Cyc_Absyn_Tqual _tmp266;void*_tmp267;struct Cyc_List_List*_tmp268;int _tmp269;struct Cyc_Absyn_VarargInfo*_tmp26A;struct Cyc_List_List*_tmp26B;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_List_List*_tmp26E;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_List_List*_tmp270;struct Cyc_List_List*_tmp272;struct Cyc_List_List*_tmp274;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp278;struct Cyc_List_List*_tmp27A;struct Cyc_List_List*_tmp27C;void*_tmp27E;void*_tmp280;void*_tmp282;void*_tmp284;struct Cyc_List_List*_tmp286;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp25A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp259;if(_tmp25A->tag != 2)goto _LL13E;else{_tmp25B=_tmp25A->f1;}}_LL13D:
# 692
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp25B)){
{const char*_tmpFBC;Cyc_Tcutil_failure_reason=((_tmpFBC="(type variable would escape scope)",_tag_dyneither(_tmpFBC,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 696
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp25C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp259;if(_tmp25C->tag != 1)goto _LL140;else{_tmp25D=(void*)_tmp25C->f2;_tmp25E=(struct Cyc_Core_Opt**)& _tmp25C->f4;}}_LL13F:
# 698
 if(t == evar){
{const char*_tmpFBD;Cyc_Tcutil_failure_reason=((_tmpFBD="(occurs check)",_tag_dyneither(_tmpFBD,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 702
if(_tmp25D != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp25D);else{
# 705
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp25E))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 712
if(problem){
struct Cyc_List_List*_tmp289=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp25E))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpFBE;_tmp289=((_tmpFBE=_cycalloc(sizeof(*_tmpFBE)),((_tmpFBE->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpFBE->tl=_tmp289,_tmpFBE))))));}}}{
# 718
struct Cyc_Core_Opt*_tmpFBF;*_tmp25E=((_tmpFBF=_cycalloc(sizeof(*_tmpFBF)),((_tmpFBF->v=_tmp289,_tmpFBF))));};}}}
# 721
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp259;if(_tmp25F->tag != 5)goto _LL142;else{_tmp260=_tmp25F->f1;}}_LL141:
# 724
 Cyc_Tcutil_occurs(evar,r,env,_tmp260.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp260.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp261=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp259;if(_tmp261->tag != 8)goto _LL144;else{_tmp262=(_tmp261->f1).elt_type;}}_LL143:
# 728
 Cyc_Tcutil_occurs(evar,r,env,_tmp262);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp263=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp259;if(_tmp263->tag != 9)goto _LL146;else{_tmp264=(_tmp263->f1).tvars;_tmp265=(_tmp263->f1).effect;_tmp266=(_tmp263->f1).ret_tqual;_tmp267=(_tmp263->f1).ret_typ;_tmp268=(_tmp263->f1).args;_tmp269=(_tmp263->f1).c_varargs;_tmp26A=(_tmp263->f1).cyc_varargs;_tmp26B=(_tmp263->f1).rgn_po;_tmp26C=(_tmp263->f1).attributes;_tmp26D=(_tmp263->f1).requires_clause;_tmp26E=(_tmp263->f1).requires_relns;_tmp26F=(_tmp263->f1).ensures_clause;_tmp270=(_tmp263->f1).ensures_relns;}}_LL145:
# 731
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp264,env);
if(_tmp265 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp265);
Cyc_Tcutil_occurs(evar,r,env,_tmp267);
for(0;_tmp268 != 0;_tmp268=_tmp268->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp268->hd)).f3);}
if(_tmp26A != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp26A->type);
for(0;_tmp26B != 0;_tmp26B=_tmp26B->tl){
struct _tuple0*_tmp28C=(struct _tuple0*)_tmp26B->hd;void*_tmp28E;void*_tmp28F;struct _tuple0*_tmp28D=_tmp28C;_tmp28E=_tmp28D->f1;_tmp28F=_tmp28D->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp28E);
Cyc_Tcutil_occurs(evar,r,env,_tmp28F);}
# 743
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp259;if(_tmp271->tag != 10)goto _LL148;else{_tmp272=_tmp271->f1;}}_LL147:
# 745
 for(0;_tmp272 != 0;_tmp272=_tmp272->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp272->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp259;if(_tmp273->tag != 3)goto _LL14A;else{_tmp274=(_tmp273->f1).targs;}}_LL149:
# 749
 Cyc_Tcutil_occurslist(evar,r,env,_tmp274);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp259;if(_tmp275->tag != 17)goto _LL14C;else{_tmp276=_tmp275->f2;}}_LL14B:
 _tmp278=_tmp276;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp277=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp259;if(_tmp277->tag != 4)goto _LL14E;else{_tmp278=(_tmp277->f1).targs;}}_LL14D:
 _tmp27A=_tmp278;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp279=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp259;if(_tmp279->tag != 11)goto _LL150;else{_tmp27A=(_tmp279->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp27A);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp27B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp259;if(_tmp27B->tag != 12)goto _LL152;else{_tmp27C=_tmp27B->f2;}}_LL151:
# 755
 for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp27C->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp27D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp259;if(_tmp27D->tag != 19)goto _LL154;else{_tmp27E=(void*)_tmp27D->f1;}}_LL153:
 _tmp280=_tmp27E;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp27F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp259;if(_tmp27F->tag != 23)goto _LL156;else{_tmp280=(void*)_tmp27F->f1;}}_LL155:
 _tmp282=_tmp280;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp281=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp259;if(_tmp281->tag != 15)goto _LL158;else{_tmp282=(void*)_tmp281->f1;}}_LL157:
 _tmp284=_tmp282;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp283=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp259;if(_tmp283->tag != 25)goto _LL15A;else{_tmp284=(void*)_tmp283->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp284);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp285=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp259;if(_tmp285->tag != 24)goto _LL15C;else{_tmp286=_tmp285->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp286);goto _LL13B;_LL15C:;_LL15D:
# 764
 goto _LL13B;_LL13B:;};}
# 767
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 769
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 774
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 782
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpFC2;void*_tmpFC1;(_tmpFC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC2="tq1 real_const not set.",_tag_dyneither(_tmpFC2,sizeof(char),24))),_tag_dyneither(_tmpFC1,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpFC5;void*_tmpFC4;(_tmpFC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC5="tq2 real_const not set.",_tag_dyneither(_tmpFC5,sizeof(char),24))),_tag_dyneither(_tmpFC4,sizeof(void*),0)));}
# 788
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 791
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpFC6;Cyc_Tcutil_failure_reason=((_tmpFC6="(qualifiers don't match)",_tag_dyneither(_tmpFC6,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 799
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 803
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 810
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 812
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp295=x;void*_tmp296;_LL15F: if((_tmp295->No_constr).tag != 3)goto _LL161;_LL160:
# 817
{union Cyc_Absyn_Constraint _tmpFC7;*x=(((_tmpFC7.Forward_constr).val=y,(((_tmpFC7.Forward_constr).tag=2,_tmpFC7))));}return;_LL161: if((_tmp295->Eq_constr).tag != 1)goto _LL163;_tmp296=(void*)(_tmp295->Eq_constr).val;_LL162: {
# 819
union Cyc_Absyn_Constraint*_tmp298=y;void*_tmp299;_LL166: if((_tmp298->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp298->Eq_constr).tag != 1)goto _LL16A;_tmp299=(void*)(_tmp298->Eq_constr).val;_LL169:
# 822
 if(cmp(_tmp296,_tmp299)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 826
return;_LL16A: if((_tmp298->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpFCA;void*_tmpFC9;(_tmpFC9=0,Cyc_Tcutil_impos(((_tmpFCA="unify_conref: forward after compress(2)",_tag_dyneither(_tmpFCA,sizeof(char),40))),_tag_dyneither(_tmpFC9,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp295->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 829
const char*_tmpFCD;void*_tmpFCC;(_tmpFCC=0,Cyc_Tcutil_impos(((_tmpFCD="unify_conref: forward after compress",_tag_dyneither(_tmpFCD,sizeof(char),37))),_tag_dyneither(_tmpFCC,sizeof(void*),0)));}_LL15E:;};}
# 833
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp29E;_push_handler(& _tmp29E);{int _tmp2A0=0;if(setjmp(_tmp29E.handler))_tmp2A0=1;if(!_tmp2A0){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp2A1=1;_npop_handler(0);return _tmp2A1;};
# 835
;_pop_handler();}else{void*_tmp29F=(void*)_exn_thrown;void*_tmp2A3=_tmp29F;void*_tmp2A5;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2A4=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2A3;if(_tmp2A4->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 837
 return 0;_LL16F: _tmp2A5=_tmp2A3;_LL170:(void)_throw(_tmp2A5);_LL16C:;}};}
# 840
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpFCE;struct _tuple0 _tmp2A6=(_tmpFCE.f1=b1,((_tmpFCE.f2=b2,_tmpFCE)));struct _tuple0 _tmp2A7=_tmp2A6;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AF;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A7.f1;if(_tmp2A8->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A7.f2;if(_tmp2A9->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A7.f1;if(_tmp2AA->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A7.f2;if(_tmp2AB->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A7.f1;if(_tmp2AC->tag != 1)goto _LL171;else{_tmp2AD=_tmp2AC->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A7.f2;if(_tmp2AE->tag != 1)goto _LL171;else{_tmp2AF=_tmp2AE->f1;}};_LL179:
# 846
 return Cyc_Evexp_const_exp_cmp(_tmp2AD,_tmp2AF);_LL171:;}
# 850
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmpFCF;struct _tuple0 _tmp2B1=(_tmpFCF.f1=e1->r,((_tmpFCF.f2=e2->r,_tmpFCF)));struct _tuple0 _tmp2B2=_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Absyn_Exp*_tmp2B6;void*_tmp2B8;void*_tmp2BA;_LL17B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B2.f1;if(_tmp2B3->tag != 13)goto _LL17D;else{_tmp2B4=_tmp2B3->f2;}}_LL17C:
# 856
 Cyc_Tcutil_unify_cmp_exp(_tmp2B4,e2);
return;_LL17D: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2B2.f2;if(_tmp2B5->tag != 13)goto _LL17F;else{_tmp2B6=_tmp2B5->f2;}}_LL17E:
# 859
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp2B6);
return;_LL17F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2B2.f1;if(_tmp2B7->tag != 38)goto _LL181;else{_tmp2B8=(void*)_tmp2B7->f1;}}_LL180:
# 862
{void*_tmp2BB=Cyc_Tcutil_compress(_tmp2B8);void*_tmp2BC=_tmp2BB;_LL186: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2BD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BC;if(_tmp2BD->tag != 1)goto _LL188;}_LL187:
# 864
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFD2;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFD1;Cyc_Tcutil_unify_it(_tmp2B8,(void*)((_tmpFD1=_cycalloc(sizeof(*_tmpFD1)),((_tmpFD1[0]=((_tmpFD2.tag=18,((_tmpFD2.f1=e2,_tmpFD2)))),_tmpFD1)))));}
return;_LL188:;_LL189:
# 867
 goto _LL185;_LL185:;}
# 869
goto _LL17A;_LL181: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2B9=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2B2.f2;if(_tmp2B9->tag != 38)goto _LL183;else{_tmp2BA=(void*)_tmp2B9->f1;}}_LL182:
# 871
{void*_tmp2C0=Cyc_Tcutil_compress(_tmp2BA);void*_tmp2C1=_tmp2C0;_LL18B: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C2=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C1;if(_tmp2C2->tag != 1)goto _LL18D;}_LL18C:
# 873
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFD5;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFD4;Cyc_Tcutil_unify_it(_tmp2BA,(void*)((_tmpFD4=_cycalloc(sizeof(*_tmpFD4)),((_tmpFD4[0]=((_tmpFD5.tag=18,((_tmpFD5.f1=e1,_tmpFD5)))),_tmpFD4)))));}
return;_LL18D:;_LL18E:
# 876
 goto _LL18A;_LL18A:;}
# 878
goto _LL17A;_LL183:;_LL184:
 goto _LL17A;_LL17A:;}
# 881
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 884
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpFD6;struct _tuple0 _tmp2C6=(_tmpFD6.f1=b1,((_tmpFD6.f2=b2,_tmpFD6)));struct _tuple0 _tmp2C7=_tmp2C6;struct Cyc_Absyn_Exp*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CF;_LL190:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2C8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C7.f1;if(_tmp2C8->tag != 0)goto _LL192;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2C9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C7.f2;if(_tmp2C9->tag != 0)goto _LL192;};_LL191:
 return 0;_LL192: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2CA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C7.f1;if(_tmp2CA->tag != 0)goto _LL194;}_LL193:
 return - 1;_LL194: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2CB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2C7.f2;if(_tmp2CB->tag != 0)goto _LL196;}_LL195:
 return 1;_LL196:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2CC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2C7.f1;if(_tmp2CC->tag != 1)goto _LL18F;else{_tmp2CD=_tmp2CC->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2CE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2C7.f2;if(_tmp2CE->tag != 1)goto _LL18F;else{_tmp2CF=_tmp2CE->f1;}};_LL197: {
# 890
int r=Cyc_Evexp_const_exp_cmp(_tmp2CD,_tmp2CF);
if(r == 0)return 0;{
struct _handler_cons _tmp2D0;_push_handler(& _tmp2D0);{int _tmp2D2=0;if(setjmp(_tmp2D0.handler))_tmp2D2=1;if(!_tmp2D2){
Cyc_Tcutil_unify_cmp_exp(_tmp2CD,_tmp2CF);{
int _tmp2D3=0;_npop_handler(0);return _tmp2D3;};
# 893
;_pop_handler();}else{void*_tmp2D1=(void*)_exn_thrown;void*_tmp2D5=_tmp2D1;void*_tmp2D7;_LL199: {struct Cyc_Tcutil_Unify_exn_struct*_tmp2D6=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp2D5;if(_tmp2D6->tag != Cyc_Tcutil_Unify)goto _LL19B;}_LL19A:
# 896
 return r;_LL19B: _tmp2D7=_tmp2D5;_LL19C:(void)_throw(_tmp2D7);_LL198:;}};};}_LL18F:;}
# 901
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2D9=att;_LL19E: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2DA=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DA->tag != 0)goto _LL1A0;}_LL19F:
 return 0;_LL1A0: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2DB=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DB->tag != 1)goto _LL1A2;}_LL1A1:
 return 1;_LL1A2: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2DC=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DC->tag != 2)goto _LL1A4;}_LL1A3:
 return 2;_LL1A4: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2DD=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DD->tag != 3)goto _LL1A6;}_LL1A5:
 return 3;_LL1A6: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2DE=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DE->tag != 4)goto _LL1A8;}_LL1A7:
 return 4;_LL1A8: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2DF=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2DF->tag != 5)goto _LL1AA;}_LL1A9:
 return 5;_LL1AA: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2E0=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E0->tag != 6)goto _LL1AC;}_LL1AB:
 return 6;_LL1AC: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2E1=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E1->tag != 7)goto _LL1AE;}_LL1AD:
 return 7;_LL1AE: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2E2=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E2->tag != 8)goto _LL1B0;}_LL1AF:
 return 8;_LL1B0: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2E3=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E3->tag != 9)goto _LL1B2;}_LL1B1:
 return 9;_LL1B2: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2E4=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E4->tag != 10)goto _LL1B4;}_LL1B3:
 return 10;_LL1B4: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2E5=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E5->tag != 11)goto _LL1B6;}_LL1B5:
 return 11;_LL1B6: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2E6=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E6->tag != 12)goto _LL1B8;}_LL1B7:
 return 12;_LL1B8: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2E7=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E7->tag != 13)goto _LL1BA;}_LL1B9:
 return 13;_LL1BA: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2E8=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E8->tag != 14)goto _LL1BC;}_LL1BB:
 return 14;_LL1BC: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2E9=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2E9->tag != 15)goto _LL1BE;}_LL1BD:
 return 15;_LL1BE: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2EA=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2EA->tag != 16)goto _LL1C0;}_LL1BF:
 return 16;_LL1C0: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2EB=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2EB->tag != 17)goto _LL1C2;}_LL1C1:
 return 17;_LL1C2: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2EC=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2EC->tag != 18)goto _LL1C4;}_LL1C3:
 return 18;_LL1C4: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2ED=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2ED->tag != 19)goto _LL1C6;}_LL1C5:
 return 19;_LL1C6: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2EE=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2D9;if(_tmp2EE->tag != 20)goto _LL1C8;}_LL1C7:
 return 20;_LL1C8:;_LL1C9:
 return 21;_LL19D:;}
# 928
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpFD7;struct _tuple0 _tmp2EF=(_tmpFD7.f1=att1,((_tmpFD7.f2=att2,_tmpFD7)));struct _tuple0 _tmp2F0=_tmp2EF;int _tmp2F2;int _tmp2F4;int _tmp2F6;int _tmp2F8;int _tmp2FA;int _tmp2FC;struct _dyneither_ptr _tmp2FE;struct _dyneither_ptr _tmp300;enum Cyc_Absyn_Format_Type _tmp302;int _tmp303;int _tmp304;enum Cyc_Absyn_Format_Type _tmp306;int _tmp307;int _tmp308;_LL1CB:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2F1=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2F0.f1;if(_tmp2F1->tag != 0)goto _LL1CD;else{_tmp2F2=_tmp2F1->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2F3=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2F0.f2;if(_tmp2F3->tag != 0)goto _LL1CD;else{_tmp2F4=_tmp2F3->f1;}};_LL1CC:
 _tmp2F6=_tmp2F2;_tmp2F8=_tmp2F4;goto _LL1CE;_LL1CD:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2F5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2F0.f1;if(_tmp2F5->tag != 20)goto _LL1CF;else{_tmp2F6=_tmp2F5->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2F7=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2F0.f2;if(_tmp2F7->tag != 20)goto _LL1CF;else{_tmp2F8=_tmp2F7->f1;}};_LL1CE:
 _tmp2FA=_tmp2F6;_tmp2FC=_tmp2F8;goto _LL1D0;_LL1CF:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2F9=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2F0.f1;if(_tmp2F9->tag != 6)goto _LL1D1;else{_tmp2FA=_tmp2F9->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2FB=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2F0.f2;if(_tmp2FB->tag != 6)goto _LL1D1;else{_tmp2FC=_tmp2FB->f1;}};_LL1D0:
 return Cyc_Core_intcmp(_tmp2FA,_tmp2FC);_LL1D1:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2FD=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2F0.f1;if(_tmp2FD->tag != 8)goto _LL1D3;else{_tmp2FE=_tmp2FD->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2FF=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2F0.f2;if(_tmp2FF->tag != 8)goto _LL1D3;else{_tmp300=_tmp2FF->f1;}};_LL1D2:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2FE,(struct _dyneither_ptr)_tmp300);_LL1D3:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp301=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2F0.f1;if(_tmp301->tag != 19)goto _LL1D5;else{_tmp302=_tmp301->f1;_tmp303=_tmp301->f2;_tmp304=_tmp301->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp305=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2F0.f2;if(_tmp305->tag != 19)goto _LL1D5;else{_tmp306=_tmp305->f1;_tmp307=_tmp305->f2;_tmp308=_tmp305->f3;}};_LL1D4: {
# 935
int _tmp309=Cyc_Core_intcmp((int)((unsigned int)_tmp302),(int)((unsigned int)_tmp306));
if(_tmp309 != 0)return _tmp309;{
int _tmp30A=Cyc_Core_intcmp(_tmp303,_tmp307);
if(_tmp30A != 0)return _tmp30A;
return Cyc_Core_intcmp(_tmp304,_tmp308);};}_LL1D5:;_LL1D6:
# 941
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1CA:;}
# 945
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 949
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 958
static void*Cyc_Tcutil_rgns_of(void*t);
# 960
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 964
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp30C=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp30D=_tmp30C;_LL1D8: if(_tmp30D->kind != Cyc_Absyn_RgnKind)goto _LL1DA;if(_tmp30D->aliasqual != Cyc_Absyn_Unique)goto _LL1DA;_LL1D9:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1D7;_LL1DA: if(_tmp30D->kind != Cyc_Absyn_RgnKind)goto _LL1DC;if(_tmp30D->aliasqual != Cyc_Absyn_Aliasable)goto _LL1DC;_LL1DB:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1D7;_LL1DC: if(_tmp30D->kind != Cyc_Absyn_EffKind)goto _LL1DE;_LL1DD:
 t=Cyc_Absyn_empty_effect;goto _LL1D7;_LL1DE: if(_tmp30D->kind != Cyc_Absyn_IntKind)goto _LL1E0;_LL1DF:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFDA;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFD9;t=(void*)((_tmpFD9=_cycalloc(sizeof(*_tmpFD9)),((_tmpFD9[0]=((_tmpFDA.tag=18,((_tmpFDA.f1=Cyc_Absyn_uint_exp(0,0),_tmpFDA)))),_tmpFD9))));}goto _LL1D7;_LL1E0:;_LL1E1:
 t=Cyc_Absyn_sint_typ;goto _LL1D7;_LL1D7:;}{
# 973
struct _tuple16*_tmpFDB;return(_tmpFDB=_cycalloc(sizeof(*_tmpFDB)),((_tmpFDB->f1=tv,((_tmpFDB->f2=t,_tmpFDB)))));};}
# 980
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp311=Cyc_Tcutil_compress(t);void*_tmp312=_tmp311;void*_tmp31C;void*_tmp31E;void*_tmp31F;struct Cyc_List_List*_tmp321;void*_tmp323;void*_tmp324;void*_tmp326;struct Cyc_List_List*_tmp328;struct Cyc_List_List*_tmp32A;struct Cyc_List_List*_tmp32C;struct Cyc_List_List*_tmp32E;struct Cyc_List_List*_tmp331;void*_tmp332;struct Cyc_Absyn_Tqual _tmp333;void*_tmp334;struct Cyc_List_List*_tmp335;struct Cyc_Absyn_VarargInfo*_tmp336;struct Cyc_List_List*_tmp337;void*_tmp33E;struct Cyc_List_List*_tmp340;_LL1E3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp312;if(_tmp313->tag != 0)goto _LL1E5;}_LL1E4:
 goto _LL1E6;_LL1E5: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp312;if(_tmp314->tag != 7)goto _LL1E7;}_LL1E6:
 goto _LL1E8;_LL1E7: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp315=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312;if(_tmp315->tag != 13)goto _LL1E9;}_LL1E8:
 goto _LL1EA;_LL1E9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312;if(_tmp316->tag != 14)goto _LL1EB;}_LL1EA:
 goto _LL1EC;_LL1EB: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp317=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp312;if(_tmp317->tag != 18)goto _LL1ED;}_LL1EC:
 goto _LL1EE;_LL1ED: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312;if(_tmp318->tag != 6)goto _LL1EF;}_LL1EE:
 return Cyc_Absyn_empty_effect;_LL1EF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp319=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp312;if(_tmp319->tag != 1)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp31A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp312;if(_tmp31A->tag != 2)goto _LL1F3;}_LL1F2: {
# 990
struct Cyc_Absyn_Kind*_tmp342=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp343=_tmp342;_LL21A: if(_tmp343->kind != Cyc_Absyn_RgnKind)goto _LL21C;_LL21B: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFDE;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFDD;return(void*)((_tmpFDD=_cycalloc(sizeof(*_tmpFDD)),((_tmpFDD[0]=((_tmpFDE.tag=23,((_tmpFDE.f1=t,_tmpFDE)))),_tmpFDD))));}_LL21C: if(_tmp343->kind != Cyc_Absyn_EffKind)goto _LL21E;_LL21D:
 return t;_LL21E: if(_tmp343->kind != Cyc_Absyn_IntKind)goto _LL220;_LL21F:
 return Cyc_Absyn_empty_effect;_LL220:;_LL221: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpFE1;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpFE0;return(void*)((_tmpFE0=_cycalloc(sizeof(*_tmpFE0)),((_tmpFE0[0]=((_tmpFE1.tag=25,((_tmpFE1.f1=t,_tmpFE1)))),_tmpFE0))));}_LL219:;}_LL1F3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp31B=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp312;if(_tmp31B->tag != 15)goto _LL1F5;else{_tmp31C=(void*)_tmp31B->f1;}}_LL1F4: {
# 996
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFE4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFE3;return(void*)((_tmpFE3=_cycalloc(sizeof(*_tmpFE3)),((_tmpFE3[0]=((_tmpFE4.tag=23,((_tmpFE4.f1=_tmp31C,_tmpFE4)))),_tmpFE3))));}_LL1F5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp31D=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312;if(_tmp31D->tag != 16)goto _LL1F7;else{_tmp31E=(void*)_tmp31D->f1;_tmp31F=(void*)_tmp31D->f2;}}_LL1F6: {
# 1000
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFE7;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFE6;return(void*)((_tmpFE6=_cycalloc(sizeof(*_tmpFE6)),((_tmpFE6[0]=((_tmpFE7.tag=23,((_tmpFE7.f1=_tmp31F,_tmpFE7)))),_tmpFE6))));}_LL1F7: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp320=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312;if(_tmp320->tag != 3)goto _LL1F9;else{_tmp321=(_tmp320->f1).targs;}}_LL1F8: {
# 1002
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp321);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFEA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFE9;return Cyc_Tcutil_normalize_effect((void*)((_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9[0]=((_tmpFEA.tag=24,((_tmpFEA.f1=ts,_tmpFEA)))),_tmpFE9)))));}_LL1F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp322=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312;if(_tmp322->tag != 5)goto _LL1FB;else{_tmp323=(_tmp322->f1).elt_typ;_tmp324=((_tmp322->f1).ptr_atts).rgn;}}_LL1FA: {
# 1006
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFF9;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFF8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFF7;void*_tmpFF6[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFF5;return Cyc_Tcutil_normalize_effect((void*)((_tmpFF5=_cycalloc(sizeof(*_tmpFF5)),((_tmpFF5[0]=((_tmpFF9.tag=24,((_tmpFF9.f1=((_tmpFF6[1]=Cyc_Tcutil_rgns_of(_tmp323),((_tmpFF6[0]=(void*)((_tmpFF8=_cycalloc(sizeof(*_tmpFF8)),((_tmpFF8[0]=((_tmpFF7.tag=23,((_tmpFF7.f1=_tmp324,_tmpFF7)))),_tmpFF8)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFF6,sizeof(void*),2)))))),_tmpFF9)))),_tmpFF5)))));}_LL1FB: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312;if(_tmp325->tag != 8)goto _LL1FD;else{_tmp326=(_tmp325->f1).elt_type;}}_LL1FC:
# 1008
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp326));_LL1FD: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp327=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp312;if(_tmp327->tag != 10)goto _LL1FF;else{_tmp328=_tmp327->f1;}}_LL1FE: {
# 1010
struct Cyc_List_List*_tmp353=0;
for(0;_tmp328 != 0;_tmp328=_tmp328->tl){
struct Cyc_List_List*_tmpFFA;_tmp353=((_tmpFFA=_cycalloc(sizeof(*_tmpFFA)),((_tmpFFA->hd=(*((struct _tuple12*)_tmp328->hd)).f2,((_tmpFFA->tl=_tmp353,_tmpFFA))))));}
_tmp32A=_tmp353;goto _LL200;}_LL1FF: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp329=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312;if(_tmp329->tag != 4)goto _LL201;else{_tmp32A=(_tmp329->f1).targs;}}_LL200:
 _tmp32C=_tmp32A;goto _LL202;_LL201: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312;if(_tmp32B->tag != 11)goto _LL203;else{_tmp32C=(_tmp32B->f1).targs;}}_LL202: {
# 1016
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFFD;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFFC;return Cyc_Tcutil_normalize_effect((void*)((_tmpFFC=_cycalloc(sizeof(*_tmpFFC)),((_tmpFFC[0]=((_tmpFFD.tag=24,((_tmpFFD.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp32C),_tmpFFD)))),_tmpFFC)))));}_LL203: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312;if(_tmp32D->tag != 12)goto _LL205;else{_tmp32E=_tmp32D->f2;}}_LL204: {
# 1018
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1000;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFFF;return Cyc_Tcutil_normalize_effect((void*)((_tmpFFF=_cycalloc(sizeof(*_tmpFFF)),((_tmpFFF[0]=((_tmp1000.tag=24,((_tmp1000.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp32E),_tmp1000)))),_tmpFFF)))));}_LL205: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp32F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp312;if(_tmp32F->tag != 19)goto _LL207;}_LL206:
 return Cyc_Absyn_empty_effect;_LL207: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp330=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312;if(_tmp330->tag != 9)goto _LL209;else{_tmp331=(_tmp330->f1).tvars;_tmp332=(_tmp330->f1).effect;_tmp333=(_tmp330->f1).ret_tqual;_tmp334=(_tmp330->f1).ret_typ;_tmp335=(_tmp330->f1).args;_tmp336=(_tmp330->f1).cyc_varargs;_tmp337=(_tmp330->f1).rgn_po;}}_LL208: {
# 1028
void*_tmp359=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp331),(void*)_check_null(_tmp332));
return Cyc_Tcutil_normalize_effect(_tmp359);}_LL209: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp312;if(_tmp338->tag != 21)goto _LL20B;}_LL20A:
 goto _LL20C;_LL20B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp339=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp312;if(_tmp339->tag != 22)goto _LL20D;}_LL20C:
 goto _LL20E;_LL20D: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp33A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp312;if(_tmp33A->tag != 20)goto _LL20F;}_LL20E:
 return Cyc_Absyn_empty_effect;_LL20F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp312;if(_tmp33B->tag != 23)goto _LL211;}_LL210:
 goto _LL212;_LL211: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp33C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp312;if(_tmp33C->tag != 24)goto _LL213;}_LL212:
 return t;_LL213: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp312;if(_tmp33D->tag != 25)goto _LL215;else{_tmp33E=(void*)_tmp33D->f1;}}_LL214:
 return Cyc_Tcutil_rgns_of(_tmp33E);_LL215: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312;if(_tmp33F->tag != 17)goto _LL217;else{_tmp340=_tmp33F->f2;}}_LL216: {
# 1037
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1003;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1002;return Cyc_Tcutil_normalize_effect((void*)((_tmp1002=_cycalloc(sizeof(*_tmp1002)),((_tmp1002[0]=((_tmp1003.tag=24,((_tmp1003.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp340),_tmp1003)))),_tmp1002)))));}_LL217: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp312;if(_tmp341->tag != 26)goto _LL1E2;}_LL218: {
const char*_tmp1006;void*_tmp1005;(_tmp1005=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1006="typedecl in rgns_of",_tag_dyneither(_tmp1006,sizeof(char),20))),_tag_dyneither(_tmp1005,sizeof(void*),0)));}_LL1E2:;}
# 1045
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp35E=e;struct Cyc_List_List**_tmp360;void*_tmp362;_LL223: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp35F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp35E;if(_tmp35F->tag != 24)goto _LL225;else{_tmp360=(struct Cyc_List_List**)& _tmp35F->f1;}}_LL224: {
# 1049
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp360;for(0;effs != 0;effs=effs->tl){
void*_tmp363=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp363));{
void*_tmp364=(void*)effs->hd;void*_tmp365=_tmp364;_LL22A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp365;if(_tmp366->tag != 24)goto _LL22C;}_LL22B:
 goto _LL22D;_LL22C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp367=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp365;if(_tmp367->tag != 23)goto _LL22E;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp367->f1);if(_tmp368->tag != 20)goto _LL22E;}}_LL22D:
 redo_join=1;goto _LL229;_LL22E: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp369=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp365;if(_tmp369->tag != 23)goto _LL230;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp36A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp369->f1);if(_tmp36A->tag != 22)goto _LL230;}}_LL22F:
 redo_join=1;goto _LL229;_LL230: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp36B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp365;if(_tmp36B->tag != 23)goto _LL232;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp36C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp36B->f1);if(_tmp36C->tag != 21)goto _LL232;}}_LL231:
 redo_join=1;goto _LL229;_LL232:;_LL233:
 goto _LL229;_LL229:;};}}
# 1061
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp360;for(0;effs != 0;effs=effs->tl){
void*_tmp36D=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp36E=_tmp36D;struct Cyc_List_List*_tmp370;void*_tmp377;_LL235: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp36F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp36E;if(_tmp36F->tag != 24)goto _LL237;else{_tmp370=_tmp36F->f1;}}_LL236:
# 1066
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp370,effects);
goto _LL234;_LL237: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp371=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36E;if(_tmp371->tag != 23)goto _LL239;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp371->f1);if(_tmp372->tag != 20)goto _LL239;}}_LL238:
 goto _LL234;_LL239: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp373=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36E;if(_tmp373->tag != 23)goto _LL23B;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp374=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp373->f1);if(_tmp374->tag != 22)goto _LL23B;}}_LL23A:
 goto _LL234;_LL23B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp375=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp36E;if(_tmp375->tag != 23)goto _LL23D;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp376=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp375->f1);if(_tmp376->tag != 21)goto _LL23D;}}_LL23C:
 goto _LL234;_LL23D: _tmp377=_tmp36E;_LL23E:
{struct Cyc_List_List*_tmp1007;effects=((_tmp1007=_cycalloc(sizeof(*_tmp1007)),((_tmp1007->hd=_tmp377,((_tmp1007->tl=effects,_tmp1007))))));}goto _LL234;_LL234:;}}
# 1074
*_tmp360=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL225: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp361=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp35E;if(_tmp361->tag != 25)goto _LL227;else{_tmp362=(void*)_tmp361->f1;}}_LL226: {
# 1077
void*_tmp379=Cyc_Tcutil_compress(_tmp362);void*_tmp37A=_tmp379;_LL240: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp37B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37A;if(_tmp37B->tag != 1)goto _LL242;}_LL241:
 goto _LL243;_LL242: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp37C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp37A;if(_tmp37C->tag != 2)goto _LL244;}_LL243:
 return e;_LL244:;_LL245:
 return Cyc_Tcutil_rgns_of(_tmp362);_LL23F:;}_LL227:;_LL228:
# 1082
 return e;_LL222:;};}
# 1087
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp100D;struct Cyc_Absyn_FnInfo _tmp100C;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp100B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37D=(_tmp100B=_cycalloc(sizeof(*_tmp100B)),((_tmp100B[0]=((_tmp100D.tag=9,((_tmp100D.f1=((_tmp100C.tvars=0,((_tmp100C.effect=eff,((_tmp100C.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmp100C.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmp100C.args=0,((_tmp100C.c_varargs=0,((_tmp100C.cyc_varargs=0,((_tmp100C.rgn_po=0,((_tmp100C.attributes=0,((_tmp100C.requires_clause=0,((_tmp100C.requires_relns=0,((_tmp100C.ensures_clause=0,((_tmp100C.ensures_relns=0,_tmp100C)))))))))))))))))))))))))),_tmp100D)))),_tmp100B)));
# 1097
return Cyc_Absyn_atb_typ((void*)_tmp37D,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1104
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp381=Cyc_Tcutil_compress(e);void*_tmp382=_tmp381;void*_tmp384;struct Cyc_List_List*_tmp386;void*_tmp388;struct Cyc_Core_Opt*_tmp38A;void**_tmp38B;struct Cyc_Core_Opt*_tmp38C;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp383=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp382;if(_tmp383->tag != 23)goto _LL249;else{_tmp384=(void*)_tmp383->f1;}}_LL248:
# 1111
 if(constrain)return Cyc_Tcutil_unify(r,_tmp384);
_tmp384=Cyc_Tcutil_compress(_tmp384);
if(r == _tmp384)return 1;{
struct _tuple0 _tmp100E;struct _tuple0 _tmp38D=(_tmp100E.f1=r,((_tmp100E.f2=_tmp384,_tmp100E)));struct _tuple0 _tmp38E=_tmp38D;struct Cyc_Absyn_Tvar*_tmp390;struct Cyc_Absyn_Tvar*_tmp392;_LL252:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp38F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38E.f1;if(_tmp38F->tag != 2)goto _LL254;else{_tmp390=_tmp38F->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp391=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp38E.f2;if(_tmp391->tag != 2)goto _LL254;else{_tmp392=_tmp391->f1;}};_LL253:
 return Cyc_Absyn_tvar_cmp(_tmp390,_tmp392)== 0;_LL254:;_LL255:
 return 0;_LL251:;};_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp385=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp382;if(_tmp385->tag != 24)goto _LL24B;else{_tmp386=_tmp385->f1;}}_LL24A:
# 1119
 for(0;_tmp386 != 0;_tmp386=_tmp386->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp386->hd))return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp387=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp382;if(_tmp387->tag != 25)goto _LL24D;else{_tmp388=(void*)_tmp387->f1;}}_LL24C: {
# 1123
void*_tmp394=Cyc_Tcutil_rgns_of(_tmp388);void*_tmp395=_tmp394;void*_tmp397;void*_tmp398;_LL257: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp396=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp395;if(_tmp396->tag != 25)goto _LL259;else{_tmp397=(void*)_tmp396->f1;}}_LL258:
# 1125
 if(!constrain)return 0;{
void*_tmp399=Cyc_Tcutil_compress(_tmp397);void*_tmp39A=_tmp399;struct Cyc_Core_Opt*_tmp39C;void**_tmp39D;struct Cyc_Core_Opt*_tmp39E;_LL25C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp39B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp39A;if(_tmp39B->tag != 1)goto _LL25E;else{_tmp39C=_tmp39B->f1;_tmp39D=(void**)((void**)& _tmp39B->f2);_tmp39E=_tmp39B->f4;}}_LL25D: {
# 1130
void*_tmp39F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp39E);
# 1133
Cyc_Tcutil_occurs(_tmp39F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39E))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp101D;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp101C;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp101B;void*_tmp101A[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1019;void*_tmp3A0=Cyc_Tcutil_dummy_fntype((void*)((_tmp1019=_cycalloc(sizeof(*_tmp1019)),((_tmp1019[0]=((_tmp101D.tag=24,((_tmp101D.f1=((_tmp101A[1]=(void*)((_tmp101C=_cycalloc(sizeof(*_tmp101C)),((_tmp101C[0]=((_tmp101B.tag=23,((_tmp101B.f1=r,_tmp101B)))),_tmp101C)))),((_tmp101A[0]=_tmp39F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp101A,sizeof(void*),2)))))),_tmp101D)))),_tmp1019)))));
*_tmp39D=_tmp3A0;
return 1;};}_LL25E:;_LL25F:
 return 0;_LL25B:;};_LL259: _tmp398=_tmp395;_LL25A:
# 1139
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp398);_LL256:;}_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp382;if(_tmp389->tag != 1)goto _LL24F;else{_tmp38A=_tmp389->f1;_tmp38B=(void**)((void**)& _tmp389->f2);_tmp38C=_tmp389->f4;}}_LL24E:
# 1142
 if(_tmp38A == 0  || ((struct Cyc_Absyn_Kind*)_tmp38A->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1020;void*_tmp101F;(_tmp101F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1020="effect evar has wrong kind",_tag_dyneither(_tmp1020,sizeof(char),27))),_tag_dyneither(_tmp101F,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1147
void*_tmp3A8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp38C);
# 1150
Cyc_Tcutil_occurs(_tmp3A8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp38C))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1035;struct Cyc_List_List*_tmp1034;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1033;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1032;struct Cyc_List_List*_tmp1031;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1030;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A9=(_tmp1030=_cycalloc(sizeof(*_tmp1030)),((_tmp1030[0]=((_tmp1035.tag=24,((_tmp1035.f1=((_tmp1031=_cycalloc(sizeof(*_tmp1031)),((_tmp1031->hd=_tmp3A8,((_tmp1031->tl=((_tmp1034=_cycalloc(sizeof(*_tmp1034)),((_tmp1034->hd=(void*)((_tmp1032=_cycalloc(sizeof(*_tmp1032)),((_tmp1032[0]=((_tmp1033.tag=23,((_tmp1033.f1=r,_tmp1033)))),_tmp1032)))),((_tmp1034->tl=0,_tmp1034)))))),_tmp1031)))))),_tmp1035)))),_tmp1030)));
*_tmp38B=(void*)_tmp3A9;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1161
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp3B0=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp3B1=_tmp3B0;struct Cyc_List_List*_tmp3B4;void*_tmp3B6;struct Cyc_Core_Opt*_tmp3B8;void**_tmp3B9;struct Cyc_Core_Opt*_tmp3BA;_LL261: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3B2=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3B1;if(_tmp3B2->tag != 23)goto _LL263;}_LL262:
 return 0;_LL263: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3B3=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3B1;if(_tmp3B3->tag != 24)goto _LL265;else{_tmp3B4=_tmp3B3->f1;}}_LL264:
# 1166
 for(0;_tmp3B4 != 0;_tmp3B4=_tmp3B4->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp3B4->hd))
return 1;}
return 0;_LL265: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B1;if(_tmp3B5->tag != 25)goto _LL267;else{_tmp3B6=(void*)_tmp3B5->f1;}}_LL266:
# 1171
 _tmp3B6=Cyc_Tcutil_compress(_tmp3B6);
if(t == _tmp3B6)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp3B6);{
void*_tmp3BB=Cyc_Tcutil_rgns_of(t);void*_tmp3BC=_tmp3BB;void*_tmp3BE;void*_tmp3BF;_LL26C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3BD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3BC;if(_tmp3BD->tag != 25)goto _LL26E;else{_tmp3BE=(void*)_tmp3BD->f1;}}_LL26D: {
# 1176
struct _tuple0 _tmp1036;struct _tuple0 _tmp3C0=(_tmp1036.f1=Cyc_Tcutil_compress(_tmp3BE),((_tmp1036.f2=_tmp3B6,_tmp1036)));struct _tuple0 _tmp3C1=_tmp3C0;struct Cyc_Absyn_Tvar*_tmp3C3;struct Cyc_Absyn_Tvar*_tmp3C5;_LL271:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C1.f1;if(_tmp3C2->tag != 2)goto _LL273;else{_tmp3C3=_tmp3C2->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3C4=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3C1.f2;if(_tmp3C4->tag != 2)goto _LL273;else{_tmp3C5=_tmp3C4->f1;}};_LL272:
 return Cyc_Tcutil_unify(t,_tmp3B6);_LL273:;_LL274:
 return _tmp3BE == _tmp3B6;_LL270:;}_LL26E: _tmp3BF=_tmp3BC;_LL26F:
# 1180
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp3BF);_LL26B:;};_LL267: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3B7=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B1;if(_tmp3B7->tag != 1)goto _LL269;else{_tmp3B8=_tmp3B7->f1;_tmp3B9=(void**)((void**)& _tmp3B7->f2);_tmp3BA=_tmp3B7->f4;}}_LL268:
# 1183
 if(_tmp3B8 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3B8->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1039;void*_tmp1038;(_tmp1038=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1039="effect evar has wrong kind",_tag_dyneither(_tmp1039,sizeof(char),27))),_tag_dyneither(_tmp1038,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1188
void*_tmp3C9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3BA);
# 1191
Cyc_Tcutil_occurs(_tmp3C9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BA))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp104E;struct Cyc_List_List*_tmp104D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp104C;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp104B;struct Cyc_List_List*_tmp104A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1049;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3CA=(_tmp1049=_cycalloc(sizeof(*_tmp1049)),((_tmp1049[0]=((_tmp104E.tag=24,((_tmp104E.f1=((_tmp104A=_cycalloc(sizeof(*_tmp104A)),((_tmp104A->hd=_tmp3C9,((_tmp104A->tl=((_tmp104D=_cycalloc(sizeof(*_tmp104D)),((_tmp104D->hd=(void*)((_tmp104B=_cycalloc(sizeof(*_tmp104B)),((_tmp104B[0]=((_tmp104C.tag=25,((_tmp104C.f1=t,_tmp104C)))),_tmp104B)))),((_tmp104D->tl=0,_tmp104D)))))),_tmp104A)))))),_tmp104E)))),_tmp1049)));
*_tmp3B9=(void*)_tmp3CA;
return 1;};};_LL269:;_LL26A:
 return 0;_LL260:;};}
# 1202
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3D1=e;struct Cyc_Absyn_Tvar*_tmp3D3;struct Cyc_List_List*_tmp3D5;void*_tmp3D7;struct Cyc_Core_Opt*_tmp3D9;void**_tmp3DA;struct Cyc_Core_Opt*_tmp3DB;_LL276: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3D1;if(_tmp3D2->tag != 2)goto _LL278;else{_tmp3D3=_tmp3D2->f1;}}_LL277:
 return Cyc_Absyn_tvar_cmp(v,_tmp3D3)== 0;_LL278: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3D1;if(_tmp3D4->tag != 24)goto _LL27A;else{_tmp3D5=_tmp3D4->f1;}}_LL279:
# 1207
 for(0;_tmp3D5 != 0;_tmp3D5=_tmp3D5->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3D5->hd))
return 1;}
return 0;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D6=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D1;if(_tmp3D6->tag != 25)goto _LL27C;else{_tmp3D7=(void*)_tmp3D6->f1;}}_LL27B: {
# 1212
void*_tmp3DC=Cyc_Tcutil_rgns_of(_tmp3D7);void*_tmp3DD=_tmp3DC;void*_tmp3DF;void*_tmp3E0;_LL281: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DD;if(_tmp3DE->tag != 25)goto _LL283;else{_tmp3DF=(void*)_tmp3DE->f1;}}_LL282:
# 1214
 if(!may_constrain_evars)return 0;{
void*_tmp3E1=Cyc_Tcutil_compress(_tmp3DF);void*_tmp3E2=_tmp3E1;struct Cyc_Core_Opt*_tmp3E4;void**_tmp3E5;struct Cyc_Core_Opt*_tmp3E6;_LL286: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3E2;if(_tmp3E3->tag != 1)goto _LL288;else{_tmp3E4=_tmp3E3->f1;_tmp3E5=(void**)((void**)& _tmp3E3->f2);_tmp3E6=_tmp3E3->f4;}}_LL287: {
# 1220
void*_tmp3E7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3E6);
# 1222
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3E6))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp105D;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp105C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp105B;void*_tmp105A[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1059;void*_tmp3E8=Cyc_Tcutil_dummy_fntype((void*)((_tmp1059=_cycalloc(sizeof(*_tmp1059)),((_tmp1059[0]=((_tmp105D.tag=24,((_tmp105D.f1=((_tmp105A[1]=(void*)((_tmp105C=_cycalloc(sizeof(*_tmp105C)),((_tmp105C[0]=((_tmp105B.tag=2,((_tmp105B.f1=v,_tmp105B)))),_tmp105C)))),((_tmp105A[0]=_tmp3E7,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp105A,sizeof(void*),2)))))),_tmp105D)))),_tmp1059)))));
*_tmp3E5=_tmp3E8;
return 1;};}_LL288:;_LL289:
 return 0;_LL285:;};_LL283: _tmp3E0=_tmp3DD;_LL284:
# 1228
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3E0);_LL280:;}_LL27C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3D1;if(_tmp3D8->tag != 1)goto _LL27E;else{_tmp3D9=_tmp3D8->f1;_tmp3DA=(void**)((void**)& _tmp3D8->f2);_tmp3DB=_tmp3D8->f4;}}_LL27D:
# 1231
 if(_tmp3D9 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3D9->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1060;void*_tmp105F;(_tmp105F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1060="effect evar has wrong kind",_tag_dyneither(_tmp1060,sizeof(char),27))),_tag_dyneither(_tmp105F,sizeof(void*),0)));}{
# 1235
void*_tmp3F0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3DB);
# 1237
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3DB))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1075;struct Cyc_List_List*_tmp1074;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1073;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1072;struct Cyc_List_List*_tmp1071;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1070;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3F1=(_tmp1070=_cycalloc(sizeof(*_tmp1070)),((_tmp1070[0]=((_tmp1075.tag=24,((_tmp1075.f1=((_tmp1071=_cycalloc(sizeof(*_tmp1071)),((_tmp1071->hd=_tmp3F0,((_tmp1071->tl=((_tmp1074=_cycalloc(sizeof(*_tmp1074)),((_tmp1074->hd=(void*)((_tmp1072=_cycalloc(sizeof(*_tmp1072)),((_tmp1072[0]=((_tmp1073.tag=2,((_tmp1073.f1=v,_tmp1073)))),_tmp1072)))),((_tmp1074->tl=0,_tmp1074)))))),_tmp1071)))))),_tmp1075)))),_tmp1070)));
*_tmp3DA=(void*)_tmp3F1;
return 1;};};_LL27E:;_LL27F:
 return 0;_LL275:;};}
# 1247
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3F8=e;struct Cyc_List_List*_tmp3FA;void*_tmp3FC;_LL28B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3F9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F8;if(_tmp3F9->tag != 24)goto _LL28D;else{_tmp3FA=_tmp3F9->f1;}}_LL28C:
# 1251
 for(0;_tmp3FA != 0;_tmp3FA=_tmp3FA->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3FA->hd))
return 1;}
return 0;_LL28D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3FB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3F8;if(_tmp3FB->tag != 25)goto _LL28F;else{_tmp3FC=(void*)_tmp3FB->f1;}}_LL28E: {
# 1256
void*_tmp3FE=Cyc_Tcutil_rgns_of(_tmp3FC);void*_tmp3FF=_tmp3FE;void*_tmp401;void*_tmp402;_LL294: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp400=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3FF;if(_tmp400->tag != 25)goto _LL296;else{_tmp401=(void*)_tmp400->f1;}}_LL295:
 return 0;_LL296: _tmp402=_tmp3FF;_LL297:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp402);_LL293:;}_LL28F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3FD=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F8;if(_tmp3FD->tag != 1)goto _LL291;}_LL290:
# 1260
 return evar == e;_LL291:;_LL292:
 return 0;_LL28A:;};}
# 1274 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1279
void*_tmp403=Cyc_Tcutil_compress(e1);void*_tmp404=_tmp403;struct Cyc_List_List*_tmp406;void*_tmp408;struct Cyc_Absyn_Tvar*_tmp40A;void*_tmp40C;void**_tmp40E;struct Cyc_Core_Opt*_tmp40F;_LL299: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp405=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp404;if(_tmp405->tag != 24)goto _LL29B;else{_tmp406=_tmp405->f1;}}_LL29A:
# 1281
 for(0;_tmp406 != 0;_tmp406=_tmp406->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp406->hd,e2))
return 0;}
return 1;_LL29B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp407=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp404;if(_tmp407->tag != 23)goto _LL29D;else{_tmp408=(void*)_tmp407->f1;}}_LL29C:
# 1292
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp408,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp408,(void*)& Cyc_Absyn_HeapRgn_val);_LL29D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp409=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp404;if(_tmp409->tag != 2)goto _LL29F;else{_tmp40A=_tmp409->f1;}}_LL29E:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp40A,e2);_LL29F: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp40B=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp404;if(_tmp40B->tag != 25)goto _LL2A1;else{_tmp40C=(void*)_tmp40B->f1;}}_LL2A0: {
# 1296
void*_tmp410=Cyc_Tcutil_rgns_of(_tmp40C);void*_tmp411=_tmp410;void*_tmp413;void*_tmp414;_LL2A6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp412=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp411;if(_tmp412->tag != 25)goto _LL2A8;else{_tmp413=(void*)_tmp412->f1;}}_LL2A7:
# 1301
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp413,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp413,Cyc_Absyn_sint_typ);_LL2A8: _tmp414=_tmp411;_LL2A9:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp414,e2);_LL2A5:;}_LL2A1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp404;if(_tmp40D->tag != 1)goto _LL2A3;else{_tmp40E=(void**)((void**)& _tmp40D->f2);_tmp40F=_tmp40D->f4;}}_LL2A2:
# 1306
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1310
*_tmp40E=Cyc_Absyn_empty_effect;
# 1313
return 1;}else{
# 1315
return 0;}_LL2A3:;_LL2A4: {
const char*_tmp1079;void*_tmp1078[1];struct Cyc_String_pa_PrintArg_struct _tmp1077;(_tmp1077.tag=0,((_tmp1077.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1078[0]=& _tmp1077,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1079="subset_effect: bad effect: %s",_tag_dyneither(_tmp1079,sizeof(char),30))),_tag_dyneither(_tmp1078,sizeof(void*),1)))))));}_LL298:;}
# 1331 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1347
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1349
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp418=(struct _tuple0*)r1->hd;void*_tmp41A;void*_tmp41B;struct _tuple0*_tmp419=_tmp418;_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;{
int found=_tmp41A == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp41C=(struct _tuple0*)r2->hd;void*_tmp41E;void*_tmp41F;struct _tuple0*_tmp41D=_tmp41C;_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;
if(Cyc_Tcutil_unify(_tmp41A,_tmp41E) && Cyc_Tcutil_unify(_tmp41B,_tmp41F)){
found=1;
break;}}}
# 1359
if(!found)return 0;};}}
# 1361
return 1;}
# 1364
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1368
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp420=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp107A;struct Cyc_List_List*_tmp421=(_tmp107A=_cycalloc(sizeof(*_tmp107A)),((_tmp107A->hd=_tmp420,((_tmp107A->tl=r1,_tmp107A)))));
if(Cyc_Relations_consistent_relations(_tmp421))return 0;}
# 1374
return 1;}
# 1379
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1385
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1387
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1391
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp423=t1;struct Cyc_Core_Opt*_tmp425;void**_tmp426;struct Cyc_Core_Opt*_tmp427;_LL2AB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp423;if(_tmp424->tag != 1)goto _LL2AD;else{_tmp425=_tmp424->f1;_tmp426=(void**)((void**)& _tmp424->f2);_tmp427=_tmp424->f4;}}_LL2AC:
# 1402
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp427))->v,t2);{
struct Cyc_Absyn_Kind*_tmp428=Cyc_Tcutil_typ_kind(t2);
# 1407
if(Cyc_Tcutil_kind_leq(_tmp428,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp425))->v)){
*_tmp426=t2;
return;}else{
# 1411
{void*_tmp429=t2;void**_tmp42B;struct Cyc_Core_Opt*_tmp42C;struct Cyc_Absyn_PtrInfo _tmp42E;_LL2B0: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp42A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp429;if(_tmp42A->tag != 1)goto _LL2B2;else{_tmp42B=(void**)((void**)& _tmp42A->f2);_tmp42C=_tmp42A->f4;}}_LL2B1: {
# 1414
struct Cyc_List_List*_tmp42F=(struct Cyc_List_List*)_tmp427->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp42C))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp42F,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp107B;Cyc_Tcutil_failure_reason=((_tmp107B="(type variable would escape scope)",_tag_dyneither(_tmp107B,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1421
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp425->v,_tmp428)){
*_tmp42B=t1;return;}
# 1424
{const char*_tmp107C;Cyc_Tcutil_failure_reason=((_tmp107C="(kinds are incompatible)",_tag_dyneither(_tmp107C,sizeof(char),25)));}
goto _LL2AF;}_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp429;if(_tmp42D->tag != 5)goto _LL2B4;else{_tmp42E=_tmp42D->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp425->v)->kind == Cyc_Absyn_BoxKind))goto _LL2B4;_LL2B3: {
# 1430
union Cyc_Absyn_Constraint*_tmp432=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp42E.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp433=_tmp432;_LL2B7: if((_tmp433->No_constr).tag != 3)goto _LL2B9;_LL2B8:
# 1434
{struct _union_Constraint_Eq_constr*_tmp107D;(_tmp107D=& _tmp432->Eq_constr,((_tmp107D->tag=1,_tmp107D->val=Cyc_Absyn_bounds_one)));}
*_tmp426=t2;
return;_LL2B9:;_LL2BA:
 goto _LL2B6;_LL2B6:;}
# 1439
goto _LL2AF;}_LL2B4:;_LL2B5:
 goto _LL2AF;_LL2AF:;}
# 1442
{const char*_tmp107E;Cyc_Tcutil_failure_reason=((_tmp107E="(kinds are incompatible)",_tag_dyneither(_tmp107E,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL2AD:;_LL2AE:
# 1445
 goto _LL2AA;_LL2AA:;}
# 1450
{struct _tuple0 _tmp107F;struct _tuple0 _tmp436=(_tmp107F.f1=t2,((_tmp107F.f2=t1,_tmp107F)));struct _tuple0 _tmp437=_tmp436;struct Cyc_Absyn_Tvar*_tmp43C;struct Cyc_Absyn_Tvar*_tmp43E;union Cyc_Absyn_AggrInfoU _tmp440;struct Cyc_List_List*_tmp441;union Cyc_Absyn_AggrInfoU _tmp443;struct Cyc_List_List*_tmp444;struct _tuple2*_tmp446;struct _tuple2*_tmp448;struct Cyc_List_List*_tmp44A;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_Datatypedecl*_tmp44E;struct Cyc_List_List*_tmp44F;struct Cyc_Absyn_Datatypedecl*_tmp451;struct Cyc_List_List*_tmp452;struct Cyc_Absyn_Datatypedecl*_tmp454;struct Cyc_Absyn_Datatypefield*_tmp455;struct Cyc_List_List*_tmp456;struct Cyc_Absyn_Datatypedecl*_tmp458;struct Cyc_Absyn_Datatypefield*_tmp459;struct Cyc_List_List*_tmp45A;void*_tmp45C;struct Cyc_Absyn_Tqual _tmp45D;void*_tmp45E;union Cyc_Absyn_Constraint*_tmp45F;union Cyc_Absyn_Constraint*_tmp460;union Cyc_Absyn_Constraint*_tmp461;void*_tmp463;struct Cyc_Absyn_Tqual _tmp464;void*_tmp465;union Cyc_Absyn_Constraint*_tmp466;union Cyc_Absyn_Constraint*_tmp467;union Cyc_Absyn_Constraint*_tmp468;enum Cyc_Absyn_Sign _tmp46A;enum Cyc_Absyn_Size_of _tmp46B;enum Cyc_Absyn_Sign _tmp46D;enum Cyc_Absyn_Size_of _tmp46E;int _tmp470;int _tmp472;void*_tmp474;void*_tmp476;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Exp*_tmp47A;void*_tmp47C;struct Cyc_Absyn_Tqual _tmp47D;struct Cyc_Absyn_Exp*_tmp47E;union Cyc_Absyn_Constraint*_tmp47F;void*_tmp481;struct Cyc_Absyn_Tqual _tmp482;struct Cyc_Absyn_Exp*_tmp483;union Cyc_Absyn_Constraint*_tmp484;struct Cyc_List_List*_tmp486;void*_tmp487;struct Cyc_Absyn_Tqual _tmp488;void*_tmp489;struct Cyc_List_List*_tmp48A;int _tmp48B;struct Cyc_Absyn_VarargInfo*_tmp48C;struct Cyc_List_List*_tmp48D;struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp491;struct Cyc_List_List*_tmp492;struct Cyc_List_List*_tmp494;void*_tmp495;struct Cyc_Absyn_Tqual _tmp496;void*_tmp497;struct Cyc_List_List*_tmp498;int _tmp499;struct Cyc_Absyn_VarargInfo*_tmp49A;struct Cyc_List_List*_tmp49B;struct Cyc_List_List*_tmp49C;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_List_List*_tmp49E;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_List_List*_tmp4A0;struct Cyc_List_List*_tmp4A2;struct Cyc_List_List*_tmp4A4;enum Cyc_Absyn_AggrKind _tmp4A6;struct Cyc_List_List*_tmp4A7;enum Cyc_Absyn_AggrKind _tmp4A9;struct Cyc_List_List*_tmp4AA;struct Cyc_List_List*_tmp4AC;struct Cyc_Absyn_Typedefdecl*_tmp4AD;struct Cyc_List_List*_tmp4AF;struct Cyc_Absyn_Typedefdecl*_tmp4B0;void*_tmp4B8;void*_tmp4BA;void*_tmp4BC;void*_tmp4BD;void*_tmp4BF;void*_tmp4C0;_LL2BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp438=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp437.f1;if(_tmp438->tag != 1)goto _LL2BE;}_LL2BD:
# 1453
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2BE:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp439=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp437.f1;if(_tmp439->tag != 0)goto _LL2C0;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp43A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp437.f2;if(_tmp43A->tag != 0)goto _LL2C0;};_LL2BF:
# 1456
 return;_LL2C0:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp437.f1;if(_tmp43B->tag != 2)goto _LL2C2;else{_tmp43C=_tmp43B->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp43D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp437.f2;if(_tmp43D->tag != 2)goto _LL2C2;else{_tmp43E=_tmp43D->f1;}};_LL2C1: {
# 1459
struct _dyneither_ptr*_tmp4C7=_tmp43C->name;
struct _dyneither_ptr*_tmp4C8=_tmp43E->name;
# 1462
int _tmp4C9=_tmp43C->identity;
int _tmp4CA=_tmp43E->identity;
if(_tmp4CA == _tmp4C9)return;
{const char*_tmp1080;Cyc_Tcutil_failure_reason=((_tmp1080="(variable types are not the same)",_tag_dyneither(_tmp1080,sizeof(char),34)));}
goto _LL2BB;}_LL2C2:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp43F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp437.f1;if(_tmp43F->tag != 11)goto _LL2C4;else{_tmp440=(_tmp43F->f1).aggr_info;_tmp441=(_tmp43F->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp442=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp437.f2;if(_tmp442->tag != 11)goto _LL2C4;else{_tmp443=(_tmp442->f1).aggr_info;_tmp444=(_tmp442->f1).targs;}};_LL2C3: {
# 1469
struct _tuple13 _tmp4CC=Cyc_Absyn_aggr_kinded_name(_tmp443);enum Cyc_Absyn_AggrKind _tmp4CE;struct _tuple2*_tmp4CF;struct _tuple13 _tmp4CD=_tmp4CC;_tmp4CE=_tmp4CD.f1;_tmp4CF=_tmp4CD.f2;{
struct _tuple13 _tmp4D0=Cyc_Absyn_aggr_kinded_name(_tmp440);enum Cyc_Absyn_AggrKind _tmp4D2;struct _tuple2*_tmp4D3;struct _tuple13 _tmp4D1=_tmp4D0;_tmp4D2=_tmp4D1.f1;_tmp4D3=_tmp4D1.f2;
if(_tmp4CE != _tmp4D2){{const char*_tmp1081;Cyc_Tcutil_failure_reason=((_tmp1081="(struct and union type)",_tag_dyneither(_tmp1081,sizeof(char),24)));}goto _LL2BB;}
if(Cyc_Absyn_qvar_cmp(_tmp4CF,_tmp4D3)!= 0){{const char*_tmp1082;Cyc_Tcutil_failure_reason=((_tmp1082="(different type name)",_tag_dyneither(_tmp1082,sizeof(char),22)));}goto _LL2BB;}
Cyc_Tcutil_unify_list(_tmp444,_tmp441);
return;};}_LL2C4:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp445=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp437.f1;if(_tmp445->tag != 13)goto _LL2C6;else{_tmp446=_tmp445->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp437.f2;if(_tmp447->tag != 13)goto _LL2C6;else{_tmp448=_tmp447->f1;}};_LL2C5:
# 1478
 if(Cyc_Absyn_qvar_cmp(_tmp446,_tmp448)== 0)return;
{const char*_tmp1083;Cyc_Tcutil_failure_reason=((_tmp1083="(different enum types)",_tag_dyneither(_tmp1083,sizeof(char),23)));}
goto _LL2BB;_LL2C6:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp437.f1;if(_tmp449->tag != 14)goto _LL2C8;else{_tmp44A=_tmp449->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp437.f2;if(_tmp44B->tag != 14)goto _LL2C8;else{_tmp44C=_tmp44B->f1;}};_LL2C7: {
# 1483
int bad=0;
for(0;_tmp44A != 0  && _tmp44C != 0;(_tmp44A=_tmp44A->tl,_tmp44C=_tmp44C->tl)){
struct Cyc_Absyn_Enumfield*_tmp4D7=(struct Cyc_Absyn_Enumfield*)_tmp44A->hd;
struct Cyc_Absyn_Enumfield*_tmp4D8=(struct Cyc_Absyn_Enumfield*)_tmp44C->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4D7->name,_tmp4D8->name)!= 0){
{const char*_tmp1084;Cyc_Tcutil_failure_reason=((_tmp1084="(different names for enum fields)",_tag_dyneither(_tmp1084,sizeof(char),34)));}
bad=1;
break;}
# 1492
if(_tmp4D7->tag == _tmp4D8->tag)continue;
if(_tmp4D7->tag == 0  || _tmp4D8->tag == 0){
{const char*_tmp1085;Cyc_Tcutil_failure_reason=((_tmp1085="(different tag values for enum fields)",_tag_dyneither(_tmp1085,sizeof(char),39)));}
bad=1;
break;}
# 1498
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4D7->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4D8->tag))){
{const char*_tmp1086;Cyc_Tcutil_failure_reason=((_tmp1086="(different tag values for enum fields)",_tag_dyneither(_tmp1086,sizeof(char),39)));}
bad=1;
break;}}
# 1504
if(bad)goto _LL2BB;
if(_tmp44A == 0  && _tmp44C == 0)return;
{const char*_tmp1087;Cyc_Tcutil_failure_reason=((_tmp1087="(different number of fields for enums)",_tag_dyneither(_tmp1087,sizeof(char),39)));}
goto _LL2BB;}_LL2C8:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp437.f1;if(_tmp44D->tag != 3)goto _LL2CA;else{if((((_tmp44D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2CA;_tmp44E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp44D->f1).datatype_info).KnownDatatype).val);_tmp44F=(_tmp44D->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp437.f2;if(_tmp450->tag != 3)goto _LL2CA;else{if((((_tmp450->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2CA;_tmp451=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp450->f1).datatype_info).KnownDatatype).val);_tmp452=(_tmp450->f1).targs;}};_LL2C9:
# 1511
 if(_tmp44E == _tmp451  || Cyc_Absyn_qvar_cmp(_tmp44E->name,_tmp451->name)== 0){
Cyc_Tcutil_unify_list(_tmp452,_tmp44F);
return;}
# 1515
goto _LL2BB;_LL2CA:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp437.f1;if(_tmp453->tag != 4)goto _LL2CC;else{if((((_tmp453->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2CC;_tmp454=((struct _tuple3)(((_tmp453->f1).field_info).KnownDatatypefield).val).f1;_tmp455=((struct _tuple3)(((_tmp453->f1).field_info).KnownDatatypefield).val).f2;_tmp456=(_tmp453->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp457=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp437.f2;if(_tmp457->tag != 4)goto _LL2CC;else{if((((_tmp457->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2CC;_tmp458=((struct _tuple3)(((_tmp457->f1).field_info).KnownDatatypefield).val).f1;_tmp459=((struct _tuple3)(((_tmp457->f1).field_info).KnownDatatypefield).val).f2;_tmp45A=(_tmp457->f1).targs;}};_LL2CB:
# 1519
 if((_tmp454 == _tmp458  || Cyc_Absyn_qvar_cmp(_tmp454->name,_tmp458->name)== 0) && (
_tmp455 == _tmp459  || Cyc_Absyn_qvar_cmp(_tmp455->name,_tmp459->name)== 0)){
Cyc_Tcutil_unify_list(_tmp45A,_tmp456);
return;}
# 1524
{const char*_tmp1088;Cyc_Tcutil_failure_reason=((_tmp1088="(different datatype field types)",_tag_dyneither(_tmp1088,sizeof(char),33)));}
goto _LL2BB;_LL2CC:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437.f1;if(_tmp45B->tag != 5)goto _LL2CE;else{_tmp45C=(_tmp45B->f1).elt_typ;_tmp45D=(_tmp45B->f1).elt_tq;_tmp45E=((_tmp45B->f1).ptr_atts).rgn;_tmp45F=((_tmp45B->f1).ptr_atts).nullable;_tmp460=((_tmp45B->f1).ptr_atts).bounds;_tmp461=((_tmp45B->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp437.f2;if(_tmp462->tag != 5)goto _LL2CE;else{_tmp463=(_tmp462->f1).elt_typ;_tmp464=(_tmp462->f1).elt_tq;_tmp465=((_tmp462->f1).ptr_atts).rgn;_tmp466=((_tmp462->f1).ptr_atts).nullable;_tmp467=((_tmp462->f1).ptr_atts).bounds;_tmp468=((_tmp462->f1).ptr_atts).zero_term;}};_LL2CD:
# 1529
 Cyc_Tcutil_unify_it(_tmp463,_tmp45C);
Cyc_Tcutil_unify_it(_tmp45E,_tmp465);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1089;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp468,_tmp461,((_tmp1089="(not both zero terminated)",_tag_dyneither(_tmp1089,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp464,_tmp463,_tmp45D,_tmp45C);
{const char*_tmp108A;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp467,_tmp460,((_tmp108A="(different pointer bounds)",_tag_dyneither(_tmp108A,sizeof(char),27))));}
# 1537
{void*_tmp4E0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp467);void*_tmp4E1=_tmp4E0;_LL2F9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4E2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4E1;if(_tmp4E2->tag != 0)goto _LL2FB;}_LL2FA:
 return;_LL2FB:;_LL2FC:
 goto _LL2F8;_LL2F8:;}
# 1541
{const char*_tmp108B;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp466,_tmp45F,((_tmp108B="(incompatible pointer types)",_tag_dyneither(_tmp108B,sizeof(char),29))));}
return;_LL2CE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f1;if(_tmp469->tag != 6)goto _LL2D0;else{_tmp46A=_tmp469->f1;_tmp46B=_tmp469->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp46C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp437.f2;if(_tmp46C->tag != 6)goto _LL2D0;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;}};_LL2CF:
# 1546
 if(_tmp46D == _tmp46A  && ((_tmp46E == _tmp46B  || _tmp46E == Cyc_Absyn_Int_sz  && _tmp46B == Cyc_Absyn_Long_sz) || 
_tmp46E == Cyc_Absyn_Long_sz  && _tmp46B == Cyc_Absyn_Int_sz))return;
{const char*_tmp108C;Cyc_Tcutil_failure_reason=((_tmp108C="(different integral types)",_tag_dyneither(_tmp108C,sizeof(char),27)));}
goto _LL2BB;_LL2D0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp437.f1;if(_tmp46F->tag != 7)goto _LL2D2;else{_tmp470=_tmp46F->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp437.f2;if(_tmp471->tag != 7)goto _LL2D2;else{_tmp472=_tmp471->f1;}};_LL2D1:
# 1552
 if(_tmp472 == 0  && _tmp470 == 0)return;else{
if(_tmp472 == 1  && _tmp470 == 1)return;else{
# 1555
if(((_tmp472 != 0  && _tmp472 != 1) && _tmp470 != 0) && _tmp470 != 1)return;}}
goto _LL2BB;_LL2D2:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp473=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp437.f1;if(_tmp473->tag != 19)goto _LL2D4;else{_tmp474=(void*)_tmp473->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp475=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp437.f2;if(_tmp475->tag != 19)goto _LL2D4;else{_tmp476=(void*)_tmp475->f1;}};_LL2D3:
# 1558
 Cyc_Tcutil_unify_it(_tmp474,_tmp476);return;_LL2D4:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp437.f1;if(_tmp477->tag != 18)goto _LL2D6;else{_tmp478=_tmp477->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp479=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp437.f2;if(_tmp479->tag != 18)goto _LL2D6;else{_tmp47A=_tmp479->f1;}};_LL2D5:
# 1561
 if(!Cyc_Evexp_same_const_exp(_tmp478,_tmp47A)){
{const char*_tmp108D;Cyc_Tcutil_failure_reason=((_tmp108D="(cannot prove expressions are the same)",_tag_dyneither(_tmp108D,sizeof(char),40)));}
goto _LL2BB;}
# 1565
return;_LL2D6:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp437.f1;if(_tmp47B->tag != 8)goto _LL2D8;else{_tmp47C=(_tmp47B->f1).elt_type;_tmp47D=(_tmp47B->f1).tq;_tmp47E=(_tmp47B->f1).num_elts;_tmp47F=(_tmp47B->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp437.f2;if(_tmp480->tag != 8)goto _LL2D8;else{_tmp481=(_tmp480->f1).elt_type;_tmp482=(_tmp480->f1).tq;_tmp483=(_tmp480->f1).num_elts;_tmp484=(_tmp480->f1).zero_term;}};_LL2D7:
# 1569
 Cyc_Tcutil_unify_it(_tmp481,_tmp47C);
Cyc_Tcutil_unify_tqual(_tmp482,_tmp481,_tmp47D,_tmp47C);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp108E;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp47F,_tmp484,((_tmp108E="(not both zero terminated)",_tag_dyneither(_tmp108E,sizeof(char),27))));}
if(_tmp47E == _tmp483)return;
if(_tmp47E == 0  || _tmp483 == 0)goto _LL2BB;
if(Cyc_Evexp_same_const_exp(_tmp47E,_tmp483))
return;
{const char*_tmp108F;Cyc_Tcutil_failure_reason=((_tmp108F="(different array sizes)",_tag_dyneither(_tmp108F,sizeof(char),24)));}
goto _LL2BB;_LL2D8:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437.f1;if(_tmp485->tag != 9)goto _LL2DA;else{_tmp486=(_tmp485->f1).tvars;_tmp487=(_tmp485->f1).effect;_tmp488=(_tmp485->f1).ret_tqual;_tmp489=(_tmp485->f1).ret_typ;_tmp48A=(_tmp485->f1).args;_tmp48B=(_tmp485->f1).c_varargs;_tmp48C=(_tmp485->f1).cyc_varargs;_tmp48D=(_tmp485->f1).rgn_po;_tmp48E=(_tmp485->f1).attributes;_tmp48F=(_tmp485->f1).requires_clause;_tmp490=(_tmp485->f1).requires_relns;_tmp491=(_tmp485->f1).ensures_clause;_tmp492=(_tmp485->f1).ensures_relns;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp493=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp437.f2;if(_tmp493->tag != 9)goto _LL2DA;else{_tmp494=(_tmp493->f1).tvars;_tmp495=(_tmp493->f1).effect;_tmp496=(_tmp493->f1).ret_tqual;_tmp497=(_tmp493->f1).ret_typ;_tmp498=(_tmp493->f1).args;_tmp499=(_tmp493->f1).c_varargs;_tmp49A=(_tmp493->f1).cyc_varargs;_tmp49B=(_tmp493->f1).rgn_po;_tmp49C=(_tmp493->f1).attributes;_tmp49D=(_tmp493->f1).requires_clause;_tmp49E=(_tmp493->f1).requires_relns;_tmp49F=(_tmp493->f1).ensures_clause;_tmp4A0=(_tmp493->f1).ensures_relns;}};_LL2D9: {
# 1583
int done=0;
struct _RegionHandle _tmp4E8=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4E8;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp494 != 0){
if(_tmp486 == 0){
{const char*_tmp1090;Cyc_Tcutil_failure_reason=((_tmp1090="(second function type has too few type variables)",_tag_dyneither(_tmp1090,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1591
void*_tmp4EA=((struct Cyc_Absyn_Tvar*)_tmp494->hd)->kind;
void*_tmp4EB=((struct Cyc_Absyn_Tvar*)_tmp486->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4EA,_tmp4EB)){
{const char*_tmp1096;void*_tmp1095[3];struct Cyc_String_pa_PrintArg_struct _tmp1094;struct Cyc_String_pa_PrintArg_struct _tmp1093;struct Cyc_String_pa_PrintArg_struct _tmp1092;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp1092.tag=0,((_tmp1092.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1597
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp486->hd,& Cyc_Tcutil_bk))),((_tmp1093.tag=0,((_tmp1093.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1596
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp494->hd,& Cyc_Tcutil_bk))),((_tmp1094.tag=0,((_tmp1094.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1595
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp494->hd)),((_tmp1095[0]=& _tmp1094,((_tmp1095[1]=& _tmp1093,((_tmp1095[2]=& _tmp1092,Cyc_aprintf(((_tmp1096="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1096,sizeof(char),44))),_tag_dyneither(_tmp1095,sizeof(void*),3))))))))))))))))))));}
# 1598
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1600
{struct _tuple16*_tmp10A0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp109F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp109E;struct Cyc_List_List*_tmp109D;inst=((_tmp109D=_region_malloc(rgn,sizeof(*_tmp109D)),((_tmp109D->hd=((_tmp10A0=_region_malloc(rgn,sizeof(*_tmp10A0)),((_tmp10A0->f1=(struct Cyc_Absyn_Tvar*)_tmp486->hd,((_tmp10A0->f2=(void*)((_tmp109E=_cycalloc(sizeof(*_tmp109E)),((_tmp109E[0]=((_tmp109F.tag=2,((_tmp109F.f1=(struct Cyc_Absyn_Tvar*)_tmp494->hd,_tmp109F)))),_tmp109E)))),_tmp10A0)))))),((_tmp109D->tl=inst,_tmp109D))))));}
_tmp494=_tmp494->tl;
_tmp486=_tmp486->tl;};}
# 1604
if(_tmp486 != 0){
{const char*_tmp10A1;Cyc_Tcutil_failure_reason=((_tmp10A1="(second function type has too many type variables)",_tag_dyneither(_tmp10A1,sizeof(char),51)));}
_npop_handler(0);goto _LL2BB;}
# 1608
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10AD;struct Cyc_Absyn_FnInfo _tmp10AC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10AB;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10A7;struct Cyc_Absyn_FnInfo _tmp10A6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10A5;Cyc_Tcutil_unify_it((void*)((_tmp10A5=_cycalloc(sizeof(*_tmp10A5)),((_tmp10A5[0]=((_tmp10A7.tag=9,((_tmp10A7.f1=((_tmp10A6.tvars=0,((_tmp10A6.effect=_tmp495,((_tmp10A6.ret_tqual=_tmp496,((_tmp10A6.ret_typ=_tmp497,((_tmp10A6.args=_tmp498,((_tmp10A6.c_varargs=_tmp499,((_tmp10A6.cyc_varargs=_tmp49A,((_tmp10A6.rgn_po=_tmp49B,((_tmp10A6.attributes=_tmp49C,((_tmp10A6.requires_clause=_tmp48F,((_tmp10A6.requires_relns=_tmp490,((_tmp10A6.ensures_clause=_tmp491,((_tmp10A6.ensures_relns=_tmp492,_tmp10A6)))))))))))))))))))))))))),_tmp10A7)))),_tmp10A5)))),
# 1612
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp10AB=_cycalloc(sizeof(*_tmp10AB)),((_tmp10AB[0]=((_tmp10AD.tag=9,((_tmp10AD.f1=((_tmp10AC.tvars=0,((_tmp10AC.effect=_tmp487,((_tmp10AC.ret_tqual=_tmp488,((_tmp10AC.ret_typ=_tmp489,((_tmp10AC.args=_tmp48A,((_tmp10AC.c_varargs=_tmp48B,((_tmp10AC.cyc_varargs=_tmp48C,((_tmp10AC.rgn_po=_tmp48D,((_tmp10AC.attributes=_tmp48E,((_tmp10AC.requires_clause=_tmp49D,((_tmp10AC.requires_relns=_tmp49E,((_tmp10AC.ensures_clause=_tmp49F,((_tmp10AC.ensures_relns=_tmp4A0,_tmp10AC)))))))))))))))))))))))))),_tmp10AD)))),_tmp10AB))))));}
# 1617
done=1;}}
# 1620
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp497,_tmp489);
Cyc_Tcutil_unify_tqual(_tmp496,_tmp497,_tmp488,_tmp489);
for(0;_tmp498 != 0  && _tmp48A != 0;(_tmp498=_tmp498->tl,_tmp48A=_tmp48A->tl)){
struct _tuple10 _tmp4FC=*((struct _tuple10*)_tmp498->hd);struct Cyc_Absyn_Tqual _tmp4FE;void*_tmp4FF;struct _tuple10 _tmp4FD=_tmp4FC;_tmp4FE=_tmp4FD.f2;_tmp4FF=_tmp4FD.f3;{
struct _tuple10 _tmp500=*((struct _tuple10*)_tmp48A->hd);struct Cyc_Absyn_Tqual _tmp502;void*_tmp503;struct _tuple10 _tmp501=_tmp500;_tmp502=_tmp501.f2;_tmp503=_tmp501.f3;
Cyc_Tcutil_unify_it(_tmp4FF,_tmp503);
Cyc_Tcutil_unify_tqual(_tmp4FE,_tmp4FF,_tmp502,_tmp503);};}
# 1630
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp498 != 0  || _tmp48A != 0){
{const char*_tmp10AE;Cyc_Tcutil_failure_reason=((_tmp10AE="(function types have different number of arguments)",_tag_dyneither(_tmp10AE,sizeof(char),52)));}
_npop_handler(0);goto _LL2BB;}
# 1636
if(_tmp499 != _tmp48B){
{const char*_tmp10AF;Cyc_Tcutil_failure_reason=((_tmp10AF="(only one function type takes C varargs)",_tag_dyneither(_tmp10AF,sizeof(char),41)));}
_npop_handler(0);goto _LL2BB;}{
# 1641
int bad_cyc_vararg=0;
{struct _tuple22 _tmp10B0;struct _tuple22 _tmp506=(_tmp10B0.f1=_tmp49A,((_tmp10B0.f2=_tmp48C,_tmp10B0)));struct _tuple22 _tmp507=_tmp506;struct _dyneither_ptr*_tmp508;struct Cyc_Absyn_Tqual _tmp509;void*_tmp50A;int _tmp50B;struct _dyneither_ptr*_tmp50C;struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50E;int _tmp50F;_LL2FE: if(_tmp507.f1 != 0)goto _LL300;if(_tmp507.f2 != 0)goto _LL300;_LL2FF:
 goto _LL2FD;_LL300: if(_tmp507.f1 != 0)goto _LL302;_LL301:
 goto _LL303;_LL302: if(_tmp507.f2 != 0)goto _LL304;_LL303:
# 1646
 bad_cyc_vararg=1;
{const char*_tmp10B1;Cyc_Tcutil_failure_reason=((_tmp10B1="(only one function type takes varargs)",_tag_dyneither(_tmp10B1,sizeof(char),39)));}
goto _LL2FD;_LL304: if(_tmp507.f1 == 0)goto _LL2FD;_tmp508=(_tmp507.f1)->name;_tmp509=(_tmp507.f1)->tq;_tmp50A=(_tmp507.f1)->type;_tmp50B=(_tmp507.f1)->inject;if(_tmp507.f2 == 0)goto _LL2FD;_tmp50C=(_tmp507.f2)->name;_tmp50D=(_tmp507.f2)->tq;_tmp50E=(_tmp507.f2)->type;_tmp50F=(_tmp507.f2)->inject;_LL305:
# 1650
 Cyc_Tcutil_unify_it(_tmp50A,_tmp50E);
Cyc_Tcutil_unify_tqual(_tmp509,_tmp50A,_tmp50D,_tmp50E);
if(_tmp50B != _tmp50F){
bad_cyc_vararg=1;{
const char*_tmp10B2;Cyc_Tcutil_failure_reason=((_tmp10B2="(only one function type injects varargs)",_tag_dyneither(_tmp10B2,sizeof(char),41)));};}
# 1656
goto _LL2FD;_LL2FD:;}
# 1658
if(bad_cyc_vararg){_npop_handler(0);goto _LL2BB;}{
# 1661
int bad_effect=0;
{struct _tuple0 _tmp10B3;struct _tuple0 _tmp513=(_tmp10B3.f1=_tmp495,((_tmp10B3.f2=_tmp487,_tmp10B3)));struct _tuple0 _tmp514=_tmp513;_LL307: if(_tmp514.f1 != 0)goto _LL309;if(_tmp514.f2 != 0)goto _LL309;_LL308:
 goto _LL306;_LL309: if(_tmp514.f1 != 0)goto _LL30B;_LL30A:
 goto _LL30C;_LL30B: if(_tmp514.f2 != 0)goto _LL30D;_LL30C:
 bad_effect=1;goto _LL306;_LL30D:;_LL30E:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp495),(void*)_check_null(_tmp487));goto _LL306;_LL306:;}
# 1668
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp10B4;Cyc_Tcutil_failure_reason=((_tmp10B4="(function effects do not match)",_tag_dyneither(_tmp10B4,sizeof(char),32)));}
_npop_handler(0);goto _LL2BB;}
# 1674
if(!Cyc_Tcutil_same_atts(_tmp48E,_tmp49C)){
{const char*_tmp10B5;Cyc_Tcutil_failure_reason=((_tmp10B5="(function types have different attributes)",_tag_dyneither(_tmp10B5,sizeof(char),43)));}
_npop_handler(0);goto _LL2BB;}
# 1678
if(!Cyc_Tcutil_same_rgn_po(_tmp48D,_tmp49B)){
{const char*_tmp10B6;Cyc_Tcutil_failure_reason=((_tmp10B6="(function types have different region lifetime orderings)",_tag_dyneither(_tmp10B6,sizeof(char),58)));}
_npop_handler(0);goto _LL2BB;}
# 1682
if(!Cyc_Tcutil_check_logical_equivalence(_tmp490,_tmp49E)){
{const char*_tmp10B7;Cyc_Tcutil_failure_reason=((_tmp10B7="(@requires clauses not equivalent)",_tag_dyneither(_tmp10B7,sizeof(char),35)));}
_npop_handler(0);goto _LL2BB;}
# 1686
if(!Cyc_Tcutil_check_logical_equivalence(_tmp492,_tmp4A0)){
{const char*_tmp10B8;Cyc_Tcutil_failure_reason=((_tmp10B8="(@ensures clauses not equivalent)",_tag_dyneither(_tmp10B8,sizeof(char),34)));}
_npop_handler(0);goto _LL2BB;}
# 1690
_npop_handler(0);return;};};
# 1584
;_pop_region(rgn);}_LL2DA:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp437.f1;if(_tmp4A1->tag != 10)goto _LL2DC;else{_tmp4A2=_tmp4A1->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp437.f2;if(_tmp4A3->tag != 10)goto _LL2DC;else{_tmp4A4=_tmp4A3->f1;}};_LL2DB:
# 1693
 for(0;_tmp4A4 != 0  && _tmp4A2 != 0;(_tmp4A4=_tmp4A4->tl,_tmp4A2=_tmp4A2->tl)){
struct _tuple12 _tmp51B=*((struct _tuple12*)_tmp4A4->hd);struct Cyc_Absyn_Tqual _tmp51D;void*_tmp51E;struct _tuple12 _tmp51C=_tmp51B;_tmp51D=_tmp51C.f1;_tmp51E=_tmp51C.f2;{
struct _tuple12 _tmp51F=*((struct _tuple12*)_tmp4A2->hd);struct Cyc_Absyn_Tqual _tmp521;void*_tmp522;struct _tuple12 _tmp520=_tmp51F;_tmp521=_tmp520.f1;_tmp522=_tmp520.f2;
Cyc_Tcutil_unify_it(_tmp51E,_tmp522);
Cyc_Tcutil_unify_tqual(_tmp51D,_tmp51E,_tmp521,_tmp522);};}
# 1699
if(_tmp4A4 == 0  && _tmp4A2 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10B9;Cyc_Tcutil_failure_reason=((_tmp10B9="(tuple types have different numbers of components)",_tag_dyneither(_tmp10B9,sizeof(char),51)));}
goto _LL2BB;_LL2DC:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp437.f1;if(_tmp4A5->tag != 12)goto _LL2DE;else{_tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp437.f2;if(_tmp4A8->tag != 12)goto _LL2DE;else{_tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;}};_LL2DD:
# 1706
 if(_tmp4A9 != _tmp4A6){{const char*_tmp10BA;Cyc_Tcutil_failure_reason=((_tmp10BA="(struct and union type)",_tag_dyneither(_tmp10BA,sizeof(char),24)));}goto _LL2BB;}
for(0;_tmp4AA != 0  && _tmp4A7 != 0;(_tmp4AA=_tmp4AA->tl,_tmp4A7=_tmp4A7->tl)){
struct Cyc_Absyn_Aggrfield*_tmp525=(struct Cyc_Absyn_Aggrfield*)_tmp4AA->hd;
struct Cyc_Absyn_Aggrfield*_tmp526=(struct Cyc_Absyn_Aggrfield*)_tmp4A7->hd;
if(Cyc_strptrcmp(_tmp525->name,_tmp526->name)!= 0){
{const char*_tmp10BB;Cyc_Tcutil_failure_reason=((_tmp10BB="(different member names)",_tag_dyneither(_tmp10BB,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1714
Cyc_Tcutil_unify_it(_tmp525->type,_tmp526->type);
Cyc_Tcutil_unify_tqual(_tmp525->tq,_tmp525->type,_tmp526->tq,_tmp526->type);
if(!Cyc_Tcutil_same_atts(_tmp525->attributes,_tmp526->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BC;Cyc_Tcutil_failure_reason=((_tmp10BC="(different attributes on member)",_tag_dyneither(_tmp10BC,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1722
if((_tmp525->width != 0  && _tmp526->width == 0  || 
_tmp526->width != 0  && _tmp525->width == 0) || 
(_tmp525->width != 0  && _tmp526->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp525->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp526->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BD;Cyc_Tcutil_failure_reason=((_tmp10BD="(different bitfield widths on member)",_tag_dyneither(_tmp10BD,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1731
if((_tmp525->requires_clause != 0  && _tmp526->requires_clause == 0  || 
_tmp525->requires_clause == 0  && _tmp526->requires_clause != 0) || 
(_tmp525->requires_clause == 0  && _tmp526->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp525->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp526->requires_clause))){
# 1736
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BE;Cyc_Tcutil_failure_reason=((_tmp10BE="(different @requires clauses on member)",_tag_dyneither(_tmp10BE,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1742
if(_tmp4AA == 0  && _tmp4A7 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp10BF;Cyc_Tcutil_failure_reason=((_tmp10BF="(different number of members)",_tag_dyneither(_tmp10BF,sizeof(char),30)));}
goto _LL2BB;_LL2DE:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4AB=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp437.f1;if(_tmp4AB->tag != 17)goto _LL2E0;else{_tmp4AC=_tmp4AB->f2;_tmp4AD=_tmp4AB->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp4AE=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp437.f2;if(_tmp4AE->tag != 17)goto _LL2E0;else{_tmp4AF=_tmp4AE->f2;_tmp4B0=_tmp4AE->f3;}};_LL2DF:
# 1748
 if(_tmp4AD != _tmp4B0){
{const char*_tmp10C0;Cyc_Tcutil_failure_reason=((_tmp10C0="(different abstract typedefs)",_tag_dyneither(_tmp10C0,sizeof(char),30)));}
goto _LL2BB;}
# 1752
{const char*_tmp10C1;Cyc_Tcutil_failure_reason=((_tmp10C1="(type parameters to typedef differ)",_tag_dyneither(_tmp10C1,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp4AC,_tmp4AF);
return;_LL2E0:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4B1=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp437.f1;if(_tmp4B1->tag != 20)goto _LL2E2;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp4B2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp437.f2;if(_tmp4B2->tag != 20)goto _LL2E2;};_LL2E1:
 return;_LL2E2:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4B3=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp437.f1;if(_tmp4B3->tag != 21)goto _LL2E4;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp4B4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp437.f2;if(_tmp4B4->tag != 21)goto _LL2E4;};_LL2E3:
 return;_LL2E4:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4B5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp437.f1;if(_tmp4B5->tag != 22)goto _LL2E6;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp4B6=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp437.f2;if(_tmp4B6->tag != 22)goto _LL2E6;};_LL2E5:
 return;_LL2E6:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B7=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp437.f1;if(_tmp4B7->tag != 15)goto _LL2E8;else{_tmp4B8=(void*)_tmp4B7->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp4B9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp437.f2;if(_tmp4B9->tag != 15)goto _LL2E8;else{_tmp4BA=(void*)_tmp4B9->f1;}};_LL2E7:
# 1759
 Cyc_Tcutil_unify_it(_tmp4B8,_tmp4BA);
return;_LL2E8:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4BB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp437.f1;if(_tmp4BB->tag != 16)goto _LL2EA;else{_tmp4BC=(void*)_tmp4BB->f1;_tmp4BD=(void*)_tmp4BB->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp4BE=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp437.f2;if(_tmp4BE->tag != 16)goto _LL2EA;else{_tmp4BF=(void*)_tmp4BE->f1;_tmp4C0=(void*)_tmp4BE->f2;}};_LL2E9:
# 1762
 Cyc_Tcutil_unify_it(_tmp4BC,_tmp4BF);
Cyc_Tcutil_unify_it(_tmp4BD,_tmp4C0);
return;_LL2EA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C1=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp437.f1;if(_tmp4C1->tag != 24)goto _LL2EC;}_LL2EB:
 goto _LL2ED;_LL2EC: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp4C2=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp437.f2;if(_tmp4C2->tag != 24)goto _LL2EE;}_LL2ED:
 goto _LL2EF;_LL2EE: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp437.f1;if(_tmp4C3->tag != 23)goto _LL2F0;}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4C4=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp437.f1;if(_tmp4C4->tag != 25)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp4C5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp437.f2;if(_tmp4C5->tag != 25)goto _LL2F4;}_LL2F3:
 goto _LL2F5;_LL2F4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4C6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp437.f2;if(_tmp4C6->tag != 23)goto _LL2F6;}_LL2F5:
# 1771
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp10C2;Cyc_Tcutil_failure_reason=((_tmp10C2="(effects don't unify)",_tag_dyneither(_tmp10C2,sizeof(char),22)));}
goto _LL2BB;_LL2F6:;_LL2F7:
 goto _LL2BB;_LL2BB:;}
# 1776
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1779
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1786
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp530=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp531=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp530,_tmp531);}
# 1793
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp532=x;void*_tmp533;_LL310: if((_tmp532->No_constr).tag != 3)goto _LL312;_LL311:
 return - 1;_LL312: if((_tmp532->Eq_constr).tag != 1)goto _LL314;_tmp533=(void*)(_tmp532->Eq_constr).val;_LL313: {
# 1800
union Cyc_Absyn_Constraint*_tmp534=y;void*_tmp535;_LL317: if((_tmp534->No_constr).tag != 3)goto _LL319;_LL318:
 return 1;_LL319: if((_tmp534->Eq_constr).tag != 1)goto _LL31B;_tmp535=(void*)(_tmp534->Eq_constr).val;_LL31A:
 return cmp(_tmp533,_tmp535);_LL31B: if((_tmp534->Forward_constr).tag != 2)goto _LL316;_LL31C: {
const char*_tmp10C5;void*_tmp10C4;(_tmp10C4=0,Cyc_Tcutil_impos(((_tmp10C5="unify_conref: forward after compress(2)",_tag_dyneither(_tmp10C5,sizeof(char),40))),_tag_dyneither(_tmp10C4,sizeof(void*),0)));}_LL316:;}_LL314: if((_tmp532->Forward_constr).tag != 2)goto _LL30F;_LL315: {
# 1805
const char*_tmp10C8;void*_tmp10C7;(_tmp10C7=0,Cyc_Tcutil_impos(((_tmp10C8="unify_conref: forward after compress",_tag_dyneither(_tmp10C8,sizeof(char),37))),_tag_dyneither(_tmp10C7,sizeof(void*),0)));}_LL30F:;};}
# 1809
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp53B;void*_tmp53C;struct _tuple12*_tmp53A=tqt1;_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;{
struct Cyc_Absyn_Tqual _tmp53E;void*_tmp53F;struct _tuple12*_tmp53D=tqt2;_tmp53E=_tmp53D->f1;_tmp53F=_tmp53D->f2;{
int _tmp540=Cyc_Tcutil_tqual_cmp(_tmp53B,_tmp53E);
if(_tmp540 != 0)return _tmp540;
return Cyc_Tcutil_typecmp(_tmp53C,_tmp53F);};};}
# 1817
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp541=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp541 != 0)return _tmp541;{
int _tmp542=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp542 != 0)return _tmp542;{
int _tmp543=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp543 != 0)return _tmp543;{
int _tmp544=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp544 != 0)return _tmp544;
_tmp544=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp544 != 0)return _tmp544;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1831
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp545=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp545 != 0)return _tmp545;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1837
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp546=t;_LL31E: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp547=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp546;if(_tmp547->tag != 0)goto _LL320;}_LL31F:
 return 0;_LL320: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp546;if(_tmp548->tag != 1)goto _LL322;}_LL321:
 return 1;_LL322: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp546;if(_tmp549->tag != 2)goto _LL324;}_LL323:
 return 2;_LL324: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp546;if(_tmp54A->tag != 3)goto _LL326;}_LL325:
 return 3;_LL326: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp546;if(_tmp54B->tag != 4)goto _LL328;}_LL327:
 return 4;_LL328: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp546;if(_tmp54C->tag != 5)goto _LL32A;}_LL329:
 return 5;_LL32A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp54D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp546;if(_tmp54D->tag != 6)goto _LL32C;}_LL32B:
 return 6;_LL32C: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp54E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp546;if(_tmp54E->tag != 7)goto _LL32E;else{if(_tmp54E->f1 != 0)goto _LL32E;}}_LL32D:
 return 7;_LL32E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp54F=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp546;if(_tmp54F->tag != 7)goto _LL330;}_LL32F:
 return 8;_LL330: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp546;if(_tmp550->tag != 8)goto _LL332;}_LL331:
 return 9;_LL332: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp551=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp546;if(_tmp551->tag != 9)goto _LL334;}_LL333:
 return 10;_LL334: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp546;if(_tmp552->tag != 10)goto _LL336;}_LL335:
 return 11;_LL336: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp553=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp546;if(_tmp553->tag != 11)goto _LL338;}_LL337:
 return 12;_LL338: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp546;if(_tmp554->tag != 12)goto _LL33A;}_LL339:
 return 14;_LL33A: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp555=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp546;if(_tmp555->tag != 13)goto _LL33C;}_LL33B:
 return 16;_LL33C: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp546;if(_tmp556->tag != 14)goto _LL33E;}_LL33D:
 return 17;_LL33E: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp546;if(_tmp557->tag != 15)goto _LL340;}_LL33F:
 return 18;_LL340: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp546;if(_tmp558->tag != 17)goto _LL342;}_LL341:
 return 19;_LL342: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp546;if(_tmp559->tag != 21)goto _LL344;}_LL343:
 return 20;_LL344: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp546;if(_tmp55A->tag != 20)goto _LL346;}_LL345:
 return 21;_LL346: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp546;if(_tmp55B->tag != 23)goto _LL348;}_LL347:
 return 22;_LL348: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp546;if(_tmp55C->tag != 24)goto _LL34A;}_LL349:
 return 23;_LL34A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp55D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp546;if(_tmp55D->tag != 25)goto _LL34C;}_LL34B:
 return 24;_LL34C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp546;if(_tmp55E->tag != 22)goto _LL34E;}_LL34D:
 return 25;_LL34E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp546;if(_tmp55F->tag != 19)goto _LL350;}_LL34F:
 return 26;_LL350: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp546;if(_tmp560->tag != 16)goto _LL352;}_LL351:
 return 27;_LL352: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp546;if(_tmp561->tag != 18)goto _LL354;}_LL353:
 return 28;_LL354: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp562=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp546;if(_tmp562->tag != 26)goto _LL31D;}_LL355:
 return 29;_LL31D:;}
# 1872
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp563=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp563 != 0)
return _tmp563;{
# 1881
struct _tuple0 _tmp10C9;struct _tuple0 _tmp564=(_tmp10C9.f1=t2,((_tmp10C9.f2=t1,_tmp10C9)));struct _tuple0 _tmp565=_tmp564;struct Cyc_Absyn_Tvar*_tmp569;struct Cyc_Absyn_Tvar*_tmp56B;union Cyc_Absyn_AggrInfoU _tmp56D;struct Cyc_List_List*_tmp56E;union Cyc_Absyn_AggrInfoU _tmp570;struct Cyc_List_List*_tmp571;struct _tuple2*_tmp573;struct _tuple2*_tmp575;struct Cyc_List_List*_tmp577;struct Cyc_List_List*_tmp579;struct Cyc_Absyn_Datatypedecl*_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_Absyn_Datatypedecl*_tmp57E;struct Cyc_List_List*_tmp57F;struct Cyc_Absyn_Datatypedecl*_tmp581;struct Cyc_Absyn_Datatypefield*_tmp582;struct Cyc_List_List*_tmp583;struct Cyc_Absyn_Datatypedecl*_tmp585;struct Cyc_Absyn_Datatypefield*_tmp586;struct Cyc_List_List*_tmp587;void*_tmp589;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58B;union Cyc_Absyn_Constraint*_tmp58C;union Cyc_Absyn_Constraint*_tmp58D;union Cyc_Absyn_Constraint*_tmp58E;void*_tmp590;struct Cyc_Absyn_Tqual _tmp591;void*_tmp592;union Cyc_Absyn_Constraint*_tmp593;union Cyc_Absyn_Constraint*_tmp594;union Cyc_Absyn_Constraint*_tmp595;enum Cyc_Absyn_Sign _tmp597;enum Cyc_Absyn_Size_of _tmp598;enum Cyc_Absyn_Sign _tmp59A;enum Cyc_Absyn_Size_of _tmp59B;int _tmp59D;int _tmp59F;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;union Cyc_Absyn_Constraint*_tmp5A4;void*_tmp5A6;struct Cyc_Absyn_Tqual _tmp5A7;struct Cyc_Absyn_Exp*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A9;struct Cyc_List_List*_tmp5AD;struct Cyc_List_List*_tmp5AF;enum Cyc_Absyn_AggrKind _tmp5B1;struct Cyc_List_List*_tmp5B2;enum Cyc_Absyn_AggrKind _tmp5B4;struct Cyc_List_List*_tmp5B5;void*_tmp5B7;void*_tmp5B9;void*_tmp5BB;void*_tmp5BC;void*_tmp5BE;void*_tmp5BF;void*_tmp5C1;void*_tmp5C3;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C7;_LL357:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp565.f1;if(_tmp566->tag != 1)goto _LL359;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp567=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp565.f2;if(_tmp567->tag != 1)goto _LL359;};_LL358: {
# 1883
const char*_tmp10CC;void*_tmp10CB;(_tmp10CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CC="typecmp: can only compare closed types",_tag_dyneither(_tmp10CC,sizeof(char),39))),_tag_dyneither(_tmp10CB,sizeof(void*),0)));}_LL359:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp565.f1;if(_tmp568->tag != 2)goto _LL35B;else{_tmp569=_tmp568->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp565.f2;if(_tmp56A->tag != 2)goto _LL35B;else{_tmp56B=_tmp56A->f1;}};_LL35A:
# 1887
 return Cyc_Core_intcmp(_tmp56B->identity,_tmp569->identity);_LL35B:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp565.f1;if(_tmp56C->tag != 11)goto _LL35D;else{_tmp56D=(_tmp56C->f1).aggr_info;_tmp56E=(_tmp56C->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp565.f2;if(_tmp56F->tag != 11)goto _LL35D;else{_tmp570=(_tmp56F->f1).aggr_info;_tmp571=(_tmp56F->f1).targs;}};_LL35C: {
# 1891
struct _tuple13 _tmp5D0=Cyc_Absyn_aggr_kinded_name(_tmp56D);struct _tuple2*_tmp5D2;struct _tuple13 _tmp5D1=_tmp5D0;_tmp5D2=_tmp5D1.f2;{
struct _tuple13 _tmp5D3=Cyc_Absyn_aggr_kinded_name(_tmp570);struct _tuple2*_tmp5D5;struct _tuple13 _tmp5D4=_tmp5D3;_tmp5D5=_tmp5D4.f2;{
int _tmp5D6=Cyc_Absyn_qvar_cmp(_tmp5D2,_tmp5D5);
if(_tmp5D6 != 0)return _tmp5D6;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp56E,_tmp571);}};};}_LL35D:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp572=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp565.f1;if(_tmp572->tag != 13)goto _LL35F;else{_tmp573=_tmp572->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp565.f2;if(_tmp574->tag != 13)goto _LL35F;else{_tmp575=_tmp574->f1;}};_LL35E:
# 1898
 return Cyc_Absyn_qvar_cmp(_tmp573,_tmp575);_LL35F:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp576=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp565.f1;if(_tmp576->tag != 14)goto _LL361;else{_tmp577=_tmp576->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp578=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp565.f2;if(_tmp578->tag != 14)goto _LL361;else{_tmp579=_tmp578->f1;}};_LL360:
# 1901
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp577,_tmp579);_LL361:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp57A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp565.f1;if(_tmp57A->tag != 3)goto _LL363;else{if((((_tmp57A->f1).datatype_info).KnownDatatype).tag != 2)goto _LL363;_tmp57B=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp57A->f1).datatype_info).KnownDatatype).val);_tmp57C=(_tmp57A->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp565.f2;if(_tmp57D->tag != 3)goto _LL363;else{if((((_tmp57D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL363;_tmp57E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp57D->f1).datatype_info).KnownDatatype).val);_tmp57F=(_tmp57D->f1).targs;}};_LL362:
# 1905
 if(_tmp57E == _tmp57B)return 0;{
int _tmp5D7=Cyc_Absyn_qvar_cmp(_tmp57E->name,_tmp57B->name);
if(_tmp5D7 != 0)return _tmp5D7;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp57F,_tmp57C);};_LL363:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp580=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp565.f1;if(_tmp580->tag != 4)goto _LL365;else{if((((_tmp580->f1).field_info).KnownDatatypefield).tag != 2)goto _LL365;_tmp581=((struct _tuple3)(((_tmp580->f1).field_info).KnownDatatypefield).val).f1;_tmp582=((struct _tuple3)(((_tmp580->f1).field_info).KnownDatatypefield).val).f2;_tmp583=(_tmp580->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp584=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp565.f2;if(_tmp584->tag != 4)goto _LL365;else{if((((_tmp584->f1).field_info).KnownDatatypefield).tag != 2)goto _LL365;_tmp585=((struct _tuple3)(((_tmp584->f1).field_info).KnownDatatypefield).val).f1;_tmp586=((struct _tuple3)(((_tmp584->f1).field_info).KnownDatatypefield).val).f2;_tmp587=(_tmp584->f1).targs;}};_LL364:
# 1912
 if(_tmp585 == _tmp581)return 0;{
int _tmp5D8=Cyc_Absyn_qvar_cmp(_tmp581->name,_tmp585->name);
if(_tmp5D8 != 0)return _tmp5D8;{
int _tmp5D9=Cyc_Absyn_qvar_cmp(_tmp582->name,_tmp586->name);
if(_tmp5D9 != 0)return _tmp5D9;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp587,_tmp583);};};_LL365:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp565.f1;if(_tmp588->tag != 5)goto _LL367;else{_tmp589=(_tmp588->f1).elt_typ;_tmp58A=(_tmp588->f1).elt_tq;_tmp58B=((_tmp588->f1).ptr_atts).rgn;_tmp58C=((_tmp588->f1).ptr_atts).nullable;_tmp58D=((_tmp588->f1).ptr_atts).bounds;_tmp58E=((_tmp588->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp565.f2;if(_tmp58F->tag != 5)goto _LL367;else{_tmp590=(_tmp58F->f1).elt_typ;_tmp591=(_tmp58F->f1).elt_tq;_tmp592=((_tmp58F->f1).ptr_atts).rgn;_tmp593=((_tmp58F->f1).ptr_atts).nullable;_tmp594=((_tmp58F->f1).ptr_atts).bounds;_tmp595=((_tmp58F->f1).ptr_atts).zero_term;}};_LL366: {
# 1921
int _tmp5DA=Cyc_Tcutil_typecmp(_tmp590,_tmp589);
if(_tmp5DA != 0)return _tmp5DA;{
int _tmp5DB=Cyc_Tcutil_typecmp(_tmp592,_tmp58B);
if(_tmp5DB != 0)return _tmp5DB;{
int _tmp5DC=Cyc_Tcutil_tqual_cmp(_tmp591,_tmp58A);
if(_tmp5DC != 0)return _tmp5DC;{
int _tmp5DD=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp594,_tmp58D);
if(_tmp5DD != 0)return _tmp5DD;{
int _tmp5DE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp595,_tmp58E);
if(_tmp5DE != 0)return _tmp5DE;
{void*_tmp5DF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp594);void*_tmp5E0=_tmp5DF;_LL38A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5E1=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5E0;if(_tmp5E1->tag != 0)goto _LL38C;}_LL38B:
 return 0;_LL38C:;_LL38D:
 goto _LL389;_LL389:;}
# 1935
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp593,_tmp58C);};};};};}_LL367:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp565.f1;if(_tmp596->tag != 6)goto _LL369;else{_tmp597=_tmp596->f1;_tmp598=_tmp596->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp565.f2;if(_tmp599->tag != 6)goto _LL369;else{_tmp59A=_tmp599->f1;_tmp59B=_tmp599->f2;}};_LL368:
# 1938
 if(_tmp59A != _tmp597)return Cyc_Core_intcmp((int)((unsigned int)_tmp59A),(int)((unsigned int)_tmp597));
if(_tmp59B != _tmp598)return Cyc_Core_intcmp((int)((unsigned int)_tmp59B),(int)((unsigned int)_tmp598));
return 0;_LL369:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp565.f1;if(_tmp59C->tag != 7)goto _LL36B;else{_tmp59D=_tmp59C->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp565.f2;if(_tmp59E->tag != 7)goto _LL36B;else{_tmp59F=_tmp59E->f1;}};_LL36A:
# 1943
 if(_tmp59D == _tmp59F)return 0;else{
if(_tmp59F == 0)return - 1;else{
if(_tmp59F == 1  && _tmp59D == 0)return - 1;else{
return 1;}}}_LL36B:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5A0->tag != 8)goto _LL36D;else{_tmp5A1=(_tmp5A0->f1).elt_type;_tmp5A2=(_tmp5A0->f1).tq;_tmp5A3=(_tmp5A0->f1).num_elts;_tmp5A4=(_tmp5A0->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5A5->tag != 8)goto _LL36D;else{_tmp5A6=(_tmp5A5->f1).elt_type;_tmp5A7=(_tmp5A5->f1).tq;_tmp5A8=(_tmp5A5->f1).num_elts;_tmp5A9=(_tmp5A5->f1).zero_term;}};_LL36C: {
# 1950
int _tmp5E2=Cyc_Tcutil_tqual_cmp(_tmp5A7,_tmp5A2);
if(_tmp5E2 != 0)return _tmp5E2;{
int _tmp5E3=Cyc_Tcutil_typecmp(_tmp5A6,_tmp5A1);
if(_tmp5E3 != 0)return _tmp5E3;{
int _tmp5E4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp5A4,_tmp5A9);
if(_tmp5E4 != 0)return _tmp5E4;
if(_tmp5A3 == _tmp5A8)return 0;
if(_tmp5A3 == 0  || _tmp5A8 == 0){
const char*_tmp10CF;void*_tmp10CE;(_tmp10CE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10CF="missing expression in array index",_tag_dyneither(_tmp10CF,sizeof(char),34))),_tag_dyneither(_tmp10CE,sizeof(void*),0)));}
# 1960
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp5A3,_tmp5A8);};};}_LL36D:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5AA->tag != 9)goto _LL36F;}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5AB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5AB->tag != 9)goto _LL36F;};_LL36E: {
# 1963
const char*_tmp10D2;void*_tmp10D1;(_tmp10D1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D2="typecmp: function types not handled",_tag_dyneither(_tmp10D2,sizeof(char),36))),_tag_dyneither(_tmp10D1,sizeof(void*),0)));}_LL36F:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5AC->tag != 10)goto _LL371;else{_tmp5AD=_tmp5AC->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5AE->tag != 10)goto _LL371;else{_tmp5AF=_tmp5AE->f1;}};_LL370:
# 1966
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp5AF,_tmp5AD);_LL371:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5B0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5B0->tag != 12)goto _LL373;else{_tmp5B1=_tmp5B0->f1;_tmp5B2=_tmp5B0->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5B3=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5B3->tag != 12)goto _LL373;else{_tmp5B4=_tmp5B3->f1;_tmp5B5=_tmp5B3->f2;}};_LL372:
# 1969
 if(_tmp5B4 != _tmp5B1){
if(_tmp5B4 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp5B5,_tmp5B2);_LL373:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5B6=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5B6->tag != 15)goto _LL375;else{_tmp5B7=(void*)_tmp5B6->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp5B8=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5B8->tag != 15)goto _LL375;else{_tmp5B9=(void*)_tmp5B8->f1;}};_LL374:
# 1974
 return Cyc_Tcutil_typecmp(_tmp5B7,_tmp5B9);_LL375:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5BA=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5BA->tag != 16)goto _LL377;else{_tmp5BB=(void*)_tmp5BA->f1;_tmp5BC=(void*)_tmp5BA->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp5BD=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5BD->tag != 16)goto _LL377;else{_tmp5BE=(void*)_tmp5BD->f1;_tmp5BF=(void*)_tmp5BD->f2;}};_LL376: {
# 1976
int _tmp5E9=Cyc_Tcutil_typecmp(_tmp5BB,_tmp5BE);
if(_tmp5E9 != 0)return _tmp5E9;else{
return Cyc_Tcutil_typecmp(_tmp5BC,_tmp5BF);}}_LL377:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C0=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5C0->tag != 19)goto _LL379;else{_tmp5C1=(void*)_tmp5C0->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5C2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5C2->tag != 19)goto _LL379;else{_tmp5C3=(void*)_tmp5C2->f1;}};_LL378:
 return Cyc_Tcutil_typecmp(_tmp5C1,_tmp5C3);_LL379:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C4=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp565.f1;if(_tmp5C4->tag != 18)goto _LL37B;else{_tmp5C5=_tmp5C4->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp565.f2;if(_tmp5C6->tag != 18)goto _LL37B;else{_tmp5C7=_tmp5C6->f1;}};_LL37A:
# 1981
 return Cyc_Evexp_const_exp_cmp(_tmp5C5,_tmp5C7);_LL37B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp565.f1;if(_tmp5C8->tag != 24)goto _LL37D;}_LL37C:
 goto _LL37E;_LL37D: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5C9=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp565.f2;if(_tmp5C9->tag != 24)goto _LL37F;}_LL37E:
 goto _LL380;_LL37F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5CA=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp565.f1;if(_tmp5CA->tag != 23)goto _LL381;}_LL380:
 goto _LL382;_LL381: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp565.f1;if(_tmp5CB->tag != 25)goto _LL383;}_LL382:
 goto _LL384;_LL383: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp565.f2;if(_tmp5CC->tag != 25)goto _LL385;}_LL384:
 goto _LL386;_LL385: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp565.f2;if(_tmp5CD->tag != 23)goto _LL387;}_LL386: {
const char*_tmp10D5;void*_tmp10D4;(_tmp10D4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D5="typecmp: effects not handled",_tag_dyneither(_tmp10D5,sizeof(char),29))),_tag_dyneither(_tmp10D4,sizeof(void*),0)));}_LL387:;_LL388: {
const char*_tmp10D8;void*_tmp10D7;(_tmp10D7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10D8="Unmatched case in typecmp",_tag_dyneither(_tmp10D8,sizeof(char),26))),_tag_dyneither(_tmp10D7,sizeof(void*),0)));}_LL356:;};};}
# 1992
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp5EF=Cyc_Tcutil_compress(t);void*_tmp5F0=_tmp5EF;_LL38F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F0;if(_tmp5F1->tag != 6)goto _LL391;}_LL390:
 goto _LL392;_LL391: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F0;if(_tmp5F2->tag != 7)goto _LL393;}_LL392:
 goto _LL394;_LL393: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5F0;if(_tmp5F3->tag != 13)goto _LL395;}_LL394:
 goto _LL396;_LL395: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5F4=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5F0;if(_tmp5F4->tag != 14)goto _LL397;}_LL396:
 return 1;_LL397:;_LL398:
 return 0;_LL38E:;}
# 2004
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10D9;struct _tuple0 _tmp5F5=(_tmp10D9.f1=t1,((_tmp10D9.f2=t2,_tmp10D9)));struct _tuple0 _tmp5F6=_tmp5F5;int _tmp5F8;int _tmp5FA;_LL39A:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp5F7->tag != 7)goto _LL39C;else{_tmp5F8=_tmp5F7->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5F9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp5F9->tag != 7)goto _LL39C;else{_tmp5FA=_tmp5F9->f1;}};_LL39B:
# 2009
 return _tmp5FA == 0  && _tmp5F8 != 0  || (_tmp5FA == 1  && _tmp5F8 != 0) && _tmp5F8 != 1;_LL39C:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5FB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp5FB->tag != 7)goto _LL39E;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5FC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp5FC->tag != 6)goto _LL39E;};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5FD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp5FD->tag != 7)goto _LL3A0;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5FE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp5FE->tag != 19)goto _LL3A0;};_LL39F:
 return 1;_LL3A0:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5FF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp5FF->tag != 6)goto _LL3A2;else{if(_tmp5FF->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp600->tag != 6)goto _LL3A2;else{if(_tmp600->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A2;}};_LL3A1:
 return 0;_LL3A2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp601=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp601->tag != 6)goto _LL3A4;else{if(_tmp601->f2 != Cyc_Absyn_LongLong_sz)goto _LL3A4;}}_LL3A3:
 return 1;_LL3A4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp602=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp602->tag != 6)goto _LL3A6;else{if(_tmp602->f2 != Cyc_Absyn_Long_sz)goto _LL3A6;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp603->tag != 6)goto _LL3A6;else{if(_tmp603->f2 != Cyc_Absyn_Int_sz)goto _LL3A6;}};_LL3A5:
# 2016
 goto _LL3A7;_LL3A6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp604->tag != 6)goto _LL3A8;else{if(_tmp604->f2 != Cyc_Absyn_Int_sz)goto _LL3A8;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp605->tag != 6)goto _LL3A8;else{if(_tmp605->f2 != Cyc_Absyn_Long_sz)goto _LL3A8;}};_LL3A7:
 return 0;_LL3A8:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp606->tag != 6)goto _LL3AA;else{if(_tmp606->f2 != Cyc_Absyn_Long_sz)goto _LL3AA;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp607=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp607->tag != 7)goto _LL3AA;else{if(_tmp607->f1 != 0)goto _LL3AA;}};_LL3A9:
# 2019
 goto _LL3AB;_LL3AA:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp608=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp608->tag != 6)goto _LL3AC;else{if(_tmp608->f2 != Cyc_Absyn_Int_sz)goto _LL3AC;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp609=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp609->tag != 7)goto _LL3AC;else{if(_tmp609->f1 != 0)goto _LL3AC;}};_LL3AB:
 goto _LL3AD;_LL3AC:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp60A->tag != 6)goto _LL3AE;else{if(_tmp60A->f2 != Cyc_Absyn_Long_sz)goto _LL3AE;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp60B->tag != 6)goto _LL3AE;else{if(_tmp60B->f2 != Cyc_Absyn_Short_sz)goto _LL3AE;}};_LL3AD:
 goto _LL3AF;_LL3AE:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp60C->tag != 6)goto _LL3B0;else{if(_tmp60C->f2 != Cyc_Absyn_Int_sz)goto _LL3B0;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp60D->tag != 6)goto _LL3B0;else{if(_tmp60D->f2 != Cyc_Absyn_Short_sz)goto _LL3B0;}};_LL3AF:
 goto _LL3B1;_LL3B0:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp60E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp60E->tag != 19)goto _LL3B2;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp60F->tag != 6)goto _LL3B2;else{if(_tmp60F->f2 != Cyc_Absyn_Short_sz)goto _LL3B2;}};_LL3B1:
 goto _LL3B3;_LL3B2:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp610=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp610->tag != 6)goto _LL3B4;else{if(_tmp610->f2 != Cyc_Absyn_Long_sz)goto _LL3B4;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp611=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp611->tag != 6)goto _LL3B4;else{if(_tmp611->f2 != Cyc_Absyn_Char_sz)goto _LL3B4;}};_LL3B3:
 goto _LL3B5;_LL3B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp612=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp612->tag != 6)goto _LL3B6;else{if(_tmp612->f2 != Cyc_Absyn_Int_sz)goto _LL3B6;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp613=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp613->tag != 6)goto _LL3B6;else{if(_tmp613->f2 != Cyc_Absyn_Char_sz)goto _LL3B6;}};_LL3B5:
 goto _LL3B7;_LL3B6:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp614=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp614->tag != 6)goto _LL3B8;else{if(_tmp614->f2 != Cyc_Absyn_Short_sz)goto _LL3B8;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp615=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp615->tag != 6)goto _LL3B8;else{if(_tmp615->f2 != Cyc_Absyn_Char_sz)goto _LL3B8;}};_LL3B7:
 goto _LL3B9;_LL3B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5F6.f1;if(_tmp616->tag != 19)goto _LL3BA;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp617=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F6.f2;if(_tmp617->tag != 6)goto _LL3BA;else{if(_tmp617->f2 != Cyc_Absyn_Char_sz)goto _LL3BA;}};_LL3B9:
 return 1;_LL3BA:;_LL3BB:
# 2029
 return 0;_LL399:;};}
# 2035
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2038
struct _RegionHandle _tmp619=_new_region("r");struct _RegionHandle*r=& _tmp619;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp10DA;max_arith_type=((_tmp10DA=_region_malloc(r,sizeof(*_tmp10DA)),((_tmp10DA->v=t1,_tmp10DA))));}}}}
# 2048
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp61B=0;_npop_handler(0);return _tmp61B;}}}
# 2053
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp10DF;void*_tmp10DE[2];struct Cyc_String_pa_PrintArg_struct _tmp10DD;struct Cyc_String_pa_PrintArg_struct _tmp10DC;(_tmp10DC.tag=0,((_tmp10DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp10DD.tag=0,((_tmp10DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp10DE[0]=& _tmp10DD,((_tmp10DE[1]=& _tmp10DC,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp10DF="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp10DF,sizeof(char),41))),_tag_dyneither(_tmp10DE,sizeof(void*),2)))))))))))));}{
int _tmp620=0;_npop_handler(0);return _tmp620;};}}}{
# 2059
int _tmp621=1;_npop_handler(0);return _tmp621;};
# 2038
;_pop_region(r);}
# 2064
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp622=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp623=_tmp622;_LL3BD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp624=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp623;if(_tmp624->tag != 5)goto _LL3BF;}_LL3BE:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3BC;_LL3BF:;_LL3C0:
 return 0;_LL3BC:;}
# 2070
return 1;}
# 2073
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp625=Cyc_Tcutil_compress(t);void*_tmp626=_tmp625;_LL3C2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp627=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp626;if(_tmp627->tag != 6)goto _LL3C4;}_LL3C3:
 goto _LL3C5;_LL3C4: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp628=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp626;if(_tmp628->tag != 19)goto _LL3C6;}_LL3C5:
 goto _LL3C7;_LL3C6: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp629=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp626;if(_tmp629->tag != 13)goto _LL3C8;}_LL3C7:
 goto _LL3C9;_LL3C8: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp62A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp626;if(_tmp62A->tag != 14)goto _LL3CA;}_LL3C9:
 return 1;_LL3CA:;_LL3CB:
 return 0;_LL3C1:;}
# 2084
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2088
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp10E2;void*_tmp10E1;(_tmp10E1=0,Cyc_Tcutil_warn(e->loc,((_tmp10E2="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10E2,sizeof(char),44))),_tag_dyneither(_tmp10E1,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2094
return 0;}
# 2098
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2102
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmp10E5;void*_tmp10E4;(_tmp10E4=0,Cyc_Tcutil_warn(e->loc,((_tmp10E5="integral size mismatch; conversion supplied",_tag_dyneither(_tmp10E5,sizeof(char),44))),_tag_dyneither(_tmp10E4,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2108
return 0;}
# 2113
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2121
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2123
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10E6;struct _tuple0 _tmp62F=(_tmp10E6.f1=t1,((_tmp10E6.f2=t2,_tmp10E6)));struct _tuple0 _tmp630=_tmp62F;struct Cyc_Absyn_PtrInfo _tmp632;struct Cyc_Absyn_PtrInfo _tmp634;void*_tmp636;struct Cyc_Absyn_Tqual _tmp637;struct Cyc_Absyn_Exp*_tmp638;union Cyc_Absyn_Constraint*_tmp639;void*_tmp63B;struct Cyc_Absyn_Tqual _tmp63C;struct Cyc_Absyn_Exp*_tmp63D;union Cyc_Absyn_Constraint*_tmp63E;_LL3CD:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp631=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp630.f1;if(_tmp631->tag != 5)goto _LL3CF;else{_tmp632=_tmp631->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp633=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp630.f2;if(_tmp633->tag != 5)goto _LL3CF;else{_tmp634=_tmp633->f1;}};_LL3CE: {
# 2127
int okay=1;
# 2129
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp632.ptr_atts).nullable,(_tmp634.ptr_atts).nullable))
# 2132
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp632.ptr_atts).nullable);
# 2134
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp632.ptr_atts).bounds,(_tmp634.ptr_atts).bounds)){
# 2137
struct _tuple0 _tmp10E7;struct _tuple0 _tmp641=(_tmp10E7.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp632.ptr_atts).bounds),((_tmp10E7.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp634.ptr_atts).bounds),_tmp10E7)));
# 2137
struct _tuple0 _tmp642=_tmp641;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64D;_LL3D6:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp643=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp642.f1;if(_tmp643->tag != 1)goto _LL3D8;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp644=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp642.f2;if(_tmp644->tag != 0)goto _LL3D8;};_LL3D7:
# 2139
 goto _LL3D9;_LL3D8:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp645=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp642.f1;if(_tmp645->tag != 0)goto _LL3DA;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp646=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp642.f2;if(_tmp646->tag != 0)goto _LL3DA;};_LL3D9:
 okay=1;goto _LL3D5;_LL3DA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp647=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp642.f1;if(_tmp647->tag != 1)goto _LL3DC;else{_tmp648=_tmp647->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp649=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp642.f2;if(_tmp649->tag != 1)goto _LL3DC;else{_tmp64A=_tmp649->f1;}};_LL3DB:
# 2142
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp64A,_tmp648);
# 2146
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp634.ptr_atts).zero_term)){
const char*_tmp10EA;void*_tmp10E9;(_tmp10E9=0,Cyc_Tcutil_warn(loc,((_tmp10EA="implicit cast to shorter array",_tag_dyneither(_tmp10EA,sizeof(char),31))),_tag_dyneither(_tmp10E9,sizeof(void*),0)));}
goto _LL3D5;_LL3DC:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp64B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp642.f1;if(_tmp64B->tag != 0)goto _LL3D5;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp64C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp642.f2;if(_tmp64C->tag != 1)goto _LL3D5;else{_tmp64D=_tmp64C->f1;}};_LL3DD:
# 2151
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp632.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp634.ptr_atts).bounds))
goto _LL3D5;
okay=0;
goto _LL3D5;_LL3D5:;}
# 2160
okay=okay  && (!(_tmp632.elt_tq).real_const  || (_tmp634.elt_tq).real_const);
# 2163
if(!Cyc_Tcutil_unify((_tmp632.ptr_atts).rgn,(_tmp634.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp632.ptr_atts).rgn,(_tmp634.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp10EF;void*_tmp10EE[2];struct Cyc_String_pa_PrintArg_struct _tmp10ED;struct Cyc_String_pa_PrintArg_struct _tmp10EC;(_tmp10EC.tag=0,((_tmp10EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2168
Cyc_Absynpp_typ2string((_tmp634.ptr_atts).rgn)),((_tmp10ED.tag=0,((_tmp10ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2167
Cyc_Absynpp_typ2string((_tmp632.ptr_atts).rgn)),((_tmp10EE[0]=& _tmp10ED,((_tmp10EE[1]=& _tmp10EC,Cyc_Tcutil_warn(loc,((_tmp10EF="implicit cast form region %s to region %s",_tag_dyneither(_tmp10EF,sizeof(char),42))),_tag_dyneither(_tmp10EE,sizeof(void*),2)))))))))))));}}else{
# 2169
okay=0;}}
# 2173
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp632.ptr_atts).zero_term,(_tmp634.ptr_atts).zero_term) || 
# 2176
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp632.ptr_atts).zero_term) && (_tmp634.elt_tq).real_const);{
# 2184
int _tmp655=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp634.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2187
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp634.ptr_atts).zero_term);
# 2191
okay=okay  && (Cyc_Tcutil_unify(_tmp632.elt_typ,_tmp634.elt_typ) || 
(_tmp655  && ((_tmp634.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp634.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp632.elt_typ,_tmp634.elt_typ));
# 2194
return okay;};}_LL3CF:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp635=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630.f1;if(_tmp635->tag != 8)goto _LL3D1;else{_tmp636=(_tmp635->f1).elt_type;_tmp637=(_tmp635->f1).tq;_tmp638=(_tmp635->f1).num_elts;_tmp639=(_tmp635->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp63A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp630.f2;if(_tmp63A->tag != 8)goto _LL3D1;else{_tmp63B=(_tmp63A->f1).elt_type;_tmp63C=(_tmp63A->f1).tq;_tmp63D=(_tmp63A->f1).num_elts;_tmp63E=(_tmp63A->f1).zero_term;}};_LL3D0: {
# 2198
int okay;
# 2200
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp639,_tmp63E) && (
(_tmp638 != 0  && _tmp63D != 0) && Cyc_Evexp_same_const_exp(_tmp638,_tmp63D));
# 2203
return(okay  && Cyc_Tcutil_unify(_tmp636,_tmp63B)) && (!_tmp637.real_const  || _tmp63C.real_const);}_LL3D1:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp63F=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp630.f1;if(_tmp63F->tag != 19)goto _LL3D3;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp640=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp630.f2;if(_tmp640->tag != 6)goto _LL3D3;};_LL3D2:
# 2205
 return 0;_LL3D3:;_LL3D4:
# 2207
 return Cyc_Tcutil_unify(t1,t2);_LL3CC:;};}
# 2211
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp657=Cyc_Tcutil_compress(t);void*_tmp658=_tmp657;_LL3DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp659=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp658;if(_tmp659->tag != 5)goto _LL3E1;}_LL3E0:
 return 1;_LL3E1:;_LL3E2:
 return 0;_LL3DE:;}
# 2217
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp65A=Cyc_Tcutil_compress(t);void*_tmp65B=_tmp65A;void*_tmp65D;_LL3E4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65B;if(_tmp65C->tag != 5)goto _LL3E6;else{_tmp65D=(_tmp65C->f1).elt_typ;}}_LL3E5:
 return _tmp65D;_LL3E6:;_LL3E7: {
const char*_tmp10F2;void*_tmp10F1;(_tmp10F1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F2="pointer_elt_type",_tag_dyneither(_tmp10F2,sizeof(char),17))),_tag_dyneither(_tmp10F1,sizeof(void*),0)));}_LL3E3:;}
# 2223
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp660=Cyc_Tcutil_compress(t);void*_tmp661=_tmp660;struct Cyc_Absyn_PtrAtts*_tmp663;_LL3E9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp662=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp661;if(_tmp662->tag != 5)goto _LL3EB;else{_tmp663=(struct Cyc_Absyn_PtrAtts*)&(_tmp662->f1).ptr_atts;}}_LL3EA:
 return _tmp663->rgn;_LL3EB:;_LL3EC: {
const char*_tmp10F5;void*_tmp10F4;(_tmp10F4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F5="pointer_elt_type",_tag_dyneither(_tmp10F5,sizeof(char),17))),_tag_dyneither(_tmp10F4,sizeof(void*),0)));}_LL3E8:;}
# 2230
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp666=Cyc_Tcutil_compress(t);void*_tmp667=_tmp666;void*_tmp669;_LL3EE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp668=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp667;if(_tmp668->tag != 5)goto _LL3F0;else{_tmp669=((_tmp668->f1).ptr_atts).rgn;}}_LL3EF:
# 2233
*rgn=_tmp669;
return 1;_LL3F0:;_LL3F1:
 return 0;_LL3ED:;}
# 2240
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2244
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp66A=Cyc_Tcutil_compress(t);void*_tmp66B=_tmp66A;union Cyc_Absyn_Constraint*_tmp66D;_LL3F3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66B;if(_tmp66C->tag != 5)goto _LL3F5;else{_tmp66D=((_tmp66C->f1).ptr_atts).bounds;}}_LL3F4:
# 2247
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66D)== (void*)& Cyc_Absyn_DynEither_b_val;_LL3F5:;_LL3F6:
 return 0;_LL3F2:;}
# 2253
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp66E=e->r;void*_tmp66F=_tmp66E;struct _dyneither_ptr _tmp675;void*_tmp677;struct Cyc_Absyn_Exp*_tmp678;_LL3F8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp670=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp670->tag != 0)goto _LL3FA;else{if(((_tmp670->f1).Int_c).tag != 5)goto _LL3FA;if(((struct _tuple7)((_tmp670->f1).Int_c).val).f2 != 0)goto _LL3FA;}}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp671=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp671->tag != 0)goto _LL3FC;else{if(((_tmp671->f1).Char_c).tag != 2)goto _LL3FC;if(((struct _tuple5)((_tmp671->f1).Char_c).val).f2 != 0)goto _LL3FC;}}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp672->tag != 0)goto _LL3FE;else{if(((_tmp672->f1).Short_c).tag != 4)goto _LL3FE;if(((struct _tuple6)((_tmp672->f1).Short_c).val).f2 != 0)goto _LL3FE;}}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp673=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp673->tag != 0)goto _LL400;else{if(((_tmp673->f1).LongLong_c).tag != 6)goto _LL400;if(((struct _tuple8)((_tmp673->f1).LongLong_c).val).f2 != 0)goto _LL400;}}_LL3FF:
# 2259
 return 1;_LL400: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp674=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp674->tag != 0)goto _LL402;else{if(((_tmp674->f1).Wchar_c).tag != 3)goto _LL402;_tmp675=(struct _dyneither_ptr)((_tmp674->f1).Wchar_c).val;}}_LL401: {
# 2261
unsigned long _tmp679=Cyc_strlen((struct _dyneither_ptr)_tmp675);
int i=0;
if(_tmp679 >= 2  && *((const char*)_check_dyneither_subscript(_tmp675,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp675,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp675,sizeof(char),1))== 'x'  && _tmp679 >= 3) && *((const char*)_check_dyneither_subscript(_tmp675,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp679;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp675,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2271
return 0;}}_LL402: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp676=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp66F;if(_tmp676->tag != 13)goto _LL404;else{_tmp677=(void*)_tmp676->f1;_tmp678=_tmp676->f2;}}_LL403:
 return Cyc_Tcutil_is_zero(_tmp678) && Cyc_Tcutil_admits_zero(_tmp677);_LL404:;_LL405:
 return 0;_LL3F7:;}
# 2277
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2284
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2289
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2294
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2301
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2306
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2311
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp67B;enum Cyc_Absyn_AliasQual _tmp67C;struct Cyc_Absyn_Kind*_tmp67A=ka;_tmp67B=_tmp67A->kind;_tmp67C=_tmp67A->aliasqual;
switch(_tmp67C){case Cyc_Absyn_Aliasable: _LL406:
# 2315
 switch(_tmp67B){case Cyc_Absyn_AnyKind: _LL408:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL409:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL40A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL40B:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL40C:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL40D:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL407:
# 2324
 switch(_tmp67B){case Cyc_Absyn_AnyKind: _LL410:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL411:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL412:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL413:
 return& Cyc_Tcutil_urko;default: _LL414:
 break;}
# 2331
break;case Cyc_Absyn_Top: _LL40F:
# 2333
 switch(_tmp67B){case Cyc_Absyn_AnyKind: _LL417:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL418:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL419:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL41A:
 return& Cyc_Tcutil_trko;default: _LL41B:
 break;}
# 2340
break;}{
# 2342
const char*_tmp10F9;void*_tmp10F8[1];struct Cyc_String_pa_PrintArg_struct _tmp10F7;(_tmp10F7.tag=0,((_tmp10F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10F8[0]=& _tmp10F7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10F9="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp10F9,sizeof(char),26))),_tag_dyneither(_tmp10F8,sizeof(void*),1)))))));};}
# 2345
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2353
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2360
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2365
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2370
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2375
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2380
enum Cyc_Absyn_KindQual _tmp681;enum Cyc_Absyn_AliasQual _tmp682;struct Cyc_Absyn_Kind*_tmp680=ka;_tmp681=_tmp680->kind;_tmp682=_tmp680->aliasqual;
switch(_tmp682){case Cyc_Absyn_Aliasable: _LL41D:
# 2383
 switch(_tmp681){case Cyc_Absyn_AnyKind: _LL41F:
 return& ab;case Cyc_Absyn_MemKind: _LL420:
 return& mb;case Cyc_Absyn_BoxKind: _LL421:
 return& bb;case Cyc_Absyn_RgnKind: _LL422:
 return& rb;case Cyc_Absyn_EffKind: _LL423:
 return& eb;case Cyc_Absyn_IntKind: _LL424:
 return& ib;}case Cyc_Absyn_Unique: _LL41E:
# 2392
 switch(_tmp681){case Cyc_Absyn_AnyKind: _LL427:
 return& uab;case Cyc_Absyn_MemKind: _LL428:
 return& umb;case Cyc_Absyn_BoxKind: _LL429:
 return& ubb;case Cyc_Absyn_RgnKind: _LL42A:
 return& urb;default: _LL42B:
 break;}
# 2399
break;case Cyc_Absyn_Top: _LL426:
# 2401
 switch(_tmp681){case Cyc_Absyn_AnyKind: _LL42E:
 return& tab;case Cyc_Absyn_MemKind: _LL42F:
 return& tmb;case Cyc_Absyn_BoxKind: _LL430:
 return& tbb;case Cyc_Absyn_RgnKind: _LL431:
 return& trb;default: _LL432:
 break;}
# 2408
break;}{
# 2410
const char*_tmp10FD;void*_tmp10FC[1];struct Cyc_String_pa_PrintArg_struct _tmp10FB;(_tmp10FB.tag=0,((_tmp10FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp10FC[0]=& _tmp10FB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10FD="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp10FD,sizeof(char),24))),_tag_dyneither(_tmp10FC,sizeof(void*),1)))))));};}
# 2413
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2416
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2418
struct Cyc_Core_Opt*_tmp10FE;return(_tmp10FE=_cycalloc(sizeof(*_tmp10FE)),((_tmp10FE->v=Cyc_Tcutil_kind_to_bound(k),_tmp10FE)));}
# 2423
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp1101;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp1100;e1->r=(void*)((_tmp1100=_cycalloc(sizeof(*_tmp1100)),((_tmp1100[0]=((_tmp1101.tag=0,((_tmp1101.f1=Cyc_Absyn_Null_c,_tmp1101)))),_tmp1100))));}{
struct Cyc_Core_Opt*_tmp697=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1107;struct Cyc_Absyn_PtrInfo _tmp1106;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1105;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp698=(_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((_tmp1107.tag=5,((_tmp1107.f1=((_tmp1106.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp697),((_tmp1106.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp1106.ptr_atts=(
((_tmp1106.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp697),(((_tmp1106.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp1106.ptr_atts).bounds=
# 2431
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1106.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp1106.ptr_atts).ptrloc=0,_tmp1106.ptr_atts)))))))))),_tmp1106)))))),_tmp1107)))),_tmp1105)));
e1->topt=(void*)_tmp698;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp110C;void*_tmp110B[2];struct Cyc_String_pa_PrintArg_struct _tmp110A;struct Cyc_String_pa_PrintArg_struct _tmp1109;(_tmp1109.tag=0,((_tmp1109.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp110A.tag=0,((_tmp110A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp110B[0]=& _tmp110A,((_tmp110B[1]=& _tmp1109,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp110C="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp110C,sizeof(char),56))),_tag_dyneither(_tmp110B,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2441
return 0;}
# 2444
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL434: {
const char*_tmp110D;return(_tmp110D="unknown",_tag_dyneither(_tmp110D,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL435: {
const char*_tmp110E;return(_tmp110E="no coercion",_tag_dyneither(_tmp110E,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL436: {
const char*_tmp110F;return(_tmp110F="null check",_tag_dyneither(_tmp110F,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL437: {
const char*_tmp1110;return(_tmp1110="other coercion",_tag_dyneither(_tmp1110,sizeof(char),15));}}}
# 2453
int Cyc_Tcutil_warn_alias_coerce=0;
# 2459
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2463
static int counter=0;
struct _dyneither_ptr*_tmp111D;const char*_tmp111C;void*_tmp111B[1];struct Cyc_Int_pa_PrintArg_struct _tmp111A;struct _tuple2*_tmp1119;struct _tuple2*v=(_tmp1119=_cycalloc(sizeof(*_tmp1119)),((_tmp1119->f1=Cyc_Absyn_Loc_n,((_tmp1119->f2=((_tmp111D=_cycalloc(sizeof(*_tmp111D)),((_tmp111D[0]=(struct _dyneither_ptr)((_tmp111A.tag=1,((_tmp111A.f1=(unsigned long)counter ++,((_tmp111B[0]=& _tmp111A,Cyc_aprintf(((_tmp111C="__aliasvar%d",_tag_dyneither(_tmp111C,sizeof(char),13))),_tag_dyneither(_tmp111B,sizeof(void*),1)))))))),_tmp111D)))),_tmp1119)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1120;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp111F;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp111F=_cycalloc(sizeof(*_tmp111F)),((_tmp111F[0]=((_tmp1120.tag=4,((_tmp1120.f1=vd,_tmp1120)))),_tmp111F)))),e->loc);
# 2472
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2474
{void*_tmp6A4=Cyc_Tcutil_compress(e_typ);void*_tmp6A5=_tmp6A4;void*_tmp6A7;struct Cyc_Absyn_Tqual _tmp6A8;void*_tmp6A9;union Cyc_Absyn_Constraint*_tmp6AA;union Cyc_Absyn_Constraint*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;struct Cyc_Absyn_PtrLoc*_tmp6AD;_LL43A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A6=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A5;if(_tmp6A6->tag != 5)goto _LL43C;else{_tmp6A7=(_tmp6A6->f1).elt_typ;_tmp6A8=(_tmp6A6->f1).elt_tq;_tmp6A9=((_tmp6A6->f1).ptr_atts).rgn;_tmp6AA=((_tmp6A6->f1).ptr_atts).nullable;_tmp6AB=((_tmp6A6->f1).ptr_atts).bounds;_tmp6AC=((_tmp6A6->f1).ptr_atts).zero_term;_tmp6AD=((_tmp6A6->f1).ptr_atts).ptrloc;}}_LL43B:
# 2476
{void*_tmp6AE=Cyc_Tcutil_compress(_tmp6A9);void*_tmp6AF=_tmp6AE;void**_tmp6B1;struct Cyc_Core_Opt*_tmp6B2;_LL43F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp6B0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6AF;if(_tmp6B0->tag != 1)goto _LL441;else{_tmp6B1=(void**)((void**)& _tmp6B0->f2);_tmp6B2=_tmp6B0->f4;}}_LL440: {
# 2478
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1123;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1122;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6B3=(_tmp1122=_cycalloc(sizeof(*_tmp1122)),((_tmp1122[0]=((_tmp1123.tag=2,((_tmp1123.f1=tv,_tmp1123)))),_tmp1122)));
*_tmp6B1=(void*)_tmp6B3;
goto _LL43E;}_LL441:;_LL442:
 goto _LL43E;_LL43E:;}
# 2483
goto _LL439;_LL43C:;_LL43D:
 goto _LL439;_LL439:;}
# 2487
e->topt=0;
vd->initializer=0;{
# 2491
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2493
struct _tuple15 _tmp1124;return(_tmp1124.f1=d,((_tmp1124.f2=ve,_tmp1124)));};}
# 2498
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2501
struct _RegionHandle _tmp6BF=_new_region("r");struct _RegionHandle*r=& _tmp6BF;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2506
void*_tmp6C0=Cyc_Tcutil_compress(wants_typ);void*_tmp6C1=_tmp6C0;void*_tmp6C3;_LL444: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6C2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6C1;if(_tmp6C2->tag != 5)goto _LL446;else{_tmp6C3=((_tmp6C2->f1).ptr_atts).rgn;}}_LL445: {
# 2508
void*_tmp6C5=Cyc_Tcutil_compress(_tmp6C3);void*_tmp6C6=_tmp6C5;_LL44B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6C7=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6C6;if(_tmp6C7->tag != 20)goto _LL44D;}_LL44C: {
int _tmp6C8=0;_npop_handler(0);return _tmp6C8;}_LL44D:;_LL44E: {
# 2511
struct Cyc_Absyn_Kind*_tmp6C9=Cyc_Tcutil_typ_kind(_tmp6C3);
int _tmp6CA=_tmp6C9->kind == Cyc_Absyn_RgnKind  && _tmp6C9->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6CA;}_LL44A:;}_LL446: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6C4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6C1;if(_tmp6C4->tag != 17)goto _LL448;}_LL447: {
# 2515
const char*_tmp1129;void*_tmp1128[2];struct Cyc_String_pa_PrintArg_struct _tmp1127;struct Cyc_String_pa_PrintArg_struct _tmp1126;(_tmp1126.tag=0,((_tmp1126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp1127.tag=0,((_tmp1127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp1128[0]=& _tmp1127,((_tmp1128[1]=& _tmp1126,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1129="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp1129,sizeof(char),42))),_tag_dyneither(_tmp1128,sizeof(void*),2)))))))))))));}_LL448:;_LL449: {
int _tmp6CF=0;_npop_handler(0);return _tmp6CF;}_LL443:;}{
# 2520
int _tmp6D0=0;_npop_handler(0);return _tmp6D0;};
# 2502
;_pop_region(r);}
# 2524
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2529
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2531
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2533
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp112E;void*_tmp112D[2];struct Cyc_String_pa_PrintArg_struct _tmp112C;struct Cyc_String_pa_PrintArg_struct _tmp112B;(_tmp112B.tag=0,((_tmp112B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp112C.tag=0,((_tmp112C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp112D[0]=& _tmp112C,((_tmp112D[1]=& _tmp112B,Cyc_Tcutil_warn(e->loc,((_tmp112E="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp112E,sizeof(char),53))),_tag_dyneither(_tmp112D,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2542
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp1134;void*_tmp1133[3];struct Cyc_String_pa_PrintArg_struct _tmp1132;struct Cyc_String_pa_PrintArg_struct _tmp1131;struct Cyc_String_pa_PrintArg_struct _tmp1130;(_tmp1130.tag=0,((_tmp1130.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1131.tag=0,((_tmp1131.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1132.tag=0,((_tmp1132.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp1133[0]=& _tmp1132,((_tmp1133[1]=& _tmp1131,((_tmp1133[2]=& _tmp1130,Cyc_Tcutil_warn(e->loc,((_tmp1134="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp1134,sizeof(char),40))),_tag_dyneither(_tmp1133,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2549
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2557
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp1139;void*_tmp1138[2];struct Cyc_String_pa_PrintArg_struct _tmp1137;struct Cyc_String_pa_PrintArg_struct _tmp1136;(_tmp1136.tag=0,((_tmp1136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp1137.tag=0,((_tmp1137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp1138[0]=& _tmp1137,((_tmp1138[1]=& _tmp1136,Cyc_Tcutil_warn(e->loc,((_tmp1139="implicit cast from %s to %s",_tag_dyneither(_tmp1139,sizeof(char),28))),_tag_dyneither(_tmp1138,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2563
return 0;}}}}}
# 2570
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2573
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2577
int Cyc_Tcutil_coerceable(void*t){
void*_tmp6DE=Cyc_Tcutil_compress(t);void*_tmp6DF=_tmp6DE;_LL450: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E0->tag != 6)goto _LL452;}_LL451:
 goto _LL453;_LL452: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6E1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6DF;if(_tmp6E1->tag != 7)goto _LL454;}_LL453:
 return 1;_LL454:;_LL455:
 return 0;_LL44F:;}
# 2595 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2599
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2602
struct _tuple23 _tmp6E2=*env;struct Cyc_List_List*_tmp6E4;struct _RegionHandle*_tmp6E5;struct Cyc_Tcenv_Tenv*_tmp6E6;int _tmp6E7;struct _tuple23 _tmp6E3=_tmp6E2;_tmp6E4=_tmp6E3.f1;_tmp6E5=_tmp6E3.f2;_tmp6E6=_tmp6E3.f3;_tmp6E7=_tmp6E3.f4;{
# 2604
void*_tmp6E8=_tmp6E4 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6E5,_tmp6E4,x->type);
struct Cyc_List_List*_tmp6E9=Cyc_Tcutil_flatten_typ(_tmp6E5,_tmp6E7,_tmp6E6,_tmp6E8);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6E9)== 1){
struct _tuple12*_tmp113C;struct Cyc_List_List*_tmp113B;return(_tmp113B=_region_malloc(_tmp6E5,sizeof(*_tmp113B)),((_tmp113B->hd=((_tmp113C=_region_malloc(_tmp6E5,sizeof(*_tmp113C)),((_tmp113C->f1=x->tq,((_tmp113C->f2=_tmp6E8,_tmp113C)))))),((_tmp113B->tl=0,_tmp113B)))));}else{
return _tmp6E9;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2610
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2612
struct _tuple24 _tmp6EC=*env;struct _RegionHandle*_tmp6EE;struct Cyc_Tcenv_Tenv*_tmp6EF;int _tmp6F0;struct _tuple24 _tmp6ED=_tmp6EC;_tmp6EE=_tmp6ED.f1;_tmp6EF=_tmp6ED.f2;_tmp6F0=_tmp6ED.f3;{
struct _tuple12 _tmp6F1=*x;struct Cyc_Absyn_Tqual _tmp6F3;void*_tmp6F4;struct _tuple12 _tmp6F2=_tmp6F1;_tmp6F3=_tmp6F2.f1;_tmp6F4=_tmp6F2.f2;{
struct Cyc_List_List*_tmp6F5=Cyc_Tcutil_flatten_typ(_tmp6EE,_tmp6F0,_tmp6EF,_tmp6F4);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6F5)== 1){
struct _tuple12*_tmp113F;struct Cyc_List_List*_tmp113E;return(_tmp113E=_region_malloc(_tmp6EE,sizeof(*_tmp113E)),((_tmp113E->hd=((_tmp113F=_region_malloc(_tmp6EE,sizeof(*_tmp113F)),((_tmp113F->f1=_tmp6F3,((_tmp113F->f2=_tmp6F4,_tmp113F)))))),((_tmp113E->tl=0,_tmp113E)))));}else{
return _tmp6F5;}};};}
# 2619
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2623
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp6F8=t1;struct Cyc_List_List*_tmp6FB;struct Cyc_Absyn_Aggrdecl*_tmp6FD;struct Cyc_List_List*_tmp6FE;struct Cyc_List_List*_tmp700;_LL457: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6F9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6F8;if(_tmp6F9->tag != 0)goto _LL459;}_LL458:
 return 0;_LL459: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6FA=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6F8;if(_tmp6FA->tag != 10)goto _LL45B;else{_tmp6FB=_tmp6FA->f1;}}_LL45A: {
# 2628
struct _tuple24 _tmp1140;struct _tuple24 _tmp701=(_tmp1140.f1=r,((_tmp1140.f2=te,((_tmp1140.f3=flatten,_tmp1140)))));
# 2630
struct Cyc_List_List*_tmp702=_tmp6FB;struct _tuple12*_tmp703;struct Cyc_List_List*_tmp704;_LL462: if(_tmp702 != 0)goto _LL464;_LL463:
 return 0;_LL464: if(_tmp702 == 0)goto _LL461;_tmp703=(struct _tuple12*)_tmp702->hd;_tmp704=_tmp702->tl;_LL465: {
# 2633
struct Cyc_List_List*_tmp705=Cyc_Tcutil_rcopy_tqt(& _tmp701,_tmp703);
_tmp701.f3=0;{
struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp701,_tmp6FB);
struct Cyc_List_List*_tmp1141;struct Cyc_List_List*_tmp707=(_tmp1141=_region_malloc(r,sizeof(*_tmp1141)),((_tmp1141->hd=_tmp705,((_tmp1141->tl=_tmp706,_tmp1141)))));
struct Cyc_List_List*_tmp1142;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp1142=_region_malloc(r,sizeof(*_tmp1142)),((_tmp1142->hd=_tmp705,((_tmp1142->tl=_tmp706,_tmp1142)))))));};}_LL461:;}_LL45B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6FC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6F8;if(_tmp6FC->tag != 11)goto _LL45D;else{if((((_tmp6FC->f1).aggr_info).KnownAggr).tag != 2)goto _LL45D;_tmp6FD=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6FC->f1).aggr_info).KnownAggr).val);_tmp6FE=(_tmp6FC->f1).targs;}}_LL45C:
# 2641
 if(((_tmp6FD->kind == Cyc_Absyn_UnionA  || _tmp6FD->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FD->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FD->impl))->rgn_po != 0){
# 2643
struct _tuple12*_tmp1145;struct Cyc_List_List*_tmp1144;return(_tmp1144=_region_malloc(r,sizeof(*_tmp1144)),((_tmp1144->hd=((_tmp1145=_region_malloc(r,sizeof(*_tmp1145)),((_tmp1145->f1=Cyc_Absyn_empty_tqual(0),((_tmp1145->f2=t1,_tmp1145)))))),((_tmp1144->tl=0,_tmp1144)))));}{
struct Cyc_List_List*_tmp70D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6FD->tvs,_tmp6FE);
struct _tuple23 _tmp1146;struct _tuple23 env=(_tmp1146.f1=_tmp70D,((_tmp1146.f2=r,((_tmp1146.f3=te,((_tmp1146.f4=flatten,_tmp1146)))))));
struct Cyc_List_List*_tmp70E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6FD->impl))->fields;struct Cyc_List_List*_tmp70F=_tmp70E;struct Cyc_Absyn_Aggrfield*_tmp710;struct Cyc_List_List*_tmp711;_LL467: if(_tmp70F != 0)goto _LL469;_LL468:
 return 0;_LL469: if(_tmp70F == 0)goto _LL466;_tmp710=(struct Cyc_Absyn_Aggrfield*)_tmp70F->hd;_tmp711=_tmp70F->tl;_LL46A: {
# 2649
struct Cyc_List_List*_tmp712=Cyc_Tcutil_flatten_typ_f(& env,_tmp710);
env.f4=0;{
struct Cyc_List_List*_tmp713=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp711);
struct Cyc_List_List*_tmp1147;struct Cyc_List_List*_tmp714=(_tmp1147=_region_malloc(r,sizeof(*_tmp1147)),((_tmp1147->hd=_tmp712,((_tmp1147->tl=_tmp713,_tmp1147)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp714);};}_LL466:;};_LL45D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6FF=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F8;if(_tmp6FF->tag != 12)goto _LL45F;else{if(_tmp6FF->f1 != Cyc_Absyn_StructA)goto _LL45F;_tmp700=_tmp6FF->f2;}}_LL45E: {
# 2656
struct _tuple23 _tmp1148;struct _tuple23 env=(_tmp1148.f1=0,((_tmp1148.f2=r,((_tmp1148.f3=te,((_tmp1148.f4=flatten,_tmp1148)))))));
struct Cyc_List_List*_tmp717=_tmp700;struct Cyc_Absyn_Aggrfield*_tmp718;struct Cyc_List_List*_tmp719;_LL46C: if(_tmp717 != 0)goto _LL46E;_LL46D:
 return 0;_LL46E: if(_tmp717 == 0)goto _LL46B;_tmp718=(struct Cyc_Absyn_Aggrfield*)_tmp717->hd;_tmp719=_tmp717->tl;_LL46F: {
# 2660
struct Cyc_List_List*_tmp71A=Cyc_Tcutil_flatten_typ_f(& env,_tmp718);
env.f4=0;{
struct Cyc_List_List*_tmp71B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp719);
struct Cyc_List_List*_tmp1149;struct Cyc_List_List*_tmp71C=(_tmp1149=_region_malloc(r,sizeof(*_tmp1149)),((_tmp1149->hd=_tmp71A,((_tmp1149->tl=_tmp71B,_tmp1149)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp71C);};}_LL46B:;}_LL45F:;_LL460:
# 2666
 goto _LL456;_LL456:;};}{
# 2669
struct _tuple12*_tmp114C;struct Cyc_List_List*_tmp114B;return(_tmp114B=_region_malloc(r,sizeof(*_tmp114B)),((_tmp114B->hd=((_tmp114C=_region_malloc(r,sizeof(*_tmp114C)),((_tmp114C->f1=Cyc_Absyn_empty_tqual(0),((_tmp114C->f2=t1,_tmp114C)))))),((_tmp114B->tl=0,_tmp114B)))));};}
# 2673
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp721=(void*)t->hd;void*_tmp722=_tmp721;_LL471: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp723=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp722;if(_tmp723->tag != 23)goto _LL473;}_LL472:
 goto _LL474;_LL473: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp724=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp722;if(_tmp724->tag != 4)goto _LL475;}_LL474:
 goto _LL476;_LL475: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp725=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp722;if(_tmp725->tag != 20)goto _LL477;}_LL476:
# 2679
 continue;_LL477:;_LL478:
# 2681
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL470:;}}
# 2684
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2687
return 1;}
# 2690
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp114D;struct _tuple0 _tmp726=(_tmp114D.f1=Cyc_Tcutil_compress(t1),((_tmp114D.f2=Cyc_Tcutil_compress(t2),_tmp114D)));struct _tuple0 _tmp727=_tmp726;enum Cyc_Absyn_Size_of _tmp729;enum Cyc_Absyn_Size_of _tmp72B;_LL47A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp728=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp727.f1;if(_tmp728->tag != 6)goto _LL47C;else{_tmp729=_tmp728->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp72A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp727.f2;if(_tmp72A->tag != 6)goto _LL47C;else{_tmp72B=_tmp72A->f2;}};_LL47B:
# 2693
 return(_tmp729 == _tmp72B  || _tmp729 == Cyc_Absyn_Int_sz  && _tmp72B == Cyc_Absyn_Long_sz) || 
_tmp729 == Cyc_Absyn_Long_sz  && _tmp72B == Cyc_Absyn_Int_sz;_LL47C:;_LL47D:
 return 0;_LL479:;}
# 2701
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2704
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2709
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp114E;struct _tuple0 _tmp72D=(_tmp114E.f1=t1,((_tmp114E.f2=t2,_tmp114E)));struct _tuple0 _tmp72E=_tmp72D;void*_tmp730;struct Cyc_Absyn_Tqual _tmp731;void*_tmp732;union Cyc_Absyn_Constraint*_tmp733;union Cyc_Absyn_Constraint*_tmp734;union Cyc_Absyn_Constraint*_tmp735;void*_tmp737;struct Cyc_Absyn_Tqual _tmp738;void*_tmp739;union Cyc_Absyn_Constraint*_tmp73A;union Cyc_Absyn_Constraint*_tmp73B;union Cyc_Absyn_Constraint*_tmp73C;struct Cyc_Absyn_Datatypedecl*_tmp73E;struct Cyc_Absyn_Datatypefield*_tmp73F;struct Cyc_List_List*_tmp740;struct Cyc_Absyn_Datatypedecl*_tmp742;struct Cyc_List_List*_tmp743;struct Cyc_Absyn_FnInfo _tmp745;struct Cyc_Absyn_FnInfo _tmp747;_LL47F:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp72F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72E.f1;if(_tmp72F->tag != 5)goto _LL481;else{_tmp730=(_tmp72F->f1).elt_typ;_tmp731=(_tmp72F->f1).elt_tq;_tmp732=((_tmp72F->f1).ptr_atts).rgn;_tmp733=((_tmp72F->f1).ptr_atts).nullable;_tmp734=((_tmp72F->f1).ptr_atts).bounds;_tmp735=((_tmp72F->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp736=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72E.f2;if(_tmp736->tag != 5)goto _LL481;else{_tmp737=(_tmp736->f1).elt_typ;_tmp738=(_tmp736->f1).elt_tq;_tmp739=((_tmp736->f1).ptr_atts).rgn;_tmp73A=((_tmp736->f1).ptr_atts).nullable;_tmp73B=((_tmp736->f1).ptr_atts).bounds;_tmp73C=((_tmp736->f1).ptr_atts).zero_term;}};_LL480:
# 2717
 if(_tmp731.real_const  && !_tmp738.real_const)
return 0;
# 2720
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp733,_tmp73A) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp733)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp73A))
return 0;
# 2724
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp735,_tmp73C) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp735)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp73C))
return 0;
# 2728
if((!Cyc_Tcutil_unify(_tmp732,_tmp739) && !Cyc_Tcenv_region_outlives(te,_tmp732,_tmp739)) && !
Cyc_Tcutil_subtype(te,assume,_tmp732,_tmp739))
return 0;
# 2732
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp734,_tmp73B)){
struct _tuple0 _tmp114F;struct _tuple0 _tmp748=(_tmp114F.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp734),((_tmp114F.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp73B),_tmp114F)));struct _tuple0 _tmp749=_tmp748;struct Cyc_Absyn_Exp*_tmp74D;struct Cyc_Absyn_Exp*_tmp74F;_LL488:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp74A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp749.f1;if(_tmp74A->tag != 0)goto _LL48A;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp74B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp749.f2;if(_tmp74B->tag != 0)goto _LL48A;};_LL489:
 goto _LL487;_LL48A:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp74C=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp749.f1;if(_tmp74C->tag != 1)goto _LL48C;else{_tmp74D=_tmp74C->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp74E=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp749.f2;if(_tmp74E->tag != 1)goto _LL48C;else{_tmp74F=_tmp74E->f1;}};_LL48B:
# 2737
 if(!Cyc_Evexp_lte_const_exp(_tmp74F,_tmp74D))
return 0;
goto _LL487;_LL48C:;_LL48D:
 return 0;_LL487:;}
# 2745
if(!_tmp738.real_const  && _tmp731.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp737)))
return 0;}{
# 2751
int _tmp751=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp73B,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp73C);
# 2757
struct _tuple0*_tmp1152;struct Cyc_List_List*_tmp1151;return(_tmp751  && Cyc_Tcutil_ptrsubtype(te,((_tmp1151=_cycalloc(sizeof(*_tmp1151)),((_tmp1151->hd=((_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152->f1=t1,((_tmp1152->f2=t2,_tmp1152)))))),((_tmp1151->tl=assume,_tmp1151)))))),_tmp730,_tmp737) || Cyc_Tcutil_unify(_tmp730,_tmp737)) || Cyc_Tcutil_isomorphic(_tmp730,_tmp737);};_LL481:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp73D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp72E.f1;if(_tmp73D->tag != 4)goto _LL483;else{if((((_tmp73D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL483;_tmp73E=((struct _tuple3)(((_tmp73D->f1).field_info).KnownDatatypefield).val).f1;_tmp73F=((struct _tuple3)(((_tmp73D->f1).field_info).KnownDatatypefield).val).f2;_tmp740=(_tmp73D->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp72E.f2;if(_tmp741->tag != 3)goto _LL483;else{if((((_tmp741->f1).datatype_info).KnownDatatype).tag != 2)goto _LL483;_tmp742=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp741->f1).datatype_info).KnownDatatype).val);_tmp743=(_tmp741->f1).targs;}};_LL482:
# 2763
 if(_tmp73E != _tmp742  && Cyc_Absyn_qvar_cmp(_tmp73E->name,_tmp742->name)!= 0)return 0;
# 2765
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp740)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp743))return 0;
for(0;_tmp740 != 0;(_tmp740=_tmp740->tl,_tmp743=_tmp743->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp740->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp743))->hd))return 0;}
return 1;_LL483:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp744=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72E.f1;if(_tmp744->tag != 9)goto _LL485;else{_tmp745=_tmp744->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp746=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp72E.f2;if(_tmp746->tag != 9)goto _LL485;else{_tmp747=_tmp746->f1;}};_LL484:
# 2772
 if(_tmp745.tvars != 0  || _tmp747.tvars != 0){
struct Cyc_List_List*_tmp754=_tmp745.tvars;
struct Cyc_List_List*_tmp755=_tmp747.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp754)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp755))return 0;{
struct _RegionHandle _tmp756=_new_region("r");struct _RegionHandle*r=& _tmp756;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp754 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp754->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp755))->hd)->kind)){int _tmp757=0;_npop_handler(0);return _tmp757;}
{struct _tuple16*_tmp115C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp115B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp115A;struct Cyc_List_List*_tmp1159;inst=((_tmp1159=_region_malloc(r,sizeof(*_tmp1159)),((_tmp1159->hd=((_tmp115C=_region_malloc(r,sizeof(*_tmp115C)),((_tmp115C->f1=(struct Cyc_Absyn_Tvar*)_tmp755->hd,((_tmp115C->f2=(void*)((_tmp115A=_cycalloc(sizeof(*_tmp115A)),((_tmp115A[0]=((_tmp115B.tag=2,((_tmp115B.f1=(struct Cyc_Absyn_Tvar*)_tmp754->hd,_tmp115B)))),_tmp115A)))),_tmp115C)))))),((_tmp1159->tl=inst,_tmp1159))))));}
_tmp754=_tmp754->tl;
_tmp755=_tmp755->tl;}
# 2784
if(inst != 0){
_tmp745.tvars=0;
_tmp747.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1162;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1161;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp115F;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp115E;int _tmp760=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E[0]=((_tmp115F.tag=9,((_tmp115F.f1=_tmp745,_tmp115F)))),_tmp115E)))),(void*)((_tmp1161=_cycalloc(sizeof(*_tmp1161)),((_tmp1161[0]=((_tmp1162.tag=9,((_tmp1162.f1=_tmp747,_tmp1162)))),_tmp1161)))));_npop_handler(0);return _tmp760;};}}
# 2777
;_pop_region(r);};}
# 2792
if(!Cyc_Tcutil_subtype(te,assume,_tmp745.ret_typ,_tmp747.ret_typ))return 0;{
struct Cyc_List_List*_tmp761=_tmp745.args;
struct Cyc_List_List*_tmp762=_tmp747.args;
# 2797
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp761)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp762))return 0;
# 2799
for(0;_tmp761 != 0;(_tmp761=_tmp761->tl,_tmp762=_tmp762->tl)){
struct _tuple10 _tmp763=*((struct _tuple10*)_tmp761->hd);struct Cyc_Absyn_Tqual _tmp765;void*_tmp766;struct _tuple10 _tmp764=_tmp763;_tmp765=_tmp764.f2;_tmp766=_tmp764.f3;{
struct _tuple10 _tmp767=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp762))->hd);struct Cyc_Absyn_Tqual _tmp769;void*_tmp76A;struct _tuple10 _tmp768=_tmp767;_tmp769=_tmp768.f2;_tmp76A=_tmp768.f3;
# 2803
if(_tmp769.real_const  && !_tmp765.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp76A,_tmp766))
return 0;};}
# 2807
if(_tmp745.c_varargs != _tmp747.c_varargs)return 0;
if(_tmp745.cyc_varargs != 0  && _tmp747.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp76B=*_tmp745.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp76C=*_tmp747.cyc_varargs;
# 2812
if((_tmp76C.tq).real_const  && !(_tmp76B.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp76C.type,_tmp76B.type))
return 0;}else{
if(_tmp745.cyc_varargs != 0  || _tmp747.cyc_varargs != 0)return 0;}
# 2817
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp745.effect),(void*)_check_null(_tmp747.effect)))return 0;
# 2819
if(!Cyc_Tcutil_sub_rgnpo(_tmp745.rgn_po,_tmp747.rgn_po))return 0;
# 2821
if(!Cyc_Tcutil_sub_attributes(_tmp745.attributes,_tmp747.attributes))return 0;
# 2823
if(!Cyc_Tcutil_check_logical_implication(_tmp747.requires_relns,_tmp745.requires_relns))
return 0;
# 2826
if(!Cyc_Tcutil_check_logical_implication(_tmp745.ensures_relns,_tmp747.ensures_relns))
return 0;
# 2829
return 1;};_LL485:;_LL486:
 return 0;_LL47E:;};}
# 2841 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2843
struct _RegionHandle _tmp76E=_new_region("temp");struct _RegionHandle*temp=& _tmp76E;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp76F=0;_npop_handler(0);return _tmp76F;}{
struct _tuple12*_tmp770=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp772;void*_tmp773;struct _tuple12*_tmp771=_tmp770;_tmp772=_tmp771->f1;_tmp773=_tmp771->f2;{
struct _tuple12*_tmp774=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp776;void*_tmp777;struct _tuple12*_tmp775=_tmp774;_tmp776=_tmp775->f1;_tmp777=_tmp775->f2;
# 2851
if(_tmp772.real_const  && !_tmp776.real_const){int _tmp778=0;_npop_handler(0);return _tmp778;}
# 2853
if((_tmp776.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp777))) && 
Cyc_Tcutil_subtype(te,assume,_tmp773,_tmp777))
# 2856
continue;
# 2858
if(Cyc_Tcutil_unify(_tmp773,_tmp777))
# 2860
continue;
# 2862
if(Cyc_Tcutil_isomorphic(_tmp773,_tmp777))
# 2864
continue;{
# 2867
int _tmp779=0;_npop_handler(0);return _tmp779;};};};}{
# 2869
int _tmp77A=1;_npop_handler(0);return _tmp77A;};}
# 2844
;_pop_region(temp);}
# 2873
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp77B=Cyc_Tcutil_compress(t);void*_tmp77C=_tmp77B;_LL48F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp77D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77C;if(_tmp77D->tag != 6)goto _LL491;else{if(_tmp77D->f2 != Cyc_Absyn_Char_sz)goto _LL491;}}_LL490:
 return 1;_LL491:;_LL492:
 return 0;_LL48E:;}
# 2882
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2888
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp77E=t2;_LL494: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp77F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77E;if(_tmp77F->tag != 6)goto _LL496;else{if(_tmp77F->f2 != Cyc_Absyn_Int_sz)goto _LL496;}}_LL495:
# 2892
 goto _LL497;_LL496: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp780=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp77E;if(_tmp780->tag != 6)goto _LL498;else{if(_tmp780->f2 != Cyc_Absyn_Long_sz)goto _LL498;}}_LL497:
# 2894
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL493;_LL498:;_LL499:
 goto _LL493;_LL493:;}{
# 2898
void*_tmp781=t1;void*_tmp783;struct Cyc_Absyn_Tqual _tmp784;void*_tmp785;union Cyc_Absyn_Constraint*_tmp786;union Cyc_Absyn_Constraint*_tmp787;union Cyc_Absyn_Constraint*_tmp788;void*_tmp78A;struct Cyc_Absyn_Tqual _tmp78B;struct Cyc_Absyn_Exp*_tmp78C;union Cyc_Absyn_Constraint*_tmp78D;struct Cyc_Absyn_Enumdecl*_tmp78F;void*_tmp793;_LL49B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp782=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp781;if(_tmp782->tag != 5)goto _LL49D;else{_tmp783=(_tmp782->f1).elt_typ;_tmp784=(_tmp782->f1).elt_tq;_tmp785=((_tmp782->f1).ptr_atts).rgn;_tmp786=((_tmp782->f1).ptr_atts).nullable;_tmp787=((_tmp782->f1).ptr_atts).bounds;_tmp788=((_tmp782->f1).ptr_atts).zero_term;}}_LL49C:
# 2906
{void*_tmp794=t2;void*_tmp796;struct Cyc_Absyn_Tqual _tmp797;void*_tmp798;union Cyc_Absyn_Constraint*_tmp799;union Cyc_Absyn_Constraint*_tmp79A;union Cyc_Absyn_Constraint*_tmp79B;_LL4AA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp795=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp794;if(_tmp795->tag != 5)goto _LL4AC;else{_tmp796=(_tmp795->f1).elt_typ;_tmp797=(_tmp795->f1).elt_tq;_tmp798=((_tmp795->f1).ptr_atts).rgn;_tmp799=((_tmp795->f1).ptr_atts).nullable;_tmp79A=((_tmp795->f1).ptr_atts).bounds;_tmp79B=((_tmp795->f1).ptr_atts).zero_term;}}_LL4AB: {
# 2910
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1165;struct Cyc_List_List*_tmp1164;struct Cyc_List_List*_tmp79C=(_tmp1164=_cycalloc(sizeof(*_tmp1164)),((_tmp1164->hd=((_tmp1165=_cycalloc(sizeof(*_tmp1165)),((_tmp1165->f1=t1,((_tmp1165->f2=t2,_tmp1165)))))),((_tmp1164->tl=0,_tmp1164)))));
int _tmp79D=_tmp797.real_const  || !_tmp784.real_const;
# 2924 "tcutil.cyc"
int _tmp79E=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp79A,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp79B);
# 2928
int _tmp79F=_tmp79D  && (
((_tmp79E  && Cyc_Tcutil_ptrsubtype(te,_tmp79C,_tmp783,_tmp796) || 
Cyc_Tcutil_unify(_tmp783,_tmp796)) || Cyc_Tcutil_isomorphic(_tmp783,_tmp796)) || Cyc_Tcutil_unify(_tmp796,(void*)& Cyc_Absyn_VoidType_val));
# 2932
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp788,_tmp79B) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp79B);
# 2936
int _tmp7A0=_tmp79F?0:((Cyc_Tcutil_bits_only(_tmp783) && Cyc_Tcutil_is_char_type(_tmp796)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp79B)) && (
_tmp797.real_const  || !_tmp784.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp787,_tmp79A);
if(!bounds_ok  && !_tmp7A0){
struct _tuple0 _tmp1166;struct _tuple0 _tmp7A1=(_tmp1166.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp787),((_tmp1166.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp79A),_tmp1166)));struct _tuple0 _tmp7A2=_tmp7A1;struct Cyc_Absyn_Exp*_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A6;_LL4AF:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7A3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7A2.f1;if(_tmp7A3->tag != 1)goto _LL4B1;else{_tmp7A4=_tmp7A3->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp7A5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7A2.f2;if(_tmp7A5->tag != 1)goto _LL4B1;else{_tmp7A6=_tmp7A5->f1;}};_LL4B0:
# 2946
 if(Cyc_Evexp_lte_const_exp(_tmp7A6,_tmp7A4))
bounds_ok=1;
goto _LL4AE;_LL4B1:;_LL4B2:
# 2951
 bounds_ok=1;goto _LL4AE;_LL4AE:;}
# 2953
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp786) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp799))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2958
if(((bounds_ok  && zeroterm_ok) && (_tmp79F  || _tmp7A0)) && (
Cyc_Tcutil_unify(_tmp785,_tmp798) || Cyc_Tcenv_region_outlives(te,_tmp785,_tmp798)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL4AC:;_LL4AD:
 goto _LL4A9;_LL4A9:;}
# 2964
return Cyc_Absyn_Unknown_coercion;_LL49D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp789=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp781;if(_tmp789->tag != 8)goto _LL49F;else{_tmp78A=(_tmp789->f1).elt_type;_tmp78B=(_tmp789->f1).tq;_tmp78C=(_tmp789->f1).num_elts;_tmp78D=(_tmp789->f1).zero_term;}}_LL49E:
# 2966
{void*_tmp7AA=t2;void*_tmp7AC;struct Cyc_Absyn_Tqual _tmp7AD;struct Cyc_Absyn_Exp*_tmp7AE;union Cyc_Absyn_Constraint*_tmp7AF;_LL4B4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7AB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp7AA;if(_tmp7AB->tag != 8)goto _LL4B6;else{_tmp7AC=(_tmp7AB->f1).elt_type;_tmp7AD=(_tmp7AB->f1).tq;_tmp7AE=(_tmp7AB->f1).num_elts;_tmp7AF=(_tmp7AB->f1).zero_term;}}_LL4B5: {
# 2968
int okay;
okay=
(((_tmp78C != 0  && _tmp7AE != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp78D,_tmp7AF)) && 
Cyc_Evexp_lte_const_exp(_tmp7AE,_tmp78C)) && 
Cyc_Evexp_lte_const_exp(_tmp78C,_tmp7AE);
return
# 2975
(okay  && Cyc_Tcutil_unify(_tmp78A,_tmp7AC)) && (!_tmp78B.real_const  || _tmp7AD.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL4B6:;_LL4B7:
# 2977
 return Cyc_Absyn_Unknown_coercion;_LL4B3:;}
# 2979
return Cyc_Absyn_Unknown_coercion;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp78E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp781;if(_tmp78E->tag != 13)goto _LL4A1;else{_tmp78F=_tmp78E->f2;}}_LL4A0:
# 2983
{void*_tmp7B0=t2;struct Cyc_Absyn_Enumdecl*_tmp7B2;_LL4B9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7B1=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7B0;if(_tmp7B1->tag != 13)goto _LL4BB;else{_tmp7B2=_tmp7B1->f2;}}_LL4BA:
# 2985
 if((_tmp78F->fields != 0  && _tmp7B2->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp78F->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp7B2->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL4B8;_LL4BB:;_LL4BC:
 goto _LL4B8;_LL4B8:;}
# 2991
goto _LL4A2;_LL4A1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp790=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp781;if(_tmp790->tag != 6)goto _LL4A3;}_LL4A2:
 goto _LL4A4;_LL4A3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp791=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp781;if(_tmp791->tag != 7)goto _LL4A5;}_LL4A4:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL4A5: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp792=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp781;if(_tmp792->tag != 15)goto _LL4A7;else{_tmp793=(void*)_tmp792->f1;}}_LL4A6:
# 2996
{void*_tmp7B3=t2;void*_tmp7B5;_LL4BE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp7B4=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp7B3;if(_tmp7B4->tag != 15)goto _LL4C0;else{_tmp7B5=(void*)_tmp7B4->f1;}}_LL4BF:
# 2998
 if(Cyc_Tcenv_region_outlives(te,_tmp793,_tmp7B5))return Cyc_Absyn_No_coercion;
goto _LL4BD;_LL4C0:;_LL4C1:
 goto _LL4BD;_LL4BD:;}
# 3002
return Cyc_Absyn_Unknown_coercion;_LL4A7:;_LL4A8:
 return Cyc_Absyn_Unknown_coercion;_LL49A:;};}
# 3008
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1169;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1168;e->r=(void*)((_tmp1168=_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=((_tmp1169.tag=13,((_tmp1169.f1=t,((_tmp1169.f2=_tmp7B6,((_tmp1169.f3=0,((_tmp1169.f4=c,_tmp1169)))))))))),_tmp1168))));}
e->topt=t;}}
# 3016
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp7B9=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7BA=_tmp7B9;_LL4C3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7BB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BA;if(_tmp7BB->tag != 6)goto _LL4C5;}_LL4C4:
 goto _LL4C6;_LL4C5: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7BC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7BA;if(_tmp7BC->tag != 13)goto _LL4C7;}_LL4C6:
 goto _LL4C8;_LL4C7: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7BD=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp7BA;if(_tmp7BD->tag != 14)goto _LL4C9;}_LL4C8:
 goto _LL4CA;_LL4C9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7BE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp7BA;if(_tmp7BE->tag != 19)goto _LL4CB;}_LL4CA:
 return 1;_LL4CB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7BF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp7BA;if(_tmp7BF->tag != 1)goto _LL4CD;}_LL4CC:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4CD:;_LL4CE:
 return 0;_LL4C2:;}
# 3027
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp7C0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp7C1=_tmp7C0;_LL4D0: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C1;if(_tmp7C2->tag != 7)goto _LL4D2;}_LL4D1:
 return 1;_LL4D2:;_LL4D3:
 return 0;_LL4CF:;};}
# 3036
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp7C3=Cyc_Tcutil_compress(t);void*_tmp7C4=_tmp7C3;_LL4D5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7C5=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7C4;if(_tmp7C5->tag != 9)goto _LL4D7;}_LL4D6:
 return 1;_LL4D7:;_LL4D8:
 return 0;_LL4D4:;}
# 3043
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp116A;struct _tuple0 _tmp7C6=(_tmp116A.f1=t1,((_tmp116A.f2=t2,_tmp116A)));struct _tuple0 _tmp7C7=_tmp7C6;int _tmp7C9;int _tmp7CB;_LL4DA:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7C8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7C8->tag != 7)goto _LL4DC;else{_tmp7C9=_tmp7C8->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CA=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7CA->tag != 7)goto _LL4DC;else{_tmp7CB=_tmp7CA->f1;}};_LL4DB:
# 3046
 if(_tmp7C9 != 0  && _tmp7C9 != 1)return t1;else{
if(_tmp7CB != 0  && _tmp7CB != 1)return t2;else{
if(_tmp7C9 >= _tmp7CB)return t1;else{
return t2;}}}_LL4DC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7CC->tag != 7)goto _LL4DE;}_LL4DD:
 return t1;_LL4DE: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7CD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7CD->tag != 7)goto _LL4E0;}_LL4DF:
 return t2;_LL4E0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7CE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7CE->tag != 6)goto _LL4E2;else{if(_tmp7CE->f1 != Cyc_Absyn_Unsigned)goto _LL4E2;if(_tmp7CE->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E2;}}_LL4E1:
 goto _LL4E3;_LL4E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7CF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7CF->tag != 6)goto _LL4E4;else{if(_tmp7CF->f1 != Cyc_Absyn_Unsigned)goto _LL4E4;if(_tmp7CF->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E4;}}_LL4E3:
 return Cyc_Absyn_ulonglong_typ;_LL4E4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7D0->tag != 6)goto _LL4E6;else{if(_tmp7D0->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E6;}}_LL4E5:
 goto _LL4E7;_LL4E6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7D1->tag != 6)goto _LL4E8;else{if(_tmp7D1->f2 != Cyc_Absyn_LongLong_sz)goto _LL4E8;}}_LL4E7:
 return Cyc_Absyn_slonglong_typ;_LL4E8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7D2->tag != 6)goto _LL4EA;else{if(_tmp7D2->f1 != Cyc_Absyn_Unsigned)goto _LL4EA;if(_tmp7D2->f2 != Cyc_Absyn_Long_sz)goto _LL4EA;}}_LL4E9:
 goto _LL4EB;_LL4EA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7D3->tag != 6)goto _LL4EC;else{if(_tmp7D3->f1 != Cyc_Absyn_Unsigned)goto _LL4EC;if(_tmp7D3->f2 != Cyc_Absyn_Long_sz)goto _LL4EC;}}_LL4EB:
 return Cyc_Absyn_ulong_typ;_LL4EC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7D4->tag != 6)goto _LL4EE;else{if(_tmp7D4->f1 != Cyc_Absyn_Unsigned)goto _LL4EE;if(_tmp7D4->f2 != Cyc_Absyn_Int_sz)goto _LL4EE;}}_LL4ED:
# 3059
 goto _LL4EF;_LL4EE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7D5->tag != 6)goto _LL4F0;else{if(_tmp7D5->f1 != Cyc_Absyn_Unsigned)goto _LL4F0;if(_tmp7D5->f2 != Cyc_Absyn_Int_sz)goto _LL4F0;}}_LL4EF:
 return Cyc_Absyn_uint_typ;_LL4F0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f1;if(_tmp7D6->tag != 6)goto _LL4F2;else{if(_tmp7D6->f2 != Cyc_Absyn_Long_sz)goto _LL4F2;}}_LL4F1:
 goto _LL4F3;_LL4F2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7D7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C7.f2;if(_tmp7D7->tag != 6)goto _LL4F4;else{if(_tmp7D7->f2 != Cyc_Absyn_Long_sz)goto _LL4F4;}}_LL4F3:
 return Cyc_Absyn_slong_typ;_LL4F4:;_LL4F5:
 return Cyc_Absyn_sint_typ;_LL4D9:;}
# 3068
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7D9=e->r;void*_tmp7DA=_tmp7D9;_LL4F7: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7DB=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7DA;if(_tmp7DB->tag != 3)goto _LL4F9;else{if(_tmp7DB->f2 != 0)goto _LL4F9;}}_LL4F8:
{const char*_tmp116D;void*_tmp116C;(_tmp116C=0,Cyc_Tcutil_warn(e->loc,((_tmp116D="assignment in test",_tag_dyneither(_tmp116D,sizeof(char),19))),_tag_dyneither(_tmp116C,sizeof(void*),0)));}goto _LL4F6;_LL4F9:;_LL4FA:
 goto _LL4F6;_LL4F6:;}
# 3082 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp116E;struct _tuple0 _tmp7DE=(_tmp116E.f1=c1,((_tmp116E.f2=c2,_tmp116E)));struct _tuple0 _tmp7DF=_tmp7DE;struct Cyc_Absyn_Kind*_tmp7E1;struct Cyc_Absyn_Kind*_tmp7E3;struct Cyc_Core_Opt**_tmp7E5;struct Cyc_Core_Opt**_tmp7E7;struct Cyc_Core_Opt**_tmp7E9;struct Cyc_Absyn_Kind*_tmp7EA;struct Cyc_Absyn_Kind*_tmp7EC;struct Cyc_Absyn_Kind*_tmp7EE;struct Cyc_Core_Opt**_tmp7F0;struct Cyc_Absyn_Kind*_tmp7F1;struct Cyc_Core_Opt**_tmp7F3;struct Cyc_Absyn_Kind*_tmp7F4;struct Cyc_Core_Opt**_tmp7F6;struct Cyc_Absyn_Kind*_tmp7F7;_LL4FC:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7E0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7DF.f1;if(_tmp7E0->tag != 0)goto _LL4FE;else{_tmp7E1=_tmp7E0->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7E2=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7DF.f2;if(_tmp7E2->tag != 0)goto _LL4FE;else{_tmp7E3=_tmp7E2->f1;}};_LL4FD:
 return _tmp7E1 == _tmp7E3;_LL4FE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7E4=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7DF.f2;if(_tmp7E4->tag != 1)goto _LL500;else{_tmp7E5=(struct Cyc_Core_Opt**)& _tmp7E4->f1;}}_LL4FF:
{struct Cyc_Core_Opt*_tmp116F;*_tmp7E5=((_tmp116F=_cycalloc(sizeof(*_tmp116F)),((_tmp116F->v=c1,_tmp116F))));}return 1;_LL500: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7E6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7DF.f1;if(_tmp7E6->tag != 1)goto _LL502;else{_tmp7E7=(struct Cyc_Core_Opt**)& _tmp7E6->f1;}}_LL501:
{struct Cyc_Core_Opt*_tmp1170;*_tmp7E7=((_tmp1170=_cycalloc(sizeof(*_tmp1170)),((_tmp1170->v=c2,_tmp1170))));}return 1;_LL502:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7E8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7DF.f1;if(_tmp7E8->tag != 2)goto _LL504;else{_tmp7E9=(struct Cyc_Core_Opt**)& _tmp7E8->f1;_tmp7EA=_tmp7E8->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7EB=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7DF.f2;if(_tmp7EB->tag != 0)goto _LL504;else{_tmp7EC=_tmp7EB->f1;}};_LL503:
# 3091
 if(Cyc_Tcutil_kind_leq(_tmp7EC,_tmp7EA)){
{struct Cyc_Core_Opt*_tmp1171;*_tmp7E9=((_tmp1171=_cycalloc(sizeof(*_tmp1171)),((_tmp1171->v=c2,_tmp1171))));}return 1;}else{
return 0;}_LL504:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7ED=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7DF.f1;if(_tmp7ED->tag != 0)goto _LL506;else{_tmp7EE=_tmp7ED->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7EF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7DF.f2;if(_tmp7EF->tag != 2)goto _LL506;else{_tmp7F0=(struct Cyc_Core_Opt**)& _tmp7EF->f1;_tmp7F1=_tmp7EF->f2;}};_LL505:
# 3095
 if(Cyc_Tcutil_kind_leq(_tmp7EE,_tmp7F1)){
{struct Cyc_Core_Opt*_tmp1172;*_tmp7F0=((_tmp1172=_cycalloc(sizeof(*_tmp1172)),((_tmp1172->v=c1,_tmp1172))));}return 1;}else{
return 0;}_LL506:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7DF.f1;if(_tmp7F2->tag != 2)goto _LL4FB;else{_tmp7F3=(struct Cyc_Core_Opt**)& _tmp7F2->f1;_tmp7F4=_tmp7F2->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7F5=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7DF.f2;if(_tmp7F5->tag != 2)goto _LL4FB;else{_tmp7F6=(struct Cyc_Core_Opt**)& _tmp7F5->f1;_tmp7F7=_tmp7F5->f2;}};_LL507:
# 3099
 if(Cyc_Tcutil_kind_leq(_tmp7F4,_tmp7F7)){
{struct Cyc_Core_Opt*_tmp1173;*_tmp7F6=((_tmp1173=_cycalloc(sizeof(*_tmp1173)),((_tmp1173->v=c1,_tmp1173))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp7F7,_tmp7F4)){
{struct Cyc_Core_Opt*_tmp1174;*_tmp7F3=((_tmp1174=_cycalloc(sizeof(*_tmp1174)),((_tmp1174->v=c2,_tmp1174))));}return 1;}else{
return 0;}}_LL4FB:;};}
# 3108
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3113
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1178;void*_tmp1177[1];struct Cyc_Int_pa_PrintArg_struct _tmp1176;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1176.tag=1,((_tmp1176.f1=(unsigned long)i,((_tmp1177[0]=& _tmp1176,Cyc_aprintf(((_tmp1178="#%d",_tag_dyneither(_tmp1178,sizeof(char),4))),_tag_dyneither(_tmp1177,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp117B;struct Cyc_Absyn_Tvar*_tmp117A;return(_tmp117A=_cycalloc(sizeof(*_tmp117A)),((_tmp117A->name=((_tmp117B=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp117B[0]=(struct _dyneither_ptr)s,_tmp117B)))),((_tmp117A->identity=- 1,((_tmp117A->kind=k,_tmp117A)))))));}
# 3120
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp804=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp804,sizeof(char),0))== '#';}
# 3125
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp117F;void*_tmp117E[1];struct Cyc_String_pa_PrintArg_struct _tmp117D;(_tmp117D.tag=0,((_tmp117D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp117E[0]=& _tmp117D,Cyc_printf(((_tmp117F="%s",_tag_dyneither(_tmp117F,sizeof(char),3))),_tag_dyneither(_tmp117E,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp1180;struct _dyneither_ptr _tmp808=Cyc_strconcat(((_tmp1180="`",_tag_dyneither(_tmp1180,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp1183;char _tmp1182;struct _dyneither_ptr _tmp1181;(_tmp1181=_dyneither_ptr_plus(_tmp808,sizeof(char),1),((_tmp1182=*((char*)_check_dyneither_subscript(_tmp1181,sizeof(char),0)),((_tmp1183='t',((_get_dyneither_size(_tmp1181,sizeof(char))== 1  && (_tmp1182 == '\000'  && _tmp1183 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp1181.curr)=_tmp1183)))))));}{
struct _dyneither_ptr*_tmp1184;t->name=((_tmp1184=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1184[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp808),_tmp1184))));};};}
# 3134
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3136
struct _tuple10*_tmp1185;return(_tmp1185=_cycalloc(sizeof(*_tmp1185)),((_tmp1185->f1=(*x).f1,((_tmp1185->f2=(*x).f2,((_tmp1185->f3=(*x).f3,_tmp1185)))))));}
# 3139
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3146
struct Cyc_List_List*_tmp80F=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1186;_tmp80F=((_tmp1186=_cycalloc(sizeof(*_tmp1186)),((_tmp1186->hd=(void*)atts->hd,((_tmp1186->tl=_tmp80F,_tmp1186))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp118C;struct Cyc_Absyn_FnInfo _tmp118B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp118A;return(void*)((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A[0]=((_tmp118C.tag=9,((_tmp118C.f1=((_tmp118B.tvars=fd->tvs,((_tmp118B.effect=fd->effect,((_tmp118B.ret_tqual=fd->ret_tqual,((_tmp118B.ret_typ=fd->ret_type,((_tmp118B.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp118B.c_varargs=fd->c_varargs,((_tmp118B.cyc_varargs=fd->cyc_varargs,((_tmp118B.rgn_po=fd->rgn_po,((_tmp118B.attributes=_tmp80F,((_tmp118B.requires_clause=fd->requires_clause,((_tmp118B.requires_relns=fd->requires_relns,((_tmp118B.ensures_clause=fd->ensures_clause,((_tmp118B.ensures_relns=fd->ensures_relns,_tmp118B)))))))))))))))))))))))))),_tmp118C)))),_tmp118A))));};}
# 3159
return(void*)_check_null(fd->cached_typ);}
# 3165
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3167
union Cyc_Relations_RelnOp _tmp814=*rop;union Cyc_Relations_RelnOp _tmp815=_tmp814;struct Cyc_Absyn_Vardecl*_tmp816;struct Cyc_Absyn_Vardecl*_tmp817;_LL509: if((_tmp815.RVar).tag != 2)goto _LL50B;_tmp816=(struct Cyc_Absyn_Vardecl*)(_tmp815.RVar).val;_LL50A: {
# 3169
struct _tuple2 _tmp818=*_tmp816->name;union Cyc_Absyn_Nmspace _tmp81A;struct _dyneither_ptr*_tmp81B;struct _tuple2 _tmp819=_tmp818;_tmp81A=_tmp819.f1;_tmp81B=_tmp819.f2;
if(!((int)((_tmp81A.Loc_n).tag == 4)))goto _LL508;
{const char*_tmp118D;if(Cyc_strcmp((struct _dyneither_ptr)*_tmp81B,((_tmp118D="return_value",_tag_dyneither(_tmp118D,sizeof(char),13))))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL508;}}{
# 3175
unsigned int c=0;
{struct Cyc_List_List*_tmp81D=args;for(0;_tmp81D != 0;(_tmp81D=_tmp81D->tl,c ++)){
struct _tuple10*_tmp81E=(struct _tuple10*)_tmp81D->hd;struct _dyneither_ptr*_tmp820;struct _tuple10*_tmp81F=_tmp81E;_tmp820=_tmp81F->f1;
if(_tmp820 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp81B,(struct _dyneither_ptr)*_tmp820)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3185
goto _LL508;};}_LL50B: if((_tmp815.RNumelts).tag != 3)goto _LL50D;_tmp817=(struct Cyc_Absyn_Vardecl*)(_tmp815.RNumelts).val;_LL50C: {
# 3187
struct _tuple2 _tmp821=*_tmp817->name;union Cyc_Absyn_Nmspace _tmp823;struct _dyneither_ptr*_tmp824;struct _tuple2 _tmp822=_tmp821;_tmp823=_tmp822.f1;_tmp824=_tmp822.f2;
if(!((int)((_tmp823.Loc_n).tag == 4)))goto _LL508;{
unsigned int c=0;
{struct Cyc_List_List*_tmp825=args;for(0;_tmp825 != 0;(_tmp825=_tmp825->tl,c ++)){
struct _tuple10*_tmp826=(struct _tuple10*)_tmp825->hd;struct _dyneither_ptr*_tmp828;struct _tuple10*_tmp827=_tmp826;_tmp828=_tmp827->f1;
if(_tmp828 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp824,(struct _dyneither_ptr)*_tmp828)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3199
goto _LL508;};}_LL50D:;_LL50E:
 goto _LL508;_LL508:;}
# 3204
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3206
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3210
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3212
if(e == 0)return 0;{
struct Cyc_List_List*_tmp829=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp829);
return _tmp829;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3219
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp82B;void*_tmp82C;struct _tuple12*_tmp82A=pr;_tmp82B=_tmp82A->f1;_tmp82C=_tmp82A->f2;
if(_tmp82C == t)return pr;else{
struct _tuple12*_tmp118E;return(_tmp118E=_cycalloc(sizeof(*_tmp118E)),((_tmp118E->f1=_tmp82B,((_tmp118E->f2=t,_tmp118E)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3226
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3228
struct _tuple26*_tmp1191;struct _tuple27*_tmp1190;return(_tmp1190=_region_malloc(rgn,sizeof(*_tmp1190)),((_tmp1190->f1=((_tmp1191=_region_malloc(rgn,sizeof(*_tmp1191)),((_tmp1191->f1=(*y).f1,((_tmp1191->f2=(*y).f2,_tmp1191)))))),((_tmp1190->f2=(*y).f3,_tmp1190)))));}
# 3230
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3232
struct _tuple10 _tmp830=*orig_arg;struct _dyneither_ptr*_tmp832;struct Cyc_Absyn_Tqual _tmp833;void*_tmp834;struct _tuple10 _tmp831=_tmp830;_tmp832=_tmp831.f1;_tmp833=_tmp831.f2;_tmp834=_tmp831.f3;
if(t == _tmp834)return orig_arg;{
struct _tuple10*_tmp1192;return(_tmp1192=_cycalloc(sizeof(*_tmp1192)),((_tmp1192->f1=_tmp832,((_tmp1192->f2=_tmp833,((_tmp1192->f3=t,_tmp1192)))))));};}
# 3236
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3239
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3244
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3246
struct Cyc_Absyn_Exp*_tmp1193;return(_tmp1193=_cycalloc(sizeof(*_tmp1193)),((_tmp1193->topt=old->topt,((_tmp1193->r=r,((_tmp1193->loc=old->loc,((_tmp1193->annot=old->annot,_tmp1193)))))))));}
# 3251
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp837=e->r;void*_tmp838=_tmp837;enum Cyc_Absyn_Primop _tmp83E;struct Cyc_List_List*_tmp83F;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Exp*_tmp843;struct Cyc_Absyn_Exp*_tmp845;struct Cyc_Absyn_Exp*_tmp846;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Exp*_tmp849;struct Cyc_Absyn_Exp*_tmp84B;struct Cyc_Absyn_Exp*_tmp84C;void*_tmp84E;struct Cyc_Absyn_Exp*_tmp84F;int _tmp850;enum Cyc_Absyn_Coercion _tmp851;void*_tmp853;struct Cyc_Absyn_Exp*_tmp855;void*_tmp857;struct Cyc_List_List*_tmp858;void*_tmp85A;_LL510: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp839=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp839->tag != 0)goto _LL512;}_LL511:
 goto _LL513;_LL512: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp83A=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp83A->tag != 31)goto _LL514;}_LL513:
 goto _LL515;_LL514: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp83B=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp83B->tag != 32)goto _LL516;}_LL515:
 goto _LL517;_LL516: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp83C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp83C->tag != 1)goto _LL518;}_LL517:
 return e;_LL518: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp83D=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp83D->tag != 2)goto _LL51A;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;}}_LL519:
# 3259
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83F)== 1){
struct Cyc_Absyn_Exp*_tmp85B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp83F))->hd;
struct Cyc_Absyn_Exp*_tmp85C=Cyc_Tcutil_rsubsexp(r,inst,_tmp85B);
if(_tmp85C == _tmp85B)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1199;struct Cyc_Absyn_Exp*_tmp1198[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1197;return Cyc_Tcutil_copye(e,(void*)((_tmp1197=_cycalloc(sizeof(*_tmp1197)),((_tmp1197[0]=((_tmp1199.tag=2,((_tmp1199.f1=_tmp83E,((_tmp1199.f2=((_tmp1198[0]=_tmp85C,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1198,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1199)))))),_tmp1197)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp83F)== 2){
struct Cyc_Absyn_Exp*_tmp860=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp83F))->hd;
struct Cyc_Absyn_Exp*_tmp861=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp83F->tl))->hd;
struct Cyc_Absyn_Exp*_tmp862=Cyc_Tcutil_rsubsexp(r,inst,_tmp860);
struct Cyc_Absyn_Exp*_tmp863=Cyc_Tcutil_rsubsexp(r,inst,_tmp861);
if(_tmp862 == _tmp860  && _tmp863 == _tmp861)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp119F;struct Cyc_Absyn_Exp*_tmp119E[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp119D;return Cyc_Tcutil_copye(e,(void*)((_tmp119D=_cycalloc(sizeof(*_tmp119D)),((_tmp119D[0]=((_tmp119F.tag=2,((_tmp119F.f1=_tmp83E,((_tmp119F.f2=((_tmp119E[1]=_tmp863,((_tmp119E[0]=_tmp862,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp119E,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp119F)))))),_tmp119D)))));};}else{
const char*_tmp11A2;void*_tmp11A1;return(_tmp11A1=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11A2="primop does not have 1 or 2 args!",_tag_dyneither(_tmp11A2,sizeof(char),34))),_tag_dyneither(_tmp11A1,sizeof(void*),0)));}}_LL51A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp840=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp840->tag != 5)goto _LL51C;else{_tmp841=_tmp840->f1;_tmp842=_tmp840->f2;_tmp843=_tmp840->f3;}}_LL51B: {
# 3273
struct Cyc_Absyn_Exp*_tmp869=Cyc_Tcutil_rsubsexp(r,inst,_tmp841);
struct Cyc_Absyn_Exp*_tmp86A=Cyc_Tcutil_rsubsexp(r,inst,_tmp842);
struct Cyc_Absyn_Exp*_tmp86B=Cyc_Tcutil_rsubsexp(r,inst,_tmp843);
if((_tmp869 == _tmp841  && _tmp86A == _tmp842) && _tmp86B == _tmp843)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp11A5;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11A4;return Cyc_Tcutil_copye(e,(void*)((_tmp11A4=_cycalloc(sizeof(*_tmp11A4)),((_tmp11A4[0]=((_tmp11A5.tag=5,((_tmp11A5.f1=_tmp869,((_tmp11A5.f2=_tmp86A,((_tmp11A5.f3=_tmp86B,_tmp11A5)))))))),_tmp11A4)))));};}_LL51C: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp844=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp844->tag != 6)goto _LL51E;else{_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;}}_LL51D: {
# 3279
struct Cyc_Absyn_Exp*_tmp86E=Cyc_Tcutil_rsubsexp(r,inst,_tmp845);
struct Cyc_Absyn_Exp*_tmp86F=Cyc_Tcutil_rsubsexp(r,inst,_tmp846);
if(_tmp86E == _tmp845  && _tmp86F == _tmp846)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp11A8;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp11A7;return Cyc_Tcutil_copye(e,(void*)((_tmp11A7=_cycalloc(sizeof(*_tmp11A7)),((_tmp11A7[0]=((_tmp11A8.tag=6,((_tmp11A8.f1=_tmp86E,((_tmp11A8.f2=_tmp86F,_tmp11A8)))))),_tmp11A7)))));};}_LL51E: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp847=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp847->tag != 7)goto _LL520;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;}}_LL51F: {
# 3284
struct Cyc_Absyn_Exp*_tmp872=Cyc_Tcutil_rsubsexp(r,inst,_tmp848);
struct Cyc_Absyn_Exp*_tmp873=Cyc_Tcutil_rsubsexp(r,inst,_tmp849);
if(_tmp872 == _tmp848  && _tmp873 == _tmp849)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp11AB;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp11AA;return Cyc_Tcutil_copye(e,(void*)((_tmp11AA=_cycalloc(sizeof(*_tmp11AA)),((_tmp11AA[0]=((_tmp11AB.tag=7,((_tmp11AB.f1=_tmp872,((_tmp11AB.f2=_tmp873,_tmp11AB)))))),_tmp11AA)))));};}_LL520: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp84A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp84A->tag != 8)goto _LL522;else{_tmp84B=_tmp84A->f1;_tmp84C=_tmp84A->f2;}}_LL521: {
# 3289
struct Cyc_Absyn_Exp*_tmp876=Cyc_Tcutil_rsubsexp(r,inst,_tmp84B);
struct Cyc_Absyn_Exp*_tmp877=Cyc_Tcutil_rsubsexp(r,inst,_tmp84C);
if(_tmp876 == _tmp84B  && _tmp877 == _tmp84C)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp11AE;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp11AD;return Cyc_Tcutil_copye(e,(void*)((_tmp11AD=_cycalloc(sizeof(*_tmp11AD)),((_tmp11AD[0]=((_tmp11AE.tag=8,((_tmp11AE.f1=_tmp876,((_tmp11AE.f2=_tmp877,_tmp11AE)))))),_tmp11AD)))));};}_LL522: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp84D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp84D->tag != 13)goto _LL524;else{_tmp84E=(void*)_tmp84D->f1;_tmp84F=_tmp84D->f2;_tmp850=_tmp84D->f3;_tmp851=_tmp84D->f4;}}_LL523: {
# 3294
struct Cyc_Absyn_Exp*_tmp87A=Cyc_Tcutil_rsubsexp(r,inst,_tmp84F);
void*_tmp87B=Cyc_Tcutil_rsubstitute(r,inst,_tmp84E);
if(_tmp87A == _tmp84F  && _tmp87B == _tmp84E)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp11B1;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp11B0;return Cyc_Tcutil_copye(e,(void*)((_tmp11B0=_cycalloc(sizeof(*_tmp11B0)),((_tmp11B0[0]=((_tmp11B1.tag=13,((_tmp11B1.f1=_tmp87B,((_tmp11B1.f2=_tmp87A,((_tmp11B1.f3=_tmp850,((_tmp11B1.f4=_tmp851,_tmp11B1)))))))))),_tmp11B0)))));};}_LL524: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp852=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp852->tag != 16)goto _LL526;else{_tmp853=(void*)_tmp852->f1;}}_LL525: {
# 3299
void*_tmp87E=Cyc_Tcutil_rsubstitute(r,inst,_tmp853);
if(_tmp87E == _tmp853)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp11B4;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp11B3;return Cyc_Tcutil_copye(e,(void*)((_tmp11B3=_cycalloc(sizeof(*_tmp11B3)),((_tmp11B3[0]=((_tmp11B4.tag=16,((_tmp11B4.f1=_tmp87E,_tmp11B4)))),_tmp11B3)))));};}_LL526: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp854=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp854->tag != 17)goto _LL528;else{_tmp855=_tmp854->f1;}}_LL527: {
# 3303
struct Cyc_Absyn_Exp*_tmp881=Cyc_Tcutil_rsubsexp(r,inst,_tmp855);
if(_tmp881 == _tmp855)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp11B7;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp11B6;return Cyc_Tcutil_copye(e,(void*)((_tmp11B6=_cycalloc(sizeof(*_tmp11B6)),((_tmp11B6[0]=((_tmp11B7.tag=17,((_tmp11B7.f1=_tmp881,_tmp11B7)))),_tmp11B6)))));};}_LL528: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp856=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp856->tag != 18)goto _LL52A;else{_tmp857=(void*)_tmp856->f1;_tmp858=_tmp856->f2;}}_LL529: {
# 3307
void*_tmp884=Cyc_Tcutil_rsubstitute(r,inst,_tmp857);
if(_tmp884 == _tmp857)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp11BA;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp11B9;return Cyc_Tcutil_copye(e,(void*)((_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9[0]=((_tmp11BA.tag=18,((_tmp11BA.f1=_tmp884,((_tmp11BA.f2=_tmp858,_tmp11BA)))))),_tmp11B9)))));};}_LL52A: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp859=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp838;if(_tmp859->tag != 38)goto _LL52C;else{_tmp85A=(void*)_tmp859->f1;}}_LL52B: {
# 3311
void*_tmp887=Cyc_Tcutil_rsubstitute(r,inst,_tmp85A);
if(_tmp887 == _tmp85A)return e;{
# 3314
void*_tmp888=Cyc_Tcutil_compress(_tmp887);void*_tmp889=_tmp888;struct Cyc_Absyn_Exp*_tmp88B;_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp88A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp889;if(_tmp88A->tag != 18)goto _LL531;else{_tmp88B=_tmp88A->f1;}}_LL530:
 return _tmp88B;_LL531:;_LL532: {
# 3317
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp11BD;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp11BC;return Cyc_Tcutil_copye(e,(void*)((_tmp11BC=_cycalloc(sizeof(*_tmp11BC)),((_tmp11BC[0]=((_tmp11BD.tag=38,((_tmp11BD.f1=_tmp887,_tmp11BD)))),_tmp11BC)))));}_LL52E:;};}_LL52C:;_LL52D: {
# 3320
const char*_tmp11C0;void*_tmp11BF;return(_tmp11BF=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C0="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp11C0,sizeof(char),46))),_tag_dyneither(_tmp11BF,sizeof(void*),0)));}_LL50F:;}
# 3324
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3327
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3331
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3334
void*_tmp890=f->type;
struct Cyc_Absyn_Exp*_tmp891=f->requires_clause;
void*_tmp892=Cyc_Tcutil_rsubstitute(r,inst,_tmp890);
struct Cyc_Absyn_Exp*_tmp893=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp891);
if(_tmp890 == _tmp892  && _tmp891 == _tmp893)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp11C1;return(_tmp11C1=_cycalloc(sizeof(*_tmp11C1)),((_tmp11C1->name=f->name,((_tmp11C1->tq=f->tq,((_tmp11C1->type=_tmp892,((_tmp11C1->width=f->width,((_tmp11C1->attributes=f->attributes,((_tmp11C1->requires_clause=_tmp893,_tmp11C1)))))))))))));}}
# 3344
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3347
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp895=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp896=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp897=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp895);
struct Cyc_List_List*_tmp898=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp896);
if(_tmp897 == _tmp895  && _tmp898 == _tmp896)return fs;{
# 3354
struct Cyc_List_List*_tmp11C2;return(_tmp11C2=_cycalloc(sizeof(*_tmp11C2)),((_tmp11C2->hd=_tmp897,((_tmp11C2->tl=_tmp898,_tmp11C2)))));};};}
# 3357
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3360
struct _tuple1 _tmp89A=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp89C;struct Cyc_List_List*_tmp89D;struct _tuple1 _tmp89B=_tmp89A;_tmp89C=_tmp89B.f1;_tmp89D=_tmp89B.f2;{
struct Cyc_List_List*_tmp89E=Cyc_Tcutil_substs(rgn,inst,_tmp89C);
struct Cyc_List_List*_tmp89F=Cyc_Tcutil_substs(rgn,inst,_tmp89D);
if(_tmp89E == _tmp89C  && _tmp89F == _tmp89D)
return rgn_po;else{
# 3366
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp89E,_tmp89F);}};}
# 3369
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3372
void*_tmp8A0=Cyc_Tcutil_compress(t);void*_tmp8A1=_tmp8A0;struct Cyc_Absyn_Tvar*_tmp8A3;union Cyc_Absyn_AggrInfoU _tmp8A5;struct Cyc_List_List*_tmp8A6;union Cyc_Absyn_DatatypeInfoU _tmp8A8;struct Cyc_List_List*_tmp8A9;union Cyc_Absyn_DatatypeFieldInfoU _tmp8AB;struct Cyc_List_List*_tmp8AC;struct _tuple2*_tmp8AE;struct Cyc_List_List*_tmp8AF;struct Cyc_Absyn_Typedefdecl*_tmp8B0;void*_tmp8B1;void*_tmp8B3;struct Cyc_Absyn_Tqual _tmp8B4;struct Cyc_Absyn_Exp*_tmp8B5;union Cyc_Absyn_Constraint*_tmp8B6;unsigned int _tmp8B7;void*_tmp8B9;struct Cyc_Absyn_Tqual _tmp8BA;void*_tmp8BB;union Cyc_Absyn_Constraint*_tmp8BC;union Cyc_Absyn_Constraint*_tmp8BD;union Cyc_Absyn_Constraint*_tmp8BE;struct Cyc_List_List*_tmp8C0;void*_tmp8C1;struct Cyc_Absyn_Tqual _tmp8C2;void*_tmp8C3;struct Cyc_List_List*_tmp8C4;int _tmp8C5;struct Cyc_Absyn_VarargInfo*_tmp8C6;struct Cyc_List_List*_tmp8C7;struct Cyc_List_List*_tmp8C8;struct Cyc_Absyn_Exp*_tmp8C9;struct Cyc_Absyn_Exp*_tmp8CA;struct Cyc_List_List*_tmp8CC;enum Cyc_Absyn_AggrKind _tmp8CE;struct Cyc_List_List*_tmp8CF;void*_tmp8D1;void*_tmp8D3;void*_tmp8D5;void*_tmp8D6;void*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8DA;void*_tmp8E4;void*_tmp8E6;struct Cyc_List_List*_tmp8E8;_LL534: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp8A2=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8A1;if(_tmp8A2->tag != 2)goto _LL536;else{_tmp8A3=_tmp8A2->f1;}}_LL535: {
# 3375
struct _handler_cons _tmp8EA;_push_handler(& _tmp8EA);{int _tmp8EC=0;if(setjmp(_tmp8EA.handler))_tmp8EC=1;if(!_tmp8EC){{void*_tmp8ED=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp8A3);_npop_handler(0);return _tmp8ED;};_pop_handler();}else{void*_tmp8EB=(void*)_exn_thrown;void*_tmp8EF=_tmp8EB;void*_tmp8F1;_LL56B: {struct Cyc_Core_Not_found_exn_struct*_tmp8F0=(struct Cyc_Core_Not_found_exn_struct*)_tmp8EF;if(_tmp8F0->tag != Cyc_Core_Not_found)goto _LL56D;}_LL56C:
 return t;_LL56D: _tmp8F1=_tmp8EF;_LL56E:(void)_throw(_tmp8F1);_LL56A:;}};}_LL536: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8A4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp8A1;if(_tmp8A4->tag != 11)goto _LL538;else{_tmp8A5=(_tmp8A4->f1).aggr_info;_tmp8A6=(_tmp8A4->f1).targs;}}_LL537: {
# 3378
struct Cyc_List_List*_tmp8F2=Cyc_Tcutil_substs(rgn,inst,_tmp8A6);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp11C8;struct Cyc_Absyn_AggrInfo _tmp11C7;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp11C6;return _tmp8F2 == _tmp8A6?t:(void*)((_tmp11C6=_cycalloc(sizeof(*_tmp11C6)),((_tmp11C6[0]=((_tmp11C8.tag=11,((_tmp11C8.f1=((_tmp11C7.aggr_info=_tmp8A5,((_tmp11C7.targs=_tmp8F2,_tmp11C7)))),_tmp11C8)))),_tmp11C6))));}_LL538: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp8A7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp8A1;if(_tmp8A7->tag != 3)goto _LL53A;else{_tmp8A8=(_tmp8A7->f1).datatype_info;_tmp8A9=(_tmp8A7->f1).targs;}}_LL539: {
# 3381
struct Cyc_List_List*_tmp8F6=Cyc_Tcutil_substs(rgn,inst,_tmp8A9);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp11CE;struct Cyc_Absyn_DatatypeInfo _tmp11CD;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp11CC;return _tmp8F6 == _tmp8A9?t:(void*)(
(_tmp11CC=_cycalloc(sizeof(*_tmp11CC)),((_tmp11CC[0]=((_tmp11CE.tag=3,((_tmp11CE.f1=((_tmp11CD.datatype_info=_tmp8A8,((_tmp11CD.targs=_tmp8F6,_tmp11CD)))),_tmp11CE)))),_tmp11CC))));}_LL53A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp8AA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp8A1;if(_tmp8AA->tag != 4)goto _LL53C;else{_tmp8AB=(_tmp8AA->f1).field_info;_tmp8AC=(_tmp8AA->f1).targs;}}_LL53B: {
# 3385
struct Cyc_List_List*_tmp8FA=Cyc_Tcutil_substs(rgn,inst,_tmp8AC);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp11D4;struct Cyc_Absyn_DatatypeFieldInfo _tmp11D3;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp11D2;return _tmp8FA == _tmp8AC?t:(void*)(
(_tmp11D2=_cycalloc(sizeof(*_tmp11D2)),((_tmp11D2[0]=((_tmp11D4.tag=4,((_tmp11D4.f1=((_tmp11D3.field_info=_tmp8AB,((_tmp11D3.targs=_tmp8FA,_tmp11D3)))),_tmp11D4)))),_tmp11D2))));}_LL53C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8AD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp8A1;if(_tmp8AD->tag != 17)goto _LL53E;else{_tmp8AE=_tmp8AD->f1;_tmp8AF=_tmp8AD->f2;_tmp8B0=_tmp8AD->f3;_tmp8B1=(void*)_tmp8AD->f4;}}_LL53D: {
# 3389
struct Cyc_List_List*_tmp8FE=Cyc_Tcutil_substs(rgn,inst,_tmp8AF);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp11D7;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp11D6;return _tmp8FE == _tmp8AF?t:(void*)((_tmp11D6=_cycalloc(sizeof(*_tmp11D6)),((_tmp11D6[0]=((_tmp11D7.tag=17,((_tmp11D7.f1=_tmp8AE,((_tmp11D7.f2=_tmp8FE,((_tmp11D7.f3=_tmp8B0,((_tmp11D7.f4=_tmp8B1,_tmp11D7)))))))))),_tmp11D6))));}_LL53E: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8A1;if(_tmp8B2->tag != 8)goto _LL540;else{_tmp8B3=(_tmp8B2->f1).elt_type;_tmp8B4=(_tmp8B2->f1).tq;_tmp8B5=(_tmp8B2->f1).num_elts;_tmp8B6=(_tmp8B2->f1).zero_term;_tmp8B7=(_tmp8B2->f1).zt_loc;}}_LL53F: {
# 3392
void*_tmp901=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8B3);
struct Cyc_Absyn_Exp*_tmp902=_tmp8B5 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8B5);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp11DD;struct Cyc_Absyn_ArrayInfo _tmp11DC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp11DB;return _tmp901 == _tmp8B3  && _tmp902 == _tmp8B5?t:(void*)(
(_tmp11DB=_cycalloc(sizeof(*_tmp11DB)),((_tmp11DB[0]=((_tmp11DD.tag=8,((_tmp11DD.f1=((_tmp11DC.elt_type=_tmp901,((_tmp11DC.tq=_tmp8B4,((_tmp11DC.num_elts=_tmp902,((_tmp11DC.zero_term=_tmp8B6,((_tmp11DC.zt_loc=_tmp8B7,_tmp11DC)))))))))),_tmp11DD)))),_tmp11DB))));}_LL540: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8B8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A1;if(_tmp8B8->tag != 5)goto _LL542;else{_tmp8B9=(_tmp8B8->f1).elt_typ;_tmp8BA=(_tmp8B8->f1).elt_tq;_tmp8BB=((_tmp8B8->f1).ptr_atts).rgn;_tmp8BC=((_tmp8B8->f1).ptr_atts).nullable;_tmp8BD=((_tmp8B8->f1).ptr_atts).bounds;_tmp8BE=((_tmp8B8->f1).ptr_atts).zero_term;}}_LL541: {
# 3397
void*_tmp906=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8B9);
void*_tmp907=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8BB);
union Cyc_Absyn_Constraint*_tmp908=_tmp8BD;
{void*_tmp909=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp8BD);void*_tmp90A=_tmp909;struct Cyc_Absyn_Exp*_tmp90C;_LL570: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp90B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp90A;if(_tmp90B->tag != 1)goto _LL572;else{_tmp90C=_tmp90B->f1;}}_LL571: {
# 3402
struct Cyc_Absyn_Exp*_tmp90D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp90C);
if(_tmp90D != _tmp90C){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp11E0;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp11DF;_tmp908=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp11DF=_cycalloc(sizeof(*_tmp11DF)),((_tmp11DF[0]=((_tmp11E0.tag=1,((_tmp11E0.f1=_tmp90D,_tmp11E0)))),_tmp11DF)))));}
goto _LL56F;}_LL572:;_LL573:
 goto _LL56F;_LL56F:;}
# 3408
if((_tmp906 == _tmp8B9  && _tmp907 == _tmp8BB) && _tmp908 == _tmp8BD)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp11E6;struct Cyc_Absyn_PtrInfo _tmp11E5;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp11E4;return(void*)((_tmp11E4=_cycalloc(sizeof(*_tmp11E4)),((_tmp11E4[0]=((_tmp11E6.tag=5,((_tmp11E6.f1=((_tmp11E5.elt_typ=_tmp906,((_tmp11E5.elt_tq=_tmp8BA,((_tmp11E5.ptr_atts=(((_tmp11E5.ptr_atts).rgn=_tmp907,(((_tmp11E5.ptr_atts).nullable=_tmp8BC,(((_tmp11E5.ptr_atts).bounds=_tmp908,(((_tmp11E5.ptr_atts).zero_term=_tmp8BE,(((_tmp11E5.ptr_atts).ptrloc=0,_tmp11E5.ptr_atts)))))))))),_tmp11E5)))))),_tmp11E6)))),_tmp11E4))));};}_LL542: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp8BF=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp8A1;if(_tmp8BF->tag != 9)goto _LL544;else{_tmp8C0=(_tmp8BF->f1).tvars;_tmp8C1=(_tmp8BF->f1).effect;_tmp8C2=(_tmp8BF->f1).ret_tqual;_tmp8C3=(_tmp8BF->f1).ret_typ;_tmp8C4=(_tmp8BF->f1).args;_tmp8C5=(_tmp8BF->f1).c_varargs;_tmp8C6=(_tmp8BF->f1).cyc_varargs;_tmp8C7=(_tmp8BF->f1).rgn_po;_tmp8C8=(_tmp8BF->f1).attributes;_tmp8C9=(_tmp8BF->f1).requires_clause;_tmp8CA=(_tmp8BF->f1).ensures_clause;}}_LL543:
# 3414
{struct Cyc_List_List*_tmp913=_tmp8C0;for(0;_tmp913 != 0;_tmp913=_tmp913->tl){
struct _tuple16*_tmp11F0;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11EF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp11EE;struct Cyc_List_List*_tmp11ED;inst=((_tmp11ED=_region_malloc(rgn,sizeof(*_tmp11ED)),((_tmp11ED->hd=((_tmp11F0=_region_malloc(rgn,sizeof(*_tmp11F0)),((_tmp11F0->f1=(struct Cyc_Absyn_Tvar*)_tmp913->hd,((_tmp11F0->f2=(void*)((_tmp11EE=_cycalloc(sizeof(*_tmp11EE)),((_tmp11EE[0]=((_tmp11EF.tag=2,((_tmp11EF.f1=(struct Cyc_Absyn_Tvar*)_tmp913->hd,_tmp11EF)))),_tmp11EE)))),_tmp11F0)))))),((_tmp11ED->tl=inst,_tmp11ED))))));}}{
struct _tuple1 _tmp918=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp8C4));
# 3416
struct Cyc_List_List*_tmp91A;struct Cyc_List_List*_tmp91B;struct _tuple1 _tmp919=_tmp918;_tmp91A=_tmp919.f1;_tmp91B=_tmp919.f2;{
# 3418
struct Cyc_List_List*_tmp91C=_tmp8C4;
struct Cyc_List_List*_tmp91D=Cyc_Tcutil_substs(rgn,inst,_tmp91B);
if(_tmp91D != _tmp91B)
_tmp91C=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp8C4,_tmp91D);{
void*eff2;
if(_tmp8C1 == 0)
eff2=0;else{
# 3426
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C1);
if(new_eff == _tmp8C1)
eff2=_tmp8C1;else{
# 3430
eff2=new_eff;}}{
# 3432
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp8C6 == 0)
cyc_varargs2=0;else{
# 3436
struct Cyc_Absyn_VarargInfo _tmp91E=*_tmp8C6;struct _dyneither_ptr*_tmp920;struct Cyc_Absyn_Tqual _tmp921;void*_tmp922;int _tmp923;struct Cyc_Absyn_VarargInfo _tmp91F=_tmp91E;_tmp920=_tmp91F.name;_tmp921=_tmp91F.tq;_tmp922=_tmp91F.type;_tmp923=_tmp91F.inject;{
void*_tmp924=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp922);
if(_tmp924 == _tmp922)cyc_varargs2=_tmp8C6;else{
# 3440
struct Cyc_Absyn_VarargInfo*_tmp11F1;cyc_varargs2=((_tmp11F1=_cycalloc(sizeof(*_tmp11F1)),((_tmp11F1->name=_tmp920,((_tmp11F1->tq=_tmp921,((_tmp11F1->type=_tmp924,((_tmp11F1->inject=_tmp923,_tmp11F1))))))))));}};}{
# 3442
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp8C7);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8C9);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp8CA);
struct Cyc_List_List*_tmp926=Cyc_Tcutil_extract_relns(_tmp91C,req2);
struct Cyc_List_List*_tmp927=Cyc_Tcutil_extract_relns(_tmp91C,ens2);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp11F7;struct Cyc_Absyn_FnInfo _tmp11F6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp11F5;return(void*)(
(_tmp11F5=_cycalloc(sizeof(*_tmp11F5)),((_tmp11F5[0]=((_tmp11F7.tag=9,((_tmp11F7.f1=((_tmp11F6.tvars=_tmp8C0,((_tmp11F6.effect=eff2,((_tmp11F6.ret_tqual=_tmp8C2,((_tmp11F6.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8C3),((_tmp11F6.args=_tmp91C,((_tmp11F6.c_varargs=_tmp8C5,((_tmp11F6.cyc_varargs=cyc_varargs2,((_tmp11F6.rgn_po=rgn_po2,((_tmp11F6.attributes=_tmp8C8,((_tmp11F6.requires_clause=req2,((_tmp11F6.requires_relns=_tmp926,((_tmp11F6.ensures_clause=ens2,((_tmp11F6.ensures_relns=_tmp927,_tmp11F6)))))))))))))))))))))))))),_tmp11F7)))),_tmp11F5))));};};};};};_LL544: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8CB=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8A1;if(_tmp8CB->tag != 10)goto _LL546;else{_tmp8CC=_tmp8CB->f1;}}_LL545: {
# 3452
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp92B=_tmp8CC;for(0;_tmp92B != 0;_tmp92B=_tmp92B->tl){
void*_tmp92C=(*((struct _tuple12*)_tmp92B->hd)).f2;
void*_tmp92D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp92C);
if(_tmp92C != _tmp92D)
change=1;{
# 3460
struct Cyc_List_List*_tmp11F8;ts2=((_tmp11F8=_region_malloc(rgn,sizeof(*_tmp11F8)),((_tmp11F8->hd=_tmp92D,((_tmp11F8->tl=ts2,_tmp11F8))))));};}}
# 3462
if(!change)
return t;{
struct Cyc_List_List*_tmp92F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8CC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp11FB;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp11FA;return(void*)((_tmp11FA=_cycalloc(sizeof(*_tmp11FA)),((_tmp11FA[0]=((_tmp11FB.tag=10,((_tmp11FB.f1=_tmp92F,_tmp11FB)))),_tmp11FA))));};}_LL546: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp8CD=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8A1;if(_tmp8CD->tag != 12)goto _LL548;else{_tmp8CE=_tmp8CD->f1;_tmp8CF=_tmp8CD->f2;}}_LL547: {
# 3467
struct Cyc_List_List*_tmp932=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp8CF);
if(_tmp8CF == _tmp932)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp11FE;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp11FD;return(void*)((_tmp11FD=_cycalloc(sizeof(*_tmp11FD)),((_tmp11FD[0]=((_tmp11FE.tag=12,((_tmp11FE.f1=_tmp8CE,((_tmp11FE.f2=_tmp932,_tmp11FE)))))),_tmp11FD))));};}_LL548: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp8D0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp8A1;if(_tmp8D0->tag != 1)goto _LL54A;else{_tmp8D1=(void*)_tmp8D0->f2;}}_LL549:
# 3471
 if(_tmp8D1 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D1);else{
return t;}_LL54A: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp8D2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp8A1;if(_tmp8D2->tag != 15)goto _LL54C;else{_tmp8D3=(void*)_tmp8D2->f1;}}_LL54B: {
# 3474
void*_tmp935=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D3);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1201;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1200;return _tmp935 == _tmp8D3?t:(void*)((_tmp1200=_cycalloc(sizeof(*_tmp1200)),((_tmp1200[0]=((_tmp1201.tag=15,((_tmp1201.f1=_tmp935,_tmp1201)))),_tmp1200))));}_LL54C: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp8D4=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp8A1;if(_tmp8D4->tag != 16)goto _LL54E;else{_tmp8D5=(void*)_tmp8D4->f1;_tmp8D6=(void*)_tmp8D4->f2;}}_LL54D: {
# 3477
void*_tmp938=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D5);
void*_tmp939=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D6);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp1204;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1203;return _tmp938 == _tmp8D5  && _tmp939 == _tmp8D6?t:(void*)((_tmp1203=_cycalloc(sizeof(*_tmp1203)),((_tmp1203[0]=((_tmp1204.tag=16,((_tmp1204.f1=_tmp938,((_tmp1204.f2=_tmp939,_tmp1204)))))),_tmp1203))));}_LL54E: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp8D7=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp8A1;if(_tmp8D7->tag != 19)goto _LL550;else{_tmp8D8=(void*)_tmp8D7->f1;}}_LL54F: {
# 3481
void*_tmp93C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8D8);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp1207;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1206;return _tmp93C == _tmp8D8?t:(void*)((_tmp1206=_cycalloc(sizeof(*_tmp1206)),((_tmp1206[0]=((_tmp1207.tag=19,((_tmp1207.f1=_tmp93C,_tmp1207)))),_tmp1206))));}_LL550: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp8D9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp8A1;if(_tmp8D9->tag != 18)goto _LL552;else{_tmp8DA=_tmp8D9->f1;}}_LL551: {
# 3484
struct Cyc_Absyn_Exp*_tmp93F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8DA);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp120A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1209;return _tmp93F == _tmp8DA?t:(void*)((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((_tmp1209[0]=((_tmp120A.tag=18,((_tmp120A.f1=_tmp93F,_tmp120A)))),_tmp1209))));}_LL552: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp8DB=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp8A1;if(_tmp8DB->tag != 13)goto _LL554;}_LL553:
 goto _LL555;_LL554: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8DC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp8A1;if(_tmp8DC->tag != 14)goto _LL556;}_LL555:
 goto _LL557;_LL556: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp8DD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp8A1;if(_tmp8DD->tag != 0)goto _LL558;}_LL557:
 goto _LL559;_LL558: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp8A1;if(_tmp8DE->tag != 6)goto _LL55A;}_LL559:
 goto _LL55B;_LL55A: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8DF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp8A1;if(_tmp8DF->tag != 7)goto _LL55C;}_LL55B:
 goto _LL55D;_LL55C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8E0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp8A1;if(_tmp8E0->tag != 22)goto _LL55E;}_LL55D:
 goto _LL55F;_LL55E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8E1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp8A1;if(_tmp8E1->tag != 21)goto _LL560;}_LL55F:
 goto _LL561;_LL560: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8E2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp8A1;if(_tmp8E2->tag != 20)goto _LL562;}_LL561:
 return t;_LL562: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8E3=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp8A1;if(_tmp8E3->tag != 25)goto _LL564;else{_tmp8E4=(void*)_tmp8E3->f1;}}_LL563: {
# 3495
void*_tmp942=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E4);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp120D;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp120C;return _tmp942 == _tmp8E4?t:(void*)((_tmp120C=_cycalloc(sizeof(*_tmp120C)),((_tmp120C[0]=((_tmp120D.tag=25,((_tmp120D.f1=_tmp942,_tmp120D)))),_tmp120C))));}_LL564: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8E5=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp8A1;if(_tmp8E5->tag != 23)goto _LL566;else{_tmp8E6=(void*)_tmp8E5->f1;}}_LL565: {
# 3498
void*_tmp945=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E6);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1210;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp120F;return _tmp945 == _tmp8E6?t:(void*)((_tmp120F=_cycalloc(sizeof(*_tmp120F)),((_tmp120F[0]=((_tmp1210.tag=23,((_tmp1210.f1=_tmp945,_tmp1210)))),_tmp120F))));}_LL566: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8E7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp8A1;if(_tmp8E7->tag != 24)goto _LL568;else{_tmp8E8=_tmp8E7->f1;}}_LL567: {
# 3501
struct Cyc_List_List*_tmp948=Cyc_Tcutil_substs(rgn,inst,_tmp8E8);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1213;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1212;return _tmp948 == _tmp8E8?t:(void*)((_tmp1212=_cycalloc(sizeof(*_tmp1212)),((_tmp1212[0]=((_tmp1213.tag=24,((_tmp1213.f1=_tmp948,_tmp1213)))),_tmp1212))));}_LL568: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp8E9=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp8A1;if(_tmp8E9->tag != 26)goto _LL533;}_LL569: {
const char*_tmp1216;void*_tmp1215;(_tmp1215=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1216="found typedecltype in rsubs",_tag_dyneither(_tmp1216,sizeof(char),28))),_tag_dyneither(_tmp1215,sizeof(void*),0)));}_LL533:;}
# 3507
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3510
if(ts == 0)
return 0;{
void*_tmp94D=(void*)ts->hd;
struct Cyc_List_List*_tmp94E=ts->tl;
void*_tmp94F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp94D);
struct Cyc_List_List*_tmp950=Cyc_Tcutil_substs(rgn,inst,_tmp94E);
if(_tmp94D == _tmp94F  && _tmp94E == _tmp950)
return ts;{
struct Cyc_List_List*_tmp1217;return(_tmp1217=_cycalloc(sizeof(*_tmp1217)),((_tmp1217->hd=_tmp94F,((_tmp1217->tl=_tmp950,_tmp1217)))));};};}
# 3521
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3528
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp952=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp121A;struct _tuple16*_tmp1219;return(_tmp1219=_cycalloc(sizeof(*_tmp1219)),((_tmp1219->f1=tv,((_tmp1219->f2=Cyc_Absyn_new_evar(_tmp952,((_tmp121A=_cycalloc(sizeof(*_tmp121A)),((_tmp121A->v=s,_tmp121A))))),_tmp1219)))));}
# 3533
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3535
struct Cyc_List_List*_tmp956;struct _RegionHandle*_tmp957;struct _tuple17*_tmp955=env;_tmp956=_tmp955->f1;_tmp957=_tmp955->f2;{
struct Cyc_Core_Opt*_tmp958=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp121D;struct _tuple16*_tmp121C;return(_tmp121C=_region_malloc(_tmp957,sizeof(*_tmp121C)),((_tmp121C->f1=tv,((_tmp121C->f2=Cyc_Absyn_new_evar(_tmp958,((_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D->v=_tmp956,_tmp121D))))),_tmp121C)))));};}
# 3545
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3549
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp1223;void*_tmp1222[3];struct Cyc_String_pa_PrintArg_struct _tmp1221;struct Cyc_String_pa_PrintArg_struct _tmp1220;struct Cyc_String_pa_PrintArg_struct _tmp121F;(_tmp121F.tag=0,((_tmp121F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp1220.tag=0,((_tmp1220.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp1221.tag=0,((_tmp1221.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp1222[0]=& _tmp1221,((_tmp1222[1]=& _tmp1220,((_tmp1222[2]=& _tmp121F,Cyc_Tcutil_terr(loc,((_tmp1223="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp1223,sizeof(char),59))),_tag_dyneither(_tmp1222,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp1226;void*_tmp1225;(_tmp1225=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1226="same type variable has different identity!",_tag_dyneither(_tmp1226,sizeof(char),43))),_tag_dyneither(_tmp1225,sizeof(void*),0)));}}
return tvs;}}}
# 3563
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp1227;return(_tmp1227=_cycalloc(sizeof(*_tmp1227)),((_tmp1227->hd=tv,((_tmp1227->tl=tvs,_tmp1227)))));};}
# 3569
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3571
if(tv->identity == - 1){
const char*_tmp122A;void*_tmp1229;(_tmp1229=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp122A="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp122A,sizeof(char),39))),_tag_dyneither(_tmp1229,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3575
struct Cyc_Absyn_Tvar*_tmp965=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp965->identity == - 1){
const char*_tmp122D;void*_tmp122C;(_tmp122C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp122D="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp122D,sizeof(char),41))),_tag_dyneither(_tmp122C,sizeof(void*),0)));}
if(_tmp965->identity == tv->identity)
return tvs;}}{
# 3582
struct Cyc_List_List*_tmp122E;return(_tmp122E=_cycalloc(sizeof(*_tmp122E)),((_tmp122E->hd=tv,((_tmp122E->tl=tvs,_tmp122E)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3588
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3593
if(tv->identity == - 1){
const char*_tmp1231;void*_tmp1230;(_tmp1230=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1231="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp1231,sizeof(char),44))),_tag_dyneither(_tmp1230,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3597
struct _tuple28*_tmp96B=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp96D;int*_tmp96E;struct _tuple28*_tmp96C=_tmp96B;_tmp96D=_tmp96C->f1;_tmp96E=(int*)& _tmp96C->f2;
if(_tmp96D->identity == - 1){
const char*_tmp1234;void*_tmp1233;(_tmp1233=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1234="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp1234,sizeof(char),46))),_tag_dyneither(_tmp1233,sizeof(void*),0)));}
if(_tmp96D->identity == tv->identity){
*_tmp96E=*_tmp96E  || b;
return tvs;}}}{
# 3605
struct _tuple28*_tmp1237;struct Cyc_List_List*_tmp1236;return(_tmp1236=_region_malloc(r,sizeof(*_tmp1236)),((_tmp1236->hd=((_tmp1237=_region_malloc(r,sizeof(*_tmp1237)),((_tmp1237->f1=tv,((_tmp1237->f2=b,_tmp1237)))))),((_tmp1236->tl=tvs,_tmp1236)))));};}
# 3609
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3611
if(tv->identity == - 1){
const char*_tmp123B;void*_tmp123A[1];struct Cyc_String_pa_PrintArg_struct _tmp1239;(_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp123A[0]=& _tmp1239,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp123B="bound tvar id for %s is NULL",_tag_dyneither(_tmp123B,sizeof(char),29))),_tag_dyneither(_tmp123A,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp123C;return(_tmp123C=_cycalloc(sizeof(*_tmp123C)),((_tmp123C->hd=tv,((_tmp123C->tl=tvs,_tmp123C)))));};}struct _tuple29{void*f1;int f2;};
# 3620
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3623
void*_tmp977=Cyc_Tcutil_compress(e);void*_tmp978=_tmp977;int _tmp97A;_LL575: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp979=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp978;if(_tmp979->tag != 1)goto _LL577;else{_tmp97A=_tmp979->f3;}}_LL576:
# 3625
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp97B=(struct _tuple29*)es2->hd;void*_tmp97D;int*_tmp97E;struct _tuple29*_tmp97C=_tmp97B;_tmp97D=_tmp97C->f1;_tmp97E=(int*)& _tmp97C->f2;{
void*_tmp97F=Cyc_Tcutil_compress(_tmp97D);void*_tmp980=_tmp97F;int _tmp982;_LL57A: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp981=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp980;if(_tmp981->tag != 1)goto _LL57C;else{_tmp982=_tmp981->f3;}}_LL57B:
# 3629
 if(_tmp97A == _tmp982){
if(b != *_tmp97E)*_tmp97E=1;
return es;}
# 3633
goto _LL579;_LL57C:;_LL57D:
 goto _LL579;_LL579:;};}}{
# 3637
struct _tuple29*_tmp123F;struct Cyc_List_List*_tmp123E;return(_tmp123E=_region_malloc(r,sizeof(*_tmp123E)),((_tmp123E->hd=((_tmp123F=_region_malloc(r,sizeof(*_tmp123F)),((_tmp123F->f1=e,((_tmp123F->f2=b,_tmp123F)))))),((_tmp123E->tl=es,_tmp123E)))));};_LL577:;_LL578:
 return es;_LL574:;}
# 3642
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3645
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3654
if(!present){struct Cyc_List_List*_tmp1240;r=((_tmp1240=_region_malloc(rgn,sizeof(*_tmp1240)),((_tmp1240->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1240->tl=r,_tmp1240))))));}}
# 3656
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3661
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3665
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp986=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp988;int _tmp989;struct _tuple28 _tmp987=_tmp986;_tmp988=_tmp987.f1;_tmp989=_tmp987.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp988->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3675
if(!present){struct Cyc_List_List*_tmp1241;res=((_tmp1241=_region_malloc(r,sizeof(*_tmp1241)),((_tmp1241->hd=(struct _tuple28*)tvs->hd,((_tmp1241->tl=res,_tmp1241))))));}};}
# 3677
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3681
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3683
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp1245;void*_tmp1244[1];struct Cyc_String_pa_PrintArg_struct _tmp1243;(_tmp1243.tag=0,((_tmp1243.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1244[0]=& _tmp1243,Cyc_Tcutil_terr(loc,((_tmp1245="bitfield %s does not have constant width",_tag_dyneither(_tmp1245,sizeof(char),41))),_tag_dyneither(_tmp1244,sizeof(void*),1)))))));}else{
# 3688
struct _tuple14 _tmp98E=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp990;int _tmp991;struct _tuple14 _tmp98F=_tmp98E;_tmp990=_tmp98F.f1;_tmp991=_tmp98F.f2;
if(!_tmp991){
const char*_tmp1248;void*_tmp1247;(_tmp1247=0,Cyc_Tcutil_terr(loc,((_tmp1248="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp1248,sizeof(char),45))),_tag_dyneither(_tmp1247,sizeof(void*),0)));}
w=_tmp990;}{
# 3693
void*_tmp994=Cyc_Tcutil_compress(field_typ);void*_tmp995=_tmp994;enum Cyc_Absyn_Size_of _tmp997;_LL57F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp996=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp995;if(_tmp996->tag != 6)goto _LL581;else{_tmp997=_tmp996->f2;}}_LL580:
# 3696
 switch(_tmp997){case Cyc_Absyn_Char_sz: _LL583:
 if(w > 8){const char*_tmp124B;void*_tmp124A;(_tmp124A=0,Cyc_Tcutil_terr(loc,((_tmp124B="bitfield larger than type",_tag_dyneither(_tmp124B,sizeof(char),26))),_tag_dyneither(_tmp124A,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL584:
 if(w > 16){const char*_tmp124E;void*_tmp124D;(_tmp124D=0,Cyc_Tcutil_terr(loc,((_tmp124E="bitfield larger than type",_tag_dyneither(_tmp124E,sizeof(char),26))),_tag_dyneither(_tmp124D,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL585:
 goto _LL586;case Cyc_Absyn_Int_sz: _LL586:
# 3701
 if(w > 32){const char*_tmp1251;void*_tmp1250;(_tmp1250=0,Cyc_Tcutil_terr(loc,((_tmp1251="bitfield larger than type",_tag_dyneither(_tmp1251,sizeof(char),26))),_tag_dyneither(_tmp1250,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL587:
# 3703
 if(w > 64){const char*_tmp1254;void*_tmp1253;(_tmp1253=0,Cyc_Tcutil_terr(loc,((_tmp1254="bitfield larger than type",_tag_dyneither(_tmp1254,sizeof(char),26))),_tag_dyneither(_tmp1253,sizeof(void*),0)));}break;}
# 3705
goto _LL57E;_LL581:;_LL582:
# 3707
{const char*_tmp1259;void*_tmp1258[2];struct Cyc_String_pa_PrintArg_struct _tmp1257;struct Cyc_String_pa_PrintArg_struct _tmp1256;(_tmp1256.tag=0,((_tmp1256.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp1257.tag=0,((_tmp1257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp1258[0]=& _tmp1257,((_tmp1258[1]=& _tmp1256,Cyc_Tcutil_terr(loc,((_tmp1259="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp1259,sizeof(char),52))),_tag_dyneither(_tmp1258,sizeof(void*),2)))))))))))));}
goto _LL57E;_LL57E:;};}}
# 3714
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp9A4=(void*)atts->hd;void*_tmp9A5=_tmp9A4;_LL58A: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp9A6=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp9A5;if(_tmp9A6->tag != 7)goto _LL58C;}_LL58B:
 continue;_LL58C: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp9A7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp9A5;if(_tmp9A7->tag != 6)goto _LL58E;}_LL58D:
 continue;_LL58E:;_LL58F: {
const char*_tmp125E;void*_tmp125D[2];struct Cyc_String_pa_PrintArg_struct _tmp125C;struct Cyc_String_pa_PrintArg_struct _tmp125B;(_tmp125B.tag=0,((_tmp125B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp125C.tag=0,((_tmp125C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp125D[0]=& _tmp125C,((_tmp125D[1]=& _tmp125B,Cyc_Tcutil_terr(loc,((_tmp125E="bad attribute %s on member %s",_tag_dyneither(_tmp125E,sizeof(char),30))),_tag_dyneither(_tmp125D,sizeof(void*),2)))))))))))));}_LL589:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3738
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3742
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp9AC=t;struct Cyc_Absyn_Typedefdecl*_tmp9AE;void*_tmp9AF;_LL591: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9AD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9AC;if(_tmp9AD->tag != 17)goto _LL593;else{_tmp9AE=_tmp9AD->f3;_tmp9AF=(void*)_tmp9AD->f4;}}_LL592:
# 3745
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp9AE))->tq).real_const  || (_tmp9AE->tq).print_const){
if(declared_const){const char*_tmp1261;void*_tmp1260;(_tmp1260=0,Cyc_Tcutil_warn(loc,((_tmp1261="extra const",_tag_dyneither(_tmp1261,sizeof(char),12))),_tag_dyneither(_tmp1260,sizeof(void*),0)));}
return 1;}
# 3750
if((unsigned int)_tmp9AF)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp9AF);else{
return declared_const;}_LL593:;_LL594:
 return declared_const;_LL590:;}
# 3757
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp9B2=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp9B3=_tmp9B2;void*_tmp9B5;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9B4=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B3;if(_tmp9B4->tag != 5)goto _LL598;else{_tmp9B5=((_tmp9B4->f1).ptr_atts).rgn;}}_LL597:
# 3762
{void*_tmp9B6=Cyc_Tcutil_compress(_tmp9B5);void*_tmp9B7=_tmp9B6;struct Cyc_Absyn_Tvar*_tmp9B9;_LL59B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9B8=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9B7;if(_tmp9B8->tag != 2)goto _LL59D;else{_tmp9B9=_tmp9B8->f1;}}_LL59C:
# 3764
 return Cyc_Absyn_tvar_cmp(tvar,_tmp9B9)== 0;_LL59D:;_LL59E:
 goto _LL59A;_LL59A:;}
# 3767
goto _LL595;_LL598:;_LL599:
 goto _LL595;_LL595:;}}else{
# 3773
return 1;}
return 0;}
# 3777
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3780
void*_tmp9BA=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp9BB=_tmp9BA;_LL5A0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9BC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9BB;if(_tmp9BC->tag != 2)goto _LL5A2;else{if((_tmp9BC->f2)->kind != Cyc_Absyn_RgnKind)goto _LL5A2;if((_tmp9BC->f2)->aliasqual != Cyc_Absyn_Top)goto _LL5A2;}}_LL5A1:
 goto _LL5A3;_LL5A2: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9BD=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9BB;if(_tmp9BD->tag != 0)goto _LL5A4;else{if((_tmp9BD->f1)->kind != Cyc_Absyn_RgnKind)goto _LL5A4;if((_tmp9BD->f1)->aliasqual != Cyc_Absyn_Top)goto _LL5A4;}}_LL5A3:
# 3789
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3792
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3798
return& Cyc_Tcutil_trk;_LL5A4:;_LL5A5:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL59F:;}
# 3804
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3808
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3811
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause)){
const char*_tmp1266;void*_tmp1265[2];struct Cyc_String_pa_PrintArg_struct _tmp1264;struct Cyc_String_pa_PrintArg_struct _tmp1263;(_tmp1263.tag=0,((_tmp1263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt))),((_tmp1264.tag=0,((_tmp1264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp1265[0]=& _tmp1264,((_tmp1265[1]=& _tmp1263,Cyc_Tcutil_terr(loc,((_tmp1266="%s clause has type %s instead of integral type",_tag_dyneither(_tmp1266,sizeof(char),47))),_tag_dyneither(_tmp1265,sizeof(void*),2)))))))))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns)){
const char*_tmp126B;void*_tmp126A[2];struct Cyc_String_pa_PrintArg_struct _tmp1269;struct Cyc_String_pa_PrintArg_struct _tmp1268;(_tmp1268.tag=0,((_tmp1268.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause)),((_tmp1269.tag=0,((_tmp1269.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name),((_tmp126A[0]=& _tmp1269,((_tmp126A[1]=& _tmp1268,Cyc_Tcutil_terr(clause->loc,((_tmp126B="%s clause '%s' may be unsatisfiable",_tag_dyneither(_tmp126B,sizeof(char),36))),_tag_dyneither(_tmp126A,sizeof(void*),2)))))))))))));}}{
# 3823
struct _tuple30 _tmp126C;return(_tmp126C.f1=cvtenv,((_tmp126C.f2=relns,_tmp126C)));};}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3855 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3862
{void*_tmp9C7=Cyc_Tcutil_compress(t);void*_tmp9C8=_tmp9C7;struct Cyc_Core_Opt**_tmp9CB;void**_tmp9CC;struct Cyc_Absyn_Tvar*_tmp9CE;void*_tmp9D0;void***_tmp9D1;struct Cyc_List_List*_tmp9D3;struct _tuple2*_tmp9D5;struct Cyc_Absyn_Enumdecl**_tmp9D6;union Cyc_Absyn_DatatypeInfoU*_tmp9D8;struct Cyc_List_List**_tmp9D9;union Cyc_Absyn_DatatypeFieldInfoU*_tmp9DB;struct Cyc_List_List*_tmp9DC;void*_tmp9DE;struct Cyc_Absyn_Tqual*_tmp9DF;void*_tmp9E0;union Cyc_Absyn_Constraint*_tmp9E1;union Cyc_Absyn_Constraint*_tmp9E2;union Cyc_Absyn_Constraint*_tmp9E3;void*_tmp9E5;struct Cyc_Absyn_Exp*_tmp9E7;void*_tmp9EB;struct Cyc_Absyn_Tqual*_tmp9EC;struct Cyc_Absyn_Exp**_tmp9ED;union Cyc_Absyn_Constraint*_tmp9EE;unsigned int _tmp9EF;struct Cyc_List_List**_tmp9F1;void**_tmp9F2;struct Cyc_Absyn_Tqual*_tmp9F3;void*_tmp9F4;struct Cyc_List_List*_tmp9F5;int _tmp9F6;struct Cyc_Absyn_VarargInfo*_tmp9F7;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9F9;struct Cyc_Absyn_Exp*_tmp9FA;struct Cyc_List_List**_tmp9FB;struct Cyc_Absyn_Exp*_tmp9FC;struct Cyc_List_List**_tmp9FD;struct Cyc_List_List*_tmp9FF;enum Cyc_Absyn_AggrKind _tmpA01;struct Cyc_List_List*_tmpA02;union Cyc_Absyn_AggrInfoU*_tmpA04;struct Cyc_List_List**_tmpA05;struct _tuple2*_tmpA07;struct Cyc_List_List**_tmpA08;struct Cyc_Absyn_Typedefdecl**_tmpA09;void**_tmpA0A;void*_tmpA0F;void*_tmpA11;void*_tmpA12;void*_tmpA14;void*_tmpA16;struct Cyc_List_List*_tmpA18;_LL5A7: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp9C9=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp9C8;if(_tmp9C9->tag != 0)goto _LL5A9;}_LL5A8:
 goto _LL5A6;_LL5A9: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp9CA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp9C8;if(_tmp9CA->tag != 1)goto _LL5AB;else{_tmp9CB=(struct Cyc_Core_Opt**)& _tmp9CA->f1;_tmp9CC=(void**)((void**)& _tmp9CA->f2);}}_LL5AA:
# 3866
 if(*_tmp9CB == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9CB))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp9CB))->v,expected_kind))
*_tmp9CB=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3871
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp9CC=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3874
*_tmp9CC=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp126F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp126E;struct Cyc_Absyn_Tvar*_tmpA19=Cyc_Tcutil_new_tvar((void*)((_tmp126E=_cycalloc(sizeof(*_tmp126E)),((_tmp126E[0]=((_tmp126F.tag=2,((_tmp126F.f1=0,((_tmp126F.f2=expected_kind,_tmp126F)))))),_tmp126E)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1272;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1271;*_tmp9CC=(void*)((_tmp1271=_cycalloc(sizeof(*_tmp1271)),((_tmp1271[0]=((_tmp1272.tag=2,((_tmp1272.f1=_tmpA19,_tmp1272)))),_tmp1271))));}
_tmp9CE=_tmpA19;goto _LL5AC;}else{
# 3880
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3882
goto _LL5A6;_LL5AB: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp9CD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C8;if(_tmp9CD->tag != 2)goto _LL5AD;else{_tmp9CE=_tmp9CD->f1;}}_LL5AC:
# 3884
{void*_tmpA1E=Cyc_Absyn_compress_kb(_tmp9CE->kind);void*_tmpA1F=_tmpA1E;struct Cyc_Core_Opt**_tmpA21;_LL5DE: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA20=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA1F;if(_tmpA20->tag != 1)goto _LL5E0;else{_tmpA21=(struct Cyc_Core_Opt**)& _tmpA20->f1;}}_LL5DF:
# 3886
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1278;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1277;struct Cyc_Core_Opt*_tmp1276;*_tmpA21=((_tmp1276=_cycalloc(sizeof(*_tmp1276)),((_tmp1276->v=(void*)((_tmp1278=_cycalloc(sizeof(*_tmp1278)),((_tmp1278[0]=((_tmp1277.tag=2,((_tmp1277.f1=0,((_tmp1277.f2=expected_kind,_tmp1277)))))),_tmp1278)))),_tmp1276))));}goto _LL5DD;_LL5E0:;_LL5E1:
 goto _LL5DD;_LL5DD:;}
# 3891
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp9CE);
# 3894
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp9CE,put_in_effect);
# 3896
{void*_tmpA25=Cyc_Absyn_compress_kb(_tmp9CE->kind);void*_tmpA26=_tmpA25;struct Cyc_Core_Opt**_tmpA28;struct Cyc_Absyn_Kind*_tmpA29;_LL5E3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA27=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA26;if(_tmpA27->tag != 2)goto _LL5E5;else{_tmpA28=(struct Cyc_Core_Opt**)& _tmpA27->f1;_tmpA29=_tmpA27->f2;}}_LL5E4:
# 3898
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmpA29)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp127E;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp127D;struct Cyc_Core_Opt*_tmp127C;*_tmpA28=((_tmp127C=_cycalloc(sizeof(*_tmp127C)),((_tmp127C->v=(void*)((_tmp127E=_cycalloc(sizeof(*_tmp127E)),((_tmp127E[0]=((_tmp127D.tag=2,((_tmp127D.f1=0,((_tmp127D.f2=expected_kind,_tmp127D)))))),_tmp127E)))),_tmp127C))));}
goto _LL5E2;_LL5E5:;_LL5E6:
 goto _LL5E2;_LL5E2:;}
# 3903
goto _LL5A6;_LL5AD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp9CF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp9C8;if(_tmp9CF->tag != 26)goto _LL5AF;else{_tmp9D0=(_tmp9CF->f1)->r;_tmp9D1=(void***)& _tmp9CF->f2;}}_LL5AE: {
# 3909
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmpA2D=_tmp9D0;struct Cyc_Absyn_Aggrdecl*_tmpA2F;struct Cyc_Absyn_Enumdecl*_tmpA31;struct Cyc_Absyn_Datatypedecl*_tmpA33;_LL5E8: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpA2E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmpA2D;if(_tmpA2E->tag != 0)goto _LL5EA;else{_tmpA2F=_tmpA2E->f1;}}_LL5E9:
# 3912
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA2F);goto _LL5E7;_LL5EA: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpA30=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmpA2D;if(_tmpA30->tag != 1)goto _LL5EC;else{_tmpA31=_tmpA30->f1;}}_LL5EB:
# 3914
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA31);goto _LL5E7;_LL5EC: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpA32=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmpA2D;if(_tmpA32->tag != 2)goto _LL5E7;else{_tmpA33=_tmpA32->f1;}}_LL5ED:
# 3916
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmpA33);goto _LL5E7;_LL5E7:;}
# 3918
{void**_tmp127F;*_tmp9D1=((_tmp127F=_cycalloc(sizeof(*_tmp127F)),((_tmp127F[0]=new_t,_tmp127F))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL5AF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9D2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp9C8;if(_tmp9D2->tag != 14)goto _LL5B1;else{_tmp9D3=_tmp9D2->f1;}}_LL5B0: {
# 3923
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmpA35=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpA35;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp9D3 != 0;_tmp9D3=_tmp9D3->tl){
struct Cyc_Absyn_Enumfield*_tmpA36=(struct Cyc_Absyn_Enumfield*)_tmp9D3->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmpA36->name).f2)){
const char*_tmp1283;void*_tmp1282[1];struct Cyc_String_pa_PrintArg_struct _tmp1281;(_tmp1281.tag=0,((_tmp1281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA36->name).f2),((_tmp1282[0]=& _tmp1281,Cyc_Tcutil_terr(_tmpA36->loc,((_tmp1283="duplicate enum field name %s",_tag_dyneither(_tmp1283,sizeof(char),29))),_tag_dyneither(_tmp1282,sizeof(void*),1)))))));}else{
# 3932
struct Cyc_List_List*_tmp1284;prev_fields=((_tmp1284=_region_malloc(uprev_rgn,sizeof(*_tmp1284)),((_tmp1284->hd=(*_tmpA36->name).f2,((_tmp1284->tl=prev_fields,_tmp1284))))));}
# 3934
if(_tmpA36->tag == 0)
_tmpA36->tag=Cyc_Absyn_uint_exp(tag_count,_tmpA36->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA36->tag))){
const char*_tmp1288;void*_tmp1287[1];struct Cyc_String_pa_PrintArg_struct _tmp1286;(_tmp1286.tag=0,((_tmp1286.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmpA36->name).f2),((_tmp1287[0]=& _tmp1286,Cyc_Tcutil_terr(loc,((_tmp1288="enum field %s: expression is not constant",_tag_dyneither(_tmp1288,sizeof(char),42))),_tag_dyneither(_tmp1287,sizeof(void*),1)))))));}}{
# 3939
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmpA36->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmpA3E=(*_tmpA36->name).f1;union Cyc_Absyn_Nmspace _tmpA3F=_tmpA3E;_LL5EF: if((_tmpA3F.Rel_n).tag != 1)goto _LL5F1;_LL5F0:
# 3943
(*_tmpA36->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5EE;_LL5F1:;_LL5F2:
 goto _LL5EE;_LL5EE:;}{
# 3946
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp128E;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp128D;struct _tuple29*_tmp128C;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmpA36->name).f2,(
(_tmp128C=_cycalloc(sizeof(*_tmp128C)),((_tmp128C->f1=(void*)((_tmp128E=_cycalloc(sizeof(*_tmp128E)),((_tmp128E[0]=((_tmp128D.tag=4,((_tmp128D.f1=t,((_tmp128D.f2=_tmpA36,_tmp128D)))))),_tmp128E)))),((_tmp128C->f2=1,_tmp128C)))))));};};}}
# 3950
_npop_handler(0);goto _LL5A6;
# 3924
;_pop_region(uprev_rgn);}_LL5B1: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9D4=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp9C8;if(_tmp9D4->tag != 13)goto _LL5B3;else{_tmp9D5=_tmp9D4->f1;_tmp9D6=(struct Cyc_Absyn_Enumdecl**)& _tmp9D4->f2;}}_LL5B2:
# 3952
 if(*_tmp9D6 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9D6))->fields == 0){
struct _handler_cons _tmpA43;_push_handler(& _tmpA43);{int _tmpA45=0;if(setjmp(_tmpA43.handler))_tmpA45=1;if(!_tmpA45){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9D5);
*_tmp9D6=*ed;}
# 3954
;_pop_handler();}else{void*_tmpA44=(void*)_exn_thrown;void*_tmpA47=_tmpA44;void*_tmpA49;_LL5F4: {struct Cyc_Dict_Absent_exn_struct*_tmpA48=(struct Cyc_Dict_Absent_exn_struct*)_tmpA47;if(_tmpA48->tag != Cyc_Dict_Absent)goto _LL5F6;}_LL5F5: {
# 3958
struct Cyc_Tcenv_Genv*_tmpA4A=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp128F;struct Cyc_Absyn_Enumdecl*_tmpA4B=(_tmp128F=_cycalloc(sizeof(*_tmp128F)),((_tmp128F->sc=Cyc_Absyn_Extern,((_tmp128F->name=_tmp9D5,((_tmp128F->fields=0,_tmp128F)))))));
Cyc_Tc_tcEnumdecl(te,_tmpA4A,loc,_tmpA4B);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp9D5);
*_tmp9D6=*ed;
goto _LL5F3;};}_LL5F6: _tmpA49=_tmpA47;_LL5F7:(void)_throw(_tmpA49);_LL5F3:;}};}{
# 3967
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp9D6);
# 3969
*_tmp9D5=(ed->name)[0];
goto _LL5A6;};_LL5B3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9D7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9C8;if(_tmp9D7->tag != 3)goto _LL5B5;else{_tmp9D8=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp9D7->f1).datatype_info;_tmp9D9=(struct Cyc_List_List**)&(_tmp9D7->f1).targs;}}_LL5B4: {
# 3972
struct Cyc_List_List*_tmpA4D=*_tmp9D9;
{union Cyc_Absyn_DatatypeInfoU _tmpA4E=*_tmp9D8;union Cyc_Absyn_DatatypeInfoU _tmpA4F=_tmpA4E;struct _tuple2*_tmpA50;int _tmpA51;struct Cyc_Absyn_Datatypedecl*_tmpA52;_LL5F9: if((_tmpA4F.UnknownDatatype).tag != 1)goto _LL5FB;_tmpA50=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA4F.UnknownDatatype).val).name;_tmpA51=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA4F.UnknownDatatype).val).is_extensible;_LL5FA: {
# 3975
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA53;_push_handler(& _tmpA53);{int _tmpA55=0;if(setjmp(_tmpA53.handler))_tmpA55=1;if(!_tmpA55){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA50);;_pop_handler();}else{void*_tmpA54=(void*)_exn_thrown;void*_tmpA57=_tmpA54;void*_tmpA59;_LL5FE: {struct Cyc_Dict_Absent_exn_struct*_tmpA58=(struct Cyc_Dict_Absent_exn_struct*)_tmpA57;if(_tmpA58->tag != Cyc_Dict_Absent)goto _LL600;}_LL5FF: {
# 3979
struct Cyc_Tcenv_Genv*_tmpA5A=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1290;struct Cyc_Absyn_Datatypedecl*_tmpA5B=(_tmp1290=_cycalloc(sizeof(*_tmp1290)),((_tmp1290->sc=Cyc_Absyn_Extern,((_tmp1290->name=_tmpA50,((_tmp1290->tvs=0,((_tmp1290->fields=0,((_tmp1290->is_extensible=_tmpA51,_tmp1290)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA5A,loc,_tmpA5B);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA50);
# 3984
if(_tmpA4D != 0){
{const char*_tmp1294;void*_tmp1293[1];struct Cyc_String_pa_PrintArg_struct _tmp1292;(_tmp1292.tag=0,((_tmp1292.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA50)),((_tmp1293[0]=& _tmp1292,Cyc_Tcutil_terr(loc,((_tmp1294="declare parameterized datatype %s before using",_tag_dyneither(_tmp1294,sizeof(char),47))),_tag_dyneither(_tmp1293,sizeof(void*),1)))))));}
return cvtenv;}
# 3989
goto _LL5FD;}_LL600: _tmpA59=_tmpA57;_LL601:(void)_throw(_tmpA59);_LL5FD:;}};}
# 3995
if(_tmpA51  && !(*tudp)->is_extensible){
const char*_tmp1298;void*_tmp1297[1];struct Cyc_String_pa_PrintArg_struct _tmp1296;(_tmp1296.tag=0,((_tmp1296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA50)),((_tmp1297[0]=& _tmp1296,Cyc_Tcutil_terr(loc,((_tmp1298="datatype %s was not declared @extensible",_tag_dyneither(_tmp1298,sizeof(char),41))),_tag_dyneither(_tmp1297,sizeof(void*),1)))))));}
*_tmp9D8=Cyc_Absyn_KnownDatatype(tudp);
_tmpA52=*tudp;goto _LL5FC;}_LL5FB: if((_tmpA4F.KnownDatatype).tag != 2)goto _LL5F8;_tmpA52=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA4F.KnownDatatype).val);_LL5FC: {
# 4002
struct Cyc_List_List*tvs=_tmpA52->tvs;
for(0;_tmpA4D != 0  && tvs != 0;(_tmpA4D=_tmpA4D->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA4D->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4008
{struct _tuple0 _tmp1299;struct _tuple0 _tmpA63=(_tmp1299.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1299.f2=t,_tmp1299)));struct _tuple0 _tmpA64=_tmpA63;struct Cyc_Absyn_Tvar*_tmpA67;_LL603:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA65=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA64.f1;if(_tmpA65->tag != 1)goto _LL605;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA66=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA64.f2;if(_tmpA66->tag != 2)goto _LL605;else{_tmpA67=_tmpA66->f1;}};_LL604:
# 4010
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA67);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA67,1);
continue;_LL605:;_LL606:
 goto _LL602;_LL602:;}{
# 4015
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4019
if(_tmpA4D != 0){
const char*_tmp129D;void*_tmp129C[1];struct Cyc_String_pa_PrintArg_struct _tmp129B;(_tmp129B.tag=0,((_tmp129B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA52->name)),((_tmp129C[0]=& _tmp129B,Cyc_Tcutil_terr(loc,((_tmp129D="too many type arguments for datatype %s",_tag_dyneither(_tmp129D,sizeof(char),40))),_tag_dyneither(_tmp129C,sizeof(void*),1)))))));}
if(tvs != 0){
# 4024
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp129E;hidden_ts=((_tmp129E=_cycalloc(sizeof(*_tmp129E)),((_tmp129E->hd=e,((_tmp129E->tl=hidden_ts,_tmp129E))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 4031
*_tmp9D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9D9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4033
goto _LL5F8;}_LL5F8:;}
# 4035
goto _LL5A6;}_LL5B5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp9DA=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9C8;if(_tmp9DA->tag != 4)goto _LL5B7;else{_tmp9DB=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp9DA->f1).field_info;_tmp9DC=(_tmp9DA->f1).targs;}}_LL5B6:
# 4038
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA6D=*_tmp9DB;union Cyc_Absyn_DatatypeFieldInfoU _tmpA6E=_tmpA6D;struct _tuple2*_tmpA6F;struct _tuple2*_tmpA70;int _tmpA71;struct Cyc_Absyn_Datatypedecl*_tmpA72;struct Cyc_Absyn_Datatypefield*_tmpA73;_LL608: if((_tmpA6E.UnknownDatatypefield).tag != 1)goto _LL60A;_tmpA6F=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6E.UnknownDatatypefield).val).datatype_name;_tmpA70=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6E.UnknownDatatypefield).val).field_name;_tmpA71=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA6E.UnknownDatatypefield).val).is_extensible;_LL609: {
# 4040
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA74;_push_handler(& _tmpA74);{int _tmpA76=0;if(setjmp(_tmpA74.handler))_tmpA76=1;if(!_tmpA76){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA6F);;_pop_handler();}else{void*_tmpA75=(void*)_exn_thrown;void*_tmpA78=_tmpA75;void*_tmpA7A;_LL60D: {struct Cyc_Dict_Absent_exn_struct*_tmpA79=(struct Cyc_Dict_Absent_exn_struct*)_tmpA78;if(_tmpA79->tag != Cyc_Dict_Absent)goto _LL60F;}_LL60E:
# 4044
{const char*_tmp12A2;void*_tmp12A1[1];struct Cyc_String_pa_PrintArg_struct _tmp12A0;(_tmp12A0.tag=0,((_tmp12A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA6F)),((_tmp12A1[0]=& _tmp12A0,Cyc_Tcutil_terr(loc,((_tmp12A2="unbound datatype %s",_tag_dyneither(_tmp12A2,sizeof(char),20))),_tag_dyneither(_tmp12A1,sizeof(void*),1)))))));}
return cvtenv;_LL60F: _tmpA7A=_tmpA78;_LL610:(void)_throw(_tmpA7A);_LL60C:;}};}
# 4047
{struct _handler_cons _tmpA7E;_push_handler(& _tmpA7E);{int _tmpA80=0;if(setjmp(_tmpA7E.handler))_tmpA80=1;if(!_tmpA80){
{struct _RegionHandle _tmpA81=_new_region("r");struct _RegionHandle*r=& _tmpA81;_push_region(r);
{void*_tmpA82=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA70,0);void*_tmpA83=_tmpA82;struct Cyc_Absyn_Datatypedecl*_tmpA85;struct Cyc_Absyn_Datatypefield*_tmpA86;_LL612: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA84=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA83;if(_tmpA84->tag != 2)goto _LL614;else{_tmpA85=_tmpA84->f1;_tmpA86=_tmpA84->f2;}}_LL613:
# 4051
 tuf=_tmpA86;
tud=_tmpA85;
if(_tmpA71  && !tud->is_extensible){
const char*_tmp12A6;void*_tmp12A5[1];struct Cyc_String_pa_PrintArg_struct _tmp12A4;(_tmp12A4.tag=0,((_tmp12A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA6F)),((_tmp12A5[0]=& _tmp12A4,Cyc_Tcutil_terr(loc,((_tmp12A6="datatype %s was not declared @extensible",_tag_dyneither(_tmp12A6,sizeof(char),41))),_tag_dyneither(_tmp12A5,sizeof(void*),1)))))));}
goto _LL611;_LL614:;_LL615:
{const char*_tmp12AB;void*_tmp12AA[2];struct Cyc_String_pa_PrintArg_struct _tmp12A9;struct Cyc_String_pa_PrintArg_struct _tmp12A8;(_tmp12A8.tag=0,((_tmp12A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA6F)),((_tmp12A9.tag=0,((_tmp12A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12AA[0]=& _tmp12A9,((_tmp12AA[1]=& _tmp12A8,Cyc_Tcutil_terr(loc,((_tmp12AB="unbound field %s in type datatype %s",_tag_dyneither(_tmp12AB,sizeof(char),37))),_tag_dyneither(_tmp12AA,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpA8E=cvtenv;_npop_handler(1);return _tmpA8E;};_LL611:;}
# 4049
;_pop_region(r);}
# 4048
;_pop_handler();}else{void*_tmpA7F=(void*)_exn_thrown;void*_tmpA90=_tmpA7F;void*_tmpA92;_LL617: {struct Cyc_Dict_Absent_exn_struct*_tmpA91=(struct Cyc_Dict_Absent_exn_struct*)_tmpA90;if(_tmpA91->tag != Cyc_Dict_Absent)goto _LL619;}_LL618:
# 4064
{const char*_tmp12B0;void*_tmp12AF[2];struct Cyc_String_pa_PrintArg_struct _tmp12AE;struct Cyc_String_pa_PrintArg_struct _tmp12AD;(_tmp12AD.tag=0,((_tmp12AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA6F)),((_tmp12AE.tag=0,((_tmp12AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA70)),((_tmp12AF[0]=& _tmp12AE,((_tmp12AF[1]=& _tmp12AD,Cyc_Tcutil_terr(loc,((_tmp12B0="unbound field %s in type datatype %s",_tag_dyneither(_tmp12B0,sizeof(char),37))),_tag_dyneither(_tmp12AF,sizeof(void*),2)))))))))))));}
return cvtenv;_LL619: _tmpA92=_tmpA90;_LL61A:(void)_throw(_tmpA92);_LL616:;}};}
# 4068
*_tmp9DB=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA72=tud;_tmpA73=tuf;goto _LL60B;}_LL60A: if((_tmpA6E.KnownDatatypefield).tag != 2)goto _LL607;_tmpA72=((struct _tuple3)(_tmpA6E.KnownDatatypefield).val).f1;_tmpA73=((struct _tuple3)(_tmpA6E.KnownDatatypefield).val).f2;_LL60B: {
# 4072
struct Cyc_List_List*tvs=_tmpA72->tvs;
for(0;_tmp9DC != 0  && tvs != 0;(_tmp9DC=_tmp9DC->tl,tvs=tvs->tl)){
void*t=(void*)_tmp9DC->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4078
{struct _tuple0 _tmp12B1;struct _tuple0 _tmpA97=(_tmp12B1.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp12B1.f2=t,_tmp12B1)));struct _tuple0 _tmpA98=_tmpA97;struct Cyc_Absyn_Tvar*_tmpA9B;_LL61C:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA99=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA98.f1;if(_tmpA99->tag != 1)goto _LL61E;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA9A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA98.f2;if(_tmpA9A->tag != 2)goto _LL61E;else{_tmpA9B=_tmpA9A->f1;}};_LL61D:
# 4080
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA9B);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA9B,1);
continue;_LL61E:;_LL61F:
 goto _LL61B;_LL61B:;}{
# 4085
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4089
if(_tmp9DC != 0){
const char*_tmp12B6;void*_tmp12B5[2];struct Cyc_String_pa_PrintArg_struct _tmp12B4;struct Cyc_String_pa_PrintArg_struct _tmp12B3;(_tmp12B3.tag=0,((_tmp12B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA73->name)),((_tmp12B4.tag=0,((_tmp12B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA72->name)),((_tmp12B5[0]=& _tmp12B4,((_tmp12B5[1]=& _tmp12B3,Cyc_Tcutil_terr(loc,((_tmp12B6="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp12B6,sizeof(char),43))),_tag_dyneither(_tmp12B5,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp12BB;void*_tmp12BA[2];struct Cyc_String_pa_PrintArg_struct _tmp12B9;struct Cyc_String_pa_PrintArg_struct _tmp12B8;(_tmp12B8.tag=0,((_tmp12B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA73->name)),((_tmp12B9.tag=0,((_tmp12B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA72->name)),((_tmp12BA[0]=& _tmp12B9,((_tmp12BA[1]=& _tmp12B8,Cyc_Tcutil_terr(loc,((_tmp12BB="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp12BB,sizeof(char),42))),_tag_dyneither(_tmp12BA,sizeof(void*),2)))))))))))));}
goto _LL607;}_LL607:;}
# 4097
goto _LL5A6;_LL5B7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9DD=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9C8;if(_tmp9DD->tag != 5)goto _LL5B9;else{_tmp9DE=(_tmp9DD->f1).elt_typ;_tmp9DF=(struct Cyc_Absyn_Tqual*)&(_tmp9DD->f1).elt_tq;_tmp9E0=((_tmp9DD->f1).ptr_atts).rgn;_tmp9E1=((_tmp9DD->f1).ptr_atts).nullable;_tmp9E2=((_tmp9DD->f1).ptr_atts).bounds;_tmp9E3=((_tmp9DD->f1).ptr_atts).zero_term;}}_LL5B8: {
# 4100
int is_zero_terminated;
# 4102
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9DE,1);
_tmp9DF->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9DF->print_const,_tmp9DE);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpAA5=expected_kind->aliasqual;switch(_tmpAA5){case Cyc_Absyn_Aliasable: _LL620:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL621:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL622:
 k=& Cyc_Tcutil_trk;break;}}
# 4110
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp9E0,1);
{union Cyc_Absyn_Constraint*_tmpAA6=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9E3);union Cyc_Absyn_Constraint*_tmpAA7=_tmpAA6;_LL625: if((_tmpAA7->No_constr).tag != 3)goto _LL627;_LL626:
# 4115
{void*_tmpAA8=Cyc_Tcutil_compress(_tmp9DE);void*_tmpAA9=_tmpAA8;_LL62C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpAAA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpAA9;if(_tmpAAA->tag != 6)goto _LL62E;else{if(_tmpAAA->f2 != Cyc_Absyn_Char_sz)goto _LL62E;}}_LL62D:
# 4117
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9E3,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL62B;_LL62E:;_LL62F:
# 4121
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9E3,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL62B;_LL62B:;}
# 4125
goto _LL624;_LL627: if((_tmpAA7->Eq_constr).tag != 1)goto _LL629;if((int)(_tmpAA7->Eq_constr).val != 1)goto _LL629;_LL628:
# 4128
 if(!Cyc_Tcutil_admits_zero(_tmp9DE)){
const char*_tmp12BF;void*_tmp12BE[1];struct Cyc_String_pa_PrintArg_struct _tmp12BD;(_tmp12BD.tag=0,((_tmp12BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9DE)),((_tmp12BE[0]=& _tmp12BD,Cyc_Tcutil_terr(loc,((_tmp12BF="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp12BF,sizeof(char),53))),_tag_dyneither(_tmp12BE,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL624;_LL629:;_LL62A:
# 4134
 is_zero_terminated=0;
goto _LL624;_LL624:;}
# 4138
{void*_tmpAAE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp9E2);void*_tmpAAF=_tmpAAE;struct Cyc_Absyn_Exp*_tmpAB2;_LL631: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAB0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAAF;if(_tmpAB0->tag != 0)goto _LL633;}_LL632:
 goto _LL630;_LL633: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAB1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAAF;if(_tmpAB1->tag != 1)goto _LL630;else{_tmpAB2=_tmpAB1->f1;}}_LL634: {
# 4141
struct _RegionHandle _tmpAB3=_new_region("temp");struct _RegionHandle*temp=& _tmpAB3;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpAB4=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpAB4,0,_tmpAB2);}
# 4145
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAB2,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAB2)){
const char*_tmp12C2;void*_tmp12C1;(_tmp12C1=0,Cyc_Tcutil_terr(loc,((_tmp12C2="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp12C2,sizeof(char),49))),_tag_dyneither(_tmp12C1,sizeof(void*),0)));}{
struct _tuple14 _tmpAB7=Cyc_Evexp_eval_const_uint_exp(_tmpAB2);unsigned int _tmpAB9;int _tmpABA;struct _tuple14 _tmpAB8=_tmpAB7;_tmpAB9=_tmpAB8.f1;_tmpABA=_tmpAB8.f2;
if(is_zero_terminated  && (!_tmpABA  || _tmpAB9 < 1)){
const char*_tmp12C5;void*_tmp12C4;(_tmp12C4=0,Cyc_Tcutil_terr(loc,((_tmp12C5="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp12C5,sizeof(char),55))),_tag_dyneither(_tmp12C4,sizeof(void*),0)));}
_npop_handler(0);goto _LL630;};
# 4141
;_pop_region(temp);}_LL630:;}
# 4153
goto _LL5A6;};}_LL5B9: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9E4=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp9C8;if(_tmp9E4->tag != 19)goto _LL5BB;else{_tmp9E5=(void*)_tmp9E4->f1;}}_LL5BA:
# 4155
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp9E5,1);goto _LL5A6;_LL5BB: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9E6=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp9C8;if(_tmp9E6->tag != 18)goto _LL5BD;else{_tmp9E7=_tmp9E6->f1;}}_LL5BC: {
# 4160
struct _RegionHandle _tmpABD=_new_region("temp");struct _RegionHandle*temp=& _tmpABD;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpABE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpABE,0,_tmp9E7);}
# 4164
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9E7)){
const char*_tmp12C8;void*_tmp12C7;(_tmp12C7=0,Cyc_Tcutil_terr(loc,((_tmp12C8="valueof_t requires an int expression",_tag_dyneither(_tmp12C8,sizeof(char),37))),_tag_dyneither(_tmp12C7,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9E7,te,cvtenv);
_npop_handler(0);goto _LL5A6;
# 4160
;_pop_region(temp);}_LL5BD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9E8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp9C8;if(_tmp9E8->tag != 6)goto _LL5BF;}_LL5BE:
# 4168
 goto _LL5C0;_LL5BF: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp9E9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp9C8;if(_tmp9E9->tag != 7)goto _LL5C1;}_LL5C0:
 goto _LL5A6;_LL5C1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9EA=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9C8;if(_tmp9EA->tag != 8)goto _LL5C3;else{_tmp9EB=(_tmp9EA->f1).elt_type;_tmp9EC=(struct Cyc_Absyn_Tqual*)&(_tmp9EA->f1).tq;_tmp9ED=(struct Cyc_Absyn_Exp**)&(_tmp9EA->f1).num_elts;_tmp9EE=(_tmp9EA->f1).zero_term;_tmp9EF=(_tmp9EA->f1).zt_loc;}}_LL5C2: {
# 4173
struct Cyc_Absyn_Exp*_tmpAC1=*_tmp9ED;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp9EB,1);
_tmp9EC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9EC->print_const,_tmp9EB);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpAC2=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9EE);union Cyc_Absyn_Constraint*_tmpAC3=_tmpAC2;_LL636: if((_tmpAC3->No_constr).tag != 3)goto _LL638;_LL637:
# 4180
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9EE,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4195 "tcutil.cyc"
goto _LL635;_LL638: if((_tmpAC3->Eq_constr).tag != 1)goto _LL63A;if((int)(_tmpAC3->Eq_constr).val != 1)goto _LL63A;_LL639:
# 4198
 if(!Cyc_Tcutil_admits_zero(_tmp9EB)){
const char*_tmp12CC;void*_tmp12CB[1];struct Cyc_String_pa_PrintArg_struct _tmp12CA;(_tmp12CA.tag=0,((_tmp12CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9EB)),((_tmp12CB[0]=& _tmp12CA,Cyc_Tcutil_terr(loc,((_tmp12CC="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp12CC,sizeof(char),51))),_tag_dyneither(_tmp12CB,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL635;_LL63A:;_LL63B:
# 4204
 is_zero_terminated=0;
goto _LL635;_LL635:;}
# 4209
if(_tmpAC1 == 0){
# 4211
if(is_zero_terminated)
# 4213
*_tmp9ED=(_tmpAC1=Cyc_Absyn_uint_exp(1,0));else{
# 4216
{const char*_tmp12CF;void*_tmp12CE;(_tmp12CE=0,Cyc_Tcutil_warn(loc,((_tmp12CF="array bound defaults to 1 here",_tag_dyneither(_tmp12CF,sizeof(char),31))),_tag_dyneither(_tmp12CE,sizeof(void*),0)));}
*_tmp9ED=(_tmpAC1=Cyc_Absyn_uint_exp(1,0));}}{
# 4220
struct _RegionHandle _tmpAC9=_new_region("temp");struct _RegionHandle*temp=& _tmpAC9;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpACA=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpACA,0,_tmpAC1);}
# 4224
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpAC1)){
const char*_tmp12D2;void*_tmp12D1;(_tmp12D1=0,Cyc_Tcutil_terr(loc,((_tmp12D2="array bounds expression is not an unsigned int",_tag_dyneither(_tmp12D2,sizeof(char),47))),_tag_dyneither(_tmp12D1,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpAC1,te,cvtenv);{
# 4231
struct _tuple14 _tmpACD=Cyc_Evexp_eval_const_uint_exp(_tmpAC1);unsigned int _tmpACF;int _tmpAD0;struct _tuple14 _tmpACE=_tmpACD;_tmpACF=_tmpACE.f1;_tmpAD0=_tmpACE.f2;
# 4233
if((is_zero_terminated  && _tmpAD0) && _tmpACF < 1){
const char*_tmp12D5;void*_tmp12D4;(_tmp12D4=0,Cyc_Tcutil_warn(loc,((_tmp12D5="zero terminated array cannot have zero elements",_tag_dyneither(_tmp12D5,sizeof(char),48))),_tag_dyneither(_tmp12D4,sizeof(void*),0)));}
# 4236
if((_tmpAD0  && _tmpACF < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp12D8;void*_tmp12D7;(_tmp12D7=0,Cyc_Tcutil_warn(loc,((_tmp12D8="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp12D8,sizeof(char),75))),_tag_dyneither(_tmp12D7,sizeof(void*),0)));}
*_tmp9ED=Cyc_Absyn_uint_exp(1,0);}
# 4240
_npop_handler(0);goto _LL5A6;};
# 4220
;_pop_region(temp);};};}_LL5C3: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9F0=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9C8;if(_tmp9F0->tag != 9)goto _LL5C5;else{_tmp9F1=(struct Cyc_List_List**)&(_tmp9F0->f1).tvars;_tmp9F2=(void**)&(_tmp9F0->f1).effect;_tmp9F3=(struct Cyc_Absyn_Tqual*)&(_tmp9F0->f1).ret_tqual;_tmp9F4=(_tmp9F0->f1).ret_typ;_tmp9F5=(_tmp9F0->f1).args;_tmp9F6=(_tmp9F0->f1).c_varargs;_tmp9F7=(_tmp9F0->f1).cyc_varargs;_tmp9F8=(_tmp9F0->f1).rgn_po;_tmp9F9=(_tmp9F0->f1).attributes;_tmp9FA=(_tmp9F0->f1).requires_clause;_tmp9FB=(struct Cyc_List_List**)&(_tmp9F0->f1).requires_relns;_tmp9FC=(_tmp9F0->f1).ensures_clause;_tmp9FD=(struct Cyc_List_List**)&(_tmp9F0->f1).ensures_relns;}}_LL5C4: {
# 4247
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp9F9 != 0;_tmp9F9=_tmp9F9->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp9F9->hd)){
const char*_tmp12DC;void*_tmp12DB[1];struct Cyc_String_pa_PrintArg_struct _tmp12DA;(_tmp12DA.tag=0,((_tmp12DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp9F9->hd)),((_tmp12DB[0]=& _tmp12DA,Cyc_Tcutil_terr(loc,((_tmp12DC="bad function type attribute %s",_tag_dyneither(_tmp12DC,sizeof(char),31))),_tag_dyneither(_tmp12DB,sizeof(void*),1)))))));}{
void*_tmpAD8=(void*)_tmp9F9->hd;void*_tmpAD9=_tmpAD8;enum Cyc_Absyn_Format_Type _tmpADE;int _tmpADF;int _tmpAE0;_LL63D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpADA=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAD9;if(_tmpADA->tag != 1)goto _LL63F;}_LL63E:
# 4260
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL63C;_LL63F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpADB=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAD9;if(_tmpADB->tag != 2)goto _LL641;}_LL640:
# 4262
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL63C;_LL641: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpADC=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAD9;if(_tmpADC->tag != 3)goto _LL643;}_LL642:
# 4264
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL63C;_LL643: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpADD=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpAD9;if(_tmpADD->tag != 19)goto _LL645;else{_tmpADE=_tmpADD->f1;_tmpADF=_tmpADD->f2;_tmpAE0=_tmpADD->f3;}}_LL644:
# 4266
 if(!seen_format){
seen_format=1;
ft=_tmpADE;
fmt_desc_arg=_tmpADF;
fmt_arg_start=_tmpAE0;}else{
# 4272
const char*_tmp12DF;void*_tmp12DE;(_tmp12DE=0,Cyc_Tcutil_terr(loc,((_tmp12DF="function can't have multiple format attributes",_tag_dyneither(_tmp12DF,sizeof(char),47))),_tag_dyneither(_tmp12DE,sizeof(void*),0)));}
goto _LL63C;_LL645:;_LL646:
 goto _LL63C;_LL63C:;};}
# 4277
if(num_convs > 1){
const char*_tmp12E2;void*_tmp12E1;(_tmp12E1=0,Cyc_Tcutil_terr(loc,((_tmp12E2="function can't have multiple calling conventions",_tag_dyneither(_tmp12E2,sizeof(char),49))),_tag_dyneither(_tmp12E1,sizeof(void*),0)));}
# 4282
Cyc_Tcutil_check_unique_tvars(loc,*_tmp9F1);
{struct Cyc_List_List*b=*_tmp9F1;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpAE5=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpAE6=_tmpAE5;_LL648: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAE7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE6;if(_tmpAE7->tag != 0)goto _LL64A;else{if((_tmpAE7->f1)->kind != Cyc_Absyn_MemKind)goto _LL64A;}}_LL649:
# 4288
{const char*_tmp12E6;void*_tmp12E5[1];struct Cyc_String_pa_PrintArg_struct _tmp12E4;(_tmp12E4.tag=0,((_tmp12E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp12E5[0]=& _tmp12E4,Cyc_Tcutil_terr(loc,((_tmp12E6="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp12E6,sizeof(char),51))),_tag_dyneither(_tmp12E5,sizeof(void*),1)))))));}
goto _LL647;_LL64A:;_LL64B:
 goto _LL647;_LL647:;};}}{
# 4296
struct _RegionHandle _tmpAEB=_new_region("newr");struct _RegionHandle*newr=& _tmpAEB;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp12E7;struct Cyc_Tcutil_CVTEnv _tmpAEC=
(_tmp12E7.r=newr,((_tmp12E7.kind_env=cvtenv.kind_env,((_tmp12E7.free_vars=0,((_tmp12E7.free_evars=0,((_tmp12E7.generalize_evars=cvtenv.generalize_evars,((_tmp12E7.fn_result=1,_tmp12E7)))))))))));
# 4302
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_tmk,_tmp9F4,1);
_tmp9F3->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9F3->print_const,_tmp9F4);
_tmpAEC.fn_result=0;
{struct Cyc_List_List*a=_tmp9F5;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpAED=(struct _tuple10*)a->hd;
void*_tmpAEE=(*_tmpAED).f3;
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_tmk,_tmpAEE,1);{
int _tmpAEF=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAED).f2).print_const,_tmpAEE);
((*_tmpAED).f2).real_const=_tmpAEF;
# 4313
if(Cyc_Tcutil_is_array(_tmpAEE)){
# 4316
void*_tmpAF0=Cyc_Absyn_new_evar(0,0);
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_rk,_tmpAF0,1);
(*_tmpAED).f3=Cyc_Tcutil_promote_array(_tmpAEE,_tmpAF0,0);}};}}
# 4323
if(_tmp9F7 != 0){
if(_tmp9F6){const char*_tmp12EA;void*_tmp12E9;(_tmp12E9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp12EA="both c_vararg and cyc_vararg",_tag_dyneither(_tmp12EA,sizeof(char),29))),_tag_dyneither(_tmp12E9,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpAF3=*_tmp9F7;void*_tmpAF5;int _tmpAF6;struct Cyc_Absyn_VarargInfo _tmpAF4=_tmpAF3;_tmpAF5=_tmpAF4.type;_tmpAF6=_tmpAF4.inject;
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_tmk,_tmpAF5,1);
(_tmp9F7->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp9F7->tq).print_const,_tmpAF5);
# 4329
if(_tmpAF6){
void*_tmpAF7=Cyc_Tcutil_compress(_tmpAF5);void*_tmpAF8=_tmpAF7;void*_tmpAFA;union Cyc_Absyn_Constraint*_tmpAFB;union Cyc_Absyn_Constraint*_tmpAFC;_LL64D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAF9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF8;if(_tmpAF9->tag != 5)goto _LL64F;else{_tmpAFA=(_tmpAF9->f1).elt_typ;_tmpAFB=((_tmpAF9->f1).ptr_atts).bounds;_tmpAFC=((_tmpAF9->f1).ptr_atts).zero_term;}}_LL64E:
# 4332
{void*_tmpAFD=Cyc_Tcutil_compress(_tmpAFA);void*_tmpAFE=_tmpAFD;_LL652: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAFF=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAFE;if(_tmpAFF->tag != 3)goto _LL654;}_LL653:
# 4334
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpAFC)){
const char*_tmp12ED;void*_tmp12EC;(_tmp12EC=0,Cyc_Tcutil_terr(loc,((_tmp12ED="can't inject into a zeroterm pointer",_tag_dyneither(_tmp12ED,sizeof(char),37))),_tag_dyneither(_tmp12EC,sizeof(void*),0)));}
{void*_tmpB02=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpAFB);void*_tmpB03=_tmpB02;_LL657: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB04=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB03;if(_tmpB04->tag != 0)goto _LL659;}_LL658:
# 4338
{const char*_tmp12F0;void*_tmp12EF;(_tmp12EF=0,Cyc_Tcutil_terr(loc,((_tmp12F0="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp12F0,sizeof(char),44))),_tag_dyneither(_tmp12EF,sizeof(void*),0)));}
goto _LL656;_LL659:;_LL65A:
 goto _LL656;_LL656:;}
# 4342
goto _LL651;_LL654:;_LL655:
{const char*_tmp12F3;void*_tmp12F2;(_tmp12F2=0,Cyc_Tcutil_terr(loc,((_tmp12F3="can't inject a non-datatype type",_tag_dyneither(_tmp12F3,sizeof(char),33))),_tag_dyneither(_tmp12F2,sizeof(void*),0)));}goto _LL651;_LL651:;}
# 4345
goto _LL64C;_LL64F:;_LL650:
{const char*_tmp12F6;void*_tmp12F5;(_tmp12F5=0,Cyc_Tcutil_terr(loc,((_tmp12F6="expecting a datatype pointer type",_tag_dyneither(_tmp12F6,sizeof(char),34))),_tag_dyneither(_tmp12F5,sizeof(void*),0)));}goto _LL64C;_LL64C:;}};}
# 4351
if(seen_format){
int _tmpB0B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F5);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpB0B) || fmt_arg_start < 0) || 
# 4355
_tmp9F7 == 0  && fmt_arg_start != 0) || 
_tmp9F7 != 0  && fmt_arg_start != _tmpB0B + 1){
const char*_tmp12F9;void*_tmp12F8;(_tmp12F8=0,Cyc_Tcutil_terr(loc,((_tmp12F9="bad format descriptor",_tag_dyneither(_tmp12F9,sizeof(char),22))),_tag_dyneither(_tmp12F8,sizeof(void*),0)));}else{
# 4360
struct _tuple10 _tmpB0E=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9F5,fmt_desc_arg - 1);void*_tmpB10;struct _tuple10 _tmpB0F=_tmpB0E;_tmpB10=_tmpB0F.f3;
# 4362
{void*_tmpB11=Cyc_Tcutil_compress(_tmpB10);void*_tmpB12=_tmpB11;void*_tmpB14;union Cyc_Absyn_Constraint*_tmpB15;union Cyc_Absyn_Constraint*_tmpB16;_LL65C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB13=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB12;if(_tmpB13->tag != 5)goto _LL65E;else{_tmpB14=(_tmpB13->f1).elt_typ;_tmpB15=((_tmpB13->f1).ptr_atts).bounds;_tmpB16=((_tmpB13->f1).ptr_atts).zero_term;}}_LL65D:
# 4365
{struct _tuple0 _tmp12FA;struct _tuple0 _tmpB17=(_tmp12FA.f1=Cyc_Tcutil_compress(_tmpB14),((_tmp12FA.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB15),_tmp12FA)));struct _tuple0 _tmpB18=_tmpB17;_LL661:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpB19=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB18.f1;if(_tmpB19->tag != 6)goto _LL663;else{if(_tmpB19->f1 != Cyc_Absyn_None)goto _LL663;if(_tmpB19->f2 != Cyc_Absyn_Char_sz)goto _LL663;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpB1A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB18.f2;if(_tmpB1A->tag != 0)goto _LL663;};_LL662:
 goto _LL660;_LL663:;_LL664:
{const char*_tmp12FD;void*_tmp12FC;(_tmp12FC=0,Cyc_Tcutil_terr(loc,((_tmp12FD="format descriptor is not a char ? type",_tag_dyneither(_tmp12FD,sizeof(char),39))),_tag_dyneither(_tmp12FC,sizeof(void*),0)));}goto _LL660;_LL660:;}
# 4369
goto _LL65B;_LL65E:;_LL65F:
{const char*_tmp1300;void*_tmp12FF;(_tmp12FF=0,Cyc_Tcutil_terr(loc,((_tmp1300="format descriptor is not a char ? type",_tag_dyneither(_tmp1300,sizeof(char),39))),_tag_dyneither(_tmp12FF,sizeof(void*),0)));}goto _LL65B;_LL65B:;}
# 4372
if(fmt_arg_start != 0){
# 4376
int problem;
{struct _tuple31 _tmp1301;struct _tuple31 _tmpB20=(_tmp1301.f1=ft,((_tmp1301.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9F7))->type)),_tmp1301)));struct _tuple31 _tmpB21=_tmpB20;struct Cyc_Absyn_Datatypedecl*_tmpB23;struct Cyc_Absyn_Datatypedecl*_tmpB25;_LL666: if(_tmpB21.f1 != Cyc_Absyn_Printf_ft)goto _LL668;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB22=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB21.f2;if(_tmpB22->tag != 3)goto _LL668;else{if((((_tmpB22->f1).datatype_info).KnownDatatype).tag != 2)goto _LL668;_tmpB23=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB22->f1).datatype_info).KnownDatatype).val);}};_LL667:
# 4379
 problem=Cyc_Absyn_qvar_cmp(_tmpB23->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL665;_LL668: if(_tmpB21.f1 != Cyc_Absyn_Scanf_ft)goto _LL66A;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpB24=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpB21.f2;if(_tmpB24->tag != 3)goto _LL66A;else{if((((_tmpB24->f1).datatype_info).KnownDatatype).tag != 2)goto _LL66A;_tmpB25=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpB24->f1).datatype_info).KnownDatatype).val);}};_LL669:
# 4381
 problem=Cyc_Absyn_qvar_cmp(_tmpB25->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL665;_LL66A:;_LL66B:
# 4383
 problem=1;goto _LL665;_LL665:;}
# 4385
if(problem){
const char*_tmp1304;void*_tmp1303;(_tmp1303=0,Cyc_Tcutil_terr(loc,((_tmp1304="format attribute and vararg types don't match",_tag_dyneither(_tmp1304,sizeof(char),46))),_tag_dyneither(_tmp1303,sizeof(void*),0)));}}}}
# 4393
{struct Cyc_List_List*rpo=_tmp9F8;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpB29=(struct _tuple0*)rpo->hd;void*_tmpB2B;void*_tmpB2C;struct _tuple0*_tmpB2A=_tmpB29;_tmpB2B=_tmpB2A->f1;_tmpB2C=_tmpB2A->f2;
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_ek,_tmpB2B,1);
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_trk,_tmpB2C,1);}}{
# 4401
struct Cyc_Tcenv_Fenv*_tmpB2D=Cyc_Tcenv_bogus_fenv(newr,_tmp9F4,_tmp9F5);
struct Cyc_Tcenv_Tenv*_tmp1305;struct Cyc_Tcenv_Tenv*_tmpB2E=(_tmp1305=_region_malloc(newr,sizeof(*_tmp1305)),((_tmp1305->ns=te->ns,((_tmp1305->ae=te->ae,((_tmp1305->le=_tmpB2D,((_tmp1305->allow_valueof=1,_tmp1305)))))))));
const char*_tmp1306;struct _tuple30 _tmpB2F=Cyc_Tcutil_check_clause(loc,_tmpB2E,_tmpAEC,((_tmp1306="@requires",_tag_dyneither(_tmp1306,sizeof(char),10))),_tmp9FA);struct Cyc_Tcutil_CVTEnv _tmpB31;struct Cyc_List_List*_tmpB32;struct _tuple30 _tmpB30=_tmpB2F;_tmpB31=_tmpB30.f1;_tmpB32=_tmpB30.f2;
_tmpAEC=_tmpB31;
*_tmp9FB=_tmpB32;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp9F5,_tmpB32);{
# 4408
struct _dyneither_ptr*_tmpB33=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct _tuple2*_tmp1309;struct Cyc_Absyn_Vardecl*_tmp1308;struct Cyc_Absyn_Vardecl*_tmpB34=(_tmp1308=_cycalloc(sizeof(*_tmp1308)),((_tmp1308->sc=Cyc_Absyn_Public,((_tmp1308->name=((_tmp1309=_cycalloc(sizeof(*_tmp1309)),((_tmp1309->f1=Cyc_Absyn_Loc_n,((_tmp1309->f2=_tmpB33,_tmp1309)))))),((_tmp1308->tq=
Cyc_Absyn_empty_tqual(0),((_tmp1308->type=_tmp9F4,((_tmp1308->initializer=0,((_tmp1308->rgn=
Cyc_Tcenv_curr_rgn(_tmpB2E),((_tmp1308->attributes=0,((_tmp1308->escapes=0,_tmp1308)))))))))))))))));
# 4413
_tmpB2E=Cyc_Tcenv_add_local_var(newr,0,_tmpB2E,_tmpB34);{
const char*_tmp130A;struct _tuple30 _tmpB35=Cyc_Tcutil_check_clause(loc,_tmpB2E,_tmpAEC,((_tmp130A="@ensures",_tag_dyneither(_tmp130A,sizeof(char),9))),_tmp9FC);struct Cyc_Tcutil_CVTEnv _tmpB37;struct Cyc_List_List*_tmpB38;struct _tuple30 _tmpB36=_tmpB35;_tmpB37=_tmpB36.f1;_tmpB38=_tmpB36.f2;
_tmpAEC=_tmpB37;
*_tmp9FD=_tmpB38;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp9F5,_tmpB38);
# 4419
if(*_tmp9F2 != 0)
_tmpAEC=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAEC,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp9F2),1);else{
# 4422
struct Cyc_List_List*effect=0;
# 4427
{struct Cyc_List_List*tvs=_tmpAEC.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB39=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB3B;int _tmpB3C;struct _tuple28 _tmpB3A=_tmpB39;_tmpB3B=_tmpB3A.f1;_tmpB3C=_tmpB3A.f2;
if(!_tmpB3C)continue;{
void*_tmpB3D=Cyc_Absyn_compress_kb(_tmpB3B->kind);void*_tmpB3E=_tmpB3D;struct Cyc_Core_Opt**_tmpB40;struct Cyc_Absyn_Kind*_tmpB41;struct Cyc_Absyn_Kind*_tmpB43;struct Cyc_Core_Opt**_tmpB47;struct Cyc_Core_Opt**_tmpB4A;_LL66D:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB3F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB3F->tag != 2)goto _LL66F;else{_tmpB40=(struct Cyc_Core_Opt**)& _tmpB3F->f1;_tmpB41=_tmpB3F->f2;}}if(!(_tmpB41->kind == Cyc_Absyn_RgnKind))goto _LL66F;_LL66E:
# 4433
 if(_tmpB41->aliasqual == Cyc_Absyn_Top){
*_tmpB40=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpB43=_tmpB41;goto _LL670;}
# 4436
*_tmpB40=Cyc_Tcutil_kind_to_bound_opt(_tmpB41);_tmpB43=_tmpB41;goto _LL670;_LL66F:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB42=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB42->tag != 0)goto _LL671;else{_tmpB43=_tmpB42->f1;}}if(!(_tmpB43->kind == Cyc_Absyn_RgnKind))goto _LL671;_LL670:
# 4438
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1319;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1318;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1317;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1316;struct Cyc_List_List*_tmp1315;effect=((_tmp1315=_cycalloc(sizeof(*_tmp1315)),((_tmp1315->hd=(void*)((_tmp1319=_cycalloc(sizeof(*_tmp1319)),((_tmp1319[0]=((_tmp1316.tag=23,((_tmp1316.f1=(void*)((_tmp1318=_cycalloc(sizeof(*_tmp1318)),((_tmp1318[0]=((_tmp1317.tag=2,((_tmp1317.f1=_tmpB3B,_tmp1317)))),_tmp1318)))),_tmp1316)))),_tmp1319)))),((_tmp1315->tl=effect,_tmp1315))))));}goto _LL66C;_LL671: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB44=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB44->tag != 2)goto _LL673;else{if((_tmpB44->f2)->kind != Cyc_Absyn_IntKind)goto _LL673;}}_LL672:
 goto _LL674;_LL673: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB45=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB45->tag != 0)goto _LL675;else{if((_tmpB45->f1)->kind != Cyc_Absyn_IntKind)goto _LL675;}}_LL674:
 goto _LL66C;_LL675: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB46=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB46->tag != 2)goto _LL677;else{_tmpB47=(struct Cyc_Core_Opt**)& _tmpB46->f1;if((_tmpB46->f2)->kind != Cyc_Absyn_EffKind)goto _LL677;}}_LL676:
# 4442
*_tmpB47=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL678;_LL677: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB48=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB48->tag != 0)goto _LL679;else{if((_tmpB48->f1)->kind != Cyc_Absyn_EffKind)goto _LL679;}}_LL678:
# 4444
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp131F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp131E;struct Cyc_List_List*_tmp131D;effect=((_tmp131D=_cycalloc(sizeof(*_tmp131D)),((_tmp131D->hd=(void*)((_tmp131F=_cycalloc(sizeof(*_tmp131F)),((_tmp131F[0]=((_tmp131E.tag=2,((_tmp131E.f1=_tmpB3B,_tmp131E)))),_tmp131F)))),((_tmp131D->tl=effect,_tmp131D))))));}goto _LL66C;_LL679: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB49=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB3E;if(_tmpB49->tag != 1)goto _LL67B;else{_tmpB4A=(struct Cyc_Core_Opt**)& _tmpB49->f1;}}_LL67A:
# 4446
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1325;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1324;struct Cyc_Core_Opt*_tmp1323;*_tmpB4A=((_tmp1323=_cycalloc(sizeof(*_tmp1323)),((_tmp1323->v=(void*)((_tmp1325=_cycalloc(sizeof(*_tmp1325)),((_tmp1325[0]=((_tmp1324.tag=2,((_tmp1324.f1=0,((_tmp1324.f2=& Cyc_Tcutil_ak,_tmp1324)))))),_tmp1325)))),_tmp1323))));}goto _LL67C;_LL67B:;_LL67C:
# 4449
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1334;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1333;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1332;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1331;struct Cyc_List_List*_tmp1330;effect=((_tmp1330=_cycalloc(sizeof(*_tmp1330)),((_tmp1330->hd=(void*)((_tmp1334=_cycalloc(sizeof(*_tmp1334)),((_tmp1334[0]=((_tmp1331.tag=25,((_tmp1331.f1=(void*)((_tmp1333=_cycalloc(sizeof(*_tmp1333)),((_tmp1333[0]=((_tmp1332.tag=2,((_tmp1332.f1=_tmpB3B,_tmp1332)))),_tmp1333)))),_tmp1331)))),_tmp1334)))),((_tmp1330->tl=effect,_tmp1330))))));}goto _LL66C;_LL66C:;};}}
# 4453
{struct Cyc_List_List*ts=_tmpAEC.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB5B=*((struct _tuple29*)ts->hd);void*_tmpB5D;int _tmpB5E;struct _tuple29 _tmpB5C=_tmpB5B;_tmpB5D=_tmpB5C.f1;_tmpB5E=_tmpB5C.f2;
if(!_tmpB5E)continue;{
struct Cyc_Absyn_Kind*_tmpB5F=Cyc_Tcutil_typ_kind(_tmpB5D);struct Cyc_Absyn_Kind*_tmpB60=_tmpB5F;_LL67E: if(_tmpB60->kind != Cyc_Absyn_RgnKind)goto _LL680;_LL67F:
# 4458
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp133A;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp1339;struct Cyc_List_List*_tmp1338;effect=((_tmp1338=_cycalloc(sizeof(*_tmp1338)),((_tmp1338->hd=(void*)((_tmp133A=_cycalloc(sizeof(*_tmp133A)),((_tmp133A[0]=((_tmp1339.tag=23,((_tmp1339.f1=_tmpB5D,_tmp1339)))),_tmp133A)))),((_tmp1338->tl=effect,_tmp1338))))));}goto _LL67D;_LL680: if(_tmpB60->kind != Cyc_Absyn_EffKind)goto _LL682;_LL681:
# 4460
{struct Cyc_List_List*_tmp133B;effect=((_tmp133B=_cycalloc(sizeof(*_tmp133B)),((_tmp133B->hd=_tmpB5D,((_tmp133B->tl=effect,_tmp133B))))));}goto _LL67D;_LL682: if(_tmpB60->kind != Cyc_Absyn_IntKind)goto _LL684;_LL683:
 goto _LL67D;_LL684:;_LL685:
# 4463
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1341;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp1340;struct Cyc_List_List*_tmp133F;effect=((_tmp133F=_cycalloc(sizeof(*_tmp133F)),((_tmp133F->hd=(void*)((_tmp1341=_cycalloc(sizeof(*_tmp1341)),((_tmp1341[0]=((_tmp1340.tag=25,((_tmp1340.f1=_tmpB5D,_tmp1340)))),_tmp1341)))),((_tmp133F->tl=effect,_tmp133F))))));}goto _LL67D;_LL67D:;};}}{
# 4466
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1344;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1343;*_tmp9F2=(void*)((_tmp1343=_cycalloc(sizeof(*_tmp1343)),((_tmp1343[0]=((_tmp1344.tag=24,((_tmp1344.f1=effect,_tmp1344)))),_tmp1343))));};}
# 4472
if(*_tmp9F1 != 0){
struct Cyc_List_List*bs=*_tmp9F1;for(0;bs != 0;bs=bs->tl){
void*_tmpB6A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB6B=_tmpB6A;struct Cyc_Core_Opt**_tmpB6D;struct Cyc_Core_Opt**_tmpB6F;struct Cyc_Core_Opt**_tmpB71;struct Cyc_Core_Opt**_tmpB73;struct Cyc_Core_Opt**_tmpB75;struct Cyc_Core_Opt**_tmpB77;struct Cyc_Core_Opt**_tmpB79;struct Cyc_Core_Opt**_tmpB7B;struct Cyc_Core_Opt**_tmpB7D;struct Cyc_Core_Opt**_tmpB7F;struct Cyc_Absyn_Kind*_tmpB80;_LL687: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB6C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB6C->tag != 1)goto _LL689;else{_tmpB6D=(struct Cyc_Core_Opt**)& _tmpB6C->f1;}}_LL688:
# 4476
{const char*_tmp1348;void*_tmp1347[1];struct Cyc_String_pa_PrintArg_struct _tmp1346;(_tmp1346.tag=0,((_tmp1346.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp1347[0]=& _tmp1346,Cyc_Tcutil_warn(loc,((_tmp1348="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp1348,sizeof(char),47))),_tag_dyneither(_tmp1347,sizeof(void*),1)))))));}
# 4478
_tmpB6F=_tmpB6D;goto _LL68A;_LL689: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB6E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB6E->tag != 2)goto _LL68B;else{_tmpB6F=(struct Cyc_Core_Opt**)& _tmpB6E->f1;if((_tmpB6E->f2)->kind != Cyc_Absyn_BoxKind)goto _LL68B;if((_tmpB6E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL68B;}}_LL68A:
 _tmpB71=_tmpB6F;goto _LL68C;_LL68B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB70=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB70->tag != 2)goto _LL68D;else{_tmpB71=(struct Cyc_Core_Opt**)& _tmpB70->f1;if((_tmpB70->f2)->kind != Cyc_Absyn_MemKind)goto _LL68D;if((_tmpB70->f2)->aliasqual != Cyc_Absyn_Top)goto _LL68D;}}_LL68C:
 _tmpB73=_tmpB71;goto _LL68E;_LL68D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB72=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB72->tag != 2)goto _LL68F;else{_tmpB73=(struct Cyc_Core_Opt**)& _tmpB72->f1;if((_tmpB72->f2)->kind != Cyc_Absyn_MemKind)goto _LL68F;if((_tmpB72->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL68F;}}_LL68E:
 _tmpB75=_tmpB73;goto _LL690;_LL68F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB74=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB74->tag != 2)goto _LL691;else{_tmpB75=(struct Cyc_Core_Opt**)& _tmpB74->f1;if((_tmpB74->f2)->kind != Cyc_Absyn_AnyKind)goto _LL691;if((_tmpB74->f2)->aliasqual != Cyc_Absyn_Top)goto _LL691;}}_LL690:
 _tmpB77=_tmpB75;goto _LL692;_LL691: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB76=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB76->tag != 2)goto _LL693;else{_tmpB77=(struct Cyc_Core_Opt**)& _tmpB76->f1;if((_tmpB76->f2)->kind != Cyc_Absyn_AnyKind)goto _LL693;if((_tmpB76->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL693;}}_LL692:
# 4484
*_tmpB77=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL686;_LL693: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB78=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB78->tag != 2)goto _LL695;else{_tmpB79=(struct Cyc_Core_Opt**)& _tmpB78->f1;if((_tmpB78->f2)->kind != Cyc_Absyn_MemKind)goto _LL695;if((_tmpB78->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL695;}}_LL694:
 _tmpB7B=_tmpB79;goto _LL696;_LL695: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB7A->tag != 2)goto _LL697;else{_tmpB7B=(struct Cyc_Core_Opt**)& _tmpB7A->f1;if((_tmpB7A->f2)->kind != Cyc_Absyn_AnyKind)goto _LL697;if((_tmpB7A->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL697;}}_LL696:
# 4487
*_tmpB7B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL686;_LL697: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB7C->tag != 2)goto _LL699;else{_tmpB7D=(struct Cyc_Core_Opt**)& _tmpB7C->f1;if((_tmpB7C->f2)->kind != Cyc_Absyn_RgnKind)goto _LL699;if((_tmpB7C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL699;}}_LL698:
# 4489
*_tmpB7D=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL686;_LL699: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB7E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB7E->tag != 2)goto _LL69B;else{_tmpB7F=(struct Cyc_Core_Opt**)& _tmpB7E->f1;_tmpB80=_tmpB7E->f2;}}_LL69A:
# 4491
*_tmpB7F=Cyc_Tcutil_kind_to_bound_opt(_tmpB80);goto _LL686;_LL69B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB6B;if(_tmpB81->tag != 0)goto _LL69D;else{if((_tmpB81->f1)->kind != Cyc_Absyn_MemKind)goto _LL69D;}}_LL69C:
# 4493
{const char*_tmp134B;void*_tmp134A;(_tmp134A=0,Cyc_Tcutil_terr(loc,((_tmp134B="functions can't abstract M types",_tag_dyneither(_tmp134B,sizeof(char),33))),_tag_dyneither(_tmp134A,sizeof(void*),0)));}goto _LL686;_LL69D:;_LL69E:
 goto _LL686;_LL686:;}}
# 4498
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpAEC.kind_env,*_tmp9F1);
_tmpAEC.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpAEC.r,_tmpAEC.free_vars,*_tmp9F1);
# 4501
{struct Cyc_List_List*tvs=_tmpAEC.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB87=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB89;int _tmpB8A;struct _tuple28 _tmpB88=_tmpB87;_tmpB89=_tmpB88.f1;_tmpB8A=_tmpB88.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB89,_tmpB8A);}}{
# 4506
struct Cyc_List_List*evs=_tmpAEC.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpB8B=*((struct _tuple29*)evs->hd);void*_tmpB8D;int _tmpB8E;struct _tuple29 _tmpB8C=_tmpB8B;_tmpB8D=_tmpB8C.f1;_tmpB8E=_tmpB8C.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpB8D,_tmpB8E);}};};};};}
# 4511
_npop_handler(0);goto _LL5A6;
# 4296
;_pop_region(newr);};}_LL5C5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9FE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9C8;if(_tmp9FE->tag != 10)goto _LL5C7;else{_tmp9FF=_tmp9FE->f1;}}_LL5C6:
# 4514
 for(0;_tmp9FF != 0;_tmp9FF=_tmp9FF->tl){
struct _tuple12*_tmpB96=(struct _tuple12*)_tmp9FF->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpB96).f2,1);
((*_tmpB96).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB96).f1).print_const,(*_tmpB96).f2);}
# 4520
goto _LL5A6;_LL5C7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA00=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9C8;if(_tmpA00->tag != 12)goto _LL5C9;else{_tmpA01=_tmpA00->f1;_tmpA02=_tmpA00->f2;}}_LL5C8: {
# 4524
struct _RegionHandle _tmpB97=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpB97;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmpA02 != 0;_tmpA02=_tmpA02->tl){
struct Cyc_Absyn_Aggrfield*_tmpB98=(struct Cyc_Absyn_Aggrfield*)_tmpA02->hd;struct _dyneither_ptr*_tmpB9A;struct Cyc_Absyn_Tqual*_tmpB9B;void*_tmpB9C;struct Cyc_Absyn_Exp*_tmpB9D;struct Cyc_List_List*_tmpB9E;struct Cyc_Absyn_Exp*_tmpB9F;struct Cyc_Absyn_Aggrfield*_tmpB99=_tmpB98;_tmpB9A=_tmpB99->name;_tmpB9B=(struct Cyc_Absyn_Tqual*)& _tmpB99->tq;_tmpB9C=_tmpB99->type;_tmpB9D=_tmpB99->width;_tmpB9E=_tmpB99->attributes;_tmpB9F=_tmpB99->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB9A)){
const char*_tmp134F;void*_tmp134E[1];struct Cyc_String_pa_PrintArg_struct _tmp134D;(_tmp134D.tag=0,((_tmp134D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9A),((_tmp134E[0]=& _tmp134D,Cyc_Tcutil_terr(loc,((_tmp134F="duplicate field %s",_tag_dyneither(_tmp134F,sizeof(char),19))),_tag_dyneither(_tmp134E,sizeof(void*),1)))))));}
{const char*_tmp1350;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB9A,((_tmp1350="",_tag_dyneither(_tmp1350,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp1351;prev_fields=((_tmp1351=_region_malloc(aprev_rgn,sizeof(*_tmp1351)),((_tmp1351->hd=_tmpB9A,((_tmp1351->tl=prev_fields,_tmp1351))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB9C,1);
_tmpB9B->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB9B->print_const,_tmpB9C);
if((_tmpA01 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpB9C)) && _tmpB9F == 0){
# 4536
const char*_tmp1355;void*_tmp1354[1];struct Cyc_String_pa_PrintArg_struct _tmp1353;(_tmp1353.tag=0,((_tmp1353.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB9A),((_tmp1354[0]=& _tmp1353,Cyc_Tcutil_warn(loc,((_tmp1355="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp1355,sizeof(char),74))),_tag_dyneither(_tmp1354,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpB9C,_tmpB9D,_tmpB9A);
Cyc_Tcutil_check_field_atts(loc,_tmpB9A,_tmpB9E);
if(_tmpB9F != 0){
# 4541
if(_tmpA01 != Cyc_Absyn_UnionA){
const char*_tmp1358;void*_tmp1357;(_tmp1357=0,Cyc_Tcutil_terr(loc,((_tmp1358="@requires clause is only allowed on union members",_tag_dyneither(_tmp1358,sizeof(char),50))),_tag_dyneither(_tmp1357,sizeof(void*),0)));}{
struct _RegionHandle _tmpBAA=_new_region("temp");struct _RegionHandle*temp=& _tmpBAA;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpBAB=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpBAB,0,_tmpB9F);}
# 4547
if(!Cyc_Tcutil_is_integral(_tmpB9F)){
const char*_tmp135C;void*_tmp135B[1];struct Cyc_String_pa_PrintArg_struct _tmp135A;(_tmp135A.tag=0,((_tmp135A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB9F->topt))),((_tmp135B[0]=& _tmp135A,Cyc_Tcutil_terr(loc,((_tmp135C="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp135C,sizeof(char),54))),_tag_dyneither(_tmp135B,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB9F,te,cvtenv);
# 4543
;_pop_region(temp);};}}}
# 4554
_npop_handler(0);goto _LL5A6;
# 4524
;_pop_region(aprev_rgn);}_LL5C9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpA03=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9C8;if(_tmpA03->tag != 11)goto _LL5CB;else{_tmpA04=(union Cyc_Absyn_AggrInfoU*)&(_tmpA03->f1).aggr_info;_tmpA05=(struct Cyc_List_List**)&(_tmpA03->f1).targs;}}_LL5CA:
# 4557
{union Cyc_Absyn_AggrInfoU _tmpBAF=*_tmpA04;union Cyc_Absyn_AggrInfoU _tmpBB0=_tmpBAF;enum Cyc_Absyn_AggrKind _tmpBB1;struct _tuple2*_tmpBB2;struct Cyc_Core_Opt*_tmpBB3;struct Cyc_Absyn_Aggrdecl*_tmpBB4;_LL6A0: if((_tmpBB0.UnknownAggr).tag != 1)goto _LL6A2;_tmpBB1=((struct _tuple4)(_tmpBB0.UnknownAggr).val).f1;_tmpBB2=((struct _tuple4)(_tmpBB0.UnknownAggr).val).f2;_tmpBB3=((struct _tuple4)(_tmpBB0.UnknownAggr).val).f3;_LL6A1: {
# 4559
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpBB5;_push_handler(& _tmpBB5);{int _tmpBB7=0;if(setjmp(_tmpBB5.handler))_tmpBB7=1;if(!_tmpBB7){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBB2);{
struct Cyc_Absyn_Aggrdecl*_tmpBB8=*adp;
if(_tmpBB8->kind != _tmpBB1){
if(_tmpBB8->kind == Cyc_Absyn_StructA){
const char*_tmp1361;void*_tmp1360[2];struct Cyc_String_pa_PrintArg_struct _tmp135F;struct Cyc_String_pa_PrintArg_struct _tmp135E;(_tmp135E.tag=0,((_tmp135E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp135F.tag=0,((_tmp135F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4565
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1360[0]=& _tmp135F,((_tmp1360[1]=& _tmp135E,Cyc_Tcutil_terr(loc,((_tmp1361="expecting struct %s instead of union %s",_tag_dyneither(_tmp1361,sizeof(char),40))),_tag_dyneither(_tmp1360,sizeof(void*),2)))))))))))));}else{
# 4568
const char*_tmp1366;void*_tmp1365[2];struct Cyc_String_pa_PrintArg_struct _tmp1364;struct Cyc_String_pa_PrintArg_struct _tmp1363;(_tmp1363.tag=0,((_tmp1363.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1364.tag=0,((_tmp1364.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4568
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1365[0]=& _tmp1364,((_tmp1365[1]=& _tmp1363,Cyc_Tcutil_terr(loc,((_tmp1366="expecting union %s instead of struct %s",_tag_dyneither(_tmp1366,sizeof(char),40))),_tag_dyneither(_tmp1365,sizeof(void*),2)))))))))))));}}
# 4571
if((unsigned int)_tmpBB3  && (int)_tmpBB3->v){
if(!((unsigned int)_tmpBB8->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpBB8->impl))->tagged){
const char*_tmp136A;void*_tmp1369[1];struct Cyc_String_pa_PrintArg_struct _tmp1368;(_tmp1368.tag=0,((_tmp1368.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp1369[0]=& _tmp1368,Cyc_Tcutil_terr(loc,((_tmp136A="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp136A,sizeof(char),42))),_tag_dyneither(_tmp1369,sizeof(void*),1)))))));}}
# 4577
*_tmpA04=Cyc_Absyn_KnownAggr(adp);};
# 4561
;_pop_handler();}else{void*_tmpBB6=(void*)_exn_thrown;void*_tmpBC5=_tmpBB6;void*_tmpBC7;_LL6A5: {struct Cyc_Dict_Absent_exn_struct*_tmpBC6=(struct Cyc_Dict_Absent_exn_struct*)_tmpBC5;if(_tmpBC6->tag != Cyc_Dict_Absent)goto _LL6A7;}_LL6A6: {
# 4581
struct Cyc_Tcenv_Genv*_tmpBC8=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp136B;struct Cyc_Absyn_Aggrdecl*_tmpBC9=(_tmp136B=_cycalloc(sizeof(*_tmp136B)),((_tmp136B->kind=_tmpBB1,((_tmp136B->sc=Cyc_Absyn_Extern,((_tmp136B->name=_tmpBB2,((_tmp136B->tvs=0,((_tmp136B->impl=0,((_tmp136B->attributes=0,_tmp136B)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpBC8,loc,_tmpBC9);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpBB2);
*_tmpA04=Cyc_Absyn_KnownAggr(adp);
# 4587
if(*_tmpA05 != 0){
{const char*_tmp136F;void*_tmp136E[1];struct Cyc_String_pa_PrintArg_struct _tmp136D;(_tmp136D.tag=0,((_tmp136D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpBB2)),((_tmp136E[0]=& _tmp136D,Cyc_Tcutil_terr(loc,((_tmp136F="declare parameterized type %s before using",_tag_dyneither(_tmp136F,sizeof(char),43))),_tag_dyneither(_tmp136E,sizeof(void*),1)))))));}
return cvtenv;}
# 4592
goto _LL6A4;}_LL6A7: _tmpBC7=_tmpBC5;_LL6A8:(void)_throw(_tmpBC7);_LL6A4:;}};}
# 4594
_tmpBB4=*adp;goto _LL6A3;}_LL6A2: if((_tmpBB0.KnownAggr).tag != 2)goto _LL69F;_tmpBB4=*((struct Cyc_Absyn_Aggrdecl**)(_tmpBB0.KnownAggr).val);_LL6A3: {
# 4596
struct Cyc_List_List*tvs=_tmpBB4->tvs;
struct Cyc_List_List*ts=*_tmpA05;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpBCE=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpBCF=(void*)ts->hd;
# 4604
{struct _tuple0 _tmp1370;struct _tuple0 _tmpBD0=(_tmp1370.f1=Cyc_Absyn_compress_kb(_tmpBCE->kind),((_tmp1370.f2=_tmpBCF,_tmp1370)));struct _tuple0 _tmpBD1=_tmpBD0;struct Cyc_Absyn_Tvar*_tmpBD4;_LL6AA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBD2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBD1.f1;if(_tmpBD2->tag != 1)goto _LL6AC;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpBD1.f2;if(_tmpBD3->tag != 2)goto _LL6AC;else{_tmpBD4=_tmpBD3->f1;}};_LL6AB:
# 4606
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpBD4);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpBD4,1);
continue;_LL6AC:;_LL6AD:
 goto _LL6A9;_LL6A9:;}{
# 4611
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4615
if(ts != 0){
const char*_tmp1374;void*_tmp1373[1];struct Cyc_String_pa_PrintArg_struct _tmp1372;(_tmp1372.tag=0,((_tmp1372.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpBB4->name)),((_tmp1373[0]=& _tmp1372,Cyc_Tcutil_terr(loc,((_tmp1374="too many parameters for type %s",_tag_dyneither(_tmp1374,sizeof(char),32))),_tag_dyneither(_tmp1373,sizeof(void*),1)))))));}
if(tvs != 0){
# 4619
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1375;hidden_ts=((_tmp1375=_cycalloc(sizeof(*_tmp1375)),((_tmp1375->hd=e,((_tmp1375->tl=hidden_ts,_tmp1375))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4626
*_tmpA05=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmpA05,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL69F:;}
# 4629
goto _LL5A6;_LL5CB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA06=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8;if(_tmpA06->tag != 17)goto _LL5CD;else{_tmpA07=_tmpA06->f1;_tmpA08=(struct Cyc_List_List**)& _tmpA06->f2;_tmpA09=(struct Cyc_Absyn_Typedefdecl**)& _tmpA06->f3;_tmpA0A=(void**)((void**)& _tmpA06->f4);}}_LL5CC: {
# 4632
struct Cyc_List_List*targs=*_tmpA08;
# 4634
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpBDA;_push_handler(& _tmpBDA);{int _tmpBDC=0;if(setjmp(_tmpBDA.handler))_tmpBDC=1;if(!_tmpBDC){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmpA07);;_pop_handler();}else{void*_tmpBDB=(void*)_exn_thrown;void*_tmpBDE=_tmpBDB;void*_tmpBE0;_LL6AF: {struct Cyc_Dict_Absent_exn_struct*_tmpBDF=(struct Cyc_Dict_Absent_exn_struct*)_tmpBDE;if(_tmpBDF->tag != Cyc_Dict_Absent)goto _LL6B1;}_LL6B0:
# 4637
{const char*_tmp1379;void*_tmp1378[1];struct Cyc_String_pa_PrintArg_struct _tmp1377;(_tmp1377.tag=0,((_tmp1377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA07)),((_tmp1378[0]=& _tmp1377,Cyc_Tcutil_terr(loc,((_tmp1379="unbound typedef name %s",_tag_dyneither(_tmp1379,sizeof(char),24))),_tag_dyneither(_tmp1378,sizeof(void*),1)))))));}
return cvtenv;_LL6B1: _tmpBE0=_tmpBDE;_LL6B2:(void)_throw(_tmpBE0);_LL6AE:;}};}
# 4640
*_tmpA09=td;
# 4642
_tmpA07[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpBE4=_new_region("temp");struct _RegionHandle*temp=& _tmpBE4;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4648
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4653
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp137C;struct Cyc_List_List*_tmp137B;inst=((_tmp137B=_region_malloc(temp,sizeof(*_tmp137B)),((_tmp137B->hd=((_tmp137C=_region_malloc(temp,sizeof(*_tmp137C)),((_tmp137C->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp137C->f2=(void*)ts->hd,_tmp137C)))))),((_tmp137B->tl=inst,_tmp137B))))));};}
# 4657
if(ts != 0){
const char*_tmp1380;void*_tmp137F[1];struct Cyc_String_pa_PrintArg_struct _tmp137E;(_tmp137E.tag=0,((_tmp137E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA07)),((_tmp137F[0]=& _tmp137E,Cyc_Tcutil_terr(loc,((_tmp1380="too many parameters for typedef %s",_tag_dyneither(_tmp1380,sizeof(char),35))),_tag_dyneither(_tmp137F,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4662
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1381;hidden_ts=((_tmp1381=_cycalloc(sizeof(*_tmp1381)),((_tmp1381->hd=e,((_tmp1381->tl=hidden_ts,_tmp1381))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp1384;struct Cyc_List_List*_tmp1383;inst=((_tmp1383=_cycalloc(sizeof(*_tmp1383)),((_tmp1383->hd=((_tmp1384=_cycalloc(sizeof(*_tmp1384)),((_tmp1384->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1384->f2=e,_tmp1384)))))),((_tmp1383->tl=inst,_tmp1383))))));};}
# 4670
*_tmpA08=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4672
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmpA0A=new_typ;}}
# 4679
_npop_handler(0);goto _LL5A6;
# 4645
;_pop_region(temp);};}_LL5CD: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpA0B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp9C8;if(_tmpA0B->tag != 22)goto _LL5CF;}_LL5CE:
# 4680
 goto _LL5D0;_LL5CF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA0C=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp9C8;if(_tmpA0C->tag != 21)goto _LL5D1;}_LL5D0:
 goto _LL5D2;_LL5D1: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpA0D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp9C8;if(_tmpA0D->tag != 20)goto _LL5D3;}_LL5D2:
 goto _LL5A6;_LL5D3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA0E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp9C8;if(_tmpA0E->tag != 15)goto _LL5D5;else{_tmpA0F=(void*)_tmpA0E->f1;}}_LL5D4:
# 4684
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA0F,1);goto _LL5A6;_LL5D5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA10=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp9C8;if(_tmpA10->tag != 16)goto _LL5D7;else{_tmpA11=(void*)_tmpA10->f1;_tmpA12=(void*)_tmpA10->f2;}}_LL5D6:
# 4687
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA11,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmpA12,1);
goto _LL5A6;_LL5D7: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpA13=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp9C8;if(_tmpA13->tag != 23)goto _LL5D9;else{_tmpA14=(void*)_tmpA13->f1;}}_LL5D8:
# 4691
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmpA14,1);goto _LL5A6;_LL5D9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpA15=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp9C8;if(_tmpA15->tag != 25)goto _LL5DB;else{_tmpA16=(void*)_tmpA15->f1;}}_LL5DA:
# 4693
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpA16,1);goto _LL5A6;_LL5DB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpA17=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp9C8;if(_tmpA17->tag != 24)goto _LL5A6;else{_tmpA18=_tmpA17->f1;}}_LL5DC:
# 4695
 for(0;_tmpA18 != 0;_tmpA18=_tmpA18->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmpA18->hd,1);}
goto _LL5A6;_LL5A6:;}
# 4699
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpBED=t;struct Cyc_Core_Opt*_tmpBEF;void*_tmpBF0;_LL6B4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpBEE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpBED;if(_tmpBEE->tag != 1)goto _LL6B6;else{_tmpBEF=_tmpBEE->f1;_tmpBF0=(void*)_tmpBEE->f2;}}_LL6B5: {
# 4702
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpBF1=_tmpBEF;struct Cyc_Absyn_Kind*_tmpBF2;_LL6B9: if(_tmpBF1 != 0)goto _LL6BB;_LL6BA:
{const char*_tmp1385;s=((_tmp1385="kind=NULL ",_tag_dyneither(_tmp1385,sizeof(char),11)));}goto _LL6B8;_LL6BB: if(_tmpBF1 == 0)goto _LL6B8;_tmpBF2=(struct Cyc_Absyn_Kind*)_tmpBF1->v;_LL6BC:
{const char*_tmp1389;void*_tmp1388[1];struct Cyc_String_pa_PrintArg_struct _tmp1387;s=(struct _dyneither_ptr)((_tmp1387.tag=0,((_tmp1387.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpBF2)),((_tmp1388[0]=& _tmp1387,Cyc_aprintf(((_tmp1389="kind=%s ",_tag_dyneither(_tmp1389,sizeof(char),9))),_tag_dyneither(_tmp1388,sizeof(void*),1))))))));}goto _LL6B8;_LL6B8:;}
# 4707
if(_tmpBF0 == 0){
const char*_tmp138D;void*_tmp138C[1];struct Cyc_String_pa_PrintArg_struct _tmp138B;s=(struct _dyneither_ptr)((_tmp138B.tag=0,((_tmp138B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp138C[0]=& _tmp138B,Cyc_aprintf(((_tmp138D="%s ref=NULL",_tag_dyneither(_tmp138D,sizeof(char),12))),_tag_dyneither(_tmp138C,sizeof(void*),1))))))));}else{
# 4710
const char*_tmp1392;void*_tmp1391[2];struct Cyc_String_pa_PrintArg_struct _tmp1390;struct Cyc_String_pa_PrintArg_struct _tmp138F;s=(struct _dyneither_ptr)((_tmp138F.tag=0,((_tmp138F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpBF0)),((_tmp1390.tag=0,((_tmp1390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1391[0]=& _tmp1390,((_tmp1391[1]=& _tmp138F,Cyc_aprintf(((_tmp1392="%s ref=%s",_tag_dyneither(_tmp1392,sizeof(char),10))),_tag_dyneither(_tmp1391,sizeof(void*),2))))))))))))));}
# 4712
goto _LL6B3;}_LL6B6:;_LL6B7:
 goto _LL6B3;_LL6B3:;}{
# 4715
const char*_tmp1398;void*_tmp1397[3];struct Cyc_String_pa_PrintArg_struct _tmp1396;struct Cyc_String_pa_PrintArg_struct _tmp1395;struct Cyc_String_pa_PrintArg_struct _tmp1394;(_tmp1394.tag=0,((_tmp1394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1395.tag=0,((_tmp1395.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1396.tag=0,((_tmp1396.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1397[0]=& _tmp1396,((_tmp1397[1]=& _tmp1395,((_tmp1397[2]=& _tmp1394,Cyc_Tcutil_terr(loc,((_tmp1398="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1398,sizeof(char),51))),_tag_dyneither(_tmp1397,sizeof(void*),3)))))))))))))))))));};}
# 4718
return cvtenv;}
# 4726
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4728
{void*_tmpC03=e->r;void*_tmpC04=_tmpC03;struct Cyc_List_List*_tmpC0A;struct Cyc_Absyn_Exp*_tmpC0C;struct Cyc_Absyn_Exp*_tmpC0D;struct Cyc_Absyn_Exp*_tmpC0E;struct Cyc_Absyn_Exp*_tmpC10;struct Cyc_Absyn_Exp*_tmpC11;struct Cyc_Absyn_Exp*_tmpC13;struct Cyc_Absyn_Exp*_tmpC14;struct Cyc_Absyn_Exp*_tmpC16;struct Cyc_Absyn_Exp*_tmpC17;void*_tmpC19;struct Cyc_Absyn_Exp*_tmpC1A;void*_tmpC1C;void*_tmpC1E;void*_tmpC20;struct Cyc_Absyn_Exp*_tmpC22;_LL6BE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpC05=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC05->tag != 0)goto _LL6C0;}_LL6BF:
 goto _LL6C1;_LL6C0: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpC06=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC06->tag != 31)goto _LL6C2;}_LL6C1:
 goto _LL6C3;_LL6C2: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC07=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC07->tag != 32)goto _LL6C4;}_LL6C3:
 goto _LL6C5;_LL6C4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC08=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC08->tag != 1)goto _LL6C6;}_LL6C5:
 goto _LL6BD;_LL6C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpC09=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC09->tag != 2)goto _LL6C8;else{_tmpC0A=_tmpC09->f2;}}_LL6C7:
# 4734
 for(0;_tmpC0A != 0;_tmpC0A=_tmpC0A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpC0A->hd,te,cvtenv);}
goto _LL6BD;_LL6C8: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpC0B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC0B->tag != 5)goto _LL6CA;else{_tmpC0C=_tmpC0B->f1;_tmpC0D=_tmpC0B->f2;_tmpC0E=_tmpC0B->f3;}}_LL6C9:
# 4738
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC0E,te,cvtenv);
goto _LL6BD;_LL6CA: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpC0F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC0F->tag != 6)goto _LL6CC;else{_tmpC10=_tmpC0F->f1;_tmpC11=_tmpC0F->f2;}}_LL6CB:
 _tmpC13=_tmpC10;_tmpC14=_tmpC11;goto _LL6CD;_LL6CC: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpC12=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC12->tag != 7)goto _LL6CE;else{_tmpC13=_tmpC12->f1;_tmpC14=_tmpC12->f2;}}_LL6CD:
 _tmpC16=_tmpC13;_tmpC17=_tmpC14;goto _LL6CF;_LL6CE: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpC15=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC15->tag != 8)goto _LL6D0;else{_tmpC16=_tmpC15->f1;_tmpC17=_tmpC15->f2;}}_LL6CF:
# 4745
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC16,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC17,te,cvtenv);
goto _LL6BD;_LL6D0: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC18=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC18->tag != 13)goto _LL6D2;else{_tmpC19=(void*)_tmpC18->f1;_tmpC1A=_tmpC18->f2;}}_LL6D1:
# 4749
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC1A,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC19,1);
goto _LL6BD;_LL6D2: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpC1B=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC1B->tag != 18)goto _LL6D4;else{_tmpC1C=(void*)_tmpC1B->f1;}}_LL6D3:
 _tmpC1E=_tmpC1C;goto _LL6D5;_LL6D4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpC1D=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC1D->tag != 16)goto _LL6D6;else{_tmpC1E=(void*)_tmpC1D->f1;}}_LL6D5:
# 4754
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpC1E,1);
goto _LL6BD;_LL6D6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpC1F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC1F->tag != 38)goto _LL6D8;else{_tmpC20=(void*)_tmpC1F->f1;}}_LL6D7:
# 4757
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpC20,1);
goto _LL6BD;_LL6D8: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpC21=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC04;if(_tmpC21->tag != 17)goto _LL6DA;else{_tmpC22=_tmpC21->f1;}}_LL6D9:
# 4760
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpC22,te,cvtenv);
goto _LL6BD;_LL6DA:;_LL6DB: {
# 4763
const char*_tmp139B;void*_tmp139A;(_tmp139A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp139B="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp139B,sizeof(char),66))),_tag_dyneither(_tmp139A,sizeof(void*),0)));}_LL6BD:;}
# 4765
return cvtenv;}
# 4768
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4773
struct Cyc_List_List*_tmpC25=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4776
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpC26=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpC28;struct _tuple28 _tmpC27=_tmpC26;_tmpC28=_tmpC27.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpC25,_tmpC28);}}
# 4784
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpC29=*((struct _tuple29*)evs->hd);void*_tmpC2B;struct _tuple29 _tmpC2A=_tmpC29;_tmpC2B=_tmpC2A.f1;{
void*_tmpC2C=Cyc_Tcutil_compress(_tmpC2B);void*_tmpC2D=_tmpC2C;struct Cyc_Core_Opt**_tmpC2F;_LL6DD: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpC2E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpC2D;if(_tmpC2E->tag != 1)goto _LL6DF;else{_tmpC2F=(struct Cyc_Core_Opt**)& _tmpC2E->f4;}}_LL6DE:
# 4788
 if(*_tmpC2F == 0){
struct Cyc_Core_Opt*_tmp139C;*_tmpC2F=((_tmp139C=_cycalloc(sizeof(*_tmp139C)),((_tmp139C->v=_tmpC25,_tmp139C))));}else{
# 4792
struct Cyc_List_List*_tmpC31=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpC2F))->v;
struct Cyc_List_List*_tmpC32=0;
for(0;_tmpC31 != 0;_tmpC31=_tmpC31->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpC25,(struct Cyc_Absyn_Tvar*)_tmpC31->hd)){
struct Cyc_List_List*_tmp139D;_tmpC32=((_tmp139D=_cycalloc(sizeof(*_tmp139D)),((_tmp139D->hd=(struct Cyc_Absyn_Tvar*)_tmpC31->hd,((_tmp139D->tl=_tmpC32,_tmp139D))))));}}{
struct Cyc_Core_Opt*_tmp139E;*_tmpC2F=((_tmp139E=_cycalloc(sizeof(*_tmp139E)),((_tmp139E->v=_tmpC32,_tmp139E))));};}
# 4799
goto _LL6DC;_LL6DF:;_LL6E0:
 goto _LL6DC;_LL6DC:;};}}
# 4803
return cvt;}
# 4809
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4811
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpC35=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpC36=_tmpC35;_LL6E2: if(_tmpC36->kind != Cyc_Absyn_RgnKind)goto _LL6E4;if(_tmpC36->aliasqual != Cyc_Absyn_Unique)goto _LL6E4;_LL6E3:
# 4815
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp13A1;void*_tmp13A0;(_tmp13A0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A1="can't unify evar with unique region!",_tag_dyneither(_tmp13A1,sizeof(char),37))),_tag_dyneither(_tmp13A0,sizeof(void*),0)));}
goto _LL6E1;_LL6E4: if(_tmpC36->kind != Cyc_Absyn_RgnKind)goto _LL6E6;if(_tmpC36->aliasqual != Cyc_Absyn_Aliasable)goto _LL6E6;_LL6E5:
 goto _LL6E7;_LL6E6: if(_tmpC36->kind != Cyc_Absyn_RgnKind)goto _LL6E8;if(_tmpC36->aliasqual != Cyc_Absyn_Top)goto _LL6E8;_LL6E7:
# 4820
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp13A4;void*_tmp13A3;(_tmp13A3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A4="can't unify evar with heap!",_tag_dyneither(_tmp13A4,sizeof(char),28))),_tag_dyneither(_tmp13A3,sizeof(void*),0)));}
goto _LL6E1;_LL6E8: if(_tmpC36->kind != Cyc_Absyn_EffKind)goto _LL6EA;_LL6E9:
# 4823
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp13A7;void*_tmp13A6;(_tmp13A6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A7="can't unify evar with {}!",_tag_dyneither(_tmp13A7,sizeof(char),26))),_tag_dyneither(_tmp13A6,sizeof(void*),0)));}
goto _LL6E1;_LL6EA:;_LL6EB:
# 4826
{const char*_tmp13AC;void*_tmp13AB[2];struct Cyc_String_pa_PrintArg_struct _tmp13AA;struct Cyc_String_pa_PrintArg_struct _tmp13A9;(_tmp13A9.tag=0,((_tmp13A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp13AA.tag=0,((_tmp13AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp13AB[0]=& _tmp13AA,((_tmp13AB[1]=& _tmp13A9,Cyc_Tcutil_terr(loc,((_tmp13AC="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp13AC,sizeof(char),52))),_tag_dyneither(_tmp13AB,sizeof(void*),2)))))))))))));}
goto _LL6E1;_LL6E1:;}}
# 4838
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpC41=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmpC42=_new_region("temp");struct _RegionHandle*temp=& _tmpC42;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp13AD;struct Cyc_Tcutil_CVTEnv _tmpC43=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13AD.r=temp,((_tmp13AD.kind_env=_tmpC41,((_tmp13AD.free_vars=0,((_tmp13AD.free_evars=0,((_tmp13AD.generalize_evars=generalize_evars,((_tmp13AD.fn_result=0,_tmp13AD)))))))))))),expected_kind,t);
# 4847
struct Cyc_List_List*_tmpC44=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC43.free_vars);
struct Cyc_List_List*_tmpC45=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC43.free_evars);
# 4851
if(_tmpC41 != 0){
struct Cyc_List_List*_tmpC46=0;
{struct Cyc_List_List*_tmpC47=_tmpC44;for(0;(unsigned int)_tmpC47;_tmpC47=_tmpC47->tl){
struct Cyc_Absyn_Tvar*_tmpC48=(struct Cyc_Absyn_Tvar*)_tmpC47->hd;
int found=0;
{struct Cyc_List_List*_tmpC49=_tmpC41;for(0;(unsigned int)_tmpC49;_tmpC49=_tmpC49->tl){
if(Cyc_Absyn_tvar_cmp(_tmpC48,(struct Cyc_Absyn_Tvar*)_tmpC49->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp13AE;_tmpC46=((_tmp13AE=_region_malloc(temp,sizeof(*_tmp13AE)),((_tmp13AE->hd=(struct Cyc_Absyn_Tvar*)_tmpC47->hd,((_tmp13AE->tl=_tmpC46,_tmp13AE))))));}}}
# 4861
_tmpC44=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC46);}
# 4867
{struct Cyc_List_List*x=_tmpC44;for(0;x != 0;x=x->tl){
void*_tmpC4B=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpC4C=_tmpC4B;struct Cyc_Core_Opt**_tmpC4E;struct Cyc_Core_Opt**_tmpC50;struct Cyc_Core_Opt**_tmpC52;struct Cyc_Core_Opt**_tmpC54;struct Cyc_Core_Opt**_tmpC56;struct Cyc_Core_Opt**_tmpC58;struct Cyc_Core_Opt**_tmpC5A;struct Cyc_Absyn_Kind*_tmpC5B;enum Cyc_Absyn_AliasQual _tmpC5D;_LL6ED: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpC4D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC4D->tag != 1)goto _LL6EF;else{_tmpC4E=(struct Cyc_Core_Opt**)& _tmpC4D->f1;}}_LL6EE:
 _tmpC50=_tmpC4E;goto _LL6F0;_LL6EF: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC4F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC4F->tag != 2)goto _LL6F1;else{_tmpC50=(struct Cyc_Core_Opt**)& _tmpC4F->f1;if((_tmpC4F->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6F1;if((_tmpC4F->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F1;}}_LL6F0:
 _tmpC52=_tmpC50;goto _LL6F2;_LL6F1: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC51=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC51->tag != 2)goto _LL6F3;else{_tmpC52=(struct Cyc_Core_Opt**)& _tmpC51->f1;if((_tmpC51->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F3;if((_tmpC51->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F3;}}_LL6F2:
 _tmpC54=_tmpC52;goto _LL6F4;_LL6F3: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC53=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC53->tag != 2)goto _LL6F5;else{_tmpC54=(struct Cyc_Core_Opt**)& _tmpC53->f1;if((_tmpC53->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F5;if((_tmpC53->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6F5;}}_LL6F4:
# 4873
*_tmpC54=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6EC;_LL6F5: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC55=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC55->tag != 2)goto _LL6F7;else{_tmpC56=(struct Cyc_Core_Opt**)& _tmpC55->f1;if((_tmpC55->f2)->kind != Cyc_Absyn_MemKind)goto _LL6F7;if((_tmpC55->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6F7;}}_LL6F6:
# 4875
*_tmpC56=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6EC;_LL6F7: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC57=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC57->tag != 2)goto _LL6F9;else{_tmpC58=(struct Cyc_Core_Opt**)& _tmpC57->f1;if((_tmpC57->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6F9;if((_tmpC57->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6F9;}}_LL6F8:
# 4877
*_tmpC58=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6EC;_LL6F9: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpC59=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC59->tag != 2)goto _LL6FB;else{_tmpC5A=(struct Cyc_Core_Opt**)& _tmpC59->f1;_tmpC5B=_tmpC59->f2;}}_LL6FA:
# 4879
*_tmpC5A=Cyc_Tcutil_kind_to_bound_opt(_tmpC5B);goto _LL6EC;_LL6FB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpC5C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpC4C;if(_tmpC5C->tag != 0)goto _LL6FD;else{if((_tmpC5C->f1)->kind != Cyc_Absyn_MemKind)goto _LL6FD;_tmpC5D=(_tmpC5C->f1)->aliasqual;}}_LL6FC:
# 4881
{const char*_tmp13B5;void*_tmp13B4[2];struct Cyc_String_pa_PrintArg_struct _tmp13B3;struct Cyc_Absyn_Kind*_tmp13B2;struct Cyc_String_pa_PrintArg_struct _tmp13B1;(_tmp13B1.tag=0,((_tmp13B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp13B2=_cycalloc_atomic(sizeof(*_tmp13B2)),((_tmp13B2->kind=Cyc_Absyn_MemKind,((_tmp13B2->aliasqual=_tmpC5D,_tmp13B2)))))))),((_tmp13B3.tag=0,((_tmp13B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp13B4[0]=& _tmp13B3,((_tmp13B4[1]=& _tmp13B1,Cyc_Tcutil_terr(loc,((_tmp13B5="type variable %s cannot have kind %s",_tag_dyneither(_tmp13B5,sizeof(char),37))),_tag_dyneither(_tmp13B4,sizeof(void*),2)))))))))))));}
goto _LL6EC;_LL6FD:;_LL6FE:
 goto _LL6EC;_LL6EC:;}}
# 4888
if(_tmpC44 != 0  || _tmpC45 != 0){
{void*_tmpC63=Cyc_Tcutil_compress(t);void*_tmpC64=_tmpC63;struct Cyc_List_List**_tmpC66;_LL700: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC65=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC64;if(_tmpC65->tag != 9)goto _LL702;else{_tmpC66=(struct Cyc_List_List**)&(_tmpC65->f1).tvars;}}_LL701:
# 4891
 if(*_tmpC66 == 0){
# 4893
*_tmpC66=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpC44);
_tmpC44=0;}
# 4896
goto _LL6FF;_LL702:;_LL703:
 goto _LL6FF;_LL6FF:;}
# 4899
if(_tmpC44 != 0){
const char*_tmp13B9;void*_tmp13B8[1];struct Cyc_String_pa_PrintArg_struct _tmp13B7;(_tmp13B7.tag=0,((_tmp13B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpC44->hd)->name),((_tmp13B8[0]=& _tmp13B7,Cyc_Tcutil_terr(loc,((_tmp13B9="unbound type variable %s",_tag_dyneither(_tmp13B9,sizeof(char),25))),_tag_dyneither(_tmp13B8,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpC45,t,loc);}}
# 4843
;_pop_region(temp);}
# 4910
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4913
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC6B=Cyc_Tcutil_compress(t);void*_tmpC6C=_tmpC6B;struct Cyc_List_List*_tmpC6E;void*_tmpC6F;struct Cyc_Absyn_Tqual _tmpC70;void*_tmpC71;struct Cyc_List_List*_tmpC72;struct Cyc_Absyn_Exp*_tmpC73;struct Cyc_List_List*_tmpC74;struct Cyc_Absyn_Exp*_tmpC75;struct Cyc_List_List*_tmpC76;_LL705: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC6D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC6C;if(_tmpC6D->tag != 9)goto _LL707;else{_tmpC6E=(_tmpC6D->f1).tvars;_tmpC6F=(_tmpC6D->f1).effect;_tmpC70=(_tmpC6D->f1).ret_tqual;_tmpC71=(_tmpC6D->f1).ret_typ;_tmpC72=(_tmpC6D->f1).args;_tmpC73=(_tmpC6D->f1).requires_clause;_tmpC74=(_tmpC6D->f1).requires_relns;_tmpC75=(_tmpC6D->f1).ensures_clause;_tmpC76=(_tmpC6D->f1).ensures_relns;}}_LL706:
# 4916
 fd->tvs=_tmpC6E;
fd->effect=_tmpC6F;
{struct Cyc_List_List*_tmpC77=fd->args;for(0;_tmpC77 != 0;(_tmpC77=_tmpC77->tl,_tmpC72=_tmpC72->tl)){
# 4920
(*((struct _tuple10*)_tmpC77->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC72))->hd)).f2;
(*((struct _tuple10*)_tmpC77->hd)).f3=(*((struct _tuple10*)_tmpC72->hd)).f3;}}
# 4923
fd->ret_tqual=_tmpC70;
fd->ret_type=_tmpC71;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC71);
fd->requires_clause=_tmpC73;
fd->requires_relns=_tmpC74;
fd->ensures_clause=_tmpC75;
fd->ensures_relns=_tmpC76;
goto _LL704;_LL707:;_LL708: {
const char*_tmp13BC;void*_tmp13BB;(_tmp13BB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13BC="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp13BC,sizeof(char),38))),_tag_dyneither(_tmp13BB,sizeof(void*),0)));}_LL704:;}{
# 4934
struct _RegionHandle _tmpC7A=_new_region("r");struct _RegionHandle*r=& _tmpC7A;_push_region(r);{
const char*_tmp13BD;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp13BD="function declaration has repeated parameter",_tag_dyneither(_tmp13BD,sizeof(char),44))));}
# 4939
fd->cached_typ=t;
# 4934
;_pop_region(r);};}
# 4944
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4947
struct _RegionHandle _tmpC7C=_new_region("r");struct _RegionHandle*r=& _tmpC7C;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp13BE;struct Cyc_Tcutil_CVTEnv _tmpC7D=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp13BE.r=r,((_tmp13BE.kind_env=bound_tvars,((_tmp13BE.free_vars=0,((_tmp13BE.free_evars=0,((_tmp13BE.generalize_evars=0,((_tmp13BE.fn_result=0,_tmp13BE)))))))))))),expected_kind,t);
# 4951
struct Cyc_List_List*_tmpC7E=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC7D.free_vars),bound_tvars);
# 4953
struct Cyc_List_List*_tmpC7F=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC7D.free_evars);
{struct Cyc_List_List*fs=_tmpC7E;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC80=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp13C3;void*_tmp13C2[2];struct Cyc_String_pa_PrintArg_struct _tmp13C1;struct Cyc_String_pa_PrintArg_struct _tmp13C0;(_tmp13C0.tag=0,((_tmp13C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13C1.tag=0,((_tmp13C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC80),((_tmp13C2[0]=& _tmp13C1,((_tmp13C2[1]=& _tmp13C0,Cyc_Tcutil_terr(loc,((_tmp13C3="unbound type variable %s in type %s",_tag_dyneither(_tmp13C3,sizeof(char),36))),_tag_dyneither(_tmp13C2,sizeof(void*),2)))))))))))));}}
# 4958
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC7F,t,loc);}
# 4948
;_pop_region(r);}
# 4963
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4968
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4974
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4977
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp13C8;void*_tmp13C7[2];struct Cyc_String_pa_PrintArg_struct _tmp13C6;struct Cyc_String_pa_PrintArg_struct _tmp13C5;(_tmp13C5.tag=0,((_tmp13C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp13C6.tag=0,((_tmp13C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp13C7[0]=& _tmp13C6,((_tmp13C7[1]=& _tmp13C5,Cyc_Tcutil_terr(loc,((_tmp13C8="%s: %s",_tag_dyneither(_tmp13C8,sizeof(char),7))),_tag_dyneither(_tmp13C7,sizeof(void*),2)))))))))))));}}}}
# 4983
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4987
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4991
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp13C9;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp13C9="duplicate type variable",_tag_dyneither(_tmp13C9,sizeof(char),24))));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5005 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5010
struct _RegionHandle _tmpC8B=_new_region("temp");struct _RegionHandle*temp=& _tmpC8B;_push_region(temp);
# 5014
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp13CA;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp13CA="",_tag_dyneither(_tmp13CA,sizeof(char),1))))!= 0){
struct _tuple32*_tmp13CD;struct Cyc_List_List*_tmp13CC;fields=((_tmp13CC=_region_malloc(temp,sizeof(*_tmp13CC)),((_tmp13CC->hd=((_tmp13CD=_region_malloc(temp,sizeof(*_tmp13CD)),((_tmp13CD->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp13CD->f2=0,_tmp13CD)))))),((_tmp13CC->tl=fields,_tmp13CC))))));}}}
# 5019
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5021
const char*_tmp13CF;const char*_tmp13CE;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp13CF="struct",_tag_dyneither(_tmp13CF,sizeof(char),7)):((_tmp13CE="union",_tag_dyneither(_tmp13CE,sizeof(char),6)));
# 5024
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmpC8F=(struct _tuple33*)des->hd;struct Cyc_List_List*_tmpC91;void*_tmpC92;struct _tuple33*_tmpC90=_tmpC8F;_tmpC91=_tmpC90->f1;_tmpC92=_tmpC90->f2;
if(_tmpC91 == 0){
# 5029
struct Cyc_List_List*_tmpC93=fields;
for(0;_tmpC93 != 0;_tmpC93=_tmpC93->tl){
if(!(*((struct _tuple32*)_tmpC93->hd)).f2){
(*((struct _tuple32*)_tmpC93->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp13D5;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp13D4;struct Cyc_List_List*_tmp13D3;(*((struct _tuple33*)des->hd)).f1=((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3->hd=(void*)((_tmp13D5=_cycalloc(sizeof(*_tmp13D5)),((_tmp13D5[0]=((_tmp13D4.tag=1,((_tmp13D4.f1=((*((struct _tuple32*)_tmpC93->hd)).f1)->name,_tmp13D4)))),_tmp13D5)))),((_tmp13D3->tl=0,_tmp13D3))))));}
{struct _tuple34*_tmp13D8;struct Cyc_List_List*_tmp13D7;ans=((_tmp13D7=_region_malloc(rgn,sizeof(*_tmp13D7)),((_tmp13D7->hd=((_tmp13D8=_region_malloc(rgn,sizeof(*_tmp13D8)),((_tmp13D8->f1=(*((struct _tuple32*)_tmpC93->hd)).f1,((_tmp13D8->f2=_tmpC92,_tmp13D8)))))),((_tmp13D7->tl=ans,_tmp13D7))))));}
break;}}
# 5037
if(_tmpC93 == 0){
const char*_tmp13DC;void*_tmp13DB[1];struct Cyc_String_pa_PrintArg_struct _tmp13DA;(_tmp13DA.tag=0,((_tmp13DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13DB[0]=& _tmp13DA,Cyc_Tcutil_terr(loc,((_tmp13DC="too many arguments to %s",_tag_dyneither(_tmp13DC,sizeof(char),25))),_tag_dyneither(_tmp13DB,sizeof(void*),1)))))));}}else{
if(_tmpC91->tl != 0){
# 5041
const char*_tmp13DF;void*_tmp13DE;(_tmp13DE=0,Cyc_Tcutil_terr(loc,((_tmp13DF="multiple designators are not yet supported",_tag_dyneither(_tmp13DF,sizeof(char),43))),_tag_dyneither(_tmp13DE,sizeof(void*),0)));}else{
# 5044
void*_tmpC9E=(void*)_tmpC91->hd;void*_tmpC9F=_tmpC9E;struct _dyneither_ptr*_tmpCA2;_LL70A: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpCA0=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC9F;if(_tmpCA0->tag != 0)goto _LL70C;}_LL70B:
# 5046
{const char*_tmp13E3;void*_tmp13E2[1];struct Cyc_String_pa_PrintArg_struct _tmp13E1;(_tmp13E1.tag=0,((_tmp13E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp13E2[0]=& _tmp13E1,Cyc_Tcutil_terr(loc,((_tmp13E3="array designator used in argument to %s",_tag_dyneither(_tmp13E3,sizeof(char),40))),_tag_dyneither(_tmp13E2,sizeof(void*),1)))))));}
goto _LL709;_LL70C: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpCA1=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC9F;if(_tmpCA1->tag != 1)goto _LL709;else{_tmpCA2=_tmpCA1->f1;}}_LL70D: {
# 5049
struct Cyc_List_List*_tmpCA6=fields;
for(0;_tmpCA6 != 0;_tmpCA6=_tmpCA6->tl){
if(Cyc_strptrcmp(_tmpCA2,((*((struct _tuple32*)_tmpCA6->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmpCA6->hd)).f2){
const char*_tmp13E7;void*_tmp13E6[1];struct Cyc_String_pa_PrintArg_struct _tmp13E5;(_tmp13E5.tag=0,((_tmp13E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA2),((_tmp13E6[0]=& _tmp13E5,Cyc_Tcutil_terr(loc,((_tmp13E7="member %s has already been used as an argument",_tag_dyneither(_tmp13E7,sizeof(char),47))),_tag_dyneither(_tmp13E6,sizeof(void*),1)))))));}
(*((struct _tuple32*)_tmpCA6->hd)).f2=1;
{struct _tuple34*_tmp13EA;struct Cyc_List_List*_tmp13E9;ans=((_tmp13E9=_region_malloc(rgn,sizeof(*_tmp13E9)),((_tmp13E9->hd=((_tmp13EA=_region_malloc(rgn,sizeof(*_tmp13EA)),((_tmp13EA->f1=(*((struct _tuple32*)_tmpCA6->hd)).f1,((_tmp13EA->f2=_tmpC92,_tmp13EA)))))),((_tmp13E9->tl=ans,_tmp13E9))))));}
break;}}
# 5058
if(_tmpCA6 == 0){
const char*_tmp13EE;void*_tmp13ED[1];struct Cyc_String_pa_PrintArg_struct _tmp13EC;(_tmp13EC.tag=0,((_tmp13EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpCA2),((_tmp13ED[0]=& _tmp13EC,Cyc_Tcutil_terr(loc,((_tmp13EE="bad field designator %s",_tag_dyneither(_tmp13EE,sizeof(char),24))),_tag_dyneither(_tmp13ED,sizeof(void*),1)))))));}
goto _LL709;}_LL709:;}}}
# 5063
if(aggr_kind == Cyc_Absyn_StructA)
# 5065
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
{const char*_tmp13F1;void*_tmp13F0;(_tmp13F0=0,Cyc_Tcutil_terr(loc,((_tmp13F1="too few arguments to struct",_tag_dyneither(_tmp13F1,sizeof(char),28))),_tag_dyneither(_tmp13F0,sizeof(void*),0)));}
break;}}else{
# 5072
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found){const char*_tmp13F4;void*_tmp13F3;(_tmp13F3=0,Cyc_Tcutil_terr(loc,((_tmp13F4="only one member of a union is allowed",_tag_dyneither(_tmp13F4,sizeof(char),38))),_tag_dyneither(_tmp13F3,sizeof(void*),0)));}
found=1;}}
# 5079
if(!found){const char*_tmp13F7;void*_tmp13F6;(_tmp13F6=0,Cyc_Tcutil_terr(loc,((_tmp13F7="missing member for union",_tag_dyneither(_tmp13F7,sizeof(char),25))),_tag_dyneither(_tmp13F6,sizeof(void*),0)));}}{
# 5082
struct Cyc_List_List*_tmpCB5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpCB5;};};}
# 5014
;_pop_region(temp);}
# 5085
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCB8=Cyc_Tcutil_compress(t);void*_tmpCB9=_tmpCB8;void*_tmpCBB;union Cyc_Absyn_Constraint*_tmpCBC;_LL70F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCBA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCB9;if(_tmpCBA->tag != 5)goto _LL711;else{_tmpCBB=(_tmpCBA->f1).elt_typ;_tmpCBC=((_tmpCBA->f1).ptr_atts).bounds;}}_LL710: {
# 5088
void*_tmpCBD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpCBC);void*_tmpCBE=_tmpCBD;_LL714: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCBF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCBE;if(_tmpCBF->tag != 0)goto _LL716;}_LL715:
# 5090
*elt_typ_dest=_tmpCBB;
return 1;_LL716:;_LL717:
 return 0;_LL713:;}_LL711:;_LL712:
# 5094
 return 0;_LL70E:;}
# 5098
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpCC0=Cyc_Tcutil_compress(t);void*_tmpCC1=_tmpCC0;void*_tmpCC3;union Cyc_Absyn_Constraint*_tmpCC4;_LL719: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC1;if(_tmpCC2->tag != 5)goto _LL71B;else{_tmpCC3=(_tmpCC2->f1).elt_typ;_tmpCC4=((_tmpCC2->f1).ptr_atts).zero_term;}}_LL71A:
# 5101
*elt_typ_dest=_tmpCC3;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCC4);_LL71B:;_LL71C:
 return 0;_LL718:;}
# 5109
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5111
void*_tmpCC5=Cyc_Tcutil_compress(t);void*_tmpCC6=_tmpCC5;void*_tmpCC8;union Cyc_Absyn_Constraint*_tmpCC9;union Cyc_Absyn_Constraint*_tmpCCA;void*_tmpCCC;struct Cyc_Absyn_Tqual _tmpCCD;struct Cyc_Absyn_Exp*_tmpCCE;union Cyc_Absyn_Constraint*_tmpCCF;_LL71E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCC7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCC6;if(_tmpCC7->tag != 5)goto _LL720;else{_tmpCC8=(_tmpCC7->f1).elt_typ;_tmpCC9=((_tmpCC7->f1).ptr_atts).bounds;_tmpCCA=((_tmpCC7->f1).ptr_atts).zero_term;}}_LL71F:
# 5113
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCCA)){
*ptr_type=t;
*elt_type=_tmpCC8;
{void*_tmpCD0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpCC9);void*_tmpCD1=_tmpCD0;_LL725: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpCD2=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpCD1;if(_tmpCD2->tag != 0)goto _LL727;}_LL726:
*is_dyneither=1;goto _LL724;_LL727:;_LL728:
*is_dyneither=0;goto _LL724;_LL724:;}
# 5120
return 1;}else{
return 0;}_LL720: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpCCB=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpCC6;if(_tmpCCB->tag != 8)goto _LL722;else{_tmpCCC=(_tmpCCB->f1).elt_type;_tmpCCD=(_tmpCCB->f1).tq;_tmpCCE=(_tmpCCB->f1).num_elts;_tmpCCF=(_tmpCCB->f1).zero_term;}}_LL721:
# 5123
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpCCF)){
*elt_type=_tmpCCC;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL722:;_LL723:
 return 0;_LL71D:;}
# 5136
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5138
void*_tmpCD3=e1->r;void*_tmpCD4=_tmpCD3;struct Cyc_Absyn_Exp*_tmpCD7;struct Cyc_Absyn_Exp*_tmpCD9;struct Cyc_Absyn_Exp*_tmpCDB;struct Cyc_Absyn_Exp*_tmpCDD;struct Cyc_Absyn_Exp*_tmpCDF;struct Cyc_Absyn_Exp*_tmpCE1;_LL72A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCD5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCD5->tag != 13)goto _LL72C;}_LL72B: {
# 5140
const char*_tmp13FB;void*_tmp13FA[1];struct Cyc_String_pa_PrintArg_struct _tmp13F9;(_tmp13F9.tag=0,((_tmp13F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp13FA[0]=& _tmp13F9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FB="we have a cast in a lhs:  %s",_tag_dyneither(_tmp13FB,sizeof(char),29))),_tag_dyneither(_tmp13FA,sizeof(void*),1)))))));}_LL72C: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCD6=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCD6->tag != 19)goto _LL72E;else{_tmpCD7=_tmpCD6->f1;}}_LL72D:
 _tmpCD9=_tmpCD7;goto _LL72F;_LL72E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCD8=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCD8->tag != 22)goto _LL730;else{_tmpCD9=_tmpCD8->f1;}}_LL72F:
# 5143
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCD9->topt),ptr_type,is_dyneither,elt_type);_LL730: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCDA=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCDA->tag != 21)goto _LL732;else{_tmpCDB=_tmpCDA->f1;}}_LL731:
 _tmpCDD=_tmpCDB;goto _LL733;_LL732: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCDC=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCDC->tag != 20)goto _LL734;else{_tmpCDD=_tmpCDC->f1;}}_LL733:
# 5147
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCDD->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp13FF;void*_tmp13FE[1];struct Cyc_String_pa_PrintArg_struct _tmp13FD;(_tmp13FD.tag=0,((_tmp13FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp13FE[0]=& _tmp13FD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13FF="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp13FF,sizeof(char),51))),_tag_dyneither(_tmp13FE,sizeof(void*),1)))))));}
return 0;_LL734: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpCDE=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCDE->tag != 12)goto _LL736;else{_tmpCDF=_tmpCDE->f1;}}_LL735:
 _tmpCE1=_tmpCDF;goto _LL737;_LL736: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpCE0=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCE0->tag != 11)goto _LL738;else{_tmpCE1=_tmpCE0->f1;}}_LL737:
# 5153
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpCE1->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp1403;void*_tmp1402[1];struct Cyc_String_pa_PrintArg_struct _tmp1401;(_tmp1401.tag=0,((_tmp1401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1402[0]=& _tmp1401,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1403="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp1403,sizeof(char),49))),_tag_dyneither(_tmp1402,sizeof(void*),1)))))));}
return 0;_LL738: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE2=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCD4;if(_tmpCE2->tag != 1)goto _LL73A;}_LL739:
 return 0;_LL73A:;_LL73B: {
# 5159
const char*_tmp1407;void*_tmp1406[1];struct Cyc_String_pa_PrintArg_struct _tmp1405;(_tmp1405.tag=0,((_tmp1405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1406[0]=& _tmp1405,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1407="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1407,sizeof(char),39))),_tag_dyneither(_tmp1406,sizeof(void*),1)))))));}_LL729:;}
# 5163
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5174
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpCEF=Cyc_Tcutil_compress(r);void*_tmpCF0=_tmpCEF;struct Cyc_Absyn_Tvar*_tmpCF4;_LL73D: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpCF1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpCF0;if(_tmpCF1->tag != 22)goto _LL73F;}_LL73E:
 return !must_be_unique;_LL73F: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpCF2=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpCF0;if(_tmpCF2->tag != 21)goto _LL741;}_LL740:
 return 1;_LL741: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCF3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpCF0;if(_tmpCF3->tag != 2)goto _LL743;else{_tmpCF4=_tmpCF3->f1;}}_LL742: {
# 5179
struct Cyc_Absyn_Kind*_tmpCF5=Cyc_Tcutil_tvar_kind(_tmpCF4,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpCF7;enum Cyc_Absyn_AliasQual _tmpCF8;struct Cyc_Absyn_Kind*_tmpCF6=_tmpCF5;_tmpCF7=_tmpCF6->kind;_tmpCF8=_tmpCF6->aliasqual;
if(_tmpCF7 == Cyc_Absyn_RgnKind  && (_tmpCF8 == Cyc_Absyn_Unique  || _tmpCF8 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpCF9=Cyc_Absyn_compress_kb(_tmpCF4->kind);void*_tmpCFA=_tmpCF9;struct Cyc_Core_Opt**_tmpCFC;_LL746: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCFB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCFA;if(_tmpCFB->tag != 2)goto _LL748;else{_tmpCFC=(struct Cyc_Core_Opt**)& _tmpCFB->f1;if((_tmpCFB->f2)->kind != Cyc_Absyn_RgnKind)goto _LL748;if((_tmpCFB->f2)->aliasqual != Cyc_Absyn_Top)goto _LL748;}}_LL747:
# 5183
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp140D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp140C;struct Cyc_Core_Opt*_tmp140B;*_tmpCFC=((_tmp140B=_cycalloc(sizeof(*_tmp140B)),((_tmp140B->v=(void*)((_tmp140D=_cycalloc(sizeof(*_tmp140D)),((_tmp140D[0]=((_tmp140C.tag=2,((_tmp140C.f1=0,((_tmp140C.f2=& Cyc_Tcutil_rk,_tmp140C)))))),_tmp140D)))),_tmp140B))));}
return 0;_LL748:;_LL749:
 return 1;_LL745:;}
# 5188
return 0;}_LL743:;_LL744:
 return 0;_LL73C:;}
# 5195
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpD00=Cyc_Tcutil_compress(t);void*_tmpD01=_tmpD00;void*_tmpD03;struct Cyc_Absyn_Tvar*_tmpD05;_LL74B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD01;if(_tmpD02->tag != 5)goto _LL74D;else{_tmpD03=((_tmpD02->f1).ptr_atts).rgn;}}_LL74C:
# 5198
 return Cyc_Tcutil_is_noalias_region(_tmpD03,must_be_unique);_LL74D: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD04=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpD01;if(_tmpD04->tag != 2)goto _LL74F;else{_tmpD05=_tmpD04->f1;}}_LL74E: {
# 5200
struct Cyc_Absyn_Kind*_tmpD06=Cyc_Tcutil_tvar_kind(_tmpD05,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpD08;enum Cyc_Absyn_AliasQual _tmpD09;struct Cyc_Absyn_Kind*_tmpD07=_tmpD06;_tmpD08=_tmpD07->kind;_tmpD09=_tmpD07->aliasqual;
switch(_tmpD08){case Cyc_Absyn_BoxKind: _LL751:
 goto _LL752;case Cyc_Absyn_AnyKind: _LL752: goto _LL753;case Cyc_Absyn_MemKind: _LL753:
 if(_tmpD09 == Cyc_Absyn_Unique  || _tmpD09 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpD0A=Cyc_Absyn_compress_kb(_tmpD05->kind);void*_tmpD0B=_tmpD0A;struct Cyc_Core_Opt**_tmpD0D;enum Cyc_Absyn_KindQual _tmpD0E;_LL756: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpD0C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD0B;if(_tmpD0C->tag != 2)goto _LL758;else{_tmpD0D=(struct Cyc_Core_Opt**)& _tmpD0C->f1;_tmpD0E=(_tmpD0C->f2)->kind;if((_tmpD0C->f2)->aliasqual != Cyc_Absyn_Top)goto _LL758;}}_LL757:
# 5206
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1417;struct Cyc_Absyn_Kind*_tmp1416;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1415;struct Cyc_Core_Opt*_tmp1414;*_tmpD0D=((_tmp1414=_cycalloc(sizeof(*_tmp1414)),((_tmp1414->v=(void*)((_tmp1417=_cycalloc(sizeof(*_tmp1417)),((_tmp1417[0]=((_tmp1415.tag=2,((_tmp1415.f1=0,((_tmp1415.f2=((_tmp1416=_cycalloc_atomic(sizeof(*_tmp1416)),((_tmp1416->kind=_tmpD0E,((_tmp1416->aliasqual=Cyc_Absyn_Aliasable,_tmp1416)))))),_tmp1415)))))),_tmp1417)))),_tmp1414))));}
return 0;_LL758:;_LL759:
# 5210
 return 1;_LL755:;}
# 5213
return 0;default: _LL754:
 return 0;}}_LL74F:;_LL750:
# 5216
 return 0;_LL74A:;}
# 5219
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpD13=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpD13,0))return 1;{
void*_tmpD14=_tmpD13;struct Cyc_List_List*_tmpD16;struct Cyc_Absyn_Aggrdecl**_tmpD18;struct Cyc_List_List*_tmpD19;struct Cyc_List_List*_tmpD1B;union Cyc_Absyn_DatatypeInfoU _tmpD1E;struct Cyc_List_List*_tmpD1F;union Cyc_Absyn_DatatypeFieldInfoU _tmpD21;struct Cyc_List_List*_tmpD22;_LL75C: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD15=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD14;if(_tmpD15->tag != 10)goto _LL75E;else{_tmpD16=_tmpD15->f1;}}_LL75D:
# 5224
 while(_tmpD16 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpD16->hd)).f2))return 1;
_tmpD16=_tmpD16->tl;}
# 5228
return 0;_LL75E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD17=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD14;if(_tmpD17->tag != 11)goto _LL760;else{if((((_tmpD17->f1).aggr_info).KnownAggr).tag != 2)goto _LL760;_tmpD18=(struct Cyc_Absyn_Aggrdecl**)(((_tmpD17->f1).aggr_info).KnownAggr).val;_tmpD19=(_tmpD17->f1).targs;}}_LL75F:
# 5230
 if((*_tmpD18)->impl == 0)return 0;else{
# 5232
struct Cyc_List_List*_tmpD23=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpD18)->tvs,_tmpD19);
struct Cyc_List_List*_tmpD24=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpD18)->impl))->fields;
void*t;
while(_tmpD24 != 0){
t=_tmpD23 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpD24->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpD23,((struct Cyc_Absyn_Aggrfield*)_tmpD24->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpD24=_tmpD24->tl;}
# 5240
return 0;}_LL760: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD1A=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD14;if(_tmpD1A->tag != 12)goto _LL762;else{_tmpD1B=_tmpD1A->f2;}}_LL761:
# 5243
 while(_tmpD1B != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpD1B->hd)->type))return 1;
_tmpD1B=_tmpD1B->tl;}
# 5247
return 0;_LL762: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD1C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD14;if(_tmpD1C->tag != 11)goto _LL764;else{if((((_tmpD1C->f1).aggr_info).UnknownAggr).tag != 1)goto _LL764;}}_LL763:
# 5250
 return 0;_LL764: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpD1D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpD14;if(_tmpD1D->tag != 3)goto _LL766;else{_tmpD1E=(_tmpD1D->f1).datatype_info;_tmpD1F=(_tmpD1D->f1).targs;}}_LL765: {
# 5252
union Cyc_Absyn_DatatypeInfoU _tmpD25=_tmpD1E;struct _tuple2*_tmpD26;int _tmpD27;struct Cyc_List_List*_tmpD28;struct Cyc_Core_Opt*_tmpD29;_LL76B: if((_tmpD25.UnknownDatatype).tag != 1)goto _LL76D;_tmpD26=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD25.UnknownDatatype).val).name;_tmpD27=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpD25.UnknownDatatype).val).is_extensible;_LL76C:
# 5255
 return 0;_LL76D: if((_tmpD25.KnownDatatype).tag != 2)goto _LL76A;_tmpD28=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD25.KnownDatatype).val))->tvs;_tmpD29=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpD25.KnownDatatype).val))->fields;_LL76E:
# 5258
 return 0;_LL76A:;}_LL766: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpD20=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpD14;if(_tmpD20->tag != 4)goto _LL768;else{_tmpD21=(_tmpD20->f1).field_info;_tmpD22=(_tmpD20->f1).targs;}}_LL767: {
# 5261
union Cyc_Absyn_DatatypeFieldInfoU _tmpD2A=_tmpD21;struct Cyc_Absyn_Datatypedecl*_tmpD2B;struct Cyc_Absyn_Datatypefield*_tmpD2C;_LL770: if((_tmpD2A.UnknownDatatypefield).tag != 1)goto _LL772;_LL771:
# 5264
 return 0;_LL772: if((_tmpD2A.KnownDatatypefield).tag != 2)goto _LL76F;_tmpD2B=((struct _tuple3)(_tmpD2A.KnownDatatypefield).val).f1;_tmpD2C=((struct _tuple3)(_tmpD2A.KnownDatatypefield).val).f2;_LL773: {
# 5266
struct Cyc_List_List*_tmpD2D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD2B->tvs,_tmpD22);
struct Cyc_List_List*_tmpD2E=_tmpD2C->typs;
while(_tmpD2E != 0){
_tmpD13=_tmpD2D == 0?(*((struct _tuple12*)_tmpD2E->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpD2D,(*((struct _tuple12*)_tmpD2E->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpD13))return 1;
_tmpD2E=_tmpD2E->tl;}
# 5273
return 0;}_LL76F:;}_LL768:;_LL769:
# 5275
 return 0;_LL75B:;};}
# 5279
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpD2F=Cyc_Tcutil_compress(t);void*_tmpD30=_tmpD2F;struct Cyc_Absyn_Aggrdecl*_tmpD32;struct Cyc_List_List*_tmpD33;struct Cyc_List_List*_tmpD35;_LL775: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD31=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD30;if(_tmpD31->tag != 11)goto _LL777;else{if((((_tmpD31->f1).aggr_info).KnownAggr).tag != 2)goto _LL777;_tmpD32=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD31->f1).aggr_info).KnownAggr).val);_tmpD33=(_tmpD31->f1).targs;}}_LL776:
# 5282
 _tmpD35=_tmpD32->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD32->impl))->fields;goto _LL778;_LL777: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD34=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD30;if(_tmpD34->tag != 12)goto _LL779;else{_tmpD35=_tmpD34->f2;}}_LL778: {
# 5284
struct Cyc_Absyn_Aggrfield*_tmpD36=Cyc_Absyn_lookup_field(_tmpD35,f);
{struct Cyc_Absyn_Aggrfield*_tmpD37=_tmpD36;void*_tmpD38;_LL77C: if(_tmpD37 != 0)goto _LL77E;_LL77D: {
const char*_tmp141A;void*_tmp1419;(_tmp1419=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141A="is_noalias_field: missing field",_tag_dyneither(_tmp141A,sizeof(char),32))),_tag_dyneither(_tmp1419,sizeof(void*),0)));}_LL77E: if(_tmpD37 == 0)goto _LL77B;_tmpD38=_tmpD37->type;_LL77F:
# 5288
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpD38);_LL77B:;}
# 5290
return 0;}_LL779:;_LL77A: {
# 5292
const char*_tmp141E;void*_tmp141D[1];struct Cyc_String_pa_PrintArg_struct _tmp141C;(_tmp141C.tag=0,((_tmp141C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp141D[0]=& _tmp141C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp141E="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp141E,sizeof(char),36))),_tag_dyneither(_tmp141D,sizeof(void*),1)))))));}_LL774:;}
# 5300
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpD3E=e->r;void*_tmpD3F=_tmpD3E;struct Cyc_Absyn_Exp*_tmpD43;struct Cyc_Absyn_Exp*_tmpD45;struct Cyc_Absyn_Exp*_tmpD47;struct _dyneither_ptr*_tmpD48;struct Cyc_Absyn_Exp*_tmpD4A;struct Cyc_Absyn_Exp*_tmpD4B;struct Cyc_Absyn_Exp*_tmpD4D;struct Cyc_Absyn_Exp*_tmpD4E;struct Cyc_Absyn_Exp*_tmpD50;struct Cyc_Absyn_Exp*_tmpD52;struct Cyc_Absyn_Stmt*_tmpD54;_LL781: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD40=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD40->tag != 1)goto _LL783;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD41=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpD40->f2);if(_tmpD41->tag != 1)goto _LL783;}}_LL782:
 return 0;_LL783: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD42=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD42->tag != 21)goto _LL785;else{_tmpD43=_tmpD42->f1;}}_LL784:
 _tmpD45=_tmpD43;goto _LL786;_LL785: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD44=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD44->tag != 19)goto _LL787;else{_tmpD45=_tmpD44->f1;}}_LL786:
# 5305
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpD45->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpD45);_LL787: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD46=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD46->tag != 20)goto _LL789;else{_tmpD47=_tmpD46->f1;_tmpD48=_tmpD46->f2;}}_LL788:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD47);_LL789: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD49=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD49->tag != 22)goto _LL78B;else{_tmpD4A=_tmpD49->f1;_tmpD4B=_tmpD49->f2;}}_LL78A: {
# 5309
void*_tmpD55=Cyc_Tcutil_compress((void*)_check_null(_tmpD4A->topt));void*_tmpD56=_tmpD55;_LL796: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD57=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD56;if(_tmpD57->tag != 10)goto _LL798;}_LL797:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD4A);_LL798:;_LL799:
 return 0;_LL795:;}_LL78B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpD4C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD4C->tag != 5)goto _LL78D;else{_tmpD4D=_tmpD4C->f2;_tmpD4E=_tmpD4C->f3;}}_LL78C:
# 5314
 return Cyc_Tcutil_is_noalias_path(r,_tmpD4D) && Cyc_Tcutil_is_noalias_path(r,_tmpD4E);_LL78D: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpD4F=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD4F->tag != 8)goto _LL78F;else{_tmpD50=_tmpD4F->f2;}}_LL78E:
 _tmpD52=_tmpD50;goto _LL790;_LL78F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpD51=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD51->tag != 13)goto _LL791;else{_tmpD52=_tmpD51->f2;}}_LL790:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD52);_LL791: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpD53=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpD3F;if(_tmpD53->tag != 36)goto _LL793;else{_tmpD54=_tmpD53->f1;}}_LL792:
# 5318
 while(1){
void*_tmpD58=_tmpD54->r;void*_tmpD59=_tmpD58;struct Cyc_Absyn_Stmt*_tmpD5B;struct Cyc_Absyn_Stmt*_tmpD5C;struct Cyc_Absyn_Decl*_tmpD5E;struct Cyc_Absyn_Stmt*_tmpD5F;struct Cyc_Absyn_Exp*_tmpD61;_LL79B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD5A=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD59;if(_tmpD5A->tag != 2)goto _LL79D;else{_tmpD5B=_tmpD5A->f1;_tmpD5C=_tmpD5A->f2;}}_LL79C:
 _tmpD54=_tmpD5C;goto _LL79A;_LL79D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD5D=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD59;if(_tmpD5D->tag != 12)goto _LL79F;else{_tmpD5E=_tmpD5D->f1;_tmpD5F=_tmpD5D->f2;}}_LL79E:
 _tmpD54=_tmpD5F;goto _LL79A;_LL79F: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD60=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD59;if(_tmpD60->tag != 1)goto _LL7A1;else{_tmpD61=_tmpD60->f1;}}_LL7A0:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD61);_LL7A1:;_LL7A2: {
const char*_tmp1421;void*_tmp1420;(_tmp1420=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1421="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp1421,sizeof(char),40))),_tag_dyneither(_tmp1420,sizeof(void*),0)));}_LL79A:;}_LL793:;_LL794:
# 5326
 return 1;_LL780:;}
# 5343 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5345
struct _tuple18 _tmp1422;struct _tuple18 bogus_ans=(_tmp1422.f1=0,((_tmp1422.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1422)));
void*_tmpD64=e->r;void*_tmpD65=_tmpD64;struct _tuple2*_tmpD67;void*_tmpD68;struct Cyc_Absyn_Exp*_tmpD6A;struct _dyneither_ptr*_tmpD6B;int _tmpD6C;struct Cyc_Absyn_Exp*_tmpD6E;struct _dyneither_ptr*_tmpD6F;int _tmpD70;struct Cyc_Absyn_Exp*_tmpD72;struct Cyc_Absyn_Exp*_tmpD74;struct Cyc_Absyn_Exp*_tmpD75;_LL7A4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD66=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD65;if(_tmpD66->tag != 1)goto _LL7A6;else{_tmpD67=_tmpD66->f1;_tmpD68=(void*)_tmpD66->f2;}}_LL7A5: {
# 5349
void*_tmpD76=_tmpD68;struct Cyc_Absyn_Vardecl*_tmpD7A;struct Cyc_Absyn_Vardecl*_tmpD7C;struct Cyc_Absyn_Vardecl*_tmpD7E;struct Cyc_Absyn_Vardecl*_tmpD80;_LL7B1: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD77=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD77->tag != 0)goto _LL7B3;}_LL7B2:
 goto _LL7B4;_LL7B3: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD78=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD78->tag != 2)goto _LL7B5;}_LL7B4:
 return bogus_ans;_LL7B5: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD79=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD79->tag != 1)goto _LL7B7;else{_tmpD7A=_tmpD79->f1;}}_LL7B6: {
# 5353
void*_tmpD81=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD82=_tmpD81;_LL7BE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD83=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD82;if(_tmpD83->tag != 8)goto _LL7C0;}_LL7BF: {
# 5355
struct _tuple18 _tmp1423;return(_tmp1423.f1=1,((_tmp1423.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1423)));}_LL7C0:;_LL7C1: {
struct _tuple18 _tmp1424;return(_tmp1424.f1=(_tmpD7A->tq).real_const,((_tmp1424.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp1424)));}_LL7BD:;}_LL7B7: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD7B=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD7B->tag != 4)goto _LL7B9;else{_tmpD7C=_tmpD7B->f1;}}_LL7B8: {
# 5359
void*_tmpD86=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD87=_tmpD86;_LL7C3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD88=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD87;if(_tmpD88->tag != 8)goto _LL7C5;}_LL7C4: {
struct _tuple18 _tmp1425;return(_tmp1425.f1=1,((_tmp1425.f2=(void*)_check_null(_tmpD7C->rgn),_tmp1425)));}_LL7C5:;_LL7C6:
# 5362
 _tmpD7C->escapes=1;{
struct _tuple18 _tmp1426;return(_tmp1426.f1=(_tmpD7C->tq).real_const,((_tmp1426.f2=(void*)_check_null(_tmpD7C->rgn),_tmp1426)));};_LL7C2:;}_LL7B9: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD7D=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD7D->tag != 5)goto _LL7BB;else{_tmpD7E=_tmpD7D->f1;}}_LL7BA:
# 5365
 _tmpD80=_tmpD7E;goto _LL7BC;_LL7BB: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD7F=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD76;if(_tmpD7F->tag != 3)goto _LL7B0;else{_tmpD80=_tmpD7F->f1;}}_LL7BC:
# 5367
 _tmpD80->escapes=1;{
struct _tuple18 _tmp1427;return(_tmp1427.f1=(_tmpD80->tq).real_const,((_tmp1427.f2=(void*)_check_null(_tmpD80->rgn),_tmp1427)));};_LL7B0:;}_LL7A6: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD69=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD65;if(_tmpD69->tag != 20)goto _LL7A8;else{_tmpD6A=_tmpD69->f1;_tmpD6B=_tmpD69->f2;_tmpD6C=_tmpD69->f3;}}_LL7A7:
# 5372
 if(_tmpD6C)return bogus_ans;{
# 5375
void*_tmpD8C=Cyc_Tcutil_compress((void*)_check_null(_tmpD6A->topt));void*_tmpD8D=_tmpD8C;struct Cyc_List_List*_tmpD8F;struct Cyc_Absyn_Aggrdecl*_tmpD91;_LL7C8: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD8E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD8D;if(_tmpD8E->tag != 12)goto _LL7CA;else{_tmpD8F=_tmpD8E->f2;}}_LL7C9: {
# 5377
struct Cyc_Absyn_Aggrfield*_tmpD92=Cyc_Absyn_lookup_field(_tmpD8F,_tmpD6B);
if(_tmpD92 != 0  && _tmpD92->width == 0){
struct _tuple18 _tmpD93=Cyc_Tcutil_addressof_props(te,_tmpD6A);int _tmpD95;void*_tmpD96;struct _tuple18 _tmpD94=_tmpD93;_tmpD95=_tmpD94.f1;_tmpD96=_tmpD94.f2;{
struct _tuple18 _tmp1428;return(_tmp1428.f1=(_tmpD92->tq).real_const  || _tmpD95,((_tmp1428.f2=_tmpD96,_tmp1428)));};}
# 5382
return bogus_ans;}_LL7CA: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD90=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD8D;if(_tmpD90->tag != 11)goto _LL7CC;else{if((((_tmpD90->f1).aggr_info).KnownAggr).tag != 2)goto _LL7CC;_tmpD91=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD90->f1).aggr_info).KnownAggr).val);}}_LL7CB: {
# 5384
struct Cyc_Absyn_Aggrfield*_tmpD98=Cyc_Absyn_lookup_decl_field(_tmpD91,_tmpD6B);
if(_tmpD98 != 0  && _tmpD98->width == 0){
struct _tuple18 _tmpD99=Cyc_Tcutil_addressof_props(te,_tmpD6A);int _tmpD9B;void*_tmpD9C;struct _tuple18 _tmpD9A=_tmpD99;_tmpD9B=_tmpD9A.f1;_tmpD9C=_tmpD9A.f2;{
struct _tuple18 _tmp1429;return(_tmp1429.f1=(_tmpD98->tq).real_const  || _tmpD9B,((_tmp1429.f2=_tmpD9C,_tmp1429)));};}
# 5389
return bogus_ans;}_LL7CC:;_LL7CD:
 return bogus_ans;_LL7C7:;};_LL7A8: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD6D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD65;if(_tmpD6D->tag != 21)goto _LL7AA;else{_tmpD6E=_tmpD6D->f1;_tmpD6F=_tmpD6D->f2;_tmpD70=_tmpD6D->f3;}}_LL7A9:
# 5394
 if(_tmpD70)return bogus_ans;{
# 5398
void*_tmpD9E=Cyc_Tcutil_compress((void*)_check_null(_tmpD6E->topt));void*_tmpD9F=_tmpD9E;void*_tmpDA1;void*_tmpDA2;_LL7CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDA0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD9F;if(_tmpDA0->tag != 5)goto _LL7D1;else{_tmpDA1=(_tmpDA0->f1).elt_typ;_tmpDA2=((_tmpDA0->f1).ptr_atts).rgn;}}_LL7D0: {
# 5400
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpDA3=Cyc_Tcutil_compress(_tmpDA1);void*_tmpDA4=_tmpDA3;struct Cyc_List_List*_tmpDA6;struct Cyc_Absyn_Aggrdecl*_tmpDA8;_LL7D4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDA5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDA4;if(_tmpDA5->tag != 12)goto _LL7D6;else{_tmpDA6=_tmpDA5->f2;}}_LL7D5:
# 5403
 finfo=Cyc_Absyn_lookup_field(_tmpDA6,_tmpD6F);goto _LL7D3;_LL7D6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDA7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDA4;if(_tmpDA7->tag != 11)goto _LL7D8;else{if((((_tmpDA7->f1).aggr_info).KnownAggr).tag != 2)goto _LL7D8;_tmpDA8=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDA7->f1).aggr_info).KnownAggr).val);}}_LL7D7:
# 5405
 finfo=Cyc_Absyn_lookup_decl_field(_tmpDA8,_tmpD6F);goto _LL7D3;_LL7D8:;_LL7D9:
 return bogus_ans;_LL7D3:;}
# 5408
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp142A;return(_tmp142A.f1=(finfo->tq).real_const,((_tmp142A.f2=_tmpDA2,_tmp142A)));}
return bogus_ans;}_LL7D1:;_LL7D2:
 return bogus_ans;_LL7CE:;};_LL7AA: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD71=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD65;if(_tmpD71->tag != 19)goto _LL7AC;else{_tmpD72=_tmpD71->f1;}}_LL7AB: {
# 5415
void*_tmpDAA=Cyc_Tcutil_compress((void*)_check_null(_tmpD72->topt));void*_tmpDAB=_tmpDAA;struct Cyc_Absyn_Tqual _tmpDAD;void*_tmpDAE;_LL7DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDAC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDAB;if(_tmpDAC->tag != 5)goto _LL7DD;else{_tmpDAD=(_tmpDAC->f1).elt_tq;_tmpDAE=((_tmpDAC->f1).ptr_atts).rgn;}}_LL7DC: {
# 5417
struct _tuple18 _tmp142B;return(_tmp142B.f1=_tmpDAD.real_const,((_tmp142B.f2=_tmpDAE,_tmp142B)));}_LL7DD:;_LL7DE:
 return bogus_ans;_LL7DA:;}_LL7AC: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD73=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD65;if(_tmpD73->tag != 22)goto _LL7AE;else{_tmpD74=_tmpD73->f1;_tmpD75=_tmpD73->f2;}}_LL7AD: {
# 5423
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD74->topt));
void*_tmpDB0=t;struct Cyc_List_List*_tmpDB2;struct Cyc_Absyn_Tqual _tmpDB4;void*_tmpDB5;struct Cyc_Absyn_Tqual _tmpDB7;_LL7E0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDB1=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDB0;if(_tmpDB1->tag != 10)goto _LL7E2;else{_tmpDB2=_tmpDB1->f1;}}_LL7E1: {
# 5427
struct _tuple14 _tmpDB8=Cyc_Evexp_eval_const_uint_exp(_tmpD75);unsigned int _tmpDBA;int _tmpDBB;struct _tuple14 _tmpDB9=_tmpDB8;_tmpDBA=_tmpDB9.f1;_tmpDBB=_tmpDB9.f2;
if(!_tmpDBB)
return bogus_ans;{
struct _tuple12*_tmpDBC=Cyc_Absyn_lookup_tuple_field(_tmpDB2,(int)_tmpDBA);
if(_tmpDBC != 0){
struct _tuple18 _tmp142C;return(_tmp142C.f1=((*_tmpDBC).f1).real_const,((_tmp142C.f2=(Cyc_Tcutil_addressof_props(te,_tmpD74)).f2,_tmp142C)));}
return bogus_ans;};}_LL7E2: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDB3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDB0;if(_tmpDB3->tag != 5)goto _LL7E4;else{_tmpDB4=(_tmpDB3->f1).elt_tq;_tmpDB5=((_tmpDB3->f1).ptr_atts).rgn;}}_LL7E3: {
# 5435
struct _tuple18 _tmp142D;return(_tmp142D.f1=_tmpDB4.real_const,((_tmp142D.f2=_tmpDB5,_tmp142D)));}_LL7E4: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDB6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDB0;if(_tmpDB6->tag != 8)goto _LL7E6;else{_tmpDB7=(_tmpDB6->f1).tq;}}_LL7E5: {
# 5441
struct _tuple18 _tmp142E;return(_tmp142E.f1=_tmpDB7.real_const,((_tmp142E.f2=(Cyc_Tcutil_addressof_props(te,_tmpD74)).f2,_tmp142E)));}_LL7E6:;_LL7E7:
 return bogus_ans;_LL7DF:;}_LL7AE:;_LL7AF:
# 5445
{const char*_tmp1431;void*_tmp1430;(_tmp1430=0,Cyc_Tcutil_terr(e->loc,((_tmp1431="unary & applied to non-lvalue",_tag_dyneither(_tmp1431,sizeof(char),30))),_tag_dyneither(_tmp1430,sizeof(void*),0)));}
return bogus_ans;_LL7A3:;}
# 5452
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpDC3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpDC4=_tmpDC3;struct Cyc_Absyn_Exp*_tmpDC7;_LL7E9: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDC5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDC4;if(_tmpDC5->tag != 0)goto _LL7EB;}_LL7EA:
 return;_LL7EB: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDC6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDC4;if(_tmpDC6->tag != 1)goto _LL7E8;else{_tmpDC7=_tmpDC6->f1;}}_LL7EC: {
# 5457
struct _tuple14 _tmpDC8=Cyc_Evexp_eval_const_uint_exp(_tmpDC7);unsigned int _tmpDCA;int _tmpDCB;struct _tuple14 _tmpDC9=_tmpDC8;_tmpDCA=_tmpDC9.f1;_tmpDCB=_tmpDC9.f2;
if(_tmpDCB  && _tmpDCA <= i){
const char*_tmp1436;void*_tmp1435[2];struct Cyc_Int_pa_PrintArg_struct _tmp1434;struct Cyc_Int_pa_PrintArg_struct _tmp1433;(_tmp1433.tag=1,((_tmp1433.f1=(unsigned long)((int)i),((_tmp1434.tag=1,((_tmp1434.f1=(unsigned long)((int)_tmpDCA),((_tmp1435[0]=& _tmp1434,((_tmp1435[1]=& _tmp1433,Cyc_Tcutil_terr(loc,((_tmp1436="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp1436,sizeof(char),39))),_tag_dyneither(_tmp1435,sizeof(void*),2)))))))))))));}
return;}_LL7E8:;};}
# 5464
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5468
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpDD0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpDD1=_tmpDD0;struct Cyc_Absyn_Exp*_tmpDD3;_LL7EE: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpDD2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpDD1;if(_tmpDD2->tag != 1)goto _LL7F0;else{_tmpDD3=_tmpDD2->f1;}}_LL7EF: {
# 5471
struct _tuple14 _tmpDD4=Cyc_Evexp_eval_const_uint_exp(_tmpDD3);unsigned int _tmpDD6;int _tmpDD7;struct _tuple14 _tmpDD5=_tmpDD4;_tmpDD6=_tmpDD5.f1;_tmpDD7=_tmpDD5.f2;
return _tmpDD7  && _tmpDD6 == 1;}_LL7F0:;_LL7F1:
 return 0;_LL7ED:;}
# 5477
int Cyc_Tcutil_bits_only(void*t){
void*_tmpDD8=Cyc_Tcutil_compress(t);void*_tmpDD9=_tmpDD8;void*_tmpDE0;union Cyc_Absyn_Constraint*_tmpDE1;struct Cyc_List_List*_tmpDE3;struct Cyc_Absyn_Aggrdecl*_tmpDE6;struct Cyc_List_List*_tmpDE7;struct Cyc_List_List*_tmpDE9;_LL7F3: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDDA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDA->tag != 0)goto _LL7F5;}_LL7F4:
 goto _LL7F6;_LL7F5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDDB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDB->tag != 6)goto _LL7F7;}_LL7F6:
 goto _LL7F8;_LL7F7: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDDC=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDC->tag != 7)goto _LL7F9;}_LL7F8:
 return 1;_LL7F9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDDD=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDD->tag != 13)goto _LL7FB;}_LL7FA:
 goto _LL7FC;_LL7FB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDDE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDE->tag != 14)goto _LL7FD;}_LL7FC:
 return 0;_LL7FD: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDDF=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD9;if(_tmpDDF->tag != 8)goto _LL7FF;else{_tmpDE0=(_tmpDDF->f1).elt_type;_tmpDE1=(_tmpDDF->f1).zero_term;}}_LL7FE:
# 5487
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpDE1) && Cyc_Tcutil_bits_only(_tmpDE0);_LL7FF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDE2=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDD9;if(_tmpDE2->tag != 10)goto _LL801;else{_tmpDE3=_tmpDE2->f1;}}_LL800:
# 5489
 for(0;_tmpDE3 != 0;_tmpDE3=_tmpDE3->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpDE3->hd)).f2))return 0;}
return 1;_LL801: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDD9;if(_tmpDE4->tag != 11)goto _LL803;else{if((((_tmpDE4->f1).aggr_info).UnknownAggr).tag != 1)goto _LL803;}}_LL802:
 return 0;_LL803: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE5=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDD9;if(_tmpDE5->tag != 11)goto _LL805;else{if((((_tmpDE5->f1).aggr_info).KnownAggr).tag != 2)goto _LL805;_tmpDE6=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpDE5->f1).aggr_info).KnownAggr).val);_tmpDE7=(_tmpDE5->f1).targs;}}_LL804:
# 5494
 if(_tmpDE6->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpDEA=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDEA;_push_region(rgn);
{struct Cyc_List_List*_tmpDEB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpDE6->tvs,_tmpDE7);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDE6->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpDEB,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpDEC=0;_npop_handler(0);return _tmpDEC;}}}{
int _tmpDED=1;_npop_handler(0);return _tmpDED;};}
# 5501
;_pop_region(rgn);};};_LL805: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDE8=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDD9;if(_tmpDE8->tag != 12)goto _LL807;else{_tmpDE9=_tmpDE8->f2;}}_LL806:
# 5507
 for(0;_tmpDE9 != 0;_tmpDE9=_tmpDE9->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpDE9->hd)->type))return 0;}
return 1;_LL807:;_LL808:
 return 0;_LL7F2:;}
# 5515
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpDEE=e->r;void*_tmpDEF=_tmpDEE;struct Cyc_Absyn_Exp*_tmpDF2;struct Cyc_Absyn_Exp*_tmpDF4;_LL80A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDF0=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDEF;if(_tmpDF0->tag != 1)goto _LL80C;}_LL80B:
 return 1;_LL80C: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpDF1=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDEF;if(_tmpDF1->tag != 11)goto _LL80E;else{_tmpDF2=_tmpDF1->f1;}}_LL80D:
 _tmpDF4=_tmpDF2;goto _LL80F;_LL80E: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpDF3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDEF;if(_tmpDF3->tag != 12)goto _LL810;else{_tmpDF4=_tmpDF3->f1;}}_LL80F:
 e=_tmpDF4;continue;_LL810:;_LL811:
# 5522
 return 0;_LL809:;}}
# 5532
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpDF5=e->r;void*_tmpDF6=_tmpDF5;struct _tuple2*_tmpDFE;void*_tmpDFF;struct Cyc_Absyn_Exp*_tmpE01;struct Cyc_Absyn_Exp*_tmpE02;struct Cyc_Absyn_Exp*_tmpE03;struct Cyc_Absyn_Exp*_tmpE05;struct Cyc_Absyn_Exp*_tmpE06;struct Cyc_Absyn_Exp*_tmpE08;struct Cyc_Absyn_Exp*_tmpE0A;void*_tmpE0C;struct Cyc_Absyn_Exp*_tmpE0D;void*_tmpE0F;struct Cyc_Absyn_Exp*_tmpE10;struct Cyc_Absyn_Exp*_tmpE12;struct Cyc_Absyn_Exp*_tmpE14;struct Cyc_Absyn_Exp*_tmpE15;struct Cyc_Absyn_Exp*_tmpE17;struct Cyc_List_List*_tmpE19;struct Cyc_List_List*_tmpE1B;struct Cyc_List_List*_tmpE1D;enum Cyc_Absyn_Primop _tmpE1F;struct Cyc_List_List*_tmpE20;struct Cyc_List_List*_tmpE22;struct Cyc_List_List*_tmpE24;_LL813: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpDF7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDF7->tag != 0)goto _LL815;}_LL814:
 goto _LL816;_LL815: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpDF8=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDF8->tag != 16)goto _LL817;}_LL816:
 goto _LL818;_LL817: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDF9=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDF9->tag != 17)goto _LL819;}_LL818:
 goto _LL81A;_LL819: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDFA=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDFA->tag != 18)goto _LL81B;}_LL81A:
 goto _LL81C;_LL81B: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDFB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDFB->tag != 31)goto _LL81D;}_LL81C:
 goto _LL81E;_LL81D: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDFC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDFC->tag != 32)goto _LL81F;}_LL81E:
 return 1;_LL81F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDFD=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpDFD->tag != 1)goto _LL821;else{_tmpDFE=_tmpDFD->f1;_tmpDFF=(void*)_tmpDFD->f2;}}_LL820: {
# 5543
void*_tmpE25=_tmpDFF;struct Cyc_Absyn_Vardecl*_tmpE28;struct Cyc_Absyn_Vardecl*_tmpE2A;_LL842: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpE26=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpE25;if(_tmpE26->tag != 2)goto _LL844;}_LL843:
 return 1;_LL844: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE27=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE25;if(_tmpE27->tag != 1)goto _LL846;else{_tmpE28=_tmpE27->f1;}}_LL845: {
# 5546
void*_tmpE2C=Cyc_Tcutil_compress(_tmpE28->type);void*_tmpE2D=_tmpE2C;_LL84D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE2E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE2D;if(_tmpE2E->tag != 8)goto _LL84F;}_LL84E:
 goto _LL850;_LL84F: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE2F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE2D;if(_tmpE2F->tag != 9)goto _LL851;}_LL850:
 return 1;_LL851:;_LL852:
 return var_okay;_LL84C:;}_LL846: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE29=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE25;if(_tmpE29->tag != 4)goto _LL848;else{_tmpE2A=_tmpE29->f1;}}_LL847:
# 5553
 if(_tmpE2A->sc == Cyc_Absyn_Static){
void*_tmpE30=Cyc_Tcutil_compress(_tmpE2A->type);void*_tmpE31=_tmpE30;_LL854: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE32=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE31;if(_tmpE32->tag != 8)goto _LL856;}_LL855:
 goto _LL857;_LL856: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE33=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE31;if(_tmpE33->tag != 9)goto _LL858;}_LL857:
 return 1;_LL858:;_LL859:
 return var_okay;_LL853:;}else{
# 5560
return var_okay;}_LL848: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpE2B=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpE25;if(_tmpE2B->tag != 0)goto _LL84A;}_LL849:
 return 0;_LL84A:;_LL84B:
 return var_okay;_LL841:;}_LL821: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpE00=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE00->tag != 5)goto _LL823;else{_tmpE01=_tmpE00->f1;_tmpE02=_tmpE00->f2;_tmpE03=_tmpE00->f3;}}_LL822:
# 5566
 return(Cyc_Tcutil_cnst_exp(0,_tmpE01) && 
Cyc_Tcutil_cnst_exp(0,_tmpE02)) && 
Cyc_Tcutil_cnst_exp(0,_tmpE03);_LL823: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpE04=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE04->tag != 8)goto _LL825;else{_tmpE05=_tmpE04->f1;_tmpE06=_tmpE04->f2;}}_LL824:
# 5570
 return Cyc_Tcutil_cnst_exp(0,_tmpE05) && Cyc_Tcutil_cnst_exp(0,_tmpE06);_LL825: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpE07=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE07->tag != 11)goto _LL827;else{_tmpE08=_tmpE07->f1;}}_LL826:
 _tmpE0A=_tmpE08;goto _LL828;_LL827: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpE09=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE09->tag != 12)goto _LL829;else{_tmpE0A=_tmpE09->f1;}}_LL828:
# 5573
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE0A);_LL829: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE0B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE0B->tag != 13)goto _LL82B;else{_tmpE0C=(void*)_tmpE0B->f1;_tmpE0D=_tmpE0B->f2;if(_tmpE0B->f4 != Cyc_Absyn_No_coercion)goto _LL82B;}}_LL82A:
# 5575
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE0D);_LL82B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpE0E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE0E->tag != 13)goto _LL82D;else{_tmpE0F=(void*)_tmpE0E->f1;_tmpE10=_tmpE0E->f2;}}_LL82C:
# 5578
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpE10);_LL82D: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpE11=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE11->tag != 14)goto _LL82F;else{_tmpE12=_tmpE11->f1;}}_LL82E:
# 5580
 return Cyc_Tcutil_cnst_exp(1,_tmpE12);_LL82F: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE13=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE13->tag != 26)goto _LL831;else{_tmpE14=_tmpE13->f2;_tmpE15=_tmpE13->f3;}}_LL830:
# 5582
 return Cyc_Tcutil_cnst_exp(0,_tmpE14) && Cyc_Tcutil_cnst_exp(0,_tmpE15);_LL831: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE16=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE16->tag != 27)goto _LL833;else{_tmpE17=_tmpE16->f1;}}_LL832:
# 5584
 return Cyc_Tcutil_cnst_exp(0,_tmpE17);_LL833: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE18=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE18->tag != 25)goto _LL835;else{_tmpE19=_tmpE18->f1;}}_LL834:
 _tmpE1B=_tmpE19;goto _LL836;_LL835: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE1A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE1A->tag != 29)goto _LL837;else{_tmpE1B=_tmpE1A->f2;}}_LL836:
 _tmpE1D=_tmpE1B;goto _LL838;_LL837: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE1C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE1C->tag != 28)goto _LL839;else{_tmpE1D=_tmpE1C->f3;}}_LL838:
# 5588
 for(0;_tmpE1D != 0;_tmpE1D=_tmpE1D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpE1D->hd)).f2))
return 0;}
return 1;_LL839: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpE1E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE1E->tag != 2)goto _LL83B;else{_tmpE1F=_tmpE1E->f1;_tmpE20=_tmpE1E->f2;}}_LL83A:
# 5593
 _tmpE22=_tmpE20;goto _LL83C;_LL83B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpE21=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE21->tag != 23)goto _LL83D;else{_tmpE22=_tmpE21->f1;}}_LL83C:
 _tmpE24=_tmpE22;goto _LL83E;_LL83D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE23=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDF6;if(_tmpE23->tag != 30)goto _LL83F;else{_tmpE24=_tmpE23->f1;}}_LL83E:
# 5596
 for(0;_tmpE24 != 0;_tmpE24=_tmpE24->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpE24->hd))
return 0;}
return 1;_LL83F:;_LL840:
 return 0;_LL812:;}
# 5604
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5608
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5610
int Cyc_Tcutil_supports_default(void*t){
void*_tmpE34=Cyc_Tcutil_compress(t);void*_tmpE35=_tmpE34;union Cyc_Absyn_Constraint*_tmpE3A;union Cyc_Absyn_Constraint*_tmpE3B;void*_tmpE3D;struct Cyc_List_List*_tmpE3F;union Cyc_Absyn_AggrInfoU _tmpE41;struct Cyc_List_List*_tmpE42;struct Cyc_List_List*_tmpE44;_LL85B: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpE36=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpE35;if(_tmpE36->tag != 0)goto _LL85D;}_LL85C:
 goto _LL85E;_LL85D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE37=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE35;if(_tmpE37->tag != 6)goto _LL85F;}_LL85E:
 goto _LL860;_LL85F: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE38=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE35;if(_tmpE38->tag != 7)goto _LL861;}_LL860:
 return 1;_LL861: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE39=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE35;if(_tmpE39->tag != 5)goto _LL863;else{_tmpE3A=((_tmpE39->f1).ptr_atts).nullable;_tmpE3B=((_tmpE39->f1).ptr_atts).bounds;}}_LL862: {
# 5617
void*_tmpE47=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE3B);void*_tmpE48=_tmpE47;_LL872: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE49=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE48;if(_tmpE49->tag != 0)goto _LL874;}_LL873:
 return 1;_LL874:;_LL875:
# 5620
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpE3A);_LL871:;}_LL863: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE3C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE35;if(_tmpE3C->tag != 8)goto _LL865;else{_tmpE3D=(_tmpE3C->f1).elt_type;}}_LL864:
# 5623
 return Cyc_Tcutil_supports_default(_tmpE3D);_LL865: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpE3E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpE35;if(_tmpE3E->tag != 10)goto _LL867;else{_tmpE3F=_tmpE3E->f1;}}_LL866:
# 5625
 for(0;_tmpE3F != 0;_tmpE3F=_tmpE3F->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpE3F->hd)).f2))return 0;}
return 1;_LL867: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE40=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpE35;if(_tmpE40->tag != 11)goto _LL869;else{_tmpE41=(_tmpE40->f1).aggr_info;_tmpE42=(_tmpE40->f1).targs;}}_LL868: {
# 5629
struct Cyc_Absyn_Aggrdecl*_tmpE4A=Cyc_Absyn_get_known_aggrdecl(_tmpE41);
if(_tmpE4A->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE4A->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpE4A->tvs,_tmpE42,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpE4A->impl))->fields);}_LL869: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpE43=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpE35;if(_tmpE43->tag != 12)goto _LL86B;else{_tmpE44=_tmpE43->f2;}}_LL86A:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpE44);_LL86B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpE45=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpE35;if(_tmpE45->tag != 14)goto _LL86D;}_LL86C:
# 5635
 goto _LL86E;_LL86D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpE46=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpE35;if(_tmpE46->tag != 13)goto _LL86F;}_LL86E:
 return 1;_LL86F:;_LL870:
# 5638
 return 0;_LL85A:;}
# 5643
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpE4B=t;struct Cyc_Absyn_Typedefdecl*_tmpE4D;_LL877: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpE4C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpE4B;if(_tmpE4C->tag != 17)goto _LL879;else{_tmpE4D=_tmpE4C->f3;}}_LL878:
# 5646
 if(_tmpE4D != 0){
struct Cyc_Absyn_Tqual _tmpE4E=_tmpE4D->tq;
if(((_tmpE4E.print_const  || _tmpE4E.q_volatile) || _tmpE4E.q_restrict) || _tmpE4E.real_const){
# 5651
const char*_tmp143A;void*_tmp1439[1];struct Cyc_String_pa_PrintArg_struct _tmp1438;(_tmp1438.tag=0,((_tmp1438.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1439[0]=& _tmp1438,Cyc_Tcutil_warn(loc,((_tmp143A="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp143A,sizeof(char),44))),_tag_dyneither(_tmp1439,sizeof(void*),1)))))));}}
# 5654
goto _LL876;_LL879:;_LL87A:
 goto _LL876;_LL876:;}
# 5659
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5661
struct _RegionHandle _tmpE52=_new_region("rgn");struct _RegionHandle*rgn=& _tmpE52;_push_region(rgn);{
struct Cyc_List_List*_tmpE53=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpE54=1;_npop_handler(0);return _tmpE54;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpE53,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpE55=0;_npop_handler(0);return _tmpE55;}}}{
# 5670
int _tmpE56=1;_npop_handler(0);return _tmpE56;};
# 5661
;_pop_region(rgn);}
# 5676
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpE57=Cyc_Tcutil_compress(t);void*_tmpE58=_tmpE57;union Cyc_Absyn_Constraint*_tmpE5C;union Cyc_Absyn_Constraint*_tmpE5D;_LL87C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE59=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpE58;if(_tmpE59->tag != 6)goto _LL87E;}_LL87D:
 goto _LL87F;_LL87E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE5A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpE58;if(_tmpE5A->tag != 7)goto _LL880;}_LL87F:
 return 1;_LL880: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE5B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE58;if(_tmpE5B->tag != 5)goto _LL882;else{_tmpE5C=((_tmpE5B->f1).ptr_atts).nullable;_tmpE5D=((_tmpE5B->f1).ptr_atts).bounds;}}_LL881: {
# 5681
void*_tmpE5E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE5D);void*_tmpE5F=_tmpE5E;_LL885: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE60=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE5F;if(_tmpE60->tag != 0)goto _LL887;}_LL886:
# 5685
 return 0;_LL887:;_LL888:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE5C);_LL884:;}_LL882:;_LL883:
# 5688
 return 0;_LL87B:;}
# 5692
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE61=Cyc_Tcutil_compress(t);void*_tmpE62=_tmpE61;void*_tmpE64;struct Cyc_List_List*_tmpE66;_LL88A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE63=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE62;if(_tmpE63->tag != 5)goto _LL88C;else{_tmpE64=(_tmpE63->f1).elt_typ;}}_LL88B:
 return Cyc_Tcutil_is_noreturn(_tmpE64);_LL88C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE65=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE62;if(_tmpE65->tag != 9)goto _LL88E;else{_tmpE66=(_tmpE65->f1).attributes;}}_LL88D:
# 5696
 for(0;_tmpE66 != 0;_tmpE66=_tmpE66->tl){
void*_tmpE67=(void*)_tmpE66->hd;void*_tmpE68=_tmpE67;_LL891: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE69=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE68;if(_tmpE69->tag != 4)goto _LL893;}_LL892:
 return 1;_LL893:;_LL894:
 continue;_LL890:;}
# 5701
goto _LL889;_LL88E:;_LL88F:
 goto _LL889;_LL889:;}
# 5704
return 0;}
# 5709
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE6A=Cyc_Tcutil_compress(t);void*_tmpE6B=_tmpE6A;struct Cyc_List_List**_tmpE6D;_LL896: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE6C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE6B;if(_tmpE6C->tag != 9)goto _LL898;else{_tmpE6D=(struct Cyc_List_List**)&(_tmpE6C->f1).attributes;}}_LL897: {
# 5712
struct Cyc_List_List*_tmpE6E=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE6D,(void*)atts->hd)){
struct Cyc_List_List*_tmp143B;*_tmpE6D=((_tmp143B=_cycalloc(sizeof(*_tmp143B)),((_tmp143B->hd=(void*)atts->hd,((_tmp143B->tl=*_tmpE6D,_tmp143B))))));}}else{
# 5719
struct Cyc_List_List*_tmp143C;_tmpE6E=((_tmp143C=_cycalloc(sizeof(*_tmp143C)),((_tmp143C->hd=(void*)atts->hd,((_tmp143C->tl=_tmpE6E,_tmp143C))))));}}
return _tmpE6E;}_LL898:;_LL899: {
const char*_tmp143F;void*_tmp143E;(_tmp143E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp143F="transfer_fn_type_atts",_tag_dyneither(_tmp143F,sizeof(char),22))),_tag_dyneither(_tmp143E,sizeof(void*),0)));}_LL895:;}
# 5726
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE73=Cyc_Tcutil_compress(t);void*_tmpE74=_tmpE73;struct Cyc_Absyn_PtrInfo*_tmpE76;struct Cyc_Absyn_Exp*_tmpE78;_LL89B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE75=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE74;if(_tmpE75->tag != 5)goto _LL89D;else{_tmpE76=(struct Cyc_Absyn_PtrInfo*)& _tmpE75->f1;}}_LL89C:
# 5730
{void*_tmpE79=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE76->ptr_atts).bounds);void*_tmpE7A=_tmpE79;struct Cyc_Absyn_Exp*_tmpE7C;_LL8A2: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE7B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE7A;if(_tmpE7B->tag != 1)goto _LL8A4;else{_tmpE7C=_tmpE7B->f1;}}_LL8A3:
 bound=_tmpE7C;goto _LL8A1;_LL8A4:;_LL8A5:
 goto _LL8A1;_LL8A1:;}
# 5734
goto _LL89A;_LL89D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE77=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE74;if(_tmpE77->tag != 8)goto _LL89F;else{_tmpE78=(_tmpE77->f1).num_elts;}}_LL89E:
# 5736
 bound=_tmpE78;
goto _LL89A;_LL89F:;_LL8A0:
 goto _LL89A;_LL89A:;}
# 5740
return bound;}
# 5745
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE7D=b;struct Cyc_Absyn_Vardecl*_tmpE7F;struct Cyc_Absyn_Vardecl*_tmpE81;struct Cyc_Absyn_Vardecl*_tmpE83;struct Cyc_Absyn_Vardecl*_tmpE85;_LL8A7: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE7E=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE7D;if(_tmpE7E->tag != 5)goto _LL8A9;else{_tmpE7F=_tmpE7E->f1;}}_LL8A8:
 _tmpE81=_tmpE7F;goto _LL8AA;_LL8A9: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE80=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE7D;if(_tmpE80->tag != 4)goto _LL8AB;else{_tmpE81=_tmpE80->f1;}}_LL8AA:
 _tmpE83=_tmpE81;goto _LL8AC;_LL8AB: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE82=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE7D;if(_tmpE82->tag != 3)goto _LL8AD;else{_tmpE83=_tmpE82->f1;}}_LL8AC:
 _tmpE85=_tmpE83;goto _LL8AE;_LL8AD: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE84=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE7D;if(_tmpE84->tag != 1)goto _LL8AF;else{_tmpE85=_tmpE84->f1;}}_LL8AE:
# 5751
 if(!_tmpE85->escapes)return _tmpE85;
goto _LL8A6;_LL8AF:;_LL8B0:
 goto _LL8A6;_LL8A6:;}
# 5755
return 0;}
# 5759
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE86=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp1440;_tmpE86=((_tmp1440=_cycalloc(sizeof(*_tmp1440)),((_tmp1440->hd=*((void**)_check_null((void**)x->hd)),((_tmp1440->tl=_tmpE86,_tmp1440))))));}}}
return _tmpE86;}
# 5766
int Cyc_Tcutil_is_array(void*t){
# 5768
void*_tmpE88=Cyc_Tcutil_compress(t);void*_tmpE89=_tmpE88;_LL8B2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE8A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE89;if(_tmpE8A->tag != 8)goto _LL8B4;}_LL8B3:
# 5770
 return 1;_LL8B4:;_LL8B5:
 return 0;_LL8B1:;}
# 5775
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5777
void*_tmpE8B=Cyc_Tcutil_compress(t);void*_tmpE8C=_tmpE8B;void*_tmpE8E;struct Cyc_Absyn_Tqual _tmpE8F;struct Cyc_Absyn_Exp*_tmpE90;union Cyc_Absyn_Constraint*_tmpE91;unsigned int _tmpE92;_LL8B7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE8D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE8C;if(_tmpE8D->tag != 8)goto _LL8B9;else{_tmpE8E=(_tmpE8D->f1).elt_type;_tmpE8F=(_tmpE8D->f1).tq;_tmpE90=(_tmpE8D->f1).num_elts;_tmpE91=(_tmpE8D->f1).zero_term;_tmpE92=(_tmpE8D->f1).zt_loc;}}_LL8B8: {
# 5779
void*b;
if(_tmpE90 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5783
struct Cyc_Absyn_Exp*bnd=_tmpE90;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp1443;void*_tmp1442;(_tmp1442=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1443="cannot convert tag without type!",_tag_dyneither(_tmp1443,sizeof(char),33))),_tag_dyneither(_tmp1442,sizeof(void*),0)));}{
void*_tmpE95=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpE96=_tmpE95;void*_tmpE98;_LL8BC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE97=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE96;if(_tmpE97->tag != 19)goto _LL8BE;else{_tmpE98=(void*)_tmpE97->f1;}}_LL8BD:
# 5789
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1446;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1445;b=(void*)((_tmp1445=_cycalloc(sizeof(*_tmp1445)),((_tmp1445[0]=((_tmp1446.tag=1,((_tmp1446.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpE98,0),0,Cyc_Absyn_No_coercion,0),_tmp1446)))),_tmp1445))));}
goto _LL8BB;_LL8BE:;_LL8BF:
# 5792
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp1449;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp1448;b=(void*)((_tmp1448=_cycalloc(sizeof(*_tmp1448)),((_tmp1448[0]=((_tmp1449.tag=1,((_tmp1449.f1=bnd,_tmp1449)))),_tmp1448))));}else{
# 5795
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL8BB:;};}else{
# 5799
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp144C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp144B;b=(void*)((_tmp144B=_cycalloc(sizeof(*_tmp144B)),((_tmp144B[0]=((_tmp144C.tag=1,((_tmp144C.f1=bnd,_tmp144C)))),_tmp144B))));}}
# 5801
return Cyc_Absyn_atb_typ(_tmpE8E,rgn,_tmpE8F,b,_tmpE91);}_LL8B9:;_LL8BA:
# 5804
 return t;_LL8B6:;}
