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
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71
void*Cyc_Tcutil_compress(void*t);
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 112
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 137
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 145
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
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
struct _tuple12 _tmp217;struct _tuple12 _tmp6=(_tmp217.f1=_tmp1,((_tmp217.f2=_tmp4,_tmp217)));struct _tuple12 _tmp7=_tmp6;struct Cyc_List_List*_tmp8;struct Cyc_List_List*_tmp9;struct Cyc_List_List*_tmpA;struct Cyc_List_List*_tmpB;struct Cyc_List_List*_tmpC;struct Cyc_List_List*_tmpD;_LL1: if(((_tmp7.f1).Loc_n).tag != 4)goto _LL3;if(((_tmp7.f2).Loc_n).tag != 4)goto _LL3;_LL2:
 return 0;_LL3: if(((_tmp7.f1).Rel_n).tag != 1)goto _LL5;_tmp8=(struct Cyc_List_List*)((_tmp7.f1).Rel_n).val;if(((_tmp7.f2).Rel_n).tag != 1)goto _LL5;_tmp9=(struct Cyc_List_List*)((_tmp7.f2).Rel_n).val;_LL4:
 return Cyc_Absyn_strlist_cmp(_tmp8,_tmp9);_LL5: if(((_tmp7.f1).Abs_n).tag != 2)goto _LL7;_tmpA=(struct Cyc_List_List*)((_tmp7.f1).Abs_n).val;if(((_tmp7.f2).Abs_n).tag != 2)goto _LL7;_tmpB=(struct Cyc_List_List*)((_tmp7.f2).Abs_n).val;_LL6:
 return Cyc_Absyn_strlist_cmp(_tmpA,_tmpB);_LL7: if(((_tmp7.f1).C_n).tag != 3)goto _LL9;_tmpC=(struct Cyc_List_List*)((_tmp7.f1).C_n).val;if(((_tmp7.f2).C_n).tag != 3)goto _LL9;_tmpD=(struct Cyc_List_List*)((_tmp7.f2).C_n).val;_LL8:
 return Cyc_Absyn_strlist_cmp(_tmpC,_tmpD);_LL9: if(((_tmp7.f1).Loc_n).tag != 4)goto _LLB;_LLA:
# 70
 return - 1;_LLB: if(((_tmp7.f2).Loc_n).tag != 4)goto _LLD;_LLC:
 return 1;_LLD: if(((_tmp7.f1).Rel_n).tag != 1)goto _LLF;_LLE:
 return - 1;_LLF: if(((_tmp7.f2).Rel_n).tag != 1)goto _LL11;_LL10:
 return 1;_LL11: if(((_tmp7.f1).Abs_n).tag != 2)goto _LL13;_LL12:
 return - 1;_LL13: if(((_tmp7.f2).Abs_n).tag != 2)goto _LL0;_LL14:
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
union Cyc_Absyn_Nmspace _tmp218;return((_tmp218.C_n).val=x,(((_tmp218.C_n).tag=3,_tmp218)));}else{
# 90
union Cyc_Absyn_Nmspace _tmp219;return((_tmp219.Abs_n).val=x,(((_tmp219.Abs_n).tag=2,_tmp219)));}}
# 92
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*x){union Cyc_Absyn_Nmspace _tmp21A;return((_tmp21A.Rel_n).val=x,(((_tmp21A.Rel_n).tag=1,_tmp21A)));}
union Cyc_Absyn_Nmspace Cyc_Absyn_rel_ns_null={.Rel_n={1,0}};
# 95
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*qv){
union Cyc_Absyn_Nmspace _tmp12=(*qv).f1;union Cyc_Absyn_Nmspace _tmp13=_tmp12;_LL16: if((_tmp13.Rel_n).tag != 1)goto _LL18;if((struct Cyc_List_List*)(_tmp13.Rel_n).val != 0)goto _LL18;_LL17:
 goto _LL19;_LL18: if((_tmp13.Abs_n).tag != 2)goto _LL1A;if((struct Cyc_List_List*)(_tmp13.Abs_n).val != 0)goto _LL1A;_LL19:
 goto _LL1B;_LL1A: if((_tmp13.Loc_n).tag != 4)goto _LL1C;_LL1B:
 return 0;_LL1C:;_LL1D:
 return 1;_LL15:;}
# 104
static int Cyc_Absyn_new_type_counter=0;
# 106
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*env){
struct Cyc_Absyn_Evar_Absyn_Type_struct _tmp21D;struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp21C;return(void*)((_tmp21C=_cycalloc(sizeof(*_tmp21C)),((_tmp21C[0]=((_tmp21D.tag=1,((_tmp21D.f1=k,((_tmp21D.f2=0,((_tmp21D.f3=Cyc_Absyn_new_type_counter ++,((_tmp21D.f4=env,_tmp21D)))))))))),_tmp21C))));}
# 109
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*tenv){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,tenv);}
# 113
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y){
struct Cyc_Absyn_Tqual _tmp21E;return(_tmp21E.print_const=x.print_const  || y.print_const,((_tmp21E.q_volatile=
x.q_volatile  || y.q_volatile,((_tmp21E.q_restrict=
x.q_restrict  || y.q_restrict,((_tmp21E.real_const=
x.real_const  || y.real_const,((_tmp21E.loc=
Cyc_Position_segment_join(x.loc,y.loc),_tmp21E)))))))));}
# 121
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp21F;return(_tmp21F.print_const=0,((_tmp21F.q_volatile=0,((_tmp21F.q_restrict=0,((_tmp21F.real_const=0,((_tmp21F.loc=loc,_tmp21F)))))))));}
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int loc){struct Cyc_Absyn_Tqual _tmp220;return(_tmp220.print_const=1,((_tmp220.q_volatile=0,((_tmp220.q_restrict=0,((_tmp220.real_const=1,((_tmp220.loc=loc,_tmp220)))))))));}
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
union Cyc_Absyn_DatatypeInfoU _tmp221;return((_tmp221.UnknownDatatype).val=udi,(((_tmp221.UnknownDatatype).tag=1,_tmp221)));}
# 138
extern union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**d){
union Cyc_Absyn_DatatypeInfoU _tmp222;return((_tmp222.KnownDatatype).val=d,(((_tmp222.KnownDatatype).tag=2,_tmp222)));}
# 141
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo s){
union Cyc_Absyn_DatatypeFieldInfoU _tmp223;return((_tmp223.UnknownDatatypefield).val=s,(((_tmp223.UnknownDatatypefield).tag=1,_tmp223)));}
# 144
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*dd,struct Cyc_Absyn_Datatypefield*df){
struct _tuple1 _tmp226;union Cyc_Absyn_DatatypeFieldInfoU _tmp225;return((_tmp225.KnownDatatypefield).val=((_tmp226.f1=dd,((_tmp226.f2=df,_tmp226)))),(((_tmp225.KnownDatatypefield).tag=2,_tmp225)));}
# 147
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind ak,struct _tuple0*n,struct Cyc_Core_Opt*tagged){
struct _tuple2 _tmp229;union Cyc_Absyn_AggrInfoU _tmp228;return((_tmp228.UnknownAggr).val=((_tmp229.f1=ak,((_tmp229.f2=n,((_tmp229.f3=tagged,_tmp229)))))),(((_tmp228.UnknownAggr).tag=1,_tmp228)));}
# 150
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**ad){
union Cyc_Absyn_AggrInfoU _tmp22A;return((_tmp22A.KnownAggr).val=ad,(((_tmp22A.KnownAggr).tag=2,_tmp22A)));}
# 154
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x){union Cyc_Absyn_Constraint*_tmp22B;return(_tmp22B=_cycalloc(sizeof(*_tmp22B)),(((_tmp22B->Eq_constr).val=x,(((_tmp22B->Eq_constr).tag=1,_tmp22B)))));}
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref(){union Cyc_Absyn_Constraint*_tmp22C;return(_tmp22C=_cycalloc_atomic(sizeof(*_tmp22C)),(((_tmp22C->No_constr).val=0,(((_tmp22C->No_constr).tag=3,_tmp22C)))));}
# 161
struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val={0};
# 163
static union Cyc_Absyn_Constraint Cyc_Absyn_true_conref_v={.Eq_constr={1,(void*)1}};union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref=& Cyc_Absyn_true_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_false_conref_v={.Eq_constr={1,(void*)0}};union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref=& Cyc_Absyn_false_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_one_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_one_bt)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref=& Cyc_Absyn_bounds_one_conref_v;
static union Cyc_Absyn_Constraint Cyc_Absyn_bounds_dyneither_conref_v={.Eq_constr={1,(void*)((void*)& Cyc_Absyn_DynEither_b_val)}};union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref=& Cyc_Absyn_bounds_dyneither_conref_v;
# 170
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp27=x;union Cyc_Absyn_Constraint*_tmp28;_LL1F: if((_tmp27->No_constr).tag != 3)goto _LL21;_LL20:
 goto _LL22;_LL21: if((_tmp27->Eq_constr).tag != 1)goto _LL23;_LL22:
 return x;_LL23: if((_tmp27->Forward_constr).tag != 2)goto _LL1E;_tmp28=(union Cyc_Absyn_Constraint*)(_tmp27->Forward_constr).val;_LL24: {
# 175
union Cyc_Absyn_Constraint*_tmp29=Cyc_Absyn_compress_conref(_tmp28);
{struct _union_Constraint_Forward_constr*_tmp22D;(_tmp22D=& x->Forward_constr,((_tmp22D->tag=2,_tmp22D->val=_tmp29)));}
return _tmp29;}_LL1E:;}
# 181
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp2B=Cyc_Absyn_compress_conref(x);union Cyc_Absyn_Constraint*_tmp2C=_tmp2B;void*_tmp2D;_LL26: if((_tmp2C->Eq_constr).tag != 1)goto _LL28;_tmp2D=(void*)(_tmp2C->Eq_constr).val;_LL27:
 return _tmp2D;_LL28:;_LL29: {
const char*_tmp230;void*_tmp22F;(_tmp22F=0,Cyc_Tcutil_impos(((_tmp230="conref_val",_tag_dyneither(_tmp230,sizeof(char),11))),_tag_dyneither(_tmp22F,sizeof(void*),0)));}_LL25:;}
# 188
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x){
union Cyc_Absyn_Constraint*_tmp30=x;void*_tmp31;_LL2B: if((_tmp30->Eq_constr).tag != 1)goto _LL2D;_tmp31=(void*)(_tmp30->Eq_constr).val;_LL2C:
 return _tmp31;_LL2D: if((_tmp30->No_constr).tag != 3)goto _LL2F;_LL2E:
 return y;_LL2F:;_LL30:
 return Cyc_Absyn_conref_def(y,Cyc_Absyn_compress_conref(x));_LL2A:;}
# 196
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x){
x=Cyc_Absyn_compress_conref(x);{
union Cyc_Absyn_Constraint*_tmp32=x;void*_tmp33;_LL32: if((_tmp32->Eq_constr).tag != 1)goto _LL34;_tmp33=(void*)(_tmp32->Eq_constr).val;_LL33:
 return _tmp33;_LL34:;_LL35:
{struct _union_Constraint_Eq_constr*_tmp231;(_tmp231=& x->Eq_constr,((_tmp231->tag=1,_tmp231->val=y)));}return y;_LL31:;};}
# 204
void*Cyc_Absyn_compress_kb(void*k){
void*_tmp35=k;void**_tmp3A;void**_tmp3C;_LL37: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp36=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp36->tag != 0)goto _LL39;}_LL38:
 goto _LL3A;_LL39: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp37=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp37->tag != 1)goto _LL3B;else{if(_tmp37->f1 != 0)goto _LL3B;}}_LL3A:
 goto _LL3C;_LL3B: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp38=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp38->tag != 2)goto _LL3D;else{if(_tmp38->f1 != 0)goto _LL3D;}}_LL3C:
 return k;_LL3D: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp39=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp39->tag != 1)goto _LL3F;else{if(_tmp39->f1 == 0)goto _LL3F;_tmp3A=(void**)&(_tmp39->f1)->v;}}_LL3E:
 _tmp3C=_tmp3A;goto _LL40;_LL3F: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3B=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp3B->tag != 2)goto _LL36;else{if(_tmp3B->f1 == 0)goto _LL36;_tmp3C=(void**)&(_tmp3B->f1)->v;}}_LL40:
# 211
*_tmp3C=Cyc_Absyn_compress_kb(*_tmp3C);
return*_tmp3C;_LL36:;}
# 216
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb){
void*_tmp3D=Cyc_Absyn_compress_kb(kb);void*_tmp3E=_tmp3D;struct Cyc_Absyn_Kind*_tmp40;struct Cyc_Core_Opt**_tmp42;struct Cyc_Core_Opt**_tmp44;struct Cyc_Absyn_Kind*_tmp45;_LL42: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3E;if(_tmp3F->tag != 0)goto _LL44;else{_tmp40=_tmp3F->f1;}}_LL43:
 return _tmp40;_LL44: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp41=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp3E;if(_tmp41->tag != 1)goto _LL46;else{_tmp42=(struct Cyc_Core_Opt**)& _tmp41->f1;}}_LL45:
 _tmp44=_tmp42;_tmp45=& Cyc_Tcutil_bk;goto _LL47;_LL46: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp43=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3E;if(_tmp43->tag != 2)goto _LL41;else{_tmp44=(struct Cyc_Core_Opt**)& _tmp43->f1;_tmp45=_tmp43->f2;}}_LL47:
# 221
{struct Cyc_Core_Opt*_tmp232;*_tmp44=((_tmp232=_cycalloc(sizeof(*_tmp232)),((_tmp232->v=Cyc_Tcutil_kind_to_bound(_tmp45),_tmp232))));}
return _tmp45;_LL41:;}
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
 return Wchar_t_unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;}}static char _tmp5D[4]="exn";
# 328
static struct _dyneither_ptr Cyc_Absyn_exn_str={_tmp5D,_tmp5D,_tmp5D + 4};
static struct _tuple0 Cyc_Absyn_exn_name_v={{.Abs_n={2,0}},& Cyc_Absyn_exn_str};
struct _tuple0*Cyc_Absyn_exn_name=& Cyc_Absyn_exn_name_v;static char _tmp5E[15]="Null_Exception";
# 340 "absyn.cyc"
static struct _dyneither_ptr Cyc_Absyn_Null_Exception_str={_tmp5E,_tmp5E,_tmp5E + 15};static struct _tuple0 Cyc_Absyn_Null_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Null_Exception_str};struct _tuple0*Cyc_Absyn_Null_Exception_name=& Cyc_Absyn_Null_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Null_Exception_tuf_v={& Cyc_Absyn_Null_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Null_Exception_tuf=& Cyc_Absyn_Null_Exception_tuf_v;static char _tmp5F[13]="Array_bounds";
static struct _dyneither_ptr Cyc_Absyn_Array_bounds_str={_tmp5F,_tmp5F,_tmp5F + 13};static struct _tuple0 Cyc_Absyn_Array_bounds_pr={{.Abs_n={2,0}},& Cyc_Absyn_Array_bounds_str};struct _tuple0*Cyc_Absyn_Array_bounds_name=& Cyc_Absyn_Array_bounds_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Array_bounds_tuf_v={& Cyc_Absyn_Array_bounds_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Array_bounds_tuf=& Cyc_Absyn_Array_bounds_tuf_v;static char _tmp60[16]="Match_Exception";
static struct _dyneither_ptr Cyc_Absyn_Match_Exception_str={_tmp60,_tmp60,_tmp60 + 16};static struct _tuple0 Cyc_Absyn_Match_Exception_pr={{.Abs_n={2,0}},& Cyc_Absyn_Match_Exception_str};struct _tuple0*Cyc_Absyn_Match_Exception_name=& Cyc_Absyn_Match_Exception_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Match_Exception_tuf_v={& Cyc_Absyn_Match_Exception_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Match_Exception_tuf=& Cyc_Absyn_Match_Exception_tuf_v;static char _tmp61[10]="Bad_alloc";
static struct _dyneither_ptr Cyc_Absyn_Bad_alloc_str={_tmp61,_tmp61,_tmp61 + 10};static struct _tuple0 Cyc_Absyn_Bad_alloc_pr={{.Abs_n={2,0}},& Cyc_Absyn_Bad_alloc_str};struct _tuple0*Cyc_Absyn_Bad_alloc_name=& Cyc_Absyn_Bad_alloc_pr;static struct Cyc_Absyn_Datatypefield Cyc_Absyn_Bad_alloc_tuf_v={& Cyc_Absyn_Bad_alloc_pr,0,0,Cyc_Absyn_Extern};struct Cyc_Absyn_Datatypefield*Cyc_Absyn_Bad_alloc_tuf=& Cyc_Absyn_Bad_alloc_tuf_v;
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
struct Cyc_Core_Opt*_tmp233;exn_type_val=((_tmp233=_cycalloc(sizeof(*_tmp233)),((_tmp233->v=Cyc_Absyn_at_typ((void*)& Cyc_Absyn_exn_typ_tt,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp233))));}
# 365
return(void*)exn_type_val->v;}static char _tmp64[9]="PrintArg";
# 369
static struct _dyneither_ptr Cyc_Absyn_printarg_str={_tmp64,_tmp64,_tmp64 + 9};static char _tmp65[9]="ScanfArg";
static struct _dyneither_ptr Cyc_Absyn_scanfarg_str={_tmp65,_tmp65,_tmp65 + 9};
static struct _tuple0 Cyc_Absyn_datatype_print_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_printarg_str};
# 373
static struct _tuple0 Cyc_Absyn_datatype_scanf_arg_qvar_p={{.Abs_n={2,0}},& Cyc_Absyn_scanfarg_str};
# 375
struct _tuple0*Cyc_Absyn_datatype_print_arg_qvar=& Cyc_Absyn_datatype_print_arg_qvar_p;
struct _tuple0*Cyc_Absyn_datatype_scanf_arg_qvar=& Cyc_Absyn_datatype_scanf_arg_qvar_p;static char _tmp66[14]="unique_region";
# 383
static struct _dyneither_ptr Cyc_Absyn_uniquergn_str={_tmp66,_tmp66,_tmp66 + 14};static char _tmp67[5]="Core";
static struct _dyneither_ptr Cyc_Absyn_Core_str={_tmp67,_tmp67,_tmp67 + 5};
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
void*_tmp6B=Cyc_Tcutil_compress(rgn);void*_tmp6C=_tmp6B;_LL63: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp6D=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6C;if(_tmp6D->tag != 20)goto _LL65;}_LL64:
# 412
 if(Cyc_Absyn_string_t_opt == 0){
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 415
void**_tmp234;Cyc_Absyn_string_t_opt=((_tmp234=_cycalloc(sizeof(*_tmp234)),((_tmp234[0]=t,_tmp234))));}
# 417
return*((void**)_check_null(Cyc_Absyn_string_t_opt));_LL65:;_LL66:
# 419
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_empty_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL62:;}
# 422
static void**Cyc_Absyn_const_string_t_opt=0;
void*Cyc_Absyn_const_string_typ(void*rgn){
void*_tmp6F=Cyc_Tcutil_compress(rgn);void*_tmp70=_tmp6F;_LL68: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp71=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp70;if(_tmp71->tag != 20)goto _LL6A;}_LL69:
# 426
 if(Cyc_Absyn_const_string_t_opt == 0){
void*t=Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 429
void**_tmp235;Cyc_Absyn_const_string_t_opt=((_tmp235=_cycalloc(sizeof(*_tmp235)),((_tmp235[0]=t,_tmp235))));}
# 431
return*((void**)_check_null(Cyc_Absyn_const_string_t_opt));_LL6A:;_LL6B:
# 433
 return Cyc_Absyn_starb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);_LL67:;}
# 438
void*Cyc_Absyn_starb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 440
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp23B;struct Cyc_Absyn_PtrInfo _tmp23A;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp239;return(void*)((_tmp239=_cycalloc(sizeof(*_tmp239)),((_tmp239[0]=((_tmp23B.tag=5,((_tmp23B.f1=((_tmp23A.elt_typ=t,((_tmp23A.elt_tq=tq,((_tmp23A.ptr_atts=(
((_tmp23A.ptr_atts).rgn=r,(((_tmp23A.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp23A.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp23A.ptr_atts).zero_term=zeroterm,(((_tmp23A.ptr_atts).ptrloc=0,_tmp23A.ptr_atts)))))))))),_tmp23A)))))),_tmp23B)))),_tmp239))));}
# 446
void*Cyc_Absyn_atb_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zeroterm){
# 448
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp241;struct Cyc_Absyn_PtrInfo _tmp240;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp23F;return(void*)((_tmp23F=_cycalloc(sizeof(*_tmp23F)),((_tmp23F[0]=((_tmp241.tag=5,((_tmp241.f1=((_tmp240.elt_typ=t,((_tmp240.elt_tq=tq,((_tmp240.ptr_atts=(
((_tmp240.ptr_atts).rgn=r,(((_tmp240.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp240.ptr_atts).bounds=
((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)(b),(((_tmp240.ptr_atts).zero_term=zeroterm,(((_tmp240.ptr_atts).ptrloc=0,_tmp240.ptr_atts)))))))))),_tmp240)))))),_tmp241)))),_tmp23F))));}
# 455
void*Cyc_Absyn_star_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp247;struct Cyc_Absyn_PtrInfo _tmp246;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp245;return(void*)((_tmp245=_cycalloc(sizeof(*_tmp245)),((_tmp245[0]=((_tmp247.tag=5,((_tmp247.f1=((_tmp246.elt_typ=t,((_tmp246.elt_tq=tq,((_tmp246.ptr_atts=(
((_tmp246.ptr_atts).rgn=r,(((_tmp246.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp246.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp246.ptr_atts).zero_term=zeroterm,(((_tmp246.ptr_atts).ptrloc=0,_tmp246.ptr_atts)))))))))),_tmp246)))))),_tmp247)))),_tmp245))));}
# 462
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp24D;struct Cyc_Absyn_PtrInfo _tmp24C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp24B;return(void*)((_tmp24B=_cycalloc(sizeof(*_tmp24B)),((_tmp24B[0]=((_tmp24D.tag=5,((_tmp24D.f1=((_tmp24C.elt_typ=t,((_tmp24C.elt_tq=tq,((_tmp24C.ptr_atts=(
((_tmp24C.ptr_atts).rgn=(void*)& Cyc_Absyn_HeapRgn_val,(((_tmp24C.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp24C.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp24C.ptr_atts).zero_term=Cyc_Absyn_false_conref,(((_tmp24C.ptr_atts).ptrloc=0,_tmp24C.ptr_atts)))))))))),_tmp24C)))))),_tmp24D)))),_tmp24B))));}
# 470
void*Cyc_Absyn_at_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp253;struct Cyc_Absyn_PtrInfo _tmp252;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp251;return(void*)((_tmp251=_cycalloc(sizeof(*_tmp251)),((_tmp251[0]=((_tmp253.tag=5,((_tmp253.f1=((_tmp252.elt_typ=t,((_tmp252.elt_tq=tq,((_tmp252.ptr_atts=(
((_tmp252.ptr_atts).rgn=r,(((_tmp252.ptr_atts).nullable=Cyc_Absyn_false_conref,(((_tmp252.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,(((_tmp252.ptr_atts).zero_term=zeroterm,(((_tmp252.ptr_atts).ptrloc=0,_tmp252.ptr_atts)))))))))),_tmp252)))))),_tmp253)))),_tmp251))));}
# 477
void*Cyc_Absyn_dyneither_typ(void*t,void*r,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zeroterm){
struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp259;struct Cyc_Absyn_PtrInfo _tmp258;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp257;return(void*)((_tmp257=_cycalloc(sizeof(*_tmp257)),((_tmp257[0]=((_tmp259.tag=5,((_tmp259.f1=((_tmp258.elt_typ=t,((_tmp258.elt_tq=tq,((_tmp258.ptr_atts=(
((_tmp258.ptr_atts).rgn=r,(((_tmp258.ptr_atts).nullable=Cyc_Absyn_true_conref,(((_tmp258.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,(((_tmp258.ptr_atts).zero_term=zeroterm,(((_tmp258.ptr_atts).ptrloc=0,_tmp258.ptr_atts)))))))))),_tmp258)))))),_tmp259)))),_tmp257))));}
# 485
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc){
# 487
struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp25F;struct Cyc_Absyn_ArrayInfo _tmp25E;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp25D;return(void*)((_tmp25D=_cycalloc(sizeof(*_tmp25D)),((_tmp25D[0]=((_tmp25F.tag=8,((_tmp25F.f1=((_tmp25E.elt_type=elt_type,((_tmp25E.tq=tq,((_tmp25E.num_elts=num_elts,((_tmp25E.zero_term=zero_term,((_tmp25E.zt_loc=ztloc,_tmp25E)))))))))),_tmp25F)))),_tmp25D))));}static char _tmp8F[8]="__sFILE";
# 495
void*Cyc_Absyn_file_typ(){
static void**file_t_opt=0;
static struct _dyneither_ptr sf_str={_tmp8F,_tmp8F,_tmp8F + 8};
static struct _dyneither_ptr*sf=& sf_str;
# 500
if(file_t_opt == 0){
struct _tuple0*_tmp260;struct _tuple0*file_t_name=(_tmp260=_cycalloc(sizeof(*_tmp260)),((_tmp260->f1=Cyc_Absyn_Abs_n(0,0),((_tmp260->f2=sf,_tmp260)))));
struct Cyc_Absyn_Aggrdecl*_tmp261;struct Cyc_Absyn_Aggrdecl*sd=(_tmp261=_cycalloc(sizeof(*_tmp261)),((_tmp261->kind=Cyc_Absyn_StructA,((_tmp261->sc=Cyc_Absyn_Abstract,((_tmp261->name=file_t_name,((_tmp261->tvs=0,((_tmp261->impl=0,((_tmp261->attributes=0,_tmp261)))))))))))));
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp26B;struct Cyc_Absyn_Aggrdecl**_tmp26A;struct Cyc_Absyn_AggrInfo _tmp269;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp268;void*file_struct_typ=(void*)((_tmp268=_cycalloc(sizeof(*_tmp268)),((_tmp268[0]=((_tmp26B.tag=11,((_tmp26B.f1=((_tmp269.aggr_info=Cyc_Absyn_KnownAggr(((_tmp26A=_cycalloc(sizeof(*_tmp26A)),((_tmp26A[0]=sd,_tmp26A))))),((_tmp269.targs=0,_tmp269)))),_tmp26B)))),_tmp268))));
void**_tmp26C;file_t_opt=((_tmp26C=_cycalloc(sizeof(*_tmp26C)),((_tmp26C[0]=Cyc_Absyn_at_typ(file_struct_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp26C))));}
# 507
return*file_t_opt;}
# 510
void*Cyc_Absyn_void_star_typ(){
static void**void_star_t_opt=0;
# 513
if(void_star_t_opt == 0){
void**_tmp26D;void_star_t_opt=((_tmp26D=_cycalloc(sizeof(*_tmp26D)),((_tmp26D[0]=Cyc_Absyn_star_typ((void*)& Cyc_Absyn_VoidType_val,(void*)& Cyc_Absyn_HeapRgn_val,
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),_tmp26D))));}
return*void_star_t_opt;}
# 518
static struct Cyc_Absyn_JoinEff_Absyn_Type_struct Cyc_Absyn_empty_eff={24,0};
void*Cyc_Absyn_empty_effect=(void*)& Cyc_Absyn_empty_eff;
# 525
void*Cyc_Absyn_aggr_typ(enum Cyc_Absyn_AggrKind k,struct _dyneither_ptr*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp277;struct _tuple0*_tmp276;struct Cyc_Absyn_AggrInfo _tmp275;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp274;return(void*)((_tmp274=_cycalloc(sizeof(*_tmp274)),((_tmp274[0]=((_tmp277.tag=11,((_tmp277.f1=((_tmp275.aggr_info=Cyc_Absyn_UnknownAggr(k,((_tmp276=_cycalloc(sizeof(*_tmp276)),((_tmp276->f1=Cyc_Absyn_rel_ns_null,((_tmp276->f2=name,_tmp276)))))),0),((_tmp275.targs=0,_tmp275)))),_tmp277)))),_tmp274))));}
# 529
void*Cyc_Absyn_strct(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_StructA,name);}
void*Cyc_Absyn_union_typ(struct _dyneither_ptr*name){return Cyc_Absyn_aggr_typ(Cyc_Absyn_UnionA,name);}
# 532
void*Cyc_Absyn_strctq(struct _tuple0*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp27D;struct Cyc_Absyn_AggrInfo _tmp27C;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp27B;return(void*)((_tmp27B=_cycalloc(sizeof(*_tmp27B)),((_tmp27B[0]=((_tmp27D.tag=11,((_tmp27D.f1=((_tmp27C.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,name,0),((_tmp27C.targs=0,_tmp27C)))),_tmp27D)))),_tmp27B))));}
# 535
void*Cyc_Absyn_unionq_typ(struct _tuple0*name){
struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp283;struct Cyc_Absyn_AggrInfo _tmp282;struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp281;return(void*)((_tmp281=_cycalloc(sizeof(*_tmp281)),((_tmp281[0]=((_tmp283.tag=11,((_tmp283.f1=((_tmp282.aggr_info=Cyc_Absyn_UnknownAggr(Cyc_Absyn_UnionA,name,0),((_tmp282.targs=0,_tmp282)))),_tmp283)))),_tmp281))));}
# 539
union Cyc_Absyn_Cnst Cyc_Absyn_Null_c={.Null_c={1,0}};
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign sn,char c){struct _tuple3 _tmp286;union Cyc_Absyn_Cnst _tmp285;return((_tmp285.Char_c).val=((_tmp286.f1=sn,((_tmp286.f2=c,_tmp286)))),(((_tmp285.Char_c).tag=2,_tmp285)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Wchar_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp287;return((_tmp287.Wchar_c).val=s,(((_tmp287.Wchar_c).tag=3,_tmp287)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign sn,short s){struct _tuple4 _tmp28A;union Cyc_Absyn_Cnst _tmp289;return((_tmp289.Short_c).val=((_tmp28A.f1=sn,((_tmp28A.f2=s,_tmp28A)))),(((_tmp289.Short_c).tag=4,_tmp289)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign sn,int i){struct _tuple5 _tmp28D;union Cyc_Absyn_Cnst _tmp28C;return((_tmp28C.Int_c).val=((_tmp28D.f1=sn,((_tmp28D.f2=i,_tmp28D)))),(((_tmp28C.Int_c).tag=5,_tmp28C)));}
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign sn,long long l){struct _tuple6 _tmp290;union Cyc_Absyn_Cnst _tmp28F;return((_tmp28F.LongLong_c).val=((_tmp290.f1=sn,((_tmp290.f2=l,_tmp290)))),(((_tmp28F.LongLong_c).tag=6,_tmp28F)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr s,int i){struct _tuple7 _tmp293;union Cyc_Absyn_Cnst _tmp292;return((_tmp292.Float_c).val=((_tmp293.f1=s,((_tmp293.f2=i,_tmp293)))),(((_tmp292.Float_c).tag=7,_tmp292)));}
union Cyc_Absyn_Cnst Cyc_Absyn_String_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp294;return((_tmp294.String_c).val=s,(((_tmp294.String_c).tag=8,_tmp294)));}
union Cyc_Absyn_Cnst Cyc_Absyn_Wstring_c(struct _dyneither_ptr s){union Cyc_Absyn_Cnst _tmp295;return((_tmp295.Wstring_c).val=s,(((_tmp295.Wstring_c).tag=9,_tmp295)));}
# 550
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*r,unsigned int loc){
struct Cyc_Absyn_Exp*_tmp296;return(_tmp296=_cycalloc(sizeof(*_tmp296)),((_tmp296->topt=0,((_tmp296->r=r,((_tmp296->loc=loc,((_tmp296->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp296)))))))));}
# 553
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct _tmp299;struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp298;return Cyc_Absyn_new_exp((void*)((_tmp298=_cycalloc(sizeof(*_tmp298)),((_tmp298[0]=((_tmp299.tag=15,((_tmp299.f1=rgn_handle,((_tmp299.f2=e,_tmp299)))))),_tmp298)))),loc);}
# 556
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*_tmp29A;return(_tmp29A=_cycalloc(sizeof(*_tmp29A)),((_tmp29A[0]=*e,_tmp29A)));}
# 559
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst c,unsigned int loc){
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp29D;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp29C;return Cyc_Absyn_new_exp((void*)((_tmp29C=_cycalloc(sizeof(*_tmp29C)),((_tmp29C[0]=((_tmp29D.tag=0,((_tmp29D.f1=c,_tmp29D)))),_tmp29C)))),loc);}
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
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2A0;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp29F;return Cyc_Absyn_new_exp((void*)((_tmp29F=_cycalloc(sizeof(*_tmp29F)),((_tmp29F[0]=((_tmp2A0.tag=1,((_tmp2A0.f1=q,((_tmp2A0.f2=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp2A0)))))),_tmp29F)))),loc);}
# 600
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(struct _tuple0*q,void*b,unsigned int loc){
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2A3;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A2;return Cyc_Absyn_new_exp((void*)((_tmp2A2=_cycalloc(sizeof(*_tmp2A2)),((_tmp2A2[0]=((_tmp2A3.tag=1,((_tmp2A3.f1=q,((_tmp2A3.f2=b,_tmp2A3)))))),_tmp2A2)))),loc);}
# 604
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*q,unsigned int loc){
struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct _tmp2A6;struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp2A5;return Cyc_Absyn_new_exp((void*)((_tmp2A5=_cycalloc(sizeof(*_tmp2A5)),((_tmp2A5[0]=((_tmp2A6.tag=1,((_tmp2A6.f1=q,((_tmp2A6.f2=(void*)& Cyc_Absyn_Unresolved_b_val,_tmp2A6)))))),_tmp2A5)))),loc);}
# 607
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop p,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp2A9;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp2A8;return Cyc_Absyn_new_exp((void*)((_tmp2A8=_cycalloc(sizeof(*_tmp2A8)),((_tmp2A8[0]=((_tmp2A9.tag=2,((_tmp2A9.f1=p,((_tmp2A9.f2=es,_tmp2A9)))))),_tmp2A8)))),loc);}
# 610
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_List_List*_tmp2AA;return Cyc_Absyn_primop_exp(p,((_tmp2AA=_cycalloc(sizeof(*_tmp2AA)),((_tmp2AA->hd=e,((_tmp2AA->tl=0,_tmp2AA)))))),loc);}
# 613
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_List_List*_tmp2AD;struct Cyc_List_List*_tmp2AC;return Cyc_Absyn_primop_exp(p,((_tmp2AC=_cycalloc(sizeof(*_tmp2AC)),((_tmp2AC->hd=e1,((_tmp2AC->tl=((_tmp2AD=_cycalloc(sizeof(*_tmp2AD)),((_tmp2AD->hd=e2,((_tmp2AD->tl=0,_tmp2AD)))))),_tmp2AC)))))),loc);}
# 616
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct _tmp2B0;struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2AF;return Cyc_Absyn_new_exp((void*)((_tmp2AF=_cycalloc(sizeof(*_tmp2AF)),((_tmp2AF[0]=((_tmp2B0.tag=34,((_tmp2B0.f1=e1,((_tmp2B0.f2=e2,_tmp2B0)))))),_tmp2AF)))),loc);}
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
struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct _tmp2B3;struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp2B2;return Cyc_Absyn_new_exp((void*)((_tmp2B2=_cycalloc(sizeof(*_tmp2B2)),((_tmp2B2[0]=((_tmp2B3.tag=3,((_tmp2B3.f1=e1,((_tmp2B3.f2=popt,((_tmp2B3.f3=e2,_tmp2B3)))))))),_tmp2B2)))),loc);}
# 632
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_assignop_exp(e1,0,e2,loc);}
# 635
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i,unsigned int loc){
struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp2B6;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2B5;return Cyc_Absyn_new_exp((void*)((_tmp2B5=_cycalloc(sizeof(*_tmp2B5)),((_tmp2B5[0]=((_tmp2B6.tag=4,((_tmp2B6.f1=e,((_tmp2B6.f2=i,_tmp2B6)))))),_tmp2B5)))),loc);}
# 638
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreInc,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PreDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){return Cyc_Absyn_increment_exp(e,Cyc_Absyn_PostDec,loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,unsigned int loc){
struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp2B9;struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp2B8;return Cyc_Absyn_new_exp((void*)((_tmp2B8=_cycalloc(sizeof(*_tmp2B8)),((_tmp2B8[0]=((_tmp2B9.tag=5,((_tmp2B9.f1=e1,((_tmp2B9.f2=e2,((_tmp2B9.f3=e3,_tmp2B9)))))))),_tmp2B8)))),loc);}
# 645
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp2BC;struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp2BB;return Cyc_Absyn_new_exp((void*)((_tmp2BB=_cycalloc(sizeof(*_tmp2BB)),((_tmp2BB[0]=((_tmp2BC.tag=6,((_tmp2BC.f1=e1,((_tmp2BC.f2=e2,_tmp2BC)))))),_tmp2BB)))),loc);}
# 648
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp2BF;struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp2BE;return Cyc_Absyn_new_exp((void*)((_tmp2BE=_cycalloc(sizeof(*_tmp2BE)),((_tmp2BE[0]=((_tmp2BF.tag=7,((_tmp2BF.f1=e1,((_tmp2BF.f2=e2,_tmp2BF)))))),_tmp2BE)))),loc);}
# 651
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp2C2;struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp2C1;return Cyc_Absyn_new_exp((void*)((_tmp2C1=_cycalloc(sizeof(*_tmp2C1)),((_tmp2C1[0]=((_tmp2C2.tag=8,((_tmp2C2.f1=e1,((_tmp2C2.f2=e2,_tmp2C2)))))),_tmp2C1)))),loc);}
# 654
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp2C5;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp2C4;return Cyc_Absyn_new_exp((void*)((_tmp2C4=_cycalloc(sizeof(*_tmp2C4)),((_tmp2C4[0]=((_tmp2C5.tag=9,((_tmp2C5.f1=e,((_tmp2C5.f2=es,((_tmp2C5.f3=0,((_tmp2C5.f4=0,_tmp2C5)))))))))),_tmp2C4)))),loc);}
# 657
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmp2C8;struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp2C7;return Cyc_Absyn_new_exp((void*)((_tmp2C7=_cycalloc(sizeof(*_tmp2C7)),((_tmp2C7[0]=((_tmp2C8.tag=9,((_tmp2C8.f1=e,((_tmp2C8.f2=es,((_tmp2C8.f3=0,((_tmp2C8.f4=1,_tmp2C8)))))))))),_tmp2C7)))),loc);}
# 660
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct _tmp2CB;struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp2CA;return Cyc_Absyn_new_exp((void*)((_tmp2CA=_cycalloc(sizeof(*_tmp2CA)),((_tmp2CA[0]=((_tmp2CB.tag=11,((_tmp2CB.f1=e,_tmp2CB)))),_tmp2CA)))),loc);}
# 663
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts,unsigned int loc){
struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp2CE;struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp2CD;return Cyc_Absyn_new_exp((void*)((_tmp2CD=_cycalloc(sizeof(*_tmp2CD)),((_tmp2CD[0]=((_tmp2CE.tag=12,((_tmp2CE.f1=e,((_tmp2CE.f2=ts,_tmp2CE)))))),_tmp2CD)))),loc);}
# 666
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*t,struct Cyc_Absyn_Exp*e,int user_cast,enum Cyc_Absyn_Coercion c,unsigned int loc){
struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp2D1;struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp2D0;return Cyc_Absyn_new_exp((void*)((_tmp2D0=_cycalloc(sizeof(*_tmp2D0)),((_tmp2D0[0]=((_tmp2D1.tag=13,((_tmp2D1.f1=t,((_tmp2D1.f2=e,((_tmp2D1.f3=user_cast,((_tmp2D1.f4=c,_tmp2D1)))))))))),_tmp2D0)))),loc);}
# 669
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp2D4;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp2D3;return Cyc_Absyn_new_exp((void*)((_tmp2D3=_cycalloc(sizeof(*_tmp2D3)),((_tmp2D3[0]=((_tmp2D4.tag=10,((_tmp2D4.f1=e,_tmp2D4)))),_tmp2D3)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct _tmp2D7;struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp2D6;return Cyc_Absyn_new_exp((void*)((_tmp2D6=_cycalloc(sizeof(*_tmp2D6)),((_tmp2D6[0]=((_tmp2D7.tag=14,((_tmp2D7.f1=e,_tmp2D7)))),_tmp2D6)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int loc){
struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp2DA;struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp2D9;return Cyc_Absyn_new_exp((void*)((_tmp2D9=_cycalloc(sizeof(*_tmp2D9)),((_tmp2D9[0]=((_tmp2DA.tag=16,((_tmp2DA.f1=t,_tmp2DA)))),_tmp2D9)))),loc);}
# 674
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp2DD;struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp2DC;return Cyc_Absyn_new_exp((void*)((_tmp2DC=_cycalloc(sizeof(*_tmp2DC)),((_tmp2DC[0]=((_tmp2DD.tag=17,((_tmp2DD.f1=e,_tmp2DD)))),_tmp2DC)))),loc);}
# 677
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*t,struct Cyc_List_List*ofs,unsigned int loc){
struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp2E0;struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp2DF;return Cyc_Absyn_new_exp((void*)((_tmp2DF=_cycalloc(sizeof(*_tmp2DF)),((_tmp2DF[0]=((_tmp2E0.tag=18,((_tmp2E0.f1=t,((_tmp2E0.f2=ofs,_tmp2E0)))))),_tmp2DF)))),loc);}
# 680
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct _tmp2E3;struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp2E2;return Cyc_Absyn_new_exp((void*)((_tmp2E2=_cycalloc(sizeof(*_tmp2E2)),((_tmp2E2[0]=((_tmp2E3.tag=19,((_tmp2E3.f1=e,_tmp2E3)))),_tmp2E2)))),loc);}
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmp2E6;struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp2E5;return Cyc_Absyn_new_exp((void*)((_tmp2E5=_cycalloc(sizeof(*_tmp2E5)),((_tmp2E5[0]=((_tmp2E6.tag=20,((_tmp2E6.f1=e,((_tmp2E6.f2=n,((_tmp2E6.f3=0,((_tmp2E6.f4=0,_tmp2E6)))))))))),_tmp2E5)))),loc);}
# 684
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n,unsigned int loc){
struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmp2E9;struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp2E8;return Cyc_Absyn_new_exp((void*)((_tmp2E8=_cycalloc(sizeof(*_tmp2E8)),((_tmp2E8[0]=((_tmp2E9.tag=21,((_tmp2E9.f1=e,((_tmp2E9.f2=n,((_tmp2E9.f3=0,((_tmp2E9.f4=0,_tmp2E9)))))))))),_tmp2E8)))),loc);}
# 687
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct _tmp2EC;struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2EB;return Cyc_Absyn_new_exp((void*)((_tmp2EB=_cycalloc(sizeof(*_tmp2EB)),((_tmp2EB[0]=((_tmp2EC.tag=22,((_tmp2EC.f1=e1,((_tmp2EC.f2=e2,_tmp2EC)))))),_tmp2EB)))),loc);}
# 690
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*es,unsigned int loc){
struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct _tmp2EF;struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2EE;return Cyc_Absyn_new_exp((void*)((_tmp2EE=_cycalloc(sizeof(*_tmp2EE)),((_tmp2EE[0]=((_tmp2EF.tag=23,((_tmp2EF.f1=es,_tmp2EF)))),_tmp2EE)))),loc);}
# 693
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*s,unsigned int loc){struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct _tmp2F2;struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2F1;return Cyc_Absyn_new_exp((void*)((_tmp2F1=_cycalloc(sizeof(*_tmp2F1)),((_tmp2F1[0]=((_tmp2F2.tag=36,((_tmp2F2.f1=s,_tmp2F2)))),_tmp2F1)))),loc);}
# 695
struct Cyc_Absyn_Exp*Cyc_Absyn_match_exn_exp(unsigned int loc){
return Cyc_Absyn_var_exp(Cyc_Absyn_Match_Exception_name,loc);}
# 699
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*t,unsigned int loc){
struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp2F5;struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp2F4;return Cyc_Absyn_new_exp((void*)((_tmp2F4=_cycalloc(sizeof(*_tmp2F4)),((_tmp2F4[0]=((_tmp2F5.tag=38,((_tmp2F5.f1=t,_tmp2F5)))),_tmp2F4)))),loc);}
# 702
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int loc){
struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct _tmp2F8;struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp2F7;return Cyc_Absyn_new_exp((void*)((_tmp2F7=_cycalloc(sizeof(*_tmp2F7)),((_tmp2F7[0]=((_tmp2F8.tag=39,((_tmp2F8.f1=volatile_kw,((_tmp2F8.f2=body,_tmp2F8)))))),_tmp2F7)))),loc);}struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 706
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*es,unsigned int loc){
# 708
struct Cyc_List_List*dles=0;
for(0;es != 0;es=es->tl){
struct _tuple13*_tmp2FB;struct Cyc_List_List*_tmp2FA;dles=((_tmp2FA=_cycalloc(sizeof(*_tmp2FA)),((_tmp2FA->hd=((_tmp2FB=_cycalloc(sizeof(*_tmp2FB)),((_tmp2FB->f1=0,((_tmp2FB->f2=(struct Cyc_Absyn_Exp*)es->hd,_tmp2FB)))))),((_tmp2FA->tl=dles,_tmp2FA))))));}
dles=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);{
struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2FE;struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2FD;return Cyc_Absyn_new_exp((void*)((_tmp2FD=_cycalloc(sizeof(*_tmp2FD)),((_tmp2FD[0]=((_tmp2FE.tag=25,((_tmp2FE.f1=dles,_tmp2FE)))),_tmp2FD)))),loc);};}
# 714
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*n,struct Cyc_List_List*dles,unsigned int loc){
# 717
struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp301;struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp300;return Cyc_Absyn_new_exp((void*)((_tmp300=_cycalloc(sizeof(*_tmp300)),((_tmp300[0]=((_tmp301.tag=35,((_tmp301.f1=n,((_tmp301.f2=dles,_tmp301)))))),_tmp300)))),loc);}
# 720
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc){
struct Cyc_Absyn_Stmt*_tmp302;return(_tmp302=_cycalloc(sizeof(*_tmp302)),((_tmp302->r=s,((_tmp302->loc=loc,((_tmp302->non_local_preds=0,((_tmp302->try_depth=0,((_tmp302->annot=(void*)& Cyc_Absyn_EmptyAnnot_val,_tmp302)))))))))));}
# 724
struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct Cyc_Absyn_Skip_s_val={0};
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc){return Cyc_Absyn_new_stmt((void*)& Cyc_Absyn_Skip_s_val,loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct _tmp305;struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp304;return Cyc_Absyn_new_stmt((void*)((_tmp304=_cycalloc(sizeof(*_tmp304)),((_tmp304[0]=((_tmp305.tag=1,((_tmp305.f1=e,_tmp305)))),_tmp304)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmts(struct Cyc_List_List*ss,unsigned int loc){
if(ss == 0)return Cyc_Absyn_skip_stmt(loc);else{
if(ss->tl == 0)return(struct Cyc_Absyn_Stmt*)ss->hd;else{
return Cyc_Absyn_seq_stmt((struct Cyc_Absyn_Stmt*)ss->hd,Cyc_Absyn_seq_stmts(ss->tl,loc),loc);}}}
# 732
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct _tmp308;struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp307;return Cyc_Absyn_new_stmt((void*)((_tmp307=_cycalloc(sizeof(*_tmp307)),((_tmp307[0]=((_tmp308.tag=3,((_tmp308.f1=e,_tmp308)))),_tmp307)))),loc);}
# 735
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct _tmp30B;struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp30A;return Cyc_Absyn_new_stmt((void*)((_tmp30A=_cycalloc(sizeof(*_tmp30A)),((_tmp30A[0]=((_tmp30B.tag=4,((_tmp30B.f1=e,((_tmp30B.f2=s1,((_tmp30B.f3=s2,_tmp30B)))))))),_tmp30A)))),loc);}
# 738
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct _tmp311;struct _tuple9 _tmp310;struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp30F;return Cyc_Absyn_new_stmt((void*)((_tmp30F=_cycalloc(sizeof(*_tmp30F)),((_tmp30F[0]=((_tmp311.tag=5,((_tmp311.f1=((_tmp310.f1=e,((_tmp310.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp310)))),((_tmp311.f2=s,_tmp311)))))),_tmp30F)))),loc);}
# 741
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc){struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct _tmp314;struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp313;return Cyc_Absyn_new_stmt((void*)((_tmp313=_cycalloc(sizeof(*_tmp313)),((_tmp313[0]=((_tmp314.tag=6,((_tmp314.f1=0,_tmp314)))),_tmp313)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc){struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct _tmp317;struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp316;return Cyc_Absyn_new_stmt((void*)((_tmp316=_cycalloc(sizeof(*_tmp316)),((_tmp316[0]=((_tmp317.tag=7,((_tmp317.f1=0,_tmp317)))),_tmp316)))),loc);}
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct _tmp320;struct _tuple9 _tmp31F;struct _tuple9 _tmp31E;struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp31D;return Cyc_Absyn_new_stmt((void*)((_tmp31D=_cycalloc(sizeof(*_tmp31D)),((_tmp31D[0]=((_tmp320.tag=9,((_tmp320.f1=e1,((_tmp320.f2=((_tmp31F.f1=e2,((_tmp31F.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp31F)))),((_tmp320.f3=(
(_tmp31E.f1=e3,((_tmp31E.f2=Cyc_Absyn_skip_stmt(e3->loc),_tmp31E)))),((_tmp320.f4=s,_tmp320)))))))))),_tmp31D)))),loc);}
# 748
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,unsigned int loc){
struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct _tmp323;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp322;return Cyc_Absyn_new_stmt((void*)((_tmp322=_cycalloc(sizeof(*_tmp322)),((_tmp322[0]=((_tmp323.tag=10,((_tmp323.f1=e,((_tmp323.f2=scs,_tmp323)))))),_tmp322)))),loc);}struct _tuple14{void*f1;void*f2;};
# 751
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc){
struct _tuple14 _tmp324;struct _tuple14 _tmp10D=(_tmp324.f1=s1->r,((_tmp324.f2=s2->r,_tmp324)));struct _tuple14 _tmp10E=_tmp10D;_LL6D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp10F=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp10E.f1;if(_tmp10F->tag != 0)goto _LL6F;}_LL6E:
 return s2;_LL6F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp110=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp10E.f2;if(_tmp110->tag != 0)goto _LL71;}_LL70:
 return s1;_LL71:;_LL72: {
struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp327;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp326;return Cyc_Absyn_new_stmt((void*)((_tmp326=_cycalloc(sizeof(*_tmp326)),((_tmp326[0]=((_tmp327.tag=2,((_tmp327.f1=s1,((_tmp327.f2=s2,_tmp327)))))),_tmp326)))),loc);}_LL6C:;}
# 758
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc){
struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct _tmp32A;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp329;return Cyc_Absyn_new_stmt((void*)((_tmp329=_cycalloc(sizeof(*_tmp329)),((_tmp329[0]=((_tmp32A.tag=11,((_tmp32A.f1=el,((_tmp32A.f2=0,_tmp32A)))))),_tmp329)))),loc);}
# 761
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp32D;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp32C;return Cyc_Absyn_new_stmt((void*)((_tmp32C=_cycalloc(sizeof(*_tmp32C)),((_tmp32C[0]=((_tmp32D.tag=12,((_tmp32D.f1=d,((_tmp32D.f2=s,_tmp32D)))))),_tmp32C)))),loc);}
# 764
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct _tmp330;struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp32F;struct Cyc_Absyn_Decl*d=Cyc_Absyn_new_decl((void*)((_tmp32F=_cycalloc(sizeof(*_tmp32F)),((_tmp32F[0]=((_tmp330.tag=0,((_tmp330.f1=Cyc_Absyn_new_vardecl(x,t,init),_tmp330)))),_tmp32F)))),loc);
struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct _tmp333;struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp332;return Cyc_Absyn_new_stmt((void*)((_tmp332=_cycalloc(sizeof(*_tmp332)),((_tmp332[0]=((_tmp333.tag=12,((_tmp333.f1=d,((_tmp333.f2=s,_tmp333)))))),_tmp332)))),loc);}
# 768
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc){
struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct _tmp336;struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp335;return Cyc_Absyn_new_stmt((void*)((_tmp335=_cycalloc(sizeof(*_tmp335)),((_tmp335[0]=((_tmp336.tag=13,((_tmp336.f1=v,((_tmp336.f2=s,_tmp336)))))),_tmp335)))),loc);}
# 771
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct _tmp33C;struct _tuple9 _tmp33B;struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp33A;return Cyc_Absyn_new_stmt((void*)((_tmp33A=_cycalloc(sizeof(*_tmp33A)),((_tmp33A[0]=((_tmp33C.tag=14,((_tmp33C.f1=s,((_tmp33C.f2=((_tmp33B.f1=e,((_tmp33B.f2=Cyc_Absyn_skip_stmt(e->loc),_tmp33B)))),_tmp33C)))))),_tmp33A)))),loc);}
# 774
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*scs,unsigned int loc){
struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct _tmp33F;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp33E;return Cyc_Absyn_new_stmt((void*)((_tmp33E=_cycalloc(sizeof(*_tmp33E)),((_tmp33E[0]=((_tmp33F.tag=15,((_tmp33F.f1=s,((_tmp33F.f2=scs,_tmp33F)))))),_tmp33E)))),loc);}
# 777
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc){
struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct _tmp342;struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp341;return Cyc_Absyn_new_stmt((void*)((_tmp341=_cycalloc(sizeof(*_tmp341)),((_tmp341[0]=((_tmp342.tag=8,((_tmp342.f1=lab,((_tmp342.f2=0,_tmp342)))))),_tmp341)))),loc);}
# 780
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc){
return Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(e1,e2,loc),loc);}
# 784
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s){struct Cyc_Absyn_Pat*_tmp343;return(_tmp343=_cycalloc(sizeof(*_tmp343)),((_tmp343->r=p,((_tmp343->topt=0,((_tmp343->loc=s,_tmp343)))))));}
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct _tmp346;struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp345;return Cyc_Absyn_new_pat((void*)((_tmp345=_cycalloc(sizeof(*_tmp345)),((_tmp345[0]=((_tmp346.tag=16,((_tmp346.f1=e,_tmp346)))),_tmp345)))),e->loc);}
struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val={0};
struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val={8};
# 790
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc){struct Cyc_Absyn_Decl*_tmp347;return(_tmp347=_cycalloc(sizeof(*_tmp347)),((_tmp347->r=r,((_tmp347->loc=loc,_tmp347)))));}
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc){
struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct _tmp34A;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp349;return Cyc_Absyn_new_decl((void*)((_tmp349=_cycalloc(sizeof(*_tmp349)),((_tmp349[0]=((_tmp34A.tag=2,((_tmp34A.f1=p,((_tmp34A.f2=0,((_tmp34A.f3=e,_tmp34A)))))))),_tmp349)))),loc);}
# 794
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*vds,unsigned int loc){
struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct _tmp34D;struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp34C;return Cyc_Absyn_new_decl((void*)((_tmp34C=_cycalloc(sizeof(*_tmp34C)),((_tmp34C[0]=((_tmp34D.tag=3,((_tmp34D.f1=vds,_tmp34D)))),_tmp34C)))),loc);}
# 797
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,int resetable,struct Cyc_Absyn_Exp*eo,unsigned int loc){
struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct _tmp350;struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp34F;return Cyc_Absyn_new_decl((void*)((_tmp34F=_cycalloc(sizeof(*_tmp34F)),((_tmp34F[0]=((_tmp350.tag=4,((_tmp350.f1=tv,((_tmp350.f2=vd,((_tmp350.f3=resetable,((_tmp350.f4=eo,_tmp350)))))))))),_tmp34F)))),loc);}
# 800
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct _tmp353;struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp352;return Cyc_Absyn_new_decl((void*)((_tmp352=_cycalloc(sizeof(*_tmp352)),((_tmp352[0]=((_tmp353.tag=5,((_tmp353.f1=tv,((_tmp353.f2=vd,_tmp353)))))),_tmp352)))),loc);}
# 804
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*_tmp354;return(_tmp354=_cycalloc(sizeof(*_tmp354)),((_tmp354->sc=Cyc_Absyn_Public,((_tmp354->name=x,((_tmp354->tq=Cyc_Absyn_empty_tqual(0),((_tmp354->type=t,((_tmp354->initializer=init,((_tmp354->rgn=0,((_tmp354->attributes=0,((_tmp354->escapes=0,_tmp354)))))))))))))))));}
# 809
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init){
struct Cyc_Absyn_Vardecl*_tmp355;return(_tmp355=_cycalloc(sizeof(*_tmp355)),((_tmp355->sc=Cyc_Absyn_Static,((_tmp355->name=x,((_tmp355->tq=Cyc_Absyn_empty_tqual(0),((_tmp355->type=t,((_tmp355->initializer=init,((_tmp355->rgn=0,((_tmp355->attributes=0,((_tmp355->escapes=0,_tmp355)))))))))))))))));}
# 814
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged){
# 818
struct Cyc_Absyn_AggrdeclImpl*_tmp356;return(_tmp356=_cycalloc(sizeof(*_tmp356)),((_tmp356->exist_vars=exists,((_tmp356->rgn_po=po,((_tmp356->fields=fs,((_tmp356->tagged=tagged,_tmp356)))))))));}
# 821
struct Cyc_Absyn_Decl*Cyc_Absyn_aggr_decl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 824
struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct _tmp35C;struct Cyc_Absyn_Aggrdecl*_tmp35B;struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp35A;return Cyc_Absyn_new_decl((void*)((_tmp35A=_cycalloc(sizeof(*_tmp35A)),((_tmp35A[0]=((_tmp35C.tag=6,((_tmp35C.f1=((_tmp35B=_cycalloc(sizeof(*_tmp35B)),((_tmp35B->kind=k,((_tmp35B->sc=s,((_tmp35B->name=n,((_tmp35B->tvs=ts,((_tmp35B->impl=i,((_tmp35B->attributes=atts,_tmp35B)))))))))))))),_tmp35C)))),_tmp35A)))),loc);}
# 828
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc){
# 831
struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp366;struct Cyc_Absyn_Aggrdecl*_tmp365;struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct _tmp364;struct Cyc_Absyn_TypeDecl*_tmp363;return(_tmp363=_cycalloc(sizeof(*_tmp363)),((_tmp363->r=(void*)((_tmp366=_cycalloc(sizeof(*_tmp366)),((_tmp366[0]=((_tmp364.tag=0,((_tmp364.f1=((_tmp365=_cycalloc(sizeof(*_tmp365)),((_tmp365->kind=k,((_tmp365->sc=s,((_tmp365->name=n,((_tmp365->tvs=ts,((_tmp365->impl=i,((_tmp365->attributes=atts,_tmp365)))))))))))))),_tmp364)))),_tmp366)))),((_tmp363->loc=loc,_tmp363)))));}
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
struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct _tmp36C;struct Cyc_Absyn_Datatypedecl*_tmp36B;struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp36A;return Cyc_Absyn_new_decl((void*)((_tmp36A=_cycalloc(sizeof(*_tmp36A)),((_tmp36A[0]=((_tmp36C.tag=7,((_tmp36C.f1=((_tmp36B=_cycalloc(sizeof(*_tmp36B)),((_tmp36B->sc=s,((_tmp36B->name=n,((_tmp36B->tvs=ts,((_tmp36B->fields=fs,((_tmp36B->is_extensible=is_extensible,_tmp36B)))))))))))),_tmp36C)))),_tmp36A)))),loc);}
# 851
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc){
# 854
struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp376;struct Cyc_Absyn_Datatypedecl*_tmp375;struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct _tmp374;struct Cyc_Absyn_TypeDecl*_tmp373;return(_tmp373=_cycalloc(sizeof(*_tmp373)),((_tmp373->r=(void*)((_tmp376=_cycalloc(sizeof(*_tmp376)),((_tmp376[0]=((_tmp374.tag=2,((_tmp374.f1=((_tmp375=_cycalloc(sizeof(*_tmp375)),((_tmp375->sc=s,((_tmp375->name=n,((_tmp375->tvs=ts,((_tmp375->fields=fs,((_tmp375->is_extensible=is_extensible,_tmp375)))))))))))),_tmp374)))),_tmp376)))),((_tmp373->loc=loc,_tmp373)))));}
# 866 "absyn.cyc"
static void Cyc_Absyn_expand_arg(struct _tuple8*a){
void*_tmp144=(*a).f3;
void*_tmp145=Cyc_Absyn_pointer_expand(_tmp144,1);
if(_tmp144 != _tmp145)
(*a).f3=_tmp145;}
# 873
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts){
# 880
((void(*)(void(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Absyn_expand_arg,args);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp37C;struct Cyc_Absyn_FnInfo _tmp37B;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37A;return(void*)((_tmp37A=_cycalloc(sizeof(*_tmp37A)),((_tmp37A[0]=((_tmp37C.tag=9,((_tmp37C.f1=((_tmp37B.tvars=tvs,((_tmp37B.ret_tqual=ret_tqual,((_tmp37B.ret_typ=
# 883
Cyc_Absyn_pointer_expand(ret_typ,0),((_tmp37B.effect=eff_typ,((_tmp37B.args=args,((_tmp37B.c_varargs=c_varargs,((_tmp37B.cyc_varargs=cyc_varargs,((_tmp37B.rgn_po=rgn_po,((_tmp37B.attributes=atts,_tmp37B)))))))))))))))))),_tmp37C)))),_tmp37A))));};}
# 893
void*Cyc_Absyn_pointer_expand(void*t,int fresh_evar){
void*_tmp149=Cyc_Tcutil_compress(t);void*_tmp14A=_tmp149;_LL74: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp14B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14A;if(_tmp14B->tag != 9)goto _LL76;}_LL75: {
struct Cyc_Core_Opt*_tmp37D;return Cyc_Absyn_at_typ(t,
fresh_evar?
Cyc_Absyn_new_evar(((_tmp37D=_cycalloc(sizeof(*_tmp37D)),((_tmp37D->v=& Cyc_Tcutil_rk,_tmp37D)))),0):(void*)& Cyc_Absyn_HeapRgn_val,
# 899
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref);}_LL76:;_LL77:
 return t;_LL73:;}
# 915 "absyn.cyc"
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*e){
void*_tmp14D=e->r;void*_tmp14E=_tmp14D;struct Cyc_Absyn_Vardecl*_tmp153;struct Cyc_Absyn_Vardecl*_tmp156;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp160;_LL79: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp14F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp14F->tag != 1)goto _LL7B;else{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp150=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)((void*)_tmp14F->f2);if(_tmp150->tag != 2)goto _LL7B;}}_LL7A:
# 918
 return 0;_LL7B: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp151=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp151->tag != 1)goto _LL7D;else{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp152=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((void*)_tmp151->f2);if(_tmp152->tag != 1)goto _LL7D;else{_tmp153=_tmp152->f1;}}}_LL7C:
 _tmp156=_tmp153;goto _LL7E;_LL7D: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp154->tag != 1)goto _LL7F;else{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp155=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((void*)_tmp154->f2);if(_tmp155->tag != 4)goto _LL7F;else{_tmp156=_tmp155->f1;}}}_LL7E: {
# 921
void*_tmp161=Cyc_Tcutil_compress(_tmp156->type);void*_tmp162=_tmp161;_LL90: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp162;if(_tmp163->tag != 8)goto _LL92;}_LL91:
 return 0;_LL92:;_LL93:
 return 1;_LL8F:;}_LL7F: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp157=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp157->tag != 1)goto _LL81;}_LL80:
# 925
 goto _LL82;_LL81: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp158->tag != 21)goto _LL83;}_LL82:
 goto _LL84;_LL83: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp159->tag != 19)goto _LL85;}_LL84:
 goto _LL86;_LL85: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp15A->tag != 22)goto _LL87;}_LL86:
 return 1;_LL87: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp15B->tag != 20)goto _LL89;else{_tmp15C=_tmp15B->f1;}}_LL88:
 return Cyc_Absyn_is_lvalue(_tmp15C);_LL89: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp15D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp15D->tag != 12)goto _LL8B;else{_tmp15E=_tmp15D->f1;}}_LL8A:
 return Cyc_Absyn_is_lvalue(_tmp15E);_LL8B: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp15F=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp14E;if(_tmp15F->tag != 11)goto _LL8D;else{_tmp160=_tmp15F->f1;}}_LL8C:
 return Cyc_Absyn_is_lvalue(_tmp160);_LL8D:;_LL8E:
 return 0;_LL78:;}
# 936
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*fields,struct _dyneither_ptr*v){
{struct Cyc_List_List*_tmp164=fields;for(0;_tmp164 != 0;_tmp164=_tmp164->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Aggrfield*)_tmp164->hd)->name,v)== 0)
return(struct Cyc_Absyn_Aggrfield*)_tmp164->hd;}}
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
void*_tmp176=a;int _tmp178;int _tmp17F;struct _dyneither_ptr _tmp182;int _tmp18E;int _tmp18F;int _tmp191;int _tmp192;int _tmp194;int _tmp196;int _tmp198;struct _dyneither_ptr _tmp19B;_LL95: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp177=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp176;if(_tmp177->tag != 0)goto _LL97;else{_tmp178=_tmp177->f1;}}_LL96: {
const char*_tmp381;void*_tmp380[1];struct Cyc_Int_pa_PrintArg_struct _tmp37F;return(struct _dyneither_ptr)((_tmp37F.tag=1,((_tmp37F.f1=(unsigned long)_tmp178,((_tmp380[0]=& _tmp37F,Cyc_aprintf(((_tmp381="regparm(%d)",_tag_dyneither(_tmp381,sizeof(char),12))),_tag_dyneither(_tmp380,sizeof(void*),1))))))));}_LL97: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp179=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp176;if(_tmp179->tag != 1)goto _LL99;}_LL98: {
const char*_tmp382;return(_tmp382="stdcall",_tag_dyneither(_tmp382,sizeof(char),8));}_LL99: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp17A=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp176;if(_tmp17A->tag != 2)goto _LL9B;}_LL9A: {
const char*_tmp383;return(_tmp383="cdecl",_tag_dyneither(_tmp383,sizeof(char),6));}_LL9B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp17B=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp176;if(_tmp17B->tag != 3)goto _LL9D;}_LL9C: {
const char*_tmp384;return(_tmp384="fastcall",_tag_dyneither(_tmp384,sizeof(char),9));}_LL9D: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp17C=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp176;if(_tmp17C->tag != 4)goto _LL9F;}_LL9E: {
const char*_tmp385;return(_tmp385="noreturn",_tag_dyneither(_tmp385,sizeof(char),9));}_LL9F: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp17D=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp176;if(_tmp17D->tag != 5)goto _LLA1;}_LLA0: {
const char*_tmp386;return(_tmp386="const",_tag_dyneither(_tmp386,sizeof(char),6));}_LLA1: {struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp17E=(struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp176;if(_tmp17E->tag != 6)goto _LLA3;else{_tmp17F=_tmp17E->f1;}}_LLA2:
# 984
 if(_tmp17F == - 1){const char*_tmp387;return(_tmp387="aligned",_tag_dyneither(_tmp387,sizeof(char),8));}else{
# 986
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLC9: {
const char*_tmp38B;void*_tmp38A[1];struct Cyc_Int_pa_PrintArg_struct _tmp389;return(struct _dyneither_ptr)((_tmp389.tag=1,((_tmp389.f1=(unsigned long)_tmp17F,((_tmp38A[0]=& _tmp389,Cyc_aprintf(((_tmp38B="aligned(%d)",_tag_dyneither(_tmp38B,sizeof(char),12))),_tag_dyneither(_tmp38A,sizeof(void*),1))))))));}case Cyc_Cyclone_Vc_c: _LLCA: {
const char*_tmp38F;void*_tmp38E[1];struct Cyc_Int_pa_PrintArg_struct _tmp38D;return(struct _dyneither_ptr)((_tmp38D.tag=1,((_tmp38D.f1=(unsigned long)_tmp17F,((_tmp38E[0]=& _tmp38D,Cyc_aprintf(((_tmp38F="align(%d)",_tag_dyneither(_tmp38F,sizeof(char),10))),_tag_dyneither(_tmp38E,sizeof(void*),1))))))));}}}_LLA3: {struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*_tmp180=(struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct*)_tmp176;if(_tmp180->tag != 7)goto _LLA5;}_LLA4: {
# 990
const char*_tmp390;return(_tmp390="packed",_tag_dyneither(_tmp390,sizeof(char),7));}_LLA5: {struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp181=(struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp176;if(_tmp181->tag != 8)goto _LLA7;else{_tmp182=_tmp181->f1;}}_LLA6: {
const char*_tmp394;void*_tmp393[1];struct Cyc_String_pa_PrintArg_struct _tmp392;return(struct _dyneither_ptr)((_tmp392.tag=0,((_tmp392.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp182),((_tmp393[0]=& _tmp392,Cyc_aprintf(((_tmp394="section(\"%s\")",_tag_dyneither(_tmp394,sizeof(char),14))),_tag_dyneither(_tmp393,sizeof(void*),1))))))));}_LLA7: {struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*_tmp183=(struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct*)_tmp176;if(_tmp183->tag != 9)goto _LLA9;}_LLA8: {
const char*_tmp395;return(_tmp395="nocommon",_tag_dyneither(_tmp395,sizeof(char),9));}_LLA9: {struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*_tmp184=(struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct*)_tmp176;if(_tmp184->tag != 10)goto _LLAB;}_LLAA: {
const char*_tmp396;return(_tmp396="shared",_tag_dyneither(_tmp396,sizeof(char),7));}_LLAB: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp185=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp176;if(_tmp185->tag != 11)goto _LLAD;}_LLAC: {
const char*_tmp397;return(_tmp397="unused",_tag_dyneither(_tmp397,sizeof(char),7));}_LLAD: {struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*_tmp186=(struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct*)_tmp176;if(_tmp186->tag != 12)goto _LLAF;}_LLAE: {
const char*_tmp398;return(_tmp398="weak",_tag_dyneither(_tmp398,sizeof(char),5));}_LLAF: {struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*_tmp187=(struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct*)_tmp176;if(_tmp187->tag != 13)goto _LLB1;}_LLB0: {
const char*_tmp399;return(_tmp399="dllimport",_tag_dyneither(_tmp399,sizeof(char),10));}_LLB1: {struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*_tmp188=(struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct*)_tmp176;if(_tmp188->tag != 14)goto _LLB3;}_LLB2: {
const char*_tmp39A;return(_tmp39A="dllexport",_tag_dyneither(_tmp39A,sizeof(char),10));}_LLB3: {struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*_tmp189=(struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct*)_tmp176;if(_tmp189->tag != 15)goto _LLB5;}_LLB4: {
const char*_tmp39B;return(_tmp39B="no_instrument_function",_tag_dyneither(_tmp39B,sizeof(char),23));}_LLB5: {struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*_tmp18A=(struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct*)_tmp176;if(_tmp18A->tag != 16)goto _LLB7;}_LLB6: {
const char*_tmp39C;return(_tmp39C="constructor",_tag_dyneither(_tmp39C,sizeof(char),12));}_LLB7: {struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*_tmp18B=(struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct*)_tmp176;if(_tmp18B->tag != 17)goto _LLB9;}_LLB8: {
const char*_tmp39D;return(_tmp39D="destructor",_tag_dyneither(_tmp39D,sizeof(char),11));}_LLB9: {struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*_tmp18C=(struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct*)_tmp176;if(_tmp18C->tag != 18)goto _LLBB;}_LLBA: {
const char*_tmp39E;return(_tmp39E="no_check_memory_usage",_tag_dyneither(_tmp39E,sizeof(char),22));}_LLBB: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp18D=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp176;if(_tmp18D->tag != 19)goto _LLBD;else{if(_tmp18D->f1 != Cyc_Absyn_Printf_ft)goto _LLBD;_tmp18E=_tmp18D->f2;_tmp18F=_tmp18D->f3;}}_LLBC: {
const char*_tmp3A3;void*_tmp3A2[2];struct Cyc_Int_pa_PrintArg_struct _tmp3A1;struct Cyc_Int_pa_PrintArg_struct _tmp3A0;return(struct _dyneither_ptr)((_tmp3A0.tag=1,((_tmp3A0.f1=(unsigned int)_tmp18F,((_tmp3A1.tag=1,((_tmp3A1.f1=(unsigned int)_tmp18E,((_tmp3A2[0]=& _tmp3A1,((_tmp3A2[1]=& _tmp3A0,Cyc_aprintf(((_tmp3A3="format(printf,%u,%u)",_tag_dyneither(_tmp3A3,sizeof(char),21))),_tag_dyneither(_tmp3A2,sizeof(void*),2))))))))))))));}_LLBD: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp190=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp176;if(_tmp190->tag != 19)goto _LLBF;else{if(_tmp190->f1 != Cyc_Absyn_Scanf_ft)goto _LLBF;_tmp191=_tmp190->f2;_tmp192=_tmp190->f3;}}_LLBE: {
const char*_tmp3A8;void*_tmp3A7[2];struct Cyc_Int_pa_PrintArg_struct _tmp3A6;struct Cyc_Int_pa_PrintArg_struct _tmp3A5;return(struct _dyneither_ptr)((_tmp3A5.tag=1,((_tmp3A5.f1=(unsigned int)_tmp192,((_tmp3A6.tag=1,((_tmp3A6.f1=(unsigned int)_tmp191,((_tmp3A7[0]=& _tmp3A6,((_tmp3A7[1]=& _tmp3A5,Cyc_aprintf(((_tmp3A8="format(scanf,%u,%u)",_tag_dyneither(_tmp3A8,sizeof(char),20))),_tag_dyneither(_tmp3A7,sizeof(void*),2))))))))))))));}_LLBF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp193=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp176;if(_tmp193->tag != 20)goto _LLC1;else{_tmp194=_tmp193->f1;}}_LLC0: {
const char*_tmp3AC;void*_tmp3AB[1];struct Cyc_Int_pa_PrintArg_struct _tmp3AA;return(struct _dyneither_ptr)((_tmp3AA.tag=1,((_tmp3AA.f1=(unsigned long)_tmp194,((_tmp3AB[0]=& _tmp3AA,Cyc_aprintf(((_tmp3AC="initializes(%d)",_tag_dyneither(_tmp3AC,sizeof(char),16))),_tag_dyneither(_tmp3AB,sizeof(void*),1))))))));}_LLC1: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp195=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp176;if(_tmp195->tag != 21)goto _LLC3;else{_tmp196=_tmp195->f1;}}_LLC2: {
const char*_tmp3B0;void*_tmp3AF[1];struct Cyc_Int_pa_PrintArg_struct _tmp3AE;return(struct _dyneither_ptr)((_tmp3AE.tag=1,((_tmp3AE.f1=(unsigned long)_tmp196,((_tmp3AF[0]=& _tmp3AE,Cyc_aprintf(((_tmp3B0="noliveunique(%d)",_tag_dyneither(_tmp3B0,sizeof(char),17))),_tag_dyneither(_tmp3AF,sizeof(void*),1))))))));}_LLC3: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp197=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp176;if(_tmp197->tag != 22)goto _LLC5;else{_tmp198=_tmp197->f1;}}_LLC4: {
const char*_tmp3B4;void*_tmp3B3[1];struct Cyc_Int_pa_PrintArg_struct _tmp3B2;return(struct _dyneither_ptr)((_tmp3B2.tag=1,((_tmp3B2.f1=(unsigned long)_tmp198,((_tmp3B3[0]=& _tmp3B2,Cyc_aprintf(((_tmp3B4="noconsume(%d)",_tag_dyneither(_tmp3B4,sizeof(char),14))),_tag_dyneither(_tmp3B3,sizeof(void*),1))))))));}_LLC5: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp199=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp176;if(_tmp199->tag != 23)goto _LLC7;}_LLC6: {
const char*_tmp3B5;return(_tmp3B5="pure",_tag_dyneither(_tmp3B5,sizeof(char),5));}_LLC7: {struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp19A=(struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*)_tmp176;if(_tmp19A->tag != 24)goto _LL94;else{_tmp19B=_tmp19A->f1;}}_LLC8: {
const char*_tmp3B9;void*_tmp3B8[1];struct Cyc_String_pa_PrintArg_struct _tmp3B7;return(struct _dyneither_ptr)((_tmp3B7.tag=0,((_tmp3B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp19B),((_tmp3B8[0]=& _tmp3B7,Cyc_aprintf(((_tmp3B9="__mode__(\"%s\")",_tag_dyneither(_tmp3B9,sizeof(char),15))),_tag_dyneither(_tmp3B8,sizeof(void*),1))))))));}_LL94:;}
# 1013
int Cyc_Absyn_fntype_att(void*a){
void*_tmp1CE=a;int _tmp1D0;_LLCD: {struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp1CF=(struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1CF->tag != 0)goto _LLCF;else{_tmp1D0=_tmp1CF->f1;}}_LLCE:
 goto _LLD0;_LLCF: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp1D1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D1->tag != 3)goto _LLD1;}_LLD0:
 goto _LLD2;_LLD1: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp1D2=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D2->tag != 1)goto _LLD3;}_LLD2:
 goto _LLD4;_LLD3: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp1D3=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D3->tag != 2)goto _LLD5;}_LLD4:
 goto _LLD6;_LLD5: {struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*_tmp1D4=(struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D4->tag != 4)goto _LLD7;}_LLD6:
 goto _LLD8;_LLD7: {struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*_tmp1D5=(struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D5->tag != 23)goto _LLD9;}_LLD8:
 goto _LLDA;_LLD9: {struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp1D6=(struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D6->tag != 19)goto _LLDB;}_LLDA:
 goto _LLDC;_LLDB: {struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*_tmp1D7=(struct Cyc_Absyn_Const_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D7->tag != 5)goto _LLDD;}_LLDC:
 goto _LLDE;_LLDD: {struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp1D8=(struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D8->tag != 21)goto _LLDF;}_LLDE:
 goto _LLE0;_LLDF: {struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp1D9=(struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1D9->tag != 20)goto _LLE1;}_LLE0:
 goto _LLE2;_LLE1: {struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp1DA=(struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmp1CE;if(_tmp1DA->tag != 22)goto _LLE3;}_LLE2:
 return 1;_LLE3:;_LLE4:
 return 0;_LLCC:;}static char _tmp1DB[3]="f0";
# 1031
static struct _dyneither_ptr Cyc_Absyn_f0={_tmp1DB,_tmp1DB,_tmp1DB + 3};
static struct _dyneither_ptr*Cyc_Absyn_field_names_v[1]={& Cyc_Absyn_f0};
static struct _dyneither_ptr Cyc_Absyn_field_names={(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v),(void*)((struct _dyneither_ptr**)Cyc_Absyn_field_names_v + 1)};struct _dyneither_ptr*Cyc_Absyn_fieldname(int i);static void _tmp3C7(unsigned int*fsz,unsigned int*_tmp3C6,unsigned int*_tmp3C5,struct _dyneither_ptr***_tmp3C3){for(*_tmp3C6=0;*_tmp3C6 < *_tmp3C5;(*_tmp3C6)++){struct Cyc_Int_pa_PrintArg_struct _tmp3C1;void*_tmp3C0[1];const char*_tmp3BF;struct _dyneither_ptr*_tmp3BE;(*_tmp3C3)[*_tmp3C6]=(struct _dyneither_ptr*)(
# 1039
*_tmp3C6 < *fsz?*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),(int)*_tmp3C6)):((_tmp3BE=_cycalloc(sizeof(*_tmp3BE)),((_tmp3BE[0]=(struct _dyneither_ptr)((_tmp3C1.tag=1,((_tmp3C1.f1=(unsigned long)((int)*_tmp3C6),((_tmp3C0[0]=& _tmp3C1,Cyc_aprintf(((_tmp3BF="f%d",_tag_dyneither(_tmp3BF,sizeof(char),4))),_tag_dyneither(_tmp3C0,sizeof(void*),1)))))))),_tmp3BE)))));}}
# 1034
struct _dyneither_ptr*Cyc_Absyn_fieldname(int i){
unsigned int fsz=_get_dyneither_size(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*));
if(i >= fsz){
unsigned int _tmp3C6;unsigned int _tmp3C5;struct _dyneither_ptr _tmp3C4;struct _dyneither_ptr**_tmp3C3;unsigned int _tmp3C2;Cyc_Absyn_field_names=(
(_tmp3C2=(unsigned int)(i + 1),((_tmp3C3=(struct _dyneither_ptr**)_cycalloc(_check_times(sizeof(struct _dyneither_ptr*),_tmp3C2)),((_tmp3C4=_tag_dyneither(_tmp3C3,sizeof(struct _dyneither_ptr*),_tmp3C2),((((_tmp3C5=_tmp3C2,_tmp3C7(& fsz,& _tmp3C6,& _tmp3C5,& _tmp3C3))),_tmp3C4))))))));}
# 1040
return*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Absyn_field_names,sizeof(struct _dyneither_ptr*),i));}
# 1043
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1E4=info;enum Cyc_Absyn_AggrKind _tmp1E5;struct _tuple0*_tmp1E6;enum Cyc_Absyn_AggrKind _tmp1E7;struct _tuple0*_tmp1E8;_LLE6: if((_tmp1E4.UnknownAggr).tag != 1)goto _LLE8;_tmp1E5=((struct _tuple2)(_tmp1E4.UnknownAggr).val).f1;_tmp1E6=((struct _tuple2)(_tmp1E4.UnknownAggr).val).f2;_LLE7: {
struct _tuple11 _tmp3C8;return(_tmp3C8.f1=_tmp1E5,((_tmp3C8.f2=_tmp1E6,_tmp3C8)));}_LLE8: if((_tmp1E4.KnownAggr).tag != 2)goto _LLE5;_tmp1E7=(*((struct Cyc_Absyn_Aggrdecl**)(_tmp1E4.KnownAggr).val))->kind;_tmp1E8=(*((struct Cyc_Absyn_Aggrdecl**)(_tmp1E4.KnownAggr).val))->name;_LLE9: {
struct _tuple11 _tmp3C9;return(_tmp3C9.f1=_tmp1E7,((_tmp3C9.f2=_tmp1E8,_tmp3C9)));}_LLE5:;}
# 1049
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info){
union Cyc_Absyn_AggrInfoU _tmp1EB=info;struct Cyc_Absyn_Aggrdecl*_tmp1EC;_LLEB: if((_tmp1EB.UnknownAggr).tag != 1)goto _LLED;_LLEC: {
const char*_tmp3CC;void*_tmp3CB;(_tmp3CB=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp3CC="unchecked aggrdecl",_tag_dyneither(_tmp3CC,sizeof(char),19))),_tag_dyneither(_tmp3CB,sizeof(void*),0)));}_LLED: if((_tmp1EB.KnownAggr).tag != 2)goto _LLEA;_tmp1EC=*((struct Cyc_Absyn_Aggrdecl**)(_tmp1EB.KnownAggr).val);_LLEE:
 return _tmp1EC;_LLEA:;}
# 1055
int Cyc_Absyn_is_union_type(void*t){
void*_tmp1EF=Cyc_Tcutil_compress(t);void*_tmp1F0=_tmp1EF;union Cyc_Absyn_AggrInfoU _tmp1F3;_LLF0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1F1=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F0;if(_tmp1F1->tag != 12)goto _LLF2;else{if(_tmp1F1->f1 != Cyc_Absyn_UnionA)goto _LLF2;}}_LLF1:
 return 1;_LLF2: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1F2=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F0;if(_tmp1F2->tag != 11)goto _LLF4;else{_tmp1F3=(_tmp1F2->f1).aggr_info;}}_LLF3:
 return(Cyc_Absyn_aggr_kinded_name(_tmp1F3)).f1 == Cyc_Absyn_UnionA;_LLF4:;_LLF5:
 return 0;_LLEF:;}
# 1062
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*t){
void*_tmp1F4=Cyc_Tcutil_compress(t);void*_tmp1F5=_tmp1F4;struct Cyc_List_List*_tmp1F7;union Cyc_Absyn_AggrInfoU _tmp1F9;_LLF7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp1F6=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F6->tag != 12)goto _LLF9;else{if(_tmp1F6->f1 != Cyc_Absyn_UnionA)goto _LLF9;_tmp1F7=_tmp1F6->f2;}}_LLF8:
# 1065
 if(_tmp1F7 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp1F7->hd)->requires_clause == 0;_LLF9: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp1F8=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1F5;if(_tmp1F8->tag != 11)goto _LLFB;else{_tmp1F9=(_tmp1F8->f1).aggr_info;}}_LLFA: {
# 1068
union Cyc_Absyn_AggrInfoU _tmp1FA=_tmp1F9;struct Cyc_Absyn_Aggrdecl*_tmp1FB;enum Cyc_Absyn_AggrKind _tmp1FC;enum Cyc_Absyn_AggrKind _tmp1FD;int _tmp1FE;_LLFE: if((_tmp1FA.KnownAggr).tag != 2)goto _LL100;_tmp1FB=*((struct Cyc_Absyn_Aggrdecl**)(_tmp1FA.KnownAggr).val);_LLFF:
# 1070
 if(_tmp1FB->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp1FF=_tmp1FB->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1FF))->tagged)return 0;{
struct Cyc_List_List*_tmp200=_tmp1FF->fields;
if(_tmp200 == 0)return 1;
return((struct Cyc_Absyn_Aggrfield*)_tmp200->hd)->requires_clause == 0;};};_LL100: if((_tmp1FA.UnknownAggr).tag != 1)goto _LL102;_tmp1FC=((struct _tuple2)(_tmp1FA.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3 != 0)goto _LL102;_LL101:
 return _tmp1FC == Cyc_Absyn_UnionA;_LL102: if((_tmp1FA.UnknownAggr).tag != 1)goto _LLFD;_tmp1FD=((struct _tuple2)(_tmp1FA.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3 == 0)goto _LLFD;_tmp1FE=(int)(((struct _tuple2)(_tmp1FA.UnknownAggr).val).f3)->v;_LL103:
 return _tmp1FD == Cyc_Absyn_UnionA  && !_tmp1FE;_LLFD:;}_LLFB:;_LLFC:
# 1079
 return 0;_LLF6:;}
# 1082
int Cyc_Absyn_is_require_union_type(void*t){
void*_tmp201=Cyc_Tcutil_compress(t);void*_tmp202=_tmp201;struct Cyc_List_List*_tmp204;union Cyc_Absyn_AggrInfoU _tmp206;_LL105: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp203=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp202;if(_tmp203->tag != 12)goto _LL107;else{if(_tmp203->f1 != Cyc_Absyn_UnionA)goto _LL107;_tmp204=_tmp203->f2;}}_LL106:
# 1085
 if(_tmp204 == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp204->hd)->requires_clause != 0;_LL107: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp205=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp202;if(_tmp205->tag != 11)goto _LL109;else{_tmp206=(_tmp205->f1).aggr_info;}}_LL108: {
# 1088
union Cyc_Absyn_AggrInfoU _tmp207=_tmp206;struct Cyc_Absyn_Aggrdecl*_tmp208;enum Cyc_Absyn_AggrKind _tmp209;enum Cyc_Absyn_AggrKind _tmp20A;int _tmp20B;_LL10C: if((_tmp207.KnownAggr).tag != 2)goto _LL10E;_tmp208=*((struct Cyc_Absyn_Aggrdecl**)(_tmp207.KnownAggr).val);_LL10D:
# 1090
 if(_tmp208->kind != Cyc_Absyn_UnionA)return 0;{
struct Cyc_Absyn_AggrdeclImpl*_tmp20C=_tmp208->impl;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp20C))->tagged)return 0;{
struct Cyc_List_List*_tmp20D=_tmp20C->fields;
if(_tmp20D == 0)return 0;
return((struct Cyc_Absyn_Aggrfield*)_tmp20D->hd)->requires_clause != 0;};};_LL10E: if((_tmp207.UnknownAggr).tag != 1)goto _LL110;_tmp209=((struct _tuple2)(_tmp207.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp207.UnknownAggr).val).f3 != 0)goto _LL110;_LL10F:
 return 0;_LL110: if((_tmp207.UnknownAggr).tag != 1)goto _LL10B;_tmp20A=((struct _tuple2)(_tmp207.UnknownAggr).val).f1;if(((struct _tuple2)(_tmp207.UnknownAggr).val).f3 == 0)goto _LL10B;_tmp20B=(int)(((struct _tuple2)(_tmp207.UnknownAggr).val).f3)->v;_LL111:
 return 0;_LL10B:;}_LL109:;_LL10A:
# 1099
 return 0;_LL104:;}
# 1102
int Cyc_Absyn_is_aggr_type(void*t){
void*_tmp20E=Cyc_Tcutil_compress(t);void*_tmp20F=_tmp20E;_LL113: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp210=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp20F;if(_tmp210->tag != 3)goto _LL115;}_LL114:
 goto _LL116;_LL115: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp211=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp20F;if(_tmp211->tag != 4)goto _LL117;}_LL116:
 goto _LL118;_LL117: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp212=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp20F;if(_tmp212->tag != 10)goto _LL119;}_LL118:
 goto _LL11A;_LL119: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp213=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp20F;if(_tmp213->tag != 11)goto _LL11B;}_LL11A:
 goto _LL11C;_LL11B: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp214=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp20F;if(_tmp214->tag != 12)goto _LL11D;}_LL11C:
 return 1;_LL11D:;_LL11E:
 return 0;_LL112:;}
# 1113
struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val={14};
struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val={15};
# 1123 "absyn.cyc"
int Cyc_Absyn_porting_c_code=0;
int Cyc_Absyn_no_regions=0;
