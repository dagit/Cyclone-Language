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
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 258
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 387
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
# 66 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 315 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 317
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
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
struct Cyc_List_List*_tmp5D0;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5D0=_cycalloc(sizeof(*_tmp5D0)),((_tmp5D0->hd=v,((_tmp5D0->tl=0,_tmp5D0)))))));}
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
const char*_tmp5D1;return(_tmp5D1="\\a",_tag_dyneither(_tmp5D1,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5D2;return(_tmp5D2="\\b",_tag_dyneither(_tmp5D2,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5D3;return(_tmp5D3="\\f",_tag_dyneither(_tmp5D3,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5D4;return(_tmp5D4="\\n",_tag_dyneither(_tmp5D4,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5D5;return(_tmp5D5="\\r",_tag_dyneither(_tmp5D5,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5D6;return(_tmp5D6="\\t",_tag_dyneither(_tmp5D6,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5D7;return(_tmp5D7="\\v",_tag_dyneither(_tmp5D7,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5D8;return(_tmp5D8="\\\\",_tag_dyneither(_tmp5D8,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5D9;return(_tmp5D9="\"",_tag_dyneither(_tmp5D9,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5DA;return(_tmp5DA="\\'",_tag_dyneither(_tmp5DA,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5DD;char _tmp5DC;struct _dyneither_ptr _tmp5DB;(_tmp5DB=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5DC=*((char*)_check_dyneither_subscript(_tmp5DB,sizeof(char),0)),((_tmp5DD=c,((_get_dyneither_size(_tmp5DB,sizeof(char))== 1  && (_tmp5DC == '\000'  && _tmp5DD != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DB.curr)=_tmp5DD)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp5E0;char _tmp5DF;struct _dyneither_ptr _tmp5DE;(_tmp5DE=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DF=*((char*)_check_dyneither_subscript(_tmp5DE,sizeof(char),0)),((_tmp5E0='\\',((_get_dyneither_size(_tmp5DE,sizeof(char))== 1  && (_tmp5DF == '\000'  && _tmp5E0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DE.curr)=_tmp5E0)))))));}
{char _tmp5E3;char _tmp5E2;struct _dyneither_ptr _tmp5E1;(_tmp5E1=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E2=*((char*)_check_dyneither_subscript(_tmp5E1,sizeof(char),0)),((_tmp5E3=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5E1,sizeof(char))== 1  && (_tmp5E2 == '\000'  && _tmp5E3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E1.curr)=_tmp5E3)))))));}
{char _tmp5E6;char _tmp5E5;struct _dyneither_ptr _tmp5E4;(_tmp5E4=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E5=*((char*)_check_dyneither_subscript(_tmp5E4,sizeof(char),0)),((_tmp5E6=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5E4,sizeof(char))== 1  && (_tmp5E5 == '\000'  && _tmp5E6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E4.curr)=_tmp5E6)))))));}
{char _tmp5E9;char _tmp5E8;struct _dyneither_ptr _tmp5E7;(_tmp5E7=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E8=*((char*)_check_dyneither_subscript(_tmp5E7,sizeof(char),0)),((_tmp5E9=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp5E7,sizeof(char))== 1  && (_tmp5E8 == '\000'  && _tmp5E9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E7.curr)=_tmp5E9)))))));}
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
char _stmttmp0=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1D=_stmttmp0;char _tmp1E;_LLD: if(_tmp1D != '\a')goto _LLF;_LLE:
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
char _stmttmp1=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1F=_stmttmp1;char _tmp20;_LL22: if(_tmp1F != '\a')goto _LL24;_LL23:
{char _tmp5EC;char _tmp5EB;struct _dyneither_ptr _tmp5EA;(_tmp5EA=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EB=*((char*)_check_dyneither_subscript(_tmp5EA,sizeof(char),0)),((_tmp5EC='\\',((_get_dyneither_size(_tmp5EA,sizeof(char))== 1  && (_tmp5EB == '\000'  && _tmp5EC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EA.curr)=_tmp5EC)))))));}{char _tmp5EF;char _tmp5EE;struct _dyneither_ptr _tmp5ED;(_tmp5ED=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EE=*((char*)_check_dyneither_subscript(_tmp5ED,sizeof(char),0)),((_tmp5EF='a',((_get_dyneither_size(_tmp5ED,sizeof(char))== 1  && (_tmp5EE == '\000'  && _tmp5EF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5ED.curr)=_tmp5EF)))))));}goto _LL21;_LL24: if(_tmp1F != '\b')goto _LL26;_LL25:
{char _tmp5F2;char _tmp5F1;struct _dyneither_ptr _tmp5F0;(_tmp5F0=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F1=*((char*)_check_dyneither_subscript(_tmp5F0,sizeof(char),0)),((_tmp5F2='\\',((_get_dyneither_size(_tmp5F0,sizeof(char))== 1  && (_tmp5F1 == '\000'  && _tmp5F2 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F0.curr)=_tmp5F2)))))));}{char _tmp5F5;char _tmp5F4;struct _dyneither_ptr _tmp5F3;(_tmp5F3=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F4=*((char*)_check_dyneither_subscript(_tmp5F3,sizeof(char),0)),((_tmp5F5='b',((_get_dyneither_size(_tmp5F3,sizeof(char))== 1  && (_tmp5F4 == '\000'  && _tmp5F5 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F3.curr)=_tmp5F5)))))));}goto _LL21;_LL26: if(_tmp1F != '\f')goto _LL28;_LL27:
{char _tmp5F8;char _tmp5F7;struct _dyneither_ptr _tmp5F6;(_tmp5F6=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F7=*((char*)_check_dyneither_subscript(_tmp5F6,sizeof(char),0)),((_tmp5F8='\\',((_get_dyneither_size(_tmp5F6,sizeof(char))== 1  && (_tmp5F7 == '\000'  && _tmp5F8 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F6.curr)=_tmp5F8)))))));}{char _tmp5FB;char _tmp5FA;struct _dyneither_ptr _tmp5F9;(_tmp5F9=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FA=*((char*)_check_dyneither_subscript(_tmp5F9,sizeof(char),0)),((_tmp5FB='f',((_get_dyneither_size(_tmp5F9,sizeof(char))== 1  && (_tmp5FA == '\000'  && _tmp5FB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F9.curr)=_tmp5FB)))))));}goto _LL21;_LL28: if(_tmp1F != '\n')goto _LL2A;_LL29:
{char _tmp5FE;char _tmp5FD;struct _dyneither_ptr _tmp5FC;(_tmp5FC=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FD=*((char*)_check_dyneither_subscript(_tmp5FC,sizeof(char),0)),((_tmp5FE='\\',((_get_dyneither_size(_tmp5FC,sizeof(char))== 1  && (_tmp5FD == '\000'  && _tmp5FE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FC.curr)=_tmp5FE)))))));}{char _tmp601;char _tmp600;struct _dyneither_ptr _tmp5FF;(_tmp5FF=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp600=*((char*)_check_dyneither_subscript(_tmp5FF,sizeof(char),0)),((_tmp601='n',((_get_dyneither_size(_tmp5FF,sizeof(char))== 1  && (_tmp600 == '\000'  && _tmp601 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FF.curr)=_tmp601)))))));}goto _LL21;_LL2A: if(_tmp1F != '\r')goto _LL2C;_LL2B:
{char _tmp604;char _tmp603;struct _dyneither_ptr _tmp602;(_tmp602=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp603=*((char*)_check_dyneither_subscript(_tmp602,sizeof(char),0)),((_tmp604='\\',((_get_dyneither_size(_tmp602,sizeof(char))== 1  && (_tmp603 == '\000'  && _tmp604 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp602.curr)=_tmp604)))))));}{char _tmp607;char _tmp606;struct _dyneither_ptr _tmp605;(_tmp605=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp606=*((char*)_check_dyneither_subscript(_tmp605,sizeof(char),0)),((_tmp607='r',((_get_dyneither_size(_tmp605,sizeof(char))== 1  && (_tmp606 == '\000'  && _tmp607 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp605.curr)=_tmp607)))))));}goto _LL21;_LL2C: if(_tmp1F != '\t')goto _LL2E;_LL2D:
{char _tmp60A;char _tmp609;struct _dyneither_ptr _tmp608;(_tmp608=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp609=*((char*)_check_dyneither_subscript(_tmp608,sizeof(char),0)),((_tmp60A='\\',((_get_dyneither_size(_tmp608,sizeof(char))== 1  && (_tmp609 == '\000'  && _tmp60A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp608.curr)=_tmp60A)))))));}{char _tmp60D;char _tmp60C;struct _dyneither_ptr _tmp60B;(_tmp60B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60C=*((char*)_check_dyneither_subscript(_tmp60B,sizeof(char),0)),((_tmp60D='t',((_get_dyneither_size(_tmp60B,sizeof(char))== 1  && (_tmp60C == '\000'  && _tmp60D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60B.curr)=_tmp60D)))))));}goto _LL21;_LL2E: if(_tmp1F != '\v')goto _LL30;_LL2F:
{char _tmp610;char _tmp60F;struct _dyneither_ptr _tmp60E;(_tmp60E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60F=*((char*)_check_dyneither_subscript(_tmp60E,sizeof(char),0)),((_tmp610='\\',((_get_dyneither_size(_tmp60E,sizeof(char))== 1  && (_tmp60F == '\000'  && _tmp610 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60E.curr)=_tmp610)))))));}{char _tmp613;char _tmp612;struct _dyneither_ptr _tmp611;(_tmp611=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp612=*((char*)_check_dyneither_subscript(_tmp611,sizeof(char),0)),((_tmp613='v',((_get_dyneither_size(_tmp611,sizeof(char))== 1  && (_tmp612 == '\000'  && _tmp613 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp611.curr)=_tmp613)))))));}goto _LL21;_LL30: if(_tmp1F != '\\')goto _LL32;_LL31:
{char _tmp616;char _tmp615;struct _dyneither_ptr _tmp614;(_tmp614=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp615=*((char*)_check_dyneither_subscript(_tmp614,sizeof(char),0)),((_tmp616='\\',((_get_dyneither_size(_tmp614,sizeof(char))== 1  && (_tmp615 == '\000'  && _tmp616 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp614.curr)=_tmp616)))))));}{char _tmp619;char _tmp618;struct _dyneither_ptr _tmp617;(_tmp617=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp618=*((char*)_check_dyneither_subscript(_tmp617,sizeof(char),0)),((_tmp619='\\',((_get_dyneither_size(_tmp617,sizeof(char))== 1  && (_tmp618 == '\000'  && _tmp619 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp617.curr)=_tmp619)))))));}goto _LL21;_LL32: if(_tmp1F != '"')goto _LL34;_LL33:
{char _tmp61C;char _tmp61B;struct _dyneither_ptr _tmp61A;(_tmp61A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61B=*((char*)_check_dyneither_subscript(_tmp61A,sizeof(char),0)),((_tmp61C='\\',((_get_dyneither_size(_tmp61A,sizeof(char))== 1  && (_tmp61B == '\000'  && _tmp61C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61A.curr)=_tmp61C)))))));}{char _tmp61F;char _tmp61E;struct _dyneither_ptr _tmp61D;(_tmp61D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61E=*((char*)_check_dyneither_subscript(_tmp61D,sizeof(char),0)),((_tmp61F='"',((_get_dyneither_size(_tmp61D,sizeof(char))== 1  && (_tmp61E == '\000'  && _tmp61F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61D.curr)=_tmp61F)))))));}goto _LL21;_LL34: _tmp20=_tmp1F;_LL35:
# 341
 if(_tmp20 >= ' '  && _tmp20 <= '~'){char _tmp622;char _tmp621;struct _dyneither_ptr _tmp620;(_tmp620=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp621=*((char*)_check_dyneither_subscript(_tmp620,sizeof(char),0)),((_tmp622=_tmp20,((_get_dyneither_size(_tmp620,sizeof(char))== 1  && (_tmp621 == '\000'  && _tmp622 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp620.curr)=_tmp622)))))));}else{
# 343
{char _tmp625;char _tmp624;struct _dyneither_ptr _tmp623;(_tmp623=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp624=*((char*)_check_dyneither_subscript(_tmp623,sizeof(char),0)),((_tmp625='\\',((_get_dyneither_size(_tmp623,sizeof(char))== 1  && (_tmp624 == '\000'  && _tmp625 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp623.curr)=_tmp625)))))));}
{char _tmp628;char _tmp627;struct _dyneither_ptr _tmp626;(_tmp626=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp627=*((char*)_check_dyneither_subscript(_tmp626,sizeof(char),0)),((_tmp628=(char)('0' + (_tmp20 >> 6 & 7)),((_get_dyneither_size(_tmp626,sizeof(char))== 1  && (_tmp627 == '\000'  && _tmp628 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp626.curr)=_tmp628)))))));}
{char _tmp62B;char _tmp62A;struct _dyneither_ptr _tmp629;(_tmp629=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62A=*((char*)_check_dyneither_subscript(_tmp629,sizeof(char),0)),((_tmp62B=(char)('0' + (_tmp20 >> 3 & 7)),((_get_dyneither_size(_tmp629,sizeof(char))== 1  && (_tmp62A == '\000'  && _tmp62B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp629.curr)=_tmp62B)))))));}{
char _tmp62E;char _tmp62D;struct _dyneither_ptr _tmp62C;(_tmp62C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62D=*((char*)_check_dyneither_subscript(_tmp62C,sizeof(char),0)),((_tmp62E=(char)('0' + (_tmp20 & 7)),((_get_dyneither_size(_tmp62C,sizeof(char))== 1  && (_tmp62D == '\000'  && _tmp62E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62C.curr)=_tmp62E)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp62F;l=((_tmp62F=_cycalloc(sizeof(*_tmp62F)),((_tmp62F->hd=Cyc_Absynpp_restrict_sp,((_tmp62F->tl=l,_tmp62F))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp630;l=((_tmp630=_cycalloc(sizeof(*_tmp630)),((_tmp630->hd=Cyc_Absynpp_volatile_sp,((_tmp630->tl=l,_tmp630))))));}
if(tq.print_const){struct Cyc_List_List*_tmp631;l=((_tmp631=_cycalloc(sizeof(*_tmp631)),((_tmp631->hd=Cyc_Absynpp_const_sp,((_tmp631->tl=l,_tmp631))))));}{
const char*_tmp634;const char*_tmp633;const char*_tmp632;return Cyc_PP_egroup(((_tmp632="",_tag_dyneither(_tmp632,sizeof(char),1))),((_tmp633=" ",_tag_dyneither(_tmp633,sizeof(char),2))),((_tmp634=" ",_tag_dyneither(_tmp634,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=_tmp6F->kind;_tmp71=_tmp6F->aliasqual;
switch(_tmp70){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp635;return(_tmp635="A",_tag_dyneither(_tmp635,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp636;return(_tmp636="UA",_tag_dyneither(_tmp636,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp637;return(_tmp637="TA",_tag_dyneither(_tmp637,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp638;return(_tmp638="M",_tag_dyneither(_tmp638,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp639;return(_tmp639="UM",_tag_dyneither(_tmp639,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp63A;return(_tmp63A="TM",_tag_dyneither(_tmp63A,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp63B;return(_tmp63B="B",_tag_dyneither(_tmp63B,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp63C;return(_tmp63C="UB",_tag_dyneither(_tmp63C,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp63D;return(_tmp63D="TB",_tag_dyneither(_tmp63D,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp63E;return(_tmp63E="R",_tag_dyneither(_tmp63E,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp63F;return(_tmp63F="UR",_tag_dyneither(_tmp63F,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp640;return(_tmp640="TR",_tag_dyneither(_tmp640,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp641;return(_tmp641="E",_tag_dyneither(_tmp641,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp642;return(_tmp642="I",_tag_dyneither(_tmp642,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_stmttmp2=Cyc_Absyn_compress_kb(c);void*_tmp80=_stmttmp2;struct Cyc_Absyn_Kind*_tmp82;struct Cyc_Absyn_Kind*_tmp85;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp81->tag != 0)goto _LL50;else{_tmp82=_tmp81->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp82);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp83->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp643;return(_tmp643="{?}",_tag_dyneither(_tmp643,sizeof(char),4));}else{
const char*_tmp644;return(_tmp644="?",_tag_dyneither(_tmp644,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp84->tag != 2)goto _LL4D;else{_tmp85=_tmp84->f2;}}_LL53: {
const char*_tmp648;void*_tmp647[1];struct Cyc_String_pa_PrintArg_struct _tmp646;return(struct _dyneither_ptr)((_tmp646.tag=0,((_tmp646.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp85)),((_tmp647[0]=& _tmp646,Cyc_aprintf(((_tmp648="<=%s",_tag_dyneither(_tmp648,sizeof(char),5))),_tag_dyneither(_tmp647,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_stmttmp3=Cyc_Absyn_compress_kb(c);void*_tmp8B=_stmttmp3;struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp90;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL57;else{_tmp8D=_tmp8C->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8D));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8E->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp649;return Cyc_PP_text_width(((_tmp649="{?}",_tag_dyneither(_tmp649,sizeof(char),4))),1);}else{
const char*_tmp64A;return Cyc_PP_text(((_tmp64A="?",_tag_dyneither(_tmp64A,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8F->tag != 2)goto _LL54;else{_tmp90=_tmp8F->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp90));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp64D;const char*_tmp64C;const char*_tmp64B;return Cyc_PP_egroup(((_tmp64B="<",_tag_dyneither(_tmp64B,sizeof(char),2))),((_tmp64C=">",_tag_dyneither(_tmp64C,sizeof(char),2))),((_tmp64D=",",_tag_dyneither(_tmp64D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp96=*tv->name;
# 432
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
const char*_tmp650;struct Cyc_PP_Doc*_tmp64F[2];return(_tmp64F[1]=Cyc_PP_text(_dyneither_ptr_plus(_tmp96,sizeof(char),1)),((_tmp64F[0]=Cyc_PP_text(((_tmp650="`TTT",_tag_dyneither(_tmp650,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp64F,sizeof(struct Cyc_PP_Doc*),2)))));}
return Cyc_PP_text(_tmp96);}
# 437
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_stmttmp4=Cyc_Absyn_compress_kb(tv->kind);void*_tmp99=_stmttmp4;struct Cyc_Absyn_Kind*_tmp9D;struct Cyc_Absyn_Kind*_tmp9F;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9A->tag != 1)goto _LL5E;}_LL5D:
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9B->tag != 0)goto _LL60;else{if((_tmp9B->f1)->kind != Cyc_Absyn_BoxKind)goto _LL60;if((_tmp9B->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9C->tag != 2)goto _LL62;else{_tmp9D=_tmp9C->f2;}}_LL61:
 _tmp9F=_tmp9D;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9E=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp99;if(_tmp9E->tag != 0)goto _LL5B;else{_tmp9F=_tmp9E->f1;}}_LL63: {
const char*_tmp653;struct Cyc_PP_Doc*_tmp652[3];return(_tmp652[2]=Cyc_Absynpp_kind2doc(_tmp9F),((_tmp652[1]=Cyc_PP_text(((_tmp653="::",_tag_dyneither(_tmp653,sizeof(char),3)))),((_tmp652[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp652,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 446
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp656;const char*_tmp655;const char*_tmp654;return Cyc_PP_egroup(((_tmp654="<",_tag_dyneither(_tmp654,sizeof(char),2))),((_tmp655=">",_tag_dyneither(_tmp655,sizeof(char),2))),((_tmp656=",",_tag_dyneither(_tmp656,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp659;const char*_tmp658;const char*_tmp657;return Cyc_PP_egroup(((_tmp657="<",_tag_dyneither(_tmp657,sizeof(char),2))),((_tmp658=">",_tag_dyneither(_tmp658,sizeof(char),2))),((_tmp659=",",_tag_dyneither(_tmp659,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp65C;const char*_tmp65B;const char*_tmp65A;return Cyc_PP_group(((_tmp65A="(",_tag_dyneither(_tmp65A,sizeof(char),2))),((_tmp65B=")",_tag_dyneither(_tmp65B,sizeof(char),2))),((_tmp65C=",",_tag_dyneither(_tmp65C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpAB=att;_LL65: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAC=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAC->tag != 1)goto _LL67;}_LL66:
 goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAD=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAD->tag != 2)goto _LL69;}_LL68:
 goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAE=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAB;if(_tmpAE->tag != 3)goto _LL6B;}_LL6A:
 return Cyc_PP_nil_doc();_LL6B:;_LL6C:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp5=(void*)atts->hd;void*_tmpAF=_stmttmp5;_LL6E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB0=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB0->tag != 1)goto _LL70;}_LL6F: {
const char*_tmp65D;return Cyc_PP_text(((_tmp65D=" _stdcall ",_tag_dyneither(_tmp65D,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB1=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB1->tag != 2)goto _LL72;}_LL71: {
const char*_tmp65E;return Cyc_PP_text(((_tmp65E=" _cdecl ",_tag_dyneither(_tmp65E,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB2=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAF;if(_tmpB2->tag != 3)goto _LL74;}_LL73: {
const char*_tmp65F;return Cyc_PP_text(((_tmp65F=" _fastcall ",_tag_dyneither(_tmp65F,sizeof(char),12))));}_LL74:;_LL75:
 goto _LL6D;_LL6D:;}
# 481
return Cyc_PP_nil_doc();}
# 484
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_stmttmp6=(void*)atts2->hd;void*_tmpB6=_stmttmp6;_LL77: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB7->tag != 1)goto _LL79;}_LL78:
 goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB8->tag != 2)goto _LL7B;}_LL7A:
 goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB6;if(_tmpB9->tag != 3)goto _LL7D;}_LL7C:
 goto _LL76;_LL7D:;_LL7E:
 hasatt=1;goto _LL76;_LL76:;}}
# 493
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp66A;const char*_tmp669;const char*_tmp668;const char*_tmp667;const char*_tmp666;struct Cyc_PP_Doc*_tmp665[3];return(_tmp665[2]=
# 497
Cyc_PP_text(((_tmp666=")",_tag_dyneither(_tmp666,sizeof(char),2)))),((_tmp665[1]=
# 496
Cyc_PP_group(((_tmp669="",_tag_dyneither(_tmp669,sizeof(char),1))),((_tmp668="",_tag_dyneither(_tmp668,sizeof(char),1))),((_tmp667=" ",_tag_dyneither(_tmp667,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp665[0]=
# 495
Cyc_PP_text(((_tmp66A=" __declspec(",_tag_dyneither(_tmp66A,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp665,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 500
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp673;const char*_tmp672;const char*_tmp671;const char*_tmp670;struct Cyc_PP_Doc*_tmp66F[2];return(_tmp66F[1]=
Cyc_PP_group(((_tmp672="((",_tag_dyneither(_tmp672,sizeof(char),3))),((_tmp671="))",_tag_dyneither(_tmp671,sizeof(char),3))),((_tmp670=",",_tag_dyneither(_tmp670,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp66F[0]=
# 508
Cyc_PP_text(((_tmp673=" __attribute__",_tag_dyneither(_tmp673,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp66F,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 513
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_stmttmp7=(void*)tms->hd;void*_tmpC5=_stmttmp7;_LL83: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC6=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC6->tag != 2)goto _LL85;}_LL84:
 return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC7=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC5;if(_tmpC7->tag != 5)goto _LL87;}_LL86:
# 518
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL89:
 return 0;default: _LL8A:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL87:;_LL88:
# 522
 return 0;_LL82:;};}
# 526
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 529
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){
const char*_tmp674;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp674="{?}",_tag_dyneither(_tmp674,sizeof(char),4))),1);}else{
const char*_tmp675;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp675="?",_tag_dyneither(_tmp675,sizeof(char),2))));}}
# 536
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp676;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp676="{\\lb}",_tag_dyneither(_tmp676,sizeof(char),6))),1);}else{
const char*_tmp677;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp677="{",_tag_dyneither(_tmp677,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp678;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp678="{\\rb}",_tag_dyneither(_tmp678,sizeof(char),6))),1);}else{
const char*_tmp679;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp679="}",_tag_dyneither(_tmp679,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp67A;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp67A="\\$",_tag_dyneither(_tmp67A,sizeof(char),3))),1);}else{
const char*_tmp67B;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp67B="$",_tag_dyneither(_tmp67B,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 565
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp67C[3];return(_tmp67C[2]=Cyc_Absynpp_rb(),((_tmp67C[1]=Cyc_PP_seq(sep,ss),((_tmp67C[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp67C,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 570
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_stmttmp8=(void*)tms->hd;void*_tmpD1=_stmttmp8;struct Cyc_List_List*_tmpD6;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD2->tag != 0)goto _LL8F;}_LL8E:
{const char*_tmp67F;void*_tmp67E;(_tmp67E=0,Cyc_fprintf(Cyc_stderr,((_tmp67F="Carray_mod ",_tag_dyneither(_tmp67F,sizeof(char),12))),_tag_dyneither(_tmp67E,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD3=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD3->tag != 1)goto _LL91;}_LL90:
{const char*_tmp682;void*_tmp681;(_tmp681=0,Cyc_fprintf(Cyc_stderr,((_tmp682="ConstArray_mod ",_tag_dyneither(_tmp682,sizeof(char),16))),_tag_dyneither(_tmp681,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD4=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD4->tag != 3)goto _LL93;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD5=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpD4->f1);if(_tmpD5->tag != 1)goto _LL93;else{_tmpD6=_tmpD5->f1;}}}_LL92:
# 576
{const char*_tmp685;void*_tmp684;(_tmp684=0,Cyc_fprintf(Cyc_stderr,((_tmp685="Function_mod(",_tag_dyneither(_tmp685,sizeof(char),14))),_tag_dyneither(_tmp684,sizeof(void*),0)));}
for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct _dyneither_ptr*_tmpE1=(*((struct _tuple8*)_tmpD6->hd)).f1;
if(_tmpE1 == 0){const char*_tmp688;void*_tmp687;(_tmp687=0,Cyc_fprintf(Cyc_stderr,((_tmp688="?",_tag_dyneither(_tmp688,sizeof(char),2))),_tag_dyneither(_tmp687,sizeof(void*),0)));}else{
void*_tmp689;(_tmp689=0,Cyc_fprintf(Cyc_stderr,*_tmpE1,_tag_dyneither(_tmp689,sizeof(void*),0)));}
if(_tmpD6->tl != 0){const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C=",",_tag_dyneither(_tmp68C,sizeof(char),2))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}}
# 583
{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((_tmp68F=") ",_tag_dyneither(_tmp68F,sizeof(char),3))),_tag_dyneither(_tmp68E,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD7->tag != 3)goto _LL95;}_LL94:
# 586
{const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(Cyc_stderr,((_tmp692="Function_mod()",_tag_dyneither(_tmp692,sizeof(char),15))),_tag_dyneither(_tmp691,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD8->tag != 5)goto _LL97;}_LL96:
{const char*_tmp695;void*_tmp694;(_tmp694=0,Cyc_fprintf(Cyc_stderr,((_tmp695="Attributes_mod ",_tag_dyneither(_tmp695,sizeof(char),16))),_tag_dyneither(_tmp694,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD9=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpD9->tag != 4)goto _LL99;}_LL98:
{const char*_tmp698;void*_tmp697;(_tmp697=0,Cyc_fprintf(Cyc_stderr,((_tmp698="TypeParams_mod ",_tag_dyneither(_tmp698,sizeof(char),16))),_tag_dyneither(_tmp697,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD1;if(_tmpDA->tag != 2)goto _LL8C;}_LL9A:
{const char*_tmp69B;void*_tmp69A;(_tmp69A=0,Cyc_fprintf(Cyc_stderr,((_tmp69B="Pointer_mod ",_tag_dyneither(_tmp69B,sizeof(char),13))),_tag_dyneither(_tmp69A,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 591
const char*_tmp69E;void*_tmp69D;(_tmp69D=0,Cyc_fprintf(Cyc_stderr,((_tmp69E="\n",_tag_dyneither(_tmp69E,sizeof(char),2))),_tag_dyneither(_tmp69D,sizeof(void*),0)));};}
# 594
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6A3;const char*_tmp6A2;struct Cyc_PP_Doc*_tmp6A1[3];struct Cyc_PP_Doc*p_rest=(_tmp6A1[2]=Cyc_PP_text(((_tmp6A2=")",_tag_dyneither(_tmp6A2,sizeof(char),2)))),((_tmp6A1[1]=rest,((_tmp6A1[0]=Cyc_PP_text(((_tmp6A3="(",_tag_dyneither(_tmp6A3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_stmttmp9=(void*)tms->hd;void*_tmpF3=_stmttmp9;union Cyc_Absyn_Constraint*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;union Cyc_Absyn_Constraint*_tmpF8;void*_tmpFA;struct Cyc_List_List*_tmpFC;struct Cyc_List_List*_tmpFE;unsigned int _tmpFF;int _tmp100;void*_tmp102;union Cyc_Absyn_Constraint*_tmp103;union Cyc_Absyn_Constraint*_tmp104;union Cyc_Absyn_Constraint*_tmp105;struct Cyc_Absyn_Tqual _tmp106;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF4->tag != 0)goto _LL9E;else{_tmpF5=_tmpF4->f1;}}_LL9D:
# 602
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A8;const char*_tmp6A7;struct Cyc_PP_Doc*_tmp6A6[2];return(_tmp6A6[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF5)?
Cyc_PP_text(((_tmp6A7="[]@zeroterm",_tag_dyneither(_tmp6A7,sizeof(char),12)))): Cyc_PP_text(((_tmp6A8="[]",_tag_dyneither(_tmp6A8,sizeof(char),3)))),((_tmp6A6[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A6,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF6=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF6->tag != 1)goto _LLA0;else{_tmpF7=_tmpF6->f1;_tmpF8=_tmpF6->f2;}}_LL9F:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6AF;const char*_tmp6AE;const char*_tmp6AD;struct Cyc_PP_Doc*_tmp6AC[4];return(_tmp6AC[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF8)?Cyc_PP_text(((_tmp6AD="]@zeroterm",_tag_dyneither(_tmp6AD,sizeof(char),11)))): Cyc_PP_text(((_tmp6AE="]",_tag_dyneither(_tmp6AE,sizeof(char),2)))),((_tmp6AC[2]=
# 607
Cyc_Absynpp_exp2doc(_tmpF7),((_tmp6AC[1]=Cyc_PP_text(((_tmp6AF="[",_tag_dyneither(_tmp6AF,sizeof(char),2)))),((_tmp6AC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AC,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpF9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpF9->tag != 3)goto _LLA2;else{_tmpFA=(void*)_tmpF9->f1;}}_LLA1:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp10E=_tmpFA;struct Cyc_List_List*_tmp110;int _tmp111;struct Cyc_Absyn_VarargInfo*_tmp112;void*_tmp113;struct Cyc_List_List*_tmp114;struct Cyc_List_List*_tmp116;unsigned int _tmp117;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp10F=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp10F->tag != 1)goto _LLAB;else{_tmp110=_tmp10F->f1;_tmp111=_tmp10F->f2;_tmp112=_tmp10F->f3;_tmp113=(void*)_tmp10F->f4;_tmp114=_tmp10F->f5;}}_LLAA: {
# 613
struct Cyc_PP_Doc*_tmp6B0[2];return(_tmp6B0[1]=Cyc_Absynpp_funargs2doc(_tmp110,_tmp111,_tmp112,_tmp113,_tmp114),((_tmp6B0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B0,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp115=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp10E;if(_tmp115->tag != 0)goto _LLA8;else{_tmp116=_tmp115->f1;_tmp117=_tmp115->f2;}}_LLAC: {
# 615
const char*_tmp6B7;const char*_tmp6B6;const char*_tmp6B5;struct Cyc_PP_Doc*_tmp6B4[2];return(_tmp6B4[1]=Cyc_PP_group(((_tmp6B7="(",_tag_dyneither(_tmp6B7,sizeof(char),2))),((_tmp6B6=")",_tag_dyneither(_tmp6B6,sizeof(char),2))),((_tmp6B5=",",_tag_dyneither(_tmp6B5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp116)),((_tmp6B4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFB->tag != 5)goto _LLA4;else{_tmpFC=_tmpFB->f2;}}_LLA3:
# 618
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 620
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B8[2];return(_tmp6B8[1]=Cyc_Absynpp_atts2doc(_tmpFC),((_tmp6B8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B8,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 623
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6B9[2];return(_tmp6B9[1]=rest,((_tmp6B9[0]=Cyc_Absynpp_callconv2doc(_tmpFC),Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFD=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmpFD->tag != 4)goto _LLA6;else{_tmpFE=_tmpFD->f1;_tmpFF=_tmpFD->f2;_tmp100=_tmpFD->f3;}}_LLA5:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp100){
struct Cyc_PP_Doc*_tmp6BA[2];return(_tmp6BA[1]=Cyc_Absynpp_ktvars2doc(_tmpFE),((_tmp6BA[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 632
struct Cyc_PP_Doc*_tmp6BB[2];return(_tmp6BB[1]=Cyc_Absynpp_tvars2doc(_tmpFE),((_tmp6BB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp101=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF3;if(_tmp101->tag != 2)goto _LL9B;else{_tmp102=(_tmp101->f1).rgn;_tmp103=(_tmp101->f1).nullable;_tmp104=(_tmp101->f1).bounds;_tmp105=(_tmp101->f1).zero_term;_tmp106=_tmp101->f2;}}_LLA7: {
# 636
struct Cyc_PP_Doc*ptr;
{void*_stmttmpA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp104);void*_tmp121=_stmttmpA;struct Cyc_Absyn_Exp*_tmp124;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp122=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp121;if(_tmp122->tag != 0)goto _LLB3;}_LLB2:
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp123=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp121;if(_tmp123->tag != 1)goto _LLB0;else{_tmp124=_tmp123->f1;}}_LLB4:
# 640
{const char*_tmp6BD;const char*_tmp6BC;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp103)?(_tmp6BD="*",_tag_dyneither(_tmp6BD,sizeof(char),2)):((_tmp6BC="@",_tag_dyneither(_tmp6BC,sizeof(char),2))));}{
struct _tuple11 _stmttmpB=Cyc_Evexp_eval_const_uint_exp(_tmp124);unsigned int _tmp128;int _tmp129;struct _tuple11 _tmp127=_stmttmpB;_tmp128=_tmp127.f1;_tmp129=_tmp127.f2;
if(!_tmp129  || _tmp128 != 1){
struct Cyc_PP_Doc*_tmp6BE[4];ptr=((_tmp6BE[3]=Cyc_Absynpp_rb(),((_tmp6BE[2]=Cyc_Absynpp_exp2doc(_tmp124),((_tmp6BE[1]=Cyc_Absynpp_lb(),((_tmp6BE[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}
# 646
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6C1;struct Cyc_PP_Doc*_tmp6C0[2];ptr=((_tmp6C0[1]=Cyc_PP_text(((_tmp6C1="@zeroterm ",_tag_dyneither(_tmp6C1,sizeof(char),11)))),((_tmp6C0[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C0,sizeof(struct Cyc_PP_Doc*),2))))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp105)){
const char*_tmp6C4;struct Cyc_PP_Doc*_tmp6C3[2];ptr=((_tmp6C3[1]=Cyc_PP_text(((_tmp6C4="@nozeroterm ",_tag_dyneither(_tmp6C4,sizeof(char),13)))),((_tmp6C3[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C3,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 652
{void*_stmttmpC=Cyc_Tcutil_compress(_tmp102);void*_tmp12F=_stmttmpC;_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp130=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12F;if(_tmp130->tag != 20)goto _LLB8;}_LLB7:
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp131=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12F;if(_tmp131->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
 goto _LLB5;_LLBA:;_LLBB: {
const char*_tmp6C7;struct Cyc_PP_Doc*_tmp6C6[3];ptr=((_tmp6C6[2]=Cyc_PP_text(((_tmp6C7=" ",_tag_dyneither(_tmp6C7,sizeof(char),2)))),((_tmp6C6[1]=Cyc_Absynpp_rgn2doc(_tmp102),((_tmp6C6[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C6,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 657
{struct Cyc_PP_Doc*_tmp6C8[2];ptr=((_tmp6C8[1]=Cyc_Absynpp_tqual2doc(_tmp106),((_tmp6C8[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C8,sizeof(struct Cyc_PP_Doc*),2))))));}{
struct Cyc_PP_Doc*_tmp6C9[2];return(_tmp6C9[1]=rest,((_tmp6C9[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C9,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 662
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_stmttmpD=Cyc_Tcutil_compress(t);void*_tmp139=_stmttmpD;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp139;if(_tmp13A->tag != 20)goto _LLBF;}_LLBE: {
const char*_tmp6CA;return Cyc_PP_text(((_tmp6CA="`H",_tag_dyneither(_tmp6CA,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp139;if(_tmp13B->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp6CB;return Cyc_PP_text(((_tmp6CB="`U",_tag_dyneither(_tmp6CB,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13C=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp139;if(_tmp13C->tag != 22)goto _LLC3;}_LLC2: {
const char*_tmp6CC;return Cyc_PP_text(((_tmp6CC="`RC",_tag_dyneither(_tmp6CC,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp139;if(_tmp13D->tag != 2)goto _LLC5;}_LLC4:
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
const char*_tmp6D1;const char*_tmp6D0;struct Cyc_PP_Doc*_tmp6CF[3];return(_tmp6CF[2]=Cyc_PP_text(((_tmp6D0=")",_tag_dyneither(_tmp6D0,sizeof(char),2)))),((_tmp6CF[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp6CF[0]=Cyc_PP_text(((_tmp6D1="@region(",_tag_dyneither(_tmp6D1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6CF,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 672
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 676
void*_stmttmpE=Cyc_Tcutil_compress(t);void*_tmp144=_stmttmpE;void*_tmp146;struct Cyc_List_List*_tmp148;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp144;if(_tmp145->tag != 23)goto _LLCA;else{_tmp146=(void*)_tmp145->f1;}}_LLC9:
{struct Cyc_List_List*_tmp6D2;*rgions=((_tmp6D2=_cycalloc(sizeof(*_tmp6D2)),((_tmp6D2->hd=Cyc_Absynpp_rgn2doc(_tmp146),((_tmp6D2->tl=*rgions,_tmp6D2))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp147=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp144;if(_tmp147->tag != 24)goto _LLCC;else{_tmp148=_tmp147->f1;}}_LLCB:
# 679
 for(0;_tmp148 != 0;_tmp148=_tmp148->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp148->hd);}
# 682
goto _LLC7;_LLCC:;_LLCD:
{struct Cyc_List_List*_tmp6D3;*effects=((_tmp6D3=_cycalloc(sizeof(*_tmp6D3)),((_tmp6D3->hd=Cyc_Absynpp_typ2doc(t),((_tmp6D3->tl=*effects,_tmp6D3))))));}goto _LLC7;_LLC7:;}
# 687
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6D6;const char*_tmp6D5;const char*_tmp6D4;return Cyc_PP_group(((_tmp6D4="",_tag_dyneither(_tmp6D4,sizeof(char),1))),((_tmp6D5="",_tag_dyneither(_tmp6D5,sizeof(char),1))),((_tmp6D6="+",_tag_dyneither(_tmp6D6,sizeof(char),2))),effects);}else{
# 695
const char*_tmp6D7;struct Cyc_PP_Doc*_tmp14E=Cyc_Absynpp_group_braces(((_tmp6D7=",",_tag_dyneither(_tmp6D7,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6DB;const char*_tmp6DA;const char*_tmp6D9;const char*_tmp6D8;return Cyc_PP_group(((_tmp6D8="",_tag_dyneither(_tmp6D8,sizeof(char),1))),((_tmp6D9="",_tag_dyneither(_tmp6D9,sizeof(char),1))),((_tmp6DA="+",_tag_dyneither(_tmp6DA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6DB=_cycalloc(sizeof(*_tmp6DB)),((_tmp6DB->hd=_tmp14E,((_tmp6DB->tl=0,_tmp6DB))))))));}}
# 700
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLCE: {
const char*_tmp6DC;return Cyc_PP_text(((_tmp6DC="struct ",_tag_dyneither(_tmp6DC,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
const char*_tmp6DD;return Cyc_PP_text(((_tmp6DD="union ",_tag_dyneither(_tmp6DD,sizeof(char),7))));}}}
# 708
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp156=t;struct Cyc_Core_Opt*_tmp15C;void*_tmp15D;int _tmp15E;struct Cyc_Core_Opt*_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;union Cyc_Absyn_DatatypeInfoU _tmp163;struct Cyc_List_List*_tmp164;union Cyc_Absyn_DatatypeFieldInfoU _tmp166;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_Sign _tmp169;enum Cyc_Absyn_Size_of _tmp16A;int _tmp16C;struct Cyc_List_List*_tmp16E;union Cyc_Absyn_AggrInfoU _tmp170;struct Cyc_List_List*_tmp171;enum Cyc_Absyn_AggrKind _tmp173;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp176;struct _tuple0*_tmp178;struct Cyc_Absyn_Exp*_tmp17A;struct _tuple0*_tmp17C;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_Typedefdecl*_tmp17E;void*_tmp180;void*_tmp182;void*_tmp183;void*_tmp185;void*_tmp18A;struct Cyc_Absyn_Aggrdecl*_tmp18F;struct Cyc_Absyn_Enumdecl*_tmp192;struct Cyc_Absyn_Datatypedecl*_tmp195;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp156;if(_tmp157->tag != 8)goto _LLD4;}_LLD3: {
# 712
const char*_tmp6DE;return Cyc_PP_text(((_tmp6DE="[[[array]]]",_tag_dyneither(_tmp6DE,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp156;if(_tmp158->tag != 9)goto _LLD6;}_LLD5:
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp156;if(_tmp159->tag != 5)goto _LLD8;}_LLD7:
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp156;if(_tmp15A->tag != 0)goto _LLDA;}_LLD9:
# 716
{const char*_tmp6DF;s=Cyc_PP_text(((_tmp6DF="void",_tag_dyneither(_tmp6DF,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp156;if(_tmp15B->tag != 1)goto _LLDC;else{_tmp15C=_tmp15B->f1;_tmp15D=(void*)_tmp15B->f2;_tmp15E=_tmp15B->f3;_tmp15F=_tmp15B->f4;}}_LLDB:
# 718
 if(_tmp15D != 0)
# 720
return Cyc_Absynpp_ntyp2doc(_tmp15D);else{
# 728
const char*_tmp6EF;const char*_tmp6EE;const char*_tmp6ED;struct Cyc_Int_pa_PrintArg_struct _tmp6EC;void*_tmp6EB[1];const char*_tmp6EA;const char*_tmp6E9;struct Cyc_PP_Doc*_tmp6E8[6];s=((_tmp6E8[5]=
# 733
_tmp15C == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15C->v),((_tmp6E8[4]=Cyc_PP_text(((_tmp6E9=")::",_tag_dyneither(_tmp6E9,sizeof(char),4)))),((_tmp6E8[3]=
# 731
(!Cyc_Absynpp_print_full_evars  || _tmp15F == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15F->v),((_tmp6E8[2]=
# 730
Cyc_PP_text((struct _dyneither_ptr)((_tmp6EC.tag=1,((_tmp6EC.f1=(unsigned long)_tmp15E,((_tmp6EB[0]=& _tmp6EC,Cyc_aprintf(((_tmp6EA="%d",_tag_dyneither(_tmp6EA,sizeof(char),3))),_tag_dyneither(_tmp6EB,sizeof(void*),1))))))))),((_tmp6E8[1]=
# 729
Cyc_PP_text(((_tmp6ED="(",_tag_dyneither(_tmp6ED,sizeof(char),2)))),((_tmp6E8[0]=
# 728
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6EE="\\%",_tag_dyneither(_tmp6EE,sizeof(char),3)))): Cyc_PP_text(((_tmp6EF="%",_tag_dyneither(_tmp6EF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 736
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp156;if(_tmp160->tag != 2)goto _LLDE;else{_tmp161=_tmp160->f1;}}_LLDD:
# 738
 s=Cyc_Absynpp_tvar2doc(_tmp161);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6F2;struct Cyc_PP_Doc*_tmp6F1[3];s=((_tmp6F1[2]=Cyc_Absynpp_kindbound2doc(_tmp161->kind),((_tmp6F1[1]=Cyc_PP_text(((_tmp6F2="::",_tag_dyneither(_tmp6F2,sizeof(char),3)))),((_tmp6F1[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6F1,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 742
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp162=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp156;if(_tmp162->tag != 3)goto _LLE0;else{_tmp163=(_tmp162->f1).datatype_info;_tmp164=(_tmp162->f1).targs;}}_LLDF:
# 744
{union Cyc_Absyn_DatatypeInfoU _tmp1A2=_tmp163;struct _tuple0*_tmp1A3;int _tmp1A4;struct _tuple0*_tmp1A5;int _tmp1A6;_LL10D: if((_tmp1A2.UnknownDatatype).tag != 1)goto _LL10F;_tmp1A3=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A2.UnknownDatatype).val).name;_tmp1A4=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A2.UnknownDatatype).val).is_extensible;_LL10E:
# 746
 _tmp1A5=_tmp1A3;_tmp1A6=_tmp1A4;goto _LL110;_LL10F: if((_tmp1A2.KnownDatatype).tag != 2)goto _LL10C;_tmp1A5=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A2.KnownDatatype).val))->name;_tmp1A6=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A2.KnownDatatype).val))->is_extensible;_LL110: {
# 748
const char*_tmp6F3;struct Cyc_PP_Doc*_tmp1A7=Cyc_PP_text(((_tmp6F3="datatype ",_tag_dyneither(_tmp6F3,sizeof(char),10))));
const char*_tmp6F4;struct Cyc_PP_Doc*_tmp1A8=_tmp1A6?Cyc_PP_text(((_tmp6F4="@extensible ",_tag_dyneither(_tmp6F4,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6F5[4];s=((_tmp6F5[3]=Cyc_Absynpp_tps2doc(_tmp164),((_tmp6F5[2]=Cyc_Absynpp_qvar2doc(_tmp1A5),((_tmp6F5[1]=_tmp1A7,((_tmp6F5[0]=_tmp1A8,Cyc_PP_cat(_tag_dyneither(_tmp6F5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL10C;}_LL10C:;}
# 753
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp165=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp156;if(_tmp165->tag != 4)goto _LLE2;else{_tmp166=(_tmp165->f1).field_info;_tmp167=(_tmp165->f1).targs;}}_LLE1:
# 755
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AC=_tmp166;struct _tuple0*_tmp1AD;struct _tuple0*_tmp1AE;int _tmp1AF;struct _tuple0*_tmp1B0;int _tmp1B1;struct _tuple0*_tmp1B2;_LL112: if((_tmp1AC.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1AD=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AC.UnknownDatatypefield).val).datatype_name;_tmp1AE=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AC.UnknownDatatypefield).val).field_name;_tmp1AF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AC.UnknownDatatypefield).val).is_extensible;_LL113:
# 757
 _tmp1B0=_tmp1AD;_tmp1B1=_tmp1AF;_tmp1B2=_tmp1AE;goto _LL115;_LL114: if((_tmp1AC.KnownDatatypefield).tag != 2)goto _LL111;_tmp1B0=(((struct _tuple1)(_tmp1AC.KnownDatatypefield).val).f1)->name;_tmp1B1=(((struct _tuple1)(_tmp1AC.KnownDatatypefield).val).f1)->is_extensible;_tmp1B2=(((struct _tuple1)(_tmp1AC.KnownDatatypefield).val).f2)->name;_LL115: {
# 760
const char*_tmp6F7;const char*_tmp6F6;struct Cyc_PP_Doc*_tmp1B3=Cyc_PP_text(_tmp1B1?(_tmp6F7="@extensible datatype ",_tag_dyneither(_tmp6F7,sizeof(char),22)):((_tmp6F6="datatype ",_tag_dyneither(_tmp6F6,sizeof(char),10))));
{const char*_tmp6FA;struct Cyc_PP_Doc*_tmp6F9[4];s=((_tmp6F9[3]=Cyc_Absynpp_qvar2doc(_tmp1B2),((_tmp6F9[2]=Cyc_PP_text(((_tmp6FA=".",_tag_dyneither(_tmp6FA,sizeof(char),2)))),((_tmp6F9[1]=Cyc_Absynpp_qvar2doc(_tmp1B0),((_tmp6F9[0]=_tmp1B3,Cyc_PP_cat(_tag_dyneither(_tmp6F9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 764
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp156;if(_tmp168->tag != 6)goto _LLE4;else{_tmp169=_tmp168->f1;_tmp16A=_tmp168->f2;}}_LLE3: {
# 766
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp169){case Cyc_Absyn_None: _LL116:
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
{const char*_tmp6FB;sns=((_tmp6FB="",_tag_dyneither(_tmp6FB,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
{const char*_tmp6FC;sns=((_tmp6FC="unsigned ",_tag_dyneither(_tmp6FC,sizeof(char),10)));}break;}
# 773
switch(_tmp16A){case Cyc_Absyn_Char_sz: _LL11A:
# 775
 switch(_tmp169){case Cyc_Absyn_None: _LL11C:
{const char*_tmp6FD;sns=((_tmp6FD="",_tag_dyneither(_tmp6FD,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
{const char*_tmp6FE;sns=((_tmp6FE="signed ",_tag_dyneither(_tmp6FE,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
{const char*_tmp6FF;sns=((_tmp6FF="unsigned ",_tag_dyneither(_tmp6FF,sizeof(char),10)));}break;}
# 780
{const char*_tmp700;ts=((_tmp700="char",_tag_dyneither(_tmp700,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL11B:
{const char*_tmp701;ts=((_tmp701="short",_tag_dyneither(_tmp701,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
{const char*_tmp702;ts=((_tmp702="int",_tag_dyneither(_tmp702,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
{const char*_tmp703;ts=((_tmp703="long",_tag_dyneither(_tmp703,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 786
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
{const char*_tmp704;ts=((_tmp704="long long",_tag_dyneither(_tmp704,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
{const char*_tmp705;ts=((_tmp705="__int64",_tag_dyneither(_tmp705,sizeof(char),8)));}break;}
# 790
break;}
# 792
{const char*_tmp70A;void*_tmp709[2];struct Cyc_String_pa_PrintArg_struct _tmp708;struct Cyc_String_pa_PrintArg_struct _tmp707;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp707.tag=0,((_tmp707.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp708.tag=0,((_tmp708.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp709[0]=& _tmp708,((_tmp709[1]=& _tmp707,Cyc_aprintf(((_tmp70A="%s%s",_tag_dyneither(_tmp70A,sizeof(char),5))),_tag_dyneither(_tmp709,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp156;if(_tmp16B->tag != 7)goto _LLE6;else{_tmp16C=_tmp16B->f1;}}_LLE5:
# 795
 if(_tmp16C == 0){
const char*_tmp70B;s=Cyc_PP_text(((_tmp70B="float",_tag_dyneither(_tmp70B,sizeof(char),6))));}else{
if(_tmp16C == 1){
const char*_tmp70C;s=Cyc_PP_text(((_tmp70C="double",_tag_dyneither(_tmp70C,sizeof(char),7))));}else{
# 800
const char*_tmp70D;s=Cyc_PP_text(((_tmp70D="long double",_tag_dyneither(_tmp70D,sizeof(char),12))));}}
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp156;if(_tmp16D->tag != 10)goto _LLE8;else{_tmp16E=_tmp16D->f1;}}_LLE7:
# 803
{struct Cyc_PP_Doc*_tmp70E[2];s=((_tmp70E[1]=Cyc_Absynpp_args2doc(_tmp16E),((_tmp70E[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp16F=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp156;if(_tmp16F->tag != 11)goto _LLEA;else{_tmp170=(_tmp16F->f1).aggr_info;_tmp171=(_tmp16F->f1).targs;}}_LLE9: {
# 806
struct _tuple10 _stmttmpF=Cyc_Absyn_aggr_kinded_name(_tmp170);enum Cyc_Absyn_AggrKind _tmp1CC;struct _tuple0*_tmp1CD;struct _tuple10 _tmp1CB=_stmttmpF;_tmp1CC=_tmp1CB.f1;_tmp1CD=_tmp1CB.f2;
{struct Cyc_PP_Doc*_tmp70F[3];s=((_tmp70F[2]=Cyc_Absynpp_tps2doc(_tmp171),((_tmp70F[1]=Cyc_Absynpp_qvar2doc(_tmp1CD),((_tmp70F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CC),Cyc_PP_cat(_tag_dyneither(_tmp70F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp172=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp156;if(_tmp172->tag != 12)goto _LLEC;else{_tmp173=_tmp172->f1;_tmp174=_tmp172->f2;}}_LLEB:
# 810
{struct Cyc_PP_Doc*_tmp710[4];s=((_tmp710[3]=
# 812
Cyc_Absynpp_rb(),((_tmp710[2]=
# 811
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp174)),((_tmp710[1]=
# 810
Cyc_Absynpp_lb(),((_tmp710[0]=Cyc_Absynpp_aggr_kind2doc(_tmp173),Cyc_PP_cat(_tag_dyneither(_tmp710,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 813
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp156;if(_tmp175->tag != 14)goto _LLEE;else{_tmp176=_tmp175->f1;}}_LLED:
# 815
{const char*_tmp713;struct Cyc_PP_Doc*_tmp712[4];s=((_tmp712[3]=Cyc_Absynpp_rb(),((_tmp712[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp176)),((_tmp712[1]=Cyc_Absynpp_lb(),((_tmp712[0]=Cyc_PP_text(((_tmp713="enum ",_tag_dyneither(_tmp713,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp712,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp156;if(_tmp177->tag != 13)goto _LLF0;else{_tmp178=_tmp177->f1;}}_LLEF:
# 817
{const char*_tmp716;struct Cyc_PP_Doc*_tmp715[2];s=((_tmp715[1]=Cyc_Absynpp_qvar2doc(_tmp178),((_tmp715[0]=Cyc_PP_text(((_tmp716="enum ",_tag_dyneither(_tmp716,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp715,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp156;if(_tmp179->tag != 18)goto _LLF2;else{_tmp17A=_tmp179->f1;}}_LLF1:
# 819
{const char*_tmp71B;const char*_tmp71A;struct Cyc_PP_Doc*_tmp719[3];s=((_tmp719[2]=Cyc_PP_text(((_tmp71A=")",_tag_dyneither(_tmp71A,sizeof(char),2)))),((_tmp719[1]=Cyc_Absynpp_exp2doc(_tmp17A),((_tmp719[0]=Cyc_PP_text(((_tmp71B="valueof_t(",_tag_dyneither(_tmp71B,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp719,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp156;if(_tmp17B->tag != 17)goto _LLF4;else{_tmp17C=_tmp17B->f1;_tmp17D=_tmp17B->f2;_tmp17E=_tmp17B->f3;}}_LLF3:
# 826
{struct Cyc_PP_Doc*_tmp71C[2];s=((_tmp71C[1]=Cyc_Absynpp_tps2doc(_tmp17D),((_tmp71C[0]=Cyc_Absynpp_qvar2doc(_tmp17C),Cyc_PP_cat(_tag_dyneither(_tmp71C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp156;if(_tmp17F->tag != 15)goto _LLF6;else{_tmp180=(void*)_tmp17F->f1;}}_LLF5:
# 829
{const char*_tmp721;const char*_tmp720;struct Cyc_PP_Doc*_tmp71F[3];s=((_tmp71F[2]=Cyc_PP_text(((_tmp720=">",_tag_dyneither(_tmp720,sizeof(char),2)))),((_tmp71F[1]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp71F[0]=Cyc_PP_text(((_tmp721="region_t<",_tag_dyneither(_tmp721,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp71F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp181=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp156;if(_tmp181->tag != 16)goto _LLF8;else{_tmp182=(void*)_tmp181->f1;_tmp183=(void*)_tmp181->f2;}}_LLF7:
# 831
{const char*_tmp728;const char*_tmp727;const char*_tmp726;struct Cyc_PP_Doc*_tmp725[5];s=((_tmp725[4]=
Cyc_PP_text(((_tmp726=">",_tag_dyneither(_tmp726,sizeof(char),2)))),((_tmp725[3]=
# 831
Cyc_Absynpp_rgn2doc(_tmp183),((_tmp725[2]=Cyc_PP_text(((_tmp727=",",_tag_dyneither(_tmp727,sizeof(char),2)))),((_tmp725[1]=Cyc_Absynpp_rgn2doc(_tmp182),((_tmp725[0]=Cyc_PP_text(((_tmp728="dynregion_t<",_tag_dyneither(_tmp728,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp156;if(_tmp184->tag != 19)goto _LLFA;else{_tmp185=(void*)_tmp184->f1;}}_LLF9:
# 834
{const char*_tmp72D;const char*_tmp72C;struct Cyc_PP_Doc*_tmp72B[3];s=((_tmp72B[2]=Cyc_PP_text(((_tmp72C=">",_tag_dyneither(_tmp72C,sizeof(char),2)))),((_tmp72B[1]=Cyc_Absynpp_typ2doc(_tmp185),((_tmp72B[0]=Cyc_PP_text(((_tmp72D="tag_t<",_tag_dyneither(_tmp72D,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp72B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp156;if(_tmp186->tag != 21)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp156;if(_tmp187->tag != 20)goto _LLFE;}_LLFD:
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp156;if(_tmp188->tag != 22)goto _LL100;}_LLFF:
# 838
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp156;if(_tmp189->tag != 25)goto _LL102;else{_tmp18A=(void*)_tmp189->f1;}}_LL101:
# 840
{const char*_tmp732;const char*_tmp731;struct Cyc_PP_Doc*_tmp730[3];s=((_tmp730[2]=Cyc_PP_text(((_tmp731=")",_tag_dyneither(_tmp731,sizeof(char),2)))),((_tmp730[1]=Cyc_Absynpp_typ2doc(_tmp18A),((_tmp730[0]=Cyc_PP_text(((_tmp732="regions(",_tag_dyneither(_tmp732,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp730,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp156;if(_tmp18B->tag != 23)goto _LL104;}_LL103:
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp156;if(_tmp18C->tag != 24)goto _LL106;}_LL105:
# 843
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp18D->tag != 26)goto _LL108;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp18E=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp18D->f1)->r;if(_tmp18E->tag != 0)goto _LL108;else{_tmp18F=_tmp18E->f1;}}}_LL107:
# 845
 s=Cyc_Absynpp_aggrdecl2doc(_tmp18F);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp190->tag != 26)goto _LL10A;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp191=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp190->f1)->r;if(_tmp191->tag != 1)goto _LL10A;else{_tmp192=_tmp191->f1;}}}_LL109:
# 847
 s=Cyc_Absynpp_enumdecl2doc(_tmp192);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp193=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp156;if(_tmp193->tag != 26)goto _LLD1;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp194=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp193->f1)->r;if(_tmp194->tag != 2)goto _LLD1;else{_tmp195=_tmp194->f1;}}}_LL10B:
# 849
 s=Cyc_Absynpp_datatypedecl2doc(_tmp195);goto _LLD1;_LLD1:;}
# 851
return s;}
# 854
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 858
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1E6;void*_tmp1E7;struct _tuple16*_tmp1E5=cmp;_tmp1E6=_tmp1E5->f1;_tmp1E7=_tmp1E5->f2;{
const char*_tmp735;struct Cyc_PP_Doc*_tmp734[3];return(_tmp734[2]=Cyc_Absynpp_rgn2doc(_tmp1E7),((_tmp734[1]=Cyc_PP_text(((_tmp735=" > ",_tag_dyneither(_tmp735,sizeof(char),4)))),((_tmp734[0]=Cyc_Absynpp_rgn2doc(_tmp1E6),Cyc_PP_cat(_tag_dyneither(_tmp734,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 863
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp738;const char*_tmp737;const char*_tmp736;return Cyc_PP_group(((_tmp736="",_tag_dyneither(_tmp736,sizeof(char),1))),((_tmp737="",_tag_dyneither(_tmp737,sizeof(char),1))),((_tmp738=",",_tag_dyneither(_tmp738,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 867
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1ED=(*t).f1;
struct Cyc_Core_Opt*_tmp739;struct Cyc_Core_Opt*dopt=_tmp1ED == 0?0:((_tmp739=_cycalloc(sizeof(*_tmp739)),((_tmp739->v=Cyc_PP_text(*_tmp1ED),_tmp739))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 873
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 876
struct Cyc_List_List*_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp73C;struct Cyc_List_List*_tmp73B;_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1EF,((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B->hd=Cyc_PP_text(((_tmp73C="...",_tag_dyneither(_tmp73C,sizeof(char),4)))),((_tmp73B->tl=0,_tmp73B)))))));}else{
if(cyc_varargs != 0){
const char*_tmp745;const char*_tmp744;const char*_tmp743;struct _tuple8*_tmp742;struct Cyc_PP_Doc*_tmp741[3];struct Cyc_PP_Doc*_tmp1F2=(_tmp741[2]=
# 883
Cyc_Absynpp_funarg2doc(((_tmp742=_cycalloc(sizeof(*_tmp742)),((_tmp742->f1=cyc_varargs->name,((_tmp742->f2=cyc_varargs->tq,((_tmp742->f3=cyc_varargs->type,_tmp742))))))))),((_tmp741[1]=
# 882
cyc_varargs->inject?Cyc_PP_text(((_tmp743=" inject ",_tag_dyneither(_tmp743,sizeof(char),9)))): Cyc_PP_text(((_tmp744=" ",_tag_dyneither(_tmp744,sizeof(char),2)))),((_tmp741[0]=
# 881
Cyc_PP_text(((_tmp745="...",_tag_dyneither(_tmp745,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp741,sizeof(struct Cyc_PP_Doc*),3)))))));
# 885
struct Cyc_List_List*_tmp746;_tmp1EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1EF,((_tmp746=_cycalloc(sizeof(*_tmp746)),((_tmp746->hd=_tmp1F2,((_tmp746->tl=0,_tmp746)))))));}}{
# 887
const char*_tmp749;const char*_tmp748;const char*_tmp747;struct Cyc_PP_Doc*_tmp1F9=Cyc_PP_group(((_tmp747="",_tag_dyneither(_tmp747,sizeof(char),1))),((_tmp748="",_tag_dyneither(_tmp748,sizeof(char),1))),((_tmp749=",",_tag_dyneither(_tmp749,sizeof(char),2))),_tmp1EF);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp74C;struct Cyc_PP_Doc*_tmp74B[3];_tmp1F9=((_tmp74B[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp74B[1]=Cyc_PP_text(((_tmp74C=";",_tag_dyneither(_tmp74C,sizeof(char),2)))),((_tmp74B[0]=_tmp1F9,Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp74F;struct Cyc_PP_Doc*_tmp74E[3];_tmp1F9=((_tmp74E[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp74E[1]=Cyc_PP_text(((_tmp74F=":",_tag_dyneither(_tmp74F,sizeof(char),2)))),((_tmp74E[0]=_tmp1F9,Cyc_PP_cat(_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp754;const char*_tmp753;struct Cyc_PP_Doc*_tmp752[3];return(_tmp752[2]=Cyc_PP_text(((_tmp753=")",_tag_dyneither(_tmp753,sizeof(char),2)))),((_tmp752[1]=_tmp1F9,((_tmp752[0]=Cyc_PP_text(((_tmp754="(",_tag_dyneither(_tmp754,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 895
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp205;struct Cyc_Absyn_Tqual _tmp206;void*_tmp207;struct _tuple8*_tmp204=arg;_tmp205=_tmp204->f1;_tmp206=_tmp204->f2;_tmp207=_tmp204->f3;{
struct _tuple8*_tmp755;return(_tmp755=_cycalloc(sizeof(*_tmp755)),((_tmp755->f1=_tmp205,((_tmp755->f2=_tmp206,((_tmp755->f3=_tmp207,_tmp755)))))));};}
# 900
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 902
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp209=0;
int match;
{union Cyc_Absyn_Nmspace _stmttmp10=(*q).f1;union Cyc_Absyn_Nmspace _tmp20A=_stmttmp10;struct Cyc_List_List*_tmp20B;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20D;_LL128: if((_tmp20A.Loc_n).tag != 4)goto _LL12A;_LL129:
# 907
 _tmp20B=0;goto _LL12B;_LL12A: if((_tmp20A.Rel_n).tag != 1)goto _LL12C;_tmp20B=(struct Cyc_List_List*)(_tmp20A.Rel_n).val;_LL12B:
# 909
 match=0;
_tmp209=_tmp20B;
goto _LL127;_LL12C: if((_tmp20A.C_n).tag != 3)goto _LL12E;_tmp20C=(struct Cyc_List_List*)(_tmp20A.C_n).val;_LL12D:
# 913
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20C,Cyc_Absynpp_curr_namespace);
# 915
goto _LL127;_LL12E: if((_tmp20A.Abs_n).tag != 2)goto _LL127;_tmp20D=(struct Cyc_List_List*)(_tmp20A.Abs_n).val;_LL12F:
# 917
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20D,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp756;_tmp209=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->hd=Cyc_Absynpp_cyc_stringptr,((_tmp756->tl=_tmp20D,_tmp756))))): _tmp20D;}
goto _LL127;_LL127:;}
# 921
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp758;struct Cyc_List_List*_tmp757;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp209,(
(_tmp757=_cycalloc(sizeof(*_tmp757)),((_tmp757->hd=(*q).f2,((_tmp757->tl=0,_tmp757))))))),((_tmp758="_",_tag_dyneither(_tmp758,sizeof(char),2))));}else{
# 926
if(match)
return*(*q).f2;else{
# 929
const char*_tmp75A;struct Cyc_List_List*_tmp759;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp209,(
(_tmp759=_cycalloc(sizeof(*_tmp759)),((_tmp759->hd=(*q).f2,((_tmp759->tl=0,_tmp759))))))),((_tmp75A="::",_tag_dyneither(_tmp75A,sizeof(char),3))));}}}
# 934
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 938
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp213=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 942
const char*_tmp75C;const char*_tmp75B;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75B="\\textbf{",_tag_dyneither(_tmp75B,sizeof(char),9))),(struct _dyneither_ptr)_tmp213),((_tmp75C="}",_tag_dyneither(_tmp75C,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp213));}else{
return Cyc_PP_text(_tmp213);}}
# 947
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 949
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 952
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _stmttmp11=(*v).f1;union Cyc_Absyn_Nmspace _tmp216=_stmttmp11;struct Cyc_List_List*_tmp217;struct Cyc_List_List*_tmp218;_LL131: if((_tmp216.Loc_n).tag != 4)goto _LL133;_LL132:
 goto _LL134;_LL133: if((_tmp216.Rel_n).tag != 1)goto _LL135;if((struct Cyc_List_List*)(_tmp216.Rel_n).val != 0)goto _LL135;_LL134:
 return*(*v).f2;_LL135: if((_tmp216.C_n).tag != 3)goto _LL137;_tmp217=(struct Cyc_List_List*)(_tmp216.C_n).val;_LL136:
 _tmp218=_tmp217;goto _LL138;_LL137: if((_tmp216.Abs_n).tag != 2)goto _LL139;_tmp218=(struct Cyc_List_List*)(_tmp216.Abs_n).val;_LL138:
# 958
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp218,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 961
goto _LL13A;}_LL139:;_LL13A: {
# 963
const char*_tmp75D;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp75D="/* bad namespace : */ ",_tag_dyneither(_tmp75D,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
# 966
return*(*v).f2;}}
# 969
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 972
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp21A=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 976
const char*_tmp75F;const char*_tmp75E;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp75E="\\textbf{",_tag_dyneither(_tmp75E,sizeof(char),9))),(struct _dyneither_ptr)_tmp21A),((_tmp75F="}",_tag_dyneither(_tmp75F,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp21A));}else{
return Cyc_PP_text(_tmp21A);}}
# 981
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 992 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_stmttmp12=e->r;void*_tmp21D=_stmttmp12;enum Cyc_Absyn_Primop _tmp221;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_Absyn_Exp*_tmp22D;_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp21E=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp21E->tag != 0)goto _LL13E;}_LL13D:
 goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp21F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp21F->tag != 1)goto _LL140;}_LL13F:
 return 10000;_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp220=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp220->tag != 2)goto _LL142;else{_tmp221=_tmp220->f1;}}_LL141:
# 997
 switch(_tmp221){case Cyc_Absyn_Plus: _LL18C:
 return 100;case Cyc_Absyn_Times: _LL18D:
 return 110;case Cyc_Absyn_Minus: _LL18E:
 return 100;case Cyc_Absyn_Div: _LL18F:
 goto _LL190;case Cyc_Absyn_Mod: _LL190:
 return 110;case Cyc_Absyn_Eq: _LL191:
 goto _LL192;case Cyc_Absyn_Neq: _LL192:
 return 70;case Cyc_Absyn_Gt: _LL193:
 goto _LL194;case Cyc_Absyn_Lt: _LL194:
 goto _LL195;case Cyc_Absyn_Gte: _LL195:
 goto _LL196;case Cyc_Absyn_Lte: _LL196:
 return 80;case Cyc_Absyn_Not: _LL197:
 goto _LL198;case Cyc_Absyn_Bitnot: _LL198:
 return 130;case Cyc_Absyn_Bitand: _LL199:
 return 60;case Cyc_Absyn_Bitor: _LL19A:
 return 40;case Cyc_Absyn_Bitxor: _LL19B:
 return 50;case Cyc_Absyn_Bitlshift: _LL19C:
 return 90;case Cyc_Absyn_Bitlrshift: _LL19D:
 return 80;case Cyc_Absyn_Bitarshift: _LL19E:
 return 80;case Cyc_Absyn_Numelts: _LL19F:
 return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp222=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp222->tag != 3)goto _LL144;}_LL143:
# 1019
 return 20;_LL144: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp223=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp223->tag != 4)goto _LL146;}_LL145:
 return 130;_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp224=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp224->tag != 5)goto _LL148;}_LL147:
 return 30;_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp225->tag != 6)goto _LL14A;}_LL149:
 return 35;_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp226=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp226->tag != 7)goto _LL14C;}_LL14B:
 return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp227=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp227->tag != 8)goto _LL14E;}_LL14D:
 return 10;_LL14E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp228->tag != 9)goto _LL150;}_LL14F:
 return 140;_LL150: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp229=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp229->tag != 10)goto _LL152;}_LL151:
 return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp22A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp22A->tag != 11)goto _LL154;else{_tmp22B=_tmp22A->f1;}}_LL153:
 return Cyc_Absynpp_exp_prec(_tmp22B);_LL154: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp22C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp22C->tag != 12)goto _LL156;else{_tmp22D=_tmp22C->f1;}}_LL155:
 return Cyc_Absynpp_exp_prec(_tmp22D);_LL156: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp22E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp22E->tag != 13)goto _LL158;}_LL157:
 return 120;_LL158: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp22F=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp22F->tag != 15)goto _LL15A;}_LL159:
 return 15;_LL15A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp230=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp230->tag != 14)goto _LL15C;}_LL15B:
 goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp231=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp231->tag != 16)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp232=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp232->tag != 17)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp233=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp233->tag != 38)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp234=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp234->tag != 39)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp235=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp235->tag != 37)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp236->tag != 18)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp237->tag != 19)goto _LL16A;}_LL169:
 return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp238=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp238->tag != 20)goto _LL16C;}_LL16B:
 goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp239->tag != 21)goto _LL16E;}_LL16D:
 goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23A->tag != 22)goto _LL170;}_LL16F:
 return 140;_LL170: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23B->tag != 23)goto _LL172;}_LL171:
 return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp23C=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23C->tag != 24)goto _LL174;}_LL173:
 goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23D->tag != 25)goto _LL176;}_LL175:
 goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23E->tag != 26)goto _LL178;}_LL177:
 goto _LL179;_LL178: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp23F->tag != 27)goto _LL17A;}_LL179:
 goto _LL17B;_LL17A: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp240->tag != 28)goto _LL17C;}_LL17B:
 goto _LL17D;_LL17C: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp241->tag != 29)goto _LL17E;}_LL17D:
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp242->tag != 30)goto _LL180;}_LL17F:
 goto _LL181;_LL180: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp243->tag != 31)goto _LL182;}_LL181:
 goto _LL183;_LL182: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp244->tag != 32)goto _LL184;}_LL183:
 goto _LL185;_LL184: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp245->tag != 33)goto _LL186;}_LL185:
 goto _LL187;_LL186: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp246->tag != 34)goto _LL188;}_LL187:
 goto _LL189;_LL188: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp247->tag != 35)goto _LL18A;}_LL189:
 return 140;_LL18A: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp21D;if(_tmp248->tag != 36)goto _LL13B;}_LL18B:
 return 10000;_LL13B:;}
# 1059
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1063
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_stmttmp13=e->r;void*_tmp249=_stmttmp13;union Cyc_Absyn_Cnst _tmp24B;struct _tuple0*_tmp24D;enum Cyc_Absyn_Primop _tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Core_Opt*_tmp253;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Exp*_tmp256;enum Cyc_Absyn_Incrementor _tmp257;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_List_List*_tmp267;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;void*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp275;void*_tmp277;struct Cyc_Absyn_Exp*_tmp279;void*_tmp27B;int _tmp27D;struct _dyneither_ptr _tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct _dyneither_ptr*_tmp281;void*_tmp283;struct Cyc_List_List*_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct Cyc_Absyn_Exp*_tmp288;struct _dyneither_ptr*_tmp289;struct Cyc_Absyn_Exp*_tmp28B;struct _dyneither_ptr*_tmp28C;struct Cyc_Absyn_Exp*_tmp28E;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_List_List*_tmp291;struct _tuple8*_tmp293;struct Cyc_List_List*_tmp294;struct Cyc_List_List*_tmp296;struct Cyc_Absyn_Vardecl*_tmp298;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp29C;void*_tmp29D;struct _tuple0*_tmp29F;struct Cyc_List_List*_tmp2A0;struct Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*_tmp2A5;struct Cyc_Absyn_Datatypefield*_tmp2A6;struct _tuple0*_tmp2A8;struct _tuple0*_tmp2AA;int _tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;void**_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Core_Opt*_tmp2B4;struct Cyc_List_List*_tmp2B5;struct Cyc_Absyn_Stmt*_tmp2B7;_LL1A2: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LL1A4;else{_tmp24B=_tmp24A->f1;}}_LL1A3:
 s=Cyc_Absynpp_cnst2doc(_tmp24B);goto _LL1A1;_LL1A4: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24C->tag != 1)goto _LL1A6;else{_tmp24D=_tmp24C->f1;}}_LL1A5:
 s=Cyc_Absynpp_qvar2doc(_tmp24D);goto _LL1A1;_LL1A6: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24E->tag != 2)goto _LL1A8;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;}}_LL1A7:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp24F,_tmp250);goto _LL1A1;_LL1A8: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp251->tag != 3)goto _LL1AA;else{_tmp252=_tmp251->f1;_tmp253=_tmp251->f2;_tmp254=_tmp251->f3;}}_LL1A9:
# 1071
{const char*_tmp764;const char*_tmp763;struct Cyc_PP_Doc*_tmp762[5];s=((_tmp762[4]=
# 1075
Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),((_tmp762[3]=
# 1074
Cyc_PP_text(((_tmp763="= ",_tag_dyneither(_tmp763,sizeof(char),3)))),((_tmp762[2]=
# 1073
_tmp253 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp253->v),((_tmp762[1]=
# 1072
Cyc_PP_text(((_tmp764=" ",_tag_dyneither(_tmp764,sizeof(char),2)))),((_tmp762[0]=
# 1071
Cyc_Absynpp_exp2doc_prec(myprec,_tmp252),Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1076
goto _LL1A1;_LL1AA: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp255->tag != 4)goto _LL1AC;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}_LL1AB: {
# 1078
struct Cyc_PP_Doc*_tmp2BB=Cyc_Absynpp_exp2doc_prec(myprec,_tmp256);
switch(_tmp257){case Cyc_Absyn_PreInc: _LL1F2:
{const char*_tmp767;struct Cyc_PP_Doc*_tmp766[2];s=((_tmp766[1]=_tmp2BB,((_tmp766[0]=Cyc_PP_text(((_tmp767="++",_tag_dyneither(_tmp767,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F3:
{const char*_tmp76A;struct Cyc_PP_Doc*_tmp769[2];s=((_tmp769[1]=_tmp2BB,((_tmp769[0]=Cyc_PP_text(((_tmp76A="--",_tag_dyneither(_tmp76A,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp769,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F4:
{const char*_tmp76D;struct Cyc_PP_Doc*_tmp76C[2];s=((_tmp76C[1]=Cyc_PP_text(((_tmp76D="++",_tag_dyneither(_tmp76D,sizeof(char),3)))),((_tmp76C[0]=_tmp2BB,Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F5:
{const char*_tmp770;struct Cyc_PP_Doc*_tmp76F[2];s=((_tmp76F[1]=Cyc_PP_text(((_tmp770="--",_tag_dyneither(_tmp770,sizeof(char),3)))),((_tmp76F[0]=_tmp2BB,Cyc_PP_cat(_tag_dyneither(_tmp76F,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1085
goto _LL1A1;}_LL1AC: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp258->tag != 5)goto _LL1AE;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;_tmp25B=_tmp258->f3;}}_LL1AD:
# 1087
{const char*_tmp775;const char*_tmp774;struct Cyc_PP_Doc*_tmp773[5];s=((_tmp773[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),((_tmp773[3]=Cyc_PP_text(((_tmp774=" : ",_tag_dyneither(_tmp774,sizeof(char),4)))),((_tmp773[2]=
# 1087
Cyc_Absynpp_exp2doc_prec(0,_tmp25A),((_tmp773[1]=Cyc_PP_text(((_tmp775=" ? ",_tag_dyneither(_tmp775,sizeof(char),4)))),((_tmp773[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp259),Cyc_PP_cat(_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1089
goto _LL1A1;_LL1AE: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp25C->tag != 6)goto _LL1B0;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}_LL1AF:
# 1091
{const char*_tmp778;struct Cyc_PP_Doc*_tmp777[3];s=((_tmp777[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25E),((_tmp777[1]=Cyc_PP_text(((_tmp778=" && ",_tag_dyneither(_tmp778,sizeof(char),5)))),((_tmp777[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),Cyc_PP_cat(_tag_dyneither(_tmp777,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1B0: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp25F->tag != 7)goto _LL1B2;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}_LL1B1:
# 1094
{const char*_tmp77B;struct Cyc_PP_Doc*_tmp77A[3];s=((_tmp77A[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp261),((_tmp77A[1]=Cyc_PP_text(((_tmp77B=" || ",_tag_dyneither(_tmp77B,sizeof(char),5)))),((_tmp77A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),Cyc_PP_cat(_tag_dyneither(_tmp77A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1B2: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp262->tag != 8)goto _LL1B4;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}_LL1B3:
# 1099
{const char*_tmp782;const char*_tmp781;const char*_tmp780;struct Cyc_PP_Doc*_tmp77F[5];s=((_tmp77F[4]=Cyc_PP_text(((_tmp780=")",_tag_dyneither(_tmp780,sizeof(char),2)))),((_tmp77F[3]=Cyc_Absynpp_exp2doc(_tmp264),((_tmp77F[2]=Cyc_PP_text(((_tmp781=", ",_tag_dyneither(_tmp781,sizeof(char),3)))),((_tmp77F[1]=Cyc_Absynpp_exp2doc(_tmp263),((_tmp77F[0]=Cyc_PP_text(((_tmp782="(",_tag_dyneither(_tmp782,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A1;_LL1B4: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp265->tag != 9)goto _LL1B6;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}_LL1B5:
# 1102
{const char*_tmp787;const char*_tmp786;struct Cyc_PP_Doc*_tmp785[4];s=((_tmp785[3]=
# 1105
Cyc_PP_text(((_tmp786=")",_tag_dyneither(_tmp786,sizeof(char),2)))),((_tmp785[2]=
# 1104
Cyc_Absynpp_exps2doc_prec(20,_tmp267),((_tmp785[1]=
# 1103
Cyc_PP_text(((_tmp787="(",_tag_dyneither(_tmp787,sizeof(char),2)))),((_tmp785[0]=
# 1102
Cyc_Absynpp_exp2doc_prec(myprec,_tmp266),Cyc_PP_cat(_tag_dyneither(_tmp785,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1106
goto _LL1A1;_LL1B6: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp268->tag != 10)goto _LL1B8;else{_tmp269=_tmp268->f1;}}_LL1B7:
# 1108
{const char*_tmp78A;struct Cyc_PP_Doc*_tmp789[2];s=((_tmp789[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp269),((_tmp789[0]=Cyc_PP_text(((_tmp78A="throw ",_tag_dyneither(_tmp78A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp789,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A1;_LL1B8: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26A->tag != 11)goto _LL1BA;else{_tmp26B=_tmp26A->f1;}}_LL1B9:
# 1111
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26B);
goto _LL1A1;_LL1BA: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26C->tag != 12)goto _LL1BC;else{_tmp26D=_tmp26C->f1;}}_LL1BB:
# 1114
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26D);
goto _LL1A1;_LL1BC: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26E->tag != 13)goto _LL1BE;else{_tmp26F=(void*)_tmp26E->f1;_tmp270=_tmp26E->f2;}}_LL1BD:
# 1117
{const char*_tmp78F;const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[4];s=((_tmp78D[3]=
# 1120
Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),((_tmp78D[2]=
# 1119
Cyc_PP_text(((_tmp78E=")",_tag_dyneither(_tmp78E,sizeof(char),2)))),((_tmp78D[1]=
# 1118
Cyc_Absynpp_typ2doc(_tmp26F),((_tmp78D[0]=
# 1117
Cyc_PP_text(((_tmp78F="(",_tag_dyneither(_tmp78F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1121
goto _LL1A1;_LL1BE: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp271->tag != 14)goto _LL1C0;else{_tmp272=_tmp271->f1;}}_LL1BF:
# 1123
{const char*_tmp792;struct Cyc_PP_Doc*_tmp791[2];s=((_tmp791[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp791[0]=
# 1123
Cyc_PP_text(((_tmp792="&",_tag_dyneither(_tmp792,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp791,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1125
goto _LL1A1;_LL1C0: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp273->tag != 15)goto _LL1C2;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;}}_LL1C1:
# 1127
 if(_tmp274 == 0){
const char*_tmp795;struct Cyc_PP_Doc*_tmp794[2];s=((_tmp794[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp794[0]=Cyc_PP_text(((_tmp795="new ",_tag_dyneither(_tmp795,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp794,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1130
const char*_tmp79A;const char*_tmp799;struct Cyc_PP_Doc*_tmp798[4];s=((_tmp798[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp798[2]=
# 1130
Cyc_PP_text(((_tmp799=") ",_tag_dyneither(_tmp799,sizeof(char),3)))),((_tmp798[1]=Cyc_Absynpp_exp2doc(_tmp274),((_tmp798[0]=Cyc_PP_text(((_tmp79A="rnew(",_tag_dyneither(_tmp79A,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp798,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1132
goto _LL1A1;_LL1C2: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp276=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp276->tag != 16)goto _LL1C4;else{_tmp277=(void*)_tmp276->f1;}}_LL1C3:
{const char*_tmp79F;const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[3];s=((_tmp79D[2]=Cyc_PP_text(((_tmp79E=")",_tag_dyneither(_tmp79E,sizeof(char),2)))),((_tmp79D[1]=Cyc_Absynpp_typ2doc(_tmp277),((_tmp79D[0]=Cyc_PP_text(((_tmp79F="sizeof(",_tag_dyneither(_tmp79F,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1C4: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp278->tag != 17)goto _LL1C6;else{_tmp279=_tmp278->f1;}}_LL1C5:
{const char*_tmp7A4;const char*_tmp7A3;struct Cyc_PP_Doc*_tmp7A2[3];s=((_tmp7A2[2]=Cyc_PP_text(((_tmp7A3=")",_tag_dyneither(_tmp7A3,sizeof(char),2)))),((_tmp7A2[1]=Cyc_Absynpp_exp2doc(_tmp279),((_tmp7A2[0]=Cyc_PP_text(((_tmp7A4="sizeof(",_tag_dyneither(_tmp7A4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A2,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1C6: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27A->tag != 38)goto _LL1C8;else{_tmp27B=(void*)_tmp27A->f1;}}_LL1C7:
{const char*_tmp7A9;const char*_tmp7A8;struct Cyc_PP_Doc*_tmp7A7[3];s=((_tmp7A7[2]=Cyc_PP_text(((_tmp7A8=")",_tag_dyneither(_tmp7A8,sizeof(char),2)))),((_tmp7A7[1]=Cyc_Absynpp_typ2doc(_tmp27B),((_tmp7A7[0]=Cyc_PP_text(((_tmp7A9="valueof(",_tag_dyneither(_tmp7A9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A1;_LL1C8: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27C->tag != 39)goto _LL1CA;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}_LL1C9:
# 1137
 if(_tmp27D){
const char*_tmp7B0;const char*_tmp7AF;const char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[4];s=((_tmp7AD[3]=Cyc_PP_text(((_tmp7AE=")",_tag_dyneither(_tmp7AE,sizeof(char),2)))),((_tmp7AD[2]=Cyc_PP_text(_tmp27E),((_tmp7AD[1]=Cyc_PP_text(((_tmp7AF=" volatile (",_tag_dyneither(_tmp7AF,sizeof(char),12)))),((_tmp7AD[0]=Cyc_PP_text(((_tmp7B0="__asm__",_tag_dyneither(_tmp7B0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1140
const char*_tmp7B5;const char*_tmp7B4;struct Cyc_PP_Doc*_tmp7B3[3];s=((_tmp7B3[2]=Cyc_PP_text(((_tmp7B4=")",_tag_dyneither(_tmp7B4,sizeof(char),2)))),((_tmp7B3[1]=Cyc_PP_text(_tmp27E),((_tmp7B3[0]=Cyc_PP_text(((_tmp7B5="__asm__(",_tag_dyneither(_tmp7B5,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7B3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1CA: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp27F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27F->tag != 37)goto _LL1CC;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}_LL1CB:
# 1143
{const char*_tmp7BC;const char*_tmp7BB;const char*_tmp7BA;struct Cyc_PP_Doc*_tmp7B9[5];s=((_tmp7B9[4]=Cyc_PP_text(((_tmp7BA=")",_tag_dyneither(_tmp7BA,sizeof(char),2)))),((_tmp7B9[3]=Cyc_PP_textptr(_tmp281),((_tmp7B9[2]=Cyc_PP_text(((_tmp7BB=".",_tag_dyneither(_tmp7BB,sizeof(char),2)))),((_tmp7B9[1]=Cyc_Absynpp_exp2doc(_tmp280),((_tmp7B9[0]=Cyc_PP_text(((_tmp7BC="tagcheck(",_tag_dyneither(_tmp7BC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A1;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp282=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp282->tag != 18)goto _LL1CE;else{_tmp283=(void*)_tmp282->f1;_tmp284=_tmp282->f2;}}_LL1CD:
# 1146
{const char*_tmp7C1;const char*_tmp7C0;struct Cyc_PP_Doc*_tmp7BF[3];s=((_tmp7BF[2]=Cyc_PP_text(((_tmp7C0=",",_tag_dyneither(_tmp7C0,sizeof(char),2)))),((_tmp7BF[1]=Cyc_Absynpp_typ2doc(_tmp283),((_tmp7BF[0]=Cyc_PP_text(((_tmp7C1="offsetof(",_tag_dyneither(_tmp7C1,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7BF,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp284 != 0;_tmp284=_tmp284->tl){
void*_stmttmp14=(void*)_tmp284->hd;void*_tmp2F5=_stmttmp14;struct _dyneither_ptr*_tmp2F7;unsigned int _tmp2F9;_LL1F8: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2F6=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F6->tag != 0)goto _LL1FA;else{_tmp2F7=_tmp2F6->f1;}}_LL1F9:
{const char*_tmp7C4;struct Cyc_PP_Doc*_tmp7C3[3];s=((_tmp7C3[2]=
_tmp284->tl != 0?Cyc_PP_text(((_tmp7C4=".",_tag_dyneither(_tmp7C4,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7C3[1]=
# 1149
Cyc_PP_textptr(_tmp2F7),((_tmp7C3[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7C3,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1151
goto _LL1F7;_LL1FA: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2F8=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2F5;if(_tmp2F8->tag != 1)goto _LL1F7;else{_tmp2F9=_tmp2F8->f1;}}_LL1FB:
{struct Cyc_Int_pa_PrintArg_struct _tmp7CE;void*_tmp7CD[1];const char*_tmp7CC;const char*_tmp7CB;struct Cyc_PP_Doc*_tmp7CA[3];s=((_tmp7CA[2]=
_tmp284->tl != 0?Cyc_PP_text(((_tmp7CB=".",_tag_dyneither(_tmp7CB,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7CA[1]=
# 1152
Cyc_PP_text((struct _dyneither_ptr)((_tmp7CE.tag=1,((_tmp7CE.f1=(unsigned long)((int)_tmp2F9),((_tmp7CD[0]=& _tmp7CE,Cyc_aprintf(((_tmp7CC="%d",_tag_dyneither(_tmp7CC,sizeof(char),3))),_tag_dyneither(_tmp7CD,sizeof(void*),1))))))))),((_tmp7CA[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7CA,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1154
goto _LL1F7;_LL1F7:;}
# 1156
{const char*_tmp7D1;struct Cyc_PP_Doc*_tmp7D0[2];s=((_tmp7D0[1]=Cyc_PP_text(((_tmp7D1=")",_tag_dyneither(_tmp7D1,sizeof(char),2)))),((_tmp7D0[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7D0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A1;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp285=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp285->tag != 19)goto _LL1D0;else{_tmp286=_tmp285->f1;}}_LL1CF:
# 1159
{const char*_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[2];s=((_tmp7D3[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp286),((_tmp7D3[0]=Cyc_PP_text(((_tmp7D4="*",_tag_dyneither(_tmp7D4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A1;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp287=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp287->tag != 20)goto _LL1D2;else{_tmp288=_tmp287->f1;_tmp289=_tmp287->f2;}}_LL1D1:
# 1162
{const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[3];s=((_tmp7D6[2]=Cyc_PP_textptr(_tmp289),((_tmp7D6[1]=Cyc_PP_text(((_tmp7D7=".",_tag_dyneither(_tmp7D7,sizeof(char),2)))),((_tmp7D6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),Cyc_PP_cat(_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp28A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp28A->tag != 21)goto _LL1D4;else{_tmp28B=_tmp28A->f1;_tmp28C=_tmp28A->f2;}}_LL1D3:
# 1165
{const char*_tmp7DA;struct Cyc_PP_Doc*_tmp7D9[3];s=((_tmp7D9[2]=Cyc_PP_textptr(_tmp28C),((_tmp7D9[1]=Cyc_PP_text(((_tmp7DA="->",_tag_dyneither(_tmp7DA,sizeof(char),3)))),((_tmp7D9[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28B),Cyc_PP_cat(_tag_dyneither(_tmp7D9,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A1;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp28D->tag != 22)goto _LL1D6;else{_tmp28E=_tmp28D->f1;_tmp28F=_tmp28D->f2;}}_LL1D5:
# 1168
{const char*_tmp7DF;const char*_tmp7DE;struct Cyc_PP_Doc*_tmp7DD[4];s=((_tmp7DD[3]=
# 1171
Cyc_PP_text(((_tmp7DE="]",_tag_dyneither(_tmp7DE,sizeof(char),2)))),((_tmp7DD[2]=
# 1170
Cyc_Absynpp_exp2doc(_tmp28F),((_tmp7DD[1]=
# 1169
Cyc_PP_text(((_tmp7DF="[",_tag_dyneither(_tmp7DF,sizeof(char),2)))),((_tmp7DD[0]=
# 1168
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28E),Cyc_PP_cat(_tag_dyneither(_tmp7DD,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1172
goto _LL1A1;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp290=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp290->tag != 23)goto _LL1D8;else{_tmp291=_tmp290->f1;}}_LL1D7:
# 1174
{const char*_tmp7E4;const char*_tmp7E3;struct Cyc_PP_Doc*_tmp7E2[4];s=((_tmp7E2[3]=
# 1177
Cyc_PP_text(((_tmp7E3=")",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E2[2]=
# 1176
Cyc_Absynpp_exps2doc_prec(20,_tmp291),((_tmp7E2[1]=
# 1175
Cyc_PP_text(((_tmp7E4="(",_tag_dyneither(_tmp7E4,sizeof(char),2)))),((_tmp7E2[0]=
# 1174
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7E2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1178
goto _LL1A1;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp292->tag != 24)goto _LL1DA;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}_LL1D9:
# 1180
{const char*_tmp7EB;const char*_tmp7EA;const char*_tmp7E9;struct Cyc_PP_Doc*_tmp7E8[4];s=((_tmp7E8[3]=
# 1183
Cyc_Absynpp_group_braces(((_tmp7E9=",",_tag_dyneither(_tmp7E9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp294)),((_tmp7E8[2]=
# 1182
Cyc_PP_text(((_tmp7EA=")",_tag_dyneither(_tmp7EA,sizeof(char),2)))),((_tmp7E8[1]=
# 1181
Cyc_Absynpp_typ2doc((*_tmp293).f3),((_tmp7E8[0]=
# 1180
Cyc_PP_text(((_tmp7EB="(",_tag_dyneither(_tmp7EB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1184
goto _LL1A1;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp295->tag != 25)goto _LL1DC;else{_tmp296=_tmp295->f1;}}_LL1DB:
# 1186
{const char*_tmp7EC;s=Cyc_Absynpp_group_braces(((_tmp7EC=",",_tag_dyneither(_tmp7EC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp296));}
goto _LL1A1;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp297->tag != 26)goto _LL1DE;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;_tmp29A=_tmp297->f3;}}_LL1DD:
# 1189
{const char*_tmp7F3;const char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[8];s=((_tmp7F0[7]=
# 1195
Cyc_Absynpp_rb(),((_tmp7F0[6]=
# 1194
Cyc_Absynpp_exp2doc(_tmp29A),((_tmp7F0[5]=
# 1193
Cyc_PP_text(((_tmp7F1=" : ",_tag_dyneither(_tmp7F1,sizeof(char),4)))),((_tmp7F0[4]=
# 1192
Cyc_Absynpp_exp2doc(_tmp299),((_tmp7F0[3]=
# 1191
Cyc_PP_text(((_tmp7F2=" < ",_tag_dyneither(_tmp7F2,sizeof(char),4)))),((_tmp7F0[2]=
# 1190
Cyc_PP_text(*(*_tmp298->name).f2),((_tmp7F0[1]=
# 1189
Cyc_PP_text(((_tmp7F3="for ",_tag_dyneither(_tmp7F3,sizeof(char),5)))),((_tmp7F0[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1196
goto _LL1A1;_LL1DE: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp29B=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp29B->tag != 27)goto _LL1E0;else{_tmp29C=_tmp29B->f1;_tmp29D=(void*)_tmp29B->f2;}}_LL1DF:
# 1198
{const char*_tmp7FA;const char*_tmp7F9;const char*_tmp7F8;struct Cyc_PP_Doc*_tmp7F7[7];s=((_tmp7F7[6]=
# 1203
Cyc_Absynpp_rb(),((_tmp7F7[5]=
# 1202
Cyc_Absynpp_typ2doc(_tmp29D),((_tmp7F7[4]=
# 1201
Cyc_PP_text(((_tmp7F8=" : ",_tag_dyneither(_tmp7F8,sizeof(char),4)))),((_tmp7F7[3]=
# 1200
Cyc_Absynpp_exp2doc(_tmp29C),((_tmp7F7[2]=
# 1199
Cyc_PP_text(((_tmp7F9=" < ",_tag_dyneither(_tmp7F9,sizeof(char),4)))),((_tmp7F7[1]=
# 1198
Cyc_PP_text(((_tmp7FA="for x ",_tag_dyneither(_tmp7FA,sizeof(char),7)))),((_tmp7F7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7F7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1204
goto _LL1A1;_LL1E0: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp29E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp29E->tag != 28)goto _LL1E2;else{_tmp29F=_tmp29E->f1;_tmp2A0=_tmp29E->f2;_tmp2A1=_tmp29E->f3;}}_LL1E1: {
# 1206
struct Cyc_List_List*_tmp31C=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A1);
{const char*_tmp7FF;struct Cyc_List_List*_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[2];s=((_tmp7FD[1]=
Cyc_Absynpp_group_braces(((_tmp7FF=",",_tag_dyneither(_tmp7FF,sizeof(char),2))),
_tmp2A0 != 0?(_tmp7FE=_cycalloc(sizeof(*_tmp7FE)),((_tmp7FE->hd=Cyc_Absynpp_tps2doc(_tmp2A0),((_tmp7FE->tl=_tmp31C,_tmp7FE))))): _tmp31C),((_tmp7FD[0]=
# 1207
Cyc_Absynpp_qvar2doc(_tmp29F),Cyc_PP_cat(_tag_dyneither(_tmp7FD,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1210
goto _LL1A1;}_LL1E2: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A2->tag != 29)goto _LL1E4;else{_tmp2A3=_tmp2A2->f2;}}_LL1E3:
# 1212
{const char*_tmp800;s=Cyc_Absynpp_group_braces(((_tmp800=",",_tag_dyneither(_tmp800,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A3));}
goto _LL1A1;_LL1E4: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A4=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A4->tag != 30)goto _LL1E6;else{_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A4->f3;}}_LL1E5:
# 1215
 if(_tmp2A5 == 0)
# 1217
s=Cyc_Absynpp_qvar2doc(_tmp2A6->name);else{
# 1219
const char*_tmp807;const char*_tmp806;const char*_tmp805;struct Cyc_PP_Doc*_tmp804[2];s=((_tmp804[1]=
Cyc_PP_egroup(((_tmp807="(",_tag_dyneither(_tmp807,sizeof(char),2))),((_tmp806=")",_tag_dyneither(_tmp806,sizeof(char),2))),((_tmp805=",",_tag_dyneither(_tmp805,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A5)),((_tmp804[0]=
# 1219
Cyc_Absynpp_qvar2doc(_tmp2A6->name),Cyc_PP_cat(_tag_dyneither(_tmp804,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1221
goto _LL1A1;_LL1E6: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2A7=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A7->tag != 31)goto _LL1E8;else{_tmp2A8=_tmp2A7->f1;}}_LL1E7:
# 1223
 s=Cyc_Absynpp_qvar2doc(_tmp2A8);
goto _LL1A1;_LL1E8: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2A9=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A9->tag != 32)goto _LL1EA;else{_tmp2AA=_tmp2A9->f1;}}_LL1E9:
# 1226
 s=Cyc_Absynpp_qvar2doc(_tmp2AA);
goto _LL1A1;_LL1EA: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2AB->tag != 33)goto _LL1EC;else{_tmp2AC=(_tmp2AB->f1).is_calloc;_tmp2AD=(_tmp2AB->f1).rgn;_tmp2AE=(_tmp2AB->f1).elt_type;_tmp2AF=(_tmp2AB->f1).num_elts;}}_LL1EB:
# 1231
 if(_tmp2AC){
# 1233
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2AE)),0);
if(_tmp2AD == 0){
const char*_tmp80E;const char*_tmp80D;const char*_tmp80C;struct Cyc_PP_Doc*_tmp80B[5];s=((_tmp80B[4]=Cyc_PP_text(((_tmp80C=")",_tag_dyneither(_tmp80C,sizeof(char),2)))),((_tmp80B[3]=Cyc_Absynpp_exp2doc(st),((_tmp80B[2]=Cyc_PP_text(((_tmp80D=",",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80B[1]=Cyc_Absynpp_exp2doc(_tmp2AF),((_tmp80B[0]=Cyc_PP_text(((_tmp80E="calloc(",_tag_dyneither(_tmp80E,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp80B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1237
const char*_tmp817;const char*_tmp816;const char*_tmp815;const char*_tmp814;struct Cyc_PP_Doc*_tmp813[7];s=((_tmp813[6]=
Cyc_PP_text(((_tmp814=")",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp813[5]=Cyc_Absynpp_exp2doc(st),((_tmp813[4]=Cyc_PP_text(((_tmp815=",",_tag_dyneither(_tmp815,sizeof(char),2)))),((_tmp813[3]=Cyc_Absynpp_exp2doc(_tmp2AF),((_tmp813[2]=
# 1237
Cyc_PP_text(((_tmp816=",",_tag_dyneither(_tmp816,sizeof(char),2)))),((_tmp813[1]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp813[0]=Cyc_PP_text(((_tmp817="rcalloc(",_tag_dyneither(_tmp817,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp813,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1240
struct Cyc_Absyn_Exp*new_e;
# 1242
if(_tmp2AE == 0)
new_e=_tmp2AF;else{
# 1245
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2AE,0),_tmp2AF,0);}
# 1247
if(_tmp2AD == 0){
const char*_tmp81C;const char*_tmp81B;struct Cyc_PP_Doc*_tmp81A[3];s=((_tmp81A[2]=Cyc_PP_text(((_tmp81B=")",_tag_dyneither(_tmp81B,sizeof(char),2)))),((_tmp81A[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp81A[0]=Cyc_PP_text(((_tmp81C="malloc(",_tag_dyneither(_tmp81C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp81A,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1250
const char*_tmp823;const char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*_tmp820[5];s=((_tmp820[4]=
Cyc_PP_text(((_tmp821=")",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp820[2]=
# 1250
Cyc_PP_text(((_tmp822=",",_tag_dyneither(_tmp822,sizeof(char),2)))),((_tmp820[1]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp820[0]=Cyc_PP_text(((_tmp823="rmalloc(",_tag_dyneither(_tmp823,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1253
goto _LL1A1;_LL1EC: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2B0=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B0->tag != 34)goto _LL1EE;else{_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;}}_LL1ED:
# 1255
{const char*_tmp826;struct Cyc_PP_Doc*_tmp825[3];s=((_tmp825[2]=
# 1257
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B2),((_tmp825[1]=
# 1256
Cyc_PP_text(((_tmp826=" :=: ",_tag_dyneither(_tmp826,sizeof(char),6)))),((_tmp825[0]=
# 1255
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B1),Cyc_PP_cat(_tag_dyneither(_tmp825,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1258
goto _LL1A1;_LL1EE: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B3=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B3->tag != 35)goto _LL1F0;else{_tmp2B4=_tmp2B3->f1;_tmp2B5=_tmp2B3->f2;}}_LL1EF:
# 1261
{const char*_tmp827;s=Cyc_Absynpp_group_braces(((_tmp827=",",_tag_dyneither(_tmp827,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B5));}
goto _LL1A1;_LL1F0: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2B6=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B6->tag != 36)goto _LL1A1;else{_tmp2B7=_tmp2B6->f1;}}_LL1F1:
# 1264
{const char*_tmp82C;const char*_tmp82B;struct Cyc_PP_Doc*_tmp82A[7];s=((_tmp82A[6]=
# 1266
Cyc_PP_text(((_tmp82B=")",_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp82A[5]=Cyc_Absynpp_rb(),((_tmp82A[4]=Cyc_PP_blank_doc(),((_tmp82A[3]=
# 1265
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B7,1)),((_tmp82A[2]=
# 1264
Cyc_PP_blank_doc(),((_tmp82A[1]=Cyc_Absynpp_lb(),((_tmp82A[0]=Cyc_PP_text(((_tmp82C="(",_tag_dyneither(_tmp82C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1267
goto _LL1A1;_LL1A1:;}
# 1269
if(inprec >= myprec){
const char*_tmp831;const char*_tmp830;struct Cyc_PP_Doc*_tmp82F[3];s=((_tmp82F[2]=Cyc_PP_text(((_tmp830=")",_tag_dyneither(_tmp830,sizeof(char),2)))),((_tmp82F[1]=s,((_tmp82F[0]=Cyc_PP_text(((_tmp831="(",_tag_dyneither(_tmp831,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1274
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp33E=d;struct Cyc_Absyn_Exp*_tmp340;struct _dyneither_ptr*_tmp342;_LL1FD: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33F=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33E;if(_tmp33F->tag != 0)goto _LL1FF;else{_tmp340=_tmp33F->f1;}}_LL1FE: {
const char*_tmp836;const char*_tmp835;struct Cyc_PP_Doc*_tmp834[3];return(_tmp834[2]=Cyc_PP_text(((_tmp835="]",_tag_dyneither(_tmp835,sizeof(char),2)))),((_tmp834[1]=Cyc_Absynpp_exp2doc(_tmp340),((_tmp834[0]=Cyc_PP_text(((_tmp836=".[",_tag_dyneither(_tmp836,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp834,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FF: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp341=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33E;if(_tmp341->tag != 1)goto _LL1FC;else{_tmp342=_tmp341->f1;}}_LL200: {
const char*_tmp839;struct Cyc_PP_Doc*_tmp838[2];return(_tmp838[1]=Cyc_PP_textptr(_tmp342),((_tmp838[0]=Cyc_PP_text(((_tmp839=".",_tag_dyneither(_tmp839,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp838,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1FC:;}
# 1281
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp840;const char*_tmp83F;const char*_tmp83E;struct Cyc_PP_Doc*_tmp83D[2];return(_tmp83D[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp83D[0]=
# 1283
Cyc_PP_egroup(((_tmp840="",_tag_dyneither(_tmp840,sizeof(char),1))),((_tmp83F="=",_tag_dyneither(_tmp83F,sizeof(char),2))),((_tmp83E="=",_tag_dyneither(_tmp83E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp83D,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1287
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp843;const char*_tmp842;const char*_tmp841;return Cyc_PP_group(((_tmp841="",_tag_dyneither(_tmp841,sizeof(char),1))),((_tmp842="",_tag_dyneither(_tmp842,sizeof(char),1))),((_tmp843=",",_tag_dyneither(_tmp843,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1292
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp34F=c;enum Cyc_Absyn_Sign _tmp350;char _tmp351;struct _dyneither_ptr _tmp352;enum Cyc_Absyn_Sign _tmp353;short _tmp354;int _tmp355;int _tmp356;int _tmp357;enum Cyc_Absyn_Sign _tmp358;long long _tmp359;struct _dyneither_ptr _tmp35A;struct _dyneither_ptr _tmp35B;struct _dyneither_ptr _tmp35C;_LL202: if((_tmp34F.Char_c).tag != 2)goto _LL204;_tmp350=((struct _tuple3)(_tmp34F.Char_c).val).f1;_tmp351=((struct _tuple3)(_tmp34F.Char_c).val).f2;_LL203: {
const char*_tmp847;void*_tmp846[1];struct Cyc_String_pa_PrintArg_struct _tmp845;return Cyc_PP_text((struct _dyneither_ptr)((_tmp845.tag=0,((_tmp845.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp351)),((_tmp846[0]=& _tmp845,Cyc_aprintf(((_tmp847="'%s'",_tag_dyneither(_tmp847,sizeof(char),5))),_tag_dyneither(_tmp846,sizeof(void*),1)))))))));}_LL204: if((_tmp34F.Wchar_c).tag != 3)goto _LL206;_tmp352=(struct _dyneither_ptr)(_tmp34F.Wchar_c).val;_LL205: {
const char*_tmp84B;void*_tmp84A[1];struct Cyc_String_pa_PrintArg_struct _tmp849;return Cyc_PP_text((struct _dyneither_ptr)((_tmp849.tag=0,((_tmp849.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352),((_tmp84A[0]=& _tmp849,Cyc_aprintf(((_tmp84B="L'%s'",_tag_dyneither(_tmp84B,sizeof(char),6))),_tag_dyneither(_tmp84A,sizeof(void*),1)))))))));}_LL206: if((_tmp34F.Short_c).tag != 4)goto _LL208;_tmp353=((struct _tuple4)(_tmp34F.Short_c).val).f1;_tmp354=((struct _tuple4)(_tmp34F.Short_c).val).f2;_LL207: {
const char*_tmp84F;void*_tmp84E[1];struct Cyc_Int_pa_PrintArg_struct _tmp84D;return Cyc_PP_text((struct _dyneither_ptr)((_tmp84D.tag=1,((_tmp84D.f1=(unsigned long)((int)_tmp354),((_tmp84E[0]=& _tmp84D,Cyc_aprintf(((_tmp84F="%d",_tag_dyneither(_tmp84F,sizeof(char),3))),_tag_dyneither(_tmp84E,sizeof(void*),1)))))))));}_LL208: if((_tmp34F.Int_c).tag != 5)goto _LL20A;if(((struct _tuple5)(_tmp34F.Int_c).val).f1 != Cyc_Absyn_None)goto _LL20A;_tmp355=((struct _tuple5)(_tmp34F.Int_c).val).f2;_LL209:
 _tmp356=_tmp355;goto _LL20B;_LL20A: if((_tmp34F.Int_c).tag != 5)goto _LL20C;if(((struct _tuple5)(_tmp34F.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL20C;_tmp356=((struct _tuple5)(_tmp34F.Int_c).val).f2;_LL20B: {
const char*_tmp853;void*_tmp852[1];struct Cyc_Int_pa_PrintArg_struct _tmp851;return Cyc_PP_text((struct _dyneither_ptr)((_tmp851.tag=1,((_tmp851.f1=(unsigned long)_tmp356,((_tmp852[0]=& _tmp851,Cyc_aprintf(((_tmp853="%d",_tag_dyneither(_tmp853,sizeof(char),3))),_tag_dyneither(_tmp852,sizeof(void*),1)))))))));}_LL20C: if((_tmp34F.Int_c).tag != 5)goto _LL20E;if(((struct _tuple5)(_tmp34F.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL20E;_tmp357=((struct _tuple5)(_tmp34F.Int_c).val).f2;_LL20D: {
const char*_tmp857;void*_tmp856[1];struct Cyc_Int_pa_PrintArg_struct _tmp855;return Cyc_PP_text((struct _dyneither_ptr)((_tmp855.tag=1,((_tmp855.f1=(unsigned int)_tmp357,((_tmp856[0]=& _tmp855,Cyc_aprintf(((_tmp857="%u",_tag_dyneither(_tmp857,sizeof(char),3))),_tag_dyneither(_tmp856,sizeof(void*),1)))))))));}_LL20E: if((_tmp34F.LongLong_c).tag != 6)goto _LL210;_tmp358=((struct _tuple6)(_tmp34F.LongLong_c).val).f1;_tmp359=((struct _tuple6)(_tmp34F.LongLong_c).val).f2;_LL20F: {
# 1301
const char*_tmp858;return Cyc_PP_text(((_tmp858="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp858,sizeof(char),27))));}_LL210: if((_tmp34F.Float_c).tag != 7)goto _LL212;_tmp35A=((struct _tuple7)(_tmp34F.Float_c).val).f1;_LL211:
 return Cyc_PP_text(_tmp35A);_LL212: if((_tmp34F.Null_c).tag != 1)goto _LL214;_LL213: {
const char*_tmp859;return Cyc_PP_text(((_tmp859="NULL",_tag_dyneither(_tmp859,sizeof(char),5))));}_LL214: if((_tmp34F.String_c).tag != 8)goto _LL216;_tmp35B=(struct _dyneither_ptr)(_tmp34F.String_c).val;_LL215: {
# 1305
const char*_tmp85E;const char*_tmp85D;struct Cyc_PP_Doc*_tmp85C[3];return(_tmp85C[2]=Cyc_PP_text(((_tmp85D="\"",_tag_dyneither(_tmp85D,sizeof(char),2)))),((_tmp85C[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp35B)),((_tmp85C[0]=Cyc_PP_text(((_tmp85E="\"",_tag_dyneither(_tmp85E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp85C,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL216: if((_tmp34F.Wstring_c).tag != 9)goto _LL201;_tmp35C=(struct _dyneither_ptr)(_tmp34F.Wstring_c).val;_LL217: {
# 1307
const char*_tmp863;const char*_tmp862;struct Cyc_PP_Doc*_tmp861[3];return(_tmp861[2]=Cyc_PP_text(((_tmp862="\"",_tag_dyneither(_tmp862,sizeof(char),2)))),((_tmp861[1]=Cyc_PP_text(_tmp35C),((_tmp861[0]=Cyc_PP_text(((_tmp863="L\"",_tag_dyneither(_tmp863,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp861,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL201:;}
# 1311
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp870;const char*_tmp86F;void*_tmp86E[1];struct Cyc_String_pa_PrintArg_struct _tmp86D;struct Cyc_Core_Failure_exn_struct*_tmp86C;(int)_throw((void*)((_tmp86C=_cycalloc(sizeof(*_tmp86C)),((_tmp86C[0]=((_tmp870.tag=Cyc_Core_Failure,((_tmp870.f1=(struct _dyneither_ptr)((_tmp86D.tag=0,((_tmp86D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp86E[0]=& _tmp86D,Cyc_aprintf(((_tmp86F="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp86F,sizeof(char),47))),_tag_dyneither(_tmp86E,sizeof(void*),1)))))))),_tmp870)))),_tmp86C)))));}{
# 1317
const char*_tmp875;const char*_tmp874;struct Cyc_PP_Doc*_tmp873[3];return(_tmp873[2]=Cyc_PP_text(((_tmp874=")",_tag_dyneither(_tmp874,sizeof(char),2)))),((_tmp873[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp873[0]=Cyc_PP_text(((_tmp875="numelts(",_tag_dyneither(_tmp875,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp873,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1319
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp882;const char*_tmp881;void*_tmp880[1];struct Cyc_String_pa_PrintArg_struct _tmp87F;struct Cyc_Core_Failure_exn_struct*_tmp87E;(int)_throw((void*)((_tmp87E=_cycalloc(sizeof(*_tmp87E)),((_tmp87E[0]=((_tmp882.tag=Cyc_Core_Failure,((_tmp882.f1=(struct _dyneither_ptr)((_tmp87F.tag=0,((_tmp87F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp880[0]=& _tmp87F,Cyc_aprintf(((_tmp881="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp881,sizeof(char),38))),_tag_dyneither(_tmp880,sizeof(void*),1)))))))),_tmp882)))),_tmp87E)))));}else{
# 1323
if(ds->tl == 0){
const char*_tmp885;struct Cyc_PP_Doc*_tmp884[3];return(_tmp884[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp884[1]=Cyc_PP_text(((_tmp885=" ",_tag_dyneither(_tmp885,sizeof(char),2)))),((_tmp884[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp884,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp892;const char*_tmp891;void*_tmp890[1];struct Cyc_String_pa_PrintArg_struct _tmp88F;struct Cyc_Core_Failure_exn_struct*_tmp88E;(int)_throw((void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp892.tag=Cyc_Core_Failure,((_tmp892.f1=(struct _dyneither_ptr)((_tmp88F.tag=0,((_tmp88F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp890[0]=& _tmp88F,Cyc_aprintf(((_tmp891="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp891,sizeof(char),47))),_tag_dyneither(_tmp890,sizeof(void*),1)))))))),_tmp892)))),_tmp88E)))));}else{
# 1329
const char*_tmp897;const char*_tmp896;struct Cyc_PP_Doc*_tmp895[5];return(_tmp895[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp895[3]=Cyc_PP_text(((_tmp896=" ",_tag_dyneither(_tmp896,sizeof(char),2)))),((_tmp895[2]=ps,((_tmp895[1]=Cyc_PP_text(((_tmp897=" ",_tag_dyneither(_tmp897,sizeof(char),2)))),((_tmp895[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp895,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1333
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL218: {
const char*_tmp898;return(_tmp898="+",_tag_dyneither(_tmp898,sizeof(char),2));}case Cyc_Absyn_Times: _LL219: {
const char*_tmp899;return(_tmp899="*",_tag_dyneither(_tmp899,sizeof(char),2));}case Cyc_Absyn_Minus: _LL21A: {
const char*_tmp89A;return(_tmp89A="-",_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Div: _LL21B: {
const char*_tmp89B;return(_tmp89B="/",_tag_dyneither(_tmp89B,sizeof(char),2));}case Cyc_Absyn_Mod: _LL21C: {
const char*_tmp89D;const char*_tmp89C;return Cyc_Absynpp_print_for_cycdoc?(_tmp89D="\\%",_tag_dyneither(_tmp89D,sizeof(char),3)):((_tmp89C="%",_tag_dyneither(_tmp89C,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL21D: {
const char*_tmp89E;return(_tmp89E="==",_tag_dyneither(_tmp89E,sizeof(char),3));}case Cyc_Absyn_Neq: _LL21E: {
const char*_tmp89F;return(_tmp89F="!=",_tag_dyneither(_tmp89F,sizeof(char),3));}case Cyc_Absyn_Gt: _LL21F: {
const char*_tmp8A0;return(_tmp8A0=">",_tag_dyneither(_tmp8A0,sizeof(char),2));}case Cyc_Absyn_Lt: _LL220: {
const char*_tmp8A1;return(_tmp8A1="<",_tag_dyneither(_tmp8A1,sizeof(char),2));}case Cyc_Absyn_Gte: _LL221: {
const char*_tmp8A2;return(_tmp8A2=">=",_tag_dyneither(_tmp8A2,sizeof(char),3));}case Cyc_Absyn_Lte: _LL222: {
const char*_tmp8A3;return(_tmp8A3="<=",_tag_dyneither(_tmp8A3,sizeof(char),3));}case Cyc_Absyn_Not: _LL223: {
const char*_tmp8A4;return(_tmp8A4="!",_tag_dyneither(_tmp8A4,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL224: {
const char*_tmp8A5;return(_tmp8A5="~",_tag_dyneither(_tmp8A5,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL225: {
const char*_tmp8A6;return(_tmp8A6="&",_tag_dyneither(_tmp8A6,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL226: {
const char*_tmp8A7;return(_tmp8A7="|",_tag_dyneither(_tmp8A7,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL227: {
const char*_tmp8A8;return(_tmp8A8="^",_tag_dyneither(_tmp8A8,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL228: {
const char*_tmp8A9;return(_tmp8A9="<<",_tag_dyneither(_tmp8A9,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL229: {
const char*_tmp8AA;return(_tmp8AA=">>",_tag_dyneither(_tmp8AA,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL22A: {
const char*_tmp8AB;return(_tmp8AB=">>>",_tag_dyneither(_tmp8AB,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL22B: {
const char*_tmp8AC;return(_tmp8AC="numelts",_tag_dyneither(_tmp8AC,sizeof(char),8));}}}
# 1358
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1362
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_stmttmp15=s->r;void*_tmp3A0=_stmttmp15;_LL22E: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3A1=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A1->tag != 12)goto _LL230;}_LL22F:
 return 1;_LL230:;_LL231:
 return 0;_LL22D:;}
# 1369
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_stmttmp16=st->r;void*_tmp3A2=_stmttmp16;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A7;struct Cyc_Absyn_Stmt*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Stmt*_tmp3AD;struct Cyc_Absyn_Stmt*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Stmt*_tmp3B1;struct _dyneither_ptr*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3B9;struct Cyc_Absyn_Stmt*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_List_List*_tmp3BD;struct Cyc_List_List*_tmp3C0;struct Cyc_Absyn_Decl*_tmp3C2;struct Cyc_Absyn_Stmt*_tmp3C3;struct _dyneither_ptr*_tmp3C5;struct Cyc_Absyn_Stmt*_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C8;struct Cyc_Absyn_Exp*_tmp3C9;struct Cyc_Absyn_Stmt*_tmp3CB;struct Cyc_List_List*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CE;_LL233: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3A3=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3A3->tag != 0)goto _LL235;}_LL234:
{const char*_tmp8AD;s=Cyc_PP_text(((_tmp8AD=";",_tag_dyneither(_tmp8AD,sizeof(char),2))));}goto _LL232;_LL235: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3A4=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3A4->tag != 1)goto _LL237;else{_tmp3A5=_tmp3A4->f1;}}_LL236:
{const char*_tmp8B0;struct Cyc_PP_Doc*_tmp8AF[2];s=((_tmp8AF[1]=Cyc_PP_text(((_tmp8B0=";",_tag_dyneither(_tmp8B0,sizeof(char),2)))),((_tmp8AF[0]=Cyc_Absynpp_exp2doc(_tmp3A5),Cyc_PP_cat(_tag_dyneither(_tmp8AF,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL232;_LL237: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3A6=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3A6->tag != 2)goto _LL239;else{_tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;}}_LL238:
# 1375
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3A7)){
struct Cyc_PP_Doc*_tmp8BB[5];struct Cyc_PP_Doc*_tmp8BA[7];const char*_tmp8B9;const char*_tmp8B8;struct Cyc_PP_Doc*_tmp8B7[7];s=((_tmp8B7[6]=
# 1383
Cyc_Absynpp_is_declaration(_tmp3A8)?
stmtexp?
(_tmp8BA[6]=
# 1388
Cyc_PP_line_doc(),((_tmp8BA[5]=
# 1387
Cyc_PP_text(((_tmp8B9=");",_tag_dyneither(_tmp8B9,sizeof(char),3)))),((_tmp8BA[4]=Cyc_Absynpp_rb(),((_tmp8BA[3]=
# 1386
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp)),((_tmp8BA[2]=
# 1385
Cyc_PP_blank_doc(),((_tmp8BA[1]=Cyc_Absynpp_lb(),((_tmp8BA[0]=Cyc_PP_text(((_tmp8B8="(",_tag_dyneither(_tmp8B8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1389
(_tmp8BB[4]=
# 1392
Cyc_PP_line_doc(),((_tmp8BB[3]=
# 1391
Cyc_Absynpp_rb(),((_tmp8BB[2]=
# 1390
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp)),((_tmp8BB[1]=
# 1389
Cyc_PP_blank_doc(),((_tmp8BB[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8BB,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1393
 Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp),((_tmp8B7[5]=
# 1382
Cyc_PP_line_doc(),((_tmp8B7[4]=
# 1381
Cyc_Absynpp_rb(),((_tmp8B7[3]=
# 1380
Cyc_PP_line_doc(),((_tmp8B7[2]=
# 1379
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A7,0)),((_tmp8B7[1]=
# 1378
Cyc_PP_blank_doc(),((_tmp8B7[0]=
# 1377
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1394
if(Cyc_Absynpp_is_declaration(_tmp3A8)){
struct Cyc_PP_Doc*_tmp8C6[4];struct Cyc_PP_Doc*_tmp8C5[6];const char*_tmp8C4;const char*_tmp8C3;struct Cyc_PP_Doc*_tmp8C2[4];s=((_tmp8C2[3]=
# 1405
Cyc_PP_line_doc(),((_tmp8C2[2]=
# 1397
stmtexp?
(_tmp8C5[5]=
# 1400
Cyc_PP_text(((_tmp8C4=");",_tag_dyneither(_tmp8C4,sizeof(char),3)))),((_tmp8C5[4]=Cyc_Absynpp_rb(),((_tmp8C5[3]=
# 1399
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp)),((_tmp8C5[2]=
# 1398
Cyc_PP_blank_doc(),((_tmp8C5[1]=Cyc_Absynpp_lb(),((_tmp8C5[0]=Cyc_PP_text(((_tmp8C3="(",_tag_dyneither(_tmp8C3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1402
(_tmp8C6[3]=
# 1404
Cyc_Absynpp_rb(),((_tmp8C6[2]=
# 1403
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp)),((_tmp8C6[1]=
# 1402
Cyc_PP_blank_doc(),((_tmp8C6[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8C6,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8C2[1]=
# 1396
Cyc_PP_line_doc(),((_tmp8C2[0]=
# 1395
Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(_tag_dyneither(_tmp8C2,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1407
struct Cyc_PP_Doc*_tmp8C7[3];s=((_tmp8C7[2]=Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp),((_tmp8C7[1]=Cyc_PP_line_doc(),((_tmp8C7[0]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1410
struct Cyc_PP_Doc*_tmp8C8[3];s=((_tmp8C8[2]=Cyc_Absynpp_stmt2doc(_tmp3A8,stmtexp),((_tmp8C8[1]=Cyc_PP_line_doc(),((_tmp8C8[0]=Cyc_Absynpp_stmt2doc(_tmp3A7,0),Cyc_PP_cat(_tag_dyneither(_tmp8C8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL232;_LL239: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3A9->tag != 3)goto _LL23B;else{_tmp3AA=_tmp3A9->f1;}}_LL23A:
# 1413
 if(_tmp3AA == 0){
const char*_tmp8C9;s=Cyc_PP_text(((_tmp8C9="return;",_tag_dyneither(_tmp8C9,sizeof(char),8))));}else{
# 1416
const char*_tmp8CE;const char*_tmp8CD;struct Cyc_PP_Doc*_tmp8CC[3];s=((_tmp8CC[2]=
# 1418
Cyc_PP_text(((_tmp8CD=";",_tag_dyneither(_tmp8CD,sizeof(char),2)))),((_tmp8CC[1]=
# 1417
_tmp3AA == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3AA),((_tmp8CC[0]=
# 1416
Cyc_PP_text(((_tmp8CE="return ",_tag_dyneither(_tmp8CE,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8CC,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1419
goto _LL232;_LL23B: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3AB=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3AB->tag != 4)goto _LL23D;else{_tmp3AC=_tmp3AB->f1;_tmp3AD=_tmp3AB->f2;_tmp3AE=_tmp3AB->f3;}}_LL23C: {
# 1421
int print_else;
{void*_stmttmp17=_tmp3AE->r;void*_tmp3E2=_stmttmp17;_LL258: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3E2;if(_tmp3E3->tag != 0)goto _LL25A;}_LL259:
 print_else=0;goto _LL257;_LL25A:;_LL25B:
 print_else=1;goto _LL257;_LL257:;}
# 1426
{const char*_tmp8DD;const char*_tmp8DC;struct Cyc_PP_Doc*_tmp8DB[2];struct Cyc_PP_Doc*_tmp8DA[6];struct Cyc_PP_Doc*_tmp8D9[2];const char*_tmp8D8;struct Cyc_PP_Doc*_tmp8D7[8];s=((_tmp8D7[7]=
# 1433
print_else?
(_tmp8DA[5]=
# 1439
Cyc_Absynpp_rb(),((_tmp8DA[4]=
# 1438
Cyc_PP_line_doc(),((_tmp8DA[3]=
# 1437
Cyc_PP_nest(2,((_tmp8D9[1]=Cyc_Absynpp_stmt2doc(_tmp3AE,0),((_tmp8D9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DA[2]=
# 1436
Cyc_Absynpp_lb(),((_tmp8DA[1]=
# 1435
Cyc_PP_text(((_tmp8D8="else ",_tag_dyneither(_tmp8D8,sizeof(char),6)))),((_tmp8DA[0]=
# 1434
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DA,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1440
 Cyc_PP_nil_doc(),((_tmp8D7[6]=
# 1432
Cyc_Absynpp_rb(),((_tmp8D7[5]=
# 1431
Cyc_PP_line_doc(),((_tmp8D7[4]=
# 1430
Cyc_PP_nest(2,((_tmp8DB[1]=Cyc_Absynpp_stmt2doc(_tmp3AD,0),((_tmp8DB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D7[3]=
# 1429
Cyc_Absynpp_lb(),((_tmp8D7[2]=
# 1428
Cyc_PP_text(((_tmp8DC=") ",_tag_dyneither(_tmp8DC,sizeof(char),3)))),((_tmp8D7[1]=
# 1427
Cyc_Absynpp_exp2doc(_tmp3AC),((_tmp8D7[0]=
# 1426
Cyc_PP_text(((_tmp8DD="if (",_tag_dyneither(_tmp8DD,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1441
goto _LL232;}_LL23D: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3AF=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3AF->tag != 5)goto _LL23F;else{_tmp3B0=(_tmp3AF->f1).f1;_tmp3B1=_tmp3AF->f2;}}_LL23E:
# 1443
{const char*_tmp8E4;const char*_tmp8E3;struct Cyc_PP_Doc*_tmp8E2[2];struct Cyc_PP_Doc*_tmp8E1[7];s=((_tmp8E1[6]=
# 1449
Cyc_Absynpp_rb(),((_tmp8E1[5]=
# 1448
Cyc_PP_line_doc(),((_tmp8E1[4]=
# 1447
Cyc_PP_nest(2,((_tmp8E2[1]=Cyc_Absynpp_stmt2doc(_tmp3B1,0),((_tmp8E2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E1[3]=
# 1446
Cyc_Absynpp_lb(),((_tmp8E1[2]=
# 1445
Cyc_PP_text(((_tmp8E3=") ",_tag_dyneither(_tmp8E3,sizeof(char),3)))),((_tmp8E1[1]=
# 1444
Cyc_Absynpp_exp2doc(_tmp3B0),((_tmp8E1[0]=
# 1443
Cyc_PP_text(((_tmp8E4="while (",_tag_dyneither(_tmp8E4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8E1,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1450
goto _LL232;_LL23F: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3B2=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3B2->tag != 6)goto _LL241;}_LL240:
{const char*_tmp8E5;s=Cyc_PP_text(((_tmp8E5="break;",_tag_dyneither(_tmp8E5,sizeof(char),7))));}goto _LL232;_LL241: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3B3=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3B3->tag != 7)goto _LL243;}_LL242:
{const char*_tmp8E6;s=Cyc_PP_text(((_tmp8E6="continue;",_tag_dyneither(_tmp8E6,sizeof(char),10))));}goto _LL232;_LL243: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3B4=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3B4->tag != 8)goto _LL245;else{_tmp3B5=_tmp3B4->f1;}}_LL244:
{const char*_tmp8EA;void*_tmp8E9[1];struct Cyc_String_pa_PrintArg_struct _tmp8E8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8E8.tag=0,((_tmp8E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3B5),((_tmp8E9[0]=& _tmp8E8,Cyc_aprintf(((_tmp8EA="goto %s;",_tag_dyneither(_tmp8EA,sizeof(char),9))),_tag_dyneither(_tmp8E9,sizeof(void*),1)))))))));}goto _LL232;_LL245: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3B6=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3B6->tag != 9)goto _LL247;else{_tmp3B7=_tmp3B6->f1;_tmp3B8=(_tmp3B6->f2).f1;_tmp3B9=(_tmp3B6->f3).f1;_tmp3BA=_tmp3B6->f4;}}_LL246:
# 1455
{const char*_tmp8F5;const char*_tmp8F4;const char*_tmp8F3;const char*_tmp8F2;struct Cyc_PP_Doc*_tmp8F1[2];struct Cyc_PP_Doc*_tmp8F0[11];s=((_tmp8F0[10]=
# 1465
Cyc_Absynpp_rb(),((_tmp8F0[9]=
# 1464
Cyc_PP_line_doc(),((_tmp8F0[8]=
# 1463
Cyc_PP_nest(2,((_tmp8F1[1]=Cyc_Absynpp_stmt2doc(_tmp3BA,0),((_tmp8F1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8F0[7]=
# 1462
Cyc_Absynpp_lb(),((_tmp8F0[6]=
# 1461
Cyc_PP_text(((_tmp8F2=") ",_tag_dyneither(_tmp8F2,sizeof(char),3)))),((_tmp8F0[5]=
# 1460
Cyc_Absynpp_exp2doc(_tmp3B9),((_tmp8F0[4]=
# 1459
Cyc_PP_text(((_tmp8F3="; ",_tag_dyneither(_tmp8F3,sizeof(char),3)))),((_tmp8F0[3]=
# 1458
Cyc_Absynpp_exp2doc(_tmp3B8),((_tmp8F0[2]=
# 1457
Cyc_PP_text(((_tmp8F4="; ",_tag_dyneither(_tmp8F4,sizeof(char),3)))),((_tmp8F0[1]=
# 1456
Cyc_Absynpp_exp2doc(_tmp3B7),((_tmp8F0[0]=
# 1455
Cyc_PP_text(((_tmp8F5="for(",_tag_dyneither(_tmp8F5,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1466
goto _LL232;_LL247: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3BB=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3BB->tag != 10)goto _LL249;else{_tmp3BC=_tmp3BB->f1;_tmp3BD=_tmp3BB->f2;}}_LL248:
# 1468
{const char*_tmp8FA;const char*_tmp8F9;struct Cyc_PP_Doc*_tmp8F8[8];s=((_tmp8F8[7]=
# 1475
Cyc_Absynpp_rb(),((_tmp8F8[6]=
# 1474
Cyc_PP_line_doc(),((_tmp8F8[5]=
# 1473
Cyc_Absynpp_switchclauses2doc(_tmp3BD),((_tmp8F8[4]=
# 1472
Cyc_PP_line_doc(),((_tmp8F8[3]=
# 1471
Cyc_Absynpp_lb(),((_tmp8F8[2]=
# 1470
Cyc_PP_text(((_tmp8F9=") ",_tag_dyneither(_tmp8F9,sizeof(char),3)))),((_tmp8F8[1]=
# 1469
Cyc_Absynpp_exp2doc(_tmp3BC),((_tmp8F8[0]=
# 1468
Cyc_PP_text(((_tmp8FA="switch (",_tag_dyneither(_tmp8FA,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8F8,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1476
goto _LL232;_LL249: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3BE=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3BE->tag != 11)goto _LL24B;else{if(_tmp3BE->f1 != 0)goto _LL24B;}}_LL24A:
{const char*_tmp8FB;s=Cyc_PP_text(((_tmp8FB="fallthru;",_tag_dyneither(_tmp8FB,sizeof(char),10))));}goto _LL232;_LL24B: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3BF=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3BF->tag != 11)goto _LL24D;else{_tmp3C0=_tmp3BF->f1;}}_LL24C:
# 1479
{const char*_tmp900;const char*_tmp8FF;struct Cyc_PP_Doc*_tmp8FE[3];s=((_tmp8FE[2]=Cyc_PP_text(((_tmp8FF=");",_tag_dyneither(_tmp8FF,sizeof(char),3)))),((_tmp8FE[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3C0),((_tmp8FE[0]=Cyc_PP_text(((_tmp900="fallthru(",_tag_dyneither(_tmp900,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8FE,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL232;_LL24D: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3C1=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3C1->tag != 12)goto _LL24F;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;}}_LL24E:
# 1481
{struct Cyc_PP_Doc*_tmp901[3];s=((_tmp901[2]=
# 1483
Cyc_Absynpp_stmt2doc(_tmp3C3,stmtexp),((_tmp901[1]=
# 1482
Cyc_PP_line_doc(),((_tmp901[0]=
# 1481
Cyc_Absynpp_decl2doc(_tmp3C2),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1484
goto _LL232;_LL24F: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3C4=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3C4->tag != 13)goto _LL251;else{_tmp3C5=_tmp3C4->f1;_tmp3C6=_tmp3C4->f2;}}_LL250:
# 1486
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C6)){
if(stmtexp){
const char*_tmp906;const char*_tmp905;struct Cyc_PP_Doc*_tmp904[8];s=((_tmp904[7]=
# 1494
Cyc_PP_text(((_tmp905=");",_tag_dyneither(_tmp905,sizeof(char),3)))),((_tmp904[6]=Cyc_Absynpp_rb(),((_tmp904[5]=
# 1493
Cyc_PP_line_doc(),((_tmp904[4]=
# 1492
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C6,1)),((_tmp904[3]=
# 1491
Cyc_PP_line_doc(),((_tmp904[2]=
# 1490
Cyc_Absynpp_lb(),((_tmp904[1]=
# 1489
Cyc_PP_text(((_tmp906=": (",_tag_dyneither(_tmp906,sizeof(char),4)))),((_tmp904[0]=
# 1488
Cyc_PP_textptr(_tmp3C5),Cyc_PP_cat(_tag_dyneither(_tmp904,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1496
const char*_tmp909;struct Cyc_PP_Doc*_tmp908[7];s=((_tmp908[6]=
# 1502
Cyc_Absynpp_rb(),((_tmp908[5]=
# 1501
Cyc_PP_line_doc(),((_tmp908[4]=
# 1500
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C6,0)),((_tmp908[3]=
# 1499
Cyc_PP_line_doc(),((_tmp908[2]=
# 1498
Cyc_Absynpp_lb(),((_tmp908[1]=
# 1497
Cyc_PP_text(((_tmp909=": ",_tag_dyneither(_tmp909,sizeof(char),3)))),((_tmp908[0]=
# 1496
Cyc_PP_textptr(_tmp3C5),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1504
const char*_tmp90C;struct Cyc_PP_Doc*_tmp90B[3];s=((_tmp90B[2]=Cyc_Absynpp_stmt2doc(_tmp3C6,stmtexp),((_tmp90B[1]=Cyc_PP_text(((_tmp90C=": ",_tag_dyneither(_tmp90C,sizeof(char),3)))),((_tmp90B[0]=Cyc_PP_textptr(_tmp3C5),Cyc_PP_cat(_tag_dyneither(_tmp90B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL232;_LL251: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3C7=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3C7->tag != 14)goto _LL253;else{_tmp3C8=_tmp3C7->f1;_tmp3C9=(_tmp3C7->f2).f1;}}_LL252:
# 1507
{const char*_tmp913;const char*_tmp912;const char*_tmp911;struct Cyc_PP_Doc*_tmp910[9];s=((_tmp910[8]=
# 1515
Cyc_PP_text(((_tmp911=");",_tag_dyneither(_tmp911,sizeof(char),3)))),((_tmp910[7]=
# 1514
Cyc_Absynpp_exp2doc(_tmp3C9),((_tmp910[6]=
# 1513
Cyc_PP_text(((_tmp912=" while (",_tag_dyneither(_tmp912,sizeof(char),9)))),((_tmp910[5]=
# 1512
Cyc_Absynpp_rb(),((_tmp910[4]=
# 1511
Cyc_PP_line_doc(),((_tmp910[3]=
# 1510
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C8,0)),((_tmp910[2]=
# 1509
Cyc_PP_line_doc(),((_tmp910[1]=
# 1508
Cyc_Absynpp_lb(),((_tmp910[0]=
# 1507
Cyc_PP_text(((_tmp913="do ",_tag_dyneither(_tmp913,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp910,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1516
goto _LL232;_LL253: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3CA=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3CA->tag != 15)goto _LL255;else{_tmp3CB=_tmp3CA->f1;_tmp3CC=_tmp3CA->f2;}}_LL254:
# 1518
{const char*_tmp918;const char*_tmp917;struct Cyc_PP_Doc*_tmp916[12];s=((_tmp916[11]=
# 1529
Cyc_Absynpp_rb(),((_tmp916[10]=
# 1528
Cyc_PP_line_doc(),((_tmp916[9]=
# 1527
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3CC)),((_tmp916[8]=
# 1526
Cyc_PP_line_doc(),((_tmp916[7]=
# 1525
Cyc_Absynpp_lb(),((_tmp916[6]=
# 1524
Cyc_PP_text(((_tmp917=" catch ",_tag_dyneither(_tmp917,sizeof(char),8)))),((_tmp916[5]=
# 1523
Cyc_Absynpp_rb(),((_tmp916[4]=
# 1522
Cyc_PP_line_doc(),((_tmp916[3]=
# 1521
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3CB,0)),((_tmp916[2]=
# 1520
Cyc_PP_line_doc(),((_tmp916[1]=
# 1519
Cyc_Absynpp_lb(),((_tmp916[0]=
# 1518
Cyc_PP_text(((_tmp918="try ",_tag_dyneither(_tmp918,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp916,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1530
goto _LL232;_LL255: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3CD=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3A2;if(_tmp3CD->tag != 16)goto _LL232;else{_tmp3CE=_tmp3CD->f1;}}_LL256:
# 1532
{const char*_tmp91D;const char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[3];s=((_tmp91B[2]=Cyc_PP_text(((_tmp91C=");",_tag_dyneither(_tmp91C,sizeof(char),3)))),((_tmp91B[1]=Cyc_Absynpp_exp2doc(_tmp3CE),((_tmp91B[0]=Cyc_PP_text(((_tmp91D="reset_region(",_tag_dyneither(_tmp91D,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL232;_LL232:;}
# 1535
return s;}
# 1538
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_stmttmp18=p->r;void*_tmp413=_stmttmp18;enum Cyc_Absyn_Sign _tmp417;int _tmp418;char _tmp41A;struct _dyneither_ptr _tmp41C;struct Cyc_Absyn_Vardecl*_tmp41E;struct Cyc_Absyn_Vardecl*_tmp421;struct Cyc_Absyn_Pat*_tmp422;struct Cyc_Absyn_Tvar*_tmp424;struct Cyc_Absyn_Vardecl*_tmp425;struct Cyc_List_List*_tmp427;int _tmp428;struct Cyc_Absyn_Pat*_tmp42A;struct Cyc_Absyn_Vardecl*_tmp42C;struct Cyc_Absyn_Vardecl*_tmp42F;struct Cyc_Absyn_Pat*_tmp430;struct _tuple0*_tmp432;struct _tuple0*_tmp434;struct Cyc_List_List*_tmp435;int _tmp436;union Cyc_Absyn_AggrInfoU _tmp438;struct Cyc_List_List*_tmp439;struct Cyc_List_List*_tmp43A;int _tmp43B;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43E;int _tmp43F;struct Cyc_Absyn_Enumfield*_tmp441;struct Cyc_Absyn_Enumfield*_tmp443;struct Cyc_Absyn_Datatypefield*_tmp445;struct Cyc_Absyn_Datatypefield*_tmp447;struct Cyc_List_List*_tmp448;int _tmp449;struct Cyc_Absyn_Exp*_tmp44B;_LL25D: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp414=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp414->tag != 0)goto _LL25F;}_LL25E:
{const char*_tmp91E;s=Cyc_PP_text(((_tmp91E="_",_tag_dyneither(_tmp91E,sizeof(char),2))));}goto _LL25C;_LL25F: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp415=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp415->tag != 8)goto _LL261;}_LL260:
{const char*_tmp91F;s=Cyc_PP_text(((_tmp91F="NULL",_tag_dyneither(_tmp91F,sizeof(char),5))));}goto _LL25C;_LL261: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp416=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp416->tag != 9)goto _LL263;else{_tmp417=_tmp416->f1;_tmp418=_tmp416->f2;}}_LL262:
# 1544
 if(_tmp417 != Cyc_Absyn_Unsigned){
const char*_tmp923;void*_tmp922[1];struct Cyc_Int_pa_PrintArg_struct _tmp921;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp921.tag=1,((_tmp921.f1=(unsigned long)_tmp418,((_tmp922[0]=& _tmp921,Cyc_aprintf(((_tmp923="%d",_tag_dyneither(_tmp923,sizeof(char),3))),_tag_dyneither(_tmp922,sizeof(void*),1)))))))));}else{
const char*_tmp927;void*_tmp926[1];struct Cyc_Int_pa_PrintArg_struct _tmp925;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp925.tag=1,((_tmp925.f1=(unsigned int)_tmp418,((_tmp926[0]=& _tmp925,Cyc_aprintf(((_tmp927="%u",_tag_dyneither(_tmp927,sizeof(char),3))),_tag_dyneither(_tmp926,sizeof(void*),1)))))))));}
goto _LL25C;_LL263: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp419=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp419->tag != 10)goto _LL265;else{_tmp41A=_tmp419->f1;}}_LL264:
{const char*_tmp92B;void*_tmp92A[1];struct Cyc_String_pa_PrintArg_struct _tmp929;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp929.tag=0,((_tmp929.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp41A)),((_tmp92A[0]=& _tmp929,Cyc_aprintf(((_tmp92B="'%s'",_tag_dyneither(_tmp92B,sizeof(char),5))),_tag_dyneither(_tmp92A,sizeof(void*),1)))))))));}goto _LL25C;_LL265: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp41B=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp41B->tag != 11)goto _LL267;else{_tmp41C=_tmp41B->f1;}}_LL266:
 s=Cyc_PP_text(_tmp41C);goto _LL25C;_LL267: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41D=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp41D->tag != 1)goto _LL269;else{_tmp41E=_tmp41D->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp41F=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp41D->f2)->r;if(_tmp41F->tag != 0)goto _LL269;};}}_LL268:
# 1551
 s=Cyc_Absynpp_qvar2doc(_tmp41E->name);goto _LL25C;_LL269: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp420=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp420->tag != 1)goto _LL26B;else{_tmp421=_tmp420->f1;_tmp422=_tmp420->f2;}}_LL26A:
# 1554
{const char*_tmp92E;struct Cyc_PP_Doc*_tmp92D[3];s=((_tmp92D[2]=Cyc_Absynpp_pat2doc(_tmp422),((_tmp92D[1]=Cyc_PP_text(((_tmp92E=" as ",_tag_dyneither(_tmp92E,sizeof(char),5)))),((_tmp92D[0]=Cyc_Absynpp_qvar2doc(_tmp421->name),Cyc_PP_cat(_tag_dyneither(_tmp92D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL25C;_LL26B: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp423=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp423->tag != 3)goto _LL26D;else{_tmp424=_tmp423->f1;_tmp425=_tmp423->f2;}}_LL26C:
# 1556
{const char*_tmp933;const char*_tmp932;struct Cyc_PP_Doc*_tmp931[4];s=((_tmp931[3]=Cyc_PP_text(((_tmp932=">",_tag_dyneither(_tmp932,sizeof(char),2)))),((_tmp931[2]=Cyc_Absynpp_tvar2doc(_tmp424),((_tmp931[1]=Cyc_PP_text(((_tmp933="<",_tag_dyneither(_tmp933,sizeof(char),2)))),((_tmp931[0]=Cyc_Absynpp_qvar2doc(_tmp425->name),Cyc_PP_cat(_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL25C;_LL26D: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp426=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp426->tag != 4)goto _LL26F;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}_LL26E:
# 1559
{const char*_tmp93C;const char*_tmp93B;const char*_tmp93A;const char*_tmp939;struct Cyc_PP_Doc*_tmp938[4];s=((_tmp938[3]=
_tmp428?Cyc_PP_text(((_tmp939=", ...)",_tag_dyneither(_tmp939,sizeof(char),7)))): Cyc_PP_text(((_tmp93A=")",_tag_dyneither(_tmp93A,sizeof(char),2)))),((_tmp938[2]=
# 1559
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp93B=",",_tag_dyneither(_tmp93B,sizeof(char),2))),_tmp427),((_tmp938[1]=Cyc_PP_text(((_tmp93C="(",_tag_dyneither(_tmp93C,sizeof(char),2)))),((_tmp938[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1561
goto _LL25C;_LL26F: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp429=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp429->tag != 5)goto _LL271;else{_tmp42A=_tmp429->f1;}}_LL270:
# 1563
{const char*_tmp93F;struct Cyc_PP_Doc*_tmp93E[2];s=((_tmp93E[1]=Cyc_Absynpp_pat2doc(_tmp42A),((_tmp93E[0]=Cyc_PP_text(((_tmp93F="&",_tag_dyneither(_tmp93F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp93E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25C;_LL271: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp42B=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp42B->tag != 2)goto _LL273;else{_tmp42C=_tmp42B->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp42D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp42B->f2)->r;if(_tmp42D->tag != 0)goto _LL273;};}}_LL272:
# 1566
{const char*_tmp942;struct Cyc_PP_Doc*_tmp941[2];s=((_tmp941[1]=Cyc_Absynpp_qvar2doc(_tmp42C->name),((_tmp941[0]=Cyc_PP_text(((_tmp942="*",_tag_dyneither(_tmp942,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25C;_LL273: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp42E=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp42E->tag != 2)goto _LL275;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;}}_LL274:
# 1569
{const char*_tmp947;const char*_tmp946;struct Cyc_PP_Doc*_tmp945[4];s=((_tmp945[3]=Cyc_Absynpp_pat2doc(_tmp430),((_tmp945[2]=Cyc_PP_text(((_tmp946=" as ",_tag_dyneither(_tmp946,sizeof(char),5)))),((_tmp945[1]=Cyc_Absynpp_qvar2doc(_tmp42F->name),((_tmp945[0]=Cyc_PP_text(((_tmp947="*",_tag_dyneither(_tmp947,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp945,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL25C;_LL275: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp431=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp431->tag != 14)goto _LL277;else{_tmp432=_tmp431->f1;}}_LL276:
# 1572
 s=Cyc_Absynpp_qvar2doc(_tmp432);
goto _LL25C;_LL277: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp433=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp433->tag != 15)goto _LL279;else{_tmp434=_tmp433->f1;_tmp435=_tmp433->f2;_tmp436=_tmp433->f3;}}_LL278: {
# 1575
const char*_tmp949;const char*_tmp948;struct _dyneither_ptr term=_tmp436?(_tmp949=", ...)",_tag_dyneither(_tmp949,sizeof(char),7)):((_tmp948=")",_tag_dyneither(_tmp948,sizeof(char),2)));
{const char*_tmp94E;const char*_tmp94D;struct Cyc_PP_Doc*_tmp94C[2];s=((_tmp94C[1]=Cyc_PP_group(((_tmp94E="(",_tag_dyneither(_tmp94E,sizeof(char),2))),term,((_tmp94D=",",_tag_dyneither(_tmp94D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp435)),((_tmp94C[0]=Cyc_Absynpp_qvar2doc(_tmp434),Cyc_PP_cat(_tag_dyneither(_tmp94C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25C;}_LL279: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp437=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp437->tag != 6)goto _LL27B;else{if(_tmp437->f1 == 0)goto _LL27B;_tmp438=(_tmp437->f1)->aggr_info;_tmp439=_tmp437->f2;_tmp43A=_tmp437->f3;_tmp43B=_tmp437->f4;}}_LL27A: {
# 1579
const char*_tmp950;const char*_tmp94F;struct _dyneither_ptr term=_tmp43B?(_tmp950=", ...}",_tag_dyneither(_tmp950,sizeof(char),7)):((_tmp94F="}",_tag_dyneither(_tmp94F,sizeof(char),2)));
struct _tuple10 _stmttmp19=Cyc_Absyn_aggr_kinded_name(_tmp438);struct _tuple0*_tmp46E;struct _tuple10 _tmp46D=_stmttmp19;_tmp46E=_tmp46D.f2;
{const char*_tmp95B;const char*_tmp95A;const char*_tmp959;const char*_tmp958;const char*_tmp957;struct Cyc_PP_Doc*_tmp956[4];s=((_tmp956[3]=
# 1583
Cyc_PP_group(((_tmp958="",_tag_dyneither(_tmp958,sizeof(char),1))),term,((_tmp957=",",_tag_dyneither(_tmp957,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp43A)),((_tmp956[2]=
# 1582
Cyc_PP_egroup(((_tmp95B="[",_tag_dyneither(_tmp95B,sizeof(char),2))),((_tmp95A="]",_tag_dyneither(_tmp95A,sizeof(char),2))),((_tmp959=",",_tag_dyneither(_tmp959,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp439)),((_tmp956[1]=
# 1581
Cyc_Absynpp_lb(),((_tmp956[0]=Cyc_Absynpp_qvar2doc(_tmp46E),Cyc_PP_cat(_tag_dyneither(_tmp956,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1584
goto _LL25C;}_LL27B: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp43C=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp43C->tag != 6)goto _LL27D;else{if(_tmp43C->f1 != 0)goto _LL27D;_tmp43D=_tmp43C->f2;_tmp43E=_tmp43C->f3;_tmp43F=_tmp43C->f4;}}_LL27C: {
# 1586
const char*_tmp95D;const char*_tmp95C;struct _dyneither_ptr term=_tmp43F?(_tmp95D=", ...}",_tag_dyneither(_tmp95D,sizeof(char),7)):((_tmp95C="}",_tag_dyneither(_tmp95C,sizeof(char),2)));
{const char*_tmp968;const char*_tmp967;const char*_tmp966;const char*_tmp965;const char*_tmp964;struct Cyc_PP_Doc*_tmp963[3];s=((_tmp963[2]=
# 1589
Cyc_PP_group(((_tmp965="",_tag_dyneither(_tmp965,sizeof(char),1))),term,((_tmp964=",",_tag_dyneither(_tmp964,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp43E)),((_tmp963[1]=
# 1588
Cyc_PP_egroup(((_tmp968="[",_tag_dyneither(_tmp968,sizeof(char),2))),((_tmp967="]",_tag_dyneither(_tmp967,sizeof(char),2))),((_tmp966=",",_tag_dyneither(_tmp966,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp43D)),((_tmp963[0]=
# 1587
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp963,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1590
goto _LL25C;}_LL27D: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp440->tag != 12)goto _LL27F;else{_tmp441=_tmp440->f2;}}_LL27E:
 _tmp443=_tmp441;goto _LL280;_LL27F: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp442=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp442->tag != 13)goto _LL281;else{_tmp443=_tmp442->f2;}}_LL280:
 s=Cyc_Absynpp_qvar2doc(_tmp443->name);goto _LL25C;_LL281: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp444=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp444->tag != 7)goto _LL283;else{_tmp445=_tmp444->f2;if(_tmp444->f3 != 0)goto _LL283;}}_LL282:
 s=Cyc_Absynpp_qvar2doc(_tmp445->name);goto _LL25C;_LL283: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp446=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp446->tag != 7)goto _LL285;else{_tmp447=_tmp446->f2;_tmp448=_tmp446->f3;_tmp449=_tmp446->f4;}}_LL284: {
# 1595
const char*_tmp96A;const char*_tmp969;struct _dyneither_ptr term=_tmp449?(_tmp96A=", ...)",_tag_dyneither(_tmp96A,sizeof(char),7)):((_tmp969=")",_tag_dyneither(_tmp969,sizeof(char),2)));
{const char*_tmp96F;const char*_tmp96E;struct Cyc_PP_Doc*_tmp96D[2];s=((_tmp96D[1]=Cyc_PP_egroup(((_tmp96F="(",_tag_dyneither(_tmp96F,sizeof(char),2))),term,((_tmp96E=",",_tag_dyneither(_tmp96E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp448)),((_tmp96D[0]=Cyc_Absynpp_qvar2doc(_tmp447->name),Cyc_PP_cat(_tag_dyneither(_tmp96D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL25C;}_LL285: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp44A=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp413;if(_tmp44A->tag != 16)goto _LL25C;else{_tmp44B=_tmp44A->f1;}}_LL286:
# 1599
 s=Cyc_Absynpp_exp2doc(_tmp44B);goto _LL25C;_LL25C:;}
# 1601
return s;}
# 1604
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp976;const char*_tmp975;const char*_tmp974;struct Cyc_PP_Doc*_tmp973[2];return(_tmp973[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp973[0]=
# 1605
Cyc_PP_egroup(((_tmp976="",_tag_dyneither(_tmp976,sizeof(char),1))),((_tmp975="=",_tag_dyneither(_tmp975,sizeof(char),2))),((_tmp974="=",_tag_dyneither(_tmp974,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp973,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1609
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[2];struct Cyc_PP_Doc*_tmp979[2];return(_tmp979[1]=
Cyc_PP_nest(2,((_tmp97A[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp97A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp979[0]=
# 1611
Cyc_PP_text(((_tmp97B="default: ",_tag_dyneither(_tmp97B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp979,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1613
if(c->where_clause == 0){
const char*_tmp982;const char*_tmp981;struct Cyc_PP_Doc*_tmp980[2];struct Cyc_PP_Doc*_tmp97F[4];return(_tmp97F[3]=
# 1617
Cyc_PP_nest(2,((_tmp980[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp980[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp980,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp97F[2]=
# 1616
Cyc_PP_text(((_tmp981=": ",_tag_dyneither(_tmp981,sizeof(char),3)))),((_tmp97F[1]=
# 1615
Cyc_Absynpp_pat2doc(c->pattern),((_tmp97F[0]=
# 1614
Cyc_PP_text(((_tmp982="case ",_tag_dyneither(_tmp982,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp97F,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1619
const char*_tmp98B;const char*_tmp98A;const char*_tmp989;struct Cyc_PP_Doc*_tmp988[2];struct Cyc_PP_Doc*_tmp987[6];return(_tmp987[5]=
# 1624
Cyc_PP_nest(2,((_tmp988[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp988[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp988,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp987[4]=
# 1623
Cyc_PP_text(((_tmp989=": ",_tag_dyneither(_tmp989,sizeof(char),3)))),((_tmp987[3]=
# 1622
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp987[2]=
# 1621
Cyc_PP_text(((_tmp98A=" && ",_tag_dyneither(_tmp98A,sizeof(char),5)))),((_tmp987[1]=
# 1620
Cyc_Absynpp_pat2doc(c->pattern),((_tmp987[0]=
# 1619
Cyc_PP_text(((_tmp98B="case ",_tag_dyneither(_tmp98B,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp987,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1627
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp98C;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp98C="",_tag_dyneither(_tmp98C,sizeof(char),1))),cs);}
# 1631
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1635
const char*_tmp98F;struct Cyc_PP_Doc*_tmp98E[3];return(_tmp98E[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp98E[1]=Cyc_PP_text(((_tmp98F=" = ",_tag_dyneither(_tmp98F,sizeof(char),4)))),((_tmp98E[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1638
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp990;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp990=",",_tag_dyneither(_tmp990,sizeof(char),2))),fs);}
# 1642
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1646
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp991;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp991=",",_tag_dyneither(_tmp991,sizeof(char),2))),vds);}
# 1650
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp49A;struct _tuple0*_tmp49B;struct Cyc_Absyn_Tqual _tmp49C;void*_tmp49D;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Vardecl*_tmp499=vd;_tmp49A=_tmp499->sc;_tmp49B=_tmp499->name;_tmp49C=_tmp499->tq;_tmp49D=_tmp499->type;_tmp49E=_tmp499->initializer;_tmp49F=_tmp499->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp49B);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49F);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL287:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL288:
# 1659
{void*_stmttmp1A=Cyc_Tcutil_compress(_tmp49D);void*_tmp4A0=_stmttmp1A;struct Cyc_List_List*_tmp4A2;_LL28B: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4A1=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4A0;if(_tmp4A1->tag != 9)goto _LL28D;else{_tmp4A2=(_tmp4A1->f1).attributes;}}_LL28C:
# 1661
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A2);
goto _LL28A;_LL28D:;_LL28E:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL28A;_LL28A:;}
# 1665
break;}{
# 1668
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28F:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL290:
 tmp_doc=attsdoc;break;}
# 1673
{struct Cyc_Core_Opt*_tmp99E;struct Cyc_PP_Doc*_tmp99D[2];struct Cyc_PP_Doc*_tmp99C[2];const char*_tmp99B;const char*_tmp99A;struct Cyc_PP_Doc*_tmp999[5];s=((_tmp999[4]=
# 1680
Cyc_PP_text(((_tmp99A=";",_tag_dyneither(_tmp99A,sizeof(char),2)))),((_tmp999[3]=
# 1677
_tmp49E == 0?
Cyc_PP_nil_doc():(
(_tmp99C[1]=Cyc_Absynpp_exp2doc(_tmp49E),((_tmp99C[0]=Cyc_PP_text(((_tmp99B=" = ",_tag_dyneither(_tmp99B,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp999[2]=
# 1676
Cyc_Absynpp_tqtd2doc(_tmp49C,_tmp49D,((_tmp99E=_cycalloc(sizeof(*_tmp99E)),((_tmp99E->v=((_tmp99D[1]=sn,((_tmp99D[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp99E))))),((_tmp999[1]=
# 1675
Cyc_Absynpp_scope2doc(_tmp49A),((_tmp999[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1681
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1684
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _stmttmp1B=*x;struct _tuple0*_tmp4AA;struct _tuple17 _tmp4A9=_stmttmp1B;_tmp4AA=_tmp4A9.f2;
return Cyc_Absynpp_qvar2doc(_tmp4AA);}
# 1689
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp99F[4];return(_tmp99F[3]=
# 1694
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp99F[2]=
# 1693
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp99F[1]=
# 1692
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp99F[0]=
# 1691
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp99F,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1696
struct Cyc_PP_Doc*_tmp9A7[2];const char*_tmp9A6;struct Cyc_PP_Doc*_tmp9A5[2];struct Cyc_PP_Doc*_tmp9A4[12];return(_tmp9A4[11]=
# 1707
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9A4[10]=
# 1706
Cyc_Absynpp_rb(),((_tmp9A4[9]=
# 1705
Cyc_PP_line_doc(),((_tmp9A4[8]=
# 1704
Cyc_PP_nest(2,((_tmp9A5[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9A5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A4[7]=
# 1702
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp9A7[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp9A7[0]=Cyc_PP_text(((_tmp9A6=":",_tag_dyneither(_tmp9A6,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9A4[6]=
# 1701
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp9A4[5]=
# 1700
Cyc_Absynpp_lb(),((_tmp9A4[4]=Cyc_PP_blank_doc(),((_tmp9A4[3]=
# 1699
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9A4[2]=
# 1698
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9A4[1]=
# 1697
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9A4[0]=
# 1696
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9A4,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1710
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4B1;struct _tuple0*_tmp4B2;struct Cyc_List_List*_tmp4B3;struct Cyc_Core_Opt*_tmp4B4;int _tmp4B5;struct Cyc_Absyn_Datatypedecl*_tmp4B0=dd;_tmp4B1=_tmp4B0->sc;_tmp4B2=_tmp4B0->name;_tmp4B3=_tmp4B0->tvs;_tmp4B4=_tmp4B0->fields;_tmp4B5=_tmp4B0->is_extensible;
if(_tmp4B4 == 0){
const char*_tmp9AC;const char*_tmp9AB;struct Cyc_PP_Doc*_tmp9AA[5];return(_tmp9AA[4]=
# 1717
Cyc_Absynpp_ktvars2doc(_tmp4B3),((_tmp9AA[3]=
# 1716
_tmp4B5?Cyc_Absynpp_qvar2bolddoc(_tmp4B2): Cyc_Absynpp_typedef_name2bolddoc(_tmp4B2),((_tmp9AA[2]=
# 1715
Cyc_PP_text(((_tmp9AB="datatype ",_tag_dyneither(_tmp9AB,sizeof(char),10)))),((_tmp9AA[1]=
# 1714
_tmp4B5?Cyc_PP_text(((_tmp9AC="@extensible ",_tag_dyneither(_tmp9AC,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9AA[0]=
# 1713
Cyc_Absynpp_scope2doc(_tmp4B1),Cyc_PP_cat(_tag_dyneither(_tmp9AA,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1719
const char*_tmp9B3;const char*_tmp9B2;struct Cyc_PP_Doc*_tmp9B1[2];struct Cyc_PP_Doc*_tmp9B0[10];return(_tmp9B0[9]=
# 1727
Cyc_Absynpp_rb(),((_tmp9B0[8]=
# 1726
Cyc_PP_line_doc(),((_tmp9B0[7]=
# 1725
Cyc_PP_nest(2,((_tmp9B1[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4B4->v),((_tmp9B1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B0[6]=
# 1724
Cyc_Absynpp_lb(),((_tmp9B0[5]=Cyc_PP_blank_doc(),((_tmp9B0[4]=
# 1723
Cyc_Absynpp_ktvars2doc(_tmp4B3),((_tmp9B0[3]=
# 1722
_tmp4B5?Cyc_Absynpp_qvar2bolddoc(_tmp4B2): Cyc_Absynpp_typedef_name2bolddoc(_tmp4B2),((_tmp9B0[2]=
# 1721
Cyc_PP_text(((_tmp9B2="datatype ",_tag_dyneither(_tmp9B2,sizeof(char),10)))),((_tmp9B0[1]=
# 1720
_tmp4B5?Cyc_PP_text(((_tmp9B3="@extensible ",_tag_dyneither(_tmp9B3,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9B0[0]=
# 1719
Cyc_Absynpp_scope2doc(_tmp4B1),Cyc_PP_cat(_tag_dyneither(_tmp9B0,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1730
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4BE;struct _tuple0*_tmp4BF;struct Cyc_Core_Opt*_tmp4C0;struct Cyc_Absyn_Enumdecl*_tmp4BD=ed;_tmp4BE=_tmp4BD->sc;_tmp4BF=_tmp4BD->name;_tmp4C0=_tmp4BD->fields;
if(_tmp4C0 == 0){
const char*_tmp9B6;struct Cyc_PP_Doc*_tmp9B5[3];return(_tmp9B5[2]=
# 1735
Cyc_Absynpp_typedef_name2bolddoc(_tmp4BF),((_tmp9B5[1]=
# 1734
Cyc_PP_text(((_tmp9B6="enum ",_tag_dyneither(_tmp9B6,sizeof(char),6)))),((_tmp9B5[0]=
# 1733
Cyc_Absynpp_scope2doc(_tmp4BE),Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1738
const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[2];struct Cyc_PP_Doc*_tmp9B9[8];return(_tmp9B9[7]=
# 1744
Cyc_Absynpp_rb(),((_tmp9B9[6]=
# 1743
Cyc_PP_line_doc(),((_tmp9B9[5]=
# 1742
Cyc_PP_nest(2,((_tmp9BA[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4C0->v),((_tmp9BA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B9[4]=
# 1741
Cyc_Absynpp_lb(),((_tmp9B9[3]=Cyc_PP_blank_doc(),((_tmp9B9[2]=
# 1740
Cyc_Absynpp_qvar2bolddoc(_tmp4BF),((_tmp9B9[1]=
# 1739
Cyc_PP_text(((_tmp9BB="enum ",_tag_dyneither(_tmp9BB,sizeof(char),6)))),((_tmp9B9[0]=
# 1738
Cyc_Absynpp_scope2doc(_tmp4BE),Cyc_PP_cat(_tag_dyneither(_tmp9B9,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1747
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_stmttmp1C=d->r;void*_tmp4C6=_stmttmp1C;struct Cyc_Absyn_Fndecl*_tmp4C8;struct Cyc_Absyn_Aggrdecl*_tmp4CA;struct Cyc_Absyn_Vardecl*_tmp4CC;struct Cyc_Absyn_Tvar*_tmp4CE;struct Cyc_Absyn_Vardecl*_tmp4CF;int _tmp4D0;struct Cyc_Absyn_Exp*_tmp4D1;struct Cyc_Absyn_Tvar*_tmp4D3;struct Cyc_Absyn_Vardecl*_tmp4D4;struct Cyc_Absyn_Datatypedecl*_tmp4D6;struct Cyc_Absyn_Pat*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_List_List*_tmp4DB;struct Cyc_Absyn_Enumdecl*_tmp4DD;struct Cyc_Absyn_Typedefdecl*_tmp4DF;struct _dyneither_ptr*_tmp4E1;struct Cyc_List_List*_tmp4E2;struct _tuple0*_tmp4E4;struct Cyc_List_List*_tmp4E5;struct Cyc_List_List*_tmp4E7;struct Cyc_List_List*_tmp4E9;struct Cyc_List_List*_tmp4EA;_LL293: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4C7=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4C7->tag != 1)goto _LL295;else{_tmp4C8=_tmp4C7->f1;}}_LL294: {
# 1751
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9C1;struct Cyc_Absyn_FnInfo _tmp9C0;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9BF;void*t=(void*)((_tmp9BF=_cycalloc(sizeof(*_tmp9BF)),((_tmp9BF[0]=((_tmp9C1.tag=9,((_tmp9C1.f1=((_tmp9C0.tvars=_tmp4C8->tvs,((_tmp9C0.effect=_tmp4C8->effect,((_tmp9C0.ret_tqual=_tmp4C8->ret_tqual,((_tmp9C0.ret_typ=_tmp4C8->ret_type,((_tmp9C0.args=
# 1755
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C8->args),((_tmp9C0.c_varargs=_tmp4C8->c_varargs,((_tmp9C0.cyc_varargs=_tmp4C8->cyc_varargs,((_tmp9C0.rgn_po=_tmp4C8->rgn_po,((_tmp9C0.attributes=0,_tmp9C0)))))))))))))))))),_tmp9C1)))),_tmp9BF))));
# 1759
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C8->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C8->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B3:
{const char*_tmp9C2;inlinedoc=Cyc_PP_text(((_tmp9C2="inline ",_tag_dyneither(_tmp9C2,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B4:
{const char*_tmp9C3;inlinedoc=Cyc_PP_text(((_tmp9C3="__inline ",_tag_dyneither(_tmp9C3,sizeof(char),10))));}break;}else{
# 1767
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C8->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B6:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B7:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C8->attributes);break;}{
# 1774
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C8->name);
struct Cyc_PP_Doc*_tmp9C6[2];struct Cyc_Core_Opt*_tmp9C5;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9C5=_cycalloc(sizeof(*_tmp9C5)),((_tmp9C5->v=((_tmp9C6[1]=namedoc,((_tmp9C6[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9C6,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9C5)))));
struct Cyc_PP_Doc*_tmp9C9[2];struct Cyc_PP_Doc*_tmp9C8[5];struct Cyc_PP_Doc*bodydoc=(_tmp9C8[4]=
# 1780
Cyc_Absynpp_rb(),((_tmp9C8[3]=
# 1779
Cyc_PP_line_doc(),((_tmp9C8[2]=
# 1778
Cyc_PP_nest(2,((_tmp9C9[1]=Cyc_Absynpp_stmt2doc(_tmp4C8->body,0),((_tmp9C9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C8[1]=
# 1777
Cyc_Absynpp_lb(),((_tmp9C8[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C8,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1781
{struct Cyc_PP_Doc*_tmp9CA[4];s=((_tmp9CA[3]=bodydoc,((_tmp9CA[2]=tqtddoc,((_tmp9CA[1]=scopedoc,((_tmp9CA[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1783
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B9:
{struct Cyc_PP_Doc*_tmp9CB[2];s=((_tmp9CB[1]=s,((_tmp9CB[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9CB,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2BA:
 break;}
# 1788
goto _LL292;};};}_LL295: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C9=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4C9->tag != 6)goto _LL297;else{_tmp4CA=_tmp4C9->f1;}}_LL296:
# 1791
{const char*_tmp9CE;struct Cyc_PP_Doc*_tmp9CD[2];s=((_tmp9CD[1]=Cyc_PP_text(((_tmp9CE=";",_tag_dyneither(_tmp9CE,sizeof(char),2)))),((_tmp9CD[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4CA),Cyc_PP_cat(_tag_dyneither(_tmp9CD,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL292;_LL297: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4CB=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4CB->tag != 0)goto _LL299;else{_tmp4CC=_tmp4CB->f1;}}_LL298:
# 1794
 s=Cyc_Absynpp_vardecl2doc(_tmp4CC);
goto _LL292;_LL299: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4CD=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4CD->tag != 4)goto _LL29B;else{_tmp4CE=_tmp4CD->f1;_tmp4CF=_tmp4CD->f2;_tmp4D0=_tmp4CD->f3;_tmp4D1=_tmp4CD->f4;}}_LL29A:
# 1797
{const char*_tmp9E1;const char*_tmp9E0;const char*_tmp9DF;const char*_tmp9DE;struct Cyc_PP_Doc*_tmp9DD[3];const char*_tmp9DC;const char*_tmp9DB;const char*_tmp9DA;struct Cyc_PP_Doc*_tmp9D9[8];s=((_tmp9D9[7]=
# 1805
Cyc_PP_text(((_tmp9DA=";",_tag_dyneither(_tmp9DA,sizeof(char),2)))),((_tmp9D9[6]=
# 1803
_tmp4D1 != 0?
(_tmp9DD[2]=Cyc_PP_text(((_tmp9DC=")",_tag_dyneither(_tmp9DC,sizeof(char),2)))),((_tmp9DD[1]=Cyc_Absynpp_exp2doc(_tmp4D1),((_tmp9DD[0]=Cyc_PP_text(((_tmp9DB=" = open(",_tag_dyneither(_tmp9DB,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9D9[5]=
# 1802
_tmp4D0?Cyc_PP_text(((_tmp9DE=" @resetable",_tag_dyneither(_tmp9DE,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9D9[4]=
# 1801
Cyc_Absynpp_qvar2doc(_tmp4CF->name),((_tmp9D9[3]=
# 1800
Cyc_PP_text(((_tmp9DF=">",_tag_dyneither(_tmp9DF,sizeof(char),2)))),((_tmp9D9[2]=
# 1799
Cyc_Absynpp_tvar2doc(_tmp4CE),((_tmp9D9[1]=
# 1798
Cyc_PP_text(((_tmp9E0="<",_tag_dyneither(_tmp9E0,sizeof(char),2)))),((_tmp9D9[0]=
# 1797
Cyc_PP_text(((_tmp9E1="region",_tag_dyneither(_tmp9E1,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1806
goto _LL292;_LL29B: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4D2=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4D2->tag != 5)goto _LL29D;else{_tmp4D3=_tmp4D2->f1;_tmp4D4=_tmp4D2->f2;}}_LL29C:
# 1808
{const char*_tmp9EA;const char*_tmp9E9;const char*_tmp9E8;const char*_tmp9E7;struct Cyc_PP_Doc*_tmp9E6[6];s=((_tmp9E6[5]=
Cyc_PP_text(((_tmp9E7=";",_tag_dyneither(_tmp9E7,sizeof(char),2)))),((_tmp9E6[4]=Cyc_Absynpp_vardecl2doc(_tmp4D4),((_tmp9E6[3]=
# 1808
Cyc_PP_text(((_tmp9E8="> ",_tag_dyneither(_tmp9E8,sizeof(char),3)))),((_tmp9E6[2]=Cyc_Absynpp_tvar2doc(_tmp4D3),((_tmp9E6[1]=Cyc_PP_text(((_tmp9E9=" <",_tag_dyneither(_tmp9E9,sizeof(char),3)))),((_tmp9E6[0]=Cyc_PP_text(((_tmp9EA="alias",_tag_dyneither(_tmp9EA,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9E6,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 1810
goto _LL292;_LL29D: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4D5=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4D5->tag != 7)goto _LL29F;else{_tmp4D6=_tmp4D5->f1;}}_LL29E:
# 1812
{const char*_tmp9ED;struct Cyc_PP_Doc*_tmp9EC[2];s=((_tmp9EC[1]=Cyc_PP_text(((_tmp9ED=";",_tag_dyneither(_tmp9ED,sizeof(char),2)))),((_tmp9EC[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4D6),Cyc_PP_cat(_tag_dyneither(_tmp9EC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL292;_LL29F: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D7=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4D7->tag != 2)goto _LL2A1;else{_tmp4D8=_tmp4D7->f1;_tmp4D9=_tmp4D7->f3;}}_LL2A0:
# 1815
{const char*_tmp9F4;const char*_tmp9F3;const char*_tmp9F2;struct Cyc_PP_Doc*_tmp9F1[5];s=((_tmp9F1[4]=
# 1819
Cyc_PP_text(((_tmp9F2=";",_tag_dyneither(_tmp9F2,sizeof(char),2)))),((_tmp9F1[3]=
# 1818
Cyc_Absynpp_exp2doc(_tmp4D9),((_tmp9F1[2]=
# 1817
Cyc_PP_text(((_tmp9F3=" = ",_tag_dyneither(_tmp9F3,sizeof(char),4)))),((_tmp9F1[1]=
# 1816
Cyc_Absynpp_pat2doc(_tmp4D8),((_tmp9F1[0]=
# 1815
Cyc_PP_text(((_tmp9F4="let ",_tag_dyneither(_tmp9F4,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F1,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1820
goto _LL292;_LL2A1: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4DA=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4DA->tag != 3)goto _LL2A3;else{_tmp4DB=_tmp4DA->f1;}}_LL2A2:
# 1822
{const char*_tmp9F9;const char*_tmp9F8;struct Cyc_PP_Doc*_tmp9F7[3];s=((_tmp9F7[2]=Cyc_PP_text(((_tmp9F8=";",_tag_dyneither(_tmp9F8,sizeof(char),2)))),((_tmp9F7[1]=Cyc_Absynpp_ids2doc(_tmp4DB),((_tmp9F7[0]=Cyc_PP_text(((_tmp9F9="let ",_tag_dyneither(_tmp9F9,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9F7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL292;_LL2A3: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4DC=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4DC->tag != 8)goto _LL2A5;else{_tmp4DD=_tmp4DC->f1;}}_LL2A4:
# 1825
{const char*_tmp9FC;struct Cyc_PP_Doc*_tmp9FB[2];s=((_tmp9FB[1]=Cyc_PP_text(((_tmp9FC=";",_tag_dyneither(_tmp9FC,sizeof(char),2)))),((_tmp9FB[0]=Cyc_Absynpp_enumdecl2doc(_tmp4DD),Cyc_PP_cat(_tag_dyneither(_tmp9FB,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL292;_LL2A5: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4DE=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4DE->tag != 9)goto _LL2A7;else{_tmp4DF=_tmp4DE->f1;}}_LL2A6: {
# 1828
void*t;
if(_tmp4DF->defn != 0)
t=(void*)_check_null(_tmp4DF->defn);else{
# 1832
t=Cyc_Absyn_new_evar(_tmp4DF->kind,0);}
{const char*_tmpA06;struct Cyc_Core_Opt*_tmpA05;struct Cyc_PP_Doc*_tmpA04[2];const char*_tmpA03;struct Cyc_PP_Doc*_tmpA02[4];s=((_tmpA02[3]=
# 1840
Cyc_PP_text(((_tmpA03=";",_tag_dyneither(_tmpA03,sizeof(char),2)))),((_tmpA02[2]=
# 1839
Cyc_Absynpp_atts2doc(_tmp4DF->atts),((_tmpA02[1]=
# 1834
Cyc_Absynpp_tqtd2doc(_tmp4DF->tq,t,(
# 1836
(_tmpA05=_cycalloc(sizeof(*_tmpA05)),((_tmpA05->v=((_tmpA04[1]=
Cyc_Absynpp_tvars2doc(_tmp4DF->tvs),((_tmpA04[0]=
# 1836
Cyc_Absynpp_typedef_name2bolddoc(_tmp4DF->name),Cyc_PP_cat(_tag_dyneither(_tmpA04,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA05))))),((_tmpA02[0]=
# 1833
Cyc_PP_text(((_tmpA06="typedef ",_tag_dyneither(_tmpA06,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1842
goto _LL292;}_LL2A7: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4E0=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4E0->tag != 10)goto _LL2A9;else{_tmp4E1=_tmp4E0->f1;_tmp4E2=_tmp4E0->f2;}}_LL2A8:
# 1844
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4E1);
{const char*_tmpA0B;const char*_tmpA0A;struct Cyc_PP_Doc*_tmpA09[8];s=((_tmpA09[7]=
# 1851
Cyc_Absynpp_rb(),((_tmpA09[6]=
# 1850
Cyc_PP_line_doc(),((_tmpA09[5]=
# 1849
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0A="",_tag_dyneither(_tmpA0A,sizeof(char),1))),_tmp4E2),((_tmpA09[4]=
# 1848
Cyc_PP_line_doc(),((_tmpA09[3]=
# 1847
Cyc_Absynpp_lb(),((_tmpA09[2]=Cyc_PP_blank_doc(),((_tmpA09[1]=
# 1846
Cyc_PP_textptr(_tmp4E1),((_tmpA09[0]=
# 1845
Cyc_PP_text(((_tmpA0B="namespace ",_tag_dyneither(_tmpA0B,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA09,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1852
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL292;_LL2A9: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4E3=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4E3->tag != 11)goto _LL2AB;else{_tmp4E4=_tmp4E3->f1;_tmp4E5=_tmp4E3->f2;}}_LL2AA:
# 1855
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA10;const char*_tmpA0F;struct Cyc_PP_Doc*_tmpA0E[8];s=((_tmpA0E[7]=
# 1862
Cyc_Absynpp_rb(),((_tmpA0E[6]=
# 1861
Cyc_PP_line_doc(),((_tmpA0E[5]=
# 1860
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0F="",_tag_dyneither(_tmpA0F,sizeof(char),1))),_tmp4E5),((_tmpA0E[4]=
# 1859
Cyc_PP_line_doc(),((_tmpA0E[3]=
# 1858
Cyc_Absynpp_lb(),((_tmpA0E[2]=Cyc_PP_blank_doc(),((_tmpA0E[1]=
# 1857
Cyc_Absynpp_qvar2doc(_tmp4E4),((_tmpA0E[0]=
# 1856
Cyc_PP_text(((_tmpA10="using ",_tag_dyneither(_tmpA10,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA0E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1864
const char*_tmpA1B;const char*_tmpA1A;const char*_tmpA19;const char*_tmpA18;const char*_tmpA17;struct Cyc_PP_Doc*_tmpA16[11];s=((_tmpA16[10]=
# 1874
Cyc_PP_text(((_tmpA17=" */",_tag_dyneither(_tmpA17,sizeof(char),4)))),((_tmpA16[9]=
# 1873
Cyc_Absynpp_rb(),((_tmpA16[8]=
# 1872
Cyc_PP_text(((_tmpA18="/* ",_tag_dyneither(_tmpA18,sizeof(char),4)))),((_tmpA16[7]=
# 1871
Cyc_PP_line_doc(),((_tmpA16[6]=
# 1870
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA19="",_tag_dyneither(_tmpA19,sizeof(char),1))),_tmp4E5),((_tmpA16[5]=
# 1869
Cyc_PP_line_doc(),((_tmpA16[4]=
# 1868
Cyc_PP_text(((_tmpA1A=" */",_tag_dyneither(_tmpA1A,sizeof(char),4)))),((_tmpA16[3]=
# 1867
Cyc_Absynpp_lb(),((_tmpA16[2]=
# 1866
Cyc_PP_blank_doc(),((_tmpA16[1]=
# 1865
Cyc_Absynpp_qvar2doc(_tmp4E4),((_tmpA16[0]=
# 1864
Cyc_PP_text(((_tmpA1B="/* using ",_tag_dyneither(_tmpA1B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA16,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1875
goto _LL292;_LL2AB: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4E6=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4E6->tag != 12)goto _LL2AD;else{_tmp4E7=_tmp4E6->f1;}}_LL2AC:
# 1877
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA20;const char*_tmpA1F;struct Cyc_PP_Doc*_tmpA1E[6];s=((_tmpA1E[5]=
# 1883
Cyc_Absynpp_rb(),((_tmpA1E[4]=
# 1882
Cyc_PP_line_doc(),((_tmpA1E[3]=
# 1881
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1F="",_tag_dyneither(_tmpA1F,sizeof(char),1))),_tmp4E7),((_tmpA1E[2]=
# 1880
Cyc_PP_line_doc(),((_tmpA1E[1]=
# 1879
Cyc_Absynpp_lb(),((_tmpA1E[0]=
# 1878
Cyc_PP_text(((_tmpA20="extern \"C\" ",_tag_dyneither(_tmpA20,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA1E,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1885
const char*_tmpA2B;const char*_tmpA2A;const char*_tmpA29;const char*_tmpA28;const char*_tmpA27;struct Cyc_PP_Doc*_tmpA26[9];s=((_tmpA26[8]=
# 1893
Cyc_PP_text(((_tmpA27=" */",_tag_dyneither(_tmpA27,sizeof(char),4)))),((_tmpA26[7]=
# 1892
Cyc_Absynpp_rb(),((_tmpA26[6]=
# 1891
Cyc_PP_text(((_tmpA28="/* ",_tag_dyneither(_tmpA28,sizeof(char),4)))),((_tmpA26[5]=
# 1890
Cyc_PP_line_doc(),((_tmpA26[4]=
# 1889
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA29="",_tag_dyneither(_tmpA29,sizeof(char),1))),_tmp4E7),((_tmpA26[3]=
# 1888
Cyc_PP_line_doc(),((_tmpA26[2]=
# 1887
Cyc_PP_text(((_tmpA2A=" */",_tag_dyneither(_tmpA2A,sizeof(char),4)))),((_tmpA26[1]=
# 1886
Cyc_Absynpp_lb(),((_tmpA26[0]=
# 1885
Cyc_PP_text(((_tmpA2B="/* extern \"C\" ",_tag_dyneither(_tmpA2B,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1894
goto _LL292;_LL2AD: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E8=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4E8->tag != 13)goto _LL2AF;else{_tmp4E9=_tmp4E8->f1;_tmp4EA=_tmp4E8->f2;}}_LL2AE:
# 1896
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4EA != 0){
const char*_tmpA30;const char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[7];exs_doc=((_tmpA2E[6]=
# 1901
Cyc_Absynpp_rb(),((_tmpA2E[5]=Cyc_PP_line_doc(),((_tmpA2E[4]=
# 1900
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA2F=",",_tag_dyneither(_tmpA2F,sizeof(char),2))),_tmp4EA),((_tmpA2E[3]=Cyc_PP_line_doc(),((_tmpA2E[2]=
# 1899
Cyc_Absynpp_lb(),((_tmpA2E[1]=Cyc_PP_text(((_tmpA30=" export ",_tag_dyneither(_tmpA30,sizeof(char),9)))),((_tmpA2E[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1903
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA35;const char*_tmpA34;struct Cyc_PP_Doc*_tmpA33[6];s=((_tmpA33[5]=exs_doc,((_tmpA33[4]=
# 1908
Cyc_PP_line_doc(),((_tmpA33[3]=
# 1907
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA34="",_tag_dyneither(_tmpA34,sizeof(char),1))),_tmp4E9),((_tmpA33[2]=
# 1906
Cyc_PP_line_doc(),((_tmpA33[1]=
# 1905
Cyc_Absynpp_lb(),((_tmpA33[0]=
# 1904
Cyc_PP_text(((_tmpA35="extern \"C include\" ",_tag_dyneither(_tmpA35,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA33,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1911
const char*_tmpA40;const char*_tmpA3F;const char*_tmpA3E;const char*_tmpA3D;const char*_tmpA3C;struct Cyc_PP_Doc*_tmpA3B[9];s=((_tmpA3B[8]=
# 1919
Cyc_PP_text(((_tmpA3C=" */",_tag_dyneither(_tmpA3C,sizeof(char),4)))),((_tmpA3B[7]=
# 1918
Cyc_Absynpp_rb(),((_tmpA3B[6]=
# 1917
Cyc_PP_text(((_tmpA3D="/* ",_tag_dyneither(_tmpA3D,sizeof(char),4)))),((_tmpA3B[5]=
# 1916
Cyc_PP_line_doc(),((_tmpA3B[4]=
# 1915
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA3E="",_tag_dyneither(_tmpA3E,sizeof(char),1))),_tmp4E9),((_tmpA3B[3]=
# 1914
Cyc_PP_line_doc(),((_tmpA3B[2]=
# 1913
Cyc_PP_text(((_tmpA3F=" */",_tag_dyneither(_tmpA3F,sizeof(char),4)))),((_tmpA3B[1]=
# 1912
Cyc_Absynpp_lb(),((_tmpA3B[0]=
# 1911
Cyc_PP_text(((_tmpA40="/* extern \"C include\" ",_tag_dyneither(_tmpA40,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA3B,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1920
goto _LL292;_LL2AF: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4EB=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4EB->tag != 14)goto _LL2B1;}_LL2B0:
# 1922
{const char*_tmpA43;struct Cyc_PP_Doc*_tmpA42[2];s=((_tmpA42[1]=Cyc_Absynpp_lb(),((_tmpA42[0]=Cyc_PP_text(((_tmpA43="__cyclone_port_on__;",_tag_dyneither(_tmpA43,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA42,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL292;_LL2B1: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4EC=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4C6;if(_tmp4EC->tag != 15)goto _LL292;}_LL2B2:
# 1925
{const char*_tmpA46;struct Cyc_PP_Doc*_tmpA45[2];s=((_tmpA45[1]=Cyc_Absynpp_lb(),((_tmpA45[0]=Cyc_PP_text(((_tmpA46="__cyclone_port_off__;",_tag_dyneither(_tmpA46,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA45,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL292;_LL292:;}
# 1929
return s;}
# 1932
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2BC: {
const char*_tmpA47;return Cyc_PP_text(((_tmpA47="static ",_tag_dyneither(_tmpA47,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2BD:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2BE: {
const char*_tmpA48;return Cyc_PP_text(((_tmpA48="extern ",_tag_dyneither(_tmpA48,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2BF: {
const char*_tmpA49;return Cyc_PP_text(((_tmpA49="extern \"C\" ",_tag_dyneither(_tmpA49,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C0: {
const char*_tmpA4A;return Cyc_PP_text(((_tmpA4A="abstract ",_tag_dyneither(_tmpA4A,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2C1: {
const char*_tmpA4B;return Cyc_PP_text(((_tmpA4B="register ",_tag_dyneither(_tmpA4B,sizeof(char),10))));}}}
# 1945
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp542=t;struct Cyc_Absyn_Tvar*_tmp544;struct Cyc_List_List*_tmp546;_LL2C4: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp542;if(_tmp543->tag != 2)goto _LL2C6;else{_tmp544=_tmp543->f1;}}_LL2C5:
 return Cyc_Tcutil_is_temp_tvar(_tmp544);_LL2C6: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp545=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp542;if(_tmp545->tag != 24)goto _LL2C8;else{_tmp546=_tmp545->f1;}}_LL2C7:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp546);_LL2C8:;_LL2C9:
 return 0;_LL2C3:;}
# 1957
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp547=t;struct Cyc_Absyn_Typedefdecl*_tmp54B;void*_tmp54C;_LL2CB: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp547;if(_tmp548->tag != 12)goto _LL2CD;}_LL2CC:
 return 1;_LL2CD: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp549=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp547;if(_tmp549->tag != 14)goto _LL2CF;}_LL2CE:
 return 1;_LL2CF:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp54A=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp547;if(_tmp54A->tag != 17)goto _LL2D1;else{_tmp54B=_tmp54A->f3;_tmp54C=(void*)_tmp54A->f4;}}if(!(_tmp54C != 0))goto _LL2D1;_LL2D0:
# 1964
 return Cyc_Absynpp_is_anon_aggrtype(_tmp54C);_LL2D1:;_LL2D2:
 return 0;_LL2CA:;}
# 1971
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1974
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA4C;struct _tuple16 _stmttmp1D=(_tmpA4C.f1=(void*)tms->hd,((_tmpA4C.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA4C)));struct _tuple16 _tmp54D=_stmttmp1D;_LL2D4:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp54E=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp54D.f1;if(_tmp54E->tag != 2)goto _LL2D6;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54F=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp54D.f2;if(_tmp54F->tag != 3)goto _LL2D6;};_LL2D5: {
# 1977
struct Cyc_List_List*_tmpA4F;struct Cyc_List_List*_tmpA4E;return(_tmpA4E=_region_malloc(r,sizeof(*_tmpA4E)),((_tmpA4E->hd=(void*)tms->hd,((_tmpA4E->tl=((_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA4F->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA4F)))))),_tmpA4E)))));}_LL2D6:;_LL2D7: {
struct Cyc_List_List*_tmpA50;return(_tmpA50=_region_malloc(r,sizeof(*_tmpA50)),((_tmpA50->hd=atts,((_tmpA50->tl=tms,_tmpA50)))));}_LL2D3:;}else{
# 1980
struct Cyc_List_List*_tmpA51;return(_tmpA51=_region_malloc(r,sizeof(*_tmpA51)),((_tmpA51->hd=atts,((_tmpA51->tl=tms,_tmpA51)))));}}
# 1985
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1987
void*_tmp555=t;void*_tmp557;struct Cyc_Absyn_Tqual _tmp558;struct Cyc_Absyn_Exp*_tmp559;union Cyc_Absyn_Constraint*_tmp55A;unsigned int _tmp55B;void*_tmp55D;struct Cyc_Absyn_Tqual _tmp55E;struct Cyc_Absyn_PtrAtts _tmp55F;struct Cyc_List_List*_tmp561;void*_tmp562;struct Cyc_Absyn_Tqual _tmp563;void*_tmp564;struct Cyc_List_List*_tmp565;int _tmp566;struct Cyc_Absyn_VarargInfo*_tmp567;struct Cyc_List_List*_tmp568;struct Cyc_List_List*_tmp569;struct Cyc_Core_Opt*_tmp56B;void*_tmp56C;int _tmp56D;struct _tuple0*_tmp56F;struct Cyc_List_List*_tmp570;struct Cyc_Absyn_Typedefdecl*_tmp571;void*_tmp572;_LL2D9: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp555;if(_tmp556->tag != 8)goto _LL2DB;else{_tmp557=(_tmp556->f1).elt_type;_tmp558=(_tmp556->f1).tq;_tmp559=(_tmp556->f1).num_elts;_tmp55A=(_tmp556->f1).zero_term;_tmp55B=(_tmp556->f1).zt_loc;}}_LL2DA: {
# 1990
struct _tuple12 _stmttmp21=Cyc_Absynpp_to_tms(r,_tmp558,_tmp557);struct Cyc_Absyn_Tqual _tmp574;void*_tmp575;struct Cyc_List_List*_tmp576;struct _tuple12 _tmp573=_stmttmp21;_tmp574=_tmp573.f1;_tmp575=_tmp573.f2;_tmp576=_tmp573.f3;{
void*tm;
if(_tmp559 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA54;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA53;tm=(void*)((_tmpA53=_region_malloc(r,sizeof(*_tmpA53)),((_tmpA53[0]=((_tmpA54.tag=0,((_tmpA54.f1=_tmp55A,((_tmpA54.f2=_tmp55B,_tmpA54)))))),_tmpA53))));}else{
# 1995
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA57;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA56;tm=(void*)((_tmpA56=_region_malloc(r,sizeof(*_tmpA56)),((_tmpA56[0]=((_tmpA57.tag=1,((_tmpA57.f1=_tmp559,((_tmpA57.f2=_tmp55A,((_tmpA57.f3=_tmp55B,_tmpA57)))))))),_tmpA56))));}{
struct Cyc_List_List*_tmpA5A;struct _tuple12 _tmpA59;return(_tmpA59.f1=_tmp574,((_tmpA59.f2=_tmp575,((_tmpA59.f3=((_tmpA5A=_region_malloc(r,sizeof(*_tmpA5A)),((_tmpA5A->hd=tm,((_tmpA5A->tl=_tmp576,_tmpA5A)))))),_tmpA59)))));};};}_LL2DB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp55C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp555;if(_tmp55C->tag != 5)goto _LL2DD;else{_tmp55D=(_tmp55C->f1).elt_typ;_tmp55E=(_tmp55C->f1).elt_tq;_tmp55F=(_tmp55C->f1).ptr_atts;}}_LL2DC: {
# 1999
struct _tuple12 _stmttmp20=Cyc_Absynpp_to_tms(r,_tmp55E,_tmp55D);struct Cyc_Absyn_Tqual _tmp57E;void*_tmp57F;struct Cyc_List_List*_tmp580;struct _tuple12 _tmp57D=_stmttmp20;_tmp57E=_tmp57D.f1;_tmp57F=_tmp57D.f2;_tmp580=_tmp57D.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA60;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA5F;struct Cyc_List_List*_tmpA5E;_tmp580=((_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E->hd=(void*)((_tmpA60=_region_malloc(r,sizeof(*_tmpA60)),((_tmpA60[0]=((_tmpA5F.tag=2,((_tmpA5F.f1=_tmp55F,((_tmpA5F.f2=tq,_tmpA5F)))))),_tmpA60)))),((_tmpA5E->tl=_tmp580,_tmpA5E))))));}{
struct _tuple12 _tmpA61;return(_tmpA61.f1=_tmp57E,((_tmpA61.f2=_tmp57F,((_tmpA61.f3=_tmp580,_tmpA61)))));};}_LL2DD: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp560=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp555;if(_tmp560->tag != 9)goto _LL2DF;else{_tmp561=(_tmp560->f1).tvars;_tmp562=(_tmp560->f1).effect;_tmp563=(_tmp560->f1).ret_tqual;_tmp564=(_tmp560->f1).ret_typ;_tmp565=(_tmp560->f1).args;_tmp566=(_tmp560->f1).c_varargs;_tmp567=(_tmp560->f1).cyc_varargs;_tmp568=(_tmp560->f1).rgn_po;_tmp569=(_tmp560->f1).attributes;}}_LL2DE:
# 2005
 if(!Cyc_Absynpp_print_all_tvars){
# 2009
if(_tmp562 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp562)){
_tmp562=0;
_tmp561=0;}}else{
# 2014
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2017
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp561);}{
# 2022
struct _tuple12 _stmttmp1E=Cyc_Absynpp_to_tms(r,_tmp563,_tmp564);struct Cyc_Absyn_Tqual _tmp586;void*_tmp587;struct Cyc_List_List*_tmp588;struct _tuple12 _tmp585=_stmttmp1E;_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;_tmp588=_tmp585.f3;{
struct Cyc_List_List*tms=_tmp588;
# 2033 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2E5:
# 2035
 if(_tmp569 != 0){
# 2038
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA64;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA63;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=5,((_tmpA64.f1=0,((_tmpA64.f2=_tmp569,_tmpA64)))))),_tmpA63)))),tms);}
# 2040
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA73;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA72;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA71;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA70;struct Cyc_List_List*_tmpA6F;tms=((_tmpA6F=_region_malloc(r,sizeof(*_tmpA6F)),((_tmpA6F->hd=(void*)((_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73[0]=(
(_tmpA70.tag=3,((_tmpA70.f1=(void*)((_tmpA72=_region_malloc(r,sizeof(*_tmpA72)),((_tmpA72[0]=((_tmpA71.tag=1,((_tmpA71.f1=_tmp565,((_tmpA71.f2=_tmp566,((_tmpA71.f3=_tmp567,((_tmpA71.f4=_tmp562,((_tmpA71.f5=_tmp568,_tmpA71)))))))))))),_tmpA72)))),_tmpA70)))),_tmpA73)))),((_tmpA6F->tl=tms,_tmpA6F))))));}
# 2044
break;case Cyc_Cyclone_Vc_c: _LL2E6:
# 2046
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA81;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA80;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA7F;struct Cyc_List_List*_tmpA7E;tms=((_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E->hd=(void*)((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=(
(_tmpA7F.tag=3,((_tmpA7F.f1=(void*)((_tmpA81=_region_malloc(r,sizeof(*_tmpA81)),((_tmpA81[0]=((_tmpA80.tag=1,((_tmpA80.f1=_tmp565,((_tmpA80.f2=_tmp566,((_tmpA80.f3=_tmp567,((_tmpA80.f4=_tmp562,((_tmpA80.f5=_tmp568,_tmpA80)))))))))))),_tmpA81)))),_tmpA7F)))),_tmpA82)))),((_tmpA7E->tl=tms,_tmpA7E))))));}
# 2050
for(0;_tmp569 != 0;_tmp569=_tmp569->tl){
void*_stmttmp1F=(void*)_tmp569->hd;void*_tmp595=_stmttmp1F;_LL2E9: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp596=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp595;if(_tmp596->tag != 1)goto _LL2EB;}_LL2EA:
 goto _LL2EC;_LL2EB: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp597=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp595;if(_tmp597->tag != 2)goto _LL2ED;}_LL2EC:
 goto _LL2EE;_LL2ED: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp598=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp595;if(_tmp598->tag != 3)goto _LL2EF;}_LL2EE:
# 2055
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA8C;struct Cyc_List_List*_tmpA8B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA8A;struct Cyc_List_List*_tmpA89;tms=((_tmpA89=_region_malloc(r,sizeof(*_tmpA89)),((_tmpA89->hd=(void*)((_tmpA8C=_region_malloc(r,sizeof(*_tmpA8C)),((_tmpA8C[0]=((_tmpA8A.tag=5,((_tmpA8A.f1=0,((_tmpA8A.f2=((_tmpA8B=_cycalloc(sizeof(*_tmpA8B)),((_tmpA8B->hd=(void*)_tmp569->hd,((_tmpA8B->tl=0,_tmpA8B)))))),_tmpA8A)))))),_tmpA8C)))),((_tmpA89->tl=tms,_tmpA89))))));}
goto AfterAtts;_LL2EF:;_LL2F0:
 goto _LL2E8;_LL2E8:;}
# 2059
break;}
# 2062
AfterAtts:
 if(_tmp561 != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA92;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA91;struct Cyc_List_List*_tmpA90;tms=((_tmpA90=_region_malloc(r,sizeof(*_tmpA90)),((_tmpA90->hd=(void*)((_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92[0]=((_tmpA91.tag=4,((_tmpA91.f1=_tmp561,((_tmpA91.f2=0,((_tmpA91.f3=1,_tmpA91)))))))),_tmpA92)))),((_tmpA90->tl=tms,_tmpA90))))));}{
struct _tuple12 _tmpA93;return(_tmpA93.f1=_tmp586,((_tmpA93.f2=_tmp587,((_tmpA93.f3=tms,_tmpA93)))));};};};_LL2DF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp56A=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp555;if(_tmp56A->tag != 1)goto _LL2E1;else{_tmp56B=_tmp56A->f1;_tmp56C=(void*)_tmp56A->f2;_tmp56D=_tmp56A->f3;}}_LL2E0:
# 2068
 if(_tmp56C == 0){
struct _tuple12 _tmpA94;return(_tmpA94.f1=tq,((_tmpA94.f2=t,((_tmpA94.f3=0,_tmpA94)))));}else{
# 2071
return Cyc_Absynpp_to_tms(r,tq,_tmp56C);}_LL2E1: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp56E=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp555;if(_tmp56E->tag != 17)goto _LL2E3;else{_tmp56F=_tmp56E->f1;_tmp570=_tmp56E->f2;_tmp571=_tmp56E->f3;_tmp572=(void*)_tmp56E->f4;}}_LL2E2:
# 2076
 if((_tmp572 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp572)){
struct _tuple12 _tmpA95;return(_tmpA95.f1=tq,((_tmpA95.f2=t,((_tmpA95.f3=0,_tmpA95)))));}else{
# 2079
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp572);}_LL2E3:;_LL2E4: {
# 2084
struct _tuple12 _tmpA96;return(_tmpA96.f1=tq,((_tmpA96.f2=t,((_tmpA96.f3=0,_tmpA96)))));}_LL2D8:;}
# 2088
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2090
void*_tmp5A4=t;void*_tmp5A6;void*_tmp5A8;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A4;if(_tmp5A5->tag != 1)goto _LL2F4;else{_tmp5A6=(void*)_tmp5A5->f2;}}if(!(_tmp5A6 != 0))goto _LL2F4;_LL2F3:
 return Cyc_Absynpp_is_char_ptr(_tmp5A6);_LL2F4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A7=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4;if(_tmp5A7->tag != 5)goto _LL2F6;else{_tmp5A8=(_tmp5A7->f1).elt_typ;}}_LL2F5:
# 2093
 L: {
void*_tmp5A9=_tmp5A8;void*_tmp5AB;void*_tmp5AD;_LL2F9:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A9;if(_tmp5AA->tag != 1)goto _LL2FB;else{_tmp5AB=(void*)_tmp5AA->f2;}}if(!(_tmp5AB != 0))goto _LL2FB;_LL2FA:
 _tmp5A8=_tmp5AB;goto L;_LL2FB:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A9;if(_tmp5AC->tag != 17)goto _LL2FD;else{_tmp5AD=(void*)_tmp5AC->f4;}}if(!(_tmp5AD != 0))goto _LL2FD;_LL2FC:
 _tmp5A8=_tmp5AD;goto L;_LL2FD: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AE=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A9;if(_tmp5AE->tag != 6)goto _LL2FF;else{if(_tmp5AE->f2 != Cyc_Absyn_Char_sz)goto _LL2FF;}}_LL2FE:
 return 1;_LL2FF:;_LL300:
 return 0;_LL2F8:;}_LL2F6:;_LL2F7:
# 2100
 return 0;_LL2F1:;}
# 2104
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5AF=_new_region("temp");struct _RegionHandle*temp=& _tmp5AF;_push_region(temp);
{struct _tuple12 _stmttmp22=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5B1;void*_tmp5B2;struct Cyc_List_List*_tmp5B3;struct _tuple12 _tmp5B0=_stmttmp22;_tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;_tmp5B3=_tmp5B0.f3;
_tmp5B3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5B3);
if(_tmp5B3 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA97[2];struct Cyc_PP_Doc*_tmp5B5=(_tmpA97[1]=Cyc_Absynpp_ntyp2doc(_tmp5B2),((_tmpA97[0]=Cyc_Absynpp_tqual2doc(_tmp5B1),Cyc_PP_cat(_tag_dyneither(_tmpA97,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5B5;}else{
# 2111
const char*_tmpA9A;struct Cyc_PP_Doc*_tmpA99[4];struct Cyc_PP_Doc*_tmp5B8=
(_tmpA99[3]=
# 2115
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5B3),((_tmpA99[2]=
# 2114
Cyc_PP_text(((_tmpA9A=" ",_tag_dyneither(_tmpA9A,sizeof(char),2)))),((_tmpA99[1]=
# 2113
Cyc_Absynpp_ntyp2doc(_tmp5B2),((_tmpA99[0]=
# 2112
Cyc_Absynpp_tqual2doc(_tmp5B1),Cyc_PP_cat(_tag_dyneither(_tmpA99,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B8;}}
# 2106
;_pop_region(temp);}
# 2119
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp5B9=f->requires_clause;
if((unsigned int)_tmp5B9){
const char*_tmpA9D;struct Cyc_PP_Doc*_tmpA9C[2];requires_doc=((_tmpA9C[1]=Cyc_Absynpp_exp2doc(_tmp5B9),((_tmpA9C[0]=Cyc_PP_text(((_tmpA9D="@requires ",_tag_dyneither(_tmpA9D,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA9C,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2125
requires_doc=Cyc_PP_nil_doc();}
# 2127
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL301:
# 2130
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAA4;const char*_tmpAA3;const char*_tmpAA2;struct Cyc_PP_Doc*_tmpAA1[5];return(_tmpAA1[4]=
# 2133
Cyc_PP_text(((_tmpAA2=";",_tag_dyneither(_tmpAA2,sizeof(char),2)))),((_tmpAA1[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAA1[2]=
# 2132
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAA1[1]=Cyc_PP_text(((_tmpAA3=":",_tag_dyneither(_tmpAA3,sizeof(char),2)))),((_tmpAA1[0]=
# 2131
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA4=_cycalloc(sizeof(*_tmpAA4)),((_tmpAA4->v=Cyc_PP_textptr(f->name),_tmpAA4))))),Cyc_PP_cat(_tag_dyneither(_tmpAA1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2135
struct Cyc_Core_Opt*_tmpAA9;const char*_tmpAA8;struct Cyc_PP_Doc*_tmpAA7[4];return(_tmpAA7[3]=
Cyc_PP_text(((_tmpAA8=";",_tag_dyneither(_tmpAA8,sizeof(char),2)))),((_tmpAA7[2]=requires_doc,((_tmpAA7[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAA7[0]=
# 2135
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA9=_cycalloc(sizeof(*_tmpAA9)),((_tmpAA9->v=Cyc_PP_textptr(f->name),_tmpAA9))))),Cyc_PP_cat(_tag_dyneither(_tmpAA7,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL302:
# 2138
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAB0;const char*_tmpAAF;const char*_tmpAAE;struct Cyc_PP_Doc*_tmpAAD[5];return(_tmpAAD[4]=
# 2141
Cyc_PP_text(((_tmpAAE=";",_tag_dyneither(_tmpAAE,sizeof(char),2)))),((_tmpAAD[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAAD[2]=Cyc_PP_text(((_tmpAAF=":",_tag_dyneither(_tmpAAF,sizeof(char),2)))),((_tmpAAD[1]=
# 2140
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAB0=_cycalloc(sizeof(*_tmpAB0)),((_tmpAB0->v=Cyc_PP_textptr(f->name),_tmpAB0))))),((_tmpAAD[0]=
# 2139
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAAD,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2143
struct Cyc_Core_Opt*_tmpAB5;const char*_tmpAB4;struct Cyc_PP_Doc*_tmpAB3[4];return(_tmpAB3[3]=
# 2145
Cyc_PP_text(((_tmpAB4=";",_tag_dyneither(_tmpAB4,sizeof(char),2)))),((_tmpAB3[2]=requires_doc,((_tmpAB3[1]=
# 2144
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAB5=_cycalloc(sizeof(*_tmpAB5)),((_tmpAB5->v=Cyc_PP_textptr(f->name),_tmpAB5))))),((_tmpAB3[0]=
# 2143
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAB3,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2150
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAB6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAB6="",_tag_dyneither(_tmpAB6,sizeof(char),1))),fields);}
# 2154
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAB7[3];return(_tmpAB7[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAB7[1]=
# 2155
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAB7[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAB7,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2159
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAB8;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAB8=",",_tag_dyneither(_tmpAB8,sizeof(char),2))),fields);}
# 2168
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpABB;void*_tmpABA;(_tmpABA=0,Cyc_fprintf(f,((_tmpABB="\n",_tag_dyneither(_tmpABB,sizeof(char),2))),_tag_dyneither(_tmpABA,sizeof(void*),0)));};}}
# 2175
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpABC;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpABC="",_tag_dyneither(_tmpABC,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2178
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2182
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_tvar2doc(t),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2193
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
