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
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 256 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 261
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 268
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
# 32 "lineno.h"
typedef struct Cyc_Lineno_Pos*Cyc_Lineno_pos_t;extern char Cyc_Position_Exit[5];struct Cyc_Position_Exit_exn_struct{char*tag;};
# 37 "position.h"
typedef unsigned int Cyc_Position_seg_t;
unsigned int Cyc_Position_segment_join(unsigned int,unsigned int);struct Cyc_Position_Lex_Position_Error_kind_struct{int tag;};struct Cyc_Position_Parse_Position_Error_kind_struct{int tag;};struct Cyc_Position_Elab_Position_Error_kind_struct{int tag;};
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 99
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
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
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;
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
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 321
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 338
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);
# 340
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 352
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
extern struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val;
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 523
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr);
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr);
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 688 "absyn.h"
extern struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val;struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 750
extern struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val;struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 886
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};char Cyc_Absyn_EmptyAnnot[11]="EmptyAnnot";struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 901
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 908
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 913
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 916
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 921
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 925
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 930
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 936
void*Cyc_Absyn_compress_kb(void*);
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 943
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 946
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 948
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 950
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 952
extern void*Cyc_Absyn_empty_effect;
# 954
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 960
void*Cyc_Absyn_exn_typ();
# 962
extern struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar;
# 965
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 968
void*Cyc_Absyn_file_typ();
# 970
extern struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one;
extern void*Cyc_Absyn_bounds_one;
# 973
void*Cyc_Absyn_starb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 976
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 979
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 982
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 985
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 987
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 990
void*Cyc_Absyn_void_star_typ();
# 992
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 996
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name);
# 998
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*,void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1069
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1096
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1100
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,int,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1111
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1114
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1117
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1120
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1123
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1128
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1134
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts);
# 1144
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1146
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1149
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1151
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple10{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1153
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1157
int Cyc_Absyn_fntype_att(void*a);
# 1159
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1161
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1163
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);
# 1165
int Cyc_Absyn_is_union_type(void*);
# 1167
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);
# 1169
int Cyc_Absyn_is_require_union_type(void*);
# 1171
int Cyc_Absyn_is_aggr_type(void*t);
# 1177
extern int Cyc_Absyn_porting_c_code;
# 1179
extern int Cyc_Absyn_no_regions;
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 40 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 51
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Set_Set*namespaces;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;struct Cyc_List_List*availables;};
# 70
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 74
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_NotLoop_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_CaseEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_FnEnd_j_Tcenv_Jumpee_struct{int tag;};struct Cyc_Tcenv_Stmt_j_Tcenv_Jumpee_struct{int tag;struct Cyc_Absyn_Stmt*f1;};
# 85
typedef void*Cyc_Tcenv_jumpee_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Dict_Dict ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;};
# 96
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 130 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 66
void*Cyc_Tcutil_compress(void*t);
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 131
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 25 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 27
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 33 "absyn.cyc"
int Cyc_Cyclone_tovc_r=0;
# 35
enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
# 44
static int Cyc_Absyn_strlist_cmp(struct Cyc_List_List*ss1,struct Cyc_List_List*ss2){
for(0;ss1 != 0;ss1=ss1->tl){
if(ss2 == 0)return 1;{
int i=Cyc_strptrcmp((struct _dyneither_ptr*)ss1->hd,(struct _dyneither_ptr*)ss2->hd);
if(i != 0)return i;
ss2=ss2->tl;};}
# 51
if(ss2 != 0)return - 1;
return 0;}
# 54
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*vs1,struct Cyc_List_List*vs2){
if((int)vs1 == (int)vs2)return 0;
return Cyc_Absyn_strlist_cmp(vs1,vs2);}struct _tuple12{union Cyc_Absyn_Nmspace f1;union Cyc_Absyn_Nmspace f2;};
# 58
int Cyc_Absyn_qvar_cmp(struct _tuple0*q1,struct _tuple0*q2){
if(q1 == q2)return 0;{
union Cyc_Absyn_Nmspace _tmp1;struct _dyneither_ptr*_tmp2;struct _tuple0*_tmp0=q1;_tmp1=_tmp0->f1;_tmp2=_tmp0->f2;{
union Cyc_Absyn_Nmspace _tmp4;struct _dyneither_ptr*_tmp5;struct _tuple0*_tmp3=q2;_tmp4=_tmp3->f1;_tmp5=_tmp3->f2;{
int i=Cyc_strptrcmp(_tmp2,_tmp5);
if(i != 0)return i;{
struct _tuple12 _tmp209;struct _tuple12 _stmttmp0=(_tmp209.f1=_tmp1,((_tmp209.f2=_tmp4,_tmp209)));struct _tuple12 _tmp6=_stmttmp0;struct Cyc_List_List*_tmp7;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpC;_LL1: if(((_tmp6.f1).Loc_n).tag != 4)goto _LL3;if(((_tmp6.f2).Loc_n).tag != 4)goto _LL3;_LL2:
 return 0;_LL3: if(((_tmp6.f1).Rel_n).tag != 1)goto _LL5;_tmp7=(struct Cyc_List_List*)((_tmp6.f1).Rel_n).val;if(((_tmp6.f2).Rel_n).tag != 1)goto _LL5;_tmp8=(struct Cyc_List_List*)((_tmp6.f2).Rel_n).val;_LL4:
 return Cyc_Absyn_strlist_cmp(_tmp7,_tmp8);_LL5: if(((_tmp6.f1).Abs_n).tag != 2)goto _LL7;_tmp9=(struct Cyc_List_List*)((_tmp6.f1).Abs_n).val;if(((_tmp6.f2).Abs_n).tag != 2)goto _LL7;_tmpA=(struct Cyc_List_List*)((_tmp6.f2).Abs_n).val;_LL6:
 return Cyc_Absyn_strlist_cmp(_tmp9,_tmpA);_LL7: if(((_tmp6.f1).C_n).tag != 3)goto _LL9;_tmpB=(struct Cyc_List_List*)((_tmp6.f1).C_n).val;if(((_tmp6.f2).C_n).tag != 3)goto _LL9;_tmpC=(struct Cyc_List_List*)((_tmp6.f2).C_n).val;_LL8:
 return Cyc_Absyn_strlist_cmp(_tmpB,_tmpC);_LL9: if(((_tmp6.f1).Loc_n).tag != 4)goto _LLB;_LLA:
# 70
 return - 1;_LLB: if(((_tmp6.f2).Loc_n).tag != 4)goto _LLD;_LLC:
 return 1;_LLD: if(((_tmp6.f1).Rel_n).tag != 1)goto _LLF;_LLE:
 return - 1;_LLF: if(((_tmp6.f2).Rel_n).tag != 1)goto _LL11;_LL10:
 return 1;_LL11: if(((_tmp6.f1).Abs_n).tag != 2)goto _LL13;_LL12:
 return - 1;_LL13: if(((_tmp6.f2).Abs_n).tag != 2)goto _LL0;_LL14:
 return 1;_LL0:;};};};};}
# 79
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
int i=Cyc_strptrcmp(tv1->name,tv2->name);
if(i != 0)return i;
return tv1->identity - tv2->identity;}
# 85
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n={.Loc_n={4,0}};
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*x,int C_scope){
if(C_scope){
union Cyc_Absyn_Nmspace _tmp20A;return((_tmp20A.C_n).val=x,(((_tmp20A.C_n).tag=3,_tmp20A)));}else{
# 90
union Cyc_Absyn_Nmspace _tmp20B;return((_tmp20B.Abs_n).val=x,(((_tmp20B.Abs_n).tag=2,_tmp20B)));}}
# 92
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp20C;return((_tmp20C.Rel_n).val=x,(((_tmp20C.Rel_n).tag=1,_tmp20C)));}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 95
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _stmttmp1=(*qv).f1;union Cyc_Absyn_Nmspace _tmp11=_stmttmp1;_LL16: if((_tmp11.Rel_n).tag != 1)goto _LL18;if((struct Cyc_List_List*)(_tmp11.Rel_n).val != 0)goto _LL18;_LL17:
 goto _LL19;_LL18: if((_tmp11.Abs_n).tag != 2)goto _LL1A;if((struct Cyc_List_List*)(_tmp11.Abs_n).val != 0)goto _LL1A;_LL19:
 goto _LL1B;_LL1A: if((_tmp11.Loc_n).tag != 4)goto _LL1C;_LL1B:
 return 0;_LL1C:;_LL1D:
 return 1;_LL15:;}
# 104
static int Cyc_Absyn_new_type_counter=0;
# 106
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp20F;struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp20E;return(void*)((_tmp20E=_cycalloc(sizeof(*_tmp20E)),((_tmp20E[0]=((_tmp20F.tag=1,((_tmp20F.f1=k,((_tmp20F.f2=0,((_tmp20F.f3=Cyc_Absyn_new_type_counter ++,((_tmp20F.f4=env,_tmp20F)))))))))),_tmp20E))));}
# 109
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 113
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
struct Cyc_Absyn_Tqual _tmp210;return(_tmp210.print_const=x.print_const  || y.print_const,((_tmp210.q_volatile=
x.q_volatile  || y.q_volatile,((_tmp210.q_restrict=
x.q_restrict  || y.q_restrict,((_tmp210.real_const=
x.real_const  || y.real_const,((_tmp210.loc=
Cyc_Position_segment_join(x.loc,y.loc),_tmp210)))))))));}
# 121
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp211;return(_tmp211.print_const=0,((_tmp211.q_volatile=0,((_tmp211.q_restrict=0,((_tmp211.real_const=0,((_tmp211.loc=loc,_tmp211)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp212;return(_tmp212.print_const=1,((_tmp212.q_volatile=0,((_tmp212.q_restrict=0,((_tmp212.real_const=1,((_tmp212.loc=loc,_tmp212)))))))));}
# 124
struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val={Cyc_Absyn_EmptyAnnot};
# 128
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Absyn_one_b_raw={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
# 130
struct Cyc_Absyn_Exp Cyc_Absyn_exp_unsigned_one_v={0,(void*)& Cyc_Absyn_one_b_raw,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
struct Cyc_Absyn_Exp*Cyc_Absyn_exp_unsigned_one=& Cyc_Absyn_exp_unsigned_one_v;
static struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct Cyc_Absyn_one_bt={1,& Cyc_Absyn_exp_unsigned_one_v};
void*Cyc_Absyn_bounds_one=(void*)& Cyc_Absyn_one_bt;
# 135
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo udi){
union Cyc_Absyn_DatatypeInfoU _tmp213;return((_tmp213.UnknownDatatype).val=udi,(((_tmp213.UnknownDatatype).tag=1,_tmp213)));}
# 138
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
union Cyc_Absyn_DatatypeInfoU _tmp214;return((_tmp214.KnownDatatype).val=d,(((_tmp214.KnownDatatype).tag=2,_tmp214)));}
# 141
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
union Cyc_Absyn_DatatypeFieldInfoU _tmp215;return((_tmp215.UnknownDatatypefield).val=s,(((_tmp215.UnknownDatatypefield).tag=1,_tmp215)));}
# 144
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
struct _tuple1 _tmp218;union Cyc_Absyn_DatatypeFieldInfoU _tmp217;return((_tmp217.KnownDatatypefield).val=((_tmp218.f1=dd,((_tmp218.f2=df,_tmp218)))),(((_tmp217.KnownDatatypefield).tag=2,_tmp217)));}
# 147
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
struct _tuple2 _tmp21B;union Cyc_Absyn_AggrInfoU _tmp21A;return((_tmp21A.UnknownAggr).val=((_tmp21B.f1=ak,((_tmp21B.f2=n,((_tmp21B.f3=tagged,_tmp21B)))))),(((_tmp21A.UnknownAggr).tag=1,_tmp21A)));}
# 150
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
union Cyc_Absyn_AggrInfoU _tmp21C;return((_tmp21C.KnownAggr).val=ad,(((_tmp21C.KnownAggr).tag=2,_tmp21C)));}
# 154
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*_tmp21D;return(_tmp21D=_cycalloc(sizeof(*_tmp21D)),(((_tmp21D->Eq_constr).val=x,(((_tmp21D->Eq_constr).tag=1,_tmp21D)))));}
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){union Cyc_Absyn_Constraint*_tmp21E;return(_tmp21E=_cycalloc_atomic(sizeof(*_tmp21E)),(((_tmp21E->No_constr).val=0,(((_tmp21E->No_constr).tag=3,_tmp21E)))));}
# 161
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0};
# 163
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 170
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp25=x;union Cyc_Absyn_Constraint*_tmp26;_LL1F: if((_tmp25->No_constr).tag != 3)goto _LL21;_LL20:
 goto _LL22;_LL21: if((_tmp25->Eq_constr).tag != 1)goto _LL23;_LL22:
 return x;_LL23: if((_tmp25->Forward_constr).tag != 2)goto _LL1E;_tmp26=(union Cyc_Absyn_Constraint*)(_tmp25->Forward_constr).val;_LL24: {
# 175
union Cyc_Absyn_Constraint*_tmp27=Cyc_Absyn_compress_conref(_tmp26);
{struct _union_Constraint_Forward_constr*_tmp21F;(_tmp21F=& x->Forward_constr,((_tmp21F->tag=2,_tmp21F->val=_tmp27)));}
return _tmp27;}_LL1E:;}
# 181
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_stmttmp2=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint*_tmp29=_stmttmp2;void*_tmp2A;_LL26: if((_tmp29->Eq_constr).tag != 1)goto _LL28;_tmp2A=(void*)(_tmp29->Eq_constr).val;_LL27:
 return _tmp2A;_LL28:;_LL29: {
const char*_tmp222;void*_tmp221;(_tmp221=0,Cyc_Tcutil_impos(((_tmp222="conref_val",_tag_dyneither(_tmp222,sizeof(char),11))),_tag_dyneither(_tmp221,sizeof(void*),0)));}_LL25:;}
# 188
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp2D=x;void*_tmp2E;_LL2B: if((_tmp2D->Eq_constr).tag != 1)goto _LL2D;_tmp2E=(void*)(_tmp2D->Eq_constr).val;_LL2C:
 return _tmp2E;_LL2D: if((_tmp2D->No_constr).tag != 3)goto _LL2F;_LL2E:
 return y;_LL2F:;_LL30:
 return Cyc_Absyn_conref_def(y,Cyc_Absyn_compress_conref(x));_LL2A:;}
# 196
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
x=Cyc_Absyn_compress_conref(x);{
union Cyc_Absyn_Constraint*_tmp2F=x;void*_tmp30;_LL32: if((_tmp2F->Eq_constr).tag != 1)goto _LL34;_tmp30=(void*)(_tmp2F->Eq_constr).val;_LL33:
 return _tmp30;_LL34:;_LL35:
{struct _union_Constraint_Eq_constr*_tmp223;(_tmp223=& x->Eq_constr,((_tmp223->tag=1,_tmp223->val=y)));}return y;_LL31:;};}
# 204
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp32=k;void**_tmp37;void**_tmp39;_LL37: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp33=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp32;if(_tmp33->tag != 0)goto _LL39;}_LL38:
 goto _LL3A;_LL39: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32;if(_tmp34->tag != 1)goto _LL3B;else{if(_tmp34->f1 != 0)goto _LL3B;}}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp35=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32;if(_tmp35->tag != 2)goto _LL3D;else{if(_tmp35->f1 != 0)goto _LL3D;}}_LL3C:
 return k;_LL3D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp36=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp32;if(_tmp36->tag != 1)goto _LL3F;else{if(_tmp36->f1 == 0)goto _LL3F;_tmp37=(void**)&(_tmp36->f1)->v;}}_LL3E:
 _tmp39=_tmp37;goto _LL40;_LL3F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp38=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp32;if(_tmp38->tag != 2)goto _LL36;else{if(_tmp38->f1 == 0)goto _LL36;_tmp39=(void**)&(_tmp38->f1)->v;}}_LL40:
# 211
*_tmp39=Cyc_Absyn_compress_kb(*_tmp39);
return*_tmp39;_LL36:;}
# 216
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_stmttmp3=Cyc_Absyn_compress_kb(kb);void*_tmp3A=_stmttmp3;struct Cyc_Absyn_Kind*_tmp3C;struct Cyc_Core_Opt**_tmp3E;struct Cyc_Core_Opt**_tmp40;struct Cyc_Absyn_Kind*_tmp41;_LL42: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3A;if(_tmp3B->tag != 0)goto _LL44;else{_tmp3C=_tmp3B->f1;}}_LL43:
 return _tmp3C;_LL44: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp3A;if(_tmp3D->tag != 1)goto _LL46;else{_tmp3E=(struct Cyc_Core_Opt**)& _tmp3D->f1;}}_LL45:
 _tmp40=_tmp3E;_tmp41=& Cyc_Tcutil_bk;goto _LL47;_LL46: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3A;if(_tmp3F->tag != 2)goto _LL41;else{_tmp40=(struct Cyc_Core_Opt**)& _tmp3F->f1;_tmp41=_tmp3F->f2;}}_LL47:
# 221
{struct Cyc_Core_Opt*_tmp224;*_tmp40=((_tmp224=_cycalloc(sizeof(*_tmp224)),((_tmp224->v=Cyc_Tcutil_kind_to_bound(_tmp41),_tmp224))));}
return _tmp41;_LL41:;}
# 227
struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val={20};
struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val={21};
struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val={22};
struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val={0};
# 233
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_char_tt={6,Cyc_Absyn_None,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uchar_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ushort_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_uint_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_ulonglong_tt={6,Cyc_Absyn_Unsigned,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_char_typ=(void*)& Cyc_Absyn_char_tt;
void*Cyc_Absyn_uchar_typ=(void*)& Cyc_Absyn_uchar_tt;
void*Cyc_Absyn_ushort_typ=(void*)& Cyc_Absyn_ushort_tt;
void*Cyc_Absyn_uint_typ=(void*)& Cyc_Absyn_uint_tt;
void*Cyc_Absyn_ulong_typ=(void*)& Cyc_Absyn_ulong_tt;
void*Cyc_Absyn_ulonglong_typ=(void*)& Cyc_Absyn_ulonglong_tt;
# 246
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_schar_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Char_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sshort_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_sint_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_slonglong_tt={6,Cyc_Absyn_Signed,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_schar_typ=(void*)& Cyc_Absyn_schar_tt;
void*Cyc_Absyn_sshort_typ=(void*)& Cyc_Absyn_sshort_tt;
void*Cyc_Absyn_sint_typ=(void*)& Cyc_Absyn_sint_tt;
void*Cyc_Absyn_slong_typ=(void*)& Cyc_Absyn_slong_tt;
void*Cyc_Absyn_slonglong_typ=(void*)& Cyc_Absyn_slonglong_tt;
# 257
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nshort_tt={6,Cyc_Absyn_None,Cyc_Absyn_Short_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nint_tt={6,Cyc_Absyn_None,Cyc_Absyn_Int_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlong_tt={6,Cyc_Absyn_None,Cyc_Absyn_Long_sz};
static struct Cyc_Absyn_IntType_Absyn_Type_struct Cyc_Absyn_nlonglong_tt={6,Cyc_Absyn_None,Cyc_Absyn_LongLong_sz};
void*Cyc_Absyn_nshort_typ=(void*)& Cyc_Absyn_nshort_tt;
void*Cyc_Absyn_nint_typ=(void*)& Cyc_Absyn_nint_tt;
void*Cyc_Absyn_nlong_typ=(void*)& Cyc_Absyn_nlong_tt;
void*Cyc_Absyn_nlonglong_typ=(void*)& Cyc_Absyn_nlonglong_tt;
# 266
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign sn,enum Cyc_Absyn_Size_of sz){
switch(sn){case Cyc_Absyn_Signed: _LL48:
# 269
 switch(sz){case Cyc_Absyn_Char_sz: _LL4A:
 return Cyc_Absyn_schar_typ;case Cyc_Absyn_Short_sz: _LL4B:
 return Cyc_Absyn_sshort_typ;case Cyc_Absyn_Int_sz: _LL4C:
 return Cyc_Absyn_sint_typ;case Cyc_Absyn_Long_sz: _LL4D:
 return Cyc_Absyn_slong_typ;case Cyc_Absyn_LongLong_sz: _LL4E:
 return Cyc_Absyn_slonglong_typ;}case Cyc_Absyn_Unsigned: _LL49:
# 277
 switch(sz){case Cyc_Absyn_Char_sz: _LL51:
 return Cyc_Absyn_uchar_typ;case Cyc_Absyn_Short_sz: _LL52:
 return Cyc_Absyn_ushort_typ;case Cyc_Absyn_Int_sz: _LL53:
 return Cyc_Absyn_uint_typ;case Cyc_Absyn_Long_sz: _LL54:
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_LongLong_sz: _LL55:
 return Cyc_Absyn_ulonglong_typ;}case Cyc_Absyn_None: _LL50:
# 285
 switch(sz){case Cyc_Absyn_Char_sz: _LL58:
 return Cyc_Absyn_char_typ;case Cyc_Absyn_Short_sz: _LL59:
 return Cyc_Absyn_nshort_typ;case Cyc_Absyn_Int_sz: _LL5A:
 return Cyc_Absyn_nint_typ;case Cyc_Absyn_Long_sz: _LL5B:
 return Cyc_Absyn_nlong_typ;case Cyc_Absyn_LongLong_sz: _LL5C:
 return Cyc_Absyn_nlonglong_typ;}}}
# 295
void*Cyc_Absyn_float_typ(int i){
static struct Cyc_Absyn_FloatType_Absyn_Type_struct fl={7,0};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct db={7,1};
static struct Cyc_Absyn_FloatType_Absyn_Type_struct ldb={7,2};
if(i == 0)return(void*)& fl;
if(i == 1)return(void*)& db;
return(void*)& ldb;}
# 306
extern int Wchar_t_unsigned;
extern int Sizeof_wchar_t;
# 309
void*Cyc_Absyn_wchar_typ(){
switch(Sizeof_wchar_t){case 1: _LL5E:
# 320 "absyn.cyc"
 return Wchar_t_unsigned?Cyc_Absyn_uchar_typ: Cyc_Absyn_schar_typ;case 2: _LL5F:
 return Wchar_t_unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;default: _LL60:
# 324
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}}static char _tmp59[4]="exn";
# 328
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp59,_tmp59,_tmp59 + 4};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5A[15]="Null_Exception";
# 340 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5A,_tmp5A,_tmp5A + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp5B[13]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp5B,_tmp5B,_tmp5B + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp5C[16]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp5C,_tmp5C,_tmp5C + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp5D[10]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp5D,_tmp5D,_tmp5D + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
# 346
static struct Cyc_List_List Cyc_Absyn_exn_l0={(void*)& Cyc_Absyn_Null_Exception_tuf_v,0};
static struct Cyc_List_List Cyc_Absyn_exn_l1={(void*)& Cyc_Absyn_Array_bounds_tuf_v,& Cyc_Absyn_exn_l0};
static struct Cyc_List_List Cyc_Absyn_exn_l2={(void*)& Cyc_Absyn_Match_Exception_tuf_v,& Cyc_Absyn_exn_l1};
static struct Cyc_List_List Cyc_Absyn_exn_l3={(void*)& Cyc_Absyn_Bad_alloc_tuf_v,& Cyc_Absyn_exn_l2};
# 351
static struct Cyc_Core_Opt Cyc_Absyn_exn_ol={(void*)& Cyc_Absyn_exn_l3};
static struct Cyc_Absyn_Datatypedecl Cyc_Absyn_exn_tud_v={Cyc_Absyn_Extern,& Cyc_Absyn_exn_name_v,0,& Cyc_Absyn_exn_ol,1};
# 354
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud=& Cyc_Absyn_exn_tud_v;
# 357
static struct Cyc_Absyn_DatatypeType_Absyn_Type_struct Cyc_Absyn_exn_typ_tt={3,{{.KnownDatatype={2,& Cyc_Absyn_exn_tud}},0}};
# 359
void*Cyc_Absyn_exn_typ(){
static struct Cyc_Core_Opt*exn_type_val=0;
if(exn_type_val == 0){
struct Cyc_Core_Opt*_tmp225;exn_type_val=((_tmp225=_cycalloc(sizeof(*_tmp225)),((_tmp225->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp225))));}
# 365
return(void*)exn_type_val->v;}static char _tmp60[9]="PrintArg";
# 369
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp60,_tmp60,_tmp60 + 9};static char _tmp61[9]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp61,_tmp61,_tmp61 + 9};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 373
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 375
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp62[14]="unique_region";
# 383
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp62,_tmp62,_tmp62 + 14};static char _tmp63[5]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp63,_tmp63,_tmp63 + 5};
static struct Cyc_List_List Cyc_Absyn_uniquergn_nmspace={(void*)& Cyc_Absyn_Core_str,0};
# 387
static struct _tuple0 Cyc_Absyn_uniquergn_qvar_p={{.Abs_n={2,& Cyc_Absyn_uniquergn_nmspace}},& Cyc_Absyn_uniquergn_str};
# 389
struct _tuple0*Cyc_Absyn_uniquergn_qvar=& Cyc_Absyn_uniquergn_qvar_p;
# 391
static struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct Cyc_Absyn_uniquergn_typ={15,(void*)& Cyc_Absyn_UniqueRgn_val};
# 394
static struct Cyc_Absyn_Vardecl Cyc_Absyn_uniquergn_var_d={Cyc_Absyn_Extern,& Cyc_Absyn_uniquergn_qvar_p,{0,0,0,0,0},(void*)& Cyc_Absyn_uniquergn_typ,0,0,0,0};
# 397
static struct Cyc_Absyn_Global_b_Absyn_Binding_struct Cyc_Absyn_uniquergn_bind_p={1,& Cyc_Absyn_uniquergn_var_d};
# 399
static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Absyn_uniquergn_exp_r={1,& Cyc_Absyn_uniquergn_qvar_p,(void*)& Cyc_Absyn_uniquergn_bind_p};
# 402
static struct Cyc_Absyn_Exp Cyc_Absyn_uniquergn_exp_p={(void*)& Cyc_Absyn_uniquergn_typ,(void*)& Cyc_Absyn_uniquergn_exp_r,0,(void*)& Cyc_Absyn_EmptyAnnot_val};
# 405
struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp=& Cyc_Absyn_uniquergn_exp_p;
# 408
static void**Cyc_Absyn_string_t_opt=0;
void*Cyc_Absyn_string_typ(void*rgn){
void*_stmttmp4=Cyc_Tcutil_compress(rgn);void*_tmp67=_stmttmp4;_LL63: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp68=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp67;if(_tmp68->tag != 20)goto _LL65;}_LL64:
# 412
 if(Cyc_Absyn_string_t_opt == 0){
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 415
void**_tmp226;Cyc_Absyn_string_t_opt=((_tmp226=_cycalloc(sizeof(*_tmp226)),((_tmp226[0]=t,_tmp226))));}
# 417
return*((void**)_check_null(Cyc_Absyn_string_t_opt));_LL65:;_LL66:
# 419
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL62:;}
# 422
static void**Cyc_Absyn_const_string_t_opt=0;
void*Cyc_Absyn_const_string_typ(void*rgn){
void*_stmttmp5=Cyc_Tcutil_compress(rgn);void*_tmp6A=_stmttmp5;_LL68: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6B=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6A;if(_tmp6B->tag != 20)goto _LL6A;}_LL69:
# 426
 if(Cyc_Absyn_const_string_t_opt == 0){
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 429
void**_tmp227;Cyc_Absyn_const_string_t_opt=((_tmp227=_cycalloc(sizeof(*_tmp227)),((_tmp227[0]=t,_tmp227))));}
# 431
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));_LL6A:;_LL6B:
# 433
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL67:;}
# 438
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 440
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp22D;struct Cyc_Absyn_PtrInfo _tmp22C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp22B;return(void*)((_tmp22B=_cycalloc(sizeof(*_tmp22B)),((_tmp22B[0]=((_tmp22D.tag=5,((_tmp22D.f1=((_tmp22C.elt_typ=t,((_tmp22C.elt_tq=tq,((_tmp22C.ptr_atts=(
((_tmp22C.ptr_atts).rgn=r,(((_tmp22C.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp22C.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp22C.ptr_atts).zero_term=zeroterm,(((_tmp22C.ptr_atts).ptrloc=0,_tmp22C.ptr_atts)))))))))),_tmp22C)))))),_tmp22D)))),_tmp22B))));}
# 446
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 448
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp233;struct Cyc_Absyn_PtrInfo _tmp232;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp231;return(void*)((_tmp231=_cycalloc(sizeof(*_tmp231)),((_tmp231[0]=((_tmp233.tag=5,((_tmp233.f1=((_tmp232.elt_typ=t,((_tmp232.elt_tq=tq,((_tmp232.ptr_atts=(
((_tmp232.ptr_atts).rgn=r,(((_tmp232.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp232.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp232.ptr_atts).zero_term=zeroterm,(((_tmp232.ptr_atts).ptrloc=0,_tmp232.ptr_atts)))))))))),_tmp232)))))),_tmp233)))),_tmp231))));}
# 455
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp239;struct Cyc_Absyn_PtrInfo _tmp238;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp237;return(void*)((_tmp237=_cycalloc(sizeof(*_tmp237)),((_tmp237[0]=((_tmp239.tag=5,((_tmp239.f1=((_tmp238.elt_typ=t,((_tmp238.elt_tq=tq,((_tmp238.ptr_atts=(
((_tmp238.ptr_atts).rgn=r,(((_tmp238.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp238.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp238.ptr_atts).zero_term=zeroterm,(((_tmp238.ptr_atts).ptrloc=0,_tmp238.ptr_atts)))))))))),_tmp238)))))),_tmp239)))),_tmp237))));}
# 462
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp23F;struct Cyc_Absyn_PtrInfo _tmp23E;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23D;return(void*)((_tmp23D=_cycalloc(sizeof(*_tmp23D)),((_tmp23D[0]=((_tmp23F.tag=5,((_tmp23F.f1=((_tmp23E.elt_typ=t,((_tmp23E.elt_tq=tq,((_tmp23E.ptr_atts=(
((_tmp23E.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp23E.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp23E.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp23E.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp23E.ptr_atts).ptrloc=0,_tmp23E.ptr_atts)))))))))),_tmp23E)))))),_tmp23F)))),_tmp23D))));}
# 470
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp245;struct Cyc_Absyn_PtrInfo _tmp244;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp243;return(void*)((_tmp243=_cycalloc(sizeof(*_tmp243)),((_tmp243[0]=((_tmp245.tag=5,((_tmp245.f1=((_tmp244.elt_typ=t,((_tmp244.elt_tq=tq,((_tmp244.ptr_atts=(
((_tmp244.ptr_atts).rgn=r,(((_tmp244.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp244.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp244.ptr_atts).zero_term=zeroterm,(((_tmp244.ptr_atts).ptrloc=0,_tmp244.ptr_atts)))))))))),_tmp244)))))),_tmp245)))),_tmp243))));}
# 477
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp24B;struct Cyc_Absyn_PtrInfo _tmp24A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp249;return(void*)((_tmp249=_cycalloc(sizeof(*_tmp249)),((_tmp249[0]=((_tmp24B.tag=5,((_tmp24B.f1=((_tmp24A.elt_typ=t,((_tmp24A.elt_tq=tq,((_tmp24A.ptr_atts=(
((_tmp24A.ptr_atts).rgn=r,(((_tmp24A.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp24A.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp24A.ptr_atts).zero_term=zeroterm,(((_tmp24A.ptr_atts).ptrloc=0,_tmp24A.ptr_atts)))))))))),_tmp24A)))))),_tmp24B)))),_tmp249))));}
# 485
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 487
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp251;struct Cyc_Absyn_ArrayInfo _tmp250;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp24F;return(void*)((_tmp24F=_cycalloc(sizeof(*_tmp24F)),((_tmp24F[0]=((_tmp251.tag=8,((_tmp251.f1=((_tmp250.elt_type=elt_type,((_tmp250.tq=tq,((_tmp250.num_elts=num_elts,((_tmp250.zero_term=zero_term,((_tmp250.zt_loc=ztloc,_tmp250)))))))))),_tmp251)))),_tmp24F))));}static char _tmp89[8]="__sFILE";
# 495
void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;
static struct _dyneither_ptr sf_str={_tmp89,_tmp89,_tmp89 + 8};
static struct _dyneither_ptr*sf=& sf_str;
# 500
if(file_t_opt == 0){
struct _tuple0*_tmp252;struct _tuple0*file_t_name=(_tmp252=_cycalloc(sizeof(*_tmp252)),((_tmp252->f1=Cyc_Absyn_Abs_n(0,0),((_tmp252->f2=sf,_tmp252)))));
struct Cyc_Absyn_Aggrdecl*_tmp253;struct Cyc_Absyn_Aggrdecl*sd=(_tmp253=_cycalloc(sizeof(*_tmp253)),((_tmp253->kind=Cyc_Absyn_StructA,((_tmp253->sc=Cyc_Absyn_Abstract,((_tmp253->name=file_t_name,((_tmp253->tvs=0,((_tmp253->impl=0,((_tmp253->attributes=0,_tmp253)))))))))))));
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp25D;struct Cyc_Absyn_Aggrdecl**_tmp25C;struct Cyc_Absyn_AggrInfo _tmp25B;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp25A;void*file_struct_typ=(void*)((_tmp25A=_cycalloc(sizeof(*_tmp25A)),((_tmp25A[0]=((_tmp25D.tag=11,((_tmp25D.f1=((_tmp25B.aggr_info=Cyc_Absyn_KnownAggr(((_tmp25C=_cycalloc(sizeof(*_tmp25C)),((_tmp25C[0]=sd,_tmp25C))))),((_tmp25B.targs=0,_tmp25B)))),_tmp25D)))),_tmp25A))));
void**_tmp25E;file_t_opt=((_tmp25E=_cycalloc(sizeof(*_tmp25E)),((_tmp25E[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp25E))));}
# 507
return*file_t_opt;}
# 510
void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;
# 513
if(void_star_t_opt == 0){
void**_tmp25F;void_star_t_opt=((_tmp25F=_cycalloc(sizeof(*_tmp25F)),((_tmp25F[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp25F))));}
return*void_star_t_opt;}
# 518
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 525
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp269;struct _tuple0*_tmp268;struct Cyc_Absyn_AggrInfo _tmp267;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp266;return(void*)((_tmp266=_cycalloc(sizeof(*_tmp266)),((_tmp266[0]=((_tmp269.tag=11,((_tmp269.f1=((_tmp267.aggr_info=Cyc_Absyn_UnknownAggr(k,((_tmp268=_cycalloc(sizeof(*_tmp268)),((_tmp268->f1=Cyc_Absyn_rel_ns_null,((_tmp268->f2=name,_tmp268)))))),0),((_tmp267.targs=0,_tmp267)))),_tmp269)))),_tmp266))));}
# 529
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 532
void*Cyc_Absyn_strctq(struct _tuple0*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp26F;struct Cyc_Absyn_AggrInfo _tmp26E;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp26D;return(void*)((_tmp26D=_cycalloc(sizeof(*_tmp26D)),((_tmp26D[0]=((_tmp26F.tag=11,((_tmp26F.f1=((_tmp26E.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),((_tmp26E.targs=0,_tmp26E)))),_tmp26F)))),_tmp26D))));}
# 535
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp275;struct Cyc_Absyn_AggrInfo _tmp274;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp273;return(void*)((_tmp273=_cycalloc(sizeof(*_tmp273)),((_tmp273[0]=((_tmp275.tag=11,((_tmp275.f1=((_tmp274.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((_tmp274.targs=0,_tmp274)))),_tmp275)))),_tmp273))));}
# 539
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct _tuple3 _tmp278;union Cyc_Absyn_Cnst _tmp277;return((_tmp277.Char_c).val=((_tmp278.f1=sn,((_tmp278.f2=c,_tmp278)))),(((_tmp277.Char_c).tag=2,_tmp277)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp279;return((_tmp279.Wchar_c).val=s,(((_tmp279.Wchar_c).tag=3,_tmp279)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp27C;union Cyc_Absyn_Cnst _tmp27B;return((_tmp27B.Short_c).val=((_tmp27C.f1=sn,((_tmp27C.f2=s,_tmp27C)))),(((_tmp27B.Short_c).tag=4,_tmp27B)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){struct _tuple5 _tmp27F;union Cyc_Absyn_Cnst _tmp27E;return((_tmp27E.Int_c).val=((_tmp27F.f1=sn,((_tmp27F.f2=i,_tmp27F)))),(((_tmp27E.Int_c).tag=5,_tmp27E)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct _tuple6 _tmp282;union Cyc_Absyn_Cnst _tmp281;return((_tmp281.LongLong_c).val=((_tmp282.f1=sn,((_tmp282.f2=l,_tmp282)))),(((_tmp281.LongLong_c).tag=6,_tmp281)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){struct _tuple7 _tmp285;union Cyc_Absyn_Cnst _tmp284;return((_tmp284.Float_c).val=((_tmp285.f1=s,((_tmp285.f2=i,_tmp285)))),(((_tmp284.Float_c).tag=7,_tmp284)));}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp286;return((_tmp286.String_c).val=s,(((_tmp286.String_c).tag=8,_tmp286)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp287;return((_tmp287.Wstring_c).val=s,(((_tmp287.Wstring_c).tag=9,_tmp287)));}
# 550
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
struct Cyc_Absyn_Exp*_tmp288;return(_tmp288=_cycalloc(sizeof(*_tmp288)),((_tmp288->topt=0,((_tmp288->r=r,((_tmp288->loc=loc,((_tmp288->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp288)))))))));}
# 553
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp28B;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp28A;return Cyc_Absyn_new_exp((void*)((_tmp28A=_cycalloc(sizeof(*_tmp28A)),((_tmp28A[0]=((_tmp28B.tag=15,((_tmp28B.f1=rgn_handle,((_tmp28B.f2=e,_tmp28B)))))),_tmp28A)))),loc);}
# 556
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*_tmp28C;return(_tmp28C=_cycalloc(sizeof(*_tmp28C)),((_tmp28C[0]=*e,_tmp28C)));}
# 559
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp28F;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp28E;return Cyc_Absyn_new_exp((void*)((_tmp28E=_cycalloc(sizeof(*_tmp28E)),((_tmp28E[0]=((_tmp28F.tag=0,((_tmp28F.f1=c,_tmp28F)))),_tmp28E)))),loc);}
# 562
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct null_const={0,{.Null_c={1,0}}};
return Cyc_Absyn_new_exp((void*)& null_const,loc);}
# 566
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign s,int i,unsigned int seg){return Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(s,i),seg);}
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct szero={0,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct sone={0,{.Int_c={5,{Cyc_Absyn_Signed,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& szero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& sone,loc);}
return Cyc_Absyn_int_exp(Cyc_Absyn_Signed,i,loc);}
# 574
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int i,unsigned int loc){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uzero={0,{.Int_c={5,{Cyc_Absyn_Unsigned,0}}}};
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct uone={0,{.Int_c={5,{Cyc_Absyn_Unsigned,1}}}};
if(i == 0)return Cyc_Absyn_new_exp((void*)& uzero,loc);else{
if(i == 1)return Cyc_Absyn_new_exp((void*)& uone,loc);else{
return Cyc_Absyn_int_exp(Cyc_Absyn_Unsigned,(int)i,loc);}}}
# 581
struct Cyc_Absyn_Exp*Cyc_Absyn_bool_exp(int b,unsigned int loc){return Cyc_Absyn_signed_int_exp(b?1: 0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int loc){return Cyc_Absyn_bool_exp(1,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int loc){return Cyc_Absyn_bool_exp(0,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int loc){
return Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(Cyc_Absyn_None,c),loc);}
# 587
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int loc){
return Cyc_Absyn_const_exp(Cyc_Absyn_Wchar_c(s),loc);}
# 590
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(f,i),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_String_c(s),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int loc){return Cyc_Absyn_const_exp(Cyc_Absyn_Wstring_c(s),loc);}
# 594
struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Absyn_Unresolved_b_val={0};
# 597
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*q,unsigned int loc){
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp292;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp291;return Cyc_Absyn_new_exp((void*)((_tmp291=_cycalloc(sizeof(*_tmp291)),((_tmp291[0]=((_tmp292.tag=1,((_tmp292.f1=q,((_tmp292.f2=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp292)))))),_tmp291)))),loc);}
# 600
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,unsigned int loc){
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp295;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp294;return Cyc_Absyn_new_exp((void*)((_tmp294=_cycalloc(sizeof(*_tmp294)),((_tmp294[0]=((_tmp295.tag=1,((_tmp295.f1=q,((_tmp295.f2=b,_tmp295)))))),_tmp294)))),loc);}
# 604
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp298;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp297;return Cyc_Absyn_new_exp((void*)((_tmp297=_cycalloc(sizeof(*_tmp297)),((_tmp297[0]=((_tmp298.tag=1,((_tmp298.f1=q,((_tmp298.f2=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp298)))))),_tmp297)))),loc);}
# 607
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp29B;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp29A;return Cyc_Absyn_new_exp((void*)((_tmp29A=_cycalloc(sizeof(*_tmp29A)),((_tmp29A[0]=((_tmp29B.tag=2,((_tmp29B.f1=p,((_tmp29B.f2=es,_tmp29B)))))),_tmp29A)))),loc);}
# 610
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_List_List*_tmp29C;return Cyc_Absyn_primop_exp(p,((_tmp29C=_cycalloc(sizeof(*_tmp29C)),((_tmp29C->hd=e,((_tmp29C->tl=0,_tmp29C)))))),loc);}
# 613
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp29E;return Cyc_Absyn_primop_exp(p,((_tmp29E=_cycalloc(sizeof(*_tmp29E)),((_tmp29E->hd=e1,((_tmp29E->tl=((_tmp29F=_cycalloc(sizeof(*_tmp29F)),((_tmp29F->hd=e2,((_tmp29F->tl=0,_tmp29F)))))),_tmp29E)))))),loc);}
# 616
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmp2A2;struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2A1;return Cyc_Absyn_new_exp((void*)((_tmp2A1=_cycalloc(sizeof(*_tmp2A1)),((_tmp2A1[0]=((_tmp2A2.tag=34,((_tmp2A2.f1=e1,((_tmp2A2.f2=e2,_tmp2A2)))))),_tmp2A1)))),loc);}
# 619
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lt,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Gte,e1,e2,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){return Cyc_Absyn_prim2_exp(Cyc_Absyn_Lte,e1,e2,loc);}
# 629
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp2A5;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp2A4;return Cyc_Absyn_new_exp((void*)((_tmp2A4=_cycalloc(sizeof(*_tmp2A4)),((_tmp2A4[0]=((_tmp2A5.tag=3,((_tmp2A5.f1=e1,((_tmp2A5.f2=popt,((_tmp2A5.f3=e2,_tmp2A5)))))))),_tmp2A4)))),loc);}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp2A8;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2A7;return Cyc_Absyn_new_exp((void*)((_tmp2A7=_cycalloc(sizeof(*_tmp2A7)),((_tmp2A7[0]=((_tmp2A8.tag=4,((_tmp2A8.f1=e,((_tmp2A8.f2=i,_tmp2A8)))))),_tmp2A7)))),loc);}
# 638
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp2AB;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp2AA;return Cyc_Absyn_new_exp((void*)((_tmp2AA=_cycalloc(sizeof(*_tmp2AA)),((_tmp2AA[0]=((_tmp2AB.tag=5,((_tmp2AB.f1=e1,((_tmp2AB.f2=e2,((_tmp2AB.f3=e3,_tmp2AB)))))))),_tmp2AA)))),loc);}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp2AE;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp2AD;return Cyc_Absyn_new_exp((void*)((_tmp2AD=_cycalloc(sizeof(*_tmp2AD)),((_tmp2AD[0]=((_tmp2AE.tag=6,((_tmp2AE.f1=e1,((_tmp2AE.f2=e2,_tmp2AE)))))),_tmp2AD)))),loc);}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp2B1;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp2B0;return Cyc_Absyn_new_exp((void*)((_tmp2B0=_cycalloc(sizeof(*_tmp2B0)),((_tmp2B0[0]=((_tmp2B1.tag=7,((_tmp2B1.f1=e1,((_tmp2B1.f2=e2,_tmp2B1)))))),_tmp2B0)))),loc);}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp2B4;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp2B3;return Cyc_Absyn_new_exp((void*)((_tmp2B3=_cycalloc(sizeof(*_tmp2B3)),((_tmp2B3[0]=((_tmp2B4.tag=8,((_tmp2B4.f1=e1,((_tmp2B4.f2=e2,_tmp2B4)))))),_tmp2B3)))),loc);}
# 654
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp2B7;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp2B6;return Cyc_Absyn_new_exp((void*)((_tmp2B6=_cycalloc(sizeof(*_tmp2B6)),((_tmp2B6[0]=((_tmp2B7.tag=9,((_tmp2B7.f1=e,((_tmp2B7.f2=es,((_tmp2B7.f3=0,((_tmp2B7.f4=0,_tmp2B7)))))))))),_tmp2B6)))),loc);}
# 657
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp2BA;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp2B9;return Cyc_Absyn_new_exp((void*)((_tmp2B9=_cycalloc(sizeof(*_tmp2B9)),((_tmp2B9[0]=((_tmp2BA.tag=9,((_tmp2BA.f1=e,((_tmp2BA.f2=es,((_tmp2BA.f3=0,((_tmp2BA.f4=1,_tmp2BA)))))))))),_tmp2B9)))),loc);}
# 660
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp2BD;struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp2BC;return Cyc_Absyn_new_exp((void*)((_tmp2BC=_cycalloc(sizeof(*_tmp2BC)),((_tmp2BC[0]=((_tmp2BD.tag=11,((_tmp2BD.f1=e,_tmp2BD)))),_tmp2BC)))),loc);}
# 663
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp2C0;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp2BF;return Cyc_Absyn_new_exp((void*)((_tmp2BF=_cycalloc(sizeof(*_tmp2BF)),((_tmp2BF[0]=((_tmp2C0.tag=12,((_tmp2C0.f1=e,((_tmp2C0.f2=ts,_tmp2C0)))))),_tmp2BF)))),loc);}
# 666
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp2C3;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2C2;return Cyc_Absyn_new_exp((void*)((_tmp2C2=_cycalloc(sizeof(*_tmp2C2)),((_tmp2C2[0]=((_tmp2C3.tag=13,((_tmp2C3.f1=t,((_tmp2C3.f2=e,((_tmp2C3.f3=user_cast,((_tmp2C3.f4=c,_tmp2C3)))))))))),_tmp2C2)))),loc);}
# 669
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp2C6;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp2C5;return Cyc_Absyn_new_exp((void*)((_tmp2C5=_cycalloc(sizeof(*_tmp2C5)),((_tmp2C5[0]=((_tmp2C6.tag=10,((_tmp2C6.f1=e,_tmp2C6)))),_tmp2C5)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmp2C9;struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp2C8;return Cyc_Absyn_new_exp((void*)((_tmp2C8=_cycalloc(sizeof(*_tmp2C8)),((_tmp2C8[0]=((_tmp2C9.tag=14,((_tmp2C9.f1=e,_tmp2C9)))),_tmp2C8)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp2CC;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp2CB;return Cyc_Absyn_new_exp((void*)((_tmp2CB=_cycalloc(sizeof(*_tmp2CB)),((_tmp2CB[0]=((_tmp2CC.tag=16,((_tmp2CC.f1=t,_tmp2CC)))),_tmp2CB)))),loc);}
# 674
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp2CF;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp2CE;return Cyc_Absyn_new_exp((void*)((_tmp2CE=_cycalloc(sizeof(*_tmp2CE)),((_tmp2CE[0]=((_tmp2CF.tag=17,((_tmp2CF.f1=e,_tmp2CF)))),_tmp2CE)))),loc);}
# 677
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp2D2;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2D1;return Cyc_Absyn_new_exp((void*)((_tmp2D1=_cycalloc(sizeof(*_tmp2D1)),((_tmp2D1[0]=((_tmp2D2.tag=18,((_tmp2D2.f1=t,((_tmp2D2.f2=ofs,_tmp2D2)))))),_tmp2D1)))),loc);}
# 680
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp2D5;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2D4;return Cyc_Absyn_new_exp((void*)((_tmp2D4=_cycalloc(sizeof(*_tmp2D4)),((_tmp2D4[0]=((_tmp2D5.tag=19,((_tmp2D5.f1=e,_tmp2D5)))),_tmp2D4)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp2D8;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2D7;return Cyc_Absyn_new_exp((void*)((_tmp2D7=_cycalloc(sizeof(*_tmp2D7)),((_tmp2D7[0]=((_tmp2D8.tag=20,((_tmp2D8.f1=e,((_tmp2D8.f2=n,((_tmp2D8.f3=0,((_tmp2D8.f4=0,_tmp2D8)))))))))),_tmp2D7)))),loc);}
# 684
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp2DB;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2DA;return Cyc_Absyn_new_exp((void*)((_tmp2DA=_cycalloc(sizeof(*_tmp2DA)),((_tmp2DA[0]=((_tmp2DB.tag=21,((_tmp2DB.f1=e,((_tmp2DB.f2=n,((_tmp2DB.f3=0,((_tmp2DB.f4=0,_tmp2DB)))))))))),_tmp2DA)))),loc);}
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp2DE;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2DD;return Cyc_Absyn_new_exp((void*)((_tmp2DD=_cycalloc(sizeof(*_tmp2DD)),((_tmp2DD[0]=((_tmp2DE.tag=22,((_tmp2DE.f1=e1,((_tmp2DE.f2=e2,_tmp2DE)))))),_tmp2DD)))),loc);}
# 690
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmp2E1;struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2E0;return Cyc_Absyn_new_exp((void*)((_tmp2E0=_cycalloc(sizeof(*_tmp2E0)),((_tmp2E0[0]=((_tmp2E1.tag=23,((_tmp2E1.f1=es,_tmp2E1)))),_tmp2E0)))),loc);}
# 693
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp2E4;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2E3;return Cyc_Absyn_new_exp((void*)((_tmp2E3=_cycalloc(sizeof(*_tmp2E3)),((_tmp2E3[0]=((_tmp2E4.tag=36,((_tmp2E4.f1=s,_tmp2E4)))),_tmp2E3)))),loc);}
# 695
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 699
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp2E7;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2E6;return Cyc_Absyn_new_exp((void*)((_tmp2E6=_cycalloc(sizeof(*_tmp2E6)),((_tmp2E6[0]=((_tmp2E7.tag=38,((_tmp2E7.f1=t,_tmp2E7)))),_tmp2E6)))),loc);}
# 702
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmp2EA;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp2E9;return Cyc_Absyn_new_exp((void*)((_tmp2E9=_cycalloc(sizeof(*_tmp2E9)),((_tmp2E9[0]=((_tmp2EA.tag=39,((_tmp2EA.f1=volatile_kw,((_tmp2EA.f2=body,_tmp2EA)))))),_tmp2E9)))),loc);}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 706
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 708
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
struct _tuple13*_tmp2ED;struct Cyc_List_List*_tmp2EC;dles=((_tmp2EC=_cycalloc(sizeof(*_tmp2EC)),((_tmp2EC->hd=((_tmp2ED=_cycalloc(sizeof(*_tmp2ED)),((_tmp2ED->f1=0,((_tmp2ED->f2=(struct Cyc_Absyn_Exp*)es->hd,_tmp2ED)))))),((_tmp2EC->tl=dles,_tmp2EC))))));}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2F0;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2EF;return Cyc_Absyn_new_exp((void*)((_tmp2EF=_cycalloc(sizeof(*_tmp2EF)),((_tmp2EF[0]=((_tmp2F0.tag=25,((_tmp2F0.f1=dles,_tmp2F0)))),_tmp2EF)))),loc);};}
# 714
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 717
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp2F3;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2F2;return Cyc_Absyn_new_exp((void*)((_tmp2F2=_cycalloc(sizeof(*_tmp2F2)),((_tmp2F2[0]=((_tmp2F3.tag=35,((_tmp2F3.f1=n,((_tmp2F3.f2=dles,_tmp2F3)))))),_tmp2F2)))),loc);}
# 720
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
struct Cyc_Absyn_Stmt*_tmp2F4;return(_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4->r=s,((_tmp2F4->loc=loc,((_tmp2F4->non_local_preds=0,((_tmp2F4->try_depth=0,((_tmp2F4->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp2F4)))))))))));}
# 724
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp2F7;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp2F6;return Cyc_Absyn_new_stmt((void*)((_tmp2F6=_cycalloc(sizeof(*_tmp2F6)),((_tmp2F6[0]=((_tmp2F7.tag=1,((_tmp2F7.f1=e,_tmp2F7)))),_tmp2F6)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}
# 732
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmp2FA;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp2F9;return Cyc_Absyn_new_stmt((void*)((_tmp2F9=_cycalloc(sizeof(*_tmp2F9)),((_tmp2F9[0]=((_tmp2FA.tag=3,((_tmp2FA.f1=e,_tmp2FA)))),_tmp2F9)))),loc);}
# 735
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp2FD;struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp2FC;return Cyc_Absyn_new_stmt((void*)((_tmp2FC=_cycalloc(sizeof(*_tmp2FC)),((_tmp2FC[0]=((_tmp2FD.tag=4,((_tmp2FD.f1=e,((_tmp2FD.f2=s1,((_tmp2FD.f3=s2,_tmp2FD)))))))),_tmp2FC)))),loc);}
# 738
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp303;struct _tuple9 _tmp302;struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp301;return Cyc_Absyn_new_stmt((void*)((_tmp301=_cycalloc(sizeof(*_tmp301)),((_tmp301[0]=((_tmp303.tag=5,((_tmp303.f1=((_tmp302.f1=e,((_tmp302.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp302)))),((_tmp303.f2=s,_tmp303)))))),_tmp301)))),loc);}
# 741
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct _tmp306;struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp305;return Cyc_Absyn_new_stmt((void*)((_tmp305=_cycalloc(sizeof(*_tmp305)),((_tmp305[0]=((_tmp306.tag=6,((_tmp306.f1=0,_tmp306)))),_tmp305)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct _tmp309;struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp308;return Cyc_Absyn_new_stmt((void*)((_tmp308=_cycalloc(sizeof(*_tmp308)),((_tmp308[0]=((_tmp309.tag=7,((_tmp309.f1=0,_tmp309)))),_tmp308)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp312;struct _tuple9 _tmp311;struct _tuple9 _tmp310;struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp30F;return Cyc_Absyn_new_stmt((void*)((_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=((_tmp312.tag=9,((_tmp312.f1=e1,((_tmp312.f2=((_tmp311.f1=e2,((_tmp311.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp311)))),((_tmp312.f3=(
(_tmp310.f1=e3,((_tmp310.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp310)))),((_tmp312.f4=s,_tmp312)))))))))),_tmp30F)))),loc);}
# 748
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp315;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp314;return Cyc_Absyn_new_stmt((void*)((_tmp314=_cycalloc(sizeof(*_tmp314)),((_tmp314[0]=((_tmp315.tag=10,((_tmp315.f1=e,((_tmp315.f2=scs,_tmp315)))))),_tmp314)))),loc);}struct _tuple14{void*f1;void*f2;};
# 751
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp316;struct _tuple14 _stmttmp6=(_tmp316.f1=s1->r,((_tmp316.f2=s2->r,_tmp316)));struct _tuple14 _tmp107=_stmttmp6;_LL6D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp108=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp107.f1;if(_tmp108->tag != 0)goto _LL6F;}_LL6E:
 return s2;_LL6F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp109=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp107.f2;if(_tmp109->tag != 0)goto _LL71;}_LL70:
 return s1;_LL71:;_LL72: {
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp319;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp318;return Cyc_Absyn_new_stmt((void*)((_tmp318=_cycalloc(sizeof(*_tmp318)),((_tmp318[0]=((_tmp319.tag=2,((_tmp319.f1=s1,((_tmp319.f2=s2,_tmp319)))))),_tmp318)))),loc);}_LL6C:;}
# 758
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp31C;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp31B;return Cyc_Absyn_new_stmt((void*)((_tmp31B=_cycalloc(sizeof(*_tmp31B)),((_tmp31B[0]=((_tmp31C.tag=11,((_tmp31C.f1=el,((_tmp31C.f2=0,_tmp31C)))))),_tmp31B)))),loc);}
# 761
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp31F;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp31E;return Cyc_Absyn_new_stmt((void*)((_tmp31E=_cycalloc(sizeof(*_tmp31E)),((_tmp31E[0]=((_tmp31F.tag=12,((_tmp31F.f1=d,((_tmp31F.f2=s,_tmp31F)))))),_tmp31E)))),loc);}
# 764
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp322;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp321;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((void*)((_tmp321=_cycalloc(sizeof(*_tmp321)),((_tmp321[0]=((_tmp322.tag=0,((_tmp322.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp322)))),_tmp321)))),loc);
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp325;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp324;return Cyc_Absyn_new_stmt((void*)((_tmp324=_cycalloc(sizeof(*_tmp324)),((_tmp324[0]=((_tmp325.tag=12,((_tmp325.f1=d,((_tmp325.f2=s,_tmp325)))))),_tmp324)))),loc);}
# 768
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp328;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp327;return Cyc_Absyn_new_stmt((void*)((_tmp327=_cycalloc(sizeof(*_tmp327)),((_tmp327[0]=((_tmp328.tag=13,((_tmp328.f1=v,((_tmp328.f2=s,_tmp328)))))),_tmp327)))),loc);}
# 771
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp32E;struct _tuple9 _tmp32D;struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp32C;return Cyc_Absyn_new_stmt((void*)((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32E.tag=14,((_tmp32E.f1=s,((_tmp32E.f2=((_tmp32D.f1=e,((_tmp32D.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp32D)))),_tmp32E)))))),_tmp32C)))),loc);}
# 774
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp331;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp330;return Cyc_Absyn_new_stmt((void*)((_tmp330=_cycalloc(sizeof(*_tmp330)),((_tmp330[0]=((_tmp331.tag=15,((_tmp331.f1=s,((_tmp331.f2=scs,_tmp331)))))),_tmp330)))),loc);}
# 777
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp334;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp333;return Cyc_Absyn_new_stmt((void*)((_tmp333=_cycalloc(sizeof(*_tmp333)),((_tmp333[0]=((_tmp334.tag=8,((_tmp334.f1=lab,((_tmp334.f2=0,_tmp334)))))),_tmp333)))),loc);}
# 780
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}
# 784
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){struct Cyc_Absyn_Pat*_tmp335;return(_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335->r=p,((_tmp335->topt=0,((_tmp335->loc=s,_tmp335)))))));}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp338;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp337;return Cyc_Absyn_new_pat((void*)((_tmp337=_cycalloc(sizeof(*_tmp337)),((_tmp337[0]=((_tmp338.tag=16,((_tmp338.f1=e,_tmp338)))),_tmp337)))),e->loc);}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={8};
# 790
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){struct Cyc_Absyn_Decl*_tmp339;return(_tmp339=_cycalloc(sizeof(*_tmp339)),((_tmp339->r=r,((_tmp339->loc=loc,_tmp339)))));}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp33C;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp33B;return Cyc_Absyn_new_decl((void*)((_tmp33B=_cycalloc(sizeof(*_tmp33B)),((_tmp33B[0]=((_tmp33C.tag=2,((_tmp33C.f1=p,((_tmp33C.f2=0,((_tmp33C.f3=e,_tmp33C)))))))),_tmp33B)))),loc);}
# 794
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp33F;struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp33E;return Cyc_Absyn_new_decl((void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E[0]=((_tmp33F.tag=3,((_tmp33F.f1=vds,_tmp33F)))),_tmp33E)))),loc);}
# 797
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*eo,unsigned int loc){
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp342;struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp341;return Cyc_Absyn_new_decl((void*)((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=((_tmp342.tag=4,((_tmp342.f1=tv,((_tmp342.f2=vd,((_tmp342.f3=resetable,((_tmp342.f4=eo,_tmp342)))))))))),_tmp341)))),loc);}
# 800
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct _tmp345;struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp344;return Cyc_Absyn_new_decl((void*)((_tmp344=_cycalloc(sizeof(*_tmp344)),((_tmp344[0]=((_tmp345.tag=5,((_tmp345.f1=tv,((_tmp345.f2=vd,_tmp345)))))),_tmp344)))),loc);}
# 804
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*_tmp346;return(_tmp346=_cycalloc(sizeof(*_tmp346)),((_tmp346->sc=Cyc_Absyn_Public,((_tmp346->name=x,((_tmp346->tq=Cyc_Absyn_empty_tqual(0),((_tmp346->type=t,((_tmp346->initializer=init,((_tmp346->rgn=0,((_tmp346->attributes=0,((_tmp346->escapes=0,_tmp346)))))))))))))))));}
# 809
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*_tmp347;return(_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347->sc=Cyc_Absyn_Static,((_tmp347->name=x,((_tmp347->tq=Cyc_Absyn_empty_tqual(0),((_tmp347->type=t,((_tmp347->initializer=init,((_tmp347->rgn=0,((_tmp347->attributes=0,((_tmp347->escapes=0,_tmp347)))))))))))))))));}
# 814
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 818
struct Cyc_Absyn_AggrdeclImpl*_tmp348;return(_tmp348=_cycalloc(sizeof(*_tmp348)),((_tmp348->exist_vars=exists,((_tmp348->rgn_po=po,((_tmp348->fields=fs,((_tmp348->tagged=tagged,_tmp348)))))))));}
# 821
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 824
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp34E;struct Cyc_Absyn_Aggrdecl*_tmp34D;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp34C;return Cyc_Absyn_new_decl((void*)((_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C[0]=((_tmp34E.tag=6,((_tmp34E.f1=((_tmp34D=_cycalloc(sizeof(*_tmp34D)),((_tmp34D->kind=k,((_tmp34D->sc=s,((_tmp34D->name=n,((_tmp34D->tvs=ts,((_tmp34D->impl=i,((_tmp34D->attributes=atts,_tmp34D)))))))))))))),_tmp34E)))),_tmp34C)))),loc);}
# 828
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 831
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp358;struct Cyc_Absyn_Aggrdecl*_tmp357;struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp356;struct Cyc_Absyn_TypeDecl*_tmp355;return(_tmp355=_cycalloc(sizeof(*_tmp355)),((_tmp355->r=(void*)((_tmp358=_cycalloc(sizeof(*_tmp358)),((_tmp358[0]=((_tmp356.tag=0,((_tmp356.f1=((_tmp357=_cycalloc(sizeof(*_tmp357)),((_tmp357->kind=k,((_tmp357->sc=s,((_tmp357->name=n,((_tmp357->tvs=ts,((_tmp357->impl=i,((_tmp357->attributes=atts,_tmp357)))))))))))))),_tmp356)))),_tmp358)))),((_tmp355->loc=loc,_tmp355)))));}
# 835
struct Cyc_Absyn_Decl*Cyc_Absyn_struct_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 838
return Cyc_Absyn_aggr_decl(Cyc_Absyn_StructA,s,n,ts,i,atts,loc);}
# 840
struct Cyc_Absyn_Decl*Cyc_Absyn_union_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 843
return Cyc_Absyn_aggr_decl(Cyc_Absyn_UnionA,s,n,ts,i,atts,loc);}
# 845
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 848
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp35E;struct Cyc_Absyn_Datatypedecl*_tmp35D;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp35C;return Cyc_Absyn_new_decl((void*)((_tmp35C=_cycalloc(sizeof(*_tmp35C)),((_tmp35C[0]=((_tmp35E.tag=7,((_tmp35E.f1=((_tmp35D=_cycalloc(sizeof(*_tmp35D)),((_tmp35D->sc=s,((_tmp35D->name=n,((_tmp35D->tvs=ts,((_tmp35D->fields=fs,((_tmp35D->is_extensible=is_extensible,_tmp35D)))))))))))),_tmp35E)))),_tmp35C)))),loc);}
# 851
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 854
struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp368;struct Cyc_Absyn_Datatypedecl*_tmp367;struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp366;struct Cyc_Absyn_TypeDecl*_tmp365;return(_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->r=(void*)((_tmp368=_cycalloc(sizeof(*_tmp368)),((_tmp368[0]=((_tmp366.tag=2,((_tmp366.f1=((_tmp367=_cycalloc(sizeof(*_tmp367)),((_tmp367->sc=s,((_tmp367->name=n,((_tmp367->tvs=ts,((_tmp367->fields=fs,((_tmp367->is_extensible=is_extensible,_tmp367)))))))))))),_tmp366)))),_tmp368)))),((_tmp365->loc=loc,_tmp365)))));}
# 866 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp13D=(*((struct _tuple8*)_check_null(a))).f3;
void*_tmp13E=Cyc_Absyn_pointer_expand(_tmp13D,1);
if(_tmp13D != _tmp13E)
(*a).f3=_tmp13E;}
# 873
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){
# 880
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp36E;struct Cyc_Absyn_FnInfo _tmp36D;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36C;return(void*)((_tmp36C=_cycalloc(sizeof(*_tmp36C)),((_tmp36C[0]=((_tmp36E.tag=9,((_tmp36E.f1=((_tmp36D.tvars=tvs,((_tmp36D.ret_tqual=ret_tqual,((_tmp36D.ret_typ=
# 883
Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp36D.effect=eff_typ,((_tmp36D.args=args,((_tmp36D.c_varargs=c_varargs,((_tmp36D.cyc_varargs=cyc_varargs,((_tmp36D.rgn_po=rgn_po,((_tmp36D.attributes=atts,_tmp36D)))))))))))))))))),_tmp36E)))),_tmp36C))));};}
# 893
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_stmttmp7=Cyc_Tcutil_compress(t);void*_tmp142=_stmttmp7;_LL74: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 9)goto _LL76;}_LL75: {
struct Cyc_Core_Opt*_tmp36F;return Cyc_Absyn_at_typ(t,
fresh_evar?
Cyc_Absyn_new_evar(((_tmp36F=_cycalloc(sizeof(*_tmp36F)),((_tmp36F->v=& Cyc_Tcutil_rk,_tmp36F)))),0):(void*)& Cyc_Absyn_HeapRgn_val,
# 899
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL76:;_LL77:
 return t;_LL73:;}
# 915 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_stmttmp8=e->r;void*_tmp145=_stmttmp8;struct Cyc_Absyn_Vardecl*_tmp14A;struct Cyc_Absyn_Vardecl*_tmp14D;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp157;_LL79: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp146=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp146->tag != 1)goto _LL7B;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp147=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp146->f2);if(_tmp147->tag != 2)goto _LL7B;}}_LL7A:
# 918
 return 0;_LL7B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp148=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp148->tag != 1)goto _LL7D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp149=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp148->f2);if(_tmp149->tag != 1)goto _LL7D;else{_tmp14A=_tmp149->f1;}}}_LL7C:
 _tmp14D=_tmp14A;goto _LL7E;_LL7D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp14B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp14B->tag != 1)goto _LL7F;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp14C=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp14B->f2);if(_tmp14C->tag != 4)goto _LL7F;else{_tmp14D=_tmp14C->f1;}}}_LL7E: {
# 921
void*_stmttmp9=Cyc_Tcutil_compress(_tmp14D->type);void*_tmp158=_stmttmp9;_LL90: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158;if(_tmp159->tag != 8)goto _LL92;}_LL91:
 return 0;_LL92:;_LL93:
 return 1;_LL8F:;}_LL7F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp14E->tag != 1)goto _LL81;}_LL80:
# 925
 goto _LL82;_LL81: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp14F->tag != 21)goto _LL83;}_LL82:
 goto _LL84;_LL83: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp150->tag != 19)goto _LL85;}_LL84:
 goto _LL86;_LL85: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp151->tag != 22)goto _LL87;}_LL86:
 return 1;_LL87: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp152->tag != 20)goto _LL89;else{_tmp153=_tmp152->f1;}}_LL88:
 return Cyc_Absyn_is_lvalue(_tmp153);_LL89: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp154->tag != 12)goto _LL8B;else{_tmp155=_tmp154->f1;}}_LL8A:
 return Cyc_Absyn_is_lvalue(_tmp155);_LL8B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp145;if(_tmp156->tag != 11)goto _LL8D;else{_tmp157=_tmp156->f1;}}_LL8C:
 return Cyc_Absyn_is_lvalue(_tmp157);_LL8D:;_LL8E:
 return 0;_LL78:;}
# 936
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmp15A=fields;for(0;_tmp15A != 0;_tmp15A=_tmp15A->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp15A->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmp15A->hd;}}
return 0;}
# 942
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*v){
return ad->impl == 0?0: Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields,v);}
# 946
struct _tuple10*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*ts,int i){
for(0;i != 0;-- i){
if(ts == 0)
return 0;
ts=ts->tl;}
# 952
if(ts == 0)
return 0;
return(struct _tuple10*)ts->hd;}
# 957
struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val={1};
struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val={2};
struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val={3};
struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val={4};
struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val={5};
struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val={7};
struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct Cyc_Absyn_Nocommon_att_val={9};
struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val={10};
struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val={11};
struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val={12};
struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val={13};
struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val={14};
struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val={15};
struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val={16};
struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val={17};
struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val={18};
struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val={23};
# 975
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*a){
void*_tmp16C=a;int _tmp16E;int _tmp175;struct _dyneither_ptr _tmp178;int _tmp184;int _tmp185;int _tmp187;int _tmp188;int _tmp18A;int _tmp18C;int _tmp18E;struct _dyneither_ptr _tmp191;_LL95: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp16D=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp16D->tag != 0)goto _LL97;else{_tmp16E=_tmp16D->f1;}}_LL96: {
const char*_tmp373;void*_tmp372[1];struct Cyc_Int_pa_PrintArg_struct _tmp371;return(struct _dyneither_ptr)((_tmp371.tag=1,((_tmp371.f1=(unsigned long)_tmp16E,((_tmp372[0]=& _tmp371,Cyc_aprintf(((_tmp373="regparm(%d)",_tag_dyneither(_tmp373,sizeof(char),12))),_tag_dyneither(_tmp372,sizeof(void*),1))))))));}_LL97: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp16F=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp16F->tag != 1)goto _LL99;}_LL98: {
const char*_tmp374;return(_tmp374="stdcall",_tag_dyneither(_tmp374,sizeof(char),8));}_LL99: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp170=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp170->tag != 2)goto _LL9B;}_LL9A: {
const char*_tmp375;return(_tmp375="cdecl",_tag_dyneither(_tmp375,sizeof(char),6));}_LL9B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp171=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp171->tag != 3)goto _LL9D;}_LL9C: {
const char*_tmp376;return(_tmp376="fastcall",_tag_dyneither(_tmp376,sizeof(char),9));}_LL9D: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp172=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp172->tag != 4)goto _LL9F;}_LL9E: {
const char*_tmp377;return(_tmp377="noreturn",_tag_dyneither(_tmp377,sizeof(char),9));}_LL9F: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp173=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp173->tag != 5)goto _LLA1;}_LLA0: {
const char*_tmp378;return(_tmp378="const",_tag_dyneither(_tmp378,sizeof(char),6));}_LLA1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp174=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp174->tag != 6)goto _LLA3;else{_tmp175=_tmp174->f1;}}_LLA2:
# 984
 if(_tmp175 == - 1){const char*_tmp379;return(_tmp379="aligned",_tag_dyneither(_tmp379,sizeof(char),8));}else{
# 986
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLC9: {
const char*_tmp37D;void*_tmp37C[1];struct Cyc_Int_pa_PrintArg_struct _tmp37B;return(struct _dyneither_ptr)((_tmp37B.tag=1,((_tmp37B.f1=(unsigned long)_tmp175,((_tmp37C[0]=& _tmp37B,Cyc_aprintf(((_tmp37D="aligned(%d)",_tag_dyneither(_tmp37D,sizeof(char),12))),_tag_dyneither(_tmp37C,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c: _LLCA: {
const char*_tmp381;void*_tmp380[1];struct Cyc_Int_pa_PrintArg_struct _tmp37F;return(struct _dyneither_ptr)((_tmp37F.tag=1,((_tmp37F.f1=(unsigned long)_tmp175,((_tmp380[0]=& _tmp37F,Cyc_aprintf(((_tmp381="align(%d)",_tag_dyneither(_tmp381,sizeof(char),10))),_tag_dyneither(_tmp380,sizeof(void*),1))))))));}}}_LLA3: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp176=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp176->tag != 7)goto _LLA5;}_LLA4: {
# 990
const char*_tmp382;return(_tmp382="packed",_tag_dyneither(_tmp382,sizeof(char),7));}_LLA5: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp177=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp177->tag != 8)goto _LLA7;else{_tmp178=_tmp177->f1;}}_LLA6: {
const char*_tmp386;void*_tmp385[1];struct Cyc_String_pa_PrintArg_struct _tmp384;return(struct _dyneither_ptr)((_tmp384.tag=0,((_tmp384.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp178),((_tmp385[0]=& _tmp384,Cyc_aprintf(((_tmp386="section(\"%s\")",_tag_dyneither(_tmp386,sizeof(char),14))),_tag_dyneither(_tmp385,sizeof(void*),1))))))));}_LLA7: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp179=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp179->tag != 9)goto _LLA9;}_LLA8: {
const char*_tmp387;return(_tmp387="nocommon",_tag_dyneither(_tmp387,sizeof(char),9));}_LLA9: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp17A=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17A->tag != 10)goto _LLAB;}_LLAA: {
const char*_tmp388;return(_tmp388="shared",_tag_dyneither(_tmp388,sizeof(char),7));}_LLAB: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp17B=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17B->tag != 11)goto _LLAD;}_LLAC: {
const char*_tmp389;return(_tmp389="unused",_tag_dyneither(_tmp389,sizeof(char),7));}_LLAD: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp17C=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17C->tag != 12)goto _LLAF;}_LLAE: {
const char*_tmp38A;return(_tmp38A="weak",_tag_dyneither(_tmp38A,sizeof(char),5));}_LLAF: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp17D=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17D->tag != 13)goto _LLB1;}_LLB0: {
const char*_tmp38B;return(_tmp38B="dllimport",_tag_dyneither(_tmp38B,sizeof(char),10));}_LLB1: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp17E=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17E->tag != 14)goto _LLB3;}_LLB2: {
const char*_tmp38C;return(_tmp38C="dllexport",_tag_dyneither(_tmp38C,sizeof(char),10));}_LLB3: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp17F=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp17F->tag != 15)goto _LLB5;}_LLB4: {
const char*_tmp38D;return(_tmp38D="no_instrument_function",_tag_dyneither(_tmp38D,sizeof(char),23));}_LLB5: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp180=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp180->tag != 16)goto _LLB7;}_LLB6: {
const char*_tmp38E;return(_tmp38E="constructor",_tag_dyneither(_tmp38E,sizeof(char),12));}_LLB7: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp181=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp181->tag != 17)goto _LLB9;}_LLB8: {
const char*_tmp38F;return(_tmp38F="destructor",_tag_dyneither(_tmp38F,sizeof(char),11));}_LLB9: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp182=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp182->tag != 18)goto _LLBB;}_LLBA: {
const char*_tmp390;return(_tmp390="no_check_memory_usage",_tag_dyneither(_tmp390,sizeof(char),22));}_LLBB: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp183=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp183->tag != 19)goto _LLBD;else{if(_tmp183->f1 != Cyc_Absyn_Printf_ft)goto _LLBD;_tmp184=_tmp183->f2;_tmp185=_tmp183->f3;}}_LLBC: {
const char*_tmp395;void*_tmp394[2];struct Cyc_Int_pa_PrintArg_struct _tmp393;struct Cyc_Int_pa_PrintArg_struct _tmp392;return(struct _dyneither_ptr)((_tmp392.tag=1,((_tmp392.f1=(unsigned int)_tmp185,((_tmp393.tag=1,((_tmp393.f1=(unsigned int)_tmp184,((_tmp394[0]=& _tmp393,((_tmp394[1]=& _tmp392,Cyc_aprintf(((_tmp395="format(printf,%u,%u)",_tag_dyneither(_tmp395,sizeof(char),21))),_tag_dyneither(_tmp394,sizeof(void*),2))))))))))))));}_LLBD: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp186=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp186->tag != 19)goto _LLBF;else{if(_tmp186->f1 != Cyc_Absyn_Scanf_ft)goto _LLBF;_tmp187=_tmp186->f2;_tmp188=_tmp186->f3;}}_LLBE: {
const char*_tmp39A;void*_tmp399[2];struct Cyc_Int_pa_PrintArg_struct _tmp398;struct Cyc_Int_pa_PrintArg_struct _tmp397;return(struct _dyneither_ptr)((_tmp397.tag=1,((_tmp397.f1=(unsigned int)_tmp188,((_tmp398.tag=1,((_tmp398.f1=(unsigned int)_tmp187,((_tmp399[0]=& _tmp398,((_tmp399[1]=& _tmp397,Cyc_aprintf(((_tmp39A="format(scanf,%u,%u)",_tag_dyneither(_tmp39A,sizeof(char),20))),_tag_dyneither(_tmp399,sizeof(void*),2))))))))))))));}_LLBF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp189=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp189->tag != 20)goto _LLC1;else{_tmp18A=_tmp189->f1;}}_LLC0: {
const char*_tmp39E;void*_tmp39D[1];struct Cyc_Int_pa_PrintArg_struct _tmp39C;return(struct _dyneither_ptr)((_tmp39C.tag=1,((_tmp39C.f1=(unsigned long)_tmp18A,((_tmp39D[0]=& _tmp39C,Cyc_aprintf(((_tmp39E="initializes(%d)",_tag_dyneither(_tmp39E,sizeof(char),16))),_tag_dyneither(_tmp39D,sizeof(void*),1))))))));}_LLC1: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp18B=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp18B->tag != 21)goto _LLC3;else{_tmp18C=_tmp18B->f1;}}_LLC2: {
const char*_tmp3A2;void*_tmp3A1[1];struct Cyc_Int_pa_PrintArg_struct _tmp3A0;return(struct _dyneither_ptr)((_tmp3A0.tag=1,((_tmp3A0.f1=(unsigned long)_tmp18C,((_tmp3A1[0]=& _tmp3A0,Cyc_aprintf(((_tmp3A2="noliveunique(%d)",_tag_dyneither(_tmp3A2,sizeof(char),17))),_tag_dyneither(_tmp3A1,sizeof(void*),1))))))));}_LLC3: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp18D=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp18D->tag != 22)goto _LLC5;else{_tmp18E=_tmp18D->f1;}}_LLC4: {
const char*_tmp3A6;void*_tmp3A5[1];struct Cyc_Int_pa_PrintArg_struct _tmp3A4;return(struct _dyneither_ptr)((_tmp3A4.tag=1,((_tmp3A4.f1=(unsigned long)_tmp18E,((_tmp3A5[0]=& _tmp3A4,Cyc_aprintf(((_tmp3A6="noconsume(%d)",_tag_dyneither(_tmp3A6,sizeof(char),14))),_tag_dyneither(_tmp3A5,sizeof(void*),1))))))));}_LLC5: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp18F=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp18F->tag != 23)goto _LLC7;}_LLC6: {
const char*_tmp3A7;return(_tmp3A7="pure",_tag_dyneither(_tmp3A7,sizeof(char),5));}_LLC7: {struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp190=(struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp16C;if(_tmp190->tag != 24)goto _LL94;else{_tmp191=_tmp190->f1;}}_LLC8: {
const char*_tmp3AB;void*_tmp3AA[1];struct Cyc_String_pa_PrintArg_struct _tmp3A9;return(struct _dyneither_ptr)((_tmp3A9.tag=0,((_tmp3A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp191),((_tmp3AA[0]=& _tmp3A9,Cyc_aprintf(((_tmp3AB="__mode__(\"%s\")",_tag_dyneither(_tmp3AB,sizeof(char),15))),_tag_dyneither(_tmp3AA,sizeof(void*),1))))))));}_LL94:;}
# 1013
int Cyc_Absyn_fntype_att(void*a){
void*_tmp1C4=a;int _tmp1C6;_LLCD: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp1C5=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1C5->tag != 0)goto _LLCF;else{_tmp1C6=_tmp1C5->f1;}}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp1C7=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1C7->tag != 3)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp1C8=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1C8->tag != 1)goto _LLD3;}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp1C9=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1C9->tag != 2)goto _LLD5;}_LLD4:
 goto _LLD6;_LLD5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1CA=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CA->tag != 4)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp1CB=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CB->tag != 23)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1CC=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CC->tag != 19)goto _LLDB;}_LLDA:
 goto _LLDC;_LLDB: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1CD=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CD->tag != 5)goto _LLDD;}_LLDC:
 goto _LLDE;_LLDD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1CE=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CE->tag != 21)goto _LLDF;}_LLDE:
 goto _LLE0;_LLDF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1CF=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1CF->tag != 20)goto _LLE1;}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1D0=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1C4;if(_tmp1D0->tag != 22)goto _LLE3;}_LLE2:
 return 1;_LLE3:;_LLE4:
 return 0;_LLCC:;}static char _tmp1D1[3]="f0";
# 1031
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1D1,_tmp1D1,_tmp1D1 + 3};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(int i);static void _tmp3B9(unsigned int*fsz,unsigned int*_tmp3B8,unsigned int*_tmp3B7,struct _dyneither_ptr***_tmp3B5){for(*_tmp3B8=0;*_tmp3B8 < *_tmp3B7;(*_tmp3B8)++){struct Cyc_Int_pa_PrintArg_struct _tmp3B3;void*_tmp3B2[1];const char*_tmp3B1;struct _dyneither_ptr*_tmp3B0;(*_tmp3B5)[*_tmp3B8]=(struct _dyneither_ptr*)(
# 1039
*_tmp3B8 < *fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)*_tmp3B8)):((_tmp3B0=_cycalloc(sizeof(*_tmp3B0)),((_tmp3B0[0]=(struct _dyneither_ptr)((_tmp3B3.tag=1,((_tmp3B3.f1=(unsigned long)((int)*_tmp3B8),((_tmp3B2[0]=& _tmp3B3,Cyc_aprintf(((_tmp3B1="f%d",_tag_dyneither(_tmp3B1,sizeof(char),4))),_tag_dyneither(_tmp3B2,sizeof(void*),1)))))))),_tmp3B0)))));}}
# 1034
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if(i >= fsz){
unsigned int _tmp3B8;unsigned int _tmp3B7;struct _dyneither_ptr _tmp3B6;struct _dyneither_ptr**_tmp3B5;unsigned int _tmp3B4;Cyc_Absyn_field_names=(
(_tmp3B4=(unsigned int)(i + 1),((_tmp3B5=(struct _dyneither_ptr**)_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp3B4)),((_tmp3B6=_tag_dyneither(_tmp3B5,sizeof(struct _dyneither_ptr*),_tmp3B4),((((_tmp3B7=_tmp3B4,_tmp3B9(& fsz,& _tmp3B8,& _tmp3B7,& _tmp3B5))),_tmp3B6))))))));}
# 1040
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1043
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1DA=info;enum Cyc_Absyn_AggrKind _tmp1DB;struct _tuple0*_tmp1DC;enum Cyc_Absyn_AggrKind _tmp1DD;struct _tuple0*_tmp1DE;_LLE6: if((_tmp1DA.UnknownAggr).tag != 1)goto _LLE8;_tmp1DB=((struct _tuple2)(_tmp1DA.UnknownAggr).val).f1;_tmp1DC=((struct _tuple2)(_tmp1DA.UnknownAggr).val).f2;_LLE7: {
struct _tuple11 _tmp3BA;return(_tmp3BA.f1=_tmp1DB,((_tmp3BA.f2=_tmp1DC,_tmp3BA)));}_LLE8: if((_tmp1DA.KnownAggr).tag != 2)goto _LLE5;_tmp1DD=(*((struct Cyc_Absyn_Aggrdecl**)(_tmp1DA.KnownAggr).val))->kind;_tmp1DE=(*((struct Cyc_Absyn_Aggrdecl**)(_tmp1DA.KnownAggr).val))->name;_LLE9: {
struct _tuple11 _tmp3BB;return(_tmp3BB.f1=_tmp1DD,((_tmp3BB.f2=_tmp1DE,_tmp3BB)));}_LLE5:;}
# 1049
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1E1=info;struct Cyc_Absyn_Aggrdecl*_tmp1E2;_LLEB: if((_tmp1E1.UnknownAggr).tag != 1)goto _LLED;_LLEC: {
const char*_tmp3BE;void*_tmp3BD;(_tmp3BD=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3BE="unchecked aggrdecl",_tag_dyneither(_tmp3BE,sizeof(char),19))),_tag_dyneither(_tmp3BD,sizeof(void*),0)));}_LLED: if((_tmp1E1.KnownAggr).tag != 2)goto _LLEA;_tmp1E2=*((struct Cyc_Absyn_Aggrdecl**)(_tmp1E1.KnownAggr).val);_LLEE:
 return _tmp1E2;_LLEA:;}
# 1055
int Cyc_Absyn_is_union_type(void*t){
void*_stmttmpA=Cyc_Tcutil_compress(t);void*_tmp1E5=_stmttmpA;union Cyc_Absyn_AggrInfoU _tmp1E8;_LLF0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1E6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E5;if(_tmp1E6->tag != 12)goto _LLF2;else{if(_tmp1E6->f1 != Cyc_Absyn_UnionA)goto _LLF2;}}_LLF1:
 return 1;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1E7=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E5;if(_tmp1E7->tag != 11)goto _LLF4;else{_tmp1E8=(_tmp1E7->f1).aggr_info;}}_LLF3:
 return(Cyc_Absyn_aggr_kinded_name(_tmp1E8)).f1 == Cyc_Absyn_UnionA;_LLF4:;_LLF5:
 return 0;_LLEF:;}
# 1062
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_stmttmpB=Cyc_Tcutil_compress(t);void*_tmp1E9=_stmttmpB;struct Cyc_List_List*_tmp1EB;union Cyc_Absyn_AggrInfoU _tmp1ED;_LLF7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1EA=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1E9;if(_tmp1EA->tag != 12)goto _LLF9;else{if(_tmp1EA->f1 != Cyc_Absyn_UnionA)goto _LLF9;_tmp1EB=_tmp1EA->f2;}}_LLF8:
# 1065
 if(_tmp1EB == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1EB->hd)->requires_clause == 0;_LLF9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1EC=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1E9;if(_tmp1EC->tag != 11)goto _LLFB;else{_tmp1ED=(_tmp1EC->f1).aggr_info;}}_LLFA: {
# 1068
union Cyc_Absyn_AggrInfoU _tmp1EE=_tmp1ED;struct Cyc_Absyn_Aggrdecl*_tmp1EF;enum Cyc_Absyn_AggrKind _tmp1F0;enum Cyc_Absyn_AggrKind _tmp1F1;int _tmp1F2;_LLFE: if((_tmp1EE.KnownAggr).tag != 2)goto _LL100;_tmp1EF=*((struct Cyc_Absyn_Aggrdecl**)(_tmp1EE.KnownAggr).val);_LLFF:
# 1070
 if(_tmp1EF->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1F3=((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp1EF))->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F3))->tagged)return 0;{
struct Cyc_List_List*_tmp1F4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1F3))->fields;
if(_tmp1F4 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1F4->hd)->requires_clause == 0;};};_LL100: if((_tmp1EE.UnknownAggr).tag != 1)goto _LL102;_tmp1F0=((struct _tuple2)(_tmp1EE.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1EE.UnknownAggr).val).f3 != 0)goto _LL102;_LL101:
 return _tmp1F0 == Cyc_Absyn_UnionA;_LL102: if((_tmp1EE.UnknownAggr).tag != 1)goto _LLFD;_tmp1F1=((struct _tuple2)(_tmp1EE.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1EE.UnknownAggr).val).f3 == 0)goto _LLFD;_tmp1F2=(int)(((struct _tuple2)(_tmp1EE.UnknownAggr).val).f3)->v;_LL103:
 return _tmp1F1 == Cyc_Absyn_UnionA  && !_tmp1F2;_LLFD:;}_LLFB:;_LLFC:
# 1079
 return 0;_LLF6:;}
# 1082
int Cyc_Absyn_is_require_union_type(void*t){
void*_stmttmpC=Cyc_Tcutil_compress(t);void*_tmp1F5=_stmttmpC;struct Cyc_List_List*_tmp1F7;union Cyc_Absyn_AggrInfoU _tmp1F9;_LL105: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 12)goto _LL107;else{if(_tmp1F6->f1 != Cyc_Absyn_UnionA)goto _LL107;_tmp1F7=_tmp1F6->f2;}}_LL106:
# 1085
 if(_tmp1F7 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp1F7->hd)->requires_clause != 0;_LL107: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F8->tag != 11)goto _LL109;else{_tmp1F9=(_tmp1F8->f1).aggr_info;}}_LL108: {
# 1088
union Cyc_Absyn_AggrInfoU _tmp1FA=_tmp1F9;struct Cyc_Absyn_Aggrdecl*_tmp1FB;enum Cyc_Absyn_AggrKind _tmp1FC;enum Cyc_Absyn_AggrKind _tmp1FD;int _tmp1FE;_LL10C: if((_tmp1FA.KnownAggr).tag != 2)goto _LL10E;_tmp1FB=*((struct Cyc_Absyn_Aggrdecl**)(_tmp1FA.KnownAggr).val);_LL10D:
# 1090
 if(_tmp1FB->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1FF=((struct Cyc_Absyn_Aggrdecl*)_check_null(_tmp1FB))->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FF))->tagged)return 0;{
struct Cyc_List_List*_tmp200=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FF))->fields;
if(_tmp200 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp200->hd)->requires_clause != 0;};};_LL10E: if((_tmp1FA.UnknownAggr).tag != 1)goto _LL110;_tmp1FC=((struct _tuple2)(_tmp1FA.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3 != 0)goto _LL110;_LL10F:
 return 0;_LL110: if((_tmp1FA.UnknownAggr).tag != 1)goto _LL10B;_tmp1FD=((struct _tuple2)(_tmp1FA.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3 == 0)goto _LL10B;_tmp1FE=(int)(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3)->v;_LL111:
 return 0;_LL10B:;}_LL109:;_LL10A:
# 1099
 return 0;_LL104:;}
# 1102
int Cyc_Absyn_is_aggr_type(void*t){
void*_stmttmpD=Cyc_Tcutil_compress(t);void*_tmp201=_stmttmpD;_LL113: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp202=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp201;if(_tmp202->tag != 3)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp201;if(_tmp203->tag != 4)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp204=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp201;if(_tmp204->tag != 10)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp201;if(_tmp205->tag != 11)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp206=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp201;if(_tmp206->tag != 12)goto _LL11D;}_LL11C:
 return 1;_LL11D:;_LL11E:
 return 0;_LL112:;}
# 1113
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={14};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={15};
# 1123 "absyn.cyc"
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
