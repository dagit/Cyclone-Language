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
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 899
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 906
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 929
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 944
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 946
extern void*Cyc_Absyn_sint_typ;
# 958
void*Cyc_Absyn_exn_typ();
# 969
extern void*Cyc_Absyn_bounds_one;
# 977
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 983
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 985
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 988
void*Cyc_Absyn_void_star_typ();
# 990
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 996
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple1*,void*,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1053
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1087
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1094
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1098
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1103
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1144
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);
# 1161
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1177
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
struct Cyc_Dict_Dict*_tmpA30;struct Cyc_Dict_Dict*_tmpA2F;struct Cyc_Set_Set**_tmpA2E;struct Cyc_List_List**_tmpA2D;struct Cyc_Dict_Dict*_tmpA2C;struct Cyc_List_List**_tmpA2B;struct Cyc_Toc_TocState*_tmpA2A;return(_tmpA2A=_region_malloc(d,sizeof(*_tmpA2A)),((_tmpA2A->tuple_types=(
(_tmpA2B=_region_malloc(d,sizeof(*_tmpA2B)),((_tmpA2B[0]=0,_tmpA2B)))),((_tmpA2A->aggrs_so_far=(
(_tmpA2C=_region_malloc(d,sizeof(*_tmpA2C)),((_tmpA2C[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA2C)))),((_tmpA2A->abs_struct_types=(
(_tmpA2D=_region_malloc(d,sizeof(*_tmpA2D)),((_tmpA2D[0]=0,_tmpA2D)))),((_tmpA2A->datatypes_so_far=(
(_tmpA2E=_region_malloc(d,sizeof(*_tmpA2E)),((_tmpA2E[0]=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA2E)))),((_tmpA2A->xdatatypes_so_far=(
(_tmpA2F=_region_malloc(d,sizeof(*_tmpA2F)),((_tmpA2F[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp),_tmpA2F)))),((_tmpA2A->qvar_tags=(
(_tmpA30=_region_malloc(d,sizeof(*_tmpA30)),((_tmpA30[0]=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple14*,struct _tuple14*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp),_tmpA30)))),((_tmpA2A->temp_labels=
((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d),_tmpA2A)))))))))))))));}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
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
{struct Cyc_Toc_TocStateWrap _tmpA31;*ts=((_tmpA31.dyn=(struct Cyc_Core_DynamicRegion*)_tmp11,((_tmpA31.state=(struct Cyc_Toc_TocState*)_tmp12,_tmpA31))));}
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct _tuple15 _tmpA32;struct _tuple15 _tmp15=(_tmpA32.f1=_tmp12,((_tmpA32.f2=arg,_tmpA32)));
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
struct _tuple16 _tmpA33;struct _tuple16 env=(_tmpA33.f1=q,((_tmpA33.f2=type_maker,_tmpA33)));
return((void*(*)(struct _tuple16*arg,void*(*f)(struct _RegionHandle*,struct _tuple17*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}
# 172
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;char Cyc_Toc_Toc_Unimplemented[18]="Toc_Unimplemented";struct Cyc_Toc_Toc_Unimplemented_exn_struct{char*tag;};char Cyc_Toc_Toc_Impossible[15]="Toc_Impossible";struct Cyc_Toc_Toc_Impossible_exn_struct{char*tag;};
# 179
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 182
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA36;void*_tmpA35;(_tmpA35=0,Cyc_fprintf(Cyc_stderr,((_tmpA36="\n",_tag_dyneither(_tmpA36,sizeof(char),2))),_tag_dyneither(_tmpA35,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Unimplemented_exn_struct _tmpA39;struct Cyc_Toc_Toc_Unimplemented_exn_struct*_tmpA38;(int)_throw((void*)((_tmpA38=_cycalloc_atomic(sizeof(*_tmpA38)),((_tmpA38[0]=((_tmpA39.tag=Cyc_Toc_Toc_Unimplemented,_tmpA39)),_tmpA38)))));};}
# 187
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 190
Cyc_vfprintf(Cyc_stderr,fmt,ap);
{const char*_tmpA3C;void*_tmpA3B;(_tmpA3B=0,Cyc_fprintf(Cyc_stderr,((_tmpA3C="\n",_tag_dyneither(_tmpA3C,sizeof(char),2))),_tag_dyneither(_tmpA3B,sizeof(void*),0)));}
Cyc_fflush(Cyc_stderr);{
struct Cyc_Toc_Toc_Impossible_exn_struct _tmpA3F;struct Cyc_Toc_Toc_Impossible_exn_struct*_tmpA3E;(int)_throw((void*)((_tmpA3E=_cycalloc_atomic(sizeof(*_tmpA3E)),((_tmpA3E[0]=((_tmpA3F.tag=Cyc_Toc_Toc_Impossible,_tmpA3F)),_tmpA3E)))));};}char Cyc_Toc_Match_error[12]="Match_error";struct Cyc_Toc_Match_error_exn_struct{char*tag;};static char _tmp29[5]="curr";
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
struct Cyc_Absyn_Stmt**_tmpA40;skip_stmt_opt=((_tmpA40=_cycalloc(sizeof(*_tmpA40)),((_tmpA40[0]=Cyc_Absyn_skip_stmt(0),_tmpA40))));}
return*skip_stmt_opt;}
# 316
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0);}
# 319
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpA43;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmpA42;return(void*)((_tmpA42=_cycalloc(sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=13,((_tmpA43.f1=t,((_tmpA43.f2=e,((_tmpA43.f3=0,((_tmpA43.f4=Cyc_Absyn_No_coercion,_tmpA43)))))))))),_tmpA42))));}
# 322
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmpA46;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmpA45;return(void*)((_tmpA45=_cycalloc(sizeof(*_tmpA45)),((_tmpA45[0]=((_tmpA46.tag=19,((_tmpA46.f1=e,_tmpA46)))),_tmpA45))));}
# 325
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmpA49;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmpA48;return(void*)((_tmpA48=_cycalloc(sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=22,((_tmpA49.f1=e1,((_tmpA49.f2=e2,_tmpA49)))))),_tmpA48))));}
# 328
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmpA4C;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmpA4B;return(void*)((_tmpA4B=_cycalloc(sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=36,((_tmpA4C.f1=s,_tmpA4C)))),_tmpA4B))));}
# 331
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpA4F;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpA4E;return(void*)((_tmpA4E=_cycalloc(sizeof(*_tmpA4E)),((_tmpA4E[0]=((_tmpA4F.tag=16,((_tmpA4F.f1=t,_tmpA4F)))),_tmpA4E))));}
# 334
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpA52;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpA51;return(void*)((_tmpA51=_cycalloc(sizeof(*_tmpA51)),((_tmpA51[0]=((_tmpA52.tag=9,((_tmpA52.f1=e,((_tmpA52.f2=es,((_tmpA52.f3=0,((_tmpA52.f4=1,_tmpA52)))))))))),_tmpA51))));}
# 337
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmpA55;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmpA54;return(void*)((_tmpA54=_cycalloc(sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA55.tag=1,((_tmpA55.f1=e,_tmpA55)))),_tmpA54))));}
# 340
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpA58;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpA57;return(void*)((_tmpA57=_cycalloc(sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA58.tag=2,((_tmpA58.f1=s1,((_tmpA58.f2=s2,_tmpA58)))))),_tmpA57))));}
# 343
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpA5B;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmpA5A;return(void*)((_tmpA5A=_cycalloc(sizeof(*_tmpA5A)),((_tmpA5A[0]=((_tmpA5B.tag=5,((_tmpA5B.f1=e1,((_tmpA5B.f2=e2,((_tmpA5B.f3=e3,_tmpA5B)))))))),_tmpA5A))));}
# 346
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpA5E;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpA5D;return(void*)((_tmpA5D=_cycalloc(sizeof(*_tmpA5D)),((_tmpA5D[0]=((_tmpA5E.tag=20,((_tmpA5E.f1=e,((_tmpA5E.f2=n,((_tmpA5E.f3=0,((_tmpA5E.f4=0,_tmpA5E)))))))))),_tmpA5D))));}
# 349
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpA61;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpA60;return(void*)((_tmpA60=_cycalloc(sizeof(*_tmpA60)),((_tmpA60[0]=((_tmpA61.tag=21,((_tmpA61.f1=e,((_tmpA61.f2=n,((_tmpA61.f3=0,((_tmpA61.f4=0,_tmpA61)))))))))),_tmpA60))));}
# 352
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 355
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpA64;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpA63;return(void*)((_tmpA63=_cycalloc(sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=35,((_tmpA64.f1=tdopt,((_tmpA64.f2=ds,_tmpA64)))))),_tmpA63))));}
# 357
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmpA67;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmpA66;return(void*)((_tmpA66=_cycalloc(sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA67.tag=8,((_tmpA67.f1=v,((_tmpA67.f2=s,_tmpA67)))))),_tmpA66))));}
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
struct Cyc_Core_Impossible_exn_struct _tmpA6D;const char*_tmpA6C;struct Cyc_Core_Impossible_exn_struct*_tmpA6B;(int)_throw((void*)((_tmpA6B=_cycalloc(sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6D.tag=Cyc_Core_Impossible,((_tmpA6D.f1=((_tmpA6C="impossible IntType (not char, short or int)",_tag_dyneither(_tmpA6C,sizeof(char),44))),_tmpA6D)))),_tmpA6B)))));}}
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
struct Cyc_Core_Impossible_exn_struct _tmpA7A;const char*_tmpA79;void*_tmpA78[1];struct Cyc_String_pa_PrintArg_struct _tmpA77;struct Cyc_Core_Impossible_exn_struct*_tmpA76;(int)_throw((void*)((_tmpA76=_cycalloc(sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA7A.tag=Cyc_Core_Impossible,((_tmpA7A.f1=(struct _dyneither_ptr)((_tmpA77.tag=0,((_tmpA77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpA78[0]=& _tmpA77,Cyc_aprintf(((_tmpA79="impossible expression type %s (not int, float, double, or pointer)",_tag_dyneither(_tmpA79,sizeof(char),67))),_tag_dyneither(_tmpA78,sizeof(void*),1)))))))),_tmpA7A)))),_tmpA76)))));}_LL5:;}
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
struct Cyc_Core_Impossible_exn_struct _tmpA80;const char*_tmpA7F;struct Cyc_Core_Impossible_exn_struct*_tmpA7E;(int)_throw((void*)((_tmpA7E=_cycalloc(sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA80.tag=Cyc_Core_Impossible,((_tmpA80.f1=((_tmpA7F="impossible type (not pointer)",_tag_dyneither(_tmpA7F,sizeof(char),30))),_tmpA80)))),_tmpA7E)))));}_LL17:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
const char*_tmpA83;void*_tmpA82;(_tmpA82=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpA83="is_nullable",_tag_dyneither(_tmpA83,sizeof(char),12))),_tag_dyneither(_tmpA82,sizeof(void*),0)));}_LL37:;}
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
char*_tmpA85;unsigned int _tmpA84;struct _dyneither_ptr buf=(_tmpA84=len,((_tmpA85=_cyccalloc_atomic(sizeof(char),_tmpA84),_tag_dyneither(_tmpA85,sizeof(char),_tmpA84))));
struct _dyneither_ptr p=buf;
Cyc_strcpy(p,(struct _dyneither_ptr)s);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)slen);
for(0;nms != 0;nms=nms->tl){
struct _dyneither_ptr*_tmp11B=(struct _dyneither_ptr*)nms->hd;struct _dyneither_ptr _tmp11D;struct _dyneither_ptr*_tmp11C=_tmp11B;_tmp11D=*_tmp11C;
{char _tmpA88;char _tmpA87;struct _dyneither_ptr _tmpA86;(_tmpA86=p,((_tmpA87=*((char*)_check_dyneither_subscript(_tmpA86,sizeof(char),0)),((_tmpA88='_',((_get_dyneither_size(_tmpA86,sizeof(char))== 1  && (_tmpA87 == '\000'  && _tmpA88 != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA86.curr)=_tmpA88)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp11D);
_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)Cyc_strlen((struct _dyneither_ptr)_tmp11D));}
# 527
{char _tmpA8B;char _tmpA8A;struct _dyneither_ptr _tmpA89;(_tmpA89=p,((_tmpA8A=*((char*)_check_dyneither_subscript(_tmpA89,sizeof(char),0)),((_tmpA8B='_',((_get_dyneither_size(_tmpA89,sizeof(char))== 1  && (_tmpA8A == '\000'  && _tmpA8B != '\000')?_throw_arraybounds(): 1,*((char*)_tmpA89.curr)=_tmpA8B)))))));}_dyneither_ptr_inplace_plus(& p,sizeof(char),1);
Cyc_strcpy(p,(struct _dyneither_ptr)_tmp115);_dyneither_ptr_inplace_plus(& p,sizeof(char),(int)vlen);
{const char*_tmpA8C;Cyc_strcpy(p,((_tmpA8C="_struct",_tag_dyneither(_tmpA8C,sizeof(char),8))));}
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
struct _tuple14*_tmpA8D;struct _tuple14*_tmp137=(_tmpA8D=_cycalloc(sizeof(*_tmpA8D)),((_tmpA8D->f1=_tmp12D,((_tmpA8D->f2=_tmp12E,_tmpA8D)))));
union Cyc_Absyn_Nmspace _tmp139;struct _dyneither_ptr _tmp13A;struct _tuple1*_tmp138=_tmp12D;_tmp139=_tmp138->f1;_tmp13A=*_tmp138->f2;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmp13A,_tmp12E);
struct _dyneither_ptr*_tmpA90;struct _tuple1*_tmpA8F;struct _tuple1*res=(_tmpA8F=_cycalloc(sizeof(*_tmpA8F)),((_tmpA8F->f1=_tmp139,((_tmpA8F->f2=((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90[0]=newvar,_tmpA90)))),_tmpA8F)))));
*_tmp129=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple14*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmp129,_tmp137,res);
return res;};}_LL48: _tmp136=_tmp134;_LL49:(void)_throw(_tmp136);_LL45:;}};};};}
# 561
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple14 _tmpA91;struct _tuple14 env=(_tmpA91.f1=fieldname,((_tmpA91.f2=dtname,_tmpA91)));
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
struct Cyc_Int_pa_PrintArg_struct _tmpA99;void*_tmpA98[1];const char*_tmpA97;struct _dyneither_ptr*_tmpA96;struct _dyneither_ptr*xname=(_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96[0]=(struct _dyneither_ptr)((_tmpA99.tag=1,((_tmpA99.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpA98[0]=& _tmpA99,Cyc_aprintf(((_tmpA97="_tuple%d",_tag_dyneither(_tmpA97,sizeof(char),9))),_tag_dyneither(_tmpA98,sizeof(void*),1)))))))),_tmpA96)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmp143);
struct Cyc_List_List*_tmp14A=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
struct Cyc_Absyn_Aggrfield*_tmpA9C;struct Cyc_List_List*_tmpA9B;_tmp14A=((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->name=Cyc_Absyn_fieldname(i),((_tmpA9C->tq=Cyc_Toc_mt_tq,((_tmpA9C->type=(void*)ts2->hd,((_tmpA9C->width=0,((_tmpA9C->attributes=0,((_tmpA9C->requires_clause=0,_tmpA9C)))))))))))))),((_tmpA9B->tl=_tmp14A,_tmpA9B))))));}}
_tmp14A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14A);{
struct Cyc_Absyn_AggrdeclImpl*_tmpAA1;struct _tuple1*_tmpAA0;struct Cyc_Absyn_Aggrdecl*_tmpA9F;struct Cyc_Absyn_Aggrdecl*_tmp14D=(_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->kind=Cyc_Absyn_StructA,((_tmpA9F->sc=Cyc_Absyn_Public,((_tmpA9F->name=(
(_tmpAA0=_cycalloc(sizeof(*_tmpAA0)),((_tmpAA0->f1=Cyc_Absyn_Rel_n(0),((_tmpAA0->f2=xname,_tmpAA0)))))),((_tmpA9F->tvs=0,((_tmpA9F->impl=(
(_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->exist_vars=0,((_tmpAA1->rgn_po=0,((_tmpAA1->fields=_tmp14A,((_tmpAA1->tagged=0,_tmpAA1)))))))))),((_tmpA9F->attributes=0,_tmpA9F)))))))))))));
# 600
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAA7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAA6;struct Cyc_List_List*_tmpAA5;Cyc_Toc_result_decls=((_tmpAA5=_cycalloc(sizeof(*_tmpAA5)),((_tmpAA5->hd=Cyc_Absyn_new_decl((void*)((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7[0]=((_tmpAA6.tag=5,((_tmpAA6.f1=_tmp14D,_tmpAA6)))),_tmpAA7)))),0),((_tmpAA5->tl=Cyc_Toc_result_decls,_tmpAA5))))));}
{struct _tuple22*_tmpAAA;struct Cyc_List_List*_tmpAA9;*_tmp142=((_tmpAA9=_region_malloc(d,sizeof(*_tmpAA9)),((_tmpAA9->hd=((_tmpAAA=_region_malloc(d,sizeof(*_tmpAAA)),((_tmpAAA->f1=x,((_tmpAAA->f2=ts,_tmpAAA)))))),((_tmpAA9->tl=*_tmp142,_tmpAA9))))));}
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
struct Cyc_Int_pa_PrintArg_struct _tmpAB2;void*_tmpAB1[1];const char*_tmpAB0;struct _dyneither_ptr*_tmpAAF;struct _dyneither_ptr*xname=(_tmpAAF=_cycalloc(sizeof(*_tmpAAF)),((_tmpAAF[0]=(struct _dyneither_ptr)((_tmpAB2.tag=1,((_tmpAB2.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpAB1[0]=& _tmpAB2,Cyc_aprintf(((_tmpAB0="_tuple%d",_tag_dyneither(_tmpAB0,sizeof(char),9))),_tag_dyneither(_tmpAB1,sizeof(void*),1)))))))),_tmpAAF)));
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp16C=0;
# 669
{struct _tuple25*_tmpAB5;struct Cyc_List_List*_tmpAB4;*_tmp15C=((_tmpAB4=_region_malloc(d,sizeof(*_tmpAB4)),((_tmpAB4->hd=((_tmpAB5=_region_malloc(d,sizeof(*_tmpAB5)),((_tmpAB5->f1=_tmp15D,((_tmpAB5->f2=_tmp15F,((_tmpAB5->f3=x,_tmpAB5)))))))),((_tmpAB4->tl=*_tmp15C,_tmpAB4))))));}{
# 672
struct _RegionHandle _tmp16F=_new_region("r");struct _RegionHandle*r=& _tmp16F;_push_region(r);
{struct Cyc_List_List*_tmp170=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp15E,_tmp15F);
for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
struct Cyc_Absyn_Aggrfield*_tmp171=(struct Cyc_Absyn_Aggrfield*)_tmp160->hd;
void*t=_tmp171->type;
t=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp170,t));
# 679
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))break;{
struct Cyc_Absyn_Aggrfield*_tmpAB8;struct Cyc_List_List*_tmpAB7;_tmp16C=((_tmpAB7=_cycalloc(sizeof(*_tmpAB7)),((_tmpAB7->hd=((_tmpAB8=_cycalloc(sizeof(*_tmpAB8)),((_tmpAB8->name=_tmp171->name,((_tmpAB8->tq=Cyc_Toc_mt_tq,((_tmpAB8->type=t,((_tmpAB8->width=_tmp171->width,((_tmpAB8->attributes=_tmp171->attributes,((_tmpAB8->requires_clause=0,_tmpAB8)))))))))))))),((_tmpAB7->tl=_tmp16C,_tmpAB7))))));};}
# 683
_tmp16C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp16C);{
struct Cyc_Absyn_AggrdeclImpl*_tmpABD;struct _tuple1*_tmpABC;struct Cyc_Absyn_Aggrdecl*_tmpABB;struct Cyc_Absyn_Aggrdecl*_tmp174=(_tmpABB=_cycalloc(sizeof(*_tmpABB)),((_tmpABB->kind=Cyc_Absyn_StructA,((_tmpABB->sc=Cyc_Absyn_Public,((_tmpABB->name=(
(_tmpABC=_cycalloc(sizeof(*_tmpABC)),((_tmpABC->f1=Cyc_Absyn_Rel_n(0),((_tmpABC->f2=xname,_tmpABC)))))),((_tmpABB->tvs=0,((_tmpABB->impl=(
(_tmpABD=_cycalloc(sizeof(*_tmpABD)),((_tmpABD->exist_vars=0,((_tmpABD->rgn_po=0,((_tmpABD->fields=_tmp16C,((_tmpABD->tagged=0,_tmpABD)))))))))),((_tmpABB->attributes=0,_tmpABB)))))))))))));
# 691
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpAC3;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpAC2;struct Cyc_List_List*_tmpAC1;Cyc_Toc_result_decls=((_tmpAC1=_cycalloc(sizeof(*_tmpAC1)),((_tmpAC1->hd=Cyc_Absyn_new_decl((void*)((_tmpAC3=_cycalloc(sizeof(*_tmpAC3)),((_tmpAC3[0]=((_tmpAC2.tag=5,((_tmpAC2.f1=_tmp174,_tmpAC2)))),_tmpAC3)))),0),((_tmpAC1->tl=Cyc_Toc_result_decls,_tmpAC1))))));}{
void*_tmp178=x;_npop_handler(0);return _tmp178;};};}
# 673
;_pop_region(r);};};}
# 695
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 699
struct _tuple23 _tmpAC4;struct _tuple23 env=(_tmpAC4.f1=struct_name,((_tmpAC4.f2=type_vars,((_tmpAC4.f3=type_args,((_tmpAC4.f4=fields,_tmpAC4)))))));
return((void*(*)(struct _tuple23*arg,void*(*f)(struct _RegionHandle*,struct _tuple24*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 706
struct _tuple1*Cyc_Toc_temp_var(){
int _tmp181=Cyc_Toc_temp_var_counter ++;
struct _dyneither_ptr*_tmpAD1;const char*_tmpAD0;void*_tmpACF[1];struct Cyc_Int_pa_PrintArg_struct _tmpACE;struct _tuple1*_tmpACD;struct _tuple1*res=(_tmpACD=_cycalloc(sizeof(*_tmpACD)),((_tmpACD->f1=Cyc_Absyn_Loc_n,((_tmpACD->f2=((_tmpAD1=_cycalloc(sizeof(*_tmpAD1)),((_tmpAD1[0]=(struct _dyneither_ptr)((_tmpACE.tag=1,((_tmpACE.f1=(unsigned int)_tmp181,((_tmpACF[0]=& _tmpACE,Cyc_aprintf(((_tmpAD0="_tmp%X",_tag_dyneither(_tmpAD0,sizeof(char),7))),_tag_dyneither(_tmpACF,sizeof(void*),1)))))))),_tmpAD1)))),_tmpACD)))));
return res;}struct _tuple26{struct Cyc_Toc_TocState*f1;int f2;};
# 714
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple26*env){
struct _tuple26 _tmp187=*env;struct Cyc_Xarray_Xarray*_tmp189;struct _tuple26 _tmp188=_tmp187;_tmp189=(_tmp188.f1)->temp_labels;{
int _tmp18A=Cyc_Toc_fresh_label_counter ++;
if(_tmp18A < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp189))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp189,_tmp18A);{
struct Cyc_Int_pa_PrintArg_struct _tmpAD9;void*_tmpAD8[1];const char*_tmpAD7;struct _dyneither_ptr*_tmpAD6;struct _dyneither_ptr*res=(_tmpAD6=_cycalloc(sizeof(*_tmpAD6)),((_tmpAD6[0]=(struct _dyneither_ptr)((_tmpAD9.tag=1,((_tmpAD9.f1=(unsigned int)_tmp18A,((_tmpAD8[0]=& _tmpAD9,Cyc_aprintf(((_tmpAD7="_LL%X",_tag_dyneither(_tmpAD7,sizeof(char),6))),_tag_dyneither(_tmpAD8,sizeof(void*),1)))))))),_tmpAD6)));
if(((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp189,res)!= _tmp18A){
const char*_tmpADC;void*_tmpADB;(_tmpADB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpADC="fresh_label: add_ind returned bad index...",_tag_dyneither(_tmpADC,sizeof(char),43))),_tag_dyneither(_tmpADB,sizeof(void*),0)));}
return res;};};}
# 725
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple26*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 732
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp191=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp191))->hd)->name)!= 0){
++ ans;
_tmp191=_tmp191->tl;}
# 739
return Cyc_Absyn_uint_exp((unsigned int)ans,0);}
# 745
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
# 750
struct _tuple9 _tmp192=*a;struct _dyneither_ptr*_tmp194;struct Cyc_Absyn_Tqual _tmp195;void*_tmp196;struct _tuple9 _tmp193=_tmp192;_tmp194=_tmp193.f1;_tmp195=_tmp193.f2;_tmp196=_tmp193.f3;{
struct _tuple9*_tmpADD;return(_tmpADD=_cycalloc(sizeof(*_tmpADD)),((_tmpADD->f1=_tmp194,((_tmpADD->f2=_tmp195,((_tmpADD->f3=Cyc_Toc_typ_to_c(_tmp196),_tmpADD)))))));};}
# 753
static struct _tuple11*Cyc_Toc_typ_to_c_f(struct _tuple11*x){
struct _tuple11 _tmp198=*x;struct Cyc_Absyn_Tqual _tmp19A;void*_tmp19B;struct _tuple11 _tmp199=_tmp198;_tmp19A=_tmp199.f1;_tmp19B=_tmp199.f2;{
struct _tuple11*_tmpADE;return(_tmpADE=_cycalloc(sizeof(*_tmpADE)),((_tmpADE->f1=_tmp19A,((_tmpADE->f2=Cyc_Toc_typ_to_c(_tmp19B),_tmpADE)))));};}
# 772 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp19D=Cyc_Tcutil_compress(t);void*_tmp19E=_tmp19D;void*_tmp1A0;struct Cyc_Absyn_Tqual _tmp1A1;struct Cyc_Absyn_Exp*_tmp1A2;union Cyc_Absyn_Constraint*_tmp1A3;unsigned int _tmp1A4;void*_tmp1A6;_LL52: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp19F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19E;if(_tmp19F->tag != 8)goto _LL54;else{_tmp1A0=(_tmp19F->f1).elt_type;_tmp1A1=(_tmp19F->f1).tq;_tmp1A2=(_tmp19F->f1).num_elts;_tmp1A3=(_tmp19F->f1).zero_term;_tmp1A4=(_tmp19F->f1).zt_loc;}}_LL53:
# 775
 return Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(_tmp1A0),_tmp1A1);_LL54:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1A5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19E;if(_tmp1A5->tag != 1)goto _LL56;else{_tmp1A6=(void*)_tmp1A5->f2;}}if(!(_tmp1A6 != 0))goto _LL56;_LL55:
 return Cyc_Toc_typ_to_c_array(_tmp1A6);_LL56:;_LL57:
 return Cyc_Toc_typ_to_c(t);_LL51:;}
# 781
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f){
# 783
struct Cyc_Absyn_Aggrfield*_tmpADF;return(_tmpADF=_cycalloc(sizeof(*_tmpADF)),((_tmpADF->name=f->name,((_tmpADF->tq=Cyc_Toc_mt_tq,((_tmpADF->type=
# 785
Cyc_Toc_typ_to_c(f->type),((_tmpADF->width=f->width,((_tmpADF->attributes=f->attributes,((_tmpADF->requires_clause=0,_tmpADF)))))))))))));}
# 790
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 792
return;}
# 795
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0){
void**_tmpAE0;cs=((_tmpAE0=_cycalloc(sizeof(*_tmpAE0)),((_tmpAE0[0]=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref),_tmpAE0))));}
# 800
return*cs;}
# 802
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAE1;r=((_tmpAE1=_cycalloc(sizeof(*_tmpAE1)),((_tmpAE1[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp),Cyc_Toc_mt_tq),_tmpAE1))));}
# 807
return*r;}
# 809
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0){
void**_tmpAE2;r=((_tmpAE2=_cycalloc(sizeof(*_tmpAE2)),((_tmpAE2[0]=Cyc_Absyn_cstar_typ(Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp),Cyc_Toc_mt_tq),_tmpAE2))));}
# 814
return*r;}
# 816
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp1AB=Cyc_Tcutil_compress(t);void*_tmp1AC=_tmp1AB;struct Cyc_Absyn_Tvar*_tmp1AE;_LL59: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1AD=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AC;if(_tmp1AD->tag != 2)goto _LL5B;else{_tmp1AE=_tmp1AD->f1;}}_LL5A:
# 819
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);_LL5B:;_LL5C:
 return 0;_LL58:;}
# 823
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp1AF=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp1B0=_tmp1AF;_LL5E: if(_tmp1B0->kind != Cyc_Absyn_AnyKind)goto _LL60;_LL5F:
 return 1;_LL60:;_LL61:
 return 0;_LL5D:;}
# 830
static void*Cyc_Toc_typ_to_c(void*t){
void*_tmp1B1=t;void**_tmp1B4;struct Cyc_Absyn_Tvar*_tmp1B6;struct Cyc_Absyn_Datatypedecl*_tmp1B9;struct Cyc_Absyn_Datatypefield*_tmp1BA;void*_tmp1BD;struct Cyc_Absyn_Tqual _tmp1BE;union Cyc_Absyn_Constraint*_tmp1BF;void*_tmp1C3;struct Cyc_Absyn_Tqual _tmp1C4;struct Cyc_Absyn_Exp*_tmp1C5;unsigned int _tmp1C6;struct Cyc_Absyn_Tqual _tmp1C8;void*_tmp1C9;struct Cyc_List_List*_tmp1CA;int _tmp1CB;struct Cyc_Absyn_VarargInfo*_tmp1CC;struct Cyc_List_List*_tmp1CD;struct Cyc_List_List*_tmp1CF;enum Cyc_Absyn_AggrKind _tmp1D1;struct Cyc_List_List*_tmp1D2;union Cyc_Absyn_AggrInfoU _tmp1D4;struct Cyc_List_List*_tmp1D5;struct _tuple1*_tmp1D7;struct Cyc_List_List*_tmp1D9;struct _tuple1*_tmp1DB;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Typedefdecl*_tmp1DD;void*_tmp1DE;void*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1EA;struct Cyc_Absyn_Aggrdecl*_tmp1ED;struct Cyc_Absyn_Enumdecl*_tmp1F0;struct Cyc_Absyn_Datatypedecl*_tmp1F3;void**_tmp1F4;_LL63: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp1B2=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B2->tag != 0)goto _LL65;}_LL64:
 return t;_LL65: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp1B3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B1;if(_tmp1B3->tag != 1)goto _LL67;else{_tmp1B4=(void**)((void**)& _tmp1B3->f2);}}_LL66:
# 834
 if(*_tmp1B4 == 0){
*_tmp1B4=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 838
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp1B4));_LL67: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp1B5=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B5->tag != 2)goto _LL69;else{_tmp1B6=_tmp1B5->f1;}}_LL68:
# 840
 if((Cyc_Tcutil_tvar_kind(_tmp1B6,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 843
return(void*)& Cyc_Absyn_VoidType_val;else{
# 845
return Cyc_Absyn_void_star_typ();}_LL69: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp1B7=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B7->tag != 3)goto _LL6B;}_LL6A:
# 847
 return(void*)& Cyc_Absyn_VoidType_val;_LL6B: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1B8=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B1;if(_tmp1B8->tag != 4)goto _LL6D;else{if((((_tmp1B8->f1).field_info).KnownDatatypefield).tag != 2)goto _LL6D;_tmp1B9=((struct _tuple2)(((_tmp1B8->f1).field_info).KnownDatatypefield).val).f1;_tmp1BA=((struct _tuple2)(((_tmp1B8->f1).field_info).KnownDatatypefield).val).f2;}}_LL6C:
# 849
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp1BA->name,_tmp1B9->name));_LL6D: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp1BB=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1B1;if(_tmp1BB->tag != 4)goto _LL6F;}_LL6E: {
const char*_tmpAE5;void*_tmpAE4;(_tmpAE4=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAE5="unresolved DatatypeFieldType",_tag_dyneither(_tmpAE5,sizeof(char),29))),_tag_dyneither(_tmpAE4,sizeof(void*),0)));}_LL6F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1BC=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B1;if(_tmp1BC->tag != 5)goto _LL71;else{_tmp1BD=(_tmp1BC->f1).elt_typ;_tmp1BE=(_tmp1BC->f1).elt_tq;_tmp1BF=((_tmp1BC->f1).ptr_atts).bounds;}}_LL70:
# 854
 _tmp1BD=Cyc_Toc_typ_to_c(_tmp1BD);{
void*_tmp1F7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp1BF);void*_tmp1F8=_tmp1F7;_LLA0: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp1F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1F8;if(_tmp1F9->tag != 0)goto _LLA2;}_LLA1:
 return Cyc_Toc_dyneither_ptr_typ;_LLA2:;_LLA3:
 return Cyc_Absyn_star_typ(_tmp1BD,(void*)& Cyc_Absyn_HeapRgn_val,_tmp1BE,Cyc_Absyn_false_conref);_LL9F:;};_LL71: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp1C0=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1B1;if(_tmp1C0->tag != 6)goto _LL73;}_LL72:
# 859
 goto _LL74;_LL73: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp1C1=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp1B1;if(_tmp1C1->tag != 7)goto _LL75;}_LL74:
 return t;_LL75: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp1C2=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B1;if(_tmp1C2->tag != 8)goto _LL77;else{_tmp1C3=(_tmp1C2->f1).elt_type;_tmp1C4=(_tmp1C2->f1).tq;_tmp1C5=(_tmp1C2->f1).num_elts;_tmp1C6=(_tmp1C2->f1).zt_loc;}}_LL76:
# 862
 return Cyc_Absyn_array_typ(Cyc_Toc_typ_to_c(_tmp1C3),_tmp1C4,_tmp1C5,Cyc_Absyn_false_conref,_tmp1C6);_LL77: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1C7=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B1;if(_tmp1C7->tag != 9)goto _LL79;else{_tmp1C8=(_tmp1C7->f1).ret_tqual;_tmp1C9=(_tmp1C7->f1).ret_typ;_tmp1CA=(_tmp1C7->f1).args;_tmp1CB=(_tmp1C7->f1).c_varargs;_tmp1CC=(_tmp1C7->f1).cyc_varargs;_tmp1CD=(_tmp1C7->f1).attributes;}}_LL78: {
# 868
struct Cyc_List_List*_tmp1FA=0;
for(0;_tmp1CD != 0;_tmp1CD=_tmp1CD->tl){
void*_tmp1FB=(void*)_tmp1CD->hd;void*_tmp1FC=_tmp1FB;_LLA5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1FD=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp1FD->tag != 4)goto _LLA7;}_LLA6:
 goto _LLA8;_LLA7: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1FE=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp1FE->tag != 5)goto _LLA9;}_LLA8:
 goto _LLAA;_LLA9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1FF=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp1FF->tag != 19)goto _LLAB;}_LLAA:
 continue;_LLAB: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp200=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp200->tag != 22)goto _LLAD;}_LLAC:
 continue;_LLAD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp201=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp201->tag != 21)goto _LLAF;}_LLAE:
 continue;_LLAF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp202=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1FC;if(_tmp202->tag != 20)goto _LLB1;}_LLB0:
 continue;_LLB1:;_LLB2:
{struct Cyc_List_List*_tmpAE6;_tmp1FA=((_tmpAE6=_cycalloc(sizeof(*_tmpAE6)),((_tmpAE6->hd=(void*)_tmp1CD->hd,((_tmpAE6->tl=_tmp1FA,_tmpAE6))))));}goto _LLA4;_LLA4:;}{
# 879
struct Cyc_List_List*_tmp204=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp1CA);
if(_tmp1CC != 0){
# 882
void*_tmp205=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp1CC->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmpAE7;struct _tuple9*_tmp206=(_tmpAE7=_cycalloc(sizeof(*_tmpAE7)),((_tmpAE7->f1=_tmp1CC->name,((_tmpAE7->f2=_tmp1CC->tq,((_tmpAE7->f3=_tmp205,_tmpAE7)))))));
struct Cyc_List_List*_tmpAE8;_tmp204=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp204,((_tmpAE8=_cycalloc(sizeof(*_tmpAE8)),((_tmpAE8->hd=_tmp206,((_tmpAE8->tl=0,_tmpAE8)))))));}{
# 886
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpAEE;struct Cyc_Absyn_FnInfo _tmpAED;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAEC;return(void*)((_tmpAEC=_cycalloc(sizeof(*_tmpAEC)),((_tmpAEC[0]=((_tmpAEE.tag=9,((_tmpAEE.f1=((_tmpAED.tvars=0,((_tmpAED.effect=0,((_tmpAED.ret_tqual=_tmp1C8,((_tmpAED.ret_typ=Cyc_Toc_typ_to_c(_tmp1C9),((_tmpAED.args=_tmp204,((_tmpAED.c_varargs=_tmp1CB,((_tmpAED.cyc_varargs=0,((_tmpAED.rgn_po=0,((_tmpAED.attributes=_tmp1FA,_tmpAED)))))))))))))))))),_tmpAEE)))),_tmpAEC))));};};}_LL79: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp1CE=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B1;if(_tmp1CE->tag != 10)goto _LL7B;else{_tmp1CF=_tmp1CE->f1;}}_LL7A:
# 891
 _tmp1CF=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_typ_to_c_f,_tmp1CF);
return Cyc_Toc_add_tuple_type(_tmp1CF);_LL7B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1D0=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B1;if(_tmp1D0->tag != 12)goto _LL7D;else{_tmp1D1=_tmp1D0->f1;_tmp1D2=_tmp1D0->f2;}}_LL7C: {
# 896
struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpAF1;struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAF0;return(void*)((_tmpAF0=_cycalloc(sizeof(*_tmpAF0)),((_tmpAF0[0]=((_tmpAF1.tag=12,((_tmpAF1.f1=_tmp1D1,((_tmpAF1.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_aggrfield_to_c,_tmp1D2),_tmpAF1)))))),_tmpAF0))));}_LL7D: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1D3=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B1;if(_tmp1D3->tag != 11)goto _LL7F;else{_tmp1D4=(_tmp1D3->f1).aggr_info;_tmp1D5=(_tmp1D3->f1).targs;}}_LL7E:
# 900
{union Cyc_Absyn_AggrInfoU _tmp20E=_tmp1D4;_LLB4: if((_tmp20E.UnknownAggr).tag != 1)goto _LLB6;_LLB5:
 return t;_LLB6:;_LLB7:
 goto _LLB3;_LLB3:;}{
# 904
struct Cyc_Absyn_Aggrdecl*_tmp20F=Cyc_Absyn_get_known_aggrdecl(_tmp1D4);
# 908
if(_tmp20F->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp210=_tmp20F->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20F->impl))->fields;
if(_tmp210 == 0)return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);
for(0;_tmp210->tl != 0;_tmp210=_tmp210->tl){;}{
void*_tmp211=((struct Cyc_Absyn_Aggrfield*)_tmp210->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp211))){
# 918
struct _RegionHandle _tmp212=_new_region("r");struct _RegionHandle*r=& _tmp212;_push_region(r);
{struct Cyc_List_List*_tmp213=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp20F->tvs,_tmp1D5);
void*_tmp214=Cyc_Tcutil_rsubstitute(r,_tmp213,_tmp211);
if(Cyc_Toc_is_abstract_type(_tmp214)){void*_tmp215=Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);_npop_handler(0);return _tmp215;}{
void*_tmp216=Cyc_Toc_add_struct_type(_tmp20F->name,_tmp20F->tvs,_tmp1D5,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20F->impl))->fields);_npop_handler(0);return _tmp216;};}
# 919
;_pop_region(r);}
# 924
return Cyc_Toc_aggrdecl_type(_tmp20F->name,Cyc_Absyn_strctq);};};};_LL7F: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp1D6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1B1;if(_tmp1D6->tag != 13)goto _LL81;else{_tmp1D7=_tmp1D6->f1;}}_LL80:
 return t;_LL81: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp1D8=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp1B1;if(_tmp1D8->tag != 14)goto _LL83;else{_tmp1D9=_tmp1D8->f1;}}_LL82:
 Cyc_Toc_enumfields_to_c(_tmp1D9);return t;_LL83: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp1DA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B1;if(_tmp1DA->tag != 17)goto _LL85;else{_tmp1DB=_tmp1DA->f1;_tmp1DC=_tmp1DA->f2;_tmp1DD=_tmp1DA->f3;_tmp1DE=(void*)_tmp1DA->f4;}}_LL84:
# 928
 if(_tmp1DE == 0  || Cyc_noexpand_r){
if(_tmp1DC != 0){
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAF4;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF3;return(void*)((_tmpAF3=_cycalloc(sizeof(*_tmpAF3)),((_tmpAF3[0]=((_tmpAF4.tag=17,((_tmpAF4.f1=_tmp1DB,((_tmpAF4.f2=0,((_tmpAF4.f3=_tmp1DD,((_tmpAF4.f4=0,_tmpAF4)))))))))),_tmpAF3))));}else{
return t;}}else{
# 933
struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpAF7;struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF6;return(void*)((_tmpAF6=_cycalloc(sizeof(*_tmpAF6)),((_tmpAF6[0]=((_tmpAF7.tag=17,((_tmpAF7.f1=_tmp1DB,((_tmpAF7.f2=0,((_tmpAF7.f3=_tmp1DD,((_tmpAF7.f4=Cyc_Toc_typ_to_c(_tmp1DE),_tmpAF7)))))))))),_tmpAF6))));}_LL85: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp1DF=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1B1;if(_tmp1DF->tag != 19)goto _LL87;}_LL86:
 return Cyc_Absyn_uint_typ;_LL87: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp1E0=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1B1;if(_tmp1E0->tag != 15)goto _LL89;else{_tmp1E1=(void*)_tmp1E0->f1;}}_LL88:
 return Cyc_Toc_rgn_typ();_LL89: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp1E2=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp1B1;if(_tmp1E2->tag != 16)goto _LL8B;}_LL8A:
 return Cyc_Toc_dyn_rgn_typ();_LL8B: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp1E3=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp1B1;if(_tmp1E3->tag != 20)goto _LL8D;}_LL8C:
# 939
 goto _LL8E;_LL8D: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp1E4=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp1B1;if(_tmp1E4->tag != 21)goto _LL8F;}_LL8E:
 goto _LL90;_LL8F: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp1E5=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp1B1;if(_tmp1E5->tag != 22)goto _LL91;}_LL90:
 goto _LL92;_LL91: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp1E6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1B1;if(_tmp1E6->tag != 23)goto _LL93;}_LL92:
 goto _LL94;_LL93: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1B1;if(_tmp1E7->tag != 24)goto _LL95;}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp1E8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1B1;if(_tmp1E8->tag != 25)goto _LL97;}_LL96:
 return Cyc_Absyn_void_star_typ();_LL97: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E9=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp1B1;if(_tmp1E9->tag != 18)goto _LL99;else{_tmp1EA=_tmp1E9->f1;}}_LL98:
# 949
 return t;_LL99: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1EB=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B1;if(_tmp1EB->tag != 26)goto _LL9B;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp1EC=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp1EB->f1)->r;if(_tmp1EC->tag != 0)goto _LL9B;else{_tmp1ED=_tmp1EC->f1;}}}_LL9A:
# 951
 Cyc_Toc_aggrdecl_to_c(_tmp1ED,1);
if(_tmp1ED->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp1ED->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp1ED->name,Cyc_Absyn_strctq);}_LL9B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1EE=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B1;if(_tmp1EE->tag != 26)goto _LL9D;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1EF=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1EE->f1)->r;if(_tmp1EF->tag != 1)goto _LL9D;else{_tmp1F0=_tmp1EF->f1;}}}_LL9C:
# 956
 Cyc_Toc_enumdecl_to_c(_tmp1F0);
return t;_LL9D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B1;if(_tmp1F1->tag != 26)goto _LL62;else{{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1F2=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1F1->f1)->r;if(_tmp1F2->tag != 2)goto _LL62;else{_tmp1F3=_tmp1F2->f1;}}_tmp1F4=_tmp1F1->f2;}}_LL9E:
# 959
 Cyc_Toc_datatypedecl_to_c(_tmp1F3);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp1F4)));_LL62:;}
# 964
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp21B=t;void*_tmp21D;struct Cyc_Absyn_Tqual _tmp21E;_LLB9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp21C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp21B;if(_tmp21C->tag != 8)goto _LLBB;else{_tmp21D=(_tmp21C->f1).elt_type;_tmp21E=(_tmp21C->f1).tq;}}_LLBA:
# 967
 return Cyc_Toc_cast_it(Cyc_Absyn_star_typ(_tmp21D,(void*)& Cyc_Absyn_HeapRgn_val,_tmp21E,Cyc_Absyn_false_conref),e);_LLBB:;_LLBC:
 return Cyc_Toc_cast_it(t,e);_LLB8:;}
# 974
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp21F=Cyc_Tcutil_compress(t);void*_tmp220=_tmp21F;void*_tmp22A;union Cyc_Absyn_AggrInfoU _tmp22C;struct Cyc_List_List*_tmp22E;struct Cyc_Absyn_Datatypedecl*_tmp230;struct Cyc_Absyn_Datatypefield*_tmp231;struct Cyc_List_List*_tmp233;_LLBE: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp221=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp220;if(_tmp221->tag != 0)goto _LLC0;}_LLBF:
 return 1;_LLC0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp222=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp220;if(_tmp222->tag != 2)goto _LLC2;}_LLC1:
 return 0;_LLC2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp223=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp220;if(_tmp223->tag != 6)goto _LLC4;}_LLC3:
 goto _LLC5;_LLC4: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp224=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp220;if(_tmp224->tag != 13)goto _LLC6;}_LLC5:
 goto _LLC7;_LLC6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp225=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp220;if(_tmp225->tag != 14)goto _LLC8;}_LLC7:
 goto _LLC9;_LLC8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp226=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp220;if(_tmp226->tag != 7)goto _LLCA;}_LLC9:
 goto _LLCB;_LLCA: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp227=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp220;if(_tmp227->tag != 9)goto _LLCC;}_LLCB:
 goto _LLCD;_LLCC: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp228=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp220;if(_tmp228->tag != 19)goto _LLCE;}_LLCD:
 return 1;_LLCE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp229=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp220;if(_tmp229->tag != 8)goto _LLD0;else{_tmp22A=(_tmp229->f1).elt_type;}}_LLCF:
 return Cyc_Toc_atomic_typ(_tmp22A);_LLD0: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp22B=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp220;if(_tmp22B->tag != 11)goto _LLD2;else{_tmp22C=(_tmp22B->f1).aggr_info;}}_LLD1:
# 991
{union Cyc_Absyn_AggrInfoU _tmp238=_tmp22C;_LLE3: if((_tmp238.UnknownAggr).tag != 1)goto _LLE5;_LLE4:
 return 0;_LLE5:;_LLE6:
 goto _LLE2;_LLE2:;}{
# 995
struct Cyc_Absyn_Aggrdecl*_tmp239=Cyc_Absyn_get_known_aggrdecl(_tmp22C);
if(_tmp239->impl == 0)
return 0;
{struct Cyc_List_List*_tmp23A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp239->impl))->fields;for(0;_tmp23A != 0;_tmp23A=_tmp23A->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp23A->hd)->type))return 0;}}
return 1;};_LLD2: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp22D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp220;if(_tmp22D->tag != 12)goto _LLD4;else{_tmp22E=_tmp22D->f2;}}_LLD3:
# 1002
 for(0;_tmp22E != 0;_tmp22E=_tmp22E->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp22E->hd)->type))return 0;}
return 1;_LLD4: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp22F=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp220;if(_tmp22F->tag != 4)goto _LLD6;else{if((((_tmp22F->f1).field_info).KnownDatatypefield).tag != 2)goto _LLD6;_tmp230=((struct _tuple2)(((_tmp22F->f1).field_info).KnownDatatypefield).val).f1;_tmp231=((struct _tuple2)(((_tmp22F->f1).field_info).KnownDatatypefield).val).f2;}}_LLD5:
# 1006
 _tmp233=_tmp231->typs;goto _LLD7;_LLD6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp232=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp220;if(_tmp232->tag != 10)goto _LLD8;else{_tmp233=_tmp232->f1;}}_LLD7:
# 1008
 for(0;_tmp233 != 0;_tmp233=_tmp233->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple11*)_tmp233->hd)).f2))return 0;}
return 1;_LLD8: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp234=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp220;if(_tmp234->tag != 3)goto _LLDA;}_LLD9:
# 1013
 goto _LLDB;_LLDA: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp235=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp220;if(_tmp235->tag != 5)goto _LLDC;}_LLDB:
 goto _LLDD;_LLDC: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp236=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp220;if(_tmp236->tag != 16)goto _LLDE;}_LLDD:
 goto _LLDF;_LLDE: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp237=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp220;if(_tmp237->tag != 15)goto _LLE0;}_LLDF:
 return 0;_LLE0:;_LLE1: {
const char*_tmpAFB;void*_tmpAFA[1];struct Cyc_String_pa_PrintArg_struct _tmpAF9;(_tmpAF9.tag=0,((_tmpAF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpAFA[0]=& _tmpAF9,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFB="atomic_typ:  bad type %s",_tag_dyneither(_tmpAFB,sizeof(char),25))),_tag_dyneither(_tmpAFA,sizeof(void*),1)))))));}_LLBD:;}
# 1021
static int Cyc_Toc_is_void_star(void*t){
void*_tmp23E=Cyc_Tcutil_compress(t);void*_tmp23F=_tmp23E;void*_tmp241;_LLE8: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp240=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23F;if(_tmp240->tag != 5)goto _LLEA;else{_tmp241=(_tmp240->f1).elt_typ;}}_LLE9: {
# 1024
void*_tmp242=Cyc_Tcutil_compress(_tmp241);void*_tmp243=_tmp242;_LLED: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp244=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp243;if(_tmp244->tag != 0)goto _LLEF;}_LLEE:
 return 1;_LLEF:;_LLF0:
 return 0;_LLEC:;}_LLEA:;_LLEB:
# 1028
 return 0;_LLE7:;}
# 1032
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1036
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1041
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp245=Cyc_Tcutil_compress(t);void*_tmp246=_tmp245;union Cyc_Absyn_AggrInfoU _tmp248;struct Cyc_List_List*_tmp24A;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp247=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp246;if(_tmp247->tag != 11)goto _LLF4;else{_tmp248=(_tmp247->f1).aggr_info;}}_LLF3: {
# 1044
struct Cyc_Absyn_Aggrdecl*_tmp24B=Cyc_Absyn_get_known_aggrdecl(_tmp248);
if(_tmp24B->impl == 0){
const char*_tmpAFE;void*_tmpAFD;(_tmpAFD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpAFE="is_poly_field: type missing fields",_tag_dyneither(_tmpAFE,sizeof(char),35))),_tag_dyneither(_tmpAFD,sizeof(void*),0)));}
_tmp24A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp24B->impl))->fields;goto _LLF5;}_LLF4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp249=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp246;if(_tmp249->tag != 12)goto _LLF6;else{_tmp24A=_tmp249->f2;}}_LLF5: {
# 1049
struct Cyc_Absyn_Aggrfield*_tmp24E=Cyc_Absyn_lookup_field(_tmp24A,f);
if(_tmp24E == 0){
const char*_tmpB02;void*_tmpB01[1];struct Cyc_String_pa_PrintArg_struct _tmpB00;(_tmpB00.tag=0,((_tmpB00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB01[0]=& _tmpB00,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB02="is_poly_field: bad field %s",_tag_dyneither(_tmpB02,sizeof(char),28))),_tag_dyneither(_tmpB01,sizeof(void*),1)))))));}
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp24E->type);}_LLF6:;_LLF7: {
const char*_tmpB06;void*_tmpB05[1];struct Cyc_String_pa_PrintArg_struct _tmpB04;(_tmpB04.tag=0,((_tmpB04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB05[0]=& _tmpB04,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB06="is_poly_field: bad type %s",_tag_dyneither(_tmpB06,sizeof(char),27))),_tag_dyneither(_tmpB05,sizeof(void*),1)))))));}_LLF1:;}
# 1060
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp255=e->r;void*_tmp256=_tmp255;struct Cyc_Absyn_Exp*_tmp258;struct _dyneither_ptr*_tmp259;struct Cyc_Absyn_Exp*_tmp25B;struct _dyneither_ptr*_tmp25C;_LLF9: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp256;if(_tmp257->tag != 20)goto _LLFB;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}_LLFA:
# 1063
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp258->topt),_tmp259) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LLFB: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp256;if(_tmp25A->tag != 21)goto _LLFD;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;}}_LLFC: {
# 1066
void*_tmp25D=Cyc_Tcutil_compress((void*)_check_null(_tmp25B->topt));void*_tmp25E=_tmp25D;void*_tmp260;_LL100: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp25F=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25E;if(_tmp25F->tag != 5)goto _LL102;else{_tmp260=(_tmp25F->f1).elt_typ;}}_LL101:
# 1068
 return Cyc_Toc_is_poly_field(_tmp260,_tmp25C) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));_LL102:;_LL103: {
const char*_tmpB0A;void*_tmpB09[1];struct Cyc_String_pa_PrintArg_struct _tmpB08;(_tmpB08.tag=0,((_tmpB08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp25B->topt))),((_tmpB09[0]=& _tmpB08,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB0A="is_poly_project: bad type %s",_tag_dyneither(_tmpB0A,sizeof(char),29))),_tag_dyneither(_tmpB09,sizeof(void*),1)))))));}_LLFF:;}_LLFD:;_LLFE:
# 1071
 return 0;_LLF8:;}
# 1076
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB0B;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_e,((_tmpB0B=_cycalloc(sizeof(*_tmpB0B)),((_tmpB0B->hd=s,((_tmpB0B->tl=0,_tmpB0B)))))),0);}
# 1080
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
struct Cyc_List_List*_tmpB0C;return Cyc_Absyn_fncall_exp(Cyc_Toc__cycalloc_atomic_e,((_tmpB0C=_cycalloc(sizeof(*_tmpB0C)),((_tmpB0C->hd=s,((_tmpB0C->tl=0,_tmpB0C)))))),0);}
# 1084
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1090
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
struct Cyc_Absyn_Exp*_tmpB0D[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_malloc_e,((_tmpB0D[1]=s,((_tmpB0D[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB0D,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
# 1094
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type)){
struct Cyc_Absyn_Exp*_tmpB0E[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_atomic_e,((_tmpB0E[1]=n,((_tmpB0E[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB0E,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}else{
# 1098
struct Cyc_Absyn_Exp*_tmpB0F[2];return Cyc_Absyn_fncall_exp(Cyc_Toc__cyccalloc_e,((_tmpB0F[1]=n,((_tmpB0F[0]=s,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB0F,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}}
# 1101
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
struct Cyc_Absyn_Exp*_tmpB10[3];return Cyc_Absyn_fncall_exp(Cyc_Toc__region_calloc_e,((_tmpB10[2]=n,((_tmpB10[1]=s,((_tmpB10[0]=rgn,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB10,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_List_List*_tmpB11;return Cyc_Absyn_fncall_exp(Cyc_Toc__throw_e,((_tmpB11=_cycalloc(sizeof(*_tmpB11)),((_tmpB11->hd=e,((_tmpB11->tl=0,_tmpB11)))))),0);}
# 1110
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
struct Cyc_Absyn_Exp*_tmpB12[0];return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_match_e,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB12,sizeof(struct Cyc_Absyn_Exp*),0)),0),0);}
# 1116
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1124
int is_string=0;
{void*_tmp26C=e->r;void*_tmp26D=_tmp26C;_LL105: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26D;if(_tmp26E->tag != 0)goto _LL107;else{if(((_tmp26E->f1).String_c).tag != 8)goto _LL107;}}_LL106:
 is_string=1;goto _LL104;_LL107: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp26F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp26D;if(_tmp26F->tag != 0)goto _LL109;else{if(((_tmp26F->f1).Wstring_c).tag != 9)goto _LL109;}}_LL108:
 is_string=1;goto _LL104;_LL109:;_LL10A:
 goto _LL104;_LL104:;}{
# 1130
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpB18;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpB17;struct Cyc_List_List*_tmpB16;Cyc_Toc_result_decls=((_tmpB16=_cycalloc(sizeof(*_tmpB16)),((_tmpB16->hd=Cyc_Absyn_new_decl((void*)((_tmpB18=_cycalloc(sizeof(*_tmpB18)),((_tmpB18[0]=((_tmpB17.tag=0,((_tmpB17.f1=vd,_tmpB17)))),_tmpB18)))),0),((_tmpB16->tl=Cyc_Toc_result_decls,_tmpB16))))));}
xexp=Cyc_Absyn_var_exp(x,0);
xplussz=Cyc_Absyn_add_exp(xexp,sz,0);}else{
# 1140
xexp=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),e);
# 1142
xplussz=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),Cyc_Absyn_add_exp(e,sz,0));}{
# 1144
struct Cyc_Absyn_Exp*urm_exp;
{struct _tuple19*_tmpB1F;struct _tuple19*_tmpB1E;struct _tuple19*_tmpB1D;struct _tuple19*_tmpB1C[3];urm_exp=Cyc_Absyn_unresolvedmem_exp(0,((_tmpB1C[2]=(
# 1147
(_tmpB1D=_cycalloc(sizeof(*_tmpB1D)),((_tmpB1D->f1=0,((_tmpB1D->f2=xplussz,_tmpB1D)))))),((_tmpB1C[1]=(
# 1146
(_tmpB1E=_cycalloc(sizeof(*_tmpB1E)),((_tmpB1E->f1=0,((_tmpB1E->f2=xexp,_tmpB1E)))))),((_tmpB1C[0]=(
# 1145
(_tmpB1F=_cycalloc(sizeof(*_tmpB1F)),((_tmpB1F->f1=0,((_tmpB1F->f2=xexp,_tmpB1F)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpB1C,sizeof(struct _tuple19*),3)))))))),0);}
# 1148
return urm_exp;};};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;struct Cyc_Dict_Dict next_case_env;};struct Cyc_Toc_StructInfo{struct Cyc_Absyn_Exp**varsizeexp;struct Cyc_Absyn_Exp*lhs_exp;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;struct Cyc_Dict_Dict varmap;int toplevel;int*in_lhs;struct Cyc_Toc_StructInfo struct_info;int*in_sizeof;struct _RegionHandle*rgn;};
# 1191 "toc.cyc"
typedef struct Cyc_Toc_Env*Cyc_Toc_env_t;
# 1193
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
int _tmp278;struct Cyc_Toc_Env*_tmp277=nv;_tmp278=_tmp277->toplevel;
return _tmp278;}
# 1197
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
int*_tmp27A;struct Cyc_Toc_Env*_tmp279=nv;_tmp27A=_tmp279->in_lhs;
return*_tmp27A;}
# 1202
static struct Cyc_Absyn_Exp*Cyc_Toc_lookup_varmap(struct Cyc_Toc_Env*nv,struct _tuple1*x){
struct Cyc_Dict_Dict _tmp27C;struct Cyc_Toc_Env*_tmp27B=nv;_tmp27C=_tmp27B->varmap;
return((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp27C,x);}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
int*_tmpB27;struct Cyc_Absyn_Exp**_tmpB26;int*_tmpB25;struct Cyc_Toc_Env*_tmpB24;return(_tmpB24=_region_malloc(r,sizeof(*_tmpB24)),((_tmpB24->break_lab=(struct _dyneither_ptr**)0,((_tmpB24->continue_lab=(struct _dyneither_ptr**)0,((_tmpB24->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB24->varmap=(struct Cyc_Dict_Dict)
# 1212
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),((_tmpB24->toplevel=(int)1,((_tmpB24->in_lhs=(int*)(
# 1214
(_tmpB25=_region_malloc(r,sizeof(*_tmpB25)),((_tmpB25[0]=0,_tmpB25)))),((_tmpB24->struct_info=(struct Cyc_Toc_StructInfo)(
((_tmpB24->struct_info).varsizeexp=((_tmpB26=_cycalloc(sizeof(*_tmpB26)),((_tmpB26[0]=0,_tmpB26)))),(((_tmpB24->struct_info).lhs_exp=0,_tmpB24->struct_info)))),((_tmpB24->in_sizeof=(int*)(
(_tmpB27=_region_malloc(r,sizeof(*_tmpB27)),((_tmpB27[0]=0,_tmpB27)))),((_tmpB24->rgn=(struct _RegionHandle*)r,_tmpB24)))))))))))))))))));}
# 1220
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp282;struct _dyneither_ptr**_tmp283;struct Cyc_Toc_FallthruInfo*_tmp284;struct Cyc_Dict_Dict _tmp285;int _tmp286;int*_tmp287;struct Cyc_Toc_StructInfo _tmp288;int*_tmp289;struct Cyc_Toc_Env*_tmp281=e;_tmp282=_tmp281->break_lab;_tmp283=_tmp281->continue_lab;_tmp284=_tmp281->fallthru_info;_tmp285=_tmp281->varmap;_tmp286=_tmp281->toplevel;_tmp287=_tmp281->in_lhs;_tmp288=_tmp281->struct_info;_tmp289=_tmp281->in_sizeof;{
struct Cyc_Toc_Env*_tmpB28;return(_tmpB28=_region_malloc(r,sizeof(*_tmpB28)),((_tmpB28->break_lab=(struct _dyneither_ptr**)_tmp282,((_tmpB28->continue_lab=(struct _dyneither_ptr**)_tmp283,((_tmpB28->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp284,((_tmpB28->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp285),((_tmpB28->toplevel=(int)_tmp286,((_tmpB28->in_lhs=(int*)_tmp287,((_tmpB28->struct_info=(struct Cyc_Toc_StructInfo)_tmp288,((_tmpB28->in_sizeof=(int*)_tmp289,((_tmpB28->rgn=(struct _RegionHandle*)r,_tmpB28)))))))))))))))))));};}
# 1225
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp28C;struct _dyneither_ptr**_tmp28D;struct Cyc_Toc_FallthruInfo*_tmp28E;struct Cyc_Dict_Dict _tmp28F;int _tmp290;int*_tmp291;struct Cyc_Toc_StructInfo _tmp292;int*_tmp293;struct Cyc_Toc_Env*_tmp28B=e;_tmp28C=_tmp28B->break_lab;_tmp28D=_tmp28B->continue_lab;_tmp28E=_tmp28B->fallthru_info;_tmp28F=_tmp28B->varmap;_tmp290=_tmp28B->toplevel;_tmp291=_tmp28B->in_lhs;_tmp292=_tmp28B->struct_info;_tmp293=_tmp28B->in_sizeof;{
struct Cyc_Toc_Env*_tmpB29;return(_tmpB29=_region_malloc(r,sizeof(*_tmpB29)),((_tmpB29->break_lab=(struct _dyneither_ptr**)_tmp28C,((_tmpB29->continue_lab=(struct _dyneither_ptr**)_tmp28D,((_tmpB29->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp28E,((_tmpB29->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp28F),((_tmpB29->toplevel=(int)0,((_tmpB29->in_lhs=(int*)_tmp291,((_tmpB29->struct_info=(struct Cyc_Toc_StructInfo)_tmp292,((_tmpB29->in_sizeof=(int*)_tmp293,((_tmpB29->rgn=(struct _RegionHandle*)r,_tmpB29)))))))))))))))))));};}
# 1229
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp296;struct _dyneither_ptr**_tmp297;struct Cyc_Toc_FallthruInfo*_tmp298;struct Cyc_Dict_Dict _tmp299;int _tmp29A;int*_tmp29B;struct Cyc_Toc_StructInfo _tmp29C;int*_tmp29D;struct Cyc_Toc_Env*_tmp295=e;_tmp296=_tmp295->break_lab;_tmp297=_tmp295->continue_lab;_tmp298=_tmp295->fallthru_info;_tmp299=_tmp295->varmap;_tmp29A=_tmp295->toplevel;_tmp29B=_tmp295->in_lhs;_tmp29C=_tmp295->struct_info;_tmp29D=_tmp295->in_sizeof;{
struct Cyc_Toc_Env*_tmpB2A;return(_tmpB2A=_region_malloc(r,sizeof(*_tmpB2A)),((_tmpB2A->break_lab=(struct _dyneither_ptr**)_tmp296,((_tmpB2A->continue_lab=(struct _dyneither_ptr**)_tmp297,((_tmpB2A->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp298,((_tmpB2A->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp299),((_tmpB2A->toplevel=(int)1,((_tmpB2A->in_lhs=(int*)_tmp29B,((_tmpB2A->struct_info=(struct Cyc_Toc_StructInfo)_tmp29C,((_tmpB2A->in_sizeof=(int*)_tmp29D,((_tmpB2A->rgn=(struct _RegionHandle*)r,_tmpB2A)))))))))))))))))));};}
# 1233
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
int*_tmp2A0;struct Cyc_Toc_Env*_tmp29F=e;_tmp2A0=_tmp29F->in_lhs;
*_tmp2A0=b;}
# 1237
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
int*_tmp2A2;struct Cyc_Toc_Env*_tmp2A1=e;_tmp2A2=_tmp2A1->in_sizeof;{
int _tmp2A3=*_tmp2A2;
*_tmp2A2=b;
return _tmp2A3;};}
# 1243
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
int*_tmp2A5;struct Cyc_Toc_Env*_tmp2A4=e;_tmp2A5=_tmp2A4->in_sizeof;
return*_tmp2A5;}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_set_lhs_exp(struct _RegionHandle*r,struct Cyc_Absyn_Exp*exp,struct Cyc_Toc_Env*e){
# 1250
struct _dyneither_ptr**_tmp2A7;struct _dyneither_ptr**_tmp2A8;struct Cyc_Toc_FallthruInfo*_tmp2A9;struct Cyc_Dict_Dict _tmp2AA;int _tmp2AB;int*_tmp2AC;struct Cyc_Absyn_Exp**_tmp2AD;int*_tmp2AE;struct Cyc_Toc_Env*_tmp2A6=e;_tmp2A7=_tmp2A6->break_lab;_tmp2A8=_tmp2A6->continue_lab;_tmp2A9=_tmp2A6->fallthru_info;_tmp2AA=_tmp2A6->varmap;_tmp2AB=_tmp2A6->toplevel;_tmp2AC=_tmp2A6->in_lhs;_tmp2AD=(_tmp2A6->struct_info).varsizeexp;_tmp2AE=_tmp2A6->in_sizeof;{
struct Cyc_Toc_Env*_tmpB2B;return(_tmpB2B=_region_malloc(r,sizeof(*_tmpB2B)),((_tmpB2B->break_lab=(struct _dyneither_ptr**)_tmp2A7,((_tmpB2B->continue_lab=(struct _dyneither_ptr**)_tmp2A8,((_tmpB2B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2A9,((_tmpB2B->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2AA),((_tmpB2B->toplevel=(int)_tmp2AB,((_tmpB2B->in_lhs=(int*)_tmp2AC,((_tmpB2B->struct_info=(struct Cyc_Toc_StructInfo)(((_tmpB2B->struct_info).varsizeexp=_tmp2AD,(((_tmpB2B->struct_info).lhs_exp=exp,_tmpB2B->struct_info)))),((_tmpB2B->in_sizeof=(int*)_tmp2AE,((_tmpB2B->rgn=(struct _RegionHandle*)r,_tmpB2B)))))))))))))))))));};}
# 1256
static struct Cyc_Toc_Env*Cyc_Toc_add_varmap(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _tuple1*x,struct Cyc_Absyn_Exp*y){
{union Cyc_Absyn_Nmspace _tmp2B0=(*x).f1;union Cyc_Absyn_Nmspace _tmp2B1=_tmp2B0;_LL10C: if((_tmp2B1.Rel_n).tag != 1)goto _LL10E;_LL10D: {
# 1259
const char*_tmpB2F;void*_tmpB2E[1];struct Cyc_String_pa_PrintArg_struct _tmpB2D;(_tmpB2D.tag=0,((_tmpB2D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(x)),((_tmpB2E[0]=& _tmpB2D,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB2F="Toc::add_varmap on Rel_n: %s\n",_tag_dyneither(_tmpB2F,sizeof(char),30))),_tag_dyneither(_tmpB2E,sizeof(void*),1)))))));}_LL10E:;_LL10F:
 goto _LL10B;_LL10B:;}{
# 1262
struct _dyneither_ptr**_tmp2B6;struct _dyneither_ptr**_tmp2B7;struct Cyc_Toc_FallthruInfo*_tmp2B8;struct Cyc_Dict_Dict _tmp2B9;int _tmp2BA;int*_tmp2BB;struct Cyc_Toc_StructInfo _tmp2BC;int*_tmp2BD;struct Cyc_Toc_Env*_tmp2B5=e;_tmp2B6=_tmp2B5->break_lab;_tmp2B7=_tmp2B5->continue_lab;_tmp2B8=_tmp2B5->fallthru_info;_tmp2B9=_tmp2B5->varmap;_tmp2BA=_tmp2B5->toplevel;_tmp2BB=_tmp2B5->in_lhs;_tmp2BC=_tmp2B5->struct_info;_tmp2BD=_tmp2B5->in_sizeof;{
struct Cyc_Dict_Dict _tmp2BE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2B9),x,y);
struct Cyc_Toc_Env*_tmpB30;return(_tmpB30=_region_malloc(r,sizeof(*_tmpB30)),((_tmpB30->break_lab=(struct _dyneither_ptr**)_tmp2B6,((_tmpB30->continue_lab=(struct _dyneither_ptr**)_tmp2B7,((_tmpB30->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2B8,((_tmpB30->varmap=(struct Cyc_Dict_Dict)_tmp2BE,((_tmpB30->toplevel=(int)_tmp2BA,((_tmpB30->in_lhs=(int*)_tmp2BB,((_tmpB30->struct_info=(struct Cyc_Toc_StructInfo)_tmp2BC,((_tmpB30->in_sizeof=(int*)_tmp2BD,((_tmpB30->rgn=(struct _RegionHandle*)r,_tmpB30)))))))))))))))))));};};}
# 1269
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct _dyneither_ptr**_tmp2C1;struct _dyneither_ptr**_tmp2C2;struct Cyc_Toc_FallthruInfo*_tmp2C3;struct Cyc_Dict_Dict _tmp2C4;int _tmp2C5;int*_tmp2C6;struct Cyc_Toc_StructInfo _tmp2C7;int*_tmp2C8;struct Cyc_Toc_Env*_tmp2C0=e;_tmp2C1=_tmp2C0->break_lab;_tmp2C2=_tmp2C0->continue_lab;_tmp2C3=_tmp2C0->fallthru_info;_tmp2C4=_tmp2C0->varmap;_tmp2C5=_tmp2C0->toplevel;_tmp2C6=_tmp2C0->in_lhs;_tmp2C7=_tmp2C0->struct_info;_tmp2C8=_tmp2C0->in_sizeof;{
struct Cyc_Toc_Env*_tmpB31;return(_tmpB31=_region_malloc(r,sizeof(*_tmpB31)),((_tmpB31->break_lab=(struct _dyneither_ptr**)0,((_tmpB31->continue_lab=(struct _dyneither_ptr**)0,((_tmpB31->fallthru_info=(struct Cyc_Toc_FallthruInfo*)_tmp2C3,((_tmpB31->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2C4),((_tmpB31->toplevel=(int)_tmp2C5,((_tmpB31->in_lhs=(int*)_tmp2C6,((_tmpB31->struct_info=(struct Cyc_Toc_StructInfo)_tmp2C7,((_tmpB31->in_sizeof=(int*)_tmp2C8,((_tmpB31->rgn=(struct _RegionHandle*)r,_tmpB31)))))))))))))))))));};}
# 1275
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders,struct Cyc_Toc_Env*next_case_env){
# 1280
struct Cyc_List_List*fallthru_vars=0;
for(0;fallthru_binders != 0;fallthru_binders=fallthru_binders->tl){
struct Cyc_List_List*_tmpB32;fallthru_vars=((_tmpB32=_region_malloc(r,sizeof(*_tmpB32)),((_tmpB32->hd=((struct Cyc_Absyn_Vardecl*)fallthru_binders->hd)->name,((_tmpB32->tl=fallthru_vars,_tmpB32))))));}
fallthru_vars=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fallthru_vars);{
struct _dyneither_ptr**_tmp2CC;struct _dyneither_ptr**_tmp2CD;struct Cyc_Toc_FallthruInfo*_tmp2CE;struct Cyc_Dict_Dict _tmp2CF;int _tmp2D0;int*_tmp2D1;struct Cyc_Toc_StructInfo _tmp2D2;int*_tmp2D3;struct Cyc_Toc_Env*_tmp2CB=e;_tmp2CC=_tmp2CB->break_lab;_tmp2CD=_tmp2CB->continue_lab;_tmp2CE=_tmp2CB->fallthru_info;_tmp2CF=_tmp2CB->varmap;_tmp2D0=_tmp2CB->toplevel;_tmp2D1=_tmp2CB->in_lhs;_tmp2D2=_tmp2CB->struct_info;_tmp2D3=_tmp2CB->in_sizeof;{
struct Cyc_Dict_Dict _tmp2D5;struct Cyc_Toc_Env*_tmp2D4=next_case_env;_tmp2D5=_tmp2D4->varmap;{
struct Cyc_Toc_FallthruInfo*_tmpB33;struct Cyc_Toc_FallthruInfo*fi=
(_tmpB33=_region_malloc(r,sizeof(*_tmpB33)),((_tmpB33->label=fallthru_l,((_tmpB33->binders=fallthru_vars,((_tmpB33->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2D5),_tmpB33)))))));
struct _dyneither_ptr**_tmpB36;struct Cyc_Toc_Env*_tmpB35;return(_tmpB35=_region_malloc(r,sizeof(*_tmpB35)),((_tmpB35->break_lab=(struct _dyneither_ptr**)((_tmpB36=_region_malloc(r,sizeof(*_tmpB36)),((_tmpB36[0]=break_l,_tmpB36)))),((_tmpB35->continue_lab=(struct _dyneither_ptr**)_tmp2CD,((_tmpB35->fallthru_info=(struct Cyc_Toc_FallthruInfo*)fi,((_tmpB35->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2CF),((_tmpB35->toplevel=(int)_tmp2D0,((_tmpB35->in_lhs=(int*)_tmp2D1,((_tmpB35->struct_info=(struct Cyc_Toc_StructInfo)_tmp2D2,((_tmpB35->in_sizeof=(int*)_tmp2D3,((_tmpB35->rgn=(struct _RegionHandle*)r,_tmpB35)))))))))))))))))));};};};}
# 1292
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1295
struct _dyneither_ptr**_tmp2DA;struct _dyneither_ptr**_tmp2DB;struct Cyc_Toc_FallthruInfo*_tmp2DC;struct Cyc_Dict_Dict _tmp2DD;int _tmp2DE;int*_tmp2DF;struct Cyc_Toc_StructInfo _tmp2E0;int*_tmp2E1;struct Cyc_Toc_Env*_tmp2D9=e;_tmp2DA=_tmp2D9->break_lab;_tmp2DB=_tmp2D9->continue_lab;_tmp2DC=_tmp2D9->fallthru_info;_tmp2DD=_tmp2D9->varmap;_tmp2DE=_tmp2D9->toplevel;_tmp2DF=_tmp2D9->in_lhs;_tmp2E0=_tmp2D9->struct_info;_tmp2E1=_tmp2D9->in_sizeof;{
struct _dyneither_ptr**_tmpB39;struct Cyc_Toc_Env*_tmpB38;return(_tmpB38=_region_malloc(r,sizeof(*_tmpB38)),((_tmpB38->break_lab=(struct _dyneither_ptr**)((_tmpB39=_region_malloc(r,sizeof(*_tmpB39)),((_tmpB39[0]=break_l,_tmpB39)))),((_tmpB38->continue_lab=(struct _dyneither_ptr**)_tmp2DB,((_tmpB38->fallthru_info=(struct Cyc_Toc_FallthruInfo*)0,((_tmpB38->varmap=(struct Cyc_Dict_Dict)((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2DD),((_tmpB38->toplevel=(int)_tmp2DE,((_tmpB38->in_lhs=(int*)_tmp2DF,((_tmpB38->struct_info=(struct Cyc_Toc_StructInfo)_tmp2E0,((_tmpB38->in_sizeof=(int*)_tmp2E1,((_tmpB38->rgn=(struct _RegionHandle*)r,_tmpB38)))))))))))))))))));};}
# 1302
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1305
struct _dyneither_ptr**_tmp2E5;struct _dyneither_ptr**_tmp2E6;struct Cyc_Toc_FallthruInfo*_tmp2E7;struct Cyc_Dict_Dict _tmp2E8;int _tmp2E9;int*_tmp2EA;struct Cyc_Toc_StructInfo _tmp2EB;int*_tmp2EC;struct Cyc_Toc_Env*_tmp2E4=e;_tmp2E5=_tmp2E4->break_lab;_tmp2E6=_tmp2E4->continue_lab;_tmp2E7=_tmp2E4->fallthru_info;_tmp2E8=_tmp2E4->varmap;_tmp2E9=_tmp2E4->toplevel;_tmp2EA=_tmp2E4->in_lhs;_tmp2EB=_tmp2E4->struct_info;_tmp2EC=_tmp2E4->in_sizeof;{
struct Cyc_Toc_FallthruInfo*_tmpB3C;struct Cyc_Toc_Env*_tmpB3B;return(_tmpB3B=_region_malloc(r,sizeof(*_tmpB3B)),((_tmpB3B->break_lab=(struct _dyneither_ptr**)0,((_tmpB3B->continue_lab=(struct _dyneither_ptr**)_tmp2E6,((_tmpB3B->fallthru_info=(struct Cyc_Toc_FallthruInfo*)((_tmpB3C=_region_malloc(r,sizeof(*_tmpB3C)),((_tmpB3C->label=next_l,((_tmpB3C->binders=0,((_tmpB3C->next_case_env=
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(r,Cyc_Absyn_qvar_cmp),_tmpB3C)))))))),((_tmpB3B->varmap=(struct Cyc_Dict_Dict)
((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,struct Cyc_Dict_Dict))Cyc_Dict_rshare)(r,_tmp2E8),((_tmpB3B->toplevel=(int)_tmp2E9,((_tmpB3B->in_lhs=(int*)_tmp2EA,((_tmpB3B->struct_info=(struct Cyc_Toc_StructInfo)_tmp2EB,((_tmpB3B->in_sizeof=(int*)_tmp2EC,((_tmpB3B->rgn=(struct _RegionHandle*)r,_tmpB3B)))))))))))))))))));};}
# 1320 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1323
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1325
void*_tmp2EF=e->annot;void*_tmp2F0=_tmp2EF;_LL111: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2F1=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2F0;if(_tmp2F1->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL113;}_LL112:
# 1328
 return Cyc_Toc_is_nullable((void*)_check_null(e->topt));_LL113: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2F2=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2F0;if(_tmp2F2->tag != Cyc_CfFlowInfo_NotZero)goto _LL115;}_LL114:
# 1331
 return 0;_LL115: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2F3=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2F0;if(_tmp2F3->tag != Cyc_CfFlowInfo_IsZero)goto _LL117;}_LL116:
# 1333
{const char*_tmpB3F;void*_tmpB3E;(_tmpB3E=0,Cyc_Tcutil_terr(e->loc,((_tmpB3F="dereference of NULL pointer",_tag_dyneither(_tmpB3F,sizeof(char),28))),_tag_dyneither(_tmpB3E,sizeof(void*),0)));}
return 0;_LL117: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp2F4=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2F0;if(_tmp2F4->tag != Cyc_Absyn_EmptyAnnot)goto _LL119;}_LL118:
# 1339
 return 1;_LL119:;_LL11A: {
const char*_tmpB42;void*_tmpB41;(_tmpB41=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB42="need_null_check",_tag_dyneither(_tmpB42,sizeof(char),16))),_tag_dyneither(_tmpB41,sizeof(void*),0)));}_LL110:;}
# 1344
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp2F9=e->annot;void*_tmp2FA=_tmp2F9;struct Cyc_List_List*_tmp2FC;struct Cyc_List_List*_tmp2FE;_LL11C: {struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*_tmp2FB=(struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp2FA;if(_tmp2FB->tag != Cyc_CfFlowInfo_UnknownZ)goto _LL11E;else{_tmp2FC=_tmp2FB->f1;}}_LL11D:
 return _tmp2FC;_LL11E: {struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*_tmp2FD=(struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp2FA;if(_tmp2FD->tag != Cyc_CfFlowInfo_NotZero)goto _LL120;else{_tmp2FE=_tmp2FD->f1;}}_LL11F:
 return _tmp2FE;_LL120: {struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*_tmp2FF=(struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp2FA;if(_tmp2FF->tag != Cyc_CfFlowInfo_IsZero)goto _LL122;}_LL121:
# 1349
{const char*_tmpB45;void*_tmpB44;(_tmpB44=0,Cyc_Tcutil_terr(e->loc,((_tmpB45="dereference of NULL pointer",_tag_dyneither(_tmpB45,sizeof(char),28))),_tag_dyneither(_tmpB44,sizeof(void*),0)));}
return 0;_LL122: {struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*_tmp300=(struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp2FA;if(_tmp300->tag != Cyc_Absyn_EmptyAnnot)goto _LL124;}_LL123:
 return 0;_LL124:;_LL125: {
const char*_tmpB48;void*_tmpB47;(_tmpB47=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB48="get_relns",_tag_dyneither(_tmpB48,sizeof(char),10))),_tag_dyneither(_tmpB47,sizeof(void*),0)));}_LL11B:;}static char _tmp30A[8]="*bogus*";
# 1359
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1369 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1372
struct Cyc_Absyn_Vardecl*x;
{void*_tmp305=a->r;void*_tmp306=_tmp305;void*_tmp308;_LL127: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp307=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp306;if(_tmp307->tag != 1)goto _LL129;else{_tmp308=(void*)_tmp307->f2;}}_LL128: {
# 1375
struct Cyc_Absyn_Vardecl*_tmp309=Cyc_Tcutil_nonesc_vardecl(_tmp308);
if(_tmp309 == 0)goto _LL12A;
x=_tmp309;
goto _LL126;}_LL129:;_LL12A: {
# 1381
static struct _dyneither_ptr bogus_string={_tmp30A,_tmp30A,_tmp30A + 8};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1384
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,{0,0,0,0,0},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1390
x=& bogus_vardecl;
x->type=a_typ;}_LL126:;}{
# 1393
void*_tmp30B=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1398
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp30B);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
relns=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);}
# 1407
if(valid_rop_i){
struct Cyc_List_List*_tmp30C=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1415
if(!Cyc_Relations_consistent_relations(_tmp30C))return 1;}
# 1418
inner_loop: {
void*_tmp30D=i->r;void*_tmp30E=_tmp30D;void*_tmp310;struct Cyc_Absyn_Exp*_tmp311;struct Cyc_Absyn_Exp*_tmp313;struct Cyc_Absyn_Exp*_tmp314;_LL12C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp30F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30E;if(_tmp30F->tag != 13)goto _LL12E;else{_tmp310=(void*)_tmp30F->f1;_tmp311=_tmp30F->f2;}}_LL12D:
 i=_tmp311;goto inner_loop;_LL12E: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp312=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30E;if(_tmp312->tag != 2)goto _LL130;else{if(_tmp312->f1 != Cyc_Absyn_Mod)goto _LL130;if(_tmp312->f2 == 0)goto _LL130;_tmp313=(struct Cyc_Absyn_Exp*)(_tmp312->f2)->hd;if((_tmp312->f2)->tl == 0)goto _LL130;_tmp314=(struct Cyc_Absyn_Exp*)((_tmp312->f2)->tl)->hd;}}_LL12F: {
# 1424
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0);
if(Cyc_Relations_exp2relnop(_tmp314,& rop_z)){
# 1427
struct Cyc_List_List*_tmp315=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1429
return !Cyc_Relations_consistent_relations(_tmp315);}
# 1431
goto _LL12B;}_LL130:;_LL131:
 goto _LL12B;_LL12B:;}
# 1434
return 0;};}
# 1437
static void*Cyc_Toc_get_c_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB4B;void*_tmpB4A;(_tmpB4A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4B="Missing type in primop ",_tag_dyneither(_tmpB4B,sizeof(char),24))),_tag_dyneither(_tmpB4A,sizeof(void*),0)));}
return Cyc_Toc_typ_to_c((void*)_check_null(e->topt));}
# 1441
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0){const char*_tmpB4E;void*_tmpB4D;(_tmpB4D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB4E="Missing type in primop ",_tag_dyneither(_tmpB4E,sizeof(char),24))),_tag_dyneither(_tmpB4D,sizeof(void*),0)));}
return(void*)_check_null(e->topt);}
# 1445
static struct _tuple11*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
struct _tuple11*_tmpB4F;return(_tmpB4F=_cycalloc(sizeof(*_tmpB4F)),((_tmpB4F->f1=Cyc_Toc_mt_tq,((_tmpB4F->f2=Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmpB4F)))));}
# 1448
static struct _tuple19*Cyc_Toc_add_designator(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple19*_tmpB50;return(_tmpB50=_cycalloc(sizeof(*_tmpB50)),((_tmpB50->f1=0,((_tmpB50->f2=e,_tmpB50)))));};}
# 1455
static struct Cyc_Absyn_Exp*Cyc_Toc_make_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*x,void*struct_typ,struct Cyc_Absyn_Stmt*s,int pointer,struct Cyc_Absyn_Exp*rgnopt,int is_atomic,int do_declare){
# 1458
struct Cyc_Absyn_Exp*eo;
void*t;
if(pointer){
struct Cyc_Absyn_Exp*_tmp31C=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(x,0),0);
if(*(nv->struct_info).varsizeexp != 0){
_tmp31C=Cyc_Absyn_add_exp(_tmp31C,(struct Cyc_Absyn_Exp*)_check_null(*(nv->struct_info).varsizeexp),0);
*(nv->struct_info).varsizeexp=0;}
# 1466
t=Cyc_Absyn_cstar_typ(struct_typ,Cyc_Toc_mt_tq);
if(rgnopt == 0  || Cyc_Absyn_no_regions)
eo=is_atomic?Cyc_Toc_malloc_atomic(_tmp31C): Cyc_Toc_malloc_ptr(_tmp31C);else{
# 1470
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);
eo=Cyc_Toc_rmalloc_exp(r,_tmp31C);}}else{
# 1475
t=struct_typ;
eo=0;}
# 1478
if(do_declare){
void*_tmp31D=x->r;void*_tmp31E=_tmp31D;struct _tuple1*_tmp320;_LL133: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp31F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp31E;if(_tmp31F->tag != 1)goto _LL135;else{_tmp320=_tmp31F->f1;}}_LL134:
# 1481
 return Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp320,t,eo,s,0),0);_LL135:;_LL136: {
# 1483
const char*_tmpB53;void*_tmpB52;(_tmpB52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB53="bogus x to make_struct",_tag_dyneither(_tmpB53,sizeof(char),23))),_tag_dyneither(_tmpB52,sizeof(void*),0)));}_LL132:;}else{
# 1487
if(eo != 0)
return Cyc_Absyn_stmt_exp(Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(x,eo,0),0),s,0),0);else{
# 1490
return Cyc_Absyn_stmt_exp(s,0);}}}
# 1494
static int Cyc_Toc_abstract_aggr(void*typ){
void*_tmp323=Cyc_Tcutil_compress(typ);void*_tmp324=_tmp323;union Cyc_Absyn_AggrInfoU _tmp326;struct Cyc_List_List*_tmp327;struct Cyc_List_List*_tmp329;_LL138: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp325=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp324;if(_tmp325->tag != 11)goto _LL13A;else{_tmp326=(_tmp325->f1).aggr_info;_tmp327=(_tmp325->f1).targs;}}_LL139: {
# 1497
struct Cyc_Absyn_Aggrdecl*ad=Cyc_Absyn_get_known_aggrdecl(_tmp326);
if(ad->impl == 0)return 0;{
struct Cyc_List_List*aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
_tmp329=aggrfields;goto _LL13B;};}_LL13A: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp328=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp324;if(_tmp328->tag != 12)goto _LL13C;else{_tmp329=_tmp328->f2;}}_LL13B:
# 1502
 if(_tmp329 == 0)return 0;
return 1;_LL13C:;_LL13D:
# 1508
 return 0;_LL137:;}
# 1512
static void*Cyc_Toc_array_elt_type(void*t){
void*_tmp32A=Cyc_Tcutil_compress(t);void*_tmp32B=_tmp32A;void*_tmp32D;_LL13F: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp32C=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp32B;if(_tmp32C->tag != 8)goto _LL141;else{_tmp32D=(_tmp32C->f1).elt_type;}}_LL140:
 return _tmp32D;_LL141:;_LL142: {
const char*_tmpB57;void*_tmpB56[1];struct Cyc_String_pa_PrintArg_struct _tmpB55;(_tmpB55.tag=0,((_tmpB55.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpB56[0]=& _tmpB55,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB57="array_elt_type called on %s",_tag_dyneither(_tmpB57,sizeof(char),28))),_tag_dyneither(_tmpB56,sizeof(void*),1)))))));}_LL13E:;}
# 1519
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated);
# 1525
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s);
# 1530
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_array(struct Cyc_Toc_Env*nv,void*array_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_List_List*dles0,struct Cyc_Absyn_Stmt*s){
# 1535
void*orig_elt_type=Cyc_Toc_array_elt_type(array_type);
int count=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dles0)- 1;
{struct Cyc_List_List*_tmp331=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles0);for(0;_tmp331 != 0;_tmp331=_tmp331->tl){
# 1539
struct _tuple19*_tmp332=(struct _tuple19*)_tmp331->hd;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp335;struct _tuple19*_tmp333=_tmp332;_tmp334=_tmp333->f1;_tmp335=_tmp333->f2;{
# 1543
struct Cyc_Absyn_Exp*e_index;
if(_tmp334 == 0)
e_index=Cyc_Absyn_signed_int_exp(count --,0);else{
# 1547
if(_tmp334->tl != 0){const char*_tmpB5A;void*_tmpB59;(_tmpB59=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB5A="multiple designators in array",_tag_dyneither(_tmpB5A,sizeof(char),30))),_tag_dyneither(_tmpB59,sizeof(void*),0)));}{
void*_tmp338=(void*)_tmp334->hd;
void*_tmp339=_tmp338;struct Cyc_Absyn_Exp*_tmp33B;_LL144: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33A=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp339;if(_tmp33A->tag != 0)goto _LL146;else{_tmp33B=_tmp33A->f1;}}_LL145:
# 1551
 Cyc_Toc_exp_to_c(nv,_tmp33B);
e_index=_tmp33B;
goto _LL143;_LL146: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp339;if(_tmp33C->tag != 1)goto _LL143;}_LL147: {
const char*_tmpB5D;void*_tmpB5C;(_tmpB5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpB5D="field name designators in array",_tag_dyneither(_tmpB5D,sizeof(char),32))),_tag_dyneither(_tmpB5C,sizeof(void*),0)));}_LL143:;};}{
# 1557
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,e_index,0);
void*_tmp33F=_tmp335->r;void*_tmp340=_tmp33F;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Vardecl*_tmp344;struct Cyc_Absyn_Exp*_tmp345;struct Cyc_Absyn_Exp*_tmp346;int _tmp347;void*_tmp349;struct Cyc_List_List*_tmp34A;_LL149: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp341=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp340;if(_tmp341->tag != 25)goto _LL14B;else{_tmp342=_tmp341->f1;}}_LL14A:
# 1560
 s=Cyc_Toc_init_array(nv,orig_elt_type,lval,_tmp342,s);
goto _LL148;_LL14B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp343=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp340;if(_tmp343->tag != 26)goto _LL14D;else{_tmp344=_tmp343->f1;_tmp345=_tmp343->f2;_tmp346=_tmp343->f3;_tmp347=_tmp343->f4;}}_LL14C:
# 1563
 s=Cyc_Toc_init_comprehension(nv,orig_elt_type,lval,_tmp344,_tmp345,_tmp346,_tmp347,s,0);
# 1565
goto _LL148;_LL14D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp348=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp340;if(_tmp348->tag != 29)goto _LL14F;else{_tmp349=(void*)_tmp348->f1;_tmp34A=_tmp348->f2;}}_LL14E:
# 1567
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp349,_tmp34A,s);
goto _LL148;_LL14F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp34B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp340;if(_tmp34B->tag != 27)goto _LL151;}_LL150:
# 1570
 goto _LL148;_LL151:;_LL152:
# 1572
 Cyc_Toc_exp_to_c(nv,_tmp335);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(lhs,e_index,0),
Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(orig_elt_type),_tmp335),0),s,0);
goto _LL148;_LL148:;};};}}
# 1578
return s;}
# 1583
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_comprehension(struct Cyc_Toc_Env*nv,void*comprehension_type,struct Cyc_Absyn_Exp*lhs,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int zero_term,struct Cyc_Absyn_Stmt*s,int e1_already_translated){
# 1588
struct _tuple1*_tmp34C=vd->name;
void*expected_elt_type;
{void*_tmp34D=Cyc_Tcutil_compress(comprehension_type);void*_tmp34E=_tmp34D;void*_tmp350;void*_tmp352;_LL154: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp34F=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34E;if(_tmp34F->tag != 8)goto _LL156;else{_tmp350=(_tmp34F->f1).elt_type;}}_LL155:
# 1592
 expected_elt_type=_tmp350;goto _LL153;_LL156: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp351=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34E;if(_tmp351->tag != 5)goto _LL158;else{_tmp352=(_tmp351->f1).elt_typ;}}_LL157:
# 1594
 expected_elt_type=_tmp352;goto _LL153;_LL158:;_LL159: {
const char*_tmpB60;void*_tmpB5F;(_tmpB5F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB60="init_comprehension passed a bad type!",_tag_dyneither(_tmpB60,sizeof(char),38))),_tag_dyneither(_tmpB5F,sizeof(void*),0)));}_LL153:;}{
# 1597
void*_tmp355=Cyc_Toc_typ_to_c((void*)_check_null(e2->topt));
if(!e1_already_translated)
Cyc_Toc_exp_to_c(nv,e1);
# 1601
{void*_tmp356=e2->r;void*_tmp357=_tmp356;_LL15B: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp358=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp357;if(_tmp358->tag != 27)goto _LL15D;}_LL15C:
 return Cyc_Absyn_skip_stmt(0);_LL15D:;_LL15E:
 goto _LL15A;_LL15A:;}{
# 1605
struct _RegionHandle _tmp359=_new_region("r2");struct _RegionHandle*r2=& _tmp359;_push_region(r2);
{struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpB63;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpB62;struct Cyc_Toc_Env*nv2=Cyc_Toc_add_varmap(r2,nv,_tmp34C,Cyc_Absyn_varb_exp(_tmp34C,(void*)((_tmpB62=_cycalloc(sizeof(*_tmpB62)),((_tmpB62[0]=((_tmpB63.tag=4,((_tmpB63.f1=vd,_tmpB63)))),_tmpB62)))),0));
struct _tuple1*max=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*ea=Cyc_Absyn_assign_exp(Cyc_Absyn_var_exp(_tmp34C,0),Cyc_Absyn_signed_int_exp(0,0),0);
struct Cyc_Absyn_Exp*eb=Cyc_Absyn_lt_exp(Cyc_Absyn_var_exp(_tmp34C,0),Cyc_Absyn_var_exp(max,0),0);
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_post_inc_exp(Cyc_Absyn_var_exp(_tmp34C,0),0);
# 1613
struct Cyc_Absyn_Exp*lval=Cyc_Absyn_subscript_exp(lhs,Cyc_Absyn_var_exp(_tmp34C,0),0);
struct Cyc_Absyn_Stmt*body;
# 1617
{void*_tmp35A=e2->r;void*_tmp35B=_tmp35A;struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Vardecl*_tmp35F;struct Cyc_Absyn_Exp*_tmp360;struct Cyc_Absyn_Exp*_tmp361;int _tmp362;void*_tmp364;struct Cyc_List_List*_tmp365;_LL160: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp35C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp35B;if(_tmp35C->tag != 25)goto _LL162;else{_tmp35D=_tmp35C->f1;}}_LL161:
# 1619
 body=Cyc_Toc_init_array(nv2,expected_elt_type,lval,_tmp35D,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL162: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp35E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp35B;if(_tmp35E->tag != 26)goto _LL164;else{_tmp35F=_tmp35E->f1;_tmp360=_tmp35E->f2;_tmp361=_tmp35E->f3;_tmp362=_tmp35E->f4;}}_LL163:
# 1622
 body=Cyc_Toc_init_comprehension(nv2,expected_elt_type,lval,_tmp35F,_tmp360,_tmp361,_tmp362,Cyc_Toc_skip_stmt_dl(),0);
goto _LL15F;_LL164: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp363=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp35B;if(_tmp363->tag != 29)goto _LL166;else{_tmp364=(void*)_tmp363->f1;_tmp365=_tmp363->f2;}}_LL165:
# 1625
 body=Cyc_Toc_init_anon_struct(nv,lval,_tmp364,_tmp365,Cyc_Toc_skip_stmt_dl());
goto _LL15F;_LL166:;_LL167:
# 1628
 Cyc_Toc_exp_to_c(nv2,e2);
body=Cyc_Absyn_assign_stmt(lval,Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(expected_elt_type),e2),0);
goto _LL15F;_LL15F:;}{
# 1632
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_for_stmt(ea,eb,ec,body,0);
# 1634
if(zero_term){
# 1639
struct Cyc_Absyn_Exp*ex=Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_new_exp(lhs->r,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(_tmp355,Cyc_Absyn_uint_exp(0,0)),0);
s2=Cyc_Absyn_seq_stmt(s2,Cyc_Absyn_exp_stmt(ex,0),0);}{
# 1644
struct Cyc_Absyn_Stmt*_tmp366=Cyc_Absyn_seq_stmt(Cyc_Absyn_declare_stmt(max,Cyc_Absyn_uint_typ,e1,
Cyc_Absyn_declare_stmt(_tmp34C,Cyc_Absyn_uint_typ,0,s2,0),0),s,0);_npop_handler(0);return _tmp366;};};}
# 1606
;_pop_region(r2);};};}
# 1650
static struct Cyc_Absyn_Stmt*Cyc_Toc_init_anon_struct(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*lhs,void*struct_type,struct Cyc_List_List*dles,struct Cyc_Absyn_Stmt*s){
# 1653
{struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(dles);for(0;_tmp369 != 0;_tmp369=_tmp369->tl){
struct _tuple19*_tmp36A=(struct _tuple19*)_tmp369->hd;struct Cyc_List_List*_tmp36C;struct Cyc_Absyn_Exp*_tmp36D;struct _tuple19*_tmp36B=_tmp36A;_tmp36C=_tmp36B->f1;_tmp36D=_tmp36B->f2;
if(_tmp36C == 0){
const char*_tmpB66;void*_tmpB65;(_tmpB65=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB66="empty designator list",_tag_dyneither(_tmpB66,sizeof(char),22))),_tag_dyneither(_tmpB65,sizeof(void*),0)));}
if(_tmp36C->tl != 0){
const char*_tmpB69;void*_tmpB68;(_tmpB68=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB69="too many designators in anonymous struct",_tag_dyneither(_tmpB69,sizeof(char),41))),_tag_dyneither(_tmpB68,sizeof(void*),0)));}{
void*_tmp372=(void*)_tmp36C->hd;void*_tmp373=_tmp372;struct _dyneither_ptr*_tmp375;_LL169: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp374=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp373;if(_tmp374->tag != 1)goto _LL16B;else{_tmp375=_tmp374->f1;}}_LL16A: {
# 1661
struct Cyc_Absyn_Exp*lval=Cyc_Toc_member_exp(lhs,_tmp375,0);
{void*_tmp376=_tmp36D->r;void*_tmp377=_tmp376;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Vardecl*_tmp37B;struct Cyc_Absyn_Exp*_tmp37C;struct Cyc_Absyn_Exp*_tmp37D;int _tmp37E;void*_tmp380;struct Cyc_List_List*_tmp381;_LL16E: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp378=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp378->tag != 25)goto _LL170;else{_tmp379=_tmp378->f1;}}_LL16F:
# 1664
 s=Cyc_Toc_init_array(nv,(void*)_check_null(_tmp36D->topt),lval,_tmp379,s);goto _LL16D;_LL170: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp37A=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp37A->tag != 26)goto _LL172;else{_tmp37B=_tmp37A->f1;_tmp37C=_tmp37A->f2;_tmp37D=_tmp37A->f3;_tmp37E=_tmp37A->f4;}}_LL171:
# 1666
 s=Cyc_Toc_init_comprehension(nv,(void*)_check_null(_tmp36D->topt),lval,_tmp37B,_tmp37C,_tmp37D,_tmp37E,s,0);
goto _LL16D;_LL172: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp37F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp37F->tag != 29)goto _LL174;else{_tmp380=(void*)_tmp37F->f1;_tmp381=_tmp37F->f2;}}_LL173:
# 1669
 s=Cyc_Toc_init_anon_struct(nv,lval,_tmp380,_tmp381,s);goto _LL16D;_LL174: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp382=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp377;if(_tmp382->tag != 27)goto _LL176;}_LL175:
# 1671
 goto _LL16D;_LL176:;_LL177:
# 1673
 Cyc_Toc_exp_to_c(nv,_tmp36D);
# 1675
if(Cyc_Toc_is_poly_field(struct_type,_tmp375))
_tmp36D=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp36D);
# 1678
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp36D,0),0),s,0);
goto _LL16D;_LL16D:;}
# 1681
goto _LL168;}_LL16B:;_LL16C: {
const char*_tmpB6C;void*_tmpB6B;(_tmpB6B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB6C="array designator in struct",_tag_dyneither(_tmpB6C,sizeof(char),27))),_tag_dyneither(_tmpB6B,sizeof(void*),0)));}_LL168:;};}}
# 1685
return s;}
# 1690
static struct Cyc_Absyn_Exp*Cyc_Toc_init_tuple(struct Cyc_Toc_Env*nv,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*es){
# 1694
struct _RegionHandle _tmp385=_new_region("r");struct _RegionHandle*r=& _tmp385;_push_region(r);
{struct Cyc_List_List*_tmp386=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_rmap)(r,Cyc_Toc_tup_to_c,es);
void*_tmp387=Cyc_Toc_add_tuple_type(_tmp386);
# 1698
struct _tuple1*_tmp388=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp389=Cyc_Absyn_var_exp(_tmp388,0);
struct Cyc_Absyn_Stmt*_tmp38A=Cyc_Absyn_exp_stmt(_tmp389,0);
# 1702
struct Cyc_Absyn_Exp*(*_tmp38B)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
# 1704
int is_atomic=1;
struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,es);
{int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38C);for(0;_tmp38C != 0;(_tmp38C=_tmp38C->tl,-- i)){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp38C->hd;
struct Cyc_Absyn_Exp*lval=_tmp38B(_tmp389,Cyc_Absyn_fieldname(i),0);
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(e->topt));{
void*_tmp38D=e->r;void*_tmp38E=_tmp38D;struct Cyc_List_List*_tmp390;struct Cyc_Absyn_Vardecl*_tmp392;struct Cyc_Absyn_Exp*_tmp393;struct Cyc_Absyn_Exp*_tmp394;int _tmp395;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp38F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp38E;if(_tmp38F->tag != 25)goto _LL17B;else{_tmp390=_tmp38F->f1;}}_LL17A:
# 1712
 _tmp38A=Cyc_Toc_init_array(nv,(void*)_check_null(e->topt),lval,_tmp390,_tmp38A);
goto _LL178;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp391=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp38E;if(_tmp391->tag != 26)goto _LL17D;else{_tmp392=_tmp391->f1;_tmp393=_tmp391->f2;_tmp394=_tmp391->f3;_tmp395=_tmp391->f4;}}_LL17C:
# 1715
 _tmp38A=Cyc_Toc_init_comprehension(nv,(void*)_check_null(e->topt),lval,_tmp392,_tmp393,_tmp394,_tmp395,_tmp38A,0);
# 1717
goto _LL178;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp396=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp38E;if(_tmp396->tag != 27)goto _LL17F;}_LL17E:
# 1719
 goto _LL178;_LL17F:;_LL180:
# 1722
 Cyc_Toc_exp_to_c(nv,e);
_tmp38A=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp38B(_tmp389,Cyc_Absyn_fieldname(i),0),e,0),0),_tmp38A,0);
# 1725
goto _LL178;_LL178:;};}}{
# 1728
struct Cyc_Absyn_Exp*_tmp397=Cyc_Toc_make_struct(nv,Cyc_Absyn_var_exp(_tmp388,0),_tmp387,_tmp38A,pointer,rgnopt,is_atomic,1);_npop_handler(0);return _tmp397;};}
# 1695
;_pop_region(r);}
# 1732
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp398=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp398 != 0;_tmp398=_tmp398->tl){
struct Cyc_Absyn_Aggrfield*_tmp399=(struct Cyc_Absyn_Aggrfield*)_tmp398->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp399->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}{
# 1739
struct Cyc_String_pa_PrintArg_struct _tmpB74;void*_tmpB73[1];const char*_tmpB72;void*_tmpB71;(_tmpB71=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)((struct _dyneither_ptr)((_tmpB74.tag=0,((_tmpB74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpB73[0]=& _tmpB74,Cyc_aprintf(((_tmpB72="get_member_offset %s failed",_tag_dyneither(_tmpB72,sizeof(char),28))),_tag_dyneither(_tmpB73,sizeof(void*),1)))))))),_tag_dyneither(_tmpB71,sizeof(void*),0)));};}
# 1742
static int Cyc_Toc_init_variable_array(void**typ){
{void*_tmp39E=Cyc_Tcutil_compress(*typ);void*_tmp39F=_tmp39E;void*_tmp3A1;struct Cyc_Absyn_Tqual _tmp3A2;struct Cyc_Absyn_Exp*_tmp3A3;union Cyc_Absyn_Constraint*_tmp3A4;unsigned int _tmp3A5;_LL182: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp3A0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39F;if(_tmp3A0->tag != 8)goto _LL184;else{_tmp3A1=(_tmp3A0->f1).elt_type;_tmp3A2=(_tmp3A0->f1).tq;_tmp3A3=(_tmp3A0->f1).num_elts;_tmp3A4=(_tmp3A0->f1).zero_term;_tmp3A5=(_tmp3A0->f1).zt_loc;}}_LL183:
# 1747
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp3A3))){
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB7A;struct Cyc_Absyn_ArrayInfo _tmpB79;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpB78;*typ=(void*)((_tmpB78=_cycalloc(sizeof(*_tmpB78)),((_tmpB78[0]=((_tmpB7A.tag=8,((_tmpB7A.f1=((_tmpB79.elt_type=_tmp3A1,((_tmpB79.tq=_tmp3A2,((_tmpB79.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpB79.zero_term=_tmp3A4,((_tmpB79.zt_loc=_tmp3A5,_tmpB79)))))))))),_tmpB7A)))),_tmpB78))));}
return 1;}
# 1751
goto _LL181;_LL184:;_LL185:
 goto _LL181;_LL181:;}
# 1754
return 0;}
# 1757
struct Cyc_Absyn_Aggrdecl*Cyc_Toc_update_aggr_type(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*fn,void*new_field_type){
# 1759
struct Cyc_List_List*new_fields=0;
struct Cyc_List_List*fields;
for(fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;fields != 0;fields=fields->tl){
# 1763
if(!Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)fields->hd)->name,fn)){
struct Cyc_Absyn_Aggrfield*_tmp3A9=(struct Cyc_Absyn_Aggrfield*)fields->hd;
struct Cyc_Absyn_Aggrfield*_tmpB7B;struct Cyc_Absyn_Aggrfield*_tmp3AA=(_tmpB7B=_cycalloc(sizeof(*_tmpB7B)),((_tmpB7B->name=_tmp3A9->name,((_tmpB7B->tq=Cyc_Toc_mt_tq,((_tmpB7B->type=new_field_type,((_tmpB7B->width=_tmp3A9->width,((_tmpB7B->attributes=_tmp3A9->attributes,((_tmpB7B->requires_clause=0,_tmpB7B)))))))))))));
# 1771
struct Cyc_List_List*_tmpB7C;new_fields=((_tmpB7C=_cycalloc(sizeof(*_tmpB7C)),((_tmpB7C->hd=_tmp3AA,((_tmpB7C->tl=new_fields,_tmpB7C))))));}else{
# 1773
struct Cyc_List_List*_tmpB7D;new_fields=((_tmpB7D=_cycalloc(sizeof(*_tmpB7D)),((_tmpB7D->hd=(struct Cyc_Absyn_Aggrfield*)fields->hd,((_tmpB7D->tl=new_fields,_tmpB7D))))));}}{
# 1775
struct Cyc_List_List*_tmp3AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);
struct Cyc_Int_pa_PrintArg_struct _tmpB85;void*_tmpB84[1];const char*_tmpB83;struct _dyneither_ptr*_tmpB82;struct _dyneither_ptr*name=
(_tmpB82=_cycalloc(sizeof(*_tmpB82)),((_tmpB82[0]=(struct _dyneither_ptr)((_tmpB85.tag=1,((_tmpB85.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB84[0]=& _tmpB85,Cyc_aprintf(((_tmpB83="_genStruct%d",_tag_dyneither(_tmpB83,sizeof(char),13))),_tag_dyneither(_tmpB84,sizeof(void*),1)))))))),_tmpB82)));
struct _tuple1*_tmpB86;struct _tuple1*qv=(_tmpB86=_cycalloc(sizeof(*_tmpB86)),((_tmpB86->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB86->f2=name,_tmpB86)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB87;struct Cyc_Absyn_AggrdeclImpl*_tmp3AF=(_tmpB87=_cycalloc(sizeof(*_tmpB87)),((_tmpB87->exist_vars=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmpB87->rgn_po=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po,((_tmpB87->fields=_tmp3AE,((_tmpB87->tagged=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged,_tmpB87)))))))));
# 1783
struct Cyc_Absyn_Aggrdecl*_tmpB88;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB88=_cycalloc(sizeof(*_tmpB88)),((_tmpB88->kind=ad->kind,((_tmpB88->sc=ad->sc,((_tmpB88->name=qv,((_tmpB88->tvs=ad->tvs,((_tmpB88->impl=_tmp3AF,((_tmpB88->attributes=ad->attributes,_tmpB88)))))))))))));
# 1789
Cyc_Toc_aggrdecl_to_c(new_ad,1);
return new_ad;};}
# 1793
static struct Cyc_Absyn_Exp*Cyc_Toc_init_struct(struct Cyc_Toc_Env*nv,void*struct_type,struct Cyc_List_List*exist_types,int pointer,struct Cyc_Absyn_Exp*rgnopt,struct Cyc_List_List*dles,struct _tuple1*tdn){
# 1799
int do_declare=0;
struct Cyc_Absyn_Exp*xexp;
if((nv->struct_info).lhs_exp == 0){
struct _tuple1*_tmp3B7=Cyc_Toc_temp_var();
xexp=Cyc_Absyn_var_exp(_tmp3B7,0);
do_declare=1;}else{
# 1807
xexp=(struct Cyc_Absyn_Exp*)_check_null((nv->struct_info).lhs_exp);
nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);}{
# 1810
struct Cyc_Absyn_Stmt*_tmp3B8=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(xexp),0);
# 1814
struct Cyc_Absyn_Exp**varexp;
{void*_tmp3B9=_tmp3B8->r;void*_tmp3BA=_tmp3B9;struct Cyc_Absyn_Exp**_tmp3BC;_LL187: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3BB=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3BA;if(_tmp3BB->tag != 1)goto _LL189;else{_tmp3BC=(struct Cyc_Absyn_Exp**)& _tmp3BB->f1;}}_LL188:
 varexp=_tmp3BC;goto _LL186;_LL189:;_LL18A: {
const char*_tmpB8B;void*_tmpB8A;(_tmpB8A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8B="stmt not an expression!",_tag_dyneither(_tmpB8B,sizeof(char),24))),_tag_dyneither(_tmpB8A,sizeof(void*),0)));}_LL186:;}{
# 1820
struct Cyc_Absyn_Exp*(*_tmp3BF)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)=pointer?Cyc_Absyn_aggrarrow_exp: Cyc_Toc_member_exp;
void*aggr_typ=Cyc_Toc_typ_to_c(struct_type);
void*orig_typ=aggr_typ;
# 1824
int is_atomic=1;
struct Cyc_List_List*forall_types;
struct Cyc_Absyn_Aggrdecl*ad;
struct Cyc_List_List*aggrfields;struct Cyc_List_List*orig_aggrfields;
struct _dyneither_ptr**index=0;
{void*_tmp3C0=Cyc_Tcutil_compress(struct_type);void*_tmp3C1=_tmp3C0;union Cyc_Absyn_AggrInfoU _tmp3C3;struct Cyc_List_List*_tmp3C4;_LL18C: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3C2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C1;if(_tmp3C2->tag != 11)goto _LL18E;else{_tmp3C3=(_tmp3C2->f1).aggr_info;_tmp3C4=(_tmp3C2->f1).targs;}}_LL18D:
# 1831
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp3C3);
orig_aggrfields=(aggrfields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
forall_types=_tmp3C4;
goto _LL18B;_LL18E:;_LL18F: {
const char*_tmpB8E;void*_tmpB8D;(_tmpB8D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpB8E="init_struct: bad struct type",_tag_dyneither(_tmpB8E,sizeof(char),29))),_tag_dyneither(_tmpB8D,sizeof(void*),0)));}_LL18B:;}
# 1838
if(exist_types != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields != 0){
struct Cyc_List_List*_tmp3C7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;
for(0;_tmp3C7->tl != 0;_tmp3C7=_tmp3C7->tl){;}{
struct Cyc_Absyn_Aggrfield*_tmp3C8=(struct Cyc_Absyn_Aggrfield*)_tmp3C7->hd;
struct _RegionHandle _tmp3C9=_new_region("temp");struct _RegionHandle*temp=& _tmp3C9;_push_region(temp);
{struct Cyc_List_List*_tmp3CA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,ad->tvs,forall_types);
# 1847
if(Cyc_Toc_is_abstract_type(Cyc_Tcutil_rsubstitute(temp,_tmp3CA,_tmp3C8->type))){
struct Cyc_List_List*_tmp3CB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(temp,temp,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,exist_types);
struct Cyc_List_List*_tmp3CC=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(temp,_tmp3CA,_tmp3CB);
# 1851
struct Cyc_List_List*new_fields=0;
for(_tmp3C7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;_tmp3C7 != 0;_tmp3C7=_tmp3C7->tl){
struct Cyc_Absyn_Aggrfield*_tmp3CD=(struct Cyc_Absyn_Aggrfield*)_tmp3C7->hd;
struct Cyc_Absyn_Aggrfield*_tmpB8F;struct Cyc_Absyn_Aggrfield*_tmp3CE=(_tmpB8F=_cycalloc(sizeof(*_tmpB8F)),((_tmpB8F->name=_tmp3CD->name,((_tmpB8F->tq=Cyc_Toc_mt_tq,((_tmpB8F->type=
# 1856
Cyc_Tcutil_rsubstitute(temp,_tmp3CC,_tmp3CD->type),((_tmpB8F->width=_tmp3CD->width,((_tmpB8F->attributes=_tmp3CD->attributes,((_tmpB8F->requires_clause=0,_tmpB8F)))))))))))));
# 1864
if(_tmp3C7->tl == 0){
if(Cyc_Toc_init_variable_array(& _tmp3CE->type)){
struct _dyneither_ptr**_tmpB90;index=((_tmpB90=_cycalloc(sizeof(*_tmpB90)),((_tmpB90[0]=_tmp3CE->name,_tmpB90))));}}{
# 1871
struct Cyc_List_List*_tmpB91;new_fields=((_tmpB91=_cycalloc(sizeof(*_tmpB91)),((_tmpB91->hd=_tmp3CE,((_tmpB91->tl=new_fields,_tmpB91))))));};}
# 1873
exist_types=0;
aggrfields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(new_fields);{
struct Cyc_Int_pa_PrintArg_struct _tmpB99;void*_tmpB98[1];const char*_tmpB97;struct _dyneither_ptr*_tmpB96;struct _dyneither_ptr*name=
(_tmpB96=_cycalloc(sizeof(*_tmpB96)),((_tmpB96[0]=(struct _dyneither_ptr)((_tmpB99.tag=1,((_tmpB99.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++,((_tmpB98[0]=& _tmpB99,Cyc_aprintf(((_tmpB97="_genStruct%d",_tag_dyneither(_tmpB97,sizeof(char),13))),_tag_dyneither(_tmpB98,sizeof(void*),1)))))))),_tmpB96)));
struct _tuple1*_tmpB9A;struct _tuple1*qv=(_tmpB9A=_cycalloc(sizeof(*_tmpB9A)),((_tmpB9A->f1=Cyc_Absyn_Abs_n(0,1),((_tmpB9A->f2=name,_tmpB9A)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpB9B;struct Cyc_Absyn_AggrdeclImpl*_tmp3D2=(_tmpB9B=_cycalloc(sizeof(*_tmpB9B)),((_tmpB9B->exist_vars=0,((_tmpB9B->rgn_po=0,((_tmpB9B->fields=aggrfields,((_tmpB9B->tagged=0,_tmpB9B)))))))));
# 1882
struct Cyc_Absyn_Aggrdecl*_tmpB9C;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpB9C=_cycalloc(sizeof(*_tmpB9C)),((_tmpB9C->kind=Cyc_Absyn_StructA,((_tmpB9C->sc=Cyc_Absyn_Public,((_tmpB9C->name=qv,((_tmpB9C->tvs=0,((_tmpB9C->impl=_tmp3D2,((_tmpB9C->attributes=0,_tmpB9C)))))))))))));
# 1888
Cyc_Toc_aggrdecl_to_c(new_ad,1);
ad=new_ad;
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBA6;struct Cyc_Absyn_Aggrdecl**_tmpBA5;struct Cyc_Absyn_AggrInfo _tmpBA4;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBA3;struct_type=(void*)((_tmpBA3=_cycalloc(sizeof(*_tmpBA3)),((_tmpBA3[0]=((_tmpBA6.tag=11,((_tmpBA6.f1=((_tmpBA4.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBA5=_cycalloc(sizeof(*_tmpBA5)),((_tmpBA5[0]=new_ad,_tmpBA5))))),((_tmpBA4.targs=0,_tmpBA4)))),_tmpBA6)))),_tmpBA3))));}
# 1892
aggr_typ=Cyc_Toc_typ_to_c(struct_type);};}}
# 1843
;_pop_region(temp);};}{
# 1896
int is_tagged_union=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged;
struct _RegionHandle _tmp3DE=_new_region("r");struct _RegionHandle*r=& _tmp3DE;_push_region(r);
{struct Cyc_List_List*_tmp3DF=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rrev)(r,dles);for(0;_tmp3DF != 0;_tmp3DF=_tmp3DF->tl){
struct _tuple19*_tmp3E0=(struct _tuple19*)_tmp3DF->hd;struct Cyc_List_List*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E3;struct _tuple19*_tmp3E1=_tmp3E0;_tmp3E2=_tmp3E1->f1;_tmp3E3=_tmp3E1->f2;
is_atomic=is_atomic  && Cyc_Toc_atomic_typ((void*)_check_null(_tmp3E3->topt));
if(_tmp3E2 == 0){
const char*_tmpBA9;void*_tmpBA8;(_tmpBA8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBA9="empty designator list",_tag_dyneither(_tmpBA9,sizeof(char),22))),_tag_dyneither(_tmpBA8,sizeof(void*),0)));}
if(_tmp3E2->tl != 0){
# 1906
struct _tuple1*_tmp3E6=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3E7=Cyc_Absyn_var_exp(_tmp3E6,0);
for(0;_tmp3E2 != 0;_tmp3E2=_tmp3E2->tl){
void*_tmp3E8=(void*)_tmp3E2->hd;void*_tmp3E9=_tmp3E8;struct _dyneither_ptr*_tmp3EB;_LL191: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3EA=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3E9;if(_tmp3EA->tag != 1)goto _LL193;else{_tmp3EB=_tmp3EA->f1;}}_LL192:
# 1912
 if(Cyc_Toc_is_poly_field(struct_type,_tmp3EB))
_tmp3E7=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3E7);
_tmp3B8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3BF(xexp,_tmp3EB,0),_tmp3E7,0),0),_tmp3B8,0);
# 1916
goto _LL190;_LL193:;_LL194: {
const char*_tmpBAC;void*_tmpBAB;(_tmpBAB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBAC="array designator in struct",_tag_dyneither(_tmpBAC,sizeof(char),27))),_tag_dyneither(_tmpBAB,sizeof(void*),0)));}_LL190:;}
# 1920
Cyc_Toc_exp_to_c(nv,_tmp3E3);
_tmp3B8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp3E7,_tmp3E3,0),0),_tmp3B8,0);}else{
# 1923
void*_tmp3EE=(void*)_tmp3E2->hd;void*_tmp3EF=_tmp3EE;struct _dyneither_ptr*_tmp3F1;_LL196: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp3F0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp3EF;if(_tmp3F0->tag != 1)goto _LL198;else{_tmp3F1=_tmp3F0->f1;}}_LL197: {
# 1925
struct Cyc_Absyn_Exp*lval=_tmp3BF(xexp,_tmp3F1,0);
if(is_tagged_union){
int i=Cyc_Toc_get_member_offset(ad,_tmp3F1);
struct Cyc_Absyn_Exp*f_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct Cyc_Absyn_Exp*lhs=Cyc_Toc_member_exp(lval,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*assn_exp=Cyc_Absyn_assign_exp(lhs,f_tag_exp,0);
_tmp3B8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(assn_exp,0),_tmp3B8,0);
lval=Cyc_Toc_member_exp(lval,Cyc_Toc_val_sp,0);}{
# 1934
int e1_translated=0;
{void*_tmp3F2=_tmp3E3->r;void*_tmp3F3=_tmp3F2;struct Cyc_List_List*_tmp3F5;struct Cyc_Absyn_Vardecl*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3F9;int _tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FD;void*_tmp3FF;struct Cyc_List_List*_tmp400;_LL19B: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp3F4=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3F3;if(_tmp3F4->tag != 25)goto _LL19D;else{_tmp3F5=_tmp3F4->f1;}}_LL19C:
# 1937
 _tmp3B8=Cyc_Toc_init_array(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(aggrfields,_tmp3F1)))->type,lval,_tmp3F5,_tmp3B8);
# 1939
goto _LL19A;_LL19D: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp3F6=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3F3;if(_tmp3F6->tag != 26)goto _LL19F;else{_tmp3F7=_tmp3F6->f1;_tmp3F8=_tmp3F6->f2;_tmp3F9=_tmp3F6->f3;_tmp3FA=_tmp3F6->f4;}}_LL19E:
# 1941
 _tmp3B8=Cyc_Toc_init_comprehension(nv,((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(do_declare?aggrfields: orig_aggrfields,_tmp3F1)))->type,lval,_tmp3F7,_tmp3F8,_tmp3F9,_tmp3FA,_tmp3B8,0);
# 1945
e1_translated=1;
_tmp3FC=_tmp3F8;_tmp3FD=(void*)_check_null(_tmp3F9->topt);goto _LL1A0;_LL19F: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp3FB=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3F3;if(_tmp3FB->tag != 27)goto _LL1A1;else{_tmp3FC=_tmp3FB->f1;_tmp3FD=(void*)_tmp3FB->f2;}}_LL1A0:
# 1950
 if(index != 0  && !Cyc_strptrcmp(*index,_tmp3F1)){
if(!e1_translated)Cyc_Toc_exp_to_c(nv,_tmp3FC);{
void*_tmp401=Cyc_Toc_typ_to_c(_tmp3FD);
struct _tuple6 _tmpBB0;union Cyc_Absyn_Cnst _tmpBAF;struct Cyc_Absyn_Exp*_tmpBAD[2];*(nv->struct_info).varsizeexp=
Cyc_Absyn_add_exp(Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpBAD[1]=Cyc_Absyn_sizeoftyp_exp(_tmp401,0),((_tmpBAD[0]=_tmp3FC,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBAD,sizeof(struct Cyc_Absyn_Exp*),2)))))),0),
Cyc_Absyn_const_exp((((_tmpBAF.Int_c).val=((_tmpBB0.f1=Cyc_Absyn_Unsigned,((_tmpBB0.f2=(int)sizeof(double),_tmpBB0)))),(((_tmpBAF.Int_c).tag=5,_tmpBAF)))),0),0);};}
# 1958
goto _LL19A;_LL1A1: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp3FE=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp3F3;if(_tmp3FE->tag != 29)goto _LL1A3;else{_tmp3FF=(void*)_tmp3FE->f1;_tmp400=_tmp3FE->f2;}}_LL1A2:
# 1960
 _tmp3B8=Cyc_Toc_init_anon_struct(nv,lval,_tmp3FF,_tmp400,_tmp3B8);goto _LL19A;_LL1A3:;_LL1A4: {
# 1962
void*old_e_typ=(void*)_check_null(_tmp3E3->topt);
int was_ptr_type=Cyc_Toc_is_pointer_or_boxed_tvar(old_e_typ);
Cyc_Toc_exp_to_c(Cyc_Toc_set_lhs_exp(nv->rgn,lval,nv),_tmp3E3);
{void*_tmp405=old_e_typ;_LL1A6: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp406=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp405;if(_tmp406->tag != 11)goto _LL1A8;}_LL1A7:
# 1969
 if(old_e_typ != _tmp3E3->topt  && !Cyc_Tcutil_unify(Cyc_Toc_typ_to_c(old_e_typ),(void*)_check_null(_tmp3E3->topt))){
# 1973
ad=Cyc_Toc_update_aggr_type(ad,_tmp3F1,(void*)_check_null(_tmp3E3->topt));
{struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpBBA;struct Cyc_Absyn_Aggrdecl**_tmpBB9;struct Cyc_Absyn_AggrInfo _tmpBB8;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpBB7;struct_type=(void*)(
(_tmpBB7=_cycalloc(sizeof(*_tmpBB7)),((_tmpBB7[0]=((_tmpBBA.tag=11,((_tmpBBA.f1=((_tmpBB8.aggr_info=Cyc_Absyn_KnownAggr(((_tmpBB9=_cycalloc(sizeof(*_tmpBB9)),((_tmpBB9[0]=ad,_tmpBB9))))),((_tmpBB8.targs=0,_tmpBB8)))),_tmpBBA)))),_tmpBB7))));}
# 1977
aggr_typ=Cyc_Toc_typ_to_c(struct_type);}
# 1979
goto _LL1A5;_LL1A8:;_LL1A9:
 goto _LL1A5;_LL1A5:;}{
# 1982
struct Cyc_Absyn_Aggrfield*_tmp40B=Cyc_Absyn_lookup_field(aggrfields,_tmp3F1);
# 1984
if(Cyc_Toc_is_poly_field(struct_type,_tmp3F1) && !was_ptr_type)
_tmp3E3=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp3E3);
# 1987
if(exist_types != 0)
_tmp3E3=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_check_null(_tmp40B))->type),_tmp3E3);
# 1990
_tmp3B8=Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(lval,_tmp3E3,0),0),_tmp3B8,0);
goto _LL19A;};}_LL19A:;}
# 1993
goto _LL195;};}_LL198:;_LL199: {
const char*_tmpBBD;void*_tmpBBC;(_tmpBBC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBBD="array designator in struct",_tag_dyneither(_tmpBBD,sizeof(char),27))),_tag_dyneither(_tmpBBC,sizeof(void*),0)));}_LL195:;}}}
# 2000
if(aggr_typ != orig_typ  && pointer){
void*cast_type=Cyc_Absyn_cstar_typ(orig_typ,Cyc_Toc_mt_tq);
*varexp=Cyc_Toc_cast_it(cast_type,*varexp);}{
# 2005
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Toc_make_struct(nv,xexp,aggr_typ,_tmp3B8,pointer,rgnopt,is_atomic,do_declare);
# 2007
_tmp40E->topt=struct_type;{
struct Cyc_Absyn_Exp*_tmp40F=_tmp40E;_npop_handler(0);return _tmp40F;};};
# 1898
;_pop_region(r);};};};}struct _tuple27{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 2013
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple27*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0);}
# 2016
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0);}
# 2019
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmpBC0;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmpBBF;return Cyc_Absyn_new_exp((void*)((_tmpBBF=_cycalloc(sizeof(*_tmpBBF)),((_tmpBBF[0]=((_tmpBC0.tag=4,((_tmpBC0.f1=e1,((_tmpBC0.f2=incr,_tmpBC0)))))),_tmpBBF)))),0);}
# 2023
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 2032
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2034
void*_tmp412=e1->r;void*_tmp413=_tmp412;struct Cyc_Absyn_Stmt*_tmp415;void*_tmp417;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp41A;struct _dyneither_ptr*_tmp41B;int _tmp41C;int _tmp41D;_LL1AB: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp414=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp414->tag != 36)goto _LL1AD;else{_tmp415=_tmp414->f1;}}_LL1AC:
 Cyc_Toc_lvalue_assign_stmt(_tmp415,fs,f,f_env);goto _LL1AA;_LL1AD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp416=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp416->tag != 13)goto _LL1AF;else{_tmp417=(void*)_tmp416->f1;_tmp418=_tmp416->f2;}}_LL1AE:
 Cyc_Toc_lvalue_assign(_tmp418,fs,f,f_env);goto _LL1AA;_LL1AF: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp419=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp413;if(_tmp419->tag != 20)goto _LL1B1;else{_tmp41A=_tmp419->f1;_tmp41B=_tmp419->f2;_tmp41C=_tmp419->f3;_tmp41D=_tmp419->f4;}}_LL1B0:
# 2039
 e1->r=_tmp41A->r;
{struct Cyc_List_List*_tmpBC1;Cyc_Toc_lvalue_assign(e1,((_tmpBC1=_cycalloc(sizeof(*_tmpBC1)),((_tmpBC1->hd=_tmp41B,((_tmpBC1->tl=fs,_tmpBC1)))))),f,f_env);}
goto _LL1AA;_LL1B1:;_LL1B2: {
# 2047
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 2049
for(0;fs != 0;fs=fs->tl){
e1_copy=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);}
e1->r=(f(e1_copy,f_env))->r;
goto _LL1AA;}_LL1AA:;}
# 2055
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 2057
void*_tmp41F=s->r;void*_tmp420=_tmp41F;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Decl*_tmp424;struct Cyc_Absyn_Stmt*_tmp425;struct Cyc_Absyn_Stmt*_tmp427;_LL1B4: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp421=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp420;if(_tmp421->tag != 1)goto _LL1B6;else{_tmp422=_tmp421->f1;}}_LL1B5:
 Cyc_Toc_lvalue_assign(_tmp422,fs,f,f_env);goto _LL1B3;_LL1B6: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp423=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp420;if(_tmp423->tag != 12)goto _LL1B8;else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}_LL1B7:
# 2060
 Cyc_Toc_lvalue_assign_stmt(_tmp425,fs,f,f_env);goto _LL1B3;_LL1B8: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp426=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp420;if(_tmp426->tag != 2)goto _LL1BA;else{_tmp427=_tmp426->f2;}}_LL1B9:
 Cyc_Toc_lvalue_assign_stmt(_tmp427,fs,f,f_env);goto _LL1B3;_LL1BA:;_LL1BB: {
const char*_tmpBC5;void*_tmpBC4[1];struct Cyc_String_pa_PrintArg_struct _tmpBC3;(_tmpBC3.tag=0,((_tmpBC3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBC4[0]=& _tmpBC3,Cyc_Toc_toc_impos(((_tmpBC5="lvalue_assign_stmt: %s",_tag_dyneither(_tmpBC5,sizeof(char),23))),_tag_dyneither(_tmpBC4,sizeof(void*),1)))))));}_LL1B3:;}
# 2066
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 2070
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp42B=e->r;void*_tmp42C=_tmp42B;void**_tmp42E;struct Cyc_Absyn_Exp**_tmp42F;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Stmt*_tmp433;_LL1BD: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp42D=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp42C;if(_tmp42D->tag != 13)goto _LL1BF;else{_tmp42E=(void**)((void**)& _tmp42D->f1);_tmp42F=(struct Cyc_Absyn_Exp**)& _tmp42D->f2;}}_LL1BE:
# 2073
*_tmp42F=Cyc_Toc_push_address_exp(*_tmp42F);
*_tmp42E=Cyc_Absyn_cstar_typ(*_tmp42E,Cyc_Toc_mt_tq);
return e;_LL1BF: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp430=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp42C;if(_tmp430->tag != 19)goto _LL1C1;else{_tmp431=_tmp430->f1;}}_LL1C0:
# 2077
 return _tmp431;_LL1C1: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp432=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp42C;if(_tmp432->tag != 36)goto _LL1C3;else{_tmp433=_tmp432->f1;}}_LL1C2:
# 2081
 Cyc_Toc_push_address_stmt(_tmp433);
return e;_LL1C3:;_LL1C4:
# 2084
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0);{
const char*_tmpBC9;void*_tmpBC8[1];struct Cyc_String_pa_PrintArg_struct _tmpBC7;(_tmpBC7.tag=0,((_tmpBC7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpBC8[0]=& _tmpBC7,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBC9="can't take & of exp %s",_tag_dyneither(_tmpBC9,sizeof(char),23))),_tag_dyneither(_tmpBC8,sizeof(void*),1)))))));};_LL1BC:;}
# 2089
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp437=s->r;void*_tmp438=_tmp437;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Stmt*_tmp43C;struct Cyc_Absyn_Exp**_tmp43E;_LL1C6: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp439=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp438;if(_tmp439->tag != 2)goto _LL1C8;else{_tmp43A=_tmp439->f2;}}_LL1C7:
 _tmp43C=_tmp43A;goto _LL1C9;_LL1C8: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp43B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp438;if(_tmp43B->tag != 12)goto _LL1CA;else{_tmp43C=_tmp43B->f2;}}_LL1C9:
 Cyc_Toc_push_address_stmt(_tmp43C);goto _LL1C5;_LL1CA: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp43D=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp438;if(_tmp43D->tag != 1)goto _LL1CC;else{_tmp43E=(struct Cyc_Absyn_Exp**)& _tmp43D->f1;}}_LL1CB:
*_tmp43E=Cyc_Toc_push_address_exp(*_tmp43E);goto _LL1C5;_LL1CC:;_LL1CD: {
# 2095
const char*_tmpBCD;void*_tmpBCC[1];struct Cyc_String_pa_PrintArg_struct _tmpBCB;(_tmpBCB.tag=0,((_tmpBCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s)),((_tmpBCC[0]=& _tmpBCB,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBCD="can't take & of stmt %s",_tag_dyneither(_tmpBCD,sizeof(char),24))),_tag_dyneither(_tmpBCC,sizeof(void*),1)))))));}_LL1C5:;}
# 2100
static struct Cyc_List_List*Cyc_Toc_rmap_2c(struct _RegionHandle*r2,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
# 2102
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 2104
if(x == 0)return 0;
{struct Cyc_List_List*_tmpBCE;result=((_tmpBCE=_region_malloc(r2,sizeof(*_tmpBCE)),((_tmpBCE->hd=f(x->hd,env),((_tmpBCE->tl=0,_tmpBCE))))));}
prev=result;
for(x=x->tl;x != 0;x=x->tl){
{struct Cyc_List_List*_tmpBCF;((struct Cyc_List_List*)_check_null(prev))->tl=((_tmpBCF=_region_malloc(r2,sizeof(*_tmpBCF)),((_tmpBCF->hd=f(x->hd,env),((_tmpBCF->tl=0,_tmpBCF))))));}
prev=prev->tl;}
# 2111
return result;}
# 2113
static struct Cyc_List_List*Cyc_Toc_map_2c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_Toc_rmap_2c(Cyc_Core_heap_region,f,env,x);}
# 2117
static struct _tuple19*Cyc_Toc_make_dle(struct Cyc_Absyn_Exp*e){
struct _tuple19*_tmpBD0;return(_tmpBD0=_cycalloc(sizeof(*_tmpBD0)),((_tmpBD0->f1=0,((_tmpBD0->f2=e,_tmpBD0)))));}
# 2121
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp445=Cyc_Tcutil_compress(t);void*_tmp446=_tmp445;struct Cyc_Absyn_PtrInfo _tmp448;_LL1CF: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp447=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp446;if(_tmp447->tag != 5)goto _LL1D1;else{_tmp448=_tmp447->f1;}}_LL1D0:
 return _tmp448;_LL1D1:;_LL1D2: {
const char*_tmpBD3;void*_tmpBD2;(_tmpBD2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBD3="get_ptr_typ: not a pointer!",_tag_dyneither(_tmpBD3,sizeof(char),28))),_tag_dyneither(_tmpBD2,sizeof(void*),0)));}_LL1CE:;}
# 2131
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp44B=Cyc_Tcutil_compress(t);void*_tmp44C=_tmp44B;enum Cyc_Absyn_Sign _tmp44F;enum Cyc_Absyn_Sign _tmp451;enum Cyc_Absyn_Sign _tmp455;enum Cyc_Absyn_Sign _tmp457;enum Cyc_Absyn_Sign _tmp459;int _tmp45D;_LL1D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp44D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44C;if(_tmp44D->tag != 5)goto _LL1D6;}_LL1D5:
 res=Cyc_Absyn_null_exp(0);goto _LL1D3;_LL1D6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp44E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44C;if(_tmp44E->tag != 6)goto _LL1D8;else{_tmp44F=_tmp44E->f1;if(_tmp44E->f2 != Cyc_Absyn_Char_sz)goto _LL1D8;}}_LL1D7:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp44F,'\000'),0);goto _LL1D3;_LL1D8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp450=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44C;if(_tmp450->tag != 6)goto _LL1DA;else{_tmp451=_tmp450->f1;if(_tmp450->f2 != Cyc_Absyn_Short_sz)goto _LL1DA;}}_LL1D9:
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp451,0),0);goto _LL1D3;_LL1DA: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp452=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp44C;if(_tmp452->tag != 13)goto _LL1DC;}_LL1DB:
 goto _LL1DD;_LL1DC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp453=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp44C;if(_tmp453->tag != 14)goto _LL1DE;}_LL1DD:
 _tmp455=Cyc_Absyn_Unsigned;goto _LL1DF;_LL1DE: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp454=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44C;if(_tmp454->tag != 6)goto _LL1E0;else{_tmp455=_tmp454->f1;if(_tmp454->f2 != Cyc_Absyn_Int_sz)goto _LL1E0;}}_LL1DF:
 _tmp457=_tmp455;goto _LL1E1;_LL1E0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp456=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44C;if(_tmp456->tag != 6)goto _LL1E2;else{_tmp457=_tmp456->f1;if(_tmp456->f2 != Cyc_Absyn_Long_sz)goto _LL1E2;}}_LL1E1:
# 2141
 res=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp457,0),0);goto _LL1D3;_LL1E2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp458=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp44C;if(_tmp458->tag != 6)goto _LL1E4;else{_tmp459=_tmp458->f1;if(_tmp458->f2 != Cyc_Absyn_LongLong_sz)goto _LL1E4;}}_LL1E3:
# 2143
 res=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp459,(long long)0),0);goto _LL1D3;_LL1E4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp45A=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp44C;if(_tmp45A->tag != 7)goto _LL1E6;else{if(_tmp45A->f1 != 0)goto _LL1E6;}}_LL1E5:
{const char*_tmpBD4;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBD4="0.0F",_tag_dyneither(_tmpBD4,sizeof(char),5))),0),0);}goto _LL1D3;_LL1E6: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp45B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp44C;if(_tmp45B->tag != 7)goto _LL1E8;else{if(_tmp45B->f1 != 1)goto _LL1E8;}}_LL1E7:
{const char*_tmpBD5;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBD5="0.0",_tag_dyneither(_tmpBD5,sizeof(char),4))),1),0);}goto _LL1D3;_LL1E8: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp45C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp44C;if(_tmp45C->tag != 7)goto _LL1EA;else{_tmp45D=_tmp45C->f1;}}_LL1E9:
{const char*_tmpBD6;res=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(((_tmpBD6="0.0L",_tag_dyneither(_tmpBD6,sizeof(char),5))),_tmp45D),0);}goto _LL1D3;_LL1EA:;_LL1EB: {
# 2148
const char*_tmpBDA;void*_tmpBD9[1];struct Cyc_String_pa_PrintArg_struct _tmpBD8;(_tmpBD8.tag=0,((_tmpBD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t)),((_tmpBD9[0]=& _tmpBD8,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDA="found non-zero type %s in generate_zero",_tag_dyneither(_tmpBDA,sizeof(char),40))),_tag_dyneither(_tmpBD9,sizeof(void*),1)))))));}_LL1D3:;}
# 2150
res->topt=t;
return res;}
# 2157
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 2170 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp464=Cyc_Toc_typ_to_c(elt_type);
void*_tmp465=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp466=Cyc_Absyn_cstar_typ(_tmp464,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmpBDB;struct Cyc_Core_Opt*_tmp467=(_tmpBDB=_cycalloc(sizeof(*_tmpBDB)),((_tmpBDB->v=_tmp466,_tmpBDB)));
struct Cyc_Absyn_Exp*xinit;
{void*_tmp468=e1->r;void*_tmp469=_tmp468;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*_tmp46D;struct Cyc_Absyn_Exp*_tmp46E;_LL1ED: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp46A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp469;if(_tmp46A->tag != 19)goto _LL1EF;else{_tmp46B=_tmp46A->f1;}}_LL1EE:
# 2178
 if(!is_dyneither){
_tmp46B=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp46B,0,Cyc_Absyn_Other_coercion,0);
_tmp46B->topt=fat_ptr_type;}
# 2182
Cyc_Toc_exp_to_c(nv,_tmp46B);xinit=_tmp46B;goto _LL1EC;_LL1EF: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp46C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp469;if(_tmp46C->tag != 22)goto _LL1F1;else{_tmp46D=_tmp46C->f1;_tmp46E=_tmp46C->f2;}}_LL1F0:
# 2184
 if(!is_dyneither){
_tmp46D=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp46D,0,Cyc_Absyn_Other_coercion,0);
_tmp46D->topt=fat_ptr_type;}
# 2188
Cyc_Toc_exp_to_c(nv,_tmp46D);Cyc_Toc_exp_to_c(nv,_tmp46E);
{struct Cyc_Absyn_Exp*_tmpBDC[3];xinit=Cyc_Absyn_fncall_exp(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpBDC[2]=_tmp46E,((_tmpBDC[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBDC[0]=_tmp46D,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBDC,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);}
goto _LL1EC;_LL1F1:;_LL1F2: {
const char*_tmpBDF;void*_tmpBDE;(_tmpBDE=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpBDF="found bad lhs for zero-terminated pointer assignment",_tag_dyneither(_tmpBDF,sizeof(char),53))),_tag_dyneither(_tmpBDE,sizeof(void*),0)));}_LL1EC:;}{
# 2194
struct _tuple1*_tmp472=Cyc_Toc_temp_var();
struct _RegionHandle _tmp473=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp473;_push_region(rgn2);
{struct Cyc_Toc_Env*_tmp474=Cyc_Toc_add_varmap(rgn2,nv,_tmp472,Cyc_Absyn_var_exp(_tmp472,0));
struct Cyc_Absyn_Vardecl*_tmpBE0;struct Cyc_Absyn_Vardecl*_tmp475=(_tmpBE0=_cycalloc(sizeof(*_tmpBE0)),((_tmpBE0->sc=Cyc_Absyn_Public,((_tmpBE0->name=_tmp472,((_tmpBE0->tq=Cyc_Toc_mt_tq,((_tmpBE0->type=_tmp465,((_tmpBE0->initializer=xinit,((_tmpBE0->rgn=0,((_tmpBE0->attributes=0,((_tmpBE0->escapes=0,_tmpBE0)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBE3;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBE2;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp476=(_tmpBE2=_cycalloc(sizeof(*_tmpBE2)),((_tmpBE2[0]=((_tmpBE3.tag=4,((_tmpBE3.f1=_tmp475,_tmpBE3)))),_tmpBE2)));
struct Cyc_Absyn_Exp*_tmp477=Cyc_Absyn_varb_exp(_tmp472,(void*)_tmp476,0);
_tmp477->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp478=Cyc_Absyn_deref_exp(_tmp477,0);
_tmp478->topt=elt_type;
Cyc_Toc_exp_to_c(_tmp474,_tmp478);{
struct _tuple1*_tmp479=Cyc_Toc_temp_var();
_tmp474=Cyc_Toc_add_varmap(rgn2,_tmp474,_tmp479,Cyc_Absyn_var_exp(_tmp479,0));{
struct Cyc_Absyn_Vardecl*_tmpBE4;struct Cyc_Absyn_Vardecl*_tmp47A=(_tmpBE4=_cycalloc(sizeof(*_tmpBE4)),((_tmpBE4->sc=Cyc_Absyn_Public,((_tmpBE4->name=_tmp479,((_tmpBE4->tq=Cyc_Toc_mt_tq,((_tmpBE4->type=_tmp464,((_tmpBE4->initializer=_tmp478,((_tmpBE4->rgn=0,((_tmpBE4->attributes=0,((_tmpBE4->escapes=0,_tmpBE4)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBE7;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBE6;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47B=(_tmpBE6=_cycalloc(sizeof(*_tmpBE6)),((_tmpBE6[0]=((_tmpBE7.tag=4,((_tmpBE7.f1=_tmp47A,_tmpBE7)))),_tmpBE6)));
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp47C=Cyc_Absyn_varb_exp(_tmp479,(void*)_tmp47B,0);
_tmp47C->topt=_tmp478->topt;
z_init=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp47C,e2,0);
z_init->topt=_tmp47C->topt;}
# 2215
Cyc_Toc_exp_to_c(_tmp474,z_init);{
struct _tuple1*_tmp47D=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmpBE8;struct Cyc_Absyn_Vardecl*_tmp47E=(_tmpBE8=_cycalloc(sizeof(*_tmpBE8)),((_tmpBE8->sc=Cyc_Absyn_Public,((_tmpBE8->name=_tmp47D,((_tmpBE8->tq=Cyc_Toc_mt_tq,((_tmpBE8->type=_tmp464,((_tmpBE8->initializer=z_init,((_tmpBE8->rgn=0,((_tmpBE8->attributes=0,((_tmpBE8->escapes=0,_tmpBE8)))))))))))))))));
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBEB;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBEA;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp47F=(_tmpBEA=_cycalloc(sizeof(*_tmpBEA)),((_tmpBEA[0]=((_tmpBEB.tag=4,((_tmpBEB.f1=_tmp47E,_tmpBEB)))),_tmpBEA)));
_tmp474=Cyc_Toc_add_varmap(rgn2,_tmp474,_tmp47D,Cyc_Absyn_var_exp(_tmp47D,0));{
# 2222
struct Cyc_Absyn_Exp*_tmp480=Cyc_Absyn_varb_exp(_tmp479,(void*)_tmp47B,0);_tmp480->topt=_tmp478->topt;{
struct Cyc_Absyn_Exp*_tmp481=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp482=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp480,_tmp481,0);
_tmp482->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp474,_tmp482);{
# 2228
struct Cyc_Absyn_Exp*_tmp483=Cyc_Absyn_varb_exp(_tmp47D,(void*)_tmp47F,0);_tmp483->topt=_tmp478->topt;{
struct Cyc_Absyn_Exp*_tmp484=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp485=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp483,_tmp484,0);
_tmp485->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(_tmp474,_tmp485);{
# 2234
struct Cyc_Absyn_Exp*_tmpBEC[2];struct Cyc_List_List*_tmp486=(_tmpBEC[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0),((_tmpBEC[0]=
# 2234
Cyc_Absyn_varb_exp(_tmp472,(void*)_tmp476,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpBEC,sizeof(struct Cyc_Absyn_Exp*),2)))));
# 2236
struct Cyc_Absyn_Exp*_tmp487=Cyc_Absyn_uint_exp(1,0);
struct Cyc_Absyn_Exp*xsize;
xsize=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp486,0),_tmp487,0);{
# 2241
struct Cyc_Absyn_Exp*_tmp488=Cyc_Absyn_and_exp(xsize,Cyc_Absyn_and_exp(_tmp482,_tmp485,0),0);
struct Cyc_Absyn_Stmt*_tmp489=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0),0);
struct Cyc_Absyn_Exp*_tmp48A=Cyc_Toc_member_exp(Cyc_Absyn_varb_exp(_tmp472,(void*)_tmp476,0),Cyc_Toc_curr_sp,0);
_tmp48A=Cyc_Toc_cast_it(_tmp466,_tmp48A);{
struct Cyc_Absyn_Exp*_tmp48B=Cyc_Absyn_deref_exp(_tmp48A,0);
struct Cyc_Absyn_Exp*_tmp48C=Cyc_Absyn_assign_exp(_tmp48B,Cyc_Absyn_var_exp(_tmp47D,0),0);
struct Cyc_Absyn_Stmt*_tmp48D=Cyc_Absyn_exp_stmt(_tmp48C,0);
_tmp48D=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp488,_tmp489,Cyc_Absyn_skip_stmt(0),0),_tmp48D,0);
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF2;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBF1;struct Cyc_Absyn_Decl*_tmpBF0;_tmp48D=Cyc_Absyn_decl_stmt(((_tmpBF0=_cycalloc(sizeof(*_tmpBF0)),((_tmpBF0->r=(void*)((_tmpBF2=_cycalloc(sizeof(*_tmpBF2)),((_tmpBF2[0]=((_tmpBF1.tag=0,((_tmpBF1.f1=_tmp47E,_tmpBF1)))),_tmpBF2)))),((_tmpBF0->loc=0,_tmpBF0)))))),_tmp48D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBF8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBF7;struct Cyc_Absyn_Decl*_tmpBF6;_tmp48D=Cyc_Absyn_decl_stmt(((_tmpBF6=_cycalloc(sizeof(*_tmpBF6)),((_tmpBF6->r=(void*)((_tmpBF8=_cycalloc(sizeof(*_tmpBF8)),((_tmpBF8[0]=((_tmpBF7.tag=0,((_tmpBF7.f1=_tmp47A,_tmpBF7)))),_tmpBF8)))),((_tmpBF6->loc=0,_tmpBF6)))))),_tmp48D,0);}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpBFE;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpBFD;struct Cyc_Absyn_Decl*_tmpBFC;_tmp48D=Cyc_Absyn_decl_stmt(((_tmpBFC=_cycalloc(sizeof(*_tmpBFC)),((_tmpBFC->r=(void*)((_tmpBFE=_cycalloc(sizeof(*_tmpBFE)),((_tmpBFE[0]=((_tmpBFD.tag=0,((_tmpBFD.f1=_tmp475,_tmpBFD)))),_tmpBFE)))),((_tmpBFC->loc=0,_tmpBFC)))))),_tmp48D,0);}
e->r=Cyc_Toc_stmt_exp_r(_tmp48D);};};};};};};};};};};};}
# 2196
;_pop_region(rgn2);};}
# 2267 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 2271
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp4A2=Cyc_Tcutil_compress(aggrtype);void*_tmp4A3=_tmp4A2;union Cyc_Absyn_AggrInfoU _tmp4A5;_LL1F4: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4A4=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4A3;if(_tmp4A4->tag != 11)goto _LL1F6;else{_tmp4A5=(_tmp4A4->f1).aggr_info;}}_LL1F5:
# 2274
 ad=Cyc_Absyn_get_known_aggrdecl(_tmp4A5);goto _LL1F3;_LL1F6:;_LL1F7: {
struct Cyc_String_pa_PrintArg_struct _tmpC06;void*_tmpC05[1];const char*_tmpC04;void*_tmpC03;(_tmpC03=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)((_tmpC06.tag=0,((_tmpC06.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype)),((_tmpC05[0]=& _tmpC06,Cyc_aprintf(((_tmpC04="expecting union but found %s in check_tagged_union",_tag_dyneither(_tmpC04,sizeof(char),51))),_tag_dyneither(_tmpC05,sizeof(void*),1)))))))),_tag_dyneither(_tmpC03,sizeof(void*),0)));}_LL1F3:;}{
# 2278
struct _tuple1*_tmp4AA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4AB=Cyc_Absyn_var_exp(_tmp4AA,0);
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp4AD=Cyc_Absyn_aggrarrow_exp(_tmp4AB,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4AE=Cyc_Absyn_neq_exp(_tmp4AD,_tmp4AC,0);
struct Cyc_Absyn_Exp*_tmp4AF=Cyc_Absyn_aggrarrow_exp(_tmp4AB,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4B0=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp4AF,0),0);
struct Cyc_Absyn_Stmt*_tmp4B1=Cyc_Absyn_ifthenelse_stmt(_tmp4AE,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
void*_tmp4B2=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Absyn_address_exp(aggrproj(e1,f,0),0);
struct Cyc_Absyn_Stmt*_tmp4B4=Cyc_Absyn_declare_stmt(_tmp4AA,_tmp4B2,_tmp4B3,Cyc_Absyn_seq_stmt(_tmp4B1,_tmp4B0,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4B4);}else{
# 2292
struct Cyc_Absyn_Exp*_tmp4B5=Cyc_Toc_member_exp(aggrproj(_tmp4AB,f,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*_tmp4B6=Cyc_Absyn_neq_exp(_tmp4B5,_tmp4AC,0);
struct Cyc_Absyn_Exp*_tmp4B7=Cyc_Toc_member_exp(aggrproj(_tmp4AB,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Stmt*_tmp4B8=Cyc_Absyn_exp_stmt(_tmp4B7,0);
struct Cyc_Absyn_Stmt*_tmp4B9=Cyc_Absyn_ifthenelse_stmt(_tmp4B6,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*_tmp4BA=Cyc_Absyn_declare_stmt(_tmp4AA,e1_c_type,e1,Cyc_Absyn_seq_stmt(_tmp4B9,_tmp4B8,0),0);
return Cyc_Toc_stmt_exp_r(_tmp4BA);}};}
# 2302
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 2305
void*_tmp4BB=Cyc_Tcutil_compress(t);void*_tmp4BC=_tmp4BB;union Cyc_Absyn_AggrInfoU _tmp4BE;_LL1F9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp4BD=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4BC;if(_tmp4BD->tag != 11)goto _LL1FB;else{_tmp4BE=(_tmp4BD->f1).aggr_info;}}_LL1FA: {
# 2307
struct Cyc_Absyn_Aggrdecl*_tmp4BF=Cyc_Absyn_get_known_aggrdecl(_tmp4BE);
*f_tag=Cyc_Toc_get_member_offset(_tmp4BF,f);{
# 2310
const char*_tmpC0B;void*_tmpC0A[2];struct Cyc_String_pa_PrintArg_struct _tmpC09;struct Cyc_String_pa_PrintArg_struct _tmpC08;struct _dyneither_ptr str=(struct _dyneither_ptr)((_tmpC08.tag=0,((_tmpC08.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f),((_tmpC09.tag=0,((_tmpC09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp4BF->name).f2),((_tmpC0A[0]=& _tmpC09,((_tmpC0A[1]=& _tmpC08,Cyc_aprintf(((_tmpC0B="_union_%s_%s",_tag_dyneither(_tmpC0B,sizeof(char),13))),_tag_dyneither(_tmpC0A,sizeof(void*),2))))))))))))));
{struct _dyneither_ptr*_tmpC0C;*tagged_member_type=Cyc_Absyn_strct(((_tmpC0C=_cycalloc(sizeof(*_tmpC0C)),((_tmpC0C[0]=str,_tmpC0C)))));}
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp4BF->impl))->tagged;};}_LL1FB:;_LL1FC:
 return 0;_LL1F8:;}
# 2321
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 2324
void*_tmp4C5=e->r;void*_tmp4C6=_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4CA;struct _dyneither_ptr*_tmp4CB;int*_tmp4CC;struct Cyc_Absyn_Exp*_tmp4CE;struct _dyneither_ptr*_tmp4CF;int*_tmp4D0;_LL1FE: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4C7=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C6;if(_tmp4C7->tag != 13)goto _LL200;else{_tmp4C8=_tmp4C7->f2;}}_LL1FF: {
const char*_tmpC0F;void*_tmpC0E;(_tmpC0E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC0F="cast on lhs!",_tag_dyneither(_tmpC0F,sizeof(char),13))),_tag_dyneither(_tmpC0E,sizeof(void*),0)));}_LL200: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp4C9=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C6;if(_tmp4C9->tag != 20)goto _LL202;else{_tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;_tmp4CC=(int*)& _tmp4C9->f4;}}_LL201:
# 2327
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp4CA->topt),_tmp4CB,f_tag,tagged_member_type,clear_read,_tmp4CC);_LL202: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp4CD=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C6;if(_tmp4CD->tag != 21)goto _LL204;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;_tmp4D0=(int*)& _tmp4CD->f4;}}_LL203: {
# 2330
void*_tmp4D3=Cyc_Tcutil_compress((void*)_check_null(_tmp4CE->topt));void*_tmp4D4=_tmp4D3;void*_tmp4D6;_LL207: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4D5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4;if(_tmp4D5->tag != 5)goto _LL209;else{_tmp4D6=(_tmp4D5->f1).elt_typ;}}_LL208:
# 2332
 return Cyc_Toc_is_tagged_union_project_impl(_tmp4D6,_tmp4CF,f_tag,tagged_member_type,clear_read,_tmp4D0);_LL209:;_LL20A:
# 2334
 return 0;_LL206:;}_LL204:;_LL205:
# 2336
 return 0;_LL1FD:;}
# 2348 "toc.cyc"
void Cyc_Toc_add_tagged_union_check_for_swap(struct Cyc_Absyn_Exp*e,int tag,void*mem_type){
struct _tuple1*_tmp4D7=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4D7,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(temp_val,0),0);
struct Cyc_Absyn_Exp*_tmp4D8=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_ifthenelse_stmt(_tmp4D8,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4D7,Cyc_Absyn_cstar_typ(mem_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(e)),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
e->r=Cyc_Toc_stmt_exp_r(s1);}
# 2372 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 2376
struct _tuple1*_tmp4D9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp4D9,0);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0),0);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
s2=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0),0);else{
# 2386
struct Cyc_Absyn_Exp*_tmp4DA=Cyc_Absyn_neq_exp(temp_tag,f_tag,0);
s2=Cyc_Absyn_ifthenelse_stmt(_tmp4DA,Cyc_Toc_throw_match_stmt(),Cyc_Toc_skip_stmt_dl(),0);}{
# 2389
struct Cyc_Absyn_Stmt*s1=Cyc_Absyn_declare_stmt(_tmp4D9,Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(e1),
Cyc_Absyn_seq_stmt(s2,s3,0),0);
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple28{struct _tuple1*f1;void*f2;struct Cyc_Absyn_Exp*f3;};
# 2395
static void Cyc_Toc_new_comprehension_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,void*new_e_type,struct Cyc_Absyn_Exp*rgnopt,void*old_typ,struct Cyc_Absyn_Exp*e1,void*t1,int zero_term,struct Cyc_Absyn_Exp*init_exp,struct Cyc_Absyn_Vardecl*vd){
# 2410 "toc.cyc"
struct _tuple1*max=Cyc_Toc_temp_var();
struct _tuple1*a=Cyc_Toc_temp_var();
void*old_elt_typ=t1;
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*ptr_typ=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
Cyc_Toc_exp_to_c(nv,e1);{
struct Cyc_Absyn_Exp*_tmp4DB=Cyc_Absyn_var_exp(max,0);
# 2418
struct Cyc_Absyn_Stmt*s;
if(zero_term)
_tmp4DB=Cyc_Absyn_add_exp(_tmp4DB,Cyc_Absyn_uint_exp(1,0),0);
if(init_exp != 0)
s=Cyc_Toc_init_comprehension(nv,new_e_type,
Cyc_Absyn_var_exp(a,0),(struct Cyc_Absyn_Vardecl*)_check_null(vd),Cyc_Absyn_var_exp(max,0),init_exp,zero_term,
Cyc_Toc_skip_stmt_dl(),1);else{
if(zero_term)
s=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(a,0),Cyc_Absyn_var_exp(max,0),0),
Cyc_Toc_cast_it(elt_typ,Cyc_Absyn_uint_exp(0,0)),0),0);else{
# 2429
s=Cyc_Absyn_skip_stmt(0);}}{
# 2431
struct _RegionHandle _tmp4DC=_new_region("r");struct _RegionHandle*r=& _tmp4DC;_push_region(r);
{struct _tuple28*_tmpC12;struct Cyc_List_List*_tmpC11;struct Cyc_List_List*decls=
(_tmpC11=_region_malloc(r,sizeof(*_tmpC11)),((_tmpC11->hd=((_tmpC12=_region_malloc(r,sizeof(*_tmpC12)),((_tmpC12->f1=max,((_tmpC12->f2=Cyc_Absyn_uint_typ,((_tmpC12->f3=e1,_tmpC12)))))))),((_tmpC11->tl=0,_tmpC11)))));
struct Cyc_Absyn_Exp*ai;
if(rgnopt == 0  || Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*_tmpC13[2];ai=Cyc_Toc_malloc_exp(old_elt_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC13[1]=_tmp4DB,((_tmpC13[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC13,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));}else{
# 2441
struct Cyc_Absyn_Exp*r=rgnopt;
Cyc_Toc_exp_to_c(nv,r);{
struct Cyc_Absyn_Exp*_tmpC14[2];ai=Cyc_Toc_rmalloc_exp(r,Cyc_Absyn_fncall_exp(Cyc_Toc__check_times_e,(
(_tmpC14[1]=_tmp4DB,((_tmpC14[0]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC14,sizeof(struct Cyc_Absyn_Exp*),2)))))),0));};}{
# 2447
struct Cyc_Absyn_Exp*ainit=Cyc_Toc_cast_it(ptr_typ,ai);
{struct _tuple28*_tmpC17;struct Cyc_List_List*_tmpC16;decls=((_tmpC16=_region_malloc(r,sizeof(*_tmpC16)),((_tmpC16->hd=((_tmpC17=_region_malloc(r,sizeof(*_tmpC17)),((_tmpC17->f1=a,((_tmpC17->f2=ptr_typ,((_tmpC17->f3=ainit,_tmpC17)))))))),((_tmpC16->tl=decls,_tmpC16))))));}
if(!Cyc_Tcutil_is_pointer_type(old_typ)){
const char*_tmpC1A;void*_tmpC19;(_tmpC19=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC1A="new_comprehension_to_c: comprehension is not an array type",_tag_dyneither(_tmpC1A,sizeof(char),59))),_tag_dyneither(_tmpC19,sizeof(void*),0)));}
if(Cyc_Tcutil_is_dyneither_ptr(old_typ)){
struct _tuple1*_tmp4E3=Cyc_Toc_temp_var();
void*_tmp4E4=Cyc_Toc_typ_to_c(old_typ);
struct Cyc_Absyn_Exp*_tmp4E5=Cyc_Toc__tag_dyneither_e;
struct Cyc_Absyn_Exp*_tmpC1B[3];struct Cyc_Absyn_Exp*_tmp4E6=
Cyc_Absyn_fncall_exp(_tmp4E5,(
(_tmpC1B[2]=_tmp4DB,((_tmpC1B[1]=Cyc_Absyn_sizeoftyp_exp(elt_typ,0),((_tmpC1B[0]=Cyc_Absyn_var_exp(a,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC1B,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 2459
{struct _tuple28*_tmpC1E;struct Cyc_List_List*_tmpC1D;decls=((_tmpC1D=_region_malloc(r,sizeof(*_tmpC1D)),((_tmpC1D->hd=((_tmpC1E=_region_malloc(r,sizeof(*_tmpC1E)),((_tmpC1E->f1=_tmp4E3,((_tmpC1E->f2=_tmp4E4,((_tmpC1E->f3=_tmp4E6,_tmpC1E)))))))),((_tmpC1D->tl=decls,_tmpC1D))))));}
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(_tmp4E3,0),0),0);}else{
# 2462
s=Cyc_Absyn_seq_stmt(s,Cyc_Absyn_exp_stmt(Cyc_Absyn_var_exp(a,0),0),0);}
{struct Cyc_List_List*_tmp4EA=decls;for(0;_tmp4EA != 0;_tmp4EA=_tmp4EA->tl){
struct _tuple28 _tmp4EB=*((struct _tuple28*)_tmp4EA->hd);struct _tuple1*_tmp4ED;void*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4EF;struct _tuple28 _tmp4EC=_tmp4EB;_tmp4ED=_tmp4EC.f1;_tmp4EE=_tmp4EC.f2;_tmp4EF=_tmp4EC.f3;
s=Cyc_Absyn_declare_stmt(_tmp4ED,_tmp4EE,_tmp4EF,s,0);}}
# 2467
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2432
;_pop_region(r);};};}struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);static void _tmpC6D(unsigned int*_tmpC6C,unsigned int*_tmpC6B,struct _tuple1***_tmpC69){for(*_tmpC6C=0;*_tmpC6C < *_tmpC6B;(*_tmpC6C)++){(*_tmpC69)[*_tmpC6C]=(struct _tuple1*)
# 2842 "toc.cyc"
Cyc_Toc_temp_var();}}
# 2471 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp4F2=e->r;
if(e->topt == 0){
const char*_tmpC22;void*_tmpC21[1];struct Cyc_String_pa_PrintArg_struct _tmpC20;(_tmpC20.tag=0,((_tmpC20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC21[0]=& _tmpC20,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC22="exp_to_c: no type for %s",_tag_dyneither(_tmpC22,sizeof(char),25))),_tag_dyneither(_tmpC21,sizeof(void*),1)))))));}
# 2477
if((nv->struct_info).lhs_exp != 0){
void*_tmp4F6=_tmp4F2;_LL20C: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4F7=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4F6;if(_tmp4F7->tag != 28)goto _LL20E;}_LL20D:
 goto _LL20B;_LL20E:;_LL20F:
 nv=Cyc_Toc_set_lhs_exp(nv->rgn,0,nv);_LL20B:;}{
# 2485
void*old_typ=(void*)_check_null(e->topt);
void*_tmp4F8=_tmp4F2;struct _tuple1*_tmp4FC;void*_tmp4FD;enum Cyc_Absyn_Primop _tmp4FF;struct Cyc_List_List*_tmp500;struct Cyc_Absyn_Exp*_tmp502;enum Cyc_Absyn_Incrementor _tmp503;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Core_Opt*_tmp506;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50E;struct Cyc_Absyn_Exp*_tmp510;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_List_List*_tmp51A;int _tmp51B;struct Cyc_List_List*_tmp51C;struct Cyc_Absyn_VarargInfo*_tmp51D;struct Cyc_Absyn_Exp*_tmp51F;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_List_List*_tmp524;void**_tmp526;struct Cyc_Absyn_Exp*_tmp527;int _tmp528;enum Cyc_Absyn_Coercion _tmp529;struct Cyc_Absyn_Exp*_tmp52B;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp530;void*_tmp532;void*_tmp534;struct Cyc_List_List*_tmp535;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp539;struct _dyneither_ptr*_tmp53A;int _tmp53B;int _tmp53C;struct Cyc_Absyn_Exp*_tmp53E;struct _dyneither_ptr*_tmp53F;int _tmp540;int _tmp541;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_List_List*_tmp546;struct Cyc_List_List*_tmp548;struct Cyc_Absyn_Vardecl*_tmp54A;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54C;int _tmp54D;struct Cyc_Absyn_Exp*_tmp54F;void*_tmp550;int _tmp551;struct _tuple1*_tmp553;struct Cyc_List_List*_tmp554;struct Cyc_List_List*_tmp555;struct Cyc_Absyn_Aggrdecl*_tmp556;void*_tmp558;struct Cyc_List_List*_tmp559;struct Cyc_List_List*_tmp55B;struct Cyc_Absyn_Datatypedecl*_tmp55C;struct Cyc_Absyn_Datatypefield*_tmp55D;int _tmp561;struct Cyc_Absyn_Exp*_tmp562;void**_tmp563;struct Cyc_Absyn_Exp*_tmp564;int _tmp565;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Exp*_tmp568;struct Cyc_Absyn_Exp*_tmp56A;struct _dyneither_ptr*_tmp56B;struct Cyc_Absyn_Stmt*_tmp56D;_LL211: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4F9=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4F9->tag != 0)goto _LL213;else{if(((_tmp4F9->f1).Null_c).tag != 1)goto _LL213;}}_LL212: {
# 2492
struct Cyc_Absyn_Exp*_tmp572=Cyc_Absyn_uint_exp(0,0);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp572,_tmp572))->r;else{
# 2497
struct Cyc_Absyn_Exp*_tmpC23[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__tag_dyneither_e,((_tmpC23[2]=_tmp572,((_tmpC23[1]=_tmp572,((_tmpC23[0]=_tmp572,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC23,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}else{
# 2499
e->r=(void*)& Cyc_Toc_zero_exp;}
# 2501
goto _LL210;}_LL213: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4FA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4FA->tag != 0)goto _LL215;}_LL214:
 goto _LL210;_LL215: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp4FB=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4FB->tag != 1)goto _LL217;else{_tmp4FC=_tmp4FB->f1;_tmp4FD=(void*)_tmp4FB->f2;}}_LL216:
# 2504
{struct _handler_cons _tmp574;_push_handler(& _tmp574);{int _tmp576=0;if(setjmp(_tmp574.handler))_tmp576=1;if(!_tmp576){e->r=(Cyc_Toc_lookup_varmap(nv,_tmp4FC))->r;;_pop_handler();}else{void*_tmp575=(void*)_exn_thrown;void*_tmp578=_tmp575;void*_tmp57A;_LL266: {struct Cyc_Dict_Absent_exn_struct*_tmp579=(struct Cyc_Dict_Absent_exn_struct*)_tmp578;if(_tmp579->tag != Cyc_Dict_Absent)goto _LL268;}_LL267: {
# 2506
const char*_tmpC27;void*_tmpC26[1];struct Cyc_String_pa_PrintArg_struct _tmpC25;(_tmpC25.tag=0,((_tmpC25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp4FC)),((_tmpC26[0]=& _tmpC25,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC27="Can't find %s in exp_to_c, Var\n",_tag_dyneither(_tmpC27,sizeof(char),32))),_tag_dyneither(_tmpC26,sizeof(void*),1)))))));}_LL268: _tmp57A=_tmp578;_LL269:(void)_throw(_tmp57A);_LL265:;}};}
# 2508
goto _LL210;_LL217: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp4FE=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp4FE->tag != 2)goto _LL219;else{_tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;}}_LL218: {
# 2511
struct Cyc_List_List*_tmp57E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp500);
# 2513
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp500);
switch(_tmp4FF){case Cyc_Absyn_Numelts: _LL26A: {
# 2516
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd;
{void*_tmp57F=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp580=_tmp57F;struct Cyc_Absyn_Exp*_tmp582;void*_tmp584;union Cyc_Absyn_Constraint*_tmp585;union Cyc_Absyn_Constraint*_tmp586;union Cyc_Absyn_Constraint*_tmp587;_LL26D: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp581=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp580;if(_tmp581->tag != 8)goto _LL26F;else{_tmp582=(_tmp581->f1).num_elts;}}_LL26E:
# 2520
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp582))){
const char*_tmpC2A;void*_tmpC29;(_tmpC29=0,Cyc_Tcutil_terr(e->loc,((_tmpC2A="can't calculate numelts",_tag_dyneither(_tmpC2A,sizeof(char),24))),_tag_dyneither(_tmpC29,sizeof(void*),0)));}
e->r=_tmp582->r;goto _LL26C;_LL26F: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp583=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp580;if(_tmp583->tag != 5)goto _LL271;else{_tmp584=(_tmp583->f1).elt_typ;_tmp585=((_tmp583->f1).ptr_atts).nullable;_tmp586=((_tmp583->f1).ptr_atts).bounds;_tmp587=((_tmp583->f1).ptr_atts).zero_term;}}_LL270:
# 2524
{void*_tmp58A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp586);void*_tmp58B=_tmp58A;struct Cyc_Absyn_Exp*_tmp58E;_LL274: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp58C=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp58B;if(_tmp58C->tag != 0)goto _LL276;}_LL275:
# 2526
{struct Cyc_Absyn_Exp*_tmpC2B[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__get_dyneither_size_e,(
(_tmpC2B[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp584),0),((_tmpC2B[0]=(struct Cyc_Absyn_Exp*)_tmp500->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2B,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
goto _LL273;_LL276: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58D=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58B;if(_tmp58D->tag != 1)goto _LL273;else{_tmp58E=_tmp58D->f1;}}_LL277:
# 2532
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp587)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp500->hd);
# 2536
struct Cyc_Absyn_Exp*_tmpC2C[2];e->r=Cyc_Toc_fncall_exp_r(function_e,((_tmpC2C[1]=_tmp58E,((_tmpC2C[0]=(struct Cyc_Absyn_Exp*)_tmp500->hd,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC2C,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp585)){
if(!Cyc_Evexp_c_can_eval(_tmp58E)){
const char*_tmpC2F;void*_tmpC2E;(_tmpC2E=0,Cyc_Tcutil_terr(e->loc,((_tmpC2F="can't calculate numelts",_tag_dyneither(_tmpC2F,sizeof(char),24))),_tag_dyneither(_tmpC2E,sizeof(void*),0)));}
# 2541
e->r=Cyc_Toc_conditional_exp_r(arg,_tmp58E,Cyc_Absyn_uint_exp(0,0));}else{
# 2543
e->r=_tmp58E->r;goto _LL273;}}
# 2545
goto _LL273;_LL273:;}
# 2547
goto _LL26C;_LL271:;_LL272: {
# 2549
const char*_tmpC34;void*_tmpC33[2];struct Cyc_String_pa_PrintArg_struct _tmpC32;struct Cyc_String_pa_PrintArg_struct _tmpC31;(_tmpC31.tag=0,((_tmpC31.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC32.tag=0,((_tmpC32.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt))),((_tmpC33[0]=& _tmpC32,((_tmpC33[1]=& _tmpC31,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC34="size primop applied to non-array %s (%s)",_tag_dyneither(_tmpC34,sizeof(char),41))),_tag_dyneither(_tmpC33,sizeof(void*),2)))))))))))));}_LL26C:;}
# 2552
break;}case Cyc_Absyn_Plus: _LL26B:
# 2557
{void*_tmp597=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp57E))->hd);void*_tmp598=_tmp597;void*_tmp59A;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59C;_LL27A: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp599=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598;if(_tmp599->tag != 5)goto _LL27C;else{_tmp59A=(_tmp599->f1).elt_typ;_tmp59B=((_tmp599->f1).ptr_atts).bounds;_tmp59C=((_tmp599->f1).ptr_atts).zero_term;}}_LL27B:
# 2559
{void*_tmp59D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp59B);void*_tmp59E=_tmp59D;struct Cyc_Absyn_Exp*_tmp5A1;_LL27F: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp59F=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp59E;if(_tmp59F->tag != 0)goto _LL281;}_LL280: {
# 2561
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500->tl))->hd;
{struct Cyc_Absyn_Exp*_tmpC35[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC35[2]=e2,((_tmpC35[1]=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp59A),0),((_tmpC35[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC35,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}
goto _LL27E;}_LL281: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5A0=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59E;if(_tmp5A0->tag != 1)goto _LL27E;else{_tmp5A1=_tmp5A0->f1;}}_LL282:
# 2567
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59C)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500->tl))->hd;
struct Cyc_Absyn_Exp*_tmpC36[3];e->r=(Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1),((_tmpC36[2]=e2,((_tmpC36[1]=_tmp5A1,((_tmpC36[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC36,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0))->r;}
# 2572
goto _LL27E;_LL27E:;}
# 2574
goto _LL279;_LL27C:;_LL27D:
# 2576
 goto _LL279;_LL279:;}
# 2578
break;case Cyc_Absyn_Minus: _LL278: {
# 2583
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp57E))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp57E->tl))->hd)){
e1->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp);
e2->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp);
e->r=(Cyc_Absyn_divide_exp(Cyc_Absyn_copy_exp(e),
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),0))->r;}else{
# 2593
struct Cyc_Absyn_Exp*_tmpC37[3];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__dyneither_ptr_plus_e,(
(_tmpC37[2]=
Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0),((_tmpC37[1]=
# 2594
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC37[0]=e1,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC37,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 2598
break;}case Cyc_Absyn_Eq: _LL283:
 goto _LL284;case Cyc_Absyn_Neq: _LL284:
 goto _LL285;case Cyc_Absyn_Gt: _LL285:
 goto _LL286;case Cyc_Absyn_Gte: _LL286:
 goto _LL287;case Cyc_Absyn_Lt: _LL287:
 goto _LL288;case Cyc_Absyn_Lte: _LL288: {
# 2606
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp500->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp57E))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp57E->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ))
e1->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e1->r,0),Cyc_Toc_curr_sp,0));
if(Cyc_Tcutil_is_tagged_pointer_typ(t2))
e2->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c(elt_typ),Cyc_Toc_mt_tq),Cyc_Toc_member_exp(Cyc_Absyn_new_exp(e2->r,0),Cyc_Toc_curr_sp,0));
break;}default: _LL289:
 break;}
# 2618
goto _LL210;}_LL219: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp501=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp501->tag != 4)goto _LL21B;else{_tmp502=_tmp501->f1;_tmp503=_tmp501->f2;}}_LL21A: {
# 2620
void*e2_cyc_typ=(void*)_check_null(_tmp502->topt);
# 2629 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
const char*_tmpC38;struct _dyneither_ptr incr_str=(_tmpC38="increment",_tag_dyneither(_tmpC38,sizeof(char),10));
if(_tmp503 == Cyc_Absyn_PreDec  || _tmp503 == Cyc_Absyn_PostDec){const char*_tmpC39;incr_str=((_tmpC39="decrement",_tag_dyneither(_tmpC39,sizeof(char),10)));}
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp502,& ptr_type,& is_dyneither,& elt_type)){
{const char*_tmpC3D;void*_tmpC3C[1];struct Cyc_String_pa_PrintArg_struct _tmpC3B;(_tmpC3B.tag=0,((_tmpC3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC3C[0]=& _tmpC3B,Cyc_Tcutil_terr(e->loc,((_tmpC3D="in-place %s is not supported when dereferencing a zero-terminated pointer",_tag_dyneither(_tmpC3D,sizeof(char),74))),_tag_dyneither(_tmpC3C,sizeof(void*),1)))))));}{
const char*_tmpC40;void*_tmpC3F;(_tmpC3F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC40="in-place inc/dec on zero-term",_tag_dyneither(_tmpC40,sizeof(char),30))),_tag_dyneither(_tmpC3F,sizeof(void*),0)));};}{
# 2638
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp502,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_signed_int_exp(1,0);
_tmp5AB->topt=Cyc_Absyn_sint_typ;
switch(_tmp503){case Cyc_Absyn_PreInc: _LL28B:
# 2645
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC46;struct Cyc_Core_Opt*_tmpC45;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC44;e->r=(void*)((_tmpC44=_cycalloc(sizeof(*_tmpC44)),((_tmpC44[0]=((_tmpC46.tag=3,((_tmpC46.f1=_tmp502,((_tmpC46.f2=((_tmpC45=_cycalloc_atomic(sizeof(*_tmpC45)),((_tmpC45->v=(void*)Cyc_Absyn_Plus,_tmpC45)))),((_tmpC46.f3=_tmp5AB,_tmpC46)))))))),_tmpC44))));}
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL28C:
# 2649
{struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmpC4C;struct Cyc_Core_Opt*_tmpC4B;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmpC4A;e->r=(void*)((_tmpC4A=_cycalloc(sizeof(*_tmpC4A)),((_tmpC4A[0]=((_tmpC4C.tag=3,((_tmpC4C.f1=_tmp502,((_tmpC4C.f2=((_tmpC4B=_cycalloc_atomic(sizeof(*_tmpC4B)),((_tmpC4B->v=(void*)Cyc_Absyn_Minus,_tmpC4B)))),((_tmpC4C.f3=_tmp5AB,_tmpC4C)))))))),_tmpC4A))));}
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL28D:
# 2653
{const char*_tmpC50;void*_tmpC4F[1];struct Cyc_String_pa_PrintArg_struct _tmpC4E;(_tmpC4E.tag=0,((_tmpC4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str),((_tmpC4F[0]=& _tmpC4E,Cyc_Tcutil_terr(e->loc,((_tmpC50="in-place post-%s is not supported on @tagged union members",_tag_dyneither(_tmpC50,sizeof(char),59))),_tag_dyneither(_tmpC4F,sizeof(void*),1)))))));}{
# 2655
const char*_tmpC53;void*_tmpC52;(_tmpC52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC53="in-place inc/dec on @tagged union",_tag_dyneither(_tmpC53,sizeof(char),34))),_tag_dyneither(_tmpC52,sizeof(void*),0)));};}}
# 2658
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp502);
Cyc_Toc_set_lhs(nv,0);{
# 2663
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp503 == Cyc_Absyn_PostInc  || _tmp503 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2669
if(_tmp503 == Cyc_Absyn_PreDec  || _tmp503 == Cyc_Absyn_PostDec)
change=-1;{
struct Cyc_Absyn_Exp*_tmpC54[3];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC54[2]=
# 2673
Cyc_Absyn_signed_int_exp(change,0),((_tmpC54[1]=
# 2672
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC54[0]=
# 2671
Cyc_Toc_push_address_exp(_tmp502),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC54,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2674
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2678
struct Cyc_Toc_functionSet*_tmp5B8=_tmp503 != Cyc_Absyn_PostInc?& Cyc_Toc__zero_arr_inplace_plus_functionSet:& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
# 2680
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp5B8,_tmp502);
struct Cyc_Absyn_Exp*_tmpC55[2];e->r=Cyc_Toc_fncall_exp_r(fn_e,((_tmpC55[1]=
Cyc_Absyn_signed_int_exp(1,0),((_tmpC55[0]=
# 2681
Cyc_Toc_push_address_exp(_tmp502),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC55,sizeof(struct Cyc_Absyn_Exp*),2)))))));}else{
# 2683
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp502)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp502,0,Cyc_Toc_incr_lvalue,_tmp503);
e->r=_tmp502->r;}}}
# 2687
goto _LL210;};};}_LL21B: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp504=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp504->tag != 3)goto _LL21D;else{_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;_tmp507=_tmp504->f3;}}_LL21C: {
# 2706 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp505->topt);
void*e2_old_typ=(void*)_check_null(_tmp507->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp505,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp505);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp507);
e->r=Cyc_Toc_tagged_union_assignop(_tmp505,e1_old_typ,_tmp506,_tmp507,e2_old_typ,f_tag,tagged_member_struct_type);
# 2717
return;}{
# 2719
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp505,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp505,_tmp506,_tmp507,ptr_type,is_dyneither,elt_type);
# 2725
return;}{
# 2729
int e1_poly=Cyc_Toc_is_poly_project(_tmp505);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp505);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp507);{
# 2735
int done=0;
if(_tmp506 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp5BB=(enum Cyc_Absyn_Primop)_tmp506->v;switch(_tmp5BB){case Cyc_Absyn_Plus: _LL28F:
 change=_tmp507;break;case Cyc_Absyn_Minus: _LL290:
# 2743
 change=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp507,0);break;default: _LL291: {
const char*_tmpC58;void*_tmpC57;(_tmpC57=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC58="bad t ? pointer arithmetic",_tag_dyneither(_tmpC58,sizeof(char),27))),_tag_dyneither(_tmpC57,sizeof(void*),0)));}}}
# 2746
done=1;{
# 2748
struct Cyc_Absyn_Exp*_tmp5BE=Cyc_Toc__dyneither_ptr_inplace_plus_e;
struct Cyc_Absyn_Exp*_tmpC59[3];e->r=Cyc_Toc_fncall_exp_r(_tmp5BE,((_tmpC59[2]=change,((_tmpC59[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0),((_tmpC59[0]=
# 2749
Cyc_Toc_push_address_exp(_tmp505),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC59,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}else{
# 2752
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2755
enum Cyc_Absyn_Primop _tmp5C0=(enum Cyc_Absyn_Primop)_tmp506->v;switch(_tmp5C0){case Cyc_Absyn_Plus: _LL293:
# 2757
 done=1;
{struct Cyc_Absyn_Exp*_tmpC5A[2];e->r=Cyc_Toc_fncall_exp_r(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp505),((_tmpC5A[1]=_tmp507,((_tmpC5A[0]=_tmp505,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC5A,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
break;default: _LL294: {
const char*_tmpC5D;void*_tmpC5C;(_tmpC5C=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC5D="bad zero-terminated pointer arithmetic",_tag_dyneither(_tmpC5D,sizeof(char),39))),_tag_dyneither(_tmpC5C,sizeof(void*),0)));}}}}}
# 2764
if(!done){
# 2766
if(e1_poly)
_tmp507->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp507->r,0));
# 2772
if(!Cyc_Absyn_is_lvalue(_tmp505)){
{struct _tuple27 _tmpC60;struct _tuple27*_tmpC5F;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple27*),struct _tuple27*f_env))Cyc_Toc_lvalue_assign)(_tmp505,0,Cyc_Toc_assignop_lvalue,((_tmpC5F=_cycalloc(sizeof(struct _tuple27)* 1),((_tmpC5F[0]=(struct _tuple27)((_tmpC60.f1=_tmp506,((_tmpC60.f2=_tmp507,_tmpC60)))),_tmpC5F)))));}
e->r=_tmp505->r;}}
# 2777
goto _LL210;};};};}_LL21D: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp508=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp508->tag != 5)goto _LL21F;else{_tmp509=_tmp508->f1;_tmp50A=_tmp508->f2;_tmp50B=_tmp508->f3;}}_LL21E:
# 2779
 Cyc_Toc_exp_to_c(nv,_tmp509);
Cyc_Toc_exp_to_c(nv,_tmp50A);
Cyc_Toc_exp_to_c(nv,_tmp50B);
goto _LL210;_LL21F: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp50C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp50C->tag != 6)goto _LL221;else{_tmp50D=_tmp50C->f1;_tmp50E=_tmp50C->f2;}}_LL220:
# 2784
 Cyc_Toc_exp_to_c(nv,_tmp50D);
Cyc_Toc_exp_to_c(nv,_tmp50E);
goto _LL210;_LL221: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp50F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp50F->tag != 7)goto _LL223;else{_tmp510=_tmp50F->f1;_tmp511=_tmp50F->f2;}}_LL222:
# 2788
 Cyc_Toc_exp_to_c(nv,_tmp510);
Cyc_Toc_exp_to_c(nv,_tmp511);
goto _LL210;_LL223: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp512=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp512->tag != 8)goto _LL225;else{_tmp513=_tmp512->f1;_tmp514=_tmp512->f2;}}_LL224:
# 2792
 Cyc_Toc_exp_to_c(nv,_tmp513);
Cyc_Toc_exp_to_c(nv,_tmp514);
goto _LL210;_LL225: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp515=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp515->tag != 9)goto _LL227;else{_tmp516=_tmp515->f1;_tmp517=_tmp515->f2;if(_tmp515->f3 != 0)goto _LL227;}}_LL226:
# 2796
 Cyc_Toc_exp_to_c(nv,_tmp516);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp517);
goto _LL210;_LL227: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp518=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp518->tag != 9)goto _LL229;else{_tmp519=_tmp518->f1;_tmp51A=_tmp518->f2;if(_tmp518->f3 == 0)goto _LL229;_tmp51B=(_tmp518->f3)->num_varargs;_tmp51C=(_tmp518->f3)->injectors;_tmp51D=(_tmp518->f3)->vai;}}_LL228: {
# 2808 "toc.cyc"
struct _RegionHandle _tmp5C6=_new_region("r");struct _RegionHandle*r=& _tmp5C6;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp51B,0);
void*cva_type=Cyc_Toc_typ_to_c(_tmp51D->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0);
# 2816
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51A);
int num_normargs=num_args - _tmp51B;
# 2820
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp51A=_tmp51A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp51A))->hd);{
struct Cyc_List_List*_tmpC61;new_args=((_tmpC61=_cycalloc(sizeof(*_tmpC61)),((_tmpC61->hd=(struct Cyc_Absyn_Exp*)_tmp51A->hd,((_tmpC61->tl=new_args,_tmpC61))))));};}}
# 2825
{struct Cyc_Absyn_Exp*_tmpC64[3];struct Cyc_List_List*_tmpC63;new_args=((_tmpC63=_cycalloc(sizeof(*_tmpC63)),((_tmpC63->hd=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpC64[2]=num_varargs_exp,((_tmpC64[1]=
Cyc_Absyn_sizeoftyp_exp(cva_type,0),((_tmpC64[0]=argvexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC64,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0),((_tmpC63->tl=new_args,_tmpC63))))));}
# 2830
new_args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);
# 2832
Cyc_Toc_exp_to_c(nv,_tmp519);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp519,new_args,0),0);
# 2836
if(_tmp51D->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp5CA=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp51D->type));void*_tmp5CB=_tmp5CA;struct Cyc_Absyn_Datatypedecl*_tmp5CD;_LL297: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5CC=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5CB;if(_tmp5CC->tag != 3)goto _LL299;else{if((((_tmp5CC->f1).datatype_info).KnownDatatype).tag != 2)goto _LL299;_tmp5CD=*((struct Cyc_Absyn_Datatypedecl**)(((_tmp5CC->f1).datatype_info).KnownDatatype).val);}}_LL298:
 tud=_tmp5CD;goto _LL296;_LL299:;_LL29A: {
const char*_tmpC67;void*_tmpC66;(_tmpC66=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpC67="toc: unknown datatype in vararg with inject",_tag_dyneither(_tmpC67,sizeof(char),44))),_tag_dyneither(_tmpC66,sizeof(void*),0)));}_LL296:;}{
# 2842
unsigned int _tmpC6C;unsigned int _tmpC6B;struct _dyneither_ptr _tmpC6A;struct _tuple1**_tmpC69;unsigned int _tmpC68;struct _dyneither_ptr vs=(_tmpC68=(unsigned int)_tmp51B,((_tmpC69=(struct _tuple1**)_region_malloc(r,_check_times(sizeof(struct _tuple1*),_tmpC68)),((_tmpC6A=_tag_dyneither(_tmpC69,sizeof(struct _tuple1*),_tmpC68),((((_tmpC6B=_tmpC68,_tmpC6D(& _tmpC6C,& _tmpC6B,& _tmpC69))),_tmpC6A)))))));
# 2844
if(_tmp51B != 0){
# 2846
struct Cyc_List_List*_tmp5D0=0;
{int i=_tmp51B - 1;for(0;i >= 0;-- i){
struct Cyc_List_List*_tmpC6E;_tmp5D0=((_tmpC6E=_cycalloc(sizeof(*_tmpC6E)),((_tmpC6E->hd=Cyc_Toc_make_dle(Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0),0)),((_tmpC6E->tl=_tmp5D0,_tmpC6E))))));}}
# 2850
s=Cyc_Absyn_declare_stmt(argv,arr_type,Cyc_Absyn_unresolvedmem_exp(0,_tmp5D0,0),s,0);{
# 2853
int i=0;for(0;_tmp51A != 0;(((_tmp51A=_tmp51A->tl,_tmp51C=_tmp51C->tl)),++ i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp51A->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0);
struct Cyc_Absyn_Datatypefield*_tmp5D2=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp51C))->hd;struct _tuple1*_tmp5D4;struct Cyc_List_List*_tmp5D5;struct Cyc_Absyn_Datatypefield*_tmp5D3=_tmp5D2;_tmp5D4=_tmp5D3->name;_tmp5D5=_tmp5D3->typs;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp5D5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
arg=Cyc_Toc_cast_it(field_typ,arg);
# 2868
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Absyn_fieldname(1),0),arg,0),s,0);
# 2871
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(varexp,Cyc_Toc_tag_sp,0),
Cyc_Toc_datatype_tag(tud,_tmp5D4),0),s,0);
# 2874
s=Cyc_Absyn_declare_stmt(var,Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp5D4,tud->name)),0,s,0);};}};}else{
# 2881
struct _tuple19*_tmpC6F[3];struct Cyc_List_List*_tmp5D6=(_tmpC6F[2]=
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC6F[1]=
# 2881
Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((_tmpC6F[0]=Cyc_Toc_make_dle(Cyc_Absyn_uint_exp(0,0)),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC6F,sizeof(struct _tuple19*),3)))))));
# 2883
s=Cyc_Absyn_declare_stmt(argv,Cyc_Absyn_void_star_typ(),Cyc_Absyn_uint_exp(0,0),s,0);}};}else{
# 2893
{int i=0;for(0;_tmp51A != 0;(_tmp51A=_tmp51A->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp51A->hd);
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(argvexp,Cyc_Absyn_uint_exp((unsigned int)i,0),0),(struct Cyc_Absyn_Exp*)_tmp51A->hd,0),s,0);}}
# 2899
s=Cyc_Absyn_declare_stmt(argv,arr_type,0,s,0);}
# 2912 "toc.cyc"
e->r=Cyc_Toc_stmt_exp_r(s);};}
# 2914
_npop_handler(0);goto _LL210;
# 2808 "toc.cyc"
;_pop_region(r);}_LL229: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp51E=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp51E->tag != 10)goto _LL22B;else{_tmp51F=_tmp51E->f1;}}_LL22A:
# 2917 "toc.cyc"
 Cyc_Toc_exp_to_c(nv,_tmp51F);
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c(old_typ),Cyc_Toc_newthrow_exp(_tmp51F),0))->r;
goto _LL210;_LL22B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp520=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp520->tag != 11)goto _LL22D;else{_tmp521=_tmp520->f1;}}_LL22C:
 Cyc_Toc_exp_to_c(nv,_tmp521);goto _LL210;_LL22D: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp522=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp522->tag != 12)goto _LL22F;else{_tmp523=_tmp522->f1;_tmp524=_tmp522->f2;}}_LL22E:
# 2922
 Cyc_Toc_exp_to_c(nv,_tmp523);
# 2931 "toc.cyc"
for(0;_tmp524 != 0;_tmp524=_tmp524->tl){
enum Cyc_Absyn_KindQual _tmp5DC=(Cyc_Tcutil_typ_kind((void*)_tmp524->hd))->kind;
if(_tmp5DC != Cyc_Absyn_EffKind  && _tmp5DC != Cyc_Absyn_RgnKind){
{void*_tmp5DD=Cyc_Tcutil_compress((void*)_tmp524->hd);void*_tmp5DE=_tmp5DD;_LL29C: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5DF=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5DE;if(_tmp5DF->tag != 2)goto _LL29E;}_LL29D:
 goto _LL29F;_LL29E: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp5E0=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp5DE;if(_tmp5E0->tag != 3)goto _LL2A0;}_LL29F:
 continue;_LL2A0:;_LL2A1:
# 2938
 e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp523,0))->r;
goto _LL29B;_LL29B:;}
# 2941
break;}}
# 2944
goto _LL210;_LL22F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp525=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp525->tag != 13)goto _LL231;else{_tmp526=(void**)((void**)& _tmp525->f1);_tmp527=_tmp525->f2;_tmp528=_tmp525->f3;_tmp529=_tmp525->f4;}}_LL230: {
# 2946
void*old_t2=(void*)_check_null(_tmp527->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp526;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp526=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp527);
# 2953
{struct _tuple29 _tmpC70;struct _tuple29 _tmp5E1=(_tmpC70.f1=Cyc_Tcutil_compress(old_t2),((_tmpC70.f2=Cyc_Tcutil_compress(new_typ),_tmpC70)));struct _tuple29 _tmp5E2=_tmp5E1;struct Cyc_Absyn_PtrInfo _tmp5E4;struct Cyc_Absyn_PtrInfo _tmp5E6;struct Cyc_Absyn_PtrInfo _tmp5E8;_LL2A3:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E2.f1;if(_tmp5E3->tag != 5)goto _LL2A5;else{_tmp5E4=_tmp5E3->f1;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E2.f2;if(_tmp5E5->tag != 5)goto _LL2A5;else{_tmp5E6=_tmp5E5->f1;}};_LL2A4: {
# 2955
int _tmp5EA=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E4.ptr_atts).nullable);
int _tmp5EB=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E6.ptr_atts).nullable);
void*_tmp5EC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E4.ptr_atts).bounds);
void*_tmp5ED=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E6.ptr_atts).bounds);
int _tmp5EE=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E4.ptr_atts).zero_term);
int _tmp5EF=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp5E6.ptr_atts).zero_term);
{struct _tuple29 _tmpC71;struct _tuple29 _tmp5F0=(_tmpC71.f1=_tmp5EC,((_tmpC71.f2=_tmp5ED,_tmpC71)));struct _tuple29 _tmp5F1=_tmp5F0;struct Cyc_Absyn_Exp*_tmp5F3;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Exp*_tmp5FB;_LL2AA:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F2=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F1.f1;if(_tmp5F2->tag != 1)goto _LL2AC;else{_tmp5F3=_tmp5F2->f1;}}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F4=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F1.f2;if(_tmp5F4->tag != 1)goto _LL2AC;else{_tmp5F5=_tmp5F4->f1;}};_LL2AB:
# 2963
 if((!Cyc_Evexp_c_can_eval(_tmp5F3) || !Cyc_Evexp_c_can_eval(_tmp5F5)) && !
Cyc_Evexp_same_const_exp(_tmp5F3,_tmp5F5)){
const char*_tmpC74;void*_tmpC73;(_tmpC73=0,Cyc_Tcutil_terr(e->loc,((_tmpC74="can't validate cast due to potential size differences",_tag_dyneither(_tmpC74,sizeof(char),54))),_tag_dyneither(_tmpC73,sizeof(void*),0)));}
if(_tmp5EA  && !_tmp5EB){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC77;void*_tmpC76;(_tmpC76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC77="can't do NULL-check conversion at top-level",_tag_dyneither(_tmpC77,sizeof(char),44))),_tag_dyneither(_tmpC76,sizeof(void*),0)));}
# 2972
if(_tmp529 != Cyc_Absyn_NonNull_to_Null){
const char*_tmpC7B;void*_tmpC7A[1];struct Cyc_String_pa_PrintArg_struct _tmpC79;(_tmpC79.tag=0,((_tmpC79.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC7A[0]=& _tmpC79,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC7B="null-check conversion mis-classified: %s",_tag_dyneither(_tmpC7B,sizeof(char),41))),_tag_dyneither(_tmpC7A,sizeof(void*),1)))))));}{
int do_null_check=Cyc_Toc_need_null_check(_tmp527);
if(do_null_check){
if(!_tmp528){
const char*_tmpC7E;void*_tmpC7D;(_tmpC7D=0,Cyc_Tcutil_warn(e->loc,((_tmpC7E="inserted null check due to implicit cast from * to @ type",_tag_dyneither(_tmpC7E,sizeof(char),58))),_tag_dyneither(_tmpC7D,sizeof(void*),0)));}{
# 2979
struct Cyc_List_List*_tmpC7F;e->r=Cyc_Toc_cast_it_r(*_tmp526,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpC7F=_cycalloc(sizeof(*_tmpC7F)),((_tmpC7F->hd=_tmp527,((_tmpC7F->tl=0,_tmpC7F)))))),0));};}else{
# 2983
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp527->r;}};}else{
# 2988
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp527->r;}
# 2995
goto _LL2A9;_LL2AC:{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5F6=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F1.f1;if(_tmp5F6->tag != 1)goto _LL2AE;else{_tmp5F7=_tmp5F6->f1;}}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F8=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F1.f2;if(_tmp5F8->tag != 0)goto _LL2AE;};_LL2AD:
# 2997
 if(!Cyc_Evexp_c_can_eval(_tmp5F7)){
const char*_tmpC82;void*_tmpC81;(_tmpC81=0,Cyc_Tcutil_terr(e->loc,((_tmpC82="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC82,sizeof(char),71))),_tag_dyneither(_tmpC81,sizeof(void*),0)));}
# 3000
if(_tmp529 == Cyc_Absyn_NonNull_to_Null){
const char*_tmpC86;void*_tmpC85[1];struct Cyc_String_pa_PrintArg_struct _tmpC84;(_tmpC84.tag=0,((_tmpC84.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpC85[0]=& _tmpC84,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpC86="conversion mis-classified as null-check: %s",_tag_dyneither(_tmpC86,sizeof(char),44))),_tag_dyneither(_tmpC85,sizeof(void*),1)))))));}
if(Cyc_Toc_is_toplevel(nv)){
# 3004
if((_tmp5EE  && !(_tmp5E6.elt_tq).real_const) && !_tmp5EF)
# 3007
_tmp5F7=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp5F7,Cyc_Absyn_uint_exp(1,0),0);
# 3009
e->r=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp5F7,_tmp527))->r;}else{
# 3011
struct Cyc_Absyn_Exp*_tmp60D=Cyc_Toc__tag_dyneither_e;
# 3013
if(_tmp5EE){
# 3018
struct _tuple1*_tmp60E=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp60F=Cyc_Absyn_var_exp(_tmp60E,0);
struct Cyc_Absyn_Exp*arg3;
# 3023
{void*_tmp610=_tmp527->r;void*_tmp611=_tmp610;_LL2B3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp612=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp612->tag != 0)goto _LL2B5;else{if(((_tmp612->f1).String_c).tag != 8)goto _LL2B5;}}_LL2B4:
# 3025
 arg3=_tmp5F7;goto _LL2B2;_LL2B5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp613=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp611;if(_tmp613->tag != 0)goto _LL2B7;else{if(((_tmp613->f1).Wstring_c).tag != 9)goto _LL2B7;}}_LL2B6:
# 3027
 arg3=_tmp5F7;goto _LL2B2;_LL2B7:;_LL2B8:
# 3029
{struct Cyc_Absyn_Exp*_tmpC87[2];arg3=Cyc_Absyn_fncall_exp(Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp527),(
# 3031
(_tmpC87[1]=_tmp5F7,((_tmpC87[0]=Cyc_Toc_cast_it(Cyc_Absyn_void_star_typ(),_tmp60F),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC87,sizeof(struct Cyc_Absyn_Exp*),2)))))),0);}
goto _LL2B2;_LL2B2:;}
# 3034
if(!_tmp5EF  && !(_tmp5E6.elt_tq).real_const)
# 3037
arg3=Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,arg3,Cyc_Absyn_uint_exp(1,0),0);{
# 3039
struct Cyc_Absyn_Exp*_tmp615=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E6.elt_typ),0);
struct Cyc_Absyn_Exp*_tmpC88[3];struct Cyc_Absyn_Exp*_tmp616=Cyc_Absyn_fncall_exp(_tmp60D,((_tmpC88[2]=arg3,((_tmpC88[1]=_tmp615,((_tmpC88[0]=_tmp60F,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC88,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
struct Cyc_Absyn_Stmt*_tmp617=Cyc_Absyn_exp_stmt(_tmp616,0);
_tmp617=Cyc_Absyn_declare_stmt(_tmp60E,Cyc_Toc_typ_to_c(old_t2),_tmp527,_tmp617,0);
e->r=Cyc_Toc_stmt_exp_r(_tmp617);};}else{
# 3046
struct Cyc_Absyn_Exp*_tmpC89[3];e->r=Cyc_Toc_fncall_exp_r(_tmp60D,(
(_tmpC89[2]=_tmp5F7,((_tmpC89[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E6.elt_typ),0),((_tmpC89[0]=_tmp527,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC89,sizeof(struct Cyc_Absyn_Exp*),3)))))))));}}
# 3052
goto _LL2A9;_LL2AE:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5F9=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F1.f1;if(_tmp5F9->tag != 0)goto _LL2B0;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp5FA=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp5F1.f2;if(_tmp5FA->tag != 1)goto _LL2B0;else{_tmp5FB=_tmp5FA->f1;}};_LL2AF:
# 3054
 if(!Cyc_Evexp_c_can_eval(_tmp5FB)){
const char*_tmpC8C;void*_tmpC8B;(_tmpC8B=0,Cyc_Tcutil_terr(e->loc,((_tmpC8C="cannot perform coercion since numelts cannot be determined statically.",_tag_dyneither(_tmpC8C,sizeof(char),71))),_tag_dyneither(_tmpC8B,sizeof(void*),0)));}
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC8F;void*_tmpC8E;(_tmpC8E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC8F="can't coerce t? to t* or t@ at the top-level",_tag_dyneither(_tmpC8F,sizeof(char),45))),_tag_dyneither(_tmpC8E,sizeof(void*),0)));}{
# 3066 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp61E=_tmp5FB;
if(_tmp5EE  && !_tmp5EF)
_tmp61E=Cyc_Absyn_add_exp(_tmp5FB,Cyc_Absyn_uint_exp(1,0),0);{
# 3073
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmpC90[3];struct Cyc_Absyn_Exp*_tmp620=Cyc_Absyn_fncall_exp(_tmp61F,(
(_tmpC90[2]=_tmp61E,((_tmpC90[1]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E4.elt_typ),0),((_tmpC90[0]=_tmp527,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC90,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);
# 3078
if(_tmp5EB){
struct Cyc_List_List*_tmpC91;_tmp620->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_null_e,(
(_tmpC91=_cycalloc(sizeof(*_tmpC91)),((_tmpC91->hd=Cyc_Absyn_copy_exp(_tmp620),((_tmpC91->tl=0,_tmpC91)))))));}
e->r=Cyc_Toc_cast_it_r(*_tmp526,_tmp620);
goto _LL2A9;};};_LL2B0:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FC=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F1.f1;if(_tmp5FC->tag != 0)goto _LL2A9;}{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp5FD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp5F1.f2;if(_tmp5FD->tag != 0)goto _LL2A9;};_LL2B1:
# 3086
 DynCast:
 if((_tmp5EE  && !_tmp5EF) && !(_tmp5E6.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC94;void*_tmpC93;(_tmpC93=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC94="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel",_tag_dyneither(_tmpC94,sizeof(char),70))),_tag_dyneither(_tmpC93,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmp625=Cyc_Toc__dyneither_ptr_decrease_size_e;
struct Cyc_Absyn_Exp*_tmpC95[3];e->r=Cyc_Toc_fncall_exp_r(_tmp625,(
(_tmpC95[2]=
Cyc_Absyn_uint_exp(1,0),((_tmpC95[1]=
# 3092
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp5E4.elt_typ),0),((_tmpC95[0]=_tmp527,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpC95,sizeof(struct Cyc_Absyn_Exp*),3)))))))));};}
# 3095
goto _LL2A9;_LL2A9:;}
# 3097
goto _LL2A2;}_LL2A5:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5E7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E2.f1;if(_tmp5E7->tag != 5)goto _LL2A7;else{_tmp5E8=_tmp5E7->f1;}}{struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5E9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5E2.f2;if(_tmp5E9->tag != 6)goto _LL2A7;};_LL2A6:
# 3099
{void*_tmp628=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp5E8.ptr_atts).bounds);void*_tmp629=_tmp628;_LL2BA: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp62A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp629;if(_tmp62A->tag != 0)goto _LL2BC;}_LL2BB:
# 3101
 _tmp527->r=Cyc_Toc_aggrmember_exp_r(Cyc_Absyn_new_exp(_tmp527->r,_tmp527->loc),Cyc_Toc_curr_sp);goto _LL2B9;_LL2BC:;_LL2BD:
 goto _LL2B9;_LL2B9:;}
# 3104
goto _LL2A2;_LL2A7:;_LL2A8:
# 3106
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp527->r;
goto _LL2A2;_LL2A2:;}
# 3110
goto _LL210;}_LL231: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp52A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp52A->tag != 14)goto _LL233;else{_tmp52B=_tmp52A->f1;}}_LL232:
# 3114
{void*_tmp62C=_tmp52B->r;void*_tmp62D=_tmp62C;struct _tuple1*_tmp62F;struct Cyc_List_List*_tmp630;struct Cyc_List_List*_tmp631;struct Cyc_List_List*_tmp633;_LL2BF: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp62E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp62E->tag != 28)goto _LL2C1;else{_tmp62F=_tmp62E->f1;_tmp630=_tmp62E->f2;_tmp631=_tmp62E->f3;}}_LL2C0:
# 3116
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC99;void*_tmpC98[1];struct Cyc_String_pa_PrintArg_struct _tmpC97;(_tmpC97.tag=0,((_tmpC97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp52B->loc)),((_tmpC98[0]=& _tmpC97,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC99="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC99,sizeof(char),42))),_tag_dyneither(_tmpC98,sizeof(void*),1)))))));}{
struct Cyc_Absyn_Exp*_tmp637=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp52B->topt),_tmp630,1,0,_tmp631,_tmp62F);
e->r=_tmp637->r;
e->topt=_tmp637->topt;
goto _LL2BE;};_LL2C1: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp632=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp62D;if(_tmp632->tag != 23)goto _LL2C3;else{_tmp633=_tmp632->f1;}}_LL2C2:
# 3124
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpC9D;void*_tmpC9C[1];struct Cyc_String_pa_PrintArg_struct _tmpC9B;(_tmpC9B.tag=0,((_tmpC9B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(_tmp52B->loc)),((_tmpC9C[0]=& _tmpC9B,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpC9D="%s: & on non-identifiers at the top-level",_tag_dyneither(_tmpC9D,sizeof(char),42))),_tag_dyneither(_tmpC9C,sizeof(void*),1)))))));}
e->r=(Cyc_Toc_init_tuple(nv,1,0,_tmp633))->r;
goto _LL2BE;_LL2C3:;_LL2C4:
# 3130
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp52B);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp52B)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp52B,0,Cyc_Toc_address_lvalue,1);
# 3136
e->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),_tmp52B);}
# 3138
goto _LL2BE;_LL2BE:;}
# 3140
goto _LL210;_LL233: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp52C=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp52C->tag != 15)goto _LL235;else{_tmp52D=_tmp52C->f1;_tmp52E=_tmp52C->f2;}}_LL234:
# 3143
 if(Cyc_Toc_is_toplevel(nv)){
const char*_tmpCA1;void*_tmpCA0[1];struct Cyc_String_pa_PrintArg_struct _tmpC9F;(_tmpC9F.tag=0,((_tmpC9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(_tmp52E->loc)),((_tmpCA0[0]=& _tmpC9F,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpCA1="%s: new at top-level",_tag_dyneither(_tmpCA1,sizeof(char),21))),_tag_dyneither(_tmpCA0,sizeof(void*),1)))))));}{
void*new_e_type=(void*)_check_null(_tmp52E->topt);
{void*_tmp63E=_tmp52E->r;void*_tmp63F=_tmp63E;struct Cyc_List_List*_tmp641;struct Cyc_Absyn_Vardecl*_tmp643;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp645;int _tmp646;struct Cyc_Absyn_Exp*_tmp648;void*_tmp649;int _tmp64A;struct _tuple1*_tmp64C;struct Cyc_List_List*_tmp64D;struct Cyc_List_List*_tmp64E;struct Cyc_Absyn_Aggrdecl*_tmp64F;struct Cyc_List_List*_tmp651;_LL2C6: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp640=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp640->tag != 25)goto _LL2C8;else{_tmp641=_tmp640->f1;}}_LL2C7: {
# 3151
struct _tuple1*_tmp652=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp653=Cyc_Absyn_var_exp(_tmp652,0);
struct Cyc_Absyn_Stmt*_tmp654=Cyc_Toc_init_array(nv,new_e_type,_tmp653,_tmp641,Cyc_Absyn_exp_stmt(_tmp653,0));
void*old_elt_typ;
{void*_tmp655=Cyc_Tcutil_compress(old_typ);void*_tmp656=_tmp655;void*_tmp658;struct Cyc_Absyn_Tqual _tmp659;union Cyc_Absyn_Constraint*_tmp65A;_LL2D3: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp657=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp656;if(_tmp657->tag != 5)goto _LL2D5;else{_tmp658=(_tmp657->f1).elt_typ;_tmp659=(_tmp657->f1).elt_tq;_tmp65A=((_tmp657->f1).ptr_atts).zero_term;}}_LL2D4:
# 3157
 old_elt_typ=_tmp658;goto _LL2D2;_LL2D5:;_LL2D6: {
# 3159
const char*_tmpCA4;void*_tmpCA3;old_elt_typ=(
(_tmpCA3=0,((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCA4="exp_to_c:new array expression doesn't have ptr type",_tag_dyneither(_tmpCA4,sizeof(char),52))),_tag_dyneither(_tmpCA3,sizeof(void*),0))));}_LL2D2:;}{
# 3162
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
void*_tmp65D=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp65E=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(elt_typ,0),
Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp641),0),0);
struct Cyc_Absyn_Exp*e1;
if(_tmp52D == 0  || Cyc_Absyn_no_regions)
e1=Cyc_Toc_malloc_exp(old_elt_typ,_tmp65E);else{
# 3170
struct Cyc_Absyn_Exp*r=_tmp52D;
Cyc_Toc_exp_to_c(nv,r);
e1=Cyc_Toc_rmalloc_exp(r,_tmp65E);}
# 3175
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp652,_tmp65D,e1,_tmp654,0));
goto _LL2C5;};}_LL2C8: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp642=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp642->tag != 26)goto _LL2CA;else{_tmp643=_tmp642->f1;_tmp644=_tmp642->f2;_tmp645=_tmp642->f3;_tmp646=_tmp642->f4;}}_LL2C9:
# 3179
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp52D,old_typ,_tmp644,(void*)_check_null(_tmp645->topt),_tmp646,_tmp645,_tmp643);
goto _LL2C5;_LL2CA: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp647=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp647->tag != 27)goto _LL2CC;else{_tmp648=_tmp647->f1;_tmp649=(void*)_tmp647->f2;_tmp64A=_tmp647->f3;}}_LL2CB:
# 3183
 Cyc_Toc_new_comprehension_to_c(nv,e,new_e_type,_tmp52D,old_typ,_tmp648,_tmp649,_tmp64A,0,0);
goto _LL2C5;_LL2CC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp64B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp64B->tag != 28)goto _LL2CE;else{_tmp64C=_tmp64B->f1;_tmp64D=_tmp64B->f2;_tmp64E=_tmp64B->f3;_tmp64F=_tmp64B->f4;}}_LL2CD: {
# 3188
struct Cyc_Absyn_Exp*_tmp65F=Cyc_Toc_init_struct(nv,(void*)_check_null(_tmp52E->topt),_tmp64D,1,_tmp52D,_tmp64E,_tmp64C);
e->r=_tmp65F->r;
e->topt=_tmp65F->topt;
goto _LL2C5;}_LL2CE: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp650=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp63F;if(_tmp650->tag != 23)goto _LL2D0;else{_tmp651=_tmp650->f1;}}_LL2CF:
# 3194
 e->r=(Cyc_Toc_init_tuple(nv,1,_tmp52D,_tmp651))->r;
goto _LL2C5;_LL2D0:;_LL2D1: {
# 3201
void*old_elt_typ=(void*)_check_null(_tmp52E->topt);
void*elt_typ=Cyc_Toc_typ_to_c(old_elt_typ);
# 3204
struct _tuple1*_tmp660=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp661=Cyc_Absyn_var_exp(_tmp660,0);
struct Cyc_Absyn_Exp*mexp=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp661,0),0);
struct Cyc_Absyn_Exp*inner_mexp=mexp;
if(_tmp52D == 0  || Cyc_Absyn_no_regions)
mexp=Cyc_Toc_malloc_exp(old_elt_typ,mexp);else{
# 3211
struct Cyc_Absyn_Exp*r=_tmp52D;
Cyc_Toc_exp_to_c(nv,r);
mexp=Cyc_Toc_rmalloc_exp(r,mexp);}{
# 3219
int done=0;
{void*_tmp662=_tmp52E->r;void*_tmp663=_tmp662;void*_tmp665;struct Cyc_Absyn_Exp*_tmp666;_LL2D8: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp664=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp663;if(_tmp664->tag != 13)goto _LL2DA;else{_tmp665=(void*)_tmp664->f1;_tmp666=_tmp664->f2;}}_LL2D9:
# 3222
{struct _tuple29 _tmpCA5;struct _tuple29 _tmp667=(_tmpCA5.f1=Cyc_Tcutil_compress(_tmp665),((_tmpCA5.f2=Cyc_Tcutil_compress((void*)_check_null(_tmp666->topt)),_tmpCA5)));struct _tuple29 _tmp668=_tmp667;void*_tmp66A;union Cyc_Absyn_Constraint*_tmp66B;union Cyc_Absyn_Constraint*_tmp66D;_LL2DD:{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp669=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668.f1;if(_tmp669->tag != 5)goto _LL2DF;else{_tmp66A=(_tmp669->f1).elt_typ;_tmp66B=((_tmp669->f1).ptr_atts).bounds;}}{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp66C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668.f2;if(_tmp66C->tag != 5)goto _LL2DF;else{_tmp66D=((_tmp66C->f1).ptr_atts).bounds;}};_LL2DE:
# 3225
{struct _tuple29 _tmpCA6;struct _tuple29 _tmp66E=(_tmpCA6.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66B),((_tmpCA6.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp66D),_tmpCA6)));struct _tuple29 _tmp66F=_tmp66E;struct Cyc_Absyn_Exp*_tmp672;_LL2E2:{struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp670=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp66F.f1;if(_tmp670->tag != 0)goto _LL2E4;}{struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp671=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp66F.f2;if(_tmp671->tag != 1)goto _LL2E4;else{_tmp672=_tmp671->f1;}};_LL2E3:
# 3227
 Cyc_Toc_exp_to_c(nv,_tmp666);
inner_mexp->r=Cyc_Toc_sizeoftyp_exp_r(elt_typ);
done=1;{
struct Cyc_Absyn_Exp*_tmp673=Cyc_Toc__init_dyneither_ptr_e;
{struct Cyc_Absyn_Exp*_tmpCA7[4];e->r=Cyc_Toc_fncall_exp_r(_tmp673,(
(_tmpCA7[3]=_tmp672,((_tmpCA7[2]=
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp66A),0),((_tmpCA7[1]=_tmp666,((_tmpCA7[0]=mexp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCA7,sizeof(struct Cyc_Absyn_Exp*),4)))))))))));}
# 3235
goto _LL2E1;};_LL2E4:;_LL2E5:
 goto _LL2E1;_LL2E1:;}
# 3238
goto _LL2DC;_LL2DF:;_LL2E0:
 goto _LL2DC;_LL2DC:;}
# 3241
goto _LL2D7;_LL2DA:;_LL2DB:
 goto _LL2D7;_LL2D7:;}
# 3244
if(!done){
struct Cyc_Absyn_Stmt*_tmp677=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(_tmp661),0);
struct Cyc_Absyn_Exp*_tmp678=Cyc_Absyn_signed_int_exp(0,0);
Cyc_Toc_exp_to_c(nv,_tmp52E);
_tmp677=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(Cyc_Absyn_subscript_exp(_tmp661,_tmp678,0),_tmp52E,0),_tmp677,0);{
# 3250
void*_tmp679=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp660,_tmp679,mexp,_tmp677,0));};}
# 3253
goto _LL2C5;};}_LL2C5:;}
# 3255
goto _LL210;};_LL235: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp52F=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp52F->tag != 17)goto _LL237;else{_tmp530=_tmp52F->f1;}}_LL236: {
# 3258
int _tmp67A=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp530);
Cyc_Toc_set_in_sizeof(nv,_tmp67A);
goto _LL210;}_LL237: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp531=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp531->tag != 16)goto _LL239;else{_tmp532=(void*)_tmp531->f1;}}_LL238:
{struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpCAA;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmpCA9;e->r=(void*)((_tmpCA9=_cycalloc(sizeof(*_tmpCA9)),((_tmpCA9[0]=((_tmpCAA.tag=16,((_tmpCAA.f1=Cyc_Toc_typ_to_c(_tmp532),_tmpCAA)))),_tmpCA9))));}goto _LL210;_LL239: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp533=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp533->tag != 18)goto _LL23B;else{_tmp534=(void*)_tmp533->f1;_tmp535=_tmp533->f2;}}_LL23A: {
# 3264
void*_tmp67D=_tmp534;
struct Cyc_List_List*_tmp67E=_tmp535;
for(0;_tmp67E != 0;_tmp67E=_tmp67E->tl){
void*_tmp67F=(void*)_tmp67E->hd;void*_tmp680=_tmp67F;struct _dyneither_ptr*_tmp682;unsigned int _tmp684;_LL2E7: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp681=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp680;if(_tmp681->tag != 0)goto _LL2E9;else{_tmp682=_tmp681->f1;}}_LL2E8:
 goto _LL2E6;_LL2E9: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp683=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp680;if(_tmp683->tag != 1)goto _LL2E6;else{_tmp684=_tmp683->f1;}}_LL2EA:
# 3270
{void*_tmp685=Cyc_Tcutil_compress(_tmp67D);void*_tmp686=_tmp685;union Cyc_Absyn_AggrInfoU _tmp688;struct Cyc_List_List*_tmp68A;struct Cyc_List_List*_tmp68C;struct Cyc_Absyn_Datatypefield*_tmp68E;_LL2EC: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp687=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp686;if(_tmp687->tag != 11)goto _LL2EE;else{_tmp688=(_tmp687->f1).aggr_info;}}_LL2ED: {
# 3272
struct Cyc_Absyn_Aggrdecl*_tmp68F=Cyc_Absyn_get_known_aggrdecl(_tmp688);
if(_tmp68F->impl == 0){
const char*_tmpCAD;void*_tmpCAC;(_tmpCAC=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCAD="struct fields must be known",_tag_dyneither(_tmpCAD,sizeof(char),28))),_tag_dyneither(_tmpCAC,sizeof(void*),0)));}
_tmp68A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68F->impl))->fields;goto _LL2EF;}_LL2EE: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp689=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp686;if(_tmp689->tag != 12)goto _LL2F0;else{_tmp68A=_tmp689->f2;}}_LL2EF: {
# 3277
struct Cyc_Absyn_Aggrfield*_tmp692=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp68A,(int)_tmp684);
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCB0;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCAF;_tmp67E->hd=(void*)((void*)((_tmpCAF=_cycalloc(sizeof(*_tmpCAF)),((_tmpCAF[0]=((_tmpCB0.tag=0,((_tmpCB0.f1=_tmp692->name,_tmpCB0)))),_tmpCAF)))));}
_tmp67D=_tmp692->type;
goto _LL2EB;}_LL2F0: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp68B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp686;if(_tmp68B->tag != 10)goto _LL2F2;else{_tmp68C=_tmp68B->f1;}}_LL2F1:
# 3282
{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCB3;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCB2;_tmp67E->hd=(void*)((void*)((_tmpCB2=_cycalloc(sizeof(*_tmpCB2)),((_tmpCB2[0]=((_tmpCB3.tag=0,((_tmpCB3.f1=Cyc_Absyn_fieldname((int)(_tmp684 + 1)),_tmpCB3)))),_tmpCB2)))));}
_tmp67D=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp68C,(int)_tmp684)).f2;
goto _LL2EB;_LL2F2: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp68D=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp686;if(_tmp68D->tag != 4)goto _LL2F4;else{if((((_tmp68D->f1).field_info).KnownDatatypefield).tag != 2)goto _LL2F4;_tmp68E=((struct _tuple2)(((_tmp68D->f1).field_info).KnownDatatypefield).val).f2;}}_LL2F3:
# 3286
 if(_tmp684 == 0){
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCB6;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCB5;_tmp67E->hd=(void*)((void*)((_tmpCB5=_cycalloc(sizeof(*_tmpCB5)),((_tmpCB5[0]=((_tmpCB6.tag=0,((_tmpCB6.f1=Cyc_Toc_tag_sp,_tmpCB6)))),_tmpCB5)))));}else{
# 3289
_tmp67D=(*((struct _tuple11*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp68E->typs,(int)(_tmp684 - 1))).f2;{
struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct _tmpCB9;struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmpCB8;_tmp67E->hd=(void*)((void*)((_tmpCB8=_cycalloc(sizeof(*_tmpCB8)),((_tmpCB8[0]=((_tmpCB9.tag=0,((_tmpCB9.f1=Cyc_Absyn_fieldname((int)_tmp684),_tmpCB9)))),_tmpCB8)))));};}
# 3292
goto _LL2EB;_LL2F4:;_LL2F5:
{const char*_tmpCBC;void*_tmpCBB;(_tmpCBB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCBC="impossible type for offsetof tuple index",_tag_dyneither(_tmpCBC,sizeof(char),41))),_tag_dyneither(_tmpCBB,sizeof(void*),0)));}
goto _LL2EB;_LL2EB:;}
# 3296
goto _LL2E6;_LL2E6:;}
# 3299
{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpCBF;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmpCBE;e->r=(void*)((_tmpCBE=_cycalloc(sizeof(*_tmpCBE)),((_tmpCBE[0]=((_tmpCBF.tag=18,((_tmpCBF.f1=Cyc_Toc_typ_to_c(_tmp534),((_tmpCBF.f2=_tmp535,_tmpCBF)))))),_tmpCBE))));}
goto _LL210;}_LL23B: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp536=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp536->tag != 19)goto _LL23D;else{_tmp537=_tmp536->f1;}}_LL23C: {
# 3302
int _tmp69F=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6A0=Cyc_Tcutil_compress((void*)_check_null(_tmp537->topt));
{void*_tmp6A1=_tmp6A0;void*_tmp6A3;struct Cyc_Absyn_Tqual _tmp6A4;void*_tmp6A5;union Cyc_Absyn_Constraint*_tmp6A6;union Cyc_Absyn_Constraint*_tmp6A7;union Cyc_Absyn_Constraint*_tmp6A8;_LL2F7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6A2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6A1;if(_tmp6A2->tag != 5)goto _LL2F9;else{_tmp6A3=(_tmp6A2->f1).elt_typ;_tmp6A4=(_tmp6A2->f1).elt_tq;_tmp6A5=((_tmp6A2->f1).ptr_atts).rgn;_tmp6A6=((_tmp6A2->f1).ptr_atts).nullable;_tmp6A7=((_tmp6A2->f1).ptr_atts).bounds;_tmp6A8=((_tmp6A2->f1).ptr_atts).zero_term;}}_LL2F8:
# 3307
{void*_tmp6A9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6A7);void*_tmp6AA=_tmp6A9;struct Cyc_Absyn_Exp*_tmp6AC;_LL2FC: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6AB=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6AA;if(_tmp6AB->tag != 1)goto _LL2FE;else{_tmp6AC=_tmp6AB->f1;}}_LL2FD: {
# 3309
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp537);
Cyc_Toc_exp_to_c(nv,_tmp537);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCC2;void*_tmpCC1;(_tmpCC1=0,Cyc_Tcutil_warn(e->loc,((_tmpCC2="inserted null check due to dereference",_tag_dyneither(_tmpCC2,sizeof(char),39))),_tag_dyneither(_tmpCC1,sizeof(void*),0)));}{
# 3315
struct Cyc_List_List*_tmpCC3;_tmp537->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(_tmp6A0),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCC3=_cycalloc(sizeof(*_tmpCC3)),((_tmpCC3->hd=Cyc_Absyn_copy_exp(_tmp537),((_tmpCC3->tl=0,_tmpCC3)))))),0));};}
# 3323
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6A8)){
struct _tuple12 _tmp6B1=Cyc_Evexp_eval_const_uint_exp(_tmp6AC);unsigned int _tmp6B3;int _tmp6B4;struct _tuple12 _tmp6B2=_tmp6B1;_tmp6B3=_tmp6B2.f1;_tmp6B4=_tmp6B2.f2;
# 3329
if(!_tmp6B4  || _tmp6B3 <= 0){
const char*_tmpCC6;void*_tmpCC5;(_tmpCC5=0,Cyc_Tcutil_terr(e->loc,((_tmpCC6="cannot determine dereference is in bounds",_tag_dyneither(_tmpCC6,sizeof(char),42))),_tag_dyneither(_tmpCC5,sizeof(void*),0)));}}
# 3332
goto _LL2FB;}_LL2FE: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6AD=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6AA;if(_tmp6AD->tag != 0)goto _LL2FB;}_LL2FF: {
# 3335
struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Absyn_uint_exp(0,0);
_tmp6B7->topt=Cyc_Absyn_uint_typ;
e->r=Cyc_Toc_subscript_exp_r(_tmp537,_tmp6B7);
Cyc_Toc_exp_to_c(nv,e);
goto _LL2FB;}_LL2FB:;}
# 3341
goto _LL2F6;_LL2F9:;_LL2FA: {
const char*_tmpCC9;void*_tmpCC8;(_tmpCC8=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCC9="exp_to_c: Deref: non-pointer",_tag_dyneither(_tmpCC9,sizeof(char),29))),_tag_dyneither(_tmpCC8,sizeof(void*),0)));}_LL2F6:;}
# 3344
Cyc_Toc_set_lhs(nv,_tmp69F);
goto _LL210;};}_LL23D: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp538=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp538->tag != 20)goto _LL23F;else{_tmp539=_tmp538->f1;_tmp53A=_tmp538->f2;_tmp53B=_tmp538->f3;_tmp53C=_tmp538->f4;}}_LL23E: {
# 3347
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp539->topt);
Cyc_Toc_exp_to_c(nv,_tmp539);
if(_tmp53B  && _tmp53C)
e->r=Cyc_Toc_check_tagged_union(_tmp539,Cyc_Toc_typ_to_c(e1_cyc_type),e1_cyc_type,_tmp53A,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);
# 3355
if(is_poly)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
goto _LL210;}_LL23F: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp53D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp53D->tag != 21)goto _LL241;else{_tmp53E=_tmp53D->f1;_tmp53F=_tmp53D->f2;_tmp540=_tmp53D->f3;_tmp541=_tmp53D->f4;}}_LL240: {
# 3359
int _tmp6BA=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp53E->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp53E);
Cyc_Toc_exp_to_c(nv,_tmp53E);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp6BB=Cyc_Toc_get_ptr_type(e1typ);void*_tmp6BD;struct Cyc_Absyn_Tqual _tmp6BE;void*_tmp6BF;union Cyc_Absyn_Constraint*_tmp6C0;union Cyc_Absyn_Constraint*_tmp6C1;union Cyc_Absyn_Constraint*_tmp6C2;struct Cyc_Absyn_PtrInfo _tmp6BC=_tmp6BB;_tmp6BD=_tmp6BC.elt_typ;_tmp6BE=_tmp6BC.elt_tq;_tmp6BF=(_tmp6BC.ptr_atts).rgn;_tmp6C0=(_tmp6BC.ptr_atts).nullable;_tmp6C1=(_tmp6BC.ptr_atts).bounds;_tmp6C2=(_tmp6BC.ptr_atts).zero_term;
{void*_tmp6C3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6C1);void*_tmp6C4=_tmp6C3;struct Cyc_Absyn_Exp*_tmp6C6;_LL301: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6C5=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6C4;if(_tmp6C5->tag != 1)goto _LL303;else{_tmp6C6=_tmp6C5->f1;}}_LL302: {
# 3370
struct _tuple12 _tmp6C8=Cyc_Evexp_eval_const_uint_exp(_tmp6C6);unsigned int _tmp6CA;int _tmp6CB;struct _tuple12 _tmp6C9=_tmp6C8;_tmp6CA=_tmp6C9.f1;_tmp6CB=_tmp6C9.f2;
if(_tmp6CB){
if(_tmp6CA < 1){
const char*_tmpCCC;void*_tmpCCB;(_tmpCCB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCCC="exp_to_c:  AggrArrow_e on pointer of size 0",_tag_dyneither(_tmpCCC,sizeof(char),44))),_tag_dyneither(_tmpCCB,sizeof(void*),0)));}
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCCF;void*_tmpCCE;(_tmpCCE=0,Cyc_Tcutil_warn(e->loc,((_tmpCCF="inserted null check due to dereference",_tag_dyneither(_tmpCCF,sizeof(char),39))),_tag_dyneither(_tmpCCE,sizeof(void*),0)));}{
# 3378
struct Cyc_Absyn_Exp*_tmpCD0[1];_tmp53E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCD0[0]=Cyc_Absyn_new_exp(_tmp53E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD0,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}}else{
# 3383
if(!Cyc_Evexp_c_can_eval(_tmp6C6)){
const char*_tmpCD3;void*_tmpCD2;(_tmpCD2=0,Cyc_Tcutil_terr(e->loc,((_tmpCD3="cannot determine pointer dereference in bounds",_tag_dyneither(_tmpCD3,sizeof(char),47))),_tag_dyneither(_tmpCD2,sizeof(void*),0)));}{
# 3387
struct Cyc_Absyn_Exp*_tmpCD4[4];_tmp53E->r=Cyc_Toc_cast_it_r(Cyc_Toc_typ_to_c(e1typ),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCD4[3]=
# 3391
Cyc_Absyn_uint_exp(0,0),((_tmpCD4[2]=
# 3390
Cyc_Absyn_sizeoftyp_exp(_tmp6BD,0),((_tmpCD4[1]=_tmp6C6,((_tmpCD4[0]=
# 3389
Cyc_Absyn_new_exp(_tmp53E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD4,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0));};}
# 3393
goto _LL300;}_LL303: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6C7=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6C4;if(_tmp6C7->tag != 0)goto _LL300;}_LL304: {
# 3396
void*ta1=Cyc_Toc_typ_to_c(_tmp6BD);
{struct Cyc_Absyn_Exp*_tmpCD5[3];_tmp53E->r=Cyc_Toc_cast_it_r(Cyc_Absyn_cstar_typ(ta1,_tmp6BE),
Cyc_Absyn_fncall_exp(Cyc_Toc__check_dyneither_subscript_e,(
(_tmpCD5[2]=
# 3401
Cyc_Absyn_uint_exp(0,0),((_tmpCD5[1]=
# 3400
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCD5[0]=
# 3399
Cyc_Absyn_new_exp(_tmp53E->r,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCD5,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0));}
# 3404
goto _LL300;}_LL300:;}
# 3406
if(_tmp540  && _tmp541)
e->r=Cyc_Toc_check_tagged_union(_tmp53E,Cyc_Toc_typ_to_c(e1typ),_tmp6BD,_tmp53F,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);
# 3409
if(is_poly  && _tmp541)
e->r=(Cyc_Toc_array_to_ptr_cast(Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),Cyc_Absyn_new_exp(e->r,0),0))->r;
Cyc_Toc_set_lhs(nv,_tmp6BA);
goto _LL210;};};}_LL241: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp542=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp542->tag != 22)goto _LL243;else{_tmp543=_tmp542->f1;_tmp544=_tmp542->f2;}}_LL242: {
# 3414
int _tmp6D5=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp6D6=Cyc_Tcutil_compress((void*)_check_null(_tmp543->topt));
# 3418
{void*_tmp6D7=_tmp6D6;struct Cyc_List_List*_tmp6D9;void*_tmp6DB;struct Cyc_Absyn_Tqual _tmp6DC;void*_tmp6DD;union Cyc_Absyn_Constraint*_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DF;union Cyc_Absyn_Constraint*_tmp6E0;_LL306: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6D8=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D7;if(_tmp6D8->tag != 10)goto _LL308;else{_tmp6D9=_tmp6D8->f1;}}_LL307:
# 3421
 Cyc_Toc_exp_to_c(nv,_tmp543);
Cyc_Toc_exp_to_c(nv,_tmp544);{
struct _tuple12 _tmp6E1=Cyc_Evexp_eval_const_uint_exp(_tmp544);unsigned int _tmp6E3;int _tmp6E4;struct _tuple12 _tmp6E2=_tmp6E1;_tmp6E3=_tmp6E2.f1;_tmp6E4=_tmp6E2.f2;
if(!_tmp6E4){
const char*_tmpCD8;void*_tmpCD7;(_tmpCD7=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpCD8="unknown tuple subscript in translation to C",_tag_dyneither(_tmpCD8,sizeof(char),44))),_tag_dyneither(_tmpCD7,sizeof(void*),0)));}
e->r=Cyc_Toc_aggrmember_exp_r(_tmp543,Cyc_Absyn_fieldname((int)(_tmp6E3 + 1)));
goto _LL305;};_LL308: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp6DA=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D7;if(_tmp6DA->tag != 5)goto _LL30A;else{_tmp6DB=(_tmp6DA->f1).elt_typ;_tmp6DC=(_tmp6DA->f1).elt_tq;_tmp6DD=((_tmp6DA->f1).ptr_atts).rgn;_tmp6DE=((_tmp6DA->f1).ptr_atts).nullable;_tmp6DF=((_tmp6DA->f1).ptr_atts).bounds;_tmp6E0=((_tmp6DA->f1).ptr_atts).zero_term;}}_LL309: {
# 3429
struct Cyc_List_List*_tmp6E7=Cyc_Toc_get_relns(_tmp543);
int _tmp6E8=Cyc_Toc_need_null_check(_tmp543);
int _tmp6E9=Cyc_Toc_in_sizeof(nv);
# 3437
int in_bnds=_tmp6E9;
{void*_tmp6EA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6DF);void*_tmp6EB=_tmp6EA;_LL30D:;_LL30E:
# 3440
 in_bnds=in_bnds  || Cyc_Toc_check_bounds(_tmp6D6,_tmp6E7,_tmp543,_tmp544);
if(Cyc_Toc_warn_bounds_checks  && !in_bnds){
const char*_tmpCDC;void*_tmpCDB[1];struct Cyc_String_pa_PrintArg_struct _tmpCDA;(_tmpCDA.tag=0,((_tmpCDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e)),((_tmpCDB[0]=& _tmpCDA,Cyc_Tcutil_warn(e->loc,((_tmpCDC="bounds check necessary for %s",_tag_dyneither(_tmpCDC,sizeof(char),30))),_tag_dyneither(_tmpCDB,sizeof(void*),1)))))));}_LL30C:;}
# 3447
Cyc_Toc_exp_to_c(nv,_tmp543);
Cyc_Toc_exp_to_c(nv,_tmp544);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp6EF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp6DF);void*_tmp6F0=_tmp6EF;struct Cyc_Absyn_Exp*_tmp6F2;_LL310: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp6F1=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp6F0;if(_tmp6F1->tag != 1)goto _LL312;else{_tmp6F2=_tmp6F1->f1;}}_LL311: {
# 3452
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6DE) && _tmp6E8;
void*ta1=Cyc_Toc_typ_to_c(_tmp6DB);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp6DC);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCDF;void*_tmpCDE;(_tmpCDE=0,Cyc_Tcutil_warn(e->loc,((_tmpCDF="inserted null check due to dereference",_tag_dyneither(_tmpCDF,sizeof(char),39))),_tag_dyneither(_tmpCDE,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*_tmpCE0[1];_tmp543->r=Cyc_Toc_cast_it_r(ta2,Cyc_Absyn_fncall_exp(Cyc_Toc__check_null_e,(
(_tmpCE0[0]=Cyc_Absyn_copy_exp(_tmp543),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE0,sizeof(struct Cyc_Absyn_Exp*),1)))),0));};}else{
# 3464
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6E0)){
# 3466
if(!Cyc_Evexp_c_can_eval(_tmp6F2)){
const char*_tmpCE3;void*_tmpCE2;(_tmpCE2=0,Cyc_Tcutil_terr(e->loc,((_tmpCE3="cannot determine subscript is in bounds",_tag_dyneither(_tmpCE3,sizeof(char),40))),_tag_dyneither(_tmpCE2,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp543);
struct Cyc_Absyn_Exp*_tmpCE4[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(function_e,(
(_tmpCE4[2]=_tmp544,((_tmpCE4[1]=_tmp6F2,((_tmpCE4[0]=_tmp543,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCE4,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));};}else{
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp6F2)){
const char*_tmpCE7;void*_tmpCE6;(_tmpCE6=0,Cyc_Tcutil_terr(e->loc,((_tmpCE7="cannot determine subscript is in bounds",_tag_dyneither(_tmpCE7,sizeof(char),40))),_tag_dyneither(_tmpCE6,sizeof(void*),0)));}
if(Cyc_Toc_warn_all_null_deref){
const char*_tmpCEA;void*_tmpCE9;(_tmpCE9=0,Cyc_Tcutil_warn(e->loc,((_tmpCEA="inserted null check due to dereference",_tag_dyneither(_tmpCEA,sizeof(char),39))),_tag_dyneither(_tmpCE9,sizeof(void*),0)));}{
# 3478
struct Cyc_Absyn_Exp*_tmpCEB[4];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(ta2,
Cyc_Absyn_fncall_exp(Cyc_Toc__check_known_subscript_null_e,(
(_tmpCEB[3]=_tmp544,((_tmpCEB[2]=
Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCEB[1]=_tmp6F2,((_tmpCEB[0]=_tmp543,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEB,sizeof(struct Cyc_Absyn_Exp*),4)))))))))),0)));};}else{
# 3484
if(!Cyc_Evexp_c_can_eval(_tmp6F2)){
const char*_tmpCEE;void*_tmpCED;(_tmpCED=0,Cyc_Tcutil_terr(e->loc,((_tmpCEE="cannot determine subscript is in bounds",_tag_dyneither(_tmpCEE,sizeof(char),40))),_tag_dyneither(_tmpCED,sizeof(void*),0)));}{
# 3487
struct Cyc_Absyn_Exp*_tmpCEF[2];_tmp544->r=Cyc_Toc_fncall_exp_r(Cyc_Toc__check_known_subscript_notnull_e,(
(_tmpCEF[1]=Cyc_Absyn_copy_exp(_tmp544),((_tmpCEF[0]=_tmp6F2,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCEF,sizeof(struct Cyc_Absyn_Exp*),2)))))));};}}}}
# 3490
goto _LL30F;}_LL312: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp6F3=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp6F0;if(_tmp6F3->tag != 0)goto _LL30F;}_LL313: {
# 3492
void*ta1=Cyc_Toc_typ_to_c(_tmp6DB);
if(in_bnds){
# 3496
++ Cyc_Toc_bounds_checks_eliminated;
e->r=Cyc_Toc_subscript_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6DC),
Cyc_Toc_member_exp(_tmp543,Cyc_Toc_curr_sp,0)),_tmp544);}else{
# 3501
struct Cyc_Absyn_Exp*_tmp702=Cyc_Toc__check_dyneither_subscript_e;
struct Cyc_Absyn_Exp*_tmpCF0[3];e->r=Cyc_Toc_deref_exp_r(Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(ta1,_tmp6DC),
Cyc_Absyn_fncall_exp(_tmp702,(
(_tmpCF0[2]=_tmp544,((_tmpCF0[1]=Cyc_Absyn_sizeoftyp_exp(ta1,0),((_tmpCF0[0]=_tmp543,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpCF0,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0)));}
# 3507
goto _LL30F;}_LL30F:;}
# 3509
goto _LL305;}_LL30A:;_LL30B: {
const char*_tmpCF3;void*_tmpCF2;(_tmpCF2=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpCF3="exp_to_c: Subscript on non-tuple/array/tuple ptr",_tag_dyneither(_tmpCF3,sizeof(char),49))),_tag_dyneither(_tmpCF2,sizeof(void*),0)));}_LL305:;}
# 3512
Cyc_Toc_set_lhs(nv,_tmp6D5);
goto _LL210;};}_LL243: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp545=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp545->tag != 23)goto _LL245;else{_tmp546=_tmp545->f1;}}_LL244:
# 3515
 if(!Cyc_Toc_is_toplevel(nv))
e->r=(Cyc_Toc_init_tuple(nv,0,0,_tmp546))->r;else{
# 3520
struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp546);
void*_tmp707=Cyc_Toc_add_tuple_type(_tmp706);
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp546 != 0;(_tmp546=_tmp546->tl,i ++)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp546->hd);{
struct _tuple19*_tmpCF6;struct Cyc_List_List*_tmpCF5;dles=((_tmpCF5=_cycalloc(sizeof(*_tmpCF5)),((_tmpCF5->hd=((_tmpCF6=_cycalloc(sizeof(*_tmpCF6)),((_tmpCF6->f1=0,((_tmpCF6->f2=(struct Cyc_Absyn_Exp*)_tmp546->hd,_tmpCF6)))))),((_tmpCF5->tl=dles,_tmpCF5))))));};}}
# 3527
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}
# 3530
goto _LL210;_LL245: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp547=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp547->tag != 25)goto _LL247;else{_tmp548=_tmp547->f1;}}_LL246:
# 3534
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp548);
{struct Cyc_List_List*_tmp70A=_tmp548;for(0;_tmp70A != 0;_tmp70A=_tmp70A->tl){
struct _tuple19*_tmp70B=(struct _tuple19*)_tmp70A->hd;struct Cyc_Absyn_Exp*_tmp70D;struct _tuple19*_tmp70C=_tmp70B;_tmp70D=_tmp70C->f2;
Cyc_Toc_exp_to_c(nv,_tmp70D);}}
# 3539
goto _LL210;_LL247: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp549=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp549->tag != 26)goto _LL249;else{_tmp54A=_tmp549->f1;_tmp54B=_tmp549->f2;_tmp54C=_tmp549->f3;_tmp54D=_tmp549->f4;}}_LL248: {
# 3543
struct _tuple12 _tmp70E=Cyc_Evexp_eval_const_uint_exp(_tmp54B);unsigned int _tmp710;int _tmp711;struct _tuple12 _tmp70F=_tmp70E;_tmp710=_tmp70F.f1;_tmp711=_tmp70F.f2;{
void*_tmp712=Cyc_Toc_typ_to_c((void*)_check_null(_tmp54C->topt));
Cyc_Toc_exp_to_c(nv,_tmp54C);{
struct Cyc_List_List*es=0;
# 3548
if(!Cyc_Toc_is_zero(_tmp54C)){
if(!_tmp711){
const char*_tmpCF9;void*_tmpCF8;(_tmpCF8=0,Cyc_Tcutil_terr(_tmp54B->loc,((_tmpCF9="cannot determine value of constant",_tag_dyneither(_tmpCF9,sizeof(char),35))),_tag_dyneither(_tmpCF8,sizeof(void*),0)));}
{unsigned int i=0;for(0;i < _tmp710;++ i){
struct _tuple19*_tmpCFC;struct Cyc_List_List*_tmpCFB;es=((_tmpCFB=_cycalloc(sizeof(*_tmpCFB)),((_tmpCFB->hd=((_tmpCFC=_cycalloc(sizeof(*_tmpCFC)),((_tmpCFC->f1=0,((_tmpCFC->f2=_tmp54C,_tmpCFC)))))),((_tmpCFB->tl=es,_tmpCFB))))));}}
# 3554
if(_tmp54D){
struct Cyc_Absyn_Exp*_tmp717=Cyc_Toc_cast_it(_tmp712,Cyc_Absyn_uint_exp(0,0));
struct _tuple19*_tmpCFF;struct Cyc_List_List*_tmpCFE;es=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(es,((_tmpCFE=_cycalloc(sizeof(*_tmpCFE)),((_tmpCFE->hd=((_tmpCFF=_cycalloc(sizeof(*_tmpCFF)),((_tmpCFF->f1=0,((_tmpCFF->f2=_tmp717,_tmpCFF)))))),((_tmpCFE->tl=0,_tmpCFE)))))));}}
# 3559
e->r=Cyc_Toc_unresolvedmem_exp_r(0,es);
goto _LL210;};};}_LL249: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp54E=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp54E->tag != 27)goto _LL24B;else{_tmp54F=_tmp54E->f1;_tmp550=(void*)_tmp54E->f2;_tmp551=_tmp54E->f3;}}_LL24A:
# 3564
 e->r=Cyc_Toc_unresolvedmem_exp_r(0,0);
goto _LL210;_LL24B: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp552=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp552->tag != 28)goto _LL24D;else{_tmp553=_tmp552->f1;_tmp554=_tmp552->f2;_tmp555=_tmp552->f3;_tmp556=_tmp552->f4;}}_LL24C:
# 3569
 if(!Cyc_Toc_is_toplevel(nv)){
struct Cyc_Absyn_Exp*_tmp71A=Cyc_Toc_init_struct(nv,old_typ,_tmp554,0,0,_tmp555,_tmp553);
e->r=_tmp71A->r;
e->topt=_tmp71A->topt;}else{
# 3580
if(_tmp556 == 0){
const char*_tmpD02;void*_tmpD01;(_tmpD01=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD02="Aggregate_e: missing aggrdecl pointer",_tag_dyneither(_tmpD02,sizeof(char),38))),_tag_dyneither(_tmpD01,sizeof(void*),0)));}{
struct Cyc_Absyn_Aggrdecl*sd2=_tmp556;
# 3584
struct _RegionHandle _tmp71D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp71D;_push_region(rgn);
{struct Cyc_List_List*_tmp71E=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp555,sd2->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields);
# 3588
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp556->impl))->tagged){
# 3590
struct _tuple30*_tmp71F=(struct _tuple30*)((struct Cyc_List_List*)_check_null(_tmp71E))->hd;struct Cyc_Absyn_Aggrfield*_tmp721;struct Cyc_Absyn_Exp*_tmp722;struct _tuple30*_tmp720=_tmp71F;_tmp721=_tmp720->f1;_tmp722=_tmp720->f2;{
void*_tmp723=(void*)_check_null(_tmp722->topt);
void*_tmp724=_tmp721->type;
Cyc_Toc_exp_to_c(nv,_tmp722);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp724) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp723))
_tmp722->r=
Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp722->r,0));{
# 3599
int i=Cyc_Toc_get_member_offset(_tmp556,_tmp721->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0);
struct _tuple19*_tmpD07;struct _tuple19*_tmpD06;struct _tuple19*_tmpD05[2];struct Cyc_List_List*_tmp725=(_tmpD05[1]=((_tmpD06=_cycalloc(sizeof(*_tmpD06)),((_tmpD06->f1=0,((_tmpD06->f2=_tmp722,_tmpD06)))))),((_tmpD05[0]=((_tmpD07=_cycalloc(sizeof(*_tmpD07)),((_tmpD07->f1=0,((_tmpD07->f2=field_tag_exp,_tmpD07)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD05,sizeof(struct _tuple19*),2)))));
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp725,0);
struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmpD0D;struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpD0C;void*_tmpD0B[1];struct Cyc_List_List*ds=(_tmpD0B[0]=(void*)((_tmpD0D=_cycalloc(sizeof(*_tmpD0D)),((_tmpD0D[0]=((_tmpD0C.tag=1,((_tmpD0C.f1=_tmp721->name,_tmpD0C)))),_tmpD0D)))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0B,sizeof(void*),1)));
struct _tuple19*_tmpD10;struct _tuple19*_tmpD0F[1];struct Cyc_List_List*dles=(_tmpD0F[0]=((_tmpD10=_cycalloc(sizeof(*_tmpD10)),((_tmpD10->f1=ds,((_tmpD10->f2=umem,_tmpD10)))))),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD0F,sizeof(struct _tuple19*),1)));
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);};};}else{
# 3608
struct Cyc_List_List*_tmp72E=0;
struct Cyc_List_List*_tmp72F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(sd2->impl))->fields;
for(0;_tmp72F != 0;_tmp72F=_tmp72F->tl){
struct Cyc_List_List*_tmp730=_tmp71E;for(0;_tmp730 != 0;_tmp730=_tmp730->tl){
if((*((struct _tuple30*)_tmp730->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp72F->hd){
struct _tuple30*_tmp731=(struct _tuple30*)_tmp730->hd;struct Cyc_Absyn_Aggrfield*_tmp733;struct Cyc_Absyn_Exp*_tmp734;struct _tuple30*_tmp732=_tmp731;_tmp733=_tmp732->f1;_tmp734=_tmp732->f2;{
void*_tmp735=Cyc_Toc_typ_to_c(_tmp733->type);
void*_tmp736=Cyc_Toc_typ_to_c((void*)_check_null(_tmp734->topt));
Cyc_Toc_exp_to_c(nv,_tmp734);
# 3618
if(!Cyc_Tcutil_unify(_tmp735,_tmp736)){
# 3620
if(!Cyc_Tcutil_is_arithmetic_type(_tmp735) || !
Cyc_Tcutil_is_arithmetic_type(_tmp736))
_tmp734=Cyc_Toc_cast_it(_tmp735,Cyc_Absyn_copy_exp(_tmp734));}
{struct _tuple19*_tmpD13;struct Cyc_List_List*_tmpD12;_tmp72E=((_tmpD12=_cycalloc(sizeof(*_tmpD12)),((_tmpD12->hd=((_tmpD13=_cycalloc(sizeof(*_tmpD13)),((_tmpD13->f1=0,((_tmpD13->f2=_tmp734,_tmpD13)))))),((_tmpD12->tl=_tmp72E,_tmpD12))))));}
break;};}}}
# 3627
e->r=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72E));}}
# 3585
;_pop_region(rgn);};}
# 3631
goto _LL210;_LL24D: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp557=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp557->tag != 29)goto _LL24F;else{_tmp558=(void*)_tmp557->f1;_tmp559=_tmp557->f2;}}_LL24E: {
# 3633
struct Cyc_List_List*fs;
{void*_tmp739=Cyc_Tcutil_compress(_tmp558);void*_tmp73A=_tmp739;struct Cyc_List_List*_tmp73C;_LL315: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp73B=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp73A;if(_tmp73B->tag != 12)goto _LL317;else{_tmp73C=_tmp73B->f2;}}_LL316:
 fs=_tmp73C;goto _LL314;_LL317:;_LL318: {
const char*_tmpD17;void*_tmpD16[1];struct Cyc_String_pa_PrintArg_struct _tmpD15;(_tmpD15.tag=0,((_tmpD15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp558)),((_tmpD16[0]=& _tmpD15,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD17="anon struct has type %s",_tag_dyneither(_tmpD17,sizeof(char),24))),_tag_dyneither(_tmpD16,sizeof(void*),1)))))));}_LL314:;}{
# 3638
struct _RegionHandle _tmp740=_new_region("rgn");struct _RegionHandle*rgn=& _tmp740;_push_region(rgn);{
struct Cyc_List_List*_tmp741=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,e->loc,_tmp559,Cyc_Absyn_StructA,fs);
for(0;_tmp741 != 0;_tmp741=_tmp741->tl){
struct _tuple30*_tmp742=(struct _tuple30*)_tmp741->hd;struct Cyc_Absyn_Aggrfield*_tmp744;struct Cyc_Absyn_Exp*_tmp745;struct _tuple30*_tmp743=_tmp742;_tmp744=_tmp743->f1;_tmp745=_tmp743->f2;{
void*_tmp746=(void*)_check_null(_tmp745->topt);
void*_tmp747=_tmp744->type;
Cyc_Toc_exp_to_c(nv,_tmp745);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp747) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp746))
_tmp745->r=Cyc_Toc_cast_it_r(Cyc_Absyn_void_star_typ(),Cyc_Absyn_new_exp(_tmp745->r,0));};}
# 3652
e->r=Cyc_Toc_unresolvedmem_exp_r(0,_tmp559);}
# 3654
_npop_handler(0);goto _LL210;
# 3638
;_pop_region(rgn);};}_LL24F: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp55A=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp55A->tag != 30)goto _LL251;else{_tmp55B=_tmp55A->f1;_tmp55C=_tmp55A->f2;_tmp55D=_tmp55A->f3;}}_LL250: {
# 3657
void*datatype_ctype;
struct Cyc_Absyn_Exp*tag_exp;
struct _tuple1*_tmp748=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp749=Cyc_Absyn_var_exp(_tmp748,0);
struct Cyc_Absyn_Exp*mem_exp;
datatype_ctype=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp55D->name,_tmp55C->name));
tag_exp=_tmp55C->is_extensible?Cyc_Absyn_var_exp(_tmp55D->name,0):
 Cyc_Toc_datatype_tag(_tmp55C,_tmp55D->name);
mem_exp=_tmp749;{
struct Cyc_List_List*_tmp74A=_tmp55D->typs;
# 3668
if(Cyc_Toc_is_toplevel(nv)){
# 3670
struct Cyc_List_List*dles=0;
for(0;_tmp55B != 0;(_tmp55B=_tmp55B->tl,_tmp74A=_tmp74A->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp55B->hd;
void*_tmp74B=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp74A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp74B))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct _tuple19*_tmpD1A;struct Cyc_List_List*_tmpD19;dles=((_tmpD19=_cycalloc(sizeof(*_tmpD19)),((_tmpD19->hd=((_tmpD1A=_cycalloc(sizeof(*_tmpD1A)),((_tmpD1A->f1=0,((_tmpD1A->f2=cur_e,_tmpD1A)))))),((_tmpD19->tl=dles,_tmpD19))))));};}
# 3681
{struct _tuple19*_tmpD1D;struct Cyc_List_List*_tmpD1C;dles=((_tmpD1C=_cycalloc(sizeof(*_tmpD1C)),((_tmpD1C->hd=((_tmpD1D=_cycalloc(sizeof(*_tmpD1D)),((_tmpD1D->f1=0,((_tmpD1D->f2=tag_exp,_tmpD1D)))))),((_tmpD1C->tl=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles),_tmpD1C))))));}
e->r=Cyc_Toc_unresolvedmem_exp_r(0,dles);}else{
# 3687
struct Cyc_List_List*_tmpD1E;struct Cyc_List_List*_tmp750=
(_tmpD1E=_cycalloc(sizeof(*_tmpD1E)),((_tmpD1E->hd=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Toc_tag_sp,0),tag_exp,0),((_tmpD1E->tl=0,_tmpD1E)))));
# 3690
{int i=1;for(0;_tmp55B != 0;(((_tmp55B=_tmp55B->tl,i ++)),_tmp74A=_tmp74A->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp55B->hd;
void*_tmp751=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp74A))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp751))
cur_e=Cyc_Toc_cast_it(field_typ,cur_e);{
struct Cyc_Absyn_Stmt*_tmp752=Cyc_Absyn_assign_stmt(Cyc_Toc_member_exp(mem_exp,Cyc_Absyn_fieldname(i),0),cur_e,0);
# 3700
struct Cyc_List_List*_tmpD1F;_tmp750=((_tmpD1F=_cycalloc(sizeof(*_tmpD1F)),((_tmpD1F->hd=_tmp752,((_tmpD1F->tl=_tmp750,_tmpD1F))))));};}}{
# 3702
struct Cyc_Absyn_Stmt*_tmp754=Cyc_Absyn_exp_stmt(_tmp749,0);
struct Cyc_List_List*_tmpD20;struct Cyc_Absyn_Stmt*_tmp755=Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((_tmpD20=_cycalloc(sizeof(*_tmpD20)),((_tmpD20->hd=_tmp754,((_tmpD20->tl=_tmp750,_tmpD20))))))),0);
e->r=Cyc_Toc_stmt_exp_r(Cyc_Absyn_declare_stmt(_tmp748,datatype_ctype,0,_tmp755,0));};}
# 3706
goto _LL210;};}_LL251: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp55E=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp55E->tag != 31)goto _LL253;}_LL252:
# 3708
 goto _LL254;_LL253: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp55F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp55F->tag != 32)goto _LL255;}_LL254:
 goto _LL210;_LL255: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp560=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp560->tag != 33)goto _LL257;else{_tmp561=(_tmp560->f1).is_calloc;_tmp562=(_tmp560->f1).rgn;_tmp563=(_tmp560->f1).elt_type;_tmp564=(_tmp560->f1).num_elts;_tmp565=(_tmp560->f1).fat_result;}}_LL256: {
# 3712
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp563)));
Cyc_Toc_exp_to_c(nv,_tmp564);
# 3716
if(_tmp565){
struct _tuple1*_tmp758=Cyc_Toc_temp_var();
struct _tuple1*_tmp759=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp561){
xexp=_tmp564;
if(_tmp562 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp562;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp758,0));}else{
# 3727
pexp=Cyc_Toc_calloc_exp(*_tmp563,Cyc_Absyn_sizeoftyp_exp(t_c,0),Cyc_Absyn_var_exp(_tmp758,0));}{
# 3729
struct Cyc_Absyn_Exp*_tmpD21[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,(
(_tmpD21[2]=
Cyc_Absyn_var_exp(_tmp758,0),((_tmpD21[1]=
# 3730
Cyc_Absyn_sizeoftyp_exp(t_c,0),((_tmpD21[0]=Cyc_Absyn_var_exp(_tmp759,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD21,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}else{
# 3733
if(_tmp562 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp562;
Cyc_Toc_exp_to_c(nv,rgn);
pexp=Cyc_Toc_rmalloc_exp(rgn,Cyc_Absyn_var_exp(_tmp758,0));}else{
# 3738
pexp=Cyc_Toc_malloc_exp(*_tmp563,Cyc_Absyn_var_exp(_tmp758,0));}{
# 3740
struct Cyc_Absyn_Exp*_tmpD22[3];rexp=Cyc_Absyn_fncall_exp(Cyc_Toc__tag_dyneither_e,((_tmpD22[2]=
Cyc_Absyn_var_exp(_tmp758,0),((_tmpD22[1]=
# 3740
Cyc_Absyn_uint_exp(1,0),((_tmpD22[0]=Cyc_Absyn_var_exp(_tmp759,0),((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD22,sizeof(struct Cyc_Absyn_Exp*),3)))))))),0);};}{
# 3743
struct Cyc_Absyn_Stmt*_tmp75C=Cyc_Absyn_declare_stmt(_tmp758,Cyc_Absyn_uint_typ,_tmp564,
Cyc_Absyn_declare_stmt(_tmp759,Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq),pexp,
Cyc_Absyn_exp_stmt(rexp,0),0),0);
e->r=Cyc_Toc_stmt_exp_r(_tmp75C);};}else{
# 3748
if(_tmp561){
if(_tmp562 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp562;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rcalloc_exp(rgn,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp564))->r;}else{
# 3754
e->r=(Cyc_Toc_calloc_exp(*_tmp563,Cyc_Absyn_sizeoftyp_exp(t_c,0),_tmp564))->r;}}else{
# 3757
if(_tmp562 != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp562;
Cyc_Toc_exp_to_c(nv,rgn);
e->r=(Cyc_Toc_rmalloc_exp(rgn,_tmp564))->r;}else{
# 3762
e->r=(Cyc_Toc_malloc_exp(*_tmp563,_tmp564))->r;}}}
# 3766
goto _LL210;}_LL257: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp566=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp566->tag != 34)goto _LL259;else{_tmp567=_tmp566->f1;_tmp568=_tmp566->f2;}}_LL258: {
# 3775
void*e1_old_typ=(void*)_check_null(_tmp567->topt);
void*e2_old_typ=(void*)_check_null(_tmp568->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ)){
const char*_tmpD25;void*_tmpD24;(_tmpD24=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD25="Swap_e: is_pointer_or_boxed: not a pointer or boxed type",_tag_dyneither(_tmpD25,sizeof(char),57))),_tag_dyneither(_tmpD24,sizeof(void*),0)));}{
# 3781
struct Cyc_Absyn_Exp*swap_fn;
if(Cyc_Tcutil_is_dyneither_ptr(e1_old_typ))
swap_fn=Cyc_Toc__swap_dyneither_e;else{
# 3785
swap_fn=Cyc_Toc__swap_word_e;}{
# 3789
int f1_tag=0;
void*tagged_mem_type1=(void*)& Cyc_Absyn_VoidType_val;
int e1_tmem=Cyc_Toc_is_tagged_union_project(_tmp567,& f1_tag,& tagged_mem_type1,1);
int f2_tag=0;
void*tagged_mem_type2=(void*)& Cyc_Absyn_VoidType_val;
int e2_tmem=Cyc_Toc_is_tagged_union_project(_tmp568,& f2_tag,& tagged_mem_type2,1);
# 3796
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp567);
Cyc_Toc_exp_to_c(nv,_tmp568);
Cyc_Toc_set_lhs(nv,0);
# 3802
if(e1_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp567,f1_tag,tagged_mem_type1);else{
# 3806
_tmp567=Cyc_Toc_push_address_exp(_tmp567);}
# 3808
if(e2_tmem)
Cyc_Toc_add_tagged_union_check_for_swap(_tmp568,f2_tag,tagged_mem_type2);else{
# 3812
_tmp568=Cyc_Toc_push_address_exp(_tmp568);}
# 3814
{struct Cyc_Absyn_Exp*_tmpD26[2];e->r=Cyc_Toc_fncall_exp_r(swap_fn,((_tmpD26[1]=_tmp568,((_tmpD26[0]=_tmp567,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD26,sizeof(struct Cyc_Absyn_Exp*),2)))))));}
# 3816
goto _LL210;};};}_LL259: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp569=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp569->tag != 37)goto _LL25B;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp569->f2;}}_LL25A: {
# 3819
void*_tmp760=Cyc_Tcutil_compress((void*)_check_null(_tmp56A->topt));
Cyc_Toc_exp_to_c(nv,_tmp56A);
{void*_tmp761=_tmp760;struct Cyc_Absyn_Aggrdecl*_tmp763;_LL31A: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp762=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp761;if(_tmp762->tag != 11)goto _LL31C;else{if((((_tmp762->f1).aggr_info).KnownAggr).tag != 2)goto _LL31C;_tmp763=*((struct Cyc_Absyn_Aggrdecl**)(((_tmp762->f1).aggr_info).KnownAggr).val);}}_LL31B: {
# 3823
struct Cyc_Absyn_Exp*_tmp764=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp763,_tmp56B),0);
struct Cyc_Absyn_Exp*_tmp765=Cyc_Toc_member_exp(_tmp56A,_tmp56B,0);
struct Cyc_Absyn_Exp*_tmp766=Cyc_Toc_member_exp(_tmp765,Cyc_Toc_tag_sp,0);
e->r=(Cyc_Absyn_eq_exp(_tmp766,_tmp764,0))->r;
goto _LL319;}_LL31C:;_LL31D: {
const char*_tmpD29;void*_tmpD28;(_tmpD28=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD29="non-aggregate type in tagcheck",_tag_dyneither(_tmpD29,sizeof(char),31))),_tag_dyneither(_tmpD28,sizeof(void*),0)));}_LL319:;}
# 3830
goto _LL210;}_LL25B: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp56C=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp56C->tag != 36)goto _LL25D;else{_tmp56D=_tmp56C->f1;}}_LL25C:
 Cyc_Toc_stmt_to_c(nv,_tmp56D);goto _LL210;_LL25D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp56E=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp56E->tag != 35)goto _LL25F;}_LL25E: {
const char*_tmpD2C;void*_tmpD2B;(_tmpD2B=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD2C="UnresolvedMem",_tag_dyneither(_tmpD2C,sizeof(char),14))),_tag_dyneither(_tmpD2B,sizeof(void*),0)));}_LL25F: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp56F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp56F->tag != 24)goto _LL261;}_LL260: {
const char*_tmpD2F;void*_tmpD2E;(_tmpD2E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpD2F="compoundlit",_tag_dyneither(_tmpD2F,sizeof(char),12))),_tag_dyneither(_tmpD2E,sizeof(void*),0)));}_LL261: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp570=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp570->tag != 38)goto _LL263;}_LL262: {
const char*_tmpD32;void*_tmpD31;(_tmpD31=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD32="valueof(-)",_tag_dyneither(_tmpD32,sizeof(char),11))),_tag_dyneither(_tmpD31,sizeof(void*),0)));}_LL263: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp571=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp4F8;if(_tmp571->tag != 39)goto _LL210;}_LL264: {
const char*_tmpD35;void*_tmpD34;(_tmpD34=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD35="__asm__",_tag_dyneither(_tmpD35,sizeof(char),8))),_tag_dyneither(_tmpD34,sizeof(void*),0)));}_LL210:;};}
# 3867 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_if_neq_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Stmt*fail_stmt){
return Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_neq_exp(e1,e2,0),fail_stmt,
# 3870
Cyc_Toc_skip_stmt_dl(),0);}
# 3873
static int Cyc_Toc_path_length(struct Cyc_Absyn_Exp*e){
void*_tmp771=e->r;void*_tmp772=_tmp771;struct Cyc_Absyn_Exp*_tmp774;struct Cyc_Absyn_Exp*_tmp776;struct Cyc_Absyn_Exp*_tmp778;_LL31F: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp773=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp772;if(_tmp773->tag != 13)goto _LL321;else{_tmp774=_tmp773->f2;}}_LL320:
 return Cyc_Toc_path_length(_tmp774);_LL321: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp775=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp772;if(_tmp775->tag != 19)goto _LL323;else{_tmp776=_tmp775->f1;}}_LL322:
 return 1 + Cyc_Toc_path_length(_tmp776);_LL323: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp777=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp772;if(_tmp777->tag != 20)goto _LL325;else{_tmp778=_tmp777->f1;}}_LL324:
 return Cyc_Toc_path_length(_tmp778);_LL325:;_LL326:
 return 0;_LL31E:;}struct _tuple31{struct Cyc_Toc_Env*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple32{struct _tuple1*f1;void*f2;};struct _tuple33{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3882
static struct _tuple31 Cyc_Toc_xlate_pat(struct Cyc_Toc_Env*nv,struct _RegionHandle*rgn,void*t,struct Cyc_Absyn_Exp*r,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Stmt*fail_stmt,struct Cyc_List_List*decls){
# 3887
struct Cyc_Absyn_Stmt*s;
{void*_tmp779=p->r;void*_tmp77A=_tmp779;struct _tuple1*_tmp77C;struct _tuple1*_tmp77E;struct Cyc_Absyn_Pat*_tmp77F;struct _tuple1*_tmp781;struct Cyc_Absyn_Vardecl*_tmp784;struct Cyc_Absyn_Pat*_tmp785;enum Cyc_Absyn_Sign _tmp788;int _tmp789;char _tmp78B;struct _dyneither_ptr _tmp78D;int _tmp78E;struct Cyc_Absyn_Enumdecl*_tmp790;struct Cyc_Absyn_Enumfield*_tmp791;void*_tmp793;struct Cyc_Absyn_Enumfield*_tmp794;struct Cyc_Absyn_Datatypedecl*_tmp797;struct Cyc_Absyn_Datatypefield*_tmp798;struct Cyc_List_List*_tmp799;struct Cyc_List_List*_tmp79B;struct Cyc_List_List*_tmp79D;union Cyc_Absyn_AggrInfoU _tmp7A0;struct Cyc_List_List*_tmp7A1;struct Cyc_Absyn_Pat*_tmp7A3;_LL328: {struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp77B=(struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp77B->tag != 2)goto _LL32A;else{_tmp77C=(_tmp77B->f2)->name;}}_LL329: {
# 3890
struct Cyc_Absyn_Pat*_tmp7A7=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);
_tmp7A7->topt=p->topt;
_tmp77E=_tmp77C;_tmp77F=_tmp7A7;goto _LL32B;}_LL32A: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp77D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp77D->tag != 1)goto _LL32C;else{_tmp77E=(_tmp77D->f1)->name;_tmp77F=_tmp77D->f2;}}_LL32B: {
# 3895
struct _tuple1*v=Cyc_Toc_temp_var();
void*_tmp7A8=(void*)_check_null(_tmp77F->topt);
{struct _tuple32*_tmpD38;struct Cyc_List_List*_tmpD37;decls=((_tmpD37=_region_malloc(rgn,sizeof(*_tmpD37)),((_tmpD37->hd=((_tmpD38=_region_malloc(rgn,sizeof(*_tmpD38)),((_tmpD38->f1=v,((_tmpD38->f2=Cyc_Toc_typ_to_c_array(_tmp7A8),_tmpD38)))))),((_tmpD37->tl=decls,_tmpD37))))));}{
struct _tuple31 _tmp7AB=Cyc_Toc_xlate_pat(Cyc_Toc_add_varmap(rgn,nv,_tmp77E,Cyc_Absyn_var_exp(v,0)),rgn,_tmp7A8,
Cyc_Absyn_var_exp(v,0),path,_tmp77F,fail_stmt,decls);
nv=_tmp7AB.f1;
decls=_tmp7AB.f2;{
struct Cyc_Absyn_Stmt*_tmp7AC=_tmp7AB.f3;
struct Cyc_Absyn_Stmt*_tmp7AD=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(v,0),r,0);
s=Cyc_Absyn_seq_stmt(_tmp7AD,_tmp7AC,0);
goto _LL327;};};}_LL32C: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp780=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp780->tag != 4)goto _LL32E;else{_tmp781=(_tmp780->f2)->name;}}_LL32D: {
# 3908
struct _tuple1*_tmp7AE=Cyc_Toc_temp_var();
void*_tmp7AF=(void*)_check_null(p->topt);
{struct _tuple32*_tmpD3B;struct Cyc_List_List*_tmpD3A;decls=((_tmpD3A=_region_malloc(rgn,sizeof(*_tmpD3A)),((_tmpD3A->hd=((_tmpD3B=_region_malloc(rgn,sizeof(*_tmpD3B)),((_tmpD3B->f1=_tmp7AE,((_tmpD3B->f2=Cyc_Toc_typ_to_c_array(_tmp7AF),_tmpD3B)))))),((_tmpD3A->tl=decls,_tmpD3A))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp781,Cyc_Absyn_var_exp(_tmp7AE,0));
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7AE,0),r,0);
goto _LL327;}_LL32E: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp782=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp782->tag != 0)goto _LL330;}_LL32F:
 s=Cyc_Toc_skip_stmt_dl();goto _LL327;_LL330: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp783=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp783->tag != 3)goto _LL332;else{_tmp784=_tmp783->f1;_tmp785=_tmp783->f2;}}_LL331: {
# 3917
struct _tuple1*_tmp7B2=Cyc_Toc_temp_var();
{struct _tuple32*_tmpD3E;struct Cyc_List_List*_tmpD3D;decls=((_tmpD3D=_region_malloc(rgn,sizeof(*_tmpD3D)),((_tmpD3D->hd=((_tmpD3E=_region_malloc(rgn,sizeof(*_tmpD3E)),((_tmpD3E->f1=_tmp7B2,((_tmpD3E->f2=Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),_tmpD3E)))))),((_tmpD3D->tl=decls,_tmpD3D))))));}
nv=Cyc_Toc_add_varmap(rgn,nv,_tmp784->name,Cyc_Absyn_var_exp(_tmp7B2,0));
# 3921
s=Cyc_Absyn_assign_stmt(Cyc_Absyn_var_exp(_tmp7B2,0),
Cyc_Toc_cast_it(Cyc_Absyn_cstar_typ(Cyc_Toc_typ_to_c_array(t),Cyc_Toc_mt_tq),
Cyc_Toc_push_address_exp(path)),0);{
struct _tuple31 _tmp7B5=Cyc_Toc_xlate_pat(nv,rgn,t,r,path,_tmp785,fail_stmt,decls);
_tmp7B5.f3=Cyc_Absyn_seq_stmt(s,_tmp7B5.f3,0);
return _tmp7B5;};}_LL332: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp786=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp786->tag != 9)goto _LL334;}_LL333:
# 3928
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_signed_int_exp(0,0),fail_stmt);goto _LL327;_LL334: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp787=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp787->tag != 10)goto _LL336;else{_tmp788=_tmp787->f1;_tmp789=_tmp787->f2;}}_LL335:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_int_exp(_tmp788,_tmp789,0),fail_stmt);goto _LL327;_LL336: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp78A=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp78A->tag != 11)goto _LL338;else{_tmp78B=_tmp78A->f1;}}_LL337:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_char_exp(_tmp78B,0),fail_stmt);goto _LL327;_LL338: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp78C=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp78C->tag != 12)goto _LL33A;else{_tmp78D=_tmp78C->f1;_tmp78E=_tmp78C->f2;}}_LL339:
 s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_float_exp(_tmp78D,_tmp78E,0),fail_stmt);goto _LL327;_LL33A: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp78F=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp78F->tag != 13)goto _LL33C;else{_tmp790=_tmp78F->f1;_tmp791=_tmp78F->f2;}}_LL33B:
# 3933
{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct _tmpD41;struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmpD40;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD40=_cycalloc(sizeof(*_tmpD40)),((_tmpD40[0]=((_tmpD41.tag=31,((_tmpD41.f1=_tmp791->name,((_tmpD41.f2=_tmp790,((_tmpD41.f3=_tmp791,_tmpD41)))))))),_tmpD40)))),0),fail_stmt);}
goto _LL327;_LL33C: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp792=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp792->tag != 14)goto _LL33E;else{_tmp793=(void*)_tmp792->f1;_tmp794=_tmp792->f2;}}_LL33D:
# 3936
{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpD44;struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpD43;s=Cyc_Toc_if_neq_stmt(r,Cyc_Absyn_new_exp((void*)((_tmpD43=_cycalloc(sizeof(*_tmpD43)),((_tmpD43[0]=((_tmpD44.tag=32,((_tmpD44.f1=_tmp794->name,((_tmpD44.f2=_tmp793,((_tmpD44.f3=_tmp794,_tmpD44)))))))),_tmpD43)))),0),fail_stmt);}
goto _LL327;_LL33E: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp795=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp795->tag != 6)goto _LL340;else{struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp796=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(_tmp795->f1)->r;if(_tmp796->tag != 8)goto _LL340;else{_tmp797=_tmp796->f1;_tmp798=_tmp796->f2;_tmp799=_tmp796->f3;}}}_LL33F:
# 3946
 s=Cyc_Toc_skip_stmt_dl();{
struct _tuple1*_tmp7BA=Cyc_Toc_temp_var();
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp798->name,_tmp797->name);
void*_tmp7BB=Cyc_Absyn_cstar_typ(Cyc_Absyn_strctq(tufstrct),Cyc_Toc_mt_tq);
int cnt=1;
struct Cyc_Absyn_Exp*rcast=Cyc_Toc_cast_it(_tmp7BB,r);
struct Cyc_List_List*_tmp7BC=_tmp798->typs;
for(0;_tmp799 != 0;(((_tmp799=_tmp799->tl,_tmp7BC=((struct Cyc_List_List*)_check_null(_tmp7BC))->tl)),++ cnt)){
struct Cyc_Absyn_Pat*_tmp7BD=(struct Cyc_Absyn_Pat*)_tmp799->hd;
if(_tmp7BD->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7BE=(*((struct _tuple11*)((struct Cyc_List_List*)_check_null(_tmp7BC))->hd)).f2;
void*_tmp7BF=(void*)_check_null(_tmp7BD->topt);
void*_tmp7C0=Cyc_Toc_typ_to_c_array(_tmp7BF);
struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7BA,0),Cyc_Absyn_fieldname(cnt),0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp7BE)))
_tmp7C1=Cyc_Toc_cast_it(_tmp7C0,_tmp7C1);{
# 3964
struct _tuple31 _tmp7C2=Cyc_Toc_xlate_pat(nv,rgn,_tmp7BF,_tmp7C1,_tmp7C1,_tmp7BD,fail_stmt,decls);
# 3966
nv=_tmp7C2.f1;
decls=_tmp7C2.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7C2.f3,0);};};}{
# 3970
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(Cyc_Absyn_var_exp(_tmp7BA,0),Cyc_Toc_tag_sp,0);
struct Cyc_Absyn_Exp*tag_exp=
_tmp797->is_extensible?Cyc_Absyn_var_exp(_tmp798->name,0): Cyc_Toc_datatype_tag(_tmp797,_tmp798->name);
struct Cyc_Absyn_Exp*test_exp=Cyc_Absyn_neq_exp(temp_tag,tag_exp,0);
s=Cyc_Absyn_ifthenelse_stmt(test_exp,fail_stmt,s,0);
if(Cyc_Toc_is_nullable((void*)_check_null(p->topt)))
# 3978
s=Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(Cyc_Absyn_var_exp(_tmp7BA,0),Cyc_Absyn_uint_exp(0,0),0),fail_stmt,s,0);
# 3981
s=Cyc_Absyn_declare_stmt(_tmp7BA,_tmp7BB,rcast,s,0);
goto _LL327;};};_LL340: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp79A=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp79A->tag != 8)goto _LL342;else{_tmp79B=_tmp79A->f3;}}_LL341:
# 3984
 _tmp79D=_tmp79B;goto _LL343;_LL342: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp79C=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp79C->tag != 5)goto _LL344;else{_tmp79D=_tmp79C->f1;}}_LL343:
# 3992
 s=Cyc_Toc_skip_stmt_dl();{
int cnt=1;
for(0;_tmp79D != 0;(_tmp79D=_tmp79D->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp7C3=(struct Cyc_Absyn_Pat*)_tmp79D->hd;
if(_tmp7C3->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp7C4=(void*)_check_null(_tmp7C3->topt);
struct _dyneither_ptr*_tmp7C5=Cyc_Absyn_fieldname(cnt);
struct _tuple31 _tmp7C6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7C4,Cyc_Toc_member_exp(r,_tmp7C5,0),
Cyc_Toc_member_exp(path,_tmp7C5,0),_tmp7C3,fail_stmt,decls);
nv=_tmp7C6.f1;
decls=_tmp7C6.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7C6.f3,0);};}
# 4006
goto _LL327;};_LL344: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp79E=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp79E->tag != 7)goto _LL346;else{if(_tmp79E->f1 != 0)goto _LL346;}}_LL345: {
# 4008
const char*_tmpD47;void*_tmpD46;(_tmpD46=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD47="unresolved aggregate pattern!",_tag_dyneither(_tmpD47,sizeof(char),30))),_tag_dyneither(_tmpD46,sizeof(void*),0)));}_LL346: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp79F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp79F->tag != 7)goto _LL348;else{if(_tmp79F->f1 == 0)goto _LL348;_tmp7A0=(_tmp79F->f1)->aggr_info;_tmp7A1=_tmp79F->f3;}}_LL347: {
# 4010
struct Cyc_Absyn_Aggrdecl*_tmp7C9=Cyc_Absyn_get_known_aggrdecl(_tmp7A0);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7C9->impl))->tagged){
# 4014
struct _tuple33*_tmp7CA=(struct _tuple33*)((struct Cyc_List_List*)_check_null(_tmp7A1))->hd;struct Cyc_List_List*_tmp7CC;struct Cyc_Absyn_Pat*_tmp7CD;struct _tuple33*_tmp7CB=_tmp7CA;_tmp7CC=_tmp7CB->f1;_tmp7CD=_tmp7CB->f2;{
struct _dyneither_ptr*f;
{void*_tmp7CE=(void*)((struct Cyc_List_List*)_check_null(_tmp7CC))->hd;void*_tmp7CF=_tmp7CE;struct _dyneither_ptr*_tmp7D1;_LL351: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7D0=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7CF;if(_tmp7D0->tag != 1)goto _LL353;else{_tmp7D1=_tmp7D0->f1;}}_LL352:
 f=_tmp7D1;goto _LL350;_LL353:;_LL354: {
const char*_tmpD4A;void*_tmpD49;(_tmpD49=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD4A="no field name in tagged union pattern",_tag_dyneither(_tmpD4A,sizeof(char),38))),_tag_dyneither(_tmpD49,sizeof(void*),0)));}_LL350:;}{
# 4021
void*_tmp7D4=(void*)_check_null(_tmp7CD->topt);
void*_tmp7D5=Cyc_Toc_typ_to_c_array(_tmp7D4);
# 4024
struct Cyc_Absyn_Exp*_tmp7D6=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(path,f,0),Cyc_Toc_val_sp,0);
struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_val_sp,0);
_tmp7D7=Cyc_Toc_cast_it(_tmp7D5,_tmp7D7);
_tmp7D6=Cyc_Toc_cast_it(_tmp7D5,_tmp7D6);{
struct _tuple31 _tmp7D8=Cyc_Toc_xlate_pat(nv,rgn,_tmp7D4,_tmp7D7,_tmp7D6,_tmp7CD,fail_stmt,decls);
# 4032
nv=_tmp7D8.f1;
decls=_tmp7D8.f2;{
struct Cyc_Absyn_Stmt*_tmp7D9=_tmp7D8.f3;
struct Cyc_Absyn_Stmt*_tmp7DA=Cyc_Toc_if_neq_stmt(Cyc_Absyn_aggrmember_exp(Cyc_Toc_member_exp(r,f,0),Cyc_Toc_tag_sp,0),
Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp7C9,f),0),fail_stmt);
# 4040
s=Cyc_Absyn_seq_stmt(_tmp7DA,_tmp7D9,0);};};};};}else{
# 4043
s=Cyc_Toc_skip_stmt_dl();
for(0;_tmp7A1 != 0;_tmp7A1=_tmp7A1->tl){
struct _tuple33*_tmp7DB=(struct _tuple33*)_tmp7A1->hd;
struct Cyc_Absyn_Pat*_tmp7DC=(*_tmp7DB).f2;
if(_tmp7DC->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*f;
{void*_tmp7DD=(void*)((struct Cyc_List_List*)_check_null((*_tmp7DB).f1))->hd;void*_tmp7DE=_tmp7DD;struct _dyneither_ptr*_tmp7E0;_LL356: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp7DF=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp7DE;if(_tmp7DF->tag != 1)goto _LL358;else{_tmp7E0=_tmp7DF->f1;}}_LL357:
 f=_tmp7E0;goto _LL355;_LL358:;_LL359: {
struct Cyc_Toc_Match_error_exn_struct _tmpD4D;struct Cyc_Toc_Match_error_exn_struct*_tmpD4C;(int)_throw((void*)((_tmpD4C=_cycalloc_atomic(sizeof(*_tmpD4C)),((_tmpD4C[0]=((_tmpD4D.tag=Cyc_Toc_Match_error,_tmpD4D)),_tmpD4C)))));}_LL355:;}{
# 4054
void*_tmp7E3=(void*)_check_null(_tmp7DC->topt);
void*_tmp7E4=Cyc_Toc_typ_to_c_array(_tmp7E3);
struct Cyc_Absyn_Exp*_tmp7E5=Cyc_Toc_member_exp(r,f,0);
if(Cyc_Toc_is_void_star_or_boxed_tvar(((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7C9->impl))->fields,f)))->type))
_tmp7E5=Cyc_Toc_cast_it(Cyc_Toc_typ_to_c_array(_tmp7E4),_tmp7E5);{
struct _tuple31 _tmp7E6=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E3,_tmp7E5,
Cyc_Toc_member_exp(path,f,0),_tmp7DC,fail_stmt,decls);
nv=_tmp7E6.f1;
decls=_tmp7E6.f2;
s=Cyc_Absyn_seq_stmt(s,_tmp7E6.f3,0);};};};}}
# 4066
goto _LL327;}_LL348: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp7A2=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp7A2->tag != 6)goto _LL34A;else{_tmp7A3=_tmp7A2->f1;}}_LL349: {
# 4069
void*_tmp7E7=(void*)_check_null(_tmp7A3->topt);
# 4072
struct _tuple31 _tmp7E8=Cyc_Toc_xlate_pat(nv,rgn,_tmp7E7,Cyc_Absyn_deref_exp(r,0),Cyc_Absyn_deref_exp(path,0),_tmp7A3,fail_stmt,decls);
# 4074
nv=_tmp7E8.f1;
decls=_tmp7E8.f2;{
struct Cyc_Absyn_Stmt*_tmp7E9=_tmp7E8.f3;
if(Cyc_Toc_is_nullable(t))
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_eq_exp(r,Cyc_Absyn_signed_int_exp(0,0),0),fail_stmt,
# 4080
Cyc_Toc_skip_stmt_dl(),0),_tmp7E9,0);else{
# 4082
s=_tmp7E9;}
goto _LL327;};}_LL34A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp7A4=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp7A4->tag != 15)goto _LL34C;}_LL34B: {
const char*_tmpD50;void*_tmpD4F;(_tmpD4F=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD50="unknownid pat",_tag_dyneither(_tmpD50,sizeof(char),14))),_tag_dyneither(_tmpD4F,sizeof(void*),0)));}_LL34C: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp7A5=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp7A5->tag != 16)goto _LL34E;}_LL34D: {
const char*_tmpD53;void*_tmpD52;(_tmpD52=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD53="unknowncall pat",_tag_dyneither(_tmpD53,sizeof(char),16))),_tag_dyneither(_tmpD52,sizeof(void*),0)));}_LL34E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp7A6=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp77A;if(_tmp7A6->tag != 17)goto _LL327;}_LL34F: {
const char*_tmpD56;void*_tmpD55;(_tmpD55=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD56="exp pat",_tag_dyneither(_tmpD56,sizeof(char),8))),_tag_dyneither(_tmpD55,sizeof(void*),0)));}_LL327:;}{
# 4088
struct _tuple31 _tmpD57;return(_tmpD57.f1=nv,((_tmpD57.f2=decls,((_tmpD57.f3=s,_tmpD57)))));};}struct _tuple34{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Switch_clause*f3;};
# 4125 "toc.cyc"
static struct _tuple34*Cyc_Toc_gen_label(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 4127
struct _tuple34*_tmpD58;return(_tmpD58=_region_malloc(r,sizeof(*_tmpD58)),((_tmpD58->f1=Cyc_Toc_fresh_label(),((_tmpD58->f2=Cyc_Toc_fresh_label(),((_tmpD58->f3=sc,_tmpD58)))))));}
# 4130
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs){
# 4132
Cyc_Toc_exp_to_c(nv,e);{
void*_tmp7F2=(void*)_check_null(e->topt);
# 4137
int leave_as_switch;
{void*_tmp7F3=Cyc_Tcutil_compress(_tmp7F2);void*_tmp7F4=_tmp7F3;_LL35B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp7F5=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F4;if(_tmp7F5->tag != 6)goto _LL35D;}_LL35C:
 goto _LL35E;_LL35D: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7F6=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp7F4;if(_tmp7F6->tag != 13)goto _LL35F;}_LL35E:
 leave_as_switch=1;goto _LL35A;_LL35F:;_LL360:
 leave_as_switch=0;goto _LL35A;_LL35A:;}
# 4143
{struct Cyc_List_List*_tmp7F7=scs;for(0;_tmp7F7 != 0;_tmp7F7=_tmp7F7->tl){
if((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(((struct Cyc_Absyn_Switch_clause*)_tmp7F7->hd)->pat_vars))->v != 0  || ((struct Cyc_Absyn_Switch_clause*)_tmp7F7->hd)->where_clause != 0){
leave_as_switch=0;
break;}}}
# 4148
if(leave_as_switch){
# 4150
struct _dyneither_ptr*next_l=Cyc_Toc_fresh_label();
{struct Cyc_List_List*_tmp7F8=scs;for(0;_tmp7F8 != 0;_tmp7F8=_tmp7F8->tl){
struct Cyc_Absyn_Stmt*_tmp7F9=((struct Cyc_Absyn_Switch_clause*)_tmp7F8->hd)->body;
((struct Cyc_Absyn_Switch_clause*)_tmp7F8->hd)->body=Cyc_Absyn_label_stmt(next_l,_tmp7F9,0);
next_l=Cyc_Toc_fresh_label();{
struct _RegionHandle _tmp7FA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7FA;_push_region(rgn);
Cyc_Toc_stmt_to_c(Cyc_Toc_switch_as_switch_env(rgn,nv,next_l),_tmp7F9);;_pop_region(rgn);};}}
# 4159
return;}{
# 4162
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*r=Cyc_Absyn_var_exp(v,0);
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4167
struct _RegionHandle _tmp7FB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp7FB;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp7FC=Cyc_Toc_share_env(rgn,nv);
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple34*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_label,rgn,scs);
struct Cyc_List_List*test_stmts=0;
struct Cyc_List_List*nvs=0;
struct Cyc_List_List*decls=0;
# 4178
{struct Cyc_List_List*_tmp7FD=lscs;for(0;_tmp7FD != 0;_tmp7FD=_tmp7FD->tl){
struct Cyc_Absyn_Switch_clause*sc=(*((struct _tuple34*)_tmp7FD->hd)).f3;
struct _dyneither_ptr*fail_lab=_tmp7FD->tl == 0?end_l:(*((struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp7FD->tl))->hd)).f1;
struct _tuple31 _tmp7FE=Cyc_Toc_xlate_pat(_tmp7FC,rgn,_tmp7F2,r,path,sc->pattern,
Cyc_Absyn_goto_stmt(fail_lab,0),decls);
# 4181
struct Cyc_Toc_Env*_tmp800;struct Cyc_List_List*_tmp801;struct Cyc_Absyn_Stmt*_tmp802;struct _tuple31 _tmp7FF=_tmp7FE;_tmp800=_tmp7FF.f1;_tmp801=_tmp7FF.f2;_tmp802=_tmp7FF.f3;
# 4184
if(sc->where_clause != 0){
struct Cyc_Absyn_Exp*_tmp803=(struct Cyc_Absyn_Exp*)_check_null(sc->where_clause);
Cyc_Toc_exp_to_c(_tmp800,_tmp803);
_tmp802=Cyc_Absyn_seq_stmt(_tmp802,Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,_tmp803,0),
Cyc_Absyn_goto_stmt(fail_lab,0),
Cyc_Toc_skip_stmt_dl(),0),0);}
# 4191
decls=_tmp801;
{struct Cyc_List_List*_tmpD59;nvs=((_tmpD59=_region_malloc(rgn,sizeof(*_tmpD59)),((_tmpD59->hd=_tmp800,((_tmpD59->tl=nvs,_tmpD59))))));}{
struct Cyc_List_List*_tmpD5A;test_stmts=((_tmpD5A=_region_malloc(rgn,sizeof(*_tmpD5A)),((_tmpD5A->hd=_tmp802,((_tmpD5A->tl=test_stmts,_tmpD5A))))));};}}
# 4195
nvs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(nvs);
test_stmts=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(test_stmts);{
struct Cyc_List_List*stmts=0;
# 4200
for(0;lscs != 0;
(((lscs=lscs->tl,nvs=nvs->tl)),test_stmts=test_stmts->tl)){
struct _tuple34*_tmp806=(struct _tuple34*)lscs->hd;struct _dyneither_ptr*_tmp808;struct _dyneither_ptr*_tmp809;struct Cyc_Absyn_Switch_clause*_tmp80A;struct _tuple34*_tmp807=_tmp806;_tmp808=_tmp807->f1;_tmp809=_tmp807->f2;_tmp80A=_tmp807->f3;{
struct Cyc_Toc_Env*_tmp80B=(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs))->hd;
struct Cyc_Absyn_Stmt*s=_tmp80A->body;
struct _RegionHandle _tmp80C=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp80C;_push_region(rgn2);
if(lscs->tl != 0){
struct _tuple34*_tmp80D=(struct _tuple34*)((struct Cyc_List_List*)_check_null(lscs->tl))->hd;struct _dyneither_ptr*_tmp80F;struct Cyc_Absyn_Switch_clause*_tmp810;struct _tuple34*_tmp80E=_tmp80D;_tmp80F=_tmp80E->f2;_tmp810=_tmp80E->f3;{
struct Cyc_List_List*_tmp811=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp810->pat_vars))->v)).f1);
_tmp811=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp811);
Cyc_Toc_stmt_to_c(Cyc_Toc_non_last_switchclause_env(rgn2,_tmp80B,end_l,_tmp80F,_tmp811,(struct Cyc_Toc_Env*)((struct Cyc_List_List*)_check_null(nvs->tl))->hd),s);};}else{
# 4215
Cyc_Toc_stmt_to_c(Cyc_Toc_last_switchclause_env(rgn2,_tmp80B,end_l),s);}
# 4217
s=Cyc_Absyn_seq_stmt(Cyc_Absyn_label_stmt(_tmp808,(struct Cyc_Absyn_Stmt*)((struct Cyc_List_List*)_check_null(test_stmts))->hd,0),
Cyc_Absyn_label_stmt(_tmp809,s,0),0);{
struct Cyc_List_List*_tmpD5B;stmts=((_tmpD5B=_region_malloc(rgn,sizeof(*_tmpD5B)),((_tmpD5B->hd=s,((_tmpD5B->tl=stmts,_tmpD5B))))));};
# 4205
;_pop_region(rgn2);};}{
# 4221
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(Cyc_Absyn_seq_stmts(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(stmts),0),
Cyc_Absyn_label_stmt(end_l,Cyc_Toc_skip_stmt_dl(),0),0);
# 4224
for(decls;decls != 0;decls=decls->tl){
struct _tuple32*_tmp813=(struct _tuple32*)decls->hd;struct _tuple1*_tmp815;void*_tmp816;struct _tuple32*_tmp814=_tmp813;_tmp815=_tmp814->f1;_tmp816=_tmp814->f2;
res=Cyc_Absyn_declare_stmt(_tmp815,_tmp816,0,res,0);}
# 4229
whole_s->r=(Cyc_Absyn_declare_stmt(v,Cyc_Toc_typ_to_c((void*)_check_null(e->topt)),e,res,0))->r;};};}
# 4168
;_pop_region(rgn);};};}
# 4234
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 4240
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 4242
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
struct Cyc_List_List*_tmpD5C;return Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__npop_handler_e,(
(_tmpD5C=_cycalloc(sizeof(*_tmpD5C)),((_tmpD5C->hd=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0),((_tmpD5C->tl=0,_tmpD5C)))))),0),0);}
# 4246
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
s->r=Cyc_Toc_seq_stmt_r(Cyc_Toc_make_npop_handler(n),Cyc_Absyn_new_stmt(s->r,0));}struct _tuple35{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 4251
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 4253
while(1){
void*_tmp818=s->r;void*_tmp819=_tmp818;struct Cyc_Absyn_Exp*_tmp81C;struct Cyc_Absyn_Stmt*_tmp81E;struct Cyc_Absyn_Stmt*_tmp81F;struct Cyc_Absyn_Exp*_tmp821;struct Cyc_Absyn_Exp*_tmp823;struct Cyc_Absyn_Stmt*_tmp824;struct Cyc_Absyn_Stmt*_tmp825;struct Cyc_Absyn_Exp*_tmp827;struct Cyc_Absyn_Stmt*_tmp828;struct Cyc_Absyn_Stmt*_tmp82A;struct Cyc_Absyn_Stmt*_tmp82C;struct Cyc_Absyn_Stmt*_tmp82E;struct Cyc_Absyn_Exp*_tmp830;struct Cyc_Absyn_Exp*_tmp831;struct Cyc_Absyn_Exp*_tmp832;struct Cyc_Absyn_Stmt*_tmp833;struct Cyc_Absyn_Exp*_tmp835;struct Cyc_List_List*_tmp836;struct Cyc_List_List*_tmp838;struct Cyc_Absyn_Switch_clause**_tmp839;struct Cyc_Absyn_Decl*_tmp83B;struct Cyc_Absyn_Stmt*_tmp83C;struct _dyneither_ptr*_tmp83E;struct Cyc_Absyn_Stmt*_tmp83F;struct Cyc_Absyn_Stmt*_tmp841;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_Absyn_Stmt*_tmp844;struct Cyc_List_List*_tmp845;struct Cyc_Absyn_Exp*_tmp847;_LL362: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp81A=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp81A->tag != 0)goto _LL364;}_LL363:
# 4256
 return;_LL364: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp81B=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp81B->tag != 1)goto _LL366;else{_tmp81C=_tmp81B->f1;}}_LL365:
# 4258
 Cyc_Toc_exp_to_c(nv,_tmp81C);
return;_LL366: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp81D=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp81D->tag != 2)goto _LL368;else{_tmp81E=_tmp81D->f1;_tmp81F=_tmp81D->f2;}}_LL367:
# 4261
 Cyc_Toc_stmt_to_c(nv,_tmp81E);
s=_tmp81F;
continue;_LL368: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp820=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp820->tag != 3)goto _LL36A;else{_tmp821=_tmp820->f1;}}_LL369: {
# 4265
void*topt=0;
if(_tmp821 != 0){
topt=Cyc_Toc_typ_to_c((void*)_check_null(_tmp821->topt));
Cyc_Toc_exp_to_c(nv,_tmp821);}
# 4271
if(s->try_depth > 0){
if(topt != 0){
struct _tuple1*_tmp848=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp849=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp848,0),0);
s->r=(Cyc_Absyn_declare_stmt(_tmp848,topt,_tmp821,
Cyc_Absyn_seq_stmt(Cyc_Toc_make_npop_handler(s->try_depth),_tmp849,0),0))->r;}else{
# 4279
Cyc_Toc_do_npop_before(s->try_depth,s);}}
# 4281
return;}_LL36A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp822=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp822->tag != 4)goto _LL36C;else{_tmp823=_tmp822->f1;_tmp824=_tmp822->f2;_tmp825=_tmp822->f3;}}_LL36B:
# 4283
 Cyc_Toc_exp_to_c(nv,_tmp823);
Cyc_Toc_stmt_to_c(nv,_tmp824);
s=_tmp825;
continue;_LL36C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp826=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp826->tag != 5)goto _LL36E;else{_tmp827=(_tmp826->f1).f1;_tmp828=_tmp826->f2;}}_LL36D:
# 4288
 Cyc_Toc_exp_to_c(nv,_tmp827);{
struct _RegionHandle _tmp84A=_new_region("temp");struct _RegionHandle*temp=& _tmp84A;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp828);
# 4292
_npop_handler(0);return;
# 4289
;_pop_region(temp);};_LL36E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp829=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp829->tag != 6)goto _LL370;else{_tmp82A=_tmp829->f1;}}_LL36F: {
# 4294
struct _dyneither_ptr**_tmp84C;struct Cyc_Toc_Env*_tmp84B=nv;_tmp84C=_tmp84B->break_lab;
if(_tmp84C != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp84C,0);{
# 4298
int dest_depth=_tmp82A == 0?0: _tmp82A->try_depth;
Cyc_Toc_do_npop_before(s->try_depth - dest_depth,s);
return;};}_LL370: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp82B=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp82B->tag != 7)goto _LL372;else{_tmp82C=_tmp82B->f1;}}_LL371: {
# 4302
struct _dyneither_ptr**_tmp84E;struct Cyc_Toc_Env*_tmp84D=nv;_tmp84E=_tmp84D->continue_lab;
if(_tmp84E != 0)
s->r=Cyc_Toc_goto_stmt_r(*_tmp84E,0);
_tmp82E=_tmp82C;goto _LL373;}_LL372: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp82D=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp82D->tag != 8)goto _LL374;else{_tmp82E=_tmp82D->f2;}}_LL373:
# 4308
 Cyc_Toc_do_npop_before(s->try_depth - ((struct Cyc_Absyn_Stmt*)_check_null(_tmp82E))->try_depth,s);
return;_LL374: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp82F=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp82F->tag != 9)goto _LL376;else{_tmp830=_tmp82F->f1;_tmp831=(_tmp82F->f2).f1;_tmp832=(_tmp82F->f3).f1;_tmp833=_tmp82F->f4;}}_LL375:
# 4312
 Cyc_Toc_exp_to_c(nv,_tmp830);Cyc_Toc_exp_to_c(nv,_tmp831);Cyc_Toc_exp_to_c(nv,_tmp832);{
struct _RegionHandle _tmp84F=_new_region("temp");struct _RegionHandle*temp=& _tmp84F;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp833);
# 4316
_npop_handler(0);return;
# 4313
;_pop_region(temp);};_LL376: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp834=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp834->tag != 10)goto _LL378;else{_tmp835=_tmp834->f1;_tmp836=_tmp834->f2;}}_LL377:
# 4318
 Cyc_Toc_xlate_switch(nv,s,_tmp835,_tmp836);
return;_LL378: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp837=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp837->tag != 11)goto _LL37A;else{_tmp838=_tmp837->f1;_tmp839=_tmp837->f2;}}_LL379: {
# 4321
struct Cyc_Toc_FallthruInfo*_tmp851;struct Cyc_Toc_Env*_tmp850=nv;_tmp851=_tmp850->fallthru_info;
if(_tmp851 == 0){
const char*_tmpD5F;void*_tmpD5E;(_tmpD5E=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpD5F="fallthru in unexpected place",_tag_dyneither(_tmpD5F,sizeof(char),29))),_tag_dyneither(_tmpD5E,sizeof(void*),0)));}{
struct Cyc_Toc_FallthruInfo _tmp854=*_tmp851;struct _dyneither_ptr*_tmp856;struct Cyc_List_List*_tmp857;struct Cyc_Dict_Dict _tmp858;struct Cyc_Toc_FallthruInfo _tmp855=_tmp854;_tmp856=_tmp855.label;_tmp857=_tmp855.binders;_tmp858=_tmp855.next_case_env;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp856,0);
# 4327
Cyc_Toc_do_npop_before(s->try_depth - ((*((struct Cyc_Absyn_Switch_clause**)_check_null(_tmp839)))->body)->try_depth,s2);{
struct Cyc_List_List*_tmp859=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp857);
struct Cyc_List_List*_tmp85A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp838);
for(0;_tmp859 != 0;(_tmp859=_tmp859->tl,_tmp85A=_tmp85A->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp85A))->hd);
s2=Cyc_Absyn_seq_stmt(Cyc_Absyn_assign_stmt(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup)(_tmp858,(struct _tuple1*)_tmp859->hd),(struct Cyc_Absyn_Exp*)_tmp85A->hd,0),s2,0);}
# 4335
s->r=s2->r;
return;};};};}_LL37A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp83A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp83A->tag != 12)goto _LL37C;else{_tmp83B=_tmp83A->f1;_tmp83C=_tmp83A->f2;}}_LL37B:
# 4341
{void*_tmp85B=_tmp83B->r;void*_tmp85C=_tmp85B;struct Cyc_Absyn_Vardecl*_tmp85E;struct Cyc_Absyn_Pat*_tmp860;struct Cyc_Absyn_Exp*_tmp861;struct Cyc_List_List*_tmp863;struct Cyc_Absyn_Fndecl*_tmp865;struct Cyc_Absyn_Tvar*_tmp867;struct Cyc_Absyn_Vardecl*_tmp868;int _tmp869;_LL385: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp85D=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp85C;if(_tmp85D->tag != 0)goto _LL387;else{_tmp85E=_tmp85D->f1;}}_LL386: {
# 4343
struct _RegionHandle _tmp86A=_new_region("temp");struct _RegionHandle*temp=& _tmp86A;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD62;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD61;struct Cyc_Toc_Env*_tmp86B=Cyc_Toc_add_varmap(temp,nv,_tmp85E->name,
Cyc_Absyn_varb_exp(_tmp85E->name,(void*)((_tmpD61=_cycalloc(sizeof(*_tmpD61)),((_tmpD61[0]=((_tmpD62.tag=4,((_tmpD62.f1=_tmp85E,_tmpD62)))),_tmpD61)))),0));
Cyc_Toc_local_decl_to_c(_tmp86B,_tmp86B,_tmp85E,_tmp83C);}
# 4348
_npop_handler(0);goto _LL384;
# 4343
;_pop_region(temp);}_LL387: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp85F=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp85C;if(_tmp85F->tag != 2)goto _LL389;else{_tmp860=_tmp85F->f1;_tmp861=_tmp85F->f3;}}_LL388:
# 4352
{void*_tmp86E=_tmp860->r;void*_tmp86F=_tmp86E;struct Cyc_Absyn_Vardecl*_tmp871;_LL392: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp870=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp86F;if(_tmp870->tag != 1)goto _LL394;else{_tmp871=_tmp870->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp872=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp870->f2)->r;if(_tmp872->tag != 0)goto _LL394;};}}_LL393: {
# 4354
struct _tuple1*old_name=_tmp871->name;
struct _tuple1*new_name=Cyc_Toc_temp_var();
_tmp871->name=new_name;
_tmp871->initializer=_tmp861;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD65;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD64;_tmp83B->r=(void*)((_tmpD64=_cycalloc(sizeof(*_tmpD64)),((_tmpD64[0]=((_tmpD65.tag=0,((_tmpD65.f1=_tmp871,_tmpD65)))),_tmpD64))));}{
struct _RegionHandle _tmp875=_new_region("temp");struct _RegionHandle*temp=& _tmp875;_push_region(temp);{
struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpD68;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpD67;struct Cyc_Toc_Env*_tmp876=
Cyc_Toc_add_varmap(temp,nv,old_name,
Cyc_Absyn_varb_exp(new_name,(void*)((_tmpD67=_cycalloc(sizeof(*_tmpD67)),((_tmpD67[0]=((_tmpD68.tag=4,((_tmpD68.f1=_tmp871,_tmpD68)))),_tmpD67)))),0));
Cyc_Toc_local_decl_to_c(_tmp876,nv,_tmp871,_tmp83C);}
# 4365
_npop_handler(0);goto _LL391;
# 4359
;_pop_region(temp);};}_LL394:;_LL395:
# 4370
 s->r=(Cyc_Toc_letdecl_to_c(nv,_tmp860,(void*)_check_null(_tmp861->topt),_tmp861,_tmp83C))->r;
goto _LL391;_LL391:;}
# 4373
goto _LL384;_LL389: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp862=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp85C;if(_tmp862->tag != 3)goto _LL38B;else{_tmp863=_tmp862->f1;}}_LL38A: {
# 4386 "toc.cyc"
struct Cyc_List_List*_tmp879=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp863);
if(_tmp879 == 0){
const char*_tmpD6B;void*_tmpD6A;(_tmpD6A=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD6B="empty Letv_d",_tag_dyneither(_tmpD6B,sizeof(char),13))),_tag_dyneither(_tmpD6A,sizeof(void*),0)));}
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD6E;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD6D;_tmp83B->r=(void*)((_tmpD6D=_cycalloc(sizeof(*_tmpD6D)),((_tmpD6D[0]=((_tmpD6E.tag=0,((_tmpD6E.f1=(struct Cyc_Absyn_Vardecl*)_tmp879->hd,_tmpD6E)))),_tmpD6D))));}
_tmp879=_tmp879->tl;
for(0;_tmp879 != 0;_tmp879=_tmp879->tl){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpD71;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpD70;struct Cyc_Absyn_Decl*_tmp87E=Cyc_Absyn_new_decl((void*)((_tmpD70=_cycalloc(sizeof(*_tmpD70)),((_tmpD70[0]=((_tmpD71.tag=0,((_tmpD71.f1=(struct Cyc_Absyn_Vardecl*)_tmp879->hd,_tmpD71)))),_tmpD70)))),0);
s->r=(Cyc_Absyn_decl_stmt(_tmp87E,Cyc_Absyn_new_stmt(s->r,0),0))->r;}
# 4395
Cyc_Toc_stmt_to_c(nv,s);
goto _LL384;}_LL38B: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp864=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp85C;if(_tmp864->tag != 1)goto _LL38D;else{_tmp865=_tmp864->f1;}}_LL38C: {
# 4398
struct _tuple1*_tmp881=_tmp865->name;
struct _RegionHandle _tmp882=_new_region("temp");struct _RegionHandle*temp=& _tmp882;_push_region(temp);{
struct Cyc_Toc_Env*_tmp883=Cyc_Toc_add_varmap(temp,nv,_tmp865->name,Cyc_Absyn_var_exp(_tmp881,0));
Cyc_Toc_fndecl_to_c(_tmp883,_tmp865,0);
Cyc_Toc_stmt_to_c(_tmp883,_tmp83C);}
# 4404
_npop_handler(0);goto _LL384;
# 4399
;_pop_region(temp);}_LL38D: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp866=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp85C;if(_tmp866->tag != 4)goto _LL38F;else{_tmp867=_tmp866->f1;_tmp868=_tmp866->f2;_tmp869=_tmp866->f3;}}_LL38E: {
# 4406
struct Cyc_Absyn_Stmt*_tmp884=_tmp83C;
# 4414
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp868->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0);
# 4421
struct _RegionHandle _tmp885=_new_region("temp");struct _RegionHandle*temp=& _tmp885;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_add_varmap(temp,nv,x_var,x_exp),_tmp884);
# 4424
if(Cyc_Absyn_no_regions)
s->r=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,
Cyc_Absyn_uint_exp(0,0),_tmp884,0))->r;else{
# 4428
struct Cyc_Absyn_Exp*_tmpD74[1];struct Cyc_Absyn_Exp*_tmpD73[1];struct Cyc_List_List*_tmpD72;s->r=(
Cyc_Absyn_declare_stmt(rh_var,rh_struct_typ,
Cyc_Absyn_fncall_exp(Cyc_Toc__new_region_e,(
(_tmpD72=_cycalloc(sizeof(*_tmpD72)),((_tmpD72->hd=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0),((_tmpD72->tl=0,_tmpD72)))))),0),
# 4433
Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,Cyc_Absyn_address_exp(rh_exp,0),
Cyc_Absyn_seq_stmt(Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_region_e,((_tmpD73[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD73,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),
Cyc_Absyn_seq_stmt(_tmp884,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_region_e,((_tmpD74[0]=x_exp,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpD74,sizeof(struct Cyc_Absyn_Exp*),1)))),0),0),0),0),0),0))->r;}
# 4439
_npop_handler(0);return;
# 4421
;_pop_region(temp);}_LL38F:;_LL390: {
# 4440
const char*_tmpD77;void*_tmpD76;(_tmpD76=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpD77="bad nested declaration within function",_tag_dyneither(_tmpD77,sizeof(char),39))),_tag_dyneither(_tmpD76,sizeof(void*),0)));}_LL384:;}
# 4442
return;_LL37C: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp83D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp83D->tag != 13)goto _LL37E;else{_tmp83E=_tmp83D->f1;_tmp83F=_tmp83D->f2;}}_LL37D:
# 4444
 s=_tmp83F;continue;_LL37E: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp840=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp840->tag != 14)goto _LL380;else{_tmp841=_tmp840->f1;_tmp842=(_tmp840->f2).f1;}}_LL37F: {
# 4446
struct _RegionHandle _tmp88B=_new_region("temp");struct _RegionHandle*temp=& _tmp88B;_push_region(temp);
Cyc_Toc_stmt_to_c(Cyc_Toc_loop_env(temp,nv),_tmp841);
Cyc_Toc_exp_to_c(nv,_tmp842);
# 4450
_npop_handler(0);return;
# 4446
;_pop_region(temp);}_LL380: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp843=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp843->tag != 15)goto _LL382;else{_tmp844=_tmp843->f1;_tmp845=_tmp843->f2;}}_LL381: {
# 4467 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 4478
e_exp->topt=e_typ;{
struct _RegionHandle _tmp88C=_new_region("temp");struct _RegionHandle*temp=& _tmp88C;_push_region(temp);{
struct Cyc_Toc_Env*_tmp88D=Cyc_Toc_add_varmap(temp,nv,e_var,e_exp);
# 4482
Cyc_Toc_stmt_to_c(_tmp88D,_tmp844);{
struct Cyc_Absyn_Stmt*_tmp88E=Cyc_Absyn_seq_stmt(_tmp844,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0),0),0);
# 4488
struct _tuple1*_tmp88F=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp890=Cyc_Absyn_var_exp(_tmp88F,0);
struct Cyc_Absyn_Vardecl*_tmp891=Cyc_Absyn_new_vardecl(_tmp88F,Cyc_Absyn_exn_typ(),0);
_tmp890->topt=Cyc_Absyn_exn_typ();{
struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpD81;struct Cyc_Absyn_Pat*_tmpD80;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpD7F;struct Cyc_Absyn_Pat*_tmpD7E;struct Cyc_Absyn_Pat*_tmp892=(_tmpD7E=_cycalloc(sizeof(*_tmpD7E)),((_tmpD7E->r=(void*)((_tmpD81=_cycalloc(sizeof(*_tmpD81)),((_tmpD81[0]=((_tmpD7F.tag=1,((_tmpD7F.f1=_tmp891,((_tmpD7F.f2=((_tmpD80=_cycalloc(sizeof(*_tmpD80)),((_tmpD80->r=(void*)& Cyc_Absyn_Wild_p_val,((_tmpD80->topt=Cyc_Absyn_exn_typ(),((_tmpD80->loc=0,_tmpD80)))))))),_tmpD7F)))))),_tmpD81)))),((_tmpD7E->topt=Cyc_Absyn_exn_typ(),((_tmpD7E->loc=0,_tmpD7E)))))));
struct Cyc_Absyn_Exp*_tmp893=Cyc_Absyn_throw_exp(_tmp890,0);
_tmp893->topt=(void*)& Cyc_Absyn_VoidType_val;{
struct Cyc_Absyn_Stmt*_tmp894=Cyc_Absyn_exp_stmt(_tmp893,0);
struct Cyc_Core_Opt*_tmpD90;struct _tuple35*_tmpD8F;struct Cyc_Absyn_Vardecl**_tmpD8E;struct Cyc_List_List*_tmpD8D;struct Cyc_Absyn_Switch_clause*_tmpD8C;struct Cyc_Absyn_Switch_clause*_tmp895=
(_tmpD8C=_cycalloc(sizeof(*_tmpD8C)),((_tmpD8C->pattern=_tmp892,((_tmpD8C->pat_vars=((_tmpD90=_cycalloc(sizeof(*_tmpD90)),((_tmpD90->v=((_tmpD8D=_cycalloc(sizeof(*_tmpD8D)),((_tmpD8D->hd=((_tmpD8F=_cycalloc(sizeof(*_tmpD8F)),((_tmpD8F->f1=((_tmpD8E=_cycalloc(sizeof(*_tmpD8E)),((_tmpD8E[0]=_tmp891,_tmpD8E)))),((_tmpD8F->f2=_tmp890,_tmpD8F)))))),((_tmpD8D->tl=0,_tmpD8D)))))),_tmpD90)))),((_tmpD8C->where_clause=0,((_tmpD8C->body=_tmp894,((_tmpD8C->loc=0,_tmpD8C)))))))))));
# 4499
struct Cyc_List_List*_tmpD91;struct Cyc_Absyn_Stmt*_tmp896=Cyc_Absyn_switch_stmt(e_exp,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp845,(
(_tmpD91=_cycalloc(sizeof(*_tmpD91)),((_tmpD91->hd=_tmp895,((_tmpD91->tl=0,_tmpD91))))))),0);
# 4503
Cyc_Toc_stmt_to_c(_tmp88D,_tmp896);{
# 4506
struct Cyc_List_List*_tmpD92;struct Cyc_Absyn_Exp*_tmp897=
Cyc_Absyn_fncall_exp(Cyc_Toc_setjmp_e,(
(_tmpD92=_cycalloc(sizeof(*_tmpD92)),((_tmpD92->hd=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0),((_tmpD92->tl=0,_tmpD92)))))),0);
# 4510
struct Cyc_List_List*_tmpD93;struct Cyc_Absyn_Stmt*_tmp898=
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__push_handler_e,(
(_tmpD93=_cycalloc(sizeof(*_tmpD93)),((_tmpD93->hd=Cyc_Absyn_address_exp(h_exp,0),((_tmpD93->tl=0,_tmpD93)))))),0),0);
# 4514
struct Cyc_Absyn_Exp*_tmp899=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0);
struct Cyc_Absyn_Exp*_tmp89A=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0);
s->r=(
Cyc_Absyn_declare_stmt(h_var,h_typ,0,
Cyc_Absyn_seq_stmt(_tmp898,
Cyc_Absyn_declare_stmt(was_thrown_var,was_thrown_typ,_tmp899,
Cyc_Absyn_seq_stmt(Cyc_Absyn_ifthenelse_stmt(_tmp897,
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp89A,0),
Cyc_Toc_skip_stmt_dl(),0),
Cyc_Absyn_ifthenelse_stmt(Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0),_tmp88E,
# 4525
Cyc_Absyn_declare_stmt(e_var,e_typ,
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e),_tmp896,0),0),0),0),0),0))->r;};};};};}
# 4529
_npop_handler(0);return;
# 4479
;_pop_region(temp);};}_LL382: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp846=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp819;if(_tmp846->tag != 16)goto _LL361;else{_tmp847=_tmp846->f1;}}_LL383:
# 4531
 if(Cyc_Absyn_no_regions)
s->r=(void*)& Cyc_Absyn_Skip_s_val;else{
# 4534
Cyc_Toc_exp_to_c(nv,_tmp847);{
struct Cyc_List_List*_tmpD94;s->r=Cyc_Toc_exp_stmt_r(Cyc_Absyn_fncall_exp(Cyc_Toc__reset_region_e,((_tmpD94=_cycalloc(sizeof(*_tmpD94)),((_tmpD94->hd=_tmp847,((_tmpD94->tl=0,_tmpD94)))))),0));};}
# 4537
return;_LL361:;}}
# 4546
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->ret_type=Cyc_Toc_typ_to_c(f->ret_type);{
struct _RegionHandle _tmp8A8=_new_region("frgn");struct _RegionHandle*frgn=& _tmp8A8;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp8A9=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp8AA=f->args;for(0;_tmp8AA != 0;_tmp8AA=_tmp8AA->tl){
struct _tuple1*_tmpD95;struct _tuple1*_tmp8AB=(_tmpD95=_cycalloc(sizeof(*_tmpD95)),((_tmpD95->f1=Cyc_Absyn_Loc_n,((_tmpD95->f2=(*((struct _tuple9*)_tmp8AA->hd)).f1,_tmpD95)))));
(*((struct _tuple9*)_tmp8AA->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp8AA->hd)).f3);
_tmp8A9=Cyc_Toc_add_varmap(frgn,_tmp8A9,_tmp8AB,Cyc_Absyn_var_exp(_tmp8AB,0));}}
# 4561
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0);return;}
# 4565
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp8AD=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct _dyneither_ptr*_tmp8AF;struct Cyc_Absyn_Tqual _tmp8B0;void*_tmp8B1;int _tmp8B2;struct Cyc_Absyn_VarargInfo _tmp8AE=_tmp8AD;_tmp8AF=_tmp8AE.name;_tmp8B0=_tmp8AE.tq;_tmp8B1=_tmp8AE.type;_tmp8B2=_tmp8AE.inject;{
void*_tmp8B3=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp8B1,(void*)& Cyc_Absyn_HeapRgn_val,_tmp8B0,Cyc_Absyn_false_conref));
struct _tuple1*_tmpD96;struct _tuple1*_tmp8B4=(_tmpD96=_cycalloc(sizeof(*_tmpD96)),((_tmpD96->f1=Cyc_Absyn_Loc_n,((_tmpD96->f2=(struct _dyneither_ptr*)_check_null(_tmp8AF),_tmpD96)))));
{struct _tuple9*_tmpD99;struct Cyc_List_List*_tmpD98;f->args=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(f->args,((_tmpD98=_cycalloc(sizeof(*_tmpD98)),((_tmpD98->hd=((_tmpD99=_cycalloc(sizeof(*_tmpD99)),((_tmpD99->f1=_tmp8AF,((_tmpD99->f2=_tmp8B0,((_tmpD99->f3=_tmp8B3,_tmpD99)))))))),((_tmpD98->tl=0,_tmpD98)))))));}
_tmp8A9=Cyc_Toc_add_varmap(frgn,_tmp8A9,_tmp8B4,Cyc_Absyn_var_exp(_tmp8B4,0));
f->cyc_varargs=0;};}
# 4574
{struct Cyc_List_List*_tmp8B8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp8B8 != 0;_tmp8B8=_tmp8B8->tl){
((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp8B8->hd)->type);}}
# 4577
Cyc_Toc_stmt_to_c(Cyc_Toc_clear_toplevel(frgn,_tmp8A9),f->body);}
# 4553
;_pop_region(frgn);};}
# 4582
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
switch(s){case Cyc_Absyn_Abstract: _LL396:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL397:
 return Cyc_Absyn_Extern;default: _LL398:
 return s;}}struct _tuple36{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple37{struct Cyc_Toc_TocState*f1;struct _tuple36*f2;};
# 4599 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple37*env){
# 4602
struct _tuple37 _tmp8B9=*env;struct Cyc_Toc_TocState*_tmp8BB;struct Cyc_Absyn_Aggrdecl*_tmp8BC;int _tmp8BD;struct _tuple37 _tmp8BA=_tmp8B9;_tmp8BB=_tmp8BA.f1;_tmp8BC=(_tmp8BA.f2)->f1;_tmp8BD=(_tmp8BA.f2)->f2;{
struct _tuple1*_tmp8BE=_tmp8BC->name;
struct Cyc_Toc_TocState _tmp8BF=*_tmp8BB;struct Cyc_Dict_Dict*_tmp8C1;struct Cyc_Toc_TocState _tmp8C0=_tmp8BF;_tmp8C1=_tmp8C0.aggrs_so_far;{
int seen_defn_before;
struct _tuple18**_tmp8C2=((struct _tuple18**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp8C1,_tmp8BE);
if(_tmp8C2 == 0){
seen_defn_before=0;{
struct _tuple18*v;
if(_tmp8BC->kind == Cyc_Absyn_StructA){
struct _tuple18*_tmpD9A;v=((_tmpD9A=_region_malloc(d,sizeof(*_tmpD9A)),((_tmpD9A->f1=_tmp8BC,((_tmpD9A->f2=Cyc_Absyn_strctq(_tmp8BE),_tmpD9A))))));}else{
# 4613
struct _tuple18*_tmpD9B;v=((_tmpD9B=_region_malloc(d,sizeof(*_tmpD9B)),((_tmpD9B->f1=_tmp8BC,((_tmpD9B->f2=Cyc_Absyn_unionq_typ(_tmp8BE),_tmpD9B))))));}
*_tmp8C1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8C1,_tmp8BE,v);};}else{
# 4616
struct _tuple18*_tmp8C5=*_tmp8C2;struct Cyc_Absyn_Aggrdecl*_tmp8C7;void*_tmp8C8;struct _tuple18*_tmp8C6=_tmp8C5;_tmp8C7=_tmp8C6->f1;_tmp8C8=_tmp8C6->f2;
if(_tmp8C7->impl == 0){
{struct _tuple18*_tmpD9C;*_tmp8C1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple18*v))Cyc_Dict_insert)(*_tmp8C1,_tmp8BE,((_tmpD9C=_region_malloc(d,sizeof(*_tmpD9C)),((_tmpD9C->f1=_tmp8BC,((_tmpD9C->f2=_tmp8C8,_tmpD9C)))))));}
seen_defn_before=0;}else{
# 4621
seen_defn_before=1;}}{
# 4623
struct Cyc_Absyn_Aggrdecl*_tmpD9D;struct Cyc_Absyn_Aggrdecl*new_ad=(_tmpD9D=_cycalloc(sizeof(*_tmpD9D)),((_tmpD9D->kind=_tmp8BC->kind,((_tmpD9D->sc=Cyc_Absyn_Public,((_tmpD9D->name=_tmp8BC->name,((_tmpD9D->tvs=0,((_tmpD9D->impl=0,((_tmpD9D->attributes=_tmp8BC->attributes,_tmpD9D)))))))))))));
# 4629
if(_tmp8BC->impl != 0  && !seen_defn_before){
{struct Cyc_Absyn_AggrdeclImpl*_tmpD9E;new_ad->impl=((_tmpD9E=_cycalloc(sizeof(*_tmpD9E)),((_tmpD9E->exist_vars=0,((_tmpD9E->rgn_po=0,((_tmpD9E->fields=0,((_tmpD9E->tagged=0,_tmpD9E))))))))));}{
# 4634
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp8CB=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8BC->impl))->fields;for(0;_tmp8CB != 0;_tmp8CB=_tmp8CB->tl){
# 4638
struct Cyc_Absyn_Aggrfield*_tmp8CC=(struct Cyc_Absyn_Aggrfield*)_tmp8CB->hd;
void*_tmp8CD=_tmp8CC->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp8CD)) && (
_tmp8BC->kind == Cyc_Absyn_StructA  && _tmp8CB->tl == 0  || _tmp8BC->kind == Cyc_Absyn_UnionA)){
# 4648
void*_tmp8CE=Cyc_Tcutil_compress(_tmp8CD);void*_tmp8CF=_tmp8CE;void*_tmp8D1;struct Cyc_Absyn_Tqual _tmp8D2;union Cyc_Absyn_Constraint*_tmp8D3;unsigned int _tmp8D4;_LL39B: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8D0=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8CF;if(_tmp8D0->tag != 8)goto _LL39D;else{_tmp8D1=(_tmp8D0->f1).elt_type;_tmp8D2=(_tmp8D0->f1).tq;_tmp8D3=(_tmp8D0->f1).zero_term;_tmp8D4=(_tmp8D0->f1).zt_loc;}}_LL39C:
# 4650
{struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpDA4;struct Cyc_Absyn_ArrayInfo _tmpDA3;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpDA2;_tmp8CD=(void*)((_tmpDA2=_cycalloc(sizeof(*_tmpDA2)),((_tmpDA2[0]=((_tmpDA4.tag=8,((_tmpDA4.f1=((_tmpDA3.elt_type=_tmp8D1,((_tmpDA3.tq=_tmp8D2,((_tmpDA3.num_elts=Cyc_Absyn_uint_exp(0,0),((_tmpDA3.zero_term=_tmp8D3,((_tmpDA3.zt_loc=_tmp8D4,_tmpDA3)))))))))),_tmpDA4)))),_tmpDA2))));}
goto _LL39A;_LL39D:;_LL39E:
# 4653
 if(Cyc_Toc_abstract_aggr(_tmp8CD))goto _LL39A;else{
if(_tmp8BC->kind == Cyc_Absyn_StructA)continue;else{
const char*_tmpDA7;void*_tmpDA6;(_tmpDA6=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(((_tmpDA7="union with non-array fields of kind TA",_tag_dyneither(_tmpDA7,sizeof(char),39))),_tag_dyneither(_tmpDA6,sizeof(void*),0)));}}_LL39A:;}{
# 4658
struct Cyc_Absyn_Aggrfield*_tmpDA8;struct Cyc_Absyn_Aggrfield*_tmp8DA=(_tmpDA8=_cycalloc(sizeof(*_tmpDA8)),((_tmpDA8->name=_tmp8CC->name,((_tmpDA8->tq=Cyc_Toc_mt_tq,((_tmpDA8->type=
# 4660
Cyc_Toc_typ_to_c(_tmp8CD),((_tmpDA8->width=_tmp8CC->width,((_tmpDA8->attributes=_tmp8CC->attributes,((_tmpDA8->requires_clause=0,_tmpDA8)))))))))))));
# 4667
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8BC->impl))->tagged){
void*_tmp8DB=_tmp8DA->type;
struct _dyneither_ptr*_tmp8DC=_tmp8DA->name;
const char*_tmpDAD;void*_tmpDAC[2];struct Cyc_String_pa_PrintArg_struct _tmpDAB;struct Cyc_String_pa_PrintArg_struct _tmpDAA;struct _dyneither_ptr s=(struct _dyneither_ptr)((_tmpDAA.tag=0,((_tmpDAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DC),((_tmpDAB.tag=0,((_tmpDAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp8BC->name).f2),((_tmpDAC[0]=& _tmpDAB,((_tmpDAC[1]=& _tmpDAA,Cyc_aprintf(((_tmpDAD="_union_%s_%s",_tag_dyneither(_tmpDAD,sizeof(char),13))),_tag_dyneither(_tmpDAC,sizeof(void*),2))))))))))))));
struct _dyneither_ptr*_tmpDAE;struct _dyneither_ptr*str=(_tmpDAE=_cycalloc(sizeof(*_tmpDAE)),((_tmpDAE[0]=s,_tmpDAE)));
struct Cyc_Absyn_Aggrfield*_tmpDAF;struct Cyc_Absyn_Aggrfield*_tmp8DD=(_tmpDAF=_cycalloc(sizeof(*_tmpDAF)),((_tmpDAF->name=Cyc_Toc_val_sp,((_tmpDAF->tq=Cyc_Toc_mt_tq,((_tmpDAF->type=_tmp8DB,((_tmpDAF->width=0,((_tmpDAF->attributes=0,((_tmpDAF->requires_clause=0,_tmpDAF)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDB0;struct Cyc_Absyn_Aggrfield*_tmp8DE=(_tmpDB0=_cycalloc(sizeof(*_tmpDB0)),((_tmpDB0->name=Cyc_Toc_tag_sp,((_tmpDB0->tq=Cyc_Toc_mt_tq,((_tmpDB0->type=Cyc_Absyn_sint_typ,((_tmpDB0->width=0,((_tmpDB0->attributes=0,((_tmpDB0->requires_clause=0,_tmpDB0)))))))))))));
struct Cyc_Absyn_Aggrfield*_tmpDB1[2];struct Cyc_List_List*_tmp8DF=(_tmpDB1[1]=_tmp8DD,((_tmpDB1[0]=_tmp8DE,((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpDB1,sizeof(struct Cyc_Absyn_Aggrfield*),2)))));
struct Cyc_Absyn_AggrdeclImpl*_tmpDB6;struct _tuple1*_tmpDB5;struct Cyc_Absyn_Aggrdecl*_tmpDB4;struct Cyc_Absyn_Aggrdecl*_tmp8E0=(_tmpDB4=_cycalloc(sizeof(*_tmpDB4)),((_tmpDB4->kind=Cyc_Absyn_StructA,((_tmpDB4->sc=Cyc_Absyn_Public,((_tmpDB4->name=(
(_tmpDB5=_cycalloc(sizeof(*_tmpDB5)),((_tmpDB5->f1=Cyc_Absyn_Loc_n,((_tmpDB5->f2=str,_tmpDB5)))))),((_tmpDB4->tvs=0,((_tmpDB4->impl=(
(_tmpDB6=_cycalloc(sizeof(*_tmpDB6)),((_tmpDB6->exist_vars=0,((_tmpDB6->rgn_po=0,((_tmpDB6->fields=_tmp8DF,((_tmpDB6->tagged=0,_tmpDB6)))))))))),((_tmpDB4->attributes=0,_tmpDB4)))))))))))));
# 4679
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDBC;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDBB;struct Cyc_List_List*_tmpDBA;Cyc_Toc_result_decls=((_tmpDBA=_cycalloc(sizeof(*_tmpDBA)),((_tmpDBA->hd=Cyc_Absyn_new_decl((void*)((_tmpDBC=_cycalloc(sizeof(*_tmpDBC)),((_tmpDBC[0]=((_tmpDBB.tag=5,((_tmpDBB.f1=_tmp8E0,_tmpDBB)))),_tmpDBC)))),0),((_tmpDBA->tl=Cyc_Toc_result_decls,_tmpDBA))))));}
_tmp8DA->type=Cyc_Absyn_strct(str);}{
# 4682
struct Cyc_List_List*_tmpDBD;new_fields=((_tmpDBD=_cycalloc(sizeof(*_tmpDBD)),((_tmpDBD->hd=_tmp8DA,((_tmpDBD->tl=new_fields,_tmpDBD))))));};};}}
# 4684
(new_ad->impl)->fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);};}
# 4686
if(_tmp8BD){
struct Cyc_Absyn_Decl*_tmpDC7;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDC6;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDC5;struct Cyc_List_List*_tmpDC4;Cyc_Toc_result_decls=((_tmpDC4=_cycalloc(sizeof(*_tmpDC4)),((_tmpDC4->hd=((_tmpDC7=_cycalloc(sizeof(*_tmpDC7)),((_tmpDC7->r=(void*)((_tmpDC5=_cycalloc(sizeof(*_tmpDC5)),((_tmpDC5[0]=((_tmpDC6.tag=5,((_tmpDC6.f1=new_ad,_tmpDC6)))),_tmpDC5)))),((_tmpDC7->loc=0,_tmpDC7)))))),((_tmpDC4->tl=Cyc_Toc_result_decls,_tmpDC4))))));}
return 0;};};};}
# 4691
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple36 _tmpDC8;struct _tuple36 p=(_tmpDC8.f1=ad,((_tmpDC8.f2=add_to_decls,_tmpDC8)));
((int(*)(struct _tuple36*arg,int(*f)(struct _RegionHandle*,struct _tuple37*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple38{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4720 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4723
struct _tuple38 _tmp8F7=*env;struct Cyc_Set_Set**_tmp8F9;struct Cyc_Absyn_Datatypedecl*_tmp8FA;struct _tuple38 _tmp8F8=_tmp8F7;_tmp8F9=(_tmp8F8.f1)->datatypes_so_far;_tmp8FA=_tmp8F8.f2;{
struct _tuple1*_tmp8FB=_tmp8FA->name;
if(_tmp8FA->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp8F9,_tmp8FB))
return 0;
*_tmp8F9=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp8F9,_tmp8FB);
{struct Cyc_List_List*_tmp8FC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp8FA->fields))->v;for(0;_tmp8FC != 0;_tmp8FC=_tmp8FC->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp8FC->hd;
# 4731
struct Cyc_List_List*_tmp8FD=0;
int i=1;
{struct Cyc_List_List*_tmp8FE=f->typs;for(0;_tmp8FE != 0;(_tmp8FE=_tmp8FE->tl,i ++)){
struct _dyneither_ptr*_tmp8FF=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmpDC9;struct Cyc_Absyn_Aggrfield*_tmp900=(_tmpDC9=_cycalloc(sizeof(*_tmpDC9)),((_tmpDC9->name=_tmp8FF,((_tmpDC9->tq=(*((struct _tuple11*)_tmp8FE->hd)).f1,((_tmpDC9->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp8FE->hd)).f2),((_tmpDC9->width=0,((_tmpDC9->attributes=0,((_tmpDC9->requires_clause=0,_tmpDC9)))))))))))));
struct Cyc_List_List*_tmpDCA;_tmp8FD=((_tmpDCA=_cycalloc(sizeof(*_tmpDCA)),((_tmpDCA->hd=_tmp900,((_tmpDCA->tl=_tmp8FD,_tmpDCA))))));}}
# 4739
{struct Cyc_Absyn_Aggrfield*_tmpDCD;struct Cyc_List_List*_tmpDCC;_tmp8FD=((_tmpDCC=_cycalloc(sizeof(*_tmpDCC)),((_tmpDCC->hd=((_tmpDCD=_cycalloc(sizeof(*_tmpDCD)),((_tmpDCD->name=Cyc_Toc_tag_sp,((_tmpDCD->tq=Cyc_Toc_mt_tq,((_tmpDCD->type=Cyc_Absyn_sint_typ,((_tmpDCD->width=0,((_tmpDCD->attributes=0,((_tmpDCD->requires_clause=0,_tmpDCD)))))))))))))),((_tmpDCC->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp8FD),_tmpDCC))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDD0;struct Cyc_Absyn_Aggrdecl*_tmpDCF;struct Cyc_Absyn_Aggrdecl*_tmp905=
(_tmpDCF=_cycalloc(sizeof(*_tmpDCF)),((_tmpDCF->kind=Cyc_Absyn_StructA,((_tmpDCF->sc=Cyc_Absyn_Public,((_tmpDCF->name=
Cyc_Toc_collapse_qvars(f->name,_tmp8FA->name),((_tmpDCF->tvs=0,((_tmpDCF->impl=(
# 4745
(_tmpDD0=_cycalloc(sizeof(*_tmpDD0)),((_tmpDD0->exist_vars=0,((_tmpDD0->rgn_po=0,((_tmpDD0->fields=_tmp8FD,((_tmpDD0->tagged=0,_tmpDD0)))))))))),((_tmpDCF->attributes=0,_tmpDCF)))))))))))));
# 4747
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDD6;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDD5;struct Cyc_List_List*_tmpDD4;Cyc_Toc_result_decls=((_tmpDD4=_cycalloc(sizeof(*_tmpDD4)),((_tmpDD4->hd=Cyc_Absyn_new_decl((void*)((_tmpDD6=_cycalloc(sizeof(*_tmpDD6)),((_tmpDD6[0]=((_tmpDD5.tag=5,((_tmpDD5.f1=_tmp905,_tmpDD5)))),_tmpDD6)))),0),((_tmpDD4->tl=Cyc_Toc_result_decls,_tmpDD4))))));};}}
# 4749
return 0;};}
# 4752
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4771 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple38*env){
# 4774
struct _tuple38 _tmp90B=*env;struct Cyc_Toc_TocState*_tmp90D;struct Cyc_Absyn_Datatypedecl*_tmp90E;struct _tuple38 _tmp90C=_tmp90B;_tmp90D=_tmp90C.f1;_tmp90E=_tmp90C.f2;
if(_tmp90E->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp90F=*_tmp90D;struct Cyc_Dict_Dict*_tmp911;struct Cyc_Toc_TocState _tmp910=_tmp90F;_tmp911=_tmp910.xdatatypes_so_far;{
struct _tuple1*_tmp912=_tmp90E->name;
{struct Cyc_List_List*_tmp913=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp90E->fields))->v;for(0;_tmp913 != 0;_tmp913=_tmp913->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp913->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0);
void*_tmp915=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp914,Cyc_Absyn_false_conref,0);
# 4785
int*_tmp916=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp911,f->name);int*_tmp917=_tmp916;_LL3A0: if(_tmp917 != 0)goto _LL3A2;_LL3A1: {
# 4787
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
initopt=Cyc_Absyn_string_exp(*fn,0);{
# 4791
struct Cyc_Absyn_Vardecl*_tmp918=Cyc_Absyn_new_vardecl(f->name,_tmp915,initopt);
_tmp918->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDDC;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDDB;struct Cyc_List_List*_tmpDDA;Cyc_Toc_result_decls=((_tmpDDA=_cycalloc(sizeof(*_tmpDDA)),((_tmpDDA->hd=Cyc_Absyn_new_decl((void*)((_tmpDDC=_cycalloc(sizeof(*_tmpDDC)),((_tmpDDC[0]=((_tmpDDB.tag=0,((_tmpDDB.f1=_tmp918,_tmpDDB)))),_tmpDDC)))),0),((_tmpDDA->tl=Cyc_Toc_result_decls,_tmpDDA))))));}
*_tmp911=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp911,f->name,f->sc != Cyc_Absyn_Extern);{
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp91C=f->typs;for(0;_tmp91C != 0;(_tmp91C=_tmp91C->tl,i ++)){
struct Cyc_Int_pa_PrintArg_struct _tmpDE4;void*_tmpDE3[1];const char*_tmpDE2;struct _dyneither_ptr*_tmpDE1;struct _dyneither_ptr*_tmp91D=(_tmpDE1=_cycalloc(sizeof(*_tmpDE1)),((_tmpDE1[0]=(struct _dyneither_ptr)((_tmpDE4.tag=1,((_tmpDE4.f1=(unsigned long)i,((_tmpDE3[0]=& _tmpDE4,Cyc_aprintf(((_tmpDE2="f%d",_tag_dyneither(_tmpDE2,sizeof(char),4))),_tag_dyneither(_tmpDE3,sizeof(void*),1)))))))),_tmpDE1)));
struct Cyc_Absyn_Aggrfield*_tmpDE5;struct Cyc_Absyn_Aggrfield*_tmp91E=(_tmpDE5=_cycalloc(sizeof(*_tmpDE5)),((_tmpDE5->name=_tmp91D,((_tmpDE5->tq=(*((struct _tuple11*)_tmp91C->hd)).f1,((_tmpDE5->type=
Cyc_Toc_typ_to_c((*((struct _tuple11*)_tmp91C->hd)).f2),((_tmpDE5->width=0,((_tmpDE5->attributes=0,((_tmpDE5->requires_clause=0,_tmpDE5)))))))))))));
struct Cyc_List_List*_tmpDE6;fields=((_tmpDE6=_cycalloc(sizeof(*_tmpDE6)),((_tmpDE6->hd=_tmp91E,((_tmpDE6->tl=fields,_tmpDE6))))));}}
# 4804
{struct Cyc_Absyn_Aggrfield*_tmpDE9;struct Cyc_List_List*_tmpDE8;fields=((_tmpDE8=_cycalloc(sizeof(*_tmpDE8)),((_tmpDE8->hd=((_tmpDE9=_cycalloc(sizeof(*_tmpDE9)),((_tmpDE9->name=Cyc_Toc_tag_sp,((_tmpDE9->tq=Cyc_Toc_mt_tq,((_tmpDE9->type=
Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq),((_tmpDE9->width=0,((_tmpDE9->attributes=0,((_tmpDE9->requires_clause=0,_tmpDE9)))))))))))))),((_tmpDE8->tl=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields),_tmpDE8))))));}{
struct Cyc_Absyn_AggrdeclImpl*_tmpDEC;struct Cyc_Absyn_Aggrdecl*_tmpDEB;struct Cyc_Absyn_Aggrdecl*_tmp927=
(_tmpDEB=_cycalloc(sizeof(*_tmpDEB)),((_tmpDEB->kind=Cyc_Absyn_StructA,((_tmpDEB->sc=Cyc_Absyn_Public,((_tmpDEB->name=
Cyc_Toc_collapse_qvars(f->name,_tmp90E->name),((_tmpDEB->tvs=0,((_tmpDEB->impl=(
# 4811
(_tmpDEC=_cycalloc(sizeof(*_tmpDEC)),((_tmpDEC->exist_vars=0,((_tmpDEC->rgn_po=0,((_tmpDEC->fields=fields,((_tmpDEC->tagged=0,_tmpDEC)))))))))),((_tmpDEB->attributes=0,_tmpDEB)))))))))))));
# 4813
{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDF2;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmpDF1;struct Cyc_List_List*_tmpDF0;Cyc_Toc_result_decls=((_tmpDF0=_cycalloc(sizeof(*_tmpDF0)),((_tmpDF0->hd=Cyc_Absyn_new_decl((void*)((_tmpDF2=_cycalloc(sizeof(*_tmpDF2)),((_tmpDF2[0]=((_tmpDF1.tag=5,((_tmpDF1.f1=_tmp927,_tmpDF1)))),_tmpDF2)))),0),((_tmpDF0->tl=Cyc_Toc_result_decls,_tmpDF0))))));}
# 4815
goto _LL39F;};};};}_LL3A2: if(_tmp917 == 0)goto _LL3A4;if(*_tmp917 != 0)goto _LL3A4;_LL3A3:
# 4817
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp92D=Cyc_Absyn_string_exp(*fn,0);
struct Cyc_Absyn_Vardecl*_tmp92E=Cyc_Absyn_new_vardecl(f->name,_tmp915,_tmp92D);
_tmp92E->sc=f->sc;
{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmpDF8;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmpDF7;struct Cyc_List_List*_tmpDF6;Cyc_Toc_result_decls=((_tmpDF6=_cycalloc(sizeof(*_tmpDF6)),((_tmpDF6->hd=Cyc_Absyn_new_decl((void*)((_tmpDF8=_cycalloc(sizeof(*_tmpDF8)),((_tmpDF8[0]=((_tmpDF7.tag=0,((_tmpDF7.f1=_tmp92E,_tmpDF7)))),_tmpDF8)))),0),((_tmpDF6->tl=Cyc_Toc_result_decls,_tmpDF6))))));}
*_tmp911=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp911,f->name,1);}
# 4824
goto _LL39F;_LL3A4:;_LL3A5:
 goto _LL39F;_LL39F:;}}
# 4828
return 0;};};}
# 4831
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple38*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4836
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4844
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*body_nv,struct Cyc_Toc_Env*init_nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
vd->type=Cyc_Toc_typ_to_c(old_typ);
# 4848
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(body_nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
void*_tmp932=init->r;void*_tmp933=_tmp932;struct Cyc_Absyn_Vardecl*_tmp935;struct Cyc_Absyn_Exp*_tmp936;struct Cyc_Absyn_Exp*_tmp937;int _tmp938;_LL3A7: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp934=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp933;if(_tmp934->tag != 26)goto _LL3A9;else{_tmp935=_tmp934->f1;_tmp936=_tmp934->f2;_tmp937=_tmp934->f3;_tmp938=_tmp934->f4;}}_LL3A8:
# 4855
 vd->initializer=0;
s->r=(Cyc_Toc_init_comprehension(init_nv,old_typ,Cyc_Absyn_var_exp(vd->name,0),_tmp935,_tmp936,_tmp937,_tmp938,
Cyc_Absyn_new_stmt(s->r,0),0))->r;
goto _LL3A6;_LL3A9:;_LL3AA:
# 4860
 if(vd->sc == Cyc_Absyn_Static){
# 4864
struct _RegionHandle _tmp939=_new_region("temp");struct _RegionHandle*temp=& _tmp939;_push_region(temp);
{struct Cyc_Toc_Env*_tmp93A=Cyc_Toc_set_toplevel(temp,init_nv);
Cyc_Toc_exp_to_c(_tmp93A,init);}
# 4865
;_pop_region(temp);}else{
# 4869
Cyc_Toc_exp_to_c(init_nv,init);}
goto _LL3A6;_LL3A6:;}else{
# 4874
void*_tmp93B=Cyc_Tcutil_compress(old_typ);void*_tmp93C=_tmp93B;void*_tmp93E;struct Cyc_Absyn_Exp*_tmp93F;union Cyc_Absyn_Constraint*_tmp940;_LL3AC:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp93D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp93C;if(_tmp93D->tag != 8)goto _LL3AE;else{_tmp93E=(_tmp93D->f1).elt_type;_tmp93F=(_tmp93D->f1).num_elts;_tmp940=(_tmp93D->f1).zero_term;}}if(!
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp940))goto _LL3AE;_LL3AD:
 if(_tmp93F == 0){
const char*_tmpDFB;void*_tmpDFA;(_tmpDFA=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpDFB="can't initialize zero-terminated array -- size unknown",_tag_dyneither(_tmpDFB,sizeof(char),55))),_tag_dyneither(_tmpDFA,sizeof(void*),0)));}{
struct Cyc_Absyn_Exp*num_elts=_tmp93F;
struct Cyc_Absyn_Exp*_tmp943=Cyc_Absyn_subscript_exp(Cyc_Absyn_var_exp(vd->name,0),
Cyc_Absyn_add_exp(num_elts,Cyc_Absyn_signed_int_exp(- 1,0),0),0);
# 4882
struct Cyc_Absyn_Exp*_tmp944=Cyc_Absyn_signed_int_exp(0,0);
s->r=Cyc_Toc_seq_stmt_r(Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp943,_tmp944,0),0),
Cyc_Absyn_new_stmt(s->r,0));
goto _LL3AB;};_LL3AE:;_LL3AF:
 goto _LL3AB;_LL3AB:;}}
# 4896
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
Cyc_Toc_exp_to_c(nv,e);{
struct _tuple1*x=Cyc_Toc_temp_var();
struct _RegionHandle _tmp945=_new_region("prgn");struct _RegionHandle*prgn=& _tmp945;_push_region(prgn);{
struct Cyc_Absyn_Stmt*_tmp946=Cyc_Toc_throw_match_stmt();
struct Cyc_Toc_Env*_tmp947=Cyc_Toc_share_env(prgn,nv);
struct _tuple31 _tmp948=
Cyc_Toc_xlate_pat(_tmp947,prgn,t,Cyc_Absyn_var_exp(x,0),Cyc_Absyn_var_exp(x,0),p,
Cyc_Toc_throw_match_stmt(),0);
# 4902
struct Cyc_Toc_Env*_tmp94A;struct Cyc_List_List*_tmp94B;struct Cyc_Absyn_Stmt*_tmp94C;struct _tuple31 _tmp949=_tmp948;_tmp94A=_tmp949.f1;_tmp94B=_tmp949.f2;_tmp94C=_tmp949.f3;
# 4905
Cyc_Toc_stmt_to_c(_tmp94A,s);
s=Cyc_Absyn_declare_stmt(x,Cyc_Toc_typ_to_c(t),e,Cyc_Absyn_seq_stmt(_tmp94C,s,0),0);
for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
struct _tuple32*_tmp94D=(struct _tuple32*)_tmp94B->hd;struct _tuple1*_tmp94F;void*_tmp950;struct _tuple32*_tmp94E=_tmp94D;_tmp94F=_tmp94E->f1;_tmp950=_tmp94E->f2;
s=Cyc_Absyn_declare_stmt(_tmp94F,_tmp950,0,s,0);}}{
# 4912
struct Cyc_Absyn_Stmt*_tmp951=s;_npop_handler(0);return _tmp951;};
# 4899
;_pop_region(prgn);};}
# 4918
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp952=e->r;void*_tmp953=_tmp952;struct Cyc_Absyn_Exp*_tmp955;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp961;struct Cyc_Absyn_Exp*_tmp963;struct Cyc_List_List*_tmp965;struct Cyc_Absyn_Exp*_tmp967;struct Cyc_Absyn_Exp*_tmp968;struct Cyc_Absyn_Exp*_tmp96A;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_Absyn_Exp*_tmp96E;struct Cyc_Absyn_Exp*_tmp970;struct Cyc_Absyn_Exp*_tmp971;struct Cyc_Absyn_Exp*_tmp973;struct Cyc_Absyn_Exp*_tmp974;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_Exp*_tmp977;struct Cyc_Absyn_Exp*_tmp979;struct Cyc_Absyn_Exp*_tmp97A;struct Cyc_Absyn_Exp*_tmp97B;struct Cyc_Absyn_Exp*_tmp97D;struct Cyc_List_List*_tmp97E;void**_tmp980;struct Cyc_Absyn_Exp*_tmp981;void**_tmp983;struct Cyc_List_List*_tmp984;struct Cyc_List_List*_tmp986;struct Cyc_List_List*_tmp988;void**_tmp98A;void**_tmp98C;struct Cyc_Absyn_Stmt*_tmp98E;struct Cyc_Absyn_MallocInfo*_tmp990;_LL3B1: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp954=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp954->tag != 19)goto _LL3B3;else{_tmp955=_tmp954->f1;}}_LL3B2:
 _tmp957=_tmp955;goto _LL3B4;_LL3B3: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp956=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp956->tag != 20)goto _LL3B5;else{_tmp957=_tmp956->f1;}}_LL3B4:
 _tmp959=_tmp957;goto _LL3B6;_LL3B5: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp958=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp958->tag != 21)goto _LL3B7;else{_tmp959=_tmp958->f1;}}_LL3B6:
 _tmp95B=_tmp959;goto _LL3B8;_LL3B7: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp95A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp95A->tag != 14)goto _LL3B9;else{_tmp95B=_tmp95A->f1;}}_LL3B8:
 _tmp95D=_tmp95B;goto _LL3BA;_LL3B9: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp95C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp95C->tag != 10)goto _LL3BB;else{_tmp95D=_tmp95C->f1;}}_LL3BA:
 _tmp95F=_tmp95D;goto _LL3BC;_LL3BB: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp95E=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp95E->tag != 11)goto _LL3BD;else{_tmp95F=_tmp95E->f1;}}_LL3BC:
 _tmp961=_tmp95F;goto _LL3BE;_LL3BD: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp960=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp960->tag != 17)goto _LL3BF;else{_tmp961=_tmp960->f1;}}_LL3BE:
 _tmp963=_tmp961;goto _LL3C0;_LL3BF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp962=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp962->tag != 4)goto _LL3C1;else{_tmp963=_tmp962->f1;}}_LL3C0:
 Cyc_Toc_exptypes_to_c(_tmp963);goto _LL3B0;_LL3C1: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp964=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp964->tag != 2)goto _LL3C3;else{_tmp965=_tmp964->f2;}}_LL3C2:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp965);goto _LL3B0;_LL3C3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp966=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp966->tag != 6)goto _LL3C5;else{_tmp967=_tmp966->f1;_tmp968=_tmp966->f2;}}_LL3C4:
 _tmp96A=_tmp967;_tmp96B=_tmp968;goto _LL3C6;_LL3C5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp969=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp969->tag != 7)goto _LL3C7;else{_tmp96A=_tmp969->f1;_tmp96B=_tmp969->f2;}}_LL3C6:
 _tmp96D=_tmp96A;_tmp96E=_tmp96B;goto _LL3C8;_LL3C7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp96C=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp96C->tag != 8)goto _LL3C9;else{_tmp96D=_tmp96C->f1;_tmp96E=_tmp96C->f2;}}_LL3C8:
 _tmp970=_tmp96D;_tmp971=_tmp96E;goto _LL3CA;_LL3C9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp96F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp96F->tag != 22)goto _LL3CB;else{_tmp970=_tmp96F->f1;_tmp971=_tmp96F->f2;}}_LL3CA:
 _tmp973=_tmp970;_tmp974=_tmp971;goto _LL3CC;_LL3CB: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp972=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp972->tag != 34)goto _LL3CD;else{_tmp973=_tmp972->f1;_tmp974=_tmp972->f2;}}_LL3CC:
 _tmp976=_tmp973;_tmp977=_tmp974;goto _LL3CE;_LL3CD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp975=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp975->tag != 3)goto _LL3CF;else{_tmp976=_tmp975->f1;_tmp977=_tmp975->f3;}}_LL3CE:
 Cyc_Toc_exptypes_to_c(_tmp976);Cyc_Toc_exptypes_to_c(_tmp977);goto _LL3B0;_LL3CF: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp978=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp978->tag != 5)goto _LL3D1;else{_tmp979=_tmp978->f1;_tmp97A=_tmp978->f2;_tmp97B=_tmp978->f3;}}_LL3D0:
# 4936
 Cyc_Toc_exptypes_to_c(_tmp979);Cyc_Toc_exptypes_to_c(_tmp97A);Cyc_Toc_exptypes_to_c(_tmp97B);goto _LL3B0;_LL3D1: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp97C=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp97C->tag != 9)goto _LL3D3;else{_tmp97D=_tmp97C->f1;_tmp97E=_tmp97C->f2;}}_LL3D2:
# 4938
 Cyc_Toc_exptypes_to_c(_tmp97D);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp97E);goto _LL3B0;_LL3D3: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp97F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp97F->tag != 13)goto _LL3D5;else{_tmp980=(void**)((void**)& _tmp97F->f1);_tmp981=_tmp97F->f2;}}_LL3D4:
*_tmp980=Cyc_Toc_typ_to_c(*_tmp980);Cyc_Toc_exptypes_to_c(_tmp981);goto _LL3B0;_LL3D5: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp982=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp982->tag != 24)goto _LL3D7;else{_tmp983=(void**)&(_tmp982->f1)->f3;_tmp984=_tmp982->f2;}}_LL3D6:
# 4941
*_tmp983=Cyc_Toc_typ_to_c(*_tmp983);
_tmp986=_tmp984;goto _LL3D8;_LL3D7: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp985=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp985->tag != 35)goto _LL3D9;else{_tmp986=_tmp985->f2;}}_LL3D8:
 _tmp988=_tmp986;goto _LL3DA;_LL3D9: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp987=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp987->tag != 25)goto _LL3DB;else{_tmp988=_tmp987->f1;}}_LL3DA:
# 4945
 for(0;_tmp988 != 0;_tmp988=_tmp988->tl){
struct _tuple19 _tmp9A0=*((struct _tuple19*)_tmp988->hd);struct Cyc_Absyn_Exp*_tmp9A2;struct _tuple19 _tmp9A1=_tmp9A0;_tmp9A2=_tmp9A1.f2;
Cyc_Toc_exptypes_to_c(_tmp9A2);}
# 4949
goto _LL3B0;_LL3DB: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp989=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp989->tag != 18)goto _LL3DD;else{_tmp98A=(void**)((void**)& _tmp989->f1);}}_LL3DC:
 _tmp98C=_tmp98A;goto _LL3DE;_LL3DD: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp98B=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp98B->tag != 16)goto _LL3DF;else{_tmp98C=(void**)((void**)& _tmp98B->f1);}}_LL3DE:
*_tmp98C=Cyc_Toc_typ_to_c(*_tmp98C);goto _LL3B0;_LL3DF: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp98D=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp98D->tag != 36)goto _LL3E1;else{_tmp98E=_tmp98D->f1;}}_LL3E0:
 Cyc_Toc_stmttypes_to_c(_tmp98E);goto _LL3B0;_LL3E1: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp98F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp98F->tag != 33)goto _LL3E3;else{_tmp990=(struct Cyc_Absyn_MallocInfo*)& _tmp98F->f1;}}_LL3E2:
# 4954
 if(_tmp990->elt_type != 0){
void**_tmpDFC;_tmp990->elt_type=((_tmpDFC=_cycalloc(sizeof(*_tmpDFC)),((_tmpDFC[0]=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp990->elt_type))),_tmpDFC))));}
Cyc_Toc_exptypes_to_c(_tmp990->num_elts);
goto _LL3B0;_LL3E3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp991=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp991->tag != 0)goto _LL3E5;}_LL3E4:
 goto _LL3E6;_LL3E5: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp992=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp992->tag != 1)goto _LL3E7;}_LL3E6:
 goto _LL3E8;_LL3E7: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp993=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp993->tag != 31)goto _LL3E9;}_LL3E8:
 goto _LL3EA;_LL3E9: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp994=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp994->tag != 39)goto _LL3EB;}_LL3EA:
 goto _LL3EC;_LL3EB: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp995=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp995->tag != 32)goto _LL3ED;}_LL3EC:
 goto _LL3B0;_LL3ED: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp996=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp996->tag != 29)goto _LL3EF;}_LL3EE:
# 4964
 goto _LL3F0;_LL3EF: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp997=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp997->tag != 30)goto _LL3F1;}_LL3F0:
 goto _LL3F2;_LL3F1: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp998=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp998->tag != 28)goto _LL3F3;}_LL3F2:
 goto _LL3F4;_LL3F3: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp999=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp999->tag != 26)goto _LL3F5;}_LL3F4:
 goto _LL3F6;_LL3F5: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp99A=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99A->tag != 27)goto _LL3F7;}_LL3F6:
 goto _LL3F8;_LL3F7: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp99B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99B->tag != 23)goto _LL3F9;}_LL3F8:
 goto _LL3FA;_LL3F9: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp99C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99C->tag != 12)goto _LL3FB;}_LL3FA:
 goto _LL3FC;_LL3FB: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp99D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99D->tag != 15)goto _LL3FD;}_LL3FC:
 goto _LL3FE;_LL3FD: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp99E=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99E->tag != 38)goto _LL3FF;}_LL3FE:
 goto _LL400;_LL3FF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp99F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp953;if(_tmp99F->tag != 37)goto _LL3B0;}_LL400:
# 4974
{const char*_tmpDFF;void*_tmpDFE;(_tmpDFE=0,Cyc_Tcutil_terr(e->loc,((_tmpDFF="Cyclone expression within C code",_tag_dyneither(_tmpDFF,sizeof(char),33))),_tag_dyneither(_tmpDFE,sizeof(void*),0)));}
goto _LL3B0;_LL3B0:;}
# 4979
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp9A6=d->r;void*_tmp9A7=_tmp9A6;struct Cyc_Absyn_Vardecl*_tmp9A9;struct Cyc_Absyn_Fndecl*_tmp9AB;struct Cyc_Absyn_Aggrdecl*_tmp9AD;struct Cyc_Absyn_Enumdecl*_tmp9AF;struct Cyc_Absyn_Typedefdecl*_tmp9B1;_LL402: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9A8=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9A8->tag != 0)goto _LL404;else{_tmp9A9=_tmp9A8->f1;}}_LL403:
# 4982
 _tmp9A9->type=Cyc_Toc_typ_to_c(_tmp9A9->type);
if(_tmp9A9->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9A9->initializer));
goto _LL401;_LL404: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9AA=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9AA->tag != 1)goto _LL406;else{_tmp9AB=_tmp9AA->f1;}}_LL405:
# 4986
 _tmp9AB->ret_type=Cyc_Toc_typ_to_c(_tmp9AB->ret_type);
{struct Cyc_List_List*_tmp9BC=_tmp9AB->args;for(0;_tmp9BC != 0;_tmp9BC=_tmp9BC->tl){
(*((struct _tuple9*)_tmp9BC->hd)).f3=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp9BC->hd)).f3);}}
# 4990
goto _LL401;_LL406: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9AC=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9AC->tag != 5)goto _LL408;else{_tmp9AD=_tmp9AC->f1;}}_LL407:
 Cyc_Toc_aggrdecl_to_c(_tmp9AD,1);goto _LL401;_LL408: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9AE=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9AE->tag != 7)goto _LL40A;else{_tmp9AF=_tmp9AE->f1;}}_LL409:
# 4993
 if(_tmp9AF->fields != 0){
struct Cyc_List_List*_tmp9BD=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp9AF->fields))->v;for(0;_tmp9BD != 0;_tmp9BD=_tmp9BD->tl){
struct Cyc_Absyn_Enumfield*_tmp9BE=(struct Cyc_Absyn_Enumfield*)_tmp9BD->hd;
if(_tmp9BE->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9BE->tag));}}
# 4998
goto _LL401;_LL40A: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9B0=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B0->tag != 8)goto _LL40C;else{_tmp9B1=_tmp9B0->f1;}}_LL40B:
 _tmp9B1->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9B1->defn));goto _LL401;_LL40C: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9B2=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B2->tag != 2)goto _LL40E;}_LL40D:
 goto _LL40F;_LL40E: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9B3=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B3->tag != 3)goto _LL410;}_LL40F:
 goto _LL411;_LL410: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9B4=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B4->tag != 6)goto _LL412;}_LL411:
 goto _LL413;_LL412: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9B5=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B5->tag != 9)goto _LL414;}_LL413:
 goto _LL415;_LL414: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp9B6=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B6->tag != 10)goto _LL416;}_LL415:
 goto _LL417;_LL416: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp9B7=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B7->tag != 11)goto _LL418;}_LL417:
 goto _LL419;_LL418: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp9B8=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B8->tag != 12)goto _LL41A;}_LL419:
 goto _LL41B;_LL41A: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9B9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9B9->tag != 4)goto _LL41C;}_LL41B:
# 5009
{const char*_tmpE02;void*_tmpE01;(_tmpE01=0,Cyc_Tcutil_terr(d->loc,((_tmpE02="Cyclone declaration within C code",_tag_dyneither(_tmpE02,sizeof(char),34))),_tag_dyneither(_tmpE01,sizeof(void*),0)));}
goto _LL401;_LL41C: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9BA=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9BA->tag != 13)goto _LL41E;}_LL41D:
 goto _LL41F;_LL41E: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9BB=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9A7;if(_tmp9BB->tag != 14)goto _LL401;}_LL41F:
# 5013
 goto _LL401;_LL401:;}
# 5017
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp9C1=s->r;void*_tmp9C2=_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Stmt*_tmp9C6;struct Cyc_Absyn_Stmt*_tmp9C7;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9CB;struct Cyc_Absyn_Stmt*_tmp9CC;struct Cyc_Absyn_Stmt*_tmp9CD;struct Cyc_Absyn_Exp*_tmp9CF;struct Cyc_Absyn_Stmt*_tmp9D0;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Stmt*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D7;struct Cyc_List_List*_tmp9D8;struct Cyc_Absyn_Decl*_tmp9DA;struct Cyc_Absyn_Stmt*_tmp9DB;struct Cyc_Absyn_Stmt*_tmp9DD;struct Cyc_Absyn_Exp*_tmp9DE;_LL421: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp9C3=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9C3->tag != 1)goto _LL423;else{_tmp9C4=_tmp9C3->f1;}}_LL422:
 Cyc_Toc_exptypes_to_c(_tmp9C4);goto _LL420;_LL423: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp9C5=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9C5->tag != 2)goto _LL425;else{_tmp9C6=_tmp9C5->f1;_tmp9C7=_tmp9C5->f2;}}_LL424:
 Cyc_Toc_stmttypes_to_c(_tmp9C6);Cyc_Toc_stmttypes_to_c(_tmp9C7);goto _LL420;_LL425: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp9C8=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9C8->tag != 3)goto _LL427;else{_tmp9C9=_tmp9C8->f1;}}_LL426:
 if(_tmp9C9 != 0)Cyc_Toc_exptypes_to_c(_tmp9C9);goto _LL420;_LL427: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp9CA=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9CA->tag != 4)goto _LL429;else{_tmp9CB=_tmp9CA->f1;_tmp9CC=_tmp9CA->f2;_tmp9CD=_tmp9CA->f3;}}_LL428:
# 5023
 Cyc_Toc_exptypes_to_c(_tmp9CB);Cyc_Toc_stmttypes_to_c(_tmp9CC);Cyc_Toc_stmttypes_to_c(_tmp9CD);goto _LL420;_LL429: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp9CE=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9CE->tag != 5)goto _LL42B;else{_tmp9CF=(_tmp9CE->f1).f1;_tmp9D0=_tmp9CE->f2;}}_LL42A:
# 5025
 Cyc_Toc_exptypes_to_c(_tmp9CF);Cyc_Toc_stmttypes_to_c(_tmp9D0);goto _LL420;_LL42B: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp9D1=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9D1->tag != 9)goto _LL42D;else{_tmp9D2=_tmp9D1->f1;_tmp9D3=(_tmp9D1->f2).f1;_tmp9D4=(_tmp9D1->f3).f1;_tmp9D5=_tmp9D1->f4;}}_LL42C:
# 5027
 Cyc_Toc_exptypes_to_c(_tmp9D2);Cyc_Toc_exptypes_to_c(_tmp9D3);Cyc_Toc_exptypes_to_c(_tmp9D4);
Cyc_Toc_stmttypes_to_c(_tmp9D5);goto _LL420;_LL42D: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp9D6=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9D6->tag != 10)goto _LL42F;else{_tmp9D7=_tmp9D6->f1;_tmp9D8=_tmp9D6->f2;}}_LL42E:
# 5030
 Cyc_Toc_exptypes_to_c(_tmp9D7);
for(0;_tmp9D8 != 0;_tmp9D8=_tmp9D8->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp9D8->hd)->body);}
goto _LL420;_LL42F: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp9D9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9D9->tag != 12)goto _LL431;else{_tmp9DA=_tmp9D9->f1;_tmp9DB=_tmp9D9->f2;}}_LL430:
 Cyc_Toc_decltypes_to_c(_tmp9DA);Cyc_Toc_stmttypes_to_c(_tmp9DB);goto _LL420;_LL431: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp9DC=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9DC->tag != 14)goto _LL433;else{_tmp9DD=_tmp9DC->f1;_tmp9DE=(_tmp9DC->f2).f1;}}_LL432:
 Cyc_Toc_stmttypes_to_c(_tmp9DD);Cyc_Toc_exptypes_to_c(_tmp9DE);goto _LL420;_LL433: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp9DF=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9DF->tag != 0)goto _LL435;}_LL434:
 goto _LL436;_LL435: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp9E0=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E0->tag != 6)goto _LL437;}_LL436:
 goto _LL438;_LL437: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp9E1=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E1->tag != 7)goto _LL439;}_LL438:
 goto _LL43A;_LL439: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp9E2=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E2->tag != 8)goto _LL43B;}_LL43A:
 goto _LL420;_LL43B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp9E3=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E3->tag != 11)goto _LL43D;}_LL43C:
 goto _LL43E;_LL43D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp9E4=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E4->tag != 13)goto _LL43F;}_LL43E:
 goto _LL440;_LL43F: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp9E5=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E5->tag != 15)goto _LL441;}_LL440:
 goto _LL442;_LL441: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp9E6=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp9C2;if(_tmp9E6->tag != 16)goto _LL420;}_LL442:
{const char*_tmpE05;void*_tmpE04;(_tmpE04=0,Cyc_Tcutil_terr(s->loc,((_tmpE05="Cyclone statement in C code",_tag_dyneither(_tmpE05,sizeof(char),28))),_tag_dyneither(_tmpE04,sizeof(void*),0)));}
goto _LL420;_LL420:;}
# 5051
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv)){
const char*_tmpE08;void*_tmpE07;(_tmpE07=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmpE08="decls_to_c: not at toplevel!",_tag_dyneither(_tmpE08,sizeof(char),29))),_tag_dyneither(_tmpE07,sizeof(void*),0)));}{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp9EB=d->r;void*_tmp9EC=_tmp9EB;struct Cyc_Absyn_Vardecl*_tmp9EE;struct Cyc_Absyn_Fndecl*_tmp9F0;struct Cyc_Absyn_Aggrdecl*_tmp9F5;struct Cyc_Absyn_Datatypedecl*_tmp9F7;struct Cyc_Absyn_Enumdecl*_tmp9F9;struct Cyc_Absyn_Typedefdecl*_tmp9FB;struct Cyc_List_List*_tmp9FF;struct Cyc_List_List*_tmpA01;struct Cyc_List_List*_tmpA03;struct Cyc_List_List*_tmpA05;_LL444: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp9ED=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9ED->tag != 0)goto _LL446;else{_tmp9EE=_tmp9ED->f1;}}_LL445: {
# 5058
struct _tuple1*_tmpA06=_tmp9EE->name;
# 5060
if(_tmp9EE->sc == Cyc_Absyn_ExternC){
struct _tuple1*_tmpE09;_tmpA06=((_tmpE09=_cycalloc(sizeof(*_tmpE09)),((_tmpE09->f1=Cyc_Absyn_Rel_n(0),((_tmpE09->f2=(*_tmpA06).f2,_tmpE09))))));}
if(_tmp9EE->initializer != 0){
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp9EE->initializer));else{
# 5066
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp9EE->initializer));}}
# 5068
{struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmpE0C;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmpE0B;nv=Cyc_Toc_add_varmap(r,nv,_tmp9EE->name,Cyc_Absyn_varb_exp(_tmpA06,(void*)((_tmpE0B=_cycalloc(sizeof(*_tmpE0B)),((_tmpE0B[0]=((_tmpE0C.tag=1,((_tmpE0C.f1=_tmp9EE,_tmpE0C)))),_tmpE0B)))),0));}
_tmp9EE->name=_tmpA06;
_tmp9EE->sc=Cyc_Toc_scope_to_c(_tmp9EE->sc);
_tmp9EE->type=Cyc_Toc_typ_to_c(_tmp9EE->type);
{struct Cyc_List_List*_tmpE0D;Cyc_Toc_result_decls=((_tmpE0D=_cycalloc(sizeof(*_tmpE0D)),((_tmpE0D->hd=d,((_tmpE0D->tl=Cyc_Toc_result_decls,_tmpE0D))))));}
goto _LL443;}_LL446: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp9EF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9EF->tag != 1)goto _LL448;else{_tmp9F0=_tmp9EF->f1;}}_LL447: {
# 5075
struct _tuple1*_tmpA0B=_tmp9F0->name;
# 5077
if(_tmp9F0->sc == Cyc_Absyn_ExternC){
{struct _tuple1*_tmpE0E;_tmpA0B=((_tmpE0E=_cycalloc(sizeof(*_tmpE0E)),((_tmpE0E->f1=Cyc_Absyn_Abs_n(0,1),((_tmpE0E->f2=(*_tmpA0B).f2,_tmpE0E))))));}
_tmp9F0->sc=Cyc_Absyn_Public;}
# 5081
nv=Cyc_Toc_add_varmap(r,nv,_tmp9F0->name,Cyc_Absyn_var_exp(_tmpA0B,0));
_tmp9F0->name=_tmpA0B;
Cyc_Toc_fndecl_to_c(nv,_tmp9F0,cinclude);
{struct Cyc_List_List*_tmpE0F;Cyc_Toc_result_decls=((_tmpE0F=_cycalloc(sizeof(*_tmpE0F)),((_tmpE0F->hd=d,((_tmpE0F->tl=Cyc_Toc_result_decls,_tmpE0F))))));}
goto _LL443;}_LL448: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp9F1=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F1->tag != 2)goto _LL44A;}_LL449:
 goto _LL44B;_LL44A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp9F2=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F2->tag != 3)goto _LL44C;}_LL44B: {
# 5088
const char*_tmpE12;void*_tmpE11;(_tmpE11=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE12="letdecl at toplevel",_tag_dyneither(_tmpE12,sizeof(char),20))),_tag_dyneither(_tmpE11,sizeof(void*),0)));}_LL44C: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp9F3=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F3->tag != 4)goto _LL44E;}_LL44D: {
# 5090
const char*_tmpE15;void*_tmpE14;(_tmpE14=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(((_tmpE15="region decl at toplevel",_tag_dyneither(_tmpE15,sizeof(char),24))),_tag_dyneither(_tmpE14,sizeof(void*),0)));}_LL44E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp9F4=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F4->tag != 5)goto _LL450;else{_tmp9F5=_tmp9F4->f1;}}_LL44F:
# 5094
 Cyc_Toc_aggrdecl_to_c(_tmp9F5,1);
goto _LL443;_LL450: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp9F6=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F6->tag != 6)goto _LL452;else{_tmp9F7=_tmp9F6->f1;}}_LL451:
# 5097
 if(_tmp9F7->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp9F7);else{
# 5100
Cyc_Toc_datatypedecl_to_c(_tmp9F7);}
goto _LL443;_LL452: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp9F8=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9F8->tag != 7)goto _LL454;else{_tmp9F9=_tmp9F8->f1;}}_LL453:
# 5103
 Cyc_Toc_enumdecl_to_c(_tmp9F9);
{struct Cyc_List_List*_tmpE16;Cyc_Toc_result_decls=((_tmpE16=_cycalloc(sizeof(*_tmpE16)),((_tmpE16->hd=d,((_tmpE16->tl=Cyc_Toc_result_decls,_tmpE16))))));}
goto _LL443;_LL454: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp9FA=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9FA->tag != 8)goto _LL456;else{_tmp9FB=_tmp9FA->f1;}}_LL455:
# 5107
 _tmp9FB->name=_tmp9FB->name;
_tmp9FB->tvs=0;
if(_tmp9FB->defn != 0)
_tmp9FB->defn=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9FB->defn));else{
# 5112
enum Cyc_Absyn_KindQual _tmpA13=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp9FB->kind))->v)->kind;switch(_tmpA13){case Cyc_Absyn_BoxKind: _LL462:
 _tmp9FB->defn=Cyc_Absyn_void_star_typ();break;default: _LL463:
 _tmp9FB->defn=(void*)& Cyc_Absyn_VoidType_val;break;}}
# 5117
{struct Cyc_List_List*_tmpE17;Cyc_Toc_result_decls=((_tmpE17=_cycalloc(sizeof(*_tmpE17)),((_tmpE17->hd=d,((_tmpE17->tl=Cyc_Toc_result_decls,_tmpE17))))));}
goto _LL443;_LL456: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp9FC=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9FC->tag != 13)goto _LL458;}_LL457:
 goto _LL459;_LL458: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp9FD=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9FD->tag != 14)goto _LL45A;}_LL459:
# 5121
 goto _LL443;_LL45A: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp9FE=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmp9FE->tag != 9)goto _LL45C;else{_tmp9FF=_tmp9FE->f2;}}_LL45B:
 _tmpA01=_tmp9FF;goto _LL45D;_LL45C: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmpA00=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmpA00->tag != 10)goto _LL45E;else{_tmpA01=_tmpA00->f2;}}_LL45D:
 _tmpA03=_tmpA01;goto _LL45F;_LL45E: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmpA02=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmpA02->tag != 11)goto _LL460;else{_tmpA03=_tmpA02->f1;}}_LL45F:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA03,top,cinclude);goto _LL443;_LL460: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmpA04=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp9EC;if(_tmpA04->tag != 12)goto _LL443;else{_tmpA05=_tmpA04->f1;}}_LL461:
 nv=Cyc_Toc_decls_to_c(r,nv,_tmpA05,top,1);goto _LL443;_LL443:;};}
# 5128
return nv;}
# 5132
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmpA15=Cyc_Core_new_rckey();struct Cyc_Core_DynamicRegion*_tmpA17;struct Cyc_Core_NewDynamicRegion _tmpA16=_tmpA15;_tmpA17=_tmpA16.key;{
struct Cyc_Toc_TocState*_tmpA18=((struct Cyc_Toc_TocState*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Toc_TocState*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpA17,0,Cyc_Toc_empty_toc_state);
{struct Cyc_Toc_TocStateWrap*_tmpE18;Cyc_Toc_toc_state=((_tmpE18=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmpE18)),((_tmpE18->dyn=(struct Cyc_Core_DynamicRegion*)_tmpA17,((_tmpE18->state=(struct Cyc_Toc_TocState*)_tmpA18,_tmpE18))))));}
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0;
Cyc_Toc_bounds_checks_eliminated=0;{
struct _dyneither_ptr**_tmpE19;Cyc_Toc_globals=_tag_dyneither(((_tmpE19=_cycalloc(sizeof(struct _dyneither_ptr*)* 41),((_tmpE19[0]=(struct _dyneither_ptr*)& Cyc_Toc__throw_str,((_tmpE19[1]=(struct _dyneither_ptr*)& Cyc_Toc_setjmp_str,((_tmpE19[2]=(struct _dyneither_ptr*)& Cyc_Toc__push_handler_str,((_tmpE19[3]=(struct _dyneither_ptr*)& Cyc_Toc__pop_handler_str,((_tmpE19[4]=(struct _dyneither_ptr*)& Cyc_Toc__exn_thrown_str,((_tmpE19[5]=(struct _dyneither_ptr*)& Cyc_Toc__npop_handler_str,((_tmpE19[6]=(struct _dyneither_ptr*)& Cyc_Toc__check_null_str,((_tmpE19[7]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_null_str,((_tmpE19[8]=(struct _dyneither_ptr*)& Cyc_Toc__check_known_subscript_notnull_str,((_tmpE19[9]=(struct _dyneither_ptr*)& Cyc_Toc__check_dyneither_subscript_str,((_tmpE19[10]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_str,((_tmpE19[11]=(struct _dyneither_ptr*)& Cyc_Toc__tag_dyneither_str,((_tmpE19[12]=(struct _dyneither_ptr*)& Cyc_Toc__init_dyneither_ptr_str,((_tmpE19[13]=(struct _dyneither_ptr*)& Cyc_Toc__untag_dyneither_ptr_str,((_tmpE19[14]=(struct _dyneither_ptr*)& Cyc_Toc__get_dyneither_size_str,((_tmpE19[15]=(struct _dyneither_ptr*)& Cyc_Toc__get_zero_arr_size_str,((_tmpE19[16]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_plus_str,((_tmpE19[17]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_plus_str,((_tmpE19[18]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_str,((_tmpE19[19]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_str,((_tmpE19[20]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,((_tmpE19[21]=(struct _dyneither_ptr*)& Cyc_Toc__zero_arr_inplace_plus_post_str,((_tmpE19[22]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_str,((_tmpE19[23]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_str,((_tmpE19[24]=(struct _dyneither_ptr*)& Cyc_Toc__cycalloc_atomic_str,((_tmpE19[25]=(struct _dyneither_ptr*)& Cyc_Toc__cyccalloc_atomic_str,((_tmpE19[26]=(struct _dyneither_ptr*)& Cyc_Toc__region_malloc_str,((_tmpE19[27]=(struct _dyneither_ptr*)& Cyc_Toc__region_calloc_str,((_tmpE19[28]=(struct _dyneither_ptr*)& Cyc_Toc__check_times_str,((_tmpE19[29]=(struct _dyneither_ptr*)& Cyc_Toc__new_region_str,((_tmpE19[30]=(struct _dyneither_ptr*)& Cyc_Toc__push_region_str,((_tmpE19[31]=(struct _dyneither_ptr*)& Cyc_Toc__pop_region_str,((_tmpE19[32]=(struct _dyneither_ptr*)& Cyc_Toc__open_dynregion_str,((_tmpE19[33]=(struct _dyneither_ptr*)& Cyc_Toc__push_dynregion_str,((_tmpE19[34]=(struct _dyneither_ptr*)& Cyc_Toc__pop_dynregion_str,((_tmpE19[35]=(struct _dyneither_ptr*)& Cyc_Toc__reset_region_str,((_tmpE19[36]=(struct _dyneither_ptr*)& Cyc_Toc__throw_arraybounds_str,((_tmpE19[37]=(struct _dyneither_ptr*)& Cyc_Toc__dyneither_ptr_decrease_size_str,((_tmpE19[38]=(struct _dyneither_ptr*)& Cyc_Toc__throw_match_str,((_tmpE19[39]=(struct _dyneither_ptr*)& Cyc_Toc__swap_word_str,((_tmpE19[40]=(struct _dyneither_ptr*)& Cyc_Toc__swap_dyneither_str,_tmpE19)))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))))),sizeof(struct _dyneither_ptr*),41);};};}
# 5188
static int Cyc_Toc_destroy_labels(struct _RegionHandle*d,struct Cyc_Toc_TocState*ts){
struct Cyc_Toc_TocState _tmpA1B=*ts;struct Cyc_Xarray_Xarray*_tmpA1D;struct Cyc_Toc_TocState _tmpA1C=_tmpA1B;_tmpA1D=_tmpA1C.temp_labels;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmpA1D);
return 0;}
# 5196
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_List_List*ds){
Cyc_Toc_init();{
struct _RegionHandle _tmpA1E=_new_region("start");struct _RegionHandle*start=& _tmpA1E;_push_region(start);
Cyc_Toc_decls_to_c(start,Cyc_Toc_empty_env(start),ds,1,0);{
# 5205
struct Cyc_Toc_TocStateWrap*ts=0;
_swap_word(& ts,& Cyc_Toc_toc_state);{
struct Cyc_Toc_TocStateWrap _tmpA1F=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Core_DynamicRegion*_tmpA21;struct Cyc_Toc_TocState*_tmpA22;struct Cyc_Toc_TocStateWrap _tmpA20=_tmpA1F;_tmpA21=_tmpA20.dyn;_tmpA22=_tmpA20.state;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct Cyc_Toc_TocState*arg,int(*body)(struct _RegionHandle*h,struct Cyc_Toc_TocState*arg)))Cyc_Core_open_region)(_tmpA21,_tmpA22,Cyc_Toc_destroy_labels);
Cyc_Core_free_rckey(_tmpA21);
((void(*)(struct _dyneither_ptr ptr))Cyc_Core_ufree)(_tag_dyneither(ts,sizeof(struct Cyc_Toc_TocStateWrap),1));};};{
# 5212
struct Cyc_List_List*_tmpA23=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);_npop_handler(0);return _tmpA23;};
# 5198
;_pop_region(start);};}
