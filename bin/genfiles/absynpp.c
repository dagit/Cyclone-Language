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
# 320 "tcutil.h"
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 322
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
struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r={0,0,0,0,0,1,0,0,0,1,1,0,1,0,0,0};
# 247
static void Cyc_Absynpp_curr_namespace_add(struct _dyneither_ptr*v){
struct Cyc_List_List*_tmp5CE;Cyc_Absynpp_curr_namespace=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(Cyc_Absynpp_curr_namespace,((_tmp5CE=_cycalloc(sizeof(*_tmp5CE)),((_tmp5CE->hd=v,((_tmp5CE->tl=0,_tmp5CE)))))));}
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
const char*_tmp5CF;return(_tmp5CF="\\a",_tag_dyneither(_tmp5CF,sizeof(char),3));}case '\b': _LL1: {
const char*_tmp5D0;return(_tmp5D0="\\b",_tag_dyneither(_tmp5D0,sizeof(char),3));}case '\f': _LL2: {
const char*_tmp5D1;return(_tmp5D1="\\f",_tag_dyneither(_tmp5D1,sizeof(char),3));}case '\n': _LL3: {
const char*_tmp5D2;return(_tmp5D2="\\n",_tag_dyneither(_tmp5D2,sizeof(char),3));}case '\r': _LL4: {
const char*_tmp5D3;return(_tmp5D3="\\r",_tag_dyneither(_tmp5D3,sizeof(char),3));}case '\t': _LL5: {
const char*_tmp5D4;return(_tmp5D4="\\t",_tag_dyneither(_tmp5D4,sizeof(char),3));}case '\v': _LL6: {
const char*_tmp5D5;return(_tmp5D5="\\v",_tag_dyneither(_tmp5D5,sizeof(char),3));}case '\\': _LL7: {
const char*_tmp5D6;return(_tmp5D6="\\\\",_tag_dyneither(_tmp5D6,sizeof(char),3));}case '"': _LL8: {
const char*_tmp5D7;return(_tmp5D7="\"",_tag_dyneither(_tmp5D7,sizeof(char),2));}case '\'': _LL9: {
const char*_tmp5D8;return(_tmp5D8="\\'",_tag_dyneither(_tmp5D8,sizeof(char),3));}default: _LLA:
# 276
 if(c >= ' '  && c <= '~'){
struct _dyneither_ptr _tmpC=Cyc_Core_new_string(2);
{char _tmp5DB;char _tmp5DA;struct _dyneither_ptr _tmp5D9;(_tmp5D9=_dyneither_ptr_plus(_tmpC,sizeof(char),0),((_tmp5DA=*((char*)_check_dyneither_subscript(_tmp5D9,sizeof(char),0)),((_tmp5DB=c,((_get_dyneither_size(_tmp5D9,sizeof(char))== 1  && (_tmp5DA == '\000'  && _tmp5DB != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5D9.curr)=_tmp5DB)))))));}
return(struct _dyneither_ptr)_tmpC;}else{
# 281
struct _dyneither_ptr _tmp10=Cyc_Core_new_string(5);
int j=0;
{char _tmp5DE;char _tmp5DD;struct _dyneither_ptr _tmp5DC;(_tmp5DC=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5DD=*((char*)_check_dyneither_subscript(_tmp5DC,sizeof(char),0)),((_tmp5DE='\\',((_get_dyneither_size(_tmp5DC,sizeof(char))== 1  && (_tmp5DD == '\000'  && _tmp5DE != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DC.curr)=_tmp5DE)))))));}
{char _tmp5E1;char _tmp5E0;struct _dyneither_ptr _tmp5DF;(_tmp5DF=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E0=*((char*)_check_dyneither_subscript(_tmp5DF,sizeof(char),0)),((_tmp5E1=(char)('0' + ((unsigned char)c >> 6 & 3)),((_get_dyneither_size(_tmp5DF,sizeof(char))== 1  && (_tmp5E0 == '\000'  && _tmp5E1 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5DF.curr)=_tmp5E1)))))));}
{char _tmp5E4;char _tmp5E3;struct _dyneither_ptr _tmp5E2;(_tmp5E2=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E3=*((char*)_check_dyneither_subscript(_tmp5E2,sizeof(char),0)),((_tmp5E4=(char)('0' + (c >> 3 & 7)),((_get_dyneither_size(_tmp5E2,sizeof(char))== 1  && (_tmp5E3 == '\000'  && _tmp5E4 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E2.curr)=_tmp5E4)))))));}
{char _tmp5E7;char _tmp5E6;struct _dyneither_ptr _tmp5E5;(_tmp5E5=_dyneither_ptr_plus(_tmp10,sizeof(char),j ++),((_tmp5E6=*((char*)_check_dyneither_subscript(_tmp5E5,sizeof(char),0)),((_tmp5E7=(char)('0' + (c & 7)),((_get_dyneither_size(_tmp5E5,sizeof(char))== 1  && (_tmp5E6 == '\000'  && _tmp5E7 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E5.curr)=_tmp5E7)))))));}
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
{char _tmp5EA;char _tmp5E9;struct _dyneither_ptr _tmp5E8;(_tmp5E8=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5E9=*((char*)_check_dyneither_subscript(_tmp5E8,sizeof(char),0)),((_tmp5EA='\\',((_get_dyneither_size(_tmp5E8,sizeof(char))== 1  && (_tmp5E9 == '\000'  && _tmp5EA != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5E8.curr)=_tmp5EA)))))));}{char _tmp5ED;char _tmp5EC;struct _dyneither_ptr _tmp5EB;(_tmp5EB=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EC=*((char*)_check_dyneither_subscript(_tmp5EB,sizeof(char),0)),((_tmp5ED='a',((_get_dyneither_size(_tmp5EB,sizeof(char))== 1  && (_tmp5EC == '\000'  && _tmp5ED != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EB.curr)=_tmp5ED)))))));}goto _LL21;_LL24: if(_tmp1F != '\b')goto _LL26;_LL25:
{char _tmp5F0;char _tmp5EF;struct _dyneither_ptr _tmp5EE;(_tmp5EE=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5EF=*((char*)_check_dyneither_subscript(_tmp5EE,sizeof(char),0)),((_tmp5F0='\\',((_get_dyneither_size(_tmp5EE,sizeof(char))== 1  && (_tmp5EF == '\000'  && _tmp5F0 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5EE.curr)=_tmp5F0)))))));}{char _tmp5F3;char _tmp5F2;struct _dyneither_ptr _tmp5F1;(_tmp5F1=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F2=*((char*)_check_dyneither_subscript(_tmp5F1,sizeof(char),0)),((_tmp5F3='b',((_get_dyneither_size(_tmp5F1,sizeof(char))== 1  && (_tmp5F2 == '\000'  && _tmp5F3 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F1.curr)=_tmp5F3)))))));}goto _LL21;_LL26: if(_tmp1F != '\f')goto _LL28;_LL27:
{char _tmp5F6;char _tmp5F5;struct _dyneither_ptr _tmp5F4;(_tmp5F4=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F5=*((char*)_check_dyneither_subscript(_tmp5F4,sizeof(char),0)),((_tmp5F6='\\',((_get_dyneither_size(_tmp5F4,sizeof(char))== 1  && (_tmp5F5 == '\000'  && _tmp5F6 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F4.curr)=_tmp5F6)))))));}{char _tmp5F9;char _tmp5F8;struct _dyneither_ptr _tmp5F7;(_tmp5F7=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5F8=*((char*)_check_dyneither_subscript(_tmp5F7,sizeof(char),0)),((_tmp5F9='f',((_get_dyneither_size(_tmp5F7,sizeof(char))== 1  && (_tmp5F8 == '\000'  && _tmp5F9 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5F7.curr)=_tmp5F9)))))));}goto _LL21;_LL28: if(_tmp1F != '\n')goto _LL2A;_LL29:
{char _tmp5FC;char _tmp5FB;struct _dyneither_ptr _tmp5FA;(_tmp5FA=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FB=*((char*)_check_dyneither_subscript(_tmp5FA,sizeof(char),0)),((_tmp5FC='\\',((_get_dyneither_size(_tmp5FA,sizeof(char))== 1  && (_tmp5FB == '\000'  && _tmp5FC != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FA.curr)=_tmp5FC)))))));}{char _tmp5FF;char _tmp5FE;struct _dyneither_ptr _tmp5FD;(_tmp5FD=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp5FE=*((char*)_check_dyneither_subscript(_tmp5FD,sizeof(char),0)),((_tmp5FF='n',((_get_dyneither_size(_tmp5FD,sizeof(char))== 1  && (_tmp5FE == '\000'  && _tmp5FF != '\000')?_throw_arraybounds(): 1,*((char*)_tmp5FD.curr)=_tmp5FF)))))));}goto _LL21;_LL2A: if(_tmp1F != '\r')goto _LL2C;_LL2B:
{char _tmp602;char _tmp601;struct _dyneither_ptr _tmp600;(_tmp600=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp601=*((char*)_check_dyneither_subscript(_tmp600,sizeof(char),0)),((_tmp602='\\',((_get_dyneither_size(_tmp600,sizeof(char))== 1  && (_tmp601 == '\000'  && _tmp602 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp600.curr)=_tmp602)))))));}{char _tmp605;char _tmp604;struct _dyneither_ptr _tmp603;(_tmp603=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp604=*((char*)_check_dyneither_subscript(_tmp603,sizeof(char),0)),((_tmp605='r',((_get_dyneither_size(_tmp603,sizeof(char))== 1  && (_tmp604 == '\000'  && _tmp605 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp603.curr)=_tmp605)))))));}goto _LL21;_LL2C: if(_tmp1F != '\t')goto _LL2E;_LL2D:
{char _tmp608;char _tmp607;struct _dyneither_ptr _tmp606;(_tmp606=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp607=*((char*)_check_dyneither_subscript(_tmp606,sizeof(char),0)),((_tmp608='\\',((_get_dyneither_size(_tmp606,sizeof(char))== 1  && (_tmp607 == '\000'  && _tmp608 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp606.curr)=_tmp608)))))));}{char _tmp60B;char _tmp60A;struct _dyneither_ptr _tmp609;(_tmp609=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60A=*((char*)_check_dyneither_subscript(_tmp609,sizeof(char),0)),((_tmp60B='t',((_get_dyneither_size(_tmp609,sizeof(char))== 1  && (_tmp60A == '\000'  && _tmp60B != '\000')?_throw_arraybounds(): 1,*((char*)_tmp609.curr)=_tmp60B)))))));}goto _LL21;_LL2E: if(_tmp1F != '\v')goto _LL30;_LL2F:
{char _tmp60E;char _tmp60D;struct _dyneither_ptr _tmp60C;(_tmp60C=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp60D=*((char*)_check_dyneither_subscript(_tmp60C,sizeof(char),0)),((_tmp60E='\\',((_get_dyneither_size(_tmp60C,sizeof(char))== 1  && (_tmp60D == '\000'  && _tmp60E != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60C.curr)=_tmp60E)))))));}{char _tmp611;char _tmp610;struct _dyneither_ptr _tmp60F;(_tmp60F=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp610=*((char*)_check_dyneither_subscript(_tmp60F,sizeof(char),0)),((_tmp611='v',((_get_dyneither_size(_tmp60F,sizeof(char))== 1  && (_tmp610 == '\000'  && _tmp611 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp60F.curr)=_tmp611)))))));}goto _LL21;_LL30: if(_tmp1F != '\\')goto _LL32;_LL31:
{char _tmp614;char _tmp613;struct _dyneither_ptr _tmp612;(_tmp612=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp613=*((char*)_check_dyneither_subscript(_tmp612,sizeof(char),0)),((_tmp614='\\',((_get_dyneither_size(_tmp612,sizeof(char))== 1  && (_tmp613 == '\000'  && _tmp614 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp612.curr)=_tmp614)))))));}{char _tmp617;char _tmp616;struct _dyneither_ptr _tmp615;(_tmp615=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp616=*((char*)_check_dyneither_subscript(_tmp615,sizeof(char),0)),((_tmp617='\\',((_get_dyneither_size(_tmp615,sizeof(char))== 1  && (_tmp616 == '\000'  && _tmp617 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp615.curr)=_tmp617)))))));}goto _LL21;_LL32: if(_tmp1F != '"')goto _LL34;_LL33:
{char _tmp61A;char _tmp619;struct _dyneither_ptr _tmp618;(_tmp618=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp619=*((char*)_check_dyneither_subscript(_tmp618,sizeof(char),0)),((_tmp61A='\\',((_get_dyneither_size(_tmp618,sizeof(char))== 1  && (_tmp619 == '\000'  && _tmp61A != '\000')?_throw_arraybounds(): 1,*((char*)_tmp618.curr)=_tmp61A)))))));}{char _tmp61D;char _tmp61C;struct _dyneither_ptr _tmp61B;(_tmp61B=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61C=*((char*)_check_dyneither_subscript(_tmp61B,sizeof(char),0)),((_tmp61D='"',((_get_dyneither_size(_tmp61B,sizeof(char))== 1  && (_tmp61C == '\000'  && _tmp61D != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61B.curr)=_tmp61D)))))));}goto _LL21;_LL34: _tmp20=_tmp1F;_LL35:
# 341
 if(_tmp20 >= ' '  && _tmp20 <= '~'){char _tmp620;char _tmp61F;struct _dyneither_ptr _tmp61E;(_tmp61E=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp61F=*((char*)_check_dyneither_subscript(_tmp61E,sizeof(char),0)),((_tmp620=_tmp20,((_get_dyneither_size(_tmp61E,sizeof(char))== 1  && (_tmp61F == '\000'  && _tmp620 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp61E.curr)=_tmp620)))))));}else{
# 343
{char _tmp623;char _tmp622;struct _dyneither_ptr _tmp621;(_tmp621=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp622=*((char*)_check_dyneither_subscript(_tmp621,sizeof(char),0)),((_tmp623='\\',((_get_dyneither_size(_tmp621,sizeof(char))== 1  && (_tmp622 == '\000'  && _tmp623 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp621.curr)=_tmp623)))))));}
{char _tmp626;char _tmp625;struct _dyneither_ptr _tmp624;(_tmp624=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp625=*((char*)_check_dyneither_subscript(_tmp624,sizeof(char),0)),((_tmp626=(char)('0' + (_tmp20 >> 6 & 7)),((_get_dyneither_size(_tmp624,sizeof(char))== 1  && (_tmp625 == '\000'  && _tmp626 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp624.curr)=_tmp626)))))));}
{char _tmp629;char _tmp628;struct _dyneither_ptr _tmp627;(_tmp627=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp628=*((char*)_check_dyneither_subscript(_tmp627,sizeof(char),0)),((_tmp629=(char)('0' + (_tmp20 >> 3 & 7)),((_get_dyneither_size(_tmp627,sizeof(char))== 1  && (_tmp628 == '\000'  && _tmp629 != '\000')?_throw_arraybounds(): 1,*((char*)_tmp627.curr)=_tmp629)))))));}{
char _tmp62C;char _tmp62B;struct _dyneither_ptr _tmp62A;(_tmp62A=_dyneither_ptr_plus(t,sizeof(char),j ++),((_tmp62B=*((char*)_check_dyneither_subscript(_tmp62A,sizeof(char),0)),((_tmp62C=(char)('0' + (_tmp20 & 7)),((_get_dyneither_size(_tmp62A,sizeof(char))== 1  && (_tmp62B == '\000'  && _tmp62C != '\000')?_throw_arraybounds(): 1,*((char*)_tmp62A.curr)=_tmp62C)))))));};}
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
if(tq.q_restrict){struct Cyc_List_List*_tmp62D;l=((_tmp62D=_cycalloc(sizeof(*_tmp62D)),((_tmp62D->hd=Cyc_Absynpp_restrict_sp,((_tmp62D->tl=l,_tmp62D))))));}
if(tq.q_volatile){struct Cyc_List_List*_tmp62E;l=((_tmp62E=_cycalloc(sizeof(*_tmp62E)),((_tmp62E->hd=Cyc_Absynpp_volatile_sp,((_tmp62E->tl=l,_tmp62E))))));}
if(tq.print_const){struct Cyc_List_List*_tmp62F;l=((_tmp62F=_cycalloc(sizeof(*_tmp62F)),((_tmp62F->hd=Cyc_Absynpp_const_sp,((_tmp62F->tl=l,_tmp62F))))));}{
const char*_tmp632;const char*_tmp631;const char*_tmp630;return Cyc_PP_egroup(((_tmp630="",_tag_dyneither(_tmp630,sizeof(char),1))),((_tmp631=" ",_tag_dyneither(_tmp631,sizeof(char),2))),((_tmp632=" ",_tag_dyneither(_tmp632,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,l));};}
# 369
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*ka){
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=_tmp6F->kind;_tmp71=_tmp6F->aliasqual;
switch(_tmp70){case Cyc_Absyn_AnyKind: _LL36:
# 373
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL38: {
const char*_tmp633;return(_tmp633="A",_tag_dyneither(_tmp633,sizeof(char),2));}case Cyc_Absyn_Unique: _LL39: {
const char*_tmp634;return(_tmp634="UA",_tag_dyneither(_tmp634,sizeof(char),3));}case Cyc_Absyn_Top: _LL3A: {
const char*_tmp635;return(_tmp635="TA",_tag_dyneither(_tmp635,sizeof(char),3));}}case Cyc_Absyn_MemKind: _LL37:
# 379
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL3D: {
const char*_tmp636;return(_tmp636="M",_tag_dyneither(_tmp636,sizeof(char),2));}case Cyc_Absyn_Unique: _LL3E: {
const char*_tmp637;return(_tmp637="UM",_tag_dyneither(_tmp637,sizeof(char),3));}case Cyc_Absyn_Top: _LL3F: {
const char*_tmp638;return(_tmp638="TM",_tag_dyneither(_tmp638,sizeof(char),3));}}case Cyc_Absyn_BoxKind: _LL3C:
# 385
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL42: {
const char*_tmp639;return(_tmp639="B",_tag_dyneither(_tmp639,sizeof(char),2));}case Cyc_Absyn_Unique: _LL43: {
const char*_tmp63A;return(_tmp63A="UB",_tag_dyneither(_tmp63A,sizeof(char),3));}case Cyc_Absyn_Top: _LL44: {
const char*_tmp63B;return(_tmp63B="TB",_tag_dyneither(_tmp63B,sizeof(char),3));}}case Cyc_Absyn_RgnKind: _LL41:
# 391
 switch(_tmp71){case Cyc_Absyn_Aliasable: _LL47: {
const char*_tmp63C;return(_tmp63C="R",_tag_dyneither(_tmp63C,sizeof(char),2));}case Cyc_Absyn_Unique: _LL48: {
const char*_tmp63D;return(_tmp63D="UR",_tag_dyneither(_tmp63D,sizeof(char),3));}case Cyc_Absyn_Top: _LL49: {
const char*_tmp63E;return(_tmp63E="TR",_tag_dyneither(_tmp63E,sizeof(char),3));}}case Cyc_Absyn_EffKind: _LL46: {
# 396
const char*_tmp63F;return(_tmp63F="E",_tag_dyneither(_tmp63F,sizeof(char),2));}case Cyc_Absyn_IntKind: _LL4B: {
const char*_tmp640;return(_tmp640="I",_tag_dyneither(_tmp640,sizeof(char),2));}}}
# 400
struct Cyc_PP_Doc*Cyc_Absynpp_kind2doc(struct Cyc_Absyn_Kind*k){return Cyc_PP_text(Cyc_Absynpp_kind2string(k));}
# 402
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*c){
void*_stmttmp2=Cyc_Absyn_compress_kb(c);void*_tmp80=_stmttmp2;struct Cyc_Absyn_Kind*_tmp82;struct Cyc_Absyn_Kind*_tmp85;_LL4E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp81=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp81->tag != 0)goto _LL50;else{_tmp82=_tmp81->f1;}}_LL4F:
 return Cyc_Absynpp_kind2string(_tmp82);_LL50: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp83->tag != 1)goto _LL52;}_LL51:
# 406
 if(Cyc_PP_tex_output){
const char*_tmp641;return(_tmp641="{?}",_tag_dyneither(_tmp641,sizeof(char),4));}else{
const char*_tmp642;return(_tmp642="?",_tag_dyneither(_tmp642,sizeof(char),2));}_LL52: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp80;if(_tmp84->tag != 2)goto _LL4D;else{_tmp85=_tmp84->f2;}}_LL53: {
const char*_tmp646;void*_tmp645[1];struct Cyc_String_pa_PrintArg_struct _tmp644;return(struct _dyneither_ptr)((_tmp644.tag=0,((_tmp644.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp85)),((_tmp645[0]=& _tmp644,Cyc_aprintf(((_tmp646="<=%s",_tag_dyneither(_tmp646,sizeof(char),5))),_tag_dyneither(_tmp645,sizeof(void*),1))))))));}_LL4D:;}
# 413
struct Cyc_PP_Doc*Cyc_Absynpp_kindbound2doc(void*c){
void*_stmttmp3=Cyc_Absyn_compress_kb(c);void*_tmp8B=_stmttmp3;struct Cyc_Absyn_Kind*_tmp8D;struct Cyc_Absyn_Kind*_tmp90;_LL55: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp8C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8C->tag != 0)goto _LL57;else{_tmp8D=_tmp8C->f1;}}_LL56:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp8D));_LL57: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp8E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8E->tag != 1)goto _LL59;}_LL58:
# 417
 if(Cyc_PP_tex_output){
const char*_tmp647;return Cyc_PP_text_width(((_tmp647="{?}",_tag_dyneither(_tmp647,sizeof(char),4))),1);}else{
const char*_tmp648;return Cyc_PP_text(((_tmp648="?",_tag_dyneither(_tmp648,sizeof(char),2))));}_LL59: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8F=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8B;if(_tmp8F->tag != 2)goto _LL54;else{_tmp90=_tmp8F->f2;}}_LL5A:
 return Cyc_PP_text(Cyc_Absynpp_kind2string(_tmp90));_LL54:;}
# 424
struct Cyc_PP_Doc*Cyc_Absynpp_tps2doc(struct Cyc_List_List*ts){
const char*_tmp64B;const char*_tmp64A;const char*_tmp649;return Cyc_PP_egroup(((_tmp649="<",_tag_dyneither(_tmp649,sizeof(char),2))),((_tmp64A=">",_tag_dyneither(_tmp64A,sizeof(char),2))),((_tmp64B=",",_tag_dyneither(_tmp64B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_typ2doc,ts));}
# 428
struct Cyc_PP_Doc*Cyc_Absynpp_tvar2doc(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr _tmp96=*tv->name;
# 432
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(tv)){
# 434
const char*_tmp64C;struct _dyneither_ptr kstring=(_tmp64C="K",_tag_dyneither(_tmp64C,sizeof(char),2));
{void*_stmttmp4=Cyc_Absyn_compress_kb(tv->kind);void*_tmp97=_stmttmp4;struct Cyc_Absyn_Kind*_tmp99;struct Cyc_Absyn_Kind*_tmp9B;_LL5C: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp98=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp98->tag != 2)goto _LL5E;else{_tmp99=_tmp98->f2;}}_LL5D:
 _tmp9B=_tmp99;goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp9A->tag != 0)goto _LL60;else{_tmp9B=_tmp9A->f1;}}_LL5F:
 kstring=Cyc_Absynpp_kind2string(_tmp9B);goto _LL5B;_LL60:;_LL61:
 goto _LL5B;_LL5B:;}{
# 440
const char*_tmp651;void*_tmp650[2];struct Cyc_String_pa_PrintArg_struct _tmp64F;struct Cyc_String_pa_PrintArg_struct _tmp64E;return Cyc_PP_text((struct _dyneither_ptr)((_tmp64E.tag=0,((_tmp64E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_dyneither_ptr_plus(_tmp96,sizeof(char),1)),((_tmp64F.tag=0,((_tmp64F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kstring),((_tmp650[0]=& _tmp64F,((_tmp650[1]=& _tmp64E,Cyc_aprintf(((_tmp651="`G%s%s",_tag_dyneither(_tmp651,sizeof(char),7))),_tag_dyneither(_tmp650,sizeof(void*),2)))))))))))))));};}
# 442
return Cyc_PP_text(_tmp96);}
# 445
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_stmttmp5=Cyc_Absyn_compress_kb(tv->kind);void*_tmpA1=_stmttmp5;struct Cyc_Absyn_Kind*_tmpA5;struct Cyc_Absyn_Kind*_tmpA7;_LL63: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmpA2=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpA1;if(_tmpA2->tag != 1)goto _LL65;}_LL64:
 goto _LL66;_LL65: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA3=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA1;if(_tmpA3->tag != 0)goto _LL67;else{if((_tmpA3->f1)->kind != Cyc_Absyn_BoxKind)goto _LL67;if((_tmpA3->f1)->aliasqual != Cyc_Absyn_Aliasable)goto _LL67;}}_LL66:
 return Cyc_Absynpp_tvar2doc(tv);_LL67: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA4=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA1;if(_tmpA4->tag != 2)goto _LL69;else{_tmpA5=_tmpA4->f2;}}_LL68:
 _tmpA7=_tmpA5;goto _LL6A;_LL69: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmpA6=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA1;if(_tmpA6->tag != 0)goto _LL62;else{_tmpA7=_tmpA6->f1;}}_LL6A: {
const char*_tmp654;struct Cyc_PP_Doc*_tmp653[3];return(_tmp653[2]=Cyc_Absynpp_kind2doc(_tmpA7),((_tmp653[1]=Cyc_PP_text(((_tmp654="::",_tag_dyneither(_tmp654,sizeof(char),3)))),((_tmp653[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp653,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL62:;}
# 454
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp657;const char*_tmp656;const char*_tmp655;return Cyc_PP_egroup(((_tmp655="<",_tag_dyneither(_tmp655,sizeof(char),2))),((_tmp656=">",_tag_dyneither(_tmp656,sizeof(char),2))),((_tmp657=",",_tag_dyneither(_tmp657,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 458
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp65A;const char*_tmp659;const char*_tmp658;return Cyc_PP_egroup(((_tmp658="<",_tag_dyneither(_tmp658,sizeof(char),2))),((_tmp659=">",_tag_dyneither(_tmp659,sizeof(char),2))),((_tmp65A=",",_tag_dyneither(_tmp65A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,tvs));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 468
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp65D;const char*_tmp65C;const char*_tmp65B;return Cyc_PP_group(((_tmp65B="(",_tag_dyneither(_tmp65B,sizeof(char),2))),((_tmp65C=")",_tag_dyneither(_tmp65C,sizeof(char),2))),((_tmp65D=",",_tag_dyneither(_tmp65D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 472
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpB3=att;_LL6C: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB4=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB3;if(_tmpB4->tag != 1)goto _LL6E;}_LL6D:
 goto _LL6F;_LL6E: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB5=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB3;if(_tmpB5->tag != 2)goto _LL70;}_LL6F:
 goto _LL71;_LL70: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB6=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB3;if(_tmpB6->tag != 3)goto _LL72;}_LL71:
 return Cyc_PP_nil_doc();_LL72:;_LL73:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL6B:;}
# 481
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_stmttmp6=(void*)atts->hd;void*_tmpB7=_stmttmp6;_LL75: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB8=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpB8->tag != 1)goto _LL77;}_LL76: {
const char*_tmp65E;return Cyc_PP_text(((_tmp65E=" _stdcall ",_tag_dyneither(_tmp65E,sizeof(char),11))));}_LL77: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB9=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpB9->tag != 2)goto _LL79;}_LL78: {
const char*_tmp65F;return Cyc_PP_text(((_tmp65F=" _cdecl ",_tag_dyneither(_tmp65F,sizeof(char),9))));}_LL79: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpBA=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB7;if(_tmpBA->tag != 3)goto _LL7B;}_LL7A: {
const char*_tmp660;return Cyc_PP_text(((_tmp660=" _fastcall ",_tag_dyneither(_tmp660,sizeof(char),12))));}_LL7B:;_LL7C:
 goto _LL74;_LL74:;}
# 489
return Cyc_PP_nil_doc();}
# 492
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_stmttmp7=(void*)atts2->hd;void*_tmpBE=_stmttmp7;_LL7E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpBF=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpBF->tag != 1)goto _LL80;}_LL7F:
 goto _LL81;_LL80: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpC0=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC0->tag != 2)goto _LL82;}_LL81:
 goto _LL83;_LL82: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpC1=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpBE;if(_tmpC1->tag != 3)goto _LL84;}_LL83:
 goto _LL7D;_LL84:;_LL85:
 hasatt=1;goto _LL7D;_LL7D:;}}
# 501
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp66B;const char*_tmp66A;const char*_tmp669;const char*_tmp668;const char*_tmp667;struct Cyc_PP_Doc*_tmp666[3];return(_tmp666[2]=
# 505
Cyc_PP_text(((_tmp667=")",_tag_dyneither(_tmp667,sizeof(char),2)))),((_tmp666[1]=
# 504
Cyc_PP_group(((_tmp66A="",_tag_dyneither(_tmp66A,sizeof(char),1))),((_tmp669="",_tag_dyneither(_tmp669,sizeof(char),1))),((_tmp668=" ",_tag_dyneither(_tmp668,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp666[0]=
# 503
Cyc_PP_text(((_tmp66B=" __declspec(",_tag_dyneither(_tmp66B,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp666,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 508
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 512
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL86:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL87: {
const char*_tmp674;const char*_tmp673;const char*_tmp672;const char*_tmp671;struct Cyc_PP_Doc*_tmp670[2];return(_tmp670[1]=
Cyc_PP_group(((_tmp673="((",_tag_dyneither(_tmp673,sizeof(char),3))),((_tmp672="))",_tag_dyneither(_tmp672,sizeof(char),3))),((_tmp671=",",_tag_dyneither(_tmp671,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp670[0]=
# 516
Cyc_PP_text(((_tmp674=" __attribute__",_tag_dyneither(_tmp674,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp670,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 521
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_stmttmp8=(void*)tms->hd;void*_tmpCD=_stmttmp8;_LL8A: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpCE=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCD;if(_tmpCE->tag != 2)goto _LL8C;}_LL8B:
 return 1;_LL8C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpCF=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCD;if(_tmpCF->tag != 5)goto _LL8E;}_LL8D:
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
const char*_tmp675;Cyc_Absynpp_cache_question=Cyc_PP_text_width(((_tmp675="{?}",_tag_dyneither(_tmp675,sizeof(char),4))),1);}else{
const char*_tmp676;Cyc_Absynpp_cache_question=Cyc_PP_text(((_tmp676="?",_tag_dyneither(_tmp676,sizeof(char),2))));}}
# 544
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 546
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp677;Cyc_Absynpp_cache_lb=Cyc_PP_text_width(((_tmp677="{\\lb}",_tag_dyneither(_tmp677,sizeof(char),6))),1);}else{
const char*_tmp678;Cyc_Absynpp_cache_lb=Cyc_PP_text(((_tmp678="{",_tag_dyneither(_tmp678,sizeof(char),2))));}}
# 553
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 555
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp679;Cyc_Absynpp_cache_rb=Cyc_PP_text_width(((_tmp679="{\\rb}",_tag_dyneither(_tmp679,sizeof(char),6))),1);}else{
const char*_tmp67A;Cyc_Absynpp_cache_rb=Cyc_PP_text(((_tmp67A="}",_tag_dyneither(_tmp67A,sizeof(char),2))));}}
# 562
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 564
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp67B;Cyc_Absynpp_cache_dollar=Cyc_PP_text_width(((_tmp67B="\\$",_tag_dyneither(_tmp67B,sizeof(char),3))),1);}else{
const char*_tmp67C;Cyc_Absynpp_cache_dollar=Cyc_PP_text(((_tmp67C="$",_tag_dyneither(_tmp67C,sizeof(char),2))));}}
# 571
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 573
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp67D[3];return(_tmp67D[2]=Cyc_Absynpp_rb(),((_tmp67D[1]=Cyc_PP_seq(sep,ss),((_tmp67D[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp67D,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 578
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_stmttmp9=(void*)tms->hd;void*_tmpD9=_stmttmp9;struct Cyc_List_List*_tmpDE;_LL94: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpDA=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpDA->tag != 0)goto _LL96;}_LL95:
{const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680="Carray_mod ",_tag_dyneither(_tmp680,sizeof(char),12))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}goto _LL93;_LL96: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpDB=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpDB->tag != 1)goto _LL98;}_LL97:
{const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683="ConstArray_mod ",_tag_dyneither(_tmp683,sizeof(char),16))),_tag_dyneither(_tmp682,sizeof(void*),0)));}goto _LL93;_LL98: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDC=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpDC->tag != 3)goto _LL9A;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpDD=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpDC->f1);if(_tmpDD->tag != 1)goto _LL9A;else{_tmpDE=_tmpDD->f1;}}}_LL99:
# 584
{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686="Function_mod(",_tag_dyneither(_tmp686,sizeof(char),14))),_tag_dyneither(_tmp685,sizeof(void*),0)));}
for(0;_tmpDE != 0;_tmpDE=_tmpDE->tl){
struct _dyneither_ptr*_tmpE9=(*((struct _tuple8*)_tmpDE->hd)).f1;
if(_tmpE9 == 0){const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="?",_tag_dyneither(_tmp689,sizeof(char),2))),_tag_dyneither(_tmp688,sizeof(void*),0)));}else{
void*_tmp68A;(_tmp68A=0,Cyc_fprintf(Cyc_stderr,*_tmpE9,_tag_dyneither(_tmp68A,sizeof(void*),0)));}
if(_tmpDE->tl != 0){const char*_tmp68D;void*_tmp68C;(_tmp68C=0,Cyc_fprintf(Cyc_stderr,((_tmp68D=",",_tag_dyneither(_tmp68D,sizeof(char),2))),_tag_dyneither(_tmp68C,sizeof(void*),0)));}}
# 591
{const char*_tmp690;void*_tmp68F;(_tmp68F=0,Cyc_fprintf(Cyc_stderr,((_tmp690=") ",_tag_dyneither(_tmp690,sizeof(char),3))),_tag_dyneither(_tmp68F,sizeof(void*),0)));}
goto _LL93;_LL9A: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpDF=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpDF->tag != 3)goto _LL9C;}_LL9B:
# 594
{const char*_tmp693;void*_tmp692;(_tmp692=0,Cyc_fprintf(Cyc_stderr,((_tmp693="Function_mod()",_tag_dyneither(_tmp693,sizeof(char),15))),_tag_dyneither(_tmp692,sizeof(void*),0)));}goto _LL93;_LL9C: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpE0=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpE0->tag != 5)goto _LL9E;}_LL9D:
{const char*_tmp696;void*_tmp695;(_tmp695=0,Cyc_fprintf(Cyc_stderr,((_tmp696="Attributes_mod ",_tag_dyneither(_tmp696,sizeof(char),16))),_tag_dyneither(_tmp695,sizeof(void*),0)));}goto _LL93;_LL9E: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpE1=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpE1->tag != 4)goto _LLA0;}_LL9F:
{const char*_tmp699;void*_tmp698;(_tmp698=0,Cyc_fprintf(Cyc_stderr,((_tmp699="TypeParams_mod ",_tag_dyneither(_tmp699,sizeof(char),16))),_tag_dyneither(_tmp698,sizeof(void*),0)));}goto _LL93;_LLA0: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpE2=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpD9;if(_tmpE2->tag != 2)goto _LL93;}_LLA1:
{const char*_tmp69C;void*_tmp69B;(_tmp69B=0,Cyc_fprintf(Cyc_stderr,((_tmp69C="Pointer_mod ",_tag_dyneither(_tmp69C,sizeof(char),13))),_tag_dyneither(_tmp69B,sizeof(void*),0)));}goto _LL93;_LL93:;}{
# 599
const char*_tmp69F;void*_tmp69E;(_tmp69E=0,Cyc_fprintf(Cyc_stderr,((_tmp69F="\n",_tag_dyneither(_tmp69F,sizeof(char),2))),_tag_dyneither(_tmp69E,sizeof(void*),0)));};}
# 602
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 604
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp6A4;const char*_tmp6A3;struct Cyc_PP_Doc*_tmp6A2[3];struct Cyc_PP_Doc*p_rest=(_tmp6A2[2]=Cyc_PP_text(((_tmp6A3=")",_tag_dyneither(_tmp6A3,sizeof(char),2)))),((_tmp6A2[1]=rest,((_tmp6A2[0]=Cyc_PP_text(((_tmp6A4="(",_tag_dyneither(_tmp6A4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6A2,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_stmttmpA=(void*)tms->hd;void*_tmpFB=_stmttmpA;union Cyc_Absyn_Constraint*_tmpFD;struct Cyc_Absyn_Exp*_tmpFF;union Cyc_Absyn_Constraint*_tmp100;void*_tmp102;struct Cyc_List_List*_tmp104;struct Cyc_List_List*_tmp106;unsigned int _tmp107;int _tmp108;void*_tmp10A;union Cyc_Absyn_Constraint*_tmp10B;union Cyc_Absyn_Constraint*_tmp10C;union Cyc_Absyn_Constraint*_tmp10D;struct Cyc_Absyn_Tqual _tmp10E;_LLA3: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpFC=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmpFC->tag != 0)goto _LLA5;else{_tmpFD=_tmpFC->f1;}}_LLA4:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A9;const char*_tmp6A8;struct Cyc_PP_Doc*_tmp6A7[2];return(_tmp6A7[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpFD)?
Cyc_PP_text(((_tmp6A8="[]@zeroterm",_tag_dyneither(_tmp6A8,sizeof(char),12)))): Cyc_PP_text(((_tmp6A9="[]",_tag_dyneither(_tmp6A9,sizeof(char),3)))),((_tmp6A7[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A7,sizeof(struct Cyc_PP_Doc*),2)))));};_LLA5: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpFE=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmpFE->tag != 1)goto _LLA7;else{_tmpFF=_tmpFE->f1;_tmp100=_tmpFE->f2;}}_LLA6:
# 614
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6B0;const char*_tmp6AF;const char*_tmp6AE;struct Cyc_PP_Doc*_tmp6AD[4];return(_tmp6AD[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp100)?Cyc_PP_text(((_tmp6AE="]@zeroterm",_tag_dyneither(_tmp6AE,sizeof(char),11)))): Cyc_PP_text(((_tmp6AF="]",_tag_dyneither(_tmp6AF,sizeof(char),2)))),((_tmp6AD[2]=
# 615
Cyc_Absynpp_exp2doc(_tmpFF),((_tmp6AD[1]=Cyc_PP_text(((_tmp6B0="[",_tag_dyneither(_tmp6B0,sizeof(char),2)))),((_tmp6AD[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AD,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA7: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp101=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmp101->tag != 3)goto _LLA9;else{_tmp102=(void*)_tmp101->f1;}}_LLA8:
# 618
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp116=_tmp102;struct Cyc_List_List*_tmp118;int _tmp119;struct Cyc_Absyn_VarargInfo*_tmp11A;void*_tmp11B;struct Cyc_List_List*_tmp11C;struct Cyc_List_List*_tmp11E;unsigned int _tmp11F;_LLB0: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp117=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp116;if(_tmp117->tag != 1)goto _LLB2;else{_tmp118=_tmp117->f1;_tmp119=_tmp117->f2;_tmp11A=_tmp117->f3;_tmp11B=(void*)_tmp117->f4;_tmp11C=_tmp117->f5;}}_LLB1: {
# 621
struct Cyc_PP_Doc*_tmp6B1[2];return(_tmp6B1[1]=Cyc_Absynpp_funargs2doc(_tmp118,_tmp119,_tmp11A,_tmp11B,_tmp11C),((_tmp6B1[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B1,sizeof(struct Cyc_PP_Doc*),2)))));}_LLB2: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp11D=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp116;if(_tmp11D->tag != 0)goto _LLAF;else{_tmp11E=_tmp11D->f1;_tmp11F=_tmp11D->f2;}}_LLB3: {
# 623
const char*_tmp6B8;const char*_tmp6B7;const char*_tmp6B6;struct Cyc_PP_Doc*_tmp6B5[2];return(_tmp6B5[1]=Cyc_PP_group(((_tmp6B8="(",_tag_dyneither(_tmp6B8,sizeof(char),2))),((_tmp6B7=")",_tag_dyneither(_tmp6B7,sizeof(char),2))),((_tmp6B6=",",_tag_dyneither(_tmp6B6,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp11E)),((_tmp6B5[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B5,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAF:;};_LLA9: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp103=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmp103->tag != 5)goto _LLAB;else{_tmp104=_tmp103->f2;}}_LLAA:
# 626
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLB4:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6B9[2];return(_tmp6B9[1]=Cyc_Absynpp_atts2doc(_tmp104),((_tmp6B9[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6B9,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLB5:
# 631
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6BA[2];return(_tmp6BA[1]=rest,((_tmp6BA[0]=Cyc_Absynpp_callconv2doc(_tmp104),Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLAB: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp105=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmp105->tag != 4)goto _LLAD;else{_tmp106=_tmp105->f1;_tmp107=_tmp105->f2;_tmp108=_tmp105->f3;}}_LLAC:
# 636
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmp108){
struct Cyc_PP_Doc*_tmp6BB[2];return(_tmp6BB[1]=Cyc_Absynpp_ktvars2doc(_tmp106),((_tmp6BB[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BB,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 640
struct Cyc_PP_Doc*_tmp6BC[2];return(_tmp6BC[1]=Cyc_Absynpp_tvars2doc(_tmp106),((_tmp6BC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAD: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp109=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpFB;if(_tmp109->tag != 2)goto _LLA2;else{_tmp10A=(_tmp109->f1).rgn;_tmp10B=(_tmp109->f1).nullable;_tmp10C=(_tmp109->f1).bounds;_tmp10D=(_tmp109->f1).zero_term;_tmp10E=_tmp109->f2;}}_LLAE: {
# 644
struct Cyc_PP_Doc*ptr;
struct Cyc_PP_Doc*mt=Cyc_PP_nil_doc();
struct Cyc_PP_Doc*ztd=mt;
struct Cyc_PP_Doc*rgd=mt;
struct Cyc_PP_Doc*tqd=Cyc_Absynpp_tqual2doc(_tmp10E);
{void*_stmttmpB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp10C);void*_tmp129=_stmttmpB;struct Cyc_Absyn_Exp*_tmp12C;_LLB8: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp12A=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp129;if(_tmp12A->tag != 0)goto _LLBA;}_LLB9:
 ptr=Cyc_Absynpp_question();goto _LLB7;_LLBA: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp12B=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp129;if(_tmp12B->tag != 1)goto _LLB7;else{_tmp12C=_tmp12B->f1;}}_LLBB:
# 652
{const char*_tmp6BE;const char*_tmp6BD;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp10B)?(_tmp6BE="*",_tag_dyneither(_tmp6BE,sizeof(char),2)):((_tmp6BD="@",_tag_dyneither(_tmp6BD,sizeof(char),2))));}{
struct _tuple11 _stmttmpC=Cyc_Evexp_eval_const_uint_exp(_tmp12C);unsigned int _tmp130;int _tmp131;struct _tuple11 _tmp12F=_stmttmpC;_tmp130=_tmp12F.f1;_tmp131=_tmp12F.f2;
if(!_tmp131  || _tmp130 != 1){
struct Cyc_PP_Doc*_tmp6BF[4];ptr=((_tmp6BF[3]=Cyc_Absynpp_rb(),((_tmp6BF[2]=Cyc_Absynpp_exp2doc(_tmp12C),((_tmp6BF[1]=Cyc_Absynpp_lb(),((_tmp6BF[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BF,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB7;};_LLB7:;}
# 658
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10D)){
const char*_tmp6C0;ztd=Cyc_PP_text(((_tmp6C0="@zeroterm",_tag_dyneither(_tmp6C0,sizeof(char),10))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp10D)){
const char*_tmp6C1;ztd=Cyc_PP_text(((_tmp6C1="@nozeroterm",_tag_dyneither(_tmp6C1,sizeof(char),12))));}}}
# 664
{void*_stmttmpD=Cyc_Tcutil_compress(_tmp10A);void*_tmp135=_stmttmpD;_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp136=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp135;if(_tmp136->tag != 20)goto _LLBF;}_LLBE:
 goto _LLBC;_LLBF:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp137=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp135;if(_tmp137->tag != 1)goto _LLC1;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLC1;_LLC0:
 goto _LLBC;_LLC1:;_LLC2:
 rgd=Cyc_Absynpp_rgn2doc(_tmp10A);_LLBC:;}{
# 669
const char*_tmp6C2;struct Cyc_PP_Doc*spacer1=tqd != mt  && (ztd != mt  || rgd != mt)?Cyc_PP_text(((_tmp6C2=" ",_tag_dyneither(_tmp6C2,sizeof(char),2)))): mt;
const char*_tmp6C3;struct Cyc_PP_Doc*spacer2=rest != mt?Cyc_PP_text(((_tmp6C3=" ",_tag_dyneither(_tmp6C3,sizeof(char),2)))): mt;
struct Cyc_PP_Doc*_tmp6C4[7];return(_tmp6C4[6]=rest,((_tmp6C4[5]=spacer2,((_tmp6C4[4]=tqd,((_tmp6C4[3]=spacer1,((_tmp6C4[2]=rgd,((_tmp6C4[1]=ztd,((_tmp6C4[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6C4,sizeof(struct Cyc_PP_Doc*),7)))))))))))))));};}_LLA2:;};}
# 675
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_stmttmpE=Cyc_Tcutil_compress(t);void*_tmp13E=_stmttmpE;_LLC4: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp13F=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp13E;if(_tmp13F->tag != 20)goto _LLC6;}_LLC5: {
const char*_tmp6C5;return Cyc_PP_text(((_tmp6C5="`H",_tag_dyneither(_tmp6C5,sizeof(char),3))));}_LLC6: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp140=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp13E;if(_tmp140->tag != 21)goto _LLC8;}_LLC7: {
const char*_tmp6C6;return Cyc_PP_text(((_tmp6C6="`U",_tag_dyneither(_tmp6C6,sizeof(char),3))));}_LLC8: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp141=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp13E;if(_tmp141->tag != 22)goto _LLCA;}_LLC9: {
const char*_tmp6C7;return Cyc_PP_text(((_tmp6C7="`RC",_tag_dyneither(_tmp6C7,sizeof(char),4))));}_LLCA:;_LLCB:
 return Cyc_Absynpp_ntyp2doc(t);_LLC3:;}
# 685
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 689
void*_stmttmpF=Cyc_Tcutil_compress(t);void*_tmp145=_stmttmpF;void*_tmp147;struct Cyc_List_List*_tmp149;_LLCD: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp146=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp145;if(_tmp146->tag != 23)goto _LLCF;else{_tmp147=(void*)_tmp146->f1;}}_LLCE:
{struct Cyc_List_List*_tmp6C8;*rgions=((_tmp6C8=_cycalloc(sizeof(*_tmp6C8)),((_tmp6C8->hd=Cyc_Absynpp_rgn2doc(_tmp147),((_tmp6C8->tl=*rgions,_tmp6C8))))));}goto _LLCC;_LLCF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp148=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp145;if(_tmp148->tag != 24)goto _LLD1;else{_tmp149=_tmp148->f1;}}_LLD0:
# 692
 for(0;_tmp149 != 0;_tmp149=_tmp149->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp149->hd);}
# 695
goto _LLCC;_LLD1:;_LLD2:
{struct Cyc_List_List*_tmp6C9;*effects=((_tmp6C9=_cycalloc(sizeof(*_tmp6C9)),((_tmp6C9->hd=Cyc_Absynpp_typ2doc(t),((_tmp6C9->tl=*effects,_tmp6C9))))));}goto _LLCC;_LLCC:;}
# 700
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6CC;const char*_tmp6CB;const char*_tmp6CA;return Cyc_PP_group(((_tmp6CA="",_tag_dyneither(_tmp6CA,sizeof(char),1))),((_tmp6CB="",_tag_dyneither(_tmp6CB,sizeof(char),1))),((_tmp6CC="+",_tag_dyneither(_tmp6CC,sizeof(char),2))),effects);}else{
# 708
const char*_tmp6CD;struct Cyc_PP_Doc*_tmp14F=Cyc_Absynpp_group_braces(((_tmp6CD=",",_tag_dyneither(_tmp6CD,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6D1;const char*_tmp6D0;const char*_tmp6CF;const char*_tmp6CE;return Cyc_PP_group(((_tmp6CE="",_tag_dyneither(_tmp6CE,sizeof(char),1))),((_tmp6CF="",_tag_dyneither(_tmp6CF,sizeof(char),1))),((_tmp6D0="+",_tag_dyneither(_tmp6D0,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6D1=_cycalloc(sizeof(*_tmp6D1)),((_tmp6D1->hd=_tmp14F,((_tmp6D1->tl=0,_tmp6D1))))))));}}
# 713
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLD3: {
const char*_tmp6D2;return Cyc_PP_text(((_tmp6D2="struct ",_tag_dyneither(_tmp6D2,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLD4: {
const char*_tmp6D3;return Cyc_PP_text(((_tmp6D3="union ",_tag_dyneither(_tmp6D3,sizeof(char),7))));}}}
# 721
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp157=t;struct Cyc_Core_Opt*_tmp15D;void*_tmp15E;int _tmp15F;struct Cyc_Core_Opt*_tmp160;struct Cyc_Absyn_Tvar*_tmp162;union Cyc_Absyn_DatatypeInfoU _tmp164;struct Cyc_List_List*_tmp165;union Cyc_Absyn_DatatypeFieldInfoU _tmp167;struct Cyc_List_List*_tmp168;enum Cyc_Absyn_Sign _tmp16A;enum Cyc_Absyn_Size_of _tmp16B;int _tmp16D;struct Cyc_List_List*_tmp16F;union Cyc_Absyn_AggrInfoU _tmp171;struct Cyc_List_List*_tmp172;enum Cyc_Absyn_AggrKind _tmp174;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp177;struct _tuple0*_tmp179;struct Cyc_Absyn_Exp*_tmp17B;struct _tuple0*_tmp17D;struct Cyc_List_List*_tmp17E;struct Cyc_Absyn_Typedefdecl*_tmp17F;void*_tmp181;void*_tmp183;void*_tmp184;void*_tmp186;void*_tmp18B;struct Cyc_Absyn_Aggrdecl*_tmp190;struct Cyc_Absyn_Enumdecl*_tmp193;struct Cyc_Absyn_Datatypedecl*_tmp196;_LLD7: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp157;if(_tmp158->tag != 8)goto _LLD9;}_LLD8: {
# 725
const char*_tmp6D4;return Cyc_PP_text(((_tmp6D4="[[[array]]]",_tag_dyneither(_tmp6D4,sizeof(char),12))));}_LLD9: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157;if(_tmp159->tag != 9)goto _LLDB;}_LLDA:
 return Cyc_PP_nil_doc();_LLDB: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp15A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp157;if(_tmp15A->tag != 5)goto _LLDD;}_LLDC:
 return Cyc_PP_nil_doc();_LLDD: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp15B=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp157;if(_tmp15B->tag != 0)goto _LLDF;}_LLDE:
# 729
{const char*_tmp6D5;s=Cyc_PP_text(((_tmp6D5="void",_tag_dyneither(_tmp6D5,sizeof(char),5))));}goto _LLD6;_LLDF: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp15C=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp157;if(_tmp15C->tag != 1)goto _LLE1;else{_tmp15D=_tmp15C->f1;_tmp15E=(void*)_tmp15C->f2;_tmp15F=_tmp15C->f3;_tmp160=_tmp15C->f4;}}_LLE0:
# 731
 if(_tmp15E != 0)
# 733
return Cyc_Absynpp_ntyp2doc(_tmp15E);else{
# 735
const char*_tmp6D6;struct _dyneither_ptr kindstring=_tmp15D == 0?(_tmp6D6="K",_tag_dyneither(_tmp6D6,sizeof(char),2)): Cyc_Absynpp_kind2string((struct Cyc_Absyn_Kind*)_tmp15D->v);
const char*_tmp6DB;void*_tmp6DA[2];struct Cyc_String_pa_PrintArg_struct _tmp6D9;struct Cyc_Int_pa_PrintArg_struct _tmp6D8;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp6D8.tag=1,((_tmp6D8.f1=(unsigned long)_tmp15F,((_tmp6D9.tag=0,((_tmp6D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)kindstring),((_tmp6DA[0]=& _tmp6D9,((_tmp6DA[1]=& _tmp6D8,Cyc_aprintf(((_tmp6DB="`E%s%d",_tag_dyneither(_tmp6DB,sizeof(char),7))),_tag_dyneither(_tmp6DA,sizeof(void*),2)))))))))))))));}
# 752 "absynpp.cyc"
goto _LLD6;_LLE1: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp161=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp157;if(_tmp161->tag != 2)goto _LLE3;else{_tmp162=_tmp161->f1;}}_LLE2:
# 754
 s=Cyc_Absynpp_tvar2doc(_tmp162);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6DE;struct Cyc_PP_Doc*_tmp6DD[3];s=((_tmp6DD[2]=Cyc_Absynpp_kindbound2doc(_tmp162->kind),((_tmp6DD[1]=Cyc_PP_text(((_tmp6DE="::",_tag_dyneither(_tmp6DE,sizeof(char),3)))),((_tmp6DD[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6DD,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 758
goto _LLD6;_LLE3: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp157;if(_tmp163->tag != 3)goto _LLE5;else{_tmp164=(_tmp163->f1).datatype_info;_tmp165=(_tmp163->f1).targs;}}_LLE4:
# 760
{union Cyc_Absyn_DatatypeInfoU _tmp1A0=_tmp164;struct _tuple0*_tmp1A1;int _tmp1A2;struct _tuple0*_tmp1A3;int _tmp1A4;_LL112: if((_tmp1A0.UnknownDatatype).tag != 1)goto _LL114;_tmp1A1=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A0.UnknownDatatype).val).name;_tmp1A2=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A0.UnknownDatatype).val).is_extensible;_LL113:
# 762
 _tmp1A3=_tmp1A1;_tmp1A4=_tmp1A2;goto _LL115;_LL114: if((_tmp1A0.KnownDatatype).tag != 2)goto _LL111;_tmp1A3=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A0.KnownDatatype).val))->name;_tmp1A4=(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A0.KnownDatatype).val))->is_extensible;_LL115: {
# 764
const char*_tmp6DF;struct Cyc_PP_Doc*_tmp1A5=Cyc_PP_text(((_tmp6DF="datatype ",_tag_dyneither(_tmp6DF,sizeof(char),10))));
const char*_tmp6E0;struct Cyc_PP_Doc*_tmp1A6=_tmp1A4?Cyc_PP_text(((_tmp6E0="@extensible ",_tag_dyneither(_tmp6E0,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6E1[4];s=((_tmp6E1[3]=Cyc_Absynpp_tps2doc(_tmp165),((_tmp6E1[2]=Cyc_Absynpp_qvar2doc(_tmp1A3),((_tmp6E1[1]=_tmp1A5,((_tmp6E1[0]=_tmp1A6,Cyc_PP_cat(_tag_dyneither(_tmp6E1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 769
goto _LLD6;_LLE5: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp157;if(_tmp166->tag != 4)goto _LLE7;else{_tmp167=(_tmp166->f1).field_info;_tmp168=(_tmp166->f1).targs;}}_LLE6:
# 771
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AA=_tmp167;struct _tuple0*_tmp1AB;struct _tuple0*_tmp1AC;int _tmp1AD;struct _tuple0*_tmp1AE;int _tmp1AF;struct _tuple0*_tmp1B0;_LL117: if((_tmp1AA.UnknownDatatypefield).tag != 1)goto _LL119;_tmp1AB=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AA.UnknownDatatypefield).val).datatype_name;_tmp1AC=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AA.UnknownDatatypefield).val).field_name;_tmp1AD=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AA.UnknownDatatypefield).val).is_extensible;_LL118:
# 773
 _tmp1AE=_tmp1AB;_tmp1AF=_tmp1AD;_tmp1B0=_tmp1AC;goto _LL11A;_LL119: if((_tmp1AA.KnownDatatypefield).tag != 2)goto _LL116;_tmp1AE=(((struct _tuple1)(_tmp1AA.KnownDatatypefield).val).f1)->name;_tmp1AF=(((struct _tuple1)(_tmp1AA.KnownDatatypefield).val).f1)->is_extensible;_tmp1B0=(((struct _tuple1)(_tmp1AA.KnownDatatypefield).val).f2)->name;_LL11A: {
# 776
const char*_tmp6E3;const char*_tmp6E2;struct Cyc_PP_Doc*_tmp1B1=Cyc_PP_text(_tmp1AF?(_tmp6E3="@extensible datatype ",_tag_dyneither(_tmp6E3,sizeof(char),22)):((_tmp6E2="datatype ",_tag_dyneither(_tmp6E2,sizeof(char),10))));
{const char*_tmp6E6;struct Cyc_PP_Doc*_tmp6E5[4];s=((_tmp6E5[3]=Cyc_Absynpp_qvar2doc(_tmp1B0),((_tmp6E5[2]=Cyc_PP_text(((_tmp6E6=".",_tag_dyneither(_tmp6E6,sizeof(char),2)))),((_tmp6E5[1]=Cyc_Absynpp_qvar2doc(_tmp1AE),((_tmp6E5[0]=_tmp1B1,Cyc_PP_cat(_tag_dyneither(_tmp6E5,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL116;}_LL116:;}
# 780
goto _LLD6;_LLE7: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp157;if(_tmp169->tag != 6)goto _LLE9;else{_tmp16A=_tmp169->f1;_tmp16B=_tmp169->f2;}}_LLE8: {
# 782
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp16A){case Cyc_Absyn_None: _LL11B:
 goto _LL11C;case Cyc_Absyn_Signed: _LL11C:
{const char*_tmp6E7;sns=((_tmp6E7="",_tag_dyneither(_tmp6E7,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL11D:
{const char*_tmp6E8;sns=((_tmp6E8="unsigned ",_tag_dyneither(_tmp6E8,sizeof(char),10)));}break;}
# 789
switch(_tmp16B){case Cyc_Absyn_Char_sz: _LL11F:
# 791
 switch(_tmp16A){case Cyc_Absyn_None: _LL121:
{const char*_tmp6E9;sns=((_tmp6E9="",_tag_dyneither(_tmp6E9,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL122:
{const char*_tmp6EA;sns=((_tmp6EA="signed ",_tag_dyneither(_tmp6EA,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL123:
{const char*_tmp6EB;sns=((_tmp6EB="unsigned ",_tag_dyneither(_tmp6EB,sizeof(char),10)));}break;}
# 796
{const char*_tmp6EC;ts=((_tmp6EC="char",_tag_dyneither(_tmp6EC,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL120:
{const char*_tmp6ED;ts=((_tmp6ED="short",_tag_dyneither(_tmp6ED,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL125:
{const char*_tmp6EE;ts=((_tmp6EE="int",_tag_dyneither(_tmp6EE,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL126:
{const char*_tmp6EF;ts=((_tmp6EF="long",_tag_dyneither(_tmp6EF,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL127:
# 802
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL129:
{const char*_tmp6F0;ts=((_tmp6F0="long long",_tag_dyneither(_tmp6F0,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL12A:
{const char*_tmp6F1;ts=((_tmp6F1="__int64",_tag_dyneither(_tmp6F1,sizeof(char),8)));}break;}
# 806
break;}
# 808
{const char*_tmp6F6;void*_tmp6F5[2];struct Cyc_String_pa_PrintArg_struct _tmp6F4;struct Cyc_String_pa_PrintArg_struct _tmp6F3;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp6F3.tag=0,((_tmp6F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp6F4.tag=0,((_tmp6F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp6F5[0]=& _tmp6F4,((_tmp6F5[1]=& _tmp6F3,Cyc_aprintf(((_tmp6F6="%s%s",_tag_dyneither(_tmp6F6,sizeof(char),5))),_tag_dyneither(_tmp6F5,sizeof(void*),2)))))))))))))));}
goto _LLD6;}_LLE9: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp16C=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp157;if(_tmp16C->tag != 7)goto _LLEB;else{_tmp16D=_tmp16C->f1;}}_LLEA:
# 811
 if(_tmp16D == 0){
const char*_tmp6F7;s=Cyc_PP_text(((_tmp6F7="float",_tag_dyneither(_tmp6F7,sizeof(char),6))));}else{
if(_tmp16D == 1){
const char*_tmp6F8;s=Cyc_PP_text(((_tmp6F8="double",_tag_dyneither(_tmp6F8,sizeof(char),7))));}else{
# 816
const char*_tmp6F9;s=Cyc_PP_text(((_tmp6F9="long double",_tag_dyneither(_tmp6F9,sizeof(char),12))));}}
goto _LLD6;_LLEB: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16E=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp157;if(_tmp16E->tag != 10)goto _LLED;else{_tmp16F=_tmp16E->f1;}}_LLEC:
# 819
{struct Cyc_PP_Doc*_tmp6FA[2];s=((_tmp6FA[1]=Cyc_Absynpp_args2doc(_tmp16F),((_tmp6FA[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp6FA,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLED: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp157;if(_tmp170->tag != 11)goto _LLEF;else{_tmp171=(_tmp170->f1).aggr_info;_tmp172=(_tmp170->f1).targs;}}_LLEE: {
# 822
struct _tuple10 _stmttmp10=Cyc_Absyn_aggr_kinded_name(_tmp171);enum Cyc_Absyn_AggrKind _tmp1CA;struct _tuple0*_tmp1CB;struct _tuple10 _tmp1C9=_stmttmp10;_tmp1CA=_tmp1C9.f1;_tmp1CB=_tmp1C9.f2;
{struct Cyc_PP_Doc*_tmp6FB[3];s=((_tmp6FB[2]=Cyc_Absynpp_tps2doc(_tmp172),((_tmp6FB[1]=Cyc_Absynpp_qvar2doc(_tmp1CB),((_tmp6FB[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CA),Cyc_PP_cat(_tag_dyneither(_tmp6FB,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD6;}_LLEF: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp157;if(_tmp173->tag != 12)goto _LLF1;else{_tmp174=_tmp173->f1;_tmp175=_tmp173->f2;}}_LLF0:
# 826
{struct Cyc_PP_Doc*_tmp6FC[4];s=((_tmp6FC[3]=
# 828
Cyc_Absynpp_rb(),((_tmp6FC[2]=
# 827
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp175)),((_tmp6FC[1]=
# 826
Cyc_Absynpp_lb(),((_tmp6FC[0]=Cyc_Absynpp_aggr_kind2doc(_tmp174),Cyc_PP_cat(_tag_dyneither(_tmp6FC,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 829
goto _LLD6;_LLF1: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp176=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp157;if(_tmp176->tag != 14)goto _LLF3;else{_tmp177=_tmp176->f1;}}_LLF2:
# 831
{const char*_tmp6FF;struct Cyc_PP_Doc*_tmp6FE[4];s=((_tmp6FE[3]=Cyc_Absynpp_rb(),((_tmp6FE[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp177)),((_tmp6FE[1]=Cyc_Absynpp_lb(),((_tmp6FE[0]=Cyc_PP_text(((_tmp6FF="enum ",_tag_dyneither(_tmp6FF,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6FE,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD6;_LLF3: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp178=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp157;if(_tmp178->tag != 13)goto _LLF5;else{_tmp179=_tmp178->f1;}}_LLF4:
# 833
{const char*_tmp702;struct Cyc_PP_Doc*_tmp701[2];s=((_tmp701[1]=Cyc_Absynpp_qvar2doc(_tmp179),((_tmp701[0]=Cyc_PP_text(((_tmp702="enum ",_tag_dyneither(_tmp702,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp701,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD6;_LLF5: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17A=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp157;if(_tmp17A->tag != 18)goto _LLF7;else{_tmp17B=_tmp17A->f1;}}_LLF6:
# 835
{const char*_tmp707;const char*_tmp706;struct Cyc_PP_Doc*_tmp705[3];s=((_tmp705[2]=Cyc_PP_text(((_tmp706=")",_tag_dyneither(_tmp706,sizeof(char),2)))),((_tmp705[1]=Cyc_Absynpp_exp2doc(_tmp17B),((_tmp705[0]=Cyc_PP_text(((_tmp707="valueof_t(",_tag_dyneither(_tmp707,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp705,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLF7: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp17C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp157;if(_tmp17C->tag != 17)goto _LLF9;else{_tmp17D=_tmp17C->f1;_tmp17E=_tmp17C->f2;_tmp17F=_tmp17C->f3;}}_LLF8:
# 842
{struct Cyc_PP_Doc*_tmp708[2];s=((_tmp708[1]=Cyc_Absynpp_tps2doc(_tmp17E),((_tmp708[0]=Cyc_Absynpp_qvar2doc(_tmp17D),Cyc_PP_cat(_tag_dyneither(_tmp708,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD6;_LLF9: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp180=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp157;if(_tmp180->tag != 15)goto _LLFB;else{_tmp181=(void*)_tmp180->f1;}}_LLFA:
# 845
{const char*_tmp70D;const char*_tmp70C;struct Cyc_PP_Doc*_tmp70B[3];s=((_tmp70B[2]=Cyc_PP_text(((_tmp70C=">",_tag_dyneither(_tmp70C,sizeof(char),2)))),((_tmp70B[1]=Cyc_Absynpp_rgn2doc(_tmp181),((_tmp70B[0]=Cyc_PP_text(((_tmp70D="region_t<",_tag_dyneither(_tmp70D,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFB: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp157;if(_tmp182->tag != 16)goto _LLFD;else{_tmp183=(void*)_tmp182->f1;_tmp184=(void*)_tmp182->f2;}}_LLFC:
# 847
{const char*_tmp714;const char*_tmp713;const char*_tmp712;struct Cyc_PP_Doc*_tmp711[5];s=((_tmp711[4]=
Cyc_PP_text(((_tmp712=">",_tag_dyneither(_tmp712,sizeof(char),2)))),((_tmp711[3]=
# 847
Cyc_Absynpp_rgn2doc(_tmp184),((_tmp711[2]=Cyc_PP_text(((_tmp713=",",_tag_dyneither(_tmp713,sizeof(char),2)))),((_tmp711[1]=Cyc_Absynpp_rgn2doc(_tmp183),((_tmp711[0]=Cyc_PP_text(((_tmp714="dynregion_t<",_tag_dyneither(_tmp714,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp711,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD6;_LLFD: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp157;if(_tmp185->tag != 19)goto _LLFF;else{_tmp186=(void*)_tmp185->f1;}}_LLFE:
# 850
{const char*_tmp719;const char*_tmp718;struct Cyc_PP_Doc*_tmp717[3];s=((_tmp717[2]=Cyc_PP_text(((_tmp718=">",_tag_dyneither(_tmp718,sizeof(char),2)))),((_tmp717[1]=Cyc_Absynpp_typ2doc(_tmp186),((_tmp717[0]=Cyc_PP_text(((_tmp719="tag_t<",_tag_dyneither(_tmp719,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp717,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LLFF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp157;if(_tmp187->tag != 21)goto _LL101;}_LL100:
 goto _LL102;_LL101: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp188=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp157;if(_tmp188->tag != 20)goto _LL103;}_LL102:
 goto _LL104;_LL103: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp157;if(_tmp189->tag != 22)goto _LL105;}_LL104:
# 854
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD6;_LL105: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp157;if(_tmp18A->tag != 25)goto _LL107;else{_tmp18B=(void*)_tmp18A->f1;}}_LL106:
# 856
{const char*_tmp71E;const char*_tmp71D;struct Cyc_PP_Doc*_tmp71C[3];s=((_tmp71C[2]=Cyc_PP_text(((_tmp71D=")",_tag_dyneither(_tmp71D,sizeof(char),2)))),((_tmp71C[1]=Cyc_Absynpp_typ2doc(_tmp18B),((_tmp71C[0]=Cyc_PP_text(((_tmp71E="regions(",_tag_dyneither(_tmp71E,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp71C,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD6;_LL107: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp18C=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp157;if(_tmp18C->tag != 23)goto _LL109;}_LL108:
 goto _LL10A;_LL109: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18D=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp157;if(_tmp18D->tag != 24)goto _LL10B;}_LL10A:
# 859
 s=Cyc_Absynpp_eff2doc(t);goto _LLD6;_LL10B: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp157;if(_tmp18E->tag != 26)goto _LL10D;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp18F=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(_tmp18E->f1)->r;if(_tmp18F->tag != 0)goto _LL10D;else{_tmp190=_tmp18F->f1;}}}_LL10C:
# 861
 s=Cyc_Absynpp_aggrdecl2doc(_tmp190);goto _LLD6;_LL10D: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp157;if(_tmp191->tag != 26)goto _LL10F;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp192=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(_tmp191->f1)->r;if(_tmp192->tag != 1)goto _LL10F;else{_tmp193=_tmp192->f1;}}}_LL10E:
# 863
 s=Cyc_Absynpp_enumdecl2doc(_tmp193);goto _LLD6;_LL10F: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp194=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp157;if(_tmp194->tag != 26)goto _LLD6;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp195=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(_tmp194->f1)->r;if(_tmp195->tag != 2)goto _LLD6;else{_tmp196=_tmp195->f1;}}}_LL110:
# 865
 s=Cyc_Absynpp_datatypedecl2doc(_tmp196);goto _LLD6;_LLD6:;}
# 867
return s;}
# 870
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*vo);}struct _tuple16{void*f1;void*f2;};
# 874
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1E4;void*_tmp1E5;struct _tuple16*_tmp1E3=cmp;_tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;{
const char*_tmp721;struct Cyc_PP_Doc*_tmp720[3];return(_tmp720[2]=Cyc_Absynpp_rgn2doc(_tmp1E5),((_tmp720[1]=Cyc_PP_text(((_tmp721=" > ",_tag_dyneither(_tmp721,sizeof(char),4)))),((_tmp720[0]=Cyc_Absynpp_rgn2doc(_tmp1E4),Cyc_PP_cat(_tag_dyneither(_tmp720,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 879
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp724;const char*_tmp723;const char*_tmp722;return Cyc_PP_group(((_tmp722="",_tag_dyneither(_tmp722,sizeof(char),1))),((_tmp723="",_tag_dyneither(_tmp723,sizeof(char),1))),((_tmp724=",",_tag_dyneither(_tmp724,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 883
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1EB=(*t).f1;
struct Cyc_Core_Opt*_tmp725;struct Cyc_Core_Opt*dopt=_tmp1EB == 0?0:((_tmp725=_cycalloc(sizeof(*_tmp725)),((_tmp725->v=Cyc_PP_text(*_tmp1EB),_tmp725))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 889
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 892
struct Cyc_List_List*_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp728;struct Cyc_List_List*_tmp727;_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1ED,((_tmp727=_cycalloc(sizeof(*_tmp727)),((_tmp727->hd=Cyc_PP_text(((_tmp728="...",_tag_dyneither(_tmp728,sizeof(char),4)))),((_tmp727->tl=0,_tmp727)))))));}else{
if(cyc_varargs != 0){
const char*_tmp731;const char*_tmp730;const char*_tmp72F;struct _tuple8*_tmp72E;struct Cyc_PP_Doc*_tmp72D[3];struct Cyc_PP_Doc*_tmp1F0=(_tmp72D[2]=
# 899
Cyc_Absynpp_funarg2doc(((_tmp72E=_cycalloc(sizeof(*_tmp72E)),((_tmp72E->f1=cyc_varargs->name,((_tmp72E->f2=cyc_varargs->tq,((_tmp72E->f3=cyc_varargs->type,_tmp72E))))))))),((_tmp72D[1]=
# 898
cyc_varargs->inject?Cyc_PP_text(((_tmp72F=" inject ",_tag_dyneither(_tmp72F,sizeof(char),9)))): Cyc_PP_text(((_tmp730=" ",_tag_dyneither(_tmp730,sizeof(char),2)))),((_tmp72D[0]=
# 897
Cyc_PP_text(((_tmp731="...",_tag_dyneither(_tmp731,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),3)))))));
# 901
struct Cyc_List_List*_tmp732;_tmp1ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1ED,((_tmp732=_cycalloc(sizeof(*_tmp732)),((_tmp732->hd=_tmp1F0,((_tmp732->tl=0,_tmp732)))))));}}{
# 903
const char*_tmp735;const char*_tmp734;const char*_tmp733;struct Cyc_PP_Doc*_tmp1F7=Cyc_PP_group(((_tmp733="",_tag_dyneither(_tmp733,sizeof(char),1))),((_tmp734="",_tag_dyneither(_tmp734,sizeof(char),1))),((_tmp735=",",_tag_dyneither(_tmp735,sizeof(char),2))),_tmp1ED);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp738;struct Cyc_PP_Doc*_tmp737[3];_tmp1F7=((_tmp737[2]=Cyc_Absynpp_eff2doc(effopt),((_tmp737[1]=Cyc_PP_text(((_tmp738=";",_tag_dyneither(_tmp738,sizeof(char),2)))),((_tmp737[0]=_tmp1F7,Cyc_PP_cat(_tag_dyneither(_tmp737,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp73B;struct Cyc_PP_Doc*_tmp73A[3];_tmp1F7=((_tmp73A[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp73A[1]=Cyc_PP_text(((_tmp73B=":",_tag_dyneither(_tmp73B,sizeof(char),2)))),((_tmp73A[0]=_tmp1F7,Cyc_PP_cat(_tag_dyneither(_tmp73A,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp740;const char*_tmp73F;struct Cyc_PP_Doc*_tmp73E[3];return(_tmp73E[2]=Cyc_PP_text(((_tmp73F=")",_tag_dyneither(_tmp73F,sizeof(char),2)))),((_tmp73E[1]=_tmp1F7,((_tmp73E[0]=Cyc_PP_text(((_tmp740="(",_tag_dyneither(_tmp740,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp73E,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 911
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp203;struct Cyc_Absyn_Tqual _tmp204;void*_tmp205;struct _tuple8*_tmp202=arg;_tmp203=_tmp202->f1;_tmp204=_tmp202->f2;_tmp205=_tmp202->f3;{
struct _tuple8*_tmp741;return(_tmp741=_cycalloc(sizeof(*_tmp741)),((_tmp741->f1=_tmp203,((_tmp741->f2=_tmp204,((_tmp741->f3=_tmp205,_tmp741)))))));};}
# 916
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 918
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp207=0;
int match;
{union Cyc_Absyn_Nmspace _stmttmp11=(*q).f1;union Cyc_Absyn_Nmspace _tmp208=_stmttmp11;struct Cyc_List_List*_tmp209;struct Cyc_List_List*_tmp20A;struct Cyc_List_List*_tmp20B;_LL12D: if((_tmp208.Loc_n).tag != 4)goto _LL12F;_LL12E:
# 923
 _tmp209=0;goto _LL130;_LL12F: if((_tmp208.Rel_n).tag != 1)goto _LL131;_tmp209=(struct Cyc_List_List*)(_tmp208.Rel_n).val;_LL130:
# 925
 match=0;
_tmp207=_tmp209;
goto _LL12C;_LL131: if((_tmp208.C_n).tag != 3)goto _LL133;_tmp20A=(struct Cyc_List_List*)(_tmp208.C_n).val;_LL132:
# 929
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20A,Cyc_Absynpp_curr_namespace);
# 931
goto _LL12C;_LL133: if((_tmp208.Abs_n).tag != 2)goto _LL12C;_tmp20B=(struct Cyc_List_List*)(_tmp208.Abs_n).val;_LL134:
# 933
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20B,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp742;_tmp207=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp742=_cycalloc(sizeof(*_tmp742)),((_tmp742->hd=Cyc_Absynpp_cyc_stringptr,((_tmp742->tl=_tmp20B,_tmp742))))): _tmp20B;}
goto _LL12C;_LL12C:;}
# 937
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp744;struct Cyc_List_List*_tmp743;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,(
(_tmp743=_cycalloc(sizeof(*_tmp743)),((_tmp743->hd=(*q).f2,((_tmp743->tl=0,_tmp743))))))),((_tmp744="_",_tag_dyneither(_tmp744,sizeof(char),2))));}else{
# 942
if(match)
return*(*q).f2;else{
# 945
const char*_tmp746;struct Cyc_List_List*_tmp745;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp207,(
(_tmp745=_cycalloc(sizeof(*_tmp745)),((_tmp745->hd=(*q).f2,((_tmp745->tl=0,_tmp745))))))),((_tmp746="::",_tag_dyneither(_tmp746,sizeof(char),3))));}}}
# 950
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 954
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp211=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 958
const char*_tmp748;const char*_tmp747;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp747="\\textbf{",_tag_dyneither(_tmp747,sizeof(char),9))),(struct _dyneither_ptr)_tmp211),((_tmp748="}",_tag_dyneither(_tmp748,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp211));}else{
return Cyc_PP_text(_tmp211);}}
# 963
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 965
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 968
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _stmttmp12=(*v).f1;union Cyc_Absyn_Nmspace _tmp214=_stmttmp12;struct Cyc_List_List*_tmp215;struct Cyc_List_List*_tmp216;_LL136: if((_tmp214.Loc_n).tag != 4)goto _LL138;_LL137:
 goto _LL139;_LL138: if((_tmp214.Rel_n).tag != 1)goto _LL13A;if((struct Cyc_List_List*)(_tmp214.Rel_n).val != 0)goto _LL13A;_LL139:
 return*(*v).f2;_LL13A: if((_tmp214.C_n).tag != 3)goto _LL13C;_tmp215=(struct Cyc_List_List*)(_tmp214.C_n).val;_LL13B:
 _tmp216=_tmp215;goto _LL13D;_LL13C: if((_tmp214.Abs_n).tag != 2)goto _LL13E;_tmp216=(struct Cyc_List_List*)(_tmp214.Abs_n).val;_LL13D:
# 974
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp216,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 977
goto _LL13F;}_LL13E:;_LL13F: {
# 979
const char*_tmp749;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp749="/* bad namespace : */ ",_tag_dyneither(_tmp749,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL135:;}else{
# 982
return*(*v).f2;}}
# 985
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 988
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp218=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 992
const char*_tmp74B;const char*_tmp74A;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp74A="\\textbf{",_tag_dyneither(_tmp74A,sizeof(char),9))),(struct _dyneither_ptr)_tmp218),((_tmp74B="}",_tag_dyneither(_tmp74B,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp218));}else{
return Cyc_PP_text(_tmp218);}}
# 997
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1008 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_stmttmp13=e->r;void*_tmp21B=_stmttmp13;enum Cyc_Absyn_Primop _tmp21F;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp22B;_LL141: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp21C=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp21C->tag != 0)goto _LL143;}_LL142:
 goto _LL144;_LL143: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp21D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp21D->tag != 1)goto _LL145;}_LL144:
 return 10000;_LL145: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp21E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp21E->tag != 2)goto _LL147;else{_tmp21F=_tmp21E->f1;}}_LL146:
# 1013
 switch(_tmp21F){case Cyc_Absyn_Plus: _LL191:
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
 return 140;}_LL147: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp220=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp220->tag != 3)goto _LL149;}_LL148:
# 1035
 return 20;_LL149: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp221=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp221->tag != 4)goto _LL14B;}_LL14A:
 return 130;_LL14B: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp222=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp222->tag != 5)goto _LL14D;}_LL14C:
 return 30;_LL14D: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp223=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp223->tag != 6)goto _LL14F;}_LL14E:
 return 35;_LL14F: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp224=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp224->tag != 7)goto _LL151;}_LL150:
 return 30;_LL151: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp225->tag != 8)goto _LL153;}_LL152:
 return 10;_LL153: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp226=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp226->tag != 9)goto _LL155;}_LL154:
 return 140;_LL155: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp227=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp227->tag != 10)goto _LL157;}_LL156:
 return 130;_LL157: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp228->tag != 11)goto _LL159;else{_tmp229=_tmp228->f1;}}_LL158:
 return Cyc_Absynpp_exp_prec(_tmp229);_LL159: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp22A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp22A->tag != 12)goto _LL15B;else{_tmp22B=_tmp22A->f1;}}_LL15A:
 return Cyc_Absynpp_exp_prec(_tmp22B);_LL15B: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp22C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp22C->tag != 13)goto _LL15D;}_LL15C:
 return 120;_LL15D: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp22D=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp22D->tag != 15)goto _LL15F;}_LL15E:
 return 15;_LL15F: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp22E=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp22E->tag != 14)goto _LL161;}_LL160:
 goto _LL162;_LL161: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp22F=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp22F->tag != 16)goto _LL163;}_LL162:
 goto _LL164;_LL163: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp230=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp230->tag != 17)goto _LL165;}_LL164:
 goto _LL166;_LL165: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp231=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp231->tag != 38)goto _LL167;}_LL166:
 goto _LL168;_LL167: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp232=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp232->tag != 39)goto _LL169;}_LL168:
 goto _LL16A;_LL169: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp233=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp233->tag != 37)goto _LL16B;}_LL16A:
 goto _LL16C;_LL16B: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp234=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp234->tag != 18)goto _LL16D;}_LL16C:
 goto _LL16E;_LL16D: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp235=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp235->tag != 19)goto _LL16F;}_LL16E:
 return 130;_LL16F: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp236->tag != 20)goto _LL171;}_LL170:
 goto _LL172;_LL171: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp237->tag != 21)goto _LL173;}_LL172:
 goto _LL174;_LL173: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp238=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp238->tag != 22)goto _LL175;}_LL174:
 return 140;_LL175: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp239->tag != 23)goto _LL177;}_LL176:
 return 150;_LL177: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23A->tag != 24)goto _LL179;}_LL178:
 goto _LL17A;_LL179: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23B->tag != 25)goto _LL17B;}_LL17A:
 goto _LL17C;_LL17B: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp23C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23C->tag != 26)goto _LL17D;}_LL17C:
 goto _LL17E;_LL17D: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23D->tag != 27)goto _LL17F;}_LL17E:
 goto _LL180;_LL17F: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23E->tag != 28)goto _LL181;}_LL180:
 goto _LL182;_LL181: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp23F->tag != 29)goto _LL183;}_LL182:
 goto _LL184;_LL183: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp240->tag != 30)goto _LL185;}_LL184:
 goto _LL186;_LL185: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp241->tag != 31)goto _LL187;}_LL186:
 goto _LL188;_LL187: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp242->tag != 32)goto _LL189;}_LL188:
 goto _LL18A;_LL189: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp243->tag != 33)goto _LL18B;}_LL18A:
 goto _LL18C;_LL18B: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp244->tag != 34)goto _LL18D;}_LL18C:
 goto _LL18E;_LL18D: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp245->tag != 35)goto _LL18F;}_LL18E:
 return 140;_LL18F: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp21B;if(_tmp246->tag != 36)goto _LL140;}_LL190:
 return 10000;_LL140:;}
# 1075
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1079
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_stmttmp14=e->r;void*_tmp247=_stmttmp14;union Cyc_Absyn_Cnst _tmp249;struct _tuple0*_tmp24B;enum Cyc_Absyn_Primop _tmp24D;struct Cyc_List_List*_tmp24E;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Core_Opt*_tmp251;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Absyn_Exp*_tmp254;enum Cyc_Absyn_Incrementor _tmp255;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_List_List*_tmp265;struct Cyc_Absyn_Exp*_tmp267;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26B;void*_tmp26D;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp273;void*_tmp275;struct Cyc_Absyn_Exp*_tmp277;void*_tmp279;int _tmp27B;struct _dyneither_ptr _tmp27C;struct Cyc_Absyn_Exp*_tmp27E;struct _dyneither_ptr*_tmp27F;void*_tmp281;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Exp*_tmp284;struct Cyc_Absyn_Exp*_tmp286;struct _dyneither_ptr*_tmp287;struct Cyc_Absyn_Exp*_tmp289;struct _dyneither_ptr*_tmp28A;struct Cyc_Absyn_Exp*_tmp28C;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_List_List*_tmp28F;struct _tuple8*_tmp291;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp294;struct Cyc_Absyn_Vardecl*_tmp296;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Exp*_tmp298;struct Cyc_Absyn_Exp*_tmp29A;void*_tmp29B;struct _tuple0*_tmp29D;struct Cyc_List_List*_tmp29E;struct Cyc_List_List*_tmp29F;struct Cyc_List_List*_tmp2A1;struct Cyc_List_List*_tmp2A3;struct Cyc_Absyn_Datatypefield*_tmp2A4;struct _tuple0*_tmp2A6;struct _tuple0*_tmp2A8;int _tmp2AA;struct Cyc_Absyn_Exp*_tmp2AB;void**_tmp2AC;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2B0;struct Cyc_Core_Opt*_tmp2B2;struct Cyc_List_List*_tmp2B3;struct Cyc_Absyn_Stmt*_tmp2B5;_LL1A7: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp248->tag != 0)goto _LL1A9;else{_tmp249=_tmp248->f1;}}_LL1A8:
 s=Cyc_Absynpp_cnst2doc(_tmp249);goto _LL1A6;_LL1A9: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp24A->tag != 1)goto _LL1AB;else{_tmp24B=_tmp24A->f1;}}_LL1AA:
 s=Cyc_Absynpp_qvar2doc(_tmp24B);goto _LL1A6;_LL1AB: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp24C->tag != 2)goto _LL1AD;else{_tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;}}_LL1AC:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp24D,_tmp24E);goto _LL1A6;_LL1AD: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp24F=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp24F->tag != 3)goto _LL1AF;else{_tmp250=_tmp24F->f1;_tmp251=_tmp24F->f2;_tmp252=_tmp24F->f3;}}_LL1AE:
# 1087
{const char*_tmp750;const char*_tmp74F;struct Cyc_PP_Doc*_tmp74E[5];s=((_tmp74E[4]=
# 1091
Cyc_Absynpp_exp2doc_prec(myprec,_tmp252),((_tmp74E[3]=
# 1090
Cyc_PP_text(((_tmp74F="= ",_tag_dyneither(_tmp74F,sizeof(char),3)))),((_tmp74E[2]=
# 1089
_tmp251 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp251->v),((_tmp74E[1]=
# 1088
Cyc_PP_text(((_tmp750=" ",_tag_dyneither(_tmp750,sizeof(char),2)))),((_tmp74E[0]=
# 1087
Cyc_Absynpp_exp2doc_prec(myprec,_tmp250),Cyc_PP_cat(_tag_dyneither(_tmp74E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1092
goto _LL1A6;_LL1AF: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp253=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp253->tag != 4)goto _LL1B1;else{_tmp254=_tmp253->f1;_tmp255=_tmp253->f2;}}_LL1B0: {
# 1094
struct Cyc_PP_Doc*_tmp2B9=Cyc_Absynpp_exp2doc_prec(myprec,_tmp254);
switch(_tmp255){case Cyc_Absyn_PreInc: _LL1F7:
{const char*_tmp753;struct Cyc_PP_Doc*_tmp752[2];s=((_tmp752[1]=_tmp2B9,((_tmp752[0]=Cyc_PP_text(((_tmp753="++",_tag_dyneither(_tmp753,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp752,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F8:
{const char*_tmp756;struct Cyc_PP_Doc*_tmp755[2];s=((_tmp755[1]=_tmp2B9,((_tmp755[0]=Cyc_PP_text(((_tmp756="--",_tag_dyneither(_tmp756,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp755,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F9:
{const char*_tmp759;struct Cyc_PP_Doc*_tmp758[2];s=((_tmp758[1]=Cyc_PP_text(((_tmp759="++",_tag_dyneither(_tmp759,sizeof(char),3)))),((_tmp758[0]=_tmp2B9,Cyc_PP_cat(_tag_dyneither(_tmp758,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1FA:
{const char*_tmp75C;struct Cyc_PP_Doc*_tmp75B[2];s=((_tmp75B[1]=Cyc_PP_text(((_tmp75C="--",_tag_dyneither(_tmp75C,sizeof(char),3)))),((_tmp75B[0]=_tmp2B9,Cyc_PP_cat(_tag_dyneither(_tmp75B,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1101
goto _LL1A6;}_LL1B1: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp256=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp256->tag != 5)goto _LL1B3;else{_tmp257=_tmp256->f1;_tmp258=_tmp256->f2;_tmp259=_tmp256->f3;}}_LL1B2:
# 1103
{const char*_tmp761;const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[5];s=((_tmp75F[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp259),((_tmp75F[3]=Cyc_PP_text(((_tmp760=" : ",_tag_dyneither(_tmp760,sizeof(char),4)))),((_tmp75F[2]=
# 1103
Cyc_Absynpp_exp2doc_prec(0,_tmp258),((_tmp75F[1]=Cyc_PP_text(((_tmp761=" ? ",_tag_dyneither(_tmp761,sizeof(char),4)))),((_tmp75F[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp257),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1105
goto _LL1A6;_LL1B3: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp25A->tag != 6)goto _LL1B5;else{_tmp25B=_tmp25A->f1;_tmp25C=_tmp25A->f2;}}_LL1B4:
# 1107
{const char*_tmp764;struct Cyc_PP_Doc*_tmp763[3];s=((_tmp763[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25C),((_tmp763[1]=Cyc_PP_text(((_tmp764=" && ",_tag_dyneither(_tmp764,sizeof(char),5)))),((_tmp763[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),Cyc_PP_cat(_tag_dyneither(_tmp763,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B5: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp25D=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp25D->tag != 7)goto _LL1B7;else{_tmp25E=_tmp25D->f1;_tmp25F=_tmp25D->f2;}}_LL1B6:
# 1110
{const char*_tmp767;struct Cyc_PP_Doc*_tmp766[3];s=((_tmp766[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25F),((_tmp766[1]=Cyc_PP_text(((_tmp767=" || ",_tag_dyneither(_tmp767,sizeof(char),5)))),((_tmp766[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25E),Cyc_PP_cat(_tag_dyneither(_tmp766,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1B7: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp260=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp260->tag != 8)goto _LL1B9;else{_tmp261=_tmp260->f1;_tmp262=_tmp260->f2;}}_LL1B8:
# 1115
{const char*_tmp76E;const char*_tmp76D;const char*_tmp76C;struct Cyc_PP_Doc*_tmp76B[5];s=((_tmp76B[4]=Cyc_PP_text(((_tmp76C=")",_tag_dyneither(_tmp76C,sizeof(char),2)))),((_tmp76B[3]=Cyc_Absynpp_exp2doc(_tmp262),((_tmp76B[2]=Cyc_PP_text(((_tmp76D=", ",_tag_dyneither(_tmp76D,sizeof(char),3)))),((_tmp76B[1]=Cyc_Absynpp_exp2doc(_tmp261),((_tmp76B[0]=Cyc_PP_text(((_tmp76E="(",_tag_dyneither(_tmp76E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp76B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1B9: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp263=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp263->tag != 9)goto _LL1BB;else{_tmp264=_tmp263->f1;_tmp265=_tmp263->f2;}}_LL1BA:
# 1118
{const char*_tmp773;const char*_tmp772;struct Cyc_PP_Doc*_tmp771[4];s=((_tmp771[3]=
# 1121
Cyc_PP_text(((_tmp772=")",_tag_dyneither(_tmp772,sizeof(char),2)))),((_tmp771[2]=
# 1120
Cyc_Absynpp_exps2doc_prec(20,_tmp265),((_tmp771[1]=
# 1119
Cyc_PP_text(((_tmp773="(",_tag_dyneither(_tmp773,sizeof(char),2)))),((_tmp771[0]=
# 1118
Cyc_Absynpp_exp2doc_prec(myprec,_tmp264),Cyc_PP_cat(_tag_dyneither(_tmp771,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1122
goto _LL1A6;_LL1BB: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp266=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp266->tag != 10)goto _LL1BD;else{_tmp267=_tmp266->f1;}}_LL1BC:
# 1124
{const char*_tmp776;struct Cyc_PP_Doc*_tmp775[2];s=((_tmp775[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp267),((_tmp775[0]=Cyc_PP_text(((_tmp776="throw ",_tag_dyneither(_tmp776,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp775,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1BD: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp268->tag != 11)goto _LL1BF;else{_tmp269=_tmp268->f1;}}_LL1BE:
# 1127
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp269);
goto _LL1A6;_LL1BF: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp26A->tag != 12)goto _LL1C1;else{_tmp26B=_tmp26A->f1;}}_LL1C0:
# 1130
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26B);
goto _LL1A6;_LL1C1: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp26C->tag != 13)goto _LL1C3;else{_tmp26D=(void*)_tmp26C->f1;_tmp26E=_tmp26C->f2;}}_LL1C2:
# 1133
{const char*_tmp77B;const char*_tmp77A;struct Cyc_PP_Doc*_tmp779[4];s=((_tmp779[3]=
# 1136
Cyc_Absynpp_exp2doc_prec(myprec,_tmp26E),((_tmp779[2]=
# 1135
Cyc_PP_text(((_tmp77A=")",_tag_dyneither(_tmp77A,sizeof(char),2)))),((_tmp779[1]=
# 1134
Cyc_Absynpp_typ2doc(_tmp26D),((_tmp779[0]=
# 1133
Cyc_PP_text(((_tmp77B="(",_tag_dyneither(_tmp77B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp779,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1137
goto _LL1A6;_LL1C3: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp26F=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp26F->tag != 14)goto _LL1C5;else{_tmp270=_tmp26F->f1;}}_LL1C4:
# 1139
{const char*_tmp77E;struct Cyc_PP_Doc*_tmp77D[2];s=((_tmp77D[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),((_tmp77D[0]=
# 1139
Cyc_PP_text(((_tmp77E="&",_tag_dyneither(_tmp77E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp77D,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1141
goto _LL1A6;_LL1C5: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp271->tag != 15)goto _LL1C7;else{_tmp272=_tmp271->f1;_tmp273=_tmp271->f2;}}_LL1C6:
# 1143
 if(_tmp272 == 0){
const char*_tmp781;struct Cyc_PP_Doc*_tmp780[2];s=((_tmp780[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),((_tmp780[0]=Cyc_PP_text(((_tmp781="new ",_tag_dyneither(_tmp781,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp780,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1146
const char*_tmp786;const char*_tmp785;struct Cyc_PP_Doc*_tmp784[4];s=((_tmp784[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp273),((_tmp784[2]=
# 1146
Cyc_PP_text(((_tmp785=") ",_tag_dyneither(_tmp785,sizeof(char),3)))),((_tmp784[1]=Cyc_Absynpp_exp2doc(_tmp272),((_tmp784[0]=Cyc_PP_text(((_tmp786="rnew(",_tag_dyneither(_tmp786,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp784,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1148
goto _LL1A6;_LL1C7: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp274=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp274->tag != 16)goto _LL1C9;else{_tmp275=(void*)_tmp274->f1;}}_LL1C8:
{const char*_tmp78B;const char*_tmp78A;struct Cyc_PP_Doc*_tmp789[3];s=((_tmp789[2]=Cyc_PP_text(((_tmp78A=")",_tag_dyneither(_tmp78A,sizeof(char),2)))),((_tmp789[1]=Cyc_Absynpp_typ2doc(_tmp275),((_tmp789[0]=Cyc_PP_text(((_tmp78B="sizeof(",_tag_dyneither(_tmp78B,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp789,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1C9: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp276=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp276->tag != 17)goto _LL1CB;else{_tmp277=_tmp276->f1;}}_LL1CA:
{const char*_tmp790;const char*_tmp78F;struct Cyc_PP_Doc*_tmp78E[3];s=((_tmp78E[2]=Cyc_PP_text(((_tmp78F=")",_tag_dyneither(_tmp78F,sizeof(char),2)))),((_tmp78E[1]=Cyc_Absynpp_exp2doc(_tmp277),((_tmp78E[0]=Cyc_PP_text(((_tmp790="sizeof(",_tag_dyneither(_tmp790,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp78E,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CB: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp278->tag != 38)goto _LL1CD;else{_tmp279=(void*)_tmp278->f1;}}_LL1CC:
{const char*_tmp795;const char*_tmp794;struct Cyc_PP_Doc*_tmp793[3];s=((_tmp793[2]=Cyc_PP_text(((_tmp794=")",_tag_dyneither(_tmp794,sizeof(char),2)))),((_tmp793[1]=Cyc_Absynpp_typ2doc(_tmp279),((_tmp793[0]=Cyc_PP_text(((_tmp795="valueof(",_tag_dyneither(_tmp795,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp793,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL1A6;_LL1CD: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp27A->tag != 39)goto _LL1CF;else{_tmp27B=_tmp27A->f1;_tmp27C=_tmp27A->f2;}}_LL1CE:
# 1153
 if(_tmp27B){
const char*_tmp79C;const char*_tmp79B;const char*_tmp79A;struct Cyc_PP_Doc*_tmp799[4];s=((_tmp799[3]=Cyc_PP_text(((_tmp79A=")",_tag_dyneither(_tmp79A,sizeof(char),2)))),((_tmp799[2]=Cyc_PP_text(_tmp27C),((_tmp799[1]=Cyc_PP_text(((_tmp79B=" volatile (",_tag_dyneither(_tmp79B,sizeof(char),12)))),((_tmp799[0]=Cyc_PP_text(((_tmp79C="__asm__",_tag_dyneither(_tmp79C,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp799,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1156
const char*_tmp7A1;const char*_tmp7A0;struct Cyc_PP_Doc*_tmp79F[3];s=((_tmp79F[2]=Cyc_PP_text(((_tmp7A0=")",_tag_dyneither(_tmp7A0,sizeof(char),2)))),((_tmp79F[1]=Cyc_PP_text(_tmp27C),((_tmp79F[0]=Cyc_PP_text(((_tmp7A1="__asm__(",_tag_dyneither(_tmp7A1,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp79F,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1CF: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp27D=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp27D->tag != 37)goto _LL1D1;else{_tmp27E=_tmp27D->f1;_tmp27F=_tmp27D->f2;}}_LL1D0:
# 1159
{const char*_tmp7A8;const char*_tmp7A7;const char*_tmp7A6;struct Cyc_PP_Doc*_tmp7A5[5];s=((_tmp7A5[4]=Cyc_PP_text(((_tmp7A6=")",_tag_dyneither(_tmp7A6,sizeof(char),2)))),((_tmp7A5[3]=Cyc_PP_textptr(_tmp27F),((_tmp7A5[2]=Cyc_PP_text(((_tmp7A7=".",_tag_dyneither(_tmp7A7,sizeof(char),2)))),((_tmp7A5[1]=Cyc_Absynpp_exp2doc(_tmp27E),((_tmp7A5[0]=Cyc_PP_text(((_tmp7A8="tagcheck(",_tag_dyneither(_tmp7A8,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7A5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL1A6;_LL1D1: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp280=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp280->tag != 18)goto _LL1D3;else{_tmp281=(void*)_tmp280->f1;_tmp282=_tmp280->f2;}}_LL1D2:
# 1162
{const char*_tmp7AD;const char*_tmp7AC;struct Cyc_PP_Doc*_tmp7AB[3];s=((_tmp7AB[2]=Cyc_PP_text(((_tmp7AC=",",_tag_dyneither(_tmp7AC,sizeof(char),2)))),((_tmp7AB[1]=Cyc_Absynpp_typ2doc(_tmp281),((_tmp7AB[0]=Cyc_PP_text(((_tmp7AD="offsetof(",_tag_dyneither(_tmp7AD,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7AB,sizeof(struct Cyc_PP_Doc*),3))))))));}
for(0;_tmp282 != 0;_tmp282=_tmp282->tl){
void*_stmttmp15=(void*)_tmp282->hd;void*_tmp2F3=_stmttmp15;struct _dyneither_ptr*_tmp2F5;unsigned int _tmp2F7;_LL1FD: {struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp2F4=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp2F3;if(_tmp2F4->tag != 0)goto _LL1FF;else{_tmp2F5=_tmp2F4->f1;}}_LL1FE:
{const char*_tmp7B0;struct Cyc_PP_Doc*_tmp7AF[3];s=((_tmp7AF[2]=
_tmp282->tl != 0?Cyc_PP_text(((_tmp7B0=".",_tag_dyneither(_tmp7B0,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7AF[1]=
# 1165
Cyc_PP_textptr(_tmp2F5),((_tmp7AF[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7AF,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1167
goto _LL1FC;_LL1FF: {struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp2F6=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp2F3;if(_tmp2F6->tag != 1)goto _LL1FC;else{_tmp2F7=_tmp2F6->f1;}}_LL200:
{struct Cyc_Int_pa_PrintArg_struct _tmp7BA;void*_tmp7B9[1];const char*_tmp7B8;const char*_tmp7B7;struct Cyc_PP_Doc*_tmp7B6[3];s=((_tmp7B6[2]=
_tmp282->tl != 0?Cyc_PP_text(((_tmp7B7=".",_tag_dyneither(_tmp7B7,sizeof(char),2)))): Cyc_PP_nil_doc(),((_tmp7B6[1]=
# 1168
Cyc_PP_text((struct _dyneither_ptr)((_tmp7BA.tag=1,((_tmp7BA.f1=(unsigned long)((int)_tmp2F7),((_tmp7B9[0]=& _tmp7BA,Cyc_aprintf(((_tmp7B8="%d",_tag_dyneither(_tmp7B8,sizeof(char),3))),_tag_dyneither(_tmp7B9,sizeof(void*),1))))))))),((_tmp7B6[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7B6,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1170
goto _LL1FC;_LL1FC:;}
# 1172
{const char*_tmp7BD;struct Cyc_PP_Doc*_tmp7BC[2];s=((_tmp7BC[1]=Cyc_PP_text(((_tmp7BD=")",_tag_dyneither(_tmp7BD,sizeof(char),2)))),((_tmp7BC[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp7BC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D3: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp283=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp283->tag != 19)goto _LL1D5;else{_tmp284=_tmp283->f1;}}_LL1D4:
# 1175
{const char*_tmp7C0;struct Cyc_PP_Doc*_tmp7BF[2];s=((_tmp7BF[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp284),((_tmp7BF[0]=Cyc_PP_text(((_tmp7C0="*",_tag_dyneither(_tmp7C0,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7BF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL1A6;_LL1D5: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp285=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp285->tag != 20)goto _LL1D7;else{_tmp286=_tmp285->f1;_tmp287=_tmp285->f2;}}_LL1D6:
# 1178
{const char*_tmp7C3;struct Cyc_PP_Doc*_tmp7C2[3];s=((_tmp7C2[2]=Cyc_PP_textptr(_tmp287),((_tmp7C2[1]=Cyc_PP_text(((_tmp7C3=".",_tag_dyneither(_tmp7C3,sizeof(char),2)))),((_tmp7C2[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp286),Cyc_PP_cat(_tag_dyneither(_tmp7C2,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D7: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp288=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp288->tag != 21)goto _LL1D9;else{_tmp289=_tmp288->f1;_tmp28A=_tmp288->f2;}}_LL1D8:
# 1181
{const char*_tmp7C6;struct Cyc_PP_Doc*_tmp7C5[3];s=((_tmp7C5[2]=Cyc_PP_textptr(_tmp28A),((_tmp7C5[1]=Cyc_PP_text(((_tmp7C6="->",_tag_dyneither(_tmp7C6,sizeof(char),3)))),((_tmp7C5[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp289),Cyc_PP_cat(_tag_dyneither(_tmp7C5,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL1A6;_LL1D9: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp28B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp28B->tag != 22)goto _LL1DB;else{_tmp28C=_tmp28B->f1;_tmp28D=_tmp28B->f2;}}_LL1DA:
# 1184
{const char*_tmp7CB;const char*_tmp7CA;struct Cyc_PP_Doc*_tmp7C9[4];s=((_tmp7C9[3]=
# 1187
Cyc_PP_text(((_tmp7CA="]",_tag_dyneither(_tmp7CA,sizeof(char),2)))),((_tmp7C9[2]=
# 1186
Cyc_Absynpp_exp2doc(_tmp28D),((_tmp7C9[1]=
# 1185
Cyc_PP_text(((_tmp7CB="[",_tag_dyneither(_tmp7CB,sizeof(char),2)))),((_tmp7C9[0]=
# 1184
Cyc_Absynpp_exp2doc_prec(myprec,_tmp28C),Cyc_PP_cat(_tag_dyneither(_tmp7C9,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1188
goto _LL1A6;_LL1DB: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp28E=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp28E->tag != 23)goto _LL1DD;else{_tmp28F=_tmp28E->f1;}}_LL1DC:
# 1190
{const char*_tmp7D0;const char*_tmp7CF;struct Cyc_PP_Doc*_tmp7CE[4];s=((_tmp7CE[3]=
# 1193
Cyc_PP_text(((_tmp7CF=")",_tag_dyneither(_tmp7CF,sizeof(char),2)))),((_tmp7CE[2]=
# 1192
Cyc_Absynpp_exps2doc_prec(20,_tmp28F),((_tmp7CE[1]=
# 1191
Cyc_PP_text(((_tmp7D0="(",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CE[0]=
# 1190
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7CE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1194
goto _LL1A6;_LL1DD: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp290=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp290->tag != 24)goto _LL1DF;else{_tmp291=_tmp290->f1;_tmp292=_tmp290->f2;}}_LL1DE:
# 1196
{const char*_tmp7D7;const char*_tmp7D6;const char*_tmp7D5;struct Cyc_PP_Doc*_tmp7D4[4];s=((_tmp7D4[3]=
# 1199
Cyc_Absynpp_group_braces(((_tmp7D5=",",_tag_dyneither(_tmp7D5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp292)),((_tmp7D4[2]=
# 1198
Cyc_PP_text(((_tmp7D6=")",_tag_dyneither(_tmp7D6,sizeof(char),2)))),((_tmp7D4[1]=
# 1197
Cyc_Absynpp_typ2doc((*_tmp291).f3),((_tmp7D4[0]=
# 1196
Cyc_PP_text(((_tmp7D7="(",_tag_dyneither(_tmp7D7,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7D4,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1200
goto _LL1A6;_LL1DF: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp293=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp293->tag != 25)goto _LL1E1;else{_tmp294=_tmp293->f1;}}_LL1E0:
# 1202
{const char*_tmp7D8;s=Cyc_Absynpp_group_braces(((_tmp7D8=",",_tag_dyneither(_tmp7D8,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp294));}
goto _LL1A6;_LL1E1: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp295->tag != 26)goto _LL1E3;else{_tmp296=_tmp295->f1;_tmp297=_tmp295->f2;_tmp298=_tmp295->f3;}}_LL1E2:
# 1205
{const char*_tmp7DF;const char*_tmp7DE;const char*_tmp7DD;struct Cyc_PP_Doc*_tmp7DC[8];s=((_tmp7DC[7]=
# 1211
Cyc_Absynpp_rb(),((_tmp7DC[6]=
# 1210
Cyc_Absynpp_exp2doc(_tmp298),((_tmp7DC[5]=
# 1209
Cyc_PP_text(((_tmp7DD=" : ",_tag_dyneither(_tmp7DD,sizeof(char),4)))),((_tmp7DC[4]=
# 1208
Cyc_Absynpp_exp2doc(_tmp297),((_tmp7DC[3]=
# 1207
Cyc_PP_text(((_tmp7DE=" < ",_tag_dyneither(_tmp7DE,sizeof(char),4)))),((_tmp7DC[2]=
# 1206
Cyc_PP_text(*(*_tmp296->name).f2),((_tmp7DC[1]=
# 1205
Cyc_PP_text(((_tmp7DF="for ",_tag_dyneither(_tmp7DF,sizeof(char),5)))),((_tmp7DC[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7DC,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1212
goto _LL1A6;_LL1E3: {struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp299=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp299->tag != 27)goto _LL1E5;else{_tmp29A=_tmp299->f1;_tmp29B=(void*)_tmp299->f2;}}_LL1E4:
# 1214
{const char*_tmp7E6;const char*_tmp7E5;const char*_tmp7E4;struct Cyc_PP_Doc*_tmp7E3[7];s=((_tmp7E3[6]=
# 1219
Cyc_Absynpp_rb(),((_tmp7E3[5]=
# 1218
Cyc_Absynpp_typ2doc(_tmp29B),((_tmp7E3[4]=
# 1217
Cyc_PP_text(((_tmp7E4=" : ",_tag_dyneither(_tmp7E4,sizeof(char),4)))),((_tmp7E3[3]=
# 1216
Cyc_Absynpp_exp2doc(_tmp29A),((_tmp7E3[2]=
# 1215
Cyc_PP_text(((_tmp7E5=" < ",_tag_dyneither(_tmp7E5,sizeof(char),4)))),((_tmp7E3[1]=
# 1214
Cyc_PP_text(((_tmp7E6="for x ",_tag_dyneither(_tmp7E6,sizeof(char),7)))),((_tmp7E3[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7E3,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1220
goto _LL1A6;_LL1E5: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp29C=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp29C->tag != 28)goto _LL1E7;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;_tmp29F=_tmp29C->f3;}}_LL1E6: {
# 1222
struct Cyc_List_List*_tmp31A=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29F);
{const char*_tmp7EB;struct Cyc_List_List*_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[2];s=((_tmp7E9[1]=
Cyc_Absynpp_group_braces(((_tmp7EB=",",_tag_dyneither(_tmp7EB,sizeof(char),2))),
_tmp29E != 0?(_tmp7EA=_cycalloc(sizeof(*_tmp7EA)),((_tmp7EA->hd=Cyc_Absynpp_tps2doc(_tmp29E),((_tmp7EA->tl=_tmp31A,_tmp7EA))))): _tmp31A),((_tmp7E9[0]=
# 1223
Cyc_Absynpp_qvar2doc(_tmp29D),Cyc_PP_cat(_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1226
goto _LL1A6;}_LL1E7: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A0=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2A0->tag != 29)goto _LL1E9;else{_tmp2A1=_tmp2A0->f2;}}_LL1E8:
# 1228
{const char*_tmp7EC;s=Cyc_Absynpp_group_braces(((_tmp7EC=",",_tag_dyneither(_tmp7EC,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A1));}
goto _LL1A6;_LL1E9: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A2=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2A2->tag != 30)goto _LL1EB;else{_tmp2A3=_tmp2A2->f1;_tmp2A4=_tmp2A2->f3;}}_LL1EA:
# 1231
 if(_tmp2A3 == 0)
# 1233
s=Cyc_Absynpp_qvar2doc(_tmp2A4->name);else{
# 1235
const char*_tmp7F3;const char*_tmp7F2;const char*_tmp7F1;struct Cyc_PP_Doc*_tmp7F0[2];s=((_tmp7F0[1]=
Cyc_PP_egroup(((_tmp7F3="(",_tag_dyneither(_tmp7F3,sizeof(char),2))),((_tmp7F2=")",_tag_dyneither(_tmp7F2,sizeof(char),2))),((_tmp7F1=",",_tag_dyneither(_tmp7F1,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A3)),((_tmp7F0[0]=
# 1235
Cyc_Absynpp_qvar2doc(_tmp2A4->name),Cyc_PP_cat(_tag_dyneither(_tmp7F0,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1237
goto _LL1A6;_LL1EB: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2A5=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2A5->tag != 31)goto _LL1ED;else{_tmp2A6=_tmp2A5->f1;}}_LL1EC:
# 1239
 s=Cyc_Absynpp_qvar2doc(_tmp2A6);
goto _LL1A6;_LL1ED: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2A7=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2A7->tag != 32)goto _LL1EF;else{_tmp2A8=_tmp2A7->f1;}}_LL1EE:
# 1242
 s=Cyc_Absynpp_qvar2doc(_tmp2A8);
goto _LL1A6;_LL1EF: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2A9=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2A9->tag != 33)goto _LL1F1;else{_tmp2AA=(_tmp2A9->f1).is_calloc;_tmp2AB=(_tmp2A9->f1).rgn;_tmp2AC=(_tmp2A9->f1).elt_type;_tmp2AD=(_tmp2A9->f1).num_elts;}}_LL1F0:
# 1247
 if(_tmp2AA){
# 1249
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2AC)),0);
if(_tmp2AB == 0){
const char*_tmp7FA;const char*_tmp7F9;const char*_tmp7F8;struct Cyc_PP_Doc*_tmp7F7[5];s=((_tmp7F7[4]=Cyc_PP_text(((_tmp7F8=")",_tag_dyneither(_tmp7F8,sizeof(char),2)))),((_tmp7F7[3]=Cyc_Absynpp_exp2doc(st),((_tmp7F7[2]=Cyc_PP_text(((_tmp7F9=",",_tag_dyneither(_tmp7F9,sizeof(char),2)))),((_tmp7F7[1]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp7F7[0]=Cyc_PP_text(((_tmp7FA="calloc(",_tag_dyneither(_tmp7FA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7F7,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1253
const char*_tmp803;const char*_tmp802;const char*_tmp801;const char*_tmp800;struct Cyc_PP_Doc*_tmp7FF[7];s=((_tmp7FF[6]=
Cyc_PP_text(((_tmp800=")",_tag_dyneither(_tmp800,sizeof(char),2)))),((_tmp7FF[5]=Cyc_Absynpp_exp2doc(st),((_tmp7FF[4]=Cyc_PP_text(((_tmp801=",",_tag_dyneither(_tmp801,sizeof(char),2)))),((_tmp7FF[3]=Cyc_Absynpp_exp2doc(_tmp2AD),((_tmp7FF[2]=
# 1253
Cyc_PP_text(((_tmp802=",",_tag_dyneither(_tmp802,sizeof(char),2)))),((_tmp7FF[1]=Cyc_Absynpp_exp2doc(_tmp2AB),((_tmp7FF[0]=Cyc_PP_text(((_tmp803="rcalloc(",_tag_dyneither(_tmp803,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7FF,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1256
struct Cyc_Absyn_Exp*new_e;
# 1258
if(_tmp2AC == 0)
new_e=_tmp2AD;else{
# 1261
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2AC,0),_tmp2AD,0);}
# 1263
if(_tmp2AB == 0){
const char*_tmp808;const char*_tmp807;struct Cyc_PP_Doc*_tmp806[3];s=((_tmp806[2]=Cyc_PP_text(((_tmp807=")",_tag_dyneither(_tmp807,sizeof(char),2)))),((_tmp806[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp806[0]=Cyc_PP_text(((_tmp808="malloc(",_tag_dyneither(_tmp808,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp806,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1266
const char*_tmp80F;const char*_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[5];s=((_tmp80C[4]=
Cyc_PP_text(((_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80C[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp80C[2]=
# 1266
Cyc_PP_text(((_tmp80E=",",_tag_dyneither(_tmp80E,sizeof(char),2)))),((_tmp80C[1]=Cyc_Absynpp_exp2doc(_tmp2AB),((_tmp80C[0]=Cyc_PP_text(((_tmp80F="rmalloc(",_tag_dyneither(_tmp80F,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1269
goto _LL1A6;_LL1F1: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2AE=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2AE->tag != 34)goto _LL1F3;else{_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;}}_LL1F2:
# 1271
{const char*_tmp812;struct Cyc_PP_Doc*_tmp811[3];s=((_tmp811[2]=
# 1273
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B0),((_tmp811[1]=
# 1272
Cyc_PP_text(((_tmp812=" :=: ",_tag_dyneither(_tmp812,sizeof(char),6)))),((_tmp811[0]=
# 1271
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2AF),Cyc_PP_cat(_tag_dyneither(_tmp811,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1274
goto _LL1A6;_LL1F3: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2B1->tag != 35)goto _LL1F5;else{_tmp2B2=_tmp2B1->f1;_tmp2B3=_tmp2B1->f2;}}_LL1F4:
# 1277
{const char*_tmp813;s=Cyc_Absynpp_group_braces(((_tmp813=",",_tag_dyneither(_tmp813,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B3));}
goto _LL1A6;_LL1F5: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2B4=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp247;if(_tmp2B4->tag != 36)goto _LL1A6;else{_tmp2B5=_tmp2B4->f1;}}_LL1F6:
# 1280
{const char*_tmp818;const char*_tmp817;struct Cyc_PP_Doc*_tmp816[7];s=((_tmp816[6]=
# 1282
Cyc_PP_text(((_tmp817=")",_tag_dyneither(_tmp817,sizeof(char),2)))),((_tmp816[5]=Cyc_Absynpp_rb(),((_tmp816[4]=Cyc_PP_blank_doc(),((_tmp816[3]=
# 1281
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B5,1)),((_tmp816[2]=
# 1280
Cyc_PP_blank_doc(),((_tmp816[1]=Cyc_Absynpp_lb(),((_tmp816[0]=Cyc_PP_text(((_tmp818="(",_tag_dyneither(_tmp818,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp816,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1283
goto _LL1A6;_LL1A6:;}
# 1285
if(inprec >= myprec){
const char*_tmp81D;const char*_tmp81C;struct Cyc_PP_Doc*_tmp81B[3];s=((_tmp81B[2]=Cyc_PP_text(((_tmp81C=")",_tag_dyneither(_tmp81C,sizeof(char),2)))),((_tmp81B[1]=s,((_tmp81B[0]=Cyc_PP_text(((_tmp81D="(",_tag_dyneither(_tmp81D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp81B,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1290
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp33C=d;struct Cyc_Absyn_Exp*_tmp33E;struct _dyneither_ptr*_tmp340;_LL202: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp33D=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp33C;if(_tmp33D->tag != 0)goto _LL204;else{_tmp33E=_tmp33D->f1;}}_LL203: {
const char*_tmp822;const char*_tmp821;struct Cyc_PP_Doc*_tmp820[3];return(_tmp820[2]=Cyc_PP_text(((_tmp821="]",_tag_dyneither(_tmp821,sizeof(char),2)))),((_tmp820[1]=Cyc_Absynpp_exp2doc(_tmp33E),((_tmp820[0]=Cyc_PP_text(((_tmp822=".[",_tag_dyneither(_tmp822,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp820,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL204: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp33F=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp33C;if(_tmp33F->tag != 1)goto _LL201;else{_tmp340=_tmp33F->f1;}}_LL205: {
const char*_tmp825;struct Cyc_PP_Doc*_tmp824[2];return(_tmp824[1]=Cyc_PP_textptr(_tmp340),((_tmp824[0]=Cyc_PP_text(((_tmp825=".",_tag_dyneither(_tmp825,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp824,sizeof(struct Cyc_PP_Doc*),2)))));}_LL201:;}
# 1297
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp82C;const char*_tmp82B;const char*_tmp82A;struct Cyc_PP_Doc*_tmp829[2];return(_tmp829[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp829[0]=
# 1299
Cyc_PP_egroup(((_tmp82C="",_tag_dyneither(_tmp82C,sizeof(char),1))),((_tmp82B="=",_tag_dyneither(_tmp82B,sizeof(char),2))),((_tmp82A="=",_tag_dyneither(_tmp82A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp829,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1303
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp82F;const char*_tmp82E;const char*_tmp82D;return Cyc_PP_group(((_tmp82D="",_tag_dyneither(_tmp82D,sizeof(char),1))),((_tmp82E="",_tag_dyneither(_tmp82E,sizeof(char),1))),((_tmp82F=",",_tag_dyneither(_tmp82F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1308
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp34D=c;enum Cyc_Absyn_Sign _tmp34E;char _tmp34F;struct _dyneither_ptr _tmp350;enum Cyc_Absyn_Sign _tmp351;short _tmp352;int _tmp353;int _tmp354;int _tmp355;enum Cyc_Absyn_Sign _tmp356;long long _tmp357;struct _dyneither_ptr _tmp358;struct _dyneither_ptr _tmp359;struct _dyneither_ptr _tmp35A;_LL207: if((_tmp34D.Char_c).tag != 2)goto _LL209;_tmp34E=((struct _tuple3)(_tmp34D.Char_c).val).f1;_tmp34F=((struct _tuple3)(_tmp34D.Char_c).val).f2;_LL208: {
const char*_tmp833;void*_tmp832[1];struct Cyc_String_pa_PrintArg_struct _tmp831;return Cyc_PP_text((struct _dyneither_ptr)((_tmp831.tag=0,((_tmp831.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp34F)),((_tmp832[0]=& _tmp831,Cyc_aprintf(((_tmp833="'%s'",_tag_dyneither(_tmp833,sizeof(char),5))),_tag_dyneither(_tmp832,sizeof(void*),1)))))))));}_LL209: if((_tmp34D.Wchar_c).tag != 3)goto _LL20B;_tmp350=(struct _dyneither_ptr)(_tmp34D.Wchar_c).val;_LL20A: {
const char*_tmp837;void*_tmp836[1];struct Cyc_String_pa_PrintArg_struct _tmp835;return Cyc_PP_text((struct _dyneither_ptr)((_tmp835.tag=0,((_tmp835.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp350),((_tmp836[0]=& _tmp835,Cyc_aprintf(((_tmp837="L'%s'",_tag_dyneither(_tmp837,sizeof(char),6))),_tag_dyneither(_tmp836,sizeof(void*),1)))))))));}_LL20B: if((_tmp34D.Short_c).tag != 4)goto _LL20D;_tmp351=((struct _tuple4)(_tmp34D.Short_c).val).f1;_tmp352=((struct _tuple4)(_tmp34D.Short_c).val).f2;_LL20C: {
const char*_tmp83B;void*_tmp83A[1];struct Cyc_Int_pa_PrintArg_struct _tmp839;return Cyc_PP_text((struct _dyneither_ptr)((_tmp839.tag=1,((_tmp839.f1=(unsigned long)((int)_tmp352),((_tmp83A[0]=& _tmp839,Cyc_aprintf(((_tmp83B="%d",_tag_dyneither(_tmp83B,sizeof(char),3))),_tag_dyneither(_tmp83A,sizeof(void*),1)))))))));}_LL20D: if((_tmp34D.Int_c).tag != 5)goto _LL20F;if(((struct _tuple5)(_tmp34D.Int_c).val).f1 != Cyc_Absyn_None)goto _LL20F;_tmp353=((struct _tuple5)(_tmp34D.Int_c).val).f2;_LL20E:
 _tmp354=_tmp353;goto _LL210;_LL20F: if((_tmp34D.Int_c).tag != 5)goto _LL211;if(((struct _tuple5)(_tmp34D.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL211;_tmp354=((struct _tuple5)(_tmp34D.Int_c).val).f2;_LL210: {
const char*_tmp83F;void*_tmp83E[1];struct Cyc_Int_pa_PrintArg_struct _tmp83D;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83D.tag=1,((_tmp83D.f1=(unsigned long)_tmp354,((_tmp83E[0]=& _tmp83D,Cyc_aprintf(((_tmp83F="%d",_tag_dyneither(_tmp83F,sizeof(char),3))),_tag_dyneither(_tmp83E,sizeof(void*),1)))))))));}_LL211: if((_tmp34D.Int_c).tag != 5)goto _LL213;if(((struct _tuple5)(_tmp34D.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL213;_tmp355=((struct _tuple5)(_tmp34D.Int_c).val).f2;_LL212: {
const char*_tmp843;void*_tmp842[1];struct Cyc_Int_pa_PrintArg_struct _tmp841;return Cyc_PP_text((struct _dyneither_ptr)((_tmp841.tag=1,((_tmp841.f1=(unsigned int)_tmp355,((_tmp842[0]=& _tmp841,Cyc_aprintf(((_tmp843="%u",_tag_dyneither(_tmp843,sizeof(char),3))),_tag_dyneither(_tmp842,sizeof(void*),1)))))))));}_LL213: if((_tmp34D.LongLong_c).tag != 6)goto _LL215;_tmp356=((struct _tuple6)(_tmp34D.LongLong_c).val).f1;_tmp357=((struct _tuple6)(_tmp34D.LongLong_c).val).f2;_LL214: {
# 1317
const char*_tmp844;return Cyc_PP_text(((_tmp844="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp844,sizeof(char),27))));}_LL215: if((_tmp34D.Float_c).tag != 7)goto _LL217;_tmp358=((struct _tuple7)(_tmp34D.Float_c).val).f1;_LL216:
 return Cyc_PP_text(_tmp358);_LL217: if((_tmp34D.Null_c).tag != 1)goto _LL219;_LL218: {
const char*_tmp845;return Cyc_PP_text(((_tmp845="NULL",_tag_dyneither(_tmp845,sizeof(char),5))));}_LL219: if((_tmp34D.String_c).tag != 8)goto _LL21B;_tmp359=(struct _dyneither_ptr)(_tmp34D.String_c).val;_LL21A: {
# 1321
const char*_tmp84A;const char*_tmp849;struct Cyc_PP_Doc*_tmp848[3];return(_tmp848[2]=Cyc_PP_text(((_tmp849="\"",_tag_dyneither(_tmp849,sizeof(char),2)))),((_tmp848[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp359)),((_tmp848[0]=Cyc_PP_text(((_tmp84A="\"",_tag_dyneither(_tmp84A,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp848,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL21B: if((_tmp34D.Wstring_c).tag != 9)goto _LL206;_tmp35A=(struct _dyneither_ptr)(_tmp34D.Wstring_c).val;_LL21C: {
# 1323
const char*_tmp84F;const char*_tmp84E;struct Cyc_PP_Doc*_tmp84D[3];return(_tmp84D[2]=Cyc_PP_text(((_tmp84E="\"",_tag_dyneither(_tmp84E,sizeof(char),2)))),((_tmp84D[1]=Cyc_PP_text(_tmp35A),((_tmp84D[0]=Cyc_PP_text(((_tmp84F="L\"",_tag_dyneither(_tmp84F,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp84D,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL206:;}
# 1327
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp85C;const char*_tmp85B;void*_tmp85A[1];struct Cyc_String_pa_PrintArg_struct _tmp859;struct Cyc_Core_Failure_exn_struct*_tmp858;(int)_throw((void*)((_tmp858=_cycalloc(sizeof(*_tmp858)),((_tmp858[0]=((_tmp85C.tag=Cyc_Core_Failure,((_tmp85C.f1=(struct _dyneither_ptr)((_tmp859.tag=0,((_tmp859.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp85A[0]=& _tmp859,Cyc_aprintf(((_tmp85B="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp85B,sizeof(char),47))),_tag_dyneither(_tmp85A,sizeof(void*),1)))))))),_tmp85C)))),_tmp858)))));}{
# 1333
const char*_tmp861;const char*_tmp860;struct Cyc_PP_Doc*_tmp85F[3];return(_tmp85F[2]=Cyc_PP_text(((_tmp860=")",_tag_dyneither(_tmp860,sizeof(char),2)))),((_tmp85F[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp85F[0]=Cyc_PP_text(((_tmp861="numelts(",_tag_dyneither(_tmp861,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp85F,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1335
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp86E;const char*_tmp86D;void*_tmp86C[1];struct Cyc_String_pa_PrintArg_struct _tmp86B;struct Cyc_Core_Failure_exn_struct*_tmp86A;(int)_throw((void*)((_tmp86A=_cycalloc(sizeof(*_tmp86A)),((_tmp86A[0]=((_tmp86E.tag=Cyc_Core_Failure,((_tmp86E.f1=(struct _dyneither_ptr)((_tmp86B.tag=0,((_tmp86B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp86C[0]=& _tmp86B,Cyc_aprintf(((_tmp86D="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp86D,sizeof(char),38))),_tag_dyneither(_tmp86C,sizeof(void*),1)))))))),_tmp86E)))),_tmp86A)))));}else{
# 1339
if(ds->tl == 0){
const char*_tmp871;struct Cyc_PP_Doc*_tmp870[3];return(_tmp870[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp870[1]=Cyc_PP_text(((_tmp871=" ",_tag_dyneither(_tmp871,sizeof(char),2)))),((_tmp870[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp870,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp87E;const char*_tmp87D;void*_tmp87C[1];struct Cyc_String_pa_PrintArg_struct _tmp87B;struct Cyc_Core_Failure_exn_struct*_tmp87A;(int)_throw((void*)((_tmp87A=_cycalloc(sizeof(*_tmp87A)),((_tmp87A[0]=((_tmp87E.tag=Cyc_Core_Failure,((_tmp87E.f1=(struct _dyneither_ptr)((_tmp87B.tag=0,((_tmp87B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp87C[0]=& _tmp87B,Cyc_aprintf(((_tmp87D="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp87D,sizeof(char),47))),_tag_dyneither(_tmp87C,sizeof(void*),1)))))))),_tmp87E)))),_tmp87A)))));}else{
# 1345
const char*_tmp883;const char*_tmp882;struct Cyc_PP_Doc*_tmp881[5];return(_tmp881[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp881[3]=Cyc_PP_text(((_tmp882=" ",_tag_dyneither(_tmp882,sizeof(char),2)))),((_tmp881[2]=ps,((_tmp881[1]=Cyc_PP_text(((_tmp883=" ",_tag_dyneither(_tmp883,sizeof(char),2)))),((_tmp881[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp881,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1349
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL21D: {
const char*_tmp884;return(_tmp884="+",_tag_dyneither(_tmp884,sizeof(char),2));}case Cyc_Absyn_Times: _LL21E: {
const char*_tmp885;return(_tmp885="*",_tag_dyneither(_tmp885,sizeof(char),2));}case Cyc_Absyn_Minus: _LL21F: {
const char*_tmp886;return(_tmp886="-",_tag_dyneither(_tmp886,sizeof(char),2));}case Cyc_Absyn_Div: _LL220: {
const char*_tmp887;return(_tmp887="/",_tag_dyneither(_tmp887,sizeof(char),2));}case Cyc_Absyn_Mod: _LL221: {
const char*_tmp889;const char*_tmp888;return Cyc_Absynpp_print_for_cycdoc?(_tmp889="\\%",_tag_dyneither(_tmp889,sizeof(char),3)):((_tmp888="%",_tag_dyneither(_tmp888,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL222: {
const char*_tmp88A;return(_tmp88A="==",_tag_dyneither(_tmp88A,sizeof(char),3));}case Cyc_Absyn_Neq: _LL223: {
const char*_tmp88B;return(_tmp88B="!=",_tag_dyneither(_tmp88B,sizeof(char),3));}case Cyc_Absyn_Gt: _LL224: {
const char*_tmp88C;return(_tmp88C=">",_tag_dyneither(_tmp88C,sizeof(char),2));}case Cyc_Absyn_Lt: _LL225: {
const char*_tmp88D;return(_tmp88D="<",_tag_dyneither(_tmp88D,sizeof(char),2));}case Cyc_Absyn_Gte: _LL226: {
const char*_tmp88E;return(_tmp88E=">=",_tag_dyneither(_tmp88E,sizeof(char),3));}case Cyc_Absyn_Lte: _LL227: {
const char*_tmp88F;return(_tmp88F="<=",_tag_dyneither(_tmp88F,sizeof(char),3));}case Cyc_Absyn_Not: _LL228: {
const char*_tmp890;return(_tmp890="!",_tag_dyneither(_tmp890,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL229: {
const char*_tmp891;return(_tmp891="~",_tag_dyneither(_tmp891,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL22A: {
const char*_tmp892;return(_tmp892="&",_tag_dyneither(_tmp892,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL22B: {
const char*_tmp893;return(_tmp893="|",_tag_dyneither(_tmp893,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL22C: {
const char*_tmp894;return(_tmp894="^",_tag_dyneither(_tmp894,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL22D: {
const char*_tmp895;return(_tmp895="<<",_tag_dyneither(_tmp895,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL22E: {
const char*_tmp896;return(_tmp896=">>",_tag_dyneither(_tmp896,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL22F: {
const char*_tmp897;return(_tmp897=">>>",_tag_dyneither(_tmp897,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL230: {
const char*_tmp898;return(_tmp898="numelts",_tag_dyneither(_tmp898,sizeof(char),8));}}}
# 1374
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1378
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_stmttmp16=s->r;void*_tmp39E=_stmttmp16;_LL233: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp39F=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39E;if(_tmp39F->tag != 12)goto _LL235;}_LL234:
 return 1;_LL235:;_LL236:
 return 0;_LL232:;}
# 1385
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_stmttmp17=st->r;void*_tmp3A0=_stmttmp17;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_Absyn_Exp*_tmp3AA;struct Cyc_Absyn_Stmt*_tmp3AB;struct Cyc_Absyn_Stmt*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Stmt*_tmp3AF;struct _dyneither_ptr*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B7;struct Cyc_Absyn_Stmt*_tmp3B8;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*_tmp3BB;struct Cyc_List_List*_tmp3BE;struct Cyc_Absyn_Decl*_tmp3C0;struct Cyc_Absyn_Stmt*_tmp3C1;struct _dyneither_ptr*_tmp3C3;struct Cyc_Absyn_Stmt*_tmp3C4;struct Cyc_Absyn_Stmt*_tmp3C6;struct Cyc_Absyn_Exp*_tmp3C7;struct Cyc_Absyn_Stmt*_tmp3C9;struct Cyc_List_List*_tmp3CA;struct Cyc_Absyn_Exp*_tmp3CC;_LL238: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3A1=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A1->tag != 0)goto _LL23A;}_LL239:
{const char*_tmp899;s=Cyc_PP_text(((_tmp899=";",_tag_dyneither(_tmp899,sizeof(char),2))));}goto _LL237;_LL23A: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp3A2=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A2->tag != 1)goto _LL23C;else{_tmp3A3=_tmp3A2->f1;}}_LL23B:
{const char*_tmp89C;struct Cyc_PP_Doc*_tmp89B[2];s=((_tmp89B[1]=Cyc_PP_text(((_tmp89C=";",_tag_dyneither(_tmp89C,sizeof(char),2)))),((_tmp89B[0]=Cyc_Absynpp_exp2doc(_tmp3A3),Cyc_PP_cat(_tag_dyneither(_tmp89B,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL237;_LL23C: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp3A4=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A4->tag != 2)goto _LL23E;else{_tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;}}_LL23D:
# 1391
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp3A5)){
struct Cyc_PP_Doc*_tmp8A7[5];struct Cyc_PP_Doc*_tmp8A6[7];const char*_tmp8A5;const char*_tmp8A4;struct Cyc_PP_Doc*_tmp8A3[7];s=((_tmp8A3[6]=
# 1399
Cyc_Absynpp_is_declaration(_tmp3A6)?
stmtexp?
(_tmp8A6[6]=
# 1404
Cyc_PP_line_doc(),((_tmp8A6[5]=
# 1403
Cyc_PP_text(((_tmp8A5=");",_tag_dyneither(_tmp8A5,sizeof(char),3)))),((_tmp8A6[4]=Cyc_Absynpp_rb(),((_tmp8A6[3]=
# 1402
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp)),((_tmp8A6[2]=
# 1401
Cyc_PP_blank_doc(),((_tmp8A6[1]=Cyc_Absynpp_lb(),((_tmp8A6[0]=Cyc_PP_text(((_tmp8A4="(",_tag_dyneither(_tmp8A4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8A6,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1405
(_tmp8A7[4]=
# 1408
Cyc_PP_line_doc(),((_tmp8A7[3]=
# 1407
Cyc_Absynpp_rb(),((_tmp8A7[2]=
# 1406
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp)),((_tmp8A7[1]=
# 1405
Cyc_PP_blank_doc(),((_tmp8A7[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8A7,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1409
 Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp),((_tmp8A3[5]=
# 1398
Cyc_PP_line_doc(),((_tmp8A3[4]=
# 1397
Cyc_Absynpp_rb(),((_tmp8A3[3]=
# 1396
Cyc_PP_line_doc(),((_tmp8A3[2]=
# 1395
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A5,0)),((_tmp8A3[1]=
# 1394
Cyc_PP_blank_doc(),((_tmp8A3[0]=
# 1393
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8A3,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1410
if(Cyc_Absynpp_is_declaration(_tmp3A6)){
struct Cyc_PP_Doc*_tmp8B2[4];struct Cyc_PP_Doc*_tmp8B1[6];const char*_tmp8B0;const char*_tmp8AF;struct Cyc_PP_Doc*_tmp8AE[4];s=((_tmp8AE[3]=
# 1421
Cyc_PP_line_doc(),((_tmp8AE[2]=
# 1413
stmtexp?
(_tmp8B1[5]=
# 1416
Cyc_PP_text(((_tmp8B0=");",_tag_dyneither(_tmp8B0,sizeof(char),3)))),((_tmp8B1[4]=Cyc_Absynpp_rb(),((_tmp8B1[3]=
# 1415
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp)),((_tmp8B1[2]=
# 1414
Cyc_PP_blank_doc(),((_tmp8B1[1]=Cyc_Absynpp_lb(),((_tmp8B1[0]=Cyc_PP_text(((_tmp8AF="(",_tag_dyneither(_tmp8AF,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8B1,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1418
(_tmp8B2[3]=
# 1420
Cyc_Absynpp_rb(),((_tmp8B2[2]=
# 1419
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp)),((_tmp8B2[1]=
# 1418
Cyc_PP_blank_doc(),((_tmp8B2[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B2,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8AE[1]=
# 1412
Cyc_PP_line_doc(),((_tmp8AE[0]=
# 1411
Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(_tag_dyneither(_tmp8AE,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1423
struct Cyc_PP_Doc*_tmp8B3[3];s=((_tmp8B3[2]=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp),((_tmp8B3[1]=Cyc_PP_line_doc(),((_tmp8B3[0]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(_tag_dyneither(_tmp8B3,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1426
struct Cyc_PP_Doc*_tmp8B4[3];s=((_tmp8B4[2]=Cyc_Absynpp_stmt2doc(_tmp3A6,stmtexp),((_tmp8B4[1]=Cyc_PP_line_doc(),((_tmp8B4[0]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),Cyc_PP_cat(_tag_dyneither(_tmp8B4,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL23E: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A7=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A7->tag != 3)goto _LL240;else{_tmp3A8=_tmp3A7->f1;}}_LL23F:
# 1429
 if(_tmp3A8 == 0){
const char*_tmp8B5;s=Cyc_PP_text(((_tmp8B5="return;",_tag_dyneither(_tmp8B5,sizeof(char),8))));}else{
# 1432
const char*_tmp8BA;const char*_tmp8B9;struct Cyc_PP_Doc*_tmp8B8[3];s=((_tmp8B8[2]=
# 1434
Cyc_PP_text(((_tmp8B9=";",_tag_dyneither(_tmp8B9,sizeof(char),2)))),((_tmp8B8[1]=
# 1433
_tmp3A8 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc(_tmp3A8),((_tmp8B8[0]=
# 1432
Cyc_PP_text(((_tmp8BA="return ",_tag_dyneither(_tmp8BA,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8B8,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1435
goto _LL237;_LL240: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3A9=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3A9->tag != 4)goto _LL242;else{_tmp3AA=_tmp3A9->f1;_tmp3AB=_tmp3A9->f2;_tmp3AC=_tmp3A9->f3;}}_LL241: {
# 1437
int print_else;
{void*_stmttmp18=_tmp3AC->r;void*_tmp3E0=_stmttmp18;_LL25D: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3E1=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3E0;if(_tmp3E1->tag != 0)goto _LL25F;}_LL25E:
 print_else=0;goto _LL25C;_LL25F:;_LL260:
 print_else=1;goto _LL25C;_LL25C:;}
# 1442
{const char*_tmp8C9;const char*_tmp8C8;struct Cyc_PP_Doc*_tmp8C7[2];struct Cyc_PP_Doc*_tmp8C6[6];struct Cyc_PP_Doc*_tmp8C5[2];const char*_tmp8C4;struct Cyc_PP_Doc*_tmp8C3[8];s=((_tmp8C3[7]=
# 1449
print_else?
(_tmp8C6[5]=
# 1455
Cyc_Absynpp_rb(),((_tmp8C6[4]=
# 1454
Cyc_PP_line_doc(),((_tmp8C6[3]=
# 1453
Cyc_PP_nest(2,((_tmp8C5[1]=Cyc_Absynpp_stmt2doc(_tmp3AC,0),((_tmp8C5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C6[2]=
# 1452
Cyc_Absynpp_lb(),((_tmp8C6[1]=
# 1451
Cyc_PP_text(((_tmp8C4="else ",_tag_dyneither(_tmp8C4,sizeof(char),6)))),((_tmp8C6[0]=
# 1450
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C6,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1456
 Cyc_PP_nil_doc(),((_tmp8C3[6]=
# 1448
Cyc_Absynpp_rb(),((_tmp8C3[5]=
# 1447
Cyc_PP_line_doc(),((_tmp8C3[4]=
# 1446
Cyc_PP_nest(2,((_tmp8C7[1]=Cyc_Absynpp_stmt2doc(_tmp3AB,0),((_tmp8C7[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8C7,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C3[3]=
# 1445
Cyc_Absynpp_lb(),((_tmp8C3[2]=
# 1444
Cyc_PP_text(((_tmp8C8=") ",_tag_dyneither(_tmp8C8,sizeof(char),3)))),((_tmp8C3[1]=
# 1443
Cyc_Absynpp_exp2doc(_tmp3AA),((_tmp8C3[0]=
# 1442
Cyc_PP_text(((_tmp8C9="if (",_tag_dyneither(_tmp8C9,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8C3,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1457
goto _LL237;}_LL242: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3AD=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3AD->tag != 5)goto _LL244;else{_tmp3AE=(_tmp3AD->f1).f1;_tmp3AF=_tmp3AD->f2;}}_LL243:
# 1459
{const char*_tmp8D0;const char*_tmp8CF;struct Cyc_PP_Doc*_tmp8CE[2];struct Cyc_PP_Doc*_tmp8CD[7];s=((_tmp8CD[6]=
# 1465
Cyc_Absynpp_rb(),((_tmp8CD[5]=
# 1464
Cyc_PP_line_doc(),((_tmp8CD[4]=
# 1463
Cyc_PP_nest(2,((_tmp8CE[1]=Cyc_Absynpp_stmt2doc(_tmp3AF,0),((_tmp8CE[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CE,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CD[3]=
# 1462
Cyc_Absynpp_lb(),((_tmp8CD[2]=
# 1461
Cyc_PP_text(((_tmp8CF=") ",_tag_dyneither(_tmp8CF,sizeof(char),3)))),((_tmp8CD[1]=
# 1460
Cyc_Absynpp_exp2doc(_tmp3AE),((_tmp8CD[0]=
# 1459
Cyc_PP_text(((_tmp8D0="while (",_tag_dyneither(_tmp8D0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8CD,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1466
goto _LL237;_LL244: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3B0=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3B0->tag != 6)goto _LL246;}_LL245:
{const char*_tmp8D1;s=Cyc_PP_text(((_tmp8D1="break;",_tag_dyneither(_tmp8D1,sizeof(char),7))));}goto _LL237;_LL246: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3B1=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3B1->tag != 7)goto _LL248;}_LL247:
{const char*_tmp8D2;s=Cyc_PP_text(((_tmp8D2="continue;",_tag_dyneither(_tmp8D2,sizeof(char),10))));}goto _LL237;_LL248: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3B2=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3B2->tag != 8)goto _LL24A;else{_tmp3B3=_tmp3B2->f1;}}_LL249:
{const char*_tmp8D6;void*_tmp8D5[1];struct Cyc_String_pa_PrintArg_struct _tmp8D4;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8D4.tag=0,((_tmp8D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3B3),((_tmp8D5[0]=& _tmp8D4,Cyc_aprintf(((_tmp8D6="goto %s;",_tag_dyneither(_tmp8D6,sizeof(char),9))),_tag_dyneither(_tmp8D5,sizeof(void*),1)))))))));}goto _LL237;_LL24A: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3B4=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3B4->tag != 9)goto _LL24C;else{_tmp3B5=_tmp3B4->f1;_tmp3B6=(_tmp3B4->f2).f1;_tmp3B7=(_tmp3B4->f3).f1;_tmp3B8=_tmp3B4->f4;}}_LL24B:
# 1471
{const char*_tmp8E1;const char*_tmp8E0;const char*_tmp8DF;const char*_tmp8DE;struct Cyc_PP_Doc*_tmp8DD[2];struct Cyc_PP_Doc*_tmp8DC[11];s=((_tmp8DC[10]=
# 1481
Cyc_Absynpp_rb(),((_tmp8DC[9]=
# 1480
Cyc_PP_line_doc(),((_tmp8DC[8]=
# 1479
Cyc_PP_nest(2,((_tmp8DD[1]=Cyc_Absynpp_stmt2doc(_tmp3B8,0),((_tmp8DD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8DD,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8DC[7]=
# 1478
Cyc_Absynpp_lb(),((_tmp8DC[6]=
# 1477
Cyc_PP_text(((_tmp8DE=") ",_tag_dyneither(_tmp8DE,sizeof(char),3)))),((_tmp8DC[5]=
# 1476
Cyc_Absynpp_exp2doc(_tmp3B7),((_tmp8DC[4]=
# 1475
Cyc_PP_text(((_tmp8DF="; ",_tag_dyneither(_tmp8DF,sizeof(char),3)))),((_tmp8DC[3]=
# 1474
Cyc_Absynpp_exp2doc(_tmp3B6),((_tmp8DC[2]=
# 1473
Cyc_PP_text(((_tmp8E0="; ",_tag_dyneither(_tmp8E0,sizeof(char),3)))),((_tmp8DC[1]=
# 1472
Cyc_Absynpp_exp2doc(_tmp3B5),((_tmp8DC[0]=
# 1471
Cyc_PP_text(((_tmp8E1="for(",_tag_dyneither(_tmp8E1,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8DC,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1482
goto _LL237;_LL24C: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3B9->tag != 10)goto _LL24E;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}_LL24D:
# 1484
{const char*_tmp8E6;const char*_tmp8E5;struct Cyc_PP_Doc*_tmp8E4[8];s=((_tmp8E4[7]=
# 1491
Cyc_Absynpp_rb(),((_tmp8E4[6]=
# 1490
Cyc_PP_line_doc(),((_tmp8E4[5]=
# 1489
Cyc_Absynpp_switchclauses2doc(_tmp3BB),((_tmp8E4[4]=
# 1488
Cyc_PP_line_doc(),((_tmp8E4[3]=
# 1487
Cyc_Absynpp_lb(),((_tmp8E4[2]=
# 1486
Cyc_PP_text(((_tmp8E5=") ",_tag_dyneither(_tmp8E5,sizeof(char),3)))),((_tmp8E4[1]=
# 1485
Cyc_Absynpp_exp2doc(_tmp3BA),((_tmp8E4[0]=
# 1484
Cyc_PP_text(((_tmp8E6="switch (",_tag_dyneither(_tmp8E6,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8E4,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1492
goto _LL237;_LL24E: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3BC=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3BC->tag != 11)goto _LL250;else{if(_tmp3BC->f1 != 0)goto _LL250;}}_LL24F:
{const char*_tmp8E7;s=Cyc_PP_text(((_tmp8E7="fallthru;",_tag_dyneither(_tmp8E7,sizeof(char),10))));}goto _LL237;_LL250: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3BD=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3BD->tag != 11)goto _LL252;else{_tmp3BE=_tmp3BD->f1;}}_LL251:
# 1495
{const char*_tmp8EC;const char*_tmp8EB;struct Cyc_PP_Doc*_tmp8EA[3];s=((_tmp8EA[2]=Cyc_PP_text(((_tmp8EB=");",_tag_dyneither(_tmp8EB,sizeof(char),3)))),((_tmp8EA[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3BE),((_tmp8EA[0]=Cyc_PP_text(((_tmp8EC="fallthru(",_tag_dyneither(_tmp8EC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8EA,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL237;_LL252: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3BF=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3BF->tag != 12)goto _LL254;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=_tmp3BF->f2;}}_LL253:
# 1497
{struct Cyc_PP_Doc*_tmp8ED[3];s=((_tmp8ED[2]=
# 1499
Cyc_Absynpp_stmt2doc(_tmp3C1,stmtexp),((_tmp8ED[1]=
# 1498
Cyc_PP_line_doc(),((_tmp8ED[0]=
# 1497
Cyc_Absynpp_decl2doc(_tmp3C0),Cyc_PP_cat(_tag_dyneither(_tmp8ED,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1500
goto _LL237;_LL254: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3C2=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3C2->tag != 13)goto _LL256;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL255:
# 1502
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3C4)){
if(stmtexp){
const char*_tmp8F2;const char*_tmp8F1;struct Cyc_PP_Doc*_tmp8F0[8];s=((_tmp8F0[7]=
# 1510
Cyc_PP_text(((_tmp8F1=");",_tag_dyneither(_tmp8F1,sizeof(char),3)))),((_tmp8F0[6]=Cyc_Absynpp_rb(),((_tmp8F0[5]=
# 1509
Cyc_PP_line_doc(),((_tmp8F0[4]=
# 1508
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C4,1)),((_tmp8F0[3]=
# 1507
Cyc_PP_line_doc(),((_tmp8F0[2]=
# 1506
Cyc_Absynpp_lb(),((_tmp8F0[1]=
# 1505
Cyc_PP_text(((_tmp8F2=": (",_tag_dyneither(_tmp8F2,sizeof(char),4)))),((_tmp8F0[0]=
# 1504
Cyc_PP_textptr(_tmp3C3),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1512
const char*_tmp8F5;struct Cyc_PP_Doc*_tmp8F4[7];s=((_tmp8F4[6]=
# 1518
Cyc_Absynpp_rb(),((_tmp8F4[5]=
# 1517
Cyc_PP_line_doc(),((_tmp8F4[4]=
# 1516
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C4,0)),((_tmp8F4[3]=
# 1515
Cyc_PP_line_doc(),((_tmp8F4[2]=
# 1514
Cyc_Absynpp_lb(),((_tmp8F4[1]=
# 1513
Cyc_PP_text(((_tmp8F5=": ",_tag_dyneither(_tmp8F5,sizeof(char),3)))),((_tmp8F4[0]=
# 1512
Cyc_PP_textptr(_tmp3C3),Cyc_PP_cat(_tag_dyneither(_tmp8F4,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1520
const char*_tmp8F8;struct Cyc_PP_Doc*_tmp8F7[3];s=((_tmp8F7[2]=Cyc_Absynpp_stmt2doc(_tmp3C4,stmtexp),((_tmp8F7[1]=Cyc_PP_text(((_tmp8F8=": ",_tag_dyneither(_tmp8F8,sizeof(char),3)))),((_tmp8F7[0]=Cyc_PP_textptr(_tmp3C3),Cyc_PP_cat(_tag_dyneither(_tmp8F7,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL256: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3C5=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3C5->tag != 14)goto _LL258;else{_tmp3C6=_tmp3C5->f1;_tmp3C7=(_tmp3C5->f2).f1;}}_LL257:
# 1523
{const char*_tmp8FF;const char*_tmp8FE;const char*_tmp8FD;struct Cyc_PP_Doc*_tmp8FC[9];s=((_tmp8FC[8]=
# 1531
Cyc_PP_text(((_tmp8FD=");",_tag_dyneither(_tmp8FD,sizeof(char),3)))),((_tmp8FC[7]=
# 1530
Cyc_Absynpp_exp2doc(_tmp3C7),((_tmp8FC[6]=
# 1529
Cyc_PP_text(((_tmp8FE=" while (",_tag_dyneither(_tmp8FE,sizeof(char),9)))),((_tmp8FC[5]=
# 1528
Cyc_Absynpp_rb(),((_tmp8FC[4]=
# 1527
Cyc_PP_line_doc(),((_tmp8FC[3]=
# 1526
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C6,0)),((_tmp8FC[2]=
# 1525
Cyc_PP_line_doc(),((_tmp8FC[1]=
# 1524
Cyc_Absynpp_lb(),((_tmp8FC[0]=
# 1523
Cyc_PP_text(((_tmp8FF="do ",_tag_dyneither(_tmp8FF,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp8FC,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1532
goto _LL237;_LL258: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3C8=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3C8->tag != 15)goto _LL25A;else{_tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;}}_LL259:
# 1534
{const char*_tmp904;const char*_tmp903;struct Cyc_PP_Doc*_tmp902[12];s=((_tmp902[11]=
# 1545
Cyc_Absynpp_rb(),((_tmp902[10]=
# 1544
Cyc_PP_line_doc(),((_tmp902[9]=
# 1543
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3CA)),((_tmp902[8]=
# 1542
Cyc_PP_line_doc(),((_tmp902[7]=
# 1541
Cyc_Absynpp_lb(),((_tmp902[6]=
# 1540
Cyc_PP_text(((_tmp903=" catch ",_tag_dyneither(_tmp903,sizeof(char),8)))),((_tmp902[5]=
# 1539
Cyc_Absynpp_rb(),((_tmp902[4]=
# 1538
Cyc_PP_line_doc(),((_tmp902[3]=
# 1537
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C9,0)),((_tmp902[2]=
# 1536
Cyc_PP_line_doc(),((_tmp902[1]=
# 1535
Cyc_Absynpp_lb(),((_tmp902[0]=
# 1534
Cyc_PP_text(((_tmp904="try ",_tag_dyneither(_tmp904,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp902,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1546
goto _LL237;_LL25A: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3CB=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp3A0;if(_tmp3CB->tag != 16)goto _LL237;else{_tmp3CC=_tmp3CB->f1;}}_LL25B:
# 1548
{const char*_tmp909;const char*_tmp908;struct Cyc_PP_Doc*_tmp907[3];s=((_tmp907[2]=Cyc_PP_text(((_tmp908=");",_tag_dyneither(_tmp908,sizeof(char),3)))),((_tmp907[1]=Cyc_Absynpp_exp2doc(_tmp3CC),((_tmp907[0]=Cyc_PP_text(((_tmp909="reset_region(",_tag_dyneither(_tmp909,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp907,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL237;_LL237:;}
# 1551
return s;}
# 1554
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_stmttmp19=p->r;void*_tmp411=_stmttmp19;enum Cyc_Absyn_Sign _tmp415;int _tmp416;char _tmp418;struct _dyneither_ptr _tmp41A;struct Cyc_Absyn_Vardecl*_tmp41C;struct Cyc_Absyn_Vardecl*_tmp41F;struct Cyc_Absyn_Pat*_tmp420;struct Cyc_Absyn_Tvar*_tmp422;struct Cyc_Absyn_Vardecl*_tmp423;struct Cyc_List_List*_tmp425;int _tmp426;struct Cyc_Absyn_Pat*_tmp428;struct Cyc_Absyn_Vardecl*_tmp42A;struct Cyc_Absyn_Vardecl*_tmp42D;struct Cyc_Absyn_Pat*_tmp42E;struct _tuple0*_tmp430;struct _tuple0*_tmp432;struct Cyc_List_List*_tmp433;int _tmp434;union Cyc_Absyn_AggrInfoU _tmp436;struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp438;int _tmp439;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*_tmp43C;int _tmp43D;struct Cyc_Absyn_Enumfield*_tmp43F;struct Cyc_Absyn_Enumfield*_tmp441;struct Cyc_Absyn_Datatypefield*_tmp443;struct Cyc_Absyn_Datatypefield*_tmp445;struct Cyc_List_List*_tmp446;int _tmp447;struct Cyc_Absyn_Exp*_tmp449;_LL262: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp412=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp412->tag != 0)goto _LL264;}_LL263:
{const char*_tmp90A;s=Cyc_PP_text(((_tmp90A="_",_tag_dyneither(_tmp90A,sizeof(char),2))));}goto _LL261;_LL264: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp413=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp413->tag != 8)goto _LL266;}_LL265:
{const char*_tmp90B;s=Cyc_PP_text(((_tmp90B="NULL",_tag_dyneither(_tmp90B,sizeof(char),5))));}goto _LL261;_LL266: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp414=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp414->tag != 9)goto _LL268;else{_tmp415=_tmp414->f1;_tmp416=_tmp414->f2;}}_LL267:
# 1560
 if(_tmp415 != Cyc_Absyn_Unsigned){
const char*_tmp90F;void*_tmp90E[1];struct Cyc_Int_pa_PrintArg_struct _tmp90D;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp90D.tag=1,((_tmp90D.f1=(unsigned long)_tmp416,((_tmp90E[0]=& _tmp90D,Cyc_aprintf(((_tmp90F="%d",_tag_dyneither(_tmp90F,sizeof(char),3))),_tag_dyneither(_tmp90E,sizeof(void*),1)))))))));}else{
const char*_tmp913;void*_tmp912[1];struct Cyc_Int_pa_PrintArg_struct _tmp911;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp911.tag=1,((_tmp911.f1=(unsigned int)_tmp416,((_tmp912[0]=& _tmp911,Cyc_aprintf(((_tmp913="%u",_tag_dyneither(_tmp913,sizeof(char),3))),_tag_dyneither(_tmp912,sizeof(void*),1)))))))));}
goto _LL261;_LL268: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp417=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp417->tag != 10)goto _LL26A;else{_tmp418=_tmp417->f1;}}_LL269:
{const char*_tmp917;void*_tmp916[1];struct Cyc_String_pa_PrintArg_struct _tmp915;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp915.tag=0,((_tmp915.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp418)),((_tmp916[0]=& _tmp915,Cyc_aprintf(((_tmp917="'%s'",_tag_dyneither(_tmp917,sizeof(char),5))),_tag_dyneither(_tmp916,sizeof(void*),1)))))))));}goto _LL261;_LL26A: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp419=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp419->tag != 11)goto _LL26C;else{_tmp41A=_tmp419->f1;}}_LL26B:
 s=Cyc_PP_text(_tmp41A);goto _LL261;_LL26C: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41B=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp41B->tag != 1)goto _LL26E;else{_tmp41C=_tmp41B->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp41D=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp41B->f2)->r;if(_tmp41D->tag != 0)goto _LL26E;};}}_LL26D:
# 1567
 s=Cyc_Absynpp_qvar2doc(_tmp41C->name);goto _LL261;_LL26E: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp41E=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp41E->tag != 1)goto _LL270;else{_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;}}_LL26F:
# 1570
{const char*_tmp91A;struct Cyc_PP_Doc*_tmp919[3];s=((_tmp919[2]=Cyc_Absynpp_pat2doc(_tmp420),((_tmp919[1]=Cyc_PP_text(((_tmp91A=" as ",_tag_dyneither(_tmp91A,sizeof(char),5)))),((_tmp919[0]=Cyc_Absynpp_qvar2doc(_tmp41F->name),Cyc_PP_cat(_tag_dyneither(_tmp919,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL261;_LL270: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp421=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp421->tag != 3)goto _LL272;else{_tmp422=_tmp421->f1;_tmp423=_tmp421->f2;}}_LL271:
# 1572
{const char*_tmp91F;const char*_tmp91E;struct Cyc_PP_Doc*_tmp91D[4];s=((_tmp91D[3]=Cyc_PP_text(((_tmp91E=">",_tag_dyneither(_tmp91E,sizeof(char),2)))),((_tmp91D[2]=Cyc_Absynpp_tvar2doc(_tmp422),((_tmp91D[1]=Cyc_PP_text(((_tmp91F="<",_tag_dyneither(_tmp91F,sizeof(char),2)))),((_tmp91D[0]=Cyc_Absynpp_qvar2doc(_tmp423->name),Cyc_PP_cat(_tag_dyneither(_tmp91D,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL272: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp424=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp424->tag != 4)goto _LL274;else{_tmp425=_tmp424->f1;_tmp426=_tmp424->f2;}}_LL273:
# 1575
{const char*_tmp928;const char*_tmp927;const char*_tmp926;const char*_tmp925;struct Cyc_PP_Doc*_tmp924[4];s=((_tmp924[3]=
_tmp426?Cyc_PP_text(((_tmp925=", ...)",_tag_dyneither(_tmp925,sizeof(char),7)))): Cyc_PP_text(((_tmp926=")",_tag_dyneither(_tmp926,sizeof(char),2)))),((_tmp924[2]=
# 1575
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp927=",",_tag_dyneither(_tmp927,sizeof(char),2))),_tmp425),((_tmp924[1]=Cyc_PP_text(((_tmp928="(",_tag_dyneither(_tmp928,sizeof(char),2)))),((_tmp924[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp924,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1577
goto _LL261;_LL274: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp427=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp427->tag != 5)goto _LL276;else{_tmp428=_tmp427->f1;}}_LL275:
# 1579
{const char*_tmp92B;struct Cyc_PP_Doc*_tmp92A[2];s=((_tmp92A[1]=Cyc_Absynpp_pat2doc(_tmp428),((_tmp92A[0]=Cyc_PP_text(((_tmp92B="&",_tag_dyneither(_tmp92B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp92A,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL276: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp429=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp429->tag != 2)goto _LL278;else{_tmp42A=_tmp429->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp42B=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(_tmp429->f2)->r;if(_tmp42B->tag != 0)goto _LL278;};}}_LL277:
# 1582
{const char*_tmp92E;struct Cyc_PP_Doc*_tmp92D[2];s=((_tmp92D[1]=Cyc_Absynpp_qvar2doc(_tmp42A->name),((_tmp92D[0]=Cyc_PP_text(((_tmp92E="*",_tag_dyneither(_tmp92E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp92D,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;_LL278: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp42C=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp42C->tag != 2)goto _LL27A;else{_tmp42D=_tmp42C->f1;_tmp42E=_tmp42C->f2;}}_LL279:
# 1585
{const char*_tmp933;const char*_tmp932;struct Cyc_PP_Doc*_tmp931[4];s=((_tmp931[3]=Cyc_Absynpp_pat2doc(_tmp42E),((_tmp931[2]=Cyc_PP_text(((_tmp932=" as ",_tag_dyneither(_tmp932,sizeof(char),5)))),((_tmp931[1]=Cyc_Absynpp_qvar2doc(_tmp42D->name),((_tmp931[0]=Cyc_PP_text(((_tmp933="*",_tag_dyneither(_tmp933,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp931,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL261;_LL27A: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp42F=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp42F->tag != 14)goto _LL27C;else{_tmp430=_tmp42F->f1;}}_LL27B:
# 1588
 s=Cyc_Absynpp_qvar2doc(_tmp430);
goto _LL261;_LL27C: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp431=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp431->tag != 15)goto _LL27E;else{_tmp432=_tmp431->f1;_tmp433=_tmp431->f2;_tmp434=_tmp431->f3;}}_LL27D: {
# 1591
const char*_tmp935;const char*_tmp934;struct _dyneither_ptr term=_tmp434?(_tmp935=", ...)",_tag_dyneither(_tmp935,sizeof(char),7)):((_tmp934=")",_tag_dyneither(_tmp934,sizeof(char),2)));
{const char*_tmp93A;const char*_tmp939;struct Cyc_PP_Doc*_tmp938[2];s=((_tmp938[1]=Cyc_PP_group(((_tmp93A="(",_tag_dyneither(_tmp93A,sizeof(char),2))),term,((_tmp939=",",_tag_dyneither(_tmp939,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp433)),((_tmp938[0]=Cyc_Absynpp_qvar2doc(_tmp432),Cyc_PP_cat(_tag_dyneither(_tmp938,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL27E: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp435=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp435->tag != 6)goto _LL280;else{if(_tmp435->f1 == 0)goto _LL280;_tmp436=(_tmp435->f1)->aggr_info;_tmp437=_tmp435->f2;_tmp438=_tmp435->f3;_tmp439=_tmp435->f4;}}_LL27F: {
# 1595
const char*_tmp93C;const char*_tmp93B;struct _dyneither_ptr term=_tmp439?(_tmp93C=", ...}",_tag_dyneither(_tmp93C,sizeof(char),7)):((_tmp93B="}",_tag_dyneither(_tmp93B,sizeof(char),2)));
struct _tuple10 _stmttmp1A=Cyc_Absyn_aggr_kinded_name(_tmp436);struct _tuple0*_tmp46C;struct _tuple10 _tmp46B=_stmttmp1A;_tmp46C=_tmp46B.f2;
{const char*_tmp947;const char*_tmp946;const char*_tmp945;const char*_tmp944;const char*_tmp943;struct Cyc_PP_Doc*_tmp942[4];s=((_tmp942[3]=
# 1599
Cyc_PP_group(((_tmp944="",_tag_dyneither(_tmp944,sizeof(char),1))),term,((_tmp943=",",_tag_dyneither(_tmp943,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp438)),((_tmp942[2]=
# 1598
Cyc_PP_egroup(((_tmp947="[",_tag_dyneither(_tmp947,sizeof(char),2))),((_tmp946="]",_tag_dyneither(_tmp946,sizeof(char),2))),((_tmp945=",",_tag_dyneither(_tmp945,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp437)),((_tmp942[1]=
# 1597
Cyc_Absynpp_lb(),((_tmp942[0]=Cyc_Absynpp_qvar2doc(_tmp46C),Cyc_PP_cat(_tag_dyneither(_tmp942,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1600
goto _LL261;}_LL280: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp43A=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp43A->tag != 6)goto _LL282;else{if(_tmp43A->f1 != 0)goto _LL282;_tmp43B=_tmp43A->f2;_tmp43C=_tmp43A->f3;_tmp43D=_tmp43A->f4;}}_LL281: {
# 1602
const char*_tmp949;const char*_tmp948;struct _dyneither_ptr term=_tmp43D?(_tmp949=", ...}",_tag_dyneither(_tmp949,sizeof(char),7)):((_tmp948="}",_tag_dyneither(_tmp948,sizeof(char),2)));
{const char*_tmp954;const char*_tmp953;const char*_tmp952;const char*_tmp951;const char*_tmp950;struct Cyc_PP_Doc*_tmp94F[3];s=((_tmp94F[2]=
# 1605
Cyc_PP_group(((_tmp951="",_tag_dyneither(_tmp951,sizeof(char),1))),term,((_tmp950=",",_tag_dyneither(_tmp950,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp43C)),((_tmp94F[1]=
# 1604
Cyc_PP_egroup(((_tmp954="[",_tag_dyneither(_tmp954,sizeof(char),2))),((_tmp953="]",_tag_dyneither(_tmp953,sizeof(char),2))),((_tmp952=",",_tag_dyneither(_tmp952,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_tvar2doc,_tmp43B)),((_tmp94F[0]=
# 1603
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp94F,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1606
goto _LL261;}_LL282: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp43E->tag != 12)goto _LL284;else{_tmp43F=_tmp43E->f2;}}_LL283:
 _tmp441=_tmp43F;goto _LL285;_LL284: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp440=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp440->tag != 13)goto _LL286;else{_tmp441=_tmp440->f2;}}_LL285:
 s=Cyc_Absynpp_qvar2doc(_tmp441->name);goto _LL261;_LL286: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp442=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp442->tag != 7)goto _LL288;else{_tmp443=_tmp442->f2;if(_tmp442->f3 != 0)goto _LL288;}}_LL287:
 s=Cyc_Absynpp_qvar2doc(_tmp443->name);goto _LL261;_LL288: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp444=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp444->tag != 7)goto _LL28A;else{_tmp445=_tmp444->f2;_tmp446=_tmp444->f3;_tmp447=_tmp444->f4;}}_LL289: {
# 1611
const char*_tmp956;const char*_tmp955;struct _dyneither_ptr term=_tmp447?(_tmp956=", ...)",_tag_dyneither(_tmp956,sizeof(char),7)):((_tmp955=")",_tag_dyneither(_tmp955,sizeof(char),2)));
{const char*_tmp95B;const char*_tmp95A;struct Cyc_PP_Doc*_tmp959[2];s=((_tmp959[1]=Cyc_PP_egroup(((_tmp95B="(",_tag_dyneither(_tmp95B,sizeof(char),2))),term,((_tmp95A=",",_tag_dyneither(_tmp95A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp446)),((_tmp959[0]=Cyc_Absynpp_qvar2doc(_tmp445->name),Cyc_PP_cat(_tag_dyneither(_tmp959,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL261;}_LL28A: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp448=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp411;if(_tmp448->tag != 16)goto _LL261;else{_tmp449=_tmp448->f1;}}_LL28B:
# 1615
 s=Cyc_Absynpp_exp2doc(_tmp449);goto _LL261;_LL261:;}
# 1617
return s;}
# 1620
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp962;const char*_tmp961;const char*_tmp960;struct Cyc_PP_Doc*_tmp95F[2];return(_tmp95F[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp95F[0]=
# 1621
Cyc_PP_egroup(((_tmp962="",_tag_dyneither(_tmp962,sizeof(char),1))),((_tmp961="=",_tag_dyneither(_tmp961,sizeof(char),2))),((_tmp960="=",_tag_dyneither(_tmp960,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1625
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp967;struct Cyc_PP_Doc*_tmp966[2];struct Cyc_PP_Doc*_tmp965[2];return(_tmp965[1]=
Cyc_PP_nest(2,((_tmp966[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp966[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp966,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp965[0]=
# 1627
Cyc_PP_text(((_tmp967="default: ",_tag_dyneither(_tmp967,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp965,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1629
if(c->where_clause == 0){
const char*_tmp96E;const char*_tmp96D;struct Cyc_PP_Doc*_tmp96C[2];struct Cyc_PP_Doc*_tmp96B[4];return(_tmp96B[3]=
# 1633
Cyc_PP_nest(2,((_tmp96C[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp96C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp96B[2]=
# 1632
Cyc_PP_text(((_tmp96D=": ",_tag_dyneither(_tmp96D,sizeof(char),3)))),((_tmp96B[1]=
# 1631
Cyc_Absynpp_pat2doc(c->pattern),((_tmp96B[0]=
# 1630
Cyc_PP_text(((_tmp96E="case ",_tag_dyneither(_tmp96E,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp96B,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1635
const char*_tmp977;const char*_tmp976;const char*_tmp975;struct Cyc_PP_Doc*_tmp974[2];struct Cyc_PP_Doc*_tmp973[6];return(_tmp973[5]=
# 1640
Cyc_PP_nest(2,((_tmp974[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp974[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp974,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp973[4]=
# 1639
Cyc_PP_text(((_tmp975=": ",_tag_dyneither(_tmp975,sizeof(char),3)))),((_tmp973[3]=
# 1638
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp973[2]=
# 1637
Cyc_PP_text(((_tmp976=" && ",_tag_dyneither(_tmp976,sizeof(char),5)))),((_tmp973[1]=
# 1636
Cyc_Absynpp_pat2doc(c->pattern),((_tmp973[0]=
# 1635
Cyc_PP_text(((_tmp977="case ",_tag_dyneither(_tmp977,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp973,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1643
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp978;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp978="",_tag_dyneither(_tmp978,sizeof(char),1))),cs);}
# 1647
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1651
const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[3];return(_tmp97A[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp97A[1]=Cyc_PP_text(((_tmp97B=" = ",_tag_dyneither(_tmp97B,sizeof(char),4)))),((_tmp97A[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1654
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp97C;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp97C=",",_tag_dyneither(_tmp97C,sizeof(char),2))),fs);}
# 1658
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1662
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp97D;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp97D=",",_tag_dyneither(_tmp97D,sizeof(char),2))),vds);}
# 1666
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp498;struct _tuple0*_tmp499;struct Cyc_Absyn_Tqual _tmp49A;void*_tmp49B;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_List_List*_tmp49D;struct Cyc_Absyn_Vardecl*_tmp497=vd;_tmp498=_tmp497->sc;_tmp499=_tmp497->name;_tmp49A=_tmp497->tq;_tmp49B=_tmp497->type;_tmp49C=_tmp497->initializer;_tmp49D=_tmp497->attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp499);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp49D);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL28C:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL28D:
# 1675
{void*_stmttmp1B=Cyc_Tcutil_compress(_tmp49B);void*_tmp49E=_stmttmp1B;struct Cyc_List_List*_tmp4A0;_LL290: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49F=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49E;if(_tmp49F->tag != 9)goto _LL292;else{_tmp4A0=(_tmp49F->f1).attributes;}}_LL291:
# 1677
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4A0);
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
{struct Cyc_Core_Opt*_tmp98A;struct Cyc_PP_Doc*_tmp989[2];struct Cyc_PP_Doc*_tmp988[2];const char*_tmp987;const char*_tmp986;struct Cyc_PP_Doc*_tmp985[5];s=((_tmp985[4]=
# 1696
Cyc_PP_text(((_tmp986=";",_tag_dyneither(_tmp986,sizeof(char),2)))),((_tmp985[3]=
# 1693
_tmp49C == 0?
Cyc_PP_nil_doc():(
(_tmp988[1]=Cyc_Absynpp_exp2doc(_tmp49C),((_tmp988[0]=Cyc_PP_text(((_tmp987=" = ",_tag_dyneither(_tmp987,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp988,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp985[2]=
# 1692
Cyc_Absynpp_tqtd2doc(_tmp49A,_tmp49B,((_tmp98A=_cycalloc(sizeof(*_tmp98A)),((_tmp98A->v=((_tmp989[1]=sn,((_tmp989[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp989,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp98A))))),((_tmp985[1]=
# 1691
Cyc_Absynpp_scope2doc(_tmp498),((_tmp985[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp985,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1697
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1700
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple17 _stmttmp1C=*x;struct _tuple0*_tmp4A8;struct _tuple17 _tmp4A7=_stmttmp1C;_tmp4A8=_tmp4A7.f2;
return Cyc_Absynpp_qvar2doc(_tmp4A8);}
# 1705
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp98B[4];return(_tmp98B[3]=
# 1710
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp98B[2]=
# 1709
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp98B[1]=
# 1708
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp98B[0]=
# 1707
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp98B,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1712
struct Cyc_PP_Doc*_tmp993[2];const char*_tmp992;struct Cyc_PP_Doc*_tmp991[2];struct Cyc_PP_Doc*_tmp990[12];return(_tmp990[11]=
# 1723
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp990[10]=
# 1722
Cyc_Absynpp_rb(),((_tmp990[9]=
# 1721
Cyc_PP_line_doc(),((_tmp990[8]=
# 1720
Cyc_PP_nest(2,((_tmp991[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp991[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp990[7]=
# 1718
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp993[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp993[0]=Cyc_PP_text(((_tmp992=":",_tag_dyneither(_tmp992,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp993,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp990[6]=
# 1717
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp990[5]=
# 1716
Cyc_Absynpp_lb(),((_tmp990[4]=Cyc_PP_blank_doc(),((_tmp990[3]=
# 1715
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp990[2]=
# 1714
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp990[1]=
# 1713
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp990[0]=
# 1712
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp990,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1726
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4AF;struct _tuple0*_tmp4B0;struct Cyc_List_List*_tmp4B1;struct Cyc_Core_Opt*_tmp4B2;int _tmp4B3;struct Cyc_Absyn_Datatypedecl*_tmp4AE=dd;_tmp4AF=_tmp4AE->sc;_tmp4B0=_tmp4AE->name;_tmp4B1=_tmp4AE->tvs;_tmp4B2=_tmp4AE->fields;_tmp4B3=_tmp4AE->is_extensible;
if(_tmp4B2 == 0){
const char*_tmp998;const char*_tmp997;struct Cyc_PP_Doc*_tmp996[5];return(_tmp996[4]=
# 1733
Cyc_Absynpp_ktvars2doc(_tmp4B1),((_tmp996[3]=
# 1732
_tmp4B3?Cyc_Absynpp_qvar2bolddoc(_tmp4B0): Cyc_Absynpp_typedef_name2bolddoc(_tmp4B0),((_tmp996[2]=
# 1731
Cyc_PP_text(((_tmp997="datatype ",_tag_dyneither(_tmp997,sizeof(char),10)))),((_tmp996[1]=
# 1730
_tmp4B3?Cyc_PP_text(((_tmp998="@extensible ",_tag_dyneither(_tmp998,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp996[0]=
# 1729
Cyc_Absynpp_scope2doc(_tmp4AF),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1735
const char*_tmp99F;const char*_tmp99E;struct Cyc_PP_Doc*_tmp99D[2];struct Cyc_PP_Doc*_tmp99C[10];return(_tmp99C[9]=
# 1743
Cyc_Absynpp_rb(),((_tmp99C[8]=
# 1742
Cyc_PP_line_doc(),((_tmp99C[7]=
# 1741
Cyc_PP_nest(2,((_tmp99D[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4B2->v),((_tmp99D[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp99D,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp99C[6]=
# 1740
Cyc_Absynpp_lb(),((_tmp99C[5]=Cyc_PP_blank_doc(),((_tmp99C[4]=
# 1739
Cyc_Absynpp_ktvars2doc(_tmp4B1),((_tmp99C[3]=
# 1738
_tmp4B3?Cyc_Absynpp_qvar2bolddoc(_tmp4B0): Cyc_Absynpp_typedef_name2bolddoc(_tmp4B0),((_tmp99C[2]=
# 1737
Cyc_PP_text(((_tmp99E="datatype ",_tag_dyneither(_tmp99E,sizeof(char),10)))),((_tmp99C[1]=
# 1736
_tmp4B3?Cyc_PP_text(((_tmp99F="@extensible ",_tag_dyneither(_tmp99F,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp99C[0]=
# 1735
Cyc_Absynpp_scope2doc(_tmp4AF),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1746
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4BC;struct _tuple0*_tmp4BD;struct Cyc_Core_Opt*_tmp4BE;struct Cyc_Absyn_Enumdecl*_tmp4BB=ed;_tmp4BC=_tmp4BB->sc;_tmp4BD=_tmp4BB->name;_tmp4BE=_tmp4BB->fields;
if(_tmp4BE == 0){
const char*_tmp9A2;struct Cyc_PP_Doc*_tmp9A1[3];return(_tmp9A1[2]=
# 1751
Cyc_Absynpp_typedef_name2bolddoc(_tmp4BD),((_tmp9A1[1]=
# 1750
Cyc_PP_text(((_tmp9A2="enum ",_tag_dyneither(_tmp9A2,sizeof(char),6)))),((_tmp9A1[0]=
# 1749
Cyc_Absynpp_scope2doc(_tmp4BC),Cyc_PP_cat(_tag_dyneither(_tmp9A1,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1754
const char*_tmp9A7;struct Cyc_PP_Doc*_tmp9A6[2];struct Cyc_PP_Doc*_tmp9A5[8];return(_tmp9A5[7]=
# 1760
Cyc_Absynpp_rb(),((_tmp9A5[6]=
# 1759
Cyc_PP_line_doc(),((_tmp9A5[5]=
# 1758
Cyc_PP_nest(2,((_tmp9A6[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4BE->v),((_tmp9A6[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A6,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A5[4]=
# 1757
Cyc_Absynpp_lb(),((_tmp9A5[3]=Cyc_PP_blank_doc(),((_tmp9A5[2]=
# 1756
Cyc_Absynpp_qvar2bolddoc(_tmp4BD),((_tmp9A5[1]=
# 1755
Cyc_PP_text(((_tmp9A7="enum ",_tag_dyneither(_tmp9A7,sizeof(char),6)))),((_tmp9A5[0]=
# 1754
Cyc_Absynpp_scope2doc(_tmp4BC),Cyc_PP_cat(_tag_dyneither(_tmp9A5,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1763
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_stmttmp1D=d->r;void*_tmp4C4=_stmttmp1D;struct Cyc_Absyn_Fndecl*_tmp4C6;struct Cyc_Absyn_Aggrdecl*_tmp4C8;struct Cyc_Absyn_Vardecl*_tmp4CA;struct Cyc_Absyn_Tvar*_tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4CD;int _tmp4CE;struct Cyc_Absyn_Exp*_tmp4CF;struct Cyc_Absyn_Tvar*_tmp4D1;struct Cyc_Absyn_Vardecl*_tmp4D2;struct Cyc_Absyn_Datatypedecl*_tmp4D4;struct Cyc_Absyn_Pat*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_List_List*_tmp4D9;struct Cyc_Absyn_Enumdecl*_tmp4DB;struct Cyc_Absyn_Typedefdecl*_tmp4DD;struct _dyneither_ptr*_tmp4DF;struct Cyc_List_List*_tmp4E0;struct _tuple0*_tmp4E2;struct Cyc_List_List*_tmp4E3;struct Cyc_List_List*_tmp4E5;struct Cyc_List_List*_tmp4E7;struct Cyc_List_List*_tmp4E8;_LL298: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4C5=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4C5->tag != 1)goto _LL29A;else{_tmp4C6=_tmp4C5->f1;}}_LL299: {
# 1767
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9AD;struct Cyc_Absyn_FnInfo _tmp9AC;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9AB;void*t=(void*)((_tmp9AB=_cycalloc(sizeof(*_tmp9AB)),((_tmp9AB[0]=((_tmp9AD.tag=9,((_tmp9AD.f1=((_tmp9AC.tvars=_tmp4C6->tvs,((_tmp9AC.effect=_tmp4C6->effect,((_tmp9AC.ret_tqual=_tmp4C6->ret_tqual,((_tmp9AC.ret_typ=_tmp4C6->ret_type,((_tmp9AC.args=
# 1771
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C6->args),((_tmp9AC.c_varargs=_tmp4C6->c_varargs,((_tmp9AC.cyc_varargs=_tmp4C6->cyc_varargs,((_tmp9AC.rgn_po=_tmp4C6->rgn_po,((_tmp9AC.attributes=0,_tmp9AC)))))))))))))))))),_tmp9AD)))),_tmp9AB))));
# 1775
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C6->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C6->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2B8:
{const char*_tmp9AE;inlinedoc=Cyc_PP_text(((_tmp9AE="inline ",_tag_dyneither(_tmp9AE,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2B9:
{const char*_tmp9AF;inlinedoc=Cyc_PP_text(((_tmp9AF="__inline ",_tag_dyneither(_tmp9AF,sizeof(char),10))));}break;}else{
# 1783
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C6->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2BB:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2BC:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C6->attributes);break;}{
# 1790
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C6->name);
struct Cyc_PP_Doc*_tmp9B2[2];struct Cyc_Core_Opt*_tmp9B1;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1->v=((_tmp9B2[1]=namedoc,((_tmp9B2[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9B2,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9B1)))));
struct Cyc_PP_Doc*_tmp9B5[2];struct Cyc_PP_Doc*_tmp9B4[5];struct Cyc_PP_Doc*bodydoc=(_tmp9B4[4]=
# 1796
Cyc_Absynpp_rb(),((_tmp9B4[3]=
# 1795
Cyc_PP_line_doc(),((_tmp9B4[2]=
# 1794
Cyc_PP_nest(2,((_tmp9B5[1]=Cyc_Absynpp_stmt2doc(_tmp4C6->body,0),((_tmp9B5[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B5,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9B4[1]=
# 1793
Cyc_Absynpp_lb(),((_tmp9B4[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9B4,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1797
{struct Cyc_PP_Doc*_tmp9B6[4];s=((_tmp9B6[3]=bodydoc,((_tmp9B6[2]=tqtddoc,((_tmp9B6[1]=scopedoc,((_tmp9B6[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9B6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1799
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2BE:
{struct Cyc_PP_Doc*_tmp9B7[2];s=((_tmp9B7[1]=s,((_tmp9B7[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9B7,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2BF:
 break;}
# 1804
goto _LL297;};};}_LL29A: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C7=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4C7->tag != 6)goto _LL29C;else{_tmp4C8=_tmp4C7->f1;}}_LL29B:
# 1807
{const char*_tmp9BA;struct Cyc_PP_Doc*_tmp9B9[2];s=((_tmp9B9[1]=Cyc_PP_text(((_tmp9BA=";",_tag_dyneither(_tmp9BA,sizeof(char),2)))),((_tmp9B9[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4C8),Cyc_PP_cat(_tag_dyneither(_tmp9B9,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL29C: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C9=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4C9->tag != 0)goto _LL29E;else{_tmp4CA=_tmp4C9->f1;}}_LL29D:
# 1810
 s=Cyc_Absynpp_vardecl2doc(_tmp4CA);
goto _LL297;_LL29E: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4CB=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4CB->tag != 4)goto _LL2A0;else{_tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;_tmp4CE=_tmp4CB->f3;_tmp4CF=_tmp4CB->f4;}}_LL29F:
# 1813
{const char*_tmp9CD;const char*_tmp9CC;const char*_tmp9CB;const char*_tmp9CA;struct Cyc_PP_Doc*_tmp9C9[3];const char*_tmp9C8;const char*_tmp9C7;const char*_tmp9C6;struct Cyc_PP_Doc*_tmp9C5[8];s=((_tmp9C5[7]=
# 1821
Cyc_PP_text(((_tmp9C6=";",_tag_dyneither(_tmp9C6,sizeof(char),2)))),((_tmp9C5[6]=
# 1819
_tmp4CF != 0?
(_tmp9C9[2]=Cyc_PP_text(((_tmp9C8=")",_tag_dyneither(_tmp9C8,sizeof(char),2)))),((_tmp9C9[1]=Cyc_Absynpp_exp2doc(_tmp4CF),((_tmp9C9[0]=Cyc_PP_text(((_tmp9C7=" = open(",_tag_dyneither(_tmp9C7,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9C9,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9C5[5]=
# 1818
_tmp4CE?Cyc_PP_text(((_tmp9CA=" @resetable",_tag_dyneither(_tmp9CA,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9C5[4]=
# 1817
Cyc_Absynpp_qvar2doc(_tmp4CD->name),((_tmp9C5[3]=
# 1816
Cyc_PP_text(((_tmp9CB=">",_tag_dyneither(_tmp9CB,sizeof(char),2)))),((_tmp9C5[2]=
# 1815
Cyc_Absynpp_tvar2doc(_tmp4CC),((_tmp9C5[1]=
# 1814
Cyc_PP_text(((_tmp9CC="<",_tag_dyneither(_tmp9CC,sizeof(char),2)))),((_tmp9C5[0]=
# 1813
Cyc_PP_text(((_tmp9CD="region",_tag_dyneither(_tmp9CD,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9C5,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1822
goto _LL297;_LL2A0: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4D0=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4D0->tag != 5)goto _LL2A2;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f2;}}_LL2A1:
# 1824
{const char*_tmp9D6;const char*_tmp9D5;const char*_tmp9D4;const char*_tmp9D3;struct Cyc_PP_Doc*_tmp9D2[6];s=((_tmp9D2[5]=
Cyc_PP_text(((_tmp9D3=";",_tag_dyneither(_tmp9D3,sizeof(char),2)))),((_tmp9D2[4]=Cyc_Absynpp_vardecl2doc(_tmp4D2),((_tmp9D2[3]=
# 1824
Cyc_PP_text(((_tmp9D4="> ",_tag_dyneither(_tmp9D4,sizeof(char),3)))),((_tmp9D2[2]=Cyc_Absynpp_tvar2doc(_tmp4D1),((_tmp9D2[1]=Cyc_PP_text(((_tmp9D5=" <",_tag_dyneither(_tmp9D5,sizeof(char),3)))),((_tmp9D2[0]=Cyc_PP_text(((_tmp9D6="alias",_tag_dyneither(_tmp9D6,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9D2,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 1826
goto _LL297;_LL2A2: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4D3=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4D3->tag != 7)goto _LL2A4;else{_tmp4D4=_tmp4D3->f1;}}_LL2A3:
# 1828
{const char*_tmp9D9;struct Cyc_PP_Doc*_tmp9D8[2];s=((_tmp9D8[1]=Cyc_PP_text(((_tmp9D9=";",_tag_dyneither(_tmp9D9,sizeof(char),2)))),((_tmp9D8[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4D4),Cyc_PP_cat(_tag_dyneither(_tmp9D8,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2A4: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D5=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4D5->tag != 2)goto _LL2A6;else{_tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f3;}}_LL2A5:
# 1831
{const char*_tmp9E0;const char*_tmp9DF;const char*_tmp9DE;struct Cyc_PP_Doc*_tmp9DD[5];s=((_tmp9DD[4]=
# 1835
Cyc_PP_text(((_tmp9DE=";",_tag_dyneither(_tmp9DE,sizeof(char),2)))),((_tmp9DD[3]=
# 1834
Cyc_Absynpp_exp2doc(_tmp4D7),((_tmp9DD[2]=
# 1833
Cyc_PP_text(((_tmp9DF=" = ",_tag_dyneither(_tmp9DF,sizeof(char),4)))),((_tmp9DD[1]=
# 1832
Cyc_Absynpp_pat2doc(_tmp4D6),((_tmp9DD[0]=
# 1831
Cyc_PP_text(((_tmp9E0="let ",_tag_dyneither(_tmp9E0,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9DD,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1836
goto _LL297;_LL2A6: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4D8=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4D8->tag != 3)goto _LL2A8;else{_tmp4D9=_tmp4D8->f1;}}_LL2A7:
# 1838
{const char*_tmp9E5;const char*_tmp9E4;struct Cyc_PP_Doc*_tmp9E3[3];s=((_tmp9E3[2]=Cyc_PP_text(((_tmp9E4=";",_tag_dyneither(_tmp9E4,sizeof(char),2)))),((_tmp9E3[1]=Cyc_Absynpp_ids2doc(_tmp4D9),((_tmp9E3[0]=Cyc_PP_text(((_tmp9E5="let ",_tag_dyneither(_tmp9E5,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E3,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL297;_LL2A8: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4DA=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4DA->tag != 8)goto _LL2AA;else{_tmp4DB=_tmp4DA->f1;}}_LL2A9:
# 1841
{const char*_tmp9E8;struct Cyc_PP_Doc*_tmp9E7[2];s=((_tmp9E7[1]=Cyc_PP_text(((_tmp9E8=";",_tag_dyneither(_tmp9E8,sizeof(char),2)))),((_tmp9E7[0]=Cyc_Absynpp_enumdecl2doc(_tmp4DB),Cyc_PP_cat(_tag_dyneither(_tmp9E7,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2AA: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4DC=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4DC->tag != 9)goto _LL2AC;else{_tmp4DD=_tmp4DC->f1;}}_LL2AB: {
# 1844
void*t;
if(_tmp4DD->defn != 0)
t=(void*)_check_null(_tmp4DD->defn);else{
# 1848
t=Cyc_Absyn_new_evar(_tmp4DD->kind,0);}
{const char*_tmp9F2;struct Cyc_Core_Opt*_tmp9F1;struct Cyc_PP_Doc*_tmp9F0[2];const char*_tmp9EF;struct Cyc_PP_Doc*_tmp9EE[4];s=((_tmp9EE[3]=
# 1856
Cyc_PP_text(((_tmp9EF=";",_tag_dyneither(_tmp9EF,sizeof(char),2)))),((_tmp9EE[2]=
# 1855
Cyc_Absynpp_atts2doc(_tmp4DD->atts),((_tmp9EE[1]=
# 1850
Cyc_Absynpp_tqtd2doc(_tmp4DD->tq,t,(
# 1852
(_tmp9F1=_cycalloc(sizeof(*_tmp9F1)),((_tmp9F1->v=((_tmp9F0[1]=
Cyc_Absynpp_tvars2doc(_tmp4DD->tvs),((_tmp9F0[0]=
# 1852
Cyc_Absynpp_typedef_name2bolddoc(_tmp4DD->name),Cyc_PP_cat(_tag_dyneither(_tmp9F0,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9F1))))),((_tmp9EE[0]=
# 1849
Cyc_PP_text(((_tmp9F2="typedef ",_tag_dyneither(_tmp9F2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9EE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1858
goto _LL297;}_LL2AC: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4DE=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4DE->tag != 10)goto _LL2AE;else{_tmp4DF=_tmp4DE->f1;_tmp4E0=_tmp4DE->f2;}}_LL2AD:
# 1860
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DF);
{const char*_tmp9F7;const char*_tmp9F6;struct Cyc_PP_Doc*_tmp9F5[8];s=((_tmp9F5[7]=
# 1867
Cyc_Absynpp_rb(),((_tmp9F5[6]=
# 1866
Cyc_PP_line_doc(),((_tmp9F5[5]=
# 1865
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9F6="",_tag_dyneither(_tmp9F6,sizeof(char),1))),_tmp4E0),((_tmp9F5[4]=
# 1864
Cyc_PP_line_doc(),((_tmp9F5[3]=
# 1863
Cyc_Absynpp_lb(),((_tmp9F5[2]=Cyc_PP_blank_doc(),((_tmp9F5[1]=
# 1862
Cyc_PP_textptr(_tmp4DF),((_tmp9F5[0]=
# 1861
Cyc_PP_text(((_tmp9F7="namespace ",_tag_dyneither(_tmp9F7,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp9F5,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1868
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL297;_LL2AE: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4E1=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4E1->tag != 11)goto _LL2B0;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;}}_LL2AF:
# 1871
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmp9FC;const char*_tmp9FB;struct Cyc_PP_Doc*_tmp9FA[8];s=((_tmp9FA[7]=
# 1878
Cyc_Absynpp_rb(),((_tmp9FA[6]=
# 1877
Cyc_PP_line_doc(),((_tmp9FA[5]=
# 1876
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9FB="",_tag_dyneither(_tmp9FB,sizeof(char),1))),_tmp4E3),((_tmp9FA[4]=
# 1875
Cyc_PP_line_doc(),((_tmp9FA[3]=
# 1874
Cyc_Absynpp_lb(),((_tmp9FA[2]=Cyc_PP_blank_doc(),((_tmp9FA[1]=
# 1873
Cyc_Absynpp_qvar2doc(_tmp4E2),((_tmp9FA[0]=
# 1872
Cyc_PP_text(((_tmp9FC="using ",_tag_dyneither(_tmp9FC,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9FA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1880
const char*_tmpA07;const char*_tmpA06;const char*_tmpA05;const char*_tmpA04;const char*_tmpA03;struct Cyc_PP_Doc*_tmpA02[11];s=((_tmpA02[10]=
# 1890
Cyc_PP_text(((_tmpA03=" */",_tag_dyneither(_tmpA03,sizeof(char),4)))),((_tmpA02[9]=
# 1889
Cyc_Absynpp_rb(),((_tmpA02[8]=
# 1888
Cyc_PP_text(((_tmpA04="/* ",_tag_dyneither(_tmpA04,sizeof(char),4)))),((_tmpA02[7]=
# 1887
Cyc_PP_line_doc(),((_tmpA02[6]=
# 1886
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA05="",_tag_dyneither(_tmpA05,sizeof(char),1))),_tmp4E3),((_tmpA02[5]=
# 1885
Cyc_PP_line_doc(),((_tmpA02[4]=
# 1884
Cyc_PP_text(((_tmpA06=" */",_tag_dyneither(_tmpA06,sizeof(char),4)))),((_tmpA02[3]=
# 1883
Cyc_Absynpp_lb(),((_tmpA02[2]=
# 1882
Cyc_PP_blank_doc(),((_tmpA02[1]=
# 1881
Cyc_Absynpp_qvar2doc(_tmp4E2),((_tmpA02[0]=
# 1880
Cyc_PP_text(((_tmpA07="/* using ",_tag_dyneither(_tmpA07,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1891
goto _LL297;_LL2B0: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4E4=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4E4->tag != 12)goto _LL2B2;else{_tmp4E5=_tmp4E4->f1;}}_LL2B1:
# 1893
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA0C;const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[6];s=((_tmpA0A[5]=
# 1899
Cyc_Absynpp_rb(),((_tmpA0A[4]=
# 1898
Cyc_PP_line_doc(),((_tmpA0A[3]=
# 1897
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0B="",_tag_dyneither(_tmpA0B,sizeof(char),1))),_tmp4E5),((_tmpA0A[2]=
# 1896
Cyc_PP_line_doc(),((_tmpA0A[1]=
# 1895
Cyc_Absynpp_lb(),((_tmpA0A[0]=
# 1894
Cyc_PP_text(((_tmpA0C="extern \"C\" ",_tag_dyneither(_tmpA0C,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1901
const char*_tmpA17;const char*_tmpA16;const char*_tmpA15;const char*_tmpA14;const char*_tmpA13;struct Cyc_PP_Doc*_tmpA12[9];s=((_tmpA12[8]=
# 1909
Cyc_PP_text(((_tmpA13=" */",_tag_dyneither(_tmpA13,sizeof(char),4)))),((_tmpA12[7]=
# 1908
Cyc_Absynpp_rb(),((_tmpA12[6]=
# 1907
Cyc_PP_text(((_tmpA14="/* ",_tag_dyneither(_tmpA14,sizeof(char),4)))),((_tmpA12[5]=
# 1906
Cyc_PP_line_doc(),((_tmpA12[4]=
# 1905
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA15="",_tag_dyneither(_tmpA15,sizeof(char),1))),_tmp4E5),((_tmpA12[3]=
# 1904
Cyc_PP_line_doc(),((_tmpA12[2]=
# 1903
Cyc_PP_text(((_tmpA16=" */",_tag_dyneither(_tmpA16,sizeof(char),4)))),((_tmpA12[1]=
# 1902
Cyc_Absynpp_lb(),((_tmpA12[0]=
# 1901
Cyc_PP_text(((_tmpA17="/* extern \"C\" ",_tag_dyneither(_tmpA17,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA12,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1910
goto _LL297;_LL2B2: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E6=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4E6->tag != 13)goto _LL2B4;else{_tmp4E7=_tmp4E6->f1;_tmp4E8=_tmp4E6->f2;}}_LL2B3:
# 1912
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4E8 != 0){
const char*_tmpA1C;const char*_tmpA1B;struct Cyc_PP_Doc*_tmpA1A[7];exs_doc=((_tmpA1A[6]=
# 1917
Cyc_Absynpp_rb(),((_tmpA1A[5]=Cyc_PP_line_doc(),((_tmpA1A[4]=
# 1916
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA1B=",",_tag_dyneither(_tmpA1B,sizeof(char),2))),_tmp4E8),((_tmpA1A[3]=Cyc_PP_line_doc(),((_tmpA1A[2]=
# 1915
Cyc_Absynpp_lb(),((_tmpA1A[1]=Cyc_PP_text(((_tmpA1C=" export ",_tag_dyneither(_tmpA1C,sizeof(char),9)))),((_tmpA1A[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA1A,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1919
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA21;const char*_tmpA20;struct Cyc_PP_Doc*_tmpA1F[6];s=((_tmpA1F[5]=exs_doc,((_tmpA1F[4]=
# 1924
Cyc_PP_line_doc(),((_tmpA1F[3]=
# 1923
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA20="",_tag_dyneither(_tmpA20,sizeof(char),1))),_tmp4E7),((_tmpA1F[2]=
# 1922
Cyc_PP_line_doc(),((_tmpA1F[1]=
# 1921
Cyc_Absynpp_lb(),((_tmpA1F[0]=
# 1920
Cyc_PP_text(((_tmpA21="extern \"C include\" ",_tag_dyneither(_tmpA21,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA1F,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1927
const char*_tmpA2C;const char*_tmpA2B;const char*_tmpA2A;const char*_tmpA29;const char*_tmpA28;struct Cyc_PP_Doc*_tmpA27[9];s=((_tmpA27[8]=
# 1935
Cyc_PP_text(((_tmpA28=" */",_tag_dyneither(_tmpA28,sizeof(char),4)))),((_tmpA27[7]=
# 1934
Cyc_Absynpp_rb(),((_tmpA27[6]=
# 1933
Cyc_PP_text(((_tmpA29="/* ",_tag_dyneither(_tmpA29,sizeof(char),4)))),((_tmpA27[5]=
# 1932
Cyc_PP_line_doc(),((_tmpA27[4]=
# 1931
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA2A="",_tag_dyneither(_tmpA2A,sizeof(char),1))),_tmp4E7),((_tmpA27[3]=
# 1930
Cyc_PP_line_doc(),((_tmpA27[2]=
# 1929
Cyc_PP_text(((_tmpA2B=" */",_tag_dyneither(_tmpA2B,sizeof(char),4)))),((_tmpA27[1]=
# 1928
Cyc_Absynpp_lb(),((_tmpA27[0]=
# 1927
Cyc_PP_text(((_tmpA2C="/* extern \"C include\" ",_tag_dyneither(_tmpA2C,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA27,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1936
goto _LL297;_LL2B4: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4E9=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4E9->tag != 14)goto _LL2B6;}_LL2B5:
# 1938
{const char*_tmpA2F;struct Cyc_PP_Doc*_tmpA2E[2];s=((_tmpA2E[1]=Cyc_Absynpp_lb(),((_tmpA2E[0]=Cyc_PP_text(((_tmpA2F="__cyclone_port_on__;",_tag_dyneither(_tmpA2F,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA2E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL2B6: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4EA=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4C4;if(_tmp4EA->tag != 15)goto _LL297;}_LL2B7:
# 1941
{const char*_tmpA32;struct Cyc_PP_Doc*_tmpA31[2];s=((_tmpA31[1]=Cyc_Absynpp_lb(),((_tmpA31[0]=Cyc_PP_text(((_tmpA32="__cyclone_port_off__;",_tag_dyneither(_tmpA32,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA31,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL297;_LL297:;}
# 1945
return s;}
# 1948
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2C1: {
const char*_tmpA33;return Cyc_PP_text(((_tmpA33="static ",_tag_dyneither(_tmpA33,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2C2:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2C3: {
const char*_tmpA34;return Cyc_PP_text(((_tmpA34="extern ",_tag_dyneither(_tmpA34,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2C4: {
const char*_tmpA35;return Cyc_PP_text(((_tmpA35="extern \"C\" ",_tag_dyneither(_tmpA35,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2C5: {
const char*_tmpA36;return Cyc_PP_text(((_tmpA36="abstract ",_tag_dyneither(_tmpA36,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2C6: {
const char*_tmpA37;return Cyc_PP_text(((_tmpA37="register ",_tag_dyneither(_tmpA37,sizeof(char),10))));}}}
# 1961
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp540=t;struct Cyc_Absyn_Tvar*_tmp542;struct Cyc_List_List*_tmp544;_LL2C9: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp541=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp540;if(_tmp541->tag != 2)goto _LL2CB;else{_tmp542=_tmp541->f1;}}_LL2CA:
 return Cyc_Tcutil_is_temp_tvar(_tmp542);_LL2CB: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp540;if(_tmp543->tag != 24)goto _LL2CD;else{_tmp544=_tmp543->f1;}}_LL2CC:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp544);_LL2CD:;_LL2CE:
 return 0;_LL2C8:;}
# 1973
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp545=t;struct Cyc_Absyn_Typedefdecl*_tmp549;void*_tmp54A;_LL2D0: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp546=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp545;if(_tmp546->tag != 12)goto _LL2D2;}_LL2D1:
 return 1;_LL2D2: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp547=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp545;if(_tmp547->tag != 14)goto _LL2D4;}_LL2D3:
 return 1;_LL2D4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp548=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp545;if(_tmp548->tag != 17)goto _LL2D6;else{_tmp549=_tmp548->f3;_tmp54A=(void*)_tmp548->f4;}}if(!(_tmp54A != 0))goto _LL2D6;_LL2D5:
# 1980
 return Cyc_Absynpp_is_anon_aggrtype(_tmp54A);_LL2D6:;_LL2D7:
 return 0;_LL2CF:;}
# 1987
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1990
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA38;struct _tuple16 _stmttmp1E=(_tmpA38.f1=(void*)tms->hd,((_tmpA38.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA38)));struct _tuple16 _tmp54B=_stmttmp1E;_LL2D9:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp54C=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp54B.f1;if(_tmp54C->tag != 2)goto _LL2DB;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54D=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp54B.f2;if(_tmp54D->tag != 3)goto _LL2DB;};_LL2DA: {
# 1993
struct Cyc_List_List*_tmpA3B;struct Cyc_List_List*_tmpA3A;return(_tmpA3A=_region_malloc(r,sizeof(*_tmpA3A)),((_tmpA3A->hd=(void*)tms->hd,((_tmpA3A->tl=((_tmpA3B=_region_malloc(r,sizeof(*_tmpA3B)),((_tmpA3B->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA3B->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA3B)))))),_tmpA3A)))));}_LL2DB:;_LL2DC: {
struct Cyc_List_List*_tmpA3C;return(_tmpA3C=_region_malloc(r,sizeof(*_tmpA3C)),((_tmpA3C->hd=atts,((_tmpA3C->tl=tms,_tmpA3C)))));}_LL2D8:;}else{
# 1996
struct Cyc_List_List*_tmpA3D;return(_tmpA3D=_region_malloc(r,sizeof(*_tmpA3D)),((_tmpA3D->hd=atts,((_tmpA3D->tl=tms,_tmpA3D)))));}}
# 2001
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 2003
void*_tmp553=t;void*_tmp555;struct Cyc_Absyn_Tqual _tmp556;struct Cyc_Absyn_Exp*_tmp557;union Cyc_Absyn_Constraint*_tmp558;unsigned int _tmp559;void*_tmp55B;struct Cyc_Absyn_Tqual _tmp55C;struct Cyc_Absyn_PtrAtts _tmp55D;struct Cyc_List_List*_tmp55F;void*_tmp560;struct Cyc_Absyn_Tqual _tmp561;void*_tmp562;struct Cyc_List_List*_tmp563;int _tmp564;struct Cyc_Absyn_VarargInfo*_tmp565;struct Cyc_List_List*_tmp566;struct Cyc_List_List*_tmp567;struct Cyc_Core_Opt*_tmp569;void*_tmp56A;int _tmp56B;struct _tuple0*_tmp56D;struct Cyc_List_List*_tmp56E;struct Cyc_Absyn_Typedefdecl*_tmp56F;void*_tmp570;_LL2DE: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp554=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp553;if(_tmp554->tag != 8)goto _LL2E0;else{_tmp555=(_tmp554->f1).elt_type;_tmp556=(_tmp554->f1).tq;_tmp557=(_tmp554->f1).num_elts;_tmp558=(_tmp554->f1).zero_term;_tmp559=(_tmp554->f1).zt_loc;}}_LL2DF: {
# 2006
struct _tuple12 _stmttmp22=Cyc_Absynpp_to_tms(r,_tmp556,_tmp555);struct Cyc_Absyn_Tqual _tmp572;void*_tmp573;struct Cyc_List_List*_tmp574;struct _tuple12 _tmp571=_stmttmp22;_tmp572=_tmp571.f1;_tmp573=_tmp571.f2;_tmp574=_tmp571.f3;{
void*tm;
if(_tmp557 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA40;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA3F;tm=(void*)((_tmpA3F=_region_malloc(r,sizeof(*_tmpA3F)),((_tmpA3F[0]=((_tmpA40.tag=0,((_tmpA40.f1=_tmp558,((_tmpA40.f2=_tmp559,_tmpA40)))))),_tmpA3F))));}else{
# 2011
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA43;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA42;tm=(void*)((_tmpA42=_region_malloc(r,sizeof(*_tmpA42)),((_tmpA42[0]=((_tmpA43.tag=1,((_tmpA43.f1=_tmp557,((_tmpA43.f2=_tmp558,((_tmpA43.f3=_tmp559,_tmpA43)))))))),_tmpA42))));}{
struct Cyc_List_List*_tmpA46;struct _tuple12 _tmpA45;return(_tmpA45.f1=_tmp572,((_tmpA45.f2=_tmp573,((_tmpA45.f3=((_tmpA46=_region_malloc(r,sizeof(*_tmpA46)),((_tmpA46->hd=tm,((_tmpA46->tl=_tmp574,_tmpA46)))))),_tmpA45)))));};};}_LL2E0: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp553;if(_tmp55A->tag != 5)goto _LL2E2;else{_tmp55B=(_tmp55A->f1).elt_typ;_tmp55C=(_tmp55A->f1).elt_tq;_tmp55D=(_tmp55A->f1).ptr_atts;}}_LL2E1: {
# 2015
struct _tuple12 _stmttmp21=Cyc_Absynpp_to_tms(r,_tmp55C,_tmp55B);struct Cyc_Absyn_Tqual _tmp57C;void*_tmp57D;struct Cyc_List_List*_tmp57E;struct _tuple12 _tmp57B=_stmttmp21;_tmp57C=_tmp57B.f1;_tmp57D=_tmp57B.f2;_tmp57E=_tmp57B.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA4C;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA4B;struct Cyc_List_List*_tmpA4A;_tmp57E=((_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A->hd=(void*)((_tmpA4C=_region_malloc(r,sizeof(*_tmpA4C)),((_tmpA4C[0]=((_tmpA4B.tag=2,((_tmpA4B.f1=_tmp55D,((_tmpA4B.f2=tq,_tmpA4B)))))),_tmpA4C)))),((_tmpA4A->tl=_tmp57E,_tmpA4A))))));}{
struct _tuple12 _tmpA4D;return(_tmpA4D.f1=_tmp57C,((_tmpA4D.f2=_tmp57D,((_tmpA4D.f3=_tmp57E,_tmpA4D)))));};}_LL2E2: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp553;if(_tmp55E->tag != 9)goto _LL2E4;else{_tmp55F=(_tmp55E->f1).tvars;_tmp560=(_tmp55E->f1).effect;_tmp561=(_tmp55E->f1).ret_tqual;_tmp562=(_tmp55E->f1).ret_typ;_tmp563=(_tmp55E->f1).args;_tmp564=(_tmp55E->f1).c_varargs;_tmp565=(_tmp55E->f1).cyc_varargs;_tmp566=(_tmp55E->f1).rgn_po;_tmp567=(_tmp55E->f1).attributes;}}_LL2E3:
# 2021
 if(!Cyc_Absynpp_print_all_tvars){
# 2025
if(_tmp560 == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect(_tmp560)){
_tmp560=0;
_tmp55F=0;}}else{
# 2030
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2033
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp55F);}{
# 2038
struct _tuple12 _stmttmp1F=Cyc_Absynpp_to_tms(r,_tmp561,_tmp562);struct Cyc_Absyn_Tqual _tmp584;void*_tmp585;struct Cyc_List_List*_tmp586;struct _tuple12 _tmp583=_stmttmp1F;_tmp584=_tmp583.f1;_tmp585=_tmp583.f2;_tmp586=_tmp583.f3;{
struct Cyc_List_List*tms=_tmp586;
# 2049 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2EA:
# 2051
 if(_tmp567 != 0){
# 2054
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA50;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA4F;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA4F=_region_malloc(r,sizeof(*_tmpA4F)),((_tmpA4F[0]=((_tmpA50.tag=5,((_tmpA50.f1=0,((_tmpA50.f2=_tmp567,_tmpA50)))))),_tmpA4F)))),tms);}
# 2056
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA5F;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA5E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA5D;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA5C;struct Cyc_List_List*_tmpA5B;tms=((_tmpA5B=_region_malloc(r,sizeof(*_tmpA5B)),((_tmpA5B->hd=(void*)((_tmpA5F=_region_malloc(r,sizeof(*_tmpA5F)),((_tmpA5F[0]=(
(_tmpA5C.tag=3,((_tmpA5C.f1=(void*)((_tmpA5E=_region_malloc(r,sizeof(*_tmpA5E)),((_tmpA5E[0]=((_tmpA5D.tag=1,((_tmpA5D.f1=_tmp563,((_tmpA5D.f2=_tmp564,((_tmpA5D.f3=_tmp565,((_tmpA5D.f4=_tmp560,((_tmpA5D.f5=_tmp566,_tmpA5D)))))))))))),_tmpA5E)))),_tmpA5C)))),_tmpA5F)))),((_tmpA5B->tl=tms,_tmpA5B))))));}
# 2060
break;case Cyc_Cyclone_Vc_c: _LL2EB:
# 2062
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA6E;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA6D;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA6C;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA6B;struct Cyc_List_List*_tmpA6A;tms=((_tmpA6A=_region_malloc(r,sizeof(*_tmpA6A)),((_tmpA6A->hd=(void*)((_tmpA6E=_region_malloc(r,sizeof(*_tmpA6E)),((_tmpA6E[0]=(
(_tmpA6B.tag=3,((_tmpA6B.f1=(void*)((_tmpA6D=_region_malloc(r,sizeof(*_tmpA6D)),((_tmpA6D[0]=((_tmpA6C.tag=1,((_tmpA6C.f1=_tmp563,((_tmpA6C.f2=_tmp564,((_tmpA6C.f3=_tmp565,((_tmpA6C.f4=_tmp560,((_tmpA6C.f5=_tmp566,_tmpA6C)))))))))))),_tmpA6D)))),_tmpA6B)))),_tmpA6E)))),((_tmpA6A->tl=tms,_tmpA6A))))));}
# 2066
for(0;_tmp567 != 0;_tmp567=_tmp567->tl){
void*_stmttmp20=(void*)_tmp567->hd;void*_tmp593=_stmttmp20;_LL2EE: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp594=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp593;if(_tmp594->tag != 1)goto _LL2F0;}_LL2EF:
 goto _LL2F1;_LL2F0: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp595=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp593;if(_tmp595->tag != 2)goto _LL2F2;}_LL2F1:
 goto _LL2F3;_LL2F2: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp596=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp593;if(_tmp596->tag != 3)goto _LL2F4;}_LL2F3:
# 2071
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA78;struct Cyc_List_List*_tmpA77;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA76;struct Cyc_List_List*_tmpA75;tms=((_tmpA75=_region_malloc(r,sizeof(*_tmpA75)),((_tmpA75->hd=(void*)((_tmpA78=_region_malloc(r,sizeof(*_tmpA78)),((_tmpA78[0]=((_tmpA76.tag=5,((_tmpA76.f1=0,((_tmpA76.f2=((_tmpA77=_cycalloc(sizeof(*_tmpA77)),((_tmpA77->hd=(void*)_tmp567->hd,((_tmpA77->tl=0,_tmpA77)))))),_tmpA76)))))),_tmpA78)))),((_tmpA75->tl=tms,_tmpA75))))));}
goto AfterAtts;_LL2F4:;_LL2F5:
 goto _LL2ED;_LL2ED:;}
# 2075
break;}
# 2078
AfterAtts:
 if(_tmp55F != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA7E;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA7D;struct Cyc_List_List*_tmpA7C;tms=((_tmpA7C=_region_malloc(r,sizeof(*_tmpA7C)),((_tmpA7C->hd=(void*)((_tmpA7E=_region_malloc(r,sizeof(*_tmpA7E)),((_tmpA7E[0]=((_tmpA7D.tag=4,((_tmpA7D.f1=_tmp55F,((_tmpA7D.f2=0,((_tmpA7D.f3=1,_tmpA7D)))))))),_tmpA7E)))),((_tmpA7C->tl=tms,_tmpA7C))))));}{
struct _tuple12 _tmpA7F;return(_tmpA7F.f1=_tmp584,((_tmpA7F.f2=_tmp585,((_tmpA7F.f3=tms,_tmpA7F)))));};};};_LL2E4: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp553;if(_tmp568->tag != 1)goto _LL2E6;else{_tmp569=_tmp568->f1;_tmp56A=(void*)_tmp568->f2;_tmp56B=_tmp568->f3;}}_LL2E5:
# 2084
 if(_tmp56A == 0){
struct _tuple12 _tmpA80;return(_tmpA80.f1=tq,((_tmpA80.f2=t,((_tmpA80.f3=0,_tmpA80)))));}else{
# 2087
return Cyc_Absynpp_to_tms(r,tq,_tmp56A);}_LL2E6: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp56C=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp553;if(_tmp56C->tag != 17)goto _LL2E8;else{_tmp56D=_tmp56C->f1;_tmp56E=_tmp56C->f2;_tmp56F=_tmp56C->f3;_tmp570=(void*)_tmp56C->f4;}}_LL2E7:
# 2092
 if((_tmp570 == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype(_tmp570)){
struct _tuple12 _tmpA81;return(_tmpA81.f1=tq,((_tmpA81.f2=t,((_tmpA81.f3=0,_tmpA81)))));}else{
# 2095
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,_tmp570);}_LL2E8:;_LL2E9: {
# 2100
struct _tuple12 _tmpA82;return(_tmpA82.f1=tq,((_tmpA82.f2=t,((_tmpA82.f3=0,_tmpA82)))));}_LL2DD:;}
# 2104
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2106
void*_tmp5A2=t;void*_tmp5A4;void*_tmp5A6;_LL2F7:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A3=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A2;if(_tmp5A3->tag != 1)goto _LL2F9;else{_tmp5A4=(void*)_tmp5A3->f2;}}if(!(_tmp5A4 != 0))goto _LL2F9;_LL2F8:
 return Cyc_Absynpp_is_char_ptr(_tmp5A4);_LL2F9: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A5=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2;if(_tmp5A5->tag != 5)goto _LL2FB;else{_tmp5A6=(_tmp5A5->f1).elt_typ;}}_LL2FA:
# 2109
 L: {
void*_tmp5A7=_tmp5A6;void*_tmp5A9;void*_tmp5AB;_LL2FE:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A7;if(_tmp5A8->tag != 1)goto _LL300;else{_tmp5A9=(void*)_tmp5A8->f2;}}if(!(_tmp5A9 != 0))goto _LL300;_LL2FF:
 _tmp5A6=_tmp5A9;goto L;_LL300:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5AA=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A7;if(_tmp5AA->tag != 17)goto _LL302;else{_tmp5AB=(void*)_tmp5AA->f4;}}if(!(_tmp5AB != 0))goto _LL302;_LL301:
 _tmp5A6=_tmp5AB;goto L;_LL302: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5AC=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A7;if(_tmp5AC->tag != 6)goto _LL304;else{if(_tmp5AC->f2 != Cyc_Absyn_Char_sz)goto _LL304;}}_LL303:
 return 1;_LL304:;_LL305:
 return 0;_LL2FD:;}_LL2FB:;_LL2FC:
# 2116
 return 0;_LL2F6:;}
# 2120
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5AD=_new_region("temp");struct _RegionHandle*temp=& _tmp5AD;_push_region(temp);
{struct _tuple12 _stmttmp23=Cyc_Absynpp_to_tms(temp,tq,typ);struct Cyc_Absyn_Tqual _tmp5AF;void*_tmp5B0;struct Cyc_List_List*_tmp5B1;struct _tuple12 _tmp5AE=_stmttmp23;_tmp5AF=_tmp5AE.f1;_tmp5B0=_tmp5AE.f2;_tmp5B1=_tmp5AE.f3;
_tmp5B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5B1);
if(_tmp5B1 == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA83[2];struct Cyc_PP_Doc*_tmp5B3=(_tmpA83[1]=Cyc_Absynpp_ntyp2doc(_tmp5B0),((_tmpA83[0]=Cyc_Absynpp_tqual2doc(_tmp5AF),Cyc_PP_cat(_tag_dyneither(_tmpA83,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5B3;}else{
# 2127
const char*_tmpA86;struct Cyc_PP_Doc*_tmpA85[4];struct Cyc_PP_Doc*_tmp5B6=
(_tmpA85[3]=
# 2131
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5B1),((_tmpA85[2]=
# 2130
Cyc_PP_text(((_tmpA86=" ",_tag_dyneither(_tmpA86,sizeof(char),2)))),((_tmpA85[1]=
# 2129
Cyc_Absynpp_ntyp2doc(_tmp5B0),((_tmpA85[0]=
# 2128
Cyc_Absynpp_tqual2doc(_tmp5AF),Cyc_PP_cat(_tag_dyneither(_tmpA85,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B6;}}
# 2122
;_pop_region(temp);}
# 2135
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
struct Cyc_PP_Doc*requires_doc;
struct Cyc_Absyn_Exp*_tmp5B7=f->requires_clause;
if((unsigned int)_tmp5B7){
const char*_tmpA89;struct Cyc_PP_Doc*_tmpA88[2];requires_doc=((_tmpA88[1]=Cyc_Absynpp_exp2doc(_tmp5B7),((_tmpA88[0]=Cyc_PP_text(((_tmpA89="@requires ",_tag_dyneither(_tmpA89,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmpA88,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 2141
requires_doc=Cyc_PP_nil_doc();}
# 2143
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL306:
# 2146
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA90;const char*_tmpA8F;const char*_tmpA8E;struct Cyc_PP_Doc*_tmpA8D[5];return(_tmpA8D[4]=
# 2149
Cyc_PP_text(((_tmpA8E=";",_tag_dyneither(_tmpA8E,sizeof(char),2)))),((_tmpA8D[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA8D[2]=
# 2148
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA8D[1]=Cyc_PP_text(((_tmpA8F=":",_tag_dyneither(_tmpA8F,sizeof(char),2)))),((_tmpA8D[0]=
# 2147
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA90=_cycalloc(sizeof(*_tmpA90)),((_tmpA90->v=Cyc_PP_textptr(f->name),_tmpA90))))),Cyc_PP_cat(_tag_dyneither(_tmpA8D,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2151
struct Cyc_Core_Opt*_tmpA95;const char*_tmpA94;struct Cyc_PP_Doc*_tmpA93[4];return(_tmpA93[3]=
Cyc_PP_text(((_tmpA94=";",_tag_dyneither(_tmpA94,sizeof(char),2)))),((_tmpA93[2]=requires_doc,((_tmpA93[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA93[0]=
# 2151
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->v=Cyc_PP_textptr(f->name),_tmpA95))))),Cyc_PP_cat(_tag_dyneither(_tmpA93,sizeof(struct Cyc_PP_Doc*),4)))))))));}case Cyc_Cyclone_Vc_c: _LL307:
# 2154
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA9C;const char*_tmpA9B;const char*_tmpA9A;struct Cyc_PP_Doc*_tmpA99[5];return(_tmpA99[4]=
# 2157
Cyc_PP_text(((_tmpA9A=";",_tag_dyneither(_tmpA9A,sizeof(char),2)))),((_tmpA99[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA99[2]=Cyc_PP_text(((_tmpA9B=":",_tag_dyneither(_tmpA9B,sizeof(char),2)))),((_tmpA99[1]=
# 2156
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9C=_cycalloc(sizeof(*_tmpA9C)),((_tmpA9C->v=Cyc_PP_textptr(f->name),_tmpA9C))))),((_tmpA99[0]=
# 2155
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA99,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2159
struct Cyc_Core_Opt*_tmpAA1;const char*_tmpAA0;struct Cyc_PP_Doc*_tmpA9F[4];return(_tmpA9F[3]=
# 2161
Cyc_PP_text(((_tmpAA0=";",_tag_dyneither(_tmpAA0,sizeof(char),2)))),((_tmpA9F[2]=requires_doc,((_tmpA9F[1]=
# 2160
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->v=Cyc_PP_textptr(f->name),_tmpAA1))))),((_tmpA9F[0]=
# 2159
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA9F,sizeof(struct Cyc_PP_Doc*),4)))))))));}}}
# 2166
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAA2;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAA2="",_tag_dyneither(_tmpAA2,sizeof(char),1))),fields);}
# 2170
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAA3[3];return(_tmpAA3[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAA3[1]=
# 2171
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAA3[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAA3,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2175
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAA4;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAA4=",",_tag_dyneither(_tmpAA4,sizeof(char),2))),fields);}
# 2184
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpAA7;void*_tmpAA6;(_tmpAA6=0,Cyc_fprintf(f,((_tmpAA7="\n",_tag_dyneither(_tmpAA7,sizeof(char),2))),_tag_dyneither(_tmpAA6,sizeof(void*),0)));};}}
# 2191
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpAA8;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAA8="",_tag_dyneither(_tmpAA8,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
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
