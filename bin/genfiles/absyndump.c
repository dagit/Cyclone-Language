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
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 722 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_compress_kb(void*);
# 941
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 971
extern void*Cyc_Absyn_bounds_one;
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1155
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1161
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
# 373 "tcutil.h"
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
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
struct _tuple13 _stmttmp0=Cyc_Lex_xlate_pos(loc);struct _dyneither_ptr _tmp1;unsigned int _tmp2;struct _tuple13 _tmp0=_stmttmp0;_tmp1=_tmp0.f1;_tmp2=_tmp0.f2;
if((char*)_tmp1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)_tmp1.curr == (char*)last_file.curr  && _tmp2 == last_line)return;
if((char*)_tmp1.curr == (char*)last_file.curr  && _tmp2 == last_line + 1){
const char*_tmp3AA;void*_tmp3A9;(_tmp3A9=0,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3AA="\n",_tag_dyneither(_tmp3AA,sizeof(char),2))),_tag_dyneither(_tmp3A9,sizeof(void*),0)));}else{
if((char*)_tmp1.curr == (char*)last_file.curr){
const char*_tmp3AE;void*_tmp3AD[1];struct Cyc_Int_pa_PrintArg_struct _tmp3AC;(_tmp3AC.tag=1,((_tmp3AC.f1=(unsigned long)((int)_tmp2),((_tmp3AD[0]=& _tmp3AC,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3AE="\n# %d\n",_tag_dyneither(_tmp3AE,sizeof(char),7))),_tag_dyneither(_tmp3AD,sizeof(void*),1)))))));}else{
# 168
const char*_tmp3B3;void*_tmp3B2[2];struct Cyc_Int_pa_PrintArg_struct _tmp3B1;struct Cyc_String_pa_PrintArg_struct _tmp3B0;(_tmp3B0.tag=0,((_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1),((_tmp3B1.tag=1,((_tmp3B1.f1=(unsigned long)((int)_tmp2),((_tmp3B2[0]=& _tmp3B1,((_tmp3B2[1]=& _tmp3B0,Cyc_fprintf(*Cyc_Absyndump_dump_file,((_tmp3B3="\n# %d %s\n",_tag_dyneither(_tmp3B3,sizeof(char),10))),_tag_dyneither(_tmp3B2,sizeof(void*),2)))))))))))));}}
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
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 191
f(l->hd);}
# 193
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;l->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 200
f(env,l->hd);}
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
{union Cyc_Absyn_Nmspace _stmttmp1=(*v).f1;union Cyc_Absyn_Nmspace _tmpE=_stmttmp1;struct Cyc_List_List*_tmpF;struct Cyc_List_List*_tmp10;struct Cyc_List_List*_tmp11;_LL11: if((_tmpE.Loc_n).tag != 4)goto _LL13;_LL12:
 _tmpF=0;goto _LL14;_LL13: if((_tmpE.Rel_n).tag != 1)goto _LL15;_tmpF=(struct Cyc_List_List*)(_tmpE.Rel_n).val;_LL14:
# 228
 _tmpD=_tmpF;
goto _LL10;_LL15: if((_tmpE.C_n).tag != 3)goto _LL17;_tmp10=(struct Cyc_List_List*)(_tmpE.C_n).val;_LL16:
# 234
 Cyc_Absyndump_dump_char((int)' ');
goto _LL10;_LL17: if((_tmpE.Abs_n).tag != 2)goto _LL10;_tmp11=(struct Cyc_List_List*)(_tmpE.Abs_n).val;_LL18:
# 237
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpD=_tmp11;
goto _LL10;_LL10:;}
# 242
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3B4;Cyc_Absyndump_dump_nospace(((_tmp3B4="::",_tag_dyneither(_tmp3B4,sizeof(char),3))));}}
# 247
if(_tmpD != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpD->hd));
for(_tmpD=_tmpD->tl;_tmpD != 0;_tmpD=_tmpD->tl){
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
const char*_tmp3B5;Cyc_Absyndump_dump_nospace(((_tmp3B5="::",_tag_dyneither(_tmp3B5,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpD->hd));}
# 256
if(Cyc_Absyndump_qvar_to_Cids){const char*_tmp3B6;Cyc_Absyndump_dump_nospace(((_tmp3B6="_",_tag_dyneither(_tmp3B6,sizeof(char),2))));}else{
const char*_tmp3B7;Cyc_Absyndump_dump_nospace(((_tmp3B7="::",_tag_dyneither(_tmp3B7,sizeof(char),3))));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 260
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 263
Cyc_Absyndump_dump_str((*v).f2);}}}
# 267
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict){const char*_tmp3B8;Cyc_Absyndump_dump(((_tmp3B8="restrict",_tag_dyneither(_tmp3B8,sizeof(char),9))));}
if(tq.q_volatile){const char*_tmp3B9;Cyc_Absyndump_dump(((_tmp3B9="volatile",_tag_dyneither(_tmp3B9,sizeof(char),9))));}
if(tq.print_const){const char*_tmp3BA;Cyc_Absyndump_dump(((_tmp3BA="const",_tag_dyneither(_tmp3BA,sizeof(char),6))));}}
# 273
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
switch(sc){case Cyc_Absyn_Static: _LL19:
{const char*_tmp3BB;Cyc_Absyndump_dump(((_tmp3BB="static",_tag_dyneither(_tmp3BB,sizeof(char),7))));}return;case Cyc_Absyn_Public: _LL1A:
 return;case Cyc_Absyn_Extern: _LL1B:
{const char*_tmp3BC;Cyc_Absyndump_dump(((_tmp3BC="extern",_tag_dyneither(_tmp3BC,sizeof(char),7))));}return;case Cyc_Absyn_ExternC: _LL1C:
{const char*_tmp3BD;Cyc_Absyndump_dump(((_tmp3BD="extern \"C\"",_tag_dyneither(_tmp3BD,sizeof(char),11))));}return;case Cyc_Absyn_Abstract: _LL1D:
{const char*_tmp3BE;Cyc_Absyndump_dump(((_tmp3BE="abstract",_tag_dyneither(_tmp3BE,sizeof(char),9))));}return;case Cyc_Absyn_Register: _LL1E:
{const char*_tmp3BF;Cyc_Absyndump_dump(((_tmp3BF="register",_tag_dyneither(_tmp3BF,sizeof(char),9))));}return;}}
# 284
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp1F;enum Cyc_Absyn_AliasQual _tmp20;struct Cyc_Absyn_Kind*_tmp1E=ka;_tmp1F=_tmp1E->kind;_tmp20=_tmp1E->aliasqual;
switch(_tmp1F){case Cyc_Absyn_AnyKind: _LL20:
# 288
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL22:
{const char*_tmp3C0;Cyc_Absyndump_dump(((_tmp3C0="A",_tag_dyneither(_tmp3C0,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL23:
{const char*_tmp3C1;Cyc_Absyndump_dump(((_tmp3C1="UA",_tag_dyneither(_tmp3C1,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL24:
{const char*_tmp3C2;Cyc_Absyndump_dump(((_tmp3C2="TA",_tag_dyneither(_tmp3C2,sizeof(char),3))));}return;}case Cyc_Absyn_MemKind: _LL21:
# 294
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL27:
{const char*_tmp3C3;Cyc_Absyndump_dump(((_tmp3C3="M",_tag_dyneither(_tmp3C3,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL28:
{const char*_tmp3C4;Cyc_Absyndump_dump(((_tmp3C4="UM",_tag_dyneither(_tmp3C4,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL29:
{const char*_tmp3C5;Cyc_Absyndump_dump(((_tmp3C5="TM",_tag_dyneither(_tmp3C5,sizeof(char),3))));}return;}case Cyc_Absyn_BoxKind: _LL26:
# 300
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL2C:
{const char*_tmp3C6;Cyc_Absyndump_dump(((_tmp3C6="B",_tag_dyneither(_tmp3C6,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL2D:
{const char*_tmp3C7;Cyc_Absyndump_dump(((_tmp3C7="UB",_tag_dyneither(_tmp3C7,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL2E:
{const char*_tmp3C8;Cyc_Absyndump_dump(((_tmp3C8="TB",_tag_dyneither(_tmp3C8,sizeof(char),3))));}return;}case Cyc_Absyn_RgnKind: _LL2B:
# 306
 switch(_tmp20){case Cyc_Absyn_Aliasable: _LL31:
{const char*_tmp3C9;Cyc_Absyndump_dump(((_tmp3C9="R",_tag_dyneither(_tmp3C9,sizeof(char),2))));}return;case Cyc_Absyn_Unique: _LL32:
{const char*_tmp3CA;Cyc_Absyndump_dump(((_tmp3CA="UR",_tag_dyneither(_tmp3CA,sizeof(char),3))));}return;case Cyc_Absyn_Top: _LL33:
{const char*_tmp3CB;Cyc_Absyndump_dump(((_tmp3CB="TR",_tag_dyneither(_tmp3CB,sizeof(char),3))));}return;}case Cyc_Absyn_EffKind: _LL30:
# 311
{const char*_tmp3CC;Cyc_Absyndump_dump(((_tmp3CC="E",_tag_dyneither(_tmp3CC,sizeof(char),2))));}return;case Cyc_Absyn_IntKind: _LL35:
{const char*_tmp3CD;Cyc_Absyndump_dump(((_tmp3CD="I",_tag_dyneither(_tmp3CD,sizeof(char),2))));}return;}}
# 316
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LL37:
{const char*_tmp3CE;Cyc_Absyndump_dump(((_tmp3CE="struct ",_tag_dyneither(_tmp3CE,sizeof(char),8))));}return;case Cyc_Absyn_UnionA: _LL38:
{const char*_tmp3CF;Cyc_Absyndump_dump(((_tmp3CF="union ",_tag_dyneither(_tmp3CF,sizeof(char),7))));}return;}}
# 323
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
const char*_tmp3D2;const char*_tmp3D1;const char*_tmp3D0;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,ts,((_tmp3D0="<",_tag_dyneither(_tmp3D0,sizeof(char),2))),((_tmp3D1=">",_tag_dyneither(_tmp3D1,sizeof(char),2))),((_tmp3D2=",",_tag_dyneither(_tmp3D2,sizeof(char),2))));}
# 326
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
{const char*_tmp3D3;Cyc_Absyndump_dump(((_tmp3D3="`G",_tag_dyneither(_tmp3D3,sizeof(char),3))));}
{void*_stmttmp2=Cyc_Absyn_compress_kb(tv->kind);void*_tmp35=_stmttmp2;struct Cyc_Absyn_Kind*_tmp37;struct Cyc_Absyn_Kind*_tmp39;_LL3B: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp36=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp36->tag != 0)goto _LL3D;else{_tmp37=_tmp36->f1;}}_LL3C:
 _tmp39=_tmp37;goto _LL3E;_LL3D: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp38=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp35;if(_tmp38->tag != 2)goto _LL3F;else{_tmp39=_tmp38->f2;}}_LL3E:
 Cyc_Absyndump_dumpkind(_tmp39);goto _LL3A;_LL3F:;_LL40:
{const char*_tmp3D4;Cyc_Absyndump_dump_nospace(((_tmp3D4="K",_tag_dyneither(_tmp3D4,sizeof(char),2))));}goto _LL3A;_LL3A:;}
# 335
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 337
Cyc_Absyndump_dump(n);}}
# 339
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_stmttmp3=Cyc_Absyn_compress_kb(tv->kind);void*_tmp3B=_stmttmp3;struct Cyc_Absyn_Kind*_tmp3E;struct Cyc_Absyn_Kind*_tmp41;_LL42: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3C=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp3B;if(_tmp3C->tag != 1)goto _LL44;}_LL43:
 goto _LL41;_LL44: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp3B;if(_tmp3D->tag != 2)goto _LL46;else{_tmp3E=_tmp3D->f2;}}_LL45:
 goto _LL41;_LL46: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp3F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3B;if(_tmp3F->tag != 0)goto _LL48;else{if((_tmp3F->f1)->kind != Cyc_Absyn_BoxKind)goto _LL48;if((_tmp3F->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL48;}}_LL47:
 goto _LL41;_LL48: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp40=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp3B;if(_tmp40->tag != 0)goto _LL41;else{_tmp41=_tmp40->f1;}}_LL49:
{const char*_tmp3D5;Cyc_Absyndump_dump(((_tmp3D5="::",_tag_dyneither(_tmp3D5,sizeof(char),3))));}Cyc_Absyndump_dumpkind(_tmp41);goto _LL41;_LL41:;};}
# 348
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
const char*_tmp3D8;const char*_tmp3D7;const char*_tmp3D6;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,tvs,((_tmp3D6="<",_tag_dyneither(_tmp3D6,sizeof(char),2))),((_tmp3D7=">",_tag_dyneither(_tmp3D7,sizeof(char),2))),((_tmp3D8=",",_tag_dyneither(_tmp3D8,sizeof(char),2))));}
# 351
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
const char*_tmp3DB;const char*_tmp3DA;const char*_tmp3D9;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,tvs,((_tmp3D9="<",_tag_dyneither(_tmp3D9,sizeof(char),2))),((_tmp3DA=">",_tag_dyneither(_tmp3DA,sizeof(char),2))),((_tmp3DB=",",_tag_dyneither(_tmp3DB,sizeof(char),2))));}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 355
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 358
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
const char*_tmp3DE;const char*_tmp3DD;const char*_tmp3DC;((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,ts,((_tmp3DC="(",_tag_dyneither(_tmp3DC,sizeof(char),2))),((_tmp3DD=")",_tag_dyneither(_tmp3DD,sizeof(char),2))),((_tmp3DE=",",_tag_dyneither(_tmp3DE,sizeof(char),2))));}
# 362
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp4=(void*)atts->hd;void*_tmp4C=_stmttmp4;_LL4B: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp4D=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp4C;if(_tmp4D->tag != 1)goto _LL4D;}_LL4C:
{const char*_tmp3DF;Cyc_Absyndump_dump(((_tmp3DF="_stdcall",_tag_dyneither(_tmp3DF,sizeof(char),9))));}return;_LL4D: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp4E=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp4C;if(_tmp4E->tag != 2)goto _LL4F;}_LL4E:
{const char*_tmp3E0;Cyc_Absyndump_dump(((_tmp3E0="_cdecl",_tag_dyneither(_tmp3E0,sizeof(char),7))));}return;_LL4F: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp4F=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp4C;if(_tmp4F->tag != 3)goto _LL51;}_LL50:
{const char*_tmp3E1;Cyc_Absyndump_dump(((_tmp3E1="_fastcall",_tag_dyneither(_tmp3E1,sizeof(char),10))));}return;_LL51:;_LL52:
 goto _LL4A;_LL4A:;}}
# 372
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 374
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_stmttmp5=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp53=_stmttmp5;_LL54: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp54=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp53;if(_tmp54->tag != 1)goto _LL56;}_LL55:
 goto _LL57;_LL56: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp55=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp53;if(_tmp55->tag != 2)goto _LL58;}_LL57:
 goto _LL59;_LL58: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp56=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp53;if(_tmp56->tag != 3)goto _LL5A;}_LL59:
 goto _LL53;_LL5A:;_LL5B:
 hasatt=1;goto _LL53;_LL53:;}}
# 382
if(!hasatt)
return;
{const char*_tmp3E2;Cyc_Absyndump_dump(((_tmp3E2="__declspec(",_tag_dyneither(_tmp3E2,sizeof(char),12))));}
for(0;atts != 0;atts=atts->tl){
void*_stmttmp6=(void*)atts->hd;void*_tmp58=_stmttmp6;_LL5D: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp59=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp58;if(_tmp59->tag != 1)goto _LL5F;}_LL5E:
 goto _LL60;_LL5F: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5A=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp58;if(_tmp5A->tag != 2)goto _LL61;}_LL60:
 goto _LL62;_LL61: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5B=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp58;if(_tmp5B->tag != 3)goto _LL63;}_LL62:
 goto _LL5C;_LL63:;_LL64:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL5C;_LL5C:;}
# 392
Cyc_Absyndump_dump_char((int)')');}
# 395
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL65:
# 399
{const char*_tmp3E3;Cyc_Absyndump_dump(((_tmp3E3=" __attribute__((",_tag_dyneither(_tmp3E3,sizeof(char),17))));}
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0){const char*_tmp3E4;Cyc_Absyndump_dump(((_tmp3E4=",",_tag_dyneither(_tmp3E4,sizeof(char),2))));}}
# 404
{const char*_tmp3E5;Cyc_Absyndump_dump(((_tmp3E5=")) ",_tag_dyneither(_tmp3E5,sizeof(char),4))));}
return;case Cyc_Cyclone_Vc_c: _LL66:
# 407
 Cyc_Absyndump_dump_noncallconv(atts);
return;}}
# 412
int Cyc_Absyndump_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_stmttmp7=(void*)tms->hd;void*_tmp5F=_stmttmp7;_LL69: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp60=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp5F;if(_tmp60->tag != 2)goto _LL6B;}_LL6A:
 return 1;_LL6B:;_LL6C:
 return 0;_LL68:;};}
# 420
static void Cyc_Absyndump_dumprgn(void*t){
void*_stmttmp8=Cyc_Tcutil_compress(t);void*_tmp61=_stmttmp8;_LL6E: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp62=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp61;if(_tmp62->tag != 20)goto _LL70;}_LL6F:
{const char*_tmp3E6;Cyc_Absyndump_dump(((_tmp3E6="`H",_tag_dyneither(_tmp3E6,sizeof(char),3))));}goto _LL6D;_LL70:;_LL71:
 Cyc_Absyndump_dumpntyp(t);goto _LL6D;_LL6D:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 427
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 431
{void*_stmttmp9=Cyc_Tcutil_compress(t);void*_tmp64=_stmttmp9;void*_tmp66;struct Cyc_List_List*_tmp68;_LL73: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp65=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp64;if(_tmp65->tag != 23)goto _LL75;else{_tmp66=(void*)_tmp65->f1;}}_LL74:
{struct Cyc_List_List*_tmp3E7;rgions=((_tmp3E7=_cycalloc(sizeof(*_tmp3E7)),((_tmp3E7->hd=_tmp66,((_tmp3E7->tl=rgions,_tmp3E7))))));}goto _LL72;_LL75: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp67=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp64;if(_tmp67->tag != 24)goto _LL77;else{_tmp68=_tmp67->f1;}}_LL76:
# 434
 for(0;_tmp68 != 0;_tmp68=_tmp68->tl){
struct _tuple15 _stmttmpA=Cyc_Absyndump_effects_split((void*)_tmp68->hd);struct Cyc_List_List*_tmp6B;struct Cyc_List_List*_tmp6C;struct _tuple15 _tmp6A=_stmttmpA;_tmp6B=_tmp6A.f1;_tmp6C=_tmp6A.f2;
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6B,rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp6C,effects);}
# 439
goto _LL72;_LL77:;_LL78:
{struct Cyc_List_List*_tmp3E8;effects=((_tmp3E8=_cycalloc(sizeof(*_tmp3E8)),((_tmp3E8->hd=t,((_tmp3E8->tl=effects,_tmp3E8))))));}goto _LL72;_LL72:;}{
# 442
struct _tuple15 _tmp3E9;return(_tmp3E9.f1=rgions,((_tmp3E9.f2=effects,_tmp3E9)));};}
# 445
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _stmttmpB=Cyc_Absyndump_effects_split(t);struct Cyc_List_List*_tmp70;struct Cyc_List_List*_tmp71;struct _tuple15 _tmp6F=_stmttmpB;_tmp70=_tmp6F.f1;_tmp71=_tmp6F.f2;
_tmp70=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp70);
_tmp71=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp71);
if(_tmp71 != 0){
{struct Cyc_List_List*_tmp72=_tmp71;for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp72->hd);
if(_tmp72->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 455
if(_tmp70 != 0)Cyc_Absyndump_dump_char((int)'+');}
# 457
if(_tmp70 != 0  || _tmp71 == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp70 != 0;_tmp70=_tmp70->tl){
Cyc_Absyndump_dumprgn((void*)_tmp70->hd);
if(_tmp70->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 463
Cyc_Absyndump_dump_char((int)'}');}}
# 467
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 472
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp73=t;struct Cyc_Absyn_Tvar*_tmp79;struct Cyc_Core_Opt*_tmp7B;int _tmp7C;struct Cyc_Core_Opt*_tmp7E;void*_tmp7F;int _tmp80;union Cyc_Absyn_DatatypeInfoU _tmp82;struct Cyc_List_List*_tmp83;union Cyc_Absyn_DatatypeFieldInfoU _tmp85;struct Cyc_List_List*_tmp86;int _tmp97;struct Cyc_List_List*_tmp99;union Cyc_Absyn_AggrInfoU _tmp9B;struct Cyc_List_List*_tmp9C;enum Cyc_Absyn_AggrKind _tmp9E;struct Cyc_List_List*_tmp9F;struct _tuple0*_tmpA1;struct Cyc_List_List*_tmpA3;struct _tuple0*_tmpA5;struct Cyc_List_List*_tmpA6;struct Cyc_Absyn_Exp*_tmpA8;void*_tmpAA;void*_tmpAC;void*_tmpAD;void*_tmpAF;struct Cyc_Absyn_Aggrdecl*_tmpB4;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Datatypedecl*_tmpBA;void*_tmpBD;void*_tmpBF;struct Cyc_List_List*_tmpC1;_LL7A: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp74=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp73;if(_tmp74->tag != 8)goto _LL7C;}_LL7B:
# 475
 goto _LL7D;_LL7C: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp75=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp73;if(_tmp75->tag != 9)goto _LL7E;}_LL7D:
 goto _LL7F;_LL7E: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp76=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73;if(_tmp76->tag != 5)goto _LL80;}_LL7F:
 return;_LL80: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp77=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp73;if(_tmp77->tag != 0)goto _LL82;}_LL81:
{const char*_tmp3EA;Cyc_Absyndump_dump(((_tmp3EA="void",_tag_dyneither(_tmp3EA,sizeof(char),5))));}return;_LL82: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp78=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp73;if(_tmp78->tag != 2)goto _LL84;else{_tmp79=_tmp78->f1;}}_LL83:
 Cyc_Absyndump_dumptvar(_tmp79);return;_LL84: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp73;if(_tmp7A->tag != 1)goto _LL86;else{_tmp7B=_tmp7A->f1;if((void*)_tmp7A->f2 != 0)goto _LL86;_tmp7C=_tmp7A->f3;}}_LL85:
# 481
{const char*_tmp3EB;Cyc_Absyndump_dump(((_tmp3EB="`E",_tag_dyneither(_tmp3EB,sizeof(char),3))));}
if(_tmp7B == 0){const char*_tmp3EC;Cyc_Absyndump_dump(((_tmp3EC="K",_tag_dyneither(_tmp3EC,sizeof(char),2))));}else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmp7B->v);}
{const char*_tmp3F0;void*_tmp3EF[1];struct Cyc_Int_pa_PrintArg_struct _tmp3EE;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp3EE.tag=1,((_tmp3EE.f1=(unsigned long)_tmp7C,((_tmp3EF[0]=& _tmp3EE,Cyc_aprintf(((_tmp3F0="%d",_tag_dyneither(_tmp3F0,sizeof(char),3))),_tag_dyneither(_tmp3EF,sizeof(void*),1)))))))));}return;_LL86: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp7D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp73;if(_tmp7D->tag != 1)goto _LL88;else{_tmp7E=_tmp7D->f1;_tmp7F=(void*)_tmp7D->f2;_tmp80=_tmp7D->f3;}}_LL87:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmp7F));return;_LL88: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp81=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp73;if(_tmp81->tag != 3)goto _LL8A;else{_tmp82=(_tmp81->f1).datatype_info;_tmp83=(_tmp81->f1).targs;}}_LL89:
# 486
{union Cyc_Absyn_DatatypeInfoU _tmpC8=_tmp82;struct _tuple0*_tmpC9;int _tmpCA;struct _tuple0*_tmpCB;int _tmpCC;_LLD3: if((_tmpC8.UnknownDatatype).tag != 1)goto _LLD5;_tmpC9=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC8.UnknownDatatype).val).name;_tmpCA=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmpC8.UnknownDatatype).val).is_extensible;_LLD4:
 _tmpCB=_tmpC9;_tmpCC=_tmpCA;goto _LLD6;_LLD5: if((_tmpC8.KnownDatatype).tag != 2)goto _LLD2;_tmpCB=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC8.KnownDatatype).val))->name;_tmpCC=(*((struct Cyc_Absyn_Datatypedecl**)(_tmpC8.KnownDatatype).val))->is_extensible;_LLD6:
# 489
 if(_tmpCC){const char*_tmp3F1;Cyc_Absyndump_dump(((_tmp3F1="@extensible ",_tag_dyneither(_tmp3F1,sizeof(char),13))));}
{const char*_tmp3F2;Cyc_Absyndump_dump(((_tmp3F2="datatype ",_tag_dyneither(_tmp3F2,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpCB);
Cyc_Absyndump_dumptps(_tmp83);
goto _LLD2;_LLD2:;}
# 495
goto _LL79;_LL8A: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp84=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp73;if(_tmp84->tag != 4)goto _LL8C;else{_tmp85=(_tmp84->f1).field_info;_tmp86=(_tmp84->f1).targs;}}_LL8B:
# 497
{union Cyc_Absyn_DatatypeFieldInfoU _tmpCF=_tmp85;struct _tuple0*_tmpD0;struct _tuple0*_tmpD1;int _tmpD2;struct _tuple0*_tmpD3;int _tmpD4;struct _tuple0*_tmpD5;_LLD8: if((_tmpCF.UnknownDatatypefield).tag != 1)goto _LLDA;_tmpD0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCF.UnknownDatatypefield).val).datatype_name;_tmpD1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCF.UnknownDatatypefield).val).field_name;_tmpD2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmpCF.UnknownDatatypefield).val).is_extensible;_LLD9:
# 499
 _tmpD3=_tmpD0;_tmpD4=_tmpD2;_tmpD5=_tmpD1;goto _LLDB;_LLDA: if((_tmpCF.KnownDatatypefield).tag != 2)goto _LLD7;_tmpD3=(((struct _tuple1)(_tmpCF.KnownDatatypefield).val).f1)->name;_tmpD4=(((struct _tuple1)(_tmpCF.KnownDatatypefield).val).f1)->is_extensible;_tmpD5=(((struct _tuple1)(_tmpCF.KnownDatatypefield).val).f2)->name;_LLDB:
# 502
 if(_tmpD4){const char*_tmp3F3;Cyc_Absyndump_dump(((_tmp3F3="@extensible ",_tag_dyneither(_tmp3F3,sizeof(char),13))));}
{const char*_tmp3F4;Cyc_Absyndump_dump(((_tmp3F4="datatype ",_tag_dyneither(_tmp3F4,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmpD3);
{const char*_tmp3F5;Cyc_Absyndump_dump(((_tmp3F5=".",_tag_dyneither(_tmp3F5,sizeof(char),2))));}
Cyc_Absyndump_dumpqvar(_tmpD5);
Cyc_Absyndump_dumptps(_tmp86);
goto _LLD7;_LLD7:;}
# 510
goto _LL79;_LL8C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp87=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp87->tag != 6)goto _LL8E;else{if(_tmp87->f1 != Cyc_Absyn_None)goto _LL8E;if(_tmp87->f2 != Cyc_Absyn_Int_sz)goto _LL8E;}}_LL8D:
 goto _LL8F;_LL8E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp88=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp88->tag != 6)goto _LL90;else{if(_tmp88->f1 != Cyc_Absyn_Signed)goto _LL90;if(_tmp88->f2 != Cyc_Absyn_Int_sz)goto _LL90;}}_LL8F:
{const char*_tmp3F6;Cyc_Absyndump_dump(((_tmp3F6="int",_tag_dyneither(_tmp3F6,sizeof(char),4))));}return;_LL90: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp89=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp89->tag != 6)goto _LL92;else{if(_tmp89->f1 != Cyc_Absyn_None)goto _LL92;if(_tmp89->f2 != Cyc_Absyn_Long_sz)goto _LL92;}}_LL91:
 goto _LL93;_LL92: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8A=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8A->tag != 6)goto _LL94;else{if(_tmp8A->f1 != Cyc_Absyn_Signed)goto _LL94;if(_tmp8A->f2 != Cyc_Absyn_Long_sz)goto _LL94;}}_LL93:
{const char*_tmp3F7;Cyc_Absyndump_dump(((_tmp3F7="long",_tag_dyneither(_tmp3F7,sizeof(char),5))));}return;_LL94: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8B=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8B->tag != 6)goto _LL96;else{if(_tmp8B->f1 != Cyc_Absyn_None)goto _LL96;if(_tmp8B->f2 != Cyc_Absyn_Char_sz)goto _LL96;}}_LL95:
{const char*_tmp3F8;Cyc_Absyndump_dump(((_tmp3F8="char",_tag_dyneither(_tmp3F8,sizeof(char),5))));}return;_LL96: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8C=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8C->tag != 6)goto _LL98;else{if(_tmp8C->f1 != Cyc_Absyn_Signed)goto _LL98;if(_tmp8C->f2 != Cyc_Absyn_Char_sz)goto _LL98;}}_LL97:
{const char*_tmp3F9;Cyc_Absyndump_dump(((_tmp3F9="signed char",_tag_dyneither(_tmp3F9,sizeof(char),12))));}return;_LL98: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8D=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8D->tag != 6)goto _LL9A;else{if(_tmp8D->f1 != Cyc_Absyn_Unsigned)goto _LL9A;if(_tmp8D->f2 != Cyc_Absyn_Char_sz)goto _LL9A;}}_LL99:
{const char*_tmp3FA;Cyc_Absyndump_dump(((_tmp3FA="unsigned char",_tag_dyneither(_tmp3FA,sizeof(char),14))));}return;_LL9A: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8E=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8E->tag != 6)goto _LL9C;else{if(_tmp8E->f1 != Cyc_Absyn_None)goto _LL9C;if(_tmp8E->f2 != Cyc_Absyn_Short_sz)goto _LL9C;}}_LL9B:
 goto _LL9D;_LL9C: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp8F=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp8F->tag != 6)goto _LL9E;else{if(_tmp8F->f1 != Cyc_Absyn_Signed)goto _LL9E;if(_tmp8F->f2 != Cyc_Absyn_Short_sz)goto _LL9E;}}_LL9D:
{const char*_tmp3FB;Cyc_Absyndump_dump(((_tmp3FB="short",_tag_dyneither(_tmp3FB,sizeof(char),6))));}return;_LL9E: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp90=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp90->tag != 6)goto _LLA0;else{if(_tmp90->f1 != Cyc_Absyn_Unsigned)goto _LLA0;if(_tmp90->f2 != Cyc_Absyn_Short_sz)goto _LLA0;}}_LL9F:
{const char*_tmp3FC;Cyc_Absyndump_dump(((_tmp3FC="unsigned short",_tag_dyneither(_tmp3FC,sizeof(char),15))));}return;_LLA0: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp91=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp91->tag != 6)goto _LLA2;else{if(_tmp91->f1 != Cyc_Absyn_Unsigned)goto _LLA2;if(_tmp91->f2 != Cyc_Absyn_Int_sz)goto _LLA2;}}_LLA1:
{const char*_tmp3FD;Cyc_Absyndump_dump(((_tmp3FD="unsigned int",_tag_dyneither(_tmp3FD,sizeof(char),13))));}return;_LLA2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp92=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp92->tag != 6)goto _LLA4;else{if(_tmp92->f1 != Cyc_Absyn_Unsigned)goto _LLA4;if(_tmp92->f2 != Cyc_Absyn_Long_sz)goto _LLA4;}}_LLA3:
{const char*_tmp3FE;Cyc_Absyndump_dump(((_tmp3FE="unsigned long",_tag_dyneither(_tmp3FE,sizeof(char),14))));}return;_LLA4: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp93=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp93->tag != 6)goto _LLA6;else{if(_tmp93->f1 != Cyc_Absyn_None)goto _LLA6;if(_tmp93->f2 != Cyc_Absyn_LongLong_sz)goto _LLA6;}}_LLA5:
 goto _LLA7;_LLA6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp94=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp94->tag != 6)goto _LLA8;else{if(_tmp94->f1 != Cyc_Absyn_Signed)goto _LLA8;if(_tmp94->f2 != Cyc_Absyn_LongLong_sz)goto _LLA8;}}_LLA7:
# 525
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLDC:
{const char*_tmp3FF;Cyc_Absyndump_dump(((_tmp3FF="long long",_tag_dyneither(_tmp3FF,sizeof(char),10))));}return;case Cyc_Cyclone_Vc_c: _LLDD:
{const char*_tmp400;Cyc_Absyndump_dump(((_tmp400="__int64",_tag_dyneither(_tmp400,sizeof(char),8))));}return;}_LLA8: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp95=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73;if(_tmp95->tag != 6)goto _LLAA;else{if(_tmp95->f1 != Cyc_Absyn_Unsigned)goto _LLAA;if(_tmp95->f2 != Cyc_Absyn_LongLong_sz)goto _LLAA;}}_LLA9:
# 530
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LLDF:
{const char*_tmp401;Cyc_Absyndump_dump(((_tmp401="unsigned __int64",_tag_dyneither(_tmp401,sizeof(char),17))));}return;case Cyc_Cyclone_Gcc_c: _LLE0:
{const char*_tmp402;Cyc_Absyndump_dump(((_tmp402="unsigned long long",_tag_dyneither(_tmp402,sizeof(char),19))));}return;}_LLAA: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp96=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp73;if(_tmp96->tag != 7)goto _LLAC;else{_tmp97=_tmp96->f1;}}_LLAB:
# 535
 if(_tmp97 == 0){
const char*_tmp403;Cyc_Absyndump_dump(((_tmp403="float",_tag_dyneither(_tmp403,sizeof(char),6))));}else{
if(_tmp97 == 1){
const char*_tmp404;Cyc_Absyndump_dump(((_tmp404="double",_tag_dyneither(_tmp404,sizeof(char),7))));}else{
# 540
const char*_tmp405;Cyc_Absyndump_dump(((_tmp405="long double",_tag_dyneither(_tmp405,sizeof(char),12))));}}
return;_LLAC: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp98=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp73;if(_tmp98->tag != 10)goto _LLAE;else{_tmp99=_tmp98->f1;}}_LLAD:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmp99);return;_LLAE: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp9A=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp73;if(_tmp9A->tag != 11)goto _LLB0;else{_tmp9B=(_tmp9A->f1).aggr_info;_tmp9C=(_tmp9A->f1).targs;}}_LLAF: {
# 544
struct _tuple10 _stmttmpC=Cyc_Absyn_aggr_kinded_name(_tmp9B);enum Cyc_Absyn_AggrKind _tmpEA;struct _tuple0*_tmpEB;struct _tuple10 _tmpE9=_stmttmpC;_tmpEA=_tmpE9.f1;_tmpEB=_tmpE9.f2;
Cyc_Absyndump_dumpaggr_kind(_tmpEA);Cyc_Absyndump_dumpqvar(_tmpEB);Cyc_Absyndump_dumptps(_tmp9C);
return;}_LLB0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9D=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp73;if(_tmp9D->tag != 12)goto _LLB2;else{_tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;}}_LLB1:
# 548
 Cyc_Absyndump_dumpaggr_kind(_tmp9E);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmp9F);Cyc_Absyndump_dump_char((int)'}');return;_LLB2: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmpA0=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp73;if(_tmpA0->tag != 13)goto _LLB4;else{_tmpA1=_tmpA0->f1;}}_LLB3:
{const char*_tmp406;Cyc_Absyndump_dump(((_tmp406="enum ",_tag_dyneither(_tmp406,sizeof(char),6))));}Cyc_Absyndump_dumpqvar(_tmpA1);return;_LLB4: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmpA2=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp73;if(_tmpA2->tag != 14)goto _LLB6;else{_tmpA3=_tmpA2->f1;}}_LLB5:
{const char*_tmp407;Cyc_Absyndump_dump(((_tmp407="enum {",_tag_dyneither(_tmp407,sizeof(char),7))));}Cyc_Absyndump_dumpenumfields(_tmpA3);{const char*_tmp408;Cyc_Absyndump_dump(((_tmp408="}",_tag_dyneither(_tmp408,sizeof(char),2))));}return;_LLB6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA4=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp73;if(_tmpA4->tag != 17)goto _LLB8;else{_tmpA5=_tmpA4->f1;_tmpA6=_tmpA4->f2;}}_LLB7:
(Cyc_Absyndump_dumpqvar(_tmpA5),Cyc_Absyndump_dumptps(_tmpA6));return;_LLB8: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA7=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp73;if(_tmpA7->tag != 18)goto _LLBA;else{_tmpA8=_tmpA7->f1;}}_LLB9:
{const char*_tmp409;Cyc_Absyndump_dump(((_tmp409="valueof_t(",_tag_dyneither(_tmp409,sizeof(char),11))));}Cyc_Absyndump_dumpexp(_tmpA8);{const char*_tmp40A;Cyc_Absyndump_dump(((_tmp40A=")",_tag_dyneither(_tmp40A,sizeof(char),2))));}return;_LLBA: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpA9=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp73;if(_tmpA9->tag != 15)goto _LLBC;else{_tmpAA=(void*)_tmpA9->f1;}}_LLBB:
# 554
{const char*_tmp40B;Cyc_Absyndump_dump(((_tmp40B="region_t<",_tag_dyneither(_tmp40B,sizeof(char),10))));}Cyc_Absyndump_dumprgn(_tmpAA);{const char*_tmp40C;Cyc_Absyndump_dump(((_tmp40C=">",_tag_dyneither(_tmp40C,sizeof(char),2))));}return;_LLBC: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmpAB=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp73;if(_tmpAB->tag != 16)goto _LLBE;else{_tmpAC=(void*)_tmpAB->f1;_tmpAD=(void*)_tmpAB->f2;}}_LLBD:
{const char*_tmp40D;Cyc_Absyndump_dump(((_tmp40D="dynregion_t<",_tag_dyneither(_tmp40D,sizeof(char),13))));}Cyc_Absyndump_dumprgn(_tmpAC);
{const char*_tmp40E;Cyc_Absyndump_dump(((_tmp40E=",",_tag_dyneither(_tmp40E,sizeof(char),2))));}Cyc_Absyndump_dumprgn(_tmpAD);{const char*_tmp40F;Cyc_Absyndump_dump(((_tmp40F=">",_tag_dyneither(_tmp40F,sizeof(char),2))));}return;_LLBE: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmpAE=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp73;if(_tmpAE->tag != 19)goto _LLC0;else{_tmpAF=(void*)_tmpAE->f1;}}_LLBF:
{const char*_tmp410;Cyc_Absyndump_dump(((_tmp410="tag_t<",_tag_dyneither(_tmp410,sizeof(char),7))));}Cyc_Absyndump_dumpntyp(_tmpAF);{const char*_tmp411;Cyc_Absyndump_dump(((_tmp411=">",_tag_dyneither(_tmp411,sizeof(char),2))));}return;_LLC0: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmpB0=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp73;if(_tmpB0->tag != 21)goto _LLC2;}_LLC1:
{const char*_tmp412;Cyc_Absyndump_dump(((_tmp412="`U",_tag_dyneither(_tmp412,sizeof(char),3))));}goto _LL79;_LLC2: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmpB1=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp73;if(_tmpB1->tag != 22)goto _LLC4;}_LLC3:
{const char*_tmp413;Cyc_Absyndump_dump(((_tmp413="`RC",_tag_dyneither(_tmp413,sizeof(char),4))));}goto _LL79;_LLC4: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB2=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp73;if(_tmpB2->tag != 26)goto _LLC6;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmpB3=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmpB2->f1)->r;if(_tmpB3->tag != 0)goto _LLC6;else{_tmpB4=_tmpB3->f1;}}}_LLC5:
# 561
 Cyc_Absyndump_dump_aggrdecl(_tmpB4);return;_LLC6: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB5=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp73;if(_tmpB5->tag != 26)goto _LLC8;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmpB6=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmpB5->f1)->r;if(_tmpB6->tag != 1)goto _LLC8;else{_tmpB7=_tmpB6->f1;}}}_LLC7:
# 563
 Cyc_Absyndump_dump_enumdecl(_tmpB7);return;_LLC8: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmpB8=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp73;if(_tmpB8->tag != 26)goto _LLCA;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmpB9=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmpB8->f1)->r;if(_tmpB9->tag != 2)goto _LLCA;else{_tmpBA=_tmpB9->f1;}}}_LLC9:
# 565
 Cyc_Absyndump_dump_datatypedecl(_tmpBA);return;_LLCA: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmpBB=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp73;if(_tmpBB->tag != 20)goto _LLCC;}_LLCB:
{const char*_tmp414;Cyc_Absyndump_dump(((_tmp414="`H",_tag_dyneither(_tmp414,sizeof(char),3))));}goto _LL79;_LLCC: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpBC=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp73;if(_tmpBC->tag != 23)goto _LLCE;else{_tmpBD=(void*)_tmpBC->f1;}}_LLCD:
{const char*_tmp415;Cyc_Absyndump_dump(((_tmp415="{",_tag_dyneither(_tmp415,sizeof(char),2))));}Cyc_Absyndump_dumptyp(_tmpBD);{const char*_tmp416;Cyc_Absyndump_dump(((_tmp416="}",_tag_dyneither(_tmp416,sizeof(char),2))));}goto _LL79;_LLCE: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmpBE=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp73;if(_tmpBE->tag != 25)goto _LLD0;else{_tmpBF=(void*)_tmpBE->f1;}}_LLCF:
{const char*_tmp417;Cyc_Absyndump_dump(((_tmp417="regions(",_tag_dyneither(_tmp417,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmpBF);{const char*_tmp418;Cyc_Absyndump_dump(((_tmp418=")",_tag_dyneither(_tmp418,sizeof(char),2))));}goto _LL79;_LLD0: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpC0=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp73;if(_tmpC0->tag != 24)goto _LL79;else{_tmpC1=_tmpC0->f1;}}_LLD1:
# 570
 for(0;_tmpC1 != 0;_tmpC1=_tmpC1->tl){
Cyc_Absyndump_dumptyp((void*)_tmpC1->hd);
if(_tmpC1->tl != 0){const char*_tmp419;Cyc_Absyndump_dump(((_tmp419="+",_tag_dyneither(_tmp419,sizeof(char),2))));}}_LL79:;}
# 577
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 580
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 583
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
void*_tmp101;void*_tmp102;struct _tuple16*_tmp100=cmp;_tmp101=_tmp100->f1;_tmp102=_tmp100->f2;
Cyc_Absyndump_dumpeff(_tmp101);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmp102);}
# 587
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
const char*_tmp41A;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,rgn_po,((_tmp41A=",",_tag_dyneither(_tmp41A,sizeof(char),2))));}
# 590
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 593
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 598
if(c_varargs){
const char*_tmp41B;Cyc_Absyndump_dump(((_tmp41B="...",_tag_dyneither(_tmp41B,sizeof(char),4))));}else{
if(cyc_varargs != 0){
struct _tuple8*_tmp41C;struct _tuple8*_tmp105=(_tmp41C=_cycalloc(sizeof(*_tmp41C)),((_tmp41C->f1=cyc_varargs->name,((_tmp41C->f2=cyc_varargs->tq,((_tmp41C->f3=cyc_varargs->type,_tmp41C)))))));
{const char*_tmp41D;Cyc_Absyndump_dump(((_tmp41D="...",_tag_dyneither(_tmp41D,sizeof(char),4))));}
if(cyc_varargs->inject){const char*_tmp41E;Cyc_Absyndump_dump(((_tmp41E=" inject ",_tag_dyneither(_tmp41E,sizeof(char),9))));}
Cyc_Absyndump_dumpfunarg(_tmp105);}}
# 606
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 610
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 614
Cyc_Absyndump_dump_char((int)')');}
# 617
void Cyc_Absyndump_dumptyp(void*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 621
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmp109=d;struct Cyc_Absyn_Exp*_tmp10B;struct _dyneither_ptr*_tmp10D;_LLE3: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp10A=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp109;if(_tmp10A->tag != 0)goto _LLE5;else{_tmp10B=_tmp10A->f1;}}_LLE4:
{const char*_tmp41F;Cyc_Absyndump_dump(((_tmp41F=".[",_tag_dyneither(_tmp41F,sizeof(char),3))));}Cyc_Absyndump_dumpexp(_tmp10B);Cyc_Absyndump_dump_char((int)']');goto _LLE2;_LLE5: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp10C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp109;if(_tmp10C->tag != 1)goto _LLE2;else{_tmp10D=_tmp10C->f1;}}_LLE6:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp10D);goto _LLE2;_LLE2:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 628
void Cyc_Absyndump_dumpde(struct _tuple17*de){
{const char*_tmp422;const char*_tmp421;const char*_tmp420;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*de).f1,((_tmp420="",_tag_dyneither(_tmp420,sizeof(char),1))),((_tmp421="=",_tag_dyneither(_tmp421,sizeof(char),2))),((_tmp422="=",_tag_dyneither(_tmp422,sizeof(char),2))));}
Cyc_Absyndump_dumpexp((*de).f2);}
# 633
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp425;const char*_tmp424;const char*_tmp423;((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,inprec,es,((_tmp423="",_tag_dyneither(_tmp423,sizeof(char),1))),((_tmp424="",_tag_dyneither(_tmp424,sizeof(char),1))),((_tmp425=",",_tag_dyneither(_tmp425,sizeof(char),2))));}
# 637
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec){
const char*_tmp426;Cyc_Absyndump_dump_nospace(((_tmp426="(",_tag_dyneither(_tmp426,sizeof(char),2))));}
{void*_stmttmpD=e->r;void*_tmp116=_stmttmpD;enum Cyc_Absyn_Sign _tmp118;char _tmp119;struct _dyneither_ptr _tmp11B;enum Cyc_Absyn_Sign _tmp11D;short _tmp11E;int _tmp120;int _tmp122;int _tmp124;enum Cyc_Absyn_Sign _tmp126;long long _tmp127;struct _dyneither_ptr _tmp129;struct _dyneither_ptr _tmp12C;struct _dyneither_ptr _tmp12E;struct _tuple0*_tmp130;enum Cyc_Absyn_Primop _tmp132;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Core_Opt*_tmp136;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp13B;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp149;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Exp*_tmp14C;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_List_List*_tmp14F;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp155;void*_tmp157;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;void*_tmp15F;struct Cyc_Absyn_Exp*_tmp161;void*_tmp163;int _tmp165;struct _dyneither_ptr _tmp166;struct Cyc_Absyn_Exp*_tmp168;struct _dyneither_ptr*_tmp169;void*_tmp16B;struct Cyc_List_List*_tmp16C;struct Cyc_Absyn_Exp*_tmp16E;struct Cyc_Absyn_Exp*_tmp170;struct _dyneither_ptr*_tmp171;struct Cyc_Absyn_Exp*_tmp173;struct _dyneither_ptr*_tmp174;struct Cyc_Absyn_Exp*_tmp176;struct Cyc_Absyn_Exp*_tmp177;struct Cyc_List_List*_tmp179;struct _tuple8*_tmp17B;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Vardecl*_tmp180;struct Cyc_Absyn_Exp*_tmp181;struct Cyc_Absyn_Exp*_tmp182;struct Cyc_Absyn_Exp*_tmp184;void*_tmp185;struct _tuple0*_tmp187;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp189;struct Cyc_List_List*_tmp18B;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_Datatypefield*_tmp18E;struct _tuple0*_tmp190;struct _tuple0*_tmp192;int _tmp194;struct Cyc_Absyn_Exp*_tmp195;void**_tmp196;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Core_Opt*_tmp19C;struct Cyc_List_List*_tmp19D;struct Cyc_Absyn_Stmt*_tmp19F;_LLE8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp117=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp117->tag != 0)goto _LLEA;else{if(((_tmp117->f1).Char_c).tag != 2)goto _LLEA;_tmp118=((struct _tuple3)((_tmp117->f1).Char_c).val).f1;_tmp119=((struct _tuple3)((_tmp117->f1).Char_c).val).f2;}}_LLE9:
# 643
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp119));Cyc_Absyndump_dump_char((int)'\'');
goto _LLE7;_LLEA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp11A->tag != 0)goto _LLEC;else{if(((_tmp11A->f1).Wchar_c).tag != 3)goto _LLEC;_tmp11B=(struct _dyneither_ptr)((_tmp11A->f1).Wchar_c).val;}}_LLEB:
{const char*_tmp42A;void*_tmp429[1];struct Cyc_String_pa_PrintArg_struct _tmp428;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp428.tag=0,((_tmp428.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11B),((_tmp429[0]=& _tmp428,Cyc_aprintf(((_tmp42A="L'%s'",_tag_dyneither(_tmp42A,sizeof(char),6))),_tag_dyneither(_tmp429,sizeof(void*),1)))))))));}goto _LLE7;_LLEC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp11C->tag != 0)goto _LLEE;else{if(((_tmp11C->f1).Short_c).tag != 4)goto _LLEE;_tmp11D=((struct _tuple4)((_tmp11C->f1).Short_c).val).f1;_tmp11E=((struct _tuple4)((_tmp11C->f1).Short_c).val).f2;}}_LLED:
{const char*_tmp42E;void*_tmp42D[1];struct Cyc_Int_pa_PrintArg_struct _tmp42C;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp42C.tag=1,((_tmp42C.f1=(unsigned long)((int)_tmp11E),((_tmp42D[0]=& _tmp42C,Cyc_aprintf(((_tmp42E="%d",_tag_dyneither(_tmp42E,sizeof(char),3))),_tag_dyneither(_tmp42D,sizeof(void*),1)))))))));}goto _LLE7;_LLEE: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp11F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp11F->tag != 0)goto _LLF0;else{if(((_tmp11F->f1).Int_c).tag != 5)goto _LLF0;if(((struct _tuple5)((_tmp11F->f1).Int_c).val).f1 != Cyc_Absyn_None)goto _LLF0;_tmp120=((struct _tuple5)((_tmp11F->f1).Int_c).val).f2;}}_LLEF:
 _tmp122=_tmp120;goto _LLF1;_LLF0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp121=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp121->tag != 0)goto _LLF2;else{if(((_tmp121->f1).Int_c).tag != 5)goto _LLF2;if(((struct _tuple5)((_tmp121->f1).Int_c).val).f1 != Cyc_Absyn_Signed)goto _LLF2;_tmp122=((struct _tuple5)((_tmp121->f1).Int_c).val).f2;}}_LLF1:
{const char*_tmp432;void*_tmp431[1];struct Cyc_Int_pa_PrintArg_struct _tmp430;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp430.tag=1,((_tmp430.f1=(unsigned long)_tmp122,((_tmp431[0]=& _tmp430,Cyc_aprintf(((_tmp432="%d",_tag_dyneither(_tmp432,sizeof(char),3))),_tag_dyneither(_tmp431,sizeof(void*),1)))))))));}goto _LLE7;_LLF2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp123=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp123->tag != 0)goto _LLF4;else{if(((_tmp123->f1).Int_c).tag != 5)goto _LLF4;if(((struct _tuple5)((_tmp123->f1).Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LLF4;_tmp124=((struct _tuple5)((_tmp123->f1).Int_c).val).f2;}}_LLF3:
{const char*_tmp436;void*_tmp435[1];struct Cyc_Int_pa_PrintArg_struct _tmp434;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp434.tag=1,((_tmp434.f1=(unsigned int)_tmp124,((_tmp435[0]=& _tmp434,Cyc_aprintf(((_tmp436="%u",_tag_dyneither(_tmp436,sizeof(char),3))),_tag_dyneither(_tmp435,sizeof(void*),1)))))))));}goto _LLE7;_LLF4: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp125=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp125->tag != 0)goto _LLF6;else{if(((_tmp125->f1).LongLong_c).tag != 6)goto _LLF6;_tmp126=((struct _tuple6)((_tmp125->f1).LongLong_c).val).f1;_tmp127=((struct _tuple6)((_tmp125->f1).LongLong_c).val).f2;}}_LLF5:
# 651
{const char*_tmp437;Cyc_Absyndump_dump(((_tmp437="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp437,sizeof(char),27))));}goto _LLE7;_LLF6: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp128=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp128->tag != 0)goto _LLF8;else{if(((_tmp128->f1).Float_c).tag != 7)goto _LLF8;_tmp129=((struct _tuple7)((_tmp128->f1).Float_c).val).f1;}}_LLF7:
 Cyc_Absyndump_dump(_tmp129);goto _LLE7;_LLF8: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp12A->tag != 0)goto _LLFA;else{if(((_tmp12A->f1).Null_c).tag != 1)goto _LLFA;}}_LLF9:
{const char*_tmp438;Cyc_Absyndump_dump(((_tmp438="NULL",_tag_dyneither(_tmp438,sizeof(char),5))));}goto _LLE7;_LLFA: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12B=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp12B->tag != 0)goto _LLFC;else{if(((_tmp12B->f1).String_c).tag != 8)goto _LLFC;_tmp12C=(struct _dyneither_ptr)((_tmp12B->f1).String_c).val;}}_LLFB:
# 655
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp12C));Cyc_Absyndump_dump_char((int)'"');
goto _LLE7;_LLFC: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp12D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp12D->tag != 0)goto _LLFE;else{if(((_tmp12D->f1).Wstring_c).tag != 9)goto _LLFE;_tmp12E=(struct _dyneither_ptr)((_tmp12D->f1).Wstring_c).val;}}_LLFD:
# 658
{const char*_tmp439;Cyc_Absyndump_dump(((_tmp439="L\"",_tag_dyneither(_tmp439,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(_tmp12E);Cyc_Absyndump_dump_char((int)'"');
goto _LLE7;_LLFE: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp12F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp12F->tag != 1)goto _LL100;else{_tmp130=_tmp12F->f1;}}_LLFF:
# 661
 Cyc_Absyndump_dumpqvar(_tmp130);goto _LLE7;_LL100: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp131=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp131->tag != 2)goto _LL102;else{_tmp132=_tmp131->f1;_tmp133=_tmp131->f2;}}_LL101: {
# 664
struct _dyneither_ptr _tmp1AF=Cyc_Absynpp_prim2str(_tmp132);
{int _stmttmpF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp133);switch(_stmttmpF){case 1: _LL152:
# 667
 if(_tmp132 == Cyc_Absyn_Numelts){
{const char*_tmp43A;Cyc_Absyndump_dump(((_tmp43A="numelts(",_tag_dyneither(_tmp43A,sizeof(char),9))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp133))->hd);{
const char*_tmp43B;Cyc_Absyndump_dump(((_tmp43B=")",_tag_dyneither(_tmp43B,sizeof(char),2))));};}else{
# 672
Cyc_Absyndump_dump(_tmp1AF);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp133))->hd);}
# 675
break;case 2: _LL153:
# 677
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp133))->hd);
Cyc_Absyndump_dump(_tmp1AF);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp133->tl))->hd);
break;default: _LL154: {
# 683
struct Cyc_Core_Failure_exn_struct _tmp441;const char*_tmp440;struct Cyc_Core_Failure_exn_struct*_tmp43F;(int)_throw((void*)((_tmp43F=_cycalloc(sizeof(*_tmp43F)),((_tmp43F[0]=((_tmp441.tag=Cyc_Core_Failure,((_tmp441.f1=((_tmp440="Absyndump -- Bad number of arguments to primop",_tag_dyneither(_tmp440,sizeof(char),47))),_tmp441)))),_tmp43F)))));}}}
# 685
goto _LLE7;}_LL102: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp134=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp134->tag != 3)goto _LL104;else{_tmp135=_tmp134->f1;_tmp136=_tmp134->f2;_tmp137=_tmp134->f3;}}_LL103:
# 688
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp135);
if(_tmp136 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp136->v));
{const char*_tmp442;Cyc_Absyndump_dump_nospace(((_tmp442="=",_tag_dyneither(_tmp442,sizeof(char),2))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp137);
goto _LLE7;_LL104: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp138=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp138->tag != 4)goto _LL106;else{_tmp139=_tmp138->f1;if(_tmp138->f2 != Cyc_Absyn_PreInc)goto _LL106;}}_LL105:
# 695
{const char*_tmp443;Cyc_Absyndump_dump(((_tmp443="++",_tag_dyneither(_tmp443,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp139);goto _LLE7;_LL106: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13A=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp13A->tag != 4)goto _LL108;else{_tmp13B=_tmp13A->f1;if(_tmp13A->f2 != Cyc_Absyn_PreDec)goto _LL108;}}_LL107:
{const char*_tmp444;Cyc_Absyndump_dump(((_tmp444="--",_tag_dyneither(_tmp444,sizeof(char),3))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp13B);goto _LLE7;_LL108: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13C=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp13C->tag != 4)goto _LL10A;else{_tmp13D=_tmp13C->f1;if(_tmp13C->f2 != Cyc_Absyn_PostInc)goto _LL10A;}}_LL109:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp13D);{const char*_tmp445;Cyc_Absyndump_dump(((_tmp445="++",_tag_dyneither(_tmp445,sizeof(char),3))));}goto _LLE7;_LL10A: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp13E=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp13E->tag != 4)goto _LL10C;else{_tmp13F=_tmp13E->f1;if(_tmp13E->f2 != Cyc_Absyn_PostDec)goto _LL10C;}}_LL10B:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp13F);{const char*_tmp446;Cyc_Absyndump_dump(((_tmp446="--",_tag_dyneither(_tmp446,sizeof(char),3))));}goto _LLE7;_LL10C: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp140=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp140->tag != 5)goto _LL10E;else{_tmp141=_tmp140->f1;_tmp142=_tmp140->f2;_tmp143=_tmp140->f3;}}_LL10D:
# 701
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp141);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp142);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp143);
goto _LLE7;_LL10E: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp144=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp144->tag != 6)goto _LL110;else{_tmp145=_tmp144->f1;_tmp146=_tmp144->f2;}}_LL10F:
# 708
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp145);{const char*_tmp447;Cyc_Absyndump_dump(((_tmp447=" && ",_tag_dyneither(_tmp447,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp146);goto _LLE7;_LL110: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp147=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp147->tag != 7)goto _LL112;else{_tmp148=_tmp147->f1;_tmp149=_tmp147->f2;}}_LL111:
# 712
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp148);{const char*_tmp448;Cyc_Absyndump_dump(((_tmp448=" || ",_tag_dyneither(_tmp448,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp149);goto _LLE7;_LL112: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp14A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp14A->tag != 8)goto _LL114;else{_tmp14B=_tmp14A->f1;_tmp14C=_tmp14A->f2;}}_LL113:
# 718
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumpexp_prec(myprec,_tmp14B);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec,_tmp14C);
Cyc_Absyndump_dump_char((int)')');
goto _LLE7;_LL114: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp14D=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp14D->tag != 9)goto _LL116;else{_tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;}}_LL115:
# 725
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp14E);
{const char*_tmp449;Cyc_Absyndump_dump_nospace(((_tmp449="(",_tag_dyneither(_tmp449,sizeof(char),2))));}{
# 731
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp14E->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp14F);
{const char*_tmp44A;Cyc_Absyndump_dump_nospace(((_tmp44A=")",_tag_dyneither(_tmp44A,sizeof(char),2))));}
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LLE7;};_LL116: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp150=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp150->tag != 10)goto _LL118;else{_tmp151=_tmp150->f1;}}_LL117:
# 740
 Cyc_Absyndump_dumploc(e->loc);
{const char*_tmp44B;Cyc_Absyndump_dump(((_tmp44B="throw",_tag_dyneither(_tmp44B,sizeof(char),6))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp151);goto _LLE7;_LL118: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp152=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp152->tag != 11)goto _LL11A;else{_tmp153=_tmp152->f1;}}_LL119:
# 743
 _tmp155=_tmp153;goto _LL11B;_LL11A: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp154=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp154->tag != 12)goto _LL11C;else{_tmp155=_tmp154->f1;}}_LL11B:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp155);goto _LLE7;_LL11C: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp156=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp156->tag != 13)goto _LL11E;else{_tmp157=(void*)_tmp156->f1;_tmp158=_tmp156->f2;}}_LL11D:
# 747
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp157);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp158);
goto _LLE7;_LL11E: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp159=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp159->tag != 14)goto _LL120;else{_tmp15A=_tmp159->f1;}}_LL11F:
# 751
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);goto _LLE7;_LL120: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp15B=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp15B->tag != 15)goto _LL122;else{_tmp15C=_tmp15B->f1;_tmp15D=_tmp15B->f2;}}_LL121:
# 754
{const char*_tmp44C;Cyc_Absyndump_dump(((_tmp44C="new ",_tag_dyneither(_tmp44C,sizeof(char),5))));}Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);goto _LLE7;_LL122: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp15E=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp15E->tag != 16)goto _LL124;else{_tmp15F=(void*)_tmp15E->f1;}}_LL123:
# 757
{const char*_tmp44D;Cyc_Absyndump_dump(((_tmp44D="sizeof(",_tag_dyneither(_tmp44D,sizeof(char),8))));}Cyc_Absyndump_dumptyp(_tmp15F);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL124: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp160=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp160->tag != 17)goto _LL126;else{_tmp161=_tmp160->f1;}}_LL125:
# 760
{const char*_tmp44E;Cyc_Absyndump_dump(((_tmp44E="sizeof(",_tag_dyneither(_tmp44E,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp161);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL126: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp162=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp162->tag != 38)goto _LL128;else{_tmp163=(void*)_tmp162->f1;}}_LL127:
# 763
{const char*_tmp44F;Cyc_Absyndump_dump(((_tmp44F="valueof(",_tag_dyneither(_tmp44F,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp163);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL128: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp164=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp164->tag != 39)goto _LL12A;else{_tmp165=_tmp164->f1;_tmp166=_tmp164->f2;}}_LL129:
# 766
{const char*_tmp450;Cyc_Absyndump_dump(((_tmp450="__asm__",_tag_dyneither(_tmp450,sizeof(char),8))));}
if(_tmp165){const char*_tmp451;Cyc_Absyndump_dump(((_tmp451=" volatile ",_tag_dyneither(_tmp451,sizeof(char),11))));}
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp166);
Cyc_Absyndump_dump_char((int)')');
goto _LLE7;_LL12A: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp167=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp167->tag != 37)goto _LL12C;else{_tmp168=_tmp167->f1;_tmp169=_tmp167->f2;}}_LL12B:
# 774
{const char*_tmp452;Cyc_Absyndump_dump(((_tmp452="tagcheck(",_tag_dyneither(_tmp452,sizeof(char),10))));}Cyc_Absyndump_dumpexp(_tmp168);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp169);
Cyc_Absyndump_dump_char((int)')');
goto _LLE7;_LL12C: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp16A=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp16A->tag != 18)goto _LL12E;else{_tmp16B=(void*)_tmp16A->f1;_tmp16C=_tmp16A->f2;}}_LL12D:
# 779
{const char*_tmp453;Cyc_Absyndump_dump(((_tmp453="offsetof(",_tag_dyneither(_tmp453,sizeof(char),10))));}Cyc_Absyndump_dumptyp(_tmp16B);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
{void*_stmttmpE=(void*)_tmp16C->hd;void*_tmp1C7=_stmttmpE;struct _dyneither_ptr*_tmp1C9;unsigned int _tmp1CB;_LL157: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp1C8=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp1C7;if(_tmp1C8->tag != 0)goto _LL159;else{_tmp1C9=_tmp1C8->f1;}}_LL158:
# 783
 Cyc_Absyndump_dump_nospace(*_tmp1C9);
goto _LL156;_LL159: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp1CA=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp1C7;if(_tmp1CA->tag != 1)goto _LL156;else{_tmp1CB=_tmp1CA->f1;}}_LL15A:
# 786
{const char*_tmp457;void*_tmp456[1];struct Cyc_Int_pa_PrintArg_struct _tmp455;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp455.tag=1,((_tmp455.f1=(unsigned long)((int)_tmp1CB),((_tmp456[0]=& _tmp455,Cyc_aprintf(((_tmp457="%d",_tag_dyneither(_tmp457,sizeof(char),3))),_tag_dyneither(_tmp456,sizeof(void*),1)))))))));}
goto _LL156;_LL156:;}
# 789
if(_tmp16C->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 791
Cyc_Absyndump_dump_char((int)')');
goto _LLE7;_LL12E: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp16D=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp16D->tag != 19)goto _LL130;else{_tmp16E=_tmp16D->f1;}}_LL12F:
# 795
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp16E);goto _LLE7;_LL130: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp16F=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp16F->tag != 20)goto _LL132;else{_tmp170=_tmp16F->f1;_tmp171=_tmp16F->f2;}}_LL131:
# 798
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp170);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp171);
goto _LLE7;_LL132: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp172=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp172->tag != 21)goto _LL134;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LL133:
# 802
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp173);{const char*_tmp458;Cyc_Absyndump_dump_nospace(((_tmp458="->",_tag_dyneither(_tmp458,sizeof(char),3))));}Cyc_Absyndump_dump_nospace(*_tmp174);
goto _LLE7;_LL134: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp175=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp175->tag != 22)goto _LL136;else{_tmp176=_tmp175->f1;_tmp177=_tmp175->f2;}}_LL135:
# 806
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp176);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp177);Cyc_Absyndump_dump_char((int)']');goto _LLE7;_LL136: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp178=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp178->tag != 23)goto _LL138;else{_tmp179=_tmp178->f1;}}_LL137:
# 810
{const char*_tmp459;Cyc_Absyndump_dump(((_tmp459="$(",_tag_dyneither(_tmp459,sizeof(char),3))));}Cyc_Absyndump_dumpexps_prec(20,_tmp179);Cyc_Absyndump_dump_char((int)')');goto _LLE7;_LL138: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp17A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp17A->tag != 24)goto _LL13A;else{_tmp17B=_tmp17A->f1;_tmp17C=_tmp17A->f2;}}_LL139:
# 813
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp17B).f3);
Cyc_Absyndump_dump_char((int)')');
{const char*_tmp45C;const char*_tmp45B;const char*_tmp45A;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp17C,((_tmp45A="{",_tag_dyneither(_tmp45A,sizeof(char),2))),((_tmp45B="}",_tag_dyneither(_tmp45B,sizeof(char),2))),((_tmp45C=",",_tag_dyneither(_tmp45C,sizeof(char),2))));}
goto _LLE7;_LL13A: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp17D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp17D->tag != 25)goto _LL13C;else{_tmp17E=_tmp17D->f1;}}_LL13B:
# 820
{const char*_tmp45F;const char*_tmp45E;const char*_tmp45D;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp17E,((_tmp45D="{",_tag_dyneither(_tmp45D,sizeof(char),2))),((_tmp45E="}",_tag_dyneither(_tmp45E,sizeof(char),2))),((_tmp45F=",",_tag_dyneither(_tmp45F,sizeof(char),2))));}
goto _LLE7;_LL13C: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp17F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp17F->tag != 26)goto _LL13E;else{_tmp180=_tmp17F->f1;_tmp181=_tmp17F->f2;_tmp182=_tmp17F->f3;}}_LL13D:
# 824
{const char*_tmp460;Cyc_Absyndump_dump(((_tmp460="{for",_tag_dyneither(_tmp460,sizeof(char),5))));}Cyc_Absyndump_dump_str((*_tmp180->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp181);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp182);Cyc_Absyndump_dump_char((int)'}');
goto _LLE7;_LL13E: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp183=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp183->tag != 27)goto _LL140;else{_tmp184=_tmp183->f1;_tmp185=(void*)_tmp183->f2;}}_LL13F:
# 829
{const char*_tmp461;Cyc_Absyndump_dump(((_tmp461="{for x ",_tag_dyneither(_tmp461,sizeof(char),8))));}Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp184);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp185);Cyc_Absyndump_dump_char((int)'}');
goto _LLE7;_LL140: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp186=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp186->tag != 28)goto _LL142;else{_tmp187=_tmp186->f1;_tmp188=_tmp186->f2;_tmp189=_tmp186->f3;}}_LL141:
# 834
 Cyc_Absyndump_dumpqvar(_tmp187);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp188 != 0)
Cyc_Absyndump_dumptps(_tmp188);
{const char*_tmp464;const char*_tmp463;const char*_tmp462;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp189,((_tmp462="",_tag_dyneither(_tmp462,sizeof(char),1))),((_tmp463="}",_tag_dyneither(_tmp463,sizeof(char),2))),((_tmp464=",",_tag_dyneither(_tmp464,sizeof(char),2))));}
goto _LLE7;_LL142: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp18A=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp18A->tag != 29)goto _LL144;else{_tmp18B=_tmp18A->f2;}}_LL143:
# 842
{const char*_tmp467;const char*_tmp466;const char*_tmp465;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp18B,((_tmp465="{",_tag_dyneither(_tmp465,sizeof(char),2))),((_tmp466="}",_tag_dyneither(_tmp466,sizeof(char),2))),((_tmp467=",",_tag_dyneither(_tmp467,sizeof(char),2))));}
goto _LLE7;_LL144: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp18C=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp18C->tag != 30)goto _LL146;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f3;}}_LL145:
# 846
 Cyc_Absyndump_dumpqvar(_tmp18E->name);
if(_tmp18D != 0){const char*_tmp46A;const char*_tmp469;const char*_tmp468;((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp18D,((_tmp468="(",_tag_dyneither(_tmp468,sizeof(char),2))),((_tmp469=")",_tag_dyneither(_tmp469,sizeof(char),2))),((_tmp46A=",",_tag_dyneither(_tmp46A,sizeof(char),2))));}
goto _LLE7;_LL146: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp18F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp18F->tag != 31)goto _LL148;else{_tmp190=_tmp18F->f1;}}_LL147:
# 850
 Cyc_Absyndump_dumpqvar(_tmp190);goto _LLE7;_LL148: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp191=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp191->tag != 32)goto _LL14A;else{_tmp192=_tmp191->f1;}}_LL149:
 Cyc_Absyndump_dumpqvar(_tmp192);goto _LLE7;_LL14A: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp193=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp193->tag != 33)goto _LL14C;else{_tmp194=(_tmp193->f1).is_calloc;_tmp195=(_tmp193->f1).rgn;_tmp196=(_tmp193->f1).elt_type;_tmp197=(_tmp193->f1).num_elts;}}_LL14B:
# 854
 Cyc_Absyndump_dumploc(_tmp197->loc);
if(_tmp194){
if(_tmp195 != 0){
{const char*_tmp46B;Cyc_Absyndump_dump(((_tmp46B="rcalloc(",_tag_dyneither(_tmp46B,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp195);{const char*_tmp46C;Cyc_Absyndump_dump(((_tmp46C=",",_tag_dyneither(_tmp46C,sizeof(char),2))));};}else{
# 860
const char*_tmp46D;Cyc_Absyndump_dump(((_tmp46D="calloc",_tag_dyneither(_tmp46D,sizeof(char),7))));}
# 862
Cyc_Absyndump_dumpexp(_tmp197);
{const char*_tmp46E;Cyc_Absyndump_dump(((_tmp46E=",",_tag_dyneither(_tmp46E,sizeof(char),2))));}
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp196)),0));{
const char*_tmp46F;Cyc_Absyndump_dump(((_tmp46F=")",_tag_dyneither(_tmp46F,sizeof(char),2))));};}else{
# 867
if(_tmp195 != 0){
{const char*_tmp470;Cyc_Absyndump_dump(((_tmp470="rmalloc(",_tag_dyneither(_tmp470,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp195);{const char*_tmp471;Cyc_Absyndump_dump(((_tmp471=",",_tag_dyneither(_tmp471,sizeof(char),2))));};}else{
# 871
const char*_tmp472;Cyc_Absyndump_dump(((_tmp472="malloc(",_tag_dyneither(_tmp472,sizeof(char),8))));}
# 874
if(_tmp196 != 0)
Cyc_Absyndump_dumpexp(Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp196,0),_tmp197,0));else{
# 877
Cyc_Absyndump_dumpexp(_tmp197);}{
const char*_tmp473;Cyc_Absyndump_dump(((_tmp473=")",_tag_dyneither(_tmp473,sizeof(char),2))));};}
# 880
goto _LLE7;_LL14C: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp198=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp198->tag != 34)goto _LL14E;else{_tmp199=_tmp198->f1;_tmp19A=_tmp198->f2;}}_LL14D:
# 883
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp199);
{const char*_tmp474;Cyc_Absyndump_dump_nospace(((_tmp474=":=:",_tag_dyneither(_tmp474,sizeof(char),4))));}
Cyc_Absyndump_dumpexp_prec(myprec,_tmp19A);
goto _LLE7;_LL14E: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp19B=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp19B->tag != 35)goto _LL150;else{_tmp19C=_tmp19B->f1;_tmp19D=_tmp19B->f2;}}_LL14F:
# 890
{const char*_tmp477;const char*_tmp476;const char*_tmp475;((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp19D,((_tmp475="{",_tag_dyneither(_tmp475,sizeof(char),2))),((_tmp476="}",_tag_dyneither(_tmp476,sizeof(char),2))),((_tmp477=",",_tag_dyneither(_tmp477,sizeof(char),2))));}
goto _LLE7;_LL150: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp19E=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp116;if(_tmp19E->tag != 36)goto _LLE7;else{_tmp19F=_tmp19E->f1;}}_LL151:
# 894
{const char*_tmp478;Cyc_Absyndump_dump_nospace(((_tmp478="({",_tag_dyneither(_tmp478,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp19F,1);
{const char*_tmp479;Cyc_Absyndump_dump_nospace(((_tmp479="})",_tag_dyneither(_tmp479,sizeof(char),3))));}
goto _LLE7;_LLE7:;}
# 899
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 903
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 907
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp1F1=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp1F1->where_clause == 0  && (_tmp1F1->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp47A;Cyc_Absyndump_dump(((_tmp47A="default:",_tag_dyneither(_tmp47A,sizeof(char),9))));}else{
# 913
{const char*_tmp47B;Cyc_Absyndump_dump(((_tmp47B="case",_tag_dyneither(_tmp47B,sizeof(char),5))));}
Cyc_Absyndump_dumppat(_tmp1F1->pattern);
if(_tmp1F1->where_clause != 0){
{const char*_tmp47C;Cyc_Absyndump_dump(((_tmp47C="&&",_tag_dyneither(_tmp47C,sizeof(char),3))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1F1->where_clause));}{
# 919
const char*_tmp47D;Cyc_Absyndump_dump_nospace(((_tmp47D=":",_tag_dyneither(_tmp47D,sizeof(char),2))));};}
# 921
Cyc_Absyndump_dumpstmt(_tmp1F1->body,0);}}
# 925
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_stmttmp10=s->r;void*_tmp1F6=_stmttmp10;struct Cyc_Absyn_Exp*_tmp1F9;struct Cyc_Absyn_Stmt*_tmp1FB;struct Cyc_Absyn_Stmt*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FF;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_Absyn_Stmt*_tmp202;struct Cyc_Absyn_Stmt*_tmp203;struct Cyc_Absyn_Exp*_tmp205;struct Cyc_Absyn_Stmt*_tmp206;struct _dyneither_ptr*_tmp20A;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*_tmp20D;struct Cyc_Absyn_Exp*_tmp20E;struct Cyc_Absyn_Stmt*_tmp20F;struct Cyc_Absyn_Exp*_tmp211;struct Cyc_List_List*_tmp212;struct Cyc_Absyn_Decl*_tmp214;struct Cyc_Absyn_Stmt*_tmp215;struct _dyneither_ptr*_tmp217;struct Cyc_Absyn_Stmt*_tmp218;struct Cyc_Absyn_Stmt*_tmp21A;struct Cyc_Absyn_Exp*_tmp21B;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_Stmt*_tmp220;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Exp*_tmp223;_LL15C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp1F7=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp1F7->tag != 0)goto _LL15E;}_LL15D:
 Cyc_Absyndump_dump_semi();goto _LL15B;_LL15E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp1F8=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp1F8->tag != 1)goto _LL160;else{_tmp1F9=_tmp1F8->f1;}}_LL15F:
 Cyc_Absyndump_dumpexp(_tmp1F9);Cyc_Absyndump_dump_semi();goto _LL15B;_LL160: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp1FA=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp1FA->tag != 2)goto _LL162;else{_tmp1FB=_tmp1FA->f1;_tmp1FC=_tmp1FA->f2;}}_LL161:
# 934
 if(Cyc_Absynpp_is_declaration(_tmp1FB)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1FB,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 937
Cyc_Absyndump_dumpstmt(_tmp1FB,0);}
if(Cyc_Absynpp_is_declaration(_tmp1FC)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1FC,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 944
Cyc_Absyndump_dumpstmt(_tmp1FC,expstmt);}
goto _LL15B;_LL162: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp1FD=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp1FD->tag != 3)goto _LL164;else{if(_tmp1FD->f1 != 0)goto _LL164;}}_LL163:
{const char*_tmp47E;Cyc_Absyndump_dump(((_tmp47E="return;",_tag_dyneither(_tmp47E,sizeof(char),8))));}goto _LL15B;_LL164: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp1FE=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp1FE->tag != 3)goto _LL166;else{_tmp1FF=_tmp1FE->f1;}}_LL165:
{const char*_tmp47F;Cyc_Absyndump_dump(((_tmp47F="return",_tag_dyneither(_tmp47F,sizeof(char),7))));}Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1FF));Cyc_Absyndump_dump_semi();goto _LL15B;_LL166: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp200=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp200->tag != 4)goto _LL168;else{_tmp201=_tmp200->f1;_tmp202=_tmp200->f2;_tmp203=_tmp200->f3;}}_LL167:
# 949
{const char*_tmp480;Cyc_Absyndump_dump(((_tmp480="if(",_tag_dyneither(_tmp480,sizeof(char),4))));}Cyc_Absyndump_dumpexp(_tmp201);
{void*_stmttmp11=_tmp202->r;void*_tmp227=_stmttmp11;_LL183: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp228=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp227;if(_tmp228->tag != 2)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp229=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp227;if(_tmp229->tag != 12)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp22A=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp227;if(_tmp22A->tag != 4)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp22B=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp227;if(_tmp22B->tag != 13)goto _LL18B;}_LL18A:
{const char*_tmp481;Cyc_Absyndump_dump_nospace(((_tmp481="){",_tag_dyneither(_tmp481,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp202,0);Cyc_Absyndump_dump_char((int)'}');goto _LL182;_LL18B:;_LL18C:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp202,0);_LL182:;}
# 957
{void*_stmttmp12=_tmp203->r;void*_tmp22D=_stmttmp12;_LL18E: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp22E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp22D;if(_tmp22E->tag != 0)goto _LL190;}_LL18F:
 goto _LL18D;_LL190:;_LL191:
{const char*_tmp482;Cyc_Absyndump_dump(((_tmp482="else{",_tag_dyneither(_tmp482,sizeof(char),6))));}Cyc_Absyndump_dumpstmt(_tmp203,0);Cyc_Absyndump_dump_char((int)'}');goto _LL18D;_LL18D:;}
# 961
goto _LL15B;_LL168: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp204=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp204->tag != 5)goto _LL16A;else{_tmp205=(_tmp204->f1).f1;_tmp206=_tmp204->f2;}}_LL169:
# 963
{const char*_tmp483;Cyc_Absyndump_dump(((_tmp483="while(",_tag_dyneither(_tmp483,sizeof(char),7))));}Cyc_Absyndump_dumpexp(_tmp205);{const char*_tmp484;Cyc_Absyndump_dump_nospace(((_tmp484="){",_tag_dyneither(_tmp484,sizeof(char),3))));}
Cyc_Absyndump_dumpstmt(_tmp206,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL15B;_LL16A: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp207=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp207->tag != 6)goto _LL16C;}_LL16B:
{const char*_tmp485;Cyc_Absyndump_dump(((_tmp485="break;",_tag_dyneither(_tmp485,sizeof(char),7))));}goto _LL15B;_LL16C: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp208=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp208->tag != 7)goto _LL16E;}_LL16D:
{const char*_tmp486;Cyc_Absyndump_dump(((_tmp486="continue;",_tag_dyneither(_tmp486,sizeof(char),10))));}goto _LL15B;_LL16E: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp209=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp209->tag != 8)goto _LL170;else{_tmp20A=_tmp209->f1;}}_LL16F:
{const char*_tmp487;Cyc_Absyndump_dump(((_tmp487="goto",_tag_dyneither(_tmp487,sizeof(char),5))));}Cyc_Absyndump_dump_str(_tmp20A);Cyc_Absyndump_dump_semi();goto _LL15B;_LL170: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp20B=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp20B->tag != 9)goto _LL172;else{_tmp20C=_tmp20B->f1;_tmp20D=(_tmp20B->f2).f1;_tmp20E=(_tmp20B->f3).f1;_tmp20F=_tmp20B->f4;}}_LL171:
# 972
{const char*_tmp488;Cyc_Absyndump_dump(((_tmp488="for(",_tag_dyneither(_tmp488,sizeof(char),5))));}Cyc_Absyndump_dumpexp(_tmp20C);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp20D);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp20E);
{const char*_tmp489;Cyc_Absyndump_dump_nospace(((_tmp489="){",_tag_dyneither(_tmp489,sizeof(char),3))));}Cyc_Absyndump_dumpstmt(_tmp20F,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL15B;_LL172: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp210=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp210->tag != 10)goto _LL174;else{_tmp211=_tmp210->f1;_tmp212=_tmp210->f2;}}_LL173:
# 976
{const char*_tmp48A;Cyc_Absyndump_dump(((_tmp48A="switch(",_tag_dyneither(_tmp48A,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp211);{const char*_tmp48B;Cyc_Absyndump_dump_nospace(((_tmp48B="){",_tag_dyneither(_tmp48B,sizeof(char),3))));}
Cyc_Absyndump_dumpswitchclauses(_tmp212);
Cyc_Absyndump_dump_char((int)'}');
goto _LL15B;_LL174: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp213=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp213->tag != 12)goto _LL176;else{_tmp214=_tmp213->f1;_tmp215=_tmp213->f2;}}_LL175:
# 981
 Cyc_Absyndump_dumpdecl(_tmp214);Cyc_Absyndump_dumpstmt(_tmp215,expstmt);goto _LL15B;_LL176: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp216=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp216->tag != 13)goto _LL178;else{_tmp217=_tmp216->f1;_tmp218=_tmp216->f2;}}_LL177:
# 987
 if(Cyc_Absynpp_is_declaration(_tmp218)){
Cyc_Absyndump_dump_str(_tmp217);
if(expstmt){const char*_tmp48C;Cyc_Absyndump_dump_nospace(((_tmp48C=": ({",_tag_dyneither(_tmp48C,sizeof(char),5))));}else{const char*_tmp48D;Cyc_Absyndump_dump_nospace(((_tmp48D=": {",_tag_dyneither(_tmp48D,sizeof(char),4))));}
Cyc_Absyndump_dumpstmt(_tmp218,expstmt);
if(expstmt){const char*_tmp48E;Cyc_Absyndump_dump_nospace(((_tmp48E="});}",_tag_dyneither(_tmp48E,sizeof(char),5))));}else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 993
Cyc_Absyndump_dump_str(_tmp217);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp218,expstmt);}
# 995
goto _LL15B;_LL178: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp219=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp219->tag != 14)goto _LL17A;else{_tmp21A=_tmp219->f1;_tmp21B=(_tmp219->f2).f1;}}_LL179:
# 998
{const char*_tmp48F;Cyc_Absyndump_dump(((_tmp48F="do{",_tag_dyneither(_tmp48F,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp21A,0);
{const char*_tmp490;Cyc_Absyndump_dump_nospace(((_tmp490="}while(",_tag_dyneither(_tmp490,sizeof(char),8))));}Cyc_Absyndump_dumpexp(_tmp21B);{const char*_tmp491;Cyc_Absyndump_dump_nospace(((_tmp491=");",_tag_dyneither(_tmp491,sizeof(char),3))));}
goto _LL15B;_LL17A: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp21C=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp21C->tag != 11)goto _LL17C;else{if(_tmp21C->f1 != 0)goto _LL17C;}}_LL17B:
# 1002
{const char*_tmp492;Cyc_Absyndump_dump(((_tmp492="fallthru;",_tag_dyneither(_tmp492,sizeof(char),10))));}goto _LL15B;_LL17C: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp21D=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp21D->tag != 11)goto _LL17E;else{_tmp21E=_tmp21D->f1;}}_LL17D:
# 1004
{const char*_tmp493;Cyc_Absyndump_dump(((_tmp493="fallthru(",_tag_dyneither(_tmp493,sizeof(char),10))));}Cyc_Absyndump_dumpexps_prec(20,_tmp21E);{const char*_tmp494;Cyc_Absyndump_dump_nospace(((_tmp494=");",_tag_dyneither(_tmp494,sizeof(char),3))));}goto _LL15B;_LL17E: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp21F=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp21F->tag != 15)goto _LL180;else{_tmp220=_tmp21F->f1;_tmp221=_tmp21F->f2;}}_LL17F:
# 1007
{const char*_tmp495;Cyc_Absyndump_dump(((_tmp495="try",_tag_dyneither(_tmp495,sizeof(char),4))));}Cyc_Absyndump_dumpstmt(_tmp220,0);
{const char*_tmp496;Cyc_Absyndump_dump(((_tmp496="catch{",_tag_dyneither(_tmp496,sizeof(char),7))));}
Cyc_Absyndump_dumpswitchclauses(_tmp221);Cyc_Absyndump_dump_char((int)'}');
goto _LL15B;_LL180: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp222=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp1F6;if(_tmp222->tag != 16)goto _LL15B;else{_tmp223=_tmp222->f1;}}_LL181:
# 1013
{const char*_tmp497;Cyc_Absyndump_dump(((_tmp497="reset_region(",_tag_dyneither(_tmp497,sizeof(char),14))));}Cyc_Absyndump_dumpexp(_tmp223);{const char*_tmp498;Cyc_Absyndump_dump(((_tmp498=");",_tag_dyneither(_tmp498,sizeof(char),3))));}
goto _LL15B;_LL15B:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1019
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
{const char*_tmp49B;const char*_tmp49A;const char*_tmp499;((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,(*dp).f1,((_tmp499="",_tag_dyneither(_tmp499,sizeof(char),1))),((_tmp49A="=",_tag_dyneither(_tmp49A,sizeof(char),2))),((_tmp49B="=",_tag_dyneither(_tmp49B,sizeof(char),2))));}
Cyc_Absyndump_dumppat((*dp).f2);}
# 1024
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_stmttmp13=p->r;void*_tmp249=_stmttmp13;int _tmp24D;int _tmp24F;int _tmp251;char _tmp253;struct _dyneither_ptr _tmp255;struct Cyc_Absyn_Vardecl*_tmp257;struct Cyc_Absyn_Vardecl*_tmp25A;struct Cyc_Absyn_Pat*_tmp25B;struct Cyc_List_List*_tmp25D;int _tmp25E;struct Cyc_Absyn_Pat*_tmp260;struct Cyc_Absyn_Vardecl*_tmp262;struct Cyc_Absyn_Vardecl*_tmp265;struct Cyc_Absyn_Pat*_tmp266;struct Cyc_Absyn_Tvar*_tmp268;struct Cyc_Absyn_Vardecl*_tmp269;struct _tuple0*_tmp26B;struct _tuple0*_tmp26D;struct Cyc_List_List*_tmp26E;int _tmp26F;union Cyc_Absyn_AggrInfoU _tmp271;struct Cyc_List_List*_tmp272;struct Cyc_List_List*_tmp273;int _tmp274;struct Cyc_List_List*_tmp276;struct Cyc_List_List*_tmp277;int _tmp278;struct Cyc_Absyn_Datatypefield*_tmp27A;struct Cyc_List_List*_tmp27B;int _tmp27C;struct Cyc_Absyn_Enumfield*_tmp27E;struct Cyc_Absyn_Enumfield*_tmp280;struct Cyc_Absyn_Exp*_tmp282;_LL193: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp24A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LL195;}_LL194:
 Cyc_Absyndump_dump_char((int)'_');goto _LL192;_LL195: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp24B=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp24B->tag != 8)goto _LL197;}_LL196:
{const char*_tmp49C;Cyc_Absyndump_dump(((_tmp49C="NULL",_tag_dyneither(_tmp49C,sizeof(char),5))));}goto _LL192;_LL197: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp24C=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp24C->tag != 9)goto _LL199;else{if(_tmp24C->f1 != Cyc_Absyn_None)goto _LL199;_tmp24D=_tmp24C->f2;}}_LL198:
 _tmp24F=_tmp24D;goto _LL19A;_LL199: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp24E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp24E->tag != 9)goto _LL19B;else{if(_tmp24E->f1 != Cyc_Absyn_Signed)goto _LL19B;_tmp24F=_tmp24E->f2;}}_LL19A:
{const char*_tmp4A0;void*_tmp49F[1];struct Cyc_Int_pa_PrintArg_struct _tmp49E;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp49E.tag=1,((_tmp49E.f1=(unsigned long)_tmp24F,((_tmp49F[0]=& _tmp49E,Cyc_aprintf(((_tmp4A0="%d",_tag_dyneither(_tmp4A0,sizeof(char),3))),_tag_dyneither(_tmp49F,sizeof(void*),1)))))))));}goto _LL192;_LL19B: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp250=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp250->tag != 9)goto _LL19D;else{if(_tmp250->f1 != Cyc_Absyn_Unsigned)goto _LL19D;_tmp251=_tmp250->f2;}}_LL19C:
{const char*_tmp4A4;void*_tmp4A3[1];struct Cyc_Int_pa_PrintArg_struct _tmp4A2;Cyc_Absyndump_dump((struct _dyneither_ptr)((_tmp4A2.tag=1,((_tmp4A2.f1=(unsigned int)_tmp251,((_tmp4A3[0]=& _tmp4A2,Cyc_aprintf(((_tmp4A4="%u",_tag_dyneither(_tmp4A4,sizeof(char),3))),_tag_dyneither(_tmp4A3,sizeof(void*),1)))))))));}goto _LL192;_LL19D: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp252=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp252->tag != 10)goto _LL19F;else{_tmp253=_tmp252->f1;}}_LL19E:
# 1032
{const char*_tmp4A5;Cyc_Absyndump_dump(((_tmp4A5="'",_tag_dyneither(_tmp4A5,sizeof(char),2))));}Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp253));{const char*_tmp4A6;Cyc_Absyndump_dump_nospace(((_tmp4A6="'",_tag_dyneither(_tmp4A6,sizeof(char),2))));}goto _LL192;_LL19F: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp254=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp254->tag != 11)goto _LL1A1;else{_tmp255=_tmp254->f1;}}_LL1A0:
 Cyc_Absyndump_dump(_tmp255);goto _LL192;_LL1A1: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp256=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp256->tag != 1)goto _LL1A3;else{_tmp257=_tmp256->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp258=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp256->f2)->r;if(_tmp258->tag != 0)goto _LL1A3;};}}_LL1A2:
 Cyc_Absyndump_dumpqvar(_tmp257->name);goto _LL192;_LL1A3: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp259=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp259->tag != 1)goto _LL1A5;else{_tmp25A=_tmp259->f1;_tmp25B=_tmp259->f2;}}_LL1A4:
 Cyc_Absyndump_dumpqvar(_tmp25A->name);{const char*_tmp4A7;Cyc_Absyndump_dump(((_tmp4A7=" as ",_tag_dyneither(_tmp4A7,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp25B);goto _LL192;_LL1A5: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp25C=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp25C->tag != 4)goto _LL1A7;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}_LL1A6: {
# 1037
const char*_tmp4A9;const char*_tmp4A8;struct _dyneither_ptr term=_tmp25E?(_tmp4A9=", ...)",_tag_dyneither(_tmp4A9,sizeof(char),7)):((_tmp4A8=")",_tag_dyneither(_tmp4A8,sizeof(char),2)));
{const char*_tmp4AB;const char*_tmp4AA;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp25D,((_tmp4AA="$(",_tag_dyneither(_tmp4AA,sizeof(char),3))),term,((_tmp4AB=",",_tag_dyneither(_tmp4AB,sizeof(char),2))));}goto _LL192;}_LL1A7: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp25F=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp25F->tag != 5)goto _LL1A9;else{_tmp260=_tmp25F->f1;}}_LL1A8:
{const char*_tmp4AC;Cyc_Absyndump_dump(((_tmp4AC="&",_tag_dyneither(_tmp4AC,sizeof(char),2))));}Cyc_Absyndump_dumppat(_tmp260);goto _LL192;_LL1A9: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp261=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp261->tag != 2)goto _LL1AB;else{_tmp262=_tmp261->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp263=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp261->f2)->r;if(_tmp263->tag != 0)goto _LL1AB;};}}_LL1AA:
# 1041
{const char*_tmp4AD;Cyc_Absyndump_dump(((_tmp4AD="*",_tag_dyneither(_tmp4AD,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp262->name);goto _LL192;_LL1AB: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp264=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp264->tag != 2)goto _LL1AD;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1AC:
# 1043
{const char*_tmp4AE;Cyc_Absyndump_dump(((_tmp4AE="*",_tag_dyneither(_tmp4AE,sizeof(char),2))));}Cyc_Absyndump_dumpqvar(_tmp265->name);{const char*_tmp4AF;Cyc_Absyndump_dump(((_tmp4AF=" as ",_tag_dyneither(_tmp4AF,sizeof(char),5))));}Cyc_Absyndump_dumppat(_tmp266);goto _LL192;_LL1AD: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp267=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp267->tag != 3)goto _LL1AF;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}_LL1AE:
# 1045
 Cyc_Absyndump_dumpqvar(_tmp269->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp268);Cyc_Absyndump_dump_char((int)'>');goto _LL192;_LL1AF: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp26A=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp26A->tag != 14)goto _LL1B1;else{_tmp26B=_tmp26A->f1;}}_LL1B0:
 Cyc_Absyndump_dumpqvar(_tmp26B);goto _LL192;_LL1B1: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp26C=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp26C->tag != 15)goto _LL1B3;else{_tmp26D=_tmp26C->f1;_tmp26E=_tmp26C->f2;_tmp26F=_tmp26C->f3;}}_LL1B2: {
# 1048
const char*_tmp4B1;const char*_tmp4B0;struct _dyneither_ptr term=_tmp26F?(_tmp4B1=", ...)",_tag_dyneither(_tmp4B1,sizeof(char),7)):((_tmp4B0=")",_tag_dyneither(_tmp4B0,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp26D);{const char*_tmp4B3;const char*_tmp4B2;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp26E,((_tmp4B2="(",_tag_dyneither(_tmp4B2,sizeof(char),2))),term,((_tmp4B3=",",_tag_dyneither(_tmp4B3,sizeof(char),2))));}goto _LL192;}_LL1B3: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp270=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp270->tag != 6)goto _LL1B5;else{if(_tmp270->f1 == 0)goto _LL1B5;_tmp271=(_tmp270->f1)->aggr_info;_tmp272=_tmp270->f2;_tmp273=_tmp270->f3;_tmp274=_tmp270->f4;}}_LL1B4: {
# 1051
struct _tuple10 _stmttmp14=Cyc_Absyn_aggr_kinded_name(_tmp271);struct _tuple0*_tmp29A;struct _tuple10 _tmp299=_stmttmp14;_tmp29A=_tmp299.f2;{
const char*_tmp4B5;const char*_tmp4B4;struct _dyneither_ptr term=_tmp274?(_tmp4B5=", ...)",_tag_dyneither(_tmp4B5,sizeof(char),7)):((_tmp4B4=")",_tag_dyneither(_tmp4B4,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp29A);Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4B8;const char*_tmp4B7;const char*_tmp4B6;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp272,((_tmp4B6="[",_tag_dyneither(_tmp4B6,sizeof(char),2))),((_tmp4B7="]",_tag_dyneither(_tmp4B7,sizeof(char),2))),((_tmp4B8=",",_tag_dyneither(_tmp4B8,sizeof(char),2))));}
{const char*_tmp4BA;const char*_tmp4B9;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp273,((_tmp4B9="",_tag_dyneither(_tmp4B9,sizeof(char),1))),term,((_tmp4BA=",",_tag_dyneither(_tmp4BA,sizeof(char),2))));}
goto _LL192;};}_LL1B5: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp275=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp275->tag != 6)goto _LL1B7;else{if(_tmp275->f1 != 0)goto _LL1B7;_tmp276=_tmp275->f2;_tmp277=_tmp275->f3;_tmp278=_tmp275->f4;}}_LL1B6: {
# 1058
const char*_tmp4BC;const char*_tmp4BB;struct _dyneither_ptr term=_tmp278?(_tmp4BC=", ...)",_tag_dyneither(_tmp4BC,sizeof(char),7)):((_tmp4BB=")",_tag_dyneither(_tmp4BB,sizeof(char),2)));
Cyc_Absyndump_dump_char((int)'{');
{const char*_tmp4BF;const char*_tmp4BE;const char*_tmp4BD;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp276,((_tmp4BD="[",_tag_dyneither(_tmp4BD,sizeof(char),2))),((_tmp4BE="]",_tag_dyneither(_tmp4BE,sizeof(char),2))),((_tmp4BF=",",_tag_dyneither(_tmp4BF,sizeof(char),2))));}
{const char*_tmp4C1;const char*_tmp4C0;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp277,((_tmp4C0="",_tag_dyneither(_tmp4C0,sizeof(char),1))),term,((_tmp4C1=",",_tag_dyneither(_tmp4C1,sizeof(char),2))));}
goto _LL192;}_LL1B7: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp279=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp279->tag != 7)goto _LL1B9;else{_tmp27A=_tmp279->f2;_tmp27B=_tmp279->f3;_tmp27C=_tmp279->f4;}}_LL1B8: {
# 1064
const char*_tmp4C3;const char*_tmp4C2;struct _dyneither_ptr term=_tmp27C?(_tmp4C3=", ...)",_tag_dyneither(_tmp4C3,sizeof(char),7)):((_tmp4C2=")",_tag_dyneither(_tmp4C2,sizeof(char),2)));
Cyc_Absyndump_dumpqvar(_tmp27A->name);
if(_tmp27B != 0){const char*_tmp4C5;const char*_tmp4C4;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp27B,((_tmp4C4="(",_tag_dyneither(_tmp4C4,sizeof(char),2))),term,((_tmp4C5=",",_tag_dyneither(_tmp4C5,sizeof(char),2))));}
goto _LL192;}_LL1B9: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp27D=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp27D->tag != 12)goto _LL1BB;else{_tmp27E=_tmp27D->f2;}}_LL1BA:
 _tmp280=_tmp27E;goto _LL1BC;_LL1BB: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp27F=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp27F->tag != 13)goto _LL1BD;else{_tmp280=_tmp27F->f2;}}_LL1BC:
 Cyc_Absyndump_dumpqvar(_tmp280->name);goto _LL192;_LL1BD: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp281=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp249;if(_tmp281->tag != 16)goto _LL192;else{_tmp282=_tmp281->f1;}}_LL1BE:
 Cyc_Absyndump_dumpexp(_tmp282);goto _LL192;_LL192:;}
# 1074
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1079
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
const char*_tmp4C6;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,fields,((_tmp4C6=",",_tag_dyneither(_tmp4C6,sizeof(char),2))));}
# 1082
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
{const char*_tmp4C7;Cyc_Absyndump_dump(((_tmp4C7=" = ",_tag_dyneither(_tmp4C7,sizeof(char),4))));}
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1089
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
const char*_tmp4C8;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,fields,((_tmp4C8=",",_tag_dyneither(_tmp4C8,sizeof(char),2))));}
# 1093
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_stmttmp15=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct _dyneither_ptr*_tmp2B1;struct Cyc_Absyn_Tqual _tmp2B2;void*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Absyn_Aggrfield*_tmp2B0=_stmttmp15;_tmp2B1=_tmp2B0->name;_tmp2B2=_tmp2B0->tq;_tmp2B3=_tmp2B0->type;_tmp2B4=_tmp2B0->width;_tmp2B5=_tmp2B0->attributes;_tmp2B6=_tmp2B0->requires_clause;
# 1097
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1BF:
# 1099
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2B2,_tmp2B3,Cyc_Absyndump_dump_str,_tmp2B1);
Cyc_Absyndump_dumpatts(_tmp2B5);
break;case Cyc_Cyclone_Vc_c: _LL1C0:
# 1103
 Cyc_Absyndump_dumpatts(_tmp2B5);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp2B2,_tmp2B3,Cyc_Absyndump_dump_str,_tmp2B1);
break;}
# 1107
if((unsigned int)_tmp2B6){
{const char*_tmp4C9;Cyc_Absyndump_dump(((_tmp4C9="@requires ",_tag_dyneither(_tmp4C9,sizeof(char),11))));}
Cyc_Absyndump_dumpexp(_tmp2B6);}
# 1111
if(_tmp2B4 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp2B4);}
# 1115
Cyc_Absyndump_dump_semi();}}
# 1119
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1124
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1128
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_stmttmp16=(*pr).f1;void*_tmp2B8=_stmttmp16;_LL1C3: {struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*_tmp2B9=(struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct*)_tmp2B8;if(_tmp2B9->tag != 11)goto _LL1C5;}_LL1C4:
 goto _LL1C2;_LL1C5: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2BA=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2B8;if(_tmp2BA->tag != 1)goto _LL1C7;}_LL1C6:
{const char*_tmp4CA;Cyc_Absyndump_dump(((_tmp4CA="_stdcall",_tag_dyneither(_tmp4CA,sizeof(char),9))));}goto _LL1C2;_LL1C7: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2BB=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2B8;if(_tmp2BB->tag != 2)goto _LL1C9;}_LL1C8:
{const char*_tmp4CB;Cyc_Absyndump_dump(((_tmp4CB="_cdecl",_tag_dyneither(_tmp4CB,sizeof(char),7))));}goto _LL1C2;_LL1C9: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2BC=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2B8;if(_tmp2BC->tag != 3)goto _LL1CB;}_LL1CA:
{const char*_tmp4CC;Cyc_Absyndump_dump(((_tmp4CC="_fastcall",_tag_dyneither(_tmp4CC,sizeof(char),10))));}goto _LL1C2;_LL1CB:;_LL1CC:
 goto _LL1C2;_LL1C2:;}
# 1136
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1138
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1143
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1150
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1152
void*_tmp2C0=t;void*_tmp2C2;void*_tmp2C4;_LL1CE:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0;if(_tmp2C1->tag != 1)goto _LL1D0;else{_tmp2C2=(void*)_tmp2C1->f2;}}if(!(_tmp2C2 != 0))goto _LL1D0;_LL1CF:
 return Cyc_Absyndump_is_char_ptr(_tmp2C2);_LL1D0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2C3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0;if(_tmp2C3->tag != 5)goto _LL1D2;else{_tmp2C4=(_tmp2C3->f1).elt_typ;}}_LL1D1:
# 1155
 L: {
void*_tmp2C5=_tmp2C4;void*_tmp2C7;void*_tmp2C9;_LL1D5:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp2C6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5;if(_tmp2C6->tag != 1)goto _LL1D7;else{_tmp2C7=(void*)_tmp2C6->f2;}}if(!(_tmp2C7 != 0))goto _LL1D7;_LL1D6:
 _tmp2C4=_tmp2C7;goto L;_LL1D7:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp2C8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C5;if(_tmp2C8->tag != 17)goto _LL1D9;else{_tmp2C9=(void*)_tmp2C8->f4;}}if(!(_tmp2C9 != 0))goto _LL1D9;_LL1D8:
 _tmp2C4=_tmp2C9;goto L;_LL1D9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp2CA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2C5;if(_tmp2CA->tag != 6)goto _LL1DB;else{if(_tmp2CA->f2 != Cyc_Absyn_Char_sz)goto _LL1DB;}}_LL1DA:
 return 1;_LL1DB:;_LL1DC:
 return 0;_LL1D4:;}_LL1D2:;_LL1D3:
# 1162
 return 0;_LL1CD:;}
# 1166
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc){
enum Cyc_Absyn_Scope _tmp2CC;struct _tuple0*_tmp2CD;struct Cyc_Absyn_Tqual _tmp2CE;void*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_List_List*_tmp2D1;struct Cyc_Absyn_Vardecl*_tmp2CB=vd;_tmp2CC=_tmp2CB->sc;_tmp2CD=_tmp2CB->name;_tmp2CE=_tmp2CB->tq;_tmp2CF=_tmp2CB->type;_tmp2D0=_tmp2CB->initializer;_tmp2D1=_tmp2CB->attributes;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL1DD:
# 1171
 if(_tmp2CC == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_stmttmp1A=Cyc_Tcutil_compress(_tmp2CF);void*_tmp2D2=_stmttmp1A;_LL1E0: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2D3=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D2;if(_tmp2D3->tag != 9)goto _LL1E2;}_LL1E1:
 goto _LL1DF;_LL1E2:;_LL1E3:
 Cyc_Absyndump_dumpscope(_tmp2CC);_LL1DF:;}else{
# 1177
Cyc_Absyndump_dumpscope(_tmp2CC);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp2CE,_tmp2CF,Cyc_Absyndump_dumpqvar,_tmp2CD);
Cyc_Absyndump_dumpatts(_tmp2D1);
break;case Cyc_Cyclone_Vc_c: _LL1DE:
# 1182
 Cyc_Absyndump_dumpatts(_tmp2D1);
Cyc_Absyndump_dumpscope(_tmp2CC);{
struct _RegionHandle _tmp2D4=_new_region("temp");struct _RegionHandle*temp=& _tmp2D4;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp2CF);
struct _tuple11 _stmttmp17=Cyc_Absynpp_to_tms(temp,_tmp2CE,_tmp2CF);struct Cyc_Absyn_Tqual _tmp2D6;void*_tmp2D7;struct Cyc_List_List*_tmp2D8;struct _tuple11 _tmp2D5=_stmttmp17;_tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;_tmp2D8=_tmp2D5.f3;{
# 1188
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp2D8;for(0;tms2 != 0;tms2=tms2->tl){
void*_stmttmp18=(void*)tms2->hd;void*_tmp2D9=_stmttmp18;struct Cyc_List_List*_tmp2DB;_LL1E6: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp2DA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2D9;if(_tmp2DA->tag != 5)goto _LL1E8;else{_tmp2DB=_tmp2DA->f2;}}_LL1E7:
# 1192
 for(0;_tmp2DB != 0;_tmp2DB=_tmp2DB->tl){
void*_stmttmp19=(void*)_tmp2DB->hd;void*_tmp2DC=_stmttmp19;_LL1EB: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp2DD=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp2DC;if(_tmp2DD->tag != 1)goto _LL1ED;}_LL1EC:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL1EA;_LL1ED: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp2DE=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp2DC;if(_tmp2DE->tag != 2)goto _LL1EF;}_LL1EE:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL1EA;_LL1EF: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp2DF=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp2DC;if(_tmp2DF->tag != 3)goto _LL1F1;}_LL1F0:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL1EA;_LL1F1:;_LL1F2:
 goto _LL1EA;_LL1EA:;}
# 1199
goto _LL1E5;_LL1E8:;_LL1E9:
 goto _LL1E5;_LL1E5:;}}
# 1202
Cyc_Absyndump_dumptq(_tmp2D6);
Cyc_Absyndump_dumpntyp(_tmp2D7);{
struct _tuple19 _tmp4CD;struct _tuple19 _tmp2E0=(_tmp4CD.f1=call_conv,((_tmp4CD.f2=_tmp2CD,_tmp4CD)));
((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(is_cp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D8),Cyc_Absyndump_dump_callconv_qvar,& _tmp2E0);};};}
# 1207
_npop_handler(0);break;
# 1184
;_pop_region(temp);};}
# 1210
if(_tmp2D0 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp2D0);}
# 1214
Cyc_Absyndump_dump_semi();}
# 1217
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged){
const char*_tmp4CE;Cyc_Absyndump_dump(((_tmp4CE="@tagged ",_tag_dyneither(_tmp4CE,sizeof(char),9))));}
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1226
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0){
const char*_tmp4D1;const char*_tmp4D0;const char*_tmp4CF;((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,((_tmp4CF="<",_tag_dyneither(_tmp4CF,sizeof(char),2))),((_tmp4D0=">",_tag_dyneither(_tmp4D0,sizeof(char),2))),((_tmp4D1=",",_tag_dyneither(_tmp4D1,sizeof(char),2))));}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1233
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
{const char*_tmp4D2;Cyc_Absyndump_dump(((_tmp4D2="}",_tag_dyneither(_tmp4D2,sizeof(char),2))));}
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1238
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp2E8;struct _tuple0*_tmp2E9;struct Cyc_Core_Opt*_tmp2EA;struct Cyc_Absyn_Enumdecl*_tmp2E7=ed;_tmp2E8=_tmp2E7->sc;_tmp2E9=_tmp2E7->name;_tmp2EA=_tmp2E7->fields;
Cyc_Absyndump_dumpscope(_tmp2E8);
{const char*_tmp4D3;Cyc_Absyndump_dump(((_tmp4D3="enum ",_tag_dyneither(_tmp4D3,sizeof(char),6))));}
Cyc_Absyndump_dumpqvar(_tmp2E9);
if(_tmp2EA != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp2EA->v);{
const char*_tmp4D4;Cyc_Absyndump_dump_nospace(((_tmp4D4="}",_tag_dyneither(_tmp4D4,sizeof(char),2))));};}}
# 1249
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp2EE;struct _tuple0*_tmp2EF;struct Cyc_List_List*_tmp2F0;struct Cyc_Core_Opt*_tmp2F1;int _tmp2F2;struct Cyc_Absyn_Datatypedecl*_tmp2ED=dd;_tmp2EE=_tmp2ED->sc;_tmp2EF=_tmp2ED->name;_tmp2F0=_tmp2ED->tvs;_tmp2F1=_tmp2ED->fields;_tmp2F2=_tmp2ED->is_extensible;
Cyc_Absyndump_dumpscope(_tmp2EE);
if(_tmp2F2){const char*_tmp4D5;Cyc_Absyndump_dump(((_tmp4D5="@extensible ",_tag_dyneither(_tmp4D5,sizeof(char),13))));}
{const char*_tmp4D6;Cyc_Absyndump_dump(((_tmp4D6="datatype ",_tag_dyneither(_tmp4D6,sizeof(char),10))));}
Cyc_Absyndump_dumpqvar(_tmp2EF);
Cyc_Absyndump_dumptvars(_tmp2F0);
if(_tmp2F1 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp2F1->v);{
const char*_tmp4D7;Cyc_Absyndump_dump_nospace(((_tmp4D7="}",_tag_dyneither(_tmp4D7,sizeof(char),2))));};}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1263
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_stmttmp1B=d->r;void*_tmp2F6=_stmttmp1B;struct Cyc_Absyn_Vardecl*_tmp2F8;struct Cyc_Absyn_Fndecl*_tmp2FA;struct Cyc_Absyn_Aggrdecl*_tmp2FC;struct Cyc_Absyn_Datatypedecl*_tmp2FE;struct Cyc_Absyn_Enumdecl*_tmp300;struct Cyc_Absyn_Pat*_tmp302;struct Cyc_Absyn_Exp*_tmp303;struct Cyc_List_List*_tmp305;struct Cyc_Absyn_Tvar*_tmp307;struct Cyc_Absyn_Vardecl*_tmp308;int _tmp309;struct Cyc_Absyn_Exp*_tmp30A;struct Cyc_Absyn_Tvar*_tmp30C;struct Cyc_Absyn_Vardecl*_tmp30D;struct Cyc_Absyn_Typedefdecl*_tmp30F;struct _dyneither_ptr*_tmp311;struct Cyc_List_List*_tmp312;struct _tuple0*_tmp314;struct Cyc_List_List*_tmp315;struct Cyc_List_List*_tmp317;struct Cyc_List_List*_tmp319;struct Cyc_List_List*_tmp31A;_LL1F4: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2F7=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp2F7->tag != 0)goto _LL1F6;else{_tmp2F8=_tmp2F7->f1;}}_LL1F5:
# 1267
 Cyc_Absyndump_dumpvardecl(_tmp2F8,d->loc);
goto _LL1F3;_LL1F6: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp2F9=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp2F9->tag != 1)goto _LL1F8;else{_tmp2FA=_tmp2F9->f1;}}_LL1F7:
# 1271
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL214:
 Cyc_Absyndump_dumpatts(_tmp2FA->attributes);break;case Cyc_Cyclone_Gcc_c: _LL215:
 break;}
# 1275
if(_tmp2FA->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp31D=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp31E;_LL218: if(_tmp31D != Cyc_Cyclone_Vc_c)goto _LL21A;_LL219:
{const char*_tmp4D8;Cyc_Absyndump_dump(((_tmp4D8="__inline",_tag_dyneither(_tmp4D8,sizeof(char),9))));}goto _LL217;_LL21A: _tmp31E=_tmp31D;_LL21B:
{const char*_tmp4D9;Cyc_Absyndump_dump(((_tmp4D9="inline",_tag_dyneither(_tmp4D9,sizeof(char),7))));}goto _LL217;_LL217:;}
# 1280
Cyc_Absyndump_dumpscope(_tmp2FA->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp4DF;struct Cyc_Absyn_FnInfo _tmp4DE;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4DD;void*t=(void*)((_tmp4DD=_cycalloc(sizeof(*_tmp4DD)),((_tmp4DD[0]=((_tmp4DF.tag=9,((_tmp4DF.f1=((_tmp4DE.tvars=_tmp2FA->tvs,((_tmp4DE.effect=_tmp2FA->effect,((_tmp4DE.ret_tqual=_tmp2FA->ret_tqual,((_tmp4DE.ret_typ=_tmp2FA->ret_type,((_tmp4DE.args=
# 1283
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2FA->args),((_tmp4DE.c_varargs=_tmp2FA->c_varargs,((_tmp4DE.cyc_varargs=_tmp2FA->cyc_varargs,((_tmp4DE.rgn_po=_tmp2FA->rgn_po,((_tmp4DE.attributes=0,_tmp4DE)))))))))))))))))),_tmp4DF)))),_tmp4DD))));
# 1286
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL21C:
# 1288
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp2FA);
break;case Cyc_Cyclone_Vc_c: _LL21D:
# 1291
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_callconv_fdqvar,_tmp2FA);
break;}
# 1294
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp2FA->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F3;};_LL1F8: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp2FB=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp2FB->tag != 6)goto _LL1FA;else{_tmp2FC=_tmp2FB->f1;}}_LL1F9:
# 1299
 Cyc_Absyndump_dump_aggrdecl(_tmp2FC);Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL1FA: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2FD=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp2FD->tag != 7)goto _LL1FC;else{_tmp2FE=_tmp2FD->f1;}}_LL1FB:
# 1302
 Cyc_Absyndump_dump_datatypedecl(_tmp2FE);Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL1FC: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp2FF=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp2FF->tag != 8)goto _LL1FE;else{_tmp300=_tmp2FF->f1;}}_LL1FD:
# 1305
 Cyc_Absyndump_dump_enumdecl(_tmp300);Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL1FE: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp301=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp301->tag != 2)goto _LL200;else{_tmp302=_tmp301->f1;_tmp303=_tmp301->f3;}}_LL1FF:
# 1308
{const char*_tmp4E0;Cyc_Absyndump_dump(((_tmp4E0="let",_tag_dyneither(_tmp4E0,sizeof(char),4))));}
Cyc_Absyndump_dumppat(_tmp302);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp303);
Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL200: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp304=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp304->tag != 3)goto _LL202;else{_tmp305=_tmp304->f1;}}_LL201:
# 1315
{const char*_tmp4E1;Cyc_Absyndump_dump(((_tmp4E1="let ",_tag_dyneither(_tmp4E1,sizeof(char),5))));}
Cyc_Absyndump_dumpids(_tmp305);
Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL202: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp306=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp306->tag != 4)goto _LL204;else{_tmp307=_tmp306->f1;_tmp308=_tmp306->f2;_tmp309=_tmp306->f3;_tmp30A=_tmp306->f4;}}_LL203:
# 1320
{const char*_tmp4E2;Cyc_Absyndump_dump(((_tmp4E2="region",_tag_dyneither(_tmp4E2,sizeof(char),7))));}
{const char*_tmp4E3;Cyc_Absyndump_dump(((_tmp4E3="<",_tag_dyneither(_tmp4E3,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp307);{const char*_tmp4E4;Cyc_Absyndump_dump(((_tmp4E4="> ",_tag_dyneither(_tmp4E4,sizeof(char),3))));}Cyc_Absyndump_dumpqvar(_tmp308->name);
if(_tmp309){const char*_tmp4E5;Cyc_Absyndump_dump(((_tmp4E5="@resetable",_tag_dyneither(_tmp4E5,sizeof(char),11))));}
if(_tmp30A != 0){
{const char*_tmp4E6;Cyc_Absyndump_dump(((_tmp4E6=" = open(",_tag_dyneither(_tmp4E6,sizeof(char),9))));}
Cyc_Absyndump_dumpexp(_tmp30A);{
const char*_tmp4E7;Cyc_Absyndump_dump(((_tmp4E7=")",_tag_dyneither(_tmp4E7,sizeof(char),2))));};}
# 1328
Cyc_Absyndump_dump_semi();
goto _LL1F3;_LL204: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp30B=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp30B->tag != 5)goto _LL206;else{_tmp30C=_tmp30B->f1;_tmp30D=_tmp30B->f2;}}_LL205:
# 1331
{const char*_tmp4E8;Cyc_Absyndump_dump(((_tmp4E8="alias",_tag_dyneither(_tmp4E8,sizeof(char),6))));}
{const char*_tmp4E9;Cyc_Absyndump_dump(((_tmp4E9="<",_tag_dyneither(_tmp4E9,sizeof(char),2))));}Cyc_Absyndump_dumptvar(_tmp30C);{const char*_tmp4EA;Cyc_Absyndump_dump(((_tmp4EA=">",_tag_dyneither(_tmp4EA,sizeof(char),2))));}
Cyc_Absyndump_dumpvardecl(_tmp30D,d->loc);
goto _LL1F3;_LL206: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp30E=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp30E->tag != 9)goto _LL208;else{_tmp30F=_tmp30E->f1;}}_LL207:
# 1340
{const char*_tmp4EB;Cyc_Absyndump_dump(((_tmp4EB="typedef",_tag_dyneither(_tmp4EB,sizeof(char),8))));}{
void*t;
if(_tmp30F->defn == 0)
t=Cyc_Absyn_new_evar(_tmp30F->kind,0);else{
# 1345
t=(void*)_check_null(_tmp30F->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp30F->tq,t,Cyc_Absyndump_dumptypedefname,_tmp30F);
Cyc_Absyndump_dumpatts(_tmp30F->atts);
Cyc_Absyndump_dump_semi();
# 1350
goto _LL1F3;};_LL208: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp310=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp310->tag != 10)goto _LL20A;else{_tmp311=_tmp310->f1;_tmp312=_tmp310->f2;}}_LL209:
# 1352
{const char*_tmp4EC;Cyc_Absyndump_dump(((_tmp4EC="namespace ",_tag_dyneither(_tmp4EC,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp311);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp312 != 0;_tmp312=_tmp312->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp312->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F3;_LL20A: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp313=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp313->tag != 11)goto _LL20C;else{_tmp314=_tmp313->f1;_tmp315=_tmp313->f2;}}_LL20B:
# 1360
{const char*_tmp4ED;Cyc_Absyndump_dump(((_tmp4ED="using ",_tag_dyneither(_tmp4ED,sizeof(char),7))));}
Cyc_Absyndump_dumpqvar(_tmp314);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp315 != 0;_tmp315=_tmp315->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp315->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F3;_LL20C: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp316=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp316->tag != 12)goto _LL20E;else{_tmp317=_tmp316->f1;}}_LL20D:
# 1368
{const char*_tmp4EE;Cyc_Absyndump_dump(((_tmp4EE="extern \"C\" {",_tag_dyneither(_tmp4EE,sizeof(char),13))));}
for(0;_tmp317 != 0;_tmp317=_tmp317->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp317->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL1F3;_LL20E: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp318=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp318->tag != 13)goto _LL210;else{_tmp319=_tmp318->f1;_tmp31A=_tmp318->f2;}}_LL20F:
# 1374
{const char*_tmp4EF;Cyc_Absyndump_dump(((_tmp4EF="extern \"C include\" {",_tag_dyneither(_tmp4EF,sizeof(char),21))));}
for(0;_tmp319 != 0;_tmp319=_tmp319->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp319->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp31A != 0){
{const char*_tmp4F0;Cyc_Absyndump_dump(((_tmp4F0=" export {",_tag_dyneither(_tmp4F0,sizeof(char),10))));}
for(0;_tmp31A != 0;_tmp31A=_tmp31A->tl){
struct _tuple20 _stmttmp1C=*((struct _tuple20*)_tmp31A->hd);struct _tuple0*_tmp336;struct _tuple20 _tmp335=_stmttmp1C;_tmp336=_tmp335.f2;
Cyc_Absyndump_dumpqvar(_tmp336);
if(_tmp31A->tl != 0)Cyc_Absyndump_dump_char((int)',');}{
# 1385
const char*_tmp4F1;Cyc_Absyndump_dump(((_tmp4F1="}",_tag_dyneither(_tmp4F1,sizeof(char),2))));};}
# 1387
goto _LL1F3;_LL210: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp31B=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp31B->tag != 14)goto _LL212;}_LL211:
# 1389
{const char*_tmp4F2;Cyc_Absyndump_dump(((_tmp4F2=" __cyclone_port_on__; ",_tag_dyneither(_tmp4F2,sizeof(char),23))));}
goto _LL1F3;_LL212: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp31C=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp2F6;if(_tmp31C->tag != 15)goto _LL1F3;}_LL213:
# 1392
{const char*_tmp4F3;Cyc_Absyndump_dump(((_tmp4F3=" __cyclone_port_off__; ",_tag_dyneither(_tmp4F3,sizeof(char),24))));}
goto _LL1F3;_LL1F3:;};}
# 1397
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1404
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1409
void*_stmttmp1D=(void*)tms->hd;void*_tmp33A=_stmttmp1D;void*_tmp33C;union Cyc_Absyn_Constraint*_tmp33D;union Cyc_Absyn_Constraint*_tmp33E;union Cyc_Absyn_Constraint*_tmp33F;struct Cyc_Absyn_Tqual _tmp340;_LL220: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp33B=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp33A;if(_tmp33B->tag != 2)goto _LL222;else{_tmp33C=(_tmp33B->f1).rgn;_tmp33D=(_tmp33B->f1).nullable;_tmp33E=(_tmp33B->f1).bounds;_tmp33F=(_tmp33B->f1).zero_term;_tmp340=_tmp33B->f2;}}_LL221:
# 1413
{void*_stmttmp20=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp33E);void*_tmp341=_stmttmp20;struct Cyc_Absyn_Exp*_tmp344;_LL225: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp342=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp341;if(_tmp342->tag != 0)goto _LL227;}_LL226:
 Cyc_Absyndump_dump_char((int)'?');goto _LL224;_LL227: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp343=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp341;if(_tmp343->tag != 1)goto _LL224;else{_tmp344=_tmp343->f1;}}_LL228:
# 1416
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp33D)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp344);
goto _LL224;_LL224:;}
# 1420
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp33F)){const char*_tmp4F4;Cyc_Absyndump_dump(((_tmp4F4="@zeroterm",_tag_dyneither(_tmp4F4,sizeof(char),10))));}
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp33F)){const char*_tmp4F5;Cyc_Absyndump_dump(((_tmp4F5="@nozeroterm",_tag_dyneither(_tmp4F5,sizeof(char),12))));}
{void*_stmttmp21=Cyc_Tcutil_compress(_tmp33C);void*_tmp347=_stmttmp21;struct Cyc_Absyn_Tvar*_tmp34C;_LL22A: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp348=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp347;if(_tmp348->tag != 20)goto _LL22C;}_LL22B:
 if(!Cyc_Absyndump_qvar_to_Cids){const char*_tmp4F6;Cyc_Absyndump_dump(((_tmp4F6="`H",_tag_dyneither(_tmp4F6,sizeof(char),3))));}goto _LL229;_LL22C: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp349=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp347;if(_tmp349->tag != 21)goto _LL22E;}_LL22D:
{const char*_tmp4F7;Cyc_Absyndump_dump(((_tmp4F7="`U",_tag_dyneither(_tmp4F7,sizeof(char),3))));}goto _LL229;_LL22E: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp34A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp347;if(_tmp34A->tag != 22)goto _LL230;}_LL22F:
{const char*_tmp4F8;Cyc_Absyndump_dump(((_tmp4F8="`RC",_tag_dyneither(_tmp4F8,sizeof(char),4))));}goto _LL229;_LL230: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp347;if(_tmp34B->tag != 2)goto _LL232;else{_tmp34C=_tmp34B->f1;}}_LL231:
 Cyc_Absyndump_dumptvar(_tmp34C);goto _LL229;_LL232: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp34D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp347;if(_tmp34D->tag != 1)goto _LL234;else{if((void*)_tmp34D->f2 != 0)goto _LL234;}}_LL233:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp33C));goto _LL229;_LL234: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp34E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp347;if(_tmp34E->tag != 17)goto _LL236;}_LL235:
{const char*_tmp4F9;Cyc_Absyndump_dump(((_tmp4F9="@region(",_tag_dyneither(_tmp4F9,sizeof(char),9))));}Cyc_Absyndump_dumptyp(_tmp33C);{const char*_tmp4FA;Cyc_Absyndump_dump(((_tmp4FA=")",_tag_dyneither(_tmp4FA,sizeof(char),2))));}goto _LL229;_LL236:;_LL237: {
const char*_tmp4FD;void*_tmp4FC;(_tmp4FC=0,Cyc_Tcutil_impos(((_tmp4FD="dumptms: bad rgn type in Pointer_mod",_tag_dyneither(_tmp4FD,sizeof(char),37))),_tag_dyneither(_tmp4FC,sizeof(void*),0)));}_LL229:;}
# 1431
Cyc_Absyndump_dumptq(_tmp340);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;_LL222:;_LL223: {
# 1436
int next_is_pointer=0;
if(tms->tl != 0){
void*_stmttmp1E=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp356=_stmttmp1E;_LL239: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp357=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp356;if(_tmp357->tag != 2)goto _LL23B;}_LL23A:
 next_is_pointer=1;goto _LL238;_LL23B:;_LL23C:
 goto _LL238;_LL238:;}
# 1442
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_stmttmp1F=(void*)tms->hd;void*_tmp358=_stmttmp1F;union Cyc_Absyn_Constraint*_tmp35A;struct Cyc_Absyn_Exp*_tmp35C;union Cyc_Absyn_Constraint*_tmp35D;struct Cyc_List_List*_tmp360;int _tmp361;struct Cyc_Absyn_VarargInfo*_tmp362;void*_tmp363;struct Cyc_List_List*_tmp364;struct Cyc_List_List*_tmp367;unsigned int _tmp368;struct Cyc_List_List*_tmp36A;unsigned int _tmp36B;int _tmp36C;struct Cyc_List_List*_tmp36E;_LL23E: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp359=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp359->tag != 0)goto _LL240;else{_tmp35A=_tmp359->f1;}}_LL23F:
# 1449
{const char*_tmp4FE;Cyc_Absyndump_dump(((_tmp4FE="[]",_tag_dyneither(_tmp4FE,sizeof(char),3))));}
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp35A)){const char*_tmp4FF;Cyc_Absyndump_dump(((_tmp4FF="@zeroterm",_tag_dyneither(_tmp4FF,sizeof(char),10))));}
goto _LL23D;_LL240: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp35B=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp35B->tag != 1)goto _LL242;else{_tmp35C=_tmp35B->f1;_tmp35D=_tmp35B->f2;}}_LL241:
# 1453
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp35C);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp35D)){const char*_tmp500;Cyc_Absyndump_dump(((_tmp500="@zeroterm",_tag_dyneither(_tmp500,sizeof(char),10))));}
goto _LL23D;_LL242: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp35E=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp35E->tag != 3)goto _LL244;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp35F=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmp35E->f1);if(_tmp35F->tag != 1)goto _LL244;else{_tmp360=_tmp35F->f1;_tmp361=_tmp35F->f2;_tmp362=_tmp35F->f3;_tmp363=(void*)_tmp35F->f4;_tmp364=_tmp35F->f5;}}}_LL243:
# 1457
 Cyc_Absyndump_dumpfunargs(_tmp360,_tmp361,_tmp362,_tmp363,_tmp364);goto _LL23D;_LL244: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp365=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp365->tag != 3)goto _LL246;else{struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp366=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((void*)_tmp365->f1);if(_tmp366->tag != 0)goto _LL246;else{_tmp367=_tmp366->f1;_tmp368=_tmp366->f2;}}}_LL245:
# 1459
{const char*_tmp503;const char*_tmp502;const char*_tmp501;((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp367,((_tmp501="(",_tag_dyneither(_tmp501,sizeof(char),2))),((_tmp502=")",_tag_dyneither(_tmp502,sizeof(char),2))),((_tmp503=",",_tag_dyneither(_tmp503,sizeof(char),2))));}goto _LL23D;_LL246: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp369=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp369->tag != 4)goto _LL248;else{_tmp36A=_tmp369->f1;_tmp36B=_tmp369->f2;_tmp36C=_tmp369->f3;}}_LL247:
# 1461
 if(_tmp36C)Cyc_Absyndump_dumpkindedtvars(_tmp36A);else{Cyc_Absyndump_dumptvars(_tmp36A);}goto _LL23D;_LL248: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp36D=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp36D->tag != 5)goto _LL24A;else{_tmp36E=_tmp36D->f2;}}_LL249:
 Cyc_Absyndump_dumpatts(_tmp36E);goto _LL23D;_LL24A: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp36F=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp358;if(_tmp36F->tag != 2)goto _LL23D;}_LL24B: {
const char*_tmp506;void*_tmp505;(_tmp505=0,Cyc_Tcutil_impos(((_tmp506="dumptms",_tag_dyneither(_tmp506,sizeof(char),8))),_tag_dyneither(_tmp505,sizeof(void*),0)));}_LL23D:;}
# 1465
return;}_LL21F:;};}
# 1469
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp378=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp379=_new_region("temp");struct _RegionHandle*temp=& _tmp379;_push_region(temp);
{struct _tuple11 _stmttmp22=Cyc_Absynpp_to_tms(temp,tq,t);struct Cyc_Absyn_Tqual _tmp37B;void*_tmp37C;struct Cyc_List_List*_tmp37D;struct _tuple11 _tmp37A=_stmttmp22;_tmp37B=_tmp37A.f1;_tmp37C=_tmp37A.f2;_tmp37D=_tmp37A.f3;
Cyc_Absyndump_dumptq(_tmp37B);
Cyc_Absyndump_dumpntyp(_tmp37C);
Cyc_Absyndump_dumptms(_tmp378,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp37D),f,a);}
# 1472
;_pop_region(temp);}
# 1479
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1481
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}{
const char*_tmp509;void*_tmp508;(_tmp508=0,Cyc_fprintf(f,((_tmp509="\n",_tag_dyneither(_tmp509,sizeof(char),2))),_tag_dyneither(_tmp508,sizeof(void*),0)));};}
# 1487
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_stmttmp23=d->r;void*_tmp380=_stmttmp23;struct Cyc_Absyn_Vardecl*_tmp382;struct Cyc_Absyn_Fndecl*_tmp384;struct Cyc_Absyn_Aggrdecl*_tmp386;struct Cyc_Absyn_Datatypedecl*_tmp388;struct Cyc_Absyn_Enumdecl*_tmp38A;struct Cyc_Absyn_Typedefdecl*_tmp38C;struct _dyneither_ptr*_tmp38E;struct Cyc_List_List*_tmp38F;struct Cyc_List_List*_tmp391;struct Cyc_List_List*_tmp393;struct Cyc_List_List*_tmp395;struct Cyc_List_List*_tmp396;_LL24D: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp381=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp381->tag != 0)goto _LL24F;else{_tmp382=_tmp381->f1;}}_LL24E:
# 1490
 if(_tmp382->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp382->initializer;
_tmp382->initializer=0;
if(_tmp382->sc == Cyc_Absyn_Public){
const char*_tmp50A;Cyc_Absyndump_dump(((_tmp50A="extern ",_tag_dyneither(_tmp50A,sizeof(char),8))));}
Cyc_Absyndump_dumpvardecl(_tmp382,d->loc);
{const char*_tmp50B;Cyc_Absyndump_dump(((_tmp50B="\n",_tag_dyneither(_tmp50B,sizeof(char),2))));}
_tmp382->initializer=init;
goto _LL24C;};_LL24F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp383=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp383->tag != 1)goto _LL251;else{_tmp384=_tmp383->f1;}}_LL250:
# 1500
 if(_tmp384->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp384->sc);{
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp511;struct Cyc_Absyn_FnInfo _tmp510;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp50F;void*t=(void*)((_tmp50F=_cycalloc(sizeof(*_tmp50F)),((_tmp50F[0]=((_tmp511.tag=9,((_tmp511.f1=((_tmp510.tvars=_tmp384->tvs,((_tmp510.effect=_tmp384->effect,((_tmp510.ret_tqual=_tmp384->ret_tqual,((_tmp510.ret_typ=_tmp384->ret_type,((_tmp510.args=
# 1504
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp384->args),((_tmp510.c_varargs=_tmp384->c_varargs,((_tmp510.cyc_varargs=_tmp384->cyc_varargs,((_tmp510.rgn_po=_tmp384->rgn_po,((_tmp510.attributes=0,_tmp510)))))))))))))))))),_tmp511)))),_tmp50F))));
# 1507
((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(Cyc_Absyn_empty_tqual(0),t,Cyc_Absyndump_dump_atts_qvar,_tmp384);
{const char*_tmp512;Cyc_Absyndump_dump(((_tmp512=";\n",_tag_dyneither(_tmp512,sizeof(char),3))));}
goto _LL24C;};_LL251: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp385=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp385->tag != 6)goto _LL253;else{_tmp386=_tmp385->f1;}}_LL252:
# 1511
 if(_tmp386->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp386->impl;
if(_tmp386->sc == Cyc_Absyn_Abstract)_tmp386->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp386);
_tmp386->impl=impl;
{const char*_tmp513;Cyc_Absyndump_dump(((_tmp513=";\n",_tag_dyneither(_tmp513,sizeof(char),3))));}
goto _LL24C;};_LL253: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp387=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp387->tag != 7)goto _LL255;else{_tmp388=_tmp387->f1;}}_LL254:
# 1519
 Cyc_Absyndump_dump_datatypedecl(_tmp388);
{const char*_tmp514;Cyc_Absyndump_dump(((_tmp514=";\n",_tag_dyneither(_tmp514,sizeof(char),3))));}
goto _LL24C;_LL255: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp389=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp389->tag != 8)goto _LL257;else{_tmp38A=_tmp389->f1;}}_LL256:
# 1523
 Cyc_Absyndump_dump_enumdecl(_tmp38A);
{const char*_tmp515;Cyc_Absyndump_dump(((_tmp515=";\n",_tag_dyneither(_tmp515,sizeof(char),3))));}
goto _LL24C;_LL257: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp38B=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp38B->tag != 9)goto _LL259;else{_tmp38C=_tmp38B->f1;}}_LL258:
# 1527
 if(_tmp38C->defn == 0){
Cyc_Absyndump_dumpdecl(d);{
const char*_tmp516;Cyc_Absyndump_dump(((_tmp516="\n",_tag_dyneither(_tmp516,sizeof(char),2))));};}
# 1531
goto _LL24C;_LL259: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp38D=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp38D->tag != 10)goto _LL25B;else{_tmp38E=_tmp38D->f1;_tmp38F=_tmp38D->f2;}}_LL25A:
# 1533
{const char*_tmp517;Cyc_Absyndump_dump(((_tmp517="namespace ",_tag_dyneither(_tmp517,sizeof(char),11))));}
Cyc_Absyndump_dump_str(_tmp38E);
{const char*_tmp518;Cyc_Absyndump_dump(((_tmp518="{\n",_tag_dyneither(_tmp518,sizeof(char),3))));}
for(0;_tmp38F != 0;_tmp38F=_tmp38F->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp38F->hd);}
{const char*_tmp519;Cyc_Absyndump_dump(((_tmp519="}\n",_tag_dyneither(_tmp519,sizeof(char),3))));}
goto _LL24C;_LL25B: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp390=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp390->tag != 11)goto _LL25D;else{_tmp391=_tmp390->f2;}}_LL25C:
# 1541
 for(0;_tmp391 != 0;_tmp391=_tmp391->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp391->hd);}
goto _LL24C;_LL25D: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp392=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp392->tag != 12)goto _LL25F;else{_tmp393=_tmp392->f1;}}_LL25E:
# 1545
{const char*_tmp51A;Cyc_Absyndump_dump(((_tmp51A="extern \"C\" {",_tag_dyneither(_tmp51A,sizeof(char),13))));}
for(0;_tmp393 != 0;_tmp393=_tmp393->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp393->hd);}
{const char*_tmp51B;Cyc_Absyndump_dump(((_tmp51B="}\n",_tag_dyneither(_tmp51B,sizeof(char),3))));}
goto _LL24C;_LL25F: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp394=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp380;if(_tmp394->tag != 13)goto _LL261;else{_tmp395=_tmp394->f1;_tmp396=_tmp394->f2;}}_LL260:
# 1566 "absyndump.cyc"
 goto _LL24C;_LL261:;_LL262: {
const char*_tmp51E;void*_tmp51D;(_tmp51D=0,((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(((_tmp51E="bad top-level declaration",_tag_dyneither(_tmp51E,sizeof(char),26))),_tag_dyneither(_tmp51D,sizeof(void*),0)));}_LL24C:;}
# 1571
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
