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
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 150 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 152
extern struct _RegionHandle*Cyc_Core_unique_region;
# 159
void Cyc_Core_ufree(struct _dyneither_ptr ptr);
# 172
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
# 187
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 193
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 198
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 219
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 227
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);
# 233
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
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
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 41
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
# 46
typedef void*Cyc_Position_error_kind_t;struct Cyc_Position_Error{struct _dyneither_ptr source;unsigned int seg;void*kind;struct _dyneither_ptr desc;};
# 53
typedef struct Cyc_Position_Error*Cyc_Position_error_t;extern char Cyc_Position_Nocontext[10];struct Cyc_Position_Nocontext_exn_struct{char*tag;};struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 353
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
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
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 688 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 751
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 900
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 907
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 917
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 920
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 927
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 945
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 947
extern void*Cyc_Absyn_sint_typ;
# 959
void*Cyc_Absyn_exn_typ();
# 970
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 986
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_void_star_typ();
# 991
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 997
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1036
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1071
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1088
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1090
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1095
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1099
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1104
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1145
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1148
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1158
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1162
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1178
extern int Cyc_Absyn_no_regions;struct Cyc_PP_Ppstate;
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
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_RgnOrder_RgnPO;
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 40
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 64
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 68
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 71
void*Cyc_Tcutil_compress(void*t);
# 81
int Cyc_Tcutil_is_arithmetic_type(void*);
# 102
int Cyc_Tcutil_is_pointer_type(void*t);
# 104
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 111
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 119
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 161
int Cyc_Tcutil_unify(void*,void*);
# 166
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 256 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 264
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 266
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 276
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 334
void*Cyc_Tcutil_snd_tqt(struct _tuple11*);
# 363
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 367
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 370
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RTvar{int tag;struct Cyc_Absyn_Tvar*val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RTvar RTvar;};
# 37 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 39
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 45
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 52
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 70
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 81
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 110
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
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
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple13{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple13 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 144 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 163
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 235 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 32 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds);
# 35
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;
# 66 "toc.cyc"
extern int Cyc_noexpand_r;
# 77 "toc.cyc"
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0;
unsigned int Cyc_Toc_bounds_checks_eliminated=0;
# 86
static struct Cyc_List_List*Cyc_Toc_result_decls=0;struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple14{struct _tuple1*f1;struct _tuple1*f2;};
# 106
int Cyc_Toc_qvar_tag_cmp(struct _tuple14*x,struct _tuple14*y){
struct _tuple14 _tmp0=*x;struct _tuple1*_tmp2;struct _tuple1*_tmp3;struct _tuple14 _tmp1=_tmp0;_tmp2=_tmp1.f1;_tmp3=_tmp1.f2;{
struct _tuple14 _tmp4=*y;struct _tuple1*_tmp6;struct _tuple1*_tmp7;struct _tuple14 _tmp5=_tmp4;_tmp6=_tmp5.f1;_tmp7=_tmp5.f2;{
int i=Cyc_Absyn_qvar_cmp(_tmp2,_tmp6);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp3,_tmp7);};};}
# 115
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d,int unused){
struct Cyc_Dict_Dict*_tmpA3D;struct Cyc_Dict_Dict*_tmpA3C;struct Cyc_Set_Set**_tmpA3B;struct Cyc_List_List**_tmpA3A;struct Cyc_Dict_Dict*_tmpA39;struct Cyc_List_List**_tmpA38;struct Cyc_Toc_TocState*_tmpA37;return(_tmpA37=_region_malloc(d,sizeof(*_tmpA37)),((_tmpA37->tuple_types=(
(_tmpA38=_region_malloc(d,sizeof(*_tmpA38)),((_tmpA38[0]=0,_tmpA38)))),((_tmpA37->aggrs_so_far=(
(_tmpA39=_region_malloc(d,sizeof(*_tmpA39)),((_tmpA39[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA39)))),((_tmpA37->abs_struct_types=(
(_tmpA3A=_region_malloc(d,sizeof(*_tmpA3A)),((_tmpA3A[0]=0,_tmpA3A)))),((_tmpA37->datatypes_so_far=(
(_tmpA3B=_region_malloc(d,sizeof(*_tmpA3B)),((_tmpA3B[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA3B)))),((_tmpA37->xdatatypes_so_far=(
(_tmpA3C=_region_malloc(d,sizeof(*_tmpA3C)),((_tmpA3C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA3C)))),((_tmpA37->qvar_tags=(
(_tmpA3D=_region_malloc(d,sizeof(*_tmpA3D)),((_tmpA3D[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA3D)))),((_tmpA37->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA37)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 132
typedef struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state_t;
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple15{struct Cyc_Toc_TocState*f1;void*f2;};
# 139
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple15*)){
# 142
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpF=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmp11;struct Cyc_Toc_TocState*_tmp12;struct Cyc_Toc_TocStateWrap _tmp10=_tmpF;_tmp11=_tmp10.dyn;_tmp12=_tmp10.state;{
struct _dyneither_ptr _tmp13=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp11,sizeof(struct Cyc_Core_DynamicRegion),1));
{struct Cyc_Toc_TocStateWrap _tmpA3E;*ts=((_tmpA3E.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA3E.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA3E))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA3F;struct _tuple15 _tmp15=(_tmpA3F.f1=_tmp12,((_tmpA3F.f2=arg,_tmpA3F)));
void*_tmp16=((void*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple15*arg,void*(*body)(struct _RegionHandle*h,struct _tuple15*arg)))Cyc_Core_open_region)((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1),& _tmp15,f);
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_untag_dyneither_ptr(_tmp13,sizeof(struct Cyc_Core_DynamicRegion),1));
return _tmp16;};};};}struct _tuple16{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple17{struct Cyc_Toc_TocState*f1;struct _tuple16*f2;};struct _tuple18{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 154
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple17*env){
# 157
struct _tuple17 _tmp18=*env;struct Cyc_Toc_TocState*_tmp1A;struct _tuple1*_tmp1B;void*(*_tmp1C)(struct _tuple1*);struct _tuple17 _tmp19=_tmp18;_tmp1A=_tmp19.f1;_tmp1B=(_tmp19.f2)->f1;_tmp1C=(_tmp19.f2)->f2;{
struct _tuple18**v=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp1A->aggrs_so_far,_tmp1B);
if(v == 0)
return _tmp1C(_tmp1B);else{
# 162
struct _tuple18*_tmp1D=*v;void*_tmp1F;struct _tuple18*_tmp1E=_tmp1D;_tmp1F=_tmp1E->f2;
return _tmp1F;}};}
# 167
void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple16 _tmpA40;struct _tuple16 env=(_tmpA40.f1=q,((_tmpA40.f2=type_maker,_tmpA40)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA43;void*_tmpA42;(_tmpA42=0,Cyc_fprintf(Cyc_stderr,((_tmpA43="\n",_tag_dyneither(_tmpA43,sizeof(char),2))),_tag_dyneither(_tmpA42,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA46;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA45;(int)_throw((void*)((_tmpA45=_cycalloc_atomic(sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=Cyc_Toc_Toc_Unimplemented,_tmpA46)),_tmpA45)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA49;void*_tmpA48;(_tmpA48=0,Cyc_fprintf(Cyc_stderr,((_tmpA49="\n",_tag_dyneither(_tmpA49,sizeof(char),2))),_tag_dyneither(_tmpA48,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA4C;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA4B;(int)_throw((void*)((_tmpA4B=_cycalloc_atomic(sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=Cyc_Toc_Toc_Impossible,_tmpA4C)),_tmpA4B)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
# 205 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp29,_tmp29,_tmp29 + 5};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp2A[4]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp2A,_tmp2A,_tmp2A + 4};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp2B[4]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp2B,_tmp2B,_tmp2B + 4};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp2C[14]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp2C,_tmp2C,_tmp2C + 14};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp2D[8]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp2D,_tmp2D,_tmp2D + 8};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2E[14]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2E,_tmp2E,_tmp2E + 14};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2F[17]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2F,_tmp2F,_tmp2F + 17};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp30[16]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp30,_tmp30,_tmp30 + 16};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;
# 218
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};static char _tmp31[7]="_throw";
# 227 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp31,_tmp31,_tmp31 + 7};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1,& Cyc_Toc__throw_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp33[7]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp33,_tmp33,_tmp33 + 7};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1,& Cyc_Toc_setjmp_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp35[14]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp35,_tmp35,_tmp35 + 14};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1,& Cyc_Toc__push_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp37[13]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp37,_tmp37,_tmp37 + 13};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1,& Cyc_Toc__pop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp39[12]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp39,_tmp39,_tmp39 + 12};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1,& Cyc_Toc__exn_thrown_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp3B[14]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp3B,_tmp3B,_tmp3B + 14};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1,& Cyc_Toc__npop_handler_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp3D[12]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp3D,_tmp3D,_tmp3D + 12};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1,& Cyc_Toc__check_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp3F[28]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp3F,_tmp3F,_tmp3F + 28};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1,& Cyc_Toc__check_known_subscript_null_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp41[31]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp41,_tmp41,_tmp41 + 31};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1,& Cyc_Toc__check_known_subscript_notnull_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp43[27]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp43,_tmp43,_tmp43 + 27};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1,& Cyc_Toc__check_dyneither_subscript_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp45[15]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp45,_tmp45,_tmp45 + 15};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1,& Cyc_Toc__dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp47[15]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp47,_tmp47,_tmp47 + 15};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1,& Cyc_Toc__tag_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp49[20]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp49,_tmp49,_tmp49 + 20};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1,& Cyc_Toc__init_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp4B[21]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp4B,_tmp4B,_tmp4B + 21};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1,& Cyc_Toc__untag_dyneither_ptr_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp4D[20]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp4D,_tmp4D,_tmp4D + 20};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1,& Cyc_Toc__get_dyneither_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp4F[19]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp4F,_tmp4F,_tmp4F + 19};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1,& Cyc_Toc__get_zero_arr_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp51[24]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp51,_tmp51,_tmp51 + 24};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1,& Cyc_Toc__get_zero_arr_size_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp53[25]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp53,_tmp53,_tmp53 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1,& Cyc_Toc__get_zero_arr_size_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp55[23]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp55,_tmp55,_tmp55 + 23};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1,& Cyc_Toc__get_zero_arr_size_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp57[25]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp57,_tmp57,_tmp57 + 25};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1,& Cyc_Toc__get_zero_arr_size_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp59[26]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp59,_tmp59,_tmp59 + 26};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1,& Cyc_Toc__get_zero_arr_size_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp5B[30]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp5B,_tmp5B,_tmp5B + 30};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1,& Cyc_Toc__get_zero_arr_size_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp5D[28]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp5D,_tmp5D,_tmp5D + 28};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1,& Cyc_Toc__get_zero_arr_size_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp5F[20]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp5F,_tmp5F,_tmp5F + 20};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1,& Cyc_Toc__dyneither_ptr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp61[15]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp61,_tmp61,_tmp61 + 15};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1,& Cyc_Toc__zero_arr_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp63[20]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp63,_tmp63,_tmp63 + 20};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1,& Cyc_Toc__zero_arr_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp65[21]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp65,_tmp65,_tmp65 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1,& Cyc_Toc__zero_arr_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp67[19]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp67,_tmp67,_tmp67 + 19};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1,& Cyc_Toc__zero_arr_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp69[21]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp69,_tmp69,_tmp69 + 21};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1,& Cyc_Toc__zero_arr_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp6B[22]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp6B,_tmp6B,_tmp6B + 22};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1,& Cyc_Toc__zero_arr_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp6D[26]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp6D,_tmp6D,_tmp6D + 26};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1,& Cyc_Toc__zero_arr_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp6F[24]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp6F,_tmp6F,_tmp6F + 24};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1,& Cyc_Toc__zero_arr_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp71[28]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp71,_tmp71,_tmp71 + 28};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp73[23]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp73,_tmp73,_tmp73 + 23};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1,& Cyc_Toc__zero_arr_inplace_plus_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp75[28]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp75,_tmp75,_tmp75 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp77[29]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp77,_tmp77,_tmp77 + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp79[27]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp79,_tmp79,_tmp79 + 27};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp7B[29]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp7B,_tmp7B,_tmp7B + 29};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp7D[30]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp7D,_tmp7D,_tmp7D + 30};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp7F[34]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp7F,_tmp7F,_tmp7F + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp81[32]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp81,_tmp81,_tmp81 + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp83[33]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp83,_tmp83,_tmp83 + 33};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp85[28]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp85,_tmp85,_tmp85 + 28};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp87[33]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp87,_tmp87,_tmp87 + 33};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp89[34]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp89,_tmp89,_tmp89 + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp8B[32]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp8B,_tmp8B,_tmp8B + 32};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp8D[34]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp8D,_tmp8D,_tmp8D + 34};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp8F[35]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp8F,_tmp8F,_tmp8F + 35};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp91[39]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp91,_tmp91,_tmp91 + 39};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp93[37]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp93,_tmp93,_tmp93 + 37};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp95[10]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp95,_tmp95,_tmp95 + 10};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1,& Cyc_Toc__cycalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp97[11]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp97,_tmp97,_tmp97 + 11};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1,& Cyc_Toc__cyccalloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp99[17]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp99,_tmp99,_tmp99 + 17};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1,& Cyc_Toc__cycalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp9B[18]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp9B,_tmp9B,_tmp9B + 18};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1,& Cyc_Toc__cyccalloc_atomic_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp9D[15]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp9D,_tmp9D,_tmp9D + 15};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1,& Cyc_Toc__region_malloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp9F[15]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp9F,_tmp9F,_tmp9F + 15};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1,& Cyc_Toc__region_calloc_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmpA1[13]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmpA1,_tmpA1,_tmpA1 + 13};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1,& Cyc_Toc__check_times_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmpA3[12]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmpA3,_tmpA3,_tmpA3 + 12};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1,& Cyc_Toc__new_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmpA5[13]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmpA5,_tmpA5,_tmpA5 + 13};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1,& Cyc_Toc__push_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmpA7[12]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmpA7,_tmpA7,_tmpA7 + 12};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1,& Cyc_Toc__pop_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmpA9[16]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmpA9,_tmpA9,_tmpA9 + 16};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1,& Cyc_Toc__open_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmpAB[16]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmpAB,_tmpAB,_tmpAB + 16};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1,& Cyc_Toc__push_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmpAD[15]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmpAD,_tmpAD,_tmpAD + 15};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1,& Cyc_Toc__pop_dynregion_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmpAF[14]="_reset_region";
static struct _dyneither_ptr Cyc_Toc__reset_region_str={_tmpAF,_tmpAF,_tmpAF + 14};static struct _tuple1 Cyc_Toc__reset_region_pr={{.Loc_n={4,0}},& Cyc_Toc__reset_region_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__reset_region_re={1,& Cyc_Toc__reset_region_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__reset_region_ev={0,(void*)& Cyc_Toc__reset_region_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__reset_region_e=& Cyc_Toc__reset_region_ev;static char _tmpB1[19]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmpB1,_tmpB1,_tmpB1 + 19};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1,& Cyc_Toc__throw_arraybounds_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmpB3[29]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmpB3,_tmpB3,_tmpB3 + 29};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1,& Cyc_Toc__dyneither_ptr_decrease_size_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmpB5[11]="_swap_word";
static struct _dyneither_ptr Cyc_Toc__swap_word_str={_tmpB5,_tmpB5,_tmpB5 + 11};static struct _tuple1 Cyc_Toc__swap_word_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_word_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_word_re={1,& Cyc_Toc__swap_word_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_word_ev={0,(void*)& Cyc_Toc__swap_word_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_word_e=& Cyc_Toc__swap_word_ev;static char _tmpB7[16]="_swap_dyneither";
static struct _dyneither_ptr Cyc_Toc__swap_dyneither_str={_tmpB7,_tmpB7,_tmpB7 + 16};static struct _tuple1 Cyc_Toc__swap_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__swap_dyneither_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__swap_dyneither_re={1,& Cyc_Toc__swap_dyneither_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__swap_dyneither_ev={0,(void*)& Cyc_Toc__swap_dyneither_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__swap_dyneither_e=& Cyc_Toc__swap_dyneither_ev;static char _tmpB9[13]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmpB9,_tmpB9,_tmpB9 + 13};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1,& Cyc_Toc__throw_match_pr,(void*)& Cyc_Absyn_Unresolved_b_val};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;
# 300
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 303
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 305
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0};
# 307
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
static struct Cyc_Absyn_Stmt**skip_stmt_opt=0;
if(skip_stmt_opt == 0){
struct Cyc_Absyn_Stmt**_tmpA4D;skip_stmt_opt=((_tmpA4D=_cycalloc(sizeof(*_tmpA4D)),((_tmpA4D[0]=Cyc_Absyn_skip_stmt(0),_tmpA4D))));}
return*skip_stmt_opt;}
# 316
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 319
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA50;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA4F;return(void*)((_tmpA4F=_cycalloc(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=13,((_tmpA50.f1=t,((_tmpA50.f2=e,((_tmpA50.f3=0,((_tmpA50.f4=Cyc_Absyn_No_coercion,_tmpA50)))))))))),_tmpA4F))));}
# 322
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA53;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA52;return(void*)((_tmpA52=_cycalloc(sizeof(*_tmpA52)),((_tmpA52[0]=((_tmpA53.tag=19,((_tmpA53.f1=e,_tmpA53)))),_tmpA52))));}
# 325
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA56;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA55;return(void*)((_tmpA55=_cycalloc(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=22,((_tmpA56.f1=e1,((_tmpA56.f2=e2,_tmpA56)))))),_tmpA55))));}
# 328
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA59;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA58;return(void*)((_tmpA58=_cycalloc(sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=36,((_tmpA59.f1=s,_tmpA59)))),_tmpA58))));}
# 331
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA5C;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA5B;return(void*)((_tmpA5B=_cycalloc(sizeof(*_tmpA5B)),((_tmpA5B[0]=((_tmpA5C.tag=16,((_tmpA5C.f1=t,_tmpA5C)))),_tmpA5B))));}
# 334
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA5F;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA5E;return(void*)((_tmpA5E=_cycalloc(sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5F.tag=9,((_tmpA5F.f1=e,((_tmpA5F.f2=es,((_tmpA5F.f3=0,((_tmpA5F.f4=1,_tmpA5F)))))))))),_tmpA5E))));}
# 337
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA62;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA61;return(void*)((_tmpA61=_cycalloc(sizeof(*_tmpA61)),((_tmpA61[0]=((_tmpA62.tag=1,((_tmpA62.f1=e,_tmpA62)))),_tmpA61))));}
# 340
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA65;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA64;return(void*)((_tmpA64=_cycalloc(sizeof(*_tmpA64)),((_tmpA64[0]=((_tmpA65.tag=2,((_tmpA65.f1=s1,((_tmpA65.f2=s2,_tmpA65)))))),_tmpA64))));}
# 343
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA68;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA67;return(void*)((_tmpA67=_cycalloc(sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA68.tag=5,((_tmpA68.f1=e1,((_tmpA68.f2=e2,((_tmpA68.f3=e3,_tmpA68)))))))),_tmpA67))));}
# 346
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA6B;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA6A;return(void*)((_tmpA6A=_cycalloc(sizeof(*_tmpA6A)),((_tmpA6A[0]=((_tmpA6B.tag=20,((_tmpA6B.f1=e,((_tmpA6B.f2=n,((_tmpA6B.f3=0,((_tmpA6B.f4=0,_tmpA6B)))))))))),_tmpA6A))));}
# 349
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA6E;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA6D;return(void*)((_tmpA6D=_cycalloc(sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6E.tag=21,((_tmpA6E.f1=e,((_tmpA6E.f2=n,((_tmpA6E.f3=0,((_tmpA6E.f4=0,_tmpA6E)))))))))),_tmpA6D))));}
# 352
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 355
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA71;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA70;return(void*)((_tmpA70=_cycalloc(sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA71.tag=35,((_tmpA71.f1=tdopt,((_tmpA71.f2=ds,_tmpA71)))))),_tmpA70))));}
# 357
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA74;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA73;return(void*)((_tmpA73=_cycalloc(sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=8,((_tmpA74.f1=v,((_tmpA74.f2=s,_tmpA74)))))),_tmpA73))));}
# 360
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 364
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmpD8=e->r;void*_tmpD9=_tmpD8;struct Cyc_Absyn_Exp*_tmpDB;_LL1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpDA=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD9;if(_tmpDA->tag != 19)goto _LL3;else{_tmpDB=_tmpDA->f1;}}_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmpDB,f,loc);_LL3:;_LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 381
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 390
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 399
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 408
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 419
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmpDC=Cyc_Tcutil_compress(t);void*_tmpDD=_tmpDC;enum Cyc_Absyn_Size_of _tmpDF;_LL6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmpDE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpDD;if(_tmpDE->tag != 6)goto _LL8;else{_tmpDF=_tmpDE->f2;}}_LL7:
# 423
 switch(_tmpDF){case Cyc_Absyn_Char_sz: _LL12:
 function=fS->fchar;break;case Cyc_Absyn_Short_sz: _LL13:
 function=fS->fshort;break;case Cyc_Absyn_Int_sz: _LL14:
 function=fS->fint;break;default: _LL15: {
struct Cyc_Core_Impossible_exn_struct _tmpA7A;const char*_tmpA79;struct Cyc_Core_Impossible_exn_struct*_tmpA78;(int)_throw((void*)((_tmpA78=_cycalloc(sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA7A.tag=Cyc_Core_Impossible,((_tmpA7A.f1=((_tmpA79="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA79,sizeof(char),44))),_tmpA7A)))),_tmpA78)))));}}
# 429
goto _LL5;_LL8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE0=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDD;if(_tmpE0->tag != 7)goto _LLA;else{if(_tmpE0->f1 != 0)goto _LLA;}}_LL9:
# 431
 function=fS->ffloat;
goto _LL5;_LLA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDD;if(_tmpE1->tag != 7)goto _LLC;else{if(_tmpE1->f1 != 1)goto _LLC;}}_LLB:
# 434
 function=fS->fdouble;
goto _LL5;_LLC: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmpE2=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmpDD;if(_tmpE2->tag != 7)goto _LLE;}_LLD:
# 437
 function=fS->flongdouble;
goto _LL5;_LLE: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpE3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpDD;if(_tmpE3->tag != 5)goto _LL10;}_LLF:
# 440
 function=fS->fvoidstar;
goto _LL5;_LL10:;_LL11: {
# 443
struct Cyc_Core_Impossible_exn_struct _tmpA87;const char*_tmpA86;void*_tmpA85[1];struct Cyc_String_pa_PrintArg_struct _tmpA84;struct Cyc_Core_Impossible_exn_struct*_tmpA83;(int)_throw((void*)((_tmpA83=_cycalloc(sizeof(*_tmpA83)),((_tmpA83[0]=((_tmpA87.tag=Cyc_Core_Impossible,((_tmpA87.f1=(struct _dyneither_ptr)((_tmpA84.tag=0,((_tmpA84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA85[0]=& _tmpA84,Cyc_aprintf(((_tmpA86="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA86,sizeof(char),67))),_tag_dyneither(_tmpA85,sizeof(void*),1)))))))),_tmpA87)))),_tmpA83)))));}_LL5:;}
# 445
return function;}
# 447
struct Cyc_Absyn_Exp*Cyc_Toc_getFunction(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
return Cyc_Toc_getFunctionType(fS,(void*)_check_null(arr->topt));}
# 450
struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmpEC=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmpED=_tmpEC;void*_tmpEF;_LL18: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpEE=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpED;if(_tmpEE->tag != 5)goto _LL1A;else{_tmpEF=(_tmpEE->f1).elt_typ;}}_LL19:
# 453
 return Cyc_Toc_getFunctionType(fS,_tmpEF);_LL1A:;_LL1B: {
struct Cyc_Core_Impossible_exn_struct _tmpA8D;const char*_tmpA8C;struct Cyc_Core_Impossible_exn_struct*_tmpA8B;(int)_throw((void*)((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8D.tag=Cyc_Core_Impossible,((_tmpA8D.f1=((_tmpA8C="impossible type (not pointer)",_tag_dyneither(_tmpA8C,sizeof(char),30))),_tmpA8D)))),_tmpA8B)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 461
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmpF3=e->r;void*_tmpF4=_tmpF3;char _tmpF6;struct _dyneither_ptr _tmpF8;short _tmpFA;int _tmpFC;long long _tmpFE;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_List_List*_tmp103;struct Cyc_List_List*_tmp105;struct Cyc_List_List*_tmp107;struct Cyc_List_List*_tmp109;struct Cyc_List_List*_tmp10B;_LL1D: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF5=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpF5->tag != 0)goto _LL1F;else{if(((_tmpF5->f1).Char_c).tag != 2)goto _LL1F;_tmpF6=((struct _tuple4)((_tmpF5->f1).Char_c).val).f2;}}_LL1E:
 return _tmpF6 == '\000';_LL1F: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF7=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpF7->tag != 0)goto _LL21;else{if(((_tmpF7->f1).Wchar_c).tag != 3)goto _LL21;_tmpF8=(struct _dyneither_ptr)((_tmpF7->f1).Wchar_c).val;}}_LL20: {
# 465
unsigned long _tmp10C=Cyc_strlen((struct _dyneither_ptr)_tmpF8);
int i=0;
if(_tmp10C >= 2  && *((const char*)_check_dyneither_subscript(_tmpF8,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmpF8,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmpF8,sizeof(char),1))== 'x'  && _tmp10C >= 3) && *((const char*)_check_dyneither_subscript(_tmpF8,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp10C;++ i){
if(*((const char*)_check_dyneither_subscript(_tmpF8,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 475
return 0;}}_LL21: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpF9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpF9->tag != 0)goto _LL23;else{if(((_tmpF9->f1).Short_c).tag != 4)goto _LL23;_tmpFA=((struct _tuple5)((_tmpF9->f1).Short_c).val).f2;}}_LL22:
 return _tmpFA == 0;_LL23: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFB=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpFB->tag != 0)goto _LL25;else{if(((_tmpFB->f1).Int_c).tag != 5)goto _LL25;_tmpFC=((struct _tuple6)((_tmpFB->f1).Int_c).val).f2;}}_LL24:
 return _tmpFC == 0;_LL25: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFD=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpFD->tag != 0)goto _LL27;else{if(((_tmpFD->f1).LongLong_c).tag != 6)goto _LL27;_tmpFE=((struct _tuple7)((_tmpFD->f1).LongLong_c).val).f2;}}_LL26:
 return _tmpFE == 0;_LL27: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmpFF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmpFF->tag != 0)goto _LL29;else{if(((_tmpFF->f1).Null_c).tag != 1)goto _LL29;}}_LL28:
 return 1;_LL29: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp100=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp100->tag != 13)goto _LL2B;else{_tmp101=_tmp100->f2;}}_LL2A:
 return Cyc_Toc_is_zero(_tmp101);_LL2B: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp102=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp102->tag != 23)goto _LL2D;else{_tmp103=_tmp102->f1;}}_LL2C:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmp103);_LL2D: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp104=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp104->tag != 25)goto _LL2F;else{_tmp105=_tmp104->f1;}}_LL2E:
 _tmp107=_tmp105;goto _LL30;_LL2F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp106=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp106->tag != 28)goto _LL31;else{_tmp107=_tmp106->f3;}}_LL30:
 _tmp109=_tmp107;goto _LL32;_LL31: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp108=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp108->tag != 24)goto _LL33;else{_tmp109=_tmp108->f2;}}_LL32:
 _tmp10B=_tmp109;goto _LL34;_LL33: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp10A=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpF4;if(_tmp10A->tag != 35)goto _LL35;else{_tmp10B=_tmp10A->f2;}}_LL34:
# 486
 for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple19*)_tmp10B->hd)).f2))return 0;}
return 1;_LL35:;_LL36:
 return 0;_LL1C:;}
# 494
static int Cyc_Toc_is_nullable(void*t){
void*_tmp10D=Cyc_Tcutil_compress(t);void*_tmp10E=_tmp10D;union Cyc_Absyn_Constraint*_tmp110;_LL38: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp10F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10E;if(_tmp10F->tag != 5)goto _LL3A;else{_tmp110=((_tmp10F->f1).ptr_atts).nullable;}}_LL39:
# 497
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp110);_LL3A:;_LL3B: {
const char*_tmpA90;void*_tmpA8F;(_tmpA8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA90="is_nullable",_tag_dyneither(_tmpA90,sizeof(char),12))),_tag_dyneither(_tmpA8F,sizeof(void*),0)));}_LL37:;}
# 503
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr s,struct _tuple1*x){
unsigned int slen=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int len=9 + slen;
union Cyc_Absyn_Nmspace _tmp114;struct _dyneither_ptr _tmp115;struct _tuple1*_tmp113=x;_tmp114=_tmp113->f1;_tmp115=*_tmp113->f2;{
unsigned int vlen=Cyc_strlen((struct _dyneither_ptr)_tmp115);
len +=1 + vlen;{
struct Cyc_List_List*nms;
{union Cyc_Absyn_Nmspace _tmp116=_tmp114;struct Cyc_List_List*_tmp117;struct Cyc_List_List*_tmp118;struct Cyc_List_List*_tmp119;_LL3D: if((_tmp116.Loc_n).tag != 4)goto _LL3F;_LL3E:
 nms=0;goto _LL3C;_LL3F: if((_tmp116.Rel_n).tag != 1)goto _LL41;_tmp117=(struct Cyc_List_List*)(_tmp116.Rel_n).val;_LL40:
 nms=_tmp117;goto _LL3C;_LL41: if((_tmp116.Abs_n).tag != 2)goto _LL43;_tmp118=(struct Cyc_List_List*)(_tmp116.Abs_n).val;_LL42:
 nms=_tmp118;goto _LL3C;_LL43: if((_tmp116.C_n).tag != 3)goto _LL3C;_tmp119=(struct Cyc_List_List*)(_tmp116.C_n).val;_LL44:
 nms=_tmp119;goto _LL3C;_LL3C:;}
# 516
{struct Cyc_List_List*_tmp11A=nms;for(0;_tmp11A != 0;_tmp11A=_tmp11A->tl){
len +=1 + Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp11A->hd));}}{
char*_tmpA92;unsigned int _tmpA91;struct _dyneither_ptr buf=(_tmpA91=len,((_tmpA92=_cyccalloc_atomic(sizeof(char),_tmpA91),_tag_dyneither(_tmpA92,sizeof(char),_tmpA91))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11B=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp11D;struct _dyneither_ptr*_tmp11C=_tmp11B;_tmp11D=*_tmp11C;
{char _tmpA95;char _tmpA94;struct _dyneither_ptr _tmpA93;(_tmpA93=p,((_tmpA94=*((char*)_check_dyneither_subscript(_tmpA93,sizeof(char),0)),((_tmpA95='_',((_get_dyneither_size(_tmpA93,sizeof(char))== 1  && (_tmpA94 == '\000'  && _tmpA95 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA93.curr)=_tmpA95)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp11D);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp11D));}
# 527
{char _tmpA98;char _tmpA97;struct _dyneither_ptr _tmpA96;(_tmpA96=p,((_tmpA97=*((char*)_check_dyneither_subscript(_tmpA96,sizeof(char),0)),((_tmpA98='_',((_get_dyneither_size(_tmpA96,sizeof(char))== 1  && (_tmpA97 == '\000'  && _tmpA98 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA96.curr)=_tmpA98)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp115);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA99;Cyc_strcpy(p,((_tmpA99="_struct",_tag_dyneither(_tmpA99,sizeof(char),8))));}
return(struct _dyneither_ptr)buf;};};};}struct _tuple20{struct Cyc_Toc_TocState*f1;struct _tuple14*f2;};static char _tmp13E[8]="*bogus*";
# 539
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple20*env){
# 542
static struct _dyneither_ptr bogus_string={_tmp13E,_tmp13E,_tmp13E + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
static struct _tuple14 pair={& bogus_qvar,& bogus_qvar};
# 546
struct _tuple20 _tmp127=*env;struct Cyc_Dict_Dict*_tmp129;struct _tuple14*_tmp12A;struct _tuple20 _tmp128=_tmp127;_tmp129=(_tmp128.f1)->qvar_tags;_tmp12A=_tmp128.f2;{
struct _tuple14 _tmp12B=*_tmp12A;struct _tuple1*_tmp12D;struct _tuple1*_tmp12E;struct _tuple14 _tmp12C=_tmp12B;_tmp12D=_tmp12C.f1;_tmp12E=_tmp12C.f2;{
struct _handler_cons _tmp12F;_push_handler(& _tmp12F);{int _tmp131=0;if(setjmp(_tmp12F.handler))_tmp131=1;if(!_tmp131){
{struct _tuple1*_tmp132=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple14*,struct _tuple14*),struct _tuple14*k))Cyc_Dict_lookup_other)(*_tmp129,Cyc_Toc_qvar_tag_cmp,_tmp12A);_npop_handler(0);return _tmp132;};_pop_handler();}else{void*_tmp130=(void*)_exn_thrown;void*_tmp134=_tmp130;void*_tmp136;_LL46: {struct Cyc_Dict_Absent_exn_struct*_tmp135=(struct Cyc_Dict_Absent_exn_struct*)_tmp134;if(_tmp135->tag != Cyc_Dict_Absent)goto _LL48;}_LL47: {
# 552
struct _tuple14*_tmpA9A;struct _tuple14*_tmp137=(_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->f1=_tmp12D,((_tmpA9A->f2=_tmp12E,_tmpA9A)))));
union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr _tmp13A;struct _tuple1*_tmp138=_tmp12D;_tmp139=_tmp138->f1;_tmp13A=*_tmp138->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13A,_tmp12E);
struct _dyneither_ptr*_tmpA9D;struct _tuple1*_tmpA9C;struct _tuple1*res=(_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->f1=_tmp139,((_tmpA9C->f2=((_tmpA9D=_cycalloc(sizeof(*_tmpA9D)),((_tmpA9D[0]=newvar,_tmpA9D)))),_tmpA9C)))));
*_tmp129=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp129,_tmp137,res);
return res;};}_LL48: _tmp136=_tmp134;_LL49:(void)_throw(_tmp136);_LL45:;}};};};}
# 561
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpA9E;struct _tuple14 env=(_tmpA9E.f1=fieldname,((_tmpA9E.f2=dtname,_tmpA9E)));
return((struct _tuple1*(*)(struct _tuple14*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple20*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 566
static void*Cyc_Toc_typ_to_c(void*t);struct _tuple21{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple22{void*f1;struct Cyc_List_List*f2;};
# 570
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple21*env){
# 573
struct _tuple21 _tmp140=*env;struct Cyc_List_List**_tmp142;struct Cyc_List_List*_tmp143;struct _tuple21 _tmp141=_tmp140;_tmp142=(_tmp141.f1)->tuple_types;_tmp143=_tmp141.f2;
# 575
{struct Cyc_List_List*_tmp144=*_tmp142;for(0;_tmp144 != 0;_tmp144=_tmp144->tl){
struct _tuple22*_tmp145=(struct _tuple22*)_tmp144->hd;void*_tmp147;struct Cyc_List_List*_tmp148;struct _tuple22*_tmp146=_tmp145;_tmp147=_tmp146->f1;_tmp148=_tmp146->f2;{
struct Cyc_List_List*_tmp149=_tmp143;
for(0;_tmp149 != 0  && _tmp148 != 0;(_tmp149=_tmp149->tl,_tmp148=_tmp148->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple11*)_tmp149->hd)).f2,(void*)_tmp148->hd))
break;}
if(_tmp149 == 0  && _tmp148 == 0)
return _tmp147;};}}{
# 585
struct Cyc_Int_pa_PrintArg_struct _tmpAA6;void*_tmpAA5[1];const char*_tmpAA4;struct _dyneither_ptr*_tmpAA3;struct _dyneither_ptr*xname=(_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3[0]=(struct _dyneither_ptr)((_tmpAA6.tag=1,((_tmpAA6.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAA5[0]=& _tmpAA6,Cyc_aprintf(((_tmpAA4="_tuple%d",_tag_dyneither(_tmpAA4,sizeof(char),9))),_tag_dyneither(_tmpAA5,sizeof(void*),1)))))))),_tmpAA3)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp143);
struct Cyc_List_List*_tmp14A=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpAA9;struct Cyc_List_List*_tmpAA8;_tmp14A=((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->hd=((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->name=Cyc_Absyn_fieldname(i),((_tmpAA9->tq=Cyc_Toc_mt_tq,((_tmpAA9->type=(void*)ts2->hd,((_tmpAA9->width=0,((_tmpAA9->attributes=0,((_tmpAA9->requires_clause=0,_tmpAA9)))))))))))))),((_tmpAA8->tl=_tmp14A,_tmpAA8))))));}}
_tmp14A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14A);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAAE;struct _tuple1*_tmpAAD;struct Cyc_Absyn_Aggrdecl*_tmpAAC;struct Cyc_Absyn_Aggrdecl*_tmp14D=(_tmpAAC=_cycalloc(sizeof(*_tmpAAC)),((_tmpAAC->kind=Cyc_Absyn_StructA,((_tmpAAC->sc=Cyc_Absyn_Public,((_tmpAAC->name=(
(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD->f1=Cyc_Absyn_Rel_n(0),((_tmpAAD->f2=xname,_tmpAAD)))))),((_tmpAAC->tvs=0,((_tmpAAC->impl=(
(_tmpAAE=_cycalloc(sizeof(*_tmpAAE)),((_tmpAAE->exist_vars=0,((_tmpAAE->rgn_po=0,((_tmpAAE->fields=_tmp14A,((_tmpAAE->tagged=0,_tmpAAE)))))))))),((_tmpAAC->attributes=0,_tmpAAC)))))))))))));
# 600
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAB4;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAB3;struct Cyc_List_List*_tmpAB2;Cyc_Toc_result_decls=((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->hd=Cyc_Absyn_new_decl((void*)((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4[0]=((_tmpAB3.tag=5,((_tmpAB3.f1=_tmp14D,_tmpAB3)))),_tmpAB4)))),0),((_tmpAB2->tl=Cyc_Toc_result_decls,_tmpAB2))))));}
{struct _tuple22*_tmpAB7;struct Cyc_List_List*_tmpAB6;*_tmp142=((_tmpAB6=_region_malloc(d,sizeof(*_tmpAB6)),((_tmpAB6->hd=((_tmpAB7=_region_malloc(d,sizeof(*_tmpAB7)),((_tmpAB7->f1=x,((_tmpAB7->f2=ts,_tmpAB7)))))),((_tmpAB6->tl=*_tmp142,_tmpAB6))))));}
return x;};};}
# 605
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple23{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple24{struct Cyc_Toc_TocState*f1;struct _tuple23*f2;};struct _tuple25{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 614
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple24*env){
# 622
struct _tuple24 _tmp15A=*env;struct Cyc_List_List**_tmp15C;struct _tuple1*_tmp15D;struct Cyc_List_List*_tmp15E;struct Cyc_List_List*_tmp15F;struct Cyc_List_List*_tmp160;struct _tuple24 _tmp15B=_tmp15A;_tmp15C=(_tmp15B.f1)->abs_struct_types;_tmp15D=(_tmp15B.f2)->f1;_tmp15E=(_tmp15B.f2)->f2;_tmp15F=(_tmp15B.f2)->f3;_tmp160=(_tmp15B.f2)->f4;
# 627
{struct Cyc_List_List*_tmp161=*_tmp15C;for(0;_tmp161 != 0;_tmp161=_tmp161->tl){
struct _tuple25*_tmp162=(struct _tuple25*)_tmp161->hd;struct _tuple1*_tmp164;struct Cyc_List_List*_tmp165;void*_tmp166;struct _tuple25*_tmp163=_tmp162;_tmp164=_tmp163->f1;_tmp165=_tmp163->f2;_tmp166=_tmp163->f3;
# 630
if(Cyc_Absyn_qvar_cmp(_tmp164,_tmp15D)== 0  && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp15F)== ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp165)){
int okay=1;
{struct Cyc_List_List*_tmp167=_tmp15F;for(0;_tmp167 != 0;(_tmp167=_tmp167->tl,_tmp165=_tmp165->tl)){
void*_tmp168=(void*)_tmp167->hd;
void*_tmp169=(void*)((struct Cyc_List_List*)_check_null(_tmp165))->hd;
# 637
{struct Cyc_Absyn_Kind*_tmp16A=Cyc_Tcutil_typ_kind(_tmp168);struct Cyc_Absyn_Kind*_tmp16B=_tmp16A;_LL4B: if(_tmp16B->kind != Cyc_Absyn_EffKind)goto _LL4D;_LL4C:
 goto _LL4E;_LL4D: if(_tmp16B->kind != Cyc_Absyn_RgnKind)goto _LL4F;_LL4E:
# 642
 continue;_LL4F:;_LL50:
# 645
 if(Cyc_Tcutil_unify(_tmp168,_tmp169) || Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(_tmp168),Cyc_Toc_typ_to_c(_tmp169)))
# 647
continue;
# 650
okay=0;
goto _LL4A;_LL4A:;}
# 653
break;}}
# 655
if(okay)
# 657
return _tmp166;}}}{
# 665
struct Cyc_Int_pa_PrintArg_struct _tmpABF;void*_tmpABE[1];const char*_tmpABD;struct _dyneither_ptr*_tmpABC;struct _dyneither_ptr*xname=(_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC[0]=(struct _dyneither_ptr)((_tmpABF.tag=1,((_tmpABF.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpABE[0]=& _tmpABF,Cyc_aprintf(((_tmpABD="_tuple%d",_tag_dyneither(_tmpABD,sizeof(char),9))),_tag_dyneither(_tmpABE,sizeof(void*),1)))))))),_tmpABC)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp16C=0;
# 669
{struct _tuple25*_tmpAC2;struct Cyc_List_List*_tmpAC1;*_tmp15C=((_tmpAC1=_region_malloc(d,sizeof(*_tmpAC1)),((_tmpAC1->hd=((_tmpAC2=_region_malloc(d,sizeof(*_tmpAC2)),((_tmpAC2->f1=_tmp15D,((_tmpAC2->f2=_tmp15F,((_tmpAC2->f3=x,_tmpAC2)))))))),((_tmpAC1->tl=*_tmp15C,_tmpAC1))))));}{
# 672
struct _RegionHandle _tmp16F=_new_region("r");struct _RegionHandle*r=& _tmp16F;_push_region(r);
{struct Cyc_List_List*_tmp170=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E,_tmp15F);
for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
struct Cyc_Absyn_Aggrfield*_tmp171=(struct Cyc_Absyn_Aggrfield*)_tmp160->hd;
void*t=_tmp171->type;
struct Cyc_List_List*atts=_tmp171->attributes;
# 680
if(_tmp160->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAC8;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAC7;struct Cyc_List_List*_tmpAC6;atts=((_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6->hd=(void*)((_tmpAC8=_cycalloc_atomic(sizeof(*_tmpAC8)),((_tmpAC8[0]=((_tmpAC7.tag=6,((_tmpAC7.f1=- 1,_tmpAC7)))),_tmpAC8)))),((_tmpAC6->tl=atts,_tmpAC6))))));}
# 683
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp170,t));
# 686
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpACE;struct Cyc_Absyn_ArrayInfo _tmpACD;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpACC;t=(void*)((_tmpACC=_cycalloc(sizeof(*_tmpACC)),((_tmpACC[0]=((_tmpACE.tag=8,((_tmpACE.f1=((_tmpACD.elt_type=Cyc_Absyn_void_star_typ(),((_tmpACD.tq=Cyc_Absyn_empty_tqual(0),((_tmpACD.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpACD.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpACD.zt_loc=0,_tmpACD)))))))))),_tmpACE)))),_tmpACC))));}{
# 690
struct Cyc_Absyn_Aggrfield*_tmpAD1;struct Cyc_List_List*_tmpAD0;_tmp16C=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1->name=_tmp171->name,((_tmpAD1->tq=Cyc_Toc_mt_tq,((_tmpAD1->type=t,((_tmpAD1->width=_tmp171->width,((_tmpAD1->attributes=atts,((_tmpAD1->requires_clause=0,_tmpAD1)))))))))))))),((_tmpAD0->tl=_tmp16C,_tmpAD0))))));};}
# 692
_tmp16C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16C);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAD6;struct _tuple1*_tmpAD5;struct Cyc_Absyn_Aggrdecl*_tmpAD4;struct Cyc_Absyn_Aggrdecl*_tmp17A=(_tmpAD4=_cycalloc(sizeof(*_tmpAD4)),((_tmpAD4->kind=Cyc_Absyn_StructA,((_tmpAD4->sc=Cyc_Absyn_Public,((_tmpAD4->name=(
(_tmpAD5=_cycalloc(sizeof(*_tmpAD5)),((_tmpAD5->f1=Cyc_Absyn_Rel_n(0),((_tmpAD5->f2=xname,_tmpAD5)))))),((_tmpAD4->tvs=0,((_tmpAD4->impl=(
(_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6->exist_vars=0,((_tmpAD6->rgn_po=0,((_tmpAD6->fields=_tmp16C,((_tmpAD6->tagged=0,_tmpAD6)))))))))),((_tmpAD4->attributes=0,_tmpAD4)))))))))))));
# 700
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpADC;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpADB;struct Cyc_List_List*_tmpADA;Cyc_Toc_result_decls=((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->hd=Cyc_Absyn_new_decl((void*)((_tmpADC=_cycalloc(sizeof(*_tmpADC)),((_tmpADC[0]=((_tmpADB.tag=5,((_tmpADB.f1=_tmp17A,_tmpADB)))),_tmpADC)))),0),((_tmpADA->tl=Cyc_Toc_result_decls,_tmpADA))))));}{
void*_tmp17E=x;_npop_handler(0);return _tmp17E;};};}
# 673
;_pop_region(r);};};}
# 704
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 708
struct _tuple23 _tmpADD;struct _tuple23 env=(_tmpADD.f1=struct_name,((_tmpADD.f2=type_vars,((_tmpADD.f3=type_args,((_tmpADD.f4=fields,_tmpADD)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 715
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp187=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpAEA;const char*_tmpAE9;void*_tmpAE8[1];struct Cyc_Int_pa_PrintArg_struct _tmpAE7;struct _tuple1*_tmpAE6;struct _tuple1*res=(_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->f1=Cyc_Absyn_Loc_n,((_tmpAE6->f2=((_tmpAEA=_cycalloc(sizeof(*_tmpAEA)),((_tmpAEA[0]=(struct _dyneither_ptr)((_tmpAE7.tag=1,((_tmpAE7.f1=(unsigned int)_tmp187,((_tmpAE8[0]=& _tmpAE7,Cyc_aprintf(((_tmpAE9="_tmp%X",_tag_dyneither(_tmpAE9,sizeof(char),7))),_tag_dyneither(_tmpAE8,sizeof(void*),1)))))))),_tmpAEA)))),_tmpAE6)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 723
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp18D=*env;struct Cyc_Xarray_Xarray*_tmp18F;struct _tuple26 _tmp18E=_tmp18D;_tmp18F=(_tmp18E.f1)->temp_labels;{
int _tmp190=Cyc_Toc_fresh_label_counter ++;
if(_tmp190 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp18F))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp18F,_tmp190);{
struct Cyc_Int_pa_PrintArg_struct _tmpAF2;void*_tmpAF1[1];const char*_tmpAF0;struct _dyneither_ptr*_tmpAEF;struct _dyneither_ptr*res=(_tmpAEF=_cycalloc(sizeof(*_tmpAEF)),((_tmpAEF[0]=(struct _dyneither_ptr)((_tmpAF2.tag=1,((_tmpAF2.f1=(unsigned int)_tmp190,((_tmpAF1[0]=& _tmpAF2,Cyc_aprintf(((_tmpAF0="_LL%X",_tag_dyneither(_tmpAF0,sizeof(char),6))),_tag_dyneither(_tmpAF1,sizeof(void*),1)))))))),_tmpAEF)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp18F,res)!= _tmp190){
const char*_tmpAF5;void*_tmpAF4;(_tmpAF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAF5="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpAF5,sizeof(char),43))),_tag_dyneither(_tmpAF4,sizeof(void*),0)));}
return res;};};}
# 734
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 741
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp197=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp197))->hd)->name)!= 0){
++ ans;
_tmp197=_tmp197->tl;}
# 748
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 754
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 759
struct _tuple9 _tmp198=*a;struct _dyneither_ptr*_tmp19A;struct Cyc_Absyn_Tqual _tmp19B;void*_tmp19C;struct _tuple9 _tmp199=_tmp198;_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;_tmp19C=_tmp199.f3;{
struct _tuple9*_tmpAF6;return(_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6->f1=_tmp19A,((_tmpAF6->f2=_tmp19B,((_tmpAF6->f3=Cyc_Toc_typ_to_c(_tmp19C),_tmpAF6)))))));};}
# 762
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp19E=*x;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct _tuple11 _tmp19F=_tmp19E;_tmp1A0=_tmp19F.f1;_tmp1A1=_tmp19F.f2;{
struct _tuple11*_tmpAF7;return(_tmpAF7=_cycalloc(sizeof(*_tmpAF7)),((_tmpAF7->f1=_tmp1A0,((_tmpAF7->f2=Cyc_Toc_typ_to_c(_tmp1A1),_tmpAF7)))));};}
# 781 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp1A3=Cyc_Tcutil_compress(t);void*_tmp1A4=_tmp1A3;void*_tmp1A6;struct Cyc_Absyn_Tqual _tmp1A7;struct Cyc_Absyn_Exp*_tmp1A8;union Cyc_Absyn_Constraint*_tmp1A9;unsigned int _tmp1AA;void*_tmp1AC;_LL52: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A4;if(_tmp1A5->tag != 8)goto _LL54;else{_tmp1A6=(_tmp1A5->f1).elt_type;_tmp1A7=(_tmp1A5->f1).tq;_tmp1A8=(_tmp1A5->f1).num_elts;_tmp1A9=(_tmp1A5->f1).zero_term;_tmp1AA=(_tmp1A5->f1).zt_loc;}}_LL53:
# 784
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1A6),_tmp1A7);_LL54:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1AB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4;if(_tmp1AB->tag != 1)goto _LL56;else{_tmp1AC=(void*)_tmp1AB->f2;}}if(!(_tmp1AC != 0))goto _LL56;_LL55:
 return Cyc_Toc_typ_to_c_array(_tmp1AC);_LL56:;_LL57:
 return Cyc_Toc_typ_to_c(t);_LL51:;}
# 790
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 792
struct Cyc_Absyn_Aggrfield*_tmpAF8;return(_tmpAF8=_cycalloc(sizeof(*_tmpAF8)),((_tmpAF8->name=f->name,((_tmpAF8->tq=Cyc_Toc_mt_tq,((_tmpAF8->type=
# 794
Cyc_Toc_typ_to_c(f->type),((_tmpAF8->width=f->width,((_tmpAF8->attributes=f->attributes,((_tmpAF8->requires_clause=0,_tmpAF8)))))))))))));}
# 799
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 801
return;}
# 804
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpAF9;cs=((_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAF9))));}
# 809
return*cs;}
# 811
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAFA;r=((_tmpAFA=_cycalloc(sizeof(*_tmpAFA)),((_tmpAFA[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpAFA))));}
# 816
return*r;}
# 818
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAFB;r=((_tmpAFB=_cycalloc(sizeof(*_tmpAFB)),((_tmpAFB[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAFB))));}
# 823
return*r;}
# 825
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1B1=Cyc_Tcutil_compress(t);void*_tmp1B2=_tmp1B1;struct Cyc_Absyn_Tvar*_tmp1B4;_LL59: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1B3=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B2;if(_tmp1B3->tag != 2)goto _LL5B;else{_tmp1B4=_tmp1B3->f1;}}_LL5A:
# 828
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL5B:;_LL5C:
 return 0;_LL58:;}
# 832
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1B5=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1B6=_tmp1B5;_LL5E: if(_tmp1B6->kind != Cyc_Absyn_AnyKind)goto _LL60;_LL5F:
 return 1;_LL60:;_LL61:
 return 0;_LL5D:;}
# 839
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1B7=t;void**_tmp1BA;struct Cyc_Absyn_Tvar*_tmp1BC;struct Cyc_Absyn_Datatypedecl*_tmp1BF;struct Cyc_Absyn_Datatypefield*_tmp1C0;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;union Cyc_Absyn_Constraint*_tmp1C5;void*_tmp1C9;struct Cyc_Absyn_Tqual _tmp1CA;struct Cyc_Absyn_Exp*_tmp1CB;unsigned int _tmp1CC;struct Cyc_Absyn_Tqual _tmp1CE;void*_tmp1CF;struct Cyc_List_List*_tmp1D0;int _tmp1D1;struct Cyc_Absyn_VarargInfo*_tmp1D2;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D5;enum Cyc_Absyn_AggrKind _tmp1D7;struct Cyc_List_List*_tmp1D8;union Cyc_Absyn_AggrInfoU _tmp1DA;struct Cyc_List_List*_tmp1DB;struct _tuple1*_tmp1DD;struct Cyc_List_List*_tmp1DF;struct _tuple1*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_Absyn_Typedefdecl*_tmp1E3;void*_tmp1E4;void*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Aggrdecl*_tmp1F3;struct Cyc_Absyn_Enumdecl*_tmp1F6;struct Cyc_Absyn_Datatypedecl*_tmp1F9;void**_tmp1FA;_LL63: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1B7;if(_tmp1B8->tag != 0)goto _LL65;}_LL64:
 return t;_LL65: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1B9=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7;if(_tmp1B9->tag != 1)goto _LL67;else{_tmp1BA=(void**)((void**)& _tmp1B9->f2);}}_LL66:
# 843
 if(*_tmp1BA == 0){
*_tmp1BA=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 847
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1BA));_LL67: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B7;if(_tmp1BB->tag != 2)goto _LL69;else{_tmp1BC=_tmp1BB->f1;}}_LL68:
# 849
 if((Cyc_Tcutil_tvar_kind(_tmp1BC,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 852
return(void*)& Cyc_Absyn_VoidType_val;else{
# 854
return Cyc_Absyn_void_star_typ();}_LL69: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1BD=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1B7;if(_tmp1BD->tag != 3)goto _LL6B;}_LL6A:
# 856
 return(void*)& Cyc_Absyn_VoidType_val;_LL6B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1BE=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7;if(_tmp1BE->tag != 4)goto _LL6D;else{if((((_tmp1BE->f1).field_info).KnownDatatypefield).tag != 2)goto _LL6D;_tmp1BF=((struct _tuple2)(((_tmp1BE->f1).field_info).KnownDatatypefield).val).f1;_tmp1C0=((struct _tuple2)(((_tmp1BE->f1).field_info).KnownDatatypefield).val).f2;}}_LL6C:
# 858
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1C0->name,_tmp1BF->name));_LL6D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C1->tag != 4)goto _LL6F;}_LL6E: {
const char*_tmpAFE;void*_tmpAFD;(_tmpAFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFE="unresolved DatatypeFieldType",_tag_dyneither(_tmpAFE,sizeof(char),29))),_tag_dyneither(_tmpAFD,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C2->tag != 5)goto _LL71;else{_tmp1C3=(_tmp1C2->f1).elt_typ;_tmp1C4=(_tmp1C2->f1).elt_tq;_tmp1C5=((_tmp1C2->f1).ptr_atts).bounds;}}_LL70:
# 863
 _tmp1C3=Cyc_Toc_typ_to_c(_tmp1C3);{
void*_tmp1FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1C5);void*_tmp1FE=_tmp1FD;_LLA0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1FF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1FE;if(_tmp1FF->tag != 0)goto _LLA2;}_LLA1:
 return Cyc_Toc_dyneither_ptr_typ;_LLA2:;_LLA3:
 return Cyc_Absyn_star_typ(_tmp1C3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1C4,Cyc_Absyn_false_conref);_LL9F:;};_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1C6=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C6->tag != 6)goto _LL73;}_LL72:
# 868
 goto _LL74;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1C7=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C7->tag != 7)goto _LL75;}_LL74:
 return t;_LL75: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1C8=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C8->tag != 8)goto _LL77;else{_tmp1C9=(_tmp1C8->f1).elt_type;_tmp1CA=(_tmp1C8->f1).tq;_tmp1CB=(_tmp1C8->f1).num_elts;_tmp1CC=(_tmp1C8->f1).zt_loc;}}_LL76:
# 871
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1C9),_tmp1CA,_tmp1CB,Cyc_Absyn_false_conref,_tmp1CC);_LL77: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1CD=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B7;if(_tmp1CD->tag != 9)goto _LL79;else{_tmp1CE=(_tmp1CD->f1).ret_tqual;_tmp1CF=(_tmp1CD->f1).ret_typ;_tmp1D0=(_tmp1CD->f1).args;_tmp1D1=(_tmp1CD->f1).c_varargs;_tmp1D2=(_tmp1CD->f1).cyc_varargs;_tmp1D3=(_tmp1CD->f1).attributes;}}_LL78: {
# 877
struct Cyc_List_List*_tmp200=0;
for(0;_tmp1D3 != 0;_tmp1D3=_tmp1D3->tl){
void*_tmp201=(void*)_tmp1D3->hd;void*_tmp202=_tmp201;_LLA5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp203=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp202;if(_tmp203->tag != 4)goto _LLA7;}_LLA6:
 goto _LLA8;_LLA7: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp204=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp202;if(_tmp204->tag != 5)goto _LLA9;}_LLA8:
 goto _LLAA;_LLA9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp205=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp202;if(_tmp205->tag != 19)goto _LLAB;}_LLAA:
 continue;_LLAB: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp206=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp202;if(_tmp206->tag != 22)goto _LLAD;}_LLAC:
 continue;_LLAD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp207=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp202;if(_tmp207->tag != 21)goto _LLAF;}_LLAE:
 continue;_LLAF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp208=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp202;if(_tmp208->tag != 20)goto _LLB1;}_LLB0:
 continue;_LLB1:;_LLB2:
{struct Cyc_List_List*_tmpAFF;_tmp200=((_tmpAFF=_cycalloc(sizeof(*_tmpAFF)),((_tmpAFF->hd=(void*)_tmp1D3->hd,((_tmpAFF->tl=_tmp200,_tmpAFF))))));}goto _LLA4;_LLA4:;}{
# 888
struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1D0);
if(_tmp1D2 != 0){
# 891
void*_tmp20B=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1D2->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpB00;struct _tuple9*_tmp20C=(_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->f1=_tmp1D2->name,((_tmpB00->f2=_tmp1D2->tq,((_tmpB00->f3=_tmp20B,_tmpB00)))))));
struct Cyc_List_List*_tmpB01;_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp20A,((_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->hd=_tmp20C,((_tmpB01->tl=0,_tmpB01)))))));}{
# 895
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB07;struct Cyc_Absyn_FnInfo _tmpB06;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB05;return(void*)((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05[0]=((_tmpB07.tag=9,((_tmpB07.f1=((_tmpB06.tvars=0,((_tmpB06.effect=0,((_tmpB06.ret_tqual=_tmp1CE,((_tmpB06.ret_typ=Cyc_Toc_typ_to_c(_tmp1CF),((_tmpB06.args=_tmp20A,((_tmpB06.c_varargs=_tmp1D1,((_tmpB06.cyc_varargs=0,((_tmpB06.rgn_po=0,((_tmpB06.attributes=_tmp200,_tmpB06)))))))))))))))))),_tmpB07)))),_tmpB05))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D4->tag != 10)goto _LL7B;else{_tmp1D5=_tmp1D4->f1;}}_LL7A:
# 900
 _tmp1D5=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1D5);
return Cyc_Toc_add_tuple_type(_tmp1D5);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D6->tag != 12)goto _LL7D;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL7C: {
# 905
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB0A;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB09;return(void*)((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09[0]=((_tmpB0A.tag=12,((_tmpB0A.f1=_tmp1D7,((_tmpB0A.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1D8),_tmpB0A)))))),_tmpB09))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D9->tag != 11)goto _LL7F;else{_tmp1DA=(_tmp1D9->f1).aggr_info;_tmp1DB=(_tmp1D9->f1).targs;}}_LL7E:
# 909
{union Cyc_Absyn_AggrInfoU _tmp214=_tmp1DA;_LLB4: if((_tmp214.UnknownAggr).tag != 1)goto _LLB6;_LLB5:
 return t;_LLB6:;_LLB7:
 goto _LLB3;_LLB3:;}{
# 913
struct Cyc_Absyn_Aggrdecl*_tmp215=Cyc_Absyn_get_known_aggrdecl(_tmp1DA);
# 917
if(_tmp215->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp215->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp216=_tmp215->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp215->impl))->fields;
if(_tmp216 == 0)return Cyc_Toc_aggrdecl_type(_tmp215->name,Cyc_Absyn_strctq);
for(0;_tmp216->tl != 0;_tmp216=_tmp216->tl){;}{
void*_tmp217=((struct Cyc_Absyn_Aggrfield*)_tmp216->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp217))){
# 927
struct _RegionHandle _tmp218=_new_region("r");struct _RegionHandle*r=& _tmp218;_push_region(r);
{struct Cyc_List_List*_tmp219=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp215->tvs,_tmp1DB);
void*_tmp21A=Cyc_Tcutil_rsubstitute(r,_tmp219,_tmp217);
if(Cyc_Toc_is_abstract_type(_tmp21A)){void*_tmp21B=Cyc_Toc_aggrdecl_type(_tmp215->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp21B;}{
void*_tmp21C=Cyc_Toc_add_struct_type(_tmp215->name,_tmp215->tvs,_tmp1DB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp215->impl))->fields);_npop_handler(0);return _tmp21C;};}
# 928
;_pop_region(r);}
# 933
return Cyc_Toc_aggrdecl_type(_tmp215->name,Cyc_Absyn_strctq);};};};_LL7F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1DC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1B7;if(_tmp1DC->tag != 13)goto _LL81;else{_tmp1DD=_tmp1DC->f1;}}_LL80:
 return t;_LL81: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1DE=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1B7;if(_tmp1DE->tag != 14)goto _LL83;else{_tmp1DF=_tmp1DE->f1;}}_LL82:
 Cyc_Toc_enumfields_to_c(_tmp1DF);return t;_LL83: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B7;if(_tmp1E0->tag != 17)goto _LL85;else{_tmp1E1=_tmp1E0->f1;_tmp1E2=_tmp1E0->f2;_tmp1E3=_tmp1E0->f3;_tmp1E4=(void*)_tmp1E0->f4;}}_LL84:
# 937
 if(_tmp1E4 == 0  || Cyc_noexpand_r){
if(_tmp1E2 != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB0D;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB0C;return(void*)((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C[0]=((_tmpB0D.tag=17,((_tmpB0D.f1=_tmp1E1,((_tmpB0D.f2=0,((_tmpB0D.f3=_tmp1E3,((_tmpB0D.f4=0,_tmpB0D)))))))))),_tmpB0C))));}else{
return t;}}else{
# 942
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB10;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB0F;return(void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB10.tag=17,((_tmpB10.f1=_tmp1E1,((_tmpB10.f2=0,((_tmpB10.f3=_tmp1E3,((_tmpB10.f4=Cyc_Toc_typ_to_c(_tmp1E4),_tmpB10)))))))))),_tmpB0F))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1E5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1B7;if(_tmp1E5->tag != 19)goto _LL87;}_LL86:
 return Cyc_Absyn_uint_typ;_LL87: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1E6=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1B7;if(_tmp1E6->tag != 15)goto _LL89;else{_tmp1E7=(void*)_tmp1E6->f1;}}_LL88:
 return Cyc_Toc_rgn_typ();_LL89: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1B7;if(_tmp1E8->tag != 16)goto _LL8B;}_LL8A:
 return Cyc_Toc_dyn_rgn_typ();_LL8B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1B7;if(_tmp1E9->tag != 20)goto _LL8D;}_LL8C:
# 948
 goto _LL8E;_LL8D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1B7;if(_tmp1EA->tag != 21)goto _LL8F;}_LL8E:
 goto _LL90;_LL8F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1EB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1B7;if(_tmp1EB->tag != 22)goto _LL91;}_LL90:
 goto _LL92;_LL91: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1B7;if(_tmp1EC->tag != 23)goto _LL93;}_LL92:
 goto _LL94;_LL93: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1ED=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1B7;if(_tmp1ED->tag != 24)goto _LL95;}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1B7;if(_tmp1EE->tag != 25)goto _LL97;}_LL96:
 return Cyc_Absyn_void_star_typ();_LL97: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1EF=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1B7;if(_tmp1EF->tag != 18)goto _LL99;else{_tmp1F0=_tmp1EF->f1;}}_LL98:
# 958
 return t;_LL99: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B7;if(_tmp1F1->tag != 26)goto _LL9B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1F2=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1F1->f1)->r;if(_tmp1F2->tag != 0)goto _LL9B;else{_tmp1F3=_tmp1F2->f1;}}}_LL9A:
# 960
 Cyc_Toc_aggrdecl_to_c(_tmp1F3,1);
if(_tmp1F3->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1F3->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1F3->name,Cyc_Absyn_strctq);}_LL9B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F4=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B7;if(_tmp1F4->tag != 26)goto _LL9D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1F5=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1F4->f1)->r;if(_tmp1F5->tag != 1)goto _LL9D;else{_tmp1F6=_tmp1F5->f1;}}}_LL9C:
# 965
 Cyc_Toc_enumdecl_to_c(_tmp1F6);
return t;_LL9D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F7=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B7;if(_tmp1F7->tag != 26)goto _LL62;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1F8=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1F7->f1)->r;if(_tmp1F8->tag != 2)goto _LL62;else{_tmp1F9=_tmp1F8->f1;}}_tmp1FA=_tmp1F7->f2;}}_LL9E:
# 968
 Cyc_Toc_datatypedecl_to_c(_tmp1F9);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1FA)));_LL62:;}
# 973
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp221=t;void*_tmp223;struct Cyc_Absyn_Tqual _tmp224;_LLB9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp221;if(_tmp222->tag != 8)goto _LLBB;else{_tmp223=(_tmp222->f1).elt_type;_tmp224=(_tmp222->f1).tq;}}_LLBA:
# 976
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp223,(void*)& Cyc_Absyn_HeapRgn_val,_tmp224,Cyc_Absyn_false_conref),e);_LLBB:;_LLBC:
 return Cyc_Toc_cast_it(t,e);_LLB8:;}
# 983
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp225=Cyc_Tcutil_compress(t);void*_tmp226=_tmp225;void*_tmp230;union Cyc_Absyn_AggrInfoU _tmp232;struct Cyc_List_List*_tmp234;struct Cyc_Absyn_Datatypedecl*_tmp236;struct Cyc_Absyn_Datatypefield*_tmp237;struct Cyc_List_List*_tmp239;_LLBE: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp226;if(_tmp227->tag != 0)goto _LLC0;}_LLBF:
 return 1;_LLC0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp226;if(_tmp228->tag != 2)goto _LLC2;}_LLC1:
 return 0;_LLC2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp226;if(_tmp229->tag != 6)goto _LLC4;}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp22A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp226;if(_tmp22A->tag != 13)goto _LLC6;}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp226;if(_tmp22B->tag != 14)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp22C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp226;if(_tmp22C->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp226;if(_tmp22D->tag != 9)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp22E=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp226;if(_tmp22E->tag != 19)goto _LLCE;}_LLCD:
 return 1;_LLCE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp226;if(_tmp22F->tag != 8)goto _LLD0;else{_tmp230=(_tmp22F->f1).elt_type;}}_LLCF:
 return Cyc_Toc_atomic_typ(_tmp230);_LLD0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp231=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp226;if(_tmp231->tag != 11)goto _LLD2;else{_tmp232=(_tmp231->f1).aggr_info;}}_LLD1:
# 1000
{union Cyc_Absyn_AggrInfoU _tmp23E=_tmp232;_LLE3: if((_tmp23E.UnknownAggr).tag != 1)goto _LLE5;_LLE4:
 return 0;_LLE5:;_LLE6:
 goto _LLE2;_LLE2:;}{
# 1004
struct Cyc_Absyn_Aggrdecl*_tmp23F=Cyc_Absyn_get_known_aggrdecl(_tmp232);
if(_tmp23F->impl == 0)
return 0;
{struct Cyc_List_List*_tmp240=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp23F->impl))->fields;for(0;_tmp240 != 0;_tmp240=_tmp240->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp240->hd)->type))return 0;}}
return 1;};_LLD2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp233=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp226;if(_tmp233->tag != 12)goto _LLD4;else{_tmp234=_tmp233->f2;}}_LLD3:
# 1011
 for(0;_tmp234 != 0;_tmp234=_tmp234->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp234->hd)->type))return 0;}
return 1;_LLD4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp226;if(_tmp235->tag != 4)goto _LLD6;else{if((((_tmp235->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD6;_tmp236=((struct _tuple2)(((_tmp235->f1).field_info).KnownDatatypefield).val).f1;_tmp237=((struct _tuple2)(((_tmp235->f1).field_info).KnownDatatypefield).val).f2;}}_LLD5:
# 1015
 _tmp239=_tmp237->typs;goto _LLD7;_LLD6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp238=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp226;if(_tmp238->tag != 10)goto _LLD8;else{_tmp239=_tmp238->f1;}}_LLD7:
# 1017
 for(0;_tmp239 != 0;_tmp239=_tmp239->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp239->hd)).f2))return 0;}
return 1;_LLD8: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp23A=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp226;if(_tmp23A->tag != 3)goto _LLDA;}_LLD9:
# 1022
 goto _LLDB;_LLDA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226;if(_tmp23B->tag != 5)goto _LLDC;}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp23C=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp226;if(_tmp23C->tag != 16)goto _LLDE;}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp23D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp226;if(_tmp23D->tag != 15)goto _LLE0;}_LLDF:
 return 0;_LLE0:;_LLE1: {
const char*_tmpB14;void*_tmpB13[1];struct Cyc_String_pa_PrintArg_struct _tmpB12;(_tmpB12.tag=0,((_tmpB12.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB13[0]=& _tmpB12,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB14="atomic_typ:  bad type %s",_tag_dyneither(_tmpB14,sizeof(char),25))),_tag_dyneither(_tmpB13,sizeof(void*),1)))))));}_LLBD:;}
# 1030
static int Cyc_Toc_is_void_star(void*t){
void*_tmp244=Cyc_Tcutil_compress(t);void*_tmp245=_tmp244;void*_tmp247;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp246=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245;if(_tmp246->tag != 5)goto _LLEA;else{_tmp247=(_tmp246->f1).elt_typ;}}_LLE9: {
# 1033
void*_tmp248=Cyc_Tcutil_compress(_tmp247);void*_tmp249=_tmp248;_LLED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp24A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LLEF;}_LLEE:
 return 1;_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLEA:;_LLEB:
# 1037
 return 0;_LLE7:;}
# 1041
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1045
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1050
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp24B=Cyc_Tcutil_compress(t);void*_tmp24C=_tmp24B;union Cyc_Absyn_AggrInfoU _tmp24E;struct Cyc_List_List*_tmp250;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp24D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24C;if(_tmp24D->tag != 11)goto _LLF4;else{_tmp24E=(_tmp24D->f1).aggr_info;}}_LLF3: {
# 1053
struct Cyc_Absyn_Aggrdecl*_tmp251=Cyc_Absyn_get_known_aggrdecl(_tmp24E);
if(_tmp251->impl == 0){
const char*_tmpB17;void*_tmpB16;(_tmpB16=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB17="is_poly_field: type missing fields",_tag_dyneither(_tmpB17,sizeof(char),35))),_tag_dyneither(_tmpB16,sizeof(void*),0)));}
_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp251->impl))->fields;goto _LLF5;}_LLF4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp24F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C;if(_tmp24F->tag != 12)goto _LLF6;else{_tmp250=_tmp24F->f2;}}_LLF5: {
# 1058
struct Cyc_Absyn_Aggrfield*_tmp254=Cyc_Absyn_lookup_field(_tmp250,f);
if(_tmp254 == 0){
const char*_tmpB1B;void*_tmpB1A[1];struct Cyc_String_pa_PrintArg_struct _tmpB19;(_tmpB19.tag=0,((_tmpB19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB1A[0]=& _tmpB19,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1B="is_poly_field: bad field %s",_tag_dyneither(_tmpB1B,sizeof(char),28))),_tag_dyneither(_tmpB1A,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp254->type);}_LLF6:;_LLF7: {
const char*_tmpB1F;void*_tmpB1E[1];struct Cyc_String_pa_PrintArg_struct _tmpB1D;(_tmpB1D.tag=0,((_tmpB1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB1E[0]=& _tmpB1D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1F="is_poly_field: bad type %s",_tag_dyneither(_tmpB1F,sizeof(char),27))),_tag_dyneither(_tmpB1E,sizeof(void*),1)))))));}_LLF1:;}
# 1069
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp25B=e->r;void*_tmp25C=_tmp25B;struct Cyc_Absyn_Exp*_tmp25E;struct _dyneither_ptr*_tmp25F;struct Cyc_Absyn_Exp*_tmp261;struct _dyneither_ptr*_tmp262;_LLF9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp25D->tag != 20)goto _LLFB;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;}}_LLFA:
# 1072
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp25E->topt),_tmp25F) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp260->tag != 21)goto _LLFD;else{_tmp261=_tmp260->f1;_tmp262=_tmp260->f2;}}_LLFC: {
# 1075
void*_tmp263=Cyc_Tcutil_compress((void*)_check_null(_tmp261->topt));void*_tmp264=_tmp263;void*_tmp266;_LL100: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp265=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp264;if(_tmp265->tag != 5)goto _LL102;else{_tmp266=(_tmp265->f1).elt_typ;}}_LL101:
# 1077
 return Cyc_Toc_is_poly_field(_tmp266,_tmp262) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LL102:;_LL103: {
const char*_tmpB23;void*_tmpB22[1];struct Cyc_String_pa_PrintArg_struct _tmpB21;(_tmpB21.tag=0,((_tmpB21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp261->topt))),((_tmpB22[0]=& _tmpB21,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB23="is_poly_project: bad type %s",_tag_dyneither(_tmpB23,sizeof(char),29))),_tag_dyneither(_tmpB22,sizeof(void*),1)))))));}_LLFF:;}_LLFD:;_LLFE:
# 1080
 return 0;_LLF8:;}
# 1085
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB24;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB24=_cycalloc(sizeof(*_tmpB24)),((_tmpB24->hd=s,((_tmpB24->tl=0,_tmpB24)))))),0);}
# 1089
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB25;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB25=_cycalloc(sizeof(*_tmpB25)),((_tmpB25->hd=s,((_tmpB25->tl=0,_tmpB25)))))),0);}
# 1093
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1099
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB26[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB26[1]=s,((_tmpB26[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB26,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB27[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB27[1]=n,((_tmpB27[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB27,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1107
struct Cyc_Absyn_Exp*_tmpB28[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB28[1]=n,((_tmpB28[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB28,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB29[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB29[2]=n,((_tmpB29[1]=s,((_tmpB29[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB29,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1115
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB2A;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB2A=_cycalloc(sizeof(*_tmpB2A)),((_tmpB2A->hd=e,((_tmpB2A->tl=0,_tmpB2A)))))),0);}
# 1119
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB2B[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB2B,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1125
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1133
int is_string=0;
{void*_tmp272=e->r;void*_tmp273=_tmp272;_LL105: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp274=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp273;if(_tmp274->tag != 0)goto _LL107;else{if(((_tmp274->f1).String_c).tag != 8)goto _LL107;}}_LL106:
 is_string=1;goto _LL104;_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp273;if(_tmp275->tag != 0)goto _LL109;else{if(((_tmp275->f1).Wstring_c).tag != 9)goto _LL109;}}_LL108:
 is_string=1;goto _LL104;_LL109:;_LL10A:
 goto _LL104;_LL104:;}{
# 1139
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB31;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB30;struct Cyc_List_List*_tmpB2F;Cyc_Toc_result_decls=((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=Cyc_Absyn_new_decl((void*)((_tmpB31=_cycalloc(sizeof(*_tmpB31)),((_tmpB31[0]=((_tmpB30.tag=0,((_tmpB30.f1=vd,_tmpB30)))),_tmpB31)))),0),((_tmpB2F->tl=Cyc_Toc_result_decls,_tmpB2F))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1149
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1151
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1153
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB38;struct _tuple19*_tmpB37;struct _tuple19*_tmpB36;struct _tuple19*_tmpB35[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB35[2]=(
# 1156
(_tmpB36=_cycalloc(sizeof(*_tmpB36)),((_tmpB36->f1=0,((_tmpB36->f2=xplussz,_tmpB36)))))),((_tmpB35[1]=(
# 1155
(_tmpB37=_cycalloc(sizeof(*_tmpB37)),((_tmpB37->f1=0,((_tmpB37->f2=xexp,_tmpB37)))))),((_tmpB35[0]=(
# 1154
(_tmpB38=_cycalloc(sizeof(*_tmpB38)),((_tmpB38->f1=0,((_tmpB38->f2=xexp,_tmpB38)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB35,sizeof(struct _tuple19*),3)))))))),0);}
# 1157
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1200 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1202
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp27E;struct Cyc_Toc_Env*_tmp27D=nv;_tmp27E=_tmp27D->toplevel;
return _tmp27E;}
# 1206
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp280;struct Cyc_Toc_Env*_tmp27F=nv;_tmp280=_tmp27F->in_lhs;
return*_tmp280;}
# 1211
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp282;struct Cyc_Toc_Env*_tmp281=nv;_tmp282=_tmp281->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp282,x);}
# 1217
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB40;struct Cyc_Absyn_Exp**_tmpB3F;int*_tmpB3E;struct Cyc_Toc_Env*_tmpB3D;return(_tmpB3D=_region_malloc(r,sizeof(*_tmpB3D)),((_tmpB3D->break_lab=(struct _dyneither_ptr**)0,((_tmpB3D->continue_lab=(struct _dyneither_ptr**)0,((_tmpB3D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB3D->varmap=(struct Cyc_Dict_Dict)
# 1221
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB3D->toplevel=(int)1,((_tmpB3D->in_lhs=(int*)(
# 1223
(_tmpB3E=_region_malloc(r,sizeof(*_tmpB3E)),((_tmpB3E[0]=0,_tmpB3E)))),((_tmpB3D->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB3D->struct_info).varsizeexp=((_tmpB3F=_cycalloc(sizeof(*_tmpB3F)),((_tmpB3F[0]=0,_tmpB3F)))),(((_tmpB3D->struct_info).lhs_exp=0,_tmpB3D->struct_info)))),((_tmpB3D->in_sizeof=(int*)(
(_tmpB40=_region_malloc(r,sizeof(*_tmpB40)),((_tmpB40[0]=0,_tmpB40)))),((_tmpB3D->rgn=(struct _RegionHandle*)r,_tmpB3D)))))))))))))))))));}
# 1229
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp288;struct _dyneither_ptr**_tmp289;struct Cyc_Toc_FallthruInfo*_tmp28A;struct Cyc_Dict_Dict _tmp28B;int _tmp28C;int*_tmp28D;struct Cyc_Toc_StructInfo _tmp28E;int*_tmp28F;struct Cyc_Toc_Env*_tmp287=e;_tmp288=_tmp287->break_lab;_tmp289=_tmp287->continue_lab;_tmp28A=_tmp287->fallthru_info;_tmp28B=_tmp287->varmap;_tmp28C=_tmp287->toplevel;_tmp28D=_tmp287->in_lhs;_tmp28E=_tmp287->struct_info;_tmp28F=_tmp287->in_sizeof;{
struct Cyc_Toc_Env*_tmpB41;return(_tmpB41=_region_malloc(r,sizeof(*_tmpB41)),((_tmpB41->break_lab=(struct _dyneither_ptr**)_tmp288,((_tmpB41->continue_lab=(struct _dyneither_ptr**)_tmp289,((_tmpB41->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28A,((_tmpB41->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28B),((_tmpB41->toplevel=(int)_tmp28C,((_tmpB41->in_lhs=(int*)_tmp28D,((_tmpB41->struct_info=(struct Cyc_Toc_StructInfo)_tmp28E,((_tmpB41->in_sizeof=(int*)_tmp28F,((_tmpB41->rgn=(struct _RegionHandle*)r,_tmpB41)))))))))))))))))));};}
# 1234
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp292;struct _dyneither_ptr**_tmp293;struct Cyc_Toc_FallthruInfo*_tmp294;struct Cyc_Dict_Dict _tmp295;int _tmp296;int*_tmp297;struct Cyc_Toc_StructInfo _tmp298;int*_tmp299;struct Cyc_Toc_Env*_tmp291=e;_tmp292=_tmp291->break_lab;_tmp293=_tmp291->continue_lab;_tmp294=_tmp291->fallthru_info;_tmp295=_tmp291->varmap;_tmp296=_tmp291->toplevel;_tmp297=_tmp291->in_lhs;_tmp298=_tmp291->struct_info;_tmp299=_tmp291->in_sizeof;{
struct Cyc_Toc_Env*_tmpB42;return(_tmpB42=_region_malloc(r,sizeof(*_tmpB42)),((_tmpB42->break_lab=(struct _dyneither_ptr**)_tmp292,((_tmpB42->continue_lab=(struct _dyneither_ptr**)_tmp293,((_tmpB42->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp294,((_tmpB42->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp295),((_tmpB42->toplevel=(int)0,((_tmpB42->in_lhs=(int*)_tmp297,((_tmpB42->struct_info=(struct Cyc_Toc_StructInfo)_tmp298,((_tmpB42->in_sizeof=(int*)_tmp299,((_tmpB42->rgn=(struct _RegionHandle*)r,_tmpB42)))))))))))))))))));};}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp29C;struct _dyneither_ptr**_tmp29D;struct Cyc_Toc_FallthruInfo*_tmp29E;struct Cyc_Dict_Dict _tmp29F;int _tmp2A0;int*_tmp2A1;struct Cyc_Toc_StructInfo _tmp2A2;int*_tmp2A3;struct Cyc_Toc_Env*_tmp29B=e;_tmp29C=_tmp29B->break_lab;_tmp29D=_tmp29B->continue_lab;_tmp29E=_tmp29B->fallthru_info;_tmp29F=_tmp29B->varmap;_tmp2A0=_tmp29B->toplevel;_tmp2A1=_tmp29B->in_lhs;_tmp2A2=_tmp29B->struct_info;_tmp2A3=_tmp29B->in_sizeof;{
struct Cyc_Toc_Env*_tmpB43;return(_tmpB43=_region_malloc(r,sizeof(*_tmpB43)),((_tmpB43->break_lab=(struct _dyneither_ptr**)_tmp29C,((_tmpB43->continue_lab=(struct _dyneither_ptr**)_tmp29D,((_tmpB43->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29E,((_tmpB43->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29F),((_tmpB43->toplevel=(int)1,((_tmpB43->in_lhs=(int*)_tmp2A1,((_tmpB43->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A2,((_tmpB43->in_sizeof=(int*)_tmp2A3,((_tmpB43->rgn=(struct _RegionHandle*)r,_tmpB43)))))))))))))))))));};}
# 1242
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp2A6;struct Cyc_Toc_Env*_tmp2A5=e;_tmp2A6=_tmp2A5->in_lhs;
*_tmp2A6=b;}
# 1246
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp2A8;struct Cyc_Toc_Env*_tmp2A7=e;_tmp2A8=_tmp2A7->in_sizeof;{
int _tmp2A9=*_tmp2A8;
*_tmp2A8=b;
return _tmp2A9;};}
# 1252
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp2AB;struct Cyc_Toc_Env*_tmp2AA=e;_tmp2AB=_tmp2AA->in_sizeof;
return*_tmp2AB;}
# 1257
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1259
struct _dyneither_ptr**_tmp2AD;struct _dyneither_ptr**_tmp2AE;struct Cyc_Toc_FallthruInfo*_tmp2AF;struct Cyc_Dict_Dict _tmp2B0;int _tmp2B1;int*_tmp2B2;struct Cyc_Absyn_Exp**_tmp2B3;int*_tmp2B4;struct Cyc_Toc_Env*_tmp2AC=e;_tmp2AD=_tmp2AC->break_lab;_tmp2AE=_tmp2AC->continue_lab;_tmp2AF=_tmp2AC->fallthru_info;_tmp2B0=_tmp2AC->varmap;_tmp2B1=_tmp2AC->toplevel;_tmp2B2=_tmp2AC->in_lhs;_tmp2B3=(_tmp2AC->struct_info).varsizeexp;_tmp2B4=_tmp2AC->in_sizeof;{
struct Cyc_Toc_Env*_tmpB44;return(_tmpB44=_region_malloc(r,sizeof(*_tmpB44)),((_tmpB44->break_lab=(struct _dyneither_ptr**)_tmp2AD,((_tmpB44->continue_lab=(struct _dyneither_ptr**)_tmp2AE,((_tmpB44->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AF,((_tmpB44->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B0),((_tmpB44->toplevel=(int)_tmp2B1,((_tmpB44->in_lhs=(int*)_tmp2B2,((_tmpB44->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB44->struct_info).varsizeexp=_tmp2B3,(((_tmpB44->struct_info).lhs_exp=exp,_tmpB44->struct_info)))),((_tmpB44->in_sizeof=(int*)_tmp2B4,((_tmpB44->rgn=(struct _RegionHandle*)r,_tmpB44)))))))))))))))))));};}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2B6=(*x).f1;union Cyc_Absyn_Nmspace _tmp2B7=_tmp2B6;_LL10C: if((_tmp2B7.Rel_n).tag != 1)goto _LL10E;_LL10D: {
# 1268
const char*_tmpB48;void*_tmpB47[1];struct Cyc_String_pa_PrintArg_struct _tmpB46;(_tmpB46.tag=0,((_tmpB46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB47[0]=& _tmpB46,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB48="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB48,sizeof(char),30))),_tag_dyneither(_tmpB47,sizeof(void*),1)))))));}_LL10E:;_LL10F:
 goto _LL10B;_LL10B:;}{
# 1271
struct _dyneither_ptr**_tmp2BC;struct _dyneither_ptr**_tmp2BD;struct Cyc_Toc_FallthruInfo*_tmp2BE;struct Cyc_Dict_Dict _tmp2BF;int _tmp2C0;int*_tmp2C1;struct Cyc_Toc_StructInfo _tmp2C2;int*_tmp2C3;struct Cyc_Toc_Env*_tmp2BB=e;_tmp2BC=_tmp2BB->break_lab;_tmp2BD=_tmp2BB->continue_lab;_tmp2BE=_tmp2BB->fallthru_info;_tmp2BF=_tmp2BB->varmap;_tmp2C0=_tmp2BB->toplevel;_tmp2C1=_tmp2BB->in_lhs;_tmp2C2=_tmp2BB->struct_info;_tmp2C3=_tmp2BB->in_sizeof;{
struct Cyc_Dict_Dict _tmp2C4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BF),x,y);
struct Cyc_Toc_Env*_tmpB49;return(_tmpB49=_region_malloc(r,sizeof(*_tmpB49)),((_tmpB49->break_lab=(struct _dyneither_ptr**)_tmp2BC,((_tmpB49->continue_lab=(struct _dyneither_ptr**)_tmp2BD,((_tmpB49->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2BE,((_tmpB49->varmap=(struct Cyc_Dict_Dict)_tmp2C4,((_tmpB49->toplevel=(int)_tmp2C0,((_tmpB49->in_lhs=(int*)_tmp2C1,((_tmpB49->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C2,((_tmpB49->in_sizeof=(int*)_tmp2C3,((_tmpB49->rgn=(struct _RegionHandle*)r,_tmpB49)))))))))))))))))));};};}
# 1278
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2C7;struct _dyneither_ptr**_tmp2C8;struct Cyc_Toc_FallthruInfo*_tmp2C9;struct Cyc_Dict_Dict _tmp2CA;int _tmp2CB;int*_tmp2CC;struct Cyc_Toc_StructInfo _tmp2CD;int*_tmp2CE;struct Cyc_Toc_Env*_tmp2C6=e;_tmp2C7=_tmp2C6->break_lab;_tmp2C8=_tmp2C6->continue_lab;_tmp2C9=_tmp2C6->fallthru_info;_tmp2CA=_tmp2C6->varmap;_tmp2CB=_tmp2C6->toplevel;_tmp2CC=_tmp2C6->in_lhs;_tmp2CD=_tmp2C6->struct_info;_tmp2CE=_tmp2C6->in_sizeof;{
struct Cyc_Toc_Env*_tmpB4A;return(_tmpB4A=_region_malloc(r,sizeof(*_tmpB4A)),((_tmpB4A->break_lab=(struct _dyneither_ptr**)0,((_tmpB4A->continue_lab=(struct _dyneither_ptr**)0,((_tmpB4A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C9,((_tmpB4A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CA),((_tmpB4A->toplevel=(int)_tmp2CB,((_tmpB4A->in_lhs=(int*)_tmp2CC,((_tmpB4A->struct_info=(struct Cyc_Toc_StructInfo)_tmp2CD,((_tmpB4A->in_sizeof=(int*)_tmp2CE,((_tmpB4A->rgn=(struct _RegionHandle*)r,_tmpB4A)))))))))))))))))));};}
# 1284
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1289
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB4B;fallthru_vars=((_tmpB4B=_region_malloc(r,sizeof(*_tmpB4B)),((_tmpB4B->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB4B->tl=fallthru_vars,_tmpB4B))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2D2;struct _dyneither_ptr**_tmp2D3;struct Cyc_Toc_FallthruInfo*_tmp2D4;struct Cyc_Dict_Dict _tmp2D5;int _tmp2D6;int*_tmp2D7;struct Cyc_Toc_StructInfo _tmp2D8;int*_tmp2D9;struct Cyc_Toc_Env*_tmp2D1=e;_tmp2D2=_tmp2D1->break_lab;_tmp2D3=_tmp2D1->continue_lab;_tmp2D4=_tmp2D1->fallthru_info;_tmp2D5=_tmp2D1->varmap;_tmp2D6=_tmp2D1->toplevel;_tmp2D7=_tmp2D1->in_lhs;_tmp2D8=_tmp2D1->struct_info;_tmp2D9=_tmp2D1->in_sizeof;{
struct Cyc_Dict_Dict _tmp2DB;struct Cyc_Toc_Env*_tmp2DA=next_case_env;_tmp2DB=_tmp2DA->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB4C;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB4C=_region_malloc(r,sizeof(*_tmpB4C)),((_tmpB4C->label=fallthru_l,((_tmpB4C->binders=fallthru_vars,((_tmpB4C->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DB),_tmpB4C)))))));
struct _dyneither_ptr**_tmpB4F;struct Cyc_Toc_Env*_tmpB4E;return(_tmpB4E=_region_malloc(r,sizeof(*_tmpB4E)),((_tmpB4E->break_lab=(struct _dyneither_ptr**)((_tmpB4F=_region_malloc(r,sizeof(*_tmpB4F)),((_tmpB4F[0]=break_l,_tmpB4F)))),((_tmpB4E->continue_lab=(struct _dyneither_ptr**)_tmp2D3,((_tmpB4E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB4E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D5),((_tmpB4E->toplevel=(int)_tmp2D6,((_tmpB4E->in_lhs=(int*)_tmp2D7,((_tmpB4E->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D8,((_tmpB4E->in_sizeof=(int*)_tmp2D9,((_tmpB4E->rgn=(struct _RegionHandle*)r,_tmpB4E)))))))))))))))))));};};};}
# 1301
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1304
struct _dyneither_ptr**_tmp2E0;struct _dyneither_ptr**_tmp2E1;struct Cyc_Toc_FallthruInfo*_tmp2E2;struct Cyc_Dict_Dict _tmp2E3;int _tmp2E4;int*_tmp2E5;struct Cyc_Toc_StructInfo _tmp2E6;int*_tmp2E7;struct Cyc_Toc_Env*_tmp2DF=e;_tmp2E0=_tmp2DF->break_lab;_tmp2E1=_tmp2DF->continue_lab;_tmp2E2=_tmp2DF->fallthru_info;_tmp2E3=_tmp2DF->varmap;_tmp2E4=_tmp2DF->toplevel;_tmp2E5=_tmp2DF->in_lhs;_tmp2E6=_tmp2DF->struct_info;_tmp2E7=_tmp2DF->in_sizeof;{
struct _dyneither_ptr**_tmpB52;struct Cyc_Toc_Env*_tmpB51;return(_tmpB51=_region_malloc(r,sizeof(*_tmpB51)),((_tmpB51->break_lab=(struct _dyneither_ptr**)((_tmpB52=_region_malloc(r,sizeof(*_tmpB52)),((_tmpB52[0]=break_l,_tmpB52)))),((_tmpB51->continue_lab=(struct _dyneither_ptr**)_tmp2E1,((_tmpB51->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB51->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E3),((_tmpB51->toplevel=(int)_tmp2E4,((_tmpB51->in_lhs=(int*)_tmp2E5,((_tmpB51->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E6,((_tmpB51->in_sizeof=(int*)_tmp2E7,((_tmpB51->rgn=(struct _RegionHandle*)r,_tmpB51)))))))))))))))))));};}
# 1311
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1314
struct _dyneither_ptr**_tmp2EB;struct _dyneither_ptr**_tmp2EC;struct Cyc_Toc_FallthruInfo*_tmp2ED;struct Cyc_Dict_Dict _tmp2EE;int _tmp2EF;int*_tmp2F0;struct Cyc_Toc_StructInfo _tmp2F1;int*_tmp2F2;struct Cyc_Toc_Env*_tmp2EA=e;_tmp2EB=_tmp2EA->break_lab;_tmp2EC=_tmp2EA->continue_lab;_tmp2ED=_tmp2EA->fallthru_info;_tmp2EE=_tmp2EA->varmap;_tmp2EF=_tmp2EA->toplevel;_tmp2F0=_tmp2EA->in_lhs;_tmp2F1=_tmp2EA->struct_info;_tmp2F2=_tmp2EA->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB55;struct Cyc_Toc_Env*_tmpB54;return(_tmpB54=_region_malloc(r,sizeof(*_tmpB54)),((_tmpB54->break_lab=(struct _dyneither_ptr**)0,((_tmpB54->continue_lab=(struct _dyneither_ptr**)_tmp2EC,((_tmpB54->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->label=next_l,((_tmpB55->binders=0,((_tmpB55->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB55)))))))),((_tmpB54->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2EE),((_tmpB54->toplevel=(int)_tmp2EF,((_tmpB54->in_lhs=(int*)_tmp2F0,((_tmpB54->struct_info=(struct Cyc_Toc_StructInfo)_tmp2F1,((_tmpB54->in_sizeof=(int*)_tmp2F2,((_tmpB54->rgn=(struct _RegionHandle*)r,_tmpB54)))))))))))))))))));};}
# 1329 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1332
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1334
void*_tmp2F5=e->annot;void*_tmp2F6=_tmp2F5;_LL111: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2F7=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2F6;if(_tmp2F7->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL113;}_LL112:
# 1337
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL113: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2F8=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2F6;if(_tmp2F8->tag != Cyc_CfFlowInfo_NotZero)goto _LL115;}_LL114:
# 1340
 return 0;_LL115: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2F9=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2F6;if(_tmp2F9->tag != Cyc_CfFlowInfo_IsZero)goto _LL117;}_LL116:
# 1342
{const char*_tmpB58;void*_tmpB57;(_tmpB57=0,Cyc_Tcutil_terr(e->loc,((_tmpB58="dereference of NULL pointer",_tag_dyneither(_tmpB58,sizeof(char),28))),_tag_dyneither(_tmpB57,sizeof(void*),0)));}
return 0;_LL117: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2FA=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F6;if(_tmp2FA->tag != Cyc_Absyn_EmptyAnnot)goto _LL119;}_LL118:
# 1348
 return 1;_LL119:;_LL11A: {
const char*_tmpB5B;void*_tmpB5A;(_tmpB5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB5B="need_null_check",_tag_dyneither(_tmpB5B,sizeof(char),16))),_tag_dyneither(_tmpB5A,sizeof(void*),0)));}_LL110:;}
# 1353
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2FF=e->annot;void*_tmp300=_tmp2FF;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp304;_LL11C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp301=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp301->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL11E;else{_tmp302=_tmp301->f1;}}_LL11D:
 return _tmp302;_LL11E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp303=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp303->tag != Cyc_CfFlowInfo_NotZero)goto _LL120;else{_tmp304=_tmp303->f1;}}_LL11F:
 return _tmp304;_LL120: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp305=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp305->tag != Cyc_CfFlowInfo_IsZero)goto _LL122;}_LL121:
# 1358
{const char*_tmpB5E;void*_tmpB5D;(_tmpB5D=0,Cyc_Tcutil_terr(e->loc,((_tmpB5E="dereference of NULL pointer",_tag_dyneither(_tmpB5E,sizeof(char),28))),_tag_dyneither(_tmpB5D,sizeof(void*),0)));}
return 0;_LL122: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp306=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp306->tag != Cyc_Absyn_EmptyAnnot)goto _LL124;}_LL123:
 return 0;_LL124:;_LL125: {
const char*_tmpB61;void*_tmpB60;(_tmpB60=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB61="get_relns",_tag_dyneither(_tmpB61,sizeof(char),10))),_tag_dyneither(_tmpB60,sizeof(void*),0)));}_LL11B:;}static char _tmp310[8]="*bogus*";
# 1368
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1378 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1381
struct Cyc_Absyn_Vardecl*x;
{void*_tmp30B=a->r;void*_tmp30C=_tmp30B;void*_tmp30E;_LL127: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp30D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp30C;if(_tmp30D->tag != 1)goto _LL129;else{_tmp30E=(void*)_tmp30D->f2;}}_LL128: {
# 1384
struct Cyc_Absyn_Vardecl*_tmp30F=Cyc_Tcutil_nonesc_vardecl(_tmp30E);
if(_tmp30F == 0)goto _LL12A;
x=_tmp30F;
goto _LL126;}_LL129:;_LL12A: {
# 1390
static struct _dyneither_ptr bogus_string={_tmp310,_tmp310,_tmp310 + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1393
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1399
x=& bogus_vardecl;
x->type=a_typ;}_LL126:;}{
# 1402
void*_tmp311=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1407
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp311);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1416
if(valid_rop_i){
struct Cyc_List_List*_tmp312=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1424
if(!Cyc_Relations_consistent_relations(_tmp312))return 1;}
# 1427
inner_loop: {
void*_tmp313=i->r;void*_tmp314=_tmp313;void*_tmp316;struct Cyc_Absyn_Exp*_tmp317;struct Cyc_Absyn_Exp*_tmp319;struct Cyc_Absyn_Exp*_tmp31A;_LL12C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp315=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp314;if(_tmp315->tag != 13)goto _LL12E;else{_tmp316=(void*)_tmp315->f1;_tmp317=_tmp315->f2;}}_LL12D:
 i=_tmp317;goto inner_loop;_LL12E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp318=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp314;if(_tmp318->tag != 2)goto _LL130;else{if(_tmp318->f1 != Cyc_Absyn_Mod)goto _LL130;if(_tmp318->f2 == 0)goto _LL130;_tmp319=(struct Cyc_Absyn_Exp*)(_tmp318->f2)->hd;if((_tmp318->f2)->tl == 0)goto _LL130;_tmp31A=(struct Cyc_Absyn_Exp*)((_tmp318->f2)->tl)->hd;}}_LL12F: {
# 1433
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp31A,& rop_z)){
# 1436
struct Cyc_List_List*_tmp31B=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1438
return !Cyc_Relations_consistent_relations(_tmp31B);}
# 1440
goto _LL12B;}_LL130:;_LL131:
 goto _LL12B;_LL12B:;}
# 1443
return 0;};}
# 1446
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB64;void*_tmpB63;(_tmpB63=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB64="Missing type in primop ",_tag_dyneither(_tmpB64,sizeof(char),24))),_tag_dyneither(_tmpB63,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1450
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB67;void*_tmpB66;(_tmpB66=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB67="Missing type in primop ",_tag_dyneither(_tmpB67,sizeof(char),24))),_tag_dyneither(_tmpB66,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1454
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB68;return(_tmpB68=_cycalloc(sizeof(*_tmpB68)),((_tmpB68->f1=Cyc_Toc_mt_tq,((_tmpB68->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB68)))));}
# 1457
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB69;return(_tmpB69=_cycalloc(sizeof(*_tmpB69)),((_tmpB69->f1=0,((_tmpB69->f2=e,_tmpB69)))));};}
# 1464
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1467
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp322=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp322=Cyc_Absyn_add_exp(_tmp322,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1475
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp322): Cyc_Toc_malloc_ptr(_tmp322);else{
# 1479
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp322);}}else{
# 1484
t=struct_typ;
eo=0;}
# 1487
if(do_declare){
void*_tmp323=x->r;void*_tmp324=_tmp323;struct _tuple1*_tmp326;_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp325=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp324;if(_tmp325->tag != 1)goto _LL135;else{_tmp326=_tmp325->f1;}}_LL134:
# 1490
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp326,t,eo,s,0),0);_LL135:;_LL136: {
# 1492
const char*_tmpB6C;void*_tmpB6B;(_tmpB6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6C="bogus x to make_struct",_tag_dyneither(_tmpB6C,sizeof(char),23))),_tag_dyneither(_tmpB6B,sizeof(void*),0)));}_LL132:;}else{
# 1496
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1499
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1503
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp329=Cyc_Tcutil_compress(typ);void*_tmp32A=_tmp329;union Cyc_Absyn_AggrInfoU _tmp32C;struct Cyc_List_List*_tmp32D;struct Cyc_List_List*_tmp32F;_LL138: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp32B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp32A;if(_tmp32B->tag != 11)goto _LL13A;else{_tmp32C=(_tmp32B->f1).aggr_info;_tmp32D=(_tmp32B->f1).targs;}}_LL139: {
# 1506
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp32C);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp32F=aggrfields;goto _LL13B;};}_LL13A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp32E=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp32A;if(_tmp32E->tag != 12)goto _LL13C;else{_tmp32F=_tmp32E->f2;}}_LL13B:
# 1511
 if(_tmp32F == 0)return 0;
return 1;_LL13C:;_LL13D:
# 1517
 return 0;_LL137:;}
# 1521
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp330=Cyc_Tcutil_compress(t);void*_tmp331=_tmp330;void*_tmp333;_LL13F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp332=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp331;if(_tmp332->tag != 8)goto _LL141;else{_tmp333=(_tmp332->f1).elt_type;}}_LL140:
 return _tmp333;_LL141:;_LL142: {
const char*_tmpB70;void*_tmpB6F[1];struct Cyc_String_pa_PrintArg_struct _tmpB6E;(_tmpB6E.tag=0,((_tmpB6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB6F[0]=& _tmpB6E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB70="array_elt_type called on %s",_tag_dyneither(_tmpB70,sizeof(char),28))),_tag_dyneither(_tmpB6F,sizeof(void*),1)))))));}_LL13E:;}
# 1528
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1534
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1539
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1544
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp337=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp337 != 0;_tmp337=_tmp337->tl){
# 1548
struct _tuple19*_tmp338=(struct _tuple19*)_tmp337->hd;struct Cyc_List_List*_tmp33A;struct Cyc_Absyn_Exp*_tmp33B;struct _tuple19*_tmp339=_tmp338;_tmp33A=_tmp339->f1;_tmp33B=_tmp339->f2;{
# 1552
struct Cyc_Absyn_Exp*e_index;
if(_tmp33A == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1556
if(_tmp33A->tl != 0){const char*_tmpB73;void*_tmpB72;(_tmpB72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB73="multiple designators in array",_tag_dyneither(_tmpB73,sizeof(char),30))),_tag_dyneither(_tmpB72,sizeof(void*),0)));}{
void*_tmp33E=(void*)_tmp33A->hd;
void*_tmp33F=_tmp33E;struct Cyc_Absyn_Exp*_tmp341;_LL144: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp340=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33F;if(_tmp340->tag != 0)goto _LL146;else{_tmp341=_tmp340->f1;}}_LL145:
# 1560
 Cyc_Toc_exp_to_c(nv,_tmp341);
e_index=_tmp341;
goto _LL143;_LL146: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp342=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33F;if(_tmp342->tag != 1)goto _LL143;}_LL147: {
const char*_tmpB76;void*_tmpB75;(_tmpB75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB76="field name designators in array",_tag_dyneither(_tmpB76,sizeof(char),32))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}_LL143:;};}{
# 1566
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp345=_tmp33B->r;void*_tmp346=_tmp345;struct Cyc_List_List*_tmp348;struct Cyc_Absyn_Vardecl*_tmp34A;struct Cyc_Absyn_Exp*_tmp34B;struct Cyc_Absyn_Exp*_tmp34C;int _tmp34D;void*_tmp34F;struct Cyc_List_List*_tmp350;_LL149: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp347=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp347->tag != 25)goto _LL14B;else{_tmp348=_tmp347->f1;}}_LL14A:
# 1569
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp348,s);
goto _LL148;_LL14B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp349=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp349->tag != 26)goto _LL14D;else{_tmp34A=_tmp349->f1;_tmp34B=_tmp349->f2;_tmp34C=_tmp349->f3;_tmp34D=_tmp349->f4;}}_LL14C:
# 1572
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp34A,_tmp34B,_tmp34C,_tmp34D,s,0);
# 1574
goto _LL148;_LL14D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp34E=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp34E->tag != 29)goto _LL14F;else{_tmp34F=(void*)_tmp34E->f1;_tmp350=_tmp34E->f2;}}_LL14E:
# 1576
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp34F,_tmp350,s);
goto _LL148;_LL14F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp351=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp346;if(_tmp351->tag != 27)goto _LL151;}_LL150:
# 1579
 goto _LL148;_LL151:;_LL152:
# 1581
 Cyc_Toc_exp_to_c(nv,_tmp33B);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp33B),0),s,0);
goto _LL148;_LL148:;};};}}
# 1587
return s;}
# 1592
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1597
struct _tuple1*_tmp352=vd->name;
void*expected_elt_type;
{void*_tmp353=Cyc_Tcutil_compress(comprehension_type);void*_tmp354=_tmp353;void*_tmp356;void*_tmp358;_LL154: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp355=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp354;if(_tmp355->tag != 8)goto _LL156;else{_tmp356=(_tmp355->f1).elt_type;}}_LL155:
# 1601
 expected_elt_type=_tmp356;goto _LL153;_LL156: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp357=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp354;if(_tmp357->tag != 5)goto _LL158;else{_tmp358=(_tmp357->f1).elt_typ;}}_LL157:
# 1603
 expected_elt_type=_tmp358;goto _LL153;_LL158:;_LL159: {
const char*_tmpB79;void*_tmpB78;(_tmpB78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB79="init_comprehension passed a bad type!",_tag_dyneither(_tmpB79,sizeof(char),38))),_tag_dyneither(_tmpB78,sizeof(void*),0)));}_LL153:;}{
# 1606
void*_tmp35B=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1610
{void*_tmp35C=e2->r;void*_tmp35D=_tmp35C;_LL15B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp35E=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp35D;if(_tmp35E->tag != 27)goto _LL15D;}_LL15C:
 return Cyc_Absyn_skip_stmt(0);_LL15D:;_LL15E:
 goto _LL15A;_LL15A:;}{
# 1614
struct _RegionHandle _tmp35F=_new_region("r2");struct _RegionHandle*r2=& _tmp35F;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB7C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB7B;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp352,Cyc_Absyn_varb_exp(_tmp352,(void*)((_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B[0]=((_tmpB7C.tag=4,((_tmpB7C.f1=vd,_tmpB7C)))),_tmpB7B)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp352,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp352,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp352,0),0);
# 1622
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp352,0),0);
struct Cyc_Absyn_Stmt*body;
# 1626
{void*_tmp360=e2->r;void*_tmp361=_tmp360;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Vardecl*_tmp365;struct Cyc_Absyn_Exp*_tmp366;struct Cyc_Absyn_Exp*_tmp367;int _tmp368;void*_tmp36A;struct Cyc_List_List*_tmp36B;_LL160: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp362=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp361;if(_tmp362->tag != 25)goto _LL162;else{_tmp363=_tmp362->f1;}}_LL161:
# 1628
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp363,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL162: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp364=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp361;if(_tmp364->tag != 26)goto _LL164;else{_tmp365=_tmp364->f1;_tmp366=_tmp364->f2;_tmp367=_tmp364->f3;_tmp368=_tmp364->f4;}}_LL163:
# 1631
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp365,_tmp366,_tmp367,_tmp368,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15F;_LL164: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp369=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp361;if(_tmp369->tag != 29)goto _LL166;else{_tmp36A=(void*)_tmp369->f1;_tmp36B=_tmp369->f2;}}_LL165:
# 1634
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp36A,_tmp36B,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL166:;_LL167:
# 1637
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15F;_LL15F:;}{
# 1641
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1643
if(zero_term){
# 1648
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp35B,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1653
struct Cyc_Absyn_Stmt*_tmp36C=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp352,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp36C;};};}
# 1615
;_pop_region(r2);};};}
# 1659
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1662
{struct Cyc_List_List*_tmp36F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp36F != 0;_tmp36F=_tmp36F->tl){
struct _tuple19*_tmp370=(struct _tuple19*)_tmp36F->hd;struct Cyc_List_List*_tmp372;struct Cyc_Absyn_Exp*_tmp373;struct _tuple19*_tmp371=_tmp370;_tmp372=_tmp371->f1;_tmp373=_tmp371->f2;
if(_tmp372 == 0){
const char*_tmpB7F;void*_tmpB7E;(_tmpB7E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7F="empty designator list",_tag_dyneither(_tmpB7F,sizeof(char),22))),_tag_dyneither(_tmpB7E,sizeof(void*),0)));}
if(_tmp372->tl != 0){
const char*_tmpB82;void*_tmpB81;(_tmpB81=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB82="too many designators in anonymous struct",_tag_dyneither(_tmpB82,sizeof(char),41))),_tag_dyneither(_tmpB81,sizeof(void*),0)));}{
void*_tmp378=(void*)_tmp372->hd;void*_tmp379=_tmp378;struct _dyneither_ptr*_tmp37B;_LL169: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp37A=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp379;if(_tmp37A->tag != 1)goto _LL16B;else{_tmp37B=_tmp37A->f1;}}_LL16A: {
# 1670
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp37B,0);
{void*_tmp37C=_tmp373->r;void*_tmp37D=_tmp37C;struct Cyc_List_List*_tmp37F;struct Cyc_Absyn_Vardecl*_tmp381;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Exp*_tmp383;int _tmp384;void*_tmp386;struct Cyc_List_List*_tmp387;_LL16E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp37E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp37D;if(_tmp37E->tag != 25)goto _LL170;else{_tmp37F=_tmp37E->f1;}}_LL16F:
# 1673
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp373->topt),lval,_tmp37F,s);goto _LL16D;_LL170: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp380=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp37D;if(_tmp380->tag != 26)goto _LL172;else{_tmp381=_tmp380->f1;_tmp382=_tmp380->f2;_tmp383=_tmp380->f3;_tmp384=_tmp380->f4;}}_LL171:
# 1675
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp373->topt),lval,_tmp381,_tmp382,_tmp383,_tmp384,s,0);
goto _LL16D;_LL172: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp385=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp37D;if(_tmp385->tag != 29)goto _LL174;else{_tmp386=(void*)_tmp385->f1;_tmp387=_tmp385->f2;}}_LL173:
# 1678
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp386,_tmp387,s);goto _LL16D;_LL174: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp388=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp37D;if(_tmp388->tag != 27)goto _LL176;}_LL175:
# 1680
 goto _LL16D;_LL176:;_LL177:
# 1682
 Cyc_Toc_exp_to_c(nv,_tmp373);
# 1684
if(Cyc_Toc_is_poly_field(struct_type,_tmp37B))
_tmp373=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp373);
# 1687
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp373,0),0),s,0);
goto _LL16D;_LL16D:;}
# 1690
goto _LL168;}_LL16B:;_LL16C: {
const char*_tmpB85;void*_tmpB84;(_tmpB84=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB85="array designator in struct",_tag_dyneither(_tmpB85,sizeof(char),27))),_tag_dyneither(_tmpB84,sizeof(void*),0)));}_LL168:;};}}
# 1694
return s;}
# 1699
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1703
struct _RegionHandle _tmp38B=_new_region("r");struct _RegionHandle*r=& _tmp38B;_push_region(r);
{struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp38D=Cyc_Toc_add_tuple_type(_tmp38C);
# 1707
struct _tuple1*_tmp38E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp38F=Cyc_Absyn_var_exp(_tmp38E,0);
struct Cyc_Absyn_Stmt*_tmp390=Cyc_Absyn_exp_stmt(_tmp38F,0);
# 1711
struct Cyc_Absyn_Exp*(*_tmp391)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1713
int is_atomic=1;
struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp392);for(0;_tmp392 != 0;(_tmp392=_tmp392->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp392->hd;
struct Cyc_Absyn_Exp*lval=_tmp391(_tmp38F,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp393=e->r;void*_tmp394=_tmp393;struct Cyc_List_List*_tmp396;struct Cyc_Absyn_Vardecl*_tmp398;struct Cyc_Absyn_Exp*_tmp399;struct Cyc_Absyn_Exp*_tmp39A;int _tmp39B;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp395=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp394;if(_tmp395->tag != 25)goto _LL17B;else{_tmp396=_tmp395->f1;}}_LL17A:
# 1721
 _tmp390=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp396,_tmp390);
goto _LL178;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp397=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp394;if(_tmp397->tag != 26)goto _LL17D;else{_tmp398=_tmp397->f1;_tmp399=_tmp397->f2;_tmp39A=_tmp397->f3;_tmp39B=_tmp397->f4;}}_LL17C:
# 1724
 _tmp390=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp398,_tmp399,_tmp39A,_tmp39B,_tmp390,0);
# 1726
goto _LL178;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp39C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp394;if(_tmp39C->tag != 27)goto _LL17F;}_LL17E:
# 1728
 goto _LL178;_LL17F:;_LL180:
# 1731
 Cyc_Toc_exp_to_c(nv,e);
_tmp390=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp391(_tmp38F,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp390,0);
# 1734
goto _LL178;_LL178:;};}}{
# 1737
struct Cyc_Absyn_Exp*_tmp39D=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp38E,0),_tmp38D,_tmp390,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp39D;};}
# 1704
;_pop_region(r);}
# 1741
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp39E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp39E != 0;_tmp39E=_tmp39E->tl){
struct Cyc_Absyn_Aggrfield*_tmp39F=(struct Cyc_Absyn_Aggrfield*)_tmp39E->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp39F->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1748
struct Cyc_String_pa_PrintArg_struct _tmpB8D;void*_tmpB8C[1];const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB8D.tag=0,((_tmpB8D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB8C[0]=& _tmpB8D,Cyc_aprintf(((_tmpB8B="get_member_offset %s failed",_tag_dyneither(_tmpB8B,sizeof(char),28))),_tag_dyneither(_tmpB8C,sizeof(void*),1)))))))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));};}
# 1751
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3A4=Cyc_Tcutil_compress(*typ);void*_tmp3A5=_tmp3A4;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3AA;unsigned int _tmp3AB;_LL182: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5;if(_tmp3A6->tag != 8)goto _LL184;else{_tmp3A7=(_tmp3A6->f1).elt_type;_tmp3A8=(_tmp3A6->f1).tq;_tmp3A9=(_tmp3A6->f1).num_elts;_tmp3AA=(_tmp3A6->f1).zero_term;_tmp3AB=(_tmp3A6->f1).zt_loc;}}_LL183:
# 1756
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3A9))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB93;struct Cyc_Absyn_ArrayInfo _tmpB92;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB91;*typ=(void*)((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91[0]=((_tmpB93.tag=8,((_tmpB93.f1=((_tmpB92.elt_type=_tmp3A7,((_tmpB92.tq=_tmp3A8,((_tmpB92.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB92.zero_term=_tmp3AA,((_tmpB92.zt_loc=_tmp3AB,_tmpB92)))))))))),_tmpB93)))),_tmpB91))));}
return 1;}
# 1760
goto _LL181;_LL184:;_LL185:
 goto _LL181;_LL181:;}
# 1763
return 0;}
# 1766
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1768
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1772
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3AF=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpB94;struct Cyc_Absyn_Aggrfield*_tmp3B0=(_tmpB94=_cycalloc(sizeof(*_tmpB94)),((_tmpB94->name=_tmp3AF->name,((_tmpB94->tq=Cyc_Toc_mt_tq,((_tmpB94->type=new_field_type,((_tmpB94->width=_tmp3AF->width,((_tmpB94->attributes=_tmp3AF->attributes,((_tmpB94->requires_clause=0,_tmpB94)))))))))))));
# 1780
struct Cyc_List_List*_tmpB95;new_fields=((_tmpB95=_cycalloc(sizeof(*_tmpB95)),((_tmpB95->hd=_tmp3B0,((_tmpB95->tl=new_fields,_tmpB95))))));}else{
# 1782
struct Cyc_List_List*_tmpB96;new_fields=((_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB96->tl=new_fields,_tmpB96))))));}}{
# 1784
struct Cyc_List_List*_tmp3B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB9E;void*_tmpB9D[1];const char*_tmpB9C;struct _dyneither_ptr*_tmpB9B;struct _dyneither_ptr*name=
(_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=(struct _dyneither_ptr)((_tmpB9E.tag=1,((_tmpB9E.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB9D[0]=& _tmpB9E,Cyc_aprintf(((_tmpB9C="_genStruct%d",_tag_dyneither(_tmpB9C,sizeof(char),13))),_tag_dyneither(_tmpB9D,sizeof(void*),1)))))))),_tmpB9B)));
struct _tuple1*_tmpB9F;struct _tuple1*qv=(_tmpB9F=_cycalloc(sizeof(*_tmpB9F)),((_tmpB9F->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB9F->f2=name,_tmpB9F)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBA0;struct Cyc_Absyn_AggrdeclImpl*_tmp3B5=(_tmpBA0=_cycalloc(sizeof(*_tmpBA0)),((_tmpBA0->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpBA0->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpBA0->fields=_tmp3B4,((_tmpBA0->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpBA0)))))))));
# 1792
struct Cyc_Absyn_Aggrdecl*_tmpBA1;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->kind=ad->kind,((_tmpBA1->sc=ad->sc,((_tmpBA1->name=qv,((_tmpBA1->tvs=ad->tvs,((_tmpBA1->impl=_tmp3B5,((_tmpBA1->attributes=ad->attributes,_tmpBA1)))))))))))));
# 1798
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1802
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1808
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3BD=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3BD,0);
do_declare=1;}else{
# 1816
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1819
struct Cyc_Absyn_Stmt*_tmp3BE=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1823
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3BF=_tmp3BE->r;void*_tmp3C0=_tmp3BF;struct Cyc_Absyn_Exp**_tmp3C2;_LL187: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3C1=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C0;if(_tmp3C1->tag != 1)goto _LL189;else{_tmp3C2=(struct Cyc_Absyn_Exp**)& _tmp3C1->f1;}}_LL188:
 varexp=_tmp3C2;goto _LL186;_LL189:;_LL18A: {
const char*_tmpBA4;void*_tmpBA3;(_tmpBA3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA4="stmt not an expression!",_tag_dyneither(_tmpBA4,sizeof(char),24))),_tag_dyneither(_tmpBA3,sizeof(void*),0)));}_LL186:;}{
# 1829
struct Cyc_Absyn_Exp*(*_tmp3C5)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1833
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3C6=Cyc_Tcutil_compress(struct_type);void*_tmp3C7=_tmp3C6;union Cyc_Absyn_AggrInfoU _tmp3C9;struct Cyc_List_List*_tmp3CA;_LL18C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3C8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C7;if(_tmp3C8->tag != 11)goto _LL18E;else{_tmp3C9=(_tmp3C8->f1).aggr_info;_tmp3CA=(_tmp3C8->f1).targs;}}_LL18D:
# 1840
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3C9);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3CA;
goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpBA7;void*_tmpBA6;(_tmpBA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA7="init_struct: bad struct type",_tag_dyneither(_tmpBA7,sizeof(char),29))),_tag_dyneither(_tmpBA6,sizeof(void*),0)));}_LL18B:;}
# 1847
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3CD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3CD->tl != 0;_tmp3CD=_tmp3CD->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3CE=(struct Cyc_Absyn_Aggrfield*)_tmp3CD->hd;
struct _RegionHandle _tmp3CF=_new_region("temp");struct _RegionHandle*temp=& _tmp3CF;_push_region(temp);
{struct Cyc_List_List*_tmp3D0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1856
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3D0,_tmp3CE->type))){
struct Cyc_List_List*_tmp3D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3D2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3D0,_tmp3D1);
# 1860
struct Cyc_List_List*new_fields=0;
for(_tmp3CD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3CD != 0;_tmp3CD=_tmp3CD->tl){
struct Cyc_Absyn_Aggrfield*_tmp3D3=(struct Cyc_Absyn_Aggrfield*)_tmp3CD->hd;
struct Cyc_Absyn_Aggrfield*_tmpBA8;struct Cyc_Absyn_Aggrfield*_tmp3D4=(_tmpBA8=_cycalloc(sizeof(*_tmpBA8)),((_tmpBA8->name=_tmp3D3->name,((_tmpBA8->tq=Cyc_Toc_mt_tq,((_tmpBA8->type=
# 1865
Cyc_Tcutil_rsubstitute(temp,_tmp3D2,_tmp3D3->type),((_tmpBA8->width=_tmp3D3->width,((_tmpBA8->attributes=_tmp3D3->attributes,((_tmpBA8->requires_clause=0,_tmpBA8)))))))))))));
# 1873
if(_tmp3CD->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3D4->type)){
struct _dyneither_ptr**_tmpBA9;index=((_tmpBA9=_cycalloc(sizeof(*_tmpBA9)),((_tmpBA9[0]=_tmp3D4->name,_tmpBA9))));}}{
# 1880
struct Cyc_List_List*_tmpBAA;new_fields=((_tmpBAA=_cycalloc(sizeof(*_tmpBAA)),((_tmpBAA->hd=_tmp3D4,((_tmpBAA->tl=new_fields,_tmpBAA))))));};}
# 1882
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpBB2;void*_tmpBB1[1];const char*_tmpBB0;struct _dyneither_ptr*_tmpBAF;struct _dyneither_ptr*name=
(_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF[0]=(struct _dyneither_ptr)((_tmpBB2.tag=1,((_tmpBB2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBB1[0]=& _tmpBB2,Cyc_aprintf(((_tmpBB0="_genStruct%d",_tag_dyneither(_tmpBB0,sizeof(char),13))),_tag_dyneither(_tmpBB1,sizeof(void*),1)))))))),_tmpBAF)));
struct _tuple1*_tmpBB3;struct _tuple1*qv=(_tmpBB3=_cycalloc(sizeof(*_tmpBB3)),((_tmpBB3->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBB3->f2=name,_tmpBB3)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBB4;struct Cyc_Absyn_AggrdeclImpl*_tmp3D8=(_tmpBB4=_cycalloc(sizeof(*_tmpBB4)),((_tmpBB4->exist_vars=0,((_tmpBB4->rgn_po=0,((_tmpBB4->fields=aggrfields,((_tmpBB4->tagged=0,_tmpBB4)))))))));
# 1891
struct Cyc_Absyn_Aggrdecl*_tmpBB5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBB5=_cycalloc(sizeof(*_tmpBB5)),((_tmpBB5->kind=Cyc_Absyn_StructA,((_tmpBB5->sc=Cyc_Absyn_Public,((_tmpBB5->name=qv,((_tmpBB5->tvs=0,((_tmpBB5->impl=_tmp3D8,((_tmpBB5->attributes=0,_tmpBB5)))))))))))));
# 1897
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBBF;struct Cyc_Absyn_Aggrdecl**_tmpBBE;struct Cyc_Absyn_AggrInfo _tmpBBD;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBBC;struct_type=(void*)((_tmpBBC=_cycalloc(sizeof(*_tmpBBC)),((_tmpBBC[0]=((_tmpBBF.tag=11,((_tmpBBF.f1=((_tmpBBD.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBBE=_cycalloc(sizeof(*_tmpBBE)),((_tmpBBE[0]=new_ad,_tmpBBE))))),((_tmpBBD.targs=0,_tmpBBD)))),_tmpBBF)))),_tmpBBC))));}
# 1901
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1852
;_pop_region(temp);};}{
# 1905
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3E4=_new_region("r");struct _RegionHandle*r=& _tmp3E4;_push_region(r);
{struct Cyc_List_List*_tmp3E5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3E5 != 0;_tmp3E5=_tmp3E5->tl){
struct _tuple19*_tmp3E6=(struct _tuple19*)_tmp3E5->hd;struct Cyc_List_List*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E9;struct _tuple19*_tmp3E7=_tmp3E6;_tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3E9->topt));
if(_tmp3E8 == 0){
const char*_tmpBC2;void*_tmpBC1;(_tmpBC1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC2="empty designator list",_tag_dyneither(_tmpBC2,sizeof(char),22))),_tag_dyneither(_tmpBC1,sizeof(void*),0)));}
if(_tmp3E8->tl != 0){
# 1915
struct _tuple1*_tmp3EC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3ED=Cyc_Absyn_var_exp(_tmp3EC,0);
for(0;_tmp3E8 != 0;_tmp3E8=_tmp3E8->tl){
void*_tmp3EE=(void*)_tmp3E8->hd;void*_tmp3EF=_tmp3EE;struct _dyneither_ptr*_tmp3F1;_LL191: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3EF;if(_tmp3F0->tag != 1)goto _LL193;else{_tmp3F1=_tmp3F0->f1;}}_LL192:
# 1921
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3F1))
_tmp3ED=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3ED);
_tmp3BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3C5(xexp,_tmp3F1,0),_tmp3ED,0),0),_tmp3BE,0);
# 1925
goto _LL190;_LL193:;_LL194: {
const char*_tmpBC5;void*_tmpBC4;(_tmpBC4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC5="array designator in struct",_tag_dyneither(_tmpBC5,sizeof(char),27))),_tag_dyneither(_tmpBC4,sizeof(void*),0)));}_LL190:;}
# 1929
Cyc_Toc_exp_to_c(nv,_tmp3E9);
_tmp3BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3ED,_tmp3E9,0),0),_tmp3BE,0);}else{
# 1932
void*_tmp3F4=(void*)_tmp3E8->hd;void*_tmp3F5=_tmp3F4;struct _dyneither_ptr*_tmp3F7;_LL196: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3F5;if(_tmp3F6->tag != 1)goto _LL198;else{_tmp3F7=_tmp3F6->f1;}}_LL197: {
# 1934
struct Cyc_Absyn_Exp*lval=_tmp3C5(xexp,_tmp3F7,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3F7);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3BE,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1943
int e1_translated=0;
{void*_tmp3F8=_tmp3E9->r;void*_tmp3F9=_tmp3F8;struct Cyc_List_List*_tmp3FB;struct Cyc_Absyn_Vardecl*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp3FF;int _tmp400;struct Cyc_Absyn_Exp*_tmp402;void*_tmp403;void*_tmp405;struct Cyc_List_List*_tmp406;_LL19B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3FA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F9;if(_tmp3FA->tag != 25)goto _LL19D;else{_tmp3FB=_tmp3FA->f1;}}_LL19C:
# 1946
 _tmp3BE=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3F7)))->type,lval,_tmp3FB,_tmp3BE);
# 1948
goto _LL19A;_LL19D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3FC=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F9;if(_tmp3FC->tag != 26)goto _LL19F;else{_tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FC->f2;_tmp3FF=_tmp3FC->f3;_tmp400=_tmp3FC->f4;}}_LL19E:
# 1950
 _tmp3BE=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3F7)))->type,lval,_tmp3FD,_tmp3FE,_tmp3FF,_tmp400,_tmp3BE,0);
# 1954
e1_translated=1;
_tmp402=_tmp3FE;_tmp403=(void*)_check_null(_tmp3FF->topt);goto _LL1A0;_LL19F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp401=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3F9;if(_tmp401->tag != 27)goto _LL1A1;else{_tmp402=_tmp401->f1;_tmp403=(void*)_tmp401->f2;}}_LL1A0:
# 1959
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3F7)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp402);{
void*_tmp407=Cyc_Toc_typ_to_c(_tmp403);
struct _tuple6 _tmpBC9;union Cyc_Absyn_Cnst _tmpBC8;struct Cyc_Absyn_Exp*_tmpBC6[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBC6[1]=Cyc_Absyn_sizeoftyp_exp(_tmp407,0),((_tmpBC6[0]=_tmp402,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBC6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBC8.Int_c).val=((_tmpBC9.f1=Cyc_Absyn_Unsigned,((_tmpBC9.f2=(int)sizeof(double),_tmpBC9)))),(((_tmpBC8.Int_c).tag=5,_tmpBC8)))),0),0);};}
# 1967
goto _LL19A;_LL1A1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp404=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3F9;if(_tmp404->tag != 29)goto _LL1A3;else{_tmp405=(void*)_tmp404->f1;_tmp406=_tmp404->f2;}}_LL1A2:
# 1969
 _tmp3BE=Cyc_Toc_init_anon_struct(nv,lval,_tmp405,_tmp406,_tmp3BE);goto _LL19A;_LL1A3:;_LL1A4: {
# 1971
void*old_e_typ=(void*)_check_null(_tmp3E9->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3E9);
{void*_tmp40B=old_e_typ;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp40C=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40B;if(_tmp40C->tag != 11)goto _LL1A8;}_LL1A7:
# 1978
 if(old_e_typ != _tmp3E9->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3E9->topt))){
# 1982
ad=Cyc_Toc_update_aggr_type(ad,_tmp3F7,(void*)_check_null(_tmp3E9->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBD3;struct Cyc_Absyn_Aggrdecl**_tmpBD2;struct Cyc_Absyn_AggrInfo _tmpBD1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBD0;struct_type=(void*)(
(_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0[0]=((_tmpBD3.tag=11,((_tmpBD3.f1=((_tmpBD1.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBD2=_cycalloc(sizeof(*_tmpBD2)),((_tmpBD2[0]=ad,_tmpBD2))))),((_tmpBD1.targs=0,_tmpBD1)))),_tmpBD3)))),_tmpBD0))));}
# 1986
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1988
goto _LL1A5;_LL1A8:;_LL1A9:
 goto _LL1A5;_LL1A5:;}{
# 1991
struct Cyc_Absyn_Aggrfield*_tmp411=Cyc_Absyn_lookup_field(aggrfields,_tmp3F7);
# 1993
if(Cyc_Toc_is_poly_field(struct_type,_tmp3F7) && !was_ptr_type)
_tmp3E9=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3E9);
# 1996
if(exist_types != 0)
_tmp3E9=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp411))->type),_tmp3E9);
# 1999
_tmp3BE=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3E9,0),0),_tmp3BE,0);
goto _LL19A;};}_LL19A:;}
# 2002
goto _LL195;};}_LL198:;_LL199: {
const char*_tmpBD6;void*_tmpBD5;(_tmpBD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD6="array designator in struct",_tag_dyneither(_tmpBD6,sizeof(char),27))),_tag_dyneither(_tmpBD5,sizeof(void*),0)));}_LL195:;}}}
# 2009
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2014
struct Cyc_Absyn_Exp*_tmp414=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3BE,pointer,rgnopt,is_atomic,do_declare);
# 2016
_tmp414->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp415=_tmp414;_npop_handler(0);return _tmp415;};};
# 1907
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2022
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2025
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2028
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBD9;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBD8;return Cyc_Absyn_new_exp((void*)((_tmpBD8=_cycalloc(sizeof(*_tmpBD8)),((_tmpBD8[0]=((_tmpBD9.tag=4,((_tmpBD9.f1=e1,((_tmpBD9.f2=incr,_tmpBD9)))))),_tmpBD8)))),0);}
# 2032
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2041
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2043
void*_tmp418=e1->r;void*_tmp419=_tmp418;struct Cyc_Absyn_Stmt*_tmp41B;void*_tmp41D;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp420;struct _dyneither_ptr*_tmp421;int _tmp422;int _tmp423;_LL1AB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp41A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41A->tag != 36)goto _LL1AD;else{_tmp41B=_tmp41A->f1;}}_LL1AC:
 Cyc_Toc_lvalue_assign_stmt(_tmp41B,fs,f,f_env);goto _LL1AA;_LL1AD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp41C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41C->tag != 13)goto _LL1AF;else{_tmp41D=(void*)_tmp41C->f1;_tmp41E=_tmp41C->f2;}}_LL1AE:
 Cyc_Toc_lvalue_assign(_tmp41E,fs,f,f_env);goto _LL1AA;_LL1AF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp41F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp419;if(_tmp41F->tag != 20)goto _LL1B1;else{_tmp420=_tmp41F->f1;_tmp421=_tmp41F->f2;_tmp422=_tmp41F->f3;_tmp423=_tmp41F->f4;}}_LL1B0:
# 2048
 e1->r=_tmp420->r;
{struct Cyc_List_List*_tmpBDA;Cyc_Toc_lvalue_assign(e1,((_tmpBDA=_cycalloc(sizeof(*_tmpBDA)),((_tmpBDA->hd=_tmp421,((_tmpBDA->tl=fs,_tmpBDA)))))),f,f_env);}
goto _LL1AA;_LL1B1:;_LL1B2: {
# 2056
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2058
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1AA;}_LL1AA:;}
# 2064
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2066
void*_tmp425=s->r;void*_tmp426=_tmp425;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Decl*_tmp42A;struct Cyc_Absyn_Stmt*_tmp42B;struct Cyc_Absyn_Stmt*_tmp42D;_LL1B4: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp427=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp427->tag != 1)goto _LL1B6;else{_tmp428=_tmp427->f1;}}_LL1B5:
 Cyc_Toc_lvalue_assign(_tmp428,fs,f,f_env);goto _LL1B3;_LL1B6: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp429=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp429->tag != 12)goto _LL1B8;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}_LL1B7:
# 2069
 Cyc_Toc_lvalue_assign_stmt(_tmp42B,fs,f,f_env);goto _LL1B3;_LL1B8: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp42C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp426;if(_tmp42C->tag != 2)goto _LL1BA;else{_tmp42D=_tmp42C->f2;}}_LL1B9:
 Cyc_Toc_lvalue_assign_stmt(_tmp42D,fs,f,f_env);goto _LL1B3;_LL1BA:;_LL1BB: {
const char*_tmpBDE;void*_tmpBDD[1];struct Cyc_String_pa_PrintArg_struct _tmpBDC;(_tmpBDC.tag=0,((_tmpBDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBDD[0]=& _tmpBDC,Cyc_Toc_toc_impos(((_tmpBDE="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBDE,sizeof(char),23))),_tag_dyneither(_tmpBDD,sizeof(void*),1)))))));}_LL1B3:;}
# 2075
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2079
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp431=e->r;void*_tmp432=_tmp431;void**_tmp434;struct Cyc_Absyn_Exp**_tmp435;struct Cyc_Absyn_Exp*_tmp437;struct Cyc_Absyn_Stmt*_tmp439;_LL1BD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp433=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp432;if(_tmp433->tag != 13)goto _LL1BF;else{_tmp434=(void**)((void**)& _tmp433->f1);_tmp435=(struct Cyc_Absyn_Exp**)& _tmp433->f2;}}_LL1BE:
# 2082
*_tmp435=Cyc_Toc_push_address_exp(*_tmp435);
*_tmp434=Cyc_Absyn_cstar_typ(*_tmp434,Cyc_Toc_mt_tq);
return e;_LL1BF: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp436=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp432;if(_tmp436->tag != 19)goto _LL1C1;else{_tmp437=_tmp436->f1;}}_LL1C0:
# 2086
 return _tmp437;_LL1C1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp438=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp432;if(_tmp438->tag != 36)goto _LL1C3;else{_tmp439=_tmp438->f1;}}_LL1C2:
# 2090
 Cyc_Toc_push_address_stmt(_tmp439);
return e;_LL1C3:;_LL1C4:
# 2093
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBE2;void*_tmpBE1[1];struct Cyc_String_pa_PrintArg_struct _tmpBE0;(_tmpBE0.tag=0,((_tmpBE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBE1[0]=& _tmpBE0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE2="can't take & of exp %s",_tag_dyneither(_tmpBE2,sizeof(char),23))),_tag_dyneither(_tmpBE1,sizeof(void*),1)))))));};_LL1BC:;}
# 2098
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp43D=s->r;void*_tmp43E=_tmp43D;struct Cyc_Absyn_Stmt*_tmp440;struct Cyc_Absyn_Stmt*_tmp442;struct Cyc_Absyn_Exp**_tmp444;_LL1C6: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp43F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43E;if(_tmp43F->tag != 2)goto _LL1C8;else{_tmp440=_tmp43F->f2;}}_LL1C7:
 _tmp442=_tmp440;goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp441=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43E;if(_tmp441->tag != 12)goto _LL1CA;else{_tmp442=_tmp441->f2;}}_LL1C9:
 Cyc_Toc_push_address_stmt(_tmp442);goto _LL1C5;_LL1CA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp443=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43E;if(_tmp443->tag != 1)goto _LL1CC;else{_tmp444=(struct Cyc_Absyn_Exp**)& _tmp443->f1;}}_LL1CB:
*_tmp444=Cyc_Toc_push_address_exp(*_tmp444);goto _LL1C5;_LL1CC:;_LL1CD: {
# 2104
const char*_tmpBE6;void*_tmpBE5[1];struct Cyc_String_pa_PrintArg_struct _tmpBE4;(_tmpBE4.tag=0,((_tmpBE4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBE5[0]=& _tmpBE4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="can't take & of stmt %s",_tag_dyneither(_tmpBE6,sizeof(char),24))),_tag_dyneither(_tmpBE5,sizeof(void*),1)))))));}_LL1C5:;}
# 2109
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2111
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2113
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBE7;result=((_tmpBE7=_region_malloc(r2,sizeof(*_tmpBE7)),((_tmpBE7->hd=f(x->hd,env),((_tmpBE7->tl=0,_tmpBE7))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBE8;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBE8=_region_malloc(r2,sizeof(*_tmpBE8)),((_tmpBE8->hd=f(x->hd,env),((_tmpBE8->tl=0,_tmpBE8))))));}
prev=prev->tl;}
# 2120
return result;}
# 2122
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2126
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpBE9;return(_tmpBE9=_cycalloc(sizeof(*_tmpBE9)),((_tmpBE9->f1=0,((_tmpBE9->f2=e,_tmpBE9)))));}
# 2130
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp44B=Cyc_Tcutil_compress(t);void*_tmp44C=_tmp44B;struct Cyc_Absyn_PtrInfo _tmp44E;_LL1CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C;if(_tmp44D->tag != 5)goto _LL1D1;else{_tmp44E=_tmp44D->f1;}}_LL1D0:
 return _tmp44E;_LL1D1:;_LL1D2: {
const char*_tmpBEC;void*_tmpBEB;(_tmpBEB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBEC="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBEC,sizeof(char),28))),_tag_dyneither(_tmpBEB,sizeof(void*),0)));}_LL1CE:;}
# 2140
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp451=Cyc_Tcutil_compress(t);void*_tmp452=_tmp451;enum Cyc_Absyn_Sign _tmp455;enum Cyc_Absyn_Sign _tmp457;enum Cyc_Absyn_Sign _tmp45B;enum Cyc_Absyn_Sign _tmp45D;enum Cyc_Absyn_Sign _tmp45F;int _tmp463;_LL1D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp452;if(_tmp453->tag != 5)goto _LL1D6;}_LL1D5:
 res=Cyc_Absyn_null_exp(0);goto _LL1D3;_LL1D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452;if(_tmp454->tag != 6)goto _LL1D8;else{_tmp455=_tmp454->f1;if(_tmp454->f2 != Cyc_Absyn_Char_sz)goto _LL1D8;}}_LL1D7:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp455,'\000'),0);goto _LL1D3;_LL1D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452;if(_tmp456->tag != 6)goto _LL1DA;else{_tmp457=_tmp456->f1;if(_tmp456->f2 != Cyc_Absyn_Short_sz)goto _LL1DA;}}_LL1D9:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp457,0),0);goto _LL1D3;_LL1DA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp452;if(_tmp458->tag != 13)goto _LL1DC;}_LL1DB:
 goto _LL1DD;_LL1DC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp459=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp452;if(_tmp459->tag != 14)goto _LL1DE;}_LL1DD:
 _tmp45B=Cyc_Absyn_Unsigned;goto _LL1DF;_LL1DE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452;if(_tmp45A->tag != 6)goto _LL1E0;else{_tmp45B=_tmp45A->f1;if(_tmp45A->f2 != Cyc_Absyn_Int_sz)goto _LL1E0;}}_LL1DF:
 _tmp45D=_tmp45B;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452;if(_tmp45C->tag != 6)goto _LL1E2;else{_tmp45D=_tmp45C->f1;if(_tmp45C->f2 != Cyc_Absyn_Long_sz)goto _LL1E2;}}_LL1E1:
# 2150
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp45D,0),0);goto _LL1D3;_LL1E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp452;if(_tmp45E->tag != 6)goto _LL1E4;else{_tmp45F=_tmp45E->f1;if(_tmp45E->f2 != Cyc_Absyn_LongLong_sz)goto _LL1E4;}}_LL1E3:
# 2152
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp45F,(long long)0),0);goto _LL1D3;_LL1E4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp452;if(_tmp460->tag != 7)goto _LL1E6;else{if(_tmp460->f1 != 0)goto _LL1E6;}}_LL1E5:
{const char*_tmpBED;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBED="0.0F",_tag_dyneither(_tmpBED,sizeof(char),5))),0),0);}goto _LL1D3;_LL1E6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp452;if(_tmp461->tag != 7)goto _LL1E8;else{if(_tmp461->f1 != 1)goto _LL1E8;}}_LL1E7:
{const char*_tmpBEE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBEE="0.0",_tag_dyneither(_tmpBEE,sizeof(char),4))),1),0);}goto _LL1D3;_LL1E8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp452;if(_tmp462->tag != 7)goto _LL1EA;else{_tmp463=_tmp462->f1;}}_LL1E9:
{const char*_tmpBEF;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBEF="0.0L",_tag_dyneither(_tmpBEF,sizeof(char),5))),_tmp463),0);}goto _LL1D3;_LL1EA:;_LL1EB: {
# 2157
const char*_tmpBF3;void*_tmpBF2[1];struct Cyc_String_pa_PrintArg_struct _tmpBF1;(_tmpBF1.tag=0,((_tmpBF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBF2[0]=& _tmpBF1,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF3="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBF3,sizeof(char),40))),_tag_dyneither(_tmpBF2,sizeof(void*),1)))))));}_LL1D3:;}
# 2159
res->topt=t;
return res;}
# 2166
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2179 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp46A=Cyc_Toc_typ_to_c(elt_type);
void*_tmp46B=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp46C=Cyc_Absyn_cstar_typ(_tmp46A,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpBF4;struct Cyc_Core_Opt*_tmp46D=(_tmpBF4=_cycalloc(sizeof(*_tmpBF4)),((_tmpBF4->v=_tmp46C,_tmpBF4)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp46E=e1->r;void*_tmp46F=_tmp46E;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp474;_LL1ED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp470=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp46F;if(_tmp470->tag != 19)goto _LL1EF;else{_tmp471=_tmp470->f1;}}_LL1EE:
# 2187
 if(!is_dyneither){
_tmp471=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp471,0,Cyc_Absyn_Other_coercion,0);
_tmp471->topt=fat_ptr_type;}
# 2191
Cyc_Toc_exp_to_c(nv,_tmp471);xinit=_tmp471;goto _LL1EC;_LL1EF: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp472=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp46F;if(_tmp472->tag != 22)goto _LL1F1;else{_tmp473=_tmp472->f1;_tmp474=_tmp472->f2;}}_LL1F0:
# 2193
 if(!is_dyneither){
_tmp473=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp473,0,Cyc_Absyn_Other_coercion,0);
_tmp473->topt=fat_ptr_type;}
# 2197
Cyc_Toc_exp_to_c(nv,_tmp473);Cyc_Toc_exp_to_c(nv,_tmp474);
{struct Cyc_Absyn_Exp*_tmpBF5[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBF5[2]=_tmp474,((_tmpBF5[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBF5[0]=_tmp473,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBF5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EC;_LL1F1:;_LL1F2: {
const char*_tmpBF8;void*_tmpBF7;(_tmpBF7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF8="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBF8,sizeof(char),53))),_tag_dyneither(_tmpBF7,sizeof(void*),0)));}_LL1EC:;}{
# 2203
struct _tuple1*_tmp478=Cyc_Toc_temp_var();
struct _RegionHandle _tmp479=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp479;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp47A=Cyc_Toc_add_varmap(rgn2,nv,_tmp478,Cyc_Absyn_var_exp(_tmp478,0));
struct Cyc_Absyn_Vardecl*_tmpBF9;struct Cyc_Absyn_Vardecl*_tmp47B=(_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->sc=Cyc_Absyn_Public,((_tmpBF9->name=_tmp478,((_tmpBF9->tq=Cyc_Toc_mt_tq,((_tmpBF9->type=_tmp46B,((_tmpBF9->initializer=xinit,((_tmpBF9->rgn=0,((_tmpBF9->attributes=0,((_tmpBF9->escapes=0,_tmpBF9)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBFC;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBFB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47C=(_tmpBFB=_cycalloc(sizeof(*_tmpBFB)),((_tmpBFB[0]=((_tmpBFC.tag=4,((_tmpBFC.f1=_tmp47B,_tmpBFC)))),_tmpBFB)));
struct Cyc_Absyn_Exp*_tmp47D=Cyc_Absyn_varb_exp(_tmp478,(void*)_tmp47C,0);
_tmp47D->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp47E=Cyc_Absyn_deref_exp(_tmp47D,0);
_tmp47E->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp47A,_tmp47E);{
struct _tuple1*_tmp47F=Cyc_Toc_temp_var();
_tmp47A=Cyc_Toc_add_varmap(rgn2,_tmp47A,_tmp47F,Cyc_Absyn_var_exp(_tmp47F,0));{
struct Cyc_Absyn_Vardecl*_tmpBFD;struct Cyc_Absyn_Vardecl*_tmp480=(_tmpBFD=_cycalloc(sizeof(*_tmpBFD)),((_tmpBFD->sc=Cyc_Absyn_Public,((_tmpBFD->name=_tmp47F,((_tmpBFD->tq=Cyc_Toc_mt_tq,((_tmpBFD->type=_tmp46A,((_tmpBFD->initializer=_tmp47E,((_tmpBFD->rgn=0,((_tmpBFD->attributes=0,((_tmpBFD->escapes=0,_tmpBFD)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC00;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBFF;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp481=(_tmpBFF=_cycalloc(sizeof(*_tmpBFF)),((_tmpBFF[0]=((_tmpC00.tag=4,((_tmpC00.f1=_tmp480,_tmpC00)))),_tmpBFF)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp482=Cyc_Absyn_varb_exp(_tmp47F,(void*)_tmp481,0);
_tmp482->topt=_tmp47E->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp482,e2,0);
z_init->topt=_tmp482->topt;}
# 2224
Cyc_Toc_exp_to_c(_tmp47A,z_init);{
struct _tuple1*_tmp483=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpC01;struct Cyc_Absyn_Vardecl*_tmp484=(_tmpC01=_cycalloc(sizeof(*_tmpC01)),((_tmpC01->sc=Cyc_Absyn_Public,((_tmpC01->name=_tmp483,((_tmpC01->tq=Cyc_Toc_mt_tq,((_tmpC01->type=_tmp46A,((_tmpC01->initializer=z_init,((_tmpC01->rgn=0,((_tmpC01->attributes=0,((_tmpC01->escapes=0,_tmpC01)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC04;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC03;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp485=(_tmpC03=_cycalloc(sizeof(*_tmpC03)),((_tmpC03[0]=((_tmpC04.tag=4,((_tmpC04.f1=_tmp484,_tmpC04)))),_tmpC03)));
_tmp47A=Cyc_Toc_add_varmap(rgn2,_tmp47A,_tmp483,Cyc_Absyn_var_exp(_tmp483,0));{
# 2231
struct Cyc_Absyn_Exp*_tmp486=Cyc_Absyn_varb_exp(_tmp47F,(void*)_tmp481,0);_tmp486->topt=_tmp47E->topt;{
struct Cyc_Absyn_Exp*_tmp487=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp486,_tmp487,0);
_tmp488->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp47A,_tmp488);{
# 2237
struct Cyc_Absyn_Exp*_tmp489=Cyc_Absyn_varb_exp(_tmp483,(void*)_tmp485,0);_tmp489->topt=_tmp47E->topt;{
struct Cyc_Absyn_Exp*_tmp48A=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp48B=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp489,_tmp48A,0);
_tmp48B->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp47A,_tmp48B);{
# 2243
struct Cyc_Absyn_Exp*_tmpC05[2];struct Cyc_List_List*_tmp48C=(_tmpC05[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC05[0]=
# 2243
Cyc_Absyn_varb_exp(_tmp478,(void*)_tmp47C,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC05,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2245
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp48C,0),_tmp48D,0);{
# 2250
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp488,_tmp48B,0),0);
struct Cyc_Absyn_Stmt*_tmp48F=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp490=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp478,(void*)_tmp47C,0),Cyc_Toc_curr_sp,0);
_tmp490=Cyc_Toc_cast_it(_tmp46C,_tmp490);{
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_deref_exp(_tmp490,0);
struct Cyc_Absyn_Exp*_tmp492=Cyc_Absyn_assign_exp(_tmp491,Cyc_Absyn_var_exp(_tmp483,0),0);
struct Cyc_Absyn_Stmt*_tmp493=Cyc_Absyn_exp_stmt(_tmp492,0);
_tmp493=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp48E,_tmp48F,Cyc_Absyn_skip_stmt(0),0),_tmp493,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC0B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC0A;struct Cyc_Absyn_Decl*_tmpC09;_tmp493=Cyc_Absyn_decl_stmt(((_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09->r=(void*)((_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=((_tmpC0A.tag=0,((_tmpC0A.f1=_tmp484,_tmpC0A)))),_tmpC0B)))),((_tmpC09->loc=0,_tmpC09)))))),_tmp493,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC11;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC10;struct Cyc_Absyn_Decl*_tmpC0F;_tmp493=Cyc_Absyn_decl_stmt(((_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F->r=(void*)((_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11[0]=((_tmpC10.tag=0,((_tmpC10.f1=_tmp480,_tmpC10)))),_tmpC11)))),((_tmpC0F->loc=0,_tmpC0F)))))),_tmp493,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC17;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC16;struct Cyc_Absyn_Decl*_tmpC15;_tmp493=Cyc_Absyn_decl_stmt(((_tmpC15=_cycalloc(sizeof(*_tmpC15)),((_tmpC15->r=(void*)((_tmpC17=_cycalloc(sizeof(*_tmpC17)),((_tmpC17[0]=((_tmpC16.tag=0,((_tmpC16.f1=_tmp47B,_tmpC16)))),_tmpC17)))),((_tmpC15->loc=0,_tmpC15)))))),_tmp493,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp493);};};};};};};};};};};};}
# 2205
;_pop_region(rgn2);};}
# 2276 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2280
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4A8=Cyc_Tcutil_compress(aggrtype);void*_tmp4A9=_tmp4A8;union Cyc_Absyn_AggrInfoU _tmp4AB;_LL1F4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4AA=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A9;if(_tmp4AA->tag != 11)goto _LL1F6;else{_tmp4AB=(_tmp4AA->f1).aggr_info;}}_LL1F5:
# 2283
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4AB);goto _LL1F3;_LL1F6:;_LL1F7: {
struct Cyc_String_pa_PrintArg_struct _tmpC1F;void*_tmpC1E[1];const char*_tmpC1D;void*_tmpC1C;(_tmpC1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC1F.tag=0,((_tmpC1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC1E[0]=& _tmpC1F,Cyc_aprintf(((_tmpC1D="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC1D,sizeof(char),51))),_tag_dyneither(_tmpC1E,sizeof(void*),1)))))))),_tag_dyneither(_tmpC1C,sizeof(void*),0)));}_LL1F3:;}{
# 2287
struct _tuple1*_tmp4B0=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Absyn_var_exp(_tmp4B0,0);
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_aggrarrow_exp(_tmp4B1,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4B4=Cyc_Absyn_neq_exp(_tmp4B3,_tmp4B2,0);
struct Cyc_Absyn_Exp*_tmp4B5=Cyc_Absyn_aggrarrow_exp(_tmp4B1,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4B6=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4B5,0),0);
struct Cyc_Absyn_Stmt*_tmp4B7=Cyc_Absyn_ifthenelse_stmt(_tmp4B4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4B8=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4BA=Cyc_Absyn_declare_stmt(_tmp4B0,_tmp4B8,_tmp4B9,Cyc_Absyn_seq_stmt(_tmp4B7,_tmp4B6,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4BA);}else{
# 2301
struct Cyc_Absyn_Exp*_tmp4BB=Cyc_Toc_member_exp(aggrproj(_tmp4B1,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_Absyn_neq_exp(_tmp4BB,_tmp4B2,0);
struct Cyc_Absyn_Exp*_tmp4BD=Cyc_Toc_member_exp(aggrproj(_tmp4B1,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4BE=Cyc_Absyn_exp_stmt(_tmp4BD,0);
struct Cyc_Absyn_Stmt*_tmp4BF=Cyc_Absyn_ifthenelse_stmt(_tmp4BC,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4C0=Cyc_Absyn_declare_stmt(_tmp4B0,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4BF,_tmp4BE,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4C0);}};}
# 2311
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2314
void*_tmp4C1=Cyc_Tcutil_compress(t);void*_tmp4C2=_tmp4C1;union Cyc_Absyn_AggrInfoU _tmp4C4;_LL1F9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4C3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C2;if(_tmp4C3->tag != 11)goto _LL1FB;else{_tmp4C4=(_tmp4C3->f1).aggr_info;}}_LL1FA: {
# 2316
struct Cyc_Absyn_Aggrdecl*_tmp4C5=Cyc_Absyn_get_known_aggrdecl(_tmp4C4);
*f_tag=Cyc_Toc_get_member_offset(_tmp4C5,f);{
# 2319
const char*_tmpC24;void*_tmpC23[2];struct Cyc_String_pa_PrintArg_struct _tmpC22;struct Cyc_String_pa_PrintArg_struct _tmpC21;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC21.tag=0,((_tmpC21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC22.tag=0,((_tmpC22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4C5->name).f2),((_tmpC23[0]=& _tmpC22,((_tmpC23[1]=& _tmpC21,Cyc_aprintf(((_tmpC24="_union_%s_%s",_tag_dyneither(_tmpC24,sizeof(char),13))),_tag_dyneither(_tmpC23,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC25;*tagged_member_type=Cyc_Absyn_strct(((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25[0]=str,_tmpC25)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4C5->impl))->tagged;};}_LL1FB:;_LL1FC:
 return 0;_LL1F8:;}
# 2330
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2333
void*_tmp4CB=e->r;void*_tmp4CC=_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CE;struct Cyc_Absyn_Exp*_tmp4D0;struct _dyneither_ptr*_tmp4D1;int*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D4;struct _dyneither_ptr*_tmp4D5;int*_tmp4D6;_LL1FE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4CC;if(_tmp4CD->tag != 13)goto _LL200;else{_tmp4CE=_tmp4CD->f2;}}_LL1FF: {
const char*_tmpC28;void*_tmpC27;(_tmpC27=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC28="cast on lhs!",_tag_dyneither(_tmpC28,sizeof(char),13))),_tag_dyneither(_tmpC27,sizeof(void*),0)));}_LL200: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4CF=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4CC;if(_tmp4CF->tag != 20)goto _LL202;else{_tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f2;_tmp4D2=(int*)& _tmp4CF->f4;}}_LL201:
# 2336
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4D0->topt),_tmp4D1,f_tag,tagged_member_type,clear_read,_tmp4D2);_LL202: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4D3=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4CC;if(_tmp4D3->tag != 21)goto _LL204;else{_tmp4D4=_tmp4D3->f1;_tmp4D5=_tmp4D3->f2;_tmp4D6=(int*)& _tmp4D3->f4;}}_LL203: {
# 2339
void*_tmp4D9=Cyc_Tcutil_compress((void*)_check_null(_tmp4D4->topt));void*_tmp4DA=_tmp4D9;void*_tmp4DC;_LL207: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4DB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4DA;if(_tmp4DB->tag != 5)goto _LL209;else{_tmp4DC=(_tmp4DB->f1).elt_typ;}}_LL208:
# 2341
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4DC,_tmp4D5,f_tag,tagged_member_type,clear_read,_tmp4D6);_LL209:;_LL20A:
# 2343
 return 0;_LL206:;}_LL204:;_LL205:
# 2345
 return 0;_LL1FD:;}
# 2357 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4DD=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4DD,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4DE=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4DE,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4DD,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2381 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2385
struct _tuple1*_tmp4DF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4DF,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2395
struct Cyc_Absyn_Exp*_tmp4E0=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4E0,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2398
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4DF,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2404
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2419 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4E1=Cyc_Absyn_var_exp(max,0);
# 2427
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4E1=Cyc_Absyn_add_exp(_tmp4E1,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2438
s=Cyc_Absyn_skip_stmt(0);}}{
# 2440
struct _RegionHandle _tmp4E2=_new_region("r");struct _RegionHandle*r=& _tmp4E2;_push_region(r);
{struct _tuple28*_tmpC2B;struct Cyc_List_List*_tmpC2A;struct Cyc_List_List*decls=
(_tmpC2A=_region_malloc(r,sizeof(*_tmpC2A)),((_tmpC2A->hd=((_tmpC2B=_region_malloc(r,sizeof(*_tmpC2B)),((_tmpC2B->f1=max,((_tmpC2B->f2=Cyc_Absyn_uint_typ,((_tmpC2B->f3=e1,_tmpC2B)))))))),((_tmpC2A->tl=0,_tmpC2A)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC2C[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2C[1]=_tmp4E1,((_tmpC2C[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2450
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC2D[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC2D[1]=_tmp4E1,((_tmpC2D[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2456
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC30;struct Cyc_List_List*_tmpC2F;decls=((_tmpC2F=_region_malloc(r,sizeof(*_tmpC2F)),((_tmpC2F->hd=((_tmpC30=_region_malloc(r,sizeof(*_tmpC30)),((_tmpC30->f1=a,((_tmpC30->f2=ptr_typ,((_tmpC30->f3=ainit,_tmpC30)))))))),((_tmpC2F->tl=decls,_tmpC2F))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC33;void*_tmpC32;(_tmpC32=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC33="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC33,sizeof(char),59))),_tag_dyneither(_tmpC32,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4E9=Cyc_Toc_temp_var();
void*_tmp4EA=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4EB=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC34[3];struct Cyc_Absyn_Exp*_tmp4EC=
Cyc_Absyn_fncall_exp(_tmp4EB,(
(_tmpC34[2]=_tmp4E1,((_tmpC34[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC34[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC34,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2468
{struct _tuple28*_tmpC37;struct Cyc_List_List*_tmpC36;decls=((_tmpC36=_region_malloc(r,sizeof(*_tmpC36)),((_tmpC36->hd=((_tmpC37=_region_malloc(r,sizeof(*_tmpC37)),((_tmpC37->f1=_tmp4E9,((_tmpC37->f2=_tmp4EA,((_tmpC37->f3=_tmp4EC,_tmpC37)))))))),((_tmpC36->tl=decls,_tmpC36))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4E9,0),0),0);}else{
# 2471
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4F0=decls;for(0;_tmp4F0 != 0;_tmp4F0=_tmp4F0->tl){
struct _tuple28 _tmp4F1=*((struct _tuple28*)_tmp4F0->hd);struct _tuple1*_tmp4F3;void*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct _tuple28 _tmp4F2=_tmp4F1;_tmp4F3=_tmp4F2.f1;_tmp4F4=_tmp4F2.f2;_tmp4F5=_tmp4F2.f3;
s=Cyc_Absyn_declare_stmt(_tmp4F3,_tmp4F4,_tmp4F5,s,0);}}
# 2476
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2441
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC86(unsigned int*_tmpC85,unsigned int*_tmpC84,struct _tuple1***_tmpC82){for(*_tmpC85=0;*_tmpC85 < *_tmpC84;(*_tmpC85)++){(*_tmpC82)[*_tmpC85]=(struct _tuple1*)
# 2851 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2480 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4F8=e->r;
if(e->topt == 0){
const char*_tmpC3B;void*_tmpC3A[1];struct Cyc_String_pa_PrintArg_struct _tmpC39;(_tmpC39.tag=0,((_tmpC39.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC3A[0]=& _tmpC39,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC3B="exp_to_c: no type for %s",_tag_dyneither(_tmpC3B,sizeof(char),25))),_tag_dyneither(_tmpC3A,sizeof(void*),1)))))));}
# 2486
if((nv->struct_info).lhs_exp != 0){
void*_tmp4FC=_tmp4F8;_LL20C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4FD=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4FC;if(_tmp4FD->tag != 28)goto _LL20E;}_LL20D:
 goto _LL20B;_LL20E:;_LL20F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL20B:;}{
# 2494
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4FE=_tmp4F8;struct _tuple1*_tmp502;void*_tmp503;enum Cyc_Absyn_Primop _tmp505;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Exp*_tmp508;enum Cyc_Absyn_Incrementor _tmp509;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Core_Opt*_tmp50C;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50F;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_List_List*_tmp520;int _tmp521;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_VarargInfo*_tmp523;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_List_List*_tmp52A;void**_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;int _tmp52E;enum Cyc_Absyn_Coercion _tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp536;void*_tmp538;void*_tmp53A;struct Cyc_List_List*_tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53F;struct _dyneither_ptr*_tmp540;int _tmp541;int _tmp542;struct Cyc_Absyn_Exp*_tmp544;struct _dyneither_ptr*_tmp545;int _tmp546;int _tmp547;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54E;struct Cyc_Absyn_Vardecl*_tmp550;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;int _tmp553;struct Cyc_Absyn_Exp*_tmp555;void*_tmp556;int _tmp557;struct _tuple1*_tmp559;struct Cyc_List_List*_tmp55A;struct Cyc_List_List*_tmp55B;struct Cyc_Absyn_Aggrdecl*_tmp55C;void*_tmp55E;struct Cyc_List_List*_tmp55F;struct Cyc_List_List*_tmp561;struct Cyc_Absyn_Datatypedecl*_tmp562;struct Cyc_Absyn_Datatypefield*_tmp563;int _tmp567;struct Cyc_Absyn_Exp*_tmp568;void**_tmp569;struct Cyc_Absyn_Exp*_tmp56A;int _tmp56B;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp570;struct _dyneither_ptr*_tmp571;struct Cyc_Absyn_Stmt*_tmp573;_LL211: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4FF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp4FF->tag != 0)goto _LL213;else{if(((_tmp4FF->f1).Null_c).tag != 1)goto _LL213;}}_LL212: {
# 2501
struct Cyc_Absyn_Exp*_tmp578=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp578,_tmp578))->r;else{
# 2506
struct Cyc_Absyn_Exp*_tmpC3C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC3C[2]=_tmp578,((_tmpC3C[1]=_tmp578,((_tmpC3C[0]=_tmp578,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2508
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2510
goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp500=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp500->tag != 0)goto _LL215;}_LL214:
 goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp501->tag != 1)goto _LL217;else{_tmp502=_tmp501->f1;_tmp503=(void*)_tmp501->f2;}}_LL216:
# 2513
{struct _handler_cons _tmp57A;_push_handler(& _tmp57A);{int _tmp57C=0;if(setjmp(_tmp57A.handler))_tmp57C=1;if(!_tmp57C){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp502))->r;;_pop_handler();}else{void*_tmp57B=(void*)_exn_thrown;void*_tmp57E=_tmp57B;void*_tmp580;_LL266: {struct Cyc_Dict_Absent_exn_struct*_tmp57F=(struct Cyc_Dict_Absent_exn_struct*)_tmp57E;if(_tmp57F->tag != Cyc_Dict_Absent)goto _LL268;}_LL267: {
# 2515
const char*_tmpC40;void*_tmpC3F[1];struct Cyc_String_pa_PrintArg_struct _tmpC3E;(_tmpC3E.tag=0,((_tmpC3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp502)),((_tmpC3F[0]=& _tmpC3E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC40="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC40,sizeof(char),32))),_tag_dyneither(_tmpC3F,sizeof(void*),1)))))));}_LL268: _tmp580=_tmp57E;_LL269:(void)_throw(_tmp580);_LL265:;}};}
# 2517
goto _LL210;_LL217: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp504->tag != 2)goto _LL219;else{_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;}}_LL218: {
# 2520
struct Cyc_List_List*_tmp584=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp506);
# 2522
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp506);
switch(_tmp505){case Cyc_Absyn_Numelts: _LL26A: {
# 2525
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506))->hd;
{void*_tmp585=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp586=_tmp585;struct Cyc_Absyn_Exp*_tmp588;void*_tmp58A;union Cyc_Absyn_Constraint*_tmp58B;union Cyc_Absyn_Constraint*_tmp58C;union Cyc_Absyn_Constraint*_tmp58D;_LL26D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp587=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp586;if(_tmp587->tag != 8)goto _LL26F;else{_tmp588=(_tmp587->f1).num_elts;}}_LL26E:
# 2529
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp588))){
const char*_tmpC43;void*_tmpC42;(_tmpC42=0,Cyc_Tcutil_terr(e->loc,((_tmpC43="can't calculate numelts",_tag_dyneither(_tmpC43,sizeof(char),24))),_tag_dyneither(_tmpC42,sizeof(void*),0)));}
e->r=_tmp588->r;goto _LL26C;_LL26F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp589=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586;if(_tmp589->tag != 5)goto _LL271;else{_tmp58A=(_tmp589->f1).elt_typ;_tmp58B=((_tmp589->f1).ptr_atts).nullable;_tmp58C=((_tmp589->f1).ptr_atts).bounds;_tmp58D=((_tmp589->f1).ptr_atts).zero_term;}}_LL270:
# 2533
{void*_tmp590=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp58C);void*_tmp591=_tmp590;struct Cyc_Absyn_Exp*_tmp594;_LL274: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp592=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp591;if(_tmp592->tag != 0)goto _LL276;}_LL275:
# 2535
{struct Cyc_Absyn_Exp*_tmpC44[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC44[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp58A),0),((_tmpC44[0]=(struct Cyc_Absyn_Exp*)_tmp506->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL273;_LL276: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp593=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp591;if(_tmp593->tag != 1)goto _LL273;else{_tmp594=_tmp593->f1;}}_LL277:
# 2541
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58D)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp506->hd);
# 2545
struct Cyc_Absyn_Exp*_tmpC45[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC45[1]=_tmp594,((_tmpC45[0]=(struct Cyc_Absyn_Exp*)_tmp506->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC45,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58B)){
if(!Cyc_Evexp_c_can_eval(_tmp594)){
const char*_tmpC48;void*_tmpC47;(_tmpC47=0,Cyc_Tcutil_terr(e->loc,((_tmpC48="can't calculate numelts",_tag_dyneither(_tmpC48,sizeof(char),24))),_tag_dyneither(_tmpC47,sizeof(void*),0)));}
# 2550
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp594,Cyc_Absyn_uint_exp(0,0));}else{
# 2552
e->r=_tmp594->r;goto _LL273;}}
# 2554
goto _LL273;_LL273:;}
# 2556
goto _LL26C;_LL271:;_LL272: {
# 2558
const char*_tmpC4D;void*_tmpC4C[2];struct Cyc_String_pa_PrintArg_struct _tmpC4B;struct Cyc_String_pa_PrintArg_struct _tmpC4A;(_tmpC4A.tag=0,((_tmpC4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC4B.tag=0,((_tmpC4B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC4C[0]=& _tmpC4B,((_tmpC4C[1]=& _tmpC4A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4D="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC4D,sizeof(char),41))),_tag_dyneither(_tmpC4C,sizeof(void*),2)))))))))))));}_LL26C:;}
# 2561
break;}case Cyc_Absyn_Plus: _LL26B:
# 2566
{void*_tmp59D=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp584))->hd);void*_tmp59E=_tmp59D;void*_tmp5A0;union Cyc_Absyn_Constraint*_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A2;_LL27A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59E;if(_tmp59F->tag != 5)goto _LL27C;else{_tmp5A0=(_tmp59F->f1).elt_typ;_tmp5A1=((_tmp59F->f1).ptr_atts).bounds;_tmp5A2=((_tmp59F->f1).ptr_atts).zero_term;}}_LL27B:
# 2568
{void*_tmp5A3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A1);void*_tmp5A4=_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A7;_LL27F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5A5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5A4;if(_tmp5A5->tag != 0)goto _LL281;}_LL280: {
# 2570
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC4E[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC4E[2]=e2,((_tmpC4E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A0),0),((_tmpC4E[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL27E;}_LL281: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5A6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5A4;if(_tmp5A6->tag != 1)goto _LL27E;else{_tmp5A7=_tmp5A6->f1;}}_LL282:
# 2576
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A2)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC4F[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC4F[2]=e2,((_tmpC4F[1]=_tmp5A7,((_tmpC4F[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2581
goto _LL27E;_LL27E:;}
# 2583
goto _LL279;_LL27C:;_LL27D:
# 2585
 goto _LL279;_LL279:;}
# 2587
break;case Cyc_Absyn_Minus: _LL278: {
# 2592
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp584))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp584->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2602
struct Cyc_Absyn_Exp*_tmpC50[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC50[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC50[1]=
# 2603
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC50[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC50,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2607
break;}case Cyc_Absyn_Eq: _LL283:
 goto _LL284;case Cyc_Absyn_Neq: _LL284:
 goto _LL285;case Cyc_Absyn_Gt: _LL285:
 goto _LL286;case Cyc_Absyn_Gte: _LL286:
 goto _LL287;case Cyc_Absyn_Lt: _LL287:
 goto _LL288;case Cyc_Absyn_Lte: _LL288: {
# 2615
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp506->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp584))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp584->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL289:
 break;}
# 2627
goto _LL210;}_LL219: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp507=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp507->tag != 4)goto _LL21B;else{_tmp508=_tmp507->f1;_tmp509=_tmp507->f2;}}_LL21A: {
# 2629
void*e2_cyc_typ=(void*)_check_null(_tmp508->topt);
# 2638 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC51;struct _dyneither_ptr incr_str=(_tmpC51="increment",_tag_dyneither(_tmpC51,sizeof(char),10));
if(_tmp509 == Cyc_Absyn_PreDec  || _tmp509 == Cyc_Absyn_PostDec){const char*_tmpC52;incr_str=((_tmpC52="decrement",_tag_dyneither(_tmpC52,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp508,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC56;void*_tmpC55[1];struct Cyc_String_pa_PrintArg_struct _tmpC54;(_tmpC54.tag=0,((_tmpC54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC55[0]=& _tmpC54,Cyc_Tcutil_terr(e->loc,((_tmpC56="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC56,sizeof(char),74))),_tag_dyneither(_tmpC55,sizeof(void*),1)))))));}{
const char*_tmpC59;void*_tmpC58;(_tmpC58=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC59="in-place inc/dec on zero-term",_tag_dyneither(_tmpC59,sizeof(char),30))),_tag_dyneither(_tmpC58,sizeof(void*),0)));};}{
# 2647
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp508,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5B1=Cyc_Absyn_signed_int_exp(1,0);
_tmp5B1->topt=Cyc_Absyn_sint_typ;
switch(_tmp509){case Cyc_Absyn_PreInc: _LL28B:
# 2654
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC5F;struct Cyc_Core_Opt*_tmpC5E;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC5D;e->r=(void*)((_tmpC5D=_cycalloc(sizeof(*_tmpC5D)),((_tmpC5D[0]=((_tmpC5F.tag=3,((_tmpC5F.f1=_tmp508,((_tmpC5F.f2=((_tmpC5E=_cycalloc_atomic(sizeof(*_tmpC5E)),((_tmpC5E->v=(void*)Cyc_Absyn_Plus,_tmpC5E)))),((_tmpC5F.f3=_tmp5B1,_tmpC5F)))))))),_tmpC5D))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL28C:
# 2658
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC65;struct Cyc_Core_Opt*_tmpC64;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC63;e->r=(void*)((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63[0]=((_tmpC65.tag=3,((_tmpC65.f1=_tmp508,((_tmpC65.f2=((_tmpC64=_cycalloc_atomic(sizeof(*_tmpC64)),((_tmpC64->v=(void*)Cyc_Absyn_Minus,_tmpC64)))),((_tmpC65.f3=_tmp5B1,_tmpC65)))))))),_tmpC63))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL28D:
# 2662
{const char*_tmpC69;void*_tmpC68[1];struct Cyc_String_pa_PrintArg_struct _tmpC67;(_tmpC67.tag=0,((_tmpC67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC68[0]=& _tmpC67,Cyc_Tcutil_terr(e->loc,((_tmpC69="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC69,sizeof(char),59))),_tag_dyneither(_tmpC68,sizeof(void*),1)))))));}{
# 2664
const char*_tmpC6C;void*_tmpC6B;(_tmpC6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC6C="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC6C,sizeof(char),34))),_tag_dyneither(_tmpC6B,sizeof(void*),0)));};}}
# 2667
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp508);
Cyc_Toc_set_lhs(nv,0);{
# 2672
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp509 == Cyc_Absyn_PostInc  || _tmp509 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2678
if(_tmp509 == Cyc_Absyn_PreDec  || _tmp509 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpC6D[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC6D[2]=
# 2682
Cyc_Absyn_signed_int_exp(change,0),((_tmpC6D[1]=
# 2681
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC6D[0]=
# 2680
Cyc_Toc_push_address_exp(_tmp508),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2683
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2687
struct Cyc_Toc_functionSet*_tmp5BE=_tmp509 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2689
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5BE,_tmp508);
struct Cyc_Absyn_Exp*_tmpC6E[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC6E[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC6E[0]=
# 2690
Cyc_Toc_push_address_exp(_tmp508),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2692
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp508)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp508,0,Cyc_Toc_incr_lvalue,_tmp509);
e->r=_tmp508->r;}}}
# 2696
goto _LL210;};};}_LL21B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp50A=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp50A->tag != 3)goto _LL21D;else{_tmp50B=_tmp50A->f1;_tmp50C=_tmp50A->f2;_tmp50D=_tmp50A->f3;}}_LL21C: {
# 2715 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp50B->topt);
void*e2_old_typ=(void*)_check_null(_tmp50D->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp50B,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50B);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp50D);
e->r=Cyc_Toc_tagged_union_assignop(_tmp50B,e1_old_typ,_tmp50C,_tmp50D,e2_old_typ,f_tag,tagged_member_struct_type);
# 2726
return;}{
# 2728
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp50B,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp50B,_tmp50C,_tmp50D,ptr_type,is_dyneither,elt_type);
# 2734
return;}{
# 2738
int e1_poly=Cyc_Toc_is_poly_project(_tmp50B);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp50B);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp50D);{
# 2744
int done=0;
if(_tmp50C != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5C1=(enum Cyc_Absyn_Primop)_tmp50C->v;switch(_tmp5C1){case Cyc_Absyn_Plus: _LL28F:
 change=_tmp50D;break;case Cyc_Absyn_Minus: _LL290:
# 2752
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp50D,0);break;default: _LL291: {
const char*_tmpC71;void*_tmpC70;(_tmpC70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC71="bad t ? pointer arithmetic",_tag_dyneither(_tmpC71,sizeof(char),27))),_tag_dyneither(_tmpC70,sizeof(void*),0)));}}}
# 2755
done=1;{
# 2757
struct Cyc_Absyn_Exp*_tmp5C4=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC72[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5C4,((_tmpC72[2]=change,((_tmpC72[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC72[0]=
# 2758
Cyc_Toc_push_address_exp(_tmp50B),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC72,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2761
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2764
enum Cyc_Absyn_Primop _tmp5C6=(enum Cyc_Absyn_Primop)_tmp50C->v;switch(_tmp5C6){case Cyc_Absyn_Plus: _LL293:
# 2766
 done=1;
{struct Cyc_Absyn_Exp*_tmpC73[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp50B),((_tmpC73[1]=_tmp50D,((_tmpC73[0]=_tmp50B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC73,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL294: {
const char*_tmpC76;void*_tmpC75;(_tmpC75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC76="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC76,sizeof(char),39))),_tag_dyneither(_tmpC75,sizeof(void*),0)));}}}}}
# 2773
if(!done){
# 2775
if(e1_poly)
_tmp50D->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp50D->r,0));
# 2781
if(!Cyc_Absyn_is_lvalue(_tmp50B)){
{struct _tuple27 _tmpC79;struct _tuple27*_tmpC78;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp50B,0,Cyc_Toc_assignop_lvalue,((_tmpC78=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpC78[0]=(struct _tuple27)((_tmpC79.f1=_tmp50C,((_tmpC79.f2=_tmp50D,_tmpC79)))),_tmpC78)))));}
e->r=_tmp50B->r;}}
# 2786
goto _LL210;};};};}_LL21D: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp50E=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp50E->tag != 5)goto _LL21F;else{_tmp50F=_tmp50E->f1;_tmp510=_tmp50E->f2;_tmp511=_tmp50E->f3;}}_LL21E:
# 2788
 Cyc_Toc_exp_to_c(nv,_tmp50F);
Cyc_Toc_exp_to_c(nv,_tmp510);
Cyc_Toc_exp_to_c(nv,_tmp511);
goto _LL210;_LL21F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp512->tag != 6)goto _LL221;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}_LL220:
# 2793
 Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_exp_to_c(nv,_tmp514);
goto _LL210;_LL221: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp515->tag != 7)goto _LL223;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f2;}}_LL222:
# 2797
 Cyc_Toc_exp_to_c(nv,_tmp516);
Cyc_Toc_exp_to_c(nv,_tmp517);
goto _LL210;_LL223: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp518->tag != 8)goto _LL225;else{_tmp519=_tmp518->f1;_tmp51A=_tmp518->f2;}}_LL224:
# 2801
 Cyc_Toc_exp_to_c(nv,_tmp519);
Cyc_Toc_exp_to_c(nv,_tmp51A);
goto _LL210;_LL225: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp51B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp51B->tag != 9)goto _LL227;else{_tmp51C=_tmp51B->f1;_tmp51D=_tmp51B->f2;if(_tmp51B->f3 != 0)goto _LL227;}}_LL226:
# 2805
 Cyc_Toc_exp_to_c(nv,_tmp51C);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp51D);
goto _LL210;_LL227: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp51E->tag != 9)goto _LL229;else{_tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;if(_tmp51E->f3 == 0)goto _LL229;_tmp521=(_tmp51E->f3)->num_varargs;_tmp522=(_tmp51E->f3)->injectors;_tmp523=(_tmp51E->f3)->vai;}}_LL228: {
# 2817 "toc.cyc"
struct _RegionHandle _tmp5CC=_new_region("r");struct _RegionHandle*r=& _tmp5CC;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp521,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp523->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2825
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp520);
int num_normargs=num_args - _tmp521;
# 2829
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp520=_tmp520->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp520))->hd);{
struct Cyc_List_List*_tmpC7A;new_args=((_tmpC7A=_cycalloc(sizeof(*_tmpC7A)),((_tmpC7A->hd=(struct Cyc_Absyn_Exp*)_tmp520->hd,((_tmpC7A->tl=new_args,_tmpC7A))))));};}}
# 2834
{struct Cyc_Absyn_Exp*_tmpC7D[3];struct Cyc_List_List*_tmpC7C;new_args=((_tmpC7C=_cycalloc(sizeof(*_tmpC7C)),((_tmpC7C->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC7D[2]=num_varargs_exp,((_tmpC7D[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC7D[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC7C->tl=new_args,_tmpC7C))))));}
# 2839
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2841
Cyc_Toc_exp_to_c(nv,_tmp51F);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp51F,new_args,0),0);
# 2845
if(_tmp523->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5D0=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp523->type));void*_tmp5D1=_tmp5D0;struct Cyc_Absyn_Datatypedecl*_tmp5D3;_LL297: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5D2=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5D1;if(_tmp5D2->tag != 3)goto _LL299;else{if((((_tmp5D2->f1).datatype_info).KnownDatatype).tag != 2)goto _LL299;_tmp5D3=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5D2->f1).datatype_info).KnownDatatype).val);}}_LL298:
 tud=_tmp5D3;goto _LL296;_LL299:;_LL29A: {
const char*_tmpC80;void*_tmpC7F;(_tmpC7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC80="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC80,sizeof(char),44))),_tag_dyneither(_tmpC7F,sizeof(void*),0)));}_LL296:;}{
# 2851
unsigned int _tmpC85;unsigned int _tmpC84;struct _dyneither_ptr _tmpC83;struct _tuple1**_tmpC82;unsigned int _tmpC81;struct _dyneither_ptr vs=(_tmpC81=(unsigned int)_tmp521,((_tmpC82=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpC81)),((_tmpC83=_tag_dyneither(_tmpC82,sizeof(struct _tuple1*),_tmpC81),((((_tmpC84=_tmpC81,_tmpC86(& _tmpC85,& _tmpC84,& _tmpC82))),_tmpC83)))))));
# 2853
if(_tmp521 != 0){
# 2855
struct Cyc_List_List*_tmp5D6=0;
{int i=_tmp521 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC87;_tmp5D6=((_tmpC87=_cycalloc(sizeof(*_tmpC87)),((_tmpC87->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpC87->tl=_tmp5D6,_tmpC87))))));}}
# 2859
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5D6,0),s,0);{
# 2862
int i=0;for(0;_tmp520 != 0;(((_tmp520=_tmp520->tl,_tmp522=_tmp522->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp520->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5D8=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp522))->hd;struct _tuple1*_tmp5DA;struct Cyc_List_List*_tmp5DB;struct Cyc_Absyn_Datatypefield*_tmp5D9=_tmp5D8;_tmp5DA=_tmp5D9->name;_tmp5DB=_tmp5D9->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5DB))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2877
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2880
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5DA),0),s,0);
# 2883
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5DA,tud->name)),0,s,0);};}};}else{
# 2890
struct _tuple19*_tmpC88[3];struct Cyc_List_List*_tmp5DC=(_tmpC88[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC88[1]=
# 2890
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC88[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC88,sizeof(struct _tuple19*),3)))))));
# 2892
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2902
{int i=0;for(0;_tmp520 != 0;(_tmp520=_tmp520->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp520->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp520->hd,0),s,0);}}
# 2908
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2921 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2923
_npop_handler(0);goto _LL210;
# 2817 "toc.cyc"
;_pop_region(r);}_LL229: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp524=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp524->tag != 10)goto _LL22B;else{_tmp525=_tmp524->f1;}}_LL22A:
# 2926 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp525);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp525),0))->r;
goto _LL210;_LL22B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp526->tag != 11)goto _LL22D;else{_tmp527=_tmp526->f1;}}_LL22C:
 Cyc_Toc_exp_to_c(nv,_tmp527);goto _LL210;_LL22D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp528=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp528->tag != 12)goto _LL22F;else{_tmp529=_tmp528->f1;_tmp52A=_tmp528->f2;}}_LL22E:
# 2931
 Cyc_Toc_exp_to_c(nv,_tmp529);
# 2940 "toc.cyc"
for(0;_tmp52A != 0;_tmp52A=_tmp52A->tl){
enum Cyc_Absyn_KindQual _tmp5E2=(Cyc_Tcutil_typ_kind((void*)_tmp52A->hd))->kind;
if(_tmp5E2 != Cyc_Absyn_EffKind  && _tmp5E2 != Cyc_Absyn_RgnKind){
{void*_tmp5E3=Cyc_Tcutil_compress((void*)_tmp52A->hd);void*_tmp5E4=_tmp5E3;_LL29C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E4;if(_tmp5E5->tag != 2)goto _LL29E;}_LL29D:
 goto _LL29F;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5E6=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5E4;if(_tmp5E6->tag != 3)goto _LL2A0;}_LL29F:
 continue;_LL2A0:;_LL2A1:
# 2947
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp529,0))->r;
goto _LL29B;_LL29B:;}
# 2950
break;}}
# 2953
goto _LL210;_LL22F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp52B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp52B->tag != 13)goto _LL231;else{_tmp52C=(void**)((void**)& _tmp52B->f1);_tmp52D=_tmp52B->f2;_tmp52E=_tmp52B->f3;_tmp52F=_tmp52B->f4;}}_LL230: {
# 2955
void*old_t2=(void*)_check_null(_tmp52D->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp52C;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp52C=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp52D);
# 2962
{struct _tuple29 _tmpC89;struct _tuple29 _tmp5E7=(_tmpC89.f1=Cyc_Tcutil_compress(old_t2),((_tmpC89.f2=Cyc_Tcutil_compress(new_typ),_tmpC89)));struct _tuple29 _tmp5E8=_tmp5E7;struct Cyc_Absyn_PtrInfo _tmp5EA;struct Cyc_Absyn_PtrInfo _tmp5EC;struct Cyc_Absyn_PtrInfo _tmp5EE;_LL2A3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E8.f1;if(_tmp5E9->tag != 5)goto _LL2A5;else{_tmp5EA=_tmp5E9->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5EB=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E8.f2;if(_tmp5EB->tag != 5)goto _LL2A5;else{_tmp5EC=_tmp5EB->f1;}};_LL2A4: {
# 2964
int _tmp5F0=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5EA.ptr_atts).nullable);
int _tmp5F1=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5EC.ptr_atts).nullable);
void*_tmp5F2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5EA.ptr_atts).bounds);
void*_tmp5F3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5EC.ptr_atts).bounds);
int _tmp5F4=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5EA.ptr_atts).zero_term);
int _tmp5F5=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5EC.ptr_atts).zero_term);
{struct _tuple29 _tmpC8A;struct _tuple29 _tmp5F6=(_tmpC8A.f1=_tmp5F2,((_tmpC8A.f2=_tmp5F3,_tmpC8A)));struct _tuple29 _tmp5F7=_tmp5F6;struct Cyc_Absyn_Exp*_tmp5F9;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FD;struct Cyc_Absyn_Exp*_tmp601;_LL2AA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F8=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F7.f1;if(_tmp5F8->tag != 1)goto _LL2AC;else{_tmp5F9=_tmp5F8->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5FA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F7.f2;if(_tmp5FA->tag != 1)goto _LL2AC;else{_tmp5FB=_tmp5FA->f1;}};_LL2AB:
# 2972
 if((!Cyc_Evexp_c_can_eval(_tmp5F9) || !Cyc_Evexp_c_can_eval(_tmp5FB)) && !
Cyc_Evexp_same_const_exp(_tmp5F9,_tmp5FB)){
const char*_tmpC8D;void*_tmpC8C;(_tmpC8C=0,Cyc_Tcutil_terr(e->loc,((_tmpC8D="can't validate cast due to potential size differences",_tag_dyneither(_tmpC8D,sizeof(char),54))),_tag_dyneither(_tmpC8C,sizeof(void*),0)));}
if(_tmp5F0  && !_tmp5F1){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC90="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC90,sizeof(char),44))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}
# 2981
if(_tmp52F != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC94;void*_tmpC93[1];struct Cyc_String_pa_PrintArg_struct _tmpC92;(_tmpC92.tag=0,((_tmpC92.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC93[0]=& _tmpC92,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC94="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC94,sizeof(char),41))),_tag_dyneither(_tmpC93,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp52D);
if(do_null_check){
if(!_tmp52E){
const char*_tmpC97;void*_tmpC96;(_tmpC96=0,Cyc_Tcutil_warn(e->loc,((_tmpC97="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC97,sizeof(char),58))),_tag_dyneither(_tmpC96,sizeof(void*),0)));}{
# 2988
struct Cyc_List_List*_tmpC98;e->r=Cyc_Toc_cast_it_r(*_tmp52C,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC98=_cycalloc(sizeof(*_tmpC98)),((_tmpC98->hd=_tmp52D,((_tmpC98->tl=0,_tmpC98)))))),0));};}else{
# 2992
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52D->r;}};}else{
# 2997
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52D->r;}
# 3004
goto _LL2A9;_LL2AC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5FC=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F7.f1;if(_tmp5FC->tag != 1)goto _LL2AE;else{_tmp5FD=_tmp5FC->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FE=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F7.f2;if(_tmp5FE->tag != 0)goto _LL2AE;};_LL2AD:
# 3006
 if(!Cyc_Evexp_c_can_eval(_tmp5FD)){
const char*_tmpC9B;void*_tmpC9A;(_tmpC9A=0,Cyc_Tcutil_terr(e->loc,((_tmpC9B="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC9B,sizeof(char),71))),_tag_dyneither(_tmpC9A,sizeof(void*),0)));}
# 3009
if(_tmp52F == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC9F;void*_tmpC9E[1];struct Cyc_String_pa_PrintArg_struct _tmpC9D;(_tmpC9D.tag=0,((_tmpC9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC9E[0]=& _tmpC9D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC9F="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC9F,sizeof(char),44))),_tag_dyneither(_tmpC9E,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3013
if((_tmp5F4  && !(_tmp5EC.elt_tq).real_const) && !_tmp5F5)
# 3016
_tmp5FD=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5FD,Cyc_Absyn_uint_exp(1,0),0);
# 3018
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5FD,_tmp52D))->r;}else{
# 3020
struct Cyc_Absyn_Exp*_tmp613=Cyc_Toc__tag_dyneither_e;
# 3022
if(_tmp5F4){
# 3027
struct _tuple1*_tmp614=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp615=Cyc_Absyn_var_exp(_tmp614,0);
struct Cyc_Absyn_Exp*arg3;
# 3032
{void*_tmp616=_tmp52D->r;void*_tmp617=_tmp616;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp618=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp617;if(_tmp618->tag != 0)goto _LL2B5;else{if(((_tmp618->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4:
# 3034
 arg3=_tmp5FD;goto _LL2B2;_LL2B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp619=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp617;if(_tmp619->tag != 0)goto _LL2B7;else{if(((_tmp619->f1).Wstring_c).tag != 9)goto _LL2B7;}}_LL2B6:
# 3036
 arg3=_tmp5FD;goto _LL2B2;_LL2B7:;_LL2B8:
# 3038
{struct Cyc_Absyn_Exp*_tmpCA0[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp52D),(
# 3040
(_tmpCA0[1]=_tmp5FD,((_tmpCA0[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp615),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B2;_LL2B2:;}
# 3043
if(!_tmp5F5  && !(_tmp5EC.elt_tq).real_const)
# 3046
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3048
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5EC.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpCA1[3];struct Cyc_Absyn_Exp*_tmp61C=Cyc_Absyn_fncall_exp(_tmp613,((_tmpCA1[2]=arg3,((_tmpCA1[1]=_tmp61B,((_tmpCA1[0]=_tmp615,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp61D=Cyc_Absyn_exp_stmt(_tmp61C,0);
_tmp61D=Cyc_Absyn_declare_stmt(_tmp614,Cyc_Toc_typ_to_c(old_t2),_tmp52D,_tmp61D,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp61D);};}else{
# 3055
struct Cyc_Absyn_Exp*_tmpCA2[3];e->r=Cyc_Toc_fncall_exp_r(_tmp613,(
(_tmpCA2[2]=_tmp5FD,((_tmpCA2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5EC.elt_typ),0),((_tmpCA2[0]=_tmp52D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3061
goto _LL2A9;_LL2AE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FF=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F7.f1;if(_tmp5FF->tag != 0)goto _LL2B0;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp600=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F7.f2;if(_tmp600->tag != 1)goto _LL2B0;else{_tmp601=_tmp600->f1;}};_LL2AF:
# 3063
 if(!Cyc_Evexp_c_can_eval(_tmp601)){
const char*_tmpCA5;void*_tmpCA4;(_tmpCA4=0,Cyc_Tcutil_terr(e->loc,((_tmpCA5="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCA5,sizeof(char),71))),_tag_dyneither(_tmpCA4,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCA8;void*_tmpCA7;(_tmpCA7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA8="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpCA8,sizeof(char),45))),_tag_dyneither(_tmpCA7,sizeof(void*),0)));}{
# 3075 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp624=_tmp601;
if(_tmp5F4  && !_tmp5F5)
_tmp624=Cyc_Absyn_add_exp(_tmp601,Cyc_Absyn_uint_exp(1,0),0);{
# 3082
struct Cyc_Absyn_Exp*_tmp625=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCA9[3];struct Cyc_Absyn_Exp*_tmp626=Cyc_Absyn_fncall_exp(_tmp625,(
(_tmpCA9[2]=_tmp624,((_tmpCA9[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5EA.elt_typ),0),((_tmpCA9[0]=_tmp52D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3087
if(_tmp5F1){
struct Cyc_List_List*_tmpCAA;_tmp626->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpCAA=_cycalloc(sizeof(*_tmpCAA)),((_tmpCAA->hd=Cyc_Absyn_copy_exp(_tmp626),((_tmpCAA->tl=0,_tmpCAA)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp52C,_tmp626);
goto _LL2A9;};};_LL2B0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp602=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F7.f1;if(_tmp602->tag != 0)goto _LL2A9;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp603=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F7.f2;if(_tmp603->tag != 0)goto _LL2A9;};_LL2B1:
# 3095
 DynCast:
 if((_tmp5F4  && !_tmp5F5) && !(_tmp5EC.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCAD="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpCAD,sizeof(char),70))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpCAE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp62B,(
(_tmpCAE[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpCAE[1]=
# 3101
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5EA.elt_typ),0),((_tmpCAE[0]=_tmp52D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCAE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3104
goto _LL2A9;_LL2A9:;}
# 3106
goto _LL2A2;}_LL2A5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E8.f1;if(_tmp5ED->tag != 5)goto _LL2A7;else{_tmp5EE=_tmp5ED->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5EF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E8.f2;if(_tmp5EF->tag != 6)goto _LL2A7;};_LL2A6:
# 3108
{void*_tmp62E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5EE.ptr_atts).bounds);void*_tmp62F=_tmp62E;_LL2BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp630=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp62F;if(_tmp630->tag != 0)goto _LL2BC;}_LL2BB:
# 3110
 _tmp52D->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp52D->r,_tmp52D->loc),Cyc_Toc_curr_sp);goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 3113
goto _LL2A2;_LL2A7:;_LL2A8:
# 3115
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp52D->r;
goto _LL2A2;_LL2A2:;}
# 3119
goto _LL210;}_LL231: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp530=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp530->tag != 14)goto _LL233;else{_tmp531=_tmp530->f1;}}_LL232:
# 3123
{void*_tmp632=_tmp531->r;void*_tmp633=_tmp632;struct _tuple1*_tmp635;struct Cyc_List_List*_tmp636;struct Cyc_List_List*_tmp637;struct Cyc_List_List*_tmp639;_LL2BF: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp634=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp633;if(_tmp634->tag != 28)goto _LL2C1;else{_tmp635=_tmp634->f1;_tmp636=_tmp634->f2;_tmp637=_tmp634->f3;}}_LL2C0:
# 3125
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCB2;void*_tmpCB1[1];struct Cyc_String_pa_PrintArg_struct _tmpCB0;(_tmpCB0.tag=0,((_tmpCB0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp531->loc)),((_tmpCB1[0]=& _tmpCB0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB2="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCB2,sizeof(char),42))),_tag_dyneither(_tmpCB1,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp63D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp531->topt),_tmp636,1,0,_tmp637,_tmp635);
e->r=_tmp63D->r;
e->topt=_tmp63D->topt;
goto _LL2BE;};_LL2C1: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp638=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp633;if(_tmp638->tag != 23)goto _LL2C3;else{_tmp639=_tmp638->f1;}}_LL2C2:
# 3133
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCB6;void*_tmpCB5[1];struct Cyc_String_pa_PrintArg_struct _tmpCB4;(_tmpCB4.tag=0,((_tmpCB4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp531->loc)),((_tmpCB5[0]=& _tmpCB4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB6="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCB6,sizeof(char),42))),_tag_dyneither(_tmpCB5,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp639))->r;
goto _LL2BE;_LL2C3:;_LL2C4:
# 3139
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp531);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp531)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp531,0,Cyc_Toc_address_lvalue,1);
# 3145
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp531);}
# 3147
goto _LL2BE;_LL2BE:;}
# 3149
goto _LL210;_LL233: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp532=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp532->tag != 15)goto _LL235;else{_tmp533=_tmp532->f1;_tmp534=_tmp532->f2;}}_LL234:
# 3152
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCBA;void*_tmpCB9[1];struct Cyc_String_pa_PrintArg_struct _tmpCB8;(_tmpCB8.tag=0,((_tmpCB8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp534->loc)),((_tmpCB9[0]=& _tmpCB8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBA="%s: new at top-level",_tag_dyneither(_tmpCBA,sizeof(char),21))),_tag_dyneither(_tmpCB9,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp534->topt);
{void*_tmp644=_tmp534->r;void*_tmp645=_tmp644;struct Cyc_List_List*_tmp647;struct Cyc_Absyn_Vardecl*_tmp649;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp64B;int _tmp64C;struct Cyc_Absyn_Exp*_tmp64E;void*_tmp64F;int _tmp650;struct _tuple1*_tmp652;struct Cyc_List_List*_tmp653;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Aggrdecl*_tmp655;struct Cyc_List_List*_tmp657;_LL2C6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp646=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp645;if(_tmp646->tag != 25)goto _LL2C8;else{_tmp647=_tmp646->f1;}}_LL2C7: {
# 3160
struct _tuple1*_tmp658=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp659=Cyc_Absyn_var_exp(_tmp658,0);
struct Cyc_Absyn_Stmt*_tmp65A=Cyc_Toc_init_array(nv,new_e_type,_tmp659,_tmp647,Cyc_Absyn_exp_stmt(_tmp659,0));
void*old_elt_typ;
{void*_tmp65B=Cyc_Tcutil_compress(old_typ);void*_tmp65C=_tmp65B;void*_tmp65E;struct Cyc_Absyn_Tqual _tmp65F;union Cyc_Absyn_Constraint*_tmp660;_LL2D3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp65D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C;if(_tmp65D->tag != 5)goto _LL2D5;else{_tmp65E=(_tmp65D->f1).elt_typ;_tmp65F=(_tmp65D->f1).elt_tq;_tmp660=((_tmp65D->f1).ptr_atts).zero_term;}}_LL2D4:
# 3166
 old_elt_typ=_tmp65E;goto _LL2D2;_LL2D5:;_LL2D6: {
# 3168
const char*_tmpCBD;void*_tmpCBC;old_elt_typ=(
(_tmpCBC=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBD="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCBD,sizeof(char),52))),_tag_dyneither(_tmpCBC,sizeof(void*),0))));}_LL2D2:;}{
# 3171
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp663=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp664=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp647),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp533 == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp664);else{
# 3179
struct Cyc_Absyn_Exp*r=_tmp533;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp664);}
# 3184
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp658,_tmp663,e1,_tmp65A,0));
goto _LL2C5;};}_LL2C8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp648=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp645;if(_tmp648->tag != 26)goto _LL2CA;else{_tmp649=_tmp648->f1;_tmp64A=_tmp648->f2;_tmp64B=_tmp648->f3;_tmp64C=_tmp648->f4;}}_LL2C9:
# 3188
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp533,old_typ,_tmp64A,(void*)_check_null(_tmp64B->topt),_tmp64C,_tmp64B,_tmp649);
goto _LL2C5;_LL2CA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp64D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp645;if(_tmp64D->tag != 27)goto _LL2CC;else{_tmp64E=_tmp64D->f1;_tmp64F=(void*)_tmp64D->f2;_tmp650=_tmp64D->f3;}}_LL2CB:
# 3192
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp533,old_typ,_tmp64E,_tmp64F,_tmp650,0,0);
goto _LL2C5;_LL2CC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp651=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp645;if(_tmp651->tag != 28)goto _LL2CE;else{_tmp652=_tmp651->f1;_tmp653=_tmp651->f2;_tmp654=_tmp651->f3;_tmp655=_tmp651->f4;}}_LL2CD: {
# 3197
struct Cyc_Absyn_Exp*_tmp665=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp534->topt),_tmp653,1,_tmp533,_tmp654,_tmp652);
e->r=_tmp665->r;
e->topt=_tmp665->topt;
goto _LL2C5;}_LL2CE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp656=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp645;if(_tmp656->tag != 23)goto _LL2D0;else{_tmp657=_tmp656->f1;}}_LL2CF:
# 3203
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp533,_tmp657))->r;
goto _LL2C5;_LL2D0:;_LL2D1: {
# 3210
void*old_elt_typ=(void*)_check_null(_tmp534->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3213
struct _tuple1*_tmp666=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp667=Cyc_Absyn_var_exp(_tmp666,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp667,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp533 == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3220
struct Cyc_Absyn_Exp*r=_tmp533;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3228
int done=0;
{void*_tmp668=_tmp534->r;void*_tmp669=_tmp668;void*_tmp66B;struct Cyc_Absyn_Exp*_tmp66C;_LL2D8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp66A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp669;if(_tmp66A->tag != 13)goto _LL2DA;else{_tmp66B=(void*)_tmp66A->f1;_tmp66C=_tmp66A->f2;}}_LL2D9:
# 3231
{struct _tuple29 _tmpCBE;struct _tuple29 _tmp66D=(_tmpCBE.f1=Cyc_Tcutil_compress(_tmp66B),((_tmpCBE.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp66C->topt)),_tmpCBE)));struct _tuple29 _tmp66E=_tmp66D;void*_tmp670;union Cyc_Absyn_Constraint*_tmp671;union Cyc_Absyn_Constraint*_tmp673;_LL2DD:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66E.f1;if(_tmp66F->tag != 5)goto _LL2DF;else{_tmp670=(_tmp66F->f1).elt_typ;_tmp671=((_tmp66F->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp672=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp66E.f2;if(_tmp672->tag != 5)goto _LL2DF;else{_tmp673=((_tmp672->f1).ptr_atts).bounds;}};_LL2DE:
# 3234
{struct _tuple29 _tmpCBF;struct _tuple29 _tmp674=(_tmpCBF.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp671),((_tmpCBF.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp673),_tmpCBF)));struct _tuple29 _tmp675=_tmp674;struct Cyc_Absyn_Exp*_tmp678;_LL2E2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp676=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp675.f1;if(_tmp676->tag != 0)goto _LL2E4;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp677=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp675.f2;if(_tmp677->tag != 1)goto _LL2E4;else{_tmp678=_tmp677->f1;}};_LL2E3:
# 3236
 Cyc_Toc_exp_to_c(nv,_tmp66C);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp679=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCC0[4];e->r=Cyc_Toc_fncall_exp_r(_tmp679,(
(_tmpCC0[3]=_tmp678,((_tmpCC0[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp670),0),((_tmpCC0[1]=_tmp66C,((_tmpCC0[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCC0,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3244
goto _LL2E1;};_LL2E4:;_LL2E5:
 goto _LL2E1;_LL2E1:;}
# 3247
goto _LL2DC;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3250
goto _LL2D7;_LL2DA:;_LL2DB:
 goto _LL2D7;_LL2D7:;}
# 3253
if(!done){
struct Cyc_Absyn_Stmt*_tmp67D=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp667),0);
struct Cyc_Absyn_Exp*_tmp67E=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp534);
_tmp67D=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp667,_tmp67E,0),_tmp534,0),_tmp67D,0);{
# 3259
void*_tmp67F=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp666,_tmp67F,mexp,_tmp67D,0));};}
# 3262
goto _LL2C5;};}_LL2C5:;}
# 3264
goto _LL210;};_LL235: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp535=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp535->tag != 17)goto _LL237;else{_tmp536=_tmp535->f1;}}_LL236: {
# 3267
int _tmp680=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp536);
Cyc_Toc_set_in_sizeof(nv,_tmp680);
goto _LL210;}_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp537=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp537->tag != 16)goto _LL239;else{_tmp538=(void*)_tmp537->f1;}}_LL238:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCC3;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCC2;e->r=(void*)((_tmpCC2=_cycalloc(sizeof(*_tmpCC2)),((_tmpCC2[0]=((_tmpCC3.tag=16,((_tmpCC3.f1=Cyc_Toc_typ_to_c(_tmp538),_tmpCC3)))),_tmpCC2))));}goto _LL210;_LL239: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp539=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp539->tag != 18)goto _LL23B;else{_tmp53A=(void*)_tmp539->f1;_tmp53B=_tmp539->f2;}}_LL23A: {
# 3273
void*_tmp683=_tmp53A;
struct Cyc_List_List*_tmp684=_tmp53B;
for(0;_tmp684 != 0;_tmp684=_tmp684->tl){
void*_tmp685=(void*)_tmp684->hd;void*_tmp686=_tmp685;struct _dyneither_ptr*_tmp688;unsigned int _tmp68A;_LL2E7: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp687=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp686;if(_tmp687->tag != 0)goto _LL2E9;else{_tmp688=_tmp687->f1;}}_LL2E8:
 goto _LL2E6;_LL2E9: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp689=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp686;if(_tmp689->tag != 1)goto _LL2E6;else{_tmp68A=_tmp689->f1;}}_LL2EA:
# 3279
{void*_tmp68B=Cyc_Tcutil_compress(_tmp683);void*_tmp68C=_tmp68B;union Cyc_Absyn_AggrInfoU _tmp68E;struct Cyc_List_List*_tmp690;struct Cyc_List_List*_tmp692;struct Cyc_Absyn_Datatypefield*_tmp694;_LL2EC: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp68D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp68C;if(_tmp68D->tag != 11)goto _LL2EE;else{_tmp68E=(_tmp68D->f1).aggr_info;}}_LL2ED: {
# 3281
struct Cyc_Absyn_Aggrdecl*_tmp695=Cyc_Absyn_get_known_aggrdecl(_tmp68E);
if(_tmp695->impl == 0){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC6="struct fields must be known",_tag_dyneither(_tmpCC6,sizeof(char),28))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}
_tmp690=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp695->impl))->fields;goto _LL2EF;}_LL2EE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp68F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68C;if(_tmp68F->tag != 12)goto _LL2F0;else{_tmp690=_tmp68F->f2;}}_LL2EF: {
# 3286
struct Cyc_Absyn_Aggrfield*_tmp698=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp690,(int)_tmp68A);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCC9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCC8;_tmp684->hd=(void*)((void*)((_tmpCC8=_cycalloc(sizeof(*_tmpCC8)),((_tmpCC8[0]=((_tmpCC9.tag=0,((_tmpCC9.f1=_tmp698->name,_tmpCC9)))),_tmpCC8)))));}
_tmp683=_tmp698->type;
goto _LL2EB;}_LL2F0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp691=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68C;if(_tmp691->tag != 10)goto _LL2F2;else{_tmp692=_tmp691->f1;}}_LL2F1:
# 3291
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCCC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCCB;_tmp684->hd=(void*)((void*)((_tmpCCB=_cycalloc(sizeof(*_tmpCCB)),((_tmpCCB[0]=((_tmpCCC.tag=0,((_tmpCCC.f1=Cyc_Absyn_fieldname((int)(_tmp68A + 1)),_tmpCCC)))),_tmpCCB)))));}
_tmp683=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp692,(int)_tmp68A)).f2;
goto _LL2EB;_LL2F2: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp693=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp68C;if(_tmp693->tag != 4)goto _LL2F4;else{if((((_tmp693->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F4;_tmp694=((struct _tuple2)(((_tmp693->f1).field_info).KnownDatatypefield).val).f2;}}_LL2F3:
# 3295
 if(_tmp68A == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCCF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCCE;_tmp684->hd=(void*)((void*)((_tmpCCE=_cycalloc(sizeof(*_tmpCCE)),((_tmpCCE[0]=((_tmpCCF.tag=0,((_tmpCCF.f1=Cyc_Toc_tag_sp,_tmpCCF)))),_tmpCCE)))));}else{
# 3298
_tmp683=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp694->typs,(int)(_tmp68A - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCD2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCD1;_tmp684->hd=(void*)((void*)((_tmpCD1=_cycalloc(sizeof(*_tmpCD1)),((_tmpCD1[0]=((_tmpCD2.tag=0,((_tmpCD2.f1=Cyc_Absyn_fieldname((int)_tmp68A),_tmpCD2)))),_tmpCD1)))));};}
# 3301
goto _LL2EB;_LL2F4:;_LL2F5:
{const char*_tmpCD5;void*_tmpCD4;(_tmpCD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD5="impossible type for offsetof tuple index",_tag_dyneither(_tmpCD5,sizeof(char),41))),_tag_dyneither(_tmpCD4,sizeof(void*),0)));}
goto _LL2EB;_LL2EB:;}
# 3305
goto _LL2E6;_LL2E6:;}
# 3308
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCD8;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCD7;e->r=(void*)((_tmpCD7=_cycalloc(sizeof(*_tmpCD7)),((_tmpCD7[0]=((_tmpCD8.tag=18,((_tmpCD8.f1=Cyc_Toc_typ_to_c(_tmp53A),((_tmpCD8.f2=_tmp53B,_tmpCD8)))))),_tmpCD7))));}
goto _LL210;}_LL23B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp53C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp53C->tag != 19)goto _LL23D;else{_tmp53D=_tmp53C->f1;}}_LL23C: {
# 3311
int _tmp6A5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6A6=Cyc_Tcutil_compress((void*)_check_null(_tmp53D->topt));
{void*_tmp6A7=_tmp6A6;void*_tmp6A9;struct Cyc_Absyn_Tqual _tmp6AA;void*_tmp6AB;union Cyc_Absyn_Constraint*_tmp6AC;union Cyc_Absyn_Constraint*_tmp6AD;union Cyc_Absyn_Constraint*_tmp6AE;_LL2F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A7;if(_tmp6A8->tag != 5)goto _LL2F9;else{_tmp6A9=(_tmp6A8->f1).elt_typ;_tmp6AA=(_tmp6A8->f1).elt_tq;_tmp6AB=((_tmp6A8->f1).ptr_atts).rgn;_tmp6AC=((_tmp6A8->f1).ptr_atts).nullable;_tmp6AD=((_tmp6A8->f1).ptr_atts).bounds;_tmp6AE=((_tmp6A8->f1).ptr_atts).zero_term;}}_LL2F8:
# 3316
{void*_tmp6AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6AD);void*_tmp6B0=_tmp6AF;struct Cyc_Absyn_Exp*_tmp6B2;_LL2FC: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6B1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6B0;if(_tmp6B1->tag != 1)goto _LL2FE;else{_tmp6B2=_tmp6B1->f1;}}_LL2FD: {
# 3318
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp53D);
Cyc_Toc_exp_to_c(nv,_tmp53D);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCDB;void*_tmpCDA;(_tmpCDA=0,Cyc_Tcutil_warn(e->loc,((_tmpCDB="inserted null check due to dereference",_tag_dyneither(_tmpCDB,sizeof(char),39))),_tag_dyneither(_tmpCDA,sizeof(void*),0)));}{
# 3324
struct Cyc_List_List*_tmpCDC;_tmp53D->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6A6),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCDC=_cycalloc(sizeof(*_tmpCDC)),((_tmpCDC->hd=Cyc_Absyn_copy_exp(_tmp53D),((_tmpCDC->tl=0,_tmpCDC)))))),0));};}
# 3332
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6AE)){
struct _tuple12 _tmp6B7=Cyc_Evexp_eval_const_uint_exp(_tmp6B2);unsigned int _tmp6B9;int _tmp6BA;struct _tuple12 _tmp6B8=_tmp6B7;_tmp6B9=_tmp6B8.f1;_tmp6BA=_tmp6B8.f2;
# 3338
if(!_tmp6BA  || _tmp6B9 <= 0){
const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,Cyc_Tcutil_terr(e->loc,((_tmpCDF="cannot determine dereference is in bounds",_tag_dyneither(_tmpCDF,sizeof(char),42))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}}
# 3341
goto _LL2FB;}_LL2FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6B3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6B0;if(_tmp6B3->tag != 0)goto _LL2FB;}_LL2FF: {
# 3344
struct Cyc_Absyn_Exp*_tmp6BD=Cyc_Absyn_uint_exp(0,0);
_tmp6BD->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp53D,_tmp6BD);
Cyc_Toc_exp_to_c(nv,e);
goto _LL2FB;}_LL2FB:;}
# 3350
goto _LL2F6;_LL2F9:;_LL2FA: {
const char*_tmpCE2;void*_tmpCE1;(_tmpCE1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE2="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCE2,sizeof(char),29))),_tag_dyneither(_tmpCE1,sizeof(void*),0)));}_LL2F6:;}
# 3353
Cyc_Toc_set_lhs(nv,_tmp6A5);
goto _LL210;};}_LL23D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp53E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp53E->tag != 20)goto _LL23F;else{_tmp53F=_tmp53E->f1;_tmp540=_tmp53E->f2;_tmp541=_tmp53E->f3;_tmp542=_tmp53E->f4;}}_LL23E: {
# 3356
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp53F->topt);
Cyc_Toc_exp_to_c(nv,_tmp53F);
if(_tmp541  && _tmp542)
e->r=Cyc_Toc_check_tagged_union(_tmp53F,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp540,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3364
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL210;}_LL23F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp543=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp543->tag != 21)goto _LL241;else{_tmp544=_tmp543->f1;_tmp545=_tmp543->f2;_tmp546=_tmp543->f3;_tmp547=_tmp543->f4;}}_LL240: {
# 3368
int _tmp6C0=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp544->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp544);
Cyc_Toc_exp_to_c(nv,_tmp544);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6C1=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6C3;struct Cyc_Absyn_Tqual _tmp6C4;void*_tmp6C5;union Cyc_Absyn_Constraint*_tmp6C6;union Cyc_Absyn_Constraint*_tmp6C7;union Cyc_Absyn_Constraint*_tmp6C8;struct Cyc_Absyn_PtrInfo _tmp6C2=_tmp6C1;_tmp6C3=_tmp6C2.elt_typ;_tmp6C4=_tmp6C2.elt_tq;_tmp6C5=(_tmp6C2.ptr_atts).rgn;_tmp6C6=(_tmp6C2.ptr_atts).nullable;_tmp6C7=(_tmp6C2.ptr_atts).bounds;_tmp6C8=(_tmp6C2.ptr_atts).zero_term;
{void*_tmp6C9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6C7);void*_tmp6CA=_tmp6C9;struct Cyc_Absyn_Exp*_tmp6CC;_LL301: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6CB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6CA;if(_tmp6CB->tag != 1)goto _LL303;else{_tmp6CC=_tmp6CB->f1;}}_LL302: {
# 3379
struct _tuple12 _tmp6CE=Cyc_Evexp_eval_const_uint_exp(_tmp6CC);unsigned int _tmp6D0;int _tmp6D1;struct _tuple12 _tmp6CF=_tmp6CE;_tmp6D0=_tmp6CF.f1;_tmp6D1=_tmp6CF.f2;
if(_tmp6D1){
if(_tmp6D0 < 1){
const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCE5="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCE5,sizeof(char),44))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCE8;void*_tmpCE7;(_tmpCE7=0,Cyc_Tcutil_warn(e->loc,((_tmpCE8="inserted null check due to dereference",_tag_dyneither(_tmpCE8,sizeof(char),39))),_tag_dyneither(_tmpCE7,sizeof(void*),0)));}{
# 3387
struct Cyc_Absyn_Exp*_tmpCE9[1];_tmp544->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCE9[0]=Cyc_Absyn_new_exp(_tmp544->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE9,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3392
if(!Cyc_Evexp_c_can_eval(_tmp6CC)){
const char*_tmpCEC;void*_tmpCEB;(_tmpCEB=0,Cyc_Tcutil_terr(e->loc,((_tmpCEC="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCEC,sizeof(char),47))),_tag_dyneither(_tmpCEB,sizeof(void*),0)));}{
# 3396
struct Cyc_Absyn_Exp*_tmpCED[4];_tmp544->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCED[3]=
# 3400
Cyc_Absyn_uint_exp(0,0),((_tmpCED[2]=
# 3399
Cyc_Absyn_sizeoftyp_exp(_tmp6C3,0),((_tmpCED[1]=_tmp6CC,((_tmpCED[0]=
# 3398
Cyc_Absyn_new_exp(_tmp544->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCED,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3402
goto _LL300;}_LL303: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6CD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6CA;if(_tmp6CD->tag != 0)goto _LL300;}_LL304: {
# 3405
void*ta1=Cyc_Toc_typ_to_c(_tmp6C3);
{struct Cyc_Absyn_Exp*_tmpCEE[3];_tmp544->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6C4),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpCEE[2]=
# 3410
Cyc_Absyn_uint_exp(0,0),((_tmpCEE[1]=
# 3409
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEE[0]=
# 3408
Cyc_Absyn_new_exp(_tmp544->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3413
goto _LL300;}_LL300:;}
# 3415
if(_tmp546  && _tmp547)
e->r=Cyc_Toc_check_tagged_union(_tmp544,Cyc_Toc_typ_to_c(e1typ),_tmp6C3,_tmp545,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3418
if(is_poly  && _tmp547)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6C0);
goto _LL210;};};}_LL241: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp548=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp548->tag != 22)goto _LL243;else{_tmp549=_tmp548->f1;_tmp54A=_tmp548->f2;}}_LL242: {
# 3423
int _tmp6DB=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6DC=Cyc_Tcutil_compress((void*)_check_null(_tmp549->topt));
# 3427
{void*_tmp6DD=_tmp6DC;struct Cyc_List_List*_tmp6DF;void*_tmp6E1;struct Cyc_Absyn_Tqual _tmp6E2;void*_tmp6E3;union Cyc_Absyn_Constraint*_tmp6E4;union Cyc_Absyn_Constraint*_tmp6E5;union Cyc_Absyn_Constraint*_tmp6E6;_LL306: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6DE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6DD;if(_tmp6DE->tag != 10)goto _LL308;else{_tmp6DF=_tmp6DE->f1;}}_LL307:
# 3430
 Cyc_Toc_exp_to_c(nv,_tmp549);
Cyc_Toc_exp_to_c(nv,_tmp54A);{
struct _tuple12 _tmp6E7=Cyc_Evexp_eval_const_uint_exp(_tmp54A);unsigned int _tmp6E9;int _tmp6EA;struct _tuple12 _tmp6E8=_tmp6E7;_tmp6E9=_tmp6E8.f1;_tmp6EA=_tmp6E8.f2;
if(!_tmp6EA){
const char*_tmpCF1;void*_tmpCF0;(_tmpCF0=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF1="unknown tuple subscript in translation to C",_tag_dyneither(_tmpCF1,sizeof(char),44))),_tag_dyneither(_tmpCF0,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp549,Cyc_Absyn_fieldname((int)(_tmp6E9 + 1)));
goto _LL305;};_LL308: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6E0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DD;if(_tmp6E0->tag != 5)goto _LL30A;else{_tmp6E1=(_tmp6E0->f1).elt_typ;_tmp6E2=(_tmp6E0->f1).elt_tq;_tmp6E3=((_tmp6E0->f1).ptr_atts).rgn;_tmp6E4=((_tmp6E0->f1).ptr_atts).nullable;_tmp6E5=((_tmp6E0->f1).ptr_atts).bounds;_tmp6E6=((_tmp6E0->f1).ptr_atts).zero_term;}}_LL309: {
# 3438
struct Cyc_List_List*_tmp6ED=Cyc_Toc_get_relns(_tmp549);
int _tmp6EE=Cyc_Toc_need_null_check(_tmp549);
int _tmp6EF=Cyc_Toc_in_sizeof(nv);
# 3446
int in_bnds=_tmp6EF;
{void*_tmp6F0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6E5);void*_tmp6F1=_tmp6F0;_LL30D:;_LL30E:
# 3449
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6DC,_tmp6ED,_tmp549,_tmp54A);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpCF5;void*_tmpCF4[1];struct Cyc_String_pa_PrintArg_struct _tmpCF3;(_tmpCF3.tag=0,((_tmpCF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCF4[0]=& _tmpCF3,Cyc_Tcutil_warn(e->loc,((_tmpCF5="bounds check necessary for %s",_tag_dyneither(_tmpCF5,sizeof(char),30))),_tag_dyneither(_tmpCF4,sizeof(void*),1)))))));}_LL30C:;}
# 3456
Cyc_Toc_exp_to_c(nv,_tmp549);
Cyc_Toc_exp_to_c(nv,_tmp54A);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6F5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6E5);void*_tmp6F6=_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F8;_LL310: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6F7=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6F6;if(_tmp6F7->tag != 1)goto _LL312;else{_tmp6F8=_tmp6F7->f1;}}_LL311: {
# 3461
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6E4) && _tmp6EE;
void*ta1=Cyc_Toc_typ_to_c(_tmp6E1);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6E2);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,Cyc_Tcutil_warn(e->loc,((_tmpCF8="inserted null check due to dereference",_tag_dyneither(_tmpCF8,sizeof(char),39))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCF9[1];_tmp549->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCF9[0]=Cyc_Absyn_copy_exp(_tmp549),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3473
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6E6)){
# 3475
if(!Cyc_Evexp_c_can_eval(_tmp6F8)){
const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,Cyc_Tcutil_terr(e->loc,((_tmpCFC="cannot determine subscript is in bounds",_tag_dyneither(_tmpCFC,sizeof(char),40))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp549);
struct Cyc_Absyn_Exp*_tmpCFD[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpCFD[2]=_tmp54A,((_tmpCFD[1]=_tmp6F8,((_tmpCFD[0]=_tmp549,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFD,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6F8)){
const char*_tmpD00;void*_tmpCFF;(_tmpCFF=0,Cyc_Tcutil_terr(e->loc,((_tmpD00="cannot determine subscript is in bounds",_tag_dyneither(_tmpD00,sizeof(char),40))),_tag_dyneither(_tmpCFF,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD03;void*_tmpD02;(_tmpD02=0,Cyc_Tcutil_warn(e->loc,((_tmpD03="inserted null check due to dereference",_tag_dyneither(_tmpD03,sizeof(char),39))),_tag_dyneither(_tmpD02,sizeof(void*),0)));}{
# 3487
struct Cyc_Absyn_Exp*_tmpD04[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD04[3]=_tmp54A,((_tmpD04[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD04[1]=_tmp6F8,((_tmpD04[0]=_tmp549,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD04,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3493
if(!Cyc_Evexp_c_can_eval(_tmp6F8)){
const char*_tmpD07;void*_tmpD06;(_tmpD06=0,Cyc_Tcutil_terr(e->loc,((_tmpD07="cannot determine subscript is in bounds",_tag_dyneither(_tmpD07,sizeof(char),40))),_tag_dyneither(_tmpD06,sizeof(void*),0)));}{
# 3496
struct Cyc_Absyn_Exp*_tmpD08[2];_tmp54A->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpD08[1]=Cyc_Absyn_copy_exp(_tmp54A),((_tmpD08[0]=_tmp6F8,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD08,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3499
goto _LL30F;}_LL312: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6F6;if(_tmp6F9->tag != 0)goto _LL30F;}_LL313: {
# 3501
void*ta1=Cyc_Toc_typ_to_c(_tmp6E1);
if(in_bnds){
# 3505
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6E2),
Cyc_Toc_member_exp(_tmp549,Cyc_Toc_curr_sp,0)),_tmp54A);}else{
# 3510
struct Cyc_Absyn_Exp*_tmp708=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD09[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6E2),
Cyc_Absyn_fncall_exp(_tmp708,(
(_tmpD09[2]=_tmp54A,((_tmpD09[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD09[0]=_tmp549,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD09,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3516
goto _LL30F;}_LL30F:;}
# 3518
goto _LL305;}_LL30A:;_LL30B: {
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD0C="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD0C,sizeof(char),49))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}_LL305:;}
# 3521
Cyc_Toc_set_lhs(nv,_tmp6DB);
goto _LL210;};}_LL243: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp54B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp54B->tag != 23)goto _LL245;else{_tmp54C=_tmp54B->f1;}}_LL244:
# 3524
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp54C))->r;else{
# 3529
struct Cyc_List_List*_tmp70C=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp54C);
void*_tmp70D=Cyc_Toc_add_tuple_type(_tmp70C);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp54C != 0;(_tmp54C=_tmp54C->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp54C->hd);{
struct _tuple19*_tmpD0F;struct Cyc_List_List*_tmpD0E;dles=((_tmpD0E=_cycalloc(sizeof(*_tmpD0E)),((_tmpD0E->hd=((_tmpD0F=_cycalloc(sizeof(*_tmpD0F)),((_tmpD0F->f1=0,((_tmpD0F->f2=(struct Cyc_Absyn_Exp*)_tmp54C->hd,_tmpD0F)))))),((_tmpD0E->tl=dles,_tmpD0E))))));};}}
# 3536
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3539
goto _LL210;_LL245: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp54D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp54D->tag != 25)goto _LL247;else{_tmp54E=_tmp54D->f1;}}_LL246:
# 3543
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp54E);
{struct Cyc_List_List*_tmp710=_tmp54E;for(0;_tmp710 != 0;_tmp710=_tmp710->tl){
struct _tuple19*_tmp711=(struct _tuple19*)_tmp710->hd;struct Cyc_Absyn_Exp*_tmp713;struct _tuple19*_tmp712=_tmp711;_tmp713=_tmp712->f2;
Cyc_Toc_exp_to_c(nv,_tmp713);}}
# 3548
goto _LL210;_LL247: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp54F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp54F->tag != 26)goto _LL249;else{_tmp550=_tmp54F->f1;_tmp551=_tmp54F->f2;_tmp552=_tmp54F->f3;_tmp553=_tmp54F->f4;}}_LL248: {
# 3552
struct _tuple12 _tmp714=Cyc_Evexp_eval_const_uint_exp(_tmp551);unsigned int _tmp716;int _tmp717;struct _tuple12 _tmp715=_tmp714;_tmp716=_tmp715.f1;_tmp717=_tmp715.f2;{
void*_tmp718=Cyc_Toc_typ_to_c((void*)_check_null(_tmp552->topt));
Cyc_Toc_exp_to_c(nv,_tmp552);{
struct Cyc_List_List*es=0;
# 3557
if(!Cyc_Toc_is_zero(_tmp552)){
if(!_tmp717){
const char*_tmpD12;void*_tmpD11;(_tmpD11=0,Cyc_Tcutil_terr(_tmp551->loc,((_tmpD12="cannot determine value of constant",_tag_dyneither(_tmpD12,sizeof(char),35))),_tag_dyneither(_tmpD11,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp716;++ i){
struct _tuple19*_tmpD15;struct Cyc_List_List*_tmpD14;es=((_tmpD14=_cycalloc(sizeof(*_tmpD14)),((_tmpD14->hd=((_tmpD15=_cycalloc(sizeof(*_tmpD15)),((_tmpD15->f1=0,((_tmpD15->f2=_tmp552,_tmpD15)))))),((_tmpD14->tl=es,_tmpD14))))));}}
# 3563
if(_tmp553){
struct Cyc_Absyn_Exp*_tmp71D=Cyc_Toc_cast_it(_tmp718,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpD18;struct Cyc_List_List*_tmpD17;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD17=_cycalloc(sizeof(*_tmpD17)),((_tmpD17->hd=((_tmpD18=_cycalloc(sizeof(*_tmpD18)),((_tmpD18->f1=0,((_tmpD18->f2=_tmp71D,_tmpD18)))))),((_tmpD17->tl=0,_tmpD17)))))));}}
# 3568
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL210;};};}_LL249: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp554=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp554->tag != 27)goto _LL24B;else{_tmp555=_tmp554->f1;_tmp556=(void*)_tmp554->f2;_tmp557=_tmp554->f3;}}_LL24A:
# 3573
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL210;_LL24B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp558=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp558->tag != 28)goto _LL24D;else{_tmp559=_tmp558->f1;_tmp55A=_tmp558->f2;_tmp55B=_tmp558->f3;_tmp55C=_tmp558->f4;}}_LL24C:
# 3578
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp720=Cyc_Toc_init_struct(nv,old_typ,_tmp55A,0,0,_tmp55B,_tmp559);
e->r=_tmp720->r;
e->topt=_tmp720->topt;}else{
# 3589
if(_tmp55C == 0){
const char*_tmpD1B;void*_tmpD1A;(_tmpD1A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1B="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD1B,sizeof(char),38))),_tag_dyneither(_tmpD1A,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp55C;
# 3593
struct _RegionHandle _tmp723=_new_region("rgn");struct _RegionHandle*rgn=& _tmp723;_push_region(rgn);
{struct Cyc_List_List*_tmp724=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp55B,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3597
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp55C->impl))->tagged){
# 3599
struct _tuple30*_tmp725=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp724))->hd;struct Cyc_Absyn_Aggrfield*_tmp727;struct Cyc_Absyn_Exp*_tmp728;struct _tuple30*_tmp726=_tmp725;_tmp727=_tmp726->f1;_tmp728=_tmp726->f2;{
void*_tmp729=(void*)_check_null(_tmp728->topt);
void*_tmp72A=_tmp727->type;
Cyc_Toc_exp_to_c(nv,_tmp728);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp72A) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp729))
_tmp728->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp728->r,0));{
# 3608
int i=Cyc_Toc_get_member_offset(_tmp55C,_tmp727->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD20;struct _tuple19*_tmpD1F;struct _tuple19*_tmpD1E[2];struct Cyc_List_List*_tmp72B=(_tmpD1E[1]=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->f1=0,((_tmpD1F->f2=_tmp728,_tmpD1F)))))),((_tmpD1E[0]=((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->f1=0,((_tmpD20->f2=field_tag_exp,_tmpD20)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD1E,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp72B,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD26;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD25;void*_tmpD24[1];struct Cyc_List_List*ds=(_tmpD24[0]=(void*)((_tmpD26=_cycalloc(sizeof(*_tmpD26)),((_tmpD26[0]=((_tmpD25.tag=1,((_tmpD25.f1=_tmp727->name,_tmpD25)))),_tmpD26)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD24,sizeof(void*),1)));
struct _tuple19*_tmpD29;struct _tuple19*_tmpD28[1];struct Cyc_List_List*dles=(_tmpD28[0]=((_tmpD29=_cycalloc(sizeof(*_tmpD29)),((_tmpD29->f1=ds,((_tmpD29->f2=umem,_tmpD29)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD28,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3617
struct Cyc_List_List*_tmp734=0;
struct Cyc_List_List*_tmp735=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp735 != 0;_tmp735=_tmp735->tl){
struct Cyc_List_List*_tmp736=_tmp724;for(0;_tmp736 != 0;_tmp736=_tmp736->tl){
if((*((struct _tuple30*)_tmp736->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp735->hd){
struct _tuple30*_tmp737=(struct _tuple30*)_tmp736->hd;struct Cyc_Absyn_Aggrfield*_tmp739;struct Cyc_Absyn_Exp*_tmp73A;struct _tuple30*_tmp738=_tmp737;_tmp739=_tmp738->f1;_tmp73A=_tmp738->f2;{
void*_tmp73B=Cyc_Toc_typ_to_c(_tmp739->type);
void*_tmp73C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp73A->topt));
Cyc_Toc_exp_to_c(nv,_tmp73A);
# 3627
if(!Cyc_Tcutil_unify(_tmp73B,_tmp73C)){
# 3629
if(!Cyc_Tcutil_is_arithmetic_type(_tmp73B) || !
Cyc_Tcutil_is_arithmetic_type(_tmp73C))
_tmp73A=Cyc_Toc_cast_it(_tmp73B,Cyc_Absyn_copy_exp(_tmp73A));}
{struct _tuple19*_tmpD2C;struct Cyc_List_List*_tmpD2B;_tmp734=((_tmpD2B=_cycalloc(sizeof(*_tmpD2B)),((_tmpD2B->hd=((_tmpD2C=_cycalloc(sizeof(*_tmpD2C)),((_tmpD2C->f1=0,((_tmpD2C->f2=_tmp73A,_tmpD2C)))))),((_tmpD2B->tl=_tmp734,_tmpD2B))))));}
break;};}}}
# 3636
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp734));}}
# 3594
;_pop_region(rgn);};}
# 3640
goto _LL210;_LL24D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp55D=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp55D->tag != 29)goto _LL24F;else{_tmp55E=(void*)_tmp55D->f1;_tmp55F=_tmp55D->f2;}}_LL24E: {
# 3642
struct Cyc_List_List*fs;
{void*_tmp73F=Cyc_Tcutil_compress(_tmp55E);void*_tmp740=_tmp73F;struct Cyc_List_List*_tmp742;_LL315: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp741=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp740;if(_tmp741->tag != 12)goto _LL317;else{_tmp742=_tmp741->f2;}}_LL316:
 fs=_tmp742;goto _LL314;_LL317:;_LL318: {
const char*_tmpD30;void*_tmpD2F[1];struct Cyc_String_pa_PrintArg_struct _tmpD2E;(_tmpD2E.tag=0,((_tmpD2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp55E)),((_tmpD2F[0]=& _tmpD2E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD30="anon struct has type %s",_tag_dyneither(_tmpD30,sizeof(char),24))),_tag_dyneither(_tmpD2F,sizeof(void*),1)))))));}_LL314:;}{
# 3647
struct _RegionHandle _tmp746=_new_region("rgn");struct _RegionHandle*rgn=& _tmp746;_push_region(rgn);{
struct Cyc_List_List*_tmp747=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp55F,Cyc_Absyn_StructA,fs);
for(0;_tmp747 != 0;_tmp747=_tmp747->tl){
struct _tuple30*_tmp748=(struct _tuple30*)_tmp747->hd;struct Cyc_Absyn_Aggrfield*_tmp74A;struct Cyc_Absyn_Exp*_tmp74B;struct _tuple30*_tmp749=_tmp748;_tmp74A=_tmp749->f1;_tmp74B=_tmp749->f2;{
void*_tmp74C=(void*)_check_null(_tmp74B->topt);
void*_tmp74D=_tmp74A->type;
Cyc_Toc_exp_to_c(nv,_tmp74B);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp74D) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp74C))
_tmp74B->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp74B->r,0));};}
# 3661
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp55F);}
# 3663
_npop_handler(0);goto _LL210;
# 3647
;_pop_region(rgn);};}_LL24F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp560->tag != 30)goto _LL251;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;_tmp563=_tmp560->f3;}}_LL250: {
# 3666
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp74E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp74F=Cyc_Absyn_var_exp(_tmp74E,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp563->name,_tmp562->name));
tag_exp=_tmp562->is_extensible?Cyc_Absyn_var_exp(_tmp563->name,0):
 Cyc_Toc_datatype_tag(_tmp562,_tmp563->name);
mem_exp=_tmp74F;{
struct Cyc_List_List*_tmp750=_tmp563->typs;
# 3677
if(Cyc_Toc_is_toplevel(nv)){
# 3679
struct Cyc_List_List*dles=0;
for(0;_tmp561 != 0;(_tmp561=_tmp561->tl,_tmp750=_tmp750->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp561->hd;
void*_tmp751=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp750))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp751))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD33;struct Cyc_List_List*_tmpD32;dles=((_tmpD32=_cycalloc(sizeof(*_tmpD32)),((_tmpD32->hd=((_tmpD33=_cycalloc(sizeof(*_tmpD33)),((_tmpD33->f1=0,((_tmpD33->f2=cur_e,_tmpD33)))))),((_tmpD32->tl=dles,_tmpD32))))));};}
# 3690
{struct _tuple19*_tmpD36;struct Cyc_List_List*_tmpD35;dles=((_tmpD35=_cycalloc(sizeof(*_tmpD35)),((_tmpD35->hd=((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36->f1=0,((_tmpD36->f2=tag_exp,_tmpD36)))))),((_tmpD35->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD35))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3696
struct Cyc_List_List*_tmpD37;struct Cyc_List_List*_tmp756=
(_tmpD37=_cycalloc(sizeof(*_tmpD37)),((_tmpD37->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD37->tl=0,_tmpD37)))));
# 3699
{int i=1;for(0;_tmp561 != 0;(((_tmp561=_tmp561->tl,i ++)),_tmp750=_tmp750->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp561->hd;
void*_tmp757=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp750))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp757))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp758=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3709
struct Cyc_List_List*_tmpD38;_tmp756=((_tmpD38=_cycalloc(sizeof(*_tmpD38)),((_tmpD38->hd=_tmp758,((_tmpD38->tl=_tmp756,_tmpD38))))));};}}{
# 3711
struct Cyc_Absyn_Stmt*_tmp75A=Cyc_Absyn_exp_stmt(_tmp74F,0);
struct Cyc_List_List*_tmpD39;struct Cyc_Absyn_Stmt*_tmp75B=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->hd=_tmp75A,((_tmpD39->tl=_tmp756,_tmpD39))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp74E,datatype_ctype,0,_tmp75B,0));};}
# 3715
goto _LL210;};}_LL251: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp564=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp564->tag != 31)goto _LL253;}_LL252:
# 3717
 goto _LL254;_LL253: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp565=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp565->tag != 32)goto _LL255;}_LL254:
 goto _LL210;_LL255: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp566=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp566->tag != 33)goto _LL257;else{_tmp567=(_tmp566->f1).is_calloc;_tmp568=(_tmp566->f1).rgn;_tmp569=(_tmp566->f1).elt_type;_tmp56A=(_tmp566->f1).num_elts;_tmp56B=(_tmp566->f1).fat_result;}}_LL256: {
# 3721
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp569)));
Cyc_Toc_exp_to_c(nv,_tmp56A);
# 3725
if(_tmp56B){
struct _tuple1*_tmp75E=Cyc_Toc_temp_var();
struct _tuple1*_tmp75F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp567){
xexp=_tmp56A;
if(_tmp568 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp568;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp75E,0));}else{
# 3736
pexp=Cyc_Toc_calloc_exp(*_tmp569,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp75E,0));}{
# 3738
struct Cyc_Absyn_Exp*_tmpD3A[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD3A[2]=
Cyc_Absyn_var_exp(_tmp75E,0),((_tmpD3A[1]=
# 3739
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD3A[0]=Cyc_Absyn_var_exp(_tmp75F,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3742
if(_tmp568 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp568;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp75E,0));}else{
# 3747
pexp=Cyc_Toc_malloc_exp(*_tmp569,Cyc_Absyn_var_exp(_tmp75E,0));}{
# 3749
struct Cyc_Absyn_Exp*_tmpD3B[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD3B[2]=
Cyc_Absyn_var_exp(_tmp75E,0),((_tmpD3B[1]=
# 3749
Cyc_Absyn_uint_exp(1,0),((_tmpD3B[0]=Cyc_Absyn_var_exp(_tmp75F,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3752
struct Cyc_Absyn_Stmt*_tmp762=Cyc_Absyn_declare_stmt(_tmp75E,Cyc_Absyn_uint_typ,_tmp56A,
Cyc_Absyn_declare_stmt(_tmp75F,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp762);};}else{
# 3757
if(_tmp567){
if(_tmp568 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp568;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56A))->r;}else{
# 3763
e->r=(Cyc_Toc_calloc_exp(*_tmp569,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp56A))->r;}}else{
# 3766
if(_tmp568 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp568;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp56A))->r;}else{
# 3771
e->r=(Cyc_Toc_malloc_exp(*_tmp569,_tmp56A))->r;}}}
# 3775
goto _LL210;}_LL257: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp56C=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp56C->tag != 34)goto _LL259;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;}}_LL258: {
# 3784
void*e1_old_typ=(void*)_check_null(_tmp56D->topt);
void*e2_old_typ=(void*)_check_null(_tmp56E->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD3E;void*_tmpD3D;(_tmpD3D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD3E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD3E,sizeof(char),57))),_tag_dyneither(_tmpD3D,sizeof(void*),0)));}{
# 3790
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3794
swap_fn=Cyc_Toc__swap_word_e;}{
# 3798
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp56D,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp56E,& f2_tag,& tagged_mem_type2,1);
# 3805
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp56D);
Cyc_Toc_exp_to_c(nv,_tmp56E);
Cyc_Toc_set_lhs(nv,0);
# 3811
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp56D,f1_tag,tagged_mem_type1);else{
# 3815
_tmp56D=Cyc_Toc_push_address_exp(_tmp56D);}
# 3817
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp56E,f2_tag,tagged_mem_type2);else{
# 3821
_tmp56E=Cyc_Toc_push_address_exp(_tmp56E);}
# 3823
{struct Cyc_Absyn_Exp*_tmpD3F[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD3F[1]=_tmp56E,((_tmpD3F[0]=_tmp56D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD3F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3825
goto _LL210;};};}_LL259: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp56F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp56F->tag != 37)goto _LL25B;else{_tmp570=_tmp56F->f1;_tmp571=_tmp56F->f2;}}_LL25A: {
# 3828
void*_tmp766=Cyc_Tcutil_compress((void*)_check_null(_tmp570->topt));
Cyc_Toc_exp_to_c(nv,_tmp570);
{void*_tmp767=_tmp766;struct Cyc_Absyn_Aggrdecl*_tmp769;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp768=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp767;if(_tmp768->tag != 11)goto _LL31C;else{if((((_tmp768->f1).aggr_info).KnownAggr).tag != 2)goto _LL31C;_tmp769=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp768->f1).aggr_info).KnownAggr).val);}}_LL31B: {
# 3832
struct Cyc_Absyn_Exp*_tmp76A=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp769,_tmp571),0);
struct Cyc_Absyn_Exp*_tmp76B=Cyc_Toc_member_exp(_tmp570,_tmp571,0);
struct Cyc_Absyn_Exp*_tmp76C=Cyc_Toc_member_exp(_tmp76B,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp76C,_tmp76A,0))->r;
goto _LL319;}_LL31C:;_LL31D: {
const char*_tmpD42;void*_tmpD41;(_tmpD41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD42="non-aggregate type in tagcheck",_tag_dyneither(_tmpD42,sizeof(char),31))),_tag_dyneither(_tmpD41,sizeof(void*),0)));}_LL319:;}
# 3839
goto _LL210;}_LL25B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp572=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp572->tag != 36)goto _LL25D;else{_tmp573=_tmp572->f1;}}_LL25C:
 Cyc_Toc_stmt_to_c(nv,_tmp573);goto _LL210;_LL25D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp574->tag != 35)goto _LL25F;}_LL25E: {
const char*_tmpD45;void*_tmpD44;(_tmpD44=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD45="UnresolvedMem",_tag_dyneither(_tmpD45,sizeof(char),14))),_tag_dyneither(_tmpD44,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp575=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp575->tag != 24)goto _LL261;}_LL260: {
const char*_tmpD48;void*_tmpD47;(_tmpD47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD48="compoundlit",_tag_dyneither(_tmpD48,sizeof(char),12))),_tag_dyneither(_tmpD47,sizeof(void*),0)));}_LL261: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp576=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp576->tag != 38)goto _LL263;}_LL262: {
const char*_tmpD4B;void*_tmpD4A;(_tmpD4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4B="valueof(-)",_tag_dyneither(_tmpD4B,sizeof(char),11))),_tag_dyneither(_tmpD4A,sizeof(void*),0)));}_LL263: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp577=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4FE;if(_tmp577->tag != 39)goto _LL210;}_LL264: {
const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4E="__asm__",_tag_dyneither(_tmpD4E,sizeof(char),8))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}_LL210:;};}
# 3876 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3879
Cyc_Toc_skip_stmt_dl(),0);}
# 3882
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp777=e->r;void*_tmp778=_tmp777;struct Cyc_Absyn_Exp*_tmp77A;struct Cyc_Absyn_Exp*_tmp77C;struct Cyc_Absyn_Exp*_tmp77E;_LL31F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp779=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp779->tag != 13)goto _LL321;else{_tmp77A=_tmp779->f2;}}_LL320:
 return Cyc_Toc_path_length(_tmp77A);_LL321: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp77B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp77B->tag != 19)goto _LL323;else{_tmp77C=_tmp77B->f1;}}_LL322:
 return 1 + Cyc_Toc_path_length(_tmp77C);_LL323: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp77D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp778;if(_tmp77D->tag != 20)goto _LL325;else{_tmp77E=_tmp77D->f1;}}_LL324:
 return Cyc_Toc_path_length(_tmp77E);_LL325:;_LL326:
 return 0;_LL31E:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3891
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3896
struct Cyc_Absyn_Stmt*s;
{void*_tmp77F=p->r;void*_tmp780=_tmp77F;struct _tuple1*_tmp782;struct _tuple1*_tmp784;struct Cyc_Absyn_Pat*_tmp785;struct _tuple1*_tmp787;struct Cyc_Absyn_Vardecl*_tmp78A;struct Cyc_Absyn_Pat*_tmp78B;enum Cyc_Absyn_Sign _tmp78E;int _tmp78F;char _tmp791;struct _dyneither_ptr _tmp793;int _tmp794;struct Cyc_Absyn_Enumdecl*_tmp796;struct Cyc_Absyn_Enumfield*_tmp797;void*_tmp799;struct Cyc_Absyn_Enumfield*_tmp79A;struct Cyc_Absyn_Datatypedecl*_tmp79D;struct Cyc_Absyn_Datatypefield*_tmp79E;struct Cyc_List_List*_tmp79F;struct Cyc_List_List*_tmp7A1;struct Cyc_List_List*_tmp7A3;union Cyc_Absyn_AggrInfoU _tmp7A6;struct Cyc_List_List*_tmp7A7;struct Cyc_Absyn_Pat*_tmp7A9;_LL328: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp781=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp781->tag != 2)goto _LL32A;else{_tmp782=(_tmp781->f2)->name;}}_LL329: {
# 3899
struct Cyc_Absyn_Pat*_tmp7AD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7AD->topt=p->topt;
_tmp784=_tmp782;_tmp785=_tmp7AD;goto _LL32B;}_LL32A: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp783=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp783->tag != 1)goto _LL32C;else{_tmp784=(_tmp783->f1)->name;_tmp785=_tmp783->f2;}}_LL32B: {
# 3904
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7AE=(void*)_check_null(_tmp785->topt);
{struct _tuple32*_tmpD51;struct Cyc_List_List*_tmpD50;decls=((_tmpD50=_region_malloc(rgn,sizeof(*_tmpD50)),((_tmpD50->hd=((_tmpD51=_region_malloc(rgn,sizeof(*_tmpD51)),((_tmpD51->f1=v,((_tmpD51->f2=Cyc_Toc_typ_to_c_array(_tmp7AE),_tmpD51)))))),((_tmpD50->tl=decls,_tmpD50))))));}{
struct _tuple31 _tmp7B1=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp784,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7AE,
Cyc_Absyn_var_exp(v,0),path,_tmp785,fail_stmt,decls);
nv=_tmp7B1.f1;
decls=_tmp7B1.f2;{
struct Cyc_Absyn_Stmt*_tmp7B2=_tmp7B1.f3;
struct Cyc_Absyn_Stmt*_tmp7B3=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7B3,_tmp7B2,0);
goto _LL327;};};}_LL32C: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp786=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp786->tag != 4)goto _LL32E;else{_tmp787=(_tmp786->f2)->name;}}_LL32D: {
# 3917
struct _tuple1*_tmp7B4=Cyc_Toc_temp_var();
void*_tmp7B5=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD54;struct Cyc_List_List*_tmpD53;decls=((_tmpD53=_region_malloc(rgn,sizeof(*_tmpD53)),((_tmpD53->hd=((_tmpD54=_region_malloc(rgn,sizeof(*_tmpD54)),((_tmpD54->f1=_tmp7B4,((_tmpD54->f2=Cyc_Toc_typ_to_c_array(_tmp7B5),_tmpD54)))))),((_tmpD53->tl=decls,_tmpD53))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp787,Cyc_Absyn_var_exp(_tmp7B4,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B4,0),r,0);
goto _LL327;}_LL32E: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp788=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp788->tag != 0)goto _LL330;}_LL32F:
 s=Cyc_Toc_skip_stmt_dl();goto _LL327;_LL330: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp789=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp789->tag != 3)goto _LL332;else{_tmp78A=_tmp789->f1;_tmp78B=_tmp789->f2;}}_LL331: {
# 3926
struct _tuple1*_tmp7B8=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD57;struct Cyc_List_List*_tmpD56;decls=((_tmpD56=_region_malloc(rgn,sizeof(*_tmpD56)),((_tmpD56->hd=((_tmpD57=_region_malloc(rgn,sizeof(*_tmpD57)),((_tmpD57->f1=_tmp7B8,((_tmpD57->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD57)))))),((_tmpD56->tl=decls,_tmpD56))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp78A->name,Cyc_Absyn_var_exp(_tmp7B8,0));
# 3930
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B8,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7BB=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp78B,fail_stmt,decls);
_tmp7BB.f3=Cyc_Absyn_seq_stmt(s,_tmp7BB.f3,0);
return _tmp7BB;};}_LL332: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp78C=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp78C->tag != 9)goto _LL334;}_LL333:
# 3937
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL327;_LL334: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp78D=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp78D->tag != 10)goto _LL336;else{_tmp78E=_tmp78D->f1;_tmp78F=_tmp78D->f2;}}_LL335:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp78E,_tmp78F,0),fail_stmt);goto _LL327;_LL336: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp790=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp790->tag != 11)goto _LL338;else{_tmp791=_tmp790->f1;}}_LL337:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp791,0),fail_stmt);goto _LL327;_LL338: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp792=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp792->tag != 12)goto _LL33A;else{_tmp793=_tmp792->f1;_tmp794=_tmp792->f2;}}_LL339:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp793,_tmp794,0),fail_stmt);goto _LL327;_LL33A: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp795=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp795->tag != 13)goto _LL33C;else{_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;}}_LL33B:
# 3942
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD5A;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD59;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD59=_cycalloc(sizeof(*_tmpD59)),((_tmpD59[0]=((_tmpD5A.tag=31,((_tmpD5A.f1=_tmp797->name,((_tmpD5A.f2=_tmp796,((_tmpD5A.f3=_tmp797,_tmpD5A)))))))),_tmpD59)))),0),fail_stmt);}
goto _LL327;_LL33C: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp798=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp798->tag != 14)goto _LL33E;else{_tmp799=(void*)_tmp798->f1;_tmp79A=_tmp798->f2;}}_LL33D:
# 3945
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD5D;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD5C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C[0]=((_tmpD5D.tag=32,((_tmpD5D.f1=_tmp79A->name,((_tmpD5D.f2=_tmp799,((_tmpD5D.f3=_tmp79A,_tmpD5D)))))))),_tmpD5C)))),0),fail_stmt);}
goto _LL327;_LL33E: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp79B=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp79B->tag != 6)goto _LL340;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp79C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp79B->f1)->r;if(_tmp79C->tag != 8)goto _LL340;else{_tmp79D=_tmp79C->f1;_tmp79E=_tmp79C->f2;_tmp79F=_tmp79C->f3;}}}_LL33F:
# 3955
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7C0=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp79E->name,_tmp79D->name);
void*_tmp7C1=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7C1,r);
struct Cyc_List_List*_tmp7C2=_tmp79E->typs;
for(0;_tmp79F != 0;(((_tmp79F=_tmp79F->tl,_tmp7C2=((struct Cyc_List_List*)_check_null(_tmp7C2))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7C3=(struct Cyc_Absyn_Pat*)_tmp79F->hd;
if(_tmp7C3->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7C4=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7C2))->hd)).f2;
void*_tmp7C5=(void*)_check_null(_tmp7C3->topt);
void*_tmp7C6=Cyc_Toc_typ_to_c_array(_tmp7C5);
struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7C4)))
_tmp7C7=Cyc_Toc_cast_it(_tmp7C6,_tmp7C7);{
# 3973
struct _tuple31 _tmp7C8=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C5,_tmp7C7,_tmp7C7,_tmp7C3,fail_stmt,decls);
# 3975
nv=_tmp7C8.f1;
decls=_tmp7C8.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7C8.f3,0);};};}{
# 3979
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp79D->is_extensible?Cyc_Absyn_var_exp(_tmp79E->name,0): Cyc_Toc_datatype_tag(_tmp79D,_tmp79E->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3987
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7C0,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3990
s=Cyc_Absyn_declare_stmt(_tmp7C0,_tmp7C1,rcast,s,0);
goto _LL327;};};_LL340: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7A0=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7A0->tag != 8)goto _LL342;else{_tmp7A1=_tmp7A0->f3;}}_LL341:
# 3993
 _tmp7A3=_tmp7A1;goto _LL343;_LL342: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7A2=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7A2->tag != 5)goto _LL344;else{_tmp7A3=_tmp7A2->f1;}}_LL343:
# 4001
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7A3 != 0;(_tmp7A3=_tmp7A3->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7C9=(struct Cyc_Absyn_Pat*)_tmp7A3->hd;
if(_tmp7C9->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7CA=(void*)_check_null(_tmp7C9->topt);
struct _dyneither_ptr*_tmp7CB=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7CC=Cyc_Toc_xlate_pat(nv,rgn,_tmp7CA,Cyc_Toc_member_exp(r,_tmp7CB,0),
Cyc_Toc_member_exp(path,_tmp7CB,0),_tmp7C9,fail_stmt,decls);
nv=_tmp7CC.f1;
decls=_tmp7CC.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7CC.f3,0);};}
# 4015
goto _LL327;};_LL344: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A4=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7A4->tag != 7)goto _LL346;else{if(_tmp7A4->f1 != 0)goto _LL346;}}_LL345: {
# 4017
const char*_tmpD60;void*_tmpD5F;(_tmpD5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD60="unresolved aggregate pattern!",_tag_dyneither(_tmpD60,sizeof(char),30))),_tag_dyneither(_tmpD5F,sizeof(void*),0)));}_LL346: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7A5->tag != 7)goto _LL348;else{if(_tmp7A5->f1 == 0)goto _LL348;_tmp7A6=(_tmp7A5->f1)->aggr_info;_tmp7A7=_tmp7A5->f3;}}_LL347: {
# 4019
struct Cyc_Absyn_Aggrdecl*_tmp7CF=Cyc_Absyn_get_known_aggrdecl(_tmp7A6);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7CF->impl))->tagged){
# 4023
struct _tuple33*_tmp7D0=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7A7))->hd;struct Cyc_List_List*_tmp7D2;struct Cyc_Absyn_Pat*_tmp7D3;struct _tuple33*_tmp7D1=_tmp7D0;_tmp7D2=_tmp7D1->f1;_tmp7D3=_tmp7D1->f2;{
struct _dyneither_ptr*f;
{void*_tmp7D4=(void*)((struct Cyc_List_List*)_check_null(_tmp7D2))->hd;void*_tmp7D5=_tmp7D4;struct _dyneither_ptr*_tmp7D7;_LL351: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7D6=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7D5;if(_tmp7D6->tag != 1)goto _LL353;else{_tmp7D7=_tmp7D6->f1;}}_LL352:
 f=_tmp7D7;goto _LL350;_LL353:;_LL354: {
const char*_tmpD63;void*_tmpD62;(_tmpD62=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD63="no field name in tagged union pattern",_tag_dyneither(_tmpD63,sizeof(char),38))),_tag_dyneither(_tmpD62,sizeof(void*),0)));}_LL350:;}{
# 4030
void*_tmp7DA=(void*)_check_null(_tmp7D3->topt);
void*_tmp7DB=Cyc_Toc_typ_to_c_array(_tmp7DA);
# 4033
struct Cyc_Absyn_Exp*_tmp7DC=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7DD=Cyc_Toc_cast_it(_tmp7DB,_tmp7DD);
_tmp7DC=Cyc_Toc_cast_it(_tmp7DB,_tmp7DC);{
struct _tuple31 _tmp7DE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7DA,_tmp7DD,_tmp7DC,_tmp7D3,fail_stmt,decls);
# 4041
nv=_tmp7DE.f1;
decls=_tmp7DE.f2;{
struct Cyc_Absyn_Stmt*_tmp7DF=_tmp7DE.f3;
struct Cyc_Absyn_Stmt*_tmp7E0=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7CF,f),0),fail_stmt);
# 4049
s=Cyc_Absyn_seq_stmt(_tmp7E0,_tmp7DF,0);};};};};}else{
# 4052
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7A7 != 0;_tmp7A7=_tmp7A7->tl){
struct _tuple33*_tmp7E1=(struct _tuple33*)_tmp7A7->hd;
struct Cyc_Absyn_Pat*_tmp7E2=(*_tmp7E1).f2;
if(_tmp7E2->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7E3=(void*)((struct Cyc_List_List*)_check_null((*_tmp7E1).f1))->hd;void*_tmp7E4=_tmp7E3;struct _dyneither_ptr*_tmp7E6;_LL356: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7E5=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7E4;if(_tmp7E5->tag != 1)goto _LL358;else{_tmp7E6=_tmp7E5->f1;}}_LL357:
 f=_tmp7E6;goto _LL355;_LL358:;_LL359: {
struct Cyc_Toc_Match_error_exn_struct _tmpD66;struct Cyc_Toc_Match_error_exn_struct*_tmpD65;(int)_throw((void*)((_tmpD65=_cycalloc_atomic(sizeof(*_tmpD65)),((_tmpD65[0]=((_tmpD66.tag=Cyc_Toc_Match_error,_tmpD66)),_tmpD65)))));}_LL355:;}{
# 4063
void*_tmp7E9=(void*)_check_null(_tmp7E2->topt);
void*_tmp7EA=Cyc_Toc_typ_to_c_array(_tmp7E9);
struct Cyc_Absyn_Exp*_tmp7EB=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7CF->impl))->fields,f)))->type))
_tmp7EB=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7EA),_tmp7EB);{
struct _tuple31 _tmp7EC=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E9,_tmp7EB,
Cyc_Toc_member_exp(path,f,0),_tmp7E2,fail_stmt,decls);
nv=_tmp7EC.f1;
decls=_tmp7EC.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7EC.f3,0);};};};}}
# 4075
goto _LL327;}_LL348: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7A8=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7A8->tag != 6)goto _LL34A;else{_tmp7A9=_tmp7A8->f1;}}_LL349: {
# 4078
void*_tmp7ED=(void*)_check_null(_tmp7A9->topt);
# 4081
struct _tuple31 _tmp7EE=Cyc_Toc_xlate_pat(nv,rgn,_tmp7ED,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7A9,fail_stmt,decls);
# 4083
nv=_tmp7EE.f1;
decls=_tmp7EE.f2;{
struct Cyc_Absyn_Stmt*_tmp7EF=_tmp7EE.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4089
Cyc_Toc_skip_stmt_dl(),0),_tmp7EF,0);else{
# 4091
s=_tmp7EF;}
goto _LL327;};}_LL34A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7AA=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7AA->tag != 15)goto _LL34C;}_LL34B: {
const char*_tmpD69;void*_tmpD68;(_tmpD68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD69="unknownid pat",_tag_dyneither(_tmpD69,sizeof(char),14))),_tag_dyneither(_tmpD68,sizeof(void*),0)));}_LL34C: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7AB=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7AB->tag != 16)goto _LL34E;}_LL34D: {
const char*_tmpD6C;void*_tmpD6B;(_tmpD6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6C="unknowncall pat",_tag_dyneither(_tmpD6C,sizeof(char),16))),_tag_dyneither(_tmpD6B,sizeof(void*),0)));}_LL34E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7AC=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp780;if(_tmp7AC->tag != 17)goto _LL327;}_LL34F: {
const char*_tmpD6F;void*_tmpD6E;(_tmpD6E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD6F="exp pat",_tag_dyneither(_tmpD6F,sizeof(char),8))),_tag_dyneither(_tmpD6E,sizeof(void*),0)));}_LL327:;}{
# 4097
struct _tuple31 _tmpD70;return(_tmpD70.f1=nv,((_tmpD70.f2=decls,((_tmpD70.f3=s,_tmpD70)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4134 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4136
struct _tuple34*_tmpD71;return(_tmpD71=_region_malloc(r,sizeof(*_tmpD71)),((_tmpD71->f1=Cyc_Toc_fresh_label(),((_tmpD71->f2=Cyc_Toc_fresh_label(),((_tmpD71->f3=sc,_tmpD71)))))));}
# 4139
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4141
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7F8=(void*)_check_null(e->topt);
# 4146
int leave_as_switch;
{void*_tmp7F9=Cyc_Tcutil_compress(_tmp7F8);void*_tmp7FA=_tmp7F9;_LL35B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7FB=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7FA;if(_tmp7FB->tag != 6)goto _LL35D;}_LL35C:
 goto _LL35E;_LL35D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7FC=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7FA;if(_tmp7FC->tag != 13)goto _LL35F;}_LL35E:
 leave_as_switch=1;goto _LL35A;_LL35F:;_LL360:
 leave_as_switch=0;goto _LL35A;_LL35A:;}
# 4152
{struct Cyc_List_List*_tmp7FD=scs;for(0;_tmp7FD != 0;_tmp7FD=_tmp7FD->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7FD->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7FD->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4157
if(leave_as_switch){
# 4159
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7FE=scs;for(0;_tmp7FE != 0;_tmp7FE=_tmp7FE->tl){
struct Cyc_Absyn_Stmt*_tmp7FF=((struct Cyc_Absyn_Switch_clause*)_tmp7FE->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7FE->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7FF,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp800=_new_region("rgn");struct _RegionHandle*rgn=& _tmp800;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7FF);;_pop_region(rgn);};}}
# 4168
return;}{
# 4171
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4176
struct _RegionHandle _tmp801=_new_region("rgn");struct _RegionHandle*rgn=& _tmp801;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp802=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4187
{struct Cyc_List_List*_tmp803=lscs;for(0;_tmp803 != 0;_tmp803=_tmp803->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp803->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp803->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp803->tl))->hd)).f1;
struct _tuple31 _tmp804=Cyc_Toc_xlate_pat(_tmp802,rgn,_tmp7F8,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4190
struct Cyc_Toc_Env*_tmp806;struct Cyc_List_List*_tmp807;struct Cyc_Absyn_Stmt*_tmp808;struct _tuple31 _tmp805=_tmp804;_tmp806=_tmp805.f1;_tmp807=_tmp805.f2;_tmp808=_tmp805.f3;
# 4193
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp809=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp806,_tmp809);
_tmp808=Cyc_Absyn_seq_stmt(_tmp808,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp809,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4200
decls=_tmp807;
{struct Cyc_List_List*_tmpD72;nvs=((_tmpD72=_region_malloc(rgn,sizeof(*_tmpD72)),((_tmpD72->hd=_tmp806,((_tmpD72->tl=nvs,_tmpD72))))));}{
struct Cyc_List_List*_tmpD73;test_stmts=((_tmpD73=_region_malloc(rgn,sizeof(*_tmpD73)),((_tmpD73->hd=_tmp808,((_tmpD73->tl=test_stmts,_tmpD73))))));};}}
# 4204
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4209
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp80C=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp80E;struct _dyneither_ptr*_tmp80F;struct Cyc_Absyn_Switch_clause*_tmp810;struct _tuple34*_tmp80D=_tmp80C;_tmp80E=_tmp80D->f1;_tmp80F=_tmp80D->f2;_tmp810=_tmp80D->f3;{
struct Cyc_Toc_Env*_tmp811=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp810->body;
struct _RegionHandle _tmp812=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp812;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp813=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp815;struct Cyc_Absyn_Switch_clause*_tmp816;struct _tuple34*_tmp814=_tmp813;_tmp815=_tmp814->f2;_tmp816=_tmp814->f3;{
struct Cyc_List_List*_tmp817=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp816->pat_vars))->v)).f1);
_tmp817=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp817);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp811,end_l,_tmp815,_tmp817,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4224
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp811,end_l),s);}
# 4226
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp80E,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp80F,s,0),0);{
struct Cyc_List_List*_tmpD74;stmts=((_tmpD74=_region_malloc(rgn,sizeof(*_tmpD74)),((_tmpD74->hd=s,((_tmpD74->tl=stmts,_tmpD74))))));};
# 4214
;_pop_region(rgn2);};}{
# 4230
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4233
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp819=(struct _tuple32*)decls->hd;struct _tuple1*_tmp81B;void*_tmp81C;struct _tuple32*_tmp81A=_tmp819;_tmp81B=_tmp81A->f1;_tmp81C=_tmp81A->f2;
res=Cyc_Absyn_declare_stmt(_tmp81B,_tmp81C,0,res,0);}
# 4238
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4177
;_pop_region(rgn);};};}
# 4243
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4249
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4251
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD75;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD75=_cycalloc(sizeof(*_tmpD75)),((_tmpD75->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD75->tl=0,_tmpD75)))))),0),0);}
# 4255
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4260
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4262
while(1){
void*_tmp81E=s->r;void*_tmp81F=_tmp81E;struct Cyc_Absyn_Exp*_tmp822;struct Cyc_Absyn_Stmt*_tmp824;struct Cyc_Absyn_Stmt*_tmp825;struct Cyc_Absyn_Exp*_tmp827;struct Cyc_Absyn_Exp*_tmp829;struct Cyc_Absyn_Stmt*_tmp82A;struct Cyc_Absyn_Stmt*_tmp82B;struct Cyc_Absyn_Exp*_tmp82D;struct Cyc_Absyn_Stmt*_tmp82E;struct Cyc_Absyn_Stmt*_tmp830;struct Cyc_Absyn_Stmt*_tmp832;struct Cyc_Absyn_Stmt*_tmp834;struct Cyc_Absyn_Exp*_tmp836;struct Cyc_Absyn_Exp*_tmp837;struct Cyc_Absyn_Exp*_tmp838;struct Cyc_Absyn_Stmt*_tmp839;struct Cyc_Absyn_Exp*_tmp83B;struct Cyc_List_List*_tmp83C;struct Cyc_List_List*_tmp83E;struct Cyc_Absyn_Switch_clause**_tmp83F;struct Cyc_Absyn_Decl*_tmp841;struct Cyc_Absyn_Stmt*_tmp842;struct _dyneither_ptr*_tmp844;struct Cyc_Absyn_Stmt*_tmp845;struct Cyc_Absyn_Stmt*_tmp847;struct Cyc_Absyn_Exp*_tmp848;struct Cyc_Absyn_Stmt*_tmp84A;struct Cyc_List_List*_tmp84B;struct Cyc_Absyn_Exp*_tmp84D;_LL362: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp820=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp820->tag != 0)goto _LL364;}_LL363:
# 4265
 return;_LL364: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp821=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp821->tag != 1)goto _LL366;else{_tmp822=_tmp821->f1;}}_LL365:
# 4267
 Cyc_Toc_exp_to_c(nv,_tmp822);
return;_LL366: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp823=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp823->tag != 2)goto _LL368;else{_tmp824=_tmp823->f1;_tmp825=_tmp823->f2;}}_LL367:
# 4270
 Cyc_Toc_stmt_to_c(nv,_tmp824);
s=_tmp825;
continue;_LL368: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp826=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp826->tag != 3)goto _LL36A;else{_tmp827=_tmp826->f1;}}_LL369: {
# 4274
void*topt=0;
if(_tmp827 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp827->topt));
Cyc_Toc_exp_to_c(nv,_tmp827);}
# 4280
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp84E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp84F=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp84E,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp84E,topt,_tmp827,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp84F,0),0))->r;}else{
# 4288
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4290
return;}_LL36A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp828=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp828->tag != 4)goto _LL36C;else{_tmp829=_tmp828->f1;_tmp82A=_tmp828->f2;_tmp82B=_tmp828->f3;}}_LL36B:
# 4292
 Cyc_Toc_exp_to_c(nv,_tmp829);
Cyc_Toc_stmt_to_c(nv,_tmp82A);
s=_tmp82B;
continue;_LL36C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp82C=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp82C->tag != 5)goto _LL36E;else{_tmp82D=(_tmp82C->f1).f1;_tmp82E=_tmp82C->f2;}}_LL36D:
# 4297
 Cyc_Toc_exp_to_c(nv,_tmp82D);{
struct _RegionHandle _tmp850=_new_region("temp");struct _RegionHandle*temp=& _tmp850;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp82E);
# 4301
_npop_handler(0);return;
# 4298
;_pop_region(temp);};_LL36E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp82F=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp82F->tag != 6)goto _LL370;else{_tmp830=_tmp82F->f1;}}_LL36F: {
# 4303
struct _dyneither_ptr**_tmp852;struct Cyc_Toc_Env*_tmp851=nv;_tmp852=_tmp851->break_lab;
if(_tmp852 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp852,0);{
# 4307
int dest_depth=_tmp830 == 0?0: _tmp830->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL370: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp831=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp831->tag != 7)goto _LL372;else{_tmp832=_tmp831->f1;}}_LL371: {
# 4311
struct _dyneither_ptr**_tmp854;struct Cyc_Toc_Env*_tmp853=nv;_tmp854=_tmp853->continue_lab;
if(_tmp854 != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp854,0);
_tmp834=_tmp832;goto _LL373;}_LL372: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp833=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp833->tag != 8)goto _LL374;else{_tmp834=_tmp833->f2;}}_LL373:
# 4317
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp834))->try_depth,s);
return;_LL374: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp835=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp835->tag != 9)goto _LL376;else{_tmp836=_tmp835->f1;_tmp837=(_tmp835->f2).f1;_tmp838=(_tmp835->f3).f1;_tmp839=_tmp835->f4;}}_LL375:
# 4321
 Cyc_Toc_exp_to_c(nv,_tmp836);Cyc_Toc_exp_to_c(nv,_tmp837);Cyc_Toc_exp_to_c(nv,_tmp838);{
struct _RegionHandle _tmp855=_new_region("temp");struct _RegionHandle*temp=& _tmp855;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp839);
# 4325
_npop_handler(0);return;
# 4322
;_pop_region(temp);};_LL376: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp83A=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp83A->tag != 10)goto _LL378;else{_tmp83B=_tmp83A->f1;_tmp83C=_tmp83A->f2;}}_LL377:
# 4327
 Cyc_Toc_xlate_switch(nv,s,_tmp83B,_tmp83C);
return;_LL378: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp83D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp83D->tag != 11)goto _LL37A;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;}}_LL379: {
# 4330
struct Cyc_Toc_FallthruInfo*_tmp857;struct Cyc_Toc_Env*_tmp856=nv;_tmp857=_tmp856->fallthru_info;
if(_tmp857 == 0){
const char*_tmpD78;void*_tmpD77;(_tmpD77=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD78="fallthru in unexpected place",_tag_dyneither(_tmpD78,sizeof(char),29))),_tag_dyneither(_tmpD77,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp85A=*_tmp857;struct _dyneither_ptr*_tmp85C;struct Cyc_List_List*_tmp85D;struct Cyc_Dict_Dict _tmp85E;struct Cyc_Toc_FallthruInfo _tmp85B=_tmp85A;_tmp85C=_tmp85B.label;_tmp85D=_tmp85B.binders;_tmp85E=_tmp85B.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp85C,0);
# 4336
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp83F)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp85F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp85D);
struct Cyc_List_List*_tmp860=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp83E);
for(0;_tmp85F != 0;(_tmp85F=_tmp85F->tl,_tmp860=_tmp860->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp860))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp85E,(struct _tuple1*)_tmp85F->hd),(struct Cyc_Absyn_Exp*)_tmp860->hd,0),s2,0);}
# 4344
s->r=s2->r;
return;};};};}_LL37A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp840=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp840->tag != 12)goto _LL37C;else{_tmp841=_tmp840->f1;_tmp842=_tmp840->f2;}}_LL37B:
# 4350
{void*_tmp861=_tmp841->r;void*_tmp862=_tmp861;struct Cyc_Absyn_Vardecl*_tmp864;struct Cyc_Absyn_Pat*_tmp866;struct Cyc_Absyn_Exp*_tmp867;struct Cyc_List_List*_tmp869;struct Cyc_Absyn_Fndecl*_tmp86B;struct Cyc_Absyn_Tvar*_tmp86D;struct Cyc_Absyn_Vardecl*_tmp86E;int _tmp86F;struct Cyc_Absyn_Exp*_tmp870;_LL385: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp863=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp862;if(_tmp863->tag != 0)goto _LL387;else{_tmp864=_tmp863->f1;}}_LL386: {
# 4352
struct _RegionHandle _tmp871=_new_region("temp");struct _RegionHandle*temp=& _tmp871;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD7B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD7A;struct Cyc_Toc_Env*_tmp872=Cyc_Toc_add_varmap(temp,nv,_tmp864->name,
Cyc_Absyn_varb_exp(_tmp864->name,(void*)((_tmpD7A=_cycalloc(sizeof(*_tmpD7A)),((_tmpD7A[0]=((_tmpD7B.tag=4,((_tmpD7B.f1=_tmp864,_tmpD7B)))),_tmpD7A)))),0));
Cyc_Toc_local_decl_to_c(_tmp872,_tmp872,_tmp864,_tmp842);}
# 4357
_npop_handler(0);goto _LL384;
# 4352
;_pop_region(temp);}_LL387: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp865=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp862;if(_tmp865->tag != 2)goto _LL389;else{_tmp866=_tmp865->f1;_tmp867=_tmp865->f3;}}_LL388:
# 4361
{void*_tmp875=_tmp866->r;void*_tmp876=_tmp875;struct Cyc_Absyn_Vardecl*_tmp878;_LL392: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp877=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp876;if(_tmp877->tag != 1)goto _LL394;else{_tmp878=_tmp877->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp879=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp877->f2)->r;if(_tmp879->tag != 0)goto _LL394;};}}_LL393: {
# 4363
struct _tuple1*old_name=_tmp878->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp878->name=new_name;
_tmp878->initializer=_tmp867;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD7E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD7D;_tmp841->r=(void*)((_tmpD7D=_cycalloc(sizeof(*_tmpD7D)),((_tmpD7D[0]=((_tmpD7E.tag=0,((_tmpD7E.f1=_tmp878,_tmpD7E)))),_tmpD7D))));}{
struct _RegionHandle _tmp87C=_new_region("temp");struct _RegionHandle*temp=& _tmp87C;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD81;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD80;struct Cyc_Toc_Env*_tmp87D=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80[0]=((_tmpD81.tag=4,((_tmpD81.f1=_tmp878,_tmpD81)))),_tmpD80)))),0));
Cyc_Toc_local_decl_to_c(_tmp87D,nv,_tmp878,_tmp842);}
# 4374
_npop_handler(0);goto _LL391;
# 4368
;_pop_region(temp);};}_LL394:;_LL395:
# 4379
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp866,(void*)_check_null(_tmp867->topt),_tmp867,_tmp842))->r;
goto _LL391;_LL391:;}
# 4382
goto _LL384;_LL389: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp868=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp862;if(_tmp868->tag != 3)goto _LL38B;else{_tmp869=_tmp868->f1;}}_LL38A: {
# 4395 "toc.cyc"
struct Cyc_List_List*_tmp880=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp869);
if(_tmp880 == 0){
const char*_tmpD84;void*_tmpD83;(_tmpD83=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD84="empty Letv_d",_tag_dyneither(_tmpD84,sizeof(char),13))),_tag_dyneither(_tmpD83,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD87;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD86;_tmp841->r=(void*)((_tmpD86=_cycalloc(sizeof(*_tmpD86)),((_tmpD86[0]=((_tmpD87.tag=0,((_tmpD87.f1=(struct Cyc_Absyn_Vardecl*)_tmp880->hd,_tmpD87)))),_tmpD86))));}
_tmp880=_tmp880->tl;
for(0;_tmp880 != 0;_tmp880=_tmp880->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD89;struct Cyc_Absyn_Decl*_tmp885=Cyc_Absyn_new_decl((void*)((_tmpD89=_cycalloc(sizeof(*_tmpD89)),((_tmpD89[0]=((_tmpD8A.tag=0,((_tmpD8A.f1=(struct Cyc_Absyn_Vardecl*)_tmp880->hd,_tmpD8A)))),_tmpD89)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp885,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4404
Cyc_Toc_stmt_to_c(nv,s);
goto _LL384;}_LL38B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp86A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp862;if(_tmp86A->tag != 1)goto _LL38D;else{_tmp86B=_tmp86A->f1;}}_LL38C: {
# 4407
struct _tuple1*_tmp888=_tmp86B->name;
struct _RegionHandle _tmp889=_new_region("temp");struct _RegionHandle*temp=& _tmp889;_push_region(temp);{
struct Cyc_Toc_Env*_tmp88A=Cyc_Toc_add_varmap(temp,nv,_tmp86B->name,Cyc_Absyn_var_exp(_tmp888,0));
Cyc_Toc_fndecl_to_c(_tmp88A,_tmp86B,0);
Cyc_Toc_stmt_to_c(_tmp88A,_tmp842);}
# 4413
_npop_handler(0);goto _LL384;
# 4408
;_pop_region(temp);}_LL38D: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp86C=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp862;if(_tmp86C->tag != 4)goto _LL38F;else{_tmp86D=_tmp86C->f1;_tmp86E=_tmp86C->f2;_tmp86F=_tmp86C->f3;_tmp870=_tmp86C->f4;}}_LL38E: {
# 4415
struct Cyc_Absyn_Stmt*_tmp88B=_tmp842;
# 4423
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp86E->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4430
struct _RegionHandle _tmp88C=_new_region("temp");struct _RegionHandle*temp=& _tmp88C;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp88B);
# 4433
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp88B,0))->r;else{
if((unsigned int)_tmp870){
Cyc_Toc_exp_to_c(nv,_tmp870);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp870,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp88B,0))->r;};}else{
# 4441
struct Cyc_Absyn_Exp*_tmpD8D[1];struct Cyc_Absyn_Exp*_tmpD8C[1];struct Cyc_List_List*_tmpD8B;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD8B=_cycalloc(sizeof(*_tmpD8B)),((_tmpD8B->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD8B->tl=0,_tmpD8B)))))),0),
# 4446
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD8C[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD8C,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp88B,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD8D[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD8D,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4452
_npop_handler(0);return;
# 4430
;_pop_region(temp);}_LL38F:;_LL390: {
# 4453
const char*_tmpD90;void*_tmpD8F;(_tmpD8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD90="bad nested declaration within function",_tag_dyneither(_tmpD90,sizeof(char),39))),_tag_dyneither(_tmpD8F,sizeof(void*),0)));}_LL384:;}
# 4455
return;_LL37C: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp843=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp843->tag != 13)goto _LL37E;else{_tmp844=_tmp843->f1;_tmp845=_tmp843->f2;}}_LL37D:
# 4457
 s=_tmp845;continue;_LL37E: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp846=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp846->tag != 14)goto _LL380;else{_tmp847=_tmp846->f1;_tmp848=(_tmp846->f2).f1;}}_LL37F: {
# 4459
struct _RegionHandle _tmp893=_new_region("temp");struct _RegionHandle*temp=& _tmp893;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp847);
Cyc_Toc_exp_to_c(nv,_tmp848);
# 4463
_npop_handler(0);return;
# 4459
;_pop_region(temp);}_LL380: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp849=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp849->tag != 15)goto _LL382;else{_tmp84A=_tmp849->f1;_tmp84B=_tmp849->f2;}}_LL381: {
# 4480 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4491
e_exp->topt=e_typ;{
struct _RegionHandle _tmp894=_new_region("temp");struct _RegionHandle*temp=& _tmp894;_push_region(temp);{
struct Cyc_Toc_Env*_tmp895=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4495
Cyc_Toc_stmt_to_c(_tmp895,_tmp84A);{
struct Cyc_Absyn_Stmt*_tmp896=Cyc_Absyn_seq_stmt(_tmp84A,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4501
struct _tuple1*_tmp897=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp898=Cyc_Absyn_var_exp(_tmp897,0);
struct Cyc_Absyn_Vardecl*_tmp899=Cyc_Absyn_new_vardecl(_tmp897,Cyc_Absyn_exn_typ(),0);
_tmp898->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD9A;struct Cyc_Absyn_Pat*_tmpD99;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD98;struct Cyc_Absyn_Pat*_tmpD97;struct Cyc_Absyn_Pat*_tmp89A=(_tmpD97=_cycalloc(sizeof(*_tmpD97)),((_tmpD97->r=(void*)((_tmpD9A=_cycalloc(sizeof(*_tmpD9A)),((_tmpD9A[0]=((_tmpD98.tag=1,((_tmpD98.f1=_tmp899,((_tmpD98.f2=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD99->topt=Cyc_Absyn_exn_typ(),((_tmpD99->loc=0,_tmpD99)))))))),_tmpD98)))))),_tmpD9A)))),((_tmpD97->topt=Cyc_Absyn_exn_typ(),((_tmpD97->loc=0,_tmpD97)))))));
struct Cyc_Absyn_Exp*_tmp89B=Cyc_Absyn_throw_exp(_tmp898,0);
_tmp89B->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp89C=Cyc_Absyn_exp_stmt(_tmp89B,0);
struct Cyc_Core_Opt*_tmpDA9;struct _tuple35*_tmpDA8;struct Cyc_Absyn_Vardecl**_tmpDA7;struct Cyc_List_List*_tmpDA6;struct Cyc_Absyn_Switch_clause*_tmpDA5;struct Cyc_Absyn_Switch_clause*_tmp89D=
(_tmpDA5=_cycalloc(sizeof(*_tmpDA5)),((_tmpDA5->pattern=_tmp89A,((_tmpDA5->pat_vars=((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->v=((_tmpDA6=_cycalloc(sizeof(*_tmpDA6)),((_tmpDA6->hd=((_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->f1=((_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7[0]=_tmp899,_tmpDA7)))),((_tmpDA8->f2=_tmp898,_tmpDA8)))))),((_tmpDA6->tl=0,_tmpDA6)))))),_tmpDA9)))),((_tmpDA5->where_clause=0,((_tmpDA5->body=_tmp89C,((_tmpDA5->loc=0,_tmpDA5)))))))))));
# 4512
struct Cyc_List_List*_tmpDAA;struct Cyc_Absyn_Stmt*_tmp89E=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp84B,(
(_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA->hd=_tmp89D,((_tmpDAA->tl=0,_tmpDAA))))))),0);
# 4516
Cyc_Toc_stmt_to_c(_tmp895,_tmp89E);{
# 4519
struct Cyc_List_List*_tmpDAB;struct Cyc_Absyn_Exp*_tmp89F=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDAB=_cycalloc(sizeof(*_tmpDAB)),((_tmpDAB->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDAB->tl=0,_tmpDAB)))))),0);
# 4523
struct Cyc_List_List*_tmpDAC;struct Cyc_Absyn_Stmt*_tmp8A0=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDAC=_cycalloc(sizeof(*_tmpDAC)),((_tmpDAC->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDAC->tl=0,_tmpDAC)))))),0),0);
# 4527
struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8A0,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8A1,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp89F,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8A2,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp896,
# 4538
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp89E,0),0),0),0),0),0))->r;};};};};}
# 4542
_npop_handler(0);return;
# 4492
;_pop_region(temp);};}_LL382: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp84C=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp81F;if(_tmp84C->tag != 16)goto _LL361;else{_tmp84D=_tmp84C->f1;}}_LL383:
# 4544
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4547
Cyc_Toc_exp_to_c(nv,_tmp84D);{
struct Cyc_List_List*_tmpDAD;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDAD=_cycalloc(sizeof(*_tmpDAD)),((_tmpDAD->hd=_tmp84D,((_tmpDAD->tl=0,_tmpDAD)))))),0));};}
# 4550
return;_LL361:;}}
# 4559
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8B0=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8B0;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8B1=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8B2=f->args;for(0;_tmp8B2 != 0;_tmp8B2=_tmp8B2->tl){
struct _tuple1*_tmpDAE;struct _tuple1*_tmp8B3=(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE->f1=Cyc_Absyn_Loc_n,((_tmpDAE->f2=(*((struct _tuple9*)_tmp8B2->hd)).f1,_tmpDAE)))));
(*((struct _tuple9*)_tmp8B2->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8B2->hd)).f3);
_tmp8B1=Cyc_Toc_add_varmap(frgn,_tmp8B1,_tmp8B3,Cyc_Absyn_var_exp(_tmp8B3,0));}}
# 4574
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4578
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8B5=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8B7;struct Cyc_Absyn_Tqual _tmp8B8;void*_tmp8B9;int _tmp8BA;struct Cyc_Absyn_VarargInfo _tmp8B6=_tmp8B5;_tmp8B7=_tmp8B6.name;_tmp8B8=_tmp8B6.tq;_tmp8B9=_tmp8B6.type;_tmp8BA=_tmp8B6.inject;{
void*_tmp8BB=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8B9,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8B8,Cyc_Absyn_false_conref));
struct _tuple1*_tmpDAF;struct _tuple1*_tmp8BC=(_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->f1=Cyc_Absyn_Loc_n,((_tmpDAF->f2=(struct _dyneither_ptr*)_check_null(_tmp8B7),_tmpDAF)))));
{struct _tuple9*_tmpDB2;struct Cyc_List_List*_tmpDB1;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDB1=_cycalloc(sizeof(*_tmpDB1)),((_tmpDB1->hd=((_tmpDB2=_cycalloc(sizeof(*_tmpDB2)),((_tmpDB2->f1=_tmp8B7,((_tmpDB2->f2=_tmp8B8,((_tmpDB2->f3=_tmp8BB,_tmpDB2)))))))),((_tmpDB1->tl=0,_tmpDB1)))))));}
_tmp8B1=Cyc_Toc_add_varmap(frgn,_tmp8B1,_tmp8BC,Cyc_Absyn_var_exp(_tmp8BC,0));
f->cyc_varargs=0;};}
# 4587
{struct Cyc_List_List*_tmp8C0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8C0 != 0;_tmp8C0=_tmp8C0->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8C0->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8C0->hd)->type);}}
# 4590
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8B1),f->body);}
# 4566
;_pop_region(frgn);};}
# 4595
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL396:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL397:
 return Cyc_Absyn_Extern;default: _LL398:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4612 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4615
struct _tuple37 _tmp8C1=*env;struct Cyc_Toc_TocState*_tmp8C3;struct Cyc_Absyn_Aggrdecl*_tmp8C4;int _tmp8C5;struct _tuple37 _tmp8C2=_tmp8C1;_tmp8C3=_tmp8C2.f1;_tmp8C4=(_tmp8C2.f2)->f1;_tmp8C5=(_tmp8C2.f2)->f2;{
struct _tuple1*_tmp8C6=_tmp8C4->name;
struct Cyc_Toc_TocState _tmp8C7=*_tmp8C3;struct Cyc_Dict_Dict*_tmp8C9;struct Cyc_Toc_TocState _tmp8C8=_tmp8C7;_tmp8C9=_tmp8C8.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8CA=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8C9,_tmp8C6);
if(_tmp8CA == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8C4->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpDB3;v=((_tmpDB3=_region_malloc(d,sizeof(*_tmpDB3)),((_tmpDB3->f1=_tmp8C4,((_tmpDB3->f2=Cyc_Absyn_strctq(_tmp8C6),_tmpDB3))))));}else{
# 4626
struct _tuple18*_tmpDB4;v=((_tmpDB4=_region_malloc(d,sizeof(*_tmpDB4)),((_tmpDB4->f1=_tmp8C4,((_tmpDB4->f2=Cyc_Absyn_unionq_typ(_tmp8C6),_tmpDB4))))));}
*_tmp8C9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8C9,_tmp8C6,v);};}else{
# 4629
struct _tuple18*_tmp8CD=*_tmp8CA;struct Cyc_Absyn_Aggrdecl*_tmp8CF;void*_tmp8D0;struct _tuple18*_tmp8CE=_tmp8CD;_tmp8CF=_tmp8CE->f1;_tmp8D0=_tmp8CE->f2;
if(_tmp8CF->impl == 0){
{struct _tuple18*_tmpDB5;*_tmp8C9=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8C9,_tmp8C6,((_tmpDB5=_region_malloc(d,sizeof(*_tmpDB5)),((_tmpDB5->f1=_tmp8C4,((_tmpDB5->f2=_tmp8D0,_tmpDB5)))))));}
seen_defn_before=0;}else{
# 4634
seen_defn_before=1;}}{
# 4636
struct Cyc_Absyn_Aggrdecl*_tmpDB6;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->kind=_tmp8C4->kind,((_tmpDB6->sc=Cyc_Absyn_Public,((_tmpDB6->name=_tmp8C4->name,((_tmpDB6->tvs=0,((_tmpDB6->impl=0,((_tmpDB6->attributes=_tmp8C4->attributes,_tmpDB6)))))))))))));
# 4642
if(_tmp8C4->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDB7;new_ad->impl=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7->exist_vars=0,((_tmpDB7->rgn_po=0,((_tmpDB7->fields=0,((_tmpDB7->tagged=0,_tmpDB7))))))))));}{
# 4647
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8D3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8C4->impl))->fields;for(0;_tmp8D3 != 0;_tmp8D3=_tmp8D3->tl){
# 4651
struct Cyc_Absyn_Aggrfield*_tmp8D4=(struct Cyc_Absyn_Aggrfield*)_tmp8D3->hd;
void*_tmp8D5=_tmp8D4->type;
struct Cyc_List_List*_tmp8D6=_tmp8D4->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8D5)) && (
_tmp8C4->kind == Cyc_Absyn_StructA  && _tmp8D3->tl == 0  || _tmp8C4->kind == Cyc_Absyn_UnionA)){
# 4665 "toc.cyc"
void*_tmp8D7=Cyc_Tcutil_compress(_tmp8D5);void*_tmp8D8=_tmp8D7;void*_tmp8DA;struct Cyc_Absyn_Tqual _tmp8DB;union Cyc_Absyn_Constraint*_tmp8DC;unsigned int _tmp8DD;_LL39B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D9=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8D8;if(_tmp8D9->tag != 8)goto _LL39D;else{_tmp8DA=(_tmp8D9->f1).elt_type;_tmp8DB=(_tmp8D9->f1).tq;_tmp8DC=(_tmp8D9->f1).zero_term;_tmp8DD=(_tmp8D9->f1).zt_loc;}}_LL39C:
# 4668
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDBD;struct Cyc_Absyn_ArrayInfo _tmpDBC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDBB;_tmp8D5=(void*)((_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB[0]=((_tmpDBD.tag=8,((_tmpDBD.f1=((_tmpDBC.elt_type=_tmp8DA,((_tmpDBC.tq=_tmp8DB,((_tmpDBC.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDBC.zero_term=_tmp8DC,((_tmpDBC.zt_loc=_tmp8DD,_tmpDBC)))))))))),_tmpDBD)))),_tmpDBB))));}
goto _LL39A;_LL39D:;_LL39E:
# 4671
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDC3;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpDC2;struct Cyc_List_List*_tmpDC1;_tmp8D6=((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->hd=(void*)((_tmpDC3=_cycalloc_atomic(sizeof(*_tmpDC3)),((_tmpDC3[0]=((_tmpDC2.tag=6,((_tmpDC2.f1=- 1,_tmpDC2)))),_tmpDC3)))),((_tmpDC1->tl=_tmp8D6,_tmpDC1))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDC9;struct Cyc_Absyn_ArrayInfo _tmpDC8;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDC7;_tmp8D5=(void*)((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7[0]=((_tmpDC9.tag=8,((_tmpDC9.f1=((_tmpDC8.elt_type=Cyc_Absyn_void_star_typ(),((_tmpDC8.tq=
Cyc_Absyn_empty_tqual(0),((_tmpDC8.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpDC8.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpDC8.zt_loc=0,_tmpDC8)))))))))),_tmpDC9)))),_tmpDC7))));};_LL39A:;}{
# 4678
struct Cyc_Absyn_Aggrfield*_tmpDCA;struct Cyc_Absyn_Aggrfield*_tmp8E7=(_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->name=_tmp8D4->name,((_tmpDCA->tq=Cyc_Toc_mt_tq,((_tmpDCA->type=
# 4680
Cyc_Toc_typ_to_c(_tmp8D5),((_tmpDCA->width=_tmp8D4->width,((_tmpDCA->attributes=_tmp8D6,((_tmpDCA->requires_clause=0,_tmpDCA)))))))))))));
# 4688
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8C4->impl))->tagged){
void*_tmp8E8=_tmp8E7->type;
struct _dyneither_ptr*_tmp8E9=_tmp8E7->name;
const char*_tmpDCF;void*_tmpDCE[2];struct Cyc_String_pa_PrintArg_struct _tmpDCD;struct Cyc_String_pa_PrintArg_struct _tmpDCC;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDCC.tag=0,((_tmpDCC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8E9),((_tmpDCD.tag=0,((_tmpDCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8C4->name).f2),((_tmpDCE[0]=& _tmpDCD,((_tmpDCE[1]=& _tmpDCC,Cyc_aprintf(((_tmpDCF="_union_%s_%s",_tag_dyneither(_tmpDCF,sizeof(char),13))),_tag_dyneither(_tmpDCE,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDD0;struct _dyneither_ptr*str=(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0[0]=s,_tmpDD0)));
struct Cyc_Absyn_Aggrfield*_tmpDD1;struct Cyc_Absyn_Aggrfield*_tmp8EA=(_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->name=Cyc_Toc_val_sp,((_tmpDD1->tq=Cyc_Toc_mt_tq,((_tmpDD1->type=_tmp8E8,((_tmpDD1->width=0,((_tmpDD1->attributes=0,((_tmpDD1->requires_clause=0,_tmpDD1)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDD2;struct Cyc_Absyn_Aggrfield*_tmp8EB=(_tmpDD2=_cycalloc(sizeof(*_tmpDD2)),((_tmpDD2->name=Cyc_Toc_tag_sp,((_tmpDD2->tq=Cyc_Toc_mt_tq,((_tmpDD2->type=Cyc_Absyn_sint_typ,((_tmpDD2->width=0,((_tmpDD2->attributes=0,((_tmpDD2->requires_clause=0,_tmpDD2)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDD3[2];struct Cyc_List_List*_tmp8EC=(_tmpDD3[1]=_tmp8EA,((_tmpDD3[0]=_tmp8EB,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDD3,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDD8;struct _tuple1*_tmpDD7;struct Cyc_Absyn_Aggrdecl*_tmpDD6;struct Cyc_Absyn_Aggrdecl*_tmp8ED=(_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6->kind=Cyc_Absyn_StructA,((_tmpDD6->sc=Cyc_Absyn_Public,((_tmpDD6->name=(
(_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7->f1=Cyc_Absyn_Loc_n,((_tmpDD7->f2=str,_tmpDD7)))))),((_tmpDD6->tvs=0,((_tmpDD6->impl=(
(_tmpDD8=_cycalloc(sizeof(*_tmpDD8)),((_tmpDD8->exist_vars=0,((_tmpDD8->rgn_po=0,((_tmpDD8->fields=_tmp8EC,((_tmpDD8->tagged=0,_tmpDD8)))))))))),((_tmpDD6->attributes=0,_tmpDD6)))))))))))));
# 4700
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDDE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDDD;struct Cyc_List_List*_tmpDDC;Cyc_Toc_result_decls=((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC->hd=Cyc_Absyn_new_decl((void*)((_tmpDDE=_cycalloc(sizeof(*_tmpDDE)),((_tmpDDE[0]=((_tmpDDD.tag=5,((_tmpDDD.f1=_tmp8ED,_tmpDDD)))),_tmpDDE)))),0),((_tmpDDC->tl=Cyc_Toc_result_decls,_tmpDDC))))));}
_tmp8E7->type=Cyc_Absyn_strct(str);}{
# 4703
struct Cyc_List_List*_tmpDDF;new_fields=((_tmpDDF=_cycalloc(sizeof(*_tmpDDF)),((_tmpDDF->hd=_tmp8E7,((_tmpDDF->tl=new_fields,_tmpDDF))))));};};}}
# 4705
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4707
if(_tmp8C5){
struct Cyc_Absyn_Decl*_tmpDE9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDE8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDE7;struct Cyc_List_List*_tmpDE6;Cyc_Toc_result_decls=((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->r=(void*)((_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7[0]=((_tmpDE8.tag=5,((_tmpDE8.f1=new_ad,_tmpDE8)))),_tmpDE7)))),((_tmpDE9->loc=0,_tmpDE9)))))),((_tmpDE6->tl=Cyc_Toc_result_decls,_tmpDE6))))));}
return 0;};};};}
# 4712
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpDEA;struct _tuple36 p=(_tmpDEA.f1=ad,((_tmpDEA.f2=add_to_decls,_tmpDEA)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4741 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4744
struct _tuple38 _tmp904=*env;struct Cyc_Set_Set**_tmp906;struct Cyc_Absyn_Datatypedecl*_tmp907;struct _tuple38 _tmp905=_tmp904;_tmp906=(_tmp905.f1)->datatypes_so_far;_tmp907=_tmp905.f2;{
struct _tuple1*_tmp908=_tmp907->name;
if(_tmp907->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp906,_tmp908))
return 0;
*_tmp906=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp906,_tmp908);
{struct Cyc_List_List*_tmp909=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp907->fields))->v;for(0;_tmp909 != 0;_tmp909=_tmp909->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp909->hd;
# 4752
struct Cyc_List_List*_tmp90A=0;
int i=1;
{struct Cyc_List_List*_tmp90B=f->typs;for(0;_tmp90B != 0;(_tmp90B=_tmp90B->tl,i ++)){
struct _dyneither_ptr*_tmp90C=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpDEB;struct Cyc_Absyn_Aggrfield*_tmp90D=(_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->name=_tmp90C,((_tmpDEB->tq=(*((struct _tuple11*)_tmp90B->hd)).f1,((_tmpDEB->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp90B->hd)).f2),((_tmpDEB->width=0,((_tmpDEB->attributes=0,((_tmpDEB->requires_clause=0,_tmpDEB)))))))))))));
struct Cyc_List_List*_tmpDEC;_tmp90A=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->hd=_tmp90D,((_tmpDEC->tl=_tmp90A,_tmpDEC))))));}}
# 4760
{struct Cyc_Absyn_Aggrfield*_tmpDEF;struct Cyc_List_List*_tmpDEE;_tmp90A=((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE->hd=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->name=Cyc_Toc_tag_sp,((_tmpDEF->tq=Cyc_Toc_mt_tq,((_tmpDEF->type=Cyc_Absyn_sint_typ,((_tmpDEF->width=0,((_tmpDEF->attributes=0,((_tmpDEF->requires_clause=0,_tmpDEF)))))))))))))),((_tmpDEE->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp90A),_tmpDEE))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDF2;struct Cyc_Absyn_Aggrdecl*_tmpDF1;struct Cyc_Absyn_Aggrdecl*_tmp912=
(_tmpDF1=_cycalloc(sizeof(*_tmpDF1)),((_tmpDF1->kind=Cyc_Absyn_StructA,((_tmpDF1->sc=Cyc_Absyn_Public,((_tmpDF1->name=
Cyc_Toc_collapse_qvars(f->name,_tmp907->name),((_tmpDF1->tvs=0,((_tmpDF1->impl=(
# 4766
(_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2->exist_vars=0,((_tmpDF2->rgn_po=0,((_tmpDF2->fields=_tmp90A,((_tmpDF2->tagged=0,_tmpDF2)))))))))),((_tmpDF1->attributes=0,_tmpDF1)))))))))))));
# 4768
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF7;struct Cyc_List_List*_tmpDF6;Cyc_Toc_result_decls=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->hd=Cyc_Absyn_new_decl((void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF7.tag=5,((_tmpDF7.f1=_tmp912,_tmpDF7)))),_tmpDF8)))),0),((_tmpDF6->tl=Cyc_Toc_result_decls,_tmpDF6))))));};}}
# 4770
return 0;};}
# 4773
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4792 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4795
struct _tuple38 _tmp918=*env;struct Cyc_Toc_TocState*_tmp91A;struct Cyc_Absyn_Datatypedecl*_tmp91B;struct _tuple38 _tmp919=_tmp918;_tmp91A=_tmp919.f1;_tmp91B=_tmp919.f2;
if(_tmp91B->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp91C=*_tmp91A;struct Cyc_Dict_Dict*_tmp91E;struct Cyc_Toc_TocState _tmp91D=_tmp91C;_tmp91E=_tmp91D.xdatatypes_so_far;{
struct _tuple1*_tmp91F=_tmp91B->name;
{struct Cyc_List_List*_tmp920=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp91B->fields))->v;for(0;_tmp920 != 0;_tmp920=_tmp920->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp920->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp921=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp922=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp921,Cyc_Absyn_false_conref,0);
# 4806
int*_tmp923=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp91E,f->name);int*_tmp924=_tmp923;_LL3A0: if(_tmp924 != 0)goto _LL3A2;_LL3A1: {
# 4808
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4812
struct Cyc_Absyn_Vardecl*_tmp925=Cyc_Absyn_new_vardecl(f->name,_tmp922,initopt);
_tmp925->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDFE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDFD;struct Cyc_List_List*_tmpDFC;Cyc_Toc_result_decls=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=Cyc_Absyn_new_decl((void*)((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE[0]=((_tmpDFD.tag=0,((_tmpDFD.f1=_tmp925,_tmpDFD)))),_tmpDFE)))),0),((_tmpDFC->tl=Cyc_Toc_result_decls,_tmpDFC))))));}
*_tmp91E=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp91E,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp929=f->typs;for(0;_tmp929 != 0;(_tmp929=_tmp929->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpE06;void*_tmpE05[1];const char*_tmpE04;struct _dyneither_ptr*_tmpE03;struct _dyneither_ptr*_tmp92A=(_tmpE03=_cycalloc(sizeof(*_tmpE03)),((_tmpE03[0]=(struct _dyneither_ptr)((_tmpE06.tag=1,((_tmpE06.f1=(unsigned long)i,((_tmpE05[0]=& _tmpE06,Cyc_aprintf(((_tmpE04="f%d",_tag_dyneither(_tmpE04,sizeof(char),4))),_tag_dyneither(_tmpE05,sizeof(void*),1)))))))),_tmpE03)));
struct Cyc_Absyn_Aggrfield*_tmpE07;struct Cyc_Absyn_Aggrfield*_tmp92B=(_tmpE07=_cycalloc(sizeof(*_tmpE07)),((_tmpE07->name=_tmp92A,((_tmpE07->tq=(*((struct _tuple11*)_tmp929->hd)).f1,((_tmpE07->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp929->hd)).f2),((_tmpE07->width=0,((_tmpE07->attributes=0,((_tmpE07->requires_clause=0,_tmpE07)))))))))))));
struct Cyc_List_List*_tmpE08;fields=((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08->hd=_tmp92B,((_tmpE08->tl=fields,_tmpE08))))));}}
# 4825
{struct Cyc_Absyn_Aggrfield*_tmpE0B;struct Cyc_List_List*_tmpE0A;fields=((_tmpE0A=_cycalloc(sizeof(*_tmpE0A)),((_tmpE0A->hd=((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B->name=Cyc_Toc_tag_sp,((_tmpE0B->tq=Cyc_Toc_mt_tq,((_tmpE0B->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE0B->width=0,((_tmpE0B->attributes=0,((_tmpE0B->requires_clause=0,_tmpE0B)))))))))))))),((_tmpE0A->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE0A))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE0E;struct Cyc_Absyn_Aggrdecl*_tmpE0D;struct Cyc_Absyn_Aggrdecl*_tmp934=
(_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->kind=Cyc_Absyn_StructA,((_tmpE0D->sc=Cyc_Absyn_Public,((_tmpE0D->name=
Cyc_Toc_collapse_qvars(f->name,_tmp91B->name),((_tmpE0D->tvs=0,((_tmpE0D->impl=(
# 4832
(_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->exist_vars=0,((_tmpE0E->rgn_po=0,((_tmpE0E->fields=fields,((_tmpE0E->tagged=0,_tmpE0E)))))))))),((_tmpE0D->attributes=0,_tmpE0D)))))))))))));
# 4834
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE14;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE13;struct Cyc_List_List*_tmpE12;Cyc_Toc_result_decls=((_tmpE12=_cycalloc(sizeof(*_tmpE12)),((_tmpE12->hd=Cyc_Absyn_new_decl((void*)((_tmpE14=_cycalloc(sizeof(*_tmpE14)),((_tmpE14[0]=((_tmpE13.tag=5,((_tmpE13.f1=_tmp934,_tmpE13)))),_tmpE14)))),0),((_tmpE12->tl=Cyc_Toc_result_decls,_tmpE12))))));}
# 4836
goto _LL39F;};};};}_LL3A2: if(_tmp924 == 0)goto _LL3A4;if(*_tmp924 != 0)goto _LL3A4;_LL3A3:
# 4838
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp93A=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp93B=Cyc_Absyn_new_vardecl(f->name,_tmp922,_tmp93A);
_tmp93B->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE1A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE19;struct Cyc_List_List*_tmpE18;Cyc_Toc_result_decls=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=Cyc_Absyn_new_decl((void*)((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A[0]=((_tmpE19.tag=0,((_tmpE19.f1=_tmp93B,_tmpE19)))),_tmpE1A)))),0),((_tmpE18->tl=Cyc_Toc_result_decls,_tmpE18))))));}
*_tmp91E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp91E,f->name,1);}
# 4845
goto _LL39F;_LL3A4:;_LL3A5:
 goto _LL39F;_LL39F:;}}
# 4849
return 0;};};}
# 4852
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4857
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4865
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4869
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp93F=init->r;void*_tmp940=_tmp93F;struct Cyc_Absyn_Vardecl*_tmp942;struct Cyc_Absyn_Exp*_tmp943;struct Cyc_Absyn_Exp*_tmp944;int _tmp945;_LL3A7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp941=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp940;if(_tmp941->tag != 26)goto _LL3A9;else{_tmp942=_tmp941->f1;_tmp943=_tmp941->f2;_tmp944=_tmp941->f3;_tmp945=_tmp941->f4;}}_LL3A8:
# 4876
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp942,_tmp943,_tmp944,_tmp945,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3A6;_LL3A9:;_LL3AA:
# 4881
 if(vd->sc == Cyc_Absyn_Static){
# 4885
struct _RegionHandle _tmp946=_new_region("temp");struct _RegionHandle*temp=& _tmp946;_push_region(temp);
{struct Cyc_Toc_Env*_tmp947=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp947,init);}
# 4886
;_pop_region(temp);}else{
# 4890
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3A6;_LL3A6:;}else{
# 4895
void*_tmp948=Cyc_Tcutil_compress(old_typ);void*_tmp949=_tmp948;void*_tmp94B;struct Cyc_Absyn_Exp*_tmp94C;union Cyc_Absyn_Constraint*_tmp94D;_LL3AC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp94A=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp949;if(_tmp94A->tag != 8)goto _LL3AE;else{_tmp94B=(_tmp94A->f1).elt_type;_tmp94C=(_tmp94A->f1).num_elts;_tmp94D=(_tmp94A->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp94D))goto _LL3AE;_LL3AD:
 if(_tmp94C == 0){
const char*_tmpE1D;void*_tmpE1C;(_tmpE1C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE1D="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE1D,sizeof(char),55))),_tag_dyneither(_tmpE1C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp94C;
struct Cyc_Absyn_Exp*_tmp950=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4903
struct Cyc_Absyn_Exp*_tmp951=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp950,_tmp951,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3AB;};_LL3AE:;_LL3AF:
 goto _LL3AB;_LL3AB:;}}
# 4917
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp952=_new_region("prgn");struct _RegionHandle*prgn=& _tmp952;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp953=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp954=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp955=
Cyc_Toc_xlate_pat(_tmp954,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4923
struct Cyc_Toc_Env*_tmp957;struct Cyc_List_List*_tmp958;struct Cyc_Absyn_Stmt*_tmp959;struct _tuple31 _tmp956=_tmp955;_tmp957=_tmp956.f1;_tmp958=_tmp956.f2;_tmp959=_tmp956.f3;
# 4926
Cyc_Toc_stmt_to_c(_tmp957,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp959,s,0),0);
for(0;_tmp958 != 0;_tmp958=_tmp958->tl){
struct _tuple32*_tmp95A=(struct _tuple32*)_tmp958->hd;struct _tuple1*_tmp95C;void*_tmp95D;struct _tuple32*_tmp95B=_tmp95A;_tmp95C=_tmp95B->f1;_tmp95D=_tmp95B->f2;
s=Cyc_Absyn_declare_stmt(_tmp95C,_tmp95D,0,s,0);}}{
# 4933
struct Cyc_Absyn_Stmt*_tmp95E=s;_npop_handler(0);return _tmp95E;};
# 4920
;_pop_region(prgn);};}
# 4939
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp95F=e->r;void*_tmp960=_tmp95F;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_Absyn_Exp*_tmp966;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_List_List*_tmp972;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp975;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp980;struct Cyc_Absyn_Exp*_tmp981;struct Cyc_Absyn_Exp*_tmp983;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Exp*_tmp986;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_Absyn_Exp*_tmp98A;struct Cyc_List_List*_tmp98B;void**_tmp98D;struct Cyc_Absyn_Exp*_tmp98E;void**_tmp990;struct Cyc_List_List*_tmp991;struct Cyc_List_List*_tmp993;struct Cyc_List_List*_tmp995;void**_tmp997;void**_tmp999;struct Cyc_Absyn_Stmt*_tmp99B;struct Cyc_Absyn_MallocInfo*_tmp99D;_LL3B1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp961=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp961->tag != 19)goto _LL3B3;else{_tmp962=_tmp961->f1;}}_LL3B2:
 _tmp964=_tmp962;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp963=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp963->tag != 20)goto _LL3B5;else{_tmp964=_tmp963->f1;}}_LL3B4:
 _tmp966=_tmp964;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp965=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp965->tag != 21)goto _LL3B7;else{_tmp966=_tmp965->f1;}}_LL3B6:
 _tmp968=_tmp966;goto _LL3B8;_LL3B7: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp967=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp967->tag != 14)goto _LL3B9;else{_tmp968=_tmp967->f1;}}_LL3B8:
 _tmp96A=_tmp968;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp969=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp969->tag != 10)goto _LL3BB;else{_tmp96A=_tmp969->f1;}}_LL3BA:
 _tmp96C=_tmp96A;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp96B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp96B->tag != 11)goto _LL3BD;else{_tmp96C=_tmp96B->f1;}}_LL3BC:
 _tmp96E=_tmp96C;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp96D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp96D->tag != 17)goto _LL3BF;else{_tmp96E=_tmp96D->f1;}}_LL3BE:
 _tmp970=_tmp96E;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp96F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp96F->tag != 4)goto _LL3C1;else{_tmp970=_tmp96F->f1;}}_LL3C0:
 Cyc_Toc_exptypes_to_c(_tmp970);goto _LL3B0;_LL3C1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp971=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp971->tag != 2)goto _LL3C3;else{_tmp972=_tmp971->f2;}}_LL3C2:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp972);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp973=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp973->tag != 6)goto _LL3C5;else{_tmp974=_tmp973->f1;_tmp975=_tmp973->f2;}}_LL3C4:
 _tmp977=_tmp974;_tmp978=_tmp975;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp976=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp976->tag != 7)goto _LL3C7;else{_tmp977=_tmp976->f1;_tmp978=_tmp976->f2;}}_LL3C6:
 _tmp97A=_tmp977;_tmp97B=_tmp978;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp979=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp979->tag != 8)goto _LL3C9;else{_tmp97A=_tmp979->f1;_tmp97B=_tmp979->f2;}}_LL3C8:
 _tmp97D=_tmp97A;_tmp97E=_tmp97B;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp97C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp97C->tag != 22)goto _LL3CB;else{_tmp97D=_tmp97C->f1;_tmp97E=_tmp97C->f2;}}_LL3CA:
 _tmp980=_tmp97D;_tmp981=_tmp97E;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp97F=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp97F->tag != 34)goto _LL3CD;else{_tmp980=_tmp97F->f1;_tmp981=_tmp97F->f2;}}_LL3CC:
 _tmp983=_tmp980;_tmp984=_tmp981;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp982=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp982->tag != 3)goto _LL3CF;else{_tmp983=_tmp982->f1;_tmp984=_tmp982->f3;}}_LL3CE:
 Cyc_Toc_exptypes_to_c(_tmp983);Cyc_Toc_exptypes_to_c(_tmp984);goto _LL3B0;_LL3CF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp985=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp985->tag != 5)goto _LL3D1;else{_tmp986=_tmp985->f1;_tmp987=_tmp985->f2;_tmp988=_tmp985->f3;}}_LL3D0:
# 4957
 Cyc_Toc_exptypes_to_c(_tmp986);Cyc_Toc_exptypes_to_c(_tmp987);Cyc_Toc_exptypes_to_c(_tmp988);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp989->tag != 9)goto _LL3D3;else{_tmp98A=_tmp989->f1;_tmp98B=_tmp989->f2;}}_LL3D2:
# 4959
 Cyc_Toc_exptypes_to_c(_tmp98A);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp98B);goto _LL3B0;_LL3D3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp98C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp98C->tag != 13)goto _LL3D5;else{_tmp98D=(void**)((void**)& _tmp98C->f1);_tmp98E=_tmp98C->f2;}}_LL3D4:
*_tmp98D=Cyc_Toc_typ_to_c(*_tmp98D);Cyc_Toc_exptypes_to_c(_tmp98E);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp98F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp98F->tag != 24)goto _LL3D7;else{_tmp990=(void**)&(_tmp98F->f1)->f3;_tmp991=_tmp98F->f2;}}_LL3D6:
# 4962
*_tmp990=Cyc_Toc_typ_to_c(*_tmp990);
_tmp993=_tmp991;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp992=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp992->tag != 35)goto _LL3D9;else{_tmp993=_tmp992->f2;}}_LL3D8:
 _tmp995=_tmp993;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp994->tag != 25)goto _LL3DB;else{_tmp995=_tmp994->f1;}}_LL3DA:
# 4966
 for(0;_tmp995 != 0;_tmp995=_tmp995->tl){
struct _tuple19 _tmp9AD=*((struct _tuple19*)_tmp995->hd);struct Cyc_Absyn_Exp*_tmp9AF;struct _tuple19 _tmp9AE=_tmp9AD;_tmp9AF=_tmp9AE.f2;
Cyc_Toc_exptypes_to_c(_tmp9AF);}
# 4970
goto _LL3B0;_LL3DB: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp996=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp996->tag != 18)goto _LL3DD;else{_tmp997=(void**)((void**)& _tmp996->f1);}}_LL3DC:
 _tmp999=_tmp997;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp998->tag != 16)goto _LL3DF;else{_tmp999=(void**)((void**)& _tmp998->f1);}}_LL3DE:
*_tmp999=Cyc_Toc_typ_to_c(*_tmp999);goto _LL3B0;_LL3DF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp99A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp99A->tag != 36)goto _LL3E1;else{_tmp99B=_tmp99A->f1;}}_LL3E0:
 Cyc_Toc_stmttypes_to_c(_tmp99B);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp99C->tag != 33)goto _LL3E3;else{_tmp99D=(struct Cyc_Absyn_MallocInfo*)& _tmp99C->f1;}}_LL3E2:
# 4975
 if(_tmp99D->elt_type != 0){
void**_tmpE1E;_tmp99D->elt_type=((_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp99D->elt_type))),_tmpE1E))));}
Cyc_Toc_exptypes_to_c(_tmp99D->num_elts);
goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp99E->tag != 0)goto _LL3E5;}_LL3E4:
 goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp99F->tag != 1)goto _LL3E7;}_LL3E6:
 goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9A0=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A0->tag != 31)goto _LL3E9;}_LL3E8:
 goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9A1=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A1->tag != 39)goto _LL3EB;}_LL3EA:
 goto _LL3EC;_LL3EB: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9A2=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A2->tag != 32)goto _LL3ED;}_LL3EC:
 goto _LL3B0;_LL3ED: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9A3=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A3->tag != 29)goto _LL3EF;}_LL3EE:
# 4985
 goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9A4=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A4->tag != 30)goto _LL3F1;}_LL3F0:
 goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9A5=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A5->tag != 28)goto _LL3F3;}_LL3F2:
 goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9A6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A6->tag != 26)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9A7=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A7->tag != 27)goto _LL3F7;}_LL3F6:
 goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A8->tag != 23)goto _LL3F9;}_LL3F8:
 goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9A9->tag != 12)goto _LL3FB;}_LL3FA:
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9AA=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9AA->tag != 15)goto _LL3FD;}_LL3FC:
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9AB->tag != 38)goto _LL3FF;}_LL3FE:
 goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9AC=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp960;if(_tmp9AC->tag != 37)goto _LL3B0;}_LL400:
# 4995
{const char*_tmpE21;void*_tmpE20;(_tmpE20=0,Cyc_Tcutil_terr(e->loc,((_tmpE21="Cyclone expression within C code",_tag_dyneither(_tmpE21,sizeof(char),33))),_tag_dyneither(_tmpE20,sizeof(void*),0)));}
goto _LL3B0;_LL3B0:;}
# 5000
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9B3=d->r;void*_tmp9B4=_tmp9B3;struct Cyc_Absyn_Vardecl*_tmp9B6;struct Cyc_Absyn_Fndecl*_tmp9B8;struct Cyc_Absyn_Aggrdecl*_tmp9BA;struct Cyc_Absyn_Enumdecl*_tmp9BC;struct Cyc_Absyn_Typedefdecl*_tmp9BE;_LL402: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9B5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9B5->tag != 0)goto _LL404;else{_tmp9B6=_tmp9B5->f1;}}_LL403:
# 5003
 _tmp9B6->type=Cyc_Toc_typ_to_c(_tmp9B6->type);
if(_tmp9B6->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9B6->initializer));
goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9B7=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9B7->tag != 1)goto _LL406;else{_tmp9B8=_tmp9B7->f1;}}_LL405:
# 5007
 _tmp9B8->ret_type=Cyc_Toc_typ_to_c(_tmp9B8->ret_type);
{struct Cyc_List_List*_tmp9C9=_tmp9B8->args;for(0;_tmp9C9 != 0;_tmp9C9=_tmp9C9->tl){
(*((struct _tuple9*)_tmp9C9->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9C9->hd)).f3);}}
# 5011
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9B9=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9B9->tag != 5)goto _LL408;else{_tmp9BA=_tmp9B9->f1;}}_LL407:
 Cyc_Toc_aggrdecl_to_c(_tmp9BA,1);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9BB=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9BB->tag != 7)goto _LL40A;else{_tmp9BC=_tmp9BB->f1;}}_LL409:
# 5014
 if(_tmp9BC->fields != 0){
struct Cyc_List_List*_tmp9CA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9BC->fields))->v;for(0;_tmp9CA != 0;_tmp9CA=_tmp9CA->tl){
struct Cyc_Absyn_Enumfield*_tmp9CB=(struct Cyc_Absyn_Enumfield*)_tmp9CA->hd;
if(_tmp9CB->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9CB->tag));}}
# 5019
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9BD=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9BD->tag != 8)goto _LL40C;else{_tmp9BE=_tmp9BD->f1;}}_LL40B:
 _tmp9BE->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9BE->defn));goto _LL401;_LL40C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9BF=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9BF->tag != 2)goto _LL40E;}_LL40D:
 goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9C0=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C0->tag != 3)goto _LL410;}_LL40F:
 goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9C1=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C1->tag != 6)goto _LL412;}_LL411:
 goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9C2=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C2->tag != 9)goto _LL414;}_LL413:
 goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9C3=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C3->tag != 10)goto _LL416;}_LL415:
 goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9C4=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C4->tag != 11)goto _LL418;}_LL417:
 goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9C5=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C5->tag != 12)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9C6=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C6->tag != 4)goto _LL41C;}_LL41B:
# 5030
{const char*_tmpE24;void*_tmpE23;(_tmpE23=0,Cyc_Tcutil_terr(d->loc,((_tmpE24="Cyclone declaration within C code",_tag_dyneither(_tmpE24,sizeof(char),34))),_tag_dyneither(_tmpE23,sizeof(void*),0)));}
goto _LL401;_LL41C: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9C7=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C7->tag != 13)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9C8=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9B4;if(_tmp9C8->tag != 14)goto _LL401;}_LL41F:
# 5034
 goto _LL401;_LL401:;}
# 5038
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9CE=s->r;void*_tmp9CF=_tmp9CE;struct Cyc_Absyn_Exp*_tmp9D1;struct Cyc_Absyn_Stmt*_tmp9D3;struct Cyc_Absyn_Stmt*_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_Absyn_Exp*_tmp9D8;struct Cyc_Absyn_Stmt*_tmp9D9;struct Cyc_Absyn_Stmt*_tmp9DA;struct Cyc_Absyn_Exp*_tmp9DC;struct Cyc_Absyn_Stmt*_tmp9DD;struct Cyc_Absyn_Exp*_tmp9DF;struct Cyc_Absyn_Exp*_tmp9E0;struct Cyc_Absyn_Exp*_tmp9E1;struct Cyc_Absyn_Stmt*_tmp9E2;struct Cyc_Absyn_Exp*_tmp9E4;struct Cyc_List_List*_tmp9E5;struct Cyc_Absyn_Decl*_tmp9E7;struct Cyc_Absyn_Stmt*_tmp9E8;struct Cyc_Absyn_Stmt*_tmp9EA;struct Cyc_Absyn_Exp*_tmp9EB;_LL421: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9D0=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9D0->tag != 1)goto _LL423;else{_tmp9D1=_tmp9D0->f1;}}_LL422:
 Cyc_Toc_exptypes_to_c(_tmp9D1);goto _LL420;_LL423: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9D2=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9D2->tag != 2)goto _LL425;else{_tmp9D3=_tmp9D2->f1;_tmp9D4=_tmp9D2->f2;}}_LL424:
 Cyc_Toc_stmttypes_to_c(_tmp9D3);Cyc_Toc_stmttypes_to_c(_tmp9D4);goto _LL420;_LL425: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9D5=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9D5->tag != 3)goto _LL427;else{_tmp9D6=_tmp9D5->f1;}}_LL426:
 if(_tmp9D6 != 0)Cyc_Toc_exptypes_to_c(_tmp9D6);goto _LL420;_LL427: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9D7=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9D7->tag != 4)goto _LL429;else{_tmp9D8=_tmp9D7->f1;_tmp9D9=_tmp9D7->f2;_tmp9DA=_tmp9D7->f3;}}_LL428:
# 5044
 Cyc_Toc_exptypes_to_c(_tmp9D8);Cyc_Toc_stmttypes_to_c(_tmp9D9);Cyc_Toc_stmttypes_to_c(_tmp9DA);goto _LL420;_LL429: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9DB=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9DB->tag != 5)goto _LL42B;else{_tmp9DC=(_tmp9DB->f1).f1;_tmp9DD=_tmp9DB->f2;}}_LL42A:
# 5046
 Cyc_Toc_exptypes_to_c(_tmp9DC);Cyc_Toc_stmttypes_to_c(_tmp9DD);goto _LL420;_LL42B: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9DE=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9DE->tag != 9)goto _LL42D;else{_tmp9DF=_tmp9DE->f1;_tmp9E0=(_tmp9DE->f2).f1;_tmp9E1=(_tmp9DE->f3).f1;_tmp9E2=_tmp9DE->f4;}}_LL42C:
# 5048
 Cyc_Toc_exptypes_to_c(_tmp9DF);Cyc_Toc_exptypes_to_c(_tmp9E0);Cyc_Toc_exptypes_to_c(_tmp9E1);
Cyc_Toc_stmttypes_to_c(_tmp9E2);goto _LL420;_LL42D: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9E3=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9E3->tag != 10)goto _LL42F;else{_tmp9E4=_tmp9E3->f1;_tmp9E5=_tmp9E3->f2;}}_LL42E:
# 5051
 Cyc_Toc_exptypes_to_c(_tmp9E4);
for(0;_tmp9E5 != 0;_tmp9E5=_tmp9E5->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9E5->hd)->body);}
goto _LL420;_LL42F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9E6=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9E6->tag != 12)goto _LL431;else{_tmp9E7=_tmp9E6->f1;_tmp9E8=_tmp9E6->f2;}}_LL430:
 Cyc_Toc_decltypes_to_c(_tmp9E7);Cyc_Toc_stmttypes_to_c(_tmp9E8);goto _LL420;_LL431: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9E9=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9E9->tag != 14)goto _LL433;else{_tmp9EA=_tmp9E9->f1;_tmp9EB=(_tmp9E9->f2).f1;}}_LL432:
 Cyc_Toc_stmttypes_to_c(_tmp9EA);Cyc_Toc_exptypes_to_c(_tmp9EB);goto _LL420;_LL433: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9EC=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9EC->tag != 0)goto _LL435;}_LL434:
 goto _LL436;_LL435: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9ED=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9ED->tag != 6)goto _LL437;}_LL436:
 goto _LL438;_LL437: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9EE=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9EE->tag != 7)goto _LL439;}_LL438:
 goto _LL43A;_LL439: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9EF=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9EF->tag != 8)goto _LL43B;}_LL43A:
 goto _LL420;_LL43B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9F0=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9F0->tag != 11)goto _LL43D;}_LL43C:
 goto _LL43E;_LL43D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9F1=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9F1->tag != 13)goto _LL43F;}_LL43E:
 goto _LL440;_LL43F: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp9F2=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9F2->tag != 15)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp9F3=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9CF;if(_tmp9F3->tag != 16)goto _LL420;}_LL442:
{const char*_tmpE27;void*_tmpE26;(_tmpE26=0,Cyc_Tcutil_terr(s->loc,((_tmpE27="Cyclone statement in C code",_tag_dyneither(_tmpE27,sizeof(char),28))),_tag_dyneither(_tmpE26,sizeof(void*),0)));}
goto _LL420;_LL420:;}
# 5072
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE2A;void*_tmpE29;(_tmpE29=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE2A="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE2A,sizeof(char),29))),_tag_dyneither(_tmpE29,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp9F8=d->r;void*_tmp9F9=_tmp9F8;struct Cyc_Absyn_Vardecl*_tmp9FB;struct Cyc_Absyn_Fndecl*_tmp9FD;struct Cyc_Absyn_Aggrdecl*_tmpA02;struct Cyc_Absyn_Datatypedecl*_tmpA04;struct Cyc_Absyn_Enumdecl*_tmpA06;struct Cyc_Absyn_Typedefdecl*_tmpA08;struct Cyc_List_List*_tmpA0C;struct Cyc_List_List*_tmpA0E;struct Cyc_List_List*_tmpA10;struct Cyc_List_List*_tmpA12;_LL444: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9FA=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmp9FA->tag != 0)goto _LL446;else{_tmp9FB=_tmp9FA->f1;}}_LL445: {
# 5079
struct _tuple1*_tmpA13=_tmp9FB->name;
# 5081
if(_tmp9FB->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE2B;_tmpA13=((_tmpE2B=_cycalloc(sizeof(*_tmpE2B)),((_tmpE2B->f1=Cyc_Absyn_Rel_n(0),((_tmpE2B->f2=(*_tmpA13).f2,_tmpE2B))))));}
if(_tmp9FB->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9FB->initializer));else{
# 5087
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9FB->initializer));}}
# 5089
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE2E;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE2D;nv=Cyc_Toc_add_varmap(r,nv,_tmp9FB->name,Cyc_Absyn_varb_exp(_tmpA13,(void*)((_tmpE2D=_cycalloc(sizeof(*_tmpE2D)),((_tmpE2D[0]=((_tmpE2E.tag=1,((_tmpE2E.f1=_tmp9FB,_tmpE2E)))),_tmpE2D)))),0));}
_tmp9FB->name=_tmpA13;
_tmp9FB->sc=Cyc_Toc_scope_to_c(_tmp9FB->sc);
_tmp9FB->type=Cyc_Toc_typ_to_c(_tmp9FB->type);
{struct Cyc_List_List*_tmpE2F;Cyc_Toc_result_decls=((_tmpE2F=_cycalloc(sizeof(*_tmpE2F)),((_tmpE2F->hd=d,((_tmpE2F->tl=Cyc_Toc_result_decls,_tmpE2F))))));}
goto _LL443;}_LL446: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9FC=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmp9FC->tag != 1)goto _LL448;else{_tmp9FD=_tmp9FC->f1;}}_LL447: {
# 5096
struct _tuple1*_tmpA18=_tmp9FD->name;
# 5098
if(_tmp9FD->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE30;_tmpA18=((_tmpE30=_cycalloc(sizeof(*_tmpE30)),((_tmpE30->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE30->f2=(*_tmpA18).f2,_tmpE30))))));}
_tmp9FD->sc=Cyc_Absyn_Public;}
# 5102
nv=Cyc_Toc_add_varmap(r,nv,_tmp9FD->name,Cyc_Absyn_var_exp(_tmpA18,0));
_tmp9FD->name=_tmpA18;
Cyc_Toc_fndecl_to_c(nv,_tmp9FD,cinclude);
{struct Cyc_List_List*_tmpE31;Cyc_Toc_result_decls=((_tmpE31=_cycalloc(sizeof(*_tmpE31)),((_tmpE31->hd=d,((_tmpE31->tl=Cyc_Toc_result_decls,_tmpE31))))));}
goto _LL443;}_LL448: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9FE=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmp9FE->tag != 2)goto _LL44A;}_LL449:
 goto _LL44B;_LL44A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9FF=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmp9FF->tag != 3)goto _LL44C;}_LL44B: {
# 5109
const char*_tmpE34;void*_tmpE33;(_tmpE33=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE34="letdecl at toplevel",_tag_dyneither(_tmpE34,sizeof(char),20))),_tag_dyneither(_tmpE33,sizeof(void*),0)));}_LL44C: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA00=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA00->tag != 4)goto _LL44E;}_LL44D: {
# 5111
const char*_tmpE37;void*_tmpE36;(_tmpE36=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE37="region decl at toplevel",_tag_dyneither(_tmpE37,sizeof(char),24))),_tag_dyneither(_tmpE36,sizeof(void*),0)));}_LL44E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA01=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA01->tag != 5)goto _LL450;else{_tmpA02=_tmpA01->f1;}}_LL44F:
# 5115
 Cyc_Toc_aggrdecl_to_c(_tmpA02,1);
goto _LL443;_LL450: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA03=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA03->tag != 6)goto _LL452;else{_tmpA04=_tmpA03->f1;}}_LL451:
# 5118
 if(_tmpA04->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmpA04);else{
# 5121
Cyc_Toc_datatypedecl_to_c(_tmpA04);}
goto _LL443;_LL452: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA05=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA05->tag != 7)goto _LL454;else{_tmpA06=_tmpA05->f1;}}_LL453:
# 5124
 Cyc_Toc_enumdecl_to_c(_tmpA06);
{struct Cyc_List_List*_tmpE38;Cyc_Toc_result_decls=((_tmpE38=_cycalloc(sizeof(*_tmpE38)),((_tmpE38->hd=d,((_tmpE38->tl=Cyc_Toc_result_decls,_tmpE38))))));}
goto _LL443;_LL454: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA07=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA07->tag != 8)goto _LL456;else{_tmpA08=_tmpA07->f1;}}_LL455:
# 5128
 _tmpA08->name=_tmpA08->name;
_tmpA08->tvs=0;
if(_tmpA08->defn != 0)
_tmpA08->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA08->defn));else{
# 5133
enum Cyc_Absyn_KindQual _tmpA20=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA08->kind))->v)->kind;switch(_tmpA20){case Cyc_Absyn_BoxKind: _LL462:
 _tmpA08->defn=Cyc_Absyn_void_star_typ();break;default: _LL463:
 _tmpA08->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5138
{struct Cyc_List_List*_tmpE39;Cyc_Toc_result_decls=((_tmpE39=_cycalloc(sizeof(*_tmpE39)),((_tmpE39->hd=d,((_tmpE39->tl=Cyc_Toc_result_decls,_tmpE39))))));}
goto _LL443;_LL456: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA09=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA09->tag != 13)goto _LL458;}_LL457:
 goto _LL459;_LL458: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA0A=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA0A->tag != 14)goto _LL45A;}_LL459:
# 5142
 goto _LL443;_LL45A: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA0B=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA0B->tag != 9)goto _LL45C;else{_tmpA0C=_tmpA0B->f2;}}_LL45B:
 _tmpA0E=_tmpA0C;goto _LL45D;_LL45C: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA0D=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA0D->tag != 10)goto _LL45E;else{_tmpA0E=_tmpA0D->f2;}}_LL45D:
 _tmpA10=_tmpA0E;goto _LL45F;_LL45E: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA0F=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA0F->tag != 11)goto _LL460;else{_tmpA10=_tmpA0F->f1;}}_LL45F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA10,top,cinclude);goto _LL443;_LL460: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA11=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9F9;if(_tmpA11->tag != 12)goto _LL443;else{_tmpA12=_tmpA11->f1;}}_LL461:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA12,top,1);goto _LL443;_LL443:;};}
# 5149
return nv;}
# 5153
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA22=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA24;struct Cyc_Core_NewDynamicRegion _tmpA23=_tmpA22;_tmpA24=_tmpA23.key;{
struct Cyc_Toc_TocState*_tmpA25=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA24,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE3A;Cyc_Toc_toc_state=((_tmpE3A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE3A)),((_tmpE3A->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA24,((_tmpE3A->state=(struct Cyc_Toc_TocState*)_tmpA25,_tmpE3A))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE3B;Cyc_Toc_globals=_tag_dyneither(((_tmpE3B=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE3B[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE3B[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE3B[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE3B[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE3B[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE3B[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE3B[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE3B[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE3B[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE3B[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE3B[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE3B[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE3B[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE3B[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE3B[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE3B[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE3B[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE3B[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE3B[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE3B[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE3B[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE3B[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE3B[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE3B[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE3B[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE3B[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE3B[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE3B[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE3B[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE3B[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE3B[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE3B[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE3B[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE3B[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE3B[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE3B[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE3B[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE3B[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE3B[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE3B[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE3B[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE3B)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};};}
# 5209
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA28=*ts;struct Cyc_Xarray_Xarray*_tmpA2A;struct Cyc_Toc_TocState _tmpA29=_tmpA28;_tmpA2A=_tmpA29.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA2A);
return 0;}
# 5217
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA2B=_new_region("start");struct _RegionHandle*start=& _tmpA2B;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5226
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA2C=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA2E;struct Cyc_Toc_TocState*_tmpA2F;struct Cyc_Toc_TocStateWrap _tmpA2D=_tmpA2C;_tmpA2E=_tmpA2D.dyn;_tmpA2F=_tmpA2D.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA2E,_tmpA2F,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA2E);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5233
struct Cyc_List_List*_tmpA30=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA30;};
# 5219
;_pop_region(start);};}
