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
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 120 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 131
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 245
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 911
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 919
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 922
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 926
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 929
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 931
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 937
void*Cyc_Absyn_compress_kb(void*);
# 942
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 947
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 949
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 953
extern void*Cyc_Absyn_empty_effect;
# 963
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 972
extern void*Cyc_Absyn_bounds_one;
# 977
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple2*,void*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1105
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1150
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1152
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1154
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1156
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1158
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1162
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1164
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
# 125 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 129
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 132
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 134
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 151
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 218
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
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 46 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 48
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 52
int Cyc_Tcutil_warn_region_coerce=0;
# 55
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 60
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 64
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 69
{const char*_tmpE46;if(Cyc_strcmp(((_tmpE46="(qualifiers don't match)",_tag_dyneither(_tmpE46,sizeof(char),25))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
{const char*_tmpE4A;void*_tmpE49[1];struct Cyc_String_pa_PrintArg_struct _tmpE48;(_tmpE48.tag=0,((_tmpE48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpE49[0]=& _tmpE48,Cyc_fprintf(Cyc_stderr,((_tmpE4A="  %s\n",_tag_dyneither(_tmpE4A,sizeof(char),6))),_tag_dyneither(_tmpE49,sizeof(void*),1)))))));}
return;}}
# 74
{const char*_tmpE4B;if(Cyc_strcmp(((_tmpE4B="(function effects do not match)",_tag_dyneither(_tmpE4B,sizeof(char),32))),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}}{
# 79
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
{const char*_tmpE4F;void*_tmpE4E[1];struct Cyc_String_pa_PrintArg_struct _tmpE4D;(_tmpE4D.tag=0,((_tmpE4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1),((_tmpE4E[0]=& _tmpE4D,Cyc_fprintf(Cyc_stderr,((_tmpE4F="  %s",_tag_dyneither(_tmpE4F,sizeof(char),5))),_tag_dyneither(_tmpE4E,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
{const char*_tmpE52;void*_tmpE51;(_tmpE51=0,Cyc_fprintf(Cyc_stderr,((_tmpE52="\n\t",_tag_dyneither(_tmpE52,sizeof(char),3))),_tag_dyneither(_tmpE51,sizeof(void*),0)));}
pos=8;}else{
# 88
{const char*_tmpE55;void*_tmpE54;(_tmpE54=0,Cyc_fprintf(Cyc_stderr,((_tmpE55=" ",_tag_dyneither(_tmpE55,sizeof(char),2))),_tag_dyneither(_tmpE54,sizeof(void*),0)));}
++ pos;}
# 91
{const char*_tmpE58;void*_tmpE57;(_tmpE57=0,Cyc_fprintf(Cyc_stderr,((_tmpE58="and ",_tag_dyneither(_tmpE58,sizeof(char),5))),_tag_dyneither(_tmpE57,sizeof(void*),0)));}
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
{const char*_tmpE5B;void*_tmpE5A;(_tmpE5A=0,Cyc_fprintf(Cyc_stderr,((_tmpE5B="\n\t",_tag_dyneither(_tmpE5B,sizeof(char),3))),_tag_dyneither(_tmpE5A,sizeof(void*),0)));}
pos=8;}
# 97
{const char*_tmpE5F;void*_tmpE5E[1];struct Cyc_String_pa_PrintArg_struct _tmpE5D;(_tmpE5D.tag=0,((_tmpE5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2),((_tmpE5E[0]=& _tmpE5D,Cyc_fprintf(Cyc_stderr,((_tmpE5F="%s ",_tag_dyneither(_tmpE5F,sizeof(char),4))),_tag_dyneither(_tmpE5E,sizeof(void*),1)))))));}
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
{const char*_tmpE62;void*_tmpE61;(_tmpE61=0,Cyc_fprintf(Cyc_stderr,((_tmpE62="\n\t",_tag_dyneither(_tmpE62,sizeof(char),3))),_tag_dyneither(_tmpE61,sizeof(void*),0)));}
pos=8;}
# 103
{const char*_tmpE65;void*_tmpE64;(_tmpE64=0,Cyc_fprintf(Cyc_stderr,((_tmpE65="are not compatible. ",_tag_dyneither(_tmpE65,sizeof(char),21))),_tag_dyneither(_tmpE64,sizeof(void*),0)));}
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80){
const char*_tmpE68;void*_tmpE67;(_tmpE67=0,Cyc_fprintf(Cyc_stderr,((_tmpE68="\n\t",_tag_dyneither(_tmpE68,sizeof(char),3))),_tag_dyneither(_tmpE67,sizeof(void*),0)));}{
# 109
const char*_tmpE6C;void*_tmpE6B[1];struct Cyc_String_pa_PrintArg_struct _tmpE6A;(_tmpE6A.tag=0,((_tmpE6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason),((_tmpE6B[0]=& _tmpE6A,Cyc_fprintf(Cyc_stderr,((_tmpE6C="%s",_tag_dyneither(_tmpE6C,sizeof(char),3))),_tag_dyneither(_tmpE6B,sizeof(void*),1)))))));};}
# 111
{const char*_tmpE6F;void*_tmpE6E;(_tmpE6E=0,Cyc_fprintf(Cyc_stderr,((_tmpE6F="\n",_tag_dyneither(_tmpE6F,sizeof(char),2))),_tag_dyneither(_tmpE6E,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 115
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 118
Cyc_Position_post_error(Cyc_Position_mk_err_elab(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 121
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 124
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{const char*_tmpE73;void*_tmpE72[1];struct Cyc_String_pa_PrintArg_struct _tmpE71;(_tmpE71.tag=0,((_tmpE71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmpE72[0]=& _tmpE71,Cyc_fprintf(Cyc_stderr,((_tmpE73="Compiler Error (Tcutil::impos): %s\n",_tag_dyneither(_tmpE73,sizeof(char),36))),_tag_dyneither(_tmpE72,sizeof(void*),1)))))));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Core_Impossible_exn_struct _tmpE76;struct Cyc_Core_Impossible_exn_struct*_tmpE75;(int)_throw((void*)((_tmpE75=_cycalloc(sizeof(*_tmpE75)),((_tmpE75[0]=((_tmpE76.tag=Cyc_Core_Impossible,((_tmpE76.f1=msg,_tmpE76)))),_tmpE75)))));};}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 132
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 136
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
const char*_tmpE7B;void*_tmpE7A[2];struct Cyc_String_pa_PrintArg_struct _tmpE79;struct Cyc_String_pa_PrintArg_struct _tmpE78;(_tmpE78.tag=0,((_tmpE78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind)),((_tmpE79.tag=0,((_tmpE79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd)),((_tmpE7A[0]=& _tmpE79,((_tmpE7A[1]=& _tmpE78,Cyc_fprintf(Cyc_stderr,((_tmpE7B="%s::%s ",_tag_dyneither(_tmpE7B,sizeof(char),8))),_tag_dyneither(_tmpE7A,sizeof(void*),2)))))))))))));}
{const char*_tmpE7E;void*_tmpE7D;(_tmpE7D=0,Cyc_fprintf(Cyc_stderr,((_tmpE7E="\n",_tag_dyneither(_tmpE7E,sizeof(char),2))),_tag_dyneither(_tmpE7D,sizeof(void*),0)));}Cyc_fflush(Cyc_stderr);}
# 143
static struct Cyc_List_List*Cyc_Tcutil_warning_segs=0;
static struct Cyc_List_List*Cyc_Tcutil_warning_msgs=0;
# 148
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 151
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
{struct Cyc_List_List*_tmpE7F;Cyc_Tcutil_warning_segs=((_tmpE7F=_cycalloc(sizeof(*_tmpE7F)),((_tmpE7F->hd=(void*)sg,((_tmpE7F->tl=Cyc_Tcutil_warning_segs,_tmpE7F))))));}{
struct _dyneither_ptr*_tmpE82;struct Cyc_List_List*_tmpE81;Cyc_Tcutil_warning_msgs=((_tmpE81=_cycalloc(sizeof(*_tmpE81)),((_tmpE81->hd=((_tmpE82=_cycalloc(sizeof(*_tmpE82)),((_tmpE82[0]=msg,_tmpE82)))),((_tmpE81->tl=Cyc_Tcutil_warning_msgs,_tmpE81))))));};}
# 155
void Cyc_Tcutil_flush_warnings(){
if(Cyc_Tcutil_warning_segs == 0)
return;
{const char*_tmpE85;void*_tmpE84;(_tmpE84=0,Cyc_fprintf(Cyc_stderr,((_tmpE85="***Warnings***\n",_tag_dyneither(_tmpE85,sizeof(char),16))),_tag_dyneither(_tmpE84,sizeof(void*),0)));}{
struct Cyc_List_List*_tmp30=Cyc_Position_strings_of_segments(Cyc_Tcutil_warning_segs);
Cyc_Tcutil_warning_segs=0;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Tcutil_warning_msgs);
while(Cyc_Tcutil_warning_msgs != 0){
{const char*_tmpE8A;void*_tmpE89[2];struct Cyc_String_pa_PrintArg_struct _tmpE88;struct Cyc_String_pa_PrintArg_struct _tmpE87;(_tmpE87.tag=0,((_tmpE87.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->hd)),((_tmpE88.tag=0,((_tmpE88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp30))->hd)),((_tmpE89[0]=& _tmpE88,((_tmpE89[1]=& _tmpE87,Cyc_fprintf(Cyc_stderr,((_tmpE8A="%s: %s\n",_tag_dyneither(_tmpE8A,sizeof(char),8))),_tag_dyneither(_tmpE89,sizeof(void*),2)))))))))))));}
_tmp30=_tmp30->tl;
Cyc_Tcutil_warning_msgs=((struct Cyc_List_List*)_check_null(Cyc_Tcutil_warning_msgs))->tl;}
# 167
{const char*_tmpE8D;void*_tmpE8C;(_tmpE8C=0,Cyc_fprintf(Cyc_stderr,((_tmpE8D="**************\n",_tag_dyneither(_tmpE8D,sizeof(char),16))),_tag_dyneither(_tmpE8C,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);};}
# 172
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 176
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 181
void*Cyc_Tcutil_compress(void*t){
void*_tmp37=t;void**_tmp3B;void**_tmp3D;struct Cyc_Absyn_Exp*_tmp3F;void*_tmp41;_LL1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp38->tag != 1)goto _LL3;else{if((void*)_tmp38->f2 != 0)goto _LL3;}}_LL2:
 goto _LL4;_LL3: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp39=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp39->tag != 17)goto _LL5;else{if((void*)_tmp39->f4 != 0)goto _LL5;}}_LL4:
 return t;_LL5: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp3A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp37;if(_tmp3A->tag != 17)goto _LL7;else{_tmp3B=(void**)((void**)& _tmp3A->f4);}}_LL6: {
# 186
void*ta=(void*)_check_null(*_tmp3B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3B=t2;
return t2;}_LL7: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp37;if(_tmp3C->tag != 1)goto _LL9;else{_tmp3D=(void**)((void**)& _tmp3C->f2);}}_LL8: {
# 192
void*ta=(void*)_check_null(*_tmp3D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp3D=t2;
return t2;}_LL9: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp3E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp37;if(_tmp3E->tag != 18)goto _LLB;else{_tmp3F=_tmp3E->f1;}}_LLA:
# 198
 Cyc_Evexp_eval_const_uint_exp(_tmp3F);{
void*_tmp42=_tmp3F->r;void*_tmp43=_tmp42;void*_tmp45;_LL10: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp44=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp43;if(_tmp44->tag != 38)goto _LL12;else{_tmp45=(void*)_tmp44->f1;}}_LL11:
 return Cyc_Tcutil_compress(_tmp45);_LL12:;_LL13:
 return t;_LLF:;};_LLB: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp40=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp37;if(_tmp40->tag != 26)goto _LLD;else{if(_tmp40->f2 == 0)goto _LLD;_tmp41=*_tmp40->f2;}}_LLC:
# 204
 return Cyc_Tcutil_compress(_tmp41);_LLD:;_LLE:
 return t;_LL0:;}
# 212
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 216
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp46=cptr;void*_tmp47;union Cyc_Absyn_Constraint*_tmp48;_LL15: if((_tmp46->No_constr).tag != 3)goto _LL17;_LL16:
 return Cyc_Absyn_empty_conref();_LL17: if((_tmp46->Eq_constr).tag != 1)goto _LL19;_tmp47=(void*)(_tmp46->Eq_constr).val;_LL18:
 return Cyc_Absyn_new_conref(_tmp47);_LL19: if((_tmp46->Forward_constr).tag != 2)goto _LL14;_tmp48=(union Cyc_Absyn_Constraint*)(_tmp46->Forward_constr).val;_LL1A:
 return Cyc_Tcutil_copy_conref(_tmp48);_LL14:;}
# 223
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp49=Cyc_Absyn_compress_kb(kb);void*_tmp4A=_tmp49;struct Cyc_Absyn_Kind*_tmp4D;_LL1C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4B=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4B->tag != 1)goto _LL1E;}_LL1D: {
struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpE90;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpE8F;return(void*)((_tmpE8F=_cycalloc(sizeof(*_tmpE8F)),((_tmpE8F[0]=((_tmpE90.tag=1,((_tmpE90.f1=0,_tmpE90)))),_tmpE8F))));}_LL1E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4A;if(_tmp4C->tag != 2)goto _LL20;else{_tmp4D=_tmp4C->f2;}}_LL1F: {
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE93;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpE92;return(void*)((_tmpE92=_cycalloc(sizeof(*_tmpE92)),((_tmpE92[0]=((_tmpE93.tag=2,((_tmpE93.f1=0,((_tmpE93.f2=_tmp4D,_tmpE93)))))),_tmpE92))));}_LL20:;_LL21:
 return kb;_LL1B:;}
# 230
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 232
struct Cyc_Absyn_Tvar*_tmpE94;return(_tmpE94=_cycalloc(sizeof(*_tmpE94)),((_tmpE94->name=tv->name,((_tmpE94->identity=- 1,((_tmpE94->kind=Cyc_Tcutil_copy_kindbound(tv->kind),_tmpE94)))))));}
# 234
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 236
struct _dyneither_ptr*_tmp54;struct Cyc_Absyn_Tqual _tmp55;void*_tmp56;struct _tuple10*_tmp53=arg;_tmp54=_tmp53->f1;_tmp55=_tmp53->f2;_tmp56=_tmp53->f3;{
struct _tuple10*_tmpE95;return(_tmpE95=_cycalloc(sizeof(*_tmpE95)),((_tmpE95->f1=_tmp54,((_tmpE95->f2=_tmp55,((_tmpE95->f3=Cyc_Tcutil_copy_type(_tmp56),_tmpE95)))))));};}
# 239
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct Cyc_Absyn_Tqual _tmp59;void*_tmp5A;struct _tuple12*_tmp58=arg;_tmp59=_tmp58->f1;_tmp5A=_tmp58->f2;{
struct _tuple12*_tmpE96;return(_tmpE96=_cycalloc(sizeof(*_tmpE96)),((_tmpE96->f1=_tmp59,((_tmpE96->f2=Cyc_Tcutil_copy_type(_tmp5A),_tmpE96)))));};}
# 243
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_Absyn_Aggrfield*_tmpE97;return(_tmpE97=_cycalloc(sizeof(*_tmpE97)),((_tmpE97->name=f->name,((_tmpE97->tq=f->tq,((_tmpE97->type=Cyc_Tcutil_copy_type(f->type),((_tmpE97->width=f->width,((_tmpE97->attributes=f->attributes,((_tmpE97->requires_clause=f->requires_clause,_tmpE97)))))))))))));}
# 248
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
void*_tmp5E;void*_tmp5F;struct _tuple0*_tmp5D=x;_tmp5E=_tmp5D->f1;_tmp5F=_tmp5D->f2;{
struct _tuple0*_tmpE98;return(_tmpE98=_cycalloc(sizeof(*_tmpE98)),((_tmpE98->f1=Cyc_Tcutil_copy_type(_tmp5E),((_tmpE98->f2=Cyc_Tcutil_copy_type(_tmp5F),_tmpE98)))));};}
# 252
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
struct Cyc_Absyn_Enumfield*_tmpE99;return(_tmpE99=_cycalloc(sizeof(*_tmpE99)),((_tmpE99->name=f->name,((_tmpE99->tag=f->tag,((_tmpE99->loc=f->loc,_tmpE99)))))));}
# 255
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE9C;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE9B;return(void*)((_tmpE9B=_cycalloc(sizeof(*_tmpE9B)),((_tmpE9B[0]=((_tmpE9C.tag=2,((_tmpE9C.f1=Cyc_Tcutil_copy_tvar(t),_tmpE9C)))),_tmpE9B))));}
# 258
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp64=Cyc_Tcutil_compress(t);void*_tmp65=_tmp64;struct Cyc_Absyn_Tvar*_tmp69;union Cyc_Absyn_DatatypeInfoU _tmp6B;struct Cyc_List_List*_tmp6C;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E;struct Cyc_List_List*_tmp6F;void*_tmp71;struct Cyc_Absyn_Tqual _tmp72;void*_tmp73;union Cyc_Absyn_Constraint*_tmp74;union Cyc_Absyn_Constraint*_tmp75;union Cyc_Absyn_Constraint*_tmp76;struct Cyc_Absyn_PtrLoc*_tmp77;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7C;struct Cyc_Absyn_Exp*_tmp7D;union Cyc_Absyn_Constraint*_tmp7E;unsigned int _tmp7F;struct Cyc_List_List*_tmp81;void*_tmp82;struct Cyc_Absyn_Tqual _tmp83;void*_tmp84;struct Cyc_List_List*_tmp85;int _tmp86;struct Cyc_Absyn_VarargInfo*_tmp87;struct Cyc_List_List*_tmp88;struct Cyc_List_List*_tmp89;struct Cyc_List_List*_tmp8B;enum Cyc_Absyn_AggrKind _tmp8D;struct _tuple2*_tmp8E;struct Cyc_Core_Opt*_tmp8F;struct Cyc_List_List*_tmp90;struct Cyc_Absyn_Aggrdecl**_tmp92;struct Cyc_List_List*_tmp93;enum Cyc_Absyn_AggrKind _tmp95;struct Cyc_List_List*_tmp96;struct _tuple2*_tmp98;struct Cyc_Absyn_Enumdecl*_tmp99;struct Cyc_List_List*_tmp9B;void*_tmp9D;struct Cyc_Absyn_Exp*_tmp9F;void*_tmpA1;void*_tmpA3;void*_tmpA4;struct _tuple2*_tmpA6;struct Cyc_List_List*_tmpA7;struct Cyc_Absyn_Typedefdecl*_tmpA8;void*_tmpAD;struct Cyc_List_List*_tmpAF;void*_tmpB1;struct Cyc_Absyn_Aggrdecl*_tmpB4;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Datatypedecl*_tmpBA;_LL23: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp66=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp65;if(_tmp66->tag != 0)goto _LL25;}_LL24:
 goto _LL26;_LL25: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp65;if(_tmp67->tag != 1)goto _LL27;}_LL26:
 return t;_LL27: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65;if(_tmp68->tag != 2)goto _LL29;else{_tmp69=_tmp68->f1;}}_LL28: {
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE9F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE9E;return(void*)((_tmpE9E=_cycalloc(sizeof(*_tmpE9E)),((_tmpE9E[0]=((_tmpE9F.tag=2,((_tmpE9F.f1=Cyc_Tcutil_copy_tvar(_tmp69),_tmpE9F)))),_tmpE9E))));}_LL29: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp6A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65;if(_tmp6A->tag != 3)goto _LL2B;else{_tmp6B=(_tmp6A->f1).datatype_info;_tmp6C=(_tmp6A->f1).targs;}}_LL2A: {
# 264
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpEA5;struct Cyc_Absyn_DatatypeInfo _tmpEA4;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEA3;return(void*)((_tmpEA3=_cycalloc(sizeof(*_tmpEA3)),((_tmpEA3[0]=((_tmpEA5.tag=3,((_tmpEA5.f1=((_tmpEA4.datatype_info=_tmp6B,((_tmpEA4.targs=Cyc_Tcutil_copy_types(_tmp6C),_tmpEA4)))),_tmpEA5)))),_tmpEA3))));}_LL2B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65;if(_tmp6D->tag != 4)goto _LL2D;else{_tmp6E=(_tmp6D->f1).field_info;_tmp6F=(_tmp6D->f1).targs;}}_LL2C: {
# 266
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpEAB;struct Cyc_Absyn_DatatypeFieldInfo _tmpEAA;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA9;return(void*)((_tmpEA9=_cycalloc(sizeof(*_tmpEA9)),((_tmpEA9[0]=((_tmpEAB.tag=4,((_tmpEAB.f1=((_tmpEAA.field_info=_tmp6E,((_tmpEAA.targs=Cyc_Tcutil_copy_types(_tmp6F),_tmpEAA)))),_tmpEAB)))),_tmpEA9))));}_LL2D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65;if(_tmp70->tag != 5)goto _LL2F;else{_tmp71=(_tmp70->f1).elt_typ;_tmp72=(_tmp70->f1).elt_tq;_tmp73=((_tmp70->f1).ptr_atts).rgn;_tmp74=((_tmp70->f1).ptr_atts).nullable;_tmp75=((_tmp70->f1).ptr_atts).bounds;_tmp76=((_tmp70->f1).ptr_atts).zero_term;_tmp77=((_tmp70->f1).ptr_atts).ptrloc;}}_LL2E: {
# 268
void*_tmpC3=Cyc_Tcutil_copy_type(_tmp71);
void*_tmpC4=Cyc_Tcutil_copy_type(_tmp73);
union Cyc_Absyn_Constraint*_tmpC5=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp74);
struct Cyc_Absyn_Tqual _tmpC6=_tmp72;
union Cyc_Absyn_Constraint*_tmpC7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp75);
union Cyc_Absyn_Constraint*_tmpC8=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp76);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpEB1;struct Cyc_Absyn_PtrInfo _tmpEB0;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEAF;return(void*)((_tmpEAF=_cycalloc(sizeof(*_tmpEAF)),((_tmpEAF[0]=((_tmpEB1.tag=5,((_tmpEB1.f1=((_tmpEB0.elt_typ=_tmpC3,((_tmpEB0.elt_tq=_tmpC6,((_tmpEB0.ptr_atts=(((_tmpEB0.ptr_atts).rgn=_tmpC4,(((_tmpEB0.ptr_atts).nullable=_tmpC5,(((_tmpEB0.ptr_atts).bounds=_tmpC7,(((_tmpEB0.ptr_atts).zero_term=_tmpC8,(((_tmpEB0.ptr_atts).ptrloc=_tmp77,_tmpEB0.ptr_atts)))))))))),_tmpEB0)))))),_tmpEB1)))),_tmpEAF))));}_LL2F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp65;if(_tmp78->tag != 6)goto _LL31;}_LL30:
 goto _LL32;_LL31: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp79=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp65;if(_tmp79->tag != 7)goto _LL33;}_LL32:
 return t;_LL33: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp7A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65;if(_tmp7A->tag != 8)goto _LL35;else{_tmp7B=(_tmp7A->f1).elt_type;_tmp7C=(_tmp7A->f1).tq;_tmp7D=(_tmp7A->f1).num_elts;_tmp7E=(_tmp7A->f1).zero_term;_tmp7F=(_tmp7A->f1).zt_loc;}}_LL34: {
# 279
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpEB7;struct Cyc_Absyn_ArrayInfo _tmpEB6;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpEB5;return(void*)((_tmpEB5=_cycalloc(sizeof(*_tmpEB5)),((_tmpEB5[0]=((_tmpEB7.tag=8,((_tmpEB7.f1=((_tmpEB6.elt_type=Cyc_Tcutil_copy_type(_tmp7B),((_tmpEB6.tq=_tmp7C,((_tmpEB6.num_elts=_tmp7D,((_tmpEB6.zero_term=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmp7E),((_tmpEB6.zt_loc=_tmp7F,_tmpEB6)))))))))),_tmpEB7)))),_tmpEB5))));}_LL35: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp80=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65;if(_tmp80->tag != 9)goto _LL37;else{_tmp81=(_tmp80->f1).tvars;_tmp82=(_tmp80->f1).effect;_tmp83=(_tmp80->f1).ret_tqual;_tmp84=(_tmp80->f1).ret_typ;_tmp85=(_tmp80->f1).args;_tmp86=(_tmp80->f1).c_varargs;_tmp87=(_tmp80->f1).cyc_varargs;_tmp88=(_tmp80->f1).rgn_po;_tmp89=(_tmp80->f1).attributes;}}_LL36: {
# 281
struct Cyc_List_List*_tmpCF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmp81);
void*effopt2=_tmp82 == 0?0: Cyc_Tcutil_copy_type(_tmp82);
void*_tmpD0=Cyc_Tcutil_copy_type(_tmp84);
struct Cyc_List_List*_tmpD1=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmp85);
int _tmpD2=_tmp86;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmp87 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmp87;
struct Cyc_Absyn_VarargInfo*_tmpEB8;cyc_varargs2=((_tmpEB8=_cycalloc(sizeof(*_tmpEB8)),((_tmpEB8->name=cv->name,((_tmpEB8->tq=cv->tq,((_tmpEB8->type=Cyc_Tcutil_copy_type(cv->type),((_tmpEB8->inject=cv->inject,_tmpEB8))))))))));}{
# 292
struct Cyc_List_List*_tmpD4=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp88);
struct Cyc_List_List*_tmpD5=_tmp89;
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpEBE;struct Cyc_Absyn_FnInfo _tmpEBD;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpEBC;return(void*)((_tmpEBC=_cycalloc(sizeof(*_tmpEBC)),((_tmpEBC[0]=((_tmpEBE.tag=9,((_tmpEBE.f1=((_tmpEBD.tvars=_tmpCF,((_tmpEBD.effect=effopt2,((_tmpEBD.ret_tqual=_tmp83,((_tmpEBD.ret_typ=_tmpD0,((_tmpEBD.args=_tmpD1,((_tmpEBD.c_varargs=_tmpD2,((_tmpEBD.cyc_varargs=cyc_varargs2,((_tmpEBD.rgn_po=_tmpD4,((_tmpEBD.attributes=_tmpD5,_tmpEBD)))))))))))))))))),_tmpEBE)))),_tmpEBC))));};}_LL37: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp8A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65;if(_tmp8A->tag != 10)goto _LL39;else{_tmp8B=_tmp8A->f1;}}_LL38: {
# 296
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpEC1;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpEC0;return(void*)((_tmpEC0=_cycalloc(sizeof(*_tmpEC0)),((_tmpEC0[0]=((_tmpEC1.tag=10,((_tmpEC1.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmp8B),_tmpEC1)))),_tmpEC0))));}_LL39: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp8C->tag != 11)goto _LL3B;else{if((((_tmp8C->f1).aggr_info).UnknownAggr).tag != 1)goto _LL3B;_tmp8D=((struct _tuple4)(((_tmp8C->f1).aggr_info).UnknownAggr).val).f1;_tmp8E=((struct _tuple4)(((_tmp8C->f1).aggr_info).UnknownAggr).val).f2;_tmp8F=((struct _tuple4)(((_tmp8C->f1).aggr_info).UnknownAggr).val).f3;_tmp90=(_tmp8C->f1).targs;}}_LL3A: {
# 298
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpEC7;struct Cyc_Absyn_AggrInfo _tmpEC6;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEC5;return(void*)((_tmpEC5=_cycalloc(sizeof(*_tmpEC5)),((_tmpEC5[0]=((_tmpEC7.tag=11,((_tmpEC7.f1=((_tmpEC6.aggr_info=Cyc_Absyn_UnknownAggr(_tmp8D,_tmp8E,_tmp8F),((_tmpEC6.targs=Cyc_Tcutil_copy_types(_tmp90),_tmpEC6)))),_tmpEC7)))),_tmpEC5))));}_LL3B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65;if(_tmp91->tag != 11)goto _LL3D;else{if((((_tmp91->f1).aggr_info).KnownAggr).tag != 2)goto _LL3D;_tmp92=(struct Cyc_Absyn_Aggrdecl**)(((_tmp91->f1).aggr_info).KnownAggr).val;_tmp93=(_tmp91->f1).targs;}}_LL3C: {
# 300
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpECD;struct Cyc_Absyn_AggrInfo _tmpECC;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpECB;return(void*)((_tmpECB=_cycalloc(sizeof(*_tmpECB)),((_tmpECB[0]=((_tmpECD.tag=11,((_tmpECD.f1=((_tmpECC.aggr_info=Cyc_Absyn_KnownAggr(_tmp92),((_tmpECC.targs=Cyc_Tcutil_copy_types(_tmp93),_tmpECC)))),_tmpECD)))),_tmpECB))));}_LL3D: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65;if(_tmp94->tag != 12)goto _LL3F;else{_tmp95=_tmp94->f1;_tmp96=_tmp94->f2;}}_LL3E: {
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpED0;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpECF;return(void*)((_tmpECF=_cycalloc(sizeof(*_tmpECF)),((_tmpECF[0]=((_tmpED0.tag=12,((_tmpED0.f1=_tmp95,((_tmpED0.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmp96),_tmpED0)))))),_tmpECF))));}_LL3F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp65;if(_tmp97->tag != 13)goto _LL41;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LL40: {
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpED3;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpED2;return(void*)((_tmpED2=_cycalloc(sizeof(*_tmpED2)),((_tmpED2[0]=((_tmpED3.tag=13,((_tmpED3.f1=_tmp98,((_tmpED3.f2=_tmp99,_tmpED3)))))),_tmpED2))));}_LL41: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp65;if(_tmp9A->tag != 14)goto _LL43;else{_tmp9B=_tmp9A->f1;}}_LL42: {
struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpED6;struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpED5;return(void*)((_tmpED5=_cycalloc(sizeof(*_tmpED5)),((_tmpED5[0]=((_tmpED6.tag=14,((_tmpED6.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmp9B),_tmpED6)))),_tmpED5))));}_LL43: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp9C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65;if(_tmp9C->tag != 19)goto _LL45;else{_tmp9D=(void*)_tmp9C->f1;}}_LL44: {
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpED9;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpED8;return(void*)((_tmpED8=_cycalloc(sizeof(*_tmpED8)),((_tmpED8[0]=((_tmpED9.tag=19,((_tmpED9.f1=Cyc_Tcutil_copy_type(_tmp9D),_tmpED9)))),_tmpED8))));}_LL45: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65;if(_tmp9E->tag != 18)goto _LL47;else{_tmp9F=_tmp9E->f1;}}_LL46: {
# 307
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpEDC;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpEDB;return(void*)((_tmpEDB=_cycalloc(sizeof(*_tmpEDB)),((_tmpEDB[0]=((_tmpEDC.tag=18,((_tmpEDC.f1=_tmp9F,_tmpEDC)))),_tmpEDB))));}_LL47: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65;if(_tmpA0->tag != 15)goto _LL49;else{_tmpA1=(void*)_tmpA0->f1;}}_LL48: {
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpEDF;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpEDE;return(void*)((_tmpEDE=_cycalloc(sizeof(*_tmpEDE)),((_tmpEDE[0]=((_tmpEDF.tag=15,((_tmpEDF.f1=Cyc_Tcutil_copy_type(_tmpA1),_tmpEDF)))),_tmpEDE))));}_LL49: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65;if(_tmpA2->tag != 16)goto _LL4B;else{_tmpA3=(void*)_tmpA2->f1;_tmpA4=(void*)_tmpA2->f2;}}_LL4A: {
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpEE2;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpEE1;return(void*)((_tmpEE1=_cycalloc(sizeof(*_tmpEE1)),((_tmpEE1[0]=((_tmpEE2.tag=16,((_tmpEE2.f1=Cyc_Tcutil_copy_type(_tmpA3),((_tmpEE2.f2=Cyc_Tcutil_copy_type(_tmpA4),_tmpEE2)))))),_tmpEE1))));}_LL4B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65;if(_tmpA5->tag != 17)goto _LL4D;else{_tmpA6=_tmpA5->f1;_tmpA7=_tmpA5->f2;_tmpA8=_tmpA5->f3;}}_LL4C: {
# 311
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpEE5;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpEE4;return(void*)((_tmpEE4=_cycalloc(sizeof(*_tmpEE4)),((_tmpEE4[0]=((_tmpEE5.tag=17,((_tmpEE5.f1=_tmpA6,((_tmpEE5.f2=Cyc_Tcutil_copy_types(_tmpA7),((_tmpEE5.f3=_tmpA8,((_tmpEE5.f4=0,_tmpEE5)))))))))),_tmpEE4))));}_LL4D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp65;if(_tmpA9->tag != 21)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp65;if(_tmpAA->tag != 22)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp65;if(_tmpAB->tag != 20)goto _LL53;}_LL52:
 return t;_LL53: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp65;if(_tmpAC->tag != 23)goto _LL55;else{_tmpAD=(void*)_tmpAC->f1;}}_LL54: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpEE8;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpEE7;return(void*)((_tmpEE7=_cycalloc(sizeof(*_tmpEE7)),((_tmpEE7[0]=((_tmpEE8.tag=23,((_tmpEE8.f1=Cyc_Tcutil_copy_type(_tmpAD),_tmpEE8)))),_tmpEE7))));}_LL55: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65;if(_tmpAE->tag != 24)goto _LL57;else{_tmpAF=_tmpAE->f1;}}_LL56: {
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpEEB;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpEEA;return(void*)((_tmpEEA=_cycalloc(sizeof(*_tmpEEA)),((_tmpEEA[0]=((_tmpEEB.tag=24,((_tmpEEB.f1=Cyc_Tcutil_copy_types(_tmpAF),_tmpEEB)))),_tmpEEA))));}_LL57: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65;if(_tmpB0->tag != 25)goto _LL59;else{_tmpB1=(void*)_tmpB0->f1;}}_LL58: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpEEE;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpEED;return(void*)((_tmpEED=_cycalloc(sizeof(*_tmpEED)),((_tmpEED[0]=((_tmpEEE.tag=25,((_tmpEEE.f1=Cyc_Tcutil_copy_type(_tmpB1),_tmpEEE)))),_tmpEED))));}_LL59: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB2->tag != 26)goto _LL5B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB3=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB2->f1)->r;if(_tmpB3->tag != 0)goto _LL5B;else{_tmpB4=_tmpB3->f1;}}}_LL5A: {
# 320
struct Cyc_List_List*_tmpF7=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB4->tvs);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpEF4;struct Cyc_Absyn_AggrInfo _tmpEF3;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpEF2;return(void*)((_tmpEF2=_cycalloc(sizeof(*_tmpEF2)),((_tmpEF2[0]=((_tmpEF4.tag=11,((_tmpEF4.f1=((_tmpEF3.aggr_info=Cyc_Absyn_UnknownAggr(_tmpB4->kind,_tmpB4->name,0),((_tmpEF3.targs=_tmpF7,_tmpEF3)))),_tmpEF4)))),_tmpEF2))));}_LL5B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB5->tag != 26)goto _LL5D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 1)goto _LL5D;else{_tmpB7=_tmpB6->f1;}}}_LL5C: {
# 323
struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpEF7;struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpEF6;return(void*)((_tmpEF6=_cycalloc(sizeof(*_tmpEF6)),((_tmpEF6[0]=((_tmpEF7.tag=13,((_tmpEF7.f1=_tmpB7->name,((_tmpEF7.f2=0,_tmpEF7)))))),_tmpEF6))));}_LL5D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp65;if(_tmpB8->tag != 26)goto _LL22;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 2)goto _LL22;else{_tmpBA=_tmpB9->f1;}}}_LL5E: {
# 325
struct Cyc_List_List*_tmpFD=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpBA->tvs);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpF01;struct Cyc_Absyn_UnknownDatatypeInfo _tmpF00;struct Cyc_Absyn_DatatypeInfo _tmpEFF;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpEFE;return(void*)((_tmpEFE=_cycalloc(sizeof(*_tmpEFE)),((_tmpEFE[0]=((_tmpF01.tag=3,((_tmpF01.f1=((_tmpEFF.datatype_info=Cyc_Absyn_UnknownDatatype(((_tmpF00.name=_tmpBA->name,((_tmpF00.is_extensible=0,_tmpF00))))),((_tmpEFF.targs=_tmpFD,_tmpEFF)))),_tmpF01)))),_tmpEFE))));}_LL22:;}
# 340 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int pt,void*d){
void*_tmp102=d;struct Cyc_Absyn_Exp*_tmp104;struct _dyneither_ptr*_tmp106;_LL60: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp103=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp102;if(_tmp103->tag != 0)goto _LL62;else{_tmp104=_tmp103->f1;}}_LL61: {
struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpF04;struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpF03;return(void*)((_tmpF03=_cycalloc(sizeof(*_tmpF03)),((_tmpF03[0]=((_tmpF04.tag=0,((_tmpF04.f1=Cyc_Tcutil_deep_copy_exp(pt,_tmp104),_tmpF04)))),_tmpF03))));}_LL62: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp105=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp102;if(_tmp105->tag != 1)goto _LL5F;else{_tmp106=_tmp105->f1;}}_LL63:
 return d;_LL5F:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 346
static struct _tuple19*Cyc_Tcutil_copy_eds(int pt,struct _tuple19*e){
# 348
struct _tuple19*_tmpF05;return(_tmpF05=_cycalloc(sizeof(*_tmpF05)),((_tmpF05->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,pt,(e[0]).f1),((_tmpF05->f2=Cyc_Tcutil_deep_copy_exp(pt,(e[0]).f2),_tmpF05)))));}
# 351
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmp10A=preserve_types;
{void*_tmp10B=e->r;void*_tmp10C=_tmp10B;union Cyc_Absyn_Cnst _tmp10E;struct _tuple2*_tmp110;void*_tmp111;enum Cyc_Absyn_Primop _tmp113;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Core_Opt*_tmp117;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11A;enum Cyc_Absyn_Incrementor _tmp11B;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_List_List*_tmp12B;struct Cyc_Absyn_VarargCallInfo*_tmp12C;int _tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_List_List*_tmp134;void*_tmp136;struct Cyc_Absyn_Exp*_tmp137;int _tmp138;enum Cyc_Absyn_Coercion _tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13E;void*_tmp140;struct Cyc_Absyn_Exp*_tmp142;void*_tmp144;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp149;struct _dyneither_ptr*_tmp14A;int _tmp14B;int _tmp14C;struct Cyc_Absyn_Exp*_tmp14E;struct _dyneither_ptr*_tmp14F;int _tmp150;int _tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_List_List*_tmp156;struct _dyneither_ptr*_tmp158;struct Cyc_Absyn_Tqual _tmp159;void*_tmp15A;struct Cyc_List_List*_tmp15B;struct Cyc_List_List*_tmp15D;struct Cyc_Absyn_Vardecl*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp161;int _tmp162;struct Cyc_Absyn_Exp*_tmp164;void*_tmp165;int _tmp166;struct _tuple2*_tmp168;struct Cyc_List_List*_tmp169;struct Cyc_List_List*_tmp16A;struct Cyc_Absyn_Aggrdecl*_tmp16B;void*_tmp16D;struct Cyc_List_List*_tmp16E;struct Cyc_List_List*_tmp170;struct Cyc_Absyn_Datatypedecl*_tmp171;struct Cyc_Absyn_Datatypefield*_tmp172;struct _tuple2*_tmp174;struct Cyc_Absyn_Enumdecl*_tmp175;struct Cyc_Absyn_Enumfield*_tmp176;struct _tuple2*_tmp178;void*_tmp179;struct Cyc_Absyn_Enumfield*_tmp17A;int _tmp17C;struct Cyc_Absyn_Exp*_tmp17D;void**_tmp17E;struct Cyc_Absyn_Exp*_tmp17F;int _tmp180;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp183;struct Cyc_Core_Opt*_tmp185;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Exp*_tmp189;struct _dyneither_ptr*_tmp18A;void*_tmp18C;int _tmp18E;struct _dyneither_ptr _tmp18F;_LL65: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp10D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10D->tag != 0)goto _LL67;else{_tmp10E=_tmp10D->f1;}}_LL66:
 new_e=Cyc_Absyn_const_exp(_tmp10E,e->loc);goto _LL64;_LL67: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp10F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp10F->tag != 1)goto _LL69;else{_tmp110=_tmp10F->f1;_tmp111=(void*)_tmp10F->f2;}}_LL68:
 new_e=Cyc_Absyn_varb_exp(_tmp110,_tmp111,e->loc);goto _LL64;_LL69: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp112=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp112->tag != 2)goto _LL6B;else{_tmp113=_tmp112->f1;_tmp114=_tmp112->f2;}}_LL6A:
 new_e=Cyc_Absyn_primop_exp(_tmp113,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10A,_tmp114),e->loc);goto _LL64;_LL6B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp115=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp115->tag != 3)goto _LL6D;else{_tmp116=_tmp115->f1;_tmp117=_tmp115->f2;_tmp118=_tmp115->f3;}}_LL6C:
# 359
{struct Cyc_Core_Opt*_tmpF06;new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp116),(unsigned int)_tmp117?(_tmpF06=_cycalloc_atomic(sizeof(*_tmpF06)),((_tmpF06->v=(void*)((enum Cyc_Absyn_Primop)_tmp117->v),_tmpF06))): 0,Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp118),e->loc);}
goto _LL64;_LL6D: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp119->tag != 4)goto _LL6F;else{_tmp11A=_tmp119->f1;_tmp11B=_tmp119->f2;}}_LL6E:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp11A),_tmp11B,e->loc);goto _LL64;_LL6F: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp11C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp11C->tag != 5)goto _LL71;else{_tmp11D=_tmp11C->f1;_tmp11E=_tmp11C->f2;_tmp11F=_tmp11C->f3;}}_LL70:
# 363
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp11D),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp11F),e->loc);goto _LL64;_LL71: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp120=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp120->tag != 6)goto _LL73;else{_tmp121=_tmp120->f1;_tmp122=_tmp120->f2;}}_LL72:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp121),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp122),e->loc);goto _LL64;_LL73: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp123->tag != 7)goto _LL75;else{_tmp124=_tmp123->f1;_tmp125=_tmp123->f2;}}_LL74:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp124),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp125),e->loc);goto _LL64;_LL75: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp126=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp126->tag != 8)goto _LL77;else{_tmp127=_tmp126->f1;_tmp128=_tmp126->f2;}}_LL76:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp127),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp128),e->loc);goto _LL64;_LL77: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp129=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp129->tag != 9)goto _LL79;else{_tmp12A=_tmp129->f1;_tmp12B=_tmp129->f2;_tmp12C=_tmp129->f3;_tmp12D=_tmp129->f4;}}_LL78:
# 368
{struct Cyc_Absyn_VarargCallInfo*_tmp191=_tmp12C;int _tmp192;struct Cyc_List_List*_tmp193;struct Cyc_Absyn_VarargInfo*_tmp194;_LLB6: if(_tmp191 == 0)goto _LLB8;_tmp192=_tmp191->num_varargs;_tmp193=_tmp191->injectors;_tmp194=_tmp191->vai;_LLB7: {
# 370
struct _dyneither_ptr*_tmp196;struct Cyc_Absyn_Tqual _tmp197;void*_tmp198;int _tmp199;struct Cyc_Absyn_VarargInfo*_tmp195=_tmp194;_tmp196=_tmp195->name;_tmp197=_tmp195->tq;_tmp198=_tmp195->type;_tmp199=_tmp195->inject;
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF10;struct Cyc_Absyn_VarargInfo*_tmpF0F;struct Cyc_Absyn_VarargCallInfo*_tmpF0E;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF0D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF0D=_cycalloc(sizeof(*_tmpF0D)),((_tmpF0D[0]=((_tmpF10.tag=9,((_tmpF10.f1=
Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp12A),((_tmpF10.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10A,_tmp12B),((_tmpF10.f3=(
(_tmpF0E=_cycalloc(sizeof(*_tmpF0E)),((_tmpF0E->num_varargs=_tmp192,((_tmpF0E->injectors=_tmp193,((_tmpF0E->vai=(
(_tmpF0F=_cycalloc(sizeof(*_tmpF0F)),((_tmpF0F->name=_tmp196,((_tmpF0F->tq=_tmp197,((_tmpF0F->type=Cyc_Tcutil_copy_type(_tmp198),((_tmpF0F->inject=_tmp199,_tmpF0F)))))))))),_tmpF0E)))))))),((_tmpF10.f4=_tmp12D,_tmpF10)))))))))),_tmpF0D)))),e->loc);}
# 376
goto _LLB5;}_LLB8:;_LLB9:
# 378
{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpF13;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpF12;new_e=Cyc_Absyn_new_exp((void*)((_tmpF12=_cycalloc(sizeof(*_tmpF12)),((_tmpF12[0]=((_tmpF13.tag=9,((_tmpF13.f1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp12A),((_tmpF13.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10A,_tmp12B),((_tmpF13.f3=_tmp12C,((_tmpF13.f4=_tmp12D,_tmpF13)))))))))),_tmpF12)))),e->loc);}
goto _LLB5;_LLB5:;}
# 381
goto _LL64;_LL79: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp12E->tag != 10)goto _LL7B;else{_tmp12F=_tmp12E->f1;}}_LL7A:
 new_e=Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp12F),e->loc);goto _LL64;_LL7B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp130=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp130->tag != 11)goto _LL7D;else{_tmp131=_tmp130->f1;}}_LL7C:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp131),e->loc);
goto _LL64;_LL7D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp132->tag != 12)goto _LL7F;else{_tmp133=_tmp132->f1;_tmp134=_tmp132->f2;}}_LL7E:
# 386
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp133),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp134),e->loc);
goto _LL64;_LL7F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp135=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp135->tag != 13)goto _LL81;else{_tmp136=(void*)_tmp135->f1;_tmp137=_tmp135->f2;_tmp138=_tmp135->f3;_tmp139=_tmp135->f4;}}_LL80:
# 389
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp136),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp137),_tmp138,_tmp139,e->loc);goto _LL64;_LL81: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp13A->tag != 14)goto _LL83;else{_tmp13B=_tmp13A->f1;}}_LL82:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp13B),e->loc);goto _LL64;_LL83: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp13C->tag != 15)goto _LL85;else{_tmp13D=_tmp13C->f1;_tmp13E=_tmp13C->f2;}}_LL84: {
# 392
struct Cyc_Absyn_Exp*eo1=_tmp13D;if(_tmp13D != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp13D);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp13E),e->loc);
goto _LL64;}_LL85: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp13F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp13F->tag != 16)goto _LL87;else{_tmp140=(void*)_tmp13F->f1;}}_LL86:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp140),e->loc);
goto _LL64;_LL87: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp141->tag != 17)goto _LL89;else{_tmp142=_tmp141->f1;}}_LL88:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp142),e->loc);goto _LL64;_LL89: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp143=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp143->tag != 18)goto _LL8B;else{_tmp144=(void*)_tmp143->f1;_tmp145=_tmp143->f2;}}_LL8A:
# 399
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp144),_tmp145,e->loc);goto _LL64;_LL8B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp146=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp146->tag != 19)goto _LL8D;else{_tmp147=_tmp146->f1;}}_LL8C:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp147),e->loc);goto _LL64;_LL8D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp148->tag != 20)goto _LL8F;else{_tmp149=_tmp148->f1;_tmp14A=_tmp148->f2;_tmp14B=_tmp148->f3;_tmp14C=_tmp148->f4;}}_LL8E:
# 402
{struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF16;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF15;new_e=Cyc_Absyn_new_exp((void*)((_tmpF15=_cycalloc(sizeof(*_tmpF15)),((_tmpF15[0]=((_tmpF16.tag=20,((_tmpF16.f1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp149),((_tmpF16.f2=_tmp14A,((_tmpF16.f3=_tmp14B,((_tmpF16.f4=_tmp14C,_tmpF16)))))))))),_tmpF15)))),e->loc);}goto _LL64;_LL8F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp14D->tag != 21)goto _LL91;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;_tmp150=_tmp14D->f3;_tmp151=_tmp14D->f4;}}_LL90:
# 404
{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF19;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF18;new_e=Cyc_Absyn_new_exp((void*)((_tmpF18=_cycalloc(sizeof(*_tmpF18)),((_tmpF18[0]=((_tmpF19.tag=21,((_tmpF19.f1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp14E),((_tmpF19.f2=_tmp14F,((_tmpF19.f3=_tmp150,((_tmpF19.f4=_tmp151,_tmpF19)))))))))),_tmpF18)))),e->loc);}goto _LL64;_LL91: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp152->tag != 22)goto _LL93;else{_tmp153=_tmp152->f1;_tmp154=_tmp152->f2;}}_LL92:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp153),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp154),e->loc);
goto _LL64;_LL93: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp155->tag != 23)goto _LL95;else{_tmp156=_tmp155->f1;}}_LL94:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10A,_tmp156),e->loc);goto _LL64;_LL95: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp157->tag != 24)goto _LL97;else{_tmp158=(_tmp157->f1)->f1;_tmp159=(_tmp157->f1)->f2;_tmp15A=(_tmp157->f1)->f3;_tmp15B=_tmp157->f2;}}_LL96: {
# 409
struct _dyneither_ptr*vopt1=_tmp158;
if(_tmp158 != 0)vopt1=_tmp158;
{struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1F;struct _tuple10*_tmpF1E;struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF1D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF1D=_cycalloc(sizeof(*_tmpF1D)),((_tmpF1D[0]=((_tmpF1F.tag=24,((_tmpF1F.f1=((_tmpF1E=_cycalloc(sizeof(*_tmpF1E)),((_tmpF1E->f1=vopt1,((_tmpF1E->f2=_tmp159,((_tmpF1E->f3=Cyc_Tcutil_copy_type(_tmp15A),_tmpF1E)))))))),((_tmpF1F.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10A,_tmp15B),_tmpF1F)))))),_tmpF1D)))),e->loc);}
goto _LL64;}_LL97: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp15C->tag != 25)goto _LL99;else{_tmp15D=_tmp15C->f1;}}_LL98:
{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF22;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF21;new_e=Cyc_Absyn_new_exp((void*)((_tmpF21=_cycalloc(sizeof(*_tmpF21)),((_tmpF21[0]=((_tmpF22.tag=25,((_tmpF22.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10A,_tmp15D),_tmpF22)))),_tmpF21)))),e->loc);}
goto _LL64;_LL99: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp15E->tag != 26)goto _LL9B;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;_tmp161=_tmp15E->f3;_tmp162=_tmp15E->f4;}}_LL9A:
# 417
{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF25;struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF24;new_e=Cyc_Absyn_new_exp((void*)((_tmpF24=_cycalloc(sizeof(*_tmpF24)),((_tmpF24[0]=((_tmpF25.tag=26,((_tmpF25.f1=_tmp15F,((_tmpF25.f2=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp160),((_tmpF25.f3=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp161),((_tmpF25.f4=_tmp162,_tmpF25)))))))))),_tmpF24)))),e->loc);}
goto _LL64;_LL9B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp163=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp163->tag != 27)goto _LL9D;else{_tmp164=_tmp163->f1;_tmp165=(void*)_tmp163->f2;_tmp166=_tmp163->f3;}}_LL9C:
# 420
{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF28;struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF27;new_e=Cyc_Absyn_new_exp((void*)((_tmpF27=_cycalloc(sizeof(*_tmpF27)),((_tmpF27[0]=((_tmpF28.tag=27,((_tmpF28.f1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp164),((_tmpF28.f2=Cyc_Tcutil_copy_type(_tmp165),((_tmpF28.f3=_tmp166,_tmpF28)))))))),_tmpF27)))),_tmp164->loc);}
# 422
goto _LL64;_LL9D: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp167->tag != 28)goto _LL9F;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;_tmp16A=_tmp167->f3;_tmp16B=_tmp167->f4;}}_LL9E:
# 424
{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpF2B;struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF2A;new_e=Cyc_Absyn_new_exp((void*)((_tmpF2A=_cycalloc(sizeof(*_tmpF2A)),((_tmpF2A[0]=((_tmpF2B.tag=28,((_tmpF2B.f1=_tmp168,((_tmpF2B.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp169),((_tmpF2B.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10A,_tmp16A),((_tmpF2B.f4=_tmp16B,_tmpF2B)))))))))),_tmpF2A)))),e->loc);}
# 426
goto _LL64;_LL9F: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp16C->tag != 29)goto _LLA1;else{_tmp16D=(void*)_tmp16C->f1;_tmp16E=_tmp16C->f2;}}_LLA0:
# 428
{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpF2E;struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpF2D;new_e=Cyc_Absyn_new_exp((void*)((_tmpF2D=_cycalloc(sizeof(*_tmpF2D)),((_tmpF2D[0]=((_tmpF2E.tag=29,((_tmpF2E.f1=Cyc_Tcutil_copy_type(_tmp16D),((_tmpF2E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10A,_tmp16E),_tmpF2E)))))),_tmpF2D)))),e->loc);}
goto _LL64;_LLA1: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp16F->tag != 30)goto _LLA3;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;_tmp172=_tmp16F->f3;}}_LLA2:
# 431
{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpF31;struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpF30;new_e=Cyc_Absyn_new_exp((void*)((_tmpF30=_cycalloc(sizeof(*_tmpF30)),((_tmpF30[0]=((_tmpF31.tag=30,((_tmpF31.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmp10A,_tmp170),((_tmpF31.f2=_tmp171,((_tmpF31.f3=_tmp172,_tmpF31)))))))),_tmpF30)))),e->loc);}
goto _LL64;_LLA3: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp173=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp173->tag != 31)goto _LLA5;else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;_tmp176=_tmp173->f3;}}_LLA4:
 new_e=e;goto _LL64;_LLA5: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp177->tag != 32)goto _LLA7;else{_tmp178=_tmp177->f1;_tmp179=(void*)_tmp177->f2;_tmp17A=_tmp177->f3;}}_LLA6:
# 435
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpF34;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpF33;new_e=Cyc_Absyn_new_exp((void*)((_tmpF33=_cycalloc(sizeof(*_tmpF33)),((_tmpF33[0]=((_tmpF34.tag=32,((_tmpF34.f1=_tmp178,((_tmpF34.f2=Cyc_Tcutil_copy_type(_tmp179),((_tmpF34.f3=_tmp17A,_tmpF34)))))))),_tmpF33)))),e->loc);}
goto _LL64;_LLA7: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp17B=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp17B->tag != 33)goto _LLA9;else{_tmp17C=(_tmp17B->f1).is_calloc;_tmp17D=(_tmp17B->f1).rgn;_tmp17E=(_tmp17B->f1).elt_type;_tmp17F=(_tmp17B->f1).num_elts;_tmp180=(_tmp17B->f1).fat_result;}}_LLA8: {
# 438
struct Cyc_Absyn_Exp*_tmp1B5=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp17D;if(_tmp17D != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp17D);{
void**t1=_tmp17E;if(_tmp17E != 0){void**_tmpF35;t1=((_tmpF35=_cycalloc(sizeof(*_tmpF35)),((_tmpF35[0]=Cyc_Tcutil_copy_type(*_tmp17E),_tmpF35))));}
{struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpF3B;struct Cyc_Absyn_MallocInfo _tmpF3A;struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpF39;_tmp1B5->r=(void*)((_tmpF39=_cycalloc(sizeof(*_tmpF39)),((_tmpF39[0]=((_tmpF3B.tag=33,((_tmpF3B.f1=((_tmpF3A.is_calloc=_tmp17C,((_tmpF3A.rgn=r1,((_tmpF3A.elt_type=t1,((_tmpF3A.num_elts=_tmp17F,((_tmpF3A.fat_result=_tmp180,_tmpF3A)))))))))),_tmpF3B)))),_tmpF39))));}
new_e=_tmp1B5;
goto _LL64;};}_LLA9: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp181=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp181->tag != 34)goto _LLAB;else{_tmp182=_tmp181->f1;_tmp183=_tmp181->f2;}}_LLAA:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp182),Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp183),e->loc);goto _LL64;_LLAB: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp184=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp184->tag != 35)goto _LLAD;else{_tmp185=_tmp184->f1;_tmp186=_tmp184->f2;}}_LLAC: {
# 446
struct Cyc_Core_Opt*nopt1=_tmp185;
if(_tmp185 != 0){struct Cyc_Core_Opt*_tmpF3C;nopt1=((_tmpF3C=_cycalloc(sizeof(*_tmpF3C)),((_tmpF3C->v=(struct _tuple2*)_tmp185->v,_tmpF3C))));}
{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpF3F;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF3E;new_e=Cyc_Absyn_new_exp((void*)((_tmpF3E=_cycalloc(sizeof(*_tmpF3E)),((_tmpF3E[0]=((_tmpF3F.tag=35,((_tmpF3F.f1=nopt1,((_tmpF3F.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmp10A,_tmp186),_tmpF3F)))))),_tmpF3E)))),e->loc);}
goto _LL64;}_LLAD: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp187=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp187->tag != 36)goto _LLAF;}_LLAE: {
# 451
struct Cyc_Core_Failure_exn_struct _tmpF45;const char*_tmpF44;struct Cyc_Core_Failure_exn_struct*_tmpF43;(int)_throw((void*)((_tmpF43=_cycalloc(sizeof(*_tmpF43)),((_tmpF43[0]=((_tmpF45.tag=Cyc_Core_Failure,((_tmpF45.f1=((_tmpF44="deep_copy: statement expressions unsupported",_tag_dyneither(_tmpF44,sizeof(char),45))),_tmpF45)))),_tmpF43)))));}_LLAF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp188->tag != 37)goto _LLB1;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f2;}}_LLB0:
{struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpF48;struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF47;new_e=Cyc_Absyn_new_exp((void*)((_tmpF47=_cycalloc(sizeof(*_tmpF47)),((_tmpF47[0]=((_tmpF48.tag=37,((_tmpF48.f1=Cyc_Tcutil_deep_copy_exp(_tmp10A,_tmp189),((_tmpF48.f2=_tmp18A,_tmpF48)))))),_tmpF47)))),e->loc);}
goto _LL64;_LLB1: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp18B->tag != 38)goto _LLB3;else{_tmp18C=(void*)_tmp18B->f1;}}_LLB2:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp18C),e->loc);
goto _LL64;_LLB3: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp10C;if(_tmp18D->tag != 39)goto _LL64;else{_tmp18E=_tmp18D->f1;_tmp18F=_tmp18D->f2;}}_LLB4:
 new_e=Cyc_Absyn_asm_exp(_tmp18E,_tmp18F,e->loc);goto _LL64;_LL64:;}
# 459
if(preserve_types)new_e->topt=e->topt;
return new_e;}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 471 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
enum Cyc_Absyn_KindQual _tmp1C3;enum Cyc_Absyn_AliasQual _tmp1C4;struct Cyc_Absyn_Kind*_tmp1C2=ka1;_tmp1C3=_tmp1C2->kind;_tmp1C4=_tmp1C2->aliasqual;{
enum Cyc_Absyn_KindQual _tmp1C6;enum Cyc_Absyn_AliasQual _tmp1C7;struct Cyc_Absyn_Kind*_tmp1C5=ka2;_tmp1C6=_tmp1C5->kind;_tmp1C7=_tmp1C5->aliasqual;
# 475
if(_tmp1C3 != _tmp1C6){
struct _tuple20 _tmpF49;struct _tuple20 _tmp1C8=(_tmpF49.f1=_tmp1C3,((_tmpF49.f2=_tmp1C6,_tmpF49)));struct _tuple20 _tmp1C9=_tmp1C8;_LLBB: if(_tmp1C9.f1 != Cyc_Absyn_BoxKind)goto _LLBD;if(_tmp1C9.f2 != Cyc_Absyn_MemKind)goto _LLBD;_LLBC:
 goto _LLBE;_LLBD: if(_tmp1C9.f1 != Cyc_Absyn_BoxKind)goto _LLBF;if(_tmp1C9.f2 != Cyc_Absyn_AnyKind)goto _LLBF;_LLBE:
 goto _LLC0;_LLBF: if(_tmp1C9.f1 != Cyc_Absyn_MemKind)goto _LLC1;if(_tmp1C9.f2 != Cyc_Absyn_AnyKind)goto _LLC1;_LLC0:
 goto _LLBA;_LLC1:;_LLC2:
 return 0;_LLBA:;}
# 484
if(_tmp1C4 != _tmp1C7){
struct _tuple21 _tmpF4A;struct _tuple21 _tmp1CB=(_tmpF4A.f1=_tmp1C4,((_tmpF4A.f2=_tmp1C7,_tmpF4A)));struct _tuple21 _tmp1CC=_tmp1CB;_LLC4: if(_tmp1CC.f1 != Cyc_Absyn_Aliasable)goto _LLC6;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC6;_LLC5:
 goto _LLC7;_LLC6: if(_tmp1CC.f1 != Cyc_Absyn_Unique)goto _LLC8;if(_tmp1CC.f2 != Cyc_Absyn_Top)goto _LLC8;_LLC7:
 return 1;_LLC8:;_LLC9:
 return 0;_LLC3:;}
# 491
return 1;};}
# 494
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp1CE=Cyc_Absyn_compress_kb(tv->kind);void*_tmp1CF=_tmp1CE;struct Cyc_Absyn_Kind*_tmp1D1;struct Cyc_Absyn_Kind*_tmp1D3;_LLCB: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D0=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1CF;if(_tmp1D0->tag != 0)goto _LLCD;else{_tmp1D1=_tmp1D0->f1;}}_LLCC:
 return _tmp1D1;_LLCD: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1D2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1CF;if(_tmp1D2->tag != 2)goto _LLCF;else{_tmp1D3=_tmp1D2->f2;}}_LLCE:
 return _tmp1D3;_LLCF:;_LLD0:
# 499
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF4D;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpF4C;tv->kind=(void*)((_tmpF4C=_cycalloc(sizeof(*_tmpF4C)),((_tmpF4C[0]=((_tmpF4D.tag=2,((_tmpF4D.f1=0,((_tmpF4D.f2=def,_tmpF4D)))))),_tmpF4C))));}
return def;_LLCA:;}
# 504
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmpF4E;struct _tuple0 _tmp1D6=(_tmpF4E.f1=Cyc_Absyn_compress_kb(kb1),((_tmpF4E.f2=Cyc_Absyn_compress_kb(kb2),_tmpF4E)));struct _tuple0 _tmp1D7=_tmp1D6;struct Cyc_Absyn_Kind*_tmp1D9;struct Cyc_Absyn_Kind*_tmp1DB;struct Cyc_Core_Opt**_tmp1DD;struct Cyc_Absyn_Kind*_tmp1DE;struct Cyc_Absyn_Kind*_tmp1E0;struct Cyc_Absyn_Kind*_tmp1E2;struct Cyc_Core_Opt**_tmp1E4;struct Cyc_Absyn_Kind*_tmp1E5;struct Cyc_Core_Opt**_tmp1E7;struct Cyc_Absyn_Kind*_tmp1E8;struct Cyc_Core_Opt**_tmp1EA;struct Cyc_Absyn_Kind*_tmp1EB;struct Cyc_Core_Opt**_tmp1ED;void*_tmp1EE;void*_tmp1EF;struct Cyc_Core_Opt**_tmp1F1;_LLD2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1D8=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D7.f1;if(_tmp1D8->tag != 0)goto _LLD4;else{_tmp1D9=_tmp1D8->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DA=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D7.f2;if(_tmp1DA->tag != 0)goto _LLD4;else{_tmp1DB=_tmp1DA->f1;}};_LLD3:
 return _tmp1D9 == _tmp1DB;_LLD4:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D7.f1;if(_tmp1DC->tag != 2)goto _LLD6;else{_tmp1DD=(struct Cyc_Core_Opt**)& _tmp1DC->f1;_tmp1DE=_tmp1DC->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1DF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D7.f2;if(_tmp1DF->tag != 0)goto _LLD6;else{_tmp1E0=_tmp1DF->f1;}};_LLD5:
# 508
 if(Cyc_Tcutil_kind_leq(_tmp1E0,_tmp1DE)){
{struct Cyc_Core_Opt*_tmpF4F;*_tmp1DD=((_tmpF4F=_cycalloc(sizeof(*_tmpF4F)),((_tmpF4F->v=kb2,_tmpF4F))));}
return 1;}else{
return 0;}_LLD6:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp1E1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp1D7.f1;if(_tmp1E1->tag != 0)goto _LLD8;else{_tmp1E2=_tmp1E1->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D7.f2;if(_tmp1E3->tag != 2)goto _LLD8;else{_tmp1E4=(struct Cyc_Core_Opt**)& _tmp1E3->f1;_tmp1E5=_tmp1E3->f2;}};_LLD7:
# 513
 if(Cyc_Tcutil_kind_leq(_tmp1E2,_tmp1E5)){
{struct Cyc_Core_Opt*_tmpF50;*_tmp1E4=((_tmpF50=_cycalloc(sizeof(*_tmpF50)),((_tmpF50->v=kb1,_tmpF50))));}
return 1;}else{
return 0;}_LLD8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D7.f1;if(_tmp1E6->tag != 2)goto _LLDA;else{_tmp1E7=(struct Cyc_Core_Opt**)& _tmp1E6->f1;_tmp1E8=_tmp1E6->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1E9=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp1D7.f2;if(_tmp1E9->tag != 2)goto _LLDA;else{_tmp1EA=(struct Cyc_Core_Opt**)& _tmp1E9->f1;_tmp1EB=_tmp1E9->f2;}};_LLD9:
# 518
 if(Cyc_Tcutil_kind_leq(_tmp1E8,_tmp1EB)){
{struct Cyc_Core_Opt*_tmpF51;*_tmp1EA=((_tmpF51=_cycalloc(sizeof(*_tmpF51)),((_tmpF51->v=kb1,_tmpF51))));}
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp1EB,_tmp1E8)){
{struct Cyc_Core_Opt*_tmpF52;*_tmp1E7=((_tmpF52=_cycalloc(sizeof(*_tmpF52)),((_tmpF52->v=kb2,_tmpF52))));}
return 1;}else{
return 0;}}_LLDA:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1EC=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D7.f1;if(_tmp1EC->tag != 1)goto _LLDC;else{_tmp1ED=(struct Cyc_Core_Opt**)& _tmp1EC->f1;}}_tmp1EE=_tmp1D7.f2;_LLDB:
 _tmp1EF=_tmp1EE;_tmp1F1=_tmp1ED;goto _LLDD;_LLDC: _tmp1EF=_tmp1D7.f1;{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp1F0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp1D7.f2;if(_tmp1F0->tag != 1)goto _LLD1;else{_tmp1F1=(struct Cyc_Core_Opt**)& _tmp1F0->f1;}};_LLDD:
# 527
{struct Cyc_Core_Opt*_tmpF53;*_tmp1F1=((_tmpF53=_cycalloc(sizeof(*_tmpF53)),((_tmpF53->v=_tmp1EF,_tmpF53))));}
return 1;_LLD1:;}
# 532
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp1F8=Cyc_Tcutil_compress(t);void*_tmp1F9=_tmp1F8;struct Cyc_Absyn_Tvar*_tmp1FB;_LLDF: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1FA=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F9;if(_tmp1FA->tag != 2)goto _LLE1;else{_tmp1FB=_tmp1FA->f1;}}_LLE0: {
# 535
void*_tmp1FC=_tmp1FB->kind;
_tmp1FB->kind=kb;{
struct _tuple16 _tmpF54;return(_tmpF54.f1=_tmp1FB,((_tmpF54.f2=_tmp1FC,_tmpF54)));};}_LLE1:;_LLE2: {
# 539
const char*_tmpF58;void*_tmpF57[1];struct Cyc_String_pa_PrintArg_struct _tmpF56;(_tmpF56.tag=0,((_tmpF56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF57[0]=& _tmpF56,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF58="swap_kind: cannot update the kind of %s",_tag_dyneither(_tmpF58,sizeof(char),40))),_tag_dyneither(_tmpF57,sizeof(void*),1)))))));}_LLDE:;}
# 545
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 547
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 551
struct _RegionHandle _tmp201=_new_region("temp");struct _RegionHandle*temp=& _tmp201;_push_region(temp);
{struct Cyc_List_List*_tmp202=0;
# 554
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp203=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp204=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp205=Cyc_Tcutil_tvar_kind(_tmp203,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp206=_tmp205;_LLE4: if(_tmp206->kind != Cyc_Absyn_IntKind)goto _LLE6;_LLE5:
 goto _LLE7;_LLE6: if(_tmp206->kind != Cyc_Absyn_AnyKind)goto _LLE8;_LLE7:
# 560
{struct _tuple16*_tmpF5B;struct Cyc_List_List*_tmpF5A;_tmp202=((_tmpF5A=_region_malloc(temp,sizeof(*_tmpF5A)),((_tmpF5A->hd=((_tmpF5B=_region_malloc(temp,sizeof(*_tmpF5B)),((_tmpF5B->f1=_tmp203,((_tmpF5B->f2=_tmp204,_tmpF5B)))))),((_tmpF5A->tl=_tmp202,_tmpF5A))))));}goto _LLE3;_LLE8:;_LLE9:
 goto _LLE3;_LLE3:;}
# 564
if(_tmp202 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp202),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 552
;_pop_region(temp);}
# 569
return k;}
# 576
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp209=Cyc_Tcutil_compress(t);void*_tmp20A=_tmp209;struct Cyc_Core_Opt*_tmp20C;struct Cyc_Absyn_Tvar*_tmp20E;enum Cyc_Absyn_Size_of _tmp211;struct Cyc_Absyn_Datatypedecl*_tmp21D;struct Cyc_Absyn_Datatypefield*_tmp21E;enum Cyc_Absyn_AggrKind _tmp222;struct Cyc_List_List*_tmp223;struct Cyc_Absyn_AggrdeclImpl*_tmp224;struct Cyc_List_List*_tmp225;struct Cyc_Absyn_PtrInfo _tmp228;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Typedefdecl*_tmp22F;_LLEB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp20B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20A;if(_tmp20B->tag != 1)goto _LLED;else{_tmp20C=_tmp20B->f1;}}_LLEC:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp20C))->v;_LLED: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp20D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp20A;if(_tmp20D->tag != 2)goto _LLEF;else{_tmp20E=_tmp20D->f1;}}_LLEE:
 return Cyc_Tcutil_tvar_kind(_tmp20E,& Cyc_Tcutil_bk);_LLEF: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp20F=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp20A;if(_tmp20F->tag != 0)goto _LLF1;}_LLF0:
 return& Cyc_Tcutil_mk;_LLF1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp20A;if(_tmp210->tag != 6)goto _LLF3;else{_tmp211=_tmp210->f2;}}_LLF2:
# 582
 return(_tmp211 == Cyc_Absyn_Int_sz  || _tmp211 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;_LLF3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp20A;if(_tmp212->tag != 7)goto _LLF5;}_LLF4:
# 584
 goto _LLF6;_LLF5: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp20A;if(_tmp213->tag != 9)goto _LLF7;}_LLF6:
 return& Cyc_Tcutil_mk;_LLF7: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp20A;if(_tmp214->tag != 16)goto _LLF9;}_LLF8:
 goto _LLFA;_LLF9: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp215=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp20A;if(_tmp215->tag != 13)goto _LLFB;}_LLFA:
 goto _LLFC;_LLFB: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp216=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp20A;if(_tmp216->tag != 14)goto _LLFD;}_LLFC:
 goto _LLFE;_LLFD: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp217=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp20A;if(_tmp217->tag != 15)goto _LLFF;}_LLFE:
 return& Cyc_Tcutil_bk;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp218=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp20A;if(_tmp218->tag != 21)goto _LL101;}_LL100:
 return& Cyc_Tcutil_urk;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp219=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp20A;if(_tmp219->tag != 20)goto _LL103;}_LL102:
 return& Cyc_Tcutil_rk;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp21A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp20A;if(_tmp21A->tag != 22)goto _LL105;}_LL104:
 return& Cyc_Tcutil_trk;_LL105: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp21B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp20A;if(_tmp21B->tag != 3)goto _LL107;}_LL106:
# 595
 return& Cyc_Tcutil_ak;_LL107: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp20A;if(_tmp21C->tag != 4)goto _LL109;else{if((((_tmp21C->f1).field_info).KnownDatatypefield).tag != 2)goto _LL109;_tmp21D=((struct _tuple3)(((_tmp21C->f1).field_info).KnownDatatypefield).val).f1;_tmp21E=((struct _tuple3)(((_tmp21C->f1).field_info).KnownDatatypefield).val).f2;}}_LL108:
# 597
 return& Cyc_Tcutil_mk;_LL109: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp21F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp20A;if(_tmp21F->tag != 4)goto _LL10B;else{if((((_tmp21F->f1).field_info).UnknownDatatypefield).tag != 1)goto _LL10B;}}_LL10A: {
# 599
const char*_tmpF5E;void*_tmpF5D;(_tmpF5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF5E="typ_kind: Unresolved DatatypeFieldType",_tag_dyneither(_tmpF5E,sizeof(char),39))),_tag_dyneither(_tmpF5D,sizeof(void*),0)));}_LL10B: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp220=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp20A;if(_tmp220->tag != 11)goto _LL10D;else{if((((_tmp220->f1).aggr_info).UnknownAggr).tag != 1)goto _LL10D;}}_LL10C:
# 602
 return& Cyc_Tcutil_ak;_LL10D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp20A;if(_tmp221->tag != 11)goto _LL10F;else{if((((_tmp221->f1).aggr_info).KnownAggr).tag != 2)goto _LL10F;_tmp222=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp221->f1).aggr_info).KnownAggr).val))->kind;_tmp223=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp221->f1).aggr_info).KnownAggr).val))->tvs;_tmp224=(*((struct Cyc_Absyn_Aggrdecl**)(((_tmp221->f1).aggr_info).KnownAggr).val))->impl;_tmp225=(_tmp221->f1).targs;}}_LL10E:
# 604
 if(_tmp224 == 0)return& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp23B=_tmp224->fields;
if(_tmp23B == 0)return& Cyc_Tcutil_mk;
# 608
if(_tmp222 == Cyc_Absyn_StructA){
for(0;_tmp23B->tl != 0;_tmp23B=_tmp23B->tl){;}{
void*_tmp23C=((struct Cyc_Absyn_Aggrfield*)_tmp23B->hd)->type;
struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_field_kind(_tmp23C,_tmp225,_tmp223);
if(_tmp23D == & Cyc_Tcutil_ak  || _tmp23D == & Cyc_Tcutil_tak)return _tmp23D;};}else{
# 616
for(0;_tmp23B != 0;_tmp23B=_tmp23B->tl){
void*_tmp23E=((struct Cyc_Absyn_Aggrfield*)_tmp23B->hd)->type;
struct Cyc_Absyn_Kind*_tmp23F=Cyc_Tcutil_field_kind(_tmp23E,_tmp225,_tmp223);
if(_tmp23F == & Cyc_Tcutil_ak  || _tmp23F == & Cyc_Tcutil_tak)return _tmp23F;}}
# 622
return& Cyc_Tcutil_mk;};_LL10F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp20A;if(_tmp226->tag != 12)goto _LL111;}_LL110:
 return& Cyc_Tcutil_mk;_LL111: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20A;if(_tmp227->tag != 5)goto _LL113;else{_tmp228=_tmp227->f1;}}_LL112: {
# 625
void*_tmp240=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp228.ptr_atts).bounds);void*_tmp241=_tmp240;_LL12A: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp242=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp241;if(_tmp242->tag != 0)goto _LL12C;}_LL12B: {
# 627
enum Cyc_Absyn_AliasQual _tmp244=(Cyc_Tcutil_typ_kind((_tmp228.ptr_atts).rgn))->aliasqual;switch(_tmp244){case Cyc_Absyn_Aliasable: _LL12E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL12F:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL130:
 return& Cyc_Tcutil_tmk;}}_LL12C: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp243=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp241;if(_tmp243->tag != 1)goto _LL129;}_LL12D: {
# 633
enum Cyc_Absyn_AliasQual _tmp245=(Cyc_Tcutil_typ_kind((_tmp228.ptr_atts).rgn))->aliasqual;switch(_tmp245){case Cyc_Absyn_Aliasable: _LL132:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL133:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL134:
 return& Cyc_Tcutil_tbk;}}_LL129:;}_LL113: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp20A;if(_tmp229->tag != 18)goto _LL115;}_LL114:
# 639
 return& Cyc_Tcutil_ik;_LL115: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp20A;if(_tmp22A->tag != 19)goto _LL117;}_LL116:
 return& Cyc_Tcutil_bk;_LL117: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp20A;if(_tmp22B->tag != 8)goto _LL119;else{_tmp22C=(_tmp22B->f1).num_elts;}}_LL118:
# 642
 if(_tmp22C == 0  || Cyc_Tcutil_is_const_exp(_tmp22C))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;_LL119: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20A;if(_tmp22D->tag != 10)goto _LL11B;}_LL11A:
 return& Cyc_Tcutil_mk;_LL11B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp20A;if(_tmp22E->tag != 17)goto _LL11D;else{_tmp22F=_tmp22E->f3;}}_LL11C:
# 646
 if(_tmp22F == 0  || _tmp22F->kind == 0){
const char*_tmpF62;void*_tmpF61[1];struct Cyc_String_pa_PrintArg_struct _tmpF60;(_tmpF60.tag=0,((_tmpF60.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpF61[0]=& _tmpF60,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF62="typ_kind: typedef found: %s",_tag_dyneither(_tmpF62,sizeof(char),28))),_tag_dyneither(_tmpF61,sizeof(void*),1)))))));}
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp22F->kind))->v;_LL11D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp230=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp20A;if(_tmp230->tag != 26)goto _LL11F;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp231=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp230->f1)->r;if(_tmp231->tag != 0)goto _LL11F;}}_LL11E:
 return& Cyc_Tcutil_ak;_LL11F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp20A;if(_tmp232->tag != 26)goto _LL121;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp233=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp232->f1)->r;if(_tmp233->tag != 1)goto _LL121;}}_LL120:
 return& Cyc_Tcutil_bk;_LL121: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp20A;if(_tmp234->tag != 26)goto _LL123;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp235=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp234->f1)->r;if(_tmp235->tag != 2)goto _LL123;}}_LL122:
 return& Cyc_Tcutil_ak;_LL123: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp20A;if(_tmp236->tag != 23)goto _LL125;}_LL124:
 goto _LL126;_LL125: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp20A;if(_tmp237->tag != 24)goto _LL127;}_LL126:
 goto _LL128;_LL127: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp20A;if(_tmp238->tag != 25)goto _LLEA;}_LL128:
 return& Cyc_Tcutil_ek;_LLEA:;}
# 658
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 663
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp249;_push_handler(& _tmp249);{int _tmp24B=0;if(setjmp(_tmp249.handler))_tmp24B=1;if(!_tmp24B){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp24C=1;_npop_handler(0);return _tmp24C;};
# 665
;_pop_handler();}else{void*_tmp24A=(void*)_exn_thrown;void*_tmp24E=_tmp24A;void*_tmp250;_LL137: {struct Cyc_Tcutil_Unify_exn_struct*_tmp24F=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp24E;if(_tmp24F->tag != Cyc_Tcutil_Unify)goto _LL139;}_LL138:
# 667
 return 0;_LL139: _tmp250=_tmp24E;_LL13A:(void)_throw(_tmp250);_LL136:;}};}
# 672
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp251=t;struct Cyc_Absyn_Tvar*_tmp253;void*_tmp255;struct Cyc_Core_Opt**_tmp256;struct Cyc_Absyn_PtrInfo _tmp258;void*_tmp25A;struct Cyc_List_List*_tmp25C;void*_tmp25D;struct Cyc_Absyn_Tqual _tmp25E;void*_tmp25F;struct Cyc_List_List*_tmp260;int _tmp261;struct Cyc_Absyn_VarargInfo*_tmp262;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp266;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp26C;struct Cyc_List_List*_tmp26E;struct Cyc_List_List*_tmp270;void*_tmp272;void*_tmp274;void*_tmp276;void*_tmp278;struct Cyc_List_List*_tmp27A;_LL13C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp252=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp251;if(_tmp252->tag != 2)goto _LL13E;else{_tmp253=_tmp252->f1;}}_LL13D:
# 677
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp253)){
{const char*_tmpF63;Cyc_Tcutil_failure_reason=((_tmpF63="(type variable would escape scope)",_tag_dyneither(_tmpF63,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 681
goto _LL13B;_LL13E: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp254=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp251;if(_tmp254->tag != 1)goto _LL140;else{_tmp255=(void*)_tmp254->f2;_tmp256=(struct Cyc_Core_Opt**)& _tmp254->f4;}}_LL13F:
# 683
 if(t == evar){
{const char*_tmpF64;Cyc_Tcutil_failure_reason=((_tmpF64="(occurs check)",_tag_dyneither(_tmpF64,sizeof(char),15)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 687
if(_tmp255 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp255);else{
# 690
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp256))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 697
if(problem){
struct Cyc_List_List*_tmp27D=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp256))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
struct Cyc_List_List*_tmpF65;_tmp27D=((_tmpF65=_cycalloc(sizeof(*_tmpF65)),((_tmpF65->hd=(struct Cyc_Absyn_Tvar*)s->hd,((_tmpF65->tl=_tmp27D,_tmpF65))))));}}}{
# 703
struct Cyc_Core_Opt*_tmpF66;*_tmp256=((_tmpF66=_cycalloc(sizeof(*_tmpF66)),((_tmpF66->v=_tmp27D,_tmpF66))));};}}}
# 706
goto _LL13B;_LL140: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp257=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp251;if(_tmp257->tag != 5)goto _LL142;else{_tmp258=_tmp257->f1;}}_LL141:
# 709
 Cyc_Tcutil_occurs(evar,r,env,_tmp258.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp258.ptr_atts).rgn);
goto _LL13B;_LL142: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp259=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp251;if(_tmp259->tag != 8)goto _LL144;else{_tmp25A=(_tmp259->f1).elt_type;}}_LL143:
# 713
 Cyc_Tcutil_occurs(evar,r,env,_tmp25A);goto _LL13B;_LL144: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp25B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp251;if(_tmp25B->tag != 9)goto _LL146;else{_tmp25C=(_tmp25B->f1).tvars;_tmp25D=(_tmp25B->f1).effect;_tmp25E=(_tmp25B->f1).ret_tqual;_tmp25F=(_tmp25B->f1).ret_typ;_tmp260=(_tmp25B->f1).args;_tmp261=(_tmp25B->f1).c_varargs;_tmp262=(_tmp25B->f1).cyc_varargs;_tmp263=(_tmp25B->f1).rgn_po;_tmp264=(_tmp25B->f1).attributes;}}_LL145:
# 715
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp25C,env);
if(_tmp25D != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp25D);
Cyc_Tcutil_occurs(evar,r,env,_tmp25F);
for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp260->hd)).f3);}
if(_tmp262 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp262->type);
for(0;_tmp263 != 0;_tmp263=_tmp263->tl){
struct _tuple0*_tmp280=(struct _tuple0*)_tmp263->hd;void*_tmp282;void*_tmp283;struct _tuple0*_tmp281=_tmp280;_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;
Cyc_Tcutil_occurs(evar,r,env,_tmp282);
Cyc_Tcutil_occurs(evar,r,env,_tmp283);}
# 727
goto _LL13B;_LL146: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp251;if(_tmp265->tag != 10)goto _LL148;else{_tmp266=_tmp265->f1;}}_LL147:
# 729
 for(0;_tmp266 != 0;_tmp266=_tmp266->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp266->hd)).f2);}
goto _LL13B;_LL148: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp267=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp251;if(_tmp267->tag != 3)goto _LL14A;else{_tmp268=(_tmp267->f1).targs;}}_LL149:
# 733
 Cyc_Tcutil_occurslist(evar,r,env,_tmp268);goto _LL13B;_LL14A: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp269=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp251;if(_tmp269->tag != 17)goto _LL14C;else{_tmp26A=_tmp269->f2;}}_LL14B:
 _tmp26C=_tmp26A;goto _LL14D;_LL14C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp26B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp251;if(_tmp26B->tag != 4)goto _LL14E;else{_tmp26C=(_tmp26B->f1).targs;}}_LL14D:
 _tmp26E=_tmp26C;goto _LL14F;_LL14E: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp26D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251;if(_tmp26D->tag != 11)goto _LL150;else{_tmp26E=(_tmp26D->f1).targs;}}_LL14F:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp26E);goto _LL13B;_LL150: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp26F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251;if(_tmp26F->tag != 12)goto _LL152;else{_tmp270=_tmp26F->f2;}}_LL151:
# 739
 for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp270->hd)->type);}
goto _LL13B;_LL152: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp271=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp251;if(_tmp271->tag != 19)goto _LL154;else{_tmp272=(void*)_tmp271->f1;}}_LL153:
 _tmp274=_tmp272;goto _LL155;_LL154: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp273=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp251;if(_tmp273->tag != 23)goto _LL156;else{_tmp274=(void*)_tmp273->f1;}}_LL155:
 _tmp276=_tmp274;goto _LL157;_LL156: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp275=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp251;if(_tmp275->tag != 15)goto _LL158;else{_tmp276=(void*)_tmp275->f1;}}_LL157:
 _tmp278=_tmp276;goto _LL159;_LL158: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp277=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp251;if(_tmp277->tag != 25)goto _LL15A;else{_tmp278=(void*)_tmp277->f1;}}_LL159:
 Cyc_Tcutil_occurs(evar,r,env,_tmp278);goto _LL13B;_LL15A: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp279=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp251;if(_tmp279->tag != 24)goto _LL15C;else{_tmp27A=_tmp279->f1;}}_LL15B:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp27A);goto _LL13B;_LL15C:;_LL15D:
# 748
 goto _LL13B;_LL13B:;};}
# 751
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 753
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 758
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 766
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const){
const char*_tmpF69;void*_tmpF68;(_tmpF68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF69="tq1 real_const not set.",_tag_dyneither(_tmpF69,sizeof(char),24))),_tag_dyneither(_tmpF68,sizeof(void*),0)));}
if(tq2.print_const  && !tq2.real_const){
const char*_tmpF6C;void*_tmpF6B;(_tmpF6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpF6C="tq2 real_const not set.",_tag_dyneither(_tmpF6C,sizeof(char),24))),_tag_dyneither(_tmpF6B,sizeof(void*),0)));}
# 772
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 775
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
{const char*_tmpF6D;Cyc_Tcutil_failure_reason=((_tmpF6D="(qualifiers don't match)",_tag_dyneither(_tmpF6D,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 783
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 787
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 794
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 796
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp289=x;void*_tmp28A;_LL15F: if((_tmp289->No_constr).tag != 3)goto _LL161;_LL160:
# 801
{union Cyc_Absyn_Constraint _tmpF6E;*x=(((_tmpF6E.Forward_constr).val=y,(((_tmpF6E.Forward_constr).tag=2,_tmpF6E))));}return;_LL161: if((_tmp289->Eq_constr).tag != 1)goto _LL163;_tmp28A=(void*)(_tmp289->Eq_constr).val;_LL162: {
# 803
union Cyc_Absyn_Constraint*_tmp28C=y;void*_tmp28D;_LL166: if((_tmp28C->No_constr).tag != 3)goto _LL168;_LL167:
*y=*x;return;_LL168: if((_tmp28C->Eq_constr).tag != 1)goto _LL16A;_tmp28D=(void*)(_tmp28C->Eq_constr).val;_LL169:
# 806
 if(cmp(_tmp28A,_tmp28D)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 810
return;_LL16A: if((_tmp28C->Forward_constr).tag != 2)goto _LL165;_LL16B: {
const char*_tmpF71;void*_tmpF70;(_tmpF70=0,Cyc_Tcutil_impos(((_tmpF71="unify_conref: forward after compress(2)",_tag_dyneither(_tmpF71,sizeof(char),40))),_tag_dyneither(_tmpF70,sizeof(void*),0)));}_LL165:;}_LL163: if((_tmp289->Forward_constr).tag != 2)goto _LL15E;_LL164: {
# 813
const char*_tmpF74;void*_tmpF73;(_tmpF73=0,Cyc_Tcutil_impos(((_tmpF74="unify_conref: forward after compress",_tag_dyneither(_tmpF74,sizeof(char),37))),_tag_dyneither(_tmpF73,sizeof(void*),0)));}_LL15E:;};}
# 817
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp292;_push_handler(& _tmp292);{int _tmp294=0;if(setjmp(_tmp292.handler))_tmp294=1;if(!_tmp294){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp295=1;_npop_handler(0);return _tmp295;};
# 819
;_pop_handler();}else{void*_tmp293=(void*)_exn_thrown;void*_tmp297=_tmp293;void*_tmp299;_LL16D: {struct Cyc_Tcutil_Unify_exn_struct*_tmp298=(struct Cyc_Tcutil_Unify_exn_struct*)_tmp297;if(_tmp298->tag != Cyc_Tcutil_Unify)goto _LL16F;}_LL16E:
# 821
 return 0;_LL16F: _tmp299=_tmp297;_LL170:(void)_throw(_tmp299);_LL16C:;}};}
# 824
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmpF75;struct _tuple0 _tmp29A=(_tmpF75.f1=b1,((_tmpF75.f2=b2,_tmpF75)));struct _tuple0 _tmp29B=_tmp29A;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A3;_LL172:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29C->tag != 0)goto _LL174;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29D=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29D->tag != 0)goto _LL174;};_LL173:
 return 0;_LL174: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp29E->tag != 0)goto _LL176;}_LL175:
 return - 1;_LL176: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp29F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp29F->tag != 0)goto _LL178;}_LL177:
 return 1;_LL178:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f1;if(_tmp2A0->tag != 1)goto _LL171;else{_tmp2A1=_tmp2A0->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2A2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29B.f2;if(_tmp2A2->tag != 1)goto _LL171;else{_tmp2A3=_tmp2A2->f1;}};_LL179:
# 830
 return Cyc_Evexp_const_exp_cmp(_tmp2A1,_tmp2A3);_LL171:;}
# 834
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmpF76;struct _tuple0 _tmp2A5=(_tmpF76.f1=b1,((_tmpF76.f2=b2,_tmpF76)));struct _tuple0 _tmp2A6=_tmp2A5;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AE;_LL17B:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A6.f1;if(_tmp2A7->tag != 0)goto _LL17D;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A6.f2;if(_tmp2A8->tag != 0)goto _LL17D;};_LL17C:
 return 0;_LL17D: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2A9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A6.f1;if(_tmp2A9->tag != 0)goto _LL17F;}_LL17E:
 return - 1;_LL17F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp2AA=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp2A6.f2;if(_tmp2AA->tag != 0)goto _LL181;}_LL180:
 return 1;_LL181:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A6.f1;if(_tmp2AB->tag != 1)goto _LL17A;else{_tmp2AC=_tmp2AB->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp2AD=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2A6.f2;if(_tmp2AD->tag != 1)goto _LL17A;else{_tmp2AE=_tmp2AD->f1;}};_LL182:
 return Cyc_Evexp_const_exp_cmp(_tmp2AC,_tmp2AE);_LL17A:;}
# 843
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp2B0=att;_LL184: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B1->tag != 0)goto _LL186;}_LL185:
 return 0;_LL186: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2B2=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B2->tag != 1)goto _LL188;}_LL187:
 return 1;_LL188: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2B3=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B3->tag != 2)goto _LL18A;}_LL189:
 return 2;_LL18A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2B4=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B4->tag != 3)goto _LL18C;}_LL18B:
 return 3;_LL18C: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp2B5=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B5->tag != 4)goto _LL18E;}_LL18D:
 return 4;_LL18E: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp2B6=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B6->tag != 5)goto _LL190;}_LL18F:
 return 5;_LL190: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2B7=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B7->tag != 6)goto _LL192;}_LL191:
 return 6;_LL192: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp2B8=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B8->tag != 7)goto _LL194;}_LL193:
 return 7;_LL194: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2B9->tag != 8)goto _LL196;}_LL195:
 return 8;_LL196: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BA->tag != 9)goto _LL198;}_LL197:
 return 9;_LL198: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp2BB=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BB->tag != 10)goto _LL19A;}_LL199:
 return 10;_LL19A: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BC->tag != 11)goto _LL19C;}_LL19B:
 return 11;_LL19C: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp2BD=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BD->tag != 12)goto _LL19E;}_LL19D:
 return 12;_LL19E: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp2BE=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BE->tag != 13)goto _LL1A0;}_LL19F:
 return 13;_LL1A0: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp2BF=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2BF->tag != 14)goto _LL1A2;}_LL1A1:
 return 14;_LL1A2: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp2C0=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C0->tag != 15)goto _LL1A4;}_LL1A3:
 return 15;_LL1A4: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp2C1=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C1->tag != 16)goto _LL1A6;}_LL1A5:
 return 16;_LL1A6: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp2C2=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C2->tag != 17)goto _LL1A8;}_LL1A7:
 return 17;_LL1A8: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp2C3=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C3->tag != 18)goto _LL1AA;}_LL1A9:
 return 18;_LL1AA: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2C4=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C4->tag != 19)goto _LL1AC;}_LL1AB:
 return 19;_LL1AC: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2C5=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2B0;if(_tmp2C5->tag != 20)goto _LL1AE;}_LL1AD:
 return 20;_LL1AE:;_LL1AF:
 return 21;_LL183:;}
# 870
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmpF77;struct _tuple0 _tmp2C6=(_tmpF77.f1=att1,((_tmpF77.f2=att2,_tmpF77)));struct _tuple0 _tmp2C7=_tmp2C6;int _tmp2C9;int _tmp2CB;int _tmp2CD;int _tmp2CF;int _tmp2D1;int _tmp2D3;struct _dyneither_ptr _tmp2D5;struct _dyneither_ptr _tmp2D7;enum Cyc_Absyn_Format_Type _tmp2D9;int _tmp2DA;int _tmp2DB;enum Cyc_Absyn_Format_Type _tmp2DD;int _tmp2DE;int _tmp2DF;_LL1B1:{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2C8=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2C7.f1;if(_tmp2C8->tag != 0)goto _LL1B3;else{_tmp2C9=_tmp2C8->f1;}}{struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp2CA=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp2C7.f2;if(_tmp2CA->tag != 0)goto _LL1B3;else{_tmp2CB=_tmp2CA->f1;}};_LL1B2:
 _tmp2CD=_tmp2C9;_tmp2CF=_tmp2CB;goto _LL1B4;_LL1B3:{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2CC=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2C7.f1;if(_tmp2CC->tag != 20)goto _LL1B5;else{_tmp2CD=_tmp2CC->f1;}}{struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp2CE=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp2C7.f2;if(_tmp2CE->tag != 20)goto _LL1B5;else{_tmp2CF=_tmp2CE->f1;}};_LL1B4:
 _tmp2D1=_tmp2CD;_tmp2D3=_tmp2CF;goto _LL1B6;_LL1B5:{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2D0=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2C7.f1;if(_tmp2D0->tag != 6)goto _LL1B7;else{_tmp2D1=_tmp2D0->f1;}}{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp2D2=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp2C7.f2;if(_tmp2D2->tag != 6)goto _LL1B7;else{_tmp2D3=_tmp2D2->f1;}};_LL1B6:
 return Cyc_Core_intcmp(_tmp2D1,_tmp2D3);_LL1B7:{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2D4=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2C7.f1;if(_tmp2D4->tag != 8)goto _LL1B9;else{_tmp2D5=_tmp2D4->f1;}}{struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp2D6=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp2C7.f2;if(_tmp2D6->tag != 8)goto _LL1B9;else{_tmp2D7=_tmp2D6->f1;}};_LL1B8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2D5,(struct _dyneither_ptr)_tmp2D7);_LL1B9:{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2D8=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2C7.f1;if(_tmp2D8->tag != 19)goto _LL1BB;else{_tmp2D9=_tmp2D8->f1;_tmp2DA=_tmp2D8->f2;_tmp2DB=_tmp2D8->f3;}}{struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp2DC=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp2C7.f2;if(_tmp2DC->tag != 19)goto _LL1BB;else{_tmp2DD=_tmp2DC->f1;_tmp2DE=_tmp2DC->f2;_tmp2DF=_tmp2DC->f3;}};_LL1BA: {
# 877
int _tmp2E0=Cyc_Core_intcmp((int)((unsigned int)_tmp2D9),(int)((unsigned int)_tmp2DD));
if(_tmp2E0 != 0)return _tmp2E0;{
int _tmp2E1=Cyc_Core_intcmp(_tmp2DA,_tmp2DE);
if(_tmp2E1 != 0)return _tmp2E1;
return Cyc_Core_intcmp(_tmp2DB,_tmp2DF);};}_LL1BB:;_LL1BC:
# 883
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));_LL1B0:;}
# 887
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 891
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 900
static void*Cyc_Tcutil_rgns_of(void*t);
# 902
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 906
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp2E3=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp2E4=_tmp2E3;_LL1BE: if(_tmp2E4->kind != Cyc_Absyn_RgnKind)goto _LL1C0;if(_tmp2E4->aliasqual != Cyc_Absyn_Unique)goto _LL1C0;_LL1BF:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1BD;_LL1C0: if(_tmp2E4->kind != Cyc_Absyn_RgnKind)goto _LL1C2;if(_tmp2E4->aliasqual != Cyc_Absyn_Aliasable)goto _LL1C2;_LL1C1:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1BD;_LL1C2: if(_tmp2E4->kind != Cyc_Absyn_EffKind)goto _LL1C4;_LL1C3:
 t=Cyc_Absyn_empty_effect;goto _LL1BD;_LL1C4: if(_tmp2E4->kind != Cyc_Absyn_IntKind)goto _LL1C6;_LL1C5:
{struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpF7A;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF79;t=(void*)((_tmpF79=_cycalloc(sizeof(*_tmpF79)),((_tmpF79[0]=((_tmpF7A.tag=18,((_tmpF7A.f1=Cyc_Absyn_uint_exp(0,0),_tmpF7A)))),_tmpF79))));}goto _LL1BD;_LL1C6:;_LL1C7:
 t=Cyc_Absyn_sint_typ;goto _LL1BD;_LL1BD:;}{
# 915
struct _tuple16*_tmpF7B;return(_tmpF7B=_cycalloc(sizeof(*_tmpF7B)),((_tmpF7B->f1=tv,((_tmpF7B->f2=t,_tmpF7B)))));};}
# 922
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp2E8=Cyc_Tcutil_compress(t);void*_tmp2E9=_tmp2E8;void*_tmp2F3;void*_tmp2F5;void*_tmp2F6;struct Cyc_List_List*_tmp2F8;void*_tmp2FA;void*_tmp2FB;void*_tmp2FD;struct Cyc_List_List*_tmp2FF;struct Cyc_List_List*_tmp301;struct Cyc_List_List*_tmp303;struct Cyc_List_List*_tmp305;struct Cyc_List_List*_tmp308;void*_tmp309;struct Cyc_Absyn_Tqual _tmp30A;void*_tmp30B;struct Cyc_List_List*_tmp30C;struct Cyc_Absyn_VarargInfo*_tmp30D;struct Cyc_List_List*_tmp30E;void*_tmp315;struct Cyc_List_List*_tmp317;_LL1C9: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp2EA=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EA->tag != 0)goto _LL1CB;}_LL1CA:
 goto _LL1CC;_LL1CB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp2EB=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EB->tag != 7)goto _LL1CD;}_LL1CC:
 goto _LL1CE;_LL1CD: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp2EC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EC->tag != 13)goto _LL1CF;}_LL1CE:
 goto _LL1D0;_LL1CF: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp2ED=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp2E9;if(_tmp2ED->tag != 14)goto _LL1D1;}_LL1D0:
 goto _LL1D2;_LL1D1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2EE=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EE->tag != 18)goto _LL1D3;}_LL1D2:
 goto _LL1D4;_LL1D3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2E9;if(_tmp2EF->tag != 6)goto _LL1D5;}_LL1D4:
 return Cyc_Absyn_empty_effect;_LL1D5: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2F0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E9;if(_tmp2F0->tag != 1)goto _LL1D7;}_LL1D6:
 goto _LL1D8;_LL1D7: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F1=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E9;if(_tmp2F1->tag != 2)goto _LL1D9;}_LL1D8: {
# 932
struct Cyc_Absyn_Kind*_tmp319=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp31A=_tmp319;_LL200: if(_tmp31A->kind != Cyc_Absyn_RgnKind)goto _LL202;_LL201: {
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF7E;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF7D;return(void*)((_tmpF7D=_cycalloc(sizeof(*_tmpF7D)),((_tmpF7D[0]=((_tmpF7E.tag=23,((_tmpF7E.f1=t,_tmpF7E)))),_tmpF7D))));}_LL202: if(_tmp31A->kind != Cyc_Absyn_EffKind)goto _LL204;_LL203:
 return t;_LL204: if(_tmp31A->kind != Cyc_Absyn_IntKind)goto _LL206;_LL205:
 return Cyc_Absyn_empty_effect;_LL206:;_LL207: {
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpF81;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpF80;return(void*)((_tmpF80=_cycalloc(sizeof(*_tmpF80)),((_tmpF80[0]=((_tmpF81.tag=25,((_tmpF81.f1=t,_tmpF81)))),_tmpF80))));}_LL1FF:;}_LL1D9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp2F2=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2E9;if(_tmp2F2->tag != 15)goto _LL1DB;else{_tmp2F3=(void*)_tmp2F2->f1;}}_LL1DA: {
# 938
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF84;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF83;return(void*)((_tmpF83=_cycalloc(sizeof(*_tmpF83)),((_tmpF83[0]=((_tmpF84.tag=23,((_tmpF84.f1=_tmp2F3,_tmpF84)))),_tmpF83))));}_LL1DB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp2F4=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp2E9;if(_tmp2F4->tag != 16)goto _LL1DD;else{_tmp2F5=(void*)_tmp2F4->f1;_tmp2F6=(void*)_tmp2F4->f2;}}_LL1DC: {
# 942
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF87;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF86;return(void*)((_tmpF86=_cycalloc(sizeof(*_tmpF86)),((_tmpF86[0]=((_tmpF87.tag=23,((_tmpF87.f1=_tmp2F6,_tmpF87)))),_tmpF86))));}_LL1DD: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp2F7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp2E9;if(_tmp2F7->tag != 3)goto _LL1DF;else{_tmp2F8=(_tmp2F7->f1).targs;}}_LL1DE: {
# 944
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp2F8);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF8A;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF89;return Cyc_Tcutil_normalize_effect((void*)((_tmpF89=_cycalloc(sizeof(*_tmpF89)),((_tmpF89[0]=((_tmpF8A.tag=24,((_tmpF8A.f1=ts,_tmpF8A)))),_tmpF89)))));}_LL1DF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2F9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9;if(_tmp2F9->tag != 5)goto _LL1E1;else{_tmp2FA=(_tmp2F9->f1).elt_typ;_tmp2FB=((_tmp2F9->f1).ptr_atts).rgn;}}_LL1E0: {
# 948
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF99;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF98;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF97;void*_tmpF96[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF95;return Cyc_Tcutil_normalize_effect((void*)((_tmpF95=_cycalloc(sizeof(*_tmpF95)),((_tmpF95[0]=((_tmpF99.tag=24,((_tmpF99.f1=((_tmpF96[1]=Cyc_Tcutil_rgns_of(_tmp2FA),((_tmpF96[0]=(void*)((_tmpF98=_cycalloc(sizeof(*_tmpF98)),((_tmpF98[0]=((_tmpF97.tag=23,((_tmpF97.f1=_tmp2FB,_tmpF97)))),_tmpF98)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpF96,sizeof(void*),2)))))),_tmpF99)))),_tmpF95)))));}_LL1E1: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp2FC=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2E9;if(_tmp2FC->tag != 8)goto _LL1E3;else{_tmp2FD=(_tmp2FC->f1).elt_type;}}_LL1E2:
# 950
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp2FD));_LL1E3: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2E9;if(_tmp2FE->tag != 10)goto _LL1E5;else{_tmp2FF=_tmp2FE->f1;}}_LL1E4: {
# 952
struct Cyc_List_List*_tmp32A=0;
for(0;_tmp2FF != 0;_tmp2FF=_tmp2FF->tl){
struct Cyc_List_List*_tmpF9A;_tmp32A=((_tmpF9A=_cycalloc(sizeof(*_tmpF9A)),((_tmpF9A->hd=(*((struct _tuple12*)_tmp2FF->hd)).f2,((_tmpF9A->tl=_tmp32A,_tmpF9A))))));}
_tmp301=_tmp32A;goto _LL1E6;}_LL1E5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp300=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp2E9;if(_tmp300->tag != 4)goto _LL1E7;else{_tmp301=(_tmp300->f1).targs;}}_LL1E6:
 _tmp303=_tmp301;goto _LL1E8;_LL1E7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp302=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2E9;if(_tmp302->tag != 11)goto _LL1E9;else{_tmp303=(_tmp302->f1).targs;}}_LL1E8: {
# 958
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF9D;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF9C;return Cyc_Tcutil_normalize_effect((void*)((_tmpF9C=_cycalloc(sizeof(*_tmpF9C)),((_tmpF9C[0]=((_tmpF9D.tag=24,((_tmpF9D.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp303),_tmpF9D)))),_tmpF9C)))));}_LL1E9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp304=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2E9;if(_tmp304->tag != 12)goto _LL1EB;else{_tmp305=_tmp304->f2;}}_LL1EA: {
# 960
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFA0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF9F;return Cyc_Tcutil_normalize_effect((void*)((_tmpF9F=_cycalloc(sizeof(*_tmpF9F)),((_tmpF9F[0]=((_tmpFA0.tag=24,((_tmpFA0.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp305),_tmpFA0)))),_tmpF9F)))));}_LL1EB: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp306=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2E9;if(_tmp306->tag != 19)goto _LL1ED;}_LL1EC:
 return Cyc_Absyn_empty_effect;_LL1ED: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp307=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2E9;if(_tmp307->tag != 9)goto _LL1EF;else{_tmp308=(_tmp307->f1).tvars;_tmp309=(_tmp307->f1).effect;_tmp30A=(_tmp307->f1).ret_tqual;_tmp30B=(_tmp307->f1).ret_typ;_tmp30C=(_tmp307->f1).args;_tmp30D=(_tmp307->f1).cyc_varargs;_tmp30E=(_tmp307->f1).rgn_po;}}_LL1EE: {
# 970
void*_tmp330=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp308),(void*)_check_null(_tmp309));
return Cyc_Tcutil_normalize_effect(_tmp330);}_LL1EF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp30F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp2E9;if(_tmp30F->tag != 21)goto _LL1F1;}_LL1F0:
 goto _LL1F2;_LL1F1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp310=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp2E9;if(_tmp310->tag != 22)goto _LL1F3;}_LL1F2:
 goto _LL1F4;_LL1F3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp311=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp2E9;if(_tmp311->tag != 20)goto _LL1F5;}_LL1F4:
 return Cyc_Absyn_empty_effect;_LL1F5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp312=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2E9;if(_tmp312->tag != 23)goto _LL1F7;}_LL1F6:
 goto _LL1F8;_LL1F7: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp313=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E9;if(_tmp313->tag != 24)goto _LL1F9;}_LL1F8:
 return t;_LL1F9: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp314=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E9;if(_tmp314->tag != 25)goto _LL1FB;else{_tmp315=(void*)_tmp314->f1;}}_LL1FA:
 return Cyc_Tcutil_rgns_of(_tmp315);_LL1FB: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp316=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2E9;if(_tmp316->tag != 17)goto _LL1FD;else{_tmp317=_tmp316->f2;}}_LL1FC: {
# 979
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFA3;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFA2;return Cyc_Tcutil_normalize_effect((void*)((_tmpFA2=_cycalloc(sizeof(*_tmpFA2)),((_tmpFA2[0]=((_tmpFA3.tag=24,((_tmpFA3.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp317),_tmpFA3)))),_tmpFA2)))));}_LL1FD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp318=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2E9;if(_tmp318->tag != 26)goto _LL1C8;}_LL1FE: {
const char*_tmpFA6;void*_tmpFA5;(_tmpFA5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFA6="typedecl in rgns_of",_tag_dyneither(_tmpFA6,sizeof(char),20))),_tag_dyneither(_tmpFA5,sizeof(void*),0)));}_LL1C8:;}
# 987
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp335=e;struct Cyc_List_List**_tmp337;void*_tmp339;_LL209: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp336=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp335;if(_tmp336->tag != 24)goto _LL20B;else{_tmp337=(struct Cyc_List_List**)& _tmp336->f1;}}_LL20A: {
# 991
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp337;for(0;effs != 0;effs=effs->tl){
void*_tmp33A=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp33A));{
void*_tmp33B=(void*)effs->hd;void*_tmp33C=_tmp33B;_LL210: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp33D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp33C;if(_tmp33D->tag != 24)goto _LL212;}_LL211:
 goto _LL213;_LL212: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp33E=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33C;if(_tmp33E->tag != 23)goto _LL214;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp33E->f1);if(_tmp33F->tag != 20)goto _LL214;}}_LL213:
 redo_join=1;goto _LL20F;_LL214: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33C;if(_tmp340->tag != 23)goto _LL216;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp340->f1);if(_tmp341->tag != 22)goto _LL216;}}_LL215:
 redo_join=1;goto _LL20F;_LL216: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp33C;if(_tmp342->tag != 23)goto _LL218;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp343=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp342->f1);if(_tmp343->tag != 21)goto _LL218;}}_LL217:
 redo_join=1;goto _LL20F;_LL218:;_LL219:
 goto _LL20F;_LL20F:;};}}
# 1003
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp337;for(0;effs != 0;effs=effs->tl){
void*_tmp344=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp345=_tmp344;struct Cyc_List_List*_tmp347;void*_tmp34E;_LL21B: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp346=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp345;if(_tmp346->tag != 24)goto _LL21D;else{_tmp347=_tmp346->f1;}}_LL21C:
# 1008
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp347,effects);
goto _LL21A;_LL21D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp345;if(_tmp348->tag != 23)goto _LL21F;else{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)((void*)_tmp348->f1);if(_tmp349->tag != 20)goto _LL21F;}}_LL21E:
 goto _LL21A;_LL21F: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp345;if(_tmp34A->tag != 23)goto _LL221;else{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)((void*)_tmp34A->f1);if(_tmp34B->tag != 22)goto _LL221;}}_LL220:
 goto _LL21A;_LL221: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp34C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp345;if(_tmp34C->tag != 23)goto _LL223;else{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp34D=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)((void*)_tmp34C->f1);if(_tmp34D->tag != 21)goto _LL223;}}_LL222:
 goto _LL21A;_LL223: _tmp34E=_tmp345;_LL224:
{struct Cyc_List_List*_tmpFA7;effects=((_tmpFA7=_cycalloc(sizeof(*_tmpFA7)),((_tmpFA7->hd=_tmp34E,((_tmpFA7->tl=effects,_tmpFA7))))));}goto _LL21A;_LL21A:;}}
# 1016
*_tmp337=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}_LL20B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp338=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp335;if(_tmp338->tag != 25)goto _LL20D;else{_tmp339=(void*)_tmp338->f1;}}_LL20C: {
# 1019
void*_tmp350=Cyc_Tcutil_compress(_tmp339);void*_tmp351=_tmp350;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp352=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp351;if(_tmp352->tag != 1)goto _LL228;}_LL227:
 goto _LL229;_LL228: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp353=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp351;if(_tmp353->tag != 2)goto _LL22A;}_LL229:
 return e;_LL22A:;_LL22B:
 return Cyc_Tcutil_rgns_of(_tmp339);_LL225:;}_LL20D:;_LL20E:
# 1024
 return e;_LL208:;};}
# 1029
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpFAD;struct Cyc_Absyn_FnInfo _tmpFAC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpFAB;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp354=(_tmpFAB=_cycalloc(sizeof(*_tmpFAB)),((_tmpFAB[0]=((_tmpFAD.tag=9,((_tmpFAD.f1=((_tmpFAC.tvars=0,((_tmpFAC.effect=eff,((_tmpFAC.ret_tqual=
Cyc_Absyn_empty_tqual(0),((_tmpFAC.ret_typ=(void*)& Cyc_Absyn_VoidType_val,((_tmpFAC.args=0,((_tmpFAC.c_varargs=0,((_tmpFAC.cyc_varargs=0,((_tmpFAC.rgn_po=0,((_tmpFAC.attributes=0,_tmpFAC)))))))))))))))))),_tmpFAD)))),_tmpFAB)));
# 1035
return Cyc_Absyn_atb_typ((void*)_tmp354,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1042
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp358=Cyc_Tcutil_compress(e);void*_tmp359=_tmp358;void*_tmp35B;struct Cyc_List_List*_tmp35D;void*_tmp35F;struct Cyc_Core_Opt*_tmp361;void**_tmp362;struct Cyc_Core_Opt*_tmp363;_LL22D: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp35A=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp359;if(_tmp35A->tag != 23)goto _LL22F;else{_tmp35B=(void*)_tmp35A->f1;}}_LL22E:
# 1049
 if(constrain)return Cyc_Tcutil_unify(r,_tmp35B);
_tmp35B=Cyc_Tcutil_compress(_tmp35B);
if(r == _tmp35B)return 1;{
struct _tuple0 _tmpFAE;struct _tuple0 _tmp364=(_tmpFAE.f1=r,((_tmpFAE.f2=_tmp35B,_tmpFAE)));struct _tuple0 _tmp365=_tmp364;struct Cyc_Absyn_Tvar*_tmp367;struct Cyc_Absyn_Tvar*_tmp369;_LL238:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp366=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp365.f1;if(_tmp366->tag != 2)goto _LL23A;else{_tmp367=_tmp366->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp368=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp365.f2;if(_tmp368->tag != 2)goto _LL23A;else{_tmp369=_tmp368->f1;}};_LL239:
 return Cyc_Absyn_tvar_cmp(_tmp367,_tmp369)== 0;_LL23A:;_LL23B:
 return 0;_LL237:;};_LL22F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp35C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp359;if(_tmp35C->tag != 24)goto _LL231;else{_tmp35D=_tmp35C->f1;}}_LL230:
# 1057
 for(0;_tmp35D != 0;_tmp35D=_tmp35D->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp35D->hd))return 1;}
return 0;_LL231: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp35E=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp359;if(_tmp35E->tag != 25)goto _LL233;else{_tmp35F=(void*)_tmp35E->f1;}}_LL232: {
# 1061
void*_tmp36B=Cyc_Tcutil_rgns_of(_tmp35F);void*_tmp36C=_tmp36B;void*_tmp36E;void*_tmp36F;_LL23D: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp36D=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp36C;if(_tmp36D->tag != 25)goto _LL23F;else{_tmp36E=(void*)_tmp36D->f1;}}_LL23E:
# 1063
 if(!constrain)return 0;{
void*_tmp370=Cyc_Tcutil_compress(_tmp36E);void*_tmp371=_tmp370;struct Cyc_Core_Opt*_tmp373;void**_tmp374;struct Cyc_Core_Opt*_tmp375;_LL242: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp372=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp371;if(_tmp372->tag != 1)goto _LL244;else{_tmp373=_tmp372->f1;_tmp374=(void**)((void**)& _tmp372->f2);_tmp375=_tmp372->f4;}}_LL243: {
# 1068
void*_tmp376=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp375);
# 1071
Cyc_Tcutil_occurs(_tmp376,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp375))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFBD;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFBC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFBB;void*_tmpFBA[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFB9;void*_tmp377=Cyc_Tcutil_dummy_fntype((void*)((_tmpFB9=_cycalloc(sizeof(*_tmpFB9)),((_tmpFB9[0]=((_tmpFBD.tag=24,((_tmpFBD.f1=((_tmpFBA[1]=(void*)((_tmpFBC=_cycalloc(sizeof(*_tmpFBC)),((_tmpFBC[0]=((_tmpFBB.tag=23,((_tmpFBB.f1=r,_tmpFBB)))),_tmpFBC)))),((_tmpFBA[0]=_tmp376,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFBA,sizeof(void*),2)))))),_tmpFBD)))),_tmpFB9)))));
*_tmp374=_tmp377;
return 1;};}_LL244:;_LL245:
 return 0;_LL241:;};_LL23F: _tmp36F=_tmp36C;_LL240:
# 1077
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp36F);_LL23C:;}_LL233: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp360=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp359;if(_tmp360->tag != 1)goto _LL235;else{_tmp361=_tmp360->f1;_tmp362=(void**)((void**)& _tmp360->f2);_tmp363=_tmp360->f4;}}_LL234:
# 1080
 if(_tmp361 == 0  || ((struct Cyc_Absyn_Kind*)_tmp361->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpFC0;void*_tmpFBF;(_tmpFBF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFC0="effect evar has wrong kind",_tag_dyneither(_tmpFC0,sizeof(char),27))),_tag_dyneither(_tmpFBF,sizeof(void*),0)));}
if(!constrain)return 0;{
# 1085
void*_tmp37F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp363);
# 1088
Cyc_Tcutil_occurs(_tmp37F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp363))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFD5;struct Cyc_List_List*_tmpFD4;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpFD3;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpFD2;struct Cyc_List_List*_tmpFD1;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFD0;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp380=(_tmpFD0=_cycalloc(sizeof(*_tmpFD0)),((_tmpFD0[0]=((_tmpFD5.tag=24,((_tmpFD5.f1=((_tmpFD1=_cycalloc(sizeof(*_tmpFD1)),((_tmpFD1->hd=_tmp37F,((_tmpFD1->tl=((_tmpFD4=_cycalloc(sizeof(*_tmpFD4)),((_tmpFD4->hd=(void*)((_tmpFD2=_cycalloc(sizeof(*_tmpFD2)),((_tmpFD2[0]=((_tmpFD3.tag=23,((_tmpFD3.f1=r,_tmpFD3)))),_tmpFD2)))),((_tmpFD4->tl=0,_tmpFD4)))))),_tmpFD1)))))),_tmpFD5)))),_tmpFD0)));
*_tmp362=(void*)_tmp380;
return 1;};};_LL235:;_LL236:
 return 0;_LL22C:;};}
# 1099
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp387=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp388=_tmp387;struct Cyc_List_List*_tmp38B;void*_tmp38D;struct Cyc_Core_Opt*_tmp38F;void**_tmp390;struct Cyc_Core_Opt*_tmp391;_LL247: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp389=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp388;if(_tmp389->tag != 23)goto _LL249;}_LL248:
 return 0;_LL249: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp38A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp388;if(_tmp38A->tag != 24)goto _LL24B;else{_tmp38B=_tmp38A->f1;}}_LL24A:
# 1104
 for(0;_tmp38B != 0;_tmp38B=_tmp38B->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp38B->hd))
return 1;}
return 0;_LL24B: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp38C=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp388;if(_tmp38C->tag != 25)goto _LL24D;else{_tmp38D=(void*)_tmp38C->f1;}}_LL24C:
# 1109
 _tmp38D=Cyc_Tcutil_compress(_tmp38D);
if(t == _tmp38D)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp38D);{
void*_tmp392=Cyc_Tcutil_rgns_of(t);void*_tmp393=_tmp392;void*_tmp395;void*_tmp396;_LL252: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp394=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp393;if(_tmp394->tag != 25)goto _LL254;else{_tmp395=(void*)_tmp394->f1;}}_LL253: {
# 1114
struct _tuple0 _tmpFD6;struct _tuple0 _tmp397=(_tmpFD6.f1=Cyc_Tcutil_compress(_tmp395),((_tmpFD6.f2=_tmp38D,_tmpFD6)));struct _tuple0 _tmp398=_tmp397;struct Cyc_Absyn_Tvar*_tmp39A;struct Cyc_Absyn_Tvar*_tmp39C;_LL257:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp399=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp398.f1;if(_tmp399->tag != 2)goto _LL259;else{_tmp39A=_tmp399->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp398.f2;if(_tmp39B->tag != 2)goto _LL259;else{_tmp39C=_tmp39B->f1;}};_LL258:
 return Cyc_Tcutil_unify(t,_tmp38D);_LL259:;_LL25A:
 return _tmp395 == _tmp38D;_LL256:;}_LL254: _tmp396=_tmp393;_LL255:
# 1118
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp396);_LL251:;};_LL24D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp38E=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp388;if(_tmp38E->tag != 1)goto _LL24F;else{_tmp38F=_tmp38E->f1;_tmp390=(void**)((void**)& _tmp38E->f2);_tmp391=_tmp38E->f4;}}_LL24E:
# 1121
 if(_tmp38F == 0  || ((struct Cyc_Absyn_Kind*)_tmp38F->v)->kind != Cyc_Absyn_EffKind){
const char*_tmpFD9;void*_tmpFD8;(_tmpFD8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpFD9="effect evar has wrong kind",_tag_dyneither(_tmpFD9,sizeof(char),27))),_tag_dyneither(_tmpFD8,sizeof(void*),0)));}
if(!may_constrain_evars)return 0;{
# 1126
void*_tmp3A0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp391);
# 1129
Cyc_Tcutil_occurs(_tmp3A0,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp391))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFEE;struct Cyc_List_List*_tmpFED;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpFEC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpFEB;struct Cyc_List_List*_tmpFEA;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFE9;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3A1=(_tmpFE9=_cycalloc(sizeof(*_tmpFE9)),((_tmpFE9[0]=((_tmpFEE.tag=24,((_tmpFEE.f1=((_tmpFEA=_cycalloc(sizeof(*_tmpFEA)),((_tmpFEA->hd=_tmp3A0,((_tmpFEA->tl=((_tmpFED=_cycalloc(sizeof(*_tmpFED)),((_tmpFED->hd=(void*)((_tmpFEB=_cycalloc(sizeof(*_tmpFEB)),((_tmpFEB[0]=((_tmpFEC.tag=25,((_tmpFEC.f1=t,_tmpFEC)))),_tmpFEB)))),((_tmpFED->tl=0,_tmpFED)))))),_tmpFEA)))))),_tmpFEE)))),_tmpFE9)));
*_tmp390=(void*)_tmp3A1;
return 1;};};_LL24F:;_LL250:
 return 0;_LL246:;};}
# 1140
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3A8=e;struct Cyc_Absyn_Tvar*_tmp3AA;struct Cyc_List_List*_tmp3AC;void*_tmp3AE;struct Cyc_Core_Opt*_tmp3B0;void**_tmp3B1;struct Cyc_Core_Opt*_tmp3B2;_LL25C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3A9=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3A8;if(_tmp3A9->tag != 2)goto _LL25E;else{_tmp3AA=_tmp3A9->f1;}}_LL25D:
 return Cyc_Absyn_tvar_cmp(v,_tmp3AA)== 0;_LL25E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3AB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3A8;if(_tmp3AB->tag != 24)goto _LL260;else{_tmp3AC=_tmp3AB->f1;}}_LL25F:
# 1145
 for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp3AC->hd))
return 1;}
return 0;_LL260: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3AD=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3A8;if(_tmp3AD->tag != 25)goto _LL262;else{_tmp3AE=(void*)_tmp3AD->f1;}}_LL261: {
# 1150
void*_tmp3B3=Cyc_Tcutil_rgns_of(_tmp3AE);void*_tmp3B4=_tmp3B3;void*_tmp3B6;void*_tmp3B7;_LL267: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3B5=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3B4;if(_tmp3B5->tag != 25)goto _LL269;else{_tmp3B6=(void*)_tmp3B5->f1;}}_LL268:
# 1152
 if(!may_constrain_evars)return 0;{
void*_tmp3B8=Cyc_Tcutil_compress(_tmp3B6);void*_tmp3B9=_tmp3B8;struct Cyc_Core_Opt*_tmp3BB;void**_tmp3BC;struct Cyc_Core_Opt*_tmp3BD;_LL26C: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3BA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3B9;if(_tmp3BA->tag != 1)goto _LL26E;else{_tmp3BB=_tmp3BA->f1;_tmp3BC=(void**)((void**)& _tmp3BA->f2);_tmp3BD=_tmp3BA->f4;}}_LL26D: {
# 1158
void*_tmp3BE=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3BD);
# 1160
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BD))->v,v))return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpFFD;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpFFC;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpFFB;void*_tmpFFA[2];struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpFF9;void*_tmp3BF=Cyc_Tcutil_dummy_fntype((void*)((_tmpFF9=_cycalloc(sizeof(*_tmpFF9)),((_tmpFF9[0]=((_tmpFFD.tag=24,((_tmpFFD.f1=((_tmpFFA[1]=(void*)((_tmpFFC=_cycalloc(sizeof(*_tmpFFC)),((_tmpFFC[0]=((_tmpFFB.tag=2,((_tmpFFB.f1=v,_tmpFFB)))),_tmpFFC)))),((_tmpFFA[0]=_tmp3BE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFFA,sizeof(void*),2)))))),_tmpFFD)))),_tmpFF9)))));
*_tmp3BC=_tmp3BF;
return 1;};}_LL26E:;_LL26F:
 return 0;_LL26B:;};_LL269: _tmp3B7=_tmp3B4;_LL26A:
# 1166
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp3B7);_LL266:;}_LL262: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3AF=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3A8;if(_tmp3AF->tag != 1)goto _LL264;else{_tmp3B0=_tmp3AF->f1;_tmp3B1=(void**)((void**)& _tmp3AF->f2);_tmp3B2=_tmp3AF->f4;}}_LL263:
# 1169
 if(_tmp3B0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp3B0->v)->kind != Cyc_Absyn_EffKind){
const char*_tmp1000;void*_tmpFFF;(_tmpFFF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1000="effect evar has wrong kind",_tag_dyneither(_tmp1000,sizeof(char),27))),_tag_dyneither(_tmpFFF,sizeof(void*),0)));}{
# 1173
void*_tmp3C7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp3B2);
# 1175
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B2))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp1015;struct Cyc_List_List*_tmp1014;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1013;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1012;struct Cyc_List_List*_tmp1011;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1010;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3C8=(_tmp1010=_cycalloc(sizeof(*_tmp1010)),((_tmp1010[0]=((_tmp1015.tag=24,((_tmp1015.f1=((_tmp1011=_cycalloc(sizeof(*_tmp1011)),((_tmp1011->hd=_tmp3C7,((_tmp1011->tl=((_tmp1014=_cycalloc(sizeof(*_tmp1014)),((_tmp1014->hd=(void*)((_tmp1012=_cycalloc(sizeof(*_tmp1012)),((_tmp1012[0]=((_tmp1013.tag=2,((_tmp1013.f1=v,_tmp1013)))),_tmp1012)))),((_tmp1014->tl=0,_tmp1014)))))),_tmp1011)))))),_tmp1015)))),_tmp1010)));
*_tmp3B1=(void*)_tmp3C8;
return 1;};};_LL264:;_LL265:
 return 0;_LL25B:;};}
# 1185
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp3CF=e;struct Cyc_List_List*_tmp3D1;void*_tmp3D3;_LL271: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3CF;if(_tmp3D0->tag != 24)goto _LL273;else{_tmp3D1=_tmp3D0->f1;}}_LL272:
# 1189
 for(0;_tmp3D1 != 0;_tmp3D1=_tmp3D1->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp3D1->hd))
return 1;}
return 0;_LL273: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3CF;if(_tmp3D2->tag != 25)goto _LL275;else{_tmp3D3=(void*)_tmp3D2->f1;}}_LL274: {
# 1194
void*_tmp3D5=Cyc_Tcutil_rgns_of(_tmp3D3);void*_tmp3D6=_tmp3D5;void*_tmp3D8;void*_tmp3D9;_LL27A: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3D7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3D6;if(_tmp3D7->tag != 25)goto _LL27C;else{_tmp3D8=(void*)_tmp3D7->f1;}}_LL27B:
 return 0;_LL27C: _tmp3D9=_tmp3D6;_LL27D:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp3D9);_LL279:;}_LL275: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3D4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3CF;if(_tmp3D4->tag != 1)goto _LL277;}_LL276:
# 1198
 return evar == e;_LL277:;_LL278:
 return 0;_LL270:;};}
# 1212 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1217
void*_tmp3DA=Cyc_Tcutil_compress(e1);void*_tmp3DB=_tmp3DA;struct Cyc_List_List*_tmp3DD;void*_tmp3DF;struct Cyc_Absyn_Tvar*_tmp3E1;void*_tmp3E3;void**_tmp3E5;struct Cyc_Core_Opt*_tmp3E6;_LL27F: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp3DC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3DB;if(_tmp3DC->tag != 24)goto _LL281;else{_tmp3DD=_tmp3DC->f1;}}_LL280:
# 1219
 for(0;_tmp3DD != 0;_tmp3DD=_tmp3DD->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp3DD->hd,e2))
return 0;}
return 1;_LL281: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp3DE=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp3DB;if(_tmp3DE->tag != 23)goto _LL283;else{_tmp3DF=(void*)_tmp3DE->f1;}}_LL282:
# 1230
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp3DF,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3DF,(void*)& Cyc_Absyn_HeapRgn_val);_LL283: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3E0=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3DB;if(_tmp3E0->tag != 2)goto _LL285;else{_tmp3E1=_tmp3E0->f1;}}_LL284:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp3E1,e2);_LL285: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E2=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3DB;if(_tmp3E2->tag != 25)goto _LL287;else{_tmp3E3=(void*)_tmp3E2->f1;}}_LL286: {
# 1234
void*_tmp3E7=Cyc_Tcutil_rgns_of(_tmp3E3);void*_tmp3E8=_tmp3E7;void*_tmp3EA;void*_tmp3EB;_LL28C: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp3E9=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp3E8;if(_tmp3E9->tag != 25)goto _LL28E;else{_tmp3EA=(void*)_tmp3E9->f1;}}_LL28D:
# 1239
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp3EA,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp3EA,Cyc_Absyn_sint_typ);_LL28E: _tmp3EB=_tmp3E8;_LL28F:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp3EB,e2);_LL28B:;}_LL287: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3E4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3DB;if(_tmp3E4->tag != 1)goto _LL289;else{_tmp3E5=(void**)((void**)& _tmp3E4->f2);_tmp3E6=_tmp3E4->f4;}}_LL288:
# 1244
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1248
*_tmp3E5=Cyc_Absyn_empty_effect;
# 1251
return 1;}else{
# 1253
return 0;}_LL289:;_LL28A: {
const char*_tmp1019;void*_tmp1018[1];struct Cyc_String_pa_PrintArg_struct _tmp1017;(_tmp1017.tag=0,((_tmp1017.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1)),((_tmp1018[0]=& _tmp1017,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1019="subset_effect: bad effect: %s",_tag_dyneither(_tmp1019,sizeof(char),30))),_tag_dyneither(_tmp1018,sizeof(void*),1)))))));}_LL27E:;}
# 1269 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1285
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1287
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp3EF=(struct _tuple0*)r1->hd;void*_tmp3F1;void*_tmp3F2;struct _tuple0*_tmp3F0=_tmp3EF;_tmp3F1=_tmp3F0->f1;_tmp3F2=_tmp3F0->f2;{
int found=_tmp3F1 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp3F3=(struct _tuple0*)r2->hd;void*_tmp3F5;void*_tmp3F6;struct _tuple0*_tmp3F4=_tmp3F3;_tmp3F5=_tmp3F4->f1;_tmp3F6=_tmp3F4->f2;
if(Cyc_Tcutil_unify(_tmp3F1,_tmp3F5) && Cyc_Tcutil_unify(_tmp3F2,_tmp3F6)){
found=1;
break;}}}
# 1297
if(!found)return 0;};}}
# 1299
return 1;}
# 1303
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1305
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1309
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp3F7=t1;struct Cyc_Core_Opt*_tmp3F9;void**_tmp3FA;struct Cyc_Core_Opt*_tmp3FB;_LL291: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3F8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3F7;if(_tmp3F8->tag != 1)goto _LL293;else{_tmp3F9=_tmp3F8->f1;_tmp3FA=(void**)((void**)& _tmp3F8->f2);_tmp3FB=_tmp3F8->f4;}}_LL292:
# 1320
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3FB))->v,t2);{
struct Cyc_Absyn_Kind*_tmp3FC=Cyc_Tcutil_typ_kind(t2);
# 1325
if(Cyc_Tcutil_kind_leq(_tmp3FC,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp3F9))->v)){
*_tmp3FA=t2;
return;}else{
# 1329
{void*_tmp3FD=t2;void**_tmp3FF;struct Cyc_Core_Opt*_tmp400;struct Cyc_Absyn_PtrInfo _tmp402;_LL296: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp3FE=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp3FD;if(_tmp3FE->tag != 1)goto _LL298;else{_tmp3FF=(void**)((void**)& _tmp3FE->f2);_tmp400=_tmp3FE->f4;}}_LL297: {
# 1332
struct Cyc_List_List*_tmp403=(struct Cyc_List_List*)_tmp3FB->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp400))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp403,(struct Cyc_Absyn_Tvar*)s2->hd)){
{const char*_tmp101A;Cyc_Tcutil_failure_reason=((_tmp101A="(type variable would escape scope)",_tag_dyneither(_tmp101A,sizeof(char),35)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1339
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp3F9->v,_tmp3FC)){
*_tmp3FF=t1;return;}
# 1342
{const char*_tmp101B;Cyc_Tcutil_failure_reason=((_tmp101B="(kinds are incompatible)",_tag_dyneither(_tmp101B,sizeof(char),25)));}
goto _LL295;}_LL298:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp401=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FD;if(_tmp401->tag != 5)goto _LL29A;else{_tmp402=_tmp401->f1;}}if(!(((struct Cyc_Absyn_Kind*)_tmp3F9->v)->kind == Cyc_Absyn_BoxKind))goto _LL29A;_LL299: {
# 1348
union Cyc_Absyn_Constraint*_tmp406=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp402.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp407=_tmp406;_LL29D: if((_tmp407->No_constr).tag != 3)goto _LL29F;_LL29E:
# 1352
{struct _union_Constraint_Eq_constr*_tmp101C;(_tmp101C=& _tmp406->Eq_constr,((_tmp101C->tag=1,_tmp101C->val=Cyc_Absyn_bounds_one)));}
*_tmp3FA=t2;
return;_LL29F:;_LL2A0:
 goto _LL29C;_LL29C:;}
# 1357
goto _LL295;}_LL29A:;_LL29B:
 goto _LL295;_LL295:;}
# 1360
{const char*_tmp101D;Cyc_Tcutil_failure_reason=((_tmp101D="(kinds are incompatible)",_tag_dyneither(_tmp101D,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};_LL293:;_LL294:
# 1363
 goto _LL290;_LL290:;}
# 1368
{struct _tuple0 _tmp101E;struct _tuple0 _tmp40A=(_tmp101E.f1=t2,((_tmp101E.f2=t1,_tmp101E)));struct _tuple0 _tmp40B=_tmp40A;struct Cyc_Absyn_Tvar*_tmp410;struct Cyc_Absyn_Tvar*_tmp412;union Cyc_Absyn_AggrInfoU _tmp414;struct Cyc_List_List*_tmp415;union Cyc_Absyn_AggrInfoU _tmp417;struct Cyc_List_List*_tmp418;struct _tuple2*_tmp41A;struct _tuple2*_tmp41C;struct Cyc_List_List*_tmp41E;struct Cyc_List_List*_tmp420;struct Cyc_Absyn_Datatypedecl*_tmp422;struct Cyc_List_List*_tmp423;struct Cyc_Absyn_Datatypedecl*_tmp425;struct Cyc_List_List*_tmp426;struct Cyc_Absyn_Datatypedecl*_tmp428;struct Cyc_Absyn_Datatypefield*_tmp429;struct Cyc_List_List*_tmp42A;struct Cyc_Absyn_Datatypedecl*_tmp42C;struct Cyc_Absyn_Datatypefield*_tmp42D;struct Cyc_List_List*_tmp42E;void*_tmp430;struct Cyc_Absyn_Tqual _tmp431;void*_tmp432;union Cyc_Absyn_Constraint*_tmp433;union Cyc_Absyn_Constraint*_tmp434;union Cyc_Absyn_Constraint*_tmp435;void*_tmp437;struct Cyc_Absyn_Tqual _tmp438;void*_tmp439;union Cyc_Absyn_Constraint*_tmp43A;union Cyc_Absyn_Constraint*_tmp43B;union Cyc_Absyn_Constraint*_tmp43C;enum Cyc_Absyn_Sign _tmp43E;enum Cyc_Absyn_Size_of _tmp43F;enum Cyc_Absyn_Sign _tmp441;enum Cyc_Absyn_Size_of _tmp442;int _tmp444;int _tmp446;void*_tmp448;void*_tmp44A;struct Cyc_Absyn_Exp*_tmp44C;struct Cyc_Absyn_Exp*_tmp44E;void*_tmp450;struct Cyc_Absyn_Tqual _tmp451;struct Cyc_Absyn_Exp*_tmp452;union Cyc_Absyn_Constraint*_tmp453;void*_tmp455;struct Cyc_Absyn_Tqual _tmp456;struct Cyc_Absyn_Exp*_tmp457;union Cyc_Absyn_Constraint*_tmp458;struct Cyc_List_List*_tmp45A;void*_tmp45B;struct Cyc_Absyn_Tqual _tmp45C;void*_tmp45D;struct Cyc_List_List*_tmp45E;int _tmp45F;struct Cyc_Absyn_VarargInfo*_tmp460;struct Cyc_List_List*_tmp461;struct Cyc_List_List*_tmp462;struct Cyc_List_List*_tmp464;void*_tmp465;struct Cyc_Absyn_Tqual _tmp466;void*_tmp467;struct Cyc_List_List*_tmp468;int _tmp469;struct Cyc_Absyn_VarargInfo*_tmp46A;struct Cyc_List_List*_tmp46B;struct Cyc_List_List*_tmp46C;struct Cyc_List_List*_tmp46E;struct Cyc_List_List*_tmp470;enum Cyc_Absyn_AggrKind _tmp472;struct Cyc_List_List*_tmp473;enum Cyc_Absyn_AggrKind _tmp475;struct Cyc_List_List*_tmp476;struct Cyc_List_List*_tmp478;struct Cyc_Absyn_Typedefdecl*_tmp479;struct Cyc_List_List*_tmp47B;struct Cyc_Absyn_Typedefdecl*_tmp47C;void*_tmp484;void*_tmp486;void*_tmp488;void*_tmp489;void*_tmp48B;void*_tmp48C;_LL2A2: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp40B.f1;if(_tmp40C->tag != 1)goto _LL2A4;}_LL2A3:
# 1371
 Cyc_Tcutil_unify_it(t2,t1);
return;_LL2A4:{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp40D=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp40D->tag != 0)goto _LL2A6;}{struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp40E=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp40E->tag != 0)goto _LL2A6;};_LL2A5:
# 1374
 return;_LL2A6:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp40F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp40F->tag != 2)goto _LL2A8;else{_tmp410=_tmp40F->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp411=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp411->tag != 2)goto _LL2A8;else{_tmp412=_tmp411->f1;}};_LL2A7: {
# 1377
struct _dyneither_ptr*_tmp493=_tmp410->name;
struct _dyneither_ptr*_tmp494=_tmp412->name;
# 1380
int _tmp495=_tmp410->identity;
int _tmp496=_tmp412->identity;
if(_tmp496 == _tmp495)return;
{const char*_tmp101F;Cyc_Tcutil_failure_reason=((_tmp101F="(variable types are not the same)",_tag_dyneither(_tmp101F,sizeof(char),34)));}
goto _LL2A1;}_LL2A8:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp413=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp413->tag != 11)goto _LL2AA;else{_tmp414=(_tmp413->f1).aggr_info;_tmp415=(_tmp413->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp416=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp416->tag != 11)goto _LL2AA;else{_tmp417=(_tmp416->f1).aggr_info;_tmp418=(_tmp416->f1).targs;}};_LL2A9: {
# 1387
struct _tuple13 _tmp498=Cyc_Absyn_aggr_kinded_name(_tmp417);enum Cyc_Absyn_AggrKind _tmp49A;struct _tuple2*_tmp49B;struct _tuple13 _tmp499=_tmp498;_tmp49A=_tmp499.f1;_tmp49B=_tmp499.f2;{
struct _tuple13 _tmp49C=Cyc_Absyn_aggr_kinded_name(_tmp414);enum Cyc_Absyn_AggrKind _tmp49E;struct _tuple2*_tmp49F;struct _tuple13 _tmp49D=_tmp49C;_tmp49E=_tmp49D.f1;_tmp49F=_tmp49D.f2;
if(_tmp49A != _tmp49E){{const char*_tmp1020;Cyc_Tcutil_failure_reason=((_tmp1020="(struct and union type)",_tag_dyneither(_tmp1020,sizeof(char),24)));}goto _LL2A1;}
if(Cyc_Absyn_qvar_cmp(_tmp49B,_tmp49F)!= 0){{const char*_tmp1021;Cyc_Tcutil_failure_reason=((_tmp1021="(different type name)",_tag_dyneither(_tmp1021,sizeof(char),22)));}goto _LL2A1;}
Cyc_Tcutil_unify_list(_tmp418,_tmp415);
return;};}_LL2AA:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp419=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp419->tag != 13)goto _LL2AC;else{_tmp41A=_tmp419->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp41B=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp41B->tag != 13)goto _LL2AC;else{_tmp41C=_tmp41B->f1;}};_LL2AB:
# 1396
 if(Cyc_Absyn_qvar_cmp(_tmp41A,_tmp41C)== 0)return;
{const char*_tmp1022;Cyc_Tcutil_failure_reason=((_tmp1022="(different enum types)",_tag_dyneither(_tmp1022,sizeof(char),23)));}
goto _LL2A1;_LL2AC:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp41D=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp41D->tag != 14)goto _LL2AE;else{_tmp41E=_tmp41D->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp41F=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp41F->tag != 14)goto _LL2AE;else{_tmp420=_tmp41F->f1;}};_LL2AD: {
# 1401
int bad=0;
for(0;_tmp41E != 0  && _tmp420 != 0;(_tmp41E=_tmp41E->tl,_tmp420=_tmp420->tl)){
struct Cyc_Absyn_Enumfield*_tmp4A3=(struct Cyc_Absyn_Enumfield*)_tmp41E->hd;
struct Cyc_Absyn_Enumfield*_tmp4A4=(struct Cyc_Absyn_Enumfield*)_tmp420->hd;
if(Cyc_Absyn_qvar_cmp(_tmp4A3->name,_tmp4A4->name)!= 0){
{const char*_tmp1023;Cyc_Tcutil_failure_reason=((_tmp1023="(different names for enum fields)",_tag_dyneither(_tmp1023,sizeof(char),34)));}
bad=1;
break;}
# 1410
if(_tmp4A3->tag == _tmp4A4->tag)continue;
if(_tmp4A3->tag == 0  || _tmp4A4->tag == 0){
{const char*_tmp1024;Cyc_Tcutil_failure_reason=((_tmp1024="(different tag values for enum fields)",_tag_dyneither(_tmp1024,sizeof(char),39)));}
bad=1;
break;}
# 1416
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4A3->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp4A4->tag))){
{const char*_tmp1025;Cyc_Tcutil_failure_reason=((_tmp1025="(different tag values for enum fields)",_tag_dyneither(_tmp1025,sizeof(char),39)));}
bad=1;
break;}}
# 1422
if(bad)goto _LL2A1;
if(_tmp41E == 0  && _tmp420 == 0)return;
{const char*_tmp1026;Cyc_Tcutil_failure_reason=((_tmp1026="(different number of fields for enums)",_tag_dyneither(_tmp1026,sizeof(char),39)));}
goto _LL2A1;}_LL2AE:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp421=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp421->tag != 3)goto _LL2B0;else{if((((_tmp421->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp422=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp421->f1).datatype_info).KnownDatatype).val);_tmp423=(_tmp421->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp424=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp424->tag != 3)goto _LL2B0;else{if((((_tmp424->f1).datatype_info).KnownDatatype).tag != 2)goto _LL2B0;_tmp425=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp424->f1).datatype_info).KnownDatatype).val);_tmp426=(_tmp424->f1).targs;}};_LL2AF:
# 1429
 if(_tmp422 == _tmp425  || Cyc_Absyn_qvar_cmp(_tmp422->name,_tmp425->name)== 0){
Cyc_Tcutil_unify_list(_tmp426,_tmp423);
return;}
# 1433
goto _LL2A1;_LL2B0:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp427=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp427->tag != 4)goto _LL2B2;else{if((((_tmp427->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp428=((struct _tuple3)(((_tmp427->f1).field_info).KnownDatatypefield).val).f1;_tmp429=((struct _tuple3)(((_tmp427->f1).field_info).KnownDatatypefield).val).f2;_tmp42A=(_tmp427->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp42B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp42B->tag != 4)goto _LL2B2;else{if((((_tmp42B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2B2;_tmp42C=((struct _tuple3)(((_tmp42B->f1).field_info).KnownDatatypefield).val).f1;_tmp42D=((struct _tuple3)(((_tmp42B->f1).field_info).KnownDatatypefield).val).f2;_tmp42E=(_tmp42B->f1).targs;}};_LL2B1:
# 1437
 if((_tmp428 == _tmp42C  || Cyc_Absyn_qvar_cmp(_tmp428->name,_tmp42C->name)== 0) && (
_tmp429 == _tmp42D  || Cyc_Absyn_qvar_cmp(_tmp429->name,_tmp42D->name)== 0)){
Cyc_Tcutil_unify_list(_tmp42E,_tmp42A);
return;}
# 1442
{const char*_tmp1027;Cyc_Tcutil_failure_reason=((_tmp1027="(different datatype field types)",_tag_dyneither(_tmp1027,sizeof(char),33)));}
goto _LL2A1;_LL2B2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp42F->tag != 5)goto _LL2B4;else{_tmp430=(_tmp42F->f1).elt_typ;_tmp431=(_tmp42F->f1).elt_tq;_tmp432=((_tmp42F->f1).ptr_atts).rgn;_tmp433=((_tmp42F->f1).ptr_atts).nullable;_tmp434=((_tmp42F->f1).ptr_atts).bounds;_tmp435=((_tmp42F->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp436=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp436->tag != 5)goto _LL2B4;else{_tmp437=(_tmp436->f1).elt_typ;_tmp438=(_tmp436->f1).elt_tq;_tmp439=((_tmp436->f1).ptr_atts).rgn;_tmp43A=((_tmp436->f1).ptr_atts).nullable;_tmp43B=((_tmp436->f1).ptr_atts).bounds;_tmp43C=((_tmp436->f1).ptr_atts).zero_term;}};_LL2B3:
# 1447
 Cyc_Tcutil_unify_it(_tmp437,_tmp430);
Cyc_Tcutil_unify_it(_tmp432,_tmp439);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1028;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp43C,_tmp435,((_tmp1028="(not both zero terminated)",_tag_dyneither(_tmp1028,sizeof(char),27))));}
Cyc_Tcutil_unify_tqual(_tmp438,_tmp437,_tmp431,_tmp430);
{const char*_tmp1029;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp43B,_tmp434,((_tmp1029="(different pointer bounds)",_tag_dyneither(_tmp1029,sizeof(char),27))));}
# 1455
{void*_tmp4AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp43B);void*_tmp4AD=_tmp4AC;_LL2DF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp4AE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp4AD;if(_tmp4AE->tag != 0)goto _LL2E1;}_LL2E0:
 return;_LL2E1:;_LL2E2:
 goto _LL2DE;_LL2DE:;}
# 1459
{const char*_tmp102A;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp43A,_tmp433,((_tmp102A="(incompatible pointer types)",_tag_dyneither(_tmp102A,sizeof(char),29))));}
return;_LL2B4:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp43D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp43D->tag != 6)goto _LL2B6;else{_tmp43E=_tmp43D->f1;_tmp43F=_tmp43D->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp440=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp440->tag != 6)goto _LL2B6;else{_tmp441=_tmp440->f1;_tmp442=_tmp440->f2;}};_LL2B5:
# 1464
 if(_tmp441 == _tmp43E  && ((_tmp442 == _tmp43F  || _tmp442 == Cyc_Absyn_Int_sz  && _tmp43F == Cyc_Absyn_Long_sz) || 
_tmp442 == Cyc_Absyn_Long_sz  && _tmp43F == Cyc_Absyn_Int_sz))return;
{const char*_tmp102B;Cyc_Tcutil_failure_reason=((_tmp102B="(different integral types)",_tag_dyneither(_tmp102B,sizeof(char),27)));}
goto _LL2A1;_LL2B6:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp443=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp443->tag != 7)goto _LL2B8;else{_tmp444=_tmp443->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp445=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp445->tag != 7)goto _LL2B8;else{_tmp446=_tmp445->f1;}};_LL2B7:
# 1470
 if(_tmp446 == 0  && _tmp444 == 0)return;else{
if(_tmp446 == 1  && _tmp444 == 1)return;else{
# 1473
if(((_tmp446 != 0  && _tmp446 != 1) && _tmp444 != 0) && _tmp444 != 1)return;}}
goto _LL2A1;_LL2B8:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp447->tag != 19)goto _LL2BA;else{_tmp448=(void*)_tmp447->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp449=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp449->tag != 19)goto _LL2BA;else{_tmp44A=(void*)_tmp449->f1;}};_LL2B9:
# 1476
 Cyc_Tcutil_unify_it(_tmp448,_tmp44A);return;_LL2BA:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp44B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp44B->tag != 18)goto _LL2BC;else{_tmp44C=_tmp44B->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp44D->tag != 18)goto _LL2BC;else{_tmp44E=_tmp44D->f1;}};_LL2BB:
# 1479
 if(!Cyc_Evexp_same_const_exp(_tmp44C,_tmp44E)){
{const char*_tmp102C;Cyc_Tcutil_failure_reason=((_tmp102C="(cannot prove expressions are the same)",_tag_dyneither(_tmp102C,sizeof(char),40)));}
goto _LL2A1;}
# 1483
return;_LL2BC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp44F->tag != 8)goto _LL2BE;else{_tmp450=(_tmp44F->f1).elt_type;_tmp451=(_tmp44F->f1).tq;_tmp452=(_tmp44F->f1).num_elts;_tmp453=(_tmp44F->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp454->tag != 8)goto _LL2BE;else{_tmp455=(_tmp454->f1).elt_type;_tmp456=(_tmp454->f1).tq;_tmp457=(_tmp454->f1).num_elts;_tmp458=(_tmp454->f1).zero_term;}};_LL2BD:
# 1487
 Cyc_Tcutil_unify_it(_tmp455,_tmp450);
Cyc_Tcutil_unify_tqual(_tmp456,_tmp455,_tmp451,_tmp450);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp102D;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp453,_tmp458,((_tmp102D="(not both zero terminated)",_tag_dyneither(_tmp102D,sizeof(char),27))));}
if(_tmp452 == _tmp457)return;
if(_tmp452 == 0  || _tmp457 == 0)goto _LL2A1;
if(Cyc_Evexp_same_const_exp(_tmp452,_tmp457))
return;
{const char*_tmp102E;Cyc_Tcutil_failure_reason=((_tmp102E="(different array sizes)",_tag_dyneither(_tmp102E,sizeof(char),24)));}
goto _LL2A1;_LL2BE:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp459->tag != 9)goto _LL2C0;else{_tmp45A=(_tmp459->f1).tvars;_tmp45B=(_tmp459->f1).effect;_tmp45C=(_tmp459->f1).ret_tqual;_tmp45D=(_tmp459->f1).ret_typ;_tmp45E=(_tmp459->f1).args;_tmp45F=(_tmp459->f1).c_varargs;_tmp460=(_tmp459->f1).cyc_varargs;_tmp461=(_tmp459->f1).rgn_po;_tmp462=(_tmp459->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp463=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp463->tag != 9)goto _LL2C0;else{_tmp464=(_tmp463->f1).tvars;_tmp465=(_tmp463->f1).effect;_tmp466=(_tmp463->f1).ret_tqual;_tmp467=(_tmp463->f1).ret_typ;_tmp468=(_tmp463->f1).args;_tmp469=(_tmp463->f1).c_varargs;_tmp46A=(_tmp463->f1).cyc_varargs;_tmp46B=(_tmp463->f1).rgn_po;_tmp46C=(_tmp463->f1).attributes;}};_LL2BF: {
# 1501
int done=0;
struct _RegionHandle _tmp4B4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4B4;_push_region(rgn);{
struct Cyc_List_List*inst=0;
while(_tmp464 != 0){
if(_tmp45A == 0){
{const char*_tmp102F;Cyc_Tcutil_failure_reason=((_tmp102F="(second function type has too few type variables)",_tag_dyneither(_tmp102F,sizeof(char),50)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1509
void*_tmp4B6=((struct Cyc_Absyn_Tvar*)_tmp464->hd)->kind;
void*_tmp4B7=((struct Cyc_Absyn_Tvar*)_tmp45A->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp4B6,_tmp4B7)){
{const char*_tmp1035;void*_tmp1034[3];struct Cyc_String_pa_PrintArg_struct _tmp1033;struct Cyc_String_pa_PrintArg_struct _tmp1032;struct Cyc_String_pa_PrintArg_struct _tmp1031;Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)((_tmp1031.tag=0,((_tmp1031.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1515
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp45A->hd,& Cyc_Tcutil_bk))),((_tmp1032.tag=0,((_tmp1032.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1514
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp464->hd,& Cyc_Tcutil_bk))),((_tmp1033.tag=0,((_tmp1033.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1513
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp464->hd)),((_tmp1034[0]=& _tmp1033,((_tmp1034[1]=& _tmp1032,((_tmp1034[2]=& _tmp1031,Cyc_aprintf(((_tmp1035="(type var %s has different kinds %s and %s)",_tag_dyneither(_tmp1035,sizeof(char),44))),_tag_dyneither(_tmp1034,sizeof(void*),3))))))))))))))))))));}
# 1516
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1518
{struct _tuple16*_tmp103F;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp103E;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp103D;struct Cyc_List_List*_tmp103C;inst=((_tmp103C=_region_malloc(rgn,sizeof(*_tmp103C)),((_tmp103C->hd=((_tmp103F=_region_malloc(rgn,sizeof(*_tmp103F)),((_tmp103F->f1=(struct Cyc_Absyn_Tvar*)_tmp45A->hd,((_tmp103F->f2=(void*)((_tmp103D=_cycalloc(sizeof(*_tmp103D)),((_tmp103D[0]=((_tmp103E.tag=2,((_tmp103E.f1=(struct Cyc_Absyn_Tvar*)_tmp464->hd,_tmp103E)))),_tmp103D)))),_tmp103F)))))),((_tmp103C->tl=inst,_tmp103C))))));}
_tmp464=_tmp464->tl;
_tmp45A=_tmp45A->tl;};}
# 1522
if(_tmp45A != 0){
{const char*_tmp1040;Cyc_Tcutil_failure_reason=((_tmp1040="(second function type has too many type variables)",_tag_dyneither(_tmp1040,sizeof(char),51)));}
_npop_handler(0);goto _LL2A1;}
# 1526
if(inst != 0){
{struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp104C;struct Cyc_Absyn_FnInfo _tmp104B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp104A;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1046;struct Cyc_Absyn_FnInfo _tmp1045;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1044;Cyc_Tcutil_unify_it((void*)((_tmp1044=_cycalloc(sizeof(*_tmp1044)),((_tmp1044[0]=((_tmp1046.tag=9,((_tmp1046.f1=((_tmp1045.tvars=0,((_tmp1045.effect=_tmp465,((_tmp1045.ret_tqual=_tmp466,((_tmp1045.ret_typ=_tmp467,((_tmp1045.args=_tmp468,((_tmp1045.c_varargs=_tmp469,((_tmp1045.cyc_varargs=_tmp46A,((_tmp1045.rgn_po=_tmp46B,((_tmp1045.attributes=_tmp46C,_tmp1045)))))))))))))))))),_tmp1046)))),_tmp1044)))),
# 1529
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)(
(_tmp104A=_cycalloc(sizeof(*_tmp104A)),((_tmp104A[0]=((_tmp104C.tag=9,((_tmp104C.f1=((_tmp104B.tvars=0,((_tmp104B.effect=_tmp45B,((_tmp104B.ret_tqual=_tmp45C,((_tmp104B.ret_typ=_tmp45D,((_tmp104B.args=_tmp45E,((_tmp104B.c_varargs=_tmp45F,((_tmp104B.cyc_varargs=_tmp460,((_tmp104B.rgn_po=_tmp461,((_tmp104B.attributes=_tmp462,_tmp104B)))))))))))))))))),_tmp104C)))),_tmp104A))))));}
# 1532
done=1;}}
# 1535
if(done){
_npop_handler(0);return;}
Cyc_Tcutil_unify_it(_tmp467,_tmp45D);
Cyc_Tcutil_unify_tqual(_tmp466,_tmp467,_tmp45C,_tmp45D);
for(0;_tmp468 != 0  && _tmp45E != 0;(_tmp468=_tmp468->tl,_tmp45E=_tmp45E->tl)){
struct _tuple10 _tmp4C8=*((struct _tuple10*)_tmp468->hd);struct Cyc_Absyn_Tqual _tmp4CA;void*_tmp4CB;struct _tuple10 _tmp4C9=_tmp4C8;_tmp4CA=_tmp4C9.f2;_tmp4CB=_tmp4C9.f3;{
struct _tuple10 _tmp4CC=*((struct _tuple10*)_tmp45E->hd);struct Cyc_Absyn_Tqual _tmp4CE;void*_tmp4CF;struct _tuple10 _tmp4CD=_tmp4CC;_tmp4CE=_tmp4CD.f2;_tmp4CF=_tmp4CD.f3;
Cyc_Tcutil_unify_it(_tmp4CB,_tmp4CF);
Cyc_Tcutil_unify_tqual(_tmp4CA,_tmp4CB,_tmp4CE,_tmp4CF);};}
# 1545
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp468 != 0  || _tmp45E != 0){
{const char*_tmp104D;Cyc_Tcutil_failure_reason=((_tmp104D="(function types have different number of arguments)",_tag_dyneither(_tmp104D,sizeof(char),52)));}
_npop_handler(0);goto _LL2A1;}
# 1551
if(_tmp469 != _tmp45F){
{const char*_tmp104E;Cyc_Tcutil_failure_reason=((_tmp104E="(only one function type takes C varargs)",_tag_dyneither(_tmp104E,sizeof(char),41)));}
_npop_handler(0);goto _LL2A1;}{
# 1556
int bad_cyc_vararg=0;
{struct _tuple22 _tmp104F;struct _tuple22 _tmp4D2=(_tmp104F.f1=_tmp46A,((_tmp104F.f2=_tmp460,_tmp104F)));struct _tuple22 _tmp4D3=_tmp4D2;struct _dyneither_ptr*_tmp4D4;struct Cyc_Absyn_Tqual _tmp4D5;void*_tmp4D6;int _tmp4D7;struct _dyneither_ptr*_tmp4D8;struct Cyc_Absyn_Tqual _tmp4D9;void*_tmp4DA;int _tmp4DB;_LL2E4: if(_tmp4D3.f1 != 0)goto _LL2E6;if(_tmp4D3.f2 != 0)goto _LL2E6;_LL2E5:
 goto _LL2E3;_LL2E6: if(_tmp4D3.f1 != 0)goto _LL2E8;_LL2E7:
 goto _LL2E9;_LL2E8: if(_tmp4D3.f2 != 0)goto _LL2EA;_LL2E9:
# 1561
 bad_cyc_vararg=1;
{const char*_tmp1050;Cyc_Tcutil_failure_reason=((_tmp1050="(only one function type takes varargs)",_tag_dyneither(_tmp1050,sizeof(char),39)));}
goto _LL2E3;_LL2EA: if(_tmp4D3.f1 == 0)goto _LL2E3;_tmp4D4=(_tmp4D3.f1)->name;_tmp4D5=(_tmp4D3.f1)->tq;_tmp4D6=(_tmp4D3.f1)->type;_tmp4D7=(_tmp4D3.f1)->inject;if(_tmp4D3.f2 == 0)goto _LL2E3;_tmp4D8=(_tmp4D3.f2)->name;_tmp4D9=(_tmp4D3.f2)->tq;_tmp4DA=(_tmp4D3.f2)->type;_tmp4DB=(_tmp4D3.f2)->inject;_LL2EB:
# 1565
 Cyc_Tcutil_unify_it(_tmp4D6,_tmp4DA);
Cyc_Tcutil_unify_tqual(_tmp4D5,_tmp4D6,_tmp4D9,_tmp4DA);
if(_tmp4D7 != _tmp4DB){
bad_cyc_vararg=1;{
const char*_tmp1051;Cyc_Tcutil_failure_reason=((_tmp1051="(only one function type injects varargs)",_tag_dyneither(_tmp1051,sizeof(char),41)));};}
# 1571
goto _LL2E3;_LL2E3:;}
# 1573
if(bad_cyc_vararg){_npop_handler(0);goto _LL2A1;}{
# 1576
int bad_effect=0;
{struct _tuple0 _tmp1052;struct _tuple0 _tmp4DF=(_tmp1052.f1=_tmp465,((_tmp1052.f2=_tmp45B,_tmp1052)));struct _tuple0 _tmp4E0=_tmp4DF;_LL2ED: if(_tmp4E0.f1 != 0)goto _LL2EF;if(_tmp4E0.f2 != 0)goto _LL2EF;_LL2EE:
 goto _LL2EC;_LL2EF: if(_tmp4E0.f1 != 0)goto _LL2F1;_LL2F0:
 goto _LL2F2;_LL2F1: if(_tmp4E0.f2 != 0)goto _LL2F3;_LL2F2:
 bad_effect=1;goto _LL2EC;_LL2F3:;_LL2F4:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp465),(void*)_check_null(_tmp45B));goto _LL2EC;_LL2EC:;}
# 1583
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
{const char*_tmp1053;Cyc_Tcutil_failure_reason=((_tmp1053="(function effects do not match)",_tag_dyneither(_tmp1053,sizeof(char),32)));}
_npop_handler(0);goto _LL2A1;}
# 1589
if(!Cyc_Tcutil_same_atts(_tmp462,_tmp46C)){
{const char*_tmp1054;Cyc_Tcutil_failure_reason=((_tmp1054="(function types have different attributes)",_tag_dyneither(_tmp1054,sizeof(char),43)));}
_npop_handler(0);goto _LL2A1;}
# 1593
if(!Cyc_Tcutil_same_rgn_po(_tmp461,_tmp46B)){
{const char*_tmp1055;Cyc_Tcutil_failure_reason=((_tmp1055="(function types have different region lifetime orderings)",_tag_dyneither(_tmp1055,sizeof(char),58)));}
_npop_handler(0);goto _LL2A1;}
# 1597
_npop_handler(0);return;};};
# 1502
;_pop_region(rgn);}_LL2C0:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp46D->tag != 10)goto _LL2C2;else{_tmp46E=_tmp46D->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp46F->tag != 10)goto _LL2C2;else{_tmp470=_tmp46F->f1;}};_LL2C1:
# 1600
 for(0;_tmp470 != 0  && _tmp46E != 0;(_tmp470=_tmp470->tl,_tmp46E=_tmp46E->tl)){
struct _tuple12 _tmp4E5=*((struct _tuple12*)_tmp470->hd);struct Cyc_Absyn_Tqual _tmp4E7;void*_tmp4E8;struct _tuple12 _tmp4E6=_tmp4E5;_tmp4E7=_tmp4E6.f1;_tmp4E8=_tmp4E6.f2;{
struct _tuple12 _tmp4E9=*((struct _tuple12*)_tmp46E->hd);struct Cyc_Absyn_Tqual _tmp4EB;void*_tmp4EC;struct _tuple12 _tmp4EA=_tmp4E9;_tmp4EB=_tmp4EA.f1;_tmp4EC=_tmp4EA.f2;
Cyc_Tcutil_unify_it(_tmp4E8,_tmp4EC);
Cyc_Tcutil_unify_tqual(_tmp4E7,_tmp4E8,_tmp4EB,_tmp4EC);};}
# 1606
if(_tmp470 == 0  && _tmp46E == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1056;Cyc_Tcutil_failure_reason=((_tmp1056="(tuple types have different numbers of components)",_tag_dyneither(_tmp1056,sizeof(char),51)));}
goto _LL2A1;_LL2C2:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp471=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp471->tag != 12)goto _LL2C4;else{_tmp472=_tmp471->f1;_tmp473=_tmp471->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp474=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp474->tag != 12)goto _LL2C4;else{_tmp475=_tmp474->f1;_tmp476=_tmp474->f2;}};_LL2C3:
# 1613
 if(_tmp475 != _tmp472){{const char*_tmp1057;Cyc_Tcutil_failure_reason=((_tmp1057="(struct and union type)",_tag_dyneither(_tmp1057,sizeof(char),24)));}goto _LL2A1;}
for(0;_tmp476 != 0  && _tmp473 != 0;(_tmp476=_tmp476->tl,_tmp473=_tmp473->tl)){
struct Cyc_Absyn_Aggrfield*_tmp4EF=(struct Cyc_Absyn_Aggrfield*)_tmp476->hd;
struct Cyc_Absyn_Aggrfield*_tmp4F0=(struct Cyc_Absyn_Aggrfield*)_tmp473->hd;
if(Cyc_strptrcmp(_tmp4EF->name,_tmp4F0->name)!= 0){
{const char*_tmp1058;Cyc_Tcutil_failure_reason=((_tmp1058="(different member names)",_tag_dyneither(_tmp1058,sizeof(char),25)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1621
Cyc_Tcutil_unify_it(_tmp4EF->type,_tmp4F0->type);
Cyc_Tcutil_unify_tqual(_tmp4EF->tq,_tmp4EF->type,_tmp4F0->tq,_tmp4F0->type);
if(!Cyc_Tcutil_same_atts(_tmp4EF->attributes,_tmp4F0->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp1059;Cyc_Tcutil_failure_reason=((_tmp1059="(different attributes on member)",_tag_dyneither(_tmp1059,sizeof(char),33)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1629
if((_tmp4EF->width != 0  && _tmp4F0->width == 0  || 
_tmp4F0->width != 0  && _tmp4EF->width == 0) || 
(_tmp4EF->width != 0  && _tmp4F0->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4EF->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp4F0->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp105A;Cyc_Tcutil_failure_reason=((_tmp105A="(different bitfield widths on member)",_tag_dyneither(_tmp105A,sizeof(char),38)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1638
if((_tmp4EF->requires_clause != 0  && _tmp4F0->requires_clause == 0  || 
_tmp4EF->requires_clause == 0  && _tmp4F0->requires_clause != 0) || 
(_tmp4EF->requires_clause == 0  && _tmp4F0->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp4EF->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp4F0->requires_clause))){
# 1643
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp105B;Cyc_Tcutil_failure_reason=((_tmp105B="(different @requires clauses on member)",_tag_dyneither(_tmp105B,sizeof(char),40)));}
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1649
if(_tmp476 == 0  && _tmp473 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
{const char*_tmp105C;Cyc_Tcutil_failure_reason=((_tmp105C="(different number of members)",_tag_dyneither(_tmp105C,sizeof(char),30)));}
goto _LL2A1;_LL2C4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp477=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp477->tag != 17)goto _LL2C6;else{_tmp478=_tmp477->f2;_tmp479=_tmp477->f3;}}{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp47A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp47A->tag != 17)goto _LL2C6;else{_tmp47B=_tmp47A->f2;_tmp47C=_tmp47A->f3;}};_LL2C5:
# 1655
 if(_tmp479 != _tmp47C){
{const char*_tmp105D;Cyc_Tcutil_failure_reason=((_tmp105D="(different abstract typedefs)",_tag_dyneither(_tmp105D,sizeof(char),30)));}
goto _LL2A1;}
# 1659
{const char*_tmp105E;Cyc_Tcutil_failure_reason=((_tmp105E="(type parameters to typedef differ)",_tag_dyneither(_tmp105E,sizeof(char),36)));}
Cyc_Tcutil_unify_list(_tmp478,_tmp47B);
return;_LL2C6:{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp47D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp40B.f1;if(_tmp47D->tag != 20)goto _LL2C8;}{struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp47E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp40B.f2;if(_tmp47E->tag != 20)goto _LL2C8;};_LL2C7:
 return;_LL2C8:{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp47F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp40B.f1;if(_tmp47F->tag != 21)goto _LL2CA;}{struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp480=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp40B.f2;if(_tmp480->tag != 21)goto _LL2CA;};_LL2C9:
 return;_LL2CA:{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp481=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp40B.f1;if(_tmp481->tag != 22)goto _LL2CC;}{struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp482=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp40B.f2;if(_tmp482->tag != 22)goto _LL2CC;};_LL2CB:
 return;_LL2CC:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp483=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp483->tag != 15)goto _LL2CE;else{_tmp484=(void*)_tmp483->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp485=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp485->tag != 15)goto _LL2CE;else{_tmp486=(void*)_tmp485->f1;}};_LL2CD:
# 1666
 Cyc_Tcutil_unify_it(_tmp484,_tmp486);
return;_LL2CE:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp487=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp40B.f1;if(_tmp487->tag != 16)goto _LL2D0;else{_tmp488=(void*)_tmp487->f1;_tmp489=(void*)_tmp487->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp48A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp40B.f2;if(_tmp48A->tag != 16)goto _LL2D0;else{_tmp48B=(void*)_tmp48A->f1;_tmp48C=(void*)_tmp48A->f2;}};_LL2CF:
# 1669
 Cyc_Tcutil_unify_it(_tmp488,_tmp48B);
Cyc_Tcutil_unify_it(_tmp489,_tmp48C);
return;_LL2D0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp48D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp40B.f1;if(_tmp48D->tag != 24)goto _LL2D2;}_LL2D1:
 goto _LL2D3;_LL2D2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp48E=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp40B.f2;if(_tmp48E->tag != 24)goto _LL2D4;}_LL2D3:
 goto _LL2D5;_LL2D4: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp48F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp40B.f1;if(_tmp48F->tag != 23)goto _LL2D6;}_LL2D5:
 goto _LL2D7;_LL2D6: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp490=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40B.f1;if(_tmp490->tag != 25)goto _LL2D8;}_LL2D7:
 goto _LL2D9;_LL2D8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp491=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp40B.f2;if(_tmp491->tag != 25)goto _LL2DA;}_LL2D9:
 goto _LL2DB;_LL2DA: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp492=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp40B.f2;if(_tmp492->tag != 23)goto _LL2DC;}_LL2DB:
# 1678
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
{const char*_tmp105F;Cyc_Tcutil_failure_reason=((_tmp105F="(effects don't unify)",_tag_dyneither(_tmp105F,sizeof(char),22)));}
goto _LL2A1;_LL2DC:;_LL2DD:
 goto _LL2A1;_LL2A1:;}
# 1683
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1686
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1693
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp4FA=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp4FB=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp4FA,_tmp4FB);}
# 1700
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp4FC=x;void*_tmp4FD;_LL2F6: if((_tmp4FC->No_constr).tag != 3)goto _LL2F8;_LL2F7:
 return - 1;_LL2F8: if((_tmp4FC->Eq_constr).tag != 1)goto _LL2FA;_tmp4FD=(void*)(_tmp4FC->Eq_constr).val;_LL2F9: {
# 1707
union Cyc_Absyn_Constraint*_tmp4FE=y;void*_tmp4FF;_LL2FD: if((_tmp4FE->No_constr).tag != 3)goto _LL2FF;_LL2FE:
 return 1;_LL2FF: if((_tmp4FE->Eq_constr).tag != 1)goto _LL301;_tmp4FF=(void*)(_tmp4FE->Eq_constr).val;_LL300:
 return cmp(_tmp4FD,_tmp4FF);_LL301: if((_tmp4FE->Forward_constr).tag != 2)goto _LL2FC;_LL302: {
const char*_tmp1062;void*_tmp1061;(_tmp1061=0,Cyc_Tcutil_impos(((_tmp1062="unify_conref: forward after compress(2)",_tag_dyneither(_tmp1062,sizeof(char),40))),_tag_dyneither(_tmp1061,sizeof(void*),0)));}_LL2FC:;}_LL2FA: if((_tmp4FC->Forward_constr).tag != 2)goto _LL2F5;_LL2FB: {
# 1712
const char*_tmp1065;void*_tmp1064;(_tmp1064=0,Cyc_Tcutil_impos(((_tmp1065="unify_conref: forward after compress",_tag_dyneither(_tmp1065,sizeof(char),37))),_tag_dyneither(_tmp1064,sizeof(void*),0)));}_LL2F5:;};}
# 1716
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct Cyc_Absyn_Tqual _tmp505;void*_tmp506;struct _tuple12*_tmp504=tqt1;_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;{
struct Cyc_Absyn_Tqual _tmp508;void*_tmp509;struct _tuple12*_tmp507=tqt2;_tmp508=_tmp507->f1;_tmp509=_tmp507->f2;{
int _tmp50A=Cyc_Tcutil_tqual_cmp(_tmp505,_tmp508);
if(_tmp50A != 0)return _tmp50A;
return Cyc_Tcutil_typecmp(_tmp506,_tmp509);};};}
# 1724
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp50B=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp50B != 0)return _tmp50B;{
int _tmp50C=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp50C != 0)return _tmp50C;{
int _tmp50D=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp50D != 0)return _tmp50D;{
int _tmp50E=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp50E != 0)return _tmp50E;
_tmp50E=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp50E != 0)return _tmp50E;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1738
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp50F=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp50F != 0)return _tmp50F;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1744
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp510=t;_LL304: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp511=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp510;if(_tmp511->tag != 0)goto _LL306;}_LL305:
 return 0;_LL306: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp512=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp510;if(_tmp512->tag != 1)goto _LL308;}_LL307:
 return 1;_LL308: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp513=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp510;if(_tmp513->tag != 2)goto _LL30A;}_LL309:
 return 2;_LL30A: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp514=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp510;if(_tmp514->tag != 3)goto _LL30C;}_LL30B:
 return 3;_LL30C: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp515=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp510;if(_tmp515->tag != 4)goto _LL30E;}_LL30D:
 return 4;_LL30E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp516=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp510;if(_tmp516->tag != 5)goto _LL310;}_LL30F:
 return 5;_LL310: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp517=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp510;if(_tmp517->tag != 6)goto _LL312;}_LL311:
 return 6;_LL312: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp518=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp510;if(_tmp518->tag != 7)goto _LL314;else{if(_tmp518->f1 != 0)goto _LL314;}}_LL313:
 return 7;_LL314: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp519=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp510;if(_tmp519->tag != 7)goto _LL316;}_LL315:
 return 8;_LL316: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp51A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510;if(_tmp51A->tag != 8)goto _LL318;}_LL317:
 return 9;_LL318: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp51B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp510;if(_tmp51B->tag != 9)goto _LL31A;}_LL319:
 return 10;_LL31A: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp51C=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp510;if(_tmp51C->tag != 10)goto _LL31C;}_LL31B:
 return 11;_LL31C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp51D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp510;if(_tmp51D->tag != 11)goto _LL31E;}_LL31D:
 return 12;_LL31E: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp51E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp510;if(_tmp51E->tag != 12)goto _LL320;}_LL31F:
 return 14;_LL320: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp51F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp510;if(_tmp51F->tag != 13)goto _LL322;}_LL321:
 return 16;_LL322: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp520=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp510;if(_tmp520->tag != 14)goto _LL324;}_LL323:
 return 17;_LL324: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp521=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp510;if(_tmp521->tag != 15)goto _LL326;}_LL325:
 return 18;_LL326: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp522=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp510;if(_tmp522->tag != 17)goto _LL328;}_LL327:
 return 19;_LL328: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp523=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp510;if(_tmp523->tag != 21)goto _LL32A;}_LL329:
 return 20;_LL32A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp524=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp510;if(_tmp524->tag != 20)goto _LL32C;}_LL32B:
 return 21;_LL32C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp525=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp510;if(_tmp525->tag != 23)goto _LL32E;}_LL32D:
 return 22;_LL32E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp526=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp510;if(_tmp526->tag != 24)goto _LL330;}_LL32F:
 return 23;_LL330: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp527=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp510;if(_tmp527->tag != 25)goto _LL332;}_LL331:
 return 24;_LL332: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp528=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp510;if(_tmp528->tag != 22)goto _LL334;}_LL333:
 return 25;_LL334: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp529=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp510;if(_tmp529->tag != 19)goto _LL336;}_LL335:
 return 26;_LL336: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp52A=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp510;if(_tmp52A->tag != 16)goto _LL338;}_LL337:
 return 27;_LL338: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp52B=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp510;if(_tmp52B->tag != 18)goto _LL33A;}_LL339:
 return 28;_LL33A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp52C=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp510;if(_tmp52C->tag != 26)goto _LL303;}_LL33B:
 return 29;_LL303:;}
# 1779
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp52D=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp52D != 0)
return _tmp52D;{
# 1788
struct _tuple0 _tmp1066;struct _tuple0 _tmp52E=(_tmp1066.f1=t2,((_tmp1066.f2=t1,_tmp1066)));struct _tuple0 _tmp52F=_tmp52E;struct Cyc_Absyn_Tvar*_tmp533;struct Cyc_Absyn_Tvar*_tmp535;union Cyc_Absyn_AggrInfoU _tmp537;struct Cyc_List_List*_tmp538;union Cyc_Absyn_AggrInfoU _tmp53A;struct Cyc_List_List*_tmp53B;struct _tuple2*_tmp53D;struct _tuple2*_tmp53F;struct Cyc_List_List*_tmp541;struct Cyc_List_List*_tmp543;struct Cyc_Absyn_Datatypedecl*_tmp545;struct Cyc_List_List*_tmp546;struct Cyc_Absyn_Datatypedecl*_tmp548;struct Cyc_List_List*_tmp549;struct Cyc_Absyn_Datatypedecl*_tmp54B;struct Cyc_Absyn_Datatypefield*_tmp54C;struct Cyc_List_List*_tmp54D;struct Cyc_Absyn_Datatypedecl*_tmp54F;struct Cyc_Absyn_Datatypefield*_tmp550;struct Cyc_List_List*_tmp551;void*_tmp553;struct Cyc_Absyn_Tqual _tmp554;void*_tmp555;union Cyc_Absyn_Constraint*_tmp556;union Cyc_Absyn_Constraint*_tmp557;union Cyc_Absyn_Constraint*_tmp558;void*_tmp55A;struct Cyc_Absyn_Tqual _tmp55B;void*_tmp55C;union Cyc_Absyn_Constraint*_tmp55D;union Cyc_Absyn_Constraint*_tmp55E;union Cyc_Absyn_Constraint*_tmp55F;enum Cyc_Absyn_Sign _tmp561;enum Cyc_Absyn_Size_of _tmp562;enum Cyc_Absyn_Sign _tmp564;enum Cyc_Absyn_Size_of _tmp565;int _tmp567;int _tmp569;void*_tmp56B;struct Cyc_Absyn_Tqual _tmp56C;struct Cyc_Absyn_Exp*_tmp56D;union Cyc_Absyn_Constraint*_tmp56E;void*_tmp570;struct Cyc_Absyn_Tqual _tmp571;struct Cyc_Absyn_Exp*_tmp572;union Cyc_Absyn_Constraint*_tmp573;struct Cyc_List_List*_tmp575;void*_tmp576;struct Cyc_Absyn_Tqual _tmp577;void*_tmp578;struct Cyc_List_List*_tmp579;int _tmp57A;struct Cyc_Absyn_VarargInfo*_tmp57B;struct Cyc_List_List*_tmp57C;struct Cyc_List_List*_tmp57D;struct Cyc_List_List*_tmp57F;void*_tmp580;struct Cyc_Absyn_Tqual _tmp581;void*_tmp582;struct Cyc_List_List*_tmp583;int _tmp584;struct Cyc_Absyn_VarargInfo*_tmp585;struct Cyc_List_List*_tmp586;struct Cyc_List_List*_tmp587;struct Cyc_List_List*_tmp589;struct Cyc_List_List*_tmp58B;enum Cyc_Absyn_AggrKind _tmp58D;struct Cyc_List_List*_tmp58E;enum Cyc_Absyn_AggrKind _tmp590;struct Cyc_List_List*_tmp591;void*_tmp593;void*_tmp595;void*_tmp597;void*_tmp598;void*_tmp59A;void*_tmp59B;void*_tmp59D;void*_tmp59F;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A3;_LL33D:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp530=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52F.f1;if(_tmp530->tag != 1)goto _LL33F;}{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp531=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52F.f2;if(_tmp531->tag != 1)goto _LL33F;};_LL33E: {
# 1790
const char*_tmp1069;void*_tmp1068;(_tmp1068=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1069="typecmp: can only compare closed types",_tag_dyneither(_tmp1069,sizeof(char),39))),_tag_dyneither(_tmp1068,sizeof(void*),0)));}_LL33F:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp532=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp532->tag != 2)goto _LL341;else{_tmp533=_tmp532->f1;}}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp534=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp534->tag != 2)goto _LL341;else{_tmp535=_tmp534->f1;}};_LL340:
# 1794
 return Cyc_Core_intcmp(_tmp535->identity,_tmp533->identity);_LL341:{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp536=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp536->tag != 11)goto _LL343;else{_tmp537=(_tmp536->f1).aggr_info;_tmp538=(_tmp536->f1).targs;}}{struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp539=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp539->tag != 11)goto _LL343;else{_tmp53A=(_tmp539->f1).aggr_info;_tmp53B=(_tmp539->f1).targs;}};_LL342: {
# 1798
struct _tuple13 _tmp5AC=Cyc_Absyn_aggr_kinded_name(_tmp537);struct _tuple2*_tmp5AE;struct _tuple13 _tmp5AD=_tmp5AC;_tmp5AE=_tmp5AD.f2;{
struct _tuple13 _tmp5AF=Cyc_Absyn_aggr_kinded_name(_tmp53A);struct _tuple2*_tmp5B1;struct _tuple13 _tmp5B0=_tmp5AF;_tmp5B1=_tmp5B0.f2;{
int _tmp5B2=Cyc_Absyn_qvar_cmp(_tmp5AE,_tmp5B1);
if(_tmp5B2 != 0)return _tmp5B2;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp538,_tmp53B);}};};}_LL343:{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp53C=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp53C->tag != 13)goto _LL345;else{_tmp53D=_tmp53C->f1;}}{struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp53E->tag != 13)goto _LL345;else{_tmp53F=_tmp53E->f1;}};_LL344:
# 1805
 return Cyc_Absyn_qvar_cmp(_tmp53D,_tmp53F);_LL345:{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp540->tag != 14)goto _LL347;else{_tmp541=_tmp540->f1;}}{struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp542->tag != 14)goto _LL347;else{_tmp543=_tmp542->f1;}};_LL346:
# 1808
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp541,_tmp543);_LL347:{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp544->tag != 3)goto _LL349;else{if((((_tmp544->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp545=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp544->f1).datatype_info).KnownDatatype).val);_tmp546=(_tmp544->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp547=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp547->tag != 3)goto _LL349;else{if((((_tmp547->f1).datatype_info).KnownDatatype).tag != 2)goto _LL349;_tmp548=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp547->f1).datatype_info).KnownDatatype).val);_tmp549=(_tmp547->f1).targs;}};_LL348:
# 1812
 if(_tmp548 == _tmp545)return 0;{
int _tmp5B3=Cyc_Absyn_qvar_cmp(_tmp548->name,_tmp545->name);
if(_tmp5B3 != 0)return _tmp5B3;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp549,_tmp546);};_LL349:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp54A->tag != 4)goto _LL34B;else{if((((_tmp54A->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp54B=((struct _tuple3)(((_tmp54A->f1).field_info).KnownDatatypefield).val).f1;_tmp54C=((struct _tuple3)(((_tmp54A->f1).field_info).KnownDatatypefield).val).f2;_tmp54D=(_tmp54A->f1).targs;}}{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp54E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp54E->tag != 4)goto _LL34B;else{if((((_tmp54E->f1).field_info).KnownDatatypefield).tag != 2)goto _LL34B;_tmp54F=((struct _tuple3)(((_tmp54E->f1).field_info).KnownDatatypefield).val).f1;_tmp550=((struct _tuple3)(((_tmp54E->f1).field_info).KnownDatatypefield).val).f2;_tmp551=(_tmp54E->f1).targs;}};_LL34A:
# 1819
 if(_tmp54F == _tmp54B)return 0;{
int _tmp5B4=Cyc_Absyn_qvar_cmp(_tmp54B->name,_tmp54F->name);
if(_tmp5B4 != 0)return _tmp5B4;{
int _tmp5B5=Cyc_Absyn_qvar_cmp(_tmp54C->name,_tmp550->name);
if(_tmp5B5 != 0)return _tmp5B5;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp551,_tmp54D);};};_LL34B:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp552->tag != 5)goto _LL34D;else{_tmp553=(_tmp552->f1).elt_typ;_tmp554=(_tmp552->f1).elt_tq;_tmp555=((_tmp552->f1).ptr_atts).rgn;_tmp556=((_tmp552->f1).ptr_atts).nullable;_tmp557=((_tmp552->f1).ptr_atts).bounds;_tmp558=((_tmp552->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp559=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp559->tag != 5)goto _LL34D;else{_tmp55A=(_tmp559->f1).elt_typ;_tmp55B=(_tmp559->f1).elt_tq;_tmp55C=((_tmp559->f1).ptr_atts).rgn;_tmp55D=((_tmp559->f1).ptr_atts).nullable;_tmp55E=((_tmp559->f1).ptr_atts).bounds;_tmp55F=((_tmp559->f1).ptr_atts).zero_term;}};_LL34C: {
# 1828
int _tmp5B6=Cyc_Tcutil_typecmp(_tmp55A,_tmp553);
if(_tmp5B6 != 0)return _tmp5B6;{
int _tmp5B7=Cyc_Tcutil_typecmp(_tmp55C,_tmp555);
if(_tmp5B7 != 0)return _tmp5B7;{
int _tmp5B8=Cyc_Tcutil_tqual_cmp(_tmp55B,_tmp554);
if(_tmp5B8 != 0)return _tmp5B8;{
int _tmp5B9=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp55E,_tmp557);
if(_tmp5B9 != 0)return _tmp5B9;{
int _tmp5BA=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp55F,_tmp558);
if(_tmp5BA != 0)return _tmp5BA;
{void*_tmp5BB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp55E);void*_tmp5BC=_tmp5BB;_LL370: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5BD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5BC;if(_tmp5BD->tag != 0)goto _LL372;}_LL371:
 return 0;_LL372:;_LL373:
 goto _LL36F;_LL36F:;}
# 1842
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp55D,_tmp556);};};};};}_LL34D:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp560->tag != 6)goto _LL34F;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp563=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp563->tag != 6)goto _LL34F;else{_tmp564=_tmp563->f1;_tmp565=_tmp563->f2;}};_LL34E:
# 1845
 if(_tmp564 != _tmp561)return Cyc_Core_intcmp((int)((unsigned int)_tmp564),(int)((unsigned int)_tmp561));
if(_tmp565 != _tmp562)return Cyc_Core_intcmp((int)((unsigned int)_tmp565),(int)((unsigned int)_tmp562));
return 0;_LL34F:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp566->tag != 7)goto _LL351;else{_tmp567=_tmp566->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp568->tag != 7)goto _LL351;else{_tmp569=_tmp568->f1;}};_LL350:
# 1850
 if(_tmp567 == _tmp569)return 0;else{
if(_tmp569 == 0)return - 1;else{
if(_tmp569 == 1  && _tmp567 == 0)return - 1;else{
return 1;}}}_LL351:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp56A->tag != 8)goto _LL353;else{_tmp56B=(_tmp56A->f1).elt_type;_tmp56C=(_tmp56A->f1).tq;_tmp56D=(_tmp56A->f1).num_elts;_tmp56E=(_tmp56A->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp56F->tag != 8)goto _LL353;else{_tmp570=(_tmp56F->f1).elt_type;_tmp571=(_tmp56F->f1).tq;_tmp572=(_tmp56F->f1).num_elts;_tmp573=(_tmp56F->f1).zero_term;}};_LL352: {
# 1857
int _tmp5BE=Cyc_Tcutil_tqual_cmp(_tmp571,_tmp56C);
if(_tmp5BE != 0)return _tmp5BE;{
int _tmp5BF=Cyc_Tcutil_typecmp(_tmp570,_tmp56B);
if(_tmp5BF != 0)return _tmp5BF;{
int _tmp5C0=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp56E,_tmp573);
if(_tmp5C0 != 0)return _tmp5C0;
if(_tmp56D == _tmp572)return 0;
if(_tmp56D == 0  || _tmp572 == 0){
const char*_tmp106C;void*_tmp106B;(_tmp106B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106C="missing expression in array index",_tag_dyneither(_tmp106C,sizeof(char),34))),_tag_dyneither(_tmp106B,sizeof(void*),0)));}
# 1867
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp56D,_tmp572);};};}_LL353:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp574=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp574->tag != 9)goto _LL355;else{_tmp575=(_tmp574->f1).tvars;_tmp576=(_tmp574->f1).effect;_tmp577=(_tmp574->f1).ret_tqual;_tmp578=(_tmp574->f1).ret_typ;_tmp579=(_tmp574->f1).args;_tmp57A=(_tmp574->f1).c_varargs;_tmp57B=(_tmp574->f1).cyc_varargs;_tmp57C=(_tmp574->f1).rgn_po;_tmp57D=(_tmp574->f1).attributes;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp57E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp57E->tag != 9)goto _LL355;else{_tmp57F=(_tmp57E->f1).tvars;_tmp580=(_tmp57E->f1).effect;_tmp581=(_tmp57E->f1).ret_tqual;_tmp582=(_tmp57E->f1).ret_typ;_tmp583=(_tmp57E->f1).args;_tmp584=(_tmp57E->f1).c_varargs;_tmp585=(_tmp57E->f1).cyc_varargs;_tmp586=(_tmp57E->f1).rgn_po;_tmp587=(_tmp57E->f1).attributes;}};_LL354: {
# 1871
const char*_tmp106F;void*_tmp106E;(_tmp106E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp106F="typecmp: function types not handled",_tag_dyneither(_tmp106F,sizeof(char),36))),_tag_dyneither(_tmp106E,sizeof(void*),0)));}_LL355:{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp588=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp588->tag != 10)goto _LL357;else{_tmp589=_tmp588->f1;}}{struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp58A=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp58A->tag != 10)goto _LL357;else{_tmp58B=_tmp58A->f1;}};_LL356:
# 1874
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp58B,_tmp589);_LL357:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp58C->tag != 12)goto _LL359;else{_tmp58D=_tmp58C->f1;_tmp58E=_tmp58C->f2;}}{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp58F->tag != 12)goto _LL359;else{_tmp590=_tmp58F->f1;_tmp591=_tmp58F->f2;}};_LL358:
# 1877
 if(_tmp590 != _tmp58D){
if(_tmp590 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp591,_tmp58E);_LL359:{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp592=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp592->tag != 15)goto _LL35B;else{_tmp593=(void*)_tmp592->f1;}}{struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp594=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp594->tag != 15)goto _LL35B;else{_tmp595=(void*)_tmp594->f1;}};_LL35A:
# 1882
 return Cyc_Tcutil_typecmp(_tmp593,_tmp595);_LL35B:{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp596=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp596->tag != 16)goto _LL35D;else{_tmp597=(void*)_tmp596->f1;_tmp598=(void*)_tmp596->f2;}}{struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp599->tag != 16)goto _LL35D;else{_tmp59A=(void*)_tmp599->f1;_tmp59B=(void*)_tmp599->f2;}};_LL35C: {
# 1884
int _tmp5C5=Cyc_Tcutil_typecmp(_tmp597,_tmp59A);
if(_tmp5C5 != 0)return _tmp5C5;else{
return Cyc_Tcutil_typecmp(_tmp598,_tmp59B);}}_LL35D:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp59C=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp59C->tag != 19)goto _LL35F;else{_tmp59D=(void*)_tmp59C->f1;}}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp59E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp59E->tag != 19)goto _LL35F;else{_tmp59F=(void*)_tmp59E->f1;}};_LL35E:
 return Cyc_Tcutil_typecmp(_tmp59D,_tmp59F);_LL35F:{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52F.f1;if(_tmp5A0->tag != 18)goto _LL361;else{_tmp5A1=_tmp5A0->f1;}}{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52F.f2;if(_tmp5A2->tag != 18)goto _LL361;else{_tmp5A3=_tmp5A2->f1;}};_LL360:
# 1889
 return Cyc_Evexp_const_exp_cmp(_tmp5A1,_tmp5A3);_LL361: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp52F.f1;if(_tmp5A4->tag != 24)goto _LL363;}_LL362:
 goto _LL364;_LL363: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp52F.f2;if(_tmp5A5->tag != 24)goto _LL365;}_LL364:
 goto _LL366;_LL365: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp52F.f1;if(_tmp5A6->tag != 23)goto _LL367;}_LL366:
 goto _LL368;_LL367: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp52F.f1;if(_tmp5A7->tag != 25)goto _LL369;}_LL368:
 goto _LL36A;_LL369: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp52F.f2;if(_tmp5A8->tag != 25)goto _LL36B;}_LL36A:
 goto _LL36C;_LL36B: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp52F.f2;if(_tmp5A9->tag != 23)goto _LL36D;}_LL36C: {
const char*_tmp1072;void*_tmp1071;(_tmp1071=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1072="typecmp: effects not handled",_tag_dyneither(_tmp1072,sizeof(char),29))),_tag_dyneither(_tmp1071,sizeof(void*),0)));}_LL36D:;_LL36E: {
const char*_tmp1075;void*_tmp1074;(_tmp1074=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1075="Unmatched case in typecmp",_tag_dyneither(_tmp1075,sizeof(char),26))),_tag_dyneither(_tmp1074,sizeof(void*),0)));}_LL33C:;};};}
# 1900
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp5CB=Cyc_Tcutil_compress(t);void*_tmp5CC=_tmp5CB;_LL375: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CD->tag != 6)goto _LL377;}_LL376:
 goto _LL378;_LL377: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5CE=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CE->tag != 7)goto _LL379;}_LL378:
 goto _LL37A;_LL379: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp5CF=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5CC;if(_tmp5CF->tag != 13)goto _LL37B;}_LL37A:
 goto _LL37C;_LL37B: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp5D0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp5CC;if(_tmp5D0->tag != 14)goto _LL37D;}_LL37C:
 return 1;_LL37D:;_LL37E:
 return 0;_LL374:;}
# 1912
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1076;struct _tuple0 _tmp5D1=(_tmp1076.f1=t1,((_tmp1076.f2=t2,_tmp1076)));struct _tuple0 _tmp5D2=_tmp5D1;int _tmp5D4;int _tmp5D6;_LL380:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5D3->tag != 7)goto _LL382;else{_tmp5D4=_tmp5D3->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5D5->tag != 7)goto _LL382;else{_tmp5D6=_tmp5D5->f1;}};_LL381:
# 1917
 return _tmp5D6 == 0  && _tmp5D4 != 0  || (_tmp5D6 == 1  && _tmp5D4 != 0) && _tmp5D4 != 1;_LL382:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5D7->tag != 7)goto _LL384;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5D8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5D8->tag != 6)goto _LL384;};_LL383:
 goto _LL385;_LL384:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5D9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5D9->tag != 7)goto _LL386;}{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5DA->tag != 19)goto _LL386;};_LL385:
 return 1;_LL386:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5DB->tag != 6)goto _LL388;else{if(_tmp5DB->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5DC->tag != 6)goto _LL388;else{if(_tmp5DC->f2 != Cyc_Absyn_LongLong_sz)goto _LL388;}};_LL387:
 return 0;_LL388: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5DD->tag != 6)goto _LL38A;else{if(_tmp5DD->f2 != Cyc_Absyn_LongLong_sz)goto _LL38A;}}_LL389:
 return 1;_LL38A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5DE->tag != 6)goto _LL38C;else{if(_tmp5DE->f2 != Cyc_Absyn_Long_sz)goto _LL38C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5DF->tag != 6)goto _LL38C;else{if(_tmp5DF->f2 != Cyc_Absyn_Int_sz)goto _LL38C;}};_LL38B:
# 1924
 goto _LL38D;_LL38C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E0->tag != 6)goto _LL38E;else{if(_tmp5E0->f2 != Cyc_Absyn_Int_sz)goto _LL38E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E1->tag != 6)goto _LL38E;else{if(_tmp5E1->f2 != Cyc_Absyn_Long_sz)goto _LL38E;}};_LL38D:
 return 0;_LL38E:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E2->tag != 6)goto _LL390;else{if(_tmp5E2->f2 != Cyc_Absyn_Long_sz)goto _LL390;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E3->tag != 7)goto _LL390;else{if(_tmp5E3->f1 != 0)goto _LL390;}};_LL38F:
# 1927
 goto _LL391;_LL390:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E4=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E4->tag != 6)goto _LL392;else{if(_tmp5E4->f2 != Cyc_Absyn_Int_sz)goto _LL392;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E5->tag != 7)goto _LL392;else{if(_tmp5E5->f1 != 0)goto _LL392;}};_LL391:
 goto _LL393;_LL392:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E6->tag != 6)goto _LL394;else{if(_tmp5E6->f2 != Cyc_Absyn_Long_sz)goto _LL394;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E7->tag != 6)goto _LL394;else{if(_tmp5E7->f2 != Cyc_Absyn_Short_sz)goto _LL394;}};_LL393:
 goto _LL395;_LL394:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5E8->tag != 6)goto _LL396;else{if(_tmp5E8->f2 != Cyc_Absyn_Int_sz)goto _LL396;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5E9->tag != 6)goto _LL396;else{if(_tmp5E9->f2 != Cyc_Absyn_Short_sz)goto _LL396;}};_LL395:
 goto _LL397;_LL396:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5EA=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5EA->tag != 19)goto _LL398;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5EB->tag != 6)goto _LL398;else{if(_tmp5EB->f2 != Cyc_Absyn_Short_sz)goto _LL398;}};_LL397:
 goto _LL399;_LL398:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5EC->tag != 6)goto _LL39A;else{if(_tmp5EC->f2 != Cyc_Absyn_Long_sz)goto _LL39A;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5ED->tag != 6)goto _LL39A;else{if(_tmp5ED->f2 != Cyc_Absyn_Char_sz)goto _LL39A;}};_LL399:
 goto _LL39B;_LL39A:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5EE->tag != 6)goto _LL39C;else{if(_tmp5EE->f2 != Cyc_Absyn_Int_sz)goto _LL39C;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5EF->tag != 6)goto _LL39C;else{if(_tmp5EF->f2 != Cyc_Absyn_Char_sz)goto _LL39C;}};_LL39B:
 goto _LL39D;_LL39C:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5F0->tag != 6)goto _LL39E;else{if(_tmp5F0->f2 != Cyc_Absyn_Short_sz)goto _LL39E;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5F1->tag != 6)goto _LL39E;else{if(_tmp5F1->f2 != Cyc_Absyn_Char_sz)goto _LL39E;}};_LL39D:
 goto _LL39F;_LL39E:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp5F2=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp5D2.f1;if(_tmp5F2->tag != 19)goto _LL3A0;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5D2.f2;if(_tmp5F3->tag != 6)goto _LL3A0;else{if(_tmp5F3->f2 != Cyc_Absyn_Char_sz)goto _LL3A0;}};_LL39F:
 return 1;_LL3A0:;_LL3A1:
# 1937
 return 0;_LL37F:;};}
# 1943
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1946
struct _RegionHandle _tmp5F5=_new_region("r");struct _RegionHandle*r=& _tmp5F5;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v)){
struct Cyc_Core_Opt*_tmp1077;max_arith_type=((_tmp1077=_region_malloc(r,sizeof(*_tmp1077)),((_tmp1077->v=t1,_tmp1077))));}}}}
# 1956
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp5F7=0;_npop_handler(0);return _tmp5F7;}}}
# 1961
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
{const char*_tmp107C;void*_tmp107B[2];struct Cyc_String_pa_PrintArg_struct _tmp107A;struct Cyc_String_pa_PrintArg_struct _tmp1079;(_tmp1079.tag=0,((_tmp1079.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt))),((_tmp107A.tag=0,((_tmp107A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp107B[0]=& _tmp107A,((_tmp107B[1]=& _tmp1079,Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,((_tmp107C="type mismatch: expecting %s but found %s",_tag_dyneither(_tmp107C,sizeof(char),41))),_tag_dyneither(_tmp107B,sizeof(void*),2)))))))))))));}{
int _tmp5FC=0;_npop_handler(0);return _tmp5FC;};}}}{
# 1967
int _tmp5FD=1;_npop_handler(0);return _tmp5FD;};
# 1946
;_pop_region(r);}
# 1972
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp5FE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5FF=_tmp5FE;_LL3A3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp600=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FF;if(_tmp600->tag != 5)goto _LL3A5;}_LL3A4:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL3A2;_LL3A5:;_LL3A6:
 return 0;_LL3A2:;}
# 1978
return 1;}
# 1981
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp601=Cyc_Tcutil_compress(t);void*_tmp602=_tmp601;_LL3A8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp603=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp602;if(_tmp603->tag != 6)goto _LL3AA;}_LL3A9:
 goto _LL3AB;_LL3AA: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp604=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp602;if(_tmp604->tag != 19)goto _LL3AC;}_LL3AB:
 goto _LL3AD;_LL3AC: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp605=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp602;if(_tmp605->tag != 13)goto _LL3AE;}_LL3AD:
 goto _LL3AF;_LL3AE: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp606=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp602;if(_tmp606->tag != 14)goto _LL3B0;}_LL3AF:
 return 1;_LL3B0:;_LL3B1:
 return 0;_LL3A7:;}
# 1992
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 1996
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ)){
const char*_tmp107F;void*_tmp107E;(_tmp107E=0,Cyc_Tcutil_warn(e->loc,((_tmp107F="integral size mismatch; conversion supplied",_tag_dyneither(_tmp107F,sizeof(char),44))),_tag_dyneither(_tmp107E,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2002
return 0;}
# 2006
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2010
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ)){
const char*_tmp1082;void*_tmp1081;(_tmp1081=0,Cyc_Tcutil_warn(e->loc,((_tmp1082="integral size mismatch; conversion supplied",_tag_dyneither(_tmp1082,sizeof(char),44))),_tag_dyneither(_tmp1081,sizeof(void*),0)));}
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2016
return 0;}
# 2021
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2029
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2031
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp1083;struct _tuple0 _tmp60B=(_tmp1083.f1=t1,((_tmp1083.f2=t2,_tmp1083)));struct _tuple0 _tmp60C=_tmp60B;struct Cyc_Absyn_PtrInfo _tmp60E;struct Cyc_Absyn_PtrInfo _tmp610;void*_tmp612;struct Cyc_Absyn_Tqual _tmp613;struct Cyc_Absyn_Exp*_tmp614;union Cyc_Absyn_Constraint*_tmp615;void*_tmp617;struct Cyc_Absyn_Tqual _tmp618;struct Cyc_Absyn_Exp*_tmp619;union Cyc_Absyn_Constraint*_tmp61A;_LL3B3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60C.f1;if(_tmp60D->tag != 5)goto _LL3B5;else{_tmp60E=_tmp60D->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp60F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60C.f2;if(_tmp60F->tag != 5)goto _LL3B5;else{_tmp610=_tmp60F->f1;}};_LL3B4: {
# 2035
int okay=1;
# 2037
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp60E.ptr_atts).nullable,(_tmp610.ptr_atts).nullable))
# 2040
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60E.ptr_atts).nullable);
# 2042
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp60E.ptr_atts).bounds,(_tmp610.ptr_atts).bounds)){
# 2045
struct _tuple0 _tmp1084;struct _tuple0 _tmp61D=(_tmp1084.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp60E.ptr_atts).bounds),((_tmp1084.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp610.ptr_atts).bounds),_tmp1084)));
# 2045
struct _tuple0 _tmp61E=_tmp61D;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Exp*_tmp629;_LL3BC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp61F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61E.f1;if(_tmp61F->tag != 1)goto _LL3BE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp620=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61E.f2;if(_tmp620->tag != 0)goto _LL3BE;};_LL3BD:
# 2047
 goto _LL3BF;_LL3BE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp621=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61E.f1;if(_tmp621->tag != 0)goto _LL3C0;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp622=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61E.f2;if(_tmp622->tag != 0)goto _LL3C0;};_LL3BF:
 okay=1;goto _LL3BB;_LL3C0:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp623=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61E.f1;if(_tmp623->tag != 1)goto _LL3C2;else{_tmp624=_tmp623->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp625=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61E.f2;if(_tmp625->tag != 1)goto _LL3C2;else{_tmp626=_tmp625->f1;}};_LL3C1:
# 2050
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp626,_tmp624);
# 2054
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp610.ptr_atts).zero_term)){
const char*_tmp1087;void*_tmp1086;(_tmp1086=0,Cyc_Tcutil_warn(loc,((_tmp1087="implicit cast to shorter array",_tag_dyneither(_tmp1087,sizeof(char),31))),_tag_dyneither(_tmp1086,sizeof(void*),0)));}
goto _LL3BB;_LL3C2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp627=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp61E.f1;if(_tmp627->tag != 0)goto _LL3BB;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp628=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp61E.f2;if(_tmp628->tag != 1)goto _LL3BB;else{_tmp629=_tmp628->f1;}};_LL3C3:
# 2059
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp60E.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp610.ptr_atts).bounds))
goto _LL3BB;
okay=0;
goto _LL3BB;_LL3BB:;}
# 2068
okay=okay  && (!(_tmp60E.elt_tq).real_const  || (_tmp610.elt_tq).real_const);
# 2071
if(!Cyc_Tcutil_unify((_tmp60E.ptr_atts).rgn,(_tmp610.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp60E.ptr_atts).rgn,(_tmp610.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce){
const char*_tmp108C;void*_tmp108B[2];struct Cyc_String_pa_PrintArg_struct _tmp108A;struct Cyc_String_pa_PrintArg_struct _tmp1089;(_tmp1089.tag=0,((_tmp1089.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2076
Cyc_Absynpp_typ2string((_tmp610.ptr_atts).rgn)),((_tmp108A.tag=0,((_tmp108A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2075
Cyc_Absynpp_typ2string((_tmp60E.ptr_atts).rgn)),((_tmp108B[0]=& _tmp108A,((_tmp108B[1]=& _tmp1089,Cyc_Tcutil_warn(loc,((_tmp108C="implicit cast form region %s to region %s",_tag_dyneither(_tmp108C,sizeof(char),42))),_tag_dyneither(_tmp108B,sizeof(void*),2)))))))))))));}}else{
# 2077
okay=0;}}
# 2081
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp60E.ptr_atts).zero_term,(_tmp610.ptr_atts).zero_term) || 
# 2084
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp60E.ptr_atts).zero_term) && (_tmp610.elt_tq).real_const);{
# 2092
int _tmp631=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp610.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2095
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp610.ptr_atts).zero_term);
# 2098
okay=okay  && (Cyc_Tcutil_unify(_tmp60E.elt_typ,_tmp610.elt_typ) || 
(_tmp631  && ((_tmp610.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp610.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp60E.elt_typ,_tmp610.elt_typ));
# 2101
return okay;};}_LL3B5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp611=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60C.f1;if(_tmp611->tag != 8)goto _LL3B7;else{_tmp612=(_tmp611->f1).elt_type;_tmp613=(_tmp611->f1).tq;_tmp614=(_tmp611->f1).num_elts;_tmp615=(_tmp611->f1).zero_term;}}{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp616=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60C.f2;if(_tmp616->tag != 8)goto _LL3B7;else{_tmp617=(_tmp616->f1).elt_type;_tmp618=(_tmp616->f1).tq;_tmp619=(_tmp616->f1).num_elts;_tmp61A=(_tmp616->f1).zero_term;}};_LL3B6: {
# 2105
int okay;
# 2107
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp615,_tmp61A) && (
(_tmp614 != 0  && _tmp619 != 0) && Cyc_Evexp_same_const_exp(_tmp614,_tmp619));
# 2110
return(okay  && Cyc_Tcutil_unify(_tmp612,_tmp617)) && (!_tmp613.real_const  || _tmp618.real_const);}_LL3B7:{struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp61B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp60C.f1;if(_tmp61B->tag != 19)goto _LL3B9;}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp61C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp60C.f2;if(_tmp61C->tag != 6)goto _LL3B9;};_LL3B8:
# 2112
 return 0;_LL3B9:;_LL3BA:
# 2114
 return Cyc_Tcutil_unify(t1,t2);_LL3B2:;};}
# 2118
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp633=Cyc_Tcutil_compress(t);void*_tmp634=_tmp633;_LL3C5: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp635=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp634;if(_tmp635->tag != 5)goto _LL3C7;}_LL3C6:
 return 1;_LL3C7:;_LL3C8:
 return 0;_LL3C4:;}
# 2124
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp636=Cyc_Tcutil_compress(t);void*_tmp637=_tmp636;void*_tmp639;_LL3CA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp638=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp637;if(_tmp638->tag != 5)goto _LL3CC;else{_tmp639=(_tmp638->f1).elt_typ;}}_LL3CB:
 return _tmp639;_LL3CC:;_LL3CD: {
const char*_tmp108F;void*_tmp108E;(_tmp108E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp108F="pointer_elt_type",_tag_dyneither(_tmp108F,sizeof(char),17))),_tag_dyneither(_tmp108E,sizeof(void*),0)));}_LL3C9:;}
# 2130
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp63C=Cyc_Tcutil_compress(t);void*_tmp63D=_tmp63C;struct Cyc_Absyn_PtrAtts*_tmp63F;_LL3CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp63E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp63D;if(_tmp63E->tag != 5)goto _LL3D1;else{_tmp63F=(struct Cyc_Absyn_PtrAtts*)&(_tmp63E->f1).ptr_atts;}}_LL3D0:
 return _tmp63F->rgn;_LL3D1:;_LL3D2: {
const char*_tmp1092;void*_tmp1091;(_tmp1091=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1092="pointer_elt_type",_tag_dyneither(_tmp1092,sizeof(char),17))),_tag_dyneither(_tmp1091,sizeof(void*),0)));}_LL3CE:;}
# 2137
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp642=Cyc_Tcutil_compress(t);void*_tmp643=_tmp642;void*_tmp645;_LL3D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp644=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp643;if(_tmp644->tag != 5)goto _LL3D6;else{_tmp645=((_tmp644->f1).ptr_atts).rgn;}}_LL3D5:
# 2140
*rgn=_tmp645;
return 1;_LL3D6:;_LL3D7:
 return 0;_LL3D3:;}
# 2147
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2151
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp646=Cyc_Tcutil_compress(t);void*_tmp647=_tmp646;union Cyc_Absyn_Constraint*_tmp649;_LL3D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp648=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp647;if(_tmp648->tag != 5)goto _LL3DB;else{_tmp649=((_tmp648->f1).ptr_atts).bounds;}}_LL3DA:
# 2154
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp649)== (void*)& Cyc_Absyn_DynEither_b_val;_LL3DB:;_LL3DC:
 return 0;_LL3D8:;}
# 2160
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp64A=e->r;void*_tmp64B=_tmp64A;struct _dyneither_ptr _tmp651;void*_tmp653;struct Cyc_Absyn_Exp*_tmp654;_LL3DE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp64C->tag != 0)goto _LL3E0;else{if(((_tmp64C->f1).Int_c).tag != 5)goto _LL3E0;if(((struct _tuple7)((_tmp64C->f1).Int_c).val).f2 != 0)goto _LL3E0;}}_LL3DF:
 goto _LL3E1;_LL3E0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp64D->tag != 0)goto _LL3E2;else{if(((_tmp64D->f1).Char_c).tag != 2)goto _LL3E2;if(((struct _tuple5)((_tmp64D->f1).Char_c).val).f2 != 0)goto _LL3E2;}}_LL3E1:
 goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp64E->tag != 0)goto _LL3E4;else{if(((_tmp64E->f1).Short_c).tag != 4)goto _LL3E4;if(((struct _tuple6)((_tmp64E->f1).Short_c).val).f2 != 0)goto _LL3E4;}}_LL3E3:
 goto _LL3E5;_LL3E4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp64F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp64F->tag != 0)goto _LL3E6;else{if(((_tmp64F->f1).LongLong_c).tag != 6)goto _LL3E6;if(((struct _tuple8)((_tmp64F->f1).LongLong_c).val).f2 != 0)goto _LL3E6;}}_LL3E5:
# 2166
 return 1;_LL3E6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp650->tag != 0)goto _LL3E8;else{if(((_tmp650->f1).Wchar_c).tag != 3)goto _LL3E8;_tmp651=(struct _dyneither_ptr)((_tmp650->f1).Wchar_c).val;}}_LL3E7: {
# 2168
unsigned long _tmp655=Cyc_strlen((struct _dyneither_ptr)_tmp651);
int i=0;
if(_tmp655 >= 2  && *((const char*)_check_dyneither_subscript(_tmp651,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp651,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp651,sizeof(char),1))== 'x'  && _tmp655 >= 3) && *((const char*)_check_dyneither_subscript(_tmp651,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp655;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp651,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2178
return 0;}}_LL3E8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp652=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp64B;if(_tmp652->tag != 13)goto _LL3EA;else{_tmp653=(void*)_tmp652->f1;_tmp654=_tmp652->f2;}}_LL3E9:
 return Cyc_Tcutil_is_zero(_tmp654) && Cyc_Tcutil_admits_zero(_tmp653);_LL3EA:;_LL3EB:
 return 0;_LL3DD:;}
# 2184
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2191
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2196
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2201
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2208
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2213
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2218
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp657;enum Cyc_Absyn_AliasQual _tmp658;struct Cyc_Absyn_Kind*_tmp656=ka;_tmp657=_tmp656->kind;_tmp658=_tmp656->aliasqual;
switch(_tmp658){case Cyc_Absyn_Aliasable: _LL3EC:
# 2222
 switch(_tmp657){case Cyc_Absyn_AnyKind: _LL3EE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL3EF:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL3F0:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL3F1:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL3F2:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL3F3:
 return& Cyc_Tcutil_iko;}case Cyc_Absyn_Unique: _LL3ED:
# 2231
 switch(_tmp657){case Cyc_Absyn_AnyKind: _LL3F6:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL3F7:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL3F8:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL3F9:
 return& Cyc_Tcutil_urko;default: _LL3FA:
 break;}
# 2238
break;case Cyc_Absyn_Top: _LL3F5:
# 2240
 switch(_tmp657){case Cyc_Absyn_AnyKind: _LL3FD:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL3FE:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL3FF:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL400:
 return& Cyc_Tcutil_trko;default: _LL401:
 break;}
# 2247
break;}{
# 2249
const char*_tmp1096;void*_tmp1095[1];struct Cyc_String_pa_PrintArg_struct _tmp1094;(_tmp1094.tag=0,((_tmp1094.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1095[0]=& _tmp1094,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1096="kind_to_opt: bad kind %s\n",_tag_dyneither(_tmp1096,sizeof(char),26))),_tag_dyneither(_tmp1095,sizeof(void*),1)))))));};}
# 2252
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2260
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2267
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2272
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2277
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2282
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2287
enum Cyc_Absyn_KindQual _tmp65D;enum Cyc_Absyn_AliasQual _tmp65E;struct Cyc_Absyn_Kind*_tmp65C=ka;_tmp65D=_tmp65C->kind;_tmp65E=_tmp65C->aliasqual;
switch(_tmp65E){case Cyc_Absyn_Aliasable: _LL403:
# 2290
 switch(_tmp65D){case Cyc_Absyn_AnyKind: _LL405:
 return& ab;case Cyc_Absyn_MemKind: _LL406:
 return& mb;case Cyc_Absyn_BoxKind: _LL407:
 return& bb;case Cyc_Absyn_RgnKind: _LL408:
 return& rb;case Cyc_Absyn_EffKind: _LL409:
 return& eb;case Cyc_Absyn_IntKind: _LL40A:
 return& ib;}case Cyc_Absyn_Unique: _LL404:
# 2299
 switch(_tmp65D){case Cyc_Absyn_AnyKind: _LL40D:
 return& uab;case Cyc_Absyn_MemKind: _LL40E:
 return& umb;case Cyc_Absyn_BoxKind: _LL40F:
 return& ubb;case Cyc_Absyn_RgnKind: _LL410:
 return& urb;default: _LL411:
 break;}
# 2306
break;case Cyc_Absyn_Top: _LL40C:
# 2308
 switch(_tmp65D){case Cyc_Absyn_AnyKind: _LL414:
 return& tab;case Cyc_Absyn_MemKind: _LL415:
 return& tmb;case Cyc_Absyn_BoxKind: _LL416:
 return& tbb;case Cyc_Absyn_RgnKind: _LL417:
 return& trb;default: _LL418:
 break;}
# 2315
break;}{
# 2317
const char*_tmp109A;void*_tmp1099[1];struct Cyc_String_pa_PrintArg_struct _tmp1098;(_tmp1098.tag=0,((_tmp1098.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka)),((_tmp1099[0]=& _tmp1098,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp109A="kind_to_b: bad kind %s\n",_tag_dyneither(_tmp109A,sizeof(char),24))),_tag_dyneither(_tmp1099,sizeof(void*),1)))))));};}
# 2320
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2323
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2325
struct Cyc_Core_Opt*_tmp109B;return(_tmp109B=_cycalloc(sizeof(*_tmp109B)),((_tmp109B->v=Cyc_Tcutil_kind_to_bound(k),_tmp109B)));}
# 2330
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp109E;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp109D;e1->r=(void*)((_tmp109D=_cycalloc(sizeof(*_tmp109D)),((_tmp109D[0]=((_tmp109E.tag=0,((_tmp109E.f1=Cyc_Absyn_Null_c,_tmp109E)))),_tmp109D))));}{
struct Cyc_Core_Opt*_tmp673=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp10A4;struct Cyc_Absyn_PtrInfo _tmp10A3;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10A2;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp674=(_tmp10A2=_cycalloc(sizeof(*_tmp10A2)),((_tmp10A2[0]=((_tmp10A4.tag=5,((_tmp10A4.f1=((_tmp10A3.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp673),((_tmp10A3.elt_tq=
Cyc_Absyn_empty_tqual(0),((_tmp10A3.ptr_atts=(
((_tmp10A3.ptr_atts).rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp673),(((_tmp10A3.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp10A3.ptr_atts).bounds=
# 2338
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp10A3.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),(((_tmp10A3.ptr_atts).ptrloc=0,_tmp10A3.ptr_atts)))))))))),_tmp10A3)))))),_tmp10A4)))),_tmp10A2)));
e1->topt=(void*)_tmp674;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0){
const char*_tmp10A9;void*_tmp10A8[2];struct Cyc_String_pa_PrintArg_struct _tmp10A7;struct Cyc_String_pa_PrintArg_struct _tmp10A6;(_tmp10A6.tag=0,((_tmp10A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc)),((_tmp10A7.tag=0,((_tmp10A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp10A8[0]=& _tmp10A7,((_tmp10A8[1]=& _tmp10A6,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10A9="zero_to_null resulted in an alias coercion on %s at %s\n",_tag_dyneither(_tmp10A9,sizeof(char),56))),_tag_dyneither(_tmp10A8,sizeof(void*),2)))))))))))));}
return retv;};};}
# 2348
return 0;}
# 2351
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
switch(c){case Cyc_Absyn_Unknown_coercion: _LL41A: {
const char*_tmp10AA;return(_tmp10AA="unknown",_tag_dyneither(_tmp10AA,sizeof(char),8));}case Cyc_Absyn_No_coercion: _LL41B: {
const char*_tmp10AB;return(_tmp10AB="no coercion",_tag_dyneither(_tmp10AB,sizeof(char),12));}case Cyc_Absyn_NonNull_to_Null: _LL41C: {
const char*_tmp10AC;return(_tmp10AC="null check",_tag_dyneither(_tmp10AC,sizeof(char),11));}case Cyc_Absyn_Other_coercion: _LL41D: {
const char*_tmp10AD;return(_tmp10AD="other coercion",_tag_dyneither(_tmp10AD,sizeof(char),15));}}}
# 2360
int Cyc_Tcutil_warn_alias_coerce=0;
# 2366
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2370
static int counter=0;
struct _dyneither_ptr*_tmp10BA;const char*_tmp10B9;void*_tmp10B8[1];struct Cyc_Int_pa_PrintArg_struct _tmp10B7;struct _tuple2*_tmp10B6;struct _tuple2*v=(_tmp10B6=_cycalloc(sizeof(*_tmp10B6)),((_tmp10B6->f1=Cyc_Absyn_Loc_n,((_tmp10B6->f2=((_tmp10BA=_cycalloc(sizeof(*_tmp10BA)),((_tmp10BA[0]=(struct _dyneither_ptr)((_tmp10B7.tag=1,((_tmp10B7.f1=(unsigned long)counter ++,((_tmp10B8[0]=& _tmp10B7,Cyc_aprintf(((_tmp10B9="__aliasvar%d",_tag_dyneither(_tmp10B9,sizeof(char),13))),_tag_dyneither(_tmp10B8,sizeof(void*),1)))))))),_tmp10BA)))),_tmp10B6)))));
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp10BD;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp10BC;struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp(v,(void*)((_tmp10BC=_cycalloc(sizeof(*_tmp10BC)),((_tmp10BC[0]=((_tmp10BD.tag=4,((_tmp10BD.f1=vd,_tmp10BD)))),_tmp10BC)))),e->loc);
# 2379
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2381
{void*_tmp680=Cyc_Tcutil_compress(e_typ);void*_tmp681=_tmp680;void*_tmp683;struct Cyc_Absyn_Tqual _tmp684;void*_tmp685;union Cyc_Absyn_Constraint*_tmp686;union Cyc_Absyn_Constraint*_tmp687;union Cyc_Absyn_Constraint*_tmp688;struct Cyc_Absyn_PtrLoc*_tmp689;_LL420: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp682=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp681;if(_tmp682->tag != 5)goto _LL422;else{_tmp683=(_tmp682->f1).elt_typ;_tmp684=(_tmp682->f1).elt_tq;_tmp685=((_tmp682->f1).ptr_atts).rgn;_tmp686=((_tmp682->f1).ptr_atts).nullable;_tmp687=((_tmp682->f1).ptr_atts).bounds;_tmp688=((_tmp682->f1).ptr_atts).zero_term;_tmp689=((_tmp682->f1).ptr_atts).ptrloc;}}_LL421:
# 2383
{void*_tmp68A=Cyc_Tcutil_compress(_tmp685);void*_tmp68B=_tmp68A;void**_tmp68D;struct Cyc_Core_Opt*_tmp68E;_LL425: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp68C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp68B;if(_tmp68C->tag != 1)goto _LL427;else{_tmp68D=(void**)((void**)& _tmp68C->f2);_tmp68E=_tmp68C->f4;}}_LL426: {
# 2385
struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10C0;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10BF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68F=(_tmp10BF=_cycalloc(sizeof(*_tmp10BF)),((_tmp10BF[0]=((_tmp10C0.tag=2,((_tmp10C0.f1=tv,_tmp10C0)))),_tmp10BF)));
*_tmp68D=(void*)_tmp68F;
goto _LL424;}_LL427:;_LL428:
 goto _LL424;_LL424:;}
# 2390
goto _LL41F;_LL422:;_LL423:
 goto _LL41F;_LL41F:;}
# 2394
e->topt=0;
vd->initializer=0;{
# 2398
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2400
struct _tuple15 _tmp10C1;return(_tmp10C1.f1=d,((_tmp10C1.f2=ve,_tmp10C1)));};}
# 2405
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2408
struct _RegionHandle _tmp69B=_new_region("r");struct _RegionHandle*r=& _tmp69B;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2413
void*_tmp69C=Cyc_Tcutil_compress(wants_typ);void*_tmp69D=_tmp69C;void*_tmp69F;_LL42A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69D;if(_tmp69E->tag != 5)goto _LL42C;else{_tmp69F=((_tmp69E->f1).ptr_atts).rgn;}}_LL42B: {
# 2415
void*_tmp6A1=Cyc_Tcutil_compress(_tmp69F);void*_tmp6A2=_tmp6A1;_LL431: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6A3=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6A2;if(_tmp6A3->tag != 20)goto _LL433;}_LL432: {
int _tmp6A4=0;_npop_handler(0);return _tmp6A4;}_LL433:;_LL434: {
# 2418
struct Cyc_Absyn_Kind*_tmp6A5=Cyc_Tcutil_typ_kind(_tmp69F);
int _tmp6A6=_tmp6A5->kind == Cyc_Absyn_RgnKind  && _tmp6A5->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp6A6;}_LL430:;}_LL42C: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp6A0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp69D;if(_tmp6A0->tag != 17)goto _LL42E;}_LL42D: {
# 2422
const char*_tmp10C6;void*_tmp10C5[2];struct Cyc_String_pa_PrintArg_struct _tmp10C4;struct Cyc_String_pa_PrintArg_struct _tmp10C3;(_tmp10C3.tag=0,((_tmp10C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc)),((_tmp10C4.tag=0,((_tmp10C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ))),((_tmp10C5[0]=& _tmp10C4,((_tmp10C5[1]=& _tmp10C3,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp10C6="got typedef %s in can_insert_alias at %s\n",_tag_dyneither(_tmp10C6,sizeof(char),42))),_tag_dyneither(_tmp10C5,sizeof(void*),2)))))))))))));}_LL42E:;_LL42F: {
int _tmp6AB=0;_npop_handler(0);return _tmp6AB;}_LL429:;}{
# 2427
int _tmp6AC=0;_npop_handler(0);return _tmp6AC;};
# 2409
;_pop_region(r);}
# 2431
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2436
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2438
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2440
if(Cyc_Tcutil_will_lose_precision(t1,t2)){
const char*_tmp10CB;void*_tmp10CA[2];struct Cyc_String_pa_PrintArg_struct _tmp10C9;struct Cyc_String_pa_PrintArg_struct _tmp10C8;(_tmp10C8.tag=0,((_tmp10C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10C9.tag=0,((_tmp10C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10CA[0]=& _tmp10C9,((_tmp10CA[1]=& _tmp10C8,Cyc_Tcutil_warn(e->loc,((_tmp10CB="integral size mismatch; %s -> %s conversion supplied",_tag_dyneither(_tmp10CB,sizeof(char),53))),_tag_dyneither(_tmp10CA,sizeof(void*),2)))))))))))));}
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2449
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce){
const char*_tmp10D1;void*_tmp10D0[3];struct Cyc_String_pa_PrintArg_struct _tmp10CF;struct Cyc_String_pa_PrintArg_struct _tmp10CE;struct Cyc_String_pa_PrintArg_struct _tmp10CD;(_tmp10CD.tag=0,((_tmp10CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10CE.tag=0,((_tmp10CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10CF.tag=0,((_tmp10CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmp10D0[0]=& _tmp10CF,((_tmp10D0[1]=& _tmp10CE,((_tmp10D0[2]=& _tmp10CD,Cyc_Tcutil_warn(e->loc,((_tmp10D1="implicit alias coercion for %s:%s to %s",_tag_dyneither(_tmp10D1,sizeof(char),40))),_tag_dyneither(_tmp10D0,sizeof(void*),3)))))))))))))))))));}
*alias_coercion=1;}
# 2456
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2464
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_NonNull_to_Null){
const char*_tmp10D6;void*_tmp10D5[2];struct Cyc_String_pa_PrintArg_struct _tmp10D4;struct Cyc_String_pa_PrintArg_struct _tmp10D3;(_tmp10D3.tag=0,((_tmp10D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2)),((_tmp10D4.tag=0,((_tmp10D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1)),((_tmp10D5[0]=& _tmp10D4,((_tmp10D5[1]=& _tmp10D3,Cyc_Tcutil_warn(e->loc,((_tmp10D6="implicit cast from %s to %s",_tag_dyneither(_tmp10D6,sizeof(char),28))),_tag_dyneither(_tmp10D5,sizeof(void*),2)))))))))))));}
return 1;}else{
# 2470
return 0;}}}}}
# 2477
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2480
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2484
int Cyc_Tcutil_coerceable(void*t){
void*_tmp6BA=Cyc_Tcutil_compress(t);void*_tmp6BB=_tmp6BA;_LL436: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp6BC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6BB;if(_tmp6BC->tag != 6)goto _LL438;}_LL437:
 goto _LL439;_LL438: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp6BD=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6BB;if(_tmp6BD->tag != 7)goto _LL43A;}_LL439:
 return 1;_LL43A:;_LL43B:
 return 0;_LL435:;}
# 2502 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2506
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2509
struct _tuple23 _tmp6BE=*env;struct Cyc_List_List*_tmp6C0;struct _RegionHandle*_tmp6C1;struct Cyc_Tcenv_Tenv*_tmp6C2;int _tmp6C3;struct _tuple23 _tmp6BF=_tmp6BE;_tmp6C0=_tmp6BF.f1;_tmp6C1=_tmp6BF.f2;_tmp6C2=_tmp6BF.f3;_tmp6C3=_tmp6BF.f4;{
# 2511
void*_tmp6C4=_tmp6C0 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp6C1,_tmp6C0,x->type);
struct Cyc_List_List*_tmp6C5=Cyc_Tcutil_flatten_typ(_tmp6C1,_tmp6C3,_tmp6C2,_tmp6C4);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6C5)== 1){
struct _tuple12*_tmp10D9;struct Cyc_List_List*_tmp10D8;return(_tmp10D8=_region_malloc(_tmp6C1,sizeof(*_tmp10D8)),((_tmp10D8->hd=((_tmp10D9=_region_malloc(_tmp6C1,sizeof(*_tmp10D9)),((_tmp10D9->f1=x->tq,((_tmp10D9->f2=_tmp6C4,_tmp10D9)))))),((_tmp10D8->tl=0,_tmp10D8)))));}else{
return _tmp6C5;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2517
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2519
struct _tuple24 _tmp6C8=*env;struct _RegionHandle*_tmp6CA;struct Cyc_Tcenv_Tenv*_tmp6CB;int _tmp6CC;struct _tuple24 _tmp6C9=_tmp6C8;_tmp6CA=_tmp6C9.f1;_tmp6CB=_tmp6C9.f2;_tmp6CC=_tmp6C9.f3;{
struct _tuple12 _tmp6CD=*x;struct Cyc_Absyn_Tqual _tmp6CF;void*_tmp6D0;struct _tuple12 _tmp6CE=_tmp6CD;_tmp6CF=_tmp6CE.f1;_tmp6D0=_tmp6CE.f2;{
struct Cyc_List_List*_tmp6D1=Cyc_Tcutil_flatten_typ(_tmp6CA,_tmp6CC,_tmp6CB,_tmp6D0);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6D1)== 1){
struct _tuple12*_tmp10DC;struct Cyc_List_List*_tmp10DB;return(_tmp10DB=_region_malloc(_tmp6CA,sizeof(*_tmp10DB)),((_tmp10DB->hd=((_tmp10DC=_region_malloc(_tmp6CA,sizeof(*_tmp10DC)),((_tmp10DC->f1=_tmp6CF,((_tmp10DC->f2=_tmp6D0,_tmp10DC)))))),((_tmp10DB->tl=0,_tmp10DB)))));}else{
return _tmp6D1;}};};}
# 2526
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2530
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp6D4=t1;struct Cyc_List_List*_tmp6D7;struct Cyc_Absyn_Aggrdecl*_tmp6D9;struct Cyc_List_List*_tmp6DA;struct Cyc_List_List*_tmp6DC;_LL43D: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp6D5=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6D4;if(_tmp6D5->tag != 0)goto _LL43F;}_LL43E:
 return 0;_LL43F: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D4;if(_tmp6D6->tag != 10)goto _LL441;else{_tmp6D7=_tmp6D6->f1;}}_LL440: {
# 2535
struct _tuple24 _tmp10DD;struct _tuple24 _tmp6DD=(_tmp10DD.f1=r,((_tmp10DD.f2=te,((_tmp10DD.f3=flatten,_tmp10DD)))));
# 2537
struct Cyc_List_List*_tmp6DE=_tmp6D7;struct _tuple12*_tmp6DF;struct Cyc_List_List*_tmp6E0;_LL448: if(_tmp6DE != 0)goto _LL44A;_LL449:
 return 0;_LL44A: if(_tmp6DE == 0)goto _LL447;_tmp6DF=(struct _tuple12*)_tmp6DE->hd;_tmp6E0=_tmp6DE->tl;_LL44B: {
# 2540
struct Cyc_List_List*_tmp6E1=Cyc_Tcutil_rcopy_tqt(& _tmp6DD,_tmp6DF);
_tmp6DD.f3=0;{
struct Cyc_List_List*_tmp6E2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp6DD,_tmp6D7);
struct Cyc_List_List*_tmp10DE;struct Cyc_List_List*_tmp6E3=(_tmp10DE=_region_malloc(r,sizeof(*_tmp10DE)),((_tmp10DE->hd=_tmp6E1,((_tmp10DE->tl=_tmp6E2,_tmp10DE)))));
struct Cyc_List_List*_tmp10DF;return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,((_tmp10DF=_region_malloc(r,sizeof(*_tmp10DF)),((_tmp10DF->hd=_tmp6E1,((_tmp10DF->tl=_tmp6E2,_tmp10DF)))))));};}_LL447:;}_LL441: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp6D8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6D4;if(_tmp6D8->tag != 11)goto _LL443;else{if((((_tmp6D8->f1).aggr_info).KnownAggr).tag != 2)goto _LL443;_tmp6D9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp6D8->f1).aggr_info).KnownAggr).val);_tmp6DA=(_tmp6D8->f1).targs;}}_LL442:
# 2548
 if(((_tmp6D9->kind == Cyc_Absyn_UnionA  || _tmp6D9->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D9->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D9->impl))->rgn_po != 0){
# 2550
struct _tuple12*_tmp10E2;struct Cyc_List_List*_tmp10E1;return(_tmp10E1=_region_malloc(r,sizeof(*_tmp10E1)),((_tmp10E1->hd=((_tmp10E2=_region_malloc(r,sizeof(*_tmp10E2)),((_tmp10E2->f1=Cyc_Absyn_empty_tqual(0),((_tmp10E2->f2=t1,_tmp10E2)))))),((_tmp10E1->tl=0,_tmp10E1)))));}{
struct Cyc_List_List*_tmp6E9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp6D9->tvs,_tmp6DA);
struct _tuple23 _tmp10E3;struct _tuple23 env=(_tmp10E3.f1=_tmp6E9,((_tmp10E3.f2=r,((_tmp10E3.f3=te,((_tmp10E3.f4=flatten,_tmp10E3)))))));
struct Cyc_List_List*_tmp6EA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6D9->impl))->fields;struct Cyc_List_List*_tmp6EB=_tmp6EA;struct Cyc_Absyn_Aggrfield*_tmp6EC;struct Cyc_List_List*_tmp6ED;_LL44D: if(_tmp6EB != 0)goto _LL44F;_LL44E:
 return 0;_LL44F: if(_tmp6EB == 0)goto _LL44C;_tmp6EC=(struct Cyc_Absyn_Aggrfield*)_tmp6EB->hd;_tmp6ED=_tmp6EB->tl;_LL450: {
# 2556
struct Cyc_List_List*_tmp6EE=Cyc_Tcutil_flatten_typ_f(& env,_tmp6EC);
env.f4=0;{
struct Cyc_List_List*_tmp6EF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6ED);
struct Cyc_List_List*_tmp10E4;struct Cyc_List_List*_tmp6F0=(_tmp10E4=_region_malloc(r,sizeof(*_tmp10E4)),((_tmp10E4->hd=_tmp6EE,((_tmp10E4->tl=_tmp6EF,_tmp10E4)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6F0);};}_LL44C:;};_LL443: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6DB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D4;if(_tmp6DB->tag != 12)goto _LL445;else{if(_tmp6DB->f1 != Cyc_Absyn_StructA)goto _LL445;_tmp6DC=_tmp6DB->f2;}}_LL444: {
# 2563
struct _tuple23 _tmp10E5;struct _tuple23 env=(_tmp10E5.f1=0,((_tmp10E5.f2=r,((_tmp10E5.f3=te,((_tmp10E5.f4=flatten,_tmp10E5)))))));
struct Cyc_List_List*_tmp6F3=_tmp6DC;struct Cyc_Absyn_Aggrfield*_tmp6F4;struct Cyc_List_List*_tmp6F5;_LL452: if(_tmp6F3 != 0)goto _LL454;_LL453:
 return 0;_LL454: if(_tmp6F3 == 0)goto _LL451;_tmp6F4=(struct Cyc_Absyn_Aggrfield*)_tmp6F3->hd;_tmp6F5=_tmp6F3->tl;_LL455: {
# 2567
struct Cyc_List_List*_tmp6F6=Cyc_Tcutil_flatten_typ_f(& env,_tmp6F4);
env.f4=0;{
struct Cyc_List_List*_tmp6F7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp6F5);
struct Cyc_List_List*_tmp10E6;struct Cyc_List_List*_tmp6F8=(_tmp10E6=_region_malloc(r,sizeof(*_tmp10E6)),((_tmp10E6->hd=_tmp6F6,((_tmp10E6->tl=_tmp6F7,_tmp10E6)))));
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp6F8);};}_LL451:;}_LL445:;_LL446:
# 2573
 goto _LL43C;_LL43C:;};}{
# 2576
struct _tuple12*_tmp10E9;struct Cyc_List_List*_tmp10E8;return(_tmp10E8=_region_malloc(r,sizeof(*_tmp10E8)),((_tmp10E8->hd=((_tmp10E9=_region_malloc(r,sizeof(*_tmp10E9)),((_tmp10E9->f1=Cyc_Absyn_empty_tqual(0),((_tmp10E9->f2=t1,_tmp10E9)))))),((_tmp10E8->tl=0,_tmp10E8)))));};}
# 2580
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp6FD=(void*)t->hd;void*_tmp6FE=_tmp6FD;_LL457: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp6FF=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp6FE;if(_tmp6FF->tag != 23)goto _LL459;}_LL458:
 goto _LL45A;_LL459: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp700=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp6FE;if(_tmp700->tag != 4)goto _LL45B;}_LL45A:
 goto _LL45C;_LL45B: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp701=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp6FE;if(_tmp701->tag != 20)goto _LL45D;}_LL45C:
# 2586
 continue;_LL45D:;_LL45E:
# 2588
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;_LL456:;}}
# 2591
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2594
return 1;}
# 2597
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp10EA;struct _tuple0 _tmp702=(_tmp10EA.f1=Cyc_Tcutil_compress(t1),((_tmp10EA.f2=Cyc_Tcutil_compress(t2),_tmp10EA)));struct _tuple0 _tmp703=_tmp702;enum Cyc_Absyn_Size_of _tmp705;enum Cyc_Absyn_Size_of _tmp707;_LL460:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp704=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp703.f1;if(_tmp704->tag != 6)goto _LL462;else{_tmp705=_tmp704->f2;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp706=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp703.f2;if(_tmp706->tag != 6)goto _LL462;else{_tmp707=_tmp706->f2;}};_LL461:
# 2600
 return(_tmp705 == _tmp707  || _tmp705 == Cyc_Absyn_Int_sz  && _tmp707 == Cyc_Absyn_Long_sz) || 
_tmp705 == Cyc_Absyn_Long_sz  && _tmp707 == Cyc_Absyn_Int_sz;_LL462:;_LL463:
 return 0;_LL45F:;}
# 2608
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2611
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2616
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp10EB;struct _tuple0 _tmp709=(_tmp10EB.f1=t1,((_tmp10EB.f2=t2,_tmp10EB)));struct _tuple0 _tmp70A=_tmp709;void*_tmp70C;struct Cyc_Absyn_Tqual _tmp70D;void*_tmp70E;union Cyc_Absyn_Constraint*_tmp70F;union Cyc_Absyn_Constraint*_tmp710;union Cyc_Absyn_Constraint*_tmp711;void*_tmp713;struct Cyc_Absyn_Tqual _tmp714;void*_tmp715;union Cyc_Absyn_Constraint*_tmp716;union Cyc_Absyn_Constraint*_tmp717;union Cyc_Absyn_Constraint*_tmp718;struct Cyc_Absyn_Datatypedecl*_tmp71A;struct Cyc_Absyn_Datatypefield*_tmp71B;struct Cyc_List_List*_tmp71C;struct Cyc_Absyn_Datatypedecl*_tmp71E;struct Cyc_List_List*_tmp71F;struct Cyc_Absyn_FnInfo _tmp721;struct Cyc_Absyn_FnInfo _tmp723;_LL465:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp70A.f1;if(_tmp70B->tag != 5)goto _LL467;else{_tmp70C=(_tmp70B->f1).elt_typ;_tmp70D=(_tmp70B->f1).elt_tq;_tmp70E=((_tmp70B->f1).ptr_atts).rgn;_tmp70F=((_tmp70B->f1).ptr_atts).nullable;_tmp710=((_tmp70B->f1).ptr_atts).bounds;_tmp711=((_tmp70B->f1).ptr_atts).zero_term;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp712=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp70A.f2;if(_tmp712->tag != 5)goto _LL467;else{_tmp713=(_tmp712->f1).elt_typ;_tmp714=(_tmp712->f1).elt_tq;_tmp715=((_tmp712->f1).ptr_atts).rgn;_tmp716=((_tmp712->f1).ptr_atts).nullable;_tmp717=((_tmp712->f1).ptr_atts).bounds;_tmp718=((_tmp712->f1).ptr_atts).zero_term;}};_LL466:
# 2624
 if(_tmp70D.real_const  && !_tmp714.real_const)
return 0;
# 2627
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp70F,_tmp716) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp70F)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp716))
return 0;
# 2631
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp711,_tmp718) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp711)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp718))
return 0;
# 2635
if((!Cyc_Tcutil_unify(_tmp70E,_tmp715) && !Cyc_Tcenv_region_outlives(te,_tmp70E,_tmp715)) && !
Cyc_Tcutil_subtype(te,assume,_tmp70E,_tmp715))
return 0;
# 2639
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp710,_tmp717)){
struct _tuple0 _tmp10EC;struct _tuple0 _tmp724=(_tmp10EC.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp710),((_tmp10EC.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp717),_tmp10EC)));struct _tuple0 _tmp725=_tmp724;struct Cyc_Absyn_Exp*_tmp729;struct Cyc_Absyn_Exp*_tmp72B;_LL46E:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp726=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp725.f1;if(_tmp726->tag != 1)goto _LL470;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp727=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp725.f2;if(_tmp727->tag != 0)goto _LL470;};_LL46F:
 goto _LL46D;_LL470:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp728=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp725.f1;if(_tmp728->tag != 1)goto _LL472;else{_tmp729=_tmp728->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp72A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp725.f2;if(_tmp72A->tag != 1)goto _LL472;else{_tmp72B=_tmp72A->f1;}};_LL471:
# 2643
 if(!Cyc_Evexp_lte_const_exp(_tmp72B,_tmp729))
return 0;
goto _LL46D;_LL472:;_LL473:
 return 0;_LL46D:;}
# 2651
if(!_tmp714.real_const  && _tmp70D.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp713)))
return 0;}{
# 2657
int _tmp72D=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp717,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp718);
# 2663
struct _tuple0*_tmp10EF;struct Cyc_List_List*_tmp10EE;return(_tmp72D  && Cyc_Tcutil_ptrsubtype(te,((_tmp10EE=_cycalloc(sizeof(*_tmp10EE)),((_tmp10EE->hd=((_tmp10EF=_cycalloc(sizeof(*_tmp10EF)),((_tmp10EF->f1=t1,((_tmp10EF->f2=t2,_tmp10EF)))))),((_tmp10EE->tl=assume,_tmp10EE)))))),_tmp70C,_tmp713) || Cyc_Tcutil_unify(_tmp70C,_tmp713)) || Cyc_Tcutil_isomorphic(_tmp70C,_tmp713);};_LL467:{struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp719=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp70A.f1;if(_tmp719->tag != 4)goto _LL469;else{if((((_tmp719->f1).field_info).KnownDatatypefield).tag != 2)goto _LL469;_tmp71A=((struct _tuple3)(((_tmp719->f1).field_info).KnownDatatypefield).val).f1;_tmp71B=((struct _tuple3)(((_tmp719->f1).field_info).KnownDatatypefield).val).f2;_tmp71C=(_tmp719->f1).targs;}}{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp71D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp70A.f2;if(_tmp71D->tag != 3)goto _LL469;else{if((((_tmp71D->f1).datatype_info).KnownDatatype).tag != 2)goto _LL469;_tmp71E=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp71D->f1).datatype_info).KnownDatatype).val);_tmp71F=(_tmp71D->f1).targs;}};_LL468:
# 2669
 if(_tmp71A != _tmp71E  && Cyc_Absyn_qvar_cmp(_tmp71A->name,_tmp71E->name)!= 0)return 0;
# 2671
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp71C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp71F))return 0;
for(0;_tmp71C != 0;(_tmp71C=_tmp71C->tl,_tmp71F=_tmp71F->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp71C->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp71F))->hd))return 0;}
return 1;_LL469:{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp720=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp70A.f1;if(_tmp720->tag != 9)goto _LL46B;else{_tmp721=_tmp720->f1;}}{struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp722=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp70A.f2;if(_tmp722->tag != 9)goto _LL46B;else{_tmp723=_tmp722->f1;}};_LL46A:
# 2678
 if(_tmp721.tvars != 0  || _tmp723.tvars != 0){
struct Cyc_List_List*_tmp730=_tmp721.tvars;
struct Cyc_List_List*_tmp731=_tmp723.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp730)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp731))return 0;{
struct _RegionHandle _tmp732=_new_region("r");struct _RegionHandle*r=& _tmp732;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp730 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp730->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp731))->hd)->kind)){int _tmp733=0;_npop_handler(0);return _tmp733;}
{struct _tuple16*_tmp10F9;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp10F8;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp10F7;struct Cyc_List_List*_tmp10F6;inst=((_tmp10F6=_region_malloc(r,sizeof(*_tmp10F6)),((_tmp10F6->hd=((_tmp10F9=_region_malloc(r,sizeof(*_tmp10F9)),((_tmp10F9->f1=(struct Cyc_Absyn_Tvar*)_tmp731->hd,((_tmp10F9->f2=(void*)((_tmp10F7=_cycalloc(sizeof(*_tmp10F7)),((_tmp10F7[0]=((_tmp10F8.tag=2,((_tmp10F8.f1=(struct Cyc_Absyn_Tvar*)_tmp730->hd,_tmp10F8)))),_tmp10F7)))),_tmp10F9)))))),((_tmp10F6->tl=inst,_tmp10F6))))));}
_tmp730=_tmp730->tl;
_tmp731=_tmp731->tl;}
# 2690
if(inst != 0){
_tmp721.tvars=0;
_tmp723.tvars=0;{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10FF;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10FE;struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp10FC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp10FB;int _tmp73C=Cyc_Tcutil_subtype(te,assume,(void*)((_tmp10FB=_cycalloc(sizeof(*_tmp10FB)),((_tmp10FB[0]=((_tmp10FC.tag=9,((_tmp10FC.f1=_tmp721,_tmp10FC)))),_tmp10FB)))),(void*)((_tmp10FE=_cycalloc(sizeof(*_tmp10FE)),((_tmp10FE[0]=((_tmp10FF.tag=9,((_tmp10FF.f1=_tmp723,_tmp10FF)))),_tmp10FE)))));_npop_handler(0);return _tmp73C;};}}
# 2683
;_pop_region(r);};}
# 2698
if(!Cyc_Tcutil_subtype(te,assume,_tmp721.ret_typ,_tmp723.ret_typ))return 0;{
struct Cyc_List_List*_tmp73D=_tmp721.args;
struct Cyc_List_List*_tmp73E=_tmp723.args;
# 2703
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp73D)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp73E))return 0;
# 2705
for(0;_tmp73D != 0;(_tmp73D=_tmp73D->tl,_tmp73E=_tmp73E->tl)){
struct _tuple10 _tmp73F=*((struct _tuple10*)_tmp73D->hd);struct Cyc_Absyn_Tqual _tmp741;void*_tmp742;struct _tuple10 _tmp740=_tmp73F;_tmp741=_tmp740.f2;_tmp742=_tmp740.f3;{
struct _tuple10 _tmp743=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp73E))->hd);struct Cyc_Absyn_Tqual _tmp745;void*_tmp746;struct _tuple10 _tmp744=_tmp743;_tmp745=_tmp744.f2;_tmp746=_tmp744.f3;
# 2709
if(_tmp745.real_const  && !_tmp741.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp746,_tmp742))
return 0;};}
# 2713
if(_tmp721.c_varargs != _tmp723.c_varargs)return 0;
if(_tmp721.cyc_varargs != 0  && _tmp723.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp747=*_tmp721.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp748=*_tmp723.cyc_varargs;
# 2718
if((_tmp748.tq).real_const  && !(_tmp747.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp748.type,_tmp747.type))
return 0;}else{
if(_tmp721.cyc_varargs != 0  || _tmp723.cyc_varargs != 0)return 0;}
# 2723
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp721.effect),(void*)_check_null(_tmp723.effect)))return 0;
# 2725
if(!Cyc_Tcutil_sub_rgnpo(_tmp721.rgn_po,_tmp723.rgn_po))return 0;
# 2727
if(!Cyc_Tcutil_sub_attributes(_tmp721.attributes,_tmp723.attributes))return 0;
# 2729
return 1;};_LL46B:;_LL46C:
 return 0;_LL464:;};}
# 2741 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2743
struct _RegionHandle _tmp74A=_new_region("temp");struct _RegionHandle*temp=& _tmp74A;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp74B=0;_npop_handler(0);return _tmp74B;}{
struct _tuple12*_tmp74C=(struct _tuple12*)tqs1->hd;struct Cyc_Absyn_Tqual _tmp74E;void*_tmp74F;struct _tuple12*_tmp74D=_tmp74C;_tmp74E=_tmp74D->f1;_tmp74F=_tmp74D->f2;{
struct _tuple12*_tmp750=(struct _tuple12*)tqs2->hd;struct Cyc_Absyn_Tqual _tmp752;void*_tmp753;struct _tuple12*_tmp751=_tmp750;_tmp752=_tmp751->f1;_tmp753=_tmp751->f2;
# 2751
if(_tmp74E.real_const  && !_tmp752.real_const){int _tmp754=0;_npop_handler(0);return _tmp754;}
# 2753
if((_tmp752.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp753))) && 
Cyc_Tcutil_subtype(te,assume,_tmp74F,_tmp753))
# 2756
continue;
# 2758
if(Cyc_Tcutil_unify(_tmp74F,_tmp753))
# 2760
continue;
# 2762
if(Cyc_Tcutil_isomorphic(_tmp74F,_tmp753))
# 2764
continue;{
# 2767
int _tmp755=0;_npop_handler(0);return _tmp755;};};};}{
# 2769
int _tmp756=1;_npop_handler(0);return _tmp756;};}
# 2744
;_pop_region(temp);}
# 2773
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp757=Cyc_Tcutil_compress(t);void*_tmp758=_tmp757;_LL475: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp759=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp758;if(_tmp759->tag != 6)goto _LL477;else{if(_tmp759->f2 != Cyc_Absyn_Char_sz)goto _LL477;}}_LL476:
 return 1;_LL477:;_LL478:
 return 0;_LL474:;}
# 2782
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2788
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp75A=t2;_LL47A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75A;if(_tmp75B->tag != 6)goto _LL47C;else{if(_tmp75B->f2 != Cyc_Absyn_Int_sz)goto _LL47C;}}_LL47B:
# 2792
 goto _LL47D;_LL47C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp75C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75A;if(_tmp75C->tag != 6)goto _LL47E;else{if(_tmp75C->f2 != Cyc_Absyn_Long_sz)goto _LL47E;}}_LL47D:
# 2794
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL479;_LL47E:;_LL47F:
 goto _LL479;_LL479:;}{
# 2798
void*_tmp75D=t1;void*_tmp75F;struct Cyc_Absyn_Tqual _tmp760;void*_tmp761;union Cyc_Absyn_Constraint*_tmp762;union Cyc_Absyn_Constraint*_tmp763;union Cyc_Absyn_Constraint*_tmp764;void*_tmp766;struct Cyc_Absyn_Tqual _tmp767;struct Cyc_Absyn_Exp*_tmp768;union Cyc_Absyn_Constraint*_tmp769;struct Cyc_Absyn_Enumdecl*_tmp76B;void*_tmp76F;_LL481: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp75E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp75D;if(_tmp75E->tag != 5)goto _LL483;else{_tmp75F=(_tmp75E->f1).elt_typ;_tmp760=(_tmp75E->f1).elt_tq;_tmp761=((_tmp75E->f1).ptr_atts).rgn;_tmp762=((_tmp75E->f1).ptr_atts).nullable;_tmp763=((_tmp75E->f1).ptr_atts).bounds;_tmp764=((_tmp75E->f1).ptr_atts).zero_term;}}_LL482:
# 2806
{void*_tmp770=t2;void*_tmp772;struct Cyc_Absyn_Tqual _tmp773;void*_tmp774;union Cyc_Absyn_Constraint*_tmp775;union Cyc_Absyn_Constraint*_tmp776;union Cyc_Absyn_Constraint*_tmp777;_LL490: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp771=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp770;if(_tmp771->tag != 5)goto _LL492;else{_tmp772=(_tmp771->f1).elt_typ;_tmp773=(_tmp771->f1).elt_tq;_tmp774=((_tmp771->f1).ptr_atts).rgn;_tmp775=((_tmp771->f1).ptr_atts).nullable;_tmp776=((_tmp771->f1).ptr_atts).bounds;_tmp777=((_tmp771->f1).ptr_atts).zero_term;}}_LL491: {
# 2810
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct _tuple0*_tmp1102;struct Cyc_List_List*_tmp1101;struct Cyc_List_List*_tmp778=(_tmp1101=_cycalloc(sizeof(*_tmp1101)),((_tmp1101->hd=((_tmp1102=_cycalloc(sizeof(*_tmp1102)),((_tmp1102->f1=t1,((_tmp1102->f2=t2,_tmp1102)))))),((_tmp1101->tl=0,_tmp1101)))));
int _tmp779=_tmp773.real_const  || !_tmp760.real_const;
# 2824 "tcutil.cyc"
int _tmp77A=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp776,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp777);
# 2828
int _tmp77B=_tmp779  && (
((_tmp77A  && Cyc_Tcutil_ptrsubtype(te,_tmp778,_tmp75F,_tmp772) || 
Cyc_Tcutil_unify(_tmp75F,_tmp772)) || Cyc_Tcutil_isomorphic(_tmp75F,_tmp772)) || Cyc_Tcutil_unify(_tmp772,(void*)& Cyc_Absyn_VoidType_val));
# 2832
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp764,_tmp777) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp777);
# 2836
int _tmp77C=_tmp77B?0:((Cyc_Tcutil_bits_only(_tmp75F) && Cyc_Tcutil_is_char_type(_tmp772)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp777)) && (
_tmp773.real_const  || !_tmp760.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp763,_tmp776);
if(!bounds_ok  && !_tmp77C){
struct _tuple0 _tmp1103;struct _tuple0 _tmp77D=(_tmp1103.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp763),((_tmp1103.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp776),_tmp1103)));struct _tuple0 _tmp77E=_tmp77D;struct Cyc_Absyn_Exp*_tmp780;struct Cyc_Absyn_Exp*_tmp782;_LL495:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp77F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp77E.f1;if(_tmp77F->tag != 1)goto _LL497;else{_tmp780=_tmp77F->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp781=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp77E.f2;if(_tmp781->tag != 1)goto _LL497;else{_tmp782=_tmp781->f1;}};_LL496:
# 2846
 if(Cyc_Evexp_lte_const_exp(_tmp782,_tmp780))
bounds_ok=1;
goto _LL494;_LL497:;_LL498:
# 2851
 bounds_ok=1;goto _LL494;_LL494:;}
# 2853
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp762) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp775))
coercion=Cyc_Absyn_NonNull_to_Null;
# 2858
if(((bounds_ok  && zeroterm_ok) && (_tmp77B  || _tmp77C)) && (
Cyc_Tcutil_unify(_tmp761,_tmp774) || Cyc_Tcenv_region_outlives(te,_tmp761,_tmp774)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}_LL492:;_LL493:
 goto _LL48F;_LL48F:;}
# 2864
return Cyc_Absyn_Unknown_coercion;_LL483: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp765=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp75D;if(_tmp765->tag != 8)goto _LL485;else{_tmp766=(_tmp765->f1).elt_type;_tmp767=(_tmp765->f1).tq;_tmp768=(_tmp765->f1).num_elts;_tmp769=(_tmp765->f1).zero_term;}}_LL484:
# 2866
{void*_tmp786=t2;void*_tmp788;struct Cyc_Absyn_Tqual _tmp789;struct Cyc_Absyn_Exp*_tmp78A;union Cyc_Absyn_Constraint*_tmp78B;_LL49A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp787=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp786;if(_tmp787->tag != 8)goto _LL49C;else{_tmp788=(_tmp787->f1).elt_type;_tmp789=(_tmp787->f1).tq;_tmp78A=(_tmp787->f1).num_elts;_tmp78B=(_tmp787->f1).zero_term;}}_LL49B: {
# 2868
int okay;
okay=
(((_tmp768 != 0  && _tmp78A != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp769,_tmp78B)) && 
Cyc_Evexp_lte_const_exp(_tmp78A,_tmp768)) && 
Cyc_Evexp_lte_const_exp(_tmp768,_tmp78A);
return
# 2875
(okay  && Cyc_Tcutil_unify(_tmp766,_tmp788)) && (!_tmp767.real_const  || _tmp789.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}_LL49C:;_LL49D:
# 2877
 return Cyc_Absyn_Unknown_coercion;_LL499:;}
# 2879
return Cyc_Absyn_Unknown_coercion;_LL485: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp76A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp75D;if(_tmp76A->tag != 13)goto _LL487;else{_tmp76B=_tmp76A->f2;}}_LL486:
# 2883
{void*_tmp78C=t2;struct Cyc_Absyn_Enumdecl*_tmp78E;_LL49F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp78D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp78C;if(_tmp78D->tag != 13)goto _LL4A1;else{_tmp78E=_tmp78D->f2;}}_LL4A0:
# 2885
 if((_tmp76B->fields != 0  && _tmp78E->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp76B->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp78E->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL49E;_LL4A1:;_LL4A2:
 goto _LL49E;_LL49E:;}
# 2891
goto _LL488;_LL487: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp76C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp75D;if(_tmp76C->tag != 6)goto _LL489;}_LL488:
 goto _LL48A;_LL489: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp76D=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp75D;if(_tmp76D->tag != 7)goto _LL48B;}_LL48A:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;_LL48B: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp76E=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp75D;if(_tmp76E->tag != 15)goto _LL48D;else{_tmp76F=(void*)_tmp76E->f1;}}_LL48C:
# 2896
{void*_tmp78F=t2;void*_tmp791;_LL4A4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp790=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp78F;if(_tmp790->tag != 15)goto _LL4A6;else{_tmp791=(void*)_tmp790->f1;}}_LL4A5:
# 2898
 if(Cyc_Tcenv_region_outlives(te,_tmp76F,_tmp791))return Cyc_Absyn_No_coercion;
goto _LL4A3;_LL4A6:;_LL4A7:
 goto _LL4A3;_LL4A3:;}
# 2902
return Cyc_Absyn_Unknown_coercion;_LL48D:;_LL48E:
 return Cyc_Absyn_Unknown_coercion;_LL480:;};}
# 2908
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp792=Cyc_Absyn_copy_exp(e);
{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp1106;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp1105;e->r=(void*)((_tmp1105=_cycalloc(sizeof(*_tmp1105)),((_tmp1105[0]=((_tmp1106.tag=13,((_tmp1106.f1=t,((_tmp1106.f2=_tmp792,((_tmp1106.f3=0,((_tmp1106.f4=c,_tmp1106)))))))))),_tmp1105))));}
e->topt=t;}}
# 2916
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp795=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp796=_tmp795;_LL4A9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp797=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp796;if(_tmp797->tag != 6)goto _LL4AB;}_LL4AA:
 goto _LL4AC;_LL4AB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp798=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp796;if(_tmp798->tag != 13)goto _LL4AD;}_LL4AC:
 goto _LL4AE;_LL4AD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp799=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp796;if(_tmp799->tag != 14)goto _LL4AF;}_LL4AE:
 goto _LL4B0;_LL4AF: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp79A=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp796;if(_tmp79A->tag != 19)goto _LL4B1;}_LL4B0:
 return 1;_LL4B1: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp79B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp796;if(_tmp79B->tag != 1)goto _LL4B3;}_LL4B2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);_LL4B3:;_LL4B4:
 return 0;_LL4A8:;}
# 2927
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp79C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp79D=_tmp79C;_LL4B6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp79E=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp79D;if(_tmp79E->tag != 7)goto _LL4B8;}_LL4B7:
 return 1;_LL4B8:;_LL4B9:
 return 0;_LL4B5:;};}
# 2936
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp79F=Cyc_Tcutil_compress(t);void*_tmp7A0=_tmp79F;_LL4BB: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp7A1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp7A0;if(_tmp7A1->tag != 9)goto _LL4BD;}_LL4BC:
 return 1;_LL4BD:;_LL4BE:
 return 0;_LL4BA:;}
# 2943
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp1107;struct _tuple0 _tmp7A2=(_tmp1107.f1=t1,((_tmp1107.f2=t2,_tmp1107)));struct _tuple0 _tmp7A3=_tmp7A2;int _tmp7A5;int _tmp7A7;_LL4C0:{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A4=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7A4->tag != 7)goto _LL4C2;else{_tmp7A5=_tmp7A4->f1;}}{struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A6=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7A6->tag != 7)goto _LL4C2;else{_tmp7A7=_tmp7A6->f1;}};_LL4C1:
# 2946
 if(_tmp7A5 != 0  && _tmp7A5 != 1)return t1;else{
if(_tmp7A7 != 0  && _tmp7A7 != 1)return t2;else{
if(_tmp7A5 >= _tmp7A7)return t1;else{
return t2;}}}_LL4C2: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A8=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7A8->tag != 7)goto _LL4C4;}_LL4C3:
 return t1;_LL4C4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp7A9=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7A9->tag != 7)goto _LL4C6;}_LL4C5:
 return t2;_LL4C6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7AA->tag != 6)goto _LL4C8;else{if(_tmp7AA->f1 != Cyc_Absyn_Unsigned)goto _LL4C8;if(_tmp7AA->f2 != Cyc_Absyn_LongLong_sz)goto _LL4C8;}}_LL4C7:
 goto _LL4C9;_LL4C8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7AB->tag != 6)goto _LL4CA;else{if(_tmp7AB->f1 != Cyc_Absyn_Unsigned)goto _LL4CA;if(_tmp7AB->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CA;}}_LL4C9:
 return Cyc_Absyn_ulonglong_typ;_LL4CA: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7AC->tag != 6)goto _LL4CC;else{if(_tmp7AC->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CC;}}_LL4CB:
 goto _LL4CD;_LL4CC: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AD=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7AD->tag != 6)goto _LL4CE;else{if(_tmp7AD->f2 != Cyc_Absyn_LongLong_sz)goto _LL4CE;}}_LL4CD:
 return Cyc_Absyn_slonglong_typ;_LL4CE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7AE->tag != 6)goto _LL4D0;else{if(_tmp7AE->f1 != Cyc_Absyn_Unsigned)goto _LL4D0;if(_tmp7AE->f2 != Cyc_Absyn_Long_sz)goto _LL4D0;}}_LL4CF:
 goto _LL4D1;_LL4D0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7AF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7AF->tag != 6)goto _LL4D2;else{if(_tmp7AF->f1 != Cyc_Absyn_Unsigned)goto _LL4D2;if(_tmp7AF->f2 != Cyc_Absyn_Long_sz)goto _LL4D2;}}_LL4D1:
 return Cyc_Absyn_ulong_typ;_LL4D2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7B0->tag != 6)goto _LL4D4;else{if(_tmp7B0->f1 != Cyc_Absyn_Unsigned)goto _LL4D4;if(_tmp7B0->f2 != Cyc_Absyn_Int_sz)goto _LL4D4;}}_LL4D3:
# 2959
 goto _LL4D5;_LL4D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7B1->tag != 6)goto _LL4D6;else{if(_tmp7B1->f1 != Cyc_Absyn_Unsigned)goto _LL4D6;if(_tmp7B1->f2 != Cyc_Absyn_Int_sz)goto _LL4D6;}}_LL4D5:
 return Cyc_Absyn_uint_typ;_LL4D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f1;if(_tmp7B2->tag != 6)goto _LL4D8;else{if(_tmp7B2->f2 != Cyc_Absyn_Long_sz)goto _LL4D8;}}_LL4D7:
 goto _LL4D9;_LL4D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7B3=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7A3.f2;if(_tmp7B3->tag != 6)goto _LL4DA;else{if(_tmp7B3->f2 != Cyc_Absyn_Long_sz)goto _LL4DA;}}_LL4D9:
 return Cyc_Absyn_slong_typ;_LL4DA:;_LL4DB:
 return Cyc_Absyn_sint_typ;_LL4BF:;}
# 2968
void Cyc_Tcutil_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp7B5=e->r;void*_tmp7B6=_tmp7B5;_LL4DD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp7B7=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp7B6;if(_tmp7B7->tag != 3)goto _LL4DF;else{if(_tmp7B7->f2 != 0)goto _LL4DF;}}_LL4DE:
{const char*_tmp110A;void*_tmp1109;(_tmp1109=0,Cyc_Tcutil_warn(e->loc,((_tmp110A="assignment in test",_tag_dyneither(_tmp110A,sizeof(char),19))),_tag_dyneither(_tmp1109,sizeof(void*),0)));}goto _LL4DC;_LL4DF:;_LL4E0:
 goto _LL4DC;_LL4DC:;}
# 2982 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);{
struct _tuple0 _tmp110B;struct _tuple0 _tmp7BA=(_tmp110B.f1=c1,((_tmp110B.f2=c2,_tmp110B)));struct _tuple0 _tmp7BB=_tmp7BA;struct Cyc_Absyn_Kind*_tmp7BD;struct Cyc_Absyn_Kind*_tmp7BF;struct Cyc_Core_Opt**_tmp7C1;struct Cyc_Core_Opt**_tmp7C3;struct Cyc_Core_Opt**_tmp7C5;struct Cyc_Absyn_Kind*_tmp7C6;struct Cyc_Absyn_Kind*_tmp7C8;struct Cyc_Absyn_Kind*_tmp7CA;struct Cyc_Core_Opt**_tmp7CC;struct Cyc_Absyn_Kind*_tmp7CD;struct Cyc_Core_Opt**_tmp7CF;struct Cyc_Absyn_Kind*_tmp7D0;struct Cyc_Core_Opt**_tmp7D2;struct Cyc_Absyn_Kind*_tmp7D3;_LL4E2:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7BC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BB.f1;if(_tmp7BC->tag != 0)goto _LL4E4;else{_tmp7BD=_tmp7BC->f1;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7BE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BB.f2;if(_tmp7BE->tag != 0)goto _LL4E4;else{_tmp7BF=_tmp7BE->f1;}};_LL4E3:
 return _tmp7BD == _tmp7BF;_LL4E4: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7C0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7BB.f2;if(_tmp7C0->tag != 1)goto _LL4E6;else{_tmp7C1=(struct Cyc_Core_Opt**)& _tmp7C0->f1;}}_LL4E5:
{struct Cyc_Core_Opt*_tmp110C;*_tmp7C1=((_tmp110C=_cycalloc(sizeof(*_tmp110C)),((_tmp110C->v=c1,_tmp110C))));}return 1;_LL4E6: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp7C2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7BB.f1;if(_tmp7C2->tag != 1)goto _LL4E8;else{_tmp7C3=(struct Cyc_Core_Opt**)& _tmp7C2->f1;}}_LL4E7:
{struct Cyc_Core_Opt*_tmp110D;*_tmp7C3=((_tmp110D=_cycalloc(sizeof(*_tmp110D)),((_tmp110D->v=c2,_tmp110D))));}return 1;_LL4E8:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7C4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BB.f1;if(_tmp7C4->tag != 2)goto _LL4EA;else{_tmp7C5=(struct Cyc_Core_Opt**)& _tmp7C4->f1;_tmp7C6=_tmp7C4->f2;}}{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7C7=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BB.f2;if(_tmp7C7->tag != 0)goto _LL4EA;else{_tmp7C8=_tmp7C7->f1;}};_LL4E9:
# 2990
 if(Cyc_Tcutil_kind_leq(_tmp7C8,_tmp7C6)){
{struct Cyc_Core_Opt*_tmp110E;*_tmp7C5=((_tmp110E=_cycalloc(sizeof(*_tmp110E)),((_tmp110E->v=c2,_tmp110E))));}return 1;}else{
return 0;}_LL4EA:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp7C9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7BB.f1;if(_tmp7C9->tag != 0)goto _LL4EC;else{_tmp7CA=_tmp7C9->f1;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7CB=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BB.f2;if(_tmp7CB->tag != 2)goto _LL4EC;else{_tmp7CC=(struct Cyc_Core_Opt**)& _tmp7CB->f1;_tmp7CD=_tmp7CB->f2;}};_LL4EB:
# 2994
 if(Cyc_Tcutil_kind_leq(_tmp7CA,_tmp7CD)){
{struct Cyc_Core_Opt*_tmp110F;*_tmp7CC=((_tmp110F=_cycalloc(sizeof(*_tmp110F)),((_tmp110F->v=c1,_tmp110F))));}return 1;}else{
return 0;}_LL4EC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7CE=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BB.f1;if(_tmp7CE->tag != 2)goto _LL4E1;else{_tmp7CF=(struct Cyc_Core_Opt**)& _tmp7CE->f1;_tmp7D0=_tmp7CE->f2;}}{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp7D1=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7BB.f2;if(_tmp7D1->tag != 2)goto _LL4E1;else{_tmp7D2=(struct Cyc_Core_Opt**)& _tmp7D1->f1;_tmp7D3=_tmp7D1->f2;}};_LL4ED:
# 2998
 if(Cyc_Tcutil_kind_leq(_tmp7D0,_tmp7D3)){
{struct Cyc_Core_Opt*_tmp1110;*_tmp7D2=((_tmp1110=_cycalloc(sizeof(*_tmp1110)),((_tmp1110->v=c1,_tmp1110))));}return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp7D3,_tmp7D0)){
{struct Cyc_Core_Opt*_tmp1111;*_tmp7CF=((_tmp1111=_cycalloc(sizeof(*_tmp1111)),((_tmp1111->v=c2,_tmp1111))));}return 1;}else{
return 0;}}_LL4E1:;};}
# 3007
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3012
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
const char*_tmp1115;void*_tmp1114[1];struct Cyc_Int_pa_PrintArg_struct _tmp1113;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmp1113.tag=1,((_tmp1113.f1=(unsigned long)i,((_tmp1114[0]=& _tmp1113,Cyc_aprintf(((_tmp1115="#%d",_tag_dyneither(_tmp1115,sizeof(char),4))),_tag_dyneither(_tmp1114,sizeof(void*),1))))))));
struct _dyneither_ptr*_tmp1118;struct Cyc_Absyn_Tvar*_tmp1117;return(_tmp1117=_cycalloc(sizeof(*_tmp1117)),((_tmp1117->name=((_tmp1118=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1118[0]=(struct _dyneither_ptr)s,_tmp1118)))),((_tmp1117->identity=- 1,((_tmp1117->kind=k,_tmp1117)))))));}
# 3019
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp7E0=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp7E0,sizeof(char),0))== '#';}
# 3024
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
{const char*_tmp111C;void*_tmp111B[1];struct Cyc_String_pa_PrintArg_struct _tmp111A;(_tmp111A.tag=0,((_tmp111A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name),((_tmp111B[0]=& _tmp111A,Cyc_printf(((_tmp111C="%s",_tag_dyneither(_tmp111C,sizeof(char),3))),_tag_dyneither(_tmp111B,sizeof(void*),1)))))));}
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
const char*_tmp111D;struct _dyneither_ptr _tmp7E4=Cyc_strconcat(((_tmp111D="`",_tag_dyneither(_tmp111D,sizeof(char),2))),(struct _dyneither_ptr)*t->name);
{char _tmp1120;char _tmp111F;struct _dyneither_ptr _tmp111E;(_tmp111E=_dyneither_ptr_plus(_tmp7E4,sizeof(char),1),((_tmp111F=*((char*)_check_dyneither_subscript(_tmp111E,sizeof(char),0)),((_tmp1120='t',((_get_dyneither_size(_tmp111E,sizeof(char))== 1  && (_tmp111F == '\000'  && _tmp1120 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp111E.curr)=_tmp1120)))))));}{
struct _dyneither_ptr*_tmp1121;t->name=((_tmp1121=_cycalloc(sizeof(struct _dyneither_ptr)* 1),((_tmp1121[0]=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp7E4),_tmp1121))));};};}
# 3033
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3035
struct _tuple10*_tmp1122;return(_tmp1122=_cycalloc(sizeof(*_tmp1122)),((_tmp1122->f1=(*x).f1,((_tmp1122->f2=(*x).f2,((_tmp1122->f3=(*x).f3,_tmp1122)))))));}
# 3038
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3045
struct Cyc_List_List*_tmp7EB=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
struct Cyc_List_List*_tmp1123;_tmp7EB=((_tmp1123=_cycalloc(sizeof(*_tmp1123)),((_tmp1123->hd=(void*)atts->hd,((_tmp1123->tl=_tmp7EB,_tmp1123))))));}}}{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1129;struct Cyc_Absyn_FnInfo _tmp1128;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1127;return(void*)((_tmp1127=_cycalloc(sizeof(*_tmp1127)),((_tmp1127[0]=((_tmp1129.tag=9,((_tmp1129.f1=((_tmp1128.tvars=fd->tvs,((_tmp1128.effect=fd->effect,((_tmp1128.ret_tqual=fd->ret_tqual,((_tmp1128.ret_typ=fd->ret_type,((_tmp1128.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args),((_tmp1128.c_varargs=fd->c_varargs,((_tmp1128.cyc_varargs=fd->cyc_varargs,((_tmp1128.rgn_po=fd->rgn_po,((_tmp1128.attributes=_tmp7EB,_tmp1128)))))))))))))))))),_tmp1129)))),_tmp1127))));};}
# 3054
return(void*)_check_null(fd->cached_typ);}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3058
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct Cyc_Absyn_Tqual _tmp7F1;void*_tmp7F2;struct _tuple12*_tmp7F0=pr;_tmp7F1=_tmp7F0->f1;_tmp7F2=_tmp7F0->f2;
if(_tmp7F2 == t)return pr;else{
struct _tuple12*_tmp112A;return(_tmp112A=_cycalloc(sizeof(*_tmp112A)),((_tmp112A->f1=_tmp7F1,((_tmp112A->f2=t,_tmp112A)))));}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3065
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3067
struct _tuple26*_tmp112D;struct _tuple27*_tmp112C;return(_tmp112C=_region_malloc(rgn,sizeof(*_tmp112C)),((_tmp112C->f1=((_tmp112D=_region_malloc(rgn,sizeof(*_tmp112D)),((_tmp112D->f1=(*y).f1,((_tmp112D->f2=(*y).f2,_tmp112D)))))),((_tmp112C->f2=(*y).f3,_tmp112C)))));}
# 3069
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3071
struct _tuple10 _tmp7F6=*orig_arg;struct _dyneither_ptr*_tmp7F8;struct Cyc_Absyn_Tqual _tmp7F9;void*_tmp7FA;struct _tuple10 _tmp7F7=_tmp7F6;_tmp7F8=_tmp7F7.f1;_tmp7F9=_tmp7F7.f2;_tmp7FA=_tmp7F7.f3;
if(t == _tmp7FA)return orig_arg;{
struct _tuple10*_tmp112E;return(_tmp112E=_cycalloc(sizeof(*_tmp112E)),((_tmp112E->f1=_tmp7F8,((_tmp112E->f2=_tmp7F9,((_tmp112E->f3=t,_tmp112E)))))));};}
# 3075
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3078
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3083
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3085
struct Cyc_Absyn_Exp*_tmp112F;return(_tmp112F=_cycalloc(sizeof(*_tmp112F)),((_tmp112F->topt=old->topt,((_tmp112F->r=r,((_tmp112F->loc=old->loc,((_tmp112F->annot=old->annot,_tmp112F)))))))));}
# 3090
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp7FD=e->r;void*_tmp7FE=_tmp7FD;enum Cyc_Absyn_Primop _tmp804;struct Cyc_List_List*_tmp805;struct Cyc_Absyn_Exp*_tmp807;struct Cyc_Absyn_Exp*_tmp808;struct Cyc_Absyn_Exp*_tmp809;struct Cyc_Absyn_Exp*_tmp80B;struct Cyc_Absyn_Exp*_tmp80C;struct Cyc_Absyn_Exp*_tmp80E;struct Cyc_Absyn_Exp*_tmp80F;struct Cyc_Absyn_Exp*_tmp811;struct Cyc_Absyn_Exp*_tmp812;void*_tmp814;struct Cyc_Absyn_Exp*_tmp815;int _tmp816;enum Cyc_Absyn_Coercion _tmp817;void*_tmp819;struct Cyc_Absyn_Exp*_tmp81B;void*_tmp81D;struct Cyc_List_List*_tmp81E;void*_tmp820;_LL4EF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp7FF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp7FF->tag != 0)goto _LL4F1;}_LL4F0:
 goto _LL4F2;_LL4F1: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp800=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp800->tag != 31)goto _LL4F3;}_LL4F2:
 goto _LL4F4;_LL4F3: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp801=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp801->tag != 32)goto _LL4F5;}_LL4F4:
 goto _LL4F6;_LL4F5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp802=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp802->tag != 1)goto _LL4F7;}_LL4F6:
 return e;_LL4F7: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp803=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp803->tag != 2)goto _LL4F9;else{_tmp804=_tmp803->f1;_tmp805=_tmp803->f2;}}_LL4F8:
# 3098
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp805)== 1){
struct Cyc_Absyn_Exp*_tmp821=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp805))->hd;
struct Cyc_Absyn_Exp*_tmp822=Cyc_Tcutil_rsubsexp(r,inst,_tmp821);
if(_tmp822 == _tmp821)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp1135;struct Cyc_Absyn_Exp*_tmp1134[1];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1133;return Cyc_Tcutil_copye(e,(void*)((_tmp1133=_cycalloc(sizeof(*_tmp1133)),((_tmp1133[0]=((_tmp1135.tag=2,((_tmp1135.f1=_tmp804,((_tmp1135.f2=((_tmp1134[0]=_tmp822,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1134,sizeof(struct Cyc_Absyn_Exp*),1)))),_tmp1135)))))),_tmp1133)))));};}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp805)== 2){
struct Cyc_Absyn_Exp*_tmp826=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp805))->hd;
struct Cyc_Absyn_Exp*_tmp827=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp805->tl))->hd;
struct Cyc_Absyn_Exp*_tmp828=Cyc_Tcutil_rsubsexp(r,inst,_tmp826);
struct Cyc_Absyn_Exp*_tmp829=Cyc_Tcutil_rsubsexp(r,inst,_tmp827);
if(_tmp828 == _tmp826  && _tmp829 == _tmp827)return e;{
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp113B;struct Cyc_Absyn_Exp*_tmp113A[2];struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp1139;return Cyc_Tcutil_copye(e,(void*)((_tmp1139=_cycalloc(sizeof(*_tmp1139)),((_tmp1139[0]=((_tmp113B.tag=2,((_tmp113B.f1=_tmp804,((_tmp113B.f2=((_tmp113A[1]=_tmp829,((_tmp113A[0]=_tmp828,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp113A,sizeof(struct Cyc_Absyn_Exp*),2)))))),_tmp113B)))))),_tmp1139)))));};}else{
const char*_tmp113E;void*_tmp113D;return(_tmp113D=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp113E="primop does not have 1 or 2 args!",_tag_dyneither(_tmp113E,sizeof(char),34))),_tag_dyneither(_tmp113D,sizeof(void*),0)));}}_LL4F9: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp806=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp806->tag != 5)goto _LL4FB;else{_tmp807=_tmp806->f1;_tmp808=_tmp806->f2;_tmp809=_tmp806->f3;}}_LL4FA: {
# 3112
struct Cyc_Absyn_Exp*_tmp82F=Cyc_Tcutil_rsubsexp(r,inst,_tmp807);
struct Cyc_Absyn_Exp*_tmp830=Cyc_Tcutil_rsubsexp(r,inst,_tmp808);
struct Cyc_Absyn_Exp*_tmp831=Cyc_Tcutil_rsubsexp(r,inst,_tmp809);
if((_tmp82F == _tmp807  && _tmp830 == _tmp808) && _tmp831 == _tmp809)return e;{
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp1141;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp1140;return Cyc_Tcutil_copye(e,(void*)((_tmp1140=_cycalloc(sizeof(*_tmp1140)),((_tmp1140[0]=((_tmp1141.tag=5,((_tmp1141.f1=_tmp82F,((_tmp1141.f2=_tmp830,((_tmp1141.f3=_tmp831,_tmp1141)))))))),_tmp1140)))));};}_LL4FB: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp80A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp80A->tag != 6)goto _LL4FD;else{_tmp80B=_tmp80A->f1;_tmp80C=_tmp80A->f2;}}_LL4FC: {
# 3118
struct Cyc_Absyn_Exp*_tmp834=Cyc_Tcutil_rsubsexp(r,inst,_tmp80B);
struct Cyc_Absyn_Exp*_tmp835=Cyc_Tcutil_rsubsexp(r,inst,_tmp80C);
if(_tmp834 == _tmp80B  && _tmp835 == _tmp80C)return e;{
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp1144;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp1143;return Cyc_Tcutil_copye(e,(void*)((_tmp1143=_cycalloc(sizeof(*_tmp1143)),((_tmp1143[0]=((_tmp1144.tag=6,((_tmp1144.f1=_tmp834,((_tmp1144.f2=_tmp835,_tmp1144)))))),_tmp1143)))));};}_LL4FD: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp80D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp80D->tag != 7)goto _LL4FF;else{_tmp80E=_tmp80D->f1;_tmp80F=_tmp80D->f2;}}_LL4FE: {
# 3123
struct Cyc_Absyn_Exp*_tmp838=Cyc_Tcutil_rsubsexp(r,inst,_tmp80E);
struct Cyc_Absyn_Exp*_tmp839=Cyc_Tcutil_rsubsexp(r,inst,_tmp80F);
if(_tmp838 == _tmp80E  && _tmp839 == _tmp80F)return e;{
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp1147;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp1146;return Cyc_Tcutil_copye(e,(void*)((_tmp1146=_cycalloc(sizeof(*_tmp1146)),((_tmp1146[0]=((_tmp1147.tag=7,((_tmp1147.f1=_tmp838,((_tmp1147.f2=_tmp839,_tmp1147)))))),_tmp1146)))));};}_LL4FF: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp810=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp810->tag != 8)goto _LL501;else{_tmp811=_tmp810->f1;_tmp812=_tmp810->f2;}}_LL500: {
# 3128
struct Cyc_Absyn_Exp*_tmp83C=Cyc_Tcutil_rsubsexp(r,inst,_tmp811);
struct Cyc_Absyn_Exp*_tmp83D=Cyc_Tcutil_rsubsexp(r,inst,_tmp812);
if(_tmp83C == _tmp811  && _tmp83D == _tmp812)return e;{
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp114A;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp1149;return Cyc_Tcutil_copye(e,(void*)((_tmp1149=_cycalloc(sizeof(*_tmp1149)),((_tmp1149[0]=((_tmp114A.tag=8,((_tmp114A.f1=_tmp83C,((_tmp114A.f2=_tmp83D,_tmp114A)))))),_tmp1149)))));};}_LL501: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp813=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp813->tag != 13)goto _LL503;else{_tmp814=(void*)_tmp813->f1;_tmp815=_tmp813->f2;_tmp816=_tmp813->f3;_tmp817=_tmp813->f4;}}_LL502: {
# 3133
struct Cyc_Absyn_Exp*_tmp840=Cyc_Tcutil_rsubsexp(r,inst,_tmp815);
void*_tmp841=Cyc_Tcutil_rsubstitute(r,inst,_tmp814);
if(_tmp840 == _tmp815  && _tmp841 == _tmp814)return e;{
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp114D;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp114C;return Cyc_Tcutil_copye(e,(void*)((_tmp114C=_cycalloc(sizeof(*_tmp114C)),((_tmp114C[0]=((_tmp114D.tag=13,((_tmp114D.f1=_tmp841,((_tmp114D.f2=_tmp840,((_tmp114D.f3=_tmp816,((_tmp114D.f4=_tmp817,_tmp114D)))))))))),_tmp114C)))));};}_LL503: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp818=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp818->tag != 16)goto _LL505;else{_tmp819=(void*)_tmp818->f1;}}_LL504: {
# 3138
void*_tmp844=Cyc_Tcutil_rsubstitute(r,inst,_tmp819);
if(_tmp844 == _tmp819)return e;{
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp1150;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp114F;return Cyc_Tcutil_copye(e,(void*)((_tmp114F=_cycalloc(sizeof(*_tmp114F)),((_tmp114F[0]=((_tmp1150.tag=16,((_tmp1150.f1=_tmp844,_tmp1150)))),_tmp114F)))));};}_LL505: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp81A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp81A->tag != 17)goto _LL507;else{_tmp81B=_tmp81A->f1;}}_LL506: {
# 3142
struct Cyc_Absyn_Exp*_tmp847=Cyc_Tcutil_rsubsexp(r,inst,_tmp81B);
if(_tmp847 == _tmp81B)return e;{
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp1153;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp1152;return Cyc_Tcutil_copye(e,(void*)((_tmp1152=_cycalloc(sizeof(*_tmp1152)),((_tmp1152[0]=((_tmp1153.tag=17,((_tmp1153.f1=_tmp847,_tmp1153)))),_tmp1152)))));};}_LL507: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp81C=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp81C->tag != 18)goto _LL509;else{_tmp81D=(void*)_tmp81C->f1;_tmp81E=_tmp81C->f2;}}_LL508: {
# 3146
void*_tmp84A=Cyc_Tcutil_rsubstitute(r,inst,_tmp81D);
if(_tmp84A == _tmp81D)return e;{
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp1156;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp1155;return Cyc_Tcutil_copye(e,(void*)((_tmp1155=_cycalloc(sizeof(*_tmp1155)),((_tmp1155[0]=((_tmp1156.tag=18,((_tmp1156.f1=_tmp84A,((_tmp1156.f2=_tmp81E,_tmp1156)))))),_tmp1155)))));};}_LL509: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp81F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7FE;if(_tmp81F->tag != 38)goto _LL50B;else{_tmp820=(void*)_tmp81F->f1;}}_LL50A: {
# 3150
void*_tmp84D=Cyc_Tcutil_rsubstitute(r,inst,_tmp820);
if(_tmp84D == _tmp820)return e;{
# 3153
void*_tmp84E=Cyc_Tcutil_compress(_tmp84D);void*_tmp84F=_tmp84E;struct Cyc_Absyn_Exp*_tmp851;_LL50E: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp850=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp84F;if(_tmp850->tag != 18)goto _LL510;else{_tmp851=_tmp850->f1;}}_LL50F:
 return _tmp851;_LL510:;_LL511: {
# 3156
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp1159;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp1158;return Cyc_Tcutil_copye(e,(void*)((_tmp1158=_cycalloc(sizeof(*_tmp1158)),((_tmp1158[0]=((_tmp1159.tag=38,((_tmp1159.f1=_tmp84D,_tmp1159)))),_tmp1158)))));}_LL50D:;};}_LL50B:;_LL50C: {
# 3159
const char*_tmp115C;void*_tmp115B;return(_tmp115B=0,((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp115C="non-type-level-expression in Tcutil::rsubsexp",_tag_dyneither(_tmp115C,sizeof(char),46))),_tag_dyneither(_tmp115B,sizeof(void*),0)));}_LL4EE:;}
# 3163
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3166
void*_tmp856=f->type;
struct Cyc_Absyn_Exp*_tmp857=f->requires_clause;
void*_tmp858=Cyc_Tcutil_rsubstitute(r,inst,_tmp856);
struct Cyc_Absyn_Exp*_tmp859=_tmp857 == 0?0: Cyc_Tcutil_rsubsexp(r,inst,_tmp857);
if(_tmp856 == _tmp858  && _tmp857 == _tmp859)return f;else{
struct Cyc_Absyn_Aggrfield*_tmp115D;return(_tmp115D=_cycalloc(sizeof(*_tmp115D)),((_tmp115D->name=f->name,((_tmp115D->tq=f->tq,((_tmp115D->type=_tmp858,((_tmp115D->width=f->width,((_tmp115D->attributes=f->attributes,((_tmp115D->requires_clause=_tmp859,_tmp115D)))))))))))));}}
# 3176
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3179
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp85B=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp85C=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp85D=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp85B);
struct Cyc_List_List*_tmp85E=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp85C);
if(_tmp85D == _tmp85B  && _tmp85E == _tmp85C)return fs;{
# 3186
struct Cyc_List_List*_tmp115E;return(_tmp115E=_cycalloc(sizeof(*_tmp115E)),((_tmp115E->hd=_tmp85D,((_tmp115E->tl=_tmp85E,_tmp115E)))));};};}
# 3189
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3192
struct _tuple1 _tmp860=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct Cyc_List_List*_tmp862;struct Cyc_List_List*_tmp863;struct _tuple1 _tmp861=_tmp860;_tmp862=_tmp861.f1;_tmp863=_tmp861.f2;{
struct Cyc_List_List*_tmp864=Cyc_Tcutil_substs(rgn,inst,_tmp862);
struct Cyc_List_List*_tmp865=Cyc_Tcutil_substs(rgn,inst,_tmp863);
if(_tmp864 == _tmp862  && _tmp865 == _tmp863)
return rgn_po;else{
# 3198
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp864,_tmp865);}};}
# 3201
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3204
void*_tmp866=Cyc_Tcutil_compress(t);void*_tmp867=_tmp866;struct Cyc_Absyn_Tvar*_tmp869;union Cyc_Absyn_AggrInfoU _tmp86B;struct Cyc_List_List*_tmp86C;union Cyc_Absyn_DatatypeInfoU _tmp86E;struct Cyc_List_List*_tmp86F;union Cyc_Absyn_DatatypeFieldInfoU _tmp871;struct Cyc_List_List*_tmp872;struct _tuple2*_tmp874;struct Cyc_List_List*_tmp875;struct Cyc_Absyn_Typedefdecl*_tmp876;void*_tmp877;void*_tmp879;struct Cyc_Absyn_Tqual _tmp87A;struct Cyc_Absyn_Exp*_tmp87B;union Cyc_Absyn_Constraint*_tmp87C;unsigned int _tmp87D;void*_tmp87F;struct Cyc_Absyn_Tqual _tmp880;void*_tmp881;union Cyc_Absyn_Constraint*_tmp882;union Cyc_Absyn_Constraint*_tmp883;union Cyc_Absyn_Constraint*_tmp884;struct Cyc_List_List*_tmp886;void*_tmp887;struct Cyc_Absyn_Tqual _tmp888;void*_tmp889;struct Cyc_List_List*_tmp88A;int _tmp88B;struct Cyc_Absyn_VarargInfo*_tmp88C;struct Cyc_List_List*_tmp88D;struct Cyc_List_List*_tmp88E;struct Cyc_List_List*_tmp890;enum Cyc_Absyn_AggrKind _tmp892;struct Cyc_List_List*_tmp893;void*_tmp895;void*_tmp897;void*_tmp899;void*_tmp89A;void*_tmp89C;struct Cyc_Absyn_Exp*_tmp89E;void*_tmp8A8;void*_tmp8AA;struct Cyc_List_List*_tmp8AC;_LL513: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp868=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp867;if(_tmp868->tag != 2)goto _LL515;else{_tmp869=_tmp868->f1;}}_LL514: {
# 3207
struct _handler_cons _tmp8AE;_push_handler(& _tmp8AE);{int _tmp8B0=0;if(setjmp(_tmp8AE.handler))_tmp8B0=1;if(!_tmp8B0){{void*_tmp8B1=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp869);_npop_handler(0);return _tmp8B1;};_pop_handler();}else{void*_tmp8AF=(void*)_exn_thrown;void*_tmp8B3=_tmp8AF;void*_tmp8B5;_LL54A: {struct Cyc_Core_Not_found_exn_struct*_tmp8B4=(struct Cyc_Core_Not_found_exn_struct*)_tmp8B3;if(_tmp8B4->tag != Cyc_Core_Not_found)goto _LL54C;}_LL54B:
 return t;_LL54C: _tmp8B5=_tmp8B3;_LL54D:(void)_throw(_tmp8B5);_LL549:;}};}_LL515: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp86A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp867;if(_tmp86A->tag != 11)goto _LL517;else{_tmp86B=(_tmp86A->f1).aggr_info;_tmp86C=(_tmp86A->f1).targs;}}_LL516: {
# 3210
struct Cyc_List_List*_tmp8B6=Cyc_Tcutil_substs(rgn,inst,_tmp86C);
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1164;struct Cyc_Absyn_AggrInfo _tmp1163;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1162;return _tmp8B6 == _tmp86C?t:(void*)((_tmp1162=_cycalloc(sizeof(*_tmp1162)),((_tmp1162[0]=((_tmp1164.tag=11,((_tmp1164.f1=((_tmp1163.aggr_info=_tmp86B,((_tmp1163.targs=_tmp8B6,_tmp1163)))),_tmp1164)))),_tmp1162))));}_LL517: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp86D=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867;if(_tmp86D->tag != 3)goto _LL519;else{_tmp86E=(_tmp86D->f1).datatype_info;_tmp86F=(_tmp86D->f1).targs;}}_LL518: {
# 3213
struct Cyc_List_List*_tmp8BA=Cyc_Tcutil_substs(rgn,inst,_tmp86F);
struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp116A;struct Cyc_Absyn_DatatypeInfo _tmp1169;struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1168;return _tmp8BA == _tmp86F?t:(void*)(
(_tmp1168=_cycalloc(sizeof(*_tmp1168)),((_tmp1168[0]=((_tmp116A.tag=3,((_tmp116A.f1=((_tmp1169.datatype_info=_tmp86E,((_tmp1169.targs=_tmp8BA,_tmp1169)))),_tmp116A)))),_tmp1168))));}_LL519: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp870=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp867;if(_tmp870->tag != 4)goto _LL51B;else{_tmp871=(_tmp870->f1).field_info;_tmp872=(_tmp870->f1).targs;}}_LL51A: {
# 3217
struct Cyc_List_List*_tmp8BE=Cyc_Tcutil_substs(rgn,inst,_tmp872);
struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp1170;struct Cyc_Absyn_DatatypeFieldInfo _tmp116F;struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp116E;return _tmp8BE == _tmp872?t:(void*)(
(_tmp116E=_cycalloc(sizeof(*_tmp116E)),((_tmp116E[0]=((_tmp1170.tag=4,((_tmp1170.f1=((_tmp116F.field_info=_tmp871,((_tmp116F.targs=_tmp8BE,_tmp116F)))),_tmp1170)))),_tmp116E))));}_LL51B: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp873=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp867;if(_tmp873->tag != 17)goto _LL51D;else{_tmp874=_tmp873->f1;_tmp875=_tmp873->f2;_tmp876=_tmp873->f3;_tmp877=(void*)_tmp873->f4;}}_LL51C: {
# 3221
struct Cyc_List_List*_tmp8C2=Cyc_Tcutil_substs(rgn,inst,_tmp875);
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp1173;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1172;return _tmp8C2 == _tmp875?t:(void*)((_tmp1172=_cycalloc(sizeof(*_tmp1172)),((_tmp1172[0]=((_tmp1173.tag=17,((_tmp1173.f1=_tmp874,((_tmp1173.f2=_tmp8C2,((_tmp1173.f3=_tmp876,((_tmp1173.f4=_tmp877,_tmp1173)))))))))),_tmp1172))));}_LL51D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp878=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp867;if(_tmp878->tag != 8)goto _LL51F;else{_tmp879=(_tmp878->f1).elt_type;_tmp87A=(_tmp878->f1).tq;_tmp87B=(_tmp878->f1).num_elts;_tmp87C=(_tmp878->f1).zero_term;_tmp87D=(_tmp878->f1).zt_loc;}}_LL51E: {
# 3224
void*_tmp8C5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp879);
struct Cyc_Absyn_Exp*_tmp8C6=_tmp87B == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp87B);
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp1179;struct Cyc_Absyn_ArrayInfo _tmp1178;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1177;return _tmp8C5 == _tmp879  && _tmp8C6 == _tmp87B?t:(void*)(
(_tmp1177=_cycalloc(sizeof(*_tmp1177)),((_tmp1177[0]=((_tmp1179.tag=8,((_tmp1179.f1=((_tmp1178.elt_type=_tmp8C5,((_tmp1178.tq=_tmp87A,((_tmp1178.num_elts=_tmp8C6,((_tmp1178.zero_term=_tmp87C,((_tmp1178.zt_loc=_tmp87D,_tmp1178)))))))))),_tmp1179)))),_tmp1177))));}_LL51F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp867;if(_tmp87E->tag != 5)goto _LL521;else{_tmp87F=(_tmp87E->f1).elt_typ;_tmp880=(_tmp87E->f1).elt_tq;_tmp881=((_tmp87E->f1).ptr_atts).rgn;_tmp882=((_tmp87E->f1).ptr_atts).nullable;_tmp883=((_tmp87E->f1).ptr_atts).bounds;_tmp884=((_tmp87E->f1).ptr_atts).zero_term;}}_LL520: {
# 3229
void*_tmp8CA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp87F);
void*_tmp8CB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp881);
union Cyc_Absyn_Constraint*_tmp8CC=_tmp883;
{void*_tmp8CD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp883);void*_tmp8CE=_tmp8CD;struct Cyc_Absyn_Exp*_tmp8D0;_LL54F: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp8CF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8CE;if(_tmp8CF->tag != 1)goto _LL551;else{_tmp8D0=_tmp8CF->f1;}}_LL550: {
# 3234
struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp8D0);
if(_tmp8D1 != _tmp8D0){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp117C;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp117B;_tmp8CC=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)((_tmp117B=_cycalloc(sizeof(*_tmp117B)),((_tmp117B[0]=((_tmp117C.tag=1,((_tmp117C.f1=_tmp8D1,_tmp117C)))),_tmp117B)))));}
goto _LL54E;}_LL551:;_LL552:
 goto _LL54E;_LL54E:;}
# 3240
if((_tmp8CA == _tmp87F  && _tmp8CB == _tmp881) && _tmp8CC == _tmp883)
return t;{
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1182;struct Cyc_Absyn_PtrInfo _tmp1181;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1180;return(void*)((_tmp1180=_cycalloc(sizeof(*_tmp1180)),((_tmp1180[0]=((_tmp1182.tag=5,((_tmp1182.f1=((_tmp1181.elt_typ=_tmp8CA,((_tmp1181.elt_tq=_tmp880,((_tmp1181.ptr_atts=(((_tmp1181.ptr_atts).rgn=_tmp8CB,(((_tmp1181.ptr_atts).nullable=_tmp882,(((_tmp1181.ptr_atts).bounds=_tmp8CC,(((_tmp1181.ptr_atts).zero_term=_tmp884,(((_tmp1181.ptr_atts).ptrloc=0,_tmp1181.ptr_atts)))))))))),_tmp1181)))))),_tmp1182)))),_tmp1180))));};}_LL521: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp885=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp867;if(_tmp885->tag != 9)goto _LL523;else{_tmp886=(_tmp885->f1).tvars;_tmp887=(_tmp885->f1).effect;_tmp888=(_tmp885->f1).ret_tqual;_tmp889=(_tmp885->f1).ret_typ;_tmp88A=(_tmp885->f1).args;_tmp88B=(_tmp885->f1).c_varargs;_tmp88C=(_tmp885->f1).cyc_varargs;_tmp88D=(_tmp885->f1).rgn_po;_tmp88E=(_tmp885->f1).attributes;}}_LL522:
# 3245
{struct Cyc_List_List*_tmp8D7=_tmp886;for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){
struct _tuple16*_tmp118C;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp118B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp118A;struct Cyc_List_List*_tmp1189;inst=((_tmp1189=_region_malloc(rgn,sizeof(*_tmp1189)),((_tmp1189->hd=((_tmp118C=_region_malloc(rgn,sizeof(*_tmp118C)),((_tmp118C->f1=(struct Cyc_Absyn_Tvar*)_tmp8D7->hd,((_tmp118C->f2=(void*)((_tmp118A=_cycalloc(sizeof(*_tmp118A)),((_tmp118A[0]=((_tmp118B.tag=2,((_tmp118B.f1=(struct Cyc_Absyn_Tvar*)_tmp8D7->hd,_tmp118B)))),_tmp118A)))),_tmp118C)))))),((_tmp1189->tl=inst,_tmp1189))))));}}{
struct _tuple1 _tmp8DC=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp88A));
# 3247
struct Cyc_List_List*_tmp8DE;struct Cyc_List_List*_tmp8DF;struct _tuple1 _tmp8DD=_tmp8DC;_tmp8DE=_tmp8DD.f1;_tmp8DF=_tmp8DD.f2;{
# 3249
struct Cyc_List_List*_tmp8E0=_tmp88A;
struct Cyc_List_List*_tmp8E1=Cyc_Tcutil_substs(rgn,inst,_tmp8DF);
if(_tmp8E1 != _tmp8DF)
_tmp8E0=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp88A,_tmp8E1);{
void*eff2;
if(_tmp887 == 0)
eff2=0;else{
# 3257
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp887);
if(new_eff == _tmp887)
eff2=_tmp887;else{
# 3261
eff2=new_eff;}}{
# 3263
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp88C == 0)
cyc_varargs2=0;else{
# 3267
struct Cyc_Absyn_VarargInfo _tmp8E2=*_tmp88C;struct _dyneither_ptr*_tmp8E4;struct Cyc_Absyn_Tqual _tmp8E5;void*_tmp8E6;int _tmp8E7;struct Cyc_Absyn_VarargInfo _tmp8E3=_tmp8E2;_tmp8E4=_tmp8E3.name;_tmp8E5=_tmp8E3.tq;_tmp8E6=_tmp8E3.type;_tmp8E7=_tmp8E3.inject;{
void*_tmp8E8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8E6);
if(_tmp8E8 == _tmp8E6)cyc_varargs2=_tmp88C;else{
# 3271
struct Cyc_Absyn_VarargInfo*_tmp118D;cyc_varargs2=((_tmp118D=_cycalloc(sizeof(*_tmp118D)),((_tmp118D->name=_tmp8E4,((_tmp118D->tq=_tmp8E5,((_tmp118D->type=_tmp8E8,((_tmp118D->inject=_tmp8E7,_tmp118D))))))))));}};}{
# 3273
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp88D);
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1193;struct Cyc_Absyn_FnInfo _tmp1192;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1191;return(void*)(
(_tmp1191=_cycalloc(sizeof(*_tmp1191)),((_tmp1191[0]=((_tmp1193.tag=9,((_tmp1193.f1=((_tmp1192.tvars=_tmp886,((_tmp1192.effect=eff2,((_tmp1192.ret_tqual=_tmp888,((_tmp1192.ret_typ=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp889),((_tmp1192.args=_tmp8E0,((_tmp1192.c_varargs=_tmp88B,((_tmp1192.cyc_varargs=cyc_varargs2,((_tmp1192.rgn_po=rgn_po2,((_tmp1192.attributes=_tmp88E,_tmp1192)))))))))))))))))),_tmp1193)))),_tmp1191))));};};};};};_LL523: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp88F=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp867;if(_tmp88F->tag != 10)goto _LL525;else{_tmp890=_tmp88F->f1;}}_LL524: {
# 3278
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp8ED=_tmp890;for(0;_tmp8ED != 0;_tmp8ED=_tmp8ED->tl){
void*_tmp8EE=(*((struct _tuple12*)_tmp8ED->hd)).f2;
void*_tmp8EF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8EE);
if(_tmp8EE != _tmp8EF)
change=1;{
# 3286
struct Cyc_List_List*_tmp1194;ts2=((_tmp1194=_region_malloc(rgn,sizeof(*_tmp1194)),((_tmp1194->hd=_tmp8EF,((_tmp1194->tl=ts2,_tmp1194))))));};}}
# 3288
if(!change)
return t;{
struct Cyc_List_List*_tmp8F1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp890,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp1197;struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1196;return(void*)((_tmp1196=_cycalloc(sizeof(*_tmp1196)),((_tmp1196[0]=((_tmp1197.tag=10,((_tmp1197.f1=_tmp8F1,_tmp1197)))),_tmp1196))));};}_LL525: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp891=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp867;if(_tmp891->tag != 12)goto _LL527;else{_tmp892=_tmp891->f1;_tmp893=_tmp891->f2;}}_LL526: {
# 3293
struct Cyc_List_List*_tmp8F4=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp893);
if(_tmp893 == _tmp8F4)return t;{
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp119A;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1199;return(void*)((_tmp1199=_cycalloc(sizeof(*_tmp1199)),((_tmp1199[0]=((_tmp119A.tag=12,((_tmp119A.f1=_tmp892,((_tmp119A.f2=_tmp8F4,_tmp119A)))))),_tmp1199))));};}_LL527: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp894=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp867;if(_tmp894->tag != 1)goto _LL529;else{_tmp895=(void*)_tmp894->f2;}}_LL528:
# 3297
 if(_tmp895 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp895);else{
return t;}_LL529: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp896=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp867;if(_tmp896->tag != 15)goto _LL52B;else{_tmp897=(void*)_tmp896->f1;}}_LL52A: {
# 3300
void*_tmp8F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp897);
struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp119D;struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp119C;return _tmp8F7 == _tmp897?t:(void*)((_tmp119C=_cycalloc(sizeof(*_tmp119C)),((_tmp119C[0]=((_tmp119D.tag=15,((_tmp119D.f1=_tmp8F7,_tmp119D)))),_tmp119C))));}_LL52B: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp898=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp867;if(_tmp898->tag != 16)goto _LL52D;else{_tmp899=(void*)_tmp898->f1;_tmp89A=(void*)_tmp898->f2;}}_LL52C: {
# 3303
void*_tmp8FA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp899);
void*_tmp8FB=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp89A);
struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp11A0;struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp119F;return _tmp8FA == _tmp899  && _tmp8FB == _tmp89A?t:(void*)((_tmp119F=_cycalloc(sizeof(*_tmp119F)),((_tmp119F[0]=((_tmp11A0.tag=16,((_tmp11A0.f1=_tmp8FA,((_tmp11A0.f2=_tmp8FB,_tmp11A0)))))),_tmp119F))));}_LL52D: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp89B=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp867;if(_tmp89B->tag != 19)goto _LL52F;else{_tmp89C=(void*)_tmp89B->f1;}}_LL52E: {
# 3307
void*_tmp8FE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp89C);
struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp11A3;struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp11A2;return _tmp8FE == _tmp89C?t:(void*)((_tmp11A2=_cycalloc(sizeof(*_tmp11A2)),((_tmp11A2[0]=((_tmp11A3.tag=19,((_tmp11A3.f1=_tmp8FE,_tmp11A3)))),_tmp11A2))));}_LL52F: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp89D=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp867;if(_tmp89D->tag != 18)goto _LL531;else{_tmp89E=_tmp89D->f1;}}_LL530: {
# 3310
struct Cyc_Absyn_Exp*_tmp901=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp89E);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp11A6;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp11A5;return _tmp901 == _tmp89E?t:(void*)((_tmp11A5=_cycalloc(sizeof(*_tmp11A5)),((_tmp11A5[0]=((_tmp11A6.tag=18,((_tmp11A6.f1=_tmp901,_tmp11A6)))),_tmp11A5))));}_LL531: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp89F=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp867;if(_tmp89F->tag != 13)goto _LL533;}_LL532:
 goto _LL534;_LL533: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp8A0=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp867;if(_tmp8A0->tag != 14)goto _LL535;}_LL534:
 goto _LL536;_LL535: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp8A1=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp867;if(_tmp8A1->tag != 0)goto _LL537;}_LL536:
 goto _LL538;_LL537: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8A2=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp867;if(_tmp8A2->tag != 6)goto _LL539;}_LL538:
 goto _LL53A;_LL539: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp8A3=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp867;if(_tmp8A3->tag != 7)goto _LL53B;}_LL53A:
 goto _LL53C;_LL53B: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp8A4=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp867;if(_tmp8A4->tag != 22)goto _LL53D;}_LL53C:
 goto _LL53E;_LL53D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp8A5=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp867;if(_tmp8A5->tag != 21)goto _LL53F;}_LL53E:
 goto _LL540;_LL53F: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp8A6=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp867;if(_tmp8A6->tag != 20)goto _LL541;}_LL540:
 return t;_LL541: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8A7=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp867;if(_tmp8A7->tag != 25)goto _LL543;else{_tmp8A8=(void*)_tmp8A7->f1;}}_LL542: {
# 3321
void*_tmp904=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8A8);
struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp11A9;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp11A8;return _tmp904 == _tmp8A8?t:(void*)((_tmp11A8=_cycalloc(sizeof(*_tmp11A8)),((_tmp11A8[0]=((_tmp11A9.tag=25,((_tmp11A9.f1=_tmp904,_tmp11A9)))),_tmp11A8))));}_LL543: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8A9=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp867;if(_tmp8A9->tag != 23)goto _LL545;else{_tmp8AA=(void*)_tmp8A9->f1;}}_LL544: {
# 3324
void*_tmp907=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp8AA);
struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp11AC;struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp11AB;return _tmp907 == _tmp8AA?t:(void*)((_tmp11AB=_cycalloc(sizeof(*_tmp11AB)),((_tmp11AB[0]=((_tmp11AC.tag=23,((_tmp11AC.f1=_tmp907,_tmp11AC)))),_tmp11AB))));}_LL545: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8AB=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp867;if(_tmp8AB->tag != 24)goto _LL547;else{_tmp8AC=_tmp8AB->f1;}}_LL546: {
# 3327
struct Cyc_List_List*_tmp90A=Cyc_Tcutil_substs(rgn,inst,_tmp8AC);
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp11AF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp11AE;return _tmp90A == _tmp8AC?t:(void*)((_tmp11AE=_cycalloc(sizeof(*_tmp11AE)),((_tmp11AE[0]=((_tmp11AF.tag=24,((_tmp11AF.f1=_tmp90A,_tmp11AF)))),_tmp11AE))));}_LL547: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp8AD=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp867;if(_tmp8AD->tag != 26)goto _LL512;}_LL548: {
const char*_tmp11B2;void*_tmp11B1;(_tmp11B1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11B2="found typedecltype in rsubs",_tag_dyneither(_tmp11B2,sizeof(char),28))),_tag_dyneither(_tmp11B1,sizeof(void*),0)));}_LL512:;}
# 3333
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3336
if(ts == 0)
return 0;{
void*_tmp90F=(void*)ts->hd;
struct Cyc_List_List*_tmp910=ts->tl;
void*_tmp911=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp90F);
struct Cyc_List_List*_tmp912=Cyc_Tcutil_substs(rgn,inst,_tmp910);
if(_tmp90F == _tmp911  && _tmp910 == _tmp912)
return ts;{
struct Cyc_List_List*_tmp11B3;return(_tmp11B3=_cycalloc(sizeof(*_tmp11B3)),((_tmp11B3->hd=_tmp911,((_tmp11B3->tl=_tmp912,_tmp11B3)))));};};}
# 3347
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3354
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp914=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp11B6;struct _tuple16*_tmp11B5;return(_tmp11B5=_cycalloc(sizeof(*_tmp11B5)),((_tmp11B5->f1=tv,((_tmp11B5->f2=Cyc_Absyn_new_evar(_tmp914,((_tmp11B6=_cycalloc(sizeof(*_tmp11B6)),((_tmp11B6->v=s,_tmp11B6))))),_tmp11B5)))));}
# 3359
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3361
struct Cyc_List_List*_tmp918;struct _RegionHandle*_tmp919;struct _tuple17*_tmp917=env;_tmp918=_tmp917->f1;_tmp919=_tmp917->f2;{
struct Cyc_Core_Opt*_tmp91A=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
struct Cyc_Core_Opt*_tmp11B9;struct _tuple16*_tmp11B8;return(_tmp11B8=_region_malloc(_tmp919,sizeof(*_tmp11B8)),((_tmp11B8->f1=tv,((_tmp11B8->f2=Cyc_Absyn_new_evar(_tmp91A,((_tmp11B9=_cycalloc(sizeof(*_tmp11B9)),((_tmp11B9->v=_tmp918,_tmp11B9))))),_tmp11B8)))));};}
# 3371
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3375
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2)){
const char*_tmp11BF;void*_tmp11BE[3];struct Cyc_String_pa_PrintArg_struct _tmp11BD;struct Cyc_String_pa_PrintArg_struct _tmp11BC;struct Cyc_String_pa_PrintArg_struct _tmp11BB;(_tmp11BB.tag=0,((_tmp11BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2)),((_tmp11BC.tag=0,((_tmp11BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1)),((_tmp11BD.tag=0,((_tmp11BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name),((_tmp11BE[0]=& _tmp11BD,((_tmp11BE[1]=& _tmp11BC,((_tmp11BE[2]=& _tmp11BB,Cyc_Tcutil_terr(loc,((_tmp11BF="type variable %s is used with inconsistent kinds %s and %s",_tag_dyneither(_tmp11BF,sizeof(char),59))),_tag_dyneither(_tmp11BE,sizeof(void*),3)))))))))))))))))));}
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity){
const char*_tmp11C2;void*_tmp11C1;(_tmp11C1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C2="same type variable has different identity!",_tag_dyneither(_tmp11C2,sizeof(char),43))),_tag_dyneither(_tmp11C1,sizeof(void*),0)));}}
return tvs;}}}
# 3389
tv->identity=Cyc_Tcutil_new_tvar_id();{
struct Cyc_List_List*_tmp11C3;return(_tmp11C3=_cycalloc(sizeof(*_tmp11C3)),((_tmp11C3->hd=tv,((_tmp11C3->tl=tvs,_tmp11C3)))));};}
# 3395
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3397
if(tv->identity == - 1){
const char*_tmp11C6;void*_tmp11C5;(_tmp11C5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C6="fast_add_free_tvar: bad identity in tv",_tag_dyneither(_tmp11C6,sizeof(char),39))),_tag_dyneither(_tmp11C5,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3401
struct Cyc_Absyn_Tvar*_tmp927=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp927->identity == - 1){
const char*_tmp11C9;void*_tmp11C8;(_tmp11C8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11C9="fast_add_free_tvar: bad identity in tvs2",_tag_dyneither(_tmp11C9,sizeof(char),41))),_tag_dyneither(_tmp11C8,sizeof(void*),0)));}
if(_tmp927->identity == tv->identity)
return tvs;}}{
# 3408
struct Cyc_List_List*_tmp11CA;return(_tmp11CA=_cycalloc(sizeof(*_tmp11CA)),((_tmp11CA->hd=tv,((_tmp11CA->tl=tvs,_tmp11CA)))));};}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3414
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3419
if(tv->identity == - 1){
const char*_tmp11CD;void*_tmp11CC;(_tmp11CC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11CD="fast_add_free_tvar_bool: bad identity in tv",_tag_dyneither(_tmp11CD,sizeof(char),44))),_tag_dyneither(_tmp11CC,sizeof(void*),0)));}
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3423
struct _tuple28*_tmp92D=(struct _tuple28*)tvs2->hd;struct Cyc_Absyn_Tvar*_tmp92F;int*_tmp930;struct _tuple28*_tmp92E=_tmp92D;_tmp92F=_tmp92E->f1;_tmp930=(int*)& _tmp92E->f2;
if(_tmp92F->identity == - 1){
const char*_tmp11D0;void*_tmp11CF;(_tmp11CF=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D0="fast_add_free_tvar_bool: bad identity in tvs2",_tag_dyneither(_tmp11D0,sizeof(char),46))),_tag_dyneither(_tmp11CF,sizeof(void*),0)));}
if(_tmp92F->identity == tv->identity){
*_tmp930=*_tmp930  || b;
return tvs;}}}{
# 3431
struct _tuple28*_tmp11D3;struct Cyc_List_List*_tmp11D2;return(_tmp11D2=_region_malloc(r,sizeof(*_tmp11D2)),((_tmp11D2->hd=((_tmp11D3=_region_malloc(r,sizeof(*_tmp11D3)),((_tmp11D3->f1=tv,((_tmp11D3->f2=b,_tmp11D3)))))),((_tmp11D2->tl=tvs,_tmp11D2)))));};}
# 3435
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3437
if(tv->identity == - 1){
const char*_tmp11D7;void*_tmp11D6[1];struct Cyc_String_pa_PrintArg_struct _tmp11D5;(_tmp11D5.tag=0,((_tmp11D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv)),((_tmp11D6[0]=& _tmp11D5,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp11D7="bound tvar id for %s is NULL",_tag_dyneither(_tmp11D7,sizeof(char),29))),_tag_dyneither(_tmp11D6,sizeof(void*),1)))))));}{
struct Cyc_List_List*_tmp11D8;return(_tmp11D8=_cycalloc(sizeof(*_tmp11D8)),((_tmp11D8->hd=tv,((_tmp11D8->tl=tvs,_tmp11D8)))));};}struct _tuple29{void*f1;int f2;};
# 3446
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3449
void*_tmp939=Cyc_Tcutil_compress(e);void*_tmp93A=_tmp939;int _tmp93C;_LL554: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp93B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp93A;if(_tmp93B->tag != 1)goto _LL556;else{_tmp93C=_tmp93B->f3;}}_LL555:
# 3451
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp93D=(struct _tuple29*)es2->hd;void*_tmp93F;int*_tmp940;struct _tuple29*_tmp93E=_tmp93D;_tmp93F=_tmp93E->f1;_tmp940=(int*)& _tmp93E->f2;{
void*_tmp941=Cyc_Tcutil_compress(_tmp93F);void*_tmp942=_tmp941;int _tmp944;_LL559: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp943=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp942;if(_tmp943->tag != 1)goto _LL55B;else{_tmp944=_tmp943->f3;}}_LL55A:
# 3455
 if(_tmp93C == _tmp944){
if(b != *_tmp940)*_tmp940=1;
return es;}
# 3459
goto _LL558;_LL55B:;_LL55C:
 goto _LL558;_LL558:;};}}{
# 3463
struct _tuple29*_tmp11DB;struct Cyc_List_List*_tmp11DA;return(_tmp11DA=_region_malloc(r,sizeof(*_tmp11DA)),((_tmp11DA->hd=((_tmp11DB=_region_malloc(r,sizeof(*_tmp11DB)),((_tmp11DB->f1=e,((_tmp11DB->f2=b,_tmp11DB)))))),((_tmp11DA->tl=es,_tmp11DA)))));};_LL556:;_LL557:
 return es;_LL553:;}
# 3468
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3471
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3480
if(!present){struct Cyc_List_List*_tmp11DC;r=((_tmp11DC=_region_malloc(rgn,sizeof(*_tmp11DC)),((_tmp11DC->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp11DC->tl=r,_tmp11DC))))));}}
# 3482
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3487
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3491
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp948=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmp94A;int _tmp94B;struct _tuple28 _tmp949=_tmp948;_tmp94A=_tmp949.f1;_tmp94B=_tmp949.f2;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp94A->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3501
if(!present){struct Cyc_List_List*_tmp11DD;res=((_tmp11DD=_region_malloc(r,sizeof(*_tmp11DD)),((_tmp11DD->hd=(struct _tuple28*)tvs->hd,((_tmp11DD->tl=res,_tmp11DD))))));}};}
# 3503
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3507
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3509
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width)){
const char*_tmp11E1;void*_tmp11E0[1];struct Cyc_String_pa_PrintArg_struct _tmp11DF;(_tmp11DF.tag=0,((_tmp11DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11E0[0]=& _tmp11DF,Cyc_Tcutil_terr(loc,((_tmp11E1="bitfield %s does not have constant width",_tag_dyneither(_tmp11E1,sizeof(char),41))),_tag_dyneither(_tmp11E0,sizeof(void*),1)))))));}else{
# 3514
struct _tuple14 _tmp950=Cyc_Evexp_eval_const_uint_exp(width);unsigned int _tmp952;int _tmp953;struct _tuple14 _tmp951=_tmp950;_tmp952=_tmp951.f1;_tmp953=_tmp951.f2;
if(!_tmp953){
const char*_tmp11E4;void*_tmp11E3;(_tmp11E3=0,Cyc_Tcutil_terr(loc,((_tmp11E4="bitfield width cannot use sizeof or offsetof",_tag_dyneither(_tmp11E4,sizeof(char),45))),_tag_dyneither(_tmp11E3,sizeof(void*),0)));}
w=_tmp952;}{
# 3519
void*_tmp956=Cyc_Tcutil_compress(field_typ);void*_tmp957=_tmp956;enum Cyc_Absyn_Size_of _tmp959;_LL55E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp958=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp957;if(_tmp958->tag != 6)goto _LL560;else{_tmp959=_tmp958->f2;}}_LL55F:
# 3522
 switch(_tmp959){case Cyc_Absyn_Char_sz: _LL562:
 if(w > 8){const char*_tmp11E7;void*_tmp11E6;(_tmp11E6=0,Cyc_Tcutil_terr(loc,((_tmp11E7="bitfield larger than type",_tag_dyneither(_tmp11E7,sizeof(char),26))),_tag_dyneither(_tmp11E6,sizeof(void*),0)));}break;case Cyc_Absyn_Short_sz: _LL563:
 if(w > 16){const char*_tmp11EA;void*_tmp11E9;(_tmp11E9=0,Cyc_Tcutil_terr(loc,((_tmp11EA="bitfield larger than type",_tag_dyneither(_tmp11EA,sizeof(char),26))),_tag_dyneither(_tmp11E9,sizeof(void*),0)));}break;case Cyc_Absyn_Long_sz: _LL564:
 goto _LL565;case Cyc_Absyn_Int_sz: _LL565:
# 3527
 if(w > 32){const char*_tmp11ED;void*_tmp11EC;(_tmp11EC=0,Cyc_Tcutil_terr(loc,((_tmp11ED="bitfield larger than type",_tag_dyneither(_tmp11ED,sizeof(char),26))),_tag_dyneither(_tmp11EC,sizeof(void*),0)));}break;case Cyc_Absyn_LongLong_sz: _LL566:
# 3529
 if(w > 64){const char*_tmp11F0;void*_tmp11EF;(_tmp11EF=0,Cyc_Tcutil_terr(loc,((_tmp11F0="bitfield larger than type",_tag_dyneither(_tmp11F0,sizeof(char),26))),_tag_dyneither(_tmp11EF,sizeof(void*),0)));}break;}
# 3531
goto _LL55D;_LL560:;_LL561:
# 3533
{const char*_tmp11F5;void*_tmp11F4[2];struct Cyc_String_pa_PrintArg_struct _tmp11F3;struct Cyc_String_pa_PrintArg_struct _tmp11F2;(_tmp11F2.tag=0,((_tmp11F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ)),((_tmp11F3.tag=0,((_tmp11F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11F4[0]=& _tmp11F3,((_tmp11F4[1]=& _tmp11F2,Cyc_Tcutil_terr(loc,((_tmp11F5="bitfield %s must have integral type but has type %s",_tag_dyneither(_tmp11F5,sizeof(char),52))),_tag_dyneither(_tmp11F4,sizeof(void*),2)))))))))))));}
goto _LL55D;_LL55D:;};}}
# 3540
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp966=(void*)atts->hd;void*_tmp967=_tmp966;_LL569: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp968=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp967;if(_tmp968->tag != 7)goto _LL56B;}_LL56A:
 continue;_LL56B: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp969=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp967;if(_tmp969->tag != 6)goto _LL56D;}_LL56C:
 continue;_LL56D:;_LL56E: {
const char*_tmp11FA;void*_tmp11F9[2];struct Cyc_String_pa_PrintArg_struct _tmp11F8;struct Cyc_String_pa_PrintArg_struct _tmp11F7;(_tmp11F7.tag=0,((_tmp11F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn),((_tmp11F8.tag=0,((_tmp11F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd)),((_tmp11F9[0]=& _tmp11F8,((_tmp11F9[1]=& _tmp11F7,Cyc_Tcutil_terr(loc,((_tmp11FA="bad attribute %s on member %s",_tag_dyneither(_tmp11FA,sizeof(char),30))),_tag_dyneither(_tmp11F9,sizeof(void*),2)))))))))))));}_LL568:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3564
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3568
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp96E=t;struct Cyc_Absyn_Typedefdecl*_tmp970;void*_tmp971;_LL570: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp96F=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp96E;if(_tmp96F->tag != 17)goto _LL572;else{_tmp970=_tmp96F->f3;_tmp971=(void*)_tmp96F->f4;}}_LL571:
# 3571
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp970))->tq).real_const  || (_tmp970->tq).print_const){
if(declared_const){const char*_tmp11FD;void*_tmp11FC;(_tmp11FC=0,Cyc_Tcutil_warn(loc,((_tmp11FD="extra const",_tag_dyneither(_tmp11FD,sizeof(char),12))),_tag_dyneither(_tmp11FC,sizeof(void*),0)));}
return 1;}
# 3576
if((unsigned int)_tmp971)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp971);else{
return declared_const;}_LL572:;_LL573:
 return declared_const;_LL56F:;}
# 3583
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp974=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp975=_tmp974;void*_tmp977;_LL575: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp976=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp975;if(_tmp976->tag != 5)goto _LL577;else{_tmp977=((_tmp976->f1).ptr_atts).rgn;}}_LL576:
# 3588
{void*_tmp978=Cyc_Tcutil_compress(_tmp977);void*_tmp979=_tmp978;struct Cyc_Absyn_Tvar*_tmp97B;_LL57A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp97A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp979;if(_tmp97A->tag != 2)goto _LL57C;else{_tmp97B=_tmp97A->f1;}}_LL57B:
# 3590
 return Cyc_Absyn_tvar_cmp(tvar,_tmp97B)== 0;_LL57C:;_LL57D:
 goto _LL579;_LL579:;}
# 3593
goto _LL574;_LL577:;_LL578:
 goto _LL574;_LL574:;}}else{
# 3599
return 1;}
return 0;}
# 3603
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3606
void*_tmp97C=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp97D=_tmp97C;_LL57F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp97E=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97D;if(_tmp97E->tag != 2)goto _LL581;else{if((_tmp97E->f2)->kind != Cyc_Absyn_RgnKind)goto _LL581;if((_tmp97E->f2)->aliasqual != Cyc_Absyn_Top)goto _LL581;}}_LL580:
 goto _LL582;_LL581: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp97F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97D;if(_tmp97F->tag != 0)goto _LL583;else{if((_tmp97F->f1)->kind != Cyc_Absyn_RgnKind)goto _LL583;if((_tmp97F->f1)->aliasqual != Cyc_Absyn_Top)goto _LL583;}}_LL582:
# 3615
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3618
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3624
return& Cyc_Tcutil_trk;_LL583:;_LL584:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);_LL57E:;}
# 3659 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3662
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect){
# 3669
{void*_tmp980=Cyc_Tcutil_compress(t);void*_tmp981=_tmp980;struct Cyc_Core_Opt**_tmp984;void**_tmp985;struct Cyc_Absyn_Tvar*_tmp987;void*_tmp989;void***_tmp98A;struct Cyc_List_List*_tmp98C;struct _tuple2*_tmp98E;struct Cyc_Absyn_Enumdecl**_tmp98F;union Cyc_Absyn_DatatypeInfoU*_tmp991;struct Cyc_List_List**_tmp992;union Cyc_Absyn_DatatypeFieldInfoU*_tmp994;struct Cyc_List_List*_tmp995;void*_tmp997;struct Cyc_Absyn_Tqual*_tmp998;void*_tmp999;union Cyc_Absyn_Constraint*_tmp99A;union Cyc_Absyn_Constraint*_tmp99B;union Cyc_Absyn_Constraint*_tmp99C;void*_tmp99E;struct Cyc_Absyn_Exp*_tmp9A0;void*_tmp9A4;struct Cyc_Absyn_Tqual*_tmp9A5;struct Cyc_Absyn_Exp**_tmp9A6;union Cyc_Absyn_Constraint*_tmp9A7;unsigned int _tmp9A8;struct Cyc_List_List**_tmp9AA;void**_tmp9AB;struct Cyc_Absyn_Tqual*_tmp9AC;void*_tmp9AD;struct Cyc_List_List*_tmp9AE;int _tmp9AF;struct Cyc_Absyn_VarargInfo*_tmp9B0;struct Cyc_List_List*_tmp9B1;struct Cyc_List_List*_tmp9B2;struct Cyc_List_List*_tmp9B4;enum Cyc_Absyn_AggrKind _tmp9B6;struct Cyc_List_List*_tmp9B7;union Cyc_Absyn_AggrInfoU*_tmp9B9;struct Cyc_List_List**_tmp9BA;struct _tuple2*_tmp9BC;struct Cyc_List_List**_tmp9BD;struct Cyc_Absyn_Typedefdecl**_tmp9BE;void**_tmp9BF;void*_tmp9C4;void*_tmp9C6;void*_tmp9C7;void*_tmp9C9;void*_tmp9CB;struct Cyc_List_List*_tmp9CD;_LL586: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp982=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp981;if(_tmp982->tag != 0)goto _LL588;}_LL587:
 goto _LL585;_LL588: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp983=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp981;if(_tmp983->tag != 1)goto _LL58A;else{_tmp984=(struct Cyc_Core_Opt**)& _tmp983->f1;_tmp985=(void**)((void**)& _tmp983->f2);}}_LL589:
# 3673
 if(*_tmp984 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp984))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp984))->v,expected_kind))
*_tmp984=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3678
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp985=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3681
*_tmp985=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1200;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp11FF;struct Cyc_Absyn_Tvar*_tmp9CE=Cyc_Tcutil_new_tvar((void*)((_tmp11FF=_cycalloc(sizeof(*_tmp11FF)),((_tmp11FF[0]=((_tmp1200.tag=2,((_tmp1200.f1=0,((_tmp1200.f2=expected_kind,_tmp1200)))))),_tmp11FF)))));
{struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1203;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1202;*_tmp985=(void*)((_tmp1202=_cycalloc(sizeof(*_tmp1202)),((_tmp1202[0]=((_tmp1203.tag=2,((_tmp1203.f1=_tmp9CE,_tmp1203)))),_tmp1202))));}
_tmp987=_tmp9CE;goto _LL58B;}else{
# 3687
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3689
goto _LL585;_LL58A: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp986=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp981;if(_tmp986->tag != 2)goto _LL58C;else{_tmp987=_tmp986->f1;}}_LL58B:
# 3691
{void*_tmp9D3=Cyc_Absyn_compress_kb(_tmp987->kind);void*_tmp9D4=_tmp9D3;struct Cyc_Core_Opt**_tmp9D6;_LL5BD: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9D5=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp9D4;if(_tmp9D5->tag != 1)goto _LL5BF;else{_tmp9D6=(struct Cyc_Core_Opt**)& _tmp9D5->f1;}}_LL5BE:
# 3693
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1209;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1208;struct Cyc_Core_Opt*_tmp1207;*_tmp9D6=((_tmp1207=_cycalloc(sizeof(*_tmp1207)),((_tmp1207->v=(void*)((_tmp1209=_cycalloc(sizeof(*_tmp1209)),((_tmp1209[0]=((_tmp1208.tag=2,((_tmp1208.f1=0,((_tmp1208.f2=expected_kind,_tmp1208)))))),_tmp1209)))),_tmp1207))));}goto _LL5BC;_LL5BF:;_LL5C0:
 goto _LL5BC;_LL5BC:;}
# 3698
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp987);
# 3701
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp987,put_in_effect);
# 3703
{void*_tmp9DA=Cyc_Absyn_compress_kb(_tmp987->kind);void*_tmp9DB=_tmp9DA;struct Cyc_Core_Opt**_tmp9DD;struct Cyc_Absyn_Kind*_tmp9DE;_LL5C2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB;if(_tmp9DC->tag != 2)goto _LL5C4;else{_tmp9DD=(struct Cyc_Core_Opt**)& _tmp9DC->f1;_tmp9DE=_tmp9DC->f2;}}_LL5C3:
# 3705
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp9DE)){
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp120F;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp120E;struct Cyc_Core_Opt*_tmp120D;*_tmp9DD=((_tmp120D=_cycalloc(sizeof(*_tmp120D)),((_tmp120D->v=(void*)((_tmp120F=_cycalloc(sizeof(*_tmp120F)),((_tmp120F[0]=((_tmp120E.tag=2,((_tmp120E.f1=0,((_tmp120E.f2=expected_kind,_tmp120E)))))),_tmp120F)))),_tmp120D))));}
goto _LL5C1;_LL5C4:;_LL5C5:
 goto _LL5C1;_LL5C1:;}
# 3710
goto _LL585;_LL58C: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp988=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp981;if(_tmp988->tag != 26)goto _LL58E;else{_tmp989=(_tmp988->f1)->r;_tmp98A=(void***)& _tmp988->f2;}}_LL58D: {
# 3716
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp9E2=_tmp989;struct Cyc_Absyn_Aggrdecl*_tmp9E4;struct Cyc_Absyn_Enumdecl*_tmp9E6;struct Cyc_Absyn_Datatypedecl*_tmp9E8;_LL5C7: {struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp9E3=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp9E2;if(_tmp9E3->tag != 0)goto _LL5C9;else{_tmp9E4=_tmp9E3->f1;}}_LL5C8:
# 3719
 Cyc_Tc_tcAggrdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E4);goto _LL5C6;_LL5C9: {struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp9E5=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp9E2;if(_tmp9E5->tag != 1)goto _LL5CB;else{_tmp9E6=_tmp9E5->f1;}}_LL5CA:
# 3721
 Cyc_Tc_tcEnumdecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E6);goto _LL5C6;_LL5CB: {struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp9E7=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp9E2;if(_tmp9E7->tag != 2)goto _LL5C6;else{_tmp9E8=_tmp9E7->f1;}}_LL5CC:
# 3723
 Cyc_Tc_tcDatatypedecl(te,((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns),loc,_tmp9E8);goto _LL5C6;_LL5C6:;}
# 3725
{void**_tmp1210;*_tmp98A=((_tmp1210=_cycalloc(sizeof(*_tmp1210)),((_tmp1210[0]=new_t,_tmp1210))));}
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect);}_LL58E: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp98B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp981;if(_tmp98B->tag != 14)goto _LL590;else{_tmp98C=_tmp98B->f1;}}_LL58F: {
# 3730
struct Cyc_Tcenv_Genv*ge=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct _RegionHandle _tmp9EA=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp9EA;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp98C != 0;_tmp98C=_tmp98C->tl){
struct Cyc_Absyn_Enumfield*_tmp9EB=(struct Cyc_Absyn_Enumfield*)_tmp98C->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp9EB->name).f2)){
const char*_tmp1214;void*_tmp1213[1];struct Cyc_String_pa_PrintArg_struct _tmp1212;(_tmp1212.tag=0,((_tmp1212.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9EB->name).f2),((_tmp1213[0]=& _tmp1212,Cyc_Tcutil_terr(_tmp9EB->loc,((_tmp1214="duplicate enum field name %s",_tag_dyneither(_tmp1214,sizeof(char),29))),_tag_dyneither(_tmp1213,sizeof(void*),1)))))));}else{
# 3739
struct Cyc_List_List*_tmp1215;prev_fields=((_tmp1215=_region_malloc(uprev_rgn,sizeof(*_tmp1215)),((_tmp1215->hd=(*_tmp9EB->name).f2,((_tmp1215->tl=prev_fields,_tmp1215))))));}
# 3741
if(_tmp9EB->tag == 0)
_tmp9EB->tag=Cyc_Absyn_uint_exp(tag_count,_tmp9EB->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp9EB->tag))){
const char*_tmp1219;void*_tmp1218[1];struct Cyc_String_pa_PrintArg_struct _tmp1217;(_tmp1217.tag=0,((_tmp1217.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp9EB->name).f2),((_tmp1218[0]=& _tmp1217,Cyc_Tcutil_terr(loc,((_tmp1219="enum field %s: expression is not constant",_tag_dyneither(_tmp1219,sizeof(char),42))),_tag_dyneither(_tmp1218,sizeof(void*),1)))))));}}{
# 3746
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp9EB->tag))).f1;
tag_count=t1 + 1;
{union Cyc_Absyn_Nmspace _tmp9F3=(*_tmp9EB->name).f1;union Cyc_Absyn_Nmspace _tmp9F4=_tmp9F3;_LL5CE: if((_tmp9F4.Rel_n).tag != 1)goto _LL5D0;_LL5CF:
# 3750
(*_tmp9EB->name).f1=Cyc_Absyn_Abs_n(te->ns,0);goto _LL5CD;_LL5D0:;_LL5D1:
 goto _LL5CD;_LL5CD:;}{
# 3753
struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp121F;struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp121E;struct _tuple29*_tmp121D;ge->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct _tuple29*v))Cyc_Dict_insert)(ge->ordinaries,(*_tmp9EB->name).f2,(
(_tmp121D=_cycalloc(sizeof(*_tmp121D)),((_tmp121D->f1=(void*)((_tmp121F=_cycalloc(sizeof(*_tmp121F)),((_tmp121F[0]=((_tmp121E.tag=4,((_tmp121E.f1=t,((_tmp121E.f2=_tmp9EB,_tmp121E)))))),_tmp121F)))),((_tmp121D->f2=1,_tmp121D)))))));};};}}
# 3757
_npop_handler(0);goto _LL585;
# 3731
;_pop_region(uprev_rgn);}_LL590: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp98D=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp981;if(_tmp98D->tag != 13)goto _LL592;else{_tmp98E=_tmp98D->f1;_tmp98F=(struct Cyc_Absyn_Enumdecl**)& _tmp98D->f2;}}_LL591:
# 3759
 if(*_tmp98F == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp98F))->fields == 0){
struct _handler_cons _tmp9F8;_push_handler(& _tmp9F8);{int _tmp9FA=0;if(setjmp(_tmp9F8.handler))_tmp9FA=1;if(!_tmp9FA){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp98E);
*_tmp98F=*ed;}
# 3761
;_pop_handler();}else{void*_tmp9F9=(void*)_exn_thrown;void*_tmp9FC=_tmp9F9;void*_tmp9FE;_LL5D3: {struct Cyc_Dict_Absent_exn_struct*_tmp9FD=(struct Cyc_Dict_Absent_exn_struct*)_tmp9FC;if(_tmp9FD->tag != Cyc_Dict_Absent)goto _LL5D5;}_LL5D4: {
# 3765
struct Cyc_Tcenv_Genv*_tmp9FF=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Enumdecl*_tmp1220;struct Cyc_Absyn_Enumdecl*_tmpA00=(_tmp1220=_cycalloc(sizeof(*_tmp1220)),((_tmp1220->sc=Cyc_Absyn_Extern,((_tmp1220->name=_tmp98E,((_tmp1220->fields=0,_tmp1220)))))));
Cyc_Tc_tcEnumdecl(te,_tmp9FF,loc,_tmpA00);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp98E);
*_tmp98F=*ed;
goto _LL5D2;};}_LL5D5: _tmp9FE=_tmp9FC;_LL5D6:(void)_throw(_tmp9FE);_LL5D2:;}};}{
# 3774
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp98F);
# 3776
*_tmp98E=(ed->name)[0];
goto _LL585;};_LL592: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp990=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp981;if(_tmp990->tag != 3)goto _LL594;else{_tmp991=(union Cyc_Absyn_DatatypeInfoU*)&(_tmp990->f1).datatype_info;_tmp992=(struct Cyc_List_List**)&(_tmp990->f1).targs;}}_LL593: {
# 3779
struct Cyc_List_List*_tmpA02=*_tmp992;
{union Cyc_Absyn_DatatypeInfoU _tmpA03=*_tmp991;union Cyc_Absyn_DatatypeInfoU _tmpA04=_tmpA03;struct _tuple2*_tmpA05;int _tmpA06;struct Cyc_Absyn_Datatypedecl*_tmpA07;_LL5D8: if((_tmpA04.UnknownDatatype).tag != 1)goto _LL5DA;_tmpA05=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA04.UnknownDatatype).val).name;_tmpA06=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpA04.UnknownDatatype).val).is_extensible;_LL5D9: {
# 3782
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmpA08;_push_handler(& _tmpA08);{int _tmpA0A=0;if(setjmp(_tmpA08.handler))_tmpA0A=1;if(!_tmpA0A){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA05);;_pop_handler();}else{void*_tmpA09=(void*)_exn_thrown;void*_tmpA0C=_tmpA09;void*_tmpA0E;_LL5DD: {struct Cyc_Dict_Absent_exn_struct*_tmpA0D=(struct Cyc_Dict_Absent_exn_struct*)_tmpA0C;if(_tmpA0D->tag != Cyc_Dict_Absent)goto _LL5DF;}_LL5DE: {
# 3786
struct Cyc_Tcenv_Genv*_tmpA0F=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Datatypedecl*_tmp1221;struct Cyc_Absyn_Datatypedecl*_tmpA10=(_tmp1221=_cycalloc(sizeof(*_tmp1221)),((_tmp1221->sc=Cyc_Absyn_Extern,((_tmp1221->name=_tmpA05,((_tmp1221->tvs=0,((_tmp1221->fields=0,((_tmp1221->is_extensible=_tmpA06,_tmp1221)))))))))));
Cyc_Tc_tcDatatypedecl(te,_tmpA0F,loc,_tmpA10);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA05);
# 3791
if(_tmpA02 != 0){
{const char*_tmp1225;void*_tmp1224[1];struct Cyc_String_pa_PrintArg_struct _tmp1223;(_tmp1223.tag=0,((_tmp1223.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA05)),((_tmp1224[0]=& _tmp1223,Cyc_Tcutil_terr(loc,((_tmp1225="declare parameterized datatype %s before using",_tag_dyneither(_tmp1225,sizeof(char),47))),_tag_dyneither(_tmp1224,sizeof(void*),1)))))));}
return cvtenv;}
# 3796
goto _LL5DC;}_LL5DF: _tmpA0E=_tmpA0C;_LL5E0:(void)_throw(_tmpA0E);_LL5DC:;}};}
# 3802
if(_tmpA06  && !(*tudp)->is_extensible){
const char*_tmp1229;void*_tmp1228[1];struct Cyc_String_pa_PrintArg_struct _tmp1227;(_tmp1227.tag=0,((_tmp1227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA05)),((_tmp1228[0]=& _tmp1227,Cyc_Tcutil_terr(loc,((_tmp1229="datatype %s was not declared @extensible",_tag_dyneither(_tmp1229,sizeof(char),41))),_tag_dyneither(_tmp1228,sizeof(void*),1)))))));}
*_tmp991=Cyc_Absyn_KnownDatatype(tudp);
_tmpA07=*tudp;goto _LL5DB;}_LL5DA: if((_tmpA04.KnownDatatype).tag != 2)goto _LL5D7;_tmpA07=*((struct Cyc_Absyn_Datatypedecl**)(_tmpA04.KnownDatatype).val);_LL5DB: {
# 3809
struct Cyc_List_List*tvs=_tmpA07->tvs;
for(0;_tmpA02 != 0  && tvs != 0;(_tmpA02=_tmpA02->tl,tvs=tvs->tl)){
void*t=(void*)_tmpA02->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3815
{struct _tuple0 _tmp122A;struct _tuple0 _tmpA18=(_tmp122A.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp122A.f2=t,_tmp122A)));struct _tuple0 _tmpA19=_tmpA18;struct Cyc_Absyn_Tvar*_tmpA1C;_LL5E2:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA1A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA19.f1;if(_tmpA1A->tag != 1)goto _LL5E4;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA1B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA19.f2;if(_tmpA1B->tag != 2)goto _LL5E4;else{_tmpA1C=_tmpA1B->f1;}};_LL5E3:
# 3817
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA1C);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA1C,1);
continue;_LL5E4:;_LL5E5:
 goto _LL5E1;_LL5E1:;}{
# 3822
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3826
if(_tmpA02 != 0){
const char*_tmp122E;void*_tmp122D[1];struct Cyc_String_pa_PrintArg_struct _tmp122C;(_tmp122C.tag=0,((_tmp122C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA07->name)),((_tmp122D[0]=& _tmp122C,Cyc_Tcutil_terr(loc,((_tmp122E="too many type arguments for datatype %s",_tag_dyneither(_tmp122E,sizeof(char),40))),_tag_dyneither(_tmp122D,sizeof(void*),1)))))));}
if(tvs != 0){
# 3831
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp122F;hidden_ts=((_tmp122F=_cycalloc(sizeof(*_tmp122F)),((_tmp122F->hd=e,((_tmp122F->tl=hidden_ts,_tmp122F))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1);}
# 3838
*_tmp992=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp992,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3840
goto _LL5D7;}_LL5D7:;}
# 3842
goto _LL585;}_LL594: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp993=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp981;if(_tmp993->tag != 4)goto _LL596;else{_tmp994=(union Cyc_Absyn_DatatypeFieldInfoU*)&(_tmp993->f1).field_info;_tmp995=(_tmp993->f1).targs;}}_LL595:
# 3845
{union Cyc_Absyn_DatatypeFieldInfoU _tmpA22=*_tmp994;union Cyc_Absyn_DatatypeFieldInfoU _tmpA23=_tmpA22;struct _tuple2*_tmpA24;struct _tuple2*_tmpA25;int _tmpA26;struct Cyc_Absyn_Datatypedecl*_tmpA27;struct Cyc_Absyn_Datatypefield*_tmpA28;_LL5E7: if((_tmpA23.UnknownDatatypefield).tag != 1)goto _LL5E9;_tmpA24=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA23.UnknownDatatypefield).val).datatype_name;_tmpA25=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA23.UnknownDatatypefield).val).field_name;_tmpA26=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpA23.UnknownDatatypefield).val).is_extensible;_LL5E8: {
# 3847
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmpA29;_push_handler(& _tmpA29);{int _tmpA2B=0;if(setjmp(_tmpA29.handler))_tmpA2B=1;if(!_tmpA2B){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmpA24);;_pop_handler();}else{void*_tmpA2A=(void*)_exn_thrown;void*_tmpA2D=_tmpA2A;void*_tmpA2F;_LL5EC: {struct Cyc_Dict_Absent_exn_struct*_tmpA2E=(struct Cyc_Dict_Absent_exn_struct*)_tmpA2D;if(_tmpA2E->tag != Cyc_Dict_Absent)goto _LL5EE;}_LL5ED:
# 3851
{const char*_tmp1233;void*_tmp1232[1];struct Cyc_String_pa_PrintArg_struct _tmp1231;(_tmp1231.tag=0,((_tmp1231.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA24)),((_tmp1232[0]=& _tmp1231,Cyc_Tcutil_terr(loc,((_tmp1233="unbound datatype %s",_tag_dyneither(_tmp1233,sizeof(char),20))),_tag_dyneither(_tmp1232,sizeof(void*),1)))))));}
return cvtenv;_LL5EE: _tmpA2F=_tmpA2D;_LL5EF:(void)_throw(_tmpA2F);_LL5EB:;}};}
# 3854
{struct _handler_cons _tmpA33;_push_handler(& _tmpA33);{int _tmpA35=0;if(setjmp(_tmpA33.handler))_tmpA35=1;if(!_tmpA35){
{struct _RegionHandle _tmpA36=_new_region("r");struct _RegionHandle*r=& _tmpA36;_push_region(r);
{void*_tmpA37=Cyc_Tcenv_lookup_ordinary(r,te,loc,_tmpA25,0);void*_tmpA38=_tmpA37;struct Cyc_Absyn_Datatypedecl*_tmpA3A;struct Cyc_Absyn_Datatypefield*_tmpA3B;_LL5F1: {struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmpA39=(struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmpA38;if(_tmpA39->tag != 2)goto _LL5F3;else{_tmpA3A=_tmpA39->f1;_tmpA3B=_tmpA39->f2;}}_LL5F2:
# 3858
 tuf=_tmpA3B;
tud=_tmpA3A;
if(_tmpA26  && !tud->is_extensible){
const char*_tmp1237;void*_tmp1236[1];struct Cyc_String_pa_PrintArg_struct _tmp1235;(_tmp1235.tag=0,((_tmp1235.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA24)),((_tmp1236[0]=& _tmp1235,Cyc_Tcutil_terr(loc,((_tmp1237="datatype %s was not declared @extensible",_tag_dyneither(_tmp1237,sizeof(char),41))),_tag_dyneither(_tmp1236,sizeof(void*),1)))))));}
goto _LL5F0;_LL5F3:;_LL5F4:
{const char*_tmp123C;void*_tmp123B[2];struct Cyc_String_pa_PrintArg_struct _tmp123A;struct Cyc_String_pa_PrintArg_struct _tmp1239;(_tmp1239.tag=0,((_tmp1239.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA24)),((_tmp123A.tag=0,((_tmp123A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA25)),((_tmp123B[0]=& _tmp123A,((_tmp123B[1]=& _tmp1239,Cyc_Tcutil_terr(loc,((_tmp123C="unbound field %s in type datatype %s",_tag_dyneither(_tmp123C,sizeof(char),37))),_tag_dyneither(_tmp123B,sizeof(void*),2)))))))))))));}{
struct Cyc_Tcutil_CVTEnv _tmpA43=cvtenv;_npop_handler(1);return _tmpA43;};_LL5F0:;}
# 3856
;_pop_region(r);}
# 3855
;_pop_handler();}else{void*_tmpA34=(void*)_exn_thrown;void*_tmpA45=_tmpA34;void*_tmpA47;_LL5F6: {struct Cyc_Dict_Absent_exn_struct*_tmpA46=(struct Cyc_Dict_Absent_exn_struct*)_tmpA45;if(_tmpA46->tag != Cyc_Dict_Absent)goto _LL5F8;}_LL5F7:
# 3871
{const char*_tmp1241;void*_tmp1240[2];struct Cyc_String_pa_PrintArg_struct _tmp123F;struct Cyc_String_pa_PrintArg_struct _tmp123E;(_tmp123E.tag=0,((_tmp123E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA24)),((_tmp123F.tag=0,((_tmp123F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA25)),((_tmp1240[0]=& _tmp123F,((_tmp1240[1]=& _tmp123E,Cyc_Tcutil_terr(loc,((_tmp1241="unbound field %s in type datatype %s",_tag_dyneither(_tmp1241,sizeof(char),37))),_tag_dyneither(_tmp1240,sizeof(void*),2)))))))))))));}
return cvtenv;_LL5F8: _tmpA47=_tmpA45;_LL5F9:(void)_throw(_tmpA47);_LL5F5:;}};}
# 3875
*_tmp994=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmpA27=tud;_tmpA28=tuf;goto _LL5EA;}_LL5E9: if((_tmpA23.KnownDatatypefield).tag != 2)goto _LL5E6;_tmpA27=((struct _tuple3)(_tmpA23.KnownDatatypefield).val).f1;_tmpA28=((struct _tuple3)(_tmpA23.KnownDatatypefield).val).f2;_LL5EA: {
# 3879
struct Cyc_List_List*tvs=_tmpA27->tvs;
for(0;_tmp995 != 0  && tvs != 0;(_tmp995=_tmp995->tl,tvs=tvs->tl)){
void*t=(void*)_tmp995->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3885
{struct _tuple0 _tmp1242;struct _tuple0 _tmpA4C=(_tmp1242.f1=Cyc_Absyn_compress_kb(tv->kind),((_tmp1242.f2=t,_tmp1242)));struct _tuple0 _tmpA4D=_tmpA4C;struct Cyc_Absyn_Tvar*_tmpA50;_LL5FB:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA4E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA4D.f1;if(_tmpA4E->tag != 1)goto _LL5FD;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpA4F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA4D.f2;if(_tmpA4F->tag != 2)goto _LL5FD;else{_tmpA50=_tmpA4F->f1;}};_LL5FC:
# 3887
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpA50);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpA50,1);
continue;_LL5FD:;_LL5FE:
 goto _LL5FA;_LL5FA:;}{
# 3892
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3896
if(_tmp995 != 0){
const char*_tmp1247;void*_tmp1246[2];struct Cyc_String_pa_PrintArg_struct _tmp1245;struct Cyc_String_pa_PrintArg_struct _tmp1244;(_tmp1244.tag=0,((_tmp1244.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA28->name)),((_tmp1245.tag=0,((_tmp1245.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA27->name)),((_tmp1246[0]=& _tmp1245,((_tmp1246[1]=& _tmp1244,Cyc_Tcutil_terr(loc,((_tmp1247="too many type arguments for datatype %s.%s",_tag_dyneither(_tmp1247,sizeof(char),43))),_tag_dyneither(_tmp1246,sizeof(void*),2)))))))))))));}
if(tvs != 0){
const char*_tmp124C;void*_tmp124B[2];struct Cyc_String_pa_PrintArg_struct _tmp124A;struct Cyc_String_pa_PrintArg_struct _tmp1249;(_tmp1249.tag=0,((_tmp1249.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpA28->name)),((_tmp124A.tag=0,((_tmp124A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpA27->name)),((_tmp124B[0]=& _tmp124A,((_tmp124B[1]=& _tmp1249,Cyc_Tcutil_terr(loc,((_tmp124C="too few type arguments for datatype %s.%s",_tag_dyneither(_tmp124C,sizeof(char),42))),_tag_dyneither(_tmp124B,sizeof(void*),2)))))))))))));}
goto _LL5E6;}_LL5E6:;}
# 3904
goto _LL585;_LL596: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp996=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp981;if(_tmp996->tag != 5)goto _LL598;else{_tmp997=(_tmp996->f1).elt_typ;_tmp998=(struct Cyc_Absyn_Tqual*)&(_tmp996->f1).elt_tq;_tmp999=((_tmp996->f1).ptr_atts).rgn;_tmp99A=((_tmp996->f1).ptr_atts).nullable;_tmp99B=((_tmp996->f1).ptr_atts).bounds;_tmp99C=((_tmp996->f1).ptr_atts).zero_term;}}_LL597: {
# 3907
int is_zero_terminated;
# 3909
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp997,1);
_tmp998->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp998->print_const,_tmp997);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmpA5A=expected_kind->aliasqual;switch(_tmpA5A){case Cyc_Absyn_Aliasable: _LL5FF:
 k=& Cyc_Tcutil_rk;break;case Cyc_Absyn_Unique: _LL600:
 k=& Cyc_Tcutil_urk;break;case Cyc_Absyn_Top: _LL601:
 k=& Cyc_Tcutil_trk;break;}}
# 3917
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp999,1);
{union Cyc_Absyn_Constraint*_tmpA5B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp99C);union Cyc_Absyn_Constraint*_tmpA5C=_tmpA5B;_LL604: if((_tmpA5C->No_constr).tag != 3)goto _LL606;_LL605:
# 3922
{void*_tmpA5D=Cyc_Tcutil_compress(_tmp997);void*_tmpA5E=_tmpA5D;_LL60B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpA5F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpA5E;if(_tmpA5F->tag != 6)goto _LL60D;else{if(_tmpA5F->f2 != Cyc_Absyn_Char_sz)goto _LL60D;}}_LL60C:
# 3924
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99C,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL60A;_LL60D:;_LL60E:
# 3928
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp99C,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL60A;_LL60A:;}
# 3932
goto _LL603;_LL606: if((_tmpA5C->Eq_constr).tag != 1)goto _LL608;if((int)(_tmpA5C->Eq_constr).val != 1)goto _LL608;_LL607:
# 3935
 if(!Cyc_Tcutil_admits_zero(_tmp997)){
const char*_tmp1250;void*_tmp124F[1];struct Cyc_String_pa_PrintArg_struct _tmp124E;(_tmp124E.tag=0,((_tmp124E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp997)),((_tmp124F[0]=& _tmp124E,Cyc_Tcutil_terr(loc,((_tmp1250="cannot have a pointer to zero-terminated %s elements",_tag_dyneither(_tmp1250,sizeof(char),53))),_tag_dyneither(_tmp124F,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL603;_LL608:;_LL609:
# 3941
 is_zero_terminated=0;
goto _LL603;_LL603:;}
# 3945
{void*_tmpA63=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp99B);void*_tmpA64=_tmpA63;struct Cyc_Absyn_Exp*_tmpA67;_LL610: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpA65=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA64;if(_tmpA65->tag != 0)goto _LL612;}_LL611:
 goto _LL60F;_LL612: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpA66=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA64;if(_tmpA66->tag != 1)goto _LL60F;else{_tmpA67=_tmpA66->f1;}}_LL613: {
# 3948
struct _RegionHandle _tmpA68=_new_region("temp");struct _RegionHandle*temp=& _tmpA68;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA69=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA69,0,_tmpA67);}
# 3952
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA67,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA67)){
const char*_tmp1253;void*_tmp1252;(_tmp1252=0,Cyc_Tcutil_terr(loc,((_tmp1253="pointer bounds expression is not an unsigned int",_tag_dyneither(_tmp1253,sizeof(char),49))),_tag_dyneither(_tmp1252,sizeof(void*),0)));}{
struct _tuple14 _tmpA6C=Cyc_Evexp_eval_const_uint_exp(_tmpA67);unsigned int _tmpA6E;int _tmpA6F;struct _tuple14 _tmpA6D=_tmpA6C;_tmpA6E=_tmpA6D.f1;_tmpA6F=_tmpA6D.f2;
if(is_zero_terminated  && (!_tmpA6F  || _tmpA6E < 1)){
const char*_tmp1256;void*_tmp1255;(_tmp1255=0,Cyc_Tcutil_terr(loc,((_tmp1256="zero-terminated pointer cannot point to empty sequence",_tag_dyneither(_tmp1256,sizeof(char),55))),_tag_dyneither(_tmp1255,sizeof(void*),0)));}
_npop_handler(0);goto _LL60F;};
# 3948
;_pop_region(temp);}_LL60F:;}
# 3960
goto _LL585;};}_LL598: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp99D=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp981;if(_tmp99D->tag != 19)goto _LL59A;else{_tmp99E=(void*)_tmp99D->f1;}}_LL599:
# 3962
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp99E,1);goto _LL585;_LL59A: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp99F=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp981;if(_tmp99F->tag != 18)goto _LL59C;else{_tmp9A0=_tmp99F->f1;}}_LL59B: {
# 3967
struct _RegionHandle _tmpA72=_new_region("temp");struct _RegionHandle*temp=& _tmpA72;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA73=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA73,0,_tmp9A0);}
# 3971
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp9A0)){
const char*_tmp1259;void*_tmp1258;(_tmp1258=0,Cyc_Tcutil_terr(loc,((_tmp1259="valueof_t requires an int expression",_tag_dyneither(_tmp1259,sizeof(char),37))),_tag_dyneither(_tmp1258,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp9A0,te,cvtenv);
_npop_handler(0);goto _LL585;
# 3967
;_pop_region(temp);}_LL59C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp9A1=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp981;if(_tmp9A1->tag != 6)goto _LL59E;}_LL59D:
# 3975
 goto _LL59F;_LL59E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp9A2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp981;if(_tmp9A2->tag != 7)goto _LL5A0;}_LL59F:
 goto _LL585;_LL5A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9A3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp981;if(_tmp9A3->tag != 8)goto _LL5A2;else{_tmp9A4=(_tmp9A3->f1).elt_type;_tmp9A5=(struct Cyc_Absyn_Tqual*)&(_tmp9A3->f1).tq;_tmp9A6=(struct Cyc_Absyn_Exp**)&(_tmp9A3->f1).num_elts;_tmp9A7=(_tmp9A3->f1).zero_term;_tmp9A8=(_tmp9A3->f1).zt_loc;}}_LL5A1: {
# 3980
struct Cyc_Absyn_Exp*_tmpA76=*_tmp9A6;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp9A4,1);
_tmp9A5->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9A5->print_const,_tmp9A4);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmpA77=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp9A7);union Cyc_Absyn_Constraint*_tmpA78=_tmpA77;_LL615: if((_tmpA78->No_constr).tag != 3)goto _LL617;_LL616:
# 3987
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp9A7,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4002 "tcutil.cyc"
goto _LL614;_LL617: if((_tmpA78->Eq_constr).tag != 1)goto _LL619;if((int)(_tmpA78->Eq_constr).val != 1)goto _LL619;_LL618:
# 4005
 if(!Cyc_Tcutil_admits_zero(_tmp9A4)){
const char*_tmp125D;void*_tmp125C[1];struct Cyc_String_pa_PrintArg_struct _tmp125B;(_tmp125B.tag=0,((_tmp125B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9A4)),((_tmp125C[0]=& _tmp125B,Cyc_Tcutil_terr(loc,((_tmp125D="cannot have a zero-terminated array of %s elements",_tag_dyneither(_tmp125D,sizeof(char),51))),_tag_dyneither(_tmp125C,sizeof(void*),1)))))));}
is_zero_terminated=1;
goto _LL614;_LL619:;_LL61A:
# 4011
 is_zero_terminated=0;
goto _LL614;_LL614:;}
# 4016
if(_tmpA76 == 0){
# 4018
if(is_zero_terminated)
# 4020
*_tmp9A6=(_tmpA76=Cyc_Absyn_uint_exp(1,0));else{
# 4023
{const char*_tmp1260;void*_tmp125F;(_tmp125F=0,Cyc_Tcutil_warn(loc,((_tmp1260="array bound defaults to 1 here",_tag_dyneither(_tmp1260,sizeof(char),31))),_tag_dyneither(_tmp125F,sizeof(void*),0)));}
*_tmp9A6=(_tmpA76=Cyc_Absyn_uint_exp(1,0));}}{
# 4027
struct _RegionHandle _tmpA7E=_new_region("temp");struct _RegionHandle*temp=& _tmpA7E;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpA7F=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpA7F,0,_tmpA76);}
# 4031
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmpA76)){
const char*_tmp1263;void*_tmp1262;(_tmp1262=0,Cyc_Tcutil_terr(loc,((_tmp1263="array bounds expression is not an unsigned int",_tag_dyneither(_tmp1263,sizeof(char),47))),_tag_dyneither(_tmp1262,sizeof(void*),0)));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpA76,te,cvtenv);{
# 4038
struct _tuple14 _tmpA82=Cyc_Evexp_eval_const_uint_exp(_tmpA76);unsigned int _tmpA84;int _tmpA85;struct _tuple14 _tmpA83=_tmpA82;_tmpA84=_tmpA83.f1;_tmpA85=_tmpA83.f2;
# 4040
if((is_zero_terminated  && _tmpA85) && _tmpA84 < 1){
const char*_tmp1266;void*_tmp1265;(_tmp1265=0,Cyc_Tcutil_warn(loc,((_tmp1266="zero terminated array cannot have zero elements",_tag_dyneither(_tmp1266,sizeof(char),48))),_tag_dyneither(_tmp1265,sizeof(void*),0)));}
# 4043
if((_tmpA85  && _tmpA84 < 1) && Cyc_Cyclone_tovc_r){
{const char*_tmp1269;void*_tmp1268;(_tmp1268=0,Cyc_Tcutil_warn(loc,((_tmp1269="arrays with 0 elements are not supported except with gcc -- changing to 1.",_tag_dyneither(_tmp1269,sizeof(char),75))),_tag_dyneither(_tmp1268,sizeof(void*),0)));}
*_tmp9A6=Cyc_Absyn_uint_exp(1,0);}
# 4047
_npop_handler(0);goto _LL585;};
# 4027
;_pop_region(temp);};};}_LL5A2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9A9=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp981;if(_tmp9A9->tag != 9)goto _LL5A4;else{_tmp9AA=(struct Cyc_List_List**)&(_tmp9A9->f1).tvars;_tmp9AB=(void**)&(_tmp9A9->f1).effect;_tmp9AC=(struct Cyc_Absyn_Tqual*)&(_tmp9A9->f1).ret_tqual;_tmp9AD=(_tmp9A9->f1).ret_typ;_tmp9AE=(_tmp9A9->f1).args;_tmp9AF=(_tmp9A9->f1).c_varargs;_tmp9B0=(_tmp9A9->f1).cyc_varargs;_tmp9B1=(_tmp9A9->f1).rgn_po;_tmp9B2=(_tmp9A9->f1).attributes;}}_LL5A3: {
# 4054
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp9B2 != 0;_tmp9B2=_tmp9B2->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp9B2->hd)){
const char*_tmp126D;void*_tmp126C[1];struct Cyc_String_pa_PrintArg_struct _tmp126B;(_tmp126B.tag=0,((_tmp126B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp9B2->hd)),((_tmp126C[0]=& _tmp126B,Cyc_Tcutil_terr(loc,((_tmp126D="bad function type attribute %s",_tag_dyneither(_tmp126D,sizeof(char),31))),_tag_dyneither(_tmp126C,sizeof(void*),1)))))));}{
void*_tmpA8D=(void*)_tmp9B2->hd;void*_tmpA8E=_tmpA8D;enum Cyc_Absyn_Format_Type _tmpA93;int _tmpA94;int _tmpA95;_LL61C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpA8F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA8E;if(_tmpA8F->tag != 1)goto _LL61E;}_LL61D:
# 4067
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL61B;_LL61E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpA90=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA8E;if(_tmpA90->tag != 2)goto _LL620;}_LL61F:
# 4069
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL61B;_LL620: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpA91=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA8E;if(_tmpA91->tag != 3)goto _LL622;}_LL621:
# 4071
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL61B;_LL622: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmpA92=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmpA8E;if(_tmpA92->tag != 19)goto _LL624;else{_tmpA93=_tmpA92->f1;_tmpA94=_tmpA92->f2;_tmpA95=_tmpA92->f3;}}_LL623:
# 4073
 if(!seen_format){
seen_format=1;
ft=_tmpA93;
fmt_desc_arg=_tmpA94;
fmt_arg_start=_tmpA95;}else{
# 4079
const char*_tmp1270;void*_tmp126F;(_tmp126F=0,Cyc_Tcutil_terr(loc,((_tmp1270="function can't have multiple format attributes",_tag_dyneither(_tmp1270,sizeof(char),47))),_tag_dyneither(_tmp126F,sizeof(void*),0)));}
goto _LL61B;_LL624:;_LL625:
 goto _LL61B;_LL61B:;};}
# 4084
if(num_convs > 1){
const char*_tmp1273;void*_tmp1272;(_tmp1272=0,Cyc_Tcutil_terr(loc,((_tmp1273="function can't have multiple calling conventions",_tag_dyneither(_tmp1273,sizeof(char),49))),_tag_dyneither(_tmp1272,sizeof(void*),0)));}
# 4089
Cyc_Tcutil_check_unique_tvars(loc,*_tmp9AA);
{struct Cyc_List_List*b=*_tmp9AA;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmpA9A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmpA9B=_tmpA9A;_LL627: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA9B;if(_tmpA9C->tag != 0)goto _LL629;else{if((_tmpA9C->f1)->kind != Cyc_Absyn_MemKind)goto _LL629;}}_LL628:
# 4095
{const char*_tmp1277;void*_tmp1276[1];struct Cyc_String_pa_PrintArg_struct _tmp1275;(_tmp1275.tag=0,((_tmp1275.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name),((_tmp1276[0]=& _tmp1275,Cyc_Tcutil_terr(loc,((_tmp1277="function attempts to abstract Mem type variable %s",_tag_dyneither(_tmp1277,sizeof(char),51))),_tag_dyneither(_tmp1276,sizeof(void*),1)))))));}
goto _LL626;_LL629:;_LL62A:
 goto _LL626;_LL626:;};}}{
# 4103
struct _RegionHandle _tmpAA0=_new_region("newr");struct _RegionHandle*newr=& _tmpAA0;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp1278;struct Cyc_Tcutil_CVTEnv _tmpAA1=
(_tmp1278.r=newr,((_tmp1278.kind_env=cvtenv.kind_env,((_tmp1278.free_vars=0,((_tmp1278.free_evars=0,((_tmp1278.generalize_evars=cvtenv.generalize_evars,((_tmp1278.fn_result=1,_tmp1278)))))))))));
# 4109
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_tmk,_tmp9AD,1);
_tmp9AC->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp9AC->print_const,_tmp9AD);
_tmpAA1.fn_result=0;
{struct Cyc_List_List*a=_tmp9AE;for(0;a != 0;a=a->tl){
struct _tuple10*_tmpAA2=(struct _tuple10*)a->hd;
void*_tmpAA3=(*_tmpAA2).f3;
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_tmk,_tmpAA3,1);{
int _tmpAA4=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpAA2).f2).print_const,_tmpAA3);
((*_tmpAA2).f2).real_const=_tmpAA4;
# 4120
if(Cyc_Tcutil_is_array(_tmpAA3)){
# 4123
void*_tmpAA5=Cyc_Absyn_new_evar(0,0);
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_rk,_tmpAA5,1);
(*_tmpAA2).f3=Cyc_Tcutil_promote_array(_tmpAA3,_tmpAA5,0);}};}}
# 4130
if(_tmp9B0 != 0){
if(_tmp9AF){const char*_tmp127B;void*_tmp127A;(_tmp127A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp127B="both c_vararg and cyc_vararg",_tag_dyneither(_tmp127B,sizeof(char),29))),_tag_dyneither(_tmp127A,sizeof(void*),0)));}{
struct Cyc_Absyn_VarargInfo _tmpAA8=*_tmp9B0;void*_tmpAAA;int _tmpAAB;struct Cyc_Absyn_VarargInfo _tmpAA9=_tmpAA8;_tmpAAA=_tmpAA9.type;_tmpAAB=_tmpAA9.inject;
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_tmk,_tmpAAA,1);
(_tmp9B0->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp9B0->tq).print_const,_tmpAAA);
# 4136
if(_tmpAAB){
void*_tmpAAC=Cyc_Tcutil_compress(_tmpAAA);void*_tmpAAD=_tmpAAC;void*_tmpAAF;union Cyc_Absyn_Constraint*_tmpAB0;union Cyc_Absyn_Constraint*_tmpAB1;_LL62C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAAE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAAD;if(_tmpAAE->tag != 5)goto _LL62E;else{_tmpAAF=(_tmpAAE->f1).elt_typ;_tmpAB0=((_tmpAAE->f1).ptr_atts).bounds;_tmpAB1=((_tmpAAE->f1).ptr_atts).zero_term;}}_LL62D:
# 4139
{void*_tmpAB2=Cyc_Tcutil_compress(_tmpAAF);void*_tmpAB3=_tmpAB2;_LL631: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAB4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAB3;if(_tmpAB4->tag != 3)goto _LL633;}_LL632:
# 4141
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmpAB1)){
const char*_tmp127E;void*_tmp127D;(_tmp127D=0,Cyc_Tcutil_terr(loc,((_tmp127E="can't inject into a zeroterm pointer",_tag_dyneither(_tmp127E,sizeof(char),37))),_tag_dyneither(_tmp127D,sizeof(void*),0)));}
{void*_tmpAB7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmpAB0);void*_tmpAB8=_tmpAB7;_LL636: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpAB9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB8;if(_tmpAB9->tag != 0)goto _LL638;}_LL637:
# 4145
{const char*_tmp1281;void*_tmp1280;(_tmp1280=0,Cyc_Tcutil_terr(loc,((_tmp1281="can't inject into a fat pointer to datatype",_tag_dyneither(_tmp1281,sizeof(char),44))),_tag_dyneither(_tmp1280,sizeof(void*),0)));}
goto _LL635;_LL638:;_LL639:
 goto _LL635;_LL635:;}
# 4149
goto _LL630;_LL633:;_LL634:
{const char*_tmp1284;void*_tmp1283;(_tmp1283=0,Cyc_Tcutil_terr(loc,((_tmp1284="can't inject a non-datatype type",_tag_dyneither(_tmp1284,sizeof(char),33))),_tag_dyneither(_tmp1283,sizeof(void*),0)));}goto _LL630;_LL630:;}
# 4152
goto _LL62B;_LL62E:;_LL62F:
{const char*_tmp1287;void*_tmp1286;(_tmp1286=0,Cyc_Tcutil_terr(loc,((_tmp1287="expecting a datatype pointer type",_tag_dyneither(_tmp1287,sizeof(char),34))),_tag_dyneither(_tmp1286,sizeof(void*),0)));}goto _LL62B;_LL62B:;}};}
# 4158
if(seen_format){
int _tmpAC0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9AE);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmpAC0) || fmt_arg_start < 0) || 
# 4162
_tmp9B0 == 0  && fmt_arg_start != 0) || 
_tmp9B0 != 0  && fmt_arg_start != _tmpAC0 + 1){
const char*_tmp128A;void*_tmp1289;(_tmp1289=0,Cyc_Tcutil_terr(loc,((_tmp128A="bad format descriptor",_tag_dyneither(_tmp128A,sizeof(char),22))),_tag_dyneither(_tmp1289,sizeof(void*),0)));}else{
# 4167
struct _tuple10 _tmpAC3=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp9AE,fmt_desc_arg - 1);void*_tmpAC5;struct _tuple10 _tmpAC4=_tmpAC3;_tmpAC5=_tmpAC4.f3;
# 4169
{void*_tmpAC6=Cyc_Tcutil_compress(_tmpAC5);void*_tmpAC7=_tmpAC6;void*_tmpAC9;union Cyc_Absyn_Constraint*_tmpACA;union Cyc_Absyn_Constraint*_tmpACB;_LL63B: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC7;if(_tmpAC8->tag != 5)goto _LL63D;else{_tmpAC9=(_tmpAC8->f1).elt_typ;_tmpACA=((_tmpAC8->f1).ptr_atts).bounds;_tmpACB=((_tmpAC8->f1).ptr_atts).zero_term;}}_LL63C:
# 4172
{struct _tuple0 _tmp128B;struct _tuple0 _tmpACC=(_tmp128B.f1=Cyc_Tcutil_compress(_tmpAC9),((_tmp128B.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpACA),_tmp128B)));struct _tuple0 _tmpACD=_tmpACC;_LL640:{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpACE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpACD.f1;if(_tmpACE->tag != 6)goto _LL642;else{if(_tmpACE->f1 != Cyc_Absyn_None)goto _LL642;if(_tmpACE->f2 != Cyc_Absyn_Char_sz)goto _LL642;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpACF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpACD.f2;if(_tmpACF->tag != 0)goto _LL642;};_LL641:
 goto _LL63F;_LL642:;_LL643:
{const char*_tmp128E;void*_tmp128D;(_tmp128D=0,Cyc_Tcutil_terr(loc,((_tmp128E="format descriptor is not a char ? type",_tag_dyneither(_tmp128E,sizeof(char),39))),_tag_dyneither(_tmp128D,sizeof(void*),0)));}goto _LL63F;_LL63F:;}
# 4176
goto _LL63A;_LL63D:;_LL63E:
{const char*_tmp1291;void*_tmp1290;(_tmp1290=0,Cyc_Tcutil_terr(loc,((_tmp1291="format descriptor is not a char ? type",_tag_dyneither(_tmp1291,sizeof(char),39))),_tag_dyneither(_tmp1290,sizeof(void*),0)));}goto _LL63A;_LL63A:;}
# 4179
if(fmt_arg_start != 0){
# 4183
int problem;
{struct _tuple30 _tmp1292;struct _tuple30 _tmpAD5=(_tmp1292.f1=ft,((_tmp1292.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp9B0))->type)),_tmp1292)));struct _tuple30 _tmpAD6=_tmpAD5;struct Cyc_Absyn_Datatypedecl*_tmpAD8;struct Cyc_Absyn_Datatypedecl*_tmpADA;_LL645: if(_tmpAD6.f1 != Cyc_Absyn_Printf_ft)goto _LL647;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAD7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAD6.f2;if(_tmpAD7->tag != 3)goto _LL647;else{if((((_tmpAD7->f1).datatype_info).KnownDatatype).tag != 2)goto _LL647;_tmpAD8=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpAD7->f1).datatype_info).KnownDatatype).val);}};_LL646:
# 4186
 problem=Cyc_Absyn_qvar_cmp(_tmpAD8->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL644;_LL647: if(_tmpAD6.f1 != Cyc_Absyn_Scanf_ft)goto _LL649;{struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpAD9=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpAD6.f2;if(_tmpAD9->tag != 3)goto _LL649;else{if((((_tmpAD9->f1).datatype_info).KnownDatatype).tag != 2)goto _LL649;_tmpADA=*((struct Cyc_Absyn_Datatypedecl**)(((_tmpAD9->f1).datatype_info).KnownDatatype).val);}};_LL648:
# 4188
 problem=Cyc_Absyn_qvar_cmp(_tmpADA->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL644;_LL649:;_LL64A:
# 4190
 problem=1;goto _LL644;_LL644:;}
# 4192
if(problem){
const char*_tmp1295;void*_tmp1294;(_tmp1294=0,Cyc_Tcutil_terr(loc,((_tmp1295="format attribute and vararg types don't match",_tag_dyneither(_tmp1295,sizeof(char),46))),_tag_dyneither(_tmp1294,sizeof(void*),0)));}}}}
# 4200
{struct Cyc_List_List*rpo=_tmp9B1;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmpADE=(struct _tuple0*)rpo->hd;void*_tmpAE0;void*_tmpAE1;struct _tuple0*_tmpADF=_tmpADE;_tmpAE0=_tmpADF->f1;_tmpAE1=_tmpADF->f2;
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_ek,_tmpAE0,1);
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_trk,_tmpAE1,1);}}
# 4206
if(*_tmp9AB != 0)
_tmpAA1=Cyc_Tcutil_i_check_valid_type(loc,te,_tmpAA1,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp9AB),1);else{
# 4209
struct Cyc_List_List*effect=0;
# 4214
{struct Cyc_List_List*tvs=_tmpAA1.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpAE2=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpAE4;int _tmpAE5;struct _tuple28 _tmpAE3=_tmpAE2;_tmpAE4=_tmpAE3.f1;_tmpAE5=_tmpAE3.f2;
if(!_tmpAE5)continue;{
void*_tmpAE6=Cyc_Absyn_compress_kb(_tmpAE4->kind);void*_tmpAE7=_tmpAE6;struct Cyc_Core_Opt**_tmpAE9;struct Cyc_Absyn_Kind*_tmpAEA;struct Cyc_Absyn_Kind*_tmpAEC;struct Cyc_Core_Opt**_tmpAF0;struct Cyc_Core_Opt**_tmpAF3;_LL64C:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAE8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAE8->tag != 2)goto _LL64E;else{_tmpAE9=(struct Cyc_Core_Opt**)& _tmpAE8->f1;_tmpAEA=_tmpAE8->f2;}}if(!(_tmpAEA->kind == Cyc_Absyn_RgnKind))goto _LL64E;_LL64D:
# 4220
 if(_tmpAEA->aliasqual == Cyc_Absyn_Top){
*_tmpAE9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmpAEC=_tmpAEA;goto _LL64F;}
# 4223
*_tmpAE9=Cyc_Tcutil_kind_to_bound_opt(_tmpAEA);_tmpAEC=_tmpAEA;goto _LL64F;_LL64E:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAEB=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAEB->tag != 0)goto _LL650;else{_tmpAEC=_tmpAEB->f1;}}if(!(_tmpAEC->kind == Cyc_Absyn_RgnKind))goto _LL650;_LL64F:
# 4225
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp12A4;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12A3;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12A2;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp12A1;struct Cyc_List_List*_tmp12A0;effect=((_tmp12A0=_cycalloc(sizeof(*_tmp12A0)),((_tmp12A0->hd=(void*)((_tmp12A4=_cycalloc(sizeof(*_tmp12A4)),((_tmp12A4[0]=((_tmp12A1.tag=23,((_tmp12A1.f1=(void*)((_tmp12A3=_cycalloc(sizeof(*_tmp12A3)),((_tmp12A3[0]=((_tmp12A2.tag=2,((_tmp12A2.f1=_tmpAE4,_tmp12A2)))),_tmp12A3)))),_tmp12A1)))),_tmp12A4)))),((_tmp12A0->tl=effect,_tmp12A0))))));}goto _LL64B;_LL650: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAED=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAED->tag != 2)goto _LL652;else{if((_tmpAED->f2)->kind != Cyc_Absyn_IntKind)goto _LL652;}}_LL651:
 goto _LL653;_LL652: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAEE=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAEE->tag != 0)goto _LL654;else{if((_tmpAEE->f1)->kind != Cyc_Absyn_IntKind)goto _LL654;}}_LL653:
 goto _LL64B;_LL654: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAEF=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAEF->tag != 2)goto _LL656;else{_tmpAF0=(struct Cyc_Core_Opt**)& _tmpAEF->f1;if((_tmpAEF->f2)->kind != Cyc_Absyn_EffKind)goto _LL656;}}_LL655:
# 4229
*_tmpAF0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL657;_LL656: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAF1=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAF1->tag != 0)goto _LL658;else{if((_tmpAF1->f1)->kind != Cyc_Absyn_EffKind)goto _LL658;}}_LL657:
# 4231
{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12AA;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12A9;struct Cyc_List_List*_tmp12A8;effect=((_tmp12A8=_cycalloc(sizeof(*_tmp12A8)),((_tmp12A8->hd=(void*)((_tmp12AA=_cycalloc(sizeof(*_tmp12AA)),((_tmp12AA[0]=((_tmp12A9.tag=2,((_tmp12A9.f1=_tmpAE4,_tmp12A9)))),_tmp12AA)))),((_tmp12A8->tl=effect,_tmp12A8))))));}goto _LL64B;_LL658: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpAF2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAE7;if(_tmpAF2->tag != 1)goto _LL65A;else{_tmpAF3=(struct Cyc_Core_Opt**)& _tmpAF2->f1;}}_LL659:
# 4233
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp12B0;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp12AF;struct Cyc_Core_Opt*_tmp12AE;*_tmpAF3=((_tmp12AE=_cycalloc(sizeof(*_tmp12AE)),((_tmp12AE->v=(void*)((_tmp12B0=_cycalloc(sizeof(*_tmp12B0)),((_tmp12B0[0]=((_tmp12AF.tag=2,((_tmp12AF.f1=0,((_tmp12AF.f2=& Cyc_Tcutil_ak,_tmp12AF)))))),_tmp12B0)))),_tmp12AE))));}goto _LL65B;_LL65A:;_LL65B:
# 4236
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp12BF;struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp12BE;struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp12BD;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp12BC;struct Cyc_List_List*_tmp12BB;effect=((_tmp12BB=_cycalloc(sizeof(*_tmp12BB)),((_tmp12BB->hd=(void*)((_tmp12BF=_cycalloc(sizeof(*_tmp12BF)),((_tmp12BF[0]=((_tmp12BC.tag=25,((_tmp12BC.f1=(void*)((_tmp12BE=_cycalloc(sizeof(*_tmp12BE)),((_tmp12BE[0]=((_tmp12BD.tag=2,((_tmp12BD.f1=_tmpAE4,_tmp12BD)))),_tmp12BE)))),_tmp12BC)))),_tmp12BF)))),((_tmp12BB->tl=effect,_tmp12BB))))));}goto _LL64B;_LL64B:;};}}
# 4240
{struct Cyc_List_List*ts=_tmpAA1.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmpB04=*((struct _tuple29*)ts->hd);void*_tmpB06;int _tmpB07;struct _tuple29 _tmpB05=_tmpB04;_tmpB06=_tmpB05.f1;_tmpB07=_tmpB05.f2;
if(!_tmpB07)continue;{
struct Cyc_Absyn_Kind*_tmpB08=Cyc_Tcutil_typ_kind(_tmpB06);struct Cyc_Absyn_Kind*_tmpB09=_tmpB08;_LL65D: if(_tmpB09->kind != Cyc_Absyn_RgnKind)goto _LL65F;_LL65E:
# 4245
{struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp12C5;struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp12C4;struct Cyc_List_List*_tmp12C3;effect=((_tmp12C3=_cycalloc(sizeof(*_tmp12C3)),((_tmp12C3->hd=(void*)((_tmp12C5=_cycalloc(sizeof(*_tmp12C5)),((_tmp12C5[0]=((_tmp12C4.tag=23,((_tmp12C4.f1=_tmpB06,_tmp12C4)))),_tmp12C5)))),((_tmp12C3->tl=effect,_tmp12C3))))));}goto _LL65C;_LL65F: if(_tmpB09->kind != Cyc_Absyn_EffKind)goto _LL661;_LL660:
# 4247
{struct Cyc_List_List*_tmp12C6;effect=((_tmp12C6=_cycalloc(sizeof(*_tmp12C6)),((_tmp12C6->hd=_tmpB06,((_tmp12C6->tl=effect,_tmp12C6))))));}goto _LL65C;_LL661: if(_tmpB09->kind != Cyc_Absyn_IntKind)goto _LL663;_LL662:
 goto _LL65C;_LL663:;_LL664:
# 4250
{struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp12CC;struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp12CB;struct Cyc_List_List*_tmp12CA;effect=((_tmp12CA=_cycalloc(sizeof(*_tmp12CA)),((_tmp12CA->hd=(void*)((_tmp12CC=_cycalloc(sizeof(*_tmp12CC)),((_tmp12CC[0]=((_tmp12CB.tag=25,((_tmp12CB.f1=_tmpB06,_tmp12CB)))),_tmp12CC)))),((_tmp12CA->tl=effect,_tmp12CA))))));}goto _LL65C;_LL65C:;};}}{
# 4253
struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp12CF;struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp12CE;*_tmp9AB=(void*)((_tmp12CE=_cycalloc(sizeof(*_tmp12CE)),((_tmp12CE[0]=((_tmp12CF.tag=24,((_tmp12CF.f1=effect,_tmp12CF)))),_tmp12CE))));};}
# 4259
if(*_tmp9AA != 0){
struct Cyc_List_List*bs=*_tmp9AA;for(0;bs != 0;bs=bs->tl){
void*_tmpB13=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmpB14=_tmpB13;struct Cyc_Core_Opt**_tmpB16;struct Cyc_Core_Opt**_tmpB18;struct Cyc_Core_Opt**_tmpB1A;struct Cyc_Core_Opt**_tmpB1C;struct Cyc_Core_Opt**_tmpB1E;struct Cyc_Core_Opt**_tmpB20;struct Cyc_Core_Opt**_tmpB22;struct Cyc_Core_Opt**_tmpB24;struct Cyc_Core_Opt**_tmpB26;struct Cyc_Core_Opt**_tmpB28;struct Cyc_Absyn_Kind*_tmpB29;_LL666: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB15=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB15->tag != 1)goto _LL668;else{_tmpB16=(struct Cyc_Core_Opt**)& _tmpB15->f1;}}_LL667:
# 4263
{const char*_tmp12D3;void*_tmp12D2[1];struct Cyc_String_pa_PrintArg_struct _tmp12D1;(_tmp12D1.tag=0,((_tmp12D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name),((_tmp12D2[0]=& _tmp12D1,Cyc_Tcutil_warn(loc,((_tmp12D3="Type variable %s unconstrained, assuming boxed",_tag_dyneither(_tmp12D3,sizeof(char),47))),_tag_dyneither(_tmp12D2,sizeof(void*),1)))))));}
# 4265
_tmpB18=_tmpB16;goto _LL669;_LL668: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB17=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB17->tag != 2)goto _LL66A;else{_tmpB18=(struct Cyc_Core_Opt**)& _tmpB17->f1;if((_tmpB17->f2)->kind != Cyc_Absyn_BoxKind)goto _LL66A;if((_tmpB17->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66A;}}_LL669:
 _tmpB1A=_tmpB18;goto _LL66B;_LL66A: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB19=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB19->tag != 2)goto _LL66C;else{_tmpB1A=(struct Cyc_Core_Opt**)& _tmpB19->f1;if((_tmpB19->f2)->kind != Cyc_Absyn_MemKind)goto _LL66C;if((_tmpB19->f2)->aliasqual != Cyc_Absyn_Top)goto _LL66C;}}_LL66B:
 _tmpB1C=_tmpB1A;goto _LL66D;_LL66C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB1B->tag != 2)goto _LL66E;else{_tmpB1C=(struct Cyc_Core_Opt**)& _tmpB1B->f1;if((_tmpB1B->f2)->kind != Cyc_Absyn_MemKind)goto _LL66E;if((_tmpB1B->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL66E;}}_LL66D:
 _tmpB1E=_tmpB1C;goto _LL66F;_LL66E: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB1D->tag != 2)goto _LL670;else{_tmpB1E=(struct Cyc_Core_Opt**)& _tmpB1D->f1;if((_tmpB1D->f2)->kind != Cyc_Absyn_AnyKind)goto _LL670;if((_tmpB1D->f2)->aliasqual != Cyc_Absyn_Top)goto _LL670;}}_LL66F:
 _tmpB20=_tmpB1E;goto _LL671;_LL670: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB1F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB1F->tag != 2)goto _LL672;else{_tmpB20=(struct Cyc_Core_Opt**)& _tmpB1F->f1;if((_tmpB1F->f2)->kind != Cyc_Absyn_AnyKind)goto _LL672;if((_tmpB1F->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL672;}}_LL671:
# 4271
*_tmpB20=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL665;_LL672: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB21=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB21->tag != 2)goto _LL674;else{_tmpB22=(struct Cyc_Core_Opt**)& _tmpB21->f1;if((_tmpB21->f2)->kind != Cyc_Absyn_MemKind)goto _LL674;if((_tmpB21->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL674;}}_LL673:
 _tmpB24=_tmpB22;goto _LL675;_LL674: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB23=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB23->tag != 2)goto _LL676;else{_tmpB24=(struct Cyc_Core_Opt**)& _tmpB23->f1;if((_tmpB23->f2)->kind != Cyc_Absyn_AnyKind)goto _LL676;if((_tmpB23->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL676;}}_LL675:
# 4274
*_tmpB24=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL665;_LL676: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB25=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB25->tag != 2)goto _LL678;else{_tmpB26=(struct Cyc_Core_Opt**)& _tmpB25->f1;if((_tmpB25->f2)->kind != Cyc_Absyn_RgnKind)goto _LL678;if((_tmpB25->f2)->aliasqual != Cyc_Absyn_Top)goto _LL678;}}_LL677:
# 4276
*_tmpB26=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL665;_LL678: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpB27=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB27->tag != 2)goto _LL67A;else{_tmpB28=(struct Cyc_Core_Opt**)& _tmpB27->f1;_tmpB29=_tmpB27->f2;}}_LL679:
# 4278
*_tmpB28=Cyc_Tcutil_kind_to_bound_opt(_tmpB29);goto _LL665;_LL67A: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpB2A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpB14;if(_tmpB2A->tag != 0)goto _LL67C;else{if((_tmpB2A->f1)->kind != Cyc_Absyn_MemKind)goto _LL67C;}}_LL67B:
# 4280
{const char*_tmp12D6;void*_tmp12D5;(_tmp12D5=0,Cyc_Tcutil_terr(loc,((_tmp12D6="functions can't abstract M types",_tag_dyneither(_tmp12D6,sizeof(char),33))),_tag_dyneither(_tmp12D5,sizeof(void*),0)));}goto _LL665;_LL67C:;_LL67D:
 goto _LL665;_LL665:;}}
# 4285
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmpAA1.kind_env,*_tmp9AA);
_tmpAA1.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmpAA1.r,_tmpAA1.free_vars,*_tmp9AA);
# 4288
{struct Cyc_List_List*tvs=_tmpAA1.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmpB30=*((struct _tuple28*)tvs->hd);struct Cyc_Absyn_Tvar*_tmpB32;int _tmpB33;struct _tuple28 _tmpB31=_tmpB30;_tmpB32=_tmpB31.f1;_tmpB33=_tmpB31.f2;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB32,_tmpB33);}}{
# 4293
struct Cyc_List_List*evs=_tmpAA1.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpB34=*((struct _tuple29*)evs->hd);void*_tmpB36;int _tmpB37;struct _tuple29 _tmpB35=_tmpB34;_tmpB36=_tmpB35.f1;_tmpB37=_tmpB35.f2;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmpB36,_tmpB37);}};}
# 4298
_npop_handler(0);goto _LL585;
# 4103
;_pop_region(newr);};}_LL5A4: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9B3=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp981;if(_tmp9B3->tag != 10)goto _LL5A6;else{_tmp9B4=_tmp9B3->f1;}}_LL5A5:
# 4301
 for(0;_tmp9B4 != 0;_tmp9B4=_tmp9B4->tl){
struct _tuple12*_tmpB39=(struct _tuple12*)_tmp9B4->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmpB39).f2,1);
((*_tmpB39).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmpB39).f1).print_const,(*_tmpB39).f2);}
# 4307
goto _LL585;_LL5A6: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9B5=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp981;if(_tmp9B5->tag != 12)goto _LL5A8;else{_tmp9B6=_tmp9B5->f1;_tmp9B7=_tmp9B5->f2;}}_LL5A7: {
# 4311
struct _RegionHandle _tmpB3A=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmpB3A;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp9B7 != 0;_tmp9B7=_tmp9B7->tl){
struct Cyc_Absyn_Aggrfield*_tmpB3B=(struct Cyc_Absyn_Aggrfield*)_tmp9B7->hd;struct _dyneither_ptr*_tmpB3D;struct Cyc_Absyn_Tqual*_tmpB3E;void*_tmpB3F;struct Cyc_Absyn_Exp*_tmpB40;struct Cyc_List_List*_tmpB41;struct Cyc_Absyn_Exp*_tmpB42;struct Cyc_Absyn_Aggrfield*_tmpB3C=_tmpB3B;_tmpB3D=_tmpB3C->name;_tmpB3E=(struct Cyc_Absyn_Tqual*)& _tmpB3C->tq;_tmpB3F=_tmpB3C->type;_tmpB40=_tmpB3C->width;_tmpB41=_tmpB3C->attributes;_tmpB42=_tmpB3C->requires_clause;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmpB3D)){
const char*_tmp12DA;void*_tmp12D9[1];struct Cyc_String_pa_PrintArg_struct _tmp12D8;(_tmp12D8.tag=0,((_tmp12D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3D),((_tmp12D9[0]=& _tmp12D8,Cyc_Tcutil_terr(loc,((_tmp12DA="duplicate field %s",_tag_dyneither(_tmp12DA,sizeof(char),19))),_tag_dyneither(_tmp12D9,sizeof(void*),1)))))));}
{const char*_tmp12DB;if(Cyc_strcmp((struct _dyneither_ptr)*_tmpB3D,((_tmp12DB="",_tag_dyneither(_tmp12DB,sizeof(char),1))))!= 0){
struct Cyc_List_List*_tmp12DC;prev_fields=((_tmp12DC=_region_malloc(aprev_rgn,sizeof(*_tmp12DC)),((_tmp12DC->hd=_tmpB3D,((_tmp12DC->tl=prev_fields,_tmp12DC))))));}}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmpB3F,1);
_tmpB3E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmpB3E->print_const,_tmpB3F);
if((_tmp9B6 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmpB3F)) && _tmpB42 == 0){
# 4323
const char*_tmp12E0;void*_tmp12DF[1];struct Cyc_String_pa_PrintArg_struct _tmp12DE;(_tmp12DE.tag=0,((_tmp12DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB3D),((_tmp12DF[0]=& _tmp12DE,Cyc_Tcutil_warn(loc,((_tmp12E0="union member %s is not `bits-only' so it can only be written and not read",_tag_dyneither(_tmp12E0,sizeof(char),74))),_tag_dyneither(_tmp12DF,sizeof(void*),1)))))));}
Cyc_Tcutil_check_bitfield(loc,te,_tmpB3F,_tmpB40,_tmpB3D);
Cyc_Tcutil_check_field_atts(loc,_tmpB3D,_tmpB41);
if(_tmpB42 != 0){
# 4328
if(_tmp9B6 != Cyc_Absyn_UnionA){
const char*_tmp12E3;void*_tmp12E2;(_tmp12E2=0,Cyc_Tcutil_terr(loc,((_tmp12E3="@requires clause is only allowed on union members",_tag_dyneither(_tmp12E3,sizeof(char),50))),_tag_dyneither(_tmp12E2,sizeof(void*),0)));}{
struct _RegionHandle _tmpB4D=_new_region("temp");struct _RegionHandle*temp=& _tmpB4D;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmpB4E=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmpB4E,0,_tmpB42);}
# 4334
if(!Cyc_Tcutil_is_integral(_tmpB42)){
const char*_tmp12E7;void*_tmp12E6[1];struct Cyc_String_pa_PrintArg_struct _tmp12E5;(_tmp12E5.tag=0,((_tmp12E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB42->topt))),((_tmp12E6[0]=& _tmp12E5,Cyc_Tcutil_terr(loc,((_tmp12E7="@requires clause has type %s instead of integral type",_tag_dyneither(_tmp12E7,sizeof(char),54))),_tag_dyneither(_tmp12E6,sizeof(void*),1)))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpB42,te,cvtenv);
# 4330
;_pop_region(temp);};}}}
# 4341
_npop_handler(0);goto _LL585;
# 4311
;_pop_region(aprev_rgn);}_LL5A8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9B8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp981;if(_tmp9B8->tag != 11)goto _LL5AA;else{_tmp9B9=(union Cyc_Absyn_AggrInfoU*)&(_tmp9B8->f1).aggr_info;_tmp9BA=(struct Cyc_List_List**)&(_tmp9B8->f1).targs;}}_LL5A9:
# 4344
{union Cyc_Absyn_AggrInfoU _tmpB52=*_tmp9B9;union Cyc_Absyn_AggrInfoU _tmpB53=_tmpB52;enum Cyc_Absyn_AggrKind _tmpB54;struct _tuple2*_tmpB55;struct Cyc_Core_Opt*_tmpB56;struct Cyc_Absyn_Aggrdecl*_tmpB57;_LL67F: if((_tmpB53.UnknownAggr).tag != 1)goto _LL681;_tmpB54=((struct _tuple4)(_tmpB53.UnknownAggr).val).f1;_tmpB55=((struct _tuple4)(_tmpB53.UnknownAggr).val).f2;_tmpB56=((struct _tuple4)(_tmpB53.UnknownAggr).val).f3;_LL680: {
# 4346
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmpB58;_push_handler(& _tmpB58);{int _tmpB5A=0;if(setjmp(_tmpB58.handler))_tmpB5A=1;if(!_tmpB5A){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpB55);{
struct Cyc_Absyn_Aggrdecl*_tmpB5B=*adp;
if(_tmpB5B->kind != _tmpB54){
if(_tmpB5B->kind == Cyc_Absyn_StructA){
const char*_tmp12EC;void*_tmp12EB[2];struct Cyc_String_pa_PrintArg_struct _tmp12EA;struct Cyc_String_pa_PrintArg_struct _tmp12E9;(_tmp12E9.tag=0,((_tmp12E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12EA.tag=0,((_tmp12EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4352
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12EB[0]=& _tmp12EA,((_tmp12EB[1]=& _tmp12E9,Cyc_Tcutil_terr(loc,((_tmp12EC="expecting struct %s instead of union %s",_tag_dyneither(_tmp12EC,sizeof(char),40))),_tag_dyneither(_tmp12EB,sizeof(void*),2)))))))))))));}else{
# 4355
const char*_tmp12F1;void*_tmp12F0[2];struct Cyc_String_pa_PrintArg_struct _tmp12EF;struct Cyc_String_pa_PrintArg_struct _tmp12EE;(_tmp12EE.tag=0,((_tmp12EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12EF.tag=0,((_tmp12EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4355
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12F0[0]=& _tmp12EF,((_tmp12F0[1]=& _tmp12EE,Cyc_Tcutil_terr(loc,((_tmp12F1="expecting union %s instead of struct %s",_tag_dyneither(_tmp12F1,sizeof(char),40))),_tag_dyneither(_tmp12F0,sizeof(void*),2)))))))))))));}}
# 4358
if((unsigned int)_tmpB56  && (int)_tmpB56->v){
if(!((unsigned int)_tmpB5B->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB5B->impl))->tagged){
const char*_tmp12F5;void*_tmp12F4[1];struct Cyc_String_pa_PrintArg_struct _tmp12F3;(_tmp12F3.tag=0,((_tmp12F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12F4[0]=& _tmp12F3,Cyc_Tcutil_terr(loc,((_tmp12F5="@tagged qualfiers don't agree on union %s",_tag_dyneither(_tmp12F5,sizeof(char),42))),_tag_dyneither(_tmp12F4,sizeof(void*),1)))))));}}
# 4364
*_tmp9B9=Cyc_Absyn_KnownAggr(adp);};
# 4348
;_pop_handler();}else{void*_tmpB59=(void*)_exn_thrown;void*_tmpB68=_tmpB59;void*_tmpB6A;_LL684: {struct Cyc_Dict_Absent_exn_struct*_tmpB69=(struct Cyc_Dict_Absent_exn_struct*)_tmpB68;if(_tmpB69->tag != Cyc_Dict_Absent)goto _LL686;}_LL685: {
# 4368
struct Cyc_Tcenv_Genv*_tmpB6B=((struct Cyc_Tcenv_Genv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(te->ae,te->ns);
struct Cyc_Absyn_Aggrdecl*_tmp12F6;struct Cyc_Absyn_Aggrdecl*_tmpB6C=(_tmp12F6=_cycalloc(sizeof(*_tmp12F6)),((_tmp12F6->kind=_tmpB54,((_tmp12F6->sc=Cyc_Absyn_Extern,((_tmp12F6->name=_tmpB55,((_tmp12F6->tvs=0,((_tmp12F6->impl=0,((_tmp12F6->attributes=0,_tmp12F6)))))))))))));
Cyc_Tc_tcAggrdecl(te,_tmpB6B,loc,_tmpB6C);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmpB55);
*_tmp9B9=Cyc_Absyn_KnownAggr(adp);
# 4374
if(*_tmp9BA != 0){
{const char*_tmp12FA;void*_tmp12F9[1];struct Cyc_String_pa_PrintArg_struct _tmp12F8;(_tmp12F8.tag=0,((_tmp12F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmpB55)),((_tmp12F9[0]=& _tmp12F8,Cyc_Tcutil_terr(loc,((_tmp12FA="declare parameterized type %s before using",_tag_dyneither(_tmp12FA,sizeof(char),43))),_tag_dyneither(_tmp12F9,sizeof(void*),1)))))));}
return cvtenv;}
# 4379
goto _LL683;}_LL686: _tmpB6A=_tmpB68;_LL687:(void)_throw(_tmpB6A);_LL683:;}};}
# 4381
_tmpB57=*adp;goto _LL682;}_LL681: if((_tmpB53.KnownAggr).tag != 2)goto _LL67E;_tmpB57=*((struct Cyc_Absyn_Aggrdecl**)(_tmpB53.KnownAggr).val);_LL682: {
# 4383
struct Cyc_List_List*tvs=_tmpB57->tvs;
struct Cyc_List_List*ts=*_tmp9BA;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmpB71=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmpB72=(void*)ts->hd;
# 4391
{struct _tuple0 _tmp12FB;struct _tuple0 _tmpB73=(_tmp12FB.f1=Cyc_Absyn_compress_kb(_tmpB71->kind),((_tmp12FB.f2=_tmpB72,_tmp12FB)));struct _tuple0 _tmpB74=_tmpB73;struct Cyc_Absyn_Tvar*_tmpB77;_LL689:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpB75=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpB74.f1;if(_tmpB75->tag != 1)goto _LL68B;}{struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpB76=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpB74.f2;if(_tmpB76->tag != 2)goto _LL68B;else{_tmpB77=_tmpB76->f1;}};_LL68A:
# 4393
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmpB77);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmpB77,1);
continue;_LL68B:;_LL68C:
 goto _LL688;_LL688:;}{
# 4398
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4402
if(ts != 0){
const char*_tmp12FF;void*_tmp12FE[1];struct Cyc_String_pa_PrintArg_struct _tmp12FD;(_tmp12FD.tag=0,((_tmp12FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpB57->name)),((_tmp12FE[0]=& _tmp12FD,Cyc_Tcutil_terr(loc,((_tmp12FF="too many parameters for type %s",_tag_dyneither(_tmp12FF,sizeof(char),32))),_tag_dyneither(_tmp12FE,sizeof(void*),1)))))));}
if(tvs != 0){
# 4406
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp1300;hidden_ts=((_tmp1300=_cycalloc(sizeof(*_tmp1300)),((_tmp1300->hd=e,((_tmp1300->tl=hidden_ts,_tmp1300))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);}
# 4413
*_tmp9BA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp9BA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}}_LL67E:;}
# 4416
goto _LL585;_LL5AA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9BB=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp981;if(_tmp9BB->tag != 17)goto _LL5AC;else{_tmp9BC=_tmp9BB->f1;_tmp9BD=(struct Cyc_List_List**)& _tmp9BB->f2;_tmp9BE=(struct Cyc_Absyn_Typedefdecl**)& _tmp9BB->f3;_tmp9BF=(void**)((void**)& _tmp9BB->f4);}}_LL5AB: {
# 4419
struct Cyc_List_List*targs=*_tmp9BD;
# 4421
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmpB7D;_push_handler(& _tmpB7D);{int _tmpB7F=0;if(setjmp(_tmpB7D.handler))_tmpB7F=1;if(!_tmpB7F){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp9BC);;_pop_handler();}else{void*_tmpB7E=(void*)_exn_thrown;void*_tmpB81=_tmpB7E;void*_tmpB83;_LL68E: {struct Cyc_Dict_Absent_exn_struct*_tmpB82=(struct Cyc_Dict_Absent_exn_struct*)_tmpB81;if(_tmpB82->tag != Cyc_Dict_Absent)goto _LL690;}_LL68F:
# 4424
{const char*_tmp1304;void*_tmp1303[1];struct Cyc_String_pa_PrintArg_struct _tmp1302;(_tmp1302.tag=0,((_tmp1302.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BC)),((_tmp1303[0]=& _tmp1302,Cyc_Tcutil_terr(loc,((_tmp1304="unbound typedef name %s",_tag_dyneither(_tmp1304,sizeof(char),24))),_tag_dyneither(_tmp1303,sizeof(void*),1)))))));}
return cvtenv;_LL690: _tmpB83=_tmpB81;_LL691:(void)_throw(_tmpB83);_LL68D:;}};}
# 4427
*_tmp9BE=td;
# 4429
_tmp9BC[0]=(td->name)[0];{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmpB87=_new_region("temp");struct _RegionHandle*temp=& _tmpB87;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4435
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4440
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);{
struct _tuple16*_tmp1307;struct Cyc_List_List*_tmp1306;inst=((_tmp1306=_region_malloc(temp,sizeof(*_tmp1306)),((_tmp1306->hd=((_tmp1307=_region_malloc(temp,sizeof(*_tmp1307)),((_tmp1307->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp1307->f2=(void*)ts->hd,_tmp1307)))))),((_tmp1306->tl=inst,_tmp1306))))));};}
# 4444
if(ts != 0){
const char*_tmp130B;void*_tmp130A[1];struct Cyc_String_pa_PrintArg_struct _tmp1309;(_tmp1309.tag=0,((_tmp1309.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9BC)),((_tmp130A[0]=& _tmp1309,Cyc_Tcutil_terr(loc,((_tmp130B="too many parameters for typedef %s",_tag_dyneither(_tmp130B,sizeof(char),35))),_tag_dyneither(_tmp130A,sizeof(void*),1)))))));}
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4449
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
{struct Cyc_List_List*_tmp130C;hidden_ts=((_tmp130C=_cycalloc(sizeof(*_tmp130C)),((_tmp130C->hd=e,((_tmp130C->tl=hidden_ts,_tmp130C))))));}
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1);{
struct _tuple16*_tmp130F;struct Cyc_List_List*_tmp130E;inst=((_tmp130E=_cycalloc(sizeof(*_tmp130E)),((_tmp130E->hd=((_tmp130F=_cycalloc(sizeof(*_tmp130F)),((_tmp130F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,((_tmp130F->f2=e,_tmp130F)))))),((_tmp130E->tl=inst,_tmp130E))))));};}
# 4457
*_tmp9BD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4459
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp9BF=new_typ;}}
# 4466
_npop_handler(0);goto _LL585;
# 4432
;_pop_region(temp);};}_LL5AC: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp9C0=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp981;if(_tmp9C0->tag != 22)goto _LL5AE;}_LL5AD:
# 4467
 goto _LL5AF;_LL5AE: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp9C1=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp981;if(_tmp9C1->tag != 21)goto _LL5B0;}_LL5AF:
 goto _LL5B1;_LL5B0: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp9C2=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp981;if(_tmp9C2->tag != 20)goto _LL5B2;}_LL5B1:
 goto _LL585;_LL5B2: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp9C3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp981;if(_tmp9C3->tag != 15)goto _LL5B4;else{_tmp9C4=(void*)_tmp9C3->f1;}}_LL5B3:
# 4471
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp9C4,1);goto _LL585;_LL5B4: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp9C5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp981;if(_tmp9C5->tag != 16)goto _LL5B6;else{_tmp9C6=(void*)_tmp9C5->f1;_tmp9C7=(void*)_tmp9C5->f2;}}_LL5B5:
# 4474
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp9C6,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp9C7,1);
goto _LL585;_LL5B6: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp9C8=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp981;if(_tmp9C8->tag != 23)goto _LL5B8;else{_tmp9C9=(void*)_tmp9C8->f1;}}_LL5B7:
# 4478
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp9C9,1);goto _LL585;_LL5B8: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp9CA=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp981;if(_tmp9CA->tag != 25)goto _LL5BA;else{_tmp9CB=(void*)_tmp9CA->f1;}}_LL5B9:
# 4480
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp9CB,1);goto _LL585;_LL5BA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp9CC=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp981;if(_tmp9CC->tag != 24)goto _LL585;else{_tmp9CD=_tmp9CC->f1;}}_LL5BB:
# 4482
 for(0;_tmp9CD != 0;_tmp9CD=_tmp9CD->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp9CD->hd,1);}
goto _LL585;_LL585:;}
# 4486
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind)){
{void*_tmpB90=t;struct Cyc_Core_Opt*_tmpB92;void*_tmpB93;_LL693: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpB91=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpB90;if(_tmpB91->tag != 1)goto _LL695;else{_tmpB92=_tmpB91->f1;_tmpB93=(void*)_tmpB91->f2;}}_LL694: {
# 4489
struct _dyneither_ptr s;
{struct Cyc_Core_Opt*_tmpB94=_tmpB92;struct Cyc_Absyn_Kind*_tmpB95;_LL698: if(_tmpB94 != 0)goto _LL69A;_LL699:
{const char*_tmp1310;s=((_tmp1310="kind=NULL ",_tag_dyneither(_tmp1310,sizeof(char),11)));}goto _LL697;_LL69A: if(_tmpB94 == 0)goto _LL697;_tmpB95=(struct Cyc_Absyn_Kind*)_tmpB94->v;_LL69B:
{const char*_tmp1314;void*_tmp1313[1];struct Cyc_String_pa_PrintArg_struct _tmp1312;s=(struct _dyneither_ptr)((_tmp1312.tag=0,((_tmp1312.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmpB95)),((_tmp1313[0]=& _tmp1312,Cyc_aprintf(((_tmp1314="kind=%s ",_tag_dyneither(_tmp1314,sizeof(char),9))),_tag_dyneither(_tmp1313,sizeof(void*),1))))))));}goto _LL697;_LL697:;}
# 4494
if(_tmpB93 == 0){
const char*_tmp1318;void*_tmp1317[1];struct Cyc_String_pa_PrintArg_struct _tmp1316;s=(struct _dyneither_ptr)((_tmp1316.tag=0,((_tmp1316.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp1317[0]=& _tmp1316,Cyc_aprintf(((_tmp1318="%s ref=NULL",_tag_dyneither(_tmp1318,sizeof(char),12))),_tag_dyneither(_tmp1317,sizeof(void*),1))))))));}else{
# 4497
const char*_tmp131D;void*_tmp131C[2];struct Cyc_String_pa_PrintArg_struct _tmp131B;struct Cyc_String_pa_PrintArg_struct _tmp131A;s=(struct _dyneither_ptr)((_tmp131A.tag=0,((_tmp131A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpB93)),((_tmp131B.tag=0,((_tmp131B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s),((_tmp131C[0]=& _tmp131B,((_tmp131C[1]=& _tmp131A,Cyc_aprintf(((_tmp131D="%s ref=%s",_tag_dyneither(_tmp131D,sizeof(char),10))),_tag_dyneither(_tmp131C,sizeof(void*),2))))))))))))));}
# 4499
goto _LL692;}_LL695:;_LL696:
 goto _LL692;_LL692:;}{
# 4502
const char*_tmp1323;void*_tmp1322[3];struct Cyc_String_pa_PrintArg_struct _tmp1321;struct Cyc_String_pa_PrintArg_struct _tmp1320;struct Cyc_String_pa_PrintArg_struct _tmp131F;(_tmp131F.tag=0,((_tmp131F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind)),((_tmp1320.tag=0,((_tmp1320.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t))),((_tmp1321.tag=0,((_tmp1321.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp1322[0]=& _tmp1321,((_tmp1322[1]=& _tmp1320,((_tmp1322[2]=& _tmp131F,Cyc_Tcutil_terr(loc,((_tmp1323="type %s has kind %s but as used here needs kind %s",_tag_dyneither(_tmp1323,sizeof(char),51))),_tag_dyneither(_tmp1322,sizeof(void*),3)))))))))))))))))));};}
# 4505
return cvtenv;}
# 4513
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4515
{void*_tmpBA6=e->r;void*_tmpBA7=_tmpBA6;struct Cyc_List_List*_tmpBAD;struct Cyc_Absyn_Exp*_tmpBAF;struct Cyc_Absyn_Exp*_tmpBB0;struct Cyc_Absyn_Exp*_tmpBB1;struct Cyc_Absyn_Exp*_tmpBB3;struct Cyc_Absyn_Exp*_tmpBB4;struct Cyc_Absyn_Exp*_tmpBB6;struct Cyc_Absyn_Exp*_tmpBB7;struct Cyc_Absyn_Exp*_tmpBB9;struct Cyc_Absyn_Exp*_tmpBBA;void*_tmpBBC;struct Cyc_Absyn_Exp*_tmpBBD;void*_tmpBBF;void*_tmpBC1;void*_tmpBC3;struct Cyc_Absyn_Exp*_tmpBC5;_LL69D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpBA8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBA8->tag != 0)goto _LL69F;}_LL69E:
 goto _LL6A0;_LL69F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpBA9=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBA9->tag != 31)goto _LL6A1;}_LL6A0:
 goto _LL6A2;_LL6A1: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpBAA=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBAA->tag != 32)goto _LL6A3;}_LL6A2:
 goto _LL6A4;_LL6A3: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpBAB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBAB->tag != 1)goto _LL6A5;}_LL6A4:
 goto _LL69C;_LL6A5: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpBAC=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBAC->tag != 2)goto _LL6A7;else{_tmpBAD=_tmpBAC->f2;}}_LL6A6:
# 4521
 for(0;_tmpBAD != 0;_tmpBAD=_tmpBAD->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmpBAD->hd,te,cvtenv);}
goto _LL69C;_LL6A7: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpBAE=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBAE->tag != 5)goto _LL6A9;else{_tmpBAF=_tmpBAE->f1;_tmpBB0=_tmpBAE->f2;_tmpBB1=_tmpBAE->f3;}}_LL6A8:
# 4525
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBAF,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB0,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB1,te,cvtenv);
goto _LL69C;_LL6A9: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmpBB2=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBB2->tag != 6)goto _LL6AB;else{_tmpBB3=_tmpBB2->f1;_tmpBB4=_tmpBB2->f2;}}_LL6AA:
 _tmpBB6=_tmpBB3;_tmpBB7=_tmpBB4;goto _LL6AC;_LL6AB: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmpBB5=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBB5->tag != 7)goto _LL6AD;else{_tmpBB6=_tmpBB5->f1;_tmpBB7=_tmpBB5->f2;}}_LL6AC:
 _tmpBB9=_tmpBB6;_tmpBBA=_tmpBB7;goto _LL6AE;_LL6AD: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpBB8=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBB8->tag != 8)goto _LL6AF;else{_tmpBB9=_tmpBB8->f1;_tmpBBA=_tmpBB8->f2;}}_LL6AE:
# 4532
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBB9,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBA,te,cvtenv);
goto _LL69C;_LL6AF: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpBBB=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBBB->tag != 13)goto _LL6B1;else{_tmpBBC=(void*)_tmpBBB->f1;_tmpBBD=_tmpBBB->f2;}}_LL6B0:
# 4536
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBBD,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpBBC,1);
goto _LL69C;_LL6B1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpBBE=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBBE->tag != 18)goto _LL6B3;else{_tmpBBF=(void*)_tmpBBE->f1;}}_LL6B2:
 _tmpBC1=_tmpBBF;goto _LL6B4;_LL6B3: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpBC0=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBC0->tag != 16)goto _LL6B5;else{_tmpBC1=(void*)_tmpBC0->f1;}}_LL6B4:
# 4541
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmpBC1,1);
goto _LL69C;_LL6B5: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmpBC2=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBC2->tag != 38)goto _LL6B7;else{_tmpBC3=(void*)_tmpBC2->f1;}}_LL6B6:
# 4544
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmpBC3,1);
goto _LL69C;_LL6B7: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpBC4=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBA7;if(_tmpBC4->tag != 17)goto _LL6B9;else{_tmpBC5=_tmpBC4->f1;}}_LL6B8:
# 4547
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmpBC5,te,cvtenv);
goto _LL69C;_LL6B9:;_LL6BA: {
# 4550
const char*_tmp1326;void*_tmp1325;(_tmp1325=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1326="non-type-level-expression in Tcutil::i_check_valid_type_level_exp",_tag_dyneither(_tmp1326,sizeof(char),66))),_tag_dyneither(_tmp1325,sizeof(void*),0)));}_LL69C:;}
# 4552
return cvtenv;}
# 4555
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,void*t){
# 4560
struct Cyc_List_List*_tmpBC8=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1);
# 4563
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmpBC9=*((struct _tuple28*)vs->hd);struct Cyc_Absyn_Tvar*_tmpBCB;struct _tuple28 _tmpBCA=_tmpBC9;_tmpBCB=_tmpBCA.f1;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmpBC8,_tmpBCB);}}
# 4571
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmpBCC=*((struct _tuple29*)evs->hd);void*_tmpBCE;struct _tuple29 _tmpBCD=_tmpBCC;_tmpBCE=_tmpBCD.f1;{
void*_tmpBCF=Cyc_Tcutil_compress(_tmpBCE);void*_tmpBD0=_tmpBCF;struct Cyc_Core_Opt**_tmpBD2;_LL6BC: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmpBD1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpBD0;if(_tmpBD1->tag != 1)goto _LL6BE;else{_tmpBD2=(struct Cyc_Core_Opt**)& _tmpBD1->f4;}}_LL6BD:
# 4575
 if(*_tmpBD2 == 0){
struct Cyc_Core_Opt*_tmp1327;*_tmpBD2=((_tmp1327=_cycalloc(sizeof(*_tmp1327)),((_tmp1327->v=_tmpBC8,_tmp1327))));}else{
# 4579
struct Cyc_List_List*_tmpBD4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmpBD2))->v;
struct Cyc_List_List*_tmpBD5=0;
for(0;_tmpBD4 != 0;_tmpBD4=_tmpBD4->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmpBC8,(struct Cyc_Absyn_Tvar*)_tmpBD4->hd)){
struct Cyc_List_List*_tmp1328;_tmpBD5=((_tmp1328=_cycalloc(sizeof(*_tmp1328)),((_tmp1328->hd=(struct Cyc_Absyn_Tvar*)_tmpBD4->hd,((_tmp1328->tl=_tmpBD5,_tmp1328))))));}}{
struct Cyc_Core_Opt*_tmp1329;*_tmpBD2=((_tmp1329=_cycalloc(sizeof(*_tmp1329)),((_tmp1329->v=_tmpBD5,_tmp1329))));};}
# 4586
goto _LL6BB;_LL6BE:;_LL6BF:
 goto _LL6BB;_LL6BB:;};}}
# 4590
return cvt;}
# 4596
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4598
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmpBD8=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmpBD9=_tmpBD8;_LL6C1: if(_tmpBD9->kind != Cyc_Absyn_RgnKind)goto _LL6C3;if(_tmpBD9->aliasqual != Cyc_Absyn_Unique)goto _LL6C3;_LL6C2:
# 4602
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val)){
const char*_tmp132C;void*_tmp132B;(_tmp132B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132C="can't unify evar with unique region!",_tag_dyneither(_tmp132C,sizeof(char),37))),_tag_dyneither(_tmp132B,sizeof(void*),0)));}
goto _LL6C0;_LL6C3: if(_tmpBD9->kind != Cyc_Absyn_RgnKind)goto _LL6C5;if(_tmpBD9->aliasqual != Cyc_Absyn_Aliasable)goto _LL6C5;_LL6C4:
 goto _LL6C6;_LL6C5: if(_tmpBD9->kind != Cyc_Absyn_RgnKind)goto _LL6C7;if(_tmpBD9->aliasqual != Cyc_Absyn_Top)goto _LL6C7;_LL6C6:
# 4607
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val)){const char*_tmp132F;void*_tmp132E;(_tmp132E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp132F="can't unify evar with heap!",_tag_dyneither(_tmp132F,sizeof(char),28))),_tag_dyneither(_tmp132E,sizeof(void*),0)));}
goto _LL6C0;_LL6C7: if(_tmpBD9->kind != Cyc_Absyn_EffKind)goto _LL6C9;_LL6C8:
# 4610
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect)){const char*_tmp1332;void*_tmp1331;(_tmp1331=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1332="can't unify evar with {}!",_tag_dyneither(_tmp1332,sizeof(char),26))),_tag_dyneither(_tmp1331,sizeof(void*),0)));}
goto _LL6C0;_LL6C9:;_LL6CA:
# 4613
{const char*_tmp1337;void*_tmp1336[2];struct Cyc_String_pa_PrintArg_struct _tmp1335;struct Cyc_String_pa_PrintArg_struct _tmp1334;(_tmp1334.tag=0,((_tmp1334.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t)),((_tmp1335.tag=0,((_tmp1335.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e)),((_tmp1336[0]=& _tmp1335,((_tmp1336[1]=& _tmp1334,Cyc_Tcutil_terr(loc,((_tmp1337="hidden type variable %s isn't abstracted in type %s",_tag_dyneither(_tmp1337,sizeof(char),52))),_tag_dyneither(_tmp1336,sizeof(void*),2)))))))))))));}
goto _LL6C0;_LL6C0:;}}
# 4625
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmpBE4=Cyc_Tcenv_lookup_type_vars(te);
struct _RegionHandle _tmpBE5=_new_region("temp");struct _RegionHandle*temp=& _tmpBE5;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp1338;struct Cyc_Tcutil_CVTEnv _tmpBE6=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1338.r=temp,((_tmp1338.kind_env=_tmpBE4,((_tmp1338.free_vars=0,((_tmp1338.free_evars=0,((_tmp1338.generalize_evars=generalize_evars,((_tmp1338.fn_result=0,_tmp1338)))))))))))),& Cyc_Tcutil_tmk,t);
# 4633
struct Cyc_List_List*_tmpBE7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpBE6.free_vars);
struct Cyc_List_List*_tmpBE8=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpBE6.free_evars);
# 4637
if(_tmpBE4 != 0){
struct Cyc_List_List*_tmpBE9=0;
{struct Cyc_List_List*_tmpBEA=_tmpBE7;for(0;(unsigned int)_tmpBEA;_tmpBEA=_tmpBEA->tl){
struct Cyc_Absyn_Tvar*_tmpBEB=(struct Cyc_Absyn_Tvar*)_tmpBEA->hd;
int found=0;
{struct Cyc_List_List*_tmpBEC=_tmpBE4;for(0;(unsigned int)_tmpBEC;_tmpBEC=_tmpBEC->tl){
if(Cyc_Absyn_tvar_cmp(_tmpBEB,(struct Cyc_Absyn_Tvar*)_tmpBEC->hd)== 0){found=1;break;}}}
if(!found){
struct Cyc_List_List*_tmp1339;_tmpBE9=((_tmp1339=_region_malloc(temp,sizeof(*_tmp1339)),((_tmp1339->hd=(struct Cyc_Absyn_Tvar*)_tmpBEA->hd,((_tmp1339->tl=_tmpBE9,_tmp1339))))));}}}
# 4647
_tmpBE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpBE9);}
# 4653
{struct Cyc_List_List*x=_tmpBE7;for(0;x != 0;x=x->tl){
void*_tmpBEE=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmpBEF=_tmpBEE;struct Cyc_Core_Opt**_tmpBF1;struct Cyc_Core_Opt**_tmpBF3;struct Cyc_Core_Opt**_tmpBF5;struct Cyc_Core_Opt**_tmpBF7;struct Cyc_Core_Opt**_tmpBF9;struct Cyc_Core_Opt**_tmpBFB;struct Cyc_Core_Opt**_tmpBFD;struct Cyc_Absyn_Kind*_tmpBFE;enum Cyc_Absyn_AliasQual _tmpC00;_LL6CC: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpBF0=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBF0->tag != 1)goto _LL6CE;else{_tmpBF1=(struct Cyc_Core_Opt**)& _tmpBF0->f1;}}_LL6CD:
 _tmpBF3=_tmpBF1;goto _LL6CF;_LL6CE: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBF2->tag != 2)goto _LL6D0;else{_tmpBF3=(struct Cyc_Core_Opt**)& _tmpBF2->f1;if((_tmpBF2->f2)->kind != Cyc_Absyn_BoxKind)goto _LL6D0;if((_tmpBF2->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D0;}}_LL6CF:
 _tmpBF5=_tmpBF3;goto _LL6D1;_LL6D0: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBF4->tag != 2)goto _LL6D2;else{_tmpBF5=(struct Cyc_Core_Opt**)& _tmpBF4->f1;if((_tmpBF4->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D2;if((_tmpBF4->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D2;}}_LL6D1:
 _tmpBF7=_tmpBF5;goto _LL6D3;_LL6D2: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF6=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBF6->tag != 2)goto _LL6D4;else{_tmpBF7=(struct Cyc_Core_Opt**)& _tmpBF6->f1;if((_tmpBF6->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D4;if((_tmpBF6->f2)->aliasqual != Cyc_Absyn_Aliasable)goto _LL6D4;}}_LL6D3:
# 4659
*_tmpBF7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL6CB;_LL6D4: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBF8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBF8->tag != 2)goto _LL6D6;else{_tmpBF9=(struct Cyc_Core_Opt**)& _tmpBF8->f1;if((_tmpBF8->f2)->kind != Cyc_Absyn_MemKind)goto _LL6D6;if((_tmpBF8->f2)->aliasqual != Cyc_Absyn_Unique)goto _LL6D6;}}_LL6D5:
# 4661
*_tmpBF9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL6CB;_LL6D6: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBFA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBFA->tag != 2)goto _LL6D8;else{_tmpBFB=(struct Cyc_Core_Opt**)& _tmpBFA->f1;if((_tmpBFA->f2)->kind != Cyc_Absyn_RgnKind)goto _LL6D8;if((_tmpBFA->f2)->aliasqual != Cyc_Absyn_Top)goto _LL6D8;}}_LL6D7:
# 4663
*_tmpBFB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL6CB;_LL6D8: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpBFC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBFC->tag != 2)goto _LL6DA;else{_tmpBFD=(struct Cyc_Core_Opt**)& _tmpBFC->f1;_tmpBFE=_tmpBFC->f2;}}_LL6D9:
# 4665
*_tmpBFD=Cyc_Tcutil_kind_to_bound_opt(_tmpBFE);goto _LL6CB;_LL6DA: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpBFF=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpBEF;if(_tmpBFF->tag != 0)goto _LL6DC;else{if((_tmpBFF->f1)->kind != Cyc_Absyn_MemKind)goto _LL6DC;_tmpC00=(_tmpBFF->f1)->aliasqual;}}_LL6DB:
# 4667
{const char*_tmp1340;void*_tmp133F[2];struct Cyc_String_pa_PrintArg_struct _tmp133E;struct Cyc_Absyn_Kind*_tmp133D;struct Cyc_String_pa_PrintArg_struct _tmp133C;(_tmp133C.tag=0,((_tmp133C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(((_tmp133D=_cycalloc_atomic(sizeof(*_tmp133D)),((_tmp133D->kind=Cyc_Absyn_MemKind,((_tmp133D->aliasqual=_tmpC00,_tmp133D)))))))),((_tmp133E.tag=0,((_tmp133E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd)),((_tmp133F[0]=& _tmp133E,((_tmp133F[1]=& _tmp133C,Cyc_Tcutil_terr(loc,((_tmp1340="type variable %s cannot have kind %s",_tag_dyneither(_tmp1340,sizeof(char),37))),_tag_dyneither(_tmp133F,sizeof(void*),2)))))))))))));}
goto _LL6CB;_LL6DC:;_LL6DD:
 goto _LL6CB;_LL6CB:;}}
# 4674
if(_tmpBE7 != 0  || _tmpBE8 != 0){
{void*_tmpC06=Cyc_Tcutil_compress(t);void*_tmpC07=_tmpC06;struct Cyc_List_List**_tmpC09;void*_tmpC0A;struct Cyc_Absyn_Tqual _tmpC0B;void*_tmpC0C;struct Cyc_List_List*_tmpC0D;int _tmpC0E;struct Cyc_Absyn_VarargInfo*_tmpC0F;struct Cyc_List_List*_tmpC10;struct Cyc_List_List*_tmpC11;_LL6DF: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC08=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC07;if(_tmpC08->tag != 9)goto _LL6E1;else{_tmpC09=(struct Cyc_List_List**)&(_tmpC08->f1).tvars;_tmpC0A=(_tmpC08->f1).effect;_tmpC0B=(_tmpC08->f1).ret_tqual;_tmpC0C=(_tmpC08->f1).ret_typ;_tmpC0D=(_tmpC08->f1).args;_tmpC0E=(_tmpC08->f1).c_varargs;_tmpC0F=(_tmpC08->f1).cyc_varargs;_tmpC10=(_tmpC08->f1).rgn_po;_tmpC11=(_tmpC08->f1).attributes;}}_LL6E0:
# 4677
 if(*_tmpC09 == 0){
# 4679
*_tmpC09=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmpBE7);
_tmpBE7=0;}
# 4682
goto _LL6DE;_LL6E1:;_LL6E2:
 goto _LL6DE;_LL6DE:;}
# 4685
if(_tmpBE7 != 0){
const char*_tmp1344;void*_tmp1343[1];struct Cyc_String_pa_PrintArg_struct _tmp1342;(_tmp1342.tag=0,((_tmp1342.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmpBE7->hd)->name),((_tmp1343[0]=& _tmp1342,Cyc_Tcutil_terr(loc,((_tmp1344="unbound type variable %s",_tag_dyneither(_tmp1344,sizeof(char),25))),_tag_dyneither(_tmp1343,sizeof(void*),1)))))));}
Cyc_Tcutil_check_free_evars(_tmpBE8,t,loc);}}
# 4629
;_pop_region(temp);}
# 4696
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4699
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmpC16=Cyc_Tcutil_compress(t);void*_tmpC17=_tmpC16;struct Cyc_List_List*_tmpC19;void*_tmpC1A;struct Cyc_Absyn_Tqual _tmpC1B;void*_tmpC1C;struct Cyc_List_List*_tmpC1D;_LL6E4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpC18=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC17;if(_tmpC18->tag != 9)goto _LL6E6;else{_tmpC19=(_tmpC18->f1).tvars;_tmpC1A=(_tmpC18->f1).effect;_tmpC1B=(_tmpC18->f1).ret_tqual;_tmpC1C=(_tmpC18->f1).ret_typ;_tmpC1D=(_tmpC18->f1).args;}}_LL6E5:
# 4702
 fd->tvs=_tmpC19;
fd->effect=_tmpC1A;
{struct Cyc_List_List*_tmpC1E=fd->args;for(0;_tmpC1E != 0;(_tmpC1E=_tmpC1E->tl,_tmpC1D=_tmpC1D->tl)){
# 4706
(*((struct _tuple10*)_tmpC1E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmpC1D))->hd)).f2;
(*((struct _tuple10*)_tmpC1E->hd)).f3=(*((struct _tuple10*)_tmpC1D->hd)).f3;}}
# 4709
fd->ret_tqual=_tmpC1B;
fd->ret_type=_tmpC1C;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmpC1C);
goto _LL6E3;_LL6E6:;_LL6E7: {
const char*_tmp1347;void*_tmp1346;(_tmp1346=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1347="check_fndecl_valid_type: not a FnType",_tag_dyneither(_tmp1347,sizeof(char),38))),_tag_dyneither(_tmp1346,sizeof(void*),0)));}_LL6E3:;}{
# 4716
struct _RegionHandle _tmpC21=_new_region("r");struct _RegionHandle*r=& _tmpC21;_push_region(r);{
const char*_tmp1348;Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,(
(_tmp1348="function declaration has repeated parameter",_tag_dyneither(_tmp1348,sizeof(char),44))));}
# 4721
fd->cached_typ=t;
# 4716
;_pop_region(r);};}
# 4726
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,void*t){
# 4729
struct _RegionHandle _tmpC23=_new_region("r");struct _RegionHandle*r=& _tmpC23;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp1349;struct Cyc_Tcutil_CVTEnv _tmpC24=Cyc_Tcutil_check_valid_type(loc,te,(
(_tmp1349.r=r,((_tmp1349.kind_env=bound_tvars,((_tmp1349.free_vars=0,((_tmp1349.free_evars=0,((_tmp1349.generalize_evars=0,((_tmp1349.fn_result=0,_tmp1349)))))))))))),expected_kind,t);
# 4733
struct Cyc_List_List*_tmpC25=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmpC24.free_vars),bound_tvars);
# 4735
struct Cyc_List_List*_tmpC26=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmpC24.free_evars);
{struct Cyc_List_List*fs=_tmpC25;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmpC27=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
const char*_tmp134E;void*_tmp134D[2];struct Cyc_String_pa_PrintArg_struct _tmp134C;struct Cyc_String_pa_PrintArg_struct _tmp134B;(_tmp134B.tag=0,((_tmp134B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp134C.tag=0,((_tmp134C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC27),((_tmp134D[0]=& _tmp134C,((_tmp134D[1]=& _tmp134B,Cyc_Tcutil_terr(loc,((_tmp134E="unbound type variable %s in type %s",_tag_dyneither(_tmp134E,sizeof(char),36))),_tag_dyneither(_tmp134D,sizeof(void*),2)))))))))))));}}
# 4740
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmpC26,t,loc);}
# 4730
;_pop_region(r);}
# 4745
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4750
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4756
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4759
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0){
const char*_tmp1353;void*_tmp1352[2];struct Cyc_String_pa_PrintArg_struct _tmp1351;struct Cyc_String_pa_PrintArg_struct _tmp1350;(_tmp1350.tag=0,((_tmp1350.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd)),((_tmp1351.tag=0,((_tmp1351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg),((_tmp1352[0]=& _tmp1351,((_tmp1352[1]=& _tmp1350,Cyc_Tcutil_terr(loc,((_tmp1353="%s: %s",_tag_dyneither(_tmp1353,sizeof(char),7))),_tag_dyneither(_tmp1352,sizeof(void*),2)))))))))))));}}}}
# 4765
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4769
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4773
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
const char*_tmp1354;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,((_tmp1354="duplicate type variable",_tag_dyneither(_tmp1354,sizeof(char),24))));}struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple32{struct Cyc_List_List*f1;void*f2;};struct _tuple33{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4787 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4792
struct _RegionHandle _tmpC32=_new_region("temp");struct _RegionHandle*temp=& _tmpC32;_push_region(temp);
# 4796
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
const char*_tmp1355;if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,((_tmp1355="",_tag_dyneither(_tmp1355,sizeof(char),1))))!= 0){
struct _tuple31*_tmp1358;struct Cyc_List_List*_tmp1357;fields=((_tmp1357=_region_malloc(temp,sizeof(*_tmp1357)),((_tmp1357->hd=((_tmp1358=_region_malloc(temp,sizeof(*_tmp1358)),((_tmp1358->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,((_tmp1358->f2=0,_tmp1358)))))),((_tmp1357->tl=fields,_tmp1357))))));}}}
# 4801
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4803
const char*_tmp135A;const char*_tmp1359;struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?(_tmp135A="struct",_tag_dyneither(_tmp135A,sizeof(char),7)):((_tmp1359="union",_tag_dyneither(_tmp1359,sizeof(char),6)));
# 4806
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple32*_tmpC36=(struct _tuple32*)des->hd;struct Cyc_List_List*_tmpC38;void*_tmpC39;struct _tuple32*_tmpC37=_tmpC36;_tmpC38=_tmpC37->f1;_tmpC39=_tmpC37->f2;
if(_tmpC38 == 0){
# 4811
struct Cyc_List_List*_tmpC3A=fields;
for(0;_tmpC3A != 0;_tmpC3A=_tmpC3A->tl){
if(!(*((struct _tuple31*)_tmpC3A->hd)).f2){
(*((struct _tuple31*)_tmpC3A->hd)).f2=1;
{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp1360;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp135F;struct Cyc_List_List*_tmp135E;(*((struct _tuple32*)des->hd)).f1=((_tmp135E=_cycalloc(sizeof(*_tmp135E)),((_tmp135E->hd=(void*)((_tmp1360=_cycalloc(sizeof(*_tmp1360)),((_tmp1360[0]=((_tmp135F.tag=1,((_tmp135F.f1=((*((struct _tuple31*)_tmpC3A->hd)).f1)->name,_tmp135F)))),_tmp1360)))),((_tmp135E->tl=0,_tmp135E))))));}
{struct _tuple33*_tmp1363;struct Cyc_List_List*_tmp1362;ans=((_tmp1362=_region_malloc(rgn,sizeof(*_tmp1362)),((_tmp1362->hd=((_tmp1363=_region_malloc(rgn,sizeof(*_tmp1363)),((_tmp1363->f1=(*((struct _tuple31*)_tmpC3A->hd)).f1,((_tmp1363->f2=_tmpC39,_tmp1363)))))),((_tmp1362->tl=ans,_tmp1362))))));}
break;}}
# 4819
if(_tmpC3A == 0){
const char*_tmp1367;void*_tmp1366[1];struct Cyc_String_pa_PrintArg_struct _tmp1365;(_tmp1365.tag=0,((_tmp1365.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp1366[0]=& _tmp1365,Cyc_Tcutil_terr(loc,((_tmp1367="too many arguments to %s",_tag_dyneither(_tmp1367,sizeof(char),25))),_tag_dyneither(_tmp1366,sizeof(void*),1)))))));}}else{
if(_tmpC38->tl != 0){
# 4823
const char*_tmp136A;void*_tmp1369;(_tmp1369=0,Cyc_Tcutil_terr(loc,((_tmp136A="multiple designators are not yet supported",_tag_dyneither(_tmp136A,sizeof(char),43))),_tag_dyneither(_tmp1369,sizeof(void*),0)));}else{
# 4826
void*_tmpC45=(void*)_tmpC38->hd;void*_tmpC46=_tmpC45;struct _dyneither_ptr*_tmpC49;_LL6E9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpC47=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpC46;if(_tmpC47->tag != 0)goto _LL6EB;}_LL6EA:
# 4828
{const char*_tmp136E;void*_tmp136D[1];struct Cyc_String_pa_PrintArg_struct _tmp136C;(_tmp136C.tag=0,((_tmp136C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str),((_tmp136D[0]=& _tmp136C,Cyc_Tcutil_terr(loc,((_tmp136E="array designator used in argument to %s",_tag_dyneither(_tmp136E,sizeof(char),40))),_tag_dyneither(_tmp136D,sizeof(void*),1)))))));}
goto _LL6E8;_LL6EB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpC48=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpC46;if(_tmpC48->tag != 1)goto _LL6E8;else{_tmpC49=_tmpC48->f1;}}_LL6EC: {
# 4831
struct Cyc_List_List*_tmpC4D=fields;
for(0;_tmpC4D != 0;_tmpC4D=_tmpC4D->tl){
if(Cyc_strptrcmp(_tmpC49,((*((struct _tuple31*)_tmpC4D->hd)).f1)->name)== 0){
if((*((struct _tuple31*)_tmpC4D->hd)).f2){
const char*_tmp1372;void*_tmp1371[1];struct Cyc_String_pa_PrintArg_struct _tmp1370;(_tmp1370.tag=0,((_tmp1370.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC49),((_tmp1371[0]=& _tmp1370,Cyc_Tcutil_terr(loc,((_tmp1372="member %s has already been used as an argument",_tag_dyneither(_tmp1372,sizeof(char),47))),_tag_dyneither(_tmp1371,sizeof(void*),1)))))));}
(*((struct _tuple31*)_tmpC4D->hd)).f2=1;
{struct _tuple33*_tmp1375;struct Cyc_List_List*_tmp1374;ans=((_tmp1374=_region_malloc(rgn,sizeof(*_tmp1374)),((_tmp1374->hd=((_tmp1375=_region_malloc(rgn,sizeof(*_tmp1375)),((_tmp1375->f1=(*((struct _tuple31*)_tmpC4D->hd)).f1,((_tmp1375->f2=_tmpC39,_tmp1375)))))),((_tmp1374->tl=ans,_tmp1374))))));}
break;}}
# 4840
if(_tmpC4D == 0){
const char*_tmp1379;void*_tmp1378[1];struct Cyc_String_pa_PrintArg_struct _tmp1377;(_tmp1377.tag=0,((_tmp1377.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpC49),((_tmp1378[0]=& _tmp1377,Cyc_Tcutil_terr(loc,((_tmp1379="bad field designator %s",_tag_dyneither(_tmp1379,sizeof(char),24))),_tag_dyneither(_tmp1378,sizeof(void*),1)))))));}
goto _LL6E8;}_LL6E8:;}}}
# 4845
if(aggr_kind == Cyc_Absyn_StructA)
# 4847
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple31*)fields->hd)).f2){
{const char*_tmp137C;void*_tmp137B;(_tmp137B=0,Cyc_Tcutil_terr(loc,((_tmp137C="too few arguments to struct",_tag_dyneither(_tmp137C,sizeof(char),28))),_tag_dyneither(_tmp137B,sizeof(void*),0)));}
break;}}else{
# 4854
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple31*)fields->hd)).f2){
if(found){const char*_tmp137F;void*_tmp137E;(_tmp137E=0,Cyc_Tcutil_terr(loc,((_tmp137F="only one member of a union is allowed",_tag_dyneither(_tmp137F,sizeof(char),38))),_tag_dyneither(_tmp137E,sizeof(void*),0)));}
found=1;}}
# 4861
if(!found){const char*_tmp1382;void*_tmp1381;(_tmp1381=0,Cyc_Tcutil_terr(loc,((_tmp1382="missing member for union",_tag_dyneither(_tmp1382,sizeof(char),25))),_tag_dyneither(_tmp1381,sizeof(void*),0)));}}{
# 4864
struct Cyc_List_List*_tmpC5C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmpC5C;};};}
# 4796
;_pop_region(temp);}
# 4867
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpC5F=Cyc_Tcutil_compress(t);void*_tmpC60=_tmpC5F;void*_tmpC62;union Cyc_Absyn_Constraint*_tmpC63;_LL6EE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC61=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC60;if(_tmpC61->tag != 5)goto _LL6F0;else{_tmpC62=(_tmpC61->f1).elt_typ;_tmpC63=((_tmpC61->f1).ptr_atts).bounds;}}_LL6EF: {
# 4870
void*_tmpC64=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpC63);void*_tmpC65=_tmpC64;_LL6F3: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC66=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC65;if(_tmpC66->tag != 0)goto _LL6F5;}_LL6F4:
# 4872
*elt_typ_dest=_tmpC62;
return 1;_LL6F5:;_LL6F6:
 return 0;_LL6F2:;}_LL6F0:;_LL6F1:
# 4876
 return 0;_LL6ED:;}
# 4880
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmpC67=Cyc_Tcutil_compress(t);void*_tmpC68=_tmpC67;void*_tmpC6A;union Cyc_Absyn_Constraint*_tmpC6B;_LL6F8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC69=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC68;if(_tmpC69->tag != 5)goto _LL6FA;else{_tmpC6A=(_tmpC69->f1).elt_typ;_tmpC6B=((_tmpC69->f1).ptr_atts).zero_term;}}_LL6F9:
# 4883
*elt_typ_dest=_tmpC6A;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC6B);_LL6FA:;_LL6FB:
 return 0;_LL6F7:;}
# 4891
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 4893
void*_tmpC6C=Cyc_Tcutil_compress(t);void*_tmpC6D=_tmpC6C;void*_tmpC6F;union Cyc_Absyn_Constraint*_tmpC70;union Cyc_Absyn_Constraint*_tmpC71;void*_tmpC73;struct Cyc_Absyn_Tqual _tmpC74;struct Cyc_Absyn_Exp*_tmpC75;union Cyc_Absyn_Constraint*_tmpC76;_LL6FD: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpC6E=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC6D;if(_tmpC6E->tag != 5)goto _LL6FF;else{_tmpC6F=(_tmpC6E->f1).elt_typ;_tmpC70=((_tmpC6E->f1).ptr_atts).bounds;_tmpC71=((_tmpC6E->f1).ptr_atts).zero_term;}}_LL6FE:
# 4895
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC71)){
*ptr_type=t;
*elt_type=_tmpC6F;
{void*_tmpC77=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmpC70);void*_tmpC78=_tmpC77;_LL704: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpC79=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpC78;if(_tmpC79->tag != 0)goto _LL706;}_LL705:
*is_dyneither=1;goto _LL703;_LL706:;_LL707:
*is_dyneither=0;goto _LL703;_LL703:;}
# 4902
return 1;}else{
return 0;}_LL6FF: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpC72=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpC6D;if(_tmpC72->tag != 8)goto _LL701;else{_tmpC73=(_tmpC72->f1).elt_type;_tmpC74=(_tmpC72->f1).tq;_tmpC75=(_tmpC72->f1).num_elts;_tmpC76=(_tmpC72->f1).zero_term;}}_LL700:
# 4905
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpC76)){
*elt_type=_tmpC73;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}_LL701:;_LL702:
 return 0;_LL6FC:;}
# 4918
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 4920
void*_tmpC7A=e1->r;void*_tmpC7B=_tmpC7A;struct Cyc_Absyn_Exp*_tmpC7E;struct Cyc_Absyn_Exp*_tmpC80;struct Cyc_Absyn_Exp*_tmpC82;struct Cyc_Absyn_Exp*_tmpC84;struct Cyc_Absyn_Exp*_tmpC86;struct Cyc_Absyn_Exp*_tmpC88;_LL709: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpC7C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC7C->tag != 13)goto _LL70B;}_LL70A: {
# 4922
const char*_tmp1386;void*_tmp1385[1];struct Cyc_String_pa_PrintArg_struct _tmp1384;(_tmp1384.tag=0,((_tmp1384.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1385[0]=& _tmp1384,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1386="we have a cast in a lhs:  %s",_tag_dyneither(_tmp1386,sizeof(char),29))),_tag_dyneither(_tmp1385,sizeof(void*),1)))))));}_LL70B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpC7D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC7D->tag != 19)goto _LL70D;else{_tmpC7E=_tmpC7D->f1;}}_LL70C:
 _tmpC80=_tmpC7E;goto _LL70E;_LL70D: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpC7F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC7F->tag != 22)goto _LL70F;else{_tmpC80=_tmpC7F->f1;}}_LL70E:
# 4925
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC80->topt),ptr_type,is_dyneither,elt_type);_LL70F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC81=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC81->tag != 21)goto _LL711;else{_tmpC82=_tmpC81->f1;}}_LL710:
 _tmpC84=_tmpC82;goto _LL712;_LL711: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpC83=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC83->tag != 20)goto _LL713;else{_tmpC84=_tmpC83->f1;}}_LL712:
# 4929
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC84->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp138A;void*_tmp1389[1];struct Cyc_String_pa_PrintArg_struct _tmp1388;(_tmp1388.tag=0,((_tmp1388.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp1389[0]=& _tmp1388,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp138A="found zero pointer aggregate member assignment: %s",_tag_dyneither(_tmp138A,sizeof(char),51))),_tag_dyneither(_tmp1389,sizeof(void*),1)))))));}
return 0;_LL713: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpC85=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC85->tag != 12)goto _LL715;else{_tmpC86=_tmpC85->f1;}}_LL714:
 _tmpC88=_tmpC86;goto _LL716;_LL715: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpC87=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC87->tag != 11)goto _LL717;else{_tmpC88=_tmpC87->f1;}}_LL716:
# 4935
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpC88->topt),ptr_type,is_dyneither,elt_type)){
const char*_tmp138E;void*_tmp138D[1];struct Cyc_String_pa_PrintArg_struct _tmp138C;(_tmp138C.tag=0,((_tmp138C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1)),((_tmp138D[0]=& _tmp138C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp138E="found zero pointer instantiate/noinstantiate: %s",_tag_dyneither(_tmp138E,sizeof(char),49))),_tag_dyneither(_tmp138D,sizeof(void*),1)))))));}
return 0;_LL717: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpC89=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpC7B;if(_tmpC89->tag != 1)goto _LL719;}_LL718:
 return 0;_LL719:;_LL71A: {
# 4941
const char*_tmp1392;void*_tmp1391[1];struct Cyc_String_pa_PrintArg_struct _tmp1390;(_tmp1390.tag=0,((_tmp1390.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1)),((_tmp1391[0]=& _tmp1390,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp1392="found bad lhs in is_zero_ptr_deref: %s",_tag_dyneither(_tmp1392,sizeof(char),39))),_tag_dyneither(_tmp1391,sizeof(void*),1)))))));}_LL708:;}
# 4945
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 4956
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmpC96=Cyc_Tcutil_compress(r);void*_tmpC97=_tmpC96;struct Cyc_Absyn_Tvar*_tmpC9B;_LL71C: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpC98=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmpC97;if(_tmpC98->tag != 22)goto _LL71E;}_LL71D:
 return !must_be_unique;_LL71E: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpC99=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmpC97;if(_tmpC99->tag != 21)goto _LL720;}_LL71F:
 return 1;_LL720: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC9A=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpC97;if(_tmpC9A->tag != 2)goto _LL722;else{_tmpC9B=_tmpC9A->f1;}}_LL721: {
# 4961
struct Cyc_Absyn_Kind*_tmpC9C=Cyc_Tcutil_tvar_kind(_tmpC9B,& Cyc_Tcutil_rk);enum Cyc_Absyn_KindQual _tmpC9E;enum Cyc_Absyn_AliasQual _tmpC9F;struct Cyc_Absyn_Kind*_tmpC9D=_tmpC9C;_tmpC9E=_tmpC9D->kind;_tmpC9F=_tmpC9D->aliasqual;
if(_tmpC9E == Cyc_Absyn_RgnKind  && (_tmpC9F == Cyc_Absyn_Unique  || _tmpC9F == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpCA0=Cyc_Absyn_compress_kb(_tmpC9B->kind);void*_tmpCA1=_tmpCA0;struct Cyc_Core_Opt**_tmpCA3;_LL725: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCA2=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCA1;if(_tmpCA2->tag != 2)goto _LL727;else{_tmpCA3=(struct Cyc_Core_Opt**)& _tmpCA2->f1;if((_tmpCA2->f2)->kind != Cyc_Absyn_RgnKind)goto _LL727;if((_tmpCA2->f2)->aliasqual != Cyc_Absyn_Top)goto _LL727;}}_LL726:
# 4965
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp1398;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp1397;struct Cyc_Core_Opt*_tmp1396;*_tmpCA3=((_tmp1396=_cycalloc(sizeof(*_tmp1396)),((_tmp1396->v=(void*)((_tmp1398=_cycalloc(sizeof(*_tmp1398)),((_tmp1398[0]=((_tmp1397.tag=2,((_tmp1397.f1=0,((_tmp1397.f2=& Cyc_Tcutil_rk,_tmp1397)))))),_tmp1398)))),_tmp1396))));}
return 0;_LL727:;_LL728:
 return 1;_LL724:;}
# 4970
return 0;}_LL722:;_LL723:
 return 0;_LL71B:;}
# 4977
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpCA7=Cyc_Tcutil_compress(t);void*_tmpCA8=_tmpCA7;void*_tmpCAA;struct Cyc_Absyn_Tvar*_tmpCAC;_LL72A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpCA9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpCA8;if(_tmpCA9->tag != 5)goto _LL72C;else{_tmpCAA=((_tmpCA9->f1).ptr_atts).rgn;}}_LL72B:
# 4980
 return Cyc_Tcutil_is_noalias_region(_tmpCAA,must_be_unique);_LL72C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpCAB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpCA8;if(_tmpCAB->tag != 2)goto _LL72E;else{_tmpCAC=_tmpCAB->f1;}}_LL72D: {
# 4982
struct Cyc_Absyn_Kind*_tmpCAD=Cyc_Tcutil_tvar_kind(_tmpCAC,& Cyc_Tcutil_bk);enum Cyc_Absyn_KindQual _tmpCAF;enum Cyc_Absyn_AliasQual _tmpCB0;struct Cyc_Absyn_Kind*_tmpCAE=_tmpCAD;_tmpCAF=_tmpCAE->kind;_tmpCB0=_tmpCAE->aliasqual;
switch(_tmpCAF){case Cyc_Absyn_BoxKind: _LL730:
 goto _LL731;case Cyc_Absyn_AnyKind: _LL731: goto _LL732;case Cyc_Absyn_MemKind: _LL732:
 if(_tmpCB0 == Cyc_Absyn_Unique  || _tmpCB0 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpCB1=Cyc_Absyn_compress_kb(_tmpCAC->kind);void*_tmpCB2=_tmpCB1;struct Cyc_Core_Opt**_tmpCB4;enum Cyc_Absyn_KindQual _tmpCB5;_LL735: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpCB3=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpCB2;if(_tmpCB3->tag != 2)goto _LL737;else{_tmpCB4=(struct Cyc_Core_Opt**)& _tmpCB3->f1;_tmpCB5=(_tmpCB3->f2)->kind;if((_tmpCB3->f2)->aliasqual != Cyc_Absyn_Top)goto _LL737;}}_LL736:
# 4988
{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13A2;struct Cyc_Absyn_Kind*_tmp13A1;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp13A0;struct Cyc_Core_Opt*_tmp139F;*_tmpCB4=((_tmp139F=_cycalloc(sizeof(*_tmp139F)),((_tmp139F->v=(void*)((_tmp13A2=_cycalloc(sizeof(*_tmp13A2)),((_tmp13A2[0]=((_tmp13A0.tag=2,((_tmp13A0.f1=0,((_tmp13A0.f2=((_tmp13A1=_cycalloc_atomic(sizeof(*_tmp13A1)),((_tmp13A1->kind=_tmpCB5,((_tmp13A1->aliasqual=Cyc_Absyn_Aliasable,_tmp13A1)))))),_tmp13A0)))))),_tmp13A2)))),_tmp139F))));}
return 0;_LL737:;_LL738:
# 4992
 return 1;_LL734:;}
# 4995
return 0;default: _LL733:
 return 0;}}_LL72E:;_LL72F:
# 4998
 return 0;_LL729:;}
# 5001
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpCBA=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpCBA,0))return 1;{
void*_tmpCBB=_tmpCBA;struct Cyc_List_List*_tmpCBD;struct Cyc_Absyn_Aggrdecl**_tmpCBF;struct Cyc_List_List*_tmpCC0;struct Cyc_List_List*_tmpCC2;union Cyc_Absyn_DatatypeInfoU _tmpCC5;struct Cyc_List_List*_tmpCC6;union Cyc_Absyn_DatatypeFieldInfoU _tmpCC8;struct Cyc_List_List*_tmpCC9;_LL73B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCBC=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCBB;if(_tmpCBC->tag != 10)goto _LL73D;else{_tmpCBD=_tmpCBC->f1;}}_LL73C:
# 5006
 while(_tmpCBD != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpCBD->hd)).f2))return 1;
_tmpCBD=_tmpCBD->tl;}
# 5010
return 0;_LL73D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCBE=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCBB;if(_tmpCBE->tag != 11)goto _LL73F;else{if((((_tmpCBE->f1).aggr_info).KnownAggr).tag != 2)goto _LL73F;_tmpCBF=(struct Cyc_Absyn_Aggrdecl**)(((_tmpCBE->f1).aggr_info).KnownAggr).val;_tmpCC0=(_tmpCBE->f1).targs;}}_LL73E:
# 5012
 if((*_tmpCBF)->impl == 0)return 0;else{
# 5014
struct Cyc_List_List*_tmpCCA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpCBF)->tvs,_tmpCC0);
struct Cyc_List_List*_tmpCCB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpCBF)->impl))->fields;
void*t;
while(_tmpCCB != 0){
t=_tmpCCA == 0?((struct Cyc_Absyn_Aggrfield*)_tmpCCB->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpCCA,((struct Cyc_Absyn_Aggrfield*)_tmpCCB->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpCCB=_tmpCCB->tl;}
# 5022
return 0;}_LL73F: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCC1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCBB;if(_tmpCC1->tag != 12)goto _LL741;else{_tmpCC2=_tmpCC1->f2;}}_LL740:
# 5025
 while(_tmpCC2 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpCC2->hd)->type))return 1;
_tmpCC2=_tmpCC2->tl;}
# 5029
return 0;_LL741: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCC3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCBB;if(_tmpCC3->tag != 11)goto _LL743;else{if((((_tmpCC3->f1).aggr_info).UnknownAggr).tag != 1)goto _LL743;}}_LL742:
# 5032
 return 0;_LL743: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmpCC4=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpCBB;if(_tmpCC4->tag != 3)goto _LL745;else{_tmpCC5=(_tmpCC4->f1).datatype_info;_tmpCC6=(_tmpCC4->f1).targs;}}_LL744: {
# 5034
union Cyc_Absyn_DatatypeInfoU _tmpCCC=_tmpCC5;struct _tuple2*_tmpCCD;int _tmpCCE;struct Cyc_List_List*_tmpCCF;struct Cyc_Core_Opt*_tmpCD0;_LL74A: if((_tmpCCC.UnknownDatatype).tag != 1)goto _LL74C;_tmpCCD=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpCCC.UnknownDatatype).val).name;_tmpCCE=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpCCC.UnknownDatatype).val).is_extensible;_LL74B:
# 5037
 return 0;_LL74C: if((_tmpCCC.KnownDatatype).tag != 2)goto _LL749;_tmpCCF=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpCCC.KnownDatatype).val))->tvs;_tmpCD0=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpCCC.KnownDatatype).val))->fields;_LL74D:
# 5040
 return 0;_LL749:;}_LL745: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpCC7=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpCBB;if(_tmpCC7->tag != 4)goto _LL747;else{_tmpCC8=(_tmpCC7->f1).field_info;_tmpCC9=(_tmpCC7->f1).targs;}}_LL746: {
# 5043
union Cyc_Absyn_DatatypeFieldInfoU _tmpCD1=_tmpCC8;struct Cyc_Absyn_Datatypedecl*_tmpCD2;struct Cyc_Absyn_Datatypefield*_tmpCD3;_LL74F: if((_tmpCD1.UnknownDatatypefield).tag != 1)goto _LL751;_LL750:
# 5046
 return 0;_LL751: if((_tmpCD1.KnownDatatypefield).tag != 2)goto _LL74E;_tmpCD2=((struct _tuple3)(_tmpCD1.KnownDatatypefield).val).f1;_tmpCD3=((struct _tuple3)(_tmpCD1.KnownDatatypefield).val).f2;_LL752: {
# 5048
struct Cyc_List_List*_tmpCD4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpCD2->tvs,_tmpCC9);
struct Cyc_List_List*_tmpCD5=_tmpCD3->typs;
while(_tmpCD5 != 0){
_tmpCBA=_tmpCD4 == 0?(*((struct _tuple12*)_tmpCD5->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpCD4,(*((struct _tuple12*)_tmpCD5->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpCBA))return 1;
_tmpCD5=_tmpCD5->tl;}
# 5055
return 0;}_LL74E:;}_LL747:;_LL748:
# 5057
 return 0;_LL73A:;};}
# 5061
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpCD6=Cyc_Tcutil_compress(t);void*_tmpCD7=_tmpCD6;struct Cyc_Absyn_Aggrdecl*_tmpCD9;struct Cyc_List_List*_tmpCDA;struct Cyc_List_List*_tmpCDC;_LL754: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCD8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpCD7;if(_tmpCD8->tag != 11)goto _LL756;else{if((((_tmpCD8->f1).aggr_info).KnownAggr).tag != 2)goto _LL756;_tmpCD9=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpCD8->f1).aggr_info).KnownAggr).val);_tmpCDA=(_tmpCD8->f1).targs;}}_LL755:
# 5064
 _tmpCDC=_tmpCD9->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpCD9->impl))->fields;goto _LL757;_LL756: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpCDB=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpCD7;if(_tmpCDB->tag != 12)goto _LL758;else{_tmpCDC=_tmpCDB->f2;}}_LL757: {
# 5066
struct Cyc_Absyn_Aggrfield*_tmpCDD=Cyc_Absyn_lookup_field(_tmpCDC,f);
{struct Cyc_Absyn_Aggrfield*_tmpCDE=_tmpCDD;void*_tmpCDF;_LL75B: if(_tmpCDE != 0)goto _LL75D;_LL75C: {
const char*_tmp13A5;void*_tmp13A4;(_tmp13A4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A5="is_noalias_field: missing field",_tag_dyneither(_tmp13A5,sizeof(char),32))),_tag_dyneither(_tmp13A4,sizeof(void*),0)));}_LL75D: if(_tmpCDE == 0)goto _LL75A;_tmpCDF=_tmpCDE->type;_LL75E:
# 5070
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpCDF);_LL75A:;}
# 5072
return 0;}_LL758:;_LL759: {
# 5074
const char*_tmp13A9;void*_tmp13A8[1];struct Cyc_String_pa_PrintArg_struct _tmp13A7;(_tmp13A7.tag=0,((_tmp13A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t)),((_tmp13A8[0]=& _tmp13A7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13A9="is_noalias_field: invalid type |%s|",_tag_dyneither(_tmp13A9,sizeof(char),36))),_tag_dyneither(_tmp13A8,sizeof(void*),1)))))));}_LL753:;}
# 5082
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpCE5=e->r;void*_tmpCE6=_tmpCE5;struct Cyc_Absyn_Exp*_tmpCEA;struct Cyc_Absyn_Exp*_tmpCEC;struct Cyc_Absyn_Exp*_tmpCEE;struct _dyneither_ptr*_tmpCEF;struct Cyc_Absyn_Exp*_tmpCF1;struct Cyc_Absyn_Exp*_tmpCF2;struct Cyc_Absyn_Exp*_tmpCF4;struct Cyc_Absyn_Exp*_tmpCF5;struct Cyc_Absyn_Exp*_tmpCF7;struct Cyc_Absyn_Exp*_tmpCF9;struct Cyc_Absyn_Stmt*_tmpCFB;_LL760: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpCE7=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCE7->tag != 1)goto _LL762;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpCE8=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmpCE7->f2);if(_tmpCE8->tag != 1)goto _LL762;}}_LL761:
 return 0;_LL762: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpCE9=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCE9->tag != 21)goto _LL764;else{_tmpCEA=_tmpCE9->f1;}}_LL763:
 _tmpCEC=_tmpCEA;goto _LL765;_LL764: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpCEB=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCEB->tag != 19)goto _LL766;else{_tmpCEC=_tmpCEB->f1;}}_LL765:
# 5087
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpCEC->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpCEC);_LL766: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpCED=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCED->tag != 20)goto _LL768;else{_tmpCEE=_tmpCED->f1;_tmpCEF=_tmpCED->f2;}}_LL767:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCEE);_LL768: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpCF0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCF0->tag != 22)goto _LL76A;else{_tmpCF1=_tmpCF0->f1;_tmpCF2=_tmpCF0->f2;}}_LL769: {
# 5091
void*_tmpCFC=Cyc_Tcutil_compress((void*)_check_null(_tmpCF1->topt));void*_tmpCFD=_tmpCFC;_LL775: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpCFE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpCFD;if(_tmpCFE->tag != 10)goto _LL777;}_LL776:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCF1);_LL777:;_LL778:
 return 0;_LL774:;}_LL76A: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpCF3=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCF3->tag != 5)goto _LL76C;else{_tmpCF4=_tmpCF3->f2;_tmpCF5=_tmpCF3->f3;}}_LL76B:
# 5096
 return Cyc_Tcutil_is_noalias_path(r,_tmpCF4) && Cyc_Tcutil_is_noalias_path(r,_tmpCF5);_LL76C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpCF6=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCF6->tag != 8)goto _LL76E;else{_tmpCF7=_tmpCF6->f2;}}_LL76D:
 _tmpCF9=_tmpCF7;goto _LL76F;_LL76E: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpCF8=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCF8->tag != 13)goto _LL770;else{_tmpCF9=_tmpCF8->f2;}}_LL76F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpCF9);_LL770: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpCFA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpCE6;if(_tmpCFA->tag != 36)goto _LL772;else{_tmpCFB=_tmpCFA->f1;}}_LL771:
# 5100
 while(1){
void*_tmpCFF=_tmpCFB->r;void*_tmpD00=_tmpCFF;struct Cyc_Absyn_Stmt*_tmpD02;struct Cyc_Absyn_Stmt*_tmpD03;struct Cyc_Absyn_Decl*_tmpD05;struct Cyc_Absyn_Stmt*_tmpD06;struct Cyc_Absyn_Exp*_tmpD08;_LL77A: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpD01=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpD00;if(_tmpD01->tag != 2)goto _LL77C;else{_tmpD02=_tmpD01->f1;_tmpD03=_tmpD01->f2;}}_LL77B:
 _tmpCFB=_tmpD03;goto _LL779;_LL77C: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpD04=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpD00;if(_tmpD04->tag != 12)goto _LL77E;else{_tmpD05=_tmpD04->f1;_tmpD06=_tmpD04->f2;}}_LL77D:
 _tmpCFB=_tmpD06;goto _LL779;_LL77E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpD07=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpD00;if(_tmpD07->tag != 1)goto _LL780;else{_tmpD08=_tmpD07->f1;}}_LL77F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpD08);_LL780:;_LL781: {
const char*_tmp13AC;void*_tmp13AB;(_tmp13AB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13AC="is_noalias_stmt_exp: ill-formed StmtExp",_tag_dyneither(_tmp13AC,sizeof(char),40))),_tag_dyneither(_tmp13AB,sizeof(void*),0)));}_LL779:;}_LL772:;_LL773:
# 5108
 return 1;_LL75F:;}
# 5125 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5127
struct _tuple18 _tmp13AD;struct _tuple18 bogus_ans=(_tmp13AD.f1=0,((_tmp13AD.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AD)));
void*_tmpD0B=e->r;void*_tmpD0C=_tmpD0B;struct _tuple2*_tmpD0E;void*_tmpD0F;struct Cyc_Absyn_Exp*_tmpD11;struct _dyneither_ptr*_tmpD12;int _tmpD13;struct Cyc_Absyn_Exp*_tmpD15;struct _dyneither_ptr*_tmpD16;int _tmpD17;struct Cyc_Absyn_Exp*_tmpD19;struct Cyc_Absyn_Exp*_tmpD1B;struct Cyc_Absyn_Exp*_tmpD1C;_LL783: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD0D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD0C;if(_tmpD0D->tag != 1)goto _LL785;else{_tmpD0E=_tmpD0D->f1;_tmpD0F=(void*)_tmpD0D->f2;}}_LL784: {
# 5131
void*_tmpD1D=_tmpD0F;struct Cyc_Absyn_Vardecl*_tmpD21;struct Cyc_Absyn_Vardecl*_tmpD23;struct Cyc_Absyn_Vardecl*_tmpD25;struct Cyc_Absyn_Vardecl*_tmpD27;_LL790: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpD1E=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD1E->tag != 0)goto _LL792;}_LL791:
 goto _LL793;_LL792: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpD1F=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD1F->tag != 2)goto _LL794;}_LL793:
 return bogus_ans;_LL794: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpD20=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD20->tag != 1)goto _LL796;else{_tmpD21=_tmpD20->f1;}}_LL795: {
# 5135
void*_tmpD28=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD29=_tmpD28;_LL79D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD29;if(_tmpD2A->tag != 8)goto _LL79F;}_LL79E: {
# 5137
struct _tuple18 _tmp13AE;return(_tmp13AE.f1=1,((_tmp13AE.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AE)));}_LL79F:;_LL7A0: {
struct _tuple18 _tmp13AF;return(_tmp13AF.f1=(_tmpD21->tq).real_const,((_tmp13AF.f2=(void*)& Cyc_Absyn_HeapRgn_val,_tmp13AF)));}_LL79C:;}_LL796: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD22=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD22->tag != 4)goto _LL798;else{_tmpD23=_tmpD22->f1;}}_LL797: {
# 5141
void*_tmpD2D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpD2E=_tmpD2D;_LL7A2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD2F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD2E;if(_tmpD2F->tag != 8)goto _LL7A4;}_LL7A3: {
struct _tuple18 _tmp13B0;return(_tmp13B0.f1=1,((_tmp13B0.f2=(void*)_check_null(_tmpD23->rgn),_tmp13B0)));}_LL7A4:;_LL7A5:
# 5144
 _tmpD23->escapes=1;{
struct _tuple18 _tmp13B1;return(_tmp13B1.f1=(_tmpD23->tq).real_const,((_tmp13B1.f2=(void*)_check_null(_tmpD23->rgn),_tmp13B1)));};_LL7A1:;}_LL798: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpD24=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD24->tag != 5)goto _LL79A;else{_tmpD25=_tmpD24->f1;}}_LL799:
# 5147
 _tmpD27=_tmpD25;goto _LL79B;_LL79A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpD26=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpD1D;if(_tmpD26->tag != 3)goto _LL78F;else{_tmpD27=_tmpD26->f1;}}_LL79B:
# 5149
 _tmpD27->escapes=1;{
struct _tuple18 _tmp13B2;return(_tmp13B2.f1=(_tmpD27->tq).real_const,((_tmp13B2.f2=(void*)_check_null(_tmpD27->rgn),_tmp13B2)));};_LL78F:;}_LL785: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpD10=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD0C;if(_tmpD10->tag != 20)goto _LL787;else{_tmpD11=_tmpD10->f1;_tmpD12=_tmpD10->f2;_tmpD13=_tmpD10->f3;}}_LL786:
# 5154
 if(_tmpD13)return bogus_ans;{
# 5157
void*_tmpD33=Cyc_Tcutil_compress((void*)_check_null(_tmpD11->topt));void*_tmpD34=_tmpD33;struct Cyc_List_List*_tmpD36;struct Cyc_Absyn_Aggrdecl*_tmpD38;_LL7A7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD35=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD34;if(_tmpD35->tag != 12)goto _LL7A9;else{_tmpD36=_tmpD35->f2;}}_LL7A8: {
# 5159
struct Cyc_Absyn_Aggrfield*_tmpD39=Cyc_Absyn_lookup_field(_tmpD36,_tmpD12);
if(_tmpD39 != 0  && _tmpD39->width == 0){
struct _tuple18 _tmpD3A=Cyc_Tcutil_addressof_props(te,_tmpD11);int _tmpD3C;void*_tmpD3D;struct _tuple18 _tmpD3B=_tmpD3A;_tmpD3C=_tmpD3B.f1;_tmpD3D=_tmpD3B.f2;{
struct _tuple18 _tmp13B3;return(_tmp13B3.f1=(_tmpD39->tq).real_const  || _tmpD3C,((_tmp13B3.f2=_tmpD3D,_tmp13B3)));};}
# 5164
return bogus_ans;}_LL7A9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD37=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD34;if(_tmpD37->tag != 11)goto _LL7AB;else{if((((_tmpD37->f1).aggr_info).KnownAggr).tag != 2)goto _LL7AB;_tmpD38=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD37->f1).aggr_info).KnownAggr).val);}}_LL7AA: {
# 5166
struct Cyc_Absyn_Aggrfield*_tmpD3F=Cyc_Absyn_lookup_decl_field(_tmpD38,_tmpD12);
if(_tmpD3F != 0  && _tmpD3F->width == 0){
struct _tuple18 _tmpD40=Cyc_Tcutil_addressof_props(te,_tmpD11);int _tmpD42;void*_tmpD43;struct _tuple18 _tmpD41=_tmpD40;_tmpD42=_tmpD41.f1;_tmpD43=_tmpD41.f2;{
struct _tuple18 _tmp13B4;return(_tmp13B4.f1=(_tmpD3F->tq).real_const  || _tmpD42,((_tmp13B4.f2=_tmpD43,_tmp13B4)));};}
# 5171
return bogus_ans;}_LL7AB:;_LL7AC:
 return bogus_ans;_LL7A6:;};_LL787: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpD14=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD0C;if(_tmpD14->tag != 21)goto _LL789;else{_tmpD15=_tmpD14->f1;_tmpD16=_tmpD14->f2;_tmpD17=_tmpD14->f3;}}_LL788:
# 5176
 if(_tmpD17)return bogus_ans;{
# 5180
void*_tmpD45=Cyc_Tcutil_compress((void*)_check_null(_tmpD15->topt));void*_tmpD46=_tmpD45;void*_tmpD48;void*_tmpD49;_LL7AE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD47=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD46;if(_tmpD47->tag != 5)goto _LL7B0;else{_tmpD48=(_tmpD47->f1).elt_typ;_tmpD49=((_tmpD47->f1).ptr_atts).rgn;}}_LL7AF: {
# 5182
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpD4A=Cyc_Tcutil_compress(_tmpD48);void*_tmpD4B=_tmpD4A;struct Cyc_List_List*_tmpD4D;struct Cyc_Absyn_Aggrdecl*_tmpD4F;_LL7B3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD4C=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD4B;if(_tmpD4C->tag != 12)goto _LL7B5;else{_tmpD4D=_tmpD4C->f2;}}_LL7B4:
# 5185
 finfo=Cyc_Absyn_lookup_field(_tmpD4D,_tmpD16);goto _LL7B2;_LL7B5: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD4E=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD4B;if(_tmpD4E->tag != 11)goto _LL7B7;else{if((((_tmpD4E->f1).aggr_info).KnownAggr).tag != 2)goto _LL7B7;_tmpD4F=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD4E->f1).aggr_info).KnownAggr).val);}}_LL7B6:
# 5187
 finfo=Cyc_Absyn_lookup_decl_field(_tmpD4F,_tmpD16);goto _LL7B2;_LL7B7:;_LL7B8:
 return bogus_ans;_LL7B2:;}
# 5190
if(finfo != 0  && finfo->width == 0){
struct _tuple18 _tmp13B5;return(_tmp13B5.f1=(finfo->tq).real_const,((_tmp13B5.f2=_tmpD49,_tmp13B5)));}
return bogus_ans;}_LL7B0:;_LL7B1:
 return bogus_ans;_LL7AD:;};_LL789: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpD18=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD0C;if(_tmpD18->tag != 19)goto _LL78B;else{_tmpD19=_tmpD18->f1;}}_LL78A: {
# 5197
void*_tmpD51=Cyc_Tcutil_compress((void*)_check_null(_tmpD19->topt));void*_tmpD52=_tmpD51;struct Cyc_Absyn_Tqual _tmpD54;void*_tmpD55;_LL7BA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD53=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD52;if(_tmpD53->tag != 5)goto _LL7BC;else{_tmpD54=(_tmpD53->f1).elt_tq;_tmpD55=((_tmpD53->f1).ptr_atts).rgn;}}_LL7BB: {
# 5199
struct _tuple18 _tmp13B6;return(_tmp13B6.f1=_tmpD54.real_const,((_tmp13B6.f2=_tmpD55,_tmp13B6)));}_LL7BC:;_LL7BD:
 return bogus_ans;_LL7B9:;}_LL78B: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpD1A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD0C;if(_tmpD1A->tag != 22)goto _LL78D;else{_tmpD1B=_tmpD1A->f1;_tmpD1C=_tmpD1A->f2;}}_LL78C: {
# 5205
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpD1B->topt));
void*_tmpD57=t;struct Cyc_List_List*_tmpD59;struct Cyc_Absyn_Tqual _tmpD5B;void*_tmpD5C;struct Cyc_Absyn_Tqual _tmpD5E;_LL7BF: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD58=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD57;if(_tmpD58->tag != 10)goto _LL7C1;else{_tmpD59=_tmpD58->f1;}}_LL7C0: {
# 5209
struct _tuple14 _tmpD5F=Cyc_Evexp_eval_const_uint_exp(_tmpD1C);unsigned int _tmpD61;int _tmpD62;struct _tuple14 _tmpD60=_tmpD5F;_tmpD61=_tmpD60.f1;_tmpD62=_tmpD60.f2;
if(!_tmpD62)
return bogus_ans;{
struct _tuple12*_tmpD63=Cyc_Absyn_lookup_tuple_field(_tmpD59,(int)_tmpD61);
if(_tmpD63 != 0){
struct _tuple18 _tmp13B7;return(_tmp13B7.f1=((*_tmpD63).f1).real_const,((_tmp13B7.f2=(Cyc_Tcutil_addressof_props(te,_tmpD1B)).f2,_tmp13B7)));}
return bogus_ans;};}_LL7C1: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpD5A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD57;if(_tmpD5A->tag != 5)goto _LL7C3;else{_tmpD5B=(_tmpD5A->f1).elt_tq;_tmpD5C=((_tmpD5A->f1).ptr_atts).rgn;}}_LL7C2: {
# 5217
struct _tuple18 _tmp13B8;return(_tmp13B8.f1=_tmpD5B.real_const,((_tmp13B8.f2=_tmpD5C,_tmp13B8)));}_LL7C3: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD5D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD57;if(_tmpD5D->tag != 8)goto _LL7C5;else{_tmpD5E=(_tmpD5D->f1).tq;}}_LL7C4: {
# 5223
struct _tuple18 _tmp13B9;return(_tmp13B9.f1=_tmpD5E.real_const,((_tmp13B9.f2=(Cyc_Tcutil_addressof_props(te,_tmpD1B)).f2,_tmp13B9)));}_LL7C5:;_LL7C6:
 return bogus_ans;_LL7BE:;}_LL78D:;_LL78E:
# 5227
{const char*_tmp13BC;void*_tmp13BB;(_tmp13BB=0,Cyc_Tcutil_terr(e->loc,((_tmp13BC="unary & applied to non-lvalue",_tag_dyneither(_tmp13BC,sizeof(char),30))),_tag_dyneither(_tmp13BB,sizeof(void*),0)));}
return bogus_ans;_LL782:;}
# 5234
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpD6A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpD6B=_tmpD6A;struct Cyc_Absyn_Exp*_tmpD6E;_LL7C8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpD6C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpD6B;if(_tmpD6C->tag != 0)goto _LL7CA;}_LL7C9:
 return;_LL7CA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD6D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD6B;if(_tmpD6D->tag != 1)goto _LL7C7;else{_tmpD6E=_tmpD6D->f1;}}_LL7CB: {
# 5239
struct _tuple14 _tmpD6F=Cyc_Evexp_eval_const_uint_exp(_tmpD6E);unsigned int _tmpD71;int _tmpD72;struct _tuple14 _tmpD70=_tmpD6F;_tmpD71=_tmpD70.f1;_tmpD72=_tmpD70.f2;
if(_tmpD72  && _tmpD71 <= i){
const char*_tmp13C1;void*_tmp13C0[2];struct Cyc_Int_pa_PrintArg_struct _tmp13BF;struct Cyc_Int_pa_PrintArg_struct _tmp13BE;(_tmp13BE.tag=1,((_tmp13BE.f1=(unsigned long)((int)i),((_tmp13BF.tag=1,((_tmp13BF.f1=(unsigned long)((int)_tmpD71),((_tmp13C0[0]=& _tmp13BF,((_tmp13C0[1]=& _tmp13BE,Cyc_Tcutil_terr(loc,((_tmp13C1="dereference is out of bounds: %d <= %d",_tag_dyneither(_tmp13C1,sizeof(char),39))),_tag_dyneither(_tmp13C0,sizeof(void*),2)))))))))))));}
return;}_LL7C7:;};}
# 5246
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5250
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpD77=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpD78=_tmpD77;struct Cyc_Absyn_Exp*_tmpD7A;_LL7CD: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpD79=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpD78;if(_tmpD79->tag != 1)goto _LL7CF;else{_tmpD7A=_tmpD79->f1;}}_LL7CE: {
# 5253
struct _tuple14 _tmpD7B=Cyc_Evexp_eval_const_uint_exp(_tmpD7A);unsigned int _tmpD7D;int _tmpD7E;struct _tuple14 _tmpD7C=_tmpD7B;_tmpD7D=_tmpD7C.f1;_tmpD7E=_tmpD7C.f2;
return _tmpD7E  && _tmpD7D == 1;}_LL7CF:;_LL7D0:
 return 0;_LL7CC:;}
# 5259
int Cyc_Tcutil_bits_only(void*t){
void*_tmpD7F=Cyc_Tcutil_compress(t);void*_tmpD80=_tmpD7F;void*_tmpD87;union Cyc_Absyn_Constraint*_tmpD88;struct Cyc_List_List*_tmpD8A;struct Cyc_Absyn_Aggrdecl*_tmpD8D;struct Cyc_List_List*_tmpD8E;struct Cyc_List_List*_tmpD90;_LL7D2: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpD81=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpD80;if(_tmpD81->tag != 0)goto _LL7D4;}_LL7D3:
 goto _LL7D5;_LL7D4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpD82=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD80;if(_tmpD82->tag != 6)goto _LL7D6;}_LL7D5:
 goto _LL7D7;_LL7D6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpD83=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpD80;if(_tmpD83->tag != 7)goto _LL7D8;}_LL7D7:
 return 1;_LL7D8: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpD84=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpD80;if(_tmpD84->tag != 13)goto _LL7DA;}_LL7D9:
 goto _LL7DB;_LL7DA: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpD85=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpD80;if(_tmpD85->tag != 14)goto _LL7DC;}_LL7DB:
 return 0;_LL7DC: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpD86=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpD80;if(_tmpD86->tag != 8)goto _LL7DE;else{_tmpD87=(_tmpD86->f1).elt_type;_tmpD88=(_tmpD86->f1).zero_term;}}_LL7DD:
# 5269
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpD88) && Cyc_Tcutil_bits_only(_tmpD87);_LL7DE: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpD89=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpD80;if(_tmpD89->tag != 10)goto _LL7E0;else{_tmpD8A=_tmpD89->f1;}}_LL7DF:
# 5271
 for(0;_tmpD8A != 0;_tmpD8A=_tmpD8A->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpD8A->hd)).f2))return 0;}
return 1;_LL7E0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD80;if(_tmpD8B->tag != 11)goto _LL7E2;else{if((((_tmpD8B->f1).aggr_info).UnknownAggr).tag != 1)goto _LL7E2;}}_LL7E1:
 return 0;_LL7E2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD8C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpD80;if(_tmpD8C->tag != 11)goto _LL7E4;else{if((((_tmpD8C->f1).aggr_info).KnownAggr).tag != 2)goto _LL7E4;_tmpD8D=*((struct Cyc_Absyn_Aggrdecl**)(((_tmpD8C->f1).aggr_info).KnownAggr).val);_tmpD8E=(_tmpD8C->f1).targs;}}_LL7E3:
# 5276
 if(_tmpD8D->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD8D->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpD91=_new_region("rgn");struct _RegionHandle*rgn=& _tmpD91;_push_region(rgn);
{struct Cyc_List_List*_tmpD92=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpD8D->tvs,_tmpD8E);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpD8D->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpD92,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpD93=0;_npop_handler(0);return _tmpD93;}}}{
int _tmpD94=1;_npop_handler(0);return _tmpD94;};}
# 5283
;_pop_region(rgn);};};_LL7E4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpD8F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpD80;if(_tmpD8F->tag != 12)goto _LL7E6;else{_tmpD90=_tmpD8F->f2;}}_LL7E5:
# 5289
 for(0;_tmpD90 != 0;_tmpD90=_tmpD90->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpD90->hd)->type))return 0;}
return 1;_LL7E6:;_LL7E7:
 return 0;_LL7D1:;}
# 5297
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpD95=e->r;void*_tmpD96=_tmpD95;struct Cyc_Absyn_Exp*_tmpD99;struct Cyc_Absyn_Exp*_tmpD9B;_LL7E9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpD97=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD96;if(_tmpD97->tag != 1)goto _LL7EB;}_LL7EA:
 return 1;_LL7EB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpD98=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD96;if(_tmpD98->tag != 11)goto _LL7ED;else{_tmpD99=_tmpD98->f1;}}_LL7EC:
 _tmpD9B=_tmpD99;goto _LL7EE;_LL7ED: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpD9A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD96;if(_tmpD9A->tag != 12)goto _LL7EF;else{_tmpD9B=_tmpD9A->f1;}}_LL7EE:
 e=_tmpD9B;continue;_LL7EF:;_LL7F0:
# 5304
 return 0;_LL7E8:;}}
# 5314
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpD9C=e->r;void*_tmpD9D=_tmpD9C;struct _tuple2*_tmpDA5;void*_tmpDA6;struct Cyc_Absyn_Exp*_tmpDA8;struct Cyc_Absyn_Exp*_tmpDA9;struct Cyc_Absyn_Exp*_tmpDAA;struct Cyc_Absyn_Exp*_tmpDAC;struct Cyc_Absyn_Exp*_tmpDAD;struct Cyc_Absyn_Exp*_tmpDAF;struct Cyc_Absyn_Exp*_tmpDB1;void*_tmpDB3;struct Cyc_Absyn_Exp*_tmpDB4;void*_tmpDB6;struct Cyc_Absyn_Exp*_tmpDB7;struct Cyc_Absyn_Exp*_tmpDB9;struct Cyc_Absyn_Exp*_tmpDBB;struct Cyc_Absyn_Exp*_tmpDBC;struct Cyc_Absyn_Exp*_tmpDBE;struct Cyc_List_List*_tmpDC0;struct Cyc_List_List*_tmpDC2;struct Cyc_List_List*_tmpDC4;enum Cyc_Absyn_Primop _tmpDC6;struct Cyc_List_List*_tmpDC7;struct Cyc_List_List*_tmpDC9;struct Cyc_List_List*_tmpDCB;_LL7F2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpD9E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpD9E->tag != 0)goto _LL7F4;}_LL7F3:
 goto _LL7F5;_LL7F4: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpD9F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpD9F->tag != 16)goto _LL7F6;}_LL7F5:
 goto _LL7F7;_LL7F6: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmpDA0=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA0->tag != 17)goto _LL7F8;}_LL7F7:
 goto _LL7F9;_LL7F8: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpDA1=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA1->tag != 18)goto _LL7FA;}_LL7F9:
 goto _LL7FB;_LL7FA: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpDA2=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA2->tag != 31)goto _LL7FC;}_LL7FB:
 goto _LL7FD;_LL7FC: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpDA3=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA3->tag != 32)goto _LL7FE;}_LL7FD:
 return 1;_LL7FE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmpDA4=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA4->tag != 1)goto _LL800;else{_tmpDA5=_tmpDA4->f1;_tmpDA6=(void*)_tmpDA4->f2;}}_LL7FF: {
# 5325
void*_tmpDCC=_tmpDA6;struct Cyc_Absyn_Vardecl*_tmpDCF;struct Cyc_Absyn_Vardecl*_tmpDD1;_LL821: {struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpDCD=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmpDCC;if(_tmpDCD->tag != 2)goto _LL823;}_LL822:
 return 1;_LL823: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpDCE=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpDCC;if(_tmpDCE->tag != 1)goto _LL825;else{_tmpDCF=_tmpDCE->f1;}}_LL824: {
# 5328
void*_tmpDD3=Cyc_Tcutil_compress(_tmpDCF->type);void*_tmpDD4=_tmpDD3;_LL82C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD4;if(_tmpDD5->tag != 8)goto _LL82E;}_LL82D:
 goto _LL82F;_LL82E: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDD6=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDD4;if(_tmpDD6->tag != 9)goto _LL830;}_LL82F:
 return 1;_LL830:;_LL831:
 return var_okay;_LL82B:;}_LL825: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDD0=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpDCC;if(_tmpDD0->tag != 4)goto _LL827;else{_tmpDD1=_tmpDD0->f1;}}_LL826:
# 5335
 if(_tmpDD1->sc == Cyc_Absyn_Static){
void*_tmpDD7=Cyc_Tcutil_compress(_tmpDD1->type);void*_tmpDD8=_tmpDD7;_LL833: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDD8;if(_tmpDD9->tag != 8)goto _LL835;}_LL834:
 goto _LL836;_LL835: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpDDA=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpDD8;if(_tmpDDA->tag != 9)goto _LL837;}_LL836:
 return 1;_LL837:;_LL838:
 return var_okay;_LL832:;}else{
# 5342
return var_okay;}_LL827: {struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*_tmpDD2=(struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpDCC;if(_tmpDD2->tag != 0)goto _LL829;}_LL828:
 return 0;_LL829:;_LL82A:
 return var_okay;_LL820:;}_LL800: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpDA7=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDA7->tag != 5)goto _LL802;else{_tmpDA8=_tmpDA7->f1;_tmpDA9=_tmpDA7->f2;_tmpDAA=_tmpDA7->f3;}}_LL801:
# 5348
 return(Cyc_Tcutil_cnst_exp(0,_tmpDA8) && 
Cyc_Tcutil_cnst_exp(0,_tmpDA9)) && 
Cyc_Tcutil_cnst_exp(0,_tmpDAA);_LL802: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmpDAB=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDAB->tag != 8)goto _LL804;else{_tmpDAC=_tmpDAB->f1;_tmpDAD=_tmpDAB->f2;}}_LL803:
# 5352
 return Cyc_Tcutil_cnst_exp(0,_tmpDAC) && Cyc_Tcutil_cnst_exp(0,_tmpDAD);_LL804: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmpDAE=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDAE->tag != 11)goto _LL806;else{_tmpDAF=_tmpDAE->f1;}}_LL805:
 _tmpDB1=_tmpDAF;goto _LL807;_LL806: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmpDB0=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDB0->tag != 12)goto _LL808;else{_tmpDB1=_tmpDB0->f1;}}_LL807:
# 5355
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDB1);_LL808: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB2=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDB2->tag != 13)goto _LL80A;else{_tmpDB3=(void*)_tmpDB2->f1;_tmpDB4=_tmpDB2->f2;if(_tmpDB2->f4 != Cyc_Absyn_No_coercion)goto _LL80A;}}_LL809:
# 5357
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDB4);_LL80A: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpDB5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDB5->tag != 13)goto _LL80C;else{_tmpDB6=(void*)_tmpDB5->f1;_tmpDB7=_tmpDB5->f2;}}_LL80B:
# 5360
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpDB7);_LL80C: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmpDB8=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDB8->tag != 14)goto _LL80E;else{_tmpDB9=_tmpDB8->f1;}}_LL80D:
# 5362
 return Cyc_Tcutil_cnst_exp(1,_tmpDB9);_LL80E: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpDBA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDBA->tag != 26)goto _LL810;else{_tmpDBB=_tmpDBA->f2;_tmpDBC=_tmpDBA->f3;}}_LL80F:
# 5364
 return Cyc_Tcutil_cnst_exp(0,_tmpDBB) && Cyc_Tcutil_cnst_exp(0,_tmpDBC);_LL810: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpDBD=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDBD->tag != 27)goto _LL812;else{_tmpDBE=_tmpDBD->f1;}}_LL811:
# 5366
 return Cyc_Tcutil_cnst_exp(0,_tmpDBE);_LL812: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpDBF=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDBF->tag != 25)goto _LL814;else{_tmpDC0=_tmpDBF->f1;}}_LL813:
 _tmpDC2=_tmpDC0;goto _LL815;_LL814: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpDC1=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDC1->tag != 29)goto _LL816;else{_tmpDC2=_tmpDC1->f2;}}_LL815:
 _tmpDC4=_tmpDC2;goto _LL817;_LL816: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpDC3=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDC3->tag != 28)goto _LL818;else{_tmpDC4=_tmpDC3->f3;}}_LL817:
# 5370
 for(0;_tmpDC4 != 0;_tmpDC4=_tmpDC4->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpDC4->hd)).f2))
return 0;}
return 1;_LL818: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmpDC5=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDC5->tag != 2)goto _LL81A;else{_tmpDC6=_tmpDC5->f1;_tmpDC7=_tmpDC5->f2;}}_LL819:
# 5375
 _tmpDC9=_tmpDC7;goto _LL81B;_LL81A: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmpDC8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDC8->tag != 23)goto _LL81C;else{_tmpDC9=_tmpDC8->f1;}}_LL81B:
 _tmpDCB=_tmpDC9;goto _LL81D;_LL81C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpDCA=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpD9D;if(_tmpDCA->tag != 30)goto _LL81E;else{_tmpDCB=_tmpDCA->f1;}}_LL81D:
# 5378
 for(0;_tmpDCB != 0;_tmpDCB=_tmpDCB->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpDCB->hd))
return 0;}
return 1;_LL81E:;_LL81F:
 return 0;_LL7F1:;}
# 5386
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5390
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5392
int Cyc_Tcutil_supports_default(void*t){
void*_tmpDDB=Cyc_Tcutil_compress(t);void*_tmpDDC=_tmpDDB;union Cyc_Absyn_Constraint*_tmpDE1;union Cyc_Absyn_Constraint*_tmpDE2;void*_tmpDE4;struct Cyc_List_List*_tmpDE6;union Cyc_Absyn_AggrInfoU _tmpDE8;struct Cyc_List_List*_tmpDE9;struct Cyc_List_List*_tmpDEB;_LL83A: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmpDDD=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmpDDC;if(_tmpDDD->tag != 0)goto _LL83C;}_LL83B:
 goto _LL83D;_LL83C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDDE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDDC;if(_tmpDDE->tag != 6)goto _LL83E;}_LL83D:
 goto _LL83F;_LL83E: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpDDF=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDDC;if(_tmpDDF->tag != 7)goto _LL840;}_LL83F:
 return 1;_LL840: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpDE0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDDC;if(_tmpDE0->tag != 5)goto _LL842;else{_tmpDE1=((_tmpDE0->f1).ptr_atts).nullable;_tmpDE2=((_tmpDE0->f1).ptr_atts).bounds;}}_LL841: {
# 5399
void*_tmpDEE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpDE2);void*_tmpDEF=_tmpDEE;_LL851: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpDF0=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpDEF;if(_tmpDF0->tag != 0)goto _LL853;}_LL852:
 return 1;_LL853:;_LL854:
# 5402
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpDE1);_LL850:;}_LL842: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDE3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDDC;if(_tmpDE3->tag != 8)goto _LL844;else{_tmpDE4=(_tmpDE3->f1).elt_type;}}_LL843:
# 5405
 return Cyc_Tcutil_supports_default(_tmpDE4);_LL844: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpDE5=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDDC;if(_tmpDE5->tag != 10)goto _LL846;else{_tmpDE6=_tmpDE5->f1;}}_LL845:
# 5407
 for(0;_tmpDE6 != 0;_tmpDE6=_tmpDE6->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpDE6->hd)).f2))return 0;}
return 1;_LL846: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDE7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDDC;if(_tmpDE7->tag != 11)goto _LL848;else{_tmpDE8=(_tmpDE7->f1).aggr_info;_tmpDE9=(_tmpDE7->f1).targs;}}_LL847: {
# 5411
struct Cyc_Absyn_Aggrdecl*_tmpDF1=Cyc_Absyn_get_known_aggrdecl(_tmpDE8);
if(_tmpDF1->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDF1->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpDF1->tvs,_tmpDE9,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpDF1->impl))->fields);}_LL848: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpDEA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDDC;if(_tmpDEA->tag != 12)goto _LL84A;else{_tmpDEB=_tmpDEA->f2;}}_LL849:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpDEB);_LL84A: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpDEC=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmpDDC;if(_tmpDEC->tag != 14)goto _LL84C;}_LL84B:
# 5417
 goto _LL84D;_LL84C: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpDED=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmpDDC;if(_tmpDED->tag != 13)goto _LL84E;}_LL84D:
 return 1;_LL84E:;_LL84F:
# 5420
 return 0;_LL839:;}
# 5425
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpDF2=t;struct Cyc_Absyn_Typedefdecl*_tmpDF4;_LL856: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpDF3=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpDF2;if(_tmpDF3->tag != 17)goto _LL858;else{_tmpDF4=_tmpDF3->f3;}}_LL857:
# 5428
 if(_tmpDF4 != 0){
struct Cyc_Absyn_Tqual _tmpDF5=_tmpDF4->tq;
if(((_tmpDF5.print_const  || _tmpDF5.q_volatile) || _tmpDF5.q_restrict) || _tmpDF5.real_const){
# 5433
const char*_tmp13C5;void*_tmp13C4[1];struct Cyc_String_pa_PrintArg_struct _tmp13C3;(_tmp13C3.tag=0,((_tmp13C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmp13C4[0]=& _tmp13C3,Cyc_Tcutil_warn(loc,((_tmp13C5="qualifier within typedef type %s is ignored",_tag_dyneither(_tmp13C5,sizeof(char),44))),_tag_dyneither(_tmp13C4,sizeof(void*),1)))))));}}
# 5436
goto _LL855;_LL858:;_LL859:
 goto _LL855;_LL855:;}
# 5441
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5443
struct _RegionHandle _tmpDF9=_new_region("rgn");struct _RegionHandle*rgn=& _tmpDF9;_push_region(rgn);{
struct Cyc_List_List*_tmpDFA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpDFB=1;_npop_handler(0);return _tmpDFB;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpDFA,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpDFC=0;_npop_handler(0);return _tmpDFC;}}}{
# 5452
int _tmpDFD=1;_npop_handler(0);return _tmpDFD;};
# 5443
;_pop_region(rgn);}
# 5458
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpDFE=Cyc_Tcutil_compress(t);void*_tmpDFF=_tmpDFE;union Cyc_Absyn_Constraint*_tmpE03;union Cyc_Absyn_Constraint*_tmpE04;_LL85B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpE00=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDFF;if(_tmpE00->tag != 6)goto _LL85D;}_LL85C:
 goto _LL85E;_LL85D: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE01=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDFF;if(_tmpE01->tag != 7)goto _LL85F;}_LL85E:
 return 1;_LL85F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE02=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDFF;if(_tmpE02->tag != 5)goto _LL861;else{_tmpE03=((_tmpE02->f1).ptr_atts).nullable;_tmpE04=((_tmpE02->f1).ptr_atts).bounds;}}_LL860: {
# 5463
void*_tmpE05=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpE04);void*_tmpE06=_tmpE05;_LL864: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmpE07=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpE06;if(_tmpE07->tag != 0)goto _LL866;}_LL865:
# 5467
 return 0;_LL866:;_LL867:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpE03);_LL863:;}_LL861:;_LL862:
# 5470
 return 0;_LL85A:;}
# 5474
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpE08=Cyc_Tcutil_compress(t);void*_tmpE09=_tmpE08;void*_tmpE0B;struct Cyc_List_List*_tmpE0D;_LL869: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE0A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE09;if(_tmpE0A->tag != 5)goto _LL86B;else{_tmpE0B=(_tmpE0A->f1).elt_typ;}}_LL86A:
 return Cyc_Tcutil_is_noreturn(_tmpE0B);_LL86B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE0C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE09;if(_tmpE0C->tag != 9)goto _LL86D;else{_tmpE0D=(_tmpE0C->f1).attributes;}}_LL86C:
# 5478
 for(0;_tmpE0D != 0;_tmpE0D=_tmpE0D->tl){
void*_tmpE0E=(void*)_tmpE0D->hd;void*_tmpE0F=_tmpE0E;_LL870: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmpE10=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpE0F;if(_tmpE10->tag != 4)goto _LL872;}_LL871:
 return 1;_LL872:;_LL873:
 continue;_LL86F:;}
# 5483
goto _LL868;_LL86D:;_LL86E:
 goto _LL868;_LL868:;}
# 5486
return 0;}
# 5491
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpE11=Cyc_Tcutil_compress(t);void*_tmpE12=_tmpE11;struct Cyc_List_List**_tmpE14;_LL875: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpE13=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpE12;if(_tmpE13->tag != 9)goto _LL877;else{_tmpE14=(struct Cyc_List_List**)&(_tmpE13->f1).attributes;}}_LL876: {
# 5494
struct Cyc_List_List*_tmpE15=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpE14,(void*)atts->hd)){
struct Cyc_List_List*_tmp13C6;*_tmpE14=((_tmp13C6=_cycalloc(sizeof(*_tmp13C6)),((_tmp13C6->hd=(void*)atts->hd,((_tmp13C6->tl=*_tmpE14,_tmp13C6))))));}}else{
# 5501
struct Cyc_List_List*_tmp13C7;_tmpE15=((_tmp13C7=_cycalloc(sizeof(*_tmp13C7)),((_tmp13C7->hd=(void*)atts->hd,((_tmp13C7->tl=_tmpE15,_tmp13C7))))));}}
return _tmpE15;}_LL877:;_LL878: {
const char*_tmp13CA;void*_tmp13C9;(_tmp13C9=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13CA="transfer_fn_type_atts",_tag_dyneither(_tmp13CA,sizeof(char),22))),_tag_dyneither(_tmp13C9,sizeof(void*),0)));}_LL874:;}
# 5508
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpE1A=Cyc_Tcutil_compress(t);void*_tmpE1B=_tmpE1A;struct Cyc_Absyn_PtrInfo*_tmpE1D;struct Cyc_Absyn_Exp*_tmpE1F;_LL87A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE1C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpE1B;if(_tmpE1C->tag != 5)goto _LL87C;else{_tmpE1D=(struct Cyc_Absyn_PtrInfo*)& _tmpE1C->f1;}}_LL87B:
# 5512
{void*_tmpE20=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpE1D->ptr_atts).bounds);void*_tmpE21=_tmpE20;struct Cyc_Absyn_Exp*_tmpE23;_LL881: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE22=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpE21;if(_tmpE22->tag != 1)goto _LL883;else{_tmpE23=_tmpE22->f1;}}_LL882:
 bound=_tmpE23;goto _LL880;_LL883:;_LL884:
 goto _LL880;_LL880:;}
# 5516
goto _LL879;_LL87C: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE1E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE1B;if(_tmpE1E->tag != 8)goto _LL87E;else{_tmpE1F=(_tmpE1E->f1).num_elts;}}_LL87D:
# 5518
 bound=_tmpE1F;
goto _LL879;_LL87E:;_LL87F:
 goto _LL879;_LL879:;}
# 5522
return bound;}
# 5527
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpE24=b;struct Cyc_Absyn_Vardecl*_tmpE26;struct Cyc_Absyn_Vardecl*_tmpE28;struct Cyc_Absyn_Vardecl*_tmpE2A;struct Cyc_Absyn_Vardecl*_tmpE2C;_LL886: {struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmpE25=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpE24;if(_tmpE25->tag != 5)goto _LL888;else{_tmpE26=_tmpE25->f1;}}_LL887:
 _tmpE28=_tmpE26;goto _LL889;_LL888: {struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE27=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpE24;if(_tmpE27->tag != 4)goto _LL88A;else{_tmpE28=_tmpE27->f1;}}_LL889:
 _tmpE2A=_tmpE28;goto _LL88B;_LL88A: {struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmpE29=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpE24;if(_tmpE29->tag != 3)goto _LL88C;else{_tmpE2A=_tmpE29->f1;}}_LL88B:
 _tmpE2C=_tmpE2A;goto _LL88D;_LL88C: {struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE2B=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpE24;if(_tmpE2B->tag != 1)goto _LL88E;else{_tmpE2C=_tmpE2B->f1;}}_LL88D:
# 5533
 if(!_tmpE2C->escapes)return _tmpE2C;
goto _LL885;_LL88E:;_LL88F:
 goto _LL885;_LL885:;}
# 5537
return 0;}
# 5541
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpE2D=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0){struct Cyc_List_List*_tmp13CB;_tmpE2D=((_tmp13CB=_cycalloc(sizeof(*_tmp13CB)),((_tmp13CB->hd=*((void**)_check_null((void**)x->hd)),((_tmp13CB->tl=_tmpE2D,_tmp13CB))))));}}}
return _tmpE2D;}
# 5548
int Cyc_Tcutil_is_array(void*t){
# 5550
void*_tmpE2F=Cyc_Tcutil_compress(t);void*_tmpE30=_tmpE2F;_LL891: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE31=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE30;if(_tmpE31->tag != 8)goto _LL893;}_LL892:
# 5552
 return 1;_LL893:;_LL894:
 return 0;_LL890:;}
# 5557
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5559
void*_tmpE32=Cyc_Tcutil_compress(t);void*_tmpE33=_tmpE32;void*_tmpE35;struct Cyc_Absyn_Tqual _tmpE36;struct Cyc_Absyn_Exp*_tmpE37;union Cyc_Absyn_Constraint*_tmpE38;unsigned int _tmpE39;_LL896: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpE34=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpE33;if(_tmpE34->tag != 8)goto _LL898;else{_tmpE35=(_tmpE34->f1).elt_type;_tmpE36=(_tmpE34->f1).tq;_tmpE37=(_tmpE34->f1).num_elts;_tmpE38=(_tmpE34->f1).zero_term;_tmpE39=(_tmpE34->f1).zt_loc;}}_LL897: {
# 5561
void*b;
if(_tmpE37 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5565
struct Cyc_Absyn_Exp*bnd=_tmpE37;
if(convert_tag){
if(bnd->topt == 0){
const char*_tmp13CE;void*_tmp13CD;(_tmp13CD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp13CE="cannot convert tag without type!",_tag_dyneither(_tmp13CE,sizeof(char),33))),_tag_dyneither(_tmp13CD,sizeof(void*),0)));}{
void*_tmpE3C=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpE3D=_tmpE3C;void*_tmpE3F;_LL89B: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpE3E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpE3D;if(_tmpE3E->tag != 19)goto _LL89D;else{_tmpE3F=(void*)_tmpE3E->f1;}}_LL89C:
# 5571
{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13D1;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13D0;b=(void*)((_tmp13D0=_cycalloc(sizeof(*_tmp13D0)),((_tmp13D0[0]=((_tmp13D1.tag=1,((_tmp13D1.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpE3F,0),0,Cyc_Absyn_No_coercion,0),_tmp13D1)))),_tmp13D0))));}
goto _LL89A;_LL89D:;_LL89E:
# 5574
 if(Cyc_Tcutil_is_const_exp(bnd)){
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13D4;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13D3;b=(void*)((_tmp13D3=_cycalloc(sizeof(*_tmp13D3)),((_tmp13D3[0]=((_tmp13D4.tag=1,((_tmp13D4.f1=bnd,_tmp13D4)))),_tmp13D3))));}else{
# 5577
b=(void*)& Cyc_Absyn_DynEither_b_val;}_LL89A:;};}else{
# 5581
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp13D7;struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp13D6;b=(void*)((_tmp13D6=_cycalloc(sizeof(*_tmp13D6)),((_tmp13D6[0]=((_tmp13D7.tag=1,((_tmp13D7.f1=bnd,_tmp13D7)))),_tmp13D6))));}}
# 5583
return Cyc_Absyn_atb_typ(_tmpE35,rgn,_tmpE36,b,_tmpE38);}_LL898:;_LL899:
# 5586
 return t;_LL895:;}
