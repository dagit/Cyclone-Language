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
enum Cyc_Absyn_KindQual _tmp70;enum Cyc_Absyn_AliasQual _tmp71;struct Cyc_Absyn_Kind*_tmp6F=ka;_tmp70=(*_tmp6F).kind;_tmp71=(*_tmp6F).aliasqual;
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
return Cyc_PP_text(_tmp96);}
# 433
struct Cyc_PP_Doc*Cyc_Absynpp_ktvar2doc(struct Cyc_Absyn_Tvar*tv){
void*_tmp97=Cyc_Absyn_compress_kb(tv->kind);struct Cyc_Absyn_Kind*_tmp9B;struct Cyc_Absyn_Kind*_tmp9D;_LL5C: {struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp98=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp98->tag != 1)goto _LL5E;}_LL5D:
 goto _LL5F;_LL5E: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp99=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp99->tag != 0)goto _LL60;else{if((*_tmp99->f1).kind != Cyc_Absyn_BoxKind)goto _LL60;if((*_tmp99->f1).aliasqual != Cyc_Absyn_Aliasable)goto _LL60;}}_LL5F:
 return Cyc_Absynpp_tvar2doc(tv);_LL60: {struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9A=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp9A->tag != 2)goto _LL62;else{_tmp9B=_tmp9A->f2;}}_LL61:
 _tmp9D=_tmp9B;goto _LL63;_LL62: {struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp9C=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp97;if(_tmp9C->tag != 0)goto _LL5B;else{_tmp9D=_tmp9C->f1;}}_LL63: {
const char*_tmp647;struct Cyc_PP_Doc*_tmp646[3];return(_tmp646[2]=Cyc_Absynpp_kind2doc(_tmp9D),((_tmp646[1]=Cyc_PP_text(((_tmp647="::",_tag_dyneither(_tmp647,sizeof(char),3)))),((_tmp646[0]=Cyc_Absynpp_tvar2doc(tv),Cyc_PP_cat(_tag_dyneither(_tmp646,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL5B:;}
# 442
struct Cyc_PP_Doc*Cyc_Absynpp_ktvars2doc(struct Cyc_List_List*tvs){
const char*_tmp64A;const char*_tmp649;const char*_tmp648;return Cyc_PP_egroup(((_tmp648="<",_tag_dyneither(_tmp648,sizeof(char),2))),((_tmp649=">",_tag_dyneither(_tmp649,sizeof(char),2))),((_tmp64A=",",_tag_dyneither(_tmp64A,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_ktvar2doc,tvs));}
# 446
static struct _dyneither_ptr*Cyc_Absynpp_get_name(struct Cyc_Absyn_Tvar*tv){
return tv->name;}
# 450
struct Cyc_PP_Doc*Cyc_Absynpp_tvars2doc(struct Cyc_List_List*tvs){
if(Cyc_Absynpp_print_all_kinds)
return Cyc_Absynpp_ktvars2doc(tvs);{
const char*_tmp64D;const char*_tmp64C;const char*_tmp64B;return Cyc_PP_egroup(((_tmp64B="<",_tag_dyneither(_tmp64B,sizeof(char),2))),((_tmp64C=">",_tag_dyneither(_tmp64C,sizeof(char),2))),((_tmp64D=",",_tag_dyneither(_tmp64D,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,tvs)));};}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 456
struct Cyc_PP_Doc*Cyc_Absynpp_arg2doc(struct _tuple15*t){
return Cyc_Absynpp_tqtd2doc((*t).f1,(*t).f2,0);}
# 460
struct Cyc_PP_Doc*Cyc_Absynpp_args2doc(struct Cyc_List_List*ts){
const char*_tmp650;const char*_tmp64F;const char*_tmp64E;return Cyc_PP_group(((_tmp64E="(",_tag_dyneither(_tmp64E,sizeof(char),2))),((_tmp64F=")",_tag_dyneither(_tmp64F,sizeof(char),2))),((_tmp650=",",_tag_dyneither(_tmp650,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg2doc,ts));}
# 464
struct Cyc_PP_Doc*Cyc_Absynpp_noncallatt2doc(void*att){
void*_tmpA9=att;_LL65: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAA=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpA9;if(_tmpAA->tag != 1)goto _LL67;}_LL66:
 goto _LL68;_LL67: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAB=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpA9;if(_tmpAB->tag != 2)goto _LL69;}_LL68:
 goto _LL6A;_LL69: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpAC=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpA9;if(_tmpAC->tag != 3)goto _LL6B;}_LL6A:
 return Cyc_PP_nil_doc();_LL6B:;_LL6C:
 return Cyc_PP_text(Cyc_Absyn_attribute2string(att));_LL64:;}
# 473
struct Cyc_PP_Doc*Cyc_Absynpp_callconv2doc(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmpAD=(void*)atts->hd;_LL6E: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpAE=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpAD;if(_tmpAE->tag != 1)goto _LL70;}_LL6F: {
const char*_tmp651;return Cyc_PP_text(((_tmp651=" _stdcall ",_tag_dyneither(_tmp651,sizeof(char),11))));}_LL70: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpAF=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpAD;if(_tmpAF->tag != 2)goto _LL72;}_LL71: {
const char*_tmp652;return Cyc_PP_text(((_tmp652=" _cdecl ",_tag_dyneither(_tmp652,sizeof(char),9))));}_LL72: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB0=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpAD;if(_tmpB0->tag != 3)goto _LL74;}_LL73: {
const char*_tmp653;return Cyc_PP_text(((_tmp653=" _fastcall ",_tag_dyneither(_tmp653,sizeof(char),12))));}_LL74:;_LL75:
 goto _LL6D;_LL6D:;}
# 481
return Cyc_PP_nil_doc();}
# 484
struct Cyc_PP_Doc*Cyc_Absynpp_noncallconv2doc(struct Cyc_List_List*atts){
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmpB4=(void*)atts2->hd;_LL77: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmpB5=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmpB4;if(_tmpB5->tag != 1)goto _LL79;}_LL78:
 goto _LL7A;_LL79: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmpB6=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmpB4;if(_tmpB6->tag != 2)goto _LL7B;}_LL7A:
 goto _LL7C;_LL7B: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmpB7=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmpB4;if(_tmpB7->tag != 3)goto _LL7D;}_LL7C:
 goto _LL76;_LL7D:;_LL7E:
 hasatt=1;goto _LL76;_LL76:;}}
# 493
if(!hasatt)
return Cyc_PP_nil_doc();{
const char*_tmp65E;const char*_tmp65D;const char*_tmp65C;const char*_tmp65B;const char*_tmp65A;struct Cyc_PP_Doc*_tmp659[3];return(_tmp659[2]=
# 497
Cyc_PP_text(((_tmp65A=")",_tag_dyneither(_tmp65A,sizeof(char),2)))),((_tmp659[1]=
# 496
Cyc_PP_group(((_tmp65D="",_tag_dyneither(_tmp65D,sizeof(char),1))),((_tmp65C="",_tag_dyneither(_tmp65C,sizeof(char),1))),((_tmp65B=" ",_tag_dyneither(_tmp65B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_noncallatt2doc,atts)),((_tmp659[0]=
# 495
Cyc_PP_text(((_tmp65E=" __declspec(",_tag_dyneither(_tmp65E,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp659,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 500
struct Cyc_PP_Doc*Cyc_Absynpp_att2doc(void*a){
return Cyc_PP_text(Cyc_Absyn_attribute2string(a));}
# 504
struct Cyc_PP_Doc*Cyc_Absynpp_atts2doc(struct Cyc_List_List*atts){
if(atts == 0)return Cyc_PP_nil_doc();
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL7F:
 return Cyc_Absynpp_noncallconv2doc(atts);case Cyc_Cyclone_Gcc_c: _LL80: {
const char*_tmp667;const char*_tmp666;const char*_tmp665;const char*_tmp664;struct Cyc_PP_Doc*_tmp663[2];return(_tmp663[1]=
Cyc_PP_group(((_tmp666="((",_tag_dyneither(_tmp666,sizeof(char),3))),((_tmp665="))",_tag_dyneither(_tmp665,sizeof(char),3))),((_tmp664=",",_tag_dyneither(_tmp664,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_att2doc,atts)),((_tmp663[0]=
# 508
Cyc_PP_text(((_tmp667=" __attribute__",_tag_dyneither(_tmp667,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmp663,sizeof(struct Cyc_PP_Doc*),2)))));}}}
# 513
int Cyc_Absynpp_next_is_pointer(struct Cyc_List_List*tms){
if(tms == 0)return 0;{
void*_tmpC3=(void*)tms->hd;_LL83: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpC4=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC3;if(_tmpC4->tag != 2)goto _LL85;}_LL84:
 return 1;_LL85: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpC5=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC3;if(_tmpC5->tag != 5)goto _LL87;}_LL86:
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
const char*_tmp668;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp668="{?}",_tag_dyneither(_tmp668,sizeof(char),4))),1);}else{
const char*_tmp669;Cyc_Absynpp_cache_question=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp669="?",_tag_dyneither(_tmp669,sizeof(char),2))));}}
# 536
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_question);}
# 538
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_lb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_lb(){
if(!((unsigned int)Cyc_Absynpp_cache_lb)){
if(Cyc_PP_tex_output){
const char*_tmp66A;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp66A="{\\lb}",_tag_dyneither(_tmp66A,sizeof(char),6))),1);}else{
const char*_tmp66B;Cyc_Absynpp_cache_lb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp66B="{",_tag_dyneither(_tmp66B,sizeof(char),2))));}}
# 545
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_lb);}
# 547
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_rb=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_rb(){
if(!((unsigned int)Cyc_Absynpp_cache_rb)){
if(Cyc_PP_tex_output){
const char*_tmp66C;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp66C="{\\rb}",_tag_dyneither(_tmp66C,sizeof(char),6))),1);}else{
const char*_tmp66D;Cyc_Absynpp_cache_rb=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp66D="}",_tag_dyneither(_tmp66D,sizeof(char),2))));}}
# 554
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_rb);}
# 556
static struct Cyc_PP_Doc*Cyc_Absynpp_cache_dollar=0;
static struct Cyc_PP_Doc*Cyc_Absynpp_dollar(){
if(!((unsigned int)Cyc_Absynpp_cache_dollar)){
if(Cyc_PP_tex_output){
const char*_tmp66E;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text_width(((_tmp66E="\\$",_tag_dyneither(_tmp66E,sizeof(char),3))),1);}else{
const char*_tmp66F;Cyc_Absynpp_cache_dollar=(struct Cyc_PP_Doc*)Cyc_PP_text(((_tmp66F="$",_tag_dyneither(_tmp66F,sizeof(char),2))));}}
# 563
return(struct Cyc_PP_Doc*)_check_null(Cyc_Absynpp_cache_dollar);}
# 565
struct Cyc_PP_Doc*Cyc_Absynpp_group_braces(struct _dyneither_ptr sep,struct Cyc_List_List*ss){
struct Cyc_PP_Doc*_tmp670[3];return(_tmp670[2]=Cyc_Absynpp_rb(),((_tmp670[1]=Cyc_PP_seq(sep,ss),((_tmp670[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp670,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 570
static void Cyc_Absynpp_print_tms(struct Cyc_List_List*tms){
for(0;tms != 0;tms=tms->tl){
void*_tmpCF=(void*)tms->hd;struct Cyc_List_List*_tmpD4;_LL8D: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpD0=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD0->tag != 0)goto _LL8F;}_LL8E:
{const char*_tmp673;void*_tmp672;(_tmp672=0,Cyc_fprintf(Cyc_stderr,((_tmp673="Carray_mod ",_tag_dyneither(_tmp673,sizeof(char),12))),_tag_dyneither(_tmp672,sizeof(void*),0)));}goto _LL8C;_LL8F: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpD1=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD1->tag != 1)goto _LL91;}_LL90:
{const char*_tmp676;void*_tmp675;(_tmp675=0,Cyc_fprintf(Cyc_stderr,((_tmp676="ConstArray_mod ",_tag_dyneither(_tmp676,sizeof(char),16))),_tag_dyneither(_tmp675,sizeof(void*),0)));}goto _LL8C;_LL91: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD2=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD2->tag != 3)goto _LL93;else{struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpD3=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((void*)_tmpD2->f1);if(_tmpD3->tag != 1)goto _LL93;else{_tmpD4=_tmpD3->f1;}}}_LL92:
# 576
{const char*_tmp679;void*_tmp678;(_tmp678=0,Cyc_fprintf(Cyc_stderr,((_tmp679="Function_mod(",_tag_dyneither(_tmp679,sizeof(char),14))),_tag_dyneither(_tmp678,sizeof(void*),0)));}
for(0;_tmpD4 != 0;_tmpD4=_tmpD4->tl){
struct _dyneither_ptr*_tmpDF=(*((struct _tuple8*)_tmpD4->hd)).f1;
if(_tmpDF == 0){const char*_tmp67C;void*_tmp67B;(_tmp67B=0,Cyc_fprintf(Cyc_stderr,((_tmp67C="?",_tag_dyneither(_tmp67C,sizeof(char),2))),_tag_dyneither(_tmp67B,sizeof(void*),0)));}else{
void*_tmp67D;(_tmp67D=0,Cyc_fprintf(Cyc_stderr,*_tmpDF,_tag_dyneither(_tmp67D,sizeof(void*),0)));}
if(_tmpD4->tl != 0){const char*_tmp680;void*_tmp67F;(_tmp67F=0,Cyc_fprintf(Cyc_stderr,((_tmp680=",",_tag_dyneither(_tmp680,sizeof(char),2))),_tag_dyneither(_tmp67F,sizeof(void*),0)));}}
# 583
{const char*_tmp683;void*_tmp682;(_tmp682=0,Cyc_fprintf(Cyc_stderr,((_tmp683=") ",_tag_dyneither(_tmp683,sizeof(char),3))),_tag_dyneither(_tmp682,sizeof(void*),0)));}
goto _LL8C;_LL93: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpD5=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD5->tag != 3)goto _LL95;}_LL94:
# 586
{const char*_tmp686;void*_tmp685;(_tmp685=0,Cyc_fprintf(Cyc_stderr,((_tmp686="Function_mod()",_tag_dyneither(_tmp686,sizeof(char),15))),_tag_dyneither(_tmp685,sizeof(void*),0)));}goto _LL8C;_LL95: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpD6=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD6->tag != 5)goto _LL97;}_LL96:
{const char*_tmp689;void*_tmp688;(_tmp688=0,Cyc_fprintf(Cyc_stderr,((_tmp689="Attributes_mod ",_tag_dyneither(_tmp689,sizeof(char),16))),_tag_dyneither(_tmp688,sizeof(void*),0)));}goto _LL8C;_LL97: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpD7=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD7->tag != 4)goto _LL99;}_LL98:
{const char*_tmp68C;void*_tmp68B;(_tmp68B=0,Cyc_fprintf(Cyc_stderr,((_tmp68C="TypeParams_mod ",_tag_dyneither(_tmp68C,sizeof(char),16))),_tag_dyneither(_tmp68B,sizeof(void*),0)));}goto _LL8C;_LL99: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpD8=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCF;if(_tmpD8->tag != 2)goto _LL8C;}_LL9A:
{const char*_tmp68F;void*_tmp68E;(_tmp68E=0,Cyc_fprintf(Cyc_stderr,((_tmp68F="Pointer_mod ",_tag_dyneither(_tmp68F,sizeof(char),13))),_tag_dyneither(_tmp68E,sizeof(void*),0)));}goto _LL8C;_LL8C:;}{
# 591
const char*_tmp692;void*_tmp691;(_tmp691=0,Cyc_fprintf(Cyc_stderr,((_tmp692="\n",_tag_dyneither(_tmp692,sizeof(char),2))),_tag_dyneither(_tmp691,sizeof(void*),0)));};}
# 594
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t);
# 596
struct Cyc_PP_Doc*Cyc_Absynpp_dtms2doc(int is_char_ptr,struct Cyc_PP_Doc*d,struct Cyc_List_List*tms){
if(tms == 0)return d;{
struct Cyc_PP_Doc*rest=Cyc_Absynpp_dtms2doc(0,d,tms->tl);
const char*_tmp697;const char*_tmp696;struct Cyc_PP_Doc*_tmp695[3];struct Cyc_PP_Doc*p_rest=(_tmp695[2]=Cyc_PP_text(((_tmp696=")",_tag_dyneither(_tmp696,sizeof(char),2)))),((_tmp695[1]=rest,((_tmp695[0]=Cyc_PP_text(((_tmp697="(",_tag_dyneither(_tmp697,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp695,sizeof(struct Cyc_PP_Doc*),3)))))));
void*_tmpF1=(void*)tms->hd;union Cyc_Absyn_Constraint*_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;union Cyc_Absyn_Constraint*_tmpF6;void*_tmpF8;struct Cyc_List_List*_tmpFA;struct Cyc_List_List*_tmpFC;unsigned int _tmpFD;int _tmpFE;void*_tmp100;union Cyc_Absyn_Constraint*_tmp101;union Cyc_Absyn_Constraint*_tmp102;union Cyc_Absyn_Constraint*_tmp103;struct Cyc_Absyn_Tqual _tmp104;_LL9C: {struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpF2=(struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpF2->tag != 0)goto _LL9E;else{_tmpF3=_tmpF2->f1;}}_LL9D:
# 602
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp69C;const char*_tmp69B;struct Cyc_PP_Doc*_tmp69A[2];return(_tmp69A[1]=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF3)?
Cyc_PP_text(((_tmp69B="[]@zeroterm",_tag_dyneither(_tmp69B,sizeof(char),12)))): Cyc_PP_text(((_tmp69C="[]",_tag_dyneither(_tmp69C,sizeof(char),3)))),((_tmp69A[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp69A,sizeof(struct Cyc_PP_Doc*),2)))));};_LL9E: {struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpF4=(struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpF4->tag != 1)goto _LLA0;else{_tmpF5=_tmpF4->f1;_tmpF6=_tmpF4->f2;}}_LL9F:
# 606
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
const char*_tmp6A3;const char*_tmp6A2;const char*_tmp6A1;struct Cyc_PP_Doc*_tmp6A0[4];return(_tmp6A0[3]=
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpF6)?Cyc_PP_text(((_tmp6A1="]@zeroterm",_tag_dyneither(_tmp6A1,sizeof(char),11)))): Cyc_PP_text(((_tmp6A2="]",_tag_dyneither(_tmp6A2,sizeof(char),2)))),((_tmp6A0[2]=
# 607
Cyc_Absynpp_exp2doc(_tmpF5),((_tmp6A0[1]=Cyc_PP_text(((_tmp6A3="[",_tag_dyneither(_tmp6A3,sizeof(char),2)))),((_tmp6A0[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A0,sizeof(struct Cyc_PP_Doc*),4)))))))));};_LLA0: {struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpF7=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpF7->tag != 3)goto _LLA2;else{_tmpF8=(void*)_tmpF7->f1;}}_LLA1:
# 610
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
void*_tmp10C=_tmpF8;struct Cyc_List_List*_tmp10E;int _tmp10F;struct Cyc_Absyn_VarargInfo*_tmp110;void*_tmp111;struct Cyc_List_List*_tmp112;struct Cyc_List_List*_tmp114;unsigned int _tmp115;_LLA9: {struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp10D=(struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp10C;if(_tmp10D->tag != 1)goto _LLAB;else{_tmp10E=_tmp10D->f1;_tmp10F=_tmp10D->f2;_tmp110=_tmp10D->f3;_tmp111=(void*)_tmp10D->f4;_tmp112=_tmp10D->f5;}}_LLAA: {
# 613
struct Cyc_PP_Doc*_tmp6A4[2];return(_tmp6A4[1]=Cyc_Absynpp_funargs2doc(_tmp10E,_tmp10F,_tmp110,_tmp111,_tmp112),((_tmp6A4[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A4,sizeof(struct Cyc_PP_Doc*),2)))));}_LLAB: {struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp113=(struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp10C;if(_tmp113->tag != 0)goto _LLA8;else{_tmp114=_tmp113->f1;_tmp115=_tmp113->f2;}}_LLAC: {
# 615
const char*_tmp6AB;const char*_tmp6AA;const char*_tmp6A9;struct Cyc_PP_Doc*_tmp6A8[2];return(_tmp6A8[1]=Cyc_PP_group(((_tmp6AB="(",_tag_dyneither(_tmp6AB,sizeof(char),2))),((_tmp6AA=")",_tag_dyneither(_tmp6AA,sizeof(char),2))),((_tmp6A9=",",_tag_dyneither(_tmp6A9,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,_tmp114)),((_tmp6A8[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6A8,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA8:;};_LLA2: {struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpF9=(struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpF9->tag != 5)goto _LLA4;else{_tmpFA=_tmpF9->f2;}}_LLA3:
# 618
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LLAD:
# 620
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;{
struct Cyc_PP_Doc*_tmp6AC[2];return(_tmp6AC[1]=Cyc_Absynpp_atts2doc(_tmpFA),((_tmp6AC[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AC,sizeof(struct Cyc_PP_Doc*),2)))));};case Cyc_Cyclone_Vc_c: _LLAE:
# 623
 if(Cyc_Absynpp_next_is_pointer(tms->tl)){
struct Cyc_PP_Doc*_tmp6AD[2];return(_tmp6AD[1]=rest,((_tmp6AD[0]=Cyc_Absynpp_callconv2doc(_tmpFA),Cyc_PP_cat(_tag_dyneither(_tmp6AD,sizeof(struct Cyc_PP_Doc*),2)))));}
return rest;}_LLA4: {struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpFB=(struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpFB->tag != 4)goto _LLA6;else{_tmpFC=_tmpFB->f1;_tmpFD=_tmpFB->f2;_tmpFE=_tmpFB->f3;}}_LLA5:
# 628
 if(Cyc_Absynpp_next_is_pointer(tms->tl))rest=p_rest;
if(_tmpFE){
struct Cyc_PP_Doc*_tmp6AE[2];return(_tmp6AE[1]=Cyc_Absynpp_ktvars2doc(_tmpFC),((_tmp6AE[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AE,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 632
struct Cyc_PP_Doc*_tmp6AF[2];return(_tmp6AF[1]=Cyc_Absynpp_tvars2doc(_tmpFC),((_tmp6AF[0]=rest,Cyc_PP_cat(_tag_dyneither(_tmp6AF,sizeof(struct Cyc_PP_Doc*),2)))));}_LLA6: {struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpFF=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpF1;if(_tmpFF->tag != 2)goto _LL9B;else{_tmp100=(_tmpFF->f1).rgn;_tmp101=(_tmpFF->f1).nullable;_tmp102=(_tmpFF->f1).bounds;_tmp103=(_tmpFF->f1).zero_term;_tmp104=_tmpFF->f2;}}_LLA7: {
# 636
struct Cyc_PP_Doc*ptr;
{void*_tmp11F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp102);struct Cyc_Absyn_Exp*_tmp122;_LLB1: {struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*_tmp120=(struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp11F;if(_tmp120->tag != 0)goto _LLB3;}_LLB2:
 ptr=Cyc_Absynpp_question();goto _LLB0;_LLB3: {struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp121=(struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp11F;if(_tmp121->tag != 1)goto _LLB0;else{_tmp122=_tmp121->f1;}}_LLB4:
# 640
{const char*_tmp6B1;const char*_tmp6B0;ptr=Cyc_PP_text(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp101)?(_tmp6B1="*",_tag_dyneither(_tmp6B1,sizeof(char),2)):((_tmp6B0="@",_tag_dyneither(_tmp6B0,sizeof(char),2))));}{
unsigned int _tmp126;int _tmp127;struct _tuple11 _tmp125=Cyc_Evexp_eval_const_uint_exp(_tmp122);_tmp126=_tmp125.f1;_tmp127=_tmp125.f2;
if(!_tmp127  || _tmp126 != 1){
struct Cyc_PP_Doc*_tmp6B2[4];ptr=((_tmp6B2[3]=Cyc_Absynpp_rb(),((_tmp6B2[2]=Cyc_Absynpp_exp2doc(_tmp122),((_tmp6B2[1]=Cyc_Absynpp_lb(),((_tmp6B2[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LLB0;};_LLB0:;}
# 646
if(Cyc_Absynpp_print_zeroterm){
if(!is_char_ptr  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp103)){
const char*_tmp6B5;struct Cyc_PP_Doc*_tmp6B4[2];ptr=((_tmp6B4[1]=Cyc_PP_text(((_tmp6B5="@zeroterm ",_tag_dyneither(_tmp6B5,sizeof(char),11)))),((_tmp6B4[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B4,sizeof(struct Cyc_PP_Doc*),2))))));}else{
if(is_char_ptr  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp103)){
const char*_tmp6B8;struct Cyc_PP_Doc*_tmp6B7[2];ptr=((_tmp6B7[1]=Cyc_PP_text(((_tmp6B8="@nozeroterm ",_tag_dyneither(_tmp6B8,sizeof(char),13)))),((_tmp6B7[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6B7,sizeof(struct Cyc_PP_Doc*),2))))));}}}
# 652
{void*_tmp12D=Cyc_Tcutil_compress(_tmp100);_LLB6: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp12E=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp12D;if(_tmp12E->tag != 20)goto _LLB8;}_LLB7:
 goto _LLB5;_LLB8:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp12F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp12D;if(_tmp12F->tag != 1)goto _LLBA;}if(!Cyc_Absynpp_print_for_cycdoc)goto _LLBA;_LLB9:
 goto _LLB5;_LLBA:;_LLBB: {
const char*_tmp6BB;struct Cyc_PP_Doc*_tmp6BA[3];ptr=((_tmp6BA[2]=Cyc_PP_text(((_tmp6BB=" ",_tag_dyneither(_tmp6BB,sizeof(char),2)))),((_tmp6BA[1]=Cyc_Absynpp_rgn2doc(_tmp100),((_tmp6BA[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BA,sizeof(struct Cyc_PP_Doc*),3))))))));}_LLB5:;}
# 657
{struct Cyc_PP_Doc*_tmp6BC[2];ptr=((_tmp6BC[1]=Cyc_Absynpp_tqual2doc(_tmp104),((_tmp6BC[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BC,sizeof(struct Cyc_PP_Doc*),2))))));}{
struct Cyc_PP_Doc*_tmp6BD[2];return(_tmp6BD[1]=rest,((_tmp6BD[0]=ptr,Cyc_PP_cat(_tag_dyneither(_tmp6BD,sizeof(struct Cyc_PP_Doc*),2)))));};}_LL9B:;};}
# 662
struct Cyc_PP_Doc*Cyc_Absynpp_rgn2doc(void*t){
void*_tmp137=Cyc_Tcutil_compress(t);_LLBD: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp138=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp137;if(_tmp138->tag != 20)goto _LLBF;}_LLBE: {
const char*_tmp6BE;return Cyc_PP_text(((_tmp6BE="`H",_tag_dyneither(_tmp6BE,sizeof(char),3))));}_LLBF: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp139=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp137;if(_tmp139->tag != 21)goto _LLC1;}_LLC0: {
const char*_tmp6BF;return Cyc_PP_text(((_tmp6BF="`U",_tag_dyneither(_tmp6BF,sizeof(char),3))));}_LLC1: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp13A=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp137;if(_tmp13A->tag != 22)goto _LLC3;}_LLC2: {
const char*_tmp6C0;return Cyc_PP_text(((_tmp6C0="`RC",_tag_dyneither(_tmp6C0,sizeof(char),4))));}_LLC3: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13B=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp137;if(_tmp13B->tag != 2)goto _LLC5;}_LLC4:
 return Cyc_Absynpp_ntyp2doc(t);_LLC5:;_LLC6: {
const char*_tmp6C5;const char*_tmp6C4;struct Cyc_PP_Doc*_tmp6C3[3];return(_tmp6C3[2]=Cyc_PP_text(((_tmp6C4=")",_tag_dyneither(_tmp6C4,sizeof(char),2)))),((_tmp6C3[1]=Cyc_Absynpp_ntyp2doc(t),((_tmp6C3[0]=Cyc_PP_text(((_tmp6C5="@region(",_tag_dyneither(_tmp6C5,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp6C3,sizeof(struct Cyc_PP_Doc*),3)))))));}_LLBC:;}
# 672
static void Cyc_Absynpp_effects2docs(struct Cyc_List_List**rgions,struct Cyc_List_List**effects,void*t){
# 676
void*_tmp142=Cyc_Tcutil_compress(t);void*_tmp144;struct Cyc_List_List*_tmp146;_LLC8: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp143=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp142;if(_tmp143->tag != 23)goto _LLCA;else{_tmp144=(void*)_tmp143->f1;}}_LLC9:
{struct Cyc_List_List*_tmp6C6;*rgions=((_tmp6C6=_cycalloc(sizeof(*_tmp6C6)),((_tmp6C6->hd=Cyc_Absynpp_rgn2doc(_tmp144),((_tmp6C6->tl=*rgions,_tmp6C6))))));}goto _LLC7;_LLCA: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp145=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp142;if(_tmp145->tag != 24)goto _LLCC;else{_tmp146=_tmp145->f1;}}_LLCB:
# 679
 for(0;_tmp146 != 0;_tmp146=_tmp146->tl){
Cyc_Absynpp_effects2docs(rgions,effects,(void*)_tmp146->hd);}
# 682
goto _LLC7;_LLCC:;_LLCD:
{struct Cyc_List_List*_tmp6C7;*effects=((_tmp6C7=_cycalloc(sizeof(*_tmp6C7)),((_tmp6C7->hd=Cyc_Absynpp_typ2doc(t),((_tmp6C7->tl=*effects,_tmp6C7))))));}goto _LLC7;_LLC7:;}
# 687
struct Cyc_PP_Doc*Cyc_Absynpp_eff2doc(void*t){
struct Cyc_List_List*rgions=0;struct Cyc_List_List*effects=0;
Cyc_Absynpp_effects2docs(& rgions,& effects,t);
rgions=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rgions);
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
if(rgions == 0  && effects != 0){
const char*_tmp6CA;const char*_tmp6C9;const char*_tmp6C8;return Cyc_PP_group(((_tmp6C8="",_tag_dyneither(_tmp6C8,sizeof(char),1))),((_tmp6C9="",_tag_dyneither(_tmp6C9,sizeof(char),1))),((_tmp6CA="+",_tag_dyneither(_tmp6CA,sizeof(char),2))),effects);}else{
# 695
const char*_tmp6CB;struct Cyc_PP_Doc*_tmp14C=Cyc_Absynpp_group_braces(((_tmp6CB=",",_tag_dyneither(_tmp6CB,sizeof(char),2))),rgions);
struct Cyc_List_List*_tmp6CF;const char*_tmp6CE;const char*_tmp6CD;const char*_tmp6CC;return Cyc_PP_group(((_tmp6CC="",_tag_dyneither(_tmp6CC,sizeof(char),1))),((_tmp6CD="",_tag_dyneither(_tmp6CD,sizeof(char),1))),((_tmp6CE="+",_tag_dyneither(_tmp6CE,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(effects,((_tmp6CF=_cycalloc(sizeof(*_tmp6CF)),((_tmp6CF->hd=_tmp14C,((_tmp6CF->tl=0,_tmp6CF))))))));}}
# 700
struct Cyc_PP_Doc*Cyc_Absynpp_aggr_kind2doc(enum Cyc_Absyn_AggrKind k){
switch(k){case Cyc_Absyn_StructA: _LLCE: {
const char*_tmp6D0;return Cyc_PP_text(((_tmp6D0="struct ",_tag_dyneither(_tmp6D0,sizeof(char),8))));}case Cyc_Absyn_UnionA: _LLCF: {
const char*_tmp6D1;return Cyc_PP_text(((_tmp6D1="union ",_tag_dyneither(_tmp6D1,sizeof(char),7))));}}}
# 708
struct Cyc_PP_Doc*Cyc_Absynpp_ntyp2doc(void*t){
struct Cyc_PP_Doc*s;
{void*_tmp154=t;struct Cyc_Core_Opt*_tmp15A;void*_tmp15B;int _tmp15C;struct Cyc_Core_Opt*_tmp15D;struct Cyc_Absyn_Tvar*_tmp15F;union Cyc_Absyn_DatatypeInfoU _tmp161;struct Cyc_List_List*_tmp162;union Cyc_Absyn_DatatypeFieldInfoU _tmp164;struct Cyc_List_List*_tmp165;enum Cyc_Absyn_Sign _tmp167;enum Cyc_Absyn_Size_of _tmp168;int _tmp16A;struct Cyc_List_List*_tmp16C;union Cyc_Absyn_AggrInfoU _tmp16E;struct Cyc_List_List*_tmp16F;enum Cyc_Absyn_AggrKind _tmp171;struct Cyc_List_List*_tmp172;struct Cyc_List_List*_tmp174;struct _tuple0*_tmp176;struct Cyc_Absyn_Exp*_tmp178;struct _tuple0*_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Typedefdecl*_tmp17C;void*_tmp17E;void*_tmp180;void*_tmp181;void*_tmp183;void*_tmp188;struct Cyc_Absyn_Aggrdecl*_tmp18D;struct Cyc_Absyn_Enumdecl*_tmp190;struct Cyc_Absyn_Datatypedecl*_tmp193;_LLD2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp155=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp154;if(_tmp155->tag != 8)goto _LLD4;}_LLD3: {
# 712
const char*_tmp6D2;return Cyc_PP_text(((_tmp6D2="[[[array]]]",_tag_dyneither(_tmp6D2,sizeof(char),12))));}_LLD4: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp156=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp154;if(_tmp156->tag != 9)goto _LLD6;}_LLD5:
 return Cyc_PP_nil_doc();_LLD6: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp157=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp154;if(_tmp157->tag != 5)goto _LLD8;}_LLD7:
 return Cyc_PP_nil_doc();_LLD8: {struct Cyc_Absyn_VoidType_Absyn_Type_struct*_tmp158=(struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp154;if(_tmp158->tag != 0)goto _LLDA;}_LLD9:
# 716
{const char*_tmp6D3;s=Cyc_PP_text(((_tmp6D3="void",_tag_dyneither(_tmp6D3,sizeof(char),5))));}goto _LLD1;_LLDA: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp159=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp154;if(_tmp159->tag != 1)goto _LLDC;else{_tmp15A=_tmp159->f1;_tmp15B=(void*)_tmp159->f2;_tmp15C=_tmp159->f3;_tmp15D=_tmp159->f4;}}_LLDB:
# 718
 if(_tmp15B != 0)
# 720
return Cyc_Absynpp_ntyp2doc((void*)_tmp15B);else{
# 728
const char*_tmp6E3;const char*_tmp6E2;const char*_tmp6E1;struct Cyc_Int_pa_PrintArg_struct _tmp6E0;void*_tmp6DF[1];const char*_tmp6DE;const char*_tmp6DD;struct Cyc_PP_Doc*_tmp6DC[6];s=((_tmp6DC[5]=
# 733
_tmp15A == 0?Cyc_Absynpp_question(): Cyc_Absynpp_kind2doc((struct Cyc_Absyn_Kind*)_tmp15A->v),((_tmp6DC[4]=Cyc_PP_text(((_tmp6DD=")::",_tag_dyneither(_tmp6DD,sizeof(char),4)))),((_tmp6DC[3]=
# 731
(!Cyc_Absynpp_print_full_evars  || _tmp15D == 0)?
Cyc_PP_nil_doc(): Cyc_Absynpp_tvars2doc((struct Cyc_List_List*)_tmp15D->v),((_tmp6DC[2]=
# 730
Cyc_PP_text((struct _dyneither_ptr)((_tmp6E0.tag=1,((_tmp6E0.f1=(unsigned long)_tmp15C,((_tmp6DF[0]=& _tmp6E0,Cyc_aprintf(((_tmp6DE="%d",_tag_dyneither(_tmp6DE,sizeof(char),3))),_tag_dyneither(_tmp6DF,sizeof(void*),1))))))))),((_tmp6DC[1]=
# 729
Cyc_PP_text(((_tmp6E1="(",_tag_dyneither(_tmp6E1,sizeof(char),2)))),((_tmp6DC[0]=
# 728
Cyc_Absynpp_print_for_cycdoc?Cyc_PP_text(((_tmp6E2="\\%",_tag_dyneither(_tmp6E2,sizeof(char),3)))): Cyc_PP_text(((_tmp6E3="%",_tag_dyneither(_tmp6E3,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp6DC,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}
# 736
goto _LLD1;_LLDC: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp15E=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp154;if(_tmp15E->tag != 2)goto _LLDE;else{_tmp15F=_tmp15E->f1;}}_LLDD:
# 738
 s=Cyc_PP_textptr(_tmp15F->name);
if(Cyc_Absynpp_print_all_kinds){
const char*_tmp6E6;struct Cyc_PP_Doc*_tmp6E5[3];s=((_tmp6E5[2]=Cyc_Absynpp_kindbound2doc(_tmp15F->kind),((_tmp6E5[1]=Cyc_PP_text(((_tmp6E6="::",_tag_dyneither(_tmp6E6,sizeof(char),3)))),((_tmp6E5[0]=s,Cyc_PP_cat(_tag_dyneither(_tmp6E5,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 744
if(Cyc_Absynpp_rewrite_temp_tvars  && Cyc_Tcutil_is_temp_tvar(_tmp15F)){
# 747
const char*_tmp6EB;const char*_tmp6EA;struct Cyc_PP_Doc*_tmp6E9[3];s=((_tmp6E9[2]=Cyc_PP_text(((_tmp6EA=" */",_tag_dyneither(_tmp6EA,sizeof(char),4)))),((_tmp6E9[1]=s,((_tmp6E9[0]=Cyc_PP_text(((_tmp6EB="_ /* ",_tag_dyneither(_tmp6EB,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp6E9,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 750
goto _LLD1;_LLDE: {struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp160=(struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp154;if(_tmp160->tag != 3)goto _LLE0;else{_tmp161=(_tmp160->f1).datatype_info;_tmp162=(_tmp160->f1).targs;}}_LLDF:
# 752
{union Cyc_Absyn_DatatypeInfoU _tmp1A3=_tmp161;struct _tuple0*_tmp1A4;int _tmp1A5;struct _tuple0*_tmp1A6;int _tmp1A7;_LL10D: if((_tmp1A3.UnknownDatatype).tag != 1)goto _LL10F;_tmp1A4=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A3.UnknownDatatype).val).name;_tmp1A5=((struct Cyc_Absyn_UnknownDatatypeInfo)(_tmp1A3.UnknownDatatype).val).is_extensible;_LL10E:
# 754
 _tmp1A6=_tmp1A4;_tmp1A7=_tmp1A5;goto _LL110;_LL10F: if((_tmp1A3.KnownDatatype).tag != 2)goto _LL10C;_tmp1A6=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A3.KnownDatatype).val))).name;_tmp1A7=(*(*((struct Cyc_Absyn_Datatypedecl**)(_tmp1A3.KnownDatatype).val))).is_extensible;_LL110: {
# 756
const char*_tmp6EC;struct Cyc_PP_Doc*_tmp1A8=Cyc_PP_text(((_tmp6EC="datatype ",_tag_dyneither(_tmp6EC,sizeof(char),10))));
const char*_tmp6ED;struct Cyc_PP_Doc*_tmp1A9=_tmp1A7?Cyc_PP_text(((_tmp6ED="@extensible ",_tag_dyneither(_tmp6ED,sizeof(char),13)))): Cyc_PP_nil_doc();
{struct Cyc_PP_Doc*_tmp6EE[4];s=((_tmp6EE[3]=Cyc_Absynpp_tps2doc(_tmp162),((_tmp6EE[2]=Cyc_Absynpp_qvar2doc(_tmp1A6),((_tmp6EE[1]=_tmp1A8,((_tmp6EE[0]=_tmp1A9,Cyc_PP_cat(_tag_dyneither(_tmp6EE,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL10C;}_LL10C:;}
# 761
goto _LLD1;_LLE0: {struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp163=(struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp154;if(_tmp163->tag != 4)goto _LLE2;else{_tmp164=(_tmp163->f1).field_info;_tmp165=(_tmp163->f1).targs;}}_LLE1:
# 763
{union Cyc_Absyn_DatatypeFieldInfoU _tmp1AD=_tmp164;struct _tuple0*_tmp1AE;struct _tuple0*_tmp1AF;int _tmp1B0;struct _tuple0*_tmp1B1;int _tmp1B2;struct _tuple0*_tmp1B3;_LL112: if((_tmp1AD.UnknownDatatypefield).tag != 1)goto _LL114;_tmp1AE=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AD.UnknownDatatypefield).val).datatype_name;_tmp1AF=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AD.UnknownDatatypefield).val).field_name;_tmp1B0=((struct Cyc_Absyn_UnknownDatatypeFieldInfo)(_tmp1AD.UnknownDatatypefield).val).is_extensible;_LL113:
# 765
 _tmp1B1=_tmp1AE;_tmp1B2=_tmp1B0;_tmp1B3=_tmp1AF;goto _LL115;_LL114: if((_tmp1AD.KnownDatatypefield).tag != 2)goto _LL111;_tmp1B1=(*((struct _tuple1)(_tmp1AD.KnownDatatypefield).val).f1).name;_tmp1B2=(*((struct _tuple1)(_tmp1AD.KnownDatatypefield).val).f1).is_extensible;_tmp1B3=(*((struct _tuple1)(_tmp1AD.KnownDatatypefield).val).f2).name;_LL115: {
# 768
const char*_tmp6F0;const char*_tmp6EF;struct Cyc_PP_Doc*_tmp1B4=Cyc_PP_text(_tmp1B2?(_tmp6F0="@extensible datatype ",_tag_dyneither(_tmp6F0,sizeof(char),22)):((_tmp6EF="datatype ",_tag_dyneither(_tmp6EF,sizeof(char),10))));
{const char*_tmp6F3;struct Cyc_PP_Doc*_tmp6F2[4];s=((_tmp6F2[3]=Cyc_Absynpp_qvar2doc(_tmp1B3),((_tmp6F2[2]=Cyc_PP_text(((_tmp6F3=".",_tag_dyneither(_tmp6F3,sizeof(char),2)))),((_tmp6F2[1]=Cyc_Absynpp_qvar2doc(_tmp1B1),((_tmp6F2[0]=_tmp1B4,Cyc_PP_cat(_tag_dyneither(_tmp6F2,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL111;}_LL111:;}
# 772
goto _LLD1;_LLE2: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp166=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp154;if(_tmp166->tag != 6)goto _LLE4;else{_tmp167=_tmp166->f1;_tmp168=_tmp166->f2;}}_LLE3: {
# 774
struct _dyneither_ptr sns;
struct _dyneither_ptr ts;
switch(_tmp167){case Cyc_Absyn_None: _LL116:
 goto _LL117;case Cyc_Absyn_Signed: _LL117:
{const char*_tmp6F4;sns=((_tmp6F4="",_tag_dyneither(_tmp6F4,sizeof(char),1)));}break;case Cyc_Absyn_Unsigned: _LL118:
{const char*_tmp6F5;sns=((_tmp6F5="unsigned ",_tag_dyneither(_tmp6F5,sizeof(char),10)));}break;}
# 781
switch(_tmp168){case Cyc_Absyn_Char_sz: _LL11A:
# 783
 switch(_tmp167){case Cyc_Absyn_None: _LL11C:
{const char*_tmp6F6;sns=((_tmp6F6="",_tag_dyneither(_tmp6F6,sizeof(char),1)));}break;case Cyc_Absyn_Signed: _LL11D:
{const char*_tmp6F7;sns=((_tmp6F7="signed ",_tag_dyneither(_tmp6F7,sizeof(char),8)));}break;case Cyc_Absyn_Unsigned: _LL11E:
{const char*_tmp6F8;sns=((_tmp6F8="unsigned ",_tag_dyneither(_tmp6F8,sizeof(char),10)));}break;}
# 788
{const char*_tmp6F9;ts=((_tmp6F9="char",_tag_dyneither(_tmp6F9,sizeof(char),5)));}
break;case Cyc_Absyn_Short_sz: _LL11B:
{const char*_tmp6FA;ts=((_tmp6FA="short",_tag_dyneither(_tmp6FA,sizeof(char),6)));}break;case Cyc_Absyn_Int_sz: _LL120:
{const char*_tmp6FB;ts=((_tmp6FB="int",_tag_dyneither(_tmp6FB,sizeof(char),4)));}break;case Cyc_Absyn_Long_sz: _LL121:
{const char*_tmp6FC;ts=((_tmp6FC="long",_tag_dyneither(_tmp6FC,sizeof(char),5)));}break;case Cyc_Absyn_LongLong_sz: _LL122:
# 794
 switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL124:
{const char*_tmp6FD;ts=((_tmp6FD="long long",_tag_dyneither(_tmp6FD,sizeof(char),10)));}break;case Cyc_Cyclone_Vc_c: _LL125:
{const char*_tmp6FE;ts=((_tmp6FE="__int64",_tag_dyneither(_tmp6FE,sizeof(char),8)));}break;}
# 798
break;}
# 800
{const char*_tmp703;void*_tmp702[2];struct Cyc_String_pa_PrintArg_struct _tmp701;struct Cyc_String_pa_PrintArg_struct _tmp700;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp700.tag=0,((_tmp700.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ts),((_tmp701.tag=0,((_tmp701.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sns),((_tmp702[0]=& _tmp701,((_tmp702[1]=& _tmp700,Cyc_aprintf(((_tmp703="%s%s",_tag_dyneither(_tmp703,sizeof(char),5))),_tag_dyneither(_tmp702,sizeof(void*),2)))))))))))))));}
goto _LLD1;}_LLE4: {struct Cyc_Absyn_FloatType_Absyn_Type_struct*_tmp169=(struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp154;if(_tmp169->tag != 7)goto _LLE6;else{_tmp16A=_tmp169->f1;}}_LLE5:
# 803
 if(_tmp16A == 0){
const char*_tmp704;s=Cyc_PP_text(((_tmp704="float",_tag_dyneither(_tmp704,sizeof(char),6))));}else{
if(_tmp16A == 1){
const char*_tmp705;s=Cyc_PP_text(((_tmp705="double",_tag_dyneither(_tmp705,sizeof(char),7))));}else{
# 808
const char*_tmp706;s=Cyc_PP_text(((_tmp706="long double",_tag_dyneither(_tmp706,sizeof(char),12))));}}
goto _LLD1;_LLE6: {struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp16B=(struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp154;if(_tmp16B->tag != 10)goto _LLE8;else{_tmp16C=_tmp16B->f1;}}_LLE7:
# 811
{struct Cyc_PP_Doc*_tmp707[2];s=((_tmp707[1]=Cyc_Absynpp_args2doc(_tmp16C),((_tmp707[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp707,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLE8: {struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp16D=(struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp154;if(_tmp16D->tag != 11)goto _LLEA;else{_tmp16E=(_tmp16D->f1).aggr_info;_tmp16F=(_tmp16D->f1).targs;}}_LLE9: {
# 814
enum Cyc_Absyn_AggrKind _tmp1CD;struct _tuple0*_tmp1CE;struct _tuple10 _tmp1CC=Cyc_Absyn_aggr_kinded_name(_tmp16E);_tmp1CD=_tmp1CC.f1;_tmp1CE=_tmp1CC.f2;
{struct Cyc_PP_Doc*_tmp708[3];s=((_tmp708[2]=Cyc_Absynpp_tps2doc(_tmp16F),((_tmp708[1]=Cyc_Absynpp_qvar2doc(_tmp1CE),((_tmp708[0]=Cyc_Absynpp_aggr_kind2doc(_tmp1CD),Cyc_PP_cat(_tag_dyneither(_tmp708,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LLD1;}_LLEA: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp170=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp154;if(_tmp170->tag != 12)goto _LLEC;else{_tmp171=_tmp170->f1;_tmp172=_tmp170->f2;}}_LLEB:
# 818
{struct Cyc_PP_Doc*_tmp709[4];s=((_tmp709[3]=
# 820
Cyc_Absynpp_rb(),((_tmp709[2]=
# 819
Cyc_PP_nest(2,Cyc_Absynpp_aggrfields2doc(_tmp172)),((_tmp709[1]=
# 818
Cyc_Absynpp_lb(),((_tmp709[0]=Cyc_Absynpp_aggr_kind2doc(_tmp171),Cyc_PP_cat(_tag_dyneither(_tmp709,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 821
goto _LLD1;_LLEC: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp173=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp154;if(_tmp173->tag != 14)goto _LLEE;else{_tmp174=_tmp173->f1;}}_LLED:
# 823
{const char*_tmp70C;struct Cyc_PP_Doc*_tmp70B[4];s=((_tmp70B[3]=Cyc_Absynpp_rb(),((_tmp70B[2]=Cyc_PP_nest(2,Cyc_Absynpp_enumfields2doc(_tmp174)),((_tmp70B[1]=Cyc_Absynpp_lb(),((_tmp70B[0]=Cyc_PP_text(((_tmp70C="enum ",_tag_dyneither(_tmp70C,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70B,sizeof(struct Cyc_PP_Doc*),4))))))))));}goto _LLD1;_LLEE: {struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp175=(struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp154;if(_tmp175->tag != 13)goto _LLF0;else{_tmp176=_tmp175->f1;}}_LLEF:
# 825
{const char*_tmp70F;struct Cyc_PP_Doc*_tmp70E[2];s=((_tmp70E[1]=Cyc_Absynpp_qvar2doc(_tmp176),((_tmp70E[0]=Cyc_PP_text(((_tmp70F="enum ",_tag_dyneither(_tmp70F,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp70E,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LLD1;_LLF0: {struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp177=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp154;if(_tmp177->tag != 18)goto _LLF2;else{_tmp178=_tmp177->f1;}}_LLF1:
# 827
{const char*_tmp714;const char*_tmp713;struct Cyc_PP_Doc*_tmp712[3];s=((_tmp712[2]=Cyc_PP_text(((_tmp713=")",_tag_dyneither(_tmp713,sizeof(char),2)))),((_tmp712[1]=Cyc_Absynpp_exp2doc(_tmp178),((_tmp712[0]=Cyc_PP_text(((_tmp714="valueof_t(",_tag_dyneither(_tmp714,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp712,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF2: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp179=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp154;if(_tmp179->tag != 17)goto _LLF4;else{_tmp17A=_tmp179->f1;_tmp17B=_tmp179->f2;_tmp17C=_tmp179->f3;}}_LLF3:
# 834
{struct Cyc_PP_Doc*_tmp715[2];s=((_tmp715[1]=Cyc_Absynpp_tps2doc(_tmp17B),((_tmp715[0]=Cyc_Absynpp_qvar2doc(_tmp17A),Cyc_PP_cat(_tag_dyneither(_tmp715,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LLD1;_LLF4: {struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp17D=(struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp154;if(_tmp17D->tag != 15)goto _LLF6;else{_tmp17E=(void*)_tmp17D->f1;}}_LLF5:
# 837
{const char*_tmp71A;const char*_tmp719;struct Cyc_PP_Doc*_tmp718[3];s=((_tmp718[2]=Cyc_PP_text(((_tmp719=">",_tag_dyneither(_tmp719,sizeof(char),2)))),((_tmp718[1]=Cyc_Absynpp_rgn2doc(_tmp17E),((_tmp718[0]=Cyc_PP_text(((_tmp71A="region_t<",_tag_dyneither(_tmp71A,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp718,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLF6: {struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp17F=(struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp154;if(_tmp17F->tag != 16)goto _LLF8;else{_tmp180=(void*)_tmp17F->f1;_tmp181=(void*)_tmp17F->f2;}}_LLF7:
# 839
{const char*_tmp721;const char*_tmp720;const char*_tmp71F;struct Cyc_PP_Doc*_tmp71E[5];s=((_tmp71E[4]=
Cyc_PP_text(((_tmp71F=">",_tag_dyneither(_tmp71F,sizeof(char),2)))),((_tmp71E[3]=
# 839
Cyc_Absynpp_rgn2doc(_tmp181),((_tmp71E[2]=Cyc_PP_text(((_tmp720=",",_tag_dyneither(_tmp720,sizeof(char),2)))),((_tmp71E[1]=Cyc_Absynpp_rgn2doc(_tmp180),((_tmp71E[0]=Cyc_PP_text(((_tmp721="dynregion_t<",_tag_dyneither(_tmp721,sizeof(char),13)))),Cyc_PP_cat(_tag_dyneither(_tmp71E,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LLD1;_LLF8: {struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp182=(struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp154;if(_tmp182->tag != 19)goto _LLFA;else{_tmp183=(void*)_tmp182->f1;}}_LLF9:
# 842
{const char*_tmp726;const char*_tmp725;struct Cyc_PP_Doc*_tmp724[3];s=((_tmp724[2]=Cyc_PP_text(((_tmp725=">",_tag_dyneither(_tmp725,sizeof(char),2)))),((_tmp724[1]=Cyc_Absynpp_typ2doc(_tmp183),((_tmp724[0]=Cyc_PP_text(((_tmp726="tag_t<",_tag_dyneither(_tmp726,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp724,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LLFA: {struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*_tmp184=(struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct*)_tmp154;if(_tmp184->tag != 21)goto _LLFC;}_LLFB:
 goto _LLFD;_LLFC: {struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*_tmp185=(struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp154;if(_tmp185->tag != 20)goto _LLFE;}_LLFD:
 goto _LLFF;_LLFE: {struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*_tmp186=(struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct*)_tmp154;if(_tmp186->tag != 22)goto _LL100;}_LLFF:
# 846
 s=Cyc_Absynpp_rgn2doc(t);goto _LLD1;_LL100: {struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp187=(struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp154;if(_tmp187->tag != 25)goto _LL102;else{_tmp188=(void*)_tmp187->f1;}}_LL101:
# 848
{const char*_tmp72B;const char*_tmp72A;struct Cyc_PP_Doc*_tmp729[3];s=((_tmp729[2]=Cyc_PP_text(((_tmp72A=")",_tag_dyneither(_tmp72A,sizeof(char),2)))),((_tmp729[1]=Cyc_Absynpp_typ2doc(_tmp188),((_tmp729[0]=Cyc_PP_text(((_tmp72B="regions(",_tag_dyneither(_tmp72B,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp729,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LLD1;_LL102: {struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp189=(struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp154;if(_tmp189->tag != 23)goto _LL104;}_LL103:
 goto _LL105;_LL104: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp18A=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp154;if(_tmp18A->tag != 24)goto _LL106;}_LL105:
# 851
 s=Cyc_Absynpp_eff2doc(t);goto _LLD1;_LL106: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18B=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp154;if(_tmp18B->tag != 26)goto _LL108;else{struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*_tmp18C=(struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(*_tmp18B->f1).r;if(_tmp18C->tag != 0)goto _LL108;else{_tmp18D=_tmp18C->f1;}}}_LL107:
# 853
 s=Cyc_Absynpp_aggrdecl2doc(_tmp18D);goto _LLD1;_LL108: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp18E=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp154;if(_tmp18E->tag != 26)goto _LL10A;else{struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp18F=(struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(*_tmp18E->f1).r;if(_tmp18F->tag != 1)goto _LL10A;else{_tmp190=_tmp18F->f1;}}}_LL109:
# 855
 s=Cyc_Absynpp_enumdecl2doc(_tmp190);goto _LLD1;_LL10A: {struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp191=(struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp154;if(_tmp191->tag != 26)goto _LLD1;else{struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*_tmp192=(struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(*_tmp191->f1).r;if(_tmp192->tag != 2)goto _LLD1;else{_tmp193=_tmp192->f1;}}}_LL10B:
# 857
 s=Cyc_Absynpp_datatypedecl2doc(_tmp193);goto _LLD1;_LLD1:;}
# 859
return s;}
# 862
struct Cyc_PP_Doc*Cyc_Absynpp_vo2doc(struct _dyneither_ptr*vo){
return vo == 0?Cyc_PP_nil_doc(): Cyc_PP_text(*((struct _dyneither_ptr*)vo));}struct _tuple16{void*f1;void*f2;};
# 866
struct Cyc_PP_Doc*Cyc_Absynpp_rgn_cmp2doc(struct _tuple16*cmp){
void*_tmp1E7;void*_tmp1E8;struct _tuple16*_tmp1E6=cmp;_tmp1E7=(*_tmp1E6).f1;_tmp1E8=(*_tmp1E6).f2;{
const char*_tmp72E;struct Cyc_PP_Doc*_tmp72D[3];return(_tmp72D[2]=Cyc_Absynpp_rgn2doc(_tmp1E8),((_tmp72D[1]=Cyc_PP_text(((_tmp72E=" > ",_tag_dyneither(_tmp72E,sizeof(char),4)))),((_tmp72D[0]=Cyc_Absynpp_rgn2doc(_tmp1E7),Cyc_PP_cat(_tag_dyneither(_tmp72D,sizeof(struct Cyc_PP_Doc*),3)))))));};}
# 871
struct Cyc_PP_Doc*Cyc_Absynpp_rgnpo2doc(struct Cyc_List_List*po){
const char*_tmp731;const char*_tmp730;const char*_tmp72F;return Cyc_PP_group(((_tmp72F="",_tag_dyneither(_tmp72F,sizeof(char),1))),((_tmp730="",_tag_dyneither(_tmp730,sizeof(char),1))),((_tmp731=",",_tag_dyneither(_tmp731,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_rgn_cmp2doc,po));}
# 875
struct Cyc_PP_Doc*Cyc_Absynpp_funarg2doc(struct _tuple8*t){
struct _dyneither_ptr*_tmp1EE=(*t).f1;
struct Cyc_Core_Opt*_tmp732;struct Cyc_Core_Opt*dopt=_tmp1EE == 0?0:((_tmp732=_cycalloc(sizeof(*_tmp732)),((_tmp732->v=Cyc_PP_text(*_tmp1EE),_tmp732))));
return Cyc_Absynpp_tqtd2doc((*t).f2,(*t).f3,dopt);}
# 881
struct Cyc_PP_Doc*Cyc_Absynpp_funargs2doc(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po){
# 884
struct Cyc_List_List*_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_funarg2doc,args);
struct Cyc_PP_Doc*eff_doc;
if(c_varargs){
const char*_tmp735;struct Cyc_List_List*_tmp734;_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F0,((_tmp734=_cycalloc(sizeof(*_tmp734)),((_tmp734->hd=Cyc_PP_text(((_tmp735="...",_tag_dyneither(_tmp735,sizeof(char),4)))),((_tmp734->tl=0,_tmp734)))))));}else{
if(cyc_varargs != 0){
const char*_tmp73E;const char*_tmp73D;const char*_tmp73C;struct _tuple8*_tmp73B;struct Cyc_PP_Doc*_tmp73A[3];struct Cyc_PP_Doc*_tmp1F3=(_tmp73A[2]=
# 891
Cyc_Absynpp_funarg2doc(((_tmp73B=_cycalloc(sizeof(*_tmp73B)),((_tmp73B->f1=cyc_varargs->name,((_tmp73B->f2=cyc_varargs->tq,((_tmp73B->f3=cyc_varargs->type,_tmp73B))))))))),((_tmp73A[1]=
# 890
cyc_varargs->inject?Cyc_PP_text(((_tmp73C=" inject ",_tag_dyneither(_tmp73C,sizeof(char),9)))): Cyc_PP_text(((_tmp73D=" ",_tag_dyneither(_tmp73D,sizeof(char),2)))),((_tmp73A[0]=
# 889
Cyc_PP_text(((_tmp73E="...",_tag_dyneither(_tmp73E,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp73A,sizeof(struct Cyc_PP_Doc*),3)))))));
# 893
struct Cyc_List_List*_tmp73F;_tmp1F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F0,((_tmp73F=_cycalloc(sizeof(*_tmp73F)),((_tmp73F->hd=_tmp1F3,((_tmp73F->tl=0,_tmp73F)))))));}}{
# 895
const char*_tmp742;const char*_tmp741;const char*_tmp740;struct Cyc_PP_Doc*_tmp1FA=Cyc_PP_group(((_tmp740="",_tag_dyneither(_tmp740,sizeof(char),1))),((_tmp741="",_tag_dyneither(_tmp741,sizeof(char),1))),((_tmp742=",",_tag_dyneither(_tmp742,sizeof(char),2))),_tmp1F0);
if(effopt != 0  && Cyc_Absynpp_print_all_effects){
const char*_tmp745;struct Cyc_PP_Doc*_tmp744[3];_tmp1FA=((_tmp744[2]=Cyc_Absynpp_eff2doc((void*)effopt),((_tmp744[1]=Cyc_PP_text(((_tmp745=";",_tag_dyneither(_tmp745,sizeof(char),2)))),((_tmp744[0]=_tmp1FA,Cyc_PP_cat(_tag_dyneither(_tmp744,sizeof(struct Cyc_PP_Doc*),3))))))));}
if(rgn_po != 0){
const char*_tmp748;struct Cyc_PP_Doc*_tmp747[3];_tmp1FA=((_tmp747[2]=Cyc_Absynpp_rgnpo2doc(rgn_po),((_tmp747[1]=Cyc_PP_text(((_tmp748=":",_tag_dyneither(_tmp748,sizeof(char),2)))),((_tmp747[0]=_tmp1FA,Cyc_PP_cat(_tag_dyneither(_tmp747,sizeof(struct Cyc_PP_Doc*),3))))))));}{
const char*_tmp74D;const char*_tmp74C;struct Cyc_PP_Doc*_tmp74B[3];return(_tmp74B[2]=Cyc_PP_text(((_tmp74C=")",_tag_dyneither(_tmp74C,sizeof(char),2)))),((_tmp74B[1]=_tmp1FA,((_tmp74B[0]=Cyc_PP_text(((_tmp74D="(",_tag_dyneither(_tmp74D,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp74B,sizeof(struct Cyc_PP_Doc*),3)))))));};};}
# 903
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg){
struct _dyneither_ptr*_tmp206;struct Cyc_Absyn_Tqual _tmp207;void*_tmp208;struct _tuple8*_tmp205=arg;_tmp206=(*_tmp205).f1;_tmp207=(*_tmp205).f2;_tmp208=(*_tmp205).f3;{
struct _tuple8*_tmp74E;return(_tmp74E=_cycalloc(sizeof(*_tmp74E)),((_tmp74E->f1=(struct _dyneither_ptr*)_tmp206,((_tmp74E->f2=_tmp207,((_tmp74E->f3=_tmp208,_tmp74E)))))));};}
# 908
struct Cyc_PP_Doc*Cyc_Absynpp_var2doc(struct _dyneither_ptr*v){return Cyc_PP_text(*v);}
# 910
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*q){
struct Cyc_List_List*_tmp20A=0;
int match;
{union Cyc_Absyn_Nmspace _tmp20B=(*q).f1;struct Cyc_List_List*_tmp20C;struct Cyc_List_List*_tmp20D;struct Cyc_List_List*_tmp20E;_LL128: if((_tmp20B.Loc_n).tag != 4)goto _LL12A;_LL129:
# 915
 _tmp20C=0;goto _LL12B;_LL12A: if((_tmp20B.Rel_n).tag != 1)goto _LL12C;_tmp20C=(struct Cyc_List_List*)(_tmp20B.Rel_n).val;_LL12B:
# 917
 match=0;
_tmp20A=_tmp20C;
goto _LL127;_LL12C: if((_tmp20B.C_n).tag != 3)goto _LL12E;_tmp20D=(struct Cyc_List_List*)(_tmp20B.C_n).val;_LL12D:
# 921
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20D,Cyc_Absynpp_curr_namespace);
# 923
goto _LL127;_LL12E: if((_tmp20B.Abs_n).tag != 2)goto _LL127;_tmp20E=(struct Cyc_List_List*)(_tmp20B.Abs_n).val;_LL12F:
# 925
 match=Cyc_Absynpp_use_curr_namespace  && ((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_prefix)(Cyc_strptrcmp,_tmp20E,Cyc_Absynpp_curr_namespace);
{struct Cyc_List_List*_tmp74F;_tmp20A=Cyc_Absynpp_qvar_to_Cids  && Cyc_Absynpp_add_cyc_prefix?(_tmp74F=_cycalloc(sizeof(*_tmp74F)),((_tmp74F->hd=Cyc_Absynpp_cyc_stringptr,((_tmp74F->tl=_tmp20E,_tmp74F))))): _tmp20E;}
goto _LL127;_LL127:;}
# 929
if(Cyc_Absynpp_qvar_to_Cids){
const char*_tmp751;struct Cyc_List_List*_tmp750;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20A,(
(_tmp750=_cycalloc(sizeof(*_tmp750)),((_tmp750->hd=(*q).f2,((_tmp750->tl=0,_tmp750))))))),((_tmp751="_",_tag_dyneither(_tmp751,sizeof(char),2))));}else{
# 934
if(match)
return*(*q).f2;else{
# 937
const char*_tmp753;struct Cyc_List_List*_tmp752;return(struct _dyneither_ptr)Cyc_str_sepstr(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp20A,(
(_tmp752=_cycalloc(sizeof(*_tmp752)),((_tmp752->hd=(*q).f2,((_tmp752->tl=0,_tmp752))))))),((_tmp753="::",_tag_dyneither(_tmp753,sizeof(char),3))));}}}
# 942
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2doc(struct _tuple0*q){
return Cyc_PP_text(Cyc_Absynpp_qvar2string(q));}
# 946
struct Cyc_PP_Doc*Cyc_Absynpp_qvar2bolddoc(struct _tuple0*q){
struct _dyneither_ptr _tmp214=Cyc_Absynpp_qvar2string(q);
if(Cyc_PP_tex_output){
# 950
const char*_tmp755;const char*_tmp754;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp754="\\textbf{",_tag_dyneither(_tmp754,sizeof(char),9))),(struct _dyneither_ptr)_tmp214),((_tmp755="}",_tag_dyneither(_tmp755,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp214));}else{
return Cyc_PP_text(_tmp214);}}
# 955
struct _dyneither_ptr Cyc_Absynpp_typedef_name2string(struct _tuple0*v){
# 957
if(Cyc_Absynpp_qvar_to_Cids)return Cyc_Absynpp_qvar2string(v);
# 960
if(Cyc_Absynpp_use_curr_namespace){
union Cyc_Absyn_Nmspace _tmp217=(*v).f1;struct Cyc_List_List*_tmp218;struct Cyc_List_List*_tmp219;_LL131: if((_tmp217.Loc_n).tag != 4)goto _LL133;_LL132:
 goto _LL134;_LL133: if((_tmp217.Rel_n).tag != 1)goto _LL135;if((struct Cyc_List_List*)(_tmp217.Rel_n).val != 0)goto _LL135;_LL134:
 return*(*v).f2;_LL135: if((_tmp217.C_n).tag != 3)goto _LL137;_tmp218=(struct Cyc_List_List*)(_tmp217.C_n).val;_LL136:
 _tmp219=_tmp218;goto _LL138;_LL137: if((_tmp217.Abs_n).tag != 2)goto _LL139;_tmp219=(struct Cyc_List_List*)(_tmp217.Abs_n).val;_LL138:
# 966
 if(((int(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_strptrcmp,_tmp219,Cyc_Absynpp_curr_namespace)== 0)
return*(*v).f2;else{
# 969
goto _LL13A;}_LL139:;_LL13A: {
# 971
const char*_tmp756;return(struct _dyneither_ptr)Cyc_strconcat(((_tmp756="/* bad namespace : */ ",_tag_dyneither(_tmp756,sizeof(char),23))),(struct _dyneither_ptr)Cyc_Absynpp_qvar2string(v));}_LL130:;}else{
# 974
return*(*v).f2;}}
# 977
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2doc(struct _tuple0*v){
return Cyc_PP_text(Cyc_Absynpp_typedef_name2string(v));}
# 980
struct Cyc_PP_Doc*Cyc_Absynpp_typedef_name2bolddoc(struct _tuple0*v){
struct _dyneither_ptr _tmp21B=Cyc_Absynpp_typedef_name2string(v);
if(Cyc_PP_tex_output){
# 984
const char*_tmp758;const char*_tmp757;return Cyc_PP_text_width((struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_strconcat(((_tmp757="\\textbf{",_tag_dyneither(_tmp757,sizeof(char),9))),(struct _dyneither_ptr)_tmp21B),((_tmp758="}",_tag_dyneither(_tmp758,sizeof(char),2)))),(int)
Cyc_strlen((struct _dyneither_ptr)_tmp21B));}else{
return Cyc_PP_text(_tmp21B);}}
# 989
struct Cyc_PP_Doc*Cyc_Absynpp_typ2doc(void*t){
return Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,0);}
# 1000 "absynpp.cyc"
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*e){
void*_tmp21E=e->r;enum Cyc_Absyn_Primop _tmp222;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22E;_LL13C: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp21F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp21F->tag != 0)goto _LL13E;}_LL13D:
 goto _LL13F;_LL13E: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp220=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp220->tag != 1)goto _LL140;}_LL13F:
 return 10000;_LL140: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp221=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp221->tag != 2)goto _LL142;else{_tmp222=_tmp221->f1;}}_LL141:
# 1005
 switch(_tmp222){case Cyc_Absyn_Plus: _LL18A:
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
 return 140;}_LL142: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp223=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp223->tag != 3)goto _LL144;}_LL143:
# 1027
 return 20;_LL144: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp224=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp224->tag != 4)goto _LL146;}_LL145:
 return 130;_LL146: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp225=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp225->tag != 5)goto _LL148;}_LL147:
 return 30;_LL148: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp226=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp226->tag != 6)goto _LL14A;}_LL149:
 return 35;_LL14A: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp227=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp227->tag != 7)goto _LL14C;}_LL14B:
 return 30;_LL14C: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp228=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp228->tag != 8)goto _LL14E;}_LL14D:
 return 10;_LL14E: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp229=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp229->tag != 9)goto _LL150;}_LL14F:
 return 140;_LL150: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp22A=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp22A->tag != 10)goto _LL152;}_LL151:
 return 130;_LL152: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp22B=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp22B->tag != 11)goto _LL154;else{_tmp22C=_tmp22B->f1;}}_LL153:
 return Cyc_Absynpp_exp_prec(_tmp22C);_LL154: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp22D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp22D->tag != 12)goto _LL156;else{_tmp22E=_tmp22D->f1;}}_LL155:
 return Cyc_Absynpp_exp_prec(_tmp22E);_LL156: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp22F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp22F->tag != 13)goto _LL158;}_LL157:
 return 120;_LL158: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp230=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp230->tag != 15)goto _LL15A;}_LL159:
 return 15;_LL15A: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp231=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp231->tag != 14)goto _LL15C;}_LL15B:
 goto _LL15D;_LL15C: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp232=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp232->tag != 16)goto _LL15E;}_LL15D:
 goto _LL15F;_LL15E: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp233=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp233->tag != 17)goto _LL160;}_LL15F:
 goto _LL161;_LL160: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp234=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp234->tag != 37)goto _LL162;}_LL161:
 goto _LL163;_LL162: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp235=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp235->tag != 38)goto _LL164;}_LL163:
 goto _LL165;_LL164: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp236=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp236->tag != 36)goto _LL166;}_LL165:
 goto _LL167;_LL166: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp237=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp237->tag != 18)goto _LL168;}_LL167:
 goto _LL169;_LL168: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp238=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp238->tag != 19)goto _LL16A;}_LL169:
 return 130;_LL16A: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp239=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp239->tag != 20)goto _LL16C;}_LL16B:
 goto _LL16D;_LL16C: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp23A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23A->tag != 21)goto _LL16E;}_LL16D:
 goto _LL16F;_LL16E: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp23B=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23B->tag != 22)goto _LL170;}_LL16F:
 return 140;_LL170: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp23C=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23C->tag != 23)goto _LL172;}_LL171:
 return 150;_LL172: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp23D=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23D->tag != 24)goto _LL174;}_LL173:
 goto _LL175;_LL174: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp23E=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23E->tag != 25)goto _LL176;}_LL175:
 goto _LL177;_LL176: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp23F=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp23F->tag != 26)goto _LL178;}_LL177:
 goto _LL179;_LL178: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp240=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp240->tag != 27)goto _LL17A;}_LL179:
 goto _LL17B;_LL17A: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp241=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp241->tag != 28)goto _LL17C;}_LL17B:
 goto _LL17D;_LL17C: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp242=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp242->tag != 29)goto _LL17E;}_LL17D:
 goto _LL17F;_LL17E: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp243=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp243->tag != 30)goto _LL180;}_LL17F:
 goto _LL181;_LL180: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp244=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp244->tag != 31)goto _LL182;}_LL181:
 goto _LL183;_LL182: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp245=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp245->tag != 32)goto _LL184;}_LL183:
 goto _LL185;_LL184: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp246=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp246->tag != 33)goto _LL186;}_LL185:
 goto _LL187;_LL186: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp247=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp247->tag != 34)goto _LL188;}_LL187:
 return 140;_LL188: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp248=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp21E;if(_tmp248->tag != 35)goto _LL13B;}_LL189:
 return 10000;_LL13B:;}
# 1066
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc(struct Cyc_Absyn_Exp*e){
return Cyc_Absynpp_exp2doc_prec(0,e);}
# 1070
struct Cyc_PP_Doc*Cyc_Absynpp_exp2doc_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
struct Cyc_PP_Doc*s;
{void*_tmp249=e->r;union Cyc_Absyn_Cnst _tmp24B;struct _tuple0*_tmp24D;enum Cyc_Absyn_Primop _tmp24F;struct Cyc_List_List*_tmp250;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Core_Opt*_tmp253;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Exp*_tmp256;enum Cyc_Absyn_Incrementor _tmp257;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Exp*_tmp260;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Exp*_tmp263;struct Cyc_Absyn_Exp*_tmp264;struct Cyc_Absyn_Exp*_tmp266;struct Cyc_List_List*_tmp267;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26D;void*_tmp26F;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp274;struct Cyc_Absyn_Exp*_tmp275;void*_tmp277;struct Cyc_Absyn_Exp*_tmp279;void*_tmp27B;int _tmp27D;struct _dyneither_ptr _tmp27E;struct Cyc_Absyn_Exp*_tmp280;struct _dyneither_ptr*_tmp281;void*_tmp283;struct _dyneither_ptr*_tmp285;void*_tmp287;unsigned int _tmp289;struct Cyc_Absyn_Exp*_tmp28B;struct Cyc_Absyn_Exp*_tmp28D;struct _dyneither_ptr*_tmp28E;struct Cyc_Absyn_Exp*_tmp290;struct _dyneither_ptr*_tmp291;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_List_List*_tmp296;struct _tuple8*_tmp298;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp29B;struct Cyc_Absyn_Vardecl*_tmp29D;struct Cyc_Absyn_Exp*_tmp29E;struct Cyc_Absyn_Exp*_tmp29F;struct _tuple0*_tmp2A1;struct Cyc_List_List*_tmp2A2;struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*_tmp2A5;struct Cyc_List_List*_tmp2A7;struct Cyc_Absyn_Datatypefield*_tmp2A8;struct _tuple0*_tmp2AA;struct _tuple0*_tmp2AC;int _tmp2AE;struct Cyc_Absyn_Exp*_tmp2AF;void**_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B4;struct Cyc_Core_Opt*_tmp2B6;struct Cyc_List_List*_tmp2B7;struct Cyc_Absyn_Stmt*_tmp2B9;_LL1A0: {struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp24A=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24A->tag != 0)goto _LL1A2;else{_tmp24B=_tmp24A->f1;}}_LL1A1:
 s=Cyc_Absynpp_cnst2doc(_tmp24B);goto _LL19F;_LL1A2: {struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_tmp24C=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24C->tag != 1)goto _LL1A4;else{_tmp24D=_tmp24C->f1;}}_LL1A3:
 s=Cyc_Absynpp_qvar2doc(_tmp24D);goto _LL19F;_LL1A4: {struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp24E=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp24E->tag != 2)goto _LL1A6;else{_tmp24F=_tmp24E->f1;_tmp250=_tmp24E->f2;}}_LL1A5:
 s=Cyc_Absynpp_primapp2doc(myprec,_tmp24F,_tmp250);goto _LL19F;_LL1A6: {struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp251=(struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp251->tag != 3)goto _LL1A8;else{_tmp252=_tmp251->f1;_tmp253=_tmp251->f2;_tmp254=_tmp251->f3;}}_LL1A7:
# 1078
{const char*_tmp75D;const char*_tmp75C;struct Cyc_PP_Doc*_tmp75B[5];s=((_tmp75B[4]=
# 1082
Cyc_Absynpp_exp2doc_prec(myprec,_tmp254),((_tmp75B[3]=
# 1081
Cyc_PP_text(((_tmp75C="= ",_tag_dyneither(_tmp75C,sizeof(char),3)))),((_tmp75B[2]=
# 1080
_tmp253 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_prim2doc((enum Cyc_Absyn_Primop)_tmp253->v),((_tmp75B[1]=
# 1079
Cyc_PP_text(((_tmp75D=" ",_tag_dyneither(_tmp75D,sizeof(char),2)))),((_tmp75B[0]=
# 1078
Cyc_Absynpp_exp2doc_prec(myprec,_tmp252),Cyc_PP_cat(_tag_dyneither(_tmp75B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1083
goto _LL19F;_LL1A8: {struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp255=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp255->tag != 4)goto _LL1AA;else{_tmp256=_tmp255->f1;_tmp257=_tmp255->f2;}}_LL1A9: {
# 1085
struct Cyc_PP_Doc*_tmp2BD=Cyc_Absynpp_exp2doc_prec(myprec,_tmp256);
switch(_tmp257){case Cyc_Absyn_PreInc: _LL1F0:
{const char*_tmp760;struct Cyc_PP_Doc*_tmp75F[2];s=((_tmp75F[1]=_tmp2BD,((_tmp75F[0]=Cyc_PP_text(((_tmp760="++",_tag_dyneither(_tmp760,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp75F,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PreDec: _LL1F1:
{const char*_tmp763;struct Cyc_PP_Doc*_tmp762[2];s=((_tmp762[1]=_tmp2BD,((_tmp762[0]=Cyc_PP_text(((_tmp763="--",_tag_dyneither(_tmp763,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp762,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostInc: _LL1F2:
{const char*_tmp766;struct Cyc_PP_Doc*_tmp765[2];s=((_tmp765[1]=Cyc_PP_text(((_tmp766="++",_tag_dyneither(_tmp766,sizeof(char),3)))),((_tmp765[0]=_tmp2BD,Cyc_PP_cat(_tag_dyneither(_tmp765,sizeof(struct Cyc_PP_Doc*),2))))));}break;case Cyc_Absyn_PostDec: _LL1F3:
{const char*_tmp769;struct Cyc_PP_Doc*_tmp768[2];s=((_tmp768[1]=Cyc_PP_text(((_tmp769="--",_tag_dyneither(_tmp769,sizeof(char),3)))),((_tmp768[0]=_tmp2BD,Cyc_PP_cat(_tag_dyneither(_tmp768,sizeof(struct Cyc_PP_Doc*),2))))));}break;}
# 1092
goto _LL19F;}_LL1AA: {struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp258=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp258->tag != 5)goto _LL1AC;else{_tmp259=_tmp258->f1;_tmp25A=_tmp258->f2;_tmp25B=_tmp258->f3;}}_LL1AB:
# 1094
{const char*_tmp76E;const char*_tmp76D;struct Cyc_PP_Doc*_tmp76C[5];s=((_tmp76C[4]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp25B),((_tmp76C[3]=Cyc_PP_text(((_tmp76D=" : ",_tag_dyneither(_tmp76D,sizeof(char),4)))),((_tmp76C[2]=
# 1094
Cyc_Absynpp_exp2doc_prec(0,_tmp25A),((_tmp76C[1]=Cyc_PP_text(((_tmp76E=" ? ",_tag_dyneither(_tmp76E,sizeof(char),4)))),((_tmp76C[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp259),Cyc_PP_cat(_tag_dyneither(_tmp76C,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1096
goto _LL19F;_LL1AC: {struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp25C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp25C->tag != 6)goto _LL1AE;else{_tmp25D=_tmp25C->f1;_tmp25E=_tmp25C->f2;}}_LL1AD:
# 1098
{const char*_tmp771;struct Cyc_PP_Doc*_tmp770[3];s=((_tmp770[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25E),((_tmp770[1]=Cyc_PP_text(((_tmp771=" && ",_tag_dyneither(_tmp771,sizeof(char),5)))),((_tmp770[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp25D),Cyc_PP_cat(_tag_dyneither(_tmp770,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1AE: {struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp25F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp25F->tag != 7)goto _LL1B0;else{_tmp260=_tmp25F->f1;_tmp261=_tmp25F->f2;}}_LL1AF:
# 1101
{const char*_tmp774;struct Cyc_PP_Doc*_tmp773[3];s=((_tmp773[2]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp261),((_tmp773[1]=Cyc_PP_text(((_tmp774=" || ",_tag_dyneither(_tmp774,sizeof(char),5)))),((_tmp773[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp260),Cyc_PP_cat(_tag_dyneither(_tmp773,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1B0: {struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp262=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp262->tag != 8)goto _LL1B2;else{_tmp263=_tmp262->f1;_tmp264=_tmp262->f2;}}_LL1B1:
# 1106
{const char*_tmp77B;const char*_tmp77A;const char*_tmp779;struct Cyc_PP_Doc*_tmp778[5];s=((_tmp778[4]=Cyc_PP_text(((_tmp779=")",_tag_dyneither(_tmp779,sizeof(char),2)))),((_tmp778[3]=Cyc_Absynpp_exp2doc(_tmp264),((_tmp778[2]=Cyc_PP_text(((_tmp77A=", ",_tag_dyneither(_tmp77A,sizeof(char),3)))),((_tmp778[1]=Cyc_Absynpp_exp2doc(_tmp263),((_tmp778[0]=Cyc_PP_text(((_tmp77B="(",_tag_dyneither(_tmp77B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp778,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1B2: {struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp265=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp265->tag != 9)goto _LL1B4;else{_tmp266=_tmp265->f1;_tmp267=_tmp265->f2;}}_LL1B3:
# 1109
{const char*_tmp780;const char*_tmp77F;struct Cyc_PP_Doc*_tmp77E[4];s=((_tmp77E[3]=
# 1112
Cyc_PP_text(((_tmp77F=")",_tag_dyneither(_tmp77F,sizeof(char),2)))),((_tmp77E[2]=
# 1111
Cyc_Absynpp_exps2doc_prec(20,_tmp267),((_tmp77E[1]=
# 1110
Cyc_PP_text(((_tmp780="(",_tag_dyneither(_tmp780,sizeof(char),2)))),((_tmp77E[0]=
# 1109
Cyc_Absynpp_exp2doc_prec(myprec,_tmp266),Cyc_PP_cat(_tag_dyneither(_tmp77E,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1113
goto _LL19F;_LL1B4: {struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp268=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp268->tag != 10)goto _LL1B6;else{_tmp269=_tmp268->f1;}}_LL1B5:
# 1115
{const char*_tmp783;struct Cyc_PP_Doc*_tmp782[2];s=((_tmp782[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp269),((_tmp782[0]=Cyc_PP_text(((_tmp783="throw ",_tag_dyneither(_tmp783,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp782,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1B6: {struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_tmp26A=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26A->tag != 11)goto _LL1B8;else{_tmp26B=_tmp26A->f1;}}_LL1B7:
# 1118
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26B);
goto _LL19F;_LL1B8: {struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp26C=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26C->tag != 12)goto _LL1BA;else{_tmp26D=_tmp26C->f1;}}_LL1B9:
# 1121
 s=Cyc_Absynpp_exp2doc_prec(inprec,_tmp26D);
goto _LL19F;_LL1BA: {struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp26E=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp26E->tag != 13)goto _LL1BC;else{_tmp26F=(void*)_tmp26E->f1;_tmp270=_tmp26E->f2;}}_LL1BB:
# 1124
{const char*_tmp788;const char*_tmp787;struct Cyc_PP_Doc*_tmp786[4];s=((_tmp786[3]=
# 1127
Cyc_Absynpp_exp2doc_prec(myprec,_tmp270),((_tmp786[2]=
# 1126
Cyc_PP_text(((_tmp787=")",_tag_dyneither(_tmp787,sizeof(char),2)))),((_tmp786[1]=
# 1125
Cyc_Absynpp_typ2doc(_tmp26F),((_tmp786[0]=
# 1124
Cyc_PP_text(((_tmp788="(",_tag_dyneither(_tmp788,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp786,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1128
goto _LL19F;_LL1BC: {struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_tmp271=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp271->tag != 14)goto _LL1BE;else{_tmp272=_tmp271->f1;}}_LL1BD:
# 1130
{const char*_tmp78B;struct Cyc_PP_Doc*_tmp78A[2];s=((_tmp78A[1]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp272),((_tmp78A[0]=
# 1130
Cyc_PP_text(((_tmp78B="&",_tag_dyneither(_tmp78B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp78A,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1132
goto _LL19F;_LL1BE: {struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*_tmp273=(struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp273->tag != 15)goto _LL1C0;else{_tmp274=_tmp273->f1;_tmp275=_tmp273->f2;}}_LL1BF:
# 1134
 if(_tmp274 == 0){
const char*_tmp78E;struct Cyc_PP_Doc*_tmp78D[2];s=((_tmp78D[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp78D[0]=Cyc_PP_text(((_tmp78E="new ",_tag_dyneither(_tmp78E,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp78D,sizeof(struct Cyc_PP_Doc*),2))))));}else{
# 1137
const char*_tmp793;const char*_tmp792;struct Cyc_PP_Doc*_tmp791[4];s=((_tmp791[3]=
Cyc_Absynpp_exp2doc_prec(myprec,_tmp275),((_tmp791[2]=
# 1137
Cyc_PP_text(((_tmp792=") ",_tag_dyneither(_tmp792,sizeof(char),3)))),((_tmp791[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp274),((_tmp791[0]=Cyc_PP_text(((_tmp793="rnew(",_tag_dyneither(_tmp793,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp791,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1139
goto _LL19F;_LL1C0: {struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp276=(struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp276->tag != 16)goto _LL1C2;else{_tmp277=(void*)_tmp276->f1;}}_LL1C1:
{const char*_tmp798;const char*_tmp797;struct Cyc_PP_Doc*_tmp796[3];s=((_tmp796[2]=Cyc_PP_text(((_tmp797=")",_tag_dyneither(_tmp797,sizeof(char),2)))),((_tmp796[1]=Cyc_Absynpp_typ2doc(_tmp277),((_tmp796[0]=Cyc_PP_text(((_tmp798="sizeof(",_tag_dyneither(_tmp798,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp796,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C2: {struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp278=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp278->tag != 17)goto _LL1C4;else{_tmp279=_tmp278->f1;}}_LL1C3:
{const char*_tmp79D;const char*_tmp79C;struct Cyc_PP_Doc*_tmp79B[3];s=((_tmp79B[2]=Cyc_PP_text(((_tmp79C=")",_tag_dyneither(_tmp79C,sizeof(char),2)))),((_tmp79B[1]=Cyc_Absynpp_exp2doc(_tmp279),((_tmp79B[0]=Cyc_PP_text(((_tmp79D="sizeof(",_tag_dyneither(_tmp79D,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp79B,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C4: {struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp27A=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27A->tag != 37)goto _LL1C6;else{_tmp27B=(void*)_tmp27A->f1;}}_LL1C5:
{const char*_tmp7A2;const char*_tmp7A1;struct Cyc_PP_Doc*_tmp7A0[3];s=((_tmp7A0[2]=Cyc_PP_text(((_tmp7A1=")",_tag_dyneither(_tmp7A1,sizeof(char),2)))),((_tmp7A0[1]=Cyc_Absynpp_typ2doc(_tmp27B),((_tmp7A0[0]=Cyc_PP_text(((_tmp7A2="valueof(",_tag_dyneither(_tmp7A2,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7A0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL19F;_LL1C6: {struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp27C=(struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27C->tag != 38)goto _LL1C8;else{_tmp27D=_tmp27C->f1;_tmp27E=_tmp27C->f2;}}_LL1C7:
# 1144
 if(_tmp27D){
const char*_tmp7A9;const char*_tmp7A8;const char*_tmp7A7;struct Cyc_PP_Doc*_tmp7A6[4];s=((_tmp7A6[3]=Cyc_PP_text(((_tmp7A7=")",_tag_dyneither(_tmp7A7,sizeof(char),2)))),((_tmp7A6[2]=Cyc_PP_text(_tmp27E),((_tmp7A6[1]=Cyc_PP_text(((_tmp7A8=" volatile (",_tag_dyneither(_tmp7A8,sizeof(char),12)))),((_tmp7A6[0]=Cyc_PP_text(((_tmp7A9="__asm__",_tag_dyneither(_tmp7A9,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7A6,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1147
const char*_tmp7AE;const char*_tmp7AD;struct Cyc_PP_Doc*_tmp7AC[3];s=((_tmp7AC[2]=Cyc_PP_text(((_tmp7AD=")",_tag_dyneither(_tmp7AD,sizeof(char),2)))),((_tmp7AC[1]=Cyc_PP_text(_tmp27E),((_tmp7AC[0]=Cyc_PP_text(((_tmp7AE="__asm__(",_tag_dyneither(_tmp7AE,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp7AC,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1C8: {struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp27F=(struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp27F->tag != 36)goto _LL1CA;else{_tmp280=_tmp27F->f1;_tmp281=_tmp27F->f2;}}_LL1C9:
# 1150
{const char*_tmp7B5;const char*_tmp7B4;const char*_tmp7B3;struct Cyc_PP_Doc*_tmp7B2[5];s=((_tmp7B2[4]=Cyc_PP_text(((_tmp7B3=")",_tag_dyneither(_tmp7B3,sizeof(char),2)))),((_tmp7B2[3]=Cyc_PP_textptr(_tmp281),((_tmp7B2[2]=Cyc_PP_text(((_tmp7B4=".",_tag_dyneither(_tmp7B4,sizeof(char),2)))),((_tmp7B2[1]=Cyc_Absynpp_exp2doc(_tmp280),((_tmp7B2[0]=Cyc_PP_text(((_tmp7B5="tagcheck(",_tag_dyneither(_tmp7B5,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B2,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CA: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp282=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp282->tag != 18)goto _LL1CC;else{_tmp283=(void*)_tmp282->f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp284=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)((void*)_tmp282->f2);if(_tmp284->tag != 0)goto _LL1CC;else{_tmp285=_tmp284->f1;}};}}_LL1CB:
# 1153
{const char*_tmp7BC;const char*_tmp7BB;const char*_tmp7BA;struct Cyc_PP_Doc*_tmp7B9[5];s=((_tmp7B9[4]=Cyc_PP_text(((_tmp7BA=")",_tag_dyneither(_tmp7BA,sizeof(char),2)))),((_tmp7B9[3]=Cyc_PP_textptr(_tmp285),((_tmp7B9[2]=Cyc_PP_text(((_tmp7BB=",",_tag_dyneither(_tmp7BB,sizeof(char),2)))),((_tmp7B9[1]=Cyc_Absynpp_typ2doc(_tmp283),((_tmp7B9[0]=Cyc_PP_text(((_tmp7BC="offsetof(",_tag_dyneither(_tmp7BC,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7B9,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CC: {struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp286=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp286->tag != 18)goto _LL1CE;else{_tmp287=(void*)_tmp286->f1;{struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp288=(struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)((void*)_tmp286->f2);if(_tmp288->tag != 1)goto _LL1CE;else{_tmp289=_tmp288->f1;}};}}_LL1CD:
# 1156
{const char*_tmp7CA;const char*_tmp7C9;struct Cyc_Int_pa_PrintArg_struct _tmp7C8;void*_tmp7C7[1];const char*_tmp7C6;const char*_tmp7C5;struct Cyc_PP_Doc*_tmp7C4[5];s=((_tmp7C4[4]=Cyc_PP_text(((_tmp7C5=")",_tag_dyneither(_tmp7C5,sizeof(char),2)))),((_tmp7C4[3]=Cyc_PP_text((struct _dyneither_ptr)((_tmp7C8.tag=1,((_tmp7C8.f1=(unsigned long)((int)_tmp289),((_tmp7C7[0]=& _tmp7C8,Cyc_aprintf(((_tmp7C6="%d",_tag_dyneither(_tmp7C6,sizeof(char),3))),_tag_dyneither(_tmp7C7,sizeof(void*),1))))))))),((_tmp7C4[2]=Cyc_PP_text(((_tmp7C9=",",_tag_dyneither(_tmp7C9,sizeof(char),2)))),((_tmp7C4[1]=Cyc_Absynpp_typ2doc(_tmp287),((_tmp7C4[0]=Cyc_PP_text(((_tmp7CA="offsetof(",_tag_dyneither(_tmp7CA,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp7C4,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
goto _LL19F;_LL1CE: {struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp28A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp28A->tag != 19)goto _LL1D0;else{_tmp28B=_tmp28A->f1;}}_LL1CF:
# 1159
{const char*_tmp7CD;struct Cyc_PP_Doc*_tmp7CC[2];s=((_tmp7CC[1]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28B),((_tmp7CC[0]=Cyc_PP_text(((_tmp7CD="*",_tag_dyneither(_tmp7CD,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7CC,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL19F;_LL1D0: {struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp28C=(struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp28C->tag != 20)goto _LL1D2;else{_tmp28D=_tmp28C->f1;_tmp28E=_tmp28C->f2;}}_LL1D1:
# 1162
{const char*_tmp7D0;struct Cyc_PP_Doc*_tmp7CF[3];s=((_tmp7CF[2]=Cyc_PP_textptr(_tmp28E),((_tmp7CF[1]=Cyc_PP_text(((_tmp7D0=".",_tag_dyneither(_tmp7D0,sizeof(char),2)))),((_tmp7CF[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp28D),Cyc_PP_cat(_tag_dyneither(_tmp7CF,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D2: {struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp28F=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp28F->tag != 21)goto _LL1D4;else{_tmp290=_tmp28F->f1;_tmp291=_tmp28F->f2;}}_LL1D3:
# 1165
{const char*_tmp7D3;struct Cyc_PP_Doc*_tmp7D2[3];s=((_tmp7D2[2]=Cyc_PP_textptr(_tmp291),((_tmp7D2[1]=Cyc_PP_text(((_tmp7D3="->",_tag_dyneither(_tmp7D3,sizeof(char),3)))),((_tmp7D2[0]=Cyc_Absynpp_exp2doc_prec(myprec,_tmp290),Cyc_PP_cat(_tag_dyneither(_tmp7D2,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL19F;_LL1D4: {struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp292=(struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp292->tag != 22)goto _LL1D6;else{_tmp293=_tmp292->f1;_tmp294=_tmp292->f2;}}_LL1D5:
# 1168
{const char*_tmp7D8;const char*_tmp7D7;struct Cyc_PP_Doc*_tmp7D6[4];s=((_tmp7D6[3]=
# 1171
Cyc_PP_text(((_tmp7D7="]",_tag_dyneither(_tmp7D7,sizeof(char),2)))),((_tmp7D6[2]=
# 1170
Cyc_Absynpp_exp2doc(_tmp294),((_tmp7D6[1]=
# 1169
Cyc_PP_text(((_tmp7D8="[",_tag_dyneither(_tmp7D8,sizeof(char),2)))),((_tmp7D6[0]=
# 1168
Cyc_Absynpp_exp2doc_prec(myprec,_tmp293),Cyc_PP_cat(_tag_dyneither(_tmp7D6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1172
goto _LL19F;_LL1D6: {struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*_tmp295=(struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp295->tag != 23)goto _LL1D8;else{_tmp296=_tmp295->f1;}}_LL1D7:
# 1174
{const char*_tmp7DD;const char*_tmp7DC;struct Cyc_PP_Doc*_tmp7DB[4];s=((_tmp7DB[3]=
# 1177
Cyc_PP_text(((_tmp7DC=")",_tag_dyneither(_tmp7DC,sizeof(char),2)))),((_tmp7DB[2]=
# 1176
Cyc_Absynpp_exps2doc_prec(20,_tmp296),((_tmp7DB[1]=
# 1175
Cyc_PP_text(((_tmp7DD="(",_tag_dyneither(_tmp7DD,sizeof(char),2)))),((_tmp7DB[0]=
# 1174
Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp7DB,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1178
goto _LL19F;_LL1D8: {struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp297=(struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp297->tag != 24)goto _LL1DA;else{_tmp298=_tmp297->f1;_tmp299=_tmp297->f2;}}_LL1D9:
# 1180
{const char*_tmp7E4;const char*_tmp7E3;const char*_tmp7E2;struct Cyc_PP_Doc*_tmp7E1[4];s=((_tmp7E1[3]=
# 1183
Cyc_Absynpp_group_braces(((_tmp7E2=",",_tag_dyneither(_tmp7E2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp299)),((_tmp7E1[2]=
# 1182
Cyc_PP_text(((_tmp7E3=")",_tag_dyneither(_tmp7E3,sizeof(char),2)))),((_tmp7E1[1]=
# 1181
Cyc_Absynpp_typ2doc((*_tmp298).f3),((_tmp7E1[0]=
# 1180
Cyc_PP_text(((_tmp7E4="(",_tag_dyneither(_tmp7E4,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp7E1,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1184
goto _LL19F;_LL1DA: {struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp29A=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp29A->tag != 25)goto _LL1DC;else{_tmp29B=_tmp29A->f1;}}_LL1DB:
# 1186
{const char*_tmp7E5;s=Cyc_Absynpp_group_braces(((_tmp7E5=",",_tag_dyneither(_tmp7E5,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp29B));}
goto _LL19F;_LL1DC: {struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp29C=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp29C->tag != 26)goto _LL1DE;else{_tmp29D=_tmp29C->f1;_tmp29E=_tmp29C->f2;_tmp29F=_tmp29C->f3;}}_LL1DD:
# 1189
{const char*_tmp7EC;const char*_tmp7EB;const char*_tmp7EA;struct Cyc_PP_Doc*_tmp7E9[8];s=((_tmp7E9[7]=
# 1195
Cyc_Absynpp_rb(),((_tmp7E9[6]=
# 1194
Cyc_Absynpp_exp2doc(_tmp29F),((_tmp7E9[5]=
# 1193
Cyc_PP_text(((_tmp7EA=" : ",_tag_dyneither(_tmp7EA,sizeof(char),4)))),((_tmp7E9[4]=
# 1192
Cyc_Absynpp_exp2doc(_tmp29E),((_tmp7E9[3]=
# 1191
Cyc_PP_text(((_tmp7EB=" < ",_tag_dyneither(_tmp7EB,sizeof(char),4)))),((_tmp7E9[2]=
# 1190
Cyc_PP_text(*(*_tmp29D->name).f2),((_tmp7E9[1]=
# 1189
Cyc_PP_text(((_tmp7EC="for ",_tag_dyneither(_tmp7EC,sizeof(char),5)))),((_tmp7E9[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp7E9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1196
goto _LL19F;_LL1DE: {struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp2A0=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A0->tag != 27)goto _LL1E0;else{_tmp2A1=_tmp2A0->f1;_tmp2A2=_tmp2A0->f2;_tmp2A3=_tmp2A0->f3;}}_LL1DF: {
# 1198
struct Cyc_List_List*_tmp314=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A3);
{const char*_tmp7F1;struct Cyc_List_List*_tmp7F0;struct Cyc_PP_Doc*_tmp7EF[2];s=((_tmp7EF[1]=
Cyc_Absynpp_group_braces(((_tmp7F1=",",_tag_dyneither(_tmp7F1,sizeof(char),2))),
_tmp2A2 != 0?(_tmp7F0=_cycalloc(sizeof(*_tmp7F0)),((_tmp7F0->hd=Cyc_Absynpp_tps2doc(_tmp2A2),((_tmp7F0->tl=_tmp314,_tmp7F0))))): _tmp314),((_tmp7EF[0]=
# 1199
Cyc_Absynpp_qvar2doc(_tmp2A1),Cyc_PP_cat(_tag_dyneither(_tmp7EF,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1202
goto _LL19F;}_LL1E0: {struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp2A4=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A4->tag != 28)goto _LL1E2;else{_tmp2A5=_tmp2A4->f2;}}_LL1E1:
# 1204
{const char*_tmp7F2;s=Cyc_Absynpp_group_braces(((_tmp7F2=",",_tag_dyneither(_tmp7F2,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2A5));}
goto _LL19F;_LL1E2: {struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp2A6=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A6->tag != 29)goto _LL1E4;else{_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A6->f3;}}_LL1E3:
# 1207
 if(_tmp2A7 == 0)
# 1209
s=Cyc_Absynpp_qvar2doc(_tmp2A8->name);else{
# 1211
const char*_tmp7F9;const char*_tmp7F8;const char*_tmp7F7;struct Cyc_PP_Doc*_tmp7F6[2];s=((_tmp7F6[1]=
Cyc_PP_egroup(((_tmp7F9="(",_tag_dyneither(_tmp7F9,sizeof(char),2))),((_tmp7F8=")",_tag_dyneither(_tmp7F8,sizeof(char),2))),((_tmp7F7=",",_tag_dyneither(_tmp7F7,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_exp2doc,_tmp2A7)),((_tmp7F6[0]=
# 1211
Cyc_Absynpp_qvar2doc(_tmp2A8->name),Cyc_PP_cat(_tag_dyneither(_tmp7F6,sizeof(struct Cyc_PP_Doc*),2))))));}
# 1213
goto _LL19F;_LL1E4: {struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp2A9=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2A9->tag != 30)goto _LL1E6;else{_tmp2AA=_tmp2A9->f1;}}_LL1E5:
# 1215
 s=Cyc_Absynpp_qvar2doc(_tmp2AA);
goto _LL19F;_LL1E6: {struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp2AB=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2AB->tag != 31)goto _LL1E8;else{_tmp2AC=_tmp2AB->f1;}}_LL1E7:
# 1218
 s=Cyc_Absynpp_qvar2doc(_tmp2AC);
goto _LL19F;_LL1E8: {struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp2AD=(struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2AD->tag != 32)goto _LL1EA;else{_tmp2AE=(_tmp2AD->f1).is_calloc;_tmp2AF=(_tmp2AD->f1).rgn;_tmp2B0=(_tmp2AD->f1).elt_type;_tmp2B1=(_tmp2AD->f1).num_elts;}}_LL1E9:
# 1223
 if(_tmp2AE){
# 1225
struct Cyc_Absyn_Exp*st=Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp2B0)),0);
if(_tmp2AF == 0){
const char*_tmp800;const char*_tmp7FF;const char*_tmp7FE;struct Cyc_PP_Doc*_tmp7FD[5];s=((_tmp7FD[4]=Cyc_PP_text(((_tmp7FE=")",_tag_dyneither(_tmp7FE,sizeof(char),2)))),((_tmp7FD[3]=Cyc_Absynpp_exp2doc(st),((_tmp7FD[2]=Cyc_PP_text(((_tmp7FF=",",_tag_dyneither(_tmp7FF,sizeof(char),2)))),((_tmp7FD[1]=Cyc_Absynpp_exp2doc(_tmp2B1),((_tmp7FD[0]=Cyc_PP_text(((_tmp800="calloc(",_tag_dyneither(_tmp800,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp7FD,sizeof(struct Cyc_PP_Doc*),5))))))))))));}else{
# 1229
const char*_tmp809;const char*_tmp808;const char*_tmp807;const char*_tmp806;struct Cyc_PP_Doc*_tmp805[7];s=((_tmp805[6]=
Cyc_PP_text(((_tmp806=")",_tag_dyneither(_tmp806,sizeof(char),2)))),((_tmp805[5]=Cyc_Absynpp_exp2doc(st),((_tmp805[4]=Cyc_PP_text(((_tmp807=",",_tag_dyneither(_tmp807,sizeof(char),2)))),((_tmp805[3]=Cyc_Absynpp_exp2doc(_tmp2B1),((_tmp805[2]=
# 1229
Cyc_PP_text(((_tmp808=",",_tag_dyneither(_tmp808,sizeof(char),2)))),((_tmp805[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2AF),((_tmp805[0]=Cyc_PP_text(((_tmp809="rcalloc(",_tag_dyneither(_tmp809,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp805,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1232
struct Cyc_Absyn_Exp*new_e;
# 1234
if(_tmp2B0 == 0)
new_e=_tmp2B1;else{
# 1237
new_e=Cyc_Absyn_times_exp(Cyc_Absyn_sizeoftyp_exp(*_tmp2B0,0),_tmp2B1,0);}
# 1239
if(_tmp2AF == 0){
const char*_tmp80E;const char*_tmp80D;struct Cyc_PP_Doc*_tmp80C[3];s=((_tmp80C[2]=Cyc_PP_text(((_tmp80D=")",_tag_dyneither(_tmp80D,sizeof(char),2)))),((_tmp80C[1]=Cyc_Absynpp_exp2doc(new_e),((_tmp80C[0]=Cyc_PP_text(((_tmp80E="malloc(",_tag_dyneither(_tmp80E,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp80C,sizeof(struct Cyc_PP_Doc*),3))))))));}else{
# 1242
const char*_tmp815;const char*_tmp814;const char*_tmp813;struct Cyc_PP_Doc*_tmp812[5];s=((_tmp812[4]=
Cyc_PP_text(((_tmp813=")",_tag_dyneither(_tmp813,sizeof(char),2)))),((_tmp812[3]=Cyc_Absynpp_exp2doc(new_e),((_tmp812[2]=
# 1242
Cyc_PP_text(((_tmp814=",",_tag_dyneither(_tmp814,sizeof(char),2)))),((_tmp812[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp2AF),((_tmp812[0]=Cyc_PP_text(((_tmp815="rmalloc(",_tag_dyneither(_tmp815,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp812,sizeof(struct Cyc_PP_Doc*),5))))))))))));}}
# 1245
goto _LL19F;_LL1EA: {struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_tmp2B2=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B2->tag != 33)goto _LL1EC;else{_tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;}}_LL1EB:
# 1247
{const char*_tmp818;struct Cyc_PP_Doc*_tmp817[3];s=((_tmp817[2]=
# 1249
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B4),((_tmp817[1]=
# 1248
Cyc_PP_text(((_tmp818=" :=: ",_tag_dyneither(_tmp818,sizeof(char),6)))),((_tmp817[0]=
# 1247
Cyc_Absynpp_exp2doc_prec(myprec,_tmp2B3),Cyc_PP_cat(_tag_dyneither(_tmp817,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1250
goto _LL19F;_LL1EC: {struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp2B5=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B5->tag != 34)goto _LL1EE;else{_tmp2B6=_tmp2B5->f1;_tmp2B7=_tmp2B5->f2;}}_LL1ED:
# 1253
{const char*_tmp819;s=Cyc_Absynpp_group_braces(((_tmp819=",",_tag_dyneither(_tmp819,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_de2doc,_tmp2B7));}
goto _LL19F;_LL1EE: {struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp2B8=(struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp249;if(_tmp2B8->tag != 35)goto _LL19F;else{_tmp2B9=_tmp2B8->f1;}}_LL1EF:
# 1256
{const char*_tmp81E;const char*_tmp81D;struct Cyc_PP_Doc*_tmp81C[7];s=((_tmp81C[6]=
# 1258
Cyc_PP_text(((_tmp81D=")",_tag_dyneither(_tmp81D,sizeof(char),2)))),((_tmp81C[5]=Cyc_Absynpp_rb(),((_tmp81C[4]=Cyc_PP_blank_doc(),((_tmp81C[3]=
# 1257
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp2B9,1)),((_tmp81C[2]=
# 1256
Cyc_PP_blank_doc(),((_tmp81C[1]=Cyc_Absynpp_lb(),((_tmp81C[0]=Cyc_PP_text(((_tmp81E="(",_tag_dyneither(_tmp81E,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp81C,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1259
goto _LL19F;_LL19F:;}
# 1261
if(inprec >= myprec){
const char*_tmp823;const char*_tmp822;struct Cyc_PP_Doc*_tmp821[3];s=((_tmp821[2]=Cyc_PP_text(((_tmp822=")",_tag_dyneither(_tmp822,sizeof(char),2)))),((_tmp821[1]=s,((_tmp821[0]=Cyc_PP_text(((_tmp823="(",_tag_dyneither(_tmp823,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp821,sizeof(struct Cyc_PP_Doc*),3))))))));}
return s;}
# 1266
struct Cyc_PP_Doc*Cyc_Absynpp_designator2doc(void*d){
void*_tmp336=d;struct Cyc_Absyn_Exp*_tmp338;struct _dyneither_ptr*_tmp33A;_LL1F6: {struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp337=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp336;if(_tmp337->tag != 0)goto _LL1F8;else{_tmp338=_tmp337->f1;}}_LL1F7: {
const char*_tmp828;const char*_tmp827;struct Cyc_PP_Doc*_tmp826[3];return(_tmp826[2]=Cyc_PP_text(((_tmp827="]",_tag_dyneither(_tmp827,sizeof(char),2)))),((_tmp826[1]=Cyc_Absynpp_exp2doc(_tmp338),((_tmp826[0]=Cyc_PP_text(((_tmp828=".[",_tag_dyneither(_tmp828,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp826,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1F8: {struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp339=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp336;if(_tmp339->tag != 1)goto _LL1F5;else{_tmp33A=_tmp339->f1;}}_LL1F9: {
const char*_tmp82B;struct Cyc_PP_Doc*_tmp82A[2];return(_tmp82A[1]=Cyc_PP_textptr(_tmp33A),((_tmp82A[0]=Cyc_PP_text(((_tmp82B=".",_tag_dyneither(_tmp82B,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp82A,sizeof(struct Cyc_PP_Doc*),2)))));}_LL1F5:;}
# 1273
struct Cyc_PP_Doc*Cyc_Absynpp_de2doc(struct _tuple14*de){
if((*de).f1 == 0)return Cyc_Absynpp_exp2doc((*de).f2);else{
const char*_tmp832;const char*_tmp831;const char*_tmp830;struct Cyc_PP_Doc*_tmp82F[2];return(_tmp82F[1]=
Cyc_Absynpp_exp2doc((*de).f2),((_tmp82F[0]=
# 1275
Cyc_PP_egroup(((_tmp832="",_tag_dyneither(_tmp832,sizeof(char),1))),((_tmp831="=",_tag_dyneither(_tmp831,sizeof(char),2))),((_tmp830="=",_tag_dyneither(_tmp830,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*de).f1)),Cyc_PP_cat(_tag_dyneither(_tmp82F,sizeof(struct Cyc_PP_Doc*),2)))));}}
# 1279
struct Cyc_PP_Doc*Cyc_Absynpp_exps2doc_prec(int inprec,struct Cyc_List_List*es){
const char*_tmp835;const char*_tmp834;const char*_tmp833;return Cyc_PP_group(((_tmp833="",_tag_dyneither(_tmp833,sizeof(char),1))),((_tmp834="",_tag_dyneither(_tmp834,sizeof(char),1))),((_tmp835=",",_tag_dyneither(_tmp835,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es));}
# 1284
struct Cyc_PP_Doc*Cyc_Absynpp_cnst2doc(union Cyc_Absyn_Cnst c){
union Cyc_Absyn_Cnst _tmp347=c;enum Cyc_Absyn_Sign _tmp348;char _tmp349;struct _dyneither_ptr _tmp34A;enum Cyc_Absyn_Sign _tmp34B;short _tmp34C;int _tmp34D;int _tmp34E;int _tmp34F;enum Cyc_Absyn_Sign _tmp350;long long _tmp351;struct _dyneither_ptr _tmp352;struct _dyneither_ptr _tmp353;struct _dyneither_ptr _tmp354;_LL1FB: if((_tmp347.Char_c).tag != 2)goto _LL1FD;_tmp348=((struct _tuple3)(_tmp347.Char_c).val).f1;_tmp349=((struct _tuple3)(_tmp347.Char_c).val).f2;_LL1FC: {
const char*_tmp839;void*_tmp838[1];struct Cyc_String_pa_PrintArg_struct _tmp837;return Cyc_PP_text((struct _dyneither_ptr)((_tmp837.tag=0,((_tmp837.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp349)),((_tmp838[0]=& _tmp837,Cyc_aprintf(((_tmp839="'%s'",_tag_dyneither(_tmp839,sizeof(char),5))),_tag_dyneither(_tmp838,sizeof(void*),1)))))))));}_LL1FD: if((_tmp347.Wchar_c).tag != 3)goto _LL1FF;_tmp34A=(struct _dyneither_ptr)(_tmp347.Wchar_c).val;_LL1FE: {
const char*_tmp83D;void*_tmp83C[1];struct Cyc_String_pa_PrintArg_struct _tmp83B;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83B.tag=0,((_tmp83B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34A),((_tmp83C[0]=& _tmp83B,Cyc_aprintf(((_tmp83D="L'%s'",_tag_dyneither(_tmp83D,sizeof(char),6))),_tag_dyneither(_tmp83C,sizeof(void*),1)))))))));}_LL1FF: if((_tmp347.Short_c).tag != 4)goto _LL201;_tmp34B=((struct _tuple4)(_tmp347.Short_c).val).f1;_tmp34C=((struct _tuple4)(_tmp347.Short_c).val).f2;_LL200: {
const char*_tmp841;void*_tmp840[1];struct Cyc_Int_pa_PrintArg_struct _tmp83F;return Cyc_PP_text((struct _dyneither_ptr)((_tmp83F.tag=1,((_tmp83F.f1=(unsigned long)((int)_tmp34C),((_tmp840[0]=& _tmp83F,Cyc_aprintf(((_tmp841="%d",_tag_dyneither(_tmp841,sizeof(char),3))),_tag_dyneither(_tmp840,sizeof(void*),1)))))))));}_LL201: if((_tmp347.Int_c).tag != 5)goto _LL203;if(((struct _tuple5)(_tmp347.Int_c).val).f1 != Cyc_Absyn_None)goto _LL203;_tmp34D=((struct _tuple5)(_tmp347.Int_c).val).f2;_LL202:
 _tmp34E=_tmp34D;goto _LL204;_LL203: if((_tmp347.Int_c).tag != 5)goto _LL205;if(((struct _tuple5)(_tmp347.Int_c).val).f1 != Cyc_Absyn_Signed)goto _LL205;_tmp34E=((struct _tuple5)(_tmp347.Int_c).val).f2;_LL204: {
const char*_tmp845;void*_tmp844[1];struct Cyc_Int_pa_PrintArg_struct _tmp843;return Cyc_PP_text((struct _dyneither_ptr)((_tmp843.tag=1,((_tmp843.f1=(unsigned long)_tmp34E,((_tmp844[0]=& _tmp843,Cyc_aprintf(((_tmp845="%d",_tag_dyneither(_tmp845,sizeof(char),3))),_tag_dyneither(_tmp844,sizeof(void*),1)))))))));}_LL205: if((_tmp347.Int_c).tag != 5)goto _LL207;if(((struct _tuple5)(_tmp347.Int_c).val).f1 != Cyc_Absyn_Unsigned)goto _LL207;_tmp34F=((struct _tuple5)(_tmp347.Int_c).val).f2;_LL206: {
const char*_tmp849;void*_tmp848[1];struct Cyc_Int_pa_PrintArg_struct _tmp847;return Cyc_PP_text((struct _dyneither_ptr)((_tmp847.tag=1,((_tmp847.f1=(unsigned int)_tmp34F,((_tmp848[0]=& _tmp847,Cyc_aprintf(((_tmp849="%u",_tag_dyneither(_tmp849,sizeof(char),3))),_tag_dyneither(_tmp848,sizeof(void*),1)))))))));}_LL207: if((_tmp347.LongLong_c).tag != 6)goto _LL209;_tmp350=((struct _tuple6)(_tmp347.LongLong_c).val).f1;_tmp351=((struct _tuple6)(_tmp347.LongLong_c).val).f2;_LL208: {
# 1293
const char*_tmp84A;return Cyc_PP_text(((_tmp84A="<<FIX LONG LONG CONSTANT>>",_tag_dyneither(_tmp84A,sizeof(char),27))));}_LL209: if((_tmp347.Float_c).tag != 7)goto _LL20B;_tmp352=((struct _tuple7)(_tmp347.Float_c).val).f1;_LL20A:
 return Cyc_PP_text(_tmp352);_LL20B: if((_tmp347.Null_c).tag != 1)goto _LL20D;_LL20C: {
const char*_tmp84B;return Cyc_PP_text(((_tmp84B="NULL",_tag_dyneither(_tmp84B,sizeof(char),5))));}_LL20D: if((_tmp347.String_c).tag != 8)goto _LL20F;_tmp353=(struct _dyneither_ptr)(_tmp347.String_c).val;_LL20E: {
# 1297
const char*_tmp850;const char*_tmp84F;struct Cyc_PP_Doc*_tmp84E[3];return(_tmp84E[2]=Cyc_PP_text(((_tmp84F="\"",_tag_dyneither(_tmp84F,sizeof(char),2)))),((_tmp84E[1]=Cyc_PP_text(Cyc_Absynpp_string_escape(_tmp353)),((_tmp84E[0]=Cyc_PP_text(((_tmp850="\"",_tag_dyneither(_tmp850,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp84E,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL20F: if((_tmp347.Wstring_c).tag != 9)goto _LL1FA;_tmp354=(struct _dyneither_ptr)(_tmp347.Wstring_c).val;_LL210: {
# 1299
const char*_tmp855;const char*_tmp854;struct Cyc_PP_Doc*_tmp853[3];return(_tmp853[2]=Cyc_PP_text(((_tmp854="\"",_tag_dyneither(_tmp854,sizeof(char),2)))),((_tmp853[1]=Cyc_PP_text(_tmp354),((_tmp853[0]=Cyc_PP_text(((_tmp855="L\"",_tag_dyneither(_tmp855,sizeof(char),3)))),Cyc_PP_cat(_tag_dyneither(_tmp853,sizeof(struct Cyc_PP_Doc*),3)))))));}_LL1FA:;}
# 1303
struct Cyc_PP_Doc*Cyc_Absynpp_primapp2doc(int inprec,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
struct Cyc_PP_Doc*ps=Cyc_Absynpp_prim2doc(p);
if(p == (enum Cyc_Absyn_Primop)Cyc_Absyn_Numelts){
if(es == 0  || es->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp862;const char*_tmp861;void*_tmp860[1];struct Cyc_String_pa_PrintArg_struct _tmp85F;struct Cyc_Core_Failure_exn_struct*_tmp85E;(int)_throw((void*)((_tmp85E=_cycalloc(sizeof(*_tmp85E)),((_tmp85E[0]=((_tmp862.tag=Cyc_Core_Failure,((_tmp862.f1=(struct _dyneither_ptr)((_tmp85F.tag=0,((_tmp85F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp860[0]=& _tmp85F,Cyc_aprintf(((_tmp861="Absynpp::primapp2doc Numelts: %s with bad args",_tag_dyneither(_tmp861,sizeof(char),47))),_tag_dyneither(_tmp860,sizeof(void*),1)))))))),_tmp862)))),_tmp85E)))));}{
# 1309
const char*_tmp867;const char*_tmp866;struct Cyc_PP_Doc*_tmp865[3];return(_tmp865[2]=Cyc_PP_text(((_tmp866=")",_tag_dyneither(_tmp866,sizeof(char),2)))),((_tmp865[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)es->hd),((_tmp865[0]=Cyc_PP_text(((_tmp867="numelts(",_tag_dyneither(_tmp867,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp865,sizeof(struct Cyc_PP_Doc*),3)))))));};}else{
# 1311
struct Cyc_List_List*ds=((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Absynpp_exp2doc_prec,inprec,es);
if(ds == 0){
struct Cyc_Core_Failure_exn_struct _tmp874;const char*_tmp873;void*_tmp872[1];struct Cyc_String_pa_PrintArg_struct _tmp871;struct Cyc_Core_Failure_exn_struct*_tmp870;(int)_throw((void*)((_tmp870=_cycalloc(sizeof(*_tmp870)),((_tmp870[0]=((_tmp874.tag=Cyc_Core_Failure,((_tmp874.f1=(struct _dyneither_ptr)((_tmp871.tag=0,((_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp872[0]=& _tmp871,Cyc_aprintf(((_tmp873="Absynpp::primapp2doc: %s with no args",_tag_dyneither(_tmp873,sizeof(char),38))),_tag_dyneither(_tmp872,sizeof(void*),1)))))))),_tmp874)))),_tmp870)))));}else{
# 1315
if(ds->tl == 0){
const char*_tmp877;struct Cyc_PP_Doc*_tmp876[3];return(_tmp876[2]=(struct Cyc_PP_Doc*)ds->hd,((_tmp876[1]=Cyc_PP_text(((_tmp877=" ",_tag_dyneither(_tmp877,sizeof(char),2)))),((_tmp876[0]=ps,Cyc_PP_cat(_tag_dyneither(_tmp876,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
if(((struct Cyc_List_List*)_check_null(ds->tl))->tl != 0){
struct Cyc_Core_Failure_exn_struct _tmp884;const char*_tmp883;void*_tmp882[1];struct Cyc_String_pa_PrintArg_struct _tmp881;struct Cyc_Core_Failure_exn_struct*_tmp880;(int)_throw((void*)((_tmp880=_cycalloc(sizeof(*_tmp880)),((_tmp880[0]=((_tmp884.tag=Cyc_Core_Failure,((_tmp884.f1=(struct _dyneither_ptr)((_tmp881.tag=0,((_tmp881.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_PP_string_of_doc(ps,72)),((_tmp882[0]=& _tmp881,Cyc_aprintf(((_tmp883="Absynpp::primapp2doc: %s with more than 2 args",_tag_dyneither(_tmp883,sizeof(char),47))),_tag_dyneither(_tmp882,sizeof(void*),1)))))))),_tmp884)))),_tmp880)))));}else{
# 1321
const char*_tmp889;const char*_tmp888;struct Cyc_PP_Doc*_tmp887[5];return(_tmp887[4]=(struct Cyc_PP_Doc*)((struct Cyc_List_List*)_check_null(ds->tl))->hd,((_tmp887[3]=Cyc_PP_text(((_tmp888=" ",_tag_dyneither(_tmp888,sizeof(char),2)))),((_tmp887[2]=ps,((_tmp887[1]=Cyc_PP_text(((_tmp889=" ",_tag_dyneither(_tmp889,sizeof(char),2)))),((_tmp887[0]=(struct Cyc_PP_Doc*)ds->hd,Cyc_PP_cat(_tag_dyneither(_tmp887,sizeof(struct Cyc_PP_Doc*),5)))))))))));}}}}}
# 1325
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p){
switch(p){case Cyc_Absyn_Plus: _LL211: {
const char*_tmp88A;return(_tmp88A="+",_tag_dyneither(_tmp88A,sizeof(char),2));}case Cyc_Absyn_Times: _LL212: {
const char*_tmp88B;return(_tmp88B="*",_tag_dyneither(_tmp88B,sizeof(char),2));}case Cyc_Absyn_Minus: _LL213: {
const char*_tmp88C;return(_tmp88C="-",_tag_dyneither(_tmp88C,sizeof(char),2));}case Cyc_Absyn_Div: _LL214: {
const char*_tmp88D;return(_tmp88D="/",_tag_dyneither(_tmp88D,sizeof(char),2));}case Cyc_Absyn_Mod: _LL215: {
const char*_tmp88F;const char*_tmp88E;return Cyc_Absynpp_print_for_cycdoc?(_tmp88F="\\%",_tag_dyneither(_tmp88F,sizeof(char),3)):((_tmp88E="%",_tag_dyneither(_tmp88E,sizeof(char),2)));}case Cyc_Absyn_Eq: _LL216: {
const char*_tmp890;return(_tmp890="==",_tag_dyneither(_tmp890,sizeof(char),3));}case Cyc_Absyn_Neq: _LL217: {
const char*_tmp891;return(_tmp891="!=",_tag_dyneither(_tmp891,sizeof(char),3));}case Cyc_Absyn_Gt: _LL218: {
const char*_tmp892;return(_tmp892=">",_tag_dyneither(_tmp892,sizeof(char),2));}case Cyc_Absyn_Lt: _LL219: {
const char*_tmp893;return(_tmp893="<",_tag_dyneither(_tmp893,sizeof(char),2));}case Cyc_Absyn_Gte: _LL21A: {
const char*_tmp894;return(_tmp894=">=",_tag_dyneither(_tmp894,sizeof(char),3));}case Cyc_Absyn_Lte: _LL21B: {
const char*_tmp895;return(_tmp895="<=",_tag_dyneither(_tmp895,sizeof(char),3));}case Cyc_Absyn_Not: _LL21C: {
const char*_tmp896;return(_tmp896="!",_tag_dyneither(_tmp896,sizeof(char),2));}case Cyc_Absyn_Bitnot: _LL21D: {
const char*_tmp897;return(_tmp897="~",_tag_dyneither(_tmp897,sizeof(char),2));}case Cyc_Absyn_Bitand: _LL21E: {
const char*_tmp898;return(_tmp898="&",_tag_dyneither(_tmp898,sizeof(char),2));}case Cyc_Absyn_Bitor: _LL21F: {
const char*_tmp899;return(_tmp899="|",_tag_dyneither(_tmp899,sizeof(char),2));}case Cyc_Absyn_Bitxor: _LL220: {
const char*_tmp89A;return(_tmp89A="^",_tag_dyneither(_tmp89A,sizeof(char),2));}case Cyc_Absyn_Bitlshift: _LL221: {
const char*_tmp89B;return(_tmp89B="<<",_tag_dyneither(_tmp89B,sizeof(char),3));}case Cyc_Absyn_Bitlrshift: _LL222: {
const char*_tmp89C;return(_tmp89C=">>",_tag_dyneither(_tmp89C,sizeof(char),3));}case Cyc_Absyn_Bitarshift: _LL223: {
const char*_tmp89D;return(_tmp89D=">>>",_tag_dyneither(_tmp89D,sizeof(char),4));}case Cyc_Absyn_Numelts: _LL224: {
const char*_tmp89E;return(_tmp89E="numelts",_tag_dyneither(_tmp89E,sizeof(char),8));}}}
# 1350
struct Cyc_PP_Doc*Cyc_Absynpp_prim2doc(enum Cyc_Absyn_Primop p){
return Cyc_PP_text(Cyc_Absynpp_prim2str(p));}
# 1354
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s){
void*_tmp398=s->r;_LL227: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp399=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp398;if(_tmp399->tag != 12)goto _LL229;}_LL228:
 return 1;_LL229:;_LL22A:
 return 0;_LL226:;}
# 1361
struct Cyc_PP_Doc*Cyc_Absynpp_stmt2doc(struct Cyc_Absyn_Stmt*st,int stmtexp){
struct Cyc_PP_Doc*s;
{void*_tmp39A=st->r;struct Cyc_Absyn_Exp*_tmp39D;struct Cyc_Absyn_Stmt*_tmp39F;struct Cyc_Absyn_Stmt*_tmp3A0;struct Cyc_Absyn_Exp*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Stmt*_tmp3A5;struct Cyc_Absyn_Stmt*_tmp3A6;struct Cyc_Absyn_Exp*_tmp3A8;struct Cyc_Absyn_Stmt*_tmp3A9;struct _dyneither_ptr*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Stmt*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B4;struct Cyc_List_List*_tmp3B5;struct Cyc_List_List*_tmp3B8;struct Cyc_Absyn_Decl*_tmp3BA;struct Cyc_Absyn_Stmt*_tmp3BB;struct _dyneither_ptr*_tmp3BD;struct Cyc_Absyn_Stmt*_tmp3BE;struct Cyc_Absyn_Stmt*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3C1;struct Cyc_Absyn_Stmt*_tmp3C3;struct Cyc_List_List*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C6;_LL22C: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp39B=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp39B->tag != 0)goto _LL22E;}_LL22D:
{const char*_tmp89F;s=Cyc_PP_text(((_tmp89F=";",_tag_dyneither(_tmp89F,sizeof(char),2))));}goto _LL22B;_LL22E: {struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp39C=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp39C->tag != 1)goto _LL230;else{_tmp39D=_tmp39C->f1;}}_LL22F:
{const char*_tmp8A2;struct Cyc_PP_Doc*_tmp8A1[2];s=((_tmp8A1[1]=Cyc_PP_text(((_tmp8A2=";",_tag_dyneither(_tmp8A2,sizeof(char),2)))),((_tmp8A1[0]=Cyc_Absynpp_exp2doc(_tmp39D),Cyc_PP_cat(_tag_dyneither(_tmp8A1,sizeof(struct Cyc_PP_Doc*),2))))));}goto _LL22B;_LL230: {struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp39E=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp39E->tag != 2)goto _LL232;else{_tmp39F=_tmp39E->f1;_tmp3A0=_tmp39E->f2;}}_LL231:
# 1367
 if(Cyc_Absynpp_decls_first){
if(Cyc_Absynpp_is_declaration(_tmp39F)){
struct Cyc_PP_Doc*_tmp8AD[5];struct Cyc_PP_Doc*_tmp8AC[7];const char*_tmp8AB;const char*_tmp8AA;struct Cyc_PP_Doc*_tmp8A9[7];s=((_tmp8A9[6]=
# 1375
Cyc_Absynpp_is_declaration(_tmp3A0)?
stmtexp?
(_tmp8AC[6]=
# 1380
Cyc_PP_line_doc(),((_tmp8AC[5]=
# 1379
Cyc_PP_text(((_tmp8AB=");",_tag_dyneither(_tmp8AB,sizeof(char),3)))),((_tmp8AC[4]=Cyc_Absynpp_rb(),((_tmp8AC[3]=
# 1378
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp)),((_tmp8AC[2]=
# 1377
Cyc_PP_blank_doc(),((_tmp8AC[1]=Cyc_Absynpp_lb(),((_tmp8AC[0]=Cyc_PP_text(((_tmp8AA="(",_tag_dyneither(_tmp8AA,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8AC,sizeof(struct Cyc_PP_Doc*),7))))))))))))))):(
# 1381
(_tmp8AD[4]=
# 1384
Cyc_PP_line_doc(),((_tmp8AD[3]=
# 1383
Cyc_Absynpp_rb(),((_tmp8AD[2]=
# 1382
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp)),((_tmp8AD[1]=
# 1381
Cyc_PP_blank_doc(),((_tmp8AD[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8AD,sizeof(struct Cyc_PP_Doc*),5)))))))))))):
# 1385
 Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp),((_tmp8A9[5]=
# 1374
Cyc_PP_line_doc(),((_tmp8A9[4]=
# 1373
Cyc_Absynpp_rb(),((_tmp8A9[3]=
# 1372
Cyc_PP_line_doc(),((_tmp8A9[2]=
# 1371
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp39F,0)),((_tmp8A9[1]=
# 1370
Cyc_PP_blank_doc(),((_tmp8A9[0]=
# 1369
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8A9,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1386
if(Cyc_Absynpp_is_declaration(_tmp3A0)){
struct Cyc_PP_Doc*_tmp8B8[4];struct Cyc_PP_Doc*_tmp8B7[6];const char*_tmp8B6;const char*_tmp8B5;struct Cyc_PP_Doc*_tmp8B4[4];s=((_tmp8B4[3]=
# 1397
Cyc_PP_line_doc(),((_tmp8B4[2]=
# 1389
stmtexp?
(_tmp8B7[5]=
# 1392
Cyc_PP_text(((_tmp8B6=");",_tag_dyneither(_tmp8B6,sizeof(char),3)))),((_tmp8B7[4]=Cyc_Absynpp_rb(),((_tmp8B7[3]=
# 1391
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp)),((_tmp8B7[2]=
# 1390
Cyc_PP_blank_doc(),((_tmp8B7[1]=Cyc_Absynpp_lb(),((_tmp8B7[0]=Cyc_PP_text(((_tmp8B5="(",_tag_dyneither(_tmp8B5,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp8B7,sizeof(struct Cyc_PP_Doc*),6))))))))))))):(
# 1394
(_tmp8B8[3]=
# 1396
Cyc_Absynpp_rb(),((_tmp8B8[2]=
# 1395
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp)),((_tmp8B8[1]=
# 1394
Cyc_PP_blank_doc(),((_tmp8B8[0]=Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp8B8,sizeof(struct Cyc_PP_Doc*),4)))))))))),((_tmp8B4[1]=
# 1388
Cyc_PP_line_doc(),((_tmp8B4[0]=
# 1387
Cyc_Absynpp_stmt2doc(_tmp39F,0),Cyc_PP_cat(_tag_dyneither(_tmp8B4,sizeof(struct Cyc_PP_Doc*),4))))))))));}else{
# 1399
struct Cyc_PP_Doc*_tmp8B9[3];s=((_tmp8B9[2]=Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp),((_tmp8B9[1]=Cyc_PP_line_doc(),((_tmp8B9[0]=Cyc_Absynpp_stmt2doc(_tmp39F,0),Cyc_PP_cat(_tag_dyneither(_tmp8B9,sizeof(struct Cyc_PP_Doc*),3))))))));}}}else{
# 1402
struct Cyc_PP_Doc*_tmp8BA[3];s=((_tmp8BA[2]=Cyc_Absynpp_stmt2doc(_tmp3A0,stmtexp),((_tmp8BA[1]=Cyc_PP_line_doc(),((_tmp8BA[0]=Cyc_Absynpp_stmt2doc(_tmp39F,0),Cyc_PP_cat(_tag_dyneither(_tmp8BA,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL232: {struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_tmp3A1=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3A1->tag != 3)goto _LL234;else{_tmp3A2=_tmp3A1->f1;}}_LL233:
# 1405
 if(_tmp3A2 == 0){
const char*_tmp8BB;s=Cyc_PP_text(((_tmp8BB="return;",_tag_dyneither(_tmp8BB,sizeof(char),8))));}else{
# 1408
const char*_tmp8C0;const char*_tmp8BF;struct Cyc_PP_Doc*_tmp8BE[3];s=((_tmp8BE[2]=
# 1410
Cyc_PP_text(((_tmp8BF=";",_tag_dyneither(_tmp8BF,sizeof(char),2)))),((_tmp8BE[1]=
# 1409
_tmp3A2 == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp3A2),((_tmp8BE[0]=
# 1408
Cyc_PP_text(((_tmp8C0="return ",_tag_dyneither(_tmp8C0,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8BE,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1411
goto _LL22B;_LL234: {struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_tmp3A3=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3A3->tag != 4)goto _LL236;else{_tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A3->f2;_tmp3A6=_tmp3A3->f3;}}_LL235: {
# 1413
int print_else;
{void*_tmp3DA=_tmp3A6->r;_LL251: {struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp3DB=(struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp3DA;if(_tmp3DB->tag != 0)goto _LL253;}_LL252:
 print_else=0;goto _LL250;_LL253:;_LL254:
 print_else=1;goto _LL250;_LL250:;}
# 1418
{const char*_tmp8CF;const char*_tmp8CE;struct Cyc_PP_Doc*_tmp8CD[2];struct Cyc_PP_Doc*_tmp8CC[6];struct Cyc_PP_Doc*_tmp8CB[2];const char*_tmp8CA;struct Cyc_PP_Doc*_tmp8C9[8];s=((_tmp8C9[7]=
# 1425
print_else?
(_tmp8CC[5]=
# 1431
Cyc_Absynpp_rb(),((_tmp8CC[4]=
# 1430
Cyc_PP_line_doc(),((_tmp8CC[3]=
# 1429
Cyc_PP_nest(2,((_tmp8CB[1]=Cyc_Absynpp_stmt2doc(_tmp3A6,0),((_tmp8CB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8CC[2]=
# 1428
Cyc_Absynpp_lb(),((_tmp8CC[1]=
# 1427
Cyc_PP_text(((_tmp8CA="else ",_tag_dyneither(_tmp8CA,sizeof(char),6)))),((_tmp8CC[0]=
# 1426
Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CC,sizeof(struct Cyc_PP_Doc*),6))))))))))))):
# 1432
 Cyc_PP_nil_doc(),((_tmp8C9[6]=
# 1424
Cyc_Absynpp_rb(),((_tmp8C9[5]=
# 1423
Cyc_PP_line_doc(),((_tmp8C9[4]=
# 1422
Cyc_PP_nest(2,((_tmp8CD[1]=Cyc_Absynpp_stmt2doc(_tmp3A5,0),((_tmp8CD[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8CD,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8C9[3]=
# 1421
Cyc_Absynpp_lb(),((_tmp8C9[2]=
# 1420
Cyc_PP_text(((_tmp8CE=") ",_tag_dyneither(_tmp8CE,sizeof(char),3)))),((_tmp8C9[1]=
# 1419
Cyc_Absynpp_exp2doc(_tmp3A4),((_tmp8C9[0]=
# 1418
Cyc_PP_text(((_tmp8CF="if (",_tag_dyneither(_tmp8CF,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8C9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1433
goto _LL22B;}_LL236: {struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_tmp3A7=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3A7->tag != 5)goto _LL238;else{_tmp3A8=(_tmp3A7->f1).f1;_tmp3A9=_tmp3A7->f2;}}_LL237:
# 1435
{const char*_tmp8D6;const char*_tmp8D5;struct Cyc_PP_Doc*_tmp8D4[2];struct Cyc_PP_Doc*_tmp8D3[7];s=((_tmp8D3[6]=
# 1441
Cyc_Absynpp_rb(),((_tmp8D3[5]=
# 1440
Cyc_PP_line_doc(),((_tmp8D3[4]=
# 1439
Cyc_PP_nest(2,((_tmp8D4[1]=Cyc_Absynpp_stmt2doc(_tmp3A9,0),((_tmp8D4[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8D4,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8D3[3]=
# 1438
Cyc_Absynpp_lb(),((_tmp8D3[2]=
# 1437
Cyc_PP_text(((_tmp8D5=") ",_tag_dyneither(_tmp8D5,sizeof(char),3)))),((_tmp8D3[1]=
# 1436
Cyc_Absynpp_exp2doc(_tmp3A8),((_tmp8D3[0]=
# 1435
Cyc_PP_text(((_tmp8D6="while (",_tag_dyneither(_tmp8D6,sizeof(char),8)))),Cyc_PP_cat(_tag_dyneither(_tmp8D3,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}
# 1442
goto _LL22B;_LL238: {struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*_tmp3AA=(struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3AA->tag != 6)goto _LL23A;}_LL239:
{const char*_tmp8D7;s=Cyc_PP_text(((_tmp8D7="break;",_tag_dyneither(_tmp8D7,sizeof(char),7))));}goto _LL22B;_LL23A: {struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*_tmp3AB=(struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3AB->tag != 7)goto _LL23C;}_LL23B:
{const char*_tmp8D8;s=Cyc_PP_text(((_tmp8D8="continue;",_tag_dyneither(_tmp8D8,sizeof(char),10))));}goto _LL22B;_LL23C: {struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp3AC=(struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3AC->tag != 8)goto _LL23E;else{_tmp3AD=_tmp3AC->f1;}}_LL23D:
{const char*_tmp8DC;void*_tmp8DB[1];struct Cyc_String_pa_PrintArg_struct _tmp8DA;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp8DA.tag=0,((_tmp8DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AD),((_tmp8DB[0]=& _tmp8DA,Cyc_aprintf(((_tmp8DC="goto %s;",_tag_dyneither(_tmp8DC,sizeof(char),9))),_tag_dyneither(_tmp8DB,sizeof(void*),1)))))))));}goto _LL22B;_LL23E: {struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_tmp3AE=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3AE->tag != 9)goto _LL240;else{_tmp3AF=_tmp3AE->f1;_tmp3B0=(_tmp3AE->f2).f1;_tmp3B1=(_tmp3AE->f3).f1;_tmp3B2=_tmp3AE->f4;}}_LL23F:
# 1447
{const char*_tmp8E7;const char*_tmp8E6;const char*_tmp8E5;const char*_tmp8E4;struct Cyc_PP_Doc*_tmp8E3[2];struct Cyc_PP_Doc*_tmp8E2[11];s=((_tmp8E2[10]=
# 1457
Cyc_Absynpp_rb(),((_tmp8E2[9]=
# 1456
Cyc_PP_line_doc(),((_tmp8E2[8]=
# 1455
Cyc_PP_nest(2,((_tmp8E3[1]=Cyc_Absynpp_stmt2doc(_tmp3B2,0),((_tmp8E3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp8E3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp8E2[7]=
# 1454
Cyc_Absynpp_lb(),((_tmp8E2[6]=
# 1453
Cyc_PP_text(((_tmp8E4=") ",_tag_dyneither(_tmp8E4,sizeof(char),3)))),((_tmp8E2[5]=
# 1452
Cyc_Absynpp_exp2doc(_tmp3B1),((_tmp8E2[4]=
# 1451
Cyc_PP_text(((_tmp8E5="; ",_tag_dyneither(_tmp8E5,sizeof(char),3)))),((_tmp8E2[3]=
# 1450
Cyc_Absynpp_exp2doc(_tmp3B0),((_tmp8E2[2]=
# 1449
Cyc_PP_text(((_tmp8E6="; ",_tag_dyneither(_tmp8E6,sizeof(char),3)))),((_tmp8E2[1]=
# 1448
Cyc_Absynpp_exp2doc(_tmp3AF),((_tmp8E2[0]=
# 1447
Cyc_PP_text(((_tmp8E7="for(",_tag_dyneither(_tmp8E7,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp8E2,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1458
goto _LL22B;_LL240: {struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp3B3=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3B3->tag != 10)goto _LL242;else{_tmp3B4=_tmp3B3->f1;_tmp3B5=_tmp3B3->f2;}}_LL241:
# 1460
{const char*_tmp8EC;const char*_tmp8EB;struct Cyc_PP_Doc*_tmp8EA[8];s=((_tmp8EA[7]=
# 1467
Cyc_Absynpp_rb(),((_tmp8EA[6]=
# 1466
Cyc_PP_line_doc(),((_tmp8EA[5]=
# 1465
Cyc_Absynpp_switchclauses2doc(_tmp3B5),((_tmp8EA[4]=
# 1464
Cyc_PP_line_doc(),((_tmp8EA[3]=
# 1463
Cyc_Absynpp_lb(),((_tmp8EA[2]=
# 1462
Cyc_PP_text(((_tmp8EB=") ",_tag_dyneither(_tmp8EB,sizeof(char),3)))),((_tmp8EA[1]=
# 1461
Cyc_Absynpp_exp2doc(_tmp3B4),((_tmp8EA[0]=
# 1460
Cyc_PP_text(((_tmp8EC="switch (",_tag_dyneither(_tmp8EC,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp8EA,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1468
goto _LL22B;_LL242: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B6=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3B6->tag != 11)goto _LL244;else{if(_tmp3B6->f1 != 0)goto _LL244;}}_LL243:
{const char*_tmp8ED;s=Cyc_PP_text(((_tmp8ED="fallthru;",_tag_dyneither(_tmp8ED,sizeof(char),10))));}goto _LL22B;_LL244: {struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_tmp3B7=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3B7->tag != 11)goto _LL246;else{_tmp3B8=_tmp3B7->f1;}}_LL245:
# 1471
{const char*_tmp8F2;const char*_tmp8F1;struct Cyc_PP_Doc*_tmp8F0[3];s=((_tmp8F0[2]=Cyc_PP_text(((_tmp8F1=");",_tag_dyneither(_tmp8F1,sizeof(char),3)))),((_tmp8F0[1]=Cyc_Absynpp_exps2doc_prec(20,_tmp3B8),((_tmp8F0[0]=Cyc_PP_text(((_tmp8F2="fallthru(",_tag_dyneither(_tmp8F2,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp8F0,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL22B;_LL246: {struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp3B9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3B9->tag != 12)goto _LL248;else{_tmp3BA=_tmp3B9->f1;_tmp3BB=_tmp3B9->f2;}}_LL247:
# 1473
{struct Cyc_PP_Doc*_tmp8F3[3];s=((_tmp8F3[2]=
# 1475
Cyc_Absynpp_stmt2doc(_tmp3BB,stmtexp),((_tmp8F3[1]=
# 1474
Cyc_PP_line_doc(),((_tmp8F3[0]=
# 1473
Cyc_Absynpp_decl2doc(_tmp3BA),Cyc_PP_cat(_tag_dyneither(_tmp8F3,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1476
goto _LL22B;_LL248: {struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp3BC=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3BC->tag != 13)goto _LL24A;else{_tmp3BD=_tmp3BC->f1;_tmp3BE=_tmp3BC->f2;}}_LL249:
# 1478
 if(Cyc_Absynpp_decls_first  && Cyc_Absynpp_is_declaration(_tmp3BE)){
if(stmtexp){
const char*_tmp8F8;const char*_tmp8F7;struct Cyc_PP_Doc*_tmp8F6[8];s=((_tmp8F6[7]=
# 1486
Cyc_PP_text(((_tmp8F7=");",_tag_dyneither(_tmp8F7,sizeof(char),3)))),((_tmp8F6[6]=Cyc_Absynpp_rb(),((_tmp8F6[5]=
# 1485
Cyc_PP_line_doc(),((_tmp8F6[4]=
# 1484
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,1)),((_tmp8F6[3]=
# 1483
Cyc_PP_line_doc(),((_tmp8F6[2]=
# 1482
Cyc_Absynpp_lb(),((_tmp8F6[1]=
# 1481
Cyc_PP_text(((_tmp8F8=": (",_tag_dyneither(_tmp8F8,sizeof(char),4)))),((_tmp8F6[0]=
# 1480
Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8F6,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1488
const char*_tmp8FB;struct Cyc_PP_Doc*_tmp8FA[7];s=((_tmp8FA[6]=
# 1494
Cyc_Absynpp_rb(),((_tmp8FA[5]=
# 1493
Cyc_PP_line_doc(),((_tmp8FA[4]=
# 1492
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3BE,0)),((_tmp8FA[3]=
# 1491
Cyc_PP_line_doc(),((_tmp8FA[2]=
# 1490
Cyc_Absynpp_lb(),((_tmp8FA[1]=
# 1489
Cyc_PP_text(((_tmp8FB=": ",_tag_dyneither(_tmp8FB,sizeof(char),3)))),((_tmp8FA[0]=
# 1488
Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8FA,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}}else{
# 1496
const char*_tmp8FE;struct Cyc_PP_Doc*_tmp8FD[3];s=((_tmp8FD[2]=Cyc_Absynpp_stmt2doc(_tmp3BE,stmtexp),((_tmp8FD[1]=Cyc_PP_text(((_tmp8FE=": ",_tag_dyneither(_tmp8FE,sizeof(char),3)))),((_tmp8FD[0]=Cyc_PP_textptr(_tmp3BD),Cyc_PP_cat(_tag_dyneither(_tmp8FD,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL24A: {struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_tmp3BF=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3BF->tag != 14)goto _LL24C;else{_tmp3C0=_tmp3BF->f1;_tmp3C1=(_tmp3BF->f2).f1;}}_LL24B:
# 1499
{const char*_tmp905;const char*_tmp904;const char*_tmp903;struct Cyc_PP_Doc*_tmp902[9];s=((_tmp902[8]=
# 1507
Cyc_PP_text(((_tmp903=");",_tag_dyneither(_tmp903,sizeof(char),3)))),((_tmp902[7]=
# 1506
Cyc_Absynpp_exp2doc(_tmp3C1),((_tmp902[6]=
# 1505
Cyc_PP_text(((_tmp904=" while (",_tag_dyneither(_tmp904,sizeof(char),9)))),((_tmp902[5]=
# 1504
Cyc_Absynpp_rb(),((_tmp902[4]=
# 1503
Cyc_PP_line_doc(),((_tmp902[3]=
# 1502
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C0,0)),((_tmp902[2]=
# 1501
Cyc_PP_line_doc(),((_tmp902[1]=
# 1500
Cyc_Absynpp_lb(),((_tmp902[0]=
# 1499
Cyc_PP_text(((_tmp905="do ",_tag_dyneither(_tmp905,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp902,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1508
goto _LL22B;_LL24C: {struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_tmp3C2=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3C2->tag != 15)goto _LL24E;else{_tmp3C3=_tmp3C2->f1;_tmp3C4=_tmp3C2->f2;}}_LL24D:
# 1510
{const char*_tmp90A;const char*_tmp909;struct Cyc_PP_Doc*_tmp908[12];s=((_tmp908[11]=
# 1521
Cyc_Absynpp_rb(),((_tmp908[10]=
# 1520
Cyc_PP_line_doc(),((_tmp908[9]=
# 1519
Cyc_PP_nest(2,Cyc_Absynpp_switchclauses2doc(_tmp3C4)),((_tmp908[8]=
# 1518
Cyc_PP_line_doc(),((_tmp908[7]=
# 1517
Cyc_Absynpp_lb(),((_tmp908[6]=
# 1516
Cyc_PP_text(((_tmp909=" catch ",_tag_dyneither(_tmp909,sizeof(char),8)))),((_tmp908[5]=
# 1515
Cyc_Absynpp_rb(),((_tmp908[4]=
# 1514
Cyc_PP_line_doc(),((_tmp908[3]=
# 1513
Cyc_PP_nest(2,Cyc_Absynpp_stmt2doc(_tmp3C3,0)),((_tmp908[2]=
# 1512
Cyc_PP_line_doc(),((_tmp908[1]=
# 1511
Cyc_Absynpp_lb(),((_tmp908[0]=
# 1510
Cyc_PP_text(((_tmp90A="try ",_tag_dyneither(_tmp90A,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp908,sizeof(struct Cyc_PP_Doc*),12))))))))))))))))))))))))));}
# 1522
goto _LL22B;_LL24E: {struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*_tmp3C5=(struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct*)_tmp39A;if(_tmp3C5->tag != 16)goto _LL22B;else{_tmp3C6=_tmp3C5->f1;}}_LL24F:
# 1524
{const char*_tmp90F;const char*_tmp90E;struct Cyc_PP_Doc*_tmp90D[3];s=((_tmp90D[2]=Cyc_PP_text(((_tmp90E=");",_tag_dyneither(_tmp90E,sizeof(char),3)))),((_tmp90D[1]=Cyc_Absynpp_exp2doc(_tmp3C6),((_tmp90D[0]=Cyc_PP_text(((_tmp90F="reset_region(",_tag_dyneither(_tmp90F,sizeof(char),14)))),Cyc_PP_cat(_tag_dyneither(_tmp90D,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL22B;_LL22B:;}
# 1527
return s;}
# 1530
struct Cyc_PP_Doc*Cyc_Absynpp_pat2doc(struct Cyc_Absyn_Pat*p){
struct Cyc_PP_Doc*s;
{void*_tmp40B=p->r;enum Cyc_Absyn_Sign _tmp40F;int _tmp410;char _tmp412;struct _dyneither_ptr _tmp414;struct Cyc_Absyn_Vardecl*_tmp416;struct Cyc_Absyn_Vardecl*_tmp419;struct Cyc_Absyn_Pat*_tmp41A;struct Cyc_Absyn_Tvar*_tmp41C;struct Cyc_Absyn_Vardecl*_tmp41D;struct Cyc_List_List*_tmp41F;int _tmp420;struct Cyc_Absyn_Pat*_tmp422;struct Cyc_Absyn_Vardecl*_tmp424;struct Cyc_Absyn_Vardecl*_tmp427;struct Cyc_Absyn_Pat*_tmp428;struct _tuple0*_tmp42A;struct _tuple0*_tmp42C;struct Cyc_List_List*_tmp42D;int _tmp42E;union Cyc_Absyn_AggrInfoU _tmp430;struct Cyc_List_List*_tmp431;struct Cyc_List_List*_tmp432;int _tmp433;struct Cyc_List_List*_tmp435;struct Cyc_List_List*_tmp436;int _tmp437;struct Cyc_Absyn_Enumfield*_tmp439;struct Cyc_Absyn_Enumfield*_tmp43B;struct Cyc_Absyn_Datatypefield*_tmp43D;struct Cyc_Absyn_Datatypefield*_tmp43F;struct Cyc_List_List*_tmp440;int _tmp441;struct Cyc_Absyn_Exp*_tmp443;_LL256: {struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp40C=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp40C->tag != 0)goto _LL258;}_LL257:
{const char*_tmp910;s=Cyc_PP_text(((_tmp910="_",_tag_dyneither(_tmp910,sizeof(char),2))));}goto _LL255;_LL258: {struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*_tmp40D=(struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp40D->tag != 8)goto _LL25A;}_LL259:
{const char*_tmp911;s=Cyc_PP_text(((_tmp911="NULL",_tag_dyneither(_tmp911,sizeof(char),5))));}goto _LL255;_LL25A: {struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp40E=(struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp40E->tag != 9)goto _LL25C;else{_tmp40F=_tmp40E->f1;_tmp410=_tmp40E->f2;}}_LL25B:
# 1536
 if(_tmp40F != (enum Cyc_Absyn_Sign)Cyc_Absyn_Unsigned){
const char*_tmp915;void*_tmp914[1];struct Cyc_Int_pa_PrintArg_struct _tmp913;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp913.tag=1,((_tmp913.f1=(unsigned long)_tmp410,((_tmp914[0]=& _tmp913,Cyc_aprintf(((_tmp915="%d",_tag_dyneither(_tmp915,sizeof(char),3))),_tag_dyneither(_tmp914,sizeof(void*),1)))))))));}else{
const char*_tmp919;void*_tmp918[1];struct Cyc_Int_pa_PrintArg_struct _tmp917;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp917.tag=1,((_tmp917.f1=(unsigned int)_tmp410,((_tmp918[0]=& _tmp917,Cyc_aprintf(((_tmp919="%u",_tag_dyneither(_tmp919,sizeof(char),3))),_tag_dyneither(_tmp918,sizeof(void*),1)))))))));}
goto _LL255;_LL25C: {struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp411=(struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp411->tag != 10)goto _LL25E;else{_tmp412=_tmp411->f1;}}_LL25D:
{const char*_tmp91D;void*_tmp91C[1];struct Cyc_String_pa_PrintArg_struct _tmp91B;s=Cyc_PP_text((struct _dyneither_ptr)((_tmp91B.tag=0,((_tmp91B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_char_escape(_tmp412)),((_tmp91C[0]=& _tmp91B,Cyc_aprintf(((_tmp91D="'%s'",_tag_dyneither(_tmp91D,sizeof(char),5))),_tag_dyneither(_tmp91C,sizeof(void*),1)))))))));}goto _LL255;_LL25E: {struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp413=(struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp413->tag != 11)goto _LL260;else{_tmp414=_tmp413->f1;}}_LL25F:
 s=Cyc_PP_text(_tmp414);goto _LL255;_LL260: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp415=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp415->tag != 1)goto _LL262;else{_tmp416=_tmp415->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp417=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp415->f2).r;if(_tmp417->tag != 0)goto _LL262;};}}_LL261:
# 1543
 s=Cyc_Absynpp_qvar2doc(_tmp416->name);goto _LL255;_LL262: {struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp418=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp418->tag != 1)goto _LL264;else{_tmp419=_tmp418->f1;_tmp41A=_tmp418->f2;}}_LL263:
# 1546
{const char*_tmp920;struct Cyc_PP_Doc*_tmp91F[3];s=((_tmp91F[2]=Cyc_Absynpp_pat2doc(_tmp41A),((_tmp91F[1]=Cyc_PP_text(((_tmp920=" as ",_tag_dyneither(_tmp920,sizeof(char),5)))),((_tmp91F[0]=Cyc_Absynpp_qvar2doc(_tmp419->name),Cyc_PP_cat(_tag_dyneither(_tmp91F,sizeof(struct Cyc_PP_Doc*),3))))))));}goto _LL255;_LL264: {struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp41B=(struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp41B->tag != 3)goto _LL266;else{_tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;}}_LL265:
# 1548
{const char*_tmp925;const char*_tmp924;struct Cyc_PP_Doc*_tmp923[4];s=((_tmp923[3]=Cyc_PP_text(((_tmp924=">",_tag_dyneither(_tmp924,sizeof(char),2)))),((_tmp923[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp41C)),((_tmp923[1]=Cyc_PP_text(((_tmp925="<",_tag_dyneither(_tmp925,sizeof(char),2)))),((_tmp923[0]=Cyc_Absynpp_qvar2doc(_tmp41D->name),Cyc_PP_cat(_tag_dyneither(_tmp923,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL266: {struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp41E=(struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp41E->tag != 4)goto _LL268;else{_tmp41F=_tmp41E->f1;_tmp420=_tmp41E->f2;}}_LL267:
# 1551
{const char*_tmp92E;const char*_tmp92D;const char*_tmp92C;const char*_tmp92B;struct Cyc_PP_Doc*_tmp92A[4];s=((_tmp92A[3]=
_tmp420?Cyc_PP_text(((_tmp92B=", ...)",_tag_dyneither(_tmp92B,sizeof(char),7)))): Cyc_PP_text(((_tmp92C=")",_tag_dyneither(_tmp92C,sizeof(char),2)))),((_tmp92A[2]=
# 1551
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Pat*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_pat2doc,((_tmp92D=",",_tag_dyneither(_tmp92D,sizeof(char),2))),_tmp41F),((_tmp92A[1]=Cyc_PP_text(((_tmp92E="(",_tag_dyneither(_tmp92E,sizeof(char),2)))),((_tmp92A[0]=Cyc_Absynpp_dollar(),Cyc_PP_cat(_tag_dyneither(_tmp92A,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1553
goto _LL255;_LL268: {struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp421=(struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp421->tag != 5)goto _LL26A;else{_tmp422=_tmp421->f1;}}_LL269:
# 1555
{const char*_tmp931;struct Cyc_PP_Doc*_tmp930[2];s=((_tmp930[1]=Cyc_Absynpp_pat2doc(_tmp422),((_tmp930[0]=Cyc_PP_text(((_tmp931="&",_tag_dyneither(_tmp931,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp930,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26A: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp423=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp423->tag != 2)goto _LL26C;else{_tmp424=_tmp423->f1;{struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_tmp425=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)(*_tmp423->f2).r;if(_tmp425->tag != 0)goto _LL26C;};}}_LL26B:
# 1558
{const char*_tmp934;struct Cyc_PP_Doc*_tmp933[2];s=((_tmp933[1]=Cyc_Absynpp_qvar2doc(_tmp424->name),((_tmp933[0]=Cyc_PP_text(((_tmp934="*",_tag_dyneither(_tmp934,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp933,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;_LL26C: {struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp426=(struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp426->tag != 2)goto _LL26E;else{_tmp427=_tmp426->f1;_tmp428=_tmp426->f2;}}_LL26D:
# 1561
{const char*_tmp939;const char*_tmp938;struct Cyc_PP_Doc*_tmp937[4];s=((_tmp937[3]=Cyc_Absynpp_pat2doc(_tmp428),((_tmp937[2]=Cyc_PP_text(((_tmp938=" as ",_tag_dyneither(_tmp938,sizeof(char),5)))),((_tmp937[1]=Cyc_Absynpp_qvar2doc(_tmp427->name),((_tmp937[0]=Cyc_PP_text(((_tmp939="*",_tag_dyneither(_tmp939,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp937,sizeof(struct Cyc_PP_Doc*),4))))))))));}
goto _LL255;_LL26E: {struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp429=(struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp429->tag != 14)goto _LL270;else{_tmp42A=_tmp429->f1;}}_LL26F:
# 1564
 s=Cyc_Absynpp_qvar2doc(_tmp42A);
goto _LL255;_LL270: {struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp42B=(struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp42B->tag != 15)goto _LL272;else{_tmp42C=_tmp42B->f1;_tmp42D=_tmp42B->f2;_tmp42E=_tmp42B->f3;}}_LL271: {
# 1567
const char*_tmp93B;const char*_tmp93A;struct _dyneither_ptr term=_tmp42E?(_tmp93B=", ...)",_tag_dyneither(_tmp93B,sizeof(char),7)):((_tmp93A=")",_tag_dyneither(_tmp93A,sizeof(char),2)));
{const char*_tmp940;const char*_tmp93F;struct Cyc_PP_Doc*_tmp93E[2];s=((_tmp93E[1]=Cyc_PP_group(((_tmp940="(",_tag_dyneither(_tmp940,sizeof(char),2))),term,((_tmp93F=",",_tag_dyneither(_tmp93F,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp42D)),((_tmp93E[0]=Cyc_Absynpp_qvar2doc(_tmp42C),Cyc_PP_cat(_tag_dyneither(_tmp93E,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL272: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp42F=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp42F->tag != 6)goto _LL274;else{if(_tmp42F->f1 == 0)goto _LL274;_tmp430=(*_tmp42F->f1).aggr_info;_tmp431=_tmp42F->f2;_tmp432=_tmp42F->f3;_tmp433=_tmp42F->f4;}}_LL273: {
# 1571
const char*_tmp942;const char*_tmp941;struct _dyneither_ptr term=_tmp433?(_tmp942=", ...}",_tag_dyneither(_tmp942,sizeof(char),7)):((_tmp941="}",_tag_dyneither(_tmp941,sizeof(char),2)));
struct _tuple0*_tmp466;struct _tuple10 _tmp465=Cyc_Absyn_aggr_kinded_name(_tmp430);_tmp466=_tmp465.f2;
{const char*_tmp94D;const char*_tmp94C;const char*_tmp94B;const char*_tmp94A;const char*_tmp949;struct Cyc_PP_Doc*_tmp948[4];s=((_tmp948[3]=
# 1575
Cyc_PP_group(((_tmp94A="",_tag_dyneither(_tmp94A,sizeof(char),1))),term,((_tmp949=",",_tag_dyneither(_tmp949,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp432)),((_tmp948[2]=
# 1574
Cyc_PP_egroup(((_tmp94D="[",_tag_dyneither(_tmp94D,sizeof(char),2))),((_tmp94C="]",_tag_dyneither(_tmp94C,sizeof(char),2))),((_tmp94B=",",_tag_dyneither(_tmp94B,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp431))),((_tmp948[1]=
# 1573
Cyc_Absynpp_lb(),((_tmp948[0]=Cyc_Absynpp_qvar2doc(_tmp466),Cyc_PP_cat(_tag_dyneither(_tmp948,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1576
goto _LL255;}_LL274: {struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp434=(struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp434->tag != 6)goto _LL276;else{if(_tmp434->f1 != 0)goto _LL276;_tmp435=_tmp434->f2;_tmp436=_tmp434->f3;_tmp437=_tmp434->f4;}}_LL275: {
# 1578
const char*_tmp94F;const char*_tmp94E;struct _dyneither_ptr term=_tmp437?(_tmp94F=", ...}",_tag_dyneither(_tmp94F,sizeof(char),7)):((_tmp94E="}",_tag_dyneither(_tmp94E,sizeof(char),2)));
{const char*_tmp95A;const char*_tmp959;const char*_tmp958;const char*_tmp957;const char*_tmp956;struct Cyc_PP_Doc*_tmp955[3];s=((_tmp955[2]=
# 1581
Cyc_PP_group(((_tmp957="",_tag_dyneither(_tmp957,sizeof(char),1))),term,((_tmp956=",",_tag_dyneither(_tmp956,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_dp2doc,_tmp436)),((_tmp955[1]=
# 1580
Cyc_PP_egroup(((_tmp95A="[",_tag_dyneither(_tmp95A,sizeof(char),2))),((_tmp959="]",_tag_dyneither(_tmp959,sizeof(char),2))),((_tmp958=",",_tag_dyneither(_tmp958,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_PP_textptr,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_get_name,_tmp435))),((_tmp955[0]=
# 1579
Cyc_Absynpp_lb(),Cyc_PP_cat(_tag_dyneither(_tmp955,sizeof(struct Cyc_PP_Doc*),3))))))));}
# 1582
goto _LL255;}_LL276: {struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp438=(struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp438->tag != 12)goto _LL278;else{_tmp439=_tmp438->f2;}}_LL277:
 _tmp43B=_tmp439;goto _LL279;_LL278: {struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp43A=(struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp43A->tag != 13)goto _LL27A;else{_tmp43B=_tmp43A->f2;}}_LL279:
 s=Cyc_Absynpp_qvar2doc(_tmp43B->name);goto _LL255;_LL27A: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp43C=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp43C->tag != 7)goto _LL27C;else{_tmp43D=_tmp43C->f2;if(_tmp43C->f3 != 0)goto _LL27C;}}_LL27B:
 s=Cyc_Absynpp_qvar2doc(_tmp43D->name);goto _LL255;_LL27C: {struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp43E=(struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp43E->tag != 7)goto _LL27E;else{_tmp43F=_tmp43E->f2;_tmp440=_tmp43E->f3;_tmp441=_tmp43E->f4;}}_LL27D: {
# 1587
const char*_tmp95C;const char*_tmp95B;struct _dyneither_ptr term=_tmp441?(_tmp95C=", ...)",_tag_dyneither(_tmp95C,sizeof(char),7)):((_tmp95B=")",_tag_dyneither(_tmp95B,sizeof(char),2)));
{const char*_tmp961;const char*_tmp960;struct Cyc_PP_Doc*_tmp95F[2];s=((_tmp95F[1]=Cyc_PP_egroup(((_tmp961="(",_tag_dyneither(_tmp961,sizeof(char),2))),term,((_tmp960=",",_tag_dyneither(_tmp960,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_pat2doc,_tmp440)),((_tmp95F[0]=Cyc_Absynpp_qvar2doc(_tmp43F->name),Cyc_PP_cat(_tag_dyneither(_tmp95F,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL255;}_LL27E: {struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp442=(struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp40B;if(_tmp442->tag != 16)goto _LL255;else{_tmp443=_tmp442->f1;}}_LL27F:
# 1591
 s=Cyc_Absynpp_exp2doc(_tmp443);goto _LL255;_LL255:;}
# 1593
return s;}
# 1596
struct Cyc_PP_Doc*Cyc_Absynpp_dp2doc(struct _tuple13*dp){
const char*_tmp968;const char*_tmp967;const char*_tmp966;struct Cyc_PP_Doc*_tmp965[2];return(_tmp965[1]=
Cyc_Absynpp_pat2doc((*dp).f2),((_tmp965[0]=
# 1597
Cyc_PP_egroup(((_tmp968="",_tag_dyneither(_tmp968,sizeof(char),1))),((_tmp967="=",_tag_dyneither(_tmp967,sizeof(char),2))),((_tmp966="=",_tag_dyneither(_tmp966,sizeof(char),2))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_designator2doc,(*dp).f1)),Cyc_PP_cat(_tag_dyneither(_tmp965,sizeof(struct Cyc_PP_Doc*),2)))));}
# 1601
struct Cyc_PP_Doc*Cyc_Absynpp_switchclause2doc(struct Cyc_Absyn_Switch_clause*c){
if(c->where_clause == 0  && (c->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val){
const char*_tmp96D;struct Cyc_PP_Doc*_tmp96C[2];struct Cyc_PP_Doc*_tmp96B[2];return(_tmp96B[1]=
Cyc_PP_nest(2,((_tmp96C[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp96C[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp96C,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp96B[0]=
# 1603
Cyc_PP_text(((_tmp96D="default: ",_tag_dyneither(_tmp96D,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmp96B,sizeof(struct Cyc_PP_Doc*),2)))));}else{
# 1605
if(c->where_clause == 0){
const char*_tmp974;const char*_tmp973;struct Cyc_PP_Doc*_tmp972[2];struct Cyc_PP_Doc*_tmp971[4];return(_tmp971[3]=
# 1609
Cyc_PP_nest(2,((_tmp972[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp972[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp972,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp971[2]=
# 1608
Cyc_PP_text(((_tmp973=": ",_tag_dyneither(_tmp973,sizeof(char),3)))),((_tmp971[1]=
# 1607
Cyc_Absynpp_pat2doc(c->pattern),((_tmp971[0]=
# 1606
Cyc_PP_text(((_tmp974="case ",_tag_dyneither(_tmp974,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp971,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1611
const char*_tmp97D;const char*_tmp97C;const char*_tmp97B;struct Cyc_PP_Doc*_tmp97A[2];struct Cyc_PP_Doc*_tmp979[6];return(_tmp979[5]=
# 1616
Cyc_PP_nest(2,((_tmp97A[1]=Cyc_Absynpp_stmt2doc(c->body,0),((_tmp97A[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp97A,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp979[4]=
# 1615
Cyc_PP_text(((_tmp97B=": ",_tag_dyneither(_tmp97B,sizeof(char),3)))),((_tmp979[3]=
# 1614
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(c->where_clause)),((_tmp979[2]=
# 1613
Cyc_PP_text(((_tmp97C=" && ",_tag_dyneither(_tmp97C,sizeof(char),5)))),((_tmp979[1]=
# 1612
Cyc_Absynpp_pat2doc(c->pattern),((_tmp979[0]=
# 1611
Cyc_PP_text(((_tmp97D="case ",_tag_dyneither(_tmp97D,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp979,sizeof(struct Cyc_PP_Doc*),6)))))))))))));}}}
# 1619
struct Cyc_PP_Doc*Cyc_Absynpp_switchclauses2doc(struct Cyc_List_List*cs){
const char*_tmp97E;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Switch_clause*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_switchclause2doc,((_tmp97E="",_tag_dyneither(_tmp97E,sizeof(char),1))),cs);}
# 1623
struct Cyc_PP_Doc*Cyc_Absynpp_enumfield2doc(struct Cyc_Absyn_Enumfield*f){
if(f->tag == 0)
return Cyc_Absynpp_qvar2doc(f->name);else{
# 1627
const char*_tmp981;struct Cyc_PP_Doc*_tmp980[3];return(_tmp980[2]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->tag)),((_tmp980[1]=Cyc_PP_text(((_tmp981=" = ",_tag_dyneither(_tmp981,sizeof(char),4)))),((_tmp980[0]=Cyc_Absynpp_qvar2doc(f->name),Cyc_PP_cat(_tag_dyneither(_tmp980,sizeof(struct Cyc_PP_Doc*),3)))))));}}
# 1630
struct Cyc_PP_Doc*Cyc_Absynpp_enumfields2doc(struct Cyc_List_List*fs){
const char*_tmp982;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Enumfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_enumfield2doc,((_tmp982=",",_tag_dyneither(_tmp982,sizeof(char),2))),fs);}
# 1634
static struct Cyc_PP_Doc*Cyc_Absynpp_id2doc(struct Cyc_Absyn_Vardecl*v){
return Cyc_Absynpp_qvar2doc(v->name);}
# 1638
static struct Cyc_PP_Doc*Cyc_Absynpp_ids2doc(struct Cyc_List_List*vds){
const char*_tmp983;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Vardecl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseq)(Cyc_Absynpp_id2doc,((_tmp983=",",_tag_dyneither(_tmp983,sizeof(char),2))),vds);}
# 1642
struct Cyc_PP_Doc*Cyc_Absynpp_vardecl2doc(struct Cyc_Absyn_Vardecl*vd){
enum Cyc_Absyn_Scope _tmp492;struct _tuple0*_tmp493;struct Cyc_Absyn_Tqual _tmp494;void*_tmp495;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_List_List*_tmp497;struct Cyc_Absyn_Vardecl*_tmp491=vd;_tmp492=(*_tmp491).sc;_tmp493=(*_tmp491).name;_tmp494=(*_tmp491).tq;_tmp495=(*_tmp491).type;_tmp496=(*_tmp491).initializer;_tmp497=(*_tmp491).attributes;{
struct Cyc_PP_Doc*s;
struct Cyc_PP_Doc*sn=Cyc_Absynpp_typedef_name2bolddoc(_tmp493);
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp497);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL280:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL281:
# 1651
{void*_tmp498=Cyc_Tcutil_compress(_tmp495);struct Cyc_List_List*_tmp49A;_LL284: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp499=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp498;if(_tmp499->tag != 9)goto _LL286;else{_tmp49A=(_tmp499->f1).attributes;}}_LL285:
# 1653
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp49A);
goto _LL283;_LL286:;_LL287:
 beforenamedoc=Cyc_PP_nil_doc();goto _LL283;_LL283:;}
# 1657
break;}{
# 1660
struct Cyc_PP_Doc*tmp_doc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL288:
 tmp_doc=Cyc_PP_nil_doc();break;case Cyc_Cyclone_Vc_c: _LL289:
 tmp_doc=attsdoc;break;}
# 1665
{struct Cyc_Core_Opt*_tmp990;struct Cyc_PP_Doc*_tmp98F[2];struct Cyc_PP_Doc*_tmp98E[2];const char*_tmp98D;const char*_tmp98C;struct Cyc_PP_Doc*_tmp98B[5];s=((_tmp98B[4]=
# 1672
Cyc_PP_text(((_tmp98C=";",_tag_dyneither(_tmp98C,sizeof(char),2)))),((_tmp98B[3]=
# 1669
_tmp496 == 0?
Cyc_PP_nil_doc():(
(_tmp98E[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp496),((_tmp98E[0]=Cyc_PP_text(((_tmp98D=" = ",_tag_dyneither(_tmp98D,sizeof(char),4)))),Cyc_PP_cat(_tag_dyneither(_tmp98E,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp98B[2]=
# 1668
Cyc_Absynpp_tqtd2doc(_tmp494,_tmp495,((_tmp990=_cycalloc(sizeof(*_tmp990)),((_tmp990->v=((_tmp98F[1]=sn,((_tmp98F[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp98F,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp990))))),((_tmp98B[1]=
# 1667
Cyc_Absynpp_scope2doc(_tmp492),((_tmp98B[0]=tmp_doc,Cyc_PP_cat(_tag_dyneither(_tmp98B,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1673
return s;};};}struct _tuple17{unsigned int f1;struct _tuple0*f2;int f3;};
# 1676
struct Cyc_PP_Doc*Cyc_Absynpp_export2doc(struct _tuple17*x){
struct _tuple0*_tmp4A2;struct _tuple17 _tmp4A1=*x;_tmp4A2=_tmp4A1.f2;
return Cyc_Absynpp_qvar2doc(_tmp4A2);}
# 1681
struct Cyc_PP_Doc*Cyc_Absynpp_aggrdecl2doc(struct Cyc_Absyn_Aggrdecl*ad){
if(ad->impl == 0){
struct Cyc_PP_Doc*_tmp991[4];return(_tmp991[3]=
# 1686
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp991[2]=
# 1685
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp991[1]=
# 1684
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp991[0]=
# 1683
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp991,sizeof(struct Cyc_PP_Doc*),4)))))))));}else{
# 1688
struct Cyc_PP_Doc*_tmp999[2];const char*_tmp998;struct Cyc_PP_Doc*_tmp997[2];struct Cyc_PP_Doc*_tmp996[12];return(_tmp996[11]=
# 1699
Cyc_Absynpp_atts2doc(ad->attributes),((_tmp996[10]=
# 1698
Cyc_Absynpp_rb(),((_tmp996[9]=
# 1697
Cyc_PP_line_doc(),((_tmp996[8]=
# 1696
Cyc_PP_nest(2,((_tmp997[1]=Cyc_Absynpp_aggrfields2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields),((_tmp997[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp997,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp996[7]=
# 1694
((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po == 0?Cyc_PP_nil_doc():(
(_tmp999[1]=Cyc_Absynpp_rgnpo2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po),((_tmp999[0]=Cyc_PP_text(((_tmp998=":",_tag_dyneither(_tmp998,sizeof(char),2)))),Cyc_PP_cat(_tag_dyneither(_tmp999,sizeof(struct Cyc_PP_Doc*),2)))))),((_tmp996[6]=
# 1693
Cyc_Absynpp_ktvars2doc(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars),((_tmp996[5]=
# 1692
Cyc_Absynpp_lb(),((_tmp996[4]=Cyc_PP_blank_doc(),((_tmp996[3]=
# 1691
Cyc_Absynpp_ktvars2doc(ad->tvs),((_tmp996[2]=
# 1690
Cyc_Absynpp_qvar2bolddoc(ad->name),((_tmp996[1]=
# 1689
Cyc_Absynpp_aggr_kind2doc(ad->kind),((_tmp996[0]=
# 1688
Cyc_Absynpp_scope2doc(ad->sc),Cyc_PP_cat(_tag_dyneither(_tmp996,sizeof(struct Cyc_PP_Doc*),12)))))))))))))))))))))))));}}
# 1702
struct Cyc_PP_Doc*Cyc_Absynpp_datatypedecl2doc(struct Cyc_Absyn_Datatypedecl*dd){
enum Cyc_Absyn_Scope _tmp4A9;struct _tuple0*_tmp4AA;struct Cyc_List_List*_tmp4AB;struct Cyc_Core_Opt*_tmp4AC;int _tmp4AD;struct Cyc_Absyn_Datatypedecl*_tmp4A8=dd;_tmp4A9=(*_tmp4A8).sc;_tmp4AA=(*_tmp4A8).name;_tmp4AB=(*_tmp4A8).tvs;_tmp4AC=(*_tmp4A8).fields;_tmp4AD=(*_tmp4A8).is_extensible;
if(_tmp4AC == 0){
const char*_tmp99E;const char*_tmp99D;struct Cyc_PP_Doc*_tmp99C[5];return(_tmp99C[4]=
# 1709
Cyc_Absynpp_ktvars2doc(_tmp4AB),((_tmp99C[3]=
# 1708
_tmp4AD?Cyc_Absynpp_qvar2bolddoc(_tmp4AA): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AA),((_tmp99C[2]=
# 1707
Cyc_PP_text(((_tmp99D="datatype ",_tag_dyneither(_tmp99D,sizeof(char),10)))),((_tmp99C[1]=
# 1706
_tmp4AD?Cyc_PP_text(((_tmp99E="@extensible ",_tag_dyneither(_tmp99E,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp99C[0]=
# 1705
Cyc_Absynpp_scope2doc(_tmp4A9),Cyc_PP_cat(_tag_dyneither(_tmp99C,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 1711
const char*_tmp9A5;const char*_tmp9A4;struct Cyc_PP_Doc*_tmp9A3[2];struct Cyc_PP_Doc*_tmp9A2[10];return(_tmp9A2[9]=
# 1719
Cyc_Absynpp_rb(),((_tmp9A2[8]=
# 1718
Cyc_PP_line_doc(),((_tmp9A2[7]=
# 1717
Cyc_PP_nest(2,((_tmp9A3[1]=Cyc_Absynpp_datatypefields2doc((struct Cyc_List_List*)_tmp4AC->v),((_tmp9A3[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9A3,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9A2[6]=
# 1716
Cyc_Absynpp_lb(),((_tmp9A2[5]=Cyc_PP_blank_doc(),((_tmp9A2[4]=
# 1715
Cyc_Absynpp_ktvars2doc(_tmp4AB),((_tmp9A2[3]=
# 1714
_tmp4AD?Cyc_Absynpp_qvar2bolddoc(_tmp4AA): Cyc_Absynpp_typedef_name2bolddoc(_tmp4AA),((_tmp9A2[2]=
# 1713
Cyc_PP_text(((_tmp9A4="datatype ",_tag_dyneither(_tmp9A4,sizeof(char),10)))),((_tmp9A2[1]=
# 1712
_tmp4AD?Cyc_PP_text(((_tmp9A5="@extensible ",_tag_dyneither(_tmp9A5,sizeof(char),13)))): Cyc_PP_blank_doc(),((_tmp9A2[0]=
# 1711
Cyc_Absynpp_scope2doc(_tmp4A9),Cyc_PP_cat(_tag_dyneither(_tmp9A2,sizeof(struct Cyc_PP_Doc*),10)))))))))))))))))))));}}
# 1722
struct Cyc_PP_Doc*Cyc_Absynpp_enumdecl2doc(struct Cyc_Absyn_Enumdecl*ed){
enum Cyc_Absyn_Scope _tmp4B6;struct _tuple0*_tmp4B7;struct Cyc_Core_Opt*_tmp4B8;struct Cyc_Absyn_Enumdecl*_tmp4B5=ed;_tmp4B6=(*_tmp4B5).sc;_tmp4B7=(*_tmp4B5).name;_tmp4B8=(*_tmp4B5).fields;
if(_tmp4B8 == 0){
const char*_tmp9A8;struct Cyc_PP_Doc*_tmp9A7[3];return(_tmp9A7[2]=
# 1727
Cyc_Absynpp_typedef_name2bolddoc(_tmp4B7),((_tmp9A7[1]=
# 1726
Cyc_PP_text(((_tmp9A8="enum ",_tag_dyneither(_tmp9A8,sizeof(char),6)))),((_tmp9A7[0]=
# 1725
Cyc_Absynpp_scope2doc(_tmp4B6),Cyc_PP_cat(_tag_dyneither(_tmp9A7,sizeof(struct Cyc_PP_Doc*),3)))))));}else{
# 1730
const char*_tmp9AD;struct Cyc_PP_Doc*_tmp9AC[2];struct Cyc_PP_Doc*_tmp9AB[8];return(_tmp9AB[7]=
# 1736
Cyc_Absynpp_rb(),((_tmp9AB[6]=
# 1735
Cyc_PP_line_doc(),((_tmp9AB[5]=
# 1734
Cyc_PP_nest(2,((_tmp9AC[1]=Cyc_Absynpp_enumfields2doc((struct Cyc_List_List*)_tmp4B8->v),((_tmp9AC[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9AC,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9AB[4]=
# 1733
Cyc_Absynpp_lb(),((_tmp9AB[3]=Cyc_PP_blank_doc(),((_tmp9AB[2]=
# 1732
Cyc_Absynpp_qvar2bolddoc(_tmp4B7),((_tmp9AB[1]=
# 1731
Cyc_PP_text(((_tmp9AD="enum ",_tag_dyneither(_tmp9AD,sizeof(char),6)))),((_tmp9AB[0]=
# 1730
Cyc_Absynpp_scope2doc(_tmp4B6),Cyc_PP_cat(_tag_dyneither(_tmp9AB,sizeof(struct Cyc_PP_Doc*),8)))))))))))))))));}}
# 1739
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*d){
struct Cyc_PP_Doc*s;
{void*_tmp4BE=d->r;struct Cyc_Absyn_Fndecl*_tmp4C0;struct Cyc_Absyn_Aggrdecl*_tmp4C2;struct Cyc_Absyn_Vardecl*_tmp4C4;struct Cyc_Absyn_Tvar*_tmp4C6;struct Cyc_Absyn_Vardecl*_tmp4C7;int _tmp4C8;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_Absyn_Exp*_tmp4CB;struct Cyc_Absyn_Tvar*_tmp4CC;struct Cyc_Absyn_Vardecl*_tmp4CD;struct Cyc_Absyn_Datatypedecl*_tmp4CF;struct Cyc_Absyn_Pat*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_List_List*_tmp4D4;struct Cyc_Absyn_Enumdecl*_tmp4D6;struct Cyc_Absyn_Typedefdecl*_tmp4D8;struct _dyneither_ptr*_tmp4DA;struct Cyc_List_List*_tmp4DB;struct _tuple0*_tmp4DD;struct Cyc_List_List*_tmp4DE;struct Cyc_List_List*_tmp4E0;struct Cyc_List_List*_tmp4E2;struct Cyc_List_List*_tmp4E3;_LL28C: {struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp4BF=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4BF->tag != 1)goto _LL28E;else{_tmp4C0=_tmp4BF->f1;}}_LL28D: {
# 1743
struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp9B3;struct Cyc_Absyn_FnInfo _tmp9B2;struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9B1;void*t=(void*)((_tmp9B1=_cycalloc(sizeof(*_tmp9B1)),((_tmp9B1[0]=((_tmp9B3.tag=9,((_tmp9B3.f1=((_tmp9B2.tvars=_tmp4C0->tvs,((_tmp9B2.effect=_tmp4C0->effect,((_tmp9B2.ret_tqual=_tmp4C0->ret_tqual,((_tmp9B2.ret_typ=_tmp4C0->ret_type,((_tmp9B2.args=
# 1747
((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp4C0->args),((_tmp9B2.c_varargs=_tmp4C0->c_varargs,((_tmp9B2.cyc_varargs=_tmp4C0->cyc_varargs,((_tmp9B2.rgn_po=_tmp4C0->rgn_po,((_tmp9B2.attributes=0,_tmp9B2)))))))))))))))))),_tmp9B3)))),_tmp9B1))));
# 1751
struct Cyc_PP_Doc*attsdoc=Cyc_Absynpp_atts2doc(_tmp4C0->attributes);
struct Cyc_PP_Doc*inlinedoc;
if(_tmp4C0->is_inline)
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AC:
{const char*_tmp9B4;inlinedoc=Cyc_PP_text(((_tmp9B4="inline ",_tag_dyneither(_tmp9B4,sizeof(char),8))));}break;case Cyc_Cyclone_Vc_c: _LL2AD:
{const char*_tmp9B5;inlinedoc=Cyc_PP_text(((_tmp9B5="__inline ",_tag_dyneither(_tmp9B5,sizeof(char),10))));}break;}else{
# 1759
inlinedoc=Cyc_PP_nil_doc();}{
struct Cyc_PP_Doc*scopedoc=Cyc_Absynpp_scope2doc(_tmp4C0->sc);
struct Cyc_PP_Doc*beforenamedoc;
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2AF:
 beforenamedoc=attsdoc;break;case Cyc_Cyclone_Vc_c: _LL2B0:
 beforenamedoc=Cyc_Absynpp_callconv2doc(_tmp4C0->attributes);break;}{
# 1766
struct Cyc_PP_Doc*namedoc=Cyc_Absynpp_typedef_name2doc(_tmp4C0->name);
struct Cyc_PP_Doc*_tmp9B8[2];struct Cyc_Core_Opt*_tmp9B7;struct Cyc_PP_Doc*tqtddoc=Cyc_Absynpp_tqtd2doc(Cyc_Absyn_empty_tqual(0),t,(
(_tmp9B7=_cycalloc(sizeof(*_tmp9B7)),((_tmp9B7->v=((_tmp9B8[1]=namedoc,((_tmp9B8[0]=beforenamedoc,Cyc_PP_cat(_tag_dyneither(_tmp9B8,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9B7)))));
struct Cyc_PP_Doc*_tmp9BB[2];struct Cyc_PP_Doc*_tmp9BA[5];struct Cyc_PP_Doc*bodydoc=(_tmp9BA[4]=
# 1772
Cyc_Absynpp_rb(),((_tmp9BA[3]=
# 1771
Cyc_PP_line_doc(),((_tmp9BA[2]=
# 1770
Cyc_PP_nest(2,((_tmp9BB[1]=Cyc_Absynpp_stmt2doc(_tmp4C0->body,0),((_tmp9BB[0]=Cyc_PP_line_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BB,sizeof(struct Cyc_PP_Doc*),2))))))),((_tmp9BA[1]=
# 1769
Cyc_Absynpp_lb(),((_tmp9BA[0]=Cyc_PP_blank_doc(),Cyc_PP_cat(_tag_dyneither(_tmp9BA,sizeof(struct Cyc_PP_Doc*),5)))))))))));
# 1773
{struct Cyc_PP_Doc*_tmp9BC[4];s=((_tmp9BC[3]=bodydoc,((_tmp9BC[2]=tqtddoc,((_tmp9BC[1]=scopedoc,((_tmp9BC[0]=inlinedoc,Cyc_PP_cat(_tag_dyneither(_tmp9BC,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1775
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Vc_c: _LL2B2:
{struct Cyc_PP_Doc*_tmp9BD[2];s=((_tmp9BD[1]=s,((_tmp9BD[0]=attsdoc,Cyc_PP_cat(_tag_dyneither(_tmp9BD,sizeof(struct Cyc_PP_Doc*),2))))));}break;default: _LL2B3:
 break;}
# 1780
goto _LL28B;};};}_LL28E: {struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp4C1=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4C1->tag != 6)goto _LL290;else{_tmp4C2=_tmp4C1->f1;}}_LL28F:
# 1783
{const char*_tmp9C0;struct Cyc_PP_Doc*_tmp9BF[2];s=((_tmp9BF[1]=Cyc_PP_text(((_tmp9C0=";",_tag_dyneither(_tmp9C0,sizeof(char),2)))),((_tmp9BF[0]=Cyc_Absynpp_aggrdecl2doc(_tmp4C2),Cyc_PP_cat(_tag_dyneither(_tmp9BF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL290: {struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp4C3=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4C3->tag != 0)goto _LL292;else{_tmp4C4=_tmp4C3->f1;}}_LL291:
# 1786
 s=Cyc_Absynpp_vardecl2doc(_tmp4C4);
goto _LL28B;_LL292: {struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_tmp4C5=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4C5->tag != 4)goto _LL294;else{_tmp4C6=_tmp4C5->f1;_tmp4C7=_tmp4C5->f2;_tmp4C8=_tmp4C5->f3;_tmp4C9=_tmp4C5->f4;}}_LL293:
# 1789
{const char*_tmp9D3;const char*_tmp9D2;const char*_tmp9D1;const char*_tmp9D0;struct Cyc_PP_Doc*_tmp9CF[3];const char*_tmp9CE;const char*_tmp9CD;const char*_tmp9CC;struct Cyc_PP_Doc*_tmp9CB[8];s=((_tmp9CB[7]=
# 1797
Cyc_PP_text(((_tmp9CC=";",_tag_dyneither(_tmp9CC,sizeof(char),2)))),((_tmp9CB[6]=
# 1795
_tmp4C9 != 0?
(_tmp9CF[2]=Cyc_PP_text(((_tmp9CE=")",_tag_dyneither(_tmp9CE,sizeof(char),2)))),((_tmp9CF[1]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_tmp4C9),((_tmp9CF[0]=Cyc_PP_text(((_tmp9CD=" = open(",_tag_dyneither(_tmp9CD,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9CF,sizeof(struct Cyc_PP_Doc*),3))))))):
 Cyc_PP_nil_doc(),((_tmp9CB[5]=
# 1794
_tmp4C8?Cyc_PP_text(((_tmp9D0=" @resetable",_tag_dyneither(_tmp9D0,sizeof(char),12)))): Cyc_PP_nil_doc(),((_tmp9CB[4]=
# 1793
Cyc_Absynpp_qvar2doc(_tmp4C7->name),((_tmp9CB[3]=
# 1792
Cyc_PP_text(((_tmp9D1=">",_tag_dyneither(_tmp9D1,sizeof(char),2)))),((_tmp9CB[2]=
# 1791
Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4C6)),((_tmp9CB[1]=
# 1790
Cyc_PP_text(((_tmp9D2="<",_tag_dyneither(_tmp9D2,sizeof(char),2)))),((_tmp9CB[0]=
# 1789
Cyc_PP_text(((_tmp9D3="region",_tag_dyneither(_tmp9D3,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmp9CB,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1798
goto _LL28B;_LL294: {struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*_tmp4CA=(struct Cyc_Absyn_Alias_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4CA->tag != 5)goto _LL296;else{_tmp4CB=_tmp4CA->f1;_tmp4CC=_tmp4CA->f2;_tmp4CD=_tmp4CA->f3;}}_LL295:
# 1800
{const char*_tmp9DE;const char*_tmp9DD;const char*_tmp9DC;const char*_tmp9DB;const char*_tmp9DA;struct Cyc_PP_Doc*_tmp9D9[8];s=((_tmp9D9[7]=
Cyc_PP_text(((_tmp9DA=";",_tag_dyneither(_tmp9DA,sizeof(char),2)))),((_tmp9D9[6]=Cyc_Absynpp_exp2doc(_tmp4CB),((_tmp9D9[5]=Cyc_PP_text(((_tmp9DB=" = ",_tag_dyneither(_tmp9DB,sizeof(char),4)))),((_tmp9D9[4]=Cyc_Absynpp_qvar2doc(_tmp4CD->name),((_tmp9D9[3]=
# 1800
Cyc_PP_text(((_tmp9DC="> ",_tag_dyneither(_tmp9DC,sizeof(char),3)))),((_tmp9D9[2]=Cyc_PP_textptr(Cyc_Absynpp_get_name(_tmp4CC)),((_tmp9D9[1]=Cyc_PP_text(((_tmp9DD=" <",_tag_dyneither(_tmp9DD,sizeof(char),3)))),((_tmp9D9[0]=Cyc_PP_text(((_tmp9DE="alias",_tag_dyneither(_tmp9DE,sizeof(char),6)))),Cyc_PP_cat(_tag_dyneither(_tmp9D9,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1802
goto _LL28B;_LL296: {struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp4CE=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4CE->tag != 7)goto _LL298;else{_tmp4CF=_tmp4CE->f1;}}_LL297:
# 1804
{const char*_tmp9E1;struct Cyc_PP_Doc*_tmp9E0[2];s=((_tmp9E0[1]=Cyc_PP_text(((_tmp9E1=";",_tag_dyneither(_tmp9E1,sizeof(char),2)))),((_tmp9E0[0]=Cyc_Absynpp_datatypedecl2doc(_tmp4CF),Cyc_PP_cat(_tag_dyneither(_tmp9E0,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL298: {struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_tmp4D0=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4D0->tag != 2)goto _LL29A;else{_tmp4D1=_tmp4D0->f1;_tmp4D2=_tmp4D0->f3;}}_LL299:
# 1807
{const char*_tmp9E8;const char*_tmp9E7;const char*_tmp9E6;struct Cyc_PP_Doc*_tmp9E5[5];s=((_tmp9E5[4]=
# 1811
Cyc_PP_text(((_tmp9E6=";",_tag_dyneither(_tmp9E6,sizeof(char),2)))),((_tmp9E5[3]=
# 1810
Cyc_Absynpp_exp2doc(_tmp4D2),((_tmp9E5[2]=
# 1809
Cyc_PP_text(((_tmp9E7=" = ",_tag_dyneither(_tmp9E7,sizeof(char),4)))),((_tmp9E5[1]=
# 1808
Cyc_Absynpp_pat2doc(_tmp4D1),((_tmp9E5[0]=
# 1807
Cyc_PP_text(((_tmp9E8="let ",_tag_dyneither(_tmp9E8,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9E5,sizeof(struct Cyc_PP_Doc*),5))))))))))));}
# 1812
goto _LL28B;_LL29A: {struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_tmp4D3=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4D3->tag != 3)goto _LL29C;else{_tmp4D4=_tmp4D3->f1;}}_LL29B:
# 1814
{const char*_tmp9ED;const char*_tmp9EC;struct Cyc_PP_Doc*_tmp9EB[3];s=((_tmp9EB[2]=Cyc_PP_text(((_tmp9EC=";",_tag_dyneither(_tmp9EC,sizeof(char),2)))),((_tmp9EB[1]=Cyc_Absynpp_ids2doc(_tmp4D4),((_tmp9EB[0]=Cyc_PP_text(((_tmp9ED="let ",_tag_dyneither(_tmp9ED,sizeof(char),5)))),Cyc_PP_cat(_tag_dyneither(_tmp9EB,sizeof(struct Cyc_PP_Doc*),3))))))));}
goto _LL28B;_LL29C: {struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp4D5=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4D5->tag != 8)goto _LL29E;else{_tmp4D6=_tmp4D5->f1;}}_LL29D:
# 1817
{const char*_tmp9F0;struct Cyc_PP_Doc*_tmp9EF[2];s=((_tmp9EF[1]=Cyc_PP_text(((_tmp9F0=";",_tag_dyneither(_tmp9F0,sizeof(char),2)))),((_tmp9EF[0]=Cyc_Absynpp_enumdecl2doc(_tmp4D6),Cyc_PP_cat(_tag_dyneither(_tmp9EF,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL29E: {struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp4D7=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4D7->tag != 9)goto _LL2A0;else{_tmp4D8=_tmp4D7->f1;}}_LL29F: {
# 1820
void*t;
if(_tmp4D8->defn != 0)
t=(void*)_check_null(_tmp4D8->defn);else{
# 1824
t=Cyc_Absyn_new_evar(_tmp4D8->kind,0);}
{const char*_tmp9FA;struct Cyc_Core_Opt*_tmp9F9;struct Cyc_PP_Doc*_tmp9F8[2];const char*_tmp9F7;struct Cyc_PP_Doc*_tmp9F6[4];s=((_tmp9F6[3]=
# 1832
Cyc_PP_text(((_tmp9F7=";",_tag_dyneither(_tmp9F7,sizeof(char),2)))),((_tmp9F6[2]=
# 1831
Cyc_Absynpp_atts2doc(_tmp4D8->atts),((_tmp9F6[1]=
# 1826
Cyc_Absynpp_tqtd2doc(_tmp4D8->tq,t,(
# 1828
(_tmp9F9=_cycalloc(sizeof(*_tmp9F9)),((_tmp9F9->v=((_tmp9F8[1]=
Cyc_Absynpp_tvars2doc(_tmp4D8->tvs),((_tmp9F8[0]=
# 1828
Cyc_Absynpp_typedef_name2bolddoc(_tmp4D8->name),Cyc_PP_cat(_tag_dyneither(_tmp9F8,sizeof(struct Cyc_PP_Doc*),2)))))),_tmp9F9))))),((_tmp9F6[0]=
# 1825
Cyc_PP_text(((_tmp9FA="typedef ",_tag_dyneither(_tmp9FA,sizeof(char),9)))),Cyc_PP_cat(_tag_dyneither(_tmp9F6,sizeof(struct Cyc_PP_Doc*),4))))))))));}
# 1834
goto _LL28B;}_LL2A0: {struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp4D9=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4D9->tag != 10)goto _LL2A2;else{_tmp4DA=_tmp4D9->f1;_tmp4DB=_tmp4D9->f2;}}_LL2A1:
# 1836
 if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_add(_tmp4DA);
{const char*_tmp9FF;const char*_tmp9FE;struct Cyc_PP_Doc*_tmp9FD[8];s=((_tmp9FD[7]=
# 1843
Cyc_Absynpp_rb(),((_tmp9FD[6]=
# 1842
Cyc_PP_line_doc(),((_tmp9FD[5]=
# 1841
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmp9FE="",_tag_dyneither(_tmp9FE,sizeof(char),1))),_tmp4DB),((_tmp9FD[4]=
# 1840
Cyc_PP_line_doc(),((_tmp9FD[3]=
# 1839
Cyc_Absynpp_lb(),((_tmp9FD[2]=Cyc_PP_blank_doc(),((_tmp9FD[1]=
# 1838
Cyc_PP_textptr(_tmp4DA),((_tmp9FD[0]=
# 1837
Cyc_PP_text(((_tmp9FF="namespace ",_tag_dyneither(_tmp9FF,sizeof(char),11)))),Cyc_PP_cat(_tag_dyneither(_tmp9FD,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}
# 1844
if(Cyc_Absynpp_use_curr_namespace)Cyc_Absynpp_curr_namespace_drop();
goto _LL28B;_LL2A2: {struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp4DC=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4DC->tag != 11)goto _LL2A4;else{_tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;}}_LL2A3:
# 1847
 if(Cyc_Absynpp_print_using_stmts){
const char*_tmpA04;const char*_tmpA03;struct Cyc_PP_Doc*_tmpA02[8];s=((_tmpA02[7]=
# 1854
Cyc_Absynpp_rb(),((_tmpA02[6]=
# 1853
Cyc_PP_line_doc(),((_tmpA02[5]=
# 1852
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA03="",_tag_dyneither(_tmpA03,sizeof(char),1))),_tmp4DE),((_tmpA02[4]=
# 1851
Cyc_PP_line_doc(),((_tmpA02[3]=
# 1850
Cyc_Absynpp_lb(),((_tmpA02[2]=Cyc_PP_blank_doc(),((_tmpA02[1]=
# 1849
Cyc_Absynpp_qvar2doc(_tmp4DD),((_tmpA02[0]=
# 1848
Cyc_PP_text(((_tmpA04="using ",_tag_dyneither(_tmpA04,sizeof(char),7)))),Cyc_PP_cat(_tag_dyneither(_tmpA02,sizeof(struct Cyc_PP_Doc*),8))))))))))))))))));}else{
# 1856
const char*_tmpA0F;const char*_tmpA0E;const char*_tmpA0D;const char*_tmpA0C;const char*_tmpA0B;struct Cyc_PP_Doc*_tmpA0A[11];s=((_tmpA0A[10]=
# 1866
Cyc_PP_text(((_tmpA0B=" */",_tag_dyneither(_tmpA0B,sizeof(char),4)))),((_tmpA0A[9]=
# 1865
Cyc_Absynpp_rb(),((_tmpA0A[8]=
# 1864
Cyc_PP_text(((_tmpA0C="/* ",_tag_dyneither(_tmpA0C,sizeof(char),4)))),((_tmpA0A[7]=
# 1863
Cyc_PP_line_doc(),((_tmpA0A[6]=
# 1862
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA0D="",_tag_dyneither(_tmpA0D,sizeof(char),1))),_tmp4DE),((_tmpA0A[5]=
# 1861
Cyc_PP_line_doc(),((_tmpA0A[4]=
# 1860
Cyc_PP_text(((_tmpA0E=" */",_tag_dyneither(_tmpA0E,sizeof(char),4)))),((_tmpA0A[3]=
# 1859
Cyc_Absynpp_lb(),((_tmpA0A[2]=
# 1858
Cyc_PP_blank_doc(),((_tmpA0A[1]=
# 1857
Cyc_Absynpp_qvar2doc(_tmp4DD),((_tmpA0A[0]=
# 1856
Cyc_PP_text(((_tmpA0F="/* using ",_tag_dyneither(_tmpA0F,sizeof(char),10)))),Cyc_PP_cat(_tag_dyneither(_tmpA0A,sizeof(struct Cyc_PP_Doc*),11))))))))))))))))))))))));}
# 1867
goto _LL28B;_LL2A4: {struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp4DF=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4DF->tag != 12)goto _LL2A6;else{_tmp4E0=_tmp4DF->f1;}}_LL2A5:
# 1869
 if(Cyc_Absynpp_print_externC_stmts){
const char*_tmpA14;const char*_tmpA13;struct Cyc_PP_Doc*_tmpA12[6];s=((_tmpA12[5]=
# 1875
Cyc_Absynpp_rb(),((_tmpA12[4]=
# 1874
Cyc_PP_line_doc(),((_tmpA12[3]=
# 1873
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA13="",_tag_dyneither(_tmpA13,sizeof(char),1))),_tmp4E0),((_tmpA12[2]=
# 1872
Cyc_PP_line_doc(),((_tmpA12[1]=
# 1871
Cyc_Absynpp_lb(),((_tmpA12[0]=
# 1870
Cyc_PP_text(((_tmpA14="extern \"C\" ",_tag_dyneither(_tmpA14,sizeof(char),12)))),Cyc_PP_cat(_tag_dyneither(_tmpA12,sizeof(struct Cyc_PP_Doc*),6))))))))))))));}else{
# 1877
const char*_tmpA1F;const char*_tmpA1E;const char*_tmpA1D;const char*_tmpA1C;const char*_tmpA1B;struct Cyc_PP_Doc*_tmpA1A[9];s=((_tmpA1A[8]=
# 1885
Cyc_PP_text(((_tmpA1B=" */",_tag_dyneither(_tmpA1B,sizeof(char),4)))),((_tmpA1A[7]=
# 1884
Cyc_Absynpp_rb(),((_tmpA1A[6]=
# 1883
Cyc_PP_text(((_tmpA1C="/* ",_tag_dyneither(_tmpA1C,sizeof(char),4)))),((_tmpA1A[5]=
# 1882
Cyc_PP_line_doc(),((_tmpA1A[4]=
# 1881
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA1D="",_tag_dyneither(_tmpA1D,sizeof(char),1))),_tmp4E0),((_tmpA1A[3]=
# 1880
Cyc_PP_line_doc(),((_tmpA1A[2]=
# 1879
Cyc_PP_text(((_tmpA1E=" */",_tag_dyneither(_tmpA1E,sizeof(char),4)))),((_tmpA1A[1]=
# 1878
Cyc_Absynpp_lb(),((_tmpA1A[0]=
# 1877
Cyc_PP_text(((_tmpA1F="/* extern \"C\" ",_tag_dyneither(_tmpA1F,sizeof(char),15)))),Cyc_PP_cat(_tag_dyneither(_tmpA1A,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1886
goto _LL28B;_LL2A6: {struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp4E1=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4E1->tag != 13)goto _LL2A8;else{_tmp4E2=_tmp4E1->f1;_tmp4E3=_tmp4E1->f2;}}_LL2A7:
# 1888
 if(Cyc_Absynpp_print_externC_stmts){
struct Cyc_PP_Doc*exs_doc;
if(_tmp4E3 != 0){
const char*_tmpA24;const char*_tmpA23;struct Cyc_PP_Doc*_tmpA22[7];exs_doc=((_tmpA22[6]=
# 1893
Cyc_Absynpp_rb(),((_tmpA22[5]=Cyc_PP_line_doc(),((_tmpA22[4]=
# 1892
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct _tuple17*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_export2doc,((_tmpA23=",",_tag_dyneither(_tmpA23,sizeof(char),2))),_tmp4E3),((_tmpA22[3]=Cyc_PP_line_doc(),((_tmpA22[2]=
# 1891
Cyc_Absynpp_lb(),((_tmpA22[1]=Cyc_PP_text(((_tmpA24=" export ",_tag_dyneither(_tmpA24,sizeof(char),9)))),((_tmpA22[0]=Cyc_Absynpp_rb(),Cyc_PP_cat(_tag_dyneither(_tmpA22,sizeof(struct Cyc_PP_Doc*),7))))))))))))))));}else{
# 1895
exs_doc=Cyc_Absynpp_rb();}{
const char*_tmpA29;const char*_tmpA28;struct Cyc_PP_Doc*_tmpA27[6];s=((_tmpA27[5]=exs_doc,((_tmpA27[4]=
# 1900
Cyc_PP_line_doc(),((_tmpA27[3]=
# 1899
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA28="",_tag_dyneither(_tmpA28,sizeof(char),1))),_tmp4E2),((_tmpA27[2]=
# 1898
Cyc_PP_line_doc(),((_tmpA27[1]=
# 1897
Cyc_Absynpp_lb(),((_tmpA27[0]=
# 1896
Cyc_PP_text(((_tmpA29="extern \"C include\" ",_tag_dyneither(_tmpA29,sizeof(char),20)))),Cyc_PP_cat(_tag_dyneither(_tmpA27,sizeof(struct Cyc_PP_Doc*),6))))))))))))));};}else{
# 1903
const char*_tmpA34;const char*_tmpA33;const char*_tmpA32;const char*_tmpA31;const char*_tmpA30;struct Cyc_PP_Doc*_tmpA2F[9];s=((_tmpA2F[8]=
# 1911
Cyc_PP_text(((_tmpA30=" */",_tag_dyneither(_tmpA30,sizeof(char),4)))),((_tmpA2F[7]=
# 1910
Cyc_Absynpp_rb(),((_tmpA2F[6]=
# 1909
Cyc_PP_text(((_tmpA31="/* ",_tag_dyneither(_tmpA31,sizeof(char),4)))),((_tmpA2F[5]=
# 1908
Cyc_PP_line_doc(),((_tmpA2F[4]=
# 1907
((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Decl*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_decl2doc,((_tmpA32="",_tag_dyneither(_tmpA32,sizeof(char),1))),_tmp4E2),((_tmpA2F[3]=
# 1906
Cyc_PP_line_doc(),((_tmpA2F[2]=
# 1905
Cyc_PP_text(((_tmpA33=" */",_tag_dyneither(_tmpA33,sizeof(char),4)))),((_tmpA2F[1]=
# 1904
Cyc_Absynpp_lb(),((_tmpA2F[0]=
# 1903
Cyc_PP_text(((_tmpA34="/* extern \"C include\" ",_tag_dyneither(_tmpA34,sizeof(char),23)))),Cyc_PP_cat(_tag_dyneither(_tmpA2F,sizeof(struct Cyc_PP_Doc*),9))))))))))))))))))));}
# 1912
goto _LL28B;_LL2A8: {struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*_tmp4E4=(struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4E4->tag != 14)goto _LL2AA;}_LL2A9:
# 1914
{const char*_tmpA37;struct Cyc_PP_Doc*_tmpA36[2];s=((_tmpA36[1]=Cyc_Absynpp_lb(),((_tmpA36[0]=Cyc_PP_text(((_tmpA37="__cyclone_port_on__;",_tag_dyneither(_tmpA37,sizeof(char),21)))),Cyc_PP_cat(_tag_dyneither(_tmpA36,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL2AA: {struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*_tmp4E5=(struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct*)_tmp4BE;if(_tmp4E5->tag != 15)goto _LL28B;}_LL2AB:
# 1917
{const char*_tmpA3A;struct Cyc_PP_Doc*_tmpA39[2];s=((_tmpA39[1]=Cyc_Absynpp_lb(),((_tmpA39[0]=Cyc_PP_text(((_tmpA3A="__cyclone_port_off__;",_tag_dyneither(_tmpA3A,sizeof(char),22)))),Cyc_PP_cat(_tag_dyneither(_tmpA39,sizeof(struct Cyc_PP_Doc*),2))))));}
goto _LL28B;_LL28B:;}
# 1921
return s;}
# 1924
struct Cyc_PP_Doc*Cyc_Absynpp_scope2doc(enum Cyc_Absyn_Scope sc){
if(Cyc_Absynpp_print_for_cycdoc)return Cyc_PP_nil_doc();
switch(sc){case Cyc_Absyn_Static: _LL2B5: {
const char*_tmpA3B;return Cyc_PP_text(((_tmpA3B="static ",_tag_dyneither(_tmpA3B,sizeof(char),8))));}case Cyc_Absyn_Public: _LL2B6:
 return Cyc_PP_nil_doc();case Cyc_Absyn_Extern: _LL2B7: {
const char*_tmpA3C;return Cyc_PP_text(((_tmpA3C="extern ",_tag_dyneither(_tmpA3C,sizeof(char),8))));}case Cyc_Absyn_ExternC: _LL2B8: {
const char*_tmpA3D;return Cyc_PP_text(((_tmpA3D="extern \"C\" ",_tag_dyneither(_tmpA3D,sizeof(char),12))));}case Cyc_Absyn_Abstract: _LL2B9: {
const char*_tmpA3E;return Cyc_PP_text(((_tmpA3E="abstract ",_tag_dyneither(_tmpA3E,sizeof(char),10))));}case Cyc_Absyn_Register: _LL2BA: {
const char*_tmpA3F;return Cyc_PP_text(((_tmpA3F="register ",_tag_dyneither(_tmpA3F,sizeof(char),10))));}}}
# 1937
int Cyc_Absynpp_exists_temp_tvar_in_effect(void*t){
void*_tmp53C=t;struct Cyc_Absyn_Tvar*_tmp53E;struct Cyc_List_List*_tmp540;_LL2BD: {struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp53D=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53C;if(_tmp53D->tag != 2)goto _LL2BF;else{_tmp53E=_tmp53D->f1;}}_LL2BE:
 return Cyc_Tcutil_is_temp_tvar(_tmp53E);_LL2BF: {struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp53F=(struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp53C;if(_tmp53F->tag != 24)goto _LL2C1;else{_tmp540=_tmp53F->f1;}}_LL2C0:
 return((int(*)(int(*pred)(void*),struct Cyc_List_List*x))Cyc_List_exists)(Cyc_Absynpp_exists_temp_tvar_in_effect,_tmp540);_LL2C1:;_LL2C2:
 return 0;_LL2BC:;}
# 1949
int Cyc_Absynpp_is_anon_aggrtype(void*t){
void*_tmp541=t;struct Cyc_Absyn_Typedefdecl*_tmp545;void*_tmp546;_LL2C4: {struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp542=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp541;if(_tmp542->tag != 12)goto _LL2C6;}_LL2C5:
 return 1;_LL2C6: {struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp543=(struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp541;if(_tmp543->tag != 14)goto _LL2C8;}_LL2C7:
 return 1;_LL2C8:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp544=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp541;if(_tmp544->tag != 17)goto _LL2CA;else{_tmp545=_tmp544->f3;_tmp546=(void*)_tmp544->f4;}}if(!(_tmp546 != 0))goto _LL2CA;_LL2C9:
# 1956
 return Cyc_Absynpp_is_anon_aggrtype((void*)_tmp546);_LL2CA:;_LL2CB:
 return 0;_LL2C3:;}
# 1963
static struct Cyc_List_List*Cyc_Absynpp_bubble_attributes(struct _RegionHandle*r,void*atts,struct Cyc_List_List*tms){
# 1966
if(tms != 0  && tms->tl != 0){
struct _tuple16 _tmpA40;struct _tuple16 _tmp548=(_tmpA40.f1=(void*)tms->hd,((_tmpA40.f2=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,_tmpA40)));_LL2CD:{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp549=(struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp548.f1;if(_tmp549->tag != 2)goto _LL2CF;}{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp54A=(struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp548.f2;if(_tmp54A->tag != 3)goto _LL2CF;};_LL2CE: {
# 1969
struct Cyc_List_List*_tmpA43;struct Cyc_List_List*_tmpA42;return(_tmpA42=_region_malloc(r,sizeof(*_tmpA42)),((_tmpA42->hd=(void*)tms->hd,((_tmpA42->tl=((_tmpA43=_region_malloc(r,sizeof(*_tmpA43)),((_tmpA43->hd=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd,((_tmpA43->tl=Cyc_Absynpp_bubble_attributes(r,atts,((struct Cyc_List_List*)_check_null(tms->tl))->tl),_tmpA43)))))),_tmpA42)))));}_LL2CF:;_LL2D0: {
struct Cyc_List_List*_tmpA44;return(_tmpA44=_region_malloc(r,sizeof(*_tmpA44)),((_tmpA44->hd=atts,((_tmpA44->tl=tms,_tmpA44)))));}_LL2CC:;}else{
# 1972
struct Cyc_List_List*_tmpA45;return(_tmpA45=_region_malloc(r,sizeof(*_tmpA45)),((_tmpA45->hd=atts,((_tmpA45->tl=tms,_tmpA45)))));}}
# 1977
struct _tuple12 Cyc_Absynpp_to_tms(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t){
# 1979
void*_tmp54F=t;void*_tmp551;struct Cyc_Absyn_Tqual _tmp552;struct Cyc_Absyn_Exp*_tmp553;union Cyc_Absyn_Constraint*_tmp554;unsigned int _tmp555;void*_tmp557;struct Cyc_Absyn_Tqual _tmp558;struct Cyc_Absyn_PtrAtts _tmp559;struct Cyc_List_List*_tmp55B;void*_tmp55C;struct Cyc_Absyn_Tqual _tmp55D;void*_tmp55E;struct Cyc_List_List*_tmp55F;int _tmp560;struct Cyc_Absyn_VarargInfo*_tmp561;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp563;struct Cyc_Core_Opt*_tmp565;void*_tmp566;int _tmp567;struct _tuple0*_tmp569;struct Cyc_List_List*_tmp56A;struct Cyc_Absyn_Typedefdecl*_tmp56B;void*_tmp56C;_LL2D2: {struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp550=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp54F;if(_tmp550->tag != 8)goto _LL2D4;else{_tmp551=(_tmp550->f1).elt_type;_tmp552=(_tmp550->f1).tq;_tmp553=(_tmp550->f1).num_elts;_tmp554=(_tmp550->f1).zero_term;_tmp555=(_tmp550->f1).zt_loc;}}_LL2D3: {
# 1982
struct Cyc_Absyn_Tqual _tmp56E;void*_tmp56F;struct Cyc_List_List*_tmp570;struct _tuple12 _tmp56D=Cyc_Absynpp_to_tms(r,_tmp552,_tmp551);_tmp56E=_tmp56D.f1;_tmp56F=_tmp56D.f2;_tmp570=_tmp56D.f3;{
void*tm;
if(_tmp553 == 0){
struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct _tmpA48;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmpA47;tm=(void*)((_tmpA47=_region_malloc(r,sizeof(*_tmpA47)),((_tmpA47[0]=((_tmpA48.tag=0,((_tmpA48.f1=_tmp554,((_tmpA48.f2=_tmp555,_tmpA48)))))),_tmpA47))));}else{
# 1987
struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct _tmpA4B;struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmpA4A;tm=(void*)((_tmpA4A=_region_malloc(r,sizeof(*_tmpA4A)),((_tmpA4A[0]=((_tmpA4B.tag=1,((_tmpA4B.f1=(struct Cyc_Absyn_Exp*)_tmp553,((_tmpA4B.f2=_tmp554,((_tmpA4B.f3=_tmp555,_tmpA4B)))))))),_tmpA4A))));}{
struct Cyc_List_List*_tmpA4E;struct _tuple12 _tmpA4D;return(_tmpA4D.f1=_tmp56E,((_tmpA4D.f2=_tmp56F,((_tmpA4D.f3=((_tmpA4E=_region_malloc(r,sizeof(*_tmpA4E)),((_tmpA4E->hd=tm,((_tmpA4E->tl=_tmp570,_tmpA4E)))))),_tmpA4D)))));};};}_LL2D4: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp556=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54F;if(_tmp556->tag != 5)goto _LL2D6;else{_tmp557=(_tmp556->f1).elt_typ;_tmp558=(_tmp556->f1).elt_tq;_tmp559=(_tmp556->f1).ptr_atts;}}_LL2D5: {
# 1991
struct Cyc_Absyn_Tqual _tmp578;void*_tmp579;struct Cyc_List_List*_tmp57A;struct _tuple12 _tmp577=Cyc_Absynpp_to_tms(r,_tmp558,_tmp557);_tmp578=_tmp577.f1;_tmp579=_tmp577.f2;_tmp57A=_tmp577.f3;
{struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmpA54;struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct _tmpA53;struct Cyc_List_List*_tmpA52;_tmp57A=((_tmpA52=_region_malloc(r,sizeof(*_tmpA52)),((_tmpA52->hd=(void*)((_tmpA54=_region_malloc(r,sizeof(*_tmpA54)),((_tmpA54[0]=((_tmpA53.tag=2,((_tmpA53.f1=_tmp559,((_tmpA53.f2=tq,_tmpA53)))))),_tmpA54)))),((_tmpA52->tl=_tmp57A,_tmpA52))))));}{
struct _tuple12 _tmpA55;return(_tmpA55.f1=_tmp578,((_tmpA55.f2=_tmp579,((_tmpA55.f3=_tmp57A,_tmpA55)))));};}_LL2D6: {struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55A=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54F;if(_tmp55A->tag != 9)goto _LL2D8;else{_tmp55B=(_tmp55A->f1).tvars;_tmp55C=(_tmp55A->f1).effect;_tmp55D=(_tmp55A->f1).ret_tqual;_tmp55E=(_tmp55A->f1).ret_typ;_tmp55F=(_tmp55A->f1).args;_tmp560=(_tmp55A->f1).c_varargs;_tmp561=(_tmp55A->f1).cyc_varargs;_tmp562=(_tmp55A->f1).rgn_po;_tmp563=(_tmp55A->f1).attributes;}}_LL2D7:
# 1997
 if(!Cyc_Absynpp_print_all_tvars){
# 2001
if(_tmp55C == 0  || Cyc_Absynpp_exists_temp_tvar_in_effect((void*)_tmp55C)){
_tmp55C=0;
_tmp55B=0;}}else{
# 2006
if(Cyc_Absynpp_rewrite_temp_tvars)
# 2009
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_rewrite_temp_tvar,_tmp55B);}{
# 2014
struct Cyc_Absyn_Tqual _tmp580;void*_tmp581;struct Cyc_List_List*_tmp582;struct _tuple12 _tmp57F=Cyc_Absynpp_to_tms(r,_tmp55D,_tmp55E);_tmp580=_tmp57F.f1;_tmp581=_tmp57F.f2;_tmp582=_tmp57F.f3;{
struct Cyc_List_List*tms=_tmp582;
# 2025 "absynpp.cyc"
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2DE:
# 2027
 if(_tmp563 != 0){
# 2030
struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA58;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA57;tms=Cyc_Absynpp_bubble_attributes(r,(void*)((_tmpA57=_region_malloc(r,sizeof(*_tmpA57)),((_tmpA57[0]=((_tmpA58.tag=5,((_tmpA58.f1=0,((_tmpA58.f2=_tmp563,_tmpA58)))))),_tmpA57)))),tms);}
# 2032
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA67;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA66;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA65;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA64;struct Cyc_List_List*_tmpA63;tms=((_tmpA63=_region_malloc(r,sizeof(*_tmpA63)),((_tmpA63->hd=(void*)((_tmpA67=_region_malloc(r,sizeof(*_tmpA67)),((_tmpA67[0]=(
(_tmpA64.tag=3,((_tmpA64.f1=(void*)((void*)((_tmpA66=_region_malloc(r,sizeof(*_tmpA66)),((_tmpA66[0]=((_tmpA65.tag=1,((_tmpA65.f1=_tmp55F,((_tmpA65.f2=_tmp560,((_tmpA65.f3=_tmp561,((_tmpA65.f4=(void*)_tmp55C,((_tmpA65.f5=_tmp562,_tmpA65)))))))))))),_tmpA66))))),_tmpA64)))),_tmpA67)))),((_tmpA63->tl=tms,_tmpA63))))));}
# 2036
break;case Cyc_Cyclone_Vc_c: _LL2DF:
# 2038
{struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmpA76;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmpA75;struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct _tmpA74;struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct _tmpA73;struct Cyc_List_List*_tmpA72;tms=((_tmpA72=_region_malloc(r,sizeof(*_tmpA72)),((_tmpA72->hd=(void*)((_tmpA76=_region_malloc(r,sizeof(*_tmpA76)),((_tmpA76[0]=(
(_tmpA73.tag=3,((_tmpA73.f1=(void*)((void*)((_tmpA75=_region_malloc(r,sizeof(*_tmpA75)),((_tmpA75[0]=((_tmpA74.tag=1,((_tmpA74.f1=_tmp55F,((_tmpA74.f2=_tmp560,((_tmpA74.f3=_tmp561,((_tmpA74.f4=(void*)_tmp55C,((_tmpA74.f5=_tmp562,_tmpA74)))))))))))),_tmpA75))))),_tmpA73)))),_tmpA76)))),((_tmpA72->tl=tms,_tmpA72))))));}
# 2042
for(0;_tmp563 != 0;_tmp563=_tmp563->tl){
void*_tmp58F=(void*)_tmp563->hd;_LL2E2: {struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*_tmp590=(struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp590->tag != 1)goto _LL2E4;}_LL2E3:
 goto _LL2E5;_LL2E4: {struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*_tmp591=(struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp591->tag != 2)goto _LL2E6;}_LL2E5:
 goto _LL2E7;_LL2E6: {struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*_tmp592=(struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct*)_tmp58F;if(_tmp592->tag != 3)goto _LL2E8;}_LL2E7:
# 2047
{struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmpA80;struct Cyc_List_List*_tmpA7F;struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct _tmpA7E;struct Cyc_List_List*_tmpA7D;tms=((_tmpA7D=_region_malloc(r,sizeof(*_tmpA7D)),((_tmpA7D->hd=(void*)((_tmpA80=_region_malloc(r,sizeof(*_tmpA80)),((_tmpA80[0]=((_tmpA7E.tag=5,((_tmpA7E.f1=0,((_tmpA7E.f2=((_tmpA7F=_cycalloc(sizeof(*_tmpA7F)),((_tmpA7F->hd=(void*)_tmp563->hd,((_tmpA7F->tl=0,_tmpA7F)))))),_tmpA7E)))))),_tmpA80)))),((_tmpA7D->tl=tms,_tmpA7D))))));}
goto AfterAtts;_LL2E8:;_LL2E9:
 goto _LL2E1;_LL2E1:;}
# 2051
break;}
# 2054
AfterAtts:
 if(_tmp55B != 0){
struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmpA86;struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct _tmpA85;struct Cyc_List_List*_tmpA84;tms=((_tmpA84=_region_malloc(r,sizeof(*_tmpA84)),((_tmpA84->hd=(void*)((_tmpA86=_region_malloc(r,sizeof(*_tmpA86)),((_tmpA86[0]=((_tmpA85.tag=4,((_tmpA85.f1=_tmp55B,((_tmpA85.f2=0,((_tmpA85.f3=1,_tmpA85)))))))),_tmpA86)))),((_tmpA84->tl=tms,_tmpA84))))));}{
struct _tuple12 _tmpA87;return(_tmpA87.f1=_tmp580,((_tmpA87.f2=_tmp581,((_tmpA87.f3=tms,_tmpA87)))));};};};_LL2D8: {struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp564=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp54F;if(_tmp564->tag != 1)goto _LL2DA;else{_tmp565=_tmp564->f1;_tmp566=(void*)_tmp564->f2;_tmp567=_tmp564->f3;}}_LL2D9:
# 2060
 if(_tmp566 == 0){
struct _tuple12 _tmpA88;return(_tmpA88.f1=tq,((_tmpA88.f2=t,((_tmpA88.f3=0,_tmpA88)))));}else{
# 2063
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp566);}_LL2DA: {struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp568=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp54F;if(_tmp568->tag != 17)goto _LL2DC;else{_tmp569=_tmp568->f1;_tmp56A=_tmp568->f2;_tmp56B=_tmp568->f3;_tmp56C=(void*)_tmp568->f4;}}_LL2DB:
# 2068
 if((_tmp56C == 0  || !Cyc_Absynpp_expand_typedefs) || Cyc_Absynpp_is_anon_aggrtype((void*)_tmp56C)){
struct _tuple12 _tmpA89;return(_tmpA89.f1=tq,((_tmpA89.f2=t,((_tmpA89.f3=0,_tmpA89)))));}else{
# 2071
if(tq.real_const)
tq.print_const=tq.real_const;
return Cyc_Absynpp_to_tms(r,tq,(void*)_tmp56C);}_LL2DC:;_LL2DD: {
# 2076
struct _tuple12 _tmpA8A;return(_tmpA8A.f1=tq,((_tmpA8A.f2=t,((_tmpA8A.f3=0,_tmpA8A)))));}_LL2D1:;}
# 2080
static int Cyc_Absynpp_is_char_ptr(void*t){
# 2082
void*_tmp59E=t;void*_tmp5A0;void*_tmp5A2;_LL2EB:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp59F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp59E;if(_tmp59F->tag != 1)goto _LL2ED;else{_tmp5A0=(void*)_tmp59F->f2;}}if(!(_tmp5A0 != 0))goto _LL2ED;_LL2EC:
 return Cyc_Absynpp_is_char_ptr((void*)_tmp5A0);_LL2ED: {struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A1=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59E;if(_tmp5A1->tag != 5)goto _LL2EF;else{_tmp5A2=(_tmp5A1->f1).elt_typ;}}_LL2EE:
# 2085
 L: {
void*_tmp5A3=_tmp5A2;void*_tmp5A5;void*_tmp5A7;_LL2F2:{struct Cyc_Absyn_Evar_Absyn_Type_struct*_tmp5A4=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp5A3;if(_tmp5A4->tag != 1)goto _LL2F4;else{_tmp5A5=(void*)_tmp5A4->f2;}}if(!(_tmp5A5 != 0))goto _LL2F4;_LL2F3:
 _tmp5A2=(void*)_tmp5A5;goto L;_LL2F4:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp5A6=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5A3;if(_tmp5A6->tag != 17)goto _LL2F6;else{_tmp5A7=(void*)_tmp5A6->f4;}}if(!(_tmp5A7 != 0))goto _LL2F6;_LL2F5:
 _tmp5A2=(void*)_tmp5A7;goto L;_LL2F6: {struct Cyc_Absyn_IntType_Absyn_Type_struct*_tmp5A8=(struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5A3;if(_tmp5A8->tag != 6)goto _LL2F8;else{if(_tmp5A8->f2 != Cyc_Absyn_Char_sz)goto _LL2F8;}}_LL2F7:
 return 1;_LL2F8:;_LL2F9:
 return 0;_LL2F1:;}_LL2EF:;_LL2F0:
# 2092
 return 0;_LL2EA:;}
# 2096
struct Cyc_PP_Doc*Cyc_Absynpp_tqtd2doc(struct Cyc_Absyn_Tqual tq,void*typ,struct Cyc_Core_Opt*dopt){
struct _RegionHandle _tmp5A9=_new_region("temp");struct _RegionHandle*temp=& _tmp5A9;_push_region(temp);
{struct Cyc_Absyn_Tqual _tmp5AB;void*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct _tuple12 _tmp5AA=Cyc_Absynpp_to_tms(temp,tq,typ);_tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;_tmp5AD=_tmp5AA.f3;
_tmp5AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp5AD);
if(_tmp5AD == 0  && dopt == 0){
struct Cyc_PP_Doc*_tmpA8B[2];struct Cyc_PP_Doc*_tmp5AF=(_tmpA8B[1]=Cyc_Absynpp_ntyp2doc(_tmp5AC),((_tmpA8B[0]=Cyc_Absynpp_tqual2doc(_tmp5AB),Cyc_PP_cat(_tag_dyneither(_tmpA8B,sizeof(struct Cyc_PP_Doc*),2)))));_npop_handler(0);return _tmp5AF;}else{
# 2103
const char*_tmpA8E;struct Cyc_PP_Doc*_tmpA8D[4];struct Cyc_PP_Doc*_tmp5B2=
(_tmpA8D[3]=
# 2107
Cyc_Absynpp_dtms2doc(Cyc_Absynpp_is_char_ptr(typ),dopt == 0?Cyc_PP_nil_doc():(struct Cyc_PP_Doc*)dopt->v,_tmp5AD),((_tmpA8D[2]=
# 2106
Cyc_PP_text(((_tmpA8E=" ",_tag_dyneither(_tmpA8E,sizeof(char),2)))),((_tmpA8D[1]=
# 2105
Cyc_Absynpp_ntyp2doc(_tmp5AC),((_tmpA8D[0]=
# 2104
Cyc_Absynpp_tqual2doc(_tmp5AB),Cyc_PP_cat(_tag_dyneither(_tmpA8D,sizeof(struct Cyc_PP_Doc*),4)))))))));_npop_handler(0);return _tmp5B2;}}
# 2098
;_pop_region(temp);}
# 2111
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfield2doc(struct Cyc_Absyn_Aggrfield*f){
switch(Cyc_Cyclone_c_compiler){case Cyc_Cyclone_Gcc_c: _LL2FA:
# 2114
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpA95;const char*_tmpA94;const char*_tmpA93;struct Cyc_PP_Doc*_tmpA92[5];return(_tmpA92[4]=
# 2117
Cyc_PP_text(((_tmpA93=";",_tag_dyneither(_tmpA93,sizeof(char),2)))),((_tmpA92[3]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA92[2]=
# 2116
Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA92[1]=Cyc_PP_text(((_tmpA94=":",_tag_dyneither(_tmpA94,sizeof(char),2)))),((_tmpA92[0]=
# 2115
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA95=_cycalloc(sizeof(*_tmpA95)),((_tmpA95->v=Cyc_PP_textptr(f->name),_tmpA95))))),Cyc_PP_cat(_tag_dyneither(_tmpA92,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2119
struct Cyc_Core_Opt*_tmpA9A;const char*_tmpA99;struct Cyc_PP_Doc*_tmpA98[3];return(_tmpA98[2]=
Cyc_PP_text(((_tmpA99=";",_tag_dyneither(_tmpA99,sizeof(char),2)))),((_tmpA98[1]=Cyc_Absynpp_atts2doc(f->attributes),((_tmpA98[0]=
# 2119
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpA9A=_cycalloc(sizeof(*_tmpA9A)),((_tmpA9A->v=Cyc_PP_textptr(f->name),_tmpA9A))))),Cyc_PP_cat(_tag_dyneither(_tmpA98,sizeof(struct Cyc_PP_Doc*),3)))))));}case Cyc_Cyclone_Vc_c: _LL2FB:
# 2122
 if(f->width != 0){
struct Cyc_Core_Opt*_tmpAA1;const char*_tmpAA0;const char*_tmpA9F;struct Cyc_PP_Doc*_tmpA9E[5];return(_tmpA9E[4]=
# 2125
Cyc_PP_text(((_tmpA9F=";",_tag_dyneither(_tmpA9F,sizeof(char),2)))),((_tmpA9E[3]=Cyc_Absynpp_exp2doc((struct Cyc_Absyn_Exp*)_check_null(f->width)),((_tmpA9E[2]=Cyc_PP_text(((_tmpAA0=":",_tag_dyneither(_tmpAA0,sizeof(char),2)))),((_tmpA9E[1]=
# 2124
Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA1=_cycalloc(sizeof(*_tmpAA1)),((_tmpAA1->v=Cyc_PP_textptr(f->name),_tmpAA1))))),((_tmpA9E[0]=
# 2123
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpA9E,sizeof(struct Cyc_PP_Doc*),5)))))))))));}else{
# 2127
struct Cyc_Core_Opt*_tmpAA6;const char*_tmpAA5;struct Cyc_PP_Doc*_tmpAA4[3];return(_tmpAA4[2]=
Cyc_PP_text(((_tmpAA5=";",_tag_dyneither(_tmpAA5,sizeof(char),2)))),((_tmpAA4[1]=Cyc_Absynpp_tqtd2doc(f->tq,f->type,((_tmpAA6=_cycalloc(sizeof(*_tmpAA6)),((_tmpAA6->v=Cyc_PP_textptr(f->name),_tmpAA6))))),((_tmpAA4[0]=
# 2127
Cyc_Absynpp_atts2doc(f->attributes),Cyc_PP_cat(_tag_dyneither(_tmpAA4,sizeof(struct Cyc_PP_Doc*),3)))))));}}}
# 2133
struct Cyc_PP_Doc*Cyc_Absynpp_aggrfields2doc(struct Cyc_List_List*fields){
const char*_tmpAA7;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_aggrfield2doc,((_tmpAA7="",_tag_dyneither(_tmpAA7,sizeof(char),1))),fields);}
# 2137
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefield2doc(struct Cyc_Absyn_Datatypefield*f){
struct Cyc_PP_Doc*_tmpAA8[3];return(_tmpAA8[2]=
f->typs == 0?Cyc_PP_nil_doc(): Cyc_Absynpp_args2doc(f->typs),((_tmpAA8[1]=
# 2138
Cyc_Absynpp_typedef_name2doc(f->name),((_tmpAA8[0]=Cyc_Absynpp_scope2doc(f->sc),Cyc_PP_cat(_tag_dyneither(_tmpAA8,sizeof(struct Cyc_PP_Doc*),3)))))));}
# 2142
struct Cyc_PP_Doc*Cyc_Absynpp_datatypefields2doc(struct Cyc_List_List*fields){
const char*_tmpAA9;return((struct Cyc_PP_Doc*(*)(struct Cyc_PP_Doc*(*pp)(struct Cyc_Absyn_Datatypefield*),struct _dyneither_ptr sep,struct Cyc_List_List*l))Cyc_PP_ppseql)(Cyc_Absynpp_datatypefield2doc,((_tmpAA9=",",_tag_dyneither(_tmpAA9,sizeof(char),2))),fields);}
# 2151
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
for(0;tdl != 0;tdl=tdl->tl){
Cyc_PP_file_of_doc(Cyc_Absynpp_decl2doc((struct Cyc_Absyn_Decl*)tdl->hd),72,f);{
const char*_tmpAAC;void*_tmpAAB;(_tmpAAB=0,Cyc_fprintf(f,((_tmpAAC="\n",_tag_dyneither(_tmpAAC,sizeof(char),2))),_tag_dyneither(_tmpAAB,sizeof(void*),0)));};}}
# 2158
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl){
const char*_tmpAAD;return Cyc_PP_string_of_doc(Cyc_PP_seql(((_tmpAAD="",_tag_dyneither(_tmpAAD,sizeof(char),1))),((struct Cyc_List_List*(*)(struct Cyc_PP_Doc*(*f)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_decl2doc,tdl)),72);}
# 2161
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*e){return Cyc_PP_string_of_doc(Cyc_Absynpp_exp2doc(e),72);}
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*s){return Cyc_PP_string_of_doc(Cyc_Absynpp_stmt2doc(s,0),72);}
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*t){return Cyc_PP_string_of_doc(Cyc_Absynpp_typ2doc(t),72);}
# 2165
struct _dyneither_ptr Cyc_Absynpp_typ2cstring(void*t){
int old_qvar_to_Cids=Cyc_Absynpp_qvar_to_Cids;
int old_add_cyc_prefix=Cyc_Absynpp_add_cyc_prefix;
Cyc_Absynpp_qvar_to_Cids=1;
Cyc_Absynpp_add_cyc_prefix=0;{
struct _dyneither_ptr s=Cyc_Absynpp_typ2string(t);
Cyc_Absynpp_qvar_to_Cids=old_qvar_to_Cids;
Cyc_Absynpp_add_cyc_prefix=old_add_cyc_prefix;
return s;};}
# 2175
struct _dyneither_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop p){return Cyc_PP_string_of_doc(Cyc_Absynpp_prim2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p){return Cyc_PP_string_of_doc(Cyc_Absynpp_pat2doc(p),72);}
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc){return Cyc_PP_string_of_doc(Cyc_Absynpp_scope2doc(sc),72);}
