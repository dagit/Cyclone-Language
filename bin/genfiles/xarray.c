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
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 150 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 152
extern struct _RegionHandle*Cyc_Core_unique_region;
# 159
void Cyc_Core_ufree(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;
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
return dyn.curr;};}struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 38 "xarray.h"
typedef struct Cyc_Xarray_Xarray*Cyc_Xarray_xarray_t;
# 40
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty();
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 60
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int,void*);
# 63
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 72
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*);
# 75
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*,struct Cyc_Xarray_Xarray*);
# 78
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr);
# 81
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*,struct _dyneither_ptr arr);
# 85
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 89
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*,struct Cyc_Xarray_Xarray*,struct Cyc_Xarray_Xarray*);
# 93
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 97
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 100
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*);
# 104
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 107
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 110
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Xarray_Xarray*);
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 117
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*,struct Cyc_Xarray_Xarray*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);
# 124
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num);
# 126
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i);
# 25 "xarray.cyc"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*xarr){
return xarr->num_elmts;}
# 29
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i >= xarr->num_elmts){
struct Cyc_Core_Invalid_argument_exn_struct _tmp3D;const char*_tmp3C;struct Cyc_Core_Invalid_argument_exn_struct*_tmp3B;(int)_throw((void*)((_tmp3B=_cycalloc(sizeof(*_tmp3B)),((_tmp3B[0]=((_tmp3D.tag=Cyc_Core_Invalid_argument,((_tmp3D.f1=((_tmp3C="Xarray::get: bad index",_tag_dyneither(_tmp3C,sizeof(char),23))),_tmp3D)))),_tmp3B)))));}
return*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));}
# 35
void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*xarr,int i,void*a){
if(i < 0  || i >= xarr->num_elmts){
struct Cyc_Core_Invalid_argument_exn_struct _tmp43;const char*_tmp42;struct Cyc_Core_Invalid_argument_exn_struct*_tmp41;(int)_throw((void*)((_tmp41=_cycalloc(sizeof(*_tmp41)),((_tmp41[0]=((_tmp43.tag=Cyc_Core_Invalid_argument,((_tmp43.f1=((_tmp42="Xarray::set: bad index",_tag_dyneither(_tmp42,sizeof(char),23))),_tmp43)))),_tmp41)))));}
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i))=a;}
# 41
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*r){
struct _dyneither_ptr x=_tag_dyneither(0,0,0);
struct Cyc_Xarray_Xarray*_tmp44;return(_tmp44=_region_malloc(r,sizeof(*_tmp44)),((_tmp44->elmts=x,((_tmp44->num_elmts=0,_tmp44)))));}
# 45
struct Cyc_Xarray_Xarray*Cyc_Xarray_create_empty(){return Cyc_Xarray_rcreate_empty(Cyc_Core_heap_region);}struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a);static void _tmp50(void**a,unsigned int*_tmp4F,unsigned int*_tmp4E,void***_tmp4C){for(*_tmp4F=0;*_tmp4F < *_tmp4E;(*_tmp4F)++){(*_tmp4C)[*_tmp4F]=(void*)*a;}}
# 48
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*r,int len,void*a){
if(len < 0){
struct Cyc_Core_Invalid_argument_exn_struct _tmp4A;const char*_tmp49;struct Cyc_Core_Invalid_argument_exn_struct*_tmp48;(int)_throw((void*)((_tmp48=_cycalloc(sizeof(*_tmp48)),((_tmp48[0]=((_tmp4A.tag=Cyc_Core_Invalid_argument,((_tmp4A.f1=((_tmp49="xarrays must have a non-negative size buffer",_tag_dyneither(_tmp49,sizeof(char),45))),_tmp4A)))),_tmp48)))));}{
unsigned int _tmp56;void**_tmp55;struct _dyneither_ptr _tmp54;unsigned int _tmp53;unsigned int _tmp52;struct Cyc_Xarray_Xarray*_tmp51;return(_tmp51=_region_malloc(r,sizeof(*_tmp51)),((_tmp51->elmts=((_tmp56=(unsigned int)len,((_tmp55=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp56)),((_tmp54=_tag_dyneither(_tmp55,sizeof(void*),_tmp56),((((_tmp53=_tmp56,_tmp50(& a,& _tmp52,& _tmp53,& _tmp55))),_tmp54)))))))),((_tmp51->num_elmts=0,_tmp51)))));};}
# 54
struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int len,void*a){
return Cyc_Xarray_rcreate(Cyc_Core_heap_region,len,a);}
# 58
struct Cyc_Xarray_Xarray*Cyc_Xarray_rsingleton(struct _RegionHandle*r,int len,void*a){
if(len < 1){struct Cyc_Core_Invalid_argument_exn_struct _tmp5C;const char*_tmp5B;struct Cyc_Core_Invalid_argument_exn_struct*_tmp5A;(int)_throw((void*)((_tmp5A=_cycalloc(sizeof(*_tmp5A)),((_tmp5A[0]=((_tmp5C.tag=Cyc_Core_Invalid_argument,((_tmp5C.f1=((_tmp5B="singleton xarray must have size >=1",_tag_dyneither(_tmp5B,sizeof(char),36))),_tmp5C)))),_tmp5A)))));}{
struct Cyc_Xarray_Xarray*x=Cyc_Xarray_rcreate(r,len,a);
x->num_elmts=1;
return x;};}
# 65
struct Cyc_Xarray_Xarray*Cyc_Xarray_singleton(int len,void*a){
return Cyc_Xarray_rsingleton(Cyc_Core_heap_region,len,a);}void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a);static void _tmp61(void**a,unsigned int*_tmp60,unsigned int*_tmp5F,void***_tmp5E){for(*_tmp60=0;*_tmp60 < *_tmp5F;(*_tmp60)++){(*_tmp5E)[*_tmp60]=(void*)*a;}}static void _tmp67(struct Cyc_Xarray_Xarray**xarr,unsigned int*_tmp66,unsigned int*_tmp65,void***_tmp63){for(*_tmp66=0;*_tmp66 < *_tmp65;(*_tmp66)++){(*_tmp63)[*_tmp66]=(void*)*((void**)_check_dyneither_subscript((*xarr)->elmts,sizeof(void*),0));}}
# 69
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*xarr,void*a){
if(xarr->num_elmts == _get_dyneither_size(xarr->elmts,sizeof(void*))){
if(xarr->num_elmts == 0){
unsigned int _tmp60;unsigned int _tmp5F;void**_tmp5E;unsigned int _tmp5D;xarr->elmts=_tag_dyneither(((_tmp5D=(unsigned int)10,((_tmp5E=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp5D)),((((_tmp5F=_tmp5D,_tmp61(& a,& _tmp60,& _tmp5F,& _tmp5E))),_tmp5E)))))),sizeof(void*),(unsigned int)10);}else{
# 74
unsigned int _tmp66;unsigned int _tmp65;struct _dyneither_ptr _tmp64;void**_tmp63;unsigned int _tmp62;struct _dyneither_ptr newarr=(_tmp62=(unsigned int)(xarr->num_elmts * 2),((_tmp63=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp62)),((_tmp64=_tag_dyneither(_tmp63,sizeof(void*),_tmp62),((((_tmp65=_tmp62,_tmp67(& xarr,& _tmp66,& _tmp65,& _tmp63))),_tmp64)))))));
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(newarr,sizeof(void*),i))=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i));}}
_swap_dyneither(& xarr->elmts,& newarr);
Cyc_Core_ufree(newarr);}}
# 81
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),xarr->num_elmts ++))=a;}
# 84
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*xarr,void*a){
Cyc_Xarray_add(xarr,a);
return xarr->num_elmts - 1;}struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr);static void _tmp6E(struct Cyc_Xarray_Xarray**xarr,unsigned int*_tmp6D,unsigned int*_tmp6C,void***_tmp6A){for(*_tmp6D=0;*_tmp6D < *_tmp6C;(*_tmp6D)++){(*_tmp6A)[*_tmp6D]=(void*)*((void**)_check_dyneither_subscript((*xarr)->elmts,sizeof(void*),(int)*_tmp6D));}}
# 89
struct _dyneither_ptr Cyc_Xarray_rto_array(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0){
void**_tmp68;return _tag_dyneither(((_tmp68=_region_malloc(r,sizeof(void*)* 0),_tmp68)),sizeof(void*),0);}{
unsigned int _tmp6D;unsigned int _tmp6C;struct _dyneither_ptr _tmp6B;void**_tmp6A;unsigned int _tmp69;struct _dyneither_ptr ans=(_tmp69=(unsigned int)xarr->num_elmts,((_tmp6A=(void**)_region_malloc(r,_check_times(sizeof(void*),_tmp69)),((_tmp6B=_tag_dyneither(_tmp6A,sizeof(void*),_tmp69),((((_tmp6C=_tmp69,_tmp6E(& xarr,& _tmp6D,& _tmp6C,& _tmp6A))),_tmp6B)))))));
return ans;};}
# 96
struct _dyneither_ptr Cyc_Xarray_to_array(struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rto_array(Cyc_Core_heap_region,xarr);}struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _dyneither_ptr arr);static void _tmp74(struct _dyneither_ptr*arr,unsigned int*_tmp73,unsigned int*_tmp72,void***_tmp70){for(*_tmp73=0;*_tmp73 < *_tmp72;(*_tmp73)++){(*_tmp70)[*_tmp73]=(void*)((void**)(*arr).curr)[(int)*_tmp73];}}
# 100
struct Cyc_Xarray_Xarray*Cyc_Xarray_rfrom_array(struct _RegionHandle*r,struct _dyneither_ptr arr){
if(_get_dyneither_size(arr,sizeof(void*))== 0)
return Cyc_Xarray_rcreate_empty(r);{
unsigned int _tmp7A;void**_tmp79;struct _dyneither_ptr _tmp78;unsigned int _tmp77;unsigned int _tmp76;struct Cyc_Xarray_Xarray*_tmp75;struct Cyc_Xarray_Xarray*ans=(_tmp75=_region_malloc(r,sizeof(*_tmp75)),((_tmp75->elmts=(
(_tmp7A=_get_dyneither_size(arr,sizeof(void*)),((_tmp79=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp7A)),((_tmp78=_tag_dyneither(_tmp79,sizeof(void*),_tmp7A),((((_tmp77=_tmp7A,_tmp74(& arr,& _tmp76,& _tmp77,& _tmp79))),_tmp78)))))))),((_tmp75->num_elmts=(int)
_get_dyneither_size(arr,sizeof(void*)),_tmp75)))));
return ans;};}
# 109
struct Cyc_Xarray_Xarray*Cyc_Xarray_from_array(struct _dyneither_ptr arr){
return Cyc_Xarray_rfrom_array(Cyc_Core_heap_region,arr);}struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2);static void _tmp80(void**init,unsigned int*_tmp7F,unsigned int*_tmp7E,void***_tmp7C){for(*_tmp7F=0;*_tmp7F < *_tmp7E;(*_tmp7F)++){(*_tmp7C)[*_tmp7F]=(void*)*init;}}
# 114
struct Cyc_Xarray_Xarray*Cyc_Xarray_rappend(struct _RegionHandle*r,struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
int newsz=(int)(_get_dyneither_size(xarr1->elmts,sizeof(void*))+ _get_dyneither_size(xarr2->elmts,sizeof(void*)));
if(newsz == 0)
return Cyc_Xarray_rcreate_empty(r);{
void*init=_get_dyneither_size(xarr1->elmts,sizeof(void*))== 0?*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),0)):*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),0));
unsigned int _tmp86;void**_tmp85;struct _dyneither_ptr _tmp84;unsigned int _tmp83;unsigned int _tmp82;struct Cyc_Xarray_Xarray*_tmp81;struct Cyc_Xarray_Xarray*ans=(_tmp81=_region_malloc(r,sizeof(*_tmp81)),((_tmp81->elmts=((_tmp86=(unsigned int)newsz,((_tmp85=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp86)),((_tmp84=_tag_dyneither(_tmp85,sizeof(void*),_tmp86),((((_tmp83=_tmp86,_tmp80(& init,& _tmp82,& _tmp83,& _tmp85))),_tmp84)))))))),((_tmp81->num_elmts=0,_tmp81)))));
# 121
{int i=0;for(0;i < xarr1->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr1->elmts,sizeof(void*),i)));}}
{int i=0;for(0;i < xarr2->num_elmts;++ i){
Cyc_Xarray_add(ans,*((void**)_check_dyneither_subscript(xarr2->elmts,sizeof(void*),i)));}}
return ans;};}
# 128
struct Cyc_Xarray_Xarray*Cyc_Xarray_append(struct Cyc_Xarray_Xarray*xarr1,struct Cyc_Xarray_Xarray*xarr2){
return Cyc_Xarray_rappend(Cyc_Core_heap_region,xarr1,xarr2);}
# 132
void Cyc_Xarray_app(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 137
void Cyc_Xarray_app_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 142
void Cyc_Xarray_iter(void(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
# 147
void Cyc_Xarray_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
int i=0;for(0;i < xarr->num_elmts;++ i){
f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr);static void _tmp8C(struct Cyc_Xarray_Xarray**xarr,void*(**f)(void*),unsigned int*_tmp8B,unsigned int*_tmp8A,void***_tmp88){for(*_tmp8B=0;*_tmp8B < *_tmp8A;(*_tmp8B)++){(*_tmp88)[*_tmp8B]=(void*)
# 156
(*f)(*((void**)_check_dyneither_subscript((*xarr)->elmts,sizeof(void*),0)));}}
# 152
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 155
unsigned int _tmp92;void**_tmp91;struct _dyneither_ptr _tmp90;unsigned int _tmp8F;unsigned int _tmp8E;struct Cyc_Xarray_Xarray*_tmp8D;struct Cyc_Xarray_Xarray*ans=
(_tmp8D=_region_malloc(r,sizeof(*_tmp8D)),((_tmp8D->elmts=((_tmp92=_get_dyneither_size(xarr->elmts,sizeof(void*)),((_tmp91=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp92)),((_tmp90=_tag_dyneither(_tmp91,sizeof(void*),_tmp92),((((_tmp8F=_tmp92,_tmp8C(& xarr,& f,& _tmp8E,& _tmp8F,& _tmp91))),_tmp90)))))))),((_tmp8D->num_elmts=xarr->num_elmts,_tmp8D)))));
# 158
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=f(*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
return ans;};}
# 163
struct Cyc_Xarray_Xarray*Cyc_Xarray_map(void*(*f)(void*),struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap(Cyc_Core_heap_region,f,xarr);}struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr);static void _tmp98(struct Cyc_Xarray_Xarray**xarr,void*(**f)(void*,void*),void**env,unsigned int*_tmp97,unsigned int*_tmp96,void***_tmp94){for(*_tmp97=0;*_tmp97 < *_tmp96;(*_tmp97)++){(*_tmp94)[*_tmp97]=(void*)
# 171
(*f)(*env,*((void**)_check_dyneither_subscript((*xarr)->elmts,sizeof(void*),0)));}}
# 167
struct Cyc_Xarray_Xarray*Cyc_Xarray_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
if(xarr->num_elmts == 0)return Cyc_Xarray_rcreate_empty(r);{
# 170
unsigned int _tmp9E;void**_tmp9D;struct _dyneither_ptr _tmp9C;unsigned int _tmp9B;unsigned int _tmp9A;struct Cyc_Xarray_Xarray*_tmp99;struct Cyc_Xarray_Xarray*ans=
(_tmp99=_region_malloc(r,sizeof(*_tmp99)),((_tmp99->elmts=((_tmp9E=_get_dyneither_size(xarr->elmts,sizeof(void*)),((_tmp9D=(void**)_region_malloc(Cyc_Core_unique_region,_check_times(sizeof(void*),_tmp9E)),((_tmp9C=_tag_dyneither(_tmp9D,sizeof(void*),_tmp9E),((((_tmp9B=_tmp9E,_tmp98(& xarr,& f,& env,& _tmp9A,& _tmp9B,& _tmp9D))),_tmp9C)))))))),((_tmp99->num_elmts=xarr->num_elmts,_tmp99)))));
# 173
{int i=1;for(0;i < xarr->num_elmts;++ i){
*((void**)_check_dyneither_subscript(ans->elmts,sizeof(void*),i))=f(env,*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),i)));}}
return ans;};}
# 178
struct Cyc_Xarray_Xarray*Cyc_Xarray_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Xarray_Xarray*xarr){
return Cyc_Xarray_rmap_c(Cyc_Core_heap_region,f,env,xarr);}
# 182
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr){
struct _dyneither_ptr newarr=_tag_dyneither(0,0,0);
_swap_dyneither(& newarr,& xarr->elmts);
xarr->num_elmts=0;
Cyc_Core_ufree(newarr);}
# 189
void Cyc_Xarray_delete(struct Cyc_Xarray_Xarray*xarr,int num){
if(num > Cyc_Xarray_length(xarr)){
struct Cyc_Core_Invalid_argument_exn_struct _tmpA4;const char*_tmpA3;struct Cyc_Core_Invalid_argument_exn_struct*_tmpA2;(int)_throw((void*)((_tmpA2=_cycalloc(sizeof(*_tmpA2)),((_tmpA2[0]=((_tmpA4.tag=Cyc_Core_Invalid_argument,((_tmpA4.f1=((_tmpA3="number deleted is greater than length of xarray",_tag_dyneither(_tmpA3,sizeof(char),48))),_tmpA4)))),_tmpA2)))));}
xarr->num_elmts -=num;}
# 195
void Cyc_Xarray_remove(struct Cyc_Xarray_Xarray*xarr,int i){
if(i < 0  || i > xarr->num_elmts - 1){
struct Cyc_Core_Invalid_argument_exn_struct _tmpAA;const char*_tmpA9;struct Cyc_Core_Invalid_argument_exn_struct*_tmpA8;(int)_throw((void*)((_tmpA8=_cycalloc(sizeof(*_tmpA8)),((_tmpA8[0]=((_tmpAA.tag=Cyc_Core_Invalid_argument,((_tmpAA.f1=((_tmpA9="xarray index out of bounds",_tag_dyneither(_tmpA9,sizeof(char),27))),_tmpAA)))),_tmpA8)))));}
{int j=i;for(0;j < xarr->num_elmts - 2;++ j){
*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j))=*((void**)_check_dyneither_subscript(xarr->elmts,sizeof(void*),j + 1));}}
-- xarr->num_elmts;}
