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
# 297 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 338
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
struct Cyc_List_List*_tmp5C9;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5C9=_cycalloc(sizeof(*_tmp5C9)),((_tmp5C9->hd=v,((_tmp5C9->tl=0,_tmp5C9)))))));}
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
const char*_tmp5CA;return(_tmp5CA="\\a",_tag_dyneither(_tmp5CA,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5CB;return(_tmp5CB="\\b",_tag_dyneither(_tmp5CB,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5CC;return(_tmp5CC="\\f",_tag_dyneither(_tmp5CC,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5CD;return(_tmp5CD="\\n",_tag_dyneither(_tmp5CD,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5CE;return(_tmp5CE="\\r",_tag_dyneither(_tmp5CE,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5CF;return(_tmp5CF="\\t",_tag_dyneither(_tmp5CF,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5D0;return(_tmp5D0="\\v",_tag_dyneither(_tmp5D0,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5D1;return(_tmp5D1="\\\\",_tag_dyneither(_tmp5D1,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5D2;return(_tmp5D2="\"",_tag_dyneither(_tmp5D2,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5D3;return(_tmp5D3="\\'",_tag_dyneither(_tmp5D3,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5D6;char _tmp5D5;struct _dyneither_ptr _tmp5D4;(_tmp5D4=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5D5=*((char*)_check_dyneither_subscript(_tmp5D4,sizeof(char),0)),((_tmp5D6=c,((_get_dyneither_size(_tmp5D4,sizeof(char))== 1  && (_tmp5D5 == '\000'  && _tmp5D6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D4.curr)=_tmp5D6)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp5D9;char _tmp5D8;struct _dyneither_ptr _tmp5D7;(_tmp5D7=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5D8=*((char*)_check_dyneither_subscript(_tmp5D7,sizeof(char),0)),((_tmp5D9='\\',((_get_dyneither_size(_tmp5D7,sizeof(char))== 1  && (_tmp5D8 == '\000'  && _tmp5D9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D7.curr)=_tmp5D9)))))));}
{char _tmp5DC;char _tmp5DB;struct _dyneither_ptr _tmp5DA;(_tmp5DA=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DB=*((char*)_check_dyneither_subscript(_tmp5DA,sizeof(char),0)),((_tmp5DC=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5DA,sizeof(char))== 1  && (_tmp5DB == '\000'  && _tmp5DC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DA.curr)=_tmp5DC)))))));}
{char _tmp5DF;char _tmp5DE;struct _dyneither_ptr _tmp5DD;(_tmp5DD=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DE=*((char*)_check_dyneither_subscript(_tmp5DD,sizeof(char),0)),((_tmp5DF=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5DD,sizeof(char))== 1  && (_tmp5DE == '\000'  && _tmp5DF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DD.curr)=_tmp5DF)))))));}
{char _tmp5E2;char _tmp5E1;struct _dyneither_ptr _tmp5E0;(_tmp5E0=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E1=*((char*)_check_dyneither_subscript(_tmp5E0,sizeof(char),0)),((_tmp5E2=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp5E0,sizeof(char))== 1  && (_tmp5E1 == '\000'  && _tmp5E2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E0.curr)=_tmp5E2)))))));}
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
{char _tmp5E5;char _tmp5E4;struct _dyneither_ptr _tmp5E3;(_tmp5E3=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E4=*((char*)_check_dyneither_subscript(_tmp5E3,sizeof(char),0)),((_tmp5E5='\\',((_get_dyneither_size(_tmp5E3,sizeof(char))== 1  && (_tmp5E4 == '\000'  && _tmp5E5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E3.curr)=_tmp5E5)))))));}{char _tmp5E8;char _tmp5E7;struct _dyneither_ptr _tmp5E6;(_tmp5E6=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E7=*((char*)_check_dyneither_subscript(_tmp5E6,sizeof(char),0)),((_tmp5E8='a',((_get_dyneither_size(_tmp5E6,sizeof(char))== 1  && (_tmp5E7 == '\000'  && _tmp5E8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E6.curr)=_tmp5E8)))))));}goto _LL21;_LL24: if(_tmp1F != '\b')goto _LL26;_LL25:
{char _tmp5EB;char _tmp5EA;struct _dyneither_ptr _tmp5E9;(_tmp5E9=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EA=*((char*)_check_dyneither_subscript(_tmp5E9,sizeof(char),0)),((_tmp5EB='\\',((_get_dyneither_size(_tmp5E9,sizeof(char))== 1  && (_tmp5EA == '\000'  && _tmp5EB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E9.curr)=_tmp5EB)))))));}{char _tmp5EE;char _tmp5ED;struct _dyneither_ptr _tmp5EC;(_tmp5EC=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5ED=*((char*)_check_dyneither_subscript(_tmp5EC,sizeof(char),0)),((_tmp5EE='b',((_get_dyneither_size(_tmp5EC,sizeof(char))== 1  && (_tmp5ED == '\000'  && _tmp5EE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EC.curr)=_tmp5EE)))))));}goto _LL21;_LL26: if(_tmp1F != '\f')goto _LL28;_LL27:
{char _tmp5F1;char _tmp5F0;struct _dyneither_ptr _tmp5EF;(_tmp5EF=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F0=*((char*)_check_dyneither_subscript(_tmp5EF,sizeof(char),0)),((_tmp5F1='\\',((_get_dyneither_size(_tmp5EF,sizeof(char))== 1  && (_tmp5F0 == '\000'  && _tmp5F1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EF.curr)=_tmp5F1)))))));}{char _tmp5F4;char _tmp5F3;struct _dyneither_ptr _tmp5F2;(_tmp5F2=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F3=*((char*)_check_dyneither_subscript(_tmp5F2,sizeof(char),0)),((_tmp5F4='f',((_get_dyneither_size(_tmp5F2,sizeof(char))== 1  && (_tmp5F3 == '\000'  && _tmp5F4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F2.curr)=_tmp5F4)))))));}goto _LL21;_LL28: if(_tmp1F != '\n')goto _LL2A;_LL29:
{char _tmp5F7;char _tmp5F6;struct _dyneither_ptr _tmp5F5;(_tmp5F5=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F6=*((char*)_check_dyneither_subscript(_tmp5F5,sizeof(char),0)),((_tmp5F7='\\',((_get_dyneither_size(_tmp5F5,sizeof(char))== 1  && (_tmp5F6 == '\000'  && _tmp5F7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F5.curr)=_tmp5F7)))))));}{char _tmp5FA;char _tmp5F9;struct _dyneither_ptr _tmp5F8;(_tmp5F8=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F9=*((char*)_check_dyneither_subscript(_tmp5F8,sizeof(char),0)),((_tmp5FA='n',((_get_dyneither_size(_tmp5F8,sizeof(char))== 1  && (_tmp5F9 == '\000'  && _tmp5FA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F8.curr)=_tmp5FA)))))));}goto _LL21;_LL2A: if(_tmp1F != '\r')goto _LL2C;_LL2B:
{char _tmp5FD;char _tmp5FC;struct _dyneither_ptr _tmp5FB;(_tmp5FB=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FC=*((char*)_check_dyneither_subscript(_tmp5FB,sizeof(char),0)),((_tmp5FD='\\',((_get_dyneither_size(_tmp5FB,sizeof(char))== 1  && (_tmp5FC == '\000'  && _tmp5FD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FB.curr)=_tmp5FD)))))));}{char _tmp600;char _tmp5FF;struct _dyneither_ptr _tmp5FE;(_tmp5FE=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FF=*((char*)_check_dyneither_subscript(_tmp5FE,sizeof(char),0)),((_tmp600='r',((_get_dyneither_size(_tmp5FE,sizeof(char))== 1  && (_tmp5FF == '\000'  && _tmp600 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FE.curr)=_tmp600)))))));}goto _LL21;_LL2C: if(_tmp1F != '\t')goto _LL2E;_LL2D:
{char _tmp603;char _tmp602;struct _dyneither_ptr _tmp601;(_tmp601=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp602=*((char*)_check_dyneither_subscript(_tmp601,sizeof(char),0)),((_tmp603='\\',((_get_dyneither_size(_tmp601,sizeof(char))== 1  && (_tmp602 == '\000'  && _tmp603 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp601.curr)=_tmp603)))))));}{char _tmp606;char _tmp605;struct _dyneither_ptr _tmp604;(_tmp604=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp605=*((char*)_check_dyneither_subscript(_tmp604,sizeof(char),0)),((_tmp606='t',((_get_dyneither_size(_tmp604,sizeof(char))== 1  && (_tmp605 == '\000'  && _tmp606 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp604.curr)=_tmp606)))))));}goto _LL21;_LL2E: if(_tmp1F != '\v')goto _LL30;_LL2F:
{char _tmp609;char _tmp608;struct _dyneither_ptr _tmp607;(_tmp607=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp608=*((char*)_check_dyneither_subscript(_tmp607,sizeof(char),0)),((_tmp609='\\',((_get_dyneither_size(_tmp607,sizeof(char))== 1  && (_tmp608 == '\000'  && _tmp609 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp607.curr)=_tmp609)))))));}{char _tmp60C;char _tmp60B;struct _dyneither_ptr _tmp60A;(_tmp60A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60B=*((char*)_check_dyneither_subscript(_tmp60A,sizeof(char),0)),((_tmp60C='v',((_get_dyneither_size(_tmp60A,sizeof(char))== 1  && (_tmp60B == '\000'  && _tmp60C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60A.curr)=_tmp60C)))))));}goto _LL21;_LL30: if(_tmp1F != '\\')goto _LL32;_LL31:
{char _tmp60F;char _tmp60E;struct _dyneither_ptr _tmp60D;(_tmp60D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60E=*((char*)_check_dyneither_subscript(_tmp60D,sizeof(char),0)),((_tmp60F='\\',((_get_dyneither_size(_tmp60D,sizeof(char))== 1  && (_tmp60E == '\000'  && _tmp60F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60D.curr)=_tmp60F)))))));}{char _tmp612;char _tmp611;struct _dyneither_ptr _tmp610;(_tmp610=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp611=*((char*)_check_dyneither_subscript(_tmp610,sizeof(char),0)),((_tmp612='\\',((_get_dyneither_size(_tmp610,sizeof(char))== 1  && (_tmp611 == '\000'  && _tmp612 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp610.curr)=_tmp612)))))));}goto _LL21;_LL32: if(_tmp1F != '"')goto _LL34;_LL33:
{char _tmp615;char _tmp614;struct _dyneither_ptr _tmp613;(_tmp613=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp614=*((char*)_check_dyneither_subscript(_tmp613,sizeof(char),0)),((_tmp615='\\',((_get_dyneither_size(_tmp613,sizeof(char))== 1  && (_tmp614 == '\000'  && _tmp615 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp613.curr)=_tmp615)))))));}{char _tmp618;char _tmp617;struct _dyneither_ptr _tmp616;(_tmp616=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp617=*((char*)_check_dyneither_subscript(_tmp616,sizeof(char),0)),((_tmp618='"',((_get_dyneither_size(_tmp616,sizeof(char))== 1  && (_tmp617 == '\000'  && _tmp618 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp616.curr)=_tmp618)))))));}goto _LL21;_LL34: _tmp20=_tmp1F;_LL35:
# 341
 if(_tmp20 >= ' '  && _tmp20 <= '~'){char _tmp61B;char _tmp61A;struct _dyneither_ptr _tmp619;(_tmp619=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61A=*((char*)_check_dyneither_subscript(_tmp619,sizeof(char),0)),((_tmp61B=_tmp20,((_get_dyneither_size(_tmp619,sizeof(char))== 1  && (_tmp61A == '\000'  && _tmp61B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp619.curr)=_tmp61B)))))));}else{
# 343
{char _tmp61E;char _tmp61D;struct _dyneither_ptr _tmp61C;(_tmp61C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61D=*((char*)_check_dyneither_subscript(_tmp61C,sizeof(char),0)),((_tmp61E='\\',((_get_dyneither_size(_tmp61C,sizeof(char))== 1  && (_tmp61D == '\000'  && _tmp61E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61C.curr)=_tmp61E)))))));}
{char _tmp621;char _tmp620;struct _dyneither_ptr _tmp61F;(_tmp61F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp620=*((char*)_check_dyneither_subscript(_tmp61F,sizeof(char),0)),((_tmp621=(char)('0' + (_tmp20 >> 6 & 7)),((_get_dyneither_size(_tmp61F,sizeof(char))== 1  && (_tmp620 == '\000'  && _tmp621 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61F.curr)=_tmp621)))))));}
{char _tmp624;char _tmp623;struct _dyneither_ptr _tmp622;(_tmp622=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp623=*((char*)_check_dyneither_subscript(_tmp622,sizeof(char),0)),((_tmp624=(char)('0' + (_tmp20 >> 3 & 7)),((_get_dyneither_size(_tmp622,sizeof(char))== 1  && (_tmp623 == '\000'  && _tmp624 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp622.curr)=_tmp624)))))));}{
char _tmp627;char _tmp626;struct _dyneither_ptr _tmp625;(_tmp625=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp626=*((char*)_check_dyneither_subscript(_tmp625,sizeof(char),0)),((_tmp627=(char)('0' + (_tmp20 & 7)),((_get_dyneither_size(_tmp625,sizeof(char))== 1  && (_tmp626 == '\000'  && _tmp627 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp625.curr)=_tmp627)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp628;l=((_tmp628=_cycalloc(sizeof(*_tmp628)),((_tmp628->hd=Cyc_Absynpp_restrict_sp,((_tmp628->tl=l,_tmp628))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp629;l=((_tmp629=_cycalloc(sizeof(*_tmp629)),((_tmp629->hd=Cyc_Absynpp_volatile_sp,((_tmp629->tl=l,_tmp629))))));}
if(tq.print_const){struct Cyc_List_List*_tmp62A;l=((_tmp62A=_cycalloc(sizeof(*_tmp62A)),((_tmp62A->hd=Cyc_Absynpp_const_sp,((_tmp62A->tl=l,_tmp62A))))));}{
const char*_tmp62D;const char*_tmp62C;const char*_tmp62B;return Cyc_PP_egroup(((_tmp62B="",_tag_dyneither(_tmp62B,sizeof(char),1))),((_tmp62C=" ",_tag_dyneither(_tmp62C,sizeof(char),2))),((_tmp62D=" ",_tag_dyneither(_tmp62D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=(*_tmp6F).kind;_tmp71=(*_tmp6F).aliasqual;
switch(_tmp70){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp62E;return(_tmp62E="A",_tag_dyneither(_tmp62E,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp62F;return(_tmp62F="UA",_tag_dyneither(_tmp62F,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp630;return(_tmp630="TA",_tag_dyneither(_tmp630,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp631;return(_tmp631="M",_tag_dyneither(_tmp631,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp632;return(_tmp632="UM",_tag_dyneither(_tmp632,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp633;return(_tmp633="TM",_tag_dyneither(_tmp633,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp634;return(_tmp634="B",_tag_dyneither(_tmp634,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp635;return(_tmp635="UB",_tag_dyneither(_tmp635,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp636;return(_tmp636="TB",_tag_dyneither(_tmp636,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp637;return(_tmp637="R",_tag_dyneither(_tmp637,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp638;return(_tmp638="UR",_tag_dyneither(_tmp638,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp639;return(_tmp639="TR",_tag_dyneither(_tmp639,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp63A;return(_tmp63A="E",_tag_dyneither(_tmp63A,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp63B;return(_tmp63B="I",_tag_dyneither(_tmp63B,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp80=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp82;struct Cyc_Absyn_Kind*_tmp85;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp81->tag != 0)goto _LL50;else{_tmp82=_tmp81->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp82);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp83->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp63C;return(_tmp63C="{?}",_tag_dyneither(_tmp63C,sizeof(char),4));}else{
const char*_tmp63D;return(_tmp63D="?",_tag_dyneither(_tmp63D,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp84->tag != 2)goto _LL4D;else{_tmp85=_tmp84->f2;}}_LL53: {
const char*_tmp641;void*_tmp640[1];struct Cyc_String_pa_PrintArg_struct _tmp63F;return(struct _dyneither_ptr)((_tmp63F.tag=0,((_tmp63F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp85)),((_tmp640[0]=& _tmp63F,Cyc_aprintf(((_tmp641="<=%s",_tag_dyneither(_tmp641,sizeof(char),5))),_tag_dyneither(_tmp640,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8B=Cyc_Absyn_compress_kb(c);struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp90;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL57;else{_tmp8D=_tmp8C->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8D));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8E->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp642;return Cyc_PP_text_width(((_tmp642="{?}",_tag_dyneither(_tmp642,sizeof(char),4))),1);}else{
const char*_tmp643;return Cyc_PP_text(((_tmp643="?",_tag_dyneither(_tmp643,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8F->tag != 2)goto _LL54;else{_tmp90=_tmp8F->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp90));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp646;const char*_tmp645;const char*_tmp644;return Cyc_PP_egroup(((_tmp644="<",_tag_dyneither(_tmp644,sizeof(char),2))),((_tmp645=">",_tag_dyneither(_tmp645,sizeof(char),2))),((_tmp646=",",_tag_dyneither(_tmp646,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp96=*tv->name;
if(*((const char*)_check_dyneither_subscript(_tmp96,sizeof(char),0))== '#'){
const char*_tmp649;struct Cyc_PP_Doc*_tmp648[2];return(_tmp648[1]=Cyc_PP_text(_dyneither_ptr_plus(_tmp96,sizeof(char),1)),((_tmp648[0]=Cyc_PP_text(((_tmp649="`TTT",_tag_dyneither(_tmp649,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp648,sizeof(struct Cyc_PP_Doc*),2)))));}
return Cyc_PP_text(_tmp96);}
# 435
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp99=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL5E;}_LL5D:
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 0)goto _LL60;else{if((*_tmp9B->f1).kind != Cyc_Absyn_BoxKind)goto _LL60;if((*_tmp9B->f1).aliasqual != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61:
 _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {
const char*_tmp64C;struct Cyc_PP_Doc*_tmp64B[3];return(_tmp64B[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp64B[1]=Cyc_PP_text(((_tmp64C="::",_tag_dyneither(_tmp64C,sizeof(char),3)))),((_tmp64B[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp64B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 444
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp64F;const char*_tmp64E;const char*_tmp64D;return Cyc_PP_egroup(((_tmp64D="<",_tag_dyneither(_tmp64D,sizeof(char),2))),((_tmp64E=">",_tag_dyneither(_tmp64E,sizeof(char),2))),((_tmp64F=",",_tag_dyneither(_tmp64F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 448
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}
# 452
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp652;const char*_tmp651;const char*_tmp650;return Cyc_PP_egroup(((_tmp650="<",_tag_dyneither(_tmp650,sizeof(char),2))),((_tmp651=">",_tag_dyneither(_tmp651,sizeof(char),2))),((_tmp652=",",_tag_dyneither(_tmp652,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 458
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 462
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp655;const char*_tmp654;const char*_tmp653;return Cyc_PP_group(((_tmp653="(",_tag_dyneither(_tmp653,sizeof(char),2))),((_tmp654=")",_tag_dyneither(_tmp654,sizeof(char),2))),((_tmp655=",",_tag_dyneither(_tmp655,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
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
const char*_tmp656;return Cyc_PP_text(((_tmp656=" _stdcall ",_tag_dyneither(_tmp656,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}_LL71: {
const char*_tmp657;return Cyc_PP_text(((_tmp657=" _cdecl ",_tag_dyneither(_tmp657,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp658;return Cyc_PP_text(((_tmp658=" _fastcall ",_tag_dyneither(_tmp658,sizeof(char),12))));}_LL74:;_LL75:
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
const char*_tmp663;const char*_tmp662;const char*_tmp661;const char*_tmp660;const char*_tmp65F;struct Cyc_PP_Doc*_tmp65E[3];return(_tmp65E[2]=
# 499
Cyc_PP_text(((_tmp65F=")",_tag_dyneither(_tmp65F,sizeof(char),2)))),((_tmp65E[1]=
# 498
Cyc_PP_group(((_tmp662="",_tag_dyneither(_tmp662,sizeof(char),1))),((_tmp661="",_tag_dyneither(_tmp661,sizeof(char),1))),((_tmp660=" ",_tag_dyneither(_tmp660,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp65E[0]=
# 497
Cyc_PP_text(((_tmp663=" __declspec(",_tag_dyneither(_tmp663,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp65E,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 502
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 506
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp66C;const char*_tmp66B;const char*_tmp66A;const char*_tmp669;struct Cyc_PP_Doc*_tmp668[2];return(_tmp668[1]=
Cyc_PP_group(((_tmp66B="((",_tag_dyneither(_tmp66B,sizeof(char),3))),((_tmp66A="))",_tag_dyneither(_tmp66A,sizeof(char),3))),((_tmp669=",",_tag_dyneither(_tmp669,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp668[0]=
# 510
Cyc_PP_text(((_tmp66C=" __attribute__",_tag_dyneither(_tmp66C,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp668,sizeof(struct Cyc_PP_Doc*),2)))));}}}
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
const char*_tmp66D;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp66D="{?}",_tag_dyneither(_tmp66D,sizeof(char),4))),1);}else{
const char*_tmp66E;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp66E="?",_tag_dyneither(_tmp66E,sizeof(char),2))));}}
# 538
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 540
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp66F;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp66F="{\\lb}",_tag_dyneither(_tmp66F,sizeof(char),6))),1);}else{
const char*_tmp670;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp670="{",_tag_dyneither(_tmp670,sizeof(char),2))));}}
# 547
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 549
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp671;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp671="{\\rb}",_tag_dyneither(_tmp671,sizeof(char),6))),1);}else{
const char*_tmp672;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp672="}",_tag_dyneither(_tmp672,sizeof(char),2))));}}
# 556
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 558
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp673;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp673="\\$",_tag_dyneither(_tmp673,sizeof(char),3))),1);}else{
const char*_tmp674;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp674="$",_tag_dyneither(_tmp674,sizeof(char),2))));}}
# 565
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 567
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp675[3];return(_tmp675[2]=Cyc_Absynpp_rb(),((_tmp675[1]=Cyc_PP_seq(sep,ss),((_tmp675[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp675,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 572
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpD1=(void*)tms->hd;struct Cyc_List_List*_tmpD6;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:
{const char*_tmp678;void*_tmp677;(_tmp677=0,Cyc_fprintf(Cyc_stderr,((_tmp678="Carray_mod ",_tag_dyneither(_tmp678,sizeof(char),12))),_tag_dyneither(_tmp677,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:
{const char*_tmp67B;void*_tmp67A;(_tmp67A=0,Cyc_fprintf(Cyc_stderr,((_tmp67B="ConstArray_mod ",_tag_dyneither(_tmp67B,sizeof(char),16))),_tag_dyneither(_tmp67A,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpD4->f1);if(_tmpD5->tag != 1)goto _LL93;else{_tmpD6=_tmpD5->f1;}}}_LL92:
# 578
{const char*_tmp67E;void*_tmp67D;(_tmp67D=0,Cyc_fprintf(Cyc_stderr,((_tmp67E="Function_mod(",_tag_dyneither(_tmp67E,sizeof(char),14))),_tag_dyneither(_tmp67D,sizeof(void*),0)));}
for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _dyneither_ptr*_tmpE1=(*((struct _tuple8*)_tmpD6->hd)).f1;
if(_tmpE1 == 0){const char*_tmp681;void*_tmp680;(_tmp680=0,Cyc_fprintf(Cyc_stderr,((_tmp681="?",_tag_dyneither(_tmp681,sizeof(char),2))),_tag_dyneither(_tmp680,sizeof(void*),0)));}else{
void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,*_tmpE1,_tag_dyneither(_tmp682,sizeof(void*),0)));}
if(_tmpD6->tl != 0){const char*_tmp685;void*_tmp684;(_tmp684=0,Cyc_fprintf(Cyc_stderr,((_tmp685=",",_tag_dyneither(_tmp685,sizeof(char),2))),_tag_dyneither(_tmp684,sizeof(void*),0)));}}
# 585
{const char*_tmp688;void*_tmp687;(_tmp687=0,Cyc_fprintf(Cyc_stderr,((_tmp688=") ",_tag_dyneither(_tmp688,sizeof(char),3))),_tag_dyneither(_tmp687,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD7->tag != 3)goto _LL95;}_LL94:
# 588
{const char*_tmp68B;void*_tmp68A;(_tmp68A=0,Cyc_fprintf(Cyc_stderr,((_tmp68B="Function_mod()",_tag_dyneither(_tmp68B,sizeof(char),15))),_tag_dyneither(_tmp68A,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD8->tag != 5)goto _LL97;}_LL96:
{const char*_tmp68E;void*_tmp68D;(_tmp68D=0,Cyc_fprintf(Cyc_stderr,((_tmp68E="Attributes_mod ",_tag_dyneither(_tmp68E,sizeof(char),16))),_tag_dyneither(_tmp68D,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD9->tag != 4)goto _LL99;}_LL98:
{const char*_tmp691;void*_tmp690;(_tmp690=0,Cyc_fprintf(Cyc_stderr,((_tmp691="TypeParams_mod ",_tag_dyneither(_tmp691,sizeof(char),16))),_tag_dyneither(_tmp690,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDA->tag != 2)goto _LL8C;}_LL9A:
{const char*_tmp694;void*_tmp693;(_tmp693=0,Cyc_fprintf(Cyc_stderr,((_tmp694="Pointer_mod ",_tag_dyneither(_tmp694,sizeof(char),13))),_tag_dyneither(_tmp693,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 593
const char*_tmp697;void*_tmp696;(_tmp696=0,Cyc_fprintf(Cyc_stderr,((_tmp697="\n",_tag_dyneither(_tmp697,sizeof(char),2))),_tag_dyneither(_tmp696,sizeof(void*),0)));};}
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 598
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp69C;const char*_tmp69B;struct Cyc_PP_Doc*_tmp69A[3];struct Cyc_PP_Doc*p_rest=(_tmp69A[2]=Cyc_PP_text(((_tmp69B=")",_tag_dyneither(_tmp69B,sizeof(char),2)))),((_tmp69A[1]=rest,((_tmp69A[0]=Cyc_PP_text(((_tmp69C="(",_tag_dyneither(_tmp69C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp69A,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmpF3=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;union Cyc_Absyn_Constraint*_tmpF8;void*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;unsigned int _tmpFF;int _tmp100;void*_tmp102;union Cyc_Absyn_Constraint*_tmp103;union Cyc_Absyn_Constraint*_tmp104;union Cyc_Absyn_Constraint*_tmp105;struct Cyc_Absyn_Tqual _tmp106;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF4->tag != 0)goto _LL9E;else{_tmpF5=_tmpF4->f1;}}_LL9D:
# 604
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A1;const char*_tmp6A0;struct Cyc_PP_Doc*_tmp69F[2];return(_tmp69F[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF5)?
Cyc_PP_text(((_tmp6A0="[]@zeroterm",_tag_dyneither(_tmp6A0,sizeof(char),12)))): Cyc_PP_text(((_tmp6A1="[]",_tag_dyneither(_tmp6A1,sizeof(char),3)))),((_tmp69F[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp69F,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF6=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF6->tag != 1)goto _LLA0;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL9F:
# 608
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A8;const char*_tmp6A7;const char*_tmp6A6;struct Cyc_PP_Doc*_tmp6A5[4];return(_tmp6A5[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8)?Cyc_PP_text(((_tmp6A6="]@zeroterm",_tag_dyneither(_tmp6A6,sizeof(char),11)))): Cyc_PP_text(((_tmp6A7="]",_tag_dyneither(_tmp6A7,sizeof(char),2)))),((_tmp6A5[2]=
# 609
Cyc_Absynpp_exp2doc(_tmpF7),((_tmp6A5[1]=Cyc_PP_text(((_tmp6A8="[",_tag_dyneither(_tmp6A8,sizeof(char),2)))),((_tmp6A5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A5,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpF9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF9->tag != 3)goto _LLA2;else{_tmpFA=(void*)_tmpF9->f1;}}_LLA1:
# 612
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp10E=_tmpFA;struct Cyc_List_List*_tmp110;int _tmp111;struct Cyc_Absyn_VarargInfo*_tmp112;void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp116;unsigned int _tmp117;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp10F=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp10F->tag != 1)goto _LLAB;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;_tmp113=(void*)_tmp10F->f4;_tmp114=_tmp10F->f5;}}_LLAA: {
# 615
struct Cyc_PP_Doc*_tmp6A9[2];return(_tmp6A9[1]=Cyc_Absynpp_funargs2doc(_tmp110,_tmp111,_tmp112,_tmp113,_tmp114),((_tmp6A9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A9,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp115=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp115->tag != 0)goto _LLA8;else{_tmp116=_tmp115->f1;_tmp117=_tmp115->f2;}}_LLAC: {
# 617
const char*_tmp6B0;const char*_tmp6AF;const char*_tmp6AE;struct Cyc_PP_Doc*_tmp6AD[2];return(_tmp6AD[1]=Cyc_PP_group(((_tmp6B0="(",_tag_dyneither(_tmp6B0,sizeof(char),2))),((_tmp6AF=")",_tag_dyneither(_tmp6AF,sizeof(char),2))),((_tmp6AE=",",_tag_dyneither(_tmp6AE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp116)),((_tmp6AD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AD,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFB->tag != 5)goto _LLA4;else{_tmpFC=_tmpFB->f2;}}_LLA3:
# 620
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 622
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B1[2];return(_tmp6B1[1]=Cyc_Absynpp_atts2doc(_tmpFC),((_tmp6B1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 625
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6B2[2];return(_tmp6B2[1]=rest,((_tmp6B2[0]=Cyc_Absynpp_callconv2doc(_tmpFC),Cyc_PP_cat(_tag_dyneither(_tmp6B2,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFD->tag != 4)goto _LLA6;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_tmp100=_tmpFD->f3;}}_LLA5:
# 630
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp100){
struct Cyc_PP_Doc*_tmp6B3[2];return(_tmp6B3[1]=Cyc_Absynpp_ktvars2doc(_tmpFE),((_tmp6B3[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B3,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 634
struct Cyc_PP_Doc*_tmp6B4[2];return(_tmp6B4[1]=Cyc_Absynpp_tvars2doc(_tmpFE),((_tmp6B4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp101=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmp101->tag != 2)goto _LL9B;else{_tmp102=(_tmp101->f1).rgn;_tmp103=(_tmp101->f1).nullable;_tmp104=(_tmp101->f1).bounds;_tmp105=(_tmp101->f1).zero_term;_tmp106=_tmp101->f2;}}_LLA7: {
# 638
struct Cyc_PP_Doc*ptr;
{void*_tmp121=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp104);struct Cyc_Absyn_Exp*_tmp124;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp122=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp121;if(_tmp122->tag != 0)goto _LLB3;}_LLB2:
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp123=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp121;if(_tmp123->tag != 1)goto _LLB0;else{_tmp124=_tmp123->f1;}}_LLB4:
# 642
{const char*_tmp6B6;const char*_tmp6B5;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp103)?(_tmp6B6="*",_tag_dyneither(_tmp6B6,sizeof(char),2)):((_tmp6B5="@",_tag_dyneither(_tmp6B5,sizeof(char),2))));}{
unsigned int _tmp128;int _tmp129;struct _tuple11 _tmp127=Cyc_Evexp_eval_const_uint_exp(_tmp124);_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;
if(!_tmp129  || _tmp128 != 1){
struct Cyc_PP_Doc*_tmp6B7[4];ptr=((_tmp6B7[3]=Cyc_Absynpp_rb(),((_tmp6B7[2]=Cyc_Absynpp_exp2doc(_tmp124),((_tmp6B7[1]=Cyc_Absynpp_lb(),((_tmp6B7[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}
# 648
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6BA;struct Cyc_PP_Doc*_tmp6B9[2];ptr=((_tmp6B9[1]=Cyc_PP_text(((_tmp6BA="@zeroterm ",_tag_dyneither(_tmp6BA,sizeof(char),11)))),((_tmp6B9[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),2))))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6BD;struct Cyc_PP_Doc*_tmp6BC[2];ptr=((_tmp6BC[1]=Cyc_PP_text(((_tmp6BD="@nozeroterm ",_tag_dyneither(_tmp6BD,sizeof(char),13)))),((_tmp6BC[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 654
{void*_tmp12F=Cyc_Tcutil_compress(_tmp102);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp130=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12F;if(_tmp130->tag != 20)goto _LLB8;}_LLB7:
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp131=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12F;if(_tmp131->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
 goto _LLB5;_LLBA:;_LLBB: {
const char*_tmp6C0;struct Cyc_PP_Doc*_tmp6BF[3];ptr=((_tmp6BF[2]=Cyc_PP_text(((_tmp6C0=" ",_tag_dyneither(_tmp6C0,sizeof(char),2)))),((_tmp6BF[1]=Cyc_Absynpp_rgn2doc(_tmp102),((_tmp6BF[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 659
{struct Cyc_PP_Doc*_tmp6C1[2];ptr=((_tmp6C1[1]=Cyc_Absynpp_tqual2doc(_tmp106),((_tmp6C1[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C1,sizeof(struct Cyc_PP_Doc*),2))))));}{
struct Cyc_PP_Doc*_tmp6C2[2];return(_tmp6C2[1]=rest,((_tmp6C2[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C2,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 664
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp139=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp139;if(_tmp13A->tag != 20)goto _LLBF;}_LLBE: {
const char*_tmp6C3;return Cyc_PP_text(((_tmp6C3="`H",_tag_dyneither(_tmp6C3,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp139;if(_tmp13B->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp6C4;return Cyc_PP_text(((_tmp6C4="`U",_tag_dyneither(_tmp6C4,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp139;if(_tmp13C->tag != 22)goto _LLC3;}_LLC2: {
const char*_tmp6C5;return Cyc_PP_text(((_tmp6C5="`RC",_tag_dyneither(_tmp6C5,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp139;if(_tmp13D->tag != 2)goto _LLC5;}_LLC4:
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
const char*_tmp6CA;const char*_tmp6C9;struct Cyc_PP_Doc*_tmp6C8[3];return(_tmp6C8[2]=Cyc_PP_text(((_tmp6C9=")",_tag_dyneither(_tmp6C9,sizeof(char),2)))),((_tmp6C8[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp6C8[0]=Cyc_PP_text(((_tmp6CA="@region(",_tag_dyneither(_tmp6CA,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6C8,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 674
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 678
void*_tmp144=Cyc_Tcutil_compress(t);void*_tmp146;struct Cyc_List_List*_tmp148;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp144;if(_tmp145->tag != 23)goto _LLCA;else{_tmp146=(void*)_tmp145->f1;}}_LLC9:
{struct Cyc_List_List*_tmp6CB;*rgions=((_tmp6CB=_cycalloc(sizeof(*_tmp6CB)),((_tmp6CB->hd=Cyc_Absynpp_rgn2doc(_tmp146),((_tmp6CB->tl=*rgions,_tmp6CB))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp144;if(_tmp147->tag != 24)goto _LLCC;else{_tmp148=_tmp147->f1;}}_LLCB:
# 681
 for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp148->hd);}
# 684
goto _LLC7;_LLCC:;_LLCD:
{struct Cyc_List_List*_tmp6CC;*effects=((_tmp6CC=_cycalloc(sizeof(*_tmp6CC)),((_tmp6CC->hd=Cyc_Absynpp_typ2doc(t),((_tmp6CC->tl=*effects,_tmp6CC))))));}goto _LLC7;_LLC7:;}
# 689
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6CF;const char*_tmp6CE;const char*_tmp6CD;return Cyc_PP_group(((_tmp6CD="",_tag_dyneither(_tmp6CD,sizeof(char),1))),((_tmp6CE="",_tag_dyneither(_tmp6CE,sizeof(char),1))),((_tmp6CF="+",_tag_dyneither(_tmp6CF,sizeof(char),2))),effects);}else{
# 697
const char*_tmp6D0;struct Cyc_PP_Doc*_tmp14E=Cyc_Absynpp_group_braces(((_tmp6D0=",",_tag_dyneither(_tmp6D0,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6D4;const char*_tmp6D3;const char*_tmp6D2;const char*_tmp6D1;return Cyc_PP_group(((_tmp6D1="",_tag_dyneither(_tmp6D1,sizeof(char),1))),((_tmp6D2="",_tag_dyneither(_tmp6D2,sizeof(char),1))),((_tmp6D3="+",_tag_dyneither(_tmp6D3,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6D4=_cycalloc(sizeof(*_tmp6D4)),((_tmp6D4->hd=_tmp14E,((_tmp6D4->tl=0,_tmp6D4))))))));}}
# 702
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLCE: {
const char*_tmp6D5;return Cyc_PP_text(((_tmp6D5="struct ",_tag_dyneither(_tmp6D5,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
const char*_tmp6D6;return Cyc_PP_text(((_tmp6D6="union ",_tag_dyneither(_tmp6D6,sizeof(char),7))));}}}
# 710
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp156=t;struct Cyc_Core_Opt*_tmp15C;void*_tmp15D;int _tmp15E;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;union Cyc_Absyn_DatatypeInfoU _tmp163;struct Cyc_List_List*_tmp164;union Cyc_Absyn_DatatypeFieldInfoU _tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign _tmp169;enum Cyc_Absyn_Size_of _tmp16A;int _tmp16C;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*_tmp183;void*_tmp185;void*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp192;struct Cyc_Absyn_Datatypedecl*_tmp195;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 8)goto _LLD4;}_LLD3: {
# 714
const char*_tmp6D7;return Cyc_PP_text(((_tmp6D7="[[[array]]]",_tag_dyneither(_tmp6D7,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp156;if(_tmp158->tag != 9)goto _LLD6;}_LLD5:
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp156;if(_tmp159->tag != 5)goto _LLD8;}_LLD7:
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp156;if(_tmp15A->tag != 0)goto _LLDA;}_LLD9:
# 718
{const char*_tmp6D8;s=Cyc_PP_text(((_tmp6D8="void",_tag_dyneither(_tmp6D8,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp15B->tag != 1)goto _LLDC;else{_tmp15C=_tmp15B->f1;_tmp15D=(void*)_tmp15B->f2;_tmp15E=_tmp15B->f3;_tmp15F=_tmp15B->f4;}}_LLDB:
# 720
 if(_tmp15D != 0)
# 722
return Cyc_Absynpp_ntyp2doc((void*)_tmp15D);else{
# 730
const char*_tmp6E8;const char*_tmp6E7;const char*_tmp6E6;struct Cyc_Int_pa_PrintArg_struct _tmp6E5;void*_tmp6E4[1];const char*_tmp6E3;const char*_tmp6E2;struct Cyc_PP_Doc*_tmp6E1[6];s=((_tmp6E1[5]=
# 735
_tmp15C == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15C->v),((_tmp6E1[4]=Cyc_PP_text(((_tmp6E2=")::",_tag_dyneither(_tmp6E2,sizeof(char),4)))),((_tmp6E1[3]=
# 733
(!Cyc_Absynpp_print_full_evars  || _tmp15F == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15F->v),((_tmp6E1[2]=
# 732
Cyc_PP_text((struct _dyneither_ptr)((_tmp6E5.tag=1,((_tmp6E5.f1=(unsigned long)_tmp15E,((_tmp6E4[0]=& _tmp6E5,Cyc_aprintf(((_tmp6E3="%d",_tag_dyneither(_tmp6E3,sizeof(char),3))),_tag_dyneither(_tmp6E4,sizeof(void*),1))))))))),((_tmp6E1[1]=
# 731
Cyc_PP_text(((_tmp6E6="(",_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E1[0]=
# 730
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E7="\\%",_tag_dyneither(_tmp6E7,sizeof(char),3)))): Cyc_PP_text(((_tmp6E8="%",_tag_dyneither(_tmp6E8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E1,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 738
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp156;if(_tmp160->tag != 2)goto _LLDE;else{_tmp161=_tmp160->f1;}}_LLDD:
# 740
 s=Cyc_PP_textptr(_tmp161->name);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6EB;struct Cyc_PP_Doc*_tmp6EA[3];s=((_tmp6EA[2]=Cyc_Absynpp_kindbound2doc(_tmp161->kind),((_tmp6EA[1]=Cyc_PP_text(((_tmp6EB="::",_tag_dyneither(_tmp6EB,sizeof(char),3)))),((_tmp6EA[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6EA,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 746
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp161)){
# 749
const char*_tmp6F0;const char*_tmp6EF;struct Cyc_PP_Doc*_tmp6EE[3];s=((_tmp6EE[2]=Cyc_PP_text(((_tmp6EF=" */",_tag_dyneither(_tmp6EF,sizeof(char),4)))),((_tmp6EE[1]=s,((_tmp6EE[0]=Cyc_PP_text(((_tmp6F0="_ /* ",_tag_dyneither(_tmp6F0,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6EE,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 752
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp156;if(_tmp162->tag != 3)goto _LLE0;else{_tmp163=(_tmp162->f1).datatype_info;_tmp164=(_tmp162->f1).targs;}}_LLDF:
# 754
{union Cyc_Absyn_DatatypeInfoU _tmp1A5=_tmp163;struct _tuple0*_tmp1A6;int _tmp1A7;struct _tuple0*_tmp1A8;int _tmp1A9;_LL10D: if((_tmp1A5.UnknownDatatype).tag != 1)goto _LL10F;_tmp1A6=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).name;_tmp1A7=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A5.UnknownDatatype).val).is_extensible;_LL10E:
# 756
 _tmp1A8=_tmp1A6;_tmp1A9=_tmp1A7;goto _LL110;_LL10F: if((_tmp1A5.KnownDatatype).tag != 2)goto _LL10C;_tmp1A8=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))).name;_tmp1A9=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A5.KnownDatatype).val))).is_extensible;_LL110: {
# 758
const char*_tmp6F1;struct Cyc_PP_Doc*_tmp1AA=Cyc_PP_text(((_tmp6F1="datatype ",_tag_dyneither(_tmp6F1,sizeof(char),10))));
const char*_tmp6F2;struct Cyc_PP_Doc*_tmp1AB=_tmp1A9?Cyc_PP_text(((_tmp6F2="@extensible ",_tag_dyneither(_tmp6F2,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6F3[4];s=((_tmp6F3[3]=Cyc_Absynpp_tps2doc(_tmp164),((_tmp6F3[2]=Cyc_Absynpp_qvar2doc(_tmp1A8),((_tmp6F3[1]=_tmp1AA,((_tmp6F3[0]=_tmp1AB,Cyc_PP_cat(_tag_dyneither(_tmp6F3,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL10C;}_LL10C:;}
# 763
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp156;if(_tmp165->tag != 4)goto _LLE2;else{_tmp166=(_tmp165->f1).field_info;_tmp167=(_tmp165->f1).targs;}}_LLE1:
# 765
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AF=_tmp166;struct _tuple0*_tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;struct _tuple0*_tmp1B3;int _tmp1B4;struct _tuple0*_tmp1B5;_LL112: if((_tmp1AF.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).datatype_name;_tmp1B1=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).field_name;_tmp1B2=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AF.UnknownDatatypefield).val).is_extensible;_LL113:
# 767
 _tmp1B3=_tmp1B0;_tmp1B4=_tmp1B2;_tmp1B5=_tmp1B1;goto _LL115;_LL114: if((_tmp1AF.KnownDatatypefield).tag != 2)goto _LL111;_tmp1B3=(*((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1).name;_tmp1B4=(*((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f1).is_extensible;_tmp1B5=(*((struct _tuple1)(_tmp1AF.KnownDatatypefield).val).f2).name;_LL115: {
# 770
const char*_tmp6F5;const char*_tmp6F4;struct Cyc_PP_Doc*_tmp1B6=Cyc_PP_text(_tmp1B4?(_tmp6F5="@extensible datatype ",_tag_dyneither(_tmp6F5,sizeof(char),22)):((_tmp6F4="datatype ",_tag_dyneither(_tmp6F4,sizeof(char),10))));
{const char*_tmp6F8;struct Cyc_PP_Doc*_tmp6F7[4];s=((_tmp6F7[3]=Cyc_Absynpp_qvar2doc(_tmp1B5),((_tmp6F7[2]=Cyc_PP_text(((_tmp6F8=".",_tag_dyneither(_tmp6F8,sizeof(char),2)))),((_tmp6F7[1]=Cyc_Absynpp_qvar2doc(_tmp1B3),((_tmp6F7[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp6F7,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 774
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp156;if(_tmp168->tag != 6)goto _LLE4;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE3: {
# 776
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp169){case Cyc_Absyn_None: _LL116:
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
{const char*_tmp6F9;sns=((_tmp6F9="",_tag_dyneither(_tmp6F9,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
{const char*_tmp6FA;sns=((_tmp6FA="unsigned ",_tag_dyneither(_tmp6FA,sizeof(char),10)));}break;}
# 783
switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL11A:
# 785
 switch(_tmp169){case Cyc_Absyn_None: _LL11C:
{const char*_tmp6FB;sns=((_tmp6FB="",_tag_dyneither(_tmp6FB,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
{const char*_tmp6FC;sns=((_tmp6FC="signed ",_tag_dyneither(_tmp6FC,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
{const char*_tmp6FD;sns=((_tmp6FD="unsigned ",_tag_dyneither(_tmp6FD,sizeof(char),10)));}break;}
# 790
{const char*_tmp6FE;ts=((_tmp6FE="char",_tag_dyneither(_tmp6FE,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL11B:
{const char*_tmp6FF;ts=((_tmp6FF="short",_tag_dyneither(_tmp6FF,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
{const char*_tmp700;ts=((_tmp700="int",_tag_dyneither(_tmp700,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
{const char*_tmp701;ts=((_tmp701="long",_tag_dyneither(_tmp701,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 796
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
{const char*_tmp702;ts=((_tmp702="long long",_tag_dyneither(_tmp702,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
{const char*_tmp703;ts=((_tmp703="__int64",_tag_dyneither(_tmp703,sizeof(char),8)));}break;}
# 800
break;}
# 802
{const char*_tmp708;void*_tmp707[2];struct Cyc_String_pa_PrintArg_struct _tmp706;struct Cyc_String_pa_PrintArg_struct _tmp705;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp705.tag=0,((_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp706.tag=0,((_tmp706.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp707[0]=& _tmp706,((_tmp707[1]=& _tmp705,Cyc_aprintf(((_tmp708="%s%s",_tag_dyneither(_tmp708,sizeof(char),5))),_tag_dyneither(_tmp707,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp156;if(_tmp16B->tag != 7)goto _LLE6;else{_tmp16C=_tmp16B->f1;}}_LLE5:
# 805
 if(_tmp16C == 0){
const char*_tmp709;s=Cyc_PP_text(((_tmp709="float",_tag_dyneither(_tmp709,sizeof(char),6))));}else{
if(_tmp16C == 1){
const char*_tmp70A;s=Cyc_PP_text(((_tmp70A="double",_tag_dyneither(_tmp70A,sizeof(char),7))));}else{
# 810
const char*_tmp70B;s=Cyc_PP_text(((_tmp70B="long double",_tag_dyneither(_tmp70B,sizeof(char),12))));}}
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp156;if(_tmp16D->tag != 10)goto _LLE8;else{_tmp16E=_tmp16D->f1;}}_LLE7:
# 813
{struct Cyc_PP_Doc*_tmp70C[2];s=((_tmp70C[1]=Cyc_Absynpp_args2doc(_tmp16E),((_tmp70C[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp156;if(_tmp16F->tag != 11)goto _LLEA;else{_tmp170=(_tmp16F->f1).aggr_info;_tmp171=(_tmp16F->f1).targs;}}_LLE9: {
# 816
enum Cyc_Absyn_AggrKind _tmp1CF;struct _tuple0*_tmp1D0;struct _tuple10 _tmp1CE=Cyc_Absyn_aggr_kinded_name(_tmp170);_tmp1CF=_tmp1CE.f1;_tmp1D0=_tmp1CE.f2;
{struct Cyc_PP_Doc*_tmp70D[3];s=((_tmp70D[2]=Cyc_Absynpp_tps2doc(_tmp171),((_tmp70D[1]=Cyc_Absynpp_qvar2doc(_tmp1D0),((_tmp70D[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CF),Cyc_PP_cat(_tag_dyneither(_tmp70D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp156;if(_tmp172->tag != 12)goto _LLEC;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LLEB:
# 820
{struct Cyc_PP_Doc*_tmp70E[4];s=((_tmp70E[3]=
# 822
Cyc_Absynpp_rb(),((_tmp70E[2]=
# 821
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp70E[1]=
# 820
Cyc_Absynpp_lb(),((_tmp70E[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 823
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp156;if(_tmp175->tag != 14)goto _LLEE;else{_tmp176=_tmp175->f1;}}_LLED:
# 825
{const char*_tmp711;struct Cyc_PP_Doc*_tmp710[4];s=((_tmp710[3]=Cyc_Absynpp_rb(),((_tmp710[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp176)),((_tmp710[1]=Cyc_Absynpp_lb(),((_tmp710[0]=Cyc_PP_text(((_tmp711="enum ",_tag_dyneither(_tmp711,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp710,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp156;if(_tmp177->tag != 13)goto _LLF0;else{_tmp178=_tmp177->f1;}}_LLEF:
# 827
{const char*_tmp714;struct Cyc_PP_Doc*_tmp713[2];s=((_tmp713[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp713[0]=Cyc_PP_text(((_tmp714="enum ",_tag_dyneither(_tmp714,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp713,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp156;if(_tmp179->tag != 18)goto _LLF2;else{_tmp17A=_tmp179->f1;}}_LLF1:
# 829
{const char*_tmp719;const char*_tmp718;struct Cyc_PP_Doc*_tmp717[3];s=((_tmp717[2]=Cyc_PP_text(((_tmp718=")",_tag_dyneither(_tmp718,sizeof(char),2)))),((_tmp717[1]=Cyc_Absynpp_exp2doc(_tmp17A),((_tmp717[0]=Cyc_PP_text(((_tmp719="valueof_t(",_tag_dyneither(_tmp719,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp717,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp156;if(_tmp17B->tag != 17)goto _LLF4;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF3:
# 836
{struct Cyc_PP_Doc*_tmp71A[2];s=((_tmp71A[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp71A[0]=Cyc_Absynpp_qvar2doc(_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp71A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp156;if(_tmp17F->tag != 15)goto _LLF6;else{_tmp180=(void*)_tmp17F->f1;}}_LLF5:
# 839
{const char*_tmp71F;const char*_tmp71E;struct Cyc_PP_Doc*_tmp71D[3];s=((_tmp71D[2]=Cyc_PP_text(((_tmp71E=">",_tag_dyneither(_tmp71E,sizeof(char),2)))),((_tmp71D[1]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp71D[0]=Cyc_PP_text(((_tmp71F="region_t<",_tag_dyneither(_tmp71F,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp156;if(_tmp181->tag != 16)goto _LLF8;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF7:
# 841
{const char*_tmp726;const char*_tmp725;const char*_tmp724;struct Cyc_PP_Doc*_tmp723[5];s=((_tmp723[4]=
Cyc_PP_text(((_tmp724=">",_tag_dyneither(_tmp724,sizeof(char),2)))),((_tmp723[3]=
# 841
Cyc_Absynpp_rgn2doc(_tmp183),((_tmp723[2]=Cyc_PP_text(((_tmp725=",",_tag_dyneither(_tmp725,sizeof(char),2)))),((_tmp723[1]=Cyc_Absynpp_rgn2doc(_tmp182),((_tmp723[0]=Cyc_PP_text(((_tmp726="dynregion_t<",_tag_dyneither(_tmp726,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp723,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp156;if(_tmp184->tag != 19)goto _LLFA;else{_tmp185=(void*)_tmp184->f1;}}_LLF9:
# 844
{const char*_tmp72B;const char*_tmp72A;struct Cyc_PP_Doc*_tmp729[3];s=((_tmp729[2]=Cyc_PP_text(((_tmp72A=">",_tag_dyneither(_tmp72A,sizeof(char),2)))),((_tmp729[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp729[0]=Cyc_PP_text(((_tmp72B="tag_t<",_tag_dyneither(_tmp72B,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp156;if(_tmp186->tag != 21)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp156;if(_tmp187->tag != 20)goto _LLFE;}_LLFD:
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp156;if(_tmp188->tag != 22)goto _LL100;}_LLFF:
# 848
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp156;if(_tmp189->tag != 25)goto _LL102;else{_tmp18A=(void*)_tmp189->f1;}}_LL101:
# 850
{const char*_tmp730;const char*_tmp72F;struct Cyc_PP_Doc*_tmp72E[3];s=((_tmp72E[2]=Cyc_PP_text(((_tmp72F=")",_tag_dyneither(_tmp72F,sizeof(char),2)))),((_tmp72E[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp72E[0]=Cyc_PP_text(((_tmp730="regions(",_tag_dyneither(_tmp730,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp72E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp156;if(_tmp18B->tag != 23)goto _LL104;}_LL103:
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp156;if(_tmp18C->tag != 24)goto _LL106;}_LL105:
# 853
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp18D->tag != 26)goto _LL108;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp18E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(*_tmp18D->f1).r;if(_tmp18E->tag != 0)goto _LL108;else{_tmp18F=_tmp18E->f1;}}}_LL107:
# 855
 s=Cyc_Absynpp_aggrdecl2doc(_tmp18F);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp190->tag != 26)goto _LL10A;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp191=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(*_tmp190->f1).r;if(_tmp191->tag != 1)goto _LL10A;else{_tmp192=_tmp191->f1;}}}_LL109:
# 857
 s=Cyc_Absynpp_enumdecl2doc(_tmp192);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp193->tag != 26)goto _LLD1;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp194=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(*_tmp193->f1).r;if(_tmp194->tag != 2)goto _LLD1;else{_tmp195=_tmp194->f1;}}}_LL10B:
# 859
 s=Cyc_Absynpp_datatypedecl2doc(_tmp195);goto _LLD1;_LLD1:;}
# 861
return s;}
# 864
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo));}struct _tuple16{void*f1;void*f2;};
# 868
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1E9;void*_tmp1EA;struct _tuple16*_tmp1E8=cmp;_tmp1E9=(*_tmp1E8).f1;_tmp1EA=(*_tmp1E8).f2;{
const char*_tmp733;struct Cyc_PP_Doc*_tmp732[3];return(_tmp732[2]=Cyc_Absynpp_rgn2doc(_tmp1EA),((_tmp732[1]=Cyc_PP_text(((_tmp733=" > ",_tag_dyneither(_tmp733,sizeof(char),4)))),((_tmp732[0]=Cyc_Absynpp_rgn2doc(_tmp1E9),Cyc_PP_cat(_tag_dyneither(_tmp732,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp736;const char*_tmp735;const char*_tmp734;return Cyc_PP_group(((_tmp734="",_tag_dyneither(_tmp734,sizeof(char),1))),((_tmp735="",_tag_dyneither(_tmp735,sizeof(char),1))),((_tmp736=",",_tag_dyneither(_tmp736,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 877
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1F0=(*t).f1;
struct Cyc_Core_Opt*_tmp737;struct Cyc_Core_Opt*dopt=_tmp1F0 == 0?0:((_tmp737=_cycalloc(sizeof(*_tmp737)),((_tmp737->v=Cyc_PP_text(*_tmp1F0),_tmp737))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 886
struct Cyc_List_List*_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp73A;struct Cyc_List_List*_tmp739;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp739=_cycalloc(sizeof(*_tmp739)),((_tmp739->hd=Cyc_PP_text(((_tmp73A="...",_tag_dyneither(_tmp73A,sizeof(char),4)))),((_tmp739->tl=0,_tmp739)))))));}else{
if(cyc_varargs != 0){
const char*_tmp743;const char*_tmp742;const char*_tmp741;struct _tuple8*_tmp740;struct Cyc_PP_Doc*_tmp73F[3];struct Cyc_PP_Doc*_tmp1F5=(_tmp73F[2]=
# 893
Cyc_Absynpp_funarg2doc(((_tmp740=_cycalloc(sizeof(*_tmp740)),((_tmp740->f1=cyc_varargs->name,((_tmp740->f2=cyc_varargs->tq,((_tmp740->f3=cyc_varargs->type,_tmp740))))))))),((_tmp73F[1]=
# 892
cyc_varargs->inject?Cyc_PP_text(((_tmp741=" inject ",_tag_dyneither(_tmp741,sizeof(char),9)))): Cyc_PP_text(((_tmp742=" ",_tag_dyneither(_tmp742,sizeof(char),2)))),((_tmp73F[0]=
# 891
Cyc_PP_text(((_tmp743="...",_tag_dyneither(_tmp743,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp73F,sizeof(struct Cyc_PP_Doc*),3)))))));
# 895
struct Cyc_List_List*_tmp744;_tmp1F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,((_tmp744=_cycalloc(sizeof(*_tmp744)),((_tmp744->hd=_tmp1F5,((_tmp744->tl=0,_tmp744)))))));}}{
# 897
const char*_tmp747;const char*_tmp746;const char*_tmp745;struct Cyc_PP_Doc*_tmp1FC=Cyc_PP_group(((_tmp745="",_tag_dyneither(_tmp745,sizeof(char),1))),((_tmp746="",_tag_dyneither(_tmp746,sizeof(char),1))),((_tmp747=",",_tag_dyneither(_tmp747,sizeof(char),2))),_tmp1F2);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp74A;struct Cyc_PP_Doc*_tmp749[3];_tmp1FC=((_tmp749[2]=Cyc_Absynpp_eff2doc((void*)effopt),((_tmp749[1]=Cyc_PP_text(((_tmp74A=";",_tag_dyneither(_tmp74A,sizeof(char),2)))),((_tmp749[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp749,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp74D;struct Cyc_PP_Doc*_tmp74C[3];_tmp1FC=((_tmp74C[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74C[1]=Cyc_PP_text(((_tmp74D=":",_tag_dyneither(_tmp74D,sizeof(char),2)))),((_tmp74C[0]=_tmp1FC,Cyc_PP_cat(_tag_dyneither(_tmp74C,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp752;const char*_tmp751;struct Cyc_PP_Doc*_tmp750[3];return(_tmp750[2]=Cyc_PP_text(((_tmp751=")",_tag_dyneither(_tmp751,sizeof(char),2)))),((_tmp750[1]=_tmp1FC,((_tmp750[0]=Cyc_PP_text(((_tmp752="(",_tag_dyneither(_tmp752,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp750,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 905
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp208;struct Cyc_Absyn_Tqual _tmp209;void*_tmp20A;struct _tuple8*_tmp207=arg;_tmp208=(*_tmp207).f1;_tmp209=(*_tmp207).f2;_tmp20A=(*_tmp207).f3;{
struct _tuple8*_tmp753;return(_tmp753=_cycalloc(sizeof(*_tmp753)),((_tmp753->f1=(struct _dyneither_ptr*)_tmp208,((_tmp753->f2=_tmp209,((_tmp753->f3=_tmp20A,_tmp753)))))));};}
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
{struct Cyc_List_List*_tmp754;_tmp20C=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp754=_cycalloc(sizeof(*_tmp754)),((_tmp754->hd=Cyc_Absynpp_cyc_stringptr,((_tmp754->tl=_tmp210,_tmp754))))): _tmp210;}
goto _LL127;_LL127:;}
# 931
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp756;struct Cyc_List_List*_tmp755;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755->hd=(*q).f2,((_tmp755->tl=0,_tmp755))))))),((_tmp756="_",_tag_dyneither(_tmp756,sizeof(char),2))));}else{
# 936
if(match)
return*(*q).f2;else{
# 939
const char*_tmp758;struct Cyc_List_List*_tmp757;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20C,(
(_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->hd=(*q).f2,((_tmp757->tl=0,_tmp757))))))),((_tmp758="::",_tag_dyneither(_tmp758,sizeof(char),3))));}}}
# 944
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 948
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp216=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 952
const char*_tmp75A;const char*_tmp759;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp759="\\textbf{",_tag_dyneither(_tmp759,sizeof(char),9))),(struct _dyneither_ptr)_tmp216),((_tmp75A="}",_tag_dyneither(_tmp75A,sizeof(char),2)))),(int)
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
const char*_tmp75B;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp75B="/* bad namespace : */ ",_tag_dyneither(_tmp75B,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
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
const char*_tmp75D;const char*_tmp75C;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75C="\\textbf{",_tag_dyneither(_tmp75C,sizeof(char),9))),(struct _dyneither_ptr)_tmp21D),((_tmp75D="}",_tag_dyneither(_tmp75D,sizeof(char),2)))),(int)
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
{const char*_tmp762;const char*_tmp761;struct Cyc_PP_Doc*_tmp760[5];s=((_tmp760[4]=
# 1084
Cyc_Absynpp_exp2doc_prec(myprec,_tmp256),((_tmp760[3]=
# 1083
Cyc_PP_text(((_tmp761="= ",_tag_dyneither(_tmp761,sizeof(char),3)))),((_tmp760[2]=
# 1082
_tmp255 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp255->v),((_tmp760[1]=
# 1081
Cyc_PP_text(((_tmp762=" ",_tag_dyneither(_tmp762,sizeof(char),2)))),((_tmp760[0]=
# 1080
Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),Cyc_PP_cat(_tag_dyneither(_tmp760,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1085
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp257->tag != 4)goto _LL1AA;else{_tmp258=_tmp257->f1;_tmp259=_tmp257->f2;}}_LL1A9: {
# 1087
struct Cyc_PP_Doc*_tmp2BF=Cyc_Absynpp_exp2doc_prec(myprec,_tmp258);
switch(_tmp259){case Cyc_Absyn_PreInc: _LL1F0:
{const char*_tmp765;struct Cyc_PP_Doc*_tmp764[2];s=((_tmp764[1]=_tmp2BF,((_tmp764[0]=Cyc_PP_text(((_tmp765="++",_tag_dyneither(_tmp765,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp764,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F1:
{const char*_tmp768;struct Cyc_PP_Doc*_tmp767[2];s=((_tmp767[1]=_tmp2BF,((_tmp767[0]=Cyc_PP_text(((_tmp768="--",_tag_dyneither(_tmp768,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp767,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F2:
{const char*_tmp76B;struct Cyc_PP_Doc*_tmp76A[2];s=((_tmp76A[1]=Cyc_PP_text(((_tmp76B="++",_tag_dyneither(_tmp76B,sizeof(char),3)))),((_tmp76A[0]=_tmp2BF,Cyc_PP_cat(_tag_dyneither(_tmp76A,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F3:
{const char*_tmp76E;struct Cyc_PP_Doc*_tmp76D[2];s=((_tmp76D[1]=Cyc_PP_text(((_tmp76E="--",_tag_dyneither(_tmp76E,sizeof(char),3)))),((_tmp76D[0]=_tmp2BF,Cyc_PP_cat(_tag_dyneither(_tmp76D,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1094
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25A->tag != 5)goto _LL1AC;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;_tmp25D=_tmp25A->f3;}}_LL1AB:
# 1096
{const char*_tmp773;const char*_tmp772;struct Cyc_PP_Doc*_tmp771[5];s=((_tmp771[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),((_tmp771[3]=Cyc_PP_text(((_tmp772=" : ",_tag_dyneither(_tmp772,sizeof(char),4)))),((_tmp771[2]=
# 1096
Cyc_Absynpp_exp2doc_prec(0,_tmp25C),((_tmp771[1]=Cyc_PP_text(((_tmp773=" ? ",_tag_dyneither(_tmp773,sizeof(char),4)))),((_tmp771[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),Cyc_PP_cat(_tag_dyneither(_tmp771,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1098
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25E=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp25E->tag != 6)goto _LL1AE;else{_tmp25F=_tmp25E->f1;_tmp260=_tmp25E->f2;}}_LL1AD:
# 1100
{const char*_tmp776;struct Cyc_PP_Doc*_tmp775[3];s=((_tmp775[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),((_tmp775[1]=Cyc_PP_text(((_tmp776=" && ",_tag_dyneither(_tmp776,sizeof(char),5)))),((_tmp775[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25F),Cyc_PP_cat(_tag_dyneither(_tmp775,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp261->tag != 7)goto _LL1B0;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;}}_LL1AF:
# 1103
{const char*_tmp779;struct Cyc_PP_Doc*_tmp778[3];s=((_tmp778[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp263),((_tmp778[1]=Cyc_PP_text(((_tmp779=" || ",_tag_dyneither(_tmp779,sizeof(char),5)))),((_tmp778[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp262),Cyc_PP_cat(_tag_dyneither(_tmp778,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp264->tag != 8)goto _LL1B2;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;}}_LL1B1:
# 1108
{const char*_tmp780;const char*_tmp77F;const char*_tmp77E;struct Cyc_PP_Doc*_tmp77D[5];s=((_tmp77D[4]=Cyc_PP_text(((_tmp77E=")",_tag_dyneither(_tmp77E,sizeof(char),2)))),((_tmp77D[3]=Cyc_Absynpp_exp2doc(_tmp266),((_tmp77D[2]=Cyc_PP_text(((_tmp77F=", ",_tag_dyneither(_tmp77F,sizeof(char),3)))),((_tmp77D[1]=Cyc_Absynpp_exp2doc(_tmp265),((_tmp77D[0]=Cyc_PP_text(((_tmp780="(",_tag_dyneither(_tmp780,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77D,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp267=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp267->tag != 9)goto _LL1B4;else{_tmp268=_tmp267->f1;_tmp269=_tmp267->f2;}}_LL1B3:
# 1111
{const char*_tmp785;const char*_tmp784;struct Cyc_PP_Doc*_tmp783[4];s=((_tmp783[3]=
# 1114
Cyc_PP_text(((_tmp784=")",_tag_dyneither(_tmp784,sizeof(char),2)))),((_tmp783[2]=
# 1113
Cyc_Absynpp_exps2doc_prec(20,_tmp269),((_tmp783[1]=
# 1112
Cyc_PP_text(((_tmp785="(",_tag_dyneither(_tmp785,sizeof(char),2)))),((_tmp783[0]=
# 1111
Cyc_Absynpp_exp2doc_prec(myprec,_tmp268),Cyc_PP_cat(_tag_dyneither(_tmp783,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1115
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26A->tag != 10)goto _LL1B6;else{_tmp26B=_tmp26A->f1;}}_LL1B5:
# 1117
{const char*_tmp788;struct Cyc_PP_Doc*_tmp787[2];s=((_tmp787[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26B),((_tmp787[0]=Cyc_PP_text(((_tmp788="throw ",_tag_dyneither(_tmp788,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26C->tag != 11)goto _LL1B8;else{_tmp26D=_tmp26C->f1;}}_LL1B7:
# 1120
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26D);
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp26E->tag != 12)goto _LL1BA;else{_tmp26F=_tmp26E->f1;}}_LL1B9:
# 1123
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26F);
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp270=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp270->tag != 13)goto _LL1BC;else{_tmp271=(void*)_tmp270->f1;_tmp272=_tmp270->f2;}}_LL1BB:
# 1126
{const char*_tmp78D;const char*_tmp78C;struct Cyc_PP_Doc*_tmp78B[4];s=((_tmp78B[3]=
# 1129
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp78B[2]=
# 1128
Cyc_PP_text(((_tmp78C=")",_tag_dyneither(_tmp78C,sizeof(char),2)))),((_tmp78B[1]=
# 1127
Cyc_Absynpp_typ2doc(_tmp271),((_tmp78B[0]=
# 1126
Cyc_PP_text(((_tmp78D="(",_tag_dyneither(_tmp78D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78B,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1130
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp273->tag != 14)goto _LL1BE;else{_tmp274=_tmp273->f1;}}_LL1BD:
# 1132
{const char*_tmp790;struct Cyc_PP_Doc*_tmp78F[2];s=((_tmp78F[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp274),((_tmp78F[0]=
# 1132
Cyc_PP_text(((_tmp790="&",_tag_dyneither(_tmp790,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78F,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1134
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp275->tag != 15)goto _LL1C0;else{_tmp276=_tmp275->f1;_tmp277=_tmp275->f2;}}_LL1BF:
# 1136
 if(_tmp276 == 0){
const char*_tmp793;struct Cyc_PP_Doc*_tmp792[2];s=((_tmp792[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp792[0]=Cyc_PP_text(((_tmp793="new ",_tag_dyneither(_tmp793,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp792,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1139
const char*_tmp798;const char*_tmp797;struct Cyc_PP_Doc*_tmp796[4];s=((_tmp796[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp277),((_tmp796[2]=
# 1139
Cyc_PP_text(((_tmp797=") ",_tag_dyneither(_tmp797,sizeof(char),3)))),((_tmp796[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp276),((_tmp796[0]=Cyc_PP_text(((_tmp798="rnew(",_tag_dyneither(_tmp798,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp796,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1141
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp278->tag != 16)goto _LL1C2;else{_tmp279=(void*)_tmp278->f1;}}_LL1C1:
{const char*_tmp79D;const char*_tmp79C;struct Cyc_PP_Doc*_tmp79B[3];s=((_tmp79B[2]=Cyc_PP_text(((_tmp79C=")",_tag_dyneither(_tmp79C,sizeof(char),2)))),((_tmp79B[1]=Cyc_Absynpp_typ2doc(_tmp279),((_tmp79B[0]=Cyc_PP_text(((_tmp79D="sizeof(",_tag_dyneither(_tmp79D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27A->tag != 17)goto _LL1C4;else{_tmp27B=_tmp27A->f1;}}_LL1C3:
{const char*_tmp7A2;const char*_tmp7A1;struct Cyc_PP_Doc*_tmp7A0[3];s=((_tmp7A0[2]=Cyc_PP_text(((_tmp7A1=")",_tag_dyneither(_tmp7A1,sizeof(char),2)))),((_tmp7A0[1]=Cyc_Absynpp_exp2doc(_tmp27B),((_tmp7A0[0]=Cyc_PP_text(((_tmp7A2="sizeof(",_tag_dyneither(_tmp7A2,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27C->tag != 37)goto _LL1C6;else{_tmp27D=(void*)_tmp27C->f1;}}_LL1C5:
{const char*_tmp7A7;const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[3];s=((_tmp7A5[2]=Cyc_PP_text(((_tmp7A6=")",_tag_dyneither(_tmp7A6,sizeof(char),2)))),((_tmp7A5[1]=Cyc_Absynpp_typ2doc(_tmp27D),((_tmp7A5[0]=Cyc_PP_text(((_tmp7A7="valueof(",_tag_dyneither(_tmp7A7,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27E=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp27E->tag != 38)goto _LL1C8;else{_tmp27F=_tmp27E->f1;_tmp280=_tmp27E->f2;}}_LL1C7:
# 1146
 if(_tmp27F){
const char*_tmp7AE;const char*_tmp7AD;const char*_tmp7AC;struct Cyc_PP_Doc*_tmp7AB[4];s=((_tmp7AB[3]=Cyc_PP_text(((_tmp7AC=")",_tag_dyneither(_tmp7AC,sizeof(char),2)))),((_tmp7AB[2]=Cyc_PP_text(_tmp280),((_tmp7AB[1]=Cyc_PP_text(((_tmp7AD=" volatile (",_tag_dyneither(_tmp7AD,sizeof(char),12)))),((_tmp7AB[0]=Cyc_PP_text(((_tmp7AE="__asm__",_tag_dyneither(_tmp7AE,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7AB,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1149
const char*_tmp7B3;const char*_tmp7B2;struct Cyc_PP_Doc*_tmp7B1[3];s=((_tmp7B1[2]=Cyc_PP_text(((_tmp7B2=")",_tag_dyneither(_tmp7B2,sizeof(char),2)))),((_tmp7B1[1]=Cyc_PP_text(_tmp280),((_tmp7B1[0]=Cyc_PP_text(((_tmp7B3="__asm__(",_tag_dyneither(_tmp7B3,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7B1,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp281->tag != 36)goto _LL1CA;else{_tmp282=_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1C9:
# 1152
{const char*_tmp7BA;const char*_tmp7B9;const char*_tmp7B8;struct Cyc_PP_Doc*_tmp7B7[5];s=((_tmp7B7[4]=Cyc_PP_text(((_tmp7B8=")",_tag_dyneither(_tmp7B8,sizeof(char),2)))),((_tmp7B7[3]=Cyc_PP_textptr(_tmp283),((_tmp7B7[2]=Cyc_PP_text(((_tmp7B9=".",_tag_dyneither(_tmp7B9,sizeof(char),2)))),((_tmp7B7[1]=Cyc_Absynpp_exp2doc(_tmp282),((_tmp7B7[0]=Cyc_PP_text(((_tmp7BA="tagcheck(",_tag_dyneither(_tmp7BA,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp284->tag != 18)goto _LL1CC;else{_tmp285=(void*)_tmp284->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp286=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp284->f2);if(_tmp286->tag != 0)goto _LL1CC;else{_tmp287=_tmp286->f1;}};}}_LL1CB:
# 1155
{const char*_tmp7C1;const char*_tmp7C0;const char*_tmp7BF;struct Cyc_PP_Doc*_tmp7BE[5];s=((_tmp7BE[4]=Cyc_PP_text(((_tmp7BF=")",_tag_dyneither(_tmp7BF,sizeof(char),2)))),((_tmp7BE[3]=Cyc_PP_textptr(_tmp287),((_tmp7BE[2]=Cyc_PP_text(((_tmp7C0=",",_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BE[1]=Cyc_Absynpp_typ2doc(_tmp285),((_tmp7BE[0]=Cyc_PP_text(((_tmp7C1="offsetof(",_tag_dyneither(_tmp7C1,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BE,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp288->tag != 18)goto _LL1CE;else{_tmp289=(void*)_tmp288->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp28A=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp288->f2);if(_tmp28A->tag != 1)goto _LL1CE;else{_tmp28B=_tmp28A->f1;}};}}_LL1CD:
# 1158
{const char*_tmp7CF;const char*_tmp7CE;struct Cyc_Int_pa_PrintArg_struct _tmp7CD;void*_tmp7CC[1];const char*_tmp7CB;const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[5];s=((_tmp7C9[4]=Cyc_PP_text(((_tmp7CA=")",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((_tmp7C9[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp7CD.tag=1,((_tmp7CD.f1=(unsigned long)((int)_tmp28B),((_tmp7CC[0]=& _tmp7CD,Cyc_aprintf(((_tmp7CB="%d",_tag_dyneither(_tmp7CB,sizeof(char),3))),_tag_dyneither(_tmp7CC,sizeof(void*),1))))))))),((_tmp7C9[2]=Cyc_PP_text(((_tmp7CE=",",_tag_dyneither(_tmp7CE,sizeof(char),2)))),((_tmp7C9[1]=Cyc_Absynpp_typ2doc(_tmp289),((_tmp7C9[0]=Cyc_PP_text(((_tmp7CF="offsetof(",_tag_dyneither(_tmp7CF,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp28C=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28C->tag != 19)goto _LL1D0;else{_tmp28D=_tmp28C->f1;}}_LL1CF:
# 1161
{const char*_tmp7D2;struct Cyc_PP_Doc*_tmp7D1[2];s=((_tmp7D1[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),((_tmp7D1[0]=Cyc_PP_text(((_tmp7D2="*",_tag_dyneither(_tmp7D2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D1,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp28E=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp28E->tag != 20)goto _LL1D2;else{_tmp28F=_tmp28E->f1;_tmp290=_tmp28E->f2;}}_LL1D1:
# 1164
{const char*_tmp7D5;struct Cyc_PP_Doc*_tmp7D4[3];s=((_tmp7D4[2]=Cyc_PP_textptr(_tmp290),((_tmp7D4[1]=Cyc_PP_text(((_tmp7D5=".",_tag_dyneither(_tmp7D5,sizeof(char),2)))),((_tmp7D4[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28F),Cyc_PP_cat(_tag_dyneither(_tmp7D4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp291=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp291->tag != 21)goto _LL1D4;else{_tmp292=_tmp291->f1;_tmp293=_tmp291->f2;}}_LL1D3:
# 1167
{const char*_tmp7D8;struct Cyc_PP_Doc*_tmp7D7[3];s=((_tmp7D7[2]=Cyc_PP_textptr(_tmp293),((_tmp7D7[1]=Cyc_PP_text(((_tmp7D8="->",_tag_dyneither(_tmp7D8,sizeof(char),3)))),((_tmp7D7[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp292),Cyc_PP_cat(_tag_dyneither(_tmp7D7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp294=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp294->tag != 22)goto _LL1D6;else{_tmp295=_tmp294->f1;_tmp296=_tmp294->f2;}}_LL1D5:
# 1170
{const char*_tmp7DD;const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[4];s=((_tmp7DB[3]=
# 1173
Cyc_PP_text(((_tmp7DC="]",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DB[2]=
# 1172
Cyc_Absynpp_exp2doc(_tmp296),((_tmp7DB[1]=
# 1171
Cyc_PP_text(((_tmp7DD="[",_tag_dyneither(_tmp7DD,sizeof(char),2)))),((_tmp7DB[0]=
# 1170
Cyc_Absynpp_exp2doc_prec(myprec,_tmp295),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1174
goto _LL19F;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp297->tag != 23)goto _LL1D8;else{_tmp298=_tmp297->f1;}}_LL1D7:
# 1176
{const char*_tmp7E2;const char*_tmp7E1;struct Cyc_PP_Doc*_tmp7E0[4];s=((_tmp7E0[3]=
# 1179
Cyc_PP_text(((_tmp7E1=")",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7E0[2]=
# 1178
Cyc_Absynpp_exps2doc_prec(20,_tmp298),((_tmp7E0[1]=
# 1177
Cyc_PP_text(((_tmp7E2="(",_tag_dyneither(_tmp7E2,sizeof(char),2)))),((_tmp7E0[0]=
# 1176
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7E0,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1180
goto _LL19F;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp299->tag != 24)goto _LL1DA;else{_tmp29A=_tmp299->f1;_tmp29B=_tmp299->f2;}}_LL1D9:
# 1182
{const char*_tmp7E9;const char*_tmp7E8;const char*_tmp7E7;struct Cyc_PP_Doc*_tmp7E6[4];s=((_tmp7E6[3]=
# 1185
Cyc_Absynpp_group_braces(((_tmp7E7=",",_tag_dyneither(_tmp7E7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29B)),((_tmp7E6[2]=
# 1184
Cyc_PP_text(((_tmp7E8=")",_tag_dyneither(_tmp7E8,sizeof(char),2)))),((_tmp7E6[1]=
# 1183
Cyc_Absynpp_typ2doc((*_tmp29A).f3),((_tmp7E6[0]=
# 1182
Cyc_PP_text(((_tmp7E9="(",_tag_dyneither(_tmp7E9,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1186
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp29C=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp29C->tag != 25)goto _LL1DC;else{_tmp29D=_tmp29C->f1;}}_LL1DB:
# 1188
{const char*_tmp7EA;s=Cyc_Absynpp_group_braces(((_tmp7EA=",",_tag_dyneither(_tmp7EA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29D));}
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp29E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp29E->tag != 26)goto _LL1DE;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;_tmp2A1=_tmp29E->f3;}}_LL1DD:
# 1191
{const char*_tmp7F1;const char*_tmp7F0;const char*_tmp7EF;struct Cyc_PP_Doc*_tmp7EE[8];s=((_tmp7EE[7]=
# 1197
Cyc_Absynpp_rb(),((_tmp7EE[6]=
# 1196
Cyc_Absynpp_exp2doc(_tmp2A1),((_tmp7EE[5]=
# 1195
Cyc_PP_text(((_tmp7EF=" : ",_tag_dyneither(_tmp7EF,sizeof(char),4)))),((_tmp7EE[4]=
# 1194
Cyc_Absynpp_exp2doc(_tmp2A0),((_tmp7EE[3]=
# 1193
Cyc_PP_text(((_tmp7F0=" < ",_tag_dyneither(_tmp7F0,sizeof(char),4)))),((_tmp7EE[2]=
# 1192
Cyc_PP_text(*(*_tmp29F->name).f2),((_tmp7EE[1]=
# 1191
Cyc_PP_text(((_tmp7F1="for ",_tag_dyneither(_tmp7F1,sizeof(char),5)))),((_tmp7EE[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7EE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1198
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A2->tag != 27)goto _LL1E0;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f2;_tmp2A5=_tmp2A2->f3;}}_LL1DF: {
# 1200
struct Cyc_List_List*_tmp316=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A5);
{const char*_tmp7F6;struct Cyc_List_List*_tmp7F5;struct Cyc_PP_Doc*_tmp7F4[2];s=((_tmp7F4[1]=
Cyc_Absynpp_group_braces(((_tmp7F6=",",_tag_dyneither(_tmp7F6,sizeof(char),2))),
_tmp2A4 != 0?(_tmp7F5=_cycalloc(sizeof(*_tmp7F5)),((_tmp7F5->hd=Cyc_Absynpp_tps2doc(_tmp2A4),((_tmp7F5->tl=_tmp316,_tmp7F5))))): _tmp316),((_tmp7F4[0]=
# 1201
Cyc_Absynpp_qvar2doc(_tmp2A3),Cyc_PP_cat(_tag_dyneither(_tmp7F4,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1204
goto _LL19F;}_LL1E0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A6->tag != 28)goto _LL1E2;else{_tmp2A7=_tmp2A6->f2;}}_LL1E1:
# 1206
{const char*_tmp7F7;s=Cyc_Absynpp_group_braces(((_tmp7F7=",",_tag_dyneither(_tmp7F7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A7));}
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2A8->tag != 29)goto _LL1E4;else{_tmp2A9=_tmp2A8->f1;_tmp2AA=_tmp2A8->f3;}}_LL1E3:
# 1209
 if(_tmp2A9 == 0)
# 1211
s=Cyc_Absynpp_qvar2doc(_tmp2AA->name);else{
# 1213
const char*_tmp7FE;const char*_tmp7FD;const char*_tmp7FC;struct Cyc_PP_Doc*_tmp7FB[2];s=((_tmp7FB[1]=
Cyc_PP_egroup(((_tmp7FE="(",_tag_dyneither(_tmp7FE,sizeof(char),2))),((_tmp7FD=")",_tag_dyneither(_tmp7FD,sizeof(char),2))),((_tmp7FC=",",_tag_dyneither(_tmp7FC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A9)),((_tmp7FB[0]=
# 1213
Cyc_Absynpp_qvar2doc(_tmp2AA->name),Cyc_PP_cat(_tag_dyneither(_tmp7FB,sizeof(struct Cyc_PP_Doc*),2))))));}
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
const char*_tmp805;const char*_tmp804;const char*_tmp803;struct Cyc_PP_Doc*_tmp802[5];s=((_tmp802[4]=Cyc_PP_text(((_tmp803=")",_tag_dyneither(_tmp803,sizeof(char),2)))),((_tmp802[3]=Cyc_Absynpp_exp2doc(st),((_tmp802[2]=Cyc_PP_text(((_tmp804=",",_tag_dyneither(_tmp804,sizeof(char),2)))),((_tmp802[1]=Cyc_Absynpp_exp2doc(_tmp2B3),((_tmp802[0]=Cyc_PP_text(((_tmp805="calloc(",_tag_dyneither(_tmp805,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp802,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1231
const char*_tmp80E;const char*_tmp80D;const char*_tmp80C;const char*_tmp80B;struct Cyc_PP_Doc*_tmp80A[7];s=((_tmp80A[6]=
Cyc_PP_text(((_tmp80B=")",_tag_dyneither(_tmp80B,sizeof(char),2)))),((_tmp80A[5]=Cyc_Absynpp_exp2doc(st),((_tmp80A[4]=Cyc_PP_text(((_tmp80C=",",_tag_dyneither(_tmp80C,sizeof(char),2)))),((_tmp80A[3]=Cyc_Absynpp_exp2doc(_tmp2B3),((_tmp80A[2]=
# 1231
Cyc_PP_text(((_tmp80D=",",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2B1),((_tmp80A[0]=Cyc_PP_text(((_tmp80E="rcalloc(",_tag_dyneither(_tmp80E,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp80A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1234
struct Cyc_Absyn_Exp*new_e;
# 1236
if(_tmp2B2 == 0)
new_e=_tmp2B3;else{
# 1239
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2B2,0),_tmp2B3,0);}
# 1241
if(_tmp2B1 == 0){
const char*_tmp813;const char*_tmp812;struct Cyc_PP_Doc*_tmp811[3];s=((_tmp811[2]=Cyc_PP_text(((_tmp812=")",_tag_dyneither(_tmp812,sizeof(char),2)))),((_tmp811[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp811[0]=Cyc_PP_text(((_tmp813="malloc(",_tag_dyneither(_tmp813,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp811,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1244
const char*_tmp81A;const char*_tmp819;const char*_tmp818;struct Cyc_PP_Doc*_tmp817[5];s=((_tmp817[4]=
Cyc_PP_text(((_tmp818=")",_tag_dyneither(_tmp818,sizeof(char),2)))),((_tmp817[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp817[2]=
# 1244
Cyc_PP_text(((_tmp819=",",_tag_dyneither(_tmp819,sizeof(char),2)))),((_tmp817[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2B1),((_tmp817[0]=Cyc_PP_text(((_tmp81A="rmalloc(",_tag_dyneither(_tmp81A,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp817,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1247
goto _LL19F;_LL1EA: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B4->tag != 33)goto _LL1EC;else{_tmp2B5=_tmp2B4->f1;_tmp2B6=_tmp2B4->f2;}}_LL1EB:
# 1249
{const char*_tmp81D;struct Cyc_PP_Doc*_tmp81C[3];s=((_tmp81C[2]=
# 1251
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B6),((_tmp81C[1]=
# 1250
Cyc_PP_text(((_tmp81D=" :=: ",_tag_dyneither(_tmp81D,sizeof(char),6)))),((_tmp81C[0]=
# 1249
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B5),Cyc_PP_cat(_tag_dyneither(_tmp81C,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1252
goto _LL19F;_LL1EC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2B7->tag != 34)goto _LL1EE;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f2;}}_LL1ED:
# 1255
{const char*_tmp81E;s=Cyc_Absynpp_group_braces(((_tmp81E=",",_tag_dyneither(_tmp81E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B9));}
goto _LL19F;_LL1EE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp24B;if(_tmp2BA->tag != 35)goto _LL19F;else{_tmp2BB=_tmp2BA->f1;}}_LL1EF:
# 1258
{const char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*_tmp821[7];s=((_tmp821[6]=
# 1260
Cyc_PP_text(((_tmp822=")",_tag_dyneither(_tmp822,sizeof(char),2)))),((_tmp821[5]=Cyc_Absynpp_rb(),((_tmp821[4]=Cyc_PP_blank_doc(),((_tmp821[3]=
# 1259
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2BB,1)),((_tmp821[2]=
# 1258
Cyc_PP_blank_doc(),((_tmp821[1]=Cyc_Absynpp_lb(),((_tmp821[0]=Cyc_PP_text(((_tmp823="(",_tag_dyneither(_tmp823,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp821,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1261
goto _LL19F;_LL19F:;}
# 1263
if(inprec >= myprec){
const char*_tmp828;const char*_tmp827;struct Cyc_PP_Doc*_tmp826[3];s=((_tmp826[2]=Cyc_PP_text(((_tmp827=")",_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp826[1]=s,((_tmp826[0]=Cyc_PP_text(((_tmp828="(",_tag_dyneither(_tmp828,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp826,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1268
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp338=d;struct Cyc_Absyn_Exp*_tmp33A;struct _dyneither_ptr*_tmp33C;_LL1F6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp339=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp338;if(_tmp339->tag != 0)goto _LL1F8;else{_tmp33A=_tmp339->f1;}}_LL1F7: {
const char*_tmp82D;const char*_tmp82C;struct Cyc_PP_Doc*_tmp82B[3];return(_tmp82B[2]=Cyc_PP_text(((_tmp82C="]",_tag_dyneither(_tmp82C,sizeof(char),2)))),((_tmp82B[1]=Cyc_Absynpp_exp2doc(_tmp33A),((_tmp82B[0]=Cyc_PP_text(((_tmp82D=".[",_tag_dyneither(_tmp82D,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp82B,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33B=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp338;if(_tmp33B->tag != 1)goto _LL1F5;else{_tmp33C=_tmp33B->f1;}}_LL1F9: {
const char*_tmp830;struct Cyc_PP_Doc*_tmp82F[2];return(_tmp82F[1]=Cyc_PP_textptr(_tmp33C),((_tmp82F[0]=Cyc_PP_text(((_tmp830=".",_tag_dyneither(_tmp830,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F5:;}
# 1275
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp837;const char*_tmp836;const char*_tmp835;struct Cyc_PP_Doc*_tmp834[2];return(_tmp834[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp834[0]=
# 1277
Cyc_PP_egroup(((_tmp837="",_tag_dyneither(_tmp837,sizeof(char),1))),((_tmp836="=",_tag_dyneither(_tmp836,sizeof(char),2))),((_tmp835="=",_tag_dyneither(_tmp835,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp834,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1281
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp83A;const char*_tmp839;const char*_tmp838;return Cyc_PP_group(((_tmp838="",_tag_dyneither(_tmp838,sizeof(char),1))),((_tmp839="",_tag_dyneither(_tmp839,sizeof(char),1))),((_tmp83A=",",_tag_dyneither(_tmp83A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1286
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp349=c;enum Cyc_Absyn_Sign _tmp34A;char _tmp34B;struct _dyneither_ptr _tmp34C;enum Cyc_Absyn_Sign _tmp34D;short _tmp34E;int _tmp34F;int _tmp350;int _tmp351;enum Cyc_Absyn_Sign _tmp352;long long _tmp353;struct _dyneither_ptr _tmp354;struct _dyneither_ptr _tmp355;struct _dyneither_ptr _tmp356;_LL1FB: if((_tmp349.Char_c).tag != 2)goto _LL1FD;_tmp34A=((struct _tuple3)(_tmp349.Char_c).val).f1;_tmp34B=((struct _tuple3)(_tmp349.Char_c).val).f2;_LL1FC: {
const char*_tmp83E;void*_tmp83D[1];struct Cyc_String_pa_PrintArg_struct _tmp83C;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83C.tag=0,((_tmp83C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34B)),((_tmp83D[0]=& _tmp83C,Cyc_aprintf(((_tmp83E="'%s'",_tag_dyneither(_tmp83E,sizeof(char),5))),_tag_dyneither(_tmp83D,sizeof(void*),1)))))))));}_LL1FD: if((_tmp349.Wchar_c).tag != 3)goto _LL1FF;_tmp34C=(struct _dyneither_ptr)(_tmp349.Wchar_c).val;_LL1FE: {
const char*_tmp842;void*_tmp841[1];struct Cyc_String_pa_PrintArg_struct _tmp840;return Cyc_PP_text((struct _dyneither_ptr)((_tmp840.tag=0,((_tmp840.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34C),((_tmp841[0]=& _tmp840,Cyc_aprintf(((_tmp842="L'%s'",_tag_dyneither(_tmp842,sizeof(char),6))),_tag_dyneither(_tmp841,sizeof(void*),1)))))))));}_LL1FF: if((_tmp349.Short_c).tag != 4)goto _LL201;_tmp34D=((struct _tuple4)(_tmp349.Short_c).val).f1;_tmp34E=((struct _tuple4)(_tmp349.Short_c).val).f2;_LL200: {
const char*_tmp846;void*_tmp845[1];struct Cyc_Int_pa_PrintArg_struct _tmp844;return Cyc_PP_text((struct _dyneither_ptr)((_tmp844.tag=1,((_tmp844.f1=(unsigned long)((int)_tmp34E),((_tmp845[0]=& _tmp844,Cyc_aprintf(((_tmp846="%d",_tag_dyneither(_tmp846,sizeof(char),3))),_tag_dyneither(_tmp845,sizeof(void*),1)))))))));}_LL201: if((_tmp349.Int_c).tag != 5)goto _LL203;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_None)goto _LL203;_tmp34F=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL202:
 _tmp350=_tmp34F;goto _LL204;_LL203: if((_tmp349.Int_c).tag != 5)goto _LL205;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL205;_tmp350=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL204: {
const char*_tmp84A;void*_tmp849[1];struct Cyc_Int_pa_PrintArg_struct _tmp848;return Cyc_PP_text((struct _dyneither_ptr)((_tmp848.tag=1,((_tmp848.f1=(unsigned long)_tmp350,((_tmp849[0]=& _tmp848,Cyc_aprintf(((_tmp84A="%d",_tag_dyneither(_tmp84A,sizeof(char),3))),_tag_dyneither(_tmp849,sizeof(void*),1)))))))));}_LL205: if((_tmp349.Int_c).tag != 5)goto _LL207;if(((struct _tuple5)(_tmp349.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL207;_tmp351=((struct _tuple5)(_tmp349.Int_c).val).f2;_LL206: {
const char*_tmp84E;void*_tmp84D[1];struct Cyc_Int_pa_PrintArg_struct _tmp84C;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84C.tag=1,((_tmp84C.f1=(unsigned int)_tmp351,((_tmp84D[0]=& _tmp84C,Cyc_aprintf(((_tmp84E="%u",_tag_dyneither(_tmp84E,sizeof(char),3))),_tag_dyneither(_tmp84D,sizeof(void*),1)))))))));}_LL207: if((_tmp349.LongLong_c).tag != 6)goto _LL209;_tmp352=((struct _tuple6)(_tmp349.LongLong_c).val).f1;_tmp353=((struct _tuple6)(_tmp349.LongLong_c).val).f2;_LL208: {
# 1295
const char*_tmp84F;return Cyc_PP_text(((_tmp84F="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84F,sizeof(char),27))));}_LL209: if((_tmp349.Float_c).tag != 7)goto _LL20B;_tmp354=((struct _tuple7)(_tmp349.Float_c).val).f1;_LL20A:
 return Cyc_PP_text(_tmp354);_LL20B: if((_tmp349.Null_c).tag != 1)goto _LL20D;_LL20C: {
const char*_tmp850;return Cyc_PP_text(((_tmp850="NULL",_tag_dyneither(_tmp850,sizeof(char),5))));}_LL20D: if((_tmp349.String_c).tag != 8)goto _LL20F;_tmp355=(struct _dyneither_ptr)(_tmp349.String_c).val;_LL20E: {
# 1299
const char*_tmp855;const char*_tmp854;struct Cyc_PP_Doc*_tmp853[3];return(_tmp853[2]=Cyc_PP_text(((_tmp854="\"",_tag_dyneither(_tmp854,sizeof(char),2)))),((_tmp853[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp355)),((_tmp853[0]=Cyc_PP_text(((_tmp855="\"",_tag_dyneither(_tmp855,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp853,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20F: if((_tmp349.Wstring_c).tag != 9)goto _LL1FA;_tmp356=(struct _dyneither_ptr)(_tmp349.Wstring_c).val;_LL210: {
# 1301
const char*_tmp85A;const char*_tmp859;struct Cyc_PP_Doc*_tmp858[3];return(_tmp858[2]=Cyc_PP_text(((_tmp859="\"",_tag_dyneither(_tmp859,sizeof(char),2)))),((_tmp858[1]=Cyc_PP_text(_tmp356),((_tmp858[0]=Cyc_PP_text(((_tmp85A="L\"",_tag_dyneither(_tmp85A,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp858,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FA:;}
# 1305
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp867;const char*_tmp866;void*_tmp865[1];struct Cyc_String_pa_PrintArg_struct _tmp864;struct Cyc_Core_Failure_exn_struct*_tmp863;(int)_throw((void*)((_tmp863=_cycalloc(sizeof(*_tmp863)),((_tmp863[0]=((_tmp867.tag=Cyc_Core_Failure,((_tmp867.f1=(struct _dyneither_ptr)((_tmp864.tag=0,((_tmp864.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp865[0]=& _tmp864,Cyc_aprintf(((_tmp866="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp866,sizeof(char),47))),_tag_dyneither(_tmp865,sizeof(void*),1)))))))),_tmp867)))),_tmp863)))));}{
# 1311
const char*_tmp86C;const char*_tmp86B;struct Cyc_PP_Doc*_tmp86A[3];return(_tmp86A[2]=Cyc_PP_text(((_tmp86B=")",_tag_dyneither(_tmp86B,sizeof(char),2)))),((_tmp86A[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp86A[0]=Cyc_PP_text(((_tmp86C="numelts(",_tag_dyneither(_tmp86C,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp86A,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1313
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp879;const char*_tmp878;void*_tmp877[1];struct Cyc_String_pa_PrintArg_struct _tmp876;struct Cyc_Core_Failure_exn_struct*_tmp875;(int)_throw((void*)((_tmp875=_cycalloc(sizeof(*_tmp875)),((_tmp875[0]=((_tmp879.tag=Cyc_Core_Failure,((_tmp879.f1=(struct _dyneither_ptr)((_tmp876.tag=0,((_tmp876.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp877[0]=& _tmp876,Cyc_aprintf(((_tmp878="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp878,sizeof(char),38))),_tag_dyneither(_tmp877,sizeof(void*),1)))))))),_tmp879)))),_tmp875)))));}else{
# 1317
if(ds->tl == 0){
const char*_tmp87C;struct Cyc_PP_Doc*_tmp87B[3];return(_tmp87B[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp87B[1]=Cyc_PP_text(((_tmp87C=" ",_tag_dyneither(_tmp87C,sizeof(char),2)))),((_tmp87B[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp87B,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp889;const char*_tmp888;void*_tmp887[1];struct Cyc_String_pa_PrintArg_struct _tmp886;struct Cyc_Core_Failure_exn_struct*_tmp885;(int)_throw((void*)((_tmp885=_cycalloc(sizeof(*_tmp885)),((_tmp885[0]=((_tmp889.tag=Cyc_Core_Failure,((_tmp889.f1=(struct _dyneither_ptr)((_tmp886.tag=0,((_tmp886.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp887[0]=& _tmp886,Cyc_aprintf(((_tmp888="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp888,sizeof(char),47))),_tag_dyneither(_tmp887,sizeof(void*),1)))))))),_tmp889)))),_tmp885)))));}else{
# 1323
const char*_tmp88E;const char*_tmp88D;struct Cyc_PP_Doc*_tmp88C[5];return(_tmp88C[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp88C[3]=Cyc_PP_text(((_tmp88D=" ",_tag_dyneither(_tmp88D,sizeof(char),2)))),((_tmp88C[2]=ps,((_tmp88C[1]=Cyc_PP_text(((_tmp88E=" ",_tag_dyneither(_tmp88E,sizeof(char),2)))),((_tmp88C[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp88C,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1327
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL211: {
const char*_tmp88F;return(_tmp88F="+",_tag_dyneither(_tmp88F,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {
const char*_tmp890;return(_tmp890="*",_tag_dyneither(_tmp890,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
const char*_tmp891;return(_tmp891="-",_tag_dyneither(_tmp891,sizeof(char),2));}case Cyc_Absyn_Div: _LL214: {
const char*_tmp892;return(_tmp892="/",_tag_dyneither(_tmp892,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {
const char*_tmp894;const char*_tmp893;return Cyc_Absynpp_print_for_cycdoc?(_tmp894="\\%",_tag_dyneither(_tmp894,sizeof(char),3)):((_tmp893="%",_tag_dyneither(_tmp893,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL216: {
const char*_tmp895;return(_tmp895="==",_tag_dyneither(_tmp895,sizeof(char),3));}case Cyc_Absyn_Neq: _LL217: {
const char*_tmp896;return(_tmp896="!=",_tag_dyneither(_tmp896,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {
const char*_tmp897;return(_tmp897=">",_tag_dyneither(_tmp897,sizeof(char),2));}case Cyc_Absyn_Lt: _LL219: {
const char*_tmp898;return(_tmp898="<",_tag_dyneither(_tmp898,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21A: {
const char*_tmp899;return(_tmp899=">=",_tag_dyneither(_tmp899,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {
const char*_tmp89A;return(_tmp89A="<=",_tag_dyneither(_tmp89A,sizeof(char),3));}case Cyc_Absyn_Not: _LL21C: {
const char*_tmp89B;return(_tmp89B="!",_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL21D: {
const char*_tmp89C;return(_tmp89C="~",_tag_dyneither(_tmp89C,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {
const char*_tmp89D;return(_tmp89D="&",_tag_dyneither(_tmp89D,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL21F: {
const char*_tmp89E;return(_tmp89E="|",_tag_dyneither(_tmp89E,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL220: {
const char*_tmp89F;return(_tmp89F="^",_tag_dyneither(_tmp89F,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {
const char*_tmp8A0;return(_tmp8A0="<<",_tag_dyneither(_tmp8A0,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL222: {
const char*_tmp8A1;return(_tmp8A1=">>",_tag_dyneither(_tmp8A1,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {
const char*_tmp8A2;return(_tmp8A2=">>>",_tag_dyneither(_tmp8A2,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL224: {
const char*_tmp8A3;return(_tmp8A3="numelts",_tag_dyneither(_tmp8A3,sizeof(char),8));}}}
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
{const char*_tmp8A4;s=Cyc_PP_text(((_tmp8A4=";",_tag_dyneither(_tmp8A4,sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp39E=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp39E->tag != 1)goto _LL230;else{_tmp39F=_tmp39E->f1;}}_LL22F:
{const char*_tmp8A7;struct Cyc_PP_Doc*_tmp8A6[2];s=((_tmp8A6[1]=Cyc_PP_text(((_tmp8A7=";",_tag_dyneither(_tmp8A7,sizeof(char),2)))),((_tmp8A6[0]=Cyc_Absynpp_exp2doc(_tmp39F),Cyc_PP_cat(_tag_dyneither(_tmp8A6,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3A0=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A0->tag != 2)goto _LL232;else{_tmp3A1=_tmp3A0->f1;_tmp3A2=_tmp3A0->f2;}}_LL231:
# 1369
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3A1)){
struct Cyc_PP_Doc*_tmp8B2[5];struct Cyc_PP_Doc*_tmp8B1[7];const char*_tmp8B0;const char*_tmp8AF;struct Cyc_PP_Doc*_tmp8AE[7];s=((_tmp8AE[6]=
# 1377
Cyc_Absynpp_is_declaration(_tmp3A2)?
stmtexp?
(_tmp8B1[6]=
# 1382
Cyc_PP_line_doc(),((_tmp8B1[5]=
# 1381
Cyc_PP_text(((_tmp8B0=");",_tag_dyneither(_tmp8B0,sizeof(char),3)))),((_tmp8B1[4]=Cyc_Absynpp_rb(),((_tmp8B1[3]=
# 1380
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8B1[2]=
# 1379
Cyc_PP_blank_doc(),((_tmp8B1[1]=Cyc_Absynpp_lb(),((_tmp8B1[0]=Cyc_PP_text(((_tmp8AF="(",_tag_dyneither(_tmp8AF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1383
(_tmp8B2[4]=
# 1386
Cyc_PP_line_doc(),((_tmp8B2[3]=
# 1385
Cyc_Absynpp_rb(),((_tmp8B2[2]=
# 1384
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8B2[1]=
# 1383
Cyc_PP_blank_doc(),((_tmp8B2[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B2,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1387
 Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8AE[5]=
# 1376
Cyc_PP_line_doc(),((_tmp8AE[4]=
# 1375
Cyc_Absynpp_rb(),((_tmp8AE[3]=
# 1374
Cyc_PP_line_doc(),((_tmp8AE[2]=
# 1373
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A1,0)),((_tmp8AE[1]=
# 1372
Cyc_PP_blank_doc(),((_tmp8AE[0]=
# 1371
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8AE,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1388
if(Cyc_Absynpp_is_declaration(_tmp3A2)){
struct Cyc_PP_Doc*_tmp8BD[4];struct Cyc_PP_Doc*_tmp8BC[6];const char*_tmp8BB;const char*_tmp8BA;struct Cyc_PP_Doc*_tmp8B9[4];s=((_tmp8B9[3]=
# 1399
Cyc_PP_line_doc(),((_tmp8B9[2]=
# 1391
stmtexp?
(_tmp8BC[5]=
# 1394
Cyc_PP_text(((_tmp8BB=");",_tag_dyneither(_tmp8BB,sizeof(char),3)))),((_tmp8BC[4]=Cyc_Absynpp_rb(),((_tmp8BC[3]=
# 1393
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8BC[2]=
# 1392
Cyc_PP_blank_doc(),((_tmp8BC[1]=Cyc_Absynpp_lb(),((_tmp8BC[0]=Cyc_PP_text(((_tmp8BA="(",_tag_dyneither(_tmp8BA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BC,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1396
(_tmp8BD[3]=
# 1398
Cyc_Absynpp_rb(),((_tmp8BD[2]=
# 1397
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp)),((_tmp8BD[1]=
# 1396
Cyc_PP_blank_doc(),((_tmp8BD[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8BD,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8B9[1]=
# 1390
Cyc_PP_line_doc(),((_tmp8B9[0]=
# 1389
Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1401
struct Cyc_PP_Doc*_tmp8BE[3];s=((_tmp8BE[2]=Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8BE[1]=Cyc_PP_line_doc(),((_tmp8BE[0]=Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1404
struct Cyc_PP_Doc*_tmp8BF[3];s=((_tmp8BF[2]=Cyc_Absynpp_stmt2doc(_tmp3A2,stmtexp),((_tmp8BF[1]=Cyc_PP_line_doc(),((_tmp8BF[0]=Cyc_Absynpp_stmt2doc(_tmp3A1,0),Cyc_PP_cat(_tag_dyneither(_tmp8BF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A3=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A3->tag != 3)goto _LL234;else{_tmp3A4=_tmp3A3->f1;}}_LL233:
# 1407
 if(_tmp3A4 == 0){
const char*_tmp8C0;s=Cyc_PP_text(((_tmp8C0="return;",_tag_dyneither(_tmp8C0,sizeof(char),8))));}else{
# 1410
const char*_tmp8C5;const char*_tmp8C4;struct Cyc_PP_Doc*_tmp8C3[3];s=((_tmp8C3[2]=
# 1412
Cyc_PP_text(((_tmp8C4=";",_tag_dyneither(_tmp8C4,sizeof(char),2)))),((_tmp8C3[1]=
# 1411
_tmp3A4 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3A4),((_tmp8C3[0]=
# 1410
Cyc_PP_text(((_tmp8C5="return ",_tag_dyneither(_tmp8C5,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8C3,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1413
goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3A5=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A5->tag != 4)goto _LL236;else{_tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_tmp3A8=_tmp3A5->f3;}}_LL235: {
# 1415
int print_else;
{void*_tmp3DC=_tmp3A8->r;_LL251: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3DC;if(_tmp3DD->tag != 0)goto _LL253;}_LL252:
 print_else=0;goto _LL250;_LL253:;_LL254:
 print_else=1;goto _LL250;_LL250:;}
# 1420
{const char*_tmp8D4;const char*_tmp8D3;struct Cyc_PP_Doc*_tmp8D2[2];struct Cyc_PP_Doc*_tmp8D1[6];struct Cyc_PP_Doc*_tmp8D0[2];const char*_tmp8CF;struct Cyc_PP_Doc*_tmp8CE[8];s=((_tmp8CE[7]=
# 1427
print_else?
(_tmp8D1[5]=
# 1433
Cyc_Absynpp_rb(),((_tmp8D1[4]=
# 1432
Cyc_PP_line_doc(),((_tmp8D1[3]=
# 1431
Cyc_PP_nest(2,((_tmp8D0[1]=Cyc_Absynpp_stmt2doc(_tmp3A8,0),((_tmp8D0[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D1[2]=
# 1430
Cyc_Absynpp_lb(),((_tmp8D1[1]=
# 1429
Cyc_PP_text(((_tmp8CF="else ",_tag_dyneither(_tmp8CF,sizeof(char),6)))),((_tmp8D1[0]=
# 1428
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D1,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1434
 Cyc_PP_nil_doc(),((_tmp8CE[6]=
# 1426
Cyc_Absynpp_rb(),((_tmp8CE[5]=
# 1425
Cyc_PP_line_doc(),((_tmp8CE[4]=
# 1424
Cyc_PP_nest(2,((_tmp8D2[1]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),((_tmp8D2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D2,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CE[3]=
# 1423
Cyc_Absynpp_lb(),((_tmp8CE[2]=
# 1422
Cyc_PP_text(((_tmp8D3=") ",_tag_dyneither(_tmp8D3,sizeof(char),3)))),((_tmp8CE[1]=
# 1421
Cyc_Absynpp_exp2doc(_tmp3A6),((_tmp8CE[0]=
# 1420
Cyc_PP_text(((_tmp8D4="if (",_tag_dyneither(_tmp8D4,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1435
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3A9=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3A9->tag != 5)goto _LL238;else{_tmp3AA=(_tmp3A9->f1).f1;_tmp3AB=_tmp3A9->f2;}}_LL237:
# 1437
{const char*_tmp8DB;const char*_tmp8DA;struct Cyc_PP_Doc*_tmp8D9[2];struct Cyc_PP_Doc*_tmp8D8[7];s=((_tmp8D8[6]=
# 1443
Cyc_Absynpp_rb(),((_tmp8D8[5]=
# 1442
Cyc_PP_line_doc(),((_tmp8D8[4]=
# 1441
Cyc_PP_nest(2,((_tmp8D9[1]=Cyc_Absynpp_stmt2doc(_tmp3AB,0),((_tmp8D9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D8[3]=
# 1440
Cyc_Absynpp_lb(),((_tmp8D8[2]=
# 1439
Cyc_PP_text(((_tmp8DA=") ",_tag_dyneither(_tmp8DA,sizeof(char),3)))),((_tmp8D8[1]=
# 1438
Cyc_Absynpp_exp2doc(_tmp3AA),((_tmp8D8[0]=
# 1437
Cyc_PP_text(((_tmp8DB="while (",_tag_dyneither(_tmp8DB,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D8,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1444
goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3AC=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AC->tag != 6)goto _LL23A;}_LL239:
{const char*_tmp8DC;s=Cyc_PP_text(((_tmp8DC="break;",_tag_dyneither(_tmp8DC,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3AD=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AD->tag != 7)goto _LL23C;}_LL23B:
{const char*_tmp8DD;s=Cyc_PP_text(((_tmp8DD="continue;",_tag_dyneither(_tmp8DD,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3AE=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3AE->tag != 8)goto _LL23E;else{_tmp3AF=_tmp3AE->f1;}}_LL23D:
{const char*_tmp8E1;void*_tmp8E0[1];struct Cyc_String_pa_PrintArg_struct _tmp8DF;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DF.tag=0,((_tmp8DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF),((_tmp8E0[0]=& _tmp8DF,Cyc_aprintf(((_tmp8E1="goto %s;",_tag_dyneither(_tmp8E1,sizeof(char),9))),_tag_dyneither(_tmp8E0,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3B0=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B0->tag != 9)goto _LL240;else{_tmp3B1=_tmp3B0->f1;_tmp3B2=(_tmp3B0->f2).f1;_tmp3B3=(_tmp3B0->f3).f1;_tmp3B4=_tmp3B0->f4;}}_LL23F:
# 1449
{const char*_tmp8EC;const char*_tmp8EB;const char*_tmp8EA;const char*_tmp8E9;struct Cyc_PP_Doc*_tmp8E8[2];struct Cyc_PP_Doc*_tmp8E7[11];s=((_tmp8E7[10]=
# 1459
Cyc_Absynpp_rb(),((_tmp8E7[9]=
# 1458
Cyc_PP_line_doc(),((_tmp8E7[8]=
# 1457
Cyc_PP_nest(2,((_tmp8E8[1]=Cyc_Absynpp_stmt2doc(_tmp3B4,0),((_tmp8E8[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E8,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E7[7]=
# 1456
Cyc_Absynpp_lb(),((_tmp8E7[6]=
# 1455
Cyc_PP_text(((_tmp8E9=") ",_tag_dyneither(_tmp8E9,sizeof(char),3)))),((_tmp8E7[5]=
# 1454
Cyc_Absynpp_exp2doc(_tmp3B3),((_tmp8E7[4]=
# 1453
Cyc_PP_text(((_tmp8EA="; ",_tag_dyneither(_tmp8EA,sizeof(char),3)))),((_tmp8E7[3]=
# 1452
Cyc_Absynpp_exp2doc(_tmp3B2),((_tmp8E7[2]=
# 1451
Cyc_PP_text(((_tmp8EB="; ",_tag_dyneither(_tmp8EB,sizeof(char),3)))),((_tmp8E7[1]=
# 1450
Cyc_Absynpp_exp2doc(_tmp3B1),((_tmp8E7[0]=
# 1449
Cyc_PP_text(((_tmp8EC="for(",_tag_dyneither(_tmp8EC,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E7,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1460
goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3B5=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B5->tag != 10)goto _LL242;else{_tmp3B6=_tmp3B5->f1;_tmp3B7=_tmp3B5->f2;}}_LL241:
# 1462
{const char*_tmp8F1;const char*_tmp8F0;struct Cyc_PP_Doc*_tmp8EF[8];s=((_tmp8EF[7]=
# 1469
Cyc_Absynpp_rb(),((_tmp8EF[6]=
# 1468
Cyc_PP_line_doc(),((_tmp8EF[5]=
# 1467
Cyc_Absynpp_switchclauses2doc(_tmp3B7),((_tmp8EF[4]=
# 1466
Cyc_PP_line_doc(),((_tmp8EF[3]=
# 1465
Cyc_Absynpp_lb(),((_tmp8EF[2]=
# 1464
Cyc_PP_text(((_tmp8F0=") ",_tag_dyneither(_tmp8F0,sizeof(char),3)))),((_tmp8EF[1]=
# 1463
Cyc_Absynpp_exp2doc(_tmp3B6),((_tmp8EF[0]=
# 1462
Cyc_PP_text(((_tmp8F1="switch (",_tag_dyneither(_tmp8F1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8EF,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1470
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B8=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B8->tag != 11)goto _LL244;else{if(_tmp3B8->f1 != 0)goto _LL244;}}_LL243:
{const char*_tmp8F2;s=Cyc_PP_text(((_tmp8F2="fallthru;",_tag_dyneither(_tmp8F2,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3B9->tag != 11)goto _LL246;else{_tmp3BA=_tmp3B9->f1;}}_LL245:
# 1473
{const char*_tmp8F7;const char*_tmp8F6;struct Cyc_PP_Doc*_tmp8F5[3];s=((_tmp8F5[2]=Cyc_PP_text(((_tmp8F6=");",_tag_dyneither(_tmp8F6,sizeof(char),3)))),((_tmp8F5[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3BA),((_tmp8F5[0]=Cyc_PP_text(((_tmp8F7="fallthru(",_tag_dyneither(_tmp8F7,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F5,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3BB=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3BB->tag != 12)goto _LL248;else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;}}_LL247:
# 1475
{struct Cyc_PP_Doc*_tmp8F8[3];s=((_tmp8F8[2]=
# 1477
Cyc_Absynpp_stmt2doc(_tmp3BD,stmtexp),((_tmp8F8[1]=
# 1476
Cyc_PP_line_doc(),((_tmp8F8[0]=
# 1475
Cyc_Absynpp_decl2doc(_tmp3BC),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1478
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3BE=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3BE->tag != 13)goto _LL24A;else{_tmp3BF=_tmp3BE->f1;_tmp3C0=_tmp3BE->f2;}}_LL249:
# 1480
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C0)){
if(stmtexp){
const char*_tmp8FD;const char*_tmp8FC;struct Cyc_PP_Doc*_tmp8FB[8];s=((_tmp8FB[7]=
# 1488
Cyc_PP_text(((_tmp8FC=");",_tag_dyneither(_tmp8FC,sizeof(char),3)))),((_tmp8FB[6]=Cyc_Absynpp_rb(),((_tmp8FB[5]=
# 1487
Cyc_PP_line_doc(),((_tmp8FB[4]=
# 1486
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,1)),((_tmp8FB[3]=
# 1485
Cyc_PP_line_doc(),((_tmp8FB[2]=
# 1484
Cyc_Absynpp_lb(),((_tmp8FB[1]=
# 1483
Cyc_PP_text(((_tmp8FD=": (",_tag_dyneither(_tmp8FD,sizeof(char),4)))),((_tmp8FB[0]=
# 1482
Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp8FB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1490
const char*_tmp900;struct Cyc_PP_Doc*_tmp8FF[7];s=((_tmp8FF[6]=
# 1496
Cyc_Absynpp_rb(),((_tmp8FF[5]=
# 1495
Cyc_PP_line_doc(),((_tmp8FF[4]=
# 1494
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,0)),((_tmp8FF[3]=
# 1493
Cyc_PP_line_doc(),((_tmp8FF[2]=
# 1492
Cyc_Absynpp_lb(),((_tmp8FF[1]=
# 1491
Cyc_PP_text(((_tmp900=": ",_tag_dyneither(_tmp900,sizeof(char),3)))),((_tmp8FF[0]=
# 1490
Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp8FF,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1498
const char*_tmp903;struct Cyc_PP_Doc*_tmp902[3];s=((_tmp902[2]=Cyc_Absynpp_stmt2doc(_tmp3C0,stmtexp),((_tmp902[1]=Cyc_PP_text(((_tmp903=": ",_tag_dyneither(_tmp903,sizeof(char),3)))),((_tmp902[0]=Cyc_PP_textptr(_tmp3BF),Cyc_PP_cat(_tag_dyneither(_tmp902,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3C1=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C1->tag != 14)goto _LL24C;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=(_tmp3C1->f2).f1;}}_LL24B:
# 1501
{const char*_tmp90A;const char*_tmp909;const char*_tmp908;struct Cyc_PP_Doc*_tmp907[9];s=((_tmp907[8]=
# 1509
Cyc_PP_text(((_tmp908=");",_tag_dyneither(_tmp908,sizeof(char),3)))),((_tmp907[7]=
# 1508
Cyc_Absynpp_exp2doc(_tmp3C3),((_tmp907[6]=
# 1507
Cyc_PP_text(((_tmp909=" while (",_tag_dyneither(_tmp909,sizeof(char),9)))),((_tmp907[5]=
# 1506
Cyc_Absynpp_rb(),((_tmp907[4]=
# 1505
Cyc_PP_line_doc(),((_tmp907[3]=
# 1504
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C2,0)),((_tmp907[2]=
# 1503
Cyc_PP_line_doc(),((_tmp907[1]=
# 1502
Cyc_Absynpp_lb(),((_tmp907[0]=
# 1501
Cyc_PP_text(((_tmp90A="do ",_tag_dyneither(_tmp90A,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp907,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1510
goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3C4=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C4->tag != 15)goto _LL24E;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL24D:
# 1512
{const char*_tmp90F;const char*_tmp90E;struct Cyc_PP_Doc*_tmp90D[12];s=((_tmp90D[11]=
# 1523
Cyc_Absynpp_rb(),((_tmp90D[10]=
# 1522
Cyc_PP_line_doc(),((_tmp90D[9]=
# 1521
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3C6)),((_tmp90D[8]=
# 1520
Cyc_PP_line_doc(),((_tmp90D[7]=
# 1519
Cyc_Absynpp_lb(),((_tmp90D[6]=
# 1518
Cyc_PP_text(((_tmp90E=" catch ",_tag_dyneither(_tmp90E,sizeof(char),8)))),((_tmp90D[5]=
# 1517
Cyc_Absynpp_rb(),((_tmp90D[4]=
# 1516
Cyc_PP_line_doc(),((_tmp90D[3]=
# 1515
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C5,0)),((_tmp90D[2]=
# 1514
Cyc_PP_line_doc(),((_tmp90D[1]=
# 1513
Cyc_Absynpp_lb(),((_tmp90D[0]=
# 1512
Cyc_PP_text(((_tmp90F="try ",_tag_dyneither(_tmp90F,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1524
goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3C7=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp39C;if(_tmp3C7->tag != 16)goto _LL22B;else{_tmp3C8=_tmp3C7->f1;}}_LL24F:
# 1526
{const char*_tmp914;const char*_tmp913;struct Cyc_PP_Doc*_tmp912[3];s=((_tmp912[2]=Cyc_PP_text(((_tmp913=");",_tag_dyneither(_tmp913,sizeof(char),3)))),((_tmp912[1]=Cyc_Absynpp_exp2doc(_tmp3C8),((_tmp912[0]=Cyc_PP_text(((_tmp914="reset_region(",_tag_dyneither(_tmp914,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp912,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL22B:;}
# 1529
return s;}
# 1532
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp40D=p->r;enum Cyc_Absyn_Sign _tmp411;int _tmp412;char _tmp414;struct _dyneither_ptr _tmp416;struct Cyc_Absyn_Vardecl*_tmp418;struct Cyc_Absyn_Vardecl*_tmp41B;struct Cyc_Absyn_Pat*_tmp41C;struct Cyc_Absyn_Tvar*_tmp41E;struct Cyc_Absyn_Vardecl*_tmp41F;struct Cyc_List_List*_tmp421;int _tmp422;struct Cyc_Absyn_Pat*_tmp424;struct Cyc_Absyn_Vardecl*_tmp426;struct Cyc_Absyn_Vardecl*_tmp429;struct Cyc_Absyn_Pat*_tmp42A;struct _tuple0*_tmp42C;struct _tuple0*_tmp42E;struct Cyc_List_List*_tmp42F;int _tmp430;union Cyc_Absyn_AggrInfoU _tmp432;struct Cyc_List_List*_tmp433;struct Cyc_List_List*_tmp434;int _tmp435;struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp438;int _tmp439;struct Cyc_Absyn_Enumfield*_tmp43B;struct Cyc_Absyn_Enumfield*_tmp43D;struct Cyc_Absyn_Datatypefield*_tmp43F;struct Cyc_Absyn_Datatypefield*_tmp441;struct Cyc_List_List*_tmp442;int _tmp443;struct Cyc_Absyn_Exp*_tmp445;_LL256: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp40E=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp40E->tag != 0)goto _LL258;}_LL257:
{const char*_tmp915;s=Cyc_PP_text(((_tmp915="_",_tag_dyneither(_tmp915,sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp40F=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp40F->tag != 8)goto _LL25A;}_LL259:
{const char*_tmp916;s=Cyc_PP_text(((_tmp916="NULL",_tag_dyneither(_tmp916,sizeof(char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp410=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp410->tag != 9)goto _LL25C;else{_tmp411=_tmp410->f1;_tmp412=_tmp410->f2;}}_LL25B:
# 1538
 if(_tmp411 != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){
const char*_tmp91A;void*_tmp919[1];struct Cyc_Int_pa_PrintArg_struct _tmp918;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp918.tag=1,((_tmp918.f1=(unsigned long)_tmp412,((_tmp919[0]=& _tmp918,Cyc_aprintf(((_tmp91A="%d",_tag_dyneither(_tmp91A,sizeof(char),3))),_tag_dyneither(_tmp919,sizeof(void*),1)))))))));}else{
const char*_tmp91E;void*_tmp91D[1];struct Cyc_Int_pa_PrintArg_struct _tmp91C;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91C.tag=1,((_tmp91C.f1=(unsigned int)_tmp412,((_tmp91D[0]=& _tmp91C,Cyc_aprintf(((_tmp91E="%u",_tag_dyneither(_tmp91E,sizeof(char),3))),_tag_dyneither(_tmp91D,sizeof(void*),1)))))))));}
goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp413=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp413->tag != 10)goto _LL25E;else{_tmp414=_tmp413->f1;}}_LL25D:
{const char*_tmp922;void*_tmp921[1];struct Cyc_String_pa_PrintArg_struct _tmp920;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp920.tag=0,((_tmp920.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp414)),((_tmp921[0]=& _tmp920,Cyc_aprintf(((_tmp922="'%s'",_tag_dyneither(_tmp922,sizeof(char),5))),_tag_dyneither(_tmp921,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp415=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp415->tag != 11)goto _LL260;else{_tmp416=_tmp415->f1;}}_LL25F:
 s=Cyc_PP_text(_tmp416);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp417=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp417->tag != 1)goto _LL262;else{_tmp418=_tmp417->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp419=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp417->f2).r;if(_tmp419->tag != 0)goto _LL262;};}}_LL261:
# 1545
 s=Cyc_Absynpp_qvar2doc(_tmp418->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41A=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp41A->tag != 1)goto _LL264;else{_tmp41B=_tmp41A->f1;_tmp41C=_tmp41A->f2;}}_LL263:
# 1548
{const char*_tmp925;struct Cyc_PP_Doc*_tmp924[3];s=((_tmp924[2]=Cyc_Absynpp_pat2doc(_tmp41C),((_tmp924[1]=Cyc_PP_text(((_tmp925=" as ",_tag_dyneither(_tmp925,sizeof(char),5)))),((_tmp924[0]=Cyc_Absynpp_qvar2doc(_tmp41B->name),Cyc_PP_cat(_tag_dyneither(_tmp924,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp41D=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp41D->tag != 3)goto _LL266;else{_tmp41E=_tmp41D->f1;_tmp41F=_tmp41D->f2;}}_LL265:
# 1550
{const char*_tmp92A;const char*_tmp929;struct Cyc_PP_Doc*_tmp928[4];s=((_tmp928[3]=Cyc_PP_text(((_tmp929=">",_tag_dyneither(_tmp929,sizeof(char),2)))),((_tmp928[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp41E)),((_tmp928[1]=Cyc_PP_text(((_tmp92A="<",_tag_dyneither(_tmp92A,sizeof(char),2)))),((_tmp928[0]=Cyc_Absynpp_qvar2doc(_tmp41F->name),Cyc_PP_cat(_tag_dyneither(_tmp928,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL266: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp420=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp420->tag != 4)goto _LL268;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}_LL267:
# 1553
{const char*_tmp933;const char*_tmp932;const char*_tmp931;const char*_tmp930;struct Cyc_PP_Doc*_tmp92F[4];s=((_tmp92F[3]=
_tmp422?Cyc_PP_text(((_tmp930=", ...)",_tag_dyneither(_tmp930,sizeof(char),7)))): Cyc_PP_text(((_tmp931=")",_tag_dyneither(_tmp931,sizeof(char),2)))),((_tmp92F[2]=
# 1553
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp932=",",_tag_dyneither(_tmp932,sizeof(char),2))),_tmp421),((_tmp92F[1]=Cyc_PP_text(((_tmp933="(",_tag_dyneither(_tmp933,sizeof(char),2)))),((_tmp92F[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp92F,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1555
goto _LL255;_LL268: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp423=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp423->tag != 5)goto _LL26A;else{_tmp424=_tmp423->f1;}}_LL269:
# 1557
{const char*_tmp936;struct Cyc_PP_Doc*_tmp935[2];s=((_tmp935[1]=Cyc_Absynpp_pat2doc(_tmp424),((_tmp935[0]=Cyc_PP_text(((_tmp936="&",_tag_dyneither(_tmp936,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp935,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp425=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp425->tag != 2)goto _LL26C;else{_tmp426=_tmp425->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp427=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp425->f2).r;if(_tmp427->tag != 0)goto _LL26C;};}}_LL26B:
# 1560
{const char*_tmp939;struct Cyc_PP_Doc*_tmp938[2];s=((_tmp938[1]=Cyc_Absynpp_qvar2doc(_tmp426->name),((_tmp938[0]=Cyc_PP_text(((_tmp939="*",_tag_dyneither(_tmp939,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp428=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp428->tag != 2)goto _LL26E;else{_tmp429=_tmp428->f1;_tmp42A=_tmp428->f2;}}_LL26D:
# 1563
{const char*_tmp93E;const char*_tmp93D;struct Cyc_PP_Doc*_tmp93C[4];s=((_tmp93C[3]=Cyc_Absynpp_pat2doc(_tmp42A),((_tmp93C[2]=Cyc_PP_text(((_tmp93D=" as ",_tag_dyneither(_tmp93D,sizeof(char),5)))),((_tmp93C[1]=Cyc_Absynpp_qvar2doc(_tmp429->name),((_tmp93C[0]=Cyc_PP_text(((_tmp93E="*",_tag_dyneither(_tmp93E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93C,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp42B=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp42B->tag != 14)goto _LL270;else{_tmp42C=_tmp42B->f1;}}_LL26F:
# 1566
 s=Cyc_Absynpp_qvar2doc(_tmp42C);
goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp42D=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp42D->tag != 15)goto _LL272;else{_tmp42E=_tmp42D->f1;_tmp42F=_tmp42D->f2;_tmp430=_tmp42D->f3;}}_LL271: {
# 1569
const char*_tmp940;const char*_tmp93F;struct _dyneither_ptr term=_tmp430?(_tmp940=", ...)",_tag_dyneither(_tmp940,sizeof(char),7)):((_tmp93F=")",_tag_dyneither(_tmp93F,sizeof(char),2)));
{const char*_tmp945;const char*_tmp944;struct Cyc_PP_Doc*_tmp943[2];s=((_tmp943[1]=Cyc_PP_group(((_tmp945="(",_tag_dyneither(_tmp945,sizeof(char),2))),term,((_tmp944=",",_tag_dyneither(_tmp944,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp42F)),((_tmp943[0]=Cyc_Absynpp_qvar2doc(_tmp42E),Cyc_PP_cat(_tag_dyneither(_tmp943,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL272: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp431=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp431->tag != 6)goto _LL274;else{if(_tmp431->f1 == 0)goto _LL274;_tmp432=(*_tmp431->f1).aggr_info;_tmp433=_tmp431->f2;_tmp434=_tmp431->f3;_tmp435=_tmp431->f4;}}_LL273: {
# 1573
const char*_tmp947;const char*_tmp946;struct _dyneither_ptr term=_tmp435?(_tmp947=", ...}",_tag_dyneither(_tmp947,sizeof(char),7)):((_tmp946="}",_tag_dyneither(_tmp946,sizeof(char),2)));
struct _tuple0*_tmp468;struct _tuple10 _tmp467=Cyc_Absyn_aggr_kinded_name(_tmp432);_tmp468=_tmp467.f2;
{const char*_tmp952;const char*_tmp951;const char*_tmp950;const char*_tmp94F;const char*_tmp94E;struct Cyc_PP_Doc*_tmp94D[4];s=((_tmp94D[3]=
# 1577
Cyc_PP_group(((_tmp94F="",_tag_dyneither(_tmp94F,sizeof(char),1))),term,((_tmp94E=",",_tag_dyneither(_tmp94E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp434)),((_tmp94D[2]=
# 1576
Cyc_PP_egroup(((_tmp952="[",_tag_dyneither(_tmp952,sizeof(char),2))),((_tmp951="]",_tag_dyneither(_tmp951,sizeof(char),2))),((_tmp950=",",_tag_dyneither(_tmp950,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp433))),((_tmp94D[1]=
# 1575
Cyc_Absynpp_lb(),((_tmp94D[0]=Cyc_Absynpp_qvar2doc(_tmp468),Cyc_PP_cat(_tag_dyneither(_tmp94D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1578
goto _LL255;}_LL274: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp436=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp436->tag != 6)goto _LL276;else{if(_tmp436->f1 != 0)goto _LL276;_tmp437=_tmp436->f2;_tmp438=_tmp436->f3;_tmp439=_tmp436->f4;}}_LL275: {
# 1580
const char*_tmp954;const char*_tmp953;struct _dyneither_ptr term=_tmp439?(_tmp954=", ...}",_tag_dyneither(_tmp954,sizeof(char),7)):((_tmp953="}",_tag_dyneither(_tmp953,sizeof(char),2)));
{const char*_tmp95F;const char*_tmp95E;const char*_tmp95D;const char*_tmp95C;const char*_tmp95B;struct Cyc_PP_Doc*_tmp95A[3];s=((_tmp95A[2]=
# 1583
Cyc_PP_group(((_tmp95C="",_tag_dyneither(_tmp95C,sizeof(char),1))),term,((_tmp95B=",",_tag_dyneither(_tmp95B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp438)),((_tmp95A[1]=
# 1582
Cyc_PP_egroup(((_tmp95F="[",_tag_dyneither(_tmp95F,sizeof(char),2))),((_tmp95E="]",_tag_dyneither(_tmp95E,sizeof(char),2))),((_tmp95D=",",_tag_dyneither(_tmp95D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp437))),((_tmp95A[0]=
# 1581
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp95A,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1584
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp43A=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43A->tag != 12)goto _LL278;else{_tmp43B=_tmp43A->f2;}}_LL277:
 _tmp43D=_tmp43B;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp43C=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43C->tag != 13)goto _LL27A;else{_tmp43D=_tmp43C->f2;}}_LL279:
 s=Cyc_Absynpp_qvar2doc(_tmp43D->name);goto _LL255;_LL27A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp43E->tag != 7)goto _LL27C;else{_tmp43F=_tmp43E->f2;if(_tmp43E->f3 != 0)goto _LL27C;}}_LL27B:
 s=Cyc_Absynpp_qvar2doc(_tmp43F->name);goto _LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp440->tag != 7)goto _LL27E;else{_tmp441=_tmp440->f2;_tmp442=_tmp440->f3;_tmp443=_tmp440->f4;}}_LL27D: {
# 1589
const char*_tmp961;const char*_tmp960;struct _dyneither_ptr term=_tmp443?(_tmp961=", ...)",_tag_dyneither(_tmp961,sizeof(char),7)):((_tmp960=")",_tag_dyneither(_tmp960,sizeof(char),2)));
{const char*_tmp966;const char*_tmp965;struct Cyc_PP_Doc*_tmp964[2];s=((_tmp964[1]=Cyc_PP_egroup(((_tmp966="(",_tag_dyneither(_tmp966,sizeof(char),2))),term,((_tmp965=",",_tag_dyneither(_tmp965,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp442)),((_tmp964[0]=Cyc_Absynpp_qvar2doc(_tmp441->name),Cyc_PP_cat(_tag_dyneither(_tmp964,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL27E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp444=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp40D;if(_tmp444->tag != 16)goto _LL255;else{_tmp445=_tmp444->f1;}}_LL27F:
# 1593
 s=Cyc_Absynpp_exp2doc(_tmp445);goto _LL255;_LL255:;}
# 1595
return s;}
# 1598
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp96D;const char*_tmp96C;const char*_tmp96B;struct Cyc_PP_Doc*_tmp96A[2];return(_tmp96A[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp96A[0]=
# 1599
Cyc_PP_egroup(((_tmp96D="",_tag_dyneither(_tmp96D,sizeof(char),1))),((_tmp96C="=",_tag_dyneither(_tmp96C,sizeof(char),2))),((_tmp96B="=",_tag_dyneither(_tmp96B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp96A,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1603
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp972;struct Cyc_PP_Doc*_tmp971[2];struct Cyc_PP_Doc*_tmp970[2];return(_tmp970[1]=
Cyc_PP_nest(2,((_tmp971[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp971[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp971,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp970[0]=
# 1605
Cyc_PP_text(((_tmp972="default: ",_tag_dyneither(_tmp972,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp970,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1607
if(c->where_clause == 0){
const char*_tmp979;const char*_tmp978;struct Cyc_PP_Doc*_tmp977[2];struct Cyc_PP_Doc*_tmp976[4];return(_tmp976[3]=
# 1611
Cyc_PP_nest(2,((_tmp977[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp977[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp977,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp976[2]=
# 1610
Cyc_PP_text(((_tmp978=": ",_tag_dyneither(_tmp978,sizeof(char),3)))),((_tmp976[1]=
# 1609
Cyc_Absynpp_pat2doc(c->pattern),((_tmp976[0]=
# 1608
Cyc_PP_text(((_tmp979="case ",_tag_dyneither(_tmp979,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp976,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1613
const char*_tmp982;const char*_tmp981;const char*_tmp980;struct Cyc_PP_Doc*_tmp97F[2];struct Cyc_PP_Doc*_tmp97E[6];return(_tmp97E[5]=
# 1618
Cyc_PP_nest(2,((_tmp97F[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp97F[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97F,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97E[4]=
# 1617
Cyc_PP_text(((_tmp980=": ",_tag_dyneither(_tmp980,sizeof(char),3)))),((_tmp97E[3]=
# 1616
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp97E[2]=
# 1615
Cyc_PP_text(((_tmp981=" && ",_tag_dyneither(_tmp981,sizeof(char),5)))),((_tmp97E[1]=
# 1614
Cyc_Absynpp_pat2doc(c->pattern),((_tmp97E[0]=
# 1613
Cyc_PP_text(((_tmp982="case ",_tag_dyneither(_tmp982,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97E,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1621
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp983;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp983="",_tag_dyneither(_tmp983,sizeof(char),1))),cs);}
# 1625
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1629
const char*_tmp986;struct Cyc_PP_Doc*_tmp985[3];return(_tmp985[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp985[1]=Cyc_PP_text(((_tmp986=" = ",_tag_dyneither(_tmp986,sizeof(char),4)))),((_tmp985[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp985,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1632
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp987;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp987=",",_tag_dyneither(_tmp987,sizeof(char),2))),fs);}
# 1636
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1640
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp988;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp988=",",_tag_dyneither(_tmp988,sizeof(char),2))),vds);}
# 1644
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp494;struct _tuple0*_tmp495;struct Cyc_Absyn_Tqual _tmp496;void*_tmp497;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_List_List*_tmp499;struct Cyc_Absyn_Vardecl*_tmp493=vd;_tmp494=(*_tmp493).sc;_tmp495=(*_tmp493).name;_tmp496=(*_tmp493).tq;_tmp497=(*_tmp493).type;_tmp498=(*_tmp493).initializer;_tmp499=(*_tmp493).attributes;{
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
{struct Cyc_Core_Opt*_tmp995;struct Cyc_PP_Doc*_tmp994[2];struct Cyc_PP_Doc*_tmp993[2];const char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[5];s=((_tmp990[4]=
# 1674
Cyc_PP_text(((_tmp991=";",_tag_dyneither(_tmp991,sizeof(char),2)))),((_tmp990[3]=
# 1671
_tmp498 == 0?
Cyc_PP_nil_doc():(
(_tmp993[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp498),((_tmp993[0]=Cyc_PP_text(((_tmp992=" = ",_tag_dyneither(_tmp992,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp993,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp990[2]=
# 1670
Cyc_Absynpp_tqtd2doc(_tmp496,_tmp497,((_tmp995=_cycalloc(sizeof(*_tmp995)),((_tmp995->v=((_tmp994[1]=sn,((_tmp994[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp994,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp995))))),((_tmp990[1]=
# 1669
Cyc_Absynpp_scope2doc(_tmp494),((_tmp990[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1675
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1678
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4A4;struct _tuple17 _tmp4A3=*x;_tmp4A4=_tmp4A3.f2;
return Cyc_Absynpp_qvar2doc(_tmp4A4);}
# 1683
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp996[4];return(_tmp996[3]=
# 1688
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp996[2]=
# 1687
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp996[1]=
# 1686
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp996[0]=
# 1685
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1690
struct Cyc_PP_Doc*_tmp99E[2];const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[2];struct Cyc_PP_Doc*_tmp99B[12];return(_tmp99B[11]=
# 1701
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp99B[10]=
# 1700
Cyc_Absynpp_rb(),((_tmp99B[9]=
# 1699
Cyc_PP_line_doc(),((_tmp99B[8]=
# 1698
Cyc_PP_nest(2,((_tmp99C[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp99C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp99B[7]=
# 1696
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp99E[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp99E[0]=Cyc_PP_text(((_tmp99D=":",_tag_dyneither(_tmp99D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp99B[6]=
# 1695
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp99B[5]=
# 1694
Cyc_Absynpp_lb(),((_tmp99B[4]=Cyc_PP_blank_doc(),((_tmp99B[3]=
# 1693
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp99B[2]=
# 1692
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp99B[1]=
# 1691
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp99B[0]=
# 1690
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp99B,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1704
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4AB;struct _tuple0*_tmp4AC;struct Cyc_List_List*_tmp4AD;struct Cyc_Core_Opt*_tmp4AE;int _tmp4AF;struct Cyc_Absyn_Datatypedecl*_tmp4AA=dd;_tmp4AB=(*_tmp4AA).sc;_tmp4AC=(*_tmp4AA).name;_tmp4AD=(*_tmp4AA).tvs;_tmp4AE=(*_tmp4AA).fields;_tmp4AF=(*_tmp4AA).is_extensible;
if(_tmp4AE == 0){
const char*_tmp9A3;const char*_tmp9A2;struct Cyc_PP_Doc*_tmp9A1[5];return(_tmp9A1[4]=
# 1711
Cyc_Absynpp_ktvars2doc(_tmp4AD),((_tmp9A1[3]=
# 1710
_tmp4AF?Cyc_Absynpp_qvar2bolddoc(_tmp4AC): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AC),((_tmp9A1[2]=
# 1709
Cyc_PP_text(((_tmp9A2="datatype ",_tag_dyneither(_tmp9A2,sizeof(char),10)))),((_tmp9A1[1]=
# 1708
_tmp4AF?Cyc_PP_text(((_tmp9A3="@extensible ",_tag_dyneither(_tmp9A3,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A1[0]=
# 1707
Cyc_Absynpp_scope2doc(_tmp4AB),Cyc_PP_cat(_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1713
const char*_tmp9AA;const char*_tmp9A9;struct Cyc_PP_Doc*_tmp9A8[2];struct Cyc_PP_Doc*_tmp9A7[10];return(_tmp9A7[9]=
# 1721
Cyc_Absynpp_rb(),((_tmp9A7[8]=
# 1720
Cyc_PP_line_doc(),((_tmp9A7[7]=
# 1719
Cyc_PP_nest(2,((_tmp9A8[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4AE->v),((_tmp9A8[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A8,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A7[6]=
# 1718
Cyc_Absynpp_lb(),((_tmp9A7[5]=Cyc_PP_blank_doc(),((_tmp9A7[4]=
# 1717
Cyc_Absynpp_ktvars2doc(_tmp4AD),((_tmp9A7[3]=
# 1716
_tmp4AF?Cyc_Absynpp_qvar2bolddoc(_tmp4AC): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AC),((_tmp9A7[2]=
# 1715
Cyc_PP_text(((_tmp9A9="datatype ",_tag_dyneither(_tmp9A9,sizeof(char),10)))),((_tmp9A7[1]=
# 1714
_tmp4AF?Cyc_PP_text(((_tmp9AA="@extensible ",_tag_dyneither(_tmp9AA,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A7[0]=
# 1713
Cyc_Absynpp_scope2doc(_tmp4AB),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1724
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4B8;struct _tuple0*_tmp4B9;struct Cyc_Core_Opt*_tmp4BA;struct Cyc_Absyn_Enumdecl*_tmp4B7=ed;_tmp4B8=(*_tmp4B7).sc;_tmp4B9=(*_tmp4B7).name;_tmp4BA=(*_tmp4B7).fields;
if(_tmp4BA == 0){
const char*_tmp9AD;struct Cyc_PP_Doc*_tmp9AC[3];return(_tmp9AC[2]=
# 1729
Cyc_Absynpp_typedef_name2bolddoc(_tmp4B9),((_tmp9AC[1]=
# 1728
Cyc_PP_text(((_tmp9AD="enum ",_tag_dyneither(_tmp9AD,sizeof(char),6)))),((_tmp9AC[0]=
# 1727
Cyc_Absynpp_scope2doc(_tmp4B8),Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1732
const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[2];struct Cyc_PP_Doc*_tmp9B0[8];return(_tmp9B0[7]=
# 1738
Cyc_Absynpp_rb(),((_tmp9B0[6]=
# 1737
Cyc_PP_line_doc(),((_tmp9B0[5]=
# 1736
Cyc_PP_nest(2,((_tmp9B1[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4BA->v),((_tmp9B1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B0[4]=
# 1735
Cyc_Absynpp_lb(),((_tmp9B0[3]=Cyc_PP_blank_doc(),((_tmp9B0[2]=
# 1734
Cyc_Absynpp_qvar2bolddoc(_tmp4B9),((_tmp9B0[1]=
# 1733
Cyc_PP_text(((_tmp9B2="enum ",_tag_dyneither(_tmp9B2,sizeof(char),6)))),((_tmp9B0[0]=
# 1732
Cyc_Absynpp_scope2doc(_tmp4B8),Cyc_PP_cat(_tag_dyneither(_tmp9B0,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1741
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4C0=d->r;struct Cyc_Absyn_Fndecl*_tmp4C2;struct Cyc_Absyn_Aggrdecl*_tmp4C4;struct Cyc_Absyn_Vardecl*_tmp4C6;struct Cyc_Absyn_Tvar*_tmp4C8;struct Cyc_Absyn_Vardecl*_tmp4C9;int _tmp4CA;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Exp*_tmp4CD;struct Cyc_Absyn_Tvar*_tmp4CE;struct Cyc_Absyn_Vardecl*_tmp4CF;struct Cyc_Absyn_Datatypedecl*_tmp4D1;struct Cyc_Absyn_Pat*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_List_List*_tmp4D6;struct Cyc_Absyn_Enumdecl*_tmp4D8;struct Cyc_Absyn_Typedefdecl*_tmp4DA;struct _dyneither_ptr*_tmp4DC;struct Cyc_List_List*_tmp4DD;struct _tuple0*_tmp4DF;struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*_tmp4E4;struct Cyc_List_List*_tmp4E5;_LL28C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4C1=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C1->tag != 1)goto _LL28E;else{_tmp4C2=_tmp4C1->f1;}}_LL28D: {
# 1745
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9B8;struct Cyc_Absyn_FnInfo _tmp9B7;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9B6;void*t=(void*)((_tmp9B6=_cycalloc(sizeof(*_tmp9B6)),((_tmp9B6[0]=((_tmp9B8.tag=9,((_tmp9B8.f1=((_tmp9B7.tvars=_tmp4C2->tvs,((_tmp9B7.effect=_tmp4C2->effect,((_tmp9B7.ret_tqual=_tmp4C2->ret_tqual,((_tmp9B7.ret_typ=_tmp4C2->ret_type,((_tmp9B7.args=
# 1749
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C2->args),((_tmp9B7.c_varargs=_tmp4C2->c_varargs,((_tmp9B7.cyc_varargs=_tmp4C2->cyc_varargs,((_tmp9B7.rgn_po=_tmp4C2->rgn_po,((_tmp9B7.attributes=0,_tmp9B7)))))))))))))))))),_tmp9B8)))),_tmp9B6))));
# 1753
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C2->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C2->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AC:
{const char*_tmp9B9;inlinedoc=Cyc_PP_text(((_tmp9B9="inline ",_tag_dyneither(_tmp9B9,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:
{const char*_tmp9BA;inlinedoc=Cyc_PP_text(((_tmp9BA="__inline ",_tag_dyneither(_tmp9BA,sizeof(char),10))));}break;}else{
# 1761
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C2->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C2->attributes);break;}{
# 1768
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C2->name);
struct Cyc_PP_Doc*_tmp9BD[2];struct Cyc_Core_Opt*_tmp9BC;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9BC=_cycalloc(sizeof(*_tmp9BC)),((_tmp9BC->v=((_tmp9BD[1]=namedoc,((_tmp9BD[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9BD,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9BC)))));
struct Cyc_PP_Doc*_tmp9C0[2];struct Cyc_PP_Doc*_tmp9BF[5];struct Cyc_PP_Doc*bodydoc=(_tmp9BF[4]=
# 1774
Cyc_Absynpp_rb(),((_tmp9BF[3]=
# 1773
Cyc_PP_line_doc(),((_tmp9BF[2]=
# 1772
Cyc_PP_nest(2,((_tmp9C0[1]=Cyc_Absynpp_stmt2doc(_tmp4C2->body,0),((_tmp9C0[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9BF[1]=
# 1771
Cyc_Absynpp_lb(),((_tmp9BF[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1775
{struct Cyc_PP_Doc*_tmp9C1[4];s=((_tmp9C1[3]=bodydoc,((_tmp9C1[2]=tqtddoc,((_tmp9C1[1]=scopedoc,((_tmp9C1[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1777
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:
{struct Cyc_PP_Doc*_tmp9C2[2];s=((_tmp9C2[1]=s,((_tmp9C2[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9C2,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3:
 break;}
# 1782
goto _LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C3=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C3->tag != 6)goto _LL290;else{_tmp4C4=_tmp4C3->f1;}}_LL28F:
# 1785
{const char*_tmp9C5;struct Cyc_PP_Doc*_tmp9C4[2];s=((_tmp9C4[1]=Cyc_PP_text(((_tmp9C5=";",_tag_dyneither(_tmp9C5,sizeof(char),2)))),((_tmp9C4[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4C4),Cyc_PP_cat(_tag_dyneither(_tmp9C4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C5=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C5->tag != 0)goto _LL292;else{_tmp4C6=_tmp4C5->f1;}}_LL291:
# 1788
 s=Cyc_Absynpp_vardecl2doc(_tmp4C6);
goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4C7=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4C7->tag != 4)goto _LL294;else{_tmp4C8=_tmp4C7->f1;_tmp4C9=_tmp4C7->f2;_tmp4CA=_tmp4C7->f3;_tmp4CB=_tmp4C7->f4;}}_LL293:
# 1791
{const char*_tmp9D8;const char*_tmp9D7;const char*_tmp9D6;const char*_tmp9D5;struct Cyc_PP_Doc*_tmp9D4[3];const char*_tmp9D3;const char*_tmp9D2;const char*_tmp9D1;struct Cyc_PP_Doc*_tmp9D0[8];s=((_tmp9D0[7]=
# 1799
Cyc_PP_text(((_tmp9D1=";",_tag_dyneither(_tmp9D1,sizeof(char),2)))),((_tmp9D0[6]=
# 1797
_tmp4CB != 0?
(_tmp9D4[2]=Cyc_PP_text(((_tmp9D3=")",_tag_dyneither(_tmp9D3,sizeof(char),2)))),((_tmp9D4[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4CB),((_tmp9D4[0]=Cyc_PP_text(((_tmp9D2=" = open(",_tag_dyneither(_tmp9D2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9D4,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9D0[5]=
# 1796
_tmp4CA?Cyc_PP_text(((_tmp9D5=" @resetable",_tag_dyneither(_tmp9D5,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9D0[4]=
# 1795
Cyc_Absynpp_qvar2doc(_tmp4C9->name),((_tmp9D0[3]=
# 1794
Cyc_PP_text(((_tmp9D6=">",_tag_dyneither(_tmp9D6,sizeof(char),2)))),((_tmp9D0[2]=
# 1793
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C8)),((_tmp9D0[1]=
# 1792
Cyc_PP_text(((_tmp9D7="<",_tag_dyneither(_tmp9D7,sizeof(char),2)))),((_tmp9D0[0]=
# 1791
Cyc_PP_text(((_tmp9D8="region",_tag_dyneither(_tmp9D8,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9D0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1800
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4CC=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4CC->tag != 5)goto _LL296;else{_tmp4CD=_tmp4CC->f1;_tmp4CE=_tmp4CC->f2;_tmp4CF=_tmp4CC->f3;}}_LL295:
# 1802
{const char*_tmp9E3;const char*_tmp9E2;const char*_tmp9E1;const char*_tmp9E0;const char*_tmp9DF;struct Cyc_PP_Doc*_tmp9DE[8];s=((_tmp9DE[7]=
Cyc_PP_text(((_tmp9DF=";",_tag_dyneither(_tmp9DF,sizeof(char),2)))),((_tmp9DE[6]=Cyc_Absynpp_exp2doc(_tmp4CD),((_tmp9DE[5]=Cyc_PP_text(((_tmp9E0=" = ",_tag_dyneither(_tmp9E0,sizeof(char),4)))),((_tmp9DE[4]=Cyc_Absynpp_qvar2doc(_tmp4CF->name),((_tmp9DE[3]=
# 1802
Cyc_PP_text(((_tmp9E1="> ",_tag_dyneither(_tmp9E1,sizeof(char),3)))),((_tmp9DE[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CE)),((_tmp9DE[1]=Cyc_PP_text(((_tmp9E2=" <",_tag_dyneither(_tmp9E2,sizeof(char),3)))),((_tmp9DE[0]=Cyc_PP_text(((_tmp9E3="alias",_tag_dyneither(_tmp9E3,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9DE,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1804
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4D0=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D0->tag != 7)goto _LL298;else{_tmp4D1=_tmp4D0->f1;}}_LL297:
# 1806
{const char*_tmp9E6;struct Cyc_PP_Doc*_tmp9E5[2];s=((_tmp9E5[1]=Cyc_PP_text(((_tmp9E6=";",_tag_dyneither(_tmp9E6,sizeof(char),2)))),((_tmp9E5[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4D1),Cyc_PP_cat(_tag_dyneither(_tmp9E5,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D2=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D2->tag != 2)goto _LL29A;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f3;}}_LL299:
# 1809
{const char*_tmp9ED;const char*_tmp9EC;const char*_tmp9EB;struct Cyc_PP_Doc*_tmp9EA[5];s=((_tmp9EA[4]=
# 1813
Cyc_PP_text(((_tmp9EB=";",_tag_dyneither(_tmp9EB,sizeof(char),2)))),((_tmp9EA[3]=
# 1812
Cyc_Absynpp_exp2doc(_tmp4D4),((_tmp9EA[2]=
# 1811
Cyc_PP_text(((_tmp9EC=" = ",_tag_dyneither(_tmp9EC,sizeof(char),4)))),((_tmp9EA[1]=
# 1810
Cyc_Absynpp_pat2doc(_tmp4D3),((_tmp9EA[0]=
# 1809
Cyc_PP_text(((_tmp9ED="let ",_tag_dyneither(_tmp9ED,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9EA,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1814
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4D5=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D5->tag != 3)goto _LL29C;else{_tmp4D6=_tmp4D5->f1;}}_LL29B:
# 1816
{const char*_tmp9F2;const char*_tmp9F1;struct Cyc_PP_Doc*_tmp9F0[3];s=((_tmp9F0[2]=Cyc_PP_text(((_tmp9F1=";",_tag_dyneither(_tmp9F1,sizeof(char),2)))),((_tmp9F0[1]=Cyc_Absynpp_ids2doc(_tmp4D6),((_tmp9F0[0]=Cyc_PP_text(((_tmp9F2="let ",_tag_dyneither(_tmp9F2,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4D7=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D7->tag != 8)goto _LL29E;else{_tmp4D8=_tmp4D7->f1;}}_LL29D:
# 1819
{const char*_tmp9F5;struct Cyc_PP_Doc*_tmp9F4[2];s=((_tmp9F4[1]=Cyc_PP_text(((_tmp9F5=";",_tag_dyneither(_tmp9F5,sizeof(char),2)))),((_tmp9F4[0]=Cyc_Absynpp_enumdecl2doc(_tmp4D8),Cyc_PP_cat(_tag_dyneither(_tmp9F4,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4D9=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4D9->tag != 9)goto _LL2A0;else{_tmp4DA=_tmp4D9->f1;}}_LL29F: {
# 1822
void*t;
if(_tmp4DA->defn != 0)
t=(void*)_check_null(_tmp4DA->defn);else{
# 1826
t=Cyc_Absyn_new_evar(_tmp4DA->kind,0);}
{const char*_tmp9FF;struct Cyc_Core_Opt*_tmp9FE;struct Cyc_PP_Doc*_tmp9FD[2];const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[4];s=((_tmp9FB[3]=
# 1834
Cyc_PP_text(((_tmp9FC=";",_tag_dyneither(_tmp9FC,sizeof(char),2)))),((_tmp9FB[2]=
# 1833
Cyc_Absynpp_atts2doc(_tmp4DA->atts),((_tmp9FB[1]=
# 1828
Cyc_Absynpp_tqtd2doc(_tmp4DA->tq,t,(
# 1830
(_tmp9FE=_cycalloc(sizeof(*_tmp9FE)),((_tmp9FE->v=((_tmp9FD[1]=
Cyc_Absynpp_tvars2doc(_tmp4DA->tvs),((_tmp9FD[0]=
# 1830
Cyc_Absynpp_typedef_name2bolddoc(_tmp4DA->name),Cyc_PP_cat(_tag_dyneither(_tmp9FD,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9FE))))),((_tmp9FB[0]=
# 1827
Cyc_PP_text(((_tmp9FF="typedef ",_tag_dyneither(_tmp9FF,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1836
goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4DB=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4DB->tag != 10)goto _LL2A2;else{_tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;}}_LL2A1:
# 1838
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DC);
{const char*_tmpA04;const char*_tmpA03;struct Cyc_PP_Doc*_tmpA02[8];s=((_tmpA02[7]=
# 1845
Cyc_Absynpp_rb(),((_tmpA02[6]=
# 1844
Cyc_PP_line_doc(),((_tmpA02[5]=
# 1843
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA03="",_tag_dyneither(_tmpA03,sizeof(char),1))),_tmp4DD),((_tmpA02[4]=
# 1842
Cyc_PP_line_doc(),((_tmpA02[3]=
# 1841
Cyc_Absynpp_lb(),((_tmpA02[2]=Cyc_PP_blank_doc(),((_tmpA02[1]=
# 1840
Cyc_PP_textptr(_tmp4DC),((_tmpA02[0]=
# 1839
Cyc_PP_text(((_tmpA04="namespace ",_tag_dyneither(_tmpA04,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1846
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4DE=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4DE->tag != 11)goto _LL2A4;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL2A3:
# 1849
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA09;const char*_tmpA08;struct Cyc_PP_Doc*_tmpA07[8];s=((_tmpA07[7]=
# 1856
Cyc_Absynpp_rb(),((_tmpA07[6]=
# 1855
Cyc_PP_line_doc(),((_tmpA07[5]=
# 1854
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA08="",_tag_dyneither(_tmpA08,sizeof(char),1))),_tmp4E0),((_tmpA07[4]=
# 1853
Cyc_PP_line_doc(),((_tmpA07[3]=
# 1852
Cyc_Absynpp_lb(),((_tmpA07[2]=Cyc_PP_blank_doc(),((_tmpA07[1]=
# 1851
Cyc_Absynpp_qvar2doc(_tmp4DF),((_tmpA07[0]=
# 1850
Cyc_PP_text(((_tmpA09="using ",_tag_dyneither(_tmpA09,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA07,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1858
const char*_tmpA14;const char*_tmpA13;const char*_tmpA12;const char*_tmpA11;const char*_tmpA10;struct Cyc_PP_Doc*_tmpA0F[11];s=((_tmpA0F[10]=
# 1868
Cyc_PP_text(((_tmpA10=" */",_tag_dyneither(_tmpA10,sizeof(char),4)))),((_tmpA0F[9]=
# 1867
Cyc_Absynpp_rb(),((_tmpA0F[8]=
# 1866
Cyc_PP_text(((_tmpA11="/* ",_tag_dyneither(_tmpA11,sizeof(char),4)))),((_tmpA0F[7]=
# 1865
Cyc_PP_line_doc(),((_tmpA0F[6]=
# 1864
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA12="",_tag_dyneither(_tmpA12,sizeof(char),1))),_tmp4E0),((_tmpA0F[5]=
# 1863
Cyc_PP_line_doc(),((_tmpA0F[4]=
# 1862
Cyc_PP_text(((_tmpA13=" */",_tag_dyneither(_tmpA13,sizeof(char),4)))),((_tmpA0F[3]=
# 1861
Cyc_Absynpp_lb(),((_tmpA0F[2]=
# 1860
Cyc_PP_blank_doc(),((_tmpA0F[1]=
# 1859
Cyc_Absynpp_qvar2doc(_tmp4DF),((_tmpA0F[0]=
# 1858
Cyc_PP_text(((_tmpA14="/* using ",_tag_dyneither(_tmpA14,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA0F,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1869
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4E1=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E1->tag != 12)goto _LL2A6;else{_tmp4E2=_tmp4E1->f1;}}_LL2A5:
# 1871
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA19;const char*_tmpA18;struct Cyc_PP_Doc*_tmpA17[6];s=((_tmpA17[5]=
# 1877
Cyc_Absynpp_rb(),((_tmpA17[4]=
# 1876
Cyc_PP_line_doc(),((_tmpA17[3]=
# 1875
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA18="",_tag_dyneither(_tmpA18,sizeof(char),1))),_tmp4E2),((_tmpA17[2]=
# 1874
Cyc_PP_line_doc(),((_tmpA17[1]=
# 1873
Cyc_Absynpp_lb(),((_tmpA17[0]=
# 1872
Cyc_PP_text(((_tmpA19="extern \"C\" ",_tag_dyneither(_tmpA19,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA17,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1879
const char*_tmpA24;const char*_tmpA23;const char*_tmpA22;const char*_tmpA21;const char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[9];s=((_tmpA1F[8]=
# 1887
Cyc_PP_text(((_tmpA20=" */",_tag_dyneither(_tmpA20,sizeof(char),4)))),((_tmpA1F[7]=
# 1886
Cyc_Absynpp_rb(),((_tmpA1F[6]=
# 1885
Cyc_PP_text(((_tmpA21="/* ",_tag_dyneither(_tmpA21,sizeof(char),4)))),((_tmpA1F[5]=
# 1884
Cyc_PP_line_doc(),((_tmpA1F[4]=
# 1883
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA22="",_tag_dyneither(_tmpA22,sizeof(char),1))),_tmp4E2),((_tmpA1F[3]=
# 1882
Cyc_PP_line_doc(),((_tmpA1F[2]=
# 1881
Cyc_PP_text(((_tmpA23=" */",_tag_dyneither(_tmpA23,sizeof(char),4)))),((_tmpA1F[1]=
# 1880
Cyc_Absynpp_lb(),((_tmpA1F[0]=
# 1879
Cyc_PP_text(((_tmpA24="/* extern \"C\" ",_tag_dyneither(_tmpA24,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1888
goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E3=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E3->tag != 13)goto _LL2A8;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;}}_LL2A7:
# 1890
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4E5 != 0){
const char*_tmpA29;const char*_tmpA28;struct Cyc_PP_Doc*_tmpA27[7];exs_doc=((_tmpA27[6]=
# 1895
Cyc_Absynpp_rb(),((_tmpA27[5]=Cyc_PP_line_doc(),((_tmpA27[4]=
# 1894
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA28=",",_tag_dyneither(_tmpA28,sizeof(char),2))),_tmp4E5),((_tmpA27[3]=Cyc_PP_line_doc(),((_tmpA27[2]=
# 1893
Cyc_Absynpp_lb(),((_tmpA27[1]=Cyc_PP_text(((_tmpA29=" export ",_tag_dyneither(_tmpA29,sizeof(char),9)))),((_tmpA27[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA27,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1897
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA2E;const char*_tmpA2D;struct Cyc_PP_Doc*_tmpA2C[6];s=((_tmpA2C[5]=exs_doc,((_tmpA2C[4]=
# 1902
Cyc_PP_line_doc(),((_tmpA2C[3]=
# 1901
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA2D="",_tag_dyneither(_tmpA2D,sizeof(char),1))),_tmp4E4),((_tmpA2C[2]=
# 1900
Cyc_PP_line_doc(),((_tmpA2C[1]=
# 1899
Cyc_Absynpp_lb(),((_tmpA2C[0]=
# 1898
Cyc_PP_text(((_tmpA2E="extern \"C include\" ",_tag_dyneither(_tmpA2E,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA2C,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1905
const char*_tmpA39;const char*_tmpA38;const char*_tmpA37;const char*_tmpA36;const char*_tmpA35;struct Cyc_PP_Doc*_tmpA34[9];s=((_tmpA34[8]=
# 1913
Cyc_PP_text(((_tmpA35=" */",_tag_dyneither(_tmpA35,sizeof(char),4)))),((_tmpA34[7]=
# 1912
Cyc_Absynpp_rb(),((_tmpA34[6]=
# 1911
Cyc_PP_text(((_tmpA36="/* ",_tag_dyneither(_tmpA36,sizeof(char),4)))),((_tmpA34[5]=
# 1910
Cyc_PP_line_doc(),((_tmpA34[4]=
# 1909
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA37="",_tag_dyneither(_tmpA37,sizeof(char),1))),_tmp4E4),((_tmpA34[3]=
# 1908
Cyc_PP_line_doc(),((_tmpA34[2]=
# 1907
Cyc_PP_text(((_tmpA38=" */",_tag_dyneither(_tmpA38,sizeof(char),4)))),((_tmpA34[1]=
# 1906
Cyc_Absynpp_lb(),((_tmpA34[0]=
# 1905
Cyc_PP_text(((_tmpA39="/* extern \"C include\" ",_tag_dyneither(_tmpA39,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA34,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1914
goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4E6=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E6->tag != 14)goto _LL2AA;}_LL2A9:
# 1916
{const char*_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[2];s=((_tmpA3B[1]=Cyc_Absynpp_lb(),((_tmpA3B[0]=Cyc_PP_text(((_tmpA3C="__cyclone_port_on__;",_tag_dyneither(_tmpA3C,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4E7=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4C0;if(_tmp4E7->tag != 15)goto _LL28B;}_LL2AB:
# 1919
{const char*_tmpA3F;struct Cyc_PP_Doc*_tmpA3E[2];s=((_tmpA3E[1]=Cyc_Absynpp_lb(),((_tmpA3E[0]=Cyc_PP_text(((_tmpA3F="__cyclone_port_off__;",_tag_dyneither(_tmpA3F,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL28B:;}
# 1923
return s;}
# 1926
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2B5: {
const char*_tmpA40;return Cyc_PP_text(((_tmpA40="static ",_tag_dyneither(_tmpA40,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B6:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2B7: {
const char*_tmpA41;return Cyc_PP_text(((_tmpA41="extern ",_tag_dyneither(_tmpA41,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {
const char*_tmpA42;return Cyc_PP_text(((_tmpA42="extern \"C\" ",_tag_dyneither(_tmpA42,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2B9: {
const char*_tmpA43;return Cyc_PP_text(((_tmpA43="abstract ",_tag_dyneither(_tmpA43,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BA: {
const char*_tmpA44;return Cyc_PP_text(((_tmpA44="register ",_tag_dyneither(_tmpA44,sizeof(char),10))));}}}
# 1939
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp53E=t;struct Cyc_Absyn_Tvar*_tmp540;struct Cyc_List_List*_tmp542;_LL2BD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp53F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53E;if(_tmp53F->tag != 2)goto _LL2BF;else{_tmp540=_tmp53F->f1;}}_LL2BE:
 return Cyc_Tcutil_is_temp_tvar(_tmp540);_LL2BF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp541=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp53E;if(_tmp541->tag != 24)goto _LL2C1;else{_tmp542=_tmp541->f1;}}_LL2C0:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp542);_LL2C1:;_LL2C2:
 return 0;_LL2BC:;}
# 1951
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp543=t;struct Cyc_Absyn_Typedefdecl*_tmp547;void*_tmp548;_LL2C4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp543;if(_tmp544->tag != 12)goto _LL2C6;}_LL2C5:
 return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp545=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp543;if(_tmp545->tag != 14)goto _LL2C8;}_LL2C7:
 return 1;_LL2C8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543;if(_tmp546->tag != 17)goto _LL2CA;else{_tmp547=_tmp546->f3;_tmp548=(void*)_tmp546->f4;}}if(!(_tmp548 != 0))goto _LL2CA;_LL2C9:
# 1958
 return Cyc_Absynpp_is_anon_aggrtype((void*)_tmp548);_LL2CA:;_LL2CB:
 return 0;_LL2C3:;}
# 1965
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1968
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA45;struct _tuple16 _tmp54A=(_tmpA45.f1=(void*)tms->hd,((_tmpA45.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA45)));_LL2CD:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp54B=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp54A.f1;if(_tmp54B->tag != 2)goto _LL2CF;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp54A.f2;if(_tmp54C->tag != 3)goto _LL2CF;};_LL2CE: {
# 1971
struct Cyc_List_List*_tmpA48;struct Cyc_List_List*_tmpA47;return(_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47->hd=(void*)tms->hd,((_tmpA47->tl=((_tmpA48=_region_malloc(r,sizeof(*_tmpA48)),((_tmpA48->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA48->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA48)))))),_tmpA47)))));}_LL2CF:;_LL2D0: {
struct Cyc_List_List*_tmpA49;return(_tmpA49=_region_malloc(r,sizeof(*_tmpA49)),((_tmpA49->hd=atts,((_tmpA49->tl=tms,_tmpA49)))));}_LL2CC:;}else{
# 1974
struct Cyc_List_List*_tmpA4A;return(_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->hd=atts,((_tmpA4A->tl=tms,_tmpA4A)))));}}
# 1979
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1981
void*_tmp551=t;void*_tmp553;struct Cyc_Absyn_Tqual _tmp554;struct Cyc_Absyn_Exp*_tmp555;union Cyc_Absyn_Constraint*_tmp556;unsigned int _tmp557;void*_tmp559;struct Cyc_Absyn_Tqual _tmp55A;struct Cyc_Absyn_PtrAtts _tmp55B;struct Cyc_List_List*_tmp55D;void*_tmp55E;struct Cyc_Absyn_Tqual _tmp55F;void*_tmp560;struct Cyc_List_List*_tmp561;int _tmp562;struct Cyc_Absyn_VarargInfo*_tmp563;struct Cyc_List_List*_tmp564;struct Cyc_List_List*_tmp565;struct Cyc_Core_Opt*_tmp567;void*_tmp568;int _tmp569;struct _tuple0*_tmp56B;struct Cyc_List_List*_tmp56C;struct Cyc_Absyn_Typedefdecl*_tmp56D;void*_tmp56E;_LL2D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp552=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp551;if(_tmp552->tag != 8)goto _LL2D4;else{_tmp553=(_tmp552->f1).elt_type;_tmp554=(_tmp552->f1).tq;_tmp555=(_tmp552->f1).num_elts;_tmp556=(_tmp552->f1).zero_term;_tmp557=(_tmp552->f1).zt_loc;}}_LL2D3: {
# 1984
struct Cyc_Absyn_Tqual _tmp570;void*_tmp571;struct Cyc_List_List*_tmp572;struct _tuple12 _tmp56F=Cyc_Absynpp_to_tms(r,_tmp554,_tmp553);_tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;_tmp572=_tmp56F.f3;{
void*tm;
if(_tmp555 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA4D;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA4C;tm=(void*)((_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4D.tag=0,((_tmpA4D.f1=_tmp556,((_tmpA4D.f2=_tmp557,_tmpA4D)))))),_tmpA4C))));}else{
# 1989
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA50;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA4F;tm=(void*)((_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=1,((_tmpA50.f1=(struct Cyc_Absyn_Exp*)_tmp555,((_tmpA50.f2=_tmp556,((_tmpA50.f3=_tmp557,_tmpA50)))))))),_tmpA4F))));}{
struct Cyc_List_List*_tmpA53;struct _tuple12 _tmpA52;return(_tmpA52.f1=_tmp570,((_tmpA52.f2=_tmp571,((_tmpA52.f3=((_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53->hd=tm,((_tmpA53->tl=_tmp572,_tmpA53)))))),_tmpA52)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp558=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp551;if(_tmp558->tag != 5)goto _LL2D6;else{_tmp559=(_tmp558->f1).elt_typ;_tmp55A=(_tmp558->f1).elt_tq;_tmp55B=(_tmp558->f1).ptr_atts;}}_LL2D5: {
# 1993
struct Cyc_Absyn_Tqual _tmp57A;void*_tmp57B;struct Cyc_List_List*_tmp57C;struct _tuple12 _tmp579=Cyc_Absynpp_to_tms(r,_tmp55A,_tmp559);_tmp57A=_tmp579.f1;_tmp57B=_tmp579.f2;_tmp57C=_tmp579.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA59;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA58;struct Cyc_List_List*_tmpA57;_tmp57C=((_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57->hd=(void*)((_tmpA59=_region_malloc(r,sizeof(*_tmpA59)),((_tmpA59[0]=((_tmpA58.tag=2,((_tmpA58.f1=_tmp55B,((_tmpA58.f2=tq,_tmpA58)))))),_tmpA59)))),((_tmpA57->tl=_tmp57C,_tmpA57))))));}{
struct _tuple12 _tmpA5A;return(_tmpA5A.f1=_tmp57A,((_tmpA5A.f2=_tmp57B,((_tmpA5A.f3=_tmp57C,_tmpA5A)))));};}_LL2D6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp551;if(_tmp55C->tag != 9)goto _LL2D8;else{_tmp55D=(_tmp55C->f1).tvars;_tmp55E=(_tmp55C->f1).effect;_tmp55F=(_tmp55C->f1).ret_tqual;_tmp560=(_tmp55C->f1).ret_typ;_tmp561=(_tmp55C->f1).args;_tmp562=(_tmp55C->f1).c_varargs;_tmp563=(_tmp55C->f1).cyc_varargs;_tmp564=(_tmp55C->f1).rgn_po;_tmp565=(_tmp55C->f1).attributes;}}_LL2D7:
# 1999
 if(!Cyc_Absynpp_print_all_tvars){
# 2003
if(_tmp55E == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp55E)){
_tmp55E=0;
_tmp55D=0;}}else{
# 2008
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2011
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp55D);}{
# 2016
struct Cyc_Absyn_Tqual _tmp582;void*_tmp583;struct Cyc_List_List*_tmp584;struct _tuple12 _tmp581=Cyc_Absynpp_to_tms(r,_tmp55F,_tmp560);_tmp582=_tmp581.f1;_tmp583=_tmp581.f2;_tmp584=_tmp581.f3;{
struct Cyc_List_List*tms=_tmp584;
# 2027 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2DE:
# 2029
 if(_tmp565 != 0){
# 2032
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA5D;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA5C;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA5C=_region_malloc(r,sizeof(*_tmpA5C)),((_tmpA5C[0]=((_tmpA5D.tag=5,((_tmpA5D.f1=0,((_tmpA5D.f2=_tmp565,_tmpA5D)))))),_tmpA5C)))),tms);}
# 2034
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA6C;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA6B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA6A;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA69;struct Cyc_List_List*_tmpA68;tms=((_tmpA68=_region_malloc(r,sizeof(*_tmpA68)),((_tmpA68->hd=(void*)((_tmpA6C=_region_malloc(r,sizeof(*_tmpA6C)),((_tmpA6C[0]=(
(_tmpA69.tag=3,((_tmpA69.f1=(void*)((void*)((_tmpA6B=_region_malloc(r,sizeof(*_tmpA6B)),((_tmpA6B[0]=((_tmpA6A.tag=1,((_tmpA6A.f1=_tmp561,((_tmpA6A.f2=_tmp562,((_tmpA6A.f3=_tmp563,((_tmpA6A.f4=(void*)_tmp55E,((_tmpA6A.f5=_tmp564,_tmpA6A)))))))))))),_tmpA6B))))),_tmpA69)))),_tmpA6C)))),((_tmpA68->tl=tms,_tmpA68))))));}
# 2038
break;case Cyc_Cyclone_Vc_c: _LL2DF:
# 2040
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA7B;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA7A;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA79;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA78;struct Cyc_List_List*_tmpA77;tms=((_tmpA77=_region_malloc(r,sizeof(*_tmpA77)),((_tmpA77->hd=(void*)((_tmpA7B=_region_malloc(r,sizeof(*_tmpA7B)),((_tmpA7B[0]=(
(_tmpA78.tag=3,((_tmpA78.f1=(void*)((void*)((_tmpA7A=_region_malloc(r,sizeof(*_tmpA7A)),((_tmpA7A[0]=((_tmpA79.tag=1,((_tmpA79.f1=_tmp561,((_tmpA79.f2=_tmp562,((_tmpA79.f3=_tmp563,((_tmpA79.f4=(void*)_tmp55E,((_tmpA79.f5=_tmp564,_tmpA79)))))))))))),_tmpA7A))))),_tmpA78)))),_tmpA7B)))),((_tmpA77->tl=tms,_tmpA77))))));}
# 2044
for(0;_tmp565 != 0;_tmp565=_tmp565->tl){
void*_tmp591=(void*)_tmp565->hd;_LL2E2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp592=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp591;if(_tmp592->tag != 1)goto _LL2E4;}_LL2E3:
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp593=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp591;if(_tmp593->tag != 2)goto _LL2E6;}_LL2E5:
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp594=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp591;if(_tmp594->tag != 3)goto _LL2E8;}_LL2E7:
# 2049
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA85;struct Cyc_List_List*_tmpA84;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA83;struct Cyc_List_List*_tmpA82;tms=((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82->hd=(void*)((_tmpA85=_region_malloc(r,sizeof(*_tmpA85)),((_tmpA85[0]=((_tmpA83.tag=5,((_tmpA83.f1=0,((_tmpA83.f2=((_tmpA84=_cycalloc(sizeof(*_tmpA84)),((_tmpA84->hd=(void*)_tmp565->hd,((_tmpA84->tl=0,_tmpA84)))))),_tmpA83)))))),_tmpA85)))),((_tmpA82->tl=tms,_tmpA82))))));}
goto AfterAtts;_LL2E8:;_LL2E9:
 goto _LL2E1;_LL2E1:;}
# 2053
break;}
# 2056
AfterAtts:
 if(_tmp55D != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA8B;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA8A;struct Cyc_List_List*_tmpA89;tms=((_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->hd=(void*)((_tmpA8B=_region_malloc(r,sizeof(*_tmpA8B)),((_tmpA8B[0]=((_tmpA8A.tag=4,((_tmpA8A.f1=_tmp55D,((_tmpA8A.f2=0,((_tmpA8A.f3=1,_tmpA8A)))))))),_tmpA8B)))),((_tmpA89->tl=tms,_tmpA89))))));}{
struct _tuple12 _tmpA8C;return(_tmpA8C.f1=_tmp582,((_tmpA8C.f2=_tmp583,((_tmpA8C.f3=tms,_tmpA8C)))));};};};_LL2D8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp551;if(_tmp566->tag != 1)goto _LL2DA;else{_tmp567=_tmp566->f1;_tmp568=(void*)_tmp566->f2;_tmp569=_tmp566->f3;}}_LL2D9:
# 2062
 if(_tmp568 == 0){
struct _tuple12 _tmpA8D;return(_tmpA8D.f1=tq,((_tmpA8D.f2=t,((_tmpA8D.f3=0,_tmpA8D)))));}else{
# 2065
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp568);}_LL2DA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp551;if(_tmp56A->tag != 17)goto _LL2DC;else{_tmp56B=_tmp56A->f1;_tmp56C=_tmp56A->f2;_tmp56D=_tmp56A->f3;_tmp56E=(void*)_tmp56A->f4;}}_LL2DB:
# 2070
 if((_tmp56E == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype((void*)_tmp56E)){
struct _tuple12 _tmpA8E;return(_tmpA8E.f1=tq,((_tmpA8E.f2=t,((_tmpA8E.f3=0,_tmpA8E)))));}else{
# 2073
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp56E);}_LL2DC:;_LL2DD: {
# 2078
struct _tuple12 _tmpA8F;return(_tmpA8F.f1=tq,((_tmpA8F.f2=t,((_tmpA8F.f3=0,_tmpA8F)))));}_LL2D1:;}
# 2082
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2084
void*_tmp5A0=t;void*_tmp5A2;void*_tmp5A4;_LL2EB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A0;if(_tmp5A1->tag != 1)goto _LL2ED;else{_tmp5A2=(void*)_tmp5A1->f2;}}if(!(_tmp5A2 != 0))goto _LL2ED;_LL2EC:
 return Cyc_Absynpp_is_char_ptr((void*)_tmp5A2);_LL2ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A0;if(_tmp5A3->tag != 5)goto _LL2EF;else{_tmp5A4=(_tmp5A3->f1).elt_typ;}}_LL2EE:
# 2087
 L: {
void*_tmp5A5=_tmp5A4;void*_tmp5A7;void*_tmp5A9;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A5;if(_tmp5A6->tag != 1)goto _LL2F4;else{_tmp5A7=(void*)_tmp5A6->f2;}}if(!(_tmp5A7 != 0))goto _LL2F4;_LL2F3:
 _tmp5A4=(void*)_tmp5A7;goto L;_LL2F4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A5;if(_tmp5A8->tag != 17)goto _LL2F6;else{_tmp5A9=(void*)_tmp5A8->f4;}}if(!(_tmp5A9 != 0))goto _LL2F6;_LL2F5:
 _tmp5A4=(void*)_tmp5A9;goto L;_LL2F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A5;if(_tmp5AA->tag != 6)goto _LL2F8;else{if(_tmp5AA->f2 != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
 return 1;_LL2F8:;_LL2F9:
 return 0;_LL2F1:;}_LL2EF:;_LL2F0:
# 2094
 return 0;_LL2EA:;}
# 2098
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5AB=_new_region("temp");struct _RegionHandle*temp=& _tmp5AB;_push_region(temp);
{struct Cyc_Absyn_Tqual _tmp5AD;void*_tmp5AE;struct Cyc_List_List*_tmp5AF;struct _tuple12 _tmp5AC=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5AD=_tmp5AC.f1;_tmp5AE=_tmp5AC.f2;_tmp5AF=_tmp5AC.f3;
_tmp5AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5AF);
if(_tmp5AF == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA90[2];struct Cyc_PP_Doc*_tmp5B1=(_tmpA90[1]=Cyc_Absynpp_ntyp2doc(_tmp5AE),((_tmpA90[0]=Cyc_Absynpp_tqual2doc(_tmp5AD),Cyc_PP_cat(_tag_dyneither(_tmpA90,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5B1;}else{
# 2105
const char*_tmpA93;struct Cyc_PP_Doc*_tmpA92[4];struct Cyc_PP_Doc*_tmp5B4=
(_tmpA92[3]=
# 2109
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5AF),((_tmpA92[2]=
# 2108
Cyc_PP_text(((_tmpA93=" ",_tag_dyneither(_tmpA93,sizeof(char),2)))),((_tmpA92[1]=
# 2107
Cyc_Absynpp_ntyp2doc(_tmp5AE),((_tmpA92[0]=
# 2106
Cyc_Absynpp_tqual2doc(_tmp5AD),Cyc_PP_cat(_tag_dyneither(_tmpA92,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B4;}}
# 2100
;_pop_region(temp);}
# 2113
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA:
# 2116
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA9A;const char*_tmpA99;const char*_tmpA98;struct Cyc_PP_Doc*_tmpA97[5];return(_tmpA97[4]=
# 2119
Cyc_PP_text(((_tmpA98=";",_tag_dyneither(_tmpA98,sizeof(char),2)))),((_tmpA97[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA97[2]=
# 2118
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA97[1]=Cyc_PP_text(((_tmpA99=":",_tag_dyneither(_tmpA99,sizeof(char),2)))),((_tmpA97[0]=
# 2117
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->v=Cyc_PP_textptr(f->name),_tmpA9A))))),Cyc_PP_cat(_tag_dyneither(_tmpA97,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2121
struct Cyc_Core_Opt*_tmpA9F;const char*_tmpA9E;struct Cyc_PP_Doc*_tmpA9D[3];return(_tmpA9D[2]=
Cyc_PP_text(((_tmpA9E=";",_tag_dyneither(_tmpA9E,sizeof(char),2)))),((_tmpA9D[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA9D[0]=
# 2121
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9F=_cycalloc(sizeof(*_tmpA9F)),((_tmpA9F->v=Cyc_PP_textptr(f->name),_tmpA9F))))),Cyc_PP_cat(_tag_dyneither(_tmpA9D,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FB:
# 2124
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAA6;const char*_tmpAA5;const char*_tmpAA4;struct Cyc_PP_Doc*_tmpAA3[5];return(_tmpAA3[4]=
# 2127
Cyc_PP_text(((_tmpAA4=";",_tag_dyneither(_tmpAA4,sizeof(char),2)))),((_tmpAA3[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAA3[2]=Cyc_PP_text(((_tmpAA5=":",_tag_dyneither(_tmpAA5,sizeof(char),2)))),((_tmpAA3[1]=
# 2126
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->v=Cyc_PP_textptr(f->name),_tmpAA6))))),((_tmpAA3[0]=
# 2125
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA3,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2129
struct Cyc_Core_Opt*_tmpAAB;const char*_tmpAAA;struct Cyc_PP_Doc*_tmpAA9[3];return(_tmpAA9[2]=
Cyc_PP_text(((_tmpAAA=";",_tag_dyneither(_tmpAAA,sizeof(char),2)))),((_tmpAA9[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAAB=_cycalloc(sizeof(*_tmpAAB)),((_tmpAAB->v=Cyc_PP_textptr(f->name),_tmpAAB))))),((_tmpAA9[0]=
# 2129
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA9,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2135
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAAC;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAAC="",_tag_dyneither(_tmpAAC,sizeof(char),1))),fields);}
# 2139
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAAD[3];return(_tmpAAD[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAAD[1]=
# 2140
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAAD[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAAD,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2144
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAAE;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAAE=",",_tag_dyneither(_tmpAAE,sizeof(char),2))),fields);}
# 2153
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpAB1;void*_tmpAB0;(_tmpAB0=0,Cyc_fprintf(f,((_tmpAB1="\n",_tag_dyneither(_tmpAB1,sizeof(char),2))),_tag_dyneither(_tmpAB0,sizeof(void*),0)));};}}
# 2160
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpAB2;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAB2="",_tag_dyneither(_tmpAB2,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
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
