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
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 89 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 93
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 107 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 162
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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 121
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 166
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 183
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 246
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 371
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 375
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;
# 41
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 50
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f);
# 53
struct _dyneither_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w);
# 67 "pp.h"
struct Cyc_PP_Doc*Cyc_PP_nil_doc();
# 69
struct Cyc_PP_Doc*Cyc_PP_blank_doc();
# 72
struct Cyc_PP_Doc*Cyc_PP_line_doc();
# 78
struct Cyc_PP_Doc*Cyc_PP_text(struct _dyneither_ptr s);
# 80
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _dyneither_ptr*p);
# 83
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _dyneither_ptr s,int w);
# 91
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d);
# 94
struct Cyc_PP_Doc*Cyc_PP_cat(struct _dyneither_ptr);
# 108
struct Cyc_PP_Doc*Cyc_PP_seq(struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 112
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 117
struct Cyc_PP_Doc*Cyc_PP_seql(struct _dyneither_ptr sep,struct Cyc_List_List*l0);
# 120
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 123
struct Cyc_PP_Doc*Cyc_PP_group(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);
# 129
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _dyneither_ptr start,struct _dyneither_ptr stop,struct _dyneither_ptr sep,struct Cyc_List_List*l);struct Cyc_Lineno_Pos{struct _dyneither_ptr logical_file;struct _dyneither_ptr line;int line_no;int col;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 508
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 534
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 541
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 559
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 720 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);struct Cyc_Buffer_t;
# 46 "buffer.h"
typedef struct Cyc_Buffer_t*Cyc_Buffer_T;
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 301 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 303
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 56
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 58
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d);
# 64
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*);
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p);
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);
# 77
int Cyc_Absynpp_is_anon_aggrtype(void*t);
extern struct _dyneither_ptr Cyc_Absynpp_cyc_string;
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 87
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);
# 27 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};
# 33
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 43 "absynpp.cyc"
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp);
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs);
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope);
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*,int expstmt);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*);
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e);
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es);
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*);
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst);
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop);
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es);struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de);
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_Core_Opt*dopt);
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po);
# 63
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields);
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs);
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd);
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ad);
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*ad);
# 70
static int Cyc_Absynpp_expand_typedefs;
# 74
static int Cyc_Absynpp_qvar_to_Cids;static char _tmp0[4]="Cyc";
# 76
struct _dyneither_ptr Cyc_Absynpp_cyc_string={_tmp0,_tmp0,_tmp0 + 4};
struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr=& Cyc_Absynpp_cyc_string;
# 79
static int Cyc_Absynpp_add_cyc_prefix;
# 83
static int Cyc_Absynpp_to_VC;
# 86
static int Cyc_Absynpp_decls_first;
# 90
static int Cyc_Absynpp_rewrite_temp_tvars;
# 93
static int Cyc_Absynpp_print_all_tvars;
# 96
static int Cyc_Absynpp_print_all_kinds;
# 99
static int Cyc_Absynpp_print_all_effects;
# 102
static int Cyc_Absynpp_print_using_stmts;
# 107
static int Cyc_Absynpp_print_externC_stmts;
# 111
static int Cyc_Absynpp_print_full_evars;
# 114
static int Cyc_Absynpp_generate_line_directives;
# 117
static int Cyc_Absynpp_use_curr_namespace;
# 120
static int Cyc_Absynpp_print_zeroterm;
# 123
static struct Cyc_List_List*Cyc_Absynpp_curr_namespace=0;
# 126
int Cyc_Absynpp_print_for_cycdoc=0;struct Cyc_Absynpp_Params;
# 147
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absynpp_expand_typedefs=fs->expand_typedefs;
Cyc_Absynpp_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absynpp_to_VC=fs->to_VC;
Cyc_Absynpp_decls_first=fs->decls_first;
Cyc_Absynpp_rewrite_temp_tvars=fs->rewrite_temp_tvars;
Cyc_Absynpp_print_all_tvars=fs->print_all_tvars;
Cyc_Absynpp_print_all_kinds=fs->print_all_kinds;
Cyc_Absynpp_print_all_effects=fs->print_all_effects;
Cyc_Absynpp_print_using_stmts=fs->print_using_stmts;
Cyc_Absynpp_print_externC_stmts=fs->print_externC_stmts;
Cyc_Absynpp_print_full_evars=fs->print_full_evars;
Cyc_Absynpp_print_zeroterm=fs->print_zeroterm;
Cyc_Absynpp_generate_line_directives=fs->generate_line_directives;
Cyc_Absynpp_use_curr_namespace=fs->use_curr_namespace;
Cyc_Absynpp_curr_namespace=fs->curr_namespace;}
# 166
struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 186
struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r={1,0,1,0,0,1,0,0,0,1,1,0,1,0,1,0};
# 206
struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r={1,1,1,0,1,0,0,0,0,0,0,0,0,1,0,0};
# 226
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,0,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
struct Cyc_List_List*_tmp5C8;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5C8=_cycalloc(sizeof(*_tmp5C8)),((_tmp5C8->hd=v,((_tmp5C8->tl=0,_tmp5C8)))))));}
# 251
static void Cyc_Absynpp_suppr_last(struct Cyc_List_List**l){
if(((struct Cyc_List_List*)_check_null(*l))->tl == 0)
*l=0;else{
# 255
Cyc_Absynpp_suppr_last(&((struct Cyc_List_List*)_check_null(*l))->tl);}}
# 259
static void Cyc_Absynpp_curr_namespace_drop(){
((void(*)(struct Cyc_List_List**l))Cyc_Absynpp_suppr_last)(& Cyc_Absynpp_curr_namespace);}
# 263
struct _dyneither_ptr Cyc_Absynpp_char_escape(char c){
switch(c){case '\a': _LL0: {
const char*_tmp5C9;return(_tmp5C9="\\a",_tag_dyneither(_tmp5C9,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5CA;return(_tmp5CA="\\b",_tag_dyneither(_tmp5CA,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5CB;return(_tmp5CB="\\f",_tag_dyneither(_tmp5CB,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5CC;return(_tmp5CC="\\n",_tag_dyneither(_tmp5CC,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5CD;return(_tmp5CD="\\r",_tag_dyneither(_tmp5CD,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5CE;return(_tmp5CE="\\t",_tag_dyneither(_tmp5CE,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5CF;return(_tmp5CF="\\v",_tag_dyneither(_tmp5CF,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5D0;return(_tmp5D0="\\\\",_tag_dyneither(_tmp5D0,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5D1;return(_tmp5D1="\"",_tag_dyneither(_tmp5D1,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5D2;return(_tmp5D2="\\'",_tag_dyneither(_tmp5D2,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5D5;char _tmp5D4;struct _dyneither_ptr _tmp5D3;(_tmp5D3=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5D4=*((char*)_check_dyneither_subscript(_tmp5D3,sizeof(char),0)),((_tmp5D5=c,((_get_dyneither_size(_tmp5D3,sizeof(char))== 1  && (_tmp5D4 == '\000'  && _tmp5D5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D3.curr)=_tmp5D5)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp5D8;char _tmp5D7;struct _dyneither_ptr _tmp5D6;(_tmp5D6=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5D7=*((char*)_check_dyneither_subscript(_tmp5D6,sizeof(char),0)),((_tmp5D8='\\',((_get_dyneither_size(_tmp5D6,sizeof(char))== 1  && (_tmp5D7 == '\000'  && _tmp5D8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D6.curr)=_tmp5D8)))))));}
{char _tmp5DB;char _tmp5DA;struct _dyneither_ptr _tmp5D9;(_tmp5D9=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DA=*((char*)_check_dyneither_subscript(_tmp5D9,sizeof(char),0)),((_tmp5DB=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5D9,sizeof(char))== 1  && (_tmp5DA == '\000'  && _tmp5DB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D9.curr)=_tmp5DB)))))));}
{char _tmp5DE;char _tmp5DD;struct _dyneither_ptr _tmp5DC;(_tmp5DC=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DD=*((char*)_check_dyneither_subscript(_tmp5DC,sizeof(char),0)),((_tmp5DE=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5DC,sizeof(char))== 1  && (_tmp5DD == '\000'  && _tmp5DE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DC.curr)=_tmp5DE)))))));}
{char _tmp5E1;char _tmp5E0;struct _dyneither_ptr _tmp5DF;(_tmp5DF=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E0=*((char*)_check_dyneither_subscript(_tmp5DF,sizeof(char),0)),((_tmp5E1=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp5DF,sizeof(char))== 1  && (_tmp5E0 == '\000'  && _tmp5E1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DF.curr)=_tmp5E1)))))));}
return(struct _dyneither_ptr)_tmp10;}}}
# 292
static int Cyc_Absynpp_special(struct _dyneither_ptr s){
int sz=(int)(_get_dyneither_size(s,sizeof(char))- 1);
{int i=0;for(0;i < sz;++ i){
char c=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(((c <= ' '  || c >= '~') || c == '"') || c == '\\')
return 1;}}
# 299
return 0;}
# 302
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr s){
if(!Cyc_Absynpp_special(s))return s;{
# 305
int n=(int)(_get_dyneither_size(s,sizeof(char))- 1);
# 307
if(n > 0  && *((const char*)_check_dyneither_subscript(s,sizeof(char),n))== '\000')-- n;{
# 309
int len=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1E;_LLD: if(_tmp1D != '\a')goto _LLF;_LLE:
 goto _LL10;_LLF: if(_tmp1D != '\b')goto _LL11;_LL10:
 goto _LL12;_LL11: if(_tmp1D != '\f')goto _LL13;_LL12:
 goto _LL14;_LL13: if(_tmp1D != '\n')goto _LL15;_LL14:
 goto _LL16;_LL15: if(_tmp1D != '\r')goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp1D != '\t')goto _LL19;_LL18:
 goto _LL1A;_LL19: if(_tmp1D != '\v')goto _LL1B;_LL1A:
 goto _LL1C;_LL1B: if(_tmp1D != '\\')goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if(_tmp1D != '"')goto _LL1F;_LL1E:
 len +=2;goto _LLC;_LL1F: _tmp1E=_tmp1D;_LL20:
# 322
 if(_tmp1E >= ' '  && _tmp1E <= '~')++ len;else{
len +=4;}
goto _LLC;_LLC:;}}{
# 327
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp1F=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp20;_LL22: if(_tmp1F != '\a')goto _LL24;_LL23:
{char _tmp5E4;char _tmp5E3;struct _dyneither_ptr _tmp5E2;(_tmp5E2=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E3=*((char*)_check_dyneither_subscript(_tmp5E2,sizeof(char),0)),((_tmp5E4='\\',((_get_dyneither_size(_tmp5E2,sizeof(char))== 1  && (_tmp5E3 == '\000'  && _tmp5E4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E2.curr)=_tmp5E4)))))));}{char _tmp5E7;char _tmp5E6;struct _dyneither_ptr _tmp5E5;(_tmp5E5=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E6=*((char*)_check_dyneither_subscript(_tmp5E5,sizeof(char),0)),((_tmp5E7='a',((_get_dyneither_size(_tmp5E5,sizeof(char))== 1  && (_tmp5E6 == '\000'  && _tmp5E7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E5.curr)=_tmp5E7)))))));}goto _LL21;_LL24: if(_tmp1F != '\b')goto _LL26;_LL25:
{char _tmp5EA;char _tmp5E9;struct _dyneither_ptr _tmp5E8;(_tmp5E8=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E9=*((char*)_check_dyneither_subscript(_tmp5E8,sizeof(char),0)),((_tmp5EA='\\',((_get_dyneither_size(_tmp5E8,sizeof(char))== 1  && (_tmp5E9 == '\000'  && _tmp5EA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E8.curr)=_tmp5EA)))))));}{char _tmp5ED;char _tmp5EC;struct _dyneither_ptr _tmp5EB;(_tmp5EB=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EC=*((char*)_check_dyneither_subscript(_tmp5EB,sizeof(char),0)),((_tmp5ED='b',((_get_dyneither_size(_tmp5EB,sizeof(char))== 1  && (_tmp5EC == '\000'  && _tmp5ED != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EB.curr)=_tmp5ED)))))));}goto _LL21;_LL26: if(_tmp1F != '\f')goto _LL28;_LL27:
{char _tmp5F0;char _tmp5EF;struct _dyneither_ptr _tmp5EE;(_tmp5EE=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EF=*((char*)_check_dyneither_subscript(_tmp5EE,sizeof(char),0)),((_tmp5F0='\\',((_get_dyneither_size(_tmp5EE,sizeof(char))== 1  && (_tmp5EF == '\000'  && _tmp5F0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EE.curr)=_tmp5F0)))))));}{char _tmp5F3;char _tmp5F2;struct _dyneither_ptr _tmp5F1;(_tmp5F1=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F2=*((char*)_check_dyneither_subscript(_tmp5F1,sizeof(char),0)),((_tmp5F3='f',((_get_dyneither_size(_tmp5F1,sizeof(char))== 1  && (_tmp5F2 == '\000'  && _tmp5F3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F1.curr)=_tmp5F3)))))));}goto _LL21;_LL28: if(_tmp1F != '\n')goto _LL2A;_LL29:
{char _tmp5F6;char _tmp5F5;struct _dyneither_ptr _tmp5F4;(_tmp5F4=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F5=*((char*)_check_dyneither_subscript(_tmp5F4,sizeof(char),0)),((_tmp5F6='\\',((_get_dyneither_size(_tmp5F4,sizeof(char))== 1  && (_tmp5F5 == '\000'  && _tmp5F6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F4.curr)=_tmp5F6)))))));}{char _tmp5F9;char _tmp5F8;struct _dyneither_ptr _tmp5F7;(_tmp5F7=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F8=*((char*)_check_dyneither_subscript(_tmp5F7,sizeof(char),0)),((_tmp5F9='n',((_get_dyneither_size(_tmp5F7,sizeof(char))== 1  && (_tmp5F8 == '\000'  && _tmp5F9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F7.curr)=_tmp5F9)))))));}goto _LL21;_LL2A: if(_tmp1F != '\r')goto _LL2C;_LL2B:
{char _tmp5FC;char _tmp5FB;struct _dyneither_ptr _tmp5FA;(_tmp5FA=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FB=*((char*)_check_dyneither_subscript(_tmp5FA,sizeof(char),0)),((_tmp5FC='\\',((_get_dyneither_size(_tmp5FA,sizeof(char))== 1  && (_tmp5FB == '\000'  && _tmp5FC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FA.curr)=_tmp5FC)))))));}{char _tmp5FF;char _tmp5FE;struct _dyneither_ptr _tmp5FD;(_tmp5FD=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FE=*((char*)_check_dyneither_subscript(_tmp5FD,sizeof(char),0)),((_tmp5FF='r',((_get_dyneither_size(_tmp5FD,sizeof(char))== 1  && (_tmp5FE == '\000'  && _tmp5FF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FD.curr)=_tmp5FF)))))));}goto _LL21;_LL2C: if(_tmp1F != '\t')goto _LL2E;_LL2D:
{char _tmp602;char _tmp601;struct _dyneither_ptr _tmp600;(_tmp600=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp601=*((char*)_check_dyneither_subscript(_tmp600,sizeof(char),0)),((_tmp602='\\',((_get_dyneither_size(_tmp600,sizeof(char))== 1  && (_tmp601 == '\000'  && _tmp602 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp600.curr)=_tmp602)))))));}{char _tmp605;char _tmp604;struct _dyneither_ptr _tmp603;(_tmp603=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp604=*((char*)_check_dyneither_subscript(_tmp603,sizeof(char),0)),((_tmp605='t',((_get_dyneither_size(_tmp603,sizeof(char))== 1  && (_tmp604 == '\000'  && _tmp605 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp603.curr)=_tmp605)))))));}goto _LL21;_LL2E: if(_tmp1F != '\v')goto _LL30;_LL2F:
{char _tmp608;char _tmp607;struct _dyneither_ptr _tmp606;(_tmp606=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp607=*((char*)_check_dyneither_subscript(_tmp606,sizeof(char),0)),((_tmp608='\\',((_get_dyneither_size(_tmp606,sizeof(char))== 1  && (_tmp607 == '\000'  && _tmp608 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp606.curr)=_tmp608)))))));}{char _tmp60B;char _tmp60A;struct _dyneither_ptr _tmp609;(_tmp609=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60A=*((char*)_check_dyneither_subscript(_tmp609,sizeof(char),0)),((_tmp60B='v',((_get_dyneither_size(_tmp609,sizeof(char))== 1  && (_tmp60A == '\000'  && _tmp60B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp609.curr)=_tmp60B)))))));}goto _LL21;_LL30: if(_tmp1F != '\\')goto _LL32;_LL31:
{char _tmp60E;char _tmp60D;struct _dyneither_ptr _tmp60C;(_tmp60C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60D=*((char*)_check_dyneither_subscript(_tmp60C,sizeof(char),0)),((_tmp60E='\\',((_get_dyneither_size(_tmp60C,sizeof(char))== 1  && (_tmp60D == '\000'  && _tmp60E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60C.curr)=_tmp60E)))))));}{char _tmp611;char _tmp610;struct _dyneither_ptr _tmp60F;(_tmp60F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp610=*((char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0)),((_tmp611='\\',((_get_dyneither_size(_tmp60F,sizeof(char))== 1  && (_tmp610 == '\000'  && _tmp611 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60F.curr)=_tmp611)))))));}goto _LL21;_LL32: if(_tmp1F != '"')goto _LL34;_LL33:
{char _tmp614;char _tmp613;struct _dyneither_ptr _tmp612;(_tmp612=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp613=*((char*)_check_dyneither_subscript(_tmp612,sizeof(char),0)),((_tmp614='\\',((_get_dyneither_size(_tmp612,sizeof(char))== 1  && (_tmp613 == '\000'  && _tmp614 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp612.curr)=_tmp614)))))));}{char _tmp617;char _tmp616;struct _dyneither_ptr _tmp615;(_tmp615=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp616=*((char*)_check_dyneither_subscript(_tmp615,sizeof(char),0)),((_tmp617='"',((_get_dyneither_size(_tmp615,sizeof(char))== 1  && (_tmp616 == '\000'  && _tmp617 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp615.curr)=_tmp617)))))));}goto _LL21;_LL34: _tmp20=_tmp1F;_LL35:
# 341
 if(_tmp20 >= ' '  && _tmp20 <= '~'){char _tmp61A;char _tmp619;struct _dyneither_ptr _tmp618;(_tmp618=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp619=*((char*)_check_dyneither_subscript(_tmp618,sizeof(char),0)),((_tmp61A=_tmp20,((_get_dyneither_size(_tmp618,sizeof(char))== 1  && (_tmp619 == '\000'  && _tmp61A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp618.curr)=_tmp61A)))))));}else{
# 343
{char _tmp61D;char _tmp61C;struct _dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61C=*((char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((_tmp61D='\\',((_get_dyneither_size(_tmp61B,sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61B.curr)=_tmp61D)))))));}
{char _tmp620;char _tmp61F;struct _dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((_tmp620=(char)('0' + (_tmp20 >> 6 & 7)),((_get_dyneither_size(_tmp61E,sizeof(char))== 1  && (_tmp61F == '\000'  && _tmp620 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61E.curr)=_tmp620)))))));}
{char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623=(char)('0' + (_tmp20 >> 3 & 7)),((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}{
char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626=(char)('0' + (_tmp20 & 7)),((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));};}
# 348
goto _LL21;_LL21:;}}
# 350
return(struct _dyneither_ptr)t;};};};}static char _tmp66[9]="restrict";
# 353
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp66,_tmp66,_tmp66 + 9};static char _tmp67[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp67,_tmp67,_tmp67 + 9};static char _tmp68[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp68,_tmp68,_tmp68 + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 360
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 363
if(tq.q_restrict){struct Cyc_List_List*_tmp627;l=((_tmp627=_cycalloc(sizeof(*_tmp627)),((_tmp627->hd=Cyc_Absynpp_restrict_sp,((_tmp627->tl=l,_tmp627))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp628;l=((_tmp628=_cycalloc(sizeof(*_tmp628)),((_tmp628->hd=Cyc_Absynpp_volatile_sp,((_tmp628->tl=l,_tmp628))))));}
if(tq.print_const){struct Cyc_List_List*_tmp629;l=((_tmp629=_cycalloc(sizeof(*_tmp629)),((_tmp629->hd=Cyc_Absynpp_const_sp,((_tmp629->tl=l,_tmp629))))));}{
const char*_tmp62C;const char*_tmp62B;const char*_tmp62A;return Cyc_PP_egroup(((_tmp62A="",_tag_dyneither(_tmp62A,sizeof(char),1))),((_tmp62B=" ",_tag_dyneither(_tmp62B,sizeof(char),2))),((_tmp62C=" ",_tag_dyneither(_tmp62C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=_tmp6F->kind;_tmp71=_tmp6F->aliasqual;
switch(_tmp70){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp62D;return(_tmp62D="A",_tag_dyneither(_tmp62D,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp62E;return(_tmp62E="UA",_tag_dyneither(_tmp62E,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp62F;return(_tmp62F="TA",_tag_dyneither(_tmp62F,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp630;return(_tmp630="M",_tag_dyneither(_tmp630,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp631;return(_tmp631="UM",_tag_dyneither(_tmp631,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp632;return(_tmp632="TM",_tag_dyneither(_tmp632,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp633;return(_tmp633="B",_tag_dyneither(_tmp633,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp634;return(_tmp634="UB",_tag_dyneither(_tmp634,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp635;return(_tmp635="TB",_tag_dyneither(_tmp635,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp636;return(_tmp636="R",_tag_dyneither(_tmp636,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp637;return(_tmp637="UR",_tag_dyneither(_tmp637,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp638;return(_tmp638="TR",_tag_dyneither(_tmp638,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp639;return(_tmp639="E",_tag_dyneither(_tmp639,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp63A;return(_tmp63A="I",_tag_dyneither(_tmp63A,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp80=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp82;struct Cyc_Absyn_Kind*_tmp85;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp81->tag != 0)goto _LL50;else{_tmp82=_tmp81->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp82);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp83->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp63B;return(_tmp63B="{?}",_tag_dyneither(_tmp63B,sizeof(char),4));}else{
const char*_tmp63C;return(_tmp63C="?",_tag_dyneither(_tmp63C,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp84->tag != 2)goto _LL4D;else{_tmp85=_tmp84->f2;}}_LL53: {
const char*_tmp640;void*_tmp63F[1];struct Cyc_String_pa_PrintArg_struct _tmp63E;return(struct _dyneither_ptr)((_tmp63E.tag=0,((_tmp63E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp85)),((_tmp63F[0]=& _tmp63E,Cyc_aprintf(((_tmp640="<=%s",_tag_dyneither(_tmp640,sizeof(char),5))),_tag_dyneither(_tmp63F,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8B=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp90;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL57;else{_tmp8D=_tmp8C->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8D));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8E->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp641;return Cyc_PP_text_width(((_tmp641="{?}",_tag_dyneither(_tmp641,sizeof(char),4))),1);}else{
const char*_tmp642;return Cyc_PP_text(((_tmp642="?",_tag_dyneither(_tmp642,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8F->tag != 2)goto _LL54;else{_tmp90=_tmp8F->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp90));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp645;const char*_tmp644;const char*_tmp643;return Cyc_PP_egroup(((_tmp643="<",_tag_dyneither(_tmp643,sizeof(char),2))),((_tmp644=">",_tag_dyneither(_tmp644,sizeof(char),2))),((_tmp645=",",_tag_dyneither(_tmp645,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp96=*tv->name;
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '#'){
const char*_tmp648;struct Cyc_PP_Doc*_tmp647[2];return(_tmp647[1]=Cyc_PP_text(_dyneither_ptr_plus(_tmp96,sizeof(char),1)),((_tmp647[0]=Cyc_PP_text(((_tmp648="`TTT",_tag_dyneither(_tmp648,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp647,sizeof(struct Cyc_PP_Doc*),2)))));}
return Cyc_PP_text(_tmp96);}
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp99=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL5E;}_LL5D:
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 0)goto _LL60;else{if((_tmp9B->f1)->kind != Cyc_Absyn_BoxKind)goto _LL60;if((_tmp9B->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61:
 _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {
const char*_tmp64B;struct Cyc_PP_Doc*_tmp64A[3];return(_tmp64A[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp64A[1]=Cyc_PP_text(((_tmp64B="::",_tag_dyneither(_tmp64B,sizeof(char),3)))),((_tmp64A[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp64A,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp64E;const char*_tmp64D;const char*_tmp64C;return Cyc_PP_egroup(((_tmp64C="<",_tag_dyneither(_tmp64C,sizeof(char),2))),((_tmp64D=">",_tag_dyneither(_tmp64D,sizeof(char),2))),((_tmp64E=",",_tag_dyneither(_tmp64E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 448
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp651;const char*_tmp650;const char*_tmp64F;return Cyc_PP_egroup(((_tmp64F="<",_tag_dyneither(_tmp64F,sizeof(char),2))),((_tmp650=">",_tag_dyneither(_tmp650,sizeof(char),2))),((_tmp651=",",_tag_dyneither(_tmp651,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 458
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp654;const char*_tmp653;const char*_tmp652;return Cyc_PP_group(((_tmp652="(",_tag_dyneither(_tmp652,sizeof(char),2))),((_tmp653=")",_tag_dyneither(_tmp653,sizeof(char),2))),((_tmp654=",",_tag_dyneither(_tmp654,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 466
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpAB=att;_LL65: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66:
 goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAD=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}_LL68:
 goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A:
 return Cyc_PP_nil_doc();_LL6B:;_LL6C:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}
# 475
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpAF=(void*)atts->hd;_LL6E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {
const char*_tmp655;return Cyc_PP_text(((_tmp655=" _stdcall ",_tag_dyneither(_tmp655,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}_LL71: {
const char*_tmp656;return Cyc_PP_text(((_tmp656=" _cdecl ",_tag_dyneither(_tmp656,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp657;return Cyc_PP_text(((_tmp657=" _fastcall ",_tag_dyneither(_tmp657,sizeof(char),12))));}_LL74:;_LL75:
 goto _LL6D;_LL6D:;}
# 483
return Cyc_PP_nil_doc();}
# 486
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB6=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
 goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A:
 goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}_LL7C:
 goto _LL76;_LL7D:;_LL7E:
 hasatt=1;goto _LL76;_LL76:;}}
# 495
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp662;const char*_tmp661;const char*_tmp660;const char*_tmp65F;const char*_tmp65E;struct Cyc_PP_Doc*_tmp65D[3];return(_tmp65D[2]=
# 499
Cyc_PP_text(((_tmp65E=")",_tag_dyneither(_tmp65E,sizeof(char),2)))),((_tmp65D[1]=
# 498
Cyc_PP_group(((_tmp661="",_tag_dyneither(_tmp661,sizeof(char),1))),((_tmp660="",_tag_dyneither(_tmp660,sizeof(char),1))),((_tmp65F=" ",_tag_dyneither(_tmp65F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp65D[0]=
# 497
Cyc_PP_text(((_tmp662=" __declspec(",_tag_dyneither(_tmp662,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp65D,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 502
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 506
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp66B;const char*_tmp66A;const char*_tmp669;const char*_tmp668;struct Cyc_PP_Doc*_tmp667[2];return(_tmp667[1]=
Cyc_PP_group(((_tmp66A="((",_tag_dyneither(_tmp66A,sizeof(char),3))),((_tmp669="))",_tag_dyneither(_tmp669,sizeof(char),3))),((_tmp668=",",_tag_dyneither(_tmp668,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp667[0]=
# 510
Cyc_PP_text(((_tmp66B=" __attribute__",_tag_dyneither(_tmp66B,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp667,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 515
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC5=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC6=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}_LL84:
 return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86:
# 520
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL89:
 return 0;default: _LL8A:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL87:;_LL88:
# 524
 return 0;_LL82:;};}
# 528
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 531
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){
const char*_tmp66C;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp66C="{?}",_tag_dyneither(_tmp66C,sizeof(char),4))),1);}else{
const char*_tmp66D;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp66D="?",_tag_dyneither(_tmp66D,sizeof(char),2))));}}
# 538
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 540
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp66E;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp66E="{\\lb}",_tag_dyneither(_tmp66E,sizeof(char),6))),1);}else{
const char*_tmp66F;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp66F="{",_tag_dyneither(_tmp66F,sizeof(char),2))));}}
# 547
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 549
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp670;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp670="{\\rb}",_tag_dyneither(_tmp670,sizeof(char),6))),1);}else{
const char*_tmp671;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp671="}",_tag_dyneither(_tmp671,sizeof(char),2))));}}
# 556
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 558
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp672;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp672="\\$",_tag_dyneither(_tmp672,sizeof(char),3))),1);}else{
const char*_tmp673;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp673="$",_tag_dyneither(_tmp673,sizeof(char),2))));}}
# 565
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 567
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp674[3];return(_tmp674[2]=Cyc_Absynpp_rb(),((_tmp674[1]=Cyc_PP_seq(sep,ss),((_tmp674[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp674,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 572
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;struct Cyc_List_List*_tmpD6;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:
{const char*_tmp677;void*_tmp676;(_tmp676=0,Cyc_fprintf(Cyc_stderr,((_tmp677="Carray_mod ",_tag_dyneither(_tmp677,sizeof(char),12))),_tag_dyneither(_tmp676,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:
{const char*_tmp67A;void*_tmp679;(_tmp679=0,Cyc_fprintf(Cyc_stderr,((_tmp67A="ConstArray_mod ",_tag_dyneither(_tmp67A,sizeof(char),16))),_tag_dyneither(_tmp679,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpD4->f1);if(_tmpD5->tag != 1)goto _LL93;else{_tmpD6=_tmpD5->f1;}}}_LL92:
# 578
{const char*_tmp67D;void*_tmp67C;(_tmp67C=0,Cyc_fprintf(Cyc_stderr,((_tmp67D="Function_mod(",_tag_dyneither(_tmp67D,sizeof(char),14))),_tag_dyneither(_tmp67C,sizeof(void*),0)));}
for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _dyneither_ptr*_tmpE1=(*((struct _tuple8*)_tmpD6->hd)).f1;
if(_tmpE1 == 0){const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680="?",_tag_dyneither(_tmp680,sizeof(char),2))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}else{
void*_tmp681;(_tmp681=0,Cyc_fprintf(Cyc_stderr,*_tmpE1,_tag_dyneither(_tmp681,sizeof(void*),0)));}
if(_tmpD6->tl != 0){const char*_tmp684;void*_tmp683;(_tmp683=0,Cyc_fprintf(Cyc_stderr,((_tmp684=",",_tag_dyneither(_tmp684,sizeof(char),2))),_tag_dyneither(_tmp683,sizeof(void*),0)));}}
# 585
{const char*_tmp687;void*_tmp686;(_tmp686=0,Cyc_fprintf(Cyc_stderr,((_tmp687=") ",_tag_dyneither(_tmp687,sizeof(char),3))),_tag_dyneither(_tmp686,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD7->tag != 3)goto _LL95;}_LL94:
# 588
{const char*_tmp68A;void*_tmp689;(_tmp689=0,Cyc_fprintf(Cyc_stderr,((_tmp68A="Function_mod()",_tag_dyneither(_tmp68A,sizeof(char),15))),_tag_dyneither(_tmp689,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD8->tag != 5)goto _LL97;}_LL96:
{const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D="Attributes_mod ",_tag_dyneither(_tmp68D,sizeof(char),16))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD9->tag != 4)goto _LL99;}_LL98:
{const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690="TypeParams_mod ",_tag_dyneither(_tmp690,sizeof(char),16))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDA->tag != 2)goto _LL8C;}_LL9A:
{const char*_tmp693;void*_tmp692;(_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="Pointer_mod ",_tag_dyneither(_tmp693,sizeof(char),13))),_tag_dyneither(_tmp692,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 593
const char*_tmp696;void*_tmp695;(_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="\n",_tag_dyneither(_tmp696,sizeof(char),2))),_tag_dyneither(_tmp695,sizeof(void*),0)));};}
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp69B;const char*_tmp69A;struct Cyc_PP_Doc*_tmp699[3];struct Cyc_PP_Doc*p_rest=(_tmp699[2]=Cyc_PP_text(((_tmp69A=")",_tag_dyneither(_tmp69A,sizeof(char),2)))),((_tmp699[1]=rest,((_tmp699[0]=Cyc_PP_text(((_tmp69B="(",_tag_dyneither(_tmp69B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp699,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmpF3=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;union Cyc_Absyn_Constraint*_tmpF8;void*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;unsigned int _tmpFF;int _tmp100;void*_tmp102;union Cyc_Absyn_Constraint*_tmp103;union Cyc_Absyn_Constraint*_tmp104;union Cyc_Absyn_Constraint*_tmp105;struct Cyc_Absyn_Tqual _tmp106;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF4->tag != 0)goto _LL9E;else{_tmpF5=_tmpF4->f1;}}_LL9D:
# 604
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A0;const char*_tmp69F;struct Cyc_PP_Doc*_tmp69E[2];return(_tmp69E[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF5)?
Cyc_PP_text(((_tmp69F="[]@zeroterm",_tag_dyneither(_tmp69F,sizeof(char),12)))): Cyc_PP_text(((_tmp6A0="[]",_tag_dyneither(_tmp6A0,sizeof(char),3)))),((_tmp69E[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp69E,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF6=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF6->tag != 1)goto _LLA0;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL9F:
# 608
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A7;const char*_tmp6A6;const char*_tmp6A5;struct Cyc_PP_Doc*_tmp6A4[4];return(_tmp6A4[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8)?Cyc_PP_text(((_tmp6A5="]@zeroterm",_tag_dyneither(_tmp6A5,sizeof(char),11)))): Cyc_PP_text(((_tmp6A6="]",_tag_dyneither(_tmp6A6,sizeof(char),2)))),((_tmp6A4[2]=
# 609
Cyc_Absynpp_exp2doc(_tmpF7),((_tmp6A4[1]=Cyc_PP_text(((_tmp6A7="[",_tag_dyneither(_tmp6A7,sizeof(char),2)))),((_tmp6A4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A4,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpF9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF9->tag != 3)goto _LLA2;else{_tmpFA=(void*)_tmpF9->f1;}}_LLA1:
# 612
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp10E=_tmpFA;struct Cyc_List_List*_tmp110;int _tmp111;struct Cyc_Absyn_VarargInfo*_tmp112;void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp116;unsigned int _tmp117;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp10F=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp10F->tag != 1)goto _LLAB;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;_tmp113=(void*)_tmp10F->f4;_tmp114=_tmp10F->f5;}}_LLAA: {
# 615
struct Cyc_PP_Doc*_tmp6A8[2];return(_tmp6A8[1]=Cyc_Absynpp_funargs2doc(_tmp110,_tmp111,_tmp112,_tmp113,_tmp114),((_tmp6A8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A8,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp115=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp115->tag != 0)goto _LLA8;else{_tmp116=_tmp115->f1;_tmp117=_tmp115->f2;}}_LLAC: {
# 617
const char*_tmp6AF;const char*_tmp6AE;const char*_tmp6AD;struct Cyc_PP_Doc*_tmp6AC[2];return(_tmp6AC[1]=Cyc_PP_group(((_tmp6AF="(",_tag_dyneither(_tmp6AF,sizeof(char),2))),((_tmp6AE=")",_tag_dyneither(_tmp6AE,sizeof(char),2))),((_tmp6AD=",",_tag_dyneither(_tmp6AD,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp116)),((_tmp6AC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AC,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFB->tag != 5)goto _LLA4;else{_tmpFC=_tmpFB->f2;}}_LLA3:
# 620
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 622
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B0[2];return(_tmp6B0[1]=Cyc_Absynpp_atts2doc(_tmpFC),((_tmp6B0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B0,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 625
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6B1[2];return(_tmp6B1[1]=rest,((_tmp6B1[0]=Cyc_Absynpp_callconv2doc(_tmpFC),Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFD->tag != 4)goto _LLA6;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_tmp100=_tmpFD->f3;}}_LLA5:
# 630
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp100){
struct Cyc_PP_Doc*_tmp6B2[2];return(_tmp6B2[1]=Cyc_Absynpp_ktvars2doc(_tmpFE),((_tmp6B2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B2,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 634
struct Cyc_PP_Doc*_tmp6B3[2];return(_tmp6B3[1]=Cyc_Absynpp_tvars2doc(_tmpFE),((_tmp6B3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp101=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmp101->tag != 2)goto _LL9B;else{_tmp102=(_tmp101->f1).rgn;_tmp103=(_tmp101->f1).nullable;_tmp104=(_tmp101->f1).bounds;_tmp105=(_tmp101->f1).zero_term;_tmp106=_tmp101->f2;}}_LLA7: {
# 638
struct Cyc_PP_Doc*ptr;
{void*_tmp121=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp104);struct Cyc_Absyn_Exp*_tmp124;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp122=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp121;if(_tmp122->tag != 0)goto _LLB3;}_LLB2:
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp123=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp121;if(_tmp123->tag != 1)goto _LLB0;else{_tmp124=_tmp123->f1;}}_LLB4:
# 642
{const char*_tmp6B5;const char*_tmp6B4;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp103)?(_tmp6B5="*",_tag_dyneither(_tmp6B5,sizeof(char),2)):((_tmp6B4="@",_tag_dyneither(_tmp6B4,sizeof(char),2))));}{
unsigned int _tmp128;int _tmp129;struct _tuple11 _tmp127=Cyc_Evexp_eval_const_uint_exp(_tmp124);_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;
if(!_tmp129  || _tmp128 != 1){
struct Cyc_PP_Doc*_tmp6B6[4];ptr=((_tmp6B6[3]=Cyc_Absynpp_rb(),((_tmp6B6[2]=Cyc_Absynpp_exp2doc(_tmp124),((_tmp6B6[1]=Cyc_Absynpp_lb(),((_tmp6B6[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}
# 648
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6B9;struct Cyc_PP_Doc*_tmp6B8[2];ptr=((_tmp6B8[1]=Cyc_PP_text(((_tmp6B9="@zeroterm ",_tag_dyneither(_tmp6B9,sizeof(char),11)))),((_tmp6B8[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),2))))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6BC;struct Cyc_PP_Doc*_tmp6BB[2];ptr=((_tmp6BB[1]=Cyc_PP_text(((_tmp6BC="@nozeroterm ",_tag_dyneither(_tmp6BC,sizeof(char),13)))),((_tmp6BB[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 654
{void*_tmp12F=Cyc_Tcutil_compress(_tmp102);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp130=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12F;if(_tmp130->tag != 20)goto _LLB8;}_LLB7:
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp131=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12F;if(_tmp131->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
 goto _LLB5;_LLBA:;_LLBB: {
const char*_tmp6BF;struct Cyc_PP_Doc*_tmp6BE[3];ptr=((_tmp6BE[2]=Cyc_PP_text(((_tmp6BF=" ",_tag_dyneither(_tmp6BF,sizeof(char),2)))),((_tmp6BE[1]=Cyc_Absynpp_rgn2doc(_tmp102),((_tmp6BE[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 659
{struct Cyc_PP_Doc*_tmp6C0[2];ptr=((_tmp6C0[1]=Cyc_Absynpp_tqual2doc(_tmp106),((_tmp6C0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),2))))));}{
struct Cyc_PP_Doc*_tmp6C1[2];return(_tmp6C1[1]=rest,((_tmp6C1[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C1,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 664
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp139=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp139;if(_tmp13A->tag != 20)goto _LLBF;}_LLBE: {
const char*_tmp6C2;return Cyc_PP_text(((_tmp6C2="`H",_tag_dyneither(_tmp6C2,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp139;if(_tmp13B->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp6C3;return Cyc_PP_text(((_tmp6C3="`U",_tag_dyneither(_tmp6C3,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp139;if(_tmp13C->tag != 22)goto _LLC3;}_LLC2: {
const char*_tmp6C4;return Cyc_PP_text(((_tmp6C4="`RC",_tag_dyneither(_tmp6C4,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp139;if(_tmp13D->tag != 2)goto _LLC5;}_LLC4:
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
const char*_tmp6C9;const char*_tmp6C8;struct Cyc_PP_Doc*_tmp6C7[3];return(_tmp6C7[2]=Cyc_PP_text(((_tmp6C8=")",_tag_dyneither(_tmp6C8,sizeof(char),2)))),((_tmp6C7[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp6C7[0]=Cyc_PP_text(((_tmp6C9="@region(",_tag_dyneither(_tmp6C9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6C7,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 674
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 678
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp146;struct Cyc_List_List*_tmp148;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp144;if(_tmp145->tag != 23)goto _LLCA;else{_tmp146=(void*)_tmp145->f1;}}_LLC9:
{struct Cyc_List_List*_tmp6CA;*rgions=((_tmp6CA=_cycalloc(sizeof(*_tmp6CA)),((_tmp6CA->hd=Cyc_Absynpp_rgn2doc(_tmp146),((_tmp6CA->tl=*rgions,_tmp6CA))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp144;if(_tmp147->tag != 24)goto _LLCC;else{_tmp148=_tmp147->f1;}}_LLCB:
# 681
 for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp148->hd);}
# 684
goto _LLC7;_LLCC:;_LLCD:
{struct Cyc_List_List*_tmp6CB;*effects=((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB->hd=Cyc_Absynpp_typ2doc(t),((_tmp6CB->tl=*effects,_tmp6CB))))));}goto _LLC7;_LLC7:;}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6CE;const char*_tmp6CD;const char*_tmp6CC;return Cyc_PP_group(((_tmp6CC="",_tag_dyneither(_tmp6CC,sizeof(char),1))),((_tmp6CD="",_tag_dyneither(_tmp6CD,sizeof(char),1))),((_tmp6CE="+",_tag_dyneither(_tmp6CE,sizeof(char),2))),effects);}else{
# 697
const char*_tmp6CF;struct Cyc_PP_Doc*_tmp14E=Cyc_Absynpp_group_braces(((_tmp6CF=",",_tag_dyneither(_tmp6CF,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6D3;const char*_tmp6D2;const char*_tmp6D1;const char*_tmp6D0;return Cyc_PP_group(((_tmp6D0="",_tag_dyneither(_tmp6D0,sizeof(char),1))),((_tmp6D1="",_tag_dyneither(_tmp6D1,sizeof(char),1))),((_tmp6D2="+",_tag_dyneither(_tmp6D2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6D3=_cycalloc(sizeof(*_tmp6D3)),((_tmp6D3->hd=_tmp14E,((_tmp6D3->tl=0,_tmp6D3))))))));}}
# 702
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLCE: {
const char*_tmp6D4;return Cyc_PP_text(((_tmp6D4="struct ",_tag_dyneither(_tmp6D4,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
const char*_tmp6D5;return Cyc_PP_text(((_tmp6D5="union ",_tag_dyneither(_tmp6D5,sizeof(char),7))));}}}
# 710
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp156=t;struct Cyc_Core_Opt*_tmp15C;void*_tmp15D;int _tmp15E;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;union Cyc_Absyn_DatatypeInfoU _tmp163;struct Cyc_List_List*_tmp164;union Cyc_Absyn_DatatypeFieldInfoU _tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign _tmp169;enum Cyc_Absyn_Size_of _tmp16A;int _tmp16C;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*_tmp183;void*_tmp185;void*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp192;struct Cyc_Absyn_Datatypedecl*_tmp195;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 8)goto _LLD4;}_LLD3: {
# 714
const char*_tmp6D6;return Cyc_PP_text(((_tmp6D6="[[[array]]]",_tag_dyneither(_tmp6D6,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp156;if(_tmp158->tag != 9)goto _LLD6;}_LLD5:
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp156;if(_tmp159->tag != 5)goto _LLD8;}_LLD7:
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp156;if(_tmp15A->tag != 0)goto _LLDA;}_LLD9:
# 718
{const char*_tmp6D7;s=Cyc_PP_text(((_tmp6D7="void",_tag_dyneither(_tmp6D7,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp15B->tag != 1)goto _LLDC;else{_tmp15C=_tmp15B->f1;_tmp15D=(void*)_tmp15B->f2;_tmp15E=_tmp15B->f3;_tmp15F=_tmp15B->f4;}}_LLDB:
# 720
 if(_tmp15D != 0)
# 722
return Cyc_Absynpp_ntyp2doc(_tmp15D);else{
# 730
const char*_tmp6E7;const char*_tmp6E6;const char*_tmp6E5;struct Cyc_Int_pa_PrintArg_struct _tmp6E4;void*_tmp6E3[1];const char*_tmp6E2;const char*_tmp6E1;struct Cyc_PP_Doc*_tmp6E0[6];s=((_tmp6E0[5]=
# 735
_tmp15C == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15C->v),((_tmp6E0[4]=Cyc_PP_text(((_tmp6E1=")::",_tag_dyneither(_tmp6E1,sizeof(char),4)))),((_tmp6E0[3]=
# 733
(!Cyc_Absynpp_print_full_evars  || _tmp15F == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15F->v),((_tmp6E0[2]=
# 732
Cyc_PP_text((struct _dyneither_ptr)((_tmp6E4.tag=1,((_tmp6E4.f1=(unsigned long)_tmp15E,((_tmp6E3[0]=& _tmp6E4,Cyc_aprintf(((_tmp6E2="%d",_tag_dyneither(_tmp6E2,sizeof(char),3))),_tag_dyneither(_tmp6E3,sizeof(void*),1))))))))),((_tmp6E0[1]=
# 731
Cyc_PP_text(((_tmp6E5="(",_tag_dyneither(_tmp6E5,sizeof(char),2)))),((_tmp6E0[0]=
# 730
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E6="\\%",_tag_dyneither(_tmp6E6,sizeof(char),3)))): Cyc_PP_text(((_tmp6E7="%",_tag_dyneither(_tmp6E7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 738
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp156;if(_tmp160->tag != 2)goto _LLDE;else{_tmp161=_tmp160->f1;}}_LLDD:
# 740
 s=Cyc_PP_textptr(_tmp161->name);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6EA;struct Cyc_PP_Doc*_tmp6E9[3];s=((_tmp6E9[2]=Cyc_Absynpp_kindbound2doc(_tmp161->kind),((_tmp6E9[1]=Cyc_PP_text(((_tmp6EA="::",_tag_dyneither(_tmp6EA,sizeof(char),3)))),((_tmp6E9[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6E9,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 746
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp161)){
# 749
const char*_tmp6EF;const char*_tmp6EE;struct Cyc_PP_Doc*_tmp6ED[3];s=((_tmp6ED[2]=Cyc_PP_text(((_tmp6EE=" */",_tag_dyneither(_tmp6EE,sizeof(char),4)))),((_tmp6ED[1]=s,((_tmp6ED[0]=Cyc_PP_text(((_tmp6EF="_ /* ",_tag_dyneither(_tmp6EF,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6ED,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 752
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp156;if(_tmp162->tag != 3)goto _LLE0;else{_tmp163=(_tmp162->f1).datatype_info;_tmp164=(_tmp162->f1).targs;}}_LLDF:
# 754
{union Cyc_Absyn_DatatypeInfoU _tmp1A5=_tmp163;struct _tuple0*_tmp1A6;int _tmp1A7;struct _tuple0*_tmp1A8;int _tmp1A9;_LL10D: if((_tmp1A5.UnknownDatatype).tag != 1)goto _LL10F;_tmp1A6=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).name;_tmp1A7=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).is_extensible;_LL10E:
# 756
 _tmp1A8=_tmp1A6;_tmp1A9=_tmp1A7;goto _LL110;_LL10F: if((_tmp1A5.KnownDatatype).tag != 2)goto _LL10C;_tmp1A8=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))->name;_tmp1A9=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))->is_extensible;_LL110: {
# 758
const char*_tmp6F0;struct Cyc_PP_Doc*_tmp1AA=Cyc_PP_text(((_tmp6F0="datatype ",_tag_dyneither(_tmp6F0,sizeof(char),10))));
const char*_tmp6F1;struct Cyc_PP_Doc*_tmp1AB=_tmp1A9?Cyc_PP_text(((_tmp6F1="@extensible ",_tag_dyneither(_tmp6F1,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6F2[4];s=((_tmp6F2[3]=Cyc_Absynpp_tps2doc(_tmp164),((_tmp6F2[2]=Cyc_Absynpp_qvar2doc(_tmp1A8),((_tmp6F2[1]=_tmp1AA,((_tmp6F2[0]=_tmp1AB,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL10C;}_LL10C:;}
# 763
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp156;if(_tmp165->tag != 4)goto _LLE2;else{_tmp166=(_tmp165->f1).field_info;_tmp167=(_tmp165->f1).targs;}}_LLE1:
# 765
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AF=_tmp166;struct _tuple0*_tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;struct _tuple0*_tmp1B3;int _tmp1B4;struct _tuple0*_tmp1B5;_LL112: if((_tmp1AF.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).datatype_name;_tmp1B1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).field_name;_tmp1B2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).is_extensible;_LL113:
# 767
 _tmp1B3=_tmp1B0;_tmp1B4=_tmp1B2;_tmp1B5=_tmp1B1;goto _LL115;_LL114: if((_tmp1AF.KnownDatatypefield).tag != 2)goto _LL111;_tmp1B3=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1)->name;_tmp1B4=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1)->is_extensible;_tmp1B5=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f2)->name;_LL115: {
# 770
const char*_tmp6F4;const char*_tmp6F3;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B4?(_tmp6F4="@extensible datatype ",_tag_dyneither(_tmp6F4,sizeof(char),22)):((_tmp6F3="datatype ",_tag_dyneither(_tmp6F3,sizeof(char),10))));
{const char*_tmp6F7;struct Cyc_PP_Doc*_tmp6F6[4];s=((_tmp6F6[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp6F6[2]=Cyc_PP_text(((_tmp6F7=".",_tag_dyneither(_tmp6F7,sizeof(char),2)))),((_tmp6F6[1]=Cyc_Absynpp_qvar2doc(_tmp1B3),((_tmp6F6[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp6F6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 774
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp156;if(_tmp168->tag != 6)goto _LLE4;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE3: {
# 776
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp169){case Cyc_Absyn_None: _LL116:
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
{const char*_tmp6F8;sns=((_tmp6F8="",_tag_dyneither(_tmp6F8,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
{const char*_tmp6F9;sns=((_tmp6F9="unsigned ",_tag_dyneither(_tmp6F9,sizeof(char),10)));}break;}
# 783
switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL11A:
# 785
 switch(_tmp169){case Cyc_Absyn_None: _LL11C:
{const char*_tmp6FA;sns=((_tmp6FA="",_tag_dyneither(_tmp6FA,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
{const char*_tmp6FB;sns=((_tmp6FB="signed ",_tag_dyneither(_tmp6FB,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
{const char*_tmp6FC;sns=((_tmp6FC="unsigned ",_tag_dyneither(_tmp6FC,sizeof(char),10)));}break;}
# 790
{const char*_tmp6FD;ts=((_tmp6FD="char",_tag_dyneither(_tmp6FD,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL11B:
{const char*_tmp6FE;ts=((_tmp6FE="short",_tag_dyneither(_tmp6FE,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
{const char*_tmp6FF;ts=((_tmp6FF="int",_tag_dyneither(_tmp6FF,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
{const char*_tmp700;ts=((_tmp700="long",_tag_dyneither(_tmp700,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 796
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
{const char*_tmp701;ts=((_tmp701="long long",_tag_dyneither(_tmp701,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
{const char*_tmp702;ts=((_tmp702="__int64",_tag_dyneither(_tmp702,sizeof(char),8)));}break;}
# 800
break;}
# 802
{const char*_tmp707;void*_tmp706[2];struct Cyc_String_pa_PrintArg_struct _tmp705;struct Cyc_String_pa_PrintArg_struct _tmp704;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp706[0]=& _tmp705,((_tmp706[1]=& _tmp704,Cyc_aprintf(((_tmp707="%s%s",_tag_dyneither(_tmp707,sizeof(char),5))),_tag_dyneither(_tmp706,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp156;if(_tmp16B->tag != 7)goto _LLE6;else{_tmp16C=_tmp16B->f1;}}_LLE5:
# 805
 if(_tmp16C == 0){
const char*_tmp708;s=Cyc_PP_text(((_tmp708="float",_tag_dyneither(_tmp708,sizeof(char),6))));}else{
if(_tmp16C == 1){
const char*_tmp709;s=Cyc_PP_text(((_tmp709="double",_tag_dyneither(_tmp709,sizeof(char),7))));}else{
# 810
const char*_tmp70A;s=Cyc_PP_text(((_tmp70A="long double",_tag_dyneither(_tmp70A,sizeof(char),12))));}}
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp156;if(_tmp16D->tag != 10)goto _LLE8;else{_tmp16E=_tmp16D->f1;}}_LLE7:
# 813
{struct Cyc_PP_Doc*_tmp70B[2];s=((_tmp70B[1]=Cyc_Absynpp_args2doc(_tmp16E),((_tmp70B[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp156;if(_tmp16F->tag != 11)goto _LLEA;else{_tmp170=(_tmp16F->f1).aggr_info;_tmp171=(_tmp16F->f1).targs;}}_LLE9: {
# 816
enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*_tmp1D0;struct _tuple10 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;
{struct Cyc_PP_Doc*_tmp70C[3];s=((_tmp70C[2]=Cyc_Absynpp_tps2doc(_tmp171),((_tmp70C[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp70C[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp156;if(_tmp172->tag != 12)goto _LLEC;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LLEB:
# 820
{struct Cyc_PP_Doc*_tmp70D[4];s=((_tmp70D[3]=
# 822
Cyc_Absynpp_rb(),((_tmp70D[2]=
# 821
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp70D[1]=
# 820
Cyc_Absynpp_lb(),((_tmp70D[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp70D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 823
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp156;if(_tmp175->tag != 14)goto _LLEE;else{_tmp176=_tmp175->f1;}}_LLED:
# 825
{const char*_tmp710;struct Cyc_PP_Doc*_tmp70F[4];s=((_tmp70F[3]=Cyc_Absynpp_rb(),((_tmp70F[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp176)),((_tmp70F[1]=Cyc_Absynpp_lb(),((_tmp70F[0]=Cyc_PP_text(((_tmp710="enum ",_tag_dyneither(_tmp710,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp156;if(_tmp177->tag != 13)goto _LLF0;else{_tmp178=_tmp177->f1;}}_LLEF:
# 827
{const char*_tmp713;struct Cyc_PP_Doc*_tmp712[2];s=((_tmp712[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp712[0]=Cyc_PP_text(((_tmp713="enum ",_tag_dyneither(_tmp713,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp712,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp156;if(_tmp179->tag != 18)goto _LLF2;else{_tmp17A=_tmp179->f1;}}_LLF1:
# 829
{const char*_tmp718;const char*_tmp717;struct Cyc_PP_Doc*_tmp716[3];s=((_tmp716[2]=Cyc_PP_text(((_tmp717=")",_tag_dyneither(_tmp717,sizeof(char),2)))),((_tmp716[1]=Cyc_Absynpp_exp2doc(_tmp17A),((_tmp716[0]=Cyc_PP_text(((_tmp718="valueof_t(",_tag_dyneither(_tmp718,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp716,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp156;if(_tmp17B->tag != 17)goto _LLF4;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF3:
# 836
{struct Cyc_PP_Doc*_tmp719[2];s=((_tmp719[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp719[0]=Cyc_Absynpp_qvar2doc(_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp719,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp156;if(_tmp17F->tag != 15)goto _LLF6;else{_tmp180=(void*)_tmp17F->f1;}}_LLF5:
# 839
{const char*_tmp71E;const char*_tmp71D;struct Cyc_PP_Doc*_tmp71C[3];s=((_tmp71C[2]=Cyc_PP_text(((_tmp71D=">",_tag_dyneither(_tmp71D,sizeof(char),2)))),((_tmp71C[1]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp71C[0]=Cyc_PP_text(((_tmp71E="region_t<",_tag_dyneither(_tmp71E,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp156;if(_tmp181->tag != 16)goto _LLF8;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF7:
# 841
{const char*_tmp725;const char*_tmp724;const char*_tmp723;struct Cyc_PP_Doc*_tmp722[5];s=((_tmp722[4]=
Cyc_PP_text(((_tmp723=">",_tag_dyneither(_tmp723,sizeof(char),2)))),((_tmp722[3]=
# 841
Cyc_Absynpp_rgn2doc(_tmp183),((_tmp722[2]=Cyc_PP_text(((_tmp724=",",_tag_dyneither(_tmp724,sizeof(char),2)))),((_tmp722[1]=Cyc_Absynpp_rgn2doc(_tmp182),((_tmp722[0]=Cyc_PP_text(((_tmp725="dynregion_t<",_tag_dyneither(_tmp725,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp722,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp156;if(_tmp184->tag != 19)goto _LLFA;else{_tmp185=(void*)_tmp184->f1;}}_LLF9:
# 844
{const char*_tmp72A;const char*_tmp729;struct Cyc_PP_Doc*_tmp728[3];s=((_tmp728[2]=Cyc_PP_text(((_tmp729=">",_tag_dyneither(_tmp729,sizeof(char),2)))),((_tmp728[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp728[0]=Cyc_PP_text(((_tmp72A="tag_t<",_tag_dyneither(_tmp72A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp728,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp156;if(_tmp186->tag != 21)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp156;if(_tmp187->tag != 20)goto _LLFE;}_LLFD:
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp156;if(_tmp188->tag != 22)goto _LL100;}_LLFF:
# 848
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp156;if(_tmp189->tag != 25)goto _LL102;else{_tmp18A=(void*)_tmp189->f1;}}_LL101:
# 850
{const char*_tmp72F;const char*_tmp72E;struct Cyc_PP_Doc*_tmp72D[3];s=((_tmp72D[2]=Cyc_PP_text(((_tmp72E=")",_tag_dyneither(_tmp72E,sizeof(char),2)))),((_tmp72D[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp72D[0]=Cyc_PP_text(((_tmp72F="regions(",_tag_dyneither(_tmp72F,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp156;if(_tmp18B->tag != 23)goto _LL104;}_LL103:
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp156;if(_tmp18C->tag != 24)goto _LL106;}_LL105:
# 853
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp18D->tag != 26)goto _LL108;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp18E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp18D->f1)->r;if(_tmp18E->tag != 0)goto _LL108;else{_tmp18F=_tmp18E->f1;}}}_LL107:
# 855
 s=Cyc_Absynpp_aggrdecl2doc(_tmp18F);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp190->tag != 26)goto _LL10A;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp191=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp190->f1)->r;if(_tmp191->tag != 1)goto _LL10A;else{_tmp192=_tmp191->f1;}}}_LL109:
# 857
 s=Cyc_Absynpp_enumdecl2doc(_tmp192);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp193->tag != 26)goto _LLD1;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp194=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp193->f1)->r;if(_tmp194->tag != 2)goto _LLD1;else{_tmp195=_tmp194->f1;}}}_LL10B:
# 859
 s=Cyc_Absynpp_datatypedecl2doc(_tmp195);goto _LLD1;_LLD1:;}
# 861
return s;}
# 864
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 868
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1E9;void*_tmp1EA;struct _tuple16*_tmp1E8=cmp;_tmp1E9=_tmp1E8->f1;_tmp1EA=_tmp1E8->f2;{
const char*_tmp732;struct Cyc_PP_Doc*_tmp731[3];return(_tmp731[2]=Cyc_Absynpp_rgn2doc(_tmp1EA),((_tmp731[1]=Cyc_PP_text(((_tmp732=" > ",_tag_dyneither(_tmp732,sizeof(char),4)))),((_tmp731[0]=Cyc_Absynpp_rgn2doc(_tmp1E9),Cyc_PP_cat(_tag_dyneither(_tmp731,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp735;const char*_tmp734;const char*_tmp733;return Cyc_PP_group(((_tmp733="",_tag_dyneither(_tmp733,sizeof(char),1))),((_tmp734="",_tag_dyneither(_tmp734,sizeof(char),1))),((_tmp735=",",_tag_dyneither(_tmp735,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1F0=(*t).f1;
struct Cyc_Core_Opt*_tmp736;struct Cyc_Core_Opt*dopt=_tmp1F0 == 0?0:((_tmp736=_cycalloc(sizeof(*_tmp736)),((_tmp736->v=Cyc_PP_text(*_tmp1F0),_tmp736))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 886
struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp739;struct Cyc_List_List*_tmp738;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp738=_cycalloc(sizeof(*_tmp738)),((_tmp738->hd=Cyc_PP_text(((_tmp739="...",_tag_dyneither(_tmp739,sizeof(char),4)))),((_tmp738->tl=0,_tmp738)))))));}else{
if(cyc_varargs != 0){
const char*_tmp742;const char*_tmp741;const char*_tmp740;struct _tuple8*_tmp73F;struct Cyc_PP_Doc*_tmp73E[3];struct Cyc_PP_Doc*_tmp1F5=(_tmp73E[2]=
# 893
Cyc_Absynpp_funarg2doc(((_tmp73F=_cycalloc(sizeof(*_tmp73F)),((_tmp73F->f1=cyc_varargs->name,((_tmp73F->f2=cyc_varargs->tq,((_tmp73F->f3=cyc_varargs->type,_tmp73F))))))))),((_tmp73E[1]=
# 892
cyc_varargs->inject?Cyc_PP_text(((_tmp740=" inject ",_tag_dyneither(_tmp740,sizeof(char),9)))): Cyc_PP_text(((_tmp741=" ",_tag_dyneither(_tmp741,sizeof(char),2)))),((_tmp73E[0]=
# 891
Cyc_PP_text(((_tmp742="...",_tag_dyneither(_tmp742,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp73E,sizeof(struct Cyc_PP_Doc*),3)))))));
# 895
struct Cyc_List_List*_tmp743;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp743=_cycalloc(sizeof(*_tmp743)),((_tmp743->hd=_tmp1F5,((_tmp743->tl=0,_tmp743)))))));}}{
# 897
const char*_tmp746;const char*_tmp745;const char*_tmp744;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp744="",_tag_dyneither(_tmp744,sizeof(char),1))),((_tmp745="",_tag_dyneither(_tmp745,sizeof(char),1))),((_tmp746=",",_tag_dyneither(_tmp746,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp749;struct Cyc_PP_Doc*_tmp748[3];_tmp1FC=((_tmp748[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp748[1]=Cyc_PP_text(((_tmp749=";",_tag_dyneither(_tmp749,sizeof(char),2)))),((_tmp748[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp748,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp74C;struct Cyc_PP_Doc*_tmp74B[3];_tmp1FC=((_tmp74B[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74B[1]=Cyc_PP_text(((_tmp74C=":",_tag_dyneither(_tmp74C,sizeof(char),2)))),((_tmp74B[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp751;const char*_tmp750;struct Cyc_PP_Doc*_tmp74F[3];return(_tmp74F[2]=Cyc_PP_text(((_tmp750=")",_tag_dyneither(_tmp750,sizeof(char),2)))),((_tmp74F[1]=_tmp1FC,((_tmp74F[0]=Cyc_PP_text(((_tmp751="(",_tag_dyneither(_tmp751,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp74F,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 905
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp208;struct Cyc_Absyn_Tqual _tmp209;void*_tmp20A;struct _tuple8*_tmp207=arg;_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;_tmp20A=_tmp207->f3;{
struct _tuple8*_tmp752;return(_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752->f1=_tmp208,((_tmp752->f2=_tmp209,((_tmp752->f3=_tmp20A,_tmp752)))))));};}
# 910
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 912
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp20C=0;
int match;
{union Cyc_Absyn_Nmspace _tmp20D=(*q).f1;struct Cyc_List_List*_tmp20E;struct Cyc_List_List*_tmp20F;struct Cyc_List_List*_tmp210;_LL128: if((_tmp20D.Loc_n).tag != 4)goto _LL12A;_LL129:
# 917
 _tmp20E=0;goto _LL12B;_LL12A: if((_tmp20D.Rel_n).tag != 1)goto _LL12C;_tmp20E=(struct Cyc_List_List*)(_tmp20D.Rel_n).val;_LL12B:
# 919
 match=0;
_tmp20C=_tmp20E;
goto _LL127;_LL12C: if((_tmp20D.C_n).tag != 3)goto _LL12E;_tmp20F=(struct Cyc_List_List*)(_tmp20D.C_n).val;_LL12D:
# 923
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20F,Cyc_Absynpp_curr_namespace);
# 925
goto _LL127;_LL12E: if((_tmp20D.Abs_n).tag != 2)goto _LL127;_tmp210=(struct Cyc_List_List*)(_tmp20D.Abs_n).val;_LL12F:
# 927
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp210,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp753;_tmp20C=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753->hd=Cyc_Absynpp_cyc_stringptr,((_tmp753->tl=_tmp210,_tmp753))))): _tmp210;}
goto _LL127;_LL127:;}
# 931
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp755;struct Cyc_List_List*_tmp754;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754->hd=(*q).f2,((_tmp754->tl=0,_tmp754))))))),((_tmp755="_",_tag_dyneither(_tmp755,sizeof(char),2))));}else{
# 936
if(match)
return*(*q).f2;else{
# 939
const char*_tmp757;struct Cyc_List_List*_tmp756;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->hd=(*q).f2,((_tmp756->tl=0,_tmp756))))))),((_tmp757="::",_tag_dyneither(_tmp757,sizeof(char),3))));}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp216=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 952
const char*_tmp759;const char*_tmp758;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp758="\\textbf{",_tag_dyneither(_tmp758,sizeof(char),9))),(struct _dyneither_ptr)_tmp216),((_tmp759="}",_tag_dyneither(_tmp759,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp216));}else{
return Cyc_PP_text(_tmp216);}}
# 957
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 959
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 962
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp219=(*v).f1;struct Cyc_List_List*_tmp21A;struct Cyc_List_List*_tmp21B;_LL131: if((_tmp219.Loc_n).tag != 4)goto _LL133;_LL132:
 goto _LL134;_LL133: if((_tmp219.Rel_n).tag != 1)goto _LL135;if((struct Cyc_List_List*)(_tmp219.Rel_n).val != 0)goto _LL135;_LL134:
 return*(*v).f2;_LL135: if((_tmp219.C_n).tag != 3)goto _LL137;_tmp21A=(struct Cyc_List_List*)(_tmp219.C_n).val;_LL136:
 _tmp21B=_tmp21A;goto _LL138;_LL137: if((_tmp219.Abs_n).tag != 2)goto _LL139;_tmp21B=(struct Cyc_List_List*)(_tmp219.Abs_n).val;_LL138:
# 968
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp21B,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 971
goto _LL13A;}_LL139:;_LL13A: {
# 973
const char*_tmp75A;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp75A="/* bad namespace : */ ",_tag_dyneither(_tmp75A,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
# 976
return*(*v).f2;}}
# 979
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 982
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp21D=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 986
const char*_tmp75C;const char*_tmp75B;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75B="\\textbf{",_tag_dyneither(_tmp75B,sizeof(char),9))),(struct _dyneither_ptr)_tmp21D),((_tmp75C="}",_tag_dyneither(_tmp75C,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp21D));}else{
return Cyc_PP_text(_tmp21D);}}
# 991
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1002 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp220=e->r;enum Cyc_Absyn_Primop _tmp224;struct Cyc_Absyn_Exp*_tmp22E;struct Cyc_Absyn_Exp*_tmp230;_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp221=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp221->tag != 0)goto _LL13E;}_LL13D:
 goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp222=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp222->tag != 1)goto _LL140;}_LL13F:
 return 10000;_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp223=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp223->tag != 2)goto _LL142;else{_tmp224=_tmp223->f1;}}_LL141:
# 1007
 switch(_tmp224){case Cyc_Absyn_Plus: _LL18A:
 return 100;case Cyc_Absyn_Times: _LL18B:
 return 110;case Cyc_Absyn_Minus: _LL18C:
 return 100;case Cyc_Absyn_Div: _LL18D:
 goto _LL18E;case Cyc_Absyn_Mod: _LL18E:
 return 110;case Cyc_Absyn_Eq: _LL18F:
 goto _LL190;case Cyc_Absyn_Neq: _LL190:
 return 70;case Cyc_Absyn_Gt: _LL191:
 goto _LL192;case Cyc_Absyn_Lt: _LL192:
 goto _LL193;case Cyc_Absyn_Gte: _LL193:
 goto _LL194;case Cyc_Absyn_Lte: _LL194:
 return 80;case Cyc_Absyn_Not: _LL195:
 goto _LL196;case Cyc_Absyn_Bitnot: _LL196:
 return 130;case Cyc_Absyn_Bitand: _LL197:
 return 60;case Cyc_Absyn_Bitor: _LL198:
 return 40;case Cyc_Absyn_Bitxor: _LL199:
 return 50;case Cyc_Absyn_Bitlshift: _LL19A:
 return 90;case Cyc_Absyn_Bitlrshift: _LL19B:
 return 80;case Cyc_Absyn_Bitarshift: _LL19C:
 return 80;case Cyc_Absyn_Numelts: _LL19D:
 return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp225->tag != 3)goto _LL144;}_LL143:
# 1029
 return 20;_LL144: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp226=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp226->tag != 4)goto _LL146;}_LL145:
 return 130;_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp227=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp227->tag != 5)goto _LL148;}_LL147:
 return 30;_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp228->tag != 6)goto _LL14A;}_LL149:
 return 35;_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp229=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp229->tag != 7)goto _LL14C;}_LL14B:
 return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp22A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp22A->tag != 8)goto _LL14E;}_LL14D:
 return 10;_LL14E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp22B->tag != 9)goto _LL150;}_LL14F:
 return 140;_LL150: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp22C=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp22C->tag != 10)goto _LL152;}_LL151:
 return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp22D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp22D->tag != 11)goto _LL154;else{_tmp22E=_tmp22D->f1;}}_LL153:
 return Cyc_Absynpp_exp_prec(_tmp22E);_LL154: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp22F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp22F->tag != 12)goto _LL156;else{_tmp230=_tmp22F->f1;}}_LL155:
 return Cyc_Absynpp_exp_prec(_tmp230);_LL156: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp231=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp231->tag != 13)goto _LL158;}_LL157:
 return 120;_LL158: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp232=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp232->tag != 15)goto _LL15A;}_LL159:
 return 15;_LL15A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp233=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp233->tag != 14)goto _LL15C;}_LL15B:
 goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp234=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp234->tag != 16)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp235=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp235->tag != 17)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp236->tag != 37)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp237->tag != 38)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp238=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp238->tag != 36)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp239->tag != 18)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23A->tag != 19)goto _LL16A;}_LL169:
 return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23B->tag != 20)goto _LL16C;}_LL16B:
 goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp23C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23C->tag != 21)goto _LL16E;}_LL16D:
 goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23D->tag != 22)goto _LL170;}_LL16F:
 return 140;_LL170: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23E->tag != 23)goto _LL172;}_LL171:
 return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp23F->tag != 24)goto _LL174;}_LL173:
 goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp240->tag != 25)goto _LL176;}_LL175:
 goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp241->tag != 26)goto _LL178;}_LL177:
 goto _LL179;_LL178: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp242->tag != 27)goto _LL17A;}_LL179:
 goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp243->tag != 28)goto _LL17C;}_LL17B:
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp244->tag != 29)goto _LL17E;}_LL17D:
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp245->tag != 30)goto _LL180;}_LL17F:
 goto _LL181;_LL180: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp246->tag != 31)goto _LL182;}_LL181:
 goto _LL183;_LL182: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp247->tag != 32)goto _LL184;}_LL183:
 goto _LL185;_LL184: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp248->tag != 33)goto _LL186;}_LL185:
 goto _LL187;_LL186: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp249->tag != 34)goto _LL188;}_LL187:
 return 140;_LL188: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp220;if(_tmp24A->tag != 35)goto _LL13B;}_LL189:
 return 10000;_LL13B:;}
# 1068
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1072
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp24B=e->r;union Cyc_Absyn_Cnst _tmp24D;struct _tuple0*_tmp24F;enum Cyc_Absyn_Primop _tmp251;struct Cyc_List_List*_tmp252;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Core_Opt*_tmp255;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_Absyn_Exp*_tmp258;enum Cyc_Absyn_Incrementor _tmp259;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26F;void*_tmp271;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp277;void*_tmp279;struct Cyc_Absyn_Exp*_tmp27B;void*_tmp27D;int _tmp27F;struct _dyneither_ptr _tmp280;struct Cyc_Absyn_Exp*_tmp282;struct _dyneither_ptr*_tmp283;void*_tmp285;struct Cyc_List_List*_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct Cyc_Absyn_Exp*_tmp28A;struct _dyneither_ptr*_tmp28B;struct Cyc_Absyn_Exp*_tmp28D;struct _dyneither_ptr*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_List_List*_tmp293;struct _tuple8*_tmp295;struct Cyc_List_List*_tmp296;struct Cyc_List_List*_tmp298;struct Cyc_Absyn_Vardecl*_tmp29A;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29C;struct _tuple0*_tmp29E;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*_tmp2A4;struct Cyc_Absyn_Datatypefield*_tmp2A5;struct _tuple0*_tmp2A7;struct _tuple0*_tmp2A9;int _tmp2AB;struct Cyc_Absyn_Exp*_tmp2AC;void**_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Core_Opt*_tmp2B3;struct Cyc_List_List*_tmp2B4;struct Cyc_Absyn_Stmt*_tmp2B6;_LL1A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp24C->tag != 0)goto _LL1A2;else{_tmp24D=_tmp24C->f1;}}_LL1A1:
 s=Cyc_Absynpp_cnst2doc(_tmp24D);goto _LL19F;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp24E->tag != 1)goto _LL1A4;else{_tmp24F=_tmp24E->f1;}}_LL1A3:
 s=Cyc_Absynpp_qvar2doc(_tmp24F);goto _LL19F;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp250->tag != 2)goto _LL1A6;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}_LL1A5:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp251,_tmp252);goto _LL19F;_LL1A6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp253->tag != 3)goto _LL1A8;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;_tmp256=_tmp253->f3;}}_LL1A7:
# 1080
{const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[5];s=((_tmp75F[4]=
# 1084
Cyc_Absynpp_exp2doc_prec(myprec,_tmp256),((_tmp75F[3]=
# 1083
Cyc_PP_text(((_tmp760="= ",_tag_dyneither(_tmp760,sizeof(char),3)))),((_tmp75F[2]=
# 1082
_tmp255 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp255->v),((_tmp75F[1]=
# 1081
Cyc_PP_text(((_tmp761=" ",_tag_dyneither(_tmp761,sizeof(char),2)))),((_tmp75F[0]=
# 1080
Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1085
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp257->tag != 4)goto _LL1AA;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}_LL1A9: {
# 1087
struct Cyc_PP_Doc*_tmp2BA=Cyc_Absynpp_exp2doc_prec(myprec,_tmp258);
switch(_tmp259){case Cyc_Absyn_PreInc: _LL1EE:
{const char*_tmp764;struct Cyc_PP_Doc*_tmp763[2];s=((_tmp763[1]=_tmp2BA,((_tmp763[0]=Cyc_PP_text(((_tmp764="++",_tag_dyneither(_tmp764,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp763,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1EF:
{const char*_tmp767;struct Cyc_PP_Doc*_tmp766[2];s=((_tmp766[1]=_tmp2BA,((_tmp766[0]=Cyc_PP_text(((_tmp767="--",_tag_dyneither(_tmp767,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F0:
{const char*_tmp76A;struct Cyc_PP_Doc*_tmp769[2];s=((_tmp769[1]=Cyc_PP_text(((_tmp76A="++",_tag_dyneither(_tmp76A,sizeof(char),3)))),((_tmp769[0]=_tmp2BA,Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F1:
{const char*_tmp76D;struct Cyc_PP_Doc*_tmp76C[2];s=((_tmp76C[1]=Cyc_PP_text(((_tmp76D="--",_tag_dyneither(_tmp76D,sizeof(char),3)))),((_tmp76C[0]=_tmp2BA,Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1094
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25A->tag != 5)goto _LL1AC;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;_tmp25D=_tmp25A->f3;}}_LL1AB:
# 1096
{const char*_tmp772;const char*_tmp771;struct Cyc_PP_Doc*_tmp770[5];s=((_tmp770[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),((_tmp770[3]=Cyc_PP_text(((_tmp771=" : ",_tag_dyneither(_tmp771,sizeof(char),4)))),((_tmp770[2]=
# 1096
Cyc_Absynpp_exp2doc_prec(0,_tmp25C),((_tmp770[1]=Cyc_PP_text(((_tmp772=" ? ",_tag_dyneither(_tmp772,sizeof(char),4)))),((_tmp770[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1098
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25E->tag != 6)goto _LL1AE;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL1AD:
# 1100
{const char*_tmp775;struct Cyc_PP_Doc*_tmp774[3];s=((_tmp774[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),((_tmp774[1]=Cyc_PP_text(((_tmp775=" && ",_tag_dyneither(_tmp775,sizeof(char),5)))),((_tmp774[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25F),Cyc_PP_cat(_tag_dyneither(_tmp774,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp261->tag != 7)goto _LL1B0;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;}}_LL1AF:
# 1103
{const char*_tmp778;struct Cyc_PP_Doc*_tmp777[3];s=((_tmp777[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp263),((_tmp777[1]=Cyc_PP_text(((_tmp778=" || ",_tag_dyneither(_tmp778,sizeof(char),5)))),((_tmp777[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp262),Cyc_PP_cat(_tag_dyneither(_tmp777,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp264->tag != 8)goto _LL1B2;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1B1:
# 1108
{const char*_tmp77F;const char*_tmp77E;const char*_tmp77D;struct Cyc_PP_Doc*_tmp77C[5];s=((_tmp77C[4]=Cyc_PP_text(((_tmp77D=")",_tag_dyneither(_tmp77D,sizeof(char),2)))),((_tmp77C[3]=Cyc_Absynpp_exp2doc(_tmp266),((_tmp77C[2]=Cyc_PP_text(((_tmp77E=", ",_tag_dyneither(_tmp77E,sizeof(char),3)))),((_tmp77C[1]=Cyc_Absynpp_exp2doc(_tmp265),((_tmp77C[0]=Cyc_PP_text(((_tmp77F="(",_tag_dyneither(_tmp77F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp267->tag != 9)goto _LL1B4;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}_LL1B3:
# 1111
{const char*_tmp784;const char*_tmp783;struct Cyc_PP_Doc*_tmp782[4];s=((_tmp782[3]=
# 1114
Cyc_PP_text(((_tmp783=")",_tag_dyneither(_tmp783,sizeof(char),2)))),((_tmp782[2]=
# 1113
Cyc_Absynpp_exps2doc_prec(20,_tmp269),((_tmp782[1]=
# 1112
Cyc_PP_text(((_tmp784="(",_tag_dyneither(_tmp784,sizeof(char),2)))),((_tmp782[0]=
# 1111
Cyc_Absynpp_exp2doc_prec(myprec,_tmp268),Cyc_PP_cat(_tag_dyneither(_tmp782,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1115
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26A->tag != 10)goto _LL1B6;else{_tmp26B=_tmp26A->f1;}}_LL1B5:
# 1117
{const char*_tmp787;struct Cyc_PP_Doc*_tmp786[2];s=((_tmp786[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26B),((_tmp786[0]=Cyc_PP_text(((_tmp787="throw ",_tag_dyneither(_tmp787,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp786,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26C->tag != 11)goto _LL1B8;else{_tmp26D=_tmp26C->f1;}}_LL1B7:
# 1120
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26D);
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26E->tag != 12)goto _LL1BA;else{_tmp26F=_tmp26E->f1;}}_LL1B9:
# 1123
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26F);
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp270->tag != 13)goto _LL1BC;else{_tmp271=(void*)_tmp270->f1;_tmp272=_tmp270->f2;}}_LL1BB:
# 1126
{const char*_tmp78C;const char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[4];s=((_tmp78A[3]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp78A[2]=
# 1128
Cyc_PP_text(((_tmp78B=")",_tag_dyneither(_tmp78B,sizeof(char),2)))),((_tmp78A[1]=
# 1127
Cyc_Absynpp_typ2doc(_tmp271),((_tmp78A[0]=
# 1126
Cyc_PP_text(((_tmp78C="(",_tag_dyneither(_tmp78C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1130
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp273->tag != 14)goto _LL1BE;else{_tmp274=_tmp273->f1;}}_LL1BD:
# 1132
{const char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[2];s=((_tmp78E[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp274),((_tmp78E[0]=
# 1132
Cyc_PP_text(((_tmp78F="&",_tag_dyneither(_tmp78F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78E,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1134
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp275->tag != 15)goto _LL1C0;else{_tmp276=_tmp275->f1;_tmp277=_tmp275->f2;}}_LL1BF:
# 1136
 if(_tmp276 == 0){
const char*_tmp792;struct Cyc_PP_Doc*_tmp791[2];s=((_tmp791[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp791[0]=Cyc_PP_text(((_tmp792="new ",_tag_dyneither(_tmp792,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp791,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1139
const char*_tmp797;const char*_tmp796;struct Cyc_PP_Doc*_tmp795[4];s=((_tmp795[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp795[2]=
# 1139
Cyc_PP_text(((_tmp796=") ",_tag_dyneither(_tmp796,sizeof(char),3)))),((_tmp795[1]=Cyc_Absynpp_exp2doc(_tmp276),((_tmp795[0]=Cyc_PP_text(((_tmp797="rnew(",_tag_dyneither(_tmp797,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp795,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1141
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp278->tag != 16)goto _LL1C2;else{_tmp279=(void*)_tmp278->f1;}}_LL1C1:
{const char*_tmp79C;const char*_tmp79B;struct Cyc_PP_Doc*_tmp79A[3];s=((_tmp79A[2]=Cyc_PP_text(((_tmp79B=")",_tag_dyneither(_tmp79B,sizeof(char),2)))),((_tmp79A[1]=Cyc_Absynpp_typ2doc(_tmp279),((_tmp79A[0]=Cyc_PP_text(((_tmp79C="sizeof(",_tag_dyneither(_tmp79C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79A,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27A->tag != 17)goto _LL1C4;else{_tmp27B=_tmp27A->f1;}}_LL1C3:
{const char*_tmp7A1;const char*_tmp7A0;struct Cyc_PP_Doc*_tmp79F[3];s=((_tmp79F[2]=Cyc_PP_text(((_tmp7A0=")",_tag_dyneither(_tmp7A0,sizeof(char),2)))),((_tmp79F[1]=Cyc_Absynpp_exp2doc(_tmp27B),((_tmp79F[0]=Cyc_PP_text(((_tmp7A1="sizeof(",_tag_dyneither(_tmp7A1,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27C->tag != 37)goto _LL1C6;else{_tmp27D=(void*)_tmp27C->f1;}}_LL1C5:
{const char*_tmp7A6;const char*_tmp7A5;struct Cyc_PP_Doc*_tmp7A4[3];s=((_tmp7A4[2]=Cyc_PP_text(((_tmp7A5=")",_tag_dyneither(_tmp7A5,sizeof(char),2)))),((_tmp7A4[1]=Cyc_Absynpp_typ2doc(_tmp27D),((_tmp7A4[0]=Cyc_PP_text(((_tmp7A6="valueof(",_tag_dyneither(_tmp7A6,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A4,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27E->tag != 38)goto _LL1C8;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}_LL1C7:
# 1146
 if(_tmp27F){
const char*_tmp7AD;const char*_tmp7AC;const char*_tmp7AB;struct Cyc_PP_Doc*_tmp7AA[4];s=((_tmp7AA[3]=Cyc_PP_text(((_tmp7AB=")",_tag_dyneither(_tmp7AB,sizeof(char),2)))),((_tmp7AA[2]=Cyc_PP_text(_tmp280),((_tmp7AA[1]=Cyc_PP_text(((_tmp7AC=" volatile (",_tag_dyneither(_tmp7AC,sizeof(char),12)))),((_tmp7AA[0]=Cyc_PP_text(((_tmp7AD="__asm__",_tag_dyneither(_tmp7AD,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7AA,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1149
const char*_tmp7B2;const char*_tmp7B1;struct Cyc_PP_Doc*_tmp7B0[3];s=((_tmp7B0[2]=Cyc_PP_text(((_tmp7B1=")",_tag_dyneither(_tmp7B1,sizeof(char),2)))),((_tmp7B0[1]=Cyc_PP_text(_tmp280),((_tmp7B0[0]=Cyc_PP_text(((_tmp7B2="__asm__(",_tag_dyneither(_tmp7B2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7B0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp281->tag != 36)goto _LL1CA;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1C9:
# 1152
{const char*_tmp7B9;const char*_tmp7B8;const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[5];s=((_tmp7B6[4]=Cyc_PP_text(((_tmp7B7=")",_tag_dyneither(_tmp7B7,sizeof(char),2)))),((_tmp7B6[3]=Cyc_PP_textptr(_tmp283),((_tmp7B6[2]=Cyc_PP_text(((_tmp7B8=".",_tag_dyneither(_tmp7B8,sizeof(char),2)))),((_tmp7B6[1]=Cyc_Absynpp_exp2doc(_tmp282),((_tmp7B6[0]=Cyc_PP_text(((_tmp7B9="tagcheck(",_tag_dyneither(_tmp7B9,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp284->tag != 18)goto _LL1CC;else{_tmp285=(void*)_tmp284->f1;_tmp286=_tmp284->f2;}}_LL1CB:
# 1155
{const char*_tmp7BE;const char*_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[3];s=((_tmp7BC[2]=Cyc_PP_text(((_tmp7BD=",",_tag_dyneither(_tmp7BD,sizeof(char),2)))),((_tmp7BC[1]=Cyc_Absynpp_typ2doc(_tmp285),((_tmp7BC[0]=Cyc_PP_text(((_tmp7BE="offsetof(",_tag_dyneither(_tmp7BE,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BC,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp286 != 0;_tmp286=_tmp286->tl){
void*_tmp2F4=(void*)_tmp286->hd;struct _dyneither_ptr*_tmp2F6;unsigned int _tmp2F8;_LL1F4: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2F5=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2F4;if(_tmp2F5->tag != 0)goto _LL1F6;else{_tmp2F6=_tmp2F5->f1;}}_LL1F5:
{const char*_tmp7C1;struct Cyc_PP_Doc*_tmp7C0[3];s=((_tmp7C0[2]=
_tmp286->tl != 0?Cyc_PP_text(((_tmp7C1=".",_tag_dyneither(_tmp7C1,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7C0[1]=
# 1158
Cyc_PP_textptr(_tmp2F6),((_tmp7C0[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7C0,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1160
goto _LL1F3;_LL1F6: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2F7=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2F4;if(_tmp2F7->tag != 1)goto _LL1F3;else{_tmp2F8=_tmp2F7->f1;}}_LL1F7:
{struct Cyc_Int_pa_PrintArg_struct _tmp7CB;void*_tmp7CA[1];const char*_tmp7C9;const char*_tmp7C8;struct Cyc_PP_Doc*_tmp7C7[3];s=((_tmp7C7[2]=
_tmp286->tl != 0?Cyc_PP_text(((_tmp7C8=".",_tag_dyneither(_tmp7C8,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7C7[1]=
# 1161
Cyc_PP_text((struct _dyneither_ptr)((_tmp7CB.tag=1,((_tmp7CB.f1=(unsigned long)((int)_tmp2F8),((_tmp7CA[0]=& _tmp7CB,Cyc_aprintf(((_tmp7C9="%d",_tag_dyneither(_tmp7C9,sizeof(char),3))),_tag_dyneither(_tmp7CA,sizeof(void*),1))))))))),((_tmp7C7[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7C7,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1163
goto _LL1F3;_LL1F3:;}
# 1165
{const char*_tmp7CE;struct Cyc_PP_Doc*_tmp7CD[2];s=((_tmp7CD[1]=Cyc_PP_text(((_tmp7CE=")",_tag_dyneither(_tmp7CE,sizeof(char),2)))),((_tmp7CD[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7CD,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp287=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp287->tag != 19)goto _LL1CE;else{_tmp288=_tmp287->f1;}}_LL1CD:
# 1168
{const char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[2];s=((_tmp7D0[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),((_tmp7D0[0]=Cyc_PP_text(((_tmp7D1="*",_tag_dyneither(_tmp7D1,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1CE: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp289=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp289->tag != 20)goto _LL1D0;else{_tmp28A=_tmp289->f1;_tmp28B=_tmp289->f2;}}_LL1CF:
# 1171
{const char*_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[3];s=((_tmp7D3[2]=Cyc_PP_textptr(_tmp28B),((_tmp7D3[1]=Cyc_PP_text(((_tmp7D4=".",_tag_dyneither(_tmp7D4,sizeof(char),2)))),((_tmp7D3[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28A),Cyc_PP_cat(_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp28C=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28C->tag != 21)goto _LL1D2;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;}}_LL1D1:
# 1174
{const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[3];s=((_tmp7D6[2]=Cyc_PP_textptr(_tmp28E),((_tmp7D6[1]=Cyc_PP_text(((_tmp7D7="->",_tag_dyneither(_tmp7D7,sizeof(char),3)))),((_tmp7D6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),Cyc_PP_cat(_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D2: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28F->tag != 22)goto _LL1D4;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1D3:
# 1177
{const char*_tmp7DC;const char*_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[4];s=((_tmp7DA[3]=
# 1180
Cyc_PP_text(((_tmp7DB="]",_tag_dyneither(_tmp7DB,sizeof(char),2)))),((_tmp7DA[2]=
# 1179
Cyc_Absynpp_exp2doc(_tmp291),((_tmp7DA[1]=
# 1178
Cyc_PP_text(((_tmp7DC="[",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DA[0]=
# 1177
Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),Cyc_PP_cat(_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1181
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp292->tag != 23)goto _LL1D6;else{_tmp293=_tmp292->f1;}}_LL1D5:
# 1183
{const char*_tmp7E1;const char*_tmp7E0;struct Cyc_PP_Doc*_tmp7DF[4];s=((_tmp7DF[3]=
# 1186
Cyc_PP_text(((_tmp7E0=")",_tag_dyneither(_tmp7E0,sizeof(char),2)))),((_tmp7DF[2]=
# 1185
Cyc_Absynpp_exps2doc_prec(20,_tmp293),((_tmp7DF[1]=
# 1184
Cyc_PP_text(((_tmp7E1="(",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7DF[0]=
# 1183
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7DF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1187
goto _LL19F;_LL1D6: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp294->tag != 24)goto _LL1D8;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1D7:
# 1189
{const char*_tmp7E8;const char*_tmp7E7;const char*_tmp7E6;struct Cyc_PP_Doc*_tmp7E5[4];s=((_tmp7E5[3]=
# 1192
Cyc_Absynpp_group_braces(((_tmp7E6=",",_tag_dyneither(_tmp7E6,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp296)),((_tmp7E5[2]=
# 1191
Cyc_PP_text(((_tmp7E7=")",_tag_dyneither(_tmp7E7,sizeof(char),2)))),((_tmp7E5[1]=
# 1190
Cyc_Absynpp_typ2doc((*_tmp295).f3),((_tmp7E5[0]=
# 1189
Cyc_PP_text(((_tmp7E8="(",_tag_dyneither(_tmp7E8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1193
goto _LL19F;_LL1D8: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp297->tag != 25)goto _LL1DA;else{_tmp298=_tmp297->f1;}}_LL1D9:
# 1195
{const char*_tmp7E9;s=Cyc_Absynpp_group_braces(((_tmp7E9=",",_tag_dyneither(_tmp7E9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp298));}
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp299->tag != 26)goto _LL1DC;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;_tmp29C=_tmp299->f3;}}_LL1DB:
# 1198
{const char*_tmp7F0;const char*_tmp7EF;const char*_tmp7EE;struct Cyc_PP_Doc*_tmp7ED[8];s=((_tmp7ED[7]=
# 1204
Cyc_Absynpp_rb(),((_tmp7ED[6]=
# 1203
Cyc_Absynpp_exp2doc(_tmp29C),((_tmp7ED[5]=
# 1202
Cyc_PP_text(((_tmp7EE=" : ",_tag_dyneither(_tmp7EE,sizeof(char),4)))),((_tmp7ED[4]=
# 1201
Cyc_Absynpp_exp2doc(_tmp29B),((_tmp7ED[3]=
# 1200
Cyc_PP_text(((_tmp7EF=" < ",_tag_dyneither(_tmp7EF,sizeof(char),4)))),((_tmp7ED[2]=
# 1199
Cyc_PP_text(*(*_tmp29A->name).f2),((_tmp7ED[1]=
# 1198
Cyc_PP_text(((_tmp7F0="for ",_tag_dyneither(_tmp7F0,sizeof(char),5)))),((_tmp7ED[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1205
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp29D=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp29D->tag != 27)goto _LL1DE;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;_tmp2A0=_tmp29D->f3;}}_LL1DD: {
# 1207
struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A0);
{const char*_tmp7F5;struct Cyc_List_List*_tmp7F4;struct Cyc_PP_Doc*_tmp7F3[2];s=((_tmp7F3[1]=
Cyc_Absynpp_group_braces(((_tmp7F5=",",_tag_dyneither(_tmp7F5,sizeof(char),2))),
_tmp29F != 0?(_tmp7F4=_cycalloc(sizeof(*_tmp7F4)),((_tmp7F4->hd=Cyc_Absynpp_tps2doc(_tmp29F),((_tmp7F4->tl=_tmp317,_tmp7F4))))): _tmp317),((_tmp7F3[0]=
# 1208
Cyc_Absynpp_qvar2doc(_tmp29E),Cyc_PP_cat(_tag_dyneither(_tmp7F3,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1211
goto _LL19F;}_LL1DE: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A1=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A1->tag != 28)goto _LL1E0;else{_tmp2A2=_tmp2A1->f2;}}_LL1DF:
# 1213
{const char*_tmp7F6;s=Cyc_Absynpp_group_braces(((_tmp7F6=",",_tag_dyneither(_tmp7F6,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A2));}
goto _LL19F;_LL1E0: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A3=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A3->tag != 29)goto _LL1E2;else{_tmp2A4=_tmp2A3->f1;_tmp2A5=_tmp2A3->f3;}}_LL1E1:
# 1216
 if(_tmp2A4 == 0)
# 1218
s=Cyc_Absynpp_qvar2doc(_tmp2A5->name);else{
# 1220
const char*_tmp7FD;const char*_tmp7FC;const char*_tmp7FB;struct Cyc_PP_Doc*_tmp7FA[2];s=((_tmp7FA[1]=
Cyc_PP_egroup(((_tmp7FD="(",_tag_dyneither(_tmp7FD,sizeof(char),2))),((_tmp7FC=")",_tag_dyneither(_tmp7FC,sizeof(char),2))),((_tmp7FB=",",_tag_dyneither(_tmp7FB,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A4)),((_tmp7FA[0]=
# 1220
Cyc_Absynpp_qvar2doc(_tmp2A5->name),Cyc_PP_cat(_tag_dyneither(_tmp7FA,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1222
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A6->tag != 30)goto _LL1E4;else{_tmp2A7=_tmp2A6->f1;}}_LL1E3:
# 1224
 s=Cyc_Absynpp_qvar2doc(_tmp2A7);
goto _LL19F;_LL1E4: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A8->tag != 31)goto _LL1E6;else{_tmp2A9=_tmp2A8->f1;}}_LL1E5:
# 1227
 s=Cyc_Absynpp_qvar2doc(_tmp2A9);
goto _LL19F;_LL1E6: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2AA->tag != 32)goto _LL1E8;else{_tmp2AB=(_tmp2AA->f1).is_calloc;_tmp2AC=(_tmp2AA->f1).rgn;_tmp2AD=(_tmp2AA->f1).elt_type;_tmp2AE=(_tmp2AA->f1).num_elts;}}_LL1E7:
# 1232
 if(_tmp2AB){
# 1234
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2AD)),0);
if(_tmp2AC == 0){
const char*_tmp804;const char*_tmp803;const char*_tmp802;struct Cyc_PP_Doc*_tmp801[5];s=((_tmp801[4]=Cyc_PP_text(((_tmp802=")",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp801[3]=Cyc_Absynpp_exp2doc(st),((_tmp801[2]=Cyc_PP_text(((_tmp803=",",_tag_dyneither(_tmp803,sizeof(char),2)))),((_tmp801[1]=Cyc_Absynpp_exp2doc(_tmp2AE),((_tmp801[0]=Cyc_PP_text(((_tmp804="calloc(",_tag_dyneither(_tmp804,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp801,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1238
const char*_tmp80D;const char*_tmp80C;const char*_tmp80B;const char*_tmp80A;struct Cyc_PP_Doc*_tmp809[7];s=((_tmp809[6]=
Cyc_PP_text(((_tmp80A=")",_tag_dyneither(_tmp80A,sizeof(char),2)))),((_tmp809[5]=Cyc_Absynpp_exp2doc(st),((_tmp809[4]=Cyc_PP_text(((_tmp80B=",",_tag_dyneither(_tmp80B,sizeof(char),2)))),((_tmp809[3]=Cyc_Absynpp_exp2doc(_tmp2AE),((_tmp809[2]=
# 1238
Cyc_PP_text(((_tmp80C=",",_tag_dyneither(_tmp80C,sizeof(char),2)))),((_tmp809[1]=Cyc_Absynpp_exp2doc(_tmp2AC),((_tmp809[0]=Cyc_PP_text(((_tmp80D="rcalloc(",_tag_dyneither(_tmp80D,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp809,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1241
struct Cyc_Absyn_Exp*new_e;
# 1243
if(_tmp2AD == 0)
new_e=_tmp2AE;else{
# 1246
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2AD,0),_tmp2AE,0);}
# 1248
if(_tmp2AC == 0){
const char*_tmp812;const char*_tmp811;struct Cyc_PP_Doc*_tmp810[3];s=((_tmp810[2]=Cyc_PP_text(((_tmp811=")",_tag_dyneither(_tmp811,sizeof(char),2)))),((_tmp810[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp810[0]=Cyc_PP_text(((_tmp812="malloc(",_tag_dyneither(_tmp812,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp810,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1251
const char*_tmp819;const char*_tmp818;const char*_tmp817;struct Cyc_PP_Doc*_tmp816[5];s=((_tmp816[4]=
Cyc_PP_text(((_tmp817=")",_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp816[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp816[2]=
# 1251
Cyc_PP_text(((_tmp818=",",_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp816[1]=Cyc_Absynpp_exp2doc(_tmp2AC),((_tmp816[0]=Cyc_PP_text(((_tmp819="rmalloc(",_tag_dyneither(_tmp819,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1254
goto _LL19F;_LL1E8: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2AF=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2AF->tag != 33)goto _LL1EA;else{_tmp2B0=_tmp2AF->f1;_tmp2B1=_tmp2AF->f2;}}_LL1E9:
# 1256
{const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[3];s=((_tmp81B[2]=
# 1258
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1),((_tmp81B[1]=
# 1257
Cyc_PP_text(((_tmp81C=" :=: ",_tag_dyneither(_tmp81C,sizeof(char),6)))),((_tmp81B[0]=
# 1256
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0),Cyc_PP_cat(_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1259
goto _LL19F;_LL1EA: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B2=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B2->tag != 34)goto _LL1EC;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;}}_LL1EB:
# 1262
{const char*_tmp81D;s=Cyc_Absynpp_group_braces(((_tmp81D=",",_tag_dyneither(_tmp81D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B4));}
goto _LL19F;_LL1EC: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B5->tag != 35)goto _LL19F;else{_tmp2B6=_tmp2B5->f1;}}_LL1ED:
# 1265
{const char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*_tmp820[7];s=((_tmp820[6]=
# 1267
Cyc_PP_text(((_tmp821=")",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[5]=Cyc_Absynpp_rb(),((_tmp820[4]=Cyc_PP_blank_doc(),((_tmp820[3]=
# 1266
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B6,1)),((_tmp820[2]=
# 1265
Cyc_PP_blank_doc(),((_tmp820[1]=Cyc_Absynpp_lb(),((_tmp820[0]=Cyc_PP_text(((_tmp822="(",_tag_dyneither(_tmp822,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1268
goto _LL19F;_LL19F:;}
# 1270
if(inprec >= myprec){
const char*_tmp827;const char*_tmp826;struct Cyc_PP_Doc*_tmp825[3];s=((_tmp825[2]=Cyc_PP_text(((_tmp826=")",_tag_dyneither(_tmp826,sizeof(char),2)))),((_tmp825[1]=s,((_tmp825[0]=Cyc_PP_text(((_tmp827="(",_tag_dyneither(_tmp827,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1275
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp339=d;struct Cyc_Absyn_Exp*_tmp33B;struct _dyneither_ptr*_tmp33D;_LL1F9: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33A=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp339;if(_tmp33A->tag != 0)goto _LL1FB;else{_tmp33B=_tmp33A->f1;}}_LL1FA: {
const char*_tmp82C;const char*_tmp82B;struct Cyc_PP_Doc*_tmp82A[3];return(_tmp82A[2]=Cyc_PP_text(((_tmp82B="]",_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp82A[1]=Cyc_Absynpp_exp2doc(_tmp33B),((_tmp82A[0]=Cyc_PP_text(((_tmp82C=".[",_tag_dyneither(_tmp82C,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FB: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp339;if(_tmp33C->tag != 1)goto _LL1F8;else{_tmp33D=_tmp33C->f1;}}_LL1FC: {
const char*_tmp82F;struct Cyc_PP_Doc*_tmp82E[2];return(_tmp82E[1]=Cyc_PP_textptr(_tmp33D),((_tmp82E[0]=Cyc_PP_text(((_tmp82F=".",_tag_dyneither(_tmp82F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82E,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F8:;}
# 1282
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp836;const char*_tmp835;const char*_tmp834;struct Cyc_PP_Doc*_tmp833[2];return(_tmp833[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp833[0]=
# 1284
Cyc_PP_egroup(((_tmp836="",_tag_dyneither(_tmp836,sizeof(char),1))),((_tmp835="=",_tag_dyneither(_tmp835,sizeof(char),2))),((_tmp834="=",_tag_dyneither(_tmp834,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp833,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1288
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp839;const char*_tmp838;const char*_tmp837;return Cyc_PP_group(((_tmp837="",_tag_dyneither(_tmp837,sizeof(char),1))),((_tmp838="",_tag_dyneither(_tmp838,sizeof(char),1))),((_tmp839=",",_tag_dyneither(_tmp839,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1293
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp34A=c;enum Cyc_Absyn_Sign _tmp34B;char _tmp34C;struct _dyneither_ptr _tmp34D;enum Cyc_Absyn_Sign _tmp34E;short _tmp34F;int _tmp350;int _tmp351;int _tmp352;enum Cyc_Absyn_Sign _tmp353;long long _tmp354;struct _dyneither_ptr _tmp355;struct _dyneither_ptr _tmp356;struct _dyneither_ptr _tmp357;_LL1FE: if((_tmp34A.Char_c).tag != 2)goto _LL200;_tmp34B=((struct _tuple3)(_tmp34A.Char_c).val).f1;_tmp34C=((struct _tuple3)(_tmp34A.Char_c).val).f2;_LL1FF: {
const char*_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_PrintArg_struct _tmp83B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34C)),((_tmp83C[0]=& _tmp83B,Cyc_aprintf(((_tmp83D="'%s'",_tag_dyneither(_tmp83D,sizeof(char),5))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))))));}_LL200: if((_tmp34A.Wchar_c).tag != 3)goto _LL202;_tmp34D=(struct _dyneither_ptr)(_tmp34A.Wchar_c).val;_LL201: {
const char*_tmp841;void*_tmp840[1];struct Cyc_String_pa_PrintArg_struct _tmp83F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83F.tag=0,((_tmp83F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34D),((_tmp840[0]=& _tmp83F,Cyc_aprintf(((_tmp841="L'%s'",_tag_dyneither(_tmp841,sizeof(char),6))),_tag_dyneither(_tmp840,sizeof(void*),1)))))))));}_LL202: if((_tmp34A.Short_c).tag != 4)goto _LL204;_tmp34E=((struct _tuple4)(_tmp34A.Short_c).val).f1;_tmp34F=((struct _tuple4)(_tmp34A.Short_c).val).f2;_LL203: {
const char*_tmp845;void*_tmp844[1];struct Cyc_Int_pa_PrintArg_struct _tmp843;return Cyc_PP_text((struct _dyneither_ptr)((_tmp843.tag=1,((_tmp843.f1=(unsigned long)((int)_tmp34F),((_tmp844[0]=& _tmp843,Cyc_aprintf(((_tmp845="%d",_tag_dyneither(_tmp845,sizeof(char),3))),_tag_dyneither(_tmp844,sizeof(void*),1)))))))));}_LL204: if((_tmp34A.Int_c).tag != 5)goto _LL206;if(((struct _tuple5)(_tmp34A.Int_c).val).f1 != Cyc_Absyn_None)goto _LL206;_tmp350=((struct _tuple5)(_tmp34A.Int_c).val).f2;_LL205:
 _tmp351=_tmp350;goto _LL207;_LL206: if((_tmp34A.Int_c).tag != 5)goto _LL208;if(((struct _tuple5)(_tmp34A.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL208;_tmp351=((struct _tuple5)(_tmp34A.Int_c).val).f2;_LL207: {
const char*_tmp849;void*_tmp848[1];struct Cyc_Int_pa_PrintArg_struct _tmp847;return Cyc_PP_text((struct _dyneither_ptr)((_tmp847.tag=1,((_tmp847.f1=(unsigned long)_tmp351,((_tmp848[0]=& _tmp847,Cyc_aprintf(((_tmp849="%d",_tag_dyneither(_tmp849,sizeof(char),3))),_tag_dyneither(_tmp848,sizeof(void*),1)))))))));}_LL208: if((_tmp34A.Int_c).tag != 5)goto _LL20A;if(((struct _tuple5)(_tmp34A.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL20A;_tmp352=((struct _tuple5)(_tmp34A.Int_c).val).f2;_LL209: {
const char*_tmp84D;void*_tmp84C[1];struct Cyc_Int_pa_PrintArg_struct _tmp84B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84B.tag=1,((_tmp84B.f1=(unsigned int)_tmp352,((_tmp84C[0]=& _tmp84B,Cyc_aprintf(((_tmp84D="%u",_tag_dyneither(_tmp84D,sizeof(char),3))),_tag_dyneither(_tmp84C,sizeof(void*),1)))))))));}_LL20A: if((_tmp34A.LongLong_c).tag != 6)goto _LL20C;_tmp353=((struct _tuple6)(_tmp34A.LongLong_c).val).f1;_tmp354=((struct _tuple6)(_tmp34A.LongLong_c).val).f2;_LL20B: {
# 1302
const char*_tmp84E;return Cyc_PP_text(((_tmp84E="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84E,sizeof(char),27))));}_LL20C: if((_tmp34A.Float_c).tag != 7)goto _LL20E;_tmp355=((struct _tuple7)(_tmp34A.Float_c).val).f1;_LL20D:
 return Cyc_PP_text(_tmp355);_LL20E: if((_tmp34A.Null_c).tag != 1)goto _LL210;_LL20F: {
const char*_tmp84F;return Cyc_PP_text(((_tmp84F="NULL",_tag_dyneither(_tmp84F,sizeof(char),5))));}_LL210: if((_tmp34A.String_c).tag != 8)goto _LL212;_tmp356=(struct _dyneither_ptr)(_tmp34A.String_c).val;_LL211: {
# 1306
const char*_tmp854;const char*_tmp853;struct Cyc_PP_Doc*_tmp852[3];return(_tmp852[2]=Cyc_PP_text(((_tmp853="\"",_tag_dyneither(_tmp853,sizeof(char),2)))),((_tmp852[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp356)),((_tmp852[0]=Cyc_PP_text(((_tmp854="\"",_tag_dyneither(_tmp854,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp852,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL212: if((_tmp34A.Wstring_c).tag != 9)goto _LL1FD;_tmp357=(struct _dyneither_ptr)(_tmp34A.Wstring_c).val;_LL213: {
# 1308
const char*_tmp859;const char*_tmp858;struct Cyc_PP_Doc*_tmp857[3];return(_tmp857[2]=Cyc_PP_text(((_tmp858="\"",_tag_dyneither(_tmp858,sizeof(char),2)))),((_tmp857[1]=Cyc_PP_text(_tmp357),((_tmp857[0]=Cyc_PP_text(((_tmp859="L\"",_tag_dyneither(_tmp859,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp857,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FD:;}
# 1312
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp866;const char*_tmp865;void*_tmp864[1];struct Cyc_String_pa_PrintArg_struct _tmp863;struct Cyc_Core_Failure_exn_struct*_tmp862;(int)_throw((void*)((_tmp862=_cycalloc(sizeof(*_tmp862)),((_tmp862[0]=((_tmp866.tag=Cyc_Core_Failure,((_tmp866.f1=(struct _dyneither_ptr)((_tmp863.tag=0,((_tmp863.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp864[0]=& _tmp863,Cyc_aprintf(((_tmp865="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp865,sizeof(char),47))),_tag_dyneither(_tmp864,sizeof(void*),1)))))))),_tmp866)))),_tmp862)))));}{
# 1318
const char*_tmp86B;const char*_tmp86A;struct Cyc_PP_Doc*_tmp869[3];return(_tmp869[2]=Cyc_PP_text(((_tmp86A=")",_tag_dyneither(_tmp86A,sizeof(char),2)))),((_tmp869[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp869[0]=Cyc_PP_text(((_tmp86B="numelts(",_tag_dyneither(_tmp86B,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp869,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1320
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp878;const char*_tmp877;void*_tmp876[1];struct Cyc_String_pa_PrintArg_struct _tmp875;struct Cyc_Core_Failure_exn_struct*_tmp874;(int)_throw((void*)((_tmp874=_cycalloc(sizeof(*_tmp874)),((_tmp874[0]=((_tmp878.tag=Cyc_Core_Failure,((_tmp878.f1=(struct _dyneither_ptr)((_tmp875.tag=0,((_tmp875.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp876[0]=& _tmp875,Cyc_aprintf(((_tmp877="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp877,sizeof(char),38))),_tag_dyneither(_tmp876,sizeof(void*),1)))))))),_tmp878)))),_tmp874)))));}else{
# 1324
if(ds->tl == 0){
const char*_tmp87B;struct Cyc_PP_Doc*_tmp87A[3];return(_tmp87A[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp87A[1]=Cyc_PP_text(((_tmp87B=" ",_tag_dyneither(_tmp87B,sizeof(char),2)))),((_tmp87A[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp87A,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp888;const char*_tmp887;void*_tmp886[1];struct Cyc_String_pa_PrintArg_struct _tmp885;struct Cyc_Core_Failure_exn_struct*_tmp884;(int)_throw((void*)((_tmp884=_cycalloc(sizeof(*_tmp884)),((_tmp884[0]=((_tmp888.tag=Cyc_Core_Failure,((_tmp888.f1=(struct _dyneither_ptr)((_tmp885.tag=0,((_tmp885.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp886[0]=& _tmp885,Cyc_aprintf(((_tmp887="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp887,sizeof(char),47))),_tag_dyneither(_tmp886,sizeof(void*),1)))))))),_tmp888)))),_tmp884)))));}else{
# 1330
const char*_tmp88D;const char*_tmp88C;struct Cyc_PP_Doc*_tmp88B[5];return(_tmp88B[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp88B[3]=Cyc_PP_text(((_tmp88C=" ",_tag_dyneither(_tmp88C,sizeof(char),2)))),((_tmp88B[2]=ps,((_tmp88B[1]=Cyc_PP_text(((_tmp88D=" ",_tag_dyneither(_tmp88D,sizeof(char),2)))),((_tmp88B[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp88B,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1334
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL214: {
const char*_tmp88E;return(_tmp88E="+",_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Times: _LL215: {
const char*_tmp88F;return(_tmp88F="*",_tag_dyneither(_tmp88F,sizeof(char),2));}case Cyc_Absyn_Minus: _LL216: {
const char*_tmp890;return(_tmp890="-",_tag_dyneither(_tmp890,sizeof(char),2));}case Cyc_Absyn_Div: _LL217: {
const char*_tmp891;return(_tmp891="/",_tag_dyneither(_tmp891,sizeof(char),2));}case Cyc_Absyn_Mod: _LL218: {
const char*_tmp893;const char*_tmp892;return Cyc_Absynpp_print_for_cycdoc?(_tmp893="\\%",_tag_dyneither(_tmp893,sizeof(char),3)):((_tmp892="%",_tag_dyneither(_tmp892,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL219: {
const char*_tmp894;return(_tmp894="==",_tag_dyneither(_tmp894,sizeof(char),3));}case Cyc_Absyn_Neq: _LL21A: {
const char*_tmp895;return(_tmp895="!=",_tag_dyneither(_tmp895,sizeof(char),3));}case Cyc_Absyn_Gt: _LL21B: {
const char*_tmp896;return(_tmp896=">",_tag_dyneither(_tmp896,sizeof(char),2));}case Cyc_Absyn_Lt: _LL21C: {
const char*_tmp897;return(_tmp897="<",_tag_dyneither(_tmp897,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21D: {
const char*_tmp898;return(_tmp898=">=",_tag_dyneither(_tmp898,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21E: {
const char*_tmp899;return(_tmp899="<=",_tag_dyneither(_tmp899,sizeof(char),3));}case Cyc_Absyn_Not: _LL21F: {
const char*_tmp89A;return(_tmp89A="!",_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL220: {
const char*_tmp89B;return(_tmp89B="~",_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL221: {
const char*_tmp89C;return(_tmp89C="&",_tag_dyneither(_tmp89C,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL222: {
const char*_tmp89D;return(_tmp89D="|",_tag_dyneither(_tmp89D,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL223: {
const char*_tmp89E;return(_tmp89E="^",_tag_dyneither(_tmp89E,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL224: {
const char*_tmp89F;return(_tmp89F="<<",_tag_dyneither(_tmp89F,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL225: {
const char*_tmp8A0;return(_tmp8A0=">>",_tag_dyneither(_tmp8A0,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL226: {
const char*_tmp8A1;return(_tmp8A1=">>>",_tag_dyneither(_tmp8A1,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL227: {
const char*_tmp8A2;return(_tmp8A2="numelts",_tag_dyneither(_tmp8A2,sizeof(char),8));}}}
# 1359
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1363
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp39B=s->r;_LL22A: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp39C=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39B;if(_tmp39C->tag != 12)goto _LL22C;}_LL22B:
 return 1;_LL22C:;_LL22D:
 return 0;_LL229:;}
# 1370
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp39D=st->r;struct Cyc_Absyn_Exp*_tmp3A0;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Stmt*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Stmt*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3AB;struct Cyc_Absyn_Stmt*_tmp3AC;struct _dyneither_ptr*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_Absyn_Stmt*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Decl*_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3BE;struct _dyneither_ptr*_tmp3C0;struct Cyc_Absyn_Stmt*_tmp3C1;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C6;struct Cyc_List_List*_tmp3C7;struct Cyc_Absyn_Exp*_tmp3C9;_LL22F: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp39E=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp39E->tag != 0)goto _LL231;}_LL230:
{const char*_tmp8A3;s=Cyc_PP_text(((_tmp8A3=";",_tag_dyneither(_tmp8A3,sizeof(char),2))));}goto _LL22E;_LL231: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp39F=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp39F->tag != 1)goto _LL233;else{_tmp3A0=_tmp39F->f1;}}_LL232:
{const char*_tmp8A6;struct Cyc_PP_Doc*_tmp8A5[2];s=((_tmp8A5[1]=Cyc_PP_text(((_tmp8A6=";",_tag_dyneither(_tmp8A6,sizeof(char),2)))),((_tmp8A5[0]=Cyc_Absynpp_exp2doc(_tmp3A0),Cyc_PP_cat(_tag_dyneither(_tmp8A5,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22E;_LL233: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3A1=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3A1->tag != 2)goto _LL235;else{_tmp3A2=_tmp3A1->f1;_tmp3A3=_tmp3A1->f2;}}_LL234:
# 1376
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3A2)){
struct Cyc_PP_Doc*_tmp8B1[5];struct Cyc_PP_Doc*_tmp8B0[7];const char*_tmp8AF;const char*_tmp8AE;struct Cyc_PP_Doc*_tmp8AD[7];s=((_tmp8AD[6]=
# 1384
Cyc_Absynpp_is_declaration(_tmp3A3)?
stmtexp?
(_tmp8B0[6]=
# 1389
Cyc_PP_line_doc(),((_tmp8B0[5]=
# 1388
Cyc_PP_text(((_tmp8AF=");",_tag_dyneither(_tmp8AF,sizeof(char),3)))),((_tmp8B0[4]=Cyc_Absynpp_rb(),((_tmp8B0[3]=
# 1387
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp)),((_tmp8B0[2]=
# 1386
Cyc_PP_blank_doc(),((_tmp8B0[1]=Cyc_Absynpp_lb(),((_tmp8B0[0]=Cyc_PP_text(((_tmp8AE="(",_tag_dyneither(_tmp8AE,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8B0,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1390
(_tmp8B1[4]=
# 1393
Cyc_PP_line_doc(),((_tmp8B1[3]=
# 1392
Cyc_Absynpp_rb(),((_tmp8B1[2]=
# 1391
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp)),((_tmp8B1[1]=
# 1390
Cyc_PP_blank_doc(),((_tmp8B1[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1394
 Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp),((_tmp8AD[5]=
# 1383
Cyc_PP_line_doc(),((_tmp8AD[4]=
# 1382
Cyc_Absynpp_rb(),((_tmp8AD[3]=
# 1381
Cyc_PP_line_doc(),((_tmp8AD[2]=
# 1380
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,0)),((_tmp8AD[1]=
# 1379
Cyc_PP_blank_doc(),((_tmp8AD[0]=
# 1378
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8AD,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1395
if(Cyc_Absynpp_is_declaration(_tmp3A3)){
struct Cyc_PP_Doc*_tmp8BC[4];struct Cyc_PP_Doc*_tmp8BB[6];const char*_tmp8BA;const char*_tmp8B9;struct Cyc_PP_Doc*_tmp8B8[4];s=((_tmp8B8[3]=
# 1406
Cyc_PP_line_doc(),((_tmp8B8[2]=
# 1398
stmtexp?
(_tmp8BB[5]=
# 1401
Cyc_PP_text(((_tmp8BA=");",_tag_dyneither(_tmp8BA,sizeof(char),3)))),((_tmp8BB[4]=Cyc_Absynpp_rb(),((_tmp8BB[3]=
# 1400
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp)),((_tmp8BB[2]=
# 1399
Cyc_PP_blank_doc(),((_tmp8BB[1]=Cyc_Absynpp_lb(),((_tmp8BB[0]=Cyc_PP_text(((_tmp8B9="(",_tag_dyneither(_tmp8B9,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1403
(_tmp8BC[3]=
# 1405
Cyc_Absynpp_rb(),((_tmp8BC[2]=
# 1404
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp)),((_tmp8BC[1]=
# 1403
Cyc_PP_blank_doc(),((_tmp8BC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8B8[1]=
# 1397
Cyc_PP_line_doc(),((_tmp8B8[0]=
# 1396
Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(_tag_dyneither(_tmp8B8,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1408
struct Cyc_PP_Doc*_tmp8BD[3];s=((_tmp8BD[2]=Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp),((_tmp8BD[1]=Cyc_PP_line_doc(),((_tmp8BD[0]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(_tag_dyneither(_tmp8BD,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1411
struct Cyc_PP_Doc*_tmp8BE[3];s=((_tmp8BE[2]=Cyc_Absynpp_stmt2doc(_tmp3A3,stmtexp),((_tmp8BE[1]=Cyc_PP_line_doc(),((_tmp8BE[0]=Cyc_Absynpp_stmt2doc(_tmp3A2,0),Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22E;_LL235: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A4=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3A4->tag != 3)goto _LL237;else{_tmp3A5=_tmp3A4->f1;}}_LL236:
# 1414
 if(_tmp3A5 == 0){
const char*_tmp8BF;s=Cyc_PP_text(((_tmp8BF="return;",_tag_dyneither(_tmp8BF,sizeof(char),8))));}else{
# 1417
const char*_tmp8C4;const char*_tmp8C3;struct Cyc_PP_Doc*_tmp8C2[3];s=((_tmp8C2[2]=
# 1419
Cyc_PP_text(((_tmp8C3=";",_tag_dyneither(_tmp8C3,sizeof(char),2)))),((_tmp8C2[1]=
# 1418
_tmp3A5 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3A5),((_tmp8C2[0]=
# 1417
Cyc_PP_text(((_tmp8C4="return ",_tag_dyneither(_tmp8C4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8C2,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1420
goto _LL22E;_LL237: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3A6=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3A6->tag != 4)goto _LL239;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;_tmp3A9=_tmp3A6->f3;}}_LL238: {
# 1422
int print_else;
{void*_tmp3DD=_tmp3A9->r;_LL254: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3DE=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3DD;if(_tmp3DE->tag != 0)goto _LL256;}_LL255:
 print_else=0;goto _LL253;_LL256:;_LL257:
 print_else=1;goto _LL253;_LL253:;}
# 1427
{const char*_tmp8D3;const char*_tmp8D2;struct Cyc_PP_Doc*_tmp8D1[2];struct Cyc_PP_Doc*_tmp8D0[6];struct Cyc_PP_Doc*_tmp8CF[2];const char*_tmp8CE;struct Cyc_PP_Doc*_tmp8CD[8];s=((_tmp8CD[7]=
# 1434
print_else?
(_tmp8D0[5]=
# 1440
Cyc_Absynpp_rb(),((_tmp8D0[4]=
# 1439
Cyc_PP_line_doc(),((_tmp8D0[3]=
# 1438
Cyc_PP_nest(2,((_tmp8CF[1]=Cyc_Absynpp_stmt2doc(_tmp3A9,0),((_tmp8CF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D0[2]=
# 1437
Cyc_Absynpp_lb(),((_tmp8D0[1]=
# 1436
Cyc_PP_text(((_tmp8CE="else ",_tag_dyneither(_tmp8CE,sizeof(char),6)))),((_tmp8D0[0]=
# 1435
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D0,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1441
 Cyc_PP_nil_doc(),((_tmp8CD[6]=
# 1433
Cyc_Absynpp_rb(),((_tmp8CD[5]=
# 1432
Cyc_PP_line_doc(),((_tmp8CD[4]=
# 1431
Cyc_PP_nest(2,((_tmp8D1[1]=Cyc_Absynpp_stmt2doc(_tmp3A8,0),((_tmp8D1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CD[3]=
# 1430
Cyc_Absynpp_lb(),((_tmp8CD[2]=
# 1429
Cyc_PP_text(((_tmp8D2=") ",_tag_dyneither(_tmp8D2,sizeof(char),3)))),((_tmp8CD[1]=
# 1428
Cyc_Absynpp_exp2doc(_tmp3A7),((_tmp8CD[0]=
# 1427
Cyc_PP_text(((_tmp8D3="if (",_tag_dyneither(_tmp8D3,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CD,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1442
goto _LL22E;}_LL239: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3AA=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3AA->tag != 5)goto _LL23B;else{_tmp3AB=(_tmp3AA->f1).f1;_tmp3AC=_tmp3AA->f2;}}_LL23A:
# 1444
{const char*_tmp8DA;const char*_tmp8D9;struct Cyc_PP_Doc*_tmp8D8[2];struct Cyc_PP_Doc*_tmp8D7[7];s=((_tmp8D7[6]=
# 1450
Cyc_Absynpp_rb(),((_tmp8D7[5]=
# 1449
Cyc_PP_line_doc(),((_tmp8D7[4]=
# 1448
Cyc_PP_nest(2,((_tmp8D8[1]=Cyc_Absynpp_stmt2doc(_tmp3AC,0),((_tmp8D8[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D8,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D7[3]=
# 1447
Cyc_Absynpp_lb(),((_tmp8D7[2]=
# 1446
Cyc_PP_text(((_tmp8D9=") ",_tag_dyneither(_tmp8D9,sizeof(char),3)))),((_tmp8D7[1]=
# 1445
Cyc_Absynpp_exp2doc(_tmp3AB),((_tmp8D7[0]=
# 1444
Cyc_PP_text(((_tmp8DA="while (",_tag_dyneither(_tmp8DA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1451
goto _LL22E;_LL23B: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3AD=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3AD->tag != 6)goto _LL23D;}_LL23C:
{const char*_tmp8DB;s=Cyc_PP_text(((_tmp8DB="break;",_tag_dyneither(_tmp8DB,sizeof(char),7))));}goto _LL22E;_LL23D: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3AE=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3AE->tag != 7)goto _LL23F;}_LL23E:
{const char*_tmp8DC;s=Cyc_PP_text(((_tmp8DC="continue;",_tag_dyneither(_tmp8DC,sizeof(char),10))));}goto _LL22E;_LL23F: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3AF=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3AF->tag != 8)goto _LL241;else{_tmp3B0=_tmp3AF->f1;}}_LL240:
{const char*_tmp8E0;void*_tmp8DF[1];struct Cyc_String_pa_PrintArg_struct _tmp8DE;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DE.tag=0,((_tmp8DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3B0),((_tmp8DF[0]=& _tmp8DE,Cyc_aprintf(((_tmp8E0="goto %s;",_tag_dyneither(_tmp8E0,sizeof(char),9))),_tag_dyneither(_tmp8DF,sizeof(void*),1)))))))));}goto _LL22E;_LL241: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3B1=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3B1->tag != 9)goto _LL243;else{_tmp3B2=_tmp3B1->f1;_tmp3B3=(_tmp3B1->f2).f1;_tmp3B4=(_tmp3B1->f3).f1;_tmp3B5=_tmp3B1->f4;}}_LL242:
# 1456
{const char*_tmp8EB;const char*_tmp8EA;const char*_tmp8E9;const char*_tmp8E8;struct Cyc_PP_Doc*_tmp8E7[2];struct Cyc_PP_Doc*_tmp8E6[11];s=((_tmp8E6[10]=
# 1466
Cyc_Absynpp_rb(),((_tmp8E6[9]=
# 1465
Cyc_PP_line_doc(),((_tmp8E6[8]=
# 1464
Cyc_PP_nest(2,((_tmp8E7[1]=Cyc_Absynpp_stmt2doc(_tmp3B5,0),((_tmp8E7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E6[7]=
# 1463
Cyc_Absynpp_lb(),((_tmp8E6[6]=
# 1462
Cyc_PP_text(((_tmp8E8=") ",_tag_dyneither(_tmp8E8,sizeof(char),3)))),((_tmp8E6[5]=
# 1461
Cyc_Absynpp_exp2doc(_tmp3B4),((_tmp8E6[4]=
# 1460
Cyc_PP_text(((_tmp8E9="; ",_tag_dyneither(_tmp8E9,sizeof(char),3)))),((_tmp8E6[3]=
# 1459
Cyc_Absynpp_exp2doc(_tmp3B3),((_tmp8E6[2]=
# 1458
Cyc_PP_text(((_tmp8EA="; ",_tag_dyneither(_tmp8EA,sizeof(char),3)))),((_tmp8E6[1]=
# 1457
Cyc_Absynpp_exp2doc(_tmp3B2),((_tmp8E6[0]=
# 1456
Cyc_PP_text(((_tmp8EB="for(",_tag_dyneither(_tmp8EB,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E6,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1467
goto _LL22E;_LL243: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3B6=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3B6->tag != 10)goto _LL245;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;}}_LL244:
# 1469
{const char*_tmp8F0;const char*_tmp8EF;struct Cyc_PP_Doc*_tmp8EE[8];s=((_tmp8EE[7]=
# 1476
Cyc_Absynpp_rb(),((_tmp8EE[6]=
# 1475
Cyc_PP_line_doc(),((_tmp8EE[5]=
# 1474
Cyc_Absynpp_switchclauses2doc(_tmp3B8),((_tmp8EE[4]=
# 1473
Cyc_PP_line_doc(),((_tmp8EE[3]=
# 1472
Cyc_Absynpp_lb(),((_tmp8EE[2]=
# 1471
Cyc_PP_text(((_tmp8EF=") ",_tag_dyneither(_tmp8EF,sizeof(char),3)))),((_tmp8EE[1]=
# 1470
Cyc_Absynpp_exp2doc(_tmp3B7),((_tmp8EE[0]=
# 1469
Cyc_PP_text(((_tmp8F0="switch (",_tag_dyneither(_tmp8F0,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8EE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1477
goto _LL22E;_LL245: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3B9->tag != 11)goto _LL247;else{if(_tmp3B9->f1 != 0)goto _LL247;}}_LL246:
{const char*_tmp8F1;s=Cyc_PP_text(((_tmp8F1="fallthru;",_tag_dyneither(_tmp8F1,sizeof(char),10))));}goto _LL22E;_LL247: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3BA=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3BA->tag != 11)goto _LL249;else{_tmp3BB=_tmp3BA->f1;}}_LL248:
# 1480
{const char*_tmp8F6;const char*_tmp8F5;struct Cyc_PP_Doc*_tmp8F4[3];s=((_tmp8F4[2]=Cyc_PP_text(((_tmp8F5=");",_tag_dyneither(_tmp8F5,sizeof(char),3)))),((_tmp8F4[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3BB),((_tmp8F4[0]=Cyc_PP_text(((_tmp8F6="fallthru(",_tag_dyneither(_tmp8F6,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22E;_LL249: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3BC=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3BC->tag != 12)goto _LL24B;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL24A:
# 1482
{struct Cyc_PP_Doc*_tmp8F7[3];s=((_tmp8F7[2]=
# 1484
Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8F7[1]=
# 1483
Cyc_PP_line_doc(),((_tmp8F7[0]=
# 1482
Cyc_Absynpp_decl2doc(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8F7,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1485
goto _LL22E;_LL24B: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3BF=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3BF->tag != 13)goto _LL24D;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;}}_LL24C:
# 1487
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C1)){
if(stmtexp){
const char*_tmp8FC;const char*_tmp8FB;struct Cyc_PP_Doc*_tmp8FA[8];s=((_tmp8FA[7]=
# 1495
Cyc_PP_text(((_tmp8FB=");",_tag_dyneither(_tmp8FB,sizeof(char),3)))),((_tmp8FA[6]=Cyc_Absynpp_rb(),((_tmp8FA[5]=
# 1494
Cyc_PP_line_doc(),((_tmp8FA[4]=
# 1493
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C1,1)),((_tmp8FA[3]=
# 1492
Cyc_PP_line_doc(),((_tmp8FA[2]=
# 1491
Cyc_Absynpp_lb(),((_tmp8FA[1]=
# 1490
Cyc_PP_text(((_tmp8FC=": (",_tag_dyneither(_tmp8FC,sizeof(char),4)))),((_tmp8FA[0]=
# 1489
Cyc_PP_textptr(_tmp3C0),Cyc_PP_cat(_tag_dyneither(_tmp8FA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1497
const char*_tmp8FF;struct Cyc_PP_Doc*_tmp8FE[7];s=((_tmp8FE[6]=
# 1503
Cyc_Absynpp_rb(),((_tmp8FE[5]=
# 1502
Cyc_PP_line_doc(),((_tmp8FE[4]=
# 1501
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C1,0)),((_tmp8FE[3]=
# 1500
Cyc_PP_line_doc(),((_tmp8FE[2]=
# 1499
Cyc_Absynpp_lb(),((_tmp8FE[1]=
# 1498
Cyc_PP_text(((_tmp8FF=": ",_tag_dyneither(_tmp8FF,sizeof(char),3)))),((_tmp8FE[0]=
# 1497
Cyc_PP_textptr(_tmp3C0),Cyc_PP_cat(_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1505
const char*_tmp902;struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=Cyc_Absynpp_stmt2doc(_tmp3C1,stmtexp),((_tmp901[1]=Cyc_PP_text(((_tmp902=": ",_tag_dyneither(_tmp902,sizeof(char),3)))),((_tmp901[0]=Cyc_PP_textptr(_tmp3C0),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22E;_LL24D: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3C2=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3C2->tag != 14)goto _LL24F;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=(_tmp3C2->f2).f1;}}_LL24E:
# 1508
{const char*_tmp909;const char*_tmp908;const char*_tmp907;struct Cyc_PP_Doc*_tmp906[9];s=((_tmp906[8]=
# 1516
Cyc_PP_text(((_tmp907=");",_tag_dyneither(_tmp907,sizeof(char),3)))),((_tmp906[7]=
# 1515
Cyc_Absynpp_exp2doc(_tmp3C4),((_tmp906[6]=
# 1514
Cyc_PP_text(((_tmp908=" while (",_tag_dyneither(_tmp908,sizeof(char),9)))),((_tmp906[5]=
# 1513
Cyc_Absynpp_rb(),((_tmp906[4]=
# 1512
Cyc_PP_line_doc(),((_tmp906[3]=
# 1511
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C3,0)),((_tmp906[2]=
# 1510
Cyc_PP_line_doc(),((_tmp906[1]=
# 1509
Cyc_Absynpp_lb(),((_tmp906[0]=
# 1508
Cyc_PP_text(((_tmp909="do ",_tag_dyneither(_tmp909,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp906,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1517
goto _LL22E;_LL24F: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3C5=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3C5->tag != 15)goto _LL251;else{_tmp3C6=_tmp3C5->f1;_tmp3C7=_tmp3C5->f2;}}_LL250:
# 1519
{const char*_tmp90E;const char*_tmp90D;struct Cyc_PP_Doc*_tmp90C[12];s=((_tmp90C[11]=
# 1530
Cyc_Absynpp_rb(),((_tmp90C[10]=
# 1529
Cyc_PP_line_doc(),((_tmp90C[9]=
# 1528
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3C7)),((_tmp90C[8]=
# 1527
Cyc_PP_line_doc(),((_tmp90C[7]=
# 1526
Cyc_Absynpp_lb(),((_tmp90C[6]=
# 1525
Cyc_PP_text(((_tmp90D=" catch ",_tag_dyneither(_tmp90D,sizeof(char),8)))),((_tmp90C[5]=
# 1524
Cyc_Absynpp_rb(),((_tmp90C[4]=
# 1523
Cyc_PP_line_doc(),((_tmp90C[3]=
# 1522
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C6,0)),((_tmp90C[2]=
# 1521
Cyc_PP_line_doc(),((_tmp90C[1]=
# 1520
Cyc_Absynpp_lb(),((_tmp90C[0]=
# 1519
Cyc_PP_text(((_tmp90E="try ",_tag_dyneither(_tmp90E,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90C,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1531
goto _LL22E;_LL251: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3C8=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp39D;if(_tmp3C8->tag != 16)goto _LL22E;else{_tmp3C9=_tmp3C8->f1;}}_LL252:
# 1533
{const char*_tmp913;const char*_tmp912;struct Cyc_PP_Doc*_tmp911[3];s=((_tmp911[2]=Cyc_PP_text(((_tmp912=");",_tag_dyneither(_tmp912,sizeof(char),3)))),((_tmp911[1]=Cyc_Absynpp_exp2doc(_tmp3C9),((_tmp911[0]=Cyc_PP_text(((_tmp913="reset_region(",_tag_dyneither(_tmp913,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp911,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22E;_LL22E:;}
# 1536
return s;}
# 1539
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp40E=p->r;enum Cyc_Absyn_Sign _tmp412;int _tmp413;char _tmp415;struct _dyneither_ptr _tmp417;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Vardecl*_tmp41C;struct Cyc_Absyn_Pat*_tmp41D;struct Cyc_Absyn_Tvar*_tmp41F;struct Cyc_Absyn_Vardecl*_tmp420;struct Cyc_List_List*_tmp422;int _tmp423;struct Cyc_Absyn_Pat*_tmp425;struct Cyc_Absyn_Vardecl*_tmp427;struct Cyc_Absyn_Vardecl*_tmp42A;struct Cyc_Absyn_Pat*_tmp42B;struct _tuple0*_tmp42D;struct _tuple0*_tmp42F;struct Cyc_List_List*_tmp430;int _tmp431;union Cyc_Absyn_AggrInfoU _tmp433;struct Cyc_List_List*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;struct Cyc_List_List*_tmp438;struct Cyc_List_List*_tmp439;int _tmp43A;struct Cyc_Absyn_Enumfield*_tmp43C;struct Cyc_Absyn_Enumfield*_tmp43E;struct Cyc_Absyn_Datatypefield*_tmp440;struct Cyc_Absyn_Datatypefield*_tmp442;struct Cyc_List_List*_tmp443;int _tmp444;struct Cyc_Absyn_Exp*_tmp446;_LL259: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp40F=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp40F->tag != 0)goto _LL25B;}_LL25A:
{const char*_tmp914;s=Cyc_PP_text(((_tmp914="_",_tag_dyneither(_tmp914,sizeof(char),2))));}goto _LL258;_LL25B: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp410=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp410->tag != 8)goto _LL25D;}_LL25C:
{const char*_tmp915;s=Cyc_PP_text(((_tmp915="NULL",_tag_dyneither(_tmp915,sizeof(char),5))));}goto _LL258;_LL25D: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp411=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp411->tag != 9)goto _LL25F;else{_tmp412=_tmp411->f1;_tmp413=_tmp411->f2;}}_LL25E:
# 1545
 if(_tmp412 != Cyc_Absyn_Unsigned){
const char*_tmp919;void*_tmp918[1];struct Cyc_Int_pa_PrintArg_struct _tmp917;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp917.tag=1,((_tmp917.f1=(unsigned long)_tmp413,((_tmp918[0]=& _tmp917,Cyc_aprintf(((_tmp919="%d",_tag_dyneither(_tmp919,sizeof(char),3))),_tag_dyneither(_tmp918,sizeof(void*),1)))))))));}else{
const char*_tmp91D;void*_tmp91C[1];struct Cyc_Int_pa_PrintArg_struct _tmp91B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91B.tag=1,((_tmp91B.f1=(unsigned int)_tmp413,((_tmp91C[0]=& _tmp91B,Cyc_aprintf(((_tmp91D="%u",_tag_dyneither(_tmp91D,sizeof(char),3))),_tag_dyneither(_tmp91C,sizeof(void*),1)))))))));}
goto _LL258;_LL25F: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp414=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp414->tag != 10)goto _LL261;else{_tmp415=_tmp414->f1;}}_LL260:
{const char*_tmp921;void*_tmp920[1];struct Cyc_String_pa_PrintArg_struct _tmp91F;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91F.tag=0,((_tmp91F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp415)),((_tmp920[0]=& _tmp91F,Cyc_aprintf(((_tmp921="'%s'",_tag_dyneither(_tmp921,sizeof(char),5))),_tag_dyneither(_tmp920,sizeof(void*),1)))))))));}goto _LL258;_LL261: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp416=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp416->tag != 11)goto _LL263;else{_tmp417=_tmp416->f1;}}_LL262:
 s=Cyc_PP_text(_tmp417);goto _LL258;_LL263: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp418=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp418->tag != 1)goto _LL265;else{_tmp419=_tmp418->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp41A=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp418->f2)->r;if(_tmp41A->tag != 0)goto _LL265;};}}_LL264:
# 1552
 s=Cyc_Absynpp_qvar2doc(_tmp419->name);goto _LL258;_LL265: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp41B->tag != 1)goto _LL267;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;}}_LL266:
# 1555
{const char*_tmp924;struct Cyc_PP_Doc*_tmp923[3];s=((_tmp923[2]=Cyc_Absynpp_pat2doc(_tmp41D),((_tmp923[1]=Cyc_PP_text(((_tmp924=" as ",_tag_dyneither(_tmp924,sizeof(char),5)))),((_tmp923[0]=Cyc_Absynpp_qvar2doc(_tmp41C->name),Cyc_PP_cat(_tag_dyneither(_tmp923,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL258;_LL267: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp41E=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp41E->tag != 3)goto _LL269;else{_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;}}_LL268:
# 1557
{const char*_tmp929;const char*_tmp928;struct Cyc_PP_Doc*_tmp927[4];s=((_tmp927[3]=Cyc_PP_text(((_tmp928=">",_tag_dyneither(_tmp928,sizeof(char),2)))),((_tmp927[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp41F)),((_tmp927[1]=Cyc_PP_text(((_tmp929="<",_tag_dyneither(_tmp929,sizeof(char),2)))),((_tmp927[0]=Cyc_Absynpp_qvar2doc(_tmp420->name),Cyc_PP_cat(_tag_dyneither(_tmp927,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL258;_LL269: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp421=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp421->tag != 4)goto _LL26B;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;}}_LL26A:
# 1560
{const char*_tmp932;const char*_tmp931;const char*_tmp930;const char*_tmp92F;struct Cyc_PP_Doc*_tmp92E[4];s=((_tmp92E[3]=
_tmp423?Cyc_PP_text(((_tmp92F=", ...)",_tag_dyneither(_tmp92F,sizeof(char),7)))): Cyc_PP_text(((_tmp930=")",_tag_dyneither(_tmp930,sizeof(char),2)))),((_tmp92E[2]=
# 1560
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp931=",",_tag_dyneither(_tmp931,sizeof(char),2))),_tmp422),((_tmp92E[1]=Cyc_PP_text(((_tmp932="(",_tag_dyneither(_tmp932,sizeof(char),2)))),((_tmp92E[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp92E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1562
goto _LL258;_LL26B: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp424=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp424->tag != 5)goto _LL26D;else{_tmp425=_tmp424->f1;}}_LL26C:
# 1564
{const char*_tmp935;struct Cyc_PP_Doc*_tmp934[2];s=((_tmp934[1]=Cyc_Absynpp_pat2doc(_tmp425),((_tmp934[0]=Cyc_PP_text(((_tmp935="&",_tag_dyneither(_tmp935,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp934,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL258;_LL26D: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp426=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp426->tag != 2)goto _LL26F;else{_tmp427=_tmp426->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp428=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp426->f2)->r;if(_tmp428->tag != 0)goto _LL26F;};}}_LL26E:
# 1567
{const char*_tmp938;struct Cyc_PP_Doc*_tmp937[2];s=((_tmp937[1]=Cyc_Absynpp_qvar2doc(_tmp427->name),((_tmp937[0]=Cyc_PP_text(((_tmp938="*",_tag_dyneither(_tmp938,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp937,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL258;_LL26F: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp429=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp429->tag != 2)goto _LL271;else{_tmp42A=_tmp429->f1;_tmp42B=_tmp429->f2;}}_LL270:
# 1570
{const char*_tmp93D;const char*_tmp93C;struct Cyc_PP_Doc*_tmp93B[4];s=((_tmp93B[3]=Cyc_Absynpp_pat2doc(_tmp42B),((_tmp93B[2]=Cyc_PP_text(((_tmp93C=" as ",_tag_dyneither(_tmp93C,sizeof(char),5)))),((_tmp93B[1]=Cyc_Absynpp_qvar2doc(_tmp42A->name),((_tmp93B[0]=Cyc_PP_text(((_tmp93D="*",_tag_dyneither(_tmp93D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL258;_LL271: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp42C=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp42C->tag != 14)goto _LL273;else{_tmp42D=_tmp42C->f1;}}_LL272:
# 1573
 s=Cyc_Absynpp_qvar2doc(_tmp42D);
goto _LL258;_LL273: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp42E=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp42E->tag != 15)goto _LL275;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;_tmp431=_tmp42E->f3;}}_LL274: {
# 1576
const char*_tmp93F;const char*_tmp93E;struct _dyneither_ptr term=_tmp431?(_tmp93F=", ...)",_tag_dyneither(_tmp93F,sizeof(char),7)):((_tmp93E=")",_tag_dyneither(_tmp93E,sizeof(char),2)));
{const char*_tmp944;const char*_tmp943;struct Cyc_PP_Doc*_tmp942[2];s=((_tmp942[1]=Cyc_PP_group(((_tmp944="(",_tag_dyneither(_tmp944,sizeof(char),2))),term,((_tmp943=",",_tag_dyneither(_tmp943,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp430)),((_tmp942[0]=Cyc_Absynpp_qvar2doc(_tmp42F),Cyc_PP_cat(_tag_dyneither(_tmp942,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL258;}_LL275: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp432=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp432->tag != 6)goto _LL277;else{if(_tmp432->f1 == 0)goto _LL277;_tmp433=(_tmp432->f1)->aggr_info;_tmp434=_tmp432->f2;_tmp435=_tmp432->f3;_tmp436=_tmp432->f4;}}_LL276: {
# 1580
const char*_tmp946;const char*_tmp945;struct _dyneither_ptr term=_tmp436?(_tmp946=", ...}",_tag_dyneither(_tmp946,sizeof(char),7)):((_tmp945="}",_tag_dyneither(_tmp945,sizeof(char),2)));
struct _tuple0*_tmp469;struct _tuple10 _tmp468=Cyc_Absyn_aggr_kinded_name(_tmp433);_tmp469=_tmp468.f2;
{const char*_tmp951;const char*_tmp950;const char*_tmp94F;const char*_tmp94E;const char*_tmp94D;struct Cyc_PP_Doc*_tmp94C[4];s=((_tmp94C[3]=
# 1584
Cyc_PP_group(((_tmp94E="",_tag_dyneither(_tmp94E,sizeof(char),1))),term,((_tmp94D=",",_tag_dyneither(_tmp94D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp435)),((_tmp94C[2]=
# 1583
Cyc_PP_egroup(((_tmp951="[",_tag_dyneither(_tmp951,sizeof(char),2))),((_tmp950="]",_tag_dyneither(_tmp950,sizeof(char),2))),((_tmp94F=",",_tag_dyneither(_tmp94F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp434))),((_tmp94C[1]=
# 1582
Cyc_Absynpp_lb(),((_tmp94C[0]=Cyc_Absynpp_qvar2doc(_tmp469),Cyc_PP_cat(_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1585
goto _LL258;}_LL277: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp437=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp437->tag != 6)goto _LL279;else{if(_tmp437->f1 != 0)goto _LL279;_tmp438=_tmp437->f2;_tmp439=_tmp437->f3;_tmp43A=_tmp437->f4;}}_LL278: {
# 1587
const char*_tmp953;const char*_tmp952;struct _dyneither_ptr term=_tmp43A?(_tmp953=", ...}",_tag_dyneither(_tmp953,sizeof(char),7)):((_tmp952="}",_tag_dyneither(_tmp952,sizeof(char),2)));
{const char*_tmp95E;const char*_tmp95D;const char*_tmp95C;const char*_tmp95B;const char*_tmp95A;struct Cyc_PP_Doc*_tmp959[3];s=((_tmp959[2]=
# 1590
Cyc_PP_group(((_tmp95B="",_tag_dyneither(_tmp95B,sizeof(char),1))),term,((_tmp95A=",",_tag_dyneither(_tmp95A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp439)),((_tmp959[1]=
# 1589
Cyc_PP_egroup(((_tmp95E="[",_tag_dyneither(_tmp95E,sizeof(char),2))),((_tmp95D="]",_tag_dyneither(_tmp95D,sizeof(char),2))),((_tmp95C=",",_tag_dyneither(_tmp95C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp438))),((_tmp959[0]=
# 1588
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp959,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1591
goto _LL258;}_LL279: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp43B=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp43B->tag != 12)goto _LL27B;else{_tmp43C=_tmp43B->f2;}}_LL27A:
 _tmp43E=_tmp43C;goto _LL27C;_LL27B: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp43D=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp43D->tag != 13)goto _LL27D;else{_tmp43E=_tmp43D->f2;}}_LL27C:
 s=Cyc_Absynpp_qvar2doc(_tmp43E->name);goto _LL258;_LL27D: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp43F=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp43F->tag != 7)goto _LL27F;else{_tmp440=_tmp43F->f2;if(_tmp43F->f3 != 0)goto _LL27F;}}_LL27E:
 s=Cyc_Absynpp_qvar2doc(_tmp440->name);goto _LL258;_LL27F: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp441=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp441->tag != 7)goto _LL281;else{_tmp442=_tmp441->f2;_tmp443=_tmp441->f3;_tmp444=_tmp441->f4;}}_LL280: {
# 1596
const char*_tmp960;const char*_tmp95F;struct _dyneither_ptr term=_tmp444?(_tmp960=", ...)",_tag_dyneither(_tmp960,sizeof(char),7)):((_tmp95F=")",_tag_dyneither(_tmp95F,sizeof(char),2)));
{const char*_tmp965;const char*_tmp964;struct Cyc_PP_Doc*_tmp963[2];s=((_tmp963[1]=Cyc_PP_egroup(((_tmp965="(",_tag_dyneither(_tmp965,sizeof(char),2))),term,((_tmp964=",",_tag_dyneither(_tmp964,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp443)),((_tmp963[0]=Cyc_Absynpp_qvar2doc(_tmp442->name),Cyc_PP_cat(_tag_dyneither(_tmp963,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL258;}_LL281: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp445=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp40E;if(_tmp445->tag != 16)goto _LL258;else{_tmp446=_tmp445->f1;}}_LL282:
# 1600
 s=Cyc_Absynpp_exp2doc(_tmp446);goto _LL258;_LL258:;}
# 1602
return s;}
# 1605
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp96C;const char*_tmp96B;const char*_tmp96A;struct Cyc_PP_Doc*_tmp969[2];return(_tmp969[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp969[0]=
# 1606
Cyc_PP_egroup(((_tmp96C="",_tag_dyneither(_tmp96C,sizeof(char),1))),((_tmp96B="=",_tag_dyneither(_tmp96B,sizeof(char),2))),((_tmp96A="=",_tag_dyneither(_tmp96A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp969,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1610
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp971;struct Cyc_PP_Doc*_tmp970[2];struct Cyc_PP_Doc*_tmp96F[2];return(_tmp96F[1]=
Cyc_PP_nest(2,((_tmp970[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp970[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp970,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp96F[0]=
# 1612
Cyc_PP_text(((_tmp971="default: ",_tag_dyneither(_tmp971,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1614
if(c->where_clause == 0){
const char*_tmp978;const char*_tmp977;struct Cyc_PP_Doc*_tmp976[2];struct Cyc_PP_Doc*_tmp975[4];return(_tmp975[3]=
# 1618
Cyc_PP_nest(2,((_tmp976[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp976[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp976,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp975[2]=
# 1617
Cyc_PP_text(((_tmp977=": ",_tag_dyneither(_tmp977,sizeof(char),3)))),((_tmp975[1]=
# 1616
Cyc_Absynpp_pat2doc(c->pattern),((_tmp975[0]=
# 1615
Cyc_PP_text(((_tmp978="case ",_tag_dyneither(_tmp978,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp975,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1620
const char*_tmp981;const char*_tmp980;const char*_tmp97F;struct Cyc_PP_Doc*_tmp97E[2];struct Cyc_PP_Doc*_tmp97D[6];return(_tmp97D[5]=
# 1625
Cyc_PP_nest(2,((_tmp97E[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp97E[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97E,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97D[4]=
# 1624
Cyc_PP_text(((_tmp97F=": ",_tag_dyneither(_tmp97F,sizeof(char),3)))),((_tmp97D[3]=
# 1623
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp97D[2]=
# 1622
Cyc_PP_text(((_tmp980=" && ",_tag_dyneither(_tmp980,sizeof(char),5)))),((_tmp97D[1]=
# 1621
Cyc_Absynpp_pat2doc(c->pattern),((_tmp97D[0]=
# 1620
Cyc_PP_text(((_tmp981="case ",_tag_dyneither(_tmp981,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97D,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1628
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp982;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp982="",_tag_dyneither(_tmp982,sizeof(char),1))),cs);}
# 1632
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1636
const char*_tmp985;struct Cyc_PP_Doc*_tmp984[3];return(_tmp984[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp984[1]=Cyc_PP_text(((_tmp985=" = ",_tag_dyneither(_tmp985,sizeof(char),4)))),((_tmp984[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp984,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1639
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp986;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp986=",",_tag_dyneither(_tmp986,sizeof(char),2))),fs);}
# 1643
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1647
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp987;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp987=",",_tag_dyneither(_tmp987,sizeof(char),2))),vds);}
# 1651
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp495;struct _tuple0*_tmp496;struct Cyc_Absyn_Tqual _tmp497;void*_tmp498;struct Cyc_Absyn_Exp*_tmp499;struct Cyc_List_List*_tmp49A;struct Cyc_Absyn_Vardecl*_tmp494=vd;_tmp495=_tmp494->sc;_tmp496=_tmp494->name;_tmp497=_tmp494->tq;_tmp498=_tmp494->type;_tmp499=_tmp494->initializer;_tmp49A=_tmp494->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp496);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49A);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL283:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL284:
# 1660
{void*_tmp49B=Cyc_Tcutil_compress(_tmp498);struct Cyc_List_List*_tmp49D;_LL287: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49B;if(_tmp49C->tag != 9)goto _LL289;else{_tmp49D=(_tmp49C->f1).attributes;}}_LL288:
# 1662
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp49D);
goto _LL286;_LL289:;_LL28A:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL286;_LL286:;}
# 1666
break;}{
# 1669
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28B:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL28C:
 tmp_doc=attsdoc;break;}
# 1674
{struct Cyc_Core_Opt*_tmp994;struct Cyc_PP_Doc*_tmp993[2];struct Cyc_PP_Doc*_tmp992[2];const char*_tmp991;const char*_tmp990;struct Cyc_PP_Doc*_tmp98F[5];s=((_tmp98F[4]=
# 1681
Cyc_PP_text(((_tmp990=";",_tag_dyneither(_tmp990,sizeof(char),2)))),((_tmp98F[3]=
# 1678
_tmp499 == 0?
Cyc_PP_nil_doc():(
(_tmp992[1]=Cyc_Absynpp_exp2doc(_tmp499),((_tmp992[0]=Cyc_PP_text(((_tmp991=" = ",_tag_dyneither(_tmp991,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp992,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp98F[2]=
# 1677
Cyc_Absynpp_tqtd2doc(_tmp497,_tmp498,((_tmp994=_cycalloc(sizeof(*_tmp994)),((_tmp994->v=((_tmp993[1]=sn,((_tmp993[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp993,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp994))))),((_tmp98F[1]=
# 1676
Cyc_Absynpp_scope2doc(_tmp495),((_tmp98F[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1682
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1685
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4A5;struct _tuple17 _tmp4A4=*x;_tmp4A5=_tmp4A4.f2;
return Cyc_Absynpp_qvar2doc(_tmp4A5);}
# 1690
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp995[4];return(_tmp995[3]=
# 1695
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp995[2]=
# 1694
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp995[1]=
# 1693
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp995[0]=
# 1692
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp995,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1697
struct Cyc_PP_Doc*_tmp99D[2];const char*_tmp99C;struct Cyc_PP_Doc*_tmp99B[2];struct Cyc_PP_Doc*_tmp99A[12];return(_tmp99A[11]=
# 1708
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp99A[10]=
# 1707
Cyc_Absynpp_rb(),((_tmp99A[9]=
# 1706
Cyc_PP_line_doc(),((_tmp99A[8]=
# 1705
Cyc_PP_nest(2,((_tmp99B[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp99B[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp99B,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp99A[7]=
# 1703
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp99D[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp99D[0]=Cyc_PP_text(((_tmp99C=":",_tag_dyneither(_tmp99C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp99A[6]=
# 1702
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp99A[5]=
# 1701
Cyc_Absynpp_lb(),((_tmp99A[4]=Cyc_PP_blank_doc(),((_tmp99A[3]=
# 1700
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp99A[2]=
# 1699
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp99A[1]=
# 1698
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp99A[0]=
# 1697
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp99A,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1711
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4AC;struct _tuple0*_tmp4AD;struct Cyc_List_List*_tmp4AE;struct Cyc_Core_Opt*_tmp4AF;int _tmp4B0;struct Cyc_Absyn_Datatypedecl*_tmp4AB=dd;_tmp4AC=_tmp4AB->sc;_tmp4AD=_tmp4AB->name;_tmp4AE=_tmp4AB->tvs;_tmp4AF=_tmp4AB->fields;_tmp4B0=_tmp4AB->is_extensible;
if(_tmp4AF == 0){
const char*_tmp9A2;const char*_tmp9A1;struct Cyc_PP_Doc*_tmp9A0[5];return(_tmp9A0[4]=
# 1718
Cyc_Absynpp_ktvars2doc(_tmp4AE),((_tmp9A0[3]=
# 1717
_tmp4B0?Cyc_Absynpp_qvar2bolddoc(_tmp4AD): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AD),((_tmp9A0[2]=
# 1716
Cyc_PP_text(((_tmp9A1="datatype ",_tag_dyneither(_tmp9A1,sizeof(char),10)))),((_tmp9A0[1]=
# 1715
_tmp4B0?Cyc_PP_text(((_tmp9A2="@extensible ",_tag_dyneither(_tmp9A2,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A0[0]=
# 1714
Cyc_Absynpp_scope2doc(_tmp4AC),Cyc_PP_cat(_tag_dyneither(_tmp9A0,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1720
const char*_tmp9A9;const char*_tmp9A8;struct Cyc_PP_Doc*_tmp9A7[2];struct Cyc_PP_Doc*_tmp9A6[10];return(_tmp9A6[9]=
# 1728
Cyc_Absynpp_rb(),((_tmp9A6[8]=
# 1727
Cyc_PP_line_doc(),((_tmp9A6[7]=
# 1726
Cyc_PP_nest(2,((_tmp9A7[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4AF->v),((_tmp9A7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A6[6]=
# 1725
Cyc_Absynpp_lb(),((_tmp9A6[5]=Cyc_PP_blank_doc(),((_tmp9A6[4]=
# 1724
Cyc_Absynpp_ktvars2doc(_tmp4AE),((_tmp9A6[3]=
# 1723
_tmp4B0?Cyc_Absynpp_qvar2bolddoc(_tmp4AD): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AD),((_tmp9A6[2]=
# 1722
Cyc_PP_text(((_tmp9A8="datatype ",_tag_dyneither(_tmp9A8,sizeof(char),10)))),((_tmp9A6[1]=
# 1721
_tmp4B0?Cyc_PP_text(((_tmp9A9="@extensible ",_tag_dyneither(_tmp9A9,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A6[0]=
# 1720
Cyc_Absynpp_scope2doc(_tmp4AC),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1731
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4B9;struct _tuple0*_tmp4BA;struct Cyc_Core_Opt*_tmp4BB;struct Cyc_Absyn_Enumdecl*_tmp4B8=ed;_tmp4B9=_tmp4B8->sc;_tmp4BA=_tmp4B8->name;_tmp4BB=_tmp4B8->fields;
if(_tmp4BB == 0){
const char*_tmp9AC;struct Cyc_PP_Doc*_tmp9AB[3];return(_tmp9AB[2]=
# 1736
Cyc_Absynpp_typedef_name2bolddoc(_tmp4BA),((_tmp9AB[1]=
# 1735
Cyc_PP_text(((_tmp9AC="enum ",_tag_dyneither(_tmp9AC,sizeof(char),6)))),((_tmp9AB[0]=
# 1734
Cyc_Absynpp_scope2doc(_tmp4B9),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1739
const char*_tmp9B1;struct Cyc_PP_Doc*_tmp9B0[2];struct Cyc_PP_Doc*_tmp9AF[8];return(_tmp9AF[7]=
# 1745
Cyc_Absynpp_rb(),((_tmp9AF[6]=
# 1744
Cyc_PP_line_doc(),((_tmp9AF[5]=
# 1743
Cyc_PP_nest(2,((_tmp9B0[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4BB->v),((_tmp9B0[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9AF[4]=
# 1742
Cyc_Absynpp_lb(),((_tmp9AF[3]=Cyc_PP_blank_doc(),((_tmp9AF[2]=
# 1741
Cyc_Absynpp_qvar2bolddoc(_tmp4BA),((_tmp9AF[1]=
# 1740
Cyc_PP_text(((_tmp9B1="enum ",_tag_dyneither(_tmp9B1,sizeof(char),6)))),((_tmp9AF[0]=
# 1739
Cyc_Absynpp_scope2doc(_tmp4B9),Cyc_PP_cat(_tag_dyneither(_tmp9AF,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1748
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4C1=d->r;struct Cyc_Absyn_Fndecl*_tmp4C3;struct Cyc_Absyn_Aggrdecl*_tmp4C5;struct Cyc_Absyn_Vardecl*_tmp4C7;struct Cyc_Absyn_Tvar*_tmp4C9;struct Cyc_Absyn_Vardecl*_tmp4CA;int _tmp4CB;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_Absyn_Tvar*_tmp4CE;struct Cyc_Absyn_Vardecl*_tmp4CF;struct Cyc_Absyn_Datatypedecl*_tmp4D1;struct Cyc_Absyn_Pat*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_List_List*_tmp4D6;struct Cyc_Absyn_Enumdecl*_tmp4D8;struct Cyc_Absyn_Typedefdecl*_tmp4DA;struct _dyneither_ptr*_tmp4DC;struct Cyc_List_List*_tmp4DD;struct _tuple0*_tmp4DF;struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*_tmp4E5;_LL28F: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4C2=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4C2->tag != 1)goto _LL291;else{_tmp4C3=_tmp4C2->f1;}}_LL290: {
# 1752
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9B7;struct Cyc_Absyn_FnInfo _tmp9B6;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9B5;void*t=(void*)((_tmp9B5=_cycalloc(sizeof(*_tmp9B5)),((_tmp9B5[0]=((_tmp9B7.tag=9,((_tmp9B7.f1=((_tmp9B6.tvars=_tmp4C3->tvs,((_tmp9B6.effect=_tmp4C3->effect,((_tmp9B6.ret_tqual=_tmp4C3->ret_tqual,((_tmp9B6.ret_typ=_tmp4C3->ret_type,((_tmp9B6.args=
# 1756
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C3->args),((_tmp9B6.c_varargs=_tmp4C3->c_varargs,((_tmp9B6.cyc_varargs=_tmp4C3->cyc_varargs,((_tmp9B6.rgn_po=_tmp4C3->rgn_po,((_tmp9B6.attributes=0,_tmp9B6)))))))))))))))))),_tmp9B7)))),_tmp9B5))));
# 1760
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C3->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C3->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
{const char*_tmp9B8;inlinedoc=Cyc_PP_text(((_tmp9B8="inline ",_tag_dyneither(_tmp9B8,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B0:
{const char*_tmp9B9;inlinedoc=Cyc_PP_text(((_tmp9B9="__inline ",_tag_dyneither(_tmp9B9,sizeof(char),10))));}break;}else{
# 1768
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C3->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B2:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B3:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C3->attributes);break;}{
# 1775
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C3->name);
struct Cyc_PP_Doc*_tmp9BC[2];struct Cyc_Core_Opt*_tmp9BB;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9BB=_cycalloc(sizeof(*_tmp9BB)),((_tmp9BB->v=((_tmp9BC[1]=namedoc,((_tmp9BC[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9BC,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9BB)))));
struct Cyc_PP_Doc*_tmp9BF[2];struct Cyc_PP_Doc*_tmp9BE[5];struct Cyc_PP_Doc*bodydoc=(_tmp9BE[4]=
# 1781
Cyc_Absynpp_rb(),((_tmp9BE[3]=
# 1780
Cyc_PP_line_doc(),((_tmp9BE[2]=
# 1779
Cyc_PP_nest(2,((_tmp9BF[1]=Cyc_Absynpp_stmt2doc(_tmp4C3->body,0),((_tmp9BF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9BE[1]=
# 1778
Cyc_Absynpp_lb(),((_tmp9BE[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1782
{struct Cyc_PP_Doc*_tmp9C0[4];s=((_tmp9C0[3]=bodydoc,((_tmp9C0[2]=tqtddoc,((_tmp9C0[1]=scopedoc,((_tmp9C0[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1784
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B5:
{struct Cyc_PP_Doc*_tmp9C1[2];s=((_tmp9C1[1]=s,((_tmp9C1[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B6:
 break;}
# 1789
goto _LL28E;};};}_LL291: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C4=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4C4->tag != 6)goto _LL293;else{_tmp4C5=_tmp4C4->f1;}}_LL292:
# 1792
{const char*_tmp9C4;struct Cyc_PP_Doc*_tmp9C3[2];s=((_tmp9C3[1]=Cyc_PP_text(((_tmp9C4=";",_tag_dyneither(_tmp9C4,sizeof(char),2)))),((_tmp9C3[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4C5),Cyc_PP_cat(_tag_dyneither(_tmp9C3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28E;_LL293: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C6=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4C6->tag != 0)goto _LL295;else{_tmp4C7=_tmp4C6->f1;}}_LL294:
# 1795
 s=Cyc_Absynpp_vardecl2doc(_tmp4C7);
goto _LL28E;_LL295: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4C8=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4C8->tag != 4)goto _LL297;else{_tmp4C9=_tmp4C8->f1;_tmp4CA=_tmp4C8->f2;_tmp4CB=_tmp4C8->f3;_tmp4CC=_tmp4C8->f4;}}_LL296:
# 1798
{const char*_tmp9D7;const char*_tmp9D6;const char*_tmp9D5;const char*_tmp9D4;struct Cyc_PP_Doc*_tmp9D3[3];const char*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_PP_Doc*_tmp9CF[8];s=((_tmp9CF[7]=
# 1806
Cyc_PP_text(((_tmp9D0=";",_tag_dyneither(_tmp9D0,sizeof(char),2)))),((_tmp9CF[6]=
# 1804
_tmp4CC != 0?
(_tmp9D3[2]=Cyc_PP_text(((_tmp9D2=")",_tag_dyneither(_tmp9D2,sizeof(char),2)))),((_tmp9D3[1]=Cyc_Absynpp_exp2doc(_tmp4CC),((_tmp9D3[0]=Cyc_PP_text(((_tmp9D1=" = open(",_tag_dyneither(_tmp9D1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9D3,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9CF[5]=
# 1803
_tmp4CB?Cyc_PP_text(((_tmp9D4=" @resetable",_tag_dyneither(_tmp9D4,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9CF[4]=
# 1802
Cyc_Absynpp_qvar2doc(_tmp4CA->name),((_tmp9CF[3]=
# 1801
Cyc_PP_text(((_tmp9D5=">",_tag_dyneither(_tmp9D5,sizeof(char),2)))),((_tmp9CF[2]=
# 1800
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C9)),((_tmp9CF[1]=
# 1799
Cyc_PP_text(((_tmp9D6="<",_tag_dyneither(_tmp9D6,sizeof(char),2)))),((_tmp9CF[0]=
# 1798
Cyc_PP_text(((_tmp9D7="region",_tag_dyneither(_tmp9D7,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9CF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1807
goto _LL28E;_LL297: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4CD=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4CD->tag != 5)goto _LL299;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;}}_LL298:
# 1809
{const char*_tmp9E0;const char*_tmp9DF;const char*_tmp9DE;const char*_tmp9DD;struct Cyc_PP_Doc*_tmp9DC[6];s=((_tmp9DC[5]=
Cyc_PP_text(((_tmp9DD=";",_tag_dyneither(_tmp9DD,sizeof(char),2)))),((_tmp9DC[4]=Cyc_Absynpp_vardecl2doc(_tmp4CF),((_tmp9DC[3]=
# 1809
Cyc_PP_text(((_tmp9DE="> ",_tag_dyneither(_tmp9DE,sizeof(char),3)))),((_tmp9DC[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CE)),((_tmp9DC[1]=Cyc_PP_text(((_tmp9DF=" <",_tag_dyneither(_tmp9DF,sizeof(char),3)))),((_tmp9DC[0]=Cyc_PP_text(((_tmp9E0="alias",_tag_dyneither(_tmp9E0,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9DC,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 1811
goto _LL28E;_LL299: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4D0=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4D0->tag != 7)goto _LL29B;else{_tmp4D1=_tmp4D0->f1;}}_LL29A:
# 1813
{const char*_tmp9E3;struct Cyc_PP_Doc*_tmp9E2[2];s=((_tmp9E2[1]=Cyc_PP_text(((_tmp9E3=";",_tag_dyneither(_tmp9E3,sizeof(char),2)))),((_tmp9E2[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4D1),Cyc_PP_cat(_tag_dyneither(_tmp9E2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28E;_LL29B: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D2=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4D2->tag != 2)goto _LL29D;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f3;}}_LL29C:
# 1816
{const char*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[5];s=((_tmp9E7[4]=
# 1820
Cyc_PP_text(((_tmp9E8=";",_tag_dyneither(_tmp9E8,sizeof(char),2)))),((_tmp9E7[3]=
# 1819
Cyc_Absynpp_exp2doc(_tmp4D4),((_tmp9E7[2]=
# 1818
Cyc_PP_text(((_tmp9E9=" = ",_tag_dyneither(_tmp9E9,sizeof(char),4)))),((_tmp9E7[1]=
# 1817
Cyc_Absynpp_pat2doc(_tmp4D3),((_tmp9E7[0]=
# 1816
Cyc_PP_text(((_tmp9EA="let ",_tag_dyneither(_tmp9EA,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1821
goto _LL28E;_LL29D: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4D5=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4D5->tag != 3)goto _LL29F;else{_tmp4D6=_tmp4D5->f1;}}_LL29E:
# 1823
{const char*_tmp9EF;const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[3];s=((_tmp9ED[2]=Cyc_PP_text(((_tmp9EE=";",_tag_dyneither(_tmp9EE,sizeof(char),2)))),((_tmp9ED[1]=Cyc_Absynpp_ids2doc(_tmp4D6),((_tmp9ED[0]=Cyc_PP_text(((_tmp9EF="let ",_tag_dyneither(_tmp9EF,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28E;_LL29F: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4D7=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4D7->tag != 8)goto _LL2A1;else{_tmp4D8=_tmp4D7->f1;}}_LL2A0:
# 1826
{const char*_tmp9F2;struct Cyc_PP_Doc*_tmp9F1[2];s=((_tmp9F1[1]=Cyc_PP_text(((_tmp9F2=";",_tag_dyneither(_tmp9F2,sizeof(char),2)))),((_tmp9F1[0]=Cyc_Absynpp_enumdecl2doc(_tmp4D8),Cyc_PP_cat(_tag_dyneither(_tmp9F1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28E;_LL2A1: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4D9=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4D9->tag != 9)goto _LL2A3;else{_tmp4DA=_tmp4D9->f1;}}_LL2A2: {
# 1829
void*t;
if(_tmp4DA->defn != 0)
t=(void*)_check_null(_tmp4DA->defn);else{
# 1833
t=Cyc_Absyn_new_evar(_tmp4DA->kind,0);}
{const char*_tmp9FC;struct Cyc_Core_Opt*_tmp9FB;struct Cyc_PP_Doc*_tmp9FA[2];const char*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[4];s=((_tmp9F8[3]=
# 1841
Cyc_PP_text(((_tmp9F9=";",_tag_dyneither(_tmp9F9,sizeof(char),2)))),((_tmp9F8[2]=
# 1840
Cyc_Absynpp_atts2doc(_tmp4DA->atts),((_tmp9F8[1]=
# 1835
Cyc_Absynpp_tqtd2doc(_tmp4DA->tq,t,(
# 1837
(_tmp9FB=_cycalloc(sizeof(*_tmp9FB)),((_tmp9FB->v=((_tmp9FA[1]=
Cyc_Absynpp_tvars2doc(_tmp4DA->tvs),((_tmp9FA[0]=
# 1837
Cyc_Absynpp_typedef_name2bolddoc(_tmp4DA->name),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FB))))),((_tmp9F8[0]=
# 1834
Cyc_PP_text(((_tmp9FC="typedef ",_tag_dyneither(_tmp9FC,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1843
goto _LL28E;}_LL2A3: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4DB=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4DB->tag != 10)goto _LL2A5;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;}}_LL2A4:
# 1845
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DC);
{const char*_tmpA01;const char*_tmpA00;struct Cyc_PP_Doc*_tmp9FF[8];s=((_tmp9FF[7]=
# 1852
Cyc_Absynpp_rb(),((_tmp9FF[6]=
# 1851
Cyc_PP_line_doc(),((_tmp9FF[5]=
# 1850
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA00="",_tag_dyneither(_tmpA00,sizeof(char),1))),_tmp4DD),((_tmp9FF[4]=
# 1849
Cyc_PP_line_doc(),((_tmp9FF[3]=
# 1848
Cyc_Absynpp_lb(),((_tmp9FF[2]=Cyc_PP_blank_doc(),((_tmp9FF[1]=
# 1847
Cyc_PP_textptr(_tmp4DC),((_tmp9FF[0]=
# 1846
Cyc_PP_text(((_tmpA01="namespace ",_tag_dyneither(_tmpA01,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp9FF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1853
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28E;_LL2A5: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4DE=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4DE->tag != 11)goto _LL2A7;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL2A6:
# 1856
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA06;const char*_tmpA05;struct Cyc_PP_Doc*_tmpA04[8];s=((_tmpA04[7]=
# 1863
Cyc_Absynpp_rb(),((_tmpA04[6]=
# 1862
Cyc_PP_line_doc(),((_tmpA04[5]=
# 1861
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA05="",_tag_dyneither(_tmpA05,sizeof(char),1))),_tmp4E0),((_tmpA04[4]=
# 1860
Cyc_PP_line_doc(),((_tmpA04[3]=
# 1859
Cyc_Absynpp_lb(),((_tmpA04[2]=Cyc_PP_blank_doc(),((_tmpA04[1]=
# 1858
Cyc_Absynpp_qvar2doc(_tmp4DF),((_tmpA04[0]=
# 1857
Cyc_PP_text(((_tmpA06="using ",_tag_dyneither(_tmpA06,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1865
const char*_tmpA11;const char*_tmpA10;const char*_tmpA0F;const char*_tmpA0E;const char*_tmpA0D;struct Cyc_PP_Doc*_tmpA0C[11];s=((_tmpA0C[10]=
# 1875
Cyc_PP_text(((_tmpA0D=" */",_tag_dyneither(_tmpA0D,sizeof(char),4)))),((_tmpA0C[9]=
# 1874
Cyc_Absynpp_rb(),((_tmpA0C[8]=
# 1873
Cyc_PP_text(((_tmpA0E="/* ",_tag_dyneither(_tmpA0E,sizeof(char),4)))),((_tmpA0C[7]=
# 1872
Cyc_PP_line_doc(),((_tmpA0C[6]=
# 1871
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0F="",_tag_dyneither(_tmpA0F,sizeof(char),1))),_tmp4E0),((_tmpA0C[5]=
# 1870
Cyc_PP_line_doc(),((_tmpA0C[4]=
# 1869
Cyc_PP_text(((_tmpA10=" */",_tag_dyneither(_tmpA10,sizeof(char),4)))),((_tmpA0C[3]=
# 1868
Cyc_Absynpp_lb(),((_tmpA0C[2]=
# 1867
Cyc_PP_blank_doc(),((_tmpA0C[1]=
# 1866
Cyc_Absynpp_qvar2doc(_tmp4DF),((_tmpA0C[0]=
# 1865
Cyc_PP_text(((_tmpA11="/* using ",_tag_dyneither(_tmpA11,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA0C,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1876
goto _LL28E;_LL2A7: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4E1=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4E1->tag != 12)goto _LL2A9;else{_tmp4E2=_tmp4E1->f1;}}_LL2A8:
# 1878
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA16;const char*_tmpA15;struct Cyc_PP_Doc*_tmpA14[6];s=((_tmpA14[5]=
# 1884
Cyc_Absynpp_rb(),((_tmpA14[4]=
# 1883
Cyc_PP_line_doc(),((_tmpA14[3]=
# 1882
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA15="",_tag_dyneither(_tmpA15,sizeof(char),1))),_tmp4E2),((_tmpA14[2]=
# 1881
Cyc_PP_line_doc(),((_tmpA14[1]=
# 1880
Cyc_Absynpp_lb(),((_tmpA14[0]=
# 1879
Cyc_PP_text(((_tmpA16="extern \"C\" ",_tag_dyneither(_tmpA16,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1886
const char*_tmpA21;const char*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;const char*_tmpA1D;struct Cyc_PP_Doc*_tmpA1C[9];s=((_tmpA1C[8]=
# 1894
Cyc_PP_text(((_tmpA1D=" */",_tag_dyneither(_tmpA1D,sizeof(char),4)))),((_tmpA1C[7]=
# 1893
Cyc_Absynpp_rb(),((_tmpA1C[6]=
# 1892
Cyc_PP_text(((_tmpA1E="/* ",_tag_dyneither(_tmpA1E,sizeof(char),4)))),((_tmpA1C[5]=
# 1891
Cyc_PP_line_doc(),((_tmpA1C[4]=
# 1890
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1F="",_tag_dyneither(_tmpA1F,sizeof(char),1))),_tmp4E2),((_tmpA1C[3]=
# 1889
Cyc_PP_line_doc(),((_tmpA1C[2]=
# 1888
Cyc_PP_text(((_tmpA20=" */",_tag_dyneither(_tmpA20,sizeof(char),4)))),((_tmpA1C[1]=
# 1887
Cyc_Absynpp_lb(),((_tmpA1C[0]=
# 1886
Cyc_PP_text(((_tmpA21="/* extern \"C\" ",_tag_dyneither(_tmpA21,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA1C,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1895
goto _LL28E;_LL2A9: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E3=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4E3->tag != 13)goto _LL2AB;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;}}_LL2AA:
# 1897
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4E5 != 0){
const char*_tmpA26;const char*_tmpA25;struct Cyc_PP_Doc*_tmpA24[7];exs_doc=((_tmpA24[6]=
# 1902
Cyc_Absynpp_rb(),((_tmpA24[5]=Cyc_PP_line_doc(),((_tmpA24[4]=
# 1901
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA25=",",_tag_dyneither(_tmpA25,sizeof(char),2))),_tmp4E5),((_tmpA24[3]=Cyc_PP_line_doc(),((_tmpA24[2]=
# 1900
Cyc_Absynpp_lb(),((_tmpA24[1]=Cyc_PP_text(((_tmpA26=" export ",_tag_dyneither(_tmpA26,sizeof(char),9)))),((_tmpA24[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA24,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1904
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA2B;const char*_tmpA2A;struct Cyc_PP_Doc*_tmpA29[6];s=((_tmpA29[5]=exs_doc,((_tmpA29[4]=
# 1909
Cyc_PP_line_doc(),((_tmpA29[3]=
# 1908
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA2A="",_tag_dyneither(_tmpA2A,sizeof(char),1))),_tmp4E4),((_tmpA29[2]=
# 1907
Cyc_PP_line_doc(),((_tmpA29[1]=
# 1906
Cyc_Absynpp_lb(),((_tmpA29[0]=
# 1905
Cyc_PP_text(((_tmpA2B="extern \"C include\" ",_tag_dyneither(_tmpA2B,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA29,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1912
const char*_tmpA36;const char*_tmpA35;const char*_tmpA34;const char*_tmpA33;const char*_tmpA32;struct Cyc_PP_Doc*_tmpA31[9];s=((_tmpA31[8]=
# 1920
Cyc_PP_text(((_tmpA32=" */",_tag_dyneither(_tmpA32,sizeof(char),4)))),((_tmpA31[7]=
# 1919
Cyc_Absynpp_rb(),((_tmpA31[6]=
# 1918
Cyc_PP_text(((_tmpA33="/* ",_tag_dyneither(_tmpA33,sizeof(char),4)))),((_tmpA31[5]=
# 1917
Cyc_PP_line_doc(),((_tmpA31[4]=
# 1916
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA34="",_tag_dyneither(_tmpA34,sizeof(char),1))),_tmp4E4),((_tmpA31[3]=
# 1915
Cyc_PP_line_doc(),((_tmpA31[2]=
# 1914
Cyc_PP_text(((_tmpA35=" */",_tag_dyneither(_tmpA35,sizeof(char),4)))),((_tmpA31[1]=
# 1913
Cyc_Absynpp_lb(),((_tmpA31[0]=
# 1912
Cyc_PP_text(((_tmpA36="/* extern \"C include\" ",_tag_dyneither(_tmpA36,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA31,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1921
goto _LL28E;_LL2AB: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4E6=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4E6->tag != 14)goto _LL2AD;}_LL2AC:
# 1923
{const char*_tmpA39;struct Cyc_PP_Doc*_tmpA38[2];s=((_tmpA38[1]=Cyc_Absynpp_lb(),((_tmpA38[0]=Cyc_PP_text(((_tmpA39="__cyclone_port_on__;",_tag_dyneither(_tmpA39,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA38,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28E;_LL2AD: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4E7=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4C1;if(_tmp4E7->tag != 15)goto _LL28E;}_LL2AE:
# 1926
{const char*_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[2];s=((_tmpA3B[1]=Cyc_Absynpp_lb(),((_tmpA3B[0]=Cyc_PP_text(((_tmpA3C="__cyclone_port_off__;",_tag_dyneither(_tmpA3C,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28E;_LL28E:;}
# 1930
return s;}
# 1933
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2B8: {
const char*_tmpA3D;return Cyc_PP_text(((_tmpA3D="static ",_tag_dyneither(_tmpA3D,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B9:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2BA: {
const char*_tmpA3E;return Cyc_PP_text(((_tmpA3E="extern ",_tag_dyneither(_tmpA3E,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2BB: {
const char*_tmpA3F;return Cyc_PP_text(((_tmpA3F="extern \"C\" ",_tag_dyneither(_tmpA3F,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2BC: {
const char*_tmpA40;return Cyc_PP_text(((_tmpA40="abstract ",_tag_dyneither(_tmpA40,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BD: {
const char*_tmpA41;return Cyc_PP_text(((_tmpA41="register ",_tag_dyneither(_tmpA41,sizeof(char),10))));}}}
# 1946
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp53D=t;struct Cyc_Absyn_Tvar*_tmp53F;struct Cyc_List_List*_tmp541;_LL2C0: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp53E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53D;if(_tmp53E->tag != 2)goto _LL2C2;else{_tmp53F=_tmp53E->f1;}}_LL2C1:
 return Cyc_Tcutil_is_temp_tvar(_tmp53F);_LL2C2: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp540=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp53D;if(_tmp540->tag != 24)goto _LL2C4;else{_tmp541=_tmp540->f1;}}_LL2C3:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp541);_LL2C4:;_LL2C5:
 return 0;_LL2BF:;}
# 1958
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp542=t;struct Cyc_Absyn_Typedefdecl*_tmp546;void*_tmp547;_LL2C7: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp542;if(_tmp543->tag != 12)goto _LL2C9;}_LL2C8:
 return 1;_LL2C9: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp542;if(_tmp544->tag != 14)goto _LL2CB;}_LL2CA:
 return 1;_LL2CB:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp545=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp542;if(_tmp545->tag != 17)goto _LL2CD;else{_tmp546=_tmp545->f3;_tmp547=(void*)_tmp545->f4;}}if(!(_tmp547 != 0))goto _LL2CD;_LL2CC:
# 1965
 return Cyc_Absynpp_is_anon_aggrtype(_tmp547);_LL2CD:;_LL2CE:
 return 0;_LL2C6:;}
# 1972
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1975
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA42;struct _tuple16 _tmp549=(_tmpA42.f1=(void*)tms->hd,((_tmpA42.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA42)));_LL2D0:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp54A=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp549.f1;if(_tmp54A->tag != 2)goto _LL2D2;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54B=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp549.f2;if(_tmp54B->tag != 3)goto _LL2D2;};_LL2D1: {
# 1978
struct Cyc_List_List*_tmpA45;struct Cyc_List_List*_tmpA44;return(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->hd=(void*)tms->hd,((_tmpA44->tl=((_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA45->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA45)))))),_tmpA44)))));}_LL2D2:;_LL2D3: {
struct Cyc_List_List*_tmpA46;return(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->hd=atts,((_tmpA46->tl=tms,_tmpA46)))));}_LL2CF:;}else{
# 1981
struct Cyc_List_List*_tmpA47;return(_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47->hd=atts,((_tmpA47->tl=tms,_tmpA47)))));}}
# 1986
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1988
void*_tmp550=t;void*_tmp552;struct Cyc_Absyn_Tqual _tmp553;struct Cyc_Absyn_Exp*_tmp554;union Cyc_Absyn_Constraint*_tmp555;unsigned int _tmp556;void*_tmp558;struct Cyc_Absyn_Tqual _tmp559;struct Cyc_Absyn_PtrAtts _tmp55A;struct Cyc_List_List*_tmp55C;void*_tmp55D;struct Cyc_Absyn_Tqual _tmp55E;void*_tmp55F;struct Cyc_List_List*_tmp560;int _tmp561;struct Cyc_Absyn_VarargInfo*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_List_List*_tmp564;struct Cyc_Core_Opt*_tmp566;void*_tmp567;int _tmp568;struct _tuple0*_tmp56A;struct Cyc_List_List*_tmp56B;struct Cyc_Absyn_Typedefdecl*_tmp56C;void*_tmp56D;_LL2D5: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp551=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp550;if(_tmp551->tag != 8)goto _LL2D7;else{_tmp552=(_tmp551->f1).elt_type;_tmp553=(_tmp551->f1).tq;_tmp554=(_tmp551->f1).num_elts;_tmp555=(_tmp551->f1).zero_term;_tmp556=(_tmp551->f1).zt_loc;}}_LL2D6: {
# 1991
struct Cyc_Absyn_Tqual _tmp56F;void*_tmp570;struct Cyc_List_List*_tmp571;struct _tuple12 _tmp56E=Cyc_Absynpp_to_tms(r,_tmp553,_tmp552);_tmp56F=_tmp56E.f1;_tmp570=_tmp56E.f2;_tmp571=_tmp56E.f3;{
void*tm;
if(_tmp554 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA4A;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA49;tm=(void*)((_tmpA49=_region_malloc(r,sizeof(*_tmpA49)),((_tmpA49[0]=((_tmpA4A.tag=0,((_tmpA4A.f1=_tmp555,((_tmpA4A.f2=_tmp556,_tmpA4A)))))),_tmpA49))));}else{
# 1996
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA4D;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA4C;tm=(void*)((_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=1,((_tmpA4D.f1=_tmp554,((_tmpA4D.f2=_tmp555,((_tmpA4D.f3=_tmp556,_tmpA4D)))))))),_tmpA4C))));}{
struct Cyc_List_List*_tmpA50;struct _tuple12 _tmpA4F;return(_tmpA4F.f1=_tmp56F,((_tmpA4F.f2=_tmp570,((_tmpA4F.f3=((_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->hd=tm,((_tmpA50->tl=_tmp571,_tmpA50)))))),_tmpA4F)))));};};}_LL2D7: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp557=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550;if(_tmp557->tag != 5)goto _LL2D9;else{_tmp558=(_tmp557->f1).elt_typ;_tmp559=(_tmp557->f1).elt_tq;_tmp55A=(_tmp557->f1).ptr_atts;}}_LL2D8: {
# 2000
struct Cyc_Absyn_Tqual _tmp579;void*_tmp57A;struct Cyc_List_List*_tmp57B;struct _tuple12 _tmp578=Cyc_Absynpp_to_tms(r,_tmp559,_tmp558);_tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_tmp57B=_tmp578.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA56;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA55;struct Cyc_List_List*_tmpA54;_tmp57B=((_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54->hd=(void*)((_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA55.tag=2,((_tmpA55.f1=_tmp55A,((_tmpA55.f2=tq,_tmpA55)))))),_tmpA56)))),((_tmpA54->tl=_tmp57B,_tmpA54))))));}{
struct _tuple12 _tmpA57;return(_tmpA57.f1=_tmp579,((_tmpA57.f2=_tmp57A,((_tmpA57.f3=_tmp57B,_tmpA57)))));};}_LL2D9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp550;if(_tmp55B->tag != 9)goto _LL2DB;else{_tmp55C=(_tmp55B->f1).tvars;_tmp55D=(_tmp55B->f1).effect;_tmp55E=(_tmp55B->f1).ret_tqual;_tmp55F=(_tmp55B->f1).ret_typ;_tmp560=(_tmp55B->f1).args;_tmp561=(_tmp55B->f1).c_varargs;_tmp562=(_tmp55B->f1).cyc_varargs;_tmp563=(_tmp55B->f1).rgn_po;_tmp564=(_tmp55B->f1).attributes;}}_LL2DA:
# 2006
 if(!Cyc_Absynpp_print_all_tvars){
# 2010
if(_tmp55D == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp55D)){
_tmp55D=0;
_tmp55C=0;}}else{
# 2015
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2018
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp55C);}{
# 2023
struct Cyc_Absyn_Tqual _tmp581;void*_tmp582;struct Cyc_List_List*_tmp583;struct _tuple12 _tmp580=Cyc_Absynpp_to_tms(r,_tmp55E,_tmp55F);_tmp581=_tmp580.f1;_tmp582=_tmp580.f2;_tmp583=_tmp580.f3;{
struct Cyc_List_List*tms=_tmp583;
# 2034 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2E1:
# 2036
 if(_tmp564 != 0){
# 2039
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA5A;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA59;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA5A.tag=5,((_tmpA5A.f1=0,((_tmpA5A.f2=_tmp564,_tmpA5A)))))),_tmpA59)))),tms);}
# 2041
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA69;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA68;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA67;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA66;struct Cyc_List_List*_tmpA65;tms=((_tmpA65=_region_malloc(r,sizeof(*_tmpA65)),((_tmpA65->hd=(void*)((_tmpA69=_region_malloc(r,sizeof(*_tmpA69)),((_tmpA69[0]=(
(_tmpA66.tag=3,((_tmpA66.f1=(void*)((_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68[0]=((_tmpA67.tag=1,((_tmpA67.f1=_tmp560,((_tmpA67.f2=_tmp561,((_tmpA67.f3=_tmp562,((_tmpA67.f4=_tmp55D,((_tmpA67.f5=_tmp563,_tmpA67)))))))))))),_tmpA68)))),_tmpA66)))),_tmpA69)))),((_tmpA65->tl=tms,_tmpA65))))));}
# 2045
break;case Cyc_Cyclone_Vc_c: _LL2E2:
# 2047
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA78;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA77;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA76;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA75;struct Cyc_List_List*_tmpA74;tms=((_tmpA74=_region_malloc(r,sizeof(*_tmpA74)),((_tmpA74->hd=(void*)((_tmpA78=_region_malloc(r,sizeof(*_tmpA78)),((_tmpA78[0]=(
(_tmpA75.tag=3,((_tmpA75.f1=(void*)((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77[0]=((_tmpA76.tag=1,((_tmpA76.f1=_tmp560,((_tmpA76.f2=_tmp561,((_tmpA76.f3=_tmp562,((_tmpA76.f4=_tmp55D,((_tmpA76.f5=_tmp563,_tmpA76)))))))))))),_tmpA77)))),_tmpA75)))),_tmpA78)))),((_tmpA74->tl=tms,_tmpA74))))));}
# 2051
for(0;_tmp564 != 0;_tmp564=_tmp564->tl){
void*_tmp590=(void*)_tmp564->hd;_LL2E5: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp591=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp590;if(_tmp591->tag != 1)goto _LL2E7;}_LL2E6:
 goto _LL2E8;_LL2E7: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp592=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp590;if(_tmp592->tag != 2)goto _LL2E9;}_LL2E8:
 goto _LL2EA;_LL2E9: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp593=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp590;if(_tmp593->tag != 3)goto _LL2EB;}_LL2EA:
# 2056
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA82;struct Cyc_List_List*_tmpA81;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA80;struct Cyc_List_List*_tmpA7F;tms=((_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F->hd=(void*)((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA80.tag=5,((_tmpA80.f1=0,((_tmpA80.f2=((_tmpA81=_cycalloc(sizeof(*_tmpA81)),((_tmpA81->hd=(void*)_tmp564->hd,((_tmpA81->tl=0,_tmpA81)))))),_tmpA80)))))),_tmpA82)))),((_tmpA7F->tl=tms,_tmpA7F))))));}
goto AfterAtts;_LL2EB:;_LL2EC:
 goto _LL2E4;_LL2E4:;}
# 2060
break;}
# 2063
AfterAtts:
 if(_tmp55C != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA88;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA87;struct Cyc_List_List*_tmpA86;tms=((_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86->hd=(void*)((_tmpA88=_region_malloc(r,sizeof(*_tmpA88)),((_tmpA88[0]=((_tmpA87.tag=4,((_tmpA87.f1=_tmp55C,((_tmpA87.f2=0,((_tmpA87.f3=1,_tmpA87)))))))),_tmpA88)))),((_tmpA86->tl=tms,_tmpA86))))));}{
struct _tuple12 _tmpA89;return(_tmpA89.f1=_tmp581,((_tmpA89.f2=_tmp582,((_tmpA89.f3=tms,_tmpA89)))));};};};_LL2DB: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp550;if(_tmp565->tag != 1)goto _LL2DD;else{_tmp566=_tmp565->f1;_tmp567=(void*)_tmp565->f2;_tmp568=_tmp565->f3;}}_LL2DC:
# 2069
 if(_tmp567 == 0){
struct _tuple12 _tmpA8A;return(_tmpA8A.f1=tq,((_tmpA8A.f2=t,((_tmpA8A.f3=0,_tmpA8A)))));}else{
# 2072
return Cyc_Absynpp_to_tms(r,tq,_tmp567);}_LL2DD: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp569=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp550;if(_tmp569->tag != 17)goto _LL2DF;else{_tmp56A=_tmp569->f1;_tmp56B=_tmp569->f2;_tmp56C=_tmp569->f3;_tmp56D=(void*)_tmp569->f4;}}_LL2DE:
# 2077
 if((_tmp56D == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp56D)){
struct _tuple12 _tmpA8B;return(_tmpA8B.f1=tq,((_tmpA8B.f2=t,((_tmpA8B.f3=0,_tmpA8B)))));}else{
# 2080
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp56D);}_LL2DF:;_LL2E0: {
# 2085
struct _tuple12 _tmpA8C;return(_tmpA8C.f1=tq,((_tmpA8C.f2=t,((_tmpA8C.f3=0,_tmpA8C)))));}_LL2D4:;}
# 2089
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2091
void*_tmp59F=t;void*_tmp5A1;void*_tmp5A3;_LL2EE:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A0=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp59F;if(_tmp5A0->tag != 1)goto _LL2F0;else{_tmp5A1=(void*)_tmp5A0->f2;}}if(!(_tmp5A1 != 0))goto _LL2F0;_LL2EF:
 return Cyc_Absynpp_is_char_ptr(_tmp5A1);_LL2F0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A2=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59F;if(_tmp5A2->tag != 5)goto _LL2F2;else{_tmp5A3=(_tmp5A2->f1).elt_typ;}}_LL2F1:
# 2094
 L: {
void*_tmp5A4=_tmp5A3;void*_tmp5A6;void*_tmp5A8;_LL2F5:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A4;if(_tmp5A5->tag != 1)goto _LL2F7;else{_tmp5A6=(void*)_tmp5A5->f2;}}if(!(_tmp5A6 != 0))goto _LL2F7;_LL2F6:
 _tmp5A3=_tmp5A6;goto L;_LL2F7:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A4;if(_tmp5A7->tag != 17)goto _LL2F9;else{_tmp5A8=(void*)_tmp5A7->f4;}}if(!(_tmp5A8 != 0))goto _LL2F9;_LL2F8:
 _tmp5A3=_tmp5A8;goto L;_LL2F9: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A9=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A4;if(_tmp5A9->tag != 6)goto _LL2FB;else{if(_tmp5A9->f2 != Cyc_Absyn_Char_sz)goto _LL2FB;}}_LL2FA:
 return 1;_LL2FB:;_LL2FC:
 return 0;_LL2F4:;}_LL2F2:;_LL2F3:
# 2101
 return 0;_LL2ED:;}
# 2105
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5AA=_new_region("temp");struct _RegionHandle*temp=& _tmp5AA;_push_region(temp);
{struct Cyc_Absyn_Tqual _tmp5AC;void*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct _tuple12 _tmp5AB=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5AC=_tmp5AB.f1;_tmp5AD=_tmp5AB.f2;_tmp5AE=_tmp5AB.f3;
_tmp5AE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5AE);
if(_tmp5AE == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA8D[2];struct Cyc_PP_Doc*_tmp5B0=(_tmpA8D[1]=Cyc_Absynpp_ntyp2doc(_tmp5AD),((_tmpA8D[0]=Cyc_Absynpp_tqual2doc(_tmp5AC),Cyc_PP_cat(_tag_dyneither(_tmpA8D,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5B0;}else{
# 2112
const char*_tmpA90;struct Cyc_PP_Doc*_tmpA8F[4];struct Cyc_PP_Doc*_tmp5B3=
(_tmpA8F[3]=
# 2116
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5AE),((_tmpA8F[2]=
# 2115
Cyc_PP_text(((_tmpA90=" ",_tag_dyneither(_tmpA90,sizeof(char),2)))),((_tmpA8F[1]=
# 2114
Cyc_Absynpp_ntyp2doc(_tmp5AD),((_tmpA8F[0]=
# 2113
Cyc_Absynpp_tqual2doc(_tmp5AC),Cyc_PP_cat(_tag_dyneither(_tmpA8F,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B3;}}
# 2107
;_pop_region(temp);}
# 2120
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FD:
# 2123
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA97;const char*_tmpA96;const char*_tmpA95;struct Cyc_PP_Doc*_tmpA94[5];return(_tmpA94[4]=
# 2126
Cyc_PP_text(((_tmpA95=";",_tag_dyneither(_tmpA95,sizeof(char),2)))),((_tmpA94[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA94[2]=
# 2125
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA94[1]=Cyc_PP_text(((_tmpA96=":",_tag_dyneither(_tmpA96,sizeof(char),2)))),((_tmpA94[0]=
# 2124
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA97=_cycalloc(sizeof(*_tmpA97)),((_tmpA97->v=Cyc_PP_textptr(f->name),_tmpA97))))),Cyc_PP_cat(_tag_dyneither(_tmpA94,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2128
struct Cyc_Core_Opt*_tmpA9C;const char*_tmpA9B;struct Cyc_PP_Doc*_tmpA9A[3];return(_tmpA9A[2]=
Cyc_PP_text(((_tmpA9B=";",_tag_dyneither(_tmpA9B,sizeof(char),2)))),((_tmpA9A[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA9A[0]=
# 2128
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->v=Cyc_PP_textptr(f->name),_tmpA9C))))),Cyc_PP_cat(_tag_dyneither(_tmpA9A,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FE:
# 2131
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAA3;const char*_tmpAA2;const char*_tmpAA1;struct Cyc_PP_Doc*_tmpAA0[5];return(_tmpAA0[4]=
# 2134
Cyc_PP_text(((_tmpAA1=";",_tag_dyneither(_tmpAA1,sizeof(char),2)))),((_tmpAA0[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAA0[2]=Cyc_PP_text(((_tmpAA2=":",_tag_dyneither(_tmpAA2,sizeof(char),2)))),((_tmpAA0[1]=
# 2133
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA3=_cycalloc(sizeof(*_tmpAA3)),((_tmpAA3->v=Cyc_PP_textptr(f->name),_tmpAA3))))),((_tmpAA0[0]=
# 2132
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA0,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2136
struct Cyc_Core_Opt*_tmpAA8;const char*_tmpAA7;struct Cyc_PP_Doc*_tmpAA6[3];return(_tmpAA6[2]=
Cyc_PP_text(((_tmpAA7=";",_tag_dyneither(_tmpAA7,sizeof(char),2)))),((_tmpAA6[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA8=_cycalloc(sizeof(*_tmpAA8)),((_tmpAA8->v=Cyc_PP_textptr(f->name),_tmpAA8))))),((_tmpAA6[0]=
# 2136
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA6,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2142
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAA9;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAA9="",_tag_dyneither(_tmpAA9,sizeof(char),1))),fields);}
# 2146
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAAA[3];return(_tmpAAA[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAAA[1]=
# 2147
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAAA[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAAA,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2151
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAAB;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAAB=",",_tag_dyneither(_tmpAAB,sizeof(char),2))),fields);}
# 2160
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpAAE;void*_tmpAAD;(_tmpAAD=0,Cyc_fprintf(f,((_tmpAAE="\n",_tag_dyneither(_tmpAAE,sizeof(char),2))),_tag_dyneither(_tmpAAD,sizeof(void*),0)));};}}
# 2167
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpAAF;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAAF="",_tag_dyneither(_tmpAAF,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2170
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2174
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2184
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
