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
# 366
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 370
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_NonNull_to_Null  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;void*f2;struct Cyc_Absyn_Enumfield*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;struct Cyc_List_List*non_local_preds;int try_depth;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
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
# 65 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 300 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 341
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
struct Cyc_List_List*_tmp5C7;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5C7=_cycalloc(sizeof(*_tmp5C7)),((_tmp5C7->hd=v,((_tmp5C7->tl=0,_tmp5C7)))))));}
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
const char*_tmp5C8;return(_tmp5C8="\\a",_tag_dyneither(_tmp5C8,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5C9;return(_tmp5C9="\\b",_tag_dyneither(_tmp5C9,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5CA;return(_tmp5CA="\\f",_tag_dyneither(_tmp5CA,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5CB;return(_tmp5CB="\\n",_tag_dyneither(_tmp5CB,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5CC;return(_tmp5CC="\\r",_tag_dyneither(_tmp5CC,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5CD;return(_tmp5CD="\\t",_tag_dyneither(_tmp5CD,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5CE;return(_tmp5CE="\\v",_tag_dyneither(_tmp5CE,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5CF;return(_tmp5CF="\\\\",_tag_dyneither(_tmp5CF,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5D0;return(_tmp5D0="\"",_tag_dyneither(_tmp5D0,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5D1;return(_tmp5D1="\\'",_tag_dyneither(_tmp5D1,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5D4;char _tmp5D3;struct _dyneither_ptr _tmp5D2;(_tmp5D2=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5D3=*((char*)_check_dyneither_subscript(_tmp5D2,sizeof(char),0)),((_tmp5D4=c,((_get_dyneither_size(_tmp5D2,sizeof(char))== 1  && (_tmp5D3 == '\000'  && _tmp5D4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D2.curr)=_tmp5D4)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp5D7;char _tmp5D6;struct _dyneither_ptr _tmp5D5;(_tmp5D5=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5D6=*((char*)_check_dyneither_subscript(_tmp5D5,sizeof(char),0)),((_tmp5D7='\\',((_get_dyneither_size(_tmp5D5,sizeof(char))== 1  && (_tmp5D6 == '\000'  && _tmp5D7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D5.curr)=_tmp5D7)))))));}
{char _tmp5DA;char _tmp5D9;struct _dyneither_ptr _tmp5D8;(_tmp5D8=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5D9=*((char*)_check_dyneither_subscript(_tmp5D8,sizeof(char),0)),((_tmp5DA=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5D8,sizeof(char))== 1  && (_tmp5D9 == '\000'  && _tmp5DA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D8.curr)=_tmp5DA)))))));}
{char _tmp5DD;char _tmp5DC;struct _dyneither_ptr _tmp5DB;(_tmp5DB=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DC=*((char*)_check_dyneither_subscript(_tmp5DB,sizeof(char),0)),((_tmp5DD=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5DB,sizeof(char))== 1  && (_tmp5DC == '\000'  && _tmp5DD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DB.curr)=_tmp5DD)))))));}
{char _tmp5E0;char _tmp5DF;struct _dyneither_ptr _tmp5DE;(_tmp5DE=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DF=*((char*)_check_dyneither_subscript(_tmp5DE,sizeof(char),0)),((_tmp5E0=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp5DE,sizeof(char))== 1  && (_tmp5DF == '\000'  && _tmp5E0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DE.curr)=_tmp5E0)))))));}
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
{char _tmp5E3;char _tmp5E2;struct _dyneither_ptr _tmp5E1;(_tmp5E1=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E2=*((char*)_check_dyneither_subscript(_tmp5E1,sizeof(char),0)),((_tmp5E3='\\',((_get_dyneither_size(_tmp5E1,sizeof(char))== 1  && (_tmp5E2 == '\000'  && _tmp5E3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E1.curr)=_tmp5E3)))))));}{char _tmp5E6;char _tmp5E5;struct _dyneither_ptr _tmp5E4;(_tmp5E4=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E5=*((char*)_check_dyneither_subscript(_tmp5E4,sizeof(char),0)),((_tmp5E6='a',((_get_dyneither_size(_tmp5E4,sizeof(char))== 1  && (_tmp5E5 == '\000'  && _tmp5E6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E4.curr)=_tmp5E6)))))));}goto _LL21;_LL24: if(_tmp1F != '\b')goto _LL26;_LL25:
{char _tmp5E9;char _tmp5E8;struct _dyneither_ptr _tmp5E7;(_tmp5E7=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E8=*((char*)_check_dyneither_subscript(_tmp5E7,sizeof(char),0)),((_tmp5E9='\\',((_get_dyneither_size(_tmp5E7,sizeof(char))== 1  && (_tmp5E8 == '\000'  && _tmp5E9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E7.curr)=_tmp5E9)))))));}{char _tmp5EC;char _tmp5EB;struct _dyneither_ptr _tmp5EA;(_tmp5EA=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EB=*((char*)_check_dyneither_subscript(_tmp5EA,sizeof(char),0)),((_tmp5EC='b',((_get_dyneither_size(_tmp5EA,sizeof(char))== 1  && (_tmp5EB == '\000'  && _tmp5EC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EA.curr)=_tmp5EC)))))));}goto _LL21;_LL26: if(_tmp1F != '\f')goto _LL28;_LL27:
{char _tmp5EF;char _tmp5EE;struct _dyneither_ptr _tmp5ED;(_tmp5ED=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EE=*((char*)_check_dyneither_subscript(_tmp5ED,sizeof(char),0)),((_tmp5EF='\\',((_get_dyneither_size(_tmp5ED,sizeof(char))== 1  && (_tmp5EE == '\000'  && _tmp5EF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5ED.curr)=_tmp5EF)))))));}{char _tmp5F2;char _tmp5F1;struct _dyneither_ptr _tmp5F0;(_tmp5F0=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F1=*((char*)_check_dyneither_subscript(_tmp5F0,sizeof(char),0)),((_tmp5F2='f',((_get_dyneither_size(_tmp5F0,sizeof(char))== 1  && (_tmp5F1 == '\000'  && _tmp5F2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F0.curr)=_tmp5F2)))))));}goto _LL21;_LL28: if(_tmp1F != '\n')goto _LL2A;_LL29:
{char _tmp5F5;char _tmp5F4;struct _dyneither_ptr _tmp5F3;(_tmp5F3=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F4=*((char*)_check_dyneither_subscript(_tmp5F3,sizeof(char),0)),((_tmp5F5='\\',((_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (_tmp5F4 == '\000'  && _tmp5F5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=_tmp5F5)))))));}{char _tmp5F8;char _tmp5F7;struct _dyneither_ptr _tmp5F6;(_tmp5F6=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F7=*((char*)_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((_tmp5F8='n',((_get_dyneither_size(_tmp5F6,sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}goto _LL21;_LL2A: if(_tmp1F != '\r')goto _LL2C;_LL2B:
{char _tmp5FB;char _tmp5FA;struct _dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FA=*((char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((_tmp5FB='\\',((_get_dyneither_size(_tmp5F9,sizeof(char))== 1  && (_tmp5FA == '\000'  && _tmp5FB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F9.curr)=_tmp5FB)))))));}{char _tmp5FE;char _tmp5FD;struct _dyneither_ptr _tmp5FC;(_tmp5FC=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FD=*((char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((_tmp5FE='r',((_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'  && _tmp5FE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=_tmp5FE)))))));}goto _LL21;_LL2C: if(_tmp1F != '\t')goto _LL2E;_LL2D:
{char _tmp601;char _tmp600;struct _dyneither_ptr _tmp5FF;(_tmp5FF=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp600=*((char*)_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((_tmp601='\\',((_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'  && _tmp601 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}{char _tmp604;char _tmp603;struct _dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp603=*((char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((_tmp604='t',((_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'  && _tmp604 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}goto _LL21;_LL2E: if(_tmp1F != '\v')goto _LL30;_LL2F:
{char _tmp607;char _tmp606;struct _dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp606=*((char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((_tmp607='\\',((_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'  && _tmp607 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}{char _tmp60A;char _tmp609;struct _dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp609=*((char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((_tmp60A='v',((_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'  && _tmp60A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}goto _LL21;_LL30: if(_tmp1F != '\\')goto _LL32;_LL31:
{char _tmp60D;char _tmp60C;struct _dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60C=*((char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((_tmp60D='\\',((_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'  && _tmp60D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}{char _tmp610;char _tmp60F;struct _dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60F=*((char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((_tmp610='\\',((_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'  && _tmp610 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}goto _LL21;_LL32: if(_tmp1F != '"')goto _LL34;_LL33:
{char _tmp613;char _tmp612;struct _dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp612=*((char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((_tmp613='\\',((_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'  && _tmp613 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}{char _tmp616;char _tmp615;struct _dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp615=*((char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((_tmp616='"',((_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'  && _tmp616 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}goto _LL21;_LL34: _tmp20=_tmp1F;_LL35:
# 341
 if(_tmp20 >= ' '  && _tmp20 <= '~'){char _tmp619;char _tmp618;struct _dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp618=*((char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((_tmp619=_tmp20,((_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'  && _tmp619 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}else{
# 343
{char _tmp61C;char _tmp61B;struct _dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61B=*((char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((_tmp61C='\\',((_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'  && _tmp61C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}
{char _tmp61F;char _tmp61E;struct _dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61E=*((char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((_tmp61F=(char)('0' + (_tmp20 >> 6 & 7)),((_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'  && _tmp61F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}
{char _tmp622;char _tmp621;struct _dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp621=*((char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622=(char)('0' + (_tmp20 >> 3 & 7)),((_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}{
char _tmp625;char _tmp624;struct _dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp624=*((char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625=(char)('0' + (_tmp20 & 7)),((_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp626;l=((_tmp626=_cycalloc(sizeof(*_tmp626)),((_tmp626->hd=Cyc_Absynpp_restrict_sp,((_tmp626->tl=l,_tmp626))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp627;l=((_tmp627=_cycalloc(sizeof(*_tmp627)),((_tmp627->hd=Cyc_Absynpp_volatile_sp,((_tmp627->tl=l,_tmp627))))));}
if(tq.print_const){struct Cyc_List_List*_tmp628;l=((_tmp628=_cycalloc(sizeof(*_tmp628)),((_tmp628->hd=Cyc_Absynpp_const_sp,((_tmp628->tl=l,_tmp628))))));}{
const char*_tmp62B;const char*_tmp62A;const char*_tmp629;return Cyc_PP_egroup(((_tmp629="",_tag_dyneither(_tmp629,sizeof(char),1))),((_tmp62A=" ",_tag_dyneither(_tmp62A,sizeof(char),2))),((_tmp62B=" ",_tag_dyneither(_tmp62B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=_tmp6F->kind;_tmp71=_tmp6F->aliasqual;
switch(_tmp70){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp62C;return(_tmp62C="A",_tag_dyneither(_tmp62C,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp62D;return(_tmp62D="UA",_tag_dyneither(_tmp62D,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp62E;return(_tmp62E="TA",_tag_dyneither(_tmp62E,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp62F;return(_tmp62F="M",_tag_dyneither(_tmp62F,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp630;return(_tmp630="UM",_tag_dyneither(_tmp630,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp631;return(_tmp631="TM",_tag_dyneither(_tmp631,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp632;return(_tmp632="B",_tag_dyneither(_tmp632,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp633;return(_tmp633="UB",_tag_dyneither(_tmp633,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp634;return(_tmp634="TB",_tag_dyneither(_tmp634,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp635;return(_tmp635="R",_tag_dyneither(_tmp635,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp636;return(_tmp636="UR",_tag_dyneither(_tmp636,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp637;return(_tmp637="TR",_tag_dyneither(_tmp637,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp638;return(_tmp638="E",_tag_dyneither(_tmp638,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp639;return(_tmp639="I",_tag_dyneither(_tmp639,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp80=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp82;struct Cyc_Absyn_Kind*_tmp85;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp81->tag != 0)goto _LL50;else{_tmp82=_tmp81->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp82);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp83->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp63A;return(_tmp63A="{?}",_tag_dyneither(_tmp63A,sizeof(char),4));}else{
const char*_tmp63B;return(_tmp63B="?",_tag_dyneither(_tmp63B,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp84->tag != 2)goto _LL4D;else{_tmp85=_tmp84->f2;}}_LL53: {
const char*_tmp63F;void*_tmp63E[1];struct Cyc_String_pa_PrintArg_struct _tmp63D;return(struct _dyneither_ptr)((_tmp63D.tag=0,((_tmp63D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp85)),((_tmp63E[0]=& _tmp63D,Cyc_aprintf(((_tmp63F="<=%s",_tag_dyneither(_tmp63F,sizeof(char),5))),_tag_dyneither(_tmp63E,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8B=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp90;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL57;else{_tmp8D=_tmp8C->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8D));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8E->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp640;return Cyc_PP_text_width(((_tmp640="{?}",_tag_dyneither(_tmp640,sizeof(char),4))),1);}else{
const char*_tmp641;return Cyc_PP_text(((_tmp641="?",_tag_dyneither(_tmp641,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8F->tag != 2)goto _LL54;else{_tmp90=_tmp8F->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp90));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp644;const char*_tmp643;const char*_tmp642;return Cyc_PP_egroup(((_tmp642="<",_tag_dyneither(_tmp642,sizeof(char),2))),((_tmp643=">",_tag_dyneither(_tmp643,sizeof(char),2))),((_tmp644=",",_tag_dyneither(_tmp644,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp96=*tv->name;
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '#'){
const char*_tmp647;struct Cyc_PP_Doc*_tmp646[2];return(_tmp646[1]=Cyc_PP_text(_dyneither_ptr_plus(_tmp96,sizeof(char),1)),((_tmp646[0]=Cyc_PP_text(((_tmp647="`TTT",_tag_dyneither(_tmp647,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp646,sizeof(struct Cyc_PP_Doc*),2)))));}
return Cyc_PP_text(_tmp96);}
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp99=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL5E;}_LL5D:
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 0)goto _LL60;else{if((_tmp9B->f1)->kind != Cyc_Absyn_BoxKind)goto _LL60;if((_tmp9B->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61:
 _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {
const char*_tmp64A;struct Cyc_PP_Doc*_tmp649[3];return(_tmp649[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp649[1]=Cyc_PP_text(((_tmp64A="::",_tag_dyneither(_tmp64A,sizeof(char),3)))),((_tmp649[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp649,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp64D;const char*_tmp64C;const char*_tmp64B;return Cyc_PP_egroup(((_tmp64B="<",_tag_dyneither(_tmp64B,sizeof(char),2))),((_tmp64C=">",_tag_dyneither(_tmp64C,sizeof(char),2))),((_tmp64D=",",_tag_dyneither(_tmp64D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 448
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp650;const char*_tmp64F;const char*_tmp64E;return Cyc_PP_egroup(((_tmp64E="<",_tag_dyneither(_tmp64E,sizeof(char),2))),((_tmp64F=">",_tag_dyneither(_tmp64F,sizeof(char),2))),((_tmp650=",",_tag_dyneither(_tmp650,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 458
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp653;const char*_tmp652;const char*_tmp651;return Cyc_PP_group(((_tmp651="(",_tag_dyneither(_tmp651,sizeof(char),2))),((_tmp652=")",_tag_dyneither(_tmp652,sizeof(char),2))),((_tmp653=",",_tag_dyneither(_tmp653,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
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
const char*_tmp654;return Cyc_PP_text(((_tmp654=" _stdcall ",_tag_dyneither(_tmp654,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}_LL71: {
const char*_tmp655;return Cyc_PP_text(((_tmp655=" _cdecl ",_tag_dyneither(_tmp655,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp656;return Cyc_PP_text(((_tmp656=" _fastcall ",_tag_dyneither(_tmp656,sizeof(char),12))));}_LL74:;_LL75:
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
const char*_tmp661;const char*_tmp660;const char*_tmp65F;const char*_tmp65E;const char*_tmp65D;struct Cyc_PP_Doc*_tmp65C[3];return(_tmp65C[2]=
# 499
Cyc_PP_text(((_tmp65D=")",_tag_dyneither(_tmp65D,sizeof(char),2)))),((_tmp65C[1]=
# 498
Cyc_PP_group(((_tmp660="",_tag_dyneither(_tmp660,sizeof(char),1))),((_tmp65F="",_tag_dyneither(_tmp65F,sizeof(char),1))),((_tmp65E=" ",_tag_dyneither(_tmp65E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp65C[0]=
# 497
Cyc_PP_text(((_tmp661=" __declspec(",_tag_dyneither(_tmp661,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp65C,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 502
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 506
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp66A;const char*_tmp669;const char*_tmp668;const char*_tmp667;struct Cyc_PP_Doc*_tmp666[2];return(_tmp666[1]=
Cyc_PP_group(((_tmp669="((",_tag_dyneither(_tmp669,sizeof(char),3))),((_tmp668="))",_tag_dyneither(_tmp668,sizeof(char),3))),((_tmp667=",",_tag_dyneither(_tmp667,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp666[0]=
# 510
Cyc_PP_text(((_tmp66A=" __attribute__",_tag_dyneither(_tmp66A,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp666,sizeof(struct Cyc_PP_Doc*),2)))));}}}
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
const char*_tmp66B;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp66B="{?}",_tag_dyneither(_tmp66B,sizeof(char),4))),1);}else{
const char*_tmp66C;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp66C="?",_tag_dyneither(_tmp66C,sizeof(char),2))));}}
# 538
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 540
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp66D;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp66D="{\\lb}",_tag_dyneither(_tmp66D,sizeof(char),6))),1);}else{
const char*_tmp66E;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp66E="{",_tag_dyneither(_tmp66E,sizeof(char),2))));}}
# 547
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 549
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp66F;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp66F="{\\rb}",_tag_dyneither(_tmp66F,sizeof(char),6))),1);}else{
const char*_tmp670;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp670="}",_tag_dyneither(_tmp670,sizeof(char),2))));}}
# 556
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 558
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp671;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp671="\\$",_tag_dyneither(_tmp671,sizeof(char),3))),1);}else{
const char*_tmp672;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp672="$",_tag_dyneither(_tmp672,sizeof(char),2))));}}
# 565
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 567
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp673[3];return(_tmp673[2]=Cyc_Absynpp_rb(),((_tmp673[1]=Cyc_PP_seq(sep,ss),((_tmp673[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp673,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 572
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;struct Cyc_List_List*_tmpD6;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:
{const char*_tmp676;void*_tmp675;(_tmp675=0,Cyc_fprintf(Cyc_stderr,((_tmp676="Carray_mod ",_tag_dyneither(_tmp676,sizeof(char),12))),_tag_dyneither(_tmp675,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:
{const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_fprintf(Cyc_stderr,((_tmp679="ConstArray_mod ",_tag_dyneither(_tmp679,sizeof(char),16))),_tag_dyneither(_tmp678,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpD4->f1);if(_tmpD5->tag != 1)goto _LL93;else{_tmpD6=_tmpD5->f1;}}}_LL92:
# 578
{const char*_tmp67C;void*_tmp67B;(_tmp67B=0,Cyc_fprintf(Cyc_stderr,((_tmp67C="Function_mod(",_tag_dyneither(_tmp67C,sizeof(char),14))),_tag_dyneither(_tmp67B,sizeof(void*),0)));}
for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _dyneither_ptr*_tmpE1=(*((struct _tuple8*)_tmpD6->hd)).f1;
if(_tmpE1 == 0){const char*_tmp67F;void*_tmp67E;(_tmp67E=0,Cyc_fprintf(Cyc_stderr,((_tmp67F="?",_tag_dyneither(_tmp67F,sizeof(char),2))),_tag_dyneither(_tmp67E,sizeof(void*),0)));}else{
void*_tmp680;(_tmp680=0,Cyc_fprintf(Cyc_stderr,*_tmpE1,_tag_dyneither(_tmp680,sizeof(void*),0)));}
if(_tmpD6->tl != 0){const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683=",",_tag_dyneither(_tmp683,sizeof(char),2))),_tag_dyneither(_tmp682,sizeof(void*),0)));}}
# 585
{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686=") ",_tag_dyneither(_tmp686,sizeof(char),3))),_tag_dyneither(_tmp685,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD7->tag != 3)goto _LL95;}_LL94:
# 588
{const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="Function_mod()",_tag_dyneither(_tmp689,sizeof(char),15))),_tag_dyneither(_tmp688,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD8->tag != 5)goto _LL97;}_LL96:
{const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="Attributes_mod ",_tag_dyneither(_tmp68C,sizeof(char),16))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD9->tag != 4)goto _LL99;}_LL98:
{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((_tmp68F="TypeParams_mod ",_tag_dyneither(_tmp68F,sizeof(char),16))),_tag_dyneither(_tmp68E,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDA->tag != 2)goto _LL8C;}_LL9A:
{const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(Cyc_stderr,((_tmp692="Pointer_mod ",_tag_dyneither(_tmp692,sizeof(char),13))),_tag_dyneither(_tmp691,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 593
const char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_fprintf(Cyc_stderr,((_tmp695="\n",_tag_dyneither(_tmp695,sizeof(char),2))),_tag_dyneither(_tmp694,sizeof(void*),0)));};}
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp69A;const char*_tmp699;struct Cyc_PP_Doc*_tmp698[3];struct Cyc_PP_Doc*p_rest=(_tmp698[2]=Cyc_PP_text(((_tmp699=")",_tag_dyneither(_tmp699,sizeof(char),2)))),((_tmp698[1]=rest,((_tmp698[0]=Cyc_PP_text(((_tmp69A="(",_tag_dyneither(_tmp69A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp698,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmpF3=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;union Cyc_Absyn_Constraint*_tmpF8;void*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;unsigned int _tmpFF;int _tmp100;void*_tmp102;union Cyc_Absyn_Constraint*_tmp103;union Cyc_Absyn_Constraint*_tmp104;union Cyc_Absyn_Constraint*_tmp105;struct Cyc_Absyn_Tqual _tmp106;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF4->tag != 0)goto _LL9E;else{_tmpF5=_tmpF4->f1;}}_LL9D:
# 604
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp69F;const char*_tmp69E;struct Cyc_PP_Doc*_tmp69D[2];return(_tmp69D[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF5)?
Cyc_PP_text(((_tmp69E="[]@zeroterm",_tag_dyneither(_tmp69E,sizeof(char),12)))): Cyc_PP_text(((_tmp69F="[]",_tag_dyneither(_tmp69F,sizeof(char),3)))),((_tmp69D[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp69D,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF6=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF6->tag != 1)goto _LLA0;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL9F:
# 608
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A6;const char*_tmp6A5;const char*_tmp6A4;struct Cyc_PP_Doc*_tmp6A3[4];return(_tmp6A3[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8)?Cyc_PP_text(((_tmp6A4="]@zeroterm",_tag_dyneither(_tmp6A4,sizeof(char),11)))): Cyc_PP_text(((_tmp6A5="]",_tag_dyneither(_tmp6A5,sizeof(char),2)))),((_tmp6A3[2]=
# 609
Cyc_Absynpp_exp2doc(_tmpF7),((_tmp6A3[1]=Cyc_PP_text(((_tmp6A6="[",_tag_dyneither(_tmp6A6,sizeof(char),2)))),((_tmp6A3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A3,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpF9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF9->tag != 3)goto _LLA2;else{_tmpFA=(void*)_tmpF9->f1;}}_LLA1:
# 612
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp10E=_tmpFA;struct Cyc_List_List*_tmp110;int _tmp111;struct Cyc_Absyn_VarargInfo*_tmp112;void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp116;unsigned int _tmp117;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp10F=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp10F->tag != 1)goto _LLAB;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;_tmp113=(void*)_tmp10F->f4;_tmp114=_tmp10F->f5;}}_LLAA: {
# 615
struct Cyc_PP_Doc*_tmp6A7[2];return(_tmp6A7[1]=Cyc_Absynpp_funargs2doc(_tmp110,_tmp111,_tmp112,_tmp113,_tmp114),((_tmp6A7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A7,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp115=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp115->tag != 0)goto _LLA8;else{_tmp116=_tmp115->f1;_tmp117=_tmp115->f2;}}_LLAC: {
# 617
const char*_tmp6AE;const char*_tmp6AD;const char*_tmp6AC;struct Cyc_PP_Doc*_tmp6AB[2];return(_tmp6AB[1]=Cyc_PP_group(((_tmp6AE="(",_tag_dyneither(_tmp6AE,sizeof(char),2))),((_tmp6AD=")",_tag_dyneither(_tmp6AD,sizeof(char),2))),((_tmp6AC=",",_tag_dyneither(_tmp6AC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp116)),((_tmp6AB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AB,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFB->tag != 5)goto _LLA4;else{_tmpFC=_tmpFB->f2;}}_LLA3:
# 620
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 622
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6AF[2];return(_tmp6AF[1]=Cyc_Absynpp_atts2doc(_tmpFC),((_tmp6AF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AF,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 625
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6B0[2];return(_tmp6B0[1]=rest,((_tmp6B0[0]=Cyc_Absynpp_callconv2doc(_tmpFC),Cyc_PP_cat(_tag_dyneither(_tmp6B0,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFD->tag != 4)goto _LLA6;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_tmp100=_tmpFD->f3;}}_LLA5:
# 630
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp100){
struct Cyc_PP_Doc*_tmp6B1[2];return(_tmp6B1[1]=Cyc_Absynpp_ktvars2doc(_tmpFE),((_tmp6B1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 634
struct Cyc_PP_Doc*_tmp6B2[2];return(_tmp6B2[1]=Cyc_Absynpp_tvars2doc(_tmpFE),((_tmp6B2[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B2,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp101=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmp101->tag != 2)goto _LL9B;else{_tmp102=(_tmp101->f1).rgn;_tmp103=(_tmp101->f1).nullable;_tmp104=(_tmp101->f1).bounds;_tmp105=(_tmp101->f1).zero_term;_tmp106=_tmp101->f2;}}_LLA7: {
# 638
struct Cyc_PP_Doc*ptr;
{void*_tmp121=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp104);struct Cyc_Absyn_Exp*_tmp124;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp122=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp121;if(_tmp122->tag != 0)goto _LLB3;}_LLB2:
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp123=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp121;if(_tmp123->tag != 1)goto _LLB0;else{_tmp124=_tmp123->f1;}}_LLB4:
# 642
{const char*_tmp6B4;const char*_tmp6B3;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp103)?(_tmp6B4="*",_tag_dyneither(_tmp6B4,sizeof(char),2)):((_tmp6B3="@",_tag_dyneither(_tmp6B3,sizeof(char),2))));}{
unsigned int _tmp128;int _tmp129;struct _tuple11 _tmp127=Cyc_Evexp_eval_const_uint_exp(_tmp124);_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;
if(!_tmp129  || _tmp128 != 1){
struct Cyc_PP_Doc*_tmp6B5[4];ptr=((_tmp6B5[3]=Cyc_Absynpp_rb(),((_tmp6B5[2]=Cyc_Absynpp_exp2doc(_tmp124),((_tmp6B5[1]=Cyc_Absynpp_lb(),((_tmp6B5[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}
# 648
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6B8;struct Cyc_PP_Doc*_tmp6B7[2];ptr=((_tmp6B7[1]=Cyc_PP_text(((_tmp6B8="@zeroterm ",_tag_dyneither(_tmp6B8,sizeof(char),11)))),((_tmp6B7[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B7,sizeof(struct Cyc_PP_Doc*),2))))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6BB;struct Cyc_PP_Doc*_tmp6BA[2];ptr=((_tmp6BA[1]=Cyc_PP_text(((_tmp6BB="@nozeroterm ",_tag_dyneither(_tmp6BB,sizeof(char),13)))),((_tmp6BA[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 654
{void*_tmp12F=Cyc_Tcutil_compress(_tmp102);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp130=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12F;if(_tmp130->tag != 20)goto _LLB8;}_LLB7:
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp131=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12F;if(_tmp131->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
 goto _LLB5;_LLBA:;_LLBB: {
const char*_tmp6BE;struct Cyc_PP_Doc*_tmp6BD[3];ptr=((_tmp6BD[2]=Cyc_PP_text(((_tmp6BE=" ",_tag_dyneither(_tmp6BE,sizeof(char),2)))),((_tmp6BD[1]=Cyc_Absynpp_rgn2doc(_tmp102),((_tmp6BD[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 659
{struct Cyc_PP_Doc*_tmp6BF[2];ptr=((_tmp6BF[1]=Cyc_Absynpp_tqual2doc(_tmp106),((_tmp6BF[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),2))))));}{
struct Cyc_PP_Doc*_tmp6C0[2];return(_tmp6C0[1]=rest,((_tmp6C0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 664
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp139=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp139;if(_tmp13A->tag != 20)goto _LLBF;}_LLBE: {
const char*_tmp6C1;return Cyc_PP_text(((_tmp6C1="`H",_tag_dyneither(_tmp6C1,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp139;if(_tmp13B->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp6C2;return Cyc_PP_text(((_tmp6C2="`U",_tag_dyneither(_tmp6C2,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp139;if(_tmp13C->tag != 22)goto _LLC3;}_LLC2: {
const char*_tmp6C3;return Cyc_PP_text(((_tmp6C3="`RC",_tag_dyneither(_tmp6C3,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp139;if(_tmp13D->tag != 2)goto _LLC5;}_LLC4:
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
const char*_tmp6C8;const char*_tmp6C7;struct Cyc_PP_Doc*_tmp6C6[3];return(_tmp6C6[2]=Cyc_PP_text(((_tmp6C7=")",_tag_dyneither(_tmp6C7,sizeof(char),2)))),((_tmp6C6[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp6C6[0]=Cyc_PP_text(((_tmp6C8="@region(",_tag_dyneither(_tmp6C8,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6C6,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 674
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 678
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp146;struct Cyc_List_List*_tmp148;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp144;if(_tmp145->tag != 23)goto _LLCA;else{_tmp146=(void*)_tmp145->f1;}}_LLC9:
{struct Cyc_List_List*_tmp6C9;*rgions=((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9->hd=Cyc_Absynpp_rgn2doc(_tmp146),((_tmp6C9->tl=*rgions,_tmp6C9))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp144;if(_tmp147->tag != 24)goto _LLCC;else{_tmp148=_tmp147->f1;}}_LLCB:
# 681
 for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp148->hd);}
# 684
goto _LLC7;_LLCC:;_LLCD:
{struct Cyc_List_List*_tmp6CA;*effects=((_tmp6CA=_cycalloc(sizeof(*_tmp6CA)),((_tmp6CA->hd=Cyc_Absynpp_typ2doc(t),((_tmp6CA->tl=*effects,_tmp6CA))))));}goto _LLC7;_LLC7:;}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6CD;const char*_tmp6CC;const char*_tmp6CB;return Cyc_PP_group(((_tmp6CB="",_tag_dyneither(_tmp6CB,sizeof(char),1))),((_tmp6CC="",_tag_dyneither(_tmp6CC,sizeof(char),1))),((_tmp6CD="+",_tag_dyneither(_tmp6CD,sizeof(char),2))),effects);}else{
# 697
const char*_tmp6CE;struct Cyc_PP_Doc*_tmp14E=Cyc_Absynpp_group_braces(((_tmp6CE=",",_tag_dyneither(_tmp6CE,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6D2;const char*_tmp6D1;const char*_tmp6D0;const char*_tmp6CF;return Cyc_PP_group(((_tmp6CF="",_tag_dyneither(_tmp6CF,sizeof(char),1))),((_tmp6D0="",_tag_dyneither(_tmp6D0,sizeof(char),1))),((_tmp6D1="+",_tag_dyneither(_tmp6D1,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2->hd=_tmp14E,((_tmp6D2->tl=0,_tmp6D2))))))));}}
# 702
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLCE: {
const char*_tmp6D3;return Cyc_PP_text(((_tmp6D3="struct ",_tag_dyneither(_tmp6D3,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
const char*_tmp6D4;return Cyc_PP_text(((_tmp6D4="union ",_tag_dyneither(_tmp6D4,sizeof(char),7))));}}}
# 710
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp156=t;struct Cyc_Core_Opt*_tmp15C;void*_tmp15D;int _tmp15E;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;union Cyc_Absyn_DatatypeInfoU _tmp163;struct Cyc_List_List*_tmp164;union Cyc_Absyn_DatatypeFieldInfoU _tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign _tmp169;enum Cyc_Absyn_Size_of _tmp16A;int _tmp16C;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*_tmp183;void*_tmp185;void*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp192;struct Cyc_Absyn_Datatypedecl*_tmp195;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 8)goto _LLD4;}_LLD3: {
# 714
const char*_tmp6D5;return Cyc_PP_text(((_tmp6D5="[[[array]]]",_tag_dyneither(_tmp6D5,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp156;if(_tmp158->tag != 9)goto _LLD6;}_LLD5:
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp156;if(_tmp159->tag != 5)goto _LLD8;}_LLD7:
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp156;if(_tmp15A->tag != 0)goto _LLDA;}_LLD9:
# 718
{const char*_tmp6D6;s=Cyc_PP_text(((_tmp6D6="void",_tag_dyneither(_tmp6D6,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp15B->tag != 1)goto _LLDC;else{_tmp15C=_tmp15B->f1;_tmp15D=(void*)_tmp15B->f2;_tmp15E=_tmp15B->f3;_tmp15F=_tmp15B->f4;}}_LLDB:
# 720
 if(_tmp15D != 0)
# 722
return Cyc_Absynpp_ntyp2doc(_tmp15D);else{
# 730
const char*_tmp6E6;const char*_tmp6E5;const char*_tmp6E4;struct Cyc_Int_pa_PrintArg_struct _tmp6E3;void*_tmp6E2[1];const char*_tmp6E1;const char*_tmp6E0;struct Cyc_PP_Doc*_tmp6DF[6];s=((_tmp6DF[5]=
# 735
_tmp15C == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15C->v),((_tmp6DF[4]=Cyc_PP_text(((_tmp6E0=")::",_tag_dyneither(_tmp6E0,sizeof(char),4)))),((_tmp6DF[3]=
# 733
(!Cyc_Absynpp_print_full_evars  || _tmp15F == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15F->v),((_tmp6DF[2]=
# 732
Cyc_PP_text((struct _dyneither_ptr)((_tmp6E3.tag=1,((_tmp6E3.f1=(unsigned long)_tmp15E,((_tmp6E2[0]=& _tmp6E3,Cyc_aprintf(((_tmp6E1="%d",_tag_dyneither(_tmp6E1,sizeof(char),3))),_tag_dyneither(_tmp6E2,sizeof(void*),1))))))))),((_tmp6DF[1]=
# 731
Cyc_PP_text(((_tmp6E4="(",_tag_dyneither(_tmp6E4,sizeof(char),2)))),((_tmp6DF[0]=
# 730
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E5="\\%",_tag_dyneither(_tmp6E5,sizeof(char),3)))): Cyc_PP_text(((_tmp6E6="%",_tag_dyneither(_tmp6E6,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DF,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 738
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp156;if(_tmp160->tag != 2)goto _LLDE;else{_tmp161=_tmp160->f1;}}_LLDD:
# 740
 s=Cyc_PP_textptr(_tmp161->name);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6E9;struct Cyc_PP_Doc*_tmp6E8[3];s=((_tmp6E8[2]=Cyc_Absynpp_kindbound2doc(_tmp161->kind),((_tmp6E8[1]=Cyc_PP_text(((_tmp6E9="::",_tag_dyneither(_tmp6E9,sizeof(char),3)))),((_tmp6E8[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 746
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp161)){
# 749
const char*_tmp6EE;const char*_tmp6ED;struct Cyc_PP_Doc*_tmp6EC[3];s=((_tmp6EC[2]=Cyc_PP_text(((_tmp6ED=" */",_tag_dyneither(_tmp6ED,sizeof(char),4)))),((_tmp6EC[1]=s,((_tmp6EC[0]=Cyc_PP_text(((_tmp6EE="_ /* ",_tag_dyneither(_tmp6EE,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6EC,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 752
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp156;if(_tmp162->tag != 3)goto _LLE0;else{_tmp163=(_tmp162->f1).datatype_info;_tmp164=(_tmp162->f1).targs;}}_LLDF:
# 754
{union Cyc_Absyn_DatatypeInfoU _tmp1A5=_tmp163;struct _tuple0*_tmp1A6;int _tmp1A7;struct _tuple0*_tmp1A8;int _tmp1A9;_LL10D: if((_tmp1A5.UnknownDatatype).tag != 1)goto _LL10F;_tmp1A6=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).name;_tmp1A7=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).is_extensible;_LL10E:
# 756
 _tmp1A8=_tmp1A6;_tmp1A9=_tmp1A7;goto _LL110;_LL10F: if((_tmp1A5.KnownDatatype).tag != 2)goto _LL10C;_tmp1A8=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))->name;_tmp1A9=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))->is_extensible;_LL110: {
# 758
const char*_tmp6EF;struct Cyc_PP_Doc*_tmp1AA=Cyc_PP_text(((_tmp6EF="datatype ",_tag_dyneither(_tmp6EF,sizeof(char),10))));
const char*_tmp6F0;struct Cyc_PP_Doc*_tmp1AB=_tmp1A9?Cyc_PP_text(((_tmp6F0="@extensible ",_tag_dyneither(_tmp6F0,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6F1[4];s=((_tmp6F1[3]=Cyc_Absynpp_tps2doc(_tmp164),((_tmp6F1[2]=Cyc_Absynpp_qvar2doc(_tmp1A8),((_tmp6F1[1]=_tmp1AA,((_tmp6F1[0]=_tmp1AB,Cyc_PP_cat(_tag_dyneither(_tmp6F1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL10C;}_LL10C:;}
# 763
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp156;if(_tmp165->tag != 4)goto _LLE2;else{_tmp166=(_tmp165->f1).field_info;_tmp167=(_tmp165->f1).targs;}}_LLE1:
# 765
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AF=_tmp166;struct _tuple0*_tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;struct _tuple0*_tmp1B3;int _tmp1B4;struct _tuple0*_tmp1B5;_LL112: if((_tmp1AF.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).datatype_name;_tmp1B1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).field_name;_tmp1B2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).is_extensible;_LL113:
# 767
 _tmp1B3=_tmp1B0;_tmp1B4=_tmp1B2;_tmp1B5=_tmp1B1;goto _LL115;_LL114: if((_tmp1AF.KnownDatatypefield).tag != 2)goto _LL111;_tmp1B3=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1)->name;_tmp1B4=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1)->is_extensible;_tmp1B5=(((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f2)->name;_LL115: {
# 770
const char*_tmp6F3;const char*_tmp6F2;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B4?(_tmp6F3="@extensible datatype ",_tag_dyneither(_tmp6F3,sizeof(char),22)):((_tmp6F2="datatype ",_tag_dyneither(_tmp6F2,sizeof(char),10))));
{const char*_tmp6F6;struct Cyc_PP_Doc*_tmp6F5[4];s=((_tmp6F5[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp6F5[2]=Cyc_PP_text(((_tmp6F6=".",_tag_dyneither(_tmp6F6,sizeof(char),2)))),((_tmp6F5[1]=Cyc_Absynpp_qvar2doc(_tmp1B3),((_tmp6F5[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 774
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp156;if(_tmp168->tag != 6)goto _LLE4;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE3: {
# 776
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp169){case Cyc_Absyn_None: _LL116:
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
{const char*_tmp6F7;sns=((_tmp6F7="",_tag_dyneither(_tmp6F7,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
{const char*_tmp6F8;sns=((_tmp6F8="unsigned ",_tag_dyneither(_tmp6F8,sizeof(char),10)));}break;}
# 783
switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL11A:
# 785
 switch(_tmp169){case Cyc_Absyn_None: _LL11C:
{const char*_tmp6F9;sns=((_tmp6F9="",_tag_dyneither(_tmp6F9,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
{const char*_tmp6FA;sns=((_tmp6FA="signed ",_tag_dyneither(_tmp6FA,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
{const char*_tmp6FB;sns=((_tmp6FB="unsigned ",_tag_dyneither(_tmp6FB,sizeof(char),10)));}break;}
# 790
{const char*_tmp6FC;ts=((_tmp6FC="char",_tag_dyneither(_tmp6FC,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL11B:
{const char*_tmp6FD;ts=((_tmp6FD="short",_tag_dyneither(_tmp6FD,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
{const char*_tmp6FE;ts=((_tmp6FE="int",_tag_dyneither(_tmp6FE,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
{const char*_tmp6FF;ts=((_tmp6FF="long",_tag_dyneither(_tmp6FF,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 796
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
{const char*_tmp700;ts=((_tmp700="long long",_tag_dyneither(_tmp700,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
{const char*_tmp701;ts=((_tmp701="__int64",_tag_dyneither(_tmp701,sizeof(char),8)));}break;}
# 800
break;}
# 802
{const char*_tmp706;void*_tmp705[2];struct Cyc_String_pa_PrintArg_struct _tmp704;struct Cyc_String_pa_PrintArg_struct _tmp703;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp703.tag=0,((_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp704.tag=0,((_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp705[0]=& _tmp704,((_tmp705[1]=& _tmp703,Cyc_aprintf(((_tmp706="%s%s",_tag_dyneither(_tmp706,sizeof(char),5))),_tag_dyneither(_tmp705,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp156;if(_tmp16B->tag != 7)goto _LLE6;else{_tmp16C=_tmp16B->f1;}}_LLE5:
# 805
 if(_tmp16C == 0){
const char*_tmp707;s=Cyc_PP_text(((_tmp707="float",_tag_dyneither(_tmp707,sizeof(char),6))));}else{
if(_tmp16C == 1){
const char*_tmp708;s=Cyc_PP_text(((_tmp708="double",_tag_dyneither(_tmp708,sizeof(char),7))));}else{
# 810
const char*_tmp709;s=Cyc_PP_text(((_tmp709="long double",_tag_dyneither(_tmp709,sizeof(char),12))));}}
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp156;if(_tmp16D->tag != 10)goto _LLE8;else{_tmp16E=_tmp16D->f1;}}_LLE7:
# 813
{struct Cyc_PP_Doc*_tmp70A[2];s=((_tmp70A[1]=Cyc_Absynpp_args2doc(_tmp16E),((_tmp70A[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp156;if(_tmp16F->tag != 11)goto _LLEA;else{_tmp170=(_tmp16F->f1).aggr_info;_tmp171=(_tmp16F->f1).targs;}}_LLE9: {
# 816
enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*_tmp1D0;struct _tuple10 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;
{struct Cyc_PP_Doc*_tmp70B[3];s=((_tmp70B[2]=Cyc_Absynpp_tps2doc(_tmp171),((_tmp70B[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp70B[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp156;if(_tmp172->tag != 12)goto _LLEC;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LLEB:
# 820
{struct Cyc_PP_Doc*_tmp70C[4];s=((_tmp70C[3]=
# 822
Cyc_Absynpp_rb(),((_tmp70C[2]=
# 821
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp70C[1]=
# 820
Cyc_Absynpp_lb(),((_tmp70C[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 823
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp156;if(_tmp175->tag != 14)goto _LLEE;else{_tmp176=_tmp175->f1;}}_LLED:
# 825
{const char*_tmp70F;struct Cyc_PP_Doc*_tmp70E[4];s=((_tmp70E[3]=Cyc_Absynpp_rb(),((_tmp70E[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp176)),((_tmp70E[1]=Cyc_Absynpp_lb(),((_tmp70E[0]=Cyc_PP_text(((_tmp70F="enum ",_tag_dyneither(_tmp70F,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp156;if(_tmp177->tag != 13)goto _LLF0;else{_tmp178=_tmp177->f1;}}_LLEF:
# 827
{const char*_tmp712;struct Cyc_PP_Doc*_tmp711[2];s=((_tmp711[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp711[0]=Cyc_PP_text(((_tmp712="enum ",_tag_dyneither(_tmp712,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp711,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp156;if(_tmp179->tag != 18)goto _LLF2;else{_tmp17A=_tmp179->f1;}}_LLF1:
# 829
{const char*_tmp717;const char*_tmp716;struct Cyc_PP_Doc*_tmp715[3];s=((_tmp715[2]=Cyc_PP_text(((_tmp716=")",_tag_dyneither(_tmp716,sizeof(char),2)))),((_tmp715[1]=Cyc_Absynpp_exp2doc(_tmp17A),((_tmp715[0]=Cyc_PP_text(((_tmp717="valueof_t(",_tag_dyneither(_tmp717,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp715,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp156;if(_tmp17B->tag != 17)goto _LLF4;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF3:
# 836
{struct Cyc_PP_Doc*_tmp718[2];s=((_tmp718[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp718[0]=Cyc_Absynpp_qvar2doc(_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp718,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp156;if(_tmp17F->tag != 15)goto _LLF6;else{_tmp180=(void*)_tmp17F->f1;}}_LLF5:
# 839
{const char*_tmp71D;const char*_tmp71C;struct Cyc_PP_Doc*_tmp71B[3];s=((_tmp71B[2]=Cyc_PP_text(((_tmp71C=">",_tag_dyneither(_tmp71C,sizeof(char),2)))),((_tmp71B[1]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp71B[0]=Cyc_PP_text(((_tmp71D="region_t<",_tag_dyneither(_tmp71D,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp156;if(_tmp181->tag != 16)goto _LLF8;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF7:
# 841
{const char*_tmp724;const char*_tmp723;const char*_tmp722;struct Cyc_PP_Doc*_tmp721[5];s=((_tmp721[4]=
Cyc_PP_text(((_tmp722=">",_tag_dyneither(_tmp722,sizeof(char),2)))),((_tmp721[3]=
# 841
Cyc_Absynpp_rgn2doc(_tmp183),((_tmp721[2]=Cyc_PP_text(((_tmp723=",",_tag_dyneither(_tmp723,sizeof(char),2)))),((_tmp721[1]=Cyc_Absynpp_rgn2doc(_tmp182),((_tmp721[0]=Cyc_PP_text(((_tmp724="dynregion_t<",_tag_dyneither(_tmp724,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp721,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp156;if(_tmp184->tag != 19)goto _LLFA;else{_tmp185=(void*)_tmp184->f1;}}_LLF9:
# 844
{const char*_tmp729;const char*_tmp728;struct Cyc_PP_Doc*_tmp727[3];s=((_tmp727[2]=Cyc_PP_text(((_tmp728=">",_tag_dyneither(_tmp728,sizeof(char),2)))),((_tmp727[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp727[0]=Cyc_PP_text(((_tmp729="tag_t<",_tag_dyneither(_tmp729,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp727,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp156;if(_tmp186->tag != 21)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp156;if(_tmp187->tag != 20)goto _LLFE;}_LLFD:
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp156;if(_tmp188->tag != 22)goto _LL100;}_LLFF:
# 848
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp156;if(_tmp189->tag != 25)goto _LL102;else{_tmp18A=(void*)_tmp189->f1;}}_LL101:
# 850
{const char*_tmp72E;const char*_tmp72D;struct Cyc_PP_Doc*_tmp72C[3];s=((_tmp72C[2]=Cyc_PP_text(((_tmp72D=")",_tag_dyneither(_tmp72D,sizeof(char),2)))),((_tmp72C[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp72C[0]=Cyc_PP_text(((_tmp72E="regions(",_tag_dyneither(_tmp72E,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp156;if(_tmp18B->tag != 23)goto _LL104;}_LL103:
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
const char*_tmp731;struct Cyc_PP_Doc*_tmp730[3];return(_tmp730[2]=Cyc_Absynpp_rgn2doc(_tmp1EA),((_tmp730[1]=Cyc_PP_text(((_tmp731=" > ",_tag_dyneither(_tmp731,sizeof(char),4)))),((_tmp730[0]=Cyc_Absynpp_rgn2doc(_tmp1E9),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp734;const char*_tmp733;const char*_tmp732;return Cyc_PP_group(((_tmp732="",_tag_dyneither(_tmp732,sizeof(char),1))),((_tmp733="",_tag_dyneither(_tmp733,sizeof(char),1))),((_tmp734=",",_tag_dyneither(_tmp734,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1F0=(*t).f1;
struct Cyc_Core_Opt*_tmp735;struct Cyc_Core_Opt*dopt=_tmp1F0 == 0?0:((_tmp735=_cycalloc(sizeof(*_tmp735)),((_tmp735->v=Cyc_PP_text(*_tmp1F0),_tmp735))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 886
struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp738;struct Cyc_List_List*_tmp737;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737->hd=Cyc_PP_text(((_tmp738="...",_tag_dyneither(_tmp738,sizeof(char),4)))),((_tmp737->tl=0,_tmp737)))))));}else{
if(cyc_varargs != 0){
const char*_tmp741;const char*_tmp740;const char*_tmp73F;struct _tuple8*_tmp73E;struct Cyc_PP_Doc*_tmp73D[3];struct Cyc_PP_Doc*_tmp1F5=(_tmp73D[2]=
# 893
Cyc_Absynpp_funarg2doc(((_tmp73E=_cycalloc(sizeof(*_tmp73E)),((_tmp73E->f1=cyc_varargs->name,((_tmp73E->f2=cyc_varargs->tq,((_tmp73E->f3=cyc_varargs->type,_tmp73E))))))))),((_tmp73D[1]=
# 892
cyc_varargs->inject?Cyc_PP_text(((_tmp73F=" inject ",_tag_dyneither(_tmp73F,sizeof(char),9)))): Cyc_PP_text(((_tmp740=" ",_tag_dyneither(_tmp740,sizeof(char),2)))),((_tmp73D[0]=
# 891
Cyc_PP_text(((_tmp741="...",_tag_dyneither(_tmp741,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp73D,sizeof(struct Cyc_PP_Doc*),3)))))));
# 895
struct Cyc_List_List*_tmp742;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp742=_cycalloc(sizeof(*_tmp742)),((_tmp742->hd=_tmp1F5,((_tmp742->tl=0,_tmp742)))))));}}{
# 897
const char*_tmp745;const char*_tmp744;const char*_tmp743;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp743="",_tag_dyneither(_tmp743,sizeof(char),1))),((_tmp744="",_tag_dyneither(_tmp744,sizeof(char),1))),((_tmp745=",",_tag_dyneither(_tmp745,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp748;struct Cyc_PP_Doc*_tmp747[3];_tmp1FC=((_tmp747[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp747[1]=Cyc_PP_text(((_tmp748=";",_tag_dyneither(_tmp748,sizeof(char),2)))),((_tmp747[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp747,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp74B;struct Cyc_PP_Doc*_tmp74A[3];_tmp1FC=((_tmp74A[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74A[1]=Cyc_PP_text(((_tmp74B=":",_tag_dyneither(_tmp74B,sizeof(char),2)))),((_tmp74A[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp74A,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp750;const char*_tmp74F;struct Cyc_PP_Doc*_tmp74E[3];return(_tmp74E[2]=Cyc_PP_text(((_tmp74F=")",_tag_dyneither(_tmp74F,sizeof(char),2)))),((_tmp74E[1]=_tmp1FC,((_tmp74E[0]=Cyc_PP_text(((_tmp750="(",_tag_dyneither(_tmp750,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 905
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp208;struct Cyc_Absyn_Tqual _tmp209;void*_tmp20A;struct _tuple8*_tmp207=arg;_tmp208=_tmp207->f1;_tmp209=_tmp207->f2;_tmp20A=_tmp207->f3;{
struct _tuple8*_tmp751;return(_tmp751=_cycalloc(sizeof(*_tmp751)),((_tmp751->f1=_tmp208,((_tmp751->f2=_tmp209,((_tmp751->f3=_tmp20A,_tmp751)))))));};}
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
{struct Cyc_List_List*_tmp752;_tmp20C=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752->hd=Cyc_Absynpp_cyc_stringptr,((_tmp752->tl=_tmp210,_tmp752))))): _tmp210;}
goto _LL127;_LL127:;}
# 931
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp754;struct Cyc_List_List*_tmp753;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753->hd=(*q).f2,((_tmp753->tl=0,_tmp753))))))),((_tmp754="_",_tag_dyneither(_tmp754,sizeof(char),2))));}else{
# 936
if(match)
return*(*q).f2;else{
# 939
const char*_tmp756;struct Cyc_List_List*_tmp755;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755->hd=(*q).f2,((_tmp755->tl=0,_tmp755))))))),((_tmp756="::",_tag_dyneither(_tmp756,sizeof(char),3))));}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp216=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 952
const char*_tmp758;const char*_tmp757;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp757="\\textbf{",_tag_dyneither(_tmp757,sizeof(char),9))),(struct _dyneither_ptr)_tmp216),((_tmp758="}",_tag_dyneither(_tmp758,sizeof(char),2)))),(int)
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
const char*_tmp759;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp759="/* bad namespace : */ ",_tag_dyneither(_tmp759,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
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
const char*_tmp75B;const char*_tmp75A;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75A="\\textbf{",_tag_dyneither(_tmp75A,sizeof(char),9))),(struct _dyneither_ptr)_tmp21D),((_tmp75B="}",_tag_dyneither(_tmp75B,sizeof(char),2)))),(int)
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
{void*_tmp24B=e->r;union Cyc_Absyn_Cnst _tmp24D;struct _tuple0*_tmp24F;enum Cyc_Absyn_Primop _tmp251;struct Cyc_List_List*_tmp252;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Core_Opt*_tmp255;struct Cyc_Absyn_Exp*_tmp256;struct Cyc_Absyn_Exp*_tmp258;enum Cyc_Absyn_Incrementor _tmp259;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_List_List*_tmp269;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26F;void*_tmp271;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp277;void*_tmp279;struct Cyc_Absyn_Exp*_tmp27B;void*_tmp27D;int _tmp27F;struct _dyneither_ptr _tmp280;struct Cyc_Absyn_Exp*_tmp282;struct _dyneither_ptr*_tmp283;void*_tmp285;struct _dyneither_ptr*_tmp287;void*_tmp289;unsigned int _tmp28B;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Exp*_tmp28F;struct _dyneither_ptr*_tmp290;struct Cyc_Absyn_Exp*_tmp292;struct _dyneither_ptr*_tmp293;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp296;struct Cyc_List_List*_tmp298;struct _tuple8*_tmp29A;struct Cyc_List_List*_tmp29B;struct Cyc_List_List*_tmp29D;struct Cyc_Absyn_Vardecl*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A0;struct Cyc_Absyn_Exp*_tmp2A1;struct _tuple0*_tmp2A3;struct Cyc_List_List*_tmp2A4;struct Cyc_List_List*_tmp2A5;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Datatypefield*_tmp2AA;struct _tuple0*_tmp2AC;struct _tuple0*_tmp2AE;int _tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;void**_tmp2B2;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B5;struct Cyc_Absyn_Exp*_tmp2B6;struct Cyc_Core_Opt*_tmp2B8;struct Cyc_List_List*_tmp2B9;struct Cyc_Absyn_Stmt*_tmp2BB;_LL1A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp24C->tag != 0)goto _LL1A2;else{_tmp24D=_tmp24C->f1;}}_LL1A1:
 s=Cyc_Absynpp_cnst2doc(_tmp24D);goto _LL19F;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp24E->tag != 1)goto _LL1A4;else{_tmp24F=_tmp24E->f1;}}_LL1A3:
 s=Cyc_Absynpp_qvar2doc(_tmp24F);goto _LL19F;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp250->tag != 2)goto _LL1A6;else{_tmp251=_tmp250->f1;_tmp252=_tmp250->f2;}}_LL1A5:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp251,_tmp252);goto _LL19F;_LL1A6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp253->tag != 3)goto _LL1A8;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;_tmp256=_tmp253->f3;}}_LL1A7:
# 1080
{const char*_tmp760;const char*_tmp75F;struct Cyc_PP_Doc*_tmp75E[5];s=((_tmp75E[4]=
# 1084
Cyc_Absynpp_exp2doc_prec(myprec,_tmp256),((_tmp75E[3]=
# 1083
Cyc_PP_text(((_tmp75F="= ",_tag_dyneither(_tmp75F,sizeof(char),3)))),((_tmp75E[2]=
# 1082
_tmp255 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp255->v),((_tmp75E[1]=
# 1081
Cyc_PP_text(((_tmp760=" ",_tag_dyneither(_tmp760,sizeof(char),2)))),((_tmp75E[0]=
# 1080
Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),Cyc_PP_cat(_tag_dyneither(_tmp75E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1085
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp257->tag != 4)goto _LL1AA;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}_LL1A9: {
# 1087
struct Cyc_PP_Doc*_tmp2BF=Cyc_Absynpp_exp2doc_prec(myprec,_tmp258);
switch(_tmp259){case Cyc_Absyn_PreInc: _LL1F0:
{const char*_tmp763;struct Cyc_PP_Doc*_tmp762[2];s=((_tmp762[1]=_tmp2BF,((_tmp762[0]=Cyc_PP_text(((_tmp763="++",_tag_dyneither(_tmp763,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F1:
{const char*_tmp766;struct Cyc_PP_Doc*_tmp765[2];s=((_tmp765[1]=_tmp2BF,((_tmp765[0]=Cyc_PP_text(((_tmp766="--",_tag_dyneither(_tmp766,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F2:
{const char*_tmp769;struct Cyc_PP_Doc*_tmp768[2];s=((_tmp768[1]=Cyc_PP_text(((_tmp769="++",_tag_dyneither(_tmp769,sizeof(char),3)))),((_tmp768[0]=_tmp2BF,Cyc_PP_cat(_tag_dyneither(_tmp768,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F3:
{const char*_tmp76C;struct Cyc_PP_Doc*_tmp76B[2];s=((_tmp76B[1]=Cyc_PP_text(((_tmp76C="--",_tag_dyneither(_tmp76C,sizeof(char),3)))),((_tmp76B[0]=_tmp2BF,Cyc_PP_cat(_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1094
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25A->tag != 5)goto _LL1AC;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;_tmp25D=_tmp25A->f3;}}_LL1AB:
# 1096
{const char*_tmp771;const char*_tmp770;struct Cyc_PP_Doc*_tmp76F[5];s=((_tmp76F[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),((_tmp76F[3]=Cyc_PP_text(((_tmp770=" : ",_tag_dyneither(_tmp770,sizeof(char),4)))),((_tmp76F[2]=
# 1096
Cyc_Absynpp_exp2doc_prec(0,_tmp25C),((_tmp76F[1]=Cyc_PP_text(((_tmp771=" ? ",_tag_dyneither(_tmp771,sizeof(char),4)))),((_tmp76F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),Cyc_PP_cat(_tag_dyneither(_tmp76F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1098
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25E->tag != 6)goto _LL1AE;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL1AD:
# 1100
{const char*_tmp774;struct Cyc_PP_Doc*_tmp773[3];s=((_tmp773[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),((_tmp773[1]=Cyc_PP_text(((_tmp774=" && ",_tag_dyneither(_tmp774,sizeof(char),5)))),((_tmp773[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25F),Cyc_PP_cat(_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp261->tag != 7)goto _LL1B0;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;}}_LL1AF:
# 1103
{const char*_tmp777;struct Cyc_PP_Doc*_tmp776[3];s=((_tmp776[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp263),((_tmp776[1]=Cyc_PP_text(((_tmp777=" || ",_tag_dyneither(_tmp777,sizeof(char),5)))),((_tmp776[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp262),Cyc_PP_cat(_tag_dyneither(_tmp776,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp264->tag != 8)goto _LL1B2;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1B1:
# 1108
{const char*_tmp77E;const char*_tmp77D;const char*_tmp77C;struct Cyc_PP_Doc*_tmp77B[5];s=((_tmp77B[4]=Cyc_PP_text(((_tmp77C=")",_tag_dyneither(_tmp77C,sizeof(char),2)))),((_tmp77B[3]=Cyc_Absynpp_exp2doc(_tmp266),((_tmp77B[2]=Cyc_PP_text(((_tmp77D=", ",_tag_dyneither(_tmp77D,sizeof(char),3)))),((_tmp77B[1]=Cyc_Absynpp_exp2doc(_tmp265),((_tmp77B[0]=Cyc_PP_text(((_tmp77E="(",_tag_dyneither(_tmp77E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp267->tag != 9)goto _LL1B4;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}_LL1B3:
# 1111
{const char*_tmp783;const char*_tmp782;struct Cyc_PP_Doc*_tmp781[4];s=((_tmp781[3]=
# 1114
Cyc_PP_text(((_tmp782=")",_tag_dyneither(_tmp782,sizeof(char),2)))),((_tmp781[2]=
# 1113
Cyc_Absynpp_exps2doc_prec(20,_tmp269),((_tmp781[1]=
# 1112
Cyc_PP_text(((_tmp783="(",_tag_dyneither(_tmp783,sizeof(char),2)))),((_tmp781[0]=
# 1111
Cyc_Absynpp_exp2doc_prec(myprec,_tmp268),Cyc_PP_cat(_tag_dyneither(_tmp781,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1115
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26A->tag != 10)goto _LL1B6;else{_tmp26B=_tmp26A->f1;}}_LL1B5:
# 1117
{const char*_tmp786;struct Cyc_PP_Doc*_tmp785[2];s=((_tmp785[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26B),((_tmp785[0]=Cyc_PP_text(((_tmp786="throw ",_tag_dyneither(_tmp786,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp785,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26C->tag != 11)goto _LL1B8;else{_tmp26D=_tmp26C->f1;}}_LL1B7:
# 1120
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26D);
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26E->tag != 12)goto _LL1BA;else{_tmp26F=_tmp26E->f1;}}_LL1B9:
# 1123
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26F);
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp270->tag != 13)goto _LL1BC;else{_tmp271=(void*)_tmp270->f1;_tmp272=_tmp270->f2;}}_LL1BB:
# 1126
{const char*_tmp78B;const char*_tmp78A;struct Cyc_PP_Doc*_tmp789[4];s=((_tmp789[3]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp789[2]=
# 1128
Cyc_PP_text(((_tmp78A=")",_tag_dyneither(_tmp78A,sizeof(char),2)))),((_tmp789[1]=
# 1127
Cyc_Absynpp_typ2doc(_tmp271),((_tmp789[0]=
# 1126
Cyc_PP_text(((_tmp78B="(",_tag_dyneither(_tmp78B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp789,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1130
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp273->tag != 14)goto _LL1BE;else{_tmp274=_tmp273->f1;}}_LL1BD:
# 1132
{const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[2];s=((_tmp78D[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp274),((_tmp78D[0]=
# 1132
Cyc_PP_text(((_tmp78E="&",_tag_dyneither(_tmp78E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1134
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp275->tag != 15)goto _LL1C0;else{_tmp276=_tmp275->f1;_tmp277=_tmp275->f2;}}_LL1BF:
# 1136
 if(_tmp276 == 0){
const char*_tmp791;struct Cyc_PP_Doc*_tmp790[2];s=((_tmp790[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp790[0]=Cyc_PP_text(((_tmp791="new ",_tag_dyneither(_tmp791,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp790,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1139
const char*_tmp796;const char*_tmp795;struct Cyc_PP_Doc*_tmp794[4];s=((_tmp794[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp794[2]=
# 1139
Cyc_PP_text(((_tmp795=") ",_tag_dyneither(_tmp795,sizeof(char),3)))),((_tmp794[1]=Cyc_Absynpp_exp2doc(_tmp276),((_tmp794[0]=Cyc_PP_text(((_tmp796="rnew(",_tag_dyneither(_tmp796,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp794,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1141
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp278->tag != 16)goto _LL1C2;else{_tmp279=(void*)_tmp278->f1;}}_LL1C1:
{const char*_tmp79B;const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[3];s=((_tmp799[2]=Cyc_PP_text(((_tmp79A=")",_tag_dyneither(_tmp79A,sizeof(char),2)))),((_tmp799[1]=Cyc_Absynpp_typ2doc(_tmp279),((_tmp799[0]=Cyc_PP_text(((_tmp79B="sizeof(",_tag_dyneither(_tmp79B,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27A->tag != 17)goto _LL1C4;else{_tmp27B=_tmp27A->f1;}}_LL1C3:
{const char*_tmp7A0;const char*_tmp79F;struct Cyc_PP_Doc*_tmp79E[3];s=((_tmp79E[2]=Cyc_PP_text(((_tmp79F=")",_tag_dyneither(_tmp79F,sizeof(char),2)))),((_tmp79E[1]=Cyc_Absynpp_exp2doc(_tmp27B),((_tmp79E[0]=Cyc_PP_text(((_tmp7A0="sizeof(",_tag_dyneither(_tmp7A0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27C->tag != 37)goto _LL1C6;else{_tmp27D=(void*)_tmp27C->f1;}}_LL1C5:
{const char*_tmp7A5;const char*_tmp7A4;struct Cyc_PP_Doc*_tmp7A3[3];s=((_tmp7A3[2]=Cyc_PP_text(((_tmp7A4=")",_tag_dyneither(_tmp7A4,sizeof(char),2)))),((_tmp7A3[1]=Cyc_Absynpp_typ2doc(_tmp27D),((_tmp7A3[0]=Cyc_PP_text(((_tmp7A5="valueof(",_tag_dyneither(_tmp7A5,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27E->tag != 38)goto _LL1C8;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}_LL1C7:
# 1146
 if(_tmp27F){
const char*_tmp7AC;const char*_tmp7AB;const char*_tmp7AA;struct Cyc_PP_Doc*_tmp7A9[4];s=((_tmp7A9[3]=Cyc_PP_text(((_tmp7AA=")",_tag_dyneither(_tmp7AA,sizeof(char),2)))),((_tmp7A9[2]=Cyc_PP_text(_tmp280),((_tmp7A9[1]=Cyc_PP_text(((_tmp7AB=" volatile (",_tag_dyneither(_tmp7AB,sizeof(char),12)))),((_tmp7A9[0]=Cyc_PP_text(((_tmp7AC="__asm__",_tag_dyneither(_tmp7AC,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A9,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1149
const char*_tmp7B1;const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[3];s=((_tmp7AF[2]=Cyc_PP_text(((_tmp7B0=")",_tag_dyneither(_tmp7B0,sizeof(char),2)))),((_tmp7AF[1]=Cyc_PP_text(_tmp280),((_tmp7AF[0]=Cyc_PP_text(((_tmp7B1="__asm__(",_tag_dyneither(_tmp7B1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp281->tag != 36)goto _LL1CA;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1C9:
# 1152
{const char*_tmp7B8;const char*_tmp7B7;const char*_tmp7B6;struct Cyc_PP_Doc*_tmp7B5[5];s=((_tmp7B5[4]=Cyc_PP_text(((_tmp7B6=")",_tag_dyneither(_tmp7B6,sizeof(char),2)))),((_tmp7B5[3]=Cyc_PP_textptr(_tmp283),((_tmp7B5[2]=Cyc_PP_text(((_tmp7B7=".",_tag_dyneither(_tmp7B7,sizeof(char),2)))),((_tmp7B5[1]=Cyc_Absynpp_exp2doc(_tmp282),((_tmp7B5[0]=Cyc_PP_text(((_tmp7B8="tagcheck(",_tag_dyneither(_tmp7B8,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp284->tag != 18)goto _LL1CC;else{_tmp285=(void*)_tmp284->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp286=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp284->f2);if(_tmp286->tag != 0)goto _LL1CC;else{_tmp287=_tmp286->f1;}};}}_LL1CB:
# 1155
{const char*_tmp7BF;const char*_tmp7BE;const char*_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[5];s=((_tmp7BC[4]=Cyc_PP_text(((_tmp7BD=")",_tag_dyneither(_tmp7BD,sizeof(char),2)))),((_tmp7BC[3]=Cyc_PP_textptr(_tmp287),((_tmp7BC[2]=Cyc_PP_text(((_tmp7BE=",",_tag_dyneither(_tmp7BE,sizeof(char),2)))),((_tmp7BC[1]=Cyc_Absynpp_typ2doc(_tmp285),((_tmp7BC[0]=Cyc_PP_text(((_tmp7BF="offsetof(",_tag_dyneither(_tmp7BF,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BC,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp288->tag != 18)goto _LL1CE;else{_tmp289=(void*)_tmp288->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp28A=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp288->f2);if(_tmp28A->tag != 1)goto _LL1CE;else{_tmp28B=_tmp28A->f1;}};}}_LL1CD:
# 1158
{const char*_tmp7CD;const char*_tmp7CC;struct Cyc_Int_pa_PrintArg_struct _tmp7CB;void*_tmp7CA[1];const char*_tmp7C9;const char*_tmp7C8;struct Cyc_PP_Doc*_tmp7C7[5];s=((_tmp7C7[4]=Cyc_PP_text(((_tmp7C8=")",_tag_dyneither(_tmp7C8,sizeof(char),2)))),((_tmp7C7[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp7CB.tag=1,((_tmp7CB.f1=(unsigned long)((int)_tmp28B),((_tmp7CA[0]=& _tmp7CB,Cyc_aprintf(((_tmp7C9="%d",_tag_dyneither(_tmp7C9,sizeof(char),3))),_tag_dyneither(_tmp7CA,sizeof(void*),1))))))))),((_tmp7C7[2]=Cyc_PP_text(((_tmp7CC=",",_tag_dyneither(_tmp7CC,sizeof(char),2)))),((_tmp7C7[1]=Cyc_Absynpp_typ2doc(_tmp289),((_tmp7C7[0]=Cyc_PP_text(((_tmp7CD="offsetof(",_tag_dyneither(_tmp7CD,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp28C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28C->tag != 19)goto _LL1D0;else{_tmp28D=_tmp28C->f1;}}_LL1CF:
# 1161
{const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[2];s=((_tmp7CF[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),((_tmp7CF[0]=Cyc_PP_text(((_tmp7D0="*",_tag_dyneither(_tmp7D0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp28E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28E->tag != 20)goto _LL1D2;else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;}}_LL1D1:
# 1164
{const char*_tmp7D3;struct Cyc_PP_Doc*_tmp7D2[3];s=((_tmp7D2[2]=Cyc_PP_textptr(_tmp290),((_tmp7D2[1]=Cyc_PP_text(((_tmp7D3=".",_tag_dyneither(_tmp7D3,sizeof(char),2)))),((_tmp7D2[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F),Cyc_PP_cat(_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp291=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp291->tag != 21)goto _LL1D4;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}}_LL1D3:
# 1167
{const char*_tmp7D6;struct Cyc_PP_Doc*_tmp7D5[3];s=((_tmp7D5[2]=Cyc_PP_textptr(_tmp293),((_tmp7D5[1]=Cyc_PP_text(((_tmp7D6="->",_tag_dyneither(_tmp7D6,sizeof(char),3)))),((_tmp7D5[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),Cyc_PP_cat(_tag_dyneither(_tmp7D5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp294->tag != 22)goto _LL1D6;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1D5:
# 1170
{const char*_tmp7DB;const char*_tmp7DA;struct Cyc_PP_Doc*_tmp7D9[4];s=((_tmp7D9[3]=
# 1173
Cyc_PP_text(((_tmp7DA="]",_tag_dyneither(_tmp7DA,sizeof(char),2)))),((_tmp7D9[2]=
# 1172
Cyc_Absynpp_exp2doc(_tmp296),((_tmp7D9[1]=
# 1171
Cyc_PP_text(((_tmp7DB="[",_tag_dyneither(_tmp7DB,sizeof(char),2)))),((_tmp7D9[0]=
# 1170
Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),Cyc_PP_cat(_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1174
goto _LL19F;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp297->tag != 23)goto _LL1D8;else{_tmp298=_tmp297->f1;}}_LL1D7:
# 1176
{const char*_tmp7E0;const char*_tmp7DF;struct Cyc_PP_Doc*_tmp7DE[4];s=((_tmp7DE[3]=
# 1179
Cyc_PP_text(((_tmp7DF=")",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DE[2]=
# 1178
Cyc_Absynpp_exps2doc_prec(20,_tmp298),((_tmp7DE[1]=
# 1177
Cyc_PP_text(((_tmp7E0="(",_tag_dyneither(_tmp7E0,sizeof(char),2)))),((_tmp7DE[0]=
# 1176
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7DE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1180
goto _LL19F;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp299->tag != 24)goto _LL1DA;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;}}_LL1D9:
# 1182
{const char*_tmp7E7;const char*_tmp7E6;const char*_tmp7E5;struct Cyc_PP_Doc*_tmp7E4[4];s=((_tmp7E4[3]=
# 1185
Cyc_Absynpp_group_braces(((_tmp7E5=",",_tag_dyneither(_tmp7E5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29B)),((_tmp7E4[2]=
# 1184
Cyc_PP_text(((_tmp7E6=")",_tag_dyneither(_tmp7E6,sizeof(char),2)))),((_tmp7E4[1]=
# 1183
Cyc_Absynpp_typ2doc((*_tmp29A).f3),((_tmp7E4[0]=
# 1182
Cyc_PP_text(((_tmp7E7="(",_tag_dyneither(_tmp7E7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1186
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp29C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp29C->tag != 25)goto _LL1DC;else{_tmp29D=_tmp29C->f1;}}_LL1DB:
# 1188
{const char*_tmp7E8;s=Cyc_Absynpp_group_braces(((_tmp7E8=",",_tag_dyneither(_tmp7E8,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29D));}
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp29E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp29E->tag != 26)goto _LL1DE;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;_tmp2A1=_tmp29E->f3;}}_LL1DD:
# 1191
{const char*_tmp7EF;const char*_tmp7EE;const char*_tmp7ED;struct Cyc_PP_Doc*_tmp7EC[8];s=((_tmp7EC[7]=
# 1197
Cyc_Absynpp_rb(),((_tmp7EC[6]=
# 1196
Cyc_Absynpp_exp2doc(_tmp2A1),((_tmp7EC[5]=
# 1195
Cyc_PP_text(((_tmp7ED=" : ",_tag_dyneither(_tmp7ED,sizeof(char),4)))),((_tmp7EC[4]=
# 1194
Cyc_Absynpp_exp2doc(_tmp2A0),((_tmp7EC[3]=
# 1193
Cyc_PP_text(((_tmp7EE=" < ",_tag_dyneither(_tmp7EE,sizeof(char),4)))),((_tmp7EC[2]=
# 1192
Cyc_PP_text(*(*_tmp29F->name).f2),((_tmp7EC[1]=
# 1191
Cyc_PP_text(((_tmp7EF="for ",_tag_dyneither(_tmp7EF,sizeof(char),5)))),((_tmp7EC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7EC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1198
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A2->tag != 27)goto _LL1E0;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;_tmp2A5=_tmp2A2->f3;}}_LL1DF: {
# 1200
struct Cyc_List_List*_tmp316=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A5);
{const char*_tmp7F4;struct Cyc_List_List*_tmp7F3;struct Cyc_PP_Doc*_tmp7F2[2];s=((_tmp7F2[1]=
Cyc_Absynpp_group_braces(((_tmp7F4=",",_tag_dyneither(_tmp7F4,sizeof(char),2))),
_tmp2A4 != 0?(_tmp7F3=_cycalloc(sizeof(*_tmp7F3)),((_tmp7F3->hd=Cyc_Absynpp_tps2doc(_tmp2A4),((_tmp7F3->tl=_tmp316,_tmp7F3))))): _tmp316),((_tmp7F2[0]=
# 1201
Cyc_Absynpp_qvar2doc(_tmp2A3),Cyc_PP_cat(_tag_dyneither(_tmp7F2,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1204
goto _LL19F;}_LL1E0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A6->tag != 28)goto _LL1E2;else{_tmp2A7=_tmp2A6->f2;}}_LL1E1:
# 1206
{const char*_tmp7F5;s=Cyc_Absynpp_group_braces(((_tmp7F5=",",_tag_dyneither(_tmp7F5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A7));}
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A8->tag != 29)goto _LL1E4;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f3;}}_LL1E3:
# 1209
 if(_tmp2A9 == 0)
# 1211
s=Cyc_Absynpp_qvar2doc(_tmp2AA->name);else{
# 1213
const char*_tmp7FC;const char*_tmp7FB;const char*_tmp7FA;struct Cyc_PP_Doc*_tmp7F9[2];s=((_tmp7F9[1]=
Cyc_PP_egroup(((_tmp7FC="(",_tag_dyneither(_tmp7FC,sizeof(char),2))),((_tmp7FB=")",_tag_dyneither(_tmp7FB,sizeof(char),2))),((_tmp7FA=",",_tag_dyneither(_tmp7FA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A9)),((_tmp7F9[0]=
# 1213
Cyc_Absynpp_qvar2doc(_tmp2AA->name),Cyc_PP_cat(_tag_dyneither(_tmp7F9,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1215
goto _LL19F;_LL1E4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2AB->tag != 30)goto _LL1E6;else{_tmp2AC=_tmp2AB->f1;}}_LL1E5:
# 1217
 s=Cyc_Absynpp_qvar2doc(_tmp2AC);
goto _LL19F;_LL1E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2AD=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2AD->tag != 31)goto _LL1E8;else{_tmp2AE=_tmp2AD->f1;}}_LL1E7:
# 1220
 s=Cyc_Absynpp_qvar2doc(_tmp2AE);
goto _LL19F;_LL1E8: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AF=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2AF->tag != 32)goto _LL1EA;else{_tmp2B0=(_tmp2AF->f1).is_calloc;_tmp2B1=(_tmp2AF->f1).rgn;_tmp2B2=(_tmp2AF->f1).elt_type;_tmp2B3=(_tmp2AF->f1).num_elts;}}_LL1E9:
# 1225
 if(_tmp2B0){
# 1227
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2B2)),0);
if(_tmp2B1 == 0){
const char*_tmp803;const char*_tmp802;const char*_tmp801;struct Cyc_PP_Doc*_tmp800[5];s=((_tmp800[4]=Cyc_PP_text(((_tmp801=")",_tag_dyneither(_tmp801,sizeof(char),2)))),((_tmp800[3]=Cyc_Absynpp_exp2doc(st),((_tmp800[2]=Cyc_PP_text(((_tmp802=",",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp800[1]=Cyc_Absynpp_exp2doc(_tmp2B3),((_tmp800[0]=Cyc_PP_text(((_tmp803="calloc(",_tag_dyneither(_tmp803,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp800,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1231
const char*_tmp80C;const char*_tmp80B;const char*_tmp80A;const char*_tmp809;struct Cyc_PP_Doc*_tmp808[7];s=((_tmp808[6]=
Cyc_PP_text(((_tmp809=")",_tag_dyneither(_tmp809,sizeof(char),2)))),((_tmp808[5]=Cyc_Absynpp_exp2doc(st),((_tmp808[4]=Cyc_PP_text(((_tmp80A=",",_tag_dyneither(_tmp80A,sizeof(char),2)))),((_tmp808[3]=Cyc_Absynpp_exp2doc(_tmp2B3),((_tmp808[2]=
# 1231
Cyc_PP_text(((_tmp80B=",",_tag_dyneither(_tmp80B,sizeof(char),2)))),((_tmp808[1]=Cyc_Absynpp_exp2doc(_tmp2B1),((_tmp808[0]=Cyc_PP_text(((_tmp80C="rcalloc(",_tag_dyneither(_tmp80C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp808,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1234
struct Cyc_Absyn_Exp*new_e;
# 1236
if(_tmp2B2 == 0)
new_e=_tmp2B3;else{
# 1239
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2B2,0),_tmp2B3,0);}
# 1241
if(_tmp2B1 == 0){
const char*_tmp811;const char*_tmp810;struct Cyc_PP_Doc*_tmp80F[3];s=((_tmp80F[2]=Cyc_PP_text(((_tmp810=")",_tag_dyneither(_tmp810,sizeof(char),2)))),((_tmp80F[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp80F[0]=Cyc_PP_text(((_tmp811="malloc(",_tag_dyneither(_tmp811,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp80F,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1244
const char*_tmp818;const char*_tmp817;const char*_tmp816;struct Cyc_PP_Doc*_tmp815[5];s=((_tmp815[4]=
Cyc_PP_text(((_tmp816=")",_tag_dyneither(_tmp816,sizeof(char),2)))),((_tmp815[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp815[2]=
# 1244
Cyc_PP_text(((_tmp817=",",_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp815[1]=Cyc_Absynpp_exp2doc(_tmp2B1),((_tmp815[0]=Cyc_PP_text(((_tmp818="rmalloc(",_tag_dyneither(_tmp818,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp815,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1247
goto _LL19F;_LL1EA: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B4->tag != 33)goto _LL1EC;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}}_LL1EB:
# 1249
{const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[3];s=((_tmp81A[2]=
# 1251
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B6),((_tmp81A[1]=
# 1250
Cyc_PP_text(((_tmp81B=" :=: ",_tag_dyneither(_tmp81B,sizeof(char),6)))),((_tmp81A[0]=
# 1249
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5),Cyc_PP_cat(_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1252
goto _LL19F;_LL1EC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B7->tag != 34)goto _LL1EE;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;}}_LL1ED:
# 1255
{const char*_tmp81C;s=Cyc_Absynpp_group_braces(((_tmp81C=",",_tag_dyneither(_tmp81C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B9));}
goto _LL19F;_LL1EE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2BA->tag != 35)goto _LL19F;else{_tmp2BB=_tmp2BA->f1;}}_LL1EF:
# 1258
{const char*_tmp821;const char*_tmp820;struct Cyc_PP_Doc*_tmp81F[7];s=((_tmp81F[6]=
# 1260
Cyc_PP_text(((_tmp820=")",_tag_dyneither(_tmp820,sizeof(char),2)))),((_tmp81F[5]=Cyc_Absynpp_rb(),((_tmp81F[4]=Cyc_PP_blank_doc(),((_tmp81F[3]=
# 1259
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2BB,1)),((_tmp81F[2]=
# 1258
Cyc_PP_blank_doc(),((_tmp81F[1]=Cyc_Absynpp_lb(),((_tmp81F[0]=Cyc_PP_text(((_tmp821="(",_tag_dyneither(_tmp821,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp81F,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1261
goto _LL19F;_LL19F:;}
# 1263
if(inprec >= myprec){
const char*_tmp826;const char*_tmp825;struct Cyc_PP_Doc*_tmp824[3];s=((_tmp824[2]=Cyc_PP_text(((_tmp825=")",_tag_dyneither(_tmp825,sizeof(char),2)))),((_tmp824[1]=s,((_tmp824[0]=Cyc_PP_text(((_tmp826="(",_tag_dyneither(_tmp826,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp824,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1268
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp338=d;struct Cyc_Absyn_Exp*_tmp33A;struct _dyneither_ptr*_tmp33C;_LL1F6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp339=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp338;if(_tmp339->tag != 0)goto _LL1F8;else{_tmp33A=_tmp339->f1;}}_LL1F7: {
const char*_tmp82B;const char*_tmp82A;struct Cyc_PP_Doc*_tmp829[3];return(_tmp829[2]=Cyc_PP_text(((_tmp82A="]",_tag_dyneither(_tmp82A,sizeof(char),2)))),((_tmp829[1]=Cyc_Absynpp_exp2doc(_tmp33A),((_tmp829[0]=Cyc_PP_text(((_tmp82B=".[",_tag_dyneither(_tmp82B,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp338;if(_tmp33B->tag != 1)goto _LL1F5;else{_tmp33C=_tmp33B->f1;}}_LL1F9: {
const char*_tmp82E;struct Cyc_PP_Doc*_tmp82D[2];return(_tmp82D[1]=Cyc_PP_textptr(_tmp33C),((_tmp82D[0]=Cyc_PP_text(((_tmp82E=".",_tag_dyneither(_tmp82E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82D,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F5:;}
# 1275
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp835;const char*_tmp834;const char*_tmp833;struct Cyc_PP_Doc*_tmp832[2];return(_tmp832[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp832[0]=
# 1277
Cyc_PP_egroup(((_tmp835="",_tag_dyneither(_tmp835,sizeof(char),1))),((_tmp834="=",_tag_dyneither(_tmp834,sizeof(char),2))),((_tmp833="=",_tag_dyneither(_tmp833,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp832,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1281
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp838;const char*_tmp837;const char*_tmp836;return Cyc_PP_group(((_tmp836="",_tag_dyneither(_tmp836,sizeof(char),1))),((_tmp837="",_tag_dyneither(_tmp837,sizeof(char),1))),((_tmp838=",",_tag_dyneither(_tmp838,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1286
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp349=c;enum Cyc_Absyn_Sign _tmp34A;char _tmp34B;struct _dyneither_ptr _tmp34C;enum Cyc_Absyn_Sign _tmp34D;short _tmp34E;int _tmp34F;int _tmp350;int _tmp351;enum Cyc_Absyn_Sign _tmp352;long long _tmp353;struct _dyneither_ptr _tmp354;struct _dyneither_ptr _tmp355;struct _dyneither_ptr _tmp356;_LL1FB: if((_tmp349.Char_c).tag != 2)goto _LL1FD;_tmp34A=((struct _tuple3)(_tmp349.Char_c).val).f1;_tmp34B=((struct _tuple3)(_tmp349.Char_c).val).f2;_LL1FC: {
const char*_tmp83C;void*_tmp83B[1];struct Cyc_String_pa_PrintArg_struct _tmp83A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83A.tag=0,((_tmp83A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34B)),((_tmp83B[0]=& _tmp83A,Cyc_aprintf(((_tmp83C="'%s'",_tag_dyneither(_tmp83C,sizeof(char),5))),_tag_dyneither(_tmp83B,sizeof(void*),1)))))))));}_LL1FD: if((_tmp349.Wchar_c).tag != 3)goto _LL1FF;_tmp34C=(struct _dyneither_ptr)(_tmp349.Wchar_c).val;_LL1FE: {
const char*_tmp840;void*_tmp83F[1];struct Cyc_String_pa_PrintArg_struct _tmp83E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83E.tag=0,((_tmp83E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34C),((_tmp83F[0]=& _tmp83E,Cyc_aprintf(((_tmp840="L'%s'",_tag_dyneither(_tmp840,sizeof(char),6))),_tag_dyneither(_tmp83F,sizeof(void*),1)))))))));}_LL1FF: if((_tmp349.Short_c).tag != 4)goto _LL201;_tmp34D=((struct _tuple4)(_tmp349.Short_c).val).f1;_tmp34E=((struct _tuple4)(_tmp349.Short_c).val).f2;_LL200: {
const char*_tmp844;void*_tmp843[1];struct Cyc_Int_pa_PrintArg_struct _tmp842;return Cyc_PP_text((struct _dyneither_ptr)((_tmp842.tag=1,((_tmp842.f1=(unsigned long)((int)_tmp34E),((_tmp843[0]=& _tmp842,Cyc_aprintf(((_tmp844="%d",_tag_dyneither(_tmp844,sizeof(char),3))),_tag_dyneither(_tmp843,sizeof(void*),1)))))))));}_LL201: if((_tmp349.Int_c).tag != 5)goto _LL203;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_None)goto _LL203;_tmp34F=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL202:
 _tmp350=_tmp34F;goto _LL204;_LL203: if((_tmp349.Int_c).tag != 5)goto _LL205;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL205;_tmp350=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL204: {
const char*_tmp848;void*_tmp847[1];struct Cyc_Int_pa_PrintArg_struct _tmp846;return Cyc_PP_text((struct _dyneither_ptr)((_tmp846.tag=1,((_tmp846.f1=(unsigned long)_tmp350,((_tmp847[0]=& _tmp846,Cyc_aprintf(((_tmp848="%d",_tag_dyneither(_tmp848,sizeof(char),3))),_tag_dyneither(_tmp847,sizeof(void*),1)))))))));}_LL205: if((_tmp349.Int_c).tag != 5)goto _LL207;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL207;_tmp351=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL206: {
const char*_tmp84C;void*_tmp84B[1];struct Cyc_Int_pa_PrintArg_struct _tmp84A;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84A.tag=1,((_tmp84A.f1=(unsigned int)_tmp351,((_tmp84B[0]=& _tmp84A,Cyc_aprintf(((_tmp84C="%u",_tag_dyneither(_tmp84C,sizeof(char),3))),_tag_dyneither(_tmp84B,sizeof(void*),1)))))))));}_LL207: if((_tmp349.LongLong_c).tag != 6)goto _LL209;_tmp352=((struct _tuple6)(_tmp349.LongLong_c).val).f1;_tmp353=((struct _tuple6)(_tmp349.LongLong_c).val).f2;_LL208: {
# 1295
const char*_tmp84D;return Cyc_PP_text(((_tmp84D="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84D,sizeof(char),27))));}_LL209: if((_tmp349.Float_c).tag != 7)goto _LL20B;_tmp354=((struct _tuple7)(_tmp349.Float_c).val).f1;_LL20A:
 return Cyc_PP_text(_tmp354);_LL20B: if((_tmp349.Null_c).tag != 1)goto _LL20D;_LL20C: {
const char*_tmp84E;return Cyc_PP_text(((_tmp84E="NULL",_tag_dyneither(_tmp84E,sizeof(char),5))));}_LL20D: if((_tmp349.String_c).tag != 8)goto _LL20F;_tmp355=(struct _dyneither_ptr)(_tmp349.String_c).val;_LL20E: {
# 1299
const char*_tmp853;const char*_tmp852;struct Cyc_PP_Doc*_tmp851[3];return(_tmp851[2]=Cyc_PP_text(((_tmp852="\"",_tag_dyneither(_tmp852,sizeof(char),2)))),((_tmp851[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp355)),((_tmp851[0]=Cyc_PP_text(((_tmp853="\"",_tag_dyneither(_tmp853,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp851,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20F: if((_tmp349.Wstring_c).tag != 9)goto _LL1FA;_tmp356=(struct _dyneither_ptr)(_tmp349.Wstring_c).val;_LL210: {
# 1301
const char*_tmp858;const char*_tmp857;struct Cyc_PP_Doc*_tmp856[3];return(_tmp856[2]=Cyc_PP_text(((_tmp857="\"",_tag_dyneither(_tmp857,sizeof(char),2)))),((_tmp856[1]=Cyc_PP_text(_tmp356),((_tmp856[0]=Cyc_PP_text(((_tmp858="L\"",_tag_dyneither(_tmp858,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp856,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FA:;}
# 1305
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp865;const char*_tmp864;void*_tmp863[1];struct Cyc_String_pa_PrintArg_struct _tmp862;struct Cyc_Core_Failure_exn_struct*_tmp861;(int)_throw((void*)((_tmp861=_cycalloc(sizeof(*_tmp861)),((_tmp861[0]=((_tmp865.tag=Cyc_Core_Failure,((_tmp865.f1=(struct _dyneither_ptr)((_tmp862.tag=0,((_tmp862.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp863[0]=& _tmp862,Cyc_aprintf(((_tmp864="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp864,sizeof(char),47))),_tag_dyneither(_tmp863,sizeof(void*),1)))))))),_tmp865)))),_tmp861)))));}{
# 1311
const char*_tmp86A;const char*_tmp869;struct Cyc_PP_Doc*_tmp868[3];return(_tmp868[2]=Cyc_PP_text(((_tmp869=")",_tag_dyneither(_tmp869,sizeof(char),2)))),((_tmp868[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp868[0]=Cyc_PP_text(((_tmp86A="numelts(",_tag_dyneither(_tmp86A,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp868,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1313
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp877;const char*_tmp876;void*_tmp875[1];struct Cyc_String_pa_PrintArg_struct _tmp874;struct Cyc_Core_Failure_exn_struct*_tmp873;(int)_throw((void*)((_tmp873=_cycalloc(sizeof(*_tmp873)),((_tmp873[0]=((_tmp877.tag=Cyc_Core_Failure,((_tmp877.f1=(struct _dyneither_ptr)((_tmp874.tag=0,((_tmp874.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp875[0]=& _tmp874,Cyc_aprintf(((_tmp876="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp876,sizeof(char),38))),_tag_dyneither(_tmp875,sizeof(void*),1)))))))),_tmp877)))),_tmp873)))));}else{
# 1317
if(ds->tl == 0){
const char*_tmp87A;struct Cyc_PP_Doc*_tmp879[3];return(_tmp879[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp879[1]=Cyc_PP_text(((_tmp87A=" ",_tag_dyneither(_tmp87A,sizeof(char),2)))),((_tmp879[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp879,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp887;const char*_tmp886;void*_tmp885[1];struct Cyc_String_pa_PrintArg_struct _tmp884;struct Cyc_Core_Failure_exn_struct*_tmp883;(int)_throw((void*)((_tmp883=_cycalloc(sizeof(*_tmp883)),((_tmp883[0]=((_tmp887.tag=Cyc_Core_Failure,((_tmp887.f1=(struct _dyneither_ptr)((_tmp884.tag=0,((_tmp884.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp885[0]=& _tmp884,Cyc_aprintf(((_tmp886="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp886,sizeof(char),47))),_tag_dyneither(_tmp885,sizeof(void*),1)))))))),_tmp887)))),_tmp883)))));}else{
# 1323
const char*_tmp88C;const char*_tmp88B;struct Cyc_PP_Doc*_tmp88A[5];return(_tmp88A[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp88A[3]=Cyc_PP_text(((_tmp88B=" ",_tag_dyneither(_tmp88B,sizeof(char),2)))),((_tmp88A[2]=ps,((_tmp88A[1]=Cyc_PP_text(((_tmp88C=" ",_tag_dyneither(_tmp88C,sizeof(char),2)))),((_tmp88A[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp88A,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1327
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL211: {
const char*_tmp88D;return(_tmp88D="+",_tag_dyneither(_tmp88D,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {
const char*_tmp88E;return(_tmp88E="*",_tag_dyneither(_tmp88E,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
const char*_tmp88F;return(_tmp88F="-",_tag_dyneither(_tmp88F,sizeof(char),2));}case Cyc_Absyn_Div: _LL214: {
const char*_tmp890;return(_tmp890="/",_tag_dyneither(_tmp890,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {
const char*_tmp892;const char*_tmp891;return Cyc_Absynpp_print_for_cycdoc?(_tmp892="\\%",_tag_dyneither(_tmp892,sizeof(char),3)):((_tmp891="%",_tag_dyneither(_tmp891,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL216: {
const char*_tmp893;return(_tmp893="==",_tag_dyneither(_tmp893,sizeof(char),3));}case Cyc_Absyn_Neq: _LL217: {
const char*_tmp894;return(_tmp894="!=",_tag_dyneither(_tmp894,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {
const char*_tmp895;return(_tmp895=">",_tag_dyneither(_tmp895,sizeof(char),2));}case Cyc_Absyn_Lt: _LL219: {
const char*_tmp896;return(_tmp896="<",_tag_dyneither(_tmp896,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21A: {
const char*_tmp897;return(_tmp897=">=",_tag_dyneither(_tmp897,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {
const char*_tmp898;return(_tmp898="<=",_tag_dyneither(_tmp898,sizeof(char),3));}case Cyc_Absyn_Not: _LL21C: {
const char*_tmp899;return(_tmp899="!",_tag_dyneither(_tmp899,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL21D: {
const char*_tmp89A;return(_tmp89A="~",_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {
const char*_tmp89B;return(_tmp89B="&",_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL21F: {
const char*_tmp89C;return(_tmp89C="|",_tag_dyneither(_tmp89C,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL220: {
const char*_tmp89D;return(_tmp89D="^",_tag_dyneither(_tmp89D,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {
const char*_tmp89E;return(_tmp89E="<<",_tag_dyneither(_tmp89E,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL222: {
const char*_tmp89F;return(_tmp89F=">>",_tag_dyneither(_tmp89F,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {
const char*_tmp8A0;return(_tmp8A0=">>>",_tag_dyneither(_tmp8A0,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL224: {
const char*_tmp8A1;return(_tmp8A1="numelts",_tag_dyneither(_tmp8A1,sizeof(char),8));}}}
# 1352
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1356
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp39A=s->r;_LL227: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp39B=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp39B->tag != 12)goto _LL229;}_LL228:
 return 1;_LL229:;_LL22A:
 return 0;_LL226:;}
# 1363
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp39C=st->r;struct Cyc_Absyn_Exp*_tmp39F;struct Cyc_Absyn_Stmt*_tmp3A1;struct Cyc_Absyn_Stmt*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A6;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3AB;struct _dyneither_ptr*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_Absyn_Stmt*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_List_List*_tmp3B7;struct Cyc_List_List*_tmp3BA;struct Cyc_Absyn_Decl*_tmp3BC;struct Cyc_Absyn_Stmt*_tmp3BD;struct _dyneither_ptr*_tmp3BF;struct Cyc_Absyn_Stmt*_tmp3C0;struct Cyc_Absyn_Stmt*_tmp3C2;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C5;struct Cyc_List_List*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C8;_LL22C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp39D=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp39D->tag != 0)goto _LL22E;}_LL22D:
{const char*_tmp8A2;s=Cyc_PP_text(((_tmp8A2=";",_tag_dyneither(_tmp8A2,sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp39E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp39E->tag != 1)goto _LL230;else{_tmp39F=_tmp39E->f1;}}_LL22F:
{const char*_tmp8A5;struct Cyc_PP_Doc*_tmp8A4[2];s=((_tmp8A4[1]=Cyc_PP_text(((_tmp8A5=";",_tag_dyneither(_tmp8A5,sizeof(char),2)))),((_tmp8A4[0]=Cyc_Absynpp_exp2doc(_tmp39F),Cyc_PP_cat(_tag_dyneither(_tmp8A4,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3A0=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A0->tag != 2)goto _LL232;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;}}_LL231:
# 1369
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3A1)){
struct Cyc_PP_Doc*_tmp8B0[5];struct Cyc_PP_Doc*_tmp8AF[7];const char*_tmp8AE;const char*_tmp8AD;struct Cyc_PP_Doc*_tmp8AC[7];s=((_tmp8AC[6]=
# 1377
Cyc_Absynpp_is_declaration(_tmp3A2)?
stmtexp?
(_tmp8AF[6]=
# 1382
Cyc_PP_line_doc(),((_tmp8AF[5]=
# 1381
Cyc_PP_text(((_tmp8AE=");",_tag_dyneither(_tmp8AE,sizeof(char),3)))),((_tmp8AF[4]=Cyc_Absynpp_rb(),((_tmp8AF[3]=
# 1380
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8AF[2]=
# 1379
Cyc_PP_blank_doc(),((_tmp8AF[1]=Cyc_Absynpp_lb(),((_tmp8AF[0]=Cyc_PP_text(((_tmp8AD="(",_tag_dyneither(_tmp8AD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8AF,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1383
(_tmp8B0[4]=
# 1386
Cyc_PP_line_doc(),((_tmp8B0[3]=
# 1385
Cyc_Absynpp_rb(),((_tmp8B0[2]=
# 1384
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8B0[1]=
# 1383
Cyc_PP_blank_doc(),((_tmp8B0[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B0,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1387
 Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8AC[5]=
# 1376
Cyc_PP_line_doc(),((_tmp8AC[4]=
# 1375
Cyc_Absynpp_rb(),((_tmp8AC[3]=
# 1374
Cyc_PP_line_doc(),((_tmp8AC[2]=
# 1373
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A1,0)),((_tmp8AC[1]=
# 1372
Cyc_PP_blank_doc(),((_tmp8AC[0]=
# 1371
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8AC,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1388
if(Cyc_Absynpp_is_declaration(_tmp3A2)){
struct Cyc_PP_Doc*_tmp8BB[4];struct Cyc_PP_Doc*_tmp8BA[6];const char*_tmp8B9;const char*_tmp8B8;struct Cyc_PP_Doc*_tmp8B7[4];s=((_tmp8B7[3]=
# 1399
Cyc_PP_line_doc(),((_tmp8B7[2]=
# 1391
stmtexp?
(_tmp8BA[5]=
# 1394
Cyc_PP_text(((_tmp8B9=");",_tag_dyneither(_tmp8B9,sizeof(char),3)))),((_tmp8BA[4]=Cyc_Absynpp_rb(),((_tmp8BA[3]=
# 1393
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8BA[2]=
# 1392
Cyc_PP_blank_doc(),((_tmp8BA[1]=Cyc_Absynpp_lb(),((_tmp8BA[0]=Cyc_PP_text(((_tmp8B8="(",_tag_dyneither(_tmp8B8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1396
(_tmp8BB[3]=
# 1398
Cyc_Absynpp_rb(),((_tmp8BB[2]=
# 1397
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8BB[1]=
# 1396
Cyc_PP_blank_doc(),((_tmp8BB[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8B7[1]=
# 1390
Cyc_PP_line_doc(),((_tmp8B7[0]=
# 1389
Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1401
struct Cyc_PP_Doc*_tmp8BC[3];s=((_tmp8BC[2]=Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8BC[1]=Cyc_PP_line_doc(),((_tmp8BC[0]=Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1404
struct Cyc_PP_Doc*_tmp8BD[3];s=((_tmp8BD[2]=Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8BD[1]=Cyc_PP_line_doc(),((_tmp8BD[0]=Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8BD,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A3=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A3->tag != 3)goto _LL234;else{_tmp3A4=_tmp3A3->f1;}}_LL233:
# 1407
 if(_tmp3A4 == 0){
const char*_tmp8BE;s=Cyc_PP_text(((_tmp8BE="return;",_tag_dyneither(_tmp8BE,sizeof(char),8))));}else{
# 1410
const char*_tmp8C3;const char*_tmp8C2;struct Cyc_PP_Doc*_tmp8C1[3];s=((_tmp8C1[2]=
# 1412
Cyc_PP_text(((_tmp8C2=";",_tag_dyneither(_tmp8C2,sizeof(char),2)))),((_tmp8C1[1]=
# 1411
_tmp3A4 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3A4),((_tmp8C1[0]=
# 1410
Cyc_PP_text(((_tmp8C3="return ",_tag_dyneither(_tmp8C3,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8C1,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1413
goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3A5=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A5->tag != 4)goto _LL236;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_tmp3A8=_tmp3A5->f3;}}_LL235: {
# 1415
int print_else;
{void*_tmp3DC=_tmp3A8->r;_LL251: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3DC;if(_tmp3DD->tag != 0)goto _LL253;}_LL252:
 print_else=0;goto _LL250;_LL253:;_LL254:
 print_else=1;goto _LL250;_LL250:;}
# 1420
{const char*_tmp8D2;const char*_tmp8D1;struct Cyc_PP_Doc*_tmp8D0[2];struct Cyc_PP_Doc*_tmp8CF[6];struct Cyc_PP_Doc*_tmp8CE[2];const char*_tmp8CD;struct Cyc_PP_Doc*_tmp8CC[8];s=((_tmp8CC[7]=
# 1427
print_else?
(_tmp8CF[5]=
# 1433
Cyc_Absynpp_rb(),((_tmp8CF[4]=
# 1432
Cyc_PP_line_doc(),((_tmp8CF[3]=
# 1431
Cyc_PP_nest(2,((_tmp8CE[1]=Cyc_Absynpp_stmt2doc(_tmp3A8,0),((_tmp8CE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CF[2]=
# 1430
Cyc_Absynpp_lb(),((_tmp8CF[1]=
# 1429
Cyc_PP_text(((_tmp8CD="else ",_tag_dyneither(_tmp8CD,sizeof(char),6)))),((_tmp8CF[0]=
# 1428
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CF,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1434
 Cyc_PP_nil_doc(),((_tmp8CC[6]=
# 1426
Cyc_Absynpp_rb(),((_tmp8CC[5]=
# 1425
Cyc_PP_line_doc(),((_tmp8CC[4]=
# 1424
Cyc_PP_nest(2,((_tmp8D0[1]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),((_tmp8D0[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CC[3]=
# 1423
Cyc_Absynpp_lb(),((_tmp8CC[2]=
# 1422
Cyc_PP_text(((_tmp8D1=") ",_tag_dyneither(_tmp8D1,sizeof(char),3)))),((_tmp8CC[1]=
# 1421
Cyc_Absynpp_exp2doc(_tmp3A6),((_tmp8CC[0]=
# 1420
Cyc_PP_text(((_tmp8D2="if (",_tag_dyneither(_tmp8D2,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1435
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3A9=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A9->tag != 5)goto _LL238;else{_tmp3AA=(_tmp3A9->f1).f1;_tmp3AB=_tmp3A9->f2;}}_LL237:
# 1437
{const char*_tmp8D9;const char*_tmp8D8;struct Cyc_PP_Doc*_tmp8D7[2];struct Cyc_PP_Doc*_tmp8D6[7];s=((_tmp8D6[6]=
# 1443
Cyc_Absynpp_rb(),((_tmp8D6[5]=
# 1442
Cyc_PP_line_doc(),((_tmp8D6[4]=
# 1441
Cyc_PP_nest(2,((_tmp8D7[1]=Cyc_Absynpp_stmt2doc(_tmp3AB,0),((_tmp8D7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D6[3]=
# 1440
Cyc_Absynpp_lb(),((_tmp8D6[2]=
# 1439
Cyc_PP_text(((_tmp8D8=") ",_tag_dyneither(_tmp8D8,sizeof(char),3)))),((_tmp8D6[1]=
# 1438
Cyc_Absynpp_exp2doc(_tmp3AA),((_tmp8D6[0]=
# 1437
Cyc_PP_text(((_tmp8D9="while (",_tag_dyneither(_tmp8D9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D6,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1444
goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3AC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AC->tag != 6)goto _LL23A;}_LL239:
{const char*_tmp8DA;s=Cyc_PP_text(((_tmp8DA="break;",_tag_dyneither(_tmp8DA,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3AD=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AD->tag != 7)goto _LL23C;}_LL23B:
{const char*_tmp8DB;s=Cyc_PP_text(((_tmp8DB="continue;",_tag_dyneither(_tmp8DB,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3AE=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AE->tag != 8)goto _LL23E;else{_tmp3AF=_tmp3AE->f1;}}_LL23D:
{const char*_tmp8DF;void*_tmp8DE[1];struct Cyc_String_pa_PrintArg_struct _tmp8DD;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DD.tag=0,((_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF),((_tmp8DE[0]=& _tmp8DD,Cyc_aprintf(((_tmp8DF="goto %s;",_tag_dyneither(_tmp8DF,sizeof(char),9))),_tag_dyneither(_tmp8DE,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3B0=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B0->tag != 9)goto _LL240;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=(_tmp3B0->f2).f1;_tmp3B3=(_tmp3B0->f3).f1;_tmp3B4=_tmp3B0->f4;}}_LL23F:
# 1449
{const char*_tmp8EA;const char*_tmp8E9;const char*_tmp8E8;const char*_tmp8E7;struct Cyc_PP_Doc*_tmp8E6[2];struct Cyc_PP_Doc*_tmp8E5[11];s=((_tmp8E5[10]=
# 1459
Cyc_Absynpp_rb(),((_tmp8E5[9]=
# 1458
Cyc_PP_line_doc(),((_tmp8E5[8]=
# 1457
Cyc_PP_nest(2,((_tmp8E6[1]=Cyc_Absynpp_stmt2doc(_tmp3B4,0),((_tmp8E6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E5[7]=
# 1456
Cyc_Absynpp_lb(),((_tmp8E5[6]=
# 1455
Cyc_PP_text(((_tmp8E7=") ",_tag_dyneither(_tmp8E7,sizeof(char),3)))),((_tmp8E5[5]=
# 1454
Cyc_Absynpp_exp2doc(_tmp3B3),((_tmp8E5[4]=
# 1453
Cyc_PP_text(((_tmp8E8="; ",_tag_dyneither(_tmp8E8,sizeof(char),3)))),((_tmp8E5[3]=
# 1452
Cyc_Absynpp_exp2doc(_tmp3B2),((_tmp8E5[2]=
# 1451
Cyc_PP_text(((_tmp8E9="; ",_tag_dyneither(_tmp8E9,sizeof(char),3)))),((_tmp8E5[1]=
# 1450
Cyc_Absynpp_exp2doc(_tmp3B1),((_tmp8E5[0]=
# 1449
Cyc_PP_text(((_tmp8EA="for(",_tag_dyneither(_tmp8EA,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E5,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1460
goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3B5=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B5->tag != 10)goto _LL242;else{_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;}}_LL241:
# 1462
{const char*_tmp8EF;const char*_tmp8EE;struct Cyc_PP_Doc*_tmp8ED[8];s=((_tmp8ED[7]=
# 1469
Cyc_Absynpp_rb(),((_tmp8ED[6]=
# 1468
Cyc_PP_line_doc(),((_tmp8ED[5]=
# 1467
Cyc_Absynpp_switchclauses2doc(_tmp3B7),((_tmp8ED[4]=
# 1466
Cyc_PP_line_doc(),((_tmp8ED[3]=
# 1465
Cyc_Absynpp_lb(),((_tmp8ED[2]=
# 1464
Cyc_PP_text(((_tmp8EE=") ",_tag_dyneither(_tmp8EE,sizeof(char),3)))),((_tmp8ED[1]=
# 1463
Cyc_Absynpp_exp2doc(_tmp3B6),((_tmp8ED[0]=
# 1462
Cyc_PP_text(((_tmp8EF="switch (",_tag_dyneither(_tmp8EF,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8ED,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1470
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B8->tag != 11)goto _LL244;else{if(_tmp3B8->f1 != 0)goto _LL244;}}_LL243:
{const char*_tmp8F0;s=Cyc_PP_text(((_tmp8F0="fallthru;",_tag_dyneither(_tmp8F0,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B9->tag != 11)goto _LL246;else{_tmp3BA=_tmp3B9->f1;}}_LL245:
# 1473
{const char*_tmp8F5;const char*_tmp8F4;struct Cyc_PP_Doc*_tmp8F3[3];s=((_tmp8F3[2]=Cyc_PP_text(((_tmp8F4=");",_tag_dyneither(_tmp8F4,sizeof(char),3)))),((_tmp8F3[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3BA),((_tmp8F3[0]=Cyc_PP_text(((_tmp8F5="fallthru(",_tag_dyneither(_tmp8F5,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3BB=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3BB->tag != 12)goto _LL248;else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;}}_LL247:
# 1475
{struct Cyc_PP_Doc*_tmp8F6[3];s=((_tmp8F6[2]=
# 1477
Cyc_Absynpp_stmt2doc(_tmp3BD,stmtexp),((_tmp8F6[1]=
# 1476
Cyc_PP_line_doc(),((_tmp8F6[0]=
# 1475
Cyc_Absynpp_decl2doc(_tmp3BC),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1478
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3BE=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3BE->tag != 13)goto _LL24A;else{_tmp3BF=_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;}}_LL249:
# 1480
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C0)){
if(stmtexp){
const char*_tmp8FB;const char*_tmp8FA;struct Cyc_PP_Doc*_tmp8F9[8];s=((_tmp8F9[7]=
# 1488
Cyc_PP_text(((_tmp8FA=");",_tag_dyneither(_tmp8FA,sizeof(char),3)))),((_tmp8F9[6]=Cyc_Absynpp_rb(),((_tmp8F9[5]=
# 1487
Cyc_PP_line_doc(),((_tmp8F9[4]=
# 1486
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,1)),((_tmp8F9[3]=
# 1485
Cyc_PP_line_doc(),((_tmp8F9[2]=
# 1484
Cyc_Absynpp_lb(),((_tmp8F9[1]=
# 1483
Cyc_PP_text(((_tmp8FB=": (",_tag_dyneither(_tmp8FB,sizeof(char),4)))),((_tmp8F9[0]=
# 1482
Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp8F9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1490
const char*_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[7];s=((_tmp8FD[6]=
# 1496
Cyc_Absynpp_rb(),((_tmp8FD[5]=
# 1495
Cyc_PP_line_doc(),((_tmp8FD[4]=
# 1494
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,0)),((_tmp8FD[3]=
# 1493
Cyc_PP_line_doc(),((_tmp8FD[2]=
# 1492
Cyc_Absynpp_lb(),((_tmp8FD[1]=
# 1491
Cyc_PP_text(((_tmp8FE=": ",_tag_dyneither(_tmp8FE,sizeof(char),3)))),((_tmp8FD[0]=
# 1490
Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp8FD,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1498
const char*_tmp901;struct Cyc_PP_Doc*_tmp900[3];s=((_tmp900[2]=Cyc_Absynpp_stmt2doc(_tmp3C0,stmtexp),((_tmp900[1]=Cyc_PP_text(((_tmp901=": ",_tag_dyneither(_tmp901,sizeof(char),3)))),((_tmp900[0]=Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp900,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3C1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C1->tag != 14)goto _LL24C;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=(_tmp3C1->f2).f1;}}_LL24B:
# 1501
{const char*_tmp908;const char*_tmp907;const char*_tmp906;struct Cyc_PP_Doc*_tmp905[9];s=((_tmp905[8]=
# 1509
Cyc_PP_text(((_tmp906=");",_tag_dyneither(_tmp906,sizeof(char),3)))),((_tmp905[7]=
# 1508
Cyc_Absynpp_exp2doc(_tmp3C3),((_tmp905[6]=
# 1507
Cyc_PP_text(((_tmp907=" while (",_tag_dyneither(_tmp907,sizeof(char),9)))),((_tmp905[5]=
# 1506
Cyc_Absynpp_rb(),((_tmp905[4]=
# 1505
Cyc_PP_line_doc(),((_tmp905[3]=
# 1504
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C2,0)),((_tmp905[2]=
# 1503
Cyc_PP_line_doc(),((_tmp905[1]=
# 1502
Cyc_Absynpp_lb(),((_tmp905[0]=
# 1501
Cyc_PP_text(((_tmp908="do ",_tag_dyneither(_tmp908,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp905,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1510
goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3C4=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C4->tag != 15)goto _LL24E;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL24D:
# 1512
{const char*_tmp90D;const char*_tmp90C;struct Cyc_PP_Doc*_tmp90B[12];s=((_tmp90B[11]=
# 1523
Cyc_Absynpp_rb(),((_tmp90B[10]=
# 1522
Cyc_PP_line_doc(),((_tmp90B[9]=
# 1521
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3C6)),((_tmp90B[8]=
# 1520
Cyc_PP_line_doc(),((_tmp90B[7]=
# 1519
Cyc_Absynpp_lb(),((_tmp90B[6]=
# 1518
Cyc_PP_text(((_tmp90C=" catch ",_tag_dyneither(_tmp90C,sizeof(char),8)))),((_tmp90B[5]=
# 1517
Cyc_Absynpp_rb(),((_tmp90B[4]=
# 1516
Cyc_PP_line_doc(),((_tmp90B[3]=
# 1515
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C5,0)),((_tmp90B[2]=
# 1514
Cyc_PP_line_doc(),((_tmp90B[1]=
# 1513
Cyc_Absynpp_lb(),((_tmp90B[0]=
# 1512
Cyc_PP_text(((_tmp90D="try ",_tag_dyneither(_tmp90D,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1524
goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3C7=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C7->tag != 16)goto _LL22B;else{_tmp3C8=_tmp3C7->f1;}}_LL24F:
# 1526
{const char*_tmp912;const char*_tmp911;struct Cyc_PP_Doc*_tmp910[3];s=((_tmp910[2]=Cyc_PP_text(((_tmp911=");",_tag_dyneither(_tmp911,sizeof(char),3)))),((_tmp910[1]=Cyc_Absynpp_exp2doc(_tmp3C8),((_tmp910[0]=Cyc_PP_text(((_tmp912="reset_region(",_tag_dyneither(_tmp912,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL22B:;}
# 1529
return s;}
# 1532
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp40D=p->r;enum Cyc_Absyn_Sign _tmp411;int _tmp412;char _tmp414;struct _dyneither_ptr _tmp416;struct Cyc_Absyn_Vardecl*_tmp418;struct Cyc_Absyn_Vardecl*_tmp41B;struct Cyc_Absyn_Pat*_tmp41C;struct Cyc_Absyn_Tvar*_tmp41E;struct Cyc_Absyn_Vardecl*_tmp41F;struct Cyc_List_List*_tmp421;int _tmp422;struct Cyc_Absyn_Pat*_tmp424;struct Cyc_Absyn_Vardecl*_tmp426;struct Cyc_Absyn_Vardecl*_tmp429;struct Cyc_Absyn_Pat*_tmp42A;struct _tuple0*_tmp42C;struct _tuple0*_tmp42E;struct Cyc_List_List*_tmp42F;int _tmp430;union Cyc_Absyn_AggrInfoU _tmp432;struct Cyc_List_List*_tmp433;struct Cyc_List_List*_tmp434;int _tmp435;struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp438;int _tmp439;struct Cyc_Absyn_Enumfield*_tmp43B;struct Cyc_Absyn_Enumfield*_tmp43D;struct Cyc_Absyn_Datatypefield*_tmp43F;struct Cyc_Absyn_Datatypefield*_tmp441;struct Cyc_List_List*_tmp442;int _tmp443;struct Cyc_Absyn_Exp*_tmp445;_LL256: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp40E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp40E->tag != 0)goto _LL258;}_LL257:
{const char*_tmp913;s=Cyc_PP_text(((_tmp913="_",_tag_dyneither(_tmp913,sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp40F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp40F->tag != 8)goto _LL25A;}_LL259:
{const char*_tmp914;s=Cyc_PP_text(((_tmp914="NULL",_tag_dyneither(_tmp914,sizeof(char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp410=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp410->tag != 9)goto _LL25C;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;}}_LL25B:
# 1538
 if(_tmp411 != Cyc_Absyn_Unsigned){
const char*_tmp918;void*_tmp917[1];struct Cyc_Int_pa_PrintArg_struct _tmp916;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp916.tag=1,((_tmp916.f1=(unsigned long)_tmp412,((_tmp917[0]=& _tmp916,Cyc_aprintf(((_tmp918="%d",_tag_dyneither(_tmp918,sizeof(char),3))),_tag_dyneither(_tmp917,sizeof(void*),1)))))))));}else{
const char*_tmp91C;void*_tmp91B[1];struct Cyc_Int_pa_PrintArg_struct _tmp91A;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91A.tag=1,((_tmp91A.f1=(unsigned int)_tmp412,((_tmp91B[0]=& _tmp91A,Cyc_aprintf(((_tmp91C="%u",_tag_dyneither(_tmp91C,sizeof(char),3))),_tag_dyneither(_tmp91B,sizeof(void*),1)))))))));}
goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp413=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp413->tag != 10)goto _LL25E;else{_tmp414=_tmp413->f1;}}_LL25D:
{const char*_tmp920;void*_tmp91F[1];struct Cyc_String_pa_PrintArg_struct _tmp91E;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91E.tag=0,((_tmp91E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp414)),((_tmp91F[0]=& _tmp91E,Cyc_aprintf(((_tmp920="'%s'",_tag_dyneither(_tmp920,sizeof(char),5))),_tag_dyneither(_tmp91F,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp415=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp415->tag != 11)goto _LL260;else{_tmp416=_tmp415->f1;}}_LL25F:
 s=Cyc_PP_text(_tmp416);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp417=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp417->tag != 1)goto _LL262;else{_tmp418=_tmp417->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp419=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp417->f2)->r;if(_tmp419->tag != 0)goto _LL262;};}}_LL261:
# 1545
 s=Cyc_Absynpp_qvar2doc(_tmp418->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp41A->tag != 1)goto _LL264;else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL263:
# 1548
{const char*_tmp923;struct Cyc_PP_Doc*_tmp922[3];s=((_tmp922[2]=Cyc_Absynpp_pat2doc(_tmp41C),((_tmp922[1]=Cyc_PP_text(((_tmp923=" as ",_tag_dyneither(_tmp923,sizeof(char),5)))),((_tmp922[0]=Cyc_Absynpp_qvar2doc(_tmp41B->name),Cyc_PP_cat(_tag_dyneither(_tmp922,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp41D=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp41D->tag != 3)goto _LL266;else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL265:
# 1550
{const char*_tmp928;const char*_tmp927;struct Cyc_PP_Doc*_tmp926[4];s=((_tmp926[3]=Cyc_PP_text(((_tmp927=">",_tag_dyneither(_tmp927,sizeof(char),2)))),((_tmp926[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp41E)),((_tmp926[1]=Cyc_PP_text(((_tmp928="<",_tag_dyneither(_tmp928,sizeof(char),2)))),((_tmp926[0]=Cyc_Absynpp_qvar2doc(_tmp41F->name),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL266: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp420=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp420->tag != 4)goto _LL268;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}_LL267:
# 1553
{const char*_tmp931;const char*_tmp930;const char*_tmp92F;const char*_tmp92E;struct Cyc_PP_Doc*_tmp92D[4];s=((_tmp92D[3]=
_tmp422?Cyc_PP_text(((_tmp92E=", ...)",_tag_dyneither(_tmp92E,sizeof(char),7)))): Cyc_PP_text(((_tmp92F=")",_tag_dyneither(_tmp92F,sizeof(char),2)))),((_tmp92D[2]=
# 1553
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp930=",",_tag_dyneither(_tmp930,sizeof(char),2))),_tmp421),((_tmp92D[1]=Cyc_PP_text(((_tmp931="(",_tag_dyneither(_tmp931,sizeof(char),2)))),((_tmp92D[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp92D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1555
goto _LL255;_LL268: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp423=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp423->tag != 5)goto _LL26A;else{_tmp424=_tmp423->f1;}}_LL269:
# 1557
{const char*_tmp934;struct Cyc_PP_Doc*_tmp933[2];s=((_tmp933[1]=Cyc_Absynpp_pat2doc(_tmp424),((_tmp933[0]=Cyc_PP_text(((_tmp934="&",_tag_dyneither(_tmp934,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp425=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp425->tag != 2)goto _LL26C;else{_tmp426=_tmp425->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp427=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp425->f2)->r;if(_tmp427->tag != 0)goto _LL26C;};}}_LL26B:
# 1560
{const char*_tmp937;struct Cyc_PP_Doc*_tmp936[2];s=((_tmp936[1]=Cyc_Absynpp_qvar2doc(_tmp426->name),((_tmp936[0]=Cyc_PP_text(((_tmp937="*",_tag_dyneither(_tmp937,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp936,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp428=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp428->tag != 2)goto _LL26E;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL26D:
# 1563
{const char*_tmp93C;const char*_tmp93B;struct Cyc_PP_Doc*_tmp93A[4];s=((_tmp93A[3]=Cyc_Absynpp_pat2doc(_tmp42A),((_tmp93A[2]=Cyc_PP_text(((_tmp93B=" as ",_tag_dyneither(_tmp93B,sizeof(char),5)))),((_tmp93A[1]=Cyc_Absynpp_qvar2doc(_tmp429->name),((_tmp93A[0]=Cyc_PP_text(((_tmp93C="*",_tag_dyneither(_tmp93C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp42B=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp42B->tag != 14)goto _LL270;else{_tmp42C=_tmp42B->f1;}}_LL26F:
# 1566
 s=Cyc_Absynpp_qvar2doc(_tmp42C);
goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp42D=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp42D->tag != 15)goto _LL272;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;_tmp430=_tmp42D->f3;}}_LL271: {
# 1569
const char*_tmp93E;const char*_tmp93D;struct _dyneither_ptr term=_tmp430?(_tmp93E=", ...)",_tag_dyneither(_tmp93E,sizeof(char),7)):((_tmp93D=")",_tag_dyneither(_tmp93D,sizeof(char),2)));
{const char*_tmp943;const char*_tmp942;struct Cyc_PP_Doc*_tmp941[2];s=((_tmp941[1]=Cyc_PP_group(((_tmp943="(",_tag_dyneither(_tmp943,sizeof(char),2))),term,((_tmp942=",",_tag_dyneither(_tmp942,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp42F)),((_tmp941[0]=Cyc_Absynpp_qvar2doc(_tmp42E),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL272: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp431=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp431->tag != 6)goto _LL274;else{if(_tmp431->f1 == 0)goto _LL274;_tmp432=(_tmp431->f1)->aggr_info;_tmp433=_tmp431->f2;_tmp434=_tmp431->f3;_tmp435=_tmp431->f4;}}_LL273: {
# 1573
const char*_tmp945;const char*_tmp944;struct _dyneither_ptr term=_tmp435?(_tmp945=", ...}",_tag_dyneither(_tmp945,sizeof(char),7)):((_tmp944="}",_tag_dyneither(_tmp944,sizeof(char),2)));
struct _tuple0*_tmp468;struct _tuple10 _tmp467=Cyc_Absyn_aggr_kinded_name(_tmp432);_tmp468=_tmp467.f2;
{const char*_tmp950;const char*_tmp94F;const char*_tmp94E;const char*_tmp94D;const char*_tmp94C;struct Cyc_PP_Doc*_tmp94B[4];s=((_tmp94B[3]=
# 1577
Cyc_PP_group(((_tmp94D="",_tag_dyneither(_tmp94D,sizeof(char),1))),term,((_tmp94C=",",_tag_dyneither(_tmp94C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp434)),((_tmp94B[2]=
# 1576
Cyc_PP_egroup(((_tmp950="[",_tag_dyneither(_tmp950,sizeof(char),2))),((_tmp94F="]",_tag_dyneither(_tmp94F,sizeof(char),2))),((_tmp94E=",",_tag_dyneither(_tmp94E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp433))),((_tmp94B[1]=
# 1575
Cyc_Absynpp_lb(),((_tmp94B[0]=Cyc_Absynpp_qvar2doc(_tmp468),Cyc_PP_cat(_tag_dyneither(_tmp94B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1578
goto _LL255;}_LL274: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp436=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp436->tag != 6)goto _LL276;else{if(_tmp436->f1 != 0)goto _LL276;_tmp437=_tmp436->f2;_tmp438=_tmp436->f3;_tmp439=_tmp436->f4;}}_LL275: {
# 1580
const char*_tmp952;const char*_tmp951;struct _dyneither_ptr term=_tmp439?(_tmp952=", ...}",_tag_dyneither(_tmp952,sizeof(char),7)):((_tmp951="}",_tag_dyneither(_tmp951,sizeof(char),2)));
{const char*_tmp95D;const char*_tmp95C;const char*_tmp95B;const char*_tmp95A;const char*_tmp959;struct Cyc_PP_Doc*_tmp958[3];s=((_tmp958[2]=
# 1583
Cyc_PP_group(((_tmp95A="",_tag_dyneither(_tmp95A,sizeof(char),1))),term,((_tmp959=",",_tag_dyneither(_tmp959,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp438)),((_tmp958[1]=
# 1582
Cyc_PP_egroup(((_tmp95D="[",_tag_dyneither(_tmp95D,sizeof(char),2))),((_tmp95C="]",_tag_dyneither(_tmp95C,sizeof(char),2))),((_tmp95B=",",_tag_dyneither(_tmp95B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp437))),((_tmp958[0]=
# 1581
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp958,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1584
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp43A=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43A->tag != 12)goto _LL278;else{_tmp43B=_tmp43A->f2;}}_LL277:
 _tmp43D=_tmp43B;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp43C=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43C->tag != 13)goto _LL27A;else{_tmp43D=_tmp43C->f2;}}_LL279:
 s=Cyc_Absynpp_qvar2doc(_tmp43D->name);goto _LL255;_LL27A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43E->tag != 7)goto _LL27C;else{_tmp43F=_tmp43E->f2;if(_tmp43E->f3 != 0)goto _LL27C;}}_LL27B:
 s=Cyc_Absynpp_qvar2doc(_tmp43F->name);goto _LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp440->tag != 7)goto _LL27E;else{_tmp441=_tmp440->f2;_tmp442=_tmp440->f3;_tmp443=_tmp440->f4;}}_LL27D: {
# 1589
const char*_tmp95F;const char*_tmp95E;struct _dyneither_ptr term=_tmp443?(_tmp95F=", ...)",_tag_dyneither(_tmp95F,sizeof(char),7)):((_tmp95E=")",_tag_dyneither(_tmp95E,sizeof(char),2)));
{const char*_tmp964;const char*_tmp963;struct Cyc_PP_Doc*_tmp962[2];s=((_tmp962[1]=Cyc_PP_egroup(((_tmp964="(",_tag_dyneither(_tmp964,sizeof(char),2))),term,((_tmp963=",",_tag_dyneither(_tmp963,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp442)),((_tmp962[0]=Cyc_Absynpp_qvar2doc(_tmp441->name),Cyc_PP_cat(_tag_dyneither(_tmp962,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL27E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp444=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp444->tag != 16)goto _LL255;else{_tmp445=_tmp444->f1;}}_LL27F:
# 1593
 s=Cyc_Absynpp_exp2doc(_tmp445);goto _LL255;_LL255:;}
# 1595
return s;}
# 1598
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp96B;const char*_tmp96A;const char*_tmp969;struct Cyc_PP_Doc*_tmp968[2];return(_tmp968[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp968[0]=
# 1599
Cyc_PP_egroup(((_tmp96B="",_tag_dyneither(_tmp96B,sizeof(char),1))),((_tmp96A="=",_tag_dyneither(_tmp96A,sizeof(char),2))),((_tmp969="=",_tag_dyneither(_tmp969,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp968,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1603
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp970;struct Cyc_PP_Doc*_tmp96F[2];struct Cyc_PP_Doc*_tmp96E[2];return(_tmp96E[1]=
Cyc_PP_nest(2,((_tmp96F[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp96F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp96E[0]=
# 1605
Cyc_PP_text(((_tmp970="default: ",_tag_dyneither(_tmp970,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96E,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1607
if(c->where_clause == 0){
const char*_tmp977;const char*_tmp976;struct Cyc_PP_Doc*_tmp975[2];struct Cyc_PP_Doc*_tmp974[4];return(_tmp974[3]=
# 1611
Cyc_PP_nest(2,((_tmp975[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp975[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp975,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp974[2]=
# 1610
Cyc_PP_text(((_tmp976=": ",_tag_dyneither(_tmp976,sizeof(char),3)))),((_tmp974[1]=
# 1609
Cyc_Absynpp_pat2doc(c->pattern),((_tmp974[0]=
# 1608
Cyc_PP_text(((_tmp977="case ",_tag_dyneither(_tmp977,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1613
const char*_tmp980;const char*_tmp97F;const char*_tmp97E;struct Cyc_PP_Doc*_tmp97D[2];struct Cyc_PP_Doc*_tmp97C[6];return(_tmp97C[5]=
# 1618
Cyc_PP_nest(2,((_tmp97D[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp97D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97C[4]=
# 1617
Cyc_PP_text(((_tmp97E=": ",_tag_dyneither(_tmp97E,sizeof(char),3)))),((_tmp97C[3]=
# 1616
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp97C[2]=
# 1615
Cyc_PP_text(((_tmp97F=" && ",_tag_dyneither(_tmp97F,sizeof(char),5)))),((_tmp97C[1]=
# 1614
Cyc_Absynpp_pat2doc(c->pattern),((_tmp97C[0]=
# 1613
Cyc_PP_text(((_tmp980="case ",_tag_dyneither(_tmp980,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97C,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1621
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp981;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp981="",_tag_dyneither(_tmp981,sizeof(char),1))),cs);}
# 1625
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1629
const char*_tmp984;struct Cyc_PP_Doc*_tmp983[3];return(_tmp983[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp983[1]=Cyc_PP_text(((_tmp984=" = ",_tag_dyneither(_tmp984,sizeof(char),4)))),((_tmp983[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1632
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp985;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp985=",",_tag_dyneither(_tmp985,sizeof(char),2))),fs);}
# 1636
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1640
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp986;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp986=",",_tag_dyneither(_tmp986,sizeof(char),2))),vds);}
# 1644
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp494;struct _tuple0*_tmp495;struct Cyc_Absyn_Tqual _tmp496;void*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Vardecl*_tmp493=vd;_tmp494=_tmp493->sc;_tmp495=_tmp493->name;_tmp496=_tmp493->tq;_tmp497=_tmp493->type;_tmp498=_tmp493->initializer;_tmp499=_tmp493->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp495);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp499);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL280:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL281:
# 1653
{void*_tmp49A=Cyc_Tcutil_compress(_tmp497);struct Cyc_List_List*_tmp49C;_LL284: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49B=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49A;if(_tmp49B->tag != 9)goto _LL286;else{_tmp49C=(_tmp49B->f1).attributes;}}_LL285:
# 1655
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp49C);
goto _LL283;_LL286:;_LL287:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL283;_LL283:;}
# 1659
break;}{
# 1662
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL289:
 tmp_doc=attsdoc;break;}
# 1667
{struct Cyc_Core_Opt*_tmp993;struct Cyc_PP_Doc*_tmp992[2];struct Cyc_PP_Doc*_tmp991[2];const char*_tmp990;const char*_tmp98F;struct Cyc_PP_Doc*_tmp98E[5];s=((_tmp98E[4]=
# 1674
Cyc_PP_text(((_tmp98F=";",_tag_dyneither(_tmp98F,sizeof(char),2)))),((_tmp98E[3]=
# 1671
_tmp498 == 0?
Cyc_PP_nil_doc():(
(_tmp991[1]=Cyc_Absynpp_exp2doc(_tmp498),((_tmp991[0]=Cyc_PP_text(((_tmp990=" = ",_tag_dyneither(_tmp990,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp98E[2]=
# 1670
Cyc_Absynpp_tqtd2doc(_tmp496,_tmp497,((_tmp993=_cycalloc(sizeof(*_tmp993)),((_tmp993->v=((_tmp992[1]=sn,((_tmp992[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp992,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp993))))),((_tmp98E[1]=
# 1669
Cyc_Absynpp_scope2doc(_tmp494),((_tmp98E[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1675
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1678
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4A4;struct _tuple17 _tmp4A3=*x;_tmp4A4=_tmp4A3.f2;
return Cyc_Absynpp_qvar2doc(_tmp4A4);}
# 1683
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp994[4];return(_tmp994[3]=
# 1688
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp994[2]=
# 1687
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp994[1]=
# 1686
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp994[0]=
# 1685
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp994,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1690
struct Cyc_PP_Doc*_tmp99C[2];const char*_tmp99B;struct Cyc_PP_Doc*_tmp99A[2];struct Cyc_PP_Doc*_tmp999[12];return(_tmp999[11]=
# 1701
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp999[10]=
# 1700
Cyc_Absynpp_rb(),((_tmp999[9]=
# 1699
Cyc_PP_line_doc(),((_tmp999[8]=
# 1698
Cyc_PP_nest(2,((_tmp99A[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp99A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp99A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp999[7]=
# 1696
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp99C[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp99C[0]=Cyc_PP_text(((_tmp99B=":",_tag_dyneither(_tmp99B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp999[6]=
# 1695
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp999[5]=
# 1694
Cyc_Absynpp_lb(),((_tmp999[4]=Cyc_PP_blank_doc(),((_tmp999[3]=
# 1693
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp999[2]=
# 1692
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp999[1]=
# 1691
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp999[0]=
# 1690
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1704
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4AB;struct _tuple0*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Core_Opt*_tmp4AE;int _tmp4AF;struct Cyc_Absyn_Datatypedecl*_tmp4AA=dd;_tmp4AB=_tmp4AA->sc;_tmp4AC=_tmp4AA->name;_tmp4AD=_tmp4AA->tvs;_tmp4AE=_tmp4AA->fields;_tmp4AF=_tmp4AA->is_extensible;
if(_tmp4AE == 0){
const char*_tmp9A1;const char*_tmp9A0;struct Cyc_PP_Doc*_tmp99F[5];return(_tmp99F[4]=
# 1711
Cyc_Absynpp_ktvars2doc(_tmp4AD),((_tmp99F[3]=
# 1710
_tmp4AF?Cyc_Absynpp_qvar2bolddoc(_tmp4AC): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AC),((_tmp99F[2]=
# 1709
Cyc_PP_text(((_tmp9A0="datatype ",_tag_dyneither(_tmp9A0,sizeof(char),10)))),((_tmp99F[1]=
# 1708
_tmp4AF?Cyc_PP_text(((_tmp9A1="@extensible ",_tag_dyneither(_tmp9A1,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp99F[0]=
# 1707
Cyc_Absynpp_scope2doc(_tmp4AB),Cyc_PP_cat(_tag_dyneither(_tmp99F,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1713
const char*_tmp9A8;const char*_tmp9A7;struct Cyc_PP_Doc*_tmp9A6[2];struct Cyc_PP_Doc*_tmp9A5[10];return(_tmp9A5[9]=
# 1721
Cyc_Absynpp_rb(),((_tmp9A5[8]=
# 1720
Cyc_PP_line_doc(),((_tmp9A5[7]=
# 1719
Cyc_PP_nest(2,((_tmp9A6[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4AE->v),((_tmp9A6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A5[6]=
# 1718
Cyc_Absynpp_lb(),((_tmp9A5[5]=Cyc_PP_blank_doc(),((_tmp9A5[4]=
# 1717
Cyc_Absynpp_ktvars2doc(_tmp4AD),((_tmp9A5[3]=
# 1716
_tmp4AF?Cyc_Absynpp_qvar2bolddoc(_tmp4AC): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AC),((_tmp9A5[2]=
# 1715
Cyc_PP_text(((_tmp9A7="datatype ",_tag_dyneither(_tmp9A7,sizeof(char),10)))),((_tmp9A5[1]=
# 1714
_tmp4AF?Cyc_PP_text(((_tmp9A8="@extensible ",_tag_dyneither(_tmp9A8,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A5[0]=
# 1713
Cyc_Absynpp_scope2doc(_tmp4AB),Cyc_PP_cat(_tag_dyneither(_tmp9A5,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1724
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4B8;struct _tuple0*_tmp4B9;struct Cyc_Core_Opt*_tmp4BA;struct Cyc_Absyn_Enumdecl*_tmp4B7=ed;_tmp4B8=_tmp4B7->sc;_tmp4B9=_tmp4B7->name;_tmp4BA=_tmp4B7->fields;
if(_tmp4BA == 0){
const char*_tmp9AB;struct Cyc_PP_Doc*_tmp9AA[3];return(_tmp9AA[2]=
# 1729
Cyc_Absynpp_typedef_name2bolddoc(_tmp4B9),((_tmp9AA[1]=
# 1728
Cyc_PP_text(((_tmp9AB="enum ",_tag_dyneither(_tmp9AB,sizeof(char),6)))),((_tmp9AA[0]=
# 1727
Cyc_Absynpp_scope2doc(_tmp4B8),Cyc_PP_cat(_tag_dyneither(_tmp9AA,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1732
const char*_tmp9B0;struct Cyc_PP_Doc*_tmp9AF[2];struct Cyc_PP_Doc*_tmp9AE[8];return(_tmp9AE[7]=
# 1738
Cyc_Absynpp_rb(),((_tmp9AE[6]=
# 1737
Cyc_PP_line_doc(),((_tmp9AE[5]=
# 1736
Cyc_PP_nest(2,((_tmp9AF[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4BA->v),((_tmp9AF[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AF,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9AE[4]=
# 1735
Cyc_Absynpp_lb(),((_tmp9AE[3]=Cyc_PP_blank_doc(),((_tmp9AE[2]=
# 1734
Cyc_Absynpp_qvar2bolddoc(_tmp4B9),((_tmp9AE[1]=
# 1733
Cyc_PP_text(((_tmp9B0="enum ",_tag_dyneither(_tmp9B0,sizeof(char),6)))),((_tmp9AE[0]=
# 1732
Cyc_Absynpp_scope2doc(_tmp4B8),Cyc_PP_cat(_tag_dyneither(_tmp9AE,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1741
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4C0=d->r;struct Cyc_Absyn_Fndecl*_tmp4C2;struct Cyc_Absyn_Aggrdecl*_tmp4C4;struct Cyc_Absyn_Vardecl*_tmp4C6;struct Cyc_Absyn_Tvar*_tmp4C8;struct Cyc_Absyn_Vardecl*_tmp4C9;int _tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Tvar*_tmp4CD;struct Cyc_Absyn_Vardecl*_tmp4CE;struct Cyc_Absyn_Datatypedecl*_tmp4D0;struct Cyc_Absyn_Pat*_tmp4D2;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_List_List*_tmp4D5;struct Cyc_Absyn_Enumdecl*_tmp4D7;struct Cyc_Absyn_Typedefdecl*_tmp4D9;struct _dyneither_ptr*_tmp4DB;struct Cyc_List_List*_tmp4DC;struct _tuple0*_tmp4DE;struct Cyc_List_List*_tmp4DF;struct Cyc_List_List*_tmp4E1;struct Cyc_List_List*_tmp4E3;struct Cyc_List_List*_tmp4E4;_LL28C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4C1=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C1->tag != 1)goto _LL28E;else{_tmp4C2=_tmp4C1->f1;}}_LL28D: {
# 1745
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9B6;struct Cyc_Absyn_FnInfo _tmp9B5;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9B4;void*t=(void*)((_tmp9B4=_cycalloc(sizeof(*_tmp9B4)),((_tmp9B4[0]=((_tmp9B6.tag=9,((_tmp9B6.f1=((_tmp9B5.tvars=_tmp4C2->tvs,((_tmp9B5.effect=_tmp4C2->effect,((_tmp9B5.ret_tqual=_tmp4C2->ret_tqual,((_tmp9B5.ret_typ=_tmp4C2->ret_type,((_tmp9B5.args=
# 1749
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C2->args),((_tmp9B5.c_varargs=_tmp4C2->c_varargs,((_tmp9B5.cyc_varargs=_tmp4C2->cyc_varargs,((_tmp9B5.rgn_po=_tmp4C2->rgn_po,((_tmp9B5.attributes=0,_tmp9B5)))))))))))))))))),_tmp9B6)))),_tmp9B4))));
# 1753
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C2->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C2->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AC:
{const char*_tmp9B7;inlinedoc=Cyc_PP_text(((_tmp9B7="inline ",_tag_dyneither(_tmp9B7,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:
{const char*_tmp9B8;inlinedoc=Cyc_PP_text(((_tmp9B8="__inline ",_tag_dyneither(_tmp9B8,sizeof(char),10))));}break;}else{
# 1761
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C2->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C2->attributes);break;}{
# 1768
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C2->name);
struct Cyc_PP_Doc*_tmp9BB[2];struct Cyc_Core_Opt*_tmp9BA;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9BA=_cycalloc(sizeof(*_tmp9BA)),((_tmp9BA->v=((_tmp9BB[1]=namedoc,((_tmp9BB[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9BA)))));
struct Cyc_PP_Doc*_tmp9BE[2];struct Cyc_PP_Doc*_tmp9BD[5];struct Cyc_PP_Doc*bodydoc=(_tmp9BD[4]=
# 1774
Cyc_Absynpp_rb(),((_tmp9BD[3]=
# 1773
Cyc_PP_line_doc(),((_tmp9BD[2]=
# 1772
Cyc_PP_nest(2,((_tmp9BE[1]=Cyc_Absynpp_stmt2doc(_tmp4C2->body,0),((_tmp9BE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BE,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9BD[1]=
# 1771
Cyc_Absynpp_lb(),((_tmp9BD[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BD,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1775
{struct Cyc_PP_Doc*_tmp9BF[4];s=((_tmp9BF[3]=bodydoc,((_tmp9BF[2]=tqtddoc,((_tmp9BF[1]=scopedoc,((_tmp9BF[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1777
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:
{struct Cyc_PP_Doc*_tmp9C0[2];s=((_tmp9C0[1]=s,((_tmp9C0[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3:
 break;}
# 1782
goto _LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C3=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C3->tag != 6)goto _LL290;else{_tmp4C4=_tmp4C3->f1;}}_LL28F:
# 1785
{const char*_tmp9C3;struct Cyc_PP_Doc*_tmp9C2[2];s=((_tmp9C2[1]=Cyc_PP_text(((_tmp9C3=";",_tag_dyneither(_tmp9C3,sizeof(char),2)))),((_tmp9C2[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4C4),Cyc_PP_cat(_tag_dyneither(_tmp9C2,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C5->tag != 0)goto _LL292;else{_tmp4C6=_tmp4C5->f1;}}_LL291:
# 1788
 s=Cyc_Absynpp_vardecl2doc(_tmp4C6);
goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4C7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C7->tag != 4)goto _LL294;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C7->f2;_tmp4CA=_tmp4C7->f3;_tmp4CB=_tmp4C7->f4;}}_LL293:
# 1791
{const char*_tmp9D6;const char*_tmp9D5;const char*_tmp9D4;const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[3];const char*_tmp9D1;const char*_tmp9D0;const char*_tmp9CF;struct Cyc_PP_Doc*_tmp9CE[8];s=((_tmp9CE[7]=
# 1799
Cyc_PP_text(((_tmp9CF=";",_tag_dyneither(_tmp9CF,sizeof(char),2)))),((_tmp9CE[6]=
# 1797
_tmp4CB != 0?
(_tmp9D2[2]=Cyc_PP_text(((_tmp9D1=")",_tag_dyneither(_tmp9D1,sizeof(char),2)))),((_tmp9D2[1]=Cyc_Absynpp_exp2doc(_tmp4CB),((_tmp9D2[0]=Cyc_PP_text(((_tmp9D0=" = open(",_tag_dyneither(_tmp9D0,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9CE[5]=
# 1796
_tmp4CA?Cyc_PP_text(((_tmp9D3=" @resetable",_tag_dyneither(_tmp9D3,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9CE[4]=
# 1795
Cyc_Absynpp_qvar2doc(_tmp4C9->name),((_tmp9CE[3]=
# 1794
Cyc_PP_text(((_tmp9D4=">",_tag_dyneither(_tmp9D4,sizeof(char),2)))),((_tmp9CE[2]=
# 1793
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C8)),((_tmp9CE[1]=
# 1792
Cyc_PP_text(((_tmp9D5="<",_tag_dyneither(_tmp9D5,sizeof(char),2)))),((_tmp9CE[0]=
# 1791
Cyc_PP_text(((_tmp9D6="region",_tag_dyneither(_tmp9D6,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9CE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1800
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4CC=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4CC->tag != 5)goto _LL296;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CC->f2;}}_LL295:
# 1802
{const char*_tmp9DF;const char*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;struct Cyc_PP_Doc*_tmp9DB[6];s=((_tmp9DB[5]=
Cyc_PP_text(((_tmp9DC=";",_tag_dyneither(_tmp9DC,sizeof(char),2)))),((_tmp9DB[4]=Cyc_Absynpp_vardecl2doc(_tmp4CE),((_tmp9DB[3]=
# 1802
Cyc_PP_text(((_tmp9DD="> ",_tag_dyneither(_tmp9DD,sizeof(char),3)))),((_tmp9DB[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CD)),((_tmp9DB[1]=Cyc_PP_text(((_tmp9DE=" <",_tag_dyneither(_tmp9DE,sizeof(char),3)))),((_tmp9DB[0]=Cyc_PP_text(((_tmp9DF="alias",_tag_dyneither(_tmp9DF,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 1804
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4CF=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4CF->tag != 7)goto _LL298;else{_tmp4D0=_tmp4CF->f1;}}_LL297:
# 1806
{const char*_tmp9E2;struct Cyc_PP_Doc*_tmp9E1[2];s=((_tmp9E1[1]=Cyc_PP_text(((_tmp9E2=";",_tag_dyneither(_tmp9E2,sizeof(char),2)))),((_tmp9E1[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4D0),Cyc_PP_cat(_tag_dyneither(_tmp9E1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D1=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D1->tag != 2)goto _LL29A;else{_tmp4D2=_tmp4D1->f1;_tmp4D3=_tmp4D1->f3;}}_LL299:
# 1809
{const char*_tmp9E9;const char*_tmp9E8;const char*_tmp9E7;struct Cyc_PP_Doc*_tmp9E6[5];s=((_tmp9E6[4]=
# 1813
Cyc_PP_text(((_tmp9E7=";",_tag_dyneither(_tmp9E7,sizeof(char),2)))),((_tmp9E6[3]=
# 1812
Cyc_Absynpp_exp2doc(_tmp4D3),((_tmp9E6[2]=
# 1811
Cyc_PP_text(((_tmp9E8=" = ",_tag_dyneither(_tmp9E8,sizeof(char),4)))),((_tmp9E6[1]=
# 1810
Cyc_Absynpp_pat2doc(_tmp4D2),((_tmp9E6[0]=
# 1809
Cyc_PP_text(((_tmp9E9="let ",_tag_dyneither(_tmp9E9,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1814
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4D4=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D4->tag != 3)goto _LL29C;else{_tmp4D5=_tmp4D4->f1;}}_LL29B:
# 1816
{const char*_tmp9EE;const char*_tmp9ED;struct Cyc_PP_Doc*_tmp9EC[3];s=((_tmp9EC[2]=Cyc_PP_text(((_tmp9ED=";",_tag_dyneither(_tmp9ED,sizeof(char),2)))),((_tmp9EC[1]=Cyc_Absynpp_ids2doc(_tmp4D5),((_tmp9EC[0]=Cyc_PP_text(((_tmp9EE="let ",_tag_dyneither(_tmp9EE,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4D6=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D6->tag != 8)goto _LL29E;else{_tmp4D7=_tmp4D6->f1;}}_LL29D:
# 1819
{const char*_tmp9F1;struct Cyc_PP_Doc*_tmp9F0[2];s=((_tmp9F0[1]=Cyc_PP_text(((_tmp9F1=";",_tag_dyneither(_tmp9F1,sizeof(char),2)))),((_tmp9F0[0]=Cyc_Absynpp_enumdecl2doc(_tmp4D7),Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4D8=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D8->tag != 9)goto _LL2A0;else{_tmp4D9=_tmp4D8->f1;}}_LL29F: {
# 1822
void*t;
if(_tmp4D9->defn != 0)
t=(void*)_check_null(_tmp4D9->defn);else{
# 1826
t=Cyc_Absyn_new_evar(_tmp4D9->kind,0);}
{const char*_tmp9FB;struct Cyc_Core_Opt*_tmp9FA;struct Cyc_PP_Doc*_tmp9F9[2];const char*_tmp9F8;struct Cyc_PP_Doc*_tmp9F7[4];s=((_tmp9F7[3]=
# 1834
Cyc_PP_text(((_tmp9F8=";",_tag_dyneither(_tmp9F8,sizeof(char),2)))),((_tmp9F7[2]=
# 1833
Cyc_Absynpp_atts2doc(_tmp4D9->atts),((_tmp9F7[1]=
# 1828
Cyc_Absynpp_tqtd2doc(_tmp4D9->tq,t,(
# 1830
(_tmp9FA=_cycalloc(sizeof(*_tmp9FA)),((_tmp9FA->v=((_tmp9F9[1]=
Cyc_Absynpp_tvars2doc(_tmp4D9->tvs),((_tmp9F9[0]=
# 1830
Cyc_Absynpp_typedef_name2bolddoc(_tmp4D9->name),Cyc_PP_cat(_tag_dyneither(_tmp9F9,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FA))))),((_tmp9F7[0]=
# 1827
Cyc_PP_text(((_tmp9FB="typedef ",_tag_dyneither(_tmp9FB,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1836
goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4DA=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4DA->tag != 10)goto _LL2A2;else{_tmp4DB=_tmp4DA->f1;_tmp4DC=_tmp4DA->f2;}}_LL2A1:
# 1838
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DB);
{const char*_tmpA00;const char*_tmp9FF;struct Cyc_PP_Doc*_tmp9FE[8];s=((_tmp9FE[7]=
# 1845
Cyc_Absynpp_rb(),((_tmp9FE[6]=
# 1844
Cyc_PP_line_doc(),((_tmp9FE[5]=
# 1843
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9FF="",_tag_dyneither(_tmp9FF,sizeof(char),1))),_tmp4DC),((_tmp9FE[4]=
# 1842
Cyc_PP_line_doc(),((_tmp9FE[3]=
# 1841
Cyc_Absynpp_lb(),((_tmp9FE[2]=Cyc_PP_blank_doc(),((_tmp9FE[1]=
# 1840
Cyc_PP_textptr(_tmp4DB),((_tmp9FE[0]=
# 1839
Cyc_PP_text(((_tmpA00="namespace ",_tag_dyneither(_tmpA00,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp9FE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1846
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4DD=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4DD->tag != 11)goto _LL2A4;else{_tmp4DE=_tmp4DD->f1;_tmp4DF=_tmp4DD->f2;}}_LL2A3:
# 1849
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA05;const char*_tmpA04;struct Cyc_PP_Doc*_tmpA03[8];s=((_tmpA03[7]=
# 1856
Cyc_Absynpp_rb(),((_tmpA03[6]=
# 1855
Cyc_PP_line_doc(),((_tmpA03[5]=
# 1854
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA04="",_tag_dyneither(_tmpA04,sizeof(char),1))),_tmp4DF),((_tmpA03[4]=
# 1853
Cyc_PP_line_doc(),((_tmpA03[3]=
# 1852
Cyc_Absynpp_lb(),((_tmpA03[2]=Cyc_PP_blank_doc(),((_tmpA03[1]=
# 1851
Cyc_Absynpp_qvar2doc(_tmp4DE),((_tmpA03[0]=
# 1850
Cyc_PP_text(((_tmpA05="using ",_tag_dyneither(_tmpA05,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA03,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1858
const char*_tmpA10;const char*_tmpA0F;const char*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;struct Cyc_PP_Doc*_tmpA0B[11];s=((_tmpA0B[10]=
# 1868
Cyc_PP_text(((_tmpA0C=" */",_tag_dyneither(_tmpA0C,sizeof(char),4)))),((_tmpA0B[9]=
# 1867
Cyc_Absynpp_rb(),((_tmpA0B[8]=
# 1866
Cyc_PP_text(((_tmpA0D="/* ",_tag_dyneither(_tmpA0D,sizeof(char),4)))),((_tmpA0B[7]=
# 1865
Cyc_PP_line_doc(),((_tmpA0B[6]=
# 1864
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0E="",_tag_dyneither(_tmpA0E,sizeof(char),1))),_tmp4DF),((_tmpA0B[5]=
# 1863
Cyc_PP_line_doc(),((_tmpA0B[4]=
# 1862
Cyc_PP_text(((_tmpA0F=" */",_tag_dyneither(_tmpA0F,sizeof(char),4)))),((_tmpA0B[3]=
# 1861
Cyc_Absynpp_lb(),((_tmpA0B[2]=
# 1860
Cyc_PP_blank_doc(),((_tmpA0B[1]=
# 1859
Cyc_Absynpp_qvar2doc(_tmp4DE),((_tmpA0B[0]=
# 1858
Cyc_PP_text(((_tmpA10="/* using ",_tag_dyneither(_tmpA10,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA0B,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1869
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4E0=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E0->tag != 12)goto _LL2A6;else{_tmp4E1=_tmp4E0->f1;}}_LL2A5:
# 1871
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA15;const char*_tmpA14;struct Cyc_PP_Doc*_tmpA13[6];s=((_tmpA13[5]=
# 1877
Cyc_Absynpp_rb(),((_tmpA13[4]=
# 1876
Cyc_PP_line_doc(),((_tmpA13[3]=
# 1875
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA14="",_tag_dyneither(_tmpA14,sizeof(char),1))),_tmp4E1),((_tmpA13[2]=
# 1874
Cyc_PP_line_doc(),((_tmpA13[1]=
# 1873
Cyc_Absynpp_lb(),((_tmpA13[0]=
# 1872
Cyc_PP_text(((_tmpA15="extern \"C\" ",_tag_dyneither(_tmpA15,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA13,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1879
const char*_tmpA20;const char*_tmpA1F;const char*_tmpA1E;const char*_tmpA1D;const char*_tmpA1C;struct Cyc_PP_Doc*_tmpA1B[9];s=((_tmpA1B[8]=
# 1887
Cyc_PP_text(((_tmpA1C=" */",_tag_dyneither(_tmpA1C,sizeof(char),4)))),((_tmpA1B[7]=
# 1886
Cyc_Absynpp_rb(),((_tmpA1B[6]=
# 1885
Cyc_PP_text(((_tmpA1D="/* ",_tag_dyneither(_tmpA1D,sizeof(char),4)))),((_tmpA1B[5]=
# 1884
Cyc_PP_line_doc(),((_tmpA1B[4]=
# 1883
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1E="",_tag_dyneither(_tmpA1E,sizeof(char),1))),_tmp4E1),((_tmpA1B[3]=
# 1882
Cyc_PP_line_doc(),((_tmpA1B[2]=
# 1881
Cyc_PP_text(((_tmpA1F=" */",_tag_dyneither(_tmpA1F,sizeof(char),4)))),((_tmpA1B[1]=
# 1880
Cyc_Absynpp_lb(),((_tmpA1B[0]=
# 1879
Cyc_PP_text(((_tmpA20="/* extern \"C\" ",_tag_dyneither(_tmpA20,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA1B,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1888
goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E2=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E2->tag != 13)goto _LL2A8;else{_tmp4E3=_tmp4E2->f1;_tmp4E4=_tmp4E2->f2;}}_LL2A7:
# 1890
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4E4 != 0){
const char*_tmpA25;const char*_tmpA24;struct Cyc_PP_Doc*_tmpA23[7];exs_doc=((_tmpA23[6]=
# 1895
Cyc_Absynpp_rb(),((_tmpA23[5]=Cyc_PP_line_doc(),((_tmpA23[4]=
# 1894
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA24=",",_tag_dyneither(_tmpA24,sizeof(char),2))),_tmp4E4),((_tmpA23[3]=Cyc_PP_line_doc(),((_tmpA23[2]=
# 1893
Cyc_Absynpp_lb(),((_tmpA23[1]=Cyc_PP_text(((_tmpA25=" export ",_tag_dyneither(_tmpA25,sizeof(char),9)))),((_tmpA23[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA23,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1897
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA2A;const char*_tmpA29;struct Cyc_PP_Doc*_tmpA28[6];s=((_tmpA28[5]=exs_doc,((_tmpA28[4]=
# 1902
Cyc_PP_line_doc(),((_tmpA28[3]=
# 1901
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA29="",_tag_dyneither(_tmpA29,sizeof(char),1))),_tmp4E3),((_tmpA28[2]=
# 1900
Cyc_PP_line_doc(),((_tmpA28[1]=
# 1899
Cyc_Absynpp_lb(),((_tmpA28[0]=
# 1898
Cyc_PP_text(((_tmpA2A="extern \"C include\" ",_tag_dyneither(_tmpA2A,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA28,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1905
const char*_tmpA35;const char*_tmpA34;const char*_tmpA33;const char*_tmpA32;const char*_tmpA31;struct Cyc_PP_Doc*_tmpA30[9];s=((_tmpA30[8]=
# 1913
Cyc_PP_text(((_tmpA31=" */",_tag_dyneither(_tmpA31,sizeof(char),4)))),((_tmpA30[7]=
# 1912
Cyc_Absynpp_rb(),((_tmpA30[6]=
# 1911
Cyc_PP_text(((_tmpA32="/* ",_tag_dyneither(_tmpA32,sizeof(char),4)))),((_tmpA30[5]=
# 1910
Cyc_PP_line_doc(),((_tmpA30[4]=
# 1909
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA33="",_tag_dyneither(_tmpA33,sizeof(char),1))),_tmp4E3),((_tmpA30[3]=
# 1908
Cyc_PP_line_doc(),((_tmpA30[2]=
# 1907
Cyc_PP_text(((_tmpA34=" */",_tag_dyneither(_tmpA34,sizeof(char),4)))),((_tmpA30[1]=
# 1906
Cyc_Absynpp_lb(),((_tmpA30[0]=
# 1905
Cyc_PP_text(((_tmpA35="/* extern \"C include\" ",_tag_dyneither(_tmpA35,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA30,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1914
goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4E5=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E5->tag != 14)goto _LL2AA;}_LL2A9:
# 1916
{const char*_tmpA38;struct Cyc_PP_Doc*_tmpA37[2];s=((_tmpA37[1]=Cyc_Absynpp_lb(),((_tmpA37[0]=Cyc_PP_text(((_tmpA38="__cyclone_port_on__;",_tag_dyneither(_tmpA38,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA37,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4E6=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E6->tag != 15)goto _LL28B;}_LL2AB:
# 1919
{const char*_tmpA3B;struct Cyc_PP_Doc*_tmpA3A[2];s=((_tmpA3A[1]=Cyc_Absynpp_lb(),((_tmpA3A[0]=Cyc_PP_text(((_tmpA3B="__cyclone_port_off__;",_tag_dyneither(_tmpA3B,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA3A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL28B:;}
# 1923
return s;}
# 1926
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2B5: {
const char*_tmpA3C;return Cyc_PP_text(((_tmpA3C="static ",_tag_dyneither(_tmpA3C,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B6:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2B7: {
const char*_tmpA3D;return Cyc_PP_text(((_tmpA3D="extern ",_tag_dyneither(_tmpA3D,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {
const char*_tmpA3E;return Cyc_PP_text(((_tmpA3E="extern \"C\" ",_tag_dyneither(_tmpA3E,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2B9: {
const char*_tmpA3F;return Cyc_PP_text(((_tmpA3F="abstract ",_tag_dyneither(_tmpA3F,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BA: {
const char*_tmpA40;return Cyc_PP_text(((_tmpA40="register ",_tag_dyneither(_tmpA40,sizeof(char),10))));}}}
# 1939
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp53C=t;struct Cyc_Absyn_Tvar*_tmp53E;struct Cyc_List_List*_tmp540;_LL2BD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53C;if(_tmp53D->tag != 2)goto _LL2BF;else{_tmp53E=_tmp53D->f1;}}_LL2BE:
 return Cyc_Tcutil_is_temp_tvar(_tmp53E);_LL2BF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp53F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp53C;if(_tmp53F->tag != 24)goto _LL2C1;else{_tmp540=_tmp53F->f1;}}_LL2C0:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp540);_LL2C1:;_LL2C2:
 return 0;_LL2BC:;}
# 1951
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp541=t;struct Cyc_Absyn_Typedefdecl*_tmp545;void*_tmp546;_LL2C4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp541;if(_tmp542->tag != 12)goto _LL2C6;}_LL2C5:
 return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp541;if(_tmp543->tag != 14)goto _LL2C8;}_LL2C7:
 return 1;_LL2C8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp541;if(_tmp544->tag != 17)goto _LL2CA;else{_tmp545=_tmp544->f3;_tmp546=(void*)_tmp544->f4;}}if(!(_tmp546 != 0))goto _LL2CA;_LL2C9:
# 1958
 return Cyc_Absynpp_is_anon_aggrtype(_tmp546);_LL2CA:;_LL2CB:
 return 0;_LL2C3:;}
# 1965
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1968
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA41;struct _tuple16 _tmp548=(_tmpA41.f1=(void*)tms->hd,((_tmpA41.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA41)));_LL2CD:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp549=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp548.f1;if(_tmp549->tag != 2)goto _LL2CF;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54A=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp548.f2;if(_tmp54A->tag != 3)goto _LL2CF;};_LL2CE: {
# 1971
struct Cyc_List_List*_tmpA44;struct Cyc_List_List*_tmpA43;return(_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((_tmpA43->hd=(void*)tms->hd,((_tmpA43->tl=((_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA44->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA44)))))),_tmpA43)))));}_LL2CF:;_LL2D0: {
struct Cyc_List_List*_tmpA45;return(_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45->hd=atts,((_tmpA45->tl=tms,_tmpA45)))));}_LL2CC:;}else{
# 1974
struct Cyc_List_List*_tmpA46;return(_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->hd=atts,((_tmpA46->tl=tms,_tmpA46)))));}}
# 1979
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1981
void*_tmp54F=t;void*_tmp551;struct Cyc_Absyn_Tqual _tmp552;struct Cyc_Absyn_Exp*_tmp553;union Cyc_Absyn_Constraint*_tmp554;unsigned int _tmp555;void*_tmp557;struct Cyc_Absyn_Tqual _tmp558;struct Cyc_Absyn_PtrAtts _tmp559;struct Cyc_List_List*_tmp55B;void*_tmp55C;struct Cyc_Absyn_Tqual _tmp55D;void*_tmp55E;struct Cyc_List_List*_tmp55F;int _tmp560;struct Cyc_Absyn_VarargInfo*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_Core_Opt*_tmp565;void*_tmp566;int _tmp567;struct _tuple0*_tmp569;struct Cyc_List_List*_tmp56A;struct Cyc_Absyn_Typedefdecl*_tmp56B;void*_tmp56C;_LL2D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54F;if(_tmp550->tag != 8)goto _LL2D4;else{_tmp551=(_tmp550->f1).elt_type;_tmp552=(_tmp550->f1).tq;_tmp553=(_tmp550->f1).num_elts;_tmp554=(_tmp550->f1).zero_term;_tmp555=(_tmp550->f1).zt_loc;}}_LL2D3: {
# 1984
struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;struct Cyc_List_List*_tmp570;struct _tuple12 _tmp56D=Cyc_Absynpp_to_tms(r,_tmp552,_tmp551);_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;_tmp570=_tmp56D.f3;{
void*tm;
if(_tmp553 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA49;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA48;tm=(void*)((_tmpA48=_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48[0]=((_tmpA49.tag=0,((_tmpA49.f1=_tmp554,((_tmpA49.f2=_tmp555,_tmpA49)))))),_tmpA48))));}else{
# 1989
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA4C;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA4B;tm=(void*)((_tmpA4B=_region_malloc(r,sizeof(*_tmpA4B)),((_tmpA4B[0]=((_tmpA4C.tag=1,((_tmpA4C.f1=_tmp553,((_tmpA4C.f2=_tmp554,((_tmpA4C.f3=_tmp555,_tmpA4C)))))))),_tmpA4B))));}{
struct Cyc_List_List*_tmpA4F;struct _tuple12 _tmpA4E;return(_tmpA4E.f1=_tmp56E,((_tmpA4E.f2=_tmp56F,((_tmpA4E.f3=((_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F->hd=tm,((_tmpA4F->tl=_tmp570,_tmpA4F)))))),_tmpA4E)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54F;if(_tmp556->tag != 5)goto _LL2D6;else{_tmp557=(_tmp556->f1).elt_typ;_tmp558=(_tmp556->f1).elt_tq;_tmp559=(_tmp556->f1).ptr_atts;}}_LL2D5: {
# 1993
struct Cyc_Absyn_Tqual _tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;struct _tuple12 _tmp577=Cyc_Absynpp_to_tms(r,_tmp558,_tmp557);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;_tmp57A=_tmp577.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA55;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA54;struct Cyc_List_List*_tmpA53;_tmp57A=((_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53->hd=(void*)((_tmpA55=_region_malloc(r,sizeof(*_tmpA55)),((_tmpA55[0]=((_tmpA54.tag=2,((_tmpA54.f1=_tmp559,((_tmpA54.f2=tq,_tmpA54)))))),_tmpA55)))),((_tmpA53->tl=_tmp57A,_tmpA53))))));}{
struct _tuple12 _tmpA56;return(_tmpA56.f1=_tmp578,((_tmpA56.f2=_tmp579,((_tmpA56.f3=_tmp57A,_tmpA56)))));};}_LL2D6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54F;if(_tmp55A->tag != 9)goto _LL2D8;else{_tmp55B=(_tmp55A->f1).tvars;_tmp55C=(_tmp55A->f1).effect;_tmp55D=(_tmp55A->f1).ret_tqual;_tmp55E=(_tmp55A->f1).ret_typ;_tmp55F=(_tmp55A->f1).args;_tmp560=(_tmp55A->f1).c_varargs;_tmp561=(_tmp55A->f1).cyc_varargs;_tmp562=(_tmp55A->f1).rgn_po;_tmp563=(_tmp55A->f1).attributes;}}_LL2D7:
# 1999
 if(!Cyc_Absynpp_print_all_tvars){
# 2003
if(_tmp55C == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp55C)){
_tmp55C=0;
_tmp55B=0;}}else{
# 2008
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2011
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp55B);}{
# 2016
struct Cyc_Absyn_Tqual _tmp580;void*_tmp581;struct Cyc_List_List*_tmp582;struct _tuple12 _tmp57F=Cyc_Absynpp_to_tms(r,_tmp55D,_tmp55E);_tmp580=_tmp57F.f1;_tmp581=_tmp57F.f2;_tmp582=_tmp57F.f3;{
struct Cyc_List_List*tms=_tmp582;
# 2027 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2DE:
# 2029
 if(_tmp563 != 0){
# 2032
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA59;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA58;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA58=_region_malloc(r,sizeof(*_tmpA58)),((_tmpA58[0]=((_tmpA59.tag=5,((_tmpA59.f1=0,((_tmpA59.f2=_tmp563,_tmpA59)))))),_tmpA58)))),tms);}
# 2034
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA68;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA67;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA66;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA65;struct Cyc_List_List*_tmpA64;tms=((_tmpA64=_region_malloc(r,sizeof(*_tmpA64)),((_tmpA64->hd=(void*)((_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68[0]=(
(_tmpA65.tag=3,((_tmpA65.f1=(void*)((_tmpA67=_region_malloc(r,sizeof(*_tmpA67)),((_tmpA67[0]=((_tmpA66.tag=1,((_tmpA66.f1=_tmp55F,((_tmpA66.f2=_tmp560,((_tmpA66.f3=_tmp561,((_tmpA66.f4=_tmp55C,((_tmpA66.f5=_tmp562,_tmpA66)))))))))))),_tmpA67)))),_tmpA65)))),_tmpA68)))),((_tmpA64->tl=tms,_tmpA64))))));}
# 2038
break;case Cyc_Cyclone_Vc_c: _LL2DF:
# 2040
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA77;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA76;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA75;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA74;struct Cyc_List_List*_tmpA73;tms=((_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73->hd=(void*)((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77[0]=(
(_tmpA74.tag=3,((_tmpA74.f1=(void*)((_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76[0]=((_tmpA75.tag=1,((_tmpA75.f1=_tmp55F,((_tmpA75.f2=_tmp560,((_tmpA75.f3=_tmp561,((_tmpA75.f4=_tmp55C,((_tmpA75.f5=_tmp562,_tmpA75)))))))))))),_tmpA76)))),_tmpA74)))),_tmpA77)))),((_tmpA73->tl=tms,_tmpA73))))));}
# 2044
for(0;_tmp563 != 0;_tmp563=_tmp563->tl){
void*_tmp58F=(void*)_tmp563->hd;_LL2E2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp590=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp590->tag != 1)goto _LL2E4;}_LL2E3:
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp591=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp591->tag != 2)goto _LL2E6;}_LL2E5:
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp592=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp592->tag != 3)goto _LL2E8;}_LL2E7:
# 2049
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA81;struct Cyc_List_List*_tmpA80;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA7F;struct Cyc_List_List*_tmpA7E;tms=((_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E->hd=(void*)((_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA7F.tag=5,((_tmpA7F.f1=0,((_tmpA7F.f2=((_tmpA80=_cycalloc(sizeof(*_tmpA80)),((_tmpA80->hd=(void*)_tmp563->hd,((_tmpA80->tl=0,_tmpA80)))))),_tmpA7F)))))),_tmpA81)))),((_tmpA7E->tl=tms,_tmpA7E))))));}
goto AfterAtts;_LL2E8:;_LL2E9:
 goto _LL2E1;_LL2E1:;}
# 2053
break;}
# 2056
AfterAtts:
 if(_tmp55B != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA87;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA86;struct Cyc_List_List*_tmpA85;tms=((_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85->hd=(void*)((_tmpA87=_region_malloc(r,sizeof(*_tmpA87)),((_tmpA87[0]=((_tmpA86.tag=4,((_tmpA86.f1=_tmp55B,((_tmpA86.f2=0,((_tmpA86.f3=1,_tmpA86)))))))),_tmpA87)))),((_tmpA85->tl=tms,_tmpA85))))));}{
struct _tuple12 _tmpA88;return(_tmpA88.f1=_tmp580,((_tmpA88.f2=_tmp581,((_tmpA88.f3=tms,_tmpA88)))));};};};_LL2D8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp54F;if(_tmp564->tag != 1)goto _LL2DA;else{_tmp565=_tmp564->f1;_tmp566=(void*)_tmp564->f2;_tmp567=_tmp564->f3;}}_LL2D9:
# 2062
 if(_tmp566 == 0){
struct _tuple12 _tmpA89;return(_tmpA89.f1=tq,((_tmpA89.f2=t,((_tmpA89.f3=0,_tmpA89)))));}else{
# 2065
return Cyc_Absynpp_to_tms(r,tq,_tmp566);}_LL2DA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp54F;if(_tmp568->tag != 17)goto _LL2DC;else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;_tmp56B=_tmp568->f3;_tmp56C=(void*)_tmp568->f4;}}_LL2DB:
# 2070
 if((_tmp56C == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp56C)){
struct _tuple12 _tmpA8A;return(_tmpA8A.f1=tq,((_tmpA8A.f2=t,((_tmpA8A.f3=0,_tmpA8A)))));}else{
# 2073
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp56C);}_LL2DC:;_LL2DD: {
# 2078
struct _tuple12 _tmpA8B;return(_tmpA8B.f1=tq,((_tmpA8B.f2=t,((_tmpA8B.f3=0,_tmpA8B)))));}_LL2D1:;}
# 2082
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2084
void*_tmp59E=t;void*_tmp5A0;void*_tmp5A2;_LL2EB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp59E;if(_tmp59F->tag != 1)goto _LL2ED;else{_tmp5A0=(void*)_tmp59F->f2;}}if(!(_tmp5A0 != 0))goto _LL2ED;_LL2EC:
 return Cyc_Absynpp_is_char_ptr(_tmp5A0);_LL2ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59E;if(_tmp5A1->tag != 5)goto _LL2EF;else{_tmp5A2=(_tmp5A1->f1).elt_typ;}}_LL2EE:
# 2087
 L: {
void*_tmp5A3=_tmp5A2;void*_tmp5A5;void*_tmp5A7;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A3;if(_tmp5A4->tag != 1)goto _LL2F4;else{_tmp5A5=(void*)_tmp5A4->f2;}}if(!(_tmp5A5 != 0))goto _LL2F4;_LL2F3:
 _tmp5A2=_tmp5A5;goto L;_LL2F4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A3;if(_tmp5A6->tag != 17)goto _LL2F6;else{_tmp5A7=(void*)_tmp5A6->f4;}}if(!(_tmp5A7 != 0))goto _LL2F6;_LL2F5:
 _tmp5A2=_tmp5A7;goto L;_LL2F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3;if(_tmp5A8->tag != 6)goto _LL2F8;else{if(_tmp5A8->f2 != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
 return 1;_LL2F8:;_LL2F9:
 return 0;_LL2F1:;}_LL2EF:;_LL2F0:
# 2094
 return 0;_LL2EA:;}
# 2098
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5A9=_new_region("temp");struct _RegionHandle*temp=& _tmp5A9;_push_region(temp);
{struct Cyc_Absyn_Tqual _tmp5AB;void*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct _tuple12 _tmp5AA=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;_tmp5AD=_tmp5AA.f3;
_tmp5AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5AD);
if(_tmp5AD == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA8C[2];struct Cyc_PP_Doc*_tmp5AF=(_tmpA8C[1]=Cyc_Absynpp_ntyp2doc(_tmp5AC),((_tmpA8C[0]=Cyc_Absynpp_tqual2doc(_tmp5AB),Cyc_PP_cat(_tag_dyneither(_tmpA8C,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5AF;}else{
# 2105
const char*_tmpA8F;struct Cyc_PP_Doc*_tmpA8E[4];struct Cyc_PP_Doc*_tmp5B2=
(_tmpA8E[3]=
# 2109
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5AD),((_tmpA8E[2]=
# 2108
Cyc_PP_text(((_tmpA8F=" ",_tag_dyneither(_tmpA8F,sizeof(char),2)))),((_tmpA8E[1]=
# 2107
Cyc_Absynpp_ntyp2doc(_tmp5AC),((_tmpA8E[0]=
# 2106
Cyc_Absynpp_tqual2doc(_tmp5AB),Cyc_PP_cat(_tag_dyneither(_tmpA8E,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B2;}}
# 2100
;_pop_region(temp);}
# 2113
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA:
# 2116
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA96;const char*_tmpA95;const char*_tmpA94;struct Cyc_PP_Doc*_tmpA93[5];return(_tmpA93[4]=
# 2119
Cyc_PP_text(((_tmpA94=";",_tag_dyneither(_tmpA94,sizeof(char),2)))),((_tmpA93[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA93[2]=
# 2118
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA93[1]=Cyc_PP_text(((_tmpA95=":",_tag_dyneither(_tmpA95,sizeof(char),2)))),((_tmpA93[0]=
# 2117
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA96=_cycalloc(sizeof(*_tmpA96)),((_tmpA96->v=Cyc_PP_textptr(f->name),_tmpA96))))),Cyc_PP_cat(_tag_dyneither(_tmpA93,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2121
struct Cyc_Core_Opt*_tmpA9B;const char*_tmpA9A;struct Cyc_PP_Doc*_tmpA99[3];return(_tmpA99[2]=
Cyc_PP_text(((_tmpA9A=";",_tag_dyneither(_tmpA9A,sizeof(char),2)))),((_tmpA99[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA99[0]=
# 2121
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->v=Cyc_PP_textptr(f->name),_tmpA9B))))),Cyc_PP_cat(_tag_dyneither(_tmpA99,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FB:
# 2124
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAA2;const char*_tmpAA1;const char*_tmpAA0;struct Cyc_PP_Doc*_tmpA9F[5];return(_tmpA9F[4]=
# 2127
Cyc_PP_text(((_tmpAA0=";",_tag_dyneither(_tmpAA0,sizeof(char),2)))),((_tmpA9F[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA9F[2]=Cyc_PP_text(((_tmpAA1=":",_tag_dyneither(_tmpAA1,sizeof(char),2)))),((_tmpA9F[1]=
# 2126
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA2=_cycalloc(sizeof(*_tmpAA2)),((_tmpAA2->v=Cyc_PP_textptr(f->name),_tmpAA2))))),((_tmpA9F[0]=
# 2125
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA9F,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2129
struct Cyc_Core_Opt*_tmpAA7;const char*_tmpAA6;struct Cyc_PP_Doc*_tmpAA5[3];return(_tmpAA5[2]=
Cyc_PP_text(((_tmpAA6=";",_tag_dyneither(_tmpAA6,sizeof(char),2)))),((_tmpAA5[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA7=_cycalloc(sizeof(*_tmpAA7)),((_tmpAA7->v=Cyc_PP_textptr(f->name),_tmpAA7))))),((_tmpAA5[0]=
# 2129
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA5,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2135
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAA8;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAA8="",_tag_dyneither(_tmpAA8,sizeof(char),1))),fields);}
# 2139
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAA9[3];return(_tmpAA9[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAA9[1]=
# 2140
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAA9[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAA9,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2144
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAAA;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAAA=",",_tag_dyneither(_tmpAAA,sizeof(char),2))),fields);}
# 2153
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpAAD;void*_tmpAAC;(_tmpAAC=0,Cyc_fprintf(f,((_tmpAAD="\n",_tag_dyneither(_tmpAAD,sizeof(char),2))),_tag_dyneither(_tmpAAC,sizeof(void*),0)));};}}
# 2160
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpAAE;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAAE="",_tag_dyneither(_tmpAAE,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2163
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2167
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2177
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
