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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 71 "tcutil.h"
void*Cyc_Tcutil_compress(void*t);
# 316 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 318
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 373
int Cyc_Tcutil_is_array(void*t);
# 377
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
struct Cyc_List_List*_tmp5F2;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5F2=_cycalloc(sizeof(*_tmp5F2)),((_tmp5F2->hd=v,((_tmp5F2->tl=0,_tmp5F2)))))));}
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
const char*_tmp5F3;return(_tmp5F3="\\a",_tag_dyneither(_tmp5F3,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5F4;return(_tmp5F4="\\b",_tag_dyneither(_tmp5F4,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5F5;return(_tmp5F5="\\f",_tag_dyneither(_tmp5F5,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5F6;return(_tmp5F6="\\n",_tag_dyneither(_tmp5F6,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5F7;return(_tmp5F7="\\r",_tag_dyneither(_tmp5F7,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5F8;return(_tmp5F8="\\t",_tag_dyneither(_tmp5F8,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5F9;return(_tmp5F9="\\v",_tag_dyneither(_tmp5F9,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5FA;return(_tmp5FA="\\\\",_tag_dyneither(_tmp5FA,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5FB;return(_tmp5FB="\"",_tag_dyneither(_tmp5FB,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5FC;return(_tmp5FC="\\'",_tag_dyneither(_tmp5FC,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5FF;char _tmp5FE;struct _dyneither_ptr _tmp5FD;(_tmp5FD=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5FE=*((char*)_check_dyneither_subscript(_tmp5FD,sizeof(char),0)),((_tmp5FF=c,((_get_dyneither_size(_tmp5FD,sizeof(char))== 1  && (_tmp5FE == '\000'  && _tmp5FF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FD.curr)=_tmp5FF)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp602;char _tmp601;struct _dyneither_ptr _tmp600;(_tmp600=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp601=*((char*)_check_dyneither_subscript(_tmp600,sizeof(char),0)),((_tmp602='\\',((_get_dyneither_size(_tmp600,sizeof(char))== 1  && (_tmp601 == '\000'  && _tmp602 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp600.curr)=_tmp602)))))));}
{char _tmp605;char _tmp604;struct _dyneither_ptr _tmp603;(_tmp603=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp604=*((char*)_check_dyneither_subscript(_tmp603,sizeof(char),0)),((_tmp605=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp603,sizeof(char))== 1  && (_tmp604 == '\000'  && _tmp605 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp603.curr)=_tmp605)))))));}
{char _tmp608;char _tmp607;struct _dyneither_ptr _tmp606;(_tmp606=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp607=*((char*)_check_dyneither_subscript(_tmp606,sizeof(char),0)),((_tmp608=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp606,sizeof(char))== 1  && (_tmp607 == '\000'  && _tmp608 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp606.curr)=_tmp608)))))));}
{char _tmp60B;char _tmp60A;struct _dyneither_ptr _tmp609;(_tmp609=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp60A=*((char*)_check_dyneither_subscript(_tmp609,sizeof(char),0)),((_tmp60B=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp609,sizeof(char))== 1  && (_tmp60A == '\000'  && _tmp60B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp609.curr)=_tmp60B)))))));}
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
char _tmp1D=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp1E=_tmp1D;char _tmp1F;_LLD: if(_tmp1E != '\a')goto _LLF;_LLE:
 goto _LL10;_LLF: if(_tmp1E != '\b')goto _LL11;_LL10:
 goto _LL12;_LL11: if(_tmp1E != '\f')goto _LL13;_LL12:
 goto _LL14;_LL13: if(_tmp1E != '\n')goto _LL15;_LL14:
 goto _LL16;_LL15: if(_tmp1E != '\r')goto _LL17;_LL16:
 goto _LL18;_LL17: if(_tmp1E != '\t')goto _LL19;_LL18:
 goto _LL1A;_LL19: if(_tmp1E != '\v')goto _LL1B;_LL1A:
 goto _LL1C;_LL1B: if(_tmp1E != '\\')goto _LL1D;_LL1C:
 goto _LL1E;_LL1D: if(_tmp1E != '"')goto _LL1F;_LL1E:
 len +=2;goto _LLC;_LL1F: _tmp1F=_tmp1E;_LL20:
# 322
 if(_tmp1F >= ' '  && _tmp1F <= '~')++ len;else{
len +=4;}
goto _LLC;_LLC:;}}{
# 327
struct _dyneither_ptr t=Cyc_Core_new_string((unsigned int)(len + 1));
int j=0;
{int i=0;for(0;i <= n;++ i){
char _tmp20=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));char _tmp21=_tmp20;char _tmp22;_LL22: if(_tmp21 != '\a')goto _LL24;_LL23:
{char _tmp60E;char _tmp60D;struct _dyneither_ptr _tmp60C;(_tmp60C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60D=*((char*)_check_dyneither_subscript(_tmp60C,sizeof(char),0)),((_tmp60E='\\',((_get_dyneither_size(_tmp60C,sizeof(char))== 1  && (_tmp60D == '\000'  && _tmp60E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60C.curr)=_tmp60E)))))));}{char _tmp611;char _tmp610;struct _dyneither_ptr _tmp60F;(_tmp60F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp610=*((char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0)),((_tmp611='a',((_get_dyneither_size(_tmp60F,sizeof(char))== 1  && (_tmp610 == '\000'  && _tmp611 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60F.curr)=_tmp611)))))));}goto _LL21;_LL24: if(_tmp21 != '\b')goto _LL26;_LL25:
{char _tmp614;char _tmp613;struct _dyneither_ptr _tmp612;(_tmp612=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp613=*((char*)_check_dyneither_subscript(_tmp612,sizeof(char),0)),((_tmp614='\\',((_get_dyneither_size(_tmp612,sizeof(char))== 1  && (_tmp613 == '\000'  && _tmp614 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp612.curr)=_tmp614)))))));}{char _tmp617;char _tmp616;struct _dyneither_ptr _tmp615;(_tmp615=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp616=*((char*)_check_dyneither_subscript(_tmp615,sizeof(char),0)),((_tmp617='b',((_get_dyneither_size(_tmp615,sizeof(char))== 1  && (_tmp616 == '\000'  && _tmp617 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp615.curr)=_tmp617)))))));}goto _LL21;_LL26: if(_tmp21 != '\f')goto _LL28;_LL27:
{char _tmp61A;char _tmp619;struct _dyneither_ptr _tmp618;(_tmp618=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp619=*((char*)_check_dyneither_subscript(_tmp618,sizeof(char),0)),((_tmp61A='\\',((_get_dyneither_size(_tmp618,sizeof(char))== 1  && (_tmp619 == '\000'  && _tmp61A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp618.curr)=_tmp61A)))))));}{char _tmp61D;char _tmp61C;struct _dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61C=*((char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((_tmp61D='f',((_get_dyneither_size(_tmp61B,sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61B.curr)=_tmp61D)))))));}goto _LL21;_LL28: if(_tmp21 != '\n')goto _LL2A;_LL29:
{char _tmp620;char _tmp61F;struct _dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((_tmp620='\\',((_get_dyneither_size(_tmp61E,sizeof(char))== 1  && (_tmp61F == '\000'  && _tmp620 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61E.curr)=_tmp620)))))));}{char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623='n',((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}goto _LL21;_LL2A: if(_tmp21 != '\r')goto _LL2C;_LL2B:
{char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626='\\',((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}{char _tmp629;char _tmp628;struct _dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp628=*((char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629='r',((_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}goto _LL21;_LL2C: if(_tmp21 != '\t')goto _LL2E;_LL2D:
{char _tmp62C;char _tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((_tmp62C='\\',((_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'  && _tmp62C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));}{char _tmp62F;char _tmp62E;struct _dyneither_ptr _tmp62D;(_tmp62D=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62E=*((char*)_check_dyneither_subscript(_tmp62D,sizeof(char),0)),((_tmp62F='t',((_get_dyneither_size(_tmp62D,sizeof(char))== 1  && (_tmp62E == '\000'  && _tmp62F != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62D.curr)=_tmp62F)))))));}goto _LL21;_LL2E: if(_tmp21 != '\v')goto _LL30;_LL2F:
{char _tmp632;char _tmp631;struct _dyneither_ptr _tmp630;(_tmp630=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp631=*((char*)_check_dyneither_subscript(_tmp630,sizeof(char),0)),((_tmp632='\\',((_get_dyneither_size(_tmp630,sizeof(char))== 1  && (_tmp631 == '\000'  && _tmp632 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp630.curr)=_tmp632)))))));}{char _tmp635;char _tmp634;struct _dyneither_ptr _tmp633;(_tmp633=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp634=*((char*)_check_dyneither_subscript(_tmp633,sizeof(char),0)),((_tmp635='v',((_get_dyneither_size(_tmp633,sizeof(char))== 1  && (_tmp634 == '\000'  && _tmp635 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp633.curr)=_tmp635)))))));}goto _LL21;_LL30: if(_tmp21 != '\\')goto _LL32;_LL31:
{char _tmp638;char _tmp637;struct _dyneither_ptr _tmp636;(_tmp636=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp637=*((char*)_check_dyneither_subscript(_tmp636,sizeof(char),0)),((_tmp638='\\',((_get_dyneither_size(_tmp636,sizeof(char))== 1  && (_tmp637 == '\000'  && _tmp638 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp636.curr)=_tmp638)))))));}{char _tmp63B;char _tmp63A;struct _dyneither_ptr _tmp639;(_tmp639=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63A=*((char*)_check_dyneither_subscript(_tmp639,sizeof(char),0)),((_tmp63B='\\',((_get_dyneither_size(_tmp639,sizeof(char))== 1  && (_tmp63A == '\000'  && _tmp63B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp639.curr)=_tmp63B)))))));}goto _LL21;_LL32: if(_tmp21 != '"')goto _LL34;_LL33:
{char _tmp63E;char _tmp63D;struct _dyneither_ptr _tmp63C;(_tmp63C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp63D=*((char*)_check_dyneither_subscript(_tmp63C,sizeof(char),0)),((_tmp63E='\\',((_get_dyneither_size(_tmp63C,sizeof(char))== 1  && (_tmp63D == '\000'  && _tmp63E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63C.curr)=_tmp63E)))))));}{char _tmp641;char _tmp640;struct _dyneither_ptr _tmp63F;(_tmp63F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp640=*((char*)_check_dyneither_subscript(_tmp63F,sizeof(char),0)),((_tmp641='"',((_get_dyneither_size(_tmp63F,sizeof(char))== 1  && (_tmp640 == '\000'  && _tmp641 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp63F.curr)=_tmp641)))))));}goto _LL21;_LL34: _tmp22=_tmp21;_LL35:
# 341
 if(_tmp22 >= ' '  && _tmp22 <= '~'){char _tmp644;char _tmp643;struct _dyneither_ptr _tmp642;(_tmp642=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp643=*((char*)_check_dyneither_subscript(_tmp642,sizeof(char),0)),((_tmp644=_tmp22,((_get_dyneither_size(_tmp642,sizeof(char))== 1  && (_tmp643 == '\000'  && _tmp644 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp642.curr)=_tmp644)))))));}else{
# 343
{char _tmp647;char _tmp646;struct _dyneither_ptr _tmp645;(_tmp645=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp646=*((char*)_check_dyneither_subscript(_tmp645,sizeof(char),0)),((_tmp647='\\',((_get_dyneither_size(_tmp645,sizeof(char))== 1  && (_tmp646 == '\000'  && _tmp647 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp645.curr)=_tmp647)))))));}
{char _tmp64A;char _tmp649;struct _dyneither_ptr _tmp648;(_tmp648=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp649=*((char*)_check_dyneither_subscript(_tmp648,sizeof(char),0)),((_tmp64A=(char)('0' + (_tmp22 >> 6 & 7)),((_get_dyneither_size(_tmp648,sizeof(char))== 1  && (_tmp649 == '\000'  && _tmp64A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp648.curr)=_tmp64A)))))));}
{char _tmp64D;char _tmp64C;struct _dyneither_ptr _tmp64B;(_tmp64B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64C=*((char*)_check_dyneither_subscript(_tmp64B,sizeof(char),0)),((_tmp64D=(char)('0' + (_tmp22 >> 3 & 7)),((_get_dyneither_size(_tmp64B,sizeof(char))== 1  && (_tmp64C == '\000'  && _tmp64D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64B.curr)=_tmp64D)))))));}{
char _tmp650;char _tmp64F;struct _dyneither_ptr _tmp64E;(_tmp64E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp64F=*((char*)_check_dyneither_subscript(_tmp64E,sizeof(char),0)),((_tmp650=(char)('0' + (_tmp22 & 7)),((_get_dyneither_size(_tmp64E,sizeof(char))== 1  && (_tmp64F == '\000'  && _tmp650 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp64E.curr)=_tmp650)))))));};}
# 348
goto _LL21;_LL21:;}}
# 350
return(struct _dyneither_ptr)t;};};};}static char _tmp68[9]="restrict";
# 353
static struct _dyneither_ptr Cyc_Absynpp_restrict_string={_tmp68,_tmp68,_tmp68 + 9};static char _tmp69[9]="volatile";
static struct _dyneither_ptr Cyc_Absynpp_volatile_string={_tmp69,_tmp69,_tmp69 + 9};static char _tmp6A[6]="const";
static struct _dyneither_ptr Cyc_Absynpp_const_str={_tmp6A,_tmp6A,_tmp6A + 6};
static struct _dyneither_ptr*Cyc_Absynpp_restrict_sp=& Cyc_Absynpp_restrict_string;
static struct _dyneither_ptr*Cyc_Absynpp_volatile_sp=& Cyc_Absynpp_volatile_string;
static struct _dyneither_ptr*Cyc_Absynpp_const_sp=& Cyc_Absynpp_const_str;
# 360
struct Cyc_PP_Doc*Cyc_Absynpp_tqual2doc(struct Cyc_Absyn_Tqual tq){
struct Cyc_List_List*l=0;
# 363
if(tq.q_restrict){struct Cyc_List_List*_tmp651;l=((_tmp651=_cycalloc(sizeof(*_tmp651)),((_tmp651->hd=Cyc_Absynpp_restrict_sp,((_tmp651->tl=l,_tmp651))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp652;l=((_tmp652=_cycalloc(sizeof(*_tmp652)),((_tmp652->hd=Cyc_Absynpp_volatile_sp,((_tmp652->tl=l,_tmp652))))));}
if(tq.print_const){struct Cyc_List_List*_tmp653;l=((_tmp653=_cycalloc(sizeof(*_tmp653)),((_tmp653->hd=Cyc_Absynpp_const_sp,((_tmp653->tl=l,_tmp653))))));}{
const char*_tmp656;const char*_tmp655;const char*_tmp654;return Cyc_PP_egroup(((_tmp654="",_tag_dyneither(_tmp654,sizeof(char),1))),((_tmp655=" ",_tag_dyneither(_tmp655,sizeof(char),2))),((_tmp656=" ",_tag_dyneither(_tmp656,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp72;enum Cyc_Absyn_AliasQual _tmp73;struct Cyc_Absyn_Kind*_tmp71=ka;_tmp72=_tmp71->kind;_tmp73=_tmp71->aliasqual;
switch(_tmp72){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp657;return(_tmp657="A",_tag_dyneither(_tmp657,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp658;return(_tmp658="UA",_tag_dyneither(_tmp658,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp659;return(_tmp659="TA",_tag_dyneither(_tmp659,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp65A;return(_tmp65A="M",_tag_dyneither(_tmp65A,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp65B;return(_tmp65B="UM",_tag_dyneither(_tmp65B,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp65C;return(_tmp65C="TM",_tag_dyneither(_tmp65C,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp65D;return(_tmp65D="B",_tag_dyneither(_tmp65D,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp65E;return(_tmp65E="UB",_tag_dyneither(_tmp65E,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp65F;return(_tmp65F="TB",_tag_dyneither(_tmp65F,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp73){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp660;return(_tmp660="R",_tag_dyneither(_tmp660,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp661;return(_tmp661="UR",_tag_dyneither(_tmp661,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp662;return(_tmp662="TR",_tag_dyneither(_tmp662,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp663;return(_tmp663="E",_tag_dyneither(_tmp663,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp664;return(_tmp664="I",_tag_dyneither(_tmp664,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_tmp82=Cyc_Absyn_compress_kb(c);void*_tmp83=_tmp82;struct Cyc_Absyn_Kind*_tmp85;struct Cyc_Absyn_Kind*_tmp88;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp84->tag != 0)goto _LL50;else{_tmp85=_tmp84->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp85);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp86=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp86->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp665;return(_tmp665="{?}",_tag_dyneither(_tmp665,sizeof(char),4));}else{
const char*_tmp666;return(_tmp666="?",_tag_dyneither(_tmp666,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83;if(_tmp87->tag != 2)goto _LL4D;else{_tmp88=_tmp87->f2;}}_LL53: {
const char*_tmp66A;void*_tmp669[1];struct Cyc_String_pa_PrintArg_struct _tmp668;return(struct _dyneither_ptr)((_tmp668.tag=0,((_tmp668.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp88)),((_tmp669[0]=& _tmp668,Cyc_aprintf(((_tmp66A="<=%s",_tag_dyneither(_tmp66A,sizeof(char),5))),_tag_dyneither(_tmp669,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_tmp8E=Cyc_Absyn_compress_kb(c);void*_tmp8F=_tmp8E;struct Cyc_Absyn_Kind*_tmp91;struct Cyc_Absyn_Kind*_tmp94;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp90=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp90->tag != 0)goto _LL57;else{_tmp91=_tmp90->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp91));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp92=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp92->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp66B;return Cyc_PP_text_width(((_tmp66B="{?}",_tag_dyneither(_tmp66B,sizeof(char),4))),1);}else{
const char*_tmp66C;return Cyc_PP_text(((_tmp66C="?",_tag_dyneither(_tmp66C,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp93=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8F;if(_tmp93->tag != 2)goto _LL54;else{_tmp94=_tmp93->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp94));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp66F;const char*_tmp66E;const char*_tmp66D;return Cyc_PP_egroup(((_tmp66D="<",_tag_dyneither(_tmp66D,sizeof(char),2))),((_tmp66E=">",_tag_dyneither(_tmp66E,sizeof(char),2))),((_tmp66F=",",_tag_dyneither(_tmp66F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp9A=*tv->name;
# 432
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 434
const char*_tmp670;struct _dyneither_ptr kstring=(_tmp670="K",_tag_dyneither(_tmp670,sizeof(char),2));
{void*_tmp9B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp9C=_tmp9B;struct Cyc_Absyn_Kind*_tmp9E;struct Cyc_Absyn_Kind*_tmpA0;_LL5C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9D->tag != 2)goto _LL5E;else{_tmp9E=_tmp9D->f2;}}_LL5D:
 _tmpA0=_tmp9E;goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp9C;if(_tmp9F->tag != 0)goto _LL60;else{_tmpA0=_tmp9F->f1;}}_LL5F:
 kstring=Cyc_Absynpp_kind2string(_tmpA0);goto _LL5B;_LL60:;_LL61:
 goto _LL5B;_LL5B:;}{
# 440
const char*_tmp675;void*_tmp674[2];struct Cyc_String_pa_PrintArg_struct _tmp673;struct Cyc_String_pa_PrintArg_struct _tmp672;return Cyc_PP_text((struct _dyneither_ptr)((_tmp672.tag=0,((_tmp672.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp9A,sizeof(char),1)),((_tmp673.tag=0,((_tmp673.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp674[0]=& _tmp673,((_tmp674[1]=& _tmp672,Cyc_aprintf(((_tmp675="`G%s%s",_tag_dyneither(_tmp675,sizeof(char),7))),_tag_dyneither(_tmp674,sizeof(void*),2)))))))))))))));};}
# 442
return Cyc_PP_text(_tmp9A);}
# 445
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA7=_tmpA6;struct Cyc_Absyn_Kind*_tmpAB;struct Cyc_Absyn_Kind*_tmpAD;_LL63: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA8=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA8->tag != 1)goto _LL65;}_LL64:
 goto _LL66;_LL65: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA9=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpA9->tag != 0)goto _LL67;else{if((_tmpA9->f1)->kind != Cyc_Absyn_BoxKind)goto _LL67;if((_tmpA9->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL67;}}_LL66:
 return Cyc_Absynpp_tvar2doc(tv);_LL67: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpAA=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAA->tag != 2)goto _LL69;else{_tmpAB=_tmpAA->f2;}}_LL68:
 _tmpAD=_tmpAB;goto _LL6A;_LL69: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpAC=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA7;if(_tmpAC->tag != 0)goto _LL62;else{_tmpAD=_tmpAC->f1;}}_LL6A: {
const char*_tmp678;struct Cyc_PP_Doc*_tmp677[3];return(_tmp677[2]=Cyc_Absynpp_kind2doc(_tmpAD),((_tmp677[1]=Cyc_PP_text(((_tmp678="::",_tag_dyneither(_tmp678,sizeof(char),3)))),((_tmp677[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp677,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL62:;}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp67B;const char*_tmp67A;const char*_tmp679;return Cyc_PP_egroup(((_tmp679="<",_tag_dyneither(_tmp679,sizeof(char),2))),((_tmp67A=">",_tag_dyneither(_tmp67A,sizeof(char),2))),((_tmp67B=",",_tag_dyneither(_tmp67B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 458
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp67E;const char*_tmp67D;const char*_tmp67C;return Cyc_PP_egroup(((_tmp67C="<",_tag_dyneither(_tmp67C,sizeof(char),2))),((_tmp67D=">",_tag_dyneither(_tmp67D,sizeof(char),2))),((_tmp67E=",",_tag_dyneither(_tmp67E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp681;const char*_tmp680;const char*_tmp67F;return Cyc_PP_group(((_tmp67F="(",_tag_dyneither(_tmp67F,sizeof(char),2))),((_tmp680=")",_tag_dyneither(_tmp680,sizeof(char),2))),((_tmp681=",",_tag_dyneither(_tmp681,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 472
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB9=att;_LL6C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpBA=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBA->tag != 1)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpBB=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBB->tag != 2)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpBC=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB9;if(_tmpBC->tag != 3)goto _LL72;}_LL71:
 return Cyc_PP_nil_doc();_LL72:;_LL73:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL6B:;}
# 481
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpBD=(void*)atts->hd;void*_tmpBE=_tmpBD;_LL75: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpBF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpBF->tag != 1)goto _LL77;}_LL76: {
const char*_tmp682;return Cyc_PP_text(((_tmp682=" _stdcall ",_tag_dyneither(_tmp682,sizeof(char),11))));}_LL77: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC0->tag != 2)goto _LL79;}_LL78: {
const char*_tmp683;return Cyc_PP_text(((_tmp683=" _cdecl ",_tag_dyneither(_tmp683,sizeof(char),9))));}_LL79: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC1->tag != 3)goto _LL7B;}_LL7A: {
const char*_tmp684;return Cyc_PP_text(((_tmp684=" _fastcall ",_tag_dyneither(_tmp684,sizeof(char),12))));}_LL7B:;_LL7C:
 goto _LL74;_LL74:;}
# 489
return Cyc_PP_nil_doc();}
# 492
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpC5=(void*)atts2->hd;void*_tmpC6=_tmpC5;_LL7E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpC7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC7->tag != 1)goto _LL80;}_LL7F:
 goto _LL81;_LL80: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC8->tag != 2)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpC6;if(_tmpC9->tag != 3)goto _LL84;}_LL83:
 goto _LL7D;_LL84:;_LL85:
 hasatt=1;goto _LL7D;_LL7D:;}}
# 501
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp68F;const char*_tmp68E;const char*_tmp68D;const char*_tmp68C;const char*_tmp68B;struct Cyc_PP_Doc*_tmp68A[3];return(_tmp68A[2]=
# 505
Cyc_PP_text(((_tmp68B=")",_tag_dyneither(_tmp68B,sizeof(char),2)))),((_tmp68A[1]=
# 504
Cyc_PP_group(((_tmp68E="",_tag_dyneither(_tmp68E,sizeof(char),1))),((_tmp68D="",_tag_dyneither(_tmp68D,sizeof(char),1))),((_tmp68C=" ",_tag_dyneither(_tmp68C,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp68A[0]=
# 503
Cyc_PP_text(((_tmp68F=" __declspec(",_tag_dyneither(_tmp68F,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp68A,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 512
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL86:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL87: {
const char*_tmp698;const char*_tmp697;const char*_tmp696;const char*_tmp695;struct Cyc_PP_Doc*_tmp694[2];return(_tmp694[1]=
Cyc_PP_group(((_tmp697="((",_tag_dyneither(_tmp697,sizeof(char),3))),((_tmp696="))",_tag_dyneither(_tmp696,sizeof(char),3))),((_tmp695=",",_tag_dyneither(_tmp695,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp694[0]=
# 516
Cyc_PP_text(((_tmp698=" __attribute__",_tag_dyneither(_tmp698,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp694,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 521
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpD5=(void*)tms->hd;void*_tmpD6=_tmpD5;_LL8A: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD6;if(_tmpD7->tag != 2)goto _LL8C;}_LL8B:
 return 1;_LL8C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD6;if(_tmpD8->tag != 5)goto _LL8E;}_LL8D:
# 526
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL90:
 return 0;default: _LL91:
 return Cyc_Absynpp_next_is_pointer(tms->tl);}_LL8E:;_LL8F:
# 530
 return 0;_LL89:;};}
# 534
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t);
# 537
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_question=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_question(){
if(!((unsigned int)Cyc_Absynpp_cache_question)){
if(Cyc_PP_tex_output){
const char*_tmp699;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp699="{?}",_tag_dyneither(_tmp699,sizeof(char),4))),1);}else{
const char*_tmp69A;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp69A="?",_tag_dyneither(_tmp69A,sizeof(char),2))));}}
# 544
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 546
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp69B;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp69B="{\\lb}",_tag_dyneither(_tmp69B,sizeof(char),6))),1);}else{
const char*_tmp69C;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp69C="{",_tag_dyneither(_tmp69C,sizeof(char),2))));}}
# 553
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 555
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp69D;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp69D="{\\rb}",_tag_dyneither(_tmp69D,sizeof(char),6))),1);}else{
const char*_tmp69E;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp69E="}",_tag_dyneither(_tmp69E,sizeof(char),2))));}}
# 562
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 564
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp69F;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp69F="\\$",_tag_dyneither(_tmp69F,sizeof(char),3))),1);}else{
const char*_tmp6A0;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp6A0="$",_tag_dyneither(_tmp6A0,sizeof(char),2))));}}
# 571
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 573
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp6A1[3];return(_tmp6A1[2]=Cyc_Absynpp_rb(),((_tmp6A1[1]=Cyc_PP_seq(sep,ss),((_tmp6A1[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp6A1,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpE2=(void*)tms->hd;void*_tmpE3=_tmpE2;struct Cyc_List_List*_tmpE8;_LL94: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpE4=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE4->tag != 0)goto _LL96;}_LL95:
{const char*_tmp6A4;void*_tmp6A3;(_tmp6A3=0,Cyc_fprintf(Cyc_stderr,((_tmp6A4="Carray_mod ",_tag_dyneither(_tmp6A4,sizeof(char),12))),_tag_dyneither(_tmp6A3,sizeof(void*),0)));}goto _LL93;_LL96: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpE5=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE5->tag != 1)goto _LL98;}_LL97:
{const char*_tmp6A7;void*_tmp6A6;(_tmp6A6=0,Cyc_fprintf(Cyc_stderr,((_tmp6A7="ConstArray_mod ",_tag_dyneither(_tmp6A7,sizeof(char),16))),_tag_dyneither(_tmp6A6,sizeof(void*),0)));}goto _LL93;_LL98: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE6=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE6->tag != 3)goto _LL9A;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpE7=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpE6->f1);if(_tmpE7->tag != 1)goto _LL9A;else{_tmpE8=_tmpE7->f1;}}}_LL99:
# 584
{const char*_tmp6AA;void*_tmp6A9;(_tmp6A9=0,Cyc_fprintf(Cyc_stderr,((_tmp6AA="Function_mod(",_tag_dyneither(_tmp6AA,sizeof(char),14))),_tag_dyneither(_tmp6A9,sizeof(void*),0)));}
for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct _dyneither_ptr*_tmpF3=(*((struct _tuple8*)_tmpE8->hd)).f1;
if(_tmpF3 == 0){const char*_tmp6AD;void*_tmp6AC;(_tmp6AC=0,Cyc_fprintf(Cyc_stderr,((_tmp6AD="?",_tag_dyneither(_tmp6AD,sizeof(char),2))),_tag_dyneither(_tmp6AC,sizeof(void*),0)));}else{
void*_tmp6AE;(_tmp6AE=0,Cyc_fprintf(Cyc_stderr,*_tmpF3,_tag_dyneither(_tmp6AE,sizeof(void*),0)));}
if(_tmpE8->tl != 0){const char*_tmp6B1;void*_tmp6B0;(_tmp6B0=0,Cyc_fprintf(Cyc_stderr,((_tmp6B1=",",_tag_dyneither(_tmp6B1,sizeof(char),2))),_tag_dyneither(_tmp6B0,sizeof(void*),0)));}}
# 591
{const char*_tmp6B4;void*_tmp6B3;(_tmp6B3=0,Cyc_fprintf(Cyc_stderr,((_tmp6B4=") ",_tag_dyneither(_tmp6B4,sizeof(char),3))),_tag_dyneither(_tmp6B3,sizeof(void*),0)));}
goto _LL93;_LL9A: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpE9=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpE9->tag != 3)goto _LL9C;}_LL9B:
# 594
{const char*_tmp6B7;void*_tmp6B6;(_tmp6B6=0,Cyc_fprintf(Cyc_stderr,((_tmp6B7="Function_mod()",_tag_dyneither(_tmp6B7,sizeof(char),15))),_tag_dyneither(_tmp6B6,sizeof(void*),0)));}goto _LL93;_LL9C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpEA=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEA->tag != 5)goto _LL9E;}_LL9D:
{const char*_tmp6BA;void*_tmp6B9;(_tmp6B9=0,Cyc_fprintf(Cyc_stderr,((_tmp6BA="Attributes_mod ",_tag_dyneither(_tmp6BA,sizeof(char),16))),_tag_dyneither(_tmp6B9,sizeof(void*),0)));}goto _LL93;_LL9E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpEB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEB->tag != 4)goto _LLA0;}_LL9F:
{const char*_tmp6BD;void*_tmp6BC;(_tmp6BC=0,Cyc_fprintf(Cyc_stderr,((_tmp6BD="TypeParams_mod ",_tag_dyneither(_tmp6BD,sizeof(char),16))),_tag_dyneither(_tmp6BC,sizeof(void*),0)));}goto _LL93;_LLA0: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpEC=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE3;if(_tmpEC->tag != 2)goto _LL93;}_LLA1:
{const char*_tmp6C0;void*_tmp6BF;(_tmp6BF=0,Cyc_fprintf(Cyc_stderr,((_tmp6C0="Pointer_mod ",_tag_dyneither(_tmp6C0,sizeof(char),13))),_tag_dyneither(_tmp6BF,sizeof(void*),0)));}goto _LL93;_LL93:;}{
# 599
const char*_tmp6C3;void*_tmp6C2;(_tmp6C2=0,Cyc_fprintf(Cyc_stderr,((_tmp6C3="\n",_tag_dyneither(_tmp6C3,sizeof(char),2))),_tag_dyneither(_tmp6C2,sizeof(void*),0)));};}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6C8;const char*_tmp6C7;struct Cyc_PP_Doc*_tmp6C6[3];struct Cyc_PP_Doc*p_rest=(_tmp6C6[2]=Cyc_PP_text(((_tmp6C7=")",_tag_dyneither(_tmp6C7,sizeof(char),2)))),((_tmp6C6[1]=rest,((_tmp6C6[0]=Cyc_PP_text(((_tmp6C8="(",_tag_dyneither(_tmp6C8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6C6,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmp105=(void*)tms->hd;void*_tmp106=_tmp105;union Cyc_Absyn_Constraint*_tmp108;struct Cyc_Absyn_Exp*_tmp10A;union Cyc_Absyn_Constraint*_tmp10B;void*_tmp10D;struct Cyc_List_List*_tmp10F;struct Cyc_List_List*_tmp111;unsigned int _tmp112;int _tmp113;void*_tmp115;union Cyc_Absyn_Constraint*_tmp116;union Cyc_Absyn_Constraint*_tmp117;union Cyc_Absyn_Constraint*_tmp118;struct Cyc_Absyn_Tqual _tmp119;_LLA3: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp107=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp107->tag != 0)goto _LLA5;else{_tmp108=_tmp107->f1;}}_LLA4:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6CD;const char*_tmp6CC;struct Cyc_PP_Doc*_tmp6CB[2];return(_tmp6CB[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp108)?
Cyc_PP_text(((_tmp6CC="[]@zeroterm",_tag_dyneither(_tmp6CC,sizeof(char),12)))): Cyc_PP_text(((_tmp6CD="[]",_tag_dyneither(_tmp6CD,sizeof(char),3)))),((_tmp6CB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6CB,sizeof(struct Cyc_PP_Doc*),2)))));};_LLA5: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp109=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp109->tag != 1)goto _LLA7;else{_tmp10A=_tmp109->f1;_tmp10B=_tmp109->f2;}}_LLA6:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6D4;const char*_tmp6D3;const char*_tmp6D2;struct Cyc_PP_Doc*_tmp6D1[4];return(_tmp6D1[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10B)?Cyc_PP_text(((_tmp6D2="]@zeroterm",_tag_dyneither(_tmp6D2,sizeof(char),11)))): Cyc_PP_text(((_tmp6D3="]",_tag_dyneither(_tmp6D3,sizeof(char),2)))),((_tmp6D1[2]=
# 615
Cyc_Absynpp_exp2doc(_tmp10A),((_tmp6D1[1]=Cyc_PP_text(((_tmp6D4="[",_tag_dyneither(_tmp6D4,sizeof(char),2)))),((_tmp6D1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D1,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA7: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp10C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10C->tag != 3)goto _LLA9;else{_tmp10D=(void*)_tmp10C->f1;}}_LLA8:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp121=_tmp10D;struct Cyc_List_List*_tmp123;int _tmp124;struct Cyc_Absyn_VarargInfo*_tmp125;void*_tmp126;struct Cyc_List_List*_tmp127;struct Cyc_List_List*_tmp129;unsigned int _tmp12A;_LLB0: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp122=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp122->tag != 1)goto _LLB2;else{_tmp123=_tmp122->f1;_tmp124=_tmp122->f2;_tmp125=_tmp122->f3;_tmp126=(void*)_tmp122->f4;_tmp127=_tmp122->f5;}}_LLB1: {
# 621
struct Cyc_PP_Doc*_tmp6D5[2];return(_tmp6D5[1]=Cyc_Absynpp_funargs2doc(_tmp123,_tmp124,_tmp125,_tmp126,_tmp127),((_tmp6D5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D5,sizeof(struct Cyc_PP_Doc*),2)))));}_LLB2: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp128=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp121;if(_tmp128->tag != 0)goto _LLAF;else{_tmp129=_tmp128->f1;_tmp12A=_tmp128->f2;}}_LLB3: {
# 623
const char*_tmp6DC;const char*_tmp6DB;const char*_tmp6DA;struct Cyc_PP_Doc*_tmp6D9[2];return(_tmp6D9[1]=Cyc_PP_group(((_tmp6DC="(",_tag_dyneither(_tmp6DC,sizeof(char),2))),((_tmp6DB=")",_tag_dyneither(_tmp6DB,sizeof(char),2))),((_tmp6DA=",",_tag_dyneither(_tmp6DA,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp129)),((_tmp6D9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6D9,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAF:;};_LLA9: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp10E=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp10E->tag != 5)goto _LLAB;else{_tmp10F=_tmp10E->f2;}}_LLAA:
# 626
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLB4:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6DD[2];return(_tmp6DD[1]=Cyc_Absynpp_atts2doc(_tmp10F),((_tmp6DD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLB5:
# 631
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6DE[2];return(_tmp6DE[1]=rest,((_tmp6DE[0]=Cyc_Absynpp_callconv2doc(_tmp10F),Cyc_PP_cat(_tag_dyneither(_tmp6DE,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLAB: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp110=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp110->tag != 4)goto _LLAD;else{_tmp111=_tmp110->f1;_tmp112=_tmp110->f2;_tmp113=_tmp110->f3;}}_LLAC:
# 636
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp113){
struct Cyc_PP_Doc*_tmp6DF[2];return(_tmp6DF[1]=Cyc_Absynpp_ktvars2doc(_tmp111),((_tmp6DF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6DF,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 640
struct Cyc_PP_Doc*_tmp6E0[2];return(_tmp6E0[1]=Cyc_Absynpp_tvars2doc(_tmp111),((_tmp6E0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6E0,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAD: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp114=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp106;if(_tmp114->tag != 2)goto _LLA2;else{_tmp115=(_tmp114->f1).rgn;_tmp116=(_tmp114->f1).nullable;_tmp117=(_tmp114->f1).bounds;_tmp118=(_tmp114->f1).zero_term;_tmp119=_tmp114->f2;}}_LLAE: {
# 644
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp119);
{void*_tmp134=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp117);void*_tmp135=_tmp134;struct Cyc_Absyn_Exp*_tmp138;_LLB8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp136=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp135;if(_tmp136->tag != 0)goto _LLBA;}_LLB9:
 ptr=Cyc_Absynpp_question();goto _LLB7;_LLBA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp137=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp135;if(_tmp137->tag != 1)goto _LLB7;else{_tmp138=_tmp137->f1;}}_LLBB:
# 652
{const char*_tmp6E2;const char*_tmp6E1;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp116)?(_tmp6E2="*",_tag_dyneither(_tmp6E2,sizeof(char),2)):((_tmp6E1="@",_tag_dyneither(_tmp6E1,sizeof(char),2))));}{
struct _tuple11 _tmp13B=Cyc_Evexp_eval_const_uint_exp(_tmp138);unsigned int _tmp13D;int _tmp13E;struct _tuple11 _tmp13C=_tmp13B;_tmp13D=_tmp13C.f1;_tmp13E=_tmp13C.f2;
if(!_tmp13E  || _tmp13D != 1){
struct Cyc_PP_Doc*_tmp6E3[4];ptr=((_tmp6E3[3]=Cyc_Absynpp_rb(),((_tmp6E3[2]=Cyc_Absynpp_exp2doc(_tmp138),((_tmp6E3[1]=Cyc_Absynpp_lb(),((_tmp6E3[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6E3,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB7;};_LLB7:;}
# 658
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp6E4;ztd=Cyc_PP_text(((_tmp6E4="@zeroterm",_tag_dyneither(_tmp6E4,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp118)){
const char*_tmp6E5;ztd=Cyc_PP_text(((_tmp6E5="@nozeroterm",_tag_dyneither(_tmp6E5,sizeof(char),12))));}}}
# 664
{void*_tmp142=Cyc_Tcutil_compress(_tmp115);void*_tmp143=_tmp142;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp144=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp143;if(_tmp144->tag != 20)goto _LLBF;}_LLBE:
 goto _LLBC;_LLBF:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp143;if(_tmp145->tag != 1)goto _LLC1;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLC1;_LLC0:
 goto _LLBC;_LLC1:;_LLC2:
 rgd=Cyc_Absynpp_rgn2doc(_tmp115);_LLBC:;}{
# 669
const char*_tmp6E6;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp6E6=" ",_tag_dyneither(_tmp6E6,sizeof(char),2)))): mt;
const char*_tmp6E7;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp6E7=" ",_tag_dyneither(_tmp6E7,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp6E8[7];return(_tmp6E8[6]=rest,((_tmp6E8[5]=spacer2,((_tmp6E8[4]=tqd,((_tmp6E8[3]=spacer1,((_tmp6E8[2]=rgd,((_tmp6E8[1]=ztd,((_tmp6E8[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6E8,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}_LLA2:;};}
# 675
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp14C=Cyc_Tcutil_compress(t);void*_tmp14D=_tmp14C;_LLC4: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp14E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp14D;if(_tmp14E->tag != 20)goto _LLC6;}_LLC5: {
const char*_tmp6E9;return Cyc_PP_text(((_tmp6E9="`H",_tag_dyneither(_tmp6E9,sizeof(char),3))));}_LLC6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp14F=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp14D;if(_tmp14F->tag != 21)goto _LLC8;}_LLC7: {
const char*_tmp6EA;return Cyc_PP_text(((_tmp6EA="`U",_tag_dyneither(_tmp6EA,sizeof(char),3))));}_LLC8: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp150=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp14D;if(_tmp150->tag != 22)goto _LLCA;}_LLC9: {
const char*_tmp6EB;return Cyc_PP_text(((_tmp6EB="`RC",_tag_dyneither(_tmp6EB,sizeof(char),4))));}_LLCA:;_LLCB:
 return Cyc_Absynpp_ntyp2doc(t);_LLC3:;}
# 685
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 689
void*_tmp154=Cyc_Tcutil_compress(t);void*_tmp155=_tmp154;void*_tmp157;struct Cyc_List_List*_tmp159;_LLCD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp156=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp155;if(_tmp156->tag != 23)goto _LLCF;else{_tmp157=(void*)_tmp156->f1;}}_LLCE:
{struct Cyc_List_List*_tmp6EC;*rgions=((_tmp6EC=_cycalloc(sizeof(*_tmp6EC)),((_tmp6EC->hd=Cyc_Absynpp_rgn2doc(_tmp157),((_tmp6EC->tl=*rgions,_tmp6EC))))));}goto _LLCC;_LLCF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp155;if(_tmp158->tag != 24)goto _LLD1;else{_tmp159=_tmp158->f1;}}_LLD0:
# 692
 for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp159->hd);}
# 695
goto _LLCC;_LLD1:;_LLD2:
{struct Cyc_List_List*_tmp6ED;*effects=((_tmp6ED=_cycalloc(sizeof(*_tmp6ED)),((_tmp6ED->hd=Cyc_Absynpp_typ2doc(t),((_tmp6ED->tl=*effects,_tmp6ED))))));}goto _LLCC;_LLCC:;}
# 700
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6F0;const char*_tmp6EF;const char*_tmp6EE;return Cyc_PP_group(((_tmp6EE="",_tag_dyneither(_tmp6EE,sizeof(char),1))),((_tmp6EF="",_tag_dyneither(_tmp6EF,sizeof(char),1))),((_tmp6F0="+",_tag_dyneither(_tmp6F0,sizeof(char),2))),effects);}else{
# 708
const char*_tmp6F1;struct Cyc_PP_Doc*_tmp15F=Cyc_Absynpp_group_braces(((_tmp6F1=",",_tag_dyneither(_tmp6F1,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6F5;const char*_tmp6F4;const char*_tmp6F3;const char*_tmp6F2;return Cyc_PP_group(((_tmp6F2="",_tag_dyneither(_tmp6F2,sizeof(char),1))),((_tmp6F3="",_tag_dyneither(_tmp6F3,sizeof(char),1))),((_tmp6F4="+",_tag_dyneither(_tmp6F4,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6F5=_cycalloc(sizeof(*_tmp6F5)),((_tmp6F5->hd=_tmp15F,((_tmp6F5->tl=0,_tmp6F5))))))));}}
# 713
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLD3: {
const char*_tmp6F6;return Cyc_PP_text(((_tmp6F6="struct ",_tag_dyneither(_tmp6F6,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLD4: {
const char*_tmp6F7;return Cyc_PP_text(((_tmp6F7="union ",_tag_dyneither(_tmp6F7,sizeof(char),7))));}}}
# 721
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp167=t;struct Cyc_Core_Opt*_tmp16D;void*_tmp16E;int _tmp16F;struct Cyc_Core_Opt*_tmp170;struct Cyc_Absyn_Tvar*_tmp172;union Cyc_Absyn_DatatypeInfoU _tmp174;struct Cyc_List_List*_tmp175;union Cyc_Absyn_DatatypeFieldInfoU _tmp177;struct Cyc_List_List*_tmp178;enum Cyc_Absyn_Sign _tmp17A;enum Cyc_Absyn_Size_of _tmp17B;int _tmp17D;struct Cyc_List_List*_tmp17F;union Cyc_Absyn_AggrInfoU _tmp181;struct Cyc_List_List*_tmp182;enum Cyc_Absyn_AggrKind _tmp184;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp187;struct _tuple0*_tmp189;struct Cyc_Absyn_Exp*_tmp18B;struct _tuple0*_tmp18D;struct Cyc_List_List*_tmp18E;struct Cyc_Absyn_Typedefdecl*_tmp18F;void*_tmp191;void*_tmp193;void*_tmp194;void*_tmp196;void*_tmp19B;struct Cyc_Absyn_Aggrdecl*_tmp1A0;struct Cyc_Absyn_Enumdecl*_tmp1A3;struct Cyc_Absyn_Datatypedecl*_tmp1A6;_LLD7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp168=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp167;if(_tmp168->tag != 8)goto _LLD9;}_LLD8: {
# 725
const char*_tmp6F8;return Cyc_PP_text(((_tmp6F8="[[[array]]]",_tag_dyneither(_tmp6F8,sizeof(char),12))));}_LLD9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp167;if(_tmp169->tag != 9)goto _LLDB;}_LLDA:
 return Cyc_PP_nil_doc();_LLDB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp16A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp167;if(_tmp16A->tag != 5)goto _LLDD;}_LLDC:
 return Cyc_PP_nil_doc();_LLDD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp167;if(_tmp16B->tag != 0)goto _LLDF;}_LLDE:
# 729
{const char*_tmp6F9;s=Cyc_PP_text(((_tmp6F9="void",_tag_dyneither(_tmp6F9,sizeof(char),5))));}goto _LLD6;_LLDF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp167;if(_tmp16C->tag != 1)goto _LLE1;else{_tmp16D=_tmp16C->f1;_tmp16E=(void*)_tmp16C->f2;_tmp16F=_tmp16C->f3;_tmp170=_tmp16C->f4;}}_LLE0:
# 731
 if(_tmp16E != 0)
# 733
return Cyc_Absynpp_ntyp2doc(_tmp16E);else{
# 735
const char*_tmp6FA;struct _dyneither_ptr kindstring=_tmp16D == 0?(_tmp6FA="K",_tag_dyneither(_tmp6FA,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp16D->v);
const char*_tmp6FF;void*_tmp6FE[2];struct Cyc_String_pa_PrintArg_struct _tmp6FD;struct Cyc_Int_pa_PrintArg_struct _tmp6FC;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp6FC.tag=1,((_tmp6FC.f1=(unsigned long)_tmp16F,((_tmp6FD.tag=0,((_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp6FE[0]=& _tmp6FD,((_tmp6FE[1]=& _tmp6FC,Cyc_aprintf(((_tmp6FF="`E%s%d",_tag_dyneither(_tmp6FF,sizeof(char),7))),_tag_dyneither(_tmp6FE,sizeof(void*),2)))))))))))))));}
# 752 "absynpp.cyc"
goto _LLD6;_LLE1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp171=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp167;if(_tmp171->tag != 2)goto _LLE3;else{_tmp172=_tmp171->f1;}}_LLE2:
# 754
 s=Cyc_Absynpp_tvar2doc(_tmp172);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp702;struct Cyc_PP_Doc*_tmp701[3];s=((_tmp701[2]=Cyc_Absynpp_kindbound2doc(_tmp172->kind),((_tmp701[1]=Cyc_PP_text(((_tmp702="::",_tag_dyneither(_tmp702,sizeof(char),3)))),((_tmp701[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 758
goto _LLD6;_LLE3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp167;if(_tmp173->tag != 3)goto _LLE5;else{_tmp174=(_tmp173->f1).datatype_info;_tmp175=(_tmp173->f1).targs;}}_LLE4:
# 760
{union Cyc_Absyn_DatatypeInfoU _tmp1B0=_tmp174;struct _tuple0*_tmp1B1;int _tmp1B2;struct _tuple0*_tmp1B3;int _tmp1B4;_LL112: if((_tmp1B0.UnknownDatatype).tag != 1)goto _LL114;_tmp1B1=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B0.UnknownDatatype).val).name;_tmp1B2=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1B0.UnknownDatatype).val).is_extensible;_LL113:
# 762
 _tmp1B3=_tmp1B1;_tmp1B4=_tmp1B2;goto _LL115;_LL114: if((_tmp1B0.KnownDatatype).tag != 2)goto _LL111;_tmp1B3=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B0.KnownDatatype).val))->name;_tmp1B4=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1B0.KnownDatatype).val))->is_extensible;_LL115: {
# 764
const char*_tmp703;struct Cyc_PP_Doc*_tmp1B5=Cyc_PP_text(((_tmp703="datatype ",_tag_dyneither(_tmp703,sizeof(char),10))));
const char*_tmp704;struct Cyc_PP_Doc*_tmp1B6=_tmp1B4?Cyc_PP_text(((_tmp704="@extensible ",_tag_dyneither(_tmp704,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp705[4];s=((_tmp705[3]=Cyc_Absynpp_tps2doc(_tmp175),((_tmp705[2]=Cyc_Absynpp_qvar2doc(_tmp1B3),((_tmp705[1]=_tmp1B5,((_tmp705[0]=_tmp1B6,Cyc_PP_cat(_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 769
goto _LLD6;_LLE5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp167;if(_tmp176->tag != 4)goto _LLE7;else{_tmp177=(_tmp176->f1).field_info;_tmp178=(_tmp176->f1).targs;}}_LLE6:
# 771
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1BA=_tmp177;struct _tuple0*_tmp1BB;struct _tuple0*_tmp1BC;int _tmp1BD;struct _tuple0*_tmp1BE;int _tmp1BF;struct _tuple0*_tmp1C0;_LL117: if((_tmp1BA.UnknownDatatypefield).tag != 1)goto _LL119;_tmp1BB=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BA.UnknownDatatypefield).val).datatype_name;_tmp1BC=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BA.UnknownDatatypefield).val).field_name;_tmp1BD=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1BA.UnknownDatatypefield).val).is_extensible;_LL118:
# 773
 _tmp1BE=_tmp1BB;_tmp1BF=_tmp1BD;_tmp1C0=_tmp1BC;goto _LL11A;_LL119: if((_tmp1BA.KnownDatatypefield).tag != 2)goto _LL116;_tmp1BE=(((struct _tuple1)(_tmp1BA.KnownDatatypefield).val).f1)->name;_tmp1BF=(((struct _tuple1)(_tmp1BA.KnownDatatypefield).val).f1)->is_extensible;_tmp1C0=(((struct _tuple1)(_tmp1BA.KnownDatatypefield).val).f2)->name;_LL11A: {
# 776
const char*_tmp707;const char*_tmp706;struct Cyc_PP_Doc*_tmp1C1=Cyc_PP_text(_tmp1BF?(_tmp707="@extensible datatype ",_tag_dyneither(_tmp707,sizeof(char),22)):((_tmp706="datatype ",_tag_dyneither(_tmp706,sizeof(char),10))));
{const char*_tmp70A;struct Cyc_PP_Doc*_tmp709[4];s=((_tmp709[3]=Cyc_Absynpp_qvar2doc(_tmp1C0),((_tmp709[2]=Cyc_PP_text(((_tmp70A=".",_tag_dyneither(_tmp70A,sizeof(char),2)))),((_tmp709[1]=Cyc_Absynpp_qvar2doc(_tmp1BE),((_tmp709[0]=_tmp1C1,Cyc_PP_cat(_tag_dyneither(_tmp709,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL116;}_LL116:;}
# 780
goto _LLD6;_LLE7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp167;if(_tmp179->tag != 6)goto _LLE9;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;}}_LLE8: {
# 782
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp17A){case Cyc_Absyn_None: _LL11B:
 goto _LL11C;case Cyc_Absyn_Signed: _LL11C:
{const char*_tmp70B;sns=((_tmp70B="",_tag_dyneither(_tmp70B,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL11D:
{const char*_tmp70C;sns=((_tmp70C="unsigned ",_tag_dyneither(_tmp70C,sizeof(char),10)));}break;}
# 789
switch(_tmp17B){case Cyc_Absyn_Char_sz: _LL11F:
# 791
 switch(_tmp17A){case Cyc_Absyn_None: _LL121:
{const char*_tmp70D;sns=((_tmp70D="",_tag_dyneither(_tmp70D,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL122:
{const char*_tmp70E;sns=((_tmp70E="signed ",_tag_dyneither(_tmp70E,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL123:
{const char*_tmp70F;sns=((_tmp70F="unsigned ",_tag_dyneither(_tmp70F,sizeof(char),10)));}break;}
# 796
{const char*_tmp710;ts=((_tmp710="char",_tag_dyneither(_tmp710,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL120:
{const char*_tmp711;ts=((_tmp711="short",_tag_dyneither(_tmp711,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL125:
{const char*_tmp712;ts=((_tmp712="int",_tag_dyneither(_tmp712,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL126:
{const char*_tmp713;ts=((_tmp713="long",_tag_dyneither(_tmp713,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL127:
# 802
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL129:
{const char*_tmp714;ts=((_tmp714="long long",_tag_dyneither(_tmp714,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL12A:
{const char*_tmp715;ts=((_tmp715="__int64",_tag_dyneither(_tmp715,sizeof(char),8)));}break;}
# 806
break;}
# 808
{const char*_tmp71A;void*_tmp719[2];struct Cyc_String_pa_PrintArg_struct _tmp718;struct Cyc_String_pa_PrintArg_struct _tmp717;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp717.tag=0,((_tmp717.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp718.tag=0,((_tmp718.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp719[0]=& _tmp718,((_tmp719[1]=& _tmp717,Cyc_aprintf(((_tmp71A="%s%s",_tag_dyneither(_tmp71A,sizeof(char),5))),_tag_dyneither(_tmp719,sizeof(void*),2)))))))))))))));}
goto _LLD6;}_LLE9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp167;if(_tmp17C->tag != 7)goto _LLEB;else{_tmp17D=_tmp17C->f1;}}_LLEA:
# 811
 if(_tmp17D == 0){
const char*_tmp71B;s=Cyc_PP_text(((_tmp71B="float",_tag_dyneither(_tmp71B,sizeof(char),6))));}else{
if(_tmp17D == 1){
const char*_tmp71C;s=Cyc_PP_text(((_tmp71C="double",_tag_dyneither(_tmp71C,sizeof(char),7))));}else{
# 816
const char*_tmp71D;s=Cyc_PP_text(((_tmp71D="long double",_tag_dyneither(_tmp71D,sizeof(char),12))));}}
goto _LLD6;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp17E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp167;if(_tmp17E->tag != 10)goto _LLED;else{_tmp17F=_tmp17E->f1;}}_LLEC:
# 819
{struct Cyc_PP_Doc*_tmp71E[2];s=((_tmp71E[1]=Cyc_Absynpp_args2doc(_tmp17F),((_tmp71E[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp71E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp167;if(_tmp180->tag != 11)goto _LLEF;else{_tmp181=(_tmp180->f1).aggr_info;_tmp182=(_tmp180->f1).targs;}}_LLEE: {
# 822
struct _tuple10 _tmp1D9=Cyc_Absyn_aggr_kinded_name(_tmp181);enum Cyc_Absyn_AggrKind _tmp1DB;struct _tuple0*_tmp1DC;struct _tuple10 _tmp1DA=_tmp1D9;_tmp1DB=_tmp1DA.f1;_tmp1DC=_tmp1DA.f2;
{struct Cyc_PP_Doc*_tmp71F[3];s=((_tmp71F[2]=Cyc_Absynpp_tps2doc(_tmp182),((_tmp71F[1]=Cyc_Absynpp_qvar2doc(_tmp1DC),((_tmp71F[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1DB),Cyc_PP_cat(_tag_dyneither(_tmp71F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD6;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp183=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp167;if(_tmp183->tag != 12)goto _LLF1;else{_tmp184=_tmp183->f1;_tmp185=_tmp183->f2;}}_LLF0:
# 826
{struct Cyc_PP_Doc*_tmp720[4];s=((_tmp720[3]=
# 828
Cyc_Absynpp_rb(),((_tmp720[2]=
# 827
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp185)),((_tmp720[1]=
# 826
Cyc_Absynpp_lb(),((_tmp720[0]=Cyc_Absynpp_aggr_kind2doc(_tmp184),Cyc_PP_cat(_tag_dyneither(_tmp720,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 829
goto _LLD6;_LLF1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp167;if(_tmp186->tag != 14)goto _LLF3;else{_tmp187=_tmp186->f1;}}_LLF2:
# 831
{const char*_tmp723;struct Cyc_PP_Doc*_tmp722[4];s=((_tmp722[3]=Cyc_Absynpp_rb(),((_tmp722[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp187)),((_tmp722[1]=Cyc_Absynpp_lb(),((_tmp722[0]=Cyc_PP_text(((_tmp723="enum ",_tag_dyneither(_tmp723,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp722,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD6;_LLF3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp167;if(_tmp188->tag != 13)goto _LLF5;else{_tmp189=_tmp188->f1;}}_LLF4:
# 833
{const char*_tmp726;struct Cyc_PP_Doc*_tmp725[2];s=((_tmp725[1]=Cyc_Absynpp_qvar2doc(_tmp189),((_tmp725[0]=Cyc_PP_text(((_tmp726="enum ",_tag_dyneither(_tmp726,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp725,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD6;_LLF5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp167;if(_tmp18A->tag != 18)goto _LLF7;else{_tmp18B=_tmp18A->f1;}}_LLF6:
# 835
{const char*_tmp72B;const char*_tmp72A;struct Cyc_PP_Doc*_tmp729[3];s=((_tmp729[2]=Cyc_PP_text(((_tmp72A=")",_tag_dyneither(_tmp72A,sizeof(char),2)))),((_tmp729[1]=Cyc_Absynpp_exp2doc(_tmp18B),((_tmp729[0]=Cyc_PP_text(((_tmp72B="valueof_t(",_tag_dyneither(_tmp72B,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp167;if(_tmp18C->tag != 17)goto _LLF9;else{_tmp18D=_tmp18C->f1;_tmp18E=_tmp18C->f2;_tmp18F=_tmp18C->f3;}}_LLF8:
# 842
{struct Cyc_PP_Doc*_tmp72C[2];s=((_tmp72C[1]=Cyc_Absynpp_tps2doc(_tmp18E),((_tmp72C[0]=Cyc_Absynpp_qvar2doc(_tmp18D),Cyc_PP_cat(_tag_dyneither(_tmp72C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLF9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp190=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp167;if(_tmp190->tag != 15)goto _LLFB;else{_tmp191=(void*)_tmp190->f1;}}_LLFA:
# 845
{const char*_tmp731;const char*_tmp730;struct Cyc_PP_Doc*_tmp72F[3];s=((_tmp72F[2]=Cyc_PP_text(((_tmp730=">",_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72F[1]=Cyc_Absynpp_rgn2doc(_tmp191),((_tmp72F[0]=Cyc_PP_text(((_tmp731="region_t<",_tag_dyneither(_tmp731,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp72F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp192=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp167;if(_tmp192->tag != 16)goto _LLFD;else{_tmp193=(void*)_tmp192->f1;_tmp194=(void*)_tmp192->f2;}}_LLFC:
# 847
{const char*_tmp738;const char*_tmp737;const char*_tmp736;struct Cyc_PP_Doc*_tmp735[5];s=((_tmp735[4]=
Cyc_PP_text(((_tmp736=">",_tag_dyneither(_tmp736,sizeof(char),2)))),((_tmp735[3]=
# 847
Cyc_Absynpp_rgn2doc(_tmp194),((_tmp735[2]=Cyc_PP_text(((_tmp737=",",_tag_dyneither(_tmp737,sizeof(char),2)))),((_tmp735[1]=Cyc_Absynpp_rgn2doc(_tmp193),((_tmp735[0]=Cyc_PP_text(((_tmp738="dynregion_t<",_tag_dyneither(_tmp738,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp735,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD6;_LLFD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp195=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp167;if(_tmp195->tag != 19)goto _LLFF;else{_tmp196=(void*)_tmp195->f1;}}_LLFE:
# 850
{const char*_tmp73D;const char*_tmp73C;struct Cyc_PP_Doc*_tmp73B[3];s=((_tmp73B[2]=Cyc_PP_text(((_tmp73C=">",_tag_dyneither(_tmp73C,sizeof(char),2)))),((_tmp73B[1]=Cyc_Absynpp_typ2doc(_tmp196),((_tmp73B[0]=Cyc_PP_text(((_tmp73D="tag_t<",_tag_dyneither(_tmp73D,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp73B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp197=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp167;if(_tmp197->tag != 21)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp198=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp167;if(_tmp198->tag != 20)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp199=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp167;if(_tmp199->tag != 22)goto _LL105;}_LL104:
# 854
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD6;_LL105: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp19A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp167;if(_tmp19A->tag != 25)goto _LL107;else{_tmp19B=(void*)_tmp19A->f1;}}_LL106:
# 856
{const char*_tmp742;const char*_tmp741;struct Cyc_PP_Doc*_tmp740[3];s=((_tmp740[2]=Cyc_PP_text(((_tmp741=")",_tag_dyneither(_tmp741,sizeof(char),2)))),((_tmp740[1]=Cyc_Absynpp_typ2doc(_tmp19B),((_tmp740[0]=Cyc_PP_text(((_tmp742="regions(",_tag_dyneither(_tmp742,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp740,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL107: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp19C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp167;if(_tmp19C->tag != 23)goto _LL109;}_LL108:
 goto _LL10A;_LL109: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp19D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp167;if(_tmp19D->tag != 24)goto _LL10B;}_LL10A:
# 859
 s=Cyc_Absynpp_eff2doc(t);goto _LLD6;_LL10B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp19E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp167;if(_tmp19E->tag != 26)goto _LL10D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp19F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp19E->f1)->r;if(_tmp19F->tag != 0)goto _LL10D;else{_tmp1A0=_tmp19F->f1;}}}_LL10C:
# 861
 s=Cyc_Absynpp_aggrdecl2doc(_tmp1A0);goto _LLD6;_LL10D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A1=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp167;if(_tmp1A1->tag != 26)goto _LL10F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp1A2=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp1A1->f1)->r;if(_tmp1A2->tag != 1)goto _LL10F;else{_tmp1A3=_tmp1A2->f1;}}}_LL10E:
# 863
 s=Cyc_Absynpp_enumdecl2doc(_tmp1A3);goto _LLD6;_LL10F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp1A4=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp167;if(_tmp1A4->tag != 26)goto _LLD6;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp1A5=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp1A4->f1)->r;if(_tmp1A5->tag != 2)goto _LLD6;else{_tmp1A6=_tmp1A5->f1;}}}_LL110:
# 865
 s=Cyc_Absynpp_datatypedecl2doc(_tmp1A6);goto _LLD6;_LLD6:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1F5;void*_tmp1F6;struct _tuple16*_tmp1F4=cmp;_tmp1F5=_tmp1F4->f1;_tmp1F6=_tmp1F4->f2;{
const char*_tmp745;struct Cyc_PP_Doc*_tmp744[3];return(_tmp744[2]=Cyc_Absynpp_rgn2doc(_tmp1F6),((_tmp744[1]=Cyc_PP_text(((_tmp745=" > ",_tag_dyneither(_tmp745,sizeof(char),4)))),((_tmp744[0]=Cyc_Absynpp_rgn2doc(_tmp1F5),Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp748;const char*_tmp747;const char*_tmp746;return Cyc_PP_group(((_tmp746="",_tag_dyneither(_tmp746,sizeof(char),1))),((_tmp747="",_tag_dyneither(_tmp747,sizeof(char),1))),((_tmp748=",",_tag_dyneither(_tmp748,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1FC=(*t).f1;
struct Cyc_Core_Opt*_tmp749;struct Cyc_Core_Opt*dopt=_tmp1FC == 0?0:((_tmp749=_cycalloc(sizeof(*_tmp749)),((_tmp749->v=Cyc_PP_text(*_tmp1FC),_tmp749))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 892
struct Cyc_List_List*_tmp1FE=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp74C;struct Cyc_List_List*_tmp74B;_tmp1FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1FE,((_tmp74B=_cycalloc(sizeof(*_tmp74B)),((_tmp74B->hd=Cyc_PP_text(((_tmp74C="...",_tag_dyneither(_tmp74C,sizeof(char),4)))),((_tmp74B->tl=0,_tmp74B)))))));}else{
if(cyc_varargs != 0){
const char*_tmp755;const char*_tmp754;const char*_tmp753;struct _tuple8*_tmp752;struct Cyc_PP_Doc*_tmp751[3];struct Cyc_PP_Doc*_tmp201=(_tmp751[2]=
# 899
Cyc_Absynpp_funarg2doc(((_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752->f1=cyc_varargs->name,((_tmp752->f2=cyc_varargs->tq,((_tmp752->f3=cyc_varargs->type,_tmp752))))))))),((_tmp751[1]=
# 898
cyc_varargs->inject?Cyc_PP_text(((_tmp753=" inject ",_tag_dyneither(_tmp753,sizeof(char),9)))): Cyc_PP_text(((_tmp754=" ",_tag_dyneither(_tmp754,sizeof(char),2)))),((_tmp751[0]=
# 897
Cyc_PP_text(((_tmp755="...",_tag_dyneither(_tmp755,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp751,sizeof(struct Cyc_PP_Doc*),3)))))));
# 901
struct Cyc_List_List*_tmp756;_tmp1FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1FE,((_tmp756=_cycalloc(sizeof(*_tmp756)),((_tmp756->hd=_tmp201,((_tmp756->tl=0,_tmp756)))))));}}{
# 903
const char*_tmp759;const char*_tmp758;const char*_tmp757;struct Cyc_PP_Doc*_tmp208=Cyc_PP_group(((_tmp757="",_tag_dyneither(_tmp757,sizeof(char),1))),((_tmp758="",_tag_dyneither(_tmp758,sizeof(char),1))),((_tmp759=",",_tag_dyneither(_tmp759,sizeof(char),2))),_tmp1FE);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp75C;struct Cyc_PP_Doc*_tmp75B[3];_tmp208=((_tmp75B[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp75B[1]=Cyc_PP_text(((_tmp75C=";",_tag_dyneither(_tmp75C,sizeof(char),2)))),((_tmp75B[0]=_tmp208,Cyc_PP_cat(_tag_dyneither(_tmp75B,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp75F;struct Cyc_PP_Doc*_tmp75E[3];_tmp208=((_tmp75E[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp75E[1]=Cyc_PP_text(((_tmp75F=":",_tag_dyneither(_tmp75F,sizeof(char),2)))),((_tmp75E[0]=_tmp208,Cyc_PP_cat(_tag_dyneither(_tmp75E,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp764;const char*_tmp763;struct Cyc_PP_Doc*_tmp762[3];return(_tmp762[2]=Cyc_PP_text(((_tmp763=")",_tag_dyneither(_tmp763,sizeof(char),2)))),((_tmp762[1]=_tmp208,((_tmp762[0]=Cyc_PP_text(((_tmp764="(",_tag_dyneither(_tmp764,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 911
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp214;struct Cyc_Absyn_Tqual _tmp215;void*_tmp216;struct _tuple8*_tmp213=arg;_tmp214=_tmp213->f1;_tmp215=_tmp213->f2;_tmp216=_tmp213->f3;{
struct _tuple8*_tmp765;return(_tmp765=_cycalloc(sizeof(*_tmp765)),((_tmp765->f1=_tmp214,((_tmp765->f2=_tmp215,((_tmp765->f3=_tmp216,_tmp765)))))));};}
# 916
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 918
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp218=0;
int match;
{union Cyc_Absyn_Nmspace _tmp219=(*q).f1;union Cyc_Absyn_Nmspace _tmp21A=_tmp219;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21D;_LL12D: if((_tmp21A.Loc_n).tag != 4)goto _LL12F;_LL12E:
# 923
 _tmp21B=0;goto _LL130;_LL12F: if((_tmp21A.Rel_n).tag != 1)goto _LL131;_tmp21B=(struct Cyc_List_List*)(_tmp21A.Rel_n).val;_LL130:
# 925
 match=0;
_tmp218=_tmp21B;
goto _LL12C;_LL131: if((_tmp21A.C_n).tag != 3)goto _LL133;_tmp21C=(struct Cyc_List_List*)(_tmp21A.C_n).val;_LL132:
# 929
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp21C,Cyc_Absynpp_curr_namespace);
# 931
goto _LL12C;_LL133: if((_tmp21A.Abs_n).tag != 2)goto _LL12C;_tmp21D=(struct Cyc_List_List*)(_tmp21A.Abs_n).val;_LL134:
# 933
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp21D,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp766;_tmp218=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp766=_cycalloc(sizeof(*_tmp766)),((_tmp766->hd=Cyc_Absynpp_cyc_stringptr,((_tmp766->tl=_tmp21D,_tmp766))))): _tmp21D;}
goto _LL12C;_LL12C:;}
# 937
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp768;struct Cyc_List_List*_tmp767;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp218,(
(_tmp767=_cycalloc(sizeof(*_tmp767)),((_tmp767->hd=(*q).f2,((_tmp767->tl=0,_tmp767))))))),((_tmp768="_",_tag_dyneither(_tmp768,sizeof(char),2))));}else{
# 942
if(match)
return*(*q).f2;else{
# 945
const char*_tmp76A;struct Cyc_List_List*_tmp769;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp218,(
(_tmp769=_cycalloc(sizeof(*_tmp769)),((_tmp769->hd=(*q).f2,((_tmp769->tl=0,_tmp769))))))),((_tmp76A="::",_tag_dyneither(_tmp76A,sizeof(char),3))));}}}
# 950
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 954
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp223=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 958
const char*_tmp76C;const char*_tmp76B;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp76B="\\textbf{",_tag_dyneither(_tmp76B,sizeof(char),9))),(struct _dyneither_ptr)_tmp223),((_tmp76C="}",_tag_dyneither(_tmp76C,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp223));}else{
return Cyc_PP_text(_tmp223);}}
# 963
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 965
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 968
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp226=(*v).f1;union Cyc_Absyn_Nmspace _tmp227=_tmp226;struct Cyc_List_List*_tmp228;struct Cyc_List_List*_tmp229;_LL136: if((_tmp227.Loc_n).tag != 4)goto _LL138;_LL137:
 goto _LL139;_LL138: if((_tmp227.Rel_n).tag != 1)goto _LL13A;if((struct Cyc_List_List*)(_tmp227.Rel_n).val != 0)goto _LL13A;_LL139:
 return*(*v).f2;_LL13A: if((_tmp227.C_n).tag != 3)goto _LL13C;_tmp228=(struct Cyc_List_List*)(_tmp227.C_n).val;_LL13B:
 _tmp229=_tmp228;goto _LL13D;_LL13C: if((_tmp227.Abs_n).tag != 2)goto _LL13E;_tmp229=(struct Cyc_List_List*)(_tmp227.Abs_n).val;_LL13D:
# 974
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp229,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 977
goto _LL13F;}_LL13E:;_LL13F: {
# 979
const char*_tmp76D;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp76D="/* bad namespace : */ ",_tag_dyneither(_tmp76D,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL135:;}else{
# 982
return*(*v).f2;}}
# 985
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 988
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp22B=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 992
const char*_tmp76F;const char*_tmp76E;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp76E="\\textbf{",_tag_dyneither(_tmp76E,sizeof(char),9))),(struct _dyneither_ptr)_tmp22B),((_tmp76F="}",_tag_dyneither(_tmp76F,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp22B));}else{
return Cyc_PP_text(_tmp22B);}}
# 997
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1008 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp22E=e->r;void*_tmp22F=_tmp22E;enum Cyc_Absyn_Primop _tmp233;struct Cyc_Absyn_Exp*_tmp23D;struct Cyc_Absyn_Exp*_tmp23F;_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp230=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp230->tag != 0)goto _LL143;}_LL142:
 goto _LL144;_LL143: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp231=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp231->tag != 1)goto _LL145;}_LL144:
 return 10000;_LL145: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp232=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp232->tag != 2)goto _LL147;else{_tmp233=_tmp232->f1;}}_LL146:
# 1013
 switch(_tmp233){case Cyc_Absyn_Plus: _LL191:
 return 100;case Cyc_Absyn_Times: _LL192:
 return 110;case Cyc_Absyn_Minus: _LL193:
 return 100;case Cyc_Absyn_Div: _LL194:
 goto _LL195;case Cyc_Absyn_Mod: _LL195:
 return 110;case Cyc_Absyn_Eq: _LL196:
 goto _LL197;case Cyc_Absyn_Neq: _LL197:
 return 70;case Cyc_Absyn_Gt: _LL198:
 goto _LL199;case Cyc_Absyn_Lt: _LL199:
 goto _LL19A;case Cyc_Absyn_Gte: _LL19A:
 goto _LL19B;case Cyc_Absyn_Lte: _LL19B:
 return 80;case Cyc_Absyn_Not: _LL19C:
 goto _LL19D;case Cyc_Absyn_Bitnot: _LL19D:
 return 130;case Cyc_Absyn_Bitand: _LL19E:
 return 60;case Cyc_Absyn_Bitor: _LL19F:
 return 40;case Cyc_Absyn_Bitxor: _LL1A0:
 return 50;case Cyc_Absyn_Bitlshift: _LL1A1:
 return 90;case Cyc_Absyn_Bitlrshift: _LL1A2:
 return 80;case Cyc_Absyn_Bitarshift: _LL1A3:
 return 80;case Cyc_Absyn_Numelts: _LL1A4:
 return 140;}_LL147: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp234=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp234->tag != 3)goto _LL149;}_LL148:
# 1035
 return 20;_LL149: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp235=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp235->tag != 4)goto _LL14B;}_LL14A:
 return 130;_LL14B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp236->tag != 5)goto _LL14D;}_LL14C:
 return 30;_LL14D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp237->tag != 6)goto _LL14F;}_LL14E:
 return 35;_LL14F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp238=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp238->tag != 7)goto _LL151;}_LL150:
 return 30;_LL151: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp239->tag != 8)goto _LL153;}_LL152:
 return 10;_LL153: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp23A->tag != 9)goto _LL155;}_LL154:
 return 140;_LL155: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp23B->tag != 10)goto _LL157;}_LL156:
 return 130;_LL157: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp23C=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp23C->tag != 11)goto _LL159;else{_tmp23D=_tmp23C->f1;}}_LL158:
 return Cyc_Absynpp_exp_prec(_tmp23D);_LL159: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp23E->tag != 12)goto _LL15B;else{_tmp23F=_tmp23E->f1;}}_LL15A:
 return Cyc_Absynpp_exp_prec(_tmp23F);_LL15B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp240->tag != 13)goto _LL15D;}_LL15C:
 return 120;_LL15D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp241->tag != 15)goto _LL15F;}_LL15E:
 return 15;_LL15F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp242->tag != 14)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp243->tag != 16)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp244->tag != 17)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp245->tag != 38)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp246->tag != 39)goto _LL169;}_LL168:
 goto _LL16A;_LL169: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp247->tag != 37)goto _LL16B;}_LL16A:
 goto _LL16C;_LL16B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp248->tag != 18)goto _LL16D;}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp249=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp249->tag != 19)goto _LL16F;}_LL16E:
 return 130;_LL16F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24A->tag != 20)goto _LL171;}_LL170:
 goto _LL172;_LL171: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp24B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24B->tag != 21)goto _LL173;}_LL172:
 goto _LL174;_LL173: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24C->tag != 22)goto _LL175;}_LL174:
 return 140;_LL175: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp24D=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24D->tag != 23)goto _LL177;}_LL176:
 return 150;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24E->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp24F->tag != 25)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp250=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp250->tag != 26)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp251->tag != 27)goto _LL17F;}_LL17E:
 goto _LL180;_LL17F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp252=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp252->tag != 28)goto _LL181;}_LL180:
 goto _LL182;_LL181: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp253->tag != 29)goto _LL183;}_LL182:
 goto _LL184;_LL183: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp254=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp254->tag != 30)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp255->tag != 31)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp256->tag != 32)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp257=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp257->tag != 33)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp258->tag != 34)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp259=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp259->tag != 35)goto _LL18F;}_LL18E:
 return 140;_LL18F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp22F;if(_tmp25A->tag != 36)goto _LL140;}_LL190:
 return 10000;_LL140:;}
# 1075
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1079
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp25B=e->r;void*_tmp25C=_tmp25B;union Cyc_Absyn_Cnst _tmp25E;struct _tuple0*_tmp260;enum Cyc_Absyn_Primop _tmp262;struct Cyc_List_List*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Core_Opt*_tmp266;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp269;enum Cyc_Absyn_Incrementor _tmp26A;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp279;struct Cyc_List_List*_tmp27A;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27E;struct Cyc_Absyn_Exp*_tmp280;void*_tmp282;struct Cyc_Absyn_Exp*_tmp283;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*_tmp287;struct Cyc_Absyn_Exp*_tmp288;void*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;void*_tmp28E;int _tmp290;struct _dyneither_ptr _tmp291;struct Cyc_Absyn_Exp*_tmp293;struct _dyneither_ptr*_tmp294;void*_tmp296;struct Cyc_List_List*_tmp297;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Exp*_tmp29B;struct _dyneither_ptr*_tmp29C;struct Cyc_Absyn_Exp*_tmp29E;struct _dyneither_ptr*_tmp29F;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Exp*_tmp2A2;struct Cyc_List_List*_tmp2A4;struct _tuple8*_tmp2A6;struct Cyc_List_List*_tmp2A7;struct Cyc_List_List*_tmp2A9;struct Cyc_Absyn_Vardecl*_tmp2AB;struct Cyc_Absyn_Exp*_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AF;void*_tmp2B0;struct _tuple0*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_List_List*_tmp2B4;struct Cyc_List_List*_tmp2B6;struct Cyc_List_List*_tmp2B8;struct Cyc_Absyn_Datatypefield*_tmp2B9;struct _tuple0*_tmp2BB;struct _tuple0*_tmp2BD;int _tmp2BF;struct Cyc_Absyn_Exp*_tmp2C0;void**_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C2;struct Cyc_Absyn_Exp*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C5;struct Cyc_Core_Opt*_tmp2C7;struct Cyc_List_List*_tmp2C8;struct Cyc_Absyn_Stmt*_tmp2CA;_LL1A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp25D->tag != 0)goto _LL1A9;else{_tmp25E=_tmp25D->f1;}}_LL1A8:
 s=Cyc_Absynpp_cnst2doc(_tmp25E);goto _LL1A6;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp25F->tag != 1)goto _LL1AB;else{_tmp260=_tmp25F->f1;}}_LL1AA:
 s=Cyc_Absynpp_qvar2doc(_tmp260);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp261=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp261->tag != 2)goto _LL1AD;else{_tmp262=_tmp261->f1;_tmp263=_tmp261->f2;}}_LL1AC:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp262,_tmp263);goto _LL1A6;_LL1AD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp264=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp264->tag != 3)goto _LL1AF;else{_tmp265=_tmp264->f1;_tmp266=_tmp264->f2;_tmp267=_tmp264->f3;}}_LL1AE:
# 1087
{const char*_tmp774;const char*_tmp773;struct Cyc_PP_Doc*_tmp772[5];s=((_tmp772[4]=
# 1091
Cyc_Absynpp_exp2doc_prec(myprec,_tmp267),((_tmp772[3]=
# 1090
Cyc_PP_text(((_tmp773="= ",_tag_dyneither(_tmp773,sizeof(char),3)))),((_tmp772[2]=
# 1089
_tmp266 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp266->v),((_tmp772[1]=
# 1088
Cyc_PP_text(((_tmp774=" ",_tag_dyneither(_tmp774,sizeof(char),2)))),((_tmp772[0]=
# 1087
Cyc_Absynpp_exp2doc_prec(myprec,_tmp265),Cyc_PP_cat(_tag_dyneither(_tmp772,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1092
goto _LL1A6;_LL1AF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp268->tag != 4)goto _LL1B1;else{_tmp269=_tmp268->f1;_tmp26A=_tmp268->f2;}}_LL1B0: {
# 1094
struct Cyc_PP_Doc*_tmp2CE=Cyc_Absynpp_exp2doc_prec(myprec,_tmp269);
switch(_tmp26A){case Cyc_Absyn_PreInc: _LL1F7:
{const char*_tmp777;struct Cyc_PP_Doc*_tmp776[2];s=((_tmp776[1]=_tmp2CE,((_tmp776[0]=Cyc_PP_text(((_tmp777="++",_tag_dyneither(_tmp777,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp776,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F8:
{const char*_tmp77A;struct Cyc_PP_Doc*_tmp779[2];s=((_tmp779[1]=_tmp2CE,((_tmp779[0]=Cyc_PP_text(((_tmp77A="--",_tag_dyneither(_tmp77A,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp779,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F9:
{const char*_tmp77D;struct Cyc_PP_Doc*_tmp77C[2];s=((_tmp77C[1]=Cyc_PP_text(((_tmp77D="++",_tag_dyneither(_tmp77D,sizeof(char),3)))),((_tmp77C[0]=_tmp2CE,Cyc_PP_cat(_tag_dyneither(_tmp77C,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FA:
{const char*_tmp780;struct Cyc_PP_Doc*_tmp77F[2];s=((_tmp77F[1]=Cyc_PP_text(((_tmp780="--",_tag_dyneither(_tmp780,sizeof(char),3)))),((_tmp77F[0]=_tmp2CE,Cyc_PP_cat(_tag_dyneither(_tmp77F,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1101
goto _LL1A6;}_LL1B1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp26B=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp26B->tag != 5)goto _LL1B3;else{_tmp26C=_tmp26B->f1;_tmp26D=_tmp26B->f2;_tmp26E=_tmp26B->f3;}}_LL1B2:
# 1103
{const char*_tmp785;const char*_tmp784;struct Cyc_PP_Doc*_tmp783[5];s=((_tmp783[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp26E),((_tmp783[3]=Cyc_PP_text(((_tmp784=" : ",_tag_dyneither(_tmp784,sizeof(char),4)))),((_tmp783[2]=
# 1103
Cyc_Absynpp_exp2doc_prec(0,_tmp26D),((_tmp783[1]=Cyc_PP_text(((_tmp785=" ? ",_tag_dyneither(_tmp785,sizeof(char),4)))),((_tmp783[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp26C),Cyc_PP_cat(_tag_dyneither(_tmp783,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1105
goto _LL1A6;_LL1B3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp26F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp26F->tag != 6)goto _LL1B5;else{_tmp270=_tmp26F->f1;_tmp271=_tmp26F->f2;}}_LL1B4:
# 1107
{const char*_tmp788;struct Cyc_PP_Doc*_tmp787[3];s=((_tmp787[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp271),((_tmp787[1]=Cyc_PP_text(((_tmp788=" && ",_tag_dyneither(_tmp788,sizeof(char),5)))),((_tmp787[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),Cyc_PP_cat(_tag_dyneither(_tmp787,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp272=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp272->tag != 7)goto _LL1B7;else{_tmp273=_tmp272->f1;_tmp274=_tmp272->f2;}}_LL1B6:
# 1110
{const char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[3];s=((_tmp78A[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp274),((_tmp78A[1]=Cyc_PP_text(((_tmp78B=" || ",_tag_dyneither(_tmp78B,sizeof(char),5)))),((_tmp78A[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),Cyc_PP_cat(_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp275=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp275->tag != 8)goto _LL1B9;else{_tmp276=_tmp275->f1;_tmp277=_tmp275->f2;}}_LL1B8:
# 1115
{const char*_tmp792;const char*_tmp791;const char*_tmp790;struct Cyc_PP_Doc*_tmp78F[5];s=((_tmp78F[4]=Cyc_PP_text(((_tmp790=")",_tag_dyneither(_tmp790,sizeof(char),2)))),((_tmp78F[3]=Cyc_Absynpp_exp2doc(_tmp277),((_tmp78F[2]=Cyc_PP_text(((_tmp791=", ",_tag_dyneither(_tmp791,sizeof(char),3)))),((_tmp78F[1]=Cyc_Absynpp_exp2doc(_tmp276),((_tmp78F[0]=Cyc_PP_text(((_tmp792="(",_tag_dyneither(_tmp792,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1B9: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp278->tag != 9)goto _LL1BB;else{_tmp279=_tmp278->f1;_tmp27A=_tmp278->f2;}}_LL1BA:
# 1118
{const char*_tmp797;const char*_tmp796;struct Cyc_PP_Doc*_tmp795[4];s=((_tmp795[3]=
# 1121
Cyc_PP_text(((_tmp796=")",_tag_dyneither(_tmp796,sizeof(char),2)))),((_tmp795[2]=
# 1120
Cyc_Absynpp_exps2doc_prec(20,_tmp27A),((_tmp795[1]=
# 1119
Cyc_PP_text(((_tmp797="(",_tag_dyneither(_tmp797,sizeof(char),2)))),((_tmp795[0]=
# 1118
Cyc_Absynpp_exp2doc_prec(myprec,_tmp279),Cyc_PP_cat(_tag_dyneither(_tmp795,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1122
goto _LL1A6;_LL1BB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp27B=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp27B->tag != 10)goto _LL1BD;else{_tmp27C=_tmp27B->f1;}}_LL1BC:
# 1124
{const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[2];s=((_tmp799[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp27C),((_tmp799[0]=Cyc_PP_text(((_tmp79A="throw ",_tag_dyneither(_tmp79A,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp27D=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp27D->tag != 11)goto _LL1BF;else{_tmp27E=_tmp27D->f1;}}_LL1BE:
# 1127
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp27E);
goto _LL1A6;_LL1BF: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp27F=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp27F->tag != 12)goto _LL1C1;else{_tmp280=_tmp27F->f1;}}_LL1C0:
# 1130
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp280);
goto _LL1A6;_LL1C1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp281=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp281->tag != 13)goto _LL1C3;else{_tmp282=(void*)_tmp281->f1;_tmp283=_tmp281->f2;}}_LL1C2:
# 1133
{const char*_tmp79F;const char*_tmp79E;struct Cyc_PP_Doc*_tmp79D[4];s=((_tmp79D[3]=
# 1136
Cyc_Absynpp_exp2doc_prec(myprec,_tmp283),((_tmp79D[2]=
# 1135
Cyc_PP_text(((_tmp79E=")",_tag_dyneither(_tmp79E,sizeof(char),2)))),((_tmp79D[1]=
# 1134
Cyc_Absynpp_typ2doc(_tmp282),((_tmp79D[0]=
# 1133
Cyc_PP_text(((_tmp79F="(",_tag_dyneither(_tmp79F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp79D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1137
goto _LL1A6;_LL1C3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp284=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp284->tag != 14)goto _LL1C5;else{_tmp285=_tmp284->f1;}}_LL1C4:
# 1139
{const char*_tmp7A2;struct Cyc_PP_Doc*_tmp7A1[2];s=((_tmp7A1[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp285),((_tmp7A1[0]=
# 1139
Cyc_PP_text(((_tmp7A2="&",_tag_dyneither(_tmp7A2,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7A1,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1141
goto _LL1A6;_LL1C5: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp286=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp286->tag != 15)goto _LL1C7;else{_tmp287=_tmp286->f1;_tmp288=_tmp286->f2;}}_LL1C6:
# 1143
 if(_tmp287 == 0){
const char*_tmp7A5;struct Cyc_PP_Doc*_tmp7A4[2];s=((_tmp7A4[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),((_tmp7A4[0]=Cyc_PP_text(((_tmp7A5="new ",_tag_dyneither(_tmp7A5,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp7A4,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1146
const char*_tmp7AA;const char*_tmp7A9;struct Cyc_PP_Doc*_tmp7A8[4];s=((_tmp7A8[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp288),((_tmp7A8[2]=
# 1146
Cyc_PP_text(((_tmp7A9=") ",_tag_dyneither(_tmp7A9,sizeof(char),3)))),((_tmp7A8[1]=Cyc_Absynpp_exp2doc(_tmp287),((_tmp7A8[0]=Cyc_PP_text(((_tmp7AA="rnew(",_tag_dyneither(_tmp7AA,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp7A8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1148
goto _LL1A6;_LL1C7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp289=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp289->tag != 16)goto _LL1C9;else{_tmp28A=(void*)_tmp289->f1;}}_LL1C8:
{const char*_tmp7AF;const char*_tmp7AE;struct Cyc_PP_Doc*_tmp7AD[3];s=((_tmp7AD[2]=Cyc_PP_text(((_tmp7AE=")",_tag_dyneither(_tmp7AE,sizeof(char),2)))),((_tmp7AD[1]=Cyc_Absynpp_typ2doc(_tmp28A),((_tmp7AD[0]=Cyc_PP_text(((_tmp7AF="sizeof(",_tag_dyneither(_tmp7AF,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7AD,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1C9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp28B=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp28B->tag != 17)goto _LL1CB;else{_tmp28C=_tmp28B->f1;}}_LL1CA:
{const char*_tmp7B4;const char*_tmp7B3;struct Cyc_PP_Doc*_tmp7B2[3];s=((_tmp7B2[2]=Cyc_PP_text(((_tmp7B3=")",_tag_dyneither(_tmp7B3,sizeof(char),2)))),((_tmp7B2[1]=Cyc_Absynpp_exp2doc(_tmp28C),((_tmp7B2[0]=Cyc_PP_text(((_tmp7B4="sizeof(",_tag_dyneither(_tmp7B4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7B2,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CB: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp28D=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp28D->tag != 38)goto _LL1CD;else{_tmp28E=(void*)_tmp28D->f1;}}_LL1CC:
{const char*_tmp7B9;const char*_tmp7B8;struct Cyc_PP_Doc*_tmp7B7[3];s=((_tmp7B7[2]=Cyc_PP_text(((_tmp7B8=")",_tag_dyneither(_tmp7B8,sizeof(char),2)))),((_tmp7B7[1]=Cyc_Absynpp_typ2doc(_tmp28E),((_tmp7B7[0]=Cyc_PP_text(((_tmp7B9="valueof(",_tag_dyneither(_tmp7B9,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7B7,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CD: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp28F->tag != 39)goto _LL1CF;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1CE:
# 1153
 if(_tmp290){
const char*_tmp7C0;const char*_tmp7BF;const char*_tmp7BE;struct Cyc_PP_Doc*_tmp7BD[4];s=((_tmp7BD[3]=Cyc_PP_text(((_tmp7BE=")",_tag_dyneither(_tmp7BE,sizeof(char),2)))),((_tmp7BD[2]=Cyc_PP_text(_tmp291),((_tmp7BD[1]=Cyc_PP_text(((_tmp7BF=" volatile (",_tag_dyneither(_tmp7BF,sizeof(char),12)))),((_tmp7BD[0]=Cyc_PP_text(((_tmp7C0="__asm__",_tag_dyneither(_tmp7C0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7BD,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1156
const char*_tmp7C5;const char*_tmp7C4;struct Cyc_PP_Doc*_tmp7C3[3];s=((_tmp7C3[2]=Cyc_PP_text(((_tmp7C4=")",_tag_dyneither(_tmp7C4,sizeof(char),2)))),((_tmp7C3[1]=Cyc_PP_text(_tmp291),((_tmp7C3[0]=Cyc_PP_text(((_tmp7C5="__asm__(",_tag_dyneither(_tmp7C5,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7C3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1CF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp292->tag != 37)goto _LL1D1;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}_LL1D0:
# 1159
{const char*_tmp7CC;const char*_tmp7CB;const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[5];s=((_tmp7C9[4]=Cyc_PP_text(((_tmp7CA=")",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((_tmp7C9[3]=Cyc_PP_textptr(_tmp294),((_tmp7C9[2]=Cyc_PP_text(((_tmp7CB=".",_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7C9[1]=Cyc_Absynpp_exp2doc(_tmp293),((_tmp7C9[0]=Cyc_PP_text(((_tmp7CC="tagcheck(",_tag_dyneither(_tmp7CC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1D1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp295->tag != 18)goto _LL1D3;else{_tmp296=(void*)_tmp295->f1;_tmp297=_tmp295->f2;}}_LL1D2:
# 1162
{const char*_tmp7D1;const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[3];s=((_tmp7CF[2]=Cyc_PP_text(((_tmp7D0=",",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[1]=Cyc_Absynpp_typ2doc(_tmp296),((_tmp7CF[0]=Cyc_PP_text(((_tmp7D1="offsetof(",_tag_dyneither(_tmp7D1,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp297 != 0;_tmp297=_tmp297->tl){
void*_tmp308=(void*)_tmp297->hd;void*_tmp309=_tmp308;struct _dyneither_ptr*_tmp30B;unsigned int _tmp30D;_LL1FD: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp30A=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp309;if(_tmp30A->tag != 0)goto _LL1FF;else{_tmp30B=_tmp30A->f1;}}_LL1FE:
{const char*_tmp7D4;struct Cyc_PP_Doc*_tmp7D3[3];s=((_tmp7D3[2]=
_tmp297->tl != 0?Cyc_PP_text(((_tmp7D4=".",_tag_dyneither(_tmp7D4,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7D3[1]=
# 1165
Cyc_PP_textptr(_tmp30B),((_tmp7D3[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7D3,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1167
goto _LL1FC;_LL1FF: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp30C=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp309;if(_tmp30C->tag != 1)goto _LL1FC;else{_tmp30D=_tmp30C->f1;}}_LL200:
{struct Cyc_Int_pa_PrintArg_struct _tmp7DE;void*_tmp7DD[1];const char*_tmp7DC;const char*_tmp7DB;struct Cyc_PP_Doc*_tmp7DA[3];s=((_tmp7DA[2]=
_tmp297->tl != 0?Cyc_PP_text(((_tmp7DB=".",_tag_dyneither(_tmp7DB,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7DA[1]=
# 1168
Cyc_PP_text((struct _dyneither_ptr)((_tmp7DE.tag=1,((_tmp7DE.f1=(unsigned long)((int)_tmp30D),((_tmp7DD[0]=& _tmp7DE,Cyc_aprintf(((_tmp7DC="%d",_tag_dyneither(_tmp7DC,sizeof(char),3))),_tag_dyneither(_tmp7DD,sizeof(void*),1))))))))),((_tmp7DA[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7DA,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1170
goto _LL1FC;_LL1FC:;}
# 1172
{const char*_tmp7E1;struct Cyc_PP_Doc*_tmp7E0[2];s=((_tmp7E0[1]=Cyc_PP_text(((_tmp7E1=")",_tag_dyneither(_tmp7E1,sizeof(char),2)))),((_tmp7E0[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7E0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp298=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp298->tag != 19)goto _LL1D5;else{_tmp299=_tmp298->f1;}}_LL1D4:
# 1175
{const char*_tmp7E4;struct Cyc_PP_Doc*_tmp7E3[2];s=((_tmp7E3[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp299),((_tmp7E3[0]=Cyc_PP_text(((_tmp7E4="*",_tag_dyneither(_tmp7E4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E3,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp29A=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp29A->tag != 20)goto _LL1D7;else{_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;}}_LL1D6:
# 1178
{const char*_tmp7E7;struct Cyc_PP_Doc*_tmp7E6[3];s=((_tmp7E6[2]=Cyc_PP_textptr(_tmp29C),((_tmp7E6[1]=Cyc_PP_text(((_tmp7E7=".",_tag_dyneither(_tmp7E7,sizeof(char),2)))),((_tmp7E6[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29B),Cyc_PP_cat(_tag_dyneither(_tmp7E6,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp29D=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp29D->tag != 21)goto _LL1D9;else{_tmp29E=_tmp29D->f1;_tmp29F=_tmp29D->f2;}}_LL1D8:
# 1181
{const char*_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[3];s=((_tmp7E9[2]=Cyc_PP_textptr(_tmp29F),((_tmp7E9[1]=Cyc_PP_text(((_tmp7EA="->",_tag_dyneither(_tmp7EA,sizeof(char),3)))),((_tmp7E9[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp29E),Cyc_PP_cat(_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp2A0=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2A0->tag != 22)goto _LL1DB;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;}}_LL1DA:
# 1184
{const char*_tmp7EF;const char*_tmp7EE;struct Cyc_PP_Doc*_tmp7ED[4];s=((_tmp7ED[3]=
# 1187
Cyc_PP_text(((_tmp7EE="]",_tag_dyneither(_tmp7EE,sizeof(char),2)))),((_tmp7ED[2]=
# 1186
Cyc_Absynpp_exp2doc(_tmp2A2),((_tmp7ED[1]=
# 1185
Cyc_PP_text(((_tmp7EF="[",_tag_dyneither(_tmp7EF,sizeof(char),2)))),((_tmp7ED[0]=
# 1184
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2A1),Cyc_PP_cat(_tag_dyneither(_tmp7ED,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1188
goto _LL1A6;_LL1DB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp2A3=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2A3->tag != 23)goto _LL1DD;else{_tmp2A4=_tmp2A3->f1;}}_LL1DC:
# 1190
{const char*_tmp7F4;const char*_tmp7F3;struct Cyc_PP_Doc*_tmp7F2[4];s=((_tmp7F2[3]=
# 1193
Cyc_PP_text(((_tmp7F3=")",_tag_dyneither(_tmp7F3,sizeof(char),2)))),((_tmp7F2[2]=
# 1192
Cyc_Absynpp_exps2doc_prec(20,_tmp2A4),((_tmp7F2[1]=
# 1191
Cyc_PP_text(((_tmp7F4="(",_tag_dyneither(_tmp7F4,sizeof(char),2)))),((_tmp7F2[0]=
# 1190
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7F2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1194
goto _LL1A6;_LL1DD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp2A5=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2A5->tag != 24)goto _LL1DF;else{_tmp2A6=_tmp2A5->f1;_tmp2A7=_tmp2A5->f2;}}_LL1DE:
# 1196
{const char*_tmp7FB;const char*_tmp7FA;const char*_tmp7F9;struct Cyc_PP_Doc*_tmp7F8[4];s=((_tmp7F8[3]=
# 1199
Cyc_Absynpp_group_braces(((_tmp7F9=",",_tag_dyneither(_tmp7F9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A7)),((_tmp7F8[2]=
# 1198
Cyc_PP_text(((_tmp7FA=")",_tag_dyneither(_tmp7FA,sizeof(char),2)))),((_tmp7F8[1]=
# 1197
Cyc_Absynpp_typ2doc((*_tmp2A6).f3),((_tmp7F8[0]=
# 1196
Cyc_PP_text(((_tmp7FB="(",_tag_dyneither(_tmp7FB,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7F8,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1200
goto _LL1A6;_LL1DF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp2A8=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2A8->tag != 25)goto _LL1E1;else{_tmp2A9=_tmp2A8->f1;}}_LL1E0:
# 1202
{const char*_tmp7FC;s=Cyc_Absynpp_group_braces(((_tmp7FC=",",_tag_dyneither(_tmp7FC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A9));}
goto _LL1A6;_LL1E1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp2AA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2AA->tag != 26)goto _LL1E3;else{_tmp2AB=_tmp2AA->f1;_tmp2AC=_tmp2AA->f2;_tmp2AD=_tmp2AA->f3;}}_LL1E2:
# 1205
{const char*_tmp803;const char*_tmp802;const char*_tmp801;struct Cyc_PP_Doc*_tmp800[8];s=((_tmp800[7]=
# 1211
Cyc_Absynpp_rb(),((_tmp800[6]=
# 1210
Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp800[5]=
# 1209
Cyc_PP_text(((_tmp801=" : ",_tag_dyneither(_tmp801,sizeof(char),4)))),((_tmp800[4]=
# 1208
Cyc_Absynpp_exp2doc(_tmp2AC),((_tmp800[3]=
# 1207
Cyc_PP_text(((_tmp802=" < ",_tag_dyneither(_tmp802,sizeof(char),4)))),((_tmp800[2]=
# 1206
Cyc_PP_text(*(*_tmp2AB->name).f2),((_tmp800[1]=
# 1205
Cyc_PP_text(((_tmp803="for ",_tag_dyneither(_tmp803,sizeof(char),5)))),((_tmp800[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp800,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1212
goto _LL1A6;_LL1E3: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2AE->tag != 27)goto _LL1E5;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=(void*)_tmp2AE->f2;}}_LL1E4:
# 1214
{const char*_tmp80A;const char*_tmp809;const char*_tmp808;struct Cyc_PP_Doc*_tmp807[7];s=((_tmp807[6]=
# 1219
Cyc_Absynpp_rb(),((_tmp807[5]=
# 1218
Cyc_Absynpp_typ2doc(_tmp2B0),((_tmp807[4]=
# 1217
Cyc_PP_text(((_tmp808=" : ",_tag_dyneither(_tmp808,sizeof(char),4)))),((_tmp807[3]=
# 1216
Cyc_Absynpp_exp2doc(_tmp2AF),((_tmp807[2]=
# 1215
Cyc_PP_text(((_tmp809=" < ",_tag_dyneither(_tmp809,sizeof(char),4)))),((_tmp807[1]=
# 1214
Cyc_PP_text(((_tmp80A="for x ",_tag_dyneither(_tmp80A,sizeof(char),7)))),((_tmp807[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp807,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1220
goto _LL1A6;_LL1E5: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2B1->tag != 28)goto _LL1E7;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;_tmp2B4=_tmp2B1->f3;}}_LL1E6: {
# 1222
struct Cyc_List_List*_tmp330=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B4);
{const char*_tmp80F;struct Cyc_List_List*_tmp80E;struct Cyc_PP_Doc*_tmp80D[2];s=((_tmp80D[1]=
Cyc_Absynpp_group_braces(((_tmp80F=",",_tag_dyneither(_tmp80F,sizeof(char),2))),
_tmp2B3 != 0?(_tmp80E=_cycalloc(sizeof(*_tmp80E)),((_tmp80E->hd=Cyc_Absynpp_tps2doc(_tmp2B3),((_tmp80E->tl=_tmp330,_tmp80E))))): _tmp330),((_tmp80D[0]=
# 1223
Cyc_Absynpp_qvar2doc(_tmp2B2),Cyc_PP_cat(_tag_dyneither(_tmp80D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1226
goto _LL1A6;}_LL1E7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2B5->tag != 29)goto _LL1E9;else{_tmp2B6=_tmp2B5->f2;}}_LL1E8:
# 1228
{const char*_tmp810;s=Cyc_Absynpp_group_braces(((_tmp810=",",_tag_dyneither(_tmp810,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B6));}
goto _LL1A6;_LL1E9: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2B7=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2B7->tag != 30)goto _LL1EB;else{_tmp2B8=_tmp2B7->f1;_tmp2B9=_tmp2B7->f3;}}_LL1EA:
# 1231
 if(_tmp2B8 == 0)
# 1233
s=Cyc_Absynpp_qvar2doc(_tmp2B9->name);else{
# 1235
const char*_tmp817;const char*_tmp816;const char*_tmp815;struct Cyc_PP_Doc*_tmp814[2];s=((_tmp814[1]=
Cyc_PP_egroup(((_tmp817="(",_tag_dyneither(_tmp817,sizeof(char),2))),((_tmp816=")",_tag_dyneither(_tmp816,sizeof(char),2))),((_tmp815=",",_tag_dyneither(_tmp815,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2B8)),((_tmp814[0]=
# 1235
Cyc_Absynpp_qvar2doc(_tmp2B9->name),Cyc_PP_cat(_tag_dyneither(_tmp814,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1237
goto _LL1A6;_LL1EB: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2BA=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2BA->tag != 31)goto _LL1ED;else{_tmp2BB=_tmp2BA->f1;}}_LL1EC:
# 1239
 s=Cyc_Absynpp_qvar2doc(_tmp2BB);
goto _LL1A6;_LL1ED: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2BC=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2BC->tag != 32)goto _LL1EF;else{_tmp2BD=_tmp2BC->f1;}}_LL1EE:
# 1242
 s=Cyc_Absynpp_qvar2doc(_tmp2BD);
goto _LL1A6;_LL1EF: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2BE=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2BE->tag != 33)goto _LL1F1;else{_tmp2BF=(_tmp2BE->f1).is_calloc;_tmp2C0=(_tmp2BE->f1).rgn;_tmp2C1=(_tmp2BE->f1).elt_type;_tmp2C2=(_tmp2BE->f1).num_elts;}}_LL1F0:
# 1247
 if(_tmp2BF){
# 1249
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2C1)),0);
if(_tmp2C0 == 0){
const char*_tmp81E;const char*_tmp81D;const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[5];s=((_tmp81B[4]=Cyc_PP_text(((_tmp81C=")",_tag_dyneither(_tmp81C,sizeof(char),2)))),((_tmp81B[3]=Cyc_Absynpp_exp2doc(st),((_tmp81B[2]=Cyc_PP_text(((_tmp81D=",",_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81B[1]=Cyc_Absynpp_exp2doc(_tmp2C2),((_tmp81B[0]=Cyc_PP_text(((_tmp81E="calloc(",_tag_dyneither(_tmp81E,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1253
const char*_tmp827;const char*_tmp826;const char*_tmp825;const char*_tmp824;struct Cyc_PP_Doc*_tmp823[7];s=((_tmp823[6]=
Cyc_PP_text(((_tmp824=")",_tag_dyneither(_tmp824,sizeof(char),2)))),((_tmp823[5]=Cyc_Absynpp_exp2doc(st),((_tmp823[4]=Cyc_PP_text(((_tmp825=",",_tag_dyneither(_tmp825,sizeof(char),2)))),((_tmp823[3]=Cyc_Absynpp_exp2doc(_tmp2C2),((_tmp823[2]=
# 1253
Cyc_PP_text(((_tmp826=",",_tag_dyneither(_tmp826,sizeof(char),2)))),((_tmp823[1]=Cyc_Absynpp_exp2doc(_tmp2C0),((_tmp823[0]=Cyc_PP_text(((_tmp827="rcalloc(",_tag_dyneither(_tmp827,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp823,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1256
struct Cyc_Absyn_Exp*new_e;
# 1258
if(_tmp2C1 == 0)
new_e=_tmp2C2;else{
# 1261
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2C1,0),_tmp2C2,0);}
# 1263
if(_tmp2C0 == 0){
const char*_tmp82C;const char*_tmp82B;struct Cyc_PP_Doc*_tmp82A[3];s=((_tmp82A[2]=Cyc_PP_text(((_tmp82B=")",_tag_dyneither(_tmp82B,sizeof(char),2)))),((_tmp82A[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp82A[0]=Cyc_PP_text(((_tmp82C="malloc(",_tag_dyneither(_tmp82C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1266
const char*_tmp833;const char*_tmp832;const char*_tmp831;struct Cyc_PP_Doc*_tmp830[5];s=((_tmp830[4]=
Cyc_PP_text(((_tmp831=")",_tag_dyneither(_tmp831,sizeof(char),2)))),((_tmp830[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp830[2]=
# 1266
Cyc_PP_text(((_tmp832=",",_tag_dyneither(_tmp832,sizeof(char),2)))),((_tmp830[1]=Cyc_Absynpp_exp2doc(_tmp2C0),((_tmp830[0]=Cyc_PP_text(((_tmp833="rmalloc(",_tag_dyneither(_tmp833,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp830,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1269
goto _LL1A6;_LL1F1: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2C3=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2C3->tag != 34)goto _LL1F3;else{_tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;}}_LL1F2:
# 1271
{const char*_tmp836;struct Cyc_PP_Doc*_tmp835[3];s=((_tmp835[2]=
# 1273
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C5),((_tmp835[1]=
# 1272
Cyc_PP_text(((_tmp836=" :=: ",_tag_dyneither(_tmp836,sizeof(char),6)))),((_tmp835[0]=
# 1271
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2C4),Cyc_PP_cat(_tag_dyneither(_tmp835,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1274
goto _LL1A6;_LL1F3: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2C6=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2C6->tag != 35)goto _LL1F5;else{_tmp2C7=_tmp2C6->f1;_tmp2C8=_tmp2C6->f2;}}_LL1F4:
# 1277
{const char*_tmp837;s=Cyc_Absynpp_group_braces(((_tmp837=",",_tag_dyneither(_tmp837,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2C8));}
goto _LL1A6;_LL1F5: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2C9=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp25C;if(_tmp2C9->tag != 36)goto _LL1A6;else{_tmp2CA=_tmp2C9->f1;}}_LL1F6:
# 1280
{const char*_tmp83C;const char*_tmp83B;struct Cyc_PP_Doc*_tmp83A[7];s=((_tmp83A[6]=
# 1282
Cyc_PP_text(((_tmp83B=")",_tag_dyneither(_tmp83B,sizeof(char),2)))),((_tmp83A[5]=Cyc_Absynpp_rb(),((_tmp83A[4]=Cyc_PP_blank_doc(),((_tmp83A[3]=
# 1281
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2CA,1)),((_tmp83A[2]=
# 1280
Cyc_PP_blank_doc(),((_tmp83A[1]=Cyc_Absynpp_lb(),((_tmp83A[0]=Cyc_PP_text(((_tmp83C="(",_tag_dyneither(_tmp83C,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp83A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1283
goto _LL1A6;_LL1A6:;}
# 1285
if(inprec >= myprec){
const char*_tmp841;const char*_tmp840;struct Cyc_PP_Doc*_tmp83F[3];s=((_tmp83F[2]=Cyc_PP_text(((_tmp840=")",_tag_dyneither(_tmp840,sizeof(char),2)))),((_tmp83F[1]=s,((_tmp83F[0]=Cyc_PP_text(((_tmp841="(",_tag_dyneither(_tmp841,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp83F,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1290
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp352=d;struct Cyc_Absyn_Exp*_tmp354;struct _dyneither_ptr*_tmp356;_LL202: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp353=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp352;if(_tmp353->tag != 0)goto _LL204;else{_tmp354=_tmp353->f1;}}_LL203: {
const char*_tmp846;const char*_tmp845;struct Cyc_PP_Doc*_tmp844[3];return(_tmp844[2]=Cyc_PP_text(((_tmp845="]",_tag_dyneither(_tmp845,sizeof(char),2)))),((_tmp844[1]=Cyc_Absynpp_exp2doc(_tmp354),((_tmp844[0]=Cyc_PP_text(((_tmp846=".[",_tag_dyneither(_tmp846,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp844,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL204: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp355=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp352;if(_tmp355->tag != 1)goto _LL201;else{_tmp356=_tmp355->f1;}}_LL205: {
const char*_tmp849;struct Cyc_PP_Doc*_tmp848[2];return(_tmp848[1]=Cyc_PP_textptr(_tmp356),((_tmp848[0]=Cyc_PP_text(((_tmp849=".",_tag_dyneither(_tmp849,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp848,sizeof(struct Cyc_PP_Doc*),2)))));}_LL201:;}
# 1297
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp850;const char*_tmp84F;const char*_tmp84E;struct Cyc_PP_Doc*_tmp84D[2];return(_tmp84D[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp84D[0]=
# 1299
Cyc_PP_egroup(((_tmp850="",_tag_dyneither(_tmp850,sizeof(char),1))),((_tmp84F="=",_tag_dyneither(_tmp84F,sizeof(char),2))),((_tmp84E="=",_tag_dyneither(_tmp84E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp84D,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1303
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp853;const char*_tmp852;const char*_tmp851;return Cyc_PP_group(((_tmp851="",_tag_dyneither(_tmp851,sizeof(char),1))),((_tmp852="",_tag_dyneither(_tmp852,sizeof(char),1))),((_tmp853=",",_tag_dyneither(_tmp853,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1308
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp363=c;enum Cyc_Absyn_Sign _tmp364;char _tmp365;struct _dyneither_ptr _tmp366;enum Cyc_Absyn_Sign _tmp367;short _tmp368;int _tmp369;int _tmp36A;int _tmp36B;enum Cyc_Absyn_Sign _tmp36C;long long _tmp36D;struct _dyneither_ptr _tmp36E;struct _dyneither_ptr _tmp36F;struct _dyneither_ptr _tmp370;_LL207: if((_tmp363.Char_c).tag != 2)goto _LL209;_tmp364=((struct _tuple3)(_tmp363.Char_c).val).f1;_tmp365=((struct _tuple3)(_tmp363.Char_c).val).f2;_LL208: {
const char*_tmp857;void*_tmp856[1];struct Cyc_String_pa_PrintArg_struct _tmp855;return Cyc_PP_text((struct _dyneither_ptr)((_tmp855.tag=0,((_tmp855.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp365)),((_tmp856[0]=& _tmp855,Cyc_aprintf(((_tmp857="'%s'",_tag_dyneither(_tmp857,sizeof(char),5))),_tag_dyneither(_tmp856,sizeof(void*),1)))))))));}_LL209: if((_tmp363.Wchar_c).tag != 3)goto _LL20B;_tmp366=(struct _dyneither_ptr)(_tmp363.Wchar_c).val;_LL20A: {
const char*_tmp85B;void*_tmp85A[1];struct Cyc_String_pa_PrintArg_struct _tmp859;return Cyc_PP_text((struct _dyneither_ptr)((_tmp859.tag=0,((_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp366),((_tmp85A[0]=& _tmp859,Cyc_aprintf(((_tmp85B="L'%s'",_tag_dyneither(_tmp85B,sizeof(char),6))),_tag_dyneither(_tmp85A,sizeof(void*),1)))))))));}_LL20B: if((_tmp363.Short_c).tag != 4)goto _LL20D;_tmp367=((struct _tuple4)(_tmp363.Short_c).val).f1;_tmp368=((struct _tuple4)(_tmp363.Short_c).val).f2;_LL20C: {
const char*_tmp85F;void*_tmp85E[1];struct Cyc_Int_pa_PrintArg_struct _tmp85D;return Cyc_PP_text((struct _dyneither_ptr)((_tmp85D.tag=1,((_tmp85D.f1=(unsigned long)((int)_tmp368),((_tmp85E[0]=& _tmp85D,Cyc_aprintf(((_tmp85F="%d",_tag_dyneither(_tmp85F,sizeof(char),3))),_tag_dyneither(_tmp85E,sizeof(void*),1)))))))));}_LL20D: if((_tmp363.Int_c).tag != 5)goto _LL20F;if(((struct _tuple5)(_tmp363.Int_c).val).f1 != Cyc_Absyn_None)goto _LL20F;_tmp369=((struct _tuple5)(_tmp363.Int_c).val).f2;_LL20E:
 _tmp36A=_tmp369;goto _LL210;_LL20F: if((_tmp363.Int_c).tag != 5)goto _LL211;if(((struct _tuple5)(_tmp363.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL211;_tmp36A=((struct _tuple5)(_tmp363.Int_c).val).f2;_LL210: {
const char*_tmp863;void*_tmp862[1];struct Cyc_Int_pa_PrintArg_struct _tmp861;return Cyc_PP_text((struct _dyneither_ptr)((_tmp861.tag=1,((_tmp861.f1=(unsigned long)_tmp36A,((_tmp862[0]=& _tmp861,Cyc_aprintf(((_tmp863="%d",_tag_dyneither(_tmp863,sizeof(char),3))),_tag_dyneither(_tmp862,sizeof(void*),1)))))))));}_LL211: if((_tmp363.Int_c).tag != 5)goto _LL213;if(((struct _tuple5)(_tmp363.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL213;_tmp36B=((struct _tuple5)(_tmp363.Int_c).val).f2;_LL212: {
const char*_tmp867;void*_tmp866[1];struct Cyc_Int_pa_PrintArg_struct _tmp865;return Cyc_PP_text((struct _dyneither_ptr)((_tmp865.tag=1,((_tmp865.f1=(unsigned int)_tmp36B,((_tmp866[0]=& _tmp865,Cyc_aprintf(((_tmp867="%u",_tag_dyneither(_tmp867,sizeof(char),3))),_tag_dyneither(_tmp866,sizeof(void*),1)))))))));}_LL213: if((_tmp363.LongLong_c).tag != 6)goto _LL215;_tmp36C=((struct _tuple6)(_tmp363.LongLong_c).val).f1;_tmp36D=((struct _tuple6)(_tmp363.LongLong_c).val).f2;_LL214: {
# 1317
const char*_tmp868;return Cyc_PP_text(((_tmp868="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp868,sizeof(char),27))));}_LL215: if((_tmp363.Float_c).tag != 7)goto _LL217;_tmp36E=((struct _tuple7)(_tmp363.Float_c).val).f1;_LL216:
 return Cyc_PP_text(_tmp36E);_LL217: if((_tmp363.Null_c).tag != 1)goto _LL219;_LL218: {
const char*_tmp869;return Cyc_PP_text(((_tmp869="NULL",_tag_dyneither(_tmp869,sizeof(char),5))));}_LL219: if((_tmp363.String_c).tag != 8)goto _LL21B;_tmp36F=(struct _dyneither_ptr)(_tmp363.String_c).val;_LL21A: {
# 1321
const char*_tmp86E;const char*_tmp86D;struct Cyc_PP_Doc*_tmp86C[3];return(_tmp86C[2]=Cyc_PP_text(((_tmp86D="\"",_tag_dyneither(_tmp86D,sizeof(char),2)))),((_tmp86C[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp36F)),((_tmp86C[0]=Cyc_PP_text(((_tmp86E="\"",_tag_dyneither(_tmp86E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp86C,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21B: if((_tmp363.Wstring_c).tag != 9)goto _LL206;_tmp370=(struct _dyneither_ptr)(_tmp363.Wstring_c).val;_LL21C: {
# 1323
const char*_tmp873;const char*_tmp872;struct Cyc_PP_Doc*_tmp871[3];return(_tmp871[2]=Cyc_PP_text(((_tmp872="\"",_tag_dyneither(_tmp872,sizeof(char),2)))),((_tmp871[1]=Cyc_PP_text(_tmp370),((_tmp871[0]=Cyc_PP_text(((_tmp873="L\"",_tag_dyneither(_tmp873,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp871,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL206:;}
# 1327
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp880;const char*_tmp87F;void*_tmp87E[1];struct Cyc_String_pa_PrintArg_struct _tmp87D;struct Cyc_Core_Failure_exn_struct*_tmp87C;(int)_throw((void*)((_tmp87C=_cycalloc(sizeof(*_tmp87C)),((_tmp87C[0]=((_tmp880.tag=Cyc_Core_Failure,((_tmp880.f1=(struct _dyneither_ptr)((_tmp87D.tag=0,((_tmp87D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp87E[0]=& _tmp87D,Cyc_aprintf(((_tmp87F="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp87F,sizeof(char),47))),_tag_dyneither(_tmp87E,sizeof(void*),1)))))))),_tmp880)))),_tmp87C)))));}{
# 1333
const char*_tmp885;const char*_tmp884;struct Cyc_PP_Doc*_tmp883[3];return(_tmp883[2]=Cyc_PP_text(((_tmp884=")",_tag_dyneither(_tmp884,sizeof(char),2)))),((_tmp883[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp883[0]=Cyc_PP_text(((_tmp885="numelts(",_tag_dyneither(_tmp885,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp883,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1335
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp892;const char*_tmp891;void*_tmp890[1];struct Cyc_String_pa_PrintArg_struct _tmp88F;struct Cyc_Core_Failure_exn_struct*_tmp88E;(int)_throw((void*)((_tmp88E=_cycalloc(sizeof(*_tmp88E)),((_tmp88E[0]=((_tmp892.tag=Cyc_Core_Failure,((_tmp892.f1=(struct _dyneither_ptr)((_tmp88F.tag=0,((_tmp88F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp890[0]=& _tmp88F,Cyc_aprintf(((_tmp891="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp891,sizeof(char),38))),_tag_dyneither(_tmp890,sizeof(void*),1)))))))),_tmp892)))),_tmp88E)))));}else{
# 1339
if(ds->tl == 0){
const char*_tmp895;struct Cyc_PP_Doc*_tmp894[3];return(_tmp894[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp894[1]=Cyc_PP_text(((_tmp895=" ",_tag_dyneither(_tmp895,sizeof(char),2)))),((_tmp894[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp894,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp8A2;const char*_tmp8A1;void*_tmp8A0[1];struct Cyc_String_pa_PrintArg_struct _tmp89F;struct Cyc_Core_Failure_exn_struct*_tmp89E;(int)_throw((void*)((_tmp89E=_cycalloc(sizeof(*_tmp89E)),((_tmp89E[0]=((_tmp8A2.tag=Cyc_Core_Failure,((_tmp8A2.f1=(struct _dyneither_ptr)((_tmp89F.tag=0,((_tmp89F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp8A0[0]=& _tmp89F,Cyc_aprintf(((_tmp8A1="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp8A1,sizeof(char),47))),_tag_dyneither(_tmp8A0,sizeof(void*),1)))))))),_tmp8A2)))),_tmp89E)))));}else{
# 1345
const char*_tmp8A7;const char*_tmp8A6;struct Cyc_PP_Doc*_tmp8A5[5];return(_tmp8A5[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp8A5[3]=Cyc_PP_text(((_tmp8A6=" ",_tag_dyneither(_tmp8A6,sizeof(char),2)))),((_tmp8A5[2]=ps,((_tmp8A5[1]=Cyc_PP_text(((_tmp8A7=" ",_tag_dyneither(_tmp8A7,sizeof(char),2)))),((_tmp8A5[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp8A5,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1349
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL21D: {
const char*_tmp8A8;return(_tmp8A8="+",_tag_dyneither(_tmp8A8,sizeof(char),2));}case Cyc_Absyn_Times: _LL21E: {
const char*_tmp8A9;return(_tmp8A9="*",_tag_dyneither(_tmp8A9,sizeof(char),2));}case Cyc_Absyn_Minus: _LL21F: {
const char*_tmp8AA;return(_tmp8AA="-",_tag_dyneither(_tmp8AA,sizeof(char),2));}case Cyc_Absyn_Div: _LL220: {
const char*_tmp8AB;return(_tmp8AB="/",_tag_dyneither(_tmp8AB,sizeof(char),2));}case Cyc_Absyn_Mod: _LL221: {
const char*_tmp8AD;const char*_tmp8AC;return Cyc_Absynpp_print_for_cycdoc?(_tmp8AD="\\%",_tag_dyneither(_tmp8AD,sizeof(char),3)):((_tmp8AC="%",_tag_dyneither(_tmp8AC,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL222: {
const char*_tmp8AE;return(_tmp8AE="==",_tag_dyneither(_tmp8AE,sizeof(char),3));}case Cyc_Absyn_Neq: _LL223: {
const char*_tmp8AF;return(_tmp8AF="!=",_tag_dyneither(_tmp8AF,sizeof(char),3));}case Cyc_Absyn_Gt: _LL224: {
const char*_tmp8B0;return(_tmp8B0=">",_tag_dyneither(_tmp8B0,sizeof(char),2));}case Cyc_Absyn_Lt: _LL225: {
const char*_tmp8B1;return(_tmp8B1="<",_tag_dyneither(_tmp8B1,sizeof(char),2));}case Cyc_Absyn_Gte: _LL226: {
const char*_tmp8B2;return(_tmp8B2=">=",_tag_dyneither(_tmp8B2,sizeof(char),3));}case Cyc_Absyn_Lte: _LL227: {
const char*_tmp8B3;return(_tmp8B3="<=",_tag_dyneither(_tmp8B3,sizeof(char),3));}case Cyc_Absyn_Not: _LL228: {
const char*_tmp8B4;return(_tmp8B4="!",_tag_dyneither(_tmp8B4,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL229: {
const char*_tmp8B5;return(_tmp8B5="~",_tag_dyneither(_tmp8B5,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL22A: {
const char*_tmp8B6;return(_tmp8B6="&",_tag_dyneither(_tmp8B6,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL22B: {
const char*_tmp8B7;return(_tmp8B7="|",_tag_dyneither(_tmp8B7,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL22C: {
const char*_tmp8B8;return(_tmp8B8="^",_tag_dyneither(_tmp8B8,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL22D: {
const char*_tmp8B9;return(_tmp8B9="<<",_tag_dyneither(_tmp8B9,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL22E: {
const char*_tmp8BA;return(_tmp8BA=">>",_tag_dyneither(_tmp8BA,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL22F: {
const char*_tmp8BB;return(_tmp8BB=">>>",_tag_dyneither(_tmp8BB,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL230: {
const char*_tmp8BC;return(_tmp8BC="numelts",_tag_dyneither(_tmp8BC,sizeof(char),8));}}}
# 1374
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1378
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp3B4=s->r;void*_tmp3B5=_tmp3B4;_LL233: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3B6=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B5;if(_tmp3B6->tag != 12)goto _LL235;}_LL234:
 return 1;_LL235:;_LL236:
 return 0;_LL232:;}
# 1385
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp3B7=st->r;void*_tmp3B8=_tmp3B7;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_Absyn_Stmt*_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C6;struct Cyc_Absyn_Stmt*_tmp3C7;struct _dyneither_ptr*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CE;struct Cyc_Absyn_Exp*_tmp3CF;struct Cyc_Absyn_Stmt*_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;struct Cyc_List_List*_tmp3D3;struct Cyc_List_List*_tmp3D6;struct Cyc_Absyn_Decl*_tmp3D8;struct Cyc_Absyn_Stmt*_tmp3D9;struct _dyneither_ptr*_tmp3DB;struct Cyc_Absyn_Stmt*_tmp3DC;struct Cyc_Absyn_Stmt*_tmp3DE;struct Cyc_Absyn_Exp*_tmp3DF;struct Cyc_Absyn_Stmt*_tmp3E1;struct Cyc_List_List*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E4;_LL238: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3B9->tag != 0)goto _LL23A;}_LL239:
{const char*_tmp8BD;s=Cyc_PP_text(((_tmp8BD=";",_tag_dyneither(_tmp8BD,sizeof(char),2))));}goto _LL237;_LL23A: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3BA=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3BA->tag != 1)goto _LL23C;else{_tmp3BB=_tmp3BA->f1;}}_LL23B:
{const char*_tmp8C0;struct Cyc_PP_Doc*_tmp8BF[2];s=((_tmp8BF[1]=Cyc_PP_text(((_tmp8C0=";",_tag_dyneither(_tmp8C0,sizeof(char),2)))),((_tmp8BF[0]=Cyc_Absynpp_exp2doc(_tmp3BB),Cyc_PP_cat(_tag_dyneither(_tmp8BF,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL237;_LL23C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3BC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3BC->tag != 2)goto _LL23E;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL23D:
# 1391
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3BD)){
struct Cyc_PP_Doc*_tmp8CB[5];struct Cyc_PP_Doc*_tmp8CA[7];const char*_tmp8C9;const char*_tmp8C8;struct Cyc_PP_Doc*_tmp8C7[7];s=((_tmp8C7[6]=
# 1399
Cyc_Absynpp_is_declaration(_tmp3BE)?
stmtexp?
(_tmp8CA[6]=
# 1404
Cyc_PP_line_doc(),((_tmp8CA[5]=
# 1403
Cyc_PP_text(((_tmp8C9=");",_tag_dyneither(_tmp8C9,sizeof(char),3)))),((_tmp8CA[4]=Cyc_Absynpp_rb(),((_tmp8CA[3]=
# 1402
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp)),((_tmp8CA[2]=
# 1401
Cyc_PP_blank_doc(),((_tmp8CA[1]=Cyc_Absynpp_lb(),((_tmp8CA[0]=Cyc_PP_text(((_tmp8C8="(",_tag_dyneither(_tmp8C8,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8CA,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1405
(_tmp8CB[4]=
# 1408
Cyc_PP_line_doc(),((_tmp8CB[3]=
# 1407
Cyc_Absynpp_rb(),((_tmp8CB[2]=
# 1406
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp)),((_tmp8CB[1]=
# 1405
Cyc_PP_blank_doc(),((_tmp8CB[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8CB,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1409
 Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8C7[5]=
# 1398
Cyc_PP_line_doc(),((_tmp8C7[4]=
# 1397
Cyc_Absynpp_rb(),((_tmp8C7[3]=
# 1396
Cyc_PP_line_doc(),((_tmp8C7[2]=
# 1395
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BD,0)),((_tmp8C7[1]=
# 1394
Cyc_PP_blank_doc(),((_tmp8C7[0]=
# 1393
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1410
if(Cyc_Absynpp_is_declaration(_tmp3BE)){
struct Cyc_PP_Doc*_tmp8D6[4];struct Cyc_PP_Doc*_tmp8D5[6];const char*_tmp8D4;const char*_tmp8D3;struct Cyc_PP_Doc*_tmp8D2[4];s=((_tmp8D2[3]=
# 1421
Cyc_PP_line_doc(),((_tmp8D2[2]=
# 1413
stmtexp?
(_tmp8D5[5]=
# 1416
Cyc_PP_text(((_tmp8D4=");",_tag_dyneither(_tmp8D4,sizeof(char),3)))),((_tmp8D5[4]=Cyc_Absynpp_rb(),((_tmp8D5[3]=
# 1415
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp)),((_tmp8D5[2]=
# 1414
Cyc_PP_blank_doc(),((_tmp8D5[1]=Cyc_Absynpp_lb(),((_tmp8D5[0]=Cyc_PP_text(((_tmp8D3="(",_tag_dyneither(_tmp8D3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8D5,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1418
(_tmp8D6[3]=
# 1420
Cyc_Absynpp_rb(),((_tmp8D6[2]=
# 1419
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp)),((_tmp8D6[1]=
# 1418
Cyc_PP_blank_doc(),((_tmp8D6[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8D6,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8D2[1]=
# 1412
Cyc_PP_line_doc(),((_tmp8D2[0]=
# 1411
Cyc_Absynpp_stmt2doc(_tmp3BD,0),Cyc_PP_cat(_tag_dyneither(_tmp8D2,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1423
struct Cyc_PP_Doc*_tmp8D7[3];s=((_tmp8D7[2]=Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8D7[1]=Cyc_PP_line_doc(),((_tmp8D7[0]=Cyc_Absynpp_stmt2doc(_tmp3BD,0),Cyc_PP_cat(_tag_dyneither(_tmp8D7,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1426
struct Cyc_PP_Doc*_tmp8D8[3];s=((_tmp8D8[2]=Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8D8[1]=Cyc_PP_line_doc(),((_tmp8D8[0]=Cyc_Absynpp_stmt2doc(_tmp3BD,0),Cyc_PP_cat(_tag_dyneither(_tmp8D8,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL23E: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3BF=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3BF->tag != 3)goto _LL240;else{_tmp3C0=_tmp3BF->f1;}}_LL23F:
# 1429
 if(_tmp3C0 == 0){
const char*_tmp8D9;s=Cyc_PP_text(((_tmp8D9="return;",_tag_dyneither(_tmp8D9,sizeof(char),8))));}else{
# 1432
const char*_tmp8DE;const char*_tmp8DD;struct Cyc_PP_Doc*_tmp8DC[3];s=((_tmp8DC[2]=
# 1434
Cyc_PP_text(((_tmp8DD=";",_tag_dyneither(_tmp8DD,sizeof(char),2)))),((_tmp8DC[1]=
# 1433
_tmp3C0 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3C0),((_tmp8DC[0]=
# 1432
Cyc_PP_text(((_tmp8DE="return ",_tag_dyneither(_tmp8DE,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8DC,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1435
goto _LL237;_LL240: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3C1=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3C1->tag != 4)goto _LL242;else{_tmp3C2=_tmp3C1->f1;_tmp3C3=_tmp3C1->f2;_tmp3C4=_tmp3C1->f3;}}_LL241: {
# 1437
int print_else;
{void*_tmp3F8=_tmp3C4->r;void*_tmp3F9=_tmp3F8;_LL25D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3FA=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3F9;if(_tmp3FA->tag != 0)goto _LL25F;}_LL25E:
 print_else=0;goto _LL25C;_LL25F:;_LL260:
 print_else=1;goto _LL25C;_LL25C:;}
# 1442
{const char*_tmp8ED;const char*_tmp8EC;struct Cyc_PP_Doc*_tmp8EB[2];struct Cyc_PP_Doc*_tmp8EA[6];struct Cyc_PP_Doc*_tmp8E9[2];const char*_tmp8E8;struct Cyc_PP_Doc*_tmp8E7[8];s=((_tmp8E7[7]=
# 1449
print_else?
(_tmp8EA[5]=
# 1455
Cyc_Absynpp_rb(),((_tmp8EA[4]=
# 1454
Cyc_PP_line_doc(),((_tmp8EA[3]=
# 1453
Cyc_PP_nest(2,((_tmp8E9[1]=Cyc_Absynpp_stmt2doc(_tmp3C4,0),((_tmp8E9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8EA[2]=
# 1452
Cyc_Absynpp_lb(),((_tmp8EA[1]=
# 1451
Cyc_PP_text(((_tmp8E8="else ",_tag_dyneither(_tmp8E8,sizeof(char),6)))),((_tmp8EA[0]=
# 1450
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8EA,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1456
 Cyc_PP_nil_doc(),((_tmp8E7[6]=
# 1448
Cyc_Absynpp_rb(),((_tmp8E7[5]=
# 1447
Cyc_PP_line_doc(),((_tmp8E7[4]=
# 1446
Cyc_PP_nest(2,((_tmp8EB[1]=Cyc_Absynpp_stmt2doc(_tmp3C3,0),((_tmp8EB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8EB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E7[3]=
# 1445
Cyc_Absynpp_lb(),((_tmp8E7[2]=
# 1444
Cyc_PP_text(((_tmp8EC=") ",_tag_dyneither(_tmp8EC,sizeof(char),3)))),((_tmp8E7[1]=
# 1443
Cyc_Absynpp_exp2doc(_tmp3C2),((_tmp8E7[0]=
# 1442
Cyc_PP_text(((_tmp8ED="if (",_tag_dyneither(_tmp8ED,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E7,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1457
goto _LL237;}_LL242: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3C5=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3C5->tag != 5)goto _LL244;else{_tmp3C6=(_tmp3C5->f1).f1;_tmp3C7=_tmp3C5->f2;}}_LL243:
# 1459
{const char*_tmp8F4;const char*_tmp8F3;struct Cyc_PP_Doc*_tmp8F2[2];struct Cyc_PP_Doc*_tmp8F1[7];s=((_tmp8F1[6]=
# 1465
Cyc_Absynpp_rb(),((_tmp8F1[5]=
# 1464
Cyc_PP_line_doc(),((_tmp8F1[4]=
# 1463
Cyc_PP_nest(2,((_tmp8F2[1]=Cyc_Absynpp_stmt2doc(_tmp3C7,0),((_tmp8F2[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8F2,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8F1[3]=
# 1462
Cyc_Absynpp_lb(),((_tmp8F1[2]=
# 1461
Cyc_PP_text(((_tmp8F3=") ",_tag_dyneither(_tmp8F3,sizeof(char),3)))),((_tmp8F1[1]=
# 1460
Cyc_Absynpp_exp2doc(_tmp3C6),((_tmp8F1[0]=
# 1459
Cyc_PP_text(((_tmp8F4="while (",_tag_dyneither(_tmp8F4,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8F1,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1466
goto _LL237;_LL244: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3C8=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3C8->tag != 6)goto _LL246;}_LL245:
{const char*_tmp8F5;s=Cyc_PP_text(((_tmp8F5="break;",_tag_dyneither(_tmp8F5,sizeof(char),7))));}goto _LL237;_LL246: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3C9=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3C9->tag != 7)goto _LL248;}_LL247:
{const char*_tmp8F6;s=Cyc_PP_text(((_tmp8F6="continue;",_tag_dyneither(_tmp8F6,sizeof(char),10))));}goto _LL237;_LL248: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3CA=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3CA->tag != 8)goto _LL24A;else{_tmp3CB=_tmp3CA->f1;}}_LL249:
{const char*_tmp8FA;void*_tmp8F9[1];struct Cyc_String_pa_PrintArg_struct _tmp8F8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8F8.tag=0,((_tmp8F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3CB),((_tmp8F9[0]=& _tmp8F8,Cyc_aprintf(((_tmp8FA="goto %s;",_tag_dyneither(_tmp8FA,sizeof(char),9))),_tag_dyneither(_tmp8F9,sizeof(void*),1)))))))));}goto _LL237;_LL24A: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3CC=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3CC->tag != 9)goto _LL24C;else{_tmp3CD=_tmp3CC->f1;_tmp3CE=(_tmp3CC->f2).f1;_tmp3CF=(_tmp3CC->f3).f1;_tmp3D0=_tmp3CC->f4;}}_LL24B:
# 1471
{const char*_tmp905;const char*_tmp904;const char*_tmp903;const char*_tmp902;struct Cyc_PP_Doc*_tmp901[2];struct Cyc_PP_Doc*_tmp900[11];s=((_tmp900[10]=
# 1481
Cyc_Absynpp_rb(),((_tmp900[9]=
# 1480
Cyc_PP_line_doc(),((_tmp900[8]=
# 1479
Cyc_PP_nest(2,((_tmp901[1]=Cyc_Absynpp_stmt2doc(_tmp3D0,0),((_tmp901[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp901,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp900[7]=
# 1478
Cyc_Absynpp_lb(),((_tmp900[6]=
# 1477
Cyc_PP_text(((_tmp902=") ",_tag_dyneither(_tmp902,sizeof(char),3)))),((_tmp900[5]=
# 1476
Cyc_Absynpp_exp2doc(_tmp3CF),((_tmp900[4]=
# 1475
Cyc_PP_text(((_tmp903="; ",_tag_dyneither(_tmp903,sizeof(char),3)))),((_tmp900[3]=
# 1474
Cyc_Absynpp_exp2doc(_tmp3CE),((_tmp900[2]=
# 1473
Cyc_PP_text(((_tmp904="; ",_tag_dyneither(_tmp904,sizeof(char),3)))),((_tmp900[1]=
# 1472
Cyc_Absynpp_exp2doc(_tmp3CD),((_tmp900[0]=
# 1471
Cyc_PP_text(((_tmp905="for(",_tag_dyneither(_tmp905,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp900,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1482
goto _LL237;_LL24C: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3D1=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3D1->tag != 10)goto _LL24E;else{_tmp3D2=_tmp3D1->f1;_tmp3D3=_tmp3D1->f2;}}_LL24D:
# 1484
{const char*_tmp90A;const char*_tmp909;struct Cyc_PP_Doc*_tmp908[8];s=((_tmp908[7]=
# 1491
Cyc_Absynpp_rb(),((_tmp908[6]=
# 1490
Cyc_PP_line_doc(),((_tmp908[5]=
# 1489
Cyc_Absynpp_switchclauses2doc(_tmp3D3),((_tmp908[4]=
# 1488
Cyc_PP_line_doc(),((_tmp908[3]=
# 1487
Cyc_Absynpp_lb(),((_tmp908[2]=
# 1486
Cyc_PP_text(((_tmp909=") ",_tag_dyneither(_tmp909,sizeof(char),3)))),((_tmp908[1]=
# 1485
Cyc_Absynpp_exp2doc(_tmp3D2),((_tmp908[0]=
# 1484
Cyc_PP_text(((_tmp90A="switch (",_tag_dyneither(_tmp90A,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1492
goto _LL237;_LL24E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3D4=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3D4->tag != 11)goto _LL250;else{if(_tmp3D4->f1 != 0)goto _LL250;}}_LL24F:
{const char*_tmp90B;s=Cyc_PP_text(((_tmp90B="fallthru;",_tag_dyneither(_tmp90B,sizeof(char),10))));}goto _LL237;_LL250: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3D5=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3D5->tag != 11)goto _LL252;else{_tmp3D6=_tmp3D5->f1;}}_LL251:
# 1495
{const char*_tmp910;const char*_tmp90F;struct Cyc_PP_Doc*_tmp90E[3];s=((_tmp90E[2]=Cyc_PP_text(((_tmp90F=");",_tag_dyneither(_tmp90F,sizeof(char),3)))),((_tmp90E[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3D6),((_tmp90E[0]=Cyc_PP_text(((_tmp910="fallthru(",_tag_dyneither(_tmp910,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp90E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL237;_LL252: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3D7=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3D7->tag != 12)goto _LL254;else{_tmp3D8=_tmp3D7->f1;_tmp3D9=_tmp3D7->f2;}}_LL253:
# 1497
{struct Cyc_PP_Doc*_tmp911[3];s=((_tmp911[2]=
# 1499
Cyc_Absynpp_stmt2doc(_tmp3D9,stmtexp),((_tmp911[1]=
# 1498
Cyc_PP_line_doc(),((_tmp911[0]=
# 1497
Cyc_Absynpp_decl2doc(_tmp3D8),Cyc_PP_cat(_tag_dyneither(_tmp911,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1500
goto _LL237;_LL254: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3DA=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3DA->tag != 13)goto _LL256;else{_tmp3DB=_tmp3DA->f1;_tmp3DC=_tmp3DA->f2;}}_LL255:
# 1502
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3DC)){
if(stmtexp){
const char*_tmp916;const char*_tmp915;struct Cyc_PP_Doc*_tmp914[8];s=((_tmp914[7]=
# 1510
Cyc_PP_text(((_tmp915=");",_tag_dyneither(_tmp915,sizeof(char),3)))),((_tmp914[6]=Cyc_Absynpp_rb(),((_tmp914[5]=
# 1509
Cyc_PP_line_doc(),((_tmp914[4]=
# 1508
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,1)),((_tmp914[3]=
# 1507
Cyc_PP_line_doc(),((_tmp914[2]=
# 1506
Cyc_Absynpp_lb(),((_tmp914[1]=
# 1505
Cyc_PP_text(((_tmp916=": (",_tag_dyneither(_tmp916,sizeof(char),4)))),((_tmp914[0]=
# 1504
Cyc_PP_textptr(_tmp3DB),Cyc_PP_cat(_tag_dyneither(_tmp914,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1512
const char*_tmp919;struct Cyc_PP_Doc*_tmp918[7];s=((_tmp918[6]=
# 1518
Cyc_Absynpp_rb(),((_tmp918[5]=
# 1517
Cyc_PP_line_doc(),((_tmp918[4]=
# 1516
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DC,0)),((_tmp918[3]=
# 1515
Cyc_PP_line_doc(),((_tmp918[2]=
# 1514
Cyc_Absynpp_lb(),((_tmp918[1]=
# 1513
Cyc_PP_text(((_tmp919=": ",_tag_dyneither(_tmp919,sizeof(char),3)))),((_tmp918[0]=
# 1512
Cyc_PP_textptr(_tmp3DB),Cyc_PP_cat(_tag_dyneither(_tmp918,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1520
const char*_tmp91C;struct Cyc_PP_Doc*_tmp91B[3];s=((_tmp91B[2]=Cyc_Absynpp_stmt2doc(_tmp3DC,stmtexp),((_tmp91B[1]=Cyc_PP_text(((_tmp91C=": ",_tag_dyneither(_tmp91C,sizeof(char),3)))),((_tmp91B[0]=Cyc_PP_textptr(_tmp3DB),Cyc_PP_cat(_tag_dyneither(_tmp91B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL256: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3DD=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3DD->tag != 14)goto _LL258;else{_tmp3DE=_tmp3DD->f1;_tmp3DF=(_tmp3DD->f2).f1;}}_LL257:
# 1523
{const char*_tmp923;const char*_tmp922;const char*_tmp921;struct Cyc_PP_Doc*_tmp920[9];s=((_tmp920[8]=
# 1531
Cyc_PP_text(((_tmp921=");",_tag_dyneither(_tmp921,sizeof(char),3)))),((_tmp920[7]=
# 1530
Cyc_Absynpp_exp2doc(_tmp3DF),((_tmp920[6]=
# 1529
Cyc_PP_text(((_tmp922=" while (",_tag_dyneither(_tmp922,sizeof(char),9)))),((_tmp920[5]=
# 1528
Cyc_Absynpp_rb(),((_tmp920[4]=
# 1527
Cyc_PP_line_doc(),((_tmp920[3]=
# 1526
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3DE,0)),((_tmp920[2]=
# 1525
Cyc_PP_line_doc(),((_tmp920[1]=
# 1524
Cyc_Absynpp_lb(),((_tmp920[0]=
# 1523
Cyc_PP_text(((_tmp923="do ",_tag_dyneither(_tmp923,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp920,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1532
goto _LL237;_LL258: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3E0=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3E0->tag != 15)goto _LL25A;else{_tmp3E1=_tmp3E0->f1;_tmp3E2=_tmp3E0->f2;}}_LL259:
# 1534
{const char*_tmp928;const char*_tmp927;struct Cyc_PP_Doc*_tmp926[12];s=((_tmp926[11]=
# 1545
Cyc_Absynpp_rb(),((_tmp926[10]=
# 1544
Cyc_PP_line_doc(),((_tmp926[9]=
# 1543
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3E2)),((_tmp926[8]=
# 1542
Cyc_PP_line_doc(),((_tmp926[7]=
# 1541
Cyc_Absynpp_lb(),((_tmp926[6]=
# 1540
Cyc_PP_text(((_tmp927=" catch ",_tag_dyneither(_tmp927,sizeof(char),8)))),((_tmp926[5]=
# 1539
Cyc_Absynpp_rb(),((_tmp926[4]=
# 1538
Cyc_PP_line_doc(),((_tmp926[3]=
# 1537
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3E1,0)),((_tmp926[2]=
# 1536
Cyc_PP_line_doc(),((_tmp926[1]=
# 1535
Cyc_Absynpp_lb(),((_tmp926[0]=
# 1534
Cyc_PP_text(((_tmp928="try ",_tag_dyneither(_tmp928,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp926,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1546
goto _LL237;_LL25A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3E3=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3B8;if(_tmp3E3->tag != 16)goto _LL237;else{_tmp3E4=_tmp3E3->f1;}}_LL25B:
# 1548
{const char*_tmp92D;const char*_tmp92C;struct Cyc_PP_Doc*_tmp92B[3];s=((_tmp92B[2]=Cyc_PP_text(((_tmp92C=");",_tag_dyneither(_tmp92C,sizeof(char),3)))),((_tmp92B[1]=Cyc_Absynpp_exp2doc(_tmp3E4),((_tmp92B[0]=Cyc_PP_text(((_tmp92D="reset_region(",_tag_dyneither(_tmp92D,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp92B,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL237:;}
# 1551
return s;}
# 1554
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp42A=p->r;void*_tmp42B=_tmp42A;enum Cyc_Absyn_Sign _tmp42F;int _tmp430;char _tmp432;struct _dyneither_ptr _tmp434;struct Cyc_Absyn_Vardecl*_tmp436;struct Cyc_Absyn_Vardecl*_tmp439;struct Cyc_Absyn_Pat*_tmp43A;struct Cyc_Absyn_Tvar*_tmp43C;struct Cyc_Absyn_Vardecl*_tmp43D;struct Cyc_List_List*_tmp43F;int _tmp440;struct Cyc_Absyn_Pat*_tmp442;struct Cyc_Absyn_Vardecl*_tmp444;struct Cyc_Absyn_Vardecl*_tmp447;struct Cyc_Absyn_Pat*_tmp448;struct _tuple0*_tmp44A;struct _tuple0*_tmp44C;struct Cyc_List_List*_tmp44D;int _tmp44E;union Cyc_Absyn_AggrInfoU _tmp450;struct Cyc_List_List*_tmp451;struct Cyc_List_List*_tmp452;int _tmp453;struct Cyc_List_List*_tmp455;struct Cyc_List_List*_tmp456;int _tmp457;struct Cyc_Absyn_Enumfield*_tmp459;struct Cyc_Absyn_Enumfield*_tmp45B;struct Cyc_Absyn_Datatypefield*_tmp45D;struct Cyc_Absyn_Datatypefield*_tmp45F;struct Cyc_List_List*_tmp460;int _tmp461;struct Cyc_Absyn_Exp*_tmp463;_LL262: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp42C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp42C->tag != 0)goto _LL264;}_LL263:
{const char*_tmp92E;s=Cyc_PP_text(((_tmp92E="_",_tag_dyneither(_tmp92E,sizeof(char),2))));}goto _LL261;_LL264: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp42D=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp42D->tag != 8)goto _LL266;}_LL265:
{const char*_tmp92F;s=Cyc_PP_text(((_tmp92F="NULL",_tag_dyneither(_tmp92F,sizeof(char),5))));}goto _LL261;_LL266: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp42E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp42E->tag != 9)goto _LL268;else{_tmp42F=_tmp42E->f1;_tmp430=_tmp42E->f2;}}_LL267:
# 1560
 if(_tmp42F != Cyc_Absyn_Unsigned){
const char*_tmp933;void*_tmp932[1];struct Cyc_Int_pa_PrintArg_struct _tmp931;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp931.tag=1,((_tmp931.f1=(unsigned long)_tmp430,((_tmp932[0]=& _tmp931,Cyc_aprintf(((_tmp933="%d",_tag_dyneither(_tmp933,sizeof(char),3))),_tag_dyneither(_tmp932,sizeof(void*),1)))))))));}else{
const char*_tmp937;void*_tmp936[1];struct Cyc_Int_pa_PrintArg_struct _tmp935;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp935.tag=1,((_tmp935.f1=(unsigned int)_tmp430,((_tmp936[0]=& _tmp935,Cyc_aprintf(((_tmp937="%u",_tag_dyneither(_tmp937,sizeof(char),3))),_tag_dyneither(_tmp936,sizeof(void*),1)))))))));}
goto _LL261;_LL268: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp431=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp431->tag != 10)goto _LL26A;else{_tmp432=_tmp431->f1;}}_LL269:
{const char*_tmp93B;void*_tmp93A[1];struct Cyc_String_pa_PrintArg_struct _tmp939;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp939.tag=0,((_tmp939.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp432)),((_tmp93A[0]=& _tmp939,Cyc_aprintf(((_tmp93B="'%s'",_tag_dyneither(_tmp93B,sizeof(char),5))),_tag_dyneither(_tmp93A,sizeof(void*),1)))))))));}goto _LL261;_LL26A: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp433=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp433->tag != 11)goto _LL26C;else{_tmp434=_tmp433->f1;}}_LL26B:
 s=Cyc_PP_text(_tmp434);goto _LL261;_LL26C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp435=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp435->tag != 1)goto _LL26E;else{_tmp436=_tmp435->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp437=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp435->f2)->r;if(_tmp437->tag != 0)goto _LL26E;};}}_LL26D:
# 1567
 s=Cyc_Absynpp_qvar2doc(_tmp436->name);goto _LL261;_LL26E: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp438=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp438->tag != 1)goto _LL270;else{_tmp439=_tmp438->f1;_tmp43A=_tmp438->f2;}}_LL26F:
# 1570
{const char*_tmp93E;struct Cyc_PP_Doc*_tmp93D[3];s=((_tmp93D[2]=Cyc_Absynpp_pat2doc(_tmp43A),((_tmp93D[1]=Cyc_PP_text(((_tmp93E=" as ",_tag_dyneither(_tmp93E,sizeof(char),5)))),((_tmp93D[0]=Cyc_Absynpp_qvar2doc(_tmp439->name),Cyc_PP_cat(_tag_dyneither(_tmp93D,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL261;_LL270: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp43B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp43B->tag != 3)goto _LL272;else{_tmp43C=_tmp43B->f1;_tmp43D=_tmp43B->f2;}}_LL271:
# 1572
{const char*_tmp943;const char*_tmp942;struct Cyc_PP_Doc*_tmp941[4];s=((_tmp941[3]=Cyc_PP_text(((_tmp942=">",_tag_dyneither(_tmp942,sizeof(char),2)))),((_tmp941[2]=Cyc_Absynpp_tvar2doc(_tmp43C),((_tmp941[1]=Cyc_PP_text(((_tmp943="<",_tag_dyneither(_tmp943,sizeof(char),2)))),((_tmp941[0]=Cyc_Absynpp_qvar2doc(_tmp43D->name),Cyc_PP_cat(_tag_dyneither(_tmp941,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL272: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp43E->tag != 4)goto _LL274;else{_tmp43F=_tmp43E->f1;_tmp440=_tmp43E->f2;}}_LL273:
# 1575
{const char*_tmp94C;const char*_tmp94B;const char*_tmp94A;const char*_tmp949;struct Cyc_PP_Doc*_tmp948[4];s=((_tmp948[3]=
_tmp440?Cyc_PP_text(((_tmp949=", ...)",_tag_dyneither(_tmp949,sizeof(char),7)))): Cyc_PP_text(((_tmp94A=")",_tag_dyneither(_tmp94A,sizeof(char),2)))),((_tmp948[2]=
# 1575
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp94B=",",_tag_dyneither(_tmp94B,sizeof(char),2))),_tmp43F),((_tmp948[1]=Cyc_PP_text(((_tmp94C="(",_tag_dyneither(_tmp94C,sizeof(char),2)))),((_tmp948[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp948,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1577
goto _LL261;_LL274: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp441=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp441->tag != 5)goto _LL276;else{_tmp442=_tmp441->f1;}}_LL275:
# 1579
{const char*_tmp94F;struct Cyc_PP_Doc*_tmp94E[2];s=((_tmp94E[1]=Cyc_Absynpp_pat2doc(_tmp442),((_tmp94E[0]=Cyc_PP_text(((_tmp94F="&",_tag_dyneither(_tmp94F,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp94E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL276: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp443=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp443->tag != 2)goto _LL278;else{_tmp444=_tmp443->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp445=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp443->f2)->r;if(_tmp445->tag != 0)goto _LL278;};}}_LL277:
# 1582
{const char*_tmp952;struct Cyc_PP_Doc*_tmp951[2];s=((_tmp951[1]=Cyc_Absynpp_qvar2doc(_tmp444->name),((_tmp951[0]=Cyc_PP_text(((_tmp952="*",_tag_dyneither(_tmp952,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp951,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL278: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp446=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp446->tag != 2)goto _LL27A;else{_tmp447=_tmp446->f1;_tmp448=_tmp446->f2;}}_LL279:
# 1585
{const char*_tmp957;const char*_tmp956;struct Cyc_PP_Doc*_tmp955[4];s=((_tmp955[3]=Cyc_Absynpp_pat2doc(_tmp448),((_tmp955[2]=Cyc_PP_text(((_tmp956=" as ",_tag_dyneither(_tmp956,sizeof(char),5)))),((_tmp955[1]=Cyc_Absynpp_qvar2doc(_tmp447->name),((_tmp955[0]=Cyc_PP_text(((_tmp957="*",_tag_dyneither(_tmp957,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp955,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL27A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp449=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp449->tag != 14)goto _LL27C;else{_tmp44A=_tmp449->f1;}}_LL27B:
# 1588
 s=Cyc_Absynpp_qvar2doc(_tmp44A);
goto _LL261;_LL27C: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp44B=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp44B->tag != 15)goto _LL27E;else{_tmp44C=_tmp44B->f1;_tmp44D=_tmp44B->f2;_tmp44E=_tmp44B->f3;}}_LL27D: {
# 1591
const char*_tmp959;const char*_tmp958;struct _dyneither_ptr term=_tmp44E?(_tmp959=", ...)",_tag_dyneither(_tmp959,sizeof(char),7)):((_tmp958=")",_tag_dyneither(_tmp958,sizeof(char),2)));
{const char*_tmp95E;const char*_tmp95D;struct Cyc_PP_Doc*_tmp95C[2];s=((_tmp95C[1]=Cyc_PP_group(((_tmp95E="(",_tag_dyneither(_tmp95E,sizeof(char),2))),term,((_tmp95D=",",_tag_dyneither(_tmp95D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp44D)),((_tmp95C[0]=Cyc_Absynpp_qvar2doc(_tmp44C),Cyc_PP_cat(_tag_dyneither(_tmp95C,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL27E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp44F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp44F->tag != 6)goto _LL280;else{if(_tmp44F->f1 == 0)goto _LL280;_tmp450=(_tmp44F->f1)->aggr_info;_tmp451=_tmp44F->f2;_tmp452=_tmp44F->f3;_tmp453=_tmp44F->f4;}}_LL27F: {
# 1595
const char*_tmp960;const char*_tmp95F;struct _dyneither_ptr term=_tmp453?(_tmp960=", ...}",_tag_dyneither(_tmp960,sizeof(char),7)):((_tmp95F="}",_tag_dyneither(_tmp95F,sizeof(char),2)));
struct _tuple10 _tmp485=Cyc_Absyn_aggr_kinded_name(_tmp450);struct _tuple0*_tmp487;struct _tuple10 _tmp486=_tmp485;_tmp487=_tmp486.f2;
{const char*_tmp96B;const char*_tmp96A;const char*_tmp969;const char*_tmp968;const char*_tmp967;struct Cyc_PP_Doc*_tmp966[4];s=((_tmp966[3]=
# 1599
Cyc_PP_group(((_tmp968="",_tag_dyneither(_tmp968,sizeof(char),1))),term,((_tmp967=",",_tag_dyneither(_tmp967,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp452)),((_tmp966[2]=
# 1598
Cyc_PP_egroup(((_tmp96B="[",_tag_dyneither(_tmp96B,sizeof(char),2))),((_tmp96A="]",_tag_dyneither(_tmp96A,sizeof(char),2))),((_tmp969=",",_tag_dyneither(_tmp969,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp451)),((_tmp966[1]=
# 1597
Cyc_Absynpp_lb(),((_tmp966[0]=Cyc_Absynpp_qvar2doc(_tmp487),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1600
goto _LL261;}_LL280: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp454=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp454->tag != 6)goto _LL282;else{if(_tmp454->f1 != 0)goto _LL282;_tmp455=_tmp454->f2;_tmp456=_tmp454->f3;_tmp457=_tmp454->f4;}}_LL281: {
# 1602
const char*_tmp96D;const char*_tmp96C;struct _dyneither_ptr term=_tmp457?(_tmp96D=", ...}",_tag_dyneither(_tmp96D,sizeof(char),7)):((_tmp96C="}",_tag_dyneither(_tmp96C,sizeof(char),2)));
{const char*_tmp978;const char*_tmp977;const char*_tmp976;const char*_tmp975;const char*_tmp974;struct Cyc_PP_Doc*_tmp973[3];s=((_tmp973[2]=
# 1605
Cyc_PP_group(((_tmp975="",_tag_dyneither(_tmp975,sizeof(char),1))),term,((_tmp974=",",_tag_dyneither(_tmp974,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp456)),((_tmp973[1]=
# 1604
Cyc_PP_egroup(((_tmp978="[",_tag_dyneither(_tmp978,sizeof(char),2))),((_tmp977="]",_tag_dyneither(_tmp977,sizeof(char),2))),((_tmp976=",",_tag_dyneither(_tmp976,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp455)),((_tmp973[0]=
# 1603
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp973,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1606
goto _LL261;}_LL282: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp458=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp458->tag != 12)goto _LL284;else{_tmp459=_tmp458->f2;}}_LL283:
 _tmp45B=_tmp459;goto _LL285;_LL284: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp45A=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp45A->tag != 13)goto _LL286;else{_tmp45B=_tmp45A->f2;}}_LL285:
 s=Cyc_Absynpp_qvar2doc(_tmp45B->name);goto _LL261;_LL286: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp45C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp45C->tag != 7)goto _LL288;else{_tmp45D=_tmp45C->f2;if(_tmp45C->f3 != 0)goto _LL288;}}_LL287:
 s=Cyc_Absynpp_qvar2doc(_tmp45D->name);goto _LL261;_LL288: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp45E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp45E->tag != 7)goto _LL28A;else{_tmp45F=_tmp45E->f2;_tmp460=_tmp45E->f3;_tmp461=_tmp45E->f4;}}_LL289: {
# 1611
const char*_tmp97A;const char*_tmp979;struct _dyneither_ptr term=_tmp461?(_tmp97A=", ...)",_tag_dyneither(_tmp97A,sizeof(char),7)):((_tmp979=")",_tag_dyneither(_tmp979,sizeof(char),2)));
{const char*_tmp97F;const char*_tmp97E;struct Cyc_PP_Doc*_tmp97D[2];s=((_tmp97D[1]=Cyc_PP_egroup(((_tmp97F="(",_tag_dyneither(_tmp97F,sizeof(char),2))),term,((_tmp97E=",",_tag_dyneither(_tmp97E,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp460)),((_tmp97D[0]=Cyc_Absynpp_qvar2doc(_tmp45F->name),Cyc_PP_cat(_tag_dyneither(_tmp97D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL28A: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp462=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp42B;if(_tmp462->tag != 16)goto _LL261;else{_tmp463=_tmp462->f1;}}_LL28B:
# 1615
 s=Cyc_Absynpp_exp2doc(_tmp463);goto _LL261;_LL261:;}
# 1617
return s;}
# 1620
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp986;const char*_tmp985;const char*_tmp984;struct Cyc_PP_Doc*_tmp983[2];return(_tmp983[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp983[0]=
# 1621
Cyc_PP_egroup(((_tmp986="",_tag_dyneither(_tmp986,sizeof(char),1))),((_tmp985="=",_tag_dyneither(_tmp985,sizeof(char),2))),((_tmp984="=",_tag_dyneither(_tmp984,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp983,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1625
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp98B;struct Cyc_PP_Doc*_tmp98A[2];struct Cyc_PP_Doc*_tmp989[2];return(_tmp989[1]=
Cyc_PP_nest(2,((_tmp98A[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp98A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp98A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp989[0]=
# 1627
Cyc_PP_text(((_tmp98B="default: ",_tag_dyneither(_tmp98B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1629
if(c->where_clause == 0){
const char*_tmp992;const char*_tmp991;struct Cyc_PP_Doc*_tmp990[2];struct Cyc_PP_Doc*_tmp98F[4];return(_tmp98F[3]=
# 1633
Cyc_PP_nest(2,((_tmp990[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp990[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp98F[2]=
# 1632
Cyc_PP_text(((_tmp991=": ",_tag_dyneither(_tmp991,sizeof(char),3)))),((_tmp98F[1]=
# 1631
Cyc_Absynpp_pat2doc(c->pattern),((_tmp98F[0]=
# 1630
Cyc_PP_text(((_tmp992="case ",_tag_dyneither(_tmp992,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1635
const char*_tmp99B;const char*_tmp99A;const char*_tmp999;struct Cyc_PP_Doc*_tmp998[2];struct Cyc_PP_Doc*_tmp997[6];return(_tmp997[5]=
# 1640
Cyc_PP_nest(2,((_tmp998[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp998[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp998,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp997[4]=
# 1639
Cyc_PP_text(((_tmp999=": ",_tag_dyneither(_tmp999,sizeof(char),3)))),((_tmp997[3]=
# 1638
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp997[2]=
# 1637
Cyc_PP_text(((_tmp99A=" && ",_tag_dyneither(_tmp99A,sizeof(char),5)))),((_tmp997[1]=
# 1636
Cyc_Absynpp_pat2doc(c->pattern),((_tmp997[0]=
# 1635
Cyc_PP_text(((_tmp99B="case ",_tag_dyneither(_tmp99B,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp997,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1643
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp99C;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp99C="",_tag_dyneither(_tmp99C,sizeof(char),1))),cs);}
# 1647
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1651
const char*_tmp99F;struct Cyc_PP_Doc*_tmp99E[3];return(_tmp99E[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp99E[1]=Cyc_PP_text(((_tmp99F=" = ",_tag_dyneither(_tmp99F,sizeof(char),4)))),((_tmp99E[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp99E,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1654
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp9A0;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp9A0=",",_tag_dyneither(_tmp9A0,sizeof(char),2))),fs);}
# 1658
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1662
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp9A1;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp9A1=",",_tag_dyneither(_tmp9A1,sizeof(char),2))),vds);}
# 1666
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp4B3;struct _tuple0*_tmp4B4;struct Cyc_Absyn_Tqual _tmp4B5;void*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B7;struct Cyc_List_List*_tmp4B8;struct Cyc_Absyn_Vardecl*_tmp4B2=vd;_tmp4B3=_tmp4B2->sc;_tmp4B4=_tmp4B2->name;_tmp4B5=_tmp4B2->tq;_tmp4B6=_tmp4B2->type;_tmp4B7=_tmp4B2->initializer;_tmp4B8=_tmp4B2->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp4B4);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4B8);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28C:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL28D:
# 1675
{void*_tmp4B9=Cyc_Tcutil_compress(_tmp4B6);void*_tmp4BA=_tmp4B9;struct Cyc_List_List*_tmp4BC;_LL290: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4BB=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4BA;if(_tmp4BB->tag != 9)goto _LL292;else{_tmp4BC=(_tmp4BB->f1).attributes;}}_LL291:
# 1677
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4BC);
goto _LL28F;_LL292:;_LL293:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL28F;_LL28F:;}
# 1681
break;}{
# 1684
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL294:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL295:
 tmp_doc=attsdoc;break;}
# 1689
{struct Cyc_Core_Opt*_tmp9AE;struct Cyc_PP_Doc*_tmp9AD[2];struct Cyc_PP_Doc*_tmp9AC[2];const char*_tmp9AB;const char*_tmp9AA;struct Cyc_PP_Doc*_tmp9A9[5];s=((_tmp9A9[4]=
# 1696
Cyc_PP_text(((_tmp9AA=";",_tag_dyneither(_tmp9AA,sizeof(char),2)))),((_tmp9A9[3]=
# 1693
_tmp4B7 == 0?
Cyc_PP_nil_doc():(
(_tmp9AC[1]=Cyc_Absynpp_exp2doc(_tmp4B7),((_tmp9AC[0]=Cyc_PP_text(((_tmp9AB=" = ",_tag_dyneither(_tmp9AB,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9A9[2]=
# 1692
Cyc_Absynpp_tqtd2doc(_tmp4B5,_tmp4B6,((_tmp9AE=_cycalloc(sizeof(*_tmp9AE)),((_tmp9AE->v=((_tmp9AD[1]=sn,((_tmp9AD[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9AD,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9AE))))),((_tmp9A9[1]=
# 1691
Cyc_Absynpp_scope2doc(_tmp4B3),((_tmp9A9[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp9A9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1697
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1700
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _tmp4C3=*x;struct _tuple0*_tmp4C5;struct _tuple17 _tmp4C4=_tmp4C3;_tmp4C5=_tmp4C4.f2;
return Cyc_Absynpp_qvar2doc(_tmp4C5);}
# 1705
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp9AF[4];return(_tmp9AF[3]=
# 1710
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9AF[2]=
# 1709
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9AF[1]=
# 1708
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9AF[0]=
# 1707
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9AF,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1712
struct Cyc_PP_Doc*_tmp9B7[2];const char*_tmp9B6;struct Cyc_PP_Doc*_tmp9B5[2];struct Cyc_PP_Doc*_tmp9B4[12];return(_tmp9B4[11]=
# 1723
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp9B4[10]=
# 1722
Cyc_Absynpp_rb(),((_tmp9B4[9]=
# 1721
Cyc_PP_line_doc(),((_tmp9B4[8]=
# 1720
Cyc_PP_nest(2,((_tmp9B5[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp9B5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B4[7]=
# 1718
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp9B7[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp9B7[0]=Cyc_PP_text(((_tmp9B6=":",_tag_dyneither(_tmp9B6,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp9B7,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp9B4[6]=
# 1717
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp9B4[5]=
# 1716
Cyc_Absynpp_lb(),((_tmp9B4[4]=Cyc_PP_blank_doc(),((_tmp9B4[3]=
# 1715
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp9B4[2]=
# 1714
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp9B4[1]=
# 1713
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp9B4[0]=
# 1712
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1726
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4CC;struct _tuple0*_tmp4CD;struct Cyc_List_List*_tmp4CE;struct Cyc_Core_Opt*_tmp4CF;int _tmp4D0;struct Cyc_Absyn_Datatypedecl*_tmp4CB=dd;_tmp4CC=_tmp4CB->sc;_tmp4CD=_tmp4CB->name;_tmp4CE=_tmp4CB->tvs;_tmp4CF=_tmp4CB->fields;_tmp4D0=_tmp4CB->is_extensible;
if(_tmp4CF == 0){
const char*_tmp9BC;const char*_tmp9BB;struct Cyc_PP_Doc*_tmp9BA[5];return(_tmp9BA[4]=
# 1733
Cyc_Absynpp_ktvars2doc(_tmp4CE),((_tmp9BA[3]=
# 1732
_tmp4D0?Cyc_Absynpp_qvar2bolddoc(_tmp4CD): Cyc_Absynpp_typedef_name2bolddoc(_tmp4CD),((_tmp9BA[2]=
# 1731
Cyc_PP_text(((_tmp9BB="datatype ",_tag_dyneither(_tmp9BB,sizeof(char),10)))),((_tmp9BA[1]=
# 1730
_tmp4D0?Cyc_PP_text(((_tmp9BC="@extensible ",_tag_dyneither(_tmp9BC,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9BA[0]=
# 1729
Cyc_Absynpp_scope2doc(_tmp4CC),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1735
const char*_tmp9C3;const char*_tmp9C2;struct Cyc_PP_Doc*_tmp9C1[2];struct Cyc_PP_Doc*_tmp9C0[10];return(_tmp9C0[9]=
# 1743
Cyc_Absynpp_rb(),((_tmp9C0[8]=
# 1742
Cyc_PP_line_doc(),((_tmp9C0[7]=
# 1741
Cyc_PP_nest(2,((_tmp9C1[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4CF->v),((_tmp9C1[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9C1,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C0[6]=
# 1740
Cyc_Absynpp_lb(),((_tmp9C0[5]=Cyc_PP_blank_doc(),((_tmp9C0[4]=
# 1739
Cyc_Absynpp_ktvars2doc(_tmp4CE),((_tmp9C0[3]=
# 1738
_tmp4D0?Cyc_Absynpp_qvar2bolddoc(_tmp4CD): Cyc_Absynpp_typedef_name2bolddoc(_tmp4CD),((_tmp9C0[2]=
# 1737
Cyc_PP_text(((_tmp9C2="datatype ",_tag_dyneither(_tmp9C2,sizeof(char),10)))),((_tmp9C0[1]=
# 1736
_tmp4D0?Cyc_PP_text(((_tmp9C3="@extensible ",_tag_dyneither(_tmp9C3,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9C0[0]=
# 1735
Cyc_Absynpp_scope2doc(_tmp4CC),Cyc_PP_cat(_tag_dyneither(_tmp9C0,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1746
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4D9;struct _tuple0*_tmp4DA;struct Cyc_Core_Opt*_tmp4DB;struct Cyc_Absyn_Enumdecl*_tmp4D8=ed;_tmp4D9=_tmp4D8->sc;_tmp4DA=_tmp4D8->name;_tmp4DB=_tmp4D8->fields;
if(_tmp4DB == 0){
const char*_tmp9C6;struct Cyc_PP_Doc*_tmp9C5[3];return(_tmp9C5[2]=
# 1751
Cyc_Absynpp_typedef_name2bolddoc(_tmp4DA),((_tmp9C5[1]=
# 1750
Cyc_PP_text(((_tmp9C6="enum ",_tag_dyneither(_tmp9C6,sizeof(char),6)))),((_tmp9C5[0]=
# 1749
Cyc_Absynpp_scope2doc(_tmp4D9),Cyc_PP_cat(_tag_dyneither(_tmp9C5,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1754
const char*_tmp9CB;struct Cyc_PP_Doc*_tmp9CA[2];struct Cyc_PP_Doc*_tmp9C9[8];return(_tmp9C9[7]=
# 1760
Cyc_Absynpp_rb(),((_tmp9C9[6]=
# 1759
Cyc_PP_line_doc(),((_tmp9C9[5]=
# 1758
Cyc_PP_nest(2,((_tmp9CA[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4DB->v),((_tmp9CA[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9CA,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9C9[4]=
# 1757
Cyc_Absynpp_lb(),((_tmp9C9[3]=Cyc_PP_blank_doc(),((_tmp9C9[2]=
# 1756
Cyc_Absynpp_qvar2bolddoc(_tmp4DA),((_tmp9C9[1]=
# 1755
Cyc_PP_text(((_tmp9CB="enum ",_tag_dyneither(_tmp9CB,sizeof(char),6)))),((_tmp9C9[0]=
# 1754
Cyc_Absynpp_scope2doc(_tmp4D9),Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1763
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4E1=d->r;void*_tmp4E2=_tmp4E1;struct Cyc_Absyn_Fndecl*_tmp4E4;struct Cyc_Absyn_Aggrdecl*_tmp4E6;struct Cyc_Absyn_Vardecl*_tmp4E8;struct Cyc_Absyn_Tvar*_tmp4EA;struct Cyc_Absyn_Vardecl*_tmp4EB;int _tmp4EC;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Tvar*_tmp4EF;struct Cyc_Absyn_Vardecl*_tmp4F0;struct Cyc_Absyn_Datatypedecl*_tmp4F2;struct Cyc_Absyn_Pat*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_List_List*_tmp4F7;struct Cyc_Absyn_Enumdecl*_tmp4F9;struct Cyc_Absyn_Typedefdecl*_tmp4FB;struct _dyneither_ptr*_tmp4FD;struct Cyc_List_List*_tmp4FE;struct _tuple0*_tmp500;struct Cyc_List_List*_tmp501;struct Cyc_List_List*_tmp503;struct Cyc_List_List*_tmp505;struct Cyc_List_List*_tmp506;_LL298: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4E3=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4E3->tag != 1)goto _LL29A;else{_tmp4E4=_tmp4E3->f1;}}_LL299: {
# 1767
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9D1;struct Cyc_Absyn_FnInfo _tmp9D0;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9CF;void*t=(void*)((_tmp9CF=_cycalloc(sizeof(*_tmp9CF)),((_tmp9CF[0]=((_tmp9D1.tag=9,((_tmp9D1.f1=((_tmp9D0.tvars=_tmp4E4->tvs,((_tmp9D0.effect=_tmp4E4->effect,((_tmp9D0.ret_tqual=_tmp4E4->ret_tqual,((_tmp9D0.ret_typ=_tmp4E4->ret_type,((_tmp9D0.args=
# 1771
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4E4->args),((_tmp9D0.c_varargs=_tmp4E4->c_varargs,((_tmp9D0.cyc_varargs=_tmp4E4->cyc_varargs,((_tmp9D0.rgn_po=_tmp4E4->rgn_po,((_tmp9D0.attributes=0,_tmp9D0)))))))))))))))))),_tmp9D1)))),_tmp9CF))));
# 1775
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4E4->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4E4->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B8:
{const char*_tmp9D2;inlinedoc=Cyc_PP_text(((_tmp9D2="inline ",_tag_dyneither(_tmp9D2,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B9:
{const char*_tmp9D3;inlinedoc=Cyc_PP_text(((_tmp9D3="__inline ",_tag_dyneither(_tmp9D3,sizeof(char),10))));}break;}else{
# 1783
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4E4->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BB:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2BC:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4E4->attributes);break;}{
# 1790
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4E4->name);
struct Cyc_PP_Doc*_tmp9D6[2];struct Cyc_Core_Opt*_tmp9D5;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9D5=_cycalloc(sizeof(*_tmp9D5)),((_tmp9D5->v=((_tmp9D6[1]=namedoc,((_tmp9D6[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9D6,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9D5)))));
struct Cyc_PP_Doc*_tmp9D9[2];struct Cyc_PP_Doc*_tmp9D8[5];struct Cyc_PP_Doc*bodydoc=(_tmp9D8[4]=
# 1796
Cyc_Absynpp_rb(),((_tmp9D8[3]=
# 1795
Cyc_PP_line_doc(),((_tmp9D8[2]=
# 1794
Cyc_PP_nest(2,((_tmp9D9[1]=Cyc_Absynpp_stmt2doc(_tmp4E4->body,0),((_tmp9D9[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9D8[1]=
# 1793
Cyc_Absynpp_lb(),((_tmp9D8[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9D8,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1797
{struct Cyc_PP_Doc*_tmp9DA[4];s=((_tmp9DA[3]=bodydoc,((_tmp9DA[2]=tqtddoc,((_tmp9DA[1]=scopedoc,((_tmp9DA[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9DA,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1799
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2BE:
{struct Cyc_PP_Doc*_tmp9DB[2];s=((_tmp9DB[1]=s,((_tmp9DB[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9DB,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2BF:
 break;}
# 1804
goto _LL297;};};}_LL29A: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4E5=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4E5->tag != 6)goto _LL29C;else{_tmp4E6=_tmp4E5->f1;}}_LL29B:
# 1807
{const char*_tmp9DE;struct Cyc_PP_Doc*_tmp9DD[2];s=((_tmp9DD[1]=Cyc_PP_text(((_tmp9DE=";",_tag_dyneither(_tmp9DE,sizeof(char),2)))),((_tmp9DD[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4E6),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL29C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4E7=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4E7->tag != 0)goto _LL29E;else{_tmp4E8=_tmp4E7->f1;}}_LL29D:
# 1810
 s=Cyc_Absynpp_vardecl2doc(_tmp4E8);
goto _LL297;_LL29E: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4E9=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4E9->tag != 4)goto _LL2A0;else{_tmp4EA=_tmp4E9->f1;_tmp4EB=_tmp4E9->f2;_tmp4EC=_tmp4E9->f3;_tmp4ED=_tmp4E9->f4;}}_LL29F:
# 1813
{const char*_tmp9F1;const char*_tmp9F0;const char*_tmp9EF;const char*_tmp9EE;struct Cyc_PP_Doc*_tmp9ED[3];const char*_tmp9EC;const char*_tmp9EB;const char*_tmp9EA;struct Cyc_PP_Doc*_tmp9E9[8];s=((_tmp9E9[7]=
# 1821
Cyc_PP_text(((_tmp9EA=";",_tag_dyneither(_tmp9EA,sizeof(char),2)))),((_tmp9E9[6]=
# 1819
_tmp4ED != 0?
(_tmp9ED[2]=Cyc_PP_text(((_tmp9EC=")",_tag_dyneither(_tmp9EC,sizeof(char),2)))),((_tmp9ED[1]=Cyc_Absynpp_exp2doc(_tmp4ED),((_tmp9ED[0]=Cyc_PP_text(((_tmp9EB=" = open(",_tag_dyneither(_tmp9EB,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9ED,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9E9[5]=
# 1818
_tmp4EC?Cyc_PP_text(((_tmp9EE=" @resetable",_tag_dyneither(_tmp9EE,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9E9[4]=
# 1817
Cyc_Absynpp_qvar2doc(_tmp4EB->name),((_tmp9E9[3]=
# 1816
Cyc_PP_text(((_tmp9EF=">",_tag_dyneither(_tmp9EF,sizeof(char),2)))),((_tmp9E9[2]=
# 1815
Cyc_Absynpp_tvar2doc(_tmp4EA),((_tmp9E9[1]=
# 1814
Cyc_PP_text(((_tmp9F0="<",_tag_dyneither(_tmp9F0,sizeof(char),2)))),((_tmp9E9[0]=
# 1813
Cyc_PP_text(((_tmp9F1="region",_tag_dyneither(_tmp9F1,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9E9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1822
goto _LL297;_LL2A0: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4EE=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4EE->tag != 5)goto _LL2A2;else{_tmp4EF=_tmp4EE->f1;_tmp4F0=_tmp4EE->f2;}}_LL2A1:
# 1824
{const char*_tmp9FA;const char*_tmp9F9;const char*_tmp9F8;const char*_tmp9F7;struct Cyc_PP_Doc*_tmp9F6[6];s=((_tmp9F6[5]=
Cyc_PP_text(((_tmp9F7=";",_tag_dyneither(_tmp9F7,sizeof(char),2)))),((_tmp9F6[4]=Cyc_Absynpp_vardecl2doc(_tmp4F0),((_tmp9F6[3]=
# 1824
Cyc_PP_text(((_tmp9F8="> ",_tag_dyneither(_tmp9F8,sizeof(char),3)))),((_tmp9F6[2]=Cyc_Absynpp_tvar2doc(_tmp4EF),((_tmp9F6[1]=Cyc_PP_text(((_tmp9F9=" <",_tag_dyneither(_tmp9F9,sizeof(char),3)))),((_tmp9F6[0]=Cyc_PP_text(((_tmp9FA="alias",_tag_dyneither(_tmp9FA,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9F6,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 1826
goto _LL297;_LL2A2: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4F1=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4F1->tag != 7)goto _LL2A4;else{_tmp4F2=_tmp4F1->f1;}}_LL2A3:
# 1828
{const char*_tmp9FD;struct Cyc_PP_Doc*_tmp9FC[2];s=((_tmp9FC[1]=Cyc_PP_text(((_tmp9FD=";",_tag_dyneither(_tmp9FD,sizeof(char),2)))),((_tmp9FC[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4F2),Cyc_PP_cat(_tag_dyneither(_tmp9FC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2A4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4F3=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4F3->tag != 2)goto _LL2A6;else{_tmp4F4=_tmp4F3->f1;_tmp4F5=_tmp4F3->f3;}}_LL2A5:
# 1831
{const char*_tmpA04;const char*_tmpA03;const char*_tmpA02;struct Cyc_PP_Doc*_tmpA01[5];s=((_tmpA01[4]=
# 1835
Cyc_PP_text(((_tmpA02=";",_tag_dyneither(_tmpA02,sizeof(char),2)))),((_tmpA01[3]=
# 1834
Cyc_Absynpp_exp2doc(_tmp4F5),((_tmpA01[2]=
# 1833
Cyc_PP_text(((_tmpA03=" = ",_tag_dyneither(_tmpA03,sizeof(char),4)))),((_tmpA01[1]=
# 1832
Cyc_Absynpp_pat2doc(_tmp4F4),((_tmpA01[0]=
# 1831
Cyc_PP_text(((_tmpA04="let ",_tag_dyneither(_tmpA04,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA01,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1836
goto _LL297;_LL2A6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4F6=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4F6->tag != 3)goto _LL2A8;else{_tmp4F7=_tmp4F6->f1;}}_LL2A7:
# 1838
{const char*_tmpA09;const char*_tmpA08;struct Cyc_PP_Doc*_tmpA07[3];s=((_tmpA07[2]=Cyc_PP_text(((_tmpA08=";",_tag_dyneither(_tmpA08,sizeof(char),2)))),((_tmpA07[1]=Cyc_Absynpp_ids2doc(_tmp4F7),((_tmpA07[0]=Cyc_PP_text(((_tmpA09="let ",_tag_dyneither(_tmpA09,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmpA07,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL297;_LL2A8: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4F8=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4F8->tag != 8)goto _LL2AA;else{_tmp4F9=_tmp4F8->f1;}}_LL2A9:
# 1841
{const char*_tmpA0C;struct Cyc_PP_Doc*_tmpA0B[2];s=((_tmpA0B[1]=Cyc_PP_text(((_tmpA0C=";",_tag_dyneither(_tmpA0C,sizeof(char),2)))),((_tmpA0B[0]=Cyc_Absynpp_enumdecl2doc(_tmp4F9),Cyc_PP_cat(_tag_dyneither(_tmpA0B,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2AA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4FA=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4FA->tag != 9)goto _LL2AC;else{_tmp4FB=_tmp4FA->f1;}}_LL2AB: {
# 1844
void*t;
if(_tmp4FB->defn != 0)
t=(void*)_check_null(_tmp4FB->defn);else{
# 1848
t=Cyc_Absyn_new_evar(_tmp4FB->kind,0);}
{const char*_tmpA16;struct Cyc_Core_Opt*_tmpA15;struct Cyc_PP_Doc*_tmpA14[2];const char*_tmpA13;struct Cyc_PP_Doc*_tmpA12[4];s=((_tmpA12[3]=
# 1856
Cyc_PP_text(((_tmpA13=";",_tag_dyneither(_tmpA13,sizeof(char),2)))),((_tmpA12[2]=
# 1855
Cyc_Absynpp_atts2doc(_tmp4FB->atts),((_tmpA12[1]=
# 1850
Cyc_Absynpp_tqtd2doc(_tmp4FB->tq,t,(
# 1852
(_tmpA15=_cycalloc(sizeof(*_tmpA15)),((_tmpA15->v=((_tmpA14[1]=
Cyc_Absynpp_tvars2doc(_tmp4FB->tvs),((_tmpA14[0]=
# 1852
Cyc_Absynpp_typedef_name2bolddoc(_tmp4FB->name),Cyc_PP_cat(_tag_dyneither(_tmpA14,sizeof(struct Cyc_PP_Doc*),2)))))),_tmpA15))))),((_tmpA12[0]=
# 1849
Cyc_PP_text(((_tmpA16="typedef ",_tag_dyneither(_tmpA16,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmpA12,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1858
goto _LL297;}_LL2AC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4FC=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4FC->tag != 10)goto _LL2AE;else{_tmp4FD=_tmp4FC->f1;_tmp4FE=_tmp4FC->f2;}}_LL2AD:
# 1860
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4FD);
{const char*_tmpA1B;const char*_tmpA1A;struct Cyc_PP_Doc*_tmpA19[8];s=((_tmpA19[7]=
# 1867
Cyc_Absynpp_rb(),((_tmpA19[6]=
# 1866
Cyc_PP_line_doc(),((_tmpA19[5]=
# 1865
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1A="",_tag_dyneither(_tmpA1A,sizeof(char),1))),_tmp4FE),((_tmpA19[4]=
# 1864
Cyc_PP_line_doc(),((_tmpA19[3]=
# 1863
Cyc_Absynpp_lb(),((_tmpA19[2]=Cyc_PP_blank_doc(),((_tmpA19[1]=
# 1862
Cyc_PP_textptr(_tmp4FD),((_tmpA19[0]=
# 1861
Cyc_PP_text(((_tmpA1B="namespace ",_tag_dyneither(_tmpA1B,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA19,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1868
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL297;_LL2AE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4FF=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp4FF->tag != 11)goto _LL2B0;else{_tmp500=_tmp4FF->f1;_tmp501=_tmp4FF->f2;}}_LL2AF:
# 1871
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA20;const char*_tmpA1F;struct Cyc_PP_Doc*_tmpA1E[8];s=((_tmpA1E[7]=
# 1878
Cyc_Absynpp_rb(),((_tmpA1E[6]=
# 1877
Cyc_PP_line_doc(),((_tmpA1E[5]=
# 1876
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1F="",_tag_dyneither(_tmpA1F,sizeof(char),1))),_tmp501),((_tmpA1E[4]=
# 1875
Cyc_PP_line_doc(),((_tmpA1E[3]=
# 1874
Cyc_Absynpp_lb(),((_tmpA1E[2]=Cyc_PP_blank_doc(),((_tmpA1E[1]=
# 1873
Cyc_Absynpp_qvar2doc(_tmp500),((_tmpA1E[0]=
# 1872
Cyc_PP_text(((_tmpA20="using ",_tag_dyneither(_tmpA20,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA1E,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1880
const char*_tmpA2B;const char*_tmpA2A;const char*_tmpA29;const char*_tmpA28;const char*_tmpA27;struct Cyc_PP_Doc*_tmpA26[11];s=((_tmpA26[10]=
# 1890
Cyc_PP_text(((_tmpA27=" */",_tag_dyneither(_tmpA27,sizeof(char),4)))),((_tmpA26[9]=
# 1889
Cyc_Absynpp_rb(),((_tmpA26[8]=
# 1888
Cyc_PP_text(((_tmpA28="/* ",_tag_dyneither(_tmpA28,sizeof(char),4)))),((_tmpA26[7]=
# 1887
Cyc_PP_line_doc(),((_tmpA26[6]=
# 1886
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA29="",_tag_dyneither(_tmpA29,sizeof(char),1))),_tmp501),((_tmpA26[5]=
# 1885
Cyc_PP_line_doc(),((_tmpA26[4]=
# 1884
Cyc_PP_text(((_tmpA2A=" */",_tag_dyneither(_tmpA2A,sizeof(char),4)))),((_tmpA26[3]=
# 1883
Cyc_Absynpp_lb(),((_tmpA26[2]=
# 1882
Cyc_PP_blank_doc(),((_tmpA26[1]=
# 1881
Cyc_Absynpp_qvar2doc(_tmp500),((_tmpA26[0]=
# 1880
Cyc_PP_text(((_tmpA2B="/* using ",_tag_dyneither(_tmpA2B,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA26,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1891
goto _LL297;_LL2B0: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp502=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp502->tag != 12)goto _LL2B2;else{_tmp503=_tmp502->f1;}}_LL2B1:
# 1893
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA30;const char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[6];s=((_tmpA2E[5]=
# 1899
Cyc_Absynpp_rb(),((_tmpA2E[4]=
# 1898
Cyc_PP_line_doc(),((_tmpA2E[3]=
# 1897
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA2F="",_tag_dyneither(_tmpA2F,sizeof(char),1))),_tmp503),((_tmpA2E[2]=
# 1896
Cyc_PP_line_doc(),((_tmpA2E[1]=
# 1895
Cyc_Absynpp_lb(),((_tmpA2E[0]=
# 1894
Cyc_PP_text(((_tmpA30="extern \"C\" ",_tag_dyneither(_tmpA30,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1901
const char*_tmpA3B;const char*_tmpA3A;const char*_tmpA39;const char*_tmpA38;const char*_tmpA37;struct Cyc_PP_Doc*_tmpA36[9];s=((_tmpA36[8]=
# 1909
Cyc_PP_text(((_tmpA37=" */",_tag_dyneither(_tmpA37,sizeof(char),4)))),((_tmpA36[7]=
# 1908
Cyc_Absynpp_rb(),((_tmpA36[6]=
# 1907
Cyc_PP_text(((_tmpA38="/* ",_tag_dyneither(_tmpA38,sizeof(char),4)))),((_tmpA36[5]=
# 1906
Cyc_PP_line_doc(),((_tmpA36[4]=
# 1905
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA39="",_tag_dyneither(_tmpA39,sizeof(char),1))),_tmp503),((_tmpA36[3]=
# 1904
Cyc_PP_line_doc(),((_tmpA36[2]=
# 1903
Cyc_PP_text(((_tmpA3A=" */",_tag_dyneither(_tmpA3A,sizeof(char),4)))),((_tmpA36[1]=
# 1902
Cyc_Absynpp_lb(),((_tmpA36[0]=
# 1901
Cyc_PP_text(((_tmpA3B="/* extern \"C\" ",_tag_dyneither(_tmpA3B,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA36,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1910
goto _LL297;_LL2B2: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp504=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp504->tag != 13)goto _LL2B4;else{_tmp505=_tmp504->f1;_tmp506=_tmp504->f2;}}_LL2B3:
# 1912
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp506 != 0){
const char*_tmpA40;const char*_tmpA3F;struct Cyc_PP_Doc*_tmpA3E[7];exs_doc=((_tmpA3E[6]=
# 1917
Cyc_Absynpp_rb(),((_tmpA3E[5]=Cyc_PP_line_doc(),((_tmpA3E[4]=
# 1916
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA3F=",",_tag_dyneither(_tmpA3F,sizeof(char),2))),_tmp506),((_tmpA3E[3]=Cyc_PP_line_doc(),((_tmpA3E[2]=
# 1915
Cyc_Absynpp_lb(),((_tmpA3E[1]=Cyc_PP_text(((_tmpA40=" export ",_tag_dyneither(_tmpA40,sizeof(char),9)))),((_tmpA3E[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA3E,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1919
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA45;const char*_tmpA44;struct Cyc_PP_Doc*_tmpA43[6];s=((_tmpA43[5]=exs_doc,((_tmpA43[4]=
# 1924
Cyc_PP_line_doc(),((_tmpA43[3]=
# 1923
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA44="",_tag_dyneither(_tmpA44,sizeof(char),1))),_tmp505),((_tmpA43[2]=
# 1922
Cyc_PP_line_doc(),((_tmpA43[1]=
# 1921
Cyc_Absynpp_lb(),((_tmpA43[0]=
# 1920
Cyc_PP_text(((_tmpA45="extern \"C include\" ",_tag_dyneither(_tmpA45,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA43,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1927
const char*_tmpA50;const char*_tmpA4F;const char*_tmpA4E;const char*_tmpA4D;const char*_tmpA4C;struct Cyc_PP_Doc*_tmpA4B[9];s=((_tmpA4B[8]=
# 1935
Cyc_PP_text(((_tmpA4C=" */",_tag_dyneither(_tmpA4C,sizeof(char),4)))),((_tmpA4B[7]=
# 1934
Cyc_Absynpp_rb(),((_tmpA4B[6]=
# 1933
Cyc_PP_text(((_tmpA4D="/* ",_tag_dyneither(_tmpA4D,sizeof(char),4)))),((_tmpA4B[5]=
# 1932
Cyc_PP_line_doc(),((_tmpA4B[4]=
# 1931
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA4E="",_tag_dyneither(_tmpA4E,sizeof(char),1))),_tmp505),((_tmpA4B[3]=
# 1930
Cyc_PP_line_doc(),((_tmpA4B[2]=
# 1929
Cyc_PP_text(((_tmpA4F=" */",_tag_dyneither(_tmpA4F,sizeof(char),4)))),((_tmpA4B[1]=
# 1928
Cyc_Absynpp_lb(),((_tmpA4B[0]=
# 1927
Cyc_PP_text(((_tmpA50="/* extern \"C include\" ",_tag_dyneither(_tmpA50,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA4B,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1936
goto _LL297;_LL2B4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp507=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp507->tag != 14)goto _LL2B6;}_LL2B5:
# 1938
{const char*_tmpA53;struct Cyc_PP_Doc*_tmpA52[2];s=((_tmpA52[1]=Cyc_Absynpp_lb(),((_tmpA52[0]=Cyc_PP_text(((_tmpA53="__cyclone_port_on__;",_tag_dyneither(_tmpA53,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA52,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2B6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp508=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4E2;if(_tmp508->tag != 15)goto _LL297;}_LL2B7:
# 1941
{const char*_tmpA56;struct Cyc_PP_Doc*_tmpA55[2];s=((_tmpA55[1]=Cyc_Absynpp_lb(),((_tmpA55[0]=Cyc_PP_text(((_tmpA56="__cyclone_port_off__;",_tag_dyneither(_tmpA56,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA55,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL297:;}
# 1945
return s;}
# 1948
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2C1: {
const char*_tmpA57;return Cyc_PP_text(((_tmpA57="static ",_tag_dyneither(_tmpA57,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2C2:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2C3: {
const char*_tmpA58;return Cyc_PP_text(((_tmpA58="extern ",_tag_dyneither(_tmpA58,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C4: {
const char*_tmpA59;return Cyc_PP_text(((_tmpA59="extern \"C\" ",_tag_dyneither(_tmpA59,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C5: {
const char*_tmpA5A;return Cyc_PP_text(((_tmpA5A="abstract ",_tag_dyneither(_tmpA5A,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2C6: {
const char*_tmpA5B;return Cyc_PP_text(((_tmpA5B="register ",_tag_dyneither(_tmpA5B,sizeof(char),10))));}}}
# 1961
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp55E=t;struct Cyc_Absyn_Tvar*_tmp560;struct Cyc_List_List*_tmp562;_LL2C9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55F=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp55E;if(_tmp55F->tag != 2)goto _LL2CB;else{_tmp560=_tmp55F->f1;}}_LL2CA:
 return Cyc_Tcutil_is_temp_tvar(_tmp560);_LL2CB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp561=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp55E;if(_tmp561->tag != 24)goto _LL2CD;else{_tmp562=_tmp561->f1;}}_LL2CC:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp562);_LL2CD:;_LL2CE:
 return 0;_LL2C8:;}
# 1973
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp563=t;struct Cyc_Absyn_Typedefdecl*_tmp567;void*_tmp568;_LL2D0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp563;if(_tmp564->tag != 12)goto _LL2D2;}_LL2D1:
 return 1;_LL2D2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp565=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp563;if(_tmp565->tag != 14)goto _LL2D4;}_LL2D3:
 return 1;_LL2D4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp566=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp563;if(_tmp566->tag != 17)goto _LL2D6;else{_tmp567=_tmp566->f3;_tmp568=(void*)_tmp566->f4;}}if(!(_tmp568 != 0))goto _LL2D6;_LL2D5:
# 1980
 return Cyc_Absynpp_is_anon_aggrtype(_tmp568);_LL2D6:;_LL2D7:
 return 0;_LL2CF:;}
# 1987
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1990
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA5C;struct _tuple16 _tmp569=(_tmpA5C.f1=(void*)tms->hd,((_tmpA5C.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA5C)));struct _tuple16 _tmp56A=_tmp569;_LL2D9:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp56B=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp56A.f1;if(_tmp56B->tag != 2)goto _LL2DB;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp56C=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp56A.f2;if(_tmp56C->tag != 3)goto _LL2DB;};_LL2DA: {
# 1993
struct Cyc_List_List*_tmpA5F;struct Cyc_List_List*_tmpA5E;return(_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E->hd=(void*)tms->hd,((_tmpA5E->tl=((_tmpA5F=_region_malloc(r,sizeof(*_tmpA5F)),((_tmpA5F->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA5F->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA5F)))))),_tmpA5E)))));}_LL2DB:;_LL2DC: {
struct Cyc_List_List*_tmpA60;return(_tmpA60=_region_malloc(r,sizeof(*_tmpA60)),((_tmpA60->hd=atts,((_tmpA60->tl=tms,_tmpA60)))));}_LL2D8:;}else{
# 1996
struct Cyc_List_List*_tmpA61;return(_tmpA61=_region_malloc(r,sizeof(*_tmpA61)),((_tmpA61->hd=atts,((_tmpA61->tl=tms,_tmpA61)))));}}
# 2001
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2003
void*_tmp572=t;void*_tmp574;struct Cyc_Absyn_Tqual _tmp575;struct Cyc_Absyn_Exp*_tmp576;union Cyc_Absyn_Constraint*_tmp577;unsigned int _tmp578;void*_tmp57A;struct Cyc_Absyn_Tqual _tmp57B;struct Cyc_Absyn_PtrAtts _tmp57C;struct Cyc_List_List*_tmp57E;void*_tmp57F;struct Cyc_Absyn_Tqual _tmp580;void*_tmp581;struct Cyc_List_List*_tmp582;int _tmp583;struct Cyc_Absyn_VarargInfo*_tmp584;struct Cyc_List_List*_tmp585;struct Cyc_List_List*_tmp586;struct Cyc_Core_Opt*_tmp588;void*_tmp589;int _tmp58A;struct _tuple0*_tmp58C;struct Cyc_List_List*_tmp58D;struct Cyc_Absyn_Typedefdecl*_tmp58E;void*_tmp58F;_LL2DE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp573=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp572;if(_tmp573->tag != 8)goto _LL2E0;else{_tmp574=(_tmp573->f1).elt_type;_tmp575=(_tmp573->f1).tq;_tmp576=(_tmp573->f1).num_elts;_tmp577=(_tmp573->f1).zero_term;_tmp578=(_tmp573->f1).zt_loc;}}_LL2DF: {
# 2006
struct _tuple12 _tmp590=Cyc_Absynpp_to_tms(r,_tmp575,_tmp574);struct Cyc_Absyn_Tqual _tmp592;void*_tmp593;struct Cyc_List_List*_tmp594;struct _tuple12 _tmp591=_tmp590;_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;_tmp594=_tmp591.f3;{
void*tm;
if(_tmp576 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA64;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA63;tm=(void*)((_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63[0]=((_tmpA64.tag=0,((_tmpA64.f1=_tmp577,((_tmpA64.f2=_tmp578,_tmpA64)))))),_tmpA63))));}else{
# 2011
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA67;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA66;tm=(void*)((_tmpA66=_region_malloc(r,sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA67.tag=1,((_tmpA67.f1=_tmp576,((_tmpA67.f2=_tmp577,((_tmpA67.f3=_tmp578,_tmpA67)))))))),_tmpA66))));}{
struct Cyc_List_List*_tmpA6A;struct _tuple12 _tmpA69;return(_tmpA69.f1=_tmp592,((_tmpA69.f2=_tmp593,((_tmpA69.f3=((_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A->hd=tm,((_tmpA6A->tl=_tmp594,_tmpA6A)))))),_tmpA69)))));};};}_LL2E0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp579=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp572;if(_tmp579->tag != 5)goto _LL2E2;else{_tmp57A=(_tmp579->f1).elt_typ;_tmp57B=(_tmp579->f1).elt_tq;_tmp57C=(_tmp579->f1).ptr_atts;}}_LL2E1: {
# 2015
struct _tuple12 _tmp59B=Cyc_Absynpp_to_tms(r,_tmp57B,_tmp57A);struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59E;struct Cyc_List_List*_tmp59F;struct _tuple12 _tmp59C=_tmp59B;_tmp59D=_tmp59C.f1;_tmp59E=_tmp59C.f2;_tmp59F=_tmp59C.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA70;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA6F;struct Cyc_List_List*_tmpA6E;_tmp59F=((_tmpA6E=_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E->hd=(void*)((_tmpA70=_region_malloc(r,sizeof(*_tmpA70)),((_tmpA70[0]=((_tmpA6F.tag=2,((_tmpA6F.f1=_tmp57C,((_tmpA6F.f2=tq,_tmpA6F)))))),_tmpA70)))),((_tmpA6E->tl=_tmp59F,_tmpA6E))))));}{
struct _tuple12 _tmpA71;return(_tmpA71.f1=_tmp59D,((_tmpA71.f2=_tmp59E,((_tmpA71.f3=_tmp59F,_tmpA71)))));};}_LL2E2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp57D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp572;if(_tmp57D->tag != 9)goto _LL2E4;else{_tmp57E=(_tmp57D->f1).tvars;_tmp57F=(_tmp57D->f1).effect;_tmp580=(_tmp57D->f1).ret_tqual;_tmp581=(_tmp57D->f1).ret_typ;_tmp582=(_tmp57D->f1).args;_tmp583=(_tmp57D->f1).c_varargs;_tmp584=(_tmp57D->f1).cyc_varargs;_tmp585=(_tmp57D->f1).rgn_po;_tmp586=(_tmp57D->f1).attributes;}}_LL2E3:
# 2021
 if(!Cyc_Absynpp_print_all_tvars){
# 2025
if(_tmp57F == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp57F)){
_tmp57F=0;
_tmp57E=0;}}else{
# 2030
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2033
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp57E);}{
# 2038
struct _tuple12 _tmp5A4=Cyc_Absynpp_to_tms(r,_tmp580,_tmp581);struct Cyc_Absyn_Tqual _tmp5A6;void*_tmp5A7;struct Cyc_List_List*_tmp5A8;struct _tuple12 _tmp5A5=_tmp5A4;_tmp5A6=_tmp5A5.f1;_tmp5A7=_tmp5A5.f2;_tmp5A8=_tmp5A5.f3;{
struct Cyc_List_List*tms=_tmp5A8;
# 2049 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2EA:
# 2051
 if(_tmp586 != 0){
# 2054
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA74;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA73;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA73=_region_malloc(r,sizeof(*_tmpA73)),((_tmpA73[0]=((_tmpA74.tag=5,((_tmpA74.f1=0,((_tmpA74.f2=_tmp586,_tmpA74)))))),_tmpA73)))),tms);}
# 2056
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA83;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA82;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA81;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA80;struct Cyc_List_List*_tmpA7F;tms=((_tmpA7F=_region_malloc(r,sizeof(*_tmpA7F)),((_tmpA7F->hd=(void*)((_tmpA83=_region_malloc(r,sizeof(*_tmpA83)),((_tmpA83[0]=(
(_tmpA80.tag=3,((_tmpA80.f1=(void*)((_tmpA82=_region_malloc(r,sizeof(*_tmpA82)),((_tmpA82[0]=((_tmpA81.tag=1,((_tmpA81.f1=_tmp582,((_tmpA81.f2=_tmp583,((_tmpA81.f3=_tmp584,((_tmpA81.f4=_tmp57F,((_tmpA81.f5=_tmp585,_tmpA81)))))))))))),_tmpA82)))),_tmpA80)))),_tmpA83)))),((_tmpA7F->tl=tms,_tmpA7F))))));}
# 2060
break;case Cyc_Cyclone_Vc_c: _LL2EB:
# 2062
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA92;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA91;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA90;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA8F;struct Cyc_List_List*_tmpA8E;tms=((_tmpA8E=_region_malloc(r,sizeof(*_tmpA8E)),((_tmpA8E->hd=(void*)((_tmpA92=_region_malloc(r,sizeof(*_tmpA92)),((_tmpA92[0]=(
(_tmpA8F.tag=3,((_tmpA8F.f1=(void*)((_tmpA91=_region_malloc(r,sizeof(*_tmpA91)),((_tmpA91[0]=((_tmpA90.tag=1,((_tmpA90.f1=_tmp582,((_tmpA90.f2=_tmp583,((_tmpA90.f3=_tmp584,((_tmpA90.f4=_tmp57F,((_tmpA90.f5=_tmp585,_tmpA90)))))))))))),_tmpA91)))),_tmpA8F)))),_tmpA92)))),((_tmpA8E->tl=tms,_tmpA8E))))));}
# 2066
for(0;_tmp586 != 0;_tmp586=_tmp586->tl){
void*_tmp5B5=(void*)_tmp586->hd;void*_tmp5B6=_tmp5B5;_LL2EE: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp5B7=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp5B6;if(_tmp5B7->tag != 1)goto _LL2F0;}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp5B8=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp5B6;if(_tmp5B8->tag != 2)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp5B9=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp5B6;if(_tmp5B9->tag != 3)goto _LL2F4;}_LL2F3:
# 2071
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA9C;struct Cyc_List_List*_tmpA9B;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA9A;struct Cyc_List_List*_tmpA99;tms=((_tmpA99=_region_malloc(r,sizeof(*_tmpA99)),((_tmpA99->hd=(void*)((_tmpA9C=_region_malloc(r,sizeof(*_tmpA9C)),((_tmpA9C[0]=((_tmpA9A.tag=5,((_tmpA9A.f1=0,((_tmpA9A.f2=((_tmpA9B=_cycalloc(sizeof(*_tmpA9B)),((_tmpA9B->hd=(void*)_tmp586->hd,((_tmpA9B->tl=0,_tmpA9B)))))),_tmpA9A)))))),_tmpA9C)))),((_tmpA99->tl=tms,_tmpA99))))));}
goto AfterAtts;_LL2F4:;_LL2F5:
 goto _LL2ED;_LL2ED:;}
# 2075
break;}
# 2078
AfterAtts:
 if(_tmp57E != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpAA2;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpAA1;struct Cyc_List_List*_tmpAA0;tms=((_tmpAA0=_region_malloc(r,sizeof(*_tmpAA0)),((_tmpAA0->hd=(void*)((_tmpAA2=_region_malloc(r,sizeof(*_tmpAA2)),((_tmpAA2[0]=((_tmpAA1.tag=4,((_tmpAA1.f1=_tmp57E,((_tmpAA1.f2=0,((_tmpAA1.f3=1,_tmpAA1)))))))),_tmpAA2)))),((_tmpAA0->tl=tms,_tmpAA0))))));}{
struct _tuple12 _tmpAA3;return(_tmpAA3.f1=_tmp5A6,((_tmpAA3.f2=_tmp5A7,((_tmpAA3.f3=tms,_tmpAA3)))));};};};_LL2E4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp587=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp572;if(_tmp587->tag != 1)goto _LL2E6;else{_tmp588=_tmp587->f1;_tmp589=(void*)_tmp587->f2;_tmp58A=_tmp587->f3;}}_LL2E5:
# 2084
 if(_tmp589 == 0){
struct _tuple12 _tmpAA4;return(_tmpAA4.f1=tq,((_tmpAA4.f2=t,((_tmpAA4.f3=0,_tmpAA4)))));}else{
# 2087
return Cyc_Absynpp_to_tms(r,tq,_tmp589);}_LL2E6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp58B=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp572;if(_tmp58B->tag != 17)goto _LL2E8;else{_tmp58C=_tmp58B->f1;_tmp58D=_tmp58B->f2;_tmp58E=_tmp58B->f3;_tmp58F=(void*)_tmp58B->f4;}}_LL2E7:
# 2092
 if((_tmp58F == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp58F)){
struct _tuple12 _tmpAA5;return(_tmpAA5.f1=tq,((_tmpAA5.f2=t,((_tmpAA5.f3=0,_tmpAA5)))));}else{
# 2095
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp58F);}_LL2E8:;_LL2E9: {
# 2100
struct _tuple12 _tmpAA6;return(_tmpAA6.f1=tq,((_tmpAA6.f2=t,((_tmpAA6.f3=0,_tmpAA6)))));}_LL2DD:;}
# 2104
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2106
void*_tmp5C5=t;void*_tmp5C7;void*_tmp5C9;_LL2F7:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5C6=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5C5;if(_tmp5C6->tag != 1)goto _LL2F9;else{_tmp5C7=(void*)_tmp5C6->f2;}}if(!(_tmp5C7 != 0))goto _LL2F9;_LL2F8:
 return Cyc_Absynpp_is_char_ptr(_tmp5C7);_LL2F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5C8=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5;if(_tmp5C8->tag != 5)goto _LL2FB;else{_tmp5C9=(_tmp5C8->f1).elt_typ;}}_LL2FA:
# 2109
 L: {
void*_tmp5CA=_tmp5C9;void*_tmp5CC;void*_tmp5CE;_LL2FE:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5CB=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5CA;if(_tmp5CB->tag != 1)goto _LL300;else{_tmp5CC=(void*)_tmp5CB->f2;}}if(!(_tmp5CC != 0))goto _LL300;_LL2FF:
 _tmp5C9=_tmp5CC;goto L;_LL300:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5CD=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CD->tag != 17)goto _LL302;else{_tmp5CE=(void*)_tmp5CD->f4;}}if(!(_tmp5CE != 0))goto _LL302;_LL301:
 _tmp5C9=_tmp5CE;goto L;_LL302: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5CF=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5CA;if(_tmp5CF->tag != 6)goto _LL304;else{if(_tmp5CF->f2 != Cyc_Absyn_Char_sz)goto _LL304;}}_LL303:
 return 1;_LL304:;_LL305:
 return 0;_LL2FD:;}_LL2FB:;_LL2FC:
# 2116
 return 0;_LL2F6:;}
# 2120
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5D0=_new_region("temp");struct _RegionHandle*temp=& _tmp5D0;_push_region(temp);
{struct _tuple12 _tmp5D1=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5D3;void*_tmp5D4;struct Cyc_List_List*_tmp5D5;struct _tuple12 _tmp5D2=_tmp5D1;_tmp5D3=_tmp5D2.f1;_tmp5D4=_tmp5D2.f2;_tmp5D5=_tmp5D2.f3;
_tmp5D5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5D5);
if(_tmp5D5 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpAA7[2];struct Cyc_PP_Doc*_tmp5D7=(_tmpAA7[1]=Cyc_Absynpp_ntyp2doc(_tmp5D4),((_tmpAA7[0]=Cyc_Absynpp_tqual2doc(_tmp5D3),Cyc_PP_cat(_tag_dyneither(_tmpAA7,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5D7;}else{
# 2127
const char*_tmpAAA;struct Cyc_PP_Doc*_tmpAA9[4];struct Cyc_PP_Doc*_tmp5DA=
(_tmpAA9[3]=
# 2131
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5D5),((_tmpAA9[2]=
# 2130
Cyc_PP_text(((_tmpAAA=" ",_tag_dyneither(_tmpAAA,sizeof(char),2)))),((_tmpAA9[1]=
# 2129
Cyc_Absynpp_ntyp2doc(_tmp5D4),((_tmpAA9[0]=
# 2128
Cyc_Absynpp_tqual2doc(_tmp5D3),Cyc_PP_cat(_tag_dyneither(_tmpAA9,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5DA;}}
# 2122
;_pop_region(temp);}
# 2135
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp5DB=f->requires_clause;
if((unsigned int)_tmp5DB){
const char*_tmpAAD;struct Cyc_PP_Doc*_tmpAAC[2];requires_doc=((_tmpAAC[1]=Cyc_Absynpp_exp2doc(_tmp5DB),((_tmpAAC[0]=Cyc_PP_text(((_tmpAAD="@requires ",_tag_dyneither(_tmpAAD,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpAAC,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2141
requires_doc=Cyc_PP_nil_doc();}
# 2143
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL306:
# 2146
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAB4;const char*_tmpAB3;const char*_tmpAB2;struct Cyc_PP_Doc*_tmpAB1[5];return(_tmpAB1[4]=
# 2149
Cyc_PP_text(((_tmpAB2=";",_tag_dyneither(_tmpAB2,sizeof(char),2)))),((_tmpAB1[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAB1[2]=
# 2148
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpAB1[1]=Cyc_PP_text(((_tmpAB3=":",_tag_dyneither(_tmpAB3,sizeof(char),2)))),((_tmpAB1[0]=
# 2147
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAB4=_cycalloc(sizeof(*_tmpAB4)),((_tmpAB4->v=Cyc_PP_textptr(f->name),_tmpAB4))))),Cyc_PP_cat(_tag_dyneither(_tmpAB1,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2151
struct Cyc_Core_Opt*_tmpAB9;const char*_tmpAB8;struct Cyc_PP_Doc*_tmpAB7[4];return(_tmpAB7[3]=
Cyc_PP_text(((_tmpAB8=";",_tag_dyneither(_tmpAB8,sizeof(char),2)))),((_tmpAB7[2]=requires_doc,((_tmpAB7[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpAB7[0]=
# 2151
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAB9=_cycalloc(sizeof(*_tmpAB9)),((_tmpAB9->v=Cyc_PP_textptr(f->name),_tmpAB9))))),Cyc_PP_cat(_tag_dyneither(_tmpAB7,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL307:
# 2154
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAC0;const char*_tmpABF;const char*_tmpABE;struct Cyc_PP_Doc*_tmpABD[5];return(_tmpABD[4]=
# 2157
Cyc_PP_text(((_tmpABE=";",_tag_dyneither(_tmpABE,sizeof(char),2)))),((_tmpABD[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpABD[2]=Cyc_PP_text(((_tmpABF=":",_tag_dyneither(_tmpABF,sizeof(char),2)))),((_tmpABD[1]=
# 2156
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAC0=_cycalloc(sizeof(*_tmpAC0)),((_tmpAC0->v=Cyc_PP_textptr(f->name),_tmpAC0))))),((_tmpABD[0]=
# 2155
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpABD,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2159
struct Cyc_Core_Opt*_tmpAC5;const char*_tmpAC4;struct Cyc_PP_Doc*_tmpAC3[4];return(_tmpAC3[3]=
# 2161
Cyc_PP_text(((_tmpAC4=";",_tag_dyneither(_tmpAC4,sizeof(char),2)))),((_tmpAC3[2]=requires_doc,((_tmpAC3[1]=
# 2160
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAC5=_cycalloc(sizeof(*_tmpAC5)),((_tmpAC5->v=Cyc_PP_textptr(f->name),_tmpAC5))))),((_tmpAC3[0]=
# 2159
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAC3,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2166
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAC6;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAC6="",_tag_dyneither(_tmpAC6,sizeof(char),1))),fields);}
# 2170
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAC7[3];return(_tmpAC7[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAC7[1]=
# 2171
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAC7[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAC7,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2175
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAC8;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAC8=",",_tag_dyneither(_tmpAC8,sizeof(char),2))),fields);}
# 2184
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpACB;void*_tmpACA;(_tmpACA=0,Cyc_fprintf(f,((_tmpACB="\n",_tag_dyneither(_tmpACB,sizeof(char),2))),_tag_dyneither(_tmpACA,sizeof(void*),0)));};}}
# 2191
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpACC;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpACC="",_tag_dyneither(_tmpACC,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2194
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2198
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
# 2209
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
