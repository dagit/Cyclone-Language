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
typedef struct Cyc___cycFILE Cyc_FILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_Core_Open_Region[12];struct Cyc_Core_Open_Region_exn_struct{char*tag;};extern char Cyc_Core_Free_Region[12];struct Cyc_Core_Free_Region_exn_struct{char*tag;};
# 244 "core.h"
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 249
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 256
return 0;
return dyn.curr;};}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_Numelts_ptrqual_Absyn_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Region_ptrqual_Absyn_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Absyn_Thin_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Fat_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Zeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nozeroterm_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Notnull_ptrqual_Absyn_Pointer_qual_struct{int tag;};struct Cyc_Absyn_Nullable_ptrqual_Absyn_Pointer_qual_struct{int tag;};
# 280
typedef void*Cyc_Absyn_pointer_qual_t;
typedef struct Cyc_List_List*Cyc_Absyn_pointer_quals_t;struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 373
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 378
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};
# 444 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};
# 481
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 489
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Tvar*f2;struct Cyc_Absyn_Vardecl*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 915
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 925
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 933
void*Cyc_Absyn_compress_kb(void*);
# 938
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 968
extern void*Cyc_Absyn_bounds_one;
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1152
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1158
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 56 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 79
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 87
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 38 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 65
void*Cyc_Tcutil_compress(void*t);
# 338 "tcutil.h"
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 35 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 46
static int Cyc_Absyndump_expand_typedefs;
# 50
static int Cyc_Absyndump_qvar_to_Cids;
# 53
static int Cyc_Absyndump_add_cyc_prefix;
# 56
static int Cyc_Absyndump_generate_line_directives;
# 60
static int Cyc_Absyndump_to_VC;
# 62
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 77 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 80
void Cyc_Absyndump_dumptyp(void*);
void Cyc_Absyndump_dumpntyp(void*t);
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int);
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 95
typedef struct _dyneither_ptr Cyc_Absyndump_dump_string_t;
void Cyc_Absyndump_dumploc(unsigned int);
# 99
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 101
void Cyc_Absyndump_ignore(void*x){return;}
# 103
static unsigned int Cyc_Absyndump_pos=0;
static char Cyc_Absyndump_prev_char='x';
# 106
int Cyc_Absyndump_need_space_before(){
switch(Cyc_Absyndump_prev_char){case '{': _LL0:
 goto _LL1;case '}': _LL1:
 goto _LL2;case '(': _LL2:
 goto _LL3;case ')': _LL3:
 goto _LL4;case '[': _LL4:
 goto _LL5;case ']': _LL5:
 goto _LL6;case ';': _LL6:
 goto _LL7;case ',': _LL7:
 goto _LL8;case '=': _LL8:
 goto _LL9;case '?': _LL9:
 goto _LLA;case '!': _LLA:
 goto _LLB;case ' ': _LLB:
 goto _LLC;case '\n': _LLC:
 goto _LLD;case '*': _LLD:
 return 0;default: _LLE:
 return 1;}}
# 126
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 133
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 141
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 144
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 149
void Cyc_Absyndump_dump_char(int c){
# 151
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpC[1]="";
# 155
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpC,_tmpC,_tmpC + 1};
static unsigned int last_line=0;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 161
struct _dyneither_ptr _tmp1;unsigned int _tmp2;struct _tuple13 _tmp0=Cyc_Lex_xlate_pos(loc);_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
if((char*)_tmp1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp1.curr == (char*)last_file.curr  && _tmp2 == last_line)return;
if((char*)_tmp1.curr == (char*)last_file.curr  && _tmp2 == last_line + 1){
const char*_tmp3A1;void*_tmp3A0;(_tmp3A0=0,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3A1="\n",_tag_dyneither(_tmp3A1,sizeof(char),2))),_tag_dyneither(_tmp3A0,sizeof(void*),0)));}else{
if((char*)_tmp1.curr == (char*)last_file.curr){
const char*_tmp3A5;void*_tmp3A4[1];struct Cyc_Int_pa_PrintArg_struct _tmp3A3;(_tmp3A3.tag=1,((_tmp3A3.f1=(unsigned long)((int)_tmp2),((_tmp3A4[0]=& _tmp3A3,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3A5="\n# %d\n",_tag_dyneither(_tmp3A5,sizeof(char),7))),_tag_dyneither(_tmp3A4,sizeof(void*),1)))))));}else{
# 168
const char*_tmp3AA;void*_tmp3A9[2];struct Cyc_Int_pa_PrintArg_struct _tmp3A8;struct Cyc_String_pa_PrintArg_struct _tmp3A7;(_tmp3A7.tag=0,((_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1),((_tmp3A8.tag=1,((_tmp3A8.f1=(unsigned long)((int)_tmp2),((_tmp3A9[0]=& _tmp3A8,((_tmp3A9[1]=& _tmp3A7,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3AA="\n# %d %s\n",_tag_dyneither(_tmp3AA,sizeof(char),10))),_tag_dyneither(_tmp3A9,sizeof(void*),2)))))))))))));}}
# 170
last_file=_tmp1;
last_line=_tmp2;};}
# 176
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 180
void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 184
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f((void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 191
f((void*)l->hd);}
# 193
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(env,(void*)l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 200
f(env,(void*)l->hd);}
# 202
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 204
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 208
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 210
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 214
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 216
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 222
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpD=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmpE=(*v).f1;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;_LL11: if((_tmpE.Loc_n).tag != 4)goto _LL13;_LL12:
 _tmpF=0;goto _LL14;_LL13: if((_tmpE.Rel_n).tag != 1)goto _LL15;_tmpF=(struct Cyc_List_List*)(_tmpE.Rel_n).val;_LL14:
# 228
 _tmpD=_tmpF;
goto _LL10;_LL15: if((_tmpE.C_n).tag != 3)goto _LL17;_tmp10=(struct Cyc_List_List*)(_tmpE.C_n).val;_LL16:
# 234
 Cyc_Absyndump_dump_char((int)' ');
goto _LL10;_LL17: if((_tmpE.Abs_n).tag != 2)goto _LL10;_tmp11=(struct Cyc_List_List*)(_tmpE.Abs_n).val;_LL18:
# 237
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=(struct _dyneither_ptr**)& Cyc_Absynpp_cyc_stringptr;
_tmpD=_tmp11;
goto _LL10;_LL10:;}
# 242
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3AB;Cyc_Absyndump_dump_nospace(((_tmp3AB="::",_tag_dyneither(_tmp3AB,sizeof(char),3))));}}
# 247
if(_tmpD != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpD->hd));
for(_tmpD=_tmpD->tl;_tmpD != 0;_tmpD=_tmpD->tl){
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3AC;Cyc_Absyndump_dump_nospace(((_tmp3AC="::",_tag_dyneither(_tmp3AC,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpD->hd));}
# 256
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp3AD;Cyc_Absyndump_dump_nospace(((_tmp3AD="_",_tag_dyneither(_tmp3AD,sizeof(char),2))));}else{
const char*_tmp3AE;Cyc_Absyndump_dump_nospace(((_tmp3AE="::",_tag_dyneither(_tmp3AE,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 260
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 263
Cyc_Absyndump_dump_str((*v).f2);}}}
# 267
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict){const char*_tmp3AF;Cyc_Absyndump_dump(((_tmp3AF="restrict",_tag_dyneither(_tmp3AF,sizeof(char),9))));}
if(tq.q_volatile){const char*_tmp3B0;Cyc_Absyndump_dump(((_tmp3B0="volatile",_tag_dyneither(_tmp3B0,sizeof(char),9))));}
if(tq.print_const){const char*_tmp3B1;Cyc_Absyndump_dump(((_tmp3B1="const",_tag_dyneither(_tmp3B1,sizeof(char),6))));}}
# 273
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
switch(sc){case Cyc_Absyn_Static: _LL19:
{const char*_tmp3B2;Cyc_Absyndump_dump(((_tmp3B2="static",_tag_dyneither(_tmp3B2,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
 return;case Cyc_Absyn_Extern: _LL1B:
{const char*_tmp3B3;Cyc_Absyndump_dump(((_tmp3B3="extern",_tag_dyneither(_tmp3B3,sizeof(char),7))));}return;case Cyc_Absyn_ExternC: _LL1C:
{const char*_tmp3B4;Cyc_Absyndump_dump(((_tmp3B4="extern \"C\"",_tag_dyneither(_tmp3B4,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:
{const char*_tmp3B5;Cyc_Absyndump_dump(((_tmp3B5="abstract",_tag_dyneither(_tmp3B5,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:
{const char*_tmp3B6;Cyc_Absyndump_dump(((_tmp3B6="register",_tag_dyneither(_tmp3B6,sizeof(char),9))));}return;}}
# 284
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp1F;enum Cyc_Absyn_AliasQual _tmp20;struct Cyc_Absyn_Kind*_tmp1E=ka;_tmp1F=(*_tmp1E).kind;_tmp20=(*_tmp1E).aliasqual;
switch(_tmp1F){case Cyc_Absyn_AnyKind: _LL20:
# 288
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL22:
{const char*_tmp3B7;Cyc_Absyndump_dump(((_tmp3B7="A",_tag_dyneither(_tmp3B7,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL23:
{const char*_tmp3B8;Cyc_Absyndump_dump(((_tmp3B8="UA",_tag_dyneither(_tmp3B8,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:
{const char*_tmp3B9;Cyc_Absyndump_dump(((_tmp3B9="TA",_tag_dyneither(_tmp3B9,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL21:
# 294
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL27:
{const char*_tmp3BA;Cyc_Absyndump_dump(((_tmp3BA="M",_tag_dyneither(_tmp3BA,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:
{const char*_tmp3BB;Cyc_Absyndump_dump(((_tmp3BB="UM",_tag_dyneither(_tmp3BB,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL29:
{const char*_tmp3BC;Cyc_Absyndump_dump(((_tmp3BC="TM",_tag_dyneither(_tmp3BC,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26:
# 300
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL2C:
{const char*_tmp3BD;Cyc_Absyndump_dump(((_tmp3BD="B",_tag_dyneither(_tmp3BD,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:
{const char*_tmp3BE;Cyc_Absyndump_dump(((_tmp3BE="UB",_tag_dyneither(_tmp3BE,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:
{const char*_tmp3BF;Cyc_Absyndump_dump(((_tmp3BF="TB",_tag_dyneither(_tmp3BF,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind: _LL2B:
# 306
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL31:
{const char*_tmp3C0;Cyc_Absyndump_dump(((_tmp3C0="R",_tag_dyneither(_tmp3C0,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL32:
{const char*_tmp3C1;Cyc_Absyndump_dump(((_tmp3C1="UR",_tag_dyneither(_tmp3C1,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:
{const char*_tmp3C2;Cyc_Absyndump_dump(((_tmp3C2="TR",_tag_dyneither(_tmp3C2,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL30:
# 311
{const char*_tmp3C3;Cyc_Absyndump_dump(((_tmp3C3="E",_tag_dyneither(_tmp3C3,sizeof(char),2))));}return;case Cyc_Absyn_IntKind: _LL35:
{const char*_tmp3C4;Cyc_Absyndump_dump(((_tmp3C4="I",_tag_dyneither(_tmp3C4,sizeof(char),2))));}return;}}
# 316
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LL37:
{const char*_tmp3C5;Cyc_Absyndump_dump(((_tmp3C5="struct ",_tag_dyneither(_tmp3C5,sizeof(char),8))));}return;case Cyc_Absyn_UnionA: _LL38:
{const char*_tmp3C6;Cyc_Absyndump_dump(((_tmp3C6="union ",_tag_dyneither(_tmp3C6,sizeof(char),7))));}return;}}
# 323
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
const char*_tmp3C9;const char*_tmp3C8;const char*_tmp3C7;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((_tmp3C7="<",_tag_dyneither(_tmp3C7,sizeof(char),2))),((_tmp3C8=">",_tag_dyneither(_tmp3C8,sizeof(char),2))),((_tmp3C9=",",_tag_dyneither(_tmp3C9,sizeof(char),2))));}
# 326
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
{const char*_tmp3CA;Cyc_Absyndump_dump(((_tmp3CA="`TTT",_tag_dyneither(_tmp3CA,sizeof(char),5))));}
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 332
Cyc_Absyndump_dump(n);}}
# 334
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp35=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp38;struct Cyc_Absyn_Kind*_tmp3B;_LL3B: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp36=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp36->tag != 1)goto _LL3D;}_LL3C:
 goto _LL3A;_LL3D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp37=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp37->tag != 2)goto _LL3F;else{_tmp38=_tmp37->f2;}}_LL3E:
 goto _LL3A;_LL3F: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp39->tag != 0)goto _LL41;else{if((*_tmp39->f1).kind != Cyc_Absyn_BoxKind)goto _LL41;if((*_tmp39->f1).aliasqual != Cyc_Absyn_Aliasable)goto _LL41;}}_LL40:
 goto _LL3A;_LL41: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp3A->tag != 0)goto _LL3A;else{_tmp3B=_tmp3A->f1;}}_LL42:
{const char*_tmp3CB;Cyc_Absyndump_dump(((_tmp3CB="::",_tag_dyneither(_tmp3CB,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp3B);goto _LL3A;_LL3A:;};}
# 343
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
const char*_tmp3CE;const char*_tmp3CD;const char*_tmp3CC;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp3CC="<",_tag_dyneither(_tmp3CC,sizeof(char),2))),((_tmp3CD=">",_tag_dyneither(_tmp3CD,sizeof(char),2))),((_tmp3CE=",",_tag_dyneither(_tmp3CE,sizeof(char),2))));}
# 346
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
const char*_tmp3D1;const char*_tmp3D0;const char*_tmp3CF;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((_tmp3CF="<",_tag_dyneither(_tmp3CF,sizeof(char),2))),((_tmp3D0=">",_tag_dyneither(_tmp3D0,sizeof(char),2))),((_tmp3D1=",",_tag_dyneither(_tmp3D1,sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 350
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 353
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
const char*_tmp3D4;const char*_tmp3D3;const char*_tmp3D2;((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,((_tmp3D2="(",_tag_dyneither(_tmp3D2,sizeof(char),2))),((_tmp3D3=")",_tag_dyneither(_tmp3D3,sizeof(char),2))),((_tmp3D4=",",_tag_dyneither(_tmp3D4,sizeof(char),2))));}
# 357
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp46=(void*)atts->hd;_LL44: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp47=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp47->tag != 1)goto _LL46;}_LL45:
{const char*_tmp3D5;Cyc_Absyndump_dump(((_tmp3D5="_stdcall",_tag_dyneither(_tmp3D5,sizeof(char),9))));}return;_LL46: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp48=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp46;if(_tmp48->tag != 2)goto _LL48;}_LL47:
{const char*_tmp3D6;Cyc_Absyndump_dump(((_tmp3D6="_cdecl",_tag_dyneither(_tmp3D6,sizeof(char),7))));}return;_LL48: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp49=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp46;if(_tmp49->tag != 3)goto _LL4A;}_LL49:
{const char*_tmp3D7;Cyc_Absyndump_dump(((_tmp3D7="_fastcall",_tag_dyneither(_tmp3D7,sizeof(char),10))));}return;_LL4A:;_LL4B:
 goto _LL43;_LL43:;}}
# 367
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 369
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp4D=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;_LL4D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp4E=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp4D;if(_tmp4E->tag != 1)goto _LL4F;}_LL4E:
 goto _LL50;_LL4F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp4F=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp4D;if(_tmp4F->tag != 2)goto _LL51;}_LL50:
 goto _LL52;_LL51: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp50=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp4D;if(_tmp50->tag != 3)goto _LL53;}_LL52:
 goto _LL4C;_LL53:;_LL54:
 hasatt=1;goto _LL4C;_LL4C:;}}
# 377
if(!hasatt)
return;
{const char*_tmp3D8;Cyc_Absyndump_dump(((_tmp3D8="__declspec(",_tag_dyneither(_tmp3D8,sizeof(char),12))));}
for(0;atts != 0;atts=atts->tl){
void*_tmp52=(void*)atts->hd;_LL56: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp53=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp52;if(_tmp53->tag != 1)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp54=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp52;if(_tmp54->tag != 2)goto _LL5A;}_LL59:
 goto _LL5B;_LL5A: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp55=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp52;if(_tmp55->tag != 3)goto _LL5C;}_LL5B:
 goto _LL55;_LL5C:;_LL5D:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL55;_LL55:;}
# 387
Cyc_Absyndump_dump_char((int)')');}
# 390
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL5E:
# 394
{const char*_tmp3D9;Cyc_Absyndump_dump(((_tmp3D9=" __attribute__((",_tag_dyneither(_tmp3D9,sizeof(char),17))));}
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0){const char*_tmp3DA;Cyc_Absyndump_dump(((_tmp3DA=",",_tag_dyneither(_tmp3DA,sizeof(char),2))));}}
# 399
{const char*_tmp3DB;Cyc_Absyndump_dump(((_tmp3DB=")) ",_tag_dyneither(_tmp3DB,sizeof(char),4))));}
return;case Cyc_Cyclone_Vc_c: _LL5F:
# 402
 Cyc_Absyndump_dump_noncallconv(atts);
return;}}
# 407
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmp59=(void*)tms->hd;_LL62: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp5A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp59;if(_tmp5A->tag != 2)goto _LL64;}_LL63:
 return 1;_LL64:;_LL65:
 return 0;_LL61:;};}
# 415
static void Cyc_Absyndump_dumprgn(void*t){
void*_tmp5B=Cyc_Tcutil_compress(t);_LL67: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp5C=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp5B;if(_tmp5C->tag != 20)goto _LL69;}_LL68:
{const char*_tmp3DC;Cyc_Absyndump_dump(((_tmp3DC="`H",_tag_dyneither(_tmp3DC,sizeof(char),3))));}goto _LL66;_LL69:;_LL6A:
 Cyc_Absyndump_dumpntyp(t);goto _LL66;_LL66:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 422
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 426
{void*_tmp5E=Cyc_Tcutil_compress(t);void*_tmp60;struct Cyc_List_List*_tmp62;_LL6C: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp5F=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp5E;if(_tmp5F->tag != 23)goto _LL6E;else{_tmp60=(void*)_tmp5F->f1;}}_LL6D:
{struct Cyc_List_List*_tmp3DD;rgions=((_tmp3DD=_cycalloc(sizeof(*_tmp3DD)),((_tmp3DD->hd=_tmp60,((_tmp3DD->tl=rgions,_tmp3DD))))));}goto _LL6B;_LL6E: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp61=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp5E;if(_tmp61->tag != 24)goto _LL70;else{_tmp62=_tmp61->f1;}}_LL6F:
# 429
 for(0;_tmp62 != 0;_tmp62=_tmp62->tl){
struct Cyc_List_List*_tmp65;struct Cyc_List_List*_tmp66;struct _tuple15 _tmp64=Cyc_Absyndump_effects_split((void*)_tmp62->hd);_tmp65=_tmp64.f1;_tmp66=_tmp64.f2;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp65,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp66,effects);}
# 434
goto _LL6B;_LL70:;_LL71:
{struct Cyc_List_List*_tmp3DE;effects=((_tmp3DE=_cycalloc(sizeof(*_tmp3DE)),((_tmp3DE->hd=t,((_tmp3DE->tl=effects,_tmp3DE))))));}goto _LL6B;_LL6B:;}{
# 437
struct _tuple15 _tmp3DF;return(_tmp3DF.f1=rgions,((_tmp3DF.f2=effects,_tmp3DF)));};}
# 440
static void Cyc_Absyndump_dumpeff(void*t){
struct Cyc_List_List*_tmp6A;struct Cyc_List_List*_tmp6B;struct _tuple15 _tmp69=Cyc_Absyndump_effects_split(t);_tmp6A=_tmp69.f1;_tmp6B=_tmp69.f2;
_tmp6A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6A);
_tmp6B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6B);
if(_tmp6B != 0){
{struct Cyc_List_List*_tmp6C=_tmp6B;for(0;_tmp6C != 0;_tmp6C=_tmp6C->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp6C->hd);
if(_tmp6C->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 450
if(_tmp6A != 0)Cyc_Absyndump_dump_char((int)'+');}
# 452
if(_tmp6A != 0  || _tmp6B == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp6A != 0;_tmp6A=_tmp6A->tl){
Cyc_Absyndump_dumprgn((void*)_tmp6A->hd);
if(_tmp6A->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 458
Cyc_Absyndump_dump_char((int)'}');}}
# 462
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 467
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp6D=t;struct Cyc_Absyn_Tvar*_tmp73;struct Cyc_Core_Opt*_tmp75;int _tmp76;struct Cyc_Core_Opt*_tmp78;void*_tmp79;int _tmp7A;union Cyc_Absyn_DatatypeInfoU _tmp7C;struct Cyc_List_List*_tmp7D;union Cyc_Absyn_DatatypeFieldInfoU _tmp7F;struct Cyc_List_List*_tmp80;int _tmp91;struct Cyc_List_List*_tmp93;union Cyc_Absyn_AggrInfoU _tmp95;struct Cyc_List_List*_tmp96;enum Cyc_Absyn_AggrKind _tmp98;struct Cyc_List_List*_tmp99;struct _tuple0*_tmp9B;struct Cyc_List_List*_tmp9D;struct _tuple0*_tmp9F;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_Exp*_tmpA2;void*_tmpA4;void*_tmpA6;void*_tmpA7;void*_tmpA9;struct Cyc_Absyn_Aggrdecl*_tmpAE;struct Cyc_Absyn_Enumdecl*_tmpB1;struct Cyc_Absyn_Datatypedecl*_tmpB4;void*_tmpB7;void*_tmpB9;struct Cyc_List_List*_tmpBB;_LL73: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp6E=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D;if(_tmp6E->tag != 8)goto _LL75;}_LL74:
# 470
 goto _LL76;_LL75: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D;if(_tmp6F->tag != 9)goto _LL77;}_LL76:
 goto _LL78;_LL77: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp70=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D;if(_tmp70->tag != 5)goto _LL79;}_LL78:
 return;_LL79: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp71=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp6D;if(_tmp71->tag != 0)goto _LL7B;}_LL7A:
{const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0="void",_tag_dyneither(_tmp3E0,sizeof(char),5))));}return;_LL7B: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp72=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D;if(_tmp72->tag != 2)goto _LL7D;else{_tmp73=_tmp72->f1;}}_LL7C:
 Cyc_Absyndump_dumptvar(_tmp73);return;_LL7D: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6D;if(_tmp74->tag != 1)goto _LL7F;else{_tmp75=_tmp74->f1;if((void*)_tmp74->f2 != 0)goto _LL7F;_tmp76=_tmp74->f3;}}_LL7E:
# 476
{const char*_tmp3E1;Cyc_Absyndump_dump(((_tmp3E1="%",_tag_dyneither(_tmp3E1,sizeof(char),2))));}
if(_tmp75 == 0){const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="?",_tag_dyneither(_tmp3E2,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp75->v);}
{const char*_tmp3E6;void*_tmp3E5[1];struct Cyc_Int_pa_PrintArg_struct _tmp3E4;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3E4.tag=1,((_tmp3E4.f1=(unsigned long)_tmp76,((_tmp3E5[0]=& _tmp3E4,Cyc_aprintf(((_tmp3E6="(%d)",_tag_dyneither(_tmp3E6,sizeof(char),5))),_tag_dyneither(_tmp3E5,sizeof(void*),1)))))))));}return;_LL7F: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6D;if(_tmp77->tag != 1)goto _LL81;else{_tmp78=_tmp77->f1;_tmp79=(void*)_tmp77->f2;_tmp7A=_tmp77->f3;}}_LL80:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp79));return;_LL81: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp7B=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6D;if(_tmp7B->tag != 3)goto _LL83;else{_tmp7C=(_tmp7B->f1).datatype_info;_tmp7D=(_tmp7B->f1).targs;}}_LL82:
# 481
{union Cyc_Absyn_DatatypeInfoU _tmpC2=_tmp7C;struct _tuple0*_tmpC3;int _tmpC4;struct _tuple0*_tmpC5;int _tmpC6;_LLCC: if((_tmpC2.UnknownDatatype).tag != 1)goto _LLCE;_tmpC3=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC2.UnknownDatatype).val).name;_tmpC4=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC2.UnknownDatatype).val).is_extensible;_LLCD:
 _tmpC5=_tmpC3;_tmpC6=_tmpC4;goto _LLCF;_LLCE: if((_tmpC2.KnownDatatype).tag != 2)goto _LLCB;_tmpC5=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC2.KnownDatatype).val))).name;_tmpC6=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC2.KnownDatatype).val))).is_extensible;_LLCF:
# 484
 if(_tmpC6){const char*_tmp3E7;Cyc_Absyndump_dump(((_tmp3E7="@extensible ",_tag_dyneither(_tmp3E7,sizeof(char),13))));}
{const char*_tmp3E8;Cyc_Absyndump_dump(((_tmp3E8="datatype ",_tag_dyneither(_tmp3E8,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpC5);
Cyc_Absyndump_dumptps(_tmp7D);
goto _LLCB;_LLCB:;}
# 490
goto _LL72;_LL83: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp7E=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6D;if(_tmp7E->tag != 4)goto _LL85;else{_tmp7F=(_tmp7E->f1).field_info;_tmp80=(_tmp7E->f1).targs;}}_LL84:
# 492
{union Cyc_Absyn_DatatypeFieldInfoU _tmpC9=_tmp7F;struct _tuple0*_tmpCA;struct _tuple0*_tmpCB;int _tmpCC;struct _tuple0*_tmpCD;int _tmpCE;struct _tuple0*_tmpCF;_LLD1: if((_tmpC9.UnknownDatatypefield).tag != 1)goto _LLD3;_tmpCA=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpC9.UnknownDatatypefield).val).datatype_name;_tmpCB=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpC9.UnknownDatatypefield).val).field_name;_tmpCC=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpC9.UnknownDatatypefield).val).is_extensible;_LLD2:
# 494
 _tmpCD=_tmpCA;_tmpCE=_tmpCC;_tmpCF=_tmpCB;goto _LLD4;_LLD3: if((_tmpC9.KnownDatatypefield).tag != 2)goto _LLD0;_tmpCD=(*((struct _tuple1)(_tmpC9.KnownDatatypefield).val).f1).name;_tmpCE=(*((struct _tuple1)(_tmpC9.KnownDatatypefield).val).f1).is_extensible;_tmpCF=(*((struct _tuple1)(_tmpC9.KnownDatatypefield).val).f2).name;_LLD4:
# 497
 if(_tmpCE){const char*_tmp3E9;Cyc_Absyndump_dump(((_tmp3E9="@extensible ",_tag_dyneither(_tmp3E9,sizeof(char),13))));}
{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="datatype ",_tag_dyneither(_tmp3EA,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpCD);
{const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB=".",_tag_dyneither(_tmp3EB,sizeof(char),2))));}
Cyc_Absyndump_dumpqvar(_tmpCF);
Cyc_Absyndump_dumptps(_tmp80);
goto _LLD0;_LLD0:;}
# 505
goto _LL72;_LL85: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp81->tag != 6)goto _LL87;else{if(_tmp81->f1 != Cyc_Absyn_None)goto _LL87;if(_tmp81->f2 != Cyc_Absyn_Int_sz)goto _LL87;}}_LL86:
 goto _LL88;_LL87: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp82=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp82->tag != 6)goto _LL89;else{if(_tmp82->f1 != Cyc_Absyn_Signed)goto _LL89;if(_tmp82->f2 != Cyc_Absyn_Int_sz)goto _LL89;}}_LL88:
{const char*_tmp3EC;Cyc_Absyndump_dump(((_tmp3EC="int",_tag_dyneither(_tmp3EC,sizeof(char),4))));}return;_LL89: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp83=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp83->tag != 6)goto _LL8B;else{if(_tmp83->f1 != Cyc_Absyn_None)goto _LL8B;if(_tmp83->f2 != Cyc_Absyn_Long_sz)goto _LL8B;}}_LL8A:
 goto _LL8C;_LL8B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp84->tag != 6)goto _LL8D;else{if(_tmp84->f1 != Cyc_Absyn_Signed)goto _LL8D;if(_tmp84->f2 != Cyc_Absyn_Long_sz)goto _LL8D;}}_LL8C:
{const char*_tmp3ED;Cyc_Absyndump_dump(((_tmp3ED="long",_tag_dyneither(_tmp3ED,sizeof(char),5))));}return;_LL8D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp85=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp85->tag != 6)goto _LL8F;else{if(_tmp85->f1 != Cyc_Absyn_None)goto _LL8F;if(_tmp85->f2 != Cyc_Absyn_Char_sz)goto _LL8F;}}_LL8E:
{const char*_tmp3EE;Cyc_Absyndump_dump(((_tmp3EE="char",_tag_dyneither(_tmp3EE,sizeof(char),5))));}return;_LL8F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp86=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp86->tag != 6)goto _LL91;else{if(_tmp86->f1 != Cyc_Absyn_Signed)goto _LL91;if(_tmp86->f2 != Cyc_Absyn_Char_sz)goto _LL91;}}_LL90:
{const char*_tmp3EF;Cyc_Absyndump_dump(((_tmp3EF="signed char",_tag_dyneither(_tmp3EF,sizeof(char),12))));}return;_LL91: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp87->tag != 6)goto _LL93;else{if(_tmp87->f1 != Cyc_Absyn_Unsigned)goto _LL93;if(_tmp87->f2 != Cyc_Absyn_Char_sz)goto _LL93;}}_LL92:
{const char*_tmp3F0;Cyc_Absyndump_dump(((_tmp3F0="unsigned char",_tag_dyneither(_tmp3F0,sizeof(char),14))));}return;_LL93: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp88->tag != 6)goto _LL95;else{if(_tmp88->f1 != Cyc_Absyn_None)goto _LL95;if(_tmp88->f2 != Cyc_Absyn_Short_sz)goto _LL95;}}_LL94:
 goto _LL96;_LL95: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp89->tag != 6)goto _LL97;else{if(_tmp89->f1 != Cyc_Absyn_Signed)goto _LL97;if(_tmp89->f2 != Cyc_Absyn_Short_sz)goto _LL97;}}_LL96:
{const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="short",_tag_dyneither(_tmp3F1,sizeof(char),6))));}return;_LL97: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8A->tag != 6)goto _LL99;else{if(_tmp8A->f1 != Cyc_Absyn_Unsigned)goto _LL99;if(_tmp8A->f2 != Cyc_Absyn_Short_sz)goto _LL99;}}_LL98:
{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="unsigned short",_tag_dyneither(_tmp3F2,sizeof(char),15))));}return;_LL99: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8B->tag != 6)goto _LL9B;else{if(_tmp8B->f1 != Cyc_Absyn_Unsigned)goto _LL9B;if(_tmp8B->f2 != Cyc_Absyn_Int_sz)goto _LL9B;}}_LL9A:
{const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3="unsigned int",_tag_dyneither(_tmp3F3,sizeof(char),13))));}return;_LL9B: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8C->tag != 6)goto _LL9D;else{if(_tmp8C->f1 != Cyc_Absyn_Unsigned)goto _LL9D;if(_tmp8C->f2 != Cyc_Absyn_Long_sz)goto _LL9D;}}_LL9C:
{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="unsigned long",_tag_dyneither(_tmp3F4,sizeof(char),14))));}return;_LL9D: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8D->tag != 6)goto _LL9F;else{if(_tmp8D->f1 != Cyc_Absyn_None)goto _LL9F;if(_tmp8D->f2 != Cyc_Absyn_LongLong_sz)goto _LL9F;}}_LL9E:
 goto _LLA0;_LL9F: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8E->tag != 6)goto _LLA1;else{if(_tmp8E->f1 != Cyc_Absyn_Signed)goto _LLA1;if(_tmp8E->f2 != Cyc_Absyn_LongLong_sz)goto _LLA1;}}_LLA0:
# 520
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLD5:
{const char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5="long long",_tag_dyneither(_tmp3F5,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLD6:
{const char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="__int64",_tag_dyneither(_tmp3F6,sizeof(char),8))));}return;}_LLA1: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6D;if(_tmp8F->tag != 6)goto _LLA3;else{if(_tmp8F->f1 != Cyc_Absyn_Unsigned)goto _LLA3;if(_tmp8F->f2 != Cyc_Absyn_LongLong_sz)goto _LLA3;}}_LLA2:
# 525
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LLD8:
{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="unsigned __int64",_tag_dyneither(_tmp3F7,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLD9:
{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="unsigned long long",_tag_dyneither(_tmp3F8,sizeof(char),19))));}return;}_LLA3: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp90=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6D;if(_tmp90->tag != 7)goto _LLA5;else{_tmp91=_tmp90->f1;}}_LLA4:
# 530
 if(_tmp91 == 0){
const char*_tmp3F9;Cyc_Absyndump_dump(((_tmp3F9="float",_tag_dyneither(_tmp3F9,sizeof(char),6))));}else{
if(_tmp91 == 1){
const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="double",_tag_dyneither(_tmp3FA,sizeof(char),7))));}else{
# 535
const char*_tmp3FB;Cyc_Absyndump_dump(((_tmp3FB="long double",_tag_dyneither(_tmp3FB,sizeof(char),12))));}}
return;_LLA5: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D;if(_tmp92->tag != 10)goto _LLA7;else{_tmp93=_tmp92->f1;}}_LLA6:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp93);return;_LLA7: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6D;if(_tmp94->tag != 11)goto _LLA9;else{_tmp95=(_tmp94->f1).aggr_info;_tmp96=(_tmp94->f1).targs;}}_LLA8: {
# 539
enum Cyc_Absyn_AggrKind _tmpE4;struct _tuple0*_tmpE5;struct _tuple10 _tmpE3=Cyc_Absyn_aggr_kinded_name(_tmp95);_tmpE4=_tmpE3.f1;_tmpE5=_tmpE3.f2;
Cyc_Absyndump_dumpaggr_kind(_tmpE4);Cyc_Absyndump_dumpqvar(_tmpE5);Cyc_Absyndump_dumptps(_tmp96);
return;}_LLA9: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp97=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D;if(_tmp97->tag != 12)goto _LLAB;else{_tmp98=_tmp97->f1;_tmp99=_tmp97->f2;}}_LLAA:
# 543
 Cyc_Absyndump_dumpaggr_kind(_tmp98);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp99);Cyc_Absyndump_dump_char((int)'}');return;_LLAB: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6D;if(_tmp9A->tag != 13)goto _LLAD;else{_tmp9B=_tmp9A->f1;}}_LLAC:
{const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC="enum ",_tag_dyneither(_tmp3FC,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmp9B);return;_LLAD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp9C=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6D;if(_tmp9C->tag != 14)goto _LLAF;else{_tmp9D=_tmp9C->f1;}}_LLAE:
{const char*_tmp3FD;Cyc_Absyndump_dump(((_tmp3FD="enum {",_tag_dyneither(_tmp3FD,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(_tmp9D);{const char*_tmp3FE;Cyc_Absyndump_dump(((_tmp3FE="}",_tag_dyneither(_tmp3FE,sizeof(char),2))));}return;_LLAF: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D;if(_tmp9E->tag != 17)goto _LLB1;else{_tmp9F=_tmp9E->f1;_tmpA0=_tmp9E->f2;}}_LLB0:
(Cyc_Absyndump_dumpqvar(_tmp9F),Cyc_Absyndump_dumptps(_tmpA0));return;_LLB1: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D;if(_tmpA1->tag != 18)goto _LLB3;else{_tmpA2=_tmpA1->f1;}}_LLB2:
{const char*_tmp3FF;Cyc_Absyndump_dump(((_tmp3FF="valueof_t(",_tag_dyneither(_tmp3FF,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpA2);{const char*_tmp400;Cyc_Absyndump_dump(((_tmp400=")",_tag_dyneither(_tmp400,sizeof(char),2))));}return;_LLB3: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA3=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6D;if(_tmpA3->tag != 15)goto _LLB5;else{_tmpA4=(void*)_tmpA3->f1;}}_LLB4:
# 549
{const char*_tmp401;Cyc_Absyndump_dump(((_tmp401="region_t<",_tag_dyneither(_tmp401,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpA4);{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402=">",_tag_dyneither(_tmp402,sizeof(char),2))));}return;_LLB5: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpA5=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6D;if(_tmpA5->tag != 16)goto _LLB7;else{_tmpA6=(void*)_tmpA5->f1;_tmpA7=(void*)_tmpA5->f2;}}_LLB6:
{const char*_tmp403;Cyc_Absyndump_dump(((_tmp403="dynregion_t<",_tag_dyneither(_tmp403,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpA6);
{const char*_tmp404;Cyc_Absyndump_dump(((_tmp404=",",_tag_dyneither(_tmp404,sizeof(char),2))));}Cyc_Absyndump_dumprgn(_tmpA7);{const char*_tmp405;Cyc_Absyndump_dump(((_tmp405=">",_tag_dyneither(_tmp405,sizeof(char),2))));}return;_LLB7: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpA8=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6D;if(_tmpA8->tag != 19)goto _LLB9;else{_tmpA9=(void*)_tmpA8->f1;}}_LLB8:
{const char*_tmp406;Cyc_Absyndump_dump(((_tmp406="tag_t<",_tag_dyneither(_tmp406,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(_tmpA9);{const char*_tmp407;Cyc_Absyndump_dump(((_tmp407=">",_tag_dyneither(_tmp407,sizeof(char),2))));}return;_LLB9: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpAA=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp6D;if(_tmpAA->tag != 21)goto _LLBB;}_LLBA:
{const char*_tmp408;Cyc_Absyndump_dump(((_tmp408="`U",_tag_dyneither(_tmp408,sizeof(char),3))));}goto _LL72;_LLBB: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp6D;if(_tmpAB->tag != 22)goto _LLBD;}_LLBC:
{const char*_tmp409;Cyc_Absyndump_dump(((_tmp409="`RC",_tag_dyneither(_tmp409,sizeof(char),4))));}goto _LL72;_LLBD: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAC=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D;if(_tmpAC->tag != 26)goto _LLBF;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpAD=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(*_tmpAC->f1).r;if(_tmpAD->tag != 0)goto _LLBF;else{_tmpAE=_tmpAD->f1;}}}_LLBE:
# 556
 Cyc_Absyndump_dump_aggrdecl(_tmpAE);return;_LLBF: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpAF=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D;if(_tmpAF->tag != 26)goto _LLC1;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB0=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(*_tmpAF->f1).r;if(_tmpB0->tag != 1)goto _LLC1;else{_tmpB1=_tmpB0->f1;}}}_LLC0:
# 558
 Cyc_Absyndump_dump_enumdecl(_tmpB1);return;_LLC1: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D;if(_tmpB2->tag != 26)goto _LLC3;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB3=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(*_tmpB2->f1).r;if(_tmpB3->tag != 2)goto _LLC3;else{_tmpB4=_tmpB3->f1;}}}_LLC2:
# 560
 Cyc_Absyndump_dump_datatypedecl(_tmpB4);return;_LLC3: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp6D;if(_tmpB5->tag != 20)goto _LLC5;}_LLC4:
{const char*_tmp40A;Cyc_Absyndump_dump(((_tmp40A="`H",_tag_dyneither(_tmp40A,sizeof(char),3))));}goto _LL72;_LLC5: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpB6=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6D;if(_tmpB6->tag != 23)goto _LLC7;else{_tmpB7=(void*)_tmpB6->f1;}}_LLC6:
{const char*_tmp40B;Cyc_Absyndump_dump(((_tmp40B="{",_tag_dyneither(_tmp40B,sizeof(char),2))));}Cyc_Absyndump_dumptyp(_tmpB7);{const char*_tmp40C;Cyc_Absyndump_dump(((_tmp40C="}",_tag_dyneither(_tmp40C,sizeof(char),2))));}goto _LL72;_LLC7: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6D;if(_tmpB8->tag != 25)goto _LLC9;else{_tmpB9=(void*)_tmpB8->f1;}}_LLC8:
{const char*_tmp40D;Cyc_Absyndump_dump(((_tmp40D="regions(",_tag_dyneither(_tmp40D,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmpB9);{const char*_tmp40E;Cyc_Absyndump_dump(((_tmp40E=")",_tag_dyneither(_tmp40E,sizeof(char),2))));}goto _LL72;_LLC9: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpBA=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6D;if(_tmpBA->tag != 24)goto _LL72;else{_tmpBB=_tmpBA->f1;}}_LLCA:
# 565
 for(0;_tmpBB != 0;_tmpBB=_tmpBB->tl){
Cyc_Absyndump_dumptyp((void*)_tmpBB->hd);
if(_tmpBB->tl != 0){const char*_tmp40F;Cyc_Absyndump_dump(((_tmp40F="+",_tag_dyneither(_tmp40F,sizeof(char),2))));}}_LL72:;}
# 572
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str((struct _dyneither_ptr*)vo);}
# 575
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 578
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
void*_tmpFB;void*_tmpFC;struct _tuple16*_tmpFA=cmp;_tmpFB=(*_tmpFA).f1;_tmpFC=(*_tmpFA).f2;
Cyc_Absyndump_dumpeff(_tmpFB);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpFC);}
# 582
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
const char*_tmp410;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp410=",",_tag_dyneither(_tmp410,sizeof(char),2))));}
# 585
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 588
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 593
if(c_varargs){
const char*_tmp411;Cyc_Absyndump_dump(((_tmp411="...",_tag_dyneither(_tmp411,sizeof(char),4))));}else{
if(cyc_varargs != 0){
struct _tuple8*_tmp412;struct _tuple8*_tmpFF=(_tmp412=_cycalloc(sizeof(*_tmp412)),((_tmp412->f1=cyc_varargs->name,((_tmp412->f2=cyc_varargs->tq,((_tmp412->f3=cyc_varargs->type,_tmp412)))))));
{const char*_tmp413;Cyc_Absyndump_dump(((_tmp413="...",_tag_dyneither(_tmp413,sizeof(char),4))));}
if(cyc_varargs->inject){const char*_tmp414;Cyc_Absyndump_dump(((_tmp414=" inject ",_tag_dyneither(_tmp414,sizeof(char),9))));}
Cyc_Absyndump_dumpfunarg(_tmpFF);}}
# 601
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff((void*)effopt);}
# 605
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 609
Cyc_Absyndump_dump_char((int)')');}
# 612
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 616
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmp103=d;struct Cyc_Absyn_Exp*_tmp105;struct _dyneither_ptr*_tmp107;_LLDC: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp104=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp103;if(_tmp104->tag != 0)goto _LLDE;else{_tmp105=_tmp104->f1;}}_LLDD:
{const char*_tmp415;Cyc_Absyndump_dump(((_tmp415=".[",_tag_dyneither(_tmp415,sizeof(char),3))));}Cyc_Absyndump_dumpexp(_tmp105);Cyc_Absyndump_dump_char((int)']');goto _LLDB;_LLDE: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp106=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp103;if(_tmp106->tag != 1)goto _LLDB;else{_tmp107=_tmp106->f1;}}_LLDF:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp107);goto _LLDB;_LLDB:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 623
void Cyc_Absyndump_dumpde(struct _tuple17*de){
{const char*_tmp418;const char*_tmp417;const char*_tmp416;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp416="",_tag_dyneither(_tmp416,sizeof(char),1))),((_tmp417="=",_tag_dyneither(_tmp417,sizeof(char),2))),((_tmp418="=",_tag_dyneither(_tmp418,sizeof(char),2))));}
Cyc_Absyndump_dumpexp((*de).f2);}
# 628
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp41B;const char*_tmp41A;const char*_tmp419;((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp419="",_tag_dyneither(_tmp419,sizeof(char),1))),((_tmp41A="",_tag_dyneither(_tmp41A,sizeof(char),1))),((_tmp41B=",",_tag_dyneither(_tmp41B,sizeof(char),2))));}
# 632
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec){
const char*_tmp41C;Cyc_Absyndump_dump_nospace(((_tmp41C="(",_tag_dyneither(_tmp41C,sizeof(char),2))));}
{void*_tmp110=e->r;enum Cyc_Absyn_Sign _tmp112;char _tmp113;struct _dyneither_ptr _tmp115;enum Cyc_Absyn_Sign _tmp117;short _tmp118;int _tmp11A;int _tmp11C;int _tmp11E;enum Cyc_Absyn_Sign _tmp120;long long _tmp121;struct _dyneither_ptr _tmp123;struct _dyneither_ptr _tmp126;struct _dyneither_ptr _tmp128;struct _tuple0*_tmp12A;enum Cyc_Absyn_Primop _tmp12C;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Core_Opt*_tmp130;struct Cyc_Absyn_Exp*_tmp131;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_List_List*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14F;void*_tmp151;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp157;void*_tmp159;struct Cyc_Absyn_Exp*_tmp15B;void*_tmp15D;int _tmp15F;struct _dyneither_ptr _tmp160;struct Cyc_Absyn_Exp*_tmp162;struct _dyneither_ptr*_tmp163;void*_tmp165;struct _dyneither_ptr*_tmp167;void*_tmp169;unsigned int _tmp16B;struct Cyc_Absyn_Exp*_tmp16D;struct Cyc_Absyn_Exp*_tmp16F;struct _dyneither_ptr*_tmp170;struct Cyc_Absyn_Exp*_tmp172;struct _dyneither_ptr*_tmp173;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_List_List*_tmp178;struct _tuple8*_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Vardecl*_tmp17F;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct _tuple0*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_Datatypefield*_tmp18A;struct _tuple0*_tmp18C;struct _tuple0*_tmp18E;int _tmp190;struct Cyc_Absyn_Exp*_tmp191;void**_tmp192;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Core_Opt*_tmp198;struct Cyc_List_List*_tmp199;struct Cyc_Absyn_Stmt*_tmp19B;_LLE1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp111=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp111->tag != 0)goto _LLE3;else{if(((_tmp111->f1).Char_c).tag != 2)goto _LLE3;_tmp112=((struct _tuple3)((_tmp111->f1).Char_c).val).f1;_tmp113=((struct _tuple3)((_tmp111->f1).Char_c).val).f2;}}_LLE2:
# 638
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp113));Cyc_Absyndump_dump_char((int)'\'');
goto _LLE0;_LLE3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp114=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp114->tag != 0)goto _LLE5;else{if(((_tmp114->f1).Wchar_c).tag != 3)goto _LLE5;_tmp115=(struct _dyneither_ptr)((_tmp114->f1).Wchar_c).val;}}_LLE4:
{const char*_tmp420;void*_tmp41F[1];struct Cyc_String_pa_PrintArg_struct _tmp41E;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp41E.tag=0,((_tmp41E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp115),((_tmp41F[0]=& _tmp41E,Cyc_aprintf(((_tmp420="L'%s'",_tag_dyneither(_tmp420,sizeof(char),6))),_tag_dyneither(_tmp41F,sizeof(void*),1)))))))));}goto _LLE0;_LLE5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp116=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp116->tag != 0)goto _LLE7;else{if(((_tmp116->f1).Short_c).tag != 4)goto _LLE7;_tmp117=((struct _tuple4)((_tmp116->f1).Short_c).val).f1;_tmp118=((struct _tuple4)((_tmp116->f1).Short_c).val).f2;}}_LLE6:
{const char*_tmp424;void*_tmp423[1];struct Cyc_Int_pa_PrintArg_struct _tmp422;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp422.tag=1,((_tmp422.f1=(unsigned long)((int)_tmp118),((_tmp423[0]=& _tmp422,Cyc_aprintf(((_tmp424="%d",_tag_dyneither(_tmp424,sizeof(char),3))),_tag_dyneither(_tmp423,sizeof(void*),1)))))))));}goto _LLE0;_LLE7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp119=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp119->tag != 0)goto _LLE9;else{if(((_tmp119->f1).Int_c).tag != 5)goto _LLE9;if(((struct _tuple5)((_tmp119->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LLE9;_tmp11A=((struct _tuple5)((_tmp119->f1).Int_c).val).f2;}}_LLE8:
 _tmp11C=_tmp11A;goto _LLEA;_LLE9: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp11B->tag != 0)goto _LLEB;else{if(((_tmp11B->f1).Int_c).tag != 5)goto _LLEB;if(((struct _tuple5)((_tmp11B->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLEB;_tmp11C=((struct _tuple5)((_tmp11B->f1).Int_c).val).f2;}}_LLEA:
{const char*_tmp428;void*_tmp427[1];struct Cyc_Int_pa_PrintArg_struct _tmp426;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp426.tag=1,((_tmp426.f1=(unsigned long)_tmp11C,((_tmp427[0]=& _tmp426,Cyc_aprintf(((_tmp428="%d",_tag_dyneither(_tmp428,sizeof(char),3))),_tag_dyneither(_tmp427,sizeof(void*),1)))))))));}goto _LLE0;_LLEB: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp11D->tag != 0)goto _LLED;else{if(((_tmp11D->f1).Int_c).tag != 5)goto _LLED;if(((struct _tuple5)((_tmp11D->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LLED;_tmp11E=((struct _tuple5)((_tmp11D->f1).Int_c).val).f2;}}_LLEC:
{const char*_tmp42C;void*_tmp42B[1];struct Cyc_Int_pa_PrintArg_struct _tmp42A;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp42A.tag=1,((_tmp42A.f1=(unsigned int)_tmp11E,((_tmp42B[0]=& _tmp42A,Cyc_aprintf(((_tmp42C="%u",_tag_dyneither(_tmp42C,sizeof(char),3))),_tag_dyneither(_tmp42B,sizeof(void*),1)))))))));}goto _LLE0;_LLED: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp11F->tag != 0)goto _LLEF;else{if(((_tmp11F->f1).LongLong_c).tag != 6)goto _LLEF;_tmp120=((struct _tuple6)((_tmp11F->f1).LongLong_c).val).f1;_tmp121=((struct _tuple6)((_tmp11F->f1).LongLong_c).val).f2;}}_LLEE:
# 646
{const char*_tmp42D;Cyc_Absyndump_dump(((_tmp42D="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp42D,sizeof(char),27))));}goto _LLE0;_LLEF: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp122=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp122->tag != 0)goto _LLF1;else{if(((_tmp122->f1).Float_c).tag != 7)goto _LLF1;_tmp123=((struct _tuple7)((_tmp122->f1).Float_c).val).f1;}}_LLF0:
 Cyc_Absyndump_dump(_tmp123);goto _LLE0;_LLF1: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp124=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp124->tag != 0)goto _LLF3;else{if(((_tmp124->f1).Null_c).tag != 1)goto _LLF3;}}_LLF2:
{const char*_tmp42E;Cyc_Absyndump_dump(((_tmp42E="NULL",_tag_dyneither(_tmp42E,sizeof(char),5))));}goto _LLE0;_LLF3: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp125->tag != 0)goto _LLF5;else{if(((_tmp125->f1).String_c).tag != 8)goto _LLF5;_tmp126=(struct _dyneither_ptr)((_tmp125->f1).String_c).val;}}_LLF4:
# 650
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp126));Cyc_Absyndump_dump_char((int)'"');
goto _LLE0;_LLF5: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp127=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp127->tag != 0)goto _LLF7;else{if(((_tmp127->f1).Wstring_c).tag != 9)goto _LLF7;_tmp128=(struct _dyneither_ptr)((_tmp127->f1).Wstring_c).val;}}_LLF6:
# 653
{const char*_tmp42F;Cyc_Absyndump_dump(((_tmp42F="L\"",_tag_dyneither(_tmp42F,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp128);Cyc_Absyndump_dump_char((int)'"');
goto _LLE0;_LLF7: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp129=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp129->tag != 1)goto _LLF9;else{_tmp12A=_tmp129->f1;}}_LLF8:
# 656
 Cyc_Absyndump_dumpqvar(_tmp12A);goto _LLE0;_LLF9: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp12B->tag != 2)goto _LLFB;else{_tmp12C=_tmp12B->f1;_tmp12D=_tmp12B->f2;}}_LLFA: {
# 659
struct _dyneither_ptr _tmp1AB=Cyc_Absynpp_prim2str(_tmp12C);
switch(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp12D)){case 1: _LL14B:
# 662
 if(_tmp12C == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
{const char*_tmp430;Cyc_Absyndump_dump(((_tmp430="numelts(",_tag_dyneither(_tmp430,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd);{
const char*_tmp431;Cyc_Absyndump_dump(((_tmp431=")",_tag_dyneither(_tmp431,sizeof(char),2))));};}else{
# 667
Cyc_Absyndump_dump(_tmp1AB);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd);}
# 670
break;case 2: _LL14C:
# 672
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp12D))->hd);
Cyc_Absyndump_dump(_tmp1AB);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp12D->tl))->hd);
break;default: _LL14D: {
# 678
struct Cyc_Core_Failure_exn_struct _tmp437;const char*_tmp436;struct Cyc_Core_Failure_exn_struct*_tmp435;(int)_throw((void*)((_tmp435=_cycalloc(sizeof(*_tmp435)),((_tmp435[0]=((_tmp437.tag=Cyc_Core_Failure,((_tmp437.f1=((_tmp436="Absyndump -- Bad number of arguments to primop",_tag_dyneither(_tmp436,sizeof(char),47))),_tmp437)))),_tmp435)))));}}
# 680
goto _LLE0;}_LLFB: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp12E=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp12E->tag != 3)goto _LLFD;else{_tmp12F=_tmp12E->f1;_tmp130=_tmp12E->f2;_tmp131=_tmp12E->f3;}}_LLFC:
# 683
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp12F);
if(_tmp130 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp130->v));
{const char*_tmp438;Cyc_Absyndump_dump_nospace(((_tmp438="=",_tag_dyneither(_tmp438,sizeof(char),2))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp131);
goto _LLE0;_LLFD: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp132=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp132->tag != 4)goto _LLFF;else{_tmp133=_tmp132->f1;if(_tmp132->f2 != Cyc_Absyn_PreInc)goto _LLFF;}}_LLFE:
# 690
{const char*_tmp439;Cyc_Absyndump_dump(((_tmp439="++",_tag_dyneither(_tmp439,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp133);goto _LLE0;_LLFF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp134->tag != 4)goto _LL101;else{_tmp135=_tmp134->f1;if(_tmp134->f2 != Cyc_Absyn_PreDec)goto _LL101;}}_LL100:
{const char*_tmp43A;Cyc_Absyndump_dump(((_tmp43A="--",_tag_dyneither(_tmp43A,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp135);goto _LLE0;_LL101: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp136=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp136->tag != 4)goto _LL103;else{_tmp137=_tmp136->f1;if(_tmp136->f2 != Cyc_Absyn_PostInc)goto _LL103;}}_LL102:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp137);{const char*_tmp43B;Cyc_Absyndump_dump(((_tmp43B="++",_tag_dyneither(_tmp43B,sizeof(char),3))));}goto _LLE0;_LL103: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp138->tag != 4)goto _LL105;else{_tmp139=_tmp138->f1;if(_tmp138->f2 != Cyc_Absyn_PostDec)goto _LL105;}}_LL104:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp139);{const char*_tmp43C;Cyc_Absyndump_dump(((_tmp43C="--",_tag_dyneither(_tmp43C,sizeof(char),3))));}goto _LLE0;_LL105: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp13A->tag != 5)goto _LL107;else{_tmp13B=_tmp13A->f1;_tmp13C=_tmp13A->f2;_tmp13D=_tmp13A->f3;}}_LL106:
# 696
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp13B);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp13C);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp13D);
goto _LLE0;_LL107: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp13E->tag != 6)goto _LL109;else{_tmp13F=_tmp13E->f1;_tmp140=_tmp13E->f2;}}_LL108:
# 703
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp13F);{const char*_tmp43D;Cyc_Absyndump_dump(((_tmp43D=" && ",_tag_dyneither(_tmp43D,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp140);goto _LLE0;_LL109: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp141=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp141->tag != 7)goto _LL10B;else{_tmp142=_tmp141->f1;_tmp143=_tmp141->f2;}}_LL10A:
# 707
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp142);{const char*_tmp43E;Cyc_Absyndump_dump(((_tmp43E=" || ",_tag_dyneither(_tmp43E,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp143);goto _LLE0;_LL10B: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp144->tag != 8)goto _LL10D;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}}_LL10C:
# 713
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp145);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp146);
Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL10D: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp147->tag != 9)goto _LL10F;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LL10E:
# 720
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp148);
{const char*_tmp43F;Cyc_Absyndump_dump_nospace(((_tmp43F="(",_tag_dyneither(_tmp43F,sizeof(char),2))));}{
# 726
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp148->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp149);
{const char*_tmp440;Cyc_Absyndump_dump_nospace(((_tmp440=")",_tag_dyneither(_tmp440,sizeof(char),2))));}
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LLE0;};_LL10F: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp14A->tag != 10)goto _LL111;else{_tmp14B=_tmp14A->f1;}}_LL110:
# 735
 Cyc_Absyndump_dumploc(e->loc);
{const char*_tmp441;Cyc_Absyndump_dump(((_tmp441="throw",_tag_dyneither(_tmp441,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp14B);goto _LLE0;_LL111: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp14C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp14C->tag != 11)goto _LL113;else{_tmp14D=_tmp14C->f1;}}_LL112:
# 738
 _tmp14F=_tmp14D;goto _LL114;_LL113: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp14E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp14E->tag != 12)goto _LL115;else{_tmp14F=_tmp14E->f1;}}_LL114:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp14F);goto _LLE0;_LL115: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp150->tag != 13)goto _LL117;else{_tmp151=(void*)_tmp150->f1;_tmp152=_tmp150->f2;}}_LL116:
# 742
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp151);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp152);
goto _LLE0;_LL117: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp153=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp153->tag != 14)goto _LL119;else{_tmp154=_tmp153->f1;}}_LL118:
# 746
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);goto _LLE0;_LL119: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp155=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp155->tag != 15)goto _LL11B;else{_tmp156=_tmp155->f1;_tmp157=_tmp155->f2;}}_LL11A:
# 749
{const char*_tmp442;Cyc_Absyndump_dump(((_tmp442="new ",_tag_dyneither(_tmp442,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);goto _LLE0;_LL11B: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp158=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp158->tag != 16)goto _LL11D;else{_tmp159=(void*)_tmp158->f1;}}_LL11C:
# 752
{const char*_tmp443;Cyc_Absyndump_dump(((_tmp443="sizeof(",_tag_dyneither(_tmp443,sizeof(char),8))));}Cyc_Absyndump_dumptyp(_tmp159);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11D: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp15A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp15A->tag != 17)goto _LL11F;else{_tmp15B=_tmp15A->f1;}}_LL11E:
# 755
{const char*_tmp444;Cyc_Absyndump_dump(((_tmp444="sizeof(",_tag_dyneither(_tmp444,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp15B);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL11F: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp15C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp15C->tag != 37)goto _LL121;else{_tmp15D=(void*)_tmp15C->f1;}}_LL120:
# 758
{const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="valueof(",_tag_dyneither(_tmp445,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp15D);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL121: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp15E->tag != 38)goto _LL123;else{_tmp15F=_tmp15E->f1;_tmp160=_tmp15E->f2;}}_LL122:
# 761
{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="__asm__",_tag_dyneither(_tmp446,sizeof(char),8))));}
if(_tmp15F){const char*_tmp447;Cyc_Absyndump_dump(((_tmp447=" volatile ",_tag_dyneither(_tmp447,sizeof(char),11))));}
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp160);
Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL123: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp161=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp161->tag != 36)goto _LL125;else{_tmp162=_tmp161->f1;_tmp163=_tmp161->f2;}}_LL124:
# 769
{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448="tagcheck(",_tag_dyneither(_tmp448,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp162);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp163);
Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL125: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp164->tag != 18)goto _LL127;else{_tmp165=(void*)_tmp164->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp166=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp164->f2);if(_tmp166->tag != 0)goto _LL127;else{_tmp167=_tmp166->f1;}};}}_LL126:
# 774
{const char*_tmp449;Cyc_Absyndump_dump(((_tmp449="offsetof(",_tag_dyneither(_tmp449,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp165);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dump_nospace(*_tmp167);
Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL127: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp168=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp168->tag != 18)goto _LL129;else{_tmp169=(void*)_tmp168->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp16A=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp168->f2);if(_tmp16A->tag != 1)goto _LL129;else{_tmp16B=_tmp16A->f1;}};}}_LL128:
# 779
{const char*_tmp44A;Cyc_Absyndump_dump(((_tmp44A="offsetof(",_tag_dyneither(_tmp44A,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp169);Cyc_Absyndump_dump_char((int)',');{const char*_tmp44E;void*_tmp44D[1];struct Cyc_Int_pa_PrintArg_struct _tmp44C;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp44C.tag=1,((_tmp44C.f1=(unsigned long)((int)_tmp16B),((_tmp44D[0]=& _tmp44C,Cyc_aprintf(((_tmp44E="%d",_tag_dyneither(_tmp44E,sizeof(char),3))),_tag_dyneither(_tmp44D,sizeof(void*),1)))))))));}
Cyc_Absyndump_dump_char((int)')');
goto _LLE0;_LL129: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp16C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp16C->tag != 19)goto _LL12B;else{_tmp16D=_tmp16C->f1;}}_LL12A:
# 784
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp16D);goto _LLE0;_LL12B: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp16E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp16E->tag != 20)goto _LL12D;else{_tmp16F=_tmp16E->f1;_tmp170=_tmp16E->f2;}}_LL12C:
# 787
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp16F);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp170);
goto _LLE0;_LL12D: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp171=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp171->tag != 21)goto _LL12F;else{_tmp172=_tmp171->f1;_tmp173=_tmp171->f2;}}_LL12E:
# 791
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp172);{const char*_tmp44F;Cyc_Absyndump_dump_nospace(((_tmp44F="->",_tag_dyneither(_tmp44F,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp173);
goto _LLE0;_LL12F: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp174=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp174->tag != 22)goto _LL131;else{_tmp175=_tmp174->f1;_tmp176=_tmp174->f2;}}_LL130:
# 795
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp175);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp176);Cyc_Absyndump_dump_char((int)']');goto _LLE0;_LL131: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp177=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp177->tag != 23)goto _LL133;else{_tmp178=_tmp177->f1;}}_LL132:
# 799
{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="$(",_tag_dyneither(_tmp450,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp178);Cyc_Absyndump_dump_char((int)')');goto _LLE0;_LL133: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp179=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp179->tag != 24)goto _LL135;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;}}_LL134:
# 802
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp17A).f3);
Cyc_Absyndump_dump_char((int)')');
{const char*_tmp453;const char*_tmp452;const char*_tmp451;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp17B,((_tmp451="{",_tag_dyneither(_tmp451,sizeof(char),2))),((_tmp452="}",_tag_dyneither(_tmp452,sizeof(char),2))),((_tmp453=",",_tag_dyneither(_tmp453,sizeof(char),2))));}
goto _LLE0;_LL135: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp17C->tag != 25)goto _LL137;else{_tmp17D=_tmp17C->f1;}}_LL136:
# 809
{const char*_tmp456;const char*_tmp455;const char*_tmp454;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp17D,((_tmp454="{",_tag_dyneither(_tmp454,sizeof(char),2))),((_tmp455="}",_tag_dyneither(_tmp455,sizeof(char),2))),((_tmp456=",",_tag_dyneither(_tmp456,sizeof(char),2))));}
goto _LLE0;_LL137: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp17E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp17E->tag != 26)goto _LL139;else{_tmp17F=_tmp17E->f1;_tmp180=_tmp17E->f2;_tmp181=_tmp17E->f3;}}_LL138:
# 813
{const char*_tmp457;Cyc_Absyndump_dump(((_tmp457="new {for",_tag_dyneither(_tmp457,sizeof(char),9))));}Cyc_Absyndump_dump_str((*_tmp17F->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp180);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp181);Cyc_Absyndump_dump_char((int)'}');
goto _LLE0;_LL139: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp182=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp182->tag != 27)goto _LL13B;else{_tmp183=_tmp182->f1;_tmp184=_tmp182->f2;_tmp185=_tmp182->f3;}}_LL13A:
# 818
 Cyc_Absyndump_dumpqvar(_tmp183);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp184 != 0)
Cyc_Absyndump_dumptps(_tmp184);
{const char*_tmp45A;const char*_tmp459;const char*_tmp458;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp185,((_tmp458="",_tag_dyneither(_tmp458,sizeof(char),1))),((_tmp459="}",_tag_dyneither(_tmp459,sizeof(char),2))),((_tmp45A=",",_tag_dyneither(_tmp45A,sizeof(char),2))));}
goto _LLE0;_LL13B: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp186->tag != 28)goto _LL13D;else{_tmp187=_tmp186->f2;}}_LL13C:
# 826
{const char*_tmp45D;const char*_tmp45C;const char*_tmp45B;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp187,((_tmp45B="{",_tag_dyneither(_tmp45B,sizeof(char),2))),((_tmp45C="}",_tag_dyneither(_tmp45C,sizeof(char),2))),((_tmp45D=",",_tag_dyneither(_tmp45D,sizeof(char),2))));}
goto _LLE0;_LL13D: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp188=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp188->tag != 29)goto _LL13F;else{_tmp189=_tmp188->f1;_tmp18A=_tmp188->f3;}}_LL13E:
# 830
 Cyc_Absyndump_dumpqvar(_tmp18A->name);
if(_tmp189 != 0){const char*_tmp460;const char*_tmp45F;const char*_tmp45E;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp189,((_tmp45E="(",_tag_dyneither(_tmp45E,sizeof(char),2))),((_tmp45F=")",_tag_dyneither(_tmp45F,sizeof(char),2))),((_tmp460=",",_tag_dyneither(_tmp460,sizeof(char),2))));}
goto _LLE0;_LL13F: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp18B=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp18B->tag != 30)goto _LL141;else{_tmp18C=_tmp18B->f1;}}_LL140:
# 834
 Cyc_Absyndump_dumpqvar(_tmp18C);goto _LLE0;_LL141: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp18D=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp18D->tag != 31)goto _LL143;else{_tmp18E=_tmp18D->f1;}}_LL142:
 Cyc_Absyndump_dumpqvar(_tmp18E);goto _LLE0;_LL143: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp18F->tag != 32)goto _LL145;else{_tmp190=(_tmp18F->f1).is_calloc;_tmp191=(_tmp18F->f1).rgn;_tmp192=(_tmp18F->f1).elt_type;_tmp193=(_tmp18F->f1).num_elts;}}_LL144:
# 838
 Cyc_Absyndump_dumploc(_tmp193->loc);
if(_tmp190){
if(_tmp191 != 0){
{const char*_tmp461;Cyc_Absyndump_dump(((_tmp461="rcalloc(",_tag_dyneither(_tmp461,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp191);{const char*_tmp462;Cyc_Absyndump_dump(((_tmp462=",",_tag_dyneither(_tmp462,sizeof(char),2))));};}else{
# 844
const char*_tmp463;Cyc_Absyndump_dump(((_tmp463="calloc",_tag_dyneither(_tmp463,sizeof(char),7))));}
# 846
Cyc_Absyndump_dumpexp(_tmp193);
{const char*_tmp464;Cyc_Absyndump_dump(((_tmp464=",",_tag_dyneither(_tmp464,sizeof(char),2))));}
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp192)),0));{
const char*_tmp465;Cyc_Absyndump_dump(((_tmp465=")",_tag_dyneither(_tmp465,sizeof(char),2))));};}else{
# 851
if(_tmp191 != 0){
{const char*_tmp466;Cyc_Absyndump_dump(((_tmp466="rmalloc(",_tag_dyneither(_tmp466,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp191);{const char*_tmp467;Cyc_Absyndump_dump(((_tmp467=",",_tag_dyneither(_tmp467,sizeof(char),2))));};}else{
# 855
const char*_tmp468;Cyc_Absyndump_dump(((_tmp468="malloc(",_tag_dyneither(_tmp468,sizeof(char),8))));}
# 858
if(_tmp192 != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp192,0),_tmp193,0));else{
# 861
Cyc_Absyndump_dumpexp(_tmp193);}{
const char*_tmp469;Cyc_Absyndump_dump(((_tmp469=")",_tag_dyneither(_tmp469,sizeof(char),2))));};}
# 864
goto _LLE0;_LL145: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp194=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp194->tag != 33)goto _LL147;else{_tmp195=_tmp194->f1;_tmp196=_tmp194->f2;}}_LL146:
# 867
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp195);
{const char*_tmp46A;Cyc_Absyndump_dump_nospace(((_tmp46A=":=:",_tag_dyneither(_tmp46A,sizeof(char),4))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp196);
goto _LLE0;_LL147: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp197=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp197->tag != 34)goto _LL149;else{_tmp198=_tmp197->f1;_tmp199=_tmp197->f2;}}_LL148:
# 874
{const char*_tmp46D;const char*_tmp46C;const char*_tmp46B;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp199,((_tmp46B="{",_tag_dyneither(_tmp46B,sizeof(char),2))),((_tmp46C="}",_tag_dyneither(_tmp46C,sizeof(char),2))),((_tmp46D=",",_tag_dyneither(_tmp46D,sizeof(char),2))));}
goto _LLE0;_LL149: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp19A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp110;if(_tmp19A->tag != 35)goto _LLE0;else{_tmp19B=_tmp19A->f1;}}_LL14A:
# 878
{const char*_tmp46E;Cyc_Absyndump_dump_nospace(((_tmp46E="({",_tag_dyneither(_tmp46E,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp19B,1);
{const char*_tmp46F;Cyc_Absyndump_dump_nospace(((_tmp46F="})",_tag_dyneither(_tmp46F,sizeof(char),3))));}
goto _LLE0;_LLE0:;}
# 883
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 887
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 891
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp1E8=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp1E8->where_clause == 0  && (_tmp1E8->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp470;Cyc_Absyndump_dump(((_tmp470="default:",_tag_dyneither(_tmp470,sizeof(char),9))));}else{
# 897
{const char*_tmp471;Cyc_Absyndump_dump(((_tmp471="case",_tag_dyneither(_tmp471,sizeof(char),5))));}
Cyc_Absyndump_dumppat(_tmp1E8->pattern);
if(_tmp1E8->where_clause != 0){
{const char*_tmp472;Cyc_Absyndump_dump(((_tmp472="&&",_tag_dyneither(_tmp472,sizeof(char),3))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1E8->where_clause));}{
# 903
const char*_tmp473;Cyc_Absyndump_dump_nospace(((_tmp473=":",_tag_dyneither(_tmp473,sizeof(char),2))));};}
# 905
Cyc_Absyndump_dumpstmt(_tmp1E8->body,0);}}
# 909
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp1ED=s->r;struct Cyc_Absyn_Exp*_tmp1F0;struct Cyc_Absyn_Stmt*_tmp1F2;struct Cyc_Absyn_Stmt*_tmp1F3;struct Cyc_Absyn_Exp*_tmp1F6;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Stmt*_tmp1F9;struct Cyc_Absyn_Stmt*_tmp1FA;struct Cyc_Absyn_Exp*_tmp1FC;struct Cyc_Absyn_Stmt*_tmp1FD;struct _dyneither_ptr*_tmp201;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_Absyn_Exp*_tmp204;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Stmt*_tmp206;struct Cyc_Absyn_Exp*_tmp208;struct Cyc_List_List*_tmp209;struct Cyc_Absyn_Decl*_tmp20B;struct Cyc_Absyn_Stmt*_tmp20C;struct _dyneither_ptr*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Absyn_Stmt*_tmp211;struct Cyc_Absyn_Exp*_tmp212;struct Cyc_List_List*_tmp215;struct Cyc_Absyn_Stmt*_tmp217;struct Cyc_List_List*_tmp218;struct Cyc_Absyn_Exp*_tmp21A;_LL150: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp1EE=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1EE->tag != 0)goto _LL152;}_LL151:
 Cyc_Absyndump_dump_semi();goto _LL14F;_LL152: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp1EF=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1EF->tag != 1)goto _LL154;else{_tmp1F0=_tmp1EF->f1;}}_LL153:
 Cyc_Absyndump_dumpexp(_tmp1F0);Cyc_Absyndump_dump_semi();goto _LL14F;_LL154: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1F1=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1F1->tag != 2)goto _LL156;else{_tmp1F2=_tmp1F1->f1;_tmp1F3=_tmp1F1->f2;}}_LL155:
# 918
 if(Cyc_Absynpp_is_declaration(_tmp1F2)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1F2,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 921
Cyc_Absyndump_dumpstmt(_tmp1F2,0);}
if(Cyc_Absynpp_is_declaration(_tmp1F3)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1F3,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 928
Cyc_Absyndump_dumpstmt(_tmp1F3,expstmt);}
goto _LL14F;_LL156: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp1F4=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1F4->tag != 3)goto _LL158;else{if(_tmp1F4->f1 != 0)goto _LL158;}}_LL157:
{const char*_tmp474;Cyc_Absyndump_dump(((_tmp474="return;",_tag_dyneither(_tmp474,sizeof(char),8))));}goto _LL14F;_LL158: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp1F5=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1F5->tag != 3)goto _LL15A;else{_tmp1F6=_tmp1F5->f1;}}_LL159:
{const char*_tmp475;Cyc_Absyndump_dump(((_tmp475="return",_tag_dyneither(_tmp475,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1F6));Cyc_Absyndump_dump_semi();goto _LL14F;_LL15A: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp1F7=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1F7->tag != 4)goto _LL15C;else{_tmp1F8=_tmp1F7->f1;_tmp1F9=_tmp1F7->f2;_tmp1FA=_tmp1F7->f3;}}_LL15B:
# 933
{const char*_tmp476;Cyc_Absyndump_dump(((_tmp476="if(",_tag_dyneither(_tmp476,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp1F8);
{void*_tmp21E=_tmp1F9->r;_LL177: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp21F=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp21E;if(_tmp21F->tag != 2)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp220=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp21E;if(_tmp220->tag != 12)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp221=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp21E;if(_tmp221->tag != 4)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp222=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp21E;if(_tmp222->tag != 13)goto _LL17F;}_LL17E:
{const char*_tmp477;Cyc_Absyndump_dump_nospace(((_tmp477="){",_tag_dyneither(_tmp477,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp1F9,0);Cyc_Absyndump_dump_char((int)'}');goto _LL176;_LL17F:;_LL180:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1F9,0);_LL176:;}
# 941
{void*_tmp224=_tmp1FA->r;_LL182: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp225=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp224;if(_tmp225->tag != 0)goto _LL184;}_LL183:
 goto _LL181;_LL184:;_LL185:
{const char*_tmp478;Cyc_Absyndump_dump(((_tmp478="else{",_tag_dyneither(_tmp478,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp1FA,0);Cyc_Absyndump_dump_char((int)'}');goto _LL181;_LL181:;}
# 945
goto _LL14F;_LL15C: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp1FB=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1FB->tag != 5)goto _LL15E;else{_tmp1FC=(_tmp1FB->f1).f1;_tmp1FD=_tmp1FB->f2;}}_LL15D:
# 947
{const char*_tmp479;Cyc_Absyndump_dump(((_tmp479="while(",_tag_dyneither(_tmp479,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp1FC);{const char*_tmp47A;Cyc_Absyndump_dump_nospace(((_tmp47A="){",_tag_dyneither(_tmp47A,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp1FD,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL14F;_LL15E: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp1FE=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1FE->tag != 6)goto _LL160;}_LL15F:
{const char*_tmp47B;Cyc_Absyndump_dump(((_tmp47B="break;",_tag_dyneither(_tmp47B,sizeof(char),7))));}goto _LL14F;_LL160: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp1FF=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp1FF->tag != 7)goto _LL162;}_LL161:
{const char*_tmp47C;Cyc_Absyndump_dump(((_tmp47C="continue;",_tag_dyneither(_tmp47C,sizeof(char),10))));}goto _LL14F;_LL162: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp200=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp200->tag != 8)goto _LL164;else{_tmp201=_tmp200->f1;}}_LL163:
{const char*_tmp47D;Cyc_Absyndump_dump(((_tmp47D="goto",_tag_dyneither(_tmp47D,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp201);Cyc_Absyndump_dump_semi();goto _LL14F;_LL164: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp202=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp202->tag != 9)goto _LL166;else{_tmp203=_tmp202->f1;_tmp204=(_tmp202->f2).f1;_tmp205=(_tmp202->f3).f1;_tmp206=_tmp202->f4;}}_LL165:
# 956
{const char*_tmp47E;Cyc_Absyndump_dump(((_tmp47E="for(",_tag_dyneither(_tmp47E,sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp203);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp204);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp205);
{const char*_tmp47F;Cyc_Absyndump_dump_nospace(((_tmp47F="){",_tag_dyneither(_tmp47F,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp206,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL14F;_LL166: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp207=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp207->tag != 10)goto _LL168;else{_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;}}_LL167:
# 960
{const char*_tmp480;Cyc_Absyndump_dump(((_tmp480="switch(",_tag_dyneither(_tmp480,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp208);{const char*_tmp481;Cyc_Absyndump_dump_nospace(((_tmp481="){",_tag_dyneither(_tmp481,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp209);
Cyc_Absyndump_dump_char((int)'}');
goto _LL14F;_LL168: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp20A=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp20A->tag != 12)goto _LL16A;else{_tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;}}_LL169:
# 965
 Cyc_Absyndump_dumpdecl(_tmp20B);Cyc_Absyndump_dumpstmt(_tmp20C,expstmt);goto _LL14F;_LL16A: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp20D=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp20D->tag != 13)goto _LL16C;else{_tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;}}_LL16B:
# 971
 if(Cyc_Absynpp_is_declaration(_tmp20F)){
Cyc_Absyndump_dump_str(_tmp20E);
if(expstmt){const char*_tmp482;Cyc_Absyndump_dump_nospace(((_tmp482=": ({",_tag_dyneither(_tmp482,sizeof(char),5))));}else{const char*_tmp483;Cyc_Absyndump_dump_nospace(((_tmp483=": {",_tag_dyneither(_tmp483,sizeof(char),4))));}
Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);
if(expstmt){const char*_tmp484;Cyc_Absyndump_dump_nospace(((_tmp484="});}",_tag_dyneither(_tmp484,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 977
Cyc_Absyndump_dump_str(_tmp20E);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp20F,expstmt);}
# 979
goto _LL14F;_LL16C: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp210=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp210->tag != 14)goto _LL16E;else{_tmp211=_tmp210->f1;_tmp212=(_tmp210->f2).f1;}}_LL16D:
# 982
{const char*_tmp485;Cyc_Absyndump_dump(((_tmp485="do{",_tag_dyneither(_tmp485,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp211,0);
{const char*_tmp486;Cyc_Absyndump_dump_nospace(((_tmp486="}while(",_tag_dyneither(_tmp486,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp212);{const char*_tmp487;Cyc_Absyndump_dump_nospace(((_tmp487=");",_tag_dyneither(_tmp487,sizeof(char),3))));}
goto _LL14F;_LL16E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp213=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp213->tag != 11)goto _LL170;else{if(_tmp213->f1 != 0)goto _LL170;}}_LL16F:
# 986
{const char*_tmp488;Cyc_Absyndump_dump(((_tmp488="fallthru;",_tag_dyneither(_tmp488,sizeof(char),10))));}goto _LL14F;_LL170: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp214=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp214->tag != 11)goto _LL172;else{_tmp215=_tmp214->f1;}}_LL171:
# 988
{const char*_tmp489;Cyc_Absyndump_dump(((_tmp489="fallthru(",_tag_dyneither(_tmp489,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(20,_tmp215);{const char*_tmp48A;Cyc_Absyndump_dump_nospace(((_tmp48A=");",_tag_dyneither(_tmp48A,sizeof(char),3))));}goto _LL14F;_LL172: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp216=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp216->tag != 15)goto _LL174;else{_tmp217=_tmp216->f1;_tmp218=_tmp216->f2;}}_LL173:
# 991
{const char*_tmp48B;Cyc_Absyndump_dump(((_tmp48B="try",_tag_dyneither(_tmp48B,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp217,0);
{const char*_tmp48C;Cyc_Absyndump_dump(((_tmp48C="catch{",_tag_dyneither(_tmp48C,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp218);Cyc_Absyndump_dump_char((int)'}');
goto _LL14F;_LL174: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp219=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp1ED;if(_tmp219->tag != 16)goto _LL14F;else{_tmp21A=_tmp219->f1;}}_LL175:
# 997
{const char*_tmp48D;Cyc_Absyndump_dump(((_tmp48D="reset_region(",_tag_dyneither(_tmp48D,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp21A);{const char*_tmp48E;Cyc_Absyndump_dump(((_tmp48E=");",_tag_dyneither(_tmp48E,sizeof(char),3))));}
goto _LL14F;_LL14F:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1003
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
{const char*_tmp491;const char*_tmp490;const char*_tmp48F;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp48F="",_tag_dyneither(_tmp48F,sizeof(char),1))),((_tmp490="=",_tag_dyneither(_tmp490,sizeof(char),2))),((_tmp491="=",_tag_dyneither(_tmp491,sizeof(char),2))));}
Cyc_Absyndump_dumppat((*dp).f2);}
# 1008
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp240=p->r;int _tmp244;int _tmp246;int _tmp248;char _tmp24A;struct _dyneither_ptr _tmp24C;struct Cyc_Absyn_Vardecl*_tmp24E;struct Cyc_Absyn_Vardecl*_tmp251;struct Cyc_Absyn_Pat*_tmp252;struct Cyc_List_List*_tmp254;int _tmp255;struct Cyc_Absyn_Pat*_tmp257;struct Cyc_Absyn_Vardecl*_tmp259;struct Cyc_Absyn_Vardecl*_tmp25C;struct Cyc_Absyn_Pat*_tmp25D;struct Cyc_Absyn_Tvar*_tmp25F;struct Cyc_Absyn_Vardecl*_tmp260;struct _tuple0*_tmp262;struct _tuple0*_tmp264;struct Cyc_List_List*_tmp265;int _tmp266;union Cyc_Absyn_AggrInfoU _tmp268;struct Cyc_List_List*_tmp269;struct Cyc_List_List*_tmp26A;int _tmp26B;struct Cyc_List_List*_tmp26D;struct Cyc_List_List*_tmp26E;int _tmp26F;struct Cyc_Absyn_Datatypefield*_tmp271;struct Cyc_List_List*_tmp272;int _tmp273;struct Cyc_Absyn_Enumfield*_tmp275;struct Cyc_Absyn_Enumfield*_tmp277;struct Cyc_Absyn_Exp*_tmp279;_LL187: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp241=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp241->tag != 0)goto _LL189;}_LL188:
 Cyc_Absyndump_dump_char((int)'_');goto _LL186;_LL189: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp242=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp242->tag != 8)goto _LL18B;}_LL18A:
{const char*_tmp492;Cyc_Absyndump_dump(((_tmp492="NULL",_tag_dyneither(_tmp492,sizeof(char),5))));}goto _LL186;_LL18B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp243=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp243->tag != 9)goto _LL18D;else{if(_tmp243->f1 != Cyc_Absyn_None)goto _LL18D;_tmp244=_tmp243->f2;}}_LL18C:
 _tmp246=_tmp244;goto _LL18E;_LL18D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp245=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp245->tag != 9)goto _LL18F;else{if(_tmp245->f1 != Cyc_Absyn_Signed)goto _LL18F;_tmp246=_tmp245->f2;}}_LL18E:
{const char*_tmp496;void*_tmp495[1];struct Cyc_Int_pa_PrintArg_struct _tmp494;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp494.tag=1,((_tmp494.f1=(unsigned long)_tmp246,((_tmp495[0]=& _tmp494,Cyc_aprintf(((_tmp496="%d",_tag_dyneither(_tmp496,sizeof(char),3))),_tag_dyneither(_tmp495,sizeof(void*),1)))))))));}goto _LL186;_LL18F: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp247=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp247->tag != 9)goto _LL191;else{if(_tmp247->f1 != Cyc_Absyn_Unsigned)goto _LL191;_tmp248=_tmp247->f2;}}_LL190:
{const char*_tmp49A;void*_tmp499[1];struct Cyc_Int_pa_PrintArg_struct _tmp498;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp498.tag=1,((_tmp498.f1=(unsigned int)_tmp248,((_tmp499[0]=& _tmp498,Cyc_aprintf(((_tmp49A="%u",_tag_dyneither(_tmp49A,sizeof(char),3))),_tag_dyneither(_tmp499,sizeof(void*),1)))))))));}goto _LL186;_LL191: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp249=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp249->tag != 10)goto _LL193;else{_tmp24A=_tmp249->f1;}}_LL192:
# 1016
{const char*_tmp49B;Cyc_Absyndump_dump(((_tmp49B="'",_tag_dyneither(_tmp49B,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp24A));{const char*_tmp49C;Cyc_Absyndump_dump_nospace(((_tmp49C="'",_tag_dyneither(_tmp49C,sizeof(char),2))));}goto _LL186;_LL193: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp24B=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp24B->tag != 11)goto _LL195;else{_tmp24C=_tmp24B->f1;}}_LL194:
 Cyc_Absyndump_dump(_tmp24C);goto _LL186;_LL195: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp24D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp24D->tag != 1)goto _LL197;else{_tmp24E=_tmp24D->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp24F=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp24D->f2).r;if(_tmp24F->tag != 0)goto _LL197;};}}_LL196:
 Cyc_Absyndump_dumpqvar(_tmp24E->name);goto _LL186;_LL197: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp250=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp250->tag != 1)goto _LL199;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}_LL198:
 Cyc_Absyndump_dumpqvar(_tmp251->name);{const char*_tmp49D;Cyc_Absyndump_dump(((_tmp49D=" as ",_tag_dyneither(_tmp49D,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp252);goto _LL186;_LL199: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp253=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp253->tag != 4)goto _LL19B;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}_LL19A: {
# 1021
const char*_tmp49F;const char*_tmp49E;struct _dyneither_ptr term=_tmp255?(_tmp49F=", ...)",_tag_dyneither(_tmp49F,sizeof(char),7)):((_tmp49E=")",_tag_dyneither(_tmp49E,sizeof(char),2)));
{const char*_tmp4A1;const char*_tmp4A0;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp254,((_tmp4A0="$(",_tag_dyneither(_tmp4A0,sizeof(char),3))),term,((_tmp4A1=",",_tag_dyneither(_tmp4A1,sizeof(char),2))));}goto _LL186;}_LL19B: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp256=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp256->tag != 5)goto _LL19D;else{_tmp257=_tmp256->f1;}}_LL19C:
{const char*_tmp4A2;Cyc_Absyndump_dump(((_tmp4A2="&",_tag_dyneither(_tmp4A2,sizeof(char),2))));}Cyc_Absyndump_dumppat(_tmp257);goto _LL186;_LL19D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp258=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp258->tag != 2)goto _LL19F;else{_tmp259=_tmp258->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp25A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp258->f2).r;if(_tmp25A->tag != 0)goto _LL19F;};}}_LL19E:
# 1025
{const char*_tmp4A3;Cyc_Absyndump_dump(((_tmp4A3="*",_tag_dyneither(_tmp4A3,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp259->name);goto _LL186;_LL19F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp25B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp25B->tag != 2)goto _LL1A1;else{_tmp25C=_tmp25B->f1;_tmp25D=_tmp25B->f2;}}_LL1A0:
# 1027
{const char*_tmp4A4;Cyc_Absyndump_dump(((_tmp4A4="*",_tag_dyneither(_tmp4A4,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp25C->name);{const char*_tmp4A5;Cyc_Absyndump_dump(((_tmp4A5=" as ",_tag_dyneither(_tmp4A5,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp25D);goto _LL186;_LL1A1: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp25E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp25E->tag != 3)goto _LL1A3;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL1A2:
# 1029
 Cyc_Absyndump_dumpqvar(_tmp260->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp25F);Cyc_Absyndump_dump_char((int)'>');goto _LL186;_LL1A3: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp261=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp261->tag != 14)goto _LL1A5;else{_tmp262=_tmp261->f1;}}_LL1A4:
 Cyc_Absyndump_dumpqvar(_tmp262);goto _LL186;_LL1A5: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp263=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp263->tag != 15)goto _LL1A7;else{_tmp264=_tmp263->f1;_tmp265=_tmp263->f2;_tmp266=_tmp263->f3;}}_LL1A6: {
# 1032
const char*_tmp4A7;const char*_tmp4A6;struct _dyneither_ptr term=_tmp266?(_tmp4A7=", ...)",_tag_dyneither(_tmp4A7,sizeof(char),7)):((_tmp4A6=")",_tag_dyneither(_tmp4A6,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp264);{const char*_tmp4A9;const char*_tmp4A8;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp265,((_tmp4A8="(",_tag_dyneither(_tmp4A8,sizeof(char),2))),term,((_tmp4A9=",",_tag_dyneither(_tmp4A9,sizeof(char),2))));}goto _LL186;}_LL1A7: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp267=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp267->tag != 6)goto _LL1A9;else{if(_tmp267->f1 == 0)goto _LL1A9;_tmp268=(*_tmp267->f1).aggr_info;_tmp269=_tmp267->f2;_tmp26A=_tmp267->f3;_tmp26B=_tmp267->f4;}}_LL1A8: {
# 1035
struct _tuple0*_tmp291;struct _tuple10 _tmp290=Cyc_Absyn_aggr_kinded_name(_tmp268);_tmp291=_tmp290.f2;{
const char*_tmp4AB;const char*_tmp4AA;struct _dyneither_ptr term=_tmp26B?(_tmp4AB=", ...)",_tag_dyneither(_tmp4AB,sizeof(char),7)):((_tmp4AA=")",_tag_dyneither(_tmp4AA,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp291);Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4AE;const char*_tmp4AD;const char*_tmp4AC;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp269,((_tmp4AC="[",_tag_dyneither(_tmp4AC,sizeof(char),2))),((_tmp4AD="]",_tag_dyneither(_tmp4AD,sizeof(char),2))),((_tmp4AE=",",_tag_dyneither(_tmp4AE,sizeof(char),2))));}
{const char*_tmp4B0;const char*_tmp4AF;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp26A,((_tmp4AF="",_tag_dyneither(_tmp4AF,sizeof(char),1))),term,((_tmp4B0=",",_tag_dyneither(_tmp4B0,sizeof(char),2))));}
goto _LL186;};}_LL1A9: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp26C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp26C->tag != 6)goto _LL1AB;else{if(_tmp26C->f1 != 0)goto _LL1AB;_tmp26D=_tmp26C->f2;_tmp26E=_tmp26C->f3;_tmp26F=_tmp26C->f4;}}_LL1AA: {
# 1042
const char*_tmp4B2;const char*_tmp4B1;struct _dyneither_ptr term=_tmp26F?(_tmp4B2=", ...)",_tag_dyneither(_tmp4B2,sizeof(char),7)):((_tmp4B1=")",_tag_dyneither(_tmp4B1,sizeof(char),2)));
Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4B5;const char*_tmp4B4;const char*_tmp4B3;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp26D,((_tmp4B3="[",_tag_dyneither(_tmp4B3,sizeof(char),2))),((_tmp4B4="]",_tag_dyneither(_tmp4B4,sizeof(char),2))),((_tmp4B5=",",_tag_dyneither(_tmp4B5,sizeof(char),2))));}
{const char*_tmp4B7;const char*_tmp4B6;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp26E,((_tmp4B6="",_tag_dyneither(_tmp4B6,sizeof(char),1))),term,((_tmp4B7=",",_tag_dyneither(_tmp4B7,sizeof(char),2))));}
goto _LL186;}_LL1AB: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp270=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp270->tag != 7)goto _LL1AD;else{_tmp271=_tmp270->f2;_tmp272=_tmp270->f3;_tmp273=_tmp270->f4;}}_LL1AC: {
# 1048
const char*_tmp4B9;const char*_tmp4B8;struct _dyneither_ptr term=_tmp273?(_tmp4B9=", ...)",_tag_dyneither(_tmp4B9,sizeof(char),7)):((_tmp4B8=")",_tag_dyneither(_tmp4B8,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp271->name);
if(_tmp272 != 0){const char*_tmp4BB;const char*_tmp4BA;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp272,((_tmp4BA="(",_tag_dyneither(_tmp4BA,sizeof(char),2))),term,((_tmp4BB=",",_tag_dyneither(_tmp4BB,sizeof(char),2))));}
goto _LL186;}_LL1AD: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp274=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp274->tag != 12)goto _LL1AF;else{_tmp275=_tmp274->f2;}}_LL1AE:
 _tmp277=_tmp275;goto _LL1B0;_LL1AF: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp276=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp276->tag != 13)goto _LL1B1;else{_tmp277=_tmp276->f2;}}_LL1B0:
 Cyc_Absyndump_dumpqvar(_tmp277->name);goto _LL186;_LL1B1: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp278=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp240;if(_tmp278->tag != 16)goto _LL186;else{_tmp279=_tmp278->f1;}}_LL1B2:
 Cyc_Absyndump_dumpexp(_tmp279);goto _LL186;_LL186:;}
# 1058
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1063
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
const char*_tmp4BC;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,((_tmp4BC=",",_tag_dyneither(_tmp4BC,sizeof(char),2))));}
# 1066
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
{const char*_tmp4BD;Cyc_Absyndump_dump(((_tmp4BD=" = ",_tag_dyneither(_tmp4BD,sizeof(char),4))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1073
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
const char*_tmp4BE;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((_tmp4BE=",",_tag_dyneither(_tmp4BE,sizeof(char),2))));}
# 1077
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct _dyneither_ptr*_tmp2A8;struct Cyc_Absyn_Tqual _tmp2A9;void*_tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;struct Cyc_List_List*_tmp2AC;struct Cyc_Absyn_Aggrfield*_tmp2A7=(struct Cyc_Absyn_Aggrfield*)fields->hd;_tmp2A8=(*_tmp2A7).name;_tmp2A9=(*_tmp2A7).tq;_tmp2AA=(*_tmp2A7).type;_tmp2AB=(*_tmp2A7).width;_tmp2AC=(*_tmp2A7).attributes;
# 1081
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1B3:
# 1083
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2A9,_tmp2AA,Cyc_Absyndump_dump_str,_tmp2A8);
Cyc_Absyndump_dumpatts(_tmp2AC);
break;case Cyc_Cyclone_Vc_c: _LL1B4:
# 1087
 Cyc_Absyndump_dumpatts(_tmp2AC);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2A9,_tmp2AA,Cyc_Absyndump_dump_str,_tmp2A8);
break;}
# 1092
if(_tmp2AB != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2AB);}
# 1096
Cyc_Absyndump_dump_semi();}}
# 1100
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1105
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1109
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp2AD=(*pr).f1;_LL1B7: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2AE=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2AD;if(_tmp2AE->tag != 11)goto _LL1B9;}_LL1B8:
 goto _LL1B6;_LL1B9: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2AF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2AD;if(_tmp2AF->tag != 1)goto _LL1BB;}_LL1BA:
{const char*_tmp4BF;Cyc_Absyndump_dump(((_tmp4BF="_stdcall",_tag_dyneither(_tmp4BF,sizeof(char),9))));}goto _LL1B6;_LL1BB: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2B0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2AD;if(_tmp2B0->tag != 2)goto _LL1BD;}_LL1BC:
{const char*_tmp4C0;Cyc_Absyndump_dump(((_tmp4C0="_cdecl",_tag_dyneither(_tmp4C0,sizeof(char),7))));}goto _LL1B6;_LL1BD: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2B1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2AD;if(_tmp2B1->tag != 3)goto _LL1BF;}_LL1BE:
{const char*_tmp4C1;Cyc_Absyndump_dump(((_tmp4C1="_fastcall",_tag_dyneither(_tmp4C1,sizeof(char),10))));}goto _LL1B6;_LL1BF:;_LL1C0:
 goto _LL1B6;_LL1B6:;}
# 1117
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1119
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1124
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1131
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1133
void*_tmp2B5=t;void*_tmp2B7;void*_tmp2B9;_LL1C2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2B6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B5;if(_tmp2B6->tag != 1)goto _LL1C4;else{_tmp2B7=(void*)_tmp2B6->f2;}}if(!(_tmp2B7 != 0))goto _LL1C4;_LL1C3:
 return Cyc_Absyndump_is_char_ptr((void*)_tmp2B7);_LL1C4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2B8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B5;if(_tmp2B8->tag != 5)goto _LL1C6;else{_tmp2B9=(_tmp2B8->f1).elt_typ;}}_LL1C5:
# 1136
 L: {
void*_tmp2BA=_tmp2B9;void*_tmp2BC;void*_tmp2BE;_LL1C9:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2BB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BA;if(_tmp2BB->tag != 1)goto _LL1CB;else{_tmp2BC=(void*)_tmp2BB->f2;}}if(!(_tmp2BC != 0))goto _LL1CB;_LL1CA:
 _tmp2B9=(void*)_tmp2BC;goto L;_LL1CB:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2BD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA;if(_tmp2BD->tag != 17)goto _LL1CD;else{_tmp2BE=(void*)_tmp2BD->f4;}}if(!(_tmp2BE != 0))goto _LL1CD;_LL1CC:
 _tmp2B9=(void*)_tmp2BE;goto L;_LL1CD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2BF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2BA;if(_tmp2BF->tag != 6)goto _LL1CF;else{if(_tmp2BF->f2 != Cyc_Absyn_Char_sz)goto _LL1CF;}}_LL1CE:
 return 1;_LL1CF:;_LL1D0:
 return 0;_LL1C8:;}_LL1C6:;_LL1C7:
# 1143
 return 0;_LL1C1:;}
# 1147
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
enum Cyc_Absyn_Scope _tmp2C1;struct _tuple0*_tmp2C2;struct Cyc_Absyn_Tqual _tmp2C3;void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_List_List*_tmp2C6;struct Cyc_Absyn_Vardecl*_tmp2C0=vd;_tmp2C1=(*_tmp2C0).sc;_tmp2C2=(*_tmp2C0).name;_tmp2C3=(*_tmp2C0).tq;_tmp2C4=(*_tmp2C0).type;_tmp2C5=(*_tmp2C0).initializer;_tmp2C6=(*_tmp2C0).attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1D1:
# 1152
 if(_tmp2C1 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp2C7=Cyc_Tcutil_compress(_tmp2C4);_LL1D4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2C7;if(_tmp2C8->tag != 9)goto _LL1D6;}_LL1D5:
 goto _LL1D3;_LL1D6:;_LL1D7:
 Cyc_Absyndump_dumpscope(_tmp2C1);_LL1D3:;}else{
# 1158
Cyc_Absyndump_dumpscope(_tmp2C1);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2C3,_tmp2C4,Cyc_Absyndump_dumpqvar,_tmp2C2);
Cyc_Absyndump_dumpatts(_tmp2C6);
break;case Cyc_Cyclone_Vc_c: _LL1D2:
# 1163
 Cyc_Absyndump_dumpatts(_tmp2C6);
Cyc_Absyndump_dumpscope(_tmp2C1);{
struct _RegionHandle _tmp2C9=_new_region("temp");struct _RegionHandle*temp=& _tmp2C9;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp2C4);
struct Cyc_Absyn_Tqual _tmp2CB;void*_tmp2CC;struct Cyc_List_List*_tmp2CD;struct _tuple11 _tmp2CA=Cyc_Absynpp_to_tms(temp,_tmp2C3,_tmp2C4);_tmp2CB=_tmp2CA.f1;_tmp2CC=_tmp2CA.f2;_tmp2CD=_tmp2CA.f3;{
# 1169
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp2CD;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp2CE=(void*)tms2->hd;struct Cyc_List_List*_tmp2D0;_LL1DA: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp2CF=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2CE;if(_tmp2CF->tag != 5)goto _LL1DC;else{_tmp2D0=_tmp2CF->f2;}}_LL1DB:
# 1173
 for(0;_tmp2D0 != 0;_tmp2D0=_tmp2D0->tl){
void*_tmp2D1=(void*)_tmp2D0->hd;_LL1DF: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2D2=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2D1;if(_tmp2D2->tag != 1)goto _LL1E1;}_LL1E0:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1DE;_LL1E1: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2D3=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2D1;if(_tmp2D3->tag != 2)goto _LL1E3;}_LL1E2:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL1DE;_LL1E3: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2D4=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2D1;if(_tmp2D4->tag != 3)goto _LL1E5;}_LL1E4:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL1DE;_LL1E5:;_LL1E6:
 goto _LL1DE;_LL1DE:;}
# 1180
goto _LL1D9;_LL1DC:;_LL1DD:
 goto _LL1D9;_LL1D9:;}}
# 1183
Cyc_Absyndump_dumptq(_tmp2CB);
Cyc_Absyndump_dumpntyp(_tmp2CC);{
struct _tuple19 _tmp4C2;struct _tuple19 _tmp2D5=(_tmp4C2.f1=call_conv,((_tmp4C2.f2=_tmp2C2,_tmp4C2)));
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2CD),Cyc_Absyndump_dump_callconv_qvar,& _tmp2D5);};};}
# 1188
_npop_handler(0);break;
# 1165
;_pop_region(temp);};}
# 1191
if(_tmp2C5 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2C5);}
# 1195
Cyc_Absyndump_dump_semi();}
# 1198
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
const char*_tmp4C3;Cyc_Absyndump_dump(((_tmp4C3="@tagged ",_tag_dyneither(_tmp4C3,sizeof(char),9))));}
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1207
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
const char*_tmp4C6;const char*_tmp4C5;const char*_tmp4C4;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp4C4="<",_tag_dyneither(_tmp4C4,sizeof(char),2))),((_tmp4C5=">",_tag_dyneither(_tmp4C5,sizeof(char),2))),((_tmp4C6=",",_tag_dyneither(_tmp4C6,sizeof(char),2))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1214
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
{const char*_tmp4C7;Cyc_Absyndump_dump(((_tmp4C7="}",_tag_dyneither(_tmp4C7,sizeof(char),2))));}
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1219
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp2DD;struct _tuple0*_tmp2DE;struct Cyc_Core_Opt*_tmp2DF;struct Cyc_Absyn_Enumdecl*_tmp2DC=ed;_tmp2DD=(*_tmp2DC).sc;_tmp2DE=(*_tmp2DC).name;_tmp2DF=(*_tmp2DC).fields;
Cyc_Absyndump_dumpscope(_tmp2DD);
{const char*_tmp4C8;Cyc_Absyndump_dump(((_tmp4C8="enum ",_tag_dyneither(_tmp4C8,sizeof(char),6))));}
Cyc_Absyndump_dumpqvar(_tmp2DE);
if(_tmp2DF != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp2DF->v);{
const char*_tmp4C9;Cyc_Absyndump_dump_nospace(((_tmp4C9="}",_tag_dyneither(_tmp4C9,sizeof(char),2))));};}}
# 1230
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp2E3;struct _tuple0*_tmp2E4;struct Cyc_List_List*_tmp2E5;struct Cyc_Core_Opt*_tmp2E6;int _tmp2E7;struct Cyc_Absyn_Datatypedecl*_tmp2E2=dd;_tmp2E3=(*_tmp2E2).sc;_tmp2E4=(*_tmp2E2).name;_tmp2E5=(*_tmp2E2).tvs;_tmp2E6=(*_tmp2E2).fields;_tmp2E7=(*_tmp2E2).is_extensible;
Cyc_Absyndump_dumpscope(_tmp2E3);
if(_tmp2E7){const char*_tmp4CA;Cyc_Absyndump_dump(((_tmp4CA="@extensible ",_tag_dyneither(_tmp4CA,sizeof(char),13))));}
{const char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB="datatype ",_tag_dyneither(_tmp4CB,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmp2E4);
Cyc_Absyndump_dumptvars(_tmp2E5);
if(_tmp2E6 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp2E6->v);{
const char*_tmp4CC;Cyc_Absyndump_dump_nospace(((_tmp4CC="}",_tag_dyneither(_tmp4CC,sizeof(char),2))));};}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1244
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp2EB=d->r;struct Cyc_Absyn_Vardecl*_tmp2ED;struct Cyc_Absyn_Fndecl*_tmp2EF;struct Cyc_Absyn_Aggrdecl*_tmp2F1;struct Cyc_Absyn_Datatypedecl*_tmp2F3;struct Cyc_Absyn_Enumdecl*_tmp2F5;struct Cyc_Absyn_Pat*_tmp2F7;struct Cyc_Absyn_Exp*_tmp2F8;struct Cyc_List_List*_tmp2FA;struct Cyc_Absyn_Tvar*_tmp2FC;struct Cyc_Absyn_Vardecl*_tmp2FD;int _tmp2FE;struct Cyc_Absyn_Exp*_tmp2FF;struct Cyc_Absyn_Exp*_tmp301;struct Cyc_Absyn_Tvar*_tmp302;struct Cyc_Absyn_Vardecl*_tmp303;struct Cyc_Absyn_Typedefdecl*_tmp305;struct _dyneither_ptr*_tmp307;struct Cyc_List_List*_tmp308;struct _tuple0*_tmp30A;struct Cyc_List_List*_tmp30B;struct Cyc_List_List*_tmp30D;struct Cyc_List_List*_tmp30F;struct Cyc_List_List*_tmp310;_LL1E8: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2EC=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2EC->tag != 0)goto _LL1EA;else{_tmp2ED=_tmp2EC->f1;}}_LL1E9:
# 1248
 Cyc_Absyndump_dumpvardecl(_tmp2ED,d->loc);
goto _LL1E7;_LL1EA: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp2EE=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2EE->tag != 1)goto _LL1EC;else{_tmp2EF=_tmp2EE->f1;}}_LL1EB:
# 1252
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL208:
 Cyc_Absyndump_dumpatts(_tmp2EF->attributes);break;case Cyc_Cyclone_Gcc_c: _LL209:
 break;}
# 1256
if(_tmp2EF->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp313=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp314;_LL20C: if(_tmp313 != Cyc_Cyclone_Vc_c)goto _LL20E;_LL20D:
{const char*_tmp4CD;Cyc_Absyndump_dump(((_tmp4CD="__inline",_tag_dyneither(_tmp4CD,sizeof(char),9))));}goto _LL20B;_LL20E: _tmp314=_tmp313;_LL20F:
{const char*_tmp4CE;Cyc_Absyndump_dump(((_tmp4CE="inline",_tag_dyneither(_tmp4CE,sizeof(char),7))));}goto _LL20B;_LL20B:;}
# 1261
Cyc_Absyndump_dumpscope(_tmp2EF->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp4D4;struct Cyc_Absyn_FnInfo _tmp4D3;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D2;void*t=(void*)((_tmp4D2=_cycalloc(sizeof(*_tmp4D2)),((_tmp4D2[0]=((_tmp4D4.tag=9,((_tmp4D4.f1=((_tmp4D3.tvars=_tmp2EF->tvs,((_tmp4D3.effect=_tmp2EF->effect,((_tmp4D3.ret_tqual=_tmp2EF->ret_tqual,((_tmp4D3.ret_typ=_tmp2EF->ret_type,((_tmp4D3.args=
# 1264
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2EF->args),((_tmp4D3.c_varargs=_tmp2EF->c_varargs,((_tmp4D3.cyc_varargs=_tmp2EF->cyc_varargs,((_tmp4D3.rgn_po=_tmp2EF->rgn_po,((_tmp4D3.attributes=0,_tmp4D3)))))))))))))))))),_tmp4D4)))),_tmp4D2))));
# 1267
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL210:
# 1269
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp2EF);
break;case Cyc_Cyclone_Vc_c: _LL211:
# 1272
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp2EF);
break;}
# 1275
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp2EF->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1E7;};_LL1EC: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp2F0=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2F0->tag != 6)goto _LL1EE;else{_tmp2F1=_tmp2F0->f1;}}_LL1ED:
# 1280
 Cyc_Absyndump_dump_aggrdecl(_tmp2F1);Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1EE: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2F2=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2F2->tag != 7)goto _LL1F0;else{_tmp2F3=_tmp2F2->f1;}}_LL1EF:
# 1283
 Cyc_Absyndump_dump_datatypedecl(_tmp2F3);Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1F0: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp2F4=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2F4->tag != 8)goto _LL1F2;else{_tmp2F5=_tmp2F4->f1;}}_LL1F1:
# 1286
 Cyc_Absyndump_dump_enumdecl(_tmp2F5);Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1F2: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp2F6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2F6->tag != 2)goto _LL1F4;else{_tmp2F7=_tmp2F6->f1;_tmp2F8=_tmp2F6->f3;}}_LL1F3:
# 1289
{const char*_tmp4D5;Cyc_Absyndump_dump(((_tmp4D5="let",_tag_dyneither(_tmp4D5,sizeof(char),4))));}
Cyc_Absyndump_dumppat(_tmp2F7);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp2F8);
Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1F4: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp2F9=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2F9->tag != 3)goto _LL1F6;else{_tmp2FA=_tmp2F9->f1;}}_LL1F5:
# 1296
{const char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6="let ",_tag_dyneither(_tmp4D6,sizeof(char),5))));}
Cyc_Absyndump_dumpids(_tmp2FA);
Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1F6: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp2FB=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp2FB->tag != 4)goto _LL1F8;else{_tmp2FC=_tmp2FB->f1;_tmp2FD=_tmp2FB->f2;_tmp2FE=_tmp2FB->f3;_tmp2FF=_tmp2FB->f4;}}_LL1F7:
# 1301
{const char*_tmp4D7;Cyc_Absyndump_dump(((_tmp4D7="region",_tag_dyneither(_tmp4D7,sizeof(char),7))));}
{const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="<",_tag_dyneither(_tmp4D8,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp2FC);{const char*_tmp4D9;Cyc_Absyndump_dump(((_tmp4D9="> ",_tag_dyneither(_tmp4D9,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp2FD->name);
if(_tmp2FE){const char*_tmp4DA;Cyc_Absyndump_dump(((_tmp4DA="@resetable",_tag_dyneither(_tmp4DA,sizeof(char),11))));}
if(_tmp2FF != 0){
{const char*_tmp4DB;Cyc_Absyndump_dump(((_tmp4DB=" = open(",_tag_dyneither(_tmp4DB,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_tmp2FF);{
const char*_tmp4DC;Cyc_Absyndump_dump(((_tmp4DC=")",_tag_dyneither(_tmp4DC,sizeof(char),2))));};}
# 1309
Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1F8: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp300=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp300->tag != 5)goto _LL1FA;else{_tmp301=_tmp300->f1;_tmp302=_tmp300->f2;_tmp303=_tmp300->f3;}}_LL1F9:
# 1312
{const char*_tmp4DD;Cyc_Absyndump_dump(((_tmp4DD="alias ",_tag_dyneither(_tmp4DD,sizeof(char),7))));}
{const char*_tmp4DE;Cyc_Absyndump_dump(((_tmp4DE="<",_tag_dyneither(_tmp4DE,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp302);{const char*_tmp4DF;Cyc_Absyndump_dump(((_tmp4DF=">",_tag_dyneither(_tmp4DF,sizeof(char),2))));}
Cyc_Absyndump_dumpqvar(_tmp303->name);{const char*_tmp4E0;Cyc_Absyndump_dump(((_tmp4E0=" = ",_tag_dyneither(_tmp4E0,sizeof(char),4))));}
Cyc_Absyndump_dumpexp(_tmp301);
Cyc_Absyndump_dump_semi();
goto _LL1E7;_LL1FA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp304=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp304->tag != 9)goto _LL1FC;else{_tmp305=_tmp304->f1;}}_LL1FB:
# 1323
{const char*_tmp4E1;Cyc_Absyndump_dump(((_tmp4E1="typedef",_tag_dyneither(_tmp4E1,sizeof(char),8))));}{
void*t;
if(_tmp305->defn == 0)
t=Cyc_Absyn_new_evar(_tmp305->kind,0);else{
# 1328
t=(void*)_check_null(_tmp305->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp305->tq,t,Cyc_Absyndump_dumptypedefname,_tmp305);
Cyc_Absyndump_dumpatts(_tmp305->atts);
Cyc_Absyndump_dump_semi();
# 1333
goto _LL1E7;};_LL1FC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp306=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp306->tag != 10)goto _LL1FE;else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;}}_LL1FD:
# 1335
{const char*_tmp4E2;Cyc_Absyndump_dump(((_tmp4E2="namespace ",_tag_dyneither(_tmp4E2,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp307);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp308 != 0;_tmp308=_tmp308->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp308->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1E7;_LL1FE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp309=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp309->tag != 11)goto _LL200;else{_tmp30A=_tmp309->f1;_tmp30B=_tmp309->f2;}}_LL1FF:
# 1343
{const char*_tmp4E3;Cyc_Absyndump_dump(((_tmp4E3="using ",_tag_dyneither(_tmp4E3,sizeof(char),7))));}
Cyc_Absyndump_dumpqvar(_tmp30A);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp30B != 0;_tmp30B=_tmp30B->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp30B->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1E7;_LL200: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp30C=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp30C->tag != 12)goto _LL202;else{_tmp30D=_tmp30C->f1;}}_LL201:
# 1351
{const char*_tmp4E4;Cyc_Absyndump_dump(((_tmp4E4="extern \"C\" {",_tag_dyneither(_tmp4E4,sizeof(char),13))));}
for(0;_tmp30D != 0;_tmp30D=_tmp30D->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp30D->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1E7;_LL202: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp30E=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp30E->tag != 13)goto _LL204;else{_tmp30F=_tmp30E->f1;_tmp310=_tmp30E->f2;}}_LL203:
# 1357
{const char*_tmp4E5;Cyc_Absyndump_dump(((_tmp4E5="extern \"C include\" {",_tag_dyneither(_tmp4E5,sizeof(char),21))));}
for(0;_tmp30F != 0;_tmp30F=_tmp30F->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp30F->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp310 != 0){
{const char*_tmp4E6;Cyc_Absyndump_dump(((_tmp4E6=" export {",_tag_dyneither(_tmp4E6,sizeof(char),10))));}
for(0;_tmp310 != 0;_tmp310=_tmp310->tl){
struct _tuple0*_tmp32D;struct _tuple20 _tmp32C=*((struct _tuple20*)_tmp310->hd);_tmp32D=_tmp32C.f2;
Cyc_Absyndump_dumpqvar(_tmp32D);
if(_tmp310->tl != 0)Cyc_Absyndump_dump_char((int)',');}{
# 1368
const char*_tmp4E7;Cyc_Absyndump_dump(((_tmp4E7="}",_tag_dyneither(_tmp4E7,sizeof(char),2))));};}
# 1370
goto _LL1E7;_LL204: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp311=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp311->tag != 14)goto _LL206;}_LL205:
# 1372
{const char*_tmp4E8;Cyc_Absyndump_dump(((_tmp4E8=" __cyclone_port_on__; ",_tag_dyneither(_tmp4E8,sizeof(char),23))));}
goto _LL1E7;_LL206: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp312=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp2EB;if(_tmp312->tag != 15)goto _LL1E7;}_LL207:
# 1375
{const char*_tmp4E9;Cyc_Absyndump_dump(((_tmp4E9=" __cyclone_port_off__; ",_tag_dyneither(_tmp4E9,sizeof(char),24))));}
goto _LL1E7;_LL1E7:;};}
# 1380
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1387
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1392
void*_tmp331=(void*)tms->hd;void*_tmp333;union Cyc_Absyn_Constraint*_tmp334;union Cyc_Absyn_Constraint*_tmp335;union Cyc_Absyn_Constraint*_tmp336;struct Cyc_Absyn_Tqual _tmp337;_LL214: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp332=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp331;if(_tmp332->tag != 2)goto _LL216;else{_tmp333=(_tmp332->f1).rgn;_tmp334=(_tmp332->f1).nullable;_tmp335=(_tmp332->f1).bounds;_tmp336=(_tmp332->f1).zero_term;_tmp337=_tmp332->f2;}}_LL215:
# 1396
{void*_tmp338=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp335);struct Cyc_Absyn_Exp*_tmp33B;_LL219: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp339=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp338;if(_tmp339->tag != 0)goto _LL21B;}_LL21A:
 Cyc_Absyndump_dump_char((int)'?');goto _LL218;_LL21B: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp33A=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp338;if(_tmp33A->tag != 1)goto _LL218;else{_tmp33B=_tmp33A->f1;}}_LL21C:
# 1399
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp334)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp33B);
goto _LL218;_LL218:;}
# 1403
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp336)){const char*_tmp4EA;Cyc_Absyndump_dump(((_tmp4EA="@zeroterm",_tag_dyneither(_tmp4EA,sizeof(char),10))));}
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp336)){const char*_tmp4EB;Cyc_Absyndump_dump(((_tmp4EB="@nozeroterm",_tag_dyneither(_tmp4EB,sizeof(char),12))));}
{void*_tmp33E=Cyc_Tcutil_compress(_tmp333);struct Cyc_Absyn_Tvar*_tmp343;_LL21E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp33F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp33E;if(_tmp33F->tag != 20)goto _LL220;}_LL21F:
 if(!Cyc_Absyndump_qvar_to_Cids){const char*_tmp4EC;Cyc_Absyndump_dump(((_tmp4EC="`H",_tag_dyneither(_tmp4EC,sizeof(char),3))));}goto _LL21D;_LL220: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp340=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp33E;if(_tmp340->tag != 21)goto _LL222;}_LL221:
{const char*_tmp4ED;Cyc_Absyndump_dump(((_tmp4ED="`U",_tag_dyneither(_tmp4ED,sizeof(char),3))));}goto _LL21D;_LL222: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp341=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp33E;if(_tmp341->tag != 22)goto _LL224;}_LL223:
{const char*_tmp4EE;Cyc_Absyndump_dump(((_tmp4EE="`RC",_tag_dyneither(_tmp4EE,sizeof(char),4))));}goto _LL21D;_LL224: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp342=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp33E;if(_tmp342->tag != 2)goto _LL226;else{_tmp343=_tmp342->f1;}}_LL225:
 Cyc_Absyndump_dumptvar(_tmp343);goto _LL21D;_LL226: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp344=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp33E;if(_tmp344->tag != 1)goto _LL228;else{if((void*)_tmp344->f2 != 0)goto _LL228;}}_LL227:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp333));goto _LL21D;_LL228: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp345=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp33E;if(_tmp345->tag != 17)goto _LL22A;}_LL229:
{const char*_tmp4EF;Cyc_Absyndump_dump(((_tmp4EF="@region(",_tag_dyneither(_tmp4EF,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp333);{const char*_tmp4F0;Cyc_Absyndump_dump(((_tmp4F0=")",_tag_dyneither(_tmp4F0,sizeof(char),2))));}goto _LL21D;_LL22A:;_LL22B: {
const char*_tmp4F3;void*_tmp4F2;(_tmp4F2=0,Cyc_Tcutil_impos(((_tmp4F3="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4F3,sizeof(char),37))),_tag_dyneither(_tmp4F2,sizeof(void*),0)));}_LL21D:;}
# 1414
Cyc_Absyndump_dumptq(_tmp337);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;_LL216:;_LL217: {
# 1419
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp34D=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;_LL22D: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp34E=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp34D;if(_tmp34E->tag != 2)goto _LL22F;}_LL22E:
 next_is_pointer=1;goto _LL22C;_LL22F:;_LL230:
 goto _LL22C;_LL22C:;}
# 1425
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp34F=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmp351;struct Cyc_Absyn_Exp*_tmp353;union Cyc_Absyn_Constraint*_tmp354;struct Cyc_List_List*_tmp357;int _tmp358;struct Cyc_Absyn_VarargInfo*_tmp359;void*_tmp35A;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35E;unsigned int _tmp35F;struct Cyc_List_List*_tmp361;unsigned int _tmp362;int _tmp363;struct Cyc_List_List*_tmp365;_LL232: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp350=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp350->tag != 0)goto _LL234;else{_tmp351=_tmp350->f1;}}_LL233:
# 1432
{const char*_tmp4F4;Cyc_Absyndump_dump(((_tmp4F4="[]",_tag_dyneither(_tmp4F4,sizeof(char),3))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp351)){const char*_tmp4F5;Cyc_Absyndump_dump(((_tmp4F5="@zeroterm",_tag_dyneither(_tmp4F5,sizeof(char),10))));}
goto _LL231;_LL234: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp352=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp352->tag != 1)goto _LL236;else{_tmp353=_tmp352->f1;_tmp354=_tmp352->f2;}}_LL235:
# 1436
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp353);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp354)){const char*_tmp4F6;Cyc_Absyndump_dump(((_tmp4F6="@zeroterm",_tag_dyneither(_tmp4F6,sizeof(char),10))));}
goto _LL231;_LL236: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp355=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp355->tag != 3)goto _LL238;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp356=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmp355->f1);if(_tmp356->tag != 1)goto _LL238;else{_tmp357=_tmp356->f1;_tmp358=_tmp356->f2;_tmp359=_tmp356->f3;_tmp35A=(void*)_tmp356->f4;_tmp35B=_tmp356->f5;}}}_LL237:
# 1440
 Cyc_Absyndump_dumpfunargs(_tmp357,_tmp358,_tmp359,_tmp35A,_tmp35B);goto _LL231;_LL238: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp35C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp35C->tag != 3)goto _LL23A;else{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp35D=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((void*)_tmp35C->f1);if(_tmp35D->tag != 0)goto _LL23A;else{_tmp35E=_tmp35D->f1;_tmp35F=_tmp35D->f2;}}}_LL239:
# 1442
{const char*_tmp4F9;const char*_tmp4F8;const char*_tmp4F7;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp35E,((_tmp4F7="(",_tag_dyneither(_tmp4F7,sizeof(char),2))),((_tmp4F8=")",_tag_dyneither(_tmp4F8,sizeof(char),2))),((_tmp4F9=",",_tag_dyneither(_tmp4F9,sizeof(char),2))));}goto _LL231;_LL23A: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp360=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp360->tag != 4)goto _LL23C;else{_tmp361=_tmp360->f1;_tmp362=_tmp360->f2;_tmp363=_tmp360->f3;}}_LL23B:
# 1444
 if(_tmp363)Cyc_Absyndump_dumpkindedtvars(_tmp361);else{Cyc_Absyndump_dumptvars(_tmp361);}goto _LL231;_LL23C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp364=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp364->tag != 5)goto _LL23E;else{_tmp365=_tmp364->f2;}}_LL23D:
 Cyc_Absyndump_dumpatts(_tmp365);goto _LL231;_LL23E: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp366=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp34F;if(_tmp366->tag != 2)goto _LL231;}_LL23F: {
const char*_tmp4FC;void*_tmp4FB;(_tmp4FB=0,Cyc_Tcutil_impos(((_tmp4FC="dumptms",_tag_dyneither(_tmp4FC,sizeof(char),8))),_tag_dyneither(_tmp4FB,sizeof(void*),0)));}_LL231:;}
# 1448
return;}_LL213:;};}
# 1452
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp36F=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp370=_new_region("temp");struct _RegionHandle*temp=& _tmp370;_push_region(temp);
{struct Cyc_Absyn_Tqual _tmp372;void*_tmp373;struct Cyc_List_List*_tmp374;struct _tuple11 _tmp371=Cyc_Absynpp_to_tms(temp,tq,t);_tmp372=_tmp371.f1;_tmp373=_tmp371.f2;_tmp374=_tmp371.f3;
Cyc_Absyndump_dumptq(_tmp372);
Cyc_Absyndump_dumpntyp(_tmp373);
Cyc_Absyndump_dumptms(_tmp36F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp374),f,a);}
# 1455
;_pop_region(temp);}
# 1462
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1464
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp4FF;void*_tmp4FE;(_tmp4FE=0,Cyc_fprintf(f,((_tmp4FF="\n",_tag_dyneither(_tmp4FF,sizeof(char),2))),_tag_dyneither(_tmp4FE,sizeof(void*),0)));};}
# 1470
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp377=d->r;struct Cyc_Absyn_Vardecl*_tmp379;struct Cyc_Absyn_Fndecl*_tmp37B;struct Cyc_Absyn_Aggrdecl*_tmp37D;struct Cyc_Absyn_Datatypedecl*_tmp37F;struct Cyc_Absyn_Enumdecl*_tmp381;struct Cyc_Absyn_Typedefdecl*_tmp383;struct _dyneither_ptr*_tmp385;struct Cyc_List_List*_tmp386;struct Cyc_List_List*_tmp388;struct Cyc_List_List*_tmp38A;struct Cyc_List_List*_tmp38C;struct Cyc_List_List*_tmp38D;_LL241: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp378=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp378->tag != 0)goto _LL243;else{_tmp379=_tmp378->f1;}}_LL242:
# 1473
 if(_tmp379->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp379->initializer;
_tmp379->initializer=0;
if(_tmp379->sc == Cyc_Absyn_Public){
const char*_tmp500;Cyc_Absyndump_dump(((_tmp500="extern ",_tag_dyneither(_tmp500,sizeof(char),8))));}
Cyc_Absyndump_dumpvardecl(_tmp379,d->loc);
{const char*_tmp501;Cyc_Absyndump_dump(((_tmp501="\n",_tag_dyneither(_tmp501,sizeof(char),2))));}
_tmp379->initializer=init;
goto _LL240;};_LL243: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp37A=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp37A->tag != 1)goto _LL245;else{_tmp37B=_tmp37A->f1;}}_LL244:
# 1483
 if(_tmp37B->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp37B->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp507;struct Cyc_Absyn_FnInfo _tmp506;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp505;void*t=(void*)((_tmp505=_cycalloc(sizeof(*_tmp505)),((_tmp505[0]=((_tmp507.tag=9,((_tmp507.f1=((_tmp506.tvars=_tmp37B->tvs,((_tmp506.effect=_tmp37B->effect,((_tmp506.ret_tqual=_tmp37B->ret_tqual,((_tmp506.ret_typ=_tmp37B->ret_type,((_tmp506.args=
# 1487
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp37B->args),((_tmp506.c_varargs=_tmp37B->c_varargs,((_tmp506.cyc_varargs=_tmp37B->cyc_varargs,((_tmp506.rgn_po=_tmp37B->rgn_po,((_tmp506.attributes=0,_tmp506)))))))))))))))))),_tmp507)))),_tmp505))));
# 1490
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp37B);
{const char*_tmp508;Cyc_Absyndump_dump(((_tmp508=";\n",_tag_dyneither(_tmp508,sizeof(char),3))));}
goto _LL240;};_LL245: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp37C=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp37C->tag != 6)goto _LL247;else{_tmp37D=_tmp37C->f1;}}_LL246:
# 1494
 if(_tmp37D->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp37D->impl;
if(_tmp37D->sc == Cyc_Absyn_Abstract)_tmp37D->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp37D);
_tmp37D->impl=impl;
{const char*_tmp509;Cyc_Absyndump_dump(((_tmp509=";\n",_tag_dyneither(_tmp509,sizeof(char),3))));}
goto _LL240;};_LL247: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp37E=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp37E->tag != 7)goto _LL249;else{_tmp37F=_tmp37E->f1;}}_LL248:
# 1502
 Cyc_Absyndump_dump_datatypedecl(_tmp37F);
{const char*_tmp50A;Cyc_Absyndump_dump(((_tmp50A=";\n",_tag_dyneither(_tmp50A,sizeof(char),3))));}
goto _LL240;_LL249: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp380=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp380->tag != 8)goto _LL24B;else{_tmp381=_tmp380->f1;}}_LL24A:
# 1506
 Cyc_Absyndump_dump_enumdecl(_tmp381);
{const char*_tmp50B;Cyc_Absyndump_dump(((_tmp50B=";\n",_tag_dyneither(_tmp50B,sizeof(char),3))));}
goto _LL240;_LL24B: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp382=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp382->tag != 9)goto _LL24D;else{_tmp383=_tmp382->f1;}}_LL24C:
# 1510
 if(_tmp383->defn == 0){
Cyc_Absyndump_dumpdecl(d);{
const char*_tmp50C;Cyc_Absyndump_dump(((_tmp50C="\n",_tag_dyneither(_tmp50C,sizeof(char),2))));};}
# 1514
goto _LL240;_LL24D: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp384=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp384->tag != 10)goto _LL24F;else{_tmp385=_tmp384->f1;_tmp386=_tmp384->f2;}}_LL24E:
# 1516
{const char*_tmp50D;Cyc_Absyndump_dump(((_tmp50D="namespace ",_tag_dyneither(_tmp50D,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp385);
{const char*_tmp50E;Cyc_Absyndump_dump(((_tmp50E="{\n",_tag_dyneither(_tmp50E,sizeof(char),3))));}
for(0;_tmp386 != 0;_tmp386=_tmp386->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp386->hd);}
{const char*_tmp50F;Cyc_Absyndump_dump(((_tmp50F="}\n",_tag_dyneither(_tmp50F,sizeof(char),3))));}
goto _LL240;_LL24F: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp387=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp387->tag != 11)goto _LL251;else{_tmp388=_tmp387->f2;}}_LL250:
# 1524
 for(0;_tmp388 != 0;_tmp388=_tmp388->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp388->hd);}
goto _LL240;_LL251: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp389=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp389->tag != 12)goto _LL253;else{_tmp38A=_tmp389->f1;}}_LL252:
# 1528
{const char*_tmp510;Cyc_Absyndump_dump(((_tmp510="extern \"C\" {",_tag_dyneither(_tmp510,sizeof(char),13))));}
for(0;_tmp38A != 0;_tmp38A=_tmp38A->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp38A->hd);}
{const char*_tmp511;Cyc_Absyndump_dump(((_tmp511="}\n",_tag_dyneither(_tmp511,sizeof(char),3))));}
goto _LL240;_LL253: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp38B=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp377;if(_tmp38B->tag != 13)goto _LL255;else{_tmp38C=_tmp38B->f1;_tmp38D=_tmp38B->f2;}}_LL254:
# 1549 "absyndump.cyc"
 goto _LL240;_LL255:;_LL256: {
const char*_tmp514;void*_tmp513;(_tmp513=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp514="bad top-level declaration",_tag_dyneither(_tmp514,sizeof(char),26))),_tag_dyneither(_tmp513,sizeof(void*),0)));}_LL240:;}
# 1554
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
