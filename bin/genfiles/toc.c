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
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 105
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 365
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 385
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 390
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 442 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 445
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 458
typedef void*Cyc_Absyn_funcparams_t;
# 461
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 522
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 538
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 540
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 548
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 555
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 573
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 702 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 737
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 765
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 920
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 927
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 947
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 950
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 965
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 967
extern void*Cyc_Absyn_sint_typ;
# 979
void*Cyc_Absyn_exn_typ();
# 990
extern void*Cyc_Absyn_bounds_one;
# 998
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1004
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1006
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1009
void*Cyc_Absyn_void_star_typ();
# 1011
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1017
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1074
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1079
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1085
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1091
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1101
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1104
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1108
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1110
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1115
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1124
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1167
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1170
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1180
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1184
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1200
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
# 137 "tcenv.h"
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
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 42
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 51
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 77
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 88
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 122
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
struct Cyc_Dict_Dict*_tmpA47;struct Cyc_Dict_Dict*_tmpA46;struct Cyc_Set_Set**_tmpA45;struct Cyc_List_List**_tmpA44;struct Cyc_Dict_Dict*_tmpA43;struct Cyc_List_List**_tmpA42;struct Cyc_Toc_TocState*_tmpA41;return(_tmpA41=_region_malloc(d,sizeof(*_tmpA41)),((_tmpA41->tuple_types=(
(_tmpA42=_region_malloc(d,sizeof(*_tmpA42)),((_tmpA42[0]=0,_tmpA42)))),((_tmpA41->aggrs_so_far=(
(_tmpA43=_region_malloc(d,sizeof(*_tmpA43)),((_tmpA43[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA43)))),((_tmpA41->abs_struct_types=(
(_tmpA44=_region_malloc(d,sizeof(*_tmpA44)),((_tmpA44[0]=0,_tmpA44)))),((_tmpA41->datatypes_so_far=(
(_tmpA45=_region_malloc(d,sizeof(*_tmpA45)),((_tmpA45[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA45)))),((_tmpA41->xdatatypes_so_far=(
(_tmpA46=_region_malloc(d,sizeof(*_tmpA46)),((_tmpA46[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA46)))),((_tmpA41->qvar_tags=(
(_tmpA47=_region_malloc(d,sizeof(*_tmpA47)),((_tmpA47[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA47)))),((_tmpA41->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA41)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
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
{struct Cyc_Toc_TocStateWrap _tmpA48;*ts=((_tmpA48.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA48.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA48))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA49;struct _tuple15 _tmp15=(_tmpA49.f1=_tmp12,((_tmpA49.f2=arg,_tmpA49)));
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
struct _tuple16 _tmpA4A;struct _tuple16 env=(_tmpA4A.f1=q,((_tmpA4A.f2=type_maker,_tmpA4A)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA4D;void*_tmpA4C;(_tmpA4C=0,Cyc_fprintf(Cyc_stderr,((_tmpA4D="\n",_tag_dyneither(_tmpA4D,sizeof(char),2))),_tag_dyneither(_tmpA4C,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA50;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA4F;(int)_throw((void*)((_tmpA4F=_cycalloc_atomic(sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=Cyc_Toc_Toc_Unimplemented,_tmpA50)),_tmpA4F)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA53;void*_tmpA52;(_tmpA52=0,Cyc_fprintf(Cyc_stderr,((_tmpA53="\n",_tag_dyneither(_tmpA53,sizeof(char),2))),_tag_dyneither(_tmpA52,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA56;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA55;(int)_throw((void*)((_tmpA55=_cycalloc_atomic(sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA56.tag=Cyc_Toc_Toc_Impossible,_tmpA56)),_tmpA55)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmpA57;skip_stmt_opt=((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=Cyc_Absyn_skip_stmt(0),_tmpA57))));}
return*skip_stmt_opt;}
# 316
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 319
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA5A;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA59;return(void*)((_tmpA59=_cycalloc(sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=13,((_tmpA5A.f1=t,((_tmpA5A.f2=e,((_tmpA5A.f3=0,((_tmpA5A.f4=Cyc_Absyn_No_coercion,_tmpA5A)))))))))),_tmpA59))));}
# 322
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA5D;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA5C;return(void*)((_tmpA5C=_cycalloc(sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=19,((_tmpA5D.f1=e,_tmpA5D)))),_tmpA5C))));}
# 325
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA60;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA5F;return(void*)((_tmpA5F=_cycalloc(sizeof(*_tmpA5F)),((_tmpA5F[0]=((_tmpA60.tag=22,((_tmpA60.f1=e1,((_tmpA60.f2=e2,_tmpA60)))))),_tmpA5F))));}
# 328
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA63;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA62;return(void*)((_tmpA62=_cycalloc(sizeof(*_tmpA62)),((_tmpA62[0]=((_tmpA63.tag=36,((_tmpA63.f1=s,_tmpA63)))),_tmpA62))));}
# 331
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA66;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA65;return(void*)((_tmpA65=_cycalloc(sizeof(*_tmpA65)),((_tmpA65[0]=((_tmpA66.tag=16,((_tmpA66.f1=t,_tmpA66)))),_tmpA65))));}
# 334
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA69;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA68;return(void*)((_tmpA68=_cycalloc(sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA69.tag=9,((_tmpA69.f1=e,((_tmpA69.f2=es,((_tmpA69.f3=0,((_tmpA69.f4=1,_tmpA69)))))))))),_tmpA68))));}
# 337
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA6C;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA6B;return(void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6C.tag=1,((_tmpA6C.f1=e,_tmpA6C)))),_tmpA6B))));}
# 340
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA6F;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA6E;return(void*)((_tmpA6E=_cycalloc(sizeof(*_tmpA6E)),((_tmpA6E[0]=((_tmpA6F.tag=2,((_tmpA6F.f1=s1,((_tmpA6F.f2=s2,_tmpA6F)))))),_tmpA6E))));}
# 343
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA72;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA71;return(void*)((_tmpA71=_cycalloc(sizeof(*_tmpA71)),((_tmpA71[0]=((_tmpA72.tag=5,((_tmpA72.f1=e1,((_tmpA72.f2=e2,((_tmpA72.f3=e3,_tmpA72)))))))),_tmpA71))));}
# 346
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA75;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA74;return(void*)((_tmpA74=_cycalloc(sizeof(*_tmpA74)),((_tmpA74[0]=((_tmpA75.tag=20,((_tmpA75.f1=e,((_tmpA75.f2=n,((_tmpA75.f3=0,((_tmpA75.f4=0,_tmpA75)))))))))),_tmpA74))));}
# 349
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA78;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA77;return(void*)((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA78.tag=21,((_tmpA78.f1=e,((_tmpA78.f2=n,((_tmpA78.f3=0,((_tmpA78.f4=0,_tmpA78)))))))))),_tmpA77))));}
# 352
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 355
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA7B;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA7A;return(void*)((_tmpA7A=_cycalloc(sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA7B.tag=35,((_tmpA7B.f1=tdopt,((_tmpA7B.f2=ds,_tmpA7B)))))),_tmpA7A))));}
# 357
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA7E;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA7D;return(void*)((_tmpA7D=_cycalloc(sizeof(*_tmpA7D)),((_tmpA7D[0]=((_tmpA7E.tag=8,((_tmpA7E.f1=v,((_tmpA7E.f2=s,_tmpA7E)))))),_tmpA7D))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA84;const char*_tmpA83;struct Cyc_Core_Impossible_exn_struct*_tmpA82;(int)_throw((void*)((_tmpA82=_cycalloc(sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA84.tag=Cyc_Core_Impossible,((_tmpA84.f1=((_tmpA83="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA83,sizeof(char),44))),_tmpA84)))),_tmpA82)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA91;const char*_tmpA90;void*_tmpA8F[1];struct Cyc_String_pa_PrintArg_struct _tmpA8E;struct Cyc_Core_Impossible_exn_struct*_tmpA8D;(int)_throw((void*)((_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D[0]=((_tmpA91.tag=Cyc_Core_Impossible,((_tmpA91.f1=(struct _dyneither_ptr)((_tmpA8E.tag=0,((_tmpA8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA8F[0]=& _tmpA8E,Cyc_aprintf(((_tmpA90="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA90,sizeof(char),67))),_tag_dyneither(_tmpA8F,sizeof(void*),1)))))))),_tmpA91)))),_tmpA8D)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA97;const char*_tmpA96;struct Cyc_Core_Impossible_exn_struct*_tmpA95;(int)_throw((void*)((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95[0]=((_tmpA97.tag=Cyc_Core_Impossible,((_tmpA97.f1=((_tmpA96="impossible type (not pointer)",_tag_dyneither(_tmpA96,sizeof(char),30))),_tmpA97)))),_tmpA95)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA9A;void*_tmpA99;(_tmpA99=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA9A="is_nullable",_tag_dyneither(_tmpA9A,sizeof(char),12))),_tag_dyneither(_tmpA99,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA9C;unsigned int _tmpA9B;struct _dyneither_ptr buf=(_tmpA9B=len,((_tmpA9C=_cyccalloc_atomic(sizeof(char),_tmpA9B),_tag_dyneither(_tmpA9C,sizeof(char),_tmpA9B))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11B=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp11D;struct _dyneither_ptr*_tmp11C=_tmp11B;_tmp11D=*_tmp11C;
{char _tmpA9F;char _tmpA9E;struct _dyneither_ptr _tmpA9D;(_tmpA9D=p,((_tmpA9E=*((char*)_check_dyneither_subscript(_tmpA9D,sizeof(char),0)),((_tmpA9F='_',((_get_dyneither_size(_tmpA9D,sizeof(char))== 1  && (_tmpA9E == '\000'  && _tmpA9F != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA9D.curr)=_tmpA9F)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp11D);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp11D));}
# 527
{char _tmpAA2;char _tmpAA1;struct _dyneither_ptr _tmpAA0;(_tmpAA0=p,((_tmpAA1=*((char*)_check_dyneither_subscript(_tmpAA0,sizeof(char),0)),((_tmpAA2='_',((_get_dyneither_size(_tmpAA0,sizeof(char))== 1  && (_tmpAA1 == '\000'  && _tmpAA2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpAA0.curr)=_tmpAA2)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp115);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpAA3;Cyc_strcpy(p,((_tmpAA3="_struct",_tag_dyneither(_tmpAA3,sizeof(char),8))));}
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
struct _tuple14*_tmpAA4;struct _tuple14*_tmp137=(_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->f1=_tmp12D,((_tmpAA4->f2=_tmp12E,_tmpAA4)))));
union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr _tmp13A;struct _tuple1*_tmp138=_tmp12D;_tmp139=_tmp138->f1;_tmp13A=*_tmp138->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13A,_tmp12E);
struct _dyneither_ptr*_tmpAA7;struct _tuple1*_tmpAA6;struct _tuple1*res=(_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->f1=_tmp139,((_tmpAA6->f2=((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=newvar,_tmpAA7)))),_tmpAA6)))));
*_tmp129=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp129,_tmp137,res);
return res;};}_LL48: _tmp136=_tmp134;_LL49:(void)_throw(_tmp136);_LL45:;}};};};}
# 561
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpAA8;struct _tuple14 env=(_tmpAA8.f1=fieldname,((_tmpAA8.f2=dtname,_tmpAA8)));
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
struct Cyc_Int_pa_PrintArg_struct _tmpAB0;void*_tmpAAF[1];const char*_tmpAAE;struct _dyneither_ptr*_tmpAAD;struct _dyneither_ptr*xname=(_tmpAAD=_cycalloc(sizeof(*_tmpAAD)),((_tmpAAD[0]=(struct _dyneither_ptr)((_tmpAB0.tag=1,((_tmpAB0.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAAF[0]=& _tmpAB0,Cyc_aprintf(((_tmpAAE="_tuple%d",_tag_dyneither(_tmpAAE,sizeof(char),9))),_tag_dyneither(_tmpAAF,sizeof(void*),1)))))))),_tmpAAD)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp143);
struct Cyc_List_List*_tmp14A=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpAB3;struct Cyc_List_List*_tmpAB2;_tmp14A=((_tmpAB2=_cycalloc(sizeof(*_tmpAB2)),((_tmpAB2->hd=((_tmpAB3=_cycalloc(sizeof(*_tmpAB3)),((_tmpAB3->name=Cyc_Absyn_fieldname(i),((_tmpAB3->tq=Cyc_Toc_mt_tq,((_tmpAB3->type=(void*)ts2->hd,((_tmpAB3->width=0,((_tmpAB3->attributes=0,((_tmpAB3->requires_clause=0,_tmpAB3)))))))))))))),((_tmpAB2->tl=_tmp14A,_tmpAB2))))));}}
_tmp14A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14A);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAB8;struct _tuple1*_tmpAB7;struct Cyc_Absyn_Aggrdecl*_tmpAB6;struct Cyc_Absyn_Aggrdecl*_tmp14D=(_tmpAB6=_cycalloc(sizeof(*_tmpAB6)),((_tmpAB6->kind=Cyc_Absyn_StructA,((_tmpAB6->sc=Cyc_Absyn_Public,((_tmpAB6->name=(
(_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->f1=Cyc_Absyn_Rel_n(0),((_tmpAB7->f2=xname,_tmpAB7)))))),((_tmpAB6->tvs=0,((_tmpAB6->impl=(
(_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->exist_vars=0,((_tmpAB8->rgn_po=0,((_tmpAB8->fields=_tmp14A,((_tmpAB8->tagged=0,_tmpAB8)))))))))),((_tmpAB6->attributes=0,_tmpAB6)))))))))))));
# 600
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpABE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpABD;struct Cyc_List_List*_tmpABC;Cyc_Toc_result_decls=((_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->hd=Cyc_Absyn_new_decl((void*)((_tmpABE=_cycalloc(sizeof(*_tmpABE)),((_tmpABE[0]=((_tmpABD.tag=5,((_tmpABD.f1=_tmp14D,_tmpABD)))),_tmpABE)))),0),((_tmpABC->tl=Cyc_Toc_result_decls,_tmpABC))))));}
{struct _tuple22*_tmpAC1;struct Cyc_List_List*_tmpAC0;*_tmp142=((_tmpAC0=_region_malloc(d,sizeof(*_tmpAC0)),((_tmpAC0->hd=((_tmpAC1=_region_malloc(d,sizeof(*_tmpAC1)),((_tmpAC1->f1=x,((_tmpAC1->f2=ts,_tmpAC1)))))),((_tmpAC0->tl=*_tmp142,_tmpAC0))))));}
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
struct Cyc_Int_pa_PrintArg_struct _tmpAC9;void*_tmpAC8[1];const char*_tmpAC7;struct _dyneither_ptr*_tmpAC6;struct _dyneither_ptr*xname=(_tmpAC6=_cycalloc(sizeof(*_tmpAC6)),((_tmpAC6[0]=(struct _dyneither_ptr)((_tmpAC9.tag=1,((_tmpAC9.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAC8[0]=& _tmpAC9,Cyc_aprintf(((_tmpAC7="_tuple%d",_tag_dyneither(_tmpAC7,sizeof(char),9))),_tag_dyneither(_tmpAC8,sizeof(void*),1)))))))),_tmpAC6)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp16C=0;
# 669
{struct _tuple25*_tmpACC;struct Cyc_List_List*_tmpACB;*_tmp15C=((_tmpACB=_region_malloc(d,sizeof(*_tmpACB)),((_tmpACB->hd=((_tmpACC=_region_malloc(d,sizeof(*_tmpACC)),((_tmpACC->f1=_tmp15D,((_tmpACC->f2=_tmp15F,((_tmpACC->f3=x,_tmpACC)))))))),((_tmpACB->tl=*_tmp15C,_tmpACB))))));}{
# 672
struct _RegionHandle _tmp16F=_new_region("r");struct _RegionHandle*r=& _tmp16F;_push_region(r);
{struct Cyc_List_List*_tmp170=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E,_tmp15F);
for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
struct Cyc_Absyn_Aggrfield*_tmp171=(struct Cyc_Absyn_Aggrfield*)_tmp160->hd;
void*t=_tmp171->type;
struct Cyc_List_List*atts=_tmp171->attributes;
# 680
if(_tmp160->tl == 0  && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpAD2;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpAD1;struct Cyc_List_List*_tmpAD0;atts=((_tmpAD0=_cycalloc(sizeof(*_tmpAD0)),((_tmpAD0->hd=(void*)((_tmpAD2=_cycalloc_atomic(sizeof(*_tmpAD2)),((_tmpAD2[0]=((_tmpAD1.tag=6,((_tmpAD1.f1=- 1,_tmpAD1)))),_tmpAD2)))),((_tmpAD0->tl=atts,_tmpAD0))))));}
# 683
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp170,t));
# 686
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val)){
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpAD8;struct Cyc_Absyn_ArrayInfo _tmpAD7;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpAD6;t=(void*)((_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6[0]=((_tmpAD8.tag=8,((_tmpAD8.f1=((_tmpAD7.elt_type=Cyc_Absyn_void_star_typ(),((_tmpAD7.tq=Cyc_Absyn_empty_tqual(0),((_tmpAD7.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpAD7.zero_term=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpAD7.zt_loc=0,_tmpAD7)))))))))),_tmpAD8)))),_tmpAD6))));}{
# 690
struct Cyc_Absyn_Aggrfield*_tmpADB;struct Cyc_List_List*_tmpADA;_tmp16C=((_tmpADA=_cycalloc(sizeof(*_tmpADA)),((_tmpADA->hd=((_tmpADB=_cycalloc(sizeof(*_tmpADB)),((_tmpADB->name=_tmp171->name,((_tmpADB->tq=Cyc_Toc_mt_tq,((_tmpADB->type=t,((_tmpADB->width=_tmp171->width,((_tmpADB->attributes=atts,((_tmpADB->requires_clause=0,_tmpADB)))))))))))))),((_tmpADA->tl=_tmp16C,_tmpADA))))));};}
# 692
_tmp16C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16C);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAE0;struct _tuple1*_tmpADF;struct Cyc_Absyn_Aggrdecl*_tmpADE;struct Cyc_Absyn_Aggrdecl*_tmp17A=(_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->kind=Cyc_Absyn_StructA,((_tmpADE->sc=Cyc_Absyn_Public,((_tmpADE->name=(
(_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->f1=Cyc_Absyn_Rel_n(0),((_tmpADF->f2=xname,_tmpADF)))))),((_tmpADE->tvs=0,((_tmpADE->impl=(
(_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0->exist_vars=0,((_tmpAE0->rgn_po=0,((_tmpAE0->fields=_tmp16C,((_tmpAE0->tagged=0,_tmpAE0)))))))))),((_tmpADE->attributes=0,_tmpADE)))))))))))));
# 700
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAE6;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAE5;struct Cyc_List_List*_tmpAE4;Cyc_Toc_result_decls=((_tmpAE4=_cycalloc(sizeof(*_tmpAE4)),((_tmpAE4->hd=Cyc_Absyn_new_decl((void*)((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6[0]=((_tmpAE5.tag=5,((_tmpAE5.f1=_tmp17A,_tmpAE5)))),_tmpAE6)))),0),((_tmpAE4->tl=Cyc_Toc_result_decls,_tmpAE4))))));}{
void*_tmp17E=x;_npop_handler(0);return _tmp17E;};};}
# 673
;_pop_region(r);};};}
# 704
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 708
struct _tuple23 _tmpAE7;struct _tuple23 env=(_tmpAE7.f1=struct_name,((_tmpAE7.f2=type_vars,((_tmpAE7.f3=type_args,((_tmpAE7.f4=fields,_tmpAE7)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 715
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp187=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpAF4;const char*_tmpAF3;void*_tmpAF2[1];struct Cyc_Int_pa_PrintArg_struct _tmpAF1;struct _tuple1*_tmpAF0;struct _tuple1*res=(_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0->f1=Cyc_Absyn_Loc_n,((_tmpAF0->f2=((_tmpAF4=_cycalloc(sizeof(*_tmpAF4)),((_tmpAF4[0]=(struct _dyneither_ptr)((_tmpAF1.tag=1,((_tmpAF1.f1=(unsigned int)_tmp187,((_tmpAF2[0]=& _tmpAF1,Cyc_aprintf(((_tmpAF3="_tmp%X",_tag_dyneither(_tmpAF3,sizeof(char),7))),_tag_dyneither(_tmpAF2,sizeof(void*),1)))))))),_tmpAF4)))),_tmpAF0)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 723
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp18D=*env;struct Cyc_Xarray_Xarray*_tmp18F;struct _tuple26 _tmp18E=_tmp18D;_tmp18F=(_tmp18E.f1)->temp_labels;{
int _tmp190=Cyc_Toc_fresh_label_counter ++;
if(_tmp190 < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp18F))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp18F,_tmp190);{
struct Cyc_Int_pa_PrintArg_struct _tmpAFC;void*_tmpAFB[1];const char*_tmpAFA;struct _dyneither_ptr*_tmpAF9;struct _dyneither_ptr*res=(_tmpAF9=_cycalloc(sizeof(*_tmpAF9)),((_tmpAF9[0]=(struct _dyneither_ptr)((_tmpAFC.tag=1,((_tmpAFC.f1=(unsigned int)_tmp190,((_tmpAFB[0]=& _tmpAFC,Cyc_aprintf(((_tmpAFA="_LL%X",_tag_dyneither(_tmpAFA,sizeof(char),6))),_tag_dyneither(_tmpAFB,sizeof(void*),1)))))))),_tmpAF9)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp18F,res)!= _tmp190){
const char*_tmpAFF;void*_tmpAFE;(_tmpAFE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFF="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpAFF,sizeof(char),43))),_tag_dyneither(_tmpAFE,sizeof(void*),0)));}
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
struct _tuple9*_tmpB00;return(_tmpB00=_cycalloc(sizeof(*_tmpB00)),((_tmpB00->f1=_tmp19A,((_tmpB00->f2=_tmp19B,((_tmpB00->f3=Cyc_Toc_typ_to_c(_tmp19C),_tmpB00)))))));};}
# 762
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp19E=*x;struct Cyc_Absyn_Tqual _tmp1A0;void*_tmp1A1;struct _tuple11 _tmp19F=_tmp19E;_tmp1A0=_tmp19F.f1;_tmp1A1=_tmp19F.f2;{
struct _tuple11*_tmpB01;return(_tmpB01=_cycalloc(sizeof(*_tmpB01)),((_tmpB01->f1=_tmp1A0,((_tmpB01->f2=Cyc_Toc_typ_to_c(_tmp1A1),_tmpB01)))));};}
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
struct Cyc_Absyn_Aggrfield*_tmpB02;return(_tmpB02=_cycalloc(sizeof(*_tmpB02)),((_tmpB02->name=f->name,((_tmpB02->tq=Cyc_Toc_mt_tq,((_tmpB02->type=
# 794
Cyc_Toc_typ_to_c(f->type),((_tmpB02->width=f->width,((_tmpB02->attributes=f->attributes,((_tmpB02->requires_clause=0,_tmpB02)))))))))))));}
# 799
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 801
return;}
# 804
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpB03;cs=((_tmpB03=_cycalloc(sizeof(*_tmpB03)),((_tmpB03[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpB03))));}
# 809
return*cs;}
# 811
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB04;r=((_tmpB04=_cycalloc(sizeof(*_tmpB04)),((_tmpB04[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpB04))));}
# 816
return*r;}
# 818
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpB05;r=((_tmpB05=_cycalloc(sizeof(*_tmpB05)),((_tmpB05[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpB05))));}
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
const char*_tmpB08;void*_tmpB07;(_tmpB07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB08="unresolved DatatypeFieldType",_tag_dyneither(_tmpB08,sizeof(char),29))),_tag_dyneither(_tmpB07,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1C2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B7;if(_tmp1C2->tag != 5)goto _LL71;else{_tmp1C3=(_tmp1C2->f1).elt_typ;_tmp1C4=(_tmp1C2->f1).elt_tq;_tmp1C5=((_tmp1C2->f1).ptr_atts).bounds;}}_LL70:
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
{struct Cyc_List_List*_tmpB09;_tmp200=((_tmpB09=_cycalloc(sizeof(*_tmpB09)),((_tmpB09->hd=(void*)_tmp1D3->hd,((_tmpB09->tl=_tmp200,_tmpB09))))));}goto _LLA4;_LLA4:;}{
# 888
struct Cyc_List_List*_tmp20A=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1D0);
if(_tmp1D2 != 0){
# 891
void*_tmp20B=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1D2->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpB0A;struct _tuple9*_tmp20C=(_tmpB0A=_cycalloc(sizeof(*_tmpB0A)),((_tmpB0A->f1=_tmp1D2->name,((_tmpB0A->f2=_tmp1D2->tq,((_tmpB0A->f3=_tmp20B,_tmpB0A)))))));
struct Cyc_List_List*_tmpB0B;_tmp20A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp20A,((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->hd=_tmp20C,((_tmpB0B->tl=0,_tmpB0B)))))));}{
# 895
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB11;struct Cyc_Absyn_FnInfo _tmpB10;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpB0F;return(void*)((_tmpB0F=_cycalloc(sizeof(*_tmpB0F)),((_tmpB0F[0]=((_tmpB11.tag=9,((_tmpB11.f1=((_tmpB10.tvars=0,((_tmpB10.effect=0,((_tmpB10.ret_tqual=_tmp1CE,((_tmpB10.ret_typ=Cyc_Toc_typ_to_c(_tmp1CF),((_tmpB10.args=_tmp20A,((_tmpB10.c_varargs=_tmp1D1,((_tmpB10.cyc_varargs=0,((_tmpB10.rgn_po=0,((_tmpB10.attributes=_tmp200,((_tmpB10.requires_clause=0,((_tmpB10.requires_relns=0,((_tmpB10.ensures_clause=0,((_tmpB10.ensures_relns=0,_tmpB10)))))))))))))))))))))))))),_tmpB11)))),_tmpB0F))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1D4=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D4->tag != 10)goto _LL7B;else{_tmp1D5=_tmp1D4->f1;}}_LL7A:
# 900
 _tmp1D5=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1D5);
return Cyc_Toc_add_tuple_type(_tmp1D5);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D6->tag != 12)goto _LL7D;else{_tmp1D7=_tmp1D6->f1;_tmp1D8=_tmp1D6->f2;}}_LL7C: {
# 905
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB14;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB13;return(void*)((_tmpB13=_cycalloc(sizeof(*_tmpB13)),((_tmpB13[0]=((_tmpB14.tag=12,((_tmpB14.f1=_tmp1D7,((_tmpB14.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1D8),_tmpB14)))))),_tmpB13))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D9=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B7;if(_tmp1D9->tag != 11)goto _LL7F;else{_tmp1DA=(_tmp1D9->f1).aggr_info;_tmp1DB=(_tmp1D9->f1).targs;}}_LL7E:
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
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB17;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB16;return(void*)((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16[0]=((_tmpB17.tag=17,((_tmpB17.f1=_tmp1E1,((_tmpB17.f2=0,((_tmpB17.f3=_tmp1E3,((_tmpB17.f4=0,_tmpB17)))))))))),_tmpB16))));}else{
return t;}}else{
# 942
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB1A;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB19;return(void*)((_tmpB19=_cycalloc(sizeof(*_tmpB19)),((_tmpB19[0]=((_tmpB1A.tag=17,((_tmpB1A.f1=_tmp1E1,((_tmpB1A.f2=0,((_tmpB1A.f3=_tmp1E3,((_tmpB1A.f4=Cyc_Toc_typ_to_c(_tmp1E4),_tmpB1A)))))))))),_tmpB19))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1E5=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1B7;if(_tmp1E5->tag != 19)goto _LL87;}_LL86:
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
const char*_tmpB1E;void*_tmpB1D[1];struct Cyc_String_pa_PrintArg_struct _tmpB1C;(_tmpB1C.tag=0,((_tmpB1C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB1D[0]=& _tmpB1C,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB1E="atomic_typ:  bad type %s",_tag_dyneither(_tmpB1E,sizeof(char),25))),_tag_dyneither(_tmpB1D,sizeof(void*),1)))))));}_LLBD:;}
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
const char*_tmpB21;void*_tmpB20;(_tmpB20=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB21="is_poly_field: type missing fields",_tag_dyneither(_tmpB21,sizeof(char),35))),_tag_dyneither(_tmpB20,sizeof(void*),0)));}
_tmp250=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp251->impl))->fields;goto _LLF5;}_LLF4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp24F=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C;if(_tmp24F->tag != 12)goto _LLF6;else{_tmp250=_tmp24F->f2;}}_LLF5: {
# 1058
struct Cyc_Absyn_Aggrfield*_tmp254=Cyc_Absyn_lookup_field(_tmp250,f);
if(_tmp254 == 0){
const char*_tmpB25;void*_tmpB24[1];struct Cyc_String_pa_PrintArg_struct _tmpB23;(_tmpB23.tag=0,((_tmpB23.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB24[0]=& _tmpB23,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB25="is_poly_field: bad field %s",_tag_dyneither(_tmpB25,sizeof(char),28))),_tag_dyneither(_tmpB24,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp254->type);}_LLF6:;_LLF7: {
const char*_tmpB29;void*_tmpB28[1];struct Cyc_String_pa_PrintArg_struct _tmpB27;(_tmpB27.tag=0,((_tmpB27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB28[0]=& _tmpB27,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB29="is_poly_field: bad type %s",_tag_dyneither(_tmpB29,sizeof(char),27))),_tag_dyneither(_tmpB28,sizeof(void*),1)))))));}_LLF1:;}
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
const char*_tmpB2D;void*_tmpB2C[1];struct Cyc_String_pa_PrintArg_struct _tmpB2B;(_tmpB2B.tag=0,((_tmpB2B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp261->topt))),((_tmpB2C[0]=& _tmpB2B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2D="is_poly_project: bad type %s",_tag_dyneither(_tmpB2D,sizeof(char),29))),_tag_dyneither(_tmpB2C,sizeof(void*),1)))))));}_LLFF:;}_LLFD:;_LLFE:
# 1080
 return 0;_LLF8:;}
# 1085
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2E;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB2E=_cycalloc(sizeof(*_tmpB2E)),((_tmpB2E->hd=s,((_tmpB2E->tl=0,_tmpB2E)))))),0);}
# 1089
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB2F;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB2F=_cycalloc(sizeof(*_tmpB2F)),((_tmpB2F->hd=s,((_tmpB2F->tl=0,_tmpB2F)))))),0);}
# 1093
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1099
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB30[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB30[1]=s,((_tmpB30[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB30,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1103
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB31[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB31[1]=n,((_tmpB31[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB31,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1107
struct Cyc_Absyn_Exp*_tmpB32[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB32[1]=n,((_tmpB32[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB32,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB33[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB33[2]=n,((_tmpB33[1]=s,((_tmpB33[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB33,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1115
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB34;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB34=_cycalloc(sizeof(*_tmpB34)),((_tmpB34->hd=e,((_tmpB34->tl=0,_tmpB34)))))),0);}
# 1119
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB35[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB35,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
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
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB3B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB3A;struct Cyc_List_List*_tmpB39;Cyc_Toc_result_decls=((_tmpB39=_cycalloc(sizeof(*_tmpB39)),((_tmpB39->hd=Cyc_Absyn_new_decl((void*)((_tmpB3B=_cycalloc(sizeof(*_tmpB3B)),((_tmpB3B[0]=((_tmpB3A.tag=0,((_tmpB3A.f1=vd,_tmpB3A)))),_tmpB3B)))),0),((_tmpB39->tl=Cyc_Toc_result_decls,_tmpB39))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1149
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1151
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1153
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB42;struct _tuple19*_tmpB41;struct _tuple19*_tmpB40;struct _tuple19*_tmpB3F[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB3F[2]=(
# 1156
(_tmpB40=_cycalloc(sizeof(*_tmpB40)),((_tmpB40->f1=0,((_tmpB40->f2=xplussz,_tmpB40)))))),((_tmpB3F[1]=(
# 1155
(_tmpB41=_cycalloc(sizeof(*_tmpB41)),((_tmpB41->f1=0,((_tmpB41->f2=xexp,_tmpB41)))))),((_tmpB3F[0]=(
# 1154
(_tmpB42=_cycalloc(sizeof(*_tmpB42)),((_tmpB42->f1=0,((_tmpB42->f2=xexp,_tmpB42)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB3F,sizeof(struct _tuple19*),3)))))))),0);}
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
int*_tmpB4A;struct Cyc_Absyn_Exp**_tmpB49;int*_tmpB48;struct Cyc_Toc_Env*_tmpB47;return(_tmpB47=_region_malloc(r,sizeof(*_tmpB47)),((_tmpB47->break_lab=(struct _dyneither_ptr**)0,((_tmpB47->continue_lab=(struct _dyneither_ptr**)0,((_tmpB47->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB47->varmap=(struct Cyc_Dict_Dict)
# 1221
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB47->toplevel=(int)1,((_tmpB47->in_lhs=(int*)(
# 1223
(_tmpB48=_region_malloc(r,sizeof(*_tmpB48)),((_tmpB48[0]=0,_tmpB48)))),((_tmpB47->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB47->struct_info).varsizeexp=((_tmpB49=_cycalloc(sizeof(*_tmpB49)),((_tmpB49[0]=0,_tmpB49)))),(((_tmpB47->struct_info).lhs_exp=0,_tmpB47->struct_info)))),((_tmpB47->in_sizeof=(int*)(
(_tmpB4A=_region_malloc(r,sizeof(*_tmpB4A)),((_tmpB4A[0]=0,_tmpB4A)))),((_tmpB47->rgn=(struct _RegionHandle*)r,_tmpB47)))))))))))))))))));}
# 1229
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp288;struct _dyneither_ptr**_tmp289;struct Cyc_Toc_FallthruInfo*_tmp28A;struct Cyc_Dict_Dict _tmp28B;int _tmp28C;int*_tmp28D;struct Cyc_Toc_StructInfo _tmp28E;int*_tmp28F;struct Cyc_Toc_Env*_tmp287=e;_tmp288=_tmp287->break_lab;_tmp289=_tmp287->continue_lab;_tmp28A=_tmp287->fallthru_info;_tmp28B=_tmp287->varmap;_tmp28C=_tmp287->toplevel;_tmp28D=_tmp287->in_lhs;_tmp28E=_tmp287->struct_info;_tmp28F=_tmp287->in_sizeof;{
struct Cyc_Toc_Env*_tmpB4B;return(_tmpB4B=_region_malloc(r,sizeof(*_tmpB4B)),((_tmpB4B->break_lab=(struct _dyneither_ptr**)_tmp288,((_tmpB4B->continue_lab=(struct _dyneither_ptr**)_tmp289,((_tmpB4B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28A,((_tmpB4B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28B),((_tmpB4B->toplevel=(int)_tmp28C,((_tmpB4B->in_lhs=(int*)_tmp28D,((_tmpB4B->struct_info=(struct Cyc_Toc_StructInfo)_tmp28E,((_tmpB4B->in_sizeof=(int*)_tmp28F,((_tmpB4B->rgn=(struct _RegionHandle*)r,_tmpB4B)))))))))))))))))));};}
# 1234
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp292;struct _dyneither_ptr**_tmp293;struct Cyc_Toc_FallthruInfo*_tmp294;struct Cyc_Dict_Dict _tmp295;int _tmp296;int*_tmp297;struct Cyc_Toc_StructInfo _tmp298;int*_tmp299;struct Cyc_Toc_Env*_tmp291=e;_tmp292=_tmp291->break_lab;_tmp293=_tmp291->continue_lab;_tmp294=_tmp291->fallthru_info;_tmp295=_tmp291->varmap;_tmp296=_tmp291->toplevel;_tmp297=_tmp291->in_lhs;_tmp298=_tmp291->struct_info;_tmp299=_tmp291->in_sizeof;{
struct Cyc_Toc_Env*_tmpB4C;return(_tmpB4C=_region_malloc(r,sizeof(*_tmpB4C)),((_tmpB4C->break_lab=(struct _dyneither_ptr**)_tmp292,((_tmpB4C->continue_lab=(struct _dyneither_ptr**)_tmp293,((_tmpB4C->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp294,((_tmpB4C->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp295),((_tmpB4C->toplevel=(int)0,((_tmpB4C->in_lhs=(int*)_tmp297,((_tmpB4C->struct_info=(struct Cyc_Toc_StructInfo)_tmp298,((_tmpB4C->in_sizeof=(int*)_tmp299,((_tmpB4C->rgn=(struct _RegionHandle*)r,_tmpB4C)))))))))))))))))));};}
# 1238
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp29C;struct _dyneither_ptr**_tmp29D;struct Cyc_Toc_FallthruInfo*_tmp29E;struct Cyc_Dict_Dict _tmp29F;int _tmp2A0;int*_tmp2A1;struct Cyc_Toc_StructInfo _tmp2A2;int*_tmp2A3;struct Cyc_Toc_Env*_tmp29B=e;_tmp29C=_tmp29B->break_lab;_tmp29D=_tmp29B->continue_lab;_tmp29E=_tmp29B->fallthru_info;_tmp29F=_tmp29B->varmap;_tmp2A0=_tmp29B->toplevel;_tmp2A1=_tmp29B->in_lhs;_tmp2A2=_tmp29B->struct_info;_tmp2A3=_tmp29B->in_sizeof;{
struct Cyc_Toc_Env*_tmpB4D;return(_tmpB4D=_region_malloc(r,sizeof(*_tmpB4D)),((_tmpB4D->break_lab=(struct _dyneither_ptr**)_tmp29C,((_tmpB4D->continue_lab=(struct _dyneither_ptr**)_tmp29D,((_tmpB4D->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp29E,((_tmpB4D->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp29F),((_tmpB4D->toplevel=(int)1,((_tmpB4D->in_lhs=(int*)_tmp2A1,((_tmpB4D->struct_info=(struct Cyc_Toc_StructInfo)_tmp2A2,((_tmpB4D->in_sizeof=(int*)_tmp2A3,((_tmpB4D->rgn=(struct _RegionHandle*)r,_tmpB4D)))))))))))))))))));};}
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
struct Cyc_Toc_Env*_tmpB4E;return(_tmpB4E=_region_malloc(r,sizeof(*_tmpB4E)),((_tmpB4E->break_lab=(struct _dyneither_ptr**)_tmp2AD,((_tmpB4E->continue_lab=(struct _dyneither_ptr**)_tmp2AE,((_tmpB4E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2AF,((_tmpB4E->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B0),((_tmpB4E->toplevel=(int)_tmp2B1,((_tmpB4E->in_lhs=(int*)_tmp2B2,((_tmpB4E->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB4E->struct_info).varsizeexp=_tmp2B3,(((_tmpB4E->struct_info).lhs_exp=exp,_tmpB4E->struct_info)))),((_tmpB4E->in_sizeof=(int*)_tmp2B4,((_tmpB4E->rgn=(struct _RegionHandle*)r,_tmpB4E)))))))))))))))))));};}
# 1265
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2B6=(*x).f1;union Cyc_Absyn_Nmspace _tmp2B7=_tmp2B6;_LL10C: if((_tmp2B7.Rel_n).tag != 1)goto _LL10E;_LL10D: {
# 1268
const char*_tmpB52;void*_tmpB51[1];struct Cyc_String_pa_PrintArg_struct _tmpB50;(_tmpB50.tag=0,((_tmpB50.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB51[0]=& _tmpB50,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB52="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB52,sizeof(char),30))),_tag_dyneither(_tmpB51,sizeof(void*),1)))))));}_LL10E:;_LL10F:
 goto _LL10B;_LL10B:;}{
# 1271
struct _dyneither_ptr**_tmp2BC;struct _dyneither_ptr**_tmp2BD;struct Cyc_Toc_FallthruInfo*_tmp2BE;struct Cyc_Dict_Dict _tmp2BF;int _tmp2C0;int*_tmp2C1;struct Cyc_Toc_StructInfo _tmp2C2;int*_tmp2C3;struct Cyc_Toc_Env*_tmp2BB=e;_tmp2BC=_tmp2BB->break_lab;_tmp2BD=_tmp2BB->continue_lab;_tmp2BE=_tmp2BB->fallthru_info;_tmp2BF=_tmp2BB->varmap;_tmp2C0=_tmp2BB->toplevel;_tmp2C1=_tmp2BB->in_lhs;_tmp2C2=_tmp2BB->struct_info;_tmp2C3=_tmp2BB->in_sizeof;{
struct Cyc_Dict_Dict _tmp2C4=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2BF),x,y);
struct Cyc_Toc_Env*_tmpB53;return(_tmpB53=_region_malloc(r,sizeof(*_tmpB53)),((_tmpB53->break_lab=(struct _dyneither_ptr**)_tmp2BC,((_tmpB53->continue_lab=(struct _dyneither_ptr**)_tmp2BD,((_tmpB53->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2BE,((_tmpB53->varmap=(struct Cyc_Dict_Dict)_tmp2C4,((_tmpB53->toplevel=(int)_tmp2C0,((_tmpB53->in_lhs=(int*)_tmp2C1,((_tmpB53->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C2,((_tmpB53->in_sizeof=(int*)_tmp2C3,((_tmpB53->rgn=(struct _RegionHandle*)r,_tmpB53)))))))))))))))))));};};}
# 1278
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2C7;struct _dyneither_ptr**_tmp2C8;struct Cyc_Toc_FallthruInfo*_tmp2C9;struct Cyc_Dict_Dict _tmp2CA;int _tmp2CB;int*_tmp2CC;struct Cyc_Toc_StructInfo _tmp2CD;int*_tmp2CE;struct Cyc_Toc_Env*_tmp2C6=e;_tmp2C7=_tmp2C6->break_lab;_tmp2C8=_tmp2C6->continue_lab;_tmp2C9=_tmp2C6->fallthru_info;_tmp2CA=_tmp2C6->varmap;_tmp2CB=_tmp2C6->toplevel;_tmp2CC=_tmp2C6->in_lhs;_tmp2CD=_tmp2C6->struct_info;_tmp2CE=_tmp2C6->in_sizeof;{
struct Cyc_Toc_Env*_tmpB54;return(_tmpB54=_region_malloc(r,sizeof(*_tmpB54)),((_tmpB54->break_lab=(struct _dyneither_ptr**)0,((_tmpB54->continue_lab=(struct _dyneither_ptr**)0,((_tmpB54->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C9,((_tmpB54->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CA),((_tmpB54->toplevel=(int)_tmp2CB,((_tmpB54->in_lhs=(int*)_tmp2CC,((_tmpB54->struct_info=(struct Cyc_Toc_StructInfo)_tmp2CD,((_tmpB54->in_sizeof=(int*)_tmp2CE,((_tmpB54->rgn=(struct _RegionHandle*)r,_tmpB54)))))))))))))))))));};}
# 1284
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1289
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB55;fallthru_vars=((_tmpB55=_region_malloc(r,sizeof(*_tmpB55)),((_tmpB55->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB55->tl=fallthru_vars,_tmpB55))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2D2;struct _dyneither_ptr**_tmp2D3;struct Cyc_Toc_FallthruInfo*_tmp2D4;struct Cyc_Dict_Dict _tmp2D5;int _tmp2D6;int*_tmp2D7;struct Cyc_Toc_StructInfo _tmp2D8;int*_tmp2D9;struct Cyc_Toc_Env*_tmp2D1=e;_tmp2D2=_tmp2D1->break_lab;_tmp2D3=_tmp2D1->continue_lab;_tmp2D4=_tmp2D1->fallthru_info;_tmp2D5=_tmp2D1->varmap;_tmp2D6=_tmp2D1->toplevel;_tmp2D7=_tmp2D1->in_lhs;_tmp2D8=_tmp2D1->struct_info;_tmp2D9=_tmp2D1->in_sizeof;{
struct Cyc_Dict_Dict _tmp2DB;struct Cyc_Toc_Env*_tmp2DA=next_case_env;_tmp2DB=_tmp2DA->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB56;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB56=_region_malloc(r,sizeof(*_tmpB56)),((_tmpB56->label=fallthru_l,((_tmpB56->binders=fallthru_vars,((_tmpB56->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DB),_tmpB56)))))));
struct _dyneither_ptr**_tmpB59;struct Cyc_Toc_Env*_tmpB58;return(_tmpB58=_region_malloc(r,sizeof(*_tmpB58)),((_tmpB58->break_lab=(struct _dyneither_ptr**)((_tmpB59=_region_malloc(r,sizeof(*_tmpB59)),((_tmpB59[0]=break_l,_tmpB59)))),((_tmpB58->continue_lab=(struct _dyneither_ptr**)_tmp2D3,((_tmpB58->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB58->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D5),((_tmpB58->toplevel=(int)_tmp2D6,((_tmpB58->in_lhs=(int*)_tmp2D7,((_tmpB58->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D8,((_tmpB58->in_sizeof=(int*)_tmp2D9,((_tmpB58->rgn=(struct _RegionHandle*)r,_tmpB58)))))))))))))))))));};};};}
# 1301
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1304
struct _dyneither_ptr**_tmp2E0;struct _dyneither_ptr**_tmp2E1;struct Cyc_Toc_FallthruInfo*_tmp2E2;struct Cyc_Dict_Dict _tmp2E3;int _tmp2E4;int*_tmp2E5;struct Cyc_Toc_StructInfo _tmp2E6;int*_tmp2E7;struct Cyc_Toc_Env*_tmp2DF=e;_tmp2E0=_tmp2DF->break_lab;_tmp2E1=_tmp2DF->continue_lab;_tmp2E2=_tmp2DF->fallthru_info;_tmp2E3=_tmp2DF->varmap;_tmp2E4=_tmp2DF->toplevel;_tmp2E5=_tmp2DF->in_lhs;_tmp2E6=_tmp2DF->struct_info;_tmp2E7=_tmp2DF->in_sizeof;{
struct _dyneither_ptr**_tmpB5C;struct Cyc_Toc_Env*_tmpB5B;return(_tmpB5B=_region_malloc(r,sizeof(*_tmpB5B)),((_tmpB5B->break_lab=(struct _dyneither_ptr**)((_tmpB5C=_region_malloc(r,sizeof(*_tmpB5C)),((_tmpB5C[0]=break_l,_tmpB5C)))),((_tmpB5B->continue_lab=(struct _dyneither_ptr**)_tmp2E1,((_tmpB5B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB5B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E3),((_tmpB5B->toplevel=(int)_tmp2E4,((_tmpB5B->in_lhs=(int*)_tmp2E5,((_tmpB5B->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E6,((_tmpB5B->in_sizeof=(int*)_tmp2E7,((_tmpB5B->rgn=(struct _RegionHandle*)r,_tmpB5B)))))))))))))))))));};}
# 1311
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1314
struct _dyneither_ptr**_tmp2EB;struct _dyneither_ptr**_tmp2EC;struct Cyc_Toc_FallthruInfo*_tmp2ED;struct Cyc_Dict_Dict _tmp2EE;int _tmp2EF;int*_tmp2F0;struct Cyc_Toc_StructInfo _tmp2F1;int*_tmp2F2;struct Cyc_Toc_Env*_tmp2EA=e;_tmp2EB=_tmp2EA->break_lab;_tmp2EC=_tmp2EA->continue_lab;_tmp2ED=_tmp2EA->fallthru_info;_tmp2EE=_tmp2EA->varmap;_tmp2EF=_tmp2EA->toplevel;_tmp2F0=_tmp2EA->in_lhs;_tmp2F1=_tmp2EA->struct_info;_tmp2F2=_tmp2EA->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB5F;struct Cyc_Toc_Env*_tmpB5E;return(_tmpB5E=_region_malloc(r,sizeof(*_tmpB5E)),((_tmpB5E->break_lab=(struct _dyneither_ptr**)0,((_tmpB5E->continue_lab=(struct _dyneither_ptr**)_tmp2EC,((_tmpB5E->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB5F=_region_malloc(r,sizeof(*_tmpB5F)),((_tmpB5F->label=next_l,((_tmpB5F->binders=0,((_tmpB5F->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB5F)))))))),((_tmpB5E->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2EE),((_tmpB5E->toplevel=(int)_tmp2EF,((_tmpB5E->in_lhs=(int*)_tmp2F0,((_tmpB5E->struct_info=(struct Cyc_Toc_StructInfo)_tmp2F1,((_tmpB5E->in_sizeof=(int*)_tmp2F2,((_tmpB5E->rgn=(struct _RegionHandle*)r,_tmpB5E)))))))))))))))))));};}
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
{const char*_tmpB62;void*_tmpB61;(_tmpB61=0,Cyc_Tcutil_terr(e->loc,((_tmpB62="dereference of NULL pointer",_tag_dyneither(_tmpB62,sizeof(char),28))),_tag_dyneither(_tmpB61,sizeof(void*),0)));}
return 0;_LL117: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2FA=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F6;if(_tmp2FA->tag != Cyc_Absyn_EmptyAnnot)goto _LL119;}_LL118:
# 1348
 return 1;_LL119:;_LL11A: {
const char*_tmpB65;void*_tmpB64;(_tmpB64=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB65="need_null_check",_tag_dyneither(_tmpB65,sizeof(char),16))),_tag_dyneither(_tmpB64,sizeof(void*),0)));}_LL110:;}
# 1353
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2FF=e->annot;void*_tmp300=_tmp2FF;struct Cyc_List_List*_tmp302;struct Cyc_List_List*_tmp304;_LL11C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp301=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp301->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL11E;else{_tmp302=_tmp301->f1;}}_LL11D:
 return _tmp302;_LL11E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp303=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp303->tag != Cyc_CfFlowInfo_NotZero)goto _LL120;else{_tmp304=_tmp303->f1;}}_LL11F:
 return _tmp304;_LL120: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp305=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp305->tag != Cyc_CfFlowInfo_IsZero)goto _LL122;}_LL121:
# 1358
{const char*_tmpB68;void*_tmpB67;(_tmpB67=0,Cyc_Tcutil_terr(e->loc,((_tmpB68="dereference of NULL pointer",_tag_dyneither(_tmpB68,sizeof(char),28))),_tag_dyneither(_tmpB67,sizeof(void*),0)));}
return 0;_LL122: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp306=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp300;if(_tmp306->tag != Cyc_Absyn_EmptyAnnot)goto _LL124;}_LL123:
 return 0;_LL124:;_LL125: {
const char*_tmpB6B;void*_tmpB6A;(_tmpB6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6B="get_relns",_tag_dyneither(_tmpB6B,sizeof(char),10))),_tag_dyneither(_tmpB6A,sizeof(void*),0)));}_LL11B:;}static char _tmp310[8]="*bogus*";
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
if(e->topt == 0){const char*_tmpB6E;void*_tmpB6D;(_tmpB6D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6E="Missing type in primop ",_tag_dyneither(_tmpB6E,sizeof(char),24))),_tag_dyneither(_tmpB6D,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1450
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB71;void*_tmpB70;(_tmpB70=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB71="Missing type in primop ",_tag_dyneither(_tmpB71,sizeof(char),24))),_tag_dyneither(_tmpB70,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1454
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB72;return(_tmpB72=_cycalloc(sizeof(*_tmpB72)),((_tmpB72->f1=Cyc_Toc_mt_tq,((_tmpB72->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB72)))));}
# 1457
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB73;return(_tmpB73=_cycalloc(sizeof(*_tmpB73)),((_tmpB73->f1=0,((_tmpB73->f2=e,_tmpB73)))));};}
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
const char*_tmpB76;void*_tmpB75;(_tmpB75=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB76="bogus x to make_struct",_tag_dyneither(_tmpB76,sizeof(char),23))),_tag_dyneither(_tmpB75,sizeof(void*),0)));}_LL132:;}else{
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
const char*_tmpB7A;void*_tmpB79[1];struct Cyc_String_pa_PrintArg_struct _tmpB78;(_tmpB78.tag=0,((_tmpB78.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB79[0]=& _tmpB78,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB7A="array_elt_type called on %s",_tag_dyneither(_tmpB7A,sizeof(char),28))),_tag_dyneither(_tmpB79,sizeof(void*),1)))))));}_LL13E:;}
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
if(_tmp33A->tl != 0){const char*_tmpB7D;void*_tmpB7C;(_tmpB7C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB7D="multiple designators in array",_tag_dyneither(_tmpB7D,sizeof(char),30))),_tag_dyneither(_tmpB7C,sizeof(void*),0)));}{
void*_tmp33E=(void*)_tmp33A->hd;
void*_tmp33F=_tmp33E;struct Cyc_Absyn_Exp*_tmp341;_LL144: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp340=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33F;if(_tmp340->tag != 0)goto _LL146;else{_tmp341=_tmp340->f1;}}_LL145:
# 1560
 Cyc_Toc_exp_to_c(nv,_tmp341);
e_index=_tmp341;
goto _LL143;_LL146: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp342=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33F;if(_tmp342->tag != 1)goto _LL143;}_LL147: {
const char*_tmpB80;void*_tmpB7F;(_tmpB7F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB80="field name designators in array",_tag_dyneither(_tmpB80,sizeof(char),32))),_tag_dyneither(_tmpB7F,sizeof(void*),0)));}_LL143:;};}{
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
const char*_tmpB83;void*_tmpB82;(_tmpB82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB83="init_comprehension passed a bad type!",_tag_dyneither(_tmpB83,sizeof(char),38))),_tag_dyneither(_tmpB82,sizeof(void*),0)));}_LL153:;}{
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
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB86;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB85;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp352,Cyc_Absyn_varb_exp(_tmp352,(void*)((_tmpB85=_cycalloc(sizeof(*_tmpB85)),((_tmpB85[0]=((_tmpB86.tag=4,((_tmpB86.f1=vd,_tmpB86)))),_tmpB85)))),0));
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
const char*_tmpB89;void*_tmpB88;(_tmpB88=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB89="empty designator list",_tag_dyneither(_tmpB89,sizeof(char),22))),_tag_dyneither(_tmpB88,sizeof(void*),0)));}
if(_tmp372->tl != 0){
const char*_tmpB8C;void*_tmpB8B;(_tmpB8B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8C="too many designators in anonymous struct",_tag_dyneither(_tmpB8C,sizeof(char),41))),_tag_dyneither(_tmpB8B,sizeof(void*),0)));}{
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
const char*_tmpB8F;void*_tmpB8E;(_tmpB8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8F="array designator in struct",_tag_dyneither(_tmpB8F,sizeof(char),27))),_tag_dyneither(_tmpB8E,sizeof(void*),0)));}_LL168:;};}}
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
struct Cyc_String_pa_PrintArg_struct _tmpB97;void*_tmpB96[1];const char*_tmpB95;void*_tmpB94;(_tmpB94=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB97.tag=0,((_tmpB97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB96[0]=& _tmpB97,Cyc_aprintf(((_tmpB95="get_member_offset %s failed",_tag_dyneither(_tmpB95,sizeof(char),28))),_tag_dyneither(_tmpB96,sizeof(void*),1)))))))),_tag_dyneither(_tmpB94,sizeof(void*),0)));};}
# 1751
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp3A4=Cyc_Tcutil_compress(*typ);void*_tmp3A5=_tmp3A4;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A8;struct Cyc_Absyn_Exp*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3AA;unsigned int _tmp3AB;_LL182: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A5;if(_tmp3A6->tag != 8)goto _LL184;else{_tmp3A7=(_tmp3A6->f1).elt_type;_tmp3A8=(_tmp3A6->f1).tq;_tmp3A9=(_tmp3A6->f1).num_elts;_tmp3AA=(_tmp3A6->f1).zero_term;_tmp3AB=(_tmp3A6->f1).zt_loc;}}_LL183:
# 1756
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3A9))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB9D;struct Cyc_Absyn_ArrayInfo _tmpB9C;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB9B;*typ=(void*)((_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B[0]=((_tmpB9D.tag=8,((_tmpB9D.f1=((_tmpB9C.elt_type=_tmp3A7,((_tmpB9C.tq=_tmp3A8,((_tmpB9C.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB9C.zero_term=_tmp3AA,((_tmpB9C.zt_loc=_tmp3AB,_tmpB9C)))))))))),_tmpB9D)))),_tmpB9B))));}
return 1;}
# 1760
goto _LL181;_LL184:;_LL185:
 goto _LL181;_LL181:;}
# 1763
return 0;}
# 1766
static int Cyc_Toc_is_array_type(void*t){
void*_tmp3AF=Cyc_Tcutil_compress(t);void*_tmp3B0=_tmp3AF;_LL187: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3B1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B0;if(_tmp3B1->tag != 8)goto _LL189;}_LL188:
 return 1;_LL189:;_LL18A:
 return 0;_LL186:;}
# 1773
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1775
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1779
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3B2=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_List_List*_tmp3B3=_tmp3B2->attributes;
void*_tmp3B4=_tmp3B2->type;
if((!Cyc_Toc_is_array_type(_tmp3B4),Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp3B4)))){
struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpBA3;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpBA2;struct Cyc_List_List*_tmpBA1;_tmp3B3=((_tmpBA1=_cycalloc(sizeof(*_tmpBA1)),((_tmpBA1->hd=(void*)((_tmpBA3=_cycalloc_atomic(sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA2.tag=6,((_tmpBA2.f1=- 1,_tmpBA2)))),_tmpBA3)))),((_tmpBA1->tl=_tmp3B3,_tmpBA1))))));}{
struct Cyc_Absyn_Aggrfield*_tmpBA4;struct Cyc_Absyn_Aggrfield*_tmp3B8=(_tmpBA4=_cycalloc(sizeof(*_tmpBA4)),((_tmpBA4->name=_tmp3B2->name,((_tmpBA4->tq=Cyc_Toc_mt_tq,((_tmpBA4->type=new_field_type,((_tmpBA4->width=_tmp3B2->width,((_tmpBA4->attributes=_tmp3B3,((_tmpBA4->requires_clause=0,_tmpBA4)))))))))))));
# 1791
struct Cyc_List_List*_tmpBA5;new_fields=((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5->hd=_tmp3B8,((_tmpBA5->tl=new_fields,_tmpBA5))))));};}else{
# 1793
struct Cyc_List_List*_tmpBA6;new_fields=((_tmpBA6=_cycalloc(sizeof(*_tmpBA6)),((_tmpBA6->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpBA6->tl=new_fields,_tmpBA6))))));}}{
# 1795
struct Cyc_List_List*_tmp3BC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpBAE;void*_tmpBAD[1];const char*_tmpBAC;struct _dyneither_ptr*_tmpBAB;struct _dyneither_ptr*name=
(_tmpBAB=_cycalloc(sizeof(*_tmpBAB)),((_tmpBAB[0]=(struct _dyneither_ptr)((_tmpBAE.tag=1,((_tmpBAE.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBAD[0]=& _tmpBAE,Cyc_aprintf(((_tmpBAC="_genStruct%d",_tag_dyneither(_tmpBAC,sizeof(char),13))),_tag_dyneither(_tmpBAD,sizeof(void*),1)))))))),_tmpBAB)));
struct _tuple1*_tmpBAF;struct _tuple1*qv=(_tmpBAF=_cycalloc(sizeof(*_tmpBAF)),((_tmpBAF->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBAF->f2=name,_tmpBAF)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBB0;struct Cyc_Absyn_AggrdeclImpl*_tmp3BD=(_tmpBB0=_cycalloc(sizeof(*_tmpBB0)),((_tmpBB0->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpBB0->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpBB0->fields=_tmp3BC,((_tmpBB0->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpBB0)))))))));
# 1803
struct Cyc_Absyn_Aggrdecl*_tmpBB1;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBB1=_cycalloc(sizeof(*_tmpBB1)),((_tmpBB1->kind=ad->kind,((_tmpBB1->sc=ad->sc,((_tmpBB1->name=qv,((_tmpBB1->tvs=ad->tvs,((_tmpBB1->impl=_tmp3BD,((_tmpBB1->attributes=ad->attributes,_tmpBB1)))))))))))));
# 1809
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1813
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1819
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3C5=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3C5,0);
do_declare=1;}else{
# 1827
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1830
struct Cyc_Absyn_Stmt*_tmp3C6=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1834
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3C7=_tmp3C6->r;void*_tmp3C8=_tmp3C7;struct Cyc_Absyn_Exp**_tmp3CA;_LL18C: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3C9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3C8;if(_tmp3C9->tag != 1)goto _LL18E;else{_tmp3CA=(struct Cyc_Absyn_Exp**)& _tmp3C9->f1;}}_LL18D:
 varexp=_tmp3CA;goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpBB4;void*_tmpBB3;(_tmpBB3=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB4="stmt not an expression!",_tag_dyneither(_tmpBB4,sizeof(char),24))),_tag_dyneither(_tmpBB3,sizeof(void*),0)));}_LL18B:;}{
# 1840
struct Cyc_Absyn_Exp*(*_tmp3CD)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1844
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3CE=Cyc_Tcutil_compress(struct_type);void*_tmp3CF=_tmp3CE;union Cyc_Absyn_AggrInfoU _tmp3D1;struct Cyc_List_List*_tmp3D2;_LL191: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D0=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CF;if(_tmp3D0->tag != 11)goto _LL193;else{_tmp3D1=(_tmp3D0->f1).aggr_info;_tmp3D2=(_tmp3D0->f1).targs;}}_LL192:
# 1851
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3D1);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3D2;
goto _LL190;_LL193:;_LL194: {
const char*_tmpBB7;void*_tmpBB6;(_tmpBB6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBB7="init_struct: bad struct type",_tag_dyneither(_tmpBB7,sizeof(char),29))),_tag_dyneither(_tmpBB6,sizeof(void*),0)));}_LL190:;}
# 1858
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3D5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3D5->tl != 0;_tmp3D5=_tmp3D5->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3D6=(struct Cyc_Absyn_Aggrfield*)_tmp3D5->hd;
struct _RegionHandle _tmp3D7=_new_region("temp");struct _RegionHandle*temp=& _tmp3D7;_push_region(temp);
{struct Cyc_List_List*_tmp3D8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1867
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3D8,_tmp3D6->type))){
struct Cyc_List_List*_tmp3D9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3DA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3D8,_tmp3D9);
# 1871
struct Cyc_List_List*new_fields=0;
for(_tmp3D5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3D5 != 0;_tmp3D5=_tmp3D5->tl){
struct Cyc_Absyn_Aggrfield*_tmp3DB=(struct Cyc_Absyn_Aggrfield*)_tmp3D5->hd;
struct Cyc_Absyn_Aggrfield*_tmpBB8;struct Cyc_Absyn_Aggrfield*_tmp3DC=(_tmpBB8=_cycalloc(sizeof(*_tmpBB8)),((_tmpBB8->name=_tmp3DB->name,((_tmpBB8->tq=Cyc_Toc_mt_tq,((_tmpBB8->type=
# 1876
Cyc_Tcutil_rsubstitute(temp,_tmp3DA,_tmp3DB->type),((_tmpBB8->width=_tmp3DB->width,((_tmpBB8->attributes=_tmp3DB->attributes,((_tmpBB8->requires_clause=0,_tmpBB8)))))))))))));
# 1884
if(_tmp3D5->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3DC->type)){
struct _dyneither_ptr**_tmpBB9;index=((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9[0]=_tmp3DC->name,_tmpBB9))));}}{
# 1891
struct Cyc_List_List*_tmpBBA;new_fields=((_tmpBBA=_cycalloc(sizeof(*_tmpBBA)),((_tmpBBA->hd=_tmp3DC,((_tmpBBA->tl=new_fields,_tmpBBA))))));};}
# 1893
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpBC2;void*_tmpBC1[1];const char*_tmpBC0;struct _dyneither_ptr*_tmpBBF;struct _dyneither_ptr*name=
(_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=(struct _dyneither_ptr)((_tmpBC2.tag=1,((_tmpBC2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpBC1[0]=& _tmpBC2,Cyc_aprintf(((_tmpBC0="_genStruct%d",_tag_dyneither(_tmpBC0,sizeof(char),13))),_tag_dyneither(_tmpBC1,sizeof(void*),1)))))))),_tmpBBF)));
struct _tuple1*_tmpBC3;struct _tuple1*qv=(_tmpBC3=_cycalloc(sizeof(*_tmpBC3)),((_tmpBC3->f1=Cyc_Absyn_Abs_n(0,1),((_tmpBC3->f2=name,_tmpBC3)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpBC4;struct Cyc_Absyn_AggrdeclImpl*_tmp3E0=(_tmpBC4=_cycalloc(sizeof(*_tmpBC4)),((_tmpBC4->exist_vars=0,((_tmpBC4->rgn_po=0,((_tmpBC4->fields=aggrfields,((_tmpBC4->tagged=0,_tmpBC4)))))))));
# 1902
struct Cyc_Absyn_Aggrdecl*_tmpBC5;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpBC5=_cycalloc(sizeof(*_tmpBC5)),((_tmpBC5->kind=Cyc_Absyn_StructA,((_tmpBC5->sc=Cyc_Absyn_Public,((_tmpBC5->name=qv,((_tmpBC5->tvs=0,((_tmpBC5->impl=_tmp3E0,((_tmpBC5->attributes=0,_tmpBC5)))))))))))));
# 1908
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBCF;struct Cyc_Absyn_Aggrdecl**_tmpBCE;struct Cyc_Absyn_AggrInfo _tmpBCD;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBCC;struct_type=(void*)((_tmpBCC=_cycalloc(sizeof(*_tmpBCC)),((_tmpBCC[0]=((_tmpBCF.tag=11,((_tmpBCF.f1=((_tmpBCD.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBCE=_cycalloc(sizeof(*_tmpBCE)),((_tmpBCE[0]=new_ad,_tmpBCE))))),((_tmpBCD.targs=0,_tmpBCD)))),_tmpBCF)))),_tmpBCC))));}
# 1912
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1863
;_pop_region(temp);};}{
# 1916
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3EC=_new_region("r");struct _RegionHandle*r=& _tmp3EC;_push_region(r);
{struct Cyc_List_List*_tmp3ED=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3ED != 0;_tmp3ED=_tmp3ED->tl){
struct _tuple19*_tmp3EE=(struct _tuple19*)_tmp3ED->hd;struct Cyc_List_List*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F1;struct _tuple19*_tmp3EF=_tmp3EE;_tmp3F0=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3F1->topt));
if(_tmp3F0 == 0){
const char*_tmpBD2;void*_tmpBD1;(_tmpBD1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD2="empty designator list",_tag_dyneither(_tmpBD2,sizeof(char),22))),_tag_dyneither(_tmpBD1,sizeof(void*),0)));}
if(_tmp3F0->tl != 0){
# 1926
struct _tuple1*_tmp3F4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Absyn_var_exp(_tmp3F4,0);
for(0;_tmp3F0 != 0;_tmp3F0=_tmp3F0->tl){
void*_tmp3F6=(void*)_tmp3F0->hd;void*_tmp3F7=_tmp3F6;struct _dyneither_ptr*_tmp3F9;_LL196: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F8=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3F7;if(_tmp3F8->tag != 1)goto _LL198;else{_tmp3F9=_tmp3F8->f1;}}_LL197:
# 1932
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3F9))
_tmp3F5=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F5);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3CD(xexp,_tmp3F9,0),_tmp3F5,0),0),_tmp3C6,0);
# 1936
goto _LL195;_LL198:;_LL199: {
const char*_tmpBD5;void*_tmpBD4;(_tmpBD4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD5="array designator in struct",_tag_dyneither(_tmpBD5,sizeof(char),27))),_tag_dyneither(_tmpBD4,sizeof(void*),0)));}_LL195:;}
# 1940
Cyc_Toc_exp_to_c(nv,_tmp3F1);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3F5,_tmp3F1,0),0),_tmp3C6,0);}else{
# 1943
void*_tmp3FC=(void*)_tmp3F0->hd;void*_tmp3FD=_tmp3FC;struct _dyneither_ptr*_tmp3FF;_LL19B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3FE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3FD;if(_tmp3FE->tag != 1)goto _LL19D;else{_tmp3FF=_tmp3FE->f1;}}_LL19C: {
# 1945
struct Cyc_Absyn_Exp*lval=_tmp3CD(xexp,_tmp3FF,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3FF);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3C6,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1954
int e1_translated=0;
{void*_tmp400=_tmp3F1->r;void*_tmp401=_tmp400;struct Cyc_List_List*_tmp403;struct Cyc_Absyn_Vardecl*_tmp405;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp407;int _tmp408;struct Cyc_Absyn_Exp*_tmp40A;void*_tmp40B;void*_tmp40D;struct Cyc_List_List*_tmp40E;_LL1A0: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp402=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp402->tag != 25)goto _LL1A2;else{_tmp403=_tmp402->f1;}}_LL1A1:
# 1957
 _tmp3C6=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3FF)))->type,lval,_tmp403,_tmp3C6);
# 1959
goto _LL19F;_LL1A2: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp404=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp404->tag != 26)goto _LL1A4;else{_tmp405=_tmp404->f1;_tmp406=_tmp404->f2;_tmp407=_tmp404->f3;_tmp408=_tmp404->f4;}}_LL1A3:
# 1961
 _tmp3C6=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3FF)))->type,lval,_tmp405,_tmp406,_tmp407,_tmp408,_tmp3C6,0);
# 1965
e1_translated=1;
_tmp40A=_tmp406;_tmp40B=(void*)_check_null(_tmp407->topt);goto _LL1A5;_LL1A4: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp409=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp409->tag != 27)goto _LL1A6;else{_tmp40A=_tmp409->f1;_tmp40B=(void*)_tmp409->f2;}}_LL1A5:
# 1970
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3FF)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp40A);{
void*_tmp40F=Cyc_Toc_typ_to_c(_tmp40B);
struct _tuple6 _tmpBD9;union Cyc_Absyn_Cnst _tmpBD8;struct Cyc_Absyn_Exp*_tmpBD6[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBD6[1]=Cyc_Absyn_sizeoftyp_exp(_tmp40F,0),((_tmpBD6[0]=_tmp40A,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBD6,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBD8.Int_c).val=((_tmpBD9.f1=Cyc_Absyn_Unsigned,((_tmpBD9.f2=(int)sizeof(double),_tmpBD9)))),(((_tmpBD8.Int_c).tag=5,_tmpBD8)))),0),0);};}
# 1978
goto _LL19F;_LL1A6: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp40C=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp401;if(_tmp40C->tag != 29)goto _LL1A8;else{_tmp40D=(void*)_tmp40C->f1;_tmp40E=_tmp40C->f2;}}_LL1A7:
# 1980
 _tmp3C6=Cyc_Toc_init_anon_struct(nv,lval,_tmp40D,_tmp40E,_tmp3C6);goto _LL19F;_LL1A8:;_LL1A9: {
# 1982
void*old_e_typ=(void*)_check_null(_tmp3F1->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3F1);
{void*_tmp413=old_e_typ;_LL1AB: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp414=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp413;if(_tmp414->tag != 11)goto _LL1AD;}_LL1AC:
# 1989
 if(old_e_typ != _tmp3F1->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3F1->topt))){
# 1993
ad=Cyc_Toc_update_aggr_type(ad,_tmp3FF,(void*)_check_null(_tmp3F1->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBE3;struct Cyc_Absyn_Aggrdecl**_tmpBE2;struct Cyc_Absyn_AggrInfo _tmpBE1;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBE0;struct_type=(void*)(
(_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0[0]=((_tmpBE3.tag=11,((_tmpBE3.f1=((_tmpBE1.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=ad,_tmpBE2))))),((_tmpBE1.targs=0,_tmpBE1)))),_tmpBE3)))),_tmpBE0))));}
# 1997
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1999
goto _LL1AA;_LL1AD:;_LL1AE:
 goto _LL1AA;_LL1AA:;}{
# 2002
struct Cyc_Absyn_Aggrfield*_tmp419=Cyc_Absyn_lookup_field(aggrfields,_tmp3FF);
# 2004
if(Cyc_Toc_is_poly_field(struct_type,_tmp3FF) && !was_ptr_type)
_tmp3F1=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3F1);
# 2007
if(exist_types != 0)
_tmp3F1=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp419))->type),_tmp3F1);
# 2010
_tmp3C6=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3F1,0),0),_tmp3C6,0);
goto _LL19F;};}_LL19F:;}
# 2013
goto _LL19A;};}_LL19D:;_LL19E: {
const char*_tmpBE6;void*_tmpBE5;(_tmpBE5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBE6="array designator in struct",_tag_dyneither(_tmpBE6,sizeof(char),27))),_tag_dyneither(_tmpBE5,sizeof(void*),0)));}_LL19A:;}}}
# 2020
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2025
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3C6,pointer,rgnopt,is_atomic,do_declare);
# 2027
_tmp41C->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp41D=_tmp41C;_npop_handler(0);return _tmp41D;};};
# 1918
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2033
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2036
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2039
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBE9;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBE8;return Cyc_Absyn_new_exp((void*)((_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8[0]=((_tmpBE9.tag=4,((_tmpBE9.f1=e1,((_tmpBE9.f2=incr,_tmpBE9)))))),_tmpBE8)))),0);}
# 2043
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2052
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2054
void*_tmp420=e1->r;void*_tmp421=_tmp420;struct Cyc_Absyn_Stmt*_tmp423;void*_tmp425;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp428;struct _dyneither_ptr*_tmp429;int _tmp42A;int _tmp42B;_LL1B0: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp422=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp422->tag != 36)goto _LL1B2;else{_tmp423=_tmp422->f1;}}_LL1B1:
 Cyc_Toc_lvalue_assign_stmt(_tmp423,fs,f,f_env);goto _LL1AF;_LL1B2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp424=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp424->tag != 13)goto _LL1B4;else{_tmp425=(void*)_tmp424->f1;_tmp426=_tmp424->f2;}}_LL1B3:
 Cyc_Toc_lvalue_assign(_tmp426,fs,f,f_env);goto _LL1AF;_LL1B4: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp427=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp421;if(_tmp427->tag != 20)goto _LL1B6;else{_tmp428=_tmp427->f1;_tmp429=_tmp427->f2;_tmp42A=_tmp427->f3;_tmp42B=_tmp427->f4;}}_LL1B5:
# 2059
 e1->r=_tmp428->r;
{struct Cyc_List_List*_tmpBEA;Cyc_Toc_lvalue_assign(e1,((_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA->hd=_tmp429,((_tmpBEA->tl=fs,_tmpBEA)))))),f,f_env);}
goto _LL1AF;_LL1B6:;_LL1B7: {
# 2067
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2069
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1AF;}_LL1AF:;}
# 2075
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2077
void*_tmp42D=s->r;void*_tmp42E=_tmp42D;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Decl*_tmp432;struct Cyc_Absyn_Stmt*_tmp433;struct Cyc_Absyn_Stmt*_tmp435;_LL1B9: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp42F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp42E;if(_tmp42F->tag != 1)goto _LL1BB;else{_tmp430=_tmp42F->f1;}}_LL1BA:
 Cyc_Toc_lvalue_assign(_tmp430,fs,f,f_env);goto _LL1B8;_LL1BB: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp431=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42E;if(_tmp431->tag != 12)goto _LL1BD;else{_tmp432=_tmp431->f1;_tmp433=_tmp431->f2;}}_LL1BC:
# 2080
 Cyc_Toc_lvalue_assign_stmt(_tmp433,fs,f,f_env);goto _LL1B8;_LL1BD: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp434=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42E;if(_tmp434->tag != 2)goto _LL1BF;else{_tmp435=_tmp434->f2;}}_LL1BE:
 Cyc_Toc_lvalue_assign_stmt(_tmp435,fs,f,f_env);goto _LL1B8;_LL1BF:;_LL1C0: {
const char*_tmpBEE;void*_tmpBED[1];struct Cyc_String_pa_PrintArg_struct _tmpBEC;(_tmpBEC.tag=0,((_tmpBEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBED[0]=& _tmpBEC,Cyc_Toc_toc_impos(((_tmpBEE="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBEE,sizeof(char),23))),_tag_dyneither(_tmpBED,sizeof(void*),1)))))));}_LL1B8:;}
# 2086
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2090
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp439=e->r;void*_tmp43A=_tmp439;void**_tmp43C;struct Cyc_Absyn_Exp**_tmp43D;struct Cyc_Absyn_Exp*_tmp43F;struct Cyc_Absyn_Stmt*_tmp441;_LL1C2: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp43B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43A;if(_tmp43B->tag != 13)goto _LL1C4;else{_tmp43C=(void**)((void**)& _tmp43B->f1);_tmp43D=(struct Cyc_Absyn_Exp**)& _tmp43B->f2;}}_LL1C3:
# 2093
*_tmp43D=Cyc_Toc_push_address_exp(*_tmp43D);
*_tmp43C=Cyc_Absyn_cstar_typ(*_tmp43C,Cyc_Toc_mt_tq);
return e;_LL1C4: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp43E=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp43A;if(_tmp43E->tag != 19)goto _LL1C6;else{_tmp43F=_tmp43E->f1;}}_LL1C5:
# 2097
 return _tmp43F;_LL1C6: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp440=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp43A;if(_tmp440->tag != 36)goto _LL1C8;else{_tmp441=_tmp440->f1;}}_LL1C7:
# 2101
 Cyc_Toc_push_address_stmt(_tmp441);
return e;_LL1C8:;_LL1C9:
# 2104
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBF2;void*_tmpBF1[1];struct Cyc_String_pa_PrintArg_struct _tmpBF0;(_tmpBF0.tag=0,((_tmpBF0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBF1[0]=& _tmpBF0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF2="can't take & of exp %s",_tag_dyneither(_tmpBF2,sizeof(char),23))),_tag_dyneither(_tmpBF1,sizeof(void*),1)))))));};_LL1C1:;}
# 2109
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp445=s->r;void*_tmp446=_tmp445;struct Cyc_Absyn_Stmt*_tmp448;struct Cyc_Absyn_Stmt*_tmp44A;struct Cyc_Absyn_Exp**_tmp44C;_LL1CB: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp447=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp446;if(_tmp447->tag != 2)goto _LL1CD;else{_tmp448=_tmp447->f2;}}_LL1CC:
 _tmp44A=_tmp448;goto _LL1CE;_LL1CD: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp449=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp446;if(_tmp449->tag != 12)goto _LL1CF;else{_tmp44A=_tmp449->f2;}}_LL1CE:
 Cyc_Toc_push_address_stmt(_tmp44A);goto _LL1CA;_LL1CF: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp44B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp446;if(_tmp44B->tag != 1)goto _LL1D1;else{_tmp44C=(struct Cyc_Absyn_Exp**)& _tmp44B->f1;}}_LL1D0:
*_tmp44C=Cyc_Toc_push_address_exp(*_tmp44C);goto _LL1CA;_LL1D1:;_LL1D2: {
# 2115
const char*_tmpBF6;void*_tmpBF5[1];struct Cyc_String_pa_PrintArg_struct _tmpBF4;(_tmpBF4.tag=0,((_tmpBF4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBF5[0]=& _tmpBF4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBF6="can't take & of stmt %s",_tag_dyneither(_tmpBF6,sizeof(char),24))),_tag_dyneither(_tmpBF5,sizeof(void*),1)))))));}_LL1CA:;}
# 2120
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2122
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2124
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBF7;result=((_tmpBF7=_region_malloc(r2,sizeof(*_tmpBF7)),((_tmpBF7->hd=f(x->hd,env),((_tmpBF7->tl=0,_tmpBF7))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBF8;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBF8=_region_malloc(r2,sizeof(*_tmpBF8)),((_tmpBF8->hd=f(x->hd,env),((_tmpBF8->tl=0,_tmpBF8))))));}
prev=prev->tl;}
# 2131
return result;}
# 2133
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2137
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpBF9;return(_tmpBF9=_cycalloc(sizeof(*_tmpBF9)),((_tmpBF9->f1=0,((_tmpBF9->f2=e,_tmpBF9)))));}
# 2141
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp453=Cyc_Tcutil_compress(t);void*_tmp454=_tmp453;struct Cyc_Absyn_PtrInfo _tmp456;_LL1D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp455=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454;if(_tmp455->tag != 5)goto _LL1D6;else{_tmp456=_tmp455->f1;}}_LL1D5:
 return _tmp456;_LL1D6:;_LL1D7: {
const char*_tmpBFC;void*_tmpBFB;(_tmpBFB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBFC="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBFC,sizeof(char),28))),_tag_dyneither(_tmpBFB,sizeof(void*),0)));}_LL1D3:;}
# 2151
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp459=Cyc_Tcutil_compress(t);void*_tmp45A=_tmp459;enum Cyc_Absyn_Sign _tmp45D;enum Cyc_Absyn_Sign _tmp45F;enum Cyc_Absyn_Sign _tmp463;enum Cyc_Absyn_Sign _tmp465;enum Cyc_Absyn_Sign _tmp467;int _tmp46B;_LL1D9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45A;if(_tmp45B->tag != 5)goto _LL1DB;}_LL1DA:
 res=Cyc_Absyn_null_exp(0);goto _LL1D8;_LL1DB: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45A;if(_tmp45C->tag != 6)goto _LL1DD;else{_tmp45D=_tmp45C->f1;if(_tmp45C->f2 != Cyc_Absyn_Char_sz)goto _LL1DD;}}_LL1DC:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp45D,'\000'),0);goto _LL1D8;_LL1DD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp45E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45A;if(_tmp45E->tag != 6)goto _LL1DF;else{_tmp45F=_tmp45E->f1;if(_tmp45E->f2 != Cyc_Absyn_Short_sz)goto _LL1DF;}}_LL1DE:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp45F,0),0);goto _LL1D8;_LL1DF: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp460=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp45A;if(_tmp460->tag != 13)goto _LL1E1;}_LL1E0:
 goto _LL1E2;_LL1E1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp461=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp45A;if(_tmp461->tag != 14)goto _LL1E3;}_LL1E2:
 _tmp463=Cyc_Absyn_Unsigned;goto _LL1E4;_LL1E3: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp462=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45A;if(_tmp462->tag != 6)goto _LL1E5;else{_tmp463=_tmp462->f1;if(_tmp462->f2 != Cyc_Absyn_Int_sz)goto _LL1E5;}}_LL1E4:
 _tmp465=_tmp463;goto _LL1E6;_LL1E5: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp464=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45A;if(_tmp464->tag != 6)goto _LL1E7;else{_tmp465=_tmp464->f1;if(_tmp464->f2 != Cyc_Absyn_Long_sz)goto _LL1E7;}}_LL1E6:
# 2161
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp465,0),0);goto _LL1D8;_LL1E7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp466=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45A;if(_tmp466->tag != 6)goto _LL1E9;else{_tmp467=_tmp466->f1;if(_tmp466->f2 != Cyc_Absyn_LongLong_sz)goto _LL1E9;}}_LL1E8:
# 2163
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp467,(long long)0),0);goto _LL1D8;_LL1E9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp468=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45A;if(_tmp468->tag != 7)goto _LL1EB;else{if(_tmp468->f1 != 0)goto _LL1EB;}}_LL1EA:
{const char*_tmpBFD;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBFD="0.0F",_tag_dyneither(_tmpBFD,sizeof(char),5))),0),0);}goto _LL1D8;_LL1EB: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp469=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45A;if(_tmp469->tag != 7)goto _LL1ED;else{if(_tmp469->f1 != 1)goto _LL1ED;}}_LL1EC:
{const char*_tmpBFE;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBFE="0.0",_tag_dyneither(_tmpBFE,sizeof(char),4))),1),0);}goto _LL1D8;_LL1ED: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp46A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45A;if(_tmp46A->tag != 7)goto _LL1EF;else{_tmp46B=_tmp46A->f1;}}_LL1EE:
{const char*_tmpBFF;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBFF="0.0L",_tag_dyneither(_tmpBFF,sizeof(char),5))),_tmp46B),0);}goto _LL1D8;_LL1EF:;_LL1F0: {
# 2168
const char*_tmpC03;void*_tmpC02[1];struct Cyc_String_pa_PrintArg_struct _tmpC01;(_tmpC01.tag=0,((_tmpC01.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpC02[0]=& _tmpC01,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC03="found non-zero type %s in generate_zero",_tag_dyneither(_tmpC03,sizeof(char),40))),_tag_dyneither(_tmpC02,sizeof(void*),1)))))));}_LL1D8:;}
# 2170
res->topt=t;
return res;}
# 2177
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2190 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp472=Cyc_Toc_typ_to_c(elt_type);
void*_tmp473=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp474=Cyc_Absyn_cstar_typ(_tmp472,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpC04;struct Cyc_Core_Opt*_tmp475=(_tmpC04=_cycalloc(sizeof(*_tmpC04)),((_tmpC04->v=_tmp474,_tmpC04)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp476=e1->r;void*_tmp477=_tmp476;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47C;_LL1F2: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp478=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp477;if(_tmp478->tag != 19)goto _LL1F4;else{_tmp479=_tmp478->f1;}}_LL1F3:
# 2198
 if(!is_dyneither){
_tmp479=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp479,0,Cyc_Absyn_Other_coercion,0);
_tmp479->topt=fat_ptr_type;}
# 2202
Cyc_Toc_exp_to_c(nv,_tmp479);xinit=_tmp479;goto _LL1F1;_LL1F4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp47A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp477;if(_tmp47A->tag != 22)goto _LL1F6;else{_tmp47B=_tmp47A->f1;_tmp47C=_tmp47A->f2;}}_LL1F5:
# 2204
 if(!is_dyneither){
_tmp47B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp47B,0,Cyc_Absyn_Other_coercion,0);
_tmp47B->topt=fat_ptr_type;}
# 2208
Cyc_Toc_exp_to_c(nv,_tmp47B);Cyc_Toc_exp_to_c(nv,_tmp47C);
{struct Cyc_Absyn_Exp*_tmpC05[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC05[2]=_tmp47C,((_tmpC05[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC05[0]=_tmp47B,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC05,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1F1;_LL1F6:;_LL1F7: {
const char*_tmpC08;void*_tmpC07;(_tmpC07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC08="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpC08,sizeof(char),53))),_tag_dyneither(_tmpC07,sizeof(void*),0)));}_LL1F1:;}{
# 2214
struct _tuple1*_tmp480=Cyc_Toc_temp_var();
struct _RegionHandle _tmp481=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp481;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp482=Cyc_Toc_add_varmap(rgn2,nv,_tmp480,Cyc_Absyn_var_exp(_tmp480,0));
struct Cyc_Absyn_Vardecl*_tmpC09;struct Cyc_Absyn_Vardecl*_tmp483=(_tmpC09=_cycalloc(sizeof(*_tmpC09)),((_tmpC09->sc=Cyc_Absyn_Public,((_tmpC09->name=_tmp480,((_tmpC09->tq=Cyc_Toc_mt_tq,((_tmpC09->type=_tmp473,((_tmpC09->initializer=xinit,((_tmpC09->rgn=0,((_tmpC09->attributes=0,((_tmpC09->escapes=0,_tmpC09)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC0C;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC0B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp484=(_tmpC0B=_cycalloc(sizeof(*_tmpC0B)),((_tmpC0B[0]=((_tmpC0C.tag=4,((_tmpC0C.f1=_tmp483,_tmpC0C)))),_tmpC0B)));
struct Cyc_Absyn_Exp*_tmp485=Cyc_Absyn_varb_exp(_tmp480,(void*)_tmp484,0);
_tmp485->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp486=Cyc_Absyn_deref_exp(_tmp485,0);
_tmp486->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp482,_tmp486);{
struct _tuple1*_tmp487=Cyc_Toc_temp_var();
_tmp482=Cyc_Toc_add_varmap(rgn2,_tmp482,_tmp487,Cyc_Absyn_var_exp(_tmp487,0));{
struct Cyc_Absyn_Vardecl*_tmpC0D;struct Cyc_Absyn_Vardecl*_tmp488=(_tmpC0D=_cycalloc(sizeof(*_tmpC0D)),((_tmpC0D->sc=Cyc_Absyn_Public,((_tmpC0D->name=_tmp487,((_tmpC0D->tq=Cyc_Toc_mt_tq,((_tmpC0D->type=_tmp472,((_tmpC0D->initializer=_tmp486,((_tmpC0D->rgn=0,((_tmpC0D->attributes=0,((_tmpC0D->escapes=0,_tmpC0D)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC10;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC0F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp489=(_tmpC0F=_cycalloc(sizeof(*_tmpC0F)),((_tmpC0F[0]=((_tmpC10.tag=4,((_tmpC10.f1=_tmp488,_tmpC10)))),_tmpC0F)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp48A=Cyc_Absyn_varb_exp(_tmp487,(void*)_tmp489,0);
_tmp48A->topt=_tmp486->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp48A,e2,0);
z_init->topt=_tmp48A->topt;}
# 2235
Cyc_Toc_exp_to_c(_tmp482,z_init);{
struct _tuple1*_tmp48B=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpC11;struct Cyc_Absyn_Vardecl*_tmp48C=(_tmpC11=_cycalloc(sizeof(*_tmpC11)),((_tmpC11->sc=Cyc_Absyn_Public,((_tmpC11->name=_tmp48B,((_tmpC11->tq=Cyc_Toc_mt_tq,((_tmpC11->type=_tmp472,((_tmpC11->initializer=z_init,((_tmpC11->rgn=0,((_tmpC11->attributes=0,((_tmpC11->escapes=0,_tmpC11)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpC14;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpC13;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp48D=(_tmpC13=_cycalloc(sizeof(*_tmpC13)),((_tmpC13[0]=((_tmpC14.tag=4,((_tmpC14.f1=_tmp48C,_tmpC14)))),_tmpC13)));
_tmp482=Cyc_Toc_add_varmap(rgn2,_tmp482,_tmp48B,Cyc_Absyn_var_exp(_tmp48B,0));{
# 2242
struct Cyc_Absyn_Exp*_tmp48E=Cyc_Absyn_varb_exp(_tmp487,(void*)_tmp489,0);_tmp48E->topt=_tmp486->topt;{
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp490=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp48E,_tmp48F,0);
_tmp490->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp482,_tmp490);{
# 2248
struct Cyc_Absyn_Exp*_tmp491=Cyc_Absyn_varb_exp(_tmp48B,(void*)_tmp48D,0);_tmp491->topt=_tmp486->topt;{
struct Cyc_Absyn_Exp*_tmp492=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp493=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp491,_tmp492,0);
_tmp493->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp482,_tmp493);{
# 2254
struct Cyc_Absyn_Exp*_tmpC15[2];struct Cyc_List_List*_tmp494=(_tmpC15[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpC15[0]=
# 2254
Cyc_Absyn_varb_exp(_tmp480,(void*)_tmp484,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC15,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2256
struct Cyc_Absyn_Exp*_tmp495=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp494,0),_tmp495,0);{
# 2261
struct Cyc_Absyn_Exp*_tmp496=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp490,_tmp493,0),0);
struct Cyc_Absyn_Stmt*_tmp497=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp498=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp480,(void*)_tmp484,0),Cyc_Toc_curr_sp,0);
_tmp498=Cyc_Toc_cast_it(_tmp474,_tmp498);{
struct Cyc_Absyn_Exp*_tmp499=Cyc_Absyn_deref_exp(_tmp498,0);
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Absyn_assign_exp(_tmp499,Cyc_Absyn_var_exp(_tmp48B,0),0);
struct Cyc_Absyn_Stmt*_tmp49B=Cyc_Absyn_exp_stmt(_tmp49A,0);
_tmp49B=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp496,_tmp497,Cyc_Absyn_skip_stmt(0),0),_tmp49B,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC1B;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC1A;struct Cyc_Absyn_Decl*_tmpC19;_tmp49B=Cyc_Absyn_decl_stmt(((_tmpC19=_cycalloc(sizeof(*_tmpC19)),((_tmpC19->r=(void*)((_tmpC1B=_cycalloc(sizeof(*_tmpC1B)),((_tmpC1B[0]=((_tmpC1A.tag=0,((_tmpC1A.f1=_tmp48C,_tmpC1A)))),_tmpC1B)))),((_tmpC19->loc=0,_tmpC19)))))),_tmp49B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC21;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC20;struct Cyc_Absyn_Decl*_tmpC1F;_tmp49B=Cyc_Absyn_decl_stmt(((_tmpC1F=_cycalloc(sizeof(*_tmpC1F)),((_tmpC1F->r=(void*)((_tmpC21=_cycalloc(sizeof(*_tmpC21)),((_tmpC21[0]=((_tmpC20.tag=0,((_tmpC20.f1=_tmp488,_tmpC20)))),_tmpC21)))),((_tmpC1F->loc=0,_tmpC1F)))))),_tmp49B,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpC27;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpC26;struct Cyc_Absyn_Decl*_tmpC25;_tmp49B=Cyc_Absyn_decl_stmt(((_tmpC25=_cycalloc(sizeof(*_tmpC25)),((_tmpC25->r=(void*)((_tmpC27=_cycalloc(sizeof(*_tmpC27)),((_tmpC27[0]=((_tmpC26.tag=0,((_tmpC26.f1=_tmp483,_tmpC26)))),_tmpC27)))),((_tmpC25->loc=0,_tmpC25)))))),_tmp49B,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp49B);};};};};};};};};};};};}
# 2216
;_pop_region(rgn2);};}
# 2287 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2291
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4B0=Cyc_Tcutil_compress(aggrtype);void*_tmp4B1=_tmp4B0;union Cyc_Absyn_AggrInfoU _tmp4B3;_LL1F9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4B2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4B1;if(_tmp4B2->tag != 11)goto _LL1FB;else{_tmp4B3=(_tmp4B2->f1).aggr_info;}}_LL1FA:
# 2294
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4B3);goto _LL1F8;_LL1FB:;_LL1FC: {
struct Cyc_String_pa_PrintArg_struct _tmpC2F;void*_tmpC2E[1];const char*_tmpC2D;void*_tmpC2C;(_tmpC2C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC2F.tag=0,((_tmpC2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC2E[0]=& _tmpC2F,Cyc_aprintf(((_tmpC2D="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC2D,sizeof(char),51))),_tag_dyneither(_tmpC2E,sizeof(void*),1)))))))),_tag_dyneither(_tmpC2C,sizeof(void*),0)));}_LL1F8:;}{
# 2298
struct _tuple1*_tmp4B8=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_Absyn_var_exp(_tmp4B8,0);
struct Cyc_Absyn_Exp*_tmp4BA=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4BB=Cyc_Absyn_aggrarrow_exp(_tmp4B9,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_Absyn_neq_exp(_tmp4BB,_tmp4BA,0);
struct Cyc_Absyn_Exp*_tmp4BD=Cyc_Absyn_aggrarrow_exp(_tmp4B9,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4BE=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4BD,0),0);
struct Cyc_Absyn_Stmt*_tmp4BF=Cyc_Absyn_ifthenelse_stmt(_tmp4BC,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4C0=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4C2=Cyc_Absyn_declare_stmt(_tmp4B8,_tmp4C0,_tmp4C1,Cyc_Absyn_seq_stmt(_tmp4BF,_tmp4BE,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4C2);}else{
# 2312
struct Cyc_Absyn_Exp*_tmp4C3=Cyc_Toc_member_exp(aggrproj(_tmp4B9,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_Absyn_neq_exp(_tmp4C3,_tmp4BA,0);
struct Cyc_Absyn_Exp*_tmp4C5=Cyc_Toc_member_exp(aggrproj(_tmp4B9,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4C6=Cyc_Absyn_exp_stmt(_tmp4C5,0);
struct Cyc_Absyn_Stmt*_tmp4C7=Cyc_Absyn_ifthenelse_stmt(_tmp4C4,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4C8=Cyc_Absyn_declare_stmt(_tmp4B8,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4C7,_tmp4C6,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4C8);}};}
# 2322
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2325
void*_tmp4C9=Cyc_Tcutil_compress(t);void*_tmp4CA=_tmp4C9;union Cyc_Absyn_AggrInfoU _tmp4CC;_LL1FE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4CB=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4CA;if(_tmp4CB->tag != 11)goto _LL200;else{_tmp4CC=(_tmp4CB->f1).aggr_info;}}_LL1FF: {
# 2327
struct Cyc_Absyn_Aggrdecl*_tmp4CD=Cyc_Absyn_get_known_aggrdecl(_tmp4CC);
*f_tag=Cyc_Toc_get_member_offset(_tmp4CD,f);{
# 2330
const char*_tmpC34;void*_tmpC33[2];struct Cyc_String_pa_PrintArg_struct _tmpC32;struct Cyc_String_pa_PrintArg_struct _tmpC31;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC31.tag=0,((_tmpC31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC32.tag=0,((_tmpC32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4CD->name).f2),((_tmpC33[0]=& _tmpC32,((_tmpC33[1]=& _tmpC31,Cyc_aprintf(((_tmpC34="_union_%s_%s",_tag_dyneither(_tmpC34,sizeof(char),13))),_tag_dyneither(_tmpC33,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC35;*tagged_member_type=Cyc_Absyn_strct(((_tmpC35=_cycalloc(sizeof(*_tmpC35)),((_tmpC35[0]=str,_tmpC35)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4CD->impl))->tagged;};}_LL200:;_LL201:
 return 0;_LL1FD:;}
# 2341
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2344
void*_tmp4D3=e->r;void*_tmp4D4=_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D8;struct _dyneither_ptr*_tmp4D9;int*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4DC;struct _dyneither_ptr*_tmp4DD;int*_tmp4DE;_LL203: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D5=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4D5->tag != 13)goto _LL205;else{_tmp4D6=_tmp4D5->f2;}}_LL204: {
const char*_tmpC38;void*_tmpC37;(_tmpC37=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC38="cast on lhs!",_tag_dyneither(_tmpC38,sizeof(char),13))),_tag_dyneither(_tmpC37,sizeof(void*),0)));}_LL205: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4D7=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4D7->tag != 20)goto _LL207;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f2;_tmp4DA=(int*)& _tmp4D7->f4;}}_LL206:
# 2347
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4D8->topt),_tmp4D9,f_tag,tagged_member_type,clear_read,_tmp4DA);_LL207: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4DB=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4D4;if(_tmp4DB->tag != 21)goto _LL209;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;_tmp4DE=(int*)& _tmp4DB->f4;}}_LL208: {
# 2350
void*_tmp4E1=Cyc_Tcutil_compress((void*)_check_null(_tmp4DC->topt));void*_tmp4E2=_tmp4E1;void*_tmp4E4;_LL20C: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E2;if(_tmp4E3->tag != 5)goto _LL20E;else{_tmp4E4=(_tmp4E3->f1).elt_typ;}}_LL20D:
# 2352
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4E4,_tmp4DD,f_tag,tagged_member_type,clear_read,_tmp4DE);_LL20E:;_LL20F:
# 2354
 return 0;_LL20B:;}_LL209:;_LL20A:
# 2356
 return 0;_LL202:;}
# 2368 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4E5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4E5,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4E6=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4E6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4E5,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2392 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2396
struct _tuple1*_tmp4E7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4E7,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2406
struct Cyc_Absyn_Exp*_tmp4E8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4E8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2409
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4E7,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2415
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2430 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4E9=Cyc_Absyn_var_exp(max,0);
# 2438
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4E9=Cyc_Absyn_add_exp(_tmp4E9,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2449
s=Cyc_Absyn_skip_stmt(0);}}{
# 2451
struct _RegionHandle _tmp4EA=_new_region("r");struct _RegionHandle*r=& _tmp4EA;_push_region(r);
{struct _tuple28*_tmpC3B;struct Cyc_List_List*_tmpC3A;struct Cyc_List_List*decls=
(_tmpC3A=_region_malloc(r,sizeof(*_tmpC3A)),((_tmpC3A->hd=((_tmpC3B=_region_malloc(r,sizeof(*_tmpC3B)),((_tmpC3B->f1=max,((_tmpC3B->f2=Cyc_Absyn_uint_typ,((_tmpC3B->f3=e1,_tmpC3B)))))))),((_tmpC3A->tl=0,_tmpC3A)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC3C[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC3C[1]=_tmp4E9,((_tmpC3C[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3C,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2461
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC3D[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC3D[1]=_tmp4E9,((_tmpC3D[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC3D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2467
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC40;struct Cyc_List_List*_tmpC3F;decls=((_tmpC3F=_region_malloc(r,sizeof(*_tmpC3F)),((_tmpC3F->hd=((_tmpC40=_region_malloc(r,sizeof(*_tmpC40)),((_tmpC40->f1=a,((_tmpC40->f2=ptr_typ,((_tmpC40->f3=ainit,_tmpC40)))))))),((_tmpC3F->tl=decls,_tmpC3F))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC43;void*_tmpC42;(_tmpC42=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC43="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC43,sizeof(char),59))),_tag_dyneither(_tmpC42,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4F1=Cyc_Toc_temp_var();
void*_tmp4F2=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4F3=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC44[3];struct Cyc_Absyn_Exp*_tmp4F4=
Cyc_Absyn_fncall_exp(_tmp4F3,(
(_tmpC44[2]=_tmp4E9,((_tmpC44[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC44[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC44,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2479
{struct _tuple28*_tmpC47;struct Cyc_List_List*_tmpC46;decls=((_tmpC46=_region_malloc(r,sizeof(*_tmpC46)),((_tmpC46->hd=((_tmpC47=_region_malloc(r,sizeof(*_tmpC47)),((_tmpC47->f1=_tmp4F1,((_tmpC47->f2=_tmp4F2,((_tmpC47->f3=_tmp4F4,_tmpC47)))))))),((_tmpC46->tl=decls,_tmpC46))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4F1,0),0),0);}else{
# 2482
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4F8=decls;for(0;_tmp4F8 != 0;_tmp4F8=_tmp4F8->tl){
struct _tuple28 _tmp4F9=*((struct _tuple28*)_tmp4F8->hd);struct _tuple1*_tmp4FB;void*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct _tuple28 _tmp4FA=_tmp4F9;_tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;_tmp4FD=_tmp4FA.f3;
s=Cyc_Absyn_declare_stmt(_tmp4FB,_tmp4FC,_tmp4FD,s,0);}}
# 2487
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2452
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC96(unsigned int*_tmpC95,unsigned int*_tmpC94,struct _tuple1***_tmpC92){for(*_tmpC95=0;*_tmpC95 < *_tmpC94;(*_tmpC95)++){(*_tmpC92)[*_tmpC95]=(struct _tuple1*)
# 2862 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2491 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp500=e->r;
if(e->topt == 0){
const char*_tmpC4B;void*_tmpC4A[1];struct Cyc_String_pa_PrintArg_struct _tmpC49;(_tmpC49.tag=0,((_tmpC49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC4A[0]=& _tmpC49,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC4B="exp_to_c: no type for %s",_tag_dyneither(_tmpC4B,sizeof(char),25))),_tag_dyneither(_tmpC4A,sizeof(void*),1)))))));}
# 2497
if((nv->struct_info).lhs_exp != 0){
void*_tmp504=_tmp500;_LL211: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp505=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp504;if(_tmp505->tag != 28)goto _LL213;}_LL212:
 goto _LL210;_LL213:;_LL214:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL210:;}{
# 2505
void*old_typ=(void*)_check_null(e->topt);
void*_tmp506=_tmp500;struct _tuple1*_tmp50A;void*_tmp50B;enum Cyc_Absyn_Primop _tmp50D;struct Cyc_List_List*_tmp50E;struct Cyc_Absyn_Exp*_tmp510;enum Cyc_Absyn_Incrementor _tmp511;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Core_Opt*_tmp514;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp522;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_List_List*_tmp525;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_List_List*_tmp528;int _tmp529;struct Cyc_List_List*_tmp52A;struct Cyc_Absyn_VarargInfo*_tmp52B;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_List_List*_tmp532;void**_tmp534;struct Cyc_Absyn_Exp*_tmp535;int _tmp536;enum Cyc_Absyn_Coercion _tmp537;struct Cyc_Absyn_Exp*_tmp539;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*_tmp53E;void*_tmp540;void*_tmp542;struct Cyc_List_List*_tmp543;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp547;struct _dyneither_ptr*_tmp548;int _tmp549;int _tmp54A;struct Cyc_Absyn_Exp*_tmp54C;struct _dyneither_ptr*_tmp54D;int _tmp54E;int _tmp54F;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp556;struct Cyc_Absyn_Vardecl*_tmp558;struct Cyc_Absyn_Exp*_tmp559;struct Cyc_Absyn_Exp*_tmp55A;int _tmp55B;struct Cyc_Absyn_Exp*_tmp55D;void*_tmp55E;int _tmp55F;struct _tuple1*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_Absyn_Aggrdecl*_tmp564;void*_tmp566;struct Cyc_List_List*_tmp567;struct Cyc_List_List*_tmp569;struct Cyc_Absyn_Datatypedecl*_tmp56A;struct Cyc_Absyn_Datatypefield*_tmp56B;int _tmp56F;struct Cyc_Absyn_Exp*_tmp570;void**_tmp571;struct Cyc_Absyn_Exp*_tmp572;int _tmp573;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp578;struct _dyneither_ptr*_tmp579;struct Cyc_Absyn_Stmt*_tmp57B;_LL216: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp507=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp507->tag != 0)goto _LL218;else{if(((_tmp507->f1).Null_c).tag != 1)goto _LL218;}}_LL217: {
# 2512
struct Cyc_Absyn_Exp*_tmp580=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp580,_tmp580))->r;else{
# 2517
struct Cyc_Absyn_Exp*_tmpC4C[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC4C[2]=_tmp580,((_tmpC4C[1]=_tmp580,((_tmpC4C[0]=_tmp580,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC4C,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2519
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2521
goto _LL215;}_LL218: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp508->tag != 0)goto _LL21A;}_LL219:
 goto _LL215;_LL21A: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp509=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp509->tag != 1)goto _LL21C;else{_tmp50A=_tmp509->f1;_tmp50B=(void*)_tmp509->f2;}}_LL21B:
# 2524
{struct _handler_cons _tmp582;_push_handler(& _tmp582);{int _tmp584=0;if(setjmp(_tmp582.handler))_tmp584=1;if(!_tmp584){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp50A))->r;;_pop_handler();}else{void*_tmp583=(void*)_exn_thrown;void*_tmp586=_tmp583;void*_tmp588;_LL26B: {struct Cyc_Dict_Absent_exn_struct*_tmp587=(struct Cyc_Dict_Absent_exn_struct*)_tmp586;if(_tmp587->tag != Cyc_Dict_Absent)goto _LL26D;}_LL26C: {
# 2526
const char*_tmpC50;void*_tmpC4F[1];struct Cyc_String_pa_PrintArg_struct _tmpC4E;(_tmpC4E.tag=0,((_tmpC4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp50A)),((_tmpC4F[0]=& _tmpC4E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC50="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC50,sizeof(char),32))),_tag_dyneither(_tmpC4F,sizeof(void*),1)))))));}_LL26D: _tmp588=_tmp586;_LL26E:(void)_throw(_tmp588);_LL26A:;}};}
# 2528
goto _LL215;_LL21C: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp50C->tag != 2)goto _LL21E;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;}}_LL21D: {
# 2531
struct Cyc_List_List*_tmp58C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp50E);
# 2533
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp50E);
switch(_tmp50D){case Cyc_Absyn_Numelts: _LL26F: {
# 2536
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd;
{void*_tmp58D=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp58E=_tmp58D;struct Cyc_Absyn_Exp*_tmp590;void*_tmp592;union Cyc_Absyn_Constraint*_tmp593;union Cyc_Absyn_Constraint*_tmp594;union Cyc_Absyn_Constraint*_tmp595;_LL272: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp58F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58E;if(_tmp58F->tag != 8)goto _LL274;else{_tmp590=(_tmp58F->f1).num_elts;}}_LL273:
# 2540
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp590))){
const char*_tmpC53;void*_tmpC52;(_tmpC52=0,Cyc_Tcutil_terr(e->loc,((_tmpC53="can't calculate numelts",_tag_dyneither(_tmpC53,sizeof(char),24))),_tag_dyneither(_tmpC52,sizeof(void*),0)));}
e->r=_tmp590->r;goto _LL271;_LL274: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp591=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58E;if(_tmp591->tag != 5)goto _LL276;else{_tmp592=(_tmp591->f1).elt_typ;_tmp593=((_tmp591->f1).ptr_atts).nullable;_tmp594=((_tmp591->f1).ptr_atts).bounds;_tmp595=((_tmp591->f1).ptr_atts).zero_term;}}_LL275:
# 2544
{void*_tmp598=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp594);void*_tmp599=_tmp598;struct Cyc_Absyn_Exp*_tmp59C;_LL279: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp599;if(_tmp59A->tag != 0)goto _LL27B;}_LL27A:
# 2546
{struct Cyc_Absyn_Exp*_tmpC54[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC54[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp592),0),((_tmpC54[0]=(struct Cyc_Absyn_Exp*)_tmp50E->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL278;_LL27B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp59B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp599;if(_tmp59B->tag != 1)goto _LL278;else{_tmp59C=_tmp59B->f1;}}_LL27C:
# 2552
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp595)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp50E->hd);
# 2556
struct Cyc_Absyn_Exp*_tmpC55[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC55[1]=_tmp59C,((_tmpC55[0]=(struct Cyc_Absyn_Exp*)_tmp50E->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp593)){
if(!Cyc_Evexp_c_can_eval(_tmp59C)){
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,Cyc_Tcutil_terr(e->loc,((_tmpC58="can't calculate numelts",_tag_dyneither(_tmpC58,sizeof(char),24))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}
# 2561
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp59C,Cyc_Absyn_uint_exp(0,0));}else{
# 2563
e->r=_tmp59C->r;goto _LL278;}}
# 2565
goto _LL278;_LL278:;}
# 2567
goto _LL271;_LL276:;_LL277: {
# 2569
const char*_tmpC5D;void*_tmpC5C[2];struct Cyc_String_pa_PrintArg_struct _tmpC5B;struct Cyc_String_pa_PrintArg_struct _tmpC5A;(_tmpC5A.tag=0,((_tmpC5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC5B.tag=0,((_tmpC5B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC5C[0]=& _tmpC5B,((_tmpC5C[1]=& _tmpC5A,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC5D,sizeof(char),41))),_tag_dyneither(_tmpC5C,sizeof(void*),2)))))))))))));}_LL271:;}
# 2572
break;}case Cyc_Absyn_Plus: _LL270:
# 2577
{void*_tmp5A5=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd);void*_tmp5A6=_tmp5A5;void*_tmp5A8;union Cyc_Absyn_Constraint*_tmp5A9;union Cyc_Absyn_Constraint*_tmp5AA;_LL27F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A6;if(_tmp5A7->tag != 5)goto _LL281;else{_tmp5A8=(_tmp5A7->f1).elt_typ;_tmp5A9=((_tmp5A7->f1).ptr_atts).bounds;_tmp5AA=((_tmp5A7->f1).ptr_atts).zero_term;}}_LL280:
# 2579
{void*_tmp5AB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp5A9);void*_tmp5AC=_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AF;_LL284: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5AD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5AC;if(_tmp5AD->tag != 0)goto _LL286;}_LL285: {
# 2581
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC5E[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC5E[2]=e2,((_tmpC5E[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5A8),0),((_tmpC5E[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5E,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL283;}_LL286: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5AE=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5AC;if(_tmp5AE->tag != 1)goto _LL283;else{_tmp5AF=_tmp5AE->f1;}}_LL287:
# 2587
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AA)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC5F[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC5F[2]=e2,((_tmpC5F[1]=_tmp5AF,((_tmpC5F[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5F,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2592
goto _LL283;_LL283:;}
# 2594
goto _LL27E;_LL281:;_LL282:
# 2596
 goto _LL27E;_LL27E:;}
# 2598
break;case Cyc_Absyn_Minus: _LL27D: {
# 2603
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp58C->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2613
struct Cyc_Absyn_Exp*_tmpC60[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC60[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC60[1]=
# 2614
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC60[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC60,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2618
break;}case Cyc_Absyn_Eq: _LL288:
 goto _LL289;case Cyc_Absyn_Neq: _LL289:
 goto _LL28A;case Cyc_Absyn_Gt: _LL28A:
 goto _LL28B;case Cyc_Absyn_Gte: _LL28B:
 goto _LL28C;case Cyc_Absyn_Lt: _LL28C:
 goto _LL28D;case Cyc_Absyn_Lte: _LL28D: {
# 2626
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp50E->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp58C))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp58C->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL28E:
 break;}
# 2638
goto _LL215;}_LL21E: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp50F->tag != 4)goto _LL220;else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;}}_LL21F: {
# 2640
void*e2_cyc_typ=(void*)_check_null(_tmp510->topt);
# 2649 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC61;struct _dyneither_ptr incr_str=(_tmpC61="increment",_tag_dyneither(_tmpC61,sizeof(char),10));
if(_tmp511 == Cyc_Absyn_PreDec  || _tmp511 == Cyc_Absyn_PostDec){const char*_tmpC62;incr_str=((_tmpC62="decrement",_tag_dyneither(_tmpC62,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp510,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC66;void*_tmpC65[1];struct Cyc_String_pa_PrintArg_struct _tmpC64;(_tmpC64.tag=0,((_tmpC64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC65[0]=& _tmpC64,Cyc_Tcutil_terr(e->loc,((_tmpC66="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC66,sizeof(char),74))),_tag_dyneither(_tmpC65,sizeof(void*),1)))))));}{
const char*_tmpC69;void*_tmpC68;(_tmpC68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC69="in-place inc/dec on zero-term",_tag_dyneither(_tmpC69,sizeof(char),30))),_tag_dyneither(_tmpC68,sizeof(void*),0)));};}{
# 2658
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp510,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5B9=Cyc_Absyn_signed_int_exp(1,0);
_tmp5B9->topt=Cyc_Absyn_sint_typ;
switch(_tmp511){case Cyc_Absyn_PreInc: _LL290:
# 2665
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC6F;struct Cyc_Core_Opt*_tmpC6E;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC6D;e->r=(void*)((_tmpC6D=_cycalloc(sizeof(*_tmpC6D)),((_tmpC6D[0]=((_tmpC6F.tag=3,((_tmpC6F.f1=_tmp510,((_tmpC6F.f2=((_tmpC6E=_cycalloc_atomic(sizeof(*_tmpC6E)),((_tmpC6E->v=(void*)Cyc_Absyn_Plus,_tmpC6E)))),((_tmpC6F.f3=_tmp5B9,_tmpC6F)))))))),_tmpC6D))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL291:
# 2669
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC75;struct Cyc_Core_Opt*_tmpC74;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC73;e->r=(void*)((_tmpC73=_cycalloc(sizeof(*_tmpC73)),((_tmpC73[0]=((_tmpC75.tag=3,((_tmpC75.f1=_tmp510,((_tmpC75.f2=((_tmpC74=_cycalloc_atomic(sizeof(*_tmpC74)),((_tmpC74->v=(void*)Cyc_Absyn_Minus,_tmpC74)))),((_tmpC75.f3=_tmp5B9,_tmpC75)))))))),_tmpC73))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL292:
# 2673
{const char*_tmpC79;void*_tmpC78[1];struct Cyc_String_pa_PrintArg_struct _tmpC77;(_tmpC77.tag=0,((_tmpC77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC78[0]=& _tmpC77,Cyc_Tcutil_terr(e->loc,((_tmpC79="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC79,sizeof(char),59))),_tag_dyneither(_tmpC78,sizeof(void*),1)))))));}{
# 2675
const char*_tmpC7C;void*_tmpC7B;(_tmpC7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC7C="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC7C,sizeof(char),34))),_tag_dyneither(_tmpC7B,sizeof(void*),0)));};}}
# 2678
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp510);
Cyc_Toc_set_lhs(nv,0);{
# 2683
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp511 == Cyc_Absyn_PostInc  || _tmp511 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2689
if(_tmp511 == Cyc_Absyn_PreDec  || _tmp511 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpC7D[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC7D[2]=
# 2693
Cyc_Absyn_signed_int_exp(change,0),((_tmpC7D[1]=
# 2692
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC7D[0]=
# 2691
Cyc_Toc_push_address_exp(_tmp510),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7D,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2694
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2698
struct Cyc_Toc_functionSet*_tmp5C6=_tmp511 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2700
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5C6,_tmp510);
struct Cyc_Absyn_Exp*_tmpC7E[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC7E[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC7E[0]=
# 2701
Cyc_Toc_push_address_exp(_tmp510),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC7E,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2703
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp510)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp510,0,Cyc_Toc_incr_lvalue,_tmp511);
e->r=_tmp510->r;}}}
# 2707
goto _LL215;};};}_LL220: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp512->tag != 3)goto _LL222;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;_tmp515=_tmp512->f3;}}_LL221: {
# 2726 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp513->topt);
void*e2_old_typ=(void*)_check_null(_tmp515->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp513,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp515);
e->r=Cyc_Toc_tagged_union_assignop(_tmp513,e1_old_typ,_tmp514,_tmp515,e2_old_typ,f_tag,tagged_member_struct_type);
# 2737
return;}{
# 2739
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp513,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp513,_tmp514,_tmp515,ptr_type,is_dyneither,elt_type);
# 2745
return;}{
# 2749
int e1_poly=Cyc_Toc_is_poly_project(_tmp513);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp515);{
# 2755
int done=0;
if(_tmp514 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5C9=(enum Cyc_Absyn_Primop)_tmp514->v;switch(_tmp5C9){case Cyc_Absyn_Plus: _LL294:
 change=_tmp515;break;case Cyc_Absyn_Minus: _LL295:
# 2763
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp515,0);break;default: _LL296: {
const char*_tmpC81;void*_tmpC80;(_tmpC80=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC81="bad t ? pointer arithmetic",_tag_dyneither(_tmpC81,sizeof(char),27))),_tag_dyneither(_tmpC80,sizeof(void*),0)));}}}
# 2766
done=1;{
# 2768
struct Cyc_Absyn_Exp*_tmp5CC=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC82[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5CC,((_tmpC82[2]=change,((_tmpC82[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC82[0]=
# 2769
Cyc_Toc_push_address_exp(_tmp513),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC82,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2772
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2775
enum Cyc_Absyn_Primop _tmp5CE=(enum Cyc_Absyn_Primop)_tmp514->v;switch(_tmp5CE){case Cyc_Absyn_Plus: _LL298:
# 2777
 done=1;
{struct Cyc_Absyn_Exp*_tmpC83[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp513),((_tmpC83[1]=_tmp515,((_tmpC83[0]=_tmp513,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC83,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL299: {
const char*_tmpC86;void*_tmpC85;(_tmpC85=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC86="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC86,sizeof(char),39))),_tag_dyneither(_tmpC85,sizeof(void*),0)));}}}}}
# 2784
if(!done){
# 2786
if(e1_poly)
_tmp515->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp515->r,0));
# 2792
if(!Cyc_Absyn_is_lvalue(_tmp513)){
{struct _tuple27 _tmpC89;struct _tuple27*_tmpC88;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp513,0,Cyc_Toc_assignop_lvalue,((_tmpC88=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpC88[0]=(struct _tuple27)((_tmpC89.f1=_tmp514,((_tmpC89.f2=_tmp515,_tmpC89)))),_tmpC88)))));}
e->r=_tmp513->r;}}
# 2797
goto _LL215;};};};}_LL222: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp516=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp516->tag != 5)goto _LL224;else{_tmp517=_tmp516->f1;_tmp518=_tmp516->f2;_tmp519=_tmp516->f3;}}_LL223:
# 2799
 Cyc_Toc_exp_to_c(nv,_tmp517);
Cyc_Toc_exp_to_c(nv,_tmp518);
Cyc_Toc_exp_to_c(nv,_tmp519);
goto _LL215;_LL224: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp51A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp51A->tag != 6)goto _LL226;else{_tmp51B=_tmp51A->f1;_tmp51C=_tmp51A->f2;}}_LL225:
# 2804
 Cyc_Toc_exp_to_c(nv,_tmp51B);
Cyc_Toc_exp_to_c(nv,_tmp51C);
goto _LL215;_LL226: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp51D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp51D->tag != 7)goto _LL228;else{_tmp51E=_tmp51D->f1;_tmp51F=_tmp51D->f2;}}_LL227:
# 2808
 Cyc_Toc_exp_to_c(nv,_tmp51E);
Cyc_Toc_exp_to_c(nv,_tmp51F);
goto _LL215;_LL228: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp520->tag != 8)goto _LL22A;else{_tmp521=_tmp520->f1;_tmp522=_tmp520->f2;}}_LL229:
# 2812
 Cyc_Toc_exp_to_c(nv,_tmp521);
Cyc_Toc_exp_to_c(nv,_tmp522);
goto _LL215;_LL22A: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp523=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp523->tag != 9)goto _LL22C;else{_tmp524=_tmp523->f1;_tmp525=_tmp523->f2;if(_tmp523->f3 != 0)goto _LL22C;}}_LL22B:
# 2816
 Cyc_Toc_exp_to_c(nv,_tmp524);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp525);
goto _LL215;_LL22C: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp526=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp526->tag != 9)goto _LL22E;else{_tmp527=_tmp526->f1;_tmp528=_tmp526->f2;if(_tmp526->f3 == 0)goto _LL22E;_tmp529=(_tmp526->f3)->num_varargs;_tmp52A=(_tmp526->f3)->injectors;_tmp52B=(_tmp526->f3)->vai;}}_LL22D: {
# 2828 "toc.cyc"
struct _RegionHandle _tmp5D4=_new_region("r");struct _RegionHandle*r=& _tmp5D4;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp529,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp52B->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2836
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp528);
int num_normargs=num_args - _tmp529;
# 2840
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp528=_tmp528->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp528))->hd);{
struct Cyc_List_List*_tmpC8A;new_args=((_tmpC8A=_cycalloc(sizeof(*_tmpC8A)),((_tmpC8A->hd=(struct Cyc_Absyn_Exp*)_tmp528->hd,((_tmpC8A->tl=new_args,_tmpC8A))))));};}}
# 2845
{struct Cyc_Absyn_Exp*_tmpC8D[3];struct Cyc_List_List*_tmpC8C;new_args=((_tmpC8C=_cycalloc(sizeof(*_tmpC8C)),((_tmpC8C->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC8D[2]=num_varargs_exp,((_tmpC8D[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC8D[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC8D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC8C->tl=new_args,_tmpC8C))))));}
# 2850
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2852
Cyc_Toc_exp_to_c(nv,_tmp527);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp527,new_args,0),0);
# 2856
if(_tmp52B->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5D8=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp52B->type));void*_tmp5D9=_tmp5D8;struct Cyc_Absyn_Datatypedecl*_tmp5DB;_LL29C: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5DA=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5D9;if(_tmp5DA->tag != 3)goto _LL29E;else{if((((_tmp5DA->f1).datatype_info).KnownDatatype).tag != 2)goto _LL29E;_tmp5DB=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5DA->f1).datatype_info).KnownDatatype).val);}}_LL29D:
 tud=_tmp5DB;goto _LL29B;_LL29E:;_LL29F: {
const char*_tmpC90;void*_tmpC8F;(_tmpC8F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC90="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC90,sizeof(char),44))),_tag_dyneither(_tmpC8F,sizeof(void*),0)));}_LL29B:;}{
# 2862
unsigned int _tmpC95;unsigned int _tmpC94;struct _dyneither_ptr _tmpC93;struct _tuple1**_tmpC92;unsigned int _tmpC91;struct _dyneither_ptr vs=(_tmpC91=(unsigned int)_tmp529,((_tmpC92=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpC91)),((_tmpC93=_tag_dyneither(_tmpC92,sizeof(struct _tuple1*),_tmpC91),((((_tmpC94=_tmpC91,_tmpC96(& _tmpC95,& _tmpC94,& _tmpC92))),_tmpC93)))))));
# 2864
if(_tmp529 != 0){
# 2866
struct Cyc_List_List*_tmp5DE=0;
{int i=_tmp529 - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC97;_tmp5DE=((_tmpC97=_cycalloc(sizeof(*_tmpC97)),((_tmpC97->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpC97->tl=_tmp5DE,_tmpC97))))));}}
# 2870
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5DE,0),s,0);{
# 2873
int i=0;for(0;_tmp528 != 0;(((_tmp528=_tmp528->tl,_tmp52A=_tmp52A->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp528->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5E0=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp52A))->hd;struct _tuple1*_tmp5E2;struct Cyc_List_List*_tmp5E3;struct Cyc_Absyn_Datatypefield*_tmp5E1=_tmp5E0;_tmp5E2=_tmp5E1->name;_tmp5E3=_tmp5E1->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5E3))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2888
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2891
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5E2),0),s,0);
# 2894
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5E2,tud->name)),0,s,0);};}};}else{
# 2901
struct _tuple19*_tmpC98[3];struct Cyc_List_List*_tmp5E4=(_tmpC98[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC98[1]=
# 2901
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC98[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC98,sizeof(struct _tuple19*),3)))))));
# 2903
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2913
{int i=0;for(0;_tmp528 != 0;(_tmp528=_tmp528->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp528->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp528->hd,0),s,0);}}
# 2919
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2932 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2934
_npop_handler(0);goto _LL215;
# 2828 "toc.cyc"
;_pop_region(r);}_LL22E: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp52C->tag != 10)goto _LL230;else{_tmp52D=_tmp52C->f1;}}_LL22F:
# 2937 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp52D);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp52D),0))->r;
goto _LL215;_LL230: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp52E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp52E->tag != 11)goto _LL232;else{_tmp52F=_tmp52E->f1;}}_LL231:
 Cyc_Toc_exp_to_c(nv,_tmp52F);goto _LL215;_LL232: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp530=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp530->tag != 12)goto _LL234;else{_tmp531=_tmp530->f1;_tmp532=_tmp530->f2;}}_LL233:
# 2942
 Cyc_Toc_exp_to_c(nv,_tmp531);
# 2951 "toc.cyc"
for(0;_tmp532 != 0;_tmp532=_tmp532->tl){
enum Cyc_Absyn_KindQual _tmp5EA=(Cyc_Tcutil_typ_kind((void*)_tmp532->hd))->kind;
if(_tmp5EA != Cyc_Absyn_EffKind  && _tmp5EA != Cyc_Absyn_RgnKind){
{void*_tmp5EB=Cyc_Tcutil_compress((void*)_tmp532->hd);void*_tmp5EC=_tmp5EB;_LL2A1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5ED=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5EC;if(_tmp5ED->tag != 2)goto _LL2A3;}_LL2A2:
 goto _LL2A4;_LL2A3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5EE=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5EC;if(_tmp5EE->tag != 3)goto _LL2A5;}_LL2A4:
 continue;_LL2A5:;_LL2A6:
# 2958
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp531,0))->r;
goto _LL2A0;_LL2A0:;}
# 2961
break;}}
# 2964
goto _LL215;_LL234: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp533->tag != 13)goto _LL236;else{_tmp534=(void**)((void**)& _tmp533->f1);_tmp535=_tmp533->f2;_tmp536=_tmp533->f3;_tmp537=_tmp533->f4;}}_LL235: {
# 2966
void*old_t2=(void*)_check_null(_tmp535->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp534;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp534=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp535);
# 2973
{struct _tuple29 _tmpC99;struct _tuple29 _tmp5EF=(_tmpC99.f1=Cyc_Tcutil_compress(old_t2),((_tmpC99.f2=Cyc_Tcutil_compress(new_typ),_tmpC99)));struct _tuple29 _tmp5F0=_tmp5EF;struct Cyc_Absyn_PtrInfo _tmp5F2;struct Cyc_Absyn_PtrInfo _tmp5F4;struct Cyc_Absyn_PtrInfo _tmp5F6;_LL2A8:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F0.f1;if(_tmp5F1->tag != 5)goto _LL2AA;else{_tmp5F2=_tmp5F1->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F0.f2;if(_tmp5F3->tag != 5)goto _LL2AA;else{_tmp5F4=_tmp5F3->f1;}};_LL2A9: {
# 2975
int _tmp5F8=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F2.ptr_atts).nullable);
int _tmp5F9=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F4.ptr_atts).nullable);
void*_tmp5FA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F2.ptr_atts).bounds);
void*_tmp5FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F4.ptr_atts).bounds);
int _tmp5FC=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F2.ptr_atts).zero_term);
int _tmp5FD=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5F4.ptr_atts).zero_term);
{struct _tuple29 _tmpC9A;struct _tuple29 _tmp5FE=(_tmpC9A.f1=_tmp5FA,((_tmpC9A.f2=_tmp5FB,_tmpC9A)));struct _tuple29 _tmp5FF=_tmp5FE;struct Cyc_Absyn_Exp*_tmp601;struct Cyc_Absyn_Exp*_tmp603;struct Cyc_Absyn_Exp*_tmp605;struct Cyc_Absyn_Exp*_tmp609;_LL2AF:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp600=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FF.f1;if(_tmp600->tag != 1)goto _LL2B1;else{_tmp601=_tmp600->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp602=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FF.f2;if(_tmp602->tag != 1)goto _LL2B1;else{_tmp603=_tmp602->f1;}};_LL2B0:
# 2983
 if((!Cyc_Evexp_c_can_eval(_tmp601) || !Cyc_Evexp_c_can_eval(_tmp603)) && !
Cyc_Evexp_same_const_exp(_tmp601,_tmp603)){
const char*_tmpC9D;void*_tmpC9C;(_tmpC9C=0,Cyc_Tcutil_terr(e->loc,((_tmpC9D="can't validate cast due to potential size differences",_tag_dyneither(_tmpC9D,sizeof(char),54))),_tag_dyneither(_tmpC9C,sizeof(void*),0)));}
if(_tmp5F8  && !_tmp5F9){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCA0;void*_tmpC9F;(_tmpC9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA0="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpCA0,sizeof(char),44))),_tag_dyneither(_tmpC9F,sizeof(void*),0)));}
# 2992
if(_tmp537 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpCA4;void*_tmpCA3[1];struct Cyc_String_pa_PrintArg_struct _tmpCA2;(_tmpCA2.tag=0,((_tmpCA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCA3[0]=& _tmpCA2,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCA4="null-check conversion mis-classified: %s",_tag_dyneither(_tmpCA4,sizeof(char),41))),_tag_dyneither(_tmpCA3,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp535);
if(do_null_check){
if(!_tmp536){
const char*_tmpCA7;void*_tmpCA6;(_tmpCA6=0,Cyc_Tcutil_warn(e->loc,((_tmpCA7="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpCA7,sizeof(char),58))),_tag_dyneither(_tmpCA6,sizeof(void*),0)));}{
# 2999
struct Cyc_List_List*_tmpCA8;e->r=Cyc_Toc_cast_it_r(*_tmp534,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCA8=_cycalloc(sizeof(*_tmpCA8)),((_tmpCA8->hd=_tmp535,((_tmpCA8->tl=0,_tmpCA8)))))),0));};}else{
# 3003
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp535->r;}};}else{
# 3008
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp535->r;}
# 3015
goto _LL2AE;_LL2B1:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp604=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FF.f1;if(_tmp604->tag != 1)goto _LL2B3;else{_tmp605=_tmp604->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp606=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FF.f2;if(_tmp606->tag != 0)goto _LL2B3;};_LL2B2:
# 3017
 if(!Cyc_Evexp_c_can_eval(_tmp605)){
const char*_tmpCAB;void*_tmpCAA;(_tmpCAA=0,Cyc_Tcutil_terr(e->loc,((_tmpCAB="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCAB,sizeof(char),71))),_tag_dyneither(_tmpCAA,sizeof(void*),0)));}
# 3020
if(_tmp537 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpCAF;void*_tmpCAE[1];struct Cyc_String_pa_PrintArg_struct _tmpCAD;(_tmpCAD.tag=0,((_tmpCAD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCAE[0]=& _tmpCAD,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCAF="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpCAF,sizeof(char),44))),_tag_dyneither(_tmpCAE,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3024
if((_tmp5FC  && !(_tmp5F4.elt_tq).real_const) && !_tmp5FD)
# 3027
_tmp605=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp605,Cyc_Absyn_uint_exp(1,0),0);
# 3029
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp605,_tmp535))->r;}else{
# 3031
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Toc__tag_dyneither_e;
# 3033
if(_tmp5FC){
# 3038
struct _tuple1*_tmp61C=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp61D=Cyc_Absyn_var_exp(_tmp61C,0);
struct Cyc_Absyn_Exp*arg3;
# 3043
{void*_tmp61E=_tmp535->r;void*_tmp61F=_tmp61E;_LL2B8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp620=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp61F;if(_tmp620->tag != 0)goto _LL2BA;else{if(((_tmp620->f1).String_c).tag != 8)goto _LL2BA;}}_LL2B9:
# 3045
 arg3=_tmp605;goto _LL2B7;_LL2BA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp621=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp61F;if(_tmp621->tag != 0)goto _LL2BC;else{if(((_tmp621->f1).Wstring_c).tag != 9)goto _LL2BC;}}_LL2BB:
# 3047
 arg3=_tmp605;goto _LL2B7;_LL2BC:;_LL2BD:
# 3049
{struct Cyc_Absyn_Exp*_tmpCB0[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp535),(
# 3051
(_tmpCB0[1]=_tmp605,((_tmpCB0[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp61D),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB0,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B7;_LL2B7:;}
# 3054
if(!_tmp5FD  && !(_tmp5F4.elt_tq).real_const)
# 3057
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3059
struct Cyc_Absyn_Exp*_tmp623=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F4.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpCB1[3];struct Cyc_Absyn_Exp*_tmp624=Cyc_Absyn_fncall_exp(_tmp61B,((_tmpCB1[2]=arg3,((_tmpCB1[1]=_tmp623,((_tmpCB1[0]=_tmp61D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB1,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp625=Cyc_Absyn_exp_stmt(_tmp624,0);
_tmp625=Cyc_Absyn_declare_stmt(_tmp61C,Cyc_Toc_typ_to_c(old_t2),_tmp535,_tmp625,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp625);};}else{
# 3066
struct Cyc_Absyn_Exp*_tmpCB2[3];e->r=Cyc_Toc_fncall_exp_r(_tmp61B,(
(_tmpCB2[2]=_tmp605,((_tmpCB2[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F4.elt_typ),0),((_tmpCB2[0]=_tmp535,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB2,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3072
goto _LL2AE;_LL2B3:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp607=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FF.f1;if(_tmp607->tag != 0)goto _LL2B5;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp608=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5FF.f2;if(_tmp608->tag != 1)goto _LL2B5;else{_tmp609=_tmp608->f1;}};_LL2B4:
# 3074
 if(!Cyc_Evexp_c_can_eval(_tmp609)){
const char*_tmpCB5;void*_tmpCB4;(_tmpCB4=0,Cyc_Tcutil_terr(e->loc,((_tmpCB5="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpCB5,sizeof(char),71))),_tag_dyneither(_tmpCB4,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCB8;void*_tmpCB7;(_tmpCB7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCB8="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpCB8,sizeof(char),45))),_tag_dyneither(_tmpCB7,sizeof(void*),0)));}{
# 3086 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp62C=_tmp609;
if(_tmp5FC  && !_tmp5FD)
_tmp62C=Cyc_Absyn_add_exp(_tmp609,Cyc_Absyn_uint_exp(1,0),0);{
# 3093
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpCB9[3];struct Cyc_Absyn_Exp*_tmp62E=Cyc_Absyn_fncall_exp(_tmp62D,(
(_tmpCB9[2]=_tmp62C,((_tmpCB9[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F2.elt_typ),0),((_tmpCB9[0]=_tmp535,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCB9,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3098
if(_tmp5F9){
struct Cyc_List_List*_tmpCBA;_tmp62E->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpCBA=_cycalloc(sizeof(*_tmpCBA)),((_tmpCBA->hd=Cyc_Absyn_copy_exp(_tmp62E),((_tmpCBA->tl=0,_tmpCBA)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp534,_tmp62E);
goto _LL2AE;};};_LL2B5:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp60A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FF.f1;if(_tmp60A->tag != 0)goto _LL2AE;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp60B=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5FF.f2;if(_tmp60B->tag != 0)goto _LL2AE;};_LL2B6:
# 3106
 DynCast:
 if((_tmp5FC  && !_tmp5FD) && !(_tmp5F4.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCBD;void*_tmpCBC;(_tmpCBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCBD="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpCBD,sizeof(char),70))),_tag_dyneither(_tmpCBC,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp633=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpCBE[3];e->r=Cyc_Toc_fncall_exp_r(_tmp633,(
(_tmpCBE[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpCBE[1]=
# 3112
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5F2.elt_typ),0),((_tmpCBE[0]=_tmp535,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCBE,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3115
goto _LL2AE;_LL2AE:;}
# 3117
goto _LL2A7;}_LL2AA:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5F5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F0.f1;if(_tmp5F5->tag != 5)goto _LL2AC;else{_tmp5F6=_tmp5F5->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5F7=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5F0.f2;if(_tmp5F7->tag != 6)goto _LL2AC;};_LL2AB:
# 3119
{void*_tmp636=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5F6.ptr_atts).bounds);void*_tmp637=_tmp636;_LL2BF: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp638=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp637;if(_tmp638->tag != 0)goto _LL2C1;}_LL2C0:
# 3121
 _tmp535->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp535->r,_tmp535->loc),Cyc_Toc_curr_sp);goto _LL2BE;_LL2C1:;_LL2C2:
 goto _LL2BE;_LL2BE:;}
# 3124
goto _LL2A7;_LL2AC:;_LL2AD:
# 3126
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp535->r;
goto _LL2A7;_LL2A7:;}
# 3130
goto _LL215;}_LL236: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp538=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp538->tag != 14)goto _LL238;else{_tmp539=_tmp538->f1;}}_LL237:
# 3134
{void*_tmp63A=_tmp539->r;void*_tmp63B=_tmp63A;struct _tuple1*_tmp63D;struct Cyc_List_List*_tmp63E;struct Cyc_List_List*_tmp63F;struct Cyc_List_List*_tmp641;_LL2C4: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp63C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp63B;if(_tmp63C->tag != 28)goto _LL2C6;else{_tmp63D=_tmp63C->f1;_tmp63E=_tmp63C->f2;_tmp63F=_tmp63C->f3;}}_LL2C5:
# 3136
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC2;void*_tmpCC1[1];struct Cyc_String_pa_PrintArg_struct _tmpCC0;(_tmpCC0.tag=0,((_tmpCC0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp539->loc)),((_tmpCC1[0]=& _tmpCC0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC2="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC2,sizeof(char),42))),_tag_dyneither(_tmpCC1,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp645=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp539->topt),_tmp63E,1,0,_tmp63F,_tmp63D);
e->r=_tmp645->r;
e->topt=_tmp645->topt;
goto _LL2C3;};_LL2C6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp63B;if(_tmp640->tag != 23)goto _LL2C8;else{_tmp641=_tmp640->f1;}}_LL2C7:
# 3144
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCC6;void*_tmpCC5[1];struct Cyc_String_pa_PrintArg_struct _tmpCC4;(_tmpCC4.tag=0,((_tmpCC4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp539->loc)),((_tmpCC5[0]=& _tmpCC4,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCC6="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpCC6,sizeof(char),42))),_tag_dyneither(_tmpCC5,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp641))->r;
goto _LL2C3;_LL2C8:;_LL2C9:
# 3150
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp539);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp539)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp539,0,Cyc_Toc_address_lvalue,1);
# 3156
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp539);}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp539->topt))))
# 3160
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp539);}
# 3162
goto _LL2C3;_LL2C3:;}
# 3164
goto _LL215;_LL238: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp53A=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp53A->tag != 15)goto _LL23A;else{_tmp53B=_tmp53A->f1;_tmp53C=_tmp53A->f2;}}_LL239:
# 3167
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCCA;void*_tmpCC9[1];struct Cyc_String_pa_PrintArg_struct _tmpCC8;(_tmpCC8.tag=0,((_tmpCC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp53C->loc)),((_tmpCC9[0]=& _tmpCC8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCCA="%s: new at top-level",_tag_dyneither(_tmpCCA,sizeof(char),21))),_tag_dyneither(_tmpCC9,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp53C->topt);
{void*_tmp64C=_tmp53C->r;void*_tmp64D=_tmp64C;struct Cyc_List_List*_tmp64F;struct Cyc_Absyn_Vardecl*_tmp651;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Exp*_tmp653;int _tmp654;struct Cyc_Absyn_Exp*_tmp656;void*_tmp657;int _tmp658;struct _tuple1*_tmp65A;struct Cyc_List_List*_tmp65B;struct Cyc_List_List*_tmp65C;struct Cyc_Absyn_Aggrdecl*_tmp65D;struct Cyc_List_List*_tmp65F;_LL2CB: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp64E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp64D;if(_tmp64E->tag != 25)goto _LL2CD;else{_tmp64F=_tmp64E->f1;}}_LL2CC: {
# 3175
struct _tuple1*_tmp660=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp661=Cyc_Absyn_var_exp(_tmp660,0);
struct Cyc_Absyn_Stmt*_tmp662=Cyc_Toc_init_array(nv,new_e_type,_tmp661,_tmp64F,Cyc_Absyn_exp_stmt(_tmp661,0));
void*old_elt_typ;
{void*_tmp663=Cyc_Tcutil_compress(old_typ);void*_tmp664=_tmp663;void*_tmp666;struct Cyc_Absyn_Tqual _tmp667;union Cyc_Absyn_Constraint*_tmp668;_LL2D8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp665=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp664;if(_tmp665->tag != 5)goto _LL2DA;else{_tmp666=(_tmp665->f1).elt_typ;_tmp667=(_tmp665->f1).elt_tq;_tmp668=((_tmp665->f1).ptr_atts).zero_term;}}_LL2D9:
# 3181
 old_elt_typ=_tmp666;goto _LL2D7;_LL2DA:;_LL2DB: {
# 3183
const char*_tmpCCD;void*_tmpCCC;old_elt_typ=(
(_tmpCCC=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCCD="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCCD,sizeof(char),52))),_tag_dyneither(_tmpCCC,sizeof(void*),0))));}_LL2D7:;}{
# 3186
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp66B=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp66C=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp64F),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp53B == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp66C);else{
# 3194
struct Cyc_Absyn_Exp*r=_tmp53B;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp66C);}
# 3199
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp660,_tmp66B,e1,_tmp662,0));
goto _LL2CA;};}_LL2CD: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp64D;if(_tmp650->tag != 26)goto _LL2CF;else{_tmp651=_tmp650->f1;_tmp652=_tmp650->f2;_tmp653=_tmp650->f3;_tmp654=_tmp650->f4;}}_LL2CE:
# 3203
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp53B,old_typ,_tmp652,(void*)_check_null(_tmp653->topt),_tmp654,_tmp653,_tmp651);
goto _LL2CA;_LL2CF: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp655=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp64D;if(_tmp655->tag != 27)goto _LL2D1;else{_tmp656=_tmp655->f1;_tmp657=(void*)_tmp655->f2;_tmp658=_tmp655->f3;}}_LL2D0:
# 3207
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp53B,old_typ,_tmp656,_tmp657,_tmp658,0,0);
goto _LL2CA;_LL2D1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp659=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp64D;if(_tmp659->tag != 28)goto _LL2D3;else{_tmp65A=_tmp659->f1;_tmp65B=_tmp659->f2;_tmp65C=_tmp659->f3;_tmp65D=_tmp659->f4;}}_LL2D2: {
# 3212
struct Cyc_Absyn_Exp*_tmp66D=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp53C->topt),_tmp65B,1,_tmp53B,_tmp65C,_tmp65A);
e->r=_tmp66D->r;
e->topt=_tmp66D->topt;
goto _LL2CA;}_LL2D3: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp65E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp64D;if(_tmp65E->tag != 23)goto _LL2D5;else{_tmp65F=_tmp65E->f1;}}_LL2D4:
# 3218
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp53B,_tmp65F))->r;
goto _LL2CA;_LL2D5:;_LL2D6: {
# 3225
void*old_elt_typ=(void*)_check_null(_tmp53C->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3228
struct _tuple1*_tmp66E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp66F=Cyc_Absyn_var_exp(_tmp66E,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp66F,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp53B == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3235
struct Cyc_Absyn_Exp*r=_tmp53B;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3243
int done=0;
{void*_tmp670=_tmp53C->r;void*_tmp671=_tmp670;void*_tmp673;struct Cyc_Absyn_Exp*_tmp674;_LL2DD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp672=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp671;if(_tmp672->tag != 13)goto _LL2DF;else{_tmp673=(void*)_tmp672->f1;_tmp674=_tmp672->f2;}}_LL2DE:
# 3246
{struct _tuple29 _tmpCCE;struct _tuple29 _tmp675=(_tmpCCE.f1=Cyc_Tcutil_compress(_tmp673),((_tmpCCE.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp674->topt)),_tmpCCE)));struct _tuple29 _tmp676=_tmp675;void*_tmp678;union Cyc_Absyn_Constraint*_tmp679;union Cyc_Absyn_Constraint*_tmp67B;_LL2E2:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp677=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp676.f1;if(_tmp677->tag != 5)goto _LL2E4;else{_tmp678=(_tmp677->f1).elt_typ;_tmp679=((_tmp677->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp676.f2;if(_tmp67A->tag != 5)goto _LL2E4;else{_tmp67B=((_tmp67A->f1).ptr_atts).bounds;}};_LL2E3:
# 3249
{struct _tuple29 _tmpCCF;struct _tuple29 _tmp67C=(_tmpCCF.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp679),((_tmpCCF.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp67B),_tmpCCF)));struct _tuple29 _tmp67D=_tmp67C;struct Cyc_Absyn_Exp*_tmp680;_LL2E7:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp67E=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp67D.f1;if(_tmp67E->tag != 0)goto _LL2E9;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp67F=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67D.f2;if(_tmp67F->tag != 1)goto _LL2E9;else{_tmp680=_tmp67F->f1;}};_LL2E8:
# 3251
 Cyc_Toc_exp_to_c(nv,_tmp674);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp681=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCD0[4];e->r=Cyc_Toc_fncall_exp_r(_tmp681,(
(_tmpCD0[3]=_tmp680,((_tmpCD0[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp678),0),((_tmpCD0[1]=_tmp674,((_tmpCD0[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD0,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3259
goto _LL2E6;};_LL2E9:;_LL2EA:
 goto _LL2E6;_LL2E6:;}
# 3262
goto _LL2E1;_LL2E4:;_LL2E5:
 goto _LL2E1;_LL2E1:;}
# 3265
goto _LL2DC;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3268
if(!done){
struct Cyc_Absyn_Stmt*_tmp685=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp66F),0);
struct Cyc_Absyn_Exp*_tmp686=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp53C);
_tmp685=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp66F,_tmp686,0),_tmp53C,0),_tmp685,0);{
# 3274
void*_tmp687=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp66E,_tmp687,mexp,_tmp685,0));};}
# 3277
goto _LL2CA;};}_LL2CA:;}
# 3279
goto _LL215;};_LL23A: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp53D->tag != 17)goto _LL23C;else{_tmp53E=_tmp53D->f1;}}_LL23B: {
# 3282
int _tmp688=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp53E);
Cyc_Toc_set_in_sizeof(nv,_tmp688);
goto _LL215;}_LL23C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp53F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp53F->tag != 16)goto _LL23E;else{_tmp540=(void*)_tmp53F->f1;}}_LL23D:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCD3;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCD2;e->r=(void*)((_tmpCD2=_cycalloc(sizeof(*_tmpCD2)),((_tmpCD2[0]=((_tmpCD3.tag=16,((_tmpCD3.f1=Cyc_Toc_typ_to_c(_tmp540),_tmpCD3)))),_tmpCD2))));}goto _LL215;_LL23E: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp541=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp541->tag != 18)goto _LL240;else{_tmp542=(void*)_tmp541->f1;_tmp543=_tmp541->f2;}}_LL23F: {
# 3288
void*_tmp68B=_tmp542;
struct Cyc_List_List*_tmp68C=_tmp543;
for(0;_tmp68C != 0;_tmp68C=_tmp68C->tl){
void*_tmp68D=(void*)_tmp68C->hd;void*_tmp68E=_tmp68D;struct _dyneither_ptr*_tmp690;unsigned int _tmp692;_LL2EC: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp68F=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp68E;if(_tmp68F->tag != 0)goto _LL2EE;else{_tmp690=_tmp68F->f1;}}_LL2ED:
 goto _LL2EB;_LL2EE: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp691=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp68E;if(_tmp691->tag != 1)goto _LL2EB;else{_tmp692=_tmp691->f1;}}_LL2EF:
# 3294
{void*_tmp693=Cyc_Tcutil_compress(_tmp68B);void*_tmp694=_tmp693;union Cyc_Absyn_AggrInfoU _tmp696;struct Cyc_List_List*_tmp698;struct Cyc_List_List*_tmp69A;struct Cyc_Absyn_Datatypefield*_tmp69C;_LL2F1: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp695=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp694;if(_tmp695->tag != 11)goto _LL2F3;else{_tmp696=(_tmp695->f1).aggr_info;}}_LL2F2: {
# 3296
struct Cyc_Absyn_Aggrdecl*_tmp69D=Cyc_Absyn_get_known_aggrdecl(_tmp696);
if(_tmp69D->impl == 0){
const char*_tmpCD6;void*_tmpCD5;(_tmpCD5=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCD6="struct fields must be known",_tag_dyneither(_tmpCD6,sizeof(char),28))),_tag_dyneither(_tmpCD5,sizeof(void*),0)));}
_tmp698=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp69D->impl))->fields;goto _LL2F4;}_LL2F3: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp697=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp694;if(_tmp697->tag != 12)goto _LL2F5;else{_tmp698=_tmp697->f2;}}_LL2F4: {
# 3301
struct Cyc_Absyn_Aggrfield*_tmp6A0=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp698,(int)_tmp692);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCD9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCD8;_tmp68C->hd=(void*)((void*)((_tmpCD8=_cycalloc(sizeof(*_tmpCD8)),((_tmpCD8[0]=((_tmpCD9.tag=0,((_tmpCD9.f1=_tmp6A0->name,_tmpCD9)))),_tmpCD8)))));}
_tmp68B=_tmp6A0->type;
goto _LL2F0;}_LL2F5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp699=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp694;if(_tmp699->tag != 10)goto _LL2F7;else{_tmp69A=_tmp699->f1;}}_LL2F6:
# 3306
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCDC;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCDB;_tmp68C->hd=(void*)((void*)((_tmpCDB=_cycalloc(sizeof(*_tmpCDB)),((_tmpCDB[0]=((_tmpCDC.tag=0,((_tmpCDC.f1=Cyc_Absyn_fieldname((int)(_tmp692 + 1)),_tmpCDC)))),_tmpCDB)))));}
_tmp68B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp69A,(int)_tmp692)).f2;
goto _LL2F0;_LL2F7: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp69B=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp694;if(_tmp69B->tag != 4)goto _LL2F9;else{if((((_tmp69B->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F9;_tmp69C=((struct _tuple2)(((_tmp69B->f1).field_info).KnownDatatypefield).val).f2;}}_LL2F8:
# 3310
 if(_tmp692 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCDF;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCDE;_tmp68C->hd=(void*)((void*)((_tmpCDE=_cycalloc(sizeof(*_tmpCDE)),((_tmpCDE[0]=((_tmpCDF.tag=0,((_tmpCDF.f1=Cyc_Toc_tag_sp,_tmpCDF)))),_tmpCDE)))));}else{
# 3313
_tmp68B=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp69C->typs,(int)(_tmp692 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCE2;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCE1;_tmp68C->hd=(void*)((void*)((_tmpCE1=_cycalloc(sizeof(*_tmpCE1)),((_tmpCE1[0]=((_tmpCE2.tag=0,((_tmpCE2.f1=Cyc_Absyn_fieldname((int)_tmp692),_tmpCE2)))),_tmpCE1)))));};}
# 3316
goto _LL2F0;_LL2F9:;_LL2FA:
{const char*_tmpCE5;void*_tmpCE4;(_tmpCE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCE5="impossible type for offsetof tuple index",_tag_dyneither(_tmpCE5,sizeof(char),41))),_tag_dyneither(_tmpCE4,sizeof(void*),0)));}
goto _LL2F0;_LL2F0:;}
# 3320
goto _LL2EB;_LL2EB:;}
# 3323
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCE8;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCE7;e->r=(void*)((_tmpCE7=_cycalloc(sizeof(*_tmpCE7)),((_tmpCE7[0]=((_tmpCE8.tag=18,((_tmpCE8.f1=Cyc_Toc_typ_to_c(_tmp542),((_tmpCE8.f2=_tmp543,_tmpCE8)))))),_tmpCE7))));}
goto _LL215;}_LL240: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp544=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp544->tag != 19)goto _LL242;else{_tmp545=_tmp544->f1;}}_LL241: {
# 3326
int _tmp6AD=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6AE=Cyc_Tcutil_compress((void*)_check_null(_tmp545->topt));
{void*_tmp6AF=_tmp6AE;void*_tmp6B1;struct Cyc_Absyn_Tqual _tmp6B2;void*_tmp6B3;union Cyc_Absyn_Constraint*_tmp6B4;union Cyc_Absyn_Constraint*_tmp6B5;union Cyc_Absyn_Constraint*_tmp6B6;_LL2FC: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6B0=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6AF;if(_tmp6B0->tag != 5)goto _LL2FE;else{_tmp6B1=(_tmp6B0->f1).elt_typ;_tmp6B2=(_tmp6B0->f1).elt_tq;_tmp6B3=((_tmp6B0->f1).ptr_atts).rgn;_tmp6B4=((_tmp6B0->f1).ptr_atts).nullable;_tmp6B5=((_tmp6B0->f1).ptr_atts).bounds;_tmp6B6=((_tmp6B0->f1).ptr_atts).zero_term;}}_LL2FD:
# 3331
{void*_tmp6B7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6B5);void*_tmp6B8=_tmp6B7;struct Cyc_Absyn_Exp*_tmp6BA;_LL301: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6B9=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6B8;if(_tmp6B9->tag != 1)goto _LL303;else{_tmp6BA=_tmp6B9->f1;}}_LL302: {
# 3333
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp545);
Cyc_Toc_exp_to_c(nv,_tmp545);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCEB;void*_tmpCEA;(_tmpCEA=0,Cyc_Tcutil_warn(e->loc,((_tmpCEB="inserted null check due to dereference",_tag_dyneither(_tmpCEB,sizeof(char),39))),_tag_dyneither(_tmpCEA,sizeof(void*),0)));}{
# 3339
struct Cyc_List_List*_tmpCEC;_tmp545->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6AE),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCEC=_cycalloc(sizeof(*_tmpCEC)),((_tmpCEC->hd=Cyc_Absyn_copy_exp(_tmp545),((_tmpCEC->tl=0,_tmpCEC)))))),0));};}
# 3347
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6B6)){
struct _tuple12 _tmp6BF=Cyc_Evexp_eval_const_uint_exp(_tmp6BA);unsigned int _tmp6C1;int _tmp6C2;struct _tuple12 _tmp6C0=_tmp6BF;_tmp6C1=_tmp6C0.f1;_tmp6C2=_tmp6C0.f2;
# 3353
if(!_tmp6C2  || _tmp6C1 <= 0){
const char*_tmpCEF;void*_tmpCEE;(_tmpCEE=0,Cyc_Tcutil_terr(e->loc,((_tmpCEF="cannot determine dereference is in bounds",_tag_dyneither(_tmpCEF,sizeof(char),42))),_tag_dyneither(_tmpCEE,sizeof(void*),0)));}}
# 3356
goto _LL300;}_LL303: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6BB=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6B8;if(_tmp6BB->tag != 0)goto _LL300;}_LL304: {
# 3359
struct Cyc_Absyn_Exp*_tmp6C5=Cyc_Absyn_uint_exp(0,0);
_tmp6C5->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp545,_tmp6C5);
Cyc_Toc_exp_to_c(nv,e);
goto _LL300;}_LL300:;}
# 3365
goto _LL2FB;_LL2FE:;_LL2FF: {
const char*_tmpCF2;void*_tmpCF1;(_tmpCF1=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF2="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCF2,sizeof(char),29))),_tag_dyneither(_tmpCF1,sizeof(void*),0)));}_LL2FB:;}
# 3368
Cyc_Toc_set_lhs(nv,_tmp6AD);
goto _LL215;};}_LL242: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp546=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp546->tag != 20)goto _LL244;else{_tmp547=_tmp546->f1;_tmp548=_tmp546->f2;_tmp549=_tmp546->f3;_tmp54A=_tmp546->f4;}}_LL243: {
# 3371
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp547->topt);
Cyc_Toc_exp_to_c(nv,_tmp547);
if(_tmp549  && _tmp54A)
e->r=Cyc_Toc_check_tagged_union(_tmp547,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp548,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3379
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL215;}_LL244: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp54B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp54B->tag != 21)goto _LL246;else{_tmp54C=_tmp54B->f1;_tmp54D=_tmp54B->f2;_tmp54E=_tmp54B->f3;_tmp54F=_tmp54B->f4;}}_LL245: {
# 3383
int _tmp6C8=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp54C->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp54C);
Cyc_Toc_exp_to_c(nv,_tmp54C);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6C9=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6CB;struct Cyc_Absyn_Tqual _tmp6CC;void*_tmp6CD;union Cyc_Absyn_Constraint*_tmp6CE;union Cyc_Absyn_Constraint*_tmp6CF;union Cyc_Absyn_Constraint*_tmp6D0;struct Cyc_Absyn_PtrInfo _tmp6CA=_tmp6C9;_tmp6CB=_tmp6CA.elt_typ;_tmp6CC=_tmp6CA.elt_tq;_tmp6CD=(_tmp6CA.ptr_atts).rgn;_tmp6CE=(_tmp6CA.ptr_atts).nullable;_tmp6CF=(_tmp6CA.ptr_atts).bounds;_tmp6D0=(_tmp6CA.ptr_atts).zero_term;
{void*_tmp6D1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6CF);void*_tmp6D2=_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D4;_LL306: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6D3=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6D2;if(_tmp6D3->tag != 1)goto _LL308;else{_tmp6D4=_tmp6D3->f1;}}_LL307: {
# 3394
struct _tuple12 _tmp6D6=Cyc_Evexp_eval_const_uint_exp(_tmp6D4);unsigned int _tmp6D8;int _tmp6D9;struct _tuple12 _tmp6D7=_tmp6D6;_tmp6D8=_tmp6D7.f1;_tmp6D9=_tmp6D7.f2;
if(_tmp6D9){
if(_tmp6D8 < 1){
const char*_tmpCF5;void*_tmpCF4;(_tmpCF4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCF5="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCF5,sizeof(char),44))),_tag_dyneither(_tmpCF4,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCF8;void*_tmpCF7;(_tmpCF7=0,Cyc_Tcutil_warn(e->loc,((_tmpCF8="inserted null check due to dereference",_tag_dyneither(_tmpCF8,sizeof(char),39))),_tag_dyneither(_tmpCF7,sizeof(void*),0)));}{
# 3402
struct Cyc_Absyn_Exp*_tmpCF9[1];_tmp54C->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCF9[0]=Cyc_Absyn_new_exp(_tmp54C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF9,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3407
if(!Cyc_Evexp_c_can_eval(_tmp6D4)){
const char*_tmpCFC;void*_tmpCFB;(_tmpCFB=0,Cyc_Tcutil_terr(e->loc,((_tmpCFC="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCFC,sizeof(char),47))),_tag_dyneither(_tmpCFB,sizeof(void*),0)));}{
# 3411
struct Cyc_Absyn_Exp*_tmpCFD[4];_tmp54C->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCFD[3]=
# 3415
Cyc_Absyn_uint_exp(0,0),((_tmpCFD[2]=
# 3414
Cyc_Absyn_sizeoftyp_exp(_tmp6CB,0),((_tmpCFD[1]=_tmp6D4,((_tmpCFD[0]=
# 3413
Cyc_Absyn_new_exp(_tmp54C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFD,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3417
goto _LL305;}_LL308: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6D5=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6D2;if(_tmp6D5->tag != 0)goto _LL305;}_LL309: {
# 3420
void*ta1=Cyc_Toc_typ_to_c(_tmp6CB);
{struct Cyc_Absyn_Exp*_tmpCFE[3];_tmp54C->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6CC),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpCFE[2]=
# 3425
Cyc_Absyn_uint_exp(0,0),((_tmpCFE[1]=
# 3424
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCFE[0]=
# 3423
Cyc_Absyn_new_exp(_tmp54C->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCFE,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3428
goto _LL305;}_LL305:;}
# 3430
if(_tmp54E  && _tmp54F)
e->r=Cyc_Toc_check_tagged_union(_tmp54C,Cyc_Toc_typ_to_c(e1typ),_tmp6CB,_tmp54D,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3433
if(is_poly  && _tmp54F)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6C8);
goto _LL215;};};}_LL246: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp550=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp550->tag != 22)goto _LL248;else{_tmp551=_tmp550->f1;_tmp552=_tmp550->f2;}}_LL247: {
# 3438
int _tmp6E3=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6E4=Cyc_Tcutil_compress((void*)_check_null(_tmp551->topt));
# 3442
{void*_tmp6E5=_tmp6E4;struct Cyc_List_List*_tmp6E7;void*_tmp6E9;struct Cyc_Absyn_Tqual _tmp6EA;void*_tmp6EB;union Cyc_Absyn_Constraint*_tmp6EC;union Cyc_Absyn_Constraint*_tmp6ED;union Cyc_Absyn_Constraint*_tmp6EE;_LL30B: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6E6=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E6->tag != 10)goto _LL30D;else{_tmp6E7=_tmp6E6->f1;}}_LL30C:
# 3445
 Cyc_Toc_exp_to_c(nv,_tmp551);
Cyc_Toc_exp_to_c(nv,_tmp552);{
struct _tuple12 _tmp6EF=Cyc_Evexp_eval_const_uint_exp(_tmp552);unsigned int _tmp6F1;int _tmp6F2;struct _tuple12 _tmp6F0=_tmp6EF;_tmp6F1=_tmp6F0.f1;_tmp6F2=_tmp6F0.f2;
if(!_tmp6F2){
const char*_tmpD01;void*_tmpD00;(_tmpD00=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD01="unknown tuple subscript in translation to C",_tag_dyneither(_tmpD01,sizeof(char),44))),_tag_dyneither(_tmpD00,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp551,Cyc_Absyn_fieldname((int)(_tmp6F1 + 1)));
goto _LL30A;};_LL30D: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6E8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E5;if(_tmp6E8->tag != 5)goto _LL30F;else{_tmp6E9=(_tmp6E8->f1).elt_typ;_tmp6EA=(_tmp6E8->f1).elt_tq;_tmp6EB=((_tmp6E8->f1).ptr_atts).rgn;_tmp6EC=((_tmp6E8->f1).ptr_atts).nullable;_tmp6ED=((_tmp6E8->f1).ptr_atts).bounds;_tmp6EE=((_tmp6E8->f1).ptr_atts).zero_term;}}_LL30E: {
# 3453
struct Cyc_List_List*_tmp6F5=Cyc_Toc_get_relns(_tmp551);
int _tmp6F6=Cyc_Toc_need_null_check(_tmp551);
int _tmp6F7=Cyc_Toc_in_sizeof(nv);
# 3461
int in_bnds=_tmp6F7;
{void*_tmp6F8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6ED);void*_tmp6F9=_tmp6F8;_LL312:;_LL313:
# 3464
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6E4,_tmp6F5,_tmp551,_tmp552);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpD05;void*_tmpD04[1];struct Cyc_String_pa_PrintArg_struct _tmpD03;(_tmpD03.tag=0,((_tmpD03.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpD04[0]=& _tmpD03,Cyc_Tcutil_warn(e->loc,((_tmpD05="bounds check necessary for %s",_tag_dyneither(_tmpD05,sizeof(char),30))),_tag_dyneither(_tmpD04,sizeof(void*),1)))))));}_LL311:;}
# 3471
Cyc_Toc_exp_to_c(nv,_tmp551);
Cyc_Toc_exp_to_c(nv,_tmp552);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6FD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6ED);void*_tmp6FE=_tmp6FD;struct Cyc_Absyn_Exp*_tmp700;_LL315: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6FF=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6FE;if(_tmp6FF->tag != 1)goto _LL317;else{_tmp700=_tmp6FF->f1;}}_LL316: {
# 3476
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6EC) && _tmp6F6;
void*ta1=Cyc_Toc_typ_to_c(_tmp6E9);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6EA);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD08;void*_tmpD07;(_tmpD07=0,Cyc_Tcutil_warn(e->loc,((_tmpD08="inserted null check due to dereference",_tag_dyneither(_tmpD08,sizeof(char),39))),_tag_dyneither(_tmpD07,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpD09[1];_tmp551->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpD09[0]=Cyc_Absyn_copy_exp(_tmp551),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD09,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3488
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6EE)){
# 3490
if(!Cyc_Evexp_c_can_eval(_tmp700)){
const char*_tmpD0C;void*_tmpD0B;(_tmpD0B=0,Cyc_Tcutil_terr(e->loc,((_tmpD0C="cannot determine subscript is in bounds",_tag_dyneither(_tmpD0C,sizeof(char),40))),_tag_dyneither(_tmpD0B,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp551);
struct Cyc_Absyn_Exp*_tmpD0D[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpD0D[2]=_tmp552,((_tmpD0D[1]=_tmp700,((_tmpD0D[0]=_tmp551,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0D,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp700)){
const char*_tmpD10;void*_tmpD0F;(_tmpD0F=0,Cyc_Tcutil_terr(e->loc,((_tmpD10="cannot determine subscript is in bounds",_tag_dyneither(_tmpD10,sizeof(char),40))),_tag_dyneither(_tmpD0F,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpD13;void*_tmpD12;(_tmpD12=0,Cyc_Tcutil_warn(e->loc,((_tmpD13="inserted null check due to dereference",_tag_dyneither(_tmpD13,sizeof(char),39))),_tag_dyneither(_tmpD12,sizeof(void*),0)));}{
# 3502
struct Cyc_Absyn_Exp*_tmpD14[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpD14[3]=_tmp552,((_tmpD14[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD14[1]=_tmp700,((_tmpD14[0]=_tmp551,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD14,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3508
if(!Cyc_Evexp_c_can_eval(_tmp700)){
const char*_tmpD17;void*_tmpD16;(_tmpD16=0,Cyc_Tcutil_terr(e->loc,((_tmpD17="cannot determine subscript is in bounds",_tag_dyneither(_tmpD17,sizeof(char),40))),_tag_dyneither(_tmpD16,sizeof(void*),0)));}{
# 3511
struct Cyc_Absyn_Exp*_tmpD18[2];_tmp552->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpD18[1]=Cyc_Absyn_copy_exp(_tmp552),((_tmpD18[0]=_tmp700,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD18,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3514
goto _LL314;}_LL317: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp701=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6FE;if(_tmp701->tag != 0)goto _LL314;}_LL318: {
# 3516
void*ta1=Cyc_Toc_typ_to_c(_tmp6E9);
if(in_bnds){
# 3520
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EA),
Cyc_Toc_member_exp(_tmp551,Cyc_Toc_curr_sp,0)),_tmp552);}else{
# 3525
struct Cyc_Absyn_Exp*_tmp710=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpD19[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6EA),
Cyc_Absyn_fncall_exp(_tmp710,(
(_tmpD19[2]=_tmp552,((_tmpD19[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpD19[0]=_tmp551,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD19,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3531
goto _LL314;}_LL314:;}
# 3533
goto _LL30A;}_LL30F:;_LL310: {
const char*_tmpD1C;void*_tmpD1B;(_tmpD1B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD1C="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpD1C,sizeof(char),49))),_tag_dyneither(_tmpD1B,sizeof(void*),0)));}_LL30A:;}
# 3536
Cyc_Toc_set_lhs(nv,_tmp6E3);
goto _LL215;};}_LL248: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp553=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp553->tag != 23)goto _LL24A;else{_tmp554=_tmp553->f1;}}_LL249:
# 3539
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp554))->r;else{
# 3544
struct Cyc_List_List*_tmp714=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp554);
void*_tmp715=Cyc_Toc_add_tuple_type(_tmp714);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp554 != 0;(_tmp554=_tmp554->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp554->hd);{
struct _tuple19*_tmpD1F;struct Cyc_List_List*_tmpD1E;dles=((_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->hd=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->f1=0,((_tmpD1F->f2=(struct Cyc_Absyn_Exp*)_tmp554->hd,_tmpD1F)))))),((_tmpD1E->tl=dles,_tmpD1E))))));};}}
# 3551
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3554
goto _LL215;_LL24A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp555=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp555->tag != 25)goto _LL24C;else{_tmp556=_tmp555->f1;}}_LL24B:
# 3558
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp556);
{struct Cyc_List_List*_tmp718=_tmp556;for(0;_tmp718 != 0;_tmp718=_tmp718->tl){
struct _tuple19*_tmp719=(struct _tuple19*)_tmp718->hd;struct Cyc_Absyn_Exp*_tmp71B;struct _tuple19*_tmp71A=_tmp719;_tmp71B=_tmp71A->f2;
Cyc_Toc_exp_to_c(nv,_tmp71B);}}
# 3563
goto _LL215;_LL24C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp557->tag != 26)goto _LL24E;else{_tmp558=_tmp557->f1;_tmp559=_tmp557->f2;_tmp55A=_tmp557->f3;_tmp55B=_tmp557->f4;}}_LL24D: {
# 3567
struct _tuple12 _tmp71C=Cyc_Evexp_eval_const_uint_exp(_tmp559);unsigned int _tmp71E;int _tmp71F;struct _tuple12 _tmp71D=_tmp71C;_tmp71E=_tmp71D.f1;_tmp71F=_tmp71D.f2;{
void*_tmp720=Cyc_Toc_typ_to_c((void*)_check_null(_tmp55A->topt));
Cyc_Toc_exp_to_c(nv,_tmp55A);{
struct Cyc_List_List*es=0;
# 3572
if(!Cyc_Toc_is_zero(_tmp55A)){
if(!_tmp71F){
const char*_tmpD22;void*_tmpD21;(_tmpD21=0,Cyc_Tcutil_terr(_tmp559->loc,((_tmpD22="cannot determine value of constant",_tag_dyneither(_tmpD22,sizeof(char),35))),_tag_dyneither(_tmpD21,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp71E;++ i){
struct _tuple19*_tmpD25;struct Cyc_List_List*_tmpD24;es=((_tmpD24=_cycalloc(sizeof(*_tmpD24)),((_tmpD24->hd=((_tmpD25=_cycalloc(sizeof(*_tmpD25)),((_tmpD25->f1=0,((_tmpD25->f2=_tmp55A,_tmpD25)))))),((_tmpD24->tl=es,_tmpD24))))));}}
# 3578
if(_tmp55B){
struct Cyc_Absyn_Exp*_tmp725=Cyc_Toc_cast_it(_tmp720,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpD28;struct Cyc_List_List*_tmpD27;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpD27=_cycalloc(sizeof(*_tmpD27)),((_tmpD27->hd=((_tmpD28=_cycalloc(sizeof(*_tmpD28)),((_tmpD28->f1=0,((_tmpD28->f2=_tmp725,_tmpD28)))))),((_tmpD27->tl=0,_tmpD27)))))));}}
# 3583
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL215;};};}_LL24E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp55C=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp55C->tag != 27)goto _LL250;else{_tmp55D=_tmp55C->f1;_tmp55E=(void*)_tmp55C->f2;_tmp55F=_tmp55C->f3;}}_LL24F:
# 3588
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL215;_LL250: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp560->tag != 28)goto _LL252;else{_tmp561=_tmp560->f1;_tmp562=_tmp560->f2;_tmp563=_tmp560->f3;_tmp564=_tmp560->f4;}}_LL251:
# 3593
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp728=Cyc_Toc_init_struct(nv,old_typ,_tmp562,0,0,_tmp563,_tmp561);
e->r=_tmp728->r;
e->topt=_tmp728->topt;}else{
# 3604
if(_tmp564 == 0){
const char*_tmpD2B;void*_tmpD2A;(_tmpD2A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2B="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD2B,sizeof(char),38))),_tag_dyneither(_tmpD2A,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp564;
# 3608
struct _RegionHandle _tmp72B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp72B;_push_region(rgn);
{struct Cyc_List_List*_tmp72C=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp563,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3612
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp564->impl))->tagged){
# 3614
struct _tuple30*_tmp72D=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp72C))->hd;struct Cyc_Absyn_Aggrfield*_tmp72F;struct Cyc_Absyn_Exp*_tmp730;struct _tuple30*_tmp72E=_tmp72D;_tmp72F=_tmp72E->f1;_tmp730=_tmp72E->f2;{
void*_tmp731=(void*)_check_null(_tmp730->topt);
void*_tmp732=_tmp72F->type;
Cyc_Toc_exp_to_c(nv,_tmp730);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp732) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp731))
_tmp730->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp730->r,0));{
# 3623
int i=Cyc_Toc_get_member_offset(_tmp564,_tmp72F->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD30;struct _tuple19*_tmpD2F;struct _tuple19*_tmpD2E[2];struct Cyc_List_List*_tmp733=(_tmpD2E[1]=((_tmpD2F=_cycalloc(sizeof(*_tmpD2F)),((_tmpD2F->f1=0,((_tmpD2F->f2=_tmp730,_tmpD2F)))))),((_tmpD2E[0]=((_tmpD30=_cycalloc(sizeof(*_tmpD30)),((_tmpD30->f1=0,((_tmpD30->f2=field_tag_exp,_tmpD30)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD2E,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp733,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD36;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD35;void*_tmpD34[1];struct Cyc_List_List*ds=(_tmpD34[0]=(void*)((_tmpD36=_cycalloc(sizeof(*_tmpD36)),((_tmpD36[0]=((_tmpD35.tag=1,((_tmpD35.f1=_tmp72F->name,_tmpD35)))),_tmpD36)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD34,sizeof(void*),1)));
struct _tuple19*_tmpD39;struct _tuple19*_tmpD38[1];struct Cyc_List_List*dles=(_tmpD38[0]=((_tmpD39=_cycalloc(sizeof(*_tmpD39)),((_tmpD39->f1=ds,((_tmpD39->f2=umem,_tmpD39)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD38,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3632
struct Cyc_List_List*_tmp73C=0;
struct Cyc_List_List*_tmp73D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp73D != 0;_tmp73D=_tmp73D->tl){
struct Cyc_List_List*_tmp73E=_tmp72C;for(0;_tmp73E != 0;_tmp73E=_tmp73E->tl){
if((*((struct _tuple30*)_tmp73E->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp73D->hd){
struct _tuple30*_tmp73F=(struct _tuple30*)_tmp73E->hd;struct Cyc_Absyn_Aggrfield*_tmp741;struct Cyc_Absyn_Exp*_tmp742;struct _tuple30*_tmp740=_tmp73F;_tmp741=_tmp740->f1;_tmp742=_tmp740->f2;{
void*_tmp743=Cyc_Toc_typ_to_c(_tmp741->type);
void*_tmp744=Cyc_Toc_typ_to_c((void*)_check_null(_tmp742->topt));
Cyc_Toc_exp_to_c(nv,_tmp742);
# 3642
if(!Cyc_Tcutil_unify(_tmp743,_tmp744)){
# 3644
if(!Cyc_Tcutil_is_arithmetic_type(_tmp743) || !
Cyc_Tcutil_is_arithmetic_type(_tmp744))
_tmp742=Cyc_Toc_cast_it(_tmp743,Cyc_Absyn_copy_exp(_tmp742));}
{struct _tuple19*_tmpD3C;struct Cyc_List_List*_tmpD3B;_tmp73C=((_tmpD3B=_cycalloc(sizeof(*_tmpD3B)),((_tmpD3B->hd=((_tmpD3C=_cycalloc(sizeof(*_tmpD3C)),((_tmpD3C->f1=0,((_tmpD3C->f2=_tmp742,_tmpD3C)))))),((_tmpD3B->tl=_tmp73C,_tmpD3B))))));}
break;};}}}
# 3651
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp73C));}}
# 3609
;_pop_region(rgn);};}
# 3655
goto _LL215;_LL252: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp565=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp565->tag != 29)goto _LL254;else{_tmp566=(void*)_tmp565->f1;_tmp567=_tmp565->f2;}}_LL253: {
# 3657
struct Cyc_List_List*fs;
{void*_tmp747=Cyc_Tcutil_compress(_tmp566);void*_tmp748=_tmp747;struct Cyc_List_List*_tmp74A;_LL31A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp749=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp748;if(_tmp749->tag != 12)goto _LL31C;else{_tmp74A=_tmp749->f2;}}_LL31B:
 fs=_tmp74A;goto _LL319;_LL31C:;_LL31D: {
const char*_tmpD40;void*_tmpD3F[1];struct Cyc_String_pa_PrintArg_struct _tmpD3E;(_tmpD3E.tag=0,((_tmpD3E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp566)),((_tmpD3F[0]=& _tmpD3E,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD40="anon struct has type %s",_tag_dyneither(_tmpD40,sizeof(char),24))),_tag_dyneither(_tmpD3F,sizeof(void*),1)))))));}_LL319:;}{
# 3662
struct _RegionHandle _tmp74E=_new_region("rgn");struct _RegionHandle*rgn=& _tmp74E;_push_region(rgn);{
struct Cyc_List_List*_tmp74F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp567,Cyc_Absyn_StructA,fs);
for(0;_tmp74F != 0;_tmp74F=_tmp74F->tl){
struct _tuple30*_tmp750=(struct _tuple30*)_tmp74F->hd;struct Cyc_Absyn_Aggrfield*_tmp752;struct Cyc_Absyn_Exp*_tmp753;struct _tuple30*_tmp751=_tmp750;_tmp752=_tmp751->f1;_tmp753=_tmp751->f2;{
void*_tmp754=(void*)_check_null(_tmp753->topt);
void*_tmp755=_tmp752->type;
Cyc_Toc_exp_to_c(nv,_tmp753);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp755) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp754))
_tmp753->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp753->r,0));};}
# 3676
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp567);}
# 3678
_npop_handler(0);goto _LL215;
# 3662
;_pop_region(rgn);};}_LL254: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp568=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp568->tag != 30)goto _LL256;else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;_tmp56B=_tmp568->f3;}}_LL255: {
# 3681
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp756=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp757=Cyc_Absyn_var_exp(_tmp756,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp56B->name,_tmp56A->name));
tag_exp=_tmp56A->is_extensible?Cyc_Absyn_var_exp(_tmp56B->name,0):
 Cyc_Toc_datatype_tag(_tmp56A,_tmp56B->name);
mem_exp=_tmp757;{
struct Cyc_List_List*_tmp758=_tmp56B->typs;
# 3692
if(Cyc_Toc_is_toplevel(nv)){
# 3694
struct Cyc_List_List*dles=0;
for(0;_tmp569 != 0;(_tmp569=_tmp569->tl,_tmp758=_tmp758->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp569->hd;
void*_tmp759=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp758))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp759))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD43;struct Cyc_List_List*_tmpD42;dles=((_tmpD42=_cycalloc(sizeof(*_tmpD42)),((_tmpD42->hd=((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43->f1=0,((_tmpD43->f2=cur_e,_tmpD43)))))),((_tmpD42->tl=dles,_tmpD42))))));};}
# 3705
{struct _tuple19*_tmpD46;struct Cyc_List_List*_tmpD45;dles=((_tmpD45=_cycalloc(sizeof(*_tmpD45)),((_tmpD45->hd=((_tmpD46=_cycalloc(sizeof(*_tmpD46)),((_tmpD46->f1=0,((_tmpD46->f2=tag_exp,_tmpD46)))))),((_tmpD45->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD45))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3711
struct Cyc_List_List*_tmpD47;struct Cyc_List_List*_tmp75E=
(_tmpD47=_cycalloc(sizeof(*_tmpD47)),((_tmpD47->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD47->tl=0,_tmpD47)))));
# 3714
{int i=1;for(0;_tmp569 != 0;(((_tmp569=_tmp569->tl,i ++)),_tmp758=_tmp758->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp569->hd;
void*_tmp75F=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp758))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp75F))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp760=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3724
struct Cyc_List_List*_tmpD48;_tmp75E=((_tmpD48=_cycalloc(sizeof(*_tmpD48)),((_tmpD48->hd=_tmp760,((_tmpD48->tl=_tmp75E,_tmpD48))))));};}}{
# 3726
struct Cyc_Absyn_Stmt*_tmp762=Cyc_Absyn_exp_stmt(_tmp757,0);
struct Cyc_List_List*_tmpD49;struct Cyc_Absyn_Stmt*_tmp763=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD49=_cycalloc(sizeof(*_tmpD49)),((_tmpD49->hd=_tmp762,((_tmpD49->tl=_tmp75E,_tmpD49))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp756,datatype_ctype,0,_tmp763,0));};}
# 3730
goto _LL215;};}_LL256: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp56C=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp56C->tag != 31)goto _LL258;}_LL257:
# 3732
 goto _LL259;_LL258: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp56D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp56D->tag != 32)goto _LL25A;}_LL259:
 goto _LL215;_LL25A: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp56E=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp56E->tag != 33)goto _LL25C;else{_tmp56F=(_tmp56E->f1).is_calloc;_tmp570=(_tmp56E->f1).rgn;_tmp571=(_tmp56E->f1).elt_type;_tmp572=(_tmp56E->f1).num_elts;_tmp573=(_tmp56E->f1).fat_result;}}_LL25B: {
# 3736
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp571)));
Cyc_Toc_exp_to_c(nv,_tmp572);
# 3740
if(_tmp573){
struct _tuple1*_tmp766=Cyc_Toc_temp_var();
struct _tuple1*_tmp767=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp56F){
xexp=_tmp572;
if(_tmp570 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp570;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp766,0));}else{
# 3751
pexp=Cyc_Toc_calloc_exp(*_tmp571,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp766,0));}{
# 3753
struct Cyc_Absyn_Exp*_tmpD4A[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD4A[2]=
Cyc_Absyn_var_exp(_tmp766,0),((_tmpD4A[1]=
# 3754
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD4A[0]=Cyc_Absyn_var_exp(_tmp767,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD4A,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3757
if(_tmp570 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp570;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp766,0));}else{
# 3762
pexp=Cyc_Toc_malloc_exp(*_tmp571,Cyc_Absyn_var_exp(_tmp766,0));}{
# 3764
struct Cyc_Absyn_Exp*_tmpD4B[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD4B[2]=
Cyc_Absyn_var_exp(_tmp766,0),((_tmpD4B[1]=
# 3764
Cyc_Absyn_uint_exp(1,0),((_tmpD4B[0]=Cyc_Absyn_var_exp(_tmp767,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD4B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3767
struct Cyc_Absyn_Stmt*_tmp76A=Cyc_Absyn_declare_stmt(_tmp766,Cyc_Absyn_uint_typ,_tmp572,
Cyc_Absyn_declare_stmt(_tmp767,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp76A);};}else{
# 3772
if(_tmp56F){
if(_tmp570 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp570;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp572))->r;}else{
# 3778
e->r=(Cyc_Toc_calloc_exp(*_tmp571,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp572))->r;}}else{
# 3781
if(_tmp570 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp570;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp572))->r;}else{
# 3786
e->r=(Cyc_Toc_malloc_exp(*_tmp571,_tmp572))->r;}}}
# 3790
goto _LL215;}_LL25C: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp574=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp574->tag != 34)goto _LL25E;else{_tmp575=_tmp574->f1;_tmp576=_tmp574->f2;}}_LL25D: {
# 3799
void*e1_old_typ=(void*)_check_null(_tmp575->topt);
void*e2_old_typ=(void*)_check_null(_tmp576->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD4E;void*_tmpD4D;(_tmpD4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4E="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD4E,sizeof(char),57))),_tag_dyneither(_tmpD4D,sizeof(void*),0)));}{
# 3805
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3809
swap_fn=Cyc_Toc__swap_word_e;}{
# 3813
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp575,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp576,& f2_tag,& tagged_mem_type2,1);
# 3820
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp575);
Cyc_Toc_exp_to_c(nv,_tmp576);
Cyc_Toc_set_lhs(nv,0);
# 3826
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp575,f1_tag,tagged_mem_type1);else{
# 3830
_tmp575=Cyc_Toc_push_address_exp(_tmp575);}
# 3832
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp576,f2_tag,tagged_mem_type2);else{
# 3836
_tmp576=Cyc_Toc_push_address_exp(_tmp576);}
# 3838
{struct Cyc_Absyn_Exp*_tmpD4F[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD4F[1]=_tmp576,((_tmpD4F[0]=_tmp575,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD4F,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3840
goto _LL215;};};}_LL25E: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp577=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp577->tag != 37)goto _LL260;else{_tmp578=_tmp577->f1;_tmp579=_tmp577->f2;}}_LL25F: {
# 3843
void*_tmp76E=Cyc_Tcutil_compress((void*)_check_null(_tmp578->topt));
Cyc_Toc_exp_to_c(nv,_tmp578);
{void*_tmp76F=_tmp76E;struct Cyc_Absyn_Aggrdecl*_tmp771;_LL31F: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp770=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp76F;if(_tmp770->tag != 11)goto _LL321;else{if((((_tmp770->f1).aggr_info).KnownAggr).tag != 2)goto _LL321;_tmp771=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp770->f1).aggr_info).KnownAggr).val);}}_LL320: {
# 3847
struct Cyc_Absyn_Exp*_tmp772=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp771,_tmp579),0);
struct Cyc_Absyn_Exp*_tmp773=Cyc_Toc_member_exp(_tmp578,_tmp579,0);
struct Cyc_Absyn_Exp*_tmp774=Cyc_Toc_member_exp(_tmp773,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp774,_tmp772,0))->r;
goto _LL31E;}_LL321:;_LL322: {
const char*_tmpD52;void*_tmpD51;(_tmpD51=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD52="non-aggregate type in tagcheck",_tag_dyneither(_tmpD52,sizeof(char),31))),_tag_dyneither(_tmpD51,sizeof(void*),0)));}_LL31E:;}
# 3854
goto _LL215;}_LL260: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp57A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp57A->tag != 36)goto _LL262;else{_tmp57B=_tmp57A->f1;}}_LL261:
 Cyc_Toc_stmt_to_c(nv,_tmp57B);goto _LL215;_LL262: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp57C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp57C->tag != 35)goto _LL264;}_LL263: {
const char*_tmpD55;void*_tmpD54;(_tmpD54=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD55="UnresolvedMem",_tag_dyneither(_tmpD55,sizeof(char),14))),_tag_dyneither(_tmpD54,sizeof(void*),0)));}_LL264: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp57D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp57D->tag != 24)goto _LL266;}_LL265: {
const char*_tmpD58;void*_tmpD57;(_tmpD57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD58="compoundlit",_tag_dyneither(_tmpD58,sizeof(char),12))),_tag_dyneither(_tmpD57,sizeof(void*),0)));}_LL266: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp57E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp57E->tag != 38)goto _LL268;}_LL267: {
const char*_tmpD5B;void*_tmpD5A;(_tmpD5A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5B="valueof(-)",_tag_dyneither(_tmpD5B,sizeof(char),11))),_tag_dyneither(_tmpD5A,sizeof(void*),0)));}_LL268: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp57F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp506;if(_tmp57F->tag != 39)goto _LL215;}_LL269: {
const char*_tmpD5E;void*_tmpD5D;(_tmpD5D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5E="__asm__",_tag_dyneither(_tmpD5E,sizeof(char),8))),_tag_dyneither(_tmpD5D,sizeof(void*),0)));}_LL215:;};}
# 3891 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3894
Cyc_Toc_skip_stmt_dl(),0);}
# 3897
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp77F=e->r;void*_tmp780=_tmp77F;struct Cyc_Absyn_Exp*_tmp782;struct Cyc_Absyn_Exp*_tmp784;struct Cyc_Absyn_Exp*_tmp786;_LL324: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp781=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp780;if(_tmp781->tag != 13)goto _LL326;else{_tmp782=_tmp781->f2;}}_LL325:
 return Cyc_Toc_path_length(_tmp782);_LL326: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp783=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp780;if(_tmp783->tag != 19)goto _LL328;else{_tmp784=_tmp783->f1;}}_LL327:
 return 1 + Cyc_Toc_path_length(_tmp784);_LL328: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp785=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp780;if(_tmp785->tag != 20)goto _LL32A;else{_tmp786=_tmp785->f1;}}_LL329:
 return Cyc_Toc_path_length(_tmp786);_LL32A:;_LL32B:
 return 0;_LL323:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3906
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3911
struct Cyc_Absyn_Stmt*s;
{void*_tmp787=p->r;void*_tmp788=_tmp787;struct _tuple1*_tmp78A;struct _tuple1*_tmp78C;struct Cyc_Absyn_Pat*_tmp78D;struct _tuple1*_tmp78F;struct Cyc_Absyn_Vardecl*_tmp792;struct Cyc_Absyn_Pat*_tmp793;enum Cyc_Absyn_Sign _tmp796;int _tmp797;char _tmp799;struct _dyneither_ptr _tmp79B;int _tmp79C;struct Cyc_Absyn_Enumdecl*_tmp79E;struct Cyc_Absyn_Enumfield*_tmp79F;void*_tmp7A1;struct Cyc_Absyn_Enumfield*_tmp7A2;struct Cyc_Absyn_Datatypedecl*_tmp7A5;struct Cyc_Absyn_Datatypefield*_tmp7A6;struct Cyc_List_List*_tmp7A7;struct Cyc_List_List*_tmp7A9;struct Cyc_List_List*_tmp7AB;union Cyc_Absyn_AggrInfoU _tmp7AE;struct Cyc_List_List*_tmp7AF;struct Cyc_Absyn_Pat*_tmp7B1;_LL32D: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp789=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp789->tag != 2)goto _LL32F;else{_tmp78A=(_tmp789->f2)->name;}}_LL32E: {
# 3914
struct Cyc_Absyn_Pat*_tmp7B5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7B5->topt=p->topt;
_tmp78C=_tmp78A;_tmp78D=_tmp7B5;goto _LL330;}_LL32F: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp78B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp78B->tag != 1)goto _LL331;else{_tmp78C=(_tmp78B->f1)->name;_tmp78D=_tmp78B->f2;}}_LL330: {
# 3919
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7B6=(void*)_check_null(_tmp78D->topt);
{struct _tuple32*_tmpD61;struct Cyc_List_List*_tmpD60;decls=((_tmpD60=_region_malloc(rgn,sizeof(*_tmpD60)),((_tmpD60->hd=((_tmpD61=_region_malloc(rgn,sizeof(*_tmpD61)),((_tmpD61->f1=v,((_tmpD61->f2=Cyc_Toc_typ_to_c_array(_tmp7B6),_tmpD61)))))),((_tmpD60->tl=decls,_tmpD60))))));}{
struct _tuple31 _tmp7B9=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp78C,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7B6,
Cyc_Absyn_var_exp(v,0),path,_tmp78D,fail_stmt,decls);
nv=_tmp7B9.f1;
decls=_tmp7B9.f2;{
struct Cyc_Absyn_Stmt*_tmp7BA=_tmp7B9.f3;
struct Cyc_Absyn_Stmt*_tmp7BB=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7BB,_tmp7BA,0);
goto _LL32C;};};}_LL331: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp78E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp78E->tag != 4)goto _LL333;else{_tmp78F=(_tmp78E->f2)->name;}}_LL332: {
# 3932
struct _tuple1*_tmp7BC=Cyc_Toc_temp_var();
void*_tmp7BD=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD64;struct Cyc_List_List*_tmpD63;decls=((_tmpD63=_region_malloc(rgn,sizeof(*_tmpD63)),((_tmpD63->hd=((_tmpD64=_region_malloc(rgn,sizeof(*_tmpD64)),((_tmpD64->f1=_tmp7BC,((_tmpD64->f2=Cyc_Toc_typ_to_c_array(_tmp7BD),_tmpD64)))))),((_tmpD63->tl=decls,_tmpD63))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp78F,Cyc_Absyn_var_exp(_tmp7BC,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7BC,0),r,0);
goto _LL32C;}_LL333: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp790=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp790->tag != 0)goto _LL335;}_LL334:
 s=Cyc_Toc_skip_stmt_dl();goto _LL32C;_LL335: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp791=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp791->tag != 3)goto _LL337;else{_tmp792=_tmp791->f1;_tmp793=_tmp791->f2;}}_LL336: {
# 3941
struct _tuple1*_tmp7C0=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD67;struct Cyc_List_List*_tmpD66;decls=((_tmpD66=_region_malloc(rgn,sizeof(*_tmpD66)),((_tmpD66->hd=((_tmpD67=_region_malloc(rgn,sizeof(*_tmpD67)),((_tmpD67->f1=_tmp7C0,((_tmpD67->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD67)))))),((_tmpD66->tl=decls,_tmpD66))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp792->name,Cyc_Absyn_var_exp(_tmp7C0,0));
# 3945
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7C0,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7C3=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp793,fail_stmt,decls);
_tmp7C3.f3=Cyc_Absyn_seq_stmt(s,_tmp7C3.f3,0);
return _tmp7C3;};}_LL337: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp794=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp794->tag != 9)goto _LL339;}_LL338:
# 3952
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL32C;_LL339: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp795=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp795->tag != 10)goto _LL33B;else{_tmp796=_tmp795->f1;_tmp797=_tmp795->f2;}}_LL33A:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp796,_tmp797,0),fail_stmt);goto _LL32C;_LL33B: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp798=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp798->tag != 11)goto _LL33D;else{_tmp799=_tmp798->f1;}}_LL33C:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp799,0),fail_stmt);goto _LL32C;_LL33D: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp79A=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp79A->tag != 12)goto _LL33F;else{_tmp79B=_tmp79A->f1;_tmp79C=_tmp79A->f2;}}_LL33E:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp79B,_tmp79C,0),fail_stmt);goto _LL32C;_LL33F: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp79D=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp79D->tag != 13)goto _LL341;else{_tmp79E=_tmp79D->f1;_tmp79F=_tmp79D->f2;}}_LL340:
# 3957
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD6A;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD69;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD69=_cycalloc(sizeof(*_tmpD69)),((_tmpD69[0]=((_tmpD6A.tag=31,((_tmpD6A.f1=_tmp79F->name,((_tmpD6A.f2=_tmp79E,((_tmpD6A.f3=_tmp79F,_tmpD6A)))))))),_tmpD69)))),0),fail_stmt);}
goto _LL32C;_LL341: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp7A0=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7A0->tag != 14)goto _LL343;else{_tmp7A1=(void*)_tmp7A0->f1;_tmp7A2=_tmp7A0->f2;}}_LL342:
# 3960
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD6D;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD6C;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD6C=_cycalloc(sizeof(*_tmpD6C)),((_tmpD6C[0]=((_tmpD6D.tag=32,((_tmpD6D.f1=_tmp7A2->name,((_tmpD6D.f2=_tmp7A1,((_tmpD6D.f3=_tmp7A2,_tmpD6D)))))))),_tmpD6C)))),0),fail_stmt);}
goto _LL32C;_LL343: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7A3=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7A3->tag != 6)goto _LL345;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7A4=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp7A3->f1)->r;if(_tmp7A4->tag != 8)goto _LL345;else{_tmp7A5=_tmp7A4->f1;_tmp7A6=_tmp7A4->f2;_tmp7A7=_tmp7A4->f3;}}}_LL344:
# 3970
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7C8=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp7A6->name,_tmp7A5->name);
void*_tmp7C9=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7C9,r);
struct Cyc_List_List*_tmp7CA=_tmp7A6->typs;
for(0;_tmp7A7 != 0;(((_tmp7A7=_tmp7A7->tl,_tmp7CA=((struct Cyc_List_List*)_check_null(_tmp7CA))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7CB=(struct Cyc_Absyn_Pat*)_tmp7A7->hd;
if(_tmp7CB->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7CC=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7CA))->hd)).f2;
void*_tmp7CD=(void*)_check_null(_tmp7CB->topt);
void*_tmp7CE=Cyc_Toc_typ_to_c_array(_tmp7CD);
struct Cyc_Absyn_Exp*_tmp7CF=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7C8,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7CC)))
_tmp7CF=Cyc_Toc_cast_it(_tmp7CE,_tmp7CF);{
# 3988
struct _tuple31 _tmp7D0=Cyc_Toc_xlate_pat(nv,rgn,_tmp7CD,_tmp7CF,_tmp7CF,_tmp7CB,fail_stmt,decls);
# 3990
nv=_tmp7D0.f1;
decls=_tmp7D0.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7D0.f3,0);};};}{
# 3994
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7C8,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp7A5->is_extensible?Cyc_Absyn_var_exp(_tmp7A6->name,0): Cyc_Toc_datatype_tag(_tmp7A5,_tmp7A6->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 4002
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7C8,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 4005
s=Cyc_Absyn_declare_stmt(_tmp7C8,_tmp7C9,rcast,s,0);
goto _LL32C;};};_LL345: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp7A8=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7A8->tag != 8)goto _LL347;else{_tmp7A9=_tmp7A8->f3;}}_LL346:
# 4008
 _tmp7AB=_tmp7A9;goto _LL348;_LL347: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp7AA=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7AA->tag != 5)goto _LL349;else{_tmp7AB=_tmp7AA->f1;}}_LL348:
# 4016
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp7AB != 0;(_tmp7AB=_tmp7AB->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7D1=(struct Cyc_Absyn_Pat*)_tmp7AB->hd;
if(_tmp7D1->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7D2=(void*)_check_null(_tmp7D1->topt);
struct _dyneither_ptr*_tmp7D3=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7D4=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D2,Cyc_Toc_member_exp(r,_tmp7D3,0),
Cyc_Toc_member_exp(path,_tmp7D3,0),_tmp7D1,fail_stmt,decls);
nv=_tmp7D4.f1;
decls=_tmp7D4.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7D4.f3,0);};}
# 4030
goto _LL32C;};_LL349: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7AC=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7AC->tag != 7)goto _LL34B;else{if(_tmp7AC->f1 != 0)goto _LL34B;}}_LL34A: {
# 4032
const char*_tmpD70;void*_tmpD6F;(_tmpD6F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD70="unresolved aggregate pattern!",_tag_dyneither(_tmpD70,sizeof(char),30))),_tag_dyneither(_tmpD6F,sizeof(void*),0)));}_LL34B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp7AD=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7AD->tag != 7)goto _LL34D;else{if(_tmp7AD->f1 == 0)goto _LL34D;_tmp7AE=(_tmp7AD->f1)->aggr_info;_tmp7AF=_tmp7AD->f3;}}_LL34C: {
# 4034
struct Cyc_Absyn_Aggrdecl*_tmp7D7=Cyc_Absyn_get_known_aggrdecl(_tmp7AE);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D7->impl))->tagged){
# 4038
struct _tuple33*_tmp7D8=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7AF))->hd;struct Cyc_List_List*_tmp7DA;struct Cyc_Absyn_Pat*_tmp7DB;struct _tuple33*_tmp7D9=_tmp7D8;_tmp7DA=_tmp7D9->f1;_tmp7DB=_tmp7D9->f2;{
struct _dyneither_ptr*f;
{void*_tmp7DC=(void*)((struct Cyc_List_List*)_check_null(_tmp7DA))->hd;void*_tmp7DD=_tmp7DC;struct _dyneither_ptr*_tmp7DF;_LL356: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7DE=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7DD;if(_tmp7DE->tag != 1)goto _LL358;else{_tmp7DF=_tmp7DE->f1;}}_LL357:
 f=_tmp7DF;goto _LL355;_LL358:;_LL359: {
const char*_tmpD73;void*_tmpD72;(_tmpD72=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD73="no field name in tagged union pattern",_tag_dyneither(_tmpD73,sizeof(char),38))),_tag_dyneither(_tmpD72,sizeof(void*),0)));}_LL355:;}{
# 4045
void*_tmp7E2=(void*)_check_null(_tmp7DB->topt);
void*_tmp7E3=Cyc_Toc_typ_to_c_array(_tmp7E2);
# 4048
struct Cyc_Absyn_Exp*_tmp7E4=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7E5=Cyc_Toc_cast_it(_tmp7E3,_tmp7E5);
_tmp7E4=Cyc_Toc_cast_it(_tmp7E3,_tmp7E4);{
struct _tuple31 _tmp7E6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E2,_tmp7E5,_tmp7E4,_tmp7DB,fail_stmt,decls);
# 4056
nv=_tmp7E6.f1;
decls=_tmp7E6.f2;{
struct Cyc_Absyn_Stmt*_tmp7E7=_tmp7E6.f3;
struct Cyc_Absyn_Stmt*_tmp7E8=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7D7,f),0),fail_stmt);
# 4064
s=Cyc_Absyn_seq_stmt(_tmp7E8,_tmp7E7,0);};};};};}else{
# 4067
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7AF != 0;_tmp7AF=_tmp7AF->tl){
struct _tuple33*_tmp7E9=(struct _tuple33*)_tmp7AF->hd;
struct Cyc_Absyn_Pat*_tmp7EA=(*_tmp7E9).f2;
if(_tmp7EA->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7EB=(void*)((struct Cyc_List_List*)_check_null((*_tmp7E9).f1))->hd;void*_tmp7EC=_tmp7EB;struct _dyneither_ptr*_tmp7EE;_LL35B: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7ED=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7EC;if(_tmp7ED->tag != 1)goto _LL35D;else{_tmp7EE=_tmp7ED->f1;}}_LL35C:
 f=_tmp7EE;goto _LL35A;_LL35D:;_LL35E: {
struct Cyc_Toc_Match_error_exn_struct _tmpD76;struct Cyc_Toc_Match_error_exn_struct*_tmpD75;(int)_throw((void*)((_tmpD75=_cycalloc_atomic(sizeof(*_tmpD75)),((_tmpD75[0]=((_tmpD76.tag=Cyc_Toc_Match_error,_tmpD76)),_tmpD75)))));}_LL35A:;}{
# 4078
void*_tmp7F1=(void*)_check_null(_tmp7EA->topt);
void*_tmp7F2=Cyc_Toc_typ_to_c_array(_tmp7F1);
struct Cyc_Absyn_Exp*_tmp7F3=Cyc_Toc_member_exp(r,f,0);
void*_tmp7F4=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7D7->impl))->fields,f)))->type;
struct Cyc_Absyn_Exp*_tmp7F5=Cyc_Toc_member_exp(path,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp7F4))
_tmp7F3=Cyc_Toc_cast_it(_tmp7F2,_tmp7F3);else{
if(!Cyc_Toc_is_array_type(_tmp7F4) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp7F4))){
# 4088
_tmp7F3=Cyc_Absyn_deref_exp(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp7F2,Cyc_Toc_mt_tq),
Cyc_Absyn_address_exp(_tmp7F3,0)),0);
# 4091
_tmp7F5=Cyc_Toc_push_address_exp(_tmp7F5);
_tmp7F5=Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(_tmp7F2,Cyc_Toc_mt_tq),_tmp7F5);
_tmp7F5=Cyc_Absyn_deref_exp(_tmp7F5,0);}}{
# 4095
struct _tuple31 _tmp7F6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F1,_tmp7F3,_tmp7F5,_tmp7EA,fail_stmt,decls);
nv=_tmp7F6.f1;
decls=_tmp7F6.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7F6.f3,0);};};};}}
# 4101
goto _LL32C;}_LL34D: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7B0=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7B0->tag != 6)goto _LL34F;else{_tmp7B1=_tmp7B0->f1;}}_LL34E: {
# 4104
void*_tmp7F7=(void*)_check_null(_tmp7B1->topt);
# 4107
struct _tuple31 _tmp7F8=Cyc_Toc_xlate_pat(nv,rgn,_tmp7F7,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7B1,fail_stmt,decls);
# 4109
nv=_tmp7F8.f1;
decls=_tmp7F8.f2;{
struct Cyc_Absyn_Stmt*_tmp7F9=_tmp7F8.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4115
Cyc_Toc_skip_stmt_dl(),0),_tmp7F9,0);else{
# 4117
s=_tmp7F9;}
goto _LL32C;};}_LL34F: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7B2=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7B2->tag != 15)goto _LL351;}_LL350: {
const char*_tmpD79;void*_tmpD78;(_tmpD78=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD79="unknownid pat",_tag_dyneither(_tmpD79,sizeof(char),14))),_tag_dyneither(_tmpD78,sizeof(void*),0)));}_LL351: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7B3=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7B3->tag != 16)goto _LL353;}_LL352: {
const char*_tmpD7C;void*_tmpD7B;(_tmpD7B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7C="unknowncall pat",_tag_dyneither(_tmpD7C,sizeof(char),16))),_tag_dyneither(_tmpD7B,sizeof(void*),0)));}_LL353: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7B4=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp788;if(_tmp7B4->tag != 17)goto _LL32C;}_LL354: {
const char*_tmpD7F;void*_tmpD7E;(_tmpD7E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD7F="exp pat",_tag_dyneither(_tmpD7F,sizeof(char),8))),_tag_dyneither(_tmpD7E,sizeof(void*),0)));}_LL32C:;}{
# 4123
struct _tuple31 _tmpD80;return(_tmpD80.f1=nv,((_tmpD80.f2=decls,((_tmpD80.f3=s,_tmpD80)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4160 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4162
struct _tuple34*_tmpD81;return(_tmpD81=_region_malloc(r,sizeof(*_tmpD81)),((_tmpD81->f1=Cyc_Toc_fresh_label(),((_tmpD81->f2=Cyc_Toc_fresh_label(),((_tmpD81->f3=sc,_tmpD81)))))));}
# 4165
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4167
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp802=(void*)_check_null(e->topt);
# 4172
int leave_as_switch;
{void*_tmp803=Cyc_Tcutil_compress(_tmp802);void*_tmp804=_tmp803;_LL360: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp805=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp804;if(_tmp805->tag != 6)goto _LL362;}_LL361:
 goto _LL363;_LL362: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp806=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp804;if(_tmp806->tag != 13)goto _LL364;}_LL363:
 leave_as_switch=1;goto _LL35F;_LL364:;_LL365:
 leave_as_switch=0;goto _LL35F;_LL35F:;}
# 4178
{struct Cyc_List_List*_tmp807=scs;for(0;_tmp807 != 0;_tmp807=_tmp807->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp807->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp807->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4183
if(leave_as_switch){
# 4185
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp808=scs;for(0;_tmp808 != 0;_tmp808=_tmp808->tl){
struct Cyc_Absyn_Stmt*_tmp809=((struct Cyc_Absyn_Switch_clause*)_tmp808->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp808->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp809,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp80A=_new_region("rgn");struct _RegionHandle*rgn=& _tmp80A;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp809);;_pop_region(rgn);};}}
# 4194
return;}{
# 4197
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4202
struct _RegionHandle _tmp80B=_new_region("rgn");struct _RegionHandle*rgn=& _tmp80B;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp80C=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4213
{struct Cyc_List_List*_tmp80D=lscs;for(0;_tmp80D != 0;_tmp80D=_tmp80D->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp80D->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp80D->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp80D->tl))->hd)).f1;
struct _tuple31 _tmp80E=Cyc_Toc_xlate_pat(_tmp80C,rgn,_tmp802,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4216
struct Cyc_Toc_Env*_tmp810;struct Cyc_List_List*_tmp811;struct Cyc_Absyn_Stmt*_tmp812;struct _tuple31 _tmp80F=_tmp80E;_tmp810=_tmp80F.f1;_tmp811=_tmp80F.f2;_tmp812=_tmp80F.f3;
# 4219
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp813=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp810,_tmp813);
_tmp812=Cyc_Absyn_seq_stmt(_tmp812,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp813,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4226
decls=_tmp811;
{struct Cyc_List_List*_tmpD82;nvs=((_tmpD82=_region_malloc(rgn,sizeof(*_tmpD82)),((_tmpD82->hd=_tmp810,((_tmpD82->tl=nvs,_tmpD82))))));}{
struct Cyc_List_List*_tmpD83;test_stmts=((_tmpD83=_region_malloc(rgn,sizeof(*_tmpD83)),((_tmpD83->hd=_tmp812,((_tmpD83->tl=test_stmts,_tmpD83))))));};}}
# 4230
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4235
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp816=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp818;struct _dyneither_ptr*_tmp819;struct Cyc_Absyn_Switch_clause*_tmp81A;struct _tuple34*_tmp817=_tmp816;_tmp818=_tmp817->f1;_tmp819=_tmp817->f2;_tmp81A=_tmp817->f3;{
struct Cyc_Toc_Env*_tmp81B=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp81A->body;
struct _RegionHandle _tmp81C=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp81C;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp81D=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp81F;struct Cyc_Absyn_Switch_clause*_tmp820;struct _tuple34*_tmp81E=_tmp81D;_tmp81F=_tmp81E->f2;_tmp820=_tmp81E->f3;{
struct Cyc_List_List*_tmp821=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp820->pat_vars))->v)).f1);
_tmp821=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp821);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp81B,end_l,_tmp81F,_tmp821,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4250
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp81B,end_l),s);}
# 4252
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp818,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp819,s,0),0);{
struct Cyc_List_List*_tmpD84;stmts=((_tmpD84=_region_malloc(rgn,sizeof(*_tmpD84)),((_tmpD84->hd=s,((_tmpD84->tl=stmts,_tmpD84))))));};
# 4240
;_pop_region(rgn2);};}{
# 4256
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4259
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp823=(struct _tuple32*)decls->hd;struct _tuple1*_tmp825;void*_tmp826;struct _tuple32*_tmp824=_tmp823;_tmp825=_tmp824->f1;_tmp826=_tmp824->f2;
res=Cyc_Absyn_declare_stmt(_tmp825,_tmp826,0,res,0);}
# 4264
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4203
;_pop_region(rgn);};};}
# 4269
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4275
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4277
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD85;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD85=_cycalloc(sizeof(*_tmpD85)),((_tmpD85->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD85->tl=0,_tmpD85)))))),0),0);}
# 4281
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4286
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4288
while(1){
void*_tmp828=s->r;void*_tmp829=_tmp828;struct Cyc_Absyn_Exp*_tmp82C;struct Cyc_Absyn_Stmt*_tmp82E;struct Cyc_Absyn_Stmt*_tmp82F;struct Cyc_Absyn_Exp*_tmp831;struct Cyc_Absyn_Exp*_tmp833;struct Cyc_Absyn_Stmt*_tmp834;struct Cyc_Absyn_Stmt*_tmp835;struct Cyc_Absyn_Exp*_tmp837;struct Cyc_Absyn_Stmt*_tmp838;struct Cyc_Absyn_Stmt*_tmp83A;struct Cyc_Absyn_Stmt*_tmp83C;struct Cyc_Absyn_Stmt*_tmp83E;struct Cyc_Absyn_Exp*_tmp840;struct Cyc_Absyn_Exp*_tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Stmt*_tmp843;struct Cyc_Absyn_Exp*_tmp845;struct Cyc_List_List*_tmp846;struct Cyc_List_List*_tmp848;struct Cyc_Absyn_Switch_clause**_tmp849;struct Cyc_Absyn_Decl*_tmp84B;struct Cyc_Absyn_Stmt*_tmp84C;struct _dyneither_ptr*_tmp84E;struct Cyc_Absyn_Stmt*_tmp84F;struct Cyc_Absyn_Stmt*_tmp851;struct Cyc_Absyn_Exp*_tmp852;struct Cyc_Absyn_Stmt*_tmp854;struct Cyc_List_List*_tmp855;struct Cyc_Absyn_Exp*_tmp857;_LL367: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp82A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp82A->tag != 0)goto _LL369;}_LL368:
# 4291
 return;_LL369: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp82B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp82B->tag != 1)goto _LL36B;else{_tmp82C=_tmp82B->f1;}}_LL36A:
# 4293
 Cyc_Toc_exp_to_c(nv,_tmp82C);
return;_LL36B: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp82D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp82D->tag != 2)goto _LL36D;else{_tmp82E=_tmp82D->f1;_tmp82F=_tmp82D->f2;}}_LL36C:
# 4296
 Cyc_Toc_stmt_to_c(nv,_tmp82E);
s=_tmp82F;
continue;_LL36D: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp830=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp830->tag != 3)goto _LL36F;else{_tmp831=_tmp830->f1;}}_LL36E: {
# 4300
void*topt=0;
if(_tmp831 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp831->topt));
Cyc_Toc_exp_to_c(nv,_tmp831);}
# 4306
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp858=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp859=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp858,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp858,topt,_tmp831,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp859,0),0))->r;}else{
# 4314
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4316
return;}_LL36F: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp832=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp832->tag != 4)goto _LL371;else{_tmp833=_tmp832->f1;_tmp834=_tmp832->f2;_tmp835=_tmp832->f3;}}_LL370:
# 4318
 Cyc_Toc_exp_to_c(nv,_tmp833);
Cyc_Toc_stmt_to_c(nv,_tmp834);
s=_tmp835;
continue;_LL371: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp836=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp836->tag != 5)goto _LL373;else{_tmp837=(_tmp836->f1).f1;_tmp838=_tmp836->f2;}}_LL372:
# 4323
 Cyc_Toc_exp_to_c(nv,_tmp837);{
struct _RegionHandle _tmp85A=_new_region("temp");struct _RegionHandle*temp=& _tmp85A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp838);
# 4327
_npop_handler(0);return;
# 4324
;_pop_region(temp);};_LL373: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp839=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp839->tag != 6)goto _LL375;else{_tmp83A=_tmp839->f1;}}_LL374: {
# 4329
struct _dyneither_ptr**_tmp85C;struct Cyc_Toc_Env*_tmp85B=nv;_tmp85C=_tmp85B->break_lab;
if(_tmp85C != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp85C,0);{
# 4333
int dest_depth=_tmp83A == 0?0: _tmp83A->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL375: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp83B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp83B->tag != 7)goto _LL377;else{_tmp83C=_tmp83B->f1;}}_LL376: {
# 4337
struct _dyneither_ptr**_tmp85E;struct Cyc_Toc_Env*_tmp85D=nv;_tmp85E=_tmp85D->continue_lab;
if(_tmp85E != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp85E,0);
_tmp83E=_tmp83C;goto _LL378;}_LL377: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp83D->tag != 8)goto _LL379;else{_tmp83E=_tmp83D->f2;}}_LL378:
# 4343
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp83E))->try_depth,s);
return;_LL379: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp83F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp83F->tag != 9)goto _LL37B;else{_tmp840=_tmp83F->f1;_tmp841=(_tmp83F->f2).f1;_tmp842=(_tmp83F->f3).f1;_tmp843=_tmp83F->f4;}}_LL37A:
# 4347
 Cyc_Toc_exp_to_c(nv,_tmp840);Cyc_Toc_exp_to_c(nv,_tmp841);Cyc_Toc_exp_to_c(nv,_tmp842);{
struct _RegionHandle _tmp85F=_new_region("temp");struct _RegionHandle*temp=& _tmp85F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp843);
# 4351
_npop_handler(0);return;
# 4348
;_pop_region(temp);};_LL37B: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp844=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp844->tag != 10)goto _LL37D;else{_tmp845=_tmp844->f1;_tmp846=_tmp844->f2;}}_LL37C:
# 4353
 Cyc_Toc_xlate_switch(nv,s,_tmp845,_tmp846);
return;_LL37D: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp847=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp847->tag != 11)goto _LL37F;else{_tmp848=_tmp847->f1;_tmp849=_tmp847->f2;}}_LL37E: {
# 4356
struct Cyc_Toc_FallthruInfo*_tmp861;struct Cyc_Toc_Env*_tmp860=nv;_tmp861=_tmp860->fallthru_info;
if(_tmp861 == 0){
const char*_tmpD88;void*_tmpD87;(_tmpD87=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD88="fallthru in unexpected place",_tag_dyneither(_tmpD88,sizeof(char),29))),_tag_dyneither(_tmpD87,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp864=*_tmp861;struct _dyneither_ptr*_tmp866;struct Cyc_List_List*_tmp867;struct Cyc_Dict_Dict _tmp868;struct Cyc_Toc_FallthruInfo _tmp865=_tmp864;_tmp866=_tmp865.label;_tmp867=_tmp865.binders;_tmp868=_tmp865.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp866,0);
# 4362
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp849)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp869=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp867);
struct Cyc_List_List*_tmp86A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp848);
for(0;_tmp869 != 0;(_tmp869=_tmp869->tl,_tmp86A=_tmp86A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp86A))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp868,(struct _tuple1*)_tmp869->hd),(struct Cyc_Absyn_Exp*)_tmp86A->hd,0),s2,0);}
# 4370
s->r=s2->r;
return;};};};}_LL37F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp84A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp84A->tag != 12)goto _LL381;else{_tmp84B=_tmp84A->f1;_tmp84C=_tmp84A->f2;}}_LL380:
# 4376
{void*_tmp86B=_tmp84B->r;void*_tmp86C=_tmp86B;struct Cyc_Absyn_Vardecl*_tmp86E;struct Cyc_Absyn_Pat*_tmp870;struct Cyc_Absyn_Exp*_tmp871;struct Cyc_List_List*_tmp873;struct Cyc_Absyn_Fndecl*_tmp875;struct Cyc_Absyn_Tvar*_tmp877;struct Cyc_Absyn_Vardecl*_tmp878;int _tmp879;struct Cyc_Absyn_Exp*_tmp87A;_LL38A: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp86D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp86C;if(_tmp86D->tag != 0)goto _LL38C;else{_tmp86E=_tmp86D->f1;}}_LL38B: {
# 4378
struct _RegionHandle _tmp87B=_new_region("temp");struct _RegionHandle*temp=& _tmp87B;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD8B;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD8A;struct Cyc_Toc_Env*_tmp87C=Cyc_Toc_add_varmap(temp,nv,_tmp86E->name,
Cyc_Absyn_varb_exp(_tmp86E->name,(void*)((_tmpD8A=_cycalloc(sizeof(*_tmpD8A)),((_tmpD8A[0]=((_tmpD8B.tag=4,((_tmpD8B.f1=_tmp86E,_tmpD8B)))),_tmpD8A)))),0));
Cyc_Toc_local_decl_to_c(_tmp87C,_tmp87C,_tmp86E,_tmp84C);}
# 4383
_npop_handler(0);goto _LL389;
# 4378
;_pop_region(temp);}_LL38C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp86F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp86C;if(_tmp86F->tag != 2)goto _LL38E;else{_tmp870=_tmp86F->f1;_tmp871=_tmp86F->f3;}}_LL38D:
# 4387
{void*_tmp87F=_tmp870->r;void*_tmp880=_tmp87F;struct Cyc_Absyn_Vardecl*_tmp882;_LL397: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp881=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp880;if(_tmp881->tag != 1)goto _LL399;else{_tmp882=_tmp881->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp883=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp881->f2)->r;if(_tmp883->tag != 0)goto _LL399;};}}_LL398: {
# 4389
struct _tuple1*old_name=_tmp882->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp882->name=new_name;
_tmp882->initializer=_tmp871;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD8E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD8D;_tmp84B->r=(void*)((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D[0]=((_tmpD8E.tag=0,((_tmpD8E.f1=_tmp882,_tmpD8E)))),_tmpD8D))));}{
struct _RegionHandle _tmp886=_new_region("temp");struct _RegionHandle*temp=& _tmp886;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD91;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD90;struct Cyc_Toc_Env*_tmp887=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90[0]=((_tmpD91.tag=4,((_tmpD91.f1=_tmp882,_tmpD91)))),_tmpD90)))),0));
Cyc_Toc_local_decl_to_c(_tmp887,nv,_tmp882,_tmp84C);}
# 4400
_npop_handler(0);goto _LL396;
# 4394
;_pop_region(temp);};}_LL399:;_LL39A:
# 4405
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp870,(void*)_check_null(_tmp871->topt),_tmp871,_tmp84C))->r;
goto _LL396;_LL396:;}
# 4408
goto _LL389;_LL38E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp872=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp86C;if(_tmp872->tag != 3)goto _LL390;else{_tmp873=_tmp872->f1;}}_LL38F: {
# 4421 "toc.cyc"
struct Cyc_List_List*_tmp88A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp873);
if(_tmp88A == 0){
const char*_tmpD94;void*_tmpD93;(_tmpD93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD94="empty Letv_d",_tag_dyneither(_tmpD94,sizeof(char),13))),_tag_dyneither(_tmpD93,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD97;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD96;_tmp84B->r=(void*)((_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96[0]=((_tmpD97.tag=0,((_tmpD97.f1=(struct Cyc_Absyn_Vardecl*)_tmp88A->hd,_tmpD97)))),_tmpD96))));}
_tmp88A=_tmp88A->tl;
for(0;_tmp88A != 0;_tmp88A=_tmp88A->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD9A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD99;struct Cyc_Absyn_Decl*_tmp88F=Cyc_Absyn_new_decl((void*)((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99[0]=((_tmpD9A.tag=0,((_tmpD9A.f1=(struct Cyc_Absyn_Vardecl*)_tmp88A->hd,_tmpD9A)))),_tmpD99)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp88F,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4430
Cyc_Toc_stmt_to_c(nv,s);
goto _LL389;}_LL390: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp874=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp86C;if(_tmp874->tag != 1)goto _LL392;else{_tmp875=_tmp874->f1;}}_LL391: {
# 4433
struct _tuple1*_tmp892=_tmp875->name;
struct _RegionHandle _tmp893=_new_region("temp");struct _RegionHandle*temp=& _tmp893;_push_region(temp);{
struct Cyc_Toc_Env*_tmp894=Cyc_Toc_add_varmap(temp,nv,_tmp875->name,Cyc_Absyn_var_exp(_tmp892,0));
Cyc_Toc_fndecl_to_c(_tmp894,_tmp875,0);
Cyc_Toc_stmt_to_c(_tmp894,_tmp84C);}
# 4439
_npop_handler(0);goto _LL389;
# 4434
;_pop_region(temp);}_LL392: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp876=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp86C;if(_tmp876->tag != 4)goto _LL394;else{_tmp877=_tmp876->f1;_tmp878=_tmp876->f2;_tmp879=_tmp876->f3;_tmp87A=_tmp876->f4;}}_LL393: {
# 4441
struct Cyc_Absyn_Stmt*_tmp895=_tmp84C;
# 4449
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp878->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4456
struct _RegionHandle _tmp896=_new_region("temp");struct _RegionHandle*temp=& _tmp896;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp895);
# 4459
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp895,0))->r;else{
if((unsigned int)_tmp87A){
Cyc_Toc_exp_to_c(nv,_tmp87A);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(Cyc_Absyn_aggrarrow_exp(_tmp87A,_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"h",sizeof(char),2),0),0);
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp895,0))->r;};}else{
# 4467
struct Cyc_Absyn_Exp*_tmpD9D[1];struct Cyc_Absyn_Exp*_tmpD9C[1];struct Cyc_List_List*_tmpD9B;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD9B=_cycalloc(sizeof(*_tmpD9B)),((_tmpD9B->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD9B->tl=0,_tmpD9B)))))),0),
# 4472
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD9C[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD9C,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp895,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD9D[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD9D,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}}
# 4478
_npop_handler(0);return;
# 4456
;_pop_region(temp);}_LL394:;_LL395: {
# 4479
const char*_tmpDA0;void*_tmpD9F;(_tmpD9F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpDA0="bad nested declaration within function",_tag_dyneither(_tmpDA0,sizeof(char),39))),_tag_dyneither(_tmpD9F,sizeof(void*),0)));}_LL389:;}
# 4481
return;_LL381: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp84D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp84D->tag != 13)goto _LL383;else{_tmp84E=_tmp84D->f1;_tmp84F=_tmp84D->f2;}}_LL382:
# 4483
 s=_tmp84F;continue;_LL383: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp850=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp850->tag != 14)goto _LL385;else{_tmp851=_tmp850->f1;_tmp852=(_tmp850->f2).f1;}}_LL384: {
# 4485
struct _RegionHandle _tmp89D=_new_region("temp");struct _RegionHandle*temp=& _tmp89D;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp851);
Cyc_Toc_exp_to_c(nv,_tmp852);
# 4489
_npop_handler(0);return;
# 4485
;_pop_region(temp);}_LL385: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp853=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp853->tag != 15)goto _LL387;else{_tmp854=_tmp853->f1;_tmp855=_tmp853->f2;}}_LL386: {
# 4506 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4517
e_exp->topt=e_typ;{
struct _RegionHandle _tmp89E=_new_region("temp");struct _RegionHandle*temp=& _tmp89E;_push_region(temp);{
struct Cyc_Toc_Env*_tmp89F=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4521
Cyc_Toc_stmt_to_c(_tmp89F,_tmp854);{
struct Cyc_Absyn_Stmt*_tmp8A0=Cyc_Absyn_seq_stmt(_tmp854,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4527
struct _tuple1*_tmp8A1=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp8A2=Cyc_Absyn_var_exp(_tmp8A1,0);
struct Cyc_Absyn_Vardecl*_tmp8A3=Cyc_Absyn_new_vardecl(_tmp8A1,Cyc_Absyn_exn_typ(),0);
_tmp8A2->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDAA;struct Cyc_Absyn_Pat*_tmpDA9;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpDA8;struct Cyc_Absyn_Pat*_tmpDA7;struct Cyc_Absyn_Pat*_tmp8A4=(_tmpDA7=_cycalloc(sizeof(*_tmpDA7)),((_tmpDA7->r=(void*)((_tmpDAA=_cycalloc(sizeof(*_tmpDAA)),((_tmpDAA[0]=((_tmpDA8.tag=1,((_tmpDA8.f1=_tmp8A3,((_tmpDA8.f2=((_tmpDA9=_cycalloc(sizeof(*_tmpDA9)),((_tmpDA9->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpDA9->topt=Cyc_Absyn_exn_typ(),((_tmpDA9->loc=0,_tmpDA9)))))))),_tmpDA8)))))),_tmpDAA)))),((_tmpDA7->topt=Cyc_Absyn_exn_typ(),((_tmpDA7->loc=0,_tmpDA7)))))));
struct Cyc_Absyn_Exp*_tmp8A5=Cyc_Absyn_throw_exp(_tmp8A2,0);
_tmp8A5->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp8A6=Cyc_Absyn_exp_stmt(_tmp8A5,0);
struct Cyc_Core_Opt*_tmpDB9;struct _tuple35*_tmpDB8;struct Cyc_Absyn_Vardecl**_tmpDB7;struct Cyc_List_List*_tmpDB6;struct Cyc_Absyn_Switch_clause*_tmpDB5;struct Cyc_Absyn_Switch_clause*_tmp8A7=
(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->pattern=_tmp8A4,((_tmpDB5->pat_vars=((_tmpDB9=_cycalloc(sizeof(*_tmpDB9)),((_tmpDB9->v=((_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->hd=((_tmpDB8=_cycalloc(sizeof(*_tmpDB8)),((_tmpDB8->f1=((_tmpDB7=_cycalloc(sizeof(*_tmpDB7)),((_tmpDB7[0]=_tmp8A3,_tmpDB7)))),((_tmpDB8->f2=_tmp8A2,_tmpDB8)))))),((_tmpDB6->tl=0,_tmpDB6)))))),_tmpDB9)))),((_tmpDB5->where_clause=0,((_tmpDB5->body=_tmp8A6,((_tmpDB5->loc=0,_tmpDB5)))))))))));
# 4538
struct Cyc_List_List*_tmpDBA;struct Cyc_Absyn_Stmt*_tmp8A8=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp855,(
(_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=_tmp8A7,((_tmpDBA->tl=0,_tmpDBA))))))),0);
# 4542
Cyc_Toc_stmt_to_c(_tmp89F,_tmp8A8);{
# 4545
struct Cyc_List_List*_tmpDBB;struct Cyc_Absyn_Exp*_tmp8A9=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpDBB=_cycalloc(sizeof(*_tmpDBB)),((_tmpDBB->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpDBB->tl=0,_tmpDBB)))))),0);
# 4549
struct Cyc_List_List*_tmpDBC;struct Cyc_Absyn_Stmt*_tmp8AA=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpDBC->tl=0,_tmpDBC)))))),0),0);
# 4553
struct Cyc_Absyn_Exp*_tmp8AB=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp8AC=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp8AA,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp8AB,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp8A9,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp8AC,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp8A0,
# 4564
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp8A8,0),0),0),0),0),0))->r;};};};};}
# 4568
_npop_handler(0);return;
# 4518
;_pop_region(temp);};}_LL387: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp856=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp829;if(_tmp856->tag != 16)goto _LL366;else{_tmp857=_tmp856->f1;}}_LL388:
# 4570
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4573
Cyc_Toc_exp_to_c(nv,_tmp857);{
struct Cyc_List_List*_tmpDBD;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=_tmp857,((_tmpDBD->tl=0,_tmpDBD)))))),0));};}
# 4576
return;_LL366:;}}
# 4585
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8BA=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8BA;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8BB=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8BC=f->args;for(0;_tmp8BC != 0;_tmp8BC=_tmp8BC->tl){
struct _tuple1*_tmpDBE;struct _tuple1*_tmp8BD=(_tmpDBE=_cycalloc(sizeof(*_tmpDBE)),((_tmpDBE->f1=Cyc_Absyn_Loc_n,((_tmpDBE->f2=(*((struct _tuple9*)_tmp8BC->hd)).f1,_tmpDBE)))));
(*((struct _tuple9*)_tmp8BC->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8BC->hd)).f3);
_tmp8BB=Cyc_Toc_add_varmap(frgn,_tmp8BB,_tmp8BD,Cyc_Absyn_var_exp(_tmp8BD,0));}}
# 4602
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4606
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8BF=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8C1;struct Cyc_Absyn_Tqual _tmp8C2;void*_tmp8C3;int _tmp8C4;struct Cyc_Absyn_VarargInfo _tmp8C0=_tmp8BF;_tmp8C1=_tmp8C0.name;_tmp8C2=_tmp8C0.tq;_tmp8C3=_tmp8C0.type;_tmp8C4=_tmp8C0.inject;{
void*_tmp8C5=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8C3,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8C2,Cyc_Absyn_false_conref));
struct _tuple1*_tmpDBF;struct _tuple1*_tmp8C6=(_tmpDBF=_cycalloc(sizeof(*_tmpDBF)),((_tmpDBF->f1=Cyc_Absyn_Loc_n,((_tmpDBF->f2=(struct _dyneither_ptr*)_check_null(_tmp8C1),_tmpDBF)))));
{struct _tuple9*_tmpDC2;struct Cyc_List_List*_tmpDC1;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpDC1=_cycalloc(sizeof(*_tmpDC1)),((_tmpDC1->hd=((_tmpDC2=_cycalloc(sizeof(*_tmpDC2)),((_tmpDC2->f1=_tmp8C1,((_tmpDC2->f2=_tmp8C2,((_tmpDC2->f3=_tmp8C5,_tmpDC2)))))))),((_tmpDC1->tl=0,_tmpDC1)))))));}
_tmp8BB=Cyc_Toc_add_varmap(frgn,_tmp8BB,_tmp8C6,Cyc_Absyn_var_exp(_tmp8C6,0));
f->cyc_varargs=0;};}
# 4615
{struct Cyc_List_List*_tmp8CA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8CA != 0;_tmp8CA=_tmp8CA->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8CA->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8CA->hd)->type);}}
# 4618
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8BB),f->body);}
# 4594
;_pop_region(frgn);};}
# 4623
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL39B:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL39C:
 return Cyc_Absyn_Extern;default: _LL39D:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4640 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4643
struct _tuple37 _tmp8CB=*env;struct Cyc_Toc_TocState*_tmp8CD;struct Cyc_Absyn_Aggrdecl*_tmp8CE;int _tmp8CF;struct _tuple37 _tmp8CC=_tmp8CB;_tmp8CD=_tmp8CC.f1;_tmp8CE=(_tmp8CC.f2)->f1;_tmp8CF=(_tmp8CC.f2)->f2;{
struct _tuple1*_tmp8D0=_tmp8CE->name;
struct Cyc_Toc_TocState _tmp8D1=*_tmp8CD;struct Cyc_Dict_Dict*_tmp8D3;struct Cyc_Toc_TocState _tmp8D2=_tmp8D1;_tmp8D3=_tmp8D2.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8D4=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8D3,_tmp8D0);
if(_tmp8D4 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8CE->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpDC3;v=((_tmpDC3=_region_malloc(d,sizeof(*_tmpDC3)),((_tmpDC3->f1=_tmp8CE,((_tmpDC3->f2=Cyc_Absyn_strctq(_tmp8D0),_tmpDC3))))));}else{
# 4654
struct _tuple18*_tmpDC4;v=((_tmpDC4=_region_malloc(d,sizeof(*_tmpDC4)),((_tmpDC4->f1=_tmp8CE,((_tmpDC4->f2=Cyc_Absyn_unionq_typ(_tmp8D0),_tmpDC4))))));}
*_tmp8D3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8D3,_tmp8D0,v);};}else{
# 4657
struct _tuple18*_tmp8D7=*_tmp8D4;struct Cyc_Absyn_Aggrdecl*_tmp8D9;void*_tmp8DA;struct _tuple18*_tmp8D8=_tmp8D7;_tmp8D9=_tmp8D8->f1;_tmp8DA=_tmp8D8->f2;
if(_tmp8D9->impl == 0){
{struct _tuple18*_tmpDC5;*_tmp8D3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8D3,_tmp8D0,((_tmpDC5=_region_malloc(d,sizeof(*_tmpDC5)),((_tmpDC5->f1=_tmp8CE,((_tmpDC5->f2=_tmp8DA,_tmpDC5)))))));}
seen_defn_before=0;}else{
# 4662
seen_defn_before=1;}}{
# 4664
struct Cyc_Absyn_Aggrdecl*_tmpDC6;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpDC6=_cycalloc(sizeof(*_tmpDC6)),((_tmpDC6->kind=_tmp8CE->kind,((_tmpDC6->sc=Cyc_Absyn_Public,((_tmpDC6->name=_tmp8CE->name,((_tmpDC6->tvs=0,((_tmpDC6->impl=0,((_tmpDC6->attributes=_tmp8CE->attributes,_tmpDC6)))))))))))));
# 4670
if(_tmp8CE->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpDC7;new_ad->impl=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->exist_vars=0,((_tmpDC7->rgn_po=0,((_tmpDC7->fields=0,((_tmpDC7->tagged=0,_tmpDC7))))))))));}{
# 4675
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8DD=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8CE->impl))->fields;for(0;_tmp8DD != 0;_tmp8DD=_tmp8DD->tl){
# 4679
struct Cyc_Absyn_Aggrfield*_tmp8DE=(struct Cyc_Absyn_Aggrfield*)_tmp8DD->hd;
void*_tmp8DF=_tmp8DE->type;
struct Cyc_List_List*_tmp8E0=_tmp8DE->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8DF)) && (
_tmp8CE->kind == Cyc_Absyn_StructA  && _tmp8DD->tl == 0  || _tmp8CE->kind == Cyc_Absyn_UnionA)){
# 4693 "toc.cyc"
void*_tmp8E1=Cyc_Tcutil_compress(_tmp8DF);void*_tmp8E2=_tmp8E1;void*_tmp8E4;struct Cyc_Absyn_Tqual _tmp8E5;union Cyc_Absyn_Constraint*_tmp8E6;unsigned int _tmp8E7;_LL3A0: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8E3=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E2;if(_tmp8E3->tag != 8)goto _LL3A2;else{_tmp8E4=(_tmp8E3->f1).elt_type;_tmp8E5=(_tmp8E3->f1).tq;_tmp8E6=(_tmp8E3->f1).zero_term;_tmp8E7=(_tmp8E3->f1).zt_loc;}}_LL3A1:
# 4696
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDCD;struct Cyc_Absyn_ArrayInfo _tmpDCC;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDCB;_tmp8DF=(void*)((_tmpDCB=_cycalloc(sizeof(*_tmpDCB)),((_tmpDCB[0]=((_tmpDCD.tag=8,((_tmpDCD.f1=((_tmpDCC.elt_type=_tmp8E4,((_tmpDCC.tq=_tmp8E5,((_tmpDCC.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDCC.zero_term=_tmp8E6,((_tmpDCC.zt_loc=_tmp8E7,_tmpDCC)))))))))),_tmpDCD)))),_tmpDCB))));}
goto _LL39F;_LL3A2:;_LL3A3:
# 4699
{struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmpDD3;struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct _tmpDD2;struct Cyc_List_List*_tmpDD1;_tmp8E0=((_tmpDD1=_cycalloc(sizeof(*_tmpDD1)),((_tmpDD1->hd=(void*)((_tmpDD3=_cycalloc_atomic(sizeof(*_tmpDD3)),((_tmpDD3[0]=((_tmpDD2.tag=6,((_tmpDD2.f1=- 1,_tmpDD2)))),_tmpDD3)))),((_tmpDD1->tl=_tmp8E0,_tmpDD1))))));}{
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDD9;struct Cyc_Absyn_ArrayInfo _tmpDD8;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDD7;_tmp8DF=(void*)((_tmpDD7=_cycalloc(sizeof(*_tmpDD7)),((_tmpDD7[0]=((_tmpDD9.tag=8,((_tmpDD9.f1=((_tmpDD8.elt_type=Cyc_Absyn_void_star_typ(),((_tmpDD8.tq=
Cyc_Absyn_empty_tqual(0),((_tmpDD8.num_elts=
Cyc_Absyn_uint_exp(0,0),((_tmpDD8.zero_term=
((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0),((_tmpDD8.zt_loc=0,_tmpDD8)))))))))),_tmpDD9)))),_tmpDD7))));};_LL39F:;}{
# 4706
struct Cyc_Absyn_Aggrfield*_tmpDDA;struct Cyc_Absyn_Aggrfield*_tmp8F1=(_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->name=_tmp8DE->name,((_tmpDDA->tq=Cyc_Toc_mt_tq,((_tmpDDA->type=
# 4708
Cyc_Toc_typ_to_c(_tmp8DF),((_tmpDDA->width=_tmp8DE->width,((_tmpDDA->attributes=_tmp8E0,((_tmpDDA->requires_clause=0,_tmpDDA)))))))))))));
# 4716
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8CE->impl))->tagged){
void*_tmp8F2=_tmp8F1->type;
struct _dyneither_ptr*_tmp8F3=_tmp8F1->name;
const char*_tmpDDF;void*_tmpDDE[2];struct Cyc_String_pa_PrintArg_struct _tmpDDD;struct Cyc_String_pa_PrintArg_struct _tmpDDC;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDDC.tag=0,((_tmpDDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8F3),((_tmpDDD.tag=0,((_tmpDDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8CE->name).f2),((_tmpDDE[0]=& _tmpDDD,((_tmpDDE[1]=& _tmpDDC,Cyc_aprintf(((_tmpDDF="_union_%s_%s",_tag_dyneither(_tmpDDF,sizeof(char),13))),_tag_dyneither(_tmpDDE,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDE0;struct _dyneither_ptr*str=(_tmpDE0=_cycalloc(sizeof(*_tmpDE0)),((_tmpDE0[0]=s,_tmpDE0)));
struct Cyc_Absyn_Aggrfield*_tmpDE1;struct Cyc_Absyn_Aggrfield*_tmp8F4=(_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1->name=Cyc_Toc_val_sp,((_tmpDE1->tq=Cyc_Toc_mt_tq,((_tmpDE1->type=_tmp8F2,((_tmpDE1->width=0,((_tmpDE1->attributes=0,((_tmpDE1->requires_clause=0,_tmpDE1)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDE2;struct Cyc_Absyn_Aggrfield*_tmp8F5=(_tmpDE2=_cycalloc(sizeof(*_tmpDE2)),((_tmpDE2->name=Cyc_Toc_tag_sp,((_tmpDE2->tq=Cyc_Toc_mt_tq,((_tmpDE2->type=Cyc_Absyn_sint_typ,((_tmpDE2->width=0,((_tmpDE2->attributes=0,((_tmpDE2->requires_clause=0,_tmpDE2)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDE3[2];struct Cyc_List_List*_tmp8F6=(_tmpDE3[1]=_tmp8F4,((_tmpDE3[0]=_tmp8F5,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDE3,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDE8;struct _tuple1*_tmpDE7;struct Cyc_Absyn_Aggrdecl*_tmpDE6;struct Cyc_Absyn_Aggrdecl*_tmp8F7=(_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->kind=Cyc_Absyn_StructA,((_tmpDE6->sc=Cyc_Absyn_Public,((_tmpDE6->name=(
(_tmpDE7=_cycalloc(sizeof(*_tmpDE7)),((_tmpDE7->f1=Cyc_Absyn_Loc_n,((_tmpDE7->f2=str,_tmpDE7)))))),((_tmpDE6->tvs=0,((_tmpDE6->impl=(
(_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->exist_vars=0,((_tmpDE8->rgn_po=0,((_tmpDE8->fields=_tmp8F6,((_tmpDE8->tagged=0,_tmpDE8)))))))))),((_tmpDE6->attributes=0,_tmpDE6)))))))))))));
# 4728
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDEE;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDED;struct Cyc_List_List*_tmpDEC;Cyc_Toc_result_decls=((_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->hd=Cyc_Absyn_new_decl((void*)((_tmpDEE=_cycalloc(sizeof(*_tmpDEE)),((_tmpDEE[0]=((_tmpDED.tag=5,((_tmpDED.f1=_tmp8F7,_tmpDED)))),_tmpDEE)))),0),((_tmpDEC->tl=Cyc_Toc_result_decls,_tmpDEC))))));}
_tmp8F1->type=Cyc_Absyn_strct(str);}{
# 4731
struct Cyc_List_List*_tmpDEF;new_fields=((_tmpDEF=_cycalloc(sizeof(*_tmpDEF)),((_tmpDEF->hd=_tmp8F1,((_tmpDEF->tl=new_fields,_tmpDEF))))));};};}}
# 4733
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4735
if(_tmp8CF){
struct Cyc_Absyn_Decl*_tmpDF9;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF8;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF7;struct Cyc_List_List*_tmpDF6;Cyc_Toc_result_decls=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->hd=((_tmpDF9=_cycalloc(sizeof(*_tmpDF9)),((_tmpDF9->r=(void*)((_tmpDF7=_cycalloc(sizeof(*_tmpDF7)),((_tmpDF7[0]=((_tmpDF8.tag=5,((_tmpDF8.f1=new_ad,_tmpDF8)))),_tmpDF7)))),((_tmpDF9->loc=0,_tmpDF9)))))),((_tmpDF6->tl=Cyc_Toc_result_decls,_tmpDF6))))));}
return 0;};};};}
# 4740
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpDFA;struct _tuple36 p=(_tmpDFA.f1=ad,((_tmpDFA.f2=add_to_decls,_tmpDFA)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4769 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4772
struct _tuple38 _tmp90E=*env;struct Cyc_Set_Set**_tmp910;struct Cyc_Absyn_Datatypedecl*_tmp911;struct _tuple38 _tmp90F=_tmp90E;_tmp910=(_tmp90F.f1)->datatypes_so_far;_tmp911=_tmp90F.f2;{
struct _tuple1*_tmp912=_tmp911->name;
if(_tmp911->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp910,_tmp912))
return 0;
*_tmp910=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp910,_tmp912);
{struct Cyc_List_List*_tmp913=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp911->fields))->v;for(0;_tmp913 != 0;_tmp913=_tmp913->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp913->hd;
# 4780
struct Cyc_List_List*_tmp914=0;
int i=1;
{struct Cyc_List_List*_tmp915=f->typs;for(0;_tmp915 != 0;(_tmp915=_tmp915->tl,i ++)){
struct _dyneither_ptr*_tmp916=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpDFB;struct Cyc_Absyn_Aggrfield*_tmp917=(_tmpDFB=_cycalloc(sizeof(*_tmpDFB)),((_tmpDFB->name=_tmp916,((_tmpDFB->tq=(*((struct _tuple11*)_tmp915->hd)).f1,((_tmpDFB->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp915->hd)).f2),((_tmpDFB->width=0,((_tmpDFB->attributes=0,((_tmpDFB->requires_clause=0,_tmpDFB)))))))))))));
struct Cyc_List_List*_tmpDFC;_tmp914=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC->hd=_tmp917,((_tmpDFC->tl=_tmp914,_tmpDFC))))));}}
# 4788
{struct Cyc_Absyn_Aggrfield*_tmpDFF;struct Cyc_List_List*_tmpDFE;_tmp914=((_tmpDFE=_cycalloc(sizeof(*_tmpDFE)),((_tmpDFE->hd=((_tmpDFF=_cycalloc(sizeof(*_tmpDFF)),((_tmpDFF->name=Cyc_Toc_tag_sp,((_tmpDFF->tq=Cyc_Toc_mt_tq,((_tmpDFF->type=Cyc_Absyn_sint_typ,((_tmpDFF->width=0,((_tmpDFF->attributes=0,((_tmpDFF->requires_clause=0,_tmpDFF)))))))))))))),((_tmpDFE->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp914),_tmpDFE))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE02;struct Cyc_Absyn_Aggrdecl*_tmpE01;struct Cyc_Absyn_Aggrdecl*_tmp91C=
(_tmpE01=_cycalloc(sizeof(*_tmpE01)),((_tmpE01->kind=Cyc_Absyn_StructA,((_tmpE01->sc=Cyc_Absyn_Public,((_tmpE01->name=
Cyc_Toc_collapse_qvars(f->name,_tmp911->name),((_tmpE01->tvs=0,((_tmpE01->impl=(
# 4794
(_tmpE02=_cycalloc(sizeof(*_tmpE02)),((_tmpE02->exist_vars=0,((_tmpE02->rgn_po=0,((_tmpE02->fields=_tmp914,((_tmpE02->tagged=0,_tmpE02)))))))))),((_tmpE01->attributes=0,_tmpE01)))))))))))));
# 4796
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE08;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE07;struct Cyc_List_List*_tmpE06;Cyc_Toc_result_decls=((_tmpE06=_cycalloc(sizeof(*_tmpE06)),((_tmpE06->hd=Cyc_Absyn_new_decl((void*)((_tmpE08=_cycalloc(sizeof(*_tmpE08)),((_tmpE08[0]=((_tmpE07.tag=5,((_tmpE07.f1=_tmp91C,_tmpE07)))),_tmpE08)))),0),((_tmpE06->tl=Cyc_Toc_result_decls,_tmpE06))))));};}}
# 4798
return 0;};}
# 4801
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4820 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4823
struct _tuple38 _tmp922=*env;struct Cyc_Toc_TocState*_tmp924;struct Cyc_Absyn_Datatypedecl*_tmp925;struct _tuple38 _tmp923=_tmp922;_tmp924=_tmp923.f1;_tmp925=_tmp923.f2;
if(_tmp925->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp926=*_tmp924;struct Cyc_Dict_Dict*_tmp928;struct Cyc_Toc_TocState _tmp927=_tmp926;_tmp928=_tmp927.xdatatypes_so_far;{
struct _tuple1*_tmp929=_tmp925->name;
{struct Cyc_List_List*_tmp92A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp925->fields))->v;for(0;_tmp92A != 0;_tmp92A=_tmp92A->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp92A->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp92B=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp92C=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp92B,Cyc_Absyn_false_conref,0);
# 4834
int*_tmp92D=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp928,f->name);int*_tmp92E=_tmp92D;_LL3A5: if(_tmp92E != 0)goto _LL3A7;_LL3A6: {
# 4836
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4840
struct Cyc_Absyn_Vardecl*_tmp92F=Cyc_Absyn_new_vardecl(f->name,_tmp92C,initopt);
_tmp92F->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE0E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE0D;struct Cyc_List_List*_tmpE0C;Cyc_Toc_result_decls=((_tmpE0C=_cycalloc(sizeof(*_tmpE0C)),((_tmpE0C->hd=Cyc_Absyn_new_decl((void*)((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E[0]=((_tmpE0D.tag=0,((_tmpE0D.f1=_tmp92F,_tmpE0D)))),_tmpE0E)))),0),((_tmpE0C->tl=Cyc_Toc_result_decls,_tmpE0C))))));}
*_tmp928=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp928,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp933=f->typs;for(0;_tmp933 != 0;(_tmp933=_tmp933->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpE16;void*_tmpE15[1];const char*_tmpE14;struct _dyneither_ptr*_tmpE13;struct _dyneither_ptr*_tmp934=(_tmpE13=_cycalloc(sizeof(*_tmpE13)),((_tmpE13[0]=(struct _dyneither_ptr)((_tmpE16.tag=1,((_tmpE16.f1=(unsigned long)i,((_tmpE15[0]=& _tmpE16,Cyc_aprintf(((_tmpE14="f%d",_tag_dyneither(_tmpE14,sizeof(char),4))),_tag_dyneither(_tmpE15,sizeof(void*),1)))))))),_tmpE13)));
struct Cyc_Absyn_Aggrfield*_tmpE17;struct Cyc_Absyn_Aggrfield*_tmp935=(_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->name=_tmp934,((_tmpE17->tq=(*((struct _tuple11*)_tmp933->hd)).f1,((_tmpE17->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp933->hd)).f2),((_tmpE17->width=0,((_tmpE17->attributes=0,((_tmpE17->requires_clause=0,_tmpE17)))))))))))));
struct Cyc_List_List*_tmpE18;fields=((_tmpE18=_cycalloc(sizeof(*_tmpE18)),((_tmpE18->hd=_tmp935,((_tmpE18->tl=fields,_tmpE18))))));}}
# 4853
{struct Cyc_Absyn_Aggrfield*_tmpE1B;struct Cyc_List_List*_tmpE1A;fields=((_tmpE1A=_cycalloc(sizeof(*_tmpE1A)),((_tmpE1A->hd=((_tmpE1B=_cycalloc(sizeof(*_tmpE1B)),((_tmpE1B->name=Cyc_Toc_tag_sp,((_tmpE1B->tq=Cyc_Toc_mt_tq,((_tmpE1B->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpE1B->width=0,((_tmpE1B->attributes=0,((_tmpE1B->requires_clause=0,_tmpE1B)))))))))))))),((_tmpE1A->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpE1A))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpE1E;struct Cyc_Absyn_Aggrdecl*_tmpE1D;struct Cyc_Absyn_Aggrdecl*_tmp93E=
(_tmpE1D=_cycalloc(sizeof(*_tmpE1D)),((_tmpE1D->kind=Cyc_Absyn_StructA,((_tmpE1D->sc=Cyc_Absyn_Public,((_tmpE1D->name=
Cyc_Toc_collapse_qvars(f->name,_tmp925->name),((_tmpE1D->tvs=0,((_tmpE1D->impl=(
# 4860
(_tmpE1E=_cycalloc(sizeof(*_tmpE1E)),((_tmpE1E->exist_vars=0,((_tmpE1E->rgn_po=0,((_tmpE1E->fields=fields,((_tmpE1E->tagged=0,_tmpE1E)))))))))),((_tmpE1D->attributes=0,_tmpE1D)))))))))))));
# 4862
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpE24;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpE23;struct Cyc_List_List*_tmpE22;Cyc_Toc_result_decls=((_tmpE22=_cycalloc(sizeof(*_tmpE22)),((_tmpE22->hd=Cyc_Absyn_new_decl((void*)((_tmpE24=_cycalloc(sizeof(*_tmpE24)),((_tmpE24[0]=((_tmpE23.tag=5,((_tmpE23.f1=_tmp93E,_tmpE23)))),_tmpE24)))),0),((_tmpE22->tl=Cyc_Toc_result_decls,_tmpE22))))));}
# 4864
goto _LL3A4;};};};}_LL3A7: if(_tmp92E == 0)goto _LL3A9;if(*_tmp92E != 0)goto _LL3A9;_LL3A8:
# 4866
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp944=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp945=Cyc_Absyn_new_vardecl(f->name,_tmp92C,_tmp944);
_tmp945->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpE2A;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpE29;struct Cyc_List_List*_tmpE28;Cyc_Toc_result_decls=((_tmpE28=_cycalloc(sizeof(*_tmpE28)),((_tmpE28->hd=Cyc_Absyn_new_decl((void*)((_tmpE2A=_cycalloc(sizeof(*_tmpE2A)),((_tmpE2A[0]=((_tmpE29.tag=0,((_tmpE29.f1=_tmp945,_tmpE29)))),_tmpE2A)))),0),((_tmpE28->tl=Cyc_Toc_result_decls,_tmpE28))))));}
*_tmp928=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp928,f->name,1);}
# 4873
goto _LL3A4;_LL3A9:;_LL3AA:
 goto _LL3A4;_LL3A4:;}}
# 4877
return 0;};};}
# 4880
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4885
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4893
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4897
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp949=init->r;void*_tmp94A=_tmp949;struct Cyc_Absyn_Vardecl*_tmp94C;struct Cyc_Absyn_Exp*_tmp94D;struct Cyc_Absyn_Exp*_tmp94E;int _tmp94F;_LL3AC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp94B=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp94A;if(_tmp94B->tag != 26)goto _LL3AE;else{_tmp94C=_tmp94B->f1;_tmp94D=_tmp94B->f2;_tmp94E=_tmp94B->f3;_tmp94F=_tmp94B->f4;}}_LL3AD:
# 4904
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp94C,_tmp94D,_tmp94E,_tmp94F,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3AB;_LL3AE:;_LL3AF:
# 4909
 if(vd->sc == Cyc_Absyn_Static){
# 4913
struct _RegionHandle _tmp950=_new_region("temp");struct _RegionHandle*temp=& _tmp950;_push_region(temp);
{struct Cyc_Toc_Env*_tmp951=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp951,init);}
# 4914
;_pop_region(temp);}else{
# 4918
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3AB;_LL3AB:;}else{
# 4923
void*_tmp952=Cyc_Tcutil_compress(old_typ);void*_tmp953=_tmp952;void*_tmp955;struct Cyc_Absyn_Exp*_tmp956;union Cyc_Absyn_Constraint*_tmp957;_LL3B1:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp954=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp953;if(_tmp954->tag != 8)goto _LL3B3;else{_tmp955=(_tmp954->f1).elt_type;_tmp956=(_tmp954->f1).num_elts;_tmp957=(_tmp954->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp957))goto _LL3B3;_LL3B2:
 if(_tmp956 == 0){
const char*_tmpE2D;void*_tmpE2C;(_tmpE2C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE2D="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpE2D,sizeof(char),55))),_tag_dyneither(_tmpE2C,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp956;
struct Cyc_Absyn_Exp*_tmp95A=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4931
struct Cyc_Absyn_Exp*_tmp95B=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp95A,_tmp95B,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3B0;};_LL3B3:;_LL3B4:
 goto _LL3B0;_LL3B0:;}}
# 4945
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp95C=_new_region("prgn");struct _RegionHandle*prgn=& _tmp95C;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp95D=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp95E=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp95F=
Cyc_Toc_xlate_pat(_tmp95E,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4951
struct Cyc_Toc_Env*_tmp961;struct Cyc_List_List*_tmp962;struct Cyc_Absyn_Stmt*_tmp963;struct _tuple31 _tmp960=_tmp95F;_tmp961=_tmp960.f1;_tmp962=_tmp960.f2;_tmp963=_tmp960.f3;
# 4954
Cyc_Toc_stmt_to_c(_tmp961,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp963,s,0),0);
for(0;_tmp962 != 0;_tmp962=_tmp962->tl){
struct _tuple32*_tmp964=(struct _tuple32*)_tmp962->hd;struct _tuple1*_tmp966;void*_tmp967;struct _tuple32*_tmp965=_tmp964;_tmp966=_tmp965->f1;_tmp967=_tmp965->f2;
s=Cyc_Absyn_declare_stmt(_tmp966,_tmp967,0,s,0);}}{
# 4961
struct Cyc_Absyn_Stmt*_tmp968=s;_npop_handler(0);return _tmp968;};
# 4948
;_pop_region(prgn);};}
# 4967
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp969=e->r;void*_tmp96A=_tmp969;struct Cyc_Absyn_Exp*_tmp96C;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp972;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp978;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_List_List*_tmp97C;struct Cyc_Absyn_Exp*_tmp97E;struct Cyc_Absyn_Exp*_tmp97F;struct Cyc_Absyn_Exp*_tmp981;struct Cyc_Absyn_Exp*_tmp982;struct Cyc_Absyn_Exp*_tmp984;struct Cyc_Absyn_Exp*_tmp985;struct Cyc_Absyn_Exp*_tmp987;struct Cyc_Absyn_Exp*_tmp988;struct Cyc_Absyn_Exp*_tmp98A;struct Cyc_Absyn_Exp*_tmp98B;struct Cyc_Absyn_Exp*_tmp98D;struct Cyc_Absyn_Exp*_tmp98E;struct Cyc_Absyn_Exp*_tmp990;struct Cyc_Absyn_Exp*_tmp991;struct Cyc_Absyn_Exp*_tmp992;struct Cyc_Absyn_Exp*_tmp994;struct Cyc_List_List*_tmp995;void**_tmp997;struct Cyc_Absyn_Exp*_tmp998;void**_tmp99A;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp99D;struct Cyc_List_List*_tmp99F;void**_tmp9A1;void**_tmp9A3;struct Cyc_Absyn_Stmt*_tmp9A5;struct Cyc_Absyn_MallocInfo*_tmp9A7;_LL3B6: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp96B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp96B->tag != 19)goto _LL3B8;else{_tmp96C=_tmp96B->f1;}}_LL3B7:
 _tmp96E=_tmp96C;goto _LL3B9;_LL3B8: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp96D=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp96D->tag != 20)goto _LL3BA;else{_tmp96E=_tmp96D->f1;}}_LL3B9:
 _tmp970=_tmp96E;goto _LL3BB;_LL3BA: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp96F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp96F->tag != 21)goto _LL3BC;else{_tmp970=_tmp96F->f1;}}_LL3BB:
 _tmp972=_tmp970;goto _LL3BD;_LL3BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp971=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp971->tag != 14)goto _LL3BE;else{_tmp972=_tmp971->f1;}}_LL3BD:
 _tmp974=_tmp972;goto _LL3BF;_LL3BE: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp973=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp973->tag != 10)goto _LL3C0;else{_tmp974=_tmp973->f1;}}_LL3BF:
 _tmp976=_tmp974;goto _LL3C1;_LL3C0: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp975=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp975->tag != 11)goto _LL3C2;else{_tmp976=_tmp975->f1;}}_LL3C1:
 _tmp978=_tmp976;goto _LL3C3;_LL3C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp977=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp977->tag != 17)goto _LL3C4;else{_tmp978=_tmp977->f1;}}_LL3C3:
 _tmp97A=_tmp978;goto _LL3C5;_LL3C4: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp979=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp979->tag != 4)goto _LL3C6;else{_tmp97A=_tmp979->f1;}}_LL3C5:
 Cyc_Toc_exptypes_to_c(_tmp97A);goto _LL3B5;_LL3C6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp97B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp97B->tag != 2)goto _LL3C8;else{_tmp97C=_tmp97B->f2;}}_LL3C7:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp97C);goto _LL3B5;_LL3C8: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp97D=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp97D->tag != 6)goto _LL3CA;else{_tmp97E=_tmp97D->f1;_tmp97F=_tmp97D->f2;}}_LL3C9:
 _tmp981=_tmp97E;_tmp982=_tmp97F;goto _LL3CB;_LL3CA: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp980=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp980->tag != 7)goto _LL3CC;else{_tmp981=_tmp980->f1;_tmp982=_tmp980->f2;}}_LL3CB:
 _tmp984=_tmp981;_tmp985=_tmp982;goto _LL3CD;_LL3CC: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp983=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp983->tag != 8)goto _LL3CE;else{_tmp984=_tmp983->f1;_tmp985=_tmp983->f2;}}_LL3CD:
 _tmp987=_tmp984;_tmp988=_tmp985;goto _LL3CF;_LL3CE: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp986=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp986->tag != 22)goto _LL3D0;else{_tmp987=_tmp986->f1;_tmp988=_tmp986->f2;}}_LL3CF:
 _tmp98A=_tmp987;_tmp98B=_tmp988;goto _LL3D1;_LL3D0: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp989->tag != 34)goto _LL3D2;else{_tmp98A=_tmp989->f1;_tmp98B=_tmp989->f2;}}_LL3D1:
 _tmp98D=_tmp98A;_tmp98E=_tmp98B;goto _LL3D3;_LL3D2: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp98C=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp98C->tag != 3)goto _LL3D4;else{_tmp98D=_tmp98C->f1;_tmp98E=_tmp98C->f3;}}_LL3D3:
 Cyc_Toc_exptypes_to_c(_tmp98D);Cyc_Toc_exptypes_to_c(_tmp98E);goto _LL3B5;_LL3D4: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp98F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp98F->tag != 5)goto _LL3D6;else{_tmp990=_tmp98F->f1;_tmp991=_tmp98F->f2;_tmp992=_tmp98F->f3;}}_LL3D5:
# 4985
 Cyc_Toc_exptypes_to_c(_tmp990);Cyc_Toc_exptypes_to_c(_tmp991);Cyc_Toc_exptypes_to_c(_tmp992);goto _LL3B5;_LL3D6: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp993=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp993->tag != 9)goto _LL3D8;else{_tmp994=_tmp993->f1;_tmp995=_tmp993->f2;}}_LL3D7:
# 4987
 Cyc_Toc_exptypes_to_c(_tmp994);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp995);goto _LL3B5;_LL3D8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp996=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp996->tag != 13)goto _LL3DA;else{_tmp997=(void**)((void**)& _tmp996->f1);_tmp998=_tmp996->f2;}}_LL3D9:
*_tmp997=Cyc_Toc_typ_to_c(*_tmp997);Cyc_Toc_exptypes_to_c(_tmp998);goto _LL3B5;_LL3DA: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp999=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp999->tag != 24)goto _LL3DC;else{_tmp99A=(void**)&(_tmp999->f1)->f3;_tmp99B=_tmp999->f2;}}_LL3DB:
# 4990
*_tmp99A=Cyc_Toc_typ_to_c(*_tmp99A);
_tmp99D=_tmp99B;goto _LL3DD;_LL3DC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp99C->tag != 35)goto _LL3DE;else{_tmp99D=_tmp99C->f2;}}_LL3DD:
 _tmp99F=_tmp99D;goto _LL3DF;_LL3DE: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp99E->tag != 25)goto _LL3E0;else{_tmp99F=_tmp99E->f1;}}_LL3DF:
# 4994
 for(0;_tmp99F != 0;_tmp99F=_tmp99F->tl){
struct _tuple19 _tmp9B7=*((struct _tuple19*)_tmp99F->hd);struct Cyc_Absyn_Exp*_tmp9B9;struct _tuple19 _tmp9B8=_tmp9B7;_tmp9B9=_tmp9B8.f2;
Cyc_Toc_exptypes_to_c(_tmp9B9);}
# 4998
goto _LL3B5;_LL3E0: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp9A0=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A0->tag != 18)goto _LL3E2;else{_tmp9A1=(void**)((void**)& _tmp9A0->f1);}}_LL3E1:
 _tmp9A3=_tmp9A1;goto _LL3E3;_LL3E2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp9A2=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A2->tag != 16)goto _LL3E4;else{_tmp9A3=(void**)((void**)& _tmp9A2->f1);}}_LL3E3:
*_tmp9A3=Cyc_Toc_typ_to_c(*_tmp9A3);goto _LL3B5;_LL3E4: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp9A4=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A4->tag != 36)goto _LL3E6;else{_tmp9A5=_tmp9A4->f1;}}_LL3E5:
 Cyc_Toc_stmttypes_to_c(_tmp9A5);goto _LL3B5;_LL3E6: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp9A6=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A6->tag != 33)goto _LL3E8;else{_tmp9A7=(struct Cyc_Absyn_MallocInfo*)& _tmp9A6->f1;}}_LL3E7:
# 5003
 if(_tmp9A7->elt_type != 0){
void**_tmpE2E;_tmp9A7->elt_type=((_tmpE2E=_cycalloc(sizeof(*_tmpE2E)),((_tmpE2E[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp9A7->elt_type))),_tmpE2E))));}
Cyc_Toc_exptypes_to_c(_tmp9A7->num_elts);
goto _LL3B5;_LL3E8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp9A8=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A8->tag != 0)goto _LL3EA;}_LL3E9:
 goto _LL3EB;_LL3EA: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp9A9=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9A9->tag != 1)goto _LL3EC;}_LL3EB:
 goto _LL3ED;_LL3EC: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp9AA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AA->tag != 31)goto _LL3EE;}_LL3ED:
 goto _LL3EF;_LL3EE: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp9AB=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AB->tag != 39)goto _LL3F0;}_LL3EF:
 goto _LL3F1;_LL3F0: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp9AC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AC->tag != 32)goto _LL3F2;}_LL3F1:
 goto _LL3B5;_LL3F2: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp9AD=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AD->tag != 29)goto _LL3F4;}_LL3F3:
# 5013
 goto _LL3F5;_LL3F4: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp9AE=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AE->tag != 30)goto _LL3F6;}_LL3F5:
 goto _LL3F7;_LL3F6: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp9AF=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9AF->tag != 28)goto _LL3F8;}_LL3F7:
 goto _LL3F9;_LL3F8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp9B0=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B0->tag != 26)goto _LL3FA;}_LL3F9:
 goto _LL3FB;_LL3FA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp9B1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B1->tag != 27)goto _LL3FC;}_LL3FB:
 goto _LL3FD;_LL3FC: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp9B2=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B2->tag != 23)goto _LL3FE;}_LL3FD:
 goto _LL3FF;_LL3FE: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp9B3=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B3->tag != 12)goto _LL400;}_LL3FF:
 goto _LL401;_LL400: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp9B4=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B4->tag != 15)goto _LL402;}_LL401:
 goto _LL403;_LL402: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp9B5=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B5->tag != 38)goto _LL404;}_LL403:
 goto _LL405;_LL404: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp9B6=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp96A;if(_tmp9B6->tag != 37)goto _LL3B5;}_LL405:
# 5023
{const char*_tmpE31;void*_tmpE30;(_tmpE30=0,Cyc_Tcutil_terr(e->loc,((_tmpE31="Cyclone expression within C code",_tag_dyneither(_tmpE31,sizeof(char),33))),_tag_dyneither(_tmpE30,sizeof(void*),0)));}
goto _LL3B5;_LL3B5:;}
# 5028
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9BD=d->r;void*_tmp9BE=_tmp9BD;struct Cyc_Absyn_Vardecl*_tmp9C0;struct Cyc_Absyn_Fndecl*_tmp9C2;struct Cyc_Absyn_Aggrdecl*_tmp9C4;struct Cyc_Absyn_Enumdecl*_tmp9C6;struct Cyc_Absyn_Typedefdecl*_tmp9C8;_LL407: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9BF=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9BF->tag != 0)goto _LL409;else{_tmp9C0=_tmp9BF->f1;}}_LL408:
# 5031
 _tmp9C0->type=Cyc_Toc_typ_to_c(_tmp9C0->type);
if(_tmp9C0->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9C0->initializer));
goto _LL406;_LL409: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9C1=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9C1->tag != 1)goto _LL40B;else{_tmp9C2=_tmp9C1->f1;}}_LL40A:
# 5035
 _tmp9C2->ret_type=Cyc_Toc_typ_to_c(_tmp9C2->ret_type);
{struct Cyc_List_List*_tmp9D3=_tmp9C2->args;for(0;_tmp9D3 != 0;_tmp9D3=_tmp9D3->tl){
(*((struct _tuple9*)_tmp9D3->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9D3->hd)).f3);}}
# 5039
goto _LL406;_LL40B: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9C3=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9C3->tag != 5)goto _LL40D;else{_tmp9C4=_tmp9C3->f1;}}_LL40C:
 Cyc_Toc_aggrdecl_to_c(_tmp9C4,1);goto _LL406;_LL40D: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9C5=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9C5->tag != 7)goto _LL40F;else{_tmp9C6=_tmp9C5->f1;}}_LL40E:
# 5042
 if(_tmp9C6->fields != 0){
struct Cyc_List_List*_tmp9D4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9C6->fields))->v;for(0;_tmp9D4 != 0;_tmp9D4=_tmp9D4->tl){
struct Cyc_Absyn_Enumfield*_tmp9D5=(struct Cyc_Absyn_Enumfield*)_tmp9D4->hd;
if(_tmp9D5->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9D5->tag));}}
# 5047
goto _LL406;_LL40F: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9C7=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9C7->tag != 8)goto _LL411;else{_tmp9C8=_tmp9C7->f1;}}_LL410:
 _tmp9C8->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9C8->defn));goto _LL406;_LL411: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9C9=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9C9->tag != 2)goto _LL413;}_LL412:
 goto _LL414;_LL413: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9CA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CA->tag != 3)goto _LL415;}_LL414:
 goto _LL416;_LL415: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9CB=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CB->tag != 6)goto _LL417;}_LL416:
 goto _LL418;_LL417: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9CC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CC->tag != 9)goto _LL419;}_LL418:
 goto _LL41A;_LL419: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9CD=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CD->tag != 10)goto _LL41B;}_LL41A:
 goto _LL41C;_LL41B: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9CE=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CE->tag != 11)goto _LL41D;}_LL41C:
 goto _LL41E;_LL41D: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9CF=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9CF->tag != 12)goto _LL41F;}_LL41E:
 goto _LL420;_LL41F: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9D0=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9D0->tag != 4)goto _LL421;}_LL420:
# 5058
{const char*_tmpE34;void*_tmpE33;(_tmpE33=0,Cyc_Tcutil_terr(d->loc,((_tmpE34="Cyclone declaration within C code",_tag_dyneither(_tmpE34,sizeof(char),34))),_tag_dyneither(_tmpE33,sizeof(void*),0)));}
goto _LL406;_LL421: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9D1=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9D1->tag != 13)goto _LL423;}_LL422:
 goto _LL424;_LL423: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9D2=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9BE;if(_tmp9D2->tag != 14)goto _LL406;}_LL424:
# 5062
 goto _LL406;_LL406:;}
# 5066
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9D8=s->r;void*_tmp9D9=_tmp9D8;struct Cyc_Absyn_Exp*_tmp9DB;struct Cyc_Absyn_Stmt*_tmp9DD;struct Cyc_Absyn_Stmt*_tmp9DE;struct Cyc_Absyn_Exp*_tmp9E0;struct Cyc_Absyn_Exp*_tmp9E2;struct Cyc_Absyn_Stmt*_tmp9E3;struct Cyc_Absyn_Stmt*_tmp9E4;struct Cyc_Absyn_Exp*_tmp9E6;struct Cyc_Absyn_Stmt*_tmp9E7;struct Cyc_Absyn_Exp*_tmp9E9;struct Cyc_Absyn_Exp*_tmp9EA;struct Cyc_Absyn_Exp*_tmp9EB;struct Cyc_Absyn_Stmt*_tmp9EC;struct Cyc_Absyn_Exp*_tmp9EE;struct Cyc_List_List*_tmp9EF;struct Cyc_Absyn_Decl*_tmp9F1;struct Cyc_Absyn_Stmt*_tmp9F2;struct Cyc_Absyn_Stmt*_tmp9F4;struct Cyc_Absyn_Exp*_tmp9F5;_LL426: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9DA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9DA->tag != 1)goto _LL428;else{_tmp9DB=_tmp9DA->f1;}}_LL427:
 Cyc_Toc_exptypes_to_c(_tmp9DB);goto _LL425;_LL428: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9DC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9DC->tag != 2)goto _LL42A;else{_tmp9DD=_tmp9DC->f1;_tmp9DE=_tmp9DC->f2;}}_LL429:
 Cyc_Toc_stmttypes_to_c(_tmp9DD);Cyc_Toc_stmttypes_to_c(_tmp9DE);goto _LL425;_LL42A: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9DF=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9DF->tag != 3)goto _LL42C;else{_tmp9E0=_tmp9DF->f1;}}_LL42B:
 if(_tmp9E0 != 0)Cyc_Toc_exptypes_to_c(_tmp9E0);goto _LL425;_LL42C: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9E1=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9E1->tag != 4)goto _LL42E;else{_tmp9E2=_tmp9E1->f1;_tmp9E3=_tmp9E1->f2;_tmp9E4=_tmp9E1->f3;}}_LL42D:
# 5072
 Cyc_Toc_exptypes_to_c(_tmp9E2);Cyc_Toc_stmttypes_to_c(_tmp9E3);Cyc_Toc_stmttypes_to_c(_tmp9E4);goto _LL425;_LL42E: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9E5=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9E5->tag != 5)goto _LL430;else{_tmp9E6=(_tmp9E5->f1).f1;_tmp9E7=_tmp9E5->f2;}}_LL42F:
# 5074
 Cyc_Toc_exptypes_to_c(_tmp9E6);Cyc_Toc_stmttypes_to_c(_tmp9E7);goto _LL425;_LL430: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9E8=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9E8->tag != 9)goto _LL432;else{_tmp9E9=_tmp9E8->f1;_tmp9EA=(_tmp9E8->f2).f1;_tmp9EB=(_tmp9E8->f3).f1;_tmp9EC=_tmp9E8->f4;}}_LL431:
# 5076
 Cyc_Toc_exptypes_to_c(_tmp9E9);Cyc_Toc_exptypes_to_c(_tmp9EA);Cyc_Toc_exptypes_to_c(_tmp9EB);
Cyc_Toc_stmttypes_to_c(_tmp9EC);goto _LL425;_LL432: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9ED=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9ED->tag != 10)goto _LL434;else{_tmp9EE=_tmp9ED->f1;_tmp9EF=_tmp9ED->f2;}}_LL433:
# 5079
 Cyc_Toc_exptypes_to_c(_tmp9EE);
for(0;_tmp9EF != 0;_tmp9EF=_tmp9EF->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9EF->hd)->body);}
goto _LL425;_LL434: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9F0=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F0->tag != 12)goto _LL436;else{_tmp9F1=_tmp9F0->f1;_tmp9F2=_tmp9F0->f2;}}_LL435:
 Cyc_Toc_decltypes_to_c(_tmp9F1);Cyc_Toc_stmttypes_to_c(_tmp9F2);goto _LL425;_LL436: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9F3=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F3->tag != 14)goto _LL438;else{_tmp9F4=_tmp9F3->f1;_tmp9F5=(_tmp9F3->f2).f1;}}_LL437:
 Cyc_Toc_stmttypes_to_c(_tmp9F4);Cyc_Toc_exptypes_to_c(_tmp9F5);goto _LL425;_LL438: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9F6=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F6->tag != 0)goto _LL43A;}_LL439:
 goto _LL43B;_LL43A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9F7=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F7->tag != 6)goto _LL43C;}_LL43B:
 goto _LL43D;_LL43C: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9F8=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F8->tag != 7)goto _LL43E;}_LL43D:
 goto _LL43F;_LL43E: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9F9=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9F9->tag != 8)goto _LL440;}_LL43F:
 goto _LL425;_LL440: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9FA=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9FA->tag != 11)goto _LL442;}_LL441:
 goto _LL443;_LL442: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9FB=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9FB->tag != 13)goto _LL444;}_LL443:
 goto _LL445;_LL444: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp9FC=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9FC->tag != 15)goto _LL446;}_LL445:
 goto _LL447;_LL446: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp9FD=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9D9;if(_tmp9FD->tag != 16)goto _LL425;}_LL447:
{const char*_tmpE37;void*_tmpE36;(_tmpE36=0,Cyc_Tcutil_terr(s->loc,((_tmpE37="Cyclone statement in C code",_tag_dyneither(_tmpE37,sizeof(char),28))),_tag_dyneither(_tmpE36,sizeof(void*),0)));}
goto _LL425;_LL425:;}
# 5100
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE3A;void*_tmpE39;(_tmpE39=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE3A="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE3A,sizeof(char),29))),_tag_dyneither(_tmpE39,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmpA02=d->r;void*_tmpA03=_tmpA02;struct Cyc_Absyn_Vardecl*_tmpA05;struct Cyc_Absyn_Fndecl*_tmpA07;struct Cyc_Absyn_Aggrdecl*_tmpA0C;struct Cyc_Absyn_Datatypedecl*_tmpA0E;struct Cyc_Absyn_Enumdecl*_tmpA10;struct Cyc_Absyn_Typedefdecl*_tmpA12;struct Cyc_List_List*_tmpA16;struct Cyc_List_List*_tmpA18;struct Cyc_List_List*_tmpA1A;struct Cyc_List_List*_tmpA1C;_LL449: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpA04=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA04->tag != 0)goto _LL44B;else{_tmpA05=_tmpA04->f1;}}_LL44A: {
# 5107
struct _tuple1*_tmpA1D=_tmpA05->name;
# 5109
if(_tmpA05->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE3B;_tmpA1D=((_tmpE3B=_cycalloc(sizeof(*_tmpE3B)),((_tmpE3B->f1=Cyc_Absyn_Rel_n(0),((_tmpE3B->f2=(*_tmpA1D).f2,_tmpE3B))))));}
if(_tmpA05->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmpA05->initializer));else{
# 5115
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmpA05->initializer));}}
# 5117
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE3E;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE3D;nv=Cyc_Toc_add_varmap(r,nv,_tmpA05->name,Cyc_Absyn_varb_exp(_tmpA1D,(void*)((_tmpE3D=_cycalloc(sizeof(*_tmpE3D)),((_tmpE3D[0]=((_tmpE3E.tag=1,((_tmpE3E.f1=_tmpA05,_tmpE3E)))),_tmpE3D)))),0));}
_tmpA05->name=_tmpA1D;
_tmpA05->sc=Cyc_Toc_scope_to_c(_tmpA05->sc);
_tmpA05->type=Cyc_Toc_typ_to_c(_tmpA05->type);
{struct Cyc_List_List*_tmpE3F;Cyc_Toc_result_decls=((_tmpE3F=_cycalloc(sizeof(*_tmpE3F)),((_tmpE3F->hd=d,((_tmpE3F->tl=Cyc_Toc_result_decls,_tmpE3F))))));}
goto _LL448;}_LL44B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmpA06=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA06->tag != 1)goto _LL44D;else{_tmpA07=_tmpA06->f1;}}_LL44C: {
# 5124
struct _tuple1*_tmpA22=_tmpA07->name;
# 5126
if(_tmpA07->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE40;_tmpA22=((_tmpE40=_cycalloc(sizeof(*_tmpE40)),((_tmpE40->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE40->f2=(*_tmpA22).f2,_tmpE40))))));}
_tmpA07->sc=Cyc_Absyn_Public;}
# 5130
nv=Cyc_Toc_add_varmap(r,nv,_tmpA07->name,Cyc_Absyn_var_exp(_tmpA22,0));
_tmpA07->name=_tmpA22;
Cyc_Toc_fndecl_to_c(nv,_tmpA07,cinclude);
{struct Cyc_List_List*_tmpE41;Cyc_Toc_result_decls=((_tmpE41=_cycalloc(sizeof(*_tmpE41)),((_tmpE41->hd=d,((_tmpE41->tl=Cyc_Toc_result_decls,_tmpE41))))));}
goto _LL448;}_LL44D: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmpA08=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA08->tag != 2)goto _LL44F;}_LL44E:
 goto _LL450;_LL44F: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmpA09=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA09->tag != 3)goto _LL451;}_LL450: {
# 5137
const char*_tmpE44;void*_tmpE43;(_tmpE43=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE44="letdecl at toplevel",_tag_dyneither(_tmpE44,sizeof(char),20))),_tag_dyneither(_tmpE43,sizeof(void*),0)));}_LL451: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmpA0A=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA0A->tag != 4)goto _LL453;}_LL452: {
# 5139
const char*_tmpE47;void*_tmpE46;(_tmpE46=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE47="region decl at toplevel",_tag_dyneither(_tmpE47,sizeof(char),24))),_tag_dyneither(_tmpE46,sizeof(void*),0)));}_LL453: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpA0B=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA0B->tag != 5)goto _LL455;else{_tmpA0C=_tmpA0B->f1;}}_LL454:
# 5143
 Cyc_Toc_aggrdecl_to_c(_tmpA0C,1);
goto _LL448;_LL455: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmpA0D=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA0D->tag != 6)goto _LL457;else{_tmpA0E=_tmpA0D->f1;}}_LL456:
# 5146
 if(_tmpA0E->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmpA0E);else{
# 5149
Cyc_Toc_datatypedecl_to_c(_tmpA0E);}
goto _LL448;_LL457: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmpA0F=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA0F->tag != 7)goto _LL459;else{_tmpA10=_tmpA0F->f1;}}_LL458:
# 5152
 Cyc_Toc_enumdecl_to_c(_tmpA10);
{struct Cyc_List_List*_tmpE48;Cyc_Toc_result_decls=((_tmpE48=_cycalloc(sizeof(*_tmpE48)),((_tmpE48->hd=d,((_tmpE48->tl=Cyc_Toc_result_decls,_tmpE48))))));}
goto _LL448;_LL459: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpA11=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA11->tag != 8)goto _LL45B;else{_tmpA12=_tmpA11->f1;}}_LL45A:
# 5156
 _tmpA12->name=_tmpA12->name;
_tmpA12->tvs=0;
if(_tmpA12->defn != 0)
_tmpA12->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmpA12->defn));else{
# 5161
enum Cyc_Absyn_KindQual _tmpA2A=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmpA12->kind))->v)->kind;switch(_tmpA2A){case Cyc_Absyn_BoxKind: _LL467:
 _tmpA12->defn=Cyc_Absyn_void_star_typ();break;default: _LL468:
 _tmpA12->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5166
{struct Cyc_List_List*_tmpE49;Cyc_Toc_result_decls=((_tmpE49=_cycalloc(sizeof(*_tmpE49)),((_tmpE49->hd=d,((_tmpE49->tl=Cyc_Toc_result_decls,_tmpE49))))));}
goto _LL448;_LL45B: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmpA13=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA13->tag != 13)goto _LL45D;}_LL45C:
 goto _LL45E;_LL45D: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmpA14=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA14->tag != 14)goto _LL45F;}_LL45E:
# 5170
 goto _LL448;_LL45F: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmpA15=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA15->tag != 9)goto _LL461;else{_tmpA16=_tmpA15->f2;}}_LL460:
 _tmpA18=_tmpA16;goto _LL462;_LL461: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA17=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA17->tag != 10)goto _LL463;else{_tmpA18=_tmpA17->f2;}}_LL462:
 _tmpA1A=_tmpA18;goto _LL464;_LL463: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA19=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA19->tag != 11)goto _LL465;else{_tmpA1A=_tmpA19->f1;}}_LL464:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA1A,top,cinclude);goto _LL448;_LL465: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA1B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmpA03;if(_tmpA1B->tag != 12)goto _LL448;else{_tmpA1C=_tmpA1B->f1;}}_LL466:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA1C,top,1);goto _LL448;_LL448:;};}
# 5177
return nv;}
# 5181
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA2C=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA2E;struct Cyc_Core_NewDynamicRegion _tmpA2D=_tmpA2C;_tmpA2E=_tmpA2D.key;{
struct Cyc_Toc_TocState*_tmpA2F=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA2E,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE4A;Cyc_Toc_toc_state=((_tmpE4A=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE4A)),((_tmpE4A->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA2E,((_tmpE4A->state=(struct Cyc_Toc_TocState*)_tmpA2F,_tmpE4A))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE4B;Cyc_Toc_globals=_tag_dyneither(((_tmpE4B=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE4B[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE4B[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE4B[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE4B[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE4B[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE4B[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE4B[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE4B[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE4B[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE4B[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE4B[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE4B[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE4B[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE4B[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE4B[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE4B[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE4B[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE4B[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE4B[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE4B[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE4B[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE4B[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE4B[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE4B[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE4B[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE4B[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE4B[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE4B[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE4B[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE4B[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE4B[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE4B[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE4B[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE4B[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE4B[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE4B[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE4B[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE4B[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE4B[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE4B[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE4B[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE4B)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};};}
# 5237
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA32=*ts;struct Cyc_Xarray_Xarray*_tmpA34;struct Cyc_Toc_TocState _tmpA33=_tmpA32;_tmpA34=_tmpA33.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA34);
return 0;}
# 5245
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA35=_new_region("start");struct _RegionHandle*start=& _tmpA35;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5254
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA36=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA38;struct Cyc_Toc_TocState*_tmpA39;struct Cyc_Toc_TocStateWrap _tmpA37=_tmpA36;_tmpA38=_tmpA37.dyn;_tmpA39=_tmpA37.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA38,_tmpA39,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA38);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5261
struct Cyc_List_List*_tmpA3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA3A;};
# 5247
;_pop_region(start);};}
